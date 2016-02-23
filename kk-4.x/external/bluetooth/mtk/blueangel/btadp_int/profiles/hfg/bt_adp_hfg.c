/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#include "stdio.h"      /* Basic C I/O functions */
#include "hfg.h"
#ifdef BT_HFG_UT_TEST
#include "hfgi.h"
#endif
#include "bt_adp_hfg.h"
#include "bluetooth_hfg_message.h"

ListEntry g_channel_list;

#ifdef __BT_A2DP_PROFILE__
void a2dp_set_sco_state(U8 state);
#endif
/***************************
Internal functions
****************************/
void initialize_rsp_queue(HfgAdpChannel *adp_channel)
{
    U8 i;
    InitializeListHead(&adp_channel->rsp_queue);
    for(i = 0;i < MAX_RESPONSE_QUEUE;i++)
        InsertTailList(&adp_channel->rsp_queue, &adp_channel->rsp_list[i].response.node);
}

HfgResponse* hfg_alloc_rsp(HfgAdpChannel *adp_channel)
{
    HfgResponse *rsp = NULL;
    if (!IsListEmpty(&adp_channel->rsp_queue)) 
    {
        rsp = (HfgResponse *)RemoveHeadList(&adp_channel->rsp_queue); 
        OS_MemSet((U8*)rsp, 0, sizeof(HfgResponse));
    }
    else
    {
        printf("Runout of response queue\r\n");
    }
    return rsp;
}

void hfg_free_rsp(HfgAdpChannel *adp_channel, HfgResponse* rsp)
{
    if (!IsNodeOnList(&adp_channel->rsp_queue, &rsp->node)) 
    {
        InsertTailList(&adp_channel->rsp_queue, &rsp->node);
    }
}

void hfg_sendMsg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{
    if (dstMod == 0)
    {
        bt_prompt_trace(MOD_BT, "[ERR] hfg_sendMsg : dstMod == NULL");
        return;
    }

    if (local_para != NULL)
    {
        BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
    }
    else
    {
        BT_SendMessage(msg, dstMod, NULL, 0);
    }
}

void hfg_send_channel_msg(HfgAdpChannel *adp_channel, 
                                      msg_type msg,
                                      void *para)
{
    hfg_sendMsg(msg, adp_channel->registered_module, BT_APP_SAP, para, NULL);
}

char *allocStringBuf(const char *str, U16 len)
{
    char *ret = NULL;
    if(str && len > 0)
    {
        ret = (char*)get_ctrl_buffer(len+1);
        if(ret)
        {
            OS_MemCopy((U8*)ret, (const U8*)str, len);
        }
        ret[len] = 0;
    }
    return ret;
}

/***************
External Globals 
****************/
BOOL hfg_adp_init(void)
{
    InitializeListHead(&g_channel_list);
    return hfg_init();
}

