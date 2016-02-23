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
 * bt_ftpc_api.c
 *
 * Project:
 * --------
 *   Export function from FTP Client
 *
 * Description:
 * ------------
 *   This file is for MMI FTP Client.
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
#include "ext_osal.h"
#include "ext_nbuffer.h"
#include "bluetooth_ftps_struct.h"
#include "bluetooth_ftpc_struct.h"
#include "bt_goep_fs.h"
#include "bt_ftpc_struct.h"
#include "bt_ftpc_event.h"
#include "bt_ftpc_api.h"
#include "bt_ftpc_hdl.h"

#include "ext_ucs2.h"
#include "bt_ftp_porting.h"
#include "bt_ext_debug.h"

#define GOEP_Report bt_ext_log

#if defined(__BT_FTC_PROFILE__)
extern bt_ftpc_cntx_struct *const ftpc_cntx_p;
extern bt_ftpc_conn_cntx_struct *act_client_cntx_p;

BTMTK_FTPC_CALLBACK g_ftpc_ui_callback = NULL;
static const unsigned char OBEX_FTP_UUID[] = { 0xF9, 0xEC, 0x7B, 0xC4, 0x95, 0x3C, 0x11, 0xD2,
                                   0x98, 0x4E, 0x52, 0x54, 0x00, 0xDC, 0x9E, 0x09 };

extern void bt_ftpc_send_abortfile_req_msg(U8 goep_conn_id, U8 reason);
extern BT_BOOL bt_ftpc_start_pushfile_routine(const U8 *ucFolderpath, const U8 *ucFilename, U8 srm_enable);
/**
* @brief  This function is to initialize FTPC context
* FTP Client Enable
* 
**/
BT_BOOL btmtk_ftpc_client_enable(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GOEP_TRACE ((BT_FTP_TRC_CLASS, BT_FTPC_CNTX_INIT));
    if (FALSE != ftpc_cntx_p->enable) {
    	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_CNTX_INIT_WRN) );
    	return TRUE;
    }

	// ftpc context init
    act_client_cntx_p = &(ftpc_cntx_p->conn_cntx[0]);
    ftpc_cntx_p->ftpc_flag = 0;
    ftpc_cntx_p->curr_idx = 0xFF;
    ftpc_cntx_p->enable = TRUE;

    for (i = 0; i < FTP_MAX_SERVER_SUPPORT; i++) {
        ftpc_cntx_p->goep_conn_id[i] = 0xFF;
        ftpc_cntx_p->req_id[i] = 0xFF;
        ftpc_cntx_p->g_conn_id[i] = 0xFF;

        (act_client_cntx_p + i)->got_root = 0;
        (act_client_cntx_p + i)->browse_depth = 0;
        (act_client_cntx_p + i)->ftpc_total_len = 0;
        (act_client_cntx_p + i)->ftpc_reamin_len = 0;
        (act_client_cntx_p + i)->entity_cnt_cur_folder = 0;
        (act_client_cntx_p + i)->xml_parser_p = NULL;
        (act_client_cntx_p + i)->fh_ftpc_recv = (FHANDLE) FHANDLE_INVALID_VALUE;
        (act_client_cntx_p + i)->ftpc_state = BT_FTPC_STATE_IDLE;
        (act_client_cntx_p + i)->ftpc_data_len = 0;
        (act_client_cntx_p + i)->total_len_type = BT_FTPC_TOTAL_LEN_NO_FIRST_PKT;
        (act_client_cntx_p + i)->fh_ftpc_push = FHANDLE_INVALID_VALUE;
        (act_client_cntx_p + i)->ftpc_push_remain = 0;
        (act_client_cntx_p + i)->ftpc_use_nbuffer = FALSE;

        FTPC_MAKE_FOLDER_OBJECT((act_client_cntx_p + i)->folder_obj, i);
 
        GOEP_MEMSET((U8 *) &((act_client_cntx_p + i)->ftpc_bt_device), 0, sizeof(goep_bd_addr_struct));
        GOEP_MEMSET((U8 *) &(act_client_cntx_p + i)->ftpc_obj_name, 0, FMGR_MAX_PATH_LEN);
        GOEP_MEMSET((U8 *) &(act_client_cntx_p + i)->ftpc_filepath, 0, sizeof((act_client_cntx_p + i)->ftpc_filepath));
        
        bt_ftpc_reset_abort_req_sent_flag();

		// get the shared buffer for ftpc
     	(act_client_cntx_p + i)->ftpc_obj_buff_size = FTPC_MEM_BUFF_SIZE;
        (act_client_cntx_p + i)->ftpc_obj_buff = 
		    (U8*) btmtk_ftpc_get_profile_shared_int_buffer((act_client_cntx_p + i)->ftpc_obj_buff_size);

        //BT_FTP_ASSERT((act_client_cntx_p + i)->ftpc_obj_buff);
#ifdef __BT_GOEP_V2__ 
        // no need buffer
#else
	if (!((act_client_cntx_p + i)->ftpc_obj_buff)) {
		bt_ext_err("[BT][FTP] Failed to initialize the shared buffer for client.");
		ftpc_cntx_p->enable = FALSE;
		return FALSE;
	}
#endif

        (act_client_cntx_p + i)->ftpc_peer_mru = 512; // spec min value of OBEX support size
  
        (act_client_cntx_p + i)->discon_from_cm = FALSE;
    }
    return TRUE;

}

