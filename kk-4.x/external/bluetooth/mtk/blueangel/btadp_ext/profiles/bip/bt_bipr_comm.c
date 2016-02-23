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
 * bt_bipr_comm.c
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

static S32 bipr_handle_cmd(HMTKBIPR hBipR, U32 opcode, void* rsp);

/*
 * utility function start
*/
static U8 bipr_check_handle(HMTKBIPR hBipR)
{
    if (!btmtk_bipr_verify_handle(hBipR))
    {
#ifdef __BT_BIP_CMD_AGENT__
        BIP_PROCESS_DEAD_ASSERT(BT_BIP_ERR_INVALID_PARA);
#endif
        return 0;
    }
    return 1;
}
void btmtk_bipr_notify_app(bt_bipr_context* contxt, bt_bipr_para* para, U8 force)
{
    // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPR_EXT_ADP_NOTIFY_APP, para->opcode, para->status);

    if (force)
    {
        para->handle = (HMTKBIPR)contxt;
        contxt->init.callback(contxt->init.para, para);
    }
    /* agent case, invoke callback anyway */
#ifdef __BT_BIP_CMD_AGENT__
    else
    {
        para->handle = (HMTKBIPR)contxt;
        contxt->init.callback(contxt->init.para, para);
    }
#endif
}

/* send request to BT */
void btmtk_bipr_send_request(msg_type msg_id, void* para, U32 size)
{
    BTCMD_SendMessage(msg_id, MOD_BT, para, (U16)size);
    if (para)
//mtk03036
//        free_local_para(para);
        FREE_LOCAL_PARA(para);
}

/*
 * utility function end
*/

/*
 * API function implement
*/
static S32 bipr_activate(bt_bipr_active_info* rsp)
{
    bt_bipr_context* handle;
    bt_bipr_para para;
    bt_bip_activate_req_struct *req;

    memset((void*)&para, 0, sizeof(bt_bipr_para));


    handle = btmtk_bipr_alloc_handle();
    if (!handle)
    {
        /* no resource */
        para.opcode = BT_BIP_OP_ACTIVATE;
        para.status = BT_BIPR_OPS_CONFIRMING;
        para.data.result = BT_BIP_ERR_NO_RESOURCE;
        btmtk_bipr_notify_app(handle, &para, 0);
        return BT_BIP_ERR_NO_RESOURCE;
    }
    else
    {
        req = (bt_bip_activate_req_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_activate_req_struct));
        req->req_id = handle->req_id;
        req->bip_service_set = (U8)rsp->service;

        handle->init = *rsp;
        handle->opcode = BT_BIP_OP_ACTIVATE;
        btmtk_bipr_status_trans(handle, BIPR_STATUS_ACTIVATING);
        btmtk_bipr_send_request(MSG_ID_BT_BIP_ACTIVATE_REQ, req, sizeof(bt_bip_activate_req_struct));
    }
    return BT_BIP_ERR_PROCESSING;
}

static S32 bipr_authorized_rsp(HMTKBIPR hBipR, U32 option)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_authorize_rsp_struct* rsp;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPR_STATUS_AUTHORIZING)
    {
        para.opcode = BT_BIP_OP_AUTHORIZE;
        para.status = BT_BIPR_OPS_RESPONDING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    rsp = (bt_bip_authorize_rsp_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_authorize_rsp_struct));

    if (option)
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_AUTHORIZED);
        rsp->cnf_code = BT_BIP_CNF_SUCCESS;
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_AUTHORIZE_RSP,
            rsp,
            sizeof(bt_bip_authorize_rsp_struct));
        return BT_BIP_ERR_PROCESSING;
    }
    else
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_LISTENING);
        rsp->cnf_code = BT_BIP_CNF_FAILED;
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_AUTHORIZE_RSP,
            rsp,
            sizeof(bt_bip_authorize_rsp_struct));
        return BT_BIP_ERR_OK;
    }
}
static S32 bipr_connect_rsp(HMTKBIPR hBipR, U32 option)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_connect_rsp_struct* rsp;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPR_STATUS_ACCEPTING)
    {
        para.opcode = BT_BIP_OP_CONNECT;
        para.status = BT_BIPR_OPS_RESPONDING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    rsp = (bt_bip_connect_rsp_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_connect_rsp_struct));

    if (option)
    {
        /* here we do not change status
         * we think COMPLETE_IND will follow closely!!!
        */
        rsp->cnf_code = BT_BIP_CNF_SUCCESS;
        btmtk_bipr_send_request(MSG_ID_BT_BIP_CONNECT_RSP, rsp, sizeof(bt_bip_connect_rsp_struct));
        return BT_BIP_ERR_PROCESSING;
    }
    else
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_AUTHORIZED);
        rsp->cnf_code = BT_BIP_CNF_FAILED;
        btmtk_bipr_send_request(MSG_ID_BT_BIP_CONNECT_RSP, rsp, sizeof(bt_bip_connect_rsp_struct));
        return BT_BIP_ERR_OK;
    }
}

