/*****************************************************************************
 *
 * Filename:
 * ---------
 * gattc.c
 *
 * Project:
 * --------
 *   Bluetooth GATT-Based profiles
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth GATT-based (Client) Profiles.
 *
 * Author:
 * -------
 * SH Lai
 *
 ****************************************************************************/
#if defined(__BT_GATTC_PROFILE__)

#include "attdb.h"
#include "attdbmain.h"
#include "gattci.h"
#include "gatt_cl.h"
#include "gattcb.h"
#include "me.h"
#include "btutils.h"
#include "ble_utils.h"
#include "medev.h"
#include "btbm_int.h"

extern U32 BtbmAdvLeGetAdvIndex(void);
extern BtStatus ME_ReadRSSI(BtRemoteDevice *remDev);
extern void BT_GattUnregisterConnectedEvent(BtGattHandler *hdl);
extern U8 getValueNodeByHandle(BleServiceDB *db, ATT_HANDLE handle, ListEntry **node);
/* ALPS01112663 */
extern void BTBMGapDiscoveryResultCfm(BOOL is_update,
                               btbm_bd_addr_t bd_addr,
#ifdef __BT_4_0_BLE__
                               btbm_device_type device_type,
#endif
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
                                  kal_uint8 rssi, kal_uint8 eir_supported, kal_uint8 uuid_list_supported, kal_uint8 servlist_completed,
                                  kal_uint32 *uuid_list,
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
                                  kal_uint32 cod, kal_uint8 name_len, kal_uint8 *name);



#if XA_CONTEXT_PTR == XA_ENABLED
static GattcContext      tempContext;
GattcContext          *gattc_ctx = &tempContext;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
GattcContext           gattc_ctx;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

