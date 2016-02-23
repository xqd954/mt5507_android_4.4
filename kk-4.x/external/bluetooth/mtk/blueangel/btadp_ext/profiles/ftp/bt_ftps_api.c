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
 * bt_ftps_api.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   Export function from FTP Server
 *
 * Author:
 * -------
 * Will Chen
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/


#include "bt_goep_porting.h"


#include "bluetooth_gap_struct.h"
#include "bluetooth_struct.h"
#include "bt_os_api.h"
#include "ext_osal.h"
#include "ext_nbuffer.h"
#include "bluetooth_ftps_struct.h"
#include "bluetooth_ftpc_struct.h"
#include "bt_ftps_event.h"
#include "bt_ftps_struct.h"
#include "bt_ftps_api.h"
#include "bt_ftps_hdl.h"

#include "bt_ftps_genxml.h"

#include "ext_ucs2.h"
#include "bt_goep_fs.h"
#include "bt_ftp_porting.h"
#include "bt_ext_debug.h"
//#include "goep.h"

#define GOEP_Report bt_ext_log

#if defined(__BT_FTS_PROFILE__)
bt_ftps_cntx_struct g_bt_ftps_cntx;
bt_ftps_cntx_struct *const ftps_cntx_p = &g_bt_ftps_cntx;
bt_ftps_conn_cntx_struct *act_server_cntx_p;
BTMTK_FTPS_CALLBACK g_ftps_ui_callback = NULL;
static const unsigned char OBEX_FTP_UUID[] = { 0xF9, 0xEC, 0x7B, 0xC4, 0x95, 0x3C, 0x11, 0xD2,
                                   0x98, 0x4E, 0x52, 0x54, 0x00, 0xDC, 0x9E, 0x09 };
#define BT_BTH_OBEX_FILE_TRANSFER_UUID            (0x1106 )

BT_BOOL btmtk_ftps_activate_server(U8 tptype);
BT_BOOL ftps_disconnect_client(U8);

/**
* @brief Setup the callback for notifying UI
*/
BT_BOOL btmtk_ftps_set_callback(BTMTK_FTPS_CALLBACK callback){
	g_ftps_ui_callback = callback;
	return TRUE;
}
    
/**
* @brief This function is to initialize FTPS context
*  FTP Server Enable
*  This funciton is only to init the cntxt in ftps, if read shared path info from a hidden file, if read failed, 
*  then default path will be set to variables.
**/
static BT_BOOL ftps_cntx_init(void)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_CNTX_INIT) );

	// Access Right Init
	// Read the ftp server access right from nvram

	// TODO ReadValue(NVRAM_FTP_ACCESS_RIGHT, &(ftps_cntx_p->access_right), DS_BYTE, &error);
	ftps_cntx_p->access_right = 0; 

	if (ftps_cntx_p->access_right == 0xFF)
	{
		// default setting == READ_ONLY
		ftps_cntx_p->access_right = BT_FTP_ACCESS_RIGHT_READ_ONLY;
		// FTP_DEBUG(kal_print("ACCESS RIGHT %d\n", ftps_cntx_p->access_right);)
	}

	// FTPS error flag Init
	ftps_cntx_p->ftps_flag = 0;
	// for get cur goep_conn_id in authorize process
	ftps_cntx_p->cur_goep_conn_id = 0;

	act_server_cntx_p = &(ftps_cntx_p->conn_cntx[0]);
	(act_server_cntx_p)->ftps_browse_depth = 0;

#ifdef FTPS_STATIC_ROOT_FOLDER
	res = btmtk_get_default_root_folder_path( ftps_cntx_p->root_folder, sizeof(ftps_cntx_p->root_folder) );
	GOEP_MEMSET((act_server_cntx_p)->current_folder, 0, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);

	if (res == EXT_FS_OK)
	{
		btmtk_goep_create_folder( (const U16 *)ftps_cntx_p->root_folder);
	}else{
		BT_FTP_ASSERT(0);
	}
