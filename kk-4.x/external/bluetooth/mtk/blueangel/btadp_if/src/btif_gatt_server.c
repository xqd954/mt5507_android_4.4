/******************************************************************************
 *
 *  Copyright (C) 2009-2013 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/


/************************************************************************************
 *
 *  Filename:      btif_gatt_server.c
 *
 *  Description:   GATT server implementation
 *
 ***********************************************************************************/

#include <hardware/bluetooth.h>
#include <hardware/bt_gatt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define LOG_TAG "BTIF_GATT_SERVER"
#include "bt_ext_common.h"
#include "bt_ext_debug.h"
#include "bt_ext_apis.h"
#include "btif_common.h"
#include "bt_message.h"
#include "bluetooth_gatts_struct.h"
#include "bt_utils.h"

#if defined(__BT_4_0_BLE__)
#include "btif_gatt.h"
//#include "btif_gatt_util.h"

typedef struct btif_gatts_cb{
    uint8_t used[GATT_MAX_SERVER];
    uint8_t appuuid[GATT_MAX_SERVER][16];
    void *regContext[GATT_MAX_SERVER];
    int regCount;
}btif_gatts_cb_t; 

static btif_gatts_cb_t g_gatts_cb;

/************************************************************************************
**  Constants & Macros
************************************************************************************/
#define CHECK_BTGATT_INIT() if (bt_gatt_callbacks == NULL)\
    {\
        ALOGW("%s: BTGATT not initialized", __FUNCTION__);\
        return BT_STATUS_NOT_READY;\
    } else {\
        ALOGD("%s", __FUNCTION__);\
    }

extern const btgatt_callbacks_t *bt_gatt_callbacks;

#ifdef __DROID_STACK__
typedef enum {
    BTIF_GATTS_REGISTER_APP = 2000,
    BTIF_GATTS_UNREGISTER_APP,
    BTIF_GATTS_OPEN,
    BTIF_GATTS_CLOSE,
    BTIF_GATTS_CREATE_SERVICE,
    BTIF_GATTS_ADD_INCLUDED_SERVICE,
    BTIF_GATTS_ADD_CHARACTERISTIC,
    BTIF_GATTS_ADD_DESCRIPTOR,
    BTIF_GATTS_START_SERVICE,
    BTIF_GATTS_STOP_SERVICE,
    BTIF_GATTS_DELETE_SERVICE,
    BTIF_GATTS_SEND_INDICATION,
    BTIF_GATTS_SEND_RESPONSE
} btif_gatts_event_t;

/************************************************************************************
**  Local type definitions
************************************************************************************/

typedef struct
{
    uint8_t             value[BTGATT_MAX_ATTR_LEN];
    btgatt_response_t   response;
    btgatt_srvc_id_t    srvc_id;
    bt_bdaddr_t         bd_addr;
    bt_uuid_t           uuid;
    uint32_t            trans_id;
    uint16_t            conn_id;
    uint16_t            srvc_handle;
    uint16_t            incl_handle;
    uint16_t            attr_handle;
    uint16_t            permissions;
    uint16_t            len;
    uint8_t             server_if;
    uint8_t             is_direct;
    uint8_t             num_handles;
    uint8_t             properties;
    uint8_t             transport;
    uint8_t             confirm;
    uint8_t             status;
} __attribute__((packed)) btif_gatts_cb_t;


/************************************************************************************
**  Static variables
************************************************************************************/

extern const btgatt_callbacks_t *bt_gatt_callbacks;


/************************************************************************************
**  Static functions
************************************************************************************/

static void btapp_gatts_copy_req_data(UINT16 event, char *p_dest, char *p_src)
{
    tBTA_GATTS *p_dest_data = (tBTA_GATTS*) p_dest;
    tBTA_GATTS *p_src_data = (tBTA_GATTS*) p_src;

    if (!p_src_data || !p_dest_data)
        return;

    // Copy basic structure first
    memcpy(p_dest_data, p_src_data, sizeof(tBTA_GATTS));

    // Allocate buffer for request data if necessary
    switch (event)
    {
        case BTA_GATTS_READ_EVT:
        case BTA_GATTS_WRITE_EVT:
        case BTA_GATTS_EXEC_WRITE_EVT:
        case BTA_GATTS_MTU_EVT:
            p_dest_data->req_data.p_data = GKI_getbuf(sizeof(tBTA_GATTS_REQ_DATA));
            if (p_dest_data->req_data.p_data != NULL)
            {
                memcpy(p_dest_data->req_data.p_data, p_src_data->req_data.p_data,
                    sizeof(tBTA_GATTS_REQ_DATA));
            }
            break;

        default:
            break;
    }
}

static void btapp_gatts_free_req_data(UINT16 event, tBTA_GATTS *p_data)
{
    switch (event)
    {
        case BTA_GATTS_READ_EVT:
        case BTA_GATTS_WRITE_EVT:
        case BTA_GATTS_EXEC_WRITE_EVT:
        case BTA_GATTS_MTU_EVT:
            if (p_data && p_data->req_data.p_data)
                GKI_freebuf(p_data->req_data.p_data);
            break;

        default:
            break;
    }
}

