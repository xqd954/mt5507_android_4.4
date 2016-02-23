/*****************************************************************************
 *
 * Filename:
 * ---------
 * gattc_channel.c
 *
 * Project:
 * --------
 *   Bluetooth GATT-Based profiles
 *
 * Description:
 * ------------
 *   This file is channel manipulation for MTK Bluetooth GATT-based (Client) Profiles.
 *
 * Author:
 * -------
 * SH Lai
 *
 ****************************************************************************/
#if defined(__BT_GATTC_PROFILE__)

#include "me.h"
#include "attdb.h"
#include "attdbmain.h"
#include "gattci.h"
#include "gattcb.h"

extern BtStatus GattClientConnect(BtRemoteDevice *link);
extern void BT_GattUnregisterConnectedEvent(BtGattHandler *hdl);
extern BtStatus GattDisconnect(BtRemoteDevice *link);

/*************************************************
*    Macros
*************************************************/
//typedef U8 GatcChannelFlag;
//#define GATTC_CHANNEL_FLAG_DISCOVERED                        1

typedef U8 GattChannelEventType;
/* Trigger channel to connect after allocated */
#define GATT_CHANNEL_EVENT_INIT_START                0
/* Indicate ACL channel is disconnected */
#define GATT_CHANNEL_EVENT_ACL_DISCONNECTED   1
/* Indicate ACL channel is connected */
#define GATT_CHANNEL_EVENT_ACL_CONNECTED         2
/* Indicate ATT channel is disconnected */
#define GATT_CHANNEL_EVENT_ATT_DISCONNECTED   3
/* Indicate ATT channel is connected */
#define GATT_CHANNEL_EVENT_ATT_CONNECTED         4
/* Indicate ATT channel is connected */
#define GATT_CHANNEL_EVENT_DISCOVER_COMPLETE 5
/* Used for force disconnect channel for error cases */
#define GATT_CHANNEL_EVENT_FORCE_TERMINATE     6
/* Used for release channel when no handler registered */
#define GATT_CHANNEL_EVENT_RELEASE                      7

#define GetChannelErrorCode(ch) ((ch)->errCode)
//#define GATTC_FALG_SET(ch, mask) (ch->flag |= mask)
//#define GATTC_FALG_CLEAR(ch, mask) (ch->flag &= ~mask)

/*************************************************
*    Structures
*************************************************/
typedef struct _GattChannelEvent{
    GattChannelEventType event;
    union{
    /* GATT_CHANNEL_EVENT_ACL_CONNECTED */
    /* GATT_CHANNEL_EVENT_ATT_CONNECTED */
    /* GATT_CHANNEL_EVENT_FORCE_TERMINATE */
    GattErrorCode errCode;
    }param;
}GattChannelEvent;


/*************************************************
*    Static utility functions
*************************************************/
static void GattServiceConnectionCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status);
static void gattAPCallBack(void *handler, BtGattEvent *event);
static void GattQueryServiceDefinitionCB(void *context, BtStatus status);
static void gattServiceSearchAllCallBack(void *userdata, BtGattEvent *event);

static GattChannelState channelStateMachine(GattChannel *channel, GattChannelEvent *event);


static void initializeChannel(GattChannel *channel){
    OS_MemSet(channel, 0, sizeof(GattChannel));
    InitializeListHead(&channel->handlerList);
    InitializeListHead(&channel->releasingHandlerList);
}

static void freeChannel(GattChannel *channel){    
}

GattChannel *findExistChannel(const BD_ADDR *addr){
    ListEntry *head = NULL;
    ListEntry *entry = NULL;
    BtRemoteDevice *remDev = NULL;

    GATTC_DBG("findExistChannel");
    head = &GATTCC(channelList);
    Assert(IsListCircular(head));
    remDev = ME_FindRemoteDevice(addr);
    for(entry = GetNextNode(head); entry != head; entry = GetNextNode(entry)){
        if(((GattChannel*)entry)->link == remDev){
            GATTC_DBG("Find channel %p", entry);
            return (GattChannel*)entry;
        }
    }
    GATTC_DBG("Can not find channel");
    return NULL;
}

