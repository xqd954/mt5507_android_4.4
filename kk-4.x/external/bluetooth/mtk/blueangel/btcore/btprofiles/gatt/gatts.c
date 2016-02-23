/*****************************************************************************
 *
 * Filename:
 * ---------
 * gatts.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth GATT Server Profile.
 *
 * Author:
 * -------
 * SH Lai
 *
 ****************************************************************************/
#if defined(__BT_GATTS_PROFILE__)

#include "attdb.h"
#include "attdbmain.h"
#include "gatti.h"
#include "gattci.h"
#include "gattsi.h"
#include "gattcb.h"

extern void GattcReleaseChannel(GattChannelHandler *handler);
extern U32 BtbmAdvLeGetAdvIndex(void);
extern void BT_GattUnregisterConnectedEvent(BtGattHandler *hdl);
extern U8 getValueNodeByHandle(BleServiceDB *db, ATT_HANDLE handle, ListEntry **node);
extern void BT_ATTHandleApplicationResponse(BtRemoteDevice *link, U8 error_code, U8 *data, U16 length);
extern BtStatus GattDisconnect(BtRemoteDevice *link);
extern const BD_ADDR gatts_fake_addr;

#if XA_CONTEXT_PTR == XA_ENABLED
static GattsContext      tempContext;
GattsContext          *gatts_ctx = &tempContext;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
GattsContext           gatts_ctx;
#define GATTSC(s)  (gatts_ctx.s)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

static U8 updateServerScaningState(U8 force);
static void gattsChannelCallback(const GattChannelCallbackEvent *event);
static void gattsAdvEventCallback(const BtEvent *event);
static void gattsAPCallback(void *handler, BtGattEvent *event);
static void gattsIndicationCallback(void *userdata, BtGattEvent *event);
static void gattsNotificationCallback(void *userdata, BtGattEvent *event);

/* BG connection utilities */
static GattsBgConn *checkServerUserBgConn(GattsUserContext *user, BD_ADDR *addr){
    ListEntry *bg = NULL;
    bg = GetNextNode(&user->bgConnList);
    while(bg != &user->bgConnList){
        if(addr == NULL || OS_MemCmp(((GattsBgConn*)bg)->addr.addr, 6, addr->addr, 6)){
            return (GattsBgConn*)bg;
        }
        bg = GetNextNode(bg);
    }
    return NULL;
}

static GattsBgConn *checkServerBgConnection(BD_ADDR *bdaddr){
    ListEntry *user = &GATTSC(userList);
    GattsBgConn *bg = NULL;
    while((user = GetNextNode(user)) != &GATTSC(userList)){
        bg = checkServerUserBgConn((GattsUserContext*)user, bdaddr);
        if(bg){
            return bg;
        }
    }
    return bg;
}

static void initializeBgConn(GattsBgConn *conn){
    conn->user = NULL;
}

static void freeBgConn(GattsBgConn *conn){
}

static GattsBgConn *GattsAllocBgConn(GattsUserContext *user, BD_ADDR *addr){
    GattsBgConn *conn = NULL;
    GATTS_DBG("GattsAllocBgConn(user=%p, addr=%p)", user, addr);
    if( user && addr ){
        if(!IsListEmpty(&GATTSC(freeBgConnList))){
            conn = (GattsBgConn*)RemoveHeadList(&GATTSC(freeBgConnList));
            initializeBgConn(conn);
            conn->user = user;
            conn->addr = *addr;
            InsertTailList(&user->bgConnList, &conn->node);
            updateServerScaningState(0);
        }else{
            GATTC_ERR("Run out of conn context");
        }
    }
    return conn;
}

static void GattsFreeBgConn(GattsBgConn *conn){
    if(conn){
        RemoveEntryList(&conn->node);
        freeBgConn(conn);
        InsertTailList(&GATTSC(freeBgConnList), &conn->node);
        updateServerScaningState(0);
    }
}

static U8 updateServerScaningState(U8 force){
    if( !checkServerBgConnection(NULL)){
        BtBmLeAdvDisabled(GATTSC(leScanIdx), &GATTSC(leAdvHandler));
        return 0;
    }else{
        BtBmLeAdvEnabled(GATTSC(leScanIdx), &GATTSC(leAdvHandler));
        return 1;
    }
}
static GattsUserContext *getUserContextByHandle(GattChannelHandler *handler){
    GattsConnection *conn = ContainingRecord(handler,GattsConnection,channelHandler);
    return conn->user;
}

