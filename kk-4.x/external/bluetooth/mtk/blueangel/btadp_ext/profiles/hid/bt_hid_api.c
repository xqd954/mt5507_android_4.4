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
 * Bt_hid_api.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to provide external adp api
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

#include "bt_gap_api.h"
#include "bt_goep_porting.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_mmi_msg.h"
#include "bluetooth_hid_message.h"
#include "bluetooth_hid_struct.h"
#include "bt_hid_api.h"

#include "bt_os_api.h" 
#include "bluetooth_gap_struct.h"
#include "me_adp.h"
#include "bt_ext_debug.h"

#ifdef BTMTK_ON_LINUX
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

/* Android socket api */
#include "cutils/sockets.h"

static int g_ext_hid_sockfd;

static struct sockaddr_un g_int_adp_sockaddr;
static socklen_t g_int_adp_sockaddr_len;
static struct sockaddr_un g_ext_adp_sockaddr;
static socklen_t g_ext_adp_sockaddr_len;

BTMTK_HID_CALLBACK g_hid_ui_callback = NULL;
extern void btmtk_util_convert_mbtarray2mtkarray(U8 *dest, U8 *source);
extern int socket_make_sockaddr_un(const char *name, int namespaceId, struct sockaddr_un *p_addr, socklen_t *alen);

/************************************************************************
*		Device Role					*
*************************************************************************/

void btmtk_hidd_activate_req(void)
{
    BTCMD_SendMessage(MSG_ID_BT_HIDD_ACTIVATE_REQ, MOD_BT, NULL, 0);
}

void btmtk_hidd_deactivate_req(void)
{
    BTCMD_SendMessage(MSG_ID_BT_HIDD_DEACTIVATE_REQ, MOD_BT, NULL, 0);
}

void btmtk_hidd_connect_req(U8 *device_addr)
{
    bt_hidd_connect_req_struct msg ;
	
    memcpy((U8 *)&msg.bt_addr, device_addr, sizeof(msg.bt_addr));
    BTCMD_SendMessage(MSG_ID_BT_HIDD_CONNECT_REQ, MOD_BT, &msg, sizeof(msg));
}

void btmtk_hidd_disconnect_req(U8 *device_addr, U32 conn_id)
{
    bt_hidd_disconnect_req_struct msg;

    memcpy((U8 *)&msg.bt_addr, device_addr, sizeof(msg.bt_addr));
    msg.connection_id = conn_id;
    BTCMD_SendMessage(MSG_ID_BT_HIDD_DISCONNECT_REQ, MOD_BT, &msg, sizeof(msg));
}

void btmtk_hidd_send_unplug_req()
{
    BTCMD_SendMessage(MSG_ID_BT_HIDD_UNPLUG_REQ, MOD_BT, NULL, 0);
}

void btmtk_hidd_send_input_report(bt_hidd_report_type_enum rpt_type, U8 *rpt_data, U16 rpt_size) 
{
    bt_hidd_interrupt_data_req_struct msg;

    msg.report_type = rpt_type;
    msg.data_len = rpt_size;
	
    msg.data_ptr = (U8 *)HID_MEM_MALLOC(rpt_size);
    if (msg.data_ptr == NULL)
    {
        return;
    }	
    memset(msg.data_ptr, 0, rpt_size);
    memcpy(msg.data_ptr, rpt_data, rpt_size);

    BTCMD_SendMessage(MSG_ID_BT_HIDD_INTERRUPT_DATA_REQ, MOD_BT, &msg, sizeof(msg));
}

void btmtk_gap_write_local_cod_request(U32 cod, btbm_write_cod_type type)
{
    bt_bm_write_local_cod_req_struct msg;
    
    msg.cod = cod;
    msg.write_type = type;
    
    BTCMD_SendMessage(MSG_ID_BT_BM_WRITE_LOCAL_COD_REQ, MOD_BT, &msg, sizeof(bt_bm_write_local_cod_req_struct));
}

