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
 * bt_bipr_hdl.c
 *
 * Project:
 * --------
 *   Export function from BIP Server
 *
 * Description:
 * ------------
 *   This file is for MMI BIP Server.
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
 *
 * 09 03 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integration BT solution.
 *
 * 09 01 2010 sh.lai
 * NULL
 * Integration change into 1036OF.
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_BIPR_PROFILE__

#include "bt_types.h"
#include "bt_message.h"
#include "bt_mmi.h"
#include "bt_mmi_msg.h"
#include "ext_osal.h"
#include "bluetooth_sdp_struct.h"
#include "bluetooth_struct.h"
#include "bluetooth_bipr_struct.h"
#include "bluetooth_bipi_struct.h"
#include "bt_bipi_api.h"
#include "bt_bip_comm.h"
#include "bt_bipr_struct.h"
#include "bt_bipi_struct.h"

#include "bt_bip_internal.h"

static bt_bipr_context g_bipr_context[BIPR_MAX_INSTANCE];

#ifdef __BT_BIP_CMD_AGENT__
	#include "bt_bipr_comm.c"
#endif

static void bipr_send_deactivate_req(bt_bipr_context* cntx);

/*
 * instance access function start
 */
static bt_bipr_context* bipr_find_handle_by_req_id(U8 req_id)
{
    U32 i;

    for (i = 0; i < BIPR_MAX_INSTANCE; i++)
    {
        if (g_bipr_context[i].status != BIPR_STATUS_IDLE &&
            g_bipr_context[i].req_id == req_id)
        {
            return &g_bipr_context[i];
        }
    }
    /* add trace */
    // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPR_EXT_ADP_DO_NOT_FIND_HANDLE_BY_REQ_ID, req_id);
    return NULL;
}

static bt_bipr_context* bipr_find_handle_by_cn_id(U32 connect_id)
{
    U32 i;

    for (i = 0; i < BIPR_MAX_INSTANCE; i++)
    {
        if (g_bipr_context[i].status != BIPR_STATUS_IDLE &&
            g_bipr_context[i].connect_id == connect_id)
        {
            return &g_bipr_context[i];
        }
    }
    /* add trace */
    // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPR_EXT_ADP_DO_NOT_FIND_HANDLE_BY_CN_ID, connect_id);
    return NULL;
}

static bt_bipr_context* bipr_find_handle_by_status(U32 status)
{
    U32 i;

    for (i = 0; i < BIPR_MAX_INSTANCE; i++)
    {
        if (g_bipr_context[i].status == status)
        {
            return &g_bipr_context[i];
        }
    }
    // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPR_EXT_ADP_DO_NOT_FIND_HANDLE_BY_STATUS, status);
    return NULL;
}
/*
 * instance access function end
 */

static void bipr_handle_activate_cnf(bt_bip_activate_cnf_struct* cnf)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;
    bt_sdpdb_register_req_struct* req;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    cntx = bipr_find_handle_by_req_id(cnf->req_id);
    if (!cntx)
        return;

    if (cntx->status != BIPR_STATUS_ACTIVATING &&
        cntx->status != BIPR_STATUS_DEACTIVATING)
    {
        /* add trace current status*/
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPR_EXT_ADP_STATE_ERR, cntx->status, cntx->opcode);
        return;
    }

    switch (cntx->status)
    {
    case BIPR_STATUS_ACTIVATING:
        BIP_ASSERT(cntx->opcode == BT_BIP_OP_ACTIVATE);

        if (cnf->cnf_code == BT_BIP_CNF_SUCCESS)
        {
            btmtk_bipr_status_trans(cntx, BIPR_STATUS_REGISTERING);

            req = (bt_sdpdb_register_req_struct*)
                btmtk_bip_alloc_local_para(sizeof(bt_sdpdb_register_req_struct));
            req->uuid = BIPR_SDP_UUID;
           // btmtk_bipr_send_request(MSG_ID_BT_SDPDB_REGISTER_REQ, req, sizeof(bt_sdpdb_register_req_struct));
            btmtk_bipr_send_request(MSG_ID_BT_BIPR_REGISTER_REQ, req, sizeof(bt_sdpdb_register_req_struct));
        }
        else
        {
            btmtk_bipr_status_trans(cntx, BIPR_STATUS_IDLE);

            para.data.result = BT_BIP_ERR_FAILED;
            para.opcode = BT_BIP_OP_ACTIVATE;
            para.status = BT_BIPR_OPS_CONFIRMING;
            btmtk_bipr_notify_app(cntx, &para, 1);
        }
        break;

    case BIPR_STATUS_DEACTIVATING:
        if (cnf->cnf_code)
        {
            /* do not change status */
            bipr_send_deactivate_req(cntx);
        }
        else
        {
            para.data.result = BT_BIP_ERR_OK;
            para.opcode = BT_BIP_OP_DEACTIVATE;
            para.status = BT_BIPR_OPS_CONFIRMING;
            btmtk_bipr_notify_app(cntx, &para, 1);
            btmtk_bipr_free_handle(cntx);
        }
        break;
    }
}

