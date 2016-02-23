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
/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bt_hid_hdl.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to dispatch message from BT stack
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

#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bt_types.h"
#include "bluetooth_hid_message.h"
#include "bluetooth_hid_struct.h"
#include "bt_hid_api.h"
#include "bt_hid_hdl.h"

#include "bt_os_api.h"
#include "bt_ext_debug.h"


typedef enum
{
    BT_HID_APP_STAT_NO_CONNECTION = 0,
    BT_HID_APP_STAT_CONNECTED,

    BT_HID_APP_STAT_CONNECTING,
    BT_HID_APP_STAT_DISCONNECTING,
    BT_HID_APP_STAT_AUTHORIZING,
    BT_HID_APP_STAT_WAITCONNECT,
    BT_HID_APP_STAT_CONNCANCEL,
} BT_HID_APP_STATE;


typedef struct _BT_HID_DEVICE
{
    BD_ADDR  addr;
    U32 	 conn_id;
    BT_HID_APP_STATE	 state;
    U8		trans_protocol;
} BT_HID_DEVICE;

//HID Device role context
typedef struct _BT_HIDD_APP_CTX
{
    BT_BOOL			enabled;
    BT_BOOL			do_disable;
    BT_HID_DEVICE   		device;
    //U8				authorise; 
    //BT_HIDD_SDP_ATTRIBUTE	sdp_attribute;  
} BT_HIDD_APP_CTX;


//HID Host role context
typedef struct _BT_HIDH_APP_CTX
{
    BT_BOOL	enabled;
    BT_BOOL	do_disable;
    void	*pfnRxReportCb;
    BT_HID_DEVICE  device[HID_MAX_DEV_NUM];
} BT_HIDH_APP_CTX;


BT_HIDD_APP_CTX g_hidd_cntx;
BT_HIDH_APP_CTX g_hidh_cntx;
U32		g_conn_id;
static int capsKeyPress = 1;
static int numKeyPress = 1;
static int scrollKeyPress = 1;
extern BTMTK_HID_CALLBACK g_hid_ui_callback;



extern void btmtk_util_convert_mbtarray2mtkarray(U8 *dest, U8 *source);

static void hidh_init(void);
static void hidh_deactivate_req(void);
//static BT_HID_DEVICE *hidh_find_device_by_addr(U8 *device_addr);
static BT_HID_DEVICE *hidh_find_device_by_addr(BD_ADDR device_addr);
//static BT_HID_DEVICE *hidh_find_free_device(U8 *device_addr);
static BT_HID_DEVICE *hidh_find_free_device(BD_ADDR device_addr);
static BT_HID_DEVICE *hidh_find_connected_device(void);

static void hid_set_device_state(BT_HID_APP_STATE state);
static void hid_set_host_state(BT_HID_DEVICE *dev, BT_HID_APP_STATE state);