static BtErrorCode translateGattErrorCode(GattErrorCode err){
    BtErrorCode btErr = BEC_NO_ERROR;
    switch(err){
    case GATT_ERR_SUCCESS:
        break;
    case GATT_ERR_FAIL:
        btErr = BEC_UNSPECIFIED_ERR;
    case GATT_ERR_ACL_FAIL:
        btErr = BEC_CONN_ESTABLISH_FAIL;
    case GATT_ERR_ATT_FAIL:
        btErr = BEC_CONN_ESTABLISH_FAIL;
    case GATT_ERR_USER_TERMINATE:
        btErr = BEC_LOCAL_TERMINATED;
        break;
    default:
        GATTC_WARN("Unspecified error code %u", err);
        btErr = BEC_UNSPECIFIED_ERR;
        break;
    }
    return btErr;
}

static void discoverComplete(GattChannel *channel, GattErrorCode err){
    GattChannelEvent event;
    event.event = GATT_CHANNEL_EVENT_DISCOVER_COMPLETE;
    event.param.errCode = err;
    channelStateMachine(channel, &event);
}

static BtStatus searchAllPrimaryService(GattChannel *channel)
{
    BtStatus status = BT_STATUS_FAILED;
    GattcOp *op = NULL;
    BtGattDiscoveryAllPrimaryService *parm = NULL;

    OS_Report("searchAllPrimaryService");
    if (channel->state == GATT_CHANNEL_STATE_ATT_CONNECTED)
    {
        op = gattcAllocGattOp(channel);
        if(op == NULL){
            return BT_STATUS_NO_RESOURCES;
        }
        OS_MemSet((U8*) channel->service, 0, sizeof(GattService)*BTBM_ADP_MAX_SDAP_UUID_NO);
        InitializeListEntry(&op->op.op.node);
        op->op.apCb = gattServiceSearchAllCallBack; //btbmGattServiceSearchAllCallBack;
        op->context = channel;
        parm = &op->parm.discService;
        parm->startHandle = 0x0001;
        parm->endHandle = 0xFFFF;
        op->op.parms.p.allPrimaryService = parm;
        /* Discover GAP service handle range */
        BT_GATT_DiscoveryAllPrimaryService(channel->link, &op->op);
        /*
        op->disconnectTimer.func = 0;
        EVM_ResetTimer(&op->disconnectTimer);
        op->disconnectTimer.func = btbmGattServiceSearchTimeout;
        op->disconnectTimer.context = (void *)op;
        EVM_StartTimer(&op->disconnectTimer, 10000);
        */
        status = BT_STATUS_PENDING;
    }
    else
    {
        GATTC_ERR("Fail searching service due to wrong channel state %u", channel->state);
        //status = BT_STATUS_FAILED;
    }
    return status;
}


BtStatus discoverPrimaryService(GattChannel *channel){
    BtStatus status = BT_STATUS_NOT_FOUND;
    GattcOp *op = NULL;
    if( GATTC_IS_VALID_CHANNEL(channel) ){
        status = searchAllPrimaryService(channel);
    }
    //status = BLE_QueryServiceDefinitionByUuid(channel, channel->link, GattQueryServiceDefinitionCB, U8 uuid_size, U8 * uuid, U8 * * handle_out)
    return status;
}

/* 0 : not added, others : new added */
static U8 addInclService(GattChannel *channel, GattService *serv){
    U8 i = 0;
    U8 total = channel->primary_service_len + channel->secondary_service_len;
    GattService *existedServ = &channel->service[0];
    
    Assert(total <= BTBM_ADP_MAX_SDAP_UUID_NO);
    if(total < BTBM_ADP_MAX_SDAP_UUID_NO){
        for(;i < total; i++, existedServ++){
            if(gattc_uuid_compare(serv->uuid_len, serv->uuid, existedServ->uuid_len, existedServ->uuid)
                && (serv->startHandle != existedServ->startHandle)){
                break;
            }
        }
        if(i == total){
            *existedServ = *serv;
            return 1;
        }
    }
    return 0;
}

