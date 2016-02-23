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
 * bt_ftps_hdl.c
 *
 * Project:
 * --------
 *   Handle msg of FTP server
 *
 * Description:
 * ------------
 *   This file is for FTP Server.
 *
 * Author:
 * -------
 *  Daylong Chen
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
#include "bt_goep_fs.h"
#include "bluetooth_ftps_struct.h"
#include "bluetooth_ftpc_struct.h"
#include "bt_ftps_struct.h"
#include "bt_ftps_event.h"
#include "bt_ftps_api.h"
#include "bt_ftps_hdl.h"
#include "bt_ftps_genxml.h"

#include "ext_ucs2.h"
#include "bt_ftp_porting.h"
#include "bt_ext_debug.h"

#define GOEP_OPER_PUSH              3
#define GOEP_OPER_PULL              5


#define GOEP_Report bt_ext_log

#define FTPS_FOLDERLIST_FILEPATH_WCS L"/mnt/sdcard/list.xml"

extern bt_ftps_cntx_struct *const ftps_cntx_p;
extern bt_ftps_conn_cntx_struct *act_server_cntx_p;
void bt_ftps_set_parent_folder(U32 i);


/***************************************************************************** 
* External Variable & Functions
*****************************************************************************/



/***************************************************************************** 
* Local Variable
*****************************************************************************/


/***************************************************************************** 
* MACROS
*****************************************************************************/
/* MMI usb context bit-wise flag operation */

#ifdef __BT_FTS_PROFILE__
#define BT_FTPS_GET_FLAG(a)       ( ((ftps_cntx_p->ftps_flag) & a) == 0 ? FALSE : TRUE )
#define BT_FTPS_SET_FLAG(a)       ( (ftps_cntx_p->ftps_flag) |=  a )
#define BT_FTPS_RSPET_FLAG(a)     ( (ftps_cntx_p->ftps_flag) &=  ~a )
#define FTPS_MAKE_RECV_PATH(x)     ext_ucs2ncpy(x,  L"Received\\");
#endif /* __BT_FTS_PROFILE__ */ 


/***************************************************************************** 
* INTERNAL STATIC FUNCTION
*****************************************************************************/
#if defined(__BT_FTS_PROFILE__) 
static void bt_ftp_connection_terminated(U32 index, U8 role);
static void bt_ftp_obj_name_to_host_order(U8  *h_order_name, U8 *n_order_name);
static int bt_ftp_check_folder_existed(U8* absolute_path);
void bt_ftps_send_auth_req_msg(U8 goep_conn_id,const U8 *realm,const U8 *ucPwd, U32 u4PwdLen);

#define FTP_GLOBAL_API
U32 bt_ftps_find_goep_id(U8 index);
U32 bt_ftpc_find_goep_id(U8 index);
#define MYBT_TRACE     
///#define BT_FTP_TRC_CLASS 6
#endif /* __BT_FTS_PROFILE__ */ 

static const unsigned char OBEX_FTP_UUID[] = { 0xF9, 0xEC, 0x7B, 0xC4, 0x95, 0x3C, 0x11, 0xD2,
                                   0x98, 0x4E, 0x52, 0x54, 0x00, 0xDC, 0x9E, 0x09 };


void ftp_trace_filepath(U8 *prefix, U8 *msg){
    U8 uFilepath[512];
    if( prefix == 0 ){
        prefix = (U8 *) "";
    }
    if(msg == 0){
        return;
    }
    ext_chset_ucs2_to_utf8_string(uFilepath, sizeof(uFilepath), msg);
    bt_ext_log("[GOEP][FILEPATH] %s filepath:'%s'", prefix, uFilepath);
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftp_work_folder_init
 * DESCRIPTION
 *  This function is to initialize FTP work folder Z:\@ftp\
 *  When the folder is not existed, it will try to create, if create failed, it will set flag.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
BT_BOOL bt_ftp_work_folder_init(void)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	U8 ucRootFolder[128];

	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_WORK_FOLDER_INIT) );

	/// get the root folder and create it
	if (FALSE == btmtk_get_default_root_folder_path(ucRootFolder, 128)) {
		return FALSE;
	}

	if (TRUE == btmtk_goep_is_folder_exist((const U16 *) ucRootFolder)) {
		return TRUE;
	}

	if (EXT_FS_OK == btmtk_goep_create_folder((U16 *) ucRootFolder)) {
		return TRUE;
	}
	return FALSE;

}


/*****************************************************************************
 * FUNCTION
 *  bt_ftps_write_obj_routine
 * DESCRIPTION
 *  This function is to write obj to file from push_ind structure
 * PARAMETERS
 *  ind     [?]         
 *  i       [IN]        
 *  goep_push_ind_struct*, U32(?)
 * RETURNS
 *  int
 *****************************************************************************/
S32 bt_ftps_write_obj_routine(U32 i, goep_push_ind_struct *ind)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U8 *all_path_name_ptr;
	FHANDLE h;
	S32 u4Index, u4BuffSize, len;
	U8 *pBuffer;    

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_WRITE_OBJ, ind->pkt_type));

	// create file to write
	all_path_name_ptr = (U8 *)GET_CTRL_BUFFER(FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
	BT_FTP_ASSERT(all_path_name_ptr != NULL);
	GOEP_MEMSET(all_path_name_ptr, 0, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);

	// form obj name from context
	ext_ucs2ncpy((S8 *) all_path_name_ptr, (S8 *) (act_server_cntx_p + i)->current_folder, FMGR_MAX_PATH_LEN);
	ext_ucs2ncat((S8 *) all_path_name_ptr, (S8 *) (act_server_cntx_p + i)->ftps_obj_name, FMGR_MAX_PATH_LEN);

	// when use the fh, last use may close it failed
	if ((act_server_cntx_p + i)->ftps_fh_check_timer_set) {

		//StopTimer(BT_FTPS_PUSH_FH_CHECK_TIMER);
		if ((act_server_cntx_p + i)->fh_ftps_recv) {

			btmtk_goep_close_wstream(act_server_cntx_p->fh_ftps_recv);
			(act_server_cntx_p + i)->fh_ftps_recv = FHANDLE_INVALID_VALUE;

			// btmtk_goep_delete_file((U32 *) (act_server_cntx_p + i)->temp_pushing_file);
			btmtk_goep_delete_file( (U16 *) (act_server_cntx_p + i)->temp_pushing_file);

			GOEP_MEMSET((act_server_cntx_p + i)->temp_pushing_file, 0, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
			(act_server_cntx_p + i)->ftps_fh_check_timer_set = FALSE;
		}
	}

	h = btmtk_goep_open_wstream((U16*) all_path_name_ptr, 
			EXT_FS_READ_WRITE | EXT_FS_CREATE_ALWAYS);

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_WRITE_OBJ_OPEN, h));
	if (FALSE == ext_fs_is_valid_handle_ucs2(h)) {

		// File open error. Error handling
		//bt_ftps_set_fs_flag((int) h); //TODo

		if (all_path_name_ptr) {
			FREE_CTRL_BUFFER((void *)all_path_name_ptr);
			all_path_name_ptr = NULL;
		}
		return -1;

	} else {
		// set file handle to context
		// bt_ext_log("%s(): opened fd: %d", (act_server_cntx_p + i)->fh_ftps_recv);
		(act_server_cntx_p + i)->fh_ftps_recv = h;
	}

	// Write object context to file
	if (ind->frag_len > 0) {
		U32 len_written;
		S32 ret;

		len = ind->frag_len;
		if (FALSE != ext_nbuffer_isFull(&act_server_cntx_p->ftps_mempool)) {
			// write all data and 
			bt_ftps_flush_all_nbuffer_to_file();
			ret = act_server_cntx_p->ftps_fs_status;

			if (EXT_FS_OK == act_server_cntx_p->ftps_fs_status) {
				//X: FS_Write(handle, data, len, written);
				ret = btmtk_goep_write_wstream(h, ind->frag_ptr, len, (S32 *) &len_written);
			}

		} else {

			if (FALSE != ext_nbuffer_isEmpty(&act_server_cntx_p->ftps_mempool)) {
				bt_ftp_send_msg_toself( MSG_ID_BT_FTPS_SERVER_INTERNAL_RW, NULL, 0);
			}

			// put to end of mempool
			pBuffer = (U8 *) ext_nbuffer_get_tail_buffer( &act_server_cntx_p->ftps_mempool, (U32 *) &u4Index, &u4BuffSize);
			//BT_OPP_ASSERT(NULL != pBuffer);
			//BT_OPP_ASSERT(len > u4BuffSize);

			GOEP_MEMCPY(pBuffer, ind->frag_ptr, min(len, u4BuffSize));
			if (FALSE != ext_nbuffer_return_tail_buffer(&act_server_cntx_p->ftps_mempool, 
					u4Index, min(len, u4BuffSize))) {
				ret = EXT_FS_OK;

			} else {
				ret = EXT_FS_ERR;
			}
		}

		GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_WRITE_OBJ_WRITE, ret));

		// Write error
		if (EXT_FS_OK != ret) {
			S32 err_code;

			if (FALSE != ext_fs_is_valid_handle_ucs2((act_server_cntx_p)->fh_ftps_recv)) {
				err_code = btmtk_goep_close_wstream( (act_server_cntx_p)->fh_ftps_recv );

				if (err_code != EXT_FS_OK) {
					// copy pat filename to temp push file's name ???QQQ
					ext_ucs2ncpy((S8 *) (act_server_cntx_p + i)->temp_pushing_file,
							(S8 *) all_path_name_ptr, 
							FMGR_MAX_PATH_LEN);

					// StartTimer(BT_FTPS_PUSH_FH_CHECK_TIMER,
					//	    FTPS_PUSH_FH_CHECK_DUR,bt_ftps_push_fh_check_timer_hdler);
					// (act_server_cntx_p + i)->ftps_fh_check_timer_set = TRUE;

				} else {
					(act_server_cntx_p + i)->fh_ftps_recv = FHANDLE_INVALID_VALUE;
					btmtk_goep_delete_file((U16 *) all_path_name_ptr);
				}       

				//bt_ftps_set_fs_flag((int) ret);
			}
						
			if (all_path_name_ptr) {
				FREE_LOCAL_PARA((void *)all_path_name_ptr);
				all_path_name_ptr = NULL;
			}

			return -1;
		}

	} else {
		//FTP_DEBUG(kal_print("FTPS WRITE OBJ (LEN <= 0)\n"););
	}

	if (NULL != all_path_name_ptr) {
		FREE_CTRL_BUFFER((void *)all_path_name_ptr);
		all_path_name_ptr = NULL;
	}

	// check final packet -- if final -> notify MMI Scr
	if ((ind->pkt_type == GOEP_FINAL_PKT) || (ind->pkt_type == GOEP_SINGLE_PKT)) {
		//FTP_DEBUG(kal_print("FTPS WRITE OBJ (FINAL_PKT)\n"););
		BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);

		// close file
		if ((act_server_cntx_p + i)->fh_ftps_recv) {
			btmtk_goep_close_wstream(act_server_cntx_p->fh_ftps_recv);
			(act_server_cntx_p + i)->fh_ftps_recv = FHANDLE_INVALID_VALUE;
		}

		btmtk_ftps_ui_callback(BT_FTPSUI_PUSHED, 0);
	}

	return 1;
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftps_push_obj
 * DESCRIPTION
 *  This function is to push a obj to server
 * PARAMETERS
 *  ind     [?]         
 *  i       [IN]        
 *  goep_push_ind_struct*, U32(?)
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftps_push_obj(U8 i, U8 *obj_name, goep_pkt_type_enum pkg_type, U8 *frag_ptr, U32 frag_len, BT_BOOL *bMore)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U8 *path_buf;
	S32 folder_check; 
	S32 ret;
	U32 l_rsp_code;

	U8 debug_path[FMGR_MAX_PATH_LEN];

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE ((BT_FTP_TRC_CLASS, BT_FTPS_PUSH_OBJ));

	// GOEP_ASSERT( NULL != bMore );
	*bMore = FALSE;
	l_rsp_code = GOEP_INTERNAL_SERVER_ERR;

	if (0 == ext_ucs2strlen((const S8 *) (act_server_cntx_p + i)->ftps_filepath)) {
		folder_check = btmtk_goep_is_folder_exist((const U16*)bt_ftps_get_current_folder((U8)i));
		if (TRUE != folder_check) {
			// bt_ftps_set_fs_flag(FS_ERR_FOLDER_ERR);
			l_rsp_code = GOEP_NOT_FOUND;
			goto FTPS_PUSH_OBJ_ERR;
		}    

		// check and set file name
		// reset and save object name to context
		GOEP_MEMSET((act_server_cntx_p + i)->ftps_obj_name, 0, FTP_MAX_OBJ_NAME_LEN);

		if (ext_ucs2strlen((S8*) obj_name) > 0) {
			// bt_ftp_obj_name_to_host_order((U8 *)obj_name, (U8 *)obj_name);

			bt_ftp_ucs2_cut_name_with_ext((U8 *)(act_server_cntx_p + i)->ftps_obj_name,
					FTP_MAX_OBJ_NAME_LEN,
					(const U8 *)obj_name);

		} else {
			// put obj without carring name
			//FTP_DEBUG(kal_print("FTPS PUSH OBJ( OBJ_NAME == 0 )\n"););            
			ext_ucs2ncpy((S8 *)(act_server_cntx_p + i)->ftps_obj_name,
					(const S8 *)L"new.tmp", 16);
		}

		// Set path and file name, check file exist
		path_buf = GET_CTRL_BUFFER(FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
		BT_FTP_ASSERT(path_buf != NULL);
		GOEP_MEMSET(path_buf, 0, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);

		// check path length
		ext_ucs2ncpy((S8 *) path_buf, (const S8 *) bt_ftps_get_current_folder((U8)i), FMGR_MAX_PATH_LEN);

		if (ext_ucs2strlen((S8 *) path_buf) + ext_ucs2strlen((S8 *) (act_server_cntx_p + i)->ftps_obj_name) >
			(FMGR_MAX_PATH_LEN - 1)) {

			//FTP_DEBUG(kal_print("FTPS PUSH OBJ( PATH > FMGR_MAX_PATH_LEN )\n"););            
			if (NULL != path_buf) {
				FREE_CTRL_BUFFER((void *)path_buf);
			}

			//BT_FTPS_SET_FLAG(FS_ERR_WRITE_FILE_FAIL);
			l_rsp_code = GOEP_NOT_ACCEPTABLE;
			goto FTPS_PUSH_OBJ_ERR;
		}

		// form complete path
		bt_ftp_compose_path(bt_ftps_get_current_folder((U8)i),
				(act_server_cntx_p + i)->ftps_obj_name,
				(act_server_cntx_p + i)->ftps_filepath,
				FMGR_MAX_PATH_LEN);
	}

	ext_ucs2_str_n_to_asc_str((S8 *) debug_path, (S8 *) (act_server_cntx_p + i)->ftps_filepath, sizeof(debug_path));
	bt_ext_log("[BT][FTP] In %s(), file path: %s", __FUNCTION__, debug_path);

	ret = btmtk_goep_open_wstream((U16 *) (act_server_cntx_p + i)->ftps_filepath, EXT_FS_CREATE_ALWAYS);

	if (FALSE == ext_fs_is_valid_handle_ucs2(ret)) {
	//    FTP_DEBUG(kal_print("FTPS PUSH OBJ( Invalid file name )\n"););
	//    bt_ftps_set_fs_flag( FS_ERR_WRITE_FILE_FAIL );
		if (FALSE == btmtk_goep_is_file_exist((const U16 *) (act_server_cntx_p + i)->ftps_filepath)) {
			l_rsp_code = GOEP_NOT_FOUND;
		}

		goto FTPS_PUSH_OBJ_ERR;
	}

	(act_server_cntx_p + i)->fh_ftps_recv = ret;

	// write file
	bt_ftps_write_obj_continue_routine((U8)i, pkg_type, frag_ptr, frag_len , bMore);

	return;

FTPS_PUSH_OBJ_ERR:

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_PUSH_OBJ_ERR_GOTO));
	*bMore = FALSE;
	if (GOEP_STATUS_SUCCESS == l_rsp_code) {
		l_rsp_code = GOEP_INTERNAL_SERVER_ERR;
	}

	BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);

	bt_ftps_send_push_rsp_msg( (U8) ftps_cntx_p->goep_conn_id[i], (U8) l_rsp_code);
	btmtk_ftps_ui_callback( BT_FTPSUI_PUSHED, (void *)l_rsp_code);
	ftps_cntx_p->ftps_flag = 0;

	return;
}

/**
* @brief push_prepare 
* @param file_path the ucs2 raw OS filepath
**/
void bt_ftps_push_prepare_routine(const U8* file_path)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U8 utf8_path_name[BTMTK_FS_MAX_FILEPATH];

	ext_chset_ucs2_to_utf8_string(utf8_path_name, BTMTK_FS_MAX_FILEPATH, file_path);
	btmtk_ftps_ui_callback(BT_FTPSUI_PUSH_FILE_START, (void*) utf8_path_name);
	act_server_cntx_p->flag_abort_req_sent = FALSE;
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftps_push_delete_routine
 * DESCRIPTION
 *  This function is to delete a obj from server
 * PARAMETERS
 *  ind     [?]         
 *  i       [IN]        
 *  goep_push_ind_struct*, U32(?)
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftps_push_delete_routine(U8 i, goep_push_ind_struct *ind)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	goep_push_rsp_struct *res;
	U8 ret = 0;
	U8 utf8_path_name[BTMTK_FS_MAX_FILEPATH];

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_PUSH_DELETE));

	if (ext_ucs2strlen((S8*) ind->obj_name) > 0 || i != 0) {
		if (ext_ucs2strlen((S8 *) (act_server_cntx_p + i)->current_folder) + 
				ext_ucs2strlen((S8 *)ind->obj_name) > (FTP_MAX_OBJ_NAME_LEN/2 - 1)) {
			GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_PUSH_DELETE_INVALID_NAME));
			ret = GOEP_INTERNAL_SERVER_ERR;
			goto FTPS_PUSH_DELETE_SEND;
		}

		bt_ftp_obj_name_to_host_order( ind->obj_name, ind->obj_name);

	} else {
		GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_PUSH_DELETE_WITHOUT_NAME));
		ret = GOEP_NOT_FOUND;
		goto FTPS_PUSH_DELETE_SEND;
	}

	bt_ftp_compose_path(ftps_cntx_p->root_folder,
			act_server_cntx_p->current_folder,
			act_server_cntx_p->new_folder,
			sizeof(act_server_cntx_p->new_folder)/2);

	bt_ftp_compose_path(act_server_cntx_p->new_folder,
			ind->obj_name,
			act_server_cntx_p->ftps_filepath,
			sizeof(act_server_cntx_p->ftps_filepath)/2);

	ext_ucs2ncpy((S8 *) act_server_cntx_p->new_folder,
			(const S8 *) act_server_cntx_p->current_folder,
			sizeof(act_server_cntx_p->new_folder)/2);

	ext_ucs2ncpy((S8 *) act_server_cntx_p->ftps_obj_name,
			(const S8 *) ind->obj_name,
			sizeof(act_server_cntx_p->ftps_obj_name)/2);

	// kal_prompt_trace(MOD_MMI, "[BT-FTP-S] folder name length = %d\n", 
	//		ext_ucs2strlen((S8 *)(act_server_cntx_p + i)->ftps_obj_name));

	if (FALSE != bt_ftps_is_auto(BT_FTP_AUTO_DELETE)) {
		res = GOEP_STATUS_SUCCESS;

		if (FALSE != btmtk_goep_is_folder_exist((const U16*)(act_server_cntx_p + i)->ftps_filepath)) {
			bt_ext_log("%s(): delete folder", __FUNCTION__);

			if (btmtk_ftp_delete_folder((const U16*) (act_server_cntx_p + i)->ftps_filepath)) {
				//0xcc; // FTP spec. "Precondition fail", PTS TC_SERVER_OMA_BV_08_I
				ret = GOEP_PRECONDITION_FAILED;
			}
			goto FTPS_PUSH_DELETE_SEND;

		} else if (FALSE != btmtk_goep_is_file_exist((const U16*) (act_server_cntx_p + i)->ftps_filepath)) {
			bt_ext_log("%s(): delete file", __FUNCTION__);

			if (btmtk_ftp_delete_file((U16 *)(act_server_cntx_p + i)->ftps_filepath)) {
				//0xcc; // FTP spec. "Precondition fail", PTS TC_SERVER_OMA_BV_08_I
				ret = GOEP_PRECONDITION_FAILED;
			} else {
				// For android, use BT_FTPSUI_FILE_DELETE to indicate that the file is deleted.
				ext_chset_ucs2_to_utf8_string(utf8_path_name,
						BTMTK_FS_MAX_FILEPATH,
						(act_server_cntx_p + i)->ftps_filepath);
				btmtk_ftps_ui_callback(BT_FTPSUI_FILE_DELETE, (void*) utf8_path_name);
			}
			goto FTPS_PUSH_DELETE_SEND;

		} else {
			// it is not a file or folder
			ret = GOEP_NOT_FOUND;
			goto FTPS_PUSH_DELETE_SEND;
		}

	} else {
		if (FALSE != btmtk_goep_is_folder_exist((const U16*) (act_server_cntx_p + i)->ftps_filepath)) {
			btmtk_ftps_ui_callback(BT_FTPSUI_FOLDER_DELETE, 0);
		} else {
			btmtk_ftps_ui_callback(BT_FTPSUI_FILE_DELETE, 0);
		}

		ret = GOEP_STATUS_SUCCESS;
	}

	return;

