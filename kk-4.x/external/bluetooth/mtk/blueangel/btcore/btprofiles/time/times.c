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
 * times.c
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
#ifdef __BT_TIMES_PROFILE__
#include "bttypes.h"
#include "bt_os_api.h"
#include "eventmgr.h"

#include "bluetooth_time_struct.h"
#include "time_gatt.h"
#include "times.h"


static TimeSContext _times_contexts[TIME_MAX_CLIENT_NUM];

static ListEntry _times_tp_list;
static TimeSTransport *_times_used_tps[TIME_MAX_CLIENT_NUM];

static ListEntry _times_packet_list;
static TimePacket _times_packets[TIME_MAX_CLIENT_NUM];

/********************************************************************************************
  * External functions
  ********************************************************************************************/
extern U8 TimeAttDB_AddRecord();
extern U8 TimeAttDB_RemoveRecord();

extern BtStatus time_gatt_register_server_callback();
extern BtStatus time_gatt_deregister_server_callback();
extern BtStatus time_gatt_server_disconnect(TimeSTransport *pChnl);
extern BtStatus time_gatt_write_local_value(TimeSContext *pCntx, U8 value_id);
extern BtStatus time_gatt_notify_value(TimePacket *pkt, U8 seq_num, U32 value_id);

/*********************************************************************************************
 * Internal data and functions
 *********************************************************************************************/
static TimeSContext* _get_server_cntx() {
	U8 i = 0;
	TimeSContext* pCntx;

	for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
		pCntx = &_times_contexts[i];
		if (pCntx->in_use == FALSE) {
			pCntx->in_use = TRUE;
			return pCntx;
		}
	}

	OS_Report("[Time] No available server context.");
	return NULL;
}

static void _release_server_cntx(TimeSContext *pCntx) {
	U8 i = 0;

	for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
		if (pCntx == &_times_contexts[i]) {
			pCntx->in_use = FALSE;
			return;
		}
	}

	OS_Report("[Time] Invalid server context pointer: 0x%x", pCntx);
	TIMEASSERT(FALSE);
}

static TimeSTransport *_get_server_tp(U8 *addr) {
	U8 i;
	TimeSTransport *server_tp;

	// If the link (BD_ADDR) has connected, return NULL.
	for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
		server_tp = _times_used_tps[i];
		if (server_tp == NULL) {
			break;
		} else {
			if (btmtk_os_memcmp(server_tp->bdaddr.addr, 6, addr, 6)) {
				OS_Report("[Time] %x:%x:%x:%x:%x:%x has already connected.",
						server_tp->bdaddr.addr[0], server_tp->bdaddr.addr[1], server_tp->bdaddr.addr[2],
						server_tp->bdaddr.addr[3], server_tp->bdaddr.addr[4], server_tp->bdaddr.addr[5]);
				return NULL;
			}
		}
	}

	if (i < TIME_MAX_CLIENT_NUM && !IsListEmpty(&_times_tp_list)) {
		// Found one avaialbe server transport
		server_tp = (TimeSTransport *) RemoveHeadList(&_times_tp_list);
		// memcpy(server_tp->bdaddr.addr, addr, 6);
		server_tp->bdaddr.addr[0] = addr[0];
		server_tp->bdaddr.addr[1] = addr[1];
		server_tp->bdaddr.addr[2] = addr[2];
		server_tp->bdaddr.addr[3] = addr[3];
		server_tp->bdaddr.addr[4] = addr[4];
		server_tp->bdaddr.addr[5] = addr[5];
		server_tp->cttime_notify_config = 0x0000;
		server_tp->update_status.cur_state = TIME_UPDATE_STATE_IDLE;
		server_tp->update_status.result = TIME_UPDATE_RESULT_NO_CONNECTION;
		_times_used_tps[i] = server_tp;

	} else {
		// No available server transport
		server_tp = NULL;
	}

	return server_tp;
}

TimeSTransport* times_find_server_tp(U8 *addr) {
	U8 i;
	TimeSTransport *server_tp;

	/*
	if (link == NULL) {
		return NULL;
	}
	*/

	for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
		server_tp = _times_used_tps[i];
		if ((server_tp != NULL) && btmtk_os_memcmp(server_tp->bdaddr.addr, 6, addr, 6)) {
			return server_tp;
		}
	}

	OS_Report("[Time] WARN: No server tp found for addr: %x:%x:%x:%x:%x:%x .",
			addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

	return NULL;
}

