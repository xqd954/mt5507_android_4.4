/*****************************************************************************
 *
 * Filename:
 * ---------
 * fmpi.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth GATT-Based (Client) Profile internal used header file.
 *
 * Author:
 * -------
 * SH Lai
 *
 ****************************************************************************/
#ifndef __GATTCI_H__
#define __GATTCI_H__

#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"
#include "gatt.h"
#include "gatti.h"
#include "ble_utils.h"
#include "conmgr.h"
#include "gatt_cl.h"
#include "BtbmLeAdv.h"

/*************************************************
*    Macros
*************************************************/
#define GATTC_MAX_USER 10
#define GATTC_MAX_CONN (GATTC_MAX_USER * NUM_BT_DEVICES)
#define GATTC_MAX_OP      (GATTC_MAX_CONN*5)
#define GATTC_MAX_BG_CONN  (GATTC_MAX_USER * NUM_BT_DEVICES)
#define GATTC_MAX_REG_NOTI  (GATTC_MAX_USER * NUM_BT_DEVICES)

#define GATTC_IS_VALID_USER(u) ((u) != NULL && IsNodeOnList(&GATTCC(userList), &((u)->node)))
#define GATTC_IS_VALID_CHANNEL(c) ((c) != NULL && IsNodeOnList(&GATTCC(channelList), &((c)->node)))
#define GATTC_IS_VALID_CONNECTION(u, c) ((u) != NULL && (c) != NULL && IsNodeOnList(u->connList, &((c)->node)))

typedef U32 GattcUserFlag;
#define GATTC_USER_FLAG_SCANING                             1

typedef U32 GattcFlag;
#define GATTC_FLAG_SCANING                                       1

/*************************************************
*    Structures
*************************************************/
typedef struct _GattcConnection GattcConnection;

/************************************************
*    GATTC request definition
*************************************************/
typedef U16 GATTC_REQ_TYPE;
#define GATTC_REQ_TYPE_SERVICE_SEARCH               1
#define GATTC_REQ_TYPE_INCL_SERVICE_SEARCH      2
#define GATTC_REQ_TYPE_GET_CHAR                           3
#define GATTC_REQ_TYPE_GET_DESCR                         4
#define GATTC_REQ_TYPE_READ_CHAR                        5
#define GATTC_REQ_TYPE_WRITE_CHAR                       6
#define GATTC_REQ_TYPE_READ_DESCR                       7
#define GATTC_REQ_TYPE_WRITE_DESCR                     8
#define GATTC_REQ_TYPE_EXEC_WRITE                       9
//#define GATTS_REQ_TYPE_INDICATION                         9

/* GATTC_REQ_TYPE_SERVICE_SEARCH */
typedef struct _GattcReqParamServiceSearch{
    GattcConnection *conn;
    ATT_UUID filterUuid;
}GattcReqParamServiceSearch;

/* GATTC_REQ_TYPE_INCL_SERVICE_SEARCH */
typedef struct _GattcReqParamInclServiceSearch{
    GattcConnection *conn;
    GATT_SVC_UUID svc_uuid;
    GATT_SVC_UUID prev_incl_svc_uuid;
}GattcReqParamInclServiceSearch;

/* GATTC_REQ_TYPE_GET_CHAR */
typedef struct _GattcReqParamGetChar{
    GattcConnection *conn;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID prev_char_uuid;
}GattcReqParamGetChar;

/* GATTC_REQ_TYPE_GET_DESCR */
typedef struct _GattcReqParamGetDescr{
    GattcConnection *conn;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID char_uuid;
    ATT_UUID prev_descr_uuid;
}GattcReqParamGetDescr;

/* GATTC_REQ_TYPE_READ_CHAR */
typedef struct _GattcReqParamReadChar{
    GattcConnection *conn;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID char_uuid;
    U32 auth_req;
}GattcReqParamReadChar;

/* GATTC_REQ_TYPE_WRITE_CHAR */
typedef struct _GattcReqParamWriteChar{
    GattcConnection *conn;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID char_uuid;
    U32 write_type;
    U16 len;
    U8 value[512];
    U32 auth_req;
}GattcReqParamWriteChar;

/* GATTC_REQ_TYPE_READ_DESCR */
typedef struct _GattcReqParamReadDescr{
    GattcConnection *conn;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID char_uuid;
    ATT_UUID descr_uuid;
    U32 auth_req;
}GattcReqParamReadDescr;

/* GATTC_REQ_TYPE_WRITE_DESCR */
typedef struct _GattcReqParamWriteDescr{
    GattcConnection *conn;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID char_uuid;
    ATT_UUID descr_uuid;
    U32 write_type;
    U16 len;
    U8 value[512];
    U32 auth_req;
}GattcReqParamWriteDescr;

/* GATTC_REQ_TYPE_EXEC_WRITE */
typedef struct _GattcReqParamExecWrite{
    GattcConnection *conn;
    U8 execute; /* 0 means cancel, others mean execute */
}GattcReqParamExecWrite;


