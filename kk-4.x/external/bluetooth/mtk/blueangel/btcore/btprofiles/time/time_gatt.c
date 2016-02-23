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
 * time_gatt.c
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
#if defined(__BT_TIMEC_PROFILE__) || defined(__BT_TIMES_PROFILE__)

#include "bttypes.h"
#include "bt_os_api.h"
#include "att.h"
#include "attdb.h"
#include "gattcb.h"
#include "sdap.h"
#include "utils.h"  // for listentry

#include "bluetooth_time_struct.h"
#include "time_gatt.h"
#include "timec.h"
#include "times.h"

/********************************************************************************************
 * Internal variables
 ********************************************************************************************/
#ifdef __BT_TIMEC_PROFILE__
static TimeCTransport *_cur_timec_chnl;
#endif

#ifdef __BT_TIMES_PROFILE__
static TimeServerSharedInfo _time_server_info;
#endif

static BtGattHandler _times_gatt_handler;
static BtGattHandler _timec_gatt_handler;

/********************************************************************************************
  * External functions
  ********************************************************************************************/
// For adding or removing SDP record in BR/EDR
extern U8 TimeSdpDB_AddRecord(U8 idx, U8 cap);
extern U8 TimeSdpDB_RemoveRecord(U8 idx, U8 cap);

// For adding or removing GATT service record
extern U8 TimeAttDB_AddRecord();
extern U8 TimeAttDB_RemoveRecord();