static BOOL _release_server_tp(TimeSTransport *server_tp) {
	U8 i;

	if (server_tp == NULL) {
		return FALSE;
	}

	for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
		if (_times_used_tps[i] == server_tp) {
			_times_used_tps[i] = NULL;
			InsertTailList(&_times_tp_list, &server_tp->node);
			return TRUE;
		}
	}
	OS_Report("[Time] Error: %s() server_tp(0x%x) is not in used_list.", __FUNCTION__, server_tp);
	return FALSE;
}

TimePacket* _get_server_pkt(void *pChnl) {
	TimePacket *pkt = NULL;

	if (!IsListEmpty(&_times_packet_list)) {
		pkt = (TimePacket *) RemoveHeadList(&_times_packet_list);
		pkt->chnl = pChnl;
		btmtk_os_memset((U8 *) &pkt->gatt_params, 0, sizeof(pkt->gatt_params));
	}

	if (pkt == NULL) {
		OS_Report("[Time] No available server packet.");
	}

	return pkt;
}

void _release_server_pkt(TimePacket *pkt) {
	if (pkt == NULL) {
		OS_Report("[Time] Warning: pkt is NULL");
		return;
	}
	InsertTailList(&_times_packet_list, &pkt->node);
}

/*********************************************************************************************
 *  Callback functions
 *********************************************************************************************/
void times_gatt_connect_timeout_callback(EvmTimer *Timer) {
	TimeSTransport *pChnl;
	TimeSContext *pCntx;
	CmgrHandler *handler;

	pCntx = ContainingRecord(Timer, TimeSContext, times_gatt_connect_timer);

	OS_Report("[Time] %s()", __FUNCTION__);

	if (pCntx != NULL) {
		pChnl = &pCntx->chnl;
		handler = &pChnl->handler;

		CMGR_RemoveDataLink(handler);
	}
}

void times_cmgr_callback(CmgrHandler *handler, CmgrEvent event, BtStatus status) {
	TimeSTransport *pChnl;
	TimeSContext *pCntx;

	pChnl = ContainingRecord(handler, TimeSTransport, handler);
	pCntx = ContainingRecord(pChnl, TimeSContext, chnl);

	OS_Report("[Time] times_cmgr_callback, event: %d, status: %d", event, status);

	switch (event) {
		case CMEVENT_DATA_LINK_DIS:
			if (pChnl != NULL && btmtk_os_memcmp(pChnl->bdaddr.addr, 6, handler->remDev->bdAddr.addr, 6)) {
				times_notify_disconnected(pCntx);
			}
			break;

		case CMEVENT_DATA_LINK_CON_CNF:
			if (status == BT_STATUS_SUCCESS) {
				if (pChnl != NULL && btmtk_os_memcmp(pChnl->bdaddr.addr, 6, handler->remDev->bdAddr.addr, 6)) {
					// Start a timer to wait for gatt connection
					EVM_ResetTimer(&pCntx->times_gatt_connect_timer);
					time_gatt_register_server_callback();
					pCntx->gatt_cb_registered = TRUE;
					pCntx->times_gatt_connect_timer.func = times_gatt_connect_timeout_callback;
					EVM_StartTimer(&pCntx->times_gatt_connect_timer, 1000);
				}
			} else {
				CMGR_RemoveDataLink(handler);
				times_notify_disconnected(pCntx);
			}
			break;

		case CMEVENT_DATA_LINK_CON_IND:
			// For incoming connection
			break;

		default:
			break;
	}
}

/*********************************************************************************************
 * Functions called from internal adaptation layer
 *********************************************************************************************/
/* @brief invoke by system before using time server */
BT_BOOL TIMES_Init() {
	U8 i = 0;
	TimeSContext *pCntx;

	OS_Report("[Time] %s()", __FUNCTION__);

	InitializeListHead(&_times_tp_list);
	InitializeListHead(&_times_packet_list);

	// Reset the context
	for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
		pCntx = &_times_contexts[i];
		memset((void *) pCntx, 0, sizeof(pCntx));

		_times_packets[i].check_byte = TIME_CHECK_BYTE;
		InsertTailList(&_times_packet_list, &(_times_packets[i].node));
	}

	return TRUE;
}