#endif

	for (i = 0; i < FTP_MAX_CLIENT_SUPPORT; i++)
	{
		// ID Init
		ftps_cntx_p->goep_conn_id[i] = 0xFE; /* avoid case: When Client Connecting, BT Reset. disconnect_ind_hdlr*/
		ftps_cntx_p->g_conn_id[i] = 0xFF;


		// Send/Recv File Handle Init
		(act_server_cntx_p + i)->fh_ftps_send = (FHANDLE) FHANDLE_INVALID_VALUE;
		(act_server_cntx_p + i)->fh_ftps_recv = (FHANDLE) FHANDLE_INVALID_VALUE;

		// FTPS STATE Init
		(act_server_cntx_p + i)->ftps_state = BT_FTPS_STATE_IDLE;

		// Xml_composer Init
		GOEP_MEMSET( (U8*)&((act_server_cntx_p + i)->xml_composer), 0, sizeof(bt_ftp_file_writer_struct));

		// bt_device_addr init
		GOEP_MEMSET( (U8*)&((act_server_cntx_p + i)->ftps_bt_device), 0, sizeof(goep_bd_addr_struct));

		// FTPS Obj Name Init
		GOEP_MEMSET( (U8*)(act_server_cntx_p + i)->ftps_obj_name, 0, FTP_MAX_OBJ_NAME_LEN);

		// FTPS Dev Name Init
		GOEP_MEMSET( (U8*)(act_server_cntx_p + i)->dev_name, 0, GOEP_MAX_DEV_NAME);

		// FTPS FOLDER CONTENT FILE Init
		FTPS_MAKE_FOLDER_CONTENT(((act_server_cntx_p+i)->folder_content), i);

		// Others Init
		(act_server_cntx_p + i)->challeng_options = 0;
		(act_server_cntx_p + i)->realm[0] = 0;
		(act_server_cntx_p + i)->send_obj_type = BT_FTP_OBJ_TYPE_NONE;
		(act_server_cntx_p + i)->total_send_obj_len = 0;
		(act_server_cntx_p + i)->send_obex_pkt_size = 2048; // remote max packet size
		(act_server_cntx_p + i)->remain_send_obj_len = 0;
		(act_server_cntx_p + i)->dis_from_user = FALSE;
		(act_server_cntx_p + i)->ftps_obj_buff_size = FTPC_MEM_BUFF_SIZE; // the buffer size of local buffer
		(act_server_cntx_p + i)->ftps_obj_buff = 
		btmtk_ftps_get_profile_shared_int_buffer((act_server_cntx_p + i)->ftps_obj_buff_size);

		if( NULL == (act_server_cntx_p + i)->ftps_obj_buff  ){
			// TODO: log error
			act_server_cntx_p = NULL;
			return FALSE;
		}

		// FTPS obj buffer init
		GOEP_MEMSET( (U8*)(act_server_cntx_p + i)->ftps_obj_buff, 0, FTPS_MEM_BUFF_SIZE);

		GOEP_MEMSET((act_server_cntx_p + i)->temp_pushing_file, 0, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
		(act_server_cntx_p + i)->ftps_fh_check_timer_set = FALSE;
				
		act_server_cntx_p->ftps_auto_mask = 0; //; // BT_FTPS_AUTO_BITMASK
		act_server_cntx_p->ftps_auto_mask = BT_FTP_AUTO_BROWSE | 
						BT_FTP_AUTO_PUSH | 
						BT_FTP_AUTO_PULL |
						BT_FTP_AUTO_CREATE_FOLDER |
						BT_FTP_AUTO_DELETE |
						BT_FTP_AUTO_SETPATH;

		bt_ext_log("[BT][FTP] Server auto-mask: %02x", act_server_cntx_p->ftps_auto_mask);

		(act_server_cntx_p + i)->ftps_inbuf_pkt_error = 0 ;// BT_FTP_PRE_PKT_ERR_NONE;
	}

	FTPS_CS_INIT();
	return TRUE;
}


/**
* @brief  This function is to activate the FTP application
* FTP Server Enable
* Enable the ftp server and register the SDP record
**/
BT_BOOL btmtk_ftps_activate_server(U8 tptype)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U8 Register_flag;
	goep_register_server_req_struct *req;
	Register_flag = 0;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_ACTIVATE));
    OS_Report("[BT30] btmtk_ftps_activate_server tptype:%d ", tptype);    

	if (NULL == act_server_cntx_p) {
		if (FALSE == ftps_cntx_init()) {
			GOEP_Report( "[ftps] func: activate_server fail to ftps_cntx_init\n");
			return FALSE;
		}
	} else {
		GOEP_Report("[ftps] func: activate_server ok act_server_cntx_p has assigned\n");
		GOEP_Report("[ftps] act_server_cntx_p->ftps_state: %d\n", act_server_cntx_p->ftps_state);

		if (act_server_cntx_p->ftps_state == BT_FTPS_STATE_REGISTERING) {
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_ACTIVE);
			btmtk_ftps_ui_callback(BT_FTPSUI_READY, (void *) 0); 
		}

		return TRUE; // 091022: APPI return active already
	}

	BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_REGISTERING);

	// find all IDLE state connection and register them all
	switch ((act_server_cntx_p)->ftps_state) {

		// unactivated connection
		case BT_FTPS_STATE_REGISTERING:        
			// Construct goep register req ilm
			req = (goep_register_server_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_register_server_req_struct), TD_CTRL);
			GOEP_MEMSET((U8*) req, 0, sizeof(goep_register_server_req_struct));
			GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
			req->uuid_len = 16;
			req->req_id = (U8) 0;
			//FTP_DEBUG(kal_print("ILM REQ_ID: %d\n", req->req_id);)
			req->tp_type = GOEP_TP_BT;
			req->buf_ptr = (act_server_cntx_p)->ftps_obj_buff;
			req->buf_size = (U16)(act_server_cntx_p)->ftps_obj_buff_size;
#ifdef __BT_GOEP_V2__ 
	                req->buf_size = 0xfff0; 		
                        req->tptype = 2;           /* enable in both mode */
	                if( tptype > 0){
	                      req->obexver = 0x12; /* ver 1.2 */
	                }
#endif	
			// switch state to registering
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_REGISTERING);

			if (FALSE == ext_nbuffer_isInit(&act_server_cntx_p->ftps_mempool)) {
				//GOEP_Report("[ftps] ext_nbuffer_isInit false");
				GOEP_TRACE((BT_FTP_TRC_CLASS,BT_FTPS_INIT_NBUFFER_DISABLE,
						ext_nbuffer_isInit(&act_server_cntx_p->ftps_mempool)));
			}

			// Send MSG_ID_BT_FTPS_REGISTER_SERVER_REQ primitive
			bt_ftp_send_msg(MSG_ID_BT_FTPS_REGISTER_SERVER_REQ, req, sizeof(goep_register_server_req_struct));

			// set flag to notify the unactivaed connection is found
			if (0 == Register_flag) {
				Register_flag = 1;
			}

			break;

		// other states
		default:
			//BT_FTPS_LOG_STATE_ERR();
			break;
	}

	// without un-activated connections --> call activate cnf to CM
	if (Register_flag == 0)
	{
		// call activate cnf to CM
		//mmi_bth_activate_cnf_hdler((U32) BT_BTH_OBEX_FILE_TRANSFER_UUID);
		// btmtk_ftps_ui_callback(BT_FTPSUI_READY, (U8 *)NULL);
	}

	return TRUE;
}