FTPS_PUSH_DELETE_SEND:
	res = (goep_push_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_push_rsp_struct), TD_CTRL);
	res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];
	res->rsp_code = ret;
	bt_ftp_send_msg(MSG_ID_BT_FTPS_PUSH_RSP, res, sizeof(goep_push_rsp_struct));
	BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);

	return;
}


/**
* @brief push_delete handle ok and send the push_delete rsp to client
* @see btmtk_ftps_response_push_delete_file
**/
void bt_ftps_push_delete_routine_success_msg(U32 result)
{
	//TODO Check the delete
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	goep_push_rsp_struct *res;
	U8 i = 0;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_PUSH_DEL_CBK, (act_server_cntx_p + i)->ftps_state, result) );

	if ((act_server_cntx_p + i)->ftps_state != BT_FTPS_STATE_RECEIVING)
	{
		return;
	}

	res = (goep_push_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_push_rsp_struct),TD_CTRL);

	GOEP_MEMSET( res, 0, sizeof(goep_push_rsp_struct));
	res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];

	// if (result >= EXT_FS_OK)
	if (1)
	{
		res->rsp_code = GOEP_STATUS_SUCCESS;  
		bt_ftp_send_msg(MSG_ID_BT_FTPS_PUSH_RSP, res , sizeof(goep_push_rsp_struct));
		BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
	}
	else
	{
		res->rsp_code = GOEP_INTERNAL_SERVER_ERR;  
		bt_ftp_send_msg(MSG_ID_BT_FTPS_PUSH_RSP, res , sizeof(goep_push_rsp_struct) );
		BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
	}

	return;
}


/**
* @brief This function is to write the data to file, and send res.
 **/
void bt_ftps_write_obj_continue_routine(U8 i, goep_pkt_type_enum pkt_type, U8 *frag_ptr, U32 frag_len,BT_BOOL *bMore)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_PUSH_CONTINUE, pkt_type));
	//  ASSERT(NULL != bMore);
	*bMore = TRUE;

	// Write object context to file
	if (frag_len > 0) {
		U32 len_written;
		S32 ret;

		ret = btmtk_goep_write_wstream((act_server_cntx_p + i)->fh_ftps_recv,
				frag_ptr, frag_len, (S32*) & len_written);
		// bt_ext_log("[ftps] %s(), writren: %d", __FUNCTION__, len_written);

		GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_PUSH_CONTINUE_WRITE, ret));

		/* Write error */
		if (ret < 0) {
			S32 err_code;
			err_code = btmtk_goep_close_wstream(act_server_cntx_p->fh_ftps_recv);

			if (err_code < EXT_FS_OK && err_code != FHANDLE_INVALID_VALUE) {
				ext_ucs2ncpy((S8 *) (act_server_cntx_p + i)->temp_pushing_file,
						(S8 *) bt_ftps_get_current_folder((U8)i),
						FMGR_MAX_PATH_LEN);

				ext_ucs2ncat((S8 *) (act_server_cntx_p + i)->temp_pushing_file,
						(S8 *) (act_server_cntx_p + i)->ftps_obj_name,
						FMGR_MAX_PATH_LEN);

				// StartTimer(BT_FTPS_PUSH_FH_CHECK_TIMER,FTPS_PUSH_FH_CHECK_DUR,bt_ftps_push_fh_check_timer_hdler);
				// (act_server_cntx_p + i)->ftps_fh_check_timer_set = TRUE;

			} else {
				(act_server_cntx_p + i)->fh_ftps_recv = FHANDLE_INVALID_VALUE;
				bt_ftps_delete_pushing_file(0);
			}
			
			//bt_ftps_set_fs_flag(ret);
			BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
			bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[i], GOEP_INTERNAL_SERVER_ERR);
			ftps_cntx_p->ftps_flag = 0;
			return;
		}
	}

	if (frag_len <= act_server_cntx_p->remain_send_obj_len) {
		act_server_cntx_p->remain_send_obj_len -= frag_len;

	} else {
		act_server_cntx_p->remain_send_obj_len = 0;
	}

	// check final packet --
	if((pkt_type == GOEP_FINAL_PKT) || (pkt_type == GOEP_SINGLE_PKT)) {
		BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
		*bMore = FALSE;
		// close file
		if ((act_server_cntx_p + i)->fh_ftps_recv) {
			bt_ftps_flush_all_nbuffer_to_file();
			btmtk_goep_close_wstream(act_server_cntx_p->fh_ftps_recv);
			(act_server_cntx_p + i)->fh_ftps_recv = FHANDLE_INVALID_VALUE;
		}

		btmtk_ftps_ui_callback(BT_FTPSUI_PUSHED, (void *) 0);

	} else {
		btmtk_ftps_ui_callback(BT_FTPSUI_PUSHING, 
				(void *) (act_server_cntx_p->total_send_obj_len - act_server_cntx_p->remain_send_obj_len));
	}

	bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[i], GOEP_STATUS_SUCCESS);

	return;
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftps_create_folder
 * DESCRIPTION
 *  This function is to create a new folder and set as the current folder
 * PARAMETERS
 *  ind     [?]         
 *  i       [IN]        
 *  goep_set_folder_ind_struct*, U32(?)
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftps_create_folder(U8 i,goep_set_folder_ind_struct *ind)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	S32 ret;
	goep_set_folder_rsp_struct *res;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_CREATE_FOLDER, ftps_cntx_p->goep_conn_id[i]));
	res = (goep_set_folder_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_set_folder_rsp_struct),TD_CTRL);

	GOEP_MEMSET( res, 0, sizeof(goep_set_folder_rsp_struct));
	res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];

	// Check the access right first. If the access right is read only, return failed directly
	if (ftps_cntx_p->access_right == BT_FTP_ACCESS_RIGHT_READ_ONLY)
	{
		// Wrong Access Right
		res->rsp_code = GOEP_UNAUTHORIZED;

		bt_ftp_send_msg(MSG_ID_BT_FTPS_SET_FOLDER_RSP, res , sizeof(goep_set_folder_rsp_struct));
		return;
	}

	// check the folder name
	if (ext_ucs2strlen((S8*) ind->folder_name) > 0)
	{
		bt_ftp_obj_name_to_host_order((U8 *)ind->folder_name, (U8 *)ind->folder_name);
		bt_ftp_ucs2_cut_name_without_ext((U8 *)ind->folder_name, GOEP_MAX_FOLDER_NAME, (const U8 *)ind->folder_name);
	}
	// maybe folder_name is '   '
	if (ext_ucs2strlen((S8*)ind->folder_name) == 0)
	{
		//FTP_DEBUG(kal_print("FTPS CREATE FOLDER ( NAME == 0 )\n"););
		ext_asc_str_n_to_ucs2_str((S8*)ind->folder_name, (S8*) "new_folder", GOEP_MAX_FOLDER_NAME);
	}

	// form folder path
	if (ext_ucs2strlen((S8 *)((act_server_cntx_p + i)->current_folder)) +
		ext_ucs2strlen((S8 *)ind->folder_name) > (FMGR_MAX_PATH_LEN - 2))
	{
		res->rsp_code = GOEP_INTERNAL_SERVER_ERR;
		bt_ftp_send_msg(MSG_ID_BT_FTPS_SET_FOLDER_RSP, res, sizeof(goep_set_folder_rsp_struct));
		return;     
	}
	ext_ucs2ncpy((S8 *)(act_server_cntx_p + i)->ftps_obj_name, (S8 *) ((act_server_cntx_p + i)->current_folder), FMGR_MAX_PATH_LEN);
	ext_ucs2ncat((S8 *)(act_server_cntx_p + i)->ftps_obj_name, (S8 *)ind->folder_name, FMGR_MAX_PATH_LEN);
	ext_ucs2ncat((S8 *)(act_server_cntx_p + i)->ftps_obj_name, (const S8*)L"\\", FMGR_MAX_PATH_LEN );

	ret = btmtk_goep_create_folder( (const U16*)(act_server_cntx_p + i)->ftps_obj_name);
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_CREATE_FOLDER_CREATE_DIR, ret));

	if (EXT_FS_OK != ret)
	{
		//FTP_DEBUG(kal_print("FTPS CREATE FOLDER ( btmtk_goep_create_folder( ERROR )\n"););        
		res->rsp_code = GOEP_INTERNAL_SERVER_ERR;
		bt_ftp_send_msg(MSG_ID_BT_FTPS_SET_FOLDER_RSP, res , sizeof(goep_set_folder_rsp_struct) );
		return;     
	}
	else
	{
		(act_server_cntx_p+i)->ftps_browse_depth++;
		ext_ucs2ncpy((S8 *) ((act_server_cntx_p + i)->current_folder), (S8 *)(act_server_cntx_p + i)->ftps_obj_name, FMGR_MAX_PATH_LEN);
		res->rsp_code = GOEP_STATUS_SUCCESS;
		bt_ftp_send_msg(MSG_ID_BT_FTPS_SET_FOLDER_RSP, res , sizeof(goep_set_folder_rsp_struct) );
		return;     
	}
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftps_set_folder
 * DESCRIPTION
 *  This function is to set current folder to selected folder
 * PARAMETERS
 *  ind     [?]         
 *  i       [IN]        
 *  goep_set_folder_ind_struct*,U32(?)
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftps_set_folder(U8 i, goep_set_folder_ind_struct *ind)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U8 new_folder[FMGR_MAX_PATH_LEN * ENCODING_LENGTH];
	goep_set_folder_rsp_struct *res;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_SET_FOLDER, ftps_cntx_p->goep_conn_id[i]));
	res = (goep_set_folder_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_set_folder_rsp_struct),TD_CTRL);

	GOEP_MEMSET(res, 0, sizeof(goep_set_folder_rsp_struct));
	res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];

	GOEP_MEMSET(new_folder, 0, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);

	bt_ftp_obj_name_to_host_order(ind->folder_name, ind->folder_name);

	// not parent or current folder
	if ((ext_ucs2cmp((U8 *)ind->folder_name, (U8 *) L".") != 0) &&
		(ext_ucs2cmp((U8 *)ind->folder_name, (U8 *) L"..") != 0))
	{
		ext_ucs2ncpy((S8 *) new_folder, (S8 *) ((act_server_cntx_p + i)->current_folder), FMGR_MAX_PATH_LEN);
		// fail-safe for enough memory to store the folder_name + folder_path (2 = '\0','\')
		if (ext_ucs2strlen((S8 *) new_folder) + ext_ucs2strlen((S8 *)ind->folder_name) > (FMGR_MAX_PATH_LEN - 2))
		{
			//FTP_DEBUG(kal_print("FTPS SET FOLDER CARRING INVALID FOLDER NAME\n"););
			// send res with error rsp_code not found
			res->rsp_code = GOEP_NOT_FOUND;
			goto FTPS_SET_FOLDER_SEND;
		}

		// form folder path
		ext_ucs2ncat((S8 *) new_folder, (S8 *)ind->folder_name, FMGR_MAX_PATH_LEN);
		ext_ucs2ncat((S8 *) new_folder, (const S8 *)L"\\", FMGR_MAX_PATH_LEN);
	}

	// go to parent folder
	else if (ext_ucs2cmp((U8 *)ind->folder_name, (U8 *) L"..") == 0)
	{
		// call set parent folder routine
		BT_FTP_ASSERT(res);
		// OslFreeDataPtr(res);
		FREE_LOCAL_PARA((void *)res);
		bt_ftps_set_parent_folder(i);
		return;
	}
	// current folder
	else
	{
		// Send success response to client
		res->rsp_code = GOEP_STATUS_SUCCESS;
		goto FTPS_SET_FOLDER_SEND;
	}

FTPS_SET_FOLDER_SEND:
	// Send MSG_ID_BT_FTPS_SET_FOLDER_RSP primitive
	bt_ftp_send_msg(MSG_ID_BT_FTPS_SET_FOLDER_RSP, res , sizeof(goep_set_folder_rsp_struct));
	return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftps_set_parent_folder
 * DESCRIPTION
 *  This function is to set current folder to parent folder
 * PARAMETERS
 *  i       [IN]        
 *  U32(?)
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftps_set_parent_folder(U32 i)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	goep_set_folder_rsp_struct *res;
	S32 NameLength;
	U16 *UniPathName = 0; 
	U16 deter = '\\';

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_SET_PARENT_FOLDER, ftps_cntx_p->goep_conn_id[i]));

	res = (goep_set_folder_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_set_folder_rsp_struct),TD_CTRL);

	GOEP_MEMSET( res, 0, sizeof(goep_set_folder_rsp_struct));
	res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];

	NameLength = ext_ucs2strlen((S8 *) ((act_server_cntx_p + i)->current_folder));
	UniPathName = (U16 *)(GET_CTRL_BUFFER((NameLength + 1) * ENCODING_LENGTH));
	ext_ucs2ncpy((S8 *)UniPathName, (S8 *) ((act_server_cntx_p + i)->current_folder), FMGR_MAX_PATH_LEN);


	// UniPathName+i points to the (i+1)th ascii code in path --> minus one first
	NameLength--;
	while (NameLength > 0)
	{
		// search from tail by cutting the last ascii code step by step
		NameLength--;
		if (GOEP_MEMCMP((UniPathName + NameLength), 2, &deter, 2) == 0 ||
			GOEP_MEMCMP((UniPathName + NameLength), 2, (void *)FTP_FS_PATH_DELIMITOR, 2) == 0)
		{
			// Cut the current folder path to become parent folder path
			GOEP_MEMSET((UniPathName + NameLength + 1), 0, 2);
			break;
		}
	}

	ext_ucs2ncpy((S8 *)((act_server_cntx_p + i)->current_folder), (S8 *)UniPathName, FMGR_MAX_PATH_LEN);
	(act_server_cntx_p+i)->ftps_browse_depth--;
	res->rsp_code = GOEP_STATUS_SUCCESS;

	if(UniPathName)
	{
		FREE_CTRL_BUFFER(UniPathName);
	}
	bt_ftp_send_msg(MSG_ID_BT_FTPS_SET_FOLDER_RSP, res , sizeof(goep_set_folder_rsp_struct) );
	return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftps_pull_continue_routine
 * DESCRIPTION
 *  This function is to keeping sending the pending object requested from client
 * PARAMETERS
 *  i       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftps_pull_continue_routine(U8 i, BT_BOOL *more)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	S32 ret;
	U32 len_read;
	goep_pull_rsp_struct *res;
	static BT_BOOL bRewind = TRUE;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	*more = FALSE;
	// GOEP_TRACE(BT_FTP_TRC_CLASS, BT_FTPS_PULL_CONTINUE, ftps_cntx_p->goep_conn_id[i]);
	if (FALSE == ext_fs_is_valid_handle_ucs2((act_server_cntx_p + i)->fh_ftps_send)) {
		// open the xml file base on ftps_path
		(act_server_cntx_p + i)->fh_ftps_send =
		btmtk_goep_open_rstream((U16 *)(act_server_cntx_p + i)->ftps_filepath, 0);
		ext_nbuffer_cleanup(&(act_server_cntx_p+i)->ftps_mempool);
		(act_server_cntx_p+i)->ftps_fs_status = EXT_FS_OK;
	}

	if (FALSE == ext_fs_is_valid_handle_ucs2((act_server_cntx_p + i)->fh_ftps_send)) {
		goto FTPS_PULL_CONTINUE_ERROR;
	}

	if (0 == (act_server_cntx_p + i)->send_obex_pkt_size || 
		NULL == (act_server_cntx_p + i)->ftps_obj_buff) {
		goto FTPS_PULL_CONTINUE_ERROR;
	}

	ret = (act_server_cntx_p + i)->ftps_fs_status;

	if (EXT_FS_OK <= ret || EXT_FS_ERR_EOF == ret) {
		if (FALSE != ext_nbuffer_isEmpty( &(act_server_cntx_p + i)->ftps_mempool )) {
			ret = btmtk_goep_read_rstream(
					(act_server_cntx_p + i)->fh_ftps_send, // open file's handle
					(act_server_cntx_p + i)->ftps_obj_buff, // pull buffer
					(act_server_cntx_p + i)->send_obex_pkt_size, // remote max packet size. 2048 test pass
					(S32*) &len_read);
		// bt_ext_log("[ftps] %s(), len_read: %d", __FUNCTION__, len_read);
			bt_ftp_send_msg_toself(MSG_ID_BT_FTPS_SERVER_INTERNAL_RW, NULL, 0);

		} else {
			if (FALSE == ext_nbuffer_pop_data(
				&(act_server_cntx_p + i)->ftps_mempool,
				(void *)(act_server_cntx_p+i)->ftps_obj_buff,
				FTPS_MEM_BUFF_SIZE, (S32 *) &len_read)) {
				  //GOEP_Report("[ftps][err] pop data return FALSE");
			}
		}
	}

	// FS error
	if (ret < EXT_FS_OK && ret != EXT_FS_ERR_EOF) {
		//FTP_DEBUG(kal_print("FTPS SEND CONTINUE FS_Read error\n");)
		BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);

		btmtk_goep_close_rstream(act_server_cntx_p->fh_ftps_send);
		(act_server_cntx_p + i)->total_send_obj_len = 0;
		(act_server_cntx_p + i)->remain_send_obj_len = 0;
		(act_server_cntx_p + i)->fh_ftps_send = FHANDLE_INVALID_VALUE;

		if ((act_server_cntx_p + i)->send_obj_type == BT_FTPS_OBJ_TYPE_FOLDER) {
			//FTP_DEBUG(kal_print("FTPS SEND CONTINUE ERR (FOLDER)\n");)
			// delete the folder content file
			//btmtk_goep_delete_file((const U32*)((act_server_cntx_p + i)->folder_content));
		}
		(act_server_cntx_p + i)->send_obj_type = BT_FTPS_OBJ_TYPE_NONE;

		// Construct goep pull res ilm
		goto FTPS_PULL_CONTINUE_ERROR;
		return;
	}

	// Finished reading
	if (len_read == (act_server_cntx_p + i)->remain_send_obj_len) {
		//FTP_DEBUG(kal_print("FTPS SEND CONTINUE (LAST PACKET)\n");)
		BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);

		btmtk_goep_close_rstream(act_server_cntx_p->fh_ftps_send);
		(act_server_cntx_p + i)->total_send_obj_len = 0;
		(act_server_cntx_p + i)->remain_send_obj_len = 0;
		(act_server_cntx_p + i)->fh_ftps_send = FHANDLE_INVALID_VALUE;

		if ((act_server_cntx_p + i)->send_obj_type == BT_FTPS_OBJ_TYPE_FOLDER) {
			//FTP_DEBUG(kal_print("FTPS SEND CONTINUE FINAL FOLDER PKT\n");)
			// delete the folder content file
			//btmtk_goep_delete_file((const U32*)((act_server_cntx_p + i)->folder_content));
		}

		(act_server_cntx_p + i)->send_obj_type = BT_FTPS_OBJ_TYPE_NONE;

		// film res pkt_type attibute
		res = (goep_pull_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_pull_rsp_struct),TD_CTRL);

		// memory init
		GOEP_MEMSET( res, 0, sizeof(goep_pull_rsp_struct));
		res->pkt_type = GOEP_FINAL_PKT;

	} else if (len_read < (act_server_cntx_p + i)->remain_send_obj_len) {
		// continue transferring
		*more = TRUE;

		// FTP_DEBUG(kal_print("FTPS SEND CONTINUE (NORMAL PACKET)\n");)
		(act_server_cntx_p+i)->remain_send_obj_len  -= len_read;      
		// film res pkt_type attibute
		res = (goep_pull_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_pull_rsp_struct),TD_CTRL);

		// memory init
		GOEP_MEMSET( res, 0, sizeof(goep_pull_rsp_struct));
		res->pkt_type = GOEP_NORMAL_PKT;

	} else {
		// abnormal case
		// FTP_DEBUG(kal_print("FTPS SEND CONTINUE (ABNORMAL CASE)\n");)
		BT_FTP_ASSERT(0);
		goto FTPS_PULL_CONTINUE_ERROR;
		return;
	}

	// fill remaining res attibutes
	res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];
	res->frag_len = (U16) len_read;
	res->frag_ptr = (act_server_cntx_p + i)->ftps_obj_buff;
	res->total_obj_len = 0;
	res->rsp_code = GOEP_STATUS_SUCCESS;

	// send res to client
	bt_ftp_send_msg(MSG_ID_BT_FTPS_PULL_RSP, res , sizeof(goep_pull_rsp_struct));

	return;

