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

 /*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 * bt_bipi_comm.c
 *
 * Project:
 * --------
 *   Export function from BIP client
 *
 * Description:
 * ------------
 *   This file is for MMI BIP client.
 *
 * Author:
 * -------
 * Zhigang Yu
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Log: $
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_BIPI_PROFILE__

#ifdef  __MTK_BT_EXTERNAL_PLATFORM__
#include "bt_ext_debug.h"
#else
#include <cutils/xlog.h>
#endif
#define BIP_DBG(fmt, ...) XLOGD("[%s] %s(%d):" fmt, __FILE__, __FUNCTION__, __LINE__,  ## __VA_ARGS__)

/*
 * utility function start
*/
static U8 bipi_check_handle(HMTKBIPI hBipI)
{
    if (!btmtk_bipi_verify_handle(hBipI))
    {
#ifdef __BT_BIP_CMD_AGENT__
        BIP_PROCESS_DEAD_ASSERT(BT_BIP_ERR_INVALID_PARA);
#endif
        return 0;
    }
    return 1;
}
void btmtk_bipi_notify_app(bt_bipi_context* contxt, bt_bipi_para* para, U8 force)
{

    //kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_NOTIFY_APP, para->opcode, para->result);

    if (force)
    {
        para->handle = (HMTKBIPI)contxt;
        contxt->init.callback(contxt->init.para, para);
    }
    /* agent case, invoke callback anyway */
#ifdef __BT_BIP_CMD_AGENT__
    else
    {
        para->handle = (HMTKBIPI)contxt;
        contxt->init.callback(contxt->init.para, para);
    }
#endif
}

/* send request to BT */
static void bipi_send_request(msg_type msg_id, void* para, U32 size)
{
    BTCMD_SendMessage(msg_id, MOD_BT, para, (U16)size);
    
    if (para)
    {
        BIP_DBG("after BTCMD_SendMessage, msg=%u, ptr=0x%X, size=%lu", msg_id, (unsigned int)para, size);
//mtk03036
//        free_local_para(para);
        FREE_LOCAL_PARA((void*)para);
    }
}

/*
 * utility function end
*/

/*
 * API function implement
*/
static S32 bipi_activate(bt_bipi_active_info* init)
{
    bt_bipi_context* handle;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));




    handle = btmtk_bipi_alloc_handle();
    if (!handle)
    {
        /* no resource */
        para.result = BT_BIP_ERR_NO_RESOURCE;
        para.opcode = BT_BIP_OP_ACTIVATE;
        init->callback(init->para, &para);
        return BT_BIP_ERR_NO_RESOURCE;
    }
    else
    {
        para.cnf.activate.handle = handle;
        para.result = BT_BIP_ERR_OK;

        handle->init = *init;
        //context->status = BIPI_STATUS_ACTIVATED; set in alloc already
    }
    para.opcode = BT_BIP_OP_ACTIVATE;
    btmtk_bipi_notify_app(handle, &para, 1);
    return para.result;
}

