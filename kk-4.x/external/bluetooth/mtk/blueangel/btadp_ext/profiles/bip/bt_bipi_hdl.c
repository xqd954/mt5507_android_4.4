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
 * bt_bipi_hdl.c
 *
 * Project:
 * --------
 *   Export function from BIP Client
 *
 * Description:
 * ------------
 *   This file is for External BIP ADP.
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
//#include "xatypes.h"
#include "bt_types.h"
#include "bt_message.h"
#include "bt_mmi.h"
#include "bt_mmi_msg.h"
#include "ext_osal.h"
#include "bluetooth_struct.h"
#include "bluetooth_bipr_struct.h"
#include "bluetooth_bipi_struct.h"
#include "bt_bipi_api.h"
#include "bt_bip_comm.h"
#include "bt_bipr_struct.h"
#include "bt_bipi_struct.h"

#include "bt_bip_internal.h"

static bt_bipi_context g_bipi_context[BIPI_MAX_INSTANCE];

#ifdef __BT_BIP_CMD_AGENT__
//mtk03036 
   #include "bt_bipi_comm.c"
//   #include "bt_bipi_comm.h"
#endif

/*
 * instance access function start
 */
static bt_bipi_context* bipi_find_handle_by_req_id(U8 req_id)
{
    U32 i;

    for (i = 0; i < BIPI_MAX_INSTANCE; i++)
    {
        if (g_bipi_context[i].status != BIPI_STATUS_IDLE &&
            g_bipi_context[i].req_id == req_id)
        {
            return &g_bipi_context[i];
        }
    }
    /* add trace */
    // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_DO_NOT_FIND_HANDLE_BY_REQ_ID, req_id);
    return NULL;
}

static bt_bipi_context* bipi_find_handle_by_cn_id(U32 connect_id)
{
    U32 i;

    for (i = 0; i < BIPI_MAX_INSTANCE; i++)
    {
        if (g_bipi_context[i].status != BIPI_STATUS_IDLE &&
            g_bipi_context[i].connect_id == connect_id)
        {
            return &g_bipi_context[i];
        }
    }
    /* add trace */
    // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_DO_NOT_FIND_HANDLE_BY_CN_ID, connect_id);
    return NULL;
}
static bt_bipi_context* bipi_find_handle_by_status(U32 status)
{
    U32 i;

    for (i = 0; i < BIPR_MAX_INSTANCE; i++)
    {
        if (g_bipi_context[i].status == status)
        {
            return &g_bipi_context[i];
        }
    }
    // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_DO_NOT_FIND_HANDLE_BY_STATUS, status);
    return NULL;
}

/*
 * instance access function end
 */


/*
 * confirm handler start
 */