void btmtk_deinit_ftp_server(void) {
	GOEP_Report("[FTPS] btmtk_deinit_ftp_server") ;
	GOEP_MEMSET( (U8 *)ftps_cntx_p, 0, sizeof(ftps_cntx_p));
	act_server_cntx_p = NULL;
}

BT_BOOL btmtk_ftps_forcefully_deactivate_server(void) {
	goep_deregister_server_req_struct *dereg_req;

	bt_ext_log("[BT][FTP] %s()", __FUNCTION__);
	dereg_req = (goep_deregister_server_req_struct *) CONSTRUCT_LOCAL_PARAM(
			sizeof(goep_deregister_server_req_struct), TD_CTRL);
	dereg_req->goep_conn_id = 0xFE;

	bt_ftp_send_msg(MSG_ID_BT_FTPS_DEREGISTER_SERVER_REQ,
			dereg_req,
			sizeof(goep_deregister_server_req_struct));

	return TRUE;
}

/**
* @brief This function is to deactivate the FTP application
* FTP Server Disable
**/
BT_BOOL btmtk_ftps_deactivate_server(void)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 u4PreviousState;
	BT_BOOL dis_ret;

	//bt_sdpdb_deregister_req_struct *req;
	goep_deregister_server_req_struct *dereg_req;

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_DEACTIVATE));

	if (NULL == act_server_cntx_p) {
		GOEP_Report( "[BT][FTP] func: deactivate_server ok act_server_cntx_p is NULL\n");
		return TRUE; // APPI demo: 091022. deactivate twice is success
	}

	FTPS_CS_ENTER();

	if (BT_FTPS_STATE_IDLE == (act_server_cntx_p)->ftps_state) {
		GOEP_Report( "[BT][FTP] func: deactivate_server wrong state:%d\n", (act_server_cntx_p)->ftps_state);
		FTPS_CS_LEAVE();
		return FALSE;
	}

	u4PreviousState = (act_server_cntx_p)->ftps_state;
	BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_DEACTIVATING);

	FTPS_CS_LEAVE();

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_DEACTIVATE));

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_STATE, 0, act_server_cntx_p->ftps_state));

	switch (u4PreviousState) {
		case BT_FTPS_STATE_REGISTERING:    // activating -> deactivating
		case BT_FTPS_STATE_DEACTIVATING:   // deactivating -> deactivating
			//BT_FTP_ASSERT(0);
			BT_FTPS_STATE_TRANS(0, u4PreviousState);
			return FALSE;
			break;

		case BT_FTPS_STATE_IDLE:           // unactivated
			BT_FTPS_STATE_TRANS(0, u4PreviousState);
			return FALSE;
			break;

		// In these cases, we need to handle gracefully and disconnect.
		case BT_FTPS_STATE_AUTHORIZING:
			btmtk_ftps_authorize_reject();
			break;

		case BT_FTPS_STATE_CONNECTED:
		case BT_FTPS_STATE_RECEIVE_WAIT:
		case BT_FTPS_STATE_RECEIVING:
		case BT_FTPS_STATE_SEND_WAIT:
		case BT_FTPS_STATE_SENDING:
		case BT_FTPS_STATE_SETPATH_WAIT:
		case BT_FTPS_STATE_SETPATHING:

			// NOTE: Bttask will send a MSG_ID_FTPS_TPDISCONNECT_IND 
		 	//	     when disconnected. So handle the following deactivation 
			// 	     in btmtk_ftps_disconnect_ind_handler(void *data).

			ftps_disconnect_client(0);
			break;

		case BT_FTPS_STATE_ACTIVE:
			// send a deregister 
			dereg_req = (goep_deregister_server_req_struct *) CONSTRUCT_LOCAL_PARAM(
					sizeof(goep_deregister_server_req_struct), TD_CTRL);

			dereg_req->goep_conn_id = ftps_cntx_p->goep_conn_id[0];

			bt_ext_log("[BT][FTP] dereg, goep_conn_id: %d", dereg_req->goep_conn_id);

			bt_ftp_send_msg(MSG_ID_BT_FTPS_DEREGISTER_SERVER_REQ,
					dereg_req,
					sizeof(goep_deregister_server_req_struct));
			// BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_DEACTIVATING);
			break;

		// other activated states
		default:
			// send SDP deregister primitive

			// send req to bt

			// only one sdp deregister req is enough
			return FALSE;

			break;
	}
	return TRUE; 
}