static S32 bipr_get_capabilities(HMTKBIPR hBipR, bt_bip_capabilities_rsp* rsp)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_get_capabilities_rsp_struct* irsp;
    U32 i;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPR_STATUS_RESPONDING)
    {
        para.opcode = BT_BIP_OP_GET_CAPABILITIES;
        para.status = BT_BIPR_OPS_RESPONDING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    irsp = (bt_bip_get_capabilities_rsp_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_bip_get_capabilities_rsp_struct));

    if (rsp->result == BT_BIP_ERR_OK)
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_CONTINUE);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = BT_BIP_CNF_SUCCESS;
        irsp->created_time_filter = rsp->created_time_filter;
        irsp->encoding_filter = (bt_bip_img_format_enum)rsp->encoding_filter;
        irsp->modified_time_filter = rsp->modified_time_filter;
        irsp->pixel_filter = rsp->pixel_filter;

        irsp->preferred_format = (bt_bip_img_format_enum)rsp->preferred_format.encoding;
        irsp->preferred_maxsize = rsp->preferred_format.size;
        memcpy(&irsp->preferred_pixel, &rsp->preferred_format.pixel, sizeof(bt_bip_pixel));
        irsp->preferred_trans = rsp->preferred_format.transformation;

        for (i = 0; i < BT_BIP_MAX_IMG_FORMATS; i++)
        {
            if (rsp->supported_formats[i].encoding)
            {
                irsp->supported_img_formats[i] = (bt_bip_img_format_enum)rsp->supported_formats[i].encoding;
                memcpy(&irsp->supported_img_size[i], &rsp->supported_formats[i].pixel, sizeof(bt_bip_pixel));
                irsp->supported_maxsize[i] = rsp->supported_formats[i].size;
            }
            else
            {
                break;
            }
        }
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_GET_CAPABILITIES_RSP,
            irsp,
            sizeof(bt_bip_get_capabilities_rsp_struct));
        return BT_BIP_ERR_PROCESSING;
    }
    else
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_ABORTING);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = BT_BIP_CNF_FAILED;
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_GET_CAPABILITIES_RSP,
            irsp,
            sizeof(bt_bip_get_capabilities_rsp_struct));
        return BT_BIP_ERR_OK;
    }
}

static S32 bipr_get_images_list(HMTKBIPR hBipR, bt_bip_get_img_list_rsp* rsp)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_get_img_list_rsp_struct* irsp;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPR_STATUS_RESPONDING)
    {
        para.opcode = BT_BIP_OP_GET_IMG_LIST;
        para.status = BT_BIPR_OPS_RESPONDING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }
    irsp = (bt_bip_get_img_list_rsp_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_bip_get_img_list_rsp_struct));

    if (rsp->result == BT_BIP_ERR_OK)
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_CONTINUE);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = BT_BIP_CNF_SUCCESS;
        irsp->img_count = (U16)rsp->item_count;
        btmtk_wstrcpy(irsp->img_list_path, BIPR_IMG_LIST_FILE_NAME);
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_GET_IMG_LIST_RSP,
            irsp,
            sizeof(bt_bip_get_img_list_rsp_struct));
        return BT_BIP_ERR_PROCESSING;
    }
    else
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_ABORTING);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = GOEP_METHOD_NOT_ALLOWED; /* maybe error should be considered... */
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_GET_IMG_LIST_RSP,
            irsp,
            sizeof(bt_bip_get_img_list_rsp_struct));
        return BT_BIP_ERR_OK;
    }
}

