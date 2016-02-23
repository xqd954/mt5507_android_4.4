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

#ifndef __GATTCB_H
#define __GATTCB_H

#include "bt_feature.h"

#define BT_GATT_OPERATOR_EXCHANGE_MTU                       (0x01)
#define BT_GATT_OPERATOR_DISCOVERY_ALL_PRIMARY_SERVICE      (0x02)
#define BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID  (0x03)
#define BT_GATT_OPERATOR_FIND_INCLUDE_SERVICE               (0x04)
#define BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_OF_SERVICE      (0x05)
#define BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID             (0x06)
#define BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_DESCRIPTOR      (0x07)
#define BT_GATT_OPERATOR_READ_CHAR_VALUE                    (0x08)
#define BT_GATT_OPERATOR_READ_USING_CHAR_UUID               (0x09)
#define BT_GATT_OPERATOR_READ_LONG_CHAR_VALUE               (0x0a)
#define BT_GATT_OPERATOR_READ_MULTIPLE_CHAR_VALUE           (0x0b)
#define BT_GATT_OPERATOR_WRITE_WITHOUT_RESPONSE             (0x0c)
#define BT_GATT_OPERATOR_SIGNED_WRITE_WITHOUT_RESPONSE      (0x0e)
#define BT_GATT_OPERATOR_WRITE_CHAR_VALUE                   (0x0f)
#define BT_GATT_OPERATOR_WRITE_LONG_CHAR_VALUE              (0x10)
#define BT_GATT_OPERATOR_RELIABLEWRITES                     (0x11)
#define BT_GATT_OPERATOR_EXEC_WRITES                     (0x12)
#define BT_GATT_OPERATOR_NOTIFICATIONS                      (0x13)
#define BT_GATT_OPERATOR_INDICATIONS                        (0x14)
#define BT_GATT_OPERATOR_READ_CHAR_DESCRIPTORS              (0x15)
#define BT_GATT_OPERATOR_READ_LONG_CHAR_DESCRIPTORS         (0x16)
#define BT_GATT_OPERATOR_WRITE_CHAR_DESCRIPTORS             (0x17)
#define BT_GATT_OPERATOR_WRITE_LONG_CHAR_DESCRIPTORS        (0x18)
#define BT_GATT_OPERATOR_READ_BY_TYPE      (0x19)


#define BT_GATT_EVENT_RESULT_CONTINUE_DATA		0x00
#define BT_GATT_EVENT_RESULT_DONE_COMPLETED		0x01
#define BT_GATT_EVENT_RESULT_ERROR				0x02
#define BT_GATT_EVENT_RESULT_DISCONNECT			0x03


#define BT_GATT_APP_EVENT_CONNECTED 			0x00
#define BT_GATT_APP_EVENT_DISCONNECTED 			0x01
#define BT_GATT_APP_EVENT_VALUE_NOTIFICATION    0x02
#define BT_GATT_APP_EVENT_VALUE_INDICATION      0x03
#define BT_GATT_APP_EVENT_WRITE_REQUEST         0x04
#define BT_GATT_APP_EVENT_WRITE_COMMAND         0x05
#define BT_GATT_APP_EVENT_PREPARE_WRITE_REQUEST 0x06
#define BT_GATT_APP_EVENT_EXECUTE_WRITE_REQUEST 0x07
#define BT_GATT_APP_EVENT_SERVER                0x08



typedef struct _BtGattDiscoveryAllPrimaryServiceResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 startHandle;
	U16 endHandle;
	U8 type;
	U16 uuid2;
	U8 *uuid16;
    U8 error_code;
}BtGattDiscoveryAllPrimaryServiceResultEvent;

typedef struct _BtGattDiscoveryPrimaryServiceByUUIDResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 startHandle;
	U16 endHandle;
    U8 error_code;
}BtGattDiscoveryPrimaryServiceByUUIDResultEvent;


typedef struct _BtGattFindIncludeServicesResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 attributeHandle;
	U16 includeServiceHandle;
	U16 endGroupHandle;
	U8 type;
	U16 serviceUUID;
	U8 *serviceUUID16;
    U8 error_code;
}BtGattFindIncludeServicesResultEvent;


typedef struct _BtGattDiscoveryAllCharacteristicOfServiceResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 attributeHandle;
	U8 charProperty;
	U16 charValueAttributeHandle;
	U8 type;
	U16 charUUID;
	U8 *charUUID16;
    U8 error_code;   
}BtGattDiscoveryAllCharacteristicOfServiceResultEvent;

typedef struct _BtGattDiscoveryCharacteristicByUUIDResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 attributeHandle;
	U8 charProperty;
	U16 charValueAttributeHandle;
	U8 type;
	U16 charUUID;
	U8 *charUUID16;
    U8 error_code;    
}BtGattDiscoveryCharacteristicByUUIDResultEvent;

