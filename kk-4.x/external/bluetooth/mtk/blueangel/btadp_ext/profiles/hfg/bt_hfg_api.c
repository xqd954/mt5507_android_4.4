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

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bt_hfg_api.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Dlight Ting
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#define LOG_TAG "BTEXTADP_HF"
 
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)
#include <windows.h>
#endif  /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
#include "string.h"
#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_hfg_message.h"
#include "bluetooth_hfg_struct.h"
#include "bt_hfg_struct.h"
#include "bt_mmi_msg.h"
#include "bt_ext_system.h"
#include <errno.h>
#include <sys/socket.h>
#include "bt_ext_debug.h"

#define InitLock()
#define DeinitLock()	
#define Lock()
#define Unlock()

#ifdef BTMTK_ON_LINUX

#define RESPONSE_TIMEOUT 3000 /* ms */

//#define CAL_MSG_SIZE(type) (sizeof(ilm_struct)-MAX_ILM_BUFFER_SIZE+sizeof(type))
#define CAL_MSG_SIZE(type) sizeof(type)
#define ILM_HEADER_SIZE (sizeof(ilm_struct)-MAX_ILM_BUFFER_SIZE)
#define CHECK_ILM_SIZE(size, type) \
    if( (size) != (ILM_HEADER_SIZE+sizeof(type)) ) { \
        bt_ext_err("msg size is not match: expected=%d, returned size=%d", (ILM_HEADER_SIZE+sizeof(type)), (size)); \
        } \

BtStatus HFG_SendMessage(HfgChannelContext* Channel, msg_type msg_id, void *ptr, U16 size)
{
    int ret;
    int sockfd;
    ilm_struct *ilm;
    U16 *field;
	
    bt_ext_log("msg=%u, ptr=0x%X, size=%d", msg_id, (unsigned int)ptr, size);
    sockfd = Channel->sockfd;

    ret = -1;
    ilm = (ilm_struct*)ptr;
    /* Set src_mod to MOD_MMI */
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = MOD_BT;
    ilm->sap_id = 0;
    ilm->msg_id = msg_id;

    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm->ilm_data;
    *field = 1;
    *(++field) = size;
    //field = (U16 *)&ilm->ilm_data[2];
    //*field = size;

    // cal whole ilm size
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
    
    if(sockfd)
    {
    	 #if 0
        ret = sendto(nat->servsock, ptr, size, 0, (const sockaddr*)&nat->servname, nat->servnamelen);
        #else
        ret = send(sockfd, ptr, size, 0);
	 #endif
        if(ret < 0)
        {
            bt_ext_err("send msg fail : %s, %d", strerror(errno), errno);
        }
        else
        {
            bt_ext_log("send msg success : %d", ret);
        }
    }
    else
    {
        bt_ext_err("socket uninitialized");
    }
    return (ret >= 0) ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
}

BtStatus HFG_WaitResponse(HfgChannelContext* Channel, msg_type resp_id, ilm_struct *ilm, U16 *size)
{
    int res = 0;
    fd_set readfs;
    struct timeval Timeout;
    int sockfd = Channel->sockfd;

    Timeout.tv_usec = (RESPONSE_TIMEOUT%1000)*1000;
    Timeout.tv_sec  = RESPONSE_TIMEOUT/1000;
    FD_ZERO(&readfs);
    if(sockfd)
    {
        FD_SET(sockfd, &readfs);
    }
    else
    {
        bt_ext_err("[JNI][ERR] nat->servsock == 0. exit");
    }
    res = TEMP_FAILURE_RETRY(select(sockfd+1, &readfs, NULL, NULL, &Timeout));
    if(res > 0)
    {
        res = recvfrom(sockfd, ilm, sizeof(ilm_struct), 0, NULL, NULL);
        //bt_ext_log("[JNI] recv %d bytes resp", res);
        if(res < 0)
        {
            bt_ext_err("[JNI] recvfrom failed : %s, %d", strerror(errno), errno);
        }
        else
        {
            if( ilm->msg_id != resp_id)
            {
                res = -1;
                bt_ext_err("[JNI] recv resp=%u is mismatch, expected : %u", (unsigned int)ilm->msg_id, (unsigned int)resp_id);
            }
            else
            {
                *size = res;            
                bt_ext_log("[JNI] recv resp=%u, bytes=%d", (unsigned int)ilm->msg_id, res);
            }
        }
    }
    else if(res == 0)
    {
        bt_ext_err("[JNI] timeout waiting response, in %d milliseconds", RESPONSE_TIMEOUT);
    }
    else
    {
        bt_ext_err("[JNI] wait select failed : %s, %d", strerror(errno), errno);
    }
    return (res>0) ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
}
#endif