static void explorerIncludeService(GattChannel *channel){
    GattService *serv;
    GattService incl_serv;
    BleService *bleServ;
    BleIncludeService *inclServ;
    U8 idx = channel->service_explore_idx;
    U8 uuid_size = 0;
    U8 *uuid = NULL;

    serv = &channel->service[idx];
    bleServ = BLE_FindServiceByUuid(&channel->addr, serv->uuid_len, serv->uuid);
    if(bleServ == NULL){
        GATTC_WARN("BLE_FindServiceByUuid failed");
        return;
    }
    inclServ = BLE_FindIncludeServiceByUuid(bleServ, 0, NULL); 
    while(inclServ){
        incl_serv.uuid_len =  inclServ->uuid_size;
        if(incl_serv.uuid_len == 2){
            OS_MemCopy(incl_serv.uuid, inclServ->uuid.uuid16, 2);
        }else if(incl_serv.uuid_len == 16){
            OS_MemCopy(incl_serv.uuid, inclServ->uuid.uuid128, 16);
        }else{
            GATTC_WARN("Invalid uuid size %u", incl_serv.uuid_len);
            continue;
        }
        incl_serv.startHandle = inclServ->include_service_handle;
        incl_serv.endHandle = inclServ->end_group_handle;
        incl_serv.is_primary = 0;
        addInclService(channel, &incl_serv);
        inclServ = BLE_FindNextIncludeServiceByUuid(bleServ, inclServ, 0, NULL);
    };
}

static BtStatus exploreService(GattChannel *channel){
    BtStatus status = BT_STATUS_SUCCESS;
    GattService *serv = NULL;
    
    while(channel->service_explore_idx < (channel->primary_service_len + channel->secondary_service_len)){
        serv = &channel->service[channel->service_explore_idx];
        status = BLE_QueryServiceDefinition(channel, 
                                                                 channel->link, 
                                                                 GattQueryServiceDefinitionCB, 
                                                                 serv->uuid_len,
                                                                 serv->uuid, 
                                                                 serv->startHandle,
                                                                 serv->endHandle,
                                                                 serv->is_primary,
                                                                 &channel->query_handle);
        if(status == BT_STATUS_SUCCESS){
            explorerIncludeService(channel);
            channel->service_explore_idx++;
        }else{
            break;
        }
    }
    if(status == BT_STATUS_SUCCESS){
        discoverComplete(channel, GATT_ERR_SUCCESS);
    }else if(status != BT_STATUS_PENDING){
        discoverComplete(channel, GATT_ERR_DISCOVER_FAIL);
    }
    return status;
}

static BtStatus startExploreService(GattChannel *channel){
    BtStatus status = BT_STATUS_NOT_FOUND;
    if( GATTC_IS_VALID_CHANNEL(channel) ){
        channel->service_explore_idx = 0;
        status = exploreService(channel);
    }
    return status;
}

static void callbackChannelEvent(GattChannel *channel, GattChannelCallbackType type){
    //GattChannelHandler *handler = NULL;
    ListEntry *handlerList = &channel->handlerList;
    ListEntry *handler = NULL;
    GattChannelCallbackEvent event;
    U32 remove = 0;

    GATTC_DBG("callbackChannelEvent(channel=%p, type=%u)", channel, type);
    event.eType = type;
    switch(type){
    case GATTC_CHANNEL_CALLBACK_CONNECTED:
        handler = &channel->handlerList;
        event.errCode = translateGattErrorCode(channel->errCode);
        break;
    case GATTC_CHANNEL_CALLBACK_DISCONNECTED:
        remove = 1;
        MoveList(&channel->releasingHandlerList, &channel->handlerList);
        handlerList = &channel->releasingHandlerList;
        event.errCode = translateGattErrorCode(channel->errCode);
        break;
    case GATTC_CHANNEL_CALLBACK_DISCOVER_COMPLETED:
        handler = &channel->handlerList;
        event.errCode = translateGattErrorCode(channel->errCode);
        break;
    default:
        GATTC_ERR("Invalid channel event type : %u", type);
        return;
    }

    if(remove){
        while((handler = RemoveHeadList(handlerList)) != handlerList){
            /* Callback */
            event.handler = (GattChannelHandler*)handler;
            if(((GattChannelHandler*)handler)->callback){
                ((GattChannelHandler*)handler)->callback(&event);
            }
        };
    }else{
        handler = GetNextNode(handlerList);
        while(handler != handlerList){
            event.handler = (GattChannelHandler*)handler;
            if(((GattChannelHandler*)handler)->callback){
                ((GattChannelHandler*)handler)->callback(&event);
            }
            handler = GetNextNode(handler);
        };
    }
}