typedef struct _BtGattDiscoveryAllCharacteristicDescriptorsResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 attributeHandle;
	U8 type;
	U16 uuid16;
	U8 *uuid128;
    U8 error_code;    
}BtGattDiscoveryAllCharacteristicDescriptorsResultEvent;

typedef struct _BtGattReadCharacteristicValueResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 length; /* should be 1,2..     */
	U8 *data;   /* data[0] is the data */
    U8 error_code;    
}BtGattReadCharacteristicValueResultEvent;

typedef struct _BtGattReadCharacteristicUUIDResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 handle;
	U8 *data;
	U16 length;
    U8 error_code;    
}BtGattReadCharacteristicUUIDResultEvent;

typedef struct _BtGattReadLongCharacteristicValueResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 length;
	U8 *data;
    U8 error_code;    
}BtGattReadLongCharacteristicValueResultEvent;

typedef struct _BtGattReadMultipleCharacteristicValueResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 length;
	U8 *data;
    U8 error_code;    
}BtGattReadMultipleCharacteristicValueResultEvent;

typedef struct _BtGattWriteCharValueResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
    U8 error_code;    
}BtGattWriteCharValueResultEvent;

typedef struct _BtGattWriteLongCharValueResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
    U8 error_code;    
}BtGattWriteLongCharValueResultEvent;

typedef struct _BtGattReliableWriteResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
    U8 executed; /* Indicate if the prepared writes were executed */
    U8 error_code;    
}BtGattReliableWriteResultEvent;

typedef struct _BtGattExecWriteResultEvent
{
    U8 flag; /* 0x00 means report event, 0x01 means discovery done */
    U8 executed; /* Indicate if the prepared writes were executed */
    U8 error_code;    
}BtGattExecWriteResultEvent;

typedef struct _BtGattReadCharacteristicDescriptorsResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 length;
	U8 *data;
    U8 error_code;    
}BtGattReadCharacteristicDescriptorsResultEvent;

typedef struct _BtGattReadLongCharacteristicDescriptorsResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 length;
	U8 *data;
    U8 error_code;    
}BtGattReadLongCharacteristicDescriptorsResultEvent;

typedef struct _BtGattWriteCharDescriptorResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
    U8 error_code;    
}BtGattWriteCharDescriptorResultEvent;

typedef struct _BtGattWriteDescriptorResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
    U8 error_code;    
}BtGattWriteLongCharDescriptorResultEvent;

typedef struct _BtGattWriteWithoutResponseResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
    U8 error_code;    
}BtGattWriteWithoutResponseResultEvent;

typedef struct _BtGattSignedWriteWithoutResponseResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
    U8 error_code;    
}BtGattSignedWriteWithoutResponseResultEvent;

typedef struct _BtGattValueNotificationResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
    U8 error_code;    
}BtGattValueNotificationResultEvent;

typedef struct _BtGattValueIndicationsResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
    U8 error_code;    
}BtGattValueIndicationsResultEvent;

typedef struct _BtGattReadByTypeResultEvent
{
	U8 flag; /* 0x00 means report event, 0x01 means discovery done */
	U16 attributeHandle;
	U8 *value;
    U8 error_code;
}BtGattReadByTypeResultEvent;


#define BtGattValueIndicationEvent BtATTValueIndicationStruct
#define BtGattValueNotificationEvent BtATTValueNotificationStruct
#define BtGattWriteCommandEvent BtATTWriteCommandStruct
#define BtGattWriteRequestEvent BtATTWriteRequestStruct
#define BtGattPrepareWriteRequestEvent BtATTPrepareWriteRequestStruct
#define BtGattExecuteWriteRequestEvent BtATTExecuteWriteRequestStruct




#if 0
 


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
	U8 *signedData;
}BtGattSignedWriteWithoutResponse;

typedef struct _BtGattWriteCharValueRequest
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattWriteCharValueRequest;


typedef struct _BtGattReliableWrite
{	
	U16 handle;
	U16 size;
	U8 *data;
}BtGattReliableWrite;

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

#endif

// Use this to get the final callback
typedef void (*BTGATTUTILCALLBACK)(U8 status, void *data);

#define  GattservResult BtGattDiscoveryPrimaryServiceByUUIDResultEvent

// GattServiceCntx -> use to limit a range
typedef struct
{
    U8  flag;
    U32 userCallback;
    //BtGattDiscoveryPrimaryServiceByUUIDResultEvent
	U16 startHandle;
	U16 endHandle;
    U8  result; /* result of search the service record */
    GattservResult servHandle;
    U32 serviceUUID; /* primary service UUID */
    U32 feature; /* GATT feature */
} GattServiceCntx;

// GattCharCntx -> use to read/write a characteristic
typedef struct
{
    U8   flag;
    U16  handle; /* index */    
} GattCharCntx;

typedef struct
{
    GattServiceCntx servcntx;
    GattCharCntx charcntx;
}GattCharPacket;

#endif /* __ATT_H */ 