void btmtk_hidd_change_role(bt_hid_roll device_role)
{
    U16 event;

    /* it is expected to change COD here, but it is not feasible to do this, 
      * since it would affect the behavior of other profiles. so only fake the result.
      */

    switch (device_role)
    {
        case BTMTK_HID_KEYBOARD_ROLE:
            event = MBTEVT_HID_DEVICE_CHANGE_KEYBOARD_SUCCESS;
            btmtk_gap_write_local_cod_request(COD_MINOR_PERIPH_KEYBOARD|COD_MAJOR_PERIPHERAL, BTBM_WRITE_COD_NEW);
            break;			

        case BTMTK_HID_MOUSE_ROLE:
            event = MBTEVT_HID_DEVICE_CHANGE_MOUSE_SUCCESS;
            btmtk_gap_write_local_cod_request(COD_MINOR_PERIPH_POINTING |COD_MAJOR_PERIPHERAL, BTBM_WRITE_COD_NEW);
            break;			

        case BTMTK_HID_PHONE_ROLE:
            event = MBTEVT_HID_DEVICE_CHANGE_PHONE_SUCCESS;
            btmtk_gap_write_local_cod_request(0, BTBM_WRITE_COD_RESTORE);
            break;			

        default:
            return;			
    }

    btmtk_hidh_UI_callback(event,0, NULL);
}

void btmtk_hidd_authorize_rsp(U8 *device_addr, U32 conn_id, BT_BOOL result)
{
    bt_hidd_connection_authorize_rsp_struct msg;

    if (result)
    {
        msg.result = hidd_connect_authorization_result_accepted;
    }
    else
    {
        msg.result = hidd_connect_authorization_result_rejected;
    }
    memcpy((U8 *)&msg.bt_addr, device_addr, sizeof(msg.bt_addr));
    msg.connection_id = conn_id;
    BTCMD_SendMessage(MSG_ID_BT_HIDD_CONNECTION_AUTHORIZE_RSP, MOD_BT, &msg, sizeof(msg));
}

void btmtk_hidd_activate_req_ext()
{
    bt_hidd_req_ext_struct msg;
    
    msg.command = BT_HIDD_ACTIVATE_REQ_EXT;
    BTCMD_SendMessage(MSG_ID_BT_HIDD_REQ_EXT, MOD_MMI, &msg, sizeof(bt_hidd_req_ext_struct));
}

void btmtk_hidd_deactivate_req_ext()

{
    bt_hidd_req_ext_struct msg;
    
    msg.command = BT_HIDD_DEACTIVATE_REQ_EXT;
    BTCMD_SendMessage(MSG_ID_BT_HIDD_REQ_EXT, MOD_MMI, &msg, sizeof(bt_hidd_req_ext_struct));
}

void btmtk_hidd_connect_req_ext(BD_ADDR device_addr)

{
    bt_hidd_req_ext_struct msg ;

    memcpy((U8 *)&msg.addr, (U8*)&device_addr, sizeof(msg.addr));
    msg.command = BT_HIDD_CONNECT_REQ_EXT;
    BTCMD_SendMessage(MSG_ID_BT_HIDD_REQ_EXT, MOD_MMI, &msg, sizeof(bt_hidd_req_ext_struct));
}

void btmtk_hidd_disconnect_req_ext()

{
    bt_hidd_req_ext_struct msg;

    msg.command = BT_HIDD_DISCONNECT_REQ_EXT;
    BTCMD_SendMessage(MSG_ID_BT_HIDD_REQ_EXT, MOD_MMI, &msg, sizeof(bt_hidd_req_ext_struct));

}


/************************************************************************
*								Host Role									*
*************************************************************************/

void btmtk_hidh_activate_req(void)
{
    BTCMD_SendMessage(MSG_ID_BT_HIDH_ACTIVATE_REQ, MOD_BT, NULL, 0);
}

void btmtk_hidh_deactivate_req(void)
{
    BTCMD_SendMessage(MSG_ID_BT_HIDH_DEACTIVATE_REQ, MOD_BT, NULL, 0);
}

void btmtk_hidh_connect_req(U8 *device_addr)
{
    bt_hidh_connect_req_struct msg;
    
    memcpy((U8 *)&msg.bt_addr, device_addr, sizeof(msg.bt_addr));
    BTCMD_SendMessage(MSG_ID_BT_HIDH_CONNECT_REQ, MOD_BT, &msg, sizeof(msg));
}

