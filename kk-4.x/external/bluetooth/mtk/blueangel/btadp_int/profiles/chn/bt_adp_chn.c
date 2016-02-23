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
#include "chn.h"
#include "bluetooth_chn_message.h"
#include "bt_adp_chn.h"

ListEntry g_chn_channel_list;

/***************************
Internal functions
****************************/
void chn_sendMsg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{

    if (dstMod == 0)
    {
        return;
    }

    if (local_para != NULL)
    {
        BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
        free_local_para(local_para);
    }
    else
    {
        BT_SendMessage(msg, dstMod, NULL, 0);
    }
}

void chn_send_channel_msg(ChnAdpChannel *adp_channel, 
                                      msg_type msg,
                                      void *para)
{
    chn_sendMsg(msg, adp_channel->registered_module, BT_APP_SAP, para, NULL);
}

/***************
External Globals 
****************/
BOOL chn_adp_init(void)
{
    InitializeListHead(&g_chn_channel_list);
    return chn_init();
}

/*****************************************************************************
 * FUNCTION
 *  chn_adp_activate
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void chn_adp_activate_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NO_RESOURCES;
    ChnAdpChannel *adp_channel = NULL;
    bt_chn_activate_req_struct* msg_p;
    bt_chn_activate_cnf_struct* cnf_msg_p;

    msg_p = (bt_chn_activate_req_struct*)ilm_ptr->local_para_ptr;
    
    adp_channel = (ChnAdpChannel*)get_ctrl_buffer(sizeof(ChnAdpChannel));
    if(adp_channel != NULL)
    {
        OS_MemSet((U8*)adp_channel, 0, sizeof(ChnAdpChannel));
        {
            status = chn_register(&adp_channel->channel, chn_app_callback, msg_p->svc, msg_p->remote_svc);
            if(status == BT_STATUS_SUCCESS)
            {
                adp_channel->userContext = msg_p->req_context;
                adp_channel->registered_module = ilm_ptr->src_mod_id;
                InsertTailList(&g_chn_channel_list, &adp_channel->node);
            }
            else
            {
                free_ctrl_buffer((void*)adp_channel);
                adp_channel = NULL;
            }
            /* response with status */
        }
    }

    /* send active confirm message */
    cnf_msg_p = (bt_chn_activate_cnf_struct*) construct_local_para(sizeof(bt_chn_activate_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->result = status;
    cnf_msg_p->req_context = msg_p->req_context;
    chn_sendMsg(MSG_ID_BT_CHN_ACTIVATE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    
}

/*****************************************************************************
 * FUNCTION
 *  chn_adp_deactivate
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void chn_adp_deactivate_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_chn_deactivate_req_struct *msg_p;
    bt_chn_deactivate_cnf_struct *cnf_msg_p;
    ChnAdpChannel *adp_channel;

    msg_p = (bt_chn_deactivate_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    /* construct return message */
    cnf_msg_p = (bt_chn_deactivate_cnf_struct*) construct_local_para(sizeof(bt_chn_deactivate_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;
    
    if (IsNodeOnList(&g_chn_channel_list, &adp_channel->node)) 
    {
        status = chn_deregister(&adp_channel->channel);
        RemoveEntryList(&adp_channel->node);
        /* Send deactive confirm */
        cnf_msg_p->result = status;
        chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_DEACTIVATE_CNF, (local_para_struct*)cnf_msg_p);
        //free_ctrl_buffer(adp_channel->channel);
        free_ctrl_buffer(adp_channel);        
    }
    else
    {
        cnf_msg_p->result = status;
        chn_sendMsg(MSG_ID_BT_CHN_DEACTIVATE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
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
void chn_adp_connect_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_chn_connect_req_struct *msg_p;
    bt_chn_connect_cnf_struct *cnf_msg_p;
    ChnAdpChannel *adp_channel;

    msg_p = (bt_chn_connect_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    /* construct return message */
    cnf_msg_p = (bt_chn_deactivate_cnf_struct*) construct_local_para(sizeof(bt_chn_deactivate_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;
    
    if (IsNodeOnList(&g_chn_channel_list, &adp_channel->node)) 
    {
        status = chn_create_service_link(&adp_channel->channel, (BD_ADDR*)&msg_p->bt_addr);
        /* Send deactive confirm */
        cnf_msg_p->result = status;
        chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_CONNECT_CNF, (local_para_struct*)cnf_msg_p);
    }
    else
    {
        cnf_msg_p->result = status;
        chn_sendMsg(MSG_ID_BT_CHN_CONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  chn_adp_accept_channel_hdler
 * DESCRIPTION
 *  This function is to handle accept connect request from remote
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void chn_adp_accept_channel_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_chn_accept_channel_req_struct *msg_p;
    bt_chn_accept_channel_cnf_struct *cnf_msg_p;
    ChnAdpChannel *adp_channel;

    msg_p = (bt_chn_accept_channel_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    /* construct return message */
    cnf_msg_p = (bt_chn_accept_channel_cnf_struct*) construct_local_para(sizeof(bt_chn_accept_channel_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;

    if (IsNodeOnList(&g_chn_channel_list, &adp_channel->node)) 
    {
        status = chn_accept_connect(&adp_channel->channel);
        /* Send deactive confirm */
        cnf_msg_p->result = status;
        chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_ACCEPT_CHANNEL_CNF, (local_para_struct*)cnf_msg_p);
    }
    else
    {
        cnf_msg_p->result = status;
        chn_sendMsg(MSG_ID_BT_CHN_ACCEPT_CHANNEL_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  chn_adp_reject_channel_hdler
 * DESCRIPTION
 *  This function is to handle reject connect request from remote
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void chn_adp_reject_channel_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_chn_reject_channel_req_struct *msg_p;
    bt_chn_reject_channel_cnf_struct *cnf_msg_p;
    ChnAdpChannel *adp_channel;

    msg_p = (bt_chn_reject_channel_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    /* construct return message */
    cnf_msg_p = (bt_chn_reject_channel_cnf_struct*) construct_local_para(sizeof(bt_chn_reject_channel_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;

    if (IsNodeOnList(&g_chn_channel_list, &adp_channel->node)) 
    {
        status = chn_reject_connect(&adp_channel->channel);
        /* Send deactive confirm */
        cnf_msg_p->result = status;
        chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_REJECT_CHANNEL_CNF, (local_para_struct*)cnf_msg_p);
    }
    else
    {
        cnf_msg_p->result = status;
        chn_sendMsg(MSG_ID_BT_CHN_REJECT_CHANNEL_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}


/*****************************************************************************
 * FUNCTION
 *  chn_adp_disconnect_req_hdler
 * DESCRIPTION
 *  This function is to handle disconnect request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void chn_adp_disconnect_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_chn_disconnect_req_struct *msg_p;
    bt_chn_disconnect_cnf_struct *cnf_msg_p;
    ChnAdpChannel *adp_channel;

    msg_p = (bt_chn_disconnect_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    /* construct return message */
    cnf_msg_p = (bt_chn_disconnect_cnf_struct*) construct_local_para(sizeof(bt_chn_disconnect_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;

    if (IsNodeOnList(&g_chn_channel_list, &adp_channel->node)) 
    {
        status = chn_disconnect_service_link(&adp_channel->channel);
        /* Send deactive confirm */
        cnf_msg_p->result = status;
        chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_DISCONNECT_CNF, (local_para_struct*)cnf_msg_p);
    }
    else
    {
        cnf_msg_p->result = status;
        chn_sendMsg(MSG_ID_BT_CHN_DISCONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  chn_adp_sco_connect_req_hdler
 * DESCRIPTION
 *  This function is to handle sco connect request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void chn_adp_sco_connect_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_chn_sco_connect_req_struct *msg_p;
    bt_chn_sco_connect_cnf_struct *cnf_msg_p;
    ChnAdpChannel *adp_channel;

    msg_p = (bt_chn_sco_connect_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    /* construct return message */
    cnf_msg_p = (bt_chn_sco_connect_cnf_struct*) construct_local_para(sizeof(bt_chn_sco_connect_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;

    if (IsNodeOnList(&g_chn_channel_list, &adp_channel->node)) 
    {
        status = chn_create_audio_link(&adp_channel->channel);
        /* Send deactive confirm */
        cnf_msg_p->result = status;
        chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_SCO_CONNECT_CNF, (local_para_struct*)cnf_msg_p);
    }
    else
    {
        cnf_msg_p->result = status;
        chn_sendMsg(MSG_ID_BT_CHN_SCO_CONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  chn_adp_sco_disconnect_req_hdler
 * DESCRIPTION
 *  This function is to handle timeout case
 * PARAMETERS
 *  
 * RETURNS
 *  void
 *****************************************************************************/
void chn_adp_sco_disconnect_req_hdler(ilm_struct *ilm_ptr)
{
    BtStatus status = BT_STATUS_NOT_FOUND;
    bt_chn_sco_disconnect_req_struct *msg_p;
    bt_chn_sco_disconnect_cnf_struct *cnf_msg_p;
    ChnAdpChannel *adp_channel;

    msg_p = (bt_chn_sco_disconnect_req_struct*)ilm_ptr->local_para_ptr;
    adp_channel = msg_p->pContext;

    /* construct return message */
    cnf_msg_p = (bt_chn_sco_disconnect_cnf_struct*) construct_local_para(sizeof(bt_chn_sco_disconnect_cnf_struct), TD_UL);
    cnf_msg_p->pContext = adp_channel;
    cnf_msg_p->req_context = msg_p->req_context;

    if (IsNodeOnList(&g_chn_channel_list, &adp_channel->node)) 
    {
        status = chn_disconnect_audio_link(&adp_channel->channel);
        /* Send disconnect confirm */
        cnf_msg_p->result = status;
        chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_SCO_DISCONNECT_CNF, (local_para_struct*)cnf_msg_p);
    }
    else
    {
        cnf_msg_p->result = status;
        chn_sendMsg(MSG_ID_BT_CHN_SCO_DISCONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  chn_adp_send_data_req_hdler
 * DESCRIPTION
 *  This function is to handle gain request
 * PARAMETERS
 *  ilm_ptr     [IN] inter layer message       
 * RETURNS
 *  void
 *****************************************************************************/
void chn_adp_send_data_req_hdler(ilm_struct *ilm_ptr)
{
	BtStatus status = BT_STATUS_NOT_FOUND;
	bt_chn_send_data_req_struct *msg_p;
	bt_chn_send_data_cnf_struct *cnf_msg_p;
	ChnAdpChannel *adp_channel;
	//HfgResponse *response = NULL;
	//U16 len;

	msg_p = (bt_chn_send_data_req_struct*)ilm_ptr->local_para_ptr;
	adp_channel = msg_p->pContext;

	if (IsNodeOnList(&g_chn_channel_list, &adp_channel->node)) 
	{
		status = chn_send_data(&adp_channel->channel, msg_p->data, msg_p->size);
	}
	else
	{
		adp_channel = NULL;
	}

	/* construct return message */
	cnf_msg_p = (bt_chn_send_data_cnf_struct*) construct_local_para(sizeof(bt_chn_send_data_cnf_struct), TD_UL);
	cnf_msg_p->pContext = adp_channel;
	cnf_msg_p->req_context = msg_p->req_context;
	cnf_msg_p->result = status;
	if(adp_channel != NULL)
	{
	    chn_send_channel_msg(adp_channel, MSG_ID_BT_CHN_SEND_DATA_CNF, (local_para_struct*)cnf_msg_p);        
	}
	else
	{
	    chn_sendMsg(MSG_ID_BT_CHN_SEND_DATA_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct*)cnf_msg_p, NULL);
	}
}

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
void chn_handler(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_CHN_ACTIVATE_REQ:
            chn_adp_activate_req_hdler(ilm_ptr);
            break;
        case MSG_ID_BT_CHN_DEACTIVATE_REQ:
            chn_adp_deactivate_req_hdler(ilm_ptr);
            break;
        case MSG_ID_BT_CHN_CONNECT_REQ:
            chn_adp_connect_req_hdler(ilm_ptr);
            break;
        case MSG_ID_BT_CHN_DISCONNECT_REQ:
            chn_adp_disconnect_req_hdler(ilm_ptr);
            break;
        case MSG_ID_BT_CHN_SCO_CONNECT_REQ:
            chn_adp_sco_connect_req_hdler(ilm_ptr);
            break;
        case MSG_ID_BT_CHN_SCO_DISCONNECT_REQ:
            chn_adp_sco_disconnect_req_hdler(ilm_ptr);
            break;

        case MSG_ID_BT_CHN_ACCEPT_CHANNEL_REQ:
            chn_adp_accept_channel_hdler(ilm_ptr);
            break;
        case MSG_ID_BT_CHN_REJECT_CHANNEL_REQ:
            chn_adp_reject_channel_hdler(ilm_ptr);
            break;

        case MSG_ID_BT_CHN_SEND_DATA_REQ:
            chn_adp_send_data_req_hdler(ilm_ptr);
            break;
       
        default:
            break;
    }
}