/*******************************************************************
*   Exported functions
*******************************************************************/
static void flushSocket(HfgChannelContext *Channel, ilm_struct *ilm){
    int sock = Channel->sockfd;
    if(sock >= 0 && ilm != NULL){
        Lock();
        while(TEMP_FAILURE_RETRY(recvfrom(sock, ilm, sizeof(ilm_struct), MSG_DONTWAIT, NULL, NULL)) > 0){
            bt_ext_log("flushSocket : msg_id=%u", ilm->msg_id);
            //[ALPS00371187][Huawei_BT_Xi'an_IOT]it will not connect hfp after far away
            //if we don't flush all cached data on the socket, the sequence of the next connection 
            //after link supervision timeout will be confusion
            //break;
        }
        Unlock();
    }
}

static BtStatus hfg_common_req(HfgChannelContext *Channel, msg_type msg_id, msg_type resp_id, ilm_struct *ilm, U16 *size)
{
    BtStatus status;

    bt_ext_log("hfg_common_req(msg_id=%u,resp_id=%u, ilm=0x%X, size=%d)", msg_id, resp_id, (unsigned int)ilm, *size);
    Lock();
    /* Fill msg parameters */
    ((bt_hfg_header_struct*)ilm->ilm_data)->pContext = Channel->hfgContext;
    ((bt_hfg_header_struct*)ilm->ilm_data)->req_context = Channel;
    status = HFG_SendMessage(Channel, msg_id, ilm, *size);
    if(status == BT_STATUS_SUCCESS && resp_id != 0)
    {
        /* Wait for response */
        status = HFG_WaitResponse(Channel, resp_id, ilm, size);
    }
    Unlock();
    return status;
}