void btmtk_hidh_disconnect_req(U8 *device_addr)
{
    bt_hidh_disconnect_req_struct msg;
    memcpy((U8 *)&msg.bt_addr, device_addr, sizeof(msg.bt_addr));
    BTCMD_SendMessage(MSG_ID_BT_HIDH_DISCONNECT_REQ, MOD_BT, &msg, sizeof(msg));
}

void btmtk_hidh_get_descInfo_req(BD_ADDR device_addr)
{
    bt_hidh_get_desclist_req_struct msg;

    memcpy((U8 *)&msg.bt_addr, (U8*)&device_addr, sizeof(msg.bt_addr));
    BTCMD_SendMessage(MSG_ID_BT_HIDH_GET_DESCLIST_REQ, MOD_BT, &msg, sizeof(msg));
}

void btmtk_hidh_send_control_req(BD_ADDR device_addr, bt_hidd_ctrl_op_enum control)
{
    bt_hidh_control_req_struct msg;
    memcpy((U8 *)&msg.bt_addr, (U8*)&device_addr, sizeof(msg.bt_addr));
    msg.ctrl_op = (bt_hidd_ctrl_op_enum)control;
    BTCMD_SendMessage(MSG_ID_BT_HIDH_SEND_CONTROL_REQ, MOD_BT, &msg, sizeof(msg));
}

void btmtk_hidh_set_report_req(BD_ADDR device_addr, bt_hidd_report_type_enum rpt_type, U8 *rpt_data, U16 rpt_size)
{
    //bt_hidh_set_report_req_struct msg = {0};
    bt_hidh_set_report_req_struct *msg ;

    if (rpt_size == 0)
    {
        btmtk_hidh_UI_callback_direct(MBTEVT_HID_HOST_SET_REPORT_FAIL, &device_addr, NULL);
        return;		
    }

    //msg.data_ptr = (U8 *)HID_MEM_MALLOC(rpt_size);
    msg=(bt_hidh_set_report_req_struct*)HID_MEM_MALLOC(sizeof(bt_hidh_set_report_req_struct) + rpt_size);
    if (msg->data_ptr == NULL)
    {
        return;
    }
    memset(msg->data_ptr, 0, rpt_size);
    memcpy(msg->data_ptr, rpt_data, rpt_size);
    
    memcpy((U8 *)&msg->bt_addr, (U8*)&device_addr, sizeof(msg->bt_addr));
    msg->report_type = rpt_type;
    msg->data_len = rpt_size;
    BTCMD_SendMessage(MSG_ID_BT_HIDH_SET_REPORT_REQ, MOD_BT, msg, sizeof(*msg));
}

void btmtk_hidh_get_report_req(BD_ADDR device_addr, U8 rpt_type, U8 rpt_id, U8 buffer_size)
{
    bt_hidh_get_report_req_struct msg;

    memcpy((U8 *)&msg.bt_addr, (U8*)&device_addr, sizeof(msg.bt_addr));
    msg.report_type = rpt_type;
    msg.report_id = rpt_id;
    msg.use_rpt_id = TRUE;
    msg.buffer_size = buffer_size;
    BTCMD_SendMessage(MSG_ID_BT_HIDH_GET_REPORT_REQ, MOD_BT, &msg, sizeof(msg));
}

void btmtk_hidh_set_protocol_req(BD_ADDR device_addr, BT_BOOL boot_mode)
{
    bt_hidh_set_protocol_req_struct msg;
    memcpy((U8 *)&msg.bt_addr, (U8*)&device_addr, sizeof(msg.bt_addr));
    if (boot_mode)
    {
        msg.protocol_type = hidd_protocol_boot;
    }
    else
    {
        msg.protocol_type = hidd_protocol_report;
    }
    BTCMD_SendMessage(MSG_ID_BT_HIDH_SET_PROTOCOL_REQ, MOD_BT, &msg, sizeof(msg));
}

void btmtk_hidh_get_protocol_req(BD_ADDR device_addr)
{
    bt_hidh_get_protocol_req_struct msg;
    memcpy((U8 *)&msg.bt_addr, (U8*)&device_addr, sizeof(msg.bt_addr));
    BTCMD_SendMessage(MSG_ID_BT_HIDH_GET_PROTOCOL_REQ, MOD_BT, &msg, sizeof(msg));
}

