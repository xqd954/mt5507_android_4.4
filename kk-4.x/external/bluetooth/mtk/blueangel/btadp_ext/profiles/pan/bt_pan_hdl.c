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
 * Bt_pan_hdl.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is external adaptation layer for pan profie to handle message
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

#include "bt_message.h"
//#include "bt_os_api.h" 
#include "ext_os_api.h"

#include "bluetooth_pan_message.h"
#include "bluetooth_pan_struct.h"

#include "bt_pan_api.h"
#include "bt_pan_hdl.h"
#include "bt_pan_struct.h"

#define bt_trace(...) ((void)0)

BT_PAN_APP_CTX g_pan_cntx;

static void pan_init(void)
{
    btmtk_os_memset((U8 *)&g_pan_cntx.device, 0, sizeof(g_pan_cntx.device));
}

void btmtk_deinit_pan(void)
{
    btmtk_os_memset((U8 *)&g_pan_cntx, 0, sizeof(g_pan_cntx));
    g_pan_cntx.enabled = FALSE;
    g_pan_cntx.do_disable = FALSE;
}

void btmtk_pan_handle_activate_cnf(void *msg)
{
    bt_pan_activate_cnf_struct *msg_p = (bt_pan_activate_cnf_struct *)msg;

    //kal_trace(BT_TRACE_G2_PROFILES, PAN_EXT_ADP_ACTIVATE_DONE);

    pan_init();

    if (msg_p->result)
    {
        g_pan_cntx.enabled = TRUE;
    }
    g_PAN_MMI_Callback(NULL, EVENT_PAN_MMI_ACTIVATE_CNF, msg, sizeof(bt_pan_activate_cnf_struct));
}

void btmtk_pan_handle_deactivate_cnf(void *msg)
{
    bt_pan_deactivate_cnf_struct *msg_p = (bt_pan_deactivate_cnf_struct *)msg;

    //kal_trace(BT_TRACE_G2_PROFILES, PAN_EXT_ADP_DEACTIVATE_DONE);
    g_pan_cntx.do_disable = FALSE;

    g_pan_cntx.enabled = FALSE;
    g_PAN_MMI_Callback(NULL, EVENT_PAN_MMI_DEACTIVATE_CNF, msg, sizeof(bt_pan_deactivate_cnf_struct));
}

void btmtk_pan_handle_connection_authorize_ind(void *msg)
{
    bt_pan_connection_authorize_ind_struct *msg_p = (bt_pan_connection_authorize_ind_struct *)msg;
    BT_PAN_DEVICE *dev = pan_find_free_device((U8 *)&msg_p->bt_addr);

    if (dev == NULL)
    {
        btmtk_pan_connection_authorize_rsp((U8 *)&msg_p->bt_addr, FALSE);
        return;
    }

    dev->addr = msg_p->bt_addr;
    pan_set_state(dev, BT_PAN_APP_STAT_AUTHORIZING);

    g_PAN_MMI_Callback(NULL, EVENT_PAN_MMI_CONNECTION_AUTHORIZE_IND, msg, sizeof(bt_pan_connection_authorize_ind_struct));
}

void btmtk_pan_handle_connect_ind(void *msg)
{
    bt_pan_connect_ind_struct *msg_p = (bt_pan_connect_ind_struct *)msg;
    BT_PAN_DEVICE *dev = pan_find_device_by_addr((U8 *)&msg_p->bt_addr);

    if (dev == NULL)
    {
        kal_trace(BT_TRACE_G2_PROFILES, PAN_EXT_ADP_HANDLE_CONNECT_IND_INVALID_DEVICE);
        return;
    }

    dev->addr = msg_p->bt_addr;
		
    if ((g_pan_cntx.do_disable) ||
         (dev->state == BT_PAN_APP_STAT_CONN_CANCEL))
    {
        pan_op_disconnect(&msg_p->bt_addr);
        pan_set_state(dev, BT_PAN_APP_STAT_DISCONNECTING);
    }
    else
    {
        kal_trace(BT_TRACE_G2_PROFILES, PAN_EXT_ADP_HANDLE_CONNECT_IND_PREV_STATE, dev->state);
        pan_set_state(dev, BT_PAN_APP_STAT_CONNECTED);
        g_PAN_MMI_Callback(NULL, EVENT_PAN_MMI_CONNECT_IND, msg, sizeof(bt_pan_connect_ind_struct));
    }
}

