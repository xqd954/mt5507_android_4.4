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
*  permission of MediaTek Inc. (C) 2006
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
 * bt_adp_bm.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions which provide the service to be the adaption layer 
 *   between JAVA and MMI
 *
 * Author:
 * -------
 * Dlight Ting(mtk01239)
 *      Create 2006/2/9
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#include "stdio.h"      /* Basic C I/O functions */
#include "hfg.h"
#include "bt_types.h"
#include "bluetooth_struct.h"
#include "sdap.h"
#include "bt_adp_hfg.h"
#include "bluetooth_hfg_message.h"
#include "bluetooth_hfg_struct.h"
#include "atpi.h"
/***************
Internal Globals 
****************/

/***************
External Globals 
****************/

/***************
External API
****************/
void a2dp_set_sco_state(U8 state);

/***************************
Internal function prototypes
****************************/
/*********************
Function Definitions 
**********************/

void hfg_free_rsp(HfgAdpChannel *adp_channel, HfgResponse* rsp);

void btmtk_adp_hfg_handle_general_ind(HfgAdpChannel *adp_channel, msg_type msg, HfgCallbackParms *param)
{
    bt_hfg_common_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_common_ind_struct*) construct_local_para(sizeof(bt_hfg_common_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    hfg_send_channel_msg(adp_channel, msg, (local_para_struct*)ind_msg_p);
}

/* HFG_EVENT_QUERY_SUPPORTED_SMS_SERVICE */
void btmtk_adp_hfg_handle_sms_query_supported_srvice_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    btmtk_adp_hfg_handle_general_ind(adp_channel, MSG_ID_BT_HFG_QUERY_SUPPORTED_SMS_SERVICE_IND, param);
}

/* HFG_EVENT_QUERY_SELECTED_SMS_SERVICE */
void btmtk_adp_hfg_handle_sms_query_selected_srvice_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    btmtk_adp_hfg_handle_general_ind(adp_channel, MSG_ID_BT_HFG_QUERY_SELECTED_SMS_SERVICE_IND, param);
}

/* HFG_EVENT_SELECT_SMS_SERVICE */
void btmtk_adp_hfg_handle_sms_select_srvice_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    bt_hfg_select_sms_service_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_select_sms_service_ind_struct*) construct_local_para(sizeof(bt_hfg_select_sms_service_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    ind_msg_p->service = param->p.service;
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SELECT_SMS_SERVICE_IND, (local_para_struct*)ind_msg_p);
}

/* HFG_EVENT_QUERY_SUPPORTED_PREF_MSG_STORAGE */
void btmtk_adp_hfg_handle_sms_query_supported_pref_storage_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    btmtk_adp_hfg_handle_general_ind(adp_channel, MSG_ID_BT_HFG_QUERY_SUPPORTED_PREF_MSG_STORAGE_IND, param);
}

/* HFG_EVENT_QUERY_SELECTED_PREF_MSG_STORAGE */
void btmtk_adp_hfg_handle_sms_query_selected_pref_storage_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    btmtk_adp_hfg_handle_general_ind(adp_channel, MSG_ID_BT_HFG_QUERY_SELECTED_PREF_MSG_STORAGE_IND, param);
}

/* HFG_EVENT_SELECT_PREF_MSG_STORAGE */
void btmtk_adp_hfg_handle_sms_select_pref_storage_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    bt_hfg_select_pref_msg_storage_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_select_pref_msg_storage_ind_struct*) construct_local_para(sizeof(bt_hfg_select_pref_msg_storage_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    ind_msg_p->read = param->p.prefStorage->read;
    ind_msg_p->write = param->p.prefStorage->write;
    ind_msg_p->recv = param->p.prefStorage->recv;
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SELECT_PREF_MSG_STORAGE_IND, (local_para_struct*)ind_msg_p);
}

/* HFG_EVENT_QUERY_SUPPORTED_MSG_FORMAT */
void btmtk_adp_hfg_handle_sms_query_supported_format_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    btmtk_adp_hfg_handle_general_ind(adp_channel, MSG_ID_BT_HFG_QUERY_SUPPORTED_MSG_FORMAT_IND, param);
}

/* HFG_EVENT_QUERY_SELECTED_MSG_FORMAT */
void btmtk_adp_hfg_handle_sms_query_selected_format_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    btmtk_adp_hfg_handle_general_ind(adp_channel, MSG_ID_BT_HFG_QUERY_SELECTED_MSG_FORMAT_IND, param);
}

/* HFG_EVENT_SELECT_MSG_FORMAT */
void btmtk_adp_hfg_handle_sms_select_format_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    bt_hfg_select_msg_format_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_select_msg_format_ind_struct*) construct_local_para(sizeof(bt_hfg_select_msg_format_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    ind_msg_p->format = param->p.format;
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SELECT_MSG_FORMAT_IND, (local_para_struct*)ind_msg_p);
}

/* HFG_EVENT_QUERY_SERVICE_CENTRE */
void btmtk_adp_hfg_handle_sms_query_service_centre_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    btmtk_adp_hfg_handle_general_ind(adp_channel, MSG_ID_BT_HFG_QUERY_SERVICE_CENTRE_IND, param);
}

/* HFG_EVENT_SET_SERVICE_CENTRE */
void btmtk_adp_hfg_handle_sms_set_service_centre_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    U16 len;
    bt_hfg_set_service_centre_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_set_service_centre_ind_struct*) construct_local_para(sizeof(bt_hfg_set_service_centre_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    ind_msg_p->mask = param->p.serviceCentre->mask;
    len = OS_StrLen(param->p.serviceCentre->sca);
    if(len > 0)
    {
        if(len > MAX_ADDR_LEN)
            len = MAX_ADDR_LEN;
        OS_MemCopy((U8*)ind_msg_p->addr, (const U8*)param->p.serviceCentre->sca, len);
    }
    ind_msg_p->addr[len] = 0;
    ind_msg_p->addrType = param->p.serviceCentre->tosca;
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SET_SERVICE_CENTRE_IND, (local_para_struct*)ind_msg_p);
}

/* HFG_EVENT_QUERY_TEXT_MODE_PARAMS */
void btmtk_adp_hfg_handle_sms_query_text_mode_params_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    btmtk_adp_hfg_handle_general_ind(adp_channel, MSG_ID_BT_HFG_QUERY_TEXT_MODE_PARAMS_IND, param);
}