void btmtk_bipr_send_deregister_req(bt_bipr_context* cntx)
{
    bt_sdpdb_deregister_req_struct *req;

    req = (bt_sdpdb_deregister_req_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_sdpdb_deregister_req_struct));
    req->uuid = BIPR_SDP_UUID;
//20100812 mtk03036
   // btmtk_bipr_send_request(
   //     MSG_ID_BT_SDPDB_DEREGISTER_REQ,
   //     req,
   //     sizeof(bt_sdpdb_deregister_req_struct));
            btmtk_bipr_send_request(MSG_ID_BT_BIPR_DEREGISTER_REQ, req, sizeof(bt_sdpdb_deregister_req_struct));
//20100812 mtk03036 end 
}

static void bipr_send_deactivate_req(bt_bipr_context* cntx)
{
    bt_bip_deactivate_req_struct* req;

    req = (bt_bip_deactivate_req_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_bip_deactivate_req_struct));
    req->bip_service_set = (U8)cntx->init.service;
    req->req_id = cntx->req_id;
    btmtk_bipr_send_request(
        MSG_ID_BT_BIP_DEACTIVATE_REQ,
        req,
        sizeof(bt_bip_deactivate_req_struct));
}

static void bipr_handle_sdp_register_cnf(bt_sdpdb_register_cnf_struct* cnf)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    /* there is no other information except : uuid and result
     * so we can not get instance handle by req_id or cm_id
     * for now, we just only support one instance,
     *          here we find a REGISTERING status handle
     */

    cntx = bipr_find_handle_by_status(BIPR_STATUS_REGISTERING);
    if (cntx) {

        BIP_ASSERT(cntx->opcode == BT_BIP_OP_ACTIVATE);

        if (cnf->result == 0)
        {
            btmtk_bipr_status_trans(cntx, BIPR_STATUS_LISTENING);

            para.data.result = BT_BIP_ERR_OK;
            para.opcode = BT_BIP_OP_ACTIVATE;
            para.status = BT_BIPR_OPS_CONFIRMING;
            btmtk_bipr_notify_app(cntx, &para, 1);
        }
        else
        {
            // start to deactivate server
            btmtk_bipr_status_trans(cntx, BIPR_STATUS_REG_DEACTIVATING);

            bipr_send_deactivate_req(cntx);
        }
        return;
    }

    cntx = bipr_find_handle_by_status(BIPR_STATUS_DEREGISTERING);
    if (cntx) {
        if (cnf->result == 0)
        {
            // do not change status
            btmtk_bipr_send_deregister_req(cntx);
        }
        else
        {
            btmtk_bipr_status_trans(cntx, BIPR_STATUS_DEACTIVATING);
            bipr_send_deactivate_req(cntx);
        }
    }

    return;