static void bipi_send_connect_req(bt_bipi_context* cntx, bt_bip_obex_auth_rsp* auth_rsp)
{
    bt_bip_connect_req* conn = &cntx->connect;
    bt_bip_connect_req_struct* req;

    
    /* reset connect_id */
    cntx->connect_id = (U32)-1; 
    
    req = (bt_bip_connect_req_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_connect_req_struct));
    req->bd_addr.lap = conn->addr.lap;
    req->bd_addr.uap = conn->addr.uap;
    req->bd_addr.nap = conn->addr.nap;

    BIP_DBG("req lap:%3lx, uap:%2x, nap:%x", req->bd_addr.lap, req->bd_addr.uap, req->bd_addr.nap);



    req->req_id = cntx->req_id;
    req->bip_service = conn->service;

    /* challenge */
    if (conn->auth_req.pwd_len)
    {
        req->challenge.pwd_len = conn->auth_req.pwd_len;
        memcpy(&req->challenge.pwd, &conn->auth_req.pwd, req->challenge.pwd_len);

        req->challenge.realm_len = conn->auth_req.realm_len;
        if (req->challenge.realm_len)
            memcpy(&req->challenge.realm, &conn->auth_req.realm, req->challenge.realm_len);

        req->challenge.uid_len = conn->auth_req.uid_len;
        if (req->challenge.uid_len)
            memcpy(&req->challenge.uid, &conn->auth_req.uid, req->challenge.uid_len);
    }

    /* challenge response */
    if (auth_rsp)
    {
        /*
        req->response.pwd_len = auth_rsp->pwd_len;
        if (req->response.pwd_len)
            memcpy(&req->response.pwd, &auth_rsp->pwd, req->response.pwd_len);

        req->response.uid_len = auth_rsp->uid_len;
        if (req->response.uid_len)
            memcpy(&req->response.uid, &auth_rsp->uid, req->response.uid_len);
        */
    }


    bipi_send_request(MSG_ID_BT_BIP_CONNECT_REQ, req, sizeof(bt_bip_connect_req_struct));
}

static S32 bipi_connect(HMTKBIPI hBipI, bt_bip_connect_req* conn)
{
    bt_bipi_context* cntx = (bt_bipi_context*)hBipI;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (!bipi_check_handle(hBipI))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPI_STATUS_ACTIVATED)
    {
        para.opcode = BT_BIP_OP_CONNECT;
        para.result = BT_BIP_ERR_STATE;
        btmtk_bipi_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    /* save connect information */
    cntx->connect = *conn;

    /* change status fristly */
    btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONNECTING);
    cntx->opcode = BT_BIP_OP_CONNECT;

    bipi_send_connect_req(cntx, NULL);

    return BT_BIP_ERR_PROCESSING;
}

static S32 bipi_obex_auth_rsp(HMTKBIPI hBipI, bt_bip_obex_auth_rsp* rsp)
{
    bt_bipi_context* cntx = (bt_bipi_context*)hBipI;
    bt_bipi_para para;
    bt_bip_obex_auth_rsp_struct* irsp;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (!bipi_check_handle(hBipI))
        return BT_BIP_ERR_INVALID_PARA;

    /* now we only support CONNECTING case! */
    if (cntx->status != BIPI_STATUS_CN_BE_CHAL
        /* && context->status != BIPI_STATUS_REQ_AUTH */)
    {
        /* now we do not callback auth_rsp result */

        para.opcode = BT_BIP_OP_OBEX_AUTH;
        para.result = BT_BIP_ERR_STATE;
        btmtk_bipi_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    /* reconnect with challenge response */
    btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONNECTING);

    irsp = (bt_bip_obex_auth_rsp_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_obex_auth_rsp_struct));
    if (rsp->result == BT_BIP_ERR_OK)
    {
        irsp->result = BT_BIP_CNF_SUCCESS;
        irsp->dig_rsp.pwd_len = rsp->pwd_len;
        memcpy(irsp->dig_rsp.pwd, rsp->pwd, rsp->pwd_len);
        irsp->dig_rsp.uid_len = rsp->uid_len;
    }
    else
    {
        irsp->result = BT_BIP_CNF_FAILED;
    }
    if (rsp->uid_len)
        memcpy(irsp->dig_rsp.uid, rsp->uid, rsp->uid_len);

    bipi_send_request(MSG_ID_BT_BIPI_OBEX_AUTH_RSP, irsp, sizeof(bt_bip_obex_auth_rsp_struct));

    return BT_BIP_ERR_PROCESSING;
}