const U8  bluetooth_base_uuid[16] = 
/*
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00,
    0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
};
*/
{
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*************************************************
*    Internal utilities
*************************************************/
static U8 updateScaningState(void);
static void gattcAdvEventCallBack(const BtEvent *event);
static void gattcReadCharvalueCallback(void *userdata, BtGattEvent *event);
void gattcWriteCharvalueCallback(void *userdata, BtGattEvent *event);
void gattcWriteDescrCallback(void *userdata, BtGattEvent *event);
void gattcReadDescrCallback(void *userdata, BtGattEvent *event);
void gattcExecWriteCallback(void *userdata, BtGattEvent *event);
static void GattcFreeConn(GattcConnection *conn);
static void meCallBack(const BtEvent *Event);
static void reportConnected(const GattcUserContext *user, const GattcConnection *conn);
static void GattcAPCallBack(void *handler, BtGattEvent *event);
/*************************************************
*    Macros
*************************************************/
/*************************************************
*    Static utility functions
*************************************************/
/* BG connection utilities */
/* if addr == NULL, return first available bg connection */
static GattcBgConn *checkUserBgConn(GattcUserContext *user, const BD_ADDR *addr){
    ListEntry *bg = NULL;

    GATTC_DBG("checkUserBgConn : user=%p, addr=%p", user, addr);
    if(addr) BTBMDumpBdAddr(addr->addr);
    if( !GATTC_IS_VALID_USER(user) ){
        GATTC_ERR("Invalid user context %p", user);
        return NULL;
     }

    bg = GetNextNode(&user->bgConnList);
    while(bg != &user->bgConnList){
        GATTC_DBG("bg(%p)", bg);
        BTBMDumpBdAddr(((GattcBgConn*)bg)->addr.addr);
        if(addr == NULL || OS_MemCmp(((GattcBgConn*)bg)->addr.addr, 6, addr->addr, 6)){
            GATTC_DBG("bg(%p) found", bg);
            return (GattcBgConn*)bg;
        }
        bg = GetNextNode(bg);
    }
    GATTC_DBG("No bg found");
    return NULL;
}

static GattcRegNotification *checkUserRegNoti(GattcUserContext *user, const BD_ADDR *addr, const GATT_SVC_UUID *servUuid, const ATT_UUID *char_uuid){
    ListEntry *entry = NULL;
    GattcRegNotification *reg = NULL;

    if( !GATTC_IS_VALID_USER(user) ){
        GATTC_ERR("Invalid user context %p", user);
        return NULL;
    }
    if(!addr || !servUuid || !char_uuid){
        return NULL;
    }
    
    entry = GetHeadList(&user->regNotiList);
    Assert(IsListCircular(&user->regNotiList));
    OS_Report("checkUserRegNoti : user=%p, &user->regNotiList=%p", user, &user->regNotiList);
    while(entry != &user->regNotiList){
        reg = (GattcRegNotification*)entry;
        if(OS_MemCmp(((GattcRegNotification*)reg)->addr.addr, 6, addr->addr, 6) &&
            BLE_CompareUuid(servUuid->uuid.uuid.uuid, servUuid->uuid.uuid.len,
                                          reg->servUuid.uuid.uuid.uuid, reg->servUuid.uuid.uuid.len) &&
            reg->servUuid.uuid.inst == servUuid->uuid.inst &&
            BLE_CompareUuid(char_uuid->uuid.uuid, char_uuid->uuid.len,
                                          reg->char_uuid.uuid.uuid, reg->char_uuid.uuid.len) &&
            reg->char_uuid.inst == char_uuid->inst){
            return (GattcRegNotification*)reg;
        }
        entry = GetNextNode(entry);
    }
    return NULL;
}

static void initializeRegNoti(GattcRegNotification *regNoti){
}

static void freeRegNoti(GattcRegNotification *regNoti){
}

static GattcRegNotification *GattcAllocRegNoti(GattcUserContext *user, BD_ADDR *addr, GATT_SVC_UUID *servUuid, ATT_UUID *char_uuid){
    //ListEntry *entry = NULL;
    GattcRegNotification *regNoti = NULL;
    if( GATTC_IS_VALID_USER(user) && addr && servUuid && char_uuid){
        if(!IsListEmpty(&GATTCC(freeRegNotiList))){
            regNoti = (GattcRegNotification*)RemoveHeadList(&GATTCC(freeRegNotiList));
            initializeRegNoti(regNoti);
            regNoti->addr = *addr;
            regNoti->servUuid = *servUuid;
            regNoti->char_uuid = *char_uuid;
            InsertTailList(&user->regNotiList, &regNoti->node);
        }else{
            GATTC_ERR("Run out of reg notification");
        }
    }else{
        GATTC_ERR("Invalid parameters user=%p, addr=%p, svc=%p, char=%p", user, addr, servUuid, char_uuid);
    }
    return regNoti;
}

static void GattcFreeRegNoti(GattcRegNotification *regNoti){
    if(regNoti){
        RemoveEntryList(&regNoti->node);
        freeRegNoti(regNoti);
        InsertTailList(&GATTCC(freeRegNotiList), &regNoti->node);
    }
}

static void initializeBgConn(GattcBgConn *conn){
    conn->user = NULL;
}

static void freeBgConn(GattcBgConn *conn){
}

static GattcBgConn *GattcAllocBgConn(GattcUserContext *user, const BD_ADDR *addr){
    GattcBgConn *conn = NULL;
    GATTC_DBG("GattcAllocBgConn(user=%p, addr=%p)", user, addr);
    if( GATTC_IS_VALID_USER(user) && addr ){
        if(!IsListEmpty(&GATTCC(freeBgConnList))){
            conn = (GattcBgConn*)RemoveHeadList(&GATTCC(freeBgConnList));
            initializeBgConn(conn);
            conn->user = user;
            OS_MemCopy(conn->addr.addr, addr->addr, 6);
            InsertTailList(&user->bgConnList, &conn->node);
            updateScaningState();
        }else{
            GATTC_ERR("Run out of conn context");
        }
    }else{
        GATTC_ERR("Invalid user %p", user);
    }
    return conn;
}

static void GattcFreeBgConn(GattcBgConn *conn){
    if(conn){
        RemoveEntryList(&conn->node);
        freeBgConn(conn);
        InsertTailList(&GATTCC(freeBgConnList), &conn->node);
        updateScaningState();
    }
}

static void GattcFreeBgConnByAddr(GattcUserContext *user, BD_ADDR *addr){
    GattcBgConn *bg = checkUserBgConn(user, addr);
    if(bg){
        GattcFreeBgConn(bg);
    }
}

static BleService *findServiceBySvcUuid(BD_ADDR *bdaddr, GATT_SVC_UUID *svc_uuid){
    U8 inst = svc_uuid->uuid.inst;
    BleService *pServ = BLE_FindServiceByUuid(bdaddr, svc_uuid->uuid.uuid.len, svc_uuid->uuid.uuid.uuid);
    while(pServ && inst){
        pServ = BLE_FindNextServiceByUuid(bdaddr, pServ, svc_uuid->uuid.uuid.len, svc_uuid->uuid.uuid.uuid);
        inst--;
    }
    GATT_ASSERT(pServ);
    return pServ;
}

static U8 getServiceInst(const BD_ADDR *bdaddr, BleService *pServ){
    U8 inst = 0;
    BleService *pServInst = BLE_FindServiceByUuid(bdaddr, pServ->uuid_size, pServ->uuid.uuid128);
    while(pServ != pServInst){
        pServInst = BLE_FindNextServiceByUuid(bdaddr, pServInst, pServ->uuid_size, pServ->uuid.uuid128);
        inst++;
    }
    return inst;
}

static BleIncludeService *findInclServiceBySvcUuid(BleService *pServ, GATT_SVC_UUID *incl_svc_uuid){
    U8 inst = incl_svc_uuid->uuid.inst;
    BleIncludeService *pInclServ = BLE_FindIncludeServiceByUuid(pServ, incl_svc_uuid->uuid.uuid.len, incl_svc_uuid->uuid.uuid.uuid);
    while(pInclServ && inst){
        pInclServ = BLE_FindNextIncludeServiceByUuid(pServ, pInclServ, incl_svc_uuid->uuid.uuid.len, incl_svc_uuid->uuid.uuid.uuid);
        inst--;
    }
    return pInclServ;
}

static U8 getInclServiceInst(BleService *pServ, BleIncludeService *pInclServ){
    U8 inst = 0;
    BleIncludeService *pInclServInst = BLE_FindIncludeServiceByUuid(pServ, pInclServ->uuid_size, pInclServ->uuid.uuid128);
    while(pInclServ != pInclServInst){
        pInclServInst = BLE_FindNextIncludeServiceByUuid(pServ, pInclServInst, pInclServ->uuid_size, pInclServ->uuid.uuid128);
        inst++;
    }
    return inst;
}

static BleChar *findCharByUuid(BleService *pServ, ATT_UUID *char_uuid){
    U8 inst = char_uuid->inst;
    BleChar *pChar = BLE_FindCharByUuid(pServ, char_uuid->uuid.len, char_uuid->uuid.uuid);
    OS_Report("findCharByUuid : pServ=%p, char_uuid=%p, inst=%u", pServ, char_uuid, inst);
    while(pChar && inst){
        pChar = BLE_FindNextCharByUuid(pServ, pChar, char_uuid->uuid.len, char_uuid->uuid.uuid);
        inst--;
    }
    GATT_ASSERT(pChar);
    return pChar;
}

static U8 getCharInst(BleService *pServ, BleChar *pChar){
    U8 inst = 0;
    BleChar *pCharInst = BLE_FindCharByUuid(pServ, pChar->value_uuid_size, pChar->value_uuid.uuid128);
    while(pChar != pCharInst){
        pCharInst = BLE_FindNextCharByUuid(pServ, pCharInst, pChar->value_uuid_size, pChar->value_uuid.uuid128);
        inst++;
    }
    return inst;
}

static BleCharDesc *findDescrByUuid(BleChar *pChar, ATT_UUID *descr_uuid){
    U8 inst = descr_uuid->inst;
    BleCharDesc *pDescr = BLE_FindCharDescByUuid(pChar, descr_uuid->uuid.len, descr_uuid->uuid.uuid);
    OS_Report("findDescrByUuid : pChar=%p, descr_uuid=%p, inst=%u", pChar, descr_uuid, inst);
    while(pDescr && inst){
        pDescr = BLE_FindNextCharDescByUuid(pChar, pDescr, descr_uuid->uuid.len, descr_uuid->uuid.uuid);
        inst--;
    }
    GATT_ASSERT(pDescr);
    return pDescr;
}

static U8 getDescrInst(BleChar *pChar, BleCharDesc *pDescr){
    U8 inst = 0;
    BleCharDesc *pDescrInst = BLE_FindCharDescByUuid(pChar, pDescr->desc_uuid_size, pDescr->desc_uuid.uuid128);
    while(pDescr != pDescrInst){
        pDescrInst = BLE_FindNextCharDescByUuid(pChar, pDescrInst, pDescr->desc_uuid_size, pDescr->desc_uuid.uuid128);
        inst++;
    }
    return inst;
}

/**********************************************
*    GATTC async handlers
**********************************************/
static BtStatus asyncConnApiHandler(GattcConnection *conn, GattcReq *req){
    BtStatus status = BT_STATUS_SUCCESS;
    GattChannel *channel = conn->channel;
    GattcUserContext *user = conn->user;
    BD_ADDR *bdaddr;
    GattcCallbackParms cbParams;
    U32 connHdl = 0;    

    OS_MemSet(&cbParams, 0, sizeof(cbParams));

    if(channel){
        connHdl = BT_ATT_GetConId(channel->link); 
    }

    OS_Report("asyncConnApiHandler : type=%u", req->type);
    switch(req->type){
    case GATTC_REQ_TYPE_SERVICE_SEARCH:
        if(GattIsChannelDiscovered(channel)){
            BleService *pServ = NULL, *pServInst = NULL;
            U8 inst = 0;

            OS_Report("GATTC_REQ_TYPE_SERVICE_SEARCH : start");
            BLE_VerifyDB(&channel->link->bdAddr);
            
            cbParams.event = GATTC_EVENT_SEARCH_SERVICE_RESULT;
            cbParams.handle = user;
            cbParams.result = 0; /* Success */
            //cbParams.param.search_service_result.connHdl = conn;
            cbParams.param.search_service_result.connHdl = (GATTC_CONN_HANDLE)connHdl;
            bdaddr = &conn->channel->addr;
            pServ = BLE_FindServiceByUuid(bdaddr, 0, NULL);
            while(pServ){
                /* Counting instance */
                inst = getServiceInst(bdaddr, pServ);
                cbParams.param.search_service_result.svc_uuid.uuid.uuid.len = pServ->uuid_size;
                if(pServ->uuid_size == 2){
                    OS_MemCopy(cbParams.param.search_service_result.svc_uuid.uuid.uuid.uuid, pServ->uuid.uuid16, pServ->uuid_size);
                }else{
                    OS_MemCopy(cbParams.param.search_service_result.svc_uuid.uuid.uuid.uuid, pServ->uuid.uuid128, pServ->uuid_size);
                }
                cbParams.param.search_service_result.svc_uuid.uuid.inst = inst;
                // TODO: Set correct is_primary value
                cbParams.param.search_service_result.svc_uuid.is_primary = 1;
                user->callback(user, &cbParams);
                pServ = BLE_FindNextServiceByUuid(bdaddr, pServ, 0, NULL);
            }
            cbParams.event = GATTC_EVENT_SEARCH_SERVICE_COMPLETE_IND;
            cbParams.param.search_service_complete_ind.connHdl = (GATTC_CONN_HANDLE)connHdl;             
            user->callback(user, &cbParams);
            OS_Report("GATTC_REQ_TYPE_SERVICE_SEARCH : end");
            BLE_VerifyDB(&channel->link->bdAddr);            
        }else{
            status = BT_STATUS_BUSY;
        }
        break;
    case GATTC_REQ_TYPE_INCL_SERVICE_SEARCH:
        if(GattIsChannelDiscovered(channel)){
            BleService *pServ = NULL;
            BleIncludeService *pInclServ = NULL;
            GattcReqParamInclServiceSearch *param = &req->param.incl_service_search;
            U8 inst = 0;

            OS_Report("GATTC_REQ_TYPE_INCL_SERVICE_SEARCH : start");
            BLE_VerifyDB(&channel->link->bdAddr);

            cbParams.event = GATTC_EVENT_INCL_SERVICE_RESULT;
            cbParams.handle = user;
            cbParams.param.search_incl_service_result.connHdl = (GATTC_CONN_HANDLE)connHdl;              
            pServ = findServiceBySvcUuid(&param->conn->channel->addr, &param->svc_uuid);
            if(pServ){
                if(param->prev_incl_svc_uuid.uuid.uuid.len == 0){
                    /* Get first incl service */
                    pInclServ = BLE_FindIncludeServiceByUuid(pServ, 0, NULL);
                }else{
                    pInclServ = findInclServiceBySvcUuid(pServ, &param->prev_incl_svc_uuid);
                    if(pInclServ){
                        pInclServ = BLE_FindNextIncludeServiceByUuid(pServ, pInclServ, 0, NULL);
                    }
                }
            }
            if(pInclServ){
                cbParams.result = 0;
                inst = getInclServiceInst(pServ, pInclServ);
                cbParams.param.search_incl_service_result.incl_svc_uuid.uuid.uuid.len = pInclServ->uuid_size;
                if(pInclServ->uuid_size == 2){
                    OS_MemCopy(cbParams.param.search_incl_service_result.incl_svc_uuid.uuid.uuid.uuid, pInclServ->uuid.uuid16, pInclServ->uuid_size);
                }else{
                    OS_MemCopy(cbParams.param.search_incl_service_result.incl_svc_uuid.uuid.uuid.uuid, pInclServ->uuid.uuid128, pInclServ->uuid_size);
                }
            }else{
                cbParams.result = 1;
            }
            cbParams.param.search_incl_service_result.svc_uuid = param->svc_uuid;
            cbParams.param.search_incl_service_result.incl_svc_uuid.uuid.inst = inst;
            user->callback(user, &cbParams);
            OS_Report("GATTC_REQ_TYPE_INCL_SERVICE_SEARCH : end");
            BLE_VerifyDB(&channel->link->bdAddr);            
        }else{
            status = BT_STATUS_BUSY;
        }
        break;
    case GATTC_REQ_TYPE_GET_CHAR:
        if(GattIsChannelDiscovered(channel)){
            BleService *pServ = NULL;
            BleChar *pChar = NULL;
            GattcReqParamGetChar *param = &req->param.get_char;
            U8 inst = 0;

            OS_Report("GATTC_REQ_TYPE_GET_CHAR : start");
            BLE_VerifyDB(&channel->link->bdAddr);

            cbParams.event = GATTC_EVENT_GET_CHAR_RESULT;
            cbParams.handle = user;
            cbParams.param.get_char_result.connHdl = (GATTC_CONN_HANDLE)connHdl;              
            pServ = findServiceBySvcUuid(&param->conn->channel->addr, &param->svc_uuid);
            if(pServ){
                if(param->prev_char_uuid.uuid.len == 0){
                    /* Get first char */
                    pChar = BLE_FindCharByUuid(pServ, 0, NULL);
                }else{
                    pChar = findCharByUuid(pServ, &param->prev_char_uuid);
                    if(pChar){
                        pChar = BLE_FindNextCharByUuid(pServ, pChar, 0, NULL);
                    }
                }
            }
            if(pChar){
                cbParams.result = 0;
                inst = getCharInst(pServ, pChar);
                if(pChar->value_uuid_size == 2){
                    OS_MemCopy(cbParams.param.get_char_result.char_uuid.uuid.uuid, pChar->value_uuid.uuid16, pChar->value_uuid_size);
                }else{
                    OS_MemCopy(cbParams.param.get_char_result.char_uuid.uuid.uuid, pChar->value_uuid.uuid128, pChar->value_uuid_size);
                }
                cbParams.param.get_char_result.char_uuid.inst = inst;
                cbParams.param.get_char_result.properties = pChar->properties;
                cbParams.param.get_char_result.char_uuid.uuid.len = pChar->value_uuid_size;
            }else{
                cbParams.result = 1;
            }
            cbParams.param.get_char_result.svc_uuid = param->svc_uuid;
            user->callback(user, &cbParams);
            OS_Report("GATTC_REQ_TYPE_GET_CHAR : end");
            BLE_VerifyDB(&channel->link->bdAddr);            
        }else{
            status = BT_STATUS_BUSY;
        }
        break;        
    case GATTC_REQ_TYPE_GET_DESCR:
        if(GattIsChannelDiscovered(channel)){
            BleService *pServ = NULL;
            BleChar *pChar = NULL;
            BleCharDesc *pDescr = NULL;
            GattcReqParamGetDescr *param = &req->param.get_descr;
            U8 inst = 0;

            OS_Report("GATTC_REQ_TYPE_GET_DESCR : start");
            BLE_VerifyDB(&channel->link->bdAddr);

            cbParams.event = GATTC_EVENT_GET_DESCR_RESULT;
            cbParams.handle = user;
            cbParams.param.get_descr_result.connHdl = (GATTC_CONN_HANDLE)connHdl;             
            pServ = findServiceBySvcUuid(&param->conn->channel->addr, &param->svc_uuid);
            if(!pServ){
                status = BT_STATUS_NOT_FOUND;
                break;
            }
            pChar = findCharByUuid(pServ, &param->char_uuid);
            if(!pChar){
                status = BT_STATUS_NOT_FOUND;
                break;
            }            
            if(param->prev_descr_uuid.uuid.len == 0){
                /* Get first char */
                pDescr = BLE_FindCharDescByUuid(pChar, 0, NULL);
            }else{
                pDescr = findDescrByUuid(pChar, &param->prev_descr_uuid);
                if(pDescr){
                    pDescr = BLE_FindNextCharDescByUuid(pChar, pDescr, 0, NULL);
                }
            }
            if(pDescr){
                cbParams.result = 0;
                inst = getDescrInst(pChar, pDescr);
                cbParams.param.get_descr_result.descr_uuid.uuid.len = pDescr->desc_uuid_size;
                if(pDescr->desc_uuid_size == 2){
                    OS_MemCopy(cbParams.param.get_descr_result.descr_uuid.uuid.uuid, pDescr->desc_uuid.uuid16, pDescr->desc_uuid_size);
                }else{
                    OS_MemCopy(cbParams.param.get_descr_result.descr_uuid.uuid.uuid, pDescr->desc_uuid.uuid128, pDescr->desc_uuid_size);
                }
                cbParams.param.get_descr_result.descr_uuid.inst = inst;
            }else{
                cbParams.result = 1;
            }
            cbParams.param.get_descr_result.svc_uuid = param->svc_uuid;
            cbParams.param.get_descr_result.char_uuid = param->char_uuid;
            user->callback(user, &cbParams);
            OS_Report("GATTC_REQ_TYPE_GET_DESCR : end");
            BLE_VerifyDB(&channel->link->bdAddr);            
        }else{
            status = BT_STATUS_BUSY;
        }
        break;
    case GATTC_REQ_TYPE_READ_CHAR:
        if(GattIsChannelDiscovered(channel) && conn->currReq == NULL){
            BleService *pServ = NULL;
            BleChar *pChar = NULL;
            GattcReqParamReadChar *param = &req->param.read_char;
            GattcOp *op = NULL;

            pServ = findServiceBySvcUuid(&conn->channel->addr, &param->svc_uuid);
            if(!pServ){
                status = BT_STATUS_NOT_FOUND;
                break;
            }
            pChar = findCharByUuid(pServ, &param->char_uuid);
            if(!pChar){
                status = BT_STATUS_NOT_FOUND;
                break;
            }
            op = gattcAllocGattOp(conn);
            if(op != NULL){
                op->parm.readCharValue.handle = pChar->value_handle;
                op->op.parms.p.readCharValue = &op->parm.readCharValue;
                op->op.apCb = gattcReadCharvalueCallback;
                op->context = conn;
                status = BT_GATT_ReadCharacteristicValue(conn->channel->link, &op->op);
                if(status != BT_STATUS_PENDING){
                    gattcFreeOp(op);
                    status = BT_STATUS_FAILED;
                }else{
                    req->op = op;
                }
            }else{
                status = BT_STATUS_NOSERVICES;
            }
        }else{
            status = BT_STATUS_BUSY;
        }
        break;
    case GATTC_REQ_TYPE_WRITE_CHAR:
        if(GattIsChannelDiscovered(channel) && conn->currReq == NULL){
            BleService *pServ = NULL;
            BleChar *pChar = NULL;
            GattcWriteType type = GATTC_WRITE_TYPE_NO_RSP;
            GattcReqParamWriteChar *param = &req->param.write_char;
            GattcOp *op = NULL;

            pServ = findServiceBySvcUuid(&conn->channel->addr, &param->svc_uuid);
            if(!pServ){
                status = BT_STATUS_NOT_FOUND;
                break;
            }
            pChar = findCharByUuid(pServ, &param->char_uuid);
            if(!pChar){
                status = BT_STATUS_NOT_FOUND;
                break;
            }
            op = gattcAllocGattOp(conn);
            if(op != NULL){
                type = (U8)param->write_type;
                op->op.apCb = gattcWriteCharvalueCallback;
                op->context = conn;
                if(type == GATTC_WRITE_TYPE_NO_RSP){
                    op->parm.writeCharValueNoRsp.handle = pChar->value_handle;
                    op->parm.writeCharValueNoRsp.size = param->len;
                    op->parm.writeCharValueNoRsp.data = param->value;
                    op->op.parms.p.writeWithoutResponse = &op->parm.writeCharValueNoRsp;
                    status = BT_GATT_WriteWithoutResponse(conn->channel->link, &op->op);
                }else if(type == GATTC_WRITE_TYPE_PREPARE){
                    op->parm.reliableWrite.handle = pChar->value_handle;
                    op->parm.reliableWrite.size = param->len;
                    op->parm.reliableWrite.data = param->value;
                    op->parm.reliableWrite.offset = 0;
                    op->parm.reliableWrite.excuteFlag = 0; /* Do not auto execute */
                    op->op.parms.p.reliableWrite = &op->parm.reliableWrite;
                    status = BT_GATT_ReliableWrites(conn->channel->link, &op->op);                
                }else{
                    op->parm.writeCharValue.handle = pChar->value_handle;
                    op->parm.writeCharValue.size = param->len;
                    op->parm.writeCharValue.data = param->value;
                    op->op.parms.p.writeRequest = &op->parm.writeCharValue;
                    status = BT_GATT_WriteCharacteristicValue(conn->channel->link, &op->op);
                }
                if(status != BT_STATUS_PENDING){
                    gattcFreeOp(op);
                    status = BT_STATUS_FAILED;
                }else{
                    req->op = op;
                }
            }else{
                status = BT_STATUS_NOSERVICES;
            }
        }else{
            status = BT_STATUS_BUSY;
        }
        break;
    case GATTC_REQ_TYPE_READ_DESCR:
        if(GattIsChannelDiscovered(channel) && conn->currReq == NULL){
            BleService *pServ = NULL;
            BleChar *pChar = NULL;
            BleCharDesc *pDescr = NULL;
            GattcReqParamReadDescr *param = &req->param.read_descr;
            GattcOp *op = NULL;
            GATTC_DBG("GATTC_REQ_TYPE_READ_DESCR");
            pServ = findServiceBySvcUuid(&conn->channel->addr, &param->svc_uuid);
            if(!pServ){
                status = BT_STATUS_NOT_FOUND;
                break;
            }
            pChar = findCharByUuid(pServ, &param->char_uuid);
            if(!pChar){
                status = BT_STATUS_NOT_FOUND;
                break;
            }
            pDescr = findDescrByUuid(pChar, &param->descr_uuid);
            if(!pDescr){
                status = BT_STATUS_NOT_FOUND;
                break;
            }            
            op = gattcAllocGattOp(conn);
            if(op != NULL){
                op->parm.readCharDescr.handle = pDescr->handle;
                op->op.parms.p.readCharDescriptors = &op->parm.readCharDescr;
                op->op.apCb = gattcReadDescrCallback;
                op->context = conn;
                status = BT_GATT_ReadCharacteristicDescriptors(conn->channel->link, &op->op);
                if(status != BT_STATUS_PENDING){
                    gattcFreeOp(op);
                    status = BT_STATUS_FAILED;
                }else{
                    req->op = op;
                }
            }else{
                status = BT_STATUS_NOSERVICES;
            }
        }else{
            status = BT_STATUS_BUSY;
        }
        break;
    case GATTC_REQ_TYPE_WRITE_DESCR:
        if(GattIsChannelDiscovered(channel) && conn->currReq == NULL){
            BleService *pServ = NULL;
            BleChar *pChar = NULL;
            BleCharDesc *pDescr = NULL;
            GattcWriteType type;
            GattcReqParamWriteDescr *param = &req->param.write_descr;
            GattcOp *op = NULL;

            pServ = findServiceBySvcUuid(&conn->channel->addr, &param->svc_uuid);
            if(!pServ){
                status = BT_STATUS_NOT_FOUND;
                break;
            }
            pChar = findCharByUuid(pServ, &param->char_uuid);
            if(!pChar){
                status = BT_STATUS_NOT_FOUND;
                break;
            }
            pDescr = findDescrByUuid(pChar, &param->descr_uuid);
            if(!pDescr){
                status = BT_STATUS_NOT_FOUND;
                break;
            }
            op = gattcAllocGattOp(conn);
            if(op != NULL){
                type = (U8)param->write_type;
                op->op.apCb = gattcWriteDescrCallback;
                op->context = conn;
                if(type == GATTC_WRITE_TYPE_NO_RSP){
                    op->parm.writeCharValueNoRsp.handle = pDescr->handle;
                    op->parm.writeCharValueNoRsp.size = param->len;
                    op->parm.writeCharValueNoRsp.data = param->value;
                    op->op.parms.p.writeWithoutResponse = &op->parm.writeCharValueNoRsp;
                    status = BT_GATT_WriteWithoutResponse(conn->channel->link, &op->op);
                }else if(type == GATTC_WRITE_TYPE_PREPARE){
                    op->parm.reliableWrite.handle = pChar->value_handle;
                    op->parm.reliableWrite.size = param->len;
                    op->parm.reliableWrite.data = param->value;
                    op->parm.reliableWrite.offset = 0;
                    op->parm.reliableWrite.excuteFlag = 0; /* Do not auto execute */
                    op->op.parms.p.reliableWrite = &op->parm.reliableWrite;
                    status = BT_GATT_ReliableWrites(conn->channel->link, &op->op);
                }else{
                    op->parm.writeCharDescr.handle = pDescr->handle;
                    op->parm.writeCharDescr.size = param->len;
                    op->parm.writeCharDescr.data = param->value;
                    op->op.parms.p.writeDescriptors = &op->parm.writeCharDescr;
                    status = BT_GATT_WriteCharacteristicDescriptors(conn->channel->link, &op->op);
                }
                if(status != BT_STATUS_PENDING){
                    gattcFreeOp(op);
                    status = BT_STATUS_FAILED;
                }else{
                    req->op = op;
                }
            }else{
                status = BT_STATUS_NOSERVICES;
            }
        }else{
            status = BT_STATUS_BUSY;
        }        
        break;
    case GATTC_REQ_TYPE_EXEC_WRITE:
        if(GattIsChannelDiscovered(channel) && conn->currReq == NULL){
            GattcOp *op = NULL;
            op = gattcAllocGattOp(conn);
            if(op != NULL){
                GattcReqParamExecWrite *param = &req->param.exec_write;
                op->op.apCb = gattcExecWriteCallback;
                op->context = conn;
                op->parm.execWrite.execute = param->execute;
                op->op.parms.p.execWrite = &op->parm.execWrite;
                status= BT_GATT_ExecuteWrites(conn->channel->link, &op->op);
                if(status != BT_STATUS_PENDING){
                    gattcFreeOp(op);
                    status = BT_STATUS_FAILED;
                }else{
                    req->op = op;
                }
            }else{
                status = BT_STATUS_NO_RESOURCES;
            }
        }else{
            status = BT_STATUS_BUSY;
        }
        break;
    default:
        GATTC_ERR("Invalid request type %u", req->type);
        status = BT_STATUS_INVALID_TYPE;
        break;
    }
    return status;
}

static void asyncConnApiListHandler(GattcConnection *conn){
    BtStatus status;
    if(conn && conn->currReq == NULL){
        GattcReq *req = (GattcReq*)&conn->reqList;
        GattcReq *nextReq = (GattcReq *)GetNextNode(&req->node);
        GATTC_DBG("asyncConnApiListHandler(conn=%p)", conn);
        while((req = nextReq) != (GattcReq *)&conn->reqList){
            status = asyncConnApiHandler(conn, req);
            GATTC_DBG("asyncConnApiHandler(conn=%p, req=%p) return %d", conn, req, status);
            nextReq = (GattcReq *)GetNextNode(&req->node);
            switch(status){
            case BT_STATUS_PENDING:
                RemoveEntryList(&req->node);
                conn->currReq = req;
                return;
            case BT_STATUS_BUSY:
                return;
            case BT_STATUS_SUCCESS:
            default:
                RemoveEntryList(&req->node);
                gatt_free(req);
                break;
            }
        }
    }else{
        GATTC_ERR("Invalid conn %p", conn);
    }
}

static void asyncConnApiCallback(EvmTimer *timer){
    // TODO: Loop all request
    GattcConnection *conn = (GattcConnection*)timer->context;
    asyncConnApiListHandler(conn);
}

void gattcChannelCallback(const GattChannelCallbackEvent *event){
    GattChannelHandler *handler = event->handler;
    GattcConnection *conn = ContainingRecord(handler, GattcConnection, channelHandler);
    GattcCallbackParms params;
    GATTC_DBG("gattcChannelCallback : event=%u, error=%u", event->eType, event->errCode);
    switch(event->eType){
    case GATTC_CHANNEL_CALLBACK_CONNECTED:
        params.event = GATTC_EVENT_CONNECTED_IND;
        params.handle = conn->user;
        params.result = event->errCode;
        //params.param.connected_ind.connHdl = conn;
        params.param.connected_ind.connHdl = (GATTC_CONN_HANDLE)BT_ATT_GetConId(conn->channel->link);
        params.param.connected_ind.bdaddr = conn->channel->addr;
        //params.param.connected_ind.result = event->errCode;;
        conn->user->callback(conn->user, &params);
        break;
    case GATTC_CHANNEL_CALLBACK_DISCONNECTED:
        params.event = GATTC_EVENT_DISCONNECTED_IND;
        params.handle = conn->user;
        params.result = 0;
        params.param.disconnected_ind.connHdl = (GATTC_CONN_HANDLE)BT_ATT_GetConId(conn->channel->link);
        if((U32)params.param.disconnected_ind.connHdl >= MAX_ATT_CON_NO){
            params.param.disconnected_ind.connHdl = 0;
        }            
        params.param.disconnected_ind.bdaddr = conn->channel->addr;
        //params.param.disconnected_ind.result = 0;
        GattcFreeConn(conn);
        conn->user->callback(conn->user, &params);
        break;
    case GATTC_CHANNEL_CALLBACK_DISCOVER_COMPLETED:
        BLE_VerifyDB(&conn->channel->link->bdAddr);
        asyncConnApiListHandler(conn);
        break;
    default:
        GATTC_WARN("Invalid channel callback event");
        break;
    }
}

static void initializeConn(GattcConnection *conn){
    conn->channel = NULL;
    conn->user = NULL;
    conn->currReq = NULL;
    conn->channelHandler.callback = gattcChannelCallback;
    conn->asyncApiTimer.context = conn;
    conn->asyncApiTimer.func = asyncConnApiCallback;
    InitializeListHead(&conn->reqList);
}

static void freeConn(GattcConnection *conn){
    ListEntry *entry;
    if(conn->channel){
        GattcReleaseChannel(&conn->channelHandler);
    }
    if(conn->currReq){
        if(conn->currReq->op){
            /* If request ongoing, set op context to NULL to let cb know connection is already freed */
            GATTC_DBG("request ongoing when free connection. Set op context to NULL");
            conn->currReq->op->context = NULL;
        }
        gatt_free(conn->currReq);
    }
    while((entry = RemoveHeadList(&conn->reqList)) != &conn->reqList){
        gatt_free(entry);
    }
    if(!IsEntryAvailable(&conn->asyncApiTimer.node)){
        /* Timer is activate */
        EVM_CancelTimer(&conn->asyncApiTimer);
    }
}

static GattcConnection *GattcAllocConn(GattcUserContext *user){
    ListEntry *entry = NULL;
    if( GATTC_IS_VALID_USER(user) ){
        if(!IsListEmpty(&GATTCC(freeConnList))){
            entry = RemoveHeadList(&GATTCC(freeConnList));
            initializeConn((GattcConnection*)entry);
            ((GattcConnection*)entry)->user = user;
            InsertTailList(&user->connList, entry);
        }else{
            GATTC_ERR("Run out of conn context");
        }
    }else{
        GATTC_ERR("Invalid user %p", user);
    }
    return (GattcConnection*)entry;
}

static void GattcFreeConn(GattcConnection *conn){
    if(conn){
        RemoveEntryList(&conn->node);
        freeConn(conn);
        InsertTailList(&GATTCC(freeConnList), &conn->node);
    }
}

static void initializeUserContext(GattcUserContext *user){
    if(user){
        InitializeListHead(&user->connList);
        InitializeListHead(&user->bgConnList);
        InitializeListHead(&user->regNotiList);
        user->rssiDev = NULL;
    }
}
static void freeUserContext(GattcUserContext *user){
    ListEntry *entry = NULL;
    if(user){
        /* Free conn list */
        while((entry = GetHeadList(&user->connList)) != &user->connList){
            GattcFreeConn((GattcConnection*)entry);
        }
        /* Free bg conn list */
        while((entry = GetHeadList(&user->bgConnList)) != &user->bgConnList){
            GattcFreeBgConn((GattcBgConn*)entry);
        }        
        /* Free registered notification */
        while((entry = GetHeadList(&user->regNotiList)) != &user->regNotiList){
            GattcFreeRegNoti((GattcRegNotification*)entry);
        }
    }
}

static GattcUserContext *GattcAllocUser(){
    ListEntry *entry = NULL;
    if( !IsListEmpty(&GATTCC(freeUserList)) ){
        entry = RemoveHeadList(&GATTCC(freeUserList));
        InitializeListHead(entry);
        initializeUserContext((GattcUserContext*)entry);
        InsertTailList(&GATTCC(userList), entry);
    }else{
        GATTC_ERR("Run out of user context");
    }
    return (GattcUserContext*)entry;
}

static void GattcFreeUser(GattcUserContext *user){
    if(GATTC_IS_VALID_USER(user)){
        RemoveEntryList(&user->node);
        freeUserContext(user);
        InsertTailList(&GATTCC(freeUserList), &user->node);
    }
}

/************************************************
*   Internal GATTC API
*************************************************/
GattcOp *gattcAllocGattOp(void *context){
    GattcOp *op = NULL;
    if(!IsListEmpty(&GATTCC(freeOpList))){
        op = (GattcOp*)RemoveHeadList(&GATTCC(freeOpList));
        InitializeListEntry((ListEntry*)op);
        op->context = context;
    }else{
        GATTC_ERR("Out of free op");
    }
    return op;
}

void gattcFreeOp(GattcOp *op){
    if(op){
        InsertTailList(&GATTCC(freeOpList), &op->op.op.node);
    }
}

#define GATTC_VALID_UUID_LEN(len) (len == 2 || len == 16)

static void convert_uuid16_to_uuid128(U8 *uuid128, U8 *uuid16){
    if(uuid16 && uuid128){
        OS_MemCopy(uuid128, bluetooth_base_uuid, 16);
        /*
        uuid128[3] = uuid16[0];
        uuid128[2] = uuid16[1];
        */
        uuid128[13] = uuid16[1];
        uuid128[12] = uuid16[0];        
    }
}

U8 gattc_uuid_compare (U8 len1, U8 *uuid1, U8 len2, U8 *uuid2)
{
    U8  uuid128[16], *p128 = NULL, *p16 = NULL;

    if(!uuid1 || !uuid2){
        GATTC_WARN("gattc_uuid_compare : NULL uuid, uuid1=%p, uuid2=%p", uuid1, uuid2);
        return 0;
    }

    if(!GATTC_VALID_UUID_LEN(len1) || !GATTC_VALID_UUID_LEN(len2)){
        GATTC_WARN("gattc_uuid_compare : invalid uuid len, len1=%u, len2=%u", len1, len2);
        return 0;
    }

    if(len1 == len2){
        return OS_MemCmp(uuid1, len1, uuid2, len2);
    }else if(len1 == 2){
        p16 = uuid1;
        p128 = uuid2;
    }else{
        p16 = uuid2;
        p128 = uuid1;
    }
    convert_uuid16_to_uuid128(uuid128, p16);
    return OS_MemCmp(p128, 16, uuid128, 16);
}

/* Only queue request and handle request in async timer callback */
static BtStatus queueConnRequest(GattcConnection *conn, GattcReq *req){
    BtStatus status = BT_STATUS_PENDING;
    InsertTailList(&conn->reqList , &req->node);
    if(IsEntryAvailable(&conn->asyncApiTimer.node)){
        /* Start timer to let API becomes async */
        EVM_StartTimer(&conn->asyncApiTimer, 0);
    }
    return status;
}

static GattcConnection *getGattcConn(GattcUserContext *user, BtRemoteDevice *remDev){
    GattcConnection *conn = NULL;
    ListEntry *entry = &user->connList;
    GATTC_DBG("getGattcConn : user=%p, remDev=%p", user, remDev);
    while((entry=GetNextNode(entry)) != &user->connList){
        conn = (GattcConnection*)entry;
        if(remDev == conn->channel->link){
            GATTC_DBG("conn %p found", conn);
            return conn;
        }
    }
    GATTC_ERR("Can not find conn");
    return NULL;
}

static GattcConnection *getGattcConnByAddr(GattcUserContext *user, const BD_ADDR *bd_addr){
    BtRemoteDevice *remDev = ME_FindRemoteDevice(bd_addr);
    if(remDev){
        return getGattcConn(user, remDev);
    }
    return NULL;
}

static GattcConnection *getGattcConnFromHdl(GattcUserContext *user, GATTC_CONN_HANDLE hdl){
    U32 idx = (U32)hdl;
    GattcConnection *conn = NULL;
    ListEntry *entry = &user->connList;
    GATTC_DBG("getGattcConnFromHdl : user=%p, connhdl=%p", user, hdl);
    while((entry=GetNextNode(entry)) != &user->connList){
        conn = (GattcConnection*)entry;
        if(idx == BT_ATT_GetConId(conn->channel->link)){
            GATTC_DBG("conn %p found", conn);
            return conn;
        }
    }
    GATTC_ERR("Can not find conn");
    return NULL;
}

static BtStatus gattcConnect(GattcUserContext *user, const BD_ADDR *addr, U8 is_direct, GattcConnection **pConn){
    BtStatus status = BT_STATUS_INVALID_PARM;
    GattcConnection *conn = NULL;
    GattChannel *channel = NULL;

    conn = getGattcConnByAddr(user, addr);
    if(conn){
        GATTC_WARN("Conn %p already exist", conn);
        if(pConn) *pConn = conn;        
        return BT_STATUS_CONNECTION_EXIST;
    }
    GATTC_DBG("gattcConnect user=%p, direct=%u", user, is_direct);
    if(user && addr && IsNodeOnList(&GATTCC(userList), &user->node)){
        channel = findExistChannel(addr);
        if(channel || is_direct){
            conn = GattcAllocConn(user);
            if(conn){
                status = GattFindAndAllocChannel(addr, &conn->channelHandler);
                if(status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING){
                    GattcFreeConn(conn);
                    conn = NULL;
                }else{
                    conn->channel = conn->channelHandler.channel;
                }
                if(pConn) *pConn = conn;
            }
        }else{
            // TODO: Implement background connection here
            //BtBmLeAdvEnabled
            if( !checkUserBgConn(user, addr) ){
                if( !GattcAllocBgConn(user,addr) ){
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
    GATTC_DBG("gattcConnect return %d", status);
    return status;
}


/************************************************
*   GATTC API
*************************************************/
void GATTC_Init(void)
{
    U32 i;

    OS_LockStack();
    GATTC_DBG("+ GATTC_Init");

#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8 *)gattc_ctx, 0, sizeof(*gattc_ctx));
#else /* XA_CONTEXT_PTR == XA_ENABLED */
    OS_MemSet((U8 *)&gattc_ctx, 0, sizeof(gattc_ctx));
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

    /* Initialize User Lists */
    InitializeListHead(&GATTCC(userList));
    InitializeListHead(&GATTCC(freeUserList));
    for(i = 0;i < sizeof(GATTCC(userContext))/sizeof(GATTCC(userContext)[0]);i++){
        InsertHeadList(&GATTCC(freeUserList), &GATTCC(userContext)[i].node);
    }
    /* Initialize Channel Lists */
    InitializeListHead(&GATTCC(channelList));
    InitializeListHead(&GATTCC(freeChannelList));
    for(i = 0;i < sizeof(GATTCC(channelContext))/sizeof(GATTCC(channelContext)[0]);i++){
        InsertHeadList(&GATTCC(freeChannelList), &GATTCC(channelContext)[i].node);
    }
    /* Free connection List */
    InitializeListHead(&GATTCC(freeConnList));
    for(i = 0;i < sizeof(GATTCC(connContext))/sizeof(GATTCC(connContext)[0]);i++){
        InsertHeadList(&GATTCC(freeConnList), &GATTCC(connContext)[i].node);
    }
    /* Free op List */
    InitializeListHead(&GATTCC(freeOpList));
    for(i = 0;i < sizeof(GATTCC(gattOp))/sizeof(GATTCC(gattOp)[0]);i++){
        InsertHeadList(&GATTCC(freeOpList), &GATTCC(gattOp)[i].op.op.node);
    }
    /* Free bg conn List */
    InitializeListHead(&GATTCC(freeBgConnList));
    for(i = 0;i < sizeof(GATTCC(bgConn))/sizeof(GATTCC(bgConn)[0]);i++){
        InsertHeadList(&GATTCC(freeBgConnList), &GATTCC(bgConn)[i].node);
    }

    /* Free reg notification List */
    InitializeListHead(&GATTCC(freeRegNotiList));
    for(i = 0;i < sizeof(GATTCC(regNoti))/sizeof(GATTCC(regNoti)[0]);i++){
        InsertHeadList(&GATTCC(freeRegNotiList), &GATTCC(regNoti)[i].node);
    }

    /* Get LE scan index */
    GATTCC(leScanIdx) = BtbmAdvLeGetAdvIndex();
    GATTCC(flag) = 0;
    GATTC_DBG("Get le scan index %u", (unsigned int)GATTCC(leScanIdx));
    GATTCC(leAdvHandler).whiteList = NULL;
    GATTCC(leAdvHandler).whiteListNo = 0;
    GATTCC(leAdvHandler).blackList = NULL;
    GATTCC(leAdvHandler).blackListNo = 0;
    GATTCC(leAdvHandler).callback = gattcAdvEventCallBack;

    BT_GattRegisterConnectedEvent(&GATTCC(gatt_handler), GattcAPCallBack);

    //Register global ME eventss
    InitializeListEntry(&GATTCC(meHandler).node);
    GATTCC(meHandler).callback = meCallBack;
    ME_RegisterGlobalHandler(&GATTCC(meHandler));
    ME_SetEventMask(&GATTCC(meHandler), BEM_ALL_EVENTS);    
    //GATTC_DBG("meHandler eMask = %u, handler=%p, cb=%p", GATTCC(meHandler).eMask, &GATTCC(meHandler), GATTCC(meHandler).callback);
    
    GATTC_DBG("- GATTC_Init");    
    OS_UnlockStack();
}

void GATTC_Deinit(void){
    OS_LockStack();
    ME_UnregisterGlobalHandler(&GATTCC(meHandler));
    BT_GattUnregisterConnectedEvent(&GATTCC(gatt_handler));
    OS_UnlockStack();
}

BtStatus GATTC_Register(GATTC_REG_HANDLE *handle, GattcCallback cb){
    BtStatus status = BT_STATUS_SUCCESS;
    OS_LockStack();
    GATTC_DBG("+ GATTC_Register : handle=%p", handle);
    if(handle && cb){
        /* Allocate user context from freeUserList */
        *handle = GattcAllocUser();
        if(*handle == NULL){
            status = BT_STATUS_NO_RESOURCES;
        }else{
            ((GattcUserContext*)(*handle))->callback = cb;
        }
    }else{
        GATTC_ERR("Invalid parameter handle=%p, cb=%p", handle, cb);
        status = BT_STATUS_INVALID_PARM;
    }
    GATTC_DBG("- GATTC_Register : status=%d, handle=%p", status, handle ? *handle : 0);
    OS_UnlockStack();
    return status;	
}

BtStatus GATTC_Deregister(GATTC_REG_HANDLE handle){
    BtStatus status = BT_STATUS_SUCCESS;
    OS_LockStack();
    GATTC_DBG("+ GATTC_Deregister : handle=%p", handle);
    GattcFreeUser(handle);
    GATTC_DBG("- GATTC_Deregister : status=%d, handle=%p", status, handle);
    OS_UnlockStack();
    return status;
}

BtStatus GATTC_Open(GATTC_REG_HANDLE handle, U8 *addr, U32 is_direct, GATTC_CONN_HANDLE *connHdl){
    BtStatus status = BT_STATUS_SUCCESS;
    GattcUserContext *user = (GattcUserContext*)handle;
    GattcConnection *conn;
    GattcBgConn *bgConn = NULL;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (addr && handle && connHdl));

    OS_LockStack();
    GATTC_DBG("+ GATTC_Open : user=%p, addr=%p", user, addr);
    /* Remove bg connection first */
    GATTC_DBG("Free bg connection first");
    bgConn = checkUserBgConn(user, (BD_ADDR*)addr);
    if(bgConn) GattcFreeBgConn(bgConn);
    status = gattcConnect(user, (BD_ADDR*)addr, is_direct, &conn);
    if((status == BT_STATUS_SUCCESS) || (status == BT_STATUS_CONNECTION_EXIST)){
        *connHdl = (GATTC_CONN_HANDLE)BT_ATT_GetConId(conn->channel->link);
        status = BT_STATUS_SUCCESS;
    }
    GATTC_DBG("- GATTC_Open : status=%d", status);
    OS_UnlockStack();
    return status;
}


BtStatus GATTC_Close(GATTC_REG_HANDLE regHdl, U8 *addr, GATTC_CONN_HANDLE connHdl){
    BtStatus status = BT_STATUS_SUCCESS;
    GattcUserContext *user = (GattcUserContext*)regHdl;
    GattcConnection *conn;
    GattcBgConn *bgConn = NULL;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (user));
    OS_LockStack();
    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&GATTCC(userList), &user->node));
    conn = getGattcConnFromHdl(user, connHdl);
    if(conn){
        GattcFreeConn(conn);
    }
    bgConn = checkUserBgConn(user, (BD_ADDR*)addr);
    if(bgConn) GattcFreeBgConn(bgConn);
    OS_UnlockStack();
    return status;
}

BtStatus GATTC_Scan(GATTC_REG_HANDLE regHdl, U8 enable){
    BtStatus status = BT_STATUS_SUCCESS;
    GattcUserContext *user = (GattcUserContext*)regHdl;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (user));
    OS_LockStack();
    CheckLockedParm(BT_STATUS_NOT_FOUND, IsNodeOnList(&GATTCC(userList), &user->node));
    if(enable){
        user->flag |= GATTC_USER_FLAG_SCANING;
    }else{
        user->flag &= ~GATTC_USER_FLAG_SCANING;
    }
    updateScaningState();
    
    OS_UnlockStack();  
    return status;
}