BT_BOOL btmtk_hid_is_dev_connected(BD_ADDR RemoteBDAddr)
{
    BT_HID_APP_STATE state;

    if (g_hidd_cntx.enabled)
    {
        state = g_hidd_cntx.device.state;
    }
    else if (g_hidh_cntx.enabled)
    {
        //BT_HID_DEVICE *dev = hidh_find_device_by_addr((U8 *)&RemoteBDAddr);
        BT_HID_DEVICE *dev = hidh_find_device_by_addr(RemoteBDAddr);
        if (dev != NULL)
        {
            state = dev->state;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }

    if (state == BT_HID_APP_STAT_CONNECTED)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BT_BOOL btmtk_hid_is_connected()
{
    if (g_hidd_cntx.enabled)
    {
        if (g_hidd_cntx.device.state == BT_HID_APP_STAT_CONNECTED)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else if (g_hidh_cntx.enabled)
    {
        U8 i = 0;
        U8 max_dev_num = HID_MAX_DEV_NUM;

        for (i = 0; i < max_dev_num; i++)
        {
            if (g_hidh_cntx.device[i].state == BT_HID_APP_STAT_CONNECTED)
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void btmtk_hid_handle_message(void *pContext, ilm_struct *msg)
{

    HID_LOG("[BT-HID]btmtk_hid_handle_message start.");
    void *msg_p = NULL;  //point to local parameter in ilm_struct

#ifdef BTMTK_ON_WISE
    msg_p = (void *)msg->local_para_ptr;
#else 
    msg_p = (void *)msg->ilm_data;
#endif

   
    switch (msg->msg_id)
    {
/**************************** Device Role *****************************/
        case MSG_ID_BT_HIDD_REQ_EXT:
            btmtk_hidd_handle_req_ext(msg_p);
            break;			
	
        case MSG_ID_BT_HIDD_ACTIVATE_CNF:
            btmtk_hidd_handle_activate_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDD_DEACTIVATE_CNF:
            btmtk_hidd_handle_deactivate_cnf(msg_p);
            break;			

        case MSG_ID_BT_HIDD_CONNECT_CNF:
            btmtk_hidd_handle_connect_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDD_DISCONNECT_CNF:
            btmtk_hidd_handle_disconnect_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDD_UNPLUG_CNF:
            btmtk_hidd_handle_unplug_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDD_INTERRUPT_DATA_CNF:
            btmtk_hidd_handle_input_report_cnf(msg_p);
            break;			

        case MSG_ID_BT_HIDD_CONNECTION_AUTHORIZE_IND:
            btmtk_hidd_handle_authorize_ind(msg_p);
            break;

        case MSG_ID_BT_HIDD_CONNECT_IND:
            btmtk_hidd_handle_connect_ind(msg_p);
            break;

        case MSG_ID_BT_HIDD_DISCONNECT_IND:
            btmtk_hidd_handle_disconnect_ind(msg_p);
            break;

        case MSG_ID_BT_HIDD_CONTROL_IND:
            btmtk_hidd_handle_control_ind(msg_p);
            break;

        case MSG_ID_BT_HIDD_SET_REPORT_IND:
            btmtk_hidd_handle_set_report_ind(msg_p);
            break;

        case MSG_ID_BT_HIDD_INTERRUPT_DATA_IND:
            btmtk_hidd_handle_output_report_ind(msg_p);
            break;

/**************************** Host Role *****************************/

        case MSG_ID_BT_HIDH_REQ_EXT:
	     if(msg->dest_mod_id == MOD_MMI)
	     {    
	         bt_hidh_req_ext_struct *msg_t = (bt_hidh_req_ext_struct *)msg_p;

		  if(g_hid_ui_callback != NULL)
		  	g_hid_ui_callback(msg_t->command, &msg_t->addr, NULL);
	     }
            else
                 btmtk_hidh_handle_req_ext(msg_p);
            break;			
	
        case MSG_ID_BT_HIDH_ACTIVATE_CNF:
            btmtk_hidh_handle_activate_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDH_DEACTIVATE_CNF:
            btmtk_hidh_handle_deactivate_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDH_CONNECT_CNF:
            btmtk_hidh_handle_connect_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDH_DISCONNECT_CNF:
            btmtk_hidh_handle_disconnect_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDH_DESCINFO_CNF:
            btmtk_hidh_handle_descinfo_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDH_SEND_CONTROL_CNF:
            btmtk_hidh_handle_control_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDH_SET_REPORT_CNF:
            btmtk_hidh_handle_set_report_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDH_GET_REPORT_CNF:
            btmtk_hidh_handle_get_report_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDH_SET_PROTOCOL_CNF:
            btmtk_hidh_handle_set_protocol_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDH_GET_PROTOCOL_CNF:
            btmtk_hidh_handle_get_protocol_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDH_SET_IDLE_RATE_CNF:
            btmtk_hidh_handle_set_idle_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDH_GET_IDLE_RATE_CNF:
            btmtk_hidh_handle_get_idle_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDH_INTERRUPT_DATA_CNF:
            btmtk_hidh_handle_output_report_cnf(msg_p);
            break;

        case MSG_ID_BT_HIDH_CONNECTION_AUTHORIZE_IND:
            btmtk_hidh_handle_authorize_ind(msg_p);
            break;

        case MSG_ID_BT_HIDH_CONNECT_IND:
            btmtk_hidh_handle_connect_ind(msg_p);
            break;

        case MSG_ID_BT_HIDH_DISCONNECT_IND:
            btmtk_hidh_handle_disconnect_ind(msg_p);
            break;

        case MSG_ID_BT_HIDH_UNPLUG_IND:
            btmtk_hidh_handle_unplug_ind(msg_p);
            break;

        case MSG_ID_BT_HIDH_INTERRUPT_DATA_IND:
            btmtk_hidh_handle_input_report_ind(msg_p);
            break;

        default:
            break;
    }
}

/************************************************************************
*								Device Role								*
*************************************************************************/
static void hidd_init(void)
{
    memset((U8 *)&g_hidd_cntx, 0, sizeof(g_hidd_cntx));
    g_hidd_cntx.device.conn_id = HID_INVALID_CONN_ID;
    g_hidd_cntx.device.trans_protocol = hidd_protocol_report;
	
}

void btmtk_deinit_hid_device(void)
{
    memset((U8 *)&g_hidd_cntx, 0, sizeof(g_hidd_cntx));
    g_hidd_cntx.enabled = FALSE;
    g_hidd_cntx.do_disable = FALSE;
    g_hidd_cntx.device.conn_id = HID_INVALID_CONN_ID;
}

void btmtk_hidd_handle_req_ext(void *msg)
{
    bt_hidd_req_ext_struct *msg_p = (bt_hidd_req_ext_struct *)msg;

    switch (msg_p->command)
    {
        case BT_HIDD_ACTIVATE_REQ_EXT:
            if (!g_hidd_cntx.enabled)
            {
                btmtk_hidd_activate_req();
            }
            else
            {
                // do nothing, only notify done
                btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_ENABLE_SUCCESS, 0, NULL);
            }
            break;			

        case BT_HIDD_DEACTIVATE_REQ_EXT:
            if (g_hidd_cntx.enabled)
            {
                g_hidd_cntx.do_disable = TRUE;
    
                if (g_hidd_cntx.device.state == BT_HID_APP_STAT_NO_CONNECTION)
                {
                    btmtk_hidd_deactivate_req();
                }
                else if (g_hidd_cntx.device.state == BT_HID_APP_STAT_CONNECTED)
                {
                    // deactivate later, after the connection is disconnected
                    btmtk_hidd_disconnect_req((U8 *)&g_hidd_cntx.device.addr, g_hidd_cntx.device.conn_id);
                    hid_set_device_state(BT_HID_APP_STAT_DISCONNECTING);
                }
                else if (g_hidd_cntx.device.state == BT_HID_APP_STAT_AUTHORIZING)
                {
                    // deactivate later, after the connection is rejected
                    btmtk_hidd_authorize_rsp((U8 *)&g_hidd_cntx.device.addr, g_hidd_cntx.device.conn_id, FALSE);
                }
                else 
                {
                    // deactivate later, after the connection is done or disconnection is done
                    // BT_HID_APP_STAT_CONNECTING, BT_HID_APP_STAT_WAITCONNECT
                    // BT_HID_APP_STAT_DISCONNECTING, BT_HID_APP_STAT_CONNCANCEL,
                    HID_ERR(" deactive req error state: %d", (U8)g_hidd_cntx.device.state);
                }
            }
            else
            {
                btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_DISABLE_SUCCESS, 0, NULL);
            }
            break;			

        case BT_HIDD_CONNECT_REQ_EXT:
            if (!g_hidd_cntx.enabled)
            {
                btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_CONNECT_FAIL, 0, NULL);
                break;				
            }
            
            if (g_hidd_cntx.device.state == BT_HID_APP_STAT_NO_CONNECTION)
            {
                btmtk_hidd_connect_req((U8 *)&msg_p->addr);
                hid_set_device_state(BT_HID_APP_STAT_CONNECTING);
            }
            else
            {
                // do nothing, only notify the failure
                //btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_CONNECT_FAIL, 0);
                HID_ERR(" connect req error state: %d", (U8)g_hidd_cntx.device.state);
            }
            break;			

        case BT_HIDD_DISCONNECT_REQ_EXT:
            if (!g_hidd_cntx.enabled)
            {
                btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_DISCONNECT_FAIL, 0, NULL);
                break;				
            }
            
            if (g_hidd_cntx.device.state == BT_HID_APP_STAT_CONNECTED)
            {
                btmtk_hidd_disconnect_req((U8 *)&g_hidd_cntx.device.addr, g_hidd_cntx.device.conn_id);
                hid_set_device_state(BT_HID_APP_STAT_DISCONNECTING);
            }
            else if (g_hidd_cntx.device.state == BT_HID_APP_STAT_AUTHORIZING)
            {
                btmtk_hidd_authorize_rsp((U8 *)&g_hidd_cntx.device.addr, g_hidd_cntx.device.conn_id, FALSE);
                hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
            }
            else if ((g_hidd_cntx.device.state == BT_HID_APP_STAT_WAITCONNECT) ||
                          (g_hidd_cntx.device.state == BT_HID_APP_STAT_CONNECTING))	
            {
                hid_set_device_state(BT_HID_APP_STAT_CONNCANCEL);
            }
            else
            {
                //btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_DISCONNECT_FAIL, 0);
                HID_ERR(" disconnect req error state: %d", (U8)g_hidd_cntx.device.state);
            }
            break;			

        default:
            break;			
    }
}

void btmtk_hidd_handle_activate_cnf(void *msg)
{
    bt_hidd_activate_cnf_struct *msg_p = (bt_hidd_activate_cnf_struct*) msg;

    hidd_init();
    
   
    if(msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_ENABLE_SUCCESS, 0, NULL);

        g_hidd_cntx.enabled = TRUE;
    }
    else
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_ENABLE_FAIL, 0, NULL);
    }

    HID_MEM_FREE(msg_p->sdpAttr.DescStr);
}