static void initializeConn(GattsConnection *conn){
    conn->channel = NULL;
    conn->user = NULL;
    conn->channelHandler.callback = gattsChannelCallback;
}

static void freeConn(GattsConnection *conn){
    ListEntry *entry;
    if(conn->channel){
        GattcReleaseChannel(&conn->channelHandler);
    }
}

static GattsConnection *GattsAllocConn(GattsUserContext *user){
    ListEntry *entry = NULL;
    //if( GATTC_IS_VALID_USER(user) ){
        if(!IsListEmpty(&GATTSC(freeConnList))){
            entry = RemoveHeadList(&GATTSC(freeConnList));
            initializeConn((GattsConnection*)entry);
            ((GattsConnection*)entry)->user = user;
            InsertTailList(&user->connList, entry);
        }else{
            GATTS_ERR("Run out of conn context");
        }
    //}else{
    //    GATTC_ERR("Invalid user %p", user);
    //}
    return (GattsConnection*)entry;
}

static void GattsFreeConn(GattsConnection *conn){
    if(conn){
        RemoveEntryList(&conn->node);
        freeConn(conn);
        InsertTailList(&GATTSC(freeConnList), &conn->node);
    }
}

static void initializeUserContext(GattsUserContext *user){
    if(user){
        InitializeListHead(&user->connList);
        InitializeListHead(&user->bgConnList);
        OS_MemSet(user->prepare_count, 0, sizeof(user->prepare_count));
    }
}
static void freeUserContext(GattsUserContext *user){
    ListEntry *entry = NULL;
    if(user){
        /* Free conn list */
        while((entry = GetHeadList(&user->connList)) != &user->connList){
            GattsFreeConn((GattsConnection*)entry);
        }
        /* Free bg conn list */
        while((entry = GetHeadList(&user->bgConnList)) != &user->bgConnList){
            GattsFreeBgConn((GattsBgConn*)entry);
        }
    }
}

static GattsUserContext *GattsAllocUser(){
    ListEntry *entry = NULL;
    if( !IsListEmpty(&GATTSC(freeUserList)) ){
        entry = RemoveHeadList(&GATTSC(freeUserList));
        InitializeListHead(entry);
        initializeUserContext((GattsUserContext*)entry);
        InsertTailList(&GATTSC(userList), entry);
    }else{
        GATTS_ERR("Run out of user context");
    }
    return (GattsUserContext*)entry;
}

static void GattsFreeUser(GattsUserContext *user){
    if(GATTS_IS_VALID_USER(user)){
        RemoveEntryList(&user->node);
        freeUserContext(user);
        InsertTailList(&GATTSC(freeUserList), &user->node);
    }
}

static BtStatus gattsConnect(GattsUserContext *user, BD_ADDR *addr, U8 is_direct, GattsConnection **pConn){
    BtStatus status = BT_STATUS_INVALID_PARM;
    GattsConnection *conn = NULL;
    GattChannel *channel = NULL;
    if(GATTS_IS_VALID_USER(user) && addr){
        channel = findExistChannel(addr);
        if(channel || is_direct){
            conn = GattsAllocConn(user);
            if(conn){
                status = GattFindAndAllocChannel(addr, &conn->channelHandler);
                if(status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING){
                    GattsFreeConn(conn);
                    conn = NULL;
                }else{
                    conn->channel = conn->channelHandler.channel;
                }
                if(pConn) *pConn = conn;
            }
        }else{
            // TODO: Implement background connection here
            if( !checkServerUserBgConn(user, addr) ){
                if( !GattsAllocBgConn(user,addr) ){
                    status = BT_STATUS_NO_RESOURCES;
                }else{
                    /* Set pending to indicate BG connection is pended */
                    status = BT_STATUS_PENDING;
                }
            }
        }
    }else{
        status = BT_STATUS_INVALID_PARM;
    }
    GATTS_DBG("gattsConnect return %d", status);
    return status;
}