BtStatus GATTC_Refresh(GATTC_REG_HANDLE regHdl, U8 *addr){
    // TODO: refresh DB
    return BT_STATUS_SUCCESS;
}

BtStatus GATTC_SearchService(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, ATT_UUID *filterUuid){
    BtStatus status = BT_STATUS_SUCCESS;
    GattcUserContext *user = (GattcUserContext*)regHdl;
    GattcConnection *conn;
    GattcReq *pReq = (GattcReq*)gatt_malloc(sizeof(GattcReq));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (user));
    conn = getGattcConnFromHdl(user, connHdl);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (conn));

    OS_MemSet((U8*)pReq, 0, sizeof(GattcReq));
    if(pReq){
        pReq->type = GATTC_REQ_TYPE_SERVICE_SEARCH;
        pReq->param.service_search.conn = conn;
        if(filterUuid){
            pReq->param.service_search.filterUuid = *filterUuid;
        }
        status = queueConnRequest(conn, pReq);
    }else{
        status = BT_STATUS_NO_RESOURCES;
    }
    return status;
}

BtStatus GATTC_GetInclService(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, GATT_SVC_UUID *prevInclServ){
    BtStatus status = BT_STATUS_SUCCESS;
    GattcUserContext *user = (GattcUserContext*)regHdl;
    GattcConnection *conn;
    GattcReq *pReq = (GattcReq*)gatt_malloc(sizeof(GattcReq));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (user));
    conn = getGattcConnFromHdl(user, connHdl);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (conn));

    OS_MemSet((U8*)pReq, 0, sizeof(GattcReq));    
    if(pReq){
        pReq->type = GATTC_REQ_TYPE_INCL_SERVICE_SEARCH;
        pReq->param.incl_service_search.conn = conn;
        pReq->param.incl_service_search.svc_uuid = *servUuid;
        if(prevInclServ){
            pReq->param.incl_service_search.prev_incl_svc_uuid = *prevInclServ;
        }
        status = queueConnRequest(conn, pReq);
    }else{
        status = BT_STATUS_NO_RESOURCES;
    }
    return status;
}