/**
* @brief FTP Client Deinit
* FTP Client Disable
*/
BT_BOOL btmtk_ftpc_client_disable() {
    goep_disconnect_req_struct *req;

    /// disconnect all client and 
    GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_CNTX_DEINIT));

    if(NULL == act_client_cntx_p || FALSE == ftpc_cntx_p->enable) {
        // has been disable
        return TRUE;
    }

    ftpc_cntx_p->enable = FALSE;

    switch (act_client_cntx_p->ftpc_state) {
        case BT_FTPC_STATE_IDLE:
        case BT_FTPC_STATE_DISCONNECTING:
	    break;

	default:
			// construct disconnect req ilm
            req = (goep_disconnect_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_disconnect_req_struct), TD_CTRL);
            GOEP_MEMSET((U8 *)req, 0, sizeof(goep_disconnect_req_struct));
            req->req_id = ftpc_cntx_p->req_id[0];
            req->goep_conn_id = 0xFF;
            GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
            req->uuid_len = 16;
            
			// send disconnect req and connect cnf fail to CM
            bt_ftp_send_msg(MSG_ID_BT_FTPC_DISCONNECT_REQ, req, sizeof(goep_disconnect_req_struct));

            BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_DISCONNECTING);			
	    break;
    }
    return TRUE;
}

void btmtk_deinit_ftp_client(void){
    GOEP_Report("[FTPC] btmtk_deinit_ftp_client");
    ftpc_cntx_p->enable = FALSE;
    act_client_cntx_p = NULL;
    GOEP_MEMSET( (U8 *) ftpc_cntx_p, 0, sizeof(ftpc_cntx_p));
}

/**
* @brief This function is to connect server for ftp service
* FTP Client Connect
*  btmtk_ftpc_connect_server
*  
* @param bd_addr remote device bd address
* @param req_id optional parameter
* @return TRUE send connect reqesut success and wait for BT_FTPCUI_CONNECTED event or BT_FTPCUI_DISCONNECTED event
* @see BT_FTPCUI_EVNT_T
**/
BT_BOOL btmtk_ftpc_connect_server(btbm_bd_addr_t bd_addr)
{
    OS_Report("[BT30] btmtk_ftpc_connect_server ");
	return btmtk_ftpc_connect_server_ex(bd_addr, 0); /* default is 2 for test QDAY*/
}

BT_BOOL btmtk_ftpc_connect_server_ex(btbm_bd_addr_t bd_addr, U8 type){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    static U8 req_id =0;
    U32 i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_CONNECT_SERVER) );

    if( NULL == act_client_cntx_p || FALSE == ftpc_cntx_p->enable ){
        GOEP_Report("[ftpc][int] func:connect_server Not active !");
        return FALSE;
    }

	// Reset root folder content flag
    if( NULL == act_client_cntx_p || act_client_cntx_p != &(ftpc_cntx_p->conn_cntx[0]) ){
        GOEP_Report("[ftpc][int] func:connect_server Not active !");
        return FALSE;
    }

    req_id++;
    for (i = 0; i < FTP_MAX_SERVER_SUPPORT; i++) {
        goep_connect_req_struct *req;

        // TOOD: UI notify the connting to remoet device'addr

        if ((act_client_cntx_p + i)->ftpc_state == BT_FTPC_STATE_IDLE) {
			// update context
            ftpc_cntx_p->req_id[i] = req_id;
            (act_client_cntx_p + i)->got_root = 0;
            
            (act_client_cntx_p + i)->ftpc_bt_device.lap = bd_addr.lap;
            (act_client_cntx_p + i)->ftpc_bt_device.uap = bd_addr.uap;
            (act_client_cntx_p + i)->ftpc_bt_device.nap = bd_addr.nap;
            (act_client_cntx_p + i)->challeng_options = 0;
            GOEP_MEMSET( (act_client_cntx_p + i)->realm, 0, sizeof((act_client_cntx_p + i)->realm) );

			// connect server
			// film res pkt_type attibute
            //FTP_DEBUG(kal_print("FTPC CONNECT_SERVER TRY TO FILM ILM\n");)

            if (FALSE == ext_nbuffer_isInit(&act_client_cntx_p->ftpc_mempool)) {
                if (FALSE == ext_nbuffer_init(&act_client_cntx_p->ftpc_mempool,
			act_client_cntx_p->ftpc_obj_buff_size)) {
                    /// fail to alloc nbuffer
                    GOEP_Report( "[FTPS][WRN] btmtk_ftpc_connect_server ext_nbuffer_init fail");
                }
            }

            req = (goep_connect_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_connect_req_struct),TD_CTRL);
            GOEP_MEMSET( (U8 *)req, 0, sizeof(goep_connect_req_struct));
            req->bd_addr.lap = bd_addr.lap;
            req->bd_addr.uap = bd_addr.uap;
            req->bd_addr.nap = bd_addr.nap;
            req->buf_ptr = (act_client_cntx_p + i)->ftpc_obj_buff;
            req->buf_size = (U16) (act_client_cntx_p+ i)->ftpc_obj_buff_size; //FTPC_MEM_BUFF_SIZE;
#ifdef __BT_GOEP_V2__ 
            req->buf_size = (0xfff0); // 10M unit
#endif
            req->req_id = req_id;
            req->tp_type = GOEP_TP_BT;
            req->tptype = type; /* rfcomm or l2cap or both */
	    GOEP_Report("[FTPC] req->tptype:%d", type);            
            (act_client_cntx_p + i)->discon_from_cm = FALSE;

            GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
            req->uuid_len = 16;

            /* send connect req to server */
            BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTING);
            bt_ftp_send_msg(MSG_ID_BT_FTPC_CONNECT_REQ, req ,sizeof(goep_connect_req_struct));
     
	    return TRUE;

        } else {
            GOEP_Report( "[ftpc][int] func:connect_server wrong state:%d\n", act_client_cntx_p->ftpc_state );
        }
    }
    
    return FALSE;
}