FTPS_PULL_CONTINUE_ERROR:
	// close the file 
	if (FALSE != ext_fs_is_valid_handle_ucs2((act_server_cntx_p + i)->fh_ftps_send)) {
		btmtk_goep_close_rstream( (act_server_cntx_p + i)->fh_ftps_send );
		(act_server_cntx_p + i)->fh_ftps_send = FHANDLE_INVALID_VALUE;
	}

	// send error back
	res = (goep_pull_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_pull_rsp_struct),TD_CTRL);

	// memory init
	GOEP_MEMSET( res, 0, sizeof(goep_pull_rsp_struct));
	res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];
	res->rsp_code = GOEP_INTERNAL_SERVER_ERR;

	// send res to client
	bt_ftp_send_msg(MSG_ID_BT_FTPS_PULL_RSP, res , sizeof(goep_pull_rsp_struct) );    

	*more = FALSE;
}


/**
* @brief Send pull msg back to continue send data to client
* @param u4RspCode the response code
* @param ucObjPath the filepath of pull object (composed by root, current-folder and objname)
**/
BT_BOOL bt_ftps_send_obj_routine(U32 u4RspCode, const U8 *ucObjPath, BT_BOOL *bMore)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	goep_pull_rsp_struct *res;
	BT_BOOL l_ret;
	S32 ret;
	U32 fileSize;
	U8 i = 0;
	l_ret = TRUE; // send with success
	*bMore = FALSE;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_SEND_OBJ, ftps_cntx_p->goep_conn_id[i]) );

	// Reset the context first
	(act_server_cntx_p + i)->total_send_obj_len = 0;
	(act_server_cntx_p + i)->remain_send_obj_len = 0;
	res = (goep_pull_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_pull_rsp_struct),TD_CTRL);
	GOEP_MEMSET(res, 0, sizeof(goep_pull_rsp_struct));

	// GOEP_Report("[ftps] send_obj rspcode:%d ", (S32) u4RspCode );

	if (GOEP_STATUS_SUCCESS == u4RspCode) {
		if (FALSE == btmtk_goep_is_valid_handle((act_server_cntx_p + i)->fh_ftps_send)) {
			(act_server_cntx_p + i)->fh_ftps_send = btmtk_goep_open_rstream((U16 *)ucObjPath, 0);
		}
		ret = (BT_BOOL) btmtk_goep_get_filesize((FHANDLE)(act_server_cntx_p + i)->fh_ftps_send,
				(U8 *) ucObjPath, &fileSize);
		// GOEP_Report("[ftps] send_obj get size ret:%d size:%d", ret, (S32) fileSize);

		if ((BT_BOOL)ret == FALSE) {
			res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];
			res->rsp_code = GOEP_INTERNAL_SERVER_ERR;

			// Send MSG_ID_BT_FTPS_PULL_RSP primitive
			bt_ftp_send_msg(MSG_ID_BT_FTPS_PULL_RSP, res, sizeof(goep_pull_rsp_struct));
			BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
			return FALSE;
		}

		(act_server_cntx_p + i)->total_send_obj_len = fileSize;
		(act_server_cntx_p + i)->remain_send_obj_len = fileSize;
			
		// not empty obj
		if (fileSize > 0) {
			// goep can't handle first res pkt with data
			// FTP_DEBUG(kal_print("FTPS SEND OBJ (RES FIRST PKT)\n");)

			res->pkt_type = GOEP_FIRST_PKT;
			res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];
			res->frag_len = 0;
			res->frag_ptr = (act_server_cntx_p + i)->ftps_obj_buff;
			res->total_obj_len = fileSize;
			res->rsp_code = GOEP_STATUS_SUCCESS;

			*bMore = TRUE;
		} else { 
			// Empty body
			// FTP_DEBUG(kal_print("FTPS SEND OBJ (EMPTY FOLDER CONTENT FILE)\n");)
			// film res pkt_type attibute
			res->pkt_type = GOEP_SINGLE_PKT;
			res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];
			res->frag_len = 0;
			res->frag_ptr = (act_server_cntx_p + i)->ftps_obj_buff;
			res->total_obj_len = 0;
			res->rsp_code = GOEP_STATUS_SUCCESS;

			btmtk_goep_close_rstream(act_server_cntx_p->fh_ftps_send);
			(act_server_cntx_p + i)->fh_ftps_send = FHANDLE_INVALID_VALUE;

			BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
			l_ret = TRUE; 
		}

	} else {
		res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];
		res->frag_len = 0;
		res->frag_ptr = (act_server_cntx_p + i)->ftps_obj_buff;
		res->total_obj_len = 0;
		res->rsp_code = (U8)u4RspCode;
		BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
		l_ret = FALSE;
	}

	bt_ftp_send_msg(MSG_ID_BT_FTPS_PULL_RSP, res, sizeof(goep_pull_rsp_struct));
	return l_ret;
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftps_send_folder_content_routine
 * DESCRIPTION
 *  This function is to send the folder content requested from client
 * PARAMETERS
 *  ind     [?]         
 *  i       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftps_send_folder_content_routine(U8 index, U32 u4RspCode, const U8 *ucFilePath)
{
	// TODO: copy  ucFilePath to act_server_cntx_p->ftps_filepath
	BT_BOOL bMore;
	ext_ucs2ncpy((S8 *)act_server_cntx_p->ftps_filepath, (const S8 *) ucFilePath,
		sizeof(act_server_cntx_p->ftps_filepath)/2);

	if (FALSE != bt_ftps_send_obj_routine(GOEP_STATUS_SUCCESS, act_server_cntx_p->ftps_filepath, &bMore)) {
		(act_server_cntx_p)->send_obj_type = BT_FTPS_OBJ_TYPE_FOLDER;
		// send the default xml file back
		if (TRUE == bMore) {
			BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_SENDING);
			btmtk_ftps_ui_callback(BT_FTPSUI_BROWSING, 
					(void *) (act_server_cntx_p->total_send_obj_len- act_server_cntx_p->remain_send_obj_len));
		} else {
			btmtk_ftps_ui_callback(BT_FTPSUI_BROWSED, (void *) GOEP_STATUS_SUCCESS);     
		}
	} else {
		btmtk_ftps_ui_callback( BT_FTPSUI_BROWSED, (void *)GOEP_INTERNAL_SERVER_ERR); 
	}
}



/*****************************************************************************
 * FUNCTION
 *  bt_ftps_find_goep_id
 * DESCRIPTION
 *  This function is to find the goep_id from ftps context
 * PARAMETERS
 *  index       [IN]        
 * RETURNS
 *  U32 index
 *****************************************************************************/
U32 bt_ftps_find_goep_id(U8 index)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	//FTP_DEBUG(kal_print("FTPS FIND INDEX\n");)
	// fine the index
	for (i = 0; i < FTP_MAX_CLIENT_SUPPORT; i++)
	{
		if (ftps_cntx_p->goep_conn_id[i] == index)
		{
			return i;
		}
	}

	// index is not correct
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_FIND_GOEP_ID_FAILED, ftps_cntx_p->goep_conn_id[0], index) );
	return 0xFF;
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftps_set_fs_flag
 * DESCRIPTION
 *  
 * PARAMETERS
 *  index       [IN]        
 * RETURNS
 *  U32 index
 *****************************************************************************/
 #if 0
static void bt_ftps_set_fs_flag(S32 ret)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_SET_FS_FLAG, ret) );

    ftps_cntx_p->ftps_flag = 0;
#if 0
    //TODO convert the platform error code
    /// setup the flag of ftps 
    if (ret == FS_ROOT_DIR_FULL)
    {
        BT_FTPS_SET_FLAG(FS_ERR_ROOT_DIR_FULL);
    }
    else if (ret == FS_DISK_FULL)
    {
        BT_FTPS_SET_FLAG(FS_ERR_DISK_FULL);
    }
#ifdef __USB_IN_NORMAL_MODE__
    else if (ret == FS_DEVICE_EXPORTED_ERROR)
    {
        BT_FTPS_SET_FLAG(FS_ERR_DEVICE_EXPORTED);
    }
#endif /* __USB_IN_NORMAL_MODE__ */ 
    else if (ret == FS_DEVICE_BUSY)
    {
        BT_FTPS_SET_FLAG(FS_ERR_FS_DEVICE_BUSY);
    }
    else if (ret == FS_LOCK_MUTEX_FAIL)
    {
        BT_FTPS_SET_FLAG(FS_ERR_FS_LOCK_MUTEX_FAIL);
    }
    else if (ret == FS_MEDIA_CHANGED
                ||ret == FS_MSDC_MOUNT_ERROR
                ||ret == FS_MSDC_PRESNET_NOT_READY
                ||ret == FS_MSDC_NOT_PRESENT
                ||ret == FS_MSDC_READ_SECTOR_ERROR
                ||ret == FS_MSDC_WRITE_SECTOR_ERROR
                ||ret == FS_MSDC_DISCARD_SECTOR_ERROR)

    {
        BT_FTPS_SET_FLAG(FS_ERR_FS_NO_CARD);
    }
    else if (ret == btmtk_goep_write_wstream_PROTECTION)
    {
        BT_FTPS_SET_FLAG(FS_ERR_btmtk_goep_write_wstream_PROTEC);
    }
    else
    {
        BT_FTPS_SET_FLAG(FS_ERR_WRITE_FILE_FAIL);
    }
#endif
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftps_usb_plugin_handler
 * DESCRIPTION
 *  when usb mass plug in, this func will be called
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_ftps_usb_plugin_handler(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    U8 *all_path_name_ptr = NULL;
    U32 i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_USB_PLUG_IN) );
    switch ((act_server_cntx_p + i)->ftps_state)
    {
        case BT_FTPS_STATE_RECEIVING:
            /*close pushed file handle*/
            if ((act_server_cntx_p + i)->fh_ftps_recv)
            {
                btmtk_goep_close_wstream(act_server_cntx_p->fh_ftps_recv);
                (act_server_cntx_p + i)->fh_ftps_recv = FHANDLE_INVALID_VALUE;
            }

            /* delete pushed file*/
            all_path_name_ptr = GET_CTRL_BUFFER(FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
            if(all_path_name_ptr == NULL)
            {
                return;
            }
            ext_ucs2ncpy((S8 *) all_path_name_ptr, (S8 *) bt_ftps_get_current_folder((U8)i), FMGR_MAX_PATH_LEN);
            ext_ucs2ncat((S8 *) all_path_name_ptr, (S8 *) (act_server_cntx_p + i)->ftps_obj_name, FMGR_MAX_PATH_LEN);
            btmtk_goep_delete_file((const U16*)all_path_name_ptr);
            if( NULL != all_path_name_ptr)
            {
                FREE_CTRL_BUFFER((void *)all_path_name_ptr);
                all_path_name_ptr = NULL;
            }


            return;

    }    
}
#endif


/*****************************************************************************
 * FUNCTION
 *  bt_ftps_delete_pushing_file
 * DESCRIPTION
 *  when usb mass plug in, this func will be called
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftps_delete_pushing_file(U32 i)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U8 *all_path_name_ptr;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	all_path_name_ptr = GET_CTRL_BUFFER(FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
	BT_FTP_ASSERT(all_path_name_ptr != NULL);
	GOEP_MEMSET(all_path_name_ptr, 0, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
	ext_ucs2ncpy((S8 *) all_path_name_ptr, (const S8 *) bt_ftps_get_current_folder((U8)i), FMGR_MAX_PATH_LEN);
	ext_ucs2ncat((S8 *) all_path_name_ptr, (const S8 *) (act_server_cntx_p + i)->ftps_obj_name,  FMGR_MAX_PATH_LEN);
	btmtk_goep_delete_file((const U16*)all_path_name_ptr);


	if (all_path_name_ptr)
	{
		FREE_CTRL_BUFFER((void *)all_path_name_ptr);
		all_path_name_ptr = NULL;
	}
	return;

}    



void bt_ftps_send_auth_req_msg(U8 goep_conn_id,const U8 *realm,const U8 *ucPwd, U32 u4PwdLen)
{
	goep_auth_req_struct *req;
	req = (goep_auth_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_auth_req_struct),TD_CTRL);
	GOEP_MEMSET( req, 0, sizeof(goep_auth_req_struct));

	req->goep_conn_id = goep_conn_id;
	req->uuid_len = sizeof(OBEX_FTP_UUID) ;
	GOEP_MEMCPY( (void *)req->uuid, (void *)OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID) );

	ext_strncpy( (S8 *) req->realm_str, (S8 *)realm, sizeof(req->realm_str) );
	req->realm_len = (U8)ext_strlen((const S8 *) req->realm_str);

	GOEP_MEMCPY( (void *)req->passwd, (void *)ucPwd, u4PwdLen );
	req->passwd_len = (U8) u4PwdLen;
	req->goep_role = GOEP_SERVER_ROLE;

	bt_ftp_send_msg(MSG_ID_BT_FTPS_AUTH_REQ, req , sizeof(goep_auth_req_struct) );
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftps_send_push_rsp_msg
 * DESCRIPTION
 *  This function is used to send a push_res to client
 * PARAMETERS
 *  ind     [?]         
 *  i       [IN]        
 *  goep_push_ind_struct* U32(?)
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftps_send_push_rsp_msg(U8 goep_conn_id, U8 rsp_code)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	goep_push_rsp_struct *res;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	res = (goep_push_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_push_rsp_struct),TD_CTRL);

	GOEP_MEMSET( res, 0, sizeof(goep_push_rsp_struct));
	res->goep_conn_id = goep_conn_id;
	res->rsp_code = rsp_code;
	bt_ftp_send_msg(MSG_ID_BT_FTPS_PUSH_RSP, res , sizeof(goep_push_rsp_struct) );
	return;
}


void bt_ftps_send_pull_rsp_msg(U8 goep_conn_id, U8 rsp_code)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	goep_pull_rsp_struct *res;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	res = (goep_pull_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_pull_rsp_struct),TD_CTRL);

	GOEP_MEMSET( res, 0, sizeof(goep_pull_rsp_struct));
	res->goep_conn_id = goep_conn_id;
	res->rsp_code = rsp_code;
	bt_ftp_send_msg(MSG_ID_BT_FTPS_PULL_RSP, res , sizeof(goep_pull_rsp_struct) );
	return;
}


void bt_ftps_send_pushfile_rsp_msg(U8 goep_conn_id, U8 rsp_code, U8 *filepath,
        U32 len, U8 srm) {
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_ftps_pushfile_rsp_struct *res;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    res = (bt_ftps_pushfile_rsp_struct*) CONSTRUCT_LOCAL_PARAM(
            sizeof(bt_ftps_pushfile_rsp_struct), TD_CTRL);

    GOEP_MEMSET(res, 0, sizeof(bt_ftps_pushfile_rsp_struct));
    res->goep_conn_id = goep_conn_id;
    res->rsp_code = rsp_code;

    res->filepath_len = min(len, sizeof(res->filepath) - 2);
    GOEP_MEMCPY(res->filepath, filepath, res->filepath_len);
    res->srm_enable = srm;

    bt_ftp_send_msg(MSG_ID_BT_FTPS_PUSHFILE_RSP, res,
            sizeof(bt_ftps_pushfile_rsp_struct));
    return;
}

void bt_ftps_send_abortfile_req_msg(U8 goep_conn_id, U8 rsp_code) {
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_ftps_abortfile_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req = (bt_ftps_abortfile_req_struct*) CONSTRUCT_LOCAL_PARAM(
            sizeof(bt_ftps_abortfile_req_struct), TD_CTRL);

    GOEP_MEMSET(req, 0, sizeof(bt_ftps_abortfile_req_struct));
    req->goep_conn_id = goep_conn_id;
    req->rsp_code = rsp_code;

    bt_ftp_send_msg(MSG_ID_BT_FTPS_ABORTFILE_REQ, req,
            sizeof(bt_ftps_abortfile_req_struct));
    return;
}

void bt_ftps_send_pullfile_rsp_msg(U8 goep_conn_id, U8 rsp_code, U8 *filepath,
        U32 len, U8 srm, U8 waitCounter) {
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_ftps_pullfile_rsp_struct *res;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    res = (bt_ftps_pullfile_rsp_struct*) CONSTRUCT_LOCAL_PARAM(
            sizeof(bt_ftps_pullfile_rsp_struct), TD_CTRL);

    GOEP_MEMSET(res, 0, sizeof(bt_ftps_pullfile_rsp_struct));
    res->goep_conn_id = goep_conn_id;
    res->rsp_code = rsp_code;
    res->filepath_len = min(len, sizeof(res->filepath) - 2);
    GOEP_MEMCPY(res->filepath, filepath, res->filepath_len);
    res->srm_enable = srm;
    res->srm_waitcount = waitCounter;

    bt_ftp_send_msg(MSG_ID_BT_FTPS_PULLFILE_RSP, res,
            sizeof(bt_ftps_pullfile_rsp_struct));
    return;
}

void bt_ftps_send_disconnect_req_msg(U8 goep_conn_id)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	goep_disconnect_req_struct *req;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	req = (goep_disconnect_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_disconnect_req_struct),TD_CTRL);

	GOEP_MEMSET(req, 0, sizeof(goep_disconnect_req_struct));
	req->goep_conn_id = goep_conn_id;
	req->uuid_len = sizeof(OBEX_FTP_UUID) ;
	GOEP_MEMCPY((void *)req->uuid, (void *)OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID));
	bt_ftp_send_msg(MSG_ID_BT_FTPS_TPDISCONNECT_REQ, req , sizeof(goep_disconnect_req_struct));

	return;
}


void bt_ftps_send_auth_rsp_msg(U8 goep_conn_id,const U8 *ucUserId,const U8 *ucPwd, U32 u4PwdLen)
{
	goep_auth_rsp_struct *rsp;

	rsp = (goep_auth_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_auth_rsp_struct),TD_CTRL);
	GOEP_MEMSET( rsp, 0, sizeof(goep_auth_rsp_struct));

	rsp->goep_conn_id = goep_conn_id;
	rsp->uuid_len = sizeof(OBEX_FTP_UUID);
	GOEP_MEMCPY((void *)rsp->uuid, (void *)OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID));

	ext_strncpy((S8 *) rsp->userid, (S8 *)ucUserId, sizeof(rsp->userid));
	rsp->userid_len = (U8) ext_strlen((const S8 *) rsp->userid);
	GOEP_MEMCPY((void *)rsp->passwd, (void *)ucPwd, u4PwdLen);
	rsp->passwd_len = (U8) u4PwdLen;

	bt_ftp_send_msg(MSG_ID_BT_FTPS_AUTH_RSP, rsp, sizeof(goep_auth_rsp_struct));
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftps_push_fh_check_timer_hdler
 * DESCRIPTION
 *  This function is to close the fh of pushing file, and delete it when error happens
 * PARAMETERS
 *  index       [IN]        
 * RETURNS
 *  U32 index
 *****************************************************************************/
 #if 0