static void btapp_gatts_handle_cback(uint16_t event, char* p_param)
{
    ALOGD("%s: Event %d", __FUNCTION__, event);

    tBTA_GATTS *p_data = (tBTA_GATTS*)p_param;
    switch (event)
    {
        case BTA_GATTS_REG_EVT:
        {
            bt_uuid_t app_uuid;
            bta_to_btif_uuid(&app_uuid, &p_data->reg_oper.uuid);
            HAL_CBACK(bt_gatt_callbacks, server->register_server_cb
                , p_data->reg_oper.status
                , p_data->reg_oper.server_if
                , &app_uuid
            );
            break;
        }

        case BTA_GATTS_DEREG_EVT:
            break;

        case BTA_GATTS_CONNECT_EVT:
        {
            bt_bdaddr_t bda;
            bdcpy(bda.address, p_data->conn.remote_bda);

            btif_gatt_check_encrypted_link(p_data->conn.remote_bda);

            HAL_CBACK(bt_gatt_callbacks, server->connection_cb,
                      p_data->conn.conn_id, p_data->conn.server_if, TRUE, &bda);
            break;
        }

        case BTA_GATTS_DISCONNECT_EVT:
        {
            bt_bdaddr_t bda;
            bdcpy(bda.address, p_data->conn.remote_bda);

            HAL_CBACK(bt_gatt_callbacks, server->connection_cb,
                      p_data->conn.conn_id, p_data->conn.server_if, FALSE, &bda);

            btif_gatt_remove_encrypted_link(p_data->conn.remote_bda);
            break;
        }

        case BTA_GATTS_CREATE_EVT:
        {
            btgatt_srvc_id_t srvc_id;
            srvc_id.is_primary = p_data->create.is_primary;
            srvc_id.id.inst_id = p_data->create.svc_instance;
            bta_to_btif_uuid(&srvc_id.id.uuid, &p_data->create.uuid);

            HAL_CBACK(bt_gatt_callbacks, server->service_added_cb,
                      p_data->create.status, p_data->create.server_if, &srvc_id,
                      p_data->create.service_id
            );
        }
        break;

        case BTA_GATTS_ADD_INCL_SRVC_EVT:
            HAL_CBACK(bt_gatt_callbacks, server->included_service_added_cb,
                      p_data->add_result.status,
                      p_data->add_result.server_if,
                      p_data->add_result.service_id,
                      p_data->add_result.attr_id);
            break;

        case BTA_GATTS_ADD_CHAR_EVT:
        {
            bt_uuid_t uuid;
            bta_to_btif_uuid(&uuid, &p_data->add_result.char_uuid);

            HAL_CBACK(bt_gatt_callbacks, server->characteristic_added_cb,
                      p_data->add_result.status,
                      p_data->add_result.server_if,
                      &uuid,
                      p_data->add_result.service_id,
                      p_data->add_result.attr_id);
            break;
        }

        case BTA_GATTS_ADD_CHAR_DESCR_EVT:
        {
            bt_uuid_t uuid;
            bta_to_btif_uuid(&uuid, &p_data->add_result.char_uuid);

            HAL_CBACK(bt_gatt_callbacks, server->descriptor_added_cb,
                      p_data->add_result.status,
                      p_data->add_result.server_if,
                      &uuid,
                      p_data->add_result.service_id,
                      p_data->add_result.attr_id);
            break;
        }

        case BTA_GATTS_START_EVT:
            HAL_CBACK(bt_gatt_callbacks, server->service_started_cb,
                      p_data->srvc_oper.status,
                      p_data->srvc_oper.server_if,
                      p_data->srvc_oper.service_id);
            break;

        case BTA_GATTS_STOP_EVT:
            HAL_CBACK(bt_gatt_callbacks, server->service_stopped_cb,
                      p_data->srvc_oper.status,
                      p_data->srvc_oper.server_if,
                      p_data->srvc_oper.service_id);
            break;

        case BTA_GATTS_DELELTE_EVT:
            HAL_CBACK(bt_gatt_callbacks, server->service_deleted_cb,
                      p_data->srvc_oper.status,
                      p_data->srvc_oper.server_if,
                      p_data->srvc_oper.service_id);
            break;

        case BTA_GATTS_READ_EVT:
        {
            bt_bdaddr_t bda;
            bdcpy(bda.address, p_data->req_data.remote_bda);

            HAL_CBACK(bt_gatt_callbacks, server->request_read_cb,
                      p_data->req_data.conn_id,p_data->req_data.trans_id, &bda,
                      p_data->req_data.p_data->read_req.handle,
                      p_data->req_data.p_data->read_req.offset,
                      p_data->req_data.p_data->read_req.is_long);
            break;
        }

        case BTA_GATTS_WRITE_EVT:
        {
            bt_bdaddr_t bda;
            bdcpy(bda.address, p_data->req_data.remote_bda);

            HAL_CBACK(bt_gatt_callbacks, server->request_write_cb,
                      p_data->req_data.conn_id,p_data->req_data.trans_id, &bda,
                      p_data->req_data.p_data->write_req.handle,
                      p_data->req_data.p_data->write_req.offset,
                      p_data->req_data.p_data->write_req.len,
                      p_data->req_data.p_data->write_req.need_rsp,
                      p_data->req_data.p_data->write_req.is_prep,
                      p_data->req_data.p_data->write_req.value);
            break;
        }

        case BTA_GATTS_EXEC_WRITE_EVT:
        {
            bt_bdaddr_t bda;
            bdcpy(bda.address, p_data->req_data.remote_bda);

            HAL_CBACK(bt_gatt_callbacks, server->request_exec_write_cb,
                      p_data->req_data.conn_id,p_data->req_data.trans_id, &bda,
                      p_data->req_data.p_data->exec_write);
            break;
        }

        case BTA_GATTS_MTU_EVT:
        case BTA_GATTS_OPEN_EVT:
        case BTA_GATTS_CANCEL_OPEN_EVT:
        case BTA_GATTS_CLOSE_EVT:
            ALOGD("%s: Empty event (%d)!", __FUNCTION__, event);
            break;

        default:
            ALOGE("%s: Unhandled event (%d)!", __FUNCTION__, event);
            break;
    }

    btapp_gatts_free_req_data(event, p_data);
}