// GATT functions (gatt_util.c)
extern void BT_GattRegisterConnectedEvent(BtGattHandler *hdl, GattAPCallBack cb);
extern void BT_GattUnregisterConnectedEvent(BtGattHandler *hdl);
extern BtStatus GattClientConnect(BtRemoteDevice *link);
extern BtStatus GattDisconnect(BtRemoteDevice *link);
extern BtStatus BT_GATT_DiscoveryPrimaryServiceByUUID(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_DiscoveryCharacteristicByUUID(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_DiscoveryAllCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_ReadCharacteristicValue(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_ReadCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_WriteWithoutResponse(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_WriteCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_Notifications(BtRemoteDevice *link, BtGattOp *op);

/********************************************************************************************
 * Function declarations
 ********************************************************************************************/
#ifdef __BT_TIMEC_PROFILE__
BtStatus time_gatt_connect_service(TimeCTransport *pChnl);
BtStatus time_gatt_destroy_link(TimeCTransport *pChnl);
BtStatus time_gatt_disconnect_service(TimeCTransport *pChnl);
BtStatus time_gatt_read_remote_value(TimePacket *pkt, U8 seq_num, U32 value_id);
BtStatus time_gatt_write_remote_value(TimePacket *pkt, U8 seq_num, U32 value_id, U16 value);
#endif

#ifdef __BT_TIMES_PROFILE__
BtStatus time_gatt_server_disconnect(TimeSTransport *pChnl);
BtStatus time_gatt_write_local_value(TimeSContext *pCntx, U8 value_id);
BtStatus time_gatt_notify_value(TimePacket *pkt, U8 seq_num, U32 value_id);
extern TimeSTransport* times_find_server_tp(U8 *addr);
#endif

/********************************************************************************************
 * Internal functions
 ********************************************************************************************/
#ifdef __BT_TIMEC_PROFILE__
static BtStatus _time_gatt_discovery_primary_service_by_uuid(
		TimePacket *pkt, U16 uuid, U16 start, U16 end);
static BtStatus _time_gatt_discovery_characteristic_by_uuid(
		TimePacket *pkt, U16 uuid, U16 start, U16 end);
static BtStatus _time_gatt_read_characteristic_value(
		TimePacket *pkt, U16 char_handle);
static BtStatus _time_gatt_write_without_response(
		TimePacket *pkt, U16 char_handle);
static BtStatus _time_gatt_discovery_characteristic_descriptors(
		TimePacket *pkt, U16 start, U16 end);
static BtStatus _time_gatt_read_characteristic_descriptor(
		TimePacket *pkt, U16 desc_handle);
static BtStatus _time_gatt_write_characteristic_descriptor(
		TimePacket *pkt, U16 desc_handle);

static BtStatus _time_access_service_handle_cache(
		TimeCTransport *pChnl, U8 value_id, U8 write, U16 *start_handle, U16 *end_handle) {
	U8 start_idx = 0, end_idx = 0;

	switch (value_id) {
		case GATT_TIME_CTTIME:
		case GATT_TIME_CTTIME_NOTIFY:
		case GATT_TIME_LOCAL_TIME_INFO:
		case GATT_TIME_REF_TIME_INFO:
			start_idx = CURRENT_TIME_SERVICE_START;
			end_idx = CURRENT_TIME_SERVICE_END;
			break;

		case GATT_TIME_DST_INFO:
			start_idx = NEXT_DST_SERVICE_START;
			end_idx = NEXT_DST_SERVICE_END;
			break;

		case GATT_TIME_SERVER_UPDATE_CTRL:
		case GATT_TIME_SERVER_UPDATE_STATE:
			start_idx = REF_TIME_UPDATE_SERVICE_START;
			end_idx = REF_TIME_UPDATE_SERVICE_END;
			break;
		default:
			return BT_STATUS_FAILED;
	}

	if (write == 1) {
		pChnl->cached_handles[start_idx] = *start_handle;
		pChnl->cached_handles[end_idx] = *end_handle;
	} else {
		*start_handle = pChnl->cached_handles[start_idx];
		*end_handle = pChnl->cached_handles[end_idx];
	}

	return BT_STATUS_SUCCESS;
}

static BtStatus _time_access_value_handle_cache(
		TimeCTransport *pChnl, U8 value_id, U8 write, U16 *value_handle) {
	U8 idx = 0;

	switch (value_id) {
		case GATT_TIME_CTTIME:
			idx = CT_TIME;
			break;
		case GATT_TIME_CTTIME_NOTIFY:
			idx = CT_TIME_CHAR_CONFIG;
			break;
		case GATT_TIME_LOCAL_TIME_INFO:
			idx = LOCAL_TIME_INFO;
			break;
		case GATT_TIME_REF_TIME_INFO:
			idx = REF_TIME_INFO;
			break;
		case GATT_TIME_DST_INFO:
			idx = TIME_WITH_DST;
			break;
		case GATT_TIME_SERVER_UPDATE_CTRL:
			idx = TIME_UPDATE_CTRL_POINT;
			break;
		case GATT_TIME_SERVER_UPDATE_STATE:
			idx = TIME_UPDATE_STATE;
			break;
		default:
			return BT_STATUS_FAILED;
	}

	if (write == 1) {
		pChnl->cached_handles[idx] = *value_handle;
	} else {
		*value_handle = pChnl->cached_handles[idx];
	}

	return BT_STATUS_SUCCESS;
}
#endif

/********************************************************************************************
  * Interface Functions
  ********************************************************************************************/
#ifdef __BT_TIMEC_PROFILE__
static void timec_gatt_connection_callback(void *handler, BtGattEvent *event) {
	TimeCContext *pCntx;

	switch (event->eType) {
		case BT_GATT_APP_EVENT_CONNECTED:
			OS_Report("[Time] BT_GATT_APP_EVENT_CONNECTED");
	
			if (event->link && _cur_timec_chnl) {
				pCntx = ContainingRecord(_cur_timec_chnl, TimeCContext, chnl);
				timec_notify_connect_result(pCntx, BT_STATUS_SUCCESS);
			} else {
				OS_Report("[Time] Ignore. event->link: 0x%x, _cur_timec_chnl: 0x%x",
						event->link, _cur_timec_chnl);
			}
			break;

		case BT_GATT_APP_EVENT_DISCONNECTED:
			OS_Report("[Time] BT_GATT_APP_EVENT_DISCONNECTED");
			if (event->link && _cur_timec_chnl) {
				pCntx = ContainingRecord(_cur_timec_chnl, TimeCContext, chnl);
				timec_notify_disconnect_result(pCntx, BT_STATUS_SUCCESS);
				_cur_timec_chnl = NULL;
			} else {
				OS_Report("[Time] Ignore. event->link: 0x%x, _cur_timec_chnl: 0x%x", event->link);
			}
			break;
	
		case BT_GATT_APP_EVENT_VALUE_NOTIFICATION:
			OS_Report("[Time] BT_GATT_APP_EVENT_VALUE_NOTIFICATION");
			if (event->link && _cur_timec_chnl) {
				BtGattValueNotificationEvent *ptr = (BtGattValueNotificationEvent *) event->parms;
				// TODO: Compare the handle value with the cached one
				if (ptr->attributeHandle != ATT_HANDLE_CHARACTERISTIC_CT_TIME_VALUE) {
					OS_Report("[Time] Unrecognized handle: %d, ignore", ptr->attributeHandle);
					return;
				}

				if (ptr->attributeValue && ptr->attributeValueLen > 9) {
					pCntx = ContainingRecord(_cur_timec_chnl, TimeCContext, chnl);
					pCntx->date_time[0] = (0x00ff & ptr->attributeValue[0]) | (0xff00 & (ptr->attributeValue[1] << 8));
					pCntx->date_time[1] = (U16) ptr->attributeValue[2];
					pCntx->date_time[2] = (U16) ptr->attributeValue[3];
					pCntx->date_time[3] = (U16) ptr->attributeValue[4];
					pCntx->date_time[4] = (U16) ptr->attributeValue[5];
					pCntx->date_time[5] = (U16) ptr->attributeValue[6];
					pCntx->day_of_week = ptr->attributeValue[7];
					pCntx->frac = ptr->attributeValue[8];
					pCntx->adj_reason = ptr->attributeValue[9];
					timec_notify_server_notification(pCntx, GATT_TIME_CTTIME);
				}
			} else {
				OS_Report("[Time] Ignore. event->link: 0x%x, _cur_timec_chnl: 0x%x", event->link);
			}
			break;

		case BT_GATT_APP_EVENT_VALUE_INDICATION:
			OS_Report("[Time] BT_GATT_APP_EVENT_VALUE_INDICATION");
			break;

		default:
			break;
	}
}

static void _time_gatt_notify_query_result(TimePacket *pkt, U8 *data, U8 len, U8 result) {
	TimeCContext *pCntx;

	if (pkt == NULL) {
		OS_Report("[Time] Error: %s with NULL packet", __FUNCTION__);
		return;
	}

	if (pkt->chnl == NULL) {
		OS_Report("[Time] Error: %s with NULL channel", __FUNCTION__);
		return;
	}

	pCntx = ContainingRecord(pkt->chnl, TimeCContext, chnl);

	switch (pkt->value_id) {
		case GATT_TIME_CTTIME:
			if (result == BT_STATUS_SUCCESS && len > 9) {
				pCntx->date_time[0] = (0x00ff & data[0]) | (0xff00 & (data[1] << 8));
				pCntx->date_time[1] = (U16) data[2];
				pCntx->date_time[2] = (U16) data[3];
				pCntx->date_time[3] = (U16) data[4];
				pCntx->date_time[4] = (U16) data[5];
				pCntx->date_time[5] = (U16) data[6];
				pCntx->day_of_week = data[7];
				pCntx->frac = data[8];
				pCntx->adj_reason = data[9];
				timec_notify_read_remote_value_result(pCntx, result, pkt);
			} else {
				timec_notify_read_remote_value_result(pCntx, BT_STATUS_FAILED, pkt);
			}
			break;

		case GATT_TIME_CTTIME_NOTIFY:
			switch (pkt->type) {
				case TIME_PKT_READ:
					if (result == BT_STATUS_SUCCESS && len > 1) {
						pCntx->cttime_notify_config = (0x00ff & data[0]) | (0xff00 & (data[1] << 8));
						timec_notify_read_remote_value_result(pCntx, result, pkt);
					} else {
						timec_notify_read_remote_value_result(pCntx, BT_STATUS_FAILED, pkt);
					}
					break;

				case TIME_PKT_WRITE:
					timec_notify_write_remote_value_result(pCntx, result, pkt);
					break;
			}
			break;

		case GATT_TIME_LOCAL_TIME_INFO:
			if (result == BT_STATUS_SUCCESS && len > 1) {
				pCntx->time_zone = data[0];
				pCntx->dst = data[1];
				timec_notify_read_remote_value_result(pCntx, result, pkt);
			} else {
				timec_notify_read_remote_value_result(pCntx, BT_STATUS_FAILED, pkt);
			}
			break;

		case GATT_TIME_REF_TIME_INFO:
			if (result == BT_STATUS_SUCCESS && len > 3) {
				pCntx->time_source = data[0];
				pCntx->accuracy = data[1];
				pCntx->days_since_update = data[2];
				pCntx->hours_since_update = data[3];
				timec_notify_read_remote_value_result(pCntx, result, pkt);
			} else {
				timec_notify_read_remote_value_result(pCntx, BT_STATUS_FAILED, pkt);
			}
			break;

		case GATT_TIME_DST_INFO:
			if (result == BT_STATUS_SUCCESS && len > 7) {
				pCntx->date_time[0] = (0x00ff & data[0]) | (0xff00 & (data[1] << 8));
				pCntx->date_time[1] = (U16) data[2];
				pCntx->date_time[2] = (U16) data[3];
				pCntx->date_time[3] = (U16) data[4];
				pCntx->date_time[4] = (U16) data[5];
				pCntx->date_time[5] = (U16) data[6];
				pCntx->dst = data[7];
				timec_notify_read_remote_value_result(pCntx, result, pkt);
			} else {
				timec_notify_read_remote_value_result(pCntx, BT_STATUS_FAILED, pkt);
			}
			break;

		case GATT_TIME_SERVER_UPDATE_CTRL:
			timec_notify_write_remote_value_result(pCntx, result, pkt);
			break;

		case GATT_TIME_SERVER_UPDATE_STATE:
			if (result == BT_STATUS_SUCCESS && len > 1) {
				timec_notify_read_remote_value_result(pCntx, result, pkt);
			} else {
				timec_notify_read_remote_value_result(pCntx, BT_STATUS_FAILED, pkt);
			}
			break;
	}
}

static void _time_gatt_discovery_primary_service_by_uuid_cb(BtGattEvent *event) {
	BtStatus status;
	BtGattDiscoveryAllPrimaryServiceResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;
	U16 uuid = 0;

	ptr = (BtGattDiscoveryAllPrimaryServiceResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
			// There should be only one instance of any service of Time profile
			OS_Report("[Time] Primary Service found. type: %d, uuid: 0x%04x", ptr->type, ptr->uuid2);
			OS_Report("[Time] Handle reange: 0x%04x to 0x%04x", ptr->startHandle, ptr->endHandle);
			pkt->start_handle = ptr->startHandle;
			pkt->end_handle = ptr->endHandle;
			pkt->success_flag = 1;
			break;

		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Discovery Primary Service by UUID with result DONE_COMPLETED. success: %d",
					pkt->success_flag);
			if (pkt->success_flag == 1) {
				 _time_access_service_handle_cache(pChnl, pkt->value_id, 1, &pkt->start_handle, &pkt->end_handle);
				switch (pkt->value_id) {
					case GATT_TIME_CTTIME:
					case GATT_TIME_CTTIME_NOTIFY:
						uuid = BT_UUID_CT_TIME;
						break;
					case GATT_TIME_LOCAL_TIME_INFO:
						uuid = BT_UUID_LOCAL_TIME_INFO;
						break;
					case GATT_TIME_REF_TIME_INFO:
						uuid = BT_UUID_REF_TIME_INFO;
						break;
					case GATT_TIME_DST_INFO:
						uuid = BT_UUID_TIME_WITH_DST;
						break;
					case GATT_TIME_SERVER_UPDATE_CTRL:
						uuid = BT_UUID_TIME_UPDATE_CONTROL_POINT;
						break;
					case GATT_TIME_SERVER_UPDATE_STATE:
						uuid = BT_UUID_TIME_UPDATE_STATE;
						break;
					default:
						break;
				}

				status = _time_gatt_discovery_characteristic_by_uuid(
						pkt, uuid, pkt->start_handle, pkt->end_handle);
				OS_Report("[Time] Discovery Characteristic, status: %d", status);

				if (status != BT_STATUS_PENDING && status != BT_STATUS_SUCCESS)
					_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			} else {
				_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			}
			break;

		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}
}

static void _time_gatt_discovery_characteristic_by_uuid_cb(BtGattEvent *event) {
	BtStatus status = BT_STATUS_FAILED;
	BtGattDiscoveryCharacteristicByUUIDResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;
	U16 dsp_handle;

	ptr = (BtGattDiscoveryCharacteristicByUUIDResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
			OS_Report("[Time] Characteristic found. type: %d, uuid: 0x%04x", ptr->type, ptr->charUUID);
			OS_Report("[Time] Char handle: 0x%04x, property: 0x%02x ", ptr->attributeHandle, ptr->charProperty);
			OS_Report("[Time] Char value handle: 0x%04x", ptr->charValueAttributeHandle);
			pkt->start_handle = ptr->charValueAttributeHandle;
			pkt->success_flag = 1;
			break;

		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Discovery Characteristic by UUID with result DONE_COMPLETED. success: %d",
					pkt->success_flag);
			if (pkt->success_flag == 1) {
				switch (pkt->value_id) {
					case GATT_TIME_CTTIME:
						dsp_handle = pkt->start_handle + 1;
						_time_access_value_handle_cache(pChnl, pkt->value_id, 1, &pkt->start_handle);
						_time_access_value_handle_cache(pChnl, GATT_TIME_CTTIME_NOTIFY, 1, &dsp_handle);
						status = _time_gatt_read_characteristic_value(pkt, pkt->start_handle);
						break;

					case GATT_TIME_CTTIME_NOTIFY:
						dsp_handle = pkt->start_handle + 1;
						_time_access_value_handle_cache(pChnl, GATT_TIME_CTTIME, 1, &pkt->start_handle);
						_time_access_value_handle_cache(pChnl, pkt->value_id, 1, &dsp_handle);
						if (pkt->type == TIME_PKT_READ) {
							status = _time_gatt_read_characteristic_descriptor(pkt, pkt->start_handle + 1);
						} else { // TIME_PKT_WRITE
							status = _time_gatt_write_characteristic_descriptor(pkt, pkt->start_handle + 1);
						}
						break;

					case GATT_TIME_LOCAL_TIME_INFO:
					case GATT_TIME_REF_TIME_INFO:
					case GATT_TIME_DST_INFO:
					case GATT_TIME_SERVER_UPDATE_STATE:
						_time_access_value_handle_cache(pChnl, pkt->value_id, 1, &pkt->start_handle);
						status = _time_gatt_read_characteristic_value(pkt, pkt->start_handle);
						break;

					case GATT_TIME_SERVER_UPDATE_CTRL:
						_time_access_value_handle_cache(pChnl, pkt->value_id, 1, &pkt->start_handle);
						status = _time_gatt_write_without_response(pkt, pkt->start_handle);
						break;

					default:
						break;
				}
				
				if (status != BT_STATUS_PENDING && status != BT_STATUS_SUCCESS)
					_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			} else {
				_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			}
			break;

		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}
}

static void _time_gatt_read_characteristic_value_cb(BtGattEvent *event) {
	// To be modified.
	BtStatus status;
	BtGattReadCharacteristicValueResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;
	U16 uuid = 0;

	ptr = (BtGattReadCharacteristicValueResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Read Characteristic Value with result DONE_COMPLETED");
			_time_gatt_notify_query_result(pkt, ptr->data, ptr->length, BT_STATUS_SUCCESS);
			break;
		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}
}

static void _time_gatt_write_without_response_cb(BtGattEvent *event) {
	BtStatus status;
	BtGattWriteWithoutResponseResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;
	U16 uuid = 0;

	ptr = (BtGattWriteWithoutResponseResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Write withou Response with result DONE_COMPLETED");
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_SUCCESS);
			break;
		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}

}

static void _time_gatt_discovery_characteristic_descriptors_cb(BtGattEvent *event) {
	BtStatus status = BT_STATUS_FAILED;
	BtGattDiscoveryAllCharacteristicDescriptorsResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;
	U16 uuid = 0;

	ptr = (BtGattDiscoveryAllCharacteristicDescriptorsResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
			OS_Report("[Time] Characteristic Descriptor found. type: %d, uuid: 0x%04x", ptr->type, ptr->uuid16);
			OS_Report("[Time] Char desc value handle: 0x%04x", ptr->attributeHandle);
			pkt->start_handle = ptr->attributeHandle;
			pkt->success_flag = 1;
			break;

		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Discovery Characteristic Descriptor with result DONE_COMPLETED");
			if (pkt->success_flag != 1) {
				_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
				return;
			}

			_time_access_value_handle_cache(pChnl, pkt->value_id, 1, &pkt->start_handle);
			if (pkt->value_id == GATT_TIME_CTTIME_NOTIFY) {
				switch (pkt->type) {
					case TIME_PKT_READ:
						status = _time_gatt_read_characteristic_descriptor(pkt, pkt->start_handle);
						break;
					case TIME_PKT_WRITE:
						status = _time_gatt_write_characteristic_descriptor(pkt, pkt->start_handle);
						break;
				}

				if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
					_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
				}
			} else {
				_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			}
			break;

		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}
}