/**
* @brief This function is to reject rfcomm channel
* FTP Server Connect (Optional. The GAP_Authorization will take care this )
* bt_ftps_authorize_reject
* TODO: check by device's bdaddr
**/
BT_BOOL btmtk_ftps_authorize_reject(void)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	goep_authorize_rsp_struct *res;
	U32 u4PreviousState;

	FTPS_CS_ENTER();
	if (BT_FTPS_STATE_AUTHORIZING == (act_server_cntx_p)->ftps_state) {
		u4PreviousState = (act_server_cntx_p)->ftps_state;
		BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_ACTIVE);
	} else if (BT_FTPS_STATE_DEACTIVATING == (act_server_cntx_p)->ftps_state) {
		// Do-nothing here, just let go and reject.
		bt_ext_log("[ftps] Reject the authorize when deactivateing.");
	} else {
		FTPS_CS_LEAVE();
		return FALSE;
	}
	FTPS_CS_LEAVE();

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	/* find the current connection index */
	i = bt_ftps_find_goep_id(ftps_cntx_p->cur_goep_conn_id);

	// Send success connect response to OBEX task
	// BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_ACTIVE);

	// Construct goep push res ilm
	res = (goep_authorize_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_authorize_rsp_struct),TD_CTRL);
	GOEP_MEMSET( (void *)res, 0, sizeof(goep_authorize_rsp_struct));
	res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];
	res->rsp_code = GOEP_FORBIDDEN;
	bt_ftp_send_msg(MSG_ID_BT_FTPS_AUTHORIZE_RSP, res, sizeof(goep_authorize_rsp_struct) );

	return TRUE;
}



/**
* @brief This function is to accept rfcomm channel
* FTP Server Connect (Optional. The GAP_Authorization will take care this )
*
**/
BT_BOOL btmtk_ftps_authorize_accept(void)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	goep_authorize_rsp_struct *res;
	U32 u4PreviousState;

	FTPS_CS_ENTER();
	if (NULL != act_server_cntx_p) {
		GOEP_Report("[FTPS] authorize_accept state:(%d)", (act_server_cntx_p)->ftps_state);
	}
	if (BT_FTPS_STATE_AUTHORIZING != (act_server_cntx_p)->ftps_state) {
		FTPS_CS_LEAVE();
		return FALSE;    
	}
	u4PreviousState = (act_server_cntx_p)->ftps_state;
	//BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTING);
	FTPS_CS_LEAVE();
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	// find the current connection index
	i = bt_ftps_find_goep_id(ftps_cntx_p->cur_goep_conn_id);
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_AUTHORIZE_ACCEPT, i, (act_server_cntx_p + i)->ftps_state, ftps_cntx_p->cur_goep_conn_id) );

	// Send success connect response to OBEX task
	BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_ACTIVE);

	// for other apps query FTP connection in state active
	// accept == 0xFE, reject, delete, not authorize == 0xFF
	// connected states == 0, 1, 2,....
	ftps_cntx_p->g_conn_id[i] = 0xFE;

	// Construct goep push res ilm
	res = (goep_authorize_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_authorize_rsp_struct),TD_CTRL);
	GOEP_MEMSET( (U8*)res, 0, sizeof(goep_authorize_rsp_struct));
	res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];
	res->rsp_code = GOEP_STATUS_SUCCESS;
	bt_ftp_send_msg(MSG_ID_BT_FTPS_AUTHORIZE_RSP, res, sizeof(goep_authorize_rsp_struct) );

	return TRUE;
}



/** 
* @brief This function is to disconnect remote FTP client
* bt_ftps_disconnect_client
* @param remote bd_addr
* @param g_conn_id
**/
BT_BOOL ftps_disconnect_client(U8 index) //btbm_bd_addr_t bd_addr, U32 g_conn_id)
{

	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 u4PreviousState;
	BT_BOOL l_bRet = FALSE;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_DISCONNECT_CLIENT) );

	/* search the selected connection index */
	FTPS_CS_ENTER();
	u4PreviousState = (act_server_cntx_p)->ftps_state;
	if (BT_FTPS_STATE_IDLE == u4PreviousState ||
			BT_FTPS_STATE_REGISTERING == u4PreviousState) {

		FTPS_CS_LEAVE();
		return FALSE;    
	}

	if (u4PreviousState != BT_FTPS_STATE_DEACTIVATING) {
		BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_DISCONNECTING);
	} else {
		// When disconnect is triggered in deactivating, just keep the state.
	}

	FTPS_CS_LEAVE();

	switch (u4PreviousState)
	{
		case BT_FTPS_STATE_DISCONNECTING:  // disconnecting -> disconnecting
			//// BT_FTPS_LOG_STATE_ERR(i);
			BT_FTP_ASSERT(0);
			BT_FTPS_STATE_TRANS(0, u4PreviousState);
			return FALSE;
			// connected states
		case BT_FTPS_STATE_ACTIVE:
			//Connection sync to RFCOMM channel == Authorize
			//both accept and reject will send disconnect req to goep
		case BT_FTPS_STATE_CONNECTED:
		case BT_FTPS_STATE_RECEIVING:
		case BT_FTPS_STATE_SENDING:
			// switch ftps state to disconnecting
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_DISCONNECTING);
			// then do the same thing as state deactivating
			// deactivate triggered disconnecting
		case BT_FTPS_STATE_DEACTIVATING:
			// In this case, act_server_cntx_p will be cleared after received disconnect ind.
			// So raise the flag here to prevent accessing a null pointer.
			(act_server_cntx_p)->dis_from_user = TRUE;
			l_bRet = TRUE;
			// Send Disconnect REQ to goep
			bt_ftps_send_disconnect_req_msg(ftps_cntx_p->goep_conn_id[0]);
			// don't have to disconnect in states without connection

		default:
			// there is no connection exists -> call disconnect cnf to CM
			  
			return FALSE;
	}

	return l_bRet;
}

/**
* @brief Response the browsing request and return it with a folder xml file.
* @param ucRspCode [IN] response code
* @param ucXmlFilepath [IN] the filepath of xml file in Ucs2
*/
BT_BOOL btmtk_ftps_response_browsing_folder(U8 ucRspCode, U8 *ucXmlFilePath ){
	/// should invoke pull rsp to remote device and change state to pulling browsing xml file
	// xml file is composed done. read from file to send it out.
	bt_ftps_send_folder_content_routine(0, ucRspCode, ucXmlFilePath);
	return TRUE;
}