static void bipi_handle_connect_cnf(bt_bip_connect_cnf_struct* rsp)
{
    bt_bipi_context* cntx;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    cntx = bipi_find_handle_by_req_id(rsp->req_id);
    if (!cntx)
        return;

    switch(cntx->status)
    {
    case BIPI_STATUS_CONNECTING:
        /* normal case */
        BIP_ASSERT(cntx->opcode == BT_BIP_OP_CONNECT);
        if (rsp->cnf_code == BT_BIP_CNF_SUCCESS)
        {
            btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONNECTED);
            para.result = BT_BIP_ERR_OK;
            cntx->connect_id = rsp->cm_conn_id;
        }
        else if (rsp->cnf_code == BT_BIP_CNF_CHALLENGE_OK)
        {
            btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONNECTED);
            para.result = BT_BIP_ERR_CHALLENGE_OK;
            cntx->connect_id = rsp->cm_conn_id;
        }
        else if (rsp->cnf_code == BT_BIP_CNF_CHALLENGE_FAILED)
        {
            btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONNECTED);
            para.result = BT_BIP_ERR_CHALLENGE_FAILED;
            cntx->connect_id = rsp->cm_conn_id;
        }
        /*
        in this case, OBEX_AUTH_IND will be received!

        else if (rsp->cnf_code == BT_BIP_AUTHENTICATE_FAILED)
        {
            btmtk_bipi_status_trans(cntx, BIPI_STATUS_CN_BE_CHAL);
            para.result = BT_BIP_ERR_OBAUTH_NEEDED;
        }
        */
        else
        {
            btmtk_bipi_status_trans(cntx, BIPI_STATUS_ACTIVATED);
            para.result = BT_BIP_ERR_FAILED;
        }
        para.opcode = BT_BIP_OP_CONNECT;
        btmtk_bipi_notify_app(cntx, &para, 1);
        break;

    case BIPI_STATUS_DEACTIVATING:
        /* deactive API be invoked */
        BIP_ASSERT(cntx->opcode == BT_BIP_OP_DEACTIVATE);

        if (rsp->cnf_code == BT_BIP_CNF_SUCCESS)
        {
            /* save connection_id, otherwise can't find correct handle
             * in disconnect_ind
            */
            cntx->connect_id = rsp->cm_conn_id;
            /* then waiting for disconnect_ind */
        }
        else
        {
            /* in this case, we will not receive disconnect_ind anymore */
            btmtk_bipi_free_handle(cntx); /* idle status */

            para.result = BT_BIP_ERR_OK;
            para.opcode = BT_BIP_OP_DEACTIVATE;
            btmtk_bipi_notify_app(cntx, &para, 1);
        }
        break;

    case BIPI_STATUS_DISCONNECTING:
        /* disconnect API be invoked */
        BIP_ASSERT(cntx->opcode == BT_BIP_OP_DISCONNECT);

        if (rsp->cnf_code == BT_BIP_CNF_SUCCESS)
        {
            /* save connection_id, otherwise can't find correct handle
             * in disconnect_ind
            */
            cntx->connect_id = rsp->cm_conn_id;
            /* then waiting for disconnect_ind */
        }
        else
        {
            /* in this case, we will not receive disconnect_ind anymore */
            btmtk_bipi_status_trans(cntx, BIPI_STATUS_ACTIVATED);

            para.result = BT_BIP_ERR_OK;
            para.opcode = BT_BIP_OP_DISCONNECT;
            btmtk_bipi_notify_app(cntx, &para, 1);
        }
        break;
    default:
        BIP_ASSERT(0);
    }
}

static void bipi_handle_get_capa_cnf(bt_bip_get_capabilities_cnf_struct* rsp)
{
    bt_bipi_context* cntx;
    bt_bipi_para para;
    U32 i;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    cntx = bipi_find_handle_by_cn_id(rsp->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPI_STATUS_REQUESTING &&
        cntx->status != BIPI_STATUS_CONTINUE &&
        cntx->status != BIPI_STATUS_ABORTING)
    {
        /* add trace current status*/
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_STATE_ERR, cntx->status, cntx->opcode);
        return;
    }

    BIP_ASSERT(cntx->opcode == BT_BIP_OP_GET_CAPABILITIES);

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONNECTED);

    if (rsp->cnf_code == BT_BIP_CNF_SUCCESS)
    {
        para.result = BT_BIP_ERR_OK;
        para.cnf.capa.created_time_filter = rsp->created_time_filter;
        para.cnf.capa.encoding_filter = rsp->encoding_filter;
        para.cnf.capa.modified_time_filter = rsp->modified_time_filter;
        para.cnf.capa.pixel_filter = rsp->pixel_filter;
        para.cnf.capa.preferred_format.encoding = (U32)rsp->preferred_format;
        for (i = 0; i < BT_BIP_MAX_IMG_FORMATS && rsp->supported_img_formats[i]; i++)
        {
            para.cnf.capa.supported_formats[i].encoding = rsp->supported_img_formats[i];
            para.cnf.capa.supported_formats[i].size = rsp->supported_maxsize[i];
            memcpy(&para.cnf.capa.supported_formats[i].pixel, &rsp->supported_img_size[i], sizeof(bt_bip_pixel));
        }

    }
    else
    {
        para.result =
            cntx->status == BIPI_STATUS_ABORTING ? BT_BIP_ERR_USER_ABORT : BT_BIP_ERR_FAILED;
    }
    para.opcode = BT_BIP_OP_GET_CAPABILITIES;

    btmtk_bipi_notify_app(cntx, &para, 1);
}

static U8 bipi_handle_continue_ind(bt_bip_continue_ind_struct* ind)
{
    bt_bipi_context* cntx;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    cntx = bipi_find_handle_by_cn_id(ind->cm_conn_id);
    if (!cntx)
        return 0;

    if (cntx->status != BIPI_STATUS_REQUESTING &&
        cntx->status != BIPI_STATUS_CONTINUE)
    {
        /* add trace current status*/
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_STATE_ERR, cntx->status, cntx->opcode);
        return 1;
    }

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_INDICATING);

    para.opcode = cntx->opcode;
    para.result = BT_BIP_ERR_INDICATING;
    para.cnf.ind.data_len = ind->data_len;
    para.cnf.ind.obj_len = ind->obj_len;

    btmtk_bipi_notify_app(cntx, &para, 1);

    return 1;
}