static void bt_ftps_push_fh_check_timer_hdler(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 err_code;
    U32 i = 0;
    GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_ICON_STOP_TIMER) );


    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    err_code = btmtk_goep_close_wstream(act_server_cntx_p->fh_ftps_recv);
    if (err_code < EXT_FS_OK && err_code != FHANDLE_INVALID_VALUE)
    {
        //StartTimer(BT_FTPS_PUSH_FH_CHECK_TIMER,FTPS_PUSH_FH_CHECK_DUR,bt_ftps_push_fh_check_timer_hdler);   
    }
    else
    {
        (act_server_cntx_p + i)->fh_ftps_recv = FHANDLE_INVALID_VALUE;
        btmtk_goep_delete_file((U16 *) (act_server_cntx_p + i)->temp_pushing_file);

        GOEP_MEMSET((act_server_cntx_p + i)->temp_pushing_file, 0, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
        (act_server_cntx_p + i)->ftps_fh_check_timer_set = FALSE;

    }
    return;

}
#endif

/*****************************************************************************
 * FUNCTION
 *  bt_ftps_get_current_folder
 * DESCRIPTION
 *  
 * PARAMETERS
 *  index       [IN]        
 * RETURNS
 *  U32 index
 *****************************************************************************/
const U8* bt_ftps_get_current_folder(U8 i)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	static U8 out_path[FTP_MAX_OBJ_NAME_LEN];

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	/// should be composed by root_folder & current_folder
	bt_ftp_compose_path(ftps_cntx_p->root_folder,
			(act_server_cntx_p + i)->current_folder,
			out_path,
			sizeof(out_path));

	return out_path;
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftp_connection_terminated
 * DESCRIPTION
 *  This function is to reset cntx when the connection terminated
 * PARAMETERS
 *  index       [IN]        
 *  role        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftps_connection_terminated(U32 index, U8 reason)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_CONNECTION_TERMINATED, reason) );

	// FTPS
	if (0 == reason) {
		reason = GOEP_USER_UNKNOW;
	}

	//FTP_DEBUG(kal_print("FTPS CONNECTION TERMINATED\n");)
	if (FALSE != ext_fs_is_valid_handle_ucs2((act_server_cntx_p + index)->fh_ftps_send)) {
		btmtk_goep_close_rstream((act_server_cntx_p + index)->fh_ftps_send);
		(act_server_cntx_p + index)->fh_ftps_send = (FHANDLE) FHANDLE_INVALID_VALUE;

		if ((act_server_cntx_p + index)->send_obj_type == BT_FTPS_OBJ_TYPE_FOLDER) {
			//FTP_DEBUG(kal_print("FTPS CONNECTION TERMINATED DEL FOLDER CONTENT FILE\n");)
			// delete the folder content file
			btmtk_ftps_ui_callback(BT_FTPSUI_BROWSED, (void *)(reason==0?(U8)GOEP_USER_UNKNOW:reason)); 
		}

		if ((act_server_cntx_p + index)->send_obj_type == BT_FTPS_OBJ_TYPE_FILE) {
			btmtk_ftps_ui_callback(BT_FTPSUI_PULLED, (void *)(reason==0?(U8)GOEP_USER_UNKNOW:reason)); 
		}
	}

	(act_server_cntx_p + index)->send_obj_type = BT_FTPS_OBJ_TYPE_NONE;

	if (FALSE != ext_fs_is_valid_handle_ucs2((act_server_cntx_p + index)->fh_ftps_recv)) {
		btmtk_goep_close_wstream((act_server_cntx_p + index)->fh_ftps_recv);
		(act_server_cntx_p + index)->fh_ftps_recv = (FHANDLE) FHANDLE_INVALID_VALUE;
		btmtk_ftps_ui_callback(BT_FTPSUI_PUSHED, (void *)(reason==0?GOEP_USER_UNKNOW:reason));

		btmtk_goep_delete_file((U16 *)act_server_cntx_p->ftps_filepath);
	}

	// TODO: fill with error cause
	btmtk_ftps_ui_callback(BT_FTPSUI_DISCONNECTED, (void *) (U32) reason); 

	GOEP_MEMSET((S8 *)(act_server_cntx_p+index)->current_folder, 0,
			sizeof((S8 *)(act_server_cntx_p+index)->current_folder));

	if (NULL != (act_server_cntx_p+index)->ftps_filepath) {
		ext_ucs2ncpy((S8 *)(act_server_cntx_p+index)->ftps_filepath, (const S8 *)L"", 4);    
	}

	// clear the released g_conn_id
	ftps_cntx_p->g_conn_id[index] = 0xFF;   /// When disconnected, the g_conn_id is 0xFF
	act_server_cntx_p->dis_from_user = FALSE;

	return;
}

/**
* @brief Convert a UCS2 string from host order to network order 
* bt_ftp_obj_name_to_host_order
* @param  h_order_name host order
* @param  n_order_name network order        
**/
static void bt_ftp_obj_name_to_host_order(U8 *h_order_name, U8 *n_order_name)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	U32 bytes_len = ext_ucs2strlen((S8 *) n_order_name) * 2;
	U8 t;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	//FTP_DEBUG(kal_print("FTP OBJ NAME SWAPCPY\n");)
	for (i = 0; i < bytes_len; i += 2)    // includes null temination
	{
		t = n_order_name[i];
		h_order_name[i] = n_order_name[i + 1];
		h_order_name[i + 1] = t;
	}

	h_order_name[bytes_len] = 0;
	h_order_name[bytes_len + 1] = 0;
}




BT_BOOL bt_ftps_preprocess_pull_obj_name(U8 *ucObjName ) {
	U16 j, len;

	if (NULL == ucObjName)
		return FALSE;

	len = ext_ucs2strlen((S8 *)ucObjName) * 2;

	// switch '/' to '\' if necessary
	for (j = 0; j < len;)
	{
		// '/'
		if ( ucObjName[j] == 0x2F && ucObjName[j+1] == 0)
		{
			// switch
			ucObjName[j] = 0x5C;
		}
		// '\'
		else if (ucObjName[j] == 0x5C && ucObjName[j+1] == 0)
		{
			// don't have to change
			break;
		}
		j += 2;
	}    
	return TRUE;
}

BT_BOOL bt_ftps_is_auto(BT_FTPS_AUTO_BITMASK action)
{
	// check the auto feature, act_server_cntx_p->ftps_auto_mask
	if(action == (act_server_cntx_p->ftps_auto_mask & action)) {
		GOEP_Report("[ftps] bt_ftps_is_auto bitmask:%d", action);
		return TRUE;
	}
	return FALSE;
}

static void bt_ftps_flush_one_nbuffer_to_file(){
	S32  i4Written;
	// write all the mem block in nbufer
	if (FALSE != ext_nbuffer_pop_data_to_file(&act_server_cntx_p->ftps_mempool,
			act_server_cntx_p->fh_ftps_recv, (U32 *) &act_server_cntx_p->ftps_fs_status, (U32 *) &i4Written)) {
		if (EXT_FS_OK != act_server_cntx_p->ftps_fs_status) {
			return;
		}
	} else {

	}
}


void bt_ftps_flush_all_nbuffer_to_file(){
	S32 i4Written;
	U8  index;
	// write all the mem block in nbufer
	for (index = 0; index < NBUFFER_NUM_OF_MEM; index++) {
		//if( FALSE != ext_nbuffer_pop_data( &act_server_cntx_p->ftps_mempool, GOPP(ops_push_buff), OPPS_MAX_OBEX_PACKET_LENGTH, &i4Read) ){
		if (FALSE != ext_nbuffer_pop_data_to_file(&act_server_cntx_p->ftps_mempool,
				act_server_cntx_p->fh_ftps_recv, (U32 *) &act_server_cntx_p->ftps_fs_status, (U32 *) &i4Written)) {
			if (EXT_FS_OK != act_server_cntx_p->ftps_fs_status) {
				return;
			}
		} else {
			break;
		}
	}
}

/************************************** Handler ***********************************/

/*****************************************************************************
 * FUNCTION
 *  btmtk_ftps_register_server_cnf_handler
 * DESCRIPTION
 *  This function is to handle server REGISTER_CNF msg
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftps_register_server_cnf_handler(void *msg)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	goep_register_server_cnf_struct *rsp = (goep_register_server_cnf_struct*) msg;


	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	/* index exceeds the num of client supported */
	bt_ext_log("Enter %s", __FUNCTION__);
	if (msg == NULL) {
		bt_ext_err("msg is null.");
		return;
	}

	if (rsp->req_id >= FTP_MAX_CLIENT_SUPPORT) {
		//FTP_DEBUG(kal_print("FTPS REGISTER RSP ERROR (req_id >= client support)\n");)
		bt_ext_err("req_id: %d", rsp->req_id);
		BT_FTP_ASSERT(0);
	}

	bt_ext_log("Checking act_server_cntx_p");
	if (NULL == act_server_cntx_p) {
		// No active server cntx, ignore the cnf
		goep_deregister_server_req_struct *dereg_req;

		dereg_req = (goep_deregister_server_req_struct *) 
				CONSTRUCT_LOCAL_PARAM(sizeof(goep_deregister_server_req_struct), TD_CTRL);
		dereg_req->goep_conn_id = rsp->goep_conn_id;

		bt_ftp_send_msg(MSG_ID_BT_FTPS_DEREGISTER_SERVER_REQ, 
				dereg_req, sizeof(goep_deregister_server_req_struct));
		GOEP_Report("[FTPS][WRN] no context ignore the register cnf");
		return;
	}

	bt_ext_log("Checking registering responce");
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_REGISTER_SERVER_CNF_HDLR, (act_server_cntx_p + rsp->req_id)->ftps_state));
	// rsp->req_id == the connection index
	switch ((act_server_cntx_p )->ftps_state)
	{
		case BT_FTPS_STATE_REGISTERING:
			if (rsp->rsp_code == GOEP_STATUS_SUCCESS) {	// succeed
				bt_ext_log("[BT][FTP] Registering succeed.");
				// save goep connection id
				ftps_cntx_p->goep_conn_id[rsp->req_id] = rsp->goep_conn_id;
				BT_FTPS_STATE_TRANS(rsp->req_id, BT_FTPS_STATE_ACTIVE);
			} else {	// error handling
				bt_ext_log("[BT][FTP] Registering failed.");
				BT_FTPS_STATE_TRANS(rsp->req_id, BT_FTPS_STATE_IDLE);
				// BT_FTP_ASSERT(0);
			}

			break;

		case BT_FTPS_STATE_IDLE:
			return;

		default:
			//// BT_FTPS_LOG_STATE_ERR(rsp->req_id);
			bt_ext_log("[BT][FTP] Warning: register_server_cnf in unexpected state: %d",
					(act_server_cntx_p)->ftps_state);
			// BT_FTP_ASSERT(0);
			break;
	}

	// if there is connection wating for register response don't register SDP
	for (i = 0; i < FTP_MAX_CLIENT_SUPPORT; i++) {
		if ((act_server_cntx_p + i)->ftps_state == BT_FTPS_STATE_REGISTERING) {
			return;
		}
	}

	// Send SDP Register primitive
	ext_ucs2ncpy((S8 *)act_server_cntx_p->ftps_filepath, (const S8 *) ftps_cntx_p->root_folder,
			 sizeof(act_server_cntx_p->ftps_filepath) /2);

	btmtk_ftps_ui_callback(BT_FTPSUI_READY, (void *) (U32) rsp->rsp_code); 

}


/*****************************************************************************
 * FUNCTION
 *  btmtk_ftps_deregister_server_cnf_handler
 * DESCRIPTION
 *  This function is to handle server DEREGISTER_CNF msg
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftps_deregister_server_cnf_handler(void *msg)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	goep_deregister_server_cnf_struct *rsp = (goep_deregister_server_cnf_struct*) msg;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	// chcek rsp code first
	switch (rsp->rsp_code)
	{
		// deregister success
		case GOEP_STATUS_SUCCESS:
			break;
		// deregister failed
		default:
			BT_FTP_ASSERT(0);
			break;
	}

	// find the connection then reset the context
	i = bt_ftps_find_goep_id(rsp->goep_conn_id);

	// reset
	if (i == 0xff) {
		return;
	}

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_DEREGISTER_SERVER_CNF_HDLR, (act_server_cntx_p + i)->ftps_state));
	switch ((act_server_cntx_p + i)->ftps_state) {
		case BT_FTPS_STATE_DEACTIVATING:
			// switch state to IDLE
			// don't have to reset context
			BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_IDLE);
			break;

		// Exception!! only the state deactivatig is allowed
		default:
			//// BT_FTPS_LOG_STATE_ERR(i);
			BT_FTP_ASSERT(0);
			return;
	}

	act_server_cntx_p = NULL;

	// btmtk_ftps_ui_callback(BT_FTPS_STATE_DEACTIVATING, 0);
	btmtk_ftps_ui_callback(BT_FTPS_STATE_IDLE, 0);
	// no connection is wating for deregister rsp -> deactivate cnf to CM
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_ftps_connect_ind_handler
 * DESCRIPTION
 *  This function is to handle server CONNECT_IND msg
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftps_connect_ind_handler(void *msg)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	goep_connect_ind_struct *ind = (goep_connect_ind_struct*) msg;
	goep_connect_rsp_struct *rsp;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	// find the goep_conn_id
	i = bt_ftps_find_goep_id(ind->goep_conn_id);

	// reset
	if (i == 0xff) {
		return;
	}

	// check the bd_addr correctness (authorize one == connecting one)
	if (GOEP_MEMCMP(&(((act_server_cntx_p + i)->ftps_bt_device).lap), 4, &(ind->bd_addr.lap), 4) ||
		GOEP_MEMCMP(&(((act_server_cntx_p + i)->ftps_bt_device).uap), 1, &(ind->bd_addr.uap), 1) ||
		GOEP_MEMCMP(&(((act_server_cntx_p + i)->ftps_bt_device).nap), 2, &(ind->bd_addr.nap), 2)) {
		// FTP_DEBUG(kal_print("FTPS CONNECT IND BUT DIFFERENT BD_ADDR BETWEEN AUTHORIZE\n"));
		// BT_FTP_ASSERT(0);
	}

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_CONNECT_IND_HDLR, (act_server_cntx_p + i)->ftps_state));
	// GOEP_Report("[ftps] connect_ind struct goep_id:%d cm_id:%d peer:mru:%d\n", 
	//		ind->goep_conn_id, ind->cm_conn_id, ind->peer_mru );
	// GOEP_Report( "[ftps] server_cntx id:%d state:%d goep_id:%d conn_id:%d \n",
	//		i, (act_server_cntx_p + i)->ftps_state, ftps_cntx_p->goep_conn_id[i], ftps_cntx_p->g_conn_id[i]);

	// check the connection state
	switch ((act_server_cntx_p + i)->ftps_state) {
		// correct state to accept the authorize ind
		case BT_FTPS_STATE_ACTIVE: {
			// Construct goep connect res ilm
			rsp = (goep_connect_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_connect_rsp_struct),TD_CTRL);

			// memory init
			GOEP_MEMSET( rsp, 0, sizeof(goep_connect_rsp_struct));
			rsp->rsp_code = GOEP_STATUS_SUCCESS;
			rsp->goep_conn_id = ftps_cntx_p->goep_conn_id[i];

			ext_ucs2ncpy((S8*) ftps_cntx_p->root_folder,
					(const S8 *) g_ftps_root_folder,
					sizeof(ftps_cntx_p->root_folder));

			if (FALSE == btmtk_goep_is_folder_exist((const U16 *) ftps_cntx_p->root_folder)) {
				btmtk_goep_create_folder((const U16 *) ftps_cntx_p->root_folder);
			}

#if 0
			/// APPI may set up a fake root folder. Change real path when put file.
			if (FALSE == btmtk_goep_is_folder_exist((const U16 *) ftps_cntx_p->root_folder)) {
			GOEP_Report("[ftps][err] root folder is not exist !\n");
			rsp->rsp_code = GOEP_STATUS_FAILED;
			bt_ftp_send_msg(MSG_ID_BT_FTPS_CONNECT_RSP, rsp, sizeof(goep_connect_rsp_struct));
			return ;
			}
#endif

			///< check the read only permission when client connects to server.
			// btmtk_ftps_reset_read_only_permission(); 
				
			BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
			(act_server_cntx_p+i)->ftps_browse_depth = 0;
			(act_server_cntx_p+i)->challeng_options = 0;

			///< clean up the current folder path
			GOEP_MEMSET((U8 *)(act_server_cntx_p+i)->new_folder, 0, 
					sizeof((act_server_cntx_p+i)->new_folder)); 
			GOEP_MEMSET((U8 *)(act_server_cntx_p+i)->current_folder, 0,
					sizeof((act_server_cntx_p+i)->current_folder));
			GOEP_MEMSET((U8 *)(act_server_cntx_p+i)->realm, 0,
					sizeof((act_server_cntx_p+i)->realm));

			// check the max transfer unit size
			if (ind->peer_mru < (act_server_cntx_p + i)->ftps_obj_buff_size) {
				(act_server_cntx_p + i)->send_obex_pkt_size = ind->peer_mru;
			} else {
				(act_server_cntx_p + i)->send_obex_pkt_size = (act_server_cntx_p + i)->ftps_obj_buff_size;
			}

			// Send MSG_ID_BT_FTPS_CONNECT_RSP primitive
			bt_ftp_send_msg(MSG_ID_BT_FTPS_CONNECT_RSP, rsp , sizeof(goep_connect_rsp_struct) );

			// update ftps context
			ftps_cntx_p->g_conn_id[i] = ind->cm_conn_id;
#ifdef __BT_GOEP_V2__			
			(act_server_cntx_p + i)->bConnType = ind->tptype;
#endif
			btmtk_ftps_ui_callback(BT_FTPSUI_CONNECTED, (void*) &(ind->bd_addr));
			return;
		}

		case BT_FTPS_STATE_CONNECTED:   
			// IVT IOT Cover:
			// send connect res failed to client 
			// Construct goep connect res ilm 

			rsp = (goep_connect_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_connect_rsp_struct),TD_CTRL);

			// memory init
			GOEP_MEMSET( rsp, 0, sizeof(goep_connect_rsp_struct));
			rsp->rsp_code = GOEP_STATUS_SUCCESS;
			rsp->goep_conn_id = ftps_cntx_p->goep_conn_id[i];

			ext_ucs2ncpy((S8*) ftps_cntx_p->root_folder,
					(const S8 *) g_ftps_root_folder,
					sizeof( ftps_cntx_p->root_folder));

			if (FALSE == btmtk_goep_is_folder_exist((const U16 *) ftps_cntx_p->root_folder)) {
				btmtk_goep_create_folder( (const U16 *) ftps_cntx_p->root_folder);
			}

#if 0
			/// APPI may set up a fake root folder. Change real path when put file.
			if (FALSE == btmtk_goep_is_folder_exist( (const U16 *) ftps_cntx_p->root_folder)) {
				GOEP_Report("[ftps][err] root folder is not exist !\n");
				rsp->rsp_code = GOEP_STATUS_FAILED;
				bt_ftp_send_msg(MSG_ID_BT_FTPS_CONNECT_RSP, rsp , sizeof(goep_connect_rsp_struct));
				return ;
			}
#endif

			// update ftps context
			ftps_cntx_p->g_conn_id[i] = ind->cm_conn_id; 
			BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
			(act_server_cntx_p+i)->ftps_browse_depth = 0;

			// Send MSG_ID_BT_FTPS_CONNECT_RSP primitive
			bt_ftp_send_msg(MSG_ID_BT_FTPS_CONNECT_RSP, rsp , sizeof(goep_connect_rsp_struct));
			btmtk_ftps_ui_callback( BT_FTPSUI_CONNECTED, (void*) ind->dev_name); 
			return;

		default:    // wrong state
			//// BT_FTPS_LOG_STATE_ERR(i);
			BT_FTP_ASSERT(0);
			goto FTPS_DISCONNECT_REQ;
			return;
		}
		return;

FTPS_DISCONNECT_REQ:
	// switch ftps state to disconnecting
	BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_DISCONNECTING);
	// Send Disconnect REQ to goep
	bt_ftps_send_disconnect_req_msg( ftps_cntx_p->goep_conn_id[i] );
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_ftps_push_ind_handler
 * DESCRIPTION
 *  This function is to handle server PUSH_IND msg
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftps_push_ind_handler(void *msg)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i = 0;
	U64 free_size = 0;
	BT_BOOL bMore;
	goep_push_ind_struct *ind = (goep_push_ind_struct*) msg;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	/* find the connection */

	/*
	GOEP_Report("[ftps] server_cntx id:%d state:%d goep_id:%d conn_id:%d \n",
			i,
			(act_server_cntx_p + i)->ftps_state,
			ftps_cntx_p->goep_conn_id[i],
			ftps_cntx_p->g_conn_id[i]);
	*/

	i = bt_ftps_find_goep_id(ind->goep_conn_id);

	// reset
	if (i == 0xff || i != 0) {
		return;
	}
	// Points to shared memory
	ind->frag_ptr = (act_server_cntx_p + i)->ftps_obj_buff;

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_PUSH_IND_HDLR, (act_server_cntx_p + i)->ftps_state));

	if (TRUE == (act_server_cntx_p + i)->dis_from_user) {
		// close the 
		if ((act_server_cntx_p + i)->fh_ftps_recv) {
			btmtk_goep_close_wstream(act_server_cntx_p->fh_ftps_recv);
			(act_server_cntx_p + i)->fh_ftps_recv = FHANDLE_INVALID_VALUE;
		}

		(act_server_cntx_p + i)->total_send_obj_len = 0;
		(act_server_cntx_p + i)->remain_send_obj_len = 0;
		(act_server_cntx_p + i)->fh_ftps_send = FHANDLE_INVALID_VALUE;

		bt_ftps_send_push_rsp_msg( ftps_cntx_p->goep_conn_id[i], GOEP_FORBIDDEN);
		bt_ftps_send_disconnect_req_msg(ftps_cntx_p->goep_conn_id[i]);
		BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_DISCONNECTING);
		btmtk_ftps_ui_callback(BT_FTPSUI_PUSHED, (void *) GOEP_USER_CANCEL);
		return;
	}

	// Check the access right first. If the access right is read only, return failed directly
	// Use the same handling procedures to abort the transferring from server side.
	if (ftps_cntx_p->access_right == BT_FTP_ACCESS_RIGHT_READ_ONLY ||
			((act_server_cntx_p + i)->flag_abort_req_sent == TRUE)) {
		(act_server_cntx_p + i)->last_pkttype = ind->pkt_type;
			
		if ((act_server_cntx_p + i)->ftps_state == BT_FTPS_STATE_RECEIVING) {
			btmtk_goep_close_wstream(act_server_cntx_p->fh_ftps_recv);
			(act_server_cntx_p + i)->fh_ftps_recv = FHANDLE_INVALID_VALUE;

			bt_ftp_util_delete_file(bt_ftps_get_current_folder((U8)i),
					(act_server_cntx_p + i)->ftps_obj_name);

			BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
		}

		if ((act_server_cntx_p + i)->flag_abort_req_sent == TRUE) {
			bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[i], GOEP_FORBIDDEN);
			(act_server_cntx_p + i)->flag_abort_req_sent = FALSE;
		} else {
			bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[i], GOEP_UNAUTHORIZED);
		}
		return;
	}

	// check state to enter different handle case
	switch ((act_server_cntx_p + i)->ftps_state) {

		case BT_FTPS_STATE_CONNECTED: {
			/* put obj */
			if ((ind->pkt_type == GOEP_FIRST_PKT) || (ind->pkt_type == GOEP_SINGLE_PKT)) {
				// check the server is read only or not
				if (FALSE != btmtk_ftps_is_read_only()) {
					bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[i], GOEP_FORBIDDEN);
					return;
				}

				if (ind->put_type == GOEP_PUT_DELETE) {
					/* call ftps push delete routine and send res to client */
					// check the target is a folder or a file
					// delete file and response ok
					if (TRUE == bt_ftps_is_auto(BT_FTP_AUTO_PUSH)) { 
						bt_ftps_push_delete_routine((U8)i, ind);

					} else {
						bt_ftp_obj_name_to_host_order( ind->obj_name, ind->obj_name);

						bt_ftp_compose_path(ftps_cntx_p->root_folder,
								act_server_cntx_p->current_folder,
								act_server_cntx_p->new_folder,
								sizeof(act_server_cntx_p->new_folder)/2);

						bt_ftp_compose_path(act_server_cntx_p->new_folder,
								ind->obj_name,
								act_server_cntx_p->ftps_filepath,
								sizeof(act_server_cntx_p->ftps_filepath)/2);

						ext_ucs2ncpy((S8 *) act_server_cntx_p->new_folder,
								(const S8 *) act_server_cntx_p->current_folder,
								sizeof(act_server_cntx_p->new_folder)/2);

						ext_ucs2ncpy((S8 *) act_server_cntx_p->ftps_obj_name,
								(const S8 *) ind->obj_name,
								sizeof(act_server_cntx_p->ftps_obj_name)/2); 

						BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_RECEIVE_WAIT);
						btmtk_ftps_ui_callback(BT_FTPSUI_FILE_DELETE, 0);
					}
					return;

				} else {
#ifdef __MTK_EMMC_SUPPORT__
					if ((act_server_cntx_p + i)->ftps_browse_depth == 0) {
						free_size = bt_ftp_get_free_bytes((const U16*)DEFAULT_SDCARD_PATH);
					} else {
						free_size = bt_ftp_get_free_bytes((const U16*)bt_ftps_get_current_folder((U8) i));
					}
#else
					free_size = bt_ftp_get_free_bytes((const U16*)bt_ftps_get_current_folder((U8) i));
#endif
					bt_ext_log("[BT][FTP] obj size: %d, free size: %d", ind->total_obj_len, free_size);

					if (ind->total_obj_len >= free_size) {
						bt_ext_log("[BT][FTP] Space is not enough. obj size: %d, free size: %d", ind->total_obj_len, free_size);
						bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[i], GOEP_FORBIDDEN);
						return;
					}

					// receive the other pars
					BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_RECEIVING);

					if (TRUE == bt_ftps_is_auto(BT_FTP_AUTO_PUSH)) {
#ifdef __BT_GOEP_V2__
#endif
						// forbbiden 0 size normal file push for IVT IOT (from MAUI code)
						if ( ind->frag_len == 0 && ind->total_obj_len == 0 
								&& ind->put_type == GOEP_PUT_NORMAL 
								&& ind->pkt_type == GOEP_SINGLE_PKT) {
							// create file and close it
							bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[i], GOEP_FORBIDDEN);
							return;
						}

						BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_RECEIVING);
						// change the otns obj_name
						bt_ftp_obj_name_to_host_order((U8 *)ind->obj_name, (U8 *)ind->obj_name);