/**
* @brief Response the request of delete a file
* @param ucRspCode the response code 
* @see GOEP_STATUS
*/
BT_BOOL btmtk_ftps_response_push_delete_file(U8 ucRspCode) {
	U32 path_buf[128]; 
	path_buf[0] =0;

	FTPS_CS_ENTER();
	if (BT_FTPS_STATE_RECEIVING != (act_server_cntx_p)->ftps_state) {
		FTPS_CS_LEAVE();
		return FALSE;    
	}
	BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
	FTPS_CS_LEAVE();

	if (EXT_FS_OK != btmtk_goep_delete_file((U16 *) path_buf)) {
		bt_ftps_push_delete_routine_success_msg(GOEP_INTERNAL_SERVER_ERR);

	} else {
		// response ok to ftp client
		bt_ftps_push_delete_routine_success_msg(GOEP_STATUS_SUCCESS);
	}
	return TRUE;
}

/**
* @brief Response the request of delete a folder
* @param ucRspCode the response code 
* @see GOEP_STATUS
*/
BT_BOOL btmtk_ftps_response_push_delete_folder(U8 ucRspCode) {
	//TODO compare the ucObjName
	FTPS_CS_ENTER();
	if( BT_FTPS_STATE_RECEIVING != (act_server_cntx_p)->ftps_state ){
		FTPS_CS_LEAVE();
		return FALSE;    
	}
	BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
	FTPS_CS_LEAVE();

	btmtk_goep_delete_file( (const U16 *)(act_server_cntx_p)->ftps_obj_name);
	return TRUE;
}

/**
* @brief
* @param ucRspCode the response code 
*/
BT_BOOL btmtk_ftps_response_push_file(U8 ucRspCode, const U8 *ucFilePath) {
	FTPS_CS_ENTER();
	if (BT_FTPS_STATE_RECEIVING != (act_server_cntx_p)->ftps_state) {
		FTPS_CS_LEAVE();
		return FALSE;    
	}
	FTPS_CS_LEAVE();

	if (GOEP_STATUS_SUCCESS != ucRspCode) {
		// send back the error msg
		bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[0], ucRspCode);
	} else {
		// TODO: copy ucFilePath to ftps_cntx_p's target path
		bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[0], GOEP_STATUS_SUCCESS);
	}
	return TRUE;
}

/**
* @brief Response to BT_FTPSUI_PULL_FILE_START
*
* Allow remote to pull a file
* @param ucRspCode the response code
*/
BT_BOOL btmtk_ftps_response_pull_file(U8 ucRspCode, const U8 *ucFilePath) {
	BT_BOOL bMore;
	FTPS_CS_ENTER();
	if( BT_FTPS_STATE_SENDING != (act_server_cntx_p)->ftps_state ){
		FTPS_CS_LEAVE();
		return FALSE;    
	}
	FTPS_CS_LEAVE();

	bt_ftps_send_obj_routine(ucRspCode, ucFilePath, &bMore);
	return TRUE;
}

/**
* @brief Response to create-folder request and enter the folder when receving BT_FTPSUI_FILE_CREATE event
* @param ucRspCode the response code 
*/
BT_BOOL btmtk_ftps_response_create_folder(U8 ucRspCode) {
	U32 u4PreviousState;

	FTPS_CS_ENTER();
	if( BT_FTPS_STATE_CONNECTED != (act_server_cntx_p)->ftps_state ){
		FTPS_CS_LEAVE();
		return FALSE;    
	}
	u4PreviousState = (act_server_cntx_p)->ftps_state;
	BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
	FTPS_CS_LEAVE();

	bt_ftps_create_folder(0, NULL);
	return TRUE;
}

/**
* @brief Accept the set-path request when receive BT_FTPSUI_SET_FOLDER_START event
* FTP Server Change Folder
* @param ucRspCode the return code of OBEX set-path request
* @return TRUE accept the change folder request
*/
BT_BOOL btmtk_ftps_response_set_folder(U8 ucRspCode) {
	U32 u4PreviousState;

	// check the status, update the subpath, and send set_folder_rsp back
	FTPS_CS_ENTER();
	if( BT_FTPS_STATE_CONNECTED != (act_server_cntx_p)->ftps_state ){
		FTPS_CS_LEAVE();
		return FALSE;    
	}
	u4PreviousState = (act_server_cntx_p)->ftps_state;
	BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
	FTPS_CS_LEAVE();

	if (TRUE) {
		bt_ftps_set_folder(0, NULL); // current is entering one level
	} else {
		bt_ftps_set_parent_folder(0); // current is back one level
	}
	return TRUE;
}

/**
* @brief Setup up the read only of Server
* @param bReadOnly  to set up the server's read only attribute
* @param bResult The server is read only or not (after configure)
*/
BT_BOOL btmtk_ftps_configure_read_only(BT_BOOL bReadOnly, BT_BOOL *bResult) {
	return btmtk_ftps_setup_read_only(bReadOnly, bResult);
}