static void bipi_handle_put_img_cnf(bt_bip_put_img_cnf_struct* rsp)
{
    bt_bipi_context* cntx;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    cntx = bipi_find_handle_by_cn_id(rsp->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPI_STATUS_REQUESTING &&
        cntx->status != BIPI_STATUS_CONTINUE &&
        cntx->status != BIPI_STATUS_ABORTING)
    {
        /* add trace current status*/
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_STATE_ERR, cntx->status, cntx->opcode);
        return;
    }

    BIP_ASSERT(cntx->opcode == BT_BIP_OP_PUT_IMAGE);

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONNECTED);

    if (rsp->cnf_code == BT_BIP_CNF_SUCCESS)
    {
        para.result = BT_BIP_ERR_OK;
        strcpy((char*)para.cnf.put_img.handle, (const char*)rsp->img_handle);
    }
    else if (rsp->cnf_code == BT_BIP_PARTIAL_CONTENT)
    {
        para.result = BT_BIP_ERR_THUMBNAIL_NEEDED;
        strcpy((char*)para.cnf.put_img.handle, (const char*)rsp->img_handle);
    }
    else
    {
        para.result =
            cntx->status == BIPI_STATUS_ABORTING ? BT_BIP_ERR_USER_ABORT : BT_BIP_ERR_FAILED;
    }
    para.opcode = BT_BIP_OP_PUT_IMG;
    btmtk_bipi_notify_app(cntx, &para, 1);

}

static void bipi_handle_put_link_thum_cnf(bt_bip_put_linked_thumbnail_cnf_struct* rsp)
{
    bt_bipi_context* cntx;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    cntx = bipi_find_handle_by_cn_id(rsp->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPI_STATUS_REQUESTING &&
        cntx->status != BIPI_STATUS_CONTINUE&&
        cntx->status != BIPI_STATUS_ABORTING)
    {
        /* add trace current status*/
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_STATE_ERR, cntx->status, cntx->opcode);
        return;
    }

    BIP_ASSERT(cntx->opcode == BT_BIP_OP_PUT_LINKED_THUMBNAIL);

    if (rsp->cnf_code == BT_BIP_CNF_SUCCESS)
    {
        para.result = BT_BIP_ERR_OK;
    }
    else
    {
        para.result =
            cntx->status == BIPI_STATUS_ABORTING ? BT_BIP_ERR_USER_ABORT : BT_BIP_ERR_FAILED;
    }
    para.opcode = BT_BIP_OP_PUT_LINKED_THUMBNAIL;
    btmtk_bipi_notify_app(cntx, &para, 1);

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONNECTED);
}

static void bipi_handle_get_img_list_cnf(bt_bip_get_img_list_cnf_struct* rsp)
{
    bt_bipi_context* cntx;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    cntx = bipi_find_handle_by_cn_id(rsp->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPI_STATUS_REQUESTING &&
        cntx->status != BIPI_STATUS_CONTINUE &&
        cntx->status != BIPI_STATUS_ABORTING)
    {
        /* add trace current status*/
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_STATE_ERR, cntx->status, cntx->opcode);
        return;
    }

    BIP_ASSERT(cntx->opcode == BT_BIP_OP_GET_IMG_LIST);

    if (rsp->cnf_code == BT_BIP_CNF_SUCCESS)
    {
        para.result = BT_BIP_ERR_OK;
        para.cnf.img_list.img_count = rsp->img_count;
        btmtk_wstrcpy(para.cnf.img_list.img_list_path, rsp->img_list_path);
        /* TODO : w,h */
    }
    else
    {
        para.result =
            cntx->status == BIPI_STATUS_ABORTING ? BT_BIP_ERR_USER_ABORT : BT_BIP_ERR_FAILED;
    }
    para.opcode = BT_BIP_OP_GET_IMG_LIST;
    btmtk_bipi_notify_app(cntx, &para, 1);

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONNECTED);
}