#ifdef __MTK_EMMC_SUPPORT__
						// Before calling bt_ftps_push_obj(), the file-path should be configured well.
						if ((act_server_cntx_p + i)->ftps_browse_depth == 0) {
							// If a user tries to push a file in the parent folder of SD cards,
							// just put the file to /mnt/sdcard
							bt_ftp_compose_path((U8 *) DEFAULT_SDCARD_PATH,
									ind->obj_name,
									(act_server_cntx_p + i)->ftps_filepath, 
									(U16)FMGR_MAX_PATH_LEN);
						} else {						
							bt_ftp_compose_path(bt_ftps_get_current_folder((U8)i),
									ind->obj_name,
									(act_server_cntx_p + i)->ftps_filepath, 
									(U16)FMGR_MAX_PATH_LEN);
						}

#else
						// Before calling bt_ftps_push_obj(), the file-path should be configured well.
						bt_ftp_compose_path(bt_ftps_get_current_folder((U8)i),
								ind->obj_name,
								(act_server_cntx_p + i)->ftps_filepath, 
								(U16)FMGR_MAX_PATH_LEN);
#endif

						bt_ftps_push_prepare_routine((const U8*) (act_server_cntx_p + i)->ftps_filepath);

						bt_ftps_push_obj(0, ind->obj_name, ind->pkt_type, 
								ind->frag_ptr, ind->frag_len, &bMore);

						bt_ext_log("[BT][FTP] AUTO PUSH, bMore: %s", (bMore ? "TRUE" : "FALSE"));
							
						if (FALSE == bMore) {
							BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
						}

					} else {
						// save the pkg type
						act_server_cntx_p->total_send_obj_len = 
								act_server_cntx_p->remain_send_obj_len = ind->total_obj_len;

						ext_ucs2ncpy((S8 *) act_server_cntx_p->ftps_obj_name,
								(const S8 *) ind->obj_name,
								sizeof(act_server_cntx_p->ftps_obj_name)/2);

						bt_ftp_obj_name_to_host_order((U8 *)act_server_cntx_p->ftps_obj_name,
								(U8 *)act_server_cntx_p->ftps_obj_name);

						bt_ftp_compose_path((const U8 *) ftps_cntx_p->root_folder,
								(const U8 *) act_server_cntx_p->current_folder,
								(U8 *) act_server_cntx_p->ftps_filepath,
								sizeof(act_server_cntx_p->ftps_filepath)/2);

						bt_ftp_compose_path((const U8 *) act_server_cntx_p->ftps_filepath,
								(const U8 *) act_server_cntx_p->ftps_obj_name,
								(U8 *) act_server_cntx_p->ftps_filepath,
								sizeof(act_server_cntx_p->ftps_filepath)/2);

						(act_server_cntx_p)->last_pkttype = ind->pkt_type;
						(act_server_cntx_p)->last_pktlen = ind->frag_len;
						BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_RECEIVE_WAIT);

						bt_ftps_push_prepare_routine((const U8*) (act_server_cntx_p + i)->ftps_filepath);
					}

					return;
				}

			} else {
				bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[i], GOEP_INTERNAL_SERVER_ERR);
				return;
			}

		}

		// continue
		case BT_FTPS_STATE_RECEIVING:
			// can't support simultaneously receiving
			// bt_ext_log("[ftps] Put: RECEIVING");
			if ((ind->pkt_type == GOEP_FIRST_PKT) || (ind->pkt_type == GOEP_SINGLE_PKT)) {	
				BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
				bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[i],
						GOEP_INTERNAL_SERVER_ERR);

			} else {
				// If the permission is changed to read only, return error.
				if (FALSE != btmtk_ftps_is_read_only()) {
					btmtk_goep_close_wstream(act_server_cntx_p->fh_ftps_recv);
					(act_server_cntx_p + i)->fh_ftps_recv = FHANDLE_INVALID_VALUE;

					btmtk_goep_delete_file((U16 *)(act_server_cntx_p +i)->ftps_filepath);

					BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
					bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[i], GOEP_FORBIDDEN);
					return;
				}

				// not another first req
				// call ftps push continue routine and send res to client
				bMore = FALSE;
				bt_ftps_write_obj_continue_routine((U8)i,
						ind->pkt_type,
						ind->frag_ptr,
						ind->frag_len,
						&bMore);

				if (FALSE == bMore) {
					BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
				}

			}
			break;

		case BT_FTPS_STATE_SENDING:
			// wrong sequence msg incoming (error handling)
			// FTP_DEBUG(kal_print("PUSH IND IN WRONG STATE(SENDING)\n");)
			// BT_FTPS_LOG_STATE_ERR(i);
			BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
			bt_ftps_send_push_rsp_msg(ftps_cntx_p->goep_conn_id[i], GOEP_INTERNAL_SERVER_ERR);
			return;
			break;

		// discard this message
		case BT_FTPS_STATE_DEACTIVATING:
		case BT_FTPS_STATE_DISCONNECTING:
			return;
			break;

		// abnormal state
		default:
			//// BT_FTPS_LOG_STATE_ERR(i);
			BT_FTP_ASSERT(0);
			return;
	}
	return;
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_ftps_pull_ind_handler
 * DESCRIPTION
 *  This function is to handle server PULL_IND msg
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftps_pull_ind_handler(void *msg)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	FHANDLE h;
	BT_BOOL bMore;
	U8 folder_listing_obj[GOEP_MAX_MIME_TYPE];
	U8 path_buf[(FMGR_MAX_PATH_LEN * ENCODING_LENGTH)];
	U8 debug_path[FMGR_MAX_PATH_LEN];
	U8 xmlpath[FMGR_MAX_PATH_LEN * ENCODING_LENGTH]; // composed root, folder and object name
	goep_pull_ind_struct *ind = (goep_pull_ind_struct*) msg;
	goep_pull_rsp_struct *res;
	U8 l_rsp_code;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	// continue writing folder content by send self primitive
	ext_strncpy((S8 *) folder_listing_obj, "x-obex/folder-listing", GOEP_MAX_MIME_TYPE);

	// find the connection
	i = bt_ftps_find_goep_id(ind->goep_conn_id);

	// reset
	if (i == 0xff)
	{
		return;
	}

	l_rsp_code = GOEP_INTERNAL_SERVER_ERR;
	(act_server_cntx_p + i)->last_pkttype = ind->pkt_type;

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_PULL_IND_HDLR, (act_server_cntx_p + i)->ftps_state, ind->pkt_type));

	/*
	GOEP_Report("[ftps] pull_ind state:%d abort:%d mime:(%s) pkt_type:%d",
			act_server_cntx_p->ftps_state,
			act_server_cntx_p->dis_from_user,
			ind->obj_mime_type,
			ind->pkt_type);
	*/

	if (TRUE == (act_server_cntx_p + i)->dis_from_user) {
		bt_ftps_send_pull_rsp_msg( ftps_cntx_p->goep_conn_id[i], GOEP_FORBIDDEN );

		// close the (act_server_cntx_p + index)->fh_ftps_send
		if (FALSE != ext_fs_is_valid_handle_ucs2(act_server_cntx_p->fh_ftps_send)) {
			btmtk_goep_close_rstream(act_server_cntx_p->fh_ftps_send);
			(act_server_cntx_p + i)->total_send_obj_len = 0;
			(act_server_cntx_p + i)->remain_send_obj_len = 0;
			(act_server_cntx_p + i)->fh_ftps_send = FHANDLE_INVALID_VALUE;
		}

		bt_ftps_send_disconnect_req_msg(ftps_cntx_p->goep_conn_id[i]);
		BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_DISCONNECTING);

		btmtk_ftps_ui_callback(BT_FTPSUI_PULLED, (void *)GOEP_USER_CANCEL);

#ifdef GOEP_TEST
		printf("[ftps] abort the transmission\n");