static S32 bipr_get_image_properties(HMTKBIPR hBipR, bt_bip_get_img_prop_rsp* rsp)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_get_img_prop_rsp_struct* irsp;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPR_STATUS_RESPONDING)
    {
        para.opcode = BT_BIP_OP_GET_IMG_PROPERTIES;
        para.status = BT_BIPR_OPS_RESPONDING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }
    irsp = (bt_bip_get_img_prop_rsp_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_bip_get_img_prop_rsp_struct));

    if (rsp->result == BT_BIP_ERR_OK)
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_CONTINUE);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = BT_BIP_CNF_SUCCESS;
        strcpy((char*)irsp->img_handle, (const char*)rsp->handle);
        strcpy((char*)irsp->native_img.img_handle, (const char*)rsp->handle); /* now internal adp's struct is not so good... */
        btmtk_wstrcpy((U16*)irsp->native_img.friendly_name, rsp->friendly_name);

        irsp->native_img.encoding = (bt_bip_img_format_enum)rsp->native.encoding;
        irsp->native_img.size = rsp->native.size;
        irsp->native_img.specified_pixel_height = rsp->native.pixel.height;
        irsp->native_img.specified_pixel_width = rsp->native.pixel.width;

        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_GET_IMG_PROP_RSP,
            irsp,
            sizeof(bt_bip_get_img_prop_rsp_struct));
        return BT_BIP_ERR_PROCESSING;
    }
    else
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_ABORTING);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = GOEP_METHOD_NOT_ALLOWED; /* maybe error should be considered... */
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_GET_IMG_PROP_RSP,
            irsp,
            sizeof(bt_bip_get_img_prop_rsp_struct));
        return BT_BIP_ERR_OK;
    }
}

static S32 bipr_get_image(HMTKBIPR hBipR, bt_bip_get_img_rsp* rsp)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_get_img_rsp_struct* irsp;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPR_STATUS_RESPONDING)
    {
        para.opcode = BT_BIP_OP_GET_IMG;
        para.status = BT_BIPR_OPS_RESPONDING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    irsp = (bt_bip_get_img_rsp_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_bip_get_img_rsp_struct));

    if (rsp->result == BT_BIP_ERR_OK)
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_CONTINUE);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = BT_BIP_CNF_SUCCESS;
        strcpy((char*)irsp->img_handle, (const char*)rsp->handle);
        btmtk_wstrcpy(irsp->img_path, rsp->img_path);
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_GET_IMG_RSP,
            irsp,
            sizeof(bt_bip_get_img_rsp_struct));
        return BT_BIP_ERR_PROCESSING;
    }
    else
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_ABORTING);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = GOEP_METHOD_NOT_ALLOWED; /* maybe error should be considered... */
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_GET_IMG_RSP,
            irsp,
            sizeof(bt_bip_get_img_rsp_struct));
        return BT_BIP_ERR_OK;
    }
}

static S32 bipr_get_linked_thumbnail(HMTKBIPR hBipR, bt_bip_get_linked_thum_rsp* rsp)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_get_linked_thumbnail_rsp_struct* irsp;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPR_STATUS_RESPONDING)
    {
        para.opcode = BT_BIP_OP_GET_LINKED_THUMBNAIL;
        para.status = BT_BIPR_OPS_RESPONDING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    irsp = (bt_bip_get_linked_thumbnail_rsp_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_bip_get_linked_thumbnail_rsp_struct));

    if (rsp->result == BT_BIP_ERR_OK)
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_CONTINUE);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = BT_BIP_CNF_SUCCESS;
        btmtk_wstrcpy(irsp->img_path, rsp->img_path);
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_GET_LINKED_THUMBNAIL_RSP,
            irsp,
            sizeof(bt_bip_get_linked_thumbnail_rsp_struct));
        return BT_BIP_ERR_PROCESSING;
    }
    else
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_ABORTING);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = GOEP_METHOD_NOT_ALLOWED; /* maybe error should be considered... */
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_GET_LINKED_THUMBNAIL_RSP,
            irsp,
            sizeof(bt_bip_get_linked_thumbnail_rsp_struct));
        return BT_BIP_ERR_OK;
    }
}