BtStatus GATTC_GetChar(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, ATT_UUID *prevCharUuid){
    BtStatus status = BT_STATUS_SUCCESS;
    GattcUserContext *user = (GattcUserContext*)regHdl;
    GattcConnection *conn;
    GattcReq *pReq = (GattcReq*)gatt_malloc(sizeof(GattcReq));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (user));
    conn = getGattcConnFromHdl(user, connHdl);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (conn));

    OS_MemSet((U8*)pReq, 0, sizeof(GattcReq));
    if(pReq){
        pReq->type = GATTC_REQ_TYPE_GET_CHAR;
        pReq->param.get_char.conn = conn;
        pReq->param.get_char.svc_uuid = *servUuid;
        if(prevCharUuid){
            pReq->param.get_char.prev_char_uuid = *prevCharUuid;
        }
        status = queueConnRequest(conn, pReq);
    }else{
        status = BT_STATUS_NO_RESOURCES;
    }
    return status;
}

BtStatus GATTC_GetDescr(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, ATT_UUID *charUuid, ATT_UUID *prevDescrUuid){
    BtStatus status = BT_STATUS_SUCCESS;
    GattcUserContext *user = (GattcUserContext*)regHdl;
    GattcConnection *conn;
    GattcReq *pReq = (GattcReq*)gatt_malloc(sizeof(GattcReq));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (user));
    conn = getGattcConnFromHdl(user, connHdl);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (conn));

    OS_MemSet((U8*)pReq, 0, sizeof(GattcReq));
    if(pReq){
        pReq->type = GATTC_REQ_TYPE_GET_DESCR;
        pReq->param.get_descr.conn = conn;
        pReq->param.get_descr.svc_uuid = *servUuid;
        pReq->param.get_descr.char_uuid = *charUuid;
        if(prevDescrUuid){
            pReq->param.get_descr.prev_descr_uuid = *prevDescrUuid;
        }
        status = queueConnRequest(conn, pReq);
    }else{
        status = BT_STATUS_NO_RESOURCES;
    }
    return status;
}