static U8 checkNeedDiscoverService(GattChannel *channel){
    ListEntry *entry = &channel->handlerList;
    GattChannelHandler *handler;
    while((entry = GetNextNode(entry)) != &channel->handlerList){
        handler = (GattChannelHandler*)entry;
        if(handler->role == GATT_ROLE_CLIENT){
            return 1;
        }
    }
    return 0;
}

static GattChannelState channelStateMachine(GattChannel *channel, GattChannelEvent *event){
    U8 registered = 0;
    BtStatus status = BT_STATUS_SUCCESS;
    GattChannelState state;
    
    if(GATTC_IS_VALID_CHANNEL(channel)){
        /* If handlerList is not empty or event is init start that means someone need this channel */
        if(event->event == GATT_CHANNEL_EVENT_INIT_START || !IsListEmpty(&channel->handlerList)){
            registered = 1;
        }else{
            registered = 0;
        }
        /* Keep original state */
        state = channel->state;
        GATTC_DBG("channelStateMachine : channel(%p) state %u. event=%u, registered=%u", channel, channel->state, event->event, registered);
        switch(event->event){
        case GATT_CHANNEL_EVENT_INIT_START:
            channel->state = GATT_CHANNEL_STATE_DISCONNECTED;
            status = CMGR_RegisterHandler(&channel->cmgr_handler, GattServiceConnectionCallback);
            if(status == BT_STATUS_SUCCESS){
                status = CMGR_CreateDataLink(&channel->cmgr_handler, &channel->addr);
                channel->link = CMGR_GetRemoteDevice(&channel->cmgr_handler);
                if(status == BT_STATUS_SUCCESS){
                    event->param.errCode = GATT_ERR_SUCCESS;
                    goto ACL_CONNECTED;
                }else if(status != BT_STATUS_PENDING){
                    channel->errCode = GATT_ERR_ACL_FAIL;
                    CMGR_DeregisterHandler(&channel->cmgr_handler);
                    channel->state = GATT_CHANNEL_STATE_DISCONNECTED;
                }else{
                    channel->state = GATT_CHANNEL_STATE_ACL_CONNECTING;
                }
            }
            break; 
        case GATT_CHANNEL_EVENT_ACL_DISCONNECTED:
            /* Shall not receive this event. */
            GATTC_WARN("GATT_CHANNEL_EVENT_ACL_DISCONNECTED");
            channel->errCode = GATT_ERR_ACL_FAIL;
            channel->state = GATT_CHANNEL_STATE_DISCONNECTED;
            CMGR_DeregisterHandler(&channel->cmgr_handler);
            /* Seemed no ATT_DISCONNECTED received if connecting ATT failed. */
            BT_GattUnregisterConnectedEvent(&channel->gatt_handler);
            break; 
        case GATT_CHANNEL_EVENT_ACL_CONNECTED:
            if(channel->errCode == GATT_ERR_SUCCESS){
                /* if really ACL connected event and channel has no ERROR */
                channel->errCode = event->param.errCode;
            }
ACL_CONNECTED:
            if(registered && channel->errCode == GATT_ERR_SUCCESS){
                BT_GattRegisterConnectedEvent(&channel->gatt_handler, gattAPCallBack);
                status = GattClientConnect(CMGR_GetRemoteDevice(&channel->cmgr_handler));
                if(status == BT_STATUS_SUCCESS){
                    goto ATT_CONNECTED;
                }else if(status != BT_STATUS_PENDING){
                    channel->errCode = GATT_ERR_ATT_FAIL;
                    BT_GattUnregisterConnectedEvent(&channel->gatt_handler);
                    CMGR_RemoveDataLink(&channel->cmgr_handler);
                    CMGR_DeregisterHandler(&channel->cmgr_handler);
                    channel->state = GATT_CHANNEL_EVENT_ACL_DISCONNECTED;                    
                }else{
                    channel->state = GATT_CHANNEL_STATE_ATT_CONNECTING;
                }
            }else{
                BT_GattUnregisterConnectedEvent(&channel->gatt_handler);
                CMGR_RemoveDataLink(&channel->cmgr_handler);
                CMGR_DeregisterHandler(&channel->cmgr_handler);
                channel->state = GATT_CHANNEL_EVENT_ACL_DISCONNECTED;
            }
            break;
        case GATT_CHANNEL_EVENT_ATT_DISCONNECTED:
            GATTC_DBG("GATT_CHANNEL_EVENT_ATT_DISCONNECTED : state=%u", channel->state);
            if(channel->state == GATT_CHANNEL_STATE_ATT_CONNECTED){
                /* If previous is ATT connected not ATT disconnecting. Set Error to GATT_ERR_FAIL */
                channel->errCode = GATT_ERR_FAIL;
            }
            BT_GattUnregisterConnectedEvent(&channel->gatt_handler);
            goto ACL_CONNECTED;
        case GATT_CHANNEL_EVENT_ATT_CONNECTED:
ATT_CONNECTED:
            GATTC_DBG("GATT_CHANNEL_EVENT_ATT_CONNECTED : state=%u", channel->state);
            //Assert(channel->state == GATT_CHANNEL_STATE_ATT_CONNECTING);
            if(registered && GetChannelErrorCode(channel) == GATT_ERR_SUCCESS){
                /* Callback connected event */
                channel->state = GATT_CHANNEL_STATE_ATT_CONNECTED;
                callbackChannelEvent(channel, GATTC_CHANNEL_CALLBACK_CONNECTED);
                if(checkNeedDiscoverService(channel)){
                    /* Need discover */
                    status = discoverPrimaryService(channel);
                    if(status == BT_STATUS_SUCCESS){
                        channel->discovery_state = 2;
                    }else if(status == BT_STATUS_PENDING){
                        channel->discovery_state = 1;
                    }else{
                        channel->discovery_state = 0;
                        channel->errCode = GATT_ERR_DISCOVER_FAIL;
                        goto ATT_CONNECTED;
                    }
                }
            }else{
                status = GattDisconnect(channel->link);
                if(status != BT_STATUS_PENDING){
                    /* Back to ACL connected */
                    channel->state = GATT_CHANNEL_STATE_ACL_CONNECTED;
                    goto ACL_CONNECTED;
                }else{
                    channel->state = GATT_CHANNEL_STATE_ATT_DISCONNECTING;
                }
            }
            break;
        case GATT_CHANNEL_EVENT_DISCOVER_COMPLETE:
            GATTC_DBG("GATT_CHANNEL_EVENT_DISCOVER_COMPLETE : state=%u, err=%u", 
                                channel->state,
                                event->param.errCode);
            if(channel->errCode == GATT_ERR_SUCCESS){
                channel->errCode = event->param.errCode;
                if(channel->errCode != GATT_ERR_SUCCESS){
                    channel->discovery_state = 0;
                    goto ATT_CONNECTED;
                }else{                    
                    channel->discovery_state = 2;
                    callbackChannelEvent(channel, GATTC_CHANNEL_CALLBACK_DISCOVER_COMPLETED);
                }
            }
            break;
        case GATT_CHANNEL_EVENT_FORCE_TERMINATE:
            channel->errCode = GATT_ERR_USER_TERMINATE;
        case GATT_CHANNEL_EVENT_RELEASE:
            if(channel->state == GATT_CHANNEL_STATE_ATT_CONNECTED){
                /* go to ATT connected to continue disconnecting */
                goto ATT_CONNECTED;
            }else if(channel->state == GATT_CHANNEL_STATE_ACL_CONNECTED){
                /* go to ACL connected to continue disconnecting */
                goto ACL_CONNECTED;
            }
            break;
        default:
            GATTC_ERR("Invalid channel state");
            break;
        }

        /* Handler when leave state */
        if(state != channel->state && state == GATT_CHANNEL_STATE_ATT_CONNECTED){
            /* Leave ATT connected state */
            //GATTC_FALG_CLEAR(channel, GATTC_CHANNEL_FLAG_DISCOVERED);
            channel->discovery_state = 0;
        }

        state = channel->state;
        /* Handler when entering state */        
        if(channel->state == GATT_CHANNEL_STATE_DISCONNECTED || GetChannelErrorCode(channel) != GATT_ERR_SUCCESS){
            callbackChannelEvent(channel, GATTC_CHANNEL_CALLBACK_DISCONNECTED);
        }

        if(channel->state == GATT_CHANNEL_STATE_DISCONNECTED){
            GATTC_DBG("Channel state is disconnected. Insert to free list");
            Assert(IsNodeOnList(&GATTCC(channelList), &channel->node));
            Assert(!IsNodeOnList(&GATTCC(freeChannelList), &channel->node));
            RemoveEntryList(&channel->node);
            InsertHeadList(&GATTCC(freeChannelList), &channel->node);
            Assert(IsListEmpty(&channel->handlerList));
        }
        return state;
    }
    return GATT_CHANNEL_STATE_INITED;
}