/* HFG_EVENT_SET_TEXT_MODE_PARAMS */
void btmtk_adp_hfg_handle_sms_set_text_mode_params_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    U16 len;
    bt_hfg_set_text_mode_params_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_set_text_mode_params_ind_struct*) construct_local_para(sizeof(bt_hfg_set_text_mode_params_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    
    ind_msg_p->mask = param->p.textParams->mask;
    ind_msg_p->fo = param->p.textParams->fo;
    if(ind_msg_p->mask & HFG_SMS_MASK_VP)
    {
        if(param->p.textParams->strVp != NULL && param->p.textParams->strVp[0] != 0)
        {
            len = OS_StrLen(param->p.textParams->strVp);
            if(len > 20)
                len = 20;
            OS_MemCopy((U8*)ind_msg_p->strVp, (const U8*)param->p.textParams->strVp, len);
            ind_msg_p->strVp[len] = 0;
        }
        else
        {
            ind_msg_p->strVp[0] = 0;
            ind_msg_p->intVp = param->p.textParams->intVp;
        }
    }
    ind_msg_p->pid = param->p.textParams->pid;
    ind_msg_p->dcs = param->p.textParams->dcs;
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SET_TEXT_MODE_PARAMS_IND, (local_para_struct*)ind_msg_p);

}

/* HFG_EVENT_QUERY_SUPPORTED_SHOW_PARAMS */
void btmtk_adp_hfg_handle_sms_query_supported_show_params_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    btmtk_adp_hfg_handle_general_ind(adp_channel, MSG_ID_BT_HFG_QUERY_SUPPORTED_SHOW_PARAMS_IND, param);
}

/* HFG_EVENT_QUERY_SELECTED_SHOW_PARAMS */
void btmtk_adp_hfg_handle_sms_query_selected_show_params_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    btmtk_adp_hfg_handle_general_ind(adp_channel, MSG_ID_BT_HFG_QUERY_SELECTED_SHOW_PARAMS_IND, param);
}

/* HFG_EVENT_SET_SHOW_PARAMS */
void btmtk_adp_hfg_handle_sms_set_show_params_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    bt_hfg_set_show_params_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_set_show_params_ind_struct*) construct_local_para(sizeof(bt_hfg_set_show_params_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    ind_msg_p->show = param->p.show;
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SET_SHOW_PARAMS_IND, (local_para_struct*)ind_msg_p);
}

/* HFG_EVENT_QUERY_SUPPORTED_NEW_MSG_INDICATION */
void btmtk_adp_hfg_handle_sms_query_supported_new_msg_indication_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    btmtk_adp_hfg_handle_general_ind(adp_channel, MSG_ID_BT_HFG_QUERY_SUPPORTED_NEW_MSG_INDICATION_IND, param);
}

/* HFG_EVENT_QUERY_SELECTED_NEW_MSG_INDICATION */
void btmtk_adp_hfg_handle_sms_query_selected_new_msg_indication_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    btmtk_adp_hfg_handle_general_ind(adp_channel, MSG_ID_BT_HFG_QUERY_SELECTED_NEW_MSG_INDICATION_IND, param);
}

/* HFG_EVENT_SET_NEW_MSG_INDICATION */
void btmtk_adp_hfg_handle_sms_set_new_msg_indication_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    bt_hfg_set_new_msg_indication_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_set_new_msg_indication_ind_struct*) construct_local_para(sizeof(bt_hfg_set_new_msg_indication_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    ind_msg_p->mode = param->p.newMsgInd->mode;
    ind_msg_p->mt = param->p.newMsgInd->mt;
    ind_msg_p->bm = param->p.newMsgInd->bm;
    ind_msg_p->ds = param->p.newMsgInd->ds;
    ind_msg_p->bfr = param->p.newMsgInd->bfr;
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SET_NEW_MSG_INDICATION_IND, (local_para_struct*)ind_msg_p);
}

/* HFG_EVENT_QUERY_SUPPORTED_LIST_STATUS */
void btmtk_adp_hfg_handle_sms_query_supported_list_status_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    btmtk_adp_hfg_handle_general_ind(adp_channel, MSG_ID_BT_HFG_QUERY_SUPPORTED_LIST_STATUS_IND, param);
}

/* HFG_EVENT_LIST_MSG */
void btmtk_adp_hfg_handle_sms_list_msg_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    bt_hfg_list_msg_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_list_msg_ind_struct*) construct_local_para(sizeof(bt_hfg_list_msg_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    ind_msg_p->stat = param->p.stat;
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_LIST_MSG_IND, (local_para_struct*)ind_msg_p);
}

/* HFG_EVENT_READ_MSG */
void btmtk_adp_hfg_handle_sms_read_msg_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    bt_hfg_read_msg_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_read_msg_ind_struct*) construct_local_para(sizeof(bt_hfg_read_msg_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    ind_msg_p->index = param->p.index;
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_READ_MSG_IND, (local_para_struct*)ind_msg_p);
}

/* HFG_EVENT_SEND_MSG */
void btmtk_adp_hfg_handle_sms_send_msg_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    U16 len;
    bt_hfg_send_msg_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_send_msg_ind_struct*) construct_local_para(sizeof(bt_hfg_send_msg_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    ind_msg_p->mask = param->p.sendMsg->mask;
    if(ind_msg_p->mask & HFG_SMS_MASK_LENGTH)
    {   /* PDU mode */
        ind_msg_p->pduLen = param->p.sendMsg->pduLen;
    }
    else
    {   /* TEXT mode */
        if ( param->p.sendMsg->destAddr != NULL && param->p.sendMsg->destAddr[0] != 0)
        {
            len = OS_StrLen(param->p.sendMsg->destAddr);
            if(len > MAX_ADDR_LEN)
                len = MAX_ADDR_LEN;
            OS_MemCopy((U8*)ind_msg_p->addr, (const U8*)param->p.sendMsg->destAddr, len);
            ind_msg_p->addr[len] = 0;
            ind_msg_p->addrType = param->p.sendMsg->addrType;
        }
        else
        {
            ind_msg_p->addr[0] = 0;
        }
    }
    if(param->p.sendMsg->msg)
    {
        len = OS_StrLen(param->p.sendMsg->msg);
        if(len > MAX_SMS_TEXT_LEN)
            len = MAX_SMS_TEXT_LEN;
        OS_MemCopy((U8*)ind_msg_p->msg, (const U8*)param->p.sendMsg->msg, len);
        ind_msg_p->msg[len] = 0;
    }
    else
    {
        ind_msg_p->msg[0] = 0;
    }
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SEND_MSG_IND, (local_para_struct*)ind_msg_p);

}