/*
    cntx = bipr_find_handle_by_status(BIPR_STATUS_REGISTERING);
    if (!cntx)
        return;

    if (cntx->status != BIPR_STATUS_REGISTERING &&
        cntx->status != BIPR_STATUS_DEREGISTERING)
    {

        // add trace current status
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPR_EXT_ADP_STATE_ERR, cntx->status, cntx->opcode);
        return;
    }

    switch (cntx->status)
    {
    case BIPR_STATUS_REGISTERING:

        BIP_ASSERT(cntx->opcode == BT_BIP_OP_ACTIVATE);

        if (cnf->result == 0)
        {
            btmtk_bipr_status_trans(cntx, BIPR_STATUS_LISTENING);

            para.data.result = BT_BIP_ERR_OK;
            para.opcode = BT_BIP_OP_ACTIVATE;
            para.status = BT_BIPR_OPS_CONFIRMING;
            btmtk_bipr_notify_app(cntx, &para, 1);
        }
        else
        {
            // start to deactivate server 
            btmtk_bipr_status_trans(cntx, BIPR_STATUS_REG_DEACTIVATING);

            bipr_send_deactivate_req(cntx);
        }
        break;

    case BIPR_STATUS_DEREGISTERING:

        if (cnf->result == 0)
        {
            // do not change status 
            btmtk_bipr_send_deregister_req(cntx);
        }
        else
        {
            btmtk_bipr_status_trans(cntx, BIPR_STATUS_DEACTIVATING);
            bipr_send_deactivate_req(cntx);
        }
        break;
    }
*/

}

static void bipr_handle_sdp_deregister_cnf(bt_sdpdb_deregister_cnf_struct* cnf)
{
    bt_bipr_context* cntx;

    /* there is no other information except : uuid and result
     * so we can not get instance handle by req_id or cm_id
     * for now, we just only support one instance,
     *          here we find a REGISTERING status handle
     */
    cntx = bipr_find_handle_by_status(BIPR_STATUS_DEREGISTERING);
    if (!cntx)
        return;

    if (cntx->status != BIPR_STATUS_DEREGISTERING)
    {
        /* add trace current status*/
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPR_EXT_ADP_STATE_ERR, cntx->status, cntx->opcode);
        return;
    }

    /* in fact, we should deactivate anyway */
    /* if (rsp->cnf_code == BT_BIP_CNF_SUCCESS) */
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_DEACTIVATING);

        bipr_send_deactivate_req(cntx);
    }

    if (cnf->result != BT_BIP_CNF_SUCCESS)
    {
        /* add trace */
    }
}

static void bipr_handle_deactivate_cnf(bt_bip_deactivate_cnf_struct* cnf)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    cntx = bipr_find_handle_by_req_id(cnf->req_id);
    if (!cntx)
        return;

    if (cntx->status != BIPR_STATUS_DEACTIVATING && /* deactive req case */
        cntx->status != BIPR_STATUS_REG_DEACTIVATING) /* active req, SDP failure case */
    {
        /* add trace current status*/
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPR_EXT_ADP_STATE_ERR, cntx->status, cntx->opcode);
        return;
    }

    /* in fact, we should deactivate anyway */
    if (cntx->status == BIPR_STATUS_DEACTIVATING)
    {
        para.opcode = BT_BIP_OP_DEACTIVATE;
        para.data.result = BT_BIP_ERR_OK;
    }
    else
    {
        para.opcode = BT_BIP_OP_DEACTIVATE;
        para.data.result = BT_BIP_ERR_FAILED;
    }
    para.status = BT_BIPR_OPS_CONFIRMING;
    btmtk_bipr_notify_app(cntx, &para, 1);

    btmtk_bipr_free_handle(cntx);


    if (cnf->cnf_code != BT_BIP_CNF_SUCCESS)
    {
        /* add trace */
    }

}

