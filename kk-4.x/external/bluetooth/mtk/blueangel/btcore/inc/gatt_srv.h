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
 
#ifndef __GATT_SRV_
#define __GATT_SRV_

#include "gatti.h"
 
 /****************************************************************************
  *
  * Constants
  *
  ****************************************************************************/
  
 
 /****************************************************************************
  *
  * Macros
  *
  ****************************************************************************/
#ifndef BtStatus
typedef I8 BtStatus;

//#define BT_STATUS_SUCCESS             0
//#define BT_STATUS_FAILED              1
//#define BT_STATUS_PENDING             2
#endif

typedef void* GATTS_REG_HANDLE;
#define INVALID_GATTS_REG_HANDLE NULL
    
typedef void* GATTS_CONN_HANDLE;
#define INVALID_GATTS_CONN_HANDLE NULL
 

typedef U8 GattsEvent;
#define GATTS_EVENT_CONNECTED_IND                               1
#define GATTS_EVENT_DISCONNECTED_IND                         2
#define GATTS_EVENT_READ_REQ_IND                                 3
#define GATTS_EVENT_WRITE_REQ_IND                               4
#define GATTS_EVENT_EXEC_WRITE_REQ_IND                     5
#define GATTS_EVENT_RSP_RESULT                                      6
#define GATTS_EVENT_CONFIRMATION_RESULT                    7

 /****************************************************************************
  *
  * structures
  *
  ****************************************************************************/ 
/* event : GATTS_EVENT_CONNECTED_IND */
typedef struct{
    GATTS_CONN_HANDLE connHdl;
    BD_ADDR *bdaddr;
}GattsConnectedInd;
 
/* event : GATTS_EVENT_DISCONNECTED_IND */
typedef struct{
    GATTS_CONN_HANDLE connHdl;
    BD_ADDR *bdaddr;
}GattsDisconnectedInd;
 
/* event : GATTS_EVENT_READ_REQ_IND */
typedef struct{
    GATTS_CONN_HANDLE connHdl;
    U32 trans_id;
    BD_ADDR *bdaddr;
    ATT_HANDLE handle;
    U32 offset;
    U8 is_long;
}GattsReadReqInd;

/* event : GATTS_EVENT_WRITE_REQ_IND */
typedef struct{
    GATTS_CONN_HANDLE connHdl;
    U32 trans_id;
    BD_ADDR *bdaddr;
    ATT_HANDLE handle;
    U32 offset;
    U8 need_rsp;
    U8 is_prepare;
    U16 value_len;
    const U8 *value;
}GattsWriteReqInd;

/* event : GATTS_EVENT_EXEC_WRITE_REQ_IND */
typedef struct{
    GATTS_CONN_HANDLE connHdl;
    U32 trans_id;
    BD_ADDR *bdaddr;
    U8 exec_write;  /* execute write or cancel */
}GattsExecWriteReqInd;

/* event : GATTS_EVENT_RSP_RESULT */
typedef struct{
    ATT_HANDLE handle;
}GattsRspResult;

/* event : GATTS_EVENT_CONFIRMATION_RESULT */
typedef struct{
    GATTS_CONN_HANDLE connHdl;
    ATT_HANDLE handle;
}GattsConfirmationResult;
 
 /***********************************
 *   Callback interface
 ***********************************/
typedef struct _GattsCallbackParms 
{
    GattsEvent    event;                /* Event associated with the callback */
    GATTS_REG_HANDLE handle;  /* handle to indicate registered user */
    U8                result;               /* result of the operation */
    union {
        GattsConnectedInd                    connected_ind;                /* GATTS_EVENT_CONNECTED_IND */
        GattsDisconnectedInd                disconnected_ind;            /* GATTS_EVENT_DISCONNECTED_IND */
        GattsReadReqInd                       read_req_ind;                  /* GATTS_EVENT_READ_REQ_IND */
        GattsWriteReqInd                      write_req_ind;                 /* GATTS_EVENT_WRITE_REQ_IND */
        GattsExecWriteReqInd                exec_write_req_ind;        /* GATTS_EVENT_EXEC_WRITE_REQ_IND */
        GattsRspResult                          rsp_result;                        /* GATTS_EVENT_RSP_RESULT */
        GattsConfirmationResult             confirmation_result;        /* GATTS_EVENT_CONFIRMATION_RESULT */
    } param;
}GattsCallbackParms;
 
typedef void (*GattsCallback)(GATTS_REG_HANDLE handle, GattsCallbackParms *Parms);

 /****************************************************************************
  *
  * Function Reference
  *
  ****************************************************************************/
void GATTS_Init(void);
void GATTS_Deinit(void);
BtStatus GATTS_Register(GATTS_REG_HANDLE *handle, GattsCallback cb);
void GATTS_ReportAllConnectedLink(GATTS_REG_HANDLE handle);
BtStatus GATTS_Deregister(GATTS_REG_HANDLE *handle);
BtStatus GATTS_Open(GATTS_REG_HANDLE handle, BD_ADDR *addr, U32 is_direct, GATTS_CONN_HANDLE *connHdl);
BtStatus GATTS_Close(GATTS_REG_HANDLE regHdl, GATTS_CONN_HANDLE connHdl);
BtStatus GATTS_Indication(GATTS_REG_HANDLE regHdl, GATTS_CONN_HANDLE connHdl, ATT_HANDLE handle, U8 need_confirm, ATT_VALUE *value);
BtStatus GATTS_Response(GATTS_REG_HANDLE regHdl, GATTS_CONN_HANDLE connHdl, ATT_HANDLE handle, U8 result, ATT_VALUE *value);
ATT_HANDLE GATTS_AddService(GATTS_REG_HANDLE user, GATT_SVC_UUID *svc_uuid, U16 num_handle);
ATT_HANDLE GATTS_AddInclService(ATT_HANDLE svc_handle, ATT_HANDLE incl_svc_handle);
ATT_HANDLE GATTS_AddChar(ATT_HANDLE svc_handle, ATT_UUID *uuid, U8 properties, U32 permission);
ATT_HANDLE GATTS_AddDescr(ATT_HANDLE svc_handle, ATT_UUID *descr_uuid, U32 permission);
BtStatus GATTS_StartService(ATT_HANDLE handle, GattTransport transport);
BtStatus GATTS_StopService(ATT_HANDLE handle);
BtStatus GATTS_DeleteService(ATT_HANDLE handle);

#endif /* __GATT_SRV_ */ 