/*************************************************
*   GATTC channel API
*************************************************/
U8 GattIsChannelDiscovered(GattChannel *channel){
    if(GATTC_IS_VALID_CHANNEL(channel)){
        if(channel->state == GATT_CHANNEL_STATE_ATT_CONNECTED && channel->discovery_state == 2){
            return 1;
        }
    }else{
        GATTC_ERR("Invalid channel %p", channel);
    }
    return 0;
}

BtStatus GattFindAndAllocChannel(const BD_ADDR *addr, GattChannelHandler *handler){
    BtStatus status = BT_STATUS_SUCCESS;
    GattChannelState newState = GATT_CHANNEL_STATE_INITED;
    GattChannel *channel = NULL;
    GattChannelEvent event;
    GattChannelCallBack callback;
    
    if(addr && handler){
        channel = findExistChannel(addr);
        if(!channel){
            if(!IsListEmpty(&GATTCC(freeChannelList))){
                channel= (GattChannel*)RemoveHeadList(&GATTCC(freeChannelList));
                initializeChannel(channel);
                channel->addr = *addr;
                InsertHeadList(&GATTCC(channelList), &channel->node);
                /* Sent event to trigger channel state machine */ 
                event.event = GATT_CHANNEL_EVENT_INIT_START;
                event.param.errCode = GATT_ERR_SUCCESS;
                newState = channelStateMachine(channel, &event);
                if(newState == GATT_CHANNEL_STATE_DISCONNECTED){
                    GATTC_ERR("Create channel failed. ");
                    status = BT_STATUS_FAILED;
                 } 
            }else{
                GATTC_ERR("Run out of GATTC channel");
                status = BT_STATUS_NO_RESOURCES;
            }
        }else{
            if(GetChannelErrorCode(channel) != GATT_ERR_SUCCESS){
                GATTC_WARN("Channel is closing due to error. Reject alloc");
                status = BT_STATUS_IN_PROGRESS;
            }
        }
        if(status == BT_STATUS_SUCCESS && channel){
            handler->channel = channel;
            /* Remove callback to prevent disconnect ind */
            callback = handler->callback;
            handler->callback = NULL;
            InsertTailList(&channel->handlerList, &handler->node);
            if(channel->state != GATT_CHANNEL_STATE_ATT_CONNECTED){
                /* set status to pending */
                status = BT_STATUS_PENDING;
            }else{
                if(channel->discovery_state == 0){
                    /* let state machine to trigger */
                    event.event = GATT_CHANNEL_EVENT_ATT_CONNECTED;
                    event.param.errCode = GATT_ERR_SUCCESS;
                    newState = channelStateMachine(channel, &event);
                    if(newState != GATT_CHANNEL_STATE_ATT_CONNECTED){
                        /* discovery failed */
                        status = BT_STATUS_FAILED;
                    }
                }
            }
            handler->callback = callback;
        }

        //if(status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING){
            //if( channel && IsNodeOnList(&GATTCC(channelList), &channel->node) ){
            //    GATTC_DBG("Create channel failed, reinsert to free channel list");                
            //    RemoveEntryList(&channel->node);
            //    InsertHeadList(&GATTCC(freeChannelList), &channel->node);
            //}
        //}
    }else{
        GATTC_ERR("Invalid parameter");
        status = BT_STATUS_INVALID_PARM;
    } 
    return status;
}