static S32 bipi_get_capabilities(HMTKBIPI hBipI)
{
    bt_bipi_context* cntx = (bt_bipi_context*)hBipI;
    bt_bipi_para para;
    bt_bip_get_capabilities_req_struct* req;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (!bipi_check_handle(hBipI))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPI_STATUS_CONNECTED)
    {
        para.opcode = BT_BIP_OP_GET_CAPABILITIES;
        para.result = BT_BIP_ERR_STATE;
        btmtk_bipi_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_REQUESTING);
    cntx->opcode = BT_BIP_OP_GET_CAPABILITIES;

    req = (bt_bip_get_capabilities_req_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_get_capabilities_req_struct));
    req->cm_conn_id = cntx->connect_id;
    bipi_send_request(MSG_ID_BT_BIP_GET_CAPABILITIES_REQ,
        req,
        sizeof(bt_bip_get_capabilities_req_struct));

    return BT_BIP_ERR_PROCESSING;
}

static S32 bipi_get_images_list(HMTKBIPI hBipI, bt_bip_get_img_list_req* req)
{
    bt_bipi_context* cntx = (bt_bipi_context*)hBipI;
    bt_bipi_para para;
    bt_bip_get_img_list_req_struct* ireq;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (!bipi_check_handle(hBipI))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPI_STATUS_CONNECTED)
    {
        para.opcode = BT_BIP_OP_GET_IMG_LIST;
        para.result = BT_BIP_ERR_STATE;
        btmtk_bipi_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_REQUESTING);
    cntx->opcode = BT_BIP_OP_GET_IMG_LIST;

    ireq = (bt_bip_get_img_list_req_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_get_img_list_req_struct));
    ireq->cm_conn_id = cntx->connect_id;
    /* ireq->img_list_descriptor. = req->handle_desc.created; */
    ireq->latest_captured = req->latest_captured;
    ireq->max_img_handle_number = (U16)req->max_img_number;
    ireq->start_index = (U16)req->start_index;
    bipi_send_request(MSG_ID_BT_BIP_GET_IMG_LIST_REQ,
        ireq,
        sizeof(bt_bip_get_img_list_req_struct));

    return BT_BIP_ERR_PROCESSING;
}

static S32 bipi_get_image_properties(HMTKBIPI hBipI,bt_bip_get_img_prop_req* req)
{
    bt_bipi_context* cntx = (bt_bipi_context*)hBipI;
    bt_bipi_para para;
    bt_bip_get_img_prop_req_struct* ireq;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (!bipi_check_handle(hBipI))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPI_STATUS_CONNECTED)
    {
        para.opcode = BT_BIP_OP_GET_IMG_PROPERTIES;
        para.result = BT_BIP_ERR_STATE;
        btmtk_bipi_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_REQUESTING);
    cntx->opcode = BT_BIP_OP_GET_IMG_PROPERTIES;

    ireq = (bt_bip_get_img_prop_req_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_get_img_prop_req_struct));
    ireq->cm_conn_id = cntx->connect_id;
    strcpy((char*)ireq->img_handle, (const char*)req->handle);
    bipi_send_request(MSG_ID_BT_BIP_GET_IMG_PROP_REQ,
        ireq,
        sizeof(bt_bip_get_img_prop_req_struct));

    return BT_BIP_ERR_PROCESSING;
}

static S32 bipi_get_image(HMTKBIPI hBipI, bt_bip_get_img_req* req)
{
    bt_bipi_context* cntx = (bt_bipi_context*)hBipI;
    bt_bipi_para para;
    bt_bip_get_img_req_struct* ireq;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (!bipi_check_handle(hBipI))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPI_STATUS_CONNECTED)
    {
        para.opcode = BT_BIP_OP_GET_IMG;
        para.result = BT_BIP_ERR_STATE;
        btmtk_bipi_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_REQUESTING);
    cntx->opcode = BT_BIP_OP_GET_IMG;

    ireq = (bt_bip_get_img_req_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_get_img_req_struct));
    ireq->cm_conn_id = cntx->connect_id;
    /* ireq->img_descriptor = req->img_desc */
    btmtk_wstrcpy(ireq->img_path, req->img_path);
    btmtk_wstrcpy(cntx->file_name, req->img_path);
    strcpy((char*)ireq->img_handle, (const char*)req->handle);
    bipi_send_request(MSG_ID_BT_BIP_GET_IMG_REQ,
        ireq,
        sizeof(bt_bip_get_img_req_struct));

    return BT_BIP_ERR_PROCESSING;
}