#endif		
		return;
	}

	// check state to enter different handle case
	switch (act_server_cntx_p->ftps_state)
	{
		case BT_FTPS_STATE_CONNECTED:

			// check if there is obj name carried by pull ind
			if (ext_ucs2strlen((S8 *) ind->obj_name) > 0) {
				bt_ftp_obj_name_to_host_order(ind->obj_name, ind->obj_name);
			}

			// check pull folder listing or file
			// pull a file
			if (((S32)0) != ext_strncmp((const S8 *) ind->obj_mime_type,
				(const S8 *) folder_listing_obj,
				(U16)(ext_strlen((const S8 *)folder_listing_obj)+1)) ) {
				
				path_buf[0] = path_buf[1] = 0;
				bt_ftp_compose_path(ftps_cntx_p->root_folder,
						act_server_cntx_p->current_folder,
						path_buf ,FMGR_MAX_PATH_LEN);

				// FTP server's root path may empty. Wait for AP to assign
				// fail-safe for enough memory to store the obj_name + obj_path (1 = '\0')
				if (ext_ucs2strlen((S8 *) path_buf) + ext_ucs2strlen((S8 *)ind->obj_name) >
						(FMGR_MAX_PATH_LEN-1)) {

					//FTP_DEBUG(kal_print("FTPS PULL IND HDLR INVALID NAME\n");)
					GOEP_Report("[ftps] pull_ind filepath is too long folder-len:%d file-len:%d",
							ext_ucs2strlen((S8 *) path_buf), ext_ucs2strlen((S8 *)ind->obj_name) );
					goto FTPS_PULL_IND_ERR;
				}

				bt_ftps_preprocess_pull_obj_name( ind->obj_name ); // porting
				(act_server_cntx_p + i)->flag_abort_req_sent = FALSE;

				if (FALSE == bt_ftps_is_auto(BT_FTP_AUTO_PULL)) { 
						
					ext_ucs2ncpy((S8 *)act_server_cntx_p->ftps_obj_name, 
							(const S8 *) ind->obj_name, sizeof(act_server_cntx_p->ftps_obj_name) /2);

					bt_ftp_compose_path((const U8 *) ftps_cntx_p->root_folder,
							(const U8 *) act_server_cntx_p->current_folder,
							(U8 *) act_server_cntx_p->ftps_filepath,
							sizeof(act_server_cntx_p->ftps_filepath)/2);

					bt_ftp_compose_path((const U8 *) act_server_cntx_p->ftps_filepath,
							(const U8 *) act_server_cntx_p->ftps_obj_name,
							(U8 *) act_server_cntx_p->ftps_filepath,
							sizeof(act_server_cntx_p->ftps_filepath)/2);

					BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_SEND_WAIT);
					btmtk_ftps_ui_callback(BT_FTPSUI_PULL_FILE_START, (void *)0);

					// wait for invoking btmtk_ftps_response_pull_file

				} else {
					bt_ftp_compose_path(bt_ftps_get_current_folder((U8)i), ind->obj_name,
							path_buf, (U16)FMGR_MAX_PATH_LEN);

					ext_chset_ucs2_to_utf8_string(debug_path, sizeof(debug_path), path_buf);
					bt_ext_log("[BT][FTP] Start to pull a file: %s", debug_path);
					btmtk_ftps_ui_callback(BT_FTPSUI_PULL_FILE_START, (void *) debug_path);

					// check does the obj exist?
					// h = btmtk_goep_open_rstream((U16*) path_buf, EXT_FS_READ_ONLY);
					// if (FALSE == ext_fs_is_valid_handle_ucs2(h)) {
					(act_server_cntx_p + i)->fh_ftps_send = btmtk_goep_open_rstream((U16*) path_buf, EXT_FS_READ_ONLY);
					if (FALSE == ext_fs_is_valid_handle_ucs2((act_server_cntx_p + i)->fh_ftps_send)) {
						//FTP_DEBUG(kal_print("FTPS PULL IND HDLR btmtk_goep_open_rstream ERROR\n");)
						GOEP_Report("[ftps] fail to open rstream file");
						l_rsp_code = GOEP_NOT_FOUND;
						goto FTPS_PULL_IND_ERR;

					} else {
						if (TRUE == ext_nbuffer_isInit(&(act_server_cntx_p+i)->ftps_mempool)) {
							ext_nbuffer_cleanup(&(act_server_cntx_p+i)->ftps_mempool);
							(act_server_cntx_p + i)->ftps_fs_status = EXT_FS_OK;
							// (act_server_cntx_p + i)->fh_ftps_send = h;
							bt_ftp_send_msg_toself(MSG_ID_BT_FTPS_SERVER_INTERNAL_RW, NULL, 0);
						}
					}

					BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_SENDING);
					// save the pull pkg type to context. if it is the last one, show warning msg
					act_server_cntx_p->last_pkttype = ind->pkt_type;
						
					ext_ucs2ncpy((S8 *) (act_server_cntx_p + i)->ftps_filepath, 
							(const S8 *) path_buf, sizeof((act_server_cntx_p + i)->ftps_filepath));

					bt_ftps_send_obj_routine(GOEP_STATUS_SUCCESS, (act_server_cntx_p + i)->ftps_filepath, &bMore);
					(act_server_cntx_p + i)->send_obj_type = BT_FTPS_OBJ_TYPE_FILE;
					btmtk_ftps_ui_callback(BT_FTPSUI_PULLING, (void *)0 );
				}

				/*
				GOEP_Report("[ftps] state=%d objtype=%d \n", 
						(act_server_cntx_p + i)->ftps_state,
						(act_server_cntx_p + i)->send_obj_type);
				*/

				// pull a folder content
			} else {
				GOEP_Report("[ftps] pull_ind pull folder-listing request");
				// create a current folder's content
				ext_ucs2ncpy((S8 *)path_buf, (const S8 *) bt_ftps_get_current_folder((U8)i), (U16)FMGR_MAX_PATH_LEN);

				// fail-safe for enough memory to store the obj_name + obj_path (2 = '\','\0')
				if (ext_ucs2strlen((const S8 *) path_buf) + ext_ucs2strlen((const S8 *)ind->obj_name) > 
						(FMGR_MAX_PATH_LEN - 2)) {
					//FTP_DEBUG(kal_print("FTPS PULL IND HDLR btmtk_goep_open_rstream ERROR\n");)
					GOEP_Report("[ftps] FTPS PULL IND HDLR btmtk_goep_open_rstream ERROR");
					l_rsp_code = GOEP_NOT_ACCEPTABLE;
					goto FTPS_PULL_IND_ERR;
				}

				// check if there is obj name carried by pull ind
				if (ext_ucs2strlen((S8 *) ind->obj_name) > 0) {
					ext_ucs2ncat((S8 *) path_buf, (const S8 *)ind->obj_name, FMGR_MAX_PATH_LEN);
					ext_ucs2ncat((S8 *) path_buf, (const S8 *) FTP_FS_PATH_DELIMITOR, FMGR_MAX_PATH_LEN);
				}

				// to write folder content
				BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_SEND_WAIT);
				
				// goto FTPS_PULL_IND_CONTINUE_WRITE_FOLDER;
				if (FALSE == bt_ftps_is_auto(BT_FTP_AUTO_BROWSE)) {
					// save virtual folder path to new_folder
					bt_ftp_compose_path((act_server_cntx_p->current_folder), ind->obj_name,
							act_server_cntx_p->new_folder, sizeof(act_server_cntx_p->new_folder) /2);

					bt_ftp_compose_path((const U8 *) ftps_cntx_p->root_folder,
							(const U8 *) act_server_cntx_p->new_folder,
							(U8 *) act_server_cntx_p->ftps_filepath,
							sizeof(act_server_cntx_p->ftps_filepath)/2);

					ext_ucs2ncpy((S8 *)act_server_cntx_p->new_folder,
							(const S8*)act_server_cntx_p->current_folder,
							sizeof(act_server_cntx_p->new_folder)/2);                    
			
					ext_ucs2ncpy((S8 *) act_server_cntx_p->ftps_obj_name, (const S8 *) ind->obj_name,
							sizeof(act_server_cntx_p->ftps_obj_name)/2);

					if (0 ==  ext_ucs2strlen((S8 *)act_server_cntx_p->new_folder)) {
						// copy "/" to it
						ext_ucs2ncpy((S8 *) act_server_cntx_p->new_folder,
								(const S8 *) FTP_FS_PATH_DELIMITOR,
								sizeof(act_server_cntx_p->new_folder)/2);
					}

					// new_folder is the target    
					btmtk_ftps_ui_callback(BT_FTPSUI_BROWSE_START, NULL); 
					// wait for call btmtk_ftps_response_browsing_folder
				} else {
					// without carring folder name -> get current folder
					// check does the folder exist? If not, create the folder

					ext_ucs2_str_n_to_asc_str((S8 *) debug_path, (S8 *) path_buf, sizeof(debug_path));
					bt_ext_log("[ftps] Auto pull folder-listing, path: %s", debug_path);

					if (FALSE == btmtk_goep_is_folder_exist((const U16 *)path_buf)) {
						goto FTPS_PULL_IND_ERR;
					}

					BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_SENDING);
					if (FALSE != btmtk_ftps_get_xml_filepath(0, xmlpath, FMGR_MAX_PATH_LEN * ENCODING_LENGTH)) {
						btmtk_ftps_ui_callback(BT_FTPSUI_BROWSE_START, NULL); 

						if (EXT_FS_OK == bt_ftps_gen_folder_content(&(act_server_cntx_p + i)->xml_composer,
								(BT_BOOL)((act_server_cntx_p + i)-> ftps_browse_depth > 0 ? FALSE : TRUE),
								path_buf, xmlpath, 0, NULL)) {
							// update the context
							ext_ucs2ncpy((S8 *) act_server_cntx_p->ftps_filepath, (const S8 *) xmlpath, FMGR_MAX_PATH_LEN);

							bt_ftps_send_folder_content_routine(0, GOEP_STATUS_SUCCESS, act_server_cntx_p->ftps_filepath);

							(act_server_cntx_p + i)->send_obj_type = BT_FTPS_OBJ_TYPE_FOLDER;
							btmtk_ftps_ui_callback(BT_FTPSUI_BROWSING,
									(void *) (act_server_cntx_p->total_send_obj_len -
									act_server_cntx_p->remain_send_obj_len)); // start to browse
						} else {
							GOEP_Report("[ftps] state=%d objtype=%d \n", (act_server_cntx_p + i)->ftps_state,
									(act_server_cntx_p + i)->send_obj_type );
							goto FTPS_PULL_IND_ERR;
						}
					} else {
						GOEP_Report("[ftps] state=%d objtype=%d \n", (act_server_cntx_p + i)->ftps_state,
								(act_server_cntx_p + i)->send_obj_type );
						goto FTPS_PULL_IND_ERR;
					}
				} // if auto               
			}
			return;

		// continue
		case BT_FTPS_STATE_SENDING: 
			// can't support simultaneously sending
			if ((ind->pkt_type == GOEP_FIRST_PKT) || (ind->pkt_type == GOEP_SINGLE_PKT)) {
				//FTP_DEBUG(kal_print("FTPS PULL IND WITH WRONG PKT_TYPE(FIRST->FIRST)\n");)              
				BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);

				// send pull res to goep with error rsp_code
				GOEP_Report("[BT][FTP] state:sending receive a first pkt type:%d", ind->pkt_type);
				goto FTPS_PULL_IND_ERR;

			} else { 
				// not another first req
				bt_ftps_obj_type_enum previous_type;
				bMore = FALSE;
				previous_type = (act_server_cntx_p + i)->send_obj_type;

				if ((act_server_cntx_p + i)->flag_abort_req_sent == TRUE) {
					GOEP_Report("[BT][FTP] This sending has been aborted.");
					l_rsp_code = GOEP_FORBIDDEN;
					goto FTPS_PULL_IND_ERR;
				}

				// send the left pieces of file from send file handle
				bt_ftps_pull_continue_routine((U8)i, &bMore); 

				if (FALSE == bMore) {
					if (BT_FTPS_OBJ_TYPE_FOLDER == previous_type) {
						btmtk_ftps_ui_callback(BT_FTPSUI_BROWSED, (void *) GOEP_STATUS_SUCCESS);
					} else {
						btmtk_ftps_ui_callback(BT_FTPSUI_PULLED, (void *) GOEP_STATUS_SUCCESS);
					}

				} else {
					if (BT_FTPS_OBJ_TYPE_FOLDER == (act_server_cntx_p + i)->send_obj_type) {
						btmtk_ftps_ui_callback(BT_FTPSUI_BROWSING, 
						(void *) (act_server_cntx_p->total_send_obj_len - act_server_cntx_p->remain_send_obj_len));

					} else {
						btmtk_ftps_ui_callback(BT_FTPSUI_PULLING,
						(void *) (act_server_cntx_p->total_send_obj_len - act_server_cntx_p->remain_send_obj_len));
					}
				}

				/*
				GOEP_Report("[ftps] state=%d objtype=%d \n",
						(act_server_cntx_p + i)->ftps_state,
						(act_server_cntx_p + i)->send_obj_type );
				*/
				return;
			}

		case BT_FTPS_STATE_RECEIVING:  /* wrong sequence msg incoming (error handling) */
			//// BT_FTPS_LOG_STATE_ERR(i);
			BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);

			// send pull res to goep with error rsp_code
			goto FTPS_PULL_IND_ERR;
			// discard this message
		case BT_FTPS_STATE_DEACTIVATING:
		case BT_FTPS_STATE_DISCONNECTING:
			return;
		default:    // abnormal state
			//// BT_FTPS_LOG_STATE_ERR(i);
			GOEP_Report("[ftps][err] wrong state:(%d) NO handle! ", act_server_cntx_p->ftps_state);
			l_rsp_code = GOEP_INTERNAL_SERVER_ERR;
			goto FTPS_PULL_IND_ERR;
			return;
	}


FTPS_PULL_IND_ERR:

	//FTP_DEBUG(kal_print("FTPS PULL IND ERR (GOTO)\n");)
	if (FALSE != ext_fs_is_valid_handle_ucs2(act_server_cntx_p->fh_ftps_send)) {
		btmtk_goep_close_rstream(act_server_cntx_p->fh_ftps_send);
		(act_server_cntx_p + i)->total_send_obj_len = 0;
		(act_server_cntx_p + i)->remain_send_obj_len = 0;
		(act_server_cntx_p + i)->fh_ftps_send = FHANDLE_INVALID_VALUE;
		act_server_cntx_p->fh_ftps_send = FHANDLE_INVALID_VALUE;
	}

	if ((act_server_cntx_p + i)->flag_abort_req_sent) {
		(act_server_cntx_p + i)->flag_abort_req_sent = FALSE;
	}

	// Construct goep pull res ilm
	res = (goep_pull_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_pull_rsp_struct),TD_CTRL);
	// memory init
	GOEP_MEMSET(res, 0, sizeof(goep_pull_rsp_struct));
	res->goep_conn_id = ftps_cntx_p->goep_conn_id[i];
	res->rsp_code = l_rsp_code;

	// Send MSG_ID_BT_FTPS_PULL_RSP primitive
	bt_ftp_send_msg(MSG_ID_BT_FTPS_PULL_RSP, res , sizeof(goep_pull_rsp_struct));
	BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
	btmtk_ftps_ui_callback(BT_FTPSUI_BROWSED, (void *) (U32) l_rsp_code);

	return;
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_ftps_set_folder_ind_handler
 * DESCRIPTION
 *  This function is to handle server SET_FOLDER_IND msg
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftps_set_folder_ind_handler(void *msg)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	S32 l_rspCode;
	U8 ucNewFolderPath[FMGR_MAX_PATH_LEN * ENCODING_LENGTH];
	U8 ucParentPath[FMGR_MAX_PATH_LEN * ENCODING_LENGTH];
	goep_set_folder_ind_struct *ind = (goep_set_folder_ind_struct*) msg;

	U8 debug_path[FMGR_MAX_PATH_LEN];

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	// find out the selected index
	i = bt_ftps_find_goep_id(ind->goep_conn_id);
	l_rspCode = GOEP_INTERNAL_SERVER_ERR; //default error code

	// reset
	if (i == 0xff) {
		return;
	}

	GOEP_MEMSET(ucNewFolderPath, 0 ,FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
	GOEP_MEMSET(ucParentPath, 0, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_SET_FOLDER_IND_HDLR,
	(act_server_cntx_p + i)->ftps_state, ind->setpath_flag));

	// reverse the ind->folder_name
	if (ext_ucs2strlen((S8 *) ind->folder_name) > 0) {
		bt_ftp_obj_name_to_host_order(ind->folder_name, ind->folder_name);
	}

	// check state for different resetting
	switch ((act_server_cntx_p + i)->ftps_state)
	{
		// normal case
		case BT_FTPS_STATE_CONNECTED:
			// name, back, root, create
			if (ind->setpath_flag == GOEP_ROOT_FOLDER) {

				if (FALSE == bt_ftps_is_auto(BT_FTP_AUTO_SETPATH)) {
					// save "
					ext_ucs2ncpy((S8 *)(act_server_cntx_p)->new_folder,
							(const S8*)FTP_FS_PATH_DELIMITOR,
							sizeof((act_server_cntx_p)->new_folder));

					BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_SETPATH_WAIT);
					btmtk_ftps_ui_callback(BT_FTPSUI_SET_FOLDER_START, 0);

				} else {
					(act_server_cntx_p + i)->ftps_browse_depth = 0;
					GOEP_MEMSET((act_server_cntx_p + i)->current_folder, 0, FMGR_MAX_PATH_LEN);
					// realloc the root_folder
					if (FALSE == btmtk_get_default_root_folder_path(ftps_cntx_p->root_folder, FMGR_MAX_PATH_LEN)) {
						goto FTPS_SET_FOLDER_SUCCESS;
					}
					goto FTPS_SET_FOLDER_SUCCESS;
				}

			} else if (ind->setpath_flag == GOEP_BACK_FOLDER) {
				// call set parent folder routine
				btmtk_ftp_util_get_parent_folder(act_server_cntx_p->current_folder, 
						act_server_cntx_p->new_folder, 
						sizeof(act_server_cntx_p->new_folder)/2);

				if (0 == ext_ucs2strlen((S8 *) act_server_cntx_p->new_folder)) {
					ext_ucs2ncpy((S8 *)act_server_cntx_p->new_folder,
							(const S8 *)FTP_FS_PATH_DELIMITOR,
							sizeof(act_server_cntx_p->new_folder)/2);
				}

				// For debug
				ext_ucs2_str_n_to_asc_str((S8 *) debug_path, (S8 *) act_server_cntx_p->new_folder,
						sizeof(debug_path));
				bt_ext_log("[BT][FTP] ucNewFolderPath: %s", debug_path);

				if (FALSE == bt_ftps_is_auto(BT_FTP_AUTO_SETPATH) ) {
					BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_SETPATH_WAIT);
					btmtk_ftps_ui_callback(BT_FTPSUI_SET_FOLDER_START, 0);

				} else {
					bt_ftp_compose_path(ftps_cntx_p->root_folder, 
							act_server_cntx_p->new_folder,
							ucNewFolderPath,
							sizeof(ucNewFolderPath)/2);

					if (FALSE == btmtk_goep_is_folder_exist((const U16 *)ucNewFolderPath) ||
						((act_server_cntx_p+i)->ftps_browse_depth == 0)) {
						goto FTPS_SET_FOLDER_ERR;
					}

					(act_server_cntx_p+i)->ftps_browse_depth--;
					
					btmtk_ftps_ui_callback(BT_FTPSUI_SET_FOLDERED, NULL);
					ext_ucs2ncpy((S8 *) act_server_cntx_p->current_folder,
							(const S8 *) act_server_cntx_p->new_folder,
							sizeof(act_server_cntx_p->current_folder)/2);
					goto FTPS_SET_FOLDER_SUCCESS;
				}
				return;

			} else if (ind->setpath_flag == GOEP_FORWARD_FOLDER) {
				// call set folder routine
				GOEP_MEMSET(ucParentPath, 0, sizeof(ucParentPath));

				if (0 == ext_ucs2strlen((S8 *) ind->folder_name)) {
					// TODO
					goto FTPS_SET_FOLDER_SUCCESS;
				}

				// Compose the new current folder
				bt_ftp_compose_path(act_server_cntx_p->current_folder,
						ind->folder_name,
						act_server_cntx_p->new_folder,
						sizeof(act_server_cntx_p->new_folder)/2);

				// Compose the complete path of new folder into "ucNewFolderPath"
				// for further error checking.

				bt_ftp_compose_path(ftps_cntx_p->root_folder,
						act_server_cntx_p->new_folder,
						ucNewFolderPath,
						sizeof(ucNewFolderPath)/2);

				// For debug
				ext_ucs2_str_n_to_asc_str((S8 *) debug_path, (S8 *) ucNewFolderPath, sizeof(debug_path));
				bt_ext_log("[BT][FTP] Setpath(Forward), ucNewFolderPath: %s", debug_path);

				if (FALSE == bt_ftps_is_auto(BT_FTP_AUTO_SETPATH)) {
					BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_SETPATH_WAIT);
					btmtk_ftps_ui_callback( BT_FTPSUI_SET_FOLDER_START,0);

				} else {
					if (0 == ext_ucs2strlen((S8 *) ucNewFolderPath)) {
						bt_ext_err("[BT][FTP] Error: Setpath failed. An empty path.");
						l_rspCode = GOEP_NOT_FOUND;
						goto FTPS_SET_FOLDER_ERR;
					}

					/* PTS
					if (FALSE == btmtk_ftps_is_allow_create_folder((const U16*) ucNewFolderPath)) {
						l_rspCode = GOEP_FORBIDDEN;
						goto FTPS_SET_FOLDER_ERR;                        
					}
					*/
					
					// Check if the folder exists or not
					if (FALSE == btmtk_goep_is_folder_exist((const U16*) ucNewFolderPath)) {
						bt_ext_err("[BT][FTP] Error: folder not found.");
						// TC_SEVER_PBR_BV_06_I PTS
						l_rspCode = GOEP_NOT_FOUND;
						goto FTPS_SET_FOLDER_ERR; 
					}
					
					(act_server_cntx_p+i)->ftps_browse_depth++;
					ext_ucs2ncpy((S8 *) act_server_cntx_p->current_folder,
							(const S8 *) act_server_cntx_p->new_folder,
							sizeof(act_server_cntx_p->current_folder)/2);
					
					goto FTPS_SET_FOLDER_SUCCESS;
				}
				return;

			} else {
				// GOEP_CREATE_FOLDER
				// call create folder routine
				// create folder and set current to new folder (should send res)
				bt_ftp_compose_path(act_server_cntx_p->current_folder,
						ind->folder_name,
						act_server_cntx_p->new_folder,
						sizeof(act_server_cntx_p->new_folder)/2);

				if (FALSE != btmtk_ftps_is_read_only()) {
					l_rspCode = GOEP_UNAUTHORIZED;
					goto FTPS_SET_FOLDER_ERR;
				}
				
				if (FALSE == bt_ftps_is_auto(BT_FTP_AUTO_CREATE_FOLDER)) {
					/// new virtual path is put on new_folder attribute
					// ftps path: real absolute path
					// new_folder: current virtual folderpath
					// obj_name: new folder name
					bt_ftp_compose_path(ftps_cntx_p->root_folder,
							act_server_cntx_p->current_folder,
							act_server_cntx_p->new_folder,
							sizeof(act_server_cntx_p->new_folder)/2);

					bt_ftp_compose_path(act_server_cntx_p->new_folder,
							ind->folder_name,
							act_server_cntx_p->ftps_filepath,
							sizeof(act_server_cntx_p->ftps_filepath)/2);
			
					ext_ucs2ncpy((S8 *) act_server_cntx_p->new_folder,
							(const S8 *) act_server_cntx_p->current_folder,
							sizeof(act_server_cntx_p->new_folder)/2);

					ext_ucs2ncpy((S8 *) act_server_cntx_p->ftps_obj_name,
							(const S8 *) ind->folder_name,
							sizeof(act_server_cntx_p->ftps_obj_name)/2);
					
					BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_SETPATH_WAIT);
					btmtk_ftps_ui_callback(BT_FTPSUI_FOLDER_CREAT_START, 0);
					
				} else {
					if (0 == ext_ucs2strlen((S8 *) ind->folder_name)) {
						goto FTPS_SET_FOLDER_ERR;
					}
					GOEP_MEMSET( ucNewFolderPath, 0, sizeof(ucNewFolderPath));
					GOEP_MEMSET( ucParentPath, 0, sizeof(ucParentPath));

					bt_ftp_compose_path(act_server_cntx_p->current_folder,
							ind->folder_name,
							ucParentPath,
							sizeof(ucParentPath)/2);

					bt_ftp_compose_path(ftps_cntx_p->root_folder,
							ucParentPath,
							ucNewFolderPath,
							sizeof(ucNewFolderPath)/2);

					if (0 == ext_ucs2strlen((S8 *) ucNewFolderPath) || 0 == ext_ucs2strlen((S8 *) ucParentPath)) {
						l_rspCode = GOEP_NOT_FOUND;
						goto FTPS_SET_FOLDER_ERR;
					}

					if (FALSE == btmtk_ftps_is_allow_create_folder((const U16 *)ucNewFolderPath)) {
						l_rspCode = GOEP_FORBIDDEN;
						goto FTPS_SET_FOLDER_ERR;
					}
					
					if (EXT_FS_OK != btmtk_goep_create_folder((const U16 *)ucNewFolderPath)) {
						bt_ext_err("[BT][FTP] Error: Failed to create the folder.");
						l_rspCode = GOEP_INTERNAL_SERVER_ERR;
						goto FTPS_SET_FOLDER_ERR;
					}

					if (FALSE == btmtk_goep_is_folder_exist( (const U16 *)ucNewFolderPath)) {
						bt_ext_err("[BT][FTP] Error: Folder doesn't exist after creation");
						l_rspCode = GOEP_INTERNAL_SERVER_ERR;
						goto FTPS_SET_FOLDER_ERR;                    
					}                

					// update the current folder (create it and enter it )
					(act_server_cntx_p+i)->ftps_browse_depth++;
					ext_ucs2ncpy((S8 *) act_server_cntx_p->current_folder,
							(const S8 *) ucParentPath,
							sizeof(act_server_cntx_p->current_folder)/2);
					
					goto FTPS_SET_FOLDER_SUCCESS; 
					return;
				}
			}

		// ignore conditions
		case BT_FTPS_STATE_DEACTIVATING:
		case BT_FTPS_STATE_DISCONNECTING:
			// do nothing
			return;

		// error recovery
		case BT_FTPS_STATE_SENDING:
			if ((act_server_cntx_p + i)->fh_ftps_send) {
				btmtk_goep_close_rstream(act_server_cntx_p->fh_ftps_send);
				(act_server_cntx_p + i)->total_send_obj_len = 0;
				(act_server_cntx_p + i)->remain_send_obj_len = 0;
				(act_server_cntx_p + i)->fh_ftps_send = FHANDLE_INVALID_VALUE;
			}

			if ((act_server_cntx_p + i)->send_obj_type == BT_FTPS_OBJ_TYPE_FOLDER) {
				// FTP_DEBUG(kal_print("FTPS SET FOLDER IND HDLR DEL FOLDER CONTENT FILE\n");)
				// delete the folder content file
				btmtk_goep_delete_folder((const U16*)((act_server_cntx_p + i)->ftps_filepath));
			}

			(act_server_cntx_p + i)->send_obj_type = BT_FTPS_OBJ_TYPE_NONE;
			// send res with error rsp_code and go back to accepted state
			goto FTPS_SET_FOLDER_ERR;
			break;

		case BT_FTPS_STATE_RECEIVING:
			if ((act_server_cntx_p + i)->fh_ftps_recv) {
				btmtk_goep_close_wstream(act_server_cntx_p->fh_ftps_recv);
				(act_server_cntx_p + i)->fh_ftps_recv = FHANDLE_INVALID_VALUE;
			}

			// send res with error rsp_code and go back to accepted state
			goto FTPS_SET_FOLDER_ERR;
			break;
		// abnormal case (no connection yet -- wrong state)
		default:
			// BT_FTPS_LOG_STATE_ERR(i);
			// BT_FTP_ASSERT(0);
			return;
		}