/**
* @brief This is common function to abort connecting
* Abort the connecting 
**/
BT_BOOL btmtk_ftpc_connect_cancel()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_disconnect_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_CONNECT_CANCEL, act_client_cntx_p->ftpc_state) );
    if( NULL == act_client_cntx_p ){
        //TODO: fault error. log
        return FALSE;
    }

    switch(act_client_cntx_p->ftpc_state)
    {
        case BT_FTPC_STATE_IDLE:
           return FALSE;

        case BT_FTPC_STATE_GETTING_FOLDER:
            bt_ftpc_send_tpdisconnect_message(0);
            BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_TPDISCONNECTING);
            return TRUE;

        case BT_FTPC_STATE_CONNECTING:
            // construct disconnect req ilm
            req = (goep_disconnect_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_disconnect_req_struct),TD_CTRL);
            GOEP_MEMSET( (U8 *)req, 0, sizeof(goep_disconnect_req_struct));
            req->req_id = ftpc_cntx_p->req_id[0];
            req->goep_conn_id = 0xFF;
            GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
            req->uuid_len = 16;
            
            // send disconnect req and connect cnf fail to CM
            bt_ftp_send_msg(MSG_ID_BT_FTPC_DISCONNECT_REQ, req, sizeof(goep_disconnect_req_struct) );

            BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_DISCONNECTING);
            return TRUE;
	default:
	    break;
    }
    return FALSE;
}


/**
* @brief This function is to get file object.
* FTP Client Get File
*/
BT_BOOL btmtk_ftpc_get_file_obj(const U8 *local_path, const U8 *ucName, const U8 *ucNewName)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //U32 i;
    goep_pull_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_GET_FILE_OBJ, act_client_cntx_p->ftpc_state));
    if (NULL == act_client_cntx_p) {
        //TODO fault error
        return FALSE;
    }

    if (NULL == local_path || FALSE == btmtk_goep_is_folder_exist((const U16 *) local_path)) {
	GOEP_Report("[ftpc] func:get_file_obj  wrong foldpath parameter\n");
	return FALSE;
    }

    if( NULL == ucName || 0 == ext_ucs2strlen( (const S8 *) ucName ) ){
	GOEP_Report( "[ftpc] func:get_file_obj  wrong filename parameter\n");
	return FALSE;
    }

    // Init the statistics for received object length
    act_client_cntx_p->ftpc_total_len = 0;
    act_client_cntx_p->ftpc_reamin_len = 0;
    act_client_cntx_p->ftpc_data_len = 0;
    act_client_cntx_p->total_len_type = BT_FTPC_TOTAL_LEN_NO_FIRST_PKT;
    act_client_cntx_p->flag_abort_req_sent = FALSE;

    GOEP_Report("[FTPC] get_file_obj set default attribute = 0");

    ext_ucs2ncpy((S8 *) act_client_cntx_p->push_local_path,
	    (S8 *)local_path,
	    sizeof(act_client_cntx_p->push_local_path)/2);

    if (NULL == ucNewName || 0 == ext_ucs2strlen((const S8 *) ucNewName)) {
	GOEP_Report("[FTPC] get_file_obj compose name");
	bt_ftp_compose_path(local_path, ucName,
		(U8 *) act_client_cntx_p->ftpc_filepath,
		sizeof(act_client_cntx_p->ftpc_filepath)/2);

    } else {
	bt_ftp_compose_path(local_path,	ucNewName,
		(U8 *) act_client_cntx_p->ftpc_filepath,
		sizeof(act_client_cntx_p->ftpc_filepath)/2);
    }

    GOEP_Report("[FTPC] get_file_obj set default attribute = 0");
    ext_ucs2ncpy((S8 *) act_client_cntx_p->ftpc_obj_name,
	    (S8 *) ucName,
	    sizeof(act_client_cntx_p->ftpc_obj_name));

    if (!ucName) {
        GOEP_Report("[ftpc][wrn] func:get_file_obj wrong param\n");
        BT_FTP_ASSERT(0);
        return FALSE;
    }

    switch (act_client_cntx_p->ftpc_state) {
        case BT_FTPC_STATE_CONNECTED:
            BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_GETTING_OBJ);
            break;

        default: 
            // Incorrect state, callback and return error
            GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));
            return FALSE;
    }

    // send get obj req
    GOEP_Report("[FTPC] get_file_obj prepare msg");
    req = (goep_pull_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_pull_req_struct),TD_CTRL);
    GOEP_MEMSET((U8 *)req, 0, sizeof(goep_pull_req_struct));
    req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
    req->pkt_type = GOEP_FIRST_PKT;
    GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
    req->uuid_len = 16;

    ext_ucs2ncpy((S8 *) req->obj_name,
	    (S8 *) act_client_cntx_p->ftpc_obj_name,
	    sizeof(req->obj_name)/2 );

    req->obj_name[sizeof(req->obj_name)-2] = req->obj_name[sizeof(req->obj_name)-1] = 0;
    bt_ftp_ucs2_htons((U8 *)req->obj_name, (U8 *)req->obj_name);
    bt_ftp_send_msg(MSG_ID_BT_FTPC_PULL_REQ, req,sizeof(goep_pull_req_struct));
    return TRUE;
}