void btmtk_pan_handle_connect_cnf(void *msg)
{
    bt_pan_connect_cnf_struct *msg_p = (bt_pan_connect_cnf_struct *)msg;
    BT_PAN_DEVICE *dev = pan_find_device_by_addr((U8 *)&msg_p->bt_addr);

    if (dev == NULL)
    {
        kal_trace(BT_TRACE_G2_PROFILES, PAN_EXT_ADP_HANDLE_CONNECT_CNF_INVALID_DEVICE);
        return;
    }

    dev->addr = msg_p->bt_addr;
		
    if ((g_pan_cntx.do_disable) ||
         (dev->state == BT_PAN_APP_STAT_CONN_CANCEL))
    {
        pan_op_disconnect(&msg_p->bt_addr);
        pan_set_state(dev, BT_PAN_APP_STAT_DISCONNECTING);
    }
    else
    {
        kal_trace(BT_TRACE_G2_PROFILES, PAN_EXT_ADP_HANDLE_CONNECT_CNF_PREV_STATE, dev->state);
        pan_set_state(dev, BT_PAN_APP_STAT_CONNECTED);
        g_PAN_MMI_Callback(NULL, EVENT_PAN_MMI_CONNECT_CNF, msg, sizeof(bt_pan_connect_cnf_struct));
    }
}

void btmtk_pan_handle_disconnect_ind(void *msg)
{
    bt_pan_disconnect_ind_struct *msg_p = (bt_pan_disconnect_ind_struct *)msg;
    BT_PAN_DEVICE *dev = pan_find_device_by_addr((U8 *)&msg_p->bt_addr);

    if (dev == NULL)
    {
        return;
    }
	
    if (g_pan_cntx.do_disable)
    {
        pan_set_state(dev, BT_PAN_APP_STAT_NO_CONNECTION);
        pan_deactivate();
    }
    else
    {
        kal_trace(BT_TRACE_G2_PROFILES, PAN_EXT_ADP_HANDLE_DISCONNECT_IND_PREV_STATE, dev->state);
        pan_set_state(dev, BT_PAN_APP_STAT_NO_CONNECTION);
        g_PAN_MMI_Callback(NULL, EVENT_PAN_MMI_DISCONNECT_IND, msg, sizeof(bt_pan_disconnect_ind_struct));
    }
}

void btmtk_pan_handle_disconnect_cnf(void *msg)
{
    bt_pan_disconnect_cnf_struct *msg_p = (bt_pan_disconnect_cnf_struct *)msg;
    BT_PAN_DEVICE *dev = pan_find_device_by_addr((U8 *)&msg_p->bt_addr);

    if (dev == NULL)
    {
        return;
    }
	
    if (g_pan_cntx.do_disable)
    {
        pan_set_state(dev, BT_PAN_APP_STAT_NO_CONNECTION);
        g_PAN_MMI_Callback(NULL, EVENT_PAN_MMI_DISCONNECT_CNF, msg, sizeof(bt_pan_disconnect_cnf_struct));
        pan_deactivate();
    }
    else
    {
        kal_trace(BT_TRACE_G2_PROFILES, PAN_EXT_ADP_HANDLE_DISCONNECT_CNF_PREV_STATE, dev->state);
        pan_set_state(dev, BT_PAN_APP_STAT_NO_CONNECTION);
        g_PAN_MMI_Callback(NULL, EVENT_PAN_MMI_DISCONNECT_CNF, msg, sizeof(bt_pan_disconnect_cnf_struct));
    }
}