BtStatus GATTC_ReadChar(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, ATT_UUID *charUuid, U32 authReq){
    BtStatus status = BT_STATUS_SUCCESS;
    GattcUserContext *user = (GattcUserContext*)regHdl;
    GattcConnection *conn;
    GattcReq *pReq = (GattcReq*)gatt_malloc(sizeof(GattcReq));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (user));
    conn = getGattcConnFromHdl(user, connHdl);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (conn));
    
    OS_MemSet((U8*)pReq, 0, sizeof(GattcReq));
    if(pReq){
        pReq->type = GATTC_REQ_TYPE_READ_CHAR;
        pReq->param.read_char.conn = conn;
        pReq->param.read_char.svc_uuid = *servUuid;
        pReq->param.read_char.char_uuid = *charUuid;
        pReq->param.read_char.auth_req = authReq;
        status = queueConnRequest(conn, pReq);
    }else{
        status = BT_STATUS_NO_RESOURCES;
    }
    return status;
}

BtStatus GATTC_WriteChar(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, ATT_UUID *charUuid, U32 writeType, U16 len, U8 *value, U32 authReq){
    BtStatus status = BT_STATUS_SUCCESS;
    GattcUserContext *user = (GattcUserContext*)regHdl;
    GattcConnection *conn;
    GattcReq *pReq = (GattcReq*)gatt_malloc(sizeof(GattcReq));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (user));
    conn = getGattcConnFromHdl(user, connHdl);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (conn));
    
    OS_MemSet((U8*)pReq, 0, sizeof(GattcReq));
    if(pReq){
        pReq->type = GATTC_REQ_TYPE_WRITE_CHAR;
        pReq->param.write_char.conn = conn;
        pReq->param.write_char.svc_uuid = *servUuid;
        pReq->param.write_char.char_uuid = *charUuid;
        pReq->param.write_char.write_type = writeType;
        pReq->param.write_char.len = len;
        OS_MemCopy(pReq->param.write_char.value, value, len);
        pReq->param.write_char.auth_req = authReq;
        status = queueConnRequest(conn, pReq);
    }else{
        status = BT_STATUS_NO_RESOURCES;
    }
    return status;
}