void btmtk_hidd_handle_deactivate_cnf(void *msg)
{
    bt_hidd_deactivate_cnf_struct *msg_p = (bt_hidd_deactivate_cnf_struct *)msg;

    g_hidd_cntx.do_disable = FALSE;

    if (msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_DISABLE_SUCCESS, 0, NULL);

        g_hidd_cntx.enabled = FALSE;
    }
    else
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_DISABLE_FAIL, 0, NULL);
    }

}

void btmtk_hidd_handle_connect_cnf(void *msg)
{
    bt_hidd_connect_cnf_struct *msg_p = (bt_hidd_connect_cnf_struct *)msg;

    if (msg_p->result == hidd_result_ok)
    {
        g_hidd_cntx.device.addr = msg_p->bt_addr;
        g_hidd_cntx.device.conn_id = msg_p->connection_id;


        if ((g_hidd_cntx.do_disable) ||
              (g_hidd_cntx.device.state == BT_HID_APP_STAT_CONNCANCEL))
        {
            //hid_set_device_state(BT_HID_APP_STAT_CONNECTED);
            btmtk_hidd_disconnect_req((U8 *)&msg_p->bt_addr, msg_p->connection_id);
            hid_set_device_state(BT_HID_APP_STAT_DISCONNECTING);
        }
        else if (g_hidd_cntx.device.state == BT_HID_APP_STAT_CONNECTING)
        {
            /* normal connect process done */ 
            hid_set_device_state(BT_HID_APP_STAT_CONNECTED);
			
            //btmtk_os_memcpy((U8 *)&sdcHidDStatus->BDAddr, (U8 *)&msg_p->bt_addr, sizeof(sdcHidDStatus->BDAddr));
            btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_CONNECT_SUCCESS, 0, NULL);
        }
        else
        {
            HID_ERR(" device handle connect cnf -- succ: error state transaction, curstate: %d", g_hidd_cntx.device.state);
            hid_set_device_state(BT_HID_APP_STAT_CONNECTED);
        }
    }
    else
    {
        if (g_hidd_cntx.do_disable)
        {
            hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
            btmtk_hidd_deactivate_req();
        }
        else
        {
            if ((g_hidd_cntx.device.state == BT_HID_APP_STAT_CONNECTING) ||
                 (g_hidd_cntx.device.state == BT_HID_APP_STAT_CONNCANCEL))
            {
               hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
    		   
               btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_CONNECT_FAIL, 0, NULL);
            }
            else
            {
                HID_ERR(" device handle connect cnf -- fail: error state transaction, curstate: %d", g_hidd_cntx.device.state);
                hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
            }
        }
    }

}

void btmtk_hidd_handle_disconnect_cnf(void *msg)
{
    bt_hidd_disconnect_cnf_struct *msg_p = (bt_hidd_disconnect_cnf_struct *)msg;

    g_hidd_cntx.device.conn_id = HID_INVALID_CONN_ID;	// set to invalid connection id

    if (g_hidd_cntx.do_disable)
    {
        hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
        btmtk_hidd_deactivate_req();
    }
    else 
    {
        if (g_hidd_cntx.device.state == BT_HID_APP_STAT_DISCONNECTING)
        {
            hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
            btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_DISCONNECT_SUCCESS, 0, NULL);
        }
        else
        {
            HID_ERR(" device handle disconnect cnf -- error state transaction, curstate: %d", g_hidd_cntx.device.state);
            hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
        }
    }

}

void btmtk_hidd_handle_unplug_cnf(void *msg)
{
    bt_hidd_unplug_cnf_struct *msg_p = (bt_hidd_unplug_cnf_struct *)msg;

    if (msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_SEND_UNPLUG_SUCCESS, 0, NULL);
    }
    else
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_SEND_UNPLUG_FAIL, 0, NULL);
    }
}

void btmtk_hidd_handle_input_report_cnf(void *msg)
{
    bt_hidd_interrupt_data_cnf_struct *msg_p = (bt_hidd_interrupt_data_cnf_struct *)msg;

    if (msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_SEND_REPORT_SUCCESS, 0, NULL);
    }
    else
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_SEND_REPORT_FAIL, 0, NULL);
    }
}

void btmtk_hidd_handle_authorize_ind(void *msg)
{
    bt_hidd_connection_authorize_ind_struct *msg_p = (bt_hidd_connection_authorize_ind_struct *)msg;
    hid_set_device_state(BT_HID_APP_STAT_AUTHORIZING);

    // accept authorization as default
    btmtk_hidd_authorize_rsp((U8 *)&msg_p->bt_addr, msg_p->connection_id, TRUE);

    hid_set_device_state(BT_HID_APP_STAT_WAITCONNECT);
}

void btmtk_hidd_handle_connect_ind(void *msg)
{
    bt_hidd_connect_ind_struct *msg_p = (bt_hidd_connect_ind_struct *)msg;

    if (msg_p->result == hidd_result_ok)
    {
        g_hidd_cntx.device.addr = msg_p->bt_addr;
        g_hidd_cntx.device.conn_id = msg_p->connection_id;
		
        if ((g_hidd_cntx.do_disable) ||
              (g_hidd_cntx.device.state == BT_HID_APP_STAT_CONNCANCEL))
        {
            //hid_set_device_state(BT_HID_APP_STAT_CONNECTED);
            btmtk_hidd_disconnect_req((U8 *)&msg_p->bt_addr, msg_p->connection_id);
            hid_set_device_state(BT_HID_APP_STAT_DISCONNECTING);
        }
        else
        {
            if ((g_hidd_cntx.device.state == BT_HID_APP_STAT_NO_CONNECTION) ||
                 (g_hidd_cntx.device.state == BT_HID_APP_STAT_WAITCONNECT))		
            {
                 /* normal connect process done */ 
                 hid_set_device_state(BT_HID_APP_STAT_CONNECTED);
                           
                 btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_CONNECT_SUCCESS, 0, NULL);
            }
            else
            {
                 HID_ERR(" device handle connect ind  -- suc: error state transaction, curstate: %d", g_hidd_cntx.device.state);
                 hid_set_device_state(BT_HID_APP_STAT_CONNECTED);
            }
        }
    }
    else
    {	
        if (g_hidd_cntx.do_disable)
        {
            hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
            btmtk_hidd_deactivate_req();
        }
        else
        {
            if ((g_hidd_cntx.device.state == BT_HID_APP_STAT_NO_CONNECTION) ||
                 (g_hidd_cntx.device.state == BT_HID_APP_STAT_AUTHORIZING) ||			
    	        (g_hidd_cntx.device.state == BT_HID_APP_STAT_WAITCONNECT) ||
                 (g_hidd_cntx.device.state == BT_HID_APP_STAT_CONNCANCEL))
            {
                hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
    			
                //memset(sdcHidDStatus->BDAddr, 0, sizeof(sdcHidDStatus->BDAddr));
                btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_CONNECT_FAIL, 0, NULL);
            }
            else
            {
                HID_ERR(" device handle connect ind -- fail: error state transaction, curstate: %d", g_hidd_cntx.device.state);
                hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
            }
        }
    }

}

