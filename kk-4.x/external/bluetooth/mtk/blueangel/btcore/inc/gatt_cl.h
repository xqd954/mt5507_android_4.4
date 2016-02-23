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

#ifndef __GATT_CL_H_
#define __GATT_CL_H_

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/
typedef U8 GattcWriteType;
#define GATTC_WRITE_TYPE_NO_RSP     1
#define GATTC_WRITE_TYPE_REQUEST    2
#define GATTC_WRITE_TYPE_PREPARE    3

typedef U8 GattcEvent;

#define GATTC_EVENT_SCAN_RESULT                                   0
#define GATTC_EVENT_SCAN_COMPLETE_IND                       1
#define GATTC_EVENT_CONNECTED_IND                               2
#define GATTC_EVENT_DISCONNECTED_IND                         3
#define GATTC_EVENT_SEARCH_SERVICE_RESULT               4
#define GATTC_EVENT_SEARCH_SERVICE_COMPLETE_IND   5
#define GATTC_EVENT_INCL_SERVICE_RESULT                     6
#define GATTC_EVENT_GET_CHAR_RESULT                         7
#define GATTC_EVENT_GET_DESCR_RESULT                        8
#define GATTC_EVENT_READ_CHAR_RESULT                        9
#define GATTC_EVENT_WRITE_CHAR_RESULT                       10
#define GATTC_EVENT_READ_DESCR_RESULT                       11
#define GATTC_EVENT_WRITE_DESCR_RESULT                      12
#define GATTC_EVENT_EXEC_WRITE_RESULT                      13
#define GATTC_EVENT_READ_RSSI_RESULT                        14
#define GATTC_EVENT_NOTIFICATION_IND                       15
//#define GATTC_EVENT_GET_DEVICE_TYPE_RESULT              14


/****************************************************************************
 *
 * Macros
 *
 ****************************************************************************/

#ifndef BtStatus
typedef I8 BtStatus;

/*
#define BT_STATUS_SUCCESS             0
#define BT_STATUS_FAILED              1
#define BT_STATUS_PENDING             2
*/
#endif


/****************************************************************************
 *
 * structures
 *
 ****************************************************************************/
/* Handle for registered gatt client */
typedef void* GATTC_REG_HANDLE;
#define INVALID_GATTC_REG_HANDLE NULL

typedef void* GATTC_CONN_HANDLE;
#define INVALID_GATTC_CONN_HANDLE NULL

/* GATTC profile callback to internal interface */
/* event : GATTC_EVENT_SCAN_RESULT */
typedef struct{
    BD_ADDR addr;
    S32 rssi;
    U8 eir_len;
    U8 *eir;
}GattcScanResult;

/* event : GATTC_EVENT_SCAN_COMPLETE_IND */
typedef struct{
    U8 result;
}GattcScanCompleteInd;

/* event : GATTC_EVENT_CONNECTED_IND */
typedef struct{
    //U8 result;
    GATTC_CONN_HANDLE connHdl;
    BD_ADDR bdaddr;
}GattcConnectedInd;

/* event : GATTC_EVENT_DISCONNECTED_IND */
typedef struct{
    //U8 result;
    GATTC_CONN_HANDLE connHdl;
    BD_ADDR bdaddr;
}GattcDisconnectedInd;

/* event : GATTC_EVENT_SEARCH_SERVICE_RESULT */
typedef struct{
    GATTC_CONN_HANDLE connHdl;
    GATT_SVC_UUID svc_uuid;
}GattcSearchServiceResult;

/* event : GATTC_EVENT_SEARCH_SERVICE_COMPLETE_IND */
typedef struct{
    //U8 result;
    GATTC_CONN_HANDLE connHdl;
}GattcSearchServiceCompleteInd;

/* event : GATTC_EVENT_INCL_SERVICE_RESULT */
typedef struct{
    //U8 result;
    GATTC_CONN_HANDLE connHdl;
    GATT_SVC_UUID svc_uuid;
    GATT_SVC_UUID incl_svc_uuid;
}GattcSearchInclServiceResult;

/* event : GATTC_EVENT_GET_CHAR_RESULT */
typedef struct{
    GATTC_CONN_HANDLE connHdl;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID char_uuid;
    U8 properties;
}GattcGetCharResult;

/* event : GATTC_EVENT_GET_DESCR_RESULT */
typedef struct{
    //U8 result;
    GATTC_CONN_HANDLE connHdl;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID char_uuid;
    ATT_UUID descr_uuid;
}GattcGetDescrResult;

/* event : GATTC_EVENT_READ_CHAR_RESULT */
typedef struct{
    //U8 result;
    GATTC_CONN_HANDLE connHdl;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID char_uuid;
    U16 len;
    U8 *value;
}GattcReadCharResult;

/* event : GATTC_EVENT_WRITE_CHAR_RESULT */
typedef struct{
    GATTC_CONN_HANDLE connHdl;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID char_uuid;
}GattcWriteCharResult;

/* event : GATTC_EVENT_READ_DESCR_RESULT */
typedef struct{
    GATTC_CONN_HANDLE connHdl;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID char_uuid;
    ATT_UUID descr_uuid;
    U16 len;
    U8 *value;
}GattcReadDescrResult;

/* event : GATTC_EVENT_WRITE_DESCR_RESULT */
typedef struct{
    GATTC_CONN_HANDLE connHdl;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID char_uuid;
    ATT_UUID descr_uuid;
}GattcWriteDescrResult;