static void btapp_gatts_cback(tBTA_GATTS_EVT event, tBTA_GATTS *p_data)
{
    bt_status_t status;
    status = btif_transfer_context(btapp_gatts_handle_cback, (uint16_t) event,
        (void*)p_data, sizeof(tBTA_GATTS), btapp_gatts_copy_req_data);
    ASSERTC(status == BT_STATUS_SUCCESS, "Context transfer failed!", status);
}

static void btgatts_handle_event(uint16_t event, char* p_param)
{
    btif_gatts_cb_t* p_cb = (btif_gatts_cb_t*)p_param;
    if (!p_cb) return;

    ALOGD("%s: Event %d", __FUNCTION__, event);

    switch (event)
    {
        case BTIF_GATTS_REGISTER_APP:
        {
            tBT_UUID uuid;
            btif_to_bta_uuid(&uuid, &p_cb->uuid);
            BTA_GATTS_AppRegister(&uuid, btapp_gatts_cback);
            break;
        }

        case BTIF_GATTS_UNREGISTER_APP:
            BTA_GATTS_AppDeregister(p_cb->server_if);
            break;

        case BTIF_GATTS_OPEN:
            if (!p_cb->is_direct)
                BTA_DmBleSetBgConnType(BTM_BLE_CONN_AUTO, NULL);
            BTA_GATTS_Open(p_cb->server_if, p_cb->bd_addr.address,
                           p_cb->is_direct);
            break;

        case BTIF_GATTS_CLOSE:
            // Cancel pending foreground/background connections
            BTA_GATTS_CancelOpen(p_cb->server_if, p_cb->bd_addr.address, TRUE);
            BTA_GATTS_CancelOpen(p_cb->server_if, p_cb->bd_addr.address, FALSE);

            // Close active connection
            if (p_cb->conn_id != 0)
                BTA_GATTS_Close(p_cb->conn_id);
            break;

        case BTIF_GATTS_CREATE_SERVICE:
        {
            tBTA_GATT_SRVC_ID srvc_id;
            btif_to_bta_srvc_id(&srvc_id, &p_cb->srvc_id);
            BTA_GATTS_CreateService(p_cb->server_if, &srvc_id.id.uuid,
                                    srvc_id.id.inst_id, p_cb->num_handles,
                                    srvc_id.is_primary);
            break;
        }

        case BTIF_GATTS_ADD_INCLUDED_SERVICE:
            BTA_GATTS_AddIncludeService(p_cb->srvc_handle, p_cb->incl_handle);
            break;

        case BTIF_GATTS_ADD_CHARACTERISTIC:
        {
            tBT_UUID uuid;
            btif_to_bta_uuid(&uuid, &p_cb->uuid);

            BTA_GATTS_AddCharacteristic(p_cb->srvc_handle, &uuid,
                                        p_cb->permissions, p_cb->properties);
            break;
        }

        case BTIF_GATTS_ADD_DESCRIPTOR:
        {
            tBT_UUID uuid;
            btif_to_bta_uuid(&uuid, &p_cb->uuid);

            BTA_GATTS_AddCharDescriptor(p_cb->srvc_handle, p_cb->permissions,
                                         &uuid);
            break;
        }

        case BTIF_GATTS_START_SERVICE:
            BTA_GATTS_StartService(p_cb->srvc_handle, p_cb->transport);
            break;

        case BTIF_GATTS_STOP_SERVICE:
            BTA_GATTS_StopService(p_cb->srvc_handle);
            break;

        case BTIF_GATTS_DELETE_SERVICE:
            BTA_GATTS_DeleteService(p_cb->srvc_handle);
            break;

        case BTIF_GATTS_SEND_INDICATION:
            BTA_GATTS_HandleValueIndication(p_cb->conn_id, p_cb->attr_handle,
                                        p_cb->len, p_cb->value, p_cb->confirm);
            // TODO: Might need to send an ACK if handle value indication is
            //       invoked without need for confirmation.
            break;

        case BTIF_GATTS_SEND_RESPONSE:
        {
            tBTA_GATTS_RSP rsp_struct;
            btgatt_response_t *p_rsp = &p_cb->response;
            btif_to_bta_response(&rsp_struct, p_rsp);

            BTA_GATTS_SendRsp(p_cb->conn_id, p_cb->trans_id,
                              p_cb->status, &rsp_struct);

            HAL_CBACK(bt_gatt_callbacks, server->response_confirmation_cb,
                      0, rsp_struct.attr_value.handle);
            break;
        }

        default:
            ALOGE("%s: Unknown event (%d)!", __FUNCTION__, event);
            break;
    }
}
#endif

/********************************************
*   client_if is 1-based index
*********************************************/
static void *convertIfToContext(int server_if){
    if(server_if > 0 && server_if <= GATT_MAX_SERVER){
        return g_gatts_cb.regContext[server_if-1];
    }
    return 0;
}

/********************************************
*   return : 0 means not found
*********************************************/
static int convertContextToIf(void *regContext){
    int i;
    for(i = 0;i < GATT_MAX_SERVER;i++){
        if(regContext == g_gatts_cb.regContext[i])
            return i+1;
    }
    return 0;
}

