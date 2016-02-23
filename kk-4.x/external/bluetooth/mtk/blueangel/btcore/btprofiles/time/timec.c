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

/*****************************************************************************
 *
 * Filename:
 * ---------
 * timec.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Time profile
 *
 * Author:
 * -------
 * Jacob Lee
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 * 
 * 10 16 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 * 
 * 10 15 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_TIMEC_PROFILE__
#include "bttypes.h"
#include "bt_os_api.h"
#include "bluetooth_time_struct.h"
#include "time_gatt.h"
#include "timec.h"
#include "utils.h"
#include "eventmgr.h"

static TimeCContext _timec_contexts[TIME_MAX_SERVER_NUM];

static ListEntry _timec_packet_list;
static TimePacket _timec_packets[TIME_MAX_SERVER_NUM];

/********************************************************************************************
  * External functions
  ********************************************************************************************/
extern BtStatus time_gatt_register_client_callback() ;
extern BtStatus time_gatt_deregister_client_callback();
extern BtStatus time_gatt_connect_service(TimeCTransport *pChnl);
extern BtStatus time_gatt_destroy_link(TimeCTransport *pChnl);
extern BtStatus time_gatt_disconnect_service(TimeCTransport *pChnl);
extern BtStatus time_gatt_read_remote_value(TimePacket *pkt, U8 seq_num, U32 value_id);
extern BtStatus time_gatt_write_remote_value(TimePacket *pkt, U8 seq_num, U32 value_id, U16 value);

/*********************************************************************************************
 *  Callback functions
 *********************************************************************************************/
void timec_cmgr_callback(CmgrHandler *handler, CmgrEvent event, BtStatus status) {
	TimeCTransport *pChnl;
	TimeCContext *pCntx;

	pChnl = ContainingRecord(handler, TimeCTransport, handler);
	pCntx = ContainingRecord(pChnl, TimeCContext, chnl);

	TIMEASSERT(pChnl->check_byte == TIME_CHECK_BYTE);

	OS_Report("[Time] timec_cmgr_callback, event: %d, status: %d", event, status);

	switch (event) {
		case CMEVENT_DATA_LINK_CON_CNF:
			if (pCntx->state == TIME_CLIENT_LINK_CONNECT) {
				timec_notify_link_result(pCntx, status);

			} else if (pCntx->state == TIME_CLIENT_DISCONNECTING) {
				if (status == BT_STATUS_SUCCESS) {
					// _destroy_link(pChnl);
				} else {
					timec_notify_disconnect_result(pCntx, BT_STATUS_SUCCESS);
				}
			}
			break;

		case CMEVENT_DATA_LINK_CON_IND:
			// incoming connection
			timec_notify_incoming_link(pCntx);
			break;

		case CMEVENT_DATA_LINK_DIS:
			if (pCntx->state != TIME_CLIENT_ENABLED && pCntx->state != TIME_CLIENT_IDLE) {
				timec_notify_disconnect_result(pCntx, BT_STATUS_SUCCESS);
			}
			break;

		default:
			break;
	}
}

/*********************************************************************************************
 * Internal data and functions
 *********************************************************************************************/
static TimeCContext* _get_client_cntx() {
	U8 i = 0;
	TimeCContext* pCntx;

	for (i = 0; i < TIME_MAX_SERVER_NUM; i++) {
		pCntx = &_timec_contexts[i];
		if (pCntx->in_use == FALSE) {
			pCntx->in_use = TRUE;
			return pCntx;
		}
	}

	OS_Report("[Time] No available client context.");
	return NULL;
}

static void _release_client_cntx(TimeCContext *pCntx) {
	U8 i = 0;

	for (i = 0; i < TIME_MAX_SERVER_NUM; i++) {
		if (pCntx == &_timec_contexts[i]) {
			pCntx->in_use = FALSE;
			// Clear cached handles
			btmtk_os_memset((U8 *) pCntx->chnl.cached_handles, 0, sizeof(pCntx->chnl.cached_handles));
			return;
		}
	}

	OS_Report("[Time] Invalid client context pointer: 0x%x", pCntx);
	TIMEASSERT(FALSE);
}

TimePacket* _get_client_pkt(void *pChnl) {
	TimePacket *pkt = NULL;

	if (!IsListEmpty(&_timec_packet_list)) {
		pkt = (TimePacket *) RemoveHeadList(&_timec_packet_list);
		pkt->chnl = pChnl;
		btmtk_os_memset((U8 *) &pkt->gatt_params, 0, sizeof(pkt->gatt_params));
	}

	if (pkt == NULL) {
		OS_Report("[Time] No available client packet.");
	}

	return pkt;
}

