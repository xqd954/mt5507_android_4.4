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
 * bt_bipr_api.c
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
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_BIPR_PROFILE__

//#include "xatypes.h"
#include "bt_types.h"
#include "bt_message.h"
#include "bt_mmi.h"
#include "bt_mmi_msg.h"
#include "ext_osal.h"
#include "bluetooth_struct.h"
#include "bluetooth_bipr_struct.h"
#include "bt_bipr_api.h"
#include "bt_bip_comm.h"
#include "bt_bipr_struct.h"
#include "bt_bipi_struct.h"

#include "bt_bip_internal.h"

typedef struct
{
    U16 item_count;
    U16 status;
    EXT_FS_HANDLE handle;
} bipr_img_list_compose;

static bipr_img_list_compose g_imglist;

#ifndef __BT_BIP_CMD_AGENT__
	#include "bt_bipr_comm.c"
#endif

#ifdef __BT_BIP_CMD_AGENT__
static void bipr_post_cmd_to_agent(HMTKBIPR hBipR, U32 opcode, void* req, U32 size)
{
    bt_bip_cmd_agent_struct* p;

    p = (bt_bip_cmd_agent_struct*)btmtk_bip_alloc_local_para(size + BIP_CMD_HDR_SIZE);
    p->handle = hBipR;
    p->opcode = opcode;
    if (size)
        memcpy(BT_BIP_GET_CMD_PTR(p), req, size);
    BTCMD_SendMessage(MSG_ID_BT_BIPR_CMD, MOD_EXTERNAL, p, (U16)(size + BIP_CMD_HDR_SIZE));
}
#endif

static S32 bipr_process_cmd(HMTKBIPR hBipR, U32 opcode, void* req, U32 size)
{
#ifdef __BT_BIP_CMD_AGENT__
    bipr_post_cmd_to_agent(hBipR, opcode, req, size);
    return BT_BIP_ERR_PROCESSING;
#else
    return bipr_handle_cmd(hBipR, opcode, req);
#endif
}

///////////////////////////////////////////////////////////////////////////////////
//  API implement                                                                //
///////////////////////////////////////////////////////////////////////////////////
S32 btmtk_bipr_activate(bt_bipr_active_info* init)
{
    if (!init || !init->callback ||
        /* we only support push&pull */
        !(init->service & (BT_BIP_SRV_IMG_PUSH | BT_BIP_SRV_IMG_PULL)))
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd((void*)0, BT_BIP_OP_ACTIVATE, init, sizeof(bt_bipr_active_info));
}

S32 btmtk_bipr_authorize(HMTKBIPR hBipR, U32 auth)
{
    if (!hBipR)
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd(hBipR, BT_BIP_OP_AUTHORIZE, &auth, 4);
}

S32 btmtk_bipr_connect_rsp(HMTKBIPR hBipR, U32 option)
{
    if (!hBipR)
        return BT_BIP_ERR_INVALID_PARA;
    return bipr_process_cmd(hBipR, BT_BIP_OP_CONNECT, &option, 4);
}

/* to send challenge to client, can be to response any bip-client request */
S32 btmtk_bipr_obex_auth_req(HMTKBIPR hBipR, bt_bip_obex_auth_req* req)
{
    if (!hBipR || !req ||
        req->pwd_len > GOEP_MAX_PASSWD_SIZE ||
        req->realm_len > GOEP_MAX_REALM_SIZE ||
        req->uid_len > GOEP_MAX_USERID_SIZE)
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd(hBipR, BT_BIP_OP_OBEX_AUTH_REQ, req, sizeof(bt_bip_obex_auth_req));
}

/* to response a challenge */
S32 btmtk_bipr_obex_auth_rsp(HMTKBIPR hBipR, bt_bip_obex_auth_rsp* rsp)
{
    if (!hBipR || !rsp ||
        (rsp->result == BT_BIP_ERR_OK && !rsp->pwd) ||
        rsp->pwd_len > GOEP_MAX_PASSWD_SIZE ||
        rsp->uid_len > GOEP_MAX_USERID_SIZE)
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd(hBipR, BT_BIP_OP_OBEX_AUTH_RSP, rsp, sizeof(bt_bip_obex_auth_rsp));
}