BtStatus GATTC_ReadDescr(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, ATT_UUID *charUuid, ATT_UUID *descrUuid, U32 authReq){
    BtStatus status = BT_STATUS_SUCCESS;
    GattcUserContext *user = (GattcUserContext*)regHdl;
    GattcConnection *conn;
    GattcReq *pReq = (GattcReq*)gatt_malloc(sizeof(GattcReq));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (user));
    conn = getGattcConnFromHdl(user, connHdl);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (conn));
    
    OS_MemSet((U8*)pReq, 0, sizeof(GattcReq));
    if(pReq){
        pReq->type = GATTC_REQ_TYPE_READ_DESCR;
        pReq->param.read_descr.conn = conn;
        pReq->param.read_descr.svc_uuid = *servUuid;
        pReq->param.read_descr.char_uuid = *charUuid;
        pReq->param.read_descr.descr_uuid = *descrUuid;
        pReq->param.read_descr.auth_req = authReq;
        status = queueConnRequest(conn, pReq);
    }else{
        status = BT_STATUS_NO_RESOURCES;
    }
    return status;
}

BtStatus GATTC_WriteDescr(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, GATT_SVC_UUID *servUuid, ATT_UUID *charUuid, ATT_UUID *descrUuid, U32 writeType, U16 len, U8 *value, U32 authReq){
    BtStatus status = BT_STATUS_SUCCESS;
    GattcUserContext *user = (GattcUserContext*)regHdl;
    GattcConnection *conn;
    GattcReq *pReq = (GattcReq*)gatt_malloc(sizeof(GattcReq));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (user));
    conn = getGattcConnFromHdl(user, connHdl);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (conn));
    
    OS_MemSet((U8*)pReq, 0, sizeof(GattcReq));
    if(pReq){
        pReq->type = GATTC_REQ_TYPE_WRITE_DESCR;
        pReq->param.write_descr.conn = conn;
        pReq->param.write_descr.svc_uuid = *servUuid;
        pReq->param.write_descr.char_uuid = *charUuid;
        pReq->param.write_descr.descr_uuid = *descrUuid;
        pReq->param.write_descr.write_type = writeType;
        pReq->param.write_descr.len = len;
        OS_MemCopy(pReq->param.write_descr.value, value, len);
        pReq->param.write_descr.auth_req = authReq;
        status = queueConnRequest(conn, pReq);
    }else{
        status = BT_STATUS_NO_RESOURCES;
    }
    return status;
}

BtStatus GATTC_ExecWrite(GATTC_REG_HANDLE regHdl, GATTC_CONN_HANDLE connHdl, U8 execute){
    // TODO:
    BtStatus status = BT_STATUS_SUCCESS;
    GattcUserContext *user = (GattcUserContext*)regHdl;
    GattcConnection *conn;
    GattcReq *pReq = (GattcReq*)gatt_malloc(sizeof(GattcReq));

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (user));
    conn = getGattcConnFromHdl(user, connHdl);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (conn));
    
    OS_MemSet((U8*)pReq, 0, sizeof(GattcReq));
    if(pReq){
        pReq->type = GATTC_REQ_TYPE_EXEC_WRITE;
        pReq->param.exec_write.conn = conn;
        pReq->param.exec_write.execute = execute;
        status = queueConnRequest(conn, pReq);
    }else{
        status = BT_STATUS_NO_RESOURCES;
    }
    return status;

    return BT_STATUS_SUCCESS;    
}

BtStatus GATTC_RegisterNotification(GATTC_REG_HANDLE regHdl, U8 *bdaddr, GATT_SVC_UUID *servUuid, ATT_UUID *char_uuid, U8 reg){
    BtStatus status = BT_STATUS_FAILED;
    GattcUserContext *user = (GattcUserContext*)regHdl;
    GattcRegNotification *regNoti = NULL;
    
    regNoti = checkUserRegNoti(user, (BD_ADDR*)bdaddr, servUuid, char_uuid);
    if(regNoti){
        if(!reg){
            GattcFreeRegNoti(regNoti);
            status = BT_STATUS_SUCCESS;
        }
    }else if(reg){
        regNoti = GattcAllocRegNoti(user, (BD_ADDR*)bdaddr, servUuid, char_uuid);
        status = regNoti ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
    }
    return status;
}

BtStatus GATTC_ReadRssi(GATTC_REG_HANDLE regHdl, U8 *addr){
    BtStatus status = BT_STATUS_FAILED;
    GattcUserContext *user = (GattcUserContext*)regHdl;
    BtRemoteDevice * remDev = NULL;

    GATTC_DBG("GATTC_ReadRssi : user=%p", regHdl);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (user));
    if(!user->rssiDev){
        remDev = ME_FindRemoteDevice((BD_ADDR*)addr);
        if(remDev){
            user->rssiDev = remDev;
            status = ME_ReadRSSI(remDev);
        }else{
            GATTC_DBG("Can not find connected device");
        }
    }else{
        GATTC_DBG("Busy on read rssi od device %p", user->rssiDev);
        status = BT_STATUS_BUSY;
    }
    return status;
}

U8 GATTC_GetDeviceType(GATTC_REG_HANDLE regHdl, U8 *addr){
    BtDevType type = ME_GetDeviceType((BD_ADDR*)addr);
    return (U8)type;
}

static GattcBgConn *checkBgConnection(BD_ADDR *bdaddr){
    ListEntry *user = &GATTCC(userList);
    GattcBgConn *bg = NULL;
    OS_Report("checkBgConnection");
    while((user = GetNextNode(user)) != &GATTCC(userList)){
        bg = checkUserBgConn((GattcUserContext*)user, bdaddr);
        if(bg){
            OS_Report("Found user=%p, bg=%p", user, bg);
            return bg;
        }
    }
    OS_Report("No bg found");
    return bg;
}

static U8 checkScaning(void){
    ListEntry *user = &GATTCC(userList);
    OS_Report("checkScaning");
    while((user = GetNextNode(user)) != &GATTCC(userList)){
        if(((GattcUserContext*)user)->flag & GATTC_USER_FLAG_SCANING){
            OS_Report("user %p request scaning", user);
            return 1;
        }
    }
    OS_Report("no user request scaning");
    return 0;
}

U8 reportRssiResult(const BtEvent *Event){
    ListEntry *user = &GATTCC(userList);
    BtRemoteDevice* remDev = Event->p.rssi.remDev;
    //S8 rssi_value = (Event->p.rssi.RSSI & 0x80) ? (Event->p.rssi.RSSI - );
    BtErrorCode errCode = Event->errCode;
    GattcCallbackParms cbParam;

    cbParam.event = GATTC_EVENT_READ_RSSI_RESULT;
    cbParam.handle = user;
    cbParam.result = (Event->errCode==BEC_NO_ERROR) ? 0 : 1;
    cbParam.param.read_rssi_result.bdaddr = remDev->bdAddr;
    if(Event->p.rssi.RSSI & 0x80){
        cbParam.param.read_rssi_result.rssi = ((S32)Event->p.rssi.RSSI) - 256;//rssi_value;
    }else{
        cbParam.param.read_rssi_result.rssi = Event->p.rssi.RSSI;
    }
    OS_Report("reportRssiResult : remDev=%p, err=%u, rssi=%d", remDev, errCode, cbParam.param.read_rssi_result.rssi);
    while((user = GetNextNode(user)) != &GATTCC(userList)){
        if(((GattcUserContext*)user)->rssiDev == remDev){
            OS_Report("user %p request RSSI", user);
            ((GattcUserContext*)user)->callback(user, &cbParam);
            ((GattcUserContext*)user)->rssiDev = NULL;
        }
    }
    return 0;
}