/* event : GATTC_EVENT_EXEC_WRITE_RESULT */
typedef struct{
    GATTC_CONN_HANDLE connHdl;
    U8 executed;
}GattcExecWriteResult;

/* event : GATTC_EVENT_READ_RSSI_RESULT */
typedef struct{
    /* Address of discovered device */
    BD_ADDR bdaddr;
    /* rssi of remote device */
    S32 rssi;
}GattcReadRssiResult;

/* event : GATTC_EVENT_NOTIFICATION_IND */
typedef struct{
    GATTC_CONN_HANDLE connHdl;
    /* Address of discovered device */
    BD_ADDR bdaddr;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID char_uuid;
    U16 len;
    U8 *value;
    U8 is_notify;
}GattcNotificationInd;

typedef struct _GattcCallbackParms 
{
    GattcEvent    event;                /* Event associated with the callback */
    GATTC_REG_HANDLE handle;  /* handle to indicate registered user */
    U8                result;               /* result of the operation */
    union {
        GattcScanResult                        scan_result;                    /* GATTC_EVENT_SCAN_RESULT */
        GattcScanCompleteInd               scan_complete_ind;        /* GATTC_EVENT_SCAN_COMPLETE_IND */
        GattcConnectedInd                    connected_ind;                /* GATTC_EVENT_CONNECTED_IND */
        GattcDisconnectedInd                disconnected_ind;            /* GATTC_EVENT_DISCONNECTED_IND */
        GattcSearchServiceResult           search_service_result;     /* GATTC_EVENT_SEARCH_SERVICE_RESULT */
        GattcSearchServiceCompleteInd  search_service_complete_ind; /* GATTC_EVENT_SEARCH_SERVICE_COMPLETE_IND */
        GattcSearchInclServiceResult      search_incl_service_result;      /* GATTC_EVENT_INCL_SERVICE_RESULT */
        GattcGetCharResult                    get_char_result;                      /* GATTC_EVENT_GET_CHAR_RESULT */
        GattcGetDescrResult                   get_descr_result;                    /* GATTC_EVENT_GET_DESCR_RESULT */
        GattcReadCharResult                  read_char_result;                    /* GATTC_EVENT_READ_CHAR_RESULT */
        GattcWriteCharResult                  write_char_result;                  /* GATTC_EVENT_WRITE_CHAR_RESULT */
        GattcReadDescrResult                 read_descr_result;                  /* GATTC_EVENT_READ_DESCR_RESULT */
        GattcWriteDescrResult                 write_descr_result;                /* GATTC_EVENT_WRITE_DESCR_RESULT */
        GattcExecWriteResult                 exec_write_result;                /* GATTC_EVENT_EXEC_WRITE_RESULT */
        GattcReadRssiResult                   read_rssi_result;                     /* GATTC_EVENT_READ_RSSI_RESULT */
        GattcNotificationInd                   notification_ind;                     /* GATTC_EVENT_NOTIFICATION_IND */        
    } param;
}GattcCallbackParms;

typedef void (*GattcCallback)(GATTC_REG_HANDLE handle, GattcCallbackParms *Parms);

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/
void GATTC_Init(void);
void GATTC_Deinit(void);
BtStatus GATTC_Register(GATTC_REG_HANDLE *handle, GattcCallback cb);
BtStatus GATTC_Deregister(GATTC_REG_HANDLE handle);
BtStatus GATTC_Open(GATTC_REG_HANDLE handle, U8 *addr, U32 is_direct, GATTC_CONN_HANDLE *connHdl);
BtStatus GATTC_Close(GATTC_REG_HANDLE regHdl, U8 *addr, GATTC_CONN_HANDLE connHdl);
BtStatus GATTC_Scan(GATTC_REG_HANDLE regHdl, U8 enable);
BtStatus GATTC_Refresh(GATTC_REG_HANDLE regHdl, U8 *addr);
BtStatus GATTC_SearchService(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, ATT_UUID *filterUuid);
BtStatus GATTC_GetInclService(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, GATT_SVC_UUID *prevInclServ);
BtStatus GATTC_GetChar(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, ATT_UUID *prevCharUuid);
BtStatus GATTC_GetDescr(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, ATT_UUID *charUuid, ATT_UUID *prevDescrUuid);
BtStatus GATTC_ReadChar(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, ATT_UUID *charUuid, U32 authReq);
BtStatus GATTC_WriteChar(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, ATT_UUID *charUuid, U32 writeType, U16 len, U8 *value, U32 authReq);
BtStatus GATTC_ReadDescr(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, ATT_UUID *charUuid, ATT_UUID *descrUuid, U32 authReq);
BtStatus GATTC_WriteDescr(GATTC_REG_HANDLE hdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, ATT_UUID *charUuid, ATT_UUID *descrUuid, U32 writeType, U16 len, U8 *value, U32 authReq);
BtStatus GATTC_ExecWrite(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, U8 execute);
BtStatus GATTC_RegisterNotification(GATTC_REG_HANDLE regHdl, U8 *bdaddr, GATT_SVC_UUID *servUuid, ATT_UUID *char_uuid, U8 reg);
BtStatus GATTC_ReadRssi(GATTC_REG_HANDLE regHdl, U8 *addr);
U8 GATTC_GetDeviceType(GATTC_REG_HANDLE regHdl, U8 *addr);
#endif /* __GATT_CL_H_ */