void btmtk_hidd_handle_disconnect_ind(void *msg)
{
    bt_hidd_disconnect_ind_struct *msg_p = (bt_hidd_disconnect_ind_struct *)msg;

    g_hidd_cntx.device.conn_id = HID_INVALID_CONN_ID;	// set to invalid connection id

    if (g_hidd_cntx.do_disable)
    {
        hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
        btmtk_hidd_deactivate_req();
    }
    else
    {
        if (g_hidd_cntx.device.state == BT_HID_APP_STAT_CONNECTED)
        {
            hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
            btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_DISCONNECT_SUCCESS, 0, NULL);
        }
        else
        {
            HID_ERR(" device handle disconnect ind  -- error state transaction, curstate: %d", g_hidd_cntx.device.state);
            hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
        }
    }

}

void btmtk_hidd_handle_control_ind(void *msg)
{
    bt_hidd_control_ind_struct *msg_p = (bt_hidd_control_ind_struct *)msg;

    if (msg_p->ctrl_op == hidd_ctrl_op_virtual_cable_unplug)
    {
        btmtk_hidd_disconnect_req((U8 *)&g_hidd_cntx.device.addr, g_hidd_cntx.device.conn_id);
        hid_set_device_state(BT_HID_APP_STAT_DISCONNECTING);
    }
    else
    {
        HID_LOG("device handle control ind  -- op: %d", msg_p->ctrl_op);     
    }

    btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_RECEIVE_CONTROL, 0, NULL);
}

void btmtk_hidd_handle_set_report_ind(void *msg)
{
    bt_hidd_set_report_ind_struct *msg_p = (bt_hidd_set_report_ind_struct *)msg;

    btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_RECEIVE_REPORT, 0, NULL);

    HID_MEM_FREE(msg_p->data_ptr);
}

void btmtk_hidd_handle_output_report_ind(void *msg)
{
    bt_hidd_interrupt_data_ind_struct *msg_p = (bt_hidd_interrupt_data_ind_struct *)msg;

    btmtk_hidh_UI_callback(MBTEVT_HID_DEVICE_RECEIVE_REPORT, 0, NULL);

    HID_MEM_FREE(msg_p->data_ptr);
}

/************************************************************************
*								Host Role									*
*************************************************************************/

static void hidh_init(void)
{
    U8 i;

    memset((U8 *)&g_hidh_cntx.device, 0, sizeof(g_hidh_cntx.device));
    for (i = 0; i < HID_MAX_DEV_NUM; i++)
    {
        g_hidh_cntx.device[i].conn_id = HID_INVALID_CONN_ID;
        g_hidh_cntx.device[i].trans_protocol = hidd_protocol_report;
    }

}

void btmtk_deinit_hid_host(void)
{
    U8 i;

    memset((U8 *)&g_hidh_cntx, 0, sizeof(g_hidh_cntx));
    g_hidh_cntx.enabled = FALSE;
    g_hidh_cntx.do_disable = FALSE;
	
    for (i = 0; i < HID_MAX_DEV_NUM; i++)
    {
        g_hidh_cntx.device[i].conn_id = HID_INVALID_CONN_ID;
    }
}

void btmtk_hidh_handle_req_ext(void *msg)
{
    HID_LOG("btmtk_hidh_handle_req_ext");
    bt_hidh_req_ext_struct *msg_p = (bt_hidh_req_ext_struct *)msg;

    switch (msg_p->command)
    {
        case BT_HIDH_ACTIVATE_REQ_EXT:
        {
            if (g_hidh_cntx.enabled) 
            {
                HID_LOG("receive activate request in enabled status, so reset it");
                btmtk_deinit_hid_host();
            }
            
            g_hidh_cntx.pfnRxReportCb = msg_p->param;
            btmtk_hidh_activate_req();

            break;
        }
        
        case BT_HIDH_DEACTIVATE_REQ_EXT:
        {
            if (g_hidh_cntx.enabled)
            {
                g_hidh_cntx.do_disable = TRUE;
                hidh_deactivate_req();
            }
            else
            {
                btmtk_hidh_UI_callback_direct(MBTEVT_HID_HOST_DISABLE_SUCCESS, 0, NULL);
            }
            break;
        }
			
        case BT_HIDH_CONNECT_REQ_EXT:
        {
            //BT_HID_DEVICE *dev = hidh_find_free_device((U8 *)&msg_p->addr);
            BT_HID_DEVICE *dev = hidh_find_free_device(msg_p->addr);


            if (!g_hidh_cntx.enabled || (dev == NULL))
            {
            	if(!g_hidh_cntx.enabled)
    			HID_LOG("btmtk_hidh_handle_req_ext::!g_hidh_cntx.enabled ");
        		else if(dev == NULL)
    			HID_LOG("btmtk_hidh_handle_req_ext:: (dev == NULL)");
                btmtk_hidh_UI_callback_direct(MBTEVT_HID_HOST_CONNECT_FAIL, &msg_p->addr, NULL);
                break;				
            }
			
            if (dev->state == BT_HID_APP_STAT_NO_CONNECTION || dev->state == BT_HID_APP_STAT_CONNCANCEL)
            {
                dev->addr = msg_p->addr;
                dev->conn_id = HID_USING_CONN_ID;
                HID_LOG("btmtk_hidh_handle_req_ext::dev->addr  0x%02X%02X%02X%02X%02X%02X",				
                 dev->addr.addr[0],	 
                 dev->addr.addr[1],	 
                 dev->addr.addr[2],	 
                 dev->addr.addr[3],	 
                 dev->addr.addr[4],	 
                 dev->addr.addr[5]);
                
                hid_set_host_state(dev, BT_HID_APP_STAT_CONNECTING);
                btmtk_hidh_UI_callback_direct(MBTEVT_HID_HOST_CONNECTING, &msg_p->addr, NULL);

                btmtk_hidh_connect_req((U8 *)&msg_p->addr);
                HID_LOG("btmtk_hidh_handle_req_ext:: -------CONNECTING------: %d", (U8)dev->state);
            }
            else
            {
                // do nothing, only notify the failure
                //if(dev->state != BT_HID_APP_STAT_WAITCONNECT)
                    //btmtk_hidh_UI_callback(MBTEVT_HID_HOST_CONNECT_FAIL, &msg_p->addr);
                HID_ERR("Connect req error state: %d", (U8)dev->state);
                //hid_set_host_state(dev, BT_HID_APP_STAT_NO_CONNECTION);

            }
            break;
        }
			
        case BT_HIDH_DISCONNECT_REQ_EXT:
        {	   
            HID_LOG("tmtk_hidh_handle_req_ext::msg_p->addr  0x%02X%02X%02X%02X%02X%02X",              
		        msg_p->addr.addr[0],    
		        msg_p->addr.addr[1],    
		        msg_p->addr.addr[2],    
		        msg_p->addr.addr[3],    
		        msg_p->addr.addr[4],    
		        msg_p->addr.addr[5]);
            //BT_HID_DEVICE *dev = hidh_find_device_by_addr((U8 *)&msg_p->addr);
            BT_HID_DEVICE *dev = hidh_find_device_by_addr(msg_p->addr);
			
            if (!g_hidh_cntx.enabled || (dev == NULL))
            {
                btmtk_hidh_UI_callback_direct(MBTEVT_HID_HOST_DISCONNECT_FAIL, &msg_p->addr, NULL);
                break;				
            }
			
            if (dev->state == BT_HID_APP_STAT_CONNECTED)
            {
                hid_set_host_state(dev, BT_HID_APP_STAT_DISCONNECTING);
                btmtk_hidh_UI_callback_direct(MBTEVT_HID_HOST_DISCONNECTING, &msg_p->addr, NULL);
                btmtk_hidh_disconnect_req((U8 *)&dev->addr);

            }
            else if (dev->state == BT_HID_APP_STAT_AUTHORIZING)
            {
                btmtk_hidh_authorize_rsp((U8 *)&dev->addr, dev->conn_id, FALSE);
                hid_set_host_state(dev, BT_HID_APP_STAT_NO_CONNECTION);
            }
            else if ((dev->state == BT_HID_APP_STAT_WAITCONNECT) ||
                          (dev->state == BT_HID_APP_STAT_CONNECTING))	
            {
                hid_set_host_state(dev, BT_HID_APP_STAT_CONNCANCEL);
            }
            else
            {
                btmtk_hidh_UI_callback_direct(MBTEVT_HID_HOST_DISCONNECT_FAIL, &msg_p->addr, NULL);
                HID_ERR(" Disconnect req error state: %d", (U8)dev->state);
            }
            break;
        }
			
        default:             			
            break;
    }
}