BtStatus btmtk_hfg_register(HfgChannelContext *Channel, 
                                                        BTMTK_EventCallBack Callback, 
                                                        int sockfd, 
                                                        kal_bool bHeadset)
{
    BtStatus status;
    ilm_struct ilm;
    bt_hfg_activate_req_struct *msg_p = (bt_hfg_activate_req_struct*)ilm.ilm_data;
    U16 size = CAL_MSG_SIZE(bt_hfg_activate_req_struct);

    bt_ext_log("btmtk_hfg_register(0x%X, 0x%X, %d, %d)", (unsigned int)Channel, (unsigned int)Callback, sockfd, bHeadset?1:0);
    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    Channel->bHeadset = bHeadset;
    Channel->callback = Callback;
    Channel->sockfd = sockfd;
    
    InitLock();
    flushSocket(Channel, &ilm);
    msg_p->bHeadset = bHeadset;
    
    status = hfg_common_req( Channel, 
                                                  MSG_ID_BT_HFG_ACTIVATE_REQ, 
                                                  MSG_ID_BT_HFG_ACTIVATE_CNF,
                                                  &ilm,
                                                  &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_activate_cnf_struct *cnf_p = (bt_hfg_activate_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        if(status == BT_STATUS_SUCCESS)
        {
            Channel->hfgContext = cnf_p->pContext;
        }
        bt_ext_log("BT stack returns result=%d", cnf_p->result);
        CHECK_ILM_SIZE(size, bt_hfg_activate_cnf_struct);
    }
    else
    {
        DeinitLock();
    }
    
    return status;
}

BtStatus btmtk_hfg_deregister(HfgChannelContext *Channel)
{
    BtStatus status;
    ilm_struct ilm;
    U16 size = CAL_MSG_SIZE(bt_hfg_deactivate_req_struct);

    bt_ext_log("btmtk_hfg_deregister(0x%X)", (unsigned int)Channel);
    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    status = hfg_common_req( Channel, 
                                                  MSG_ID_BT_HFG_DEACTIVATE_REQ, 
                                                  MSG_ID_BT_HFG_DEACTIVATE_CNF,
                                                  &ilm,
                                                  &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_deactivate_cnf_struct *cnf_p = (bt_hfg_deactivate_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        if (status == BT_STATUS_SUCCESS)
        {
            Channel->hfgContext = NULL;
        }        
        CHECK_ILM_SIZE(size, bt_hfg_activate_cnf_struct);
    }
    DeinitLock();
    return status;
}

BtStatus btmtk_hfg_create_service_link(HfgChannelContext *Channel, MTK_BD_ADDR *Addr)
{
    ilm_struct ilm;    
    bt_hfg_connect_req_struct *msg_p = (bt_hfg_connect_req_struct*)ilm.ilm_data;
    U16 size = CAL_MSG_SIZE(bt_hfg_connect_req_struct);

    if (!Channel || !Addr)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    memcpy(&msg_p->bt_addr, Addr, sizeof(MTK_BD_ADDR));
    return hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_CONNECT_REQ,                                                 
                                                0,
                                                &ilm,
                                                &size);
}

BtStatus btmtk_hfg_disconnect_service_link(HfgChannelContext *Channel)
{
    ilm_struct ilm;
    U16 size = CAL_MSG_SIZE(bt_hfg_disconnect_req_struct);

    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    return hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_DISCONNECT_REQ, 
                                                0,
                                                &ilm,
                                                &size);
}

BtStatus btmtk_hfg_create_audio_link(HfgChannelContext *Channel)
{
    ilm_struct ilm;
    U16 size = CAL_MSG_SIZE(bt_hfg_sco_connect_req_struct);

    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    return hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_SCO_CONNECT_REQ, 
                                                0,
                                                &ilm,
                                                &size);
}

BtStatus btmtk_hfg_disconnect_audio_link(HfgChannelContext *Channel)
{
    ilm_struct ilm;
    U16 size = CAL_MSG_SIZE(bt_hfg_sco_disconnect_req_struct);

    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    return hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_SCO_DISCONNECT_REQ, 
                                                0,
                                                &ilm,
                                                &size);
}

BtStatus btmtk_hfg_accept_connect(HfgChannelContext *Channel)
{
    BtStatus status;
    ilm_struct ilm;
    U16 size = CAL_MSG_SIZE(bt_hfg_accept_channel_req_struct);

    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    status = hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_ACCEPT_CHANNEL_REQ, 
                                                MSG_ID_BT_HFG_ACCEPT_CHANNEL_CNF,
                                                &ilm,
                                                &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_accept_channel_cnf_struct *cnf_p = (bt_hfg_accept_channel_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        CHECK_ILM_SIZE(size, bt_hfg_accept_channel_cnf_struct);
    }
    return status;
}

BtStatus btmtk_hfg_reject_connect(HfgChannelContext *Channel)
{
    BtStatus status;
    ilm_struct ilm;
    U16 size = CAL_MSG_SIZE(bt_hfg_reject_channel_req_struct);

    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    status = hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_REJECT_CHANNEL_REQ, 
                                                MSG_ID_BT_HFG_REJECT_CHANNEL_CNF,
                                                &ilm,
                                                &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_reject_channel_cnf_struct *cnf_p = (bt_hfg_reject_channel_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        CHECK_ILM_SIZE(size, bt_hfg_reject_channel_cnf_struct);
    }
    return status;
}