/* HFG_EVENT_SEND_STORED_MSG */
void btmtk_adp_hfg_handle_sms_send_stored_msg_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    U16 len;
    bt_hfg_send_stored_msg_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_send_stored_msg_ind_struct*) construct_local_para(sizeof(bt_hfg_send_stored_msg_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    
    ind_msg_p->mask = param->p.storedMsg->mask;
    ind_msg_p->index = param->p.storedMsg->index;
    
    if(ind_msg_p->mask & HFG_SMS_MASK_ADDR)
    {
        if(param->p.storedMsg->destAddr && param->p.storedMsg->destAddr[0] != 0)
        {
            len = OS_StrLen(param->p.storedMsg->destAddr);
            if(len > MAX_ADDR_LEN)
                len = MAX_ADDR_LEN;
            OS_MemCopy((U8*)ind_msg_p->addr, (const U8*)param->p.storedMsg->destAddr, len);
            ind_msg_p->addr[len] = 0;
            ind_msg_p->addrType = param->p.storedMsg->addrType;
        }
        else
        {
            ind_msg_p->mask &= ~(HFG_SMS_MASK_ADDR|HFG_SMS_MASK_TOADDR);
        }
    }
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SEND_STORED_MSG_IND, (local_para_struct*)ind_msg_p);

}

/* HFG_EVENT_WRITE_MSG */
void btmtk_adp_hfg_handle_sms_write_msg_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    U16 len;
    bt_hfg_write_msg_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_write_msg_ind_struct*) construct_local_para(sizeof(bt_hfg_write_msg_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    
    ind_msg_p->mask = param->p.writeMsg->mask;
    if(ind_msg_p->mask & HFG_SMS_MASK_LENGTH)
    {   /* PDU mode */
        ind_msg_p->pduLen = param->p.writeMsg->pduLen;
    }
    else
    {   /* TEXT mode */
        if(ind_msg_p->mask & HFG_SMS_MASK_ADDR)
        {
            len = OS_StrLen(param->p.writeMsg->addr);
            if(len > MAX_ADDR_LEN)
                len = MAX_ADDR_LEN;
            OS_MemCopy((U8*)ind_msg_p->addr, (const U8*)param->p.writeMsg->addr, len);
            ind_msg_p->addr[len] = 0;
        }
        ind_msg_p->addrType = param->p.writeMsg->addrType;
    }
    ind_msg_p->stat = param->p.writeMsg->stat;
    if(param->p.writeMsg->msg)
    {
        len = OS_StrLen(param->p.writeMsg->msg);
        if(len > MAX_SMS_TEXT_LEN)
            len = MAX_SMS_TEXT_LEN;
        OS_MemCopy((U8*)ind_msg_p->msg, (const U8*)param->p.writeMsg->msg, len);
        ind_msg_p->msg[len] = 0;
    }
    else
    {
        ind_msg_p->msg[0] = 0;
    }
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_WRITE_MSG_IND, (local_para_struct*)ind_msg_p);
}

/* HFG_EVENT_DELETE_MSG */
void btmtk_adp_hfg_handle_sms_delete_msg_ind(HfgAdpChannel *adp_channel, HfgCallbackParms *param)
{
    bt_hfg_delete_msg_ind_struct *ind_msg_p;
    ind_msg_p = (bt_hfg_delete_msg_ind_struct*) construct_local_para(sizeof(bt_hfg_delete_msg_ind_struct), TD_UL);
    ind_msg_p->pContext = (void*)adp_channel;
    ind_msg_p->user_context = adp_channel->userContext;
    ind_msg_p->index = param->p.index;
    hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_DELETE_MSG_IND, (local_para_struct*)ind_msg_p);
}

