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
 * Bt_hfg_hdl.c
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
//#include "windows.h"
//#include "string.h"
#include "bt_types.h"
#include "bt_mmi.h"
#include "bluetooth_hfg_struct.h"
#include "bt_message.h"
#include "bt_hfg_struct.h"
//#include "bt_adp_msg.h"
//#include "bluetooth_hfg_message.h"

#define BT_STATUS_SUCCESS 0

void btmtk_hfg_handle_activate_cnf(bt_hfg_activate_cnf_struct *cnf_msg)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_activate_cnf_struct *cnf_msg;

    //cnf_msg = (bt_hfg_activate_cnf_struct*)message->ilm_data;

    hfgContext = (HfgChannelContext*)cnf_msg->req_context;
    // param.event = HFG_EVENT_ACTIVATE_CONFIRM;
    param.status = cnf_msg->result;
    hfgContext->hfgContext = cnf_msg->pContext;
    param.p.context = (void*)hfgContext;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_ACTIVATE_CONFIRM, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_deactivate_cnf(bt_hfg_deactivate_cnf_struct *cnf_msg)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_deactivate_cnf_struct *cnf_msg;
    
	//cnf_msg = (bt_hfg_deactivate_cnf_struct*)message->ilm_data;
    // param.event = HFG_EVENT_DEACTIVATE_CONFIRM;
    hfgContext = (HfgChannelContext*)cnf_msg->req_context;
    param.status = cnf_msg->result;
    param.p.context = (void*)hfgContext;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_DEACTIVATE_CONFIRM, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_general_cnf(bt_hfg_general_cnf_struct *cnf_msg, BT_CALLBACK_EVENT event)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_general_cnf_struct *cnf_msg;
    
	//cnf_msg = (bt_hfg_general_cnf_struct*)message->ilm_data;
    // param.event = event;
    hfgContext = (HfgChannelContext*)cnf_msg->req_context;
    param.status = cnf_msg->result;
    param.p.context = (void*)hfgContext;
    hfgContext->callback((void*)hfgContext, event, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_connect_cnf(bt_hfg_general_cnf_struct *cnf_msg)
{
    btmtk_hfg_handle_general_cnf(cnf_msg, EVENT_HFG_CONNECT_CONFIRM);
}

void btmtk_hfg_handle_accept_channel_cnf(bt_hfg_general_cnf_struct *cnf_msg)
{
    btmtk_hfg_handle_general_cnf(cnf_msg, EVENT_HFG_ACCEPT_CHANNEL_CONFIRM);
}

void btmtk_hfg_handle_reject_channel_cnf(bt_hfg_general_cnf_struct *cnf_msg)
{
    btmtk_hfg_handle_general_cnf(cnf_msg, EVENT_HFG_REJECT_CHANNEL_CONFIRM);
}

void btmtk_hfg_handle_disconnect_cnf(bt_hfg_general_cnf_struct *cnf_msg)
{
    btmtk_hfg_handle_general_cnf(cnf_msg, EVENT_HFG_DISCONNECT_CONFIRM);
}

void btmtk_hfg_handle_sco_connect_cnf(bt_hfg_general_cnf_struct *cnf_msg)
{
    btmtk_hfg_handle_general_cnf(cnf_msg, EVENT_HFG_SCO_CONNECT_CONFIRM);
}

void btmtk_hfg_handle_sco_disconnect_cnf(bt_hfg_general_cnf_struct *cnf_msg)
{
    btmtk_hfg_handle_general_cnf(cnf_msg, EVENT_HFG_SCO_DISCONNECT_CONFIRM);
}

void btmtk_hfg_handle_connect_req_ind(bt_hfg_connect_req_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_connect_req_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_connect_req_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_AUTH_REQ;
    param.status = BT_STATUS_SUCCESS;
    param.p.addr  = &msg_p->bt_addr;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_AUTH_REQ, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_rfcomm_connected_ind(bt_hfg_rfcomm_connected_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_rfcomm_connected_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_rfcomm_connected_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_RFCOMM_CONNECTED;
    param.status = BT_STATUS_SUCCESS;
    param.p.addr  = &msg_p->bt_addr;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_RFCOMM_CONNECTED, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_connected_ind(bt_hfg_connected_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_connected_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_connected_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_SERVICE_CONNECTED;
    param.status = BT_STATUS_SUCCESS;
    param.p.addr  = &msg_p->bt_addr;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_SERVICE_CONNECTED, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_disconnected_ind(bt_hfg_disconnected_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_disconnected_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_disconnected_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_SERVICE_DISCONNECTED;
    param.status = BT_STATUS_SUCCESS;
    param.p.addr  = &msg_p->bt_addr;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_SERVICE_DISCONNECTED, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_sco_connected_ind(bt_hfg_sco_connected_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_sco_connected_ind_struct *msg_p;

    //msg_p = (bt_hfg_sco_connected_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_AUDIO_CONNECTED;
    param.status = msg_p->status;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_AUDIO_CONNECTED, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_sco_disconnected_ind(bt_hfg_sco_connected_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_sco_connected_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_sco_connected_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_AUDIO_DISCONNECTED;
    param.status = msg_p->status;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_AUDIO_DISCONNECTED, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_response_complete_ind(unsigned long msg_id, bt_hfg_general_cnf_struct *cnf_msg)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_general_cnf_struct *cnf_msg;
    //HfgResponse response;

	//cnf_msg = (bt_hfg_general_cnf_struct*)message->ilm_data;
    switch(msg_id)
    {
    case MSG_ID_BT_HFG_OK_CNF:
        //response.type = AT_OK;
        break;
    case MSG_ID_BT_HFG_ERROR_CNF:
        //response.type = AT_ERROR;
        break;
    case MSG_ID_BT_HFG_NO_CARRIER_CNF:
        //response.type = AT_NO_CARRIER;
        break;
        
    case MSG_ID_BT_HFG_BUSY_CNF:
        //response.type = AT_BUSY;
        break;
         
    case MSG_ID_BT_HFG_NO_ANSWER_CNF:
        //response.type = AT_NO_ANSWER;
        break;
    case MSG_ID_BT_HFG_DELAYED_CNF:
        //response.type = AT_DELAYED;
        break;
    case MSG_ID_BT_HFG_BLACKLISTED_CNF:
        //response.type = AT_BLACKLISTED;
        break;
    case MSG_ID_BT_HFG_RING_CNF:
        //response.type = AT_RING;
        break;
    case MSG_ID_BT_HFG_CALL_WAITING_CNF:
        //response.type = AT_CALL_WAIT_NOTIFY;
        break;
    case MSG_ID_BT_HFG_READ_INDICATOR_CNF:
        //response.type = (AT_INDICATORS|AT_READ);
        break;
    case MSG_ID_BT_HFG_CALL_LIST_CNF:
        //response.type = AT_LIST_CURRENT_CALLS;
        break;
    case MSG_ID_BT_HFG_CALL_ID_CNF:
        //response.type = AT_CALL_ID;
        break;
    case MSG_ID_BT_HFG_INDICATOR_REPORT_CNF:
        //response.type = (AT_INDICATORS|AT_READ);
        break;
    case MSG_ID_BT_HFG_SUBSCRIBER_NUMBER_CNF:
        //response.type = (AT_SUBSCRIBER_NUM|AT_READ);
        break;
    case MSG_ID_BT_HFG_VOICE_TAG_CNF:
        //response.type = AT_VOICE_TAG;
        break;
    case MSG_ID_BT_HFG_VOICE_REC_CNF:
        //response.type = AT_VOICE_RECOGNITION;
        break;
    case MSG_ID_BT_HFG_RESPONSE_HOLD_CNF:
        //response.type = AT_RESPONSE_AND_HOLD;
        break;
    case MSG_ID_BT_HFG_SPEAKER_GAIN_CNF:
        //response.type = AT_SPEAKER_GAIN;
        break;
    case MSG_ID_BT_HFG_MIC_GAIN_CNF:
        //response.type = AT_MICROPHONE_GAIN;
        break;
    case MSG_ID_BT_HFG_RINGTONE_STATUS_CNF:
        //response.type = AT_IN_BAND_RING_TONE;
        break;
    case MSG_ID_BT_HFG_SEND_DATA_CNF:
        //response.type = AT_RAW;
        break;
    }
    
    // param.event = HFG_EVENT_RESPONSE_COMPLETE;
    hfgContext = (HfgChannelContext*)cnf_msg->req_context;
    param.status = cnf_msg->result;
    param.p.response = NULL;//&response;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_RESPONSE_COMPLETE, &param, sizeof(HfgCallbackParameters));
}

/* AT CMD indication */
void btmtk_hfg_handle_handsfree_features_ind(bt_hfg_handsfree_features_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_handsfree_features_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_handsfree_features_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_HANDSFREE_FEATURES;
    param.status = BT_STATUS_SUCCESS;
    param.p.features = msg_p->hfFeatures;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_HANDSFREE_FEATURES, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_query_indicator_value_ind(bt_hfg_query_indicator_value_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_query_indicator_value_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_query_indicator_value_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_READ_INDICATORS;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_READ_INDICATORS, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_enable_extended_errors_ind(bt_hfg_enable_extended_errors_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_enable_extended_errors_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_enable_extended_errors_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_ENABLE_EXTENDED_ERRORS;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_ENABLE_EXTENDED_ERRORS, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_answer_call_ind(bt_hfg_answer_call_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_answer_call_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_answer_call_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_ANSWER_CALL;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_ANSWER_CALL, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_dial_number_ind(bt_hfg_dial_number_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_dial_number_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_dial_number_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_DIAL_NUMBER;
    param.status = BT_STATUS_SUCCESS;
    param.p.phoneNumber = msg_p->number;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_DIAL_NUMBER, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_memory_dial_ind(bt_hfg_memory_dial_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_memory_dial_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_memory_dial_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_MEMORY_DIAL;
    param.status = BT_STATUS_SUCCESS;
    param.p.index = msg_p->index;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_MEMORY_DIAL, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_redial_ind(bt_hfg_redial_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_redial_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_redial_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_REDIAL;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_REDIAL, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_call_hold_ind(bt_hfg_call_hold_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_call_hold_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_call_hold_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_CALL_HOLD;
    param.status = BT_STATUS_SUCCESS;
    param.p.hold = &msg_p->hold;
    /*
    param.p.hold->action = msg_p->action;
    param.p.hold->index = msg_p->index;
    */
    hfgContext->callback((void*)hfgContext, EVENT_HFG_CALL_HOLD, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_query_response_hold_ind(bt_hfg_query_response_hold_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_QUERY_RESPONSE_HOLD, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_set_response_hold_ind(bt_hfg_set_response_hold_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    param.status = BT_STATUS_SUCCESS;
    param.p.respHold = msg_p->respHold;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_RESPONSE_HOLD, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_hangup_ind(bt_hfg_hang_up_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_hang_up_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_hang_up_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_HANGUP;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_HANGUP, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_generate_dtmf_ind(bt_hfg_generate_dtmf_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_generate_dtmf_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_generate_dtmf_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_GENERATE_DTMF;
    param.status = BT_STATUS_SUCCESS;
    param.p.dtmf = msg_p->dtmf;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_GENERATE_DTMF, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_get_last_voice_tag_ind(bt_hfg_get_last_voice_tag_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_get_last_voice_tag_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_get_last_voice_tag_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_GET_LAST_VOICE_TAG;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_GET_LAST_VOICE_TAG, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_list_current_calls_ind(bt_hfg_list_current_calls_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_list_current_calls_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_list_current_calls_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_LIST_CURRENT_CALLS;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_LIST_CURRENT_CALLS, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_query_subscriber_number_ind(bt_hfg_query_subscriber_number_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_query_subscriber_number_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_query_subscriber_number_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_QUERY_SUBSCRIBER_NUMBER;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_QUERY_SUBSCRIBER_NUMBER, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_query_network_operator_ind(bt_hfg_query_network_operator_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_query_network_operator_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_query_network_operator_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_QUERY_NETWORK_OPERATOR;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_QUERY_NETWORK_OPERATOR, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_report_mic_volume_ind(bt_hfg_report_mic_volume_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_report_mic_volume_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_report_mic_volume_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_REPORT_MIC_VOLUME;
    param.status = BT_STATUS_SUCCESS;
    param.p.gain = msg_p->vol;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_REPORT_MIC_VOLUME, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_report_spk_volume_ind(bt_hfg_report_speaker_volume_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_report_speaker_volume_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_report_speaker_volume_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_REPORT_SPK_VOLUME;
    param.status = BT_STATUS_SUCCESS;
    param.p.gain = msg_p->vol;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_REPORT_SPK_VOLUME, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_enable_caller_id_ind(bt_hfg_enable_caller_id_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_enable_caller_id_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_enable_caller_id_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_ENABLE_CALLER_ID;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_ENABLE_CALLER_ID, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_enable_call_waiting_ind(bt_hfg_enable_call_waiting_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;    
    //bt_hfg_enable_call_waiting_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_enable_call_waiting_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_ENABLE_CALL_WAITING;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_ENABLE_CALL_WAITING, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_disable_nrec_ind(bt_hfg_disable_nrec_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_disable_nrec_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_disable_nrec_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_DISABLE_NREC;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_DISABLE_NREC, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_enable_voice_recognition_ind(bt_hfg_enable_voice_recognition_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_enable_voice_rec_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_enable_voice_rec_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_ENABLE_VOICE_RECOGNITION;
    param.status = BT_STATUS_SUCCESS;
    param.p.enabled = msg_p->enable;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_ENABLE_VOICE_RECOGNITION, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_keypad_control_ind(bt_hfg_keypad_control_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_keypad_control_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_keypad_control_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_KEYPAD_CONTROL;
    param.status = BT_STATUS_SUCCESS;
    param.p.button = msg_p->button;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_KEYPAD_CONTROL, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_query_supported_charset_ind(bt_hfg_query_supported_charset_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_QUERY_SUPPORTED_CHARSET, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_query_selected_charset_ind(bt_hfg_query_selected_charset_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_QUERY_SELECTED_CHARSET, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_select_charset_ind(bt_hfg_select_charset_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    param.status = BT_STATUS_SUCCESS;
    param.p.charset = msg_p->charset;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_SELECT_CHARSET, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_query_model_id_ind(bt_hfg_query_model_id_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_QUERY_MODEL_ID, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_query_manufacture_id_ind(bt_hfg_query_manufacture_id_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_QUERY_MANUFACTURE_ID, &param, sizeof(HfgCallbackParameters));
}

void btmtk_hfg_handle_at_command_ind(bt_hfg_at_command_data_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_at_cmd_data_ind_struct *msg_p;
    HfgAtData atdata;
	
    //msg_p = (bt_hfg_at_cmd_data_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_AT_COMMAND_DATA;
    param.status = BT_STATUS_SUCCESS;
    atdata.dataLen = msg_p->datasize;
    atdata.rawData = (U8*)msg_p->atcmd;
    param.p.data = &atdata;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_AT_COMMAND_DATA, &param, sizeof(HfgCallbackParameters));	
}


/**********************************************************
*   Phonebook related handler
**********************************************************/
/* AT+CPBS=? : AT_SELECT_PHONEBOOK_STORAGE */
void btmtk_hfg_handle_query_supported_phonebook_ind(bt_hfg_query_supported_phonebook_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_query_supported_phonebook_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_query_supported_phonebook_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_QUERY_SUPPORTED_PHONEBOOK;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_QUERY_SUPPORTED_PHONEBOOK, &param, sizeof(HfgCallbackParameters));
}

/* AT+CPBS=<storage> : AT_SELECT_PHONEBOOK_STORAGE */
void btmtk_hfg_handle_select_phonebook_ind(bt_hfg_select_phonebook_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_select_phonebook_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_select_phonebook_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_SELECT_PHONEBOOK;
    param.status = BT_STATUS_SUCCESS;
    param.p.pbStorage = msg_p->selected;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_SELECT_PHONEBOOK, &param, sizeof(HfgCallbackParameters));
}


/* AT+CPBS? : AT_SELECT_PHONEBOOK_STORAGE */
void btmtk_hfg_handle_query_selected_phonebook_ind(bt_hfg_query_selected_phonebook_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_query_selected_phonebook_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_query_selected_phonebook_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_QUERY_SELECTED_PHONEBOOK;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_QUERY_SELECTED_PHONEBOOK, &param, sizeof(HfgCallbackParameters));
}


/* AT+CPBR=? : AT_READ_PHONEBOOK_ENTRY|AT_TEST */
void btmtk_hfg_handle_read_phonebook_info_ind(bt_hfg_read_phonebook_info_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_read_phonebook_info_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_read_phonebook_info_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_QUERY_READ_PBENTRY_INFO;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_QUERY_READ_PBENTRY_INFO, &param, sizeof(HfgCallbackParameters));
}

/* AT+CPBR=<index1>[,<index2>] : AT_READ_PHONEBOOK_ENTRY */
void btmtk_hfg_handle_read_phonebook_ind(bt_hfg_read_phonebook_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_read_phonebook_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_read_phonebook_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_READ_PBENTRY;
    param.status = BT_STATUS_SUCCESS;
    param.p.pbRead = &msg_p->read;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_READ_PBENTRY, &param, sizeof(HfgCallbackParameters));
}

/* AT+CPBF=? : AT_FIND_PHONEBOOK_ENTRY|AT_TEST */
void btmtk_hfg_handle_find_phonebook_info_ind(bt_hfg_find_phonebook_info_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_find_phonebook_info_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_find_phonebook_info_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_QUERY_FIND_PBENTRY_INFO;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_QUERY_FIND_PBENTRY_INFO, &param, sizeof(HfgCallbackParameters));
}

/* AT+CPBF=<findtext> : AT_FIND_PHONEBOOK_ENTRY */
void btmtk_hfg_handle_find_phonebook_ind(bt_hfg_find_phonebook_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_find_phonebook_ind_struct *msg_p;
    HfgPbFind pbFind;
    
    //msg_p = (bt_hfg_find_phonebook_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_FIND_PBENTRY;
    param.status = BT_STATUS_SUCCESS;
    pbFind .text = msg_p->text;
    param.p.pbFind = &pbFind;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_FIND_PBENTRY, &param, sizeof(HfgCallbackParameters));
}

/* AT+CPBW=? : AT_WRITE_PHONEBOOK_ENTRY|AT_TEST */
void btmtk_hfg_handle_write_phonebook_info_ind(bt_hfg_write_phonebook_info_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_write_phonebook_info_ind_struct *msg_p;
    
    //msg_p = (bt_hfg_write_phonebook_info_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_QUERY_WRITE_PBENTRY_INFO;
    param.status = BT_STATUS_SUCCESS;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_QUERY_WRITE_PBENTRY_INFO, &param, sizeof(HfgCallbackParameters));
}

/* AT+CPBW=[<index>][,<number>[,<type>[,<text>]]] : AT_WRITE_PHONEBOOK_ENTRY */
void btmtk_hfg_handle_write_phonebook_ind(bt_hfg_write_phonebook_ind_struct *msg_p)
{
    HfgChannelContext *hfgContext;
    HfgCallbackParameters   param;
    //bt_hfg_write_phonebook_ind_struct *msg_p;
    HfgPbWrite pbWrite;
    
    //msg_p = (bt_hfg_write_phonebook_ind_struct*)message->ilm_data;
    hfgContext = (HfgChannelContext*)msg_p->user_context;
    // param.event = HFG_EVENT_WRITE_PBENTRY;
    param.status = BT_STATUS_SUCCESS;
    pbWrite.index = msg_p->index;
    pbWrite.type = msg_p->type;
    pbWrite.number = (msg_p->number[0] == 0) ? NULL : msg_p->number;
    pbWrite.text = (msg_p->text[0] == 0) ? NULL : msg_p->text;
    param.p.pbWrite = &pbWrite;
    hfgContext->callback((void*)hfgContext, EVENT_HFG_WRITE_PBENTRY, &param, sizeof(HfgCallbackParameters));
}

/**********************************************************
*   SMS related handler
**********************************************************/
void btmtk_hfg_handle_general_ind(bt_hfg_common_ind_struct *msg_p, BT_CALLBACK_EVENT event)
{
	HfgChannelContext *hfgContext;
	HfgCallbackParameters   param;

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	hfgContext->callback((void*)hfgContext, event, &param, sizeof(HfgCallbackParameters));
}

/* MSG_ID_BT_HFG_QUERY_SUPPORTED_SMS_SERVICE_IND */
/* AT+CSMS=? */
void btmtk_hfg_handle_query_supported_sms_service_ind(bt_hfg_query_supported_sms_service_ind_struct *msg_p)
{
	btmtk_hfg_handle_general_ind(msg_p, EVENT_HFG_QUERY_SUPPORTED_SMS_SERVICE);
}

/* MSG_ID_BT_HFG_QUERY_SELECTED_SMS_SERVICE_IND */
/* AT+CSMS? */
void btmtk_hfg_handle_query_selected_sms_service_ind(bt_hfg_query_selected_sms_service_ind_struct *msg_p)
{
	btmtk_hfg_handle_general_ind(msg_p, EVENT_HFG_QUERY_SELECTED_SMS_SERVICE);
}

/* MSG_ID_BT_HFG_SELECT_SMS_SERVICE_IND */
/* AT+CSMS=<service> */
void btmtk_hfg_handle_select_sms_service_ind(bt_hfg_select_sms_service_ind_struct *msg_p)
{
	HfgChannelContext *hfgContext;
	HfgCallbackParameters   param;

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	// Fill content
	param.p.service = msg_p->service;
	hfgContext->callback((void*)hfgContext, EVENT_HFG_SELECT_SMS_SERVICE, &param, sizeof(HfgCallbackParameters));
}

/* AT_PREFERRED_SMS_STORAGE */
/* MSG_ID_BT_HFG_QUERY_SUPPORTED_PREF_MSG_STORAGE_IND */
/* AT+CPMS=? */
void btmtk_hfg_handle_query_supported_pref_msg_storage_ind(bt_hfg_query_supported_pref_msg_storage_ind_struct *msg_p)
{
	btmtk_hfg_handle_general_ind(msg_p, EVENT_HFG_QUERY_SUPPORTED_PREF_MSG_STORAGE);
}

/* MSG_ID_BT_HFG_QUERY_SELECTED_PREF_MSG_STORAGE_IND */
/* AT+CPMS? */
void btmtk_hfg_handle_query_selected_pref_msg_storage_ind(bt_hfg_query_selected_pref_msg_storage_ind_struct *msg_p)
{
	btmtk_hfg_handle_general_ind(msg_p, EVENT_HFG_QUERY_SELECTED_PREF_MSG_STORAGE);
}
/* MSG_ID_BT_HFG_SELECT_PREF_MSG_STORAGE_IND */				
/* AT+CPMS=<mem1>[,<mem2>[,<mem3>]] */
void btmtk_hfg_handle_select_pref_msg_storage_ind(bt_hfg_select_pref_msg_storage_ind_struct *msg_p)
{
	HfgChannelContext 			*hfgContext;
	HfgCallbackParameters   		param;
	HfgSMSPrefStorage_cmd		prefStorage;

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	// Fill content
	prefStorage.read = msg_p->read;
	prefStorage.write = msg_p->write;
	prefStorage.recv = msg_p->recv;
	param.p.prefStorage = &prefStorage;
	hfgContext->callback((void*)hfgContext, EVENT_HFG_SELECT_PREF_MSG_STORAGE, &param, sizeof(HfgCallbackParameters));
}
/* AT_SMS_MESSAGE_FORMAT */
/* MSG_ID_BT_HFG_QUERY_SUPPORTED_MSG_FORMAT_IND */
/* AT+CMGF=? */
void btmtk_hfg_handle_query_supported_msg_format_ind(bt_hfg_query_supported_msg_format_ind_struct *msg_p)
{
	btmtk_hfg_handle_general_ind(msg_p, EVENT_HFG_QUERY_SUPPORTED_MSG_FORMAT);
}

/* MSG_ID_BT_HFG_QUERY_SELECTED_MSG_FORMAT_IND */	
/* AT+CMGF? */
void btmtk_hfg_handle_query_selected_msg_format_ind(bt_hfg_query_selected_msg_format_ind_struct *msg_p)
{
	btmtk_hfg_handle_general_ind(msg_p, EVENT_HFG_QUERY_SELECTED_MSG_FORMAT);
}

/* MSG_ID_BT_HFG_SELECT_MSG_FORMAT_IND */				
/* AT+CMGF=<mode> */
void btmtk_hfg_handle_select_msg_format_ind(bt_hfg_select_msg_format_ind_struct *msg_p)
{
	HfgChannelContext *hfgContext;
	HfgCallbackParameters   param;

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	// Fill content
	param.p.format = msg_p->format;
	hfgContext->callback((void*)hfgContext, EVENT_HFG_SELECT_MSG_FORMAT, &param, sizeof(HfgCallbackParameters));
}

/* AT_SMS_SERVICE_CENTER */
/* MSG_ID_BT_HFG_QUERY_SERVICE_CENTRE_IND */ 		
/* AT+CSCA? */
void btmtk_hfg_handle_query_service_centre_ind(bt_hfg_query_service_centre_ind_struct *msg_p)
{
	btmtk_hfg_handle_general_ind(msg_p, EVENT_HFG_QUERY_SERVICE_CENTRE);
}

/* MSG_ID_BT_HFG_SET_SERVICE_CENTRE_IND */			
/* AT+CSCA=<sca>[,<tosca>] */
void btmtk_hfg_handle_set_service_centre_ind(bt_hfg_set_service_centre_ind_struct *msg_p)
{
	HfgChannelContext			*hfgContext;
	HfgCallbackParameters			param;
	HfgSMSSrviceCentre_cmd		serviceCentre;

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	// Fill content
	serviceCentre.mask = msg_p->mask;
	serviceCentre.tosca = msg_p->addrType;
	serviceCentre.sca = msg_p->addr;
	param.p.serviceCentre = &serviceCentre;
	hfgContext->callback((void*)hfgContext, EVENT_HFG_SET_SERVICE_CENTRE, &param, sizeof(HfgCallbackParameters));
}

/* AT_SET_TEXT_MODE_PARMS */
/* MSG_ID_BT_HFG_QUERY_TEXT_MODE_PARAMS_IND */	
/* AT+CSMP? */
void btmtk_hfg_handle_query_text_mode_params_ind(bt_hfg_query_text_mode_params_ind_struct *msg_p)
{
	btmtk_hfg_handle_general_ind(msg_p, EVENT_HFG_QUERY_TEXT_MODE_PARAMS);
}

/* MSG_ID_BT_HFG_SET_TEXT_MODE_PARAMS_IND */ 	
/* AT+CSMP=[<fo>[,<vp>[,<pid>[,<dcs>]]]] */
void btmtk_hfg_handle_set_text_mode_params_ind(bt_hfg_set_text_mode_params_ind_struct *msg_p)
{
	HfgChannelContext 			*hfgContext;
	HfgCallbackParameters   			param;
	HfgSMSTextModeParam_cmd		textParams;

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	// Fill content
	textParams.mask = msg_p->mask;
	textParams.fo = msg_p->fo;
	textParams.intVp = msg_p->intVp;
	if(msg_p->strVp[0] != 0)
	textParams.strVp = msg_p->strVp;
	else
		textParams.strVp = NULL;
	textParams.pid = msg_p->pid;
	textParams.dcs = msg_p->dcs;
	param.p.textParams = &textParams;
	hfgContext->callback((void*)hfgContext, EVENT_HFG_SET_TEXT_MODE_PARAMS, &param, sizeof(HfgCallbackParameters));
}

/* AT_SMS_SHOW_TEXT_MODE */
/* MSG_ID_BT_HFG_QUERY_SUPPORTED_SHOW_PARAMS_IND */
/* AT+CSDH=? */
void btmtk_hfg_handle_query_supported_show_params_ind(bt_hfg_query_supported_show_params_ind_struct *msg_p)
{
	btmtk_hfg_handle_general_ind(msg_p, EVENT_HFG_QUERY_SUPPORTED_SHOW_PARAMS);
}

/* MSG_ID_BT_HFG_QUERY_SELECTED_SHOW_PARAMS_IND */
/* AT+CSDH? */
void btmtk_hfg_handle_query_selected_show_params_ind(bt_hfg_query_selected_show_params_ind_struct *msg_p)
{
	btmtk_hfg_handle_general_ind(msg_p, EVENT_HFG_QUERY_SELECTED_SHOW_PARAMS);
}

/* MSG_ID_BT_HFG_SET_SHOW_PARAMS_IND */		
/* AT+CSDH=[<show>] */
void btmtk_hfg_handle_set_show_params_ind(bt_hfg_set_show_params_ind_struct *msg_p)
{
	HfgChannelContext *hfgContext;
	HfgCallbackParameters   param;

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	// Fill content
	param.p.show = msg_p->show;
	hfgContext->callback((void*)hfgContext, EVENT_HFG_SET_SHOW_PARAMS, &param, sizeof(HfgCallbackParameters));
}

/* AT_NEW_MESSAGE_INDICATION */
/* MSG_ID_BT_HFG_QUERY_SUPPORTED_NEW_MSG_INDICATION_IND */
/* AT+CNMI=? */
void btmtk_hfg_handle_query_supported_new_msg_indication_ind(bt_hfg_query_supported_new_msg_indication_ind_struct *msg_p)
{
	btmtk_hfg_handle_general_ind(msg_p, EVENT_HFG_QUERY_SUPPORTED_NEW_MSG_INDICATION);
}

/* MSG_ID_BT_HFG_QUERY_SELECTED_NEW_MSG_INDICATION_IND */	
/* AT+CNMI? */
void btmtk_hfg_handle_query_selected_new_msg_indication_ind(bt_hfg_query_selected_new_msg_indication_ind_struct *msg_p)
{
	btmtk_hfg_handle_general_ind(msg_p, EVENT_HFG_QUERY_SELECTED_NEW_MSG_INDICATION);
}

/* MSG_ID_BT_HFG_SET_NEW_MSG_INDICATION_IND */
/* AT+CNMI=[<mode>[,<mt>[,<bm>[,<ds>[,<bfr>]]]]] */
void btmtk_hfg_handle_set_new_msg_indication_ind(bt_hfg_set_new_msg_indication_ind_struct *msg_p)
{
	HfgChannelContext *hfgContext;
	HfgCallbackParameters   param;
	HfgSMSIndSetting_cmd newMsgInd; 

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	// Fill content
	newMsgInd.mode = msg_p->mode;
	newMsgInd.mt = msg_p->mt;
	newMsgInd.bm = msg_p->bm;
	newMsgInd.ds = msg_p->ds;
	newMsgInd.bfr = msg_p->bfr;
	param.p.newMsgInd = &newMsgInd;
	hfgContext->callback((void*)hfgContext, EVENT_HFG_SET_NEW_MSG_INDICATION, &param, sizeof(HfgCallbackParameters));
}

/* AT_LIST_MESSAGES */
/* MSG_ID_BT_HFG_QUERY_SUPPORTED_LIST_STATUS_IND */
/* AT+CMGL=? */
void btmtk_hfg_handle_query_supported_list_status_ind(bt_hfg_query_supported_list_status_ind_struct *msg_p)
{
	btmtk_hfg_handle_general_ind(msg_p, EVENT_HFG_QUERY_SUPPORTED_LIST_STATUS);
}

/* MSG_ID_BT_HFG_LIST_MSG_IND */
/* AT+CMGL[=<stat>] */
void btmtk_hfg_handle_list_msg_ind(bt_hfg_list_msg_ind_struct *msg_p)
{
	HfgChannelContext *hfgContext;
	HfgCallbackParameters   param;

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	// Fill content
	param.p.stat = msg_p->stat;
	hfgContext->callback((void*)hfgContext, EVENT_HFG_LIST_MSG, &param, sizeof(HfgCallbackParameters));
}

/* AT_READ_MESSAGE */
/* MSG_ID_BT_HFG_READ_MSG_IND */
/* AT+CMGR=<index> */
void btmtk_hfg_handle_read_msg_ind(bt_hfg_read_msg_ind_struct *msg_p)
{
	HfgChannelContext *hfgContext;
	HfgCallbackParameters   param;

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	// Fill content
	param.p.index = msg_p->index;
	hfgContext->callback((void*)hfgContext, EVENT_HFG_READ_MSG, &param, sizeof(HfgCallbackParameters));
}

/* AT_SEND_MESSAGE */
/* MSG_ID_BT_HFG_SEND_MSG_IND */
/* TEXT MODE : AT+CMGS=<da>[,<toda>]<CR>text is entered<ctrl-Z/ESC> */
void btmtk_hfg_handle_send_msg_ind(bt_hfg_send_msg_ind_struct *msg_p)
{
	HfgChannelContext *hfgContext;
	HfgCallbackParameters   param;
	HfgSMSSend_cmd sendMsg;

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	// Fill content
	sendMsg.mask = msg_p->mask;
	sendMsg.destAddr = (msg_p->addr[0]==0) ? NULL : msg_p->addr;
	sendMsg.addrType = msg_p->addrType;
	sendMsg.msg = msg_p->msg;
	sendMsg.pduLen = msg_p->pduLen;
	param.p.sendMsg = &sendMsg;
	hfgContext->callback((void*)hfgContext, EVENT_HFG_SEND_MSG, &param, sizeof(HfgCallbackParameters));
}
															/* PDU MODE : AT+CMGS=<length><CR>PDU is given<ctrl-Z/ESC> */
/* AT_SEND_STORED_MESSAGE */
/* MSG_ID_BT_HFG_SEND_STORED_MSG_IND */
/* AT+CMSS=<index>[,<da>[,<toda>]] */
void btmtk_hfg_handle_send_stored_msg_ind(bt_hfg_send_stored_msg_ind_struct *msg_p)
{
	HfgChannelContext 			*hfgContext;
	HfgCallbackParameters   		param;
	HfgSMSSendStored_cmd		storedMsg;

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	// Fill content
	storedMsg.mask = msg_p->mask;
	storedMsg.index = msg_p->index;
	if( storedMsg.mask & HFG_SMS_MASK_ADDR )
	{
		storedMsg.destAddr = (msg_p->addr[0]==0) ? NULL : msg_p->addr;
	}
	storedMsg.addrType = msg_p->addrType;
	param.p.storedMsg = &storedMsg;
	hfgContext->callback((void*)hfgContext, EVENT_HFG_SEND_STORED_MSG, &param, sizeof(HfgCallbackParameters));
}
															
/* AT_STORE_MESSAGE */
/* MSG_ID_BT_HFG_WRITE_MSG_IND */
/* TEXT MODE : AT+CMGW=<oa/da>[,<toda/toda>[,<stat>]]<CR>text is entered<ctrl-Z/ESC> */
void btmtk_hfg_handle_write_msg_ind(bt_hfg_write_msg_ind_struct *msg_p)
{
	HfgChannelContext *hfgContext;
	HfgCallbackParameters   param;
	HfgSMSWrite_cmd writeMsg;

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	// Fill content
	writeMsg.mask = msg_p->mask;
	if( writeMsg.mask & HFG_SMS_MASK_ADDR )
		writeMsg.addr = (msg_p->addr[0]==0) ? NULL : msg_p->addr;
	else
		writeMsg.addr = NULL;
	writeMsg.addrType = msg_p->addrType;
	writeMsg.stat = msg_p->stat;
	writeMsg.msg = msg_p->msg;
	writeMsg.pduLen = msg_p->pduLen;
	param.p.writeMsg = &writeMsg;
	hfgContext->callback((void*)hfgContext, EVENT_HFG_WRITE_MSG, &param, sizeof(HfgCallbackParameters));
}
																/* PDU MODE : AT+CMGW=<length>[,<stat>]<CR>PDU is given<ctrl-Z/ESC> */
/* AT_DELETE_MESSAGE */
/* MSG_ID_BT_HFG_DELETE_MSG_IND */
/* AT+CMGD=<index> */
void btmtk_hfg_handle_delete_msg_ind(bt_hfg_delete_msg_ind_struct *msg_p)
{
	HfgChannelContext *hfgContext;
	HfgCallbackParameters   param;

	hfgContext = (HfgChannelContext*)msg_p->user_context;
	param.status = BT_STATUS_SUCCESS;
	// Fill content
	param.p.index = msg_p->index;
	hfgContext->callback((void*)hfgContext, EVENT_HFG_DELETE_MSG, &param, sizeof(HfgCallbackParameters));
}

/*****************************************************
*   HFP event handling function
*****************************************************/
void btmtk_hfg_handle_message(msg_type msg_id, U8 *parm)
{    
    switch(msg_id)
    {
    case MSG_ID_BT_HFG_ACTIVATE_CNF:
        btmtk_hfg_handle_activate_cnf((bt_hfg_activate_cnf_struct*)parm);
        break;
        
    case MSG_ID_BT_HFG_DEACTIVATE_CNF:
        btmtk_hfg_handle_deactivate_cnf((bt_hfg_deactivate_cnf_struct*)parm);
        break;

    case MSG_ID_BT_HFG_CONNECT_CNF:
        btmtk_hfg_handle_connect_cnf((bt_hfg_general_cnf_struct*)parm);
        break;
        
    case MSG_ID_BT_HFG_ACCEPT_CHANNEL_CNF:
        btmtk_hfg_handle_accept_channel_cnf((bt_hfg_general_cnf_struct*)parm);
        break;
        
    case MSG_ID_BT_HFG_REJECT_CHANNEL_CNF:
        btmtk_hfg_handle_reject_channel_cnf((bt_hfg_general_cnf_struct*)parm);
        break;
        
    case MSG_ID_BT_HFG_DISCONNECT_CNF:
        btmtk_hfg_handle_disconnect_cnf((bt_hfg_general_cnf_struct*)parm);
        break;
        
    case MSG_ID_BT_HFG_SCO_CONNECT_CNF:
        btmtk_hfg_handle_sco_connect_cnf((bt_hfg_general_cnf_struct*)parm);
        break;
        
    case MSG_ID_BT_HFG_SCO_DISCONNECT_CNF:
        btmtk_hfg_handle_sco_disconnect_cnf((bt_hfg_general_cnf_struct*)parm);
        break;
        
    case MSG_ID_BT_HFG_GET_STATUS_CNF:
        break;

    /* Return response complete to UI */
    case MSG_ID_BT_HFG_OK_CNF:
    case MSG_ID_BT_HFG_ERROR_CNF:
    case MSG_ID_BT_HFG_NO_CARRIER_CNF:
    case MSG_ID_BT_HFG_BUSY_CNF:
    case MSG_ID_BT_HFG_NO_ANSWER_CNF:
    case MSG_ID_BT_HFG_DELAYED_CNF:
    case MSG_ID_BT_HFG_BLACKLISTED_CNF:
    case MSG_ID_BT_HFG_RING_CNF:
    case MSG_ID_BT_HFG_CALL_WAITING_CNF:
    case MSG_ID_BT_HFG_READ_INDICATOR_CNF:
    case MSG_ID_BT_HFG_CALL_LIST_CNF:
    case MSG_ID_BT_HFG_CALL_ID_CNF:
    case MSG_ID_BT_HFG_INDICATOR_REPORT_CNF:
    case MSG_ID_BT_HFG_SUBSCRIBER_NUMBER_CNF:
    case MSG_ID_BT_HFG_VOICE_TAG_CNF:
    case MSG_ID_BT_HFG_VOICE_REC_CNF:
    case MSG_ID_BT_HFG_RESPONSE_HOLD_CNF:
    case MSG_ID_BT_HFG_SPEAKER_GAIN_CNF:
    case MSG_ID_BT_HFG_MIC_GAIN_CNF:
    case MSG_ID_BT_HFG_RINGTONE_STATUS_CNF:
    case MSG_ID_BT_HFG_SUPPORTED_CHARSET_CNF:
    case MSG_ID_BT_HFG_SELECTED_CHARSET_CNF:
    case MSG_ID_BT_HFG_MODEL_ID_CNF:
    case MSG_ID_BT_HFG_MANUFACTURE_ID_CNF:
    case MSG_ID_BT_HFG_SEND_DATA_CNF:
    case MSG_ID_BT_HFG_SUPPORTED_SMS_SERVICE_CNF:
    case MSG_ID_BT_HFG_SELECTED_SMS_SERVICE_CNF:
    /* AT_SELECT_SMS_SERVICE */
    case MSG_ID_BT_HFG_SMS_SERVICE_CNF:
    /* AT_PREFERRED_SMS_STORAGE|AT_TEST */
    case MSG_ID_BT_HFG_SUPPORTED_PREF_MSG_STORAGE_CNF:
    /* AT_PREFERRED_SMS_STORAGE|AT_READ */
    case MSG_ID_BT_HFG_SELECTED_PREF_MSG_STORAGE_CNF:
    /* AT_PREFERRED_SMS_STORAGE */
    case MSG_ID_BT_HFG_PREF_MSG_STORAGE_CNF:
    /* AT_SMS_MESSAGE_FORMAT|AT_TEST */
    case MSG_ID_BT_HFG_SUPPORTED_MSG_FORMAT_CNF:
    /* AT_SMS_MESSAGE_FORMAT|AT_READ */
    case MSG_ID_BT_HFG_SELECTED_MSG_FORMAT_CNF:
    /* AT_SMS_SERVICE_CENTER|AT_READ */
    case MSG_ID_BT_HFG_SERVICE_CENTRE_CNF:
    /* AT_SET_TEXT_MODE_PARMS|AT_READ */
    case MSG_ID_BT_HFG_TEXT_MODE_PARAMS_CNF:
    /* AT_SMS_SHOW_TEXT_MODE|AT_TEST */
    case MSG_ID_BT_HFG_SUPPORTED_SHOW_PARAMS_CNF:
    /* AT_SMS_SHOW_TEXT_MODE|AT_READ */
    case MSG_ID_BT_HFG_SELECTED_SHOW_PARAMS_CNF:
    /* AT_NEW_MESSAGE_INDICATION|AT_TEST */
    case MSG_ID_BT_HFG_SUPPORTED_NEW_MSG_INDICATION_CNF:
    /* AT_NEW_MESSAGE_INDICATION|AT_READ */
    case MSG_ID_BT_HFG_SELECTED_NEW_MSG_INDICATION_CNF:
    /* AT_SMS_DELIVER_INDICATION */
    case MSG_ID_BT_HFG_NEW_MSG_INDICATION_CNF:
    /* AT_LIST_MESSAGES|AT_TEST */
    case MSG_ID_BT_HFG_SUPPORTED_LIST_STATUS_CNF:
    /* AT_LIST_MESSAGES */
    case MSG_ID_BT_HFG_LIST_MSG_CNF:
    /* AT_READ_MESSAGE */
    case MSG_ID_BT_HFG_READ_MSG_CNF:
    /* AT_SEND_MESSAGE */
    case MSG_ID_BT_HFG_SEND_MSG_CNF:
    /* AT_SEND_STORED_MESSAGE */
    case MSG_ID_BT_HFG_SEND_STORED_MSG_CNF:
    /* AT_STORE_MESSAGE */
    case MSG_ID_BT_HFG_WRITE_MSG_CNF:
    case MSG_ID_BT_HFG_SMS_ERROR_CNF:
        btmtk_hfg_handle_response_complete_ind(msg_id, (bt_hfg_general_cnf_struct*)parm);
        break;
        
    /* Indication */
    case MSG_ID_BT_HFG_CONNECT_REQ_IND:
        btmtk_hfg_handle_connect_req_ind((bt_hfg_connect_req_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_RFCOMM_CONNECTED_IND:
        btmtk_hfg_handle_rfcomm_connected_ind((bt_hfg_rfcomm_connected_ind_struct*)parm);                
        break;

    case MSG_ID_BT_HFG_CONNECTED_IND:
        btmtk_hfg_handle_connected_ind((bt_hfg_connected_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_DISCONNECTED_IND:
        btmtk_hfg_handle_disconnected_ind((bt_hfg_disconnected_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_SCO_CONNECTED_IND:
        btmtk_hfg_handle_sco_connected_ind((bt_hfg_sco_connected_ind_struct*)parm);
        break;
        
    case MSG_ID_BT_HFG_SCO_DISCONNECTED_IND:
        btmtk_hfg_handle_sco_disconnected_ind((bt_hfg_sco_connected_ind_struct*)parm);
        break;

    /* AT CMD indication */
    case MSG_ID_BT_HFG_HANDSFREE_FEATURES_IND:
        btmtk_hfg_handle_handsfree_features_ind((bt_hfg_handsfree_features_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_QUERY_INDICATOR_VALUE_IND:
        btmtk_hfg_handle_query_indicator_value_ind((bt_hfg_query_indicator_value_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_ENABLE_EXTENDED_ERRORS_IND:
        btmtk_hfg_handle_enable_extended_errors_ind((bt_hfg_enable_extended_errors_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_ANSWER_CALL_IND:
        btmtk_hfg_handle_answer_call_ind((bt_hfg_answer_call_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_DIAL_NUMBER_IND:
        btmtk_hfg_handle_dial_number_ind((bt_hfg_dial_number_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_MEMORY_DIAL_IND:
        btmtk_hfg_handle_memory_dial_ind((bt_hfg_memory_dial_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_REDIAL_IND:
        btmtk_hfg_handle_redial_ind((bt_hfg_redial_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_CALL_HOLD_IND:
        btmtk_hfg_handle_call_hold_ind((bt_hfg_call_hold_ind_struct*)parm);
        break;

	case MSG_ID_BT_HFG_QUERY_RESPONSE_HOLD_IND:
		btmtk_hfg_handle_query_response_hold_ind((bt_hfg_query_response_hold_ind_struct*)parm);
		break;
		
	case MSG_ID_BT_HFG_SET_RESPONSE_HOLD_IND:
		btmtk_hfg_handle_set_response_hold_ind((bt_hfg_set_response_hold_ind_struct*)parm);
		break;

    case MSG_ID_BT_HFG_HANGUP_IND:
        btmtk_hfg_handle_hangup_ind((bt_hfg_hang_up_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_GENERATE_DTMF_IND:
        btmtk_hfg_handle_generate_dtmf_ind((bt_hfg_generate_dtmf_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_GET_LAST_VOICE_TAG_IND:
        btmtk_hfg_handle_get_last_voice_tag_ind((bt_hfg_get_last_voice_tag_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_LIST_CURRENT_CALLS_IND:
        btmtk_hfg_handle_list_current_calls_ind((bt_hfg_list_current_calls_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_QUERY_SUBSCRIBER_NUMBER_IND:
        btmtk_hfg_handle_query_subscriber_number_ind((bt_hfg_query_subscriber_number_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_QUERY_NETWORK_OPERATOR_IND:
        btmtk_hfg_handle_query_network_operator_ind((bt_hfg_query_network_operator_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_REPORT_MIC_VOLUME_IND:
        btmtk_hfg_handle_report_mic_volume_ind((bt_hfg_report_mic_volume_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_REPORT_SPK_VOLUME_IND:
        btmtk_hfg_handle_report_spk_volume_ind((bt_hfg_report_speaker_volume_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_ENABLE_CALLER_ID_IND:
        btmtk_hfg_handle_enable_caller_id_ind((bt_hfg_enable_caller_id_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_ENABLE_CALL_WAITING_IND:
        btmtk_hfg_handle_enable_call_waiting_ind((bt_hfg_enable_call_waiting_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_DISABLE_NREC_IND:
        btmtk_hfg_handle_disable_nrec_ind((bt_hfg_disable_nrec_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_ENABLE_VOICE_RECOGNITION_IND:
        btmtk_hfg_handle_enable_voice_recognition_ind((bt_hfg_enable_voice_recognition_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_KEYPAD_CONTROL_IND:
        btmtk_hfg_handle_keypad_control_ind((bt_hfg_keypad_control_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_QUERY_SUPPORTED_CHARSET_IND:
        btmtk_hfg_handle_query_supported_charset_ind((bt_hfg_query_supported_charset_ind_struct*)parm);
        break;
		
    case MSG_ID_BT_HFG_QUERY_SELECTED_CHARSET_IND:
        btmtk_hfg_handle_query_selected_charset_ind((bt_hfg_query_selected_charset_ind_struct*)parm);
        break;
		
    case MSG_ID_BT_HFG_SELECT_CHARSET_IND:
	 btmtk_hfg_handle_select_charset_ind((bt_hfg_select_charset_ind_struct*)parm);
	 break;

    case MSG_ID_BT_HFG_QUERY_MODEL_ID_IND:
        btmtk_hfg_handle_query_model_id_ind((bt_hfg_query_model_id_ind_struct*)parm);
        break;

    case MSG_ID_BT_HFG_QUERY_MANUFACTURE_ID_IND:
        btmtk_hfg_handle_query_manufacture_id_ind((bt_hfg_query_manufacture_id_ind_struct*)parm);
	 break;

    case MSG_ID_BT_HFG_AT_COMMAND_DATA_IND:
	 btmtk_hfg_handle_at_command_ind((bt_hfg_at_command_data_ind_struct*)parm);
	 break;
    
    case MSG_ID_BT_HFG_QUERY_SUPPORTED_PHONEBOOK_IND:
        btmtk_hfg_handle_query_supported_phonebook_ind((bt_hfg_query_supported_phonebook_ind_struct*)parm);
        break;
        
    
    case MSG_ID_BT_HFG_SELECT_PHONEBOOK_IND:
        btmtk_hfg_handle_select_phonebook_ind((bt_hfg_select_phonebook_ind_struct*)parm);
        break;
        
    
    case MSG_ID_BT_HFG_QUERY_SELECTED_PHONEBOOK_IND:
        btmtk_hfg_handle_query_selected_phonebook_ind((bt_hfg_query_selected_phonebook_ind_struct*)parm);
        break;
        
    
    case MSG_ID_BT_HFG_READ_PHONEBOOK_INFO_IND:
        btmtk_hfg_handle_read_phonebook_info_ind((bt_hfg_read_phonebook_info_ind_struct*)parm);
        break;
        
    
    case MSG_ID_BT_HFG_READ_PHONEBOOK_IND:
        btmtk_hfg_handle_read_phonebook_ind((bt_hfg_read_phonebook_ind_struct*)parm);
        break;
        
    
    case MSG_ID_BT_HFG_FIND_PHONEBOOK_INFO_IND:
        btmtk_hfg_handle_find_phonebook_info_ind((bt_hfg_find_phonebook_info_ind_struct*)parm);
        break;
        
    
    case MSG_ID_BT_HFG_FIND_PHONEBOOK_IND:
        btmtk_hfg_handle_find_phonebook_ind((bt_hfg_find_phonebook_ind_struct*)parm);
        break;
        
    
    case MSG_ID_BT_HFG_WRITE_PHONEBOOK_INFO_IND:
        btmtk_hfg_handle_write_phonebook_info_ind((bt_hfg_write_phonebook_info_ind_struct*)parm);
        break;
        
    case MSG_ID_BT_HFG_WRITE_PHONEBOOK_IND:
        btmtk_hfg_handle_write_phonebook_ind((bt_hfg_write_phonebook_ind_struct*)parm);
        break;
        
	/********************************************************/
	/*	SMS related messages handling							*/
	/********************************************************/
	/* AT_SELECT_SMS_SERVICE */
	case MSG_ID_BT_HFG_QUERY_SUPPORTED_SMS_SERVICE_IND:		/* AT+CSMS=? */
		btmtk_hfg_handle_query_supported_sms_service_ind((bt_hfg_query_supported_sms_service_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_QUERY_SELECTED_SMS_SERVICE_IND:			/* AT+CSMS? */
		btmtk_hfg_handle_query_selected_sms_service_ind((bt_hfg_query_selected_sms_service_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_SELECT_SMS_SERVICE_IND:					/* AT+CSMS=<service> */
		btmtk_hfg_handle_select_sms_service_ind((bt_hfg_select_sms_service_ind_struct*)parm);
		break;
	/* AT_PREFERRED_SMS_STORAGE */
	case MSG_ID_BT_HFG_QUERY_SUPPORTED_PREF_MSG_STORAGE_IND: /* AT+CPMS=? */
		btmtk_hfg_handle_query_supported_pref_msg_storage_ind((bt_hfg_query_supported_pref_msg_storage_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_QUERY_SELECTED_PREF_MSG_STORAGE_IND:	/* AT+CPMS? */
		btmtk_hfg_handle_query_selected_pref_msg_storage_ind((bt_hfg_query_selected_pref_msg_storage_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_SELECT_PREF_MSG_STORAGE_IND:				/* AT+CPMS=<mem1>[,<mem2>[,<mem3>]] */
		btmtk_hfg_handle_select_pref_msg_storage_ind((bt_hfg_select_pref_msg_storage_ind_struct*)parm);
		break;
	/* AT_SMS_MESSAGE_FORMAT */
	case MSG_ID_BT_HFG_QUERY_SUPPORTED_MSG_FORMAT_IND:		/* AT+CMGF=? */
		btmtk_hfg_handle_query_supported_msg_format_ind((bt_hfg_query_supported_msg_format_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_QUERY_SELECTED_MSG_FORMAT_IND:			/* AT+CMGF? */
		btmtk_hfg_handle_query_selected_msg_format_ind((bt_hfg_query_selected_msg_format_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_SELECT_MSG_FORMAT_IND:					/* AT+CMGF=<mode> */
		btmtk_hfg_handle_select_msg_format_ind((bt_hfg_select_msg_format_ind_struct*)parm);
		break;
	/* AT_SMS_SERVICE_CENTER */
	case MSG_ID_BT_HFG_QUERY_SERVICE_CENTRE_IND: 				/* AT+CSCA? */
		btmtk_hfg_handle_query_service_centre_ind((bt_hfg_query_service_centre_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_SET_SERVICE_CENTRE_IND:					/* AT+CSCA=<sca>[,<tosca>] */
		btmtk_hfg_handle_set_service_centre_ind((bt_hfg_set_service_centre_ind_struct*)parm);
		break;
	/* AT_SET_TEXT_MODE_PARMS */
	case MSG_ID_BT_HFG_QUERY_TEXT_MODE_PARAMS_IND:				/* AT+CSMP? */
		btmtk_hfg_handle_query_text_mode_params_ind((bt_hfg_query_text_mode_params_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_SET_TEXT_MODE_PARAMS_IND: 			/* AT+CSMP=[<fo>[,<vp>[,<pid>[,<dcs>]]]] */
		btmtk_hfg_handle_set_text_mode_params_ind((bt_hfg_set_text_mode_params_ind_struct*)parm);
		break;
	/* AT_SMS_SHOW_TEXT_MODE */
	case MSG_ID_BT_HFG_QUERY_SUPPORTED_SHOW_PARAMS_IND:		/* AT+CSDH=? */
		btmtk_hfg_handle_query_supported_show_params_ind((bt_hfg_query_supported_show_params_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_QUERY_SELECTED_SHOW_PARAMS_IND:			/* AT+CSDH? */
		btmtk_hfg_handle_query_selected_show_params_ind((bt_hfg_query_selected_show_params_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_SET_SHOW_PARAMS_IND:
		btmtk_hfg_handle_set_show_params_ind((bt_hfg_set_show_params_ind_struct*)parm);
		break;
	/* AT_NEW_MESSAGE_INDICATION */
	case MSG_ID_BT_HFG_QUERY_SUPPORTED_NEW_MSG_INDICATION_IND:	/* AT+CNMI=? */
		btmtk_hfg_handle_query_supported_new_msg_indication_ind((bt_hfg_query_supported_new_msg_indication_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_QUERY_SELECTED_NEW_MSG_INDICATION_IND:		/* AT+CNMI? */
		btmtk_hfg_handle_query_selected_new_msg_indication_ind((bt_hfg_query_selected_new_msg_indication_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_SET_NEW_MSG_INDICATION_IND:					/* AT+CNMI=[<mode>[,<mt>[,<bm>[,<ds>[,<bfr>]]]]] */
		btmtk_hfg_handle_set_new_msg_indication_ind((bt_hfg_set_new_msg_indication_ind_struct*)parm);
		break;
	/* AT_LIST_MESSAGES */
	case MSG_ID_BT_HFG_QUERY_SUPPORTED_LIST_STATUS_IND:			/* AT+CMGL=? */
		btmtk_hfg_handle_query_supported_list_status_ind((bt_hfg_query_supported_list_status_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_LIST_MSG_IND: 							/* AT+CMGL[=<stat>] */
		btmtk_hfg_handle_list_msg_ind((bt_hfg_list_msg_ind_struct*)parm);
		break;
	/* AT_READ_MESSAGE */
	case MSG_ID_BT_HFG_READ_MSG_IND: 						/* AT+CMGR=<index> */
		btmtk_hfg_handle_read_msg_ind((bt_hfg_read_msg_ind_struct*)parm);
		break;
	/* AT_SEND_MESSAGE */
	case MSG_ID_BT_HFG_SEND_MSG_IND: 						/* TEXT MODE : AT+CMGS=<da>[,<toda>]<CR>text is entered<ctrl-Z/ESC> */
		btmtk_hfg_handle_send_msg_ind((bt_hfg_send_msg_ind_struct*)parm);
		break;															
	/* AT_SEND_STORED_MESSAGE */
	case MSG_ID_BT_HFG_SEND_STORED_MSG_IND:				/* AT+CMSS=<index>[,<da>[,<toda>]] */
		btmtk_hfg_handle_send_stored_msg_ind((bt_hfg_send_stored_msg_ind_struct*)parm);
		break;		
	/* AT_STORE_MESSAGE */
	case MSG_ID_BT_HFG_WRITE_MSG_IND:						/* TEXT MODE : AT+CMGW=<oa/da>[,<toda/toda>[,<stat>]]<CR>text is entered<ctrl-Z/ESC> */
		btmtk_hfg_handle_write_msg_ind((bt_hfg_write_msg_ind_struct*)parm);
		break;																	
	/* AT_DELETE_MESSAGE */
	case MSG_ID_BT_HFG_DELETE_MSG_IND:						/* AT+CMGD=<index> */
		btmtk_hfg_handle_delete_msg_ind((bt_hfg_delete_msg_ind_struct*)parm);
		break;
	case MSG_ID_BT_HFG_GROUP_START:
	{
		HfgChannelContext *hfgContext;
		bt_hfg_activate_req_struct *msg_p =  (bt_hfg_activate_req_struct*)parm;
		hfgContext = (HfgChannelContext*)msg_p->req_context;
		hfgContext->callback((void*)hfgContext, EVENT_HFG_DELAY_CALL_STATE_CHANGE, NULL, 0);
		break;
	}

	#ifdef BT_HFG_UT_TEST
	case MSG_ID_BT_HFG_UT_INIT_CNF:
	{
		HfgChannelContext *hfgContext;
		HfgCallbackParameters   param;
		bt_hfg_ut_init_cnf_struct *msg_p = (bt_hfg_ut_init_cnf_struct*)parm;
		hfgContext = (HfgChannelContext*)msg_p->req_context;
		param.status = BT_STATUS_SUCCESS;		
		hfgContext->callback((void*)hfgContext, EVENT_HFG_UT_INIT_CNF, &param, sizeof(HfgCallbackParameters));
		break;
	}		
	case MSG_ID_BT_HFG_UT_TX_CNF:
		printf("[HFG extADP][UT] Receive UT tx confirm\r\n");
		break;
	/* Simulate RF data send by HFG */
	case MSG_ID_BT_HFG_UT_RX_IND:
	{
		HfgChannelContext *hfgContext;
		HfgCallbackParameters   param;
		bt_hfg_ut_rx_rsp_struct rsp_msg;
		bt_hfg_ut_rx_ind_struct *ind_msg_p = (bt_hfg_ut_rx_ind_struct*)parm;
		ind_msg_p->buf[ind_msg_p->len] = 0;
		printf("[HFG extADP][UT] Receive packet: %s", ind_msg_p->buf);
		rsp_msg.pContext = ind_msg_p->pContext;
		hfgContext = rsp_msg.req_context = ind_msg_p->user_context;
		BTCMD_SendMessage(MSG_ID_BT_HFG_UT_RX_RSP, MOD_BT, &rsp_msg, sizeof(rsp_msg));
		param.p.ptr = ind_msg_p->buf;
		hfgContext->callback((void*)hfgContext, EVENT_HFG_UT_RX_IND, &param, sizeof(HfgCallbackParameters));
		break;
	}
	#endif /* BT_HFG_UT_TEST */


    default:
	//Assert(0);
        return;
    }
}

