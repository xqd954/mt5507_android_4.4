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


/*******************************************************************************
 *
 *  Filename:      btif_gatt_client.c
 *
 *  Description:   GATT client implementation
 *
 *******************************************************************************/

#include <hardware/bluetooth.h>
#include <hardware/bt_gatt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define LOG_TAG "BTIF_GATT_CLIENT"
#include "bt_ext_debug.h"
#include "bt_ext_common.h"
#include "bt_ext_apis.h"
#include "btif_common.h"
#include "bt_message.h"
#include "bluetooth_gattc_struct.h"
#include "bt_utils.h"

#if defined(__BT_4_0_BLE__)
#include "btif_gatt.h"
//#include "btif_gatt_util.h"

typedef struct btif_gattc_cb{
    uint8_t used[GATT_MAX_CLIENT];
    uint8_t appuuid[GATT_MAX_CLIENT][16];
    void *regContext[GATT_MAX_CLIENT];
    int regCount;
    int gattc_fd;
}btif_gattc_cb_t; 

static btif_gattc_cb_t g_gattc_cb;

/*******************************************************************************
**  Constants & Macros
********************************************************************************/
#define CHECK_BTGATT_INIT() if (bt_gatt_callbacks == NULL)\
    {\
        ALOGW("%s: BTGATT not initialized", __FUNCTION__);\
        return BT_STATUS_NOT_READY;\
    } else {\
        ALOGD("%s", __FUNCTION__);\
    }


extern const btgatt_callbacks_t *bt_gatt_callbacks;

#if 0
typedef enum {
    BTIF_GATTC_REGISTER_APP = 1000,
    BTIF_GATTC_UNREGISTER_APP,
    BTIF_GATTC_SCAN_START,
    BTIF_GATTC_SCAN_STOP,
    BTIF_GATTC_OPEN,
    BTIF_GATTC_CLOSE,
    BTIF_GATTC_SEARCH_SERVICE,
    BTIF_GATTC_GET_FIRST_CHAR,
    BTIF_GATTC_GET_NEXT_CHAR,
    BTIF_GATTC_GET_FIRST_CHAR_DESCR,
    BTIF_GATTC_GET_NEXT_CHAR_DESCR,
    BTIF_GATTC_GET_FIRST_INCL_SERVICE,
    BTIF_GATTC_GET_NEXT_INCL_SERVICE,
    BTIF_GATTC_READ_CHAR,
    BTIF_GATTC_READ_CHAR_DESCR,
    BTIF_GATTC_WRITE_CHAR,
    BTIF_GATTC_WRITE_CHAR_DESCR,
    BTIF_GATTC_EXECUTE_WRITE,
    BTIF_GATTC_REG_FOR_NOTIFICATION,
    BTIF_GATTC_DEREG_FOR_NOTIFICATION,
    BTIF_GATTC_REFRESH,
    BTIF_GATTC_READ_RSSI
} btif_gattc_event_t;

#define BTIF_GATT_MAX_OBSERVED_DEV 40

#define BTIF_GATT_OBSERVE_EVT   0x1000
#define BTIF_GATTC_RSSI_EVT     0x1001

/*******************************************************************************
**  Local type definitions
********************************************************************************/

typedef struct
{
    uint8_t     value[BTGATT_MAX_ATTR_LEN];
    bt_bdaddr_t bd_addr;
    btgatt_srvc_id_t srvc_id;
    btgatt_srvc_id_t incl_srvc_id;
    btgatt_gatt_id_t char_id;
    bt_uuid_t   uuid;
    uint16_t    conn_id;
    uint16_t    len;
    uint8_t     client_if;
    uint8_t     action;
    uint8_t     is_direct;
    uint8_t     search_all;
    uint8_t     auth_req;
    uint8_t     write_type;
    uint8_t     status;
    uint8_t     addr_type;
    int8_t      rssi;
    tBT_DEVICE_TYPE device_type;
} __attribute__((packed)) btif_gattc_cb_t;

typedef struct
{
    bt_bdaddr_t bd_addr;
    BOOLEAN     in_use;
}__attribute__((packed)) btif_gattc_dev_t;

typedef struct
{
    btif_gattc_dev_t remote_dev[BTIF_GATT_MAX_OBSERVED_DEV];
    uint8_t        addr_type;
    uint8_t        next_storage_idx;
}__attribute__((packed)) btif_gattc_dev_cb_t;

/*******************************************************************************
**  Static variables
********************************************************************************/

extern const btgatt_callbacks_t *bt_gatt_callbacks;
static btif_gattc_dev_cb_t  btif_gattc_dev_cb;
static btif_gattc_dev_cb_t  *p_dev_cb = &btif_gattc_dev_cb;
static uint8_t rssi_request_client_if;

/*******************************************************************************
**  Static functions
********************************************************************************/

static void btif_gattc_init_dev_cb(void)
{
    memset(p_dev_cb, 0, sizeof(btif_gattc_dev_cb_t));
}

static void btif_gattc_add_remote_bdaddr (BD_ADDR p_bda, uint8_t addr_type)
{
    BOOLEAN found=FALSE;
    uint8_t i;
    for (i = 0; i < BTIF_GATT_MAX_OBSERVED_DEV; i++)
    {
        if (!p_dev_cb->remote_dev[i].in_use )
        {
            memcpy(p_dev_cb->remote_dev[i].bd_addr.address, p_bda, BD_ADDR_LEN);
            p_dev_cb->addr_type = addr_type;
            p_dev_cb->remote_dev[i].in_use = TRUE;
            ALOGD("%s device added idx=%d", __FUNCTION__, i  );
            break;
        }
    }

    if ( i == BTIF_GATT_MAX_OBSERVED_DEV)
    {
        i= p_dev_cb->next_storage_idx;
        memcpy(p_dev_cb->remote_dev[i].bd_addr.address, p_bda, BD_ADDR_LEN);
        p_dev_cb->addr_type = addr_type;
        p_dev_cb->remote_dev[i].in_use = TRUE;
        ALOGD("%s device overwrite idx=%d", __FUNCTION__, i  );
        p_dev_cb->next_storage_idx++;
        if(p_dev_cb->next_storage_idx >= BTIF_GATT_MAX_OBSERVED_DEV)
               p_dev_cb->next_storage_idx = 0;
    }
}

static BOOLEAN btif_gattc_find_bdaddr (BD_ADDR p_bda)
{
    uint8_t i;
    for (i = 0; i < BTIF_GATT_MAX_OBSERVED_DEV; i++)
    {
        if (p_dev_cb->remote_dev[i].in_use &&
            !memcmp(p_dev_cb->remote_dev[i].bd_addr.address, p_bda, BD_ADDR_LEN))
        {
            return TRUE;
        }
    }
    return FALSE;
}

static void btif_gattc_update_properties ( btif_gattc_cb_t *p_btif_cb )
{
    uint8_t remote_name_len;
    uint8_t *p_eir_remote_name=NULL;
    bt_bdname_t bdname;

    p_eir_remote_name = BTA_CheckEirData(p_btif_cb->value,
                                         BTM_EIR_COMPLETE_LOCAL_NAME_TYPE, &remote_name_len);

    if(p_eir_remote_name == NULL)
    {
        p_eir_remote_name = BTA_CheckEirData(p_btif_cb->value,
                                BT_EIR_SHORTENED_LOCAL_NAME_TYPE, &remote_name_len);
    }

    if(p_eir_remote_name)
    {
         memcpy(bdname.name, p_eir_remote_name, remote_name_len);
         bdname.name[remote_name_len]='\0';

        ALOGD("%s BLE device name=%s len=%d dev_type=%d", __FUNCTION__, bdname.name,
              remote_name_len, p_btif_cb->device_type  );
        btif_dm_update_ble_remote_properties( p_btif_cb->bd_addr.address,   bdname.name,
                                               p_btif_cb->device_type);
    }

    btif_storage_set_remote_addr_type( &p_btif_cb->bd_addr, p_btif_cb->addr_type);
}