BtStatus TIMES_Register(TimeSApp *app, TimeSTransport *chnl, BTMTK_TIMES_CALLBACK callback) {
	BtStatus status;
	TimeSContext *pCntx;
	U8 i;

	OS_Report("[Time] %s()", __FUNCTION__);

	if (callback == NULL) {
		OS_Report("[Time] %s callback is NULL", __FUNCTION__);
		return BT_STATUS_FAILED;
	}

	pCntx = _get_server_cntx();
	if (pCntx != NULL) {
		pCntx->state = TIME_SERVER_READY; 
		pCntx->chnl.check_byte = TIME_CHECK_BYTE;
		pCntx->callback = callback;
		pCntx->app = (void*) app;
		app->timesContext = (void*) pCntx;

		InsertTailList(&_times_tp_list, &(pCntx->chnl.node));
		for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
			_times_used_tps[i] = NULL;
		}

		// Initialize fake data for Next DST and Reference Time Update services
		pCntx->date_time[0] = 1900;
		pCntx->date_time[1] = 1;
		pCntx->date_time[2] = 1;
		pCntx->date_time[3] = 0;
		pCntx->date_time[4] = 0;
		pCntx->date_time[5] = 0;
		pCntx->dst = 4;
		time_gatt_write_local_value(pCntx, GATT_TIME_DST_INFO);

		// time_gatt_register_server_callback();
		TimeAttDB_AddRecord();		
		return BT_STATUS_SUCCESS;
	}

	OS_Report("[Time] Server registration failed.");
	return BT_STATUS_FAILED;
}

BtStatus TIMES_Deregister(TimeSApp *app) {
	BtStatus status;
	TimeSContext* pCntx;
	pCntx = (TimeSContext *) app->timesContext;

	OS_Report("[Time] %s()", __FUNCTION__);

	if (pCntx != NULL) {
		switch (pCntx->state) {
			case TIME_SERVER_CONNECTED:
				pCntx->state = TIME_SERVER_DEREGISTERING;
				time_gatt_server_disconnect(&(pCntx->chnl));
				return BT_STATUS_PENDING;
				
			case TIME_SERVER_CONNECTING:
				pCntx->state = TIME_SERVER_DEREGISTERING;
				CMGR_RemoveDataLink(&(pCntx->chnl.handler));
				return BT_STATUS_PENDING;
				
			case TIME_SERVER_READY:
				TimeAttDB_RemoveRecord();
				RemoveEntryList(&(pCntx->chnl.node));
				_release_server_cntx(pCntx);
				app->timesContext = NULL;
				return BT_STATUS_SUCCESS;

			// TODO: Handle deregistering when in other states
			default:
				OS_Report("[Time] Wrong state to deregister server, state: %d.", pCntx->state);
				return BT_STATUS_FAILED;
		}
	}
	return BT_STATUS_FAILED;
}

BtStatus TIMES_AcceptIncoming(TimeSApp *app, BtStatus rspcode) {
	BtStatus status = BT_STATUS_FAILED;
	TimeSContext *pCntx;
	TimeSTransport *pChnl;
	pCntx = (TimeSContext *) app->timesContext;
	pChnl = &(pCntx->chnl);

	OS_Report("[Time] %s()", __FUNCTION__);

	if (pCntx != NULL) {
		if (pCntx->state == TIME_SERVER_CONNECTING) {
			if (rspcode == BT_STATUS_SUCCESS) {
				times_notify_connected(pCntx, pCntx->addr);
			} else {
				time_gatt_server_disconnect(pChnl);
				status = CMGR_RemoveDataLink(&(pChnl->handler));
				if (status == BT_STATUS_SUCCESS) {
					pCntx->state = TIME_SERVER_READY;
				} else {
					pCntx->state = TIME_SERVER_DISCONNECTING;
				}
			}

		} else {
			OS_Report("[Time] Wrong state to accept incoming.");
		}
	}

	return status; 
}

BtStatus TIMES_Connect(TimeSApp *app, U8 addr[6]) {
	BtStatus status = BT_STATUS_FAILED;
	TimeSContext *pCntx;
	TimeSTransport *pChnl;

	OS_Report("[Time] %s()", __FUNCTION__);

	pChnl = _get_server_tp(addr);
	if (pChnl != NULL) {
		pCntx = ContainingRecord(pChnl, TimeSContext, chnl);
		switch (pCntx->state) {
			case TIME_SERVER_READY:
				status = CMGR_RegisterHandler(&(pCntx->chnl.handler), times_cmgr_callback);
				OS_Report("[Time] CMGR_RegisterHandler() returns %d", status);
				if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
					return status;
				}

				pCntx->state = TIME_SERVER_CONNECTING;
				status = CMGR_CreateDataLink(&pChnl->handler, &pChnl->bdaddr);
				// status = CMGR_CreateBLEDataLink(&pChnl->handler, &pChnl->bdaddr);
				OS_Report("[Time] %s(), create data link to %x:%x:%x:%x:%x:%x, status: %d", __FUNCTION__,
						addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], status);
				switch (status) {
					case BT_STATUS_SUCCESS:
						EVM_ResetTimer(&pCntx->times_gatt_connect_timer);
						time_gatt_register_server_callback();
						pCntx->times_gatt_connect_timer.func = times_gatt_connect_timeout_callback;
						EVM_StartTimer(&pCntx->times_gatt_connect_timer, 1000);
						break;

					case BT_STATUS_PENDING:
						// Do nothing
						break;

					default:
						CMGR_DeregisterHandler(&(pCntx->chnl.handler));
						times_notify_disconnected(pCntx);
				}
				break;

			default:
				OS_Report("[Time] Wrong state to initiate connection, state: %d.", pCntx->state);
				break;;
		}
	}

	return status;
}