BtStatus btmtk_hfg_send_data(HfgChannelContext *Channel, const char *atStr, U16 atsize)
{
    ilm_struct ilm;
    bt_hfg_send_data_req_struct *msg_p = (bt_hfg_send_data_req_struct*)ilm.ilm_data;
    U16 size;

    bt_ext_log("btmtk_hfg_send_data( size=%d, atStr=%s)", atsize, atStr);
    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }    
    if(atsize == 0)
    {
        return BT_STATUS_FAILED;
    }
    if(atsize >= sizeof(msg_p->atresult))
    {
        return BT_STATUS_FAILED;
    }
    else
    {
        flushSocket(Channel, &ilm);
        msg_p->data_size = atsize;
        memcpy(msg_p->atresult, atStr, atsize);
        msg_p->atresult[atsize] = '\0';
        msg_p->bDone = KAL_FALSE;
    }
    size = CAL_MSG_SIZE(bt_hfg_send_data_req_struct)-sizeof(msg_p->atresult)+atsize+1;
    return hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_SEND_DATA_REQ, 
                                                MSG_ID_BT_HFG_SEND_DATA_CNF,  // 0 means no response is needed
                                                &ilm,
                                                /* Only send struct header and the at string */
                                                &size);
}

BtStatus btmtk_hfg_set_indicator_value(HfgChannelContext *Channel, HfgIndicator Indicator, U8 value)
{
    BtStatus status;
    ilm_struct ilm;
    bt_hfg_indicator_report_req_struct *msg_p = (bt_hfg_indicator_report_req_struct*)ilm.ilm_data;
    U16 size = CAL_MSG_SIZE(bt_hfg_indicator_report_req_struct);

    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    msg_p->indicator = Indicator;
    msg_p->value = value;
    status = hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_INDICATOR_REPORT_REQ, 
                                                MSG_ID_BT_HFG_INDICATOR_REPORT_CNF,
                                                &ilm,
                                                &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_indicator_report_cnf_struct *cnf_p = (bt_hfg_indicator_report_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        CHECK_ILM_SIZE(size, bt_hfg_indicator_report_cnf_struct);
    }
    return status;
}

BtStatus btmtk_hfg_send_ok(HfgChannelContext *Channel)
{
    ilm_struct ilm;
    U16 size = CAL_MSG_SIZE(bt_hfg_ok_req_struct);

    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    return hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_OK_REQ, 
                                                0,
                                                &ilm,
                                                &size);
}

BtStatus btmtk_hfg_send_error(HfgChannelContext *Channel, U8 Error)
{
    BtStatus status;
    ilm_struct ilm;
    bt_hfg_error_req_struct *msg_p = (bt_hfg_error_req_struct*)ilm.ilm_data;
    U16 size = CAL_MSG_SIZE(bt_hfg_error_req_struct);

    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    msg_p->data = Error;
    status = hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_ERROR_REQ, 
                                                MSG_ID_BT_HFG_ERROR_CNF,
                                                &ilm,
                                                &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_error_cnf_struct *cnf_p = (bt_hfg_error_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        CHECK_ILM_SIZE(size, bt_hfg_error_cnf_struct);
    }
    return status;
}

BtStatus btmtk_hfg_send_ring(HfgChannelContext *Channel)
{
    BtStatus status;
    ilm_struct ilm;
    U16 size = CAL_MSG_SIZE(bt_hfg_ring_req_struct);

    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    status = hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_RING_REQ, 
                                                MSG_ID_BT_HFG_RING_CNF,
                                                &ilm,
                                                &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_ring_cnf_struct *cnf_p = (bt_hfg_ring_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        CHECK_ILM_SIZE(size, bt_hfg_ring_cnf_struct);
    }
    return status;
}