/**
* @brief This function is to browse a new folder or refresh the current folder
* FTP Client Change Folder
* Only could enter one level directory
* @param name the sub folder to enter
* @return TRUE send request success wait for changed BT_FTPCUI_SETPATHED event
* @see BT_FTPCUI_EVNT_T
**/
BT_BOOL btmtk_ftpc_enter_folder(const U16 * name)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	goep_set_folder_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_CHANGE_FOLDER, act_client_cntx_p->ftpc_state, ftpc_cntx_p->goep_conn_id[0]));

    if (NULL == act_client_cntx_p) {
        //TODO fault error
        return FALSE;
    }

	// refresh current folder

    GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_CHANGE_FOLDER_FORWARD));

    switch (act_client_cntx_p->ftpc_state) {
	case BT_FTPC_STATE_CONNECTED:
	    BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_SETTING_FOLDER);
            break;

        default:
			// Incorrect state, callback and return error
            GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));
            return FALSE;
    }

    GOEP_Report("[FTPC] enter_folder setup copy obj_name");
    GOEP_MEMSET((U8 *)act_client_cntx_p->ftpc_obj_name, 0, sizeof(act_client_cntx_p->ftpc_obj_name));

    ext_ucs2ncpy((S8 *) act_client_cntx_p->ftpc_obj_name,
	    (const S8 *) name,
	    (sizeof(act_client_cntx_p->ftpc_obj_name)-2)/2);

    GOEP_TRACE_PATH(BT_FTP_TRC_CLASS, BT_FTPC_TARGET_FILENAME, act_client_cntx_p->ftpc_obj_name);
        
    GOEP_Report("[FTPC] enter_folder setup build a msg");

	// send set folder req
    req = (goep_set_folder_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_set_folder_req_struct), TD_CTRL);
    GOEP_MEMSET((U8 *)req, 0, sizeof(goep_set_folder_req_struct));
    req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
    GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
    req->uuid_len = 16;

    if (2 == ext_ucs2strlen((const S8 *) act_client_cntx_p->ftpc_obj_name) &&
	0 == ext_ucs2cmp((const U8 *) L"..", (const U8 *)act_client_cntx_p->ftpc_obj_name)) {
	    req->setpath_flag = GOEP_BACK_FOLDER;

    } else {
       	GOEP_Report("[FTPC] enter_folder setup msg's folder_name");
        req->setpath_flag = GOEP_FORWARD_FOLDER;
        ext_ucs2ncpy((S8 *) req->folder_name,
		(S8 *) act_client_cntx_p->ftpc_obj_name,
		sizeof(req->folder_name)/2);
    }

    req->folder_name[sizeof(req->folder_name)-2] = req->folder_name[sizeof(req->folder_name)-1] = 0;

    bt_ftp_ucs2_htons((U8 *)req->folder_name, (U8 *)req->folder_name);

    bt_ftp_send_msg(MSG_ID_BT_FTPC_SET_FOLDER_REQ, req ,sizeof(goep_set_folder_req_struct));
    return TRUE;
}