BtStatus TIMES_Disconnect(TimeSApp *app) {
	BtStatus status = BT_STATUS_SUCCESS;
	TimeSContext *pCntx;
	TimeSTransport *pChnl;
	pCntx = (TimeSContext *) app->timesContext;
	pChnl = &(pCntx->chnl);

	OS_Report("[Time] %s()", __FUNCTION__);

	if (pCntx != NULL) {
		switch (pCntx->state) {
			case TIME_SERVER_CONNECTING:
				OS_Report("[Time] Server disconnects in connecting phase.");
				TIMES_AcceptIncoming(app, BT_STATUS_FAILED);
				break;

			case TIME_SERVER_CONNECTED:
				pCntx->state = TIME_SERVER_DISCONNECTING;
				status = time_gatt_server_disconnect(pChnl);
				if (status == BT_STATUS_SUCCESS) {
					status = CMGR_RemoveDataLink(&(pChnl->handler));
					if (status == BT_STATUS_SUCCESS) {
						times_notify_disconnected(pCntx);
					}
				}
				break;

			default:
				break;
		}
	}

	return status;
}

BtStatus TIMES_Set_Local_Info(TimeSApp *app, U8 value_id) {
	BtStatus status = BT_STATUS_FAILED;
	TimeSContext *pCntx = (TimeSContext *) app->timesContext;

	if (pCntx != NULL) {
		status = time_gatt_write_local_value(pCntx, value_id);
	}
	return status;
}

BtStatus TIMES_Notify_Value(TimeSApp *app, U8 value_id) {
	BtStatus status = BT_STATUS_FAILED;
	TimeSContext *pCntx = (TimeSContext *) app->timesContext;
	TimePacket *pkt;

	OS_Report("[Time] %s()", __FUNCTION__);
	if (pCntx == NULL || pCntx->state != TIME_SERVER_CONNECTED) {
		return status;
	}

	pkt = _get_server_pkt(&pCntx->chnl);
	if (pkt != NULL) {
		// seq = 1. We have only one packet for now.
		pCntx->chnl.pkt = pkt;
		status = time_gatt_notify_value(pkt, 1, value_id);

		// Basically, the status value could only be BT_STATUS_PENDING
		if (status != BT_STATUS_PENDING) {
			_release_server_pkt(pkt);
			pCntx->chnl.pkt = NULL;
		}
	}

	return status;
}

/*********************************************************************************************
 * Notify functions. To internal and external adaptation layers.
 *********************************************************************************************/
/* Notify internal adaption layer for sending ILM to MMI */
void times_notify(TimeSContext *pCntx, U8 event_id, void *data) {
	TimeSEvent event;
	if (pCntx->callback) {
		event.event_id = event_id;
		event.data = data;
		pCntx->callback(&event, pCntx->app);

	} else {
		OS_Report("[Time] Null server callback. Force disconnect.");

		if (pCntx->state != TIME_SERVER_DISCONNECTING && pCntx->state != TIME_SERVER_IDLE) {
			TIMES_Disconnect(pCntx->app);
		}
	}
}

void times_notify_incoming(BtRemoteDevice *link) {
	TimeSTransport *pChnl;
	TimeSContext *pCntx;

	OS_Report("[Time] %s()", __FUNCTION__);

	pChnl = _get_server_tp(link->bdAddr.addr);
	if (pChnl != NULL) {
		CMGR_CreateDataLink(&pChnl->handler, &link->bdAddr);
		btmtk_os_memcpy(pChnl->bdaddr.addr, link->bdAddr.addr, sizeof(link->bdAddr));

		pCntx = ContainingRecord(pChnl, TimeSContext, chnl);

		if (pCntx->state == TIME_SERVER_READY) {
			pCntx->state = TIME_SERVER_CONNECTING;
			memcpy(pCntx->addr, link->bdAddr.addr, 6);
			times_notify(pCntx, TIMES_EVENT_INCOMING, pCntx->addr);
		}
	} else {
		// For now, reject directly.
		OS_Report("[Time] %s(): ERROR, no channel is found.", __FUNCTION__);
	}
}

