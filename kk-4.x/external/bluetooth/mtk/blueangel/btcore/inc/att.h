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

#ifndef __ATT_H
#define __ATT_H

#include "bt_feature.h"
#include "me_adp.h"
#include "att_i.h"
#include "attdef.h"

#define BT_ATT_OPCODE_ERROR_RESPONSE                      (0x01)
#define BT_ATT_OPCODE_EXCHANGE_MTU_REQUEST                (0x02)
#define BT_ATT_OPCODE_EXCHANGE_MTU_RESPONSE               (0x03)
#define BT_ATT_OPCODE_FIND_INFO_REQUEST                   (0x04)
#define BT_ATT_OPCODE_FIND_INFO_RESPONSE                  (0x05)
#define BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST          (0x06)
#define BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_RESPONSE         (0x07)
#define BT_ATT_OPCODE_READ_BY_TYPE_REQUEST                (0x08)
#define BT_ATT_OPCODE_READ_BY_TYPE_RESPONSE               (0x09)
#define BT_ATT_OPCODE_READ_REQUEST                        (0x0A)
#define BT_ATT_OPCODE_READ_RESPONSE                       (0x0B)
#define BT_ATT_OPCODE_READ_BLOB_REQUEST                   (0x0C)
#define BT_ATT_OPCODE_READ_BLOB_RESPONSE                  (0x0D)
#define BT_ATT_OPCODE_READ_MULTIPLE_REQUEST               (0x0E)
#define BT_ATT_OPCODE_READ_MULTIPLE_RESPONSE              (0x0F)
#define BT_ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST          (0x10)
#define BT_ATT_OPCODE_READ_BY_GROUP_TYPE_RESPONSE         (0x11)
#define BT_ATT_OPCODE_WRITE_REQUEST                       (0x12)
#define BT_ATT_OPCODE_WRITE_RESPONSE                      (0x13)
#define BT_ATT_OPCODE_WRITE_COMMAND                       (0x52)
#define BT_ATT_OPCODE_PREPARE_WRITE_REQUEST               (0x16)
#define BT_ATT_OPCODE_PREPARE_WRITE_RESPONSE              (0x17)
#define BT_ATT_OPCODE_EXECUTE_WRITE_REQUEST               (0x18)
#define BT_ATT_OPCODE_EXECUTE_WRITE_RESPONSE              (0x19)
#define BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION           (0x1B)
#define BT_ATT_OPCODE_HANDLE_VALUE_INDICATION             (0x1D)
#define BT_ATT_OPCODE_HANDLE_VALUE_CONFIRMATION           (0x1E)
#define BT_ATT_OPCODE_SIGNED_WRITE_COMMAND                (0xD2)


#define BT_ATT_EVENT_ERROR_RESPONSE                      (0x01)
#define BT_ATT_EVENT_EXCHANGE_MTU_REQUEST                (0x02)
#define BT_ATT_EVENT_EXCHANGE_MTU_RESPONSE               (0x03)
#define BT_ATT_EVENT_FIND_INFO_REQUEST                   (0x04)
#define BT_ATT_EVENT_FIND_INFO_RESPONSE                  (0x05)
#define BT_ATT_EVENT_FIND_BY_TYPE_VALUE_REQUEST          (0x06)
#define BT_ATT_EVENT_FIND_BY_TYPE_VALUE_RESPONSE         (0x07)
#define BT_ATT_EVENT_READ_BY_TYPE_REQUEST                (0x08)
#define BT_ATT_EVENT_READ_BY_TYPE_RESPONSE               (0x09)
#define BT_ATT_EVENT_READ_REQUEST                        (0x0A)
#define BT_ATT_EVENT_READ_RESPONSE                       (0x0B)
#define BT_ATT_EVENT_READ_BLOB_REQUEST                   (0x0C)
#define BT_ATT_EVENT_READ_BLOB_RESPONSE                  (0x0D)
#define BT_ATT_EVENT_READ_MULTIPLE_REQUEST               (0x0E)
#define BT_ATT_EVENT_READ_MULTIPLE_RESPONSE              (0x0F)
#define BT_ATT_EVENT_READ_BY_GROUP_TYPE_REQUEST          (0x10)
#define BT_ATT_EVENT_READ_BY_GROUP_TYPE_RESPONSE         (0x11)
#define BT_ATT_EVENT_WRITE_REQUEST                       (0x12)
#define BT_ATT_EVENT_WRITE_RESPONSE                      (0x13)
#define BT_ATT_EVENT_WRITE_COMMAND                       (0x52)
#define BT_ATT_EVENT_PREPARE_WRITE_REQUEST               (0x16)
#define BT_ATT_EVENT_PREPARE_WRITE_RESPONSE              (0x17)
#define BT_ATT_EVENT_EXECUTE_WRITE_REQUEST               (0x18)
#define BT_ATT_EVENT_EXECUTE_WRITE_RESPONSE              (0x19)
#define BT_ATT_EVENT_VALUE_NOTIFICATION                  (0x1B)
#define BT_ATT_EVENT_VALUE_INDICATION                    (0x1D)
#define BT_ATT_EVENT_VALUE_CONFIRMATION                  (0x1E)
#define BT_ATT_EVENT_SIGNED_WRITE_COMMAND                (0xD2)