FTPS_SET_FOLDER_SUCCESS:
	// FTP_DEBUG(kal_print("FTPS SET FOLDER SUCCESS (GOTO)\n");)
	if (FALSE != bt_ftps_is_auto(BT_FTP_AUTO_SETPATH)) {

		// [DEBUG] Printing message for debug.
		ext_ucs2_str_n_to_asc_str((S8 *) debug_path, (S8 *) act_server_cntx_p->current_folder, sizeof(debug_path));
		bt_ext_log("[BT][FTP] After set-folder success, current path: %s", debug_path);

		btmtk_ftps_ui_callback(BT_FTPSUI_SET_FOLDERED, (void *)GOEP_STATUS_SUCCESS); 
	}

	bt_ftps_send_set_folder_rsp(0, (U8)GOEP_STATUS_SUCCESS);
	return;

FTPS_SET_FOLDER_ERR:
	// FTP_DEBUG(kal_print("FTPS SET FOLDER ERR (GOTO)\n");)

	// Construct goep set folder res ilm
	bt_ftps_send_set_folder_rsp(0, (U8)l_rspCode );
	BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);
	btmtk_ftps_ui_callback(BT_FTPSUI_SET_FOLDERED, (void *)l_rspCode);
	return;
}


void bt_ftps_send_set_folder_rsp(U8 index, U8 ucRspCode){
	goep_set_folder_rsp_struct *res;

	// Construct goep set folder res ilm
	res = (goep_set_folder_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_set_folder_rsp_struct),TD_CTRL);

	// memory init
	GOEP_MEMSET( res, 0, sizeof(goep_set_folder_rsp_struct));
	res->goep_conn_id = ftps_cntx_p->goep_conn_id[index];
	res->rsp_code = ucRspCode; //GOEP_STATUS_SUCCESS;

	// Send MSG_ID_BT_FTPS_SET_FOLDER_RSP primitive
	bt_ftp_send_msg(MSG_ID_BT_FTPS_SET_FOLDER_RSP, res , sizeof(goep_set_folder_rsp_struct) );
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_ftps_abort_ind_handler
 * DESCRIPTION
 *  This function is to handle server ABORT_IND msg
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftps_abort_ind_handler(void *msg)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	goep_abort_ind_struct *ind = (goep_abort_ind_struct*) msg;
	goep_abort_rsp_struct *res;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	/* find the index of aborting goep connection */
	i = bt_ftps_find_goep_id(ind->goep_conn_id);

	// reset
	if (i == 0xff) {
		return;
	}

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_ABORT_IND_HDLR, (act_server_cntx_p + i)->ftps_state));
	btmtk_ftps_ui_callback(BT_FTPSUI_ABORTED, NULL);

	// check state for different resetting
	switch ((act_server_cntx_p + i)->ftps_state) {
		case BT_FTPS_STATE_SENDING:
			if ((act_server_cntx_p + i)->fh_ftps_send) {
				btmtk_goep_close_rstream(act_server_cntx_p->fh_ftps_send);
				(act_server_cntx_p + i)->total_send_obj_len = 0;
				(act_server_cntx_p + i)->remain_send_obj_len = 0;
				(act_server_cntx_p + i)->fh_ftps_send = FHANDLE_INVALID_VALUE;
			}

			if ((act_server_cntx_p + i)->send_obj_type == BT_FTPS_OBJ_TYPE_FOLDER) {
				 //FTP_DEBUG(kal_print("FTPS ABORT IND HDLR DEL FOLDER CONTENT FILE\n");)
				 // delete the folder content file
				 btmtk_ftps_ui_callback(BT_FTPSUI_BROWSED, (void *) GOEP_USER_CANCEL); //remote abor   
			} else {
				 btmtk_ftps_ui_callback(BT_FTPSUI_PULLED, (void *) GOEP_USER_CANCEL); //remote abor
			}

			(act_server_cntx_p + i)->send_obj_type = BT_FTPS_OBJ_TYPE_NONE;
			break;

		case BT_FTPS_STATE_RECEIVING:
			if ((act_server_cntx_p + i)->fh_ftps_recv) {
				// close file handle, and delete temp downloading obj
				btmtk_goep_close_wstream(act_server_cntx_p->fh_ftps_recv);
				(act_server_cntx_p + i)->fh_ftps_recv = FHANDLE_INVALID_VALUE;

				btmtk_goep_delete_file((U16 *)(act_server_cntx_p +i)->ftps_filepath);
			}
			
			//if (TRUE == (act_server_cntx_p + i)->dis_from_user)
			{
				btmtk_ftps_ui_callback(BT_FTPSUI_PUSHED, (void *) GOEP_USER_CANCEL); //remote abor
			}
			break;

		// ignore conditions (for race condition)
		case BT_FTPS_STATE_CONNECTED:
			break;
		case BT_FTPS_STATE_DEACTIVATING:
		case BT_FTPS_STATE_DISCONNECTING:
			return;
		// error recovery
		default:
			//// BT_FTPS_LOG_STATE_ERR(i);
			BT_FTP_ASSERT(0);
			break;
	}

	BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);

	// film res pkt_type attibute
	res = (goep_abort_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_abort_rsp_struct),TD_CTRL);

	// memory init
	GOEP_MEMSET(res, 0, sizeof(goep_abort_rsp_struct));
	res->goep_conn_id = ind->goep_conn_id;
	res->rsp_code = GOEP_STATUS_SUCCESS;

	// send abort res to goep
	bt_ftp_send_msg(MSG_ID_BT_FTPS_ABORT_RSP, res , sizeof(goep_abort_rsp_struct));
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_ftps_abort_ind_handler
 * DESCRIPTION
 *  This function is to handle server ABORT_IND msg
 * PARAMETERS
 *  msg     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftps_action_ind_handler(void *msg) {
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 i;
    goep_action_ind_struct *ind = (goep_action_ind_struct*) msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* find the index of aborting goep connection */
    i = bt_ftps_find_goep_id(ind->goep_conn_id);

    /* reset */
    if (i == 0xff) {
        return;
    }
    BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_ACTION_WAIT);
    OS_Report("[FTPS] receive a action ");
    btmtk_ftps_ui_callback(BT_FTPSUI_ACTION_START, (void *) GOEP_STATUS_SUCCESS);
}

void btmtk_ftps_action_response(U8 rsp_code) {
    U32 i = 0;
    goep_action_rsp_struct *res;

    switch ((act_server_cntx_p)->ftps_state) {
    case BT_FTPS_STATE_ACTION_WAIT:
        BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_CONNECTED);

        /* film res pkt_type attibute */
        res = (goep_action_rsp_struct*) CONSTRUCT_LOCAL_PARAM(
                sizeof(goep_action_rsp_struct), TD_CTRL);
        /* memory init */
        GOEP_MEMSET(res, 0, sizeof(goep_action_rsp_struct));
        res->goep_conn_id = ftps_cntx_p->goep_conn_id[0];
        res->rsp_code = GOEP_STATUS_SUCCESS;
        res->reason = 0;
        res->uuid_len = sizeof(OBEX_FTP_UUID);
        GOEP_MEMCPY((void *) res->uuid, (void *) OBEX_FTP_UUID,
                sizeof(OBEX_FTP_UUID));

        /* send abort res to goep */
        bt_ftp_send_msg(MSG_ID_BT_FTPS_ACTION_RSP, res,
                sizeof(goep_action_rsp_struct));
        break;
    default:
        break;
    }
}

/*****************************************************************************
* @brief This function is to handle server AUTHORIZE_IND msg
*  btmtk_ftps_authorize_ind_handler
*  if auto response the author ind, UI should not receive the BT_FTPSUI_AUTHORIZING event
*  Once MMI receives the BT_FTPSUI_AUTHORIZING, it should invoke
* @see
**/
void btmtk_ftps_authorize_ind_handler(void *msg)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	goep_authorize_ind_struct *ind = (goep_authorize_ind_struct*) msg;
	goep_authorize_rsp_struct *res = NULL;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	i = bt_ftps_find_goep_id(ind->goep_conn_id);

	/* reset */
	if (i == 0xff)
	{
		return;
	}

	bt_ext_log("[BT][FTP] Authorize requesting device: %s", ind->dev_name);

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_AUTHORIZE_IND_HDLR, (act_server_cntx_p + i)->ftps_state));

	// for MMI knows the authorizing one
	ftps_cntx_p->cur_goep_conn_id = ind->goep_conn_id;
	ftps_cntx_p->goep_conn_id[i] = ind->goep_conn_id;
	(act_server_cntx_p + i)->dis_from_user = FALSE;

	switch ((act_server_cntx_p + i)->ftps_state)
	{
		case BT_FTPS_STATE_ACTIVE: // correct state to accept the authorize ind
			GOEP_MEMSET(&((act_server_cntx_p + i)->ftps_bt_device), 0, sizeof(goep_bd_addr_struct));
			GOEP_MEMSET((act_server_cntx_p + i)->dev_name, 0, GOEP_MAX_DEV_NAME);
			GOEP_MEMCPY(&((act_server_cntx_p + i)->ftps_bt_device), &(ind->bd_addr), sizeof(goep_bd_addr_struct));
			GOEP_MEMSET(&((act_server_cntx_p + i)->bdaddr), 0, sizeof(goep_bd_addr_struct));
			GOEP_MEMCPY(&((act_server_cntx_p + i)->bdaddr), &(ind->bd_addr), sizeof(goep_bd_addr_struct));

			BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_AUTHORIZING);

			// wait for btmtk_ftps_authorize_accept
			if (FALSE == bt_ftps_is_auto(BT_FTP_AUTO_AUTHORIZATION)) {
				btmtk_ftps_ui_callback(BT_FTPSUI_AUTHORIZING, (void*) &(ind->bd_addr));
			} else {
				// auto accept it
				GOEP_Report("[FTPS] auto authroize rsp ");
				goto FTPS_AUTHROIZE_RSP_SEND;
			}

			return;
			break;
		default:    // wrong state
			/// BT_FTPS_LOG_STATE_ERR(i);
			/// BT_FTP_ASSERT(0);
			return;
	}
	return;

FTPS_AUTHROIZE_RSP_SEND:

	// Construct goep set folder res ilm
	BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_ACTIVE);

	res = (goep_authorize_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_authorize_rsp_struct),TD_CTRL);
	GOEP_MEMSET( res, 0, sizeof(goep_authorize_rsp_struct));
	res->goep_conn_id = ftps_cntx_p->goep_conn_id[0];
	res->rsp_code = GOEP_STATUS_SUCCESS;
	bt_ftp_send_msg(MSG_ID_BT_FTPS_AUTHORIZE_RSP, res, sizeof(goep_authorize_rsp_struct) );

	return;
}


void btmtk_ftps_disconnect_ind_handler(void *data){
  
	U32 i;
	goep_disconnect_ind_struct *ind = (goep_disconnect_ind_struct*) data;
	goep_deregister_server_req_struct *req;

	GOEP_Report("[BT][FTP] disconnect_ind_handler goep_id:%d reason:%d req_id:%d",
			ind->goep_conn_id, ind->reason, ind->req_id);

	// identify ftps conection or ftpc connection
	i = bt_ftps_find_goep_id(ind->goep_conn_id);
	if (0xFF == i) {
		// error! cannot find connection context
		btmtk_ftps_ui_callback(BT_FTPSUI_ERROR, 0);
		return;
	}

	switch ((act_server_cntx_p + i)->ftps_state) {
		case BT_FTPS_STATE_AUTHORIZING:
			//bt_ftps_notify_disconnected();
			BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_ACTIVE);        // state -> active

			// FTP_DEBUG(kal_print("DISCONNECT IND STATE DISCONNECTING\n");)
			/* 
			mmi_bth_disconnect_ind_hdler((act_server_cntx_p + i)->ftps_bt_device.lap,
					(act_server_cntx_p + i)->ftps_bt_device.uap,
					(act_server_cntx_p + i)->ftps_bt_device.nap,
					(U32) BT_BTH_OBEX_FILE_TRANSFER_UUID,
					(U32) ftps_cntx_p->g_conn_id[i]);
			*/
			// call CM

			// reset context
			bt_ftps_connection_terminated(i, ind->reason);
			return;

		case BT_FTPS_STATE_DISCONNECTING:
			// FTP_DEBUG(kal_print("DISCONNECT IND STATE DISCONNECTING\n");)
			BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_ACTIVE);        /* state -> active */
			// call CM
			if(act_server_cntx_p->dis_from_user  == FALSE)
			{
				// UI should control this              
			}
			// reset context
			bt_ftps_connection_terminated(i, ind->reason);
			return;

		case BT_FTPS_STATE_DEACTIVATING:
			// FTP_DEBUG(kal_print("DISCONNECT IND STATE DEACTIVATING\n");)
			// Construct goep deregister req ilm

			req = (goep_deregister_server_req_struct*) 
			CONSTRUCT_LOCAL_PARAM(sizeof(goep_deregister_server_req_struct),TD_CTRL);

			// memory init
			GOEP_MEMSET(req, 0, sizeof(goep_deregister_server_req_struct));
			req->goep_conn_id = ftps_cntx_p->goep_conn_id[i];

			// Send MSG_ID_BT_FTPS_DEREGISTER_SERVER_REQ primitive
			bt_ftp_send_msg(MSG_ID_BT_FTPS_DEREGISTER_SERVER_REQ,
			req,
			sizeof(goep_deregister_server_req_struct));

			bt_ftps_connection_terminated(i, ind->reason);
			return;

		// connected states
		case BT_FTPS_STATE_CONNECTED:
		case BT_FTPS_STATE_SEND_WAIT:
		case BT_FTPS_STATE_SETPATH_WAIT:
		case BT_FTPS_STATE_RECEIVE_WAIT:
		case BT_FTPS_STATE_SENDING:
		case BT_FTPS_STATE_RECEIVING:
			BT_FTPS_STATE_TRANS(i, BT_FTPS_STATE_ACTIVE);
			// call disconnect ind to notify CM
			if (ftps_cntx_p->g_conn_id[i] != 0xFF)
			{

			}

			// reset context
			bt_ftps_connection_terminated(i, ind->reason);
			return;
		case BT_FTPS_STATE_ACTIVE:
			// not connected yet
			// Consider following steps:
			// 1. Client: request to connect to server
			// 2. Server: raise the authorization request
			// 3. Client: cancel the connection request
			// 4. Server: accept the request from UI
			// The disconnection indication reaches external adaption layer after step4.
			// In this switch case, a UI event should be delivered out.
			bt_ftps_connection_terminated(i, ind->reason);
			break;
		default:
			// FTP_DEBUG(kal_print("DISCONNECT IND STATE ERROR (state: %d)\n",(act_server_cntx_p+i)->ftps_state);)
			//// BT_FTPS_LOG_STATE_ERR(i);
			BT_FTP_ASSERT(0);
			return;
	}
}


void btmtk_ftps_auth_ind_handler(void *data) {
	U32 u4Id = 0;
	goep_auth_ind_struct* goep_auth_ind;

	// send out auth_wait
	switch (act_server_cntx_p->ftps_state) {
		case BT_FTPSUI_READY:
		case BT_FTPSUI_AUTHORIZING:
			goep_auth_ind = (goep_auth_ind_struct *) data;
			BT_FTPS_STATE_TRANS(0, BT_FTPSUI_AUTHEN_WAIT);
			// fill up the options data
			act_server_cntx_p->challeng_options = goep_auth_ind->options;
			GOEP_MEMCPY( (U8 *)act_server_cntx_p->realm,  goep_auth_ind->realm, sizeof(act_server_cntx_p->realm));
			btmtk_ftps_ui_callback(BT_FTPSUI_AUTHEN_WAIT, 0);
			break;
		default:
			// log and send response fail back
			GOEP_Report("[FTPS][WRN] wrng state:(%d) receive auth_ind", act_server_cntx_p->ftps_state);
			if (ftps_cntx_p) {
				u4Id = ftps_cntx_p->goep_conn_id[0];
			}
			bt_ftps_send_auth_rsp_msg( (U8)u4Id, (const U8 *)"", (const U8 *)"", 0);
			break;
	}
	
}

