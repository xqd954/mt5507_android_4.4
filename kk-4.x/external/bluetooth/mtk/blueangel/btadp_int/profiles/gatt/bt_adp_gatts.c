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

#if defined(__BT_GATTS_PROFILE__)

#include "bt_adp_gatt.h"
#include "gatt_srv.h"
#include "bt_message.h"
#include "bluetooth_gatts_struct.h"

void gatts_adp_connected_ind_handler(GATTS_REG_HANDLE handle, U8 result, GattsConnectedInd *ind){
    bt_gatts_connected_ind_struct *msg;
    msg = (bt_gatts_connected_ind_struct*) construct_local_para(sizeof(bt_gatts_connected_ind_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->result = result;
    msg->conn_ctx = ind->connHdl;
    OS_MemCopy(msg->bd_addr, ind->bdaddr->addr, BD_ADDR_SIZE);
    BT_SendMessage(MSG_ID_BT_GATTS_CONNECTED_IND, MOD_MMI, msg, msg->msg_len);
}

void gatts_adp_disconnected_ind_handler(GATTS_REG_HANDLE handle, U8 result, GattsDisconnectedInd *ind){
    bt_gatts_disconnected_ind_struct *msg;
    msg = (bt_gatts_disconnected_ind_struct*) construct_local_para(sizeof(bt_gatts_disconnected_ind_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->result = result;
    msg->conn_ctx = ind->connHdl;
    OS_MemCopy(msg->bd_addr, ind->bdaddr->addr, BD_ADDR_SIZE);
    BT_SendMessage(MSG_ID_BT_GATTS_DISCONNECTED_IND, MOD_MMI, msg, msg->msg_len);
}

void gatts_adp_read_req_ind_handler(GATTS_REG_HANDLE handle, U8 result, GattsReadReqInd *ind){
    bt_gatts_read_request_ind_struct *msg;
    msg = (bt_gatts_read_request_ind_struct*) construct_local_para(sizeof(bt_gatts_read_request_ind_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->conn_ctx = ind->connHdl;
    msg->trans_id = ind->trans_id;
    OS_MemCopy(msg->bd_addr, ind->bdaddr->addr, BD_ADDR_SIZE);
    msg->handle = ind->handle;
    msg->is_long = ind->is_long;
    msg->offset = ind->offset;
    BT_SendMessage(MSG_ID_BT_GATTS_READ_REQUEST_IND, MOD_MMI, msg, msg->msg_len);
}

void gatts_adp_write_req_ind_handler(GATTS_REG_HANDLE handle, U8 result, GattsWriteReqInd *ind){
    bt_gatts_write_request_ind_struct *msg;
    msg = (bt_gatts_write_request_ind_struct*) construct_local_para(sizeof(bt_gatts_write_request_ind_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->conn_ctx = ind->connHdl;
    msg->trans_id = ind->trans_id;
    OS_MemCopy(msg->bd_addr, ind->bdaddr->addr, BD_ADDR_SIZE);
    msg->handle = ind->handle;
    msg->value.len = ind->value_len;
    OS_MemCopy(msg->value.value, ind->value, ind->value_len);
    msg->need_rsp = ind->need_rsp;
    msg->is_prepare = ind->is_prepare;
    msg->offset = ind->offset;
    BT_SendMessage(MSG_ID_BT_GATTS_WRITE_REQUEST_IND, MOD_MMI, msg, msg->msg_len);
}

void gatts_adp_execute_write_req_ind_handler(GATTS_REG_HANDLE handle, U8 result, GattsExecWriteReqInd *ind){
    bt_gatts_exec_write_request_ind_struct *msg;
    msg = (bt_gatts_exec_write_request_ind_struct*) construct_local_para(sizeof(bt_gatts_exec_write_request_ind_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->conn_ctx = ind->connHdl;
    msg->trans_id = ind->trans_id;
    msg->cancel = ind->exec_write ? 0 : 1;
    OS_MemCopy(msg->bd_addr, ind->bdaddr->addr, BD_ADDR_SIZE);
    BT_SendMessage(MSG_ID_BT_GATTS_EXEC_WRITE_REQUEST_IND, MOD_MMI, msg, msg->msg_len);
}

void gatts_adp_rsp_result_handler(GATTS_REG_HANDLE handle, U8 result, GattsRspResult *ind){
}

void gatts_adp_confirmation_result_handler(GATTS_REG_HANDLE handle, U8 result, GattsConfirmationResult *ind){
    bt_gatts_send_indication_cnf_struct *msg;
    msg = (bt_gatts_send_indication_cnf_struct*) construct_local_para(sizeof(bt_gatts_send_indication_cnf_struct), TD_UL);
    msg->reg_ctx = handle;
    msg->result = result;
    msg->conn_ctx = ind->connHdl;
    BT_SendMessage(MSG_ID_BT_GATTS_SEND_INDICATION_CNF, MOD_MMI, msg, msg->msg_len);    
}

static void gattsAdpCallback(GATTS_REG_HANDLE handle, GattsCallbackParms *Parms){
    OS_Report("gattsAdpCallback : handle=%p, event=%u", handle, Parms->event);
    switch(Parms->event){
    case GATTS_EVENT_CONNECTED_IND:
        gatts_adp_connected_ind_handler(handle, Parms->result, &Parms->param.connected_ind);
        break;
    case GATTS_EVENT_DISCONNECTED_IND:
        gatts_adp_disconnected_ind_handler(handle, Parms->result, &Parms->param.disconnected_ind);
        break;
    case GATTS_EVENT_READ_REQ_IND:
        gatts_adp_read_req_ind_handler(handle, Parms->result, &Parms->param.read_req_ind);
        break;
    case GATTS_EVENT_WRITE_REQ_IND:
        gatts_adp_write_req_ind_handler(handle, Parms->result, &Parms->param.write_req_ind);
        break;
    case GATTS_EVENT_EXEC_WRITE_REQ_IND:
        gatts_adp_execute_write_req_ind_handler(handle, Parms->result, &Parms->param.exec_write_req_ind);
        break;
    case GATTS_EVENT_RSP_RESULT:
        gatts_adp_rsp_result_handler(handle, Parms->result, &Parms->param.rsp_result);
        break;
    case GATTS_EVENT_CONFIRMATION_RESULT:
        gatts_adp_confirmation_result_handler(handle, Parms->result, &Parms->param.confirmation_result);
        break;
    default:
        GATTS_ERR("Invalid callback event %u", Parms->event);
        break;
    }
}

void gatts_adp_register_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    GATTS_REG_HANDLE handle = INVALID_GATTS_REG_HANDLE;
    bt_gatts_register_req_struct *req = (bt_gatts_register_req_struct*)ilm_ptr->ilm_data;
    bt_gatts_register_cnf_struct *cnf = NULL;
    
    req = (bt_gatts_register_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTS_Register(&handle , gattsAdpCallback);
    cnf = (bt_gatts_register_cnf_struct*) construct_local_para(sizeof(bt_gatts_register_cnf_struct), TD_UL);
    OS_MemCopy(cnf->user_id, req->user_id, sizeof(cnf->user_id));
    cnf->reg_ctx = handle;
    cnf->result = (U8)status;
    BT_SendMessage(MSG_ID_BT_GATTS_REGISTER_CNF, MOD_MMI, cnf, cnf->msg_len);
    if(status == BT_STATUS_SUCCESS){
        GATTS_ReportAllConnectedLink(handle);
    }
}

void gatts_adp_deregister_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gatts_deregister_req_struct *req = (bt_gatts_deregister_req_struct*)ilm_ptr->ilm_data;
    bt_gatts_deregister_cnf_struct *cnf = NULL;
    
    req = (bt_gatts_deregister_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTS_Deregister(req->reg_ctx);
    cnf = (bt_gatts_deregister_cnf_struct*) construct_local_para(sizeof(bt_gatts_deregister_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    BT_SendMessage(MSG_ID_BT_GATTS_DEREGISTER_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gatts_adp_connect_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gatts_connect_req_struct *req = (bt_gatts_connect_req_struct*)ilm_ptr->ilm_data;
    bt_gatts_connect_cnf_struct *cnf = NULL;
    GATTS_CONN_HANDLE connHdl = INVALID_GATTS_CONN_HANDLE;
    
    req = (bt_gatts_connect_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTS_Open(req->reg_ctx, (BD_ADDR*)req->bd_addr, req->direct, &connHdl);
    cnf = (bt_gatts_connect_cnf_struct*) construct_local_para(sizeof(bt_gatts_connect_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    cnf->conn_ctx = connHdl;
    OS_MemCopy(cnf->bd_addr, req->bd_addr, BD_ADDR_SIZE);;
    BT_SendMessage(MSG_ID_BT_GATTS_CONNECT_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gatts_adp_disconnect_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gatts_disconnect_req_struct *req = (bt_gatts_disconnect_req_struct*)ilm_ptr->ilm_data;
    bt_gatts_disconnect_cnf_struct *cnf = NULL;
    
    req = (bt_gatts_disconnect_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTS_Close(req->reg_ctx, req->conn_ctx);
    cnf = (bt_gatts_disconnect_cnf_struct*) construct_local_para(sizeof(bt_gatts_disconnect_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    cnf->conn_ctx = req->conn_ctx;
    OS_MemCopy(cnf->bd_addr, req->bd_addr, BD_ADDR_SIZE);
    BT_SendMessage(MSG_ID_BT_GATTS_DISCONNECT_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gatts_adp_add_service_req_handler(ilm_struct *ilm_ptr){
    ATT_HANDLE handle;
    bt_gatts_add_service_req_struct *req = (bt_gatts_add_service_req_struct*)ilm_ptr->ilm_data;
    bt_gatts_add_service_cnf_struct *cnf = NULL;

    req = (bt_gatts_add_service_req_struct*)ilm_ptr->local_para_ptr;
    handle = GATTS_AddService(req->reg_ctx, &req->uuid, req->num_handles);
    cnf = (bt_gatts_add_service_cnf_struct*) construct_local_para(sizeof(bt_gatts_add_service_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (handle != INVALID_ATT_HANDLE) ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
    cnf->service_handle = handle;
    cnf->uuid = req->uuid;
    BT_SendMessage(MSG_ID_BT_GATTS_ADD_SERVICE_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gatts_adp_add_incl_service_req_handler(ilm_struct *ilm_ptr){
    ATT_HANDLE handle;
    bt_gatts_add_incl_service_req_struct *req = (bt_gatts_add_incl_service_req_struct*)ilm_ptr->ilm_data;
    bt_gatts_add_incl_service_cnf_struct *cnf = NULL;

    req = (bt_gatts_add_incl_service_req_struct*)ilm_ptr->local_para_ptr;
    handle = GATTS_AddInclService(req->service_handle, req->incl_service_handle);
    cnf = (bt_gatts_add_incl_service_cnf_struct*) construct_local_para(sizeof(bt_gatts_add_incl_service_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (handle != INVALID_ATT_HANDLE) ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
    cnf->service_handle = req->service_handle;
    cnf->incl_service_handle = handle;
    BT_SendMessage(MSG_ID_BT_GATTS_ADD_INCL_SERVICE_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gatts_adp_add_char_req_handler(ilm_struct *ilm_ptr){
    ATT_HANDLE handle;
    bt_gatts_add_char_req_struct *req = (bt_gatts_add_char_req_struct*)ilm_ptr->ilm_data;
    bt_gatts_add_char_cnf_struct *cnf = NULL;

    req = (bt_gatts_add_char_req_struct*)ilm_ptr->local_para_ptr;
    handle = GATTS_AddChar(req->service_handle, &req->char_uuid, req->properties, req->permission);
    cnf = (bt_gatts_add_char_cnf_struct*) construct_local_para(sizeof(bt_gatts_add_char_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (handle != INVALID_ATT_HANDLE) ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
    cnf->service_handle = req->service_handle;
    cnf->char_uuid = req->char_uuid;
    cnf->char_handle = handle;
    BT_SendMessage(MSG_ID_BT_GATTS_ADD_CHAR_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gatts_adp_add_descr_req_handler(ilm_struct *ilm_ptr){
    ATT_HANDLE handle;
    bt_gatts_add_descr_req_struct *req = (bt_gatts_add_descr_req_struct*)ilm_ptr->ilm_data;
    bt_gatts_add_descr_cnf_struct *cnf = NULL;
    
    req = (bt_gatts_add_descr_req_struct*)ilm_ptr->local_para_ptr;
    handle = GATTS_AddDescr(req->service_handle, &req->descr_uuid, req->permission);
    cnf = (bt_gatts_add_descr_cnf_struct*) construct_local_para(sizeof(bt_gatts_add_descr_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (handle != INVALID_ATT_HANDLE) ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
    cnf->service_handle = req->service_handle;
    cnf->descr_uuid = req->descr_uuid;    
    cnf->descr_handle = handle;
    BT_SendMessage(MSG_ID_BT_GATTS_ADD_DESCR_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gatts_adp_start_service_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gatts_start_service_req_struct *req = (bt_gatts_start_service_req_struct*)ilm_ptr->ilm_data;
    bt_gatts_start_service_cnf_struct *cnf = NULL;
    GattTransport transport = 0;

    if(req->transport == 1 || req->transport == 2){
        transport |= GATT_TRANSPORT_BR_EDR;
    }
    if(req->transport == 0 || req->transport == 2){
        transport |= GATT_TRANSPORT_LE;
    }
    
    req = (bt_gatts_start_service_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTS_StartService(req->service_handle, transport);
    cnf = (bt_gatts_start_service_cnf_struct*) construct_local_para(sizeof(bt_gatts_start_service_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    cnf->service_handle = req->service_handle;
    BT_SendMessage(MSG_ID_BT_GATTS_START_SERVICE_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gatts_adp_stop_service_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gatts_stop_service_req_struct *req = (bt_gatts_stop_service_req_struct*)ilm_ptr->ilm_data;
    bt_gatts_stop_service_cnf_struct *cnf = NULL;
    
    req = (bt_gatts_stop_service_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTS_StopService(req->service_handle);
    cnf = (bt_gatts_stop_service_cnf_struct*) construct_local_para(sizeof(bt_gatts_stop_service_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    cnf->service_handle = req->service_handle;
    BT_SendMessage(MSG_ID_BT_GATTS_STOP_SERVICE_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gatts_adp_delete_service_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gatts_delete_service_req_struct *req = (bt_gatts_delete_service_req_struct*)ilm_ptr->ilm_data;
    bt_gatts_delete_service_cnf_struct *cnf = NULL;
    
    req = (bt_gatts_delete_service_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTS_DeleteService(req->service_handle);
    cnf = (bt_gatts_delete_service_cnf_struct*) construct_local_para(sizeof(bt_gatts_delete_service_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    cnf->service_handle = req->service_handle;
    BT_SendMessage(MSG_ID_BT_GATTS_DELETE_SERVICE_CNF, MOD_MMI, cnf, cnf->msg_len);
}

void gatts_adp_send_indication_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gatts_send_indication_req_struct *req = (bt_gatts_send_indication_req_struct*)ilm_ptr->ilm_data;
    bt_gatts_send_indication_cnf_struct *cnf = NULL;
    
    req = (bt_gatts_send_indication_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTS_Indication(req->reg_ctx, req->conn_ctx, req->handle, req->need_confirm, &req->value);
    if(status != BT_STATUS_PENDING){
        cnf = (bt_gatts_send_indication_cnf_struct*) construct_local_para(sizeof(bt_gatts_send_indication_cnf_struct), TD_UL);
        cnf->reg_ctx = req->reg_ctx;
        cnf->result = (U8)status;
        cnf->handle = req->handle;
        cnf->conn_ctx = req->conn_ctx;
        BT_SendMessage(MSG_ID_BT_GATTS_SEND_INDICATION_CNF, MOD_MMI, cnf, cnf->msg_len);
    }
}

void gatts_adp_send_response_req_handler(ilm_struct *ilm_ptr){
    BtStatus status;
    bt_gatts_send_response_req_struct *req = (bt_gatts_send_response_req_struct*)ilm_ptr->ilm_data;
    bt_gatts_send_response_cnf_struct *cnf = NULL;
    
    req = (bt_gatts_send_response_req_struct*)ilm_ptr->local_para_ptr;
    status = GATTS_Response(req->reg_ctx, req->conn_ctx, req->handle, req->result, &req->value);
    cnf = (bt_gatts_send_response_cnf_struct*) construct_local_para(sizeof(bt_gatts_send_response_cnf_struct), TD_UL);
    cnf->reg_ctx = req->reg_ctx;
    cnf->result = (U8)status;
    cnf->handle = req->handle;
    cnf->conn_ctx = req->conn_ctx;
    BT_SendMessage(MSG_ID_BT_GATTS_SEND_RESPONSE_CNF, MOD_MMI, cnf, cnf->msg_len);
}

/*****************************************************************************
 * FUNCTION
 *  gatts_handler
 * DESCRIPTION
 *  This function is to handle GATT Server messages sent from other tasks
 * PARAMETERS
 *  ilm_ptr     [IN]  inter layer message     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_gatts_handle_message(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch(ilm_ptr->msg_id){
        case MSG_ID_BT_GATTS_REGISTER_REQ:
            gatts_adp_register_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTS_DEREGISTER_REQ:
            gatts_adp_deregister_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTS_CONNECT_REQ:
            gatts_adp_connect_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTS_DISCONNECT_REQ:
            gatts_adp_disconnect_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTS_ADD_SERVICE_REQ:
            gatts_adp_add_service_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTS_ADD_INCL_SERVICE_REQ:
            gatts_adp_add_incl_service_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTS_ADD_CHAR_REQ:
            gatts_adp_add_char_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTS_ADD_DESCR_REQ:
            gatts_adp_add_descr_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTS_START_SERVICE_REQ:                                       /* OK */
            gatts_adp_start_service_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTS_STOP_SERVICE_REQ:                                    /* ERROR or +CME ERROR : <err> */
            gatts_adp_stop_service_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTS_DELETE_SERVICE_REQ:                           /* NO CARRIER */
            gatts_adp_delete_service_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTS_SEND_INDICATION_REQ:                                     /* BUSY */
            gatts_adp_send_indication_req_handler(ilm_ptr);
            break;
        case MSG_ID_BT_GATTS_SEND_RESPONSE_REQ:                            /* NO ANSWER */
            gatts_adp_send_response_req_handler(ilm_ptr);
            break;
        default:
            GATTS_ERR("Unknown GATTS message : %d", ilm_ptr->msg_id);
            break;
    }
}


void gatts_adp_init(void){
}

void gatts_adp_deinit(void){
}

#endif /* __BT_GATTS_PROFILE__ */