#define BT_ATT_EVENT_CONNECTED				             (0xF1)
#define BT_ATT_EVENT_CONNECTED_SECURITY_CHANGE           (0xF2)
#define BT_ATT_EVENT_DISCONNECTED  				         (0xF3)
#define BT_ATT_EVENT_NOTIFIED_PACKET_HANDLED		     (0xF4)



#define BT_ATT_ERROR_CODE_NO_ERROR                      (0x00)
#define BT_ATT_ERROR_CODE_INVALID_HANDLE                (0x01)
#define BT_ATT_ERROR_CODE_READ_NOT_PERMITTED            (0x02)
#define BT_ATT_ERROR_CODE_WRITE_NOT_PERMITTED           (0x03)
#define BT_ATT_ERROR_CODE_INVALID_PDU                   (0x04)
#define BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION   (0x05)
#define BT_ATT_ERROR_CODE_REQUEST_NOT_SUPPORTED         (0x06)
#define BT_ATT_ERROR_CODE_INVALID_OFFSET                (0x07)
#define BT_ATT_ERROR_CODE_INSUFFICIENT_AUTHORIZATION    (0x08)
#define BT_ATT_ERROR_CODE_PREPARE_QUEUE_FULL            (0x09)
#define BT_ATT_ERROR_CODE_ATTRIBUTE_NOT_FOUND           (0x0A)
#define BT_ATT_ERROR_CODE_ATTRIBUTE_NOT_LONG            (0x0B)
#define BT_ATT_ERROR_CODE_INSUFFICIENT_ENCRY_KEY_SIZE   (0x0C)
#define BT_ATT_ERROR_CODE_INVALID_ATTRIBUTE_VALUE_LEN   (0x0D)
#define BT_ATT_ERROR_CODE_UNLIKELY_ERROR                (0x0E)
#define BT_ATT_ERROR_CODE_INSUFFICIENT_ENCRYPTION       (0x0F)
#define BT_ATT_ERROR_CODE_UNSUPPORTED_GROUP_TYPE        (0x10)
#define BT_ATT_ERROR_CODE_INSUFFICIENT_RESOURCES        (0x11)


#define BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE       (0xF0)  /* Internal defined */
#define BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE     (0xF1)  /* Internal defined */

#if 0
typedef struct _BtATTErrorResponseStruct
{
	U8 opcode;
	U16 attributeHandle;
	U8 errorCode;
}BtATTErrorResponseStruct;


typedef struct _BtATTExchangeMTURequestStruct
{
	U16 clientRxMtu;
}BtATTExchangeMTURequestStruct;

typedef struct _BtATTExchangeMTUResponseStruct
{
	U16 serverRxMtu;
}BtATTExchangeMTUResponseStruct;


typedef struct _BtATTFindInfoRequestStruct
{
	U16 startingHandle;
    U16 endingHandle;
}BtATTFindInfoRequestStruct;

typedef struct _BtATTFindInfoResponseStruct
{
	U8 format;
    U16 infoDataLen;
    U8 *infoData;
}BtATTFindInfoResponseStruct;


typedef struct _BtATTFindByTypeValueRequestStruct
{
	U16 startingHandle;
    U16 endingHandle;
    U16 attributeType;
    U16 attributeValueLen;
    U8 *attributeValue;
}BtATTFindByTypeValueRequestStruct;

typedef struct _BtATTFindByTypeValueResponseStruct
{
    U16 handleInfoListLen;
    U8 *handleInfoList;
}BtATTFindByTypeValueResponseStruct;