void GattcFreeChannel(GattChannel *channel, GattErrorCode reason){
    if(GATTC_IS_VALID_CHANNEL(channel)){
        GattChannelEvent event;
        event.event = GATT_CHANNEL_EVENT_FORCE_TERMINATE;
        event.param.errCode = reason;
        if(reason == GATT_ERR_SUCCESS){
            /* Error code shall not be success, set it to user terminate */
            event.param.errCode = GATT_ERR_USER_TERMINATE;
        }else{
            event.param.errCode = reason;
        }
        channelStateMachine(channel, &event);
    }else{
        GATTC_ERR("Try to free invalid channel(%p)", channel);
    }
}

void GattcReleaseChannel(GattChannelHandler *handler){
    GattChannel *channel;
    GattChannelEvent event;
    GATTC_DBG("GattcReleaseChannel(handler=%p)", handler);
    if(handler){
        channel = handler->channel;
        if(GATTC_IS_VALID_CHANNEL(handler->channel)){
            if(IsNodeOnList(&channel->handlerList, &handler->node)){
                handler->channel = NULL;
                RemoveEntryList(&handler->node);
                if(IsListEmpty(&channel->handlerList)){
                    event.event = GATT_CHANNEL_EVENT_RELEASE;
                    event.param.errCode = GATT_ERR_SUCCESS;
                    channelStateMachine(channel,&event);
                }else{
                    GATTC_DBG("channel->handlerList is not empty");
                }
            }else if(IsNodeOnList(&channel->releasingHandlerList, &handler->node)){
                GATTC_DBG("Handler is on releaseHandlerList");
                handler->channel = NULL;
                RemoveEntryList(&handler->node);
            }else{
                GATTC_WARN("channel(%p)->hold_user is 0 when free connection(%p)", channel, handler);
            }
        }else{
            GATTC_ERR("channel(%p) in handler(%p) invalid", channel, handler);
        }
    }else{
        GATTC_WARN("Release NULL Handler");
    }
}