static void btif_gattc_upstreams_evt(uint16_t event, char* p_param)
{
    ALOGD("%s: Event %d", __FUNCTION__, event);

    tBTA_GATTC *p_data = (tBTA_GATTC*)p_param;
    switch (event)
    {
        case BTA_GATTC_REG_EVT:
        {
            bt_uuid_t app_uuid;
            bta_to_btif_uuid(&app_uuid, &p_data->reg_oper.app_uuid);
            HAL_CBACK(bt_gatt_callbacks, client->register_client_cb
                , p_data->reg_oper.status
                , p_data->reg_oper.client_if
                , &app_uuid
            );
            break;
        }

        case BTA_GATTC_DEREG_EVT:
            break;

        case BTA_GATTC_READ_CHAR_EVT:
        {
            btgatt_read_params_t data;
            set_read_value(&data, &p_data->read);

            HAL_CBACK(bt_gatt_callbacks, client->read_characteristic_cb
                , p_data->read.conn_id, p_data->read.status, &data);
            break;
        }

        case BTA_GATTC_WRITE_CHAR_EVT:
        case BTA_GATTC_PREP_WRITE_EVT:
        {
            btgatt_write_params_t data;
            bta_to_btif_srvc_id(&data.srvc_id, &p_data->write.srvc_id);
            bta_to_btif_char_id(&data.char_id, &p_data->write.char_id);

            HAL_CBACK(bt_gatt_callbacks, client->write_characteristic_cb
                , p_data->write.conn_id, p_data->write.status, &data
            );
            break;
        }

        case BTA_GATTC_EXEC_EVT:
        {
            HAL_CBACK(bt_gatt_callbacks, client->execute_write_cb
                , p_data->exec_cmpl.conn_id, p_data->exec_cmpl.status
            );
            break;
        }

        case BTA_GATTC_SEARCH_CMPL_EVT:
        {
            HAL_CBACK(bt_gatt_callbacks, client->search_complete_cb
                , p_data->search_cmpl.conn_id, p_data->search_cmpl.status);
            break;
        }

        case BTA_GATTC_SEARCH_RES_EVT:
        {
            btgatt_srvc_id_t data;
            bta_to_btif_srvc_id(&data, &(p_data->srvc_res.service_uuid));
            HAL_CBACK(bt_gatt_callbacks, client->search_result_cb
                , p_data->srvc_res.conn_id, &data);
            break;
        }

        case BTA_GATTC_READ_DESCR_EVT:
        {
            btgatt_read_params_t data;
            set_read_value(&data, &p_data->read);

            HAL_CBACK(bt_gatt_callbacks, client->read_descriptor_cb
                , p_data->read.conn_id, p_data->read.status, &data);
            break;
        }

        case BTA_GATTC_WRITE_DESCR_EVT:
        {
            btgatt_write_params_t data;
            bta_to_btif_srvc_id(&data.srvc_id, &p_data->write.srvc_id);
            bta_to_btif_char_id(&data.char_id, &p_data->write.char_id);
            bta_to_btif_uuid(&data.descr_id, &p_data->write.descr_type);

            HAL_CBACK(bt_gatt_callbacks, client->write_descriptor_cb
                , p_data->write.conn_id, p_data->write.status, &data);
            break;
        }

        case BTA_GATTC_NOTIF_EVT:
        {
            btgatt_notify_params_t data;

            bdcpy(data.bda.address, p_data->notify.bda);

            bta_to_btif_srvc_id(&data.srvc_id, &p_data->notify.char_id.srvc_id);
            bta_to_btif_char_id(&data.char_id, &p_data->notify.char_id.char_id);
            memcpy(data.value, p_data->notify.value, p_data->notify.len);

            data.is_notify = p_data->notify.is_notify;
            data.len = p_data->notify.len;

            HAL_CBACK(bt_gatt_callbacks, client->notify_cb
                , p_data->notify.conn_id, &data);

            if (p_data->notify.is_notify == FALSE)
            {
                BTA_GATTC_SendIndConfirm(p_data->notify.conn_id,
                                         &p_data->notify.char_id);
            }
            break;
        }

        case BTA_GATTC_OPEN_EVT:
        {
            bt_bdaddr_t bda;
            bdcpy(bda.address, p_data->open.remote_bda);

            if (p_data->open.status == BTA_GATT_OK)
                btif_gatt_check_encrypted_link(p_data->open.remote_bda);

            HAL_CBACK(bt_gatt_callbacks, client->open_cb, p_data->open.conn_id
                , p_data->open.status, p_data->open.client_if, &bda);
            break;
        }

        case BTA_GATTC_CLOSE_EVT:
        {
            bt_bdaddr_t bda;
            bdcpy(bda.address, p_data->close.remote_bda);
            HAL_CBACK(bt_gatt_callbacks, client->close_cb, p_data->close.conn_id
                , p_data->status, p_data->close.client_if, &bda);

            if(p_data->status == BTA_GATT_OK)
                btif_gatt_remove_encrypted_link(p_data->close.remote_bda);
            break;
        }

        case BTA_GATTC_ACL_EVT:
            ALOGD("BTA_GATTC_ACL_EVT: status = %d", p_data->status);
            /* Ignore for now */
            break;

        case BTA_GATTC_CANCEL_OPEN_EVT:
            break;

        case BTIF_GATT_OBSERVE_EVT:
        {
            btif_gattc_cb_t *p_btif_cb = (btif_gattc_cb_t*)p_param;
            if (!btif_gattc_find_bdaddr(p_btif_cb->bd_addr.address))
            {
                btif_gattc_add_remote_bdaddr(p_btif_cb->bd_addr.address, p_btif_cb->addr_type);
                btif_gattc_update_properties(p_btif_cb);
            }
            HAL_CBACK(bt_gatt_callbacks, client->scan_result_cb,
                      &p_btif_cb->bd_addr, p_btif_cb->rssi, p_btif_cb->value);
            break;
        }

        case BTIF_GATTC_RSSI_EVT:
        {
            btif_gattc_cb_t *p_btif_cb = (btif_gattc_cb_t*)p_param;
            HAL_CBACK(bt_gatt_callbacks, client->read_remote_rssi_cb, p_btif_cb->client_if,
                      &p_btif_cb->bd_addr, p_btif_cb->rssi, p_btif_cb->status);
            break;
        }

        default:
            ALOGE("%s: Unhandled event (%d)!", __FUNCTION__, event);
            break;
    }
}

static void bte_gattc_cback(tBTA_GATTC_EVT event, tBTA_GATTC *p_data)
{
    bt_status_t status = btif_transfer_context(btif_gattc_upstreams_evt,
                    (uint16_t) event, (void*)p_data, sizeof(tBTA_GATTC), NULL);
    ASSERTC(status == BT_STATUS_SUCCESS, "Context transfer failed!", status);
}

static void bte_scan_results_cb (tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data)
{
    btif_gattc_cb_t btif_cb;
    uint8_t len;

    switch (event)
    {
        case BTA_DM_INQ_RES_EVT:
        {
            bdcpy(btif_cb.bd_addr.address, p_data->inq_res.bd_addr);
            btif_cb.device_type = p_data->inq_res.device_type;
            btif_cb.rssi = p_data->inq_res.rssi;
            btif_cb.addr_type = p_data->inq_res.ble_addr_type;
            if (p_data->inq_res.p_eir)
            {
                memcpy(btif_cb.value, p_data->inq_res.p_eir, 62);
                if (BTA_CheckEirData(p_data->inq_res.p_eir, BTM_EIR_COMPLETE_LOCAL_NAME_TYPE,
                                      &len))
                {
                    p_data->inq_res.remt_name_not_required  = TRUE;
                }
            }
        }
        break;

        case BTA_DM_INQ_CMPL_EVT:
        {
            BTIF_TRACE_DEBUG2("%s  BLE observe complete. Num Resp %d",
                              __FUNCTION__,p_data->inq_cmpl.num_resps);
            return;
        }

        default:
        BTIF_TRACE_WARNING2("%s : Unknown event 0x%x", __FUNCTION__, event);
        return;
    }
    btif_transfer_context(btif_gattc_upstreams_evt, BTIF_GATT_OBSERVE_EVT,
                                 (char*) &btif_cb, sizeof(btif_gattc_cb_t), NULL);
}

static void btm_read_rssi_cb (tBTM_RSSI_RESULTS *p_result)
{
    btif_gattc_cb_t btif_cb;

    bdcpy(btif_cb.bd_addr.address, p_result->rem_bda);
    btif_cb.rssi = p_result->rssi;
    btif_cb.status = p_result->status;
    btif_cb.client_if = rssi_request_client_if;
    btif_transfer_context(btif_gattc_upstreams_evt, BTIF_GATTC_RSSI_EVT,
                                 (char*) &btif_cb, sizeof(btif_gattc_cb_t), NULL);
}