static S32 bipi_get_linked_thumbnail(HMTKBIPI hBipI, bt_bip_get_linked_thum_req* req)
{
    bt_bipi_context* cntx = (bt_bipi_context*)hBipI;
    bt_bipi_para para;
    bt_bip_get_img_req_struct* ireq;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (!bipi_check_handle(hBipI))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPI_STATUS_CONNECTED)
    {
        para.opcode = BT_BIP_OP_GET_IMG;
        para.result = BT_BIP_ERR_STATE;
        btmtk_bipi_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_REQUESTING);
    cntx->opcode = BT_BIP_OP_GET_IMG;

    ireq = (bt_bip_get_img_req_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_get_img_req_struct));
    ireq->cm_conn_id = cntx->connect_id;
    /* ireq->img_descriptor = req->img_desc */
    btmtk_wstrcpy(ireq->img_path, req->img_path);
    strcpy((char*)ireq->img_handle, (const char*)req->handle);
    bipi_send_request(MSG_ID_BT_BIP_GET_IMG_REQ,
        ireq,
        sizeof(bt_bip_get_img_req_struct));

    return BT_BIP_ERR_PROCESSING;
}

static S32 bipi_put_image(HMTKBIPI hBipI, bt_bip_put_img_req* req)
{
    bt_bipi_context* cntx = (bt_bipi_context*)hBipI;
    bt_bipi_para para;
    /* this struct seems need modify later... */
    bt_bip_put_img_req_struct* ireq;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (!bipi_check_handle(hBipI))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPI_STATUS_CONNECTED)
    {
        para.opcode = BT_BIP_OP_PUT_IMG;
        para.result = BT_BIP_ERR_STATE;
        btmtk_bipi_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_REQUESTING);
    cntx->opcode = BT_BIP_OP_PUT_IMG;

    ireq = (bt_bip_put_img_req_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_put_img_req_struct));
    ireq->cm_conn_id = cntx->connect_id;
    ireq->img_size = req->img_desc.size;
    btmtk_wstrcpy((U16*)ireq->img_name, req->img_name);
    btmtk_wstrcpy(ireq->img_path, req->img_path);

    memcpy(&ireq->img_descriptor, &req->img_desc, sizeof(bt_bip_img_desc_struct));

    bipi_send_request(MSG_ID_BT_BIP_PUT_IMG_REQ, ireq, sizeof(bt_bip_put_img_req_struct));

    return BT_BIP_ERR_PROCESSING;
}

static S32 bipi_put_linked_thumbnail(HMTKBIPI hBipI, bt_bip_put_linked_thum_req* req)
{
    bt_bipi_context* cntx = (bt_bipi_context*)hBipI;
    bt_bipi_para para;
    bt_bip_put_linked_thumbnail_req_struct* ireq;

    memset((void*)&para, 0, sizeof(bt_bipi_para));



    if (!bipi_check_handle(hBipI))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPI_STATUS_CONNECTED)
    {
        para.opcode = BT_BIP_OP_PUT_LINKED_THUMBNAIL;
        para.result = BT_BIP_ERR_STATE;
        btmtk_bipi_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_REQUESTING);
    cntx->opcode = BT_BIP_OP_PUT_LINKED_THUMBNAIL;

    ireq = (bt_bip_put_linked_thumbnail_req_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_bip_put_linked_thumbnail_req_struct));
    ireq->cm_conn_id = cntx->connect_id;
    strcpy((char*)ireq->img_handle, (char*)req->handle);
    btmtk_wstrcpy(ireq->img_path, req->img_path);

    bipi_send_request(MSG_ID_BT_BIP_PUT_LINKED_THUMBNAIL_REQ,
        ireq,
        sizeof(bt_bip_put_linked_thumbnail_req_struct));

    return BT_BIP_ERR_PROCESSING;
}


