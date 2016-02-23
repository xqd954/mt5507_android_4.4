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

#ifndef __GATT_H
#define __GATT_H

#include "bt_feature.h"

#define BT_GATT_OP_START                    0x00
#define BT_GATT_OP_END                      0x01
#define BT_GATT_OP_ERROR                    0x02
#define BT_GATT_OP_LINK_DISCONNECTED        0x03

typedef struct _BtGattEvent
{
    U8 eType;     
    BtRemoteDevice *link;
    void *parms;
    void *pGattOp;
}BtGattEvent;

typedef struct _BtGattDiscoveryAllPrimaryService
{
	U16 startHandle;
	U16 endHandle;
}BtGattDiscoveryAllPrimaryService;

typedef struct _BtGattDiscoveryPrimaryServiceByUUID
{
	U16 startHandle;
	U16 endHandle;
	U8 type;
	U8 uuid16[2];
	U8 uuid128[16];
}BtGattDiscoveryPrimaryServiceByUUID;

typedef struct _BtGattFindIncludeServices
{
	U16 startHandle;
	U16 endHandle;
	U16 tempData1;
	U16 tempData2;
	U16 tempData3;
}BtGattFindIncludeServices;


typedef struct _BtGattDiscoveryAllCharacteristicOfService
{
	U16 startHandle;
	U16 endHandle;
}BtGattDiscoveryAllCharacteristicOfService;

typedef struct _BtGattDiscoveryCharacteristicByUUID
{
	U16 startHandle;
	U16 endHandle;
	U8 type;
	U8 uuid16[2];
	U8 uuid128[16];
}BtGattDiscoveryCharacteristicByUUID;

typedef struct _BtGattDiscoveryAllCharacteristicDescriptors
{
	U16 startHandle;
	U16 endHandle;
}BtGattDiscoveryAllCharacteristicDescriptors;

typedef struct _BtGattReadCharacteristicValue
{
	U16 handle;
}BtGattReadCharacteristicValue;

typedef struct _BtGattReadCharacteristicUUID
{
	U16 startHandle;
	U16 endHandle;
	U8 type;
	U16 uuid16;
	U8 uuid128[16];
}BtGattReadCharacteristicUUID;

typedef struct _BtGattReadLongCharacteristicValue
{
	U16 handle;
	U16 offset;
}BtGattReadLongCharacteristicValue;

typedef struct _BtGattReadMultipleCharacteristicValue
{	
	/* Max support for 7 handle */
	U8 handleSetNo;
	U16 handle[7];
}BtGattReadMultipleCharacteristicValue;

typedef struct _BtGattWriteWithoutResponse
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattWriteWithoutResponse;

typedef struct _BtGattSignedWriteWithoutResponse
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattSignedWriteWithoutResponse;

typedef struct _BtGattWriteCharValueRequest
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattWriteCharValueRequest;

typedef struct _BtGattWriteLongCharacteristicValues
{	
	U16 handle;
	U16 offset;
	U16 size;
	U8 *data;
}BtGattWriteLongCharacteristicValues;


typedef struct _BtGattReliableWrite
{	
	U16 handle;
	U16 offset;
	U16 excuteFlag;    
	U16 size;
	U8 *data;
    /* Internal processing state */
    U16 curr_offset;
    U16 canceled;  /* Use to indicate if something wrong and need to cancel prepare writes */
}BtGattReliableWrite;

typedef struct _BtGattExecWrite
{	
    U8 execute;
}BtGattExecWrite;

typedef struct _BtGattValueNotifications
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattValueNotifications;

typedef struct _BtGattValueIndications
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattValueIndications;

typedef struct _BtGattReadCharacteristicDescriptors
{
	U16 handle;
}BtGattReadCharacteristicDescriptors;

typedef struct BtGattReadLongCharacteristicDescriptors
{
	U16 handle;
	U16 offset;
}BtGattReadLongCharacteristicDescriptors;

typedef struct _BtGattWriteDescriptors
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattWriteDescriptors;

typedef struct _BtGattWriteLongDescriptors
{	
	U16 handle;
	U16 offset;
	U16 size;
	U8 *data;
}BtGattWriteLongDescriptors;

typedef struct _BtGattReadByType
{
	U16 startHandle;
	U16 endHandle;
	U16 uuid16;
}BtGattReadByType;


typedef struct
{
    union
    {
		BtGattDiscoveryAllPrimaryService *allPrimaryService;
		BtGattDiscoveryPrimaryServiceByUUID *primaryServiceByUUID;
		BtGattFindIncludeServices *findIncludeServices;
		BtGattDiscoveryAllCharacteristicOfService *allCharOfServices;
		BtGattDiscoveryCharacteristicByUUID *charByUUID;
		BtGattDiscoveryAllCharacteristicDescriptors *allCharDescriptors;
		BtGattReadCharacteristicValue *readCharValue;
		BtGattReadCharacteristicUUID *readCharUUID;
		BtGattReadLongCharacteristicValue *readLongCharValue;
		BtGattReadMultipleCharacteristicValue *readMultipleCharValue;
		BtGattWriteWithoutResponse *writeWithoutResponse;
		BtGattSignedWriteWithoutResponse *signedWriteWithoutResponse;
		BtGattWriteCharValueRequest *writeRequest;
		BtGattWriteLongCharacteristicValues *writeLongCharValues;
		BtGattReliableWrite *reliableWrite;
              BtGattExecWrite *execWrite;
		BtGattValueNotifications *valueNotifications;
		BtGattValueIndications *valueIndications;
		BtGattReadCharacteristicDescriptors *readCharDescriptors;
		BtGattReadLongCharacteristicDescriptors *readLongCharDescriptors;
		BtGattWriteDescriptors *writeDescriptors;
		BtGattWriteLongDescriptors *writeLongDescriptors;
        BtGattReadByType *readByType;
	}p;
}btGattOperationData;


typedef void (*GattOpHandler) (U8 event, BtRemoteDevice *link);
typedef void (*GattAPCallBack) (void *, BtGattEvent *);

typedef struct
{
	BtOperation op; /* att operation */
	btGattOperationData parms;
	GattAPCallBack apCb;
} BtGattOp;


typedef struct _BtGattHandler
{
    ListEntry node;         	/* For internal stack use only. */
    GattAPCallBack callback;    /* Pointer to callback function */
} BtGattHandler;

void GATT_Init(void);
void BT_GattRegisterConnectedEvent(BtGattHandler *hdl, GattAPCallBack cb);
BtStatus BT_GATT_ExchangeMTU(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_DiscoveryAllPrimaryService(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_DiscoveryPrimaryServiceByUUID(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_FindIncludeService(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_DiscoveryAllCharacteristicOfService(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_DiscoveryCharacteristicByUUID(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_DiscoveryAllCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_ReadCharacteristicValue(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_ReadLongCharacteristicValues(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_WriteWithoutResponse(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_ReadMultipleCharacteristicValues(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_SignedWriteWithoutResponse(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_ReadCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_ReadUsingCharacteristicUUID(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_ReadLongCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_WriteCharacteristicValue(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_WriteLongCharacteristicValues(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_ReliableWrites(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_ExecuteWrites(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_WriteCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_WriteLongCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_Notifications(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_Indications(BtRemoteDevice *link, BtGattOp *op);
U16 BT_GATT_GetNegotiationMTU(BtRemoteDevice *link);
BtStatus BT_GATT_SetDisconnectTimeout(BtRemoteDevice *link, U32 value);

#endif /* __ATT_H */ 