void times_notify_connected(TimeSContext *pCntx, U8 *addr) {
	OS_Report("[Time] %s()", __FUNCTION__);
	if (pCntx->state == TIME_SERVER_CONNECTING) {
		pCntx->state = TIME_SERVER_CONNECTED;
		if (addr != NULL) {
			memcpy(pCntx->addr, addr, 6);
		} else {
			memset(pCntx->addr, 0, 6);
		}
		times_notify(pCntx, TIMES_EVENT_CONNECTED, pCntx->addr);

	} else {
		OS_Report("[Time] Notify connected in invalid state: %d", pCntx->state);
	}
}

void times_notify_disconnected(TimeSContext *pCntx) {
	OS_Report("[Time] %s()", __FUNCTION__);
	TimeSTransport *pChnl = &pCntx->chnl;

	if (pCntx->gatt_cb_registered == TRUE) {
		time_gatt_deregister_server_callback();
		pCntx->gatt_cb_registered = FALSE;
	}

	CMGR_DeregisterHandler(&pChnl->handler);
	_release_server_tp(pChnl);

	switch (pCntx->state) {
		case TIME_SERVER_CONNECTED:
		case TIME_SERVER_CONNECTING:
		case TIME_SERVER_DISCONNECTING:
			pCntx->state = TIME_SERVER_READY;
			times_notify(pCntx, TIMES_EVENT_DISCONNECTED, NULL);
			break;

		case TIME_SERVER_DEREGISTERING:
			pCntx->state = TIME_SERVER_IDLE;
			TimeAttDB_RemoveRecord();
			RemoveEntryList(&(pCntx->chnl.node));
			((TimeSApp *) pCntx->app)->timesContext = NULL;
			_release_server_cntx(pCntx);
			times_notify(pCntx, TIMES_EVENT_DEREGISTER, NULL);
			break;

		default:
			break;
	}
}

void times_notify_gatt_connected(BtRemoteDevice *link) {
	TimeSTransport *pChnl;
	TimeSContext *pCntx;

	OS_Report("[Time] %s()", __FUNCTION__);

	pChnl = times_find_server_tp(link->bdAddr.addr);
	if (pChnl == NULL) {
		OS_Report("[Time] %s() WARNING: No server transport", __FUNCTION__);
		return;
	}

	// The data link has been established, we are waiting for GATT connected
	pCntx = ContainingRecord(pChnl, TimeSContext, chnl);
	if (pChnl == NULL) {
		OS_Report("[Time] %s() WARNING: No server context", __FUNCTION__);
		return;
	}

	if (pCntx->state == TIME_SERVER_CONNECTING) {
		EVM_CancelTimer(&pCntx->times_gatt_connect_timer);
		pCntx->state = TIME_SERVER_CONNECTED;
		times_notify(pCntx, TIMES_EVENT_CONNECTED, pChnl->bdaddr.addr);
	} else {
		OS_Report("[Time] %s() WARNING: Wrong state %d", __FUNCTION__, pCntx->state);
	}
}

void times_notify_gatt_disconnected(BtRemoteDevice *link) {
	OS_Report("[Time] %s()", __FUNCTION__);
	TimeSTransport *pChnl;
	TimeSContext *pCntx;

	pChnl = times_find_server_tp(link->bdAddr.addr);
	if (pChnl != NULL) {
		CMGR_RemoveDataLink(&pChnl->handler);

	} else {
		OS_Report("[Time] %s(): ERROR, no channel is found.", __FUNCTION__);
	}
}

void times_notify_notify_value_result(TimeSContext *pCntx) {
	OS_Report("[Time] %s()", __FUNCTION__);

	_release_server_pkt(pCntx->chnl.pkt);
	pCntx->chnl.pkt = NULL;
	
	times_notify(pCntx, TIMES_EVENT_UPDATE_CTTIME, NULL);
}

void times_notify_request_server_update(TimeSContext *pCntx) {
	OS_Report("[Time] %s()", __FUNCTION__);
	times_notify(pCntx, TIMES_EVENT_REQUEST_UPDATE, NULL);
}

void times_notify_cancel_server_update(TimeSContext *pCntx) {
	OS_Report("[Time] %s()", __FUNCTION__);
	times_notify(pCntx, TIMES_EVENT_CANCEL_UPDATE, NULL);
}

#endif