static S32 bipi_continue(HMTKBIPI hBipI, S32 cont)
{
    bt_bipi_context* cntx = (bt_bipi_context*)hBipI;
    bt_bipi_para para;
    bt_bip_continue_rsp_struct* rsp;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (!bipi_check_handle(hBipI))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPI_STATUS_INDICATING)
    {
        /* only callback in case of failure */
        para.opcode = BT_BIP_OP_CONTINUE;
        para.result = BT_BIP_ERR_STATE;
        btmtk_bipi_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    /* here do not update opcode */
    btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONTINUE);

    rsp = (bt_bip_continue_rsp_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_continue_rsp_struct));
    rsp->cm_conn_id = cntx->connect_id;
    rsp->cnf_code = cont;

    bipi_send_request(MSG_ID_BT_BIP_CONTINUE_RSP, rsp, sizeof(bt_bip_continue_rsp_struct));

    return BT_BIP_ERR_PROCESSING;
}

static S32 bipi_abort(HMTKBIPI hBipI)
{
    bt_bipi_context* cntx = (bt_bipi_context*)hBipI;
    bt_bipi_para para;
    bt_bip_abort_req_struct* req;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (!bipi_check_handle(hBipI))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPI_STATUS_INDICATING &&
        cntx->status != BIPI_STATUS_CONTINUE &&
        cntx->status != BIPI_STATUS_REQUESTING)
    {
        /* only callback in case of failure */
        para.opcode = BT_BIP_OP_ABORT;
        para.result = BT_BIP_ERR_STATE;
        btmtk_bipi_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_ABORTING);
    /* here do not update opcode, just transform status!!!
     * then in ABORTING status, maybe receive indication, or cnf primitive msg
     *
     * in case of indication event, we just return and waiting for abort cnf
     *
     * in case of cnf event, we should check result:
     *    ok : related opcode callback with BT_BIP_ERR_OK
     *    false: related opcode callback with BT_BIP_ERR_USER_ABORT
     *  then transform status to CONNECTED
     */

    req = (bt_bip_abort_req_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_abort_req_struct));
    req->cm_conn_id = cntx->connect_id;

    bipi_send_request(MSG_ID_BT_BIP_ABORT_REQ, req, sizeof(bt_bip_abort_req_struct));

    return BT_BIP_ERR_PROCESSING;
}

static void bipi_send_disconnect_req(bt_bipi_context* cntx, U32 force)
{
    bt_bip_disconnect_req_struct* req;

    req = (bt_bip_disconnect_req_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_bip_disconnect_req_struct));
    req->cm_conn_id = cntx->connect_id;
    req->session_role = BT_BIP_INITIATOR_ROLE;
    req->req_id = cntx->req_id;
    req->disconnect_tp_directly = force == 0 ? 0 : 1;

    bipi_send_request(MSG_ID_BT_BIP_DISCONNECT_REQ, req, sizeof(bt_bip_disconnect_req_struct));
}

static S32 bipi_disconnect(HMTKBIPI hBipI, U32 force)
{
    bt_bipi_context* cntx = (bt_bipi_context*)hBipI;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (!bipi_check_handle(hBipI))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPI_STATUS_INDICATING &&
        cntx->status != BIPI_STATUS_CONTINUE &&
        cntx->status != BIPI_STATUS_REQUESTING &&
        cntx->status != BIPI_STATUS_CONNECTING &&
        cntx->status != BIPI_STATUS_CONNECTED &&
        cntx->status != BIPI_STATUS_ABORTING)
    {
        /* only callback in case of failure */
        para.opcode = BT_BIP_OP_DISCONNECT;
        para.result = BT_BIP_ERR_STATE;
        btmtk_bipi_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    if (cntx->status == BIPI_STATUS_CONNECTING)
        force = 1;

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_DISCONNECTING);

    bipi_send_disconnect_req(cntx, force);

    return BT_BIP_ERR_PROCESSING;
}