static void _time_gatt_read_characteristic_descriptor_cb(BtGattEvent *event) {
	BtStatus status;
	BtGattReadCharacteristicDescriptorsResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;
	U16 uuid = 0;

	ptr = (BtGattReadCharacteristicDescriptorsResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Read Characteristic Descriptor with result DONE_COMPLETED");
			_time_gatt_notify_query_result(pkt, ptr->data, ptr->length, BT_STATUS_SUCCESS);
			break;
		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}

}

static void _time_gatt_write_characteristic_descriptor_cb(BtGattEvent *event) {
	BtStatus status;
	BtGattWriteCharDescriptorResultEvent *ptr;
	TimePacket *pkt;
	TimeCTransport *pChnl;
	U16 uuid = 0;

	ptr = (BtGattWriteCharDescriptorResultEvent *) event->parms;
	pkt = ContainingRecord(event->pGattOp, TimePacket, gatt_op);
	pChnl = (TimeCTransport *) pkt->chnl;

	switch (ptr->flag) {
		case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
			OS_Report("[Time] Write Characteristic Descriptor with result DONE_COMPLETED");
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_SUCCESS);
			break;
		case BT_GATT_EVENT_RESULT_ERROR:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
		case BT_GATT_EVENT_RESULT_DISCONNECT:
			_time_gatt_notify_query_result(pkt, 0, 0, BT_STATUS_FAILED);
			break;
	}

}