void _release_client_pkt(TimePacket *pkt) {
	InsertTailList(&_timec_packet_list, &pkt->node);
}

// This function doesn't have any effect for now.
static BtStatus _query_channel(TimeCContext *pCntx) {
	BtStatus status;

	pCntx->state = TIME_CLIENT_QUERY;
	// status = TIME_ATT_Query_Service(&pCntx->chnl);
	return status;
}

static BtStatus _create_link(TimeCTransport *pChnl, unsigned char *addr) {
	BtStatus status = BT_STATUS_SUCCESS;
	TIMEASSERT(pChnl->check_byte == TIME_CHECK_BYTE);

	// memcpy(&chnl->bdaddr.addr, addr, 6);
	pChnl->bdaddr.addr[5] = addr[5];
	pChnl->bdaddr.addr[4] = addr[4];
	pChnl->bdaddr.addr[3] = addr[3];
	pChnl->bdaddr.addr[2] = addr[2];
	pChnl->bdaddr.addr[1] = addr[1];
	pChnl->bdaddr.addr[0] = addr[0];

	memset(&pChnl->handler, 0, sizeof(pChnl->handler));
	// status = CMGR_RegisterHandler(&pChnl->handler, (CmgrCallback) timec_cmgr_callback);
	OS_Report("[Time] %s(), register CMGR handler, status: %d", __FUNCTION__, status);

	if (status == BT_STATUS_SUCCESS) {
		status = CMGR_CreateDataLink(&pChnl->handler, &pChnl->bdaddr);
		OS_Report("[Time] %s(), create data link to %x:%x:%x:%x:%x:%x, status: %d", __FUNCTION__,
				addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], status);

		if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
			CMGR_DeregisterHandler(&(pChnl->handler));
		}
	}

	return status;
}

static BtStatus _destroy_link(TimeCTransport *pChnl) {
	BtStatus status;

	status = CMGR_RemoveDataLink(&pChnl->handler);
	status = CMGR_DeregisterHandler(&pChnl->handler);

	// Always return success
	return BT_STATUS_SUCCESS;
}

static BtStatus _open_channel(TimeCContext *pCntx) {
	BtStatus status;

	if (pCntx == NULL) {
		return BT_STATUS_FAILED;
	}
	pCntx->state = TIME_CLIENT_CONNECTING;
	status = time_gatt_connect_service(&pCntx->chnl);
	return status;
}

/*********************************************************************************************
 * Functions called from internal adaptation layer
 *********************************************************************************************/
BT_BOOL TIMEC_ResetContext() {
	U8 i = 0;
	TimeCContext *pCntx;

	OS_Report("[Time] %s()", __FUNCTION__);

	InitializeListHead(&_timec_packet_list);
	// Reset the context
	for (i = 0; i < TIME_MAX_SERVER_NUM; i++) {
		pCntx = &_timec_contexts[i];
		memset((void *) pCntx, 0, sizeof(pCntx));

		_timec_packets[i].check_byte = TIME_CHECK_BYTE;
		InsertTailList(&_timec_packet_list, &(_timec_packets[i].node));
	}
	return TRUE;
}

BtStatus TIMEC_Init(TimeCApp *app, BTMTK_TIMEC_CALLBACK callback) {
	BtStatus status = BT_STATUS_FAILED;
	TimeCContext *pCntx;

	OS_Report("[Time] %s()", __FUNCTION__);
	if (app == NULL) {
		return status;
	}

	if (app->timecContext != NULL) {
		// TODO: To terminate current connection when duplicated registration.
		return BT_STATUS_SUCCESS;
	}

	pCntx = _get_client_cntx();
	if (pCntx == NULL) {
		return status;
	}

	status = CMGR_RegisterHandler(&(pCntx->chnl.handler), (CmgrCallback) timec_cmgr_callback);
	if (status == BT_STATUS_SUCCESS) {
		app->timecContext = pCntx;
		pCntx->app = app;
		pCntx->chnl.check_byte = TIME_CHECK_BYTE;
		pCntx->callback = callback;
		pCntx->state = TIME_CLIENT_ENABLED;

	} else {
		OS_Report("[Time] Failed to register TIP client");
		_release_client_cntx(pCntx);
	}

	return status;
}

BtStatus TIMEC_Deinit(TimeCApp *app) {
	BtStatus status = BT_STATUS_SUCCESS;
	TimeCContext *pCntx;

	OS_Report("[Time] %s()", __FUNCTION__);
	if (app == NULL) {
		OS_Report("[Time] Warning: TimeCApp is null.");
		return status;
	}

	pCntx = app->timecContext;
	if (pCntx == NULL) {
		OS_Report("[Time] Warning: Time client context is null");
		return status;
	}

	CMGR_DeregisterHandler(&(pCntx->chnl.handler));

	TIMEASSERT(pCntx->chnl.check_byte == TIME_CHECK_BYTE);
	_release_client_cntx(pCntx);
	app->timecContext = NULL;
	return status;
}