/**
* @brief This function is to go to the root folder.
* FTP Client Change Folder (optional)
* @return TRUE send request success. wait for BT_FTPCUI_SETPATHED event
**/
BT_BOOL btmtk_ftpc_goto_root_folder(void)
{
    // TODO check the state and 
    
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GOEP_TRACE ((BT_FTP_TRC_CLASS, BT_FTPC_ROOT_FOLDER, act_client_cntx_p->ftpc_state, ftpc_cntx_p->goep_conn_id[0]));

    if (NULL == act_client_cntx_p) {
        //TODO fault error
        return FALSE;
    }

	// Change MMI FTC state
    switch (act_client_cntx_p->ftpc_state) {
        case BT_FTPC_STATE_CONNECTED: {
	    goep_set_folder_req_struct *req;

	    BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_SETTING_ROOT_FOLDER);

			// construct set folder req ilm
            req = (goep_set_folder_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_set_folder_req_struct),TD_CTRL);
            GOEP_MEMSET( (U8 *)req, 0, sizeof(goep_set_folder_req_struct));
            req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
            req->setpath_flag = GOEP_ROOT_FOLDER;
            GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
            req->uuid_len = 16;

			// send set folder req
            bt_ftp_send_msg(MSG_ID_BT_FTPC_SET_FOLDER_REQ, req ,sizeof(goep_set_folder_req_struct));
            break;
        }

        default:    /* Incorrect state */
	// Incorrect state, callback and return error
	// It could be in pulling state. should not close the recv file handle
            GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));
            return FALSE;
    }
    return TRUE;
}

/**
* @brief This function is to go to the parent folder.
* FTP Client Change Folder (optional)
* @return TRUE send request success. wait for BT_FTPCUI_SETPATHED event
**/
BT_BOOL btmtk_ftpc_back_folder(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_BACK_FOLDER, act_client_cntx_p->ftpc_state, ftpc_cntx_p->goep_conn_id[0]));

    if (NULL == act_client_cntx_p) {
        return FALSE;
    }

    /* Change MMI FTPC state */
    switch (act_client_cntx_p->ftpc_state) {

        case BT_FTPC_STATE_CONNECTED: {
            goep_set_folder_req_struct *req;

            BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_SETTING_BACK_FOLDER);

            /* construct set folder req ilm */
            req = (goep_set_folder_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_set_folder_req_struct),TD_CTRL);
            GOEP_MEMSET( (U8 *)req, 0, sizeof(goep_set_folder_req_struct));
            req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
            req->setpath_flag = GOEP_BACK_FOLDER;
            GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
            req->uuid_len = 16;

            /* send set folder req */
            bt_ftp_send_msg(MSG_ID_BT_FTPC_SET_FOLDER_REQ, req ,sizeof(goep_set_folder_req_struct));
            break;
        }

        default:    /* Incorrect state */
            /* Incorrect state, callback and return error */
            GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));
            return FALSE;
    }
    return TRUE;
}

/**
* @brief FTP Client disconnect with remoet device
* FTP Client disconnect
*/
BT_BOOL btmtk_ftpc_disconnect(){
    if( NULL == act_client_cntx_p ){
        //TODO fault error
        return FALSE;
    }
    
    // if current is connected, disconnect it
    switch (act_client_cntx_p->ftpc_state)
    {
        case BT_FTPC_STATE_IDLE:
        case BT_FTPC_STATE_DISCONNECTING:
            GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));
            return FALSE;
            break;
        default:
            act_client_cntx_p->discon_from_cm = TRUE;
            BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_DISCONNECTING);
            bt_ftpc_send_disconnect_message();
            return TRUE;
            break;
    }
    return FALSE;
}


/**
* @brief Push a file to remote ftp server
* @param ucFilePath the send file's path and filename that will be using by btmtk_goep_open_rstream
* @see btmtk_goep_open_rstream
*/
BT_BOOL btmtk_ftpc_push_file(const U8 *ucFolderpath, const U8 *ucFilename){

    if (NULL == act_client_cntx_p) {
        //TODO fault error
        return FALSE;
    }

    /// check state and push a push req
    switch (act_client_cntx_p->ftpc_state) {
        case BT_FTPC_STATE_CONNECTED:
            return bt_ftpc_start_push_a_file_routine(ucFolderpath ,ucFilename);
            break;

        default:
            GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));
            break;
    }

    return FALSE;
}

#ifdef __BT_GOEP_V2__
/**
 * @brief Push a file to remote ftp server
 * @param ucFilePath the send file's path and filename that will be using by btmtk_goep_open_rstream
 * @see btmtk_goep_open_rstream
 */