static GattsConnection *findConnByConnId(GattsUserContext *user, U32 conn_idx){
    ListEntry *entry = &user->connList;
    GattsConnection *conn;
    while((entry = GetNextNode(entry)) != &user->connList){
        conn = (GattsConnection*)entry;
        if( conn_idx == BT_ATT_GetConId(conn->channel->link) ){
            return conn;
        }        
    }
    return NULL;
}

/**************************************************
*   GATTS API
***************************************************/
void GATTS_Init(void)
{
    U32 i;
    OS_LockStack();

#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8 *)gatts_ctx, 0, sizeof(GattsContext));
#else /* XA_CONTEXT_PTR == XA_ENABLED */
    OS_MemSet((U8 *)&gatts_ctx, 0, sizeof(GattsContext));
#endif /* XA_CONTEXT_PTR == XA_ENABLED */
    
    /* Initialize User Lists */
    InitializeListHead(&GATTSC(userList));
    InitializeListHead(&GATTSC(freeUserList));
    for(i = 0;i < sizeof(GATTSC(userContext))/sizeof(GATTSC(userContext)[0]);i++){
        InsertHeadList(&GATTSC(freeUserList), &GATTSC(userContext)[i].node);
    }
    /* Initialize bg conn List */
    InitializeListHead(&GATTSC(freeBgConnList));
    for(i = 0;i < sizeof(GATTSC(bgConn))/sizeof(GATTSC(bgConn)[0]);i++){
        InsertHeadList(&GATTSC(freeBgConnList), &GATTSC(bgConn)[i].node);
    }

    /* Initialize conn List */
    InitializeListHead(&GATTSC(freeConnList));    
    for(i = 0;i < sizeof(GATTSC(connContext))/sizeof(GATTSC(connContext)[0]);i++){
        InsertHeadList(&GATTSC(freeConnList), &GATTSC(connContext)[i].node);
    }

    GATTSC(leScanIdx) = BtbmAdvLeGetAdvIndex();
    GATTS_DBG("Get le scan index %u", (unsigned int)GATTSC(leScanIdx));
    GATTSC(leAdvHandler).callback = gattsAdvEventCallback;
    GATTSC(leAdvHandler).whiteList = NULL;
    GATTSC(leAdvHandler).whiteListNo = 0;
    GATTSC(leAdvHandler).blackList = NULL;
    GATTSC(leAdvHandler).blackListNo = 0;

    BT_GattRegisterConnectedEvent(&GATTSC(gatt_handler), gattsAPCallback);

    OS_MemSet(GATTSC(att_conn_mask), 0, sizeof(GATTSC(att_conn_mask)));
    OS_MemSet(GATTSC(att_conn), 0, sizeof(GATTSC(att_conn)));
        
    OS_UnlockStack();
}

void GATTS_Deinit(void){
    OS_LockStack();
    BT_GattUnregisterConnectedEvent(&GATTSC(gatt_handler));
    OS_UnlockStack();
}

BtStatus GATTS_Register(GATTS_REG_HANDLE *handle, GattsCallback cb){
    BtStatus status = BT_STATUS_SUCCESS;
    OS_LockStack();
    GATTS_DBG("+ GATTS_Register : handle=%p", handle);
    if(handle && cb){
        /* Allocate user context from freeUserList */
        *handle = GattsAllocUser();
        if(*handle == NULL){
            status = BT_STATUS_NO_RESOURCES;
        }else{
            ((GattsUserContext*)(*handle))->callback = cb;
        }
    }else{
        GATTS_ERR("Invalid parameter handle=%p, cb=%p", handle, cb);
        status = BT_STATUS_INVALID_PARM;
    }
    GATTS_DBG("- GATTS_Register : status=%d, handle=%p", status, handle ? *handle : 0);
    OS_UnlockStack();
    return status;	
}

void GATTS_ReportAllConnectedLink(GATTS_REG_HANDLE handle){
    GattsUserContext *user = (GattsUserContext*)handle;
    GattsCallbackParms cbParam;
    U32 i;

    cbParam.event = GATTS_EVENT_CONNECTED_IND;
    cbParam.handle = handle;
    cbParam.result = 0;
    for(i = 0; i< MAX_ATT_CON_NO; i++){
        if(OS_TestBit(i, GATTSC(att_conn_mask))){
            cbParam.param.connected_ind.connHdl = (GATTS_CONN_HANDLE)i;
            cbParam.param.connected_ind.bdaddr = &GATTSC(att_conn)[i].link->bdAddr;
            user->callback(user, &cbParam);
        }
    }
}