BtStatus TIMEC_Connect(TimeCApp *app, U8 addr[6], BTMTK_TIMEC_CALLBACK callback) {
	BtStatus status;
	TimeCContext *pCntx = app->timecContext;

	OS_Report("[Time] %s()", __FUNCTION__);
	if (pCntx == NULL) {
		return BT_STATUS_FAILED;
	}

	TIMEASSERT(pCntx->chnl.check_byte == TIME_CHECK_BYTE);

	pCntx->state = TIME_CLIENT_LINK_CONNECT;
	pCntx->callback = callback;
	pCntx->app = app;

	// Creat ACL link
	status = _create_link(&pCntx->chnl, addr);
	if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
		OS_Report("[Time] Client failed to crate link.");
		return BT_STATUS_FAILED;
	}

	// Connect
	if (status == BT_STATUS_SUCCESS) {
		status = _open_channel(app->timecContext);
		if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
			OS_Report("[Time] Client failed to open channel.");
			time_gatt_destroy_link(&pCntx->chnl);
			return BT_STATUS_FAILED;
		}
	}

	return status;
}

BtStatus TIMEC_Disconnect(TimeCApp *app) {
	BtStatus status = BT_STATUS_SUCCESS;
	TimeCContext *pCntx = app->timecContext;

	OS_Report("[Time] %s()", __FUNCTION__);
	if (pCntx == NULL) {
		return BT_STATUS_FAILED;
	}

	if (pCntx->state != TIME_CLIENT_ENABLED) {
		pCntx->state = TIME_CLIENT_DISCONNECTING;
		status = time_gatt_disconnect_service(&pCntx->chnl);
	}

	return status;
}

BtStatus TIMEC_Read_Remote_Value(TimeCApp *app, U8 value_id) {
	BtStatus status = BT_STATUS_FAILED;
	TimeCContext *pCntx = app->timecContext;
	TimePacket *pkt;

	OS_Report("[Time] %s(), value: %d", __FUNCTION__, value_id);
	if (pCntx == NULL || pCntx->state != TIME_CLIENT_CONNECTED) {
		return status;
	}

	pkt = _get_client_pkt(&pCntx->chnl);
	if (pkt != NULL) {
		// seq = 1. We have only one packet for now.
		status = time_gatt_read_remote_value(pkt, 1, value_id);

		// Basically, the status value could only be BT_STATUS_PENDING
		if (status != BT_STATUS_PENDING) {
			_release_client_pkt(pkt);
		}
	}

	return status;
}

BtStatus TIMEC_Write_Remote_Value(TimeCApp *app, U8 value_id, U16 value) {
	BtStatus status = BT_STATUS_FAILED;
	TimeCContext *pCntx = app->timecContext;
	TimePacket *pkt;

	OS_Report("[Time] %s()", __FUNCTION__);
	if (pCntx == NULL || pCntx->state != TIME_CLIENT_CONNECTED) {
		return status;
	}

	if (value == TIME_UPDATE_CTRL_CANCEL) {
		pCntx->cancel_update = TRUE;
	} else {
		pCntx->cancel_update = FALSE;
	}

	pkt = _get_client_pkt(&pCntx->chnl);
	if (pkt != NULL) {
		// seq = 1. We have only one packet for now.
		status = time_gatt_write_remote_value(pkt, 1, value_id, value);

		// Basically, the status value could only be BT_STATUS_PENDING
		if (status != BT_STATUS_PENDING) {
			_release_client_pkt(pkt);
		}
	}

	return status;
}


/*********************************************************************************************
 * Notify functions. To internal and external adaptation layers.
 *********************************************************************************************/
/* Notify internal adaption layer for sending ILM to MMI */
void timec_notify(TimeCContext *pCntx, U8 event_id, void *data) {
	TimeCEvent event;
	if (pCntx->callback) {
		event.event_id = event_id;
		event.data = data;
		pCntx->callback(&event, pCntx->app);

	} else {
		OS_Report("[Time] Null client callback. Force disconnect.");

		if (pCntx->state != TIME_CLIENT_DISCONNECTING && pCntx->state != TIME_CLIENT_ENABLED) {
			TIMEC_Disconnect(pCntx->app);
		}
	}
}

