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
 * Bt_chn_hdl.c
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
#include "bt_types.h"
#include "bt_mmi.h"
#include "bluetooth_chn_struct.h"
#include "bt_message.h"
#include "bt_chn_struct.h"

#define BT_STATUS_SUCCESS 0

void btmtk_chn_handle_activate_cnf(bt_chn_activate_cnf_struct *cnf_msg)
{
    ChnChannelContext *chnContext;
    ChnCallbackParameters   param;

    chnContext = (ChnChannelContext*)cnf_msg->req_context;
    param.status = cnf_msg->result;
    chnContext->chnContext = cnf_msg->pContext;
    param.p.context = (void*)chnContext;
    chnContext->callback((void*)chnContext, EVENT_CHN_ACTIVATE_CONFIRM, &param, sizeof(ChnCallbackParameters));
}

void btmtk_chn_handle_deactivate_cnf(bt_chn_deactivate_cnf_struct *cnf_msg)
{
    ChnChannelContext *chnContext;
    ChnCallbackParameters   param;
    
    chnContext = (ChnChannelContext*)cnf_msg->req_context;
    param.status = cnf_msg->result;
    param.p.context = (void*)chnContext;
    chnContext->callback((void*)chnContext, EVENT_CHN_DEACTIVATE_CONFIRM, &param, sizeof(ChnCallbackParameters));
}

void btmtk_chn_handle_general_cnf(bt_chn_general_cnf_struct *cnf_msg, BT_CALLBACK_EVENT event)
{
    ChnChannelContext *chnContext;
    ChnCallbackParameters   param;

    chnContext = (ChnChannelContext*)cnf_msg->req_context;
    param.status = cnf_msg->result;
    param.p.context = (void*)chnContext;
    chnContext->callback((void*)chnContext, event, &param, sizeof(ChnCallbackParameters));
}

void btmtk_chn_handle_connect_cnf(bt_chn_general_cnf_struct *cnf_msg)
{
    btmtk_chn_handle_general_cnf(cnf_msg, EVENT_CHN_CONNECT_CONFIRM);
}

void btmtk_chn_handle_accept_channel_cnf(bt_chn_general_cnf_struct *cnf_msg)
{
    btmtk_chn_handle_general_cnf(cnf_msg, EVENT_CHN_ACCEPT_CHANNEL_CONFIRM);
}

void btmtk_chn_handle_reject_channel_cnf(bt_chn_general_cnf_struct *cnf_msg)
{
    btmtk_chn_handle_general_cnf(cnf_msg, EVENT_CHN_REJECT_CHANNEL_CONFIRM);
}

void btmtk_chn_handle_disconnect_cnf(bt_chn_general_cnf_struct *cnf_msg)
{
    btmtk_chn_handle_general_cnf(cnf_msg, EVENT_CHN_DISCONNECT_CONFIRM);
}

void btmtk_chn_handle_sco_connect_cnf(bt_chn_general_cnf_struct *cnf_msg)
{
    btmtk_chn_handle_general_cnf(cnf_msg, EVENT_CHN_SCO_CONNECT_CONFIRM);
}

void btmtk_chn_handle_sco_disconnect_cnf(bt_chn_general_cnf_struct *cnf_msg)
{
    btmtk_chn_handle_general_cnf(cnf_msg, EVENT_CHN_SCO_DISCONNECT_CONFIRM);
}

void btmtk_chn_handle_connect_req_ind(bt_chn_connect_req_ind_struct *msg_p)
{
    ChnChannelContext *chnContext;
    ChnCallbackParameters   param;
    
    chnContext = (ChnChannelContext*)msg_p->user_context;
    param.status = BT_STATUS_SUCCESS;
    param.p.addr  = &msg_p->bt_addr;
    chnContext->callback((void*)chnContext, EVENT_CHN_AUTH_REQ, &param, sizeof(ChnCallbackParameters));
}

void btmtk_chn_handle_connected_ind(bt_chn_connected_ind_struct *msg_p)
{
    ChnChannelContext *chnContext;
    ChnCallbackParameters   param;    
    
    chnContext = (ChnChannelContext*)msg_p->user_context;
    param.status = BT_STATUS_SUCCESS;
    param.p.addr  = &msg_p->bt_addr;
    chnContext->callback((void*)chnContext, EVENT_CHN_SERVICE_CONNECTED, &param, sizeof(ChnCallbackParameters));
}

void btmtk_chn_handle_disconnected_ind(bt_chn_disconnected_ind_struct *msg_p)
{
    ChnChannelContext *chnContext;
    ChnCallbackParameters   param;    
    
    chnContext = (ChnChannelContext*)msg_p->user_context;
    param.status = BT_STATUS_SUCCESS;
    param.p.addr  = &msg_p->bt_addr;
    chnContext->callback((void*)chnContext, EVENT_CHN_SERVICE_DISCONNECTED, &param, sizeof(ChnCallbackParameters));
}