/* GATTS_REQ_TYPE_INDICATION */
/*
typedef struct _GattsReqParamWriteDescr{
    GattsConnection *conn;
    GATT_SVC_UUID svc_uuid;
    ATT_UUID char_uuid;
    ATT_UUID descr_uuid;
    U32 write_type;
    U16 len;
    U8 value[512];
    U32 auth_req;
}GattsReqParamWriteDescr;
*/

typedef struct _GattcUserContext GattcUserContext;
typedef struct _GattcBgConn GattcBgConn;
typedef struct _GattcRegNotification GattcRegNotification;

    
struct _GattcBgConn{
    ListEntry node;
    BD_ADDR addr;
    GattcUserContext *user;
};

struct _GattcRegNotification{
    ListEntry node;
    BD_ADDR addr;
    GATT_SVC_UUID servUuid;
    ATT_UUID char_uuid;
};

typedef struct _GattcOp
{
    BtGattOp op;
    union 
    {
        BtGattDiscoveryAllPrimaryService discService;
        BtGattFindIncludeServices findService;
        BtGattDiscoveryAllCharacteristicDescriptors discCharDesc;
        BtGattDiscoveryPrimaryServiceByUUID discServiceUuid;
        BtGattReadCharacteristicUUID readCharUuid;
        BtGattReadCharacteristicValue readCharValue;
        BtGattWriteCharValueRequest writeCharValue;
        BtGattWriteWithoutResponse writeCharValueNoRsp;
        BtGattReliableWrite reliableWrite;
        BtGattExecWrite execWrite;
        BtGattSignedWriteWithoutResponse signedWriteCharValue;
        BtGattReadCharacteristicDescriptors readCharDescr;
        BtGattWriteDescriptors writeCharDescr;
        BtGattReadByType readByType;
        struct{
            BtGattValueIndications valueIndications;
            U8 value[ATT_MAX_VALUE_LEN];
        }indication;
        struct{
            BtGattValueNotifications valueNotifications;
            U8 value[ATT_MAX_VALUE_LEN];
        }notification;
    } parm;
    void *context;
} GattcOp;

typedef struct _GattcReq{
    ListEntry node;
    GATTC_REQ_TYPE type;
    GattcOp *op;
    union {
        GattcReqParamServiceSearch service_search;
        GattcReqParamInclServiceSearch incl_service_search;
        GattcReqParamGetChar get_char;
        GattcReqParamGetDescr get_descr;
        GattcReqParamReadChar read_char;
        GattcReqParamWriteChar write_char;
        GattcReqParamReadDescr read_descr;
        GattcReqParamWriteDescr write_descr;
        GattcReqParamExecWrite exec_write;
    }param;
}GattcReq;


/* For each channel, alloc one GattcConnection for each user */
struct _GattcConnection
{
    ListEntry         node;

    GattChannelHandler channelHandler;
    /* The pending request list */
    ListEntry reqList;
    /* Current ongoing request */
    GattcReq *currReq;
    /* Bearer */
    GattChannel *channel;
    GattcUserContext *user;
    EvmTimer asyncApiTimer;
};

/* Context for registered user */
struct _GattcUserContext
{
    ListEntry	node;

    GattcUserFlag  flag;
    GattcCallback callback;
    /* List of channels */
    ListEntry     connList;
    ListEntry     bgConnList;
    ListEntry     regNotiList;
    BtRemoteDevice *rssiDev;
};

typedef struct _GattcContext
{
    /* GattcUserContext list */
    ListEntry userList;
    ListEntry freeUserList;
    GattcUserContext userContext[GATTC_MAX_USER];
    /* GattChannelContext list */
    ListEntry channelList;
    ListEntry freeChannelList;
    GattChannel channelContext[NUM_BT_DEVICES];    
    /* Gattc connection context */
    ListEntry freeConnList;
    GattcConnection connContext[GATTC_MAX_CONN];
    /* Free operation list */
    ListEntry freeOpList;
    GattcOp gattOp[GATTC_MAX_OP];
    /* Free operation list */
    ListEntry freeBgConnList;
    GattcBgConn bgConn[GATTC_MAX_BG_CONN];
    /* Free operation list */
    ListEntry freeRegNotiList;
    GattcRegNotification regNoti[GATTC_MAX_BG_CONN];

    U32 leScanIdx;
    BtAdvEventHandler leAdvHandler;
    GattcFlag flag;

    // Register for global ME events
    BtHandler meHandler;
    
    BtGattHandler gatt_handler;

    ListEntry	dev_list;	// Collector or Thermometer connected device list 
    BtBleBondCbHandler  bond_handler;
} GattcContext;

#if XA_CONTEXT_PTR == XA_ENABLED
extern GattcContext *gattc_ctx;
#define GATTCC(s)  (gattc_ctx->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern GattcContext gattc_ctx;
#define GATTCC(s)  (gattc_ctx.s)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

GattcOp *gattcAllocGattOp(void *context);
void gattcFreeOp(GattcOp *op);
U8 gattc_uuid_compare (U8 len1, U8 *uuid1, U8 len2, U8 *uuid2);

extern const U8 bluetooth_base_uuid[16];

#endif /* __GATTCI_H__ */