static void bipr_handle_authorize_ind(bt_bip_authorize_ind_struct* ind)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    cntx = bipr_find_handle_by_status(BIPR_STATUS_LISTENING);
    if (!cntx)
        return;

    btmtk_bipr_status_trans(cntx, BIPR_STATUS_AUTHORIZING);
    cntx->opcode = BT_BIP_OP_AUTHORIZE;

    para.opcode = BT_BIP_OP_AUTHORIZE;
    para.status = BT_BIPR_OPS_INDICATING;
    para.data.auth_ind.addr.lap = ind->bd_addr.lap;
    para.data.auth_ind.addr.nap = ind->bd_addr.nap;
    para.data.auth_ind.addr.uap = ind->bd_addr.uap;
    strcpy((char*)para.data.auth_ind.dev_name, (const char*)ind->dev_name);
    btmtk_bipr_notify_app(cntx, &para, 1);
}

static void bipr_handle_connect_ind(bt_bip_connect_ind_struct* ind)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    cntx = bipr_find_handle_by_status(BIPR_STATUS_AUTHORIZED);
    if (!cntx)
        return;

    btmtk_bipr_status_trans(cntx, BIPR_STATUS_ACCEPTING);
    cntx->connect_id = ind->cm_conn_id;
    cntx->opcode = BT_BIP_OP_CONNECT;

    para.opcode = BT_BIP_OP_CONNECT;
    para.status = BT_BIPR_OPS_INDICATING;
    para.data.auth_ind.addr.lap = ind->bd_addr.lap;
    para.data.auth_ind.addr.nap = ind->bd_addr.nap;
    para.data.auth_ind.addr.uap = ind->bd_addr.uap;
    strcpy((char*)para.data.conn_ind.dev_name, (const char*)ind->dev_name);
    btmtk_bipr_notify_app(cntx, &para, 1);
}

static void bipr_handle_get_capa_ind(bt_bip_get_capabilities_ind_struct* ind)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    cntx = bipr_find_handle_by_cn_id(ind->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPR_STATUS_ACCEPTED)
        return;

    btmtk_bipr_status_trans(cntx, BIPR_STATUS_RESPONDING);
    cntx->opcode = BT_BIP_OP_GET_CAPABILITIES;

    para.opcode = BT_BIP_OP_GET_CAPABILITIES;
    para.status = BT_BIPR_OPS_INDICATING;
    btmtk_bipr_notify_app(cntx, &para, 1);
}

static void bipr_handle_put_img_ind(bt_bip_put_img_ind_struct* ind)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    cntx = bipr_find_handle_by_cn_id(ind->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPR_STATUS_ACCEPTED)
        return;

    btmtk_bipr_status_trans(cntx, BIPR_STATUS_RESPONDING);
    cntx->opcode = BT_BIP_OP_PUT_IMG;

    para.opcode = BT_BIP_OP_PUT_IMG;
    para.status = BT_BIPR_OPS_INDICATING;
    btmtk_wstrcpy(para.data.put_img.img_name, (U16*)ind->img_name);
    memcpy(&para.data.put_img.img_desc, &ind->img_descriptor, sizeof(bt_bip_img_desc));
    btmtk_bipr_notify_app(cntx, &para, 1);
}

static void bipr_handle_put_linked_thum_ind(bt_bip_put_linked_thumbnail_ind_struct* ind)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    cntx = bipr_find_handle_by_cn_id(ind->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPR_STATUS_ACCEPTED)
        return;

    btmtk_bipr_status_trans(cntx, BIPR_STATUS_RESPONDING);
    cntx->opcode = BT_BIP_OP_PUT_LINKED_THUMBNAIL;

    para.opcode = BT_BIP_OP_PUT_LINKED_THUMBNAIL;
    para.status = BT_BIPR_OPS_INDICATING;
    strcpy((char*)para.data.put_thum.handle, (char*)ind->img_handle);
    btmtk_bipr_notify_app(cntx, &para, 1);
}