BtStatus GATTS_Deregister(GATTS_REG_HANDLE *handle){
    BtStatus status = BT_STATUS_SUCCESS;
    OS_LockStack();
    GATTS_DBG("+ GATTS_Deregister : handle=%p", handle);
    GattsFreeUser((GattsUserContext*)handle);
    GATTS_DBG("- GATTS_Deregister : status=%d, handle=%p", status, handle ? *handle : 0);
    OS_UnlockStack();
    return status;	
}

BtStatus GATTS_Open(GATTS_REG_HANDLE handle, BD_ADDR *addr, U32 is_direct, GATTS_CONN_HANDLE *connHdl){
    BtStatus status = BT_STATUS_FAILED;
    GattsUserContext *user = (GattsUserContext*)handle;
    GattsConnection *pConn = NULL;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, addr != 0);
    OS_LockStack();
    GATTS_DBG("+ GATTS_Open : user=%p, addr=%p", user, addr);
    status = gattsConnect(user, addr, is_direct, &pConn);
    if(status == BT_STATUS_SUCCESS){
        *connHdl = (GATTC_CONN_HANDLE)BT_ATT_GetConId(pConn->channel->link);
    }
    GATTS_DBG("- GATTS_Open : status=%d", status);
    OS_UnlockStack();
    return status;
}

BtStatus GATTS_Close(GATTS_REG_HANDLE regHdl, GATTS_CONN_HANDLE connHdl){
    GattsConnection *conn = NULL;
    BtStatus status = BT_STATUS_FAILED;
    
    OS_LockStack();
    conn = findConnByConnId((GattsUserContext*)regHdl, (U32)connHdl);
    if(conn){
        GattsFreeConn(conn);
    }else{
        BtRemoteDevice *remDev = GATTSC(att_conn)[(U32)connHdl].link;
        if(remDev){
            /* To be same with google default behavior. Disconnect att channel. */
            status = GattDisconnect(remDev);
        }    
        GATTS_ERR("Can not find conn with ATT conn idx %p", connHdl);
    }
    OS_UnlockStack();
    return status;
}

BtStatus GATTS_Indication(GATTS_REG_HANDLE regHdl, GATTS_CONN_HANDLE connHdl, ATT_HANDLE handle, U8 need_confirm, ATT_VALUE *value){
    BtStatus status = BT_STATUS_SUCCESS;
    GattsUserContext *user = (GattsUserContext*)regHdl;
    //BT_ATT_GetConCtx(BtRemoteDevice * link)
    GattcOp *op = gattcAllocGattOp(regHdl);
    if(op){
        op->context = user;
        if(need_confirm){
            op->op.apCb = gattsIndicationCallback;
            op->parm.indication.valueIndications.handle = handle;
            op->parm.indication.valueIndications.size = value->len;
            op->parm.indication.valueIndications.data = op->parm.indication.value;
            OS_MemCopy(op->parm.indication.valueIndications.data, value->value, value->len);
            op->op.parms.p.valueIndications = &op->parm.indication.valueIndications;
            status = BT_GATT_Indications(GATTSC(att_conn)[(U32)connHdl].link, &op->op);
        }else{
            op->op.apCb = gattsNotificationCallback;
            op->parm.notification.valueNotifications.handle = handle;
            op->parm.notification.valueNotifications.size = value->len;
            op->parm.notification.valueNotifications.data = op->parm.notification.value;
            OS_MemCopy(op->parm.notification.valueNotifications.data, value->value, value->len);
            op->op.parms.p.valueNotifications = &op->parm.notification.valueNotifications;
            status = BT_GATT_Notifications(GATTSC(att_conn)[(U32)connHdl].link, &op->op);            
        }
    }else{
        status = BT_STATUS_NO_RESOURCES;
    }
    return status;
}