void btmtk_hidh_handle_activate_cnf(void *msg)
{
    HID_LOG("btmtk_hidh_handle_activate_cnf");
    bt_hidd_activate_cnf_struct *msg_p = (bt_hidd_activate_cnf_struct*) msg;

    hidh_init();
    
    if(msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_ENABLE_SUCCESS, 0, NULL);

        g_hidh_cntx.enabled = TRUE;
    }
    else
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_ENABLE_FAIL, 0, NULL);
    }

}

void btmtk_hidh_handle_deactivate_cnf(void *msg)
{
    bt_hidd_deactivate_cnf_struct *msg_p = (bt_hidd_deactivate_cnf_struct*) msg;

    g_hidh_cntx.do_disable = FALSE;
    
    if(msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_DISABLE_SUCCESS, 0, NULL);

        g_hidh_cntx.enabled = FALSE;
    }
    else
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_DISABLE_FAIL, 0, NULL);
    }

}	

void btmtk_hidh_handle_connect_cnf(void *msg)
{
    bt_hidd_connect_cnf_struct *msg_p = (bt_hidd_connect_cnf_struct *)msg;
    HID_LOG("btmtk_hidh_handle_connect_cnf::g_hidh_cntx.device[0]->addr  0x%02X%02X%02X%02X%02X%02X",              
		        g_hidh_cntx.device[0].addr.addr[0],    
		        g_hidh_cntx.device[0].addr.addr[1],    
		        g_hidh_cntx.device[0].addr.addr[2],    
		        g_hidh_cntx.device[0].addr.addr[3],    
		        g_hidh_cntx.device[0].addr.addr[4],    
		        g_hidh_cntx.device[0].addr.addr[5]);
    HID_LOG("btmtk_hidh_handle_connect_cnf:: -------g_hidh_cntx.device[0] state:%d------",g_hidh_cntx.device[0].state);

    //BT_HID_DEVICE *dev = hidh_find_device_by_addr((U8 *)&msg_p->bt_addr);
    //BT_HID_DEVICE *dev=NULL;
    //dev = &g_hidh_cntx.device[0];
    BT_HID_DEVICE *dev = hidh_find_device_by_addr(msg_p->bt_addr);
    //HID_LOG("[BT_HID_EXT]btmtk_hidh_handle_connect_cnf:: -------Original dev state:%d------",dev->state);

#ifdef __BT_HOGP_PTS_TEST__ 
    char val[256] = {0};
    int vallen = property_get("persist.bt.hogpptstest", val, "0");  
    U16 rpt_data = 0x0001;
#endif

    if (dev == NULL) 
    {    
        HID_ERR("btmtk_hidh_handle_connect_cnf::dev == NULL");             
        return;
    }


    dev->addr = msg_p->bt_addr;
    dev->conn_id = msg_p->connection_id;
	
    if (msg_p->result == hidd_result_ok)
    {
		
        if ((g_hidh_cntx.do_disable) ||
             (dev->state == BT_HID_APP_STAT_CONNCANCEL))
        {
            //hid_set_host_state(dev, BT_HID_APP_STAT_CONNECTED);
            btmtk_hidh_disconnect_req((U8 *)&msg_p->bt_addr);
            hid_set_host_state(dev, BT_HID_APP_STAT_DISCONNECTING);
        }
        else if (dev->state == BT_HID_APP_STAT_CONNECTING)
        {
            /* normal connect process done */ 
            hid_set_host_state(dev, BT_HID_APP_STAT_CONNECTED);
			
            //btmtk_os_memcpy((U8 *)&mbt_dev->BDAddr, (U8 *)&msg_p->bt_addr, sizeof(mbt_dev->BDAddr));

            btmtk_hidh_UI_callback(MBTEVT_HID_HOST_CONNECT_SUCCESS, &msg_p->bt_addr, NULL);
	HID_LOG("btmtk_hidh_handle_connect_cnf:: -------CONNECTING to CONNECTED:%d------",dev->state);
        }
        else
        {
            HID_ERR(" host handle connect cnf  -- succ: error state transaction, curstate: %d", dev->state);
            hid_set_host_state(dev, BT_HID_APP_STAT_CONNECTED);
            btmtk_hidh_UI_callback(MBTEVT_HID_HOST_CONNECT_SUCCESS, &msg_p->bt_addr, NULL);  // it might be connected because of reconnection
        }
    }
	else if(msg_p->result == hidd_result_no_resource){
		sleep(2);
		btmtk_hidh_connect_req((U8 *)&msg_p->bt_addr);
	}
    else
    {
		
        if (g_hidh_cntx.do_disable)
        {
            hid_set_host_state(dev, BT_HID_APP_STAT_NO_CONNECTION);
            hidh_deactivate_req();
        }
        else
        {
            if ((dev->state == BT_HID_APP_STAT_CONNECTING) ||
                (dev->state == BT_HID_APP_STAT_CONNCANCEL))
            {
                hid_set_host_state(dev, BT_HID_APP_STAT_NO_CONNECTION);
     		   
                btmtk_hidh_UI_callback(MBTEVT_HID_HOST_CONNECT_FAIL, &msg_p->bt_addr, NULL);
		HID_LOG("btmtk_hidh_handle_connect_cnf:: -------CONNECTING to NOCNN:%d------",dev->state);
            }
            else
            {
                // reset the state anyway
                HID_ERR("host handle connect cnf -- fail: error state transaction, curstate: %d", dev->state);
                hid_set_host_state(dev, BT_HID_APP_STAT_NO_CONNECTION);
                btmtk_hidh_UI_callback(MBTEVT_HID_HOST_DISCONNECT_SUCCESS, &msg_p->bt_addr, NULL);
				HID_LOG("btmtk_hidh_handle_connect_cnf:: -------other to NOCONN:%d------",dev->state);
            }
        }
    }

#ifdef __BT_HOGP_PTS_TEST__ 
    if ((dev->state == BT_HID_APP_STAT_CONNECTED) && vallen && (val[0] != '0'))
    {
        btmtk_hidh_set_report_req(msg_p->bt_addr, hidd_report_output, (U8*)&rpt_data, 2);  
        btmtk_hidh_send_output_report(msg_p->bt_addr, hidd_report_output, (U8*)&rpt_data, 2);        
    }
#endif
}