static void bipi_handle_get_img_prop_cnf(bt_bip_get_img_prop_cnf_struct* rsp)
{
    bt_bipi_context* cntx;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    cntx = bipi_find_handle_by_cn_id(rsp->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPI_STATUS_REQUESTING &&
        cntx->status != BIPI_STATUS_CONTINUE &&
        cntx->status != BIPI_STATUS_ABORTING)
    {
        /* add trace current status*/
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_STATE_ERR, cntx->status, cntx->opcode);
        return;
    }

    BIP_ASSERT(cntx->opcode == BT_BIP_OP_GET_IMG_PROPERTIES);

    if (rsp->cnf_code == BT_BIP_CNF_SUCCESS)
    {
        para.result = BT_BIP_ERR_OK;
        strcpy((char*)para.cnf.img_prop.handle, (const char*)rsp->native_img.img_handle);
        btmtk_wstrcpy(para.cnf.img_prop.friendly_name, (U16*)rsp->native_img.friendly_name);
        para.cnf.img_prop.native.encoding = rsp->native_img.encoding;
        para.cnf.img_prop.native.size = rsp->native_img.size;
        /* TODO : w,h */
    }
    else
    {
        para.result =
            cntx->status == BIPI_STATUS_ABORTING ? BT_BIP_ERR_USER_ABORT : BT_BIP_ERR_FAILED;
    }
    para.opcode = BT_BIP_OP_GET_IMG_PROPERTIES;
    btmtk_bipi_notify_app(cntx, &para, 1);

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONNECTED);
}

static void bipi_handle_get_img_cnf(bt_bip_get_img_cnf_struct* rsp)
{
    bt_bipi_context* cntx;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    cntx = bipi_find_handle_by_cn_id(rsp->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPI_STATUS_REQUESTING &&
        cntx->status != BIPI_STATUS_CONTINUE &&
        cntx->status != BIPI_STATUS_ABORTING)
    {
        /* add trace current status*/
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_STATE_ERR, cntx->status, cntx->opcode);
        return;
    }

    BIP_ASSERT(cntx->opcode == BT_BIP_OP_GET_IMAGE);

    if (rsp->cnf_code == BT_BIP_CNF_SUCCESS)
    {
        para.result = BT_BIP_ERR_OK;
        para.cnf.get_img.size = rsp->data_len;
    }
    else
    {
        para.result =
            cntx->status == BIPI_STATUS_ABORTING ? BT_BIP_ERR_USER_ABORT : BT_BIP_ERR_FAILED;
    }
    para.opcode = BT_BIP_OP_GET_IMG;
    btmtk_bipi_notify_app(cntx, &para, 1);

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONNECTED);
}

static void bipi_handle_get_linked_thum_cnf(bt_bip_get_linked_thumbnail_cnf_struct* rsp)
{
    bt_bipi_context* cntx;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    cntx = bipi_find_handle_by_cn_id(rsp->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPI_STATUS_REQUESTING &&
        cntx->status != BIPI_STATUS_CONTINUE &&
        cntx->status != BIPI_STATUS_ABORTING)
    {
        /* add trace current status*/
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_STATE_ERR, cntx->status, cntx->opcode);
        return;
    }

    BIP_ASSERT(cntx->opcode == BT_BIP_OP_GET_LINKED_THUMBNAIL);

    if (rsp->cnf_code == BT_BIP_CNF_SUCCESS)
    {
        para.result = BT_BIP_ERR_OK;
    }
    else
    {
        para.result =
            cntx->status == BIPI_STATUS_ABORTING ? BT_BIP_ERR_USER_ABORT : BT_BIP_ERR_FAILED;
    }
    para.opcode = BT_BIP_OP_GET_LINKED_THUMBNAIL;
    btmtk_bipi_notify_app(cntx, &para, 1);

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONNECTED);
}

static void bipi_handle_abort_cnf(bt_bip_abort_cnf_struct* rsp)
{
    bt_bipi_context* cntx;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    cntx = bipi_find_handle_by_cn_id(rsp->cm_conn_id);
    if (!cntx)
        return;

    if (cntx->status != BIPI_STATUS_ABORTING)
    {
        /* add trace current status*/
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_STATE_ERR, cntx->status, cntx->opcode);
        return;
    }

    btmtk_bipi_status_trans(cntx, BIPI_STATUS_CONNECTED);
    if (cntx->file_name[0])
    {
        ext_fs_delete_ucs2(cntx->file_name);
        cntx->file_name[0] = 0;
    }

    /* BIP_ASSERT(rsp->cnf_code == BT_BIP_CNF_SUCCESS) */
    para.result = BT_BIP_ERR_USER_ABORT;
    para.opcode = cntx->opcode;
    btmtk_bipi_notify_app(cntx, &para, 1);
}