BtStatus GATTS_Response(GATTS_REG_HANDLE regHdl, GATTS_CONN_HANDLE connHdl, ATT_HANDLE handle, U8 result, ATT_VALUE *value){
    BleServiceDB *db = NULL;
    U32 conn_id = (U32)connHdl;
    ListEntry *node = NULL;
    BtRemoteDevice *remDev = GATTSC(att_conn)[conn_id].link;
    bt_att_op op = BT_ATT_GetConCurrOpcode(remDev);
    U8 *node_value_len = NULL;
    U8 **node_value = NULL;

    OS_Report("GATTS_Response(hdl=%p, conn=%p, handle=%u, result=%u, value_len=%u)",
                        regHdl, connHdl, handle, result, value->len);
    OS_Report("op=%d", op);
    if(result == 0){
        switch(op){
        case ATT_OP_READ_MULTIPLE:
        case ATT_OP_READ_BY_TYPE:
            db = BleFindServiceDBbyAddr(&gatts_fake_addr);
            switch(getValueNodeByHandle(db, handle, &node)){
            case 1:
                /* Char value */
                {
                    BleCharNode *char_node = (BleCharNode*)node;
                    node_value_len = &char_node->characteristic.value_size;
                    node_value = &char_node->characteristic.value;
                    char_node->characteristic.value_size = value->len;
                    char_node->characteristic.value = value->value;
                }
                break;
            case 2:
                /* Descr */
                {
                    BleCharDescNode *desc_node = (BleCharDescNode*)node;
                    node_value_len = &desc_node->descriptor.desc_size;
                    node_value = &desc_node->descriptor.desc;
                    desc_node->descriptor.desc_size = value->len;
                    desc_node->descriptor.desc = value->value;
                }
                break;
            case 0:
            default:
                return BT_STATUS_FAILED;
            }
            break;
        case ATT_OP_READ:
        case ATT_OP_READ_LONG:
        case ATT_OP_WRITE:
        case ATT_OP_SIGNED_WRITE:
        case ATT_OP_PREPARE_WRITE:
        case ATT_OP_EXECUTE_WRITE:
            break;
        default:
            GATTS_WARN("Invalid op %u", op);
            return BT_STATUS_FAILED;
        }
    }
    BT_ATTHandleApplicationResponse(remDev, result, value->value, value->len);
    if(node_value_len != NULL){
       *node_value_len  = 0;
       *node_value = NULL;
    }
    return BT_STATUS_SUCCESS;
}

/*******************************************************
*    Callback functions
*******************************************************/
static void gattsChannelCallback(const GattChannelCallbackEvent *event){
    GattChannelHandler *handler = event->handler;
    GattChannel *channel = handler->channel;
    GattsConnection *conn = ContainingRecord(handler,GattsConnection,channelHandler);
    GattsUserContext *user = getUserContextByHandle(handler);
    GattsCallbackParms params;
    
    switch(event->eType){
    case GATTC_CHANNEL_CALLBACK_CONNECTED:
        /*
        params.event = GATTS_EVENT_CONNECTED_IND;
        params.handle = user;
        params.result = event->errCode;
        params.param.connected_ind.connHdl = (GATTS_CONN_HANDLE)BT_ATT_GetConId(channel->link);
        params.param.connected_ind.bdaddr = &channel->addr;
        user->callback(user, &params);
        */
        GATTS_DBG("Ignoe this event, due to server will always report device connected ind.");
        break;
    case GATTC_CHANNEL_CALLBACK_DISCONNECTED:
        params.event = GATTS_EVENT_DISCONNECTED_IND;
        params.handle = user;
        params.result = 0;
        params.param.disconnected_ind.connHdl = (GATTS_CONN_HANDLE)BT_ATT_GetConId(channel->link);
        params.param.disconnected_ind.bdaddr = &channel->addr;
        GattsFreeConn(conn);
        user->callback(user, &params);
        break;
    default:
        GATTS_WARN("Invalid channel callback event");
        break;
    }
}


static void gattsAdvEventCallback(const BtEvent *event){
    BtStatus status;
    U8 count = 0;
    ListEntry *user = &GATTSC(userList);    
    GattsBgConn *bg = NULL;
    BtInquiryResult *inqResult = (BtInquiryResult *)&event->p.inqResult;

    /* update BG connection list */
    while((user = GetNextNode(user)) != &GATTSC(userList)){
        bg = checkServerUserBgConn((GattsUserContext*)user, &inqResult->bdAddr);
        if(bg){
            status = gattsConnect((GattsUserContext*)user, &inqResult->bdAddr, 1, NULL);
            GattsFreeBgConn(bg);
        }
    }
}