BT_BOOL btmtk_ftpc_push_file_ex(const U8 *ucFolderpath, const U8 *ucFilename, U8 srm_enable) {
	if( NULL == act_client_cntx_p ) {
		return FALSE;
	}

#ifdef __BT_GOEP_V2__
	/// if connection type is 1 (l2cap), turn on srm mode
	if( act_client_cntx_p && (1 == act_client_cntx_p->ftpc_conntype) ){
		srm_enable = TRUE;
	}else{
		srm_enable = FALSE;
	}
	OS_Report("[AUTO] turn srm_enable to %d", srm_enable);
#endif

	/// check state and push a push req
	switch (act_client_cntx_p->ftpc_state)
	{
		case BT_FTPC_STATE_CONNECTED:
		return bt_ftpc_start_pushfile_routine( ucFolderpath ,ucFilename, srm_enable );
		break;
		default:
		break;
	}
	return FALSE;
}

/**
 * @brief pull a file from remote ftp server
 * @param ucFilePath the send file's path and filename that will be using by btmtk_goep_open_rstream
 * @see btmtk_goep_open_rstream
 */
BT_BOOL btmtk_ftpc_pull_file_ex(const U8 *local_folderpath, const U8 *ucName, const U8 *ucNewName, U8 srm_enable, U8 waitCount) {
	bt_ftpc_pullfile_req_struct *req;
	if( NULL == act_client_cntx_p ) {
		return FALSE;
	}
	if( NULL == ucName && NULL == ucNewName ){
		return FALSE;
	}
	if( NULL == ucName || NULL == ucNewName ){
		/* one of them is NULL. Set two are the same */
		ucName = (ucNewName!=NULL?ucNewName:ucName);
		ucNewName = ucName;
	}

#ifdef __BT_GOEP_V2__
	/// if connection type is 1 (l2cap), turn on srm mode
	if( act_client_cntx_p && (1 == act_client_cntx_p->ftpc_conntype) ){
		srm_enable = TRUE;
	}else{
		srm_enable = FALSE;
	}
	OS_Report("[AUTO] turn srm_enable to %d", srm_enable);
#endif

	/// check state and push a push req
	switch (act_client_cntx_p->ftpc_state)
	{
		case BT_FTPC_STATE_CONNECTED:
		/* copy ucName to ftpc_obj_name */
		memset( act_client_cntx_p->ftpc_filepath, 0, sizeof(act_client_cntx_p->ftpc_filepath));
		bt_ftp_compose_path(local_folderpath, ucNewName, (U8 *) act_client_cntx_p->ftpc_filepath, sizeof(act_client_cntx_p->ftpc_filepath)/2);
		ext_ucs2ncpy( (S8 *) act_client_cntx_p->ftpc_obj_name, (const S8 *) ucName, sizeof(act_client_cntx_p->ftpc_obj_name) );

		/* send get obj req */
		req = (bt_ftpc_pullfile_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(bt_ftpc_pullfile_req_struct),TD_CTRL);
		GOEP_MEMSET( req, 0, sizeof(bt_ftpc_pullfile_req_struct));
		req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];

		ext_ucs2ncpy((S8 *) req->obj_name, (S8 *) act_client_cntx_p->ftpc_obj_name, sizeof(req->obj_name)/2 );
		bt_ftp_ucs2_htons( (U8 *)req->obj_name, (U8 *)req->obj_name );

		ext_ucs2ncpy((S8 *) req->filepath, (S8 *)act_client_cntx_p->ftpc_filepath, sizeof(act_client_cntx_p->ftpc_filepath)/2);
		req->filepath_len = ext_ucs2strlen( (const S8 *)req->filepath)*2;
		req->srm_enable = srm_enable;
		req->srm_waitcount = waitCount;

		/* switch state */
		BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_GETTING_OBJ);
		bt_ftp_send_msg(MSG_ID_BT_FTPC_PULLFILE_REQ, req,sizeof(bt_ftpc_pullfile_req_struct));
		return TRUE;
		break;
		default:
		break;
	}
	return FALSE;
}
#endif

/**
* @brief Request to make a subfolder in FTP server's current folder
* FTP Client Delete Folder
* @return TRUE send request successful. Wait for   evebt
*/
BT_BOOL btmtk_ftpc_make_folder(const U8 *foldername){
    if( NULL == act_client_cntx_p ){
        //TODO fault error
        return FALSE;
    }

    /// check state and send a set path with creating new folder
    switch (act_client_cntx_p->ftpc_state)
    {
        case BT_FTPC_STATE_CONNECTED:
            return bt_ftpc_create_folder_routine( (const U16*) foldername );
            break;
        default:
            GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));
            break;
    }
    return FALSE;
}


/**
* @brief Request to delete a subfolder in FTP server's current folder
* FTP Client Delete Folder
* @return TRUE send request successful. Wait for BT_FTPCUI_FOLDER_DELETED evebt
*/
BT_BOOL btmtk_ftpc_delete_folder(const U8 *foldername){

    if( NULL == act_client_cntx_p ){
        //TODO fault error
        return FALSE;
    }

    if( 0 == ext_ucs2strlen( (const S8 *) foldername) ){
    	return FALSE;
    }

    /// check state and push a push delete
    switch (act_client_cntx_p->ftpc_state)
    {
        case BT_FTPC_STATE_CONNECTED:
            return bt_ftpc_delete_folder_routine( (const U16 *) foldername );
            break;
        default:
            GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));
            break;
    }
    return FALSE;
    
}