void time_gatt_ap_callback(void *user_data, BtGattEvent *event) {
	OS_Report("[Time] time_gatt_ap_callback(), event: %d", event->eType);
	event->pGattOp = user_data;

	switch (event->eType) {
		case BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID:
			_time_gatt_discovery_primary_service_by_uuid_cb(event);
			break;
		case BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID:
			_time_gatt_discovery_characteristic_by_uuid_cb(event);
			break;
		case BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_DESCRIPTOR:
			_time_gatt_discovery_characteristic_descriptors_cb(event);
			break;
		case BT_GATT_OPERATOR_READ_CHAR_VALUE:
			_time_gatt_read_characteristic_value_cb(event);
			break;
		case BT_GATT_OPERATOR_WRITE_WITHOUT_RESPONSE:
			_time_gatt_write_without_response_cb(event);
			break;
		case BT_GATT_OPERATOR_READ_CHAR_DESCRIPTORS:
			_time_gatt_read_characteristic_descriptor_cb(event);
			break;
		case BT_GATT_OPERATOR_WRITE_CHAR_DESCRIPTORS:
			_time_gatt_write_characteristic_descriptor_cb(event);
			break;
	}
}

static BtStatus _time_gatt_discovery_primary_service_by_uuid(
		TimePacket *pkt, U16 uuid, U16 start, U16 end) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;

	OS_Report("[Time] %s(), uuid: %x", __FUNCTION__, uuid);

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(BtGattOp));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.dps_uuid.startHandle = start;
	pkt->gatt_params.dps_uuid.endHandle = end;
	pkt->gatt_params.dps_uuid.type = 0x02;
	pkt->gatt_params.dps_uuid.uuid16[0] = uuid & 0xff;
	pkt->gatt_params.dps_uuid.uuid16[1] = uuid >> 8;
	gatt_op->parms.p.primaryServiceByUUID = &pkt->gatt_params.dps_uuid;

	return BT_GATT_DiscoveryPrimaryServiceByUUID(pChnl->handler.remDev, gatt_op);
}

static BtStatus _time_gatt_discovery_characteristic_by_uuid(
		TimePacket *pkt, U16 uuid, U16 start, U16 end) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;

	OS_Report("[Time] %s(), uuid: %x, handle range: %d to %d", __FUNCTION__, uuid, start, end);

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(BtGattOp));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.dch_uuid.startHandle = start;
	pkt->gatt_params.dch_uuid.endHandle = end;
	pkt->gatt_params.dch_uuid.type = 0x02;
	pkt->gatt_params.dch_uuid.uuid16[0] = uuid & 0xff;
	pkt->gatt_params.dch_uuid.uuid16[1] = uuid >> 8;
	gatt_op->parms.p.charByUUID = &pkt->gatt_params.dch_uuid;

	return BT_GATT_DiscoveryCharacteristicByUUID(pChnl->handler.remDev, gatt_op);
}

