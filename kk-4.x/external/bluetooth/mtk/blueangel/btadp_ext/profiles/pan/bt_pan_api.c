/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bt_pan_api.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is external adaptation layer for pan profie to send request
 *
 * Author:
 * -------
 * Ting Zheng
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#ifdef __BT_PAN_PROFILE__

#include <assert.h>

#include "bt_mmi.h"
#include "bt_struct.h"
//#include "bt_os_api.h" 
#include "ext_os_api.h"

#include "bt_message.h"
#ifdef BTMTK_ON_LINUX
#include "bt_ext_debug.h"
#include <errno.h>
#else
#include "bt_mmi_msg.h"
#endif

#include "bluetooth_pan_struct.h"
#include "bluetooth_pan_message.h"

#include "bt_pan_struct.h"
#include "bt_pan_api.h"

BTMTK_EventCallBack g_PAN_MMI_Callback;

#ifdef BTMTK_ON_LINUX
int g_panSockfd;
static void BTCMD_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size);
#endif


static void pan_process_cmd(bt_pan_op_struct *msg, U32 size)
{
#ifdef __BT_PAN_CMD_AGENT__
    BTCMD_SendMessage(MSG_ID_BT_PAN_CMD, MOD_EXTERNAL, msg, size);
#else
    pan_handle_cmd(msg);
#endif
}

void btmtk_pan_register_mmi_callback(BTMTK_EventCallBack callback)
{
    g_PAN_MMI_Callback = callback;
}

void btmtk_pan_activate_req(int service)
{
    bt_pan_op_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_pan_op_struct));
    msg.op = BT_PAN_OP_ACTIVATE;
    msg.service = service;
    pan_process_cmd(&msg, sizeof(bt_pan_op_struct));
}

void btmtk_pan_deactivate_req(void)
{
    bt_pan_op_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_pan_op_struct));
    msg.op = BT_PAN_OP_DEACTIVATE;
    pan_process_cmd(&msg, sizeof(bt_pan_op_struct));
}

void btmtk_pan_connect_req(bt_pan_service_enum local_service, bt_pan_service_enum remote_service, U8 *device_addr)
{
    bt_pan_op_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_pan_op_struct));
    msg.op = BT_PAN_OP_CONNECT;
    msg.local_service = local_service;
    msg.remote_service = remote_service;
    btmtk_os_memcpy((U8 *)&msg.addr, device_addr, sizeof(msg.addr));
	
    pan_process_cmd(&msg, sizeof(bt_pan_op_struct));
}

void btmtk_pan_disconnect_req(U8 *device_addr)
{
    bt_pan_op_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_pan_op_struct));
    msg.op = BT_PAN_OP_DISCONNECT;
    btmtk_os_memcpy((U8 *)&msg.addr, device_addr, sizeof(msg.addr));
	
    pan_process_cmd(&msg, sizeof(bt_pan_op_struct));
}

void btmtk_pan_connection_authorize_rsp(U8 *device_addr, BT_BOOL result)
{
    BT_PAN_DEVICE *dev = pan_find_device_by_addr(device_addr);
    bt_pan_connection_authorize_rsp_struct msg;

    assert(dev != NULL);
	
    /* even if authorization is deny, set state as connecting, and wait for disconnected event */
    pan_set_state(dev, BT_PAN_APP_STAT_CONNECTING);

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_pan_connection_authorize_rsp_struct));

    msg.accept = result;
    btmtk_os_memcpy((U8 *)&msg.bt_addr, device_addr, sizeof(msg.bt_addr));
    BTCMD_SendMessage(MSG_ID_BT_PAN_CONNECTION_AUTHORIZE_RSP, MOD_EXTERNAL, &msg, sizeof(msg));
}

void pan_op_activate(int service)
{
    bt_pan_activate_req_struct msg;
    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_pan_activate_req_struct));
    msg.service = service;
    BTCMD_SendMessage(MSG_ID_BT_PAN_ACTIVATE_REQ, MOD_EXTERNAL, &msg, sizeof(msg));
}

void pan_op_deactivate(void)
{
    BTCMD_SendMessage(MSG_ID_BT_PAN_DEACTIVATE_REQ, MOD_EXTERNAL, NULL, 0);
}

void pan_op_connect(bt_pan_service_enum local_service, bt_pan_service_enum remote_service, BD_ADDR *bt_addr)
{
    bt_pan_connect_req_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_pan_connect_req_struct));

    msg.local_service = local_service;
    msg.remote_service = remote_service;
    btmtk_os_memcpy((U8 *)&msg.bt_addr, bt_addr, sizeof(msg.bt_addr));
    BTCMD_SendMessage(MSG_ID_BT_PAN_CONNECT_REQ, MOD_EXTERNAL, &msg, sizeof(msg));
}

void pan_op_disconnect(BD_ADDR *bt_addr)
{
    bt_pan_disconnect_req_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_pan_disconnect_req_struct));
    btmtk_os_memcpy((U8 *)&msg.bt_addr, bt_addr, sizeof(msg.bt_addr));
    BTCMD_SendMessage(MSG_ID_BT_PAN_DISCONNECT_REQ, MOD_EXTERNAL, &msg, sizeof(msg));
}

#ifdef BTMTK_ON_LINUX
void btmtk_pan_set_sockfd(int sockfd)
{
    g_panSockfd = sockfd;
}

static void BTCMD_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size)
{
	int ret = -1;
	ilm_struct ilm;
	U16 *field;

	bt_ext_log("[SPP][ADP] BTCMD_SendMessage: %d", msg_id);

	memset(&ilm, 0, sizeof(ilm_struct));
	ilm.msg_id = msg_id;
	ilm.src_mod_id = MOD_EXTERNAL;
	ilm.dest_mod_id = dest_mod;

	assert (size <= MAX_ILM_BUFFER_SIZE);

	if(ptr != NULL)
		memcpy(ilm.ilm_data, ptr, size);

	/* LOCAL_PARA_HDR */
	field = (U16 *)ilm.ilm_data;
	*field = 1;
	field = (U16 *)&ilm.ilm_data[2];
	*field = size;

	// cal whole ilm size
	size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;

	if(g_panSockfd > 0)
	{
		ret = send(g_panSockfd, (void *)&ilm, size, 0);
		if(ret < 0)
		{
			bt_ext_err("[SPP][ADP] BTCMD_SendMessage: send fail: %s, %d", strerror(errno), errno);
		}
		bt_ext_log("[SPP][ADP] send done: %d", ret);
	}
	else
	{
		bt_ext_err("[SPP][ADP] BTCMD_SendMessage: socket not initialized");
	}
}
#endif

#endif