static void btgattc_handle_event(uint16_t event, char* p_param)
{
    tBTA_GATT_STATUS           status;
    tBT_UUID                   uuid;
    tBTA_GATT_SRVC_ID          srvc_id;
    tGATT_CHAR_PROP            out_char_prop;
    tBTA_GATTC_CHAR_ID         in_char_id;
    tBTA_GATTC_CHAR_ID         out_char_id;
    tBTA_GATTC_CHAR_DESCR_ID   in_char_descr_id;
    tBTA_GATTC_CHAR_DESCR_ID   out_char_descr_id;
    tBTA_GATTC_INCL_SVC_ID     in_incl_svc_id;
    tBTA_GATTC_INCL_SVC_ID     out_incl_svc_id;
    tBTA_GATT_UNFMT            descr_val;

    btif_gattc_cb_t* p_cb = (btif_gattc_cb_t*)p_param;
    if (!p_cb) return;

    ALOGD("%s: Event %d", __FUNCTION__, event);

    switch (event)
    {
        case BTIF_GATTC_REGISTER_APP:
            btif_to_bta_uuid(&uuid, &p_cb->uuid);
            BTA_GATTC_AppRegister(&uuid, bte_gattc_cback);
            break;

        case BTIF_GATTC_UNREGISTER_APP:
            BTA_GATTC_AppDeregister(p_cb->client_if);
            break;

        case BTIF_GATTC_SCAN_START:
            btif_gattc_init_dev_cb();
            BTA_DmBleObserve(TRUE, 0, bte_scan_results_cb);
            break;

        case BTIF_GATTC_SCAN_STOP:
            BTA_DmBleObserve(FALSE, 0, 0);
            break;

        case BTIF_GATTC_OPEN:
            if (!p_cb->is_direct)
                BTA_DmBleSetBgConnType(BTM_BLE_CONN_AUTO, NULL);

            BTA_GATTC_Open(p_cb->client_if, p_cb->bd_addr.address, p_cb->is_direct);
            break;

        case BTIF_GATTC_CLOSE:
            // Disconnect establiched connections
            if (p_cb->conn_id != 0)
                BTA_GATTC_Close(p_cb->conn_id);
            else
                BTA_GATTC_CancelOpen(p_cb->client_if, p_cb->bd_addr.address, TRUE);

            // Cancel pending background connections (remove from whitelist)
            BTA_GATTC_CancelOpen(p_cb->client_if, p_cb->bd_addr.address, FALSE);
            break;

        case BTIF_GATTC_SEARCH_SERVICE:
        {
            if (p_cb->search_all)
            {
                BTA_GATTC_ServiceSearchRequest(p_cb->conn_id, NULL);
            } else {
                btif_to_bta_uuid(&uuid, &p_cb->uuid);
                BTA_GATTC_ServiceSearchRequest(p_cb->conn_id, &uuid);
            }
            break;
        }

        case BTIF_GATTC_GET_FIRST_CHAR:
        {
            btgatt_gatt_id_t char_id;
            btif_to_bta_srvc_id(&srvc_id, &p_cb->srvc_id);
            status = BTA_GATTC_GetFirstChar(p_cb->conn_id, &srvc_id, NULL,
                                            &out_char_id, &out_char_prop);

            if (status == 0)
                bta_to_btif_char_id(&char_id, &out_char_id.char_id);

            HAL_CBACK(bt_gatt_callbacks, client->get_characteristic_cb,
                p_cb->conn_id, status, &p_cb->srvc_id,
                &char_id, out_char_prop);
            break;
        }

        case BTIF_GATTC_GET_NEXT_CHAR:
        {
            btgatt_gatt_id_t char_id;
            btif_to_bta_srvc_id(&in_char_id.srvc_id, &p_cb->srvc_id);
            btif_to_bta_char_id(&in_char_id.char_id, &p_cb->char_id);

            status = BTA_GATTC_GetNextChar(p_cb->conn_id, &in_char_id, NULL,
                                            &out_char_id, &out_char_prop);

            if (status == 0)
                bta_to_btif_char_id(&char_id, &out_char_id.char_id);

            HAL_CBACK(bt_gatt_callbacks, client->get_characteristic_cb,
                p_cb->conn_id, status, &p_cb->srvc_id,
                &char_id, out_char_prop);
            break;
        }

        case BTIF_GATTC_GET_FIRST_CHAR_DESCR:
        {
            bt_uuid_t descr_id;
            btif_to_bta_srvc_id(&in_char_id.srvc_id, &p_cb->srvc_id);
            btif_to_bta_char_id(&in_char_id.char_id, &p_cb->char_id);

            status = BTA_GATTC_GetFirstCharDescr(p_cb->conn_id, &in_char_id, NULL,
                                                    &out_char_descr_id);

            if (status == 0)
                bta_to_btif_uuid(&descr_id, &out_char_descr_id.descr_type);

            HAL_CBACK(bt_gatt_callbacks, client->get_descriptor_cb,
                p_cb->conn_id, status, &p_cb->srvc_id,
                &p_cb->char_id, &descr_id);
            break;
        }

        case BTIF_GATTC_GET_NEXT_CHAR_DESCR:
        {
            bt_uuid_t descr_id;
            btif_to_bta_srvc_id(&in_char_descr_id.char_id.srvc_id, &p_cb->srvc_id);
            btif_to_bta_char_id(&in_char_descr_id.char_id.char_id, &p_cb->char_id);
            btif_to_bta_uuid(&in_char_descr_id.descr_type, &p_cb->uuid);

            status = BTA_GATTC_GetNextCharDescr(p_cb->conn_id, &in_char_descr_id
                                        , NULL, &out_char_descr_id);

            if (status == 0)
                bta_to_btif_uuid(&descr_id, &out_char_descr_id.descr_type);

            HAL_CBACK(bt_gatt_callbacks, client->get_descriptor_cb,
                p_cb->conn_id, status, &p_cb->srvc_id,
                &p_cb->char_id, &descr_id);
            break;
        }

        case BTIF_GATTC_GET_FIRST_INCL_SERVICE:
        {
            btgatt_srvc_id_t incl_srvc_id;
            btif_to_bta_srvc_id(&srvc_id, &p_cb->srvc_id);

            status = BTA_GATTC_GetFirstIncludedService(p_cb->conn_id,
                        &srvc_id, NULL, &out_incl_svc_id);

            bta_to_btif_srvc_id(&incl_srvc_id, &out_incl_svc_id.incl_svc_id);

            HAL_CBACK(bt_gatt_callbacks, client->get_included_service_cb,
                p_cb->conn_id, status, &p_cb->srvc_id,
                &incl_srvc_id);
            break;
        }

        case BTIF_GATTC_GET_NEXT_INCL_SERVICE:
        {
            btgatt_srvc_id_t incl_srvc_id;
            btif_to_bta_srvc_id(&in_incl_svc_id.srvc_id, &p_cb->srvc_id);
            btif_to_bta_srvc_id(&in_incl_svc_id.incl_svc_id, &p_cb->incl_srvc_id);

            status = BTA_GATTC_GetNextIncludedService(p_cb->conn_id,
                        &in_incl_svc_id, NULL, &out_incl_svc_id);

            bta_to_btif_srvc_id(&incl_srvc_id, &out_incl_svc_id.incl_svc_id);

            HAL_CBACK(bt_gatt_callbacks, client->get_included_service_cb,
                p_cb->conn_id, status, &p_cb->srvc_id,
                &incl_srvc_id);
            break;
        }

        case BTIF_GATTC_READ_CHAR:
            btif_to_bta_srvc_id(&in_char_id.srvc_id, &p_cb->srvc_id);
            btif_to_bta_char_id(&in_char_id.char_id, &p_cb->char_id);

            BTA_GATTC_ReadCharacteristic(p_cb->conn_id, &in_char_id, p_cb->auth_req);
            break;

        case BTIF_GATTC_READ_CHAR_DESCR:
            btif_to_bta_srvc_id(&in_char_descr_id.char_id.srvc_id, &p_cb->srvc_id);
            btif_to_bta_char_id(&in_char_descr_id.char_id.char_id, &p_cb->char_id);
            btif_to_bta_uuid(&in_char_descr_id.descr_type, &p_cb->uuid);

            BTA_GATTC_ReadCharDescr(p_cb->conn_id, &in_char_descr_id, p_cb->auth_req);
            break;

        case BTIF_GATTC_WRITE_CHAR:
            btif_to_bta_srvc_id(&in_char_id.srvc_id, &p_cb->srvc_id);
            btif_to_bta_char_id(&in_char_id.char_id, &p_cb->char_id);

            BTA_GATTC_WriteCharValue(p_cb->conn_id, &in_char_id,
                                     p_cb->write_type,
                                     p_cb->len,
                                     p_cb->value,
                                     p_cb->auth_req);
            break;

        case BTIF_GATTC_WRITE_CHAR_DESCR:
            btif_to_bta_srvc_id(&in_char_descr_id.char_id.srvc_id, &p_cb->srvc_id);
            btif_to_bta_char_id(&in_char_descr_id.char_id.char_id, &p_cb->char_id);
            btif_to_bta_uuid(&in_char_descr_id.descr_type, &p_cb->uuid);

            descr_val.len = p_cb->len;
            descr_val.p_value = p_cb->value;

            BTA_GATTC_WriteCharDescr(p_cb->conn_id, &in_char_descr_id,
                                     p_cb->write_type, &descr_val,
                                     p_cb->auth_req);
            break;

        case BTIF_GATTC_EXECUTE_WRITE:
            BTA_GATTC_ExecuteWrite(p_cb->conn_id, p_cb->action);
            break;

        case BTIF_GATTC_REG_FOR_NOTIFICATION:
            btif_to_bta_srvc_id(&in_char_id.srvc_id, &p_cb->srvc_id);
            btif_to_bta_char_id(&in_char_id.char_id, &p_cb->char_id);

            status = BTA_GATTC_RegisterForNotifications(p_cb->client_if,
                                    p_cb->bd_addr.address, &in_char_id);

            HAL_CBACK(bt_gatt_callbacks, client->register_for_notification_cb,
                p_cb->conn_id, 1, status, &p_cb->srvc_id,
                &p_cb->char_id);
            break;

        case BTIF_GATTC_DEREG_FOR_NOTIFICATION:
            btif_to_bta_srvc_id(&in_char_id.srvc_id, &p_cb->srvc_id);
            btif_to_bta_char_id(&in_char_id.char_id, &p_cb->char_id);

            status = BTA_GATTC_DeregisterForNotifications(p_cb->client_if,
                                        p_cb->bd_addr.address, &in_char_id);

            HAL_CBACK(bt_gatt_callbacks, client->register_for_notification_cb,
                p_cb->conn_id, 0, status, &p_cb->srvc_id,
                &p_cb->char_id);
            break;

        case BTIF_GATTC_REFRESH:
            BTA_GATTC_Refresh(p_cb->bd_addr.address);
            break;

        case BTIF_GATTC_READ_RSSI:
            rssi_request_client_if = p_cb->client_if;
            BTM_ReadRSSI (p_cb->bd_addr.address, (tBTM_CMPL_CB *)btm_read_rssi_cb);
            break;

        default:
            ALOGE("%s: Unknown event (%d)!", __FUNCTION__, event);
            break;
    }
}
#endif