void btmtk_pan_handle_message(ilm_struct *msg)
{
    void *msg_p = NULL;  //point to local parameter in ilm_struct

#ifdef BTMTK_ON_WISE
    msg_p = (void *)msg->local_para_ptr;
#else 
    msg_p = (void *)msg->ilm_data;
#endif

    switch (msg->msg_id)
    {
#ifdef __BT_PAN_CMD_AGENT__
        case MSG_ID_BT_PAN_CMD:
            pan_handle_cmd((bt_pan_op_struct *)msg_p);
            break;			
#endif

        case MSG_ID_BT_PAN_ACTIVATE_CNF:
            btmtk_pan_handle_activate_cnf(msg_p);			
            break;			

        case MSG_ID_BT_PAN_DEACTIVATE_CNF:
            btmtk_pan_handle_deactivate_cnf(msg_p);			
            break;			

        case MSG_ID_BT_PAN_CONNECTION_AUTHORIZE_IND:
            btmtk_pan_handle_connection_authorize_ind(msg_p);			
            break;			

        case MSG_ID_BT_PAN_CONNECT_IND:
            btmtk_pan_handle_connect_ind(msg_p);			
            break;			

        case MSG_ID_BT_PAN_CONNECT_CNF:
            btmtk_pan_handle_connect_cnf(msg_p);			
            break;			

        case MSG_ID_BT_PAN_DISCONNECT_IND:
            btmtk_pan_handle_disconnect_ind(msg_p);			
            break;			

        case MSG_ID_BT_PAN_DISCONNECT_CNF:
            btmtk_pan_handle_disconnect_cnf(msg_p);			
            break;			

        default:			
            break;			
    }
}

void pan_deactivate(void)
{
    BT_PAN_DEVICE *dev_conn = NULL;
	
    /* disconnect all the other devices before disable the service */
    if ((dev_conn = pan_find_connected_device()) != NULL)
    {
        if (dev_conn->state == BT_PAN_APP_STAT_CONNECTED)
        {
            // deactivate later, after the connection is disconnected
            pan_op_disconnect(&dev_conn->addr);
            pan_set_state(dev_conn, BT_PAN_APP_STAT_DISCONNECTING);
        }
        else if (dev_conn->state == BT_PAN_APP_STAT_AUTHORIZING)
        {
            // deactivate later, after the connection is rejected
            btmtk_pan_connection_authorize_rsp((U8 *)&dev_conn->addr, FALSE);
        }
        else 
        {
            // deactivate later, after the connection is done or disconnection is done
            // BT_PAN_APP_STAT_CONNECTING
            // BT_PAN_APP_STAT_DISCONNECTING
            // BT_PAN_APP_STAT_CONNCANCEL
        }
    }
    else
    {
        pan_op_deactivate();
    }

}

static void pan_handle_activate_cmd(int service)
{
    if (!g_pan_cntx.enabled)
    {
        pan_op_activate(service);
    }
    else 
    {
        bt_pan_activate_cnf_struct msg;
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_pan_activate_cnf_struct));		
        msg.result = TRUE;
        g_PAN_MMI_Callback(NULL, EVENT_PAN_MMI_ACTIVATE_CNF, &msg, sizeof(bt_pan_activate_cnf_struct));
    }
}

static void pan_handle_deactivate_cmd(void)
{
    if (g_pan_cntx.enabled)
    {
        g_pan_cntx.do_disable = TRUE;
        pan_deactivate();
    }
    else
    {
        bt_pan_deactivate_cnf_struct msg;
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_pan_deactivate_cnf_struct));		
        msg.result = TRUE;
        g_PAN_MMI_Callback(NULL, EVENT_PAN_MMI_DEACTIVATE_CNF, &msg, sizeof(bt_pan_deactivate_cnf_struct));
    }
}

static void pan_handle_connect_cmd(bt_pan_service_enum local_service, bt_pan_service_enum remote_service, BD_ADDR *bt_addr)
{
    BT_PAN_DEVICE *dev = pan_find_free_device(bt_addr->addr);

    if (!g_pan_cntx.enabled || (dev == NULL))
    {
        bt_pan_connect_cnf_struct msg;
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_pan_connect_cnf_struct));		
        msg.local_service = local_service;
        msg.remote_service = remote_service;
        msg.bt_addr = *bt_addr;
        g_PAN_MMI_Callback(NULL, EVENT_PAN_MMI_CONNECT_CNF, &msg, sizeof(bt_pan_connect_cnf_struct));
        return;				
    }
	
    if (dev->state == BT_PAN_APP_STAT_NO_CONNECTION)
    {
        pan_op_connect(local_service, remote_service, bt_addr);
        dev->addr = *bt_addr;
        pan_set_state(dev, BT_PAN_APP_STAT_CONNECTING);
    }
    else
    {
        // do nothing, ignore the request
        kal_trace(BT_TRACE_G2_PROFILES, PAN_EXT_ADP_HANDLE_CONNECT_CMD, (U8)dev->state);
    }

}

