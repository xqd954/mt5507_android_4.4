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
 * bt_bipi_api.c
 *
 * Project:
 * --------
 *   Export function from BIP Client
 *
 * Description:
 * ------------
 *   This file is for MMI BIP Client.
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

#ifndef __BT_BIP_CMD_AGENT__
//mtk03036
#include "bt_bipi_comm.c"
//#include "bt_bipi_comm.h"
#endif

#ifdef __BT_BIP_CMD_AGENT__
static void bipi_post_cmd_to_agent(HMTKBIPI hBipI, U32 opcode, void* req, U32 size)
{
    bt_bip_cmd_agent_struct* p;

    p = (bt_bip_cmd_agent_struct*)btmtk_bip_alloc_local_para(size + BIP_CMD_HDR_SIZE);
    p->handle = hBipI;
    p->opcode = opcode;
    if (size)
        memcpy(BT_BIP_GET_CMD_PTR(p), req, size);
    BTCMD_SendMessage(MSG_ID_BT_BIPI_CMD, MOD_EXTERNAL, p, (U16)(size + BIP_CMD_HDR_SIZE));
}
#endif

static S32 bipi_process_cmd(HMTKBIPI hBipI, U32 opcode, void* req, U32 size)
{
#ifdef __BT_BIP_CMD_AGENT__
    bipi_post_cmd_to_agent(hBipI, opcode, req, size);
    return BT_BIP_ERR_PROCESSING;
#else
    return bipi_handle_cmd(hBipI, opcode, req);
#endif
}

///////////////////////////////////////////////////////////////////////////////////
//  API implement                                                                //
///////////////////////////////////////////////////////////////////////////////////
S32 btmtk_bipi_activate(bt_bipi_active_info* init)
{
    if (!init || !init->callback)
        return BT_BIP_ERR_INVALID_PARA;

    return bipi_process_cmd((void*)0, BT_BIP_OP_ACTIVATE, init, sizeof(bt_bipi_active_info));
}

S32 btmtk_bipi_connect(HMTKBIPI hBipI, bt_bip_connect_req* conn)
{
    if (!hBipI ||
        !conn ||
        conn->auth_req.pwd_len > GOEP_MAX_PASSWD_SIZE ||
        conn->auth_req.uid_len > GOEP_MAX_USERID_SIZE ||
        conn->auth_req.realm_len > GOEP_MAX_REALM_SIZE ||
        (conn->service != BT_BIP_SRV_IMG_PUSH &&
         conn->service != BT_BIP_SRV_IMG_PULL))
        return BT_BIP_ERR_INVALID_PARA;

    return bipi_process_cmd(hBipI, BT_BIP_OP_CONNECT, conn, sizeof(bt_bip_connect_req));
}


/* then this function will response the challenge.
 * this function will re-send current requestion with related authentication response
 */
S32 btmtk_bipi_obex_auth_rsp(HMTKBIPI hBipI, bt_bip_obex_auth_rsp* rsp)
{
    if (!hBipI || !rsp ||
        (rsp->result == BT_BIP_ERR_OK && !rsp->pwd_len)||
        rsp->pwd_len > GOEP_MAX_PASSWD_SIZE ||
        rsp->uid_len > GOEP_MAX_USERID_SIZE)
        return BT_BIP_ERR_INVALID_PARA;

    return bipi_process_cmd(hBipI, BT_BIP_OP_OBEX_AUTH_RSP, rsp, sizeof(bt_bip_obex_auth_rsp));
}

S32 btmtk_bipi_get_capabilities(HMTKBIPI hBipI)
{
    if (!hBipI)
        return BT_BIP_ERR_INVALID_PARA;

    return bipi_process_cmd(hBipI, BT_BIP_OP_GET_CAPABILITIES, 0, 0);
}

S32 btmtk_bipi_get_images_list(HMTKBIPI hBipI, bt_bip_get_img_list_req* req)
{
    if (!hBipI || !req)
        return BT_BIP_ERR_INVALID_PARA;

    return bipi_process_cmd(hBipI, BT_BIP_OP_GET_IMG_LIST, req, sizeof(bt_bip_get_img_list_req));
}