void btmtk_hidh_handle_disconnect_cnf(void *msg)
{
    bt_hidd_disconnect_cnf_struct *msg_p = (bt_hidd_disconnect_cnf_struct *)msg;
    //BT_HID_DEVICE *dev = hidh_find_device_by_addr((U8 *)&msg_p->bt_addr);
    BT_HID_DEVICE *dev = hidh_find_device_by_addr(msg_p->bt_addr);

    if (dev == NULL)
    {
        return;
    }

    dev->conn_id = HID_INVALID_CONN_ID;	// set to invalid connection id

    capsKeyPress = 1;
    numKeyPress = 1;
    scrollKeyPress = 1;
    if (g_hidh_cntx.do_disable)
    {
        hid_set_host_state(dev, BT_HID_APP_STAT_NO_CONNECTION);
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_DISCONNECT_SUCCESS, &msg_p->bt_addr, NULL);
        hidh_deactivate_req();
    }
    else
    {
        if (dev->state == BT_HID_APP_STAT_DISCONNECTING)
        {
            hid_set_host_state(dev, BT_HID_APP_STAT_NO_CONNECTION);
            btmtk_hidh_UI_callback(MBTEVT_HID_HOST_DISCONNECT_SUCCESS, &msg_p->bt_addr, NULL);
        }
        else
        {
            HID_ERR(" host handle disconnect cnf  -- error state transaction, curstate: %d", dev->state);
            hid_set_host_state(dev, BT_HID_APP_STAT_NO_CONNECTION);
            btmtk_hidh_UI_callback(MBTEVT_HID_HOST_DISCONNECT_SUCCESS, &msg_p->bt_addr, NULL);
        }
    }

}

void btmtk_hidh_handle_descinfo_cnf(void *msg)
{
    bt_hidh_descinfo_cnf_struct *msg_p = (bt_hidh_descinfo_cnf_struct *) msg;
#ifdef __BT_HOGP_PTS_TEST__ 
    char val[256] = {0};
    int vallen = property_get("persist.bt.hogpptstest", val, "0");  
    U16 rpt_data = 0x0001;
#endif    

    if (msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_GET_DESC_SUCCESS, &msg_p->bt_addr, NULL);
    }
    else
    {
        HID_LOG(" host descriptor list failed!");
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_GET_DESC_FAIL, &msg_p->bt_addr, NULL);
    }
    
#ifdef __BT_HOGP_PTS_TEST__ 
    if ((msg_p->result == hidd_result_ok) && vallen && (val[0] != '0'))
    {
        btmtk_hidh_set_report_req(msg_p->bt_addr, hidd_report_output, (U8*)&rpt_data, 2);  
        btmtk_hidh_send_output_report(msg_p->bt_addr, hidd_report_output, (U8*)&rpt_data, 2);        
    }
#endif    
}


void btmtk_hidh_handle_control_cnf(void *msg)
{
    bt_hidh_set_cmd_cnf_struct *msg_p = (bt_hidh_set_cmd_cnf_struct *) msg;

    if (msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_SEND_CONTROL_SUCCESS, &msg_p->bt_addr, NULL);
    }
    else
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_SEND_CONTROL_FAIL, &msg_p->bt_addr, NULL);
    }
}
void btmtk_hidh_handle_set_report_cnf(void *msg)
{
    bt_hidh_set_cmd_cnf_struct *msg_p = (bt_hidh_set_cmd_cnf_struct *) msg;

    if (msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_SET_REPORT_SUCCESS, &msg_p->bt_addr, NULL);
    }
    else
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_SET_REPORT_FAIL, &msg_p->bt_addr, NULL);
    }
}
void btmtk_hidh_handle_get_report_cnf(void *msg)
{
    bt_hidh_get_report_cnf_struct *msg_p = (bt_hidh_get_report_cnf_struct *) msg;

    if (msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_GET_REPORT_SUCCESS, &msg_p->bt_addr, NULL);
    
       //HID_MEM_FREE(msg_p->data_ptr);
        // send report data to parser...
    }
    else
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_GET_REPORT_FAIL, &msg_p->bt_addr, NULL);
    }

}

void btmtk_hidh_handle_set_protocol_cnf(void *msg)
{
    bt_hidh_set_cmd_cnf_struct *msg_p = (bt_hidh_set_cmd_cnf_struct *) msg;
    //BT_HID_DEVICE *dev = hidh_find_device_by_addr((U8 *)&msg_p->bt_addr);
    BT_HID_DEVICE *dev = hidh_find_device_by_addr(msg_p->bt_addr);


    if (msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_SET_PROTOCOL_SUCCESS, &msg_p->bt_addr, NULL);

        dev->trans_protocol = (U8)msg_p->data;
    }
    else
    {
        HID_LOG(" host set protocol cnf failed!");
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_SET_PROTOCOL_FAIL, &msg_p->bt_addr, NULL);
    }
}

void btmtk_hidh_handle_get_protocol_cnf(void *msg)
{
    bt_hidh_get_protocol_cnf_struct *msg_p = (bt_hidh_get_protocol_cnf_struct *) msg;
    bt_hidd_protocol_type_enum protocol_type = msg_p->protocol_type;


    if (msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_GET_PROTOCOL_SUCCESS, &msg_p->bt_addr, (void*)&protocol_type);
    }
    else
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_GET_PROTOCOL_FAIL, &msg_p->bt_addr, NULL);
    }
}

void btmtk_hidh_handle_set_idle_cnf(void *msg)
{
    bt_hidh_set_cmd_cnf_struct *msg_p = (bt_hidh_set_cmd_cnf_struct*) msg;

    if (msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_SET_IDLE_SUCCESS, &msg_p->bt_addr, NULL);
    }
    else
    {
        HID_LOG(" host set idlerate cnf failed!");
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_SET_IDLE_FAIL, &msg_p->bt_addr, NULL);
    }
}