void btmtk_hidh_set_idle_rate_req(BD_ADDR device_addr, U8 idleRate)
{
    bt_hidh_set_idle_rate_req_struct msg;
    HID_LOG("host set idlerate: %d", idleRate);

    memcpy((U8 *)&msg.bt_addr, (U8*)&device_addr, sizeof(msg.bt_addr));
    msg.idle_rate = idleRate;
    BTCMD_SendMessage(MSG_ID_BT_HIDH_SET_IDLE_RATE_REQ, MOD_BT, &msg, sizeof(msg));
}

void btmtk_hidh_get_idle_rate_req(BD_ADDR device_addr)
{
    bt_hidh_get_idle_rate_req_struct msg;
    memcpy((U8 *)&msg.bt_addr, (U8*)&device_addr, sizeof(msg.bt_addr));
    BTCMD_SendMessage(MSG_ID_BT_HIDH_GET_IDLE_RATE_REQ, MOD_BT, &msg, sizeof(msg));
}

void btmtk_hidh_send_output_report(BD_ADDR device_addr, bt_hidd_report_type_enum rpt_type, U8 *rpt_data, U16 rpt_size)
{
    //bt_hidh_interrupt_data_req_struct msg = {0};
    bt_hidh_interrupt_data_req_struct *msg ;
    if (rpt_size == 0)
    {
        btmtk_hidh_UI_callback_direct(MBTEVT_HID_HOST_SEND_REPORT_FAIL, &device_addr, NULL);
        return;
    }
    msg=(bt_hidh_interrupt_data_req_struct*)HID_MEM_MALLOC(sizeof(bt_hidh_interrupt_data_req_struct) + rpt_size);
    //msg.data_ptr = (U8 *)HID_MEM_MALLOC(rpt_size);
    //msg.data_ptr = (U8 *)malloc(rpt_size);
    //if (msg.data_ptr == NULL)
    //{
    //    return;
    //}
    memset(msg->data_ptr, 0, rpt_size);
    memcpy(msg->data_ptr, rpt_data, rpt_size);
    
    memcpy((U8 *)&msg->bt_addr, (U8*)&device_addr, sizeof(msg->bt_addr));
    msg->report_type = rpt_type;
    msg->data_len = rpt_size;
	HID_LOG("btmtk_hidh_send_output_report::msg->report_type= %d", msg->report_type);
		HID_LOG("btmtk_hidh_send_output_report::msg->data_len= %d", msg->data_len);
    BTCMD_SendMessage(MSG_ID_BT_HIDH_SEND_OUTPUT_REPORT, MOD_BT, msg, sizeof(*msg));

}

void btmtk_hidh_authorize_rsp(U8 *device_addr, U32 conn_id, BT_BOOL result)
{
    bt_hidh_connection_authorize_rsp_struct msg;

    if (result)
    {
        msg.result = hidd_connect_authorization_result_accepted;
    }
    else
    {
        msg.result = hidd_connect_authorization_result_rejected;
    }
    memcpy((U8 *)&msg.bt_addr, device_addr, sizeof(msg.bt_addr));
    msg.connection_id = conn_id;
    BTCMD_SendMessage(MSG_ID_BT_HIDH_CONNECTION_AUTHORIZE_RSP, MOD_BT, &msg, sizeof(msg));
}

void btmtk_hidh_activate_req_ext(void *pfnCB)
{
    bt_hidh_req_ext_struct msg;
    
    msg.command = BT_HIDH_ACTIVATE_REQ_EXT;
    msg.param = pfnCB;
    BTCMD_SendMessage(MSG_ID_BT_HIDH_REQ_EXT, MOD_MMI, &msg, sizeof(bt_hidh_req_ext_struct));
}

void btmtk_hidh_deactivate_req_ext(void)
{
    bt_hidh_req_ext_struct msg;
    
    msg.command = BT_HIDH_DEACTIVATE_REQ_EXT;
    BTCMD_SendMessage(MSG_ID_BT_HIDH_REQ_EXT, MOD_MMI, &msg, sizeof(bt_hidh_req_ext_struct));
}

void btmtk_hidh_connect_req_ext(BD_ADDR device_addr)