static BtStatus _time_gatt_read_characteristic_value(TimePacket *pkt, U16 char_handle) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;

	OS_Report("[Time] %s(), handle: %d", __FUNCTION__, char_handle);

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(BtGattOp));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.rch.handle = char_handle;
	gatt_op->parms.p.readCharValue = &pkt->gatt_params.rch;

	return BT_GATT_ReadCharacteristicValue(pChnl->handler.remDev, gatt_op);
}

BtStatus _time_gatt_write_without_response(TimePacket *pkt, U16 char_handle) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;
	U8 value = (U8) pkt->value;

	OS_Report("[Time] %s(), handle: %d", __FUNCTION__, char_handle);

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(BtGattOp));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.wch_wo_rsp.handle = char_handle;
	pkt->gatt_params.wch_wo_rsp.size = 1;
	pkt->gatt_params.wch_wo_rsp.data = &value;
	gatt_op->parms.p.writeWithoutResponse = &pkt->gatt_params.wch_wo_rsp;

	return BT_GATT_WriteWithoutResponse(pChnl->handler.remDev, gatt_op);
}

static BtStatus _time_gatt_discovery_characteristic_descriptors(
		TimePacket *pkt, U16 start, U16 end) {

	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;

	OS_Report("[Time] %s(), handle range: %d to %d", __FUNCTION__, start, end);

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(BtGattOp));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.dds.startHandle = start;
	pkt->gatt_params.dds.endHandle = end;
	gatt_op->parms.p.allCharDescriptors= &pkt->gatt_params.dds;

	return BT_GATT_DiscoveryAllCharacteristicDescriptors(pChnl->handler.remDev, gatt_op);
}

static BtStatus _time_gatt_read_characteristic_descriptor(TimePacket *pkt, U16 desc_handle) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;

	OS_Report("[Time] %s(), handle: %d", __FUNCTION__, desc_handle);

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(BtGattOp));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.rds.handle = desc_handle;
	gatt_op->parms.p.readCharDescriptors = &pkt->gatt_params.rds;

	return BT_GATT_ReadCharacteristicDescriptors(pChnl->handler.remDev, gatt_op);
}

static BtStatus _time_gatt_write_characteristic_descriptor(TimePacket *pkt, U16 desc_handle) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeCTransport *pChnl = (TimeCTransport *) pkt->chnl;

	OS_Report("[Time] %s(), handle: %d", __FUNCTION__, desc_handle);

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(BtGattOp));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_ap_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.wds.handle = desc_handle;
	pkt->gatt_params.wds.size = 2;
	pkt->gatt_params.wds.data = (U8 *) &pkt->value;
	gatt_op->parms.p.writeDescriptors = &pkt->gatt_params.wds;

	return BT_GATT_WriteCharacteristicDescriptors(pChnl->handler.remDev, gatt_op);
}
#endif


#ifdef __BT_TIMES_PROFILE__
static void times_gatt_connection_callback(void *handler, BtGattEvent *event) {
	switch (event->eType) {
		case BT_GATT_APP_EVENT_CONNECTED:
			OS_Report("[Time] %s() BT_GATT_APP_EVENT_CONNECTED", __FUNCTION__);
			times_notify_gatt_connected(event->link);
			break;

		case BT_GATT_APP_EVENT_DISCONNECTED:
			OS_Report("[Time] %s() BT_GATT_APP_EVENT_DISCONNECTED", __FUNCTION__);
			times_notify_gatt_disconnected(event->link);
			break;

		case BT_GATT_APP_EVENT_SERVER:
			OS_Report("[Time] BT_GATT_APP_EVENT_SERVER");
			// times_notify_incoming(event->link);
			break;

		default:
			break;
	}
}

void time_gatt_notify_value_callback(void *user_data, BtGattEvent *event) {
	BtGattValueNotificationResultEvent *ptr;
	TimePacket *pkt;
	TimeSContext *pCntx;

	OS_Report("[Time] %s(), event: 0x%02x (should be 0x12)",__FUNCTION__, event->eType);
	if (event->eType != BT_GATT_OPERATOR_NOTIFICATIONS) {
		return;
	}

	ptr = (BtGattValueNotificationResultEvent *) event->parms;
	pkt = ContainingRecord(user_data, TimePacket, gatt_op);
	pCntx = ContainingRecord(pkt->chnl, TimeSContext, chnl);

	OS_Report("[Time] Notify Value with result: %d", ptr->flag);

	if (pCntx != NULL) {
		times_notify_notify_value_result(pCntx);
	} else {
		OS_Report("[Time] ERROR: %s() with NULL context", __FUNCTION__);
	}
}

static BtStatus _time_gatt_notify_value(TimePacket *pkt, U16 value_handle, U16 size, U8 *data) {
	BtGattOp *gatt_op = &pkt->gatt_op;
	TimeSTransport *pChnl = (TimeSTransport *) pkt->chnl;

	btmtk_os_memset((U8 *) gatt_op, 0, sizeof(gatt_op));
	InitializeListEntry(&gatt_op->op.node);
	gatt_op->apCb = time_gatt_notify_value_callback;
	pkt->success_flag = 0;
	pkt->gatt_params.vno.handle = value_handle;
	pkt->gatt_params.vno.size = size;
	pkt->gatt_params.vno.data = data;
	gatt_op->parms.p.valueNotifications = &pkt->gatt_params.vno;

	return BT_GATT_Notifications(pChnl->handler.bdc->link, gatt_op);
}

U8 time_att_db_cttime_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	U8 status = BT_STATUS_FAILED;
	time_t rawtime;
	struct tm *timeinfo;

	OS_Report("[Time] %s(), type: %d", __FUNCTION__, type);
	switch (type) {
		case ATT_OP_READ:
              case ATT_OP_READ_LONG:
              case ATT_OP_READ_MULTIPLE:
              case ATT_OP_READ_BY_TYPE:

			time(&rawtime);
			timeinfo = localtime(&rawtime);

			_time_server_info.ct_time.year = timeinfo->tm_year + 1900;
			_time_server_info.ct_time.month = (U8) timeinfo->tm_mon + 1;
			_time_server_info.ct_time.day = (U8) timeinfo->tm_mday;
			_time_server_info.ct_time.hours = (U8) timeinfo->tm_hour;
			_time_server_info.ct_time.minutes = (U8) timeinfo->tm_min;
			_time_server_info.ct_time.seconds = (U8) timeinfo->tm_sec;
			_time_server_info.ct_time.wday = (U8) timeinfo->tm_wday + 1;
			_time_server_info.ct_time.frac = 0;
			_time_server_info.ct_time.adj_reason = 0;

			OS_Report("[Time] %d-%d-%d, %d:%d:%d.%d, day of week: %d",
					_time_server_info.ct_time.year,
					_time_server_info.ct_time.month,
					_time_server_info.ct_time.day,
					_time_server_info.ct_time.hours,
					_time_server_info.ct_time.minutes,
					_time_server_info.ct_time.seconds,
					_time_server_info.ct_time.frac,
					_time_server_info.ct_time.wday);

			*raw = (U8 *) &_time_server_info.ct_time;
			*len = sizeof(_time_server_info.ct_time);
			status = BT_STATUS_SUCCESS;
			break;

		default:
			break;
	}
	
	return status;
}