/**
* @brief Request to delete a file in FTP server's current folder
* FTP Client Delete File
* @return TRUE send request successful. Wait for BT_FTPCUI_FILE_DELETED evebt
*/
BT_BOOL btmtk_ftpc_delete_file(const U8 *foldername) {

    if (NULL == act_client_cntx_p) {
        return FALSE;
    }

    /// copy it to folder_obj
    if (BT_FTPC_STATE_CONNECTED== act_client_cntx_p->ftpc_state) {
	return btmtk_ftpc_delete_folder(foldername);    
    }
    return FALSE;
}


BT_BOOL btmtk_ftpc_abort() {
    if (NULL == act_client_cntx_p) {
	return FALSE;
    }

    /// check previous state
    if (TRUE == act_client_cntx_p->flag_abort_req_sent) {
	return TRUE;
    }

    /// abort the current sending or receiving
    switch (act_client_cntx_p->ftpc_state) {
	case BT_FTPC_STATE_GETTING_OBJ:
	case BT_FTPC_STATE_GETTING_FOLDER:
	case BT_FTPC_STATE_PUSHING_OBJ:
	    act_client_cntx_p->flag_abort_req_sent = TRUE;
	    return TRUE;

	default:
	    GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));
	    return FALSE;
    }
}


/**
* @param ucfolder
*/
BT_BOOL btmtk_ftpc_get_folder_content(const U8 *ucFolderpath, const U8 *ucFilename) {

    char debug[256];

    if (NULL == act_client_cntx_p) {
	return FALSE;
    }

    // copy it to folder_obj
    if (BT_FTPC_STATE_CONNECTED == act_client_cntx_p->ftpc_state) {
	// check folder path
	if ((U32) (ext_ucs2strlen((S8 *) ucFolderpath) + ext_ucs2strlen((S8 *)ucFilename))+1 > (sizeof(act_client_cntx_p->folder_obj)/2)) {
	    //GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_TARGET_FOLDERPATH_NO_RESOURCE) );
	    // not be able to save in right place
	    GOEP_Report("[FTPC] get_folder_content no resource for filepath");
	    return FALSE;
	}
		
	if (NULL != ucFolderpath && NULL != ucFilename) {
	    GOEP_TRACE_PATH(BT_FTP_TRC_CLASS, BT_FTPC_TARGET_FOLDERPATH, ucFolderpath);
	    GOEP_TRACE_PATH(BT_FTP_TRC_CLASS, BT_FTPC_TARGET_FILENAME, ucFilename);

	    btmtk_goep_compose_filepath((const U8 *) ucFolderpath,
		    (const U8 *) ucFilename,
		    act_client_cntx_p->folder_obj,
		    sizeof(act_client_cntx_p->folder_obj)/2);

	    GOEP_TRACE_PATH(BT_FTP_TRC_CLASS, BT_FTPC_TARGET_FILEPATH, act_client_cntx_p->folder_obj);

	} else if (NULL == ucFolderpath && NULL == ucFilename) {
	    // Use default folder object path.
	    ext_ucs2_str_n_to_asc_str(debug,
		    (S8 *) act_client_cntx_p->folder_obj,
		    sizeof(act_client_cntx_p->folder_obj)/2);
	} else {
	    btmtk_goep_compose_filepath((const U8 *) ucFolderpath,
		    (const U8 *) ucFilename,
		    act_client_cntx_p->folder_obj,
		    sizeof(act_client_cntx_p->folder_obj)/2);

	    GOEP_TRACE_PATH(BT_FTP_TRC_CLASS, BT_FTPC_TARGET_FILEPATH, act_client_cntx_p->folder_obj);
	}

	return bt_ftpc_start_get_folder_content_routine(0);

    } else {
	GOEP_TRACE((BT_FTP_TRC_CLASS,
		BT_FTPC_FILE_STATE_CHECK,
		(act_client_cntx_p)->ftpc_fs_status,
		(act_client_cntx_p)->ftpc_total_len,
		(act_client_cntx_p)->ftpc_push_remain,
		(act_client_cntx_p)->fh_ftpc_push));
    }

    return FALSE;
}

#ifdef __BT_GOEP_V2__
/**
 * @param ucfolder
 */