void timec_notify_link_result(TimeCContext *pCntx, U32 rspcode) {
	BtStatus status = BT_STATUS_FAILED;

	OS_Report("[Time] %s(), rspcode: %d", __FUNCTION__, rspcode);
	if (pCntx->state != TIME_CLIENT_LINK_CONNECT) {
		OS_Report("[Time] %s(): Invalid state %d", __FUNCTION__, pCntx->state);
		return;
	}

	if (rspcode == BT_STATUS_SUCCESS) {
		status = _open_channel(pCntx);
		timec_notify_connect_result(pCntx, status);

	} else if (rspcode == BT_STATUS_PENDING) {
		// Wait for the response

	} else {
		status = time_gatt_destroy_link(&pCntx->chnl);
		if (status == BT_STATUS_SUCCESS) {
			// Service not found.
			timec_notify_connect_result(pCntx, BT_STATUS_NOT_FOUND);
		}
	}
}

void timec_notify_incoming_link(TimeCContext *pCntx) {
	BtStatus status= BT_STATUS_FAILED;

	OS_Report("[Time] %s()", __FUNCTION__);

	if (pCntx == NULL) {
		OS_Report("[Time][Error] %s(), NULL client context", __FUNCTION__);
		return;
	}

	if (pCntx->state == TIME_CLIENT_ENABLED) {
		pCntx->state = TIME_CLIENT_CONNECTING;
		
		time_gatt_register_client_callback();
		status = _open_channel(pCntx);
		if (status == BT_STATUS_SUCCESS) {
			timec_notify_connect_result(pCntx, status);
		} else if (status == BT_STATUS_PENDING) {
			// Wait for result
		} else {
			time_gatt_deregister_client_callback();
			// Do NOT destroy data link even if we cannot connect with GATT service
		}
	} else {
	}

}

void timec_notify_connect_result(TimeCContext *pCntx, U32 rspcode) {
	BtStatus status = BT_STATUS_FAILED;

	OS_Report("[Time] %s(), rspcode: %d", __FUNCTION__, rspcode);
	switch (pCntx->state) {
		case TIME_CLIENT_ENABLED:
		case TIME_CLIENT_LINK_CONNECT:
		case TIME_CLIENT_QUERY:
		case TIME_CLIENT_CONNECTING:
			// Valid states
			break;

		default:
			OS_Report("[Time] %s(): Invalid state %d", __FUNCTION__, pCntx->state);
			return;
	}

	if (rspcode == BT_STATUS_SUCCESS) {
		pCntx->state = TIME_CLIENT_CONNECTED;
		timec_notify(pCntx, TIMEC_EVENT_CONNECTED, &rspcode);

	} else if (rspcode == BT_STATUS_PENDING) {
		// Wait for response

	} else {
		pCntx->state = TIME_CLIENT_ENABLED;
		timec_notify(pCntx, TIMEC_EVENT_CONNECT_FAIL, &rspcode);
	}
}

void timec_notify_disconnect_result(TimeCContext *pCntx, U32 rspcode) {
	pCntx->state = TIME_CLIENT_ENABLED;
	time_gatt_deregister_client_callback();
	timec_notify(pCntx, TIMEC_EVENT_DISCONNECTED, &rspcode);
}

void timec_notify_read_remote_value_result(TimeCContext *pCntx, U32 rspcode, TimePacket *pkt) {
	U8 event_type = 0;
	switch (pkt->value_id) {
		case GATT_TIME_CTTIME:
			event_type = TIMEC_EVENT_GET_CTTIME;
			break;
		case GATT_TIME_CTTIME_NOTIFY:
			event_type = TIMEC_EVENT_GET_CTTIME_NOTIFY;
			break;
	}
	_release_client_pkt(pkt);
	timec_notify(pCntx, event_type, &rspcode);
}

void timec_notify_write_remote_value_result(TimeCContext *pCntx, U32 rspcode, TimePacket *pkt) {
	U8 event_type = 0;
	switch (pkt->value_id) {
		case GATT_TIME_SERVER_UPDATE_CTRL:
			if (pCntx->cancel_update != TRUE) {
				event_type = TIMEC_EVENT_REQUEST_UPDATE;
			} else {
				event_type = TIMEC_EVENT_CANCEL_UPDATE;
			}
			break;

		case GATT_TIME_CTTIME_NOTIFY:
			event_type = TIMEC_EVENT_SET_CTTIME_NOTIFY;
			break;
	}
	_release_client_pkt(pkt);
	timec_notify(pCntx, event_type, &rspcode);
}

void timec_notify_server_notification(TimeCContext *pCntx, U8 value_id) {
	U8 event_type = 0;
	switch (value_id) {
		case GATT_TIME_CTTIME:
			event_type = TIMEC_EVENT_CTTIME_UPDATED;
			break;

		default:
			break;
	}
	timec_notify(pCntx, event_type, BT_STATUS_SUCCESS);
}

#endif