static void bipi_handle_obex_auth_ind(bt_bip_obex_auth_ind_struct* ind)
{
    bt_bipi_context* cntx = NULL;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (ind->cm_conn_id != (U32)-1)
        cntx = bipi_find_handle_by_cn_id(ind->cm_conn_id);

    if (!cntx) /* connect case */
    {
        cntx = bipi_find_handle_by_status(BIPI_STATUS_CONNECTING);
        if (!cntx)
        {
            /* add trace */
            return;
        }
        btmtk_bipi_status_trans(cntx, BIPI_STATUS_CN_BE_CHAL);
    }
    else
    {
        /* connected, now we do not support this case */
        /* add trace */
        // TODO log kal_trace(BT_TRACE_G6_OBEX, BIP_EXT_ADP_DO_NOT_SUPPORT_NOW, __LINE__);
        return;
    }
    para.cnf.obauth_ind.option = ind->option;
    para.cnf.obauth_ind.realm_len = ind->realm_len;
    if (ind->realm_len)
        memcpy(para.cnf.obauth_ind.realm, ind->realm, ind->realm_len);
    para.cnf.obauth_ind.option = ind->option;
    para.cnf.obauth_ind.realm_len = ind->realm_len;
    if (ind->realm_len)
        memcpy(&para.cnf.obauth_ind.realm, &ind->realm, ind->realm_len);
    para.opcode = BT_BIP_OP_OBEX_AUTH;
    para.result = BT_BIP_ERR_OBAUTH_NEEDED;
    btmtk_bipi_notify_app(cntx, &para, 1);
}

static void bipi_handle_obex_auth_cnf()
{
}

static U8 bipi_handle_disconnect_ind(bt_bip_disconnect_ind_struct* ind)
{
    bt_bipi_context* cntx;
    bt_bipi_para para;

    memset((void*)&para, 0, sizeof(bt_bipi_para));


    if (ind->session_role == BT_BIP_RESPONDER_ROLE)
    {
        return 0;
    }
    cntx = bipi_find_handle_by_cn_id(ind->cm_conn_id);
    if (!cntx)
        return 0;

    cntx->connect_id = (U32)-1;

    if (cntx->status == BIPI_STATUS_ACTIVATED ||
        cntx->status == BIPI_STATUS_IDLE)
    {
        /* add trace current status*/
        return 1;
    }

    switch (cntx->status)
    {
    case BIPI_STATUS_DISCONNECTING:
        btmtk_bipi_status_trans(cntx, BIPI_STATUS_ACTIVATED);
        para.opcode = BT_BIP_OP_DISCONNECT;
        para.result = BT_BIP_ERR_OK;
        btmtk_bipi_notify_app(cntx, &para, 1);
        break;

    case BIPI_STATUS_DEACTIVATING:
        para.opcode = BT_BIP_OP_DEACTIVATE;
        para.result = BT_BIP_ERR_OK;
        btmtk_bipi_notify_app(cntx, &para, 1);
        btmtk_bipi_free_handle(cntx); /* idle status */
        break;

    default:
        btmtk_bipi_status_trans(cntx, BIPI_STATUS_ACTIVATED);
        para.opcode = BT_BIP_OP_DISCONNECT;
        para.result = BT_BIP_ERR_SERVER_ABORT;
        btmtk_bipi_notify_app(cntx, &para, 1);
        break;
    }


    return 1;
}
/*****************************************************************************
 external function
*****************************************************************************/

/*
 * BIPI instance management start
*/
bt_bipi_context* btmtk_bipi_alloc_handle(void)
{
    U32 i;

    for (i = 0; i < BIPI_MAX_INSTANCE; i++)
    {
        if (g_bipi_context[i].status == BIPI_STATUS_IDLE)
        {
            g_bipi_context[i].status = BIPI_STATUS_ACTIVATED;
            g_bipi_context[i].req_id = (U8)(i + 1);
            g_bipi_context[i].connect_id = (U32)-1; /* invalid id */
            return &g_bipi_context[i];
        }
    }
    return NULL;
}