static void bipr_handle_get_img_list_ind(bt_bip_get_img_list_ind_struct* ind)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));



    cntx = bipr_find_handle_by_cn_id(ind->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPR_STATUS_ACCEPTED)
        return;

    btmtk_bipr_status_trans(cntx, BIPR_STATUS_RESPONDING);
    cntx->opcode = BT_BIP_OP_GET_IMG_LIST;

    para.opcode = BT_BIP_OP_GET_IMG_LIST;
    para.status = BT_BIPR_OPS_INDICATING;
    /* para.data.img_list.handle_desc.created */
    para.data.img_list.latest_captured = ind->latest_captured;
    para.data.img_list.max_img_number = ind->max_img_handle_number;
    para.data.img_list.start_index = ind->start_index;
    btmtk_bipr_notify_app(cntx, &para, 1);
}

static void bipr_handle_get_img_prop_ind(bt_bip_get_img_prop_ind_struct* ind)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    cntx = bipr_find_handle_by_cn_id(ind->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPR_STATUS_ACCEPTED)
        return;

    btmtk_bipr_status_trans(cntx, BIPR_STATUS_RESPONDING);
    cntx->opcode = BT_BIP_OP_GET_IMG_PROPERTIES;

    para.opcode = BT_BIP_OP_GET_IMG_PROPERTIES;
    para.status = BT_BIPR_OPS_INDICATING;
    strcpy((char*)para.data.img_prop.handle, (const char*)ind->img_handle);
    btmtk_bipr_notify_app(cntx, &para, 1);
}

static void bipr_handle_get_img_ind(bt_bip_get_img_ind_struct* ind)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    cntx = bipr_find_handle_by_cn_id(ind->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPR_STATUS_ACCEPTED)
        return;

    btmtk_bipr_status_trans(cntx, BIPR_STATUS_RESPONDING);
    cntx->opcode = BT_BIP_OP_GET_IMG;

    para.opcode = BT_BIP_OP_GET_IMG;
    para.status = BT_BIPR_OPS_INDICATING;
    strcpy((char*)para.data.img_prop.handle, (const char*)ind->img_handle);
    /* para.data.get_img.img_desc */
    btmtk_bipr_notify_app(cntx, &para, 1);
}

static void bipr_handle_get_linked_thum_ind(bt_bip_get_linked_thumbnail_ind_struct* ind)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));



    cntx = bipr_find_handle_by_cn_id(ind->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPR_STATUS_ACCEPTED)
        return;

    btmtk_bipr_status_trans(cntx, BIPR_STATUS_RESPONDING);
    cntx->opcode = BT_BIP_OP_GET_LINKED_THUMBNAIL;

    para.opcode = BT_BIP_OP_GET_LINKED_THUMBNAIL;
    para.status = BT_BIPR_OPS_INDICATING;
    strcpy((char*)para.data.get_thum.handle, (const char*)ind->img_handle);
    btmtk_bipr_notify_app(cntx, &para, 1);
}

static U8 bipr_handle_continue_ind(bt_bip_continue_ind_struct* ind)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;
    bt_bip_continue_rsp_struct* irsp;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    cntx = bipr_find_handle_by_cn_id(ind->cm_conn_id);
    if (!cntx)
        return 0;

    if (cntx->status != BIPR_STATUS_CONTINUE &&
        cntx->status != BIPR_STATUS_ABORTING)
        return 1;

    if (cntx->status == BIPR_STATUS_CONTINUE)
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_INDICATING);

        para.opcode = cntx->opcode;
        para.status = BT_BIPR_OPS_RESPONDING;
        para.data.cnt_ind.obj_len = ind->obj_len;
        para.data.cnt_ind.data_len = ind->data_len;
        btmtk_bipr_notify_app(cntx, &para, 1);
    }
    else
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_ABORTING);

        irsp = (bt_bip_continue_rsp_struct*)
            btmtk_bip_alloc_local_para(sizeof(bt_bip_continue_rsp_struct));
        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = BT_BIP_CNF_FAILED;
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_CONTINUE_RSP,
            irsp,
            sizeof(bt_bip_continue_rsp_struct));
    }
    return 1;
}