{
    bt_hidh_req_ext_struct msg;

    memcpy((U8 *)&msg.addr, (U8*)&device_addr, sizeof(msg.addr));
    msg.command = BT_HIDH_CONNECT_REQ_EXT;
    BTCMD_SendMessage(MSG_ID_BT_HIDH_REQ_EXT, MOD_MMI, &msg, sizeof(bt_hidh_req_ext_struct));
}

void btmtk_hidh_disconnect_req_ext(BD_ADDR device_addr)

{
    bt_hidh_req_ext_struct msg;

    memcpy((U8 *)&msg.addr, (U8*)&device_addr, sizeof(msg.addr));
    msg.command = BT_HIDH_DISCONNECT_REQ_EXT;
    BTCMD_SendMessage(MSG_ID_BT_HIDH_REQ_EXT, MOD_MMI, &msg, sizeof(bt_hidh_req_ext_struct));
}

void btmtk_hidh_UI_callback(bt_hid_host_event_type  Evt, BD_ADDR* device_addr, void* para)
{
    bt_ext_log("mbt post event:%d", Evt);
    if(g_hid_ui_callback != NULL)
   	 g_hid_ui_callback(Evt, device_addr, para);
}

void btmtk_hidh_UI_callback_direct(bt_hid_host_event_type  Evt, BD_ADDR* device_addr, void* para)
{
    bt_hidh_req_ext_struct msg;
    bt_ext_log("mbt post event direct:%d", Evt);

    if(device_addr != NULL)
    	memcpy((U8 *)&msg.addr, (U8*)device_addr, sizeof(msg.addr));
    msg.command = Evt;
    BTCMD_SendMessage(MSG_ID_BT_HIDH_REQ_EXT, MOD_MMI, &msg, sizeof(bt_hidh_req_ext_struct));
}

BT_BOOL btmtk_hidh_set_callback(BTMTK_HID_CALLBACK callback)
{
	bt_ext_log("btmtk_hidh_set_callback");
	g_hid_ui_callback = callback;
	return TRUE;
}

#ifdef BTMTK_ON_LINUX
/*****************************************************************************
 * FUNCTION
 *  bt_HID_init_socket
 * DESCRIPTION
 *  This function initializes the socket for receiving ilm from bt task and
 *  stores the address of sending socket.
 * PARAMETERS
 *  none.        
 * RETURNS
 *  Created socket file descriptor or -1 (socket-creation failed)
 *****************************************************************************/