U8 time_att_db_cttime_config_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	TimeSTransport *pChnl;

	OS_Report("[Time] %s(), type: %d, len: %d", __FUNCTION__, type, *len);

	pChnl = times_find_server_tp(link->bdAddr.addr);
	if (pChnl == NULL) {
		return BT_STATUS_FAILED;
	}

	if (type == ATT_OP_READ ||
           type == ATT_OP_READ_LONG ||
           type == ATT_OP_READ_MULTIPLE ||
           type == ATT_OP_READ_BY_TYPE) {
		*raw = (U8 *) &pChnl->cttime_notify_config;
		*len = sizeof(pChnl->cttime_notify_config);
		return BT_STATUS_SUCCESS;

	} else if (type == ATT_OP_WRITE || type == ATT_OP_EXECUTE_WRITE) {
		if (*len != 2) {
			return BT_STATUS_FAILED;
		} else {
			pChnl->cttime_notify_config = ((0x00ff & *(*raw)) | (0xff00 & (*(*raw+1) << 8)));
			OS_Report("[Time] Current Time notify config updated: 0x%04x", pChnl->cttime_notify_config);
			return BT_STATUS_SUCCESS;
		}
	}
	return BT_STATUS_FAILED;
}

U8 time_att_db_local_time_info_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	TimeSTransport *pChnl;

	OS_Report("[Time] %s(), type: %d", __FUNCTION__, type);
	pChnl = times_find_server_tp(link->bdAddr.addr);
	if (pChnl == NULL) {
		return BT_STATUS_FAILED;
	}

	if (type == ATT_OP_READ ||
           type == ATT_OP_READ_LONG ||
           type == ATT_OP_READ_MULTIPLE ||
           type == ATT_OP_READ_BY_TYPE) {
		*raw = (U8 *) &_time_server_info.local_time_info;
		*len = sizeof(_time_server_info.local_time_info);
		return BT_STATUS_SUCCESS;
	}

	return BT_STATUS_FAILED;
}

U8 time_att_db_ref_time_info_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	TimeSTransport *pChnl;

	OS_Report("[Time] %s(), type: %d", __FUNCTION__, type);
	pChnl = times_find_server_tp(link->bdAddr.addr);
	if (pChnl == NULL) {
		return BT_STATUS_FAILED;
	}

	if (type == ATT_OP_READ ||
           type == ATT_OP_READ_LONG ||
           type == ATT_OP_READ_MULTIPLE ||
           type == ATT_OP_READ_BY_TYPE) {
		*raw = (U8 *) &_time_server_info.ref_time_info;
		*len = sizeof(_time_server_info.ref_time_info);
		return BT_STATUS_SUCCESS;
	}

	return BT_STATUS_FAILED;
}

U8 time_att_db_time_with_dst_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	TimeSTransport *pChnl;

	pChnl = times_find_server_tp(link->bdAddr.addr);
	if (pChnl == NULL) {
		return BT_STATUS_FAILED;
	}

	OS_Report("[Time] %s(), type: %d", __FUNCTION__, type);
	if (type == ATT_OP_READ ||
           type == ATT_OP_READ_LONG ||
           type == ATT_OP_READ_MULTIPLE ||
           type == ATT_OP_READ_BY_TYPE) {
		*raw = (U8 *) &_time_server_info.time_with_dst;
		*len = sizeof(_time_server_info.time_with_dst);
		return BT_STATUS_FAILED;
	}

	return BT_STATUS_FAILED;
}

U8 time_att_db_update_ctrl_point_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	TimeSTransport *pChnl;
	TimeSContext *pCntx;

	pChnl = times_find_server_tp(link->bdAddr.addr);
	if (pChnl == NULL) {
		return BT_STATUS_FAILED;
	}

	OS_Report("[Time] %s(), type: %d", __FUNCTION__, type);
	pCntx = ContainingRecord(pChnl, TimeSContext, chnl);
	if (type == ATT_OP_WRITE || type == ATT_OP_EXECUTE_WRITE) {
		if ((pChnl->update_status.cur_state == TIME_UPDATE_STATE_IDLE) &&
				(*raw[0] == TIME_UPDATE_CTRL_REQUEST)) {
			pChnl->update_status.cur_state = TIME_UPDATE_STATE_UPDATE_PENDING;
			times_notify_request_server_update(pCntx);
			return BT_STATUS_SUCCESS;

		} else if ((pChnl->update_status.cur_state == TIME_UPDATE_STATE_UPDATE_PENDING) &&
				(*raw[0] == TIME_UPDATE_CTRL_CANCEL)) {
			pChnl->update_status.cur_state = TIME_UPDATE_STATE_IDLE;
			pChnl->update_status.result = TIME_UPDATE_RESULT_CANCELED;
			times_notify_cancel_server_update(pCntx);
			return BT_STATUS_SUCCESS;
		}
	}
	return BT_STATUS_FAILED;
}

U8 time_att_db_update_state_callback(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw) {
	TimeSTransport *pChnl = times_find_server_tp(link->bdAddr.addr);

	if (pChnl == NULL) {
		return BT_STATUS_FAILED;
	}

	OS_Report("[Time] %s(), type: %d", __FUNCTION__, type);
	if (type == ATT_OP_READ ||
           type == ATT_OP_READ_LONG ||
           type == ATT_OP_READ_MULTIPLE ||
           type == ATT_OP_READ_BY_TYPE) {
		*raw = (U8 *) &pChnl->update_status;
		*len = sizeof(pChnl->update_status);
		return BT_STATUS_SUCCESS;
	}

	return BT_STATUS_FAILED;
}
#endif