S32 btmtk_bipi_get_image_properties(HMTKBIPI hBipI,bt_bip_get_img_prop_req* req)
{
    if (!hBipI || !req || req->handle[0] == 0)
        return BT_BIP_ERR_INVALID_PARA;

    return bipi_process_cmd(hBipI, BT_BIP_OP_GET_IMG_PROPERTIES, req, sizeof(bt_bip_get_img_prop_req));
}

S32 btmtk_bipi_get_image(HMTKBIPI hBipI, bt_bip_get_img_req* req)
{
    if (!hBipI || !req || req->handle[0] == 0)
        return BT_BIP_ERR_INVALID_PARA;
    /* check file path now ??? */

    return bipi_process_cmd(hBipI, BT_BIP_OP_GET_IMG, req, sizeof(bt_bip_get_img_req));
}

S32 btmtk_bipi_get_linked_thumbnail(HMTKBIPI hBipI, bt_bip_get_linked_thum_req* req)
{
    if (!hBipI || !req || req->handle[0])
        return BT_BIP_ERR_INVALID_PARA;
    /* check file path now ??? */

    return bipi_process_cmd(hBipI, BT_BIP_OP_GET_LINKED_THUMBNAIL, req, sizeof(bt_bip_get_linked_thum_req));
}

S32 btmtk_bipi_put_image(HMTKBIPI hBipI, bt_bip_put_img_req* req)
{
    EXT_FS_HANDLE h;

    if (!hBipI || !req || req->img_path[0] == 0)
        return BT_BIP_ERR_INVALID_PARA;

    /* check file path now ??? */
    h = ext_fs_open_ucs2(req->img_path, EXT_FS_READ_ONLY);
    if (h < 0)
        return BT_BIP_ERR_INVALID_PARA;
    ext_fs_close_ucs2(h);

    return bipi_process_cmd(hBipI, BT_BIP_OP_PUT_IMG, req, sizeof(bt_bip_put_img_req));
}

S32 btmtk_bipi_put_linked_thumbnail(HMTKBIPI hBipI, bt_bip_put_linked_thum_req* req)
{
    EXT_FS_HANDLE h;

    if (!hBipI || !req || req->img_path[0] == 0)
        return BT_BIP_ERR_INVALID_PARA;

    /* check file path now ??? */
    h = ext_fs_open_ucs2(req->img_path, EXT_FS_READ_ONLY);
    if (h < 0)
        return BT_BIP_ERR_INVALID_PARA;
    ext_fs_close_ucs2(h);

    return bipi_process_cmd(hBipI, BT_BIP_OP_PUT_LINKED_THUMBNAIL, req, sizeof(bt_bip_put_linked_thum_req));
}

S32 btmtk_bipi_continue(HMTKBIPI hBipI, S32 cont)
{
    if (!hBipI)
        return BT_BIP_ERR_INVALID_PARA;

    return bipi_process_cmd(hBipI, BT_BIP_OP_CONTINUE, &cont, 4);
}

S32 btmtk_bipi_abort(HMTKBIPI hBipI)
{
    if (!hBipI)
        return BT_BIP_ERR_INVALID_PARA;

    return bipi_process_cmd(hBipI, BT_BIP_OP_ABORT, 0, 0);
}

S32 btmtk_bipi_disconnect(HMTKBIPI hBipI, U32 force)
{
    if (!hBipI)
        return BT_BIP_ERR_INVALID_PARA;

    return bipi_process_cmd(hBipI, BT_BIP_OP_DISCONNECT, &force, 4);
}

S32 btmtk_bipi_deactivate(HMTKBIPI hBipI)
{
    if (!hBipI)
        return BT_BIP_ERR_INVALID_PARA;

    return bipi_process_cmd(hBipI, BT_BIP_OP_DEACTIVATE, 0, 0);
}

#endif /* __BT_BIPI_PROFILE__ */