U8  bt_hid_init_socket()
{
    char buff[20];
    int err, sock_fl;

     g_int_adp_sockaddr.sun_family = AF_UNIX;
    strcpy(g_int_adp_sockaddr.sun_path, BT_SOCK_NAME_INT_ADP);
    g_int_adp_sockaddr_len = (offsetof(struct sockaddr_un, sun_path) + strlen(g_int_adp_sockaddr.sun_path) + 1);
	
    if( socket_make_sockaddr_un(BT_SOCK_NAME_EXT_ADP_HID, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&g_ext_adp_sockaddr, &g_ext_adp_sockaddr_len) < 0)
    {
        bt_ext_err("setProfileAddress(%s) failed", BT_SOCK_NAME_EXT_ADP_HID);
    }


    g_ext_hid_sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (g_ext_hid_sockfd < 0) {
	bt_ext_err("Create server socket failed.");
	return 0;
    }
    //if (0 > bind(g_ext_hid_sockfd, (struct sockaddr*) &g_int_adp_sockaddr, g_int_adp_sockaddr_len)) {
//	bt_ext_err("Bind server socket failed.");
//	return 0;
  //  }

    /* Get current socket flags */
    sock_fl = fcntl(g_ext_hid_sockfd, F_GETFL);
    if (sock_fl == -1) {
	err = errno;
	bt_ext_err("Get socket flags failed. Reason: [%d]%s", err, strerror(err));
	return 0;
    }

    /* Set ftp socket as non-block */
    if (fcntl(g_ext_hid_sockfd, F_SETFL, sock_fl | O_NONBLOCK) == -1) {
	err = errno;
	bt_ext_err("Set O_NONBLOCK flag to socket failed. Reason: [%d]%s", err, strerror(err));
	return 0;
    }

    /* Read data from socket till empty */
    while(0 < recvfrom(g_ext_hid_sockfd, (void*) buff, sizeof(buff), 0, NULL, NULL)) {
	bt_ext_log("Clearing socket...");
    }
    bt_ext_log("[BT_HID]Socket clear.");

    /* Set back original flags */
    if (fcntl(g_ext_hid_sockfd, F_SETFL, sock_fl) == -1) {
	err = errno;
	bt_ext_err("Set O_NONBLOCK flag to socket failed. Reason: [%d]%s", err, strerror(err));
	return 0;
    }
    return 1;
}

 U8 bt_hid_close_socket()
 {
	if(g_ext_hid_sockfd<0)
	{
		bt_ext_err("Invalid g_ext_hid_sockfd");
		return 0;
	}
	close(g_ext_hid_sockfd);
	g_ext_hid_sockfd=-1;
	bt_ext_log("Close g_ext_hid_sockfd OK");
	return 1;
 }

 void BTCMD_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size)
{
    //bt_ext_log("[BT_HID]BTCMD_SendMessage");
    int ret = -1;
    struct sockaddr_un* addr = NULL;
    socklen_t* addr_len = NULL;
    ilm_struct ilm;
    U16 *field;

     GOEP_MEMSET(&ilm, 0, sizeof(ilm_struct));
    ilm.msg_id = msg_id;
    ilm.src_mod_id = MOD_EXTERNAL;
    ilm.dest_mod_id = dest_mod;

    //assert (size <= MAX_ILM_BUFFER_SIZE);
    if(size > MAX_ILM_BUFFER_SIZE){
	bt_ext_log("[BT_HID]size > MAX_ILM_BUFFER_SIZE");
	return;
	}

    if(ptr != NULL)
	memcpy(ilm.ilm_data, ptr, size);

    // LOCAL_PARA_HDR
    field = (U16 *)ilm.ilm_data;
    *field = 1;
    field = (U16 *)&ilm.ilm_data[2];
    *field = size;

    // cal whole ilm size
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;

    if (g_ext_hid_sockfd) {
	if(dest_mod == MOD_BT){
		addr = &g_int_adp_sockaddr;
		addr_len = &g_int_adp_sockaddr_len;
	}else if(dest_mod == MOD_MMI){
		addr = &g_ext_adp_sockaddr;
		addr_len = &g_ext_adp_sockaddr_len;
	}
	ret = sendto(g_ext_hid_sockfd, (void *)&ilm, size, 0,
		     (struct sockaddr*) addr, *addr_len);
	if (ret < 0) {
	    bt_ext_err("Sending msg failed: %s, %d", strerror(errno), errno);
	} else {
	    bt_ext_log("Sending msg succeeded: %d", ret);
	}

    } else {
	bt_ext_err("Socket is not yet initialized.");
    }
}
#endif

//#if 0
void *bt_win_malloc(U32 size)
{
    U8 *ptr;
    ptr = (U8 *)malloc(size ); 
/*
    size = ((size+3)&0xFFFFFFFC);
    ptr = (U8 *)malloc(size + 12); 
  
    *(U32 *)ptr = BT_MEM_GUARD1;
    *(U32 *)(ptr + 4) = size;
    *(U32 *)(ptr + size + 8) = BT_MEM_GUARD2;
	
    return (void *)(ptr + 8);
	*/
	bt_ext_log("[HID_EXT]bt_hid_malloc succeeded");
    return (void *)ptr;	
}

void bt_win_free(void *ptr)
{
	bt_ext_log("[HID_EXT]Enter bt_hid_free ");
    U8 *ptr1 = ptr;
	/*
    S32 size = *(S32 *)(ptr1 - 4);
    if (*(U32 *)(ptr1 - 8) != BT_MEM_GUARD1)
    {
        assert(0);
    }
    if (*(U32 *)(ptr1 + size) != BT_MEM_GUARD2)
    {
        assert(0);
    }
    *(U32 *)(ptr1 - 8) = BT_MEM_GUARD3;
    *(U32 *)(ptr1 + size) = BT_MEM_GUARD4;
    free((ptr1 - 8));
    */
    free(ptr1);
	bt_ext_log("[HID_EXT]bt_hid_free succeeded");
}
//#endif