void btmtk_hidh_handle_get_idle_cnf(void *msg)
{
    bt_hidh_get_idle_rate_cnf_struct *msg_p = (bt_hidh_get_idle_rate_cnf_struct *) msg;


    if (msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_GET_IDLE_SUCCESS, &msg_p->bt_addr, NULL);
    }
    else
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_GET_IDLE_FAIL, &msg_p->bt_addr, NULL);
    }
}

void btmtk_hidh_handle_output_report_cnf(void *msg)
{
    bt_hidd_interrupt_data_cnf_struct *msg_p = (bt_hidd_interrupt_data_cnf_struct *) msg;

    if (msg_p->result == hidd_result_ok)
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_SEND_REPORT_SUCCESS,  &msg_p->bt_addr, NULL);
    }
    else
    {
        btmtk_hidh_UI_callback(MBTEVT_HID_HOST_SEND_REPORT_FAIL,  &msg_p->bt_addr, NULL);
    }
}

void btmtk_hidh_handle_authorize_ind(void *msg)
{
    bt_hidd_connection_authorize_ind_struct *msg_p = (bt_hidd_connection_authorize_ind_struct *)msg;
    //BT_HID_DEVICE *dev = hidh_find_free_device((U8 *)&msg_p->bt_addr);
    BT_HID_DEVICE *dev = hidh_find_free_device(msg_p->bt_addr);

    if (dev == NULL)
    {
        btmtk_hidh_authorize_rsp((U8 *)&msg_p->bt_addr, msg_p->connection_id, FALSE);
        return;
    }
	
    dev->addr = msg_p->bt_addr;
    dev->conn_id = HID_USING_CONN_ID;

    hid_set_host_state(dev, BT_HID_APP_STAT_AUTHORIZING);

    // accept authorization as default
    //btmtk_hidh_authorize_rsp((U8 *)&msg_p->bt_addr, msg_p->connection_id, TRUE);
    btmtk_hidh_UI_callback(MBTEVT_HID_HOST_RECEIVE_AUTHORIZE,  &msg_p->bt_addr, NULL);
    g_conn_id=msg_p->connection_id;

}

void btmtk_hidh_authorize_rsp_ext(BD_ADDR BT_Addr, BT_BOOL result)
{
	HID_LOG("btmtk_hidh_authorize_rsp_ext");
	BT_HID_DEVICE *dev = hidh_find_device_by_addr(BT_Addr);
	btmtk_hidh_authorize_rsp((U8 *)&BT_Addr, g_conn_id, result);
	if(result == TRUE)
		hid_set_host_state(dev, BT_HID_APP_STAT_WAITCONNECT);
}

void btmtk_hidh_handle_connect_ind(void *msg)
{
    bt_hidd_connect_ind_struct *msg_p = (bt_hidd_connect_ind_struct *)msg;
   // BT_HID_DEVICE *dev = hidh_find_device_by_addr((U8 *)&msg_p->bt_addr);
    BT_HID_DEVICE *dev = hidh_find_device_by_addr(msg_p->bt_addr);


    if (dev == NULL)
    {    
        dev = hidh_find_free_device(msg_p->bt_addr);
        if (dev == NULL)
        {
            HID_ERR("btmtk_hidh_handle_connect_ind::dev == NULL");          
            return;
        }
    }

    dev->addr = msg_p->bt_addr;
    dev->conn_id = msg_p->connection_id;


    if (msg_p->result == hidd_result_ok)
    {
	
        if ((g_hidh_cntx.do_disable) ||
             (dev->state == BT_HID_APP_STAT_CONNCANCEL))
        {
            //hid_set_host_state(dev, BT_HID_APP_STAT_CONNECTED);
            btmtk_hidh_disconnect_req((U8 *)&msg_p->bt_addr);
            hid_set_host_state(dev, BT_HID_APP_STAT_DISCONNECTING);
        }
        else
        {
            if ((dev->state == BT_HID_APP_STAT_NO_CONNECTION) ||
                 (dev->state == BT_HID_APP_STAT_WAITCONNECT))		
            {
                 /* normal connect process done */ 
                 hid_set_host_state(dev, BT_HID_APP_STAT_CONNECTED);
                 
                 //btmtk_os_memcpy((U8 *)&mbt_dev->BDAddr, (U8 *)&msg_p->bt_addr, sizeof(mbt_dev->BDAddr));
                 btmtk_hidh_UI_callback(MBTEVT_HID_HOST_CONNECT_SUCCESS,  &msg_p->bt_addr, NULL);
	        btmtk_hidh_get_descInfo_req(msg_p->bt_addr);
            }
            else
            {
                 HID_ERR(" host handle connect ind  -- suc: error state transaction, curstate: %d", dev->state);
                 hid_set_host_state(dev, BT_HID_APP_STAT_CONNECTED);
            }
        }
    }
    else
    {
		
        if (g_hidh_cntx.do_disable)
        {
            hid_set_host_state(dev, BT_HID_APP_STAT_NO_CONNECTION);
            hidh_deactivate_req();
        }
        else
        {
            if ((dev->state == BT_HID_APP_STAT_NO_CONNECTION) ||
                 (dev->state == BT_HID_APP_STAT_AUTHORIZING) ||			
    	        (dev->state == BT_HID_APP_STAT_WAITCONNECT) ||
    	        (dev->state == BT_HID_APP_STAT_CONNCANCEL))
            {
                hid_set_host_state(dev, BT_HID_APP_STAT_NO_CONNECTION);
    			
                btmtk_hidh_UI_callback(MBTEVT_HID_HOST_CONNECT_FAIL,  &msg_p->bt_addr, NULL);
            }
            else
            {
                HID_ERR(" host handle connect ind  -- fail: error state transaction, curstate: %d", dev->state);
                hid_set_host_state(dev, BT_HID_APP_STAT_NO_CONNECTION);
            }
        }
    }

}

void btmtk_hidh_handle_disconnect_ind(void *msg)
{
    bt_hidd_disconnect_ind_struct *msg_p = (bt_hidd_disconnect_ind_struct *)msg;
   // BT_HID_DEVICE *dev = hidh_find_device_by_addr((U8 *)&msg_p->bt_addr);
    BT_HID_DEVICE *dev = hidh_find_device_by_addr(msg_p->bt_addr);

    if (dev == NULL)
    {
        return;
    }
	

    dev->conn_id = HID_INVALID_CONN_ID;	// set to invalid connection id

    capsKeyPress = 1;
    numKeyPress = 1;
    scrollKeyPress = 1;

    if (g_hidh_cntx.do_disable)
    {
        hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
        hidh_deactivate_req();
    }
    else
    {
        if (dev->state == BT_HID_APP_STAT_CONNECTED)
        {
            hid_set_host_state(dev, BT_HID_APP_STAT_NO_CONNECTION);
            btmtk_hidh_UI_callback(MBTEVT_HID_HOST_DISCONNECT_SUCCESS,  &msg_p->bt_addr, NULL);
        }
        else
        {
            HID_ERR(" host handle disconnect ind  -- error state transaction, curstate: %d", dev->state);
            hid_set_device_state(BT_HID_APP_STAT_NO_CONNECTION);
        }
    }

}