void btmtk_ftps_internal_rw_handler(void *data) {

	U8 *pBuffer;
	U32 u4Read, u4BuffSize, u4Index;
	static U32 interval = 0;

	if( FALSE == ext_nbuffer_isInit(&act_server_cntx_p->ftps_mempool) ){
		return;
	}

	switch (act_server_cntx_p->ftps_state) {
		case BT_FTPS_STATE_RECEIVING:
			/// write nbuffer to file
			bt_ext_log("btmtk_ftps_internal_rw_handler(): RECEIVING");
			if (FALSE != ext_fs_is_valid_handle_ucs2(act_server_cntx_p->fh_ftps_recv)) {
				if (FALSE  == ext_nbuffer_isEmpty(&act_server_cntx_p->ftps_mempool)) {
					bt_ftps_flush_one_nbuffer_to_file();
					bt_ftp_send_msg_toself( MSG_ID_BT_FTPS_SERVER_INTERNAL_RW, NULL, 0);
				}
			}
			break;

		case BT_FTPS_STATE_SENDING:
			/// read nbuffer from file until nbuffer full
			bt_ext_log("[BT][FTP] btmtk_ftps_internal_rw_handler(): SENDING");
			if (FALSE != ext_fs_is_valid_handle_ucs2(act_server_cntx_p->fh_ftps_send)) {
				if (FALSE != ext_nbuffer_isInit(&act_server_cntx_p->ftps_mempool)) {
					if (FALSE  == ext_nbuffer_isFull(&act_server_cntx_p->ftps_mempool)) {
						// read data to a free buffer
						pBuffer = (U8 *) ext_nbuffer_get_tail_buffer(&act_server_cntx_p->ftps_mempool,
								&u4Index, (S32 *) &u4BuffSize);
						BT_FTP_ASSERT(NULL != pBuffer);
						BT_FTP_ASSERT(u4BuffSize >= act_server_cntx_p->send_obex_pkt_size);

						if (NULL != pBuffer) {
							act_server_cntx_p->ftps_fs_status = 
									btmtk_goep_read_rstream(act_server_cntx_p->fh_ftps_send,
									pBuffer, act_server_cntx_p->send_obex_pkt_size, (S32 *) &u4Read);

							if (u4Read > 0) {
								ext_nbuffer_return_tail_buffer( &act_server_cntx_p->ftps_mempool, u4Index, u4Read);
								bt_ftp_send_msg_toself(MSG_ID_BT_FTPS_SERVER_INTERNAL_RW, NULL, 0);

							} else if (0 == u4Read && EXT_FS_OK == act_server_cntx_p->ftps_fs_status) {
								// reach eof
								act_server_cntx_p->ftps_fs_status = EXT_FS_ERR_EOF;
							} else {
								btmtk_ftps_ui_callback(BT_FTPSUI_ERROR, (void *)__LINE__); //internal error
							}
							// send internal rw data
						}
					}
				}
			}
			break;

		default:
			break;
	}
}

/***************************** Server ************************/

void btmtk_ftps_auth_cnf_handler(void *data){
	// check the data
	goep_auth_cnf_struct *cnf;
	cnf = (goep_auth_cnf_struct *)data;
}

void btmtk_ftps_disconnect_cnf_handler(void *data) {
}

void btmtk_ftps_pushfile_ind_handler(void *msg) {
    bt_ftps_pushfile_ind_struct *ind;
    U8 *file = 0;
    U16 filepath[FMGR_MAX_PATH_LEN];
    U16 path_buf[FMGR_MAX_PATH_LEN];
	U64 free_size;
    OS_Report("[ftps][hdlr] btmtk_ftps_pushfile_ind_handler");

    ind = (bt_ftps_pushfile_ind_struct *) msg;
       if( TRUE == btmtk_ftps_is_read_only() ){
            /* reject  */
            OS_Report("[ftps][hdlr] reject because readonly");
              bt_ftps_send_pushfile_rsp_msg(ind->goep_conn_id, GOEP_FORBIDDEN, file, 0, 0);       
       }else{
        // change to pushing state
        BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_RECEIVING);
        bt_ftp_obj_name_to_host_order(ind->obj_name, ind->obj_name);

        // send response
        if ( ext_ucs2strlen((S8 *) ind->obj_name) == 0) {
            file = (U8 *) L"noname";
        } else { 
              /* Put the prefix */
              GOEP_MEMSET( filepath, 0, sizeof(filepath));
              bt_ftp_compose_path((U8 *) g_ftps_root_folder,  act_server_cntx_p->current_folder,
                     (U8 *) path_buf, sizeof(path_buf) /2 );

              ftp_trace_filepath((U8 *) "root", g_ftps_root_folder);
              ftp_trace_filepath((U8 *) "cur folder", act_server_cntx_p->current_folder);
              ftp_trace_filepath((U8 *) "path_buf", (U8 *) path_buf);

#ifdef __MTK_EMMC_SUPPORT__
			if (act_server_cntx_p->ftps_browse_depth == 0) {
				bt_ftp_compose_path((U8 *) DEFAULT_SDCARD_PATH, ind->obj_name, (U8 *) filepath, sizeof(filepath) /2 );
			} else {
				bt_ftp_compose_path((U8 *) path_buf, ind->obj_name, (U8 *) filepath, sizeof(filepath) /2 );
			}
#else
              bt_ftp_compose_path((U8 *) path_buf, ind->obj_name, (U8 *) filepath, sizeof(filepath) /2 );
#endif

              ftp_trace_filepath((U8 *) "finalpath ", (U8 *) filepath);

			  ext_ucs2ncpy((S8 *) (act_server_cntx_p)->ftps_filepath, (const S8 *)filepath, sizeof((act_server_cntx_p)->ftps_filepath)/2);
                  
            file = (U8 *) filepath;
        }

#ifdef __MTK_EMMC_SUPPORT__
		if (act_server_cntx_p->ftps_browse_depth == 0) {
			free_size = bt_ftp_get_free_bytes((const U16*) DEFAULT_SDCARD_PATH);
		} else {
			free_size = bt_ftp_get_free_bytes(path_buf);
		}
#else
		free_size = bt_ftp_get_free_bytes(path_buf);
#endif
		bt_ext_log("[BT][FTP] obj size: %d, free size: %d", ind->total_obj_len, free_size);

		if (ind->total_obj_len >= free_size) {
			bt_ext_log("[BT][FTP] Space is not enough. obj size: %d, free size: %d", ind->total_obj_len, free_size);
	        BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
			bt_ftps_send_pushfile_rsp_msg(ind->goep_conn_id, GOEP_FORBIDDEN, file, 0, 0);
			return;
		}

		// notify MMI
		bt_ftps_push_prepare_routine((const U8*) (act_server_cntx_p)->ftps_filepath);
		// response OK to profile 
        bt_ftps_send_pushfile_rsp_msg(ind->goep_conn_id, 0, file, ext_ucs2strlen(
                (S8 *) file) * 2, ind->srm_enable);
    }
}

void btmtk_ftps_pushfile_rsp_ind_handler(void *msg) {
    bt_ftps_pushfile_rsp_ind_struct *ind;
    OS_Report("[BT][FTP] btmtk_ftps_pushfile_rsp_ind_handler()");

    ind = (bt_ftps_pushfile_rsp_ind_struct *) msg;

	if (act_server_cntx_p == NULL) {
		OS_Report("[BT][FTP] FTP server has been disabled.");
		return;
	}

	switch (act_server_cntx_p->ftps_state) {
		case BT_FTPS_STATE_RECEIVING:
		    // change to connected state
		    BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
			break;

		case BT_FTPS_STATE_DISCONNECTING:
		case BT_FTPS_STATE_DEACTIVATING:
			// Keep the server state for ending the transmission during disconnecting or deactivating
			break;

		default:
			OS_Report("[BT][FTP] %s(), invalid state: %d", __FUNCTION__, act_server_cntx_p->ftps_state);
			return;
	}
    btmtk_ftps_ui_callback(BT_FTPSUI_PUSHED, (void *) (U32) ind->rsp_code);

    // no response
    OS_Report("[FTP] pushfile_rsp_ind result:0x%x finalsize:%d", ind->rsp_code,
            ind->total_obj_len);

}

void btmtk_ftps_pullfile_ind_handler(void *msg) {
    bt_ftps_pullfile_ind_struct *ind;
    U8 *file = 0;
    U16 filepath[FMGR_MAX_PATH_LEN];
    U16 path_buf[FMGR_MAX_PATH_LEN];
    OS_Report("[ftps][hdlr] btmtk_ftps_pullfile_ind_handler d4");

    ind = (bt_ftps_pullfile_ind_struct *) msg;

    // change to pulling state
    BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_SENDING);
    
    // send response
    if (strlen((S8 *) ind->obj_mime_type) > 4) {
        // generate a folder list xml by current folder.
        // file = FTPS_FOLDERLIST_FILEPATH_WCS; /* for test */
         // save virtual folder path to new_folder
        ext_ucs2ncpy( (S8 *)ftps_cntx_p->root_folder, (const S8 *) g_ftps_root_folder, 100);
        bt_ftp_compose_path((act_server_cntx_p->current_folder), ind->obj_name,
                act_server_cntx_p->new_folder, sizeof(act_server_cntx_p->new_folder) /2 );
        ftp_trace_filepath((U8 *) "new post folder",  act_server_cntx_p->current_folder);
        bt_ftp_compose_path((const U8 *) ftps_cntx_p->root_folder,
                (const U8 *) act_server_cntx_p->new_folder,
                (U8 *) act_server_cntx_p->ftps_filepath,
                sizeof(act_server_cntx_p->ftps_filepath)/2);
        ftp_trace_filepath((U8 *) "root", ftps_cntx_p->root_folder);
        // p->new_folder is the 
        ext_ucs2ncpy((S8 *) act_server_cntx_p->new_folder,
                (const S8 *) act_server_cntx_p->current_folder,
                sizeof(act_server_cntx_p->new_folder)/2);                    
        ftp_trace_filepath((U8 *) "cur folder", (U8 *) filepath);
        // current ftps_obj_name is the new folder name
        ext_ucs2ncpy((S8 *) act_server_cntx_p->ftps_obj_name, (const S8 *) ind->obj_name,
                sizeof(act_server_cntx_p->ftps_obj_name)/2);

        ftp_trace_filepath((U8 *) "ftps_obj_name ",  act_server_cntx_p->ftps_obj_name);
#if 0                                
        if (FALSE == btmtk_goep_is_folder_exist((const U16 *)path_buf)) {
        }
#endif
        ext_ucs2ncpy((S8 *) path_buf, (const S8 *) bt_ftps_get_current_folder((U8)0), FMGR_MAX_PATH_LEN);
        ftp_trace_filepath((U8 *) "path_buf ", (U8 *) path_buf);
        
        /* path_buf is the browsing folder ex. 'c:    emp'.  filepath is the ex. 'z:\ftpc_folder.xml' (the xml content) */
        if (FALSE != btmtk_ftps_get_xml_filepath(0, (U8 *) filepath, FMGR_MAX_PATH_LEN * ENCODING_LENGTH)) {
            if (EXT_FS_OK == bt_ftps_gen_folder_content(&(act_server_cntx_p)->xml_composer,
                        (BT_BOOL)((act_server_cntx_p)-> ftps_browse_depth > 0 ? FALSE : TRUE),
                        (U8 *) path_buf, (U8 *) filepath, 0, NULL)) {
                /* update the context */
                file = (U8 *) filepath;
				btmtk_ftps_ui_callback(BT_FTPSUI_BROWSE_START, NULL);
            }else{
                /* fail to gen file -  sent a empty one */
                file = (U8 *) filepath;
            }
        }else{
            bt_ftps_send_pullfile_rsp_msg(ind->goep_conn_id, GOEP_FORBIDDEN, file, ext_ucs2strlen(
            (S8 *) file) * 2,  ind->srm_enable, 0 ); //QDAY 3 for test delay srm parameter
        }
    } else {
        /* change to host order */
        bt_ftp_obj_name_to_host_order(ind->obj_name, ind->obj_name);

        /* Put the prefix */
        GOEP_MEMSET( filepath, 0, sizeof(filepath));
        ext_ucs2ncpy((S8 *) filepath, (const S8 *) g_ftps_root_folder, FMGR_MAX_PATH_LEN);
        // ext_ucs2ncat((S8 *) filepath, (const S8 *) L"/", FMGR_MAX_PATH_LEN);
        
        ftp_trace_filepath((U8 *) "cur folder",  act_server_cntx_p->current_folder);
        /* if we has current sub-folder, append it to full filepath */
        if( ext_ucs2strlen((S8 *) act_server_cntx_p->current_folder ) > 0 && 0 != ext_ucs2cmp(act_server_cntx_p->current_folder , (U8 *) L"/") ){
            ext_ucs2ncat((S8 *) filepath, (const S8 *) act_server_cntx_p->current_folder, FMGR_MAX_PATH_LEN);
            ext_ucs2ncat((S8 *) filepath, (const S8 *) L"/", FMGR_MAX_PATH_LEN);
        }

        ext_ucs2ncat((S8 *) filepath, (const S8 *) ind->obj_name, FMGR_MAX_PATH_LEN);
        ftp_trace_filepath((U8 *) "finalpath ", (U8 *) filepath);

		ext_chset_ucs2_to_utf8_string((U8 *)path_buf, sizeof(path_buf), (U8 *) filepath);
		bt_ext_log("[BT][FTP] Start to pull a file: %s", path_buf);
		btmtk_ftps_ui_callback(BT_FTPSUI_PULL_FILE_START, (void *) path_buf);

        file = (U8 *) filepath;
    }
    /* return the full filepath */
    bt_ftps_send_pullfile_rsp_msg(ind->goep_conn_id, 0, file, ext_ucs2strlen(
            (S8 *) file) * 2, ind->srm_enable, 0); //QDAY 3 for test delay srm parameter
}

void btmtk_ftps_pullfile_rsp_ind_handler(void *msg) {
    bt_ftps_pullfile_rsp_ind_struct *ind;
    OS_Report("[BT][FTP] btmtk_ftps_pullfile_rsp_ind_handler()");

    ind = (bt_ftps_pullfile_rsp_ind_struct *) msg;

	if (act_server_cntx_p == NULL) {
		OS_Report("[BT][FTP] FTP server has been disabled.");
		return;
	}

	switch (act_server_cntx_p->ftps_state) {
		case BT_FTPS_STATE_SENDING:
    		// change to connected state
		    BT_FTPS_STATE_TRANS(0, BT_FTPS_STATE_CONNECTED);
			break;

		case BT_FTPS_STATE_DISCONNECTING:
		case BT_FTPS_STATE_DEACTIVATING:
			// Keep the server state for ending the transmission during disconnecting or deactivating
			break;

		default:
			OS_Report("[BT][FTP] %s(), invalid state: %d", __FUNCTION__, act_server_cntx_p->ftps_state);
			return;
	}

    // no response
    btmtk_ftps_ui_callback(BT_FTPSUI_PULLED, (void *) (U32) ind->rsp_code);

    // TODO: show success or error
    OS_Report("[FTP] pushfile_rsp_ind result:%d", ind->rsp_code);
}

void btmtk_ftps_abortfile_cnf_handler(void *msg) {
    bt_ftps_abortfile_cnf_struct *ind;
    OS_Report("[ftps][hdlr] btmtk_ftps_abortfile_cnf_handler");

    ind = (bt_ftps_abortfile_cnf_struct *) msg;

    OS_Report("[FTP] abortfile_cnf result:%d", ind->rsp_code);
}

void btmtk_ftps_file_progress_handler(void *msg) {
    bt_ftps_file_progress_ind_struct *ind;
    ind = (bt_ftps_file_progress_ind_struct *) msg;
    switch (ind->operation) {
    case GOEP_OPER_PUSH:
        btmtk_ftps_ui_callback(BT_FTPSUI_PUSHING, (void *) (U32) ind->curr_length);
        break;
    case GOEP_OPER_PULL:
        btmtk_ftps_ui_callback(BT_FTPSUI_PULLING, (void *) (U32) ind->curr_length);
        break;
    default:
        break;
    }
}

void btmtk_ftps_handle_message(ilm_struct *message)
{
	void *data;
	// data = (void *)message->local_para_ptr;
	data = (void*) message->ilm_data;

	// convert GOEP ID to FTP ID. Add server interval or clietn interval 
	if (message->msg_id <= MSG_ID_BT_GOEPC_GROUP_END && message->msg_id >= MSG_ID_BT_GOEPC_GROUP_START) {
		message->msg_id += ((U32)MSG_ID_BT_FTPC_GROUP_START - (U32) MSG_ID_BT_GOEPC_GROUP_START );
	} else if (message->msg_id <= MSG_ID_BT_GOEPS_GROUP_END && message->msg_id >= MSG_ID_BT_GOEPS_GROUP_START) {
		message->msg_id += ((U32)MSG_ID_BT_FTPS_GROUP_START - (U32) MSG_ID_BT_GOEPS_GROUP_START );
	}    
	// btmtk_goep_show_id(message->msg_id, FALSE, MOD_MMI);

	if (act_server_cntx_p == NULL) {
		OS_Report("[BT][FTP] act_server_cntx_p is NULL in %s(), msg_id: %d. Ignore for now.",
				__FUNCTION__, message->msg_id);
		return;
	}

	switch (message->msg_id)
	{
		/** Server **/
		case MSG_ID_BT_FTPS_REGISTER_SERVER_CNF:
			bt_ext_log("MSG_ID_BT_FTPS_REGISTER_SERVER_CNF");
			btmtk_ftps_register_server_cnf_handler(data);
			break;
		case MSG_ID_BT_FTPS_DEREGISTER_SERVER_CNF:
			bt_ext_log("MSG_ID_BT_FTPS_DEREGISTER_SERVER_CNF");
			btmtk_ftps_deregister_server_cnf_handler(data);
			break;
		case MSG_ID_BT_FTPS_AUTHORIZE_IND: // server
			bt_ext_log("MSG_ID_BT_FTPS_AUTHORIZE_IND");
			btmtk_ftps_authorize_ind_handler(data);
			break;
		case MSG_ID_BT_FTPS_CONNECT_IND:
			bt_ext_log("MSG_ID_BT_FTPS_CONNECT_IND");
			btmtk_ftps_connect_ind_handler(data);
			break;
		case MSG_ID_BT_FTPS_PUSH_IND:
			 bt_ext_log("MSG_ID_BT_FTPS_PUSH_IND");
			btmtk_ftps_push_ind_handler(data);
			break;
		case MSG_ID_BT_FTPS_PULL_IND:
			bt_ext_log("MSG_ID_BT_FTPS_PULL_IND");
			btmtk_ftps_pull_ind_handler(data);
			break;
		case MSG_ID_BT_FTPS_SET_FOLDER_IND:
			bt_ext_log("MSG_ID_BT_FTPS_SET_FOLDER_IND");
			btmtk_ftps_set_folder_ind_handler(data);
			break;
		case MSG_ID_BT_FTPS_ABORT_IND:
			bt_ext_log("MSG_ID_BT_FTPS_ABORT_IND");
			btmtk_ftps_abort_ind_handler(data);
			break;
		case MSG_ID_BT_FTPS_ACTION_IND:
			btmtk_ftps_action_ind_handler(data);
			break;
		case MSG_ID_BT_FTPS_SERVER_INTERNAL_RW:
			// bt_ext_log("MSG_ID_BT_FTPS_SERVER_INTERNAL_RW");
			btmtk_ftps_internal_rw_handler(data);
			break;
		case MSG_ID_BT_FTPS_TPDISCONNECT_IND: // server & client
			bt_ext_log("MSG_ID_BT_FTPS_TPDISCONNECT_IND");
			btmtk_ftps_disconnect_ind_handler(data);
			break;
		case MSG_ID_BT_FTPS_AUTH_IND:
			bt_ext_log("MSG_ID_BT_FTPS_AUTH_IND");
			btmtk_ftps_auth_ind_handler(data);
			break;
		case MSG_ID_BT_FTPS_AUTH_CNF:
			bt_ext_log("MSG_ID_BT_FTPS_AUTH_CNF");
			btmtk_ftps_auth_cnf_handler(data);
			break;
		case MSG_ID_BT_FTPS_PUSHFILE_IND:
 			btmtk_ftps_pushfile_ind_handler(data);
			break;
		case MSG_ID_BT_FTPS_PUSHFILE_RSP_IND:
			btmtk_ftps_pushfile_rsp_ind_handler(data);
			break;
		case MSG_ID_BT_FTPS_PULLFILE_IND:
			btmtk_ftps_pullfile_ind_handler(data);
			break;
		case MSG_ID_BT_FTPS_PULLFILE_RSP_IND:
 			btmtk_ftps_pullfile_rsp_ind_handler(data);
 			break;
 		case MSG_ID_BT_FTPS_ABORTFILE_CNF:
			btmtk_ftps_abortfile_cnf_handler(data);
			break;
		case MSG_ID_BT_FTPS_FILE_PROGRESS_IND:
			btmtk_ftps_file_progress_handler(data);
 			break;
		default:
			bt_ext_err("Unknown message.");
			//printf("[ftps] err %d %s\n", message->msg_id, bt_goep_get_id_name(message->msg_id));
			break;
	}
}