BtStatus btmtk_hfg_send_call_list_rsp(HfgChannelContext *Channel, HfgCallListParms *Parms, kal_bool FinalFlag)
{
    BtStatus status;
    ilm_struct ilm;
    bt_hfg_call_list_req_struct *msg_p = (bt_hfg_call_list_req_struct*)ilm.ilm_data;
    U16 size = CAL_MSG_SIZE(bt_hfg_call_list_req_struct);

    if (!Channel || !Parms)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    msg_p->index = Parms->index;
    msg_p->dir = Parms->dir;
    msg_p->state = Parms->state;
    msg_p->mode = Parms->mode;
    msg_p->multiParty = Parms->multiParty;    
    msg_p->numsize = strlen(Parms->number);
    memcpy(&msg_p->number, Parms->number, msg_p->numsize);
    msg_p->number[msg_p->numsize] = '\0';
    msg_p->type = Parms->type;    
    msg_p->bFinal = FinalFlag;
    status = hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_CALL_LIST_REQ, 
                                                MSG_ID_BT_HFG_CALL_LIST_CNF,
                                                &ilm,
                                                &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_call_list_cnf_struct *cnf_p = (bt_hfg_call_list_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        CHECK_ILM_SIZE(size, bt_hfg_call_list_cnf_struct);
    }
    return status;
}

BtStatus btmtk_hfg_send_caller_id(HfgChannelContext *Channel, HfgCallerIdParms *CallId)
{
    BtStatus status;
    ilm_struct ilm;
    bt_hfg_call_id_req_struct *msg_p = (bt_hfg_call_id_req_struct*)ilm.ilm_data;
    U16 size = CAL_MSG_SIZE(bt_hfg_call_id_req_struct);

    if (!Channel || !CallId)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    msg_p->numsize = strlen(CallId->number);
    memcpy(&msg_p->number, CallId->number, msg_p->numsize);    
    msg_p->number[msg_p->numsize] = '\0';   
    msg_p->type = CallId->type;
    status = hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_CALL_ID_REQ, 
                                                MSG_ID_BT_HFG_CALL_ID_CNF,
                                                &ilm,
                                                &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_call_id_cnf_struct *cnf_p = (bt_hfg_call_id_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        CHECK_ILM_SIZE(size, bt_hfg_call_id_cnf_struct);
    }
    return status;
}

BtStatus btmtk_hfg_enable_vr(HfgChannelContext *Channel, kal_bool Enabled)
{
    BtStatus status;
    ilm_struct ilm;
    bt_hfg_voice_rec_req_struct *msg_p = (bt_hfg_voice_rec_req_struct*)ilm.ilm_data;
    U16 size = CAL_MSG_SIZE(bt_hfg_voice_rec_req_struct);

    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    msg_p->enable = Enabled;
    status = hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_VOICE_REC_REQ, 
                                                MSG_ID_BT_HFG_VOICE_REC_CNF,
                                                &ilm,
                                                &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_voice_rec_cnf_struct *cnf_p = (bt_hfg_voice_rec_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        CHECK_ILM_SIZE(size, bt_hfg_voice_rec_cnf_struct);
    }
    return status;
}

BtStatus btmtk_hfg_send_network_operator_rsp(HfgChannelContext *Channel, HfgNetworkOperator *Oper)
{
    BtStatus status;
    ilm_struct ilm;
    bt_hfg_operator_name_req_struct *msg_p = (bt_hfg_operator_name_req_struct*)ilm.ilm_data;
    U16 size = CAL_MSG_SIZE(bt_hfg_operator_name_req_struct);

    if (!Channel || !Oper)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    msg_p->mode = Oper->mode;
    msg_p->format = Oper->format;
    msg_p->opersize = strlen(Oper->oper);
    memcpy(msg_p->oper, Oper->oper, msg_p->opersize);
    msg_p->oper[msg_p->opersize] = '\0';  
    status = hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_OPERATOR_NAME_REQ, 
                                                MSG_ID_BT_HFG_OPERATOR_NAME_CNF,
                                                &ilm,
                                                &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_operator_name_cnf_struct *cnf_p = (bt_hfg_operator_name_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        CHECK_ILM_SIZE(size, bt_hfg_operator_name_cnf_struct);
    }
    return status;
}

