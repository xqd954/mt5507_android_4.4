/*****************************************************************************
 *
 * Filename:
 * ---------
 * gattsi.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth GATT-Based (Server) Profile internal used header file.
 *
 * Author:
 * -------
 * SH Lai
 *
 ****************************************************************************/
#ifndef __GATTSI_H__
#define __GATTSI_H__

#include "gatti.h"
#include "gatt_srv.h"


#define GATTS_MAX_USER 10
#define GATTS_MAX_CONN (GATTS_MAX_USER * NUM_BT_DEVICES)
#define GATTS_MAX_BG_CONN  (GATTS_MAX_USER * NUM_BT_DEVICES)

#define GATTS_IS_VALID_USER(u) ((u) != NULL && IsNodeOnList(&GATTSC(userList), &((u)->node)))

typedef U32 GattcFlag;
#define GATTC_FLAG_SCANING                                       1

typedef struct _GattsUserContext GattsUserContext;
typedef struct _GattsConnection GattsConnection;
typedef struct _GattsBgConn GattsBgConn;

typedef struct _BleLocalServiceNode{
    /* Don't put any other fields before svc_node */
    BleServiceNode svc_node;

    /* GATTS information */
    U8 is_primary;
    GattsUserContext *user;
    ATT_HANDLE nextAddedHandle; /* keep the handle for next added attribute */
    /* Memory used to hold composed ATT record */
    U16 svcRecordSize;
    U8 *pSvcRecord;
    /* Memory used to hold composed SDP record */
    U16 svcSDPRecordSize;
    U8 *pSvcSDPRecord;
    SdpRecord sdpRecord;
}BleLocalServiceNode;


typedef struct _BleLocalCharNode
{
    /* Don't put any other fields before svc_node */
    BleCharNode char_node;

    /* GATTS information */
    U32 permission;
} BleLocalCharNode;

typedef struct _BleLocalCharDescNode
{
    /* Don't put any other fields before svc_node */
    BleCharDescNode desc_node;

    /* GATTS information */
    U32 permission;
} BleLocalCharDescNode;

struct _GattsBgConn{
    ListEntry node;
    BD_ADDR addr;
    GattsUserContext *user;
};


struct _GattsConnection
{
    ListEntry         node;

    GattChannelHandler channelHandler;
    /* Bearer */
    GattChannel *channel;
    GattsUserContext *user;
};

struct _GattsUserContext
{
    ListEntry	node;
    ListEntry     connList;
    ListEntry     bgConnList;
    U16            prepare_count[MAX_ATT_CON_NO];
    GattsCallback callback;
};

typedef struct _GattsContext
{
    /* GattsUserContext list */
    ListEntry userList;
    ListEntry freeUserList;
    GattsUserContext userContext[GATTS_MAX_USER];

    /* free conn list */
    /* Gattc connection context */
    ListEntry freeConnList;
    GattcConnection connContext[GATTC_MAX_CONN];

    /* free bg conn list */
    ListEntry freeBgConnList;
    GattcBgConn bgConn[GATTS_MAX_BG_CONN];

    U32 leScanIdx;
    BtAdvEventHandler leAdvHandler;

    BtGattHandler gatt_handler;

    U8 att_conn_mask[(MAX_ATT_CON_NO+7)/8];
    struct{
        BtRemoteDevice *link;
    } att_conn[MAX_ATT_CON_NO];
} GattsContext;

#endif
