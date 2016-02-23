/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifdef __SDAP_TESTER__

#include "bluetooth_struct.h"
#include "bluetooth_sdap_tester_struct.h"
#include "bttypes.h"
#include "bt_common.h"
#include "conmgr.h"
#include "sdap.h"
#include "sdp_adp.h"
#include "bt_adp_sdap_tester.h"

static U32 g_sdp_uuid_len = 27;

static U32 g_sdp_uuid[] = {
	SC_SERIAL_PORT,
	SC_DIALUP_NETWORKING,
	SC_IRMC_SYNC,
	SC_OBEX_OBJECT_PUSH,
	SC_OBEX_FILE_TRANSFER,
	SC_HEADSET,
	SC_AUDIO_SOURCE,
	SC_AUDIO_SINK,
	SC_AV_REMOTE_CONTROL_TARGET,
	SC_AUDIO_DISTRIBUTION,
	SC_AV_REMOTE_CONTROL,
	SC_FAX,
	SC_HEADSET_AUDIO_GATEWAY,
	SC_PANU,
	SC_NAP,
	SC_GN,
	SC_DIRECT_PRINTING,
	SC_IMAGING_RESPONDER,
	SC_HANDSFREE,
	SC_HANDSFREE_AUDIO_GATEWAY,
	SC_BASIC_PRINTING,
	SC_HUMAN_INTERFACE_DEVICE,
	SC_SIM_ACCESS,
	SC_PBAP_CLIENT,
	SC_PBAP_SERVER,
	SC_GENERIC_AUDIO,
	SC_MAP,
};

static U16 g_sdp_attr[] = {
    AID_PROTOCOL_DESC_LIST,
    AID_SUPPORTED_FORMATS_LIST 
};

static CmgrHandler g_cmgr_handler;

static SdapTesterParams g_params;

void _proceed_test();

void _sdp_result_callback(SdpQueryToken *sqt, U8 result, U8 attr_idx, U8 *attr_value) {
	OS_Report("[SDAP_Tester] %s, attr_idx: %d, result: %d", __FUNCTION__, attr_idx, result);

	if (--g_params.sm_count == 0) {
		OS_Report("[SDAP_Tester] sm count down to 0, remove data link");
		sqt->rm->discFlag = TRUE;
		// CMGR_RemoveDataLink(&g_cmgr_handler);
		// CMGR_DeregisterHandler(&g_cmgr_handler);
	}
}

void _cmgr_callback(CmgrHandler *handler, CmgrEvent event, BtStatus status) {
	OS_Report("[SDAP_Tester] %s(), event: %d, status: %d", __FUNCTION__, event, status);

	switch (event) {
		case CMEVENT_DATA_LINK_CON_CNF:		
			g_params.sm_search_pattern.rm = handler->remDev;
			g_params.sm_search_pattern.sqt = &g_params.local_sqt;
			// g_params.sm_search_pattern.uuid = SC_OBEX_OBJECT_PUSH;
			// g_params.sm_search_pattern.attribute_num = 0x02;
			g_params.sm_search_pattern.uuid = SC_IMAGING_RESPONDER;
			g_params.sm_search_pattern.attribute_id = g_sdp_attr;
			g_params.sm_search_pattern.attribute_num = 0x01;
			g_params.sm_search_pattern.callback = _sdp_result_callback;
			_proceed_test();
			break;

		case CMEVENT_DATA_LINK_DIS:
			OS_Report("[SDAP Tester] Failed to create data link.");
			CMGR_DeregisterHandler(&g_cmgr_handler);
			break;

		default:
			break;
	}
}