static void freeContext(int server_if){
    if(server_if > 0 && server_if <= GATT_MAX_SERVER){
        server_if--;
        g_gatts_cb.used[server_if] = 0;
        memset(g_gatts_cb.appuuid[server_if], 0, sizeof(g_gatts_cb.appuuid[0]));
        g_gatts_cb.regContext[server_if] = 0;
    }
}

/********************************************
*   return : 0 means not found
*********************************************/
static int allocFreeContext(uint8_t *appuuid){
    int i;
    GATT_IF_API();
    gattDumpUUID("server appuuid", 16, appuuid);
    for(i = 0;i < GATT_MAX_SERVER;i++){
        if(!g_gatts_cb.used[i]){
            g_gatts_cb.used[i] = 1;
            memcpy(g_gatts_cb.appuuid[i], appuuid, 16);
            GATT_IF_DBG("alloc server slot %d", i);
            return i+1;
        }
    }
    GATT_IF_ERR("No more free context slot");
    return 0;
}

/********************************************
*   return : 0 means not found
*********************************************/
static int findContextByAppUUID(uint8_t *appuuid){
    int i;
    GATT_IF_API();
    for(i = 0;i < GATT_MAX_SERVER;i++){
        if(g_gatts_cb.used[i] && !memcmp(appuuid, g_gatts_cb.appuuid[i] , 16)){
            GATT_IF_DBG("context[%d] = %p found", i, g_gatts_cb.regContext[i]);
            return i+1;
        }
    }
    GATT_IF_WARN("No context found");
    return 0;
}

/********************************************
*   return : 0 means not found
*********************************************/
static int findContextByContext(void *ctx){
    int i;
    GATT_IF_API();
    for(i = 0;i < GATT_MAX_SERVER;i++){
        if(g_gatts_cb.used[i] && ctx == g_gatts_cb.regContext[i]){
            GATT_IF_DBG("context[%d] = %p found", i, g_gatts_cb.regContext[i]);
            return i+1;
        }
    }
    GATT_IF_WARN("No context found");
    return 0;
}