static void hfgDeactivateOtherContext(U32 bHeadset){
    HfgAdpChannel *pChannel, *nextChannel;
    BtStatus status;
    
    pChannel = (HfgAdpChannel*)GetHeadList(&g_channel_list);
    while(&pChannel->node != &g_channel_list){
        nextChannel = (HfgAdpChannel*)GetHeadList(&pChannel->node);        
        if((pChannel->bHeadset && bHeadset) || 
            (!pChannel->bHeadset && !bHeadset)){
            OS_Report("hfgDeactivateOtherContext : bHeadset=%u, context=%p", bHeadset, pChannel);
            status = hfg_deregister(&pChannel->channel);
            if(status == BT_STATUS_CONNECTION_EXIST)
            {
                pChannel->bDeregistered = KAL_TRUE;
            }
            else
            {
                OS_Report("[SH] remove and free HfgAdpChannel : %p", pChannel);
                RemoveEntryList(&pChannel->node);
                free_ctrl_buffer(pChannel);        
            }
        }
        pChannel = nextChannel;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_activate
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_activate_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NO_RESOURCES;
    HfgAdpChannel *adp_channel = NULL;
    bt_hfg_activate_req_struct* msg_p;
    bt_hfg_activate_cnf_struct* cnf_msg_p;

    msg_p = (bt_hfg_activate_req_struct*)ilm_ptr->local_para_ptr;
#ifdef __BT_USE_CUST_DATA__    
    if (msg_p->bHeadset &&
        !((U32)btGetCustvalue(CUSTID_HFP_SUPPORT_MASK) & HFP_SUPPORT_MASK_HSP_AG))
    {
        status = BT_STATUS_NOT_SUPPORTED;
    }
    if (!(msg_p->bHeadset) &&
        !((U32)btGetCustvalue(CUSTID_HFP_SUPPORT_MASK) & HFP_SUPPORT_MASK_HFP_AG))
    {
        status = BT_STATUS_NOT_SUPPORTED;
    }
#endif    
    if (status != BT_STATUS_NOT_SUPPORTED)
    {
        adp_channel = (HfgAdpChannel*)get_ctrl_buffer(sizeof(HfgAdpChannel));
        hfgDeactivateOtherContext(msg_p->bHeadset ? 1 : 0);
        if(adp_channel != NULL)
        {
            OS_MemSet((U8*)adp_channel, 0, sizeof(HfgAdpChannel));
            {
                adp_channel->bHeadset = msg_p->bHeadset ? 1 : 0;
                status = hfg_register(&adp_channel->channel, hfg_app_callback, msg_p->bHeadset);
                if(status == BT_STATUS_SUCCESS)
                {
                    adp_channel->userContext = msg_p->req_context;
                    adp_channel->registered_module = ilm_ptr->src_mod_id;
                    initialize_rsp_queue(adp_channel);
                    InsertTailList(&g_channel_list, &adp_channel->node);
                    adp_channel->timer.context = (void*)adp_channel;
                }
                else
                {
                    free_ctrl_buffer((void*)adp_channel);
                    adp_channel = NULL;
                }
            }
        }
    }
    
    /* send active confirm message */
    cnf_msg_p = (bt_hfg_activate_cnf_struct*) construct_local_para(sizeof(bt_hfg_activate_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->result = status;
    cnf_msg_p->req_context = msg_p->req_context;
    hfg_sendMsg(MSG_ID_BT_HFG_ACTIVATE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_deactivate
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_deactivate_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_deactivate_req_struct *msg_p;
    bt_hfg_deactivate_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;

    msg_p = (bt_hfg_deactivate_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    /* construct return message */
    cnf_msg_p = (bt_hfg_deactivate_cnf_struct*) construct_local_para(sizeof(bt_hfg_deactivate_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;
    
    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        status = hfg_deregister(&adp_channel->channel);
        /* Send deactive confirm */
        cnf_msg_p->result = status;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_DEACTIVATE_CNF, (local_para_struct*)cnf_msg_p);
        if(status == BT_STATUS_CONNECTION_EXIST)
        {
            adp_channel->bDeregistered = KAL_TRUE;
        }
        else
        {
            RemoveEntryList(&adp_channel->node);
            free_ctrl_buffer(adp_channel);        
        }
    }
    else
    {
        cnf_msg_p->result = status;
        hfg_sendMsg(MSG_ID_BT_HFG_DEACTIVATE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}

void hfg_adp_connect_timerout_hdl(EvmTimer *timer)
{
    HfgAdpChannel *adp_channel = (HfgAdpChannel*)timer->context;
    
    bt_prompt_trace(MOD_BT, "[HFG][Info] HFG connection timeout triggered");        
    hfg_disconnect_service_link(&adp_channel->channel);
}

/*****************************************************************************
 * FUNCTION
 *  hfga_connect_req_hdler
 * DESCRIPTION
 *  This function is to handle connect request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_connect_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_connect_req_struct *msg_p;
    bt_hfg_connect_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;

    msg_p = (bt_hfg_connect_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    /* construct return message */
    cnf_msg_p = (bt_hfg_deactivate_cnf_struct*) construct_local_para(sizeof(bt_hfg_deactivate_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;
    
    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        status = hfg_create_service_link(&adp_channel->channel, (BD_ADDR*)&msg_p->bt_addr);
        if(status == BT_STATUS_PENDING){
            adp_channel->timer.func = hfg_adp_connect_timerout_hdl;
            bt_prompt_trace(MOD_BT, "[HFG][Info] HFG connection timer started ");
            EVM_StartTimer(&adp_channel->timer, HFG_CONNECT_TIMEOUT + HFG_SLC_INITIALIZE_TIMEOUT);
        }
        /* Send deactive confirm */
        cnf_msg_p->result = status;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_CONNECT_CNF, (local_para_struct*)cnf_msg_p);
    }
    else
    {
        cnf_msg_p->result = status;
        hfg_sendMsg(MSG_ID_BT_HFG_CONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_accept_channel_hdler
 * DESCRIPTION
 *  This function is to handle accept connect request from remote
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_accept_channel_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_accept_channel_req_struct *msg_p;
    bt_hfg_accept_channel_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;

    msg_p = (bt_hfg_accept_channel_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    /* construct return message */
    cnf_msg_p = (bt_hfg_accept_channel_cnf_struct*) construct_local_para(sizeof(bt_hfg_accept_channel_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        status = hfg_accept_connect(&adp_channel->channel);
        adp_channel->timer.func = hfg_adp_connect_timerout_hdl;
        EVM_StartTimer(&adp_channel->timer, HFG_SLC_INITIALIZE_TIMEOUT);
        /* Send deactive confirm */
        cnf_msg_p->result = status;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_ACCEPT_CHANNEL_CNF, (local_para_struct*)cnf_msg_p);
    }
    else
    {
        cnf_msg_p->result = status;
        hfg_sendMsg(MSG_ID_BT_HFG_ACCEPT_CHANNEL_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_reject_channel_hdler
 * DESCRIPTION
 *  This function is to handle reject connect request from remote
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
#define __HFG_INDIRECT_REJECT__
U8 btbmCheckDeviceClipMusic801(U8 *addr);

void hfg_adp_reject_channel_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_reject_channel_req_struct *msg_p;
    bt_hfg_reject_channel_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;

    msg_p = (bt_hfg_reject_channel_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    /* construct return message */
    cnf_msg_p = (bt_hfg_reject_channel_cnf_struct*) construct_local_para(sizeof(bt_hfg_reject_channel_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        #ifdef __HFG_INDIRECT_REJECT__
            OS_Report("[IOT] remDev=%X%X%X%X%X%X", 
            adp_channel->channel.bdAddr.addr[0],
            adp_channel->channel.bdAddr.addr[1],
            adp_channel->channel.bdAddr.addr[2],
            adp_channel->channel.bdAddr.addr[3],
            adp_channel->channel.bdAddr.addr[4],
            adp_channel->channel.bdAddr.addr[5]);
        
        if(btbmCheckDeviceClipMusic801(adp_channel->channel.bdAddr.addr) != 0)
        {
            status = hfg_accept_connect(&adp_channel->channel);
            if(status== BT_STATUS_SUCCESS || status== BT_STATUS_PENDING)
            {
                status = hfg_disconnect_service_link(&adp_channel->channel);
            }
        }
        else
        {
        status = hfg_reject_connect(&adp_channel->channel);
        }
        #else
        status = hfg_reject_connect(&adp_channel->channel);
        #endif
        /* Send deactive confirm */
        cnf_msg_p->result = status;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_REJECT_CHANNEL_CNF, (local_para_struct*)cnf_msg_p);
    }
    else
    {
        cnf_msg_p->result = status;
        hfg_sendMsg(MSG_ID_BT_HFG_REJECT_CHANNEL_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_disconnect_req_hdler
 * DESCRIPTION
 *  This function is to handle disconnect request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_disconnect_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_disconnect_req_struct *msg_p;
    bt_hfg_disconnect_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;

    msg_p = (bt_hfg_disconnect_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        status = hfg_disconnect_service_link(&adp_channel->channel);
        /* Send deactive confirm */
#ifdef __HFG_SYNC_DISCONNECT__
        if(status == BT_STATUS_PENDING)
        {
            adp_channel->syncDisconnect = 1;
            return;
        }
#endif
    }
    /* construct return message */
    cnf_msg_p = (bt_hfg_disconnect_cnf_struct*) construct_local_para(sizeof(bt_hfg_disconnect_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;
    cnf_msg_p->result = status;
    hfg_sendMsg(MSG_ID_BT_HFG_DISCONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_sco_connect_req_hdler
 * DESCRIPTION
 *  This function is to handle sco connect request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_sco_connect_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_sco_connect_req_struct *msg_p;
    bt_hfg_sco_connect_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;

    msg_p = (bt_hfg_sco_connect_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    /* construct return message */
    cnf_msg_p = (bt_hfg_sco_connect_cnf_struct*) construct_local_para(sizeof(bt_hfg_sco_connect_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;
    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        status = hfg_create_audio_link(&adp_channel->channel);
        /* ALPS00235156 : DS-980 will goes wrong if we send A2DP streaming 
            data and SCO data at the same time.
        */        
        if(status == BT_STATUS_PENDING && hfg_needSuspendA2DPStreaming(&adp_channel->channel)){
            bt_prompt_trace(MOD_BT, "[HFG][Info] suspending A2DP streaming");
#ifdef __BT_A2DP_PROFILE__            
            a2dp_set_sco_state(1);
#endif
        }
        /* Send deactive confirm */
        cnf_msg_p->result = status;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SCO_CONNECT_CNF, (local_para_struct*)cnf_msg_p);
    }
    else
    {
        cnf_msg_p->result = status;
        hfg_sendMsg(MSG_ID_BT_HFG_SCO_CONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_sco_disconnect_req_hdler
 * DESCRIPTION
 *  This function is to handle timeout case
 * PARAMETERS
 *  
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_sco_disconnect_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_sco_disconnect_req_struct *msg_p;
    bt_hfg_sco_disconnect_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;

    msg_p = (bt_hfg_sco_disconnect_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    /* construct return message */
    cnf_msg_p = (bt_hfg_sco_disconnect_cnf_struct*) construct_local_para(sizeof(bt_hfg_sco_disconnect_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        status = hfg_disconnect_audio_link(&adp_channel->channel);
        /* Send disconnect confirm */
        cnf_msg_p->result = status;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SCO_DISCONNECT_CNF, (local_para_struct*)cnf_msg_p);
    }
    else
    {
        cnf_msg_p->result = status;
        hfg_sendMsg(MSG_ID_BT_HFG_SCO_DISCONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_ok_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_ok_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_ok_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_ok_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;
    
    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_send_ok(&adp_channel->channel, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);    
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_OK_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_OK_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_OK_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_error_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_error_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_error_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_error_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_send_error(&adp_channel->channel, msg_p->data, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_ERROR_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_ERROR_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_ERROR_CNF;
    }
}


/*****************************************************************************
 * FUNCTION
 *  hfg_adp_no_carrier_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_no_carrier_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_no_carrier_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_no_carrier_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_no_carrier_rsp(&adp_channel->channel, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_NO_CARRIER_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_NO_CARRIER_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_NO_CARRIER_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_busy_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_busy_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_busy_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_busy_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_busy_rsp(&adp_channel->channel, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_BUSY_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_BUSY_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_BUSY_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_no_answer_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_no_answer_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_no_answer_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_no_answer_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_no_answer_rsp(&adp_channel->channel, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_NO_ANSWER_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_NO_ANSWER_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_NO_ANSWER_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_delayed_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_delayed_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_delayed_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_delayed_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_delayed_rsp(&adp_channel->channel, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_DELAYED_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_DELAYED_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_DELAYED_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_blacklisted_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_blacklisted_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_blacklisted_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_blacklisted_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_blacklisted_rsp(&adp_channel->channel, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_BLACKLISTED_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_BLACKLISTED_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_BLACKLISTED_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_ring_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_ring_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_ring_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_ring_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_send_ring(&adp_channel->channel, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_RING_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_RING_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_RING_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_call_waiting_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_call_waiting_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_call_waiting_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char *number = NULL;
    HfgCallWaitParms wait;


    msg_p = (bt_hfg_call_waiting_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen((const char*)msg_p->number);
        if(len != 0)
        {
            number = (char*)get_ctrl_buffer(len+1);
            if(number != NULL)
            {
                OS_MemCopy((U8*)number, (const U8*)msg_p->number, len+1);
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        if (status != BT_STATUS_NO_RESOURCES)
        {
            response = hfg_alloc_rsp(adp_channel);
            if(response != NULL)
            {
                wait.classmap = msg_p->classmap;
                wait.type = msg_p->type;
                wait.number = number;
                status = hfg_call_waiting(&adp_channel->channel, &wait, response);
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(number != NULL)
            free_ctrl_buffer(number);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_CALL_WAITING_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_CALL_WAITING_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_CALL_WAITING_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_read_indicator_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_read_indicator_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_read_indicator_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_read_indicator_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_read_indicator(&adp_channel->channel, &msg_p->indicators, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_READ_INDICATOR_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_READ_INDICATOR_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_READ_INDICATOR_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_call_list_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_call_list_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_call_list_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char* number = NULL;
    HfgCallListParms  call;

    msg_p = (bt_hfg_call_list_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen((const char*)msg_p->number);
        if(len != 0)
        {
            number = (char*)get_ctrl_buffer(len+1);
            if(number != NULL)
            {
                OS_MemCopy((U8*)number, (const U8*)msg_p->number, len+1);
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        if (status != BT_STATUS_NO_RESOURCES)
        {
            response = hfg_alloc_rsp(adp_channel);
            if(response != NULL)
            {
                call.index = msg_p->index;
                call.dir = msg_p->dir;
                call.state = msg_p->state;
                call.mode = msg_p->mode;
                call.multiParty = msg_p->multiParty;
                call.type = msg_p->type;
                call.number = number;
                status = hfg_send_call_list_rsp(&adp_channel->channel, &call, response, msg_p->bFinal);
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(number != NULL)
            free_ctrl_buffer(number);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_CALL_LIST_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_CALL_LIST_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_CALL_LIST_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_call_id_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_call_id_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_call_id_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char* number = NULL;
    HfgCallerIdParms callid;

    msg_p = (bt_hfg_call_id_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen((const char*)msg_p->number);
        if(len != 0)
        {
            number = get_ctrl_buffer(len+1);
            if(number != NULL)
            {
                OS_MemCopy((U8*)number, (const U8*)msg_p->number, len+1);
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        if (status != BT_STATUS_NO_RESOURCES)
        {
            response = hfg_alloc_rsp(adp_channel);
            if(response != NULL)
            {
                callid.type = msg_p->type;
                callid.number = number;
                status = hfg_send_caller_id(&adp_channel->channel, &callid, response);
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(number != NULL)
            free_ctrl_buffer(number);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_CALL_ID_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_CALL_ID_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_CALL_ID_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_indicator_report_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_indicator_report_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_indicator_report_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_indicator_report_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_set_indicator_value(&adp_channel->channel, msg_p->indicator, msg_p->value, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_INDICATOR_REPORT_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_INDICATOR_REPORT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_INDICATOR_REPORT_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_subscriber_number_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_subscriber_number_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_subscriber_number_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char* number = NULL;
    HfgSubscriberNum subscriber;

    msg_p = (bt_hfg_subscriber_number_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen((const char*)msg_p->number);
        if(len != 0)
        {
            number = get_ctrl_buffer(len+1);
            if(number != NULL)
            {
                OS_MemCopy((U8*)number, (const U8*)msg_p->number, len+1);
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }

        if(status != BT_STATUS_NO_RESOURCES)
        {
            response = hfg_alloc_rsp(adp_channel);
            if(response != NULL)
            {
                subscriber.type = msg_p->type;
                subscriber.service = msg_p->service;
                subscriber.number = number;
                status = hfg_send_subscriber_number_rsp(&adp_channel->channel, &subscriber, response, msg_p->bFinal);
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(number != NULL)
            free_ctrl_buffer(number);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SUBSCRIBER_NUMBER_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SUBSCRIBER_NUMBER_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SUBSCRIBER_NUMBER_CNF;
    }
}


/*****************************************************************************
 * FUNCTION
 *  hfg_adp_subscriber_number_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_operator_name_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_operator_name_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char* name = NULL;
    HfgNetworkOperator netOper;
    
    msg_p = (bt_hfg_operator_name_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen((const char*)msg_p->oper);
        if(len != 0)
        {
            name = get_ctrl_buffer(len+1);
            if(name != NULL)
            {
                OS_MemCopy((U8*)name, (const U8*)msg_p->oper, len+1);
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        if (status != BT_STATUS_NO_RESOURCES)
        {
            response = hfg_alloc_rsp(adp_channel);
            if(response != NULL)
            {
                netOper.mode = msg_p->mode;
                netOper.format = msg_p->format;
                netOper.oper = name;
                status = hfg_send_network_operator_rsp(&adp_channel->channel, &netOper, response);
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(name != NULL)
            free_ctrl_buffer(name);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_OPERATOR_NAME_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_OPERATOR_NAME_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_OPERATOR_NAME_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_voice_tag_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_voice_tag_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_voice_tag_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char* number = NULL;

    msg_p = (bt_hfg_voice_tag_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen(msg_p->number);
        if(len != 0)
        {
            number = (char*)get_ctrl_buffer(len+1);
            if(number != NULL)
            {
                OS_MemCopy((U8*)number, (const U8*)msg_p->number, len+1);
                //msg_p->number = number;
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                    status = hfg_voice_tag_rsp(&adp_channel->channel, number, response);
                else
                    status = BT_STATUS_NO_RESOURCES;
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(number != NULL)
            free_ctrl_buffer(number);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_VOICE_TAG_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_VOICE_TAG_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_VOICE_TAG_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_voice_rec_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_voice_rec_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_voice_rec_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_voice_rec_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_enable_vr(&adp_channel->channel, msg_p->enable, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_VOICE_REC_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_VOICE_REC_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_VOICE_REC_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_response_hold_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
#if HFG_USE_RESP_HOLD == XA_ENABLED
void hfg_adp_response_hold_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_response_hold_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_response_hold_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_send_response_hold_state(&adp_channel->channel, &msg_p->data, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_RESPONSE_HOLD_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_RESPONSE_HOLD_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
        else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_RESPONSE_HOLD_CNF;
    }
}
#endif
/*****************************************************************************
 * FUNCTION
 *  hfg_adp_speaker_gain_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_speaker_gain_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_speaker_gain_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_speaker_gain_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_send_speaker_volume(&adp_channel->channel, msg_p->data, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SPEAKER_GAIN_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SPEAKER_GAIN_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SPEAKER_GAIN_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_mic_gain_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_mic_gain_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_mic_gain_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_mic_gain_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_send_mic_volume(&adp_channel->channel, msg_p->data, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_MIC_GAIN_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_MIC_GAIN_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_MIC_GAIN_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_voice_rec_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_ringtone_status_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_ringtone_status_req_struct *msg_p;
    bt_hfg_ringtone_status_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_ringtone_status_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_send_ringtone_status(&adp_channel->channel, msg_p->enable, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_ringtone_status_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_RINGTONE_STATUS_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_RINGTONE_STATUS_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_RINGTONE_STATUS_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_supported_charset_req_hdler
 * DESCRIPTION
 *  This function is to handle sending supported charsets by AG request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_supported_charset_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_supported_charset_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char* supported = NULL;

    msg_p = (bt_hfg_supported_charset_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen(msg_p->supported_charset);
        if(len != 0)
        {
            supported = (char*)get_ctrl_buffer(len+1);
            if(supported != NULL)
            {
                OS_MemCopy((U8*)supported, (const U8*)msg_p->supported_charset, len+1);
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                    status = hfg_send_supported_charset_rsp(&adp_channel->channel, supported, response);
                else
                    status = BT_STATUS_NO_RESOURCES;
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(supported != NULL)
            free_ctrl_buffer(supported);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SUPPORTED_CHARSET_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SUPPORTED_CHARSET_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SUPPORTED_CHARSET_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_selected_charset_req_hdler
 * DESCRIPTION
 *  This function is to handle sending supported charsets by AG request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_selected_charset_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_selected_charset_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char *selected = NULL;

    msg_p = (bt_hfg_selected_charset_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen(msg_p->selected_charset);
        if(len != 0)
        {
            selected = (char*)get_ctrl_buffer(len+1);
            if(selected != NULL)
            {
                OS_MemCopy((U8*)selected, (const U8*)msg_p->selected_charset, len+1);
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                    status = hfg_send_selected_charset_rsp(&adp_channel->channel, selected, response);
                else
                    status = BT_STATUS_NO_RESOURCES;
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(selected != NULL)
            free_ctrl_buffer(selected);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SELECTED_CHARSET_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SELECTED_CHARSET_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SELECTED_CHARSET_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_model_id_req_hdler
 * DESCRIPTION
 *  This function is to handle sending model id request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_model_id_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_model_id_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char *modelId = NULL;

    msg_p = (bt_hfg_model_id_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen(msg_p->modelId);
        if(len != 0)
        {
            modelId = (char*)get_ctrl_buffer(len+1);
            if(modelId != NULL)
            {
                OS_MemCopy((U8*)modelId, (const U8*)msg_p->modelId, len+1);
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                    status = hfg_send_model_id_rsp(&adp_channel->channel, modelId, response);
                else
                    status = BT_STATUS_NO_RESOURCES;
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(modelId != NULL)
            free_ctrl_buffer(modelId);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_MODEL_ID_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_MODEL_ID_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_MODEL_ID_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_manufacture_id_req_hdler
 * DESCRIPTION
 *  This function is to handle sending model id request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_manufacture_id_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_manufacture_id_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char *manufactureId = NULL;

    msg_p = (bt_hfg_manufacture_id_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen(msg_p->manufactureId);
        if(len != 0)
        {
            manufactureId = (char*)get_ctrl_buffer(len+1);
            if(manufactureId != NULL)
            {
                OS_MemCopy((U8*)manufactureId, (const U8*)msg_p->manufactureId, len+1);
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                    status = hfg_send_manufacture_id_rsp(&adp_channel->channel, manufactureId, response);
                else
                    status = BT_STATUS_NO_RESOURCES;
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(manufactureId != NULL)
            free_ctrl_buffer(manufactureId);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_MANUFACTURE_ID_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_MANUFACTURE_ID_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_MANUFACTURE_ID_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_send_raw_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_send_raw_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_send_data_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char* at_result = NULL;

    msg_p = (bt_hfg_send_data_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen(msg_p->atresult);
        if(len != 0)
        {
            at_result = get_ctrl_buffer(len+1);
            if(at_result != NULL)
            {
                OS_MemCopy((U8*)at_result, (const U8*)msg_p->atresult, len+1);
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                    status = hfg_send_at_response(&adp_channel->channel, at_result, response, msg_p->bDone);
                else
                    status = BT_STATUS_NO_RESOURCES;
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(at_result != NULL)
            free_ctrl_buffer(at_result);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SEND_DATA_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SEND_DATA_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SEND_DATA_CNF;
    }
}

/********************************************************/
/*  Phonebook related                                                                */
/********************************************************/
#if AT_PHONEBOOK == XA_ENABLED
void hfg_adp_supported_phonebook_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_supported_phonebook_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_supported_phonebook_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_send_pb_supported_storage_rsp(&adp_channel->channel, (U32)&msg_p->supported, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SUPPORTED_PHONEBOOK_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SUPPORTED_PHONEBOOK_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SUPPORTED_PHONEBOOK_CNF;
    }
}

void hfg_adp_selected_phonebook_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_selected_phonebook_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    HfgPbStorageSelected pbSelected;
        
    msg_p = (bt_hfg_selected_phonebook_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
        {
            pbSelected.selected = msg_p->selected;
            pbSelected.used = msg_p->used;
            pbSelected.total = msg_p->total;
            status = hfg_send_pb_selected_storage_rsp(&adp_channel->channel, (U32)&pbSelected, response);
        }
        else
        {
            status = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SELECTED_PHONEBOOK_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SELECTED_PHONEBOOK_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SELECTED_PHONEBOOK_CNF;
    }
}

void hfg_adp_read_phonebook_info_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_read_phonebook_info_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char* range = NULL;
    HfgPbReadInfo  pbReadInfo;

    msg_p = (bt_hfg_read_phonebook_info_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen(msg_p->range);
        if(len != 0)
        {
            range = (char*)get_ctrl_buffer(len+1);
            if(range != NULL)
            {
                OS_MemCopy((U8*)range, (const U8*)msg_p->range, len+1);
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                {
                    pbReadInfo.range = range;
                    pbReadInfo.numLength = msg_p->numLength;
                    pbReadInfo.textLength = msg_p->textLength;
                    status = hfg_send_pb_read_info_rsp(&adp_channel->channel, &pbReadInfo, response);
                }
                else
                {
                    status = BT_STATUS_NO_RESOURCES;
                }
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(range != NULL)
            free_ctrl_buffer(range);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_READ_PHONEBOOK_INFO_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_READ_PHONEBOOK_INFO_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_READ_PHONEBOOK_INFO_CNF;
    }
}

void hfg_adp_read_phonebook_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_read_phonebook_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char* number = NULL;
    char* text = NULL;
    HfgPbEntry  pbEntry;

    msg_p = (bt_hfg_read_phonebook_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen(msg_p->number);
        if(len > 0)
        {
            number = (char*)get_ctrl_buffer(len+1);
            if(number)
            {
                OS_MemCopy((U8*)number, (const U8*)msg_p->number, len+1);
                len = OS_StrLen(msg_p->text);
                if(len > 0)
                {
                    text = (char*)get_ctrl_buffer(len+1);
                    if(text)
                        OS_MemCopy((U8*)text, (const U8*)msg_p->text, len+1);
                }                    
            }
        }
        if(len != 0)
        {
            if(text != NULL && number != NULL)
            {
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                {
                    pbEntry.index = msg_p->index;
                    pbEntry.type = msg_p->type;
                    pbEntry.number = number;
                    pbEntry.text = text;
                    status = hfg_send_pb_read_rsp(&adp_channel->channel, &pbEntry, response, msg_p->bFinal);
                }
                else
                {
                    status = BT_STATUS_NO_RESOURCES;
                }
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(number != NULL)
            free_ctrl_buffer(number);
        if(text != NULL)
            free_ctrl_buffer(text);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_READ_PHONEBOOK_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_READ_PHONEBOOK_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_READ_PHONEBOOK_CNF;
    }
}

void hfg_adp_find_phonebook_info_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_find_phonebook_info_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    HfgPbFindInfo pbFindInfo;
        
    msg_p = (bt_hfg_find_phonebook_info_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
        {
            pbFindInfo.numLength = msg_p->numLength;
            pbFindInfo.textLength = msg_p->textLength;
            status = hfg_send_pb_find_info_rsp(&adp_channel->channel, (U32)&pbFindInfo, response);
        }
        else
        {
            status = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_FIND_PHONEBOOK_INFO_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_FIND_PHONEBOOK_INFO_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_FIND_PHONEBOOK_INFO_CNF;
    }
}

void hfg_adp_find_phonebook_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_find_phonebook_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char* number = NULL;
    char* text = NULL;
    HfgPbEntry  pbEntry;

    msg_p = (bt_hfg_find_phonebook_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen(msg_p->number);
        if(len > 0)
        {
            number = (char*)get_ctrl_buffer(len+1);
            if(number)
            {
                OS_MemCopy((U8*)number, (const U8*)msg_p->number, len+1);
                len = OS_StrLen(msg_p->text);
                if(len > 0)
                {
                    text = (char*)get_ctrl_buffer(len+1);
                    if(text)
                        OS_MemCopy((U8*)text, (const U8*)msg_p->text, len+1);
                }                    
            }
        }
        if(len != 0)
        {
            if(text != NULL && number != NULL)
            {
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                {
                    pbEntry.index = msg_p->index;
                    pbEntry.type = msg_p->type;
                    pbEntry.number = number;
                    pbEntry.text = text;
                    status = hfg_send_pb_find_rsp(&adp_channel->channel, &pbEntry, response, msg_p->bFinal);
                }
                else
                {
                    status = BT_STATUS_NO_RESOURCES;
                }
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(number != NULL)
            free_ctrl_buffer(number);
        if(text != NULL)
            free_ctrl_buffer(text);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_FIND_PHONEBOOK_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_FIND_PHONEBOOK_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_FIND_PHONEBOOK_CNF;
    }
}

void hfg_adp_write_phonebook_info_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_write_phonebook_info_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    U16 len;
    char* range = NULL;
    char* types = NULL;
    HfgPbWriteInfo  pbWriteInfo;

    msg_p = (bt_hfg_write_phonebook_info_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        len = OS_StrLen(msg_p->range);
        if(len > 0)
        {
            range = (char*)get_ctrl_buffer(len+1);
            if(range)
            {
                OS_MemCopy((U8*)range, (const U8*)msg_p->range, len+1);
                len = OS_StrLen(msg_p->types);
                if(len > 0)
                {
                    types = (char*)get_ctrl_buffer(len+1);
                    if(types)
                        OS_MemCopy((U8*)types, (const U8*)msg_p->types, len+1);
                }                    
            }
        }
        if(len != 0)
        {
            if(types != NULL && range != NULL)
            {
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                {
                    pbWriteInfo.numLength = msg_p->numLength;
                    pbWriteInfo.textLength = msg_p->textLength;
                    pbWriteInfo.range = range;
                    pbWriteInfo.types = types;
                    status = hfg_send_pb_write_info_rsp(&adp_channel->channel, &pbWriteInfo, response);
                }
                else
                {
                    status = BT_STATUS_NO_RESOURCES;
                }
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(range != NULL)
            free_ctrl_buffer(range);
        if(types != NULL)
            free_ctrl_buffer(types);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_WRITE_PHONEBOOK_INFO_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_WRITE_PHONEBOOK_INFO_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_WRITE_PHONEBOOK_INFO_CNF;
    }
}
#endif /* AT_PHONEBOOK == XA_ENABLED */
/********************************************************/
/*  SMS related                                                                         */
/********************************************************/
#if AT_SMS == XA_ENABLED

/* AT_SELECT_SMS_SERVICE|AT_TEST */
/* MSG_ID_BT_HFG_SUPPORTED_SMS_SERVICE_REQ */
void hfg_adp_hfg_handle_supported_sms_service_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_suppoted_sms_service_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    char* supportedServ = NULL;

    msg_p = (bt_hfg_suppoted_sms_service_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        if(msg_p->supportedServiceLen)
        {
            supportedServ = allocStringBuf(msg_p->supportedService, msg_p->supportedServiceLen);
            if(supportedServ)
            {
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                {
                    status = hfg_send_sms_supported_service_rsp(&adp_channel->channel, supportedServ, response);
                }
                else
                {
                    status = BT_STATUS_NO_RESOURCES;
                }
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(supportedServ)
            free_ctrl_buffer(supportedServ);
        if(response != NULL)
                    hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
            if(adp_channel != NULL)
            {
                    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SUPPORTED_SMS_SERVICE_CNF, (local_para_struct*)cnf_msg_p);        
            }
            else
            {
                    hfg_sendMsg(MSG_ID_BT_HFG_SUPPORTED_SMS_SERVICE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
            }
        }
        else
        {
            HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
            adp_response->req_context = msg_p->req_context;
            adp_response->req_msg = MSG_ID_BT_HFG_SUPPORTED_SMS_SERVICE_CNF;
        }
}
/* AT_SELECT_SMS_SERVICE|AT_READ */
/* MSG_ID_BT_HFG_SELECTED_SMS_SERVICE_REQ */
void hfg_adp_hfg_handle_selected_sms_service_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_selected_sms_service_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;
    HfgSMSService_read  service;

    msg_p = (bt_hfg_selected_sms_service_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
        {
            service.service = msg_p->type;
            service.bm = msg_p->bm;
            service.mt = msg_p->mt;
            service.mo = msg_p->mo;
            status = hfg_send_sms_selected_service_rsp(&adp_channel->channel, &service, response);
        }
        else
        {
            status = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SELECTED_SMS_SERVICE_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SELECTED_SMS_SERVICE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SELECTED_SMS_SERVICE_CNF;
    }
}

/* AT_SELECT_SMS_SERVICE */
/* MSG_ID_BT_HFG_SMS_SERVICE_REQ */
void hfg_adp_hfg_handle_sms_service_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_sms_service_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel       *adp_channel;
    HfgResponse             *response = NULL;
    HfgSMSService_result    service;

    msg_p = (bt_hfg_sms_service_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
        {
            service.bm = msg_p->bm;
            service.mt = msg_p->mt;
            service.mo = msg_p->mo;
            status = hfg_send_sms_select_service_rsp(&adp_channel->channel, &service, response);
        }
        else
        {
            status = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SMS_SERVICE_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SMS_SERVICE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SMS_SERVICE_CNF;
    }
}

/* AT_PREFERRED_SMS_STORAGE|AT_TEST */
/* MSG_ID_BT_HFG_SUPPORTED_PREF_MSG_STORAGE_REQ */
void hfg_adp_hfg_handle_supported_pref_msg_storage_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_supported_pref_msg_storage_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel           *adp_channel;
    HfgResponse                 *response = NULL;
    HfgSMSPrefStorage_test      prefStorage;

    msg_p = (bt_hfg_supported_pref_msg_storage_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
        {
            prefStorage.read = msg_p->read;
            prefStorage.write = msg_p->write;
            prefStorage.recv = msg_p->recv;
            status = hfg_send_sms_supported_pref_storage_rsp(&adp_channel->channel, &prefStorage, response);
        }
        else
        {
            status = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SUPPORTED_PREF_MSG_STORAGE_REQ, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SUPPORTED_PREF_MSG_STORAGE_REQ, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SUPPORTED_PREF_MSG_STORAGE_REQ;
    }
}

/* AT_PREFERRED_SMS_STORAGE|AT_READ */
/* MSG_ID_BT_HFG_SELECTED_PREF_MSG_STORAGE_REQ */
void hfg_adp_hfg_handle_selected_pref_msg_storage_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_selected_pref_msg_storage_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel           *adp_channel;
    HfgResponse                 *response = NULL;
    HfgSMSPrefStorage_read  prefStorage;

    msg_p = (bt_hfg_selected_pref_msg_storage_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
        {
            prefStorage.read = msg_p->read;
            prefStorage.readUsed = msg_p->readUsed;
            prefStorage.readTotal = msg_p->readTotal;
            prefStorage.write = msg_p->write;
            prefStorage.writeUsed = msg_p->writeUsed;
            prefStorage.writeTotal = msg_p->writeTotal;
            prefStorage.recv = msg_p->recv;
            prefStorage.recvUsed = msg_p->recvUsed;
            prefStorage.recvTotal = msg_p->recvTotal;
            status = hfg_send_sms_selected_pref_storage_rsp(&adp_channel->channel, &prefStorage, response);
        }
        else
        {
            status = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SELECTED_PREF_MSG_STORAGE_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SELECTED_PREF_MSG_STORAGE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SELECTED_PREF_MSG_STORAGE_CNF;
    }
}

/* AT_PREFERRED_SMS_STORAGE */
/* MSG_ID_BT_HFG_PREF_MSG_STORAGE_REQ */
void hfg_adp_hfg_handle_pref_msg_storage_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_pref_msg_storage_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel           *adp_channel;
    HfgResponse                 *response = NULL;
    HfgSMSPrefStorage_result    prefStorage;

    msg_p = (bt_hfg_pref_msg_storage_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
        {
            prefStorage.count = msg_p->count;
            prefStorage.readUsed = msg_p->readUsed;
            prefStorage.readTotal = msg_p->readTotal;
            prefStorage.writeUsed = msg_p->writeUsed;
            prefStorage.writeTotal = msg_p->writeTotal;
            prefStorage.recvUsed = msg_p->recvUsed;
            prefStorage.recvTotal = msg_p->recvTotal;
            status = hfg_send_sms_select_pref_storage_rsp(&adp_channel->channel, &prefStorage, response);
        }
        else
        {
            status = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_PREF_MSG_STORAGE_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_PREF_MSG_STORAGE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_PREF_MSG_STORAGE_CNF;
    }
}

/* AT_SMS_MESSAGE_FORMAT|AT_TEST */
/* MSG_ID_BT_HFG_SUPPORTED_MSG_FORMAT_REQ */
void hfg_adp_hfg_handle_supported_msg_format_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_supported_msg_format_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel   *adp_channel;
    HfgResponse         *response = NULL;
    char* supportedFormat = NULL;

    msg_p = (bt_hfg_supported_msg_format_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        if(msg_p->supportedFormatLen)
        {
            supportedFormat = allocStringBuf(msg_p->supportedFormat, msg_p->supportedFormatLen);
            if(supportedFormat)
            {
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                {
                    status = hfg_send_sms_supported_format_rsp(&adp_channel->channel, supportedFormat, response);
                }
                else
                {
                    status = BT_STATUS_NO_RESOURCES;
                }
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(supportedFormat)
            free_ctrl_buffer(supportedFormat);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SUPPORTED_MSG_FORMAT_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SUPPORTED_MSG_FORMAT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SUPPORTED_MSG_FORMAT_CNF;
    }
}

/* AT_SMS_MESSAGE_FORMAT|AT_READ */
/* MSG_ID_BT_HFG_SELECTED_MSG_FORMAT_REQ */
void hfg_adp_hfg_handle_selected_msg_format_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_selected_msg_format_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel   *adp_channel;
    HfgResponse         *response = NULL;

    msg_p = (bt_hfg_selected_msg_format_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
        {
            status = hfg_send_sms_selected_format_rsp(&adp_channel->channel, msg_p->mode, response);
        }
        else
        {
            status = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SELECTED_MSG_FORMAT_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SELECTED_MSG_FORMAT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SELECTED_MSG_FORMAT_CNF;
    }
}

/* AT_SMS_SERVICE_CENTER|AT_READ */
/* MSG_ID_BT_HFG_SERVICE_CENTRE_REQ */
void hfg_adp_hfg_handle_service_centre_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_service_centre_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel   *adp_channel;
    HfgResponse         *response = NULL;
    char* sca = NULL;
    HfgSMSSrviceCentre_read sc;

    msg_p = (bt_hfg_service_centre_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        if(msg_p->addrLen)
        {
            sca = allocStringBuf(msg_p->addr, msg_p->addrLen);
            if(sca)
            {
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                {
                    sc.sca = sca;
                    sc.tosca = msg_p->addrType;
                    status = hfg_send_sms_service_centre_rsp(&adp_channel->channel, &sc, response);
                }
                else
                {
                    status = BT_STATUS_NO_RESOURCES;
                }
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(sca)
            free_ctrl_buffer(sca);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SERVICE_CENTRE_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SERVICE_CENTRE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SERVICE_CENTRE_CNF;
    }
}

/* AT_SET_TEXT_MODE_PARMS|AT_READ */
/* MSG_ID_BT_HFG_TEXT_MODE_PARAMS_REQ */
void hfg_adp_hfg_handle_text_mode_params_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_text_mode_params_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel   *adp_channel;
    HfgResponse         *response = NULL;
    char* strVp = NULL;
    HfgSMSTextModeParam_read textModeParams;

    msg_p = (bt_hfg_text_mode_params_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if (!response)
        {
            status = BT_STATUS_NO_RESOURCES;
            goto exit;
        }

        if(msg_p->strVp[0] != 0)
        {
            strVp = allocStringBuf(msg_p->strVp, OS_StrLen(msg_p->strVp));
            if(!strVp)
            {
                status = BT_STATUS_NO_RESOURCES;
                goto exit;
            }
            textModeParams.strVp = strVp;
        }
        else
        {
            textModeParams.strVp = NULL;
            textModeParams.intVp = msg_p->intVp;
        }

        textModeParams.fo = msg_p->fo;
        textModeParams.pid = msg_p->pid;
        textModeParams.dcs = msg_p->dcs;
        status = hfg_send_sms_text_mode_params_rsp(&adp_channel->channel, &textModeParams, response);
    }
    else
    {
        adp_channel = NULL;
    }

exit:
    if(status != BT_STATUS_PENDING)
    {
        if(strVp)
            free_ctrl_buffer(strVp);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_TEXT_MODE_PARAMS_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_TEXT_MODE_PARAMS_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_TEXT_MODE_PARAMS_CNF;
    }
}

/* AT_SMS_SHOW_TEXT_MODE|AT_TEST */
/* MSG_ID_BT_HFG_SUPPORTED_SHOW_PARAMS_REQ */
void hfg_adp_hfg_handle_supported_show_params_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_supported_show_text_mode_params_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel   *adp_channel;
    HfgResponse         *response = NULL;
    char* supportedShow = NULL;

    msg_p = (bt_hfg_supported_show_text_mode_params_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        if(msg_p->supportedShowLen)
        {
            supportedShow = allocStringBuf(msg_p->supportedShow, msg_p->supportedShowLen);
            if(supportedShow)
            {
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                {
                    status = hfg_send_sms_supported_show_params_rsp(&adp_channel->channel, supportedShow, response);
                }
                else
                {
                    status = BT_STATUS_NO_RESOURCES;
                }
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(supportedShow)
            free_ctrl_buffer(supportedShow);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SUPPORTED_SHOW_PARAMS_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SUPPORTED_SHOW_PARAMS_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SUPPORTED_SHOW_PARAMS_CNF;
    }
}

/* AT_SMS_SHOW_TEXT_MODE|AT_READ */
/* MSG_ID_BT_HFG_SELECTED_SHOW_PARAMS_REQ */
void hfg_adp_hfg_handle_selected_show_params_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_selected_show_text_mode_params_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel   *adp_channel;
    HfgResponse         *response = NULL;

    msg_p = (bt_hfg_selected_show_text_mode_params_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
        {
            status = hfg_send_sms_selected_show_params_rsp(&adp_channel->channel, msg_p->show, response);
        }
        else
        {
            status = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SELECTED_SHOW_PARAMS_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SELECTED_SHOW_PARAMS_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SELECTED_SHOW_PARAMS_CNF;
    }
}

/* AT_NEW_MESSAGE_INDICATION|AT_TEST */
/* MSG_ID_BT_HFG_SUPPORTED_NEW_MSG_INDICATION_REQ */
void hfg_adp_hfg_handle_supported_new_msg_indication_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_supported_new_msg_indication_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel   *adp_channel;
    HfgResponse         *response = NULL;
    char* supportedNewMsgInd = NULL;

    msg_p = (bt_hfg_supported_new_msg_indication_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        if(msg_p->supportedNewMsgIndLen)
        {
            supportedNewMsgInd = allocStringBuf(msg_p->supportedNewMsgInd, msg_p->supportedNewMsgIndLen);
            if(supportedNewMsgInd)
            {
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                {
                    status = hfg_send_sms_supported_new_msg_indication_rsp(&adp_channel->channel, supportedNewMsgInd, response);
                }
                else
                {
                    status = BT_STATUS_NO_RESOURCES;
                }
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(supportedNewMsgInd)
            free_ctrl_buffer(supportedNewMsgInd);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SUPPORTED_NEW_MSG_INDICATION_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SUPPORTED_NEW_MSG_INDICATION_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SUPPORTED_NEW_MSG_INDICATION_CNF;
    }
}

/* AT_NEW_MESSAGE_INDICATION|AT_READ */
/* MSG_ID_BT_HFG_SELECTED_NEW_MSG_INDICATION_REQ */
void hfg_adp_hfg_handle_selected_new_msg_indication_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_selected_new_msg_indication_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel       *adp_channel;
    HfgResponse             *response = NULL;
    HfgSMSIndSetting_read   setting;

    msg_p = (bt_hfg_selected_new_msg_indication_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
        {
            setting.mode = msg_p->mode;
            setting.mt = msg_p->mt;
            setting.bm = msg_p->bm;
            setting.ds = msg_p->ds;
            setting.bfr = msg_p->bfr;
            status = hfg_send_sms_selected_new_msg_indication_rsp(&adp_channel->channel, &setting, response);
        }
        else
        {
            status = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SELECTED_NEW_MSG_INDICATION_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SELECTED_NEW_MSG_INDICATION_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SELECTED_NEW_MSG_INDICATION_CNF;
    }
}

/* AT_SMS_DELIVER_INDICATION */
/* MSG_ID_BT_HFG_NEW_MSG_INDICATION_REQ */
void hfg_adp_hfg_handle_new_msg_indication_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_new_msg_indication_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel       *adp_channel;
    HfgResponse             *response = NULL;
    HfgSMSNewMsgInd     newMsg;

    msg_p = (bt_hfg_new_msg_indication_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
        {
            newMsg.mem = msg_p->mem;
            newMsg.index = msg_p->index;
            status = hfg_send_sms_new_msg_indication(&adp_channel->channel, &newMsg, response);
        }
        else
        {
            status = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SUPPORTED_LIST_STATUS_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SUPPORTED_LIST_STATUS_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SUPPORTED_LIST_STATUS_CNF;
    }
}

/* AT_LIST_MESSAGES|AT_TEST */
/* MSG_ID_BT_HFG_SUPPORTED_LIST_STATUS_REQ */
void hfg_adp_hfg_handle_supported_list_status_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_supported_list_status_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel   *adp_channel;
    HfgResponse         *response = NULL;
    char* supportedListStatus = NULL;

    msg_p = (bt_hfg_supported_list_status_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        if(msg_p->supportedListStatusLen)
        {
            supportedListStatus = allocStringBuf(msg_p->supportedListStatus, msg_p->supportedListStatusLen);
            if(supportedListStatus)
            {
                response = hfg_alloc_rsp(adp_channel);
                if(response != NULL)
                {
                    status = hfg_send_sms_supported_list_status_rsp(&adp_channel->channel, supportedListStatus, response);
                }
                else
                {
                    status = BT_STATUS_NO_RESOURCES;
                }
            }
            else
            {
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            status = BT_STATUS_INVALID_PARM;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(supportedListStatus)
            free_ctrl_buffer(supportedListStatus);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SUPPORTED_LIST_STATUS_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SUPPORTED_LIST_STATUS_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SUPPORTED_LIST_STATUS_CNF;
    }
}

/* AT_LIST_MESSAGES */
/* MSG_ID_BT_HFG_LIST_MSG_REQ */
void hfg_adp_hfg_handle_list_msg_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_list_msg_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel       *adp_channel;
    HfgResponse             *response = NULL;
    HfgSMSList_result       list;
    char *addr = NULL, *alpha = NULL, *SCTimeStamp = NULL, *data = NULL, *dt = NULL;

    msg_p = (bt_hfg_list_msg_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(!response)
        {
            status = BT_STATUS_NO_RESOURCES;
            goto exit;
        }
        OS_MemSet((U8*)&list, 0, sizeof(HfgSMSList_result));
        list.type = msg_p->type;
        list.index = msg_p->index;
        list.stat = msg_p->stat;
        switch(list.type)
        {
        case HFG_SMS_TYPE_SUBMIT:
        case HFG_SMS_TYPE_DELIVER:
            list.msg.deliver.mask = msg_p->msg.deliver.mask;
            if(msg_p->msg.deliver.addr[0] == 0)
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
            addr = allocStringBuf(msg_p->msg.deliver.addr, OS_StrLen(msg_p->msg.deliver.addr));
            if(!addr)
            {
                status = BT_STATUS_NO_RESOURCES;
                goto exit;
            }
            list.msg.deliver.addr = addr;
            list.msg.deliver.addrType = msg_p->msg.deliver.addrType;
            if(list.msg.deliver.mask&HFG_SMS_MASK_ALPHA)
            {
                if(msg_p->msg.deliver.alpha[0] != 0)
                {
                    alpha = allocStringBuf(msg_p->msg.deliver.alpha, OS_StrLen(msg_p->msg.deliver.alpha) );
                    if(!alpha)
                    {
                        status = BT_STATUS_NO_RESOURCES;
                        goto exit;
                    }
                    list.msg.deliver.alpha = alpha;
                }
                else
                {
                    list.msg.deliver.mask &= ~HFG_SMS_MASK_ALPHA;
                    list.msg.deliver.alpha = NULL;
                }
            }
            if(list.msg.deliver.mask&HFG_SMS_MASK_SCTS)
            {
                if(msg_p->msg.deliver.SCTimeStamp[0] != 0)
                {
                    SCTimeStamp = allocStringBuf(msg_p->msg.deliver.SCTimeStamp, OS_StrLen(msg_p->msg.deliver.SCTimeStamp) );
                    if(!SCTimeStamp)
                    {
                        status = BT_STATUS_NO_RESOURCES;
                        goto exit;
                    }
                    list.msg.deliver.SCTimeStamp = SCTimeStamp;
                }
                else
    {
                    list.msg.deliver.mask &= ~HFG_SMS_MASK_SCTS;
                    list.msg.deliver.SCTimeStamp = NULL;
                }
            }
            if(msg_p->msg.deliver.data[0] != 0)
            {
                data = allocStringBuf(msg_p->msg.deliver.data, OS_StrLen(msg_p->msg.deliver.data) );
                if(!data)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                list.msg.deliver.data = data;
            }
            else
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
            list.msg.deliver.length = msg_p->msg.deliver.length;
        break;
        case HFG_SMS_TYPE_STATUS:
            list.msg.status.mask = msg_p->msg.status.mask;
            list.msg.status.fo = msg_p->msg.status.fo;
            list.msg.status.mr = msg_p->msg.status.mr;
            if(list.msg.status.mask&HFG_SMS_MASK_RA)
            {
                if(msg_p->msg.status.ra[0] != 0)
                {
                    addr = allocStringBuf(msg_p->msg.status.ra, OS_StrLen(msg_p->msg.status.ra));
                    if(!addr)
                    {
                        status = BT_STATUS_NO_RESOURCES;
                        goto exit;
                    }
                    list.msg.status.ra = addr;
                }
                else
                {
                    list.msg.status.mask &= ~HFG_SMS_MASK_RA;
                    list.msg.status.ra = NULL;
                }
            }
            list.msg.status.addrType = msg_p->msg.status.addrType;
            if(msg_p->msg.status.SCTimeStamp[0] != 0)
            {
                SCTimeStamp = allocStringBuf(msg_p->msg.status.SCTimeStamp, OS_StrLen(msg_p->msg.status.SCTimeStamp) );
                if(!SCTimeStamp)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                list.msg.status.SCTimeStamp = SCTimeStamp;
            }
            else
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
            if(msg_p->msg.status.dt[0] != 0)
            {
                dt = allocStringBuf(msg_p->msg.status.dt, OS_StrLen(msg_p->msg.status.dt) );
                if(!dt)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                list.msg.status.dt = dt;
            }
            else
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
            list.msg.status.st= msg_p->msg.status.st;
        break;
        case HFG_SMS_TYPE_COMMAND:
            list.msg.cmd.fo = msg_p->msg.command.fo;
            list.msg.cmd.ct = msg_p->msg.command.ct;
        break;
        case HFG_SMS_TYPE_CBM:
            list.msg.cbm.sn = msg_p->msg.cbm.sn;
            list.msg.cbm.mid = msg_p->msg.cbm.mid;
            list.msg.cbm.page = msg_p->msg.cbm.page;
            list.msg.cbm.dcs = msg_p->msg.cbm.dcs;
            if(msg_p->msg.cbm.data[0] != 0)
            {
                data = allocStringBuf(msg_p->msg.cbm.data, OS_StrLen(msg_p->msg.cbm.data) );
                if(!data)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                list.msg.cbm.data = data;
            }
            else
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
        break;
        case HFG_SMS_TYPE_PDU:
            list.msg.pdu.mask = msg_p->msg.pdu.mask;
            if(list.msg.pdu.mask&HFG_SMS_MASK_ALPHA)
            {
                if(msg_p->msg.pdu.alpha[0] != 0)
                {
                    alpha = allocStringBuf(msg_p->msg.pdu.alpha, OS_StrLen(msg_p->msg.pdu.alpha) );
                    if(!alpha)
                    {
                        status = BT_STATUS_NO_RESOURCES;
                        goto exit;
    }
                    list.msg.pdu.alpha = alpha;
}
                else
                {
                    list.msg.pdu.mask &= ~HFG_SMS_MASK_ALPHA;
                    list.msg.pdu.alpha = NULL;
                }
            }
            list.msg.pdu.length = msg_p->msg.pdu.length;
            if(msg_p->msg.pdu.data[0] != 0)
            {
                data = allocStringBuf(msg_p->msg.pdu.data, OS_StrLen(msg_p->msg.pdu.data) );
                if(!data)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                list.msg.pdu.PDU = data;
            }
            else
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
            break;
        default:
            status = BT_STATUS_INVALID_PARM;
            goto exit;
            break;
        }
        status = hfg_send_sms_list_msg_rsp(&adp_channel->channel, &list, response, msg_p->bFinal);
    }
    else
    {
        adp_channel = NULL;
    }
exit:
    if(status != BT_STATUS_PENDING)
    {
        if(addr)
            free_ctrl_buffer(addr);
        if(alpha)
            free_ctrl_buffer(alpha);
        if(SCTimeStamp)
            free_ctrl_buffer(SCTimeStamp);
        if(data)
            free_ctrl_buffer(data);
        if(dt)
            free_ctrl_buffer(dt);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_LIST_MSG_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_LIST_MSG_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_LIST_MSG_CNF;
    }
}

/* AT_READ_MESSAGE */
/* MSG_ID_BT_HFG_READ_MSG_REQ */
void hfg_adp_hfg_handle_read_msg_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_read_msg_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse         *response = NULL;
    HfgSMSRead_result       readMsg;
    char *addr = NULL, *alpha = NULL, *SCTimeStamp = NULL, *SCAddr = NULL, *strVp = NULL, *data = NULL, *dt = NULL;

    msg_p = (bt_hfg_read_msg_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(!response)
        {
            status = BT_STATUS_NO_RESOURCES;
            goto exit;
        }
        OS_MemSet((U8*)&readMsg, 0, sizeof(HfgSMSRead_result));
        readMsg.type = msg_p->type;
        readMsg.stat = msg_p->stat;
        switch(readMsg.type)
        {
        case HFG_SMS_TYPE_DELIVER:
            readMsg.msg.deliver.mask = msg_p->msg.deliver.mask;
            if(msg_p->msg.deliver.addr[0] == 0)
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
            addr = allocStringBuf(msg_p->msg.deliver.addr, OS_StrLen(msg_p->msg.deliver.addr));
            if(!addr)
            {
                status = BT_STATUS_NO_RESOURCES;
                goto exit;
            }
            readMsg.msg.deliver.addr = addr;
            readMsg.msg.deliver.addrType = msg_p->msg.deliver.addrType;
            if(readMsg.msg.deliver.mask&HFG_SMS_MASK_ALPHA)
            {
                if(msg_p->msg.deliver.alpha[0] != 0)
                {
                    alpha = allocStringBuf(msg_p->msg.deliver.alpha, OS_StrLen(msg_p->msg.deliver.alpha) );
                    if(!alpha)
                    {
                        status = BT_STATUS_NO_RESOURCES;
                        goto exit;
                    }
                    readMsg.msg.deliver.alpha = alpha;
                }
                else
                {
                    readMsg.msg.deliver.mask |= HFG_SMS_MASK_ALPHA;
                    readMsg.msg.deliver.alpha = NULL;
                }
            }
            if(msg_p->msg.deliver.SCTimeStamp[0] != 0)
            {
                SCTimeStamp = allocStringBuf(msg_p->msg.deliver.SCTimeStamp, OS_StrLen(msg_p->msg.deliver.SCTimeStamp) );
                if(!SCTimeStamp)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                readMsg.msg.deliver.SCTimeStamp = SCTimeStamp;
            }
            else
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
            
            readMsg.msg.deliver.fo = msg_p->msg.deliver.fo;
            readMsg.msg.deliver.pid = msg_p->msg.deliver.pid;
            readMsg.msg.deliver.dcs = msg_p->msg.deliver.dcs;

            if(readMsg.msg.deliver.mask&HFG_SMS_MASK_SCA)
            {
                if(msg_p->msg.deliver.SCAddr[0] != 0)
                {
                    SCAddr = allocStringBuf(msg_p->msg.deliver.SCAddr, OS_StrLen(msg_p->msg.deliver.SCAddr) );
                    if(!SCAddr)
                    {
                        status = BT_STATUS_NO_RESOURCES;
                        goto exit;
                    }
                    readMsg.msg.deliver.SCAddr = SCAddr;
                }
                else
                {
                    readMsg.msg.deliver.SCAddr = NULL;
                }
            }
            
            readMsg.msg.deliver.SCAddrType = msg_p->msg.deliver.SCAddrType;

            if(msg_p->msg.deliver.data[0] != 0)
            {
                data = allocStringBuf(msg_p->msg.deliver.data, OS_StrLen(msg_p->msg.deliver.data) );
                if(!data)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                readMsg.msg.deliver.data = data;
            }
            else
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
            readMsg.msg.deliver.length = msg_p->msg.deliver.length;
            break;
        case HFG_SMS_TYPE_SUBMIT:
            readMsg.msg.submit.mask = msg_p->msg.submit.mask;
            if(msg_p->msg.submit.addr[0] == 0)
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
            addr = allocStringBuf(msg_p->msg.submit.addr, OS_StrLen(msg_p->msg.submit.addr));
            if(!addr)
            {
                status = BT_STATUS_NO_RESOURCES;
                goto exit;
            }
            readMsg.msg.submit.addr = addr;
            readMsg.msg.submit.addrType = msg_p->msg.submit.addrType;
            if(readMsg.msg.submit.mask&HFG_SMS_MASK_ALPHA)
            {
                if(msg_p->msg.submit.alpha[0] != 0)
                {
                    alpha = allocStringBuf(msg_p->msg.submit.alpha, OS_StrLen(msg_p->msg.submit.alpha) );
                    if(!alpha)
                    {
                        status = BT_STATUS_NO_RESOURCES;
                        goto exit;
                    }
                    readMsg.msg.submit.alpha = alpha;
                }
                else
                {
                    readMsg.msg.submit.mask |= HFG_SMS_MASK_ALPHA;
                    readMsg.msg.submit.alpha = NULL;
                }
            }
            
            readMsg.msg.submit.fo = msg_p->msg.submit.fo;
            readMsg.msg.submit.pid = msg_p->msg.submit.pid;
            readMsg.msg.submit.dcs = msg_p->msg.submit.dcs;

            if(readMsg.msg.submit.mask&HFG_SMS_MASK_VP)
            {
                if(msg_p->msg.submit.strVp[0] != 0)
                {
                    strVp = allocStringBuf(msg_p->msg.submit.strVp, OS_StrLen(msg_p->msg.submit.strVp) );
                    if(!strVp)
                    {
                        status = BT_STATUS_NO_RESOURCES;
                        goto exit;
                    }
                    readMsg.msg.submit.strVp = strVp;
                }
                else
                {
                    readMsg.msg.submit.strVp = NULL;
                    readMsg.msg.submit.intVp = msg_p->msg.submit.intVp;
                }
            }


            if(readMsg.msg.submit.mask&HFG_SMS_MASK_SCA)
            {
                if(msg_p->msg.submit.SCAddr[0] != 0)
                {
                    SCAddr = allocStringBuf(msg_p->msg.submit.SCAddr, OS_StrLen(msg_p->msg.submit.SCAddr) );
                    if(!SCAddr)
                    {
                        status = BT_STATUS_NO_RESOURCES;
                        goto exit;
                    }
                    readMsg.msg.submit.SCAddr = SCAddr;
                }
                else
                {
                    readMsg.msg.submit.mask &= ~HFG_SMS_MASK_SCA;
                    readMsg.msg.submit.SCAddr = NULL;
                }
            }
            
            readMsg.msg.submit.SCAddrType = msg_p->msg.submit.SCAddrType;

            if(msg_p->msg.submit.data[0] != 0)
            {
                data = allocStringBuf(msg_p->msg.submit.data, OS_StrLen(msg_p->msg.submit.data) );
                if(!data)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                readMsg.msg.submit.data = data;
            }
            else
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
            readMsg.msg.submit.length = msg_p->msg.submit.length;
            break;
        case HFG_SMS_TYPE_STATUS:
            readMsg.msg.status.mask = msg_p->msg.status.mask;
            readMsg.msg.status.fo = msg_p->msg.status.fo;
            readMsg.msg.status.mr = msg_p->msg.status.mr;
            if(readMsg.msg.status.mask&HFG_SMS_MASK_RA)
            {
                if(msg_p->msg.status.ra[0] != 0)
                {
                    addr = allocStringBuf(msg_p->msg.status.ra, OS_StrLen(msg_p->msg.status.ra));
                    if(!addr)
                    {
                        status = BT_STATUS_NO_RESOURCES;
                        goto exit;
                    }
                    readMsg.msg.status.ra = addr;
                }
                else
                {
                    readMsg.msg.status.mask &= ~HFG_SMS_MASK_RA;
                    readMsg.msg.status.ra = NULL;
                }
            }
            readMsg.msg.status.addrType = msg_p->msg.status.addrType;
            if(msg_p->msg.status.SCTimeStamp[0] != 0)
            {
                SCTimeStamp = allocStringBuf(msg_p->msg.status.SCTimeStamp, OS_StrLen(msg_p->msg.status.SCTimeStamp) );
                if(!SCTimeStamp)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                readMsg.msg.status.SCTimeStamp = SCTimeStamp;
            }
            else
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
            if(msg_p->msg.status.dt[0] != 0)
            {
                dt = allocStringBuf(msg_p->msg.status.dt, OS_StrLen(msg_p->msg.status.dt) );
                if(!dt)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                readMsg.msg.status.dt = dt;
            }
            else
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
            readMsg.msg.status.st= msg_p->msg.status.st;
            break;
        case HFG_SMS_TYPE_COMMAND:
            readMsg.msg.cmd.mask = msg_p->msg.command.mask;
            readMsg.msg.cmd.fo = msg_p->msg.command.fo;
            readMsg.msg.cmd.ct = msg_p->msg.command.ct;
            readMsg.msg.cmd.pid = msg_p->msg.command.pid;
            readMsg.msg.cmd.mn = msg_p->msg.command.mn;
            if( msg_p->msg.command.mask & HFG_SMS_MASK_ADDR &&
                 msg_p->msg.command.destAddr[0] != 0)
            {
                addr = allocStringBuf(msg_p->msg.command.destAddr, OS_StrLen(msg_p->msg.command.destAddr));
                if(!addr)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                readMsg.msg.cmd.destAddr = addr;
            }
            else
            {
                msg_p->msg.command.mask &= ~HFG_SMS_MASK_ADDR;
                msg_p->msg.command.mask &= ~HFG_SMS_MASK_TOADDR;
                readMsg.msg.cmd.destAddr = NULL;
            }

            readMsg.msg.cmd.addrType = msg_p->msg.command.addrType;
            if(readMsg.msg.cmd.mask&HFG_SMS_MASK_CDATA)
            {
                if(msg_p->msg.command.cmdData[0] != 0)
                {
                    data = allocStringBuf(msg_p->msg.command.cmdData, OS_StrLen(msg_p->msg.command.cmdData));
                    if(!data)
                    {
                        status = BT_STATUS_NO_RESOURCES;
                        goto exit;
                    }
                    readMsg.msg.cmd.cmdData = data;
                }
                else
                {
                    readMsg.msg.cmd.mask &= ~HFG_SMS_MASK_CDATA;
                    readMsg.msg.cmd.cmdData = NULL;
                }
            }
            readMsg.msg.cmd.length = msg_p->msg.command.length;
        break;
        case HFG_SMS_TYPE_CBM:
            readMsg.msg.cbm.sn = msg_p->msg.cbm.sn;
            readMsg.msg.cbm.mid = msg_p->msg.cbm.mid;
            readMsg.msg.cbm.dcs = msg_p->msg.cbm.dcs;
            readMsg.msg.cbm.page = msg_p->msg.cbm.page;
            if(msg_p->msg.cbm.data[0] != 0)
            {
                data = allocStringBuf(msg_p->msg.cbm.data, OS_StrLen(msg_p->msg.cbm.data) );
                if(!data)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                readMsg.msg.cbm.data = data;
            }
            else
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
        break;
        case HFG_SMS_TYPE_PDU:
            readMsg.msg.pdu.mask = msg_p->msg.pdu.mask;
            if(readMsg.msg.pdu.mask&HFG_SMS_MASK_ALPHA)
            {
                if(msg_p->msg.pdu.alpha[0] != 0)
                {
                    alpha = allocStringBuf(msg_p->msg.pdu.alpha, OS_StrLen(msg_p->msg.pdu.alpha) );
                    if(!alpha)
                    {
                        status = BT_STATUS_NO_RESOURCES;
                        goto exit;
                    }
                    readMsg.msg.pdu.alpha = alpha;
                }
                else
                {
                    readMsg.msg.pdu.mask &= ~HFG_SMS_MASK_ALPHA;
                    readMsg.msg.pdu.alpha = NULL;
                }
            }
            if(msg_p->msg.pdu.data[0] != 0)
            {
                data = allocStringBuf(msg_p->msg.pdu.data, OS_StrLen(msg_p->msg.pdu.data) );
                if(!data)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                readMsg.msg.pdu.PDU = data;
            }
            else
            {
                status = BT_STATUS_INVALID_PARM;
                goto exit;
            }
            readMsg.msg.pdu.length = msg_p->msg.pdu.length;
        break;
        default:
            status = BT_STATUS_INVALID_PARM;
            goto exit;
        break;
        }
        status = hfg_send_sms_read_msg_rsp(&adp_channel->channel, &readMsg, response);
    }
    else
    {
        adp_channel = NULL;
    }
exit:
    if(status != BT_STATUS_PENDING)
    {
        if(addr)
            free_ctrl_buffer(addr);
        if(alpha)
            free_ctrl_buffer(alpha);
        if(SCTimeStamp)
            free_ctrl_buffer(SCTimeStamp);
        if(data)
            free_ctrl_buffer(data);
        if(SCAddr)
            free_ctrl_buffer(SCAddr);
        if(dt)
            free_ctrl_buffer(dt);
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);
    /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_READ_MSG_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_READ_MSG_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_READ_MSG_CNF;
    }
}

/* AT_SEND_MESSAGE */
/* MSG_ID_BT_HFG_SEND_MSG_REQ */
void hfg_adp_hfg_handle_send_msg_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_send_msg_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel       *adp_channel;
    HfgResponse             *response = NULL;
    char* ptr = NULL;
    HfgSMSSend_result       sendMsg;

    msg_p = (bt_hfg_send_msg_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;
    
    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        sendMsg.mask = msg_p->mask;
        sendMsg.mr = msg_p->mr;
        sendMsg.scts = NULL;
        sendMsg.ackpdu = NULL;
        if(sendMsg.mask & HFG_SMS_MASK_SCTS)
        {
            if(msg_p->scts[0] != 0)
            {
                ptr = allocStringBuf(msg_p->scts, OS_StrLen(msg_p->scts));
                if(!ptr)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                sendMsg.scts = ptr;
            }
            else
            {
                sendMsg.mask &= ~HFG_SMS_MASK_SCTS;
                sendMsg.scts = NULL;
            }
        }
        else    if(sendMsg.mask & HFG_SMS_MASK_ACKPDU)
        {
            if(msg_p->ackpdu[0] != 0)
            {
                ptr = allocStringBuf(msg_p->ackpdu, OS_StrLen(msg_p->ackpdu));
                if(!ptr)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                sendMsg.ackpdu = ptr;
            }
            else
            {
                sendMsg.mask &= ~HFG_SMS_MASK_ACKPDU;
                sendMsg.ackpdu = NULL;
            }
        }
    }
    else
    {
        adp_channel = NULL;
    }
    response = hfg_alloc_rsp(adp_channel);
    if(response != NULL)
    {
        status = hfg_send_sms_send_msg_rsp(&adp_channel->channel, &sendMsg, response);
    }
    else
    {
        status = BT_STATUS_NO_RESOURCES;
    }
exit:
    if(status != BT_STATUS_PENDING)
    {
        if(ptr)
            free_ctrl_buffer(ptr);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SEND_MSG_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SEND_MSG_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SEND_MSG_CNF;
    }
}

/* AT_SEND_STORED_MESSAGE */
/* MSG_ID_BT_HFG_SEND_STORED_MSG_REQ */
void hfg_adp_hfg_handle_send_stored_msg_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_send_stored_msg_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel           *adp_channel;
    HfgResponse                 *response = NULL;
    char* ptr = NULL;
    HfgSMSSendStored_result sendMsg;

    msg_p = (bt_hfg_send_stored_msg_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        sendMsg.mask = msg_p->mask;
        sendMsg.mr = msg_p->mr;
        sendMsg.scts = NULL;
        sendMsg.ackpdu = NULL;
        if(sendMsg.mask & HFG_SMS_MASK_SCTS)
        {
            if(msg_p->scts[0] != 0)
            {
                ptr = allocStringBuf(msg_p->scts, OS_StrLen(msg_p->scts));
                if(!ptr)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                sendMsg.scts = ptr;
            }
            else
            {
                sendMsg.mask &= ~HFG_SMS_MASK_SCTS;
                sendMsg.scts = NULL;
            }
        }
        else    if(sendMsg.mask & HFG_SMS_MASK_ACKPDU)
        {
            if(msg_p->ackpdu[0] != 0)
            {
                ptr = allocStringBuf(msg_p->ackpdu, OS_StrLen(msg_p->ackpdu));
                if(!ptr)
                {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
                sendMsg.ackpdu = ptr;
            }
            else
            {
                sendMsg.mask &= ~HFG_SMS_MASK_ACKPDU;
                sendMsg.ackpdu = NULL;
            }
        }
    }
    else
    {
        adp_channel = NULL;
    }
    response = hfg_alloc_rsp(adp_channel);
    if(response != NULL)
    {
        status = hfg_send_sms_send_stored_msg_rsp(&adp_channel->channel, &sendMsg, response);
    }
    else
    {
        status = BT_STATUS_NO_RESOURCES;
    }
exit:
    if(status != BT_STATUS_PENDING)
    {
        if(ptr)
            free_ctrl_buffer(ptr);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SEND_STORED_MSG_CNF, (local_para_struct*)cnf_msg_p);         
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SEND_STORED_MSG_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SEND_STORED_MSG_CNF;
    }
}

/* AT_STORE_MESSAGE */
/* MSG_ID_BT_HFG_WRITE_MSG_REQ */
void hfg_adp_hfg_handle_write_msg_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_write_msg_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel   *adp_channel;
    HfgResponse         *response = NULL;

    msg_p = (bt_hfg_write_msg_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
        {
            status = hfg_send_sms_write_msg_rsp(&adp_channel->channel, msg_p->index, response);
        }
        else
        {
            status = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
                    hfg_free_rsp(adp_channel, response);
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_WRITE_MSG_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_WRITE_MSG_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_WRITE_MSG_CNF;
    }
}

/*****************************************************************************
 * FUNCTION
 *  hfg_adp_hfg_handle_sms_error_req
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_adp_hfg_handle_sms_error_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_sms_error_req_struct *msg_p;
    bt_hfg_req_complete_cnf_struct *cnf_msg_p;
    HfgAdpChannel *adp_channel;
    HfgResponse *response = NULL;

    msg_p = (bt_hfg_sms_error_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        response = hfg_alloc_rsp(adp_channel);
        if(response != NULL)
            status = hfg_send_sms_error(&adp_channel->channel, msg_p->error, response);
        else
            status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        adp_channel = NULL;
    }

    if(status != BT_STATUS_PENDING)
    {
        if(response != NULL)
            hfg_free_rsp(adp_channel, response);  
        /* construct return message */
        cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
        cnf_msg_p->pContext = adp_channel;
        cnf_msg_p->req_context = msg_p->req_context;
        cnf_msg_p->result = status;
        if(adp_channel != NULL)
        {
            hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SMS_ERROR_CNF, (local_para_struct*)cnf_msg_p);        
        }
        else
        {
            hfg_sendMsg(MSG_ID_BT_HFG_SMS_ERROR_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
        }
    }
    else
    {
        HfgAdpResponse *adp_response = ContainingRecord(response, HfgAdpResponse, response);
        adp_response->req_context = msg_p->req_context;
        adp_response->req_msg = MSG_ID_BT_HFG_SMS_ERROR_CNF;
    }
}
#endif /* AT_SMS == XA_ENABLED */

#ifdef BT_HFG_UT_TEST
/* MSG_ID_BT_HFG_UT_INIT_REQ */
void hfg_adp_hfg_handle_ut_init_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_ut_init_req_struct *msg_p;
    bt_hfg_ut_init_cnf_struct *cnf_msg_p;
    HfgAdpChannel   *adp_channel;

    msg_p = (bt_hfg_ut_init_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        /* Init UT */
        adp_channel->channel.linkFlags = HFG_LINK_ACL|HFG_LINK_HANDSFREE;
        adp_channel->channel.flags = 0;
        adp_channel->channel.hfFeatures = 481;
        adp_channel->channel.version = 0x0105;
        adp_channel->channel.rfServerChannel = 2;
        adp_channel->channel.state = HFG_SUBSTATE_SLC;
        adp_channel->channel.indEnabled = TRUE;
        adp_channel->channel.service = 0;
        adp_channel->channel.call = 0;
        adp_channel->channel.callSetup = 0;
        adp_channel->channel.held = 0;
        adp_channel->channel.battery = 3;
        adp_channel->channel.signal = 3;
        adp_channel->channel.roaming = 0;
        
        HfgSetupIndicatorTestRsp(&adp_channel->channel);
        adp_channel->channel.atResults.type = 0;
    }
    else
    {
        adp_channel = NULL;
    }

    /* construct return message */
    cnf_msg_p = (bt_hfg_ut_init_cnf_struct*) construct_local_para(sizeof(bt_hfg_ut_init_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;
    cnf_msg_p->result = status;
    if(adp_channel != NULL)
    {
        bt_hfg_connected_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_connected_ind_struct*) construct_local_para(sizeof(bt_hfg_connected_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        OS_MemCopy(ind_msg_p->bt_addr.addr, "123456", 6);

        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_UT_INIT_CNF, (local_para_struct*)cnf_msg_p);        
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_CONNECTED_IND, (local_para_struct*)ind_msg_p);
    }
    else
    {
        hfg_sendMsg(MSG_ID_BT_HFG_UT_INIT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}

void hfg_adp_hfg_handle_ut_tx_req(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_ut_tx_req_struct *msg_p;
    bt_hfg_ut_tx_cnf_struct *cnf_msg_p;
    HfgAdpChannel   *adp_channel;

    msg_p = (bt_hfg_ut_tx_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        /* Handle UT tx data */
        RfCallbackParms parms;
        parms.status = BT_STATUS_SUCCESS;
        parms.event = RFEVENT_DATA_IND;
        parms.dataLen = msg_p->len;
        parms.ptrs.data = msg_p->buf;
        HfgRfCallback(adp_channel->channel.rfChannel, &parms);
    }
    else
    {
        adp_channel = NULL;
    }

    /* construct return message */
    cnf_msg_p = (bt_hfg_ut_tx_cnf_struct*) construct_local_para(sizeof(bt_hfg_ut_tx_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;
    cnf_msg_p->result = status;
    if(adp_channel != NULL)
    {
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_UT_TX_CNF, (local_para_struct*)cnf_msg_p);        
    }
    else
    {
        hfg_sendMsg(MSG_ID_BT_HFG_UT_TX_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}

void hfg_adp_hfg_handle_ut_rx_rsp(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_hfg_ut_rx_rsp_struct *msg_p;
    HfgAdpChannel   *adp_channel;

    msg_p = (bt_hfg_ut_rx_rsp_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    if (IsNodeOnList(&g_channel_list, &adp_channel->node)) 
    {
        /* Handle UT tx data */
        /* Call back to hfg to indicate the packet is sent complete */
        RfCallbackParms parms;
        parms.status = BT_STATUS_SUCCESS;
        parms.event = RFEVENT_PACKET_HANDLED;
        parms.ptrs.packet = &(adp_channel->channel.atTxPacket);
        HfgRfCallback(adp_channel->channel.rfChannel, &parms);
    }
    else
    {
        adp_channel = NULL;
    }
}

BtStatus hfg_adp_hfg_send_ut_rx_ind(HfgChannel *Channel, const char *buf, U16 len)
{
    bt_hfg_ut_rx_ind_struct *msg_p;
    HfgAdpChannel *adp_channel = ContainingRecord(Channel, HfgAdpChannel, channel);
    /* construct return message */
    msg_p = (bt_hfg_ut_rx_ind_struct*) construct_local_para(sizeof(bt_hfg_ut_rx_ind_struct), TD_UL);
    msg_p->pContext = (void*)adp_channel;
    msg_p->user_context = adp_channel->userContext;
    msg_p->len = len;
    OS_MemCopy(msg_p->buf, buf, len);
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_UT_RX_IND, (local_para_struct*)msg_p);
    return BT_STATUS_PENDING;
}
#endif

/*****************************************************************************
 * FUNCTION
 *  hfga_handler
 * DESCRIPTION
 *  This function is to handle HFP messages sent from other tasks
 * PARAMETERS
 *  ilm_ptr     [IN]  inter layer message     
 * RETURNS
 *  void
 *****************************************************************************/
void hfga_handler(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (ilm_ptr->msg_id)
    {
    case MSG_ID_BT_HFG_ACTIVATE_REQ:
        hfg_adp_activate_req_hdler(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_DEACTIVATE_REQ:
        hfg_adp_deactivate_req_hdler(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_CONNECT_REQ:
        hfg_adp_connect_req_hdler(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_DISCONNECT_REQ:
        hfg_adp_disconnect_req_hdler(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_SCO_CONNECT_REQ:
        hfg_adp_sco_connect_req_hdler(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_SCO_DISCONNECT_REQ:
        hfg_adp_sco_disconnect_req_hdler(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_ACCEPT_CHANNEL_REQ:
        hfg_adp_accept_channel_hdler(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_REJECT_CHANNEL_REQ:
        hfg_adp_reject_channel_hdler(ilm_ptr);
        break;

    /* OK */
    case MSG_ID_BT_HFG_OK_REQ:                                       
        hfg_adp_ok_req_hdler(ilm_ptr);
        break;
        
    /* ERROR or +CME ERROR : <err> */            
    case MSG_ID_BT_HFG_ERROR_REQ:                                    
        hfg_adp_error_req_hdler(ilm_ptr);
        break;

    /* NO CARRIER */            
    case MSG_ID_BT_HFG_NO_CARRIER_REQ:                           
        hfg_adp_no_carrier_req_hdler(ilm_ptr);
        break;

    /* BUSY */            
    case MSG_ID_BT_HFG_BUSY_REQ:                                     
        hfg_adp_busy_req_hdler(ilm_ptr);
        break;

    /* NO ANSWER */            
    case MSG_ID_BT_HFG_NO_ANSWER_REQ:                            
        hfg_adp_no_answer_req_hdler(ilm_ptr);
        break;

    /* DELAYED */            
    case MSG_ID_BT_HFG_DELAYED_REQ:                                 
        hfg_adp_delayed_req_hdler(ilm_ptr);
        break;

    /* BLACKLISTED */            
    case MSG_ID_BT_HFG_BLACKLISTED_REQ:                          
        hfg_adp_blacklisted_req_hdler(ilm_ptr);
        break;

    /* RING */            
    case MSG_ID_BT_HFG_RING_REQ:                                     
        hfg_adp_ring_req_hdler(ilm_ptr);            
        break;

    /* +CCWA:<number>,<type> */            
    case MSG_ID_BT_HFG_CALL_WAITING_REQ:                     
        hfg_adp_call_waiting_req_hdler(ilm_ptr);
        break;

    /* +CIND: <ind>[,<ind>[,...]] */            
    case MSG_ID_BT_HFG_READ_INDICATOR_REQ:                   
        hfg_adp_read_indicator_req_hdler(ilm_ptr);
        break;

    /* [+CLCC: <id1>,<dir>,<stat>,<mode>,<mpty>[,<number>,<type>] */            
    case MSG_ID_BT_HFG_CALL_LIST_REQ:                             
        hfg_adp_call_list_req_hdler(ilm_ptr);
        break;

    /* +CLIP:<number>,<type> */            
    case MSG_ID_BT_HFG_CALL_ID_REQ:                                
        hfg_adp_call_id_req_hdler(ilm_ptr);
        break;

    /* +CIEV: <ind>,<value> */            
    case MSG_ID_BT_HFG_INDICATOR_REPORT_REQ:             
        hfg_adp_indicator_report_req_hdler(ilm_ptr);
        break;

    /* +CNUM: [<alpha>],<number>, <type>,[<speed>] ,<service> */            
    case MSG_ID_BT_HFG_SUBSCRIBER_NUMBER_REQ:           
        hfg_adp_subscriber_number_req_hdler(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_OPERATOR_NAME_REQ:
        hfg_adp_operator_name_req_hdler(ilm_ptr);
        break;

    /* +BINP: <Phone number> */            
    case MSG_ID_BT_HFG_VOICE_TAG_REQ:                           
        hfg_adp_voice_tag_req_hdler(ilm_ptr);
        break;

    /* +BVRA: <vrect> */            
    case MSG_ID_BT_HFG_VOICE_REC_REQ:                            
        hfg_adp_voice_rec_req_hdler(ilm_ptr);
        break;

    /* +BTRH: <n> */             
    #if HFG_USE_RESP_HOLD == XA_ENABLED       
    case MSG_ID_BT_HFG_RESPONSE_HOLD_REQ:                     
        hfg_adp_response_hold_req_hdler(ilm_ptr);
        break;
    #endif
    
    case MSG_ID_BT_HFG_SPEAKER_GAIN_REQ:
        hfg_adp_speaker_gain_req_hdler(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_MIC_GAIN_REQ:
        hfg_adp_mic_gain_req_hdler(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_RINGTONE_STATUS_REQ:
        hfg_adp_ringtone_status_req_hdler(ilm_ptr);
        break;
        
     case MSG_ID_BT_HFG_SUPPORTED_CHARSET_REQ:
        hfg_adp_supported_charset_req_hdler(ilm_ptr);
        break;
        
     case MSG_ID_BT_HFG_SELECTED_CHARSET_REQ:
        hfg_adp_selected_charset_req_hdler(ilm_ptr);
        break;
        
     case MSG_ID_BT_HFG_MODEL_ID_REQ:
        hfg_adp_model_id_req_hdler(ilm_ptr);
        break;
        
     case MSG_ID_BT_HFG_MANUFACTURE_ID_REQ:
        hfg_adp_manufacture_id_req_hdler(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_SEND_DATA_REQ:
        hfg_adp_send_raw_req_hdler(ilm_ptr);
        break;
        
    /********************************************
    *   Phonebook related
    ********************************************/
#if AT_PHONEBOOK == XA_ENABLED
    case MSG_ID_BT_HFG_SUPPORTED_PHONEBOOK_REQ:
        hfg_adp_supported_phonebook_req_hdler(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_SELECTED_PHONEBOOK_REQ:
        hfg_adp_selected_phonebook_req_hdler(ilm_ptr);
        break;

    case MSG_ID_BT_HFG_READ_PHONEBOOK_INFO_REQ:
        hfg_adp_read_phonebook_info_req_hdler(ilm_ptr);
        break;

    case MSG_ID_BT_HFG_READ_PHONEBOOK_REQ:
        hfg_adp_read_phonebook_req_hdler(ilm_ptr);
        break;

    case MSG_ID_BT_HFG_FIND_PHONEBOOK_INFO_REQ:
        hfg_adp_find_phonebook_info_req_hdler(ilm_ptr);
        break;

    case MSG_ID_BT_HFG_FIND_PHONEBOOK_REQ:
        hfg_adp_find_phonebook_req_hdler(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_WRITE_PHONEBOOK_INFO_REQ:
        hfg_adp_write_phonebook_info_req_hdler(ilm_ptr);
        break;
#endif /* AT_PHONEBOOK == XA_ENABLED */    
    /********************************************
    *   SMS related
    ********************************************/
#if AT_SMS == XA_ENABLED    
    /* AT_SELECT_SMS_SERVICE|AT_TEST */
    case MSG_ID_BT_HFG_SUPPORTED_SMS_SERVICE_REQ:
        hfg_adp_hfg_handle_supported_sms_service_req(ilm_ptr);
        break;
                
    /* AT_SELECT_SMS_SERVICE|AT_READ */
    case MSG_ID_BT_HFG_SELECTED_SMS_SERVICE_REQ:
        hfg_adp_hfg_handle_selected_sms_service_req(ilm_ptr);
        break;
        
    /* AT_SELECT_SMS_SERVICE */
    case MSG_ID_BT_HFG_SMS_SERVICE_REQ:
        hfg_adp_hfg_handle_sms_service_req(ilm_ptr);
        break;

    /* AT_PREFERRED_SMS_STORAGE|AT_TEST */
    case MSG_ID_BT_HFG_SUPPORTED_PREF_MSG_STORAGE_REQ:
        hfg_adp_hfg_handle_supported_pref_msg_storage_req(ilm_ptr);
        break;

    /* AT_PREFERRED_SMS_STORAGE|AT_READ */
    case MSG_ID_BT_HFG_SELECTED_PREF_MSG_STORAGE_REQ:
        hfg_adp_hfg_handle_selected_pref_msg_storage_req(ilm_ptr);
        break;

    /* AT_PREFERRED_SMS_STORAGE */
    case MSG_ID_BT_HFG_PREF_MSG_STORAGE_REQ:
        hfg_adp_hfg_handle_pref_msg_storage_req(ilm_ptr);
        break;

    /* AT_SMS_MESSAGE_FORMAT|AT_TEST */
    case MSG_ID_BT_HFG_SUPPORTED_MSG_FORMAT_REQ:
        hfg_adp_hfg_handle_supported_msg_format_req(ilm_ptr);
        break;

    /* AT_SMS_MESSAGE_FORMAT|AT_READ */
    case MSG_ID_BT_HFG_SELECTED_MSG_FORMAT_REQ:
        hfg_adp_hfg_handle_selected_msg_format_req(ilm_ptr);
        break;

    /* AT_SMS_SERVICE_CENTER|AT_READ */
    case MSG_ID_BT_HFG_SERVICE_CENTRE_REQ:
        hfg_adp_hfg_handle_service_centre_req(ilm_ptr);
        break;

    /* AT_SET_TEXT_MODE_PARMS|AT_READ */
    case MSG_ID_BT_HFG_TEXT_MODE_PARAMS_REQ:
        hfg_adp_hfg_handle_text_mode_params_req(ilm_ptr);
        break;

    /* AT_SMS_SHOW_TEXT_MODE|AT_TEST */
    case MSG_ID_BT_HFG_SUPPORTED_SHOW_PARAMS_REQ:
        hfg_adp_hfg_handle_supported_show_params_req(ilm_ptr);
        break;

    /* AT_SMS_SHOW_TEXT_MODE|AT_READ */
    case MSG_ID_BT_HFG_SELECTED_SHOW_PARAMS_REQ:
        hfg_adp_hfg_handle_selected_show_params_req(ilm_ptr);
        break;

    /* AT_NEW_MESSAGE_INDICATION|AT_TEST */
    case MSG_ID_BT_HFG_SUPPORTED_NEW_MSG_INDICATION_REQ:
        hfg_adp_hfg_handle_supported_new_msg_indication_req(ilm_ptr);
        break;

    /* AT_NEW_MESSAGE_INDICATION|AT_READ */
    case MSG_ID_BT_HFG_SELECTED_NEW_MSG_INDICATION_REQ:
        hfg_adp_hfg_handle_selected_new_msg_indication_req(ilm_ptr);
        break;

    /* AT_SMS_DELIVER_INDICATION */ 
    case MSG_ID_BT_HFG_NEW_MSG_INDICATION_REQ:
        hfg_adp_hfg_handle_new_msg_indication_req(ilm_ptr);
        break;
        
    /* AT_LIST_MESSAGES|AT_TEST */
    case MSG_ID_BT_HFG_SUPPORTED_LIST_STATUS_REQ:
        hfg_adp_hfg_handle_supported_list_status_req(ilm_ptr);
        break;

    /* AT_LIST_MESSAGES */
    case MSG_ID_BT_HFG_LIST_MSG_REQ:
        hfg_adp_hfg_handle_list_msg_req(ilm_ptr);
        break;

    /* AT_READ_MESSAGE */
    case MSG_ID_BT_HFG_READ_MSG_REQ:
        hfg_adp_hfg_handle_read_msg_req(ilm_ptr);
        break;

    /* AT_SEND_MESSAGE */
    case MSG_ID_BT_HFG_SEND_MSG_REQ:
        hfg_adp_hfg_handle_send_msg_req(ilm_ptr);
        break;

    /* AT_SEND_STORED_MESSAGE */
    case MSG_ID_BT_HFG_SEND_STORED_MSG_REQ:
        hfg_adp_hfg_handle_send_stored_msg_req(ilm_ptr);
        break;

    /* AT_STORE_MESSAGE */
    case MSG_ID_BT_HFG_WRITE_MSG_REQ:
        hfg_adp_hfg_handle_write_msg_req(ilm_ptr);
        break;

    case MSG_ID_BT_HFG_SMS_ERROR_REQ:
        hfg_adp_hfg_handle_sms_error_req(ilm_ptr);
        break;
#endif /* AT_SMS == XA_ENABLED */

    #ifdef BT_HFG_UT_TEST
    case MSG_ID_BT_HFG_UT_INIT_REQ:
        hfg_adp_hfg_handle_ut_init_req(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_UT_TX_REQ:
        hfg_adp_hfg_handle_ut_tx_req(ilm_ptr);
        break;
        
    case MSG_ID_BT_HFG_UT_RX_RSP:
        hfg_adp_hfg_handle_ut_rx_rsp(ilm_ptr);
        break;
    #endif
              
    default:
        break;
    }
}
