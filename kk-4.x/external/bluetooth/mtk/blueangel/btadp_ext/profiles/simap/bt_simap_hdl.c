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
 * bt_simap_hdl.c
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
 * Bingyi Chen
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

/* 
 * bt_simap_hdl.c
 * 
 * This file is the External Adaptation handler of SIMAP profile used by Application.
 * Flow direction: APP <-- external ADP handler
 */

#include "stdio.h"
#include "bt_common.h"
#include "bluetooth_struct.h"
#include "bt_simap_api.h"
#include "bluetooth_simap_message.h"
#include "bluetooth_simap_struct.h"
#include "bt_simap_mmi_event.h"
#include "bt_simap_hdl.h"

#include "bt_ext_debug.h"

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/


/****************************************************************************
 *
 * Local function prototypes
 *
 ****************************************************************************/


/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/
#undef  LOG_TAG	
#define LOG_TAG "BTSIMAP_EXT_HDL"

extern BTMTK_EventCallBack g_SIMAP_MMI_Callback;



void btmtk_simap_handle_activate_cnf(ilm_struct *message)
{
	bt_simap_activate_cnf_struct *msg;
	
#ifdef BTMTK_ON_WISE 
    msg = (bt_simap_activate_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (bt_simap_activate_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */

    bt_ext_log("SIMAP ACTIVATE_CNF: result=%d\n", msg->result);
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_ACTIVATE_CNF, msg, sizeof(bt_simap_activate_cnf_struct));
}


void btmtk_simap_handle_deactivate_cnf(ilm_struct *message)
{
	bt_simap_deactivate_cnf_struct *msg;
	
#ifdef BTMTK_ON_WISE 
    msg = (bt_simap_deactivate_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (bt_simap_deactivate_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */

    bt_ext_log("SIMAP DEACTIVATE_CNF: result=%d\n", msg->result);
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_DEACTIVATE_CNF, msg, sizeof(bt_simap_deactivate_cnf_struct));
}


void btmtk_simap_handle_connect_ind(ilm_struct *message)
{
	bt_simap_connect_ind_struct *msg;
	
#ifdef BTMTK_ON_WISE 
    msg = (bt_simap_connect_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (bt_simap_connect_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */

    bt_ext_log("SIMAP CONNECT_IND: cid=%d; lap=%08x, uap=%02x, nap=%04x\n", msg->cid, (int)msg->lap, msg->uap, msg->nap);
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_CONNECT_IND, msg, sizeof(bt_simap_connect_ind_struct));
}


void btmtk_simap_handle_disconnect_ind(ilm_struct *message)
{
	bt_simap_disconnect_ind_struct *msg;
	
#ifdef BTMTK_ON_WISE 
    msg = (bt_simap_disconnect_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (bt_simap_disconnect_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */

    bt_ext_log("SIMAP DISCONNECT_IND: cid=%d\n", msg->cid);
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_DISCONNECT_IND, msg, sizeof(bt_simap_disconnect_ind_struct));
}


void btmtk_simap_handle_disconnect_cnf(ilm_struct *message)
{
	bt_simap_disconnect_cnf_struct *msg;
	
#ifdef BTMTK_ON_WISE 
    msg = (bt_simap_disconnect_cnf_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (bt_simap_disconnect_cnf_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */

    bt_ext_log("SIMAP DISCONNECT_CNF: result=%d, cid=%d\n", msg->result, msg->cid);
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_DISCONNECT_CNF, msg, sizeof(bt_simap_disconnect_cnf_struct));
}


void btmtk_simap_handle_auth_req(ilm_struct *message)
{
	bt_simap_auth_req_struct *msg;
	
#ifdef BTMTK_ON_WISE 
    msg = (bt_simap_auth_req_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (bt_simap_auth_req_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */

    bt_ext_log("SIMAP AUTH_REQ: lap=%08x, uap=%02x, nap=%04x\n", (int)msg->lap, msg->uap, msg->nap);
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_AUTH_REQ, msg, sizeof(bt_simap_auth_req_struct));
}


void btmtk_simap_handle_atr_rsp_sent_ind(ilm_struct *message)
{
    bt_ext_log("SIM ATR_RSP_SENT\n");
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_ATR_RSP_SENT_IND, NULL, 0);
}


void btmtk_simap_handle_apdu_rsp_sent_ind(ilm_struct *message)
{
    bt_ext_log("SIM APDU_RSP_SENT\n");
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_APDU_RSP_SENT_IND, NULL, 0);
}


void btmtk_simap_handle_sim_on_rsp_sent_ind(ilm_struct *message)
{
    bt_ext_log("SIM ON_RSP_SENT\n");
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_SIM_ON_RSP_SENT_IND, NULL, 0);
}


void btmtk_simap_handle_sim_off_rsp_sent_ind(ilm_struct *message)
{
    bt_ext_log("SIM OFF_RSP_SENT\n");
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_SIM_OFF_RSP_SENT_IND, NULL, 0);
}


void btmtk_simap_handle_sim_reset_rsp_sent_ind(ilm_struct *message)
{
    bt_ext_log("SIM RESET_RSP_SENT\n");
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_SIM_RESET_RSP_SENT_IND, NULL, 0);
}


void btmtk_simap_handle_card_status_ind_sent_ind(ilm_struct *message)
{
	bt_simap_card_status_ind_sent_ind_struct *msg;
	
#ifdef BTMTK_ON_WISE 
    msg = (bt_simap_card_status_ind_sent_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (bt_simap_card_status_ind_sent_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */

    bt_ext_log("SIMAP CARD_STATUS_IND_SENT: status=%d\n", msg->status);
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_CARD_STATUS_IND_SENT_IND, msg, sizeof(bt_simap_card_status_ind_sent_ind_struct));
}


void btmtk_simap_handle_card_reader_status_rsp_sent_ind(ilm_struct *message)
{
    bt_ext_log("SIM CARD_READER_STATUS_RSP_SENT\n");
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_CARD_READER_STATUS_RSP_SENT_IND, NULL, 0);
}


void btmtk_simap_handle_set_transport_protocol_rsp_sent_ind(ilm_struct *message)
{
    bt_ext_log("SIM SET_TRANSPORT_PROTOCOL_RSP_SENT\n");
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_SET_TRANSPORT_PROTOCOL_RSP_SENT_IND, NULL, 0);
}