BT_BOOL btmtk_ftpc_get_folder_content_ex(const U8 *ucFolderpath, const U8 *ucFilename) {
	bt_ftpc_pullfile_req_struct * req;

	if( NULL == act_client_cntx_p ) {
		return FALSE;
	}

	// copy it to folder_obj
	if ( BT_FTPC_STATE_CONNECTED== act_client_cntx_p->ftpc_state )
	{
		if( NULL != ucFolderpath && NULL != ucFilename ) {
			btmtk_goep_compose_filepath( (const U8 *) ucFolderpath, (const U8 *) ucFilename, act_client_cntx_p->folder_obj , sizeof(act_client_cntx_p->folder_obj)/2 );
		}

		BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_GETTING_FOLDER);
		/* send get current folder req */
		/* construct get folder req ilm */
		req = (bt_ftpc_pullfile_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(bt_ftpc_pullfile_req_struct),TD_CTRL);
		GOEP_MEMSET( req, 0, sizeof(bt_ftpc_pullfile_req_struct));
		req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
		ext_strncpy((S8 *) req->obj_mime_type, "x-obex/folder-listing", GOEP_MAX_MIME_TYPE);

		GOEP_MEMCPY( req->filepath, act_client_cntx_p->folder_obj, ext_ucs2strlen( (const S8 *)act_client_cntx_p->folder_obj) *2);
		req->filepath_len = ext_ucs2strlen( (const S8 *)act_client_cntx_p->folder_obj) *2;

		/* send get folder req */
		bt_ftp_send_msg(MSG_ID_BT_FTPC_PULLFILE_REQ, req ,sizeof(bt_ftpc_pullfile_req_struct));
		
		return TRUE;
	}
	return FALSE;
}
#endif

BT_BOOL btmtk_ftpc_authentication_response(const U8 *ucUserid, const U8 *ucPwd, U32 u4PwdLen)
{
	BT_BOOL l_ret;
	l_ret = FALSE;
	if( NULL == act_client_cntx_p ){
		return FALSE;
	}

	/// compose a auth_rsp and send it back
	if (act_client_cntx_p->ftpc_state == BT_FTPC_STATE_IDLE || act_client_cntx_p->ftpc_state == BT_FTPC_STATE_CONNECTING)
	{
		bt_ftpc_send_auth_rsp_msg(ftpc_cntx_p->goep_conn_id[0]	, ucUserid, ucPwd, u4PwdLen);
		BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_CONNECTING);
		return TRUE;
	}

	return l_ret;
}

/**
* @brief FTP Client ui callback notify
* @see BT_FTPCUI_EVNT_T
*/
void btmtk_ftpc_ui_callback(U32 u4Event, U8 *ucData){
    if( NULL != g_ftpc_ui_callback && ftpc_cntx_p->enable ){
    	// GOEP_Report("[FTPC] ui_callback u4Event:(%d) ucdata:(%d", u4Event, ucData);
        g_ftpc_ui_callback(u4Event, ucData);
    }
}

/**
* @brief Setup the callback for notifying UI
*/
BT_BOOL btmtk_ftpc_set_callback(BTMTK_FTPC_CALLBACK callback){
    g_ftpc_ui_callback = callback;
    return TRUE;
}

BT_BOOL btmtk_ftpc_get_status(bt_ftpc_status_struct *status){
    if( NULL == status || status->u4Size != sizeof(bt_ftpc_status_struct) ){
        return FALSE;
    }
    GOEP_MEMSET( (U8 *)status, 0, sizeof(bt_ftpc_status_struct) );
    status->u4Size = sizeof(bt_ftpc_status_struct);
    if( act_client_cntx_p ){
        status->ftpc_state = act_client_cntx_p->ftpc_state;
		GOEP_MEMCPY((U8 *)&status->bdaddr,
				(U8 *)&act_client_cntx_p->ftpc_bt_device,
				sizeof(act_client_cntx_p->ftpc_bt_device));
        status->u4TotalSize = act_client_cntx_p->ftpc_total_len;  // it could be zero and size is unknown
        status->u4DataSize = act_client_cntx_p->ftpc_data_len; // since remote could send unknow-size file
		GOEP_MEMCPY((U8 *)status->ftpc_filepath,
				(U8 *)act_client_cntx_p->push_local_path,
				sizeof(status->ftpc_filepath)-2);
		GOEP_MEMCPY((U8 *)status->ftpc_obj_name,
				(U8 *)act_client_cntx_p->ftpc_obj_name,
				sizeof(status->ftpc_obj_name) -2);
        status->challeng_options = act_client_cntx_p->challeng_options;
        GOEP_MEMCPY( (U8*)status->realm,  (const U8 *)act_client_cntx_p->realm, sizeof(status->realm) );
    }    
    
    return TRUE;
}

#ifdef __BT_GOEP_V2__
BT_BOOL btmtk_ftpc_abortfile_req(U8 reason) {
	// check the status, update the subpath, and send set_folder_rsp back
	FTPC_CS_ENTER();
	if(NULL == act_client_cntx_p) {
		FTPS_CS_LEAVE();
		return FALSE;
	}

	/* Send Disconnect REQ to goep */
	bt_ftpc_send_abortfile_req_msg(ftpc_cntx_p->goep_conn_id[0], reason);
	(act_client_cntx_p)->discon_from_cm = TRUE;
	FTPC_CS_LEAVE();

	return TRUE;
}
#endif

#endif
