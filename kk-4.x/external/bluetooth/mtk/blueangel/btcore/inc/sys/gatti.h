/*****************************************************************************
 *
 * Filename:
 * ---------
 * gatti.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth GATT-Based Profile internal used header file.
 *
 * Author:
 * -------
 * SH Lai
 *
 ****************************************************************************/
#ifndef __GATTI_H__
#define __GATTI_H__

#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"
#include "bt_adp_gatt.h"
#include "conmgr.h"
#include "gatt.h"

#define gatt_malloc(size)    get_ctrl_buffer(size)
#define gatt_free(p)            do{if(p){free_ctrl_buffer(p); p = NULL;}}while(0)

#define MIN_GATTS_HANDLE    256
#define MAX_GATTS_HANDLE    1024

typedef U32 GattRole;
#define GATT_ROLE_CLIENT     0
#define GATT_ROLE_SERVER    1

typedef U32 GattTransport;
#define GATT_TRANSPORT_LE                1
#define GATT_TRANSPORT_BR_EDR       (1<<1)

typedef U16 GattErrorCode;
#define GATT_ERR_SUCCESS                   0
#define GATT_ERR_FAIL                             1
#define GATT_ERR_ACL_FAIL                  2
#define GATT_ERR_ATT_FAIL                  3
#define GATT_ERR_USER_TERMINATE     4
#define GATT_ERR_DISCOVER_FAIL        5

#define UUID16_LEN 2
#define UUID32_LEN 4
#define UUID128_LEN 16

/************************
*   GattChannel
**************************/
typedef U8 GattChannelState;
/* Default state when channel is allocated */
#define GATT_CHANNEL_STATE_INITED                               0
/* State when disconnected, this shall not be used after allocated */
#define GATT_CHANNEL_STATE_DISCONNECTED                  1
/* State when connecting ACL */
#define GATT_CHANNEL_STATE_ACL_CONNECTING              2
/* State when ACL is connected */
#define GATT_CHANNEL_STATE_ACL_CONNECTED                3
/* State when connecting ATT channel */
#define GATT_CHANNEL_STATE_ATT_CONNECTING              4
/* State when ATT channel is connected */
#define GATT_CHANNEL_STATE_ATT_CONNECTED               5
/* State when discovering remote service */
//#define GATT_CHANNEL_STATE_DISCOVERING                    6
/* State when disconnecting ATT channel */
#define GATT_CHANNEL_STATE_ATT_DISCONNECTING        7

typedef struct _GattChannel GattChannel;
typedef struct _GattChannelCallbackEvent GattChannelCallbackEvent;
typedef struct _GattChannelHandler GattChannelHandler;

typedef U8 GattChannelCallbackType;
#define GATTC_CHANNEL_CALLBACK_CONNECTED                       1
#define GATTC_CHANNEL_CALLBACK_DISCONNECTED                 2
#define GATTC_CHANNEL_CALLBACK_DISCOVER_COMPLETED     3

struct _GattChannelCallbackEvent{
    GattChannelCallbackType eType;      /* Event causing callback */
    BtErrorCode errCode;                   /* Error code */
    GattChannelHandler *handler;     /* Pointer to handler */
};

typedef void (*GattChannelCallBack) (const GattChannelCallbackEvent*);

struct _GattChannelHandler
{
    ListEntry                   node;         /* For internal stack use only. */
    GattChannelCallBack callback;
    GattRole                   role;
    
    /* === Internal use only === */    
    GattChannel  *channel;
};

typedef struct _GattService
{    
    U8 uuid_len;
    U8 uuid[16];
    U16 startHandle;
    U16 endHandle;
    U8 is_primary;
}GattService;

/* For each bearer, allocate a channel context */
struct _GattChannel
{
    ListEntry         node;
    /* state represent the state of the channel */
    GattChannelState state;
    GattErrorCode errCode;

    /* === Internal use only === */
    U32 flag;
    BtRemoteDevice *link;
    BD_ADDR     addr;
    CmgrHandler cmgr_handler;
    BtGattHandler gatt_handler;
    ListEntry handlerList;
    /* To hold the list to prevent some one release handler when try to release all handler */
    ListEntry releasingHandlerList;
    /* Kepp all remote primary services */
    /* 0 : not discovered, 1 : discovery ongoing, 2 : discovery completed */
    U8 discovery_state;
    U8 primary_service_len;
    U8 secondary_service_len;
    U8 service_explore_idx;
    U8 *query_handle;
    GattService service[BTBM_ADP_MAX_SDAP_UUID_NO];
};

/* Gatt Channe interface */
GattChannel *findExistChannel(const BD_ADDR *addr);
U8 GattIsChannelDiscovered(GattChannel *channel);
void GattcReleaseChannel(GattChannelHandler *handler);
BtStatus GattFindAndAllocChannel(const BD_ADDR *addr, GattChannelHandler *handler);

#endif