static void bipr_handle_abort_ind(bt_bip_abort_ind_struct* ind)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    cntx = bipr_find_handle_by_cn_id(ind->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPR_STATUS_CONTINUE &&
        cntx->status != BIPR_STATUS_RESPONDING &&
        cntx->status != BIPR_STATUS_INDICATING &&
        cntx->status != BIPR_STATUS_ABORTING)
        return;

    if (cntx->file_name[0])
        ext_fs_delete_ucs2(cntx->file_name);

    para.opcode = cntx->opcode; /* BT_BIP_OP_ABORT */;
    para.status = BT_BIPR_OPS_COMPLETE; /* cntx->status == BIPR_STATUS_ABORTING ? BT_BIPR_OPS_CONFIRMING : BT_BIPR_OPS_INDICATING; */
    para.data.result = cntx->status == BIPR_STATUS_ABORTING ? BT_BIP_ERR_USER_ABORT : ind->ind_code; /* maybe this error code should be transform */
    btmtk_bipr_notify_app(cntx, &para, 1);

    cntx->opcode = BT_BIP_OP_NONE; /* clear opcode */
    cntx->file_name[0] = 0;
    if(cntx->status != BIPR_STATUS_DEAC_DISCONNECTING)
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_ACCEPTED);
}

static void bipr_handle_complete_ind(bt_bip_complete_ind_struct* ind)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    cntx = bipr_find_handle_by_cn_id(ind->cm_conn_id);
    if (!cntx)
        return;

    /* in case of aborting status, we do not callback abort failure to app.
     * instead of callback successful current operation.
    */
    if (cntx->status != BIPR_STATUS_CONTINUE &&
        cntx->status != BIPR_STATUS_RESPONDING &&
        cntx->status != BIPR_STATUS_INDICATING &&
        cntx->status != BIPR_STATUS_ABORTING &&
        cntx->status != BIPR_STATUS_ACCEPTING)
        return;

    btmtk_bipr_status_trans(cntx, BIPR_STATUS_ACCEPTED);

    para.opcode = cntx->opcode;
    para.status = BT_BIPR_OPS_COMPLETE;
    btmtk_bipr_notify_app(cntx, &para, 1);
}

static U8 bipr_handle_disconnect_ind(bt_bip_disconnect_ind_struct* ind)
{
    bt_bipr_context* cntx;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    if (ind->session_role == BT_BIP_INITIATOR_ROLE)
    {
        return 0;
    }

    cntx = bipr_find_handle_by_cn_id(ind->cm_conn_id);
    if (!cntx)
        return 0;
    /* here we try our best to reset status
     * since this event means no other event can we received from this device...
     */
    if (cntx->status <= BIPR_STATUS_LISTENING)
        return 1;

    cntx->connect_id = (U32)-1;

    switch (cntx->status)
    {
    case BIPR_STATUS_DISCONNECTING:
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_LISTENING);

        para.opcode = cntx->opcode;
        para.status = BT_BIPR_OPS_CONFIRMING;
        para.data.result = BT_BIP_ERR_OK;
        btmtk_bipr_notify_app(cntx, &para, 1);
        break;

    case BIPR_STATUS_DEAC_DISCONNECTING:
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_DEREGISTERING);
        btmtk_bipr_send_deregister_req(cntx);
        break;

    default:
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_LISTENING);

        para.opcode = BT_BIP_OP_DISCONNECT;
        para.status = BT_BIPR_OPS_INDICATING;
        para.data.result = BT_BIP_ERR_OK;
        btmtk_bipr_notify_app(cntx, &para, 1);
    }
    return 1;
}

static void bipr_handle_obex_auth_ind(bt_bip_obex_auth_ind_struct* ind)
{
    bt_bipr_context* cntx = NULL;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    cntx = bipr_find_handle_by_status(BIPR_STATUS_AUTHORIZED);
    if (!cntx)
    {
        /* add trace */
        cntx = bipr_find_handle_by_cn_id(ind->cm_conn_id);
        if (cntx)
        {
            // TODO log kal_trace(BT_TRACE_G6_OBEX, BIP_EXT_ADP_DO_NOT_SUPPORT_NOW, __LINE__);
        }
        /* connected, now we do not support this case */
        return;
    }
    btmtk_bipr_status_trans(cntx, BIPR_STATUS_CN_BE_CHAL);

    para.data.obauth_ind.option = ind->option;
    para.data.obauth_ind.realm_len = ind->realm_len;
    if (ind->realm_len)
        memcpy(para.data.obauth_ind.realm, ind->realm, ind->realm_len);
    para.status = BT_BIPR_OPS_INDICATING;
    para.opcode = BT_BIP_OP_OBEX_AUTH;
    btmtk_bipr_notify_app(cntx, &para, 1);
}