static S32 bipr_put_image(HMTKBIPR hBipR, bt_bip_put_img_rsp* rsp)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_put_img_rsp_struct* irsp;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPR_STATUS_RESPONDING)
    {
        para.opcode = BT_BIP_OP_PUT_IMG;
        para.status = BT_BIPR_OPS_RESPONDING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    irsp = (bt_bip_put_img_rsp_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_bip_put_img_rsp_struct));

    if (rsp->result == BT_BIP_ERR_OK ||
        rsp->result == BT_BIP_ERR_PARTIAL_CONTENT)
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_CONTINUE);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = rsp->result == BT_BIP_ERR_OK ? BT_BIP_CNF_SUCCESS : BT_BIP_PARTIAL_CONTENT;
        strcpy((char*)irsp->img_handle, (const char*)rsp->handle);
        btmtk_wstrcpy(irsp->img_path, rsp->img_path);
        btmtk_wstrcpy(cntx->file_name, rsp->img_path);
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_PUT_IMG_RSP,
            irsp,
            sizeof(bt_bip_put_img_rsp_struct));
        return BT_BIP_ERR_PROCESSING;
    }
    else
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_ABORTING);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = BT_BIP_CNF_FAILED;
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_PUT_IMG_RSP,
            irsp,
            sizeof(bt_bip_put_img_rsp_struct));
        return BT_BIP_ERR_OK;
    }
}

static S32 bipr_put_linked_thumbnail(HMTKBIPR hBipR, bt_bip_put_linked_thum_rsp* rsp)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_put_linked_thumbnail_rsp_struct* irsp;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPR_STATUS_RESPONDING)
    {
        para.opcode = BT_BIP_OP_PUT_LINKED_THUMBNAIL;
        para.status = BT_BIPR_OPS_RESPONDING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    irsp = (bt_bip_put_linked_thumbnail_rsp_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_bip_put_linked_thumbnail_rsp_struct));

    if (rsp->result == BT_BIP_ERR_OK)
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_CONTINUE);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = BT_BIP_CNF_SUCCESS;
        btmtk_wstrcpy(irsp->img_path, rsp->img_path);
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_PUT_LINKED_THUMBNAIL_RSP,
            irsp,
            sizeof(bt_bip_put_linked_thumbnail_rsp_struct));
        return BT_BIP_ERR_PROCESSING;
    }
    else
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_ABORTING);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = BT_BIP_CNF_FAILED;
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_PUT_LINKED_THUMBNAIL_RSP,
            irsp,
            sizeof(bt_bip_put_linked_thumbnail_rsp_struct));
        return BT_BIP_ERR_OK;
    }
}
/* there is no abort operation in OBEX server in fact
 * here just utility for foreground server to cancel current operation...
 */
static S32 bipr_abort(HMTKBIPR hBipR)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    S32 result = BT_BIP_ERR_FAILED;
    bt_bip_continue_rsp_struct* irsp;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    switch (cntx->status)
    {
        /* just reject current request */
    case BIPR_STATUS_RESPONDING:
        /* maybe following code is so tricky */
        bipr_handle_cmd(hBipR, cntx->opcode, &result);
        return BT_BIP_ERR_OK;

    case BIPR_STATUS_INDICATING:
        /* TODO */
        irsp = (bt_bip_continue_rsp_struct*)
            btmtk_bip_alloc_local_para(sizeof(bt_bip_continue_rsp_struct));

        btmtk_bipr_status_trans(cntx, BIPR_STATUS_ABORTING);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = BT_BIP_CNF_FAILED;
        btmtk_bipr_send_request(
            MSG_ID_BT_BIP_CONTINUE_RSP,
            irsp,
            sizeof(bt_bip_continue_rsp_struct));
        return BT_BIP_ERR_PROCESSING;

    case BIPR_STATUS_CONTINUE:
        /* trans status and wait for next indication
         * since server can not send response on it's initiative
        */
        btmtk_bipr_status_trans(hBipR, BIPR_STATUS_ABORTING);
        return BT_BIP_ERR_PROCESSING;
/*
    case BIPR_STATUS_RSP_AUTH:
        btmtk_bipr_status_trans(hBipR, BIPR_STATUS_ACCEPTED);
        return BT_BIP_ERR_OK;
*/
    default:
        para.opcode = BT_BIP_OP_ABORT;
        para.status = BT_BIPR_OPS_REQUESTING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
    }

    return BT_BIP_ERR_STATE;
}