BtStatus btmtk_hfg_send_mic_volume(HfgChannelContext *Channel, U32 Gain)
{
    BtStatus status;
    ilm_struct ilm;
    bt_hfg_mic_gain_req_struct *msg_p = (bt_hfg_mic_gain_req_struct*)ilm.ilm_data;
    U16 size = CAL_MSG_SIZE(bt_hfg_mic_gain_req_struct);

    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    msg_p->data = Gain;
    status = hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_MIC_GAIN_REQ, 
                                                MSG_ID_BT_HFG_MIC_GAIN_CNF,
                                                &ilm,
                                                &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_mic_gain_cnf_struct *cnf_p = (bt_hfg_mic_gain_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        CHECK_ILM_SIZE(size, bt_hfg_mic_gain_cnf_struct);
    }
    return status;
}

BtStatus btmtk_hfg_send_speaker_volume(HfgChannelContext *Channel, U32 Gain)
{
    BtStatus status;
    ilm_struct ilm;
    bt_hfg_speaker_gain_req_struct *msg_p = (bt_hfg_speaker_gain_req_struct*)ilm.ilm_data;
    U16 size = CAL_MSG_SIZE(bt_hfg_speaker_gain_req_struct);

    if (!Channel)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    msg_p->data = Gain;
    status = hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_SPEAKER_GAIN_REQ, 
                                                MSG_ID_BT_HFG_SPEAKER_GAIN_CNF,
                                                &ilm,
                                                &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_speaker_gain_cnf_struct *cnf_p = (bt_hfg_speaker_gain_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        CHECK_ILM_SIZE(size, bt_hfg_speaker_gain_cnf_struct);
    }
    return status;
}

BtStatus btmtk_hfg_call_waiting(HfgChannelContext *Channel, HfgCallWaitParms *CallWait)
{
    BtStatus status;
    ilm_struct ilm;
    bt_hfg_call_waiting_req_struct *msg_p = (bt_hfg_call_waiting_req_struct*)ilm.ilm_data;
    U16 size = CAL_MSG_SIZE(bt_hfg_call_waiting_req_struct);

    if (!Channel || !CallWait)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    msg_p->numsize = strlen(CallWait->number);
    memcpy(&msg_p->number, CallWait->number, msg_p->numsize);    
    msg_p->number[msg_p->numsize] = '\0';   
    msg_p->classmap = CallWait->classmap;
    msg_p->type = CallWait->type;
    status = hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_CALL_WAITING_REQ, 
                                                MSG_ID_BT_HFG_CALL_WAITING_CNF,
                                                &ilm,
                                                &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_call_waiting_cnf_struct *cnf_p = (bt_hfg_call_waiting_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        CHECK_ILM_SIZE(size, bt_hfg_call_waiting_cnf_struct);
    }
    return status;
}

BtStatus btmtk_hfg_read_indicator(HfgChannelContext *Channel, HfgReadIndicatorParms *ReadIndicator)
{
    BtStatus status;
    ilm_struct ilm;
    bt_hfg_read_indicator_req_struct *msg_p = (bt_hfg_read_indicator_req_struct*)ilm.ilm_data;
    U16 size = CAL_MSG_SIZE(bt_hfg_read_indicator_req_struct);

    if (!Channel || !ReadIndicator)
    {
        return BT_STATUS_FAILED;
    }
    flushSocket(Channel, &ilm);
    memcpy(&msg_p->indicators, ReadIndicator, sizeof(HfgReadIndicatorParms));
    status = hfg_common_req( Channel, 
                                                MSG_ID_BT_HFG_READ_INDICATOR_REQ, 
                                                MSG_ID_BT_HFG_READ_INDICATOR_CNF,
                                                &ilm,
                                                &size);
    if(status == BT_STATUS_SUCCESS)
    {
        bt_hfg_read_indicator_cnf_struct *cnf_p = (bt_hfg_read_indicator_cnf_struct*)ilm.ilm_data;
        status = cnf_p->result;
        CHECK_ILM_SIZE(size, bt_hfg_read_indicator_cnf_struct);
    }
    return status;
}