void time_gatt_connection_callback(void *handler, BtGattEvent *event) {
#ifdef __BT_TIMEC_PROFILE__
	timec_gatt_connection_callback(handler, event);
#endif

#ifdef __BT_TIMES_PROFILE__
	times_gatt_connection_callback(handler, event);
#endif
}

/********************************************************************************************
  * Exported functions
  ********************************************************************************************/
BtStatus time_gatt_init() {
	// BT_GattRegisterConnectedEvent(&_time_gatt_handler, time_gatt_connection_callback);
	return BT_STATUS_SUCCESS;
}

#ifdef __BT_TIMEC_PROFILE__
BtStatus time_gatt_register_client_callback() {
	BT_GattRegisterConnectedEvent(&_timec_gatt_handler, timec_gatt_connection_callback);
	return BT_STATUS_SUCCESS;
}

BtStatus time_gatt_deregister_client_callback() {
	BT_GattUnregisterConnectedEvent(&_timec_gatt_handler);
	return BT_STATUS_SUCCESS;
}

BtStatus time_gatt_connect_service(TimeCTransport *pChnl) {
	BtStatus status;
	TIMEASSERT(pChnl->check_byte == TIME_CHECK_BYTE);

	if (pChnl->handler.remDev) {
		_cur_timec_chnl = pChnl;
		// Wait for BT_GATT_APP_EVENT_CONNECTED
		status = GattClientConnect(pChnl->handler.remDev);
		OS_Report("[Time] GattClientConnect() returns %d", status);
	} else {
		status = BT_STATUS_INVALID_PARM;
		OS_Report("[Time] Error: pChnl->handler.remDev is NULL");
	}

	return status;
}

BtStatus time_gatt_destroy_link(TimeCTransport *pChnl) {
	BtStatus status;

	status = CMGR_RemoveDataLink(&pChnl->handler);
	status = CMGR_DeregisterHandler(&pChnl->handler);

	// Always return success
	return BT_STATUS_SUCCESS;
}

BtStatus time_gatt_disconnect_service(TimeCTransport *pChnl) {
	BtStatus status;
	TIMEASSERT(pChnl->check_byte == TIME_CHECK_BYTE);

	status = GattDisconnect(pChnl->handler.remDev);

	OS_Report("[Time] %s(), result: %d", __FUNCTION__, status);
	return status;
}

BtStatus time_gatt_read_remote_value(TimePacket *pkt, U8 seq_num, U32 value_id) {
	BtStatus status = BT_STATUS_FAILED;
	TimeCTransport *pChnl = pkt->chnl;
	U16 value_handle = 0x0000, start_handle = 0x0000, end_handle = 0x0000;
	U16 value_uuid = 0, service_uuid = 0;

	pkt->seq_num = seq_num;
	pkt->value_id = value_id;
	pkt->type = TIME_PKT_READ;
	pkt->value = 0;

	_time_access_value_handle_cache(pChnl, value_id, 0, &value_handle);
	_time_access_service_handle_cache(pChnl, value_id, 0, &start_handle, &end_handle);
	OS_Report("[Time] %s(), value_id: %d, value_handle: %d, service range: %d to %d",
			__FUNCTION__, value_id, value_handle, start_handle, end_handle);

	switch (value_id) {
		case GATT_TIME_CTTIME:
			if (value_handle != 0x0000) {
				status = _time_gatt_read_characteristic_value(pkt, value_handle);
				return status;
			} else {
				value_uuid = BT_UUID_CT_TIME;
				service_uuid = BT_UUID_GATT_SERVICE_TIME;
			}
			break;

		case GATT_TIME_LOCAL_TIME_INFO:
			if (value_handle != 0x0000) {
				status = _time_gatt_read_characteristic_value(pkt, value_handle);
				return status;
			} else {
				value_uuid = BT_UUID_LOCAL_TIME_INFO;
				service_uuid = BT_UUID_GATT_SERVICE_TIME;
			}
			break;

		case GATT_TIME_REF_TIME_INFO:
			if (value_handle != 0x0000) {
				status = _time_gatt_read_characteristic_value(pkt, value_handle);
				return status;
			} else {
				value_uuid = BT_UUID_REF_TIME_INFO;
				service_uuid = BT_UUID_GATT_SERVICE_TIME;
			}
			break;

		case GATT_TIME_DST_INFO:
			if (value_handle != 0x0000) {
				status = _time_gatt_read_characteristic_value(pkt, value_handle);
				return status;
			} else {
				value_uuid = BT_UUID_TIME_WITH_DST;
				service_uuid = BT_UUID_GATT_SERVICE_DST;
			}
			break;

		case GATT_TIME_SERVER_UPDATE_STATE:
			if (value_handle != 0x0000) {
				status = _time_gatt_read_characteristic_value(pkt, value_handle);
				return status;
			} else {
				value_uuid = BT_UUID_TIME_UPDATE_STATE;
				service_uuid = BT_UUID_GATT_SERVICE_TIMEUPDATE;
			}
			break;

		case GATT_TIME_CTTIME_NOTIFY:
			if (value_handle != 0x0000) {
				status = _time_gatt_read_characteristic_descriptor(pkt, value_handle);
			} else {
				value_uuid = BT_UUID_CT_TIME;
				service_uuid = BT_UUID_GATT_SERVICE_TIME;
		
			}
			break;

		default:
			return BT_STATUS_FAILED;
	}

	if (start_handle != 0x0000 && end_handle != 0x0000) {
		status = _time_gatt_discovery_characteristic_by_uuid(pkt, value_uuid, start_handle, end_handle);
	} else {
		status = _time_gatt_discovery_primary_service_by_uuid(pkt, service_uuid, 0x0001, 0xffff);
	}

	OS_Report("[Time] %s(), result: %d", __FUNCTION__, status);
	return status;
}