static S32 bipr_disconnect(HMTKBIPR hBipR, U32 force)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_disconnect_req_struct* rsp;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    switch (cntx->status)
    {
        /* following status is force disconnect only */
    case BIPR_STATUS_AUTHORIZED: /* ???? current RFcomm connect established ???? */
    case BIPR_STATUS_ACCEPTING:
    case BIPR_STATUS_CN_BE_CHAL:
    case BIPR_STATUS_RESPONDING:
    case BIPR_STATUS_CONTINUE:
    case BIPR_STATUS_INDICATING:
    case BIPR_STATUS_ABORTING:
        if (force)
        {
            btmtk_bipr_status_trans(hBipR, BIPR_STATUS_DISCONNECTING);
            cntx->opcode = BT_BIP_OP_DISCONNECT;

            rsp = (bt_bip_disconnect_req_struct*)
                btmtk_bip_alloc_local_para(sizeof(bt_bip_disconnect_req_struct));
            rsp->cm_conn_id = cntx->connect_id;
            rsp->disconnect_tp_directly = 1;
            rsp->req_id = cntx->req_id;
            rsp->session_role = BT_BIP_RESPONDER_ROLE;

            btmtk_bipr_send_request(MSG_ID_BT_BIP_DISCONNECT_REQ, rsp, sizeof(bt_bip_disconnect_req_struct));

            return BT_BIP_ERR_PROCESSING;
        }
        else
        {
            para.opcode = BT_BIP_OP_DISCONNECT;
            para.status = BT_BIPR_OPS_RESPONDING;
            para.data.result = BT_BIP_ERR_STATE;
            btmtk_bipr_notify_app(cntx, &para, 0);
            return BT_BIP_ERR_STATE;
        }

        /* following status can be disconnect normally */
    case BIPR_STATUS_ACCEPTED:
    case BIPR_STATUS_RSP_AUTH:
        btmtk_bipr_status_trans(hBipR, BIPR_STATUS_DISCONNECTING);
        cntx->opcode = BT_BIP_OP_DISCONNECT;

        rsp = (bt_bip_disconnect_req_struct*)
            btmtk_bip_alloc_local_para(sizeof(bt_bip_disconnect_req_struct));
        rsp->cm_conn_id = cntx->connect_id;
        rsp->disconnect_tp_directly = 0;
        rsp->req_id = cntx->req_id;
        rsp->session_role = BT_BIP_RESPONDER_ROLE;

        btmtk_bipr_send_request(MSG_ID_BT_BIP_DISCONNECT_REQ, rsp, sizeof(bt_bip_disconnect_req_struct));

        return BT_BIP_ERR_PROCESSING;

    default:
        para.opcode = BT_BIP_OP_DISCONNECT;
        para.status = BT_BIPR_OPS_RESPONDING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }
}
static S32 bipr_deactivate(HMTKBIPR hBipR)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_connect_rsp_struct* rsp;
    bt_bip_disconnect_req_struct* req;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    /* there are 3 things must to do:
     * 1. disconnect data link if exsit
     * 2. deregister SDB
     * 3. deactive BIP service
     */
    if (cntx->status == BIPR_STATUS_DEACTIVATING ||
        cntx->status == BIPR_STATUS_DEREGISTERING ||
        cntx->status == BIPR_STATUS_DEAC_DISCONNECTING)
    {
        /* in deactivate way already */
        para.opcode = BT_BIP_OP_DEACTIVATE;
        para.status = BT_BIPR_OPS_CONFIRMING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    switch (cntx->status)
    {
    case BIPR_STATUS_ACTIVATING:
    case BIPR_STATUS_REG_DEACTIVATING:
        /* just transform status, and in activated event handler do deactivation */
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_DEACTIVATING);
        break;

    case BIPR_STATUS_REGISTERING:
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_DEREGISTERING);
        break;

    case BIPR_STATUS_LISTENING:
        /* following two case, now have nothing to do... */
    case BIPR_STATUS_AUTHORIZING:
    case BIPR_STATUS_AUTHORIZED:
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_DEREGISTERING);
        btmtk_bipr_send_deregister_req(cntx);
        break;

    case BIPR_STATUS_ACCEPTING:
        /* response reject connect indication, then we should always receive
         * DISCONNECT_IND
         */
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_DEAC_DISCONNECTING);

        rsp = (bt_bip_connect_rsp_struct*) btmtk_bip_alloc_local_para(sizeof(bt_bip_connect_rsp_struct));
        rsp->cnf_code = BT_BIP_CNF_FAILED;
        btmtk_bipr_send_request(MSG_ID_BT_BIP_CONNECT_RSP, rsp, sizeof(bt_bip_connect_rsp_struct));
        break;

    default:
        /* all other case, do force disconnection, and tranforms staus */
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_DEAC_DISCONNECTING);

        req = (bt_bip_disconnect_req_struct*)
            btmtk_bip_alloc_local_para(sizeof(bt_bip_disconnect_req_struct));
        req->cm_conn_id = cntx->connect_id;
        req->disconnect_tp_directly = 1;
        req->req_id = cntx->req_id;
        req->session_role = BT_BIP_RESPONDER_ROLE;
        btmtk_bipr_send_request(MSG_ID_BT_BIP_DISCONNECT_REQ, req, sizeof(bt_bip_disconnect_req_struct));
    }
    return BT_BIP_ERR_PROCESSING;
}