static S32 bipi_deactivate(HMTKBIPI hBipI)
{
    bt_bipi_context* cntx = (bt_bipi_context*)hBipI;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (!bipi_check_handle(hBipI))
        return BT_BIP_ERR_INVALID_PARA;

    switch (cntx->status)
    {
    case BIPI_STATUS_IDLE:
    case BIPI_STATUS_DEACTIVATING:
        para.opcode = BT_BIP_OP_DEACTIVATE;
        para.result = BT_BIP_ERR_STATE;
        btmtk_bipi_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;

    case BIPI_STATUS_ACTIVATED:

        /* needn't status change, since instance has destory */

        btmtk_bipi_status_trans(cntx, BIPI_STATUS_IDLE);

        para.opcode = BT_BIP_OP_DEACTIVATE;
        para.result = BT_BIP_ERR_OK;
        btmtk_bipi_notify_app(cntx, &para, 1);

        btmtk_bipi_free_handle(hBipI);

        return BT_BIP_ERR_OK;

    case BIPI_STATUS_DISCONNECTING:
        btmtk_bipi_status_trans(cntx, BIPI_STATUS_DEACTIVATING);
        /* do no change OPCODE, then can know real entry */
        return BT_BIP_ERR_PROCESSING;

    default:
        btmtk_bipi_status_trans(cntx, BIPI_STATUS_DEACTIVATING);
        bipi_send_disconnect_req(cntx, 1);
        cntx->opcode = BT_BIP_OP_DEACTIVATE;
    }

    return BT_BIP_ERR_PROCESSING;
}


static S32 bipi_handle_cmd(HMTKBIPI hBipI, U32 opcode, void* req)
{
    //kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_APP_CMD, opcode);

    switch (opcode)
    {
    case BT_BIP_OP_ACTIVATE:
        return bipi_activate((bt_bipi_active_info*)req);

    case BT_BIP_OP_DEACTIVATE:
        return bipi_deactivate(hBipI);

    case BT_BIP_OP_CONNECT:
        return bipi_connect(hBipI, (bt_bip_connect_req*)req);

    case BT_BIP_OP_GET_CAPABILITIES:
        return bipi_get_capabilities(hBipI);

    case BT_BIP_OP_GET_IMG_LIST:
        return bipi_get_images_list(hBipI, (bt_bip_get_img_list_req*)req);

    case BT_BIP_OP_GET_IMG_PROPERTIES:
        return bipi_get_image_properties(hBipI, (bt_bip_get_img_prop_req*)req);

    case BT_BIP_OP_GET_IMG:
        return bipi_get_image(hBipI, (bt_bip_get_img_req*)req);

    case BT_BIP_OP_GET_LINKED_THUMBNAIL:
        return bipi_get_linked_thumbnail(hBipI, (bt_bip_get_linked_thum_req*)req);

    case BT_BIP_OP_PUT_IMG:
        return bipi_put_image(hBipI, (bt_bip_put_img_req*)req);

    case BT_BIP_OP_PUT_LINKED_THUMBNAIL:
        return bipi_put_linked_thumbnail(hBipI, (bt_bip_put_linked_thum_req*)req);

    case BT_BIP_OP_ABORT:
        return bipi_abort(hBipI);

    case BT_BIP_OP_DISCONNECT:
        return bipi_disconnect(hBipI, *(U32*)req);

    case BT_BIP_OP_OBEX_AUTH_RSP:
        return bipi_obex_auth_rsp(hBipI, (bt_bip_obex_auth_rsp*)req);

    case BT_BIP_OP_CONTINUE:
        return bipi_continue(hBipI, *(U32*)req);
    }
    return 0;
}

#endif /* __BT_BIPI_PROFILE__ */