/********************************************
*   client_if is 1-based index
*********************************************/
static void *convertIfToContext(int client_if){
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT){
        return g_gattc_cb.regContext[client_if-1];
    }
    return 0;
}

/********************************************
*   return : 0 means not found
*********************************************/
static int convertContextToIf(void *regContext){
    int i;
    for(i = 0;i < GATT_MAX_CLIENT;i++){
        if(regContext == g_gattc_cb.regContext[i])
            return i+1;
    }
    return 0;
}

static void freeContext(int client_if){
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT){
        client_if--;
        g_gattc_cb.used[client_if] = 0;
        memset(g_gattc_cb.appuuid[client_if], 0, sizeof(g_gattc_cb.appuuid[0]));
        g_gattc_cb.regContext[client_if] = 0;
    }
}

static int allocFreeContext(uint8_t *appuuid){
    int i;
    GATT_IF_API();
    gattDumpUUID("client appuuid", 16, appuuid);
    for(i = 0;i < GATT_MAX_CLIENT;i++){
        if(!g_gattc_cb.used[i]){
            g_gattc_cb.used[i] = 1;
            memcpy(g_gattc_cb.appuuid[i], appuuid, 16);
            GATT_IF_DBG("alloc client slot %d", i);
            return i+1;
        }
    }
    GATT_IF_ERR("No more free context slot");
    return 0;
}

static int findContextByAppUUID(uint8_t *appuuid){
    int i;
    GATT_IF_API();
    for(i = 0;i < GATT_MAX_CLIENT;i++){
        if(g_gattc_cb.used[i] && !memcmp(appuuid, g_gattc_cb.appuuid[i] , 16)){
            GATT_IF_DBG("context[%d] = %p found", i, g_gattc_cb.regContext[i]);
            return i+1;
        }
    }
    GATT_IF_WARN("No context found");
    return 0;
}