static void bipr_handle_obex_auth_cnf(bt_bip_obex_auth_cnf_struct* cnf)
{
    bt_bipr_context* cntx = NULL;
    bt_bipr_para para;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    /* now must have receive CONNECT_IND, then can get valid cm_conn_id already */
    cntx = bipr_find_handle_by_cn_id(cnf->cm_conn_id);

    if (!cntx) /* connect case */
    {
        /* add trace */
        return;
    }
    /* here just tell APP client has be authenticated, do not status change */
    para.data.result = cnf->cnf_code == BT_BIP_CNF_SUCCESS ? BT_BIP_ERR_OK : BT_BIP_ERR_FAILED;
    para.status = BT_BIPR_OPS_CONFIRMING;
    para.opcode = BT_BIP_OP_OBEX_AUTH;
    btmtk_bipr_notify_app(cntx, &para, 1);
}
//////////////////////////////////////////////////////////////////////////////
// external funcation
//////////////////////////////////////////////////////////////////////////////
/*
 * BIPR instance management start
*/
bt_bipr_context* btmtk_bipr_alloc_handle(void)
{
    U32 i;

    for (i = 0; i < BIPR_MAX_INSTANCE; i++)
    {
        if (g_bipr_context[i].status == BIPR_STATUS_IDLE)
        {
            /* g_bipr_context[i].status = BIPR_STATUS_ACTIVATED;
             * we think invoker will set @status
            */

            g_bipr_context[i].req_id = (U8)(i + 1);
            g_bipr_context[i].connect_id = (U32)-1; /* invalid id */
            return &g_bipr_context[i];
        }
    }
    return NULL;
}

U8 btmtk_bipr_verify_handle(HMTKBIPR hBipR)
{
    U32 i;

    for (i = 0; i < BIPR_MAX_INSTANCE; i++)
    {
        if (g_bipr_context[i].status != BIPR_STATUS_IDLE &&
            (HMTKBIPR)&g_bipr_context[i]== hBipR)
            return 1;
    }
    return 0;
}

void btmtk_bipr_free_handle(bt_bipr_context* cntx)
{
    memset(cntx, 0, sizeof(bt_bipr_context));
}

void btmtk_bipr_status_trans(bt_bipr_context* cntx, U32 status)
{
    cntx->status = status;
}

/*
 * BIPR instance management end
*/