/**
* @brief Get the current FTPS status
* @return TRUE - status is valide
*/
BT_BOOL btmtk_ftps_get_status(bt_ftps_status_struct *status) {
	if (NULL == status || status->size != sizeof(bt_ftps_status_struct)) {
		return FALSE;
	}

	GOEP_MEMSET( (U8 *) status, 0, sizeof(bt_ftps_status_struct) );
	status->size = sizeof(bt_ftps_status_struct);
	if (NULL != act_server_cntx_p) {
		status->totalsize = act_server_cntx_p->total_send_obj_len;
		status->remain_len = act_server_cntx_p->remain_send_obj_len;
		status->ftps_state = act_server_cntx_p->ftps_state;
		GOEP_MEMCPY((U8*)&status->bdaddr, (const U8 *)&act_server_cntx_p->bdaddr, sizeof(act_server_cntx_p->bdaddr));
		GOEP_MEMCPY((U8*)status->ftps_filepath, (const U8 *)act_server_cntx_p->ftps_filepath, sizeof(status->ftps_filepath));
		GOEP_MEMCPY((U8*)status->new_folder, (const U8 *)act_server_cntx_p->new_folder, sizeof(status->new_folder));
		GOEP_MEMCPY((U8*)status->obj_name, (const U8 *)act_server_cntx_p->ftps_obj_name, sizeof(status->obj_name));
		status->challeng_options = act_server_cntx_p->challeng_options;
		GOEP_MEMCPY((U8*)status->realm, (const U8 *)act_server_cntx_p->realm, sizeof(status->realm));
		return TRUE;
	}
	return FALSE;
}

/**
* @brief access to get the current folder's xml file
* @see BT_FTPSUI_BROWSING_START
*/
BT_BOOL btmtk_ftps_access_folderxml(U32 u4RspCode, const U8 *ucXmlFilePath) {
	// GOEP_Report("[ftps] access_folderxml rsp:%d ", u4RspCode);

	if( BT_FTPS_STATE_SEND_WAIT == (act_server_cntx_p)->ftps_state ){
		bt_ftps_send_folder_content_routine(0, u4RspCode, (const U8 *)ucXmlFilePath);
		return TRUE;
	}
	return FALSE;
}

BT_BOOL btmtk_ftps_access_change_folder(U8 ucRspcode,const U8 *ucFolderPath)
{   
	GOEP_Report( "[fts] func:accesss_change_folder Rspcode=%d\n", ucRspcode );

	if (BT_FTPS_STATE_SETPATH_WAIT == (act_server_cntx_p)->ftps_state)
	{
		// copy to ftps_filepath
		if (0 == ucRspcode) {
			ext_ucs2ncpy((S8 *) act_server_cntx_p->ftps_filepath, (const S8 *) ucFolderPath , sizeof(act_server_cntx_p->ftps_filepath) /2);
			ext_ucs2ncpy((S8 *) act_server_cntx_p->current_folder, (const S8 *) act_server_cntx_p->new_folder,
					sizeof(act_server_cntx_p->current_folder) /2);
			
			bt_ftps_send_set_folder_rsp(0, ucRspcode);
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
			// btmtk_ftps_ui_callback(BT_FTPSUI_SET_FOLDERED, (void *)ucRspcode); 

		} else {

			// fail !
			bt_ftps_send_set_folder_rsp(0, ucRspcode);
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);            
		}
		return TRUE;
	} else {

	}
	return FALSE;
}

BT_BOOL btmtk_ftps_gen_xmlfile(U8 *ucFolderPath, const U8 *ucXmlpath, U32 u4MaxRecord , bt_ftps_folder_filter *filter)
{
	// generate a folder's xml file and put it to file
	BT_BOOL isRoot;
	isRoot = FALSE;

	if (EXT_FS_OK == bt_ftps_gen_folder_content(&act_server_cntx_p->xml_composer,
			isRoot, ucFolderPath, (const U8 *)ucXmlpath, 0, filter))
	{
		return TRUE;
	}

	return FALSE;

}

BT_BOOL btmtk_ftps_access_put_file(U8 ucRspCode,const U8 *ucFolderPath, const U8 *ucFilename)
{   
	BT_BOOL bMore;

	if (BT_FTPS_STATE_RECEIVE_WAIT == (act_server_cntx_p)->ftps_state) {
		// save buffer into file and send the 
		if (GOEP_STATUS_SUCCESS == ucRspCode) {
			/// copy file to ftps_filepath
			if (NULL != ucFolderPath || NULL != ucFilename) {
				btmtk_goep_compose_filepath((const U8 *) ucFolderPath,
						(const U8 *)ucFilename,
						(U8 *)act_server_cntx_p->ftps_filepath,
						sizeof(act_server_cntx_p->ftps_filepath) /2);
			}

			// open file 
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_RECEIVING);
			bt_ftps_push_obj(0, act_server_cntx_p->ftps_filepath,
					act_server_cntx_p->last_pkttype,
					act_server_cntx_p->ftps_obj_buff,
					act_server_cntx_p->last_pktlen, &bMore);

			if (FALSE!= bMore) {
				BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_RECEIVING);
			}
		} else {
			// change to connected status
			// push_rsp
			bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[0], ucRspCode);
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
		}

		return TRUE;
	}
	return FALSE;
}