U8 btmtk_bipi_verify_handle(HMTKBIPI hBipI)
{
    U32 i;

    for (i = 0; i < BIPI_MAX_INSTANCE; i++)
    {
        if (g_bipi_context[i].status != BIPI_STATUS_IDLE &&
            (HMTKBIPI)&g_bipi_context[i]== hBipI)
            return 1;
    }
    return 0;
}

void btmtk_bipi_free_handle(bt_bipi_context* cntx)
{
    memset(cntx, 0, sizeof(bt_bipi_context));
}

void btmtk_bipi_status_trans(bt_bipi_context* cntx, U32 status)
{
    cntx->status = status;
}

/*
 * BIPI instance management end
*/

U8 btmtk_bipi_handle_message(ilm_struct* msg)
{
//mtk03036
#ifdef BTMTK_ON_LINUX
    void* para = (void*)msg->ilm_data;
#else
    void* para = (void*)msg->local_para_ptr;
#endif
    XLOGD("para: %p", para);
    if (!para)
      return 0;
//mtk03036 end

    // TODO log kal_trace(BT_TRACE_G6_OBEX, BIPI_EXT_ADP_RECEIVE_MSG, msg->msg_id);

    switch(msg->msg_id)
    {
#ifdef __BT_BIP_CMD_AGENT__
    case MSG_ID_BT_BIPI_CMD:
        bipi_handle_cmd(((bt_bip_cmd_agent_struct*)para)->handle,
                          ((bt_bip_cmd_agent_struct*)para)->opcode,
                          BT_BIP_GET_CMD_PTR(para));
        return 1;
#endif

   case MSG_ID_BT_BIP_CONNECT_CNF:
       bipi_handle_connect_cnf((bt_bip_connect_cnf_struct*)para);
       return 1;

   case MSG_ID_BT_BIP_GET_CAPABILITIES_CNF:
       bipi_handle_get_capa_cnf((bt_bip_get_capabilities_cnf_struct*)para);
       return 1;

   case MSG_ID_BT_BIP_CONTINUE_IND:
       /* since now this msg id used by I & R!!! */
       return bipi_handle_continue_ind((bt_bip_continue_ind_struct*)para);

   case MSG_ID_BT_BIP_PUT_IMG_CNF:
       bipi_handle_put_img_cnf((bt_bip_put_img_cnf_struct*)para);
       return 1;

   case MSG_ID_BT_BIP_PUT_LINKED_THUMBNAIL_CNF:
       bipi_handle_put_link_thum_cnf((bt_bip_put_linked_thumbnail_cnf_struct*)para);
       return 1;

   case MSG_ID_BT_BIP_GET_IMG_LIST_CNF:
       bipi_handle_get_img_list_cnf((bt_bip_get_img_list_cnf_struct*)para);
       return 1;

   case MSG_ID_BT_BIP_GET_IMG_PROP_CNF:
       bipi_handle_get_img_prop_cnf((bt_bip_get_img_prop_cnf_struct*)para);
       return 1;

   case MSG_ID_BT_BIP_GET_IMG_CNF:
       bipi_handle_get_img_cnf((bt_bip_get_img_cnf_struct*)para);
       return 1;

   case MSG_ID_BT_BIP_GET_LINKED_THUMBNAIL_CNF:
       bipi_handle_get_linked_thum_cnf((bt_bip_get_linked_thumbnail_cnf_struct*)para);
       return 1;

   case MSG_ID_BT_BIP_GET_MONITORING_IMG_CNF:
       return 1;

   case MSG_ID_BT_BIP_ABORT_CNF:
        bipi_handle_abort_cnf((bt_bip_abort_cnf_struct*)para);
        return 1;

   case MSG_ID_BT_BIPI_OBEX_AUTH_IND:
       bipi_handle_obex_auth_ind((bt_bip_obex_auth_ind_struct*)para);
       return 1;

   case MSG_ID_BT_BIPI_OBEX_AUTH_CNF:
       bipi_handle_obex_auth_cnf();
       return 1;

   case MSG_ID_BT_BIP_DISCONNECT_IND:
       return bipi_handle_disconnect_ind((bt_bip_disconnect_ind_struct*)para);
    }
    return 0;
}
#endif /* __BT_BIPI_PROFILE__ */