/* 0 : no scaning, 1 : scaning */
static U8 updateScaningState(void){
    OS_Report("updateScaningState : GATTCC(flag)=%u", GATTCC(flag));
    if( !checkBgConnection(NULL) && !checkScaning()){
        if(GATTCC(flag) & GATTC_FLAG_SCANING){
            BtBmLeAdvDisabled(GATTCC(leScanIdx), &GATTCC(leAdvHandler));
            GATTCC(flag) &= ~GATTC_FLAG_SCANING;
        }
    }else{
        //if(!(GATTCC(flag) & GATTC_FLAG_SCANING)){
            BtBmLeAdvEnabled(GATTCC(leScanIdx), &GATTCC(leAdvHandler));
            GATTCC(flag) |= GATTC_FLAG_SCANING;
        //}
    }
    return (GATTCC(flag) | GATTC_FLAG_SCANING) ? 1 : 0;
}

static U8 getEIRResponseTotalLength(U8 *eir_reponse)
{
    U8 eir_len = 0;
    U8 total_len = 0;
    U8 *eir_ptr = eir_reponse;    

    while ((total_len < 240) && eir_ptr)
    {
        eir_len = eir_ptr[0];
        if (eir_len != 0x00)
        {            
            eir_ptr += eir_len + 1;  
            total_len += (eir_len + 1);
        }
        else
        {
            break;
        }
    }
    OS_Report("[GATTC] getEIRResponseTotalLength %d", total_len);
    return total_len;
}

static void updateEIRResponse(BtInquiryResult *inqResult){
    BtDeviceContext *bdc = DS_FindDevice(&inqResult->bdAddr);
    GATTC_DBG("updateEIRResponse(%p) : bdc=%p", inqResult, bdc);
    if(bdc){
        btbmUtilParseInquiryEirResponse(bdc, inqResult->eirResponse);
        if(bdc->device_name && bdc->device_name_length){
            btbm_bd_addr_t rem_addr;
            CONVERT_ARRAY2BDADDR(&rem_addr, bdc->addr.addr);
            BTBMGapDiscoveryResultCfm(
                              TRUE, 
                              rem_addr,
                              #ifdef __BT_4_0_BLE__
                              bt_util_convert_dev(bdc->devType),
                              #endif
                              #if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
                              bdc->rssi, 
                              1, 
                              (U8)((bdc->support_service_list == BT_EIR_SERVICE_INFO_NONE) ? 0 : 1), 
                              (U8)((bdc->support_service_list == BT_EIR_SERVICE_INFO_MORE) ? 0 : 1),
                              bdc->eir_response,
                              #endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
                              bdc->classOfDevice, 
                              bdc->device_name_length, 
                              bdc->device_name);
        }
    }
}

/************************************************
*   Callback handler
*************************************************/
static void gattcAdvEventCallBack(const BtEvent *event){
    BtStatus status;
    GattcCallbackParms param;
    ListEntry *user = &GATTCC(userList);    
    GattcBgConn *bg = NULL;
    GattcConnection *conn = NULL;    
    BtInquiryResult *inqResult = (BtInquiryResult *)&event->p.inqResult;

    GATTC_DBG("gattcAdvEventCallBack");
    BTBMDumpBdAddr(inqResult->bdAddr.addr);
    /* update BG connection list */
    while((user = GetNextNode(user)) != &GATTCC(userList)){
        GATTC_DBG("Check bg of user %p", user);
        bg = checkUserBgConn((GattcUserContext*)user, &inqResult->bdAddr);
        if(bg){
            GattcFreeBgConn(bg);
            status = gattcConnect((GattcUserContext*)user, &inqResult->bdAddr, 1, &conn);
            if(status == BT_STATUS_SUCCESS){
                reportConnected((GattcUserContext*)user, conn);
            }            
        }
    }

#ifdef __BT_4_0_ACTIVE_SCAN__
    U8 len = 0;
    BtDeviceContext *bdc = DS_FindDevice(&inqResult->bdAddr);   
    if(!bdc){
        OS_Report("[GATTC][WRN] find device failure");
        return;
    }    

    if(inqResult->supported_eir){
        updateEIRResponse(inqResult);         
        len = getEIRResponseTotalLength(inqResult->eirResponse);
        if((len + bdc->cachedDataLen) < MAX_CACHED_EIR_DATA_LEN){
            OS_MemCopy(&bdc->cachedData[bdc->cachedDataLen], inqResult->eirResponse, sizeof(U8) * len);  
            bdc->cachedDataLen += len;
        }else{
            OS_Report("[GATTC][WRN] cached eir data full, ignore the new report (%d>%d)",
                len + bdc->cachedDataLen, MAX_CACHED_EIR_DATA_LEN);  
        }
    }     

    /* active scan, always wait until get scan_rsp to report the result */
    OS_Report("[GATTC] devType %d advType %d", inqResult->devType, inqResult->advType);
    if ((inqResult->devType == BT_DEV_TYPE_LE) && 
        ((inqResult->advType == HCI_ADV_IND) || 
         (inqResult->advType == HCI_ADV_SCAN_IND))){
        return;
    } 
    
    param.param.scan_result.eir = bdc->cachedData;
    param.param.scan_result.eir_len = bdc->cachedDataLen;
#else    
    if(inqResult->supported_eir){
        param.param.scan_result.eir = inqResult->eirResponse;
        param.param.scan_result.eir_len = getEIRResponseTotalLength(inqResult->eirResponse);
        updateEIRResponse(inqResult);
    }else{
        param.param.scan_result.eir = NULL;
        param.param.scan_result.eir_len = 0;
    }
#endif

    param.event = GATTC_EVENT_SCAN_RESULT;
    param.result = 0;
    param.param.scan_result.addr = inqResult->bdAddr;
    if(inqResult->rssi & 0x80){
        param.param.scan_result.rssi = ((S32)inqResult->rssi) - 256;//rssi_value;
    }else{
        param.param.scan_result.rssi = inqResult->rssi;
    }

    /* indicate scaning result */
    user = &GATTCC(userList);
    while((user = GetNextNode(user)) != &GATTCC(userList)){
        if(((GattcUserContext*)user)->flag & GATTC_USER_FLAG_SCANING){
            param.handle = user;            
            ((GattcUserContext*)user)->callback(user, &param);    
        }
    }

#ifdef __BT_4_0_ACTIVE_SCAN__
    OS_MemSet(bdc->cachedData, 0, sizeof(bdc->cachedData));
    bdc->cachedDataLen = 0;
#endif    
    //updateScaningState();
}

static void gattcReadCharvalueCallback(void *userdata, BtGattEvent *event)
{
    GattcOp *op = (GattcOp *)userdata;
    BtGattReadCharacteristicValueResultEvent *result;
    GattcConnection *conn = (GattcConnection*)op->context;
    GattcUserContext *user = NULL;
    GattcReq *req = NULL;
    GattcCallbackParms cbParam;

    if(conn){
        user = conn->user;
        req = conn->currReq;

        result = (BtGattReadCharacteristicValueResultEvent *) event->parms;
        
        cbParam.event = GATTC_EVENT_READ_CHAR_RESULT;
        cbParam.handle = user;
        
        cbParam.param.read_char_result.connHdl = (GATTC_CONN_HANDLE)BT_ATT_GetConId(conn->channel->link);
        cbParam.param.read_char_result.svc_uuid = req->param.read_char.svc_uuid;
        cbParam.param.read_char_result.char_uuid = req->param.read_char.char_uuid;
        
        switch (result->flag)
        {
            case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
                cbParam.result = 0;
                cbParam.param.read_char_result.len = result->length;
                cbParam.param.read_char_result.value = result->data;
                user->callback(user, &cbParam);
                break;
            case BT_GATT_EVENT_RESULT_ERROR:
            case BT_GATT_EVENT_RESULT_DISCONNECT:
                cbParam.result = 1;
                cbParam.param.read_char_result.len = 0;
                user->callback(user, &cbParam);
                break;
            default:
                GATTC_WARN("Invalid flag : %u", result->flag);
                break;
        }
        gatt_free(conn->currReq);
        conn->currReq = NULL;
        gattcFreeOp(op);        
        asyncConnApiListHandler(conn);
    }else{
        GATTC_WARN("gattcReadCharvalueCallback : conn is NULL.");
    }
}