BT_BOOL btmtk_ftps_access_get_file(U8 ucRspCode,const U8 *ucFolderPath, const U8 *ucFilename)
{
	BT_BOOL bMore;
	if (BT_FTPS_STATE_SEND_WAIT == (act_server_cntx_p)->ftps_state)	{
		/// copy file to ftps_filepath
		if (NULL != ucFolderPath || NULL != ucFilename) {
			btmtk_goep_compose_filepath((const U8 *) ucFolderPath,
					(const U8 *)ucFilename,
					(U8 *)act_server_cntx_p->ftps_filepath,
					sizeof(act_server_cntx_p->ftps_filepath) /2);
		}

		if (FALSE != bt_ftps_send_obj_routine( ucRspCode,  act_server_cntx_p->ftps_filepath, &bMore)) {
			(act_server_cntx_p)->send_obj_type = BT_FTP_OBJ_TYPE_FILE;
			// send the default xml file back
			if (TRUE == bMore) {
				// btmtk_ftps_ui_callback(BT_FTPSUI_PULLING, 
				//		(void *)(act_server_cntx_p->total_send_obj_len- act_server_cntx_p->remain_send_obj_len));
				BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_SENDING);
			} else {
				// btmtk_ftps_ui_callback( BT_FTPSUI_PULLED, (void *) GOEP_STATUS_SUCCESS); 
				BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
			}
			
		} else {
			// btmtk_ftps_ui_callback( BT_FTPSUI_BROWSED, (void *)GOEP_INTERNAL_SERVER_ERR); 
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
		}
		return TRUE;
	}
	return FALSE;

}

BT_BOOL btmtk_ftps_access_make_folder(U8 ucRspCode,const U8  *ucFolderPath,const U8 *ucNewFolderName)
{

	if (BT_FTPS_STATE_SETPATH_WAIT == (act_server_cntx_p)->ftps_state) {
		// create folder and enter the folder and update the current folder
		if (NULL != ucFolderPath || NULL != ucNewFolderName) {
			btmtk_goep_compose_filepath((const U8 *) ucFolderPath,
					(const U8 *)ucNewFolderName,
					(U8 *)act_server_cntx_p->ftps_filepath,
					sizeof(act_server_cntx_p->ftps_filepath) /2);
		}

		if (GOEP_STATUS_SUCCESS == ucRspCode) {
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
			if (FALSE == btmtk_goep_is_folder_exist( (const U16 *) act_server_cntx_p->ftps_filepath)) {
				// make the folder
				GOEP_Report("[BT][FTP] make_folder - check that target folder is not exit ");
				if (EXT_FS_OK == btmtk_goep_create_folder( (const U16 *) act_server_cntx_p->ftps_filepath)) {
				} else {
					GOEP_Report("[BT][FTP] make_folder - fail to create target folder ");	
					bt_ftps_send_set_folder_rsp(0, GOEP_INTERNAL_SERVER_ERR);
					return FALSE;
				}
			} else {
				GOEP_Report("[BT][FTP] make_folder - target folder is exist. no need to create it ");	
			}
			if (FALSE == btmtk_goep_is_folder_exist( (const U16 *) act_server_cntx_p->ftps_filepath)) {
				GOEP_Report("[BT][FTP] make_folder - target folder is not exist ");	
				bt_ftps_send_set_folder_rsp(0, GOEP_INTERNAL_SERVER_ERR);
				return FALSE;
			}
			ext_ucs2ncpy((S8 *) act_server_cntx_p->new_folder,
					(const S8 *) act_server_cntx_p->current_folder,
					sizeof( act_server_cntx_p->current_folder));

			btmtk_goep_compose_folderpath((const U8 *)act_server_cntx_p->new_folder,
					(const U8 *) ucNewFolderName,
					(U8 *) act_server_cntx_p->current_folder,
					sizeof(act_server_cntx_p->current_folder)/2);
			/// ucFolderPath is the real path. current_folder is the virtual path

			bt_ftps_send_set_folder_rsp(0,ucRspCode);
		} else {
			// setpath
			bt_ftps_send_set_folder_rsp(0,ucRspCode);
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
		}
		return TRUE;
	}
	return FALSE;
}

BT_BOOL btmtk_ftps_access_del_file(U8 ucRspCode,const U8 *ucFolderPath, const U8 *ucFileName)
{
	if( BT_FTPS_STATE_RECEIVE_WAIT == (act_server_cntx_p)->ftps_state )
	{
		//obj_name  delete the file

		if (GOEP_STATUS_SUCCESS == ucRspCode) {
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);

			if (NULL != ucFolderPath || NULL != ucFileName) {
				btmtk_goep_compose_filepath((const U8 *) ucFolderPath,
						(const U8 *)ucFileName,
						(U8 *)act_server_cntx_p->ftps_filepath,
						sizeof(act_server_cntx_p->ftps_filepath) /2);
			}

			if (FALSE != btmtk_goep_is_folder_exist((const U16 *) act_server_cntx_p->ftps_filepath)) {
				btmtk_goep_delete_folder((const U16 *) act_server_cntx_p->ftps_filepath);
				if (FALSE == btmtk_goep_is_folder_exist((const U16 *) act_server_cntx_p->ftps_filepath)) {
					bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[0], ucRspCode);
				} else {
					bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[0], GOEP_INTERNAL_SERVER_ERR);
				}
			} else {
				btmtk_goep_delete_file((const U16 *) act_server_cntx_p->ftps_filepath);
				if (FALSE == btmtk_goep_is_file_exist((const U16 *) act_server_cntx_p->ftps_filepath)) {
					bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[0], ucRspCode);
				} else {
					bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[0], GOEP_INTERNAL_SERVER_ERR);
				}
			}

		} else {
			// push_rsp 
			bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[0], ucRspCode);
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
		}
		return TRUE;
	}
	return FALSE;
}