static S32 bipr_obex_auth_rsp(HMTKBIPR hBipR, bt_bip_obex_auth_rsp* rsp)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_obex_auth_rsp_struct* irsp;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPR_STATUS_CN_BE_CHAL &&
        /* anyway, we can response CONNECT request with a challenge response
         * there is another case :
         *      AUTHORIZED -> ACCEPTING -> even client challenge
         *      server also challenge client then ->AUTHORIZED status again
         *      when client response challenge then ->ACCEPTING again!!!
         */
        cntx->status != BIPR_STATUS_ACCEPTING)
    {
        para.opcode = BT_BIP_OP_OBEX_AUTH;
        para.status = BT_BIPR_OPS_RESPONDING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    irsp = (bt_bip_obex_auth_rsp_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_bip_obex_auth_rsp_struct));
    irsp->cm_conn_id = cntx->connect_id;
    if (rsp->result == BT_BIP_ERR_OK)
    {
        irsp->result = BT_BIP_CNF_SUCCESS;
        irsp->dig_rsp.pwd_len = rsp->pwd_len;
        if (irsp->dig_rsp.pwd_len)
            memcpy(irsp->dig_rsp.pwd, rsp->pwd, irsp->dig_rsp.pwd_len);
        irsp->dig_rsp.uid_len = rsp->uid_len;
        if (irsp->dig_rsp.uid_len)
            memcpy(irsp->dig_rsp.uid, rsp->uid, irsp->dig_rsp.uid_len);

        /* even pass word is wrong, here will change to ACCEPTED
         * if client do not connect anymore, then client should disconnect
         */
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_ACCEPTED);
    }
    else
    {
        irsp->result = BT_BIP_CNF_FAILED;

        btmtk_bipr_status_trans(cntx, BIPR_STATUS_AUTHORIZED);
    }
    btmtk_bipr_send_request(MSG_ID_BT_BIPR_OBEX_AUTH_RSP, irsp, sizeof(bt_bip_obex_auth_rsp_struct));

    return BT_BIP_ERR_PROCESSING;
}

static S32 bipr_obex_auth_req(HMTKBIPR hBipR, bt_bip_obex_auth_req* rsp)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_obex_auth_req_struct* req;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPR_STATUS_ACCEPTING &&
        cntx->status != BIPR_STATUS_RESPONDING &&
        cntx->status != BIPR_STATUS_CN_BE_CHAL)
    {
        para.opcode = BT_BIP_OP_OBEX_AUTH;
        para.status = BT_BIPR_OPS_REQUESTING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    /* since we just surpport connect authentication, so here change to AUTHORIZED status */
    btmtk_bipr_status_trans(cntx, BIPR_STATUS_AUTHORIZED);

    req = (bt_bip_obex_auth_req_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_bip_obex_auth_req_struct));
    req->cm_conn_id = cntx->connect_id;

    req->dig_chal.pwd_len = rsp->pwd_len;
    if (req->dig_chal.pwd_len)
        memcpy(req->dig_chal.pwd, rsp->pwd, req->dig_chal.pwd_len);

    req->dig_chal.uid_len = rsp->uid_len;
    if (req->dig_chal.uid_len)
        memcpy(req->dig_chal.uid, rsp->uid, req->dig_chal.uid_len);

    req->dig_chal.realm_len = rsp->realm_len;
    if (req->dig_chal.realm_len)
        memcpy(req->dig_chal.realm, rsp->realm, req->dig_chal.realm_len);

    btmtk_bipr_send_request(MSG_ID_BT_BIPR_OBEX_AUTH_REQ, req, sizeof(bt_bip_obex_auth_req_struct));
    return BT_BIP_ERR_PROCESSING;
}