void gattcWriteCharvalueCallback(void *userdata, BtGattEvent *event){
    GattcOp *op = (GattcOp *)userdata;
    BtGattWriteCharValueResultEvent *result;
    GattcConnection *conn = (GattcConnection*)op->context;
    GattcUserContext *user = NULL;
    GattcReq *req = NULL;
    GattcCallbackParms cbParam;

    if(conn){
        user = conn->user;
        req = conn->currReq;

        result = (BtGattWriteCharValueResultEvent *) event->parms;
        
        cbParam.event = GATTC_EVENT_WRITE_CHAR_RESULT;
        cbParam.handle = user;

        cbParam.param.write_char_result.connHdl = (GATTC_CONN_HANDLE)BT_ATT_GetConId(conn->channel->link);
        cbParam.param.write_char_result.svc_uuid = req->param.write_char.svc_uuid;
        cbParam.param.write_char_result.char_uuid = req->param.write_char.char_uuid;
        
        switch (result->flag)
        {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            cbParam.result = 0;
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
        gatt_free(conn->currReq);
        conn->currReq = NULL;
        gattcFreeOp(op);
        asyncConnApiListHandler(conn);
    }else{
        GATTC_WARN("gattcWriteCharvalueCallback : conn is NULL.");
    }
}

void gattcReadDescrCallback(void *userdata, BtGattEvent *event){
    GattcOp *op = (GattcOp *)userdata;
    BtGattReadCharacteristicDescriptorsResultEvent *result;
    GattcConnection *conn = (GattcConnection*)op->context;
    GattcUserContext *user = NULL;
    GattcReq *req = NULL;
    GattcCallbackParms cbParam;

    if(conn){
        user = conn->user;
        req = conn->currReq;

        result = (BtGattReadCharacteristicDescriptorsResultEvent *) event->parms;

        cbParam.event = GATTC_EVENT_READ_DESCR_RESULT;
        cbParam.handle = user;

        cbParam.param.read_descr_result.connHdl = (GATTC_CONN_HANDLE)BT_ATT_GetConId(conn->channel->link);
        cbParam.param.read_descr_result.svc_uuid = req->param.read_descr.svc_uuid;
        cbParam.param.read_descr_result.char_uuid = req->param.read_descr.char_uuid;
        cbParam.param.read_descr_result.descr_uuid = req->param.read_descr.descr_uuid;

        switch (result->flag)
        {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            cbParam.result = 0;
            cbParam.param.read_descr_result.len = result->length;
            cbParam.param.read_descr_result.value = result->data;
            user->callback(user, &cbParam);
            break;
        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            cbParam.result = 1;
            cbParam.param.read_descr_result.len = 0;
            user->callback(user, &cbParam);        
            break;
        default:
            GATTC_WARN("Invalid flag : %u", result->flag);
            break;
        }
        gatt_free(conn->currReq);
        conn->currReq = NULL;
        gattcFreeOp(op);
        asyncConnApiListHandler(conn);
    }else{
        GATTC_WARN("gattcReadDescrCallback : conn is NULL.");
    }
}

void gattcWriteDescrCallback(void *userdata, BtGattEvent *event){
    GattcOp *op = (GattcOp *)userdata;
    BtGattWriteCharDescriptorResultEvent *result;
    GattcConnection *conn = (GattcConnection*)op->context;
    GattcUserContext *user = NULL;
    GattcReq *req = NULL;
    GattcCallbackParms cbParam;

    if(conn){
        user = conn->user;
        req = conn->currReq;
    
        result = (BtGattWriteCharDescriptorResultEvent *) event->parms;

        cbParam.event = GATTC_EVENT_WRITE_DESCR_RESULT;
        cbParam.handle = user;

        cbParam.param.write_descr_result.connHdl = (GATTC_CONN_HANDLE)BT_ATT_GetConId(conn->channel->link);
        cbParam.param.write_descr_result.svc_uuid = req->param.write_descr.svc_uuid;
        cbParam.param.write_descr_result.char_uuid = req->param.write_descr.char_uuid;
        cbParam.param.write_descr_result.descr_uuid = req->param.write_descr.descr_uuid;

        switch (result->flag)
        {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            cbParam.result = 0;
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
        gatt_free(conn->currReq);
        conn->currReq = NULL;
        gattcFreeOp(op);
        asyncConnApiListHandler(conn);
    }else{
        GATTC_WARN("gattcWriteDescrCallback : conn is NULL.");
    }
}

void gattcExecWriteCallback(void *userdata, BtGattEvent *event){
    GattcOp *op = (GattcOp *)userdata;
    BtGattExecWriteResultEvent *result;
    GattcConnection *conn = (GattcConnection*)op->context;
    GattcUserContext *user = NULL;
    GattcReq *req = NULL;
    GattcCallbackParms cbParam;

    if(conn){
        user = conn->user;
        req = conn->currReq;

        result = (BtGattExecWriteResultEvent *) event->parms;

        cbParam.event = GATTC_EVENT_EXEC_WRITE_RESULT;
        cbParam.handle = user;

        switch (result->flag)
        {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            cbParam.result = 0;
            cbParam.param.exec_write_result.connHdl = (GATTC_CONN_HANDLE)BT_ATT_GetConId(conn->channel->link);
            cbParam.param.exec_write_result.executed = result->executed;
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
        gatt_free(conn->currReq);
        conn->currReq = NULL;
        gattcFreeOp(op);
        asyncConnApiListHandler(conn);
    }else{
        GATTC_WARN("gattcExecWriteCallback : conn is NULL.");
    }
}

static void meCallBack(const BtEvent *Event){
    BtAttConn *conn;
    BtRemoteDevice* remDev;
    OS_Report("meCallBack : event=%u", Event->eType);
    switch (Event->eType)
    {
    case BTEVENT_READ_RSSI_COMPLETED:
    {
        reportRssiResult(Event);        
    }
        break;
    default:
        break;
    }
}

static void reportConnected(const GattcUserContext *user, const GattcConnection *conn){
    GattcCallbackParms cbParam;

    if(user && conn){
        cbParam.event = GATTC_EVENT_CONNECTED_IND;
        cbParam.result = 0;
        cbParam.handle = (GATTC_REG_HANDLE)user;
        cbParam.param.connected_ind.bdaddr = conn->channel->link->bdAddr;
        cbParam.param.connected_ind.connHdl = (GATTC_CONN_HANDLE)BT_ATT_GetConId(conn->channel->link);
        //cbParam.param.connected_ind.result = 0;
        user->callback((GATTC_REG_HANDLE)user, &cbParam);
    }
}

static void reportIndication(GattcUserContext *user, const BD_ADDR *bd_addr, U16 handle, U16 value_len, U8 *value, U8 is_notify){
    ListEntry *entry;
    GattcRegNotification *noti = (GattcRegNotification*)(&user->regNotiList);
    GattcConnection *conn = NULL;
    BleServiceDB *db = BleFindServiceDBbyAddr(bd_addr);
    BleServiceNode *svc = NULL;
    GattcCallbackParms cbParam;

/*
    if(db == NULL){
        OS_Report("DB is not found try to load from file");
        if(BLE_LoadServiceDatabase(bd_addr) == BT_STATUS_SUCCESS){
            db = BleFindServiceDBbyAddr(bd_addr);
        }else{
            OS_Report("reportIndication : Load cached db file failed.");
        }
    }
*/
    if(db == NULL){
        OS_Report("reportIndication : db is NULL, just return.");
        return;
    }

    cbParam.event = GATTC_EVENT_NOTIFICATION_IND;
    cbParam.handle = user;
    cbParam.result = 0;
    cbParam.param.notification_ind.bdaddr = *bd_addr;
    cbParam.param.notification_ind.len = value_len;
    cbParam.param.notification_ind.value = value;
    cbParam.param.notification_ind.is_notify = is_notify;

    /* Find svc uuid */
    svc = BleFindServicebyHandleRange(&db->service_list, handle);
    if(!svc){
        GATTC_WARN("No corresponding service node found for handle %u", handle);
        return;
    }
    OS_MemCopy(cbParam.param.notification_ind.svc_uuid.uuid.uuid.uuid, 
                           svc->service.uuid.uuid128,
                           svc->service.uuid_size);
    cbParam.param.notification_ind.svc_uuid.uuid.uuid.len = svc->service.uuid_size;
    cbParam.param.notification_ind.svc_uuid.is_primary = 1;
    cbParam.param.notification_ind.svc_uuid.uuid.inst = getServiceInst(bd_addr, &svc->service);
    switch (getValueNodeByHandle(db, handle, &entry)){
    case 1:
        /* Char value */
        {
            BleCharNode *char_node = (BleCharNode*)entry;
            OS_MemCopy(cbParam.param.notification_ind.char_uuid.uuid.uuid, 
                                   char_node->characteristic.value_uuid.uuid128,
                                   char_node->characteristic.value_uuid_size);
            cbParam.param.notification_ind.char_uuid.uuid.len = char_node->characteristic.value_uuid_size;
            cbParam.param.notification_ind.char_uuid.inst = getCharInst(&svc->service, &char_node->characteristic);
            while((noti = (GattcRegNotification*)GetNextNode(&noti->node)) != (GattcRegNotification*)&user->regNotiList){
                if(OS_MemCmp(noti->addr.addr, 6, bd_addr->addr, 6) &&
                    BLE_CompareUuid(noti->servUuid.uuid.uuid.uuid, noti->servUuid.uuid.uuid.len,
                                                  svc->service.uuid.uuid128,
                                                  svc->service.uuid_size) &&
                    noti->servUuid.uuid.inst == cbParam.param.notification_ind.svc_uuid.uuid.inst &&
                    BLE_CompareUuid(noti->char_uuid.uuid.uuid, noti->char_uuid.uuid.len,
                                                  char_node->characteristic.value_uuid.uuid128,
                                                  char_node->characteristic.value_uuid_size) &&
                    noti->char_uuid.inst == cbParam.param.notification_ind.char_uuid.inst){
                    if(gattcConnect(user, bd_addr, 1, &conn) == BT_STATUS_SUCCESS){
                        reportConnected(user, conn);
                    }
                    conn = getGattcConnByAddr(user, bd_addr);
                    if(conn && conn->channel){
                        cbParam.param.notification_ind.connHdl = (GATTC_CONN_HANDLE)BT_ATT_GetConId(conn->channel->link);
                    }else{
                        cbParam.param.notification_ind.connHdl = 0;
                    }                    
                    user->callback(user, &cbParam);
                }
            }
        }
        break;
    case 2:
        /* Descr */
        {
            /*
            BleCharDescNode *desc_node = (BleCharDescNode*)entry;
            BleFindCharbyHandle(svc, );
            OS_MemCopy(cbParam.param.notification_ind.char_uuid.uuid.uuid, 
                                   desc_node->descriptor.value_uuid.uuid128,
                                   desc_node->descriptor.value_uuid_size);
            cbParam.param.notification_ind.char_uuid.uuid.len = desc_node->descriptor.value_uuid_size;
            cbParam.param.notification_ind.char_uuid.uuid.inst = getDescrInst(&svc->service, &desc_node->descriptor);
            */
            GATTC_WARN("Descriptor notification not support");
        }
        break;
    case 0:
    default:
        GATTC_WARN("No related value node found");
        break;
    }
}

static void GattcAPCallBack(void *handler, BtGattEvent *event){
    ListEntry *user;
    BtStatus status;
    GattcBgConn *bg;
    GattcConnection *conn = NULL;
    //GattcCallbackParms cbParam;
    U8 conn_idx = BT_ATT_GetConId(event->link);
    U8 is_notify = 0;


    OS_Report("GattcAPCallBack : eType=%u", event->eType);
    user = &GATTCC(userList);
    switch(event->eType){
    case BT_GATT_APP_EVENT_CONNECTED:
        OS_Report("BT_GATT_APP_EVENT_CONNECTED : userList=%p, user=%p", &GATTCC(userList), user);
        /* Keep the link info */
        while((user = GetNextNode(user)) != &GATTCC(userList)){
            OS_Report("user=%p", user);
            bg = checkUserBgConn((GattcUserContext*)user, &event->link->bdAddr);
            if(bg){
                status = gattcConnect((GattcUserContext*)user, &event->link->bdAddr, 1, &conn);
                GattcFreeBgConn(bg);
                if(status == BT_STATUS_SUCCESS){
                    reportConnected((GattcUserContext*)user, conn);
                }
            }
        }
        break;        
    case BT_GATT_APP_EVENT_VALUE_NOTIFICATION:
        is_notify = 1;
    case BT_GATT_APP_EVENT_VALUE_INDICATION:
        {
            BtGattValueIndicationEvent *parm = (BtGattValueIndicationEvent*)event->parms;
            while((user = GetNextNode(user)) != &GATTCC(userList)){
                OS_Report("user=%p", user);
                reportIndication((GattcUserContext*)user, 
                                           &event->link->bdAddr, 
                                           parm->attributeHandle,
                                           parm->attributeValueLen,
                                           parm->attributeValue,
                                           is_notify);
            }
        }
        break;
    default:
        GATTC_WARN("Unknown GATT event : %d", event->eType);
        break;
    }
}

#endif /* __BT_GATTC_PROFILE__ */