BT_BOOL btmtk_ftps_access_del_folder(U8 ucRspCode,const U8 *ucFolderPath, const U8 *ucNewFolderName)
{

	if (BT_FTPS_STATE_RECEIVE_WAIT == (act_server_cntx_p)->ftps_state) {
		//obj_name  delete the file

		if (GOEP_STATUS_SUCCESS == ucRspCode) {
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);

			if (NULL != ucFolderPath || NULL != ucNewFolderName) {
				btmtk_goep_compose_filepath((const U8 *) ucFolderPath,
						(const U8 *)ucNewFolderName,
						(U8 *)act_server_cntx_p->ftps_filepath,
						sizeof(act_server_cntx_p->ftps_filepath) /2);
			}

			btmtk_goep_delete_folder( (const U16 *) act_server_cntx_p->ftps_filepath );
			if (FALSE == btmtk_goep_is_folder_exist((const U16 *) act_server_cntx_p->ftps_filepath)) {
				bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[0], ucRspCode);
			} else {
				bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[0], GOEP_INTERNAL_SERVER_ERR);
			}
		} else {
			// push_rsp 
			bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[0], ucRspCode);
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
		}
		return TRUE;
	}
	return FALSE;
}


/**
* @brief Disconnect the remote client device
*/
BT_BOOL btmtk_ftps_disconnect() {
	U32 u4PreviousState;

	// check the status, update the subpath, and send set_folder_rsp back
	FTPS_CS_ENTER();
	if (NULL == act_server_cntx_p) {
		FTPS_CS_LEAVE();
		return FALSE;
	}

	u4PreviousState = (act_server_cntx_p)->ftps_state;
	if (BT_FTPS_STATE_DISCONNECTING == u4PreviousState || BT_FTPS_STATE_IDLE == u4PreviousState) {
		FTPS_CS_LEAVE();
		return FALSE;    
	}

	if (BT_FTPS_STATE_REGISTERING == u4PreviousState ||
		BT_FTPS_STATE_DEACTIVATING == u4PreviousState ||
		BT_FTPS_STATE_ACTIVE == u4PreviousState) {
		FTPS_CS_LEAVE();
		return FALSE;    
	}

	(act_server_cntx_p)->dis_from_user = TRUE;
	switch ((act_server_cntx_p)->ftps_state) {
		case BT_FTPS_STATE_RECEIVE_WAIT:
			// mark as wait-disc
			break;

		case BT_FTPS_STATE_SEND_WAIT:
			// mark as wait-disc
			break;

		case BT_FTPS_STATE_RECEIVING:
		case BT_FTPS_STATE_SENDING:
			// mark as wait-disc, and 
			// (act_server_cntx_p)->dis_from_user = TRUE;
			// FTPS_CS_LEAVE();
			// return TRUE;
			break;

		case BT_FTPS_STATE_DEACTIVATING:
			// do nothing
			return TRUE;
			break;

		default:
			// send disconnect direct
			break;
	}

	BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_DISCONNECTING);
	FTPS_CS_LEAVE();

	// Send Disconnect REQ to goep
	bt_ftps_send_disconnect_req_msg(ftps_cntx_p->goep_conn_id[0]);
	(act_server_cntx_p)->dis_from_user = TRUE;

	return TRUE;
}

BT_BOOL btmtk_ftps_abort() {
	if (NULL == act_server_cntx_p) {
		return FALSE;
	}

    if (TRUE == act_server_cntx_p->flag_abort_req_sent) {
		return TRUE;
    }

    /// abort the current sending or receiving
    switch (act_server_cntx_p->ftps_state) {
		case BT_FTPS_STATE_SENDING:
		case BT_FTPS_STATE_RECEIVING:
		    act_server_cntx_p->flag_abort_req_sent = TRUE;
		    return TRUE;

		default:
		    return FALSE;
    }
}

BT_BOOL btmtk_ftps_abortfile_req() {
	U8 rspcode = GOEP_FORBIDDEN;
	// check the status, update the subpath, and send set_folder_rsp back
	FTPS_CS_ENTER();
	if(NULL == act_server_cntx_p) {
		FTPS_CS_LEAVE();
		return FALSE;
	}

	/* Send Disconnect REQ to goep */
	bt_ftps_send_abortfile_req_msg(ftps_cntx_p->goep_conn_id[0], rspcode);
	(act_server_cntx_p)->dis_from_user = TRUE;
	FTPS_CS_LEAVE();

	return TRUE;
}

BT_BOOL btmtk_ftps_set_root_folder(const U8 *ucRootFolderpath)
{
	// copy it to root
	if (0 == ext_ucs2strlen((const S8 *)ucRootFolderpath)) {
		GOEP_MEMSET( g_ftps_root_folder , 0 , 4);
		return TRUE;
	}
	ext_ucs2ncpy((S8 *) g_ftps_root_folder, (const S8 *) ucRootFolderpath, 64);
	if (ext_ucs2strlen((const S8 *) ucRootFolderpath)== ext_ucs2strlen((const S8 *) g_ftps_root_folder)) {
		return TRUE;
	}
	return FALSE;
}


BT_BOOL btmtk_ftps_authentication_response(const U8 *ucUserid, const U8 *ucPwd, U32 u4PwdLen) {
	BT_BOOL l_ret;
	l_ret = FALSE;

	/// compose a auth_rsp and send it back
	switch ((act_server_cntx_p)->ftps_state) {
		case BT_FTPSUI_AUTHEN_WAIT:
			// check BT_FTPSUI_AUTHEN_WAIT
			l_ret = TRUE;

			if (u4PwdLen > 0) {
				bt_ftps_send_auth_rsp_msg(ftps_cntx_p->goep_conn_id[0], ucUserid, ucPwd, u4PwdLen);
				BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);  // Fix: state enum doesn't have UI

			} else {
				GOEP_Report("[FTPS] btmtk_ftps_authentication_response reject it"); // reject it
				ftps_disconnect_client(0);
			}

			break;

		default:
			break;
	}

	return l_ret;
}

#endif