/*********************************************
*    Static callback functions
**********************************************/
static void GattServiceConnectionCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status){
    GattChannel *Channel = ContainingRecord(Handler, GattChannel, cmgr_handler);

    GATTC_DBG("GattServiceConnectionCallback. event=%u, status=%d", Event, Status);
    if(GATTC_IS_VALID_CHANNEL(Channel)){
        GattChannelEvent event;
        switch (Event) 
        {
        case CMEVENT_DATA_LINK_CON_CNF:
            event.event = GATT_CHANNEL_EVENT_ACL_CONNECTED;
            if (Status == BT_STATUS_SUCCESS){
                event.param.errCode = GATT_ERR_SUCCESS;
            }else{
                event.param.errCode = GATT_ERR_ACL_FAIL;
            }
            channelStateMachine(Channel, &event);
            break;

        case CMEVENT_DATA_LINK_DIS:
            event.event = GATT_CHANNEL_EVENT_ACL_DISCONNECTED;
            channelStateMachine(Channel, &event);
            break;
        default:
            GATTC_WARN("Unexpected CMGR event : %d", Event);
            break;
        }
    }else{
        GATTC_ERR("Invalid Channel (%p)", Channel);
    }
}

static void gattAPCallBack(void *handler, BtGattEvent *event){
    GattChannel *channel = ContainingRecord(handler, GattChannel, gatt_handler);
    GattChannelEvent chEvent;
    
    if(GATTC_IS_VALID_CHANNEL(channel)){
        switch(event->eType){
        case BT_GATT_APP_EVENT_CONNECTED:
            if(event->link == channel->link){
                chEvent.event = GATT_CHANNEL_EVENT_ATT_CONNECTED;
                chEvent.param.errCode = GATT_ERR_SUCCESS;
                channelStateMachine(channel, &chEvent);
            }
            break;
        case BT_GATT_APP_EVENT_DISCONNECTED:
            if(event->link == channel->link){
                chEvent.event = GATT_CHANNEL_EVENT_ATT_DISCONNECTED;
                chEvent.param.errCode = GATT_ERR_ATT_FAIL;
                channelStateMachine(channel, &chEvent);
            }
            break;
        case BT_GATT_APP_EVENT_VALUE_NOTIFICATION:
            break;
        case BT_GATT_APP_EVENT_VALUE_INDICATION:
            break;
        case BT_GATT_APP_EVENT_WRITE_REQUEST:
            break;
        case BT_GATT_APP_EVENT_WRITE_COMMAND:
            break;
        case BT_GATT_APP_EVENT_PREPARE_WRITE_REQUEST:
            break;
        case BT_GATT_APP_EVENT_EXECUTE_WRITE_REQUEST:
            break;
        case BT_GATT_APP_EVENT_SERVER:
            break;
        default:
            GATTC_WARN("Unknown GATT event : %d", event->eType);
            break;
        }
    }else{
        GATTC_ERR("Invalid channel(%p)", channel);
    }
}

