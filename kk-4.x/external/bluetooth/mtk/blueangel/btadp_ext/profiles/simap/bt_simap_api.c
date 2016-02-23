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

/* 
 * bt_simap_api.c
 * 
 * This file is the External Adaptation API of SIMAP profile used by Application.
 * Flow direction: APP --> external ADP API
 */

#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_simap_message.h"
#include "bluetooth_simap_struct.h"
#include "bt_simap_api.h"
#include "bt_struct.h"
#include "memory.h"
#include "ext_osal.h"
#include "bt_mmi_msg.h"
#include "bt_simap_porting.h"


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

#define LOG_TAG "BTSIMAP_EXT_API"

#include "bt_ext_debug.h"


BTMTK_EventCallBack g_SIMAP_MMI_Callback = NULL;


void btmtk_simap_register_mmi_callback_req(BTMTK_EventCallBack callback)
{
	g_SIMAP_MMI_Callback = callback;
}


void btmtk_simap_activate_req(void)
{
	bt_simap_send_msg(MSG_ID_BT_SIMAP_ACTIVATE_REQ, NULL, 0);
}


void btmtk_simap_deactivate_req(void)
{
	bt_simap_send_msg(MSG_ID_BT_SIMAP_DEACTIVATE_REQ, NULL, 0);
}


void btmtk_simap_send_status_ind_with_unavailable_req(void)
{
	bt_simap_send_msg(MSG_ID_BT_SIMAP_SEND_STATUS_IND_WITH_UNAVAILABLE_REQ, NULL, 0);
}


void btmtk_simap_auth_rsp(U8 result)
{
	bt_simap_auth_rsp_struct msg;
	
	bt_ext_log("btmtk_simap_auth_rsp: result=%d",result);

	msg.result = result;
	bt_simap_send_msg(MSG_ID_BT_SIMAP_AUTH_RSP, &msg, sizeof(bt_simap_auth_rsp_struct));
}


void btmtk_simap_disconnect_req(SimDisconnectType discType)
{
	bt_simap_disconnect_req_struct msg;

	bt_ext_log("btmtk_simap_disconnect_req: discType=%d",discType);

	msg.discType = discType;
	bt_simap_send_msg(MSG_ID_BT_SIMAP_DISCONNECT_REQ, &msg, sizeof(bt_simap_disconnect_req_struct));
}


void btmtk_sim_connect_cnf(U16 result, U8 currentTPType, U8 TPTypeCap, U16 atrLen, U8* atr)
{
	bt_sim_connect_cnf_struct msg;

	bt_ext_log("btmtk_sim_connect_cnf: result=%d,atrLen=%d, atr=%s",result,atrLen,atr);

	msg.result = result;
	msg.current_transport_protocol_type = currentTPType;
	msg.supported_transport_protocol_type_capability = TPTypeCap;
	msg.atr_len = atrLen > ATR_MAX_LEN ? ATR_MAX_LEN : atrLen;
	memcpy(msg.atr, atr, msg.atr_len);
	bt_simap_send_msg(MSG_ID_BT_SIM_CONNECT_CNF, &msg, sizeof(bt_sim_connect_cnf_struct));
}


void btmtk_sim_reset_cnf(U16 result, U8 currentTPType, U16 atrLen, U8* atr)
{
	bt_sim_reset_cnf_struct msg;

	bt_ext_log("btmtk_sim_reset_cnf: result=%d,atrLen=%d, atr=%s",result,atrLen,atr);

	msg.result = result;
	msg.current_transport_protocol_type = currentTPType;
	msg.atr_len = atrLen > ATR_MAX_LEN ? ATR_MAX_LEN : atrLen;
	memcpy(msg.atr, atr, msg.atr_len);
	bt_simap_send_msg(MSG_ID_BT_SIM_RESET_CNF, &msg, sizeof(bt_sim_reset_cnf_struct));
}

/*
void btmtk_sim_power_on_cnf(U16 result, U8 currentTPType, U16 atrLen, U8* atr)
{
	bt_sim_power_on_cnf_struct msg;

	bt_ext_log("btmtk_sim_power_on_cnf: result=%d,atrLen=%d, atr=%s",result,atrLen,atr);

	msg.result = result;
	msg.current_transport_protocol_type = currentTPType;
	msg.atr_len = atrLen;
	memcpy(msg.atr, atr, atrLen);
	bt_simap_send_msg(MSG_ID_BT_SIM_POWER_ON_CNF, &msg, sizeof(bt_sim_reset_cnf_struct));
}
*/
void btmtk_sim_apdu_cnf(U16 result, U16 apduRspLen, U8* apdu_rsp)
{
	bt_sim_apdu_cnf_struct msg;

	bt_ext_log("btmtk_sim_apdu_cnf: result=%d,apduRspLen=%d, apdu_rsp=%s",result,apduRspLen,apdu_rsp);

	msg.result = result;
	msg.apdu_rsp_len= apduRspLen;
	memcpy(msg.apdu_rsp, apdu_rsp, apduRspLen);
	bt_simap_send_msg(MSG_ID_BT_SIM_APDU_CNF, &msg, sizeof(bt_sim_apdu_cnf_struct));
}


void btmtk_sim_disconnect_cnf(U16 result)
{
	bt_sim_disconnect_cnf_struct msg;

	bt_ext_log("btmtk_sim_disconnect_cnf: result=%d",result);

	msg.result = result;
	bt_simap_send_msg(MSG_ID_BT_SIM_DISCONNECT_CNF, &msg, sizeof(bt_sim_disconnect_cnf_struct));
}


void btmtk_sim_power_off_cnf(U16 result)
{
	bt_sim_power_off_cnf_struct msg;

	bt_ext_log("btmtk_sim_power_off_cnf: result=%d",result);

	msg.result = result;
	bt_simap_send_msg(MSG_ID_BT_SIM_POWER_OFF_CNF, &msg, sizeof(bt_sim_power_off_cnf_struct));
}


void btmtk_simap_inject_message_req(U8 *string)
{
	bt_simap_inject_req_struct msg;

	memset(msg.string, 0, sizeof(msg.string));
	memcpy(msg.string, string, SIMAP_INJECT_MESSAGE_LENGTH_MAX);
	bt_simap_send_msg(MSG_ID_BT_SIMAP_INJECT_MESSAGE, &msg, sizeof(bt_simap_inject_req_struct));
}