typedef struct _BtATTReadByTypeRequestStruct
{
	U16 startingHandle;
    U16 endingHandle;
    U8 type;
    U16 attributeType2;
    U8 *attributeType;
    /******************************
    *   Info for execution progress
    ******************************/
    /* Keep current executed handle */
    U16 currHandle;
    U16 attrPairLen;
}BtATTReadByTypeRequestStruct;

typedef struct _BtATTReadByTypeResponseStruct
{
    U8 length;
    U16 handleDataListLen;
    U8 *handleDataList;
}BtATTReadByTypeResponseStruct;

typedef struct _BtATTReadRequestStruct
{
	U16 handle;
}BtATTReadRequestStruct;

typedef struct _BtATTReadResponseStruct
{
    U16 attributeValueLen;
    U8 *attributeValue;
}BtATTReadResponseStruct;

typedef struct _BtATTReadBlobRequestStruct
{
	U16 attributeHandle;
    U16 valueOffset;
}BtATTReadBlobRequestStruct;

typedef struct _BtATTReadBlobResponseStruct
{
    U16 partAttributeValueLen;
    U8 *partAttributeValue;
}BtATTReadBlobResponseStruct;


typedef struct _BtATTReadMultipleRequestStruct
{
    U16 setOfHandlesLen;
    U8 *setOfHandles;
    /******************************
    *   Info for execution progress
    ******************************/
    /* Pointer to current executed handle in setOfHandles */
    U8 *currHandle;
}BtATTReadMultipleRequestStruct;

typedef struct _BtATTReadMultipleResponseStruct
{
    U16 setOfValuesLen;
    U8 *setOfValues;
}BtATTReadMultipleResponseStruct;


typedef struct _BtATTReadByGroupTypeRequestStruct
{
	U16 startingHandle;
    U16 endingHandle;
    U8 type;
    U16 attributeType2;
    U8 *attributeType;
}BtATTReadByGroupTypeRequestStruct;

typedef struct _BtATTReadByGroupTypeResponseStruct
{
    U8 length;
    U16 attributeDataListLen;
    U8 *attributeDataList;
}BtATTReadByGroupTypeResponseStruct;


typedef struct _BtATTWriteRequestStruct
{
	U16 attributeHandle;
    U16 attributeValueLen;
    U8 *attributeValue;
}BtATTWriteRequestStruct;


typedef struct _BtATTWriteCommandStruct
{
	U16 attributeHandle;
    U16 attributeValueLen;
    U8 *attributeValue;
}BtATTWriteCommandStruct;
/* BtATTWriteRequestStruct and BtATTWriteCommandStruct must the same */

typedef struct _BtATTSignedWriteCommandStruct
{
	U16 attributeHandle;
    U16 attributeValueLen;
    U8 *attributeValue;
    U8 authenticationSignatureExists;
    U8 *authenticationSignature;
}BtATTSignedWriteCommandStruct;

typedef struct _BtATTPrepareWriteRequestStruct
{
	U16 attributeHandle;
    U16 valueOffset;
    U16 partAttributeValueLen;
    U8 *partAttributeValue;
}BtATTPrepareWriteRequestStruct;

typedef struct _BtATTPrepareWriteResponseStruct
{
	U16 attributeHandle;
    U16 valueOffset;
    U16 partAttributeValueLen;
    U8 *partAttributeValue;
}BtATTPrepareWriteResponseStruct;

typedef struct _BtATTExecuteWriteRequestStruct
{
    U8 flags;
    /******************************
    *   Info for execution progress
    ******************************/
    U16 currIndex;
    U16 currHandle;
    U8 err;                 /* If there already happens some error */
    U16 errHandle;    /* Keep the handle which happened error */
}BtATTExecuteWriteRequestStruct;

typedef struct _BtATTValueNotificationStruct
{
    U16 attributeHandle;
    U16 attributeValueLen;
    U8 *attributeValue;
}BtATTValueNotificationStruct;

typedef struct _BtATTValueIndicationStruct
{
    U16 attributeHandle;
    U16 attributeValueLen;
    U8 *attributeValue;
}BtATTValueIndicationStruct;
#endif