static int findContextByContext(void *ctx){
    int i;
    GATT_IF_API();
    for(i = 0;i < GATT_MAX_CLIENT;i++){
        if(g_gattc_cb.used[i] && ctx == g_gattc_cb.regContext[i]){
            GATT_IF_DBG("context[%d] = %p found", i, g_gattc_cb.regContext[i]);
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
** Function         gattc_handle_register_cnf
**
** Description      Handle gattc register confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_register_cnf(ilm_struct *pIlm){
    bt_gattc_register_cnf_struct *pCnf = (bt_gattc_register_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByAppUUID(pCnf->user_id);
    GATT_IF_API();

    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        g_gattc_cb.regContext[client_if-1] = pCnf->reg_ctx;
        bt_gatt_callbacks->client->register_client_cb(status, client_if, (bt_uuid_t*)pCnf->user_id);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_deregister_cnf
**
** Description      Handle gattc deregister confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_deregister_cnf(ilm_struct *pIlm){
    bt_gattc_deregister_cnf_struct *pCnf = (bt_gattc_deregister_cnf_struct*)pIlm->ilm_data;
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();

    if(client_if > 0 && client_if <= GATT_MAX_CLIENT){
        freeContext(client_if);
    }else{
        GATT_IF_ERR("Can not find context slot. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_scan_result_ind
**
** Description      Handle gattc scan result message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_scan_result_ind(ilm_struct *pIlm){
    bt_gattc_scan_result_ind_struct *pInd = (bt_gattc_scan_result_ind_struct*)pIlm->ilm_data;
    bt_bdaddr_t bda;
    int client_if = findContextByContext(pInd->reg_ctx);
    GATT_IF_API();
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btmtk_util_reverse_array(bda.address, pInd->bd_addr);
        bt_gatt_callbacks->client->scan_result_cb(&bda, pInd->rssi, pInd->eir);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_scan_cnf
**
** Description      Handle gattc scan confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_scan_cnf(ilm_struct *pIlm){
    bt_gattc_scan_cnf_struct *pCnf = (bt_gattc_scan_cnf_struct*)pIlm->ilm_data;
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT){
    }else{
        GATT_IF_ERR("Can not find context slot. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_scan_cancel_cnf
**
** Description      Handle gattc scan cancel confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_scan_cancel_cnf(ilm_struct *pIlm){
    bt_gattc_scan_cancel_cnf_struct *pCnf = (bt_gattc_scan_cancel_cnf_struct*)pIlm->ilm_data;
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT){
    }else{
        GATT_IF_ERR("Can not find context slot. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_connect_cnf
**
** Description      Handle gattc connect confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_connect_cnf(ilm_struct *pIlm){
    bt_gattc_connect_cnf_struct *pCnf = (bt_gattc_connect_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    bt_bdaddr_t bda;
    
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btmtk_util_reverse_array(bda.address, pCnf->bd_addr);
        if(status == GATT_SUCCESS){
            bt_gatt_callbacks->client->open_cb(GET_CONNID(client_if, pCnf->conn_ctx), status, client_if, &bda);
        }else if(status != GATT_PENDING){
            // Connect failed
            bt_gatt_callbacks->client->close_cb(0, status, client_if, &bda);
        }
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_connected_ind
**
** Description      Handle gattc connected indication message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_connected_ind(ilm_struct *pIlm){
    bt_gattc_connected_ind_struct *pInd = (bt_gattc_connected_ind_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pInd->result);
    int client_if = findContextByContext(pInd->reg_ctx);
    bt_bdaddr_t bda;
    
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btmtk_util_reverse_array(bda.address, pInd->bd_addr);
        if(status == GATT_SUCCESS){
            bt_gatt_callbacks->client->open_cb(GET_CONNID(client_if, pInd->conn_ctx), status, client_if, &bda);
        }else{
            // Connect failed
            bt_gatt_callbacks->client->close_cb(0, status, client_if, &bda);
        }
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_disconnect_cnf
**
** Description      Handle gattc disconnect confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_disconnect_cnf(ilm_struct *pIlm){
    bt_gattc_disconnect_cnf_struct *pCnf = (bt_gattc_disconnect_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    bt_bdaddr_t bda;
    
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        if(status != GATT_PENDING){
            // Disconnect success
            btmtk_util_reverse_array(bda.address, pCnf->bd_addr);
            bt_gatt_callbacks->client->close_cb(GET_CONNID(client_if, pCnf->conn_ctx), status, client_if, &bda);
        }
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_disconnected_ind
**
** Description      Handle gattc disconnected indication message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_disconnected_ind(ilm_struct *pIlm){
    bt_gattc_disconnected_ind_struct *pInd = (bt_gattc_disconnected_ind_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pInd->result);
    int client_if = findContextByContext(pInd->reg_ctx);
    bt_bdaddr_t bda;
    
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btmtk_util_reverse_array(bda.address, pInd->bd_addr);
        bt_gatt_callbacks->client->close_cb(GET_CONNID(client_if, pInd->conn_ctx), status, client_if, &bda);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_refresh_cnf
**
** Description      Handle gattc refresh confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_refresh_cnf(ilm_struct *pIlm){
    bt_gattc_refresh_cnf_struct *pCnf = (bt_gattc_refresh_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT){
    }else{
        GATT_IF_ERR("Can not find context slot. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_search_service_result_ind
**
** Description      Handle gattc search service result indication message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_search_service_result_ind(ilm_struct *pIlm){
    bt_gattc_search_service_result_ind_struct *pInd = (bt_gattc_search_service_result_ind_struct*)pIlm->ilm_data;
    int client_if = findContextByContext(pInd->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("client_if=%d", client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btgatt_srvc_id_t srvc_id;
        convertSvcUUIDToSvcId(&srvc_id, &pInd->uuid);
        bt_gatt_callbacks->client->search_result_cb(GET_CONNID(client_if, pInd->conn_ctx), &srvc_id);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_search_service_cnf
**
** Description      Handle gattc search service confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_search_service_cnf(ilm_struct *pIlm){
    bt_gattc_search_service_cnf_struct *pCnf = (bt_gattc_search_service_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        bt_gatt_callbacks->client->search_complete_cb(GET_CONNID(client_if, pCnf->conn_ctx), status);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}


/*******************************************************************************
**
** Function         gattc_handle_search_incl_service_cnf
**
** Description      Handle gattc search included service confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_search_incl_service_cnf(ilm_struct *pIlm){
    bt_gattc_get_incl_service_cnf_struct *pCnf = (bt_gattc_get_incl_service_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btgatt_srvc_id_t srvc_id, incl_srvc_id;
        convertSvcUUIDToSvcId(&srvc_id, &pCnf->svc_uuid);
        convertSvcUUIDToSvcId(&incl_srvc_id, &pCnf->incl_svc_uuid);
        bt_gatt_callbacks->client->get_included_service_cb(GET_CONNID(client_if, pCnf->conn_ctx), 
                                                                                   status,
                                                                                   &srvc_id,
                                                                                   &incl_srvc_id);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_get_char_cnf
**
** Description      Handle gattc get characteristic confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_get_char_cnf(ilm_struct *pIlm){
    bt_gattc_get_char_cnf_struct *pCnf = (bt_gattc_get_char_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btgatt_srvc_id_t srvc_id;
        btgatt_gatt_id_t char_id;
        convertSvcUUIDToSvcId(&srvc_id, &pCnf->svc_uuid);
        convertAttUUIDToCharId(&char_id, &pCnf->char_uuid);
        bt_gatt_callbacks->client->get_characteristic_cb(GET_CONNID(client_if, pCnf->conn_ctx),
                                                                               status,
                                                                               &srvc_id,
                                                                               &char_id,
                                                                               (int)pCnf->properties);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_get_descr_cnf
**
** Description      Handle gattc get descriptor confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_get_descr_cnf(ilm_struct *pIlm){
    bt_gattc_get_descr_cnf_struct *pCnf = (bt_gattc_get_descr_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btgatt_srvc_id_t srvc_id;
        btgatt_gatt_id_t char_id;
        btgatt_gatt_id_t descr_id;
        convertSvcUUIDToSvcId(&srvc_id, &pCnf->svc_uuid);
        convertAttUUIDToCharId(&char_id, &pCnf->char_uuid);
        convertAttUUIDToCharId(&descr_id, &pCnf->descr_uuid);
        bt_gatt_callbacks->client->get_descriptor_cb(GET_CONNID(client_if, pCnf->conn_ctx),
                                                                          status,
                                                                          &srvc_id,
                                                                          &char_id,
                                                                          &descr_id);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_read_char_cnf
**
** Description      Handle gattc read characteristic confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_read_char_cnf(ilm_struct *pIlm){
    bt_gattc_read_char_cnf_struct *pCnf = (bt_gattc_read_char_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btgatt_read_params_t data;
        convertSvcUUIDToSvcId(&data.srvc_id, &pCnf->svc_uuid);
        convertAttUUIDToCharId(&data.char_id, &pCnf->char_uuid);
        data.value_type = 0;
        data.value.len = pCnf->value.len;
        if(data.value.len){
            memcpy(data.value.value, pCnf->value.value, data.value.len);
        }
        data.status = status;
        bt_gatt_callbacks->client->read_characteristic_cb(GET_CONNID(client_if, pCnf->conn_ctx),
                                                                                status,
                                                                                &data);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_write_char_cnf
**
** Description      Handle gattc write characteristic confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_write_char_cnf(ilm_struct *pIlm){
    bt_gattc_write_char_cnf_struct *pCnf = (bt_gattc_write_char_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btgatt_write_params_t data;
        convertSvcUUIDToSvcId(&data.srvc_id, &pCnf->svc_uuid);
        convertAttUUIDToCharId(&data.char_id, &pCnf->char_uuid);
        data.status = status;
        bt_gatt_callbacks->client->write_characteristic_cb(GET_CONNID(client_if, pCnf->conn_ctx),
                                                                                status,
                                                                                &data);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_read_descr_cnf
**
** Description      Handle gattc read descriptor confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_read_descr_cnf(ilm_struct *pIlm){
    bt_gattc_read_descr_cnf_struct *pCnf = (bt_gattc_read_descr_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btgatt_read_params_t data;
        convertSvcUUIDToSvcId(&data.srvc_id, &pCnf->svc_uuid);
        convertAttUUIDToCharId(&data.char_id, &pCnf->char_uuid);
        convertAttUUIDToCharId(&data.descr_id, &pCnf->descr_uuid);
        data.value_type = 0;
        data.value.len = pCnf->value.len;
        if(data.value.len){
            memcpy(data.value.value, pCnf->value.value, data.value.len);
        }
        data.status = status;
        bt_gatt_callbacks->client->read_descriptor_cb(GET_CONNID(client_if, pCnf->conn_ctx),
                                                                                status,
                                                                                &data);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_write_descr_cnf
**
** Description      Handle gattc write descriptor confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_write_descr_cnf(ilm_struct *pIlm){
    bt_gattc_write_descr_cnf_struct *pCnf = (bt_gattc_write_descr_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btgatt_write_params_t data;
        convertSvcUUIDToSvcId(&data.srvc_id, &pCnf->svc_uuid);
        convertAttUUIDToCharId(&data.char_id, &pCnf->char_uuid);
        convertAttUUIDToCharId(&data.descr_id, &pCnf->descr_uuid);
        data.status = status;
        bt_gatt_callbacks->client->write_descriptor_cb(GET_CONNID(client_if, pCnf->conn_ctx),
                                                                                status,
                                                                                &data);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_exec_write_cnf
**
** Description      Handle gattc execute write confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_exec_write_cnf(ilm_struct *pIlm){
    bt_gattc_exec_write_cnf_struct *pCnf = (bt_gattc_exec_write_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        bt_gatt_callbacks->client->execute_write_cb(GET_CONNID(client_if, pCnf->conn_ctx), status);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_reg_notification_cnf
**
** Description      Handle gattc register for notification confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_reg_notification_cnf(ilm_struct *pIlm){
    bt_gattc_register_notification_cnf_struct *pCnf = (bt_gattc_register_notification_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btgatt_srvc_id_t srvc_id;
        btgatt_gatt_id_t char_id;
        convertSvcUUIDToSvcId(&srvc_id, &pCnf->svc_uuid);
        convertAttUUIDToCharId(&char_id, &pCnf->char_uuid);

        bt_gatt_callbacks->client->register_for_notification_cb(0/*GET_CONNID(client_if, pCnf->conn_ctx)*/,
                                                                                        1,
                                                                                        status,
                                                                                        &srvc_id,
                                                                                        &char_id);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_dereg_notification_cnf
**
** Description      Handle gattc deregister for notification confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_dereg_notification_cnf(ilm_struct *pIlm){
    bt_gattc_deregister_notification_cnf_struct *pCnf = (bt_gattc_deregister_notification_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btgatt_srvc_id_t srvc_id;
        btgatt_gatt_id_t char_id;
        convertSvcUUIDToSvcId(&srvc_id, &pCnf->svc_uuid);
        convertAttUUIDToCharId(&char_id, &pCnf->char_uuid);

        bt_gatt_callbacks->client->register_for_notification_cb(0 /*GET_CONNID(client_if, pCnf->conn_ctx)*/,
                                                                                        0,
                                                                                        status,
                                                                                        &srvc_id,
                                                                                        &char_id);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_read_rssi_cnf
**
** Description      Handle gattc read rssi confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_read_rssi_cnf(ilm_struct *pIlm){
    bt_gattc_read_rssi_cnf_struct *pCnf = (bt_gattc_read_rssi_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    bt_bdaddr_t bda;

    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btmtk_util_reverse_array(bda.address, pCnf->bd_addr);
        bt_gatt_callbacks->client->read_remote_rssi_cb(client_if, 
                                                                              &bda,
                                                                              pCnf->rssi,
                                                                              status);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_get_device_type_cnf
**
** Description      Handle gattc get device type confirm message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_get_device_type_cnf(ilm_struct *pIlm){
    bt_gattc_get_device_type_cnf_struct *pCnf = (bt_gattc_get_device_type_cnf_struct*)pIlm->ilm_data;
    int status = convertToGattErrCode(pCnf->result);
    int client_if = findContextByContext(pCnf->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("status=%d, client_if=%d", status, client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){

    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}

/*******************************************************************************
**
** Function         gattc_handle_notification_ind
**
** Description      Handle gattc noification indication message
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_notification_ind(ilm_struct *pIlm){
    bt_gattc_notification_ind_struct *pInd = (bt_gattc_notification_ind_struct*)pIlm->ilm_data;
    int client_if = findContextByContext(pInd->reg_ctx);
    GATT_IF_API();
    GATT_IF_DBG("client_if=%d", client_if);
    if(client_if > 0 && client_if <= GATT_MAX_CLIENT && bt_gatt_callbacks){
        btgatt_notify_params_t data;
        btmtk_util_reverse_array(data.bda.address, pInd->bd_addr);
        //memcpy(data.bda.address, pInd->bd_addr, 6);
        convertSvcUUIDToSvcId(&data.srvc_id, &pInd->svc_uuid);
        convertAttUUIDToCharId(&data.char_id, &pInd->char_uuid);
        data.len = pInd->value.len;
        if(data.len){
            memcpy(data.value, pInd->value.value, data.len);
        }
        data.is_notify = pInd->is_notify;
        bt_gatt_callbacks->client->notify_cb(GET_CONNID(client_if, pInd->conn_ctx),
                                                                                    &data);
    }else{
        GATT_IF_ERR("Can not find context slot or callback is NULL. Just Return");
    }
}


/*******************************************************************************
**
** Function         gattc_handle_message
**
** Description      Handle gattc message from internal adp
**
** Returns          void
**
*******************************************************************************/
void gattc_handle_message(void *pContext, ilm_struct *pIlm){
    U16 msg_size;
    GATT_IF_API();
    GATT_IF_DBG("gattc_handle_message : msg_id=%u", (unsigned int)pIlm->msg_id);

    msg_size = *((U16*)&pIlm->ilm_data[2]);
    
    switch(pIlm->msg_id){
    case MSG_ID_BT_GATTC_REGISTER_REQ:
    case MSG_ID_BT_GATTC_DEREGISTER_REQ:
    case MSG_ID_BT_GATTC_SCAN_REQ:
    case MSG_ID_BT_GATTC_SCAN_CANCEL_REQ:
    case MSG_ID_BT_GATTC_CONNECT_REQ:
    case MSG_ID_BT_GATTC_DISCONNECT_REQ:
    case MSG_ID_BT_GATTC_REFRESH_REQ:
    case MSG_ID_BT_GATTC_SEARCH_SERVICE_REQ:
    case MSG_ID_BT_GATTC_GET_INCL_SERVICE_REQ:
    case MSG_ID_BT_GATTC_GET_CHAR_REQ:
    case MSG_ID_BT_GATTC_GET_DESCR_REQ:
    case MSG_ID_BT_GATTC_READ_CHAR_REQ:
    case MSG_ID_BT_GATTC_WRITE_CHAR_REQ:
    case MSG_ID_BT_GATTC_READ_DESCR_REQ:
    case MSG_ID_BT_GATTC_WRITE_DESCR_REQ:
    case MSG_ID_BT_GATTC_EXEC_WRITE_REQ:
    case MSG_ID_BT_GATTC_REGISTER_NOTIFICATION_REQ:
    case MSG_ID_BT_GATTC_DEREGISTER_NOTIFICATION_REQ:
    case MSG_ID_BT_GATTC_READ_RSSI_REQ:
    case MSG_ID_BT_GATTC_GET_DEVICE_TYPE_REQ:
        btmtk_sendmsg(PROFILE_GATTC, pIlm, msg_size);
        break;
    case MSG_ID_BT_GATTC_REGISTER_CNF:
        gattc_handle_register_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_DEREGISTER_CNF:
        gattc_handle_deregister_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_SCAN_RESULT_IND:
        gattc_handle_scan_result_ind(pIlm);
        break;
    case MSG_ID_BT_GATTC_SCAN_CNF:
        gattc_handle_scan_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_SCAN_CANCEL_CNF:
        gattc_handle_scan_cancel_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_CONNECT_CNF:
        gattc_handle_connect_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_CONNECTED_IND:
        gattc_handle_connected_ind(pIlm);
        break;
    case MSG_ID_BT_GATTC_DISCONNECT_CNF:
        gattc_handle_disconnect_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_DISCONNECTED_IND:
        gattc_handle_disconnected_ind(pIlm);
        break;
    case MSG_ID_BT_GATTC_REFRESH_CNF:
        gattc_handle_refresh_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_SEARCH_SERVICE_RESULT_IND:
        gattc_handle_search_service_result_ind(pIlm);
        break;
    case MSG_ID_BT_GATTC_SEARCH_SERVICE_CNF:
        gattc_handle_search_service_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_GET_INCL_SERVICE_CNF:
        gattc_handle_search_incl_service_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_GET_CHAR_CNF:
        gattc_handle_get_char_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_GET_DESCR_CNF:
        gattc_handle_get_descr_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_READ_CHAR_CNF:
        gattc_handle_read_char_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_WRITE_CHAR_CNF:
        gattc_handle_write_char_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_READ_DESCR_CNF:
        gattc_handle_read_descr_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_WRITE_DESCR_CNF:
        gattc_handle_write_descr_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_EXEC_WRITE_CNF:
        gattc_handle_exec_write_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_REGISTER_NOTIFICATION_CNF:
        gattc_handle_reg_notification_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_DEREGISTER_NOTIFICATION_CNF:
        gattc_handle_dereg_notification_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_READ_RSSI_CNF:
        gattc_handle_read_rssi_cnf(pIlm);
        break;
    case MSG_ID_BT_GATTC_GET_DEVICE_TYPE_CNF:
        // TODO: get device type is sync call
        break;
    case MSG_ID_BT_GATTC_NOTIFICATION_IND:
        gattc_handle_notification_ind(pIlm);
        break;
    default:
        GATT_IF_WARN("Unknown message : %u", (unsigned int)pIlm->msg_id);
        break;
    }
}

/*******************************************************************************
**
** Function         gattc_handle_disconnect_callback
**
** Description      diconnect the currect active gatt link
**
** Returns          void
**
*******************************************************************************/
static void gattc_handle_disconnect_callback(void){
    GATT_IF_API();
}

static U32 gattc_wait_response(msg_type resp_id, ilm_struct *ilm, U16 *size)
{
    int res = 0;
    fd_set readfs;
    struct timeval Timeout;
    int sockfd = g_gattc_cb.gattc_fd;

    Timeout.tv_usec = (3000%1000)*1000;
    Timeout.tv_sec  = 3000/1000;
    FD_ZERO(&readfs);
    if(sockfd){
        FD_SET(sockfd, &readfs);
    }else{
        GATT_IF_ERR("nat->servsock == 0. exit");
    }
    res = TEMP_FAILURE_RETRY(select(sockfd+1, &readfs, NULL, NULL, &Timeout));
    if(res > 0){
        res = recvfrom(sockfd, ilm, sizeof(ilm_struct), 0, NULL, NULL);
        if(res < 0){
            GATT_IF_ERR("recvfrom failed : %s, %d", strerror(errno), errno);
        }else{
            if( ilm->msg_id != resp_id){
                res = -1;
                GATT_IF_ERR("recv resp=%u is mismatch, expected : %u", (unsigned int)ilm->msg_id, (unsigned int)resp_id);
            }else{
                *size = res;            
                GATT_IF_DBG("recv resp=%u, bytes=%d", (unsigned int)ilm->msg_id, res);
            }
        }
    }
    else if(res == 0){
        GATT_IF_ERR("timeout waiting response, in %d milliseconds", 3000);
    }else{
        GATT_IF_ERR("wait select failed : %s, %d", strerror(errno), errno);
    }
    return (res>0) ? 0 : 1;
}

U32 gattc_send_message(msg_type msg_id, void *ptr, U16 size)
{
    int ret;
    int sockfd;
    ilm_struct *ilm;
    U16 *field;
	
    GATT_IF_DBG("msg=%u, ptr=0x%X, size=%d", msg_id, (unsigned int)ptr, size);
    sockfd = g_gattc_cb.gattc_fd;

    ret = -1;
    ilm = (ilm_struct*)ptr;
    /* Set src_mod to MOD_MMI */
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = MOD_BT;
    ilm->sap_id = 0;
    ilm->msg_id = msg_id;

    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm->ilm_data;
    *field = 1;
    *(++field) = size;

    // cal whole ilm size
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
    
    if(sockfd){
        ret = send(sockfd, ptr, size, 0);
        if(ret < 0){
            GATT_IF_ERR("send msg fail : %s, %d", strerror(errno), errno);
        }else{
            GATT_IF_DBG("send msg success : %d", ret);
        }
    }else{
        GATT_IF_ERR("socket uninitialized");
    }
    return (ret >= 0) ? 0 : 1;
}

static U32 gattc_common_req(msg_type msg_id, msg_type resp_id, ilm_struct *ilm, U16 *size){
    U32 status;

    GATT_IF_DBG("gattc_common_req(msg_id=%u, resp_id=%u, ilm=0x%X, size=%d)", msg_id, resp_id, (unsigned int)ilm, *size);
    /* Fill msg parameters */
    status = gattc_send_message(msg_id, ilm, *size);
    if(status == BT_STATUS_SUCCESS && resp_id != 0)
    {
        /* Wait for response */
        status = gattc_wait_response(resp_id, ilm, size);
    }
    return status;
}

static int convertDeviceType(U8 devType){
    int type = devType;
    /*
    BT_DEV_TYPE_UNKNOWN(0)         ==> 0
    BT_DEV_TYPE_LE(1)                    ==> BT_DEVICE_TYPE_BLE (2)
    BT_DEV_TYPE_BR_EDR(2)            ==> BT_DEVICE_TYPE_BREDR (1)
    BT_DEV_TYPE_BR_EDR_LE(3)       ==> BT_DEVICE_TYPE_DUMO (3)
    */
    if(devType == 1){
        type = 2;
    }else if(devType == 2){
        type = 1;
    }else if(devType != 0){
        type = 0;
        GATT_IF_WARN("Invalid device type %u", devType);
    }
    GATT_IF_DBG("convertDeviceType(%u) == %d", devType, type);
    return type;
}

static int initGattcSock(void){
    g_gattc_cb.gattc_fd = -1;
    struct sockaddr_un name;
    socklen_t   namelen;
    struct sockaddr_un btname;
    socklen_t   btnamelen; 

    // Setup address of mtkbt
    btname.sun_family = AF_UNIX;
    strcpy (btname.sun_path, BT_SOCK_NAME_INT_ADP);
    btnamelen = (offsetof (struct sockaddr_un, sun_path) + strlen (btname.sun_path) + 1);

    // Init client socket
    namelen = sizeof(short);
    g_gattc_cb.gattc_fd = socket(PF_LOCAL, SOCK_DGRAM, 0);
    if (g_gattc_cb.gattc_fd < 0)
    {
        GATT_IF_ERR("create socket failed : %s, errno=%d", strerror(errno), errno);
        return BT_STATUS_FAIL;
    }
    if (bind (g_gattc_cb.gattc_fd, (struct sockaddr *) &name, namelen) < 0)
    {
        GATT_IF_ERR("bind socket failed : %s, errno=%d", strerror(errno), errno);
        goto exit;
    }

    if ( connect(g_gattc_cb.gattc_fd, (const struct sockaddr*)&btname, btnamelen) < 0)
    {
        GATT_IF_ERR("connect to /data/btserv failed : %s, errno=%d", strerror(errno), errno);
        goto exit;
    }
    return g_gattc_cb.gattc_fd;
exit:
    if(g_gattc_cb.gattc_fd >= 0){
        close(g_gattc_cb.gattc_fd);
        g_gattc_cb.gattc_fd = -1;
    }
    return -1;
}

bt_status_t btif_gattc_init( const btgatt_client_callbacks_t *callbacks ){
    memset(&g_gattc_cb, 0, sizeof(g_gattc_cb));
    btmtk_profile_register(PROFILE_GATTC, gattc_handle_message, (void*)PROFILE_GATTC);
    initGattcSock();
    return BT_STATUS_SUCCESS;
}

void  btif_gattc_cleanup( void ){
    btmtk_profile_unregister(PROFILE_GATTC);
    if(g_gattc_cb.gattc_fd >= 0){
        close(g_gattc_cb.gattc_fd);
        g_gattc_cb.gattc_fd = -1;
    }
}

/*******************************************************************************
**  Client API Functions
********************************************************************************/
static bt_status_t btif_gattc_register_app(bt_uuid_t *uuid)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    int free_idx = allocFreeContext(uuid->uu);
    bt_gattc_register_req_struct *req = (bt_gattc_register_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTC_REGISTER_REQ;
    if(uuid){
        if(free_idx > 0 && free_idx <= GATT_MAX_CLIENT){
            memcpy(req->user_id, uuid->uu, 16);
            if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_register_req_struct))){
                GATT_IF_ERR("send MSG_ID_BT_GATTC_REGISTER_REQ failed");
                status = BT_STATUS_FAIL;
                freeContext(free_idx); // free allocated context slot
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

static bt_status_t btif_gattc_unregister_app(int client_if )
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gattc_deregister_req_struct *req = (bt_gattc_deregister_req_struct*)ilm.ilm_data;

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTC_DEREGISTER_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_deregister_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_DEREGISTER_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_scan( int client_if, bool start )
{
    bt_status_t status = BT_STATUS_SUCCESS;
    int req_size;
    ilm_struct ilm;
    void *regContext = convertIfToContext(client_if);

    GATT_IF_API();
    CHECK_BTGATT_INIT();
     
    if(regContext){
        if(start){
            bt_gattc_scan_req_struct *req = (bt_gattc_scan_req_struct*)ilm.ilm_data;
            ilm.msg_id = MSG_ID_BT_GATTC_SCAN_REQ;
            req->reg_ctx =regContext;
            req_size = sizeof(bt_gattc_scan_req_struct);
        }else{
            bt_gattc_scan_cancel_req_struct *req = (bt_gattc_scan_cancel_req_struct*)ilm.ilm_data;
            ilm.msg_id = MSG_ID_BT_GATTC_SCAN_CANCEL_REQ;
            req->reg_ctx = regContext;
            req_size = sizeof(bt_gattc_scan_cancel_req_struct);
        }
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, req_size)){
            if(start){
                GATT_IF_ERR("send MSG_ID_BT_GATTC_SCAN_REQ failed");
            }else{
                GATT_IF_ERR("send MSG_ID_BT_GATTC_SCAN_CANCEL_REQ failed");
            }
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;    
}

static bt_status_t btif_gattc_open(int client_if, const bt_bdaddr_t *bd_addr, bool is_direct )
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gattc_connect_req_struct *req = (bt_gattc_connect_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTC_CONNECT_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        btmtk_util_reverse_array(req->bd_addr, bd_addr->address);
        //memcpy(req->bd_addr, bd_addr->address, sizeof(req->bd_addr));
        req->direct = is_direct ? 1 : 0;
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_connect_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_CONNECT_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_close( int client_if, const bt_bdaddr_t *bd_addr, int conn_id)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gattc_disconnect_req_struct *req = (bt_gattc_disconnect_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTC_DISCONNECT_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        req->conn_ctx = (void*)GET_CONN_FROM_CONNID(conn_id);
        btmtk_util_reverse_array(req->bd_addr, bd_addr->address);
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_disconnect_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_DISCONNECT_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_listen(int client_if, bool start)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    return status;
}

static bt_status_t btif_gattc_refresh( int client_if, const bt_bdaddr_t *bd_addr )
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gattc_refresh_req_struct *req = (bt_gattc_refresh_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTC_REFRESH_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        btmtk_util_reverse_array(req->bd_addr, bd_addr->address);
        //memcpy(req->bd_addr, bd_addr, sizeof(req->bd_addr));
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_refresh_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_REFRESH_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_search_service(int conn_id, bt_uuid_t *filter_uuid )
{
    bt_status_t status = BT_STATUS_SUCCESS;
    int client_if = GET_IF_FROM_CONNID(conn_id);
    ilm_struct ilm;
    bt_gattc_search_service_req_struct *req = (bt_gattc_search_service_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTC_SEARCH_SERVICE_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        req->conn_ctx = (void*)GET_CONN_FROM_CONNID(conn_id);
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_search_service_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_SEARCH_SERVICE_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_get_characteristic( int conn_id
        , btgatt_srvc_id_t *srvc_id, btgatt_gatt_id_t *start_char_id)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    int client_if = GET_IF_FROM_CONNID(conn_id);
    ilm_struct ilm;
    bt_gattc_get_char_req_struct *req = (bt_gattc_get_char_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTC_GET_CHAR_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        req->conn_ctx = (void*)GET_CONN_FROM_CONNID(conn_id);
        convertSvcIdToSvcUUID(&req->svc_uuid, srvc_id);
        convertCharIdToAttUUID(&req->char_uuid, start_char_id);
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_get_char_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_GET_CHAR_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_get_descriptor( int conn_id
        , btgatt_srvc_id_t *srvc_id, btgatt_gatt_id_t *char_id
        , btgatt_gatt_id_t *start_descr_id)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    int client_if = GET_IF_FROM_CONNID(conn_id);
    ilm_struct ilm;
    bt_gattc_get_descr_req_struct *req = (bt_gattc_get_descr_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();
    ilm.msg_id = MSG_ID_BT_GATTC_GET_DESCR_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        req->conn_ctx = (void*)GET_CONN_FROM_CONNID(conn_id);
        convertSvcIdToSvcUUID(&req->svc_uuid, srvc_id);
        convertCharIdToAttUUID(&req->char_uuid, char_id);
        req->descr_uuid.inst = 0; // How to set inst_id?
        convertCharIdToAttUUID(&req->descr_uuid, start_descr_id);
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_get_descr_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_GET_DESCR_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_get_included_service(int conn_id, btgatt_srvc_id_t *srvc_id,
                                                   btgatt_srvc_id_t *start_incl_srvc_id)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    int client_if = GET_IF_FROM_CONNID(conn_id);
    ilm_struct ilm;
    bt_gattc_get_incl_service_req_struct *req = (bt_gattc_get_incl_service_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();
    ilm.msg_id = MSG_ID_BT_GATTC_GET_INCL_SERVICE_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        req->conn_ctx = (void*)GET_CONN_FROM_CONNID(conn_id);
        convertSvcIdToSvcUUID(&req->svc_uuid, srvc_id);
        convertSvcIdToSvcUUID(&req->incl_svc_uuid, start_incl_srvc_id);
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_get_incl_service_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_GET_INCL_SERVICE_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_read_char(int conn_id, btgatt_srvc_id_t* srvc_id,
                                        btgatt_gatt_id_t* char_id, int auth_req )
{
    bt_status_t status = BT_STATUS_SUCCESS;
    int client_if = GET_IF_FROM_CONNID(conn_id);
    ilm_struct ilm;
    bt_gattc_read_char_req_struct *req = (bt_gattc_read_char_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();
    ilm.msg_id = MSG_ID_BT_GATTC_READ_CHAR_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        req->conn_ctx = (void*)GET_CONN_FROM_CONNID(conn_id);
        convertSvcIdToSvcUUID(&req->svc_uuid, srvc_id);
        convertCharIdToAttUUID(&req->char_uuid, char_id);
        req->auth_req = (U32)auth_req;
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_read_char_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_READ_CHAR_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_read_char_descr(int conn_id, btgatt_srvc_id_t* srvc_id,
                                              btgatt_gatt_id_t* char_id, btgatt_gatt_id_t* descr_id,
                                              int auth_req )
{
    bt_status_t status = BT_STATUS_SUCCESS;
    int client_if = GET_IF_FROM_CONNID(conn_id);
    ilm_struct ilm;
    bt_gattc_read_descr_req_struct *req = (bt_gattc_read_descr_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();
    ilm.msg_id = MSG_ID_BT_GATTC_READ_DESCR_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        req->conn_ctx = (void*)GET_CONN_FROM_CONNID(conn_id);
        convertSvcIdToSvcUUID(&req->svc_uuid, srvc_id);
        convertCharIdToAttUUID(&req->char_uuid, char_id);
        req->descr_uuid.inst = 0; // How to set descr inst
        convertCharIdToAttUUID(&req->descr_uuid, descr_id);
        req->auth_req = (U32)auth_req;
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_read_descr_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_READ_DESCR_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_write_char(int conn_id, btgatt_srvc_id_t* srvc_id,
                                         btgatt_gatt_id_t* char_id, int write_type,
                                         int len, int auth_req, char* p_value)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    int client_if = GET_IF_FROM_CONNID(conn_id);
    ilm_struct ilm;
    bt_gattc_write_char_req_struct *req = (bt_gattc_write_char_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();
    ilm.msg_id = MSG_ID_BT_GATTC_WRITE_CHAR_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        if(len && p_value){
            req->conn_ctx = (void*)GET_CONN_FROM_CONNID(conn_id);
            convertSvcIdToSvcUUID(&req->svc_uuid, srvc_id);
            convertCharIdToAttUUID(&req->char_uuid, char_id);
            req->write_type = (U32)write_type;
            req->auth_req = (U32)auth_req;
            req->value.len = len;
            memcpy(req->value.value, p_value, len);
            req->value.len = len;
            if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_write_char_req_struct))){
                GATT_IF_ERR("send MSG_ID_BT_GATTC_WRITE_CHAR_REQ failed");
                status = BT_STATUS_FAIL;
            }
        }else{
            GATT_IF_ERR("Invalid parameter : len=%d, p_value=%p", len, p_value);
            status = BT_STATUS_PARM_INVALID;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_write_char_descr(int conn_id, btgatt_srvc_id_t* srvc_id,
                                               btgatt_gatt_id_t* char_id, btgatt_gatt_id_t* descr_id,
                                               int write_type, int len, int auth_req,
                                               char* p_value)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    int client_if = GET_IF_FROM_CONNID(conn_id);
    ilm_struct ilm;
    bt_gattc_write_descr_req_struct *req = (bt_gattc_write_descr_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();
    ilm.msg_id = MSG_ID_BT_GATTC_WRITE_DESCR_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        if(len && p_value){
            req->conn_ctx = (void*)GET_CONN_FROM_CONNID(conn_id);
            convertSvcIdToSvcUUID(&req->svc_uuid, srvc_id);
            convertCharIdToAttUUID(&req->char_uuid, char_id);
            req->descr_uuid.inst = 0;
            convertCharIdToAttUUID(&req->descr_uuid, descr_id);
            req->write_type = (U32)write_type;
            req->auth_req = (U32)auth_req;
            req->value.len = len;
            memcpy(req->value.value, p_value, len);
            req->value.len = len;
            if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_write_descr_req_struct))){
                GATT_IF_ERR("send MSG_ID_BT_GATTC_WRITE_DESCR_REQ failed");
                status = BT_STATUS_FAIL;
            }
        }else{
            GATT_IF_ERR("Invalid parameter : len=%d, p_value=%p", len, p_value);
            status = BT_STATUS_PARM_INVALID;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_execute_write(int conn_id, int execute)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    int client_if = GET_IF_FROM_CONNID(conn_id);
    ilm_struct ilm;
    bt_gattc_exec_write_req_struct *req = (bt_gattc_exec_write_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTC_EXEC_WRITE_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        req->conn_ctx = (void*)GET_CONN_FROM_CONNID(conn_id);
        req->exeute = (U8)execute;
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_exec_write_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_EXEC_WRITE_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_reg_for_notification(int client_if, const bt_bdaddr_t *bd_addr,
                                                   btgatt_srvc_id_t* srvc_id,
                                                   btgatt_gatt_id_t* char_id)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gattc_register_notification_req_struct *req = (bt_gattc_register_notification_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTC_REGISTER_NOTIFICATION_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        convertSvcIdToSvcUUID(&req->svc_uuid, srvc_id);
        convertCharIdToAttUUID(&req->char_uuid, char_id);
        btmtk_util_reverse_array(req->bd_addr, bd_addr->address);
        //memcpy(req->bd_addr, bd_addr->address, 6);
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_register_notification_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_REGISTER_NOTIFICATION_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_dereg_for_notification(int client_if, const bt_bdaddr_t *bd_addr,
                                                     btgatt_srvc_id_t* srvc_id,
                                                     btgatt_gatt_id_t* char_id)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gattc_deregister_notification_req_struct *req = (bt_gattc_deregister_notification_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTC_DEREGISTER_NOTIFICATION_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        convertSvcIdToSvcUUID(&req->svc_uuid, srvc_id);
        convertCharIdToAttUUID(&req->char_uuid, char_id);
        btmtk_util_reverse_array(req->bd_addr, bd_addr->address);
        //memcpy(req->bd_addr, bd_addr->address, 6);
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_deregister_notification_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_DEREGISTER_NOTIFICATION_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static bt_status_t btif_gattc_read_remote_rssi(int client_if, const bt_bdaddr_t *bd_addr)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gattc_read_rssi_req_struct *req = (bt_gattc_read_rssi_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTC_READ_RSSI_REQ;
    req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        btmtk_util_reverse_array(req->bd_addr, bd_addr->address);
        //memcpy(req->bd_addr, bd_addr->address, 6);
        if(!btmtk_sendevt(PROFILE_GATTC, (void*)&ilm, sizeof(bt_gattc_read_rssi_req_struct))){
            GATT_IF_ERR("send MSG_ID_BT_GATTC_READ_RSSI_REQ failed");
            status = BT_STATUS_FAIL;
        }
    }else{
        GATT_IF_ERR("Invalid client_if %d", client_if);
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
}

static int btif_gattc_get_device_type( const bt_bdaddr_t *bd_addr )
{
#if 1
    ilm_struct ilm;
    U16 size = sizeof(bt_gattc_get_device_type_req_struct);
    bt_gattc_get_device_type_req_struct *req = (bt_gattc_get_device_type_req_struct*)ilm.ilm_data;
    bt_gattc_get_device_type_cnf_struct *cnf = (bt_gattc_get_device_type_cnf_struct*)ilm.ilm_data;

    btmtk_util_reverse_array(req->bd_addr, bd_addr->address);
    if( gattc_common_req(MSG_ID_BT_GATTC_GET_DEVICE_TYPE_REQ,
                                   MSG_ID_BT_GATTC_GET_DEVICE_TYPE_CNF,
                                   &ilm,
                                   &size) == BT_STATUS_SUCCESS){
        if(cnf->result == 0){
            return convertDeviceType(cnf->dev_type);
        }
    }
    return 0;
#else
    bt_status_t status = BT_STATUS_SUCCESS;
    ilm_struct ilm;
    bt_gattc_get_device_type_req_struct *req = (bt_gattc_get_device_type_req_struct*)ilm.ilm_data;    

    GATT_IF_API();
    CHECK_BTGATT_INIT();

    ilm.msg_id = MSG_ID_BT_GATTC_GET_DEVICE_TYPE_REQ;
    //req->reg_ctx = convertIfToContext(client_if);
    if(req->reg_ctx){
        memcpy(req->bd_addr, bd_addr->address, 6);
        //btmtk_sendmsg(PROFILE_GATTC, );
    }else{
        status = BT_STATUS_PARM_INVALID;
    }
    return status;
#endif
}

static bt_status_t btif_gattc_set_adv_data(int server_if, bool set_scan_rsp, bool include_name,
                    bool include_txpower, int min_interval, int max_interval, int appearance,
                    uint16_t manufacturer_len, char* manufacturer_data)
{
    bt_status_t status = BT_STATUS_SUCCESS;

    return status;
}

static bt_status_t btif_gattc_test_command(int command, btgatt_test_params_t* params)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    return status;
    //return btif_gattc_test_command_impl(command, params);
}

#ifdef __DROID_STACK__
extern bt_status_t btif_gattc_test_command_impl(int command, btgatt_test_params_t* params);

static bt_status_t btif_gattc_test_command(int command, btgatt_test_params_t* params)
{
    return btif_gattc_test_command_impl(command, params);
}
#endif

const btgatt_client_interface_t btgattClientInterface = {
    btif_gattc_register_app,
    btif_gattc_unregister_app,
    btif_gattc_scan,
    btif_gattc_open,
    btif_gattc_close,
    btif_gattc_listen,
    btif_gattc_refresh,
    btif_gattc_search_service,
    btif_gattc_get_included_service,
    btif_gattc_get_characteristic,
    btif_gattc_get_descriptor,
    btif_gattc_read_char,
    btif_gattc_write_char,
    btif_gattc_read_char_descr,
    btif_gattc_write_char_descr,
    btif_gattc_execute_write,
    btif_gattc_reg_for_notification,
    btif_gattc_dereg_for_notification,
    btif_gattc_read_remote_rssi,
    btif_gattc_get_device_type,
    btif_gattc_set_adv_data,
    btif_gattc_test_command
};

#endif