void btmtk_hidh_handle_unplug_ind(void *msg)
{
    bt_hidh_unplug_ind_struct *msg_p = (bt_hidh_unplug_ind_struct *)msg;
    //BT_HID_DEVICE *dev = hidh_find_device_by_addr((U8 *)&msg_p->bt_addr);
    BT_HID_DEVICE *dev = hidh_find_device_by_addr(msg_p->bt_addr);

    if (dev == NULL)
    {
        return;
    }

    btmtk_hidh_disconnect_req((U8 *)&dev->addr);
    hid_set_host_state(dev, BT_HID_APP_STAT_DISCONNECTING);
    btmtk_hidh_UI_callback(MBTEVT_HID_HOST_RECEIVE_UNPLUG,  &msg_p->bt_addr, NULL);
}


void btmtk_hidh_handle_input_report_ind(void *msg)
{
	HID_LOG("btmtk_hidh_handle_input_report_ind");

    bt_hidh_interrupt_data_ind_struct *msg_p = (bt_hidh_interrupt_data_ind_struct *)msg;
    //BT_HID_DEVICE *dev = hidh_find_device_by_addr((U8 *)&msg_p->bt_addr);
    BT_HID_DEVICE *dev = hidh_find_device_by_addr(msg_p->bt_addr);
    S32 usage=msg_p->data;
    U16  rpt_data;
    U16  rpt_id = 0x0001;

	HID_LOG("btmtk_hidh_handle_input_report_ind::usage=%d",usage);

    if (dev == NULL)
    {
        //HID_MEM_FREE(msg_p->data_ptr);
        return;
    }
    if(usage==KEY_CAPS_LOCK_USAGE)
    {
    	if(capsKeyPress % 2 ==1)
			rpt_data=LED_CAPS_LOCK_USAGE | rpt_id;	
		else
			rpt_data = rpt_id;
		capsKeyPress++;
    }
    else if(usage==KEY_SCROLL_LOCK_USAGE)		
    {
    	if(scrollKeyPress % 2 ==1)
			rpt_data=LED_SCROLL_LOCK_USAGE | rpt_id;
		else
			rpt_data = rpt_id;
		scrollKeyPress++;
    }
    else if(usage==KEY_NUM_LOCK_USAGE)		
    {
    	if(numKeyPress % 2 ==1)
			rpt_data=LED_NUM_LOCK_USAGE | rpt_id;	
		else
			rpt_data = rpt_id;
		numKeyPress++;
    }
    btmtk_hidh_set_report_req(msg_p->bt_addr,hidd_report_output,(U8*)&rpt_data,2);	
	//HID_MEM_FREE(msg_p->data_ptr);
}


static void hidh_deactivate_req(void)
{
    BT_HID_DEVICE *dev_conn = NULL;

    /* disconnect all the other devices before disable the service */
    if ((dev_conn = hidh_find_connected_device()) != NULL)
    {
        if (dev_conn->state == BT_HID_APP_STAT_CONNECTED)
        {
            // deactivate later, after the connection is disconnected
            btmtk_hidh_disconnect_req((U8 *)&dev_conn->addr);
            hid_set_host_state(dev_conn, BT_HID_APP_STAT_DISCONNECTING);
        }
        else if (dev_conn->state == BT_HID_APP_STAT_AUTHORIZING)
        {
            // deactivate later, after the connection is rejected
            btmtk_hidh_authorize_rsp((U8 *)&dev_conn->addr, dev_conn->conn_id, FALSE);
			btmtk_hidh_UI_callback(MBTEVT_HID_HOST_CONNECT_FAIL, &dev_conn->addr, NULL);
        }
        else if(dev_conn->state == BT_HID_APP_STAT_CONNECTING
			|| dev_conn->state == BT_HID_APP_STAT_WAITCONNECT)
        {
			btmtk_hidh_UI_callback(MBTEVT_HID_HOST_CONNECT_FAIL, &dev_conn->addr, NULL);            
        }
		else
		{
			// deactivate later, after the connection is done or disconnection is done
            // BT_HID_APP_STAT_DISCONNECTING, BT_HID_APP_STAT_CONNCANCEL,
		}
    }
    else
    {
        btmtk_hidh_deactivate_req();
    }
}

static BT_HID_DEVICE *hidh_find_device_by_addr(BD_ADDR device_addr)
{
    BT_HID_DEVICE *dev = NULL;
    U8 max_dev_num = HID_MAX_DEV_NUM;
    U8 i = 0;

    for (i = 0; i < max_dev_num; i++)
    {
        //if (HID_DEVICE_ADDR_EQUAL((U8 *)&g_hidh_cntx.device[i].addr, device_addr)) 
        if(g_hidh_cntx.device[i].addr.addr[0]==device_addr.addr[0] && \
	   g_hidh_cntx.device[i].addr.addr[1]==device_addr.addr[1] && \
	   g_hidh_cntx.device[i].addr.addr[2]==device_addr.addr[2] && \
	   g_hidh_cntx.device[i].addr.addr[3]==device_addr.addr[3] && \
	   g_hidh_cntx.device[i].addr.addr[4]==device_addr.addr[4] && \
	   g_hidh_cntx.device[i].addr.addr[5]==device_addr.addr[5] )
        {
            dev = &g_hidh_cntx.device[i];
            break;
        }
    }
    return dev;	
}

static BT_HID_DEVICE *hidh_find_free_device(BD_ADDR device_addr)
{
    BT_HID_DEVICE *dev = NULL;
    U8 max_dev_num = HID_MAX_DEV_NUM;
    U8 i = 0;

    if ((dev = hidh_find_device_by_addr(device_addr)) != NULL)
    {
        // if it is already in the device list, just pick up
        return dev;
    }
    
    for (i = 0; i < max_dev_num; i++)
    {
        if (g_hidh_cntx.device[i].conn_id == HID_INVALID_CONN_ID)
        {
            dev = &g_hidh_cntx.device[i];
            break;			
        }
    }

    return dev;	
}

static BT_HID_DEVICE *hidh_find_connected_device(void)
{
    BT_HID_DEVICE *dev = NULL;
    U8 max_dev_num = HID_MAX_DEV_NUM;
    U8 i = 0;

    for (i = 0; i < max_dev_num; i++)
    {
        if (g_hidh_cntx.device[i].state != BT_HID_APP_STAT_NO_CONNECTION)
        {
            dev = &g_hidh_cntx.device[i];
        }
    }
	
    return dev;
}

static void hid_set_device_state(BT_HID_APP_STATE state)
{
    g_hidd_cntx.device.state = state;
}

static void hid_set_host_state(BT_HID_DEVICE *dev, BT_HID_APP_STATE state)
{
    if (dev == NULL)
    {
        return;
    }
    dev->state = state;
}