BtStatus _create_link(U8 *addr) {
	BtStatus status;
	BD_ADDR bdaddr;

	memset(&g_cmgr_handler, 0, sizeof(g_cmgr_handler));
	status = CMGR_RegisterHandler(&g_cmgr_handler, (CmgrCallback) _cmgr_callback);
	if (status == BT_STATUS_SUCCESS) {
		bdaddr.addr[0] = addr[0];
		bdaddr.addr[1] = addr[1];
		bdaddr.addr[2] = addr[2];
		bdaddr.addr[3] = addr[3];
		bdaddr.addr[4] = addr[4];
		bdaddr.addr[5] = addr[5];
		status = CMGR_CreateDataLink(&g_cmgr_handler, &bdaddr);

		if (status != BT_STATUS_SUCCESS || status != BT_STATUS_PENDING) {
			CMGR_DeregisterHandler(&g_cmgr_handler);
		}
	}

	return status;
}

/********************************************************************************************
 * Test case functions
 ********************************************************************************************/
void _proceed_test() {
	BtStatus status = BT_STATUS_FAILED;
	U32 i;

	OS_Report("[SDAP_Tester] %s", __FUNCTION__);
	switch (g_params.cmd) { 
		case SDAP_TESTER_CMD_SA_2:
			for (i = 0; i < 2; i++) {
				status = SDAP_ServiceSearchAll(g_params.sa_search_pattern);
				OS_Report("[SDAP_Tester] Search all, i: %d, status: %d", i, status);
			}
			break;

		case SDAP_TESTER_CMD_SA_FULL:
			for (i = 0; i < 5; i++) {
				status = SDAP_ServiceSearchAll(g_params.sa_search_pattern);
				OS_Report("[SDAP_Tester] Search all, i: %d, status: %d", i, status);
			}
			break;

		case SDAP_TESTER_CMD_SM_2:
			g_params.sm_count = 2;
			for (i = 0 ; i < 2; i++) {
				status = SDAP_ServiceSearchMultipleAttribute(g_params.sm_search_pattern);
				OS_Report("[SDAP_Tester] Search multiple attr, i: %d, stataus: %d", i, status);
			}
			break;

		case SDAP_TESTER_CMD_SM_FULL:
			g_params.sm_count = 5;
			for (i = 0 ; i < 5; i++) {
				status = SDAP_ServiceSearchMultipleAttribute(g_params.sm_search_pattern);
				OS_Report("[SDAP_Tester] Search multiple attr, i: %d, stataus: %d", i, status);
			}
			break;

		case SDAP_TESTER_CMD_SA_SM:
			g_params.sm_count = 1;
			status = SDAP_ServiceSearchAll(g_params.sa_search_pattern);
			OS_Report("[SDAP_Tester] Search all, stataus: %d", status);
			status = SDAP_ServiceSearchMultipleAttribute(g_params.sm_search_pattern);
			OS_Report("[SDAP_Tester] Search multiple attr, stataus: %d", status);
			break;

		case SDAP_TESTER_CMD_SA_SM_FULL:
			g_params.sm_count = 2;
			status = SDAP_ServiceSearchAll(g_params.sa_search_pattern);
			OS_Report("[SDAP_Tester] Search all, stataus: %d", status);
			status = SDAP_ServiceSearchMultipleAttribute(g_params.sm_search_pattern);
			OS_Report("[SDAP_Tester] Search multiple attr, stataus: %d", status);
			status = SDAP_ServiceSearchAll(g_params.sa_search_pattern);
			OS_Report("[SDAP_Tester] Search all, stataus: %d", status);
			status = SDAP_ServiceSearchMultipleAttribute(g_params.sm_search_pattern);
			OS_Report("[SDAP_Tester] Search multiple attr, stataus: %d", status);
			status = SDAP_ServiceSearchAll(g_params.sa_search_pattern);
			OS_Report("[SDAP_Tester] Search all, stataus: %d", status);
			break;

		case SDAP_TESTER_CMD_SA_OVERFLOW:
			g_params.sm_count = 3;
			for (i = 0; i < 3; i++) {
				status = SDAP_ServiceSearchMultipleAttribute(g_params.sm_search_pattern);
				OS_Report("[SDAP_Tester] Search multiple attr, stataus: %d", status);
				status = SDAP_ServiceSearchAll(g_params.sa_search_pattern);
				OS_Report("[SDAP_Tester] Search all, stataus: %d", status);
			}
			break;

		case SDAP_TESTER_CMD_SM_OVERFLOW:
			g_params.sm_count = 2;
			for (i = 0; i < 3; i++) {
				status = SDAP_ServiceSearchAll(g_params.sa_search_pattern);
				OS_Report("[SDAP_Tester] Search all, stataus: %d", status);
				status = SDAP_ServiceSearchMultipleAttribute(g_params.sm_search_pattern);
				OS_Report("[SDAP_Tester] Search multiple attr, stataus: %d", status);
			}
			break;

		default:
			OS_Report("[SDAP_Tester] %s(), unknown cmd: %d", __FUNCTION__, g_params.cmd);
			break;
	}

}