U8 BT_ATT_FindAndAssignEmptyConn(BtRemoteDevice *link);
BtStatus BT_ATT_GetConTransId(BtRemoteDevice *link, U16 *trans_id);
const void *BT_ATT_GetConCurrReq(BtRemoteDevice *link);
bt_att_op BT_ATT_GetConCurrOpcode(BtRemoteDevice *link);
U32 BT_ATT_GetConId(BtRemoteDevice *link);
BtAttConn *BT_ATT_GetConCtx(BtRemoteDevice *link);
BtStatus BT_ATT_Send(BtRemoteDevice *link, BtPacket *pkt);
BtStatus BT_ATT_SendHandleValueConfirmation(BtRemoteDevice *link);
BtStatus BT_ATT_SendExecuteWriteResponse(BtRemoteDevice *link);
BtStatus BT_ATT_SendPrepareWriteResponse(BtATTPrepareWriteResponseStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendWriteResponse(BtRemoteDevice *link);
BtStatus BT_ATT_SendReadByGroupTypeResponse(BtATTReadByGroupTypeResponseStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendReadMultipleResponse(BtATTReadMultipleResponseStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendReadBlobResponse(BtATTReadBlobResponseStruct *ptr, BtRemoteDevice *link);
void BT_ATT_HandleApplicationPrepareResponseData(BtAttConn *conn, U8 *data, U16 length);
BtStatus BT_ATT_SendReadResponse(BtATTReadResponseStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendReadByTypeResponse(BtATTReadByTypeResponseStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendFindByTypeValueResponse(BtATTFindByTypeValueResponseStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendFindInfoResponse(BtATTFindInfoResponseStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendErrorResponse(BtATTErrorResponseStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendExchangeMTUResponse(BtATTExchangeMTUResponseStruct *ptr, BtRemoteDevice *link);
void BT_ATT_StartTimer(BtAttConn *conn, EvmTimer *timer);
void BT_ATT_SecurityCheckDone(BtRemoteDevice *link);
void BT_ATT_HandleRxReady(BtRemoteDevice *link, U8 *data, U16 length);
void BT_ATT_ReportEvent(U8 event_code, BtAttConn *conn, void *parm);
BtStatus BT_ATT_SendHandleValueIndication(BtATTValueIndicationStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendHandleValueIndication(BtATTValueIndicationStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendSignedWriteCommand(BtATTSignedWriteCommandStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendWriteCommand(BtATTWriteCommandStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendHandleValueNotification(BtATTValueNotificationStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendExecuteWriteRequest(BtATTExecuteWriteRequestStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendPrepareWriteRequest(BtATTPrepareWriteRequestStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendWriteRequest(BtATTWriteRequestStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendReadMultipleRequest(BtATTReadMultipleRequestStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendReadBlobRequest(BtATTReadBlobRequestStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendFindInfoRequest(BtATTFindInfoRequestStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendReadRequest(BtATTReadRequestStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendReadByTypeRequest(BtATTReadByTypeRequestStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendFindByTypeValueRequest(BtATTFindByTypeValueRequestStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_SendReadByGroupTypeRequest(BtATTReadByGroupTypeRequestStruct *ptr, BtRemoteDevice *link);
BtStatus BT_ATT_ForGattExchangeMTURequest(BtRemoteDevice *link);

void BT_ATT_FreeConn(BtRemoteDevice *link);
BtStatus BT_ATT_Send(BtRemoteDevice *link, BtPacket *pkt);


void BT_ATT_ReportEvent(U8 event_code, BtAttConn *conn, void *parm);
void BT_ATT_HandleErrorResponse(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleExchangeMTURequest(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleExchangeMTUResponse(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleFindInfoRequest(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleFindInfoResponse(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleFindByTypeValueRequest(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleFindByTypeValueResponse(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleReadByTypeRequest(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleReadByTypeResponse(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleReadRequest(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleReadResponse(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleReadBlobRequest(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleReadBlobResponse(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleReadMultipleRequest(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleReadMultipleResponse(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleReadByGroupTypeRequest(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleReadByGroupTypeResponse(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleWriteRequest(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleWriteResponse(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleWriteCommand(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandlePrepareWriteRequest(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandlePrepareWriteResponse(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleExecuteWriteRequest(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleExecuteWriteResponse(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleHandleValueNotification(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleHandleValueIndication(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleHandleValueConfirmation(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleHandleSignedWriteCommand(BtAttConn *conn, U8 *data, U16 length);

#endif /* __ATT_H */ 