void btmtk_simap_handle_auth_req_timeout_rfclose_ind(ilm_struct *message)
{
	bt_simap_auth_req_timeout_rfclose_ind_struct *msg;
	
#ifdef BTMTK_ON_WISE 
    msg = (bt_simap_auth_req_timeout_rfclose_ind_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (bt_simap_auth_req_timeout_rfclose_ind_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */

    bt_ext_log("SIMAP AUTH_REQ_TIMEOUT_RFCLOSE_IND: lap=%08x, uap=%02x, nap=%04x\n", (int)msg->lap, msg->uap, msg->nap);
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_AUTH_REQ_TIMEOUT_RFCLOSE_IND, msg, sizeof(bt_simap_auth_req_timeout_rfclose_ind_struct));
}


void btmtk_simap_handle_bt_reset_ind(ilm_struct *message)
{
    bt_ext_log("SIMAP BT_RESET_IND...\n");
    g_SIMAP_MMI_Callback(NULL, EVENT_SIMAP_MMI_BT_RESET_IND, NULL, 0);
}


void btmtk_sim_handle_connect_req(ilm_struct *message)
{
    bt_ext_log("SIM CONNECT_REQ\n");
    g_SIMAP_MMI_Callback(NULL, EVENT_SIM_MMI_CONNECT_REQ, NULL, 0);
}


void btmtk_sim_handle_disconnect_req(ilm_struct *message)
{
    bt_ext_log("SIM DISCONNECT_REQ\n");
    g_SIMAP_MMI_Callback(NULL, EVENT_SIM_MMI_DISCONNECT_REQ, NULL, 0);
}


void btmtk_sim_handle_apdu_req(ilm_struct *message)
{
	bt_sim_apdu_req_struct *msg;
	
#ifdef BTMTK_ON_WISE 
    msg = (bt_sim_apdu_req_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (bt_sim_apdu_req_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */

    bt_ext_log("SIMAP APDU_REQ: transport_protocol_type=%d, apdu_req_len=%d\n", msg->transport_protocol_type, msg->apdu_req_len);
    g_SIMAP_MMI_Callback(NULL, EVENT_SIM_MMI_APDU_REQ, msg, sizeof(bt_sim_apdu_req_struct));
}


void btmtk_sim_handle_power_off_req(ilm_struct *message)
{
    bt_ext_log("SIM POWER_OFF_REQ\n");
    g_SIMAP_MMI_Callback(NULL, EVENT_SIM_MMI_POWER_OFF_REQ, NULL, 0);
}


void btmtk_sim_handle_reset_req(ilm_struct *message)
{
	bt_sim_reset_req_struct *msg;
	
#ifdef BTMTK_ON_WISE 
    msg = (bt_sim_reset_req_struct *)message->local_para_ptr;
#else   /* BTMTK_ON_WISE */
    msg = (bt_sim_reset_req_struct *)message->ilm_data;
#endif  /* BTMTK_ON_WISE */

    bt_ext_log("SIMAP RESET_REQ: transport_protocol_type=%d\n", msg->transport_protocol_type);
    g_SIMAP_MMI_Callback(NULL, EVENT_SIM_MMI_RESET_REQ, msg, sizeof(bt_sim_reset_req_struct));
}


void btmtk_simap_handle_message(ilm_struct *message)
{
	bt_ext_log("btmtk_simap_handle_message: msg_id= %d", (int)message->msg_id);

	switch (message->msg_id)
	{		
		/* SIMAP --> MMI */
		case MSG_ID_BT_SIMAP_ACTIVATE_CNF:
			btmtk_simap_handle_activate_cnf(message);
			break;
		case MSG_ID_BT_SIMAP_DEACTIVATE_CNF:
			btmtk_simap_handle_deactivate_cnf(message);
			break;
		case MSG_ID_BT_SIMAP_CONNECT_IND:
			btmtk_simap_handle_connect_ind(message);
			break;
		case MSG_ID_BT_SIMAP_DISCONNECT_IND:
			btmtk_simap_handle_disconnect_ind(message);
			break;
		case MSG_ID_BT_SIMAP_DISCONNECT_CNF:
			btmtk_simap_handle_disconnect_cnf(message);
			break;
		case MSG_ID_BT_SIMAP_AUTH_REQ:
			btmtk_simap_handle_auth_req(message);
			break;
		case MSG_ID_BT_SIMAP_ATR_RSP_SENT_IND:
			btmtk_simap_handle_atr_rsp_sent_ind(message);
			break;
		case MSG_ID_BT_SIMAP_APDU_RSP_SENT_IND:
			btmtk_simap_handle_apdu_rsp_sent_ind(message);
			break;
		case MSG_ID_BT_SIMAP_SIM_ON_RSP_SENT_IND:
			btmtk_simap_handle_sim_on_rsp_sent_ind(message);
			break;
		case MSG_ID_BT_SIMAP_SIM_OFF_RSP_SENT_IND:
			btmtk_simap_handle_sim_off_rsp_sent_ind(message);
			break;
		case MSG_ID_BT_SIMAP_SIM_RESET_RSP_SENT_IND:
			btmtk_simap_handle_sim_reset_rsp_sent_ind(message);
			break;
		case MSG_ID_BT_SIMAP_CARD_STATUS_IND_SENT_IND:
			btmtk_simap_handle_card_status_ind_sent_ind(message);
			break;
		case MSG_ID_BT_SIMAP_CARD_READER_STATUS_RSP_SENT_IND:
			btmtk_simap_handle_card_reader_status_rsp_sent_ind(message);
			break;
		case MSG_ID_BT_SIMAP_SET_TRANSPORT_PROTOCOL_RSP_SENT_IND:
			btmtk_simap_handle_set_transport_protocol_rsp_sent_ind(message);
			break;
		case MSG_ID_BT_SIMAP_AUTH_REQ_TIMEOUT_RFCLOSE_IND:
			btmtk_simap_handle_auth_req_timeout_rfclose_ind(message);
			break;

		case MSG_ID_BT_SIMAP_BT_RESET_IND:
			btmtk_simap_handle_bt_reset_ind(message);
			break;

		/* SIMAP --> SIM task */
		case MSG_ID_BT_SIM_CONNECT_REQ:
			btmtk_sim_handle_connect_req(message);
			break;
		case MSG_ID_BT_SIM_DISCONNECT_REQ:
			btmtk_sim_handle_disconnect_req(message);
			break;
		case MSG_ID_BT_SIM_APDU_REQ:
			btmtk_sim_handle_apdu_req(message);
			break;
		case MSG_ID_BT_SIM_POWER_OFF_REQ:
			btmtk_sim_handle_power_off_req(message);
			break;
		case MSG_ID_BT_SIM_RESET_REQ:
			btmtk_sim_handle_reset_req(message);
			break;

		default:
			break;
	}
}