/********************************************************************************************
 * Message handlers
 ********************************************************************************************/
/* MSG_ID_BT_SDAP_TESTER_CMD handler function */
void sdap_tester_handle_cmd(ilm_struct *ilm_ptr) {
	BtStatus status = BT_STATUS_SUCCESS;
	bt_sdap_tester_cmd_struct *cmd;
	cmd = (bt_sdap_tester_cmd_struct *) ilm_ptr->ilm_data;

	g_params.cmd = cmd->cmd;
	// Step 1. Create data link if necessary
	OS_Report("[SDAP_Tester] Checking and Creating data link");
	switch (cmd->cmd) {
		case SDAP_TESTER_CMD_SA_2:
		case SDAP_TESTER_CMD_SA_FULL:
			break;
		case SDAP_TESTER_CMD_SM_2:
		case SDAP_TESTER_CMD_SM_FULL:
		case SDAP_TESTER_CMD_SA_SM:
		case SDAP_TESTER_CMD_SA_SM_FULL:
		case SDAP_TESTER_CMD_SA_OVERFLOW:
		case SDAP_TESTER_CMD_SM_OVERFLOW:
			status = _create_link(cmd->addr);
			break;
		default:
			OS_Report("[SDAP_Tester] %s(), unknown cmd: %d", __FUNCTION__, cmd->cmd);
			return;
	}

	// Step 2. Prepare parameters and proceed the test if link is created successfully
	OS_Report("[SDAP_Tester] cmd: %d, status %d", cmd->cmd, status);
	g_params.bdAddr.addr[0] = cmd->addr[0];
	g_params.bdAddr.addr[1] = cmd->addr[1];
	g_params.bdAddr.addr[2] = cmd->addr[2];
	g_params.bdAddr.addr[3] = cmd->addr[3];
	g_params.bdAddr.addr[4] = cmd->addr[4];
	g_params.bdAddr.addr[5] = cmd->addr[5];
	g_params.sa_search_pattern.bdAddr = &g_params.bdAddr;
	g_params.sa_search_pattern.sdap_len = g_sdp_uuid_len;
	g_params.sa_search_pattern.sdp_uuid = g_sdp_uuid;

	if (status == BT_STATUS_SUCCESS) {
		g_params.sm_search_pattern.rm = g_cmgr_handler.remDev;
		g_params.sm_search_pattern.sqt = &g_params.local_sqt;
		// g_params.sm_search_pattern.uuid = SC_OBEX_OBJECT_PUSH;
		// g_params.sm_search_pattern.attribute_num = 0x02;
		g_params.sm_search_pattern.uuid = SC_IMAGING_RESPONDER;
		g_params.sm_search_pattern.attribute_id = g_sdp_attr;
		g_params.sm_search_pattern.attribute_num = 0x01;
		g_params.sm_search_pattern.callback = _sdp_result_callback;

		_proceed_test();
	}
}

/********************************************************************************************
 * Entry function for message handlers
 ********************************************************************************************/
void btmtk_adp_sdap_tester_handle_message(ilm_struct *ilm_ptr) {
	switch (ilm_ptr->msg_id) {
		case MSG_ID_BT_SDAP_TESTER_CMD:
			sdap_tester_handle_cmd(ilm_ptr);
			break;
		default:
			break;
	}
}

#endif