static void pan_handle_disconnect_cmd(BD_ADDR *bt_addr)
{
    BT_PAN_DEVICE *dev = pan_find_device_by_addr(bt_addr->addr);
	
    if (!g_pan_cntx.enabled || (dev == NULL))
    {
        bt_pan_disconnect_cnf_struct msg;
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_pan_disconnect_cnf_struct));		
        msg.bt_addr = *bt_addr;
        g_PAN_MMI_Callback(NULL, EVENT_PAN_MMI_DISCONNECT_CNF, &msg, sizeof(bt_pan_disconnect_cnf_struct));
        return;				
    }
	
    if (dev->state == BT_PAN_APP_STAT_CONNECTED)
    {
        pan_op_disconnect(bt_addr);
        pan_set_state(dev, BT_PAN_APP_STAT_DISCONNECTING);
    }
    else if (dev->state == BT_PAN_APP_STAT_AUTHORIZING)
    {
        btmtk_pan_connection_authorize_rsp(bt_addr->addr, FALSE);
        pan_set_state(dev, BT_PAN_APP_STAT_NO_CONNECTION);
    }
    else if (dev->state == BT_PAN_APP_STAT_CONNECTING)	
    {
        pan_set_state(dev, BT_PAN_APP_STAT_CONN_CANCEL);
    }
    else
    {
        //do nothing, ignore the request
        kal_trace(BT_TRACE_G2_PROFILES, PAN_EXT_ADP_HANDLE_DISCONNECT_CMD, (U8)dev->state);
    }
}

void pan_handle_cmd(bt_pan_op_struct *cmd)
{
    kal_trace(BT_TRACE_G2_PROFILES, PAN_EXT_ADP_HANDLE_CMD, cmd->op);

    switch (cmd->op)
    {
        case BT_PAN_OP_ACTIVATE:
            pan_handle_activate_cmd(cmd->service);
            break;	
			
        case BT_PAN_OP_DEACTIVATE:
            pan_handle_deactivate_cmd();
            break;	
			
        case BT_PAN_OP_CONNECT:
            pan_handle_connect_cmd(cmd->local_service, cmd->remote_service, &cmd->addr);
            break;	
			
        case BT_PAN_OP_DISCONNECT:
            pan_handle_disconnect_cmd(&cmd->addr);
            break;			
			
        default:
            break;			
    }
}

BT_PAN_DEVICE *pan_find_device_by_addr(U8 *device_addr)
{
    BT_PAN_DEVICE *dev = NULL;
    U8 i = 0;

    for (i = 0; i < PAN_MAX_DEV_NUM; i++)
    {
        if (PAN_DEVICE_ADDR_EQUAL((U8 *)&g_pan_cntx.device[i].addr, device_addr)) 
        {
            dev = &g_pan_cntx.device[i];
            break;
        }
    }
    return dev;	
}

BT_PAN_DEVICE *pan_find_free_device(U8 *device_addr)
{
    BT_PAN_DEVICE *dev = NULL;
    U8 i = 0;

    if ((dev = pan_find_device_by_addr(device_addr)) != NULL)
    {
        // if it is already in the device list, just pick up
        return dev;
    }
    
    for (i = 0; i < PAN_MAX_DEV_NUM; i++)
    {
        if (g_pan_cntx.device[i].state == BT_PAN_APP_STAT_NO_CONNECTION)
        {
            dev = &g_pan_cntx.device[i];
            break;			
        }
    }

    return dev;	
}

BT_PAN_DEVICE *pan_find_connected_device(void)
{
    BT_PAN_DEVICE *dev = NULL;
    U8 i = 0;

    for (i = 0; i < PAN_MAX_DEV_NUM; i++)
    {
        if (g_pan_cntx.device[i].state != BT_PAN_APP_STAT_NO_CONNECTION)
        {
            dev = &g_pan_cntx.device[i];
        }
    }
	
    return dev;
}

void pan_set_state(BT_PAN_DEVICE *dev, BT_PAN_APP_STATE state)
{
    if (dev == NULL)
    {
        return;
    }
    dev->state = state;
}
#endif