static void gattsAPCallback(void *handler, BtGattEvent *event){
    ListEntry *user = NULL;
    GattsBgConn *bg = NULL;
    GattsCallbackParms cbParam;
    U8 conn_idx = BT_ATT_GetConId(event->link);

    OS_Report("gattsAPCallback : eType=%u conn_idx=%u", event->eType, conn_idx);
    if(conn_idx >= MAX_ATT_CON_NO){
        return;
    }
    user = &GATTSC(userList);
    switch(event->eType){
    case BT_GATT_APP_EVENT_CONNECTED:
        OS_Report("BT_GATT_APP_EVENT_CONNECTED : userList=%p, user=%p", &GATTSC(userList), user);
        /* Keep the link info */
        OS_SetBit(conn_idx, GATTSC(att_conn_mask));
        GATTSC(att_conn)[conn_idx].link = event->link;
        cbParam.event = GATTS_EVENT_CONNECTED_IND;
        
        while((user = GetNextNode(user)) != &GATTSC(userList)){
            OS_Report("user=%p", user);
            cbParam.handle = user;
            cbParam.result = 0;
            cbParam.param.connected_ind.bdaddr = &event->link->bdAddr;
            cbParam.param.connected_ind.connHdl = (GATTS_CONN_HANDLE)BT_ATT_GetConId(event->link);
            ((GattsUserContext*)user)->callback(user, &cbParam);
            bg = checkServerUserBgConn((GattsUserContext*)user, &event->link->bdAddr);
            if(bg){
                GattsFreeBgConn(bg);
            }
        }
        break;
    case BT_GATT_APP_EVENT_DISCONNECTED:
        /* Keep the link info */
        GATTSC(att_conn_mask)[conn_idx/8] &= ~(1<<(conn_idx%8));
        GATTSC(att_conn)[conn_idx].link = NULL;
        
        cbParam.event = GATTS_EVENT_DISCONNECTED_IND;
        while((user = GetNextNode(user)) != &GATTSC(userList)){
            cbParam.handle = user;
            cbParam.result = 0;
            cbParam.param.disconnected_ind.bdaddr = &event->link->bdAddr;
            cbParam.param.disconnected_ind.connHdl = (GATTS_CONN_HANDLE)BT_ATT_GetConId(event->link);
            ((GattsUserContext*)user)->callback(user, &cbParam);
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
        GATTS_WARN("Unknown GATT event : %d", event->eType);
        break;
    }
}

static void gattsIndicationCallback(void *userdata, BtGattEvent *event){
    GattcOp *op = (GattcOp *)userdata;
    BtGattValueIndicationsResultEvent *result;
    GattsUserContext *user = (GattsUserContext*)op->context;
    GattsCallbackParms cbParam;

    result = (BtGattValueIndicationsResultEvent *) event->parms;

    cbParam.event = GATTS_EVENT_CONFIRMATION_RESULT;
    cbParam.handle = user;
    switch (result->flag)
    {
    case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
        cbParam.result = 0;
        cbParam.param.confirmation_result.handle = op->op.parms.p.valueIndications->handle;
        cbParam.param.confirmation_result.connHdl = (GATTS_CONN_HANDLE)BT_ATT_GetConId(event->link);
        user->callback(user, &cbParam);
        break;
    case BT_GATT_EVENT_RESULT_ERROR:
    case BT_GATT_EVENT_RESULT_DISCONNECT:
        cbParam.result = 1;
        user->callback(user, &cbParam);        
        break;
    default:
        GATTC_WARN("Invalid flag : %u", result->flag);
        break;
    }
    gattcFreeOp(op);
}

static void gattsNotificationCallback(void *userdata, BtGattEvent *event){
    GattcOp *op = (GattcOp *)userdata;
    BtGattValueIndicationsResultEvent *result = (BtGattValueIndicationsResultEvent *) event->parms;;
    GattsUserContext *user = (GattsUserContext*)op->context;

    switch (result->flag)
    {
    case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
        break;
    case BT_GATT_EVENT_RESULT_ERROR:
    case BT_GATT_EVENT_RESULT_DISCONNECT:
        break;
    default:
        GATTC_WARN("Invalid flag : %u", result->flag);
        break;
    }
    gattcFreeOp(op);
}

#endif