static void GattQueryServiceDefinitionCB(void *context, BtStatus status){
    GattChannel *channel = (GattChannel*)context;
    GattChannelEvent event;
    if(GATTC_IS_VALID_CHANNEL(channel)){
        event.event = GATT_CHANNEL_EVENT_DISCOVER_COMPLETE;
        event.param.errCode = GATT_ERR_SUCCESS;
        if(status == BT_STATUS_SUCCESS){
            explorerIncludeService(channel);
        }           
        channel->service_explore_idx++;
        status = exploreService(channel);        
    }else{
        GATTC_ERR("GattQueryServiceDefinitionCB : Invalid channel %p", channel);
    }
}

static void gattServiceSearchAllCallBack(void *userdata, BtGattEvent *event){
    GattcOp *op =  ((GattcOp *)userdata);
    GattChannel *channel = NULL;
    BtRemoteDevice *link = NULL;
    GattService *serv = NULL;

    Report(("gattServiceSearchAllCallBack():%d", event->eType));
    
    if (op == NULL){
        return;
    }else{
        channel = (GattChannel*)op->context;
        if(!GATTC_IS_VALID_CHANNEL(channel)){
            GATTC_ERR("gattServiceSearchAllCallBack: channel invalid %p", channel);
            return;
        }else{
            link = channel->link;
            switch(event->eType){
            case BT_GATT_OPERATOR_DISCOVERY_ALL_PRIMARY_SERVICE:
            {
                /* GAP service handle range discovered, then read value */
                BtGattDiscoveryAllPrimaryServiceResultEvent *result = event->parms;
                    
                GATTC_DBG("result->flag:%d, handle 0x%x~0x%x, type=%d, uuid2=0x%x", 
                        result->flag, result->startHandle, result->endHandle, result->type, result->uuid2);
                    if (result->flag == BT_GATT_EVENT_RESULT_CONTINUE_DATA)
                    {
                        if (result->type == BT_UUID16_SIZE){
                            serv = &channel->service[channel->primary_service_len];
                            channel->service[channel->primary_service_len].uuid_len = 2;
                            StoreLE16(channel->service[channel->primary_service_len].uuid, result->uuid2);
                        }else if (result->type == BT_UUID128_SIZE){
                            serv = &channel->service[channel->primary_service_len];
                            serv->uuid_len = 16;
                            OS_MemCopy(channel->service[channel->primary_service_len].uuid, result->uuid16, 16);
                        }else{
                            Assert(0);
                        }
                        if(serv){
                            serv->is_primary = 1;
                            serv->startHandle = result->startHandle;
                            serv->endHandle = result->endHandle;
                            channel->primary_service_len++;
                        }
                    }
                    else
                    {
                        GATTC_DBG("Discovery all primary service done : %u primary services", channel->primary_service_len);
                        goto search_end;
                    }
                    break;
                }
            default:
                break;
            }
            return;
        }
    }

search_end:
    if(op){
        gattcFreeOp(op);
    }
    if(GATTC_IS_VALID_CHANNEL(channel)){
        BtStatus status;
        GattChannelEvent channel_event;        
        channel_event.event = GATT_CHANNEL_EVENT_DISCOVER_COMPLETE;
        if(channel->primary_service_len){
            status = startExploreService(channel);
        }else{
            GATTC_WARN("No primary service found.");
            discoverComplete(channel, GATT_ERR_DISCOVER_FAIL);
        }
    }
}
#endif /* __BT_GATTC_PROFILE__ */