S32 btmtk_bipr_get_capabilities(HMTKBIPR hBipR, bt_bip_capabilities_rsp* rsp)
{
    if (!hBipR || !rsp)
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd(hBipR, BT_BIP_OP_GET_CAPABILITIES, rsp, sizeof(bt_bip_capabilities_rsp));
}
/*
 * invoker should take more consideration about this API and make it keep correct status!!!
 */
S32 btmtk_bipr_get_images_list(HMTKBIPR hBipR, bt_bip_get_img_list_rsp* rsp)
{
    if (!hBipR || !rsp ||
        (rsp->result != BT_BIP_ERR_FAILED &&
        rsp->result != BT_BIP_ERR_CONTINUE &&
        rsp->result != BT_BIP_ERR_OK) ||
        (rsp->result == BT_BIP_ERR_CONTINUE && !rsp->item_count))
        return BT_BIP_ERR_INVALID_PARA;

    if (rsp->result == BT_BIP_ERR_FAILED)
    {
        return bipr_process_cmd(hBipR, BT_BIP_OP_GET_IMG_LIST, rsp, sizeof(bt_bip_get_img_list_rsp));
    }
    else if (rsp->result == BT_BIP_ERR_CONTINUE)
    {
        return BT_BIP_ERR_OK;
    }
    /* write end of xml */
    /* close file handle*/
    rsp->item_count = g_imglist.item_count;
    return bipr_process_cmd(hBipR, BT_BIP_OP_GET_IMG_LIST, rsp, sizeof(bt_bip_get_img_list_rsp));
}

S32 btmtk_bipr_get_image_properties(HMTKBIPR hBipR, bt_bip_get_img_prop_rsp* rsp)
{
    if (!hBipR || !rsp)
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd(hBipR, BT_BIP_OP_GET_IMG_PROPERTIES, rsp, sizeof(bt_bip_get_img_prop_rsp));
}

S32 btmtk_bipr_get_image(HMTKBIPR hBipR, bt_bip_get_img_rsp* rsp)
{
    if (!hBipR || !rsp)
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd(hBipR, BT_BIP_OP_GET_IMG, rsp, sizeof(bt_bip_get_img_rsp));
}

S32 btmtk_bipr_get_linked_thumbnail(HMTKBIPR hBipR, bt_bip_get_linked_thum_rsp* rsp)
{
    if (!hBipR || !rsp)
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd(hBipR, BT_BIP_OP_GET_LINKED_THUMBNAIL, rsp, sizeof(bt_bip_get_linked_thum_rsp));
}

S32 btmtk_bipr_put_image(HMTKBIPR hBipR, bt_bip_put_img_rsp* rsp)
{
    if (!hBipR || !rsp)
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd(hBipR, BT_BIP_OP_PUT_IMG, rsp, sizeof(bt_bip_put_img_rsp));
}

S32 btmtk_bipr_put_linked_thumbnail(HMTKBIPR hBipR, bt_bip_put_linked_thum_rsp* rsp)
{
    if (!hBipR || !rsp)
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd(hBipR, BT_BIP_OP_PUT_LINKED_THUMBNAIL, rsp, sizeof(bt_bip_put_linked_thum_rsp));
}

S32 btmtk_bipr_continue(HMTKBIPR hBipR, S32 cont)
{
    if (!hBipR)
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd(hBipR, BT_BIP_OP_CONTINUE, &cont, 4);
}

S32 btmtk_bipr_abort(HMTKBIPR hBipR)
{
    if (!hBipR)
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd(hBipR, BT_BIP_OP_ABORT, 0, 0);
}

S32 btmtk_bipr_disconnect(HMTKBIPR hBipR, U32 force)
{
    if (!hBipR)
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd(hBipR, BT_BIP_OP_DISCONNECT, &force, 4);
}

S32 btmtk_bipr_deactivate(HMTKBIPR hBipR)
{
    if (!hBipR)
        return BT_BIP_ERR_INVALID_PARA;

    return bipr_process_cmd(hBipR, BT_BIP_OP_DEACTIVATE, 0, 0);
}

#endif /* __BT_BIPI_PROFILE__ */