U8 btmtk_bipr_handle_message(ilm_struct *message)
{

//mtk03036
#ifdef BTMTK_ON_LINUX
    void* para = (void*)message->ilm_data;
#else
    void* para = (void*)message->local_para_ptr;
#endif
//    LOGD("para: %p", para);
    if (!para)
      return 0;
//mtk03036 end




    // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPR_EXT_ADP_RECEIVE_MSG, message->msg_id);

    switch(message->msg_id)
    {
#ifdef __BT_BIP_CMD_AGENT__
    case MSG_ID_BT_BIPR_CMD:
        bipr_handle_cmd(((bt_bip_cmd_agent_struct*)para)->handle,
            ((bt_bip_cmd_agent_struct*)para)->opcode,
            BT_BIP_GET_CMD_PTR(para));
        return 1;
#endif
    case MSG_ID_BT_BIP_ACTIVATE_CNF:
        bipr_handle_activate_cnf((bt_bip_activate_cnf_struct*)para);
        return 1;

    case MSG_ID_BT_SDPDB_REGISTER_CNF:
        bipr_handle_sdp_register_cnf((bt_sdpdb_register_cnf_struct*)para);
        return 1;

    case MSG_ID_BT_SDPDB_DEREGISTER_CNF:
        bipr_handle_sdp_deregister_cnf((bt_sdpdb_deregister_cnf_struct*)para);
        return 1;

    case MSG_ID_BT_BIP_DEACTIVATE_CNF:
        bipr_handle_deactivate_cnf((bt_bip_deactivate_cnf_struct*)para);
        return 1;

//20100812 mtk03036
    case MSG_ID_BT_BIPR_REGISTER_CNF:
        bipr_handle_sdp_register_cnf((bt_sdpdb_register_cnf_struct*)para);
        return 1;

    case MSG_ID_BT_BIPR_DEREGISTER_CNF:
        bipr_handle_sdp_deregister_cnf((bt_sdpdb_deregister_cnf_struct*)para);
        return 1;
//20100812 mtk03036

    case MSG_ID_BT_BIP_AUTHORIZE_IND:
        bipr_handle_authorize_ind((bt_bip_authorize_ind_struct*)para);
        return 1;

    case MSG_ID_BT_BIP_CONNECT_IND:
        bipr_handle_connect_ind((bt_bip_connect_ind_struct*)para);
        return 1;

    case MSG_ID_BT_BIP_GET_CAPABILITIES_IND:
        bipr_handle_get_capa_ind((bt_bip_get_capabilities_ind_struct*)para);
        return 1;

    case MSG_ID_BT_BIP_PUT_IMG_IND:
        bipr_handle_put_img_ind((bt_bip_put_img_ind_struct*)para);
        return 1;

    case MSG_ID_BT_BIP_PUT_LINKED_THUMBNAIL_IND:
        bipr_handle_put_linked_thum_ind((bt_bip_put_linked_thumbnail_ind_struct*)para);
        return 1;

    case MSG_ID_BT_BIP_GET_IMG_LIST_IND:
        bipr_handle_get_img_list_ind((bt_bip_get_img_list_ind_struct*)para);
        return 1;

    case MSG_ID_BT_BIP_GET_IMG_PROP_IND:
        bipr_handle_get_img_prop_ind((bt_bip_get_img_prop_ind_struct*)para);
        return 1;

    case MSG_ID_BT_BIP_GET_IMG_IND:
        bipr_handle_get_img_ind((bt_bip_get_img_ind_struct*)para);
        return 1;

    case MSG_ID_BT_BIP_GET_LINKED_THUMBNAIL_IND:
        bipr_handle_get_linked_thum_ind((bt_bip_get_linked_thumbnail_ind_struct*)para);
        return 1;

    case MSG_ID_BT_BIP_GET_MONITORING_IMG_IND:
        return 1;

    case MSG_ID_BT_BIP_CONTINUE_IND:
        return bipr_handle_continue_ind((bt_bip_continue_ind_struct*)para);

    case MSG_ID_BT_BIP_ABORT_IND:
        bipr_handle_abort_ind((bt_bip_abort_ind_struct*)para);
        return 1;

    case MSG_ID_BT_BIP_COMPLETE_IND:
        bipr_handle_complete_ind((bt_bip_complete_ind_struct*)para);
        return 1;

    case MSG_ID_BT_BIP_DISCONNECT_IND:
        return bipr_handle_disconnect_ind((bt_bip_disconnect_ind_struct*)para);

    case MSG_ID_BT_BIPR_OBEX_AUTH_IND:
        bipr_handle_obex_auth_ind((bt_bip_obex_auth_ind_struct*)para);
        return 1;

    case MSG_ID_BT_BIPR_OBEX_AUTH_CNF:
        bipr_handle_obex_auth_cnf((bt_bip_obex_auth_cnf_struct*)para);
        return 1;
        //   MSG_ID_BT_BIP_ENTER_USB_MODE_REQ,
        //   MSG_ID_BT_BIP_ENTER_USB_MODE_CNF,

    }
    return 0;
}
#endif /* __BT_BIPR_PROFILE__ */