void btmtk_chn_handle_sco_connected_ind(bt_chn_sco_connected_ind_struct *msg_p)
{
    ChnChannelContext *chnContext;
    ChnCallbackParameters   param;    

    chnContext = (ChnChannelContext*)msg_p->user_context;
    param.status = msg_p->status;
    chnContext->callback((void*)chnContext, EVENT_CHN_AUDIO_CONNECTED, &param, sizeof(ChnCallbackParameters));
}

void btmtk_chn_handle_sco_disconnected_ind(bt_chn_sco_connected_ind_struct *msg_p)
{
    ChnChannelContext *chnContext;
    ChnCallbackParameters   param;
    
    chnContext = (ChnChannelContext*)msg_p->user_context;
    param.status = msg_p->status;
    chnContext->callback((void*)chnContext, EVENT_CHN_AUDIO_DISCONNECTED, &param, sizeof(ChnCallbackParameters));
}

void btmtk_chn_handle_rx_data_ind(bt_chn_rx_data_ind_struct *msg_p)
{
    ChnChannelContext *chnContext;
    ChnCallbackParameters   param;
    RfBuffer				buf = {msg_p->size, msg_p->data};
    
    chnContext = (ChnChannelContext*)msg_p->user_context;
    param.status = BT_STATUS_SUCCESS;
    param.p.rxBuf = &buf;
	msg_p->data[msg_p->size] = 0;
	
    chnContext->callback((void*)chnContext, EVENT_CHN_RX_DATA, &param, sizeof(ChnCallbackParameters));	
}

/*****************************************************
*   HFP event handling function
*****************************************************/
void btmtk_chn_handle_message(msg_type msg_id, U8 *parm)
{    
    switch(msg_id)
    {
    case MSG_ID_BT_CHN_ACTIVATE_CNF:
        btmtk_chn_handle_activate_cnf((bt_chn_activate_cnf_struct*)parm);
        break;
        
    case MSG_ID_BT_CHN_DEACTIVATE_CNF:
        btmtk_chn_handle_deactivate_cnf((bt_chn_deactivate_cnf_struct*)parm);
        break;

    case MSG_ID_BT_CHN_CONNECT_CNF:
        btmtk_chn_handle_connect_cnf((bt_chn_general_cnf_struct*)parm);
        break;
        
    case MSG_ID_BT_CHN_ACCEPT_CHANNEL_CNF:
        btmtk_chn_handle_accept_channel_cnf((bt_chn_general_cnf_struct*)parm);
        break;
        
    case MSG_ID_BT_CHN_REJECT_CHANNEL_CNF:
        btmtk_chn_handle_reject_channel_cnf((bt_chn_general_cnf_struct*)parm);
        break;
        
    case MSG_ID_BT_CHN_DISCONNECT_CNF:
        btmtk_chn_handle_disconnect_cnf((bt_chn_general_cnf_struct*)parm);
        break;
        
    case MSG_ID_BT_CHN_SCO_CONNECT_CNF:
        btmtk_chn_handle_sco_connect_cnf((bt_chn_general_cnf_struct*)parm);
        break;
        
    case MSG_ID_BT_CHN_SCO_DISCONNECT_CNF:
        btmtk_chn_handle_sco_disconnect_cnf((bt_chn_general_cnf_struct*)parm);
        break;
                
    /* Indication */
    case MSG_ID_BT_CHN_CONNECT_REQ_IND:
        btmtk_chn_handle_connect_req_ind((bt_chn_connect_req_ind_struct*)parm);
        break;

    case MSG_ID_BT_CHN_CONNECTED_IND:
        btmtk_chn_handle_connected_ind((bt_chn_connected_ind_struct*)parm);
        break;

    case MSG_ID_BT_CHN_DISCONNECTED_IND:
        btmtk_chn_handle_disconnected_ind((bt_chn_disconnected_ind_struct*)parm);
        break;

    case MSG_ID_BT_CHN_SCO_CONNECTED_IND:
        btmtk_chn_handle_sco_connected_ind((bt_chn_sco_connected_ind_struct*)parm);
        break;
        
    case MSG_ID_BT_CHN_SCO_DISCONNECTED_IND:
        btmtk_chn_handle_sco_disconnected_ind((bt_chn_sco_connected_ind_struct*)parm);
        break;

    case MSG_ID_BT_CHN_RX_DATA_IND:
	 btmtk_chn_handle_rx_data_ind((bt_chn_rx_data_ind_struct*)parm);
	 break;

    default:
	//Assert(0);
        return;
    }
}

