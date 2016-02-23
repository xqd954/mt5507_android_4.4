/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
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
 * att_i.h
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 * Low Energy Attribute Protocol internal header
 *
 * Author:
 * -------
 * Autumn Li(mtk01174)
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
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
 * 08 01 2011 autumn.li
 * [ALPS00064244] [BT] [BLE] ATT/GATT revision
 * ATT/GATT revision
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/

#ifndef __ATT_I_H
#define __ATT_I_H

#include "me_adp.h"
#include "sec_adp.h"
#include "l2cap_adp.h"

#define MAX_ATT_CON_NO  NUM_BT_DEVICES
#define MAX_ATT_TX_PACKET_NO    7
/* One Request, One Response, Two Command,  Two Notification, One Confirmation */

#define MAX_PREPARE_WRITE_NO 3
#define MAX_ATT_RX_MTU_BREDR 48
#define MAX_ATT_RX_MTU_BLE   23

#define MAX_ATT_TX_MTU 60
/* For LE, ATT_MTU 23, For BR/EDR is 48, We use 60, not large or small */

#define MAX_ATT_TX_TIMEOUT 300000   
/* 30 seconds for ATT transaction timeout*/

#define ATT_CONN_DISCONNECTED       0
#define ATT_CONN_CONNECTED          1


#define ATT_TRANSACTION_NONE        0
#define ATT_TRANSACTION_ONGOING     1

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






typedef struct _BtAttEvent
{
    U8 eType;     
    BtRemoteDevice *link;
    void *parms;
}BtAttEvent;

typedef void (*GattCallBack) (BtAttEvent *);

typedef struct _BtAttPrepareWrite
{
    U8 available;
    U16 handle;
    U16 offset;
    U16 length;
    U8 writeBuffer[MAX_ATT_TX_MTU-5];
} BtAttPrepareWrite;


typedef struct _BtAttConn
{
    U8  state;
    BtRemoteDevice *link;
    BtPacket pkt[MAX_ATT_TX_PACKET_NO];
    BtPacket *bufferPkt;
    U8 txBuffer[MAX_ATT_TX_PACKET_NO][MAX_ATT_TX_MTU];
    ListEntry freePacketList;
    U16 mtu;
    U16 l2cap_id;
    U16 local_desired_mtu;
    EvmTimer txWaitTimer;
    EvmTimer rxWaitTimer;    
    U8 tx_transaction_ongoing;
    U8 rx_transaction_ongoing;
    BtAttPrepareWrite prepareWriteBuffer[MAX_PREPARE_WRITE_NO];
    BtSecurityToken secToken;
    BtSecurityRecord secRecord;
    EvmTimer securityCheckTimer;
    U8 rxTempBuffer[MAX_ATT_TX_MTU];
    U8 rxTempLength;
    //rspBuffer is used to hold the unfinished rsp data
    U8 rspBuffer[MAX_ATT_TX_MTU];
    U8 rspLength;
    union{
        BtATTReadByTypeRequestStruct read_by_type;
        BtATTReadRequestStruct read;
        BtATTReadBlobRequestStruct read_blob;
        BtATTReadMultipleRequestStruct read_multiple;
        BtATTWriteRequestStruct write;
        BtATTPrepareWriteRequestStruct prepare_write;
        BtATTExecuteWriteRequestStruct execute_write;
    }req_status;
    U8 securityCheckCount;
    U8 waitApplicationResponse;
    U32 trans_id;
} BtAttConn;


typedef struct _BtAttContext
{
    BtAttConn conn[MAX_ATT_CON_NO];
    BtAttEvent att_event;
    GattCallBack callback;
    U8 tempBuffer[MAX_ATT_TX_MTU];
    L2capPsm psm;
} BtAttContext;





#endif /* __ATT_I_H */