/*****************************************************************************
 * FUNCTION
 *  bma_init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void hfg_app_callback(HfgChannel *Channel, HfgCallbackParms *Parms)
{
    HfgAdpChannel *adp_channel = ContainingRecord(Channel, HfgAdpChannel, channel);
    U16 len = sizeof(HfgCallbackParms);

    if (!IsNodeOnList(&g_channel_list, &adp_channel->node))
    {
        return;
    }
    
    switch (Parms->event)
    {    
    case HFG_EVENT_SERVICE_CONNECT_REQ:
        break;

    case HFG_EVENT_AUTH_REQ:
    {
        bt_hfg_connect_req_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_connect_req_ind_struct*) construct_local_para(sizeof(bt_hfg_connect_req_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        OS_MemCopy( (U8*)&ind_msg_p->bt_addr, (const U8*)Parms->p.addr, sizeof(BD_ADDR));
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_CONNECT_REQ_IND, (local_para_struct*)ind_msg_p);
        break;
    }
    
    case HFG_EVENT_RFCOMM_CONNECTED:
    {
        bt_hfg_rfcomm_connected_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_rfcomm_connected_ind_struct*) construct_local_para(sizeof(bt_hfg_rfcomm_connected_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        OS_MemCopy( (U8*)&ind_msg_p->bt_addr, (const U8*)Parms->p.addr, sizeof(BD_ADDR));
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_RFCOMM_CONNECTED_IND, (local_para_struct*)ind_msg_p);
        break;
    }
    
    case HFG_EVENT_SERVICE_CONNECTED:
    {
        bt_hfg_connected_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_connected_ind_struct*) construct_local_para(sizeof(bt_hfg_connected_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        OS_MemCopy( (U8*)ind_msg_p->bt_addr.addr, (const U8*)Parms->p.addr->addr, sizeof(BD_ADDR));
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_CONNECTED_IND, (local_para_struct*)ind_msg_p);        
        bt_trace(MOD_BT, BT_HFG_INFO_CONNECT_TIMER_CANCELED);
        EVM_CancelTimer(&adp_channel->timer);
        break;
    }

    case HFG_EVENT_SERVICE_DISCONNECTED:
    {
        EVM_CancelTimer(&adp_channel->timer);        
        if(adp_channel->bDeregistered)
        {
            /* If the handle is deregistered, deregister the profile handle */
            bt_trace(MOD_BT, BT_HFG_DEFERED_DEREGISTERATION, adp_channel);
            hfg_deregister(&adp_channel->channel);
            RemoveEntryList(&adp_channel->node);
            free_ctrl_buffer(adp_channel);
        }
        else
        {
            /* Send disconnected ind only is the handle is not deregistered yet */
            #ifdef __HFG_SYNC_DISCONNECT__
            if( adp_channel->syncDisconnect )
            {
                bt_hfg_disconnect_cnf_struct *cnf_msg_p;
                cnf_msg_p = (bt_hfg_disconnect_cnf_struct*) construct_local_para(sizeof(bt_hfg_disconnect_cnf_struct), TD_UL);
                cnf_msg_p->pContext = (void*)adp_channel;
                cnf_msg_p->req_context = adp_channel->userContext;
                cnf_msg_p->result = BT_STATUS_FAILED;
                adp_channel->syncDisconnect = 0;
                hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_DISCONNECT_CNF, (local_para_struct*)cnf_msg_p);
            }
            else
            #endif
            {
                bt_hfg_disconnected_ind_struct *ind_msg_p;
                ind_msg_p = (bt_hfg_disconnected_ind_struct*) construct_local_para(sizeof(bt_hfg_disconnected_ind_struct), TD_UL);
                ind_msg_p->pContext = (void*)adp_channel;
                ind_msg_p->user_context = adp_channel->userContext;
                if( Parms->p.addr)
                    OS_MemCopy( (U8*)&ind_msg_p->bt_addr, (const U8*)Parms->p.addr, sizeof(BD_ADDR));
                else
                    OS_MemSet( (U8*)&ind_msg_p->bt_addr, 0, sizeof(BD_ADDR));   
                ind_msg_p->reason = Parms->errCode;
                hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_DISCONNECTED_IND, (local_para_struct*)ind_msg_p);
            }
        }
        break;
    }
    case HFG_EVENT_AUDIO_CONNECTED:
    {
        bt_hfg_sco_connected_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_sco_connected_ind_struct*) construct_local_para(sizeof(bt_hfg_sco_connected_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->status = Parms->status;
        /* ALPS00235156 : DS-980 will goes wrong if we send A2DP streaming 
            data and SCO data at the same time.
        */
#ifdef __BT_A2DP_PROFILE__        
        if(Parms->status == BT_STATUS_SUCCESS && hfg_needSuspendA2DPStreaming(&adp_channel->channel)){
            bt_prompt_trace(MOD_BT, "[HFG][Info] suspending A2DP streamingon SCO connected");
            a2dp_set_sco_state(1);
        }else{
            a2dp_set_sco_state(0);
        }
#endif
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SCO_CONNECTED_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_AUDIO_DISCONNECTED:
    {
        bt_hfg_sco_disconnected_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_sco_disconnected_ind_struct*) construct_local_para(sizeof(bt_hfg_sco_disconnected_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
#ifdef __BT_A2DP_PROFILE__        
        a2dp_set_sco_state(0);
#endif
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SCO_DISCONNECTED_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_HANDSFREE_FEATURES:
    {
        bt_hfg_handsfree_features_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_handsfree_features_ind_struct*) construct_local_para(sizeof(bt_hfg_handsfree_features_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->hfFeatures = Parms->p.features;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_HANDSFREE_FEATURES_IND, (local_para_struct*)ind_msg_p);        
        break;
    }
    
    case HFG_EVENT_READ_INDICATORS:
    {
        bt_hfg_query_indicator_value_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_query_indicator_value_ind_struct*) construct_local_para(sizeof(bt_hfg_query_indicator_value_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_QUERY_INDICATOR_VALUE_IND, (local_para_struct*)ind_msg_p);
        break;
    }
    
    case HFG_EVENT_ENABLE_EXTENDED_ERRORS:
    {
        bt_hfg_enable_extended_errors_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_enable_extended_errors_ind_struct*) construct_local_para(sizeof(bt_hfg_enable_extended_errors_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_ENABLE_EXTENDED_ERRORS_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_ANSWER_CALL:
    {
        bt_hfg_answer_call_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_answer_call_ind_struct*) construct_local_para(sizeof(bt_hfg_answer_call_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_ANSWER_CALL_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_DIAL_NUMBER:
    {
        bt_hfg_dial_number_ind_struct *ind_msg_p;
        len = OS_StrLen(Parms->p.phoneNumber);
        ind_msg_p = (bt_hfg_dial_number_ind_struct*) construct_local_para(sizeof(bt_hfg_dial_number_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->datasize = len+1;
        OS_MemCopy((U8*)ind_msg_p->number, (const U8*)Parms->p.phoneNumber, len+1);
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_DIAL_NUMBER_IND, (local_para_struct*)ind_msg_p);
        break;
    }
    
    case HFG_EVENT_MEMORY_DIAL:
    {
        bt_hfg_memory_dial_ind_struct *ind_msg_p;
        len = OS_StrLen(Parms->p.phoneNumber);
        ind_msg_p = (bt_hfg_memory_dial_ind_struct*) construct_local_para(sizeof(bt_hfg_memory_dial_ind_struct), TD_UL);        
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->index = (U16)AtAtoI((const U8*)Parms->p.phoneNumber,(U8)len);
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_MEMORY_DIAL_IND, (local_para_struct*)ind_msg_p);        
        break;
    }

    case HFG_EVENT_REDIAL:
    {
        bt_hfg_redial_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_redial_ind_struct*) construct_local_para(sizeof(bt_hfg_redial_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_REDIAL_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_CALL_HOLD:
    {
        bt_hfg_call_hold_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_call_hold_ind_struct*) construct_local_para(sizeof(bt_hfg_call_hold_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->hold = *Parms->p.hold;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_CALL_HOLD_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_QUERY_RESPONSE_HOLD:
    {
        bt_hfg_query_response_hold_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_query_response_hold_ind_struct*) construct_local_para(sizeof(bt_hfg_query_response_hold_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_QUERY_RESPONSE_HOLD_IND, (local_para_struct*)ind_msg_p);
        break;
    }
        
    case HFG_EVENT_RESPONSE_HOLD:
    {
        bt_hfg_set_response_hold_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_set_response_hold_ind_struct*) construct_local_para(sizeof(bt_hfg_set_response_hold_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->respHold = Parms->p.respHold;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SET_RESPONSE_HOLD_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_HANGUP:
    {
        bt_hfg_hang_up_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_hang_up_ind_struct*) construct_local_para(sizeof(bt_hfg_hang_up_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_HANGUP_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_GENERATE_DTMF:
    {
        bt_hfg_generate_dtmf_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_generate_dtmf_ind_struct*) construct_local_para(sizeof(bt_hfg_generate_dtmf_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->dtmf = Parms->p.dtmf;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_GENERATE_DTMF_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_GET_LAST_VOICE_TAG:
    {
        bt_hfg_get_last_voice_tag_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_get_last_voice_tag_ind_struct*) construct_local_para(sizeof(bt_hfg_get_last_voice_tag_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_GET_LAST_VOICE_TAG_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_LIST_CURRENT_CALLS:
    {
        bt_hfg_list_current_calls_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_list_current_calls_ind_struct*) construct_local_para(sizeof(bt_hfg_list_current_calls_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_LIST_CURRENT_CALLS_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_QUERY_SUBSCRIBER_NUMBER:
    {
        bt_hfg_query_subscriber_number_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_query_subscriber_number_ind_struct*) construct_local_para(sizeof(bt_hfg_query_subscriber_number_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_QUERY_SUBSCRIBER_NUMBER_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_QUERY_NETWORK_OPERATOR:
    {
        bt_hfg_query_network_operator_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_query_network_operator_ind_struct*) construct_local_para(sizeof(bt_hfg_query_network_operator_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_QUERY_NETWORK_OPERATOR_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_REPORT_MIC_VOLUME:
    {
        bt_hfg_report_mic_volume_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_report_mic_volume_ind_struct*) construct_local_para(sizeof(bt_hfg_report_mic_volume_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->vol = Parms->p.gain;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_REPORT_MIC_VOLUME_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_REPORT_SPK_VOLUME:
    {
        bt_hfg_report_speaker_volume_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_report_speaker_volume_ind_struct*) construct_local_para(sizeof(bt_hfg_report_speaker_volume_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->vol = Parms->p.gain;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_REPORT_SPK_VOLUME_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_ENABLE_CALLER_ID:
    {
        bt_hfg_enable_caller_id_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_enable_caller_id_ind_struct*) construct_local_para(sizeof(bt_hfg_enable_caller_id_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_ENABLE_CALLER_ID_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_ENABLE_CALL_WAITING:
    {
        bt_hfg_enable_call_waiting_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_enable_call_waiting_ind_struct*) construct_local_para(sizeof(bt_hfg_enable_call_waiting_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->enable = Parms->p.enabled;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_ENABLE_CALL_WAITING_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_DISABLE_NREC:
    {
        bt_hfg_disable_nrec_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_disable_nrec_ind_struct*) construct_local_para(sizeof(bt_hfg_disable_nrec_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_DISABLE_NREC_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_ENABLE_VOICE_RECOGNITION:
    {
        bt_hfg_enable_voice_recognition_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_enable_voice_recognition_ind_struct*) construct_local_para(sizeof(bt_hfg_enable_voice_recognition_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->enable = Parms->p.enabled;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_ENABLE_VOICE_RECOGNITION_IND, (local_para_struct*)ind_msg_p);
        break;
    }
    
    case HFG_EVENT_KEYPAD_CONTROL:
    {
        bt_hfg_keypad_control_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_keypad_control_ind_struct*) construct_local_para(sizeof(bt_hfg_keypad_control_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->button = Parms->p.button;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_KEYPAD_CONTROL_IND, (local_para_struct*)ind_msg_p);        
        break;
    }
    
    case HFG_EVENT_AT_COMMAND_DATA:
    {
        bt_hfg_at_command_data_ind_struct *ind_msg_p;
        len = Parms->p.data->dataLen;
        ind_msg_p = (bt_hfg_at_command_data_ind_struct*) construct_local_para(sizeof(bt_hfg_at_command_data_ind_struct)-MAX_AT_STRING_LEN+len+1, TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->datasize = len;
        OS_MemCopy((U8*)ind_msg_p->atcmd, (const U8*)Parms->p.data->rawData, len);
        ind_msg_p->atcmd[len] = '\0';
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_AT_COMMAND_DATA_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_QUERY_SUPPORTED_CHARSET:
    {
        bt_hfg_query_supported_charset_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_query_supported_charset_ind_struct*) construct_local_para(sizeof(bt_hfg_query_supported_charset_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_QUERY_SUPPORTED_CHARSET_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_QUERY_SELECTED_CHARSET:
    {
        bt_hfg_query_selected_charset_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_query_selected_charset_ind_struct*) construct_local_para(sizeof(bt_hfg_query_selected_charset_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_QUERY_SELECTED_CHARSET_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_SELECT_CHARSET:
    {
        bt_hfg_select_charset_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_select_charset_ind_struct*) construct_local_para(sizeof(bt_hfg_select_charset_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
         if(Parms->p.charset)
            OS_MemCopy( (U8*)ind_msg_p->charset, (const U8*)Parms->p.charset, OS_StrLen(Parms->p.charset)+1);
         else
            ind_msg_p->charset[0] = 0;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SELECT_CHARSET_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_QUERY_MODEL_ID:
    {
        bt_hfg_query_model_id_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_query_model_id_ind_struct*) construct_local_para(sizeof(bt_hfg_query_model_id_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_QUERY_MODEL_ID_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    case HFG_EVENT_QUERY_MANUFACTURE_ID:
    {
        bt_hfg_query_manufacture_id_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_query_manufacture_id_ind_struct*) construct_local_para(sizeof(bt_hfg_query_manufacture_id_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_QUERY_MANUFACTURE_ID_IND, (local_para_struct*)ind_msg_p);
        break;
    }

    /***************************************************************************
    *   Phonebook related
    ***************************************************************************/
#if AT_PHONEBOOK == XA_ENABLED
    /* AT+CPBS=<storage> : AT_SELECT_PHONEBOOK_STORAGE */
    case HFG_EVENT_QUERY_SUPPORTED_PHONEBOOK:
    {
        bt_hfg_query_supported_phonebook_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_query_supported_phonebook_ind_struct*) construct_local_para(sizeof(bt_hfg_query_supported_phonebook_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_QUERY_SUPPORTED_PHONEBOOK_IND, (local_para_struct*)ind_msg_p);        
        break;
    }

    case HFG_EVENT_SELECT_PHONEBOOK:
    {
        bt_hfg_select_phonebook_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_select_phonebook_ind_struct*) construct_local_para(sizeof(bt_hfg_select_phonebook_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->selected = Parms->p.pbStorage;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_SELECT_PHONEBOOK_IND, (local_para_struct*)ind_msg_p);        
        break;
    }

    case HFG_EVENT_QUERY_SELECTED_PHONEBOOK:
    {
        bt_hfg_query_selected_phonebook_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_query_selected_phonebook_ind_struct*) construct_local_para(sizeof(bt_hfg_query_selected_phonebook_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_QUERY_SELECTED_PHONEBOOK_IND, (local_para_struct*)ind_msg_p);        
        break;
    }

    case HFG_EVENT_QUERY_READ_PBENTRY_INFO:
    {
        bt_hfg_read_phonebook_info_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_read_phonebook_info_ind_struct*) construct_local_para(sizeof(bt_hfg_read_phonebook_info_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_READ_PHONEBOOK_INFO_IND, (local_para_struct*)ind_msg_p);        
        break;
    }

    case HFG_EVENT_READ_PBENTRY:
    {
        bt_hfg_read_phonebook_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_read_phonebook_ind_struct*) construct_local_para(sizeof(bt_hfg_read_phonebook_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->read.first = Parms->p.pbRead->first;
        ind_msg_p->read.last = Parms->p.pbRead->last;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_READ_PHONEBOOK_IND, (local_para_struct*)ind_msg_p);        
        break;
    }

    case HFG_EVENT_QUERY_FIND_PBENTRY_INFO:
    {
        bt_hfg_find_phonebook_info_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_find_phonebook_info_ind_struct*) construct_local_para(sizeof(bt_hfg_find_phonebook_info_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_FIND_PHONEBOOK_INFO_IND, (local_para_struct*)ind_msg_p);        
        break;
    }

    case HFG_EVENT_FIND_PBENTRY:
    {
        bt_hfg_find_phonebook_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_find_phonebook_ind_struct*) construct_local_para(sizeof(bt_hfg_find_phonebook_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        len = OS_StrLen(Parms->p.pbFind->text);
        OS_MemCopy((U8*)ind_msg_p->text, (const U8*)Parms->p.pbFind->text, len);
        ind_msg_p->text[len] = 0;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_FIND_PHONEBOOK_IND, (local_para_struct*)ind_msg_p);        
        break;
    }

    case HFG_EVENT_QUERY_WRITE_PBENTRY_INFO:
    {
        bt_hfg_write_phonebook_info_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_write_phonebook_info_ind_struct*) construct_local_para(sizeof(bt_hfg_write_phonebook_info_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_WRITE_PHONEBOOK_INFO_IND, (local_para_struct*)ind_msg_p);        
        break;
    }

    case HFG_EVENT_WRITE_PBENTRY:
    {
        bt_hfg_write_phonebook_ind_struct *ind_msg_p;
        ind_msg_p = (bt_hfg_write_phonebook_ind_struct*) construct_local_para(sizeof(bt_hfg_write_phonebook_ind_struct), TD_UL);
        ind_msg_p->pContext = (void*)adp_channel;
        ind_msg_p->user_context = adp_channel->userContext;
        ind_msg_p->index = Parms->p.pbWrite->index;
        ind_msg_p->type = Parms->p.pbWrite->type;
        if(Parms->p.pbWrite->number)
        {
            len = OS_StrLen(Parms->p.pbWrite->number);
            OS_MemCopy((U8*)ind_msg_p->number, (const U8*)Parms->p.pbWrite->number, len);
            ind_msg_p->number[len] = 0;
        }
        else
        {
            ind_msg_p->number[0] = 0;
        }
        if(Parms->p.pbWrite->text)
        {
            len = OS_StrLen(Parms->p.pbWrite->text);
            OS_MemCopy((U8*)ind_msg_p->text, (const U8*)Parms->p.pbWrite->text, len);
            ind_msg_p->text[len] = 0;
        }
        else
        {
            ind_msg_p->text[0] = 0;
        }
        ind_msg_p->type = Parms->p.pbWrite->type;
        hfg_send_channel_msg(adp_channel, MSG_ID_BT_HFG_WRITE_PHONEBOOK_IND, (local_para_struct*)ind_msg_p);        
        break;
    }
#endif /* AT_PHONEBOOK == XA_ENABLED */
    /***************************************************************************
    *   SMS related
    ***************************************************************************/
#if AT_SMS == XA_ENABLED
    /* AT_SELECT_SMS_SERVICE */
    case HFG_EVENT_QUERY_SUPPORTED_SMS_SERVICE:
        btmtk_adp_hfg_handle_sms_query_supported_srvice_ind(adp_channel, Parms);
        break;

    case HFG_EVENT_QUERY_SELECTED_SMS_SERVICE:
        btmtk_adp_hfg_handle_sms_query_selected_srvice_ind(adp_channel, Parms);
        break;

    case HFG_EVENT_SELECT_SMS_SERVICE:
        btmtk_adp_hfg_handle_sms_select_srvice_ind(adp_channel, Parms);
        break;

    /* AT_PREFERRED_SMS_STORAGE */
    case HFG_EVENT_QUERY_SUPPORTED_PREF_MSG_STORAGE:
        btmtk_adp_hfg_handle_sms_query_supported_pref_storage_ind(adp_channel, Parms);
        break;

    case HFG_EVENT_QUERY_SELECTED_PREF_MSG_STORAGE:
        btmtk_adp_hfg_handle_sms_query_selected_pref_storage_ind(adp_channel, Parms);
        break;

    case HFG_EVENT_SELECT_PREF_MSG_STORAGE:
        btmtk_adp_hfg_handle_sms_select_pref_storage_ind(adp_channel, Parms);
        break;

    /* AT_SMS_MESSAGE_FORMAT */
    case HFG_EVENT_QUERY_SUPPORTED_MSG_FORMAT:
        btmtk_adp_hfg_handle_sms_query_supported_format_ind(adp_channel, Parms);
        break;

    case HFG_EVENT_QUERY_SELECTED_MSG_FORMAT:
        btmtk_adp_hfg_handle_sms_query_selected_format_ind(adp_channel, Parms);
        break;

    case HFG_EVENT_SELECT_MSG_FORMAT:
        btmtk_adp_hfg_handle_sms_select_format_ind(adp_channel, Parms);
        break;

    /* AT_SMS_SERVICE_CENTER */
    case HFG_EVENT_QUERY_SERVICE_CENTRE:
        btmtk_adp_hfg_handle_sms_query_service_centre_ind(adp_channel, Parms);
        break;

    case HFG_EVENT_SET_SERVICE_CENTRE:
        btmtk_adp_hfg_handle_sms_set_service_centre_ind(adp_channel, Parms);
        break;

    /* AT_SET_TEXT_MODE_PARMS */
    case HFG_EVENT_QUERY_TEXT_MODE_PARAMS:
        btmtk_adp_hfg_handle_sms_query_text_mode_params_ind(adp_channel, Parms);
        break;

    case HFG_EVENT_SET_TEXT_MODE_PARAMS:
        btmtk_adp_hfg_handle_sms_set_text_mode_params_ind(adp_channel, Parms);
        break;

    /* AT_SMS_SHOW_TEXT_MODE */
    case HFG_EVENT_QUERY_SUPPORTED_SHOW_PARAMS:
        btmtk_adp_hfg_handle_sms_query_supported_show_params_ind(adp_channel, Parms);
        break;

    case HFG_EVENT_QUERY_SELECTED_SHOW_PARAMS:
        btmtk_adp_hfg_handle_sms_query_selected_show_params_ind(adp_channel, Parms);
        break;

    case HFG_EVENT_SET_SHOW_PARAMS:
        btmtk_adp_hfg_handle_sms_set_show_params_ind(adp_channel, Parms);
        break;

    /* AT_NEW_MESSAGE_INDICATION */
    case HFG_EVENT_QUERY_SUPPORTED_NEW_MSG_INDICATION:
        btmtk_adp_hfg_handle_sms_query_supported_new_msg_indication_ind(adp_channel, Parms);
        break;

    case HFG_EVENT_QUERY_SELECTED_NEW_MSG_INDICATION:
        btmtk_adp_hfg_handle_sms_query_selected_new_msg_indication_ind(adp_channel, Parms);
        break;

    case HFG_EVENT_SET_NEW_MSG_INDICATION:
        btmtk_adp_hfg_handle_sms_set_new_msg_indication_ind(adp_channel, Parms);
        break;

    /* AT_LIST_MESSAGES */
    case HFG_EVENT_QUERY_SUPPORTED_LIST_STATUS:
        btmtk_adp_hfg_handle_sms_query_supported_list_status_ind(adp_channel, Parms);
        break;

    case HFG_EVENT_LIST_MSG:
        btmtk_adp_hfg_handle_sms_list_msg_ind(adp_channel, Parms);
        break;

    /* AT_READ_MESSAGE */
    case HFG_EVENT_READ_MSG:
        btmtk_adp_hfg_handle_sms_read_msg_ind(adp_channel, Parms);
        break;

    /* AT_SEND_MESSAGE */
    case HFG_EVENT_SEND_MSG:
        btmtk_adp_hfg_handle_sms_send_msg_ind(adp_channel, Parms);
        break;

    /* AT_SEND_STORED_MESSAGE */
    case HFG_EVENT_SEND_STORED_MSG:
        btmtk_adp_hfg_handle_sms_send_stored_msg_ind(adp_channel, Parms);
        break;

    /* AT_STORE_MESSAGE */
    case HFG_EVENT_WRITE_MSG:
        btmtk_adp_hfg_handle_sms_write_msg_ind(adp_channel, Parms);
        break;

    /* AT_DELETE_MESSAGE */
    case HFG_EVENT_DELETE_MSG:
        btmtk_adp_hfg_handle_sms_delete_msg_ind(adp_channel, Parms);
        break;
#endif /* AT_SMS == XA_ENABLED */

    case HFG_EVENT_RESPONSE_COMPLETE:
    {
        HfgResponse* response = (HfgResponse*)(Parms->p.response);
        HfgAdpResponse *adp_response = ContainingRecord(Parms->p.response, HfgAdpResponse, response);
        bt_hfg_req_complete_cnf_struct *cnf_msg_p;
        if (!IsNodeOnList(&adp_channel->rsp_queue, &response->node)) 
        {
            cnf_msg_p = (bt_hfg_req_complete_cnf_struct*) construct_local_para(sizeof(bt_hfg_req_complete_cnf_struct), TD_UL);
            cnf_msg_p->result = Parms->status;
            cnf_msg_p->req_context = adp_response->req_context;
            switch(response->type)
            {
            case AT_CALL_WAIT_NOTIFY:
                if(response->p.hf.wait.number)
                free_ctrl_buffer((U8*)response->p.hf.wait.number);
                break;
            case AT_CALL_ID:
                if(response->p.hf.callId.number)
                free_ctrl_buffer((U8*)response->p.hf.callId.number);
                break;
            case AT_VOICE_TAG:
                if(response->p.hf.voiceTag.number)
                free_ctrl_buffer((U8*)response->p.hf.voiceTag.number);
                break;
            case AT_SUBSCRIBER_NUM:
                if(response->p.hf.subscribeNum.number)
                free_ctrl_buffer((U8*)response->p.hf.subscribeNum.number);
                break;
            case AT_LIST_CURRENT_CALLS:
                if(response->p.hf.currentCalls.number)
                free_ctrl_buffer((U8*)response->p.hf.currentCalls.number);
                break;
            case (AT_NETWORK_OPERATOR|AT_READ):
                if(response->p.hf.networkOper_read.oper)
                free_ctrl_buffer((U8*)response->p.hf.networkOper_read.oper);
                break;
            case (AT_SELECT_CHARACTER_SET|AT_TEST):
                if(response->p.charset_test)
                    free_ctrl_buffer((U8*)response->p.charset_test);
                break;
            case (AT_SELECT_CHARACTER_SET|AT_READ):
                if(response->p.charset)
                    free_ctrl_buffer((U8*)response->p.charset);
                break;
            case AT_MODEL_ID:
                if(response->p.modelID)
                    free_ctrl_buffer((U8*)response->p.modelID);
                break;
            case AT_MANUFACTURE_ID:
                if(response->p.manufactureID)
                    free_ctrl_buffer((U8*)response->p.manufactureID);
                break;
            case AT_RAW:
                free_ctrl_buffer(response->p.data);
                break;
            /****************************************
            *   Phonebook related handling
            ****************************************/
#if AT_PHONEBOOK == XA_ENABLED
            case (AT_READ_PHONEBOOK_ENTRY|AT_TEST):
                free_ctrl_buffer((U8*)response->p.pb.read_test.range);
                break;
            case AT_READ_PHONEBOOK_ENTRY:
            case AT_FIND_PHONEBOOK_ENTRY:
                free_ctrl_buffer((U8*)response->p.pb.read.number);
                free_ctrl_buffer((U8*)response->p.pb.read.text);
                break;
            case (AT_WRITE_PHONEBOOK_ENTRY|AT_TEST):
                free_ctrl_buffer((U8*)response->p.pb.write_test.range);
                free_ctrl_buffer((U8*)response->p.pb.write_test.types);
                break;
#endif /* AT_PHONEBOOK == XA_ENABLED */
            /****************************************
            *   SMS related handling
            ****************************************/
#if AT_SMS == XA_ENABLED
            case (AT_SELECT_SMS_SERVICE|AT_TEST):
                free_ctrl_buffer((U8*)response->p.sms.supportedServices);
                break;
            case (AT_SMS_MESSAGE_FORMAT|AT_TEST):
                free_ctrl_buffer((U8*)response->p.sms.supportedMsgFormat);
                break;
            case (AT_SMS_SERVICE_CENTER|AT_READ):
                free_ctrl_buffer((U8*)response->p.sms.SCA.sca);
                break;
            case (AT_SET_TEXT_MODE_PARMS|AT_READ):
                if(response->p.sms.textModeParams.strVp)
                    free_ctrl_buffer((U8*)response->p.sms.textModeParams.strVp);
                break;
            case (AT_SMS_SHOW_TEXT_MODE|AT_TEST):
                free_ctrl_buffer((U8*)response->p.sms.showModes);
                break;
            case (AT_SET_CBM_TYPES|AT_READ):
                if(response->p.sms.CBMTypes.mids)
                    free_ctrl_buffer((U8*)response->p.sms.CBMTypes.mids);
                if(response->p.sms.CBMTypes.dcss)
                    free_ctrl_buffer((U8*)response->p.sms.CBMTypes.dcss);
                break;
            case (AT_SET_CBM_TYPES|AT_TEST):
                free_ctrl_buffer((U8*)response->p.sms.supportedCBMMode);
                break;
            case (AT_NEW_MESSAGE_INDICATION|AT_TEST):
                free_ctrl_buffer((U8*)response->p.sms.newMsgInd_test);
                break;
            case (AT_LIST_MESSAGES|AT_TEST):
                free_ctrl_buffer((U8*)response->p.sms.supportedStatus);
                break;
            case AT_LIST_MESSAGES:
                switch(response->p.sms.listMsg.type)
                {
                case AT_SMS_TYPE_DELIVER:
                case AT_SMS_TYPE_SUBMIT:
                    if(response->p.sms.listMsg.msg.deliver.addr)
                        free_ctrl_buffer((U8*)response->p.sms.listMsg.msg.deliver.addr);
                    if(response->p.sms.listMsg.msg.deliver.alpha)
                        free_ctrl_buffer((U8*)response->p.sms.listMsg.msg.deliver.alpha);
                    if(response->p.sms.listMsg.msg.deliver.SCTimeStamp)
                        free_ctrl_buffer((U8*)response->p.sms.listMsg.msg.deliver.SCTimeStamp);
                    if(response->p.sms.listMsg.msg.deliver.data)
                        free_ctrl_buffer((U8*)response->p.sms.listMsg.msg.deliver.data);
                    break;
                case AT_SMS_TYPE_STATUS:
                    if(response->p.sms.listMsg.msg.status.ra)
                        free_ctrl_buffer((U8*)response->p.sms.listMsg.msg.status.ra);
                    if(response->p.sms.listMsg.msg.status.SCTimeStamp)
                        free_ctrl_buffer((U8*)response->p.sms.listMsg.msg.status.SCTimeStamp);
                    if(response->p.sms.listMsg.msg.status.dt)
                        free_ctrl_buffer((U8*)response->p.sms.listMsg.msg.status.dt);
                    break;
                case AT_SMS_TYPE_COMMAND:
                    break;
                case AT_SMS_TYPE_CBM:
                    if(response->p.sms.listMsg.msg.cbm.data)
                        free_ctrl_buffer((U8*)response->p.sms.listMsg.msg.cbm.data);
                    break;
                case AT_SMS_TYPE_PDU:
                    if(response->p.sms.listMsg.msg.pdu.alpha)
                        free_ctrl_buffer((U8*)response->p.sms.listMsg.msg.pdu.alpha);
                    if(response->p.sms.listMsg.msg.pdu.PDU)
                        free_ctrl_buffer((U8*)response->p.sms.listMsg.msg.pdu.PDU);
                    break;
                default:
                    break;
                }
                break;
            case AT_READ_MESSAGE:
                switch(response->p.sms.readMsg.type)
                {
                case AT_SMS_TYPE_DELIVER:
                    if(response->p.sms.readMsg.msg.deliver.addr)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.deliver.addr);
                    if(response->p.sms.readMsg.msg.deliver.alpha)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.deliver.alpha);
                    if(response->p.sms.readMsg.msg.deliver.SCTimeStamp)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.deliver.SCTimeStamp);
                    if(response->p.sms.readMsg.msg.deliver.SCAddr)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.deliver.SCAddr);
                    if(response->p.sms.readMsg.msg.deliver.data)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.deliver.data);
                    break;
                case AT_SMS_TYPE_SUBMIT:
                    if(response->p.sms.readMsg.msg.submit.addr)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.submit.addr);
                    if(response->p.sms.readMsg.msg.submit.alpha)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.submit.alpha);
                    if(response->p.sms.readMsg.msg.submit.strVp)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.submit.strVp);
                    if(response->p.sms.readMsg.msg.submit.SCAddr)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.submit.SCAddr);
                    if(response->p.sms.readMsg.msg.submit.data)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.submit.data);
                    break;
                case AT_SMS_TYPE_STATUS:
                    if(response->p.sms.readMsg.msg.status.ra)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.status.ra);
                    if(response->p.sms.readMsg.msg.status.SCTimeStamp)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.status.SCTimeStamp);
                    if(response->p.sms.readMsg.msg.status.dt)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.status.dt);
                    break;
                case AT_SMS_TYPE_COMMAND:
                    break;
                case AT_SMS_TYPE_CBM:
                    if(response->p.sms.readMsg.msg.cbm.data)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.cbm.data);
                    break;
                case AT_SMS_TYPE_PDU:
                    if(response->p.sms.readMsg.msg.pdu.alpha)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.pdu.alpha);
                    if(response->p.sms.readMsg.msg.pdu.PDU)
                        free_ctrl_buffer((U8*)response->p.sms.readMsg.msg.pdu.PDU);
                    break;
                default:
                    break;
                }
                break;
            case AT_SEND_MESSAGE:
            case AT_SEND_STORED_MESSAGE:
                if(response->p.sms.sendMsg_rsp.scts)
                    free_ctrl_buffer((U8*)response->p.sms.sendMsg_rsp.scts);
                if(response->p.sms.sendMsg_rsp.ackpdu)
                    free_ctrl_buffer((U8*)response->p.sms.sendMsg_rsp.ackpdu);
                break;
#endif /* AT_SMS == XA_ENABLED */
            default:
                break;
            }
            hfg_send_channel_msg(adp_channel, adp_response->req_msg, (local_para_struct*)cnf_msg_p);
            hfg_free_rsp(adp_channel, Parms->p.response);
        }
        break;
    }

    default:
        Report(("hfga_app_callback: Unsupported event."));
        break;
    }
}