/*******************************************************************************
* Message handlers
*******************************************************************************/
/*******************************************************************************
**
** Function         gatts_handle_register_cnf
**
** Description      Handle gatts register confirm message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_register_cnf(ilm_struct *pIlm){
    bt_gatts_register_cnf_struct *pCnf = (bt_gatts_register_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int server_if = findContextByAppUUID(pCnf->user_id);
    GATT_IF_API();

    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        g_gatts_cb.regContext[server_if-1] = pCnf->reg_ctx;
        bt_gatt_callbacks->server->register_server_cb(status, server_if, (bt_uuid_t*)pCnf->user_id);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_deregister_cnf
**
** Description      Handle gatts deregister confirm message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_deregister_cnf(ilm_struct *pIlm){
    bt_gatts_deregister_cnf_struct *pCnf = (bt_gatts_deregister_cnf_struct*)pIlm->ilm_data;
    int server_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();

    if(server_if > 0 && server_if <= GATT_MAX_SERVER){
        freeContext(server_if);
    }else{
        GATT_IF_ERR("Can not find context slot. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_connect_cnf
**
** Description      Handle gatts connect confirm message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_connect_cnf(ilm_struct *pIlm){
    bt_gatts_connect_cnf_struct *pCnf = (bt_gatts_connect_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int server_if = findContextByContext(pCnf->reg_ctx);
    bt_bdaddr_t bda;
    
    GATT_IF_API();
    GATT_IF_DBG("status=%d, server_if=%d", status, server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        btmtk_util_reverse_array(bda.address, pCnf->bd_addr);        
        if(status == GATT_SUCCESS){
            bt_gatt_callbacks->server->connection_cb(GET_CONNID(server_if, pCnf->conn_ctx), server_if, 1, &bda);
        }else if(status != GATT_PENDING){
            // Connect failed
            bt_gatt_callbacks->server->connection_cb(0, server_if, 0, &bda);
        }
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_connected_ind
**
** Description      Handle gatts connected indication message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_connected_ind(ilm_struct *pIlm){
    bt_gatts_connected_ind_struct *pInd = (bt_gatts_connected_ind_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pInd->result);
    int server_if = findContextByContext(pInd->reg_ctx);
    bt_bdaddr_t bda;
    
    GATT_IF_API();
    GATT_IF_DBG("status=%d, server_if=%d", status, server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        btmtk_util_reverse_array(bda.address, pInd->bd_addr);          
        if(status == GATT_SUCCESS){
            bt_gatt_callbacks->server->connection_cb(GET_CONNID(server_if, pInd->conn_ctx), server_if, 1, &bda);
        }else{
            // Connect failed
            bt_gatt_callbacks->server->connection_cb(0, server_if, 0, &bda);
        }
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_disconnect_cnf
**
** Description      Handle gatts disconnect confirm message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_disconnect_cnf(ilm_struct *pIlm){
    bt_gatts_disconnect_cnf_struct *pCnf = (bt_gatts_disconnect_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int server_if = findContextByContext(pCnf->reg_ctx);
    bt_bdaddr_t bda;
    
    GATT_IF_API();
    GATT_IF_DBG("status=%d, server_if=%d", status, server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        btmtk_util_reverse_array(bda.address, pCnf->bd_addr);          
        if(status != GATT_PENDING){
            // Disconnect success
            bt_gatt_callbacks->server->connection_cb(GET_CONNID(server_if, pCnf->conn_ctx), server_if, 0, &bda);
        }
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_disconnected_ind
**
** Description      Handle gatts disconnected indication message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_disconnected_ind(ilm_struct *pIlm){
    bt_gatts_disconnected_ind_struct *pInd = (bt_gatts_disconnected_ind_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pInd->result);
    int server_if = findContextByContext(pInd->reg_ctx);
    bt_bdaddr_t bda;
    
    GATT_IF_API();
    GATT_IF_DBG("status=%d, server_if=%d", status, server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        btmtk_util_reverse_array(bda.address, pInd->bd_addr);          
        bt_gatt_callbacks->server->connection_cb(GET_CONNID(server_if, pInd->conn_ctx), server_if, 0, &bda);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_add_service_cnf
**
** Description      Handle gatts add service confirm message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_add_service_cnf(ilm_struct *pIlm){
    bt_gatts_add_service_cnf_struct *pCnf = (bt_gatts_add_service_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int server_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, server_if=%d", status, server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        btgatt_srvc_id_t srvc_id;
        convertSvcUUIDToSvcId(&srvc_id, &pCnf->uuid);
        bt_gatt_callbacks->server->service_added_cb(status, server_if, &srvc_id, pCnf->service_handle);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_add_incl_service_cnf
**
** Description      Handle gatts add included service confirm message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_add_incl_service_cnf(ilm_struct *pIlm){
    bt_gatts_add_incl_service_cnf_struct *pCnf = (bt_gatts_add_incl_service_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int server_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, server_if=%d", status, server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        bt_gatt_callbacks->server->included_service_added_cb(status, server_if, pCnf->service_handle, pCnf->incl_service_handle);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_add_char_cnf
**
** Description      Handle gatts add characteristic confirm message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_add_char_cnf(ilm_struct *pIlm){
    bt_gatts_add_char_cnf_struct *pCnf = (bt_gatts_add_char_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int server_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, server_if=%d", status, server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        bt_uuid_t char_id;
        convertBTUUIDToBtId(&char_id, &pCnf->char_uuid.uuid);
        bt_gatt_callbacks->server->characteristic_added_cb(status, server_if, &char_id, pCnf->service_handle, pCnf->char_handle);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_add_descr_cnf
**
** Description      Handle gatts add descriptor confirm message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_add_descr_cnf(ilm_struct *pIlm){
    bt_gatts_add_descr_cnf_struct *pCnf = (bt_gatts_add_descr_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int server_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, server_if=%d", status, server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        bt_uuid_t descr_id;
        convertBTUUIDToBtId(&descr_id, &pCnf->descr_uuid.uuid);
        bt_gatt_callbacks->server->descriptor_added_cb(status, server_if, &descr_id, pCnf->service_handle, pCnf->descr_handle);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_start_service_cnf
**
** Description      Handle gatts start service confirm message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_start_service_cnf(ilm_struct *pIlm){
    bt_gatts_start_service_cnf_struct *pCnf = (bt_gatts_start_service_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int server_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, server_if=%d", status, server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        bt_gatt_callbacks->server->service_started_cb(status, server_if, pCnf->service_handle);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_stop_service_cnf
**
** Description      Handle gatts stop service confirm message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_stop_service_cnf(ilm_struct *pIlm){
    bt_gatts_stop_service_cnf_struct *pCnf = (bt_gatts_stop_service_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int server_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, server_if=%d", status, server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        bt_gatt_callbacks->server->service_stopped_cb(status, server_if, pCnf->service_handle);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_delete_service_cnf
**
** Description      Handle gatts delete service confirm message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_delete_service_cnf(ilm_struct *pIlm){
    bt_gatts_delete_service_cnf_struct *pCnf = (bt_gatts_delete_service_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int server_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, server_if=%d", status, server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        bt_gatt_callbacks->server->service_deleted_cb(status, server_if, pCnf->service_handle);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_send_indication_cnf
**
** Description      Handle gatts send indication confirm message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_send_indication_cnf(ilm_struct *pIlm){
    bt_gatts_send_indication_cnf_struct *pCnf = (bt_gatts_send_indication_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int server_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, server_if=%d", status, server_if);
    GATT_IF_DBG("need_confirm=%u", pCnf->need_confirm);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        if(pCnf->need_confirm){
            bt_gatt_callbacks->server->response_confirmation_cb(status, pCnf->handle);
        }
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_send_response_cnf
**
** Description      Handle gatts send response confirm message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_send_response_cnf(ilm_struct *pIlm){
    bt_gatts_send_response_cnf_struct *pCnf = (bt_gatts_send_response_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int server_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, server_if=%d", status, server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        bt_gatt_callbacks->server->response_confirmation_cb(status, pCnf->handle);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_read_request_ind
**
** Description      Handle gatts read request indication message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_read_request_ind(ilm_struct *pIlm){
    bt_gatts_read_request_ind_struct *pInd = (bt_gatts_read_request_ind_struct*)pIlm->ilm_data;
    int server_if = findContextByContext(pInd->reg_ctx);
    bt_bdaddr_t bda;
    
    GATT_IF_API();
    GATT_IF_DBG("server_if=%d", server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        btmtk_util_reverse_array(bda.address, pInd->bd_addr);         
        bt_gatt_callbacks->server->request_read_cb(GET_CONNID(server_if, pInd->conn_ctx),
                                                                         pInd->trans_id,
                                                                         &bda,
                                                                         pInd->handle,
                                                                         pInd->offset,
                                                                         pInd->is_long);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_write_request_ind
**
** Description      Handle gatts write request indication message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_write_request_ind(ilm_struct *pIlm){
    bt_gatts_write_request_ind_struct *pInd = (bt_gatts_write_request_ind_struct*)pIlm->ilm_data;
    int server_if = findContextByContext(pInd->reg_ctx);
    bt_bdaddr_t bda;
    
    GATT_IF_API();
    GATT_IF_DBG("server_if=%d", server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        btmtk_util_reverse_array(bda.address, pInd->bd_addr);          
        bt_gatt_callbacks->server->request_write_cb(GET_CONNID(server_if, pInd->conn_ctx),
                                                                         pInd->trans_id,
                                                                         &bda,
                                                                         pInd->handle,
                                                                         pInd->offset,
                                                                         pInd->value.len,
                                                                         pInd->need_rsp,
                                                                         pInd->is_prepare,
                                                                         pInd->value.value);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gatts_handle_execute_write_request_ind
**
** Description      Handle gatts execute write request indication message
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_execute_write_request_ind(ilm_struct *pIlm){
    bt_gatts_exec_write_request_ind_struct *pInd = (bt_gatts_exec_write_request_ind_struct*)pIlm->ilm_data;
    int server_if = findContextByContext(pInd->reg_ctx);
    bt_bdaddr_t bda;
    
    GATT_IF_API();
    GATT_IF_DBG("server_if=%d", server_if);
    if(server_if > 0 && server_if <= GATT_MAX_SERVER && bt_gatt_callbacks){
        btmtk_util_reverse_array(bda.address, pInd->bd_addr);         
        bt_gatt_callbacks->server->request_exec_write_cb(GET_CONNID(server_if, pInd->conn_ctx),
                                                                                  pInd->trans_id,
                                                                                  &bda,
                                                                                  pInd->cancel ? 0 : 1);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}



/*******************************************************************************
**
** Function         gatt_handle_message
**
** Description      Handle gatt message from internal adp
**
** Returns          void
**
*******************************************************************************/
void gatts_handle_message(void *pContext, ilm_struct *pIlm){
    U16 msg_size;
    GATT_IF_API();
    

    msg_size = *((U16*)&pIlm->ilm_data[2]);

    GATT_IF_DBG("gatts_handle_message : msg_id=%u, msg_size=%u", (unsigned int)pIlm->msg_id, msg_size);
    
    switch(pIlm->msg_id){
    case MSG_ID_BT_GATTS_REGISTER_REQ:
    case MSG_ID_BT_GATTS_DEREGISTER_REQ:
    case MSG_ID_BT_GATTS_CONNECT_REQ:        
    case MSG_ID_BT_GATTS_DISCONNECT_REQ:
    case MSG_ID_BT_GATTS_ADD_SERVICE_REQ:
    case MSG_ID_BT_GATTS_ADD_INCL_SERVICE_REQ:
    case MSG_ID_BT_GATTS_ADD_CHAR_REQ:
    case MSG_ID_BT_GATTS_ADD_DESCR_REQ:
    case MSG_ID_BT_GATTS_START_SERVICE_REQ:
    case MSG_ID_BT_GATTS_STOP_SERVICE_REQ:
    case MSG_ID_BT_GATTS_DELETE_SERVICE_REQ:
    case MSG_ID_BT_GATTS_SEND_INDICATION_REQ:
    case MSG_ID_BT_GATTS_SEND_RESPONSE_REQ:
        btmtk_sendmsg(PROFILE_GATTS, pIlm, msg_size);
        break;
    case MSG_ID_BT_GATTS_REGISTER_CNF:
        gatts_handle_register_cnf(pIlm);
        break;
        
    case MSG_ID_BT_GATTS_DEREGISTER_CNF:
        gatts_handle_deregister_cnf(pIlm);
        break;
        
    case MSG_ID_BT_GATTS_CONNECT_CNF:
        gatts_handle_connect_cnf(pIlm);
        break;
        
    case MSG_ID_BT_GATTS_CONNECTED_IND:
        gatts_handle_connected_ind(pIlm);
        break;

    case MSG_ID_BT_GATTS_DISCONNECT_CNF:
        gatts_handle_disconnect_cnf(pIlm);
        break;
        
    case MSG_ID_BT_GATTS_DISCONNECTED_IND:
        gatts_handle_disconnected_ind(pIlm);
        break;

    case MSG_ID_BT_GATTS_ADD_SERVICE_CNF:
        gatts_handle_add_service_cnf(pIlm);
        break;

    case MSG_ID_BT_GATTS_ADD_INCL_SERVICE_CNF:
        gatts_handle_add_incl_service_cnf(pIlm);
        break;

    case MSG_ID_BT_GATTS_ADD_CHAR_CNF:
        gatts_handle_add_char_cnf(pIlm);
        break;

    case MSG_ID_BT_GATTS_ADD_DESCR_CNF:
        gatts_handle_add_descr_cnf(pIlm);
        break;

    case MSG_ID_BT_GATTS_START_SERVICE_CNF:
        gatts_handle_start_service_cnf(pIlm);
        break;

    case MSG_ID_BT_GATTS_STOP_SERVICE_CNF:
        gatts_handle_stop_service_cnf(pIlm);
        break;

    case MSG_ID_BT_GATTS_DELETE_SERVICE_CNF:
        gatts_handle_delete_service_cnf(pIlm);
        break;

    case MSG_ID_BT_GATTS_SEND_INDICATION_CNF:
        gatts_handle_send_indication_cnf(pIlm);
        break;

    case MSG_ID_BT_GATTS_SEND_RESPONSE_CNF:
        gatts_handle_send_response_cnf(pIlm);
        break;

    /* Indication messages */
    case MSG_ID_BT_GATTS_READ_REQUEST_IND:
        gatts_handle_read_request_ind(pIlm);
        break;

    case MSG_ID_BT_GATTS_WRITE_REQUEST_IND:
        gatts_handle_write_request_ind(pIlm);
        break;

    case MSG_ID_BT_GATTS_EXEC_WRITE_REQUEST_IND:
        gatts_handle_execute_write_request_ind(pIlm);
        break;        
        
    default:
        GATT_IF_WARN("Unknown message : %u", (unsigned int)pIlm->msg_id);
        break;
    }
}

