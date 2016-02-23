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

#include "bttypes.h"
#include "bt_adp_gatt.h"
#include "gatt_cl.h"
#include "bt_message.h"
#include "bluetooth_gattc_struct.h"
#include "osapi.h"

#if defined(__BT_GATTC_PROFILE__)

//gattc_adp_context_struct g_gattc_context;
/*
static void convertATTtoGATTUuid(ATT_UUID *gatt_uuid, const ATT_UUID *att_uuid){
    gatt_uuid->inst = att_uuid->inst;
    gatt_uuid->len = att_uuid->uuid.len;
    gatt_uuid->uuid = att_uuid->uuid.uuid;
}

static void convertSvcUuid(GATT_SVC_UUID *svc_uuid, const GATT_SERV_ID *serv_id){
    convertATTtoGATTUuid(&svc_uuid->uuid, serv_id->uuid);
    svc_uuid->is_primary = serv_id->is_primary;
}
*/
/***********************************************************
* Callback function
************************************************************/
void gattc_adp_scan_result_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcScanResult *ind){
    bt_gattc_scan_result_ind_struct *msg;
    msg = (bt_gattc_scan_result_ind_struct*) construct_local_para(sizeof(bt_gattc_scan_result_ind_struct), TD_UL);
    msg->reg_ctx = handle;
    OS_MemCopy(msg->bd_addr, ind->addr.addr, BD_ADDR_SIZE);
    msg->rssi = ind->rssi;
    msg->eir_len = ind->eir_len;
    OS_MemCopy(msg->eir, ind->eir, msg->eir_len);
    BT_SendMessage(MSG_ID_BT_GATTC_SCAN_RESULT_IND, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_scan_complete_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcScanCompleteInd *ind){
    bt_gattc_scan_cnf_struct *msg;
    msg = (bt_gattc_scan_cnf_struct*) construct_local_para(sizeof(bt_gattc_scan_cnf_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->result = result;
    BT_SendMessage(MSG_ID_BT_GATTC_SCAN_CNF, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_connected_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcConnectedInd *ind){
    bt_gattc_connected_ind_struct *msg;
    msg = (bt_gattc_connected_ind_struct*) construct_local_para(sizeof(bt_gattc_connected_ind_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->result = result;
    msg->conn_ctx = ind->connHdl;
    OS_MemCopy(msg->bd_addr, ind->bdaddr.addr, BD_ADDR_SIZE);
    BT_SendMessage(MSG_ID_BT_GATTC_CONNECTED_IND, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_disconnected_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcDisconnectedInd *ind){
    bt_gattc_disconnected_ind_struct *msg;
    msg = (bt_gattc_disconnected_ind_struct*) construct_local_para(sizeof(bt_gattc_disconnected_ind_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->result = result;
    msg->conn_ctx = ind->connHdl;
    OS_MemCopy(msg->bd_addr, ind->bdaddr.addr, BD_ADDR_SIZE);
    BT_SendMessage(MSG_ID_BT_GATTC_DISCONNECTED_IND, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_search_service_result_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcSearchServiceResult *ind){
    bt_gattc_search_service_result_ind_struct *msg;
    msg = (bt_gattc_search_service_result_ind_struct*) construct_local_para(sizeof(bt_gattc_search_service_result_ind_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->conn_ctx = ind->connHdl;
    msg->uuid = ind->svc_uuid;
    BT_SendMessage(MSG_ID_BT_GATTC_SEARCH_SERVICE_RESULT_IND, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_search_service_complete_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcSearchServiceCompleteInd *ind){
    bt_gattc_search_service_cnf_struct *msg;
    msg = (bt_gattc_search_service_cnf_struct*) construct_local_para(sizeof(bt_gattc_search_service_cnf_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->result = result;
    msg->conn_ctx = ind->connHdl;
    BT_SendMessage(MSG_ID_BT_GATTC_SEARCH_SERVICE_CNF, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_search_incl_service_result_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcSearchInclServiceResult *ind){
    bt_gattc_get_incl_service_cnf_struct *msg;
    msg = (bt_gattc_get_incl_service_cnf_struct*) construct_local_para(sizeof(bt_gattc_get_incl_service_cnf_struct), TD_UL);
    msg->reg_ctx = handle;
     msg->result = result;
    msg->conn_ctx = ind->connHdl;
    msg->svc_uuid = ind->svc_uuid;
    msg->incl_svc_uuid = ind->incl_svc_uuid;    
    BT_SendMessage(MSG_ID_BT_GATTC_GET_INCL_SERVICE_CNF, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_get_char_result_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcGetCharResult *ind){
    bt_gattc_get_char_cnf_struct *msg;
    msg = (bt_gattc_get_char_cnf_struct*) construct_local_para(sizeof(bt_gattc_get_char_cnf_struct), TD_UL);
    msg->reg_ctx = handle;
     msg->result = result;
    msg->conn_ctx = ind->connHdl;
    msg->svc_uuid = ind->svc_uuid;
    msg->char_uuid = ind->char_uuid;
    msg->properties = ind->properties;
    BT_SendMessage(MSG_ID_BT_GATTC_GET_CHAR_CNF, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_get_descr_result_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcGetDescrResult *ind){
    bt_gattc_get_descr_cnf_struct *msg;
    msg = (bt_gattc_get_descr_cnf_struct*) construct_local_para(sizeof(bt_gattc_get_descr_cnf_struct), TD_UL);
    msg->reg_ctx = handle;
     msg->result = result;
    msg->conn_ctx = ind->connHdl;
    msg->svc_uuid = ind->svc_uuid;
    msg->char_uuid = ind->char_uuid;
    msg->descr_uuid = ind->descr_uuid;
    BT_SendMessage(MSG_ID_BT_GATTC_GET_DESCR_CNF, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_read_char_result_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcReadCharResult *ind){
    bt_gattc_read_char_cnf_struct *msg;
    msg = (bt_gattc_read_char_cnf_struct*) construct_local_para(sizeof(bt_gattc_read_char_cnf_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->result = result;
    msg->conn_ctx = ind->connHdl;
    msg->svc_uuid = ind->svc_uuid;
    msg->char_uuid = ind->char_uuid;
    msg->value.len = ind->len;
    OS_MemCopy(msg->value.value, ind->value, ind->len);
    BT_SendMessage(MSG_ID_BT_GATTC_READ_CHAR_CNF, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_write_char_result_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcWriteCharResult *ind){
    bt_gattc_write_char_cnf_struct *msg;
    msg = (bt_gattc_write_char_cnf_struct*) construct_local_para(sizeof(bt_gattc_write_char_cnf_struct), TD_UL);
    msg->reg_ctx = handle;
     msg->result = result;
    msg->conn_ctx = ind->connHdl;
    msg->svc_uuid = ind->svc_uuid;
    msg->char_uuid = ind->char_uuid;
    BT_SendMessage(MSG_ID_BT_GATTC_WRITE_CHAR_CNF, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_read_descr_result_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcReadDescrResult *ind){
    bt_gattc_read_descr_cnf_struct *msg;
    msg = (bt_gattc_read_descr_cnf_struct*) construct_local_para(sizeof(bt_gattc_read_descr_cnf_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->result = result;
    msg->conn_ctx = ind->connHdl;
    msg->svc_uuid = ind->svc_uuid;
    msg->char_uuid = ind->char_uuid;
    msg->descr_uuid = ind->descr_uuid;
    msg->value.len = ind->len;
    OS_MemCopy(msg->value.value, ind->value, ind->len);
    BT_SendMessage(MSG_ID_BT_GATTC_READ_DESCR_CNF, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_write_descr_result_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcWriteDescrResult *ind){
    bt_gattc_write_descr_cnf_struct *msg;
    msg = (bt_gattc_write_descr_cnf_struct*) construct_local_para(sizeof(bt_gattc_write_descr_cnf_struct), TD_UL);
    msg->reg_ctx = handle;
     msg->result = result;
    msg->conn_ctx = ind->connHdl;
    msg->svc_uuid = ind->svc_uuid;
    msg->char_uuid = ind->char_uuid;
    msg->descr_uuid = ind->descr_uuid;
    BT_SendMessage(MSG_ID_BT_GATTC_WRITE_DESCR_CNF, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_exec_write_result_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcExecWriteResult *ind){
    bt_gattc_exec_write_cnf_struct *msg;
    msg = (bt_gattc_exec_write_cnf_struct*) construct_local_para(sizeof(bt_gattc_exec_write_cnf_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->result = result;
    msg->conn_ctx = ind->connHdl;
    BT_SendMessage(MSG_ID_BT_GATTC_EXEC_WRITE_CNF, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_read_rssi_result_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcReadRssiResult *ind){
    bt_gattc_read_rssi_cnf_struct *msg;
    msg = (bt_gattc_read_rssi_cnf_struct*) construct_local_para(sizeof(bt_gattc_read_rssi_cnf_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->result = result;
    OS_MemCopy(msg->bd_addr, ind->bdaddr.addr, BD_ADDR_SIZE);
    msg->rssi = ind->rssi;
    BT_SendMessage(MSG_ID_BT_GATTC_READ_RSSI_CNF, MOD_MMI, msg, msg->msg_len);
}

void gattc_adp_notification_ind_handler(GATTC_REG_HANDLE handle, U8 result, GattcNotificationInd *ind){
    bt_gattc_notification_ind_struct *msg;
    msg = (bt_gattc_notification_ind_struct*) construct_local_para(sizeof(bt_gattc_notification_ind_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->conn_ctx = ind->connHdl;
    OS_MemCopy(msg->bd_addr, ind->bdaddr.addr, BD_ADDR_SIZE);
    msg->svc_uuid = ind->svc_uuid;
    msg->char_uuid = ind->char_uuid;
    msg->value.len = ind->len;
    OS_MemCopy(msg->value.value, ind->value, ind->len);
    msg->is_notify = ind->is_notify;
    BT_SendMessage(MSG_ID_BT_GATTC_NOTIFICATION_IND, MOD_MMI, msg, msg->msg_len);
}

static void gattcAdpCallback(GATTC_REG_HANDLE handle, GattcCallbackParms *Parms){
    switch(Parms->event){
    case GATTC_EVENT_SCAN_RESULT:
        gattc_adp_scan_result_ind_handler(handle, Parms->result, &Parms->param.scan_result);
        break;
    case GATTC_EVENT_SCAN_COMPLETE_IND:
        gattc_adp_scan_complete_ind_handler(handle, Parms->result, &Parms->param.scan_complete_ind);
        break;
    case GATTC_EVENT_CONNECTED_IND:
        gattc_adp_connected_ind_handler(handle, Parms->result, &Parms->param.connected_ind);
        break;
    case GATTC_EVENT_DISCONNECTED_IND:
        gattc_adp_disconnected_ind_handler(handle, Parms->result, &Parms->param.disconnected_ind);
        break;
    case GATTC_EVENT_SEARCH_SERVICE_RESULT:
        gattc_adp_search_service_result_ind_handler(handle, Parms->result, &Parms->param.search_service_result);
        break;
    case GATTC_EVENT_SEARCH_SERVICE_COMPLETE_IND:
        gattc_adp_search_service_complete_ind_handler(handle, Parms->result, &Parms->param.search_service_complete_ind);
        break;
    case GATTC_EVENT_INCL_SERVICE_RESULT:
        gattc_adp_search_incl_service_result_ind_handler(handle, Parms->result, &Parms->param.search_incl_service_result);
        break;
    case GATTC_EVENT_GET_CHAR_RESULT:
        gattc_adp_get_char_result_ind_handler(handle, Parms->result, &Parms->param.get_char_result);
        break;
    case GATTC_EVENT_GET_DESCR_RESULT:
        gattc_adp_get_descr_result_ind_handler(handle, Parms->result, &Parms->param.get_descr_result);
        break;
    case GATTC_EVENT_READ_CHAR_RESULT:
        gattc_adp_read_char_result_ind_handler(handle, Parms->result, &Parms->param.read_char_result);
        break;
    case GATTC_EVENT_WRITE_CHAR_RESULT:
        gattc_adp_write_char_result_ind_handler(handle, Parms->result, &Parms->param.write_char_result);
        break;
    case GATTC_EVENT_READ_DESCR_RESULT:
        gattc_adp_read_descr_result_ind_handler(handle, Parms->result, &Parms->param.read_descr_result);
        break;
    case GATTC_EVENT_WRITE_DESCR_RESULT:
        gattc_adp_write_descr_result_ind_handler(handle, Parms->result, &Parms->param.write_descr_result);
        break;
    case GATTC_EVENT_EXEC_WRITE_RESULT:
        gattc_adp_exec_write_result_ind_handler(handle, Parms->result, &Parms->param.exec_write_result);
        break;
    case GATTC_EVENT_READ_RSSI_RESULT:
        gattc_adp_read_rssi_result_ind_handler(handle, Parms->result, &Parms->param.read_rssi_result);
        break;
    case GATTC_EVENT_NOTIFICATION_IND:
        gattc_adp_notification_ind_handler(handle, Parms->result, &Parms->param.notification_ind);
        break;
    default:
        GATTS_ERR("Invalid callback event %u", Parms->event);
        break;
    }
}

void gattc_adp_register_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    GATTC_REG_HANDLE handle;
    bt_gattc_register_req_struct *req;
    bt_gattc_register_cnf_struct *cnf = NULL;
    
    req = (bt_gattc_register_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTC_Register(&handle , gattcAdpCallback);
    cnf = (bt_gattc_register_cnf_struct*) construct_local_para(sizeof(bt_gattc_register_cnf_struct), TD_UL);
    OS_MemCopy(cnf->user_id, req->user_id, sizeof(cnf->user_id));;
    cnf->reg_ctx = handle;
    cnf->result = (U8)status;
    BT_SendMessage(MSG_ID_BT_GATTC_REGISTER_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gattc_adp_deregister_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_deregister_req_struct *req;
    bt_gattc_deregister_cnf_struct *cnf = NULL;
    
    req = (bt_gattc_deregister_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTC_Deregister(req->reg_ctx);
    cnf = (bt_gattc_deregister_cnf_struct*) construct_local_para(sizeof(bt_gattc_deregister_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    BT_SendMessage(MSG_ID_BT_GATTC_DEREGISTER_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gattc_adp_scan_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_scan_req_struct *req;
    
    req = (bt_gattc_scan_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTC_Scan(req->reg_ctx, 1);
}

void gattc_adp_scan_cancel_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_scan_cancel_req_struct *req;
    bt_gattc_scan_cancel_cnf_struct *cnf = NULL;
    
    req = (bt_gattc_scan_cancel_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTC_Scan(req->reg_ctx, 0);
    cnf = (bt_gattc_scan_cancel_cnf_struct*) construct_local_para(sizeof(bt_gattc_scan_cancel_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    BT_SendMessage(MSG_ID_BT_GATTC_SCAN_CANCEL_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gattc_adp_connect_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_connect_req_struct *req;
    bt_gattc_connect_cnf_struct *cnf = NULL;
    GATTC_CONN_HANDLE connHdl = INVALID_GATTC_CONN_HANDLE;
    
    req = (bt_gattc_connect_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTC_Open(req->reg_ctx, req->bd_addr, req->direct, &connHdl);
    cnf = (bt_gattc_connect_cnf_struct*) construct_local_para(sizeof(bt_gattc_connect_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    OS_MemCopy(cnf->bd_addr, req->bd_addr, BD_ADDR_SIZE);
    cnf->conn_ctx = connHdl;
    BT_SendMessage(MSG_ID_BT_GATTC_CONNECT_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gattc_adp_disconnect_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_disconnect_req_struct *req;
    bt_gattc_disconnect_cnf_struct *cnf = NULL;
    
    req = (bt_gattc_disconnect_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTC_Close(req->reg_ctx, req->bd_addr, req->conn_ctx);
    cnf = (bt_gattc_disconnect_cnf_struct*) construct_local_para(sizeof(bt_gattc_disconnect_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    OS_MemCopy(cnf->bd_addr, req->bd_addr, BD_ADDR_SIZE);
    cnf->conn_ctx = req->conn_ctx;
    BT_SendMessage(MSG_ID_BT_GATTC_DISCONNECT_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gattc_adp_refresh_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_refresh_req_struct *req = (bt_gattc_refresh_req_struct*)ilm_ptr->ilm_data;
    bt_gattc_refresh_cnf_struct *cnf = NULL;
    
    req = (bt_gattc_refresh_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTC_Refresh(req->reg_ctx, req->bd_addr);
    cnf = (bt_gattc_refresh_cnf_struct*) construct_local_para(sizeof(bt_gattc_refresh_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    BT_SendMessage(MSG_ID_BT_GATTC_REFRESH_CNF, MOD_MMI, cnf, cnf->msg_len);    
}

void gattc_adp_search_service_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_search_service_req_struct *req;
    bt_gattc_search_service_cnf_struct *cnf = NULL;

    req = (bt_gattc_search_service_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTC_SearchService(req->reg_ctx, req->conn_ctx, &req->uuid);
    if(status != BT_STATUS_PENDING){
        cnf = (bt_gattc_search_service_cnf_struct*) construct_local_para(sizeof(bt_gattc_search_service_cnf_struct), TD_UL);
        cnf->reg_ctx = req->reg_ctx;
        cnf->result = (U8)status;
        cnf->conn_ctx = req->conn_ctx;
        BT_SendMessage(MSG_ID_BT_GATTC_SEARCH_SERVICE_CNF, MOD_MMI, cnf, cnf->msg_len);  
    }
}

void gattc_adp_get_incl_service_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_get_incl_service_req_struct *req;
    bt_gattc_get_incl_service_cnf_struct *cnf = NULL;

    req = (bt_gattc_get_incl_service_req_struct*)ilm_ptr->local_para_ptr;    
    status = GATTC_GetInclService(req->reg_ctx, req->conn_ctx, &req->svc_uuid, &req->incl_svc_uuid);
    if(status != BT_STATUS_PENDING){
        cnf = (bt_gattc_get_incl_service_cnf_struct*) construct_local_para(sizeof(bt_gattc_get_incl_service_cnf_struct), TD_UL);
        cnf->reg_ctx = req->reg_ctx;
        cnf->result = (U8)status;
        cnf->conn_ctx = req->conn_ctx;
        cnf->svc_uuid = req->svc_uuid;
        cnf->incl_svc_uuid.uuid.uuid.len = 0;
        BT_SendMessage(MSG_ID_BT_GATTC_GET_INCL_SERVICE_CNF, MOD_MMI, cnf, cnf->msg_len);  
    }
}

void gattc_adp_get_char_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_get_char_req_struct *req;
    bt_gattc_get_char_cnf_struct *cnf = NULL;

    req = (bt_gattc_get_char_req_struct*)ilm_ptr->local_para_ptr;    
    status = GATTC_GetChar(req->reg_ctx, req->conn_ctx, &req->svc_uuid, &req->char_uuid);
    if(status != BT_STATUS_PENDING){
        cnf = (bt_gattc_get_char_cnf_struct*) construct_local_para(sizeof(bt_gattc_get_char_cnf_struct), TD_UL);
        cnf->reg_ctx = req->reg_ctx;
        cnf->result = (U8)status;
        cnf->conn_ctx = req->conn_ctx;
        cnf->svc_uuid = req->svc_uuid;
        cnf->char_uuid.uuid.len = 0;
        cnf->properties = 0;
        BT_SendMessage(MSG_ID_BT_GATTC_GET_CHAR_CNF, MOD_MMI, cnf, cnf->msg_len);  
    }
}

void gattc_adp_get_descr_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_get_descr_req_struct *req;
    bt_gattc_get_descr_cnf_struct *cnf = NULL;

    req = (bt_gattc_get_descr_req_struct*)ilm_ptr->local_para_ptr;    
    status = GATTC_GetDescr(req->reg_ctx, req->conn_ctx, &req->svc_uuid, &req->char_uuid, &req->descr_uuid);
    if(status != BT_STATUS_PENDING){
        cnf = (bt_gattc_get_descr_cnf_struct*) construct_local_para(sizeof(bt_gattc_get_descr_cnf_struct), TD_UL);
        cnf->reg_ctx = req->reg_ctx;
        cnf->result = (U8)status;
        cnf->conn_ctx = req->conn_ctx;
        cnf->svc_uuid = req->svc_uuid;
        cnf->char_uuid = req->char_uuid;
        cnf->descr_uuid.uuid.len = 0;
        BT_SendMessage(MSG_ID_BT_GATTC_GET_DESCR_CNF, MOD_MMI, cnf, cnf->msg_len);  
    }
}

void gattc_adp_read_char_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_read_char_req_struct *req;
    bt_gattc_read_char_cnf_struct *cnf = NULL;

    req = (bt_gattc_read_char_req_struct*)ilm_ptr->local_para_ptr;    
    status = GATTC_ReadChar(req->reg_ctx, req->conn_ctx, &req->svc_uuid, &req->char_uuid, req->auth_req);
    if(status != BT_STATUS_PENDING){
        cnf = (bt_gattc_read_char_cnf_struct*) construct_local_para(sizeof(bt_gattc_read_char_cnf_struct), TD_UL);
        cnf->reg_ctx = req->reg_ctx;
        cnf->result = (U8)status;
        cnf->conn_ctx = req->conn_ctx;
        cnf->svc_uuid = req->svc_uuid;
        cnf->char_uuid = req->char_uuid;
        cnf->value.len = 0;
        BT_SendMessage(MSG_ID_BT_GATTC_READ_CHAR_CNF, MOD_MMI, cnf, cnf->msg_len);
    }
}

void gattc_adp_write_char_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_write_char_req_struct *req;
    bt_gattc_write_char_cnf_struct *cnf = NULL;

    req = (bt_gattc_write_char_req_struct*)ilm_ptr->local_para_ptr;    
    status = GATTC_WriteChar(req->reg_ctx, req->conn_ctx, &req->svc_uuid, &req->char_uuid, req->write_type, req->value.len, req->value.value, req->auth_req);
    if(status != BT_STATUS_PENDING){
        cnf = (bt_gattc_write_char_cnf_struct*) construct_local_para(sizeof(bt_gattc_write_char_cnf_struct), TD_UL);
        cnf->reg_ctx = req->reg_ctx;
        cnf->result = (U8)status;
        cnf->conn_ctx = req->conn_ctx;
        cnf->svc_uuid = req->svc_uuid;
        cnf->char_uuid = req->char_uuid;
        BT_SendMessage(MSG_ID_BT_GATTC_WRITE_CHAR_CNF, MOD_MMI, cnf, cnf->msg_len);
    }
}

void gattc_adp_read_descr_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_read_descr_req_struct *req;
    bt_gattc_read_descr_cnf_struct *cnf = NULL;

    req = (bt_gattc_read_descr_req_struct*)ilm_ptr->local_para_ptr;    
    status = GATTC_ReadDescr(req->reg_ctx, req->conn_ctx, &req->svc_uuid, &req->char_uuid, &req->descr_uuid, req->auth_req);
    if(status != BT_STATUS_PENDING){
        cnf = (bt_gattc_read_descr_cnf_struct*) construct_local_para(sizeof(bt_gattc_read_descr_cnf_struct), TD_UL);
        cnf->reg_ctx = req->reg_ctx;
        cnf->result = (U8)status;
        cnf->conn_ctx = req->conn_ctx;
        cnf->svc_uuid = req->svc_uuid;
        cnf->descr_uuid = req->descr_uuid;
        cnf->value.len = 0;
        BT_SendMessage(MSG_ID_BT_GATTC_READ_DESCR_CNF, MOD_MMI, cnf, cnf->msg_len);
    }
}

void gattc_adp_write_descr_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gattc_write_descr_req_struct *req;
    bt_gattc_write_descr_cnf_struct *cnf = NULL;

    req = (bt_gattc_write_descr_req_struct*)ilm_ptr->local_para_ptr;    
    status = GATTC_WriteDescr(req->reg_ctx, req->conn_ctx, &req->svc_uuid, &req->char_uuid, &req->descr_uuid, req->write_type, req->value.len, req->value.value, req->auth_req);
    if(status != BT_STATUS_PENDING){
        cnf = (bt_gattc_write_descr_cnf_struct*) construct_local_para(sizeof(bt_gattc_write_descr_cnf_struct), TD_UL);
        cnf->reg_ctx = req->reg_ctx;
        cnf->result = (U8)status;
        cnf->conn_ctx = req->conn_ctx;
        cnf->svc_uuid = req->svc_uuid;
        cnf->descr_uuid = req->descr_uuid;
        BT_SendMessage(MSG_ID_BT_GATTC_WRITE_DESCR_CNF, MOD_MMI, cnf, cnf->msg_len);
    }
}

void gattc_adp_exec_write_req_handler(ilm_struct *ilm_ptr){
    BtStatus status = BT_STATUS_SUCCESS;
    bt_gattc_exec_write_req_struct *req;
    bt_gattc_exec_write_cnf_struct *cnf = NULL;
    // TODO:
    req = (bt_gattc_exec_write_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTC_ExecWrite(req->reg_ctx, req->conn_ctx, req->exeute);
    if(status != BT_STATUS_PENDING){    
        cnf = (bt_gattc_exec_write_cnf_struct*) construct_local_para(sizeof(bt_gattc_exec_write_cnf_struct), TD_UL);
        cnf->reg_ctx = req->reg_ctx;
        cnf->result = (U8)status;
        cnf->conn_ctx = req->conn_ctx;
        BT_SendMessage(MSG_ID_BT_GATTC_EXEC_WRITE_CNF, MOD_MMI, cnf, cnf->msg_len);
    }
}

void gattc_adp_register_notification_req_handler(ilm_struct *ilm_ptr){
    BtStatus status = BT_STATUS_SUCCESS;
    bt_gattc_register_notification_req_struct *req;
    bt_gattc_register_notification_cnf_struct *cnf = NULL;

    req = (bt_gattc_register_notification_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTC_RegisterNotification(req->reg_ctx, req->bd_addr, &req->svc_uuid, &req->char_uuid, 1);
    cnf = (bt_gattc_register_notification_cnf_struct*) construct_local_para(sizeof(bt_gattc_register_notification_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    OS_MemCopy(cnf->bd_addr, req->bd_addr, BD_ADDR_SIZE);
    cnf->svc_uuid = req->svc_uuid;
    cnf->char_uuid = req->char_uuid;
    BT_SendMessage(MSG_ID_BT_GATTC_REGISTER_NOTIFICATION_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gattc_adp_deregister_notification_req_handler(ilm_struct *ilm_ptr){
    BtStatus status = BT_STATUS_SUCCESS;
    bt_gattc_deregister_notification_req_struct *req;
    bt_gattc_deregister_notification_cnf_struct *cnf = NULL;

    req = (bt_gattc_deregister_notification_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTC_RegisterNotification(req->reg_ctx, req->bd_addr, &req->svc_uuid, &req->char_uuid, 0);
    cnf = (bt_gattc_deregister_notification_cnf_struct*) construct_local_para(sizeof(bt_gattc_deregister_notification_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    OS_MemCopy(cnf->bd_addr, req->bd_addr, BD_ADDR_SIZE);
    cnf->svc_uuid = req->svc_uuid;
    cnf->char_uuid = req->char_uuid;
    BT_SendMessage(MSG_ID_BT_GATTC_DEREGISTER_NOTIFICATION_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gattc_adp_read_rssi_req_handler(ilm_struct *ilm_ptr){
    BtStatus status = BT_STATUS_SUCCESS;
    bt_gattc_read_rssi_req_struct *req;
    bt_gattc_read_rssi_cnf_struct *cnf = NULL;
    
    req = (bt_gattc_read_rssi_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTC_ReadRssi(req->reg_ctx, req->bd_addr);
    if(status != BT_STATUS_PENDING){    
        cnf = (bt_gattc_read_rssi_cnf_struct*) construct_local_para(sizeof(bt_gattc_read_rssi_cnf_struct), TD_UL);
        cnf->reg_ctx = req->reg_ctx;
        cnf->result = (U8)status;
        OS_MemCopy(cnf->bd_addr, req->bd_addr, BD_ADDR_SIZE);
        cnf->rssi = 0;
        BT_SendMessage(MSG_ID_BT_GATTC_READ_RSSI_CNF, MOD_MMI, cnf, cnf->msg_len);
    }
}

void gattc_adp_get_device_type_req_handler(ilm_struct *ilm_ptr){
    bt_gattc_get_device_type_req_struct *req;
    bt_gattc_get_device_type_cnf_struct *cnf = NULL;

    req = (bt_gattc_get_device_type_req_struct*)ilm_ptr->local_para_ptr;
    cnf = (bt_gattc_get_device_type_cnf_struct*) construct_local_para(sizeof(bt_gattc_get_device_type_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = BT_STATUS_SUCCESS;
    OS_MemCopy(cnf->bd_addr, req->bd_addr, BD_ADDR_SIZE);
    cnf->dev_type = GATTC_GetDeviceType(req->reg_ctx, req->bd_addr);
    BT_SendMessage(MSG_ID_BT_GATTC_GET_DEVICE_TYPE_CNF, MOD_MMI, cnf, cnf->msg_len);        
}

/*****************************************************************************
 * FUNCTION
 *  gattc_handler
 * DESCRIPTION
 *  This function is to handle GATT messages sent from other tasks
 * PARAMETERS
 *  ilm_ptr     [IN]  inter layer message     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_gattc_handle_message(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GATTC_DBG("Handle msg %d", ilm_ptr->msg_id);
    switch(ilm_ptr->msg_id){
        case MSG_ID_BT_GATTC_REGISTER_REQ:
            gattc_adp_register_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_DEREGISTER_REQ:
            gattc_adp_deregister_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_SCAN_REQ:
            gattc_adp_scan_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_SCAN_CANCEL_REQ:
            gattc_adp_scan_cancel_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_CONNECT_REQ:
            gattc_adp_connect_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_DISCONNECT_REQ:
            gattc_adp_disconnect_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_REFRESH_REQ:
            gattc_adp_refresh_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_SEARCH_SERVICE_REQ:
            gattc_adp_search_service_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_GET_INCL_SERVICE_REQ:
            gattc_adp_get_incl_service_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_GET_CHAR_REQ:
            gattc_adp_get_char_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_GET_DESCR_REQ:
            gattc_adp_get_descr_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_READ_CHAR_REQ:
            gattc_adp_read_char_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_WRITE_CHAR_REQ:
            gattc_adp_write_char_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_READ_DESCR_REQ:
            gattc_adp_read_descr_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_WRITE_DESCR_REQ:
            gattc_adp_write_descr_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_EXEC_WRITE_REQ:
            gattc_adp_exec_write_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_REGISTER_NOTIFICATION_REQ:
            gattc_adp_register_notification_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_DEREGISTER_NOTIFICATION_REQ:
            gattc_adp_deregister_notification_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_READ_RSSI_REQ:
            gattc_adp_read_rssi_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTC_GET_DEVICE_TYPE_REQ:
            gattc_adp_get_device_type_req_handler(ilm_ptr);
            break;
        default:
            GATTC_ERR("Unknown GATTS message : %d", ilm_ptr->msg_id);
            break;
    }
}

void gattc_adp_init(void){
}

void gattc_adp_deinit(void){
}

#endif /* __BT_GATTC_PROFILE__ */