// In Time Profile, the largest value size is 2-byte.
BtStatus time_gatt_write_remote_value(TimePacket *pkt, U8 seq_num, U32 value_id, U16 value) {
	BtStatus status = BT_STATUS_FAILED;
	TimeCTransport *pChnl = pkt->chnl;
	U16 value_handle = 0x0000, start_handle = 0x0000, end_handle = 0x0000;
	U16 value_uuid = 0, service_uuid = 0;

	pkt->seq_num = seq_num;
	pkt->value_id = value_id;
	pkt->type = TIME_PKT_WRITE;
	pkt->value = value;

	_time_access_value_handle_cache(pChnl, value_id, 0, &value_handle);
	_time_access_service_handle_cache(pChnl, value_id, 0, &start_handle, &end_handle);

	OS_Report("[Time] %s(), value_id: %d, value_handle: %d, service range: %d to %d",
			__FUNCTION__, value_id, value_handle, start_handle, end_handle);

	switch (value_id) {
		case GATT_TIME_SERVER_UPDATE_CTRL:
			if (value_handle != 0x0000) {
				status = _time_gatt_write_without_response(pkt, value_handle);
				return status;
			} else {
				value_uuid = BT_UUID_TIME_UPDATE_CONTROL_POINT;
				service_uuid = BT_UUID_GATT_SERVICE_TIMEUPDATE;
			}
			break;

		case GATT_TIME_CTTIME_NOTIFY:
			if (value_handle != 0x0000) {
				status = _time_gatt_write_characteristic_descriptor(pkt, value_handle);
				return status;
			} else {	
				value_uuid = BT_UUID_CT_TIME;
				service_uuid = BT_UUID_GATT_SERVICE_TIME;
			}
			break;;

		default:
			return BT_STATUS_FAILED;
	}

	if (start_handle != 0x0000 && end_handle != 0x0000) {
		status = _time_gatt_discovery_characteristic_by_uuid(pkt, value_uuid, start_handle, end_handle);
	} else {
		status = _time_gatt_discovery_primary_service_by_uuid(pkt, service_uuid, 0x0001, 0xffff);
	}

	OS_Report("[Time] %s(), result: %d", __FUNCTION__, status);
	return status;
}
#endif

#ifdef __BT_TIMES_PROFILE__
BtStatus time_gatt_register_server_callback() {
	BT_GattRegisterConnectedEvent(&_times_gatt_handler, times_gatt_connection_callback);
	return BT_STATUS_SUCCESS;
}

BtStatus time_gatt_deregister_server_callback() {
	BT_GattUnregisterConnectedEvent(&_times_gatt_handler);
	return BT_STATUS_SUCCESS;
}

BtStatus time_gatt_server_disconnect(TimeSTransport *pChnl) {
	BtStatus status;
	status = GattDisconnect(pChnl->handler.remDev);
	OS_Report("[Time] GattDisconnect() returns %d", status);
	return status;
}

BtStatus time_gatt_write_local_value(TimeSContext *pCntx, U8 value_id) {
	TimeSTransport *server_tp;
	U8 i = 0;

	if (pCntx != NULL) {
		switch (value_id) {
			case GATT_TIME_LOCAL_TIME_INFO:
				_time_server_info.local_time_info.time_zone = pCntx->time_zone;
				_time_server_info.local_time_info.curr_dst_offset = pCntx->dst;
				return BT_STATUS_SUCCESS;

			case GATT_TIME_REF_TIME_INFO:
				_time_server_info.ref_time_info.time_source = pCntx->time_source;
				_time_server_info.ref_time_info.time_accuracy = pCntx->accuracy;
				_time_server_info.ref_time_info.days_since_update = pCntx->days_since_update;
				_time_server_info.ref_time_info.hours_since_update = pCntx->hours_since_update;
				return BT_STATUS_SUCCESS;

			case GATT_TIME_DST_INFO:
				_time_server_info.time_with_dst.year = pCntx->date_time[0];
				_time_server_info.time_with_dst.month = (U8) pCntx->date_time[1];
				_time_server_info.time_with_dst.day = (U8) pCntx->date_time[2];
				_time_server_info.time_with_dst.hours = (U8) pCntx->date_time[3];
				_time_server_info.time_with_dst.minutes = (U8) pCntx->date_time[4];
				_time_server_info.time_with_dst.seconds = (U8) pCntx->date_time[5];
				_time_server_info.time_with_dst.next_dst_offset = (U8) pCntx->dst;
				return BT_STATUS_SUCCESS;

			case GATT_TIME_SERVER_UPDATE_STATE:
				// TBD
				/*for (i = 0; i < TIME_MAX_CLIENT_NUM; i++) {
					server_tp = _time_server_used_tps[i];
					if (server_tp != NULL &&
							(server_tp->update_status.cur_state == TIME_UPDATE_STATE_UPDATE_PENDING)) {
						server_tp->update_status.cur_state = TIME_UPDATE_STATE_IDLE;
						server_tp->update_status.result = pCntx->update_result;
					}
				}
				*/
				return BT_STATUS_SUCCESS;

			default:
				OS_Report("[Time] Write local value failed, value_id: %d", value_id);
		}
	} 
	return BT_STATUS_FAILED;
}

BtStatus time_gatt_notify_value(TimePacket *pkt, U8 seq_num, U32 value_id) {
	BtStatus status = BT_STATUS_FAILED;
	TimeSTransport *pChnl = pkt->chnl;
	TimeSContext *pCntx = ContainingRecord(pChnl, TimeSContext, chnl);
	U8 *data = NULL;
	U16 value_handle = 0x0000, size = 0;
	time_t rawtime;
	struct tm *timeinfo;

	OS_Report("[Time] %s(), valud_id: %d, config: 0x%x",
			__FUNCTION__, value_id, pChnl->cttime_notify_config);

	if (pCntx != NULL && (pChnl->cttime_notify_config & GATT_CHAR_PROP_NOTIFY)) {
		switch (value_id) {
			case GATT_TIME_CTTIME:
				// TODO: Check don't-notify conditions. Within 15 min, Time adjust < 1min, etc.
				time(&rawtime);
				timeinfo = localtime(&rawtime);

				_time_server_info.ct_time.year = timeinfo->tm_year + 1900;
				_time_server_info.ct_time.month = (U8) timeinfo->tm_mon + 1;
				_time_server_info.ct_time.day = (U8) timeinfo->tm_mday;
				_time_server_info.ct_time.hours = (U8) timeinfo->tm_hour;
				_time_server_info.ct_time.minutes = (U8) timeinfo->tm_min;
				_time_server_info.ct_time.seconds = (U8) timeinfo->tm_sec;
				_time_server_info.ct_time.wday = (U8) timeinfo->tm_wday + 1;
				_time_server_info.ct_time.frac = 0;
				_time_server_info.ct_time.adj_reason = pCntx->adj_reason;

				value_handle = ATT_HANDLE_CHARACTERISTIC_CT_TIME_VALUE;
				data = (U8 *) &_time_server_info.ct_time;
				size = sizeof(_time_server_info.ct_time);
				break;
		}
		status = _time_gatt_notify_value(pkt, value_handle, size, data);
	}
	return status;
}

#endif

#endif