/*******************************************************************************
**
** Function         gatts_handle_disconnect_callback
**
** Description      diconnect the currect active gatt link
**
** Returns          void
**
*******************************************************************************/
static void gatts_handle_disconnect_callback(void){
    GATT_IF_API();
}

bt_status_t btif_gatts_init( const btgatt_server_callbacks_t *callbacks ){
    memset(&g_gatts_cb, 0, sizeof(g_gatts_cb));
    btmtk_profile_register(PROFILE_GATTS, gatts_handle_message, (void*)PROFILE_GATTS);
    return BT_STATUS_SUCCESS;
}

void  btif_gatts_cleanup( void ){
    btmtk_profile_unregister(PROFILE_GATTS);
}

/************************************************************************************
**  Server API Functions
************************************************************************************/
static bt_status_t btif_gatts_register_app(bt_uuid_t *uuid)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    int free_idx = allocFreeContext(uuid->uu);
    bt_gatts_register_req_struct *req = (bt_gatts_register_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTS_REGISTER_REQ;
    if(uuid){
        if(free_idx > 0 && free_idx <= GATT_MAX_SERVER){
            memcpy(req->user_id, uuid->uu, 16);
            if(!btmtk_sendevt(PROFILE_GATTS, (void*)&ilm, sizeof(bt_gatts_register_req_struct))){
                GATT_IF_ERR("send MSG_ID_BT_GATTS_REGISTER_REQ failed");
                status = BT_STATUS_FAIL;
            }
        }else{
            GATT_IF_ERR("uuid is NULL");
            status = BT_STATUS_PARM_INVALID;
        }
    }else{
        GATT_IF_ERR("uuid is NULL");
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gatts_unregister_app( int server_if )
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gatts_deregister_req_struct *req = (bt_gatts_deregister_req_struct*)ilm.ilm_data;

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTS_DEREGISTER_REQ;
    req->reg_ctx = convertIfToContext(server_if);
    if(req->reg_ctx){
        if(!btmtk_sendevt(PROFILE_GATTS, (void*)&ilm, sizeof(bt_gatts_deregister_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTS_DEREGISTER_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid server_if %d", server_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gatts_open( int server_if, const bt_bdaddr_t *bd_addr, bool is_direct )
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gatts_connect_req_struct *req = (bt_gatts_connect_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTS_CONNECT_REQ;
    req->reg_ctx = convertIfToContext(server_if);
    if(req->reg_ctx){
        btmtk_util_reverse_array(req->bd_addr, bd_addr->address);       
        req->direct = is_direct ? 1 : 0;
        if(!btmtk_sendevt(PROFILE_GATTS, (void*)&ilm, sizeof(bt_gatts_connect_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTS_CONNECT_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid server_if %d", server_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;

}

static bt_status_t btif_gatts_close(int server_if, const bt_bdaddr_t *bd_addr, int conn_id)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gatts_disconnect_req_struct *req = (bt_gatts_disconnect_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTS_DISCONNECT_REQ;
    req->reg_ctx = convertIfToContext(server_if);
    if(req->reg_ctx){
        req->conn_ctx = (void*)GET_CONN_FROM_CONNID(conn_id);
        btmtk_util_reverse_array(req->bd_addr, bd_addr->address);        
        if(!btmtk_sendevt(PROFILE_GATTS, (void*)&ilm, sizeof(bt_gatts_disconnect_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTS_DISCONNECT_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid server_if %d", server_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gatts_add_service(int server_if, btgatt_srvc_id_t *srvc_id,
                                          int num_handles)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gatts_add_service_req_struct *req = (bt_gatts_add_service_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTS_ADD_SERVICE_REQ;
    req->reg_ctx = convertIfToContext(server_if);
    if(req->reg_ctx){
        convertSvcIdToSvcUUID(&req->uuid, srvc_id);
        req->num_handles = num_handles;
        if(!btmtk_sendevt(PROFILE_GATTS, (void*)&ilm, sizeof(bt_gatts_add_service_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTS_ADD_SERVICE_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid server_if %d", server_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gatts_add_included_service(int server_if, int service_handle,
                                                   int included_handle)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gatts_add_incl_service_req_struct *req = (bt_gatts_add_incl_service_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTS_ADD_INCL_SERVICE_REQ;
    req->reg_ctx = convertIfToContext(server_if);
    if(req->reg_ctx){
        req->service_handle = (U16)service_handle;
        req->incl_service_handle = (U16)included_handle;
        if(!btmtk_sendevt(PROFILE_GATTS, (void*)&ilm, sizeof(bt_gatts_add_incl_service_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTS_ADD_INCL_SERVICE_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid server_if %d", server_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gatts_add_characteristic(int server_if, int service_handle,
                                                 bt_uuid_t *uuid, int properties,
                                                 int permissions)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gatts_add_char_req_struct *req = (bt_gatts_add_char_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTS_ADD_CHAR_REQ;
    req->reg_ctx = convertIfToContext(server_if);
    if(req->reg_ctx){
        req->service_handle = (U16)service_handle;
        req->properties = (U32)properties;
        req->permission = (U32)permissions;
        convertBtIdToBTUUID(&req->char_uuid.uuid, uuid);
        if(!btmtk_sendevt(PROFILE_GATTS, (void*)&ilm, sizeof(bt_gatts_add_char_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTS_ADD_CHAR_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid server_if %d", server_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gatts_add_descriptor(int server_if, int service_handle, bt_uuid_t *uuid,
                                             int permissions)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gatts_add_descr_req_struct *req = (bt_gatts_add_descr_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTS_ADD_DESCR_REQ;
    req->reg_ctx = convertIfToContext(server_if);
    if(req->reg_ctx){
        req->service_handle = (U16)service_handle;
        req->descr_uuid.inst = 0;
        convertBtIdToBTUUID(&req->descr_uuid.uuid, uuid);
        req->permission = (U32)permissions;
        if(!btmtk_sendevt(PROFILE_GATTS, (void*)&ilm, sizeof(bt_gatts_add_descr_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTS_ADD_DESCR_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid server_if %d", server_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gatts_start_service(int server_if, int service_handle, int transport)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gatts_start_service_req_struct *req = (bt_gatts_start_service_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTS_START_SERVICE_REQ;
    req->reg_ctx = convertIfToContext(server_if);
    if(req->reg_ctx){
        req->service_handle = (U16)service_handle;
        req->transport = (U8)transport;
        if(!btmtk_sendevt(PROFILE_GATTS, (void*)&ilm, sizeof(bt_gatts_start_service_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTS_START_SERVICE_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid server_if %d", server_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gatts_stop_service(int server_if, int service_handle)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gatts_stop_service_req_struct *req = (bt_gatts_stop_service_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTS_STOP_SERVICE_REQ;
    req->reg_ctx = convertIfToContext(server_if);
    if(req->reg_ctx){
        req->service_handle = (U16)service_handle;
        if(!btmtk_sendevt(PROFILE_GATTS, (void*)&ilm, sizeof(bt_gatts_stop_service_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTS_STOP_SERVICE_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid server_if %d", server_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gatts_delete_service(int server_if, int service_handle)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gatts_delete_service_req_struct *req = (bt_gatts_delete_service_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTS_DELETE_SERVICE_REQ;
    req->reg_ctx = convertIfToContext(server_if);
    if(req->reg_ctx){
        req->service_handle = (U16)service_handle;
        if(!btmtk_sendevt(PROFILE_GATTS, (void*)&ilm, sizeof(bt_gatts_delete_service_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTS_DELETE_SERVICE_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid server_if %d", server_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gatts_send_indication(int server_if, int attribute_handle, int conn_id,
                                              int len, int confirm, char* p_value)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gatts_send_indication_req_struct *req = (bt_gatts_send_indication_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTS_SEND_INDICATION_REQ;
    req->reg_ctx = convertIfToContext(server_if);
    if(req->reg_ctx){
        if(len && p_value){
            req->handle = (U16)attribute_handle;
            req->conn_ctx = (void*)GET_CONN_FROM_CONNID(conn_id);
            req->need_confirm = confirm ? 1 : 0;
            req->value.len = (U16)len;
            memcpy(req->value.value, p_value, len);
            if(!btmtk_sendevt(PROFILE_GATTS, (void*)&ilm, sizeof(bt_gatts_send_indication_req_struct))){
                GATT_IF_ERR("send MSG_ID_BT_GATTS_SEND_INDICATION_REQ failed");
                status = BT_STATUS_FAIL;
            }
        }else{
            GATT_IF_WARN("Invalid value : len=%d, p_value=%p", len, p_value);
            status = BT_STATUS_PARM_INVALID;
        }
    }else{
        GATT_IF_ERR("Invalid server_if %d", server_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gatts_send_response(int conn_id, int trans_id,
                                            int status, btgatt_response_t *response)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    int server_if = GET_IF_FROM_CONNID(conn_id);
    bt_gatts_send_response_req_struct *req = (bt_gatts_send_response_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTS_SEND_RESPONSE_REQ;
    req->reg_ctx = convertIfToContext(server_if);
    if(req->reg_ctx){
        if(response){
            req->result = (U8)status;
            req->handle = (U16)response->handle;
            req->trans_id = (U16)trans_id;
            req->conn_ctx = (void*)GET_CONN_FROM_CONNID(conn_id);
            req->value.len = (U16)response->attr_value.len;
            if(response->attr_value.len){
                memcpy(req->value.value, response->attr_value.value, response->attr_value.len);
            }
            if(!btmtk_sendevt(PROFILE_GATTS, (void*)&ilm, sizeof(bt_gatts_send_response_req_struct))){
                GATT_IF_ERR("send MSG_ID_BT_GATTS_SEND_RESPONSE_REQ failed");
                ret = BT_STATUS_FAIL;
            }
        }else{
            GATT_IF_WARN("Response is null");
            ret = BT_STATUS_PARM_INVALID;
        }
    }else{
        GATT_IF_ERR("Invalid server_if %d", server_if);
        ret = BT_STATUS_PARM_INVALID;
    }
    return ret;
}

const btgatt_server_interface_t btgattServerInterface = {
    btif_gatts_register_app,
    btif_gatts_unregister_app,
    btif_gatts_open,
    btif_gatts_close,
    btif_gatts_add_service,
    btif_gatts_add_included_service,
    btif_gatts_add_characteristic,
    btif_gatts_add_descriptor,
    btif_gatts_start_service,
    btif_gatts_stop_service,
    btif_gatts_delete_service,
    btif_gatts_send_indication,
    btif_gatts_send_response
};

#endif