static S32 bipr_continue(HMTKBIPR hBipR, U32 cont)
{
    bt_bipr_context* cntx = (bt_bipr_context*)hBipR;
    bt_bipr_para para;
    bt_bip_continue_rsp_struct* irsp;

    memset((void*)&para, 0, sizeof(bt_bipr_para));

    if (!bipr_check_handle(hBipR))
        return BT_BIP_ERR_INVALID_PARA;

    if (cntx->status != BIPR_STATUS_INDICATING)
    {
        para.opcode = BT_BIP_OP_PUT_LINKED_THUMBNAIL;
        para.status = BT_BIPR_OPS_RESPONDING;
        para.data.result = BT_BIP_ERR_STATE;
        btmtk_bipr_notify_app(cntx, &para, 0);
        return BT_BIP_ERR_STATE;
    }

    irsp = (bt_bip_continue_rsp_struct*)
        btmtk_bip_alloc_local_para(sizeof(bt_bip_continue_rsp_struct));

    if (cont)
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_CONTINUE);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = BT_BIP_CNF_SUCCESS;
        btmtk_bipr_send_request(MSG_ID_BT_BIP_CONTINUE_RSP,
            irsp,
            sizeof(bt_bip_continue_rsp_struct));

        return BT_BIP_ERR_PROCESSING;
    }
    else
    {
        btmtk_bipr_status_trans(cntx, BIPR_STATUS_ACCEPTED);

        irsp->cm_conn_id = cntx->connect_id;
        irsp->cnf_code = BT_BIP_CNF_FAILED;
        btmtk_bipr_send_request(MSG_ID_BT_BIP_CONTINUE_RSP,
            irsp,
            sizeof(bt_bip_continue_rsp_struct));
        return BT_BIP_ERR_OK;
    }
}

static S32 bipr_handle_cmd(HMTKBIPR hBipR, U32 opcode, void* rsp)
{
    // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPR_EXT_ADP_APP_CMD, opcode);

    switch (opcode)
    {
    case BT_BIP_OP_ACTIVATE:
        return bipr_activate((bt_bipr_active_info*)rsp);

    case BT_BIP_OP_DEACTIVATE:
        return bipr_deactivate(hBipR);

    case BT_BIP_OP_AUTHORIZE:
        return bipr_authorized_rsp(hBipR, *(U32*)rsp);

    case BT_BIP_OP_CONNECT:
        return bipr_connect_rsp(hBipR, *(U32*)rsp);

    case BT_BIP_OP_GET_CAPABILITIES:
		return bipr_get_capabilities(hBipR, (bt_bip_capabilities_rsp*)rsp);

    case BT_BIP_OP_GET_IMG_LIST:
        return bipr_get_images_list(hBipR, (bt_bip_get_img_list_rsp*)rsp);

    case BT_BIP_OP_GET_IMG_PROPERTIES:
        return bipr_get_image_properties(hBipR, (bt_bip_get_img_prop_rsp*)rsp);

    case BT_BIP_OP_GET_IMG:
        return bipr_get_image(hBipR, (bt_bip_get_img_rsp*)rsp);

    case BT_BIP_OP_GET_LINKED_THUMBNAIL:
        return bipr_get_linked_thumbnail(hBipR, (bt_bip_get_linked_thum_rsp*)rsp);

    case BT_BIP_OP_PUT_IMG:
        return bipr_put_image(hBipR, (bt_bip_put_img_rsp*)rsp);

    case BT_BIP_OP_PUT_LINKED_THUMBNAIL:
        return bipr_put_linked_thumbnail(hBipR, (bt_bip_put_linked_thum_rsp*)rsp);

    case BT_BIP_OP_ABORT:
        return bipr_abort(hBipR);

    case BT_BIP_OP_DISCONNECT:
        return bipr_disconnect(hBipR, (U32)rsp);

    case BT_BIP_OP_OBEX_AUTH_RSP:
        return bipr_obex_auth_rsp(hBipR, (bt_bip_obex_auth_rsp*)rsp);

    case BT_BIP_OP_OBEX_AUTH_REQ:
        return bipr_obex_auth_req(hBipR, (bt_bip_obex_auth_req*)rsp);

    case BT_BIP_OP_CONTINUE:
        return bipr_continue(hBipR, (U32)rsp);
    }
    return 1;
}
#endif /* __BT_BIPR_PROFILE__ */
