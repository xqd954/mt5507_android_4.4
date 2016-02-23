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
 * bt_ftpc_hdl.h
 *
 * Project:
 * --------
 *   MMI
 *
 * Description:
 * ------------
 *   
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
#include "ext_osal.h"
#include "ext_nbuffer.h"
#include "bluetooth_ftps_struct.h"
#include "bluetooth_ftpc_struct.h"
#include "bt_ftpc_struct.h"
#include "bt_ftpc_event.h"
#include "bt_ftpc_api.h"
#include "bt_ftpc_hdl.h"
#include "ext_ucs2.h"
#include "bt_goep_fs.h"
#include "bt_ftp_porting.h"

#include "bt_ext_debug.h"

#define GOEP_Report bt_ext_log
#define GOEP_OPER_PUSH              3
#define GOEP_OPER_PULL              5

bt_ftpc_cntx_struct g_bt_ftpc_cntx;
bt_ftpc_cntx_struct *const ftpc_cntx_p = &g_bt_ftpc_cntx;
bt_ftpc_conn_cntx_struct *act_client_cntx_p;

static const unsigned char OBEX_FTP_UUID[] = {0xF9, 0xEC, 0x7B, 0xC4, 0x95, 0x3C, 0x11, 0xD2,
											  0x98, 0x4E, 0x52, 0x54, 0x00, 0xDC, 0x9E, 0x09 };


#if defined(__BT_FTC_PROFILE__)

/// local API
static void bt_ftpc_flush_one_nbuffer_to_file();
static void bt_ftpc_flush_all_nbuffer_to_file();
void bt_ftpc_reset_abort_req_sent_flag(void);
static void bt_ftpc_send_pull_folder_continue_msg();
void btmtk_ftpc_pushfile_cnf_handler(void *msg);
void btmtk_ftpc_pullfile_cnf_handler(void *msg);
void btmtk_ftpc_file_progress_ind_handler(void *msg);
void btmtk_ftpc_abortfile_cnf_handler(void *msg);

BT_BOOL bt_ftpc_should_send_abort(U8 index,  goep_pkt_type_enum pkt);

const U8 *bt_ftpc_get_current_file_name(U8 index) {
	return (U8 *)(act_client_cntx_p)->ftpc_obj_name;
}

const U8 *bt_ftpc_get_receive_folder(U8 index) {
	return (U8 *)(act_client_cntx_p)->push_local_path;
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_disconnect_server
 * DESCRIPTION
 *  This function is to disconnect the FTP application
 * PARAMETERS
 *  bd_addr         [IN]        
 *  g_conn_id       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftpc_disconnect_server(U8 i)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/


	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/

	for (i = 0; i < FTP_MAX_SERVER_SUPPORT; i++)
	{
		(act_client_cntx_p + i)->discon_from_cm = TRUE;
		switch ((act_client_cntx_p + i)->ftpc_state)
		{
			// this case shal does not happen, do nothing
			case BT_FTPC_STATE_IDLE:
			case BT_FTPC_STATE_CONNECTING:
			case BT_FTPC_STATE_CONNECTED:
			case BT_FTPC_STATE_GETTING_FOLDER:
			case BT_FTPC_STATE_GETTING_OBJ:
			case BT_FTPC_STATE_SETTING_FOLDER:
			case BT_FTPC_STATE_SETTING_BACK_FOLDER:
			case BT_FTPC_STATE_SETTING_ROOT_FOLDER:
			case BT_FTPC_STATE_ABORTING:
				break;

			// have sent tpdisconnect_req, do nothing
			case BT_FTPC_STATE_TPDISCONNECTING:
				break;

			case BT_FTPC_STATE_DISCONNECTING:
				bt_ftpc_send_tpdisconnect_message(i);
				BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_TPDISCONNECTING);
				break;

			default:
				break;
		}

	} //for  i < FTP_MAX_SERVER_SUPPORT
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_is_run
 * DESCRIPTION
 *  This function is to get current action.
 *  
 *  PARAMETERS: void
 *  void
 *  RETURNS: received file counter.(?)(?)
 *****************************************************************************/
BT_BOOL bt_ftpc_is_run(U8 index)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_IS_RUN, act_client_cntx_p->ftpc_state));

	if ((act_client_cntx_p+index)->ftpc_state >= BT_FTPC_STATE_CONNECTING
		&& (act_client_cntx_p+index)->ftpc_state < BT_FTPC_STATE_DISCONNECTING)
	{
		return TRUE;
	}
	return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_is_canceling
 * DESCRIPTION
 *  This function is to get current action.
 *  
 *  PARAMETERS: void
 *  void
 *  RETURNS: received file counter.(?)(?)
 *****************************************************************************/
BT_BOOL bt_ftpc_is_canceling(U8 index)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS,
			BT_FTPC_IS_CANCELING,
			act_client_cntx_p->ftpc_state,
			act_client_cntx_p->flag_abort_req_sent));

	if ((act_client_cntx_p+index)->ftpc_state == BT_FTPC_STATE_DISCONNECTING)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_is_aborting
 * DESCRIPTION
 *  This function is to get current action.
 *  
 *  PARAMETERS: void
 *  void
 *  RETURNS: received file counter.(?)(?)
 *****************************************************************************/
BT_BOOL bt_ftpc_is_aborting(U8 index)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS,
			BT_FTPC_IS_ABORTING,
			act_client_cntx_p->ftpc_state,
			act_client_cntx_p->flag_abort_req_sent));

	if ((act_client_cntx_p+index)->ftpc_state == BT_FTPC_STATE_ABORTING)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftp_client_is_receiving
 * DESCRIPTION
 *  This function is to get current status of ftp client.
 *  
 *  PARAMETERS: void
 *  void
 *  RETURNS: received file counter.(?)(?)
 *****************************************************************************/
BT_BOOL bt_ftp_client_is_receiving(U8 index)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	if ((act_client_cntx_p+index)->ftpc_state == BT_FTPC_STATE_GETTING_OBJ)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_retrieve_get_prog_percentage
 * DESCRIPTION
 *  This function is to retrieve the file put progress.
 * PARAMETERS
 *     
 * RETURNS
 *  1 ~ 100  The percentage of put process.(?)
 *****************************************************************************/
U8 bt_ftpc_retrieve_get_prog_percentage(U32 i)
{
	U8 l_ret = 0;
	if (((act_client_cntx_p + i)->ftpc_total_len) > ((act_client_cntx_p + i)->ftpc_data_len)) {
		l_ret = (U8 )((act_client_cntx_p + i)->ftpc_data_len/((act_client_cntx_p + i)->ftpc_total_len));
	} else {
		l_ret = 100;
	}
	return l_ret;
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_set_state
 * DESCRIPTION
 *  
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftpc_set_state(U32 i, bt_ftpc_state_enum ftpc_state)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	BT_FTPC_STATE_TRANS(i, ftpc_state);
}    


/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_get_state
 * DESCRIPTION
 *  
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
U32 bt_ftpc_get_state(U32 i)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	return (act_client_cntx_p + i)->ftpc_state;
}



/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_is_get_root
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 *  
 * RETURNS
 *  void
 *****************************************************************************/
BT_BOOL bt_ftpc_is_get_root(U32 i)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/

	if ((act_client_cntx_p + i)->got_root == 0)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_find_goep_id
 * DESCRIPTION
 *  This function is to find the goep_id from ftpc context
 * PARAMETERS
 *  index       [IN]        
 * RETURNS
 *  U32 index
 *****************************************************************************/
U32 bt_ftpc_find_goep_id(U8 index)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	//FTP_DEBUG(kal_print("FTPC FIND INDEX\n");)
	// fine the index
	for (i = 0; i < FTP_MAX_SERVER_SUPPORT; i++)
	{
		if (ftpc_cntx_p->goep_conn_id[i] == index)
		{
			return i;
		}
	}
	// index is not correct
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_FIND_GOEP_ID_FAILED, ftpc_cntx_p->goep_conn_id[0], index) );
	return 0xFF;
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_rename_recved_obj
 * DESCRIPTION
 *  This function is to rename the temp obj name to actual
 * PARAMETERS
 *  i       [IN]        
 *  U32(?)
 * RETURNS
 *  int
 *****************************************************************************/
 #if 0
static BT_BOOL bt_ftpc_rename_recved_obj(U32 i)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int attr;
    U8 *path_buf_actual;
    U8 *path_buf_tmp;
    S32 path_len;
    const U8 *curr_obj_name;
    U32 fcnt = 1;
    BT_BOOL ret = FALSE;
    U8 ucTryCount = 0;
    U8 ucIncrName[10], pre_fix[10];
    U32 pre_fix_len;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_RENAME_OBJ) );
    curr_obj_name = bt_ftpc_get_current_file_name(0	);
    
    /* Temp file name */
    path_buf_tmp = GET_CTRL_BUFFER(FMGR_MAX_PATH_LEN * ENCODING_LENGTH );
    BT_FTP_ASSERT(path_buf_tmp != NULL);
    GOEP_MEMSET(path_buf_tmp, 0, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
    btmtk_ftps_create_tmp_objname(path_buf_tmp, (act_client_cntx_p + i)->tmpname_fcnt);

    /* Actual file name */
    path_buf_actual = GET_CTRL_BUFFER(FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
    BT_FTP_ASSERT(path_buf_actual != NULL);
    GOEP_MEMSET(path_buf_actual, 0, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);

    if( FALSE ==  btmtk_goep_is_folder_exist( act_client_cntx_p->push_local_path)  ){
        return FALSE; // double check the destion folder exist or not
    }
    ext_ucs2ncpy( path_buf_actual, (const S8*)act_client_cntx_p->push_local_path, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
    btmtk_goep_create_folder((const U16 *)path_buf_actual);
    
    path_len = ext_ucs2strlen((S8 *)path_buf_actual);
    
    if (path_len + ext_ucs2strlen((S8 *) curr_obj_name) + 1 > FMGR_MAX_PATH_LEN)
    {
        bt_ftp_ucs2_cut_name_with_ext(
            path_buf_actual + path_len * ENCODING_LENGTH, 
            (FMGR_MAX_PATH_LEN - path_len) * ENCODING_LENGTH, 
            curr_obj_name);
    }
    else
    {
        btmtk_goep_compose_filepath( (const U8 *) act_client_cntx_p->push_local_path, (const U8 *) curr_obj_name, path_buf_actual, FMGR_MAX_PATH_LEN); 
    }

    /* actual file already exist */
    
    // generate  a don't exist temp file name
    if( FALSE == btmtk_goep_is_file_exist( (const U16*)path_buf_actual ) ){
        attr = btmtk_goep_rename_file((U16*) act_client_cntx_p->ftpc_filepath, (U16*) path_buf_actual);
        return TRUE;
    }else{
        // file exist
        for( ucTryCount = 0 ; ucTryCount < 128; ucTryCount++ ){
            GOEP_MEMSET(ucIncrName, 0, sizeof(ucIncrName) );
            ext_strncpy( ucIncrName, "Dup-", 10);
            ucIncrName[4] = (U8) (ucTryCount/10) + '0';
            ucIncrName[5] = (U8) (ucTryCount%10) + '0';
            
            ext_asc_str_n_to_ucs2_str(pre_fix, "Dup-1", 10);
            pre_fix_len = ext_ucs2strlen((S8 *) pre_fix);

            GOEP_MEMCPY((void *)(path_buf_actual + path_len * ENCODING_LENGTH), (void *)pre_fix, pre_fix_len * ENCODING_LENGTH);
            GOEP_MEMSET((void *)(path_buf_actual + (path_len + pre_fix_len) * ENCODING_LENGTH), 0, ENCODING_LENGTH);

            if (path_len + pre_fix_len + ext_ucs2strlen((S8 *)curr_obj_name) + 1 > FMGR_MAX_PATH_LEN)
            {
                bt_ftp_ucs2_cut_name_with_ext(
                    path_buf_actual + (path_len + pre_fix_len) * ENCODING_LENGTH,
                    (FMGR_MAX_PATH_LEN - path_len - pre_fix_len) * ENCODING_LENGTH,
                    curr_obj_name);
            }
            else
            {
                ext_ucs2ncat((S8 *) path_buf_actual, (S8 *)curr_obj_name, FMGR_MAX_PATH_LEN);
            }
            
            if( btmtk_goep_is_file_exist( (const U16*)path_buf_actual )  ){
                continue;
            }
            
            
        }
    }
 
    if( NULL != path_buf_actual ){
    	FREE_CTRL_BUFFER((void *)path_buf_actual);
    }
    if( NULL != path_buf_tmp ){
    	FREE_CTRL_BUFFER((void *)path_buf_tmp);
    }
    return ret;
}
#endif


/**
* @brief This Function is to write the first pkt of the file and create the file handle
* @return SUCCESS 
*/
int bt_ftpc_write_obj(U8 i, goep_pull_cnf_struct *rsp)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	FHANDLE h;
	goep_pull_req_struct *req;
	S32 u4Index, u4BuffSize, len;
	U8 *pBuffer;    

	char debug[256];

	i = 0;
	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_WRITE_OBJ, rsp->pkt_type, rsp->frag_len));
	// GOEP_TRACE_PATH(BT_FTP_TRC_CLASS, BT_FTPC_TARGET_FILEPATH, act_client_cntx_p->ftpc_filepath);

	ext_ucs2_str_n_to_asc_str(debug,
			(S8 *) act_client_cntx_p->ftpc_filepath,
			sizeof(act_client_cntx_p->ftpc_filepath)/2);

	// check the receive folder. copy form btmtk_ftpc_push_file
	// ext_ucs2ncpy((S8 *)act_client_cntx_p->ftpc_filepath,
	//	    (const S8 *)btmtk_ftpc_get_default_receive_filepath(),
	//	    FMGR_MAX_PATH_LEN *ENCODING_LENGTH);

	h = btmtk_goep_open_wstream((U16*) act_client_cntx_p->ftpc_filepath,
			(EXT_FS_READ_WRITE | EXT_FS_CREATE_ALWAYS));

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_WRITE_OBJ_OPEN, h));
	if (FALSE != act_client_cntx_p->ftpc_use_nbuffer) {
		ext_nbuffer_cleanup(&act_client_cntx_p->ftpc_mempool);     	
	}

	if (FALSE == ext_fs_is_valid_handle_ucs2(h)) {
		// File open error. Error handling
		GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_UTIL_OPEN_FILE_FAIL, h));
		bt_ftpc_set_fs_flag((int) h);
		return -1;

	} else {
		// set file handle to context
		(act_client_cntx_p + i)->fh_ftpc_recv = h;
	}

	// Write object context to file
	if (rsp->frag_len > 0) {
		U32 len_written;
		int ret;

		len =  rsp->frag_len;

		if (FALSE != act_client_cntx_p->ftpc_use_nbuffer) {
			if (FALSE != ext_nbuffer_isFull(&act_client_cntx_p->ftpc_mempool)) {
				// TRUE: full nbuffer
				bt_ftpc_flush_all_nbuffer_to_file();
				ret = act_client_cntx_p->ftpc_fs_status;

				if (EXT_FS_OK == act_client_cntx_p->ftpc_fs_status) {
					//X: FS_Write(handle, data, len, written);
					ret = btmtk_goep_write_wstream(h,
					rsp->frag_ptr,
					rsp->frag_len,
					(S32*) &len_written);
				}

			} else {
				if (FALSE != ext_nbuffer_isEmpty( &act_client_cntx_p->ftpc_mempool)) {
					bt_ftp_send_msg_toself( MSG_ID_BT_FTPC_CLIENT_INTERNAL_RW, NULL, 0);
				}

				// put to end of mempool
				pBuffer = (U8 *) ext_nbuffer_get_tail_buffer(&act_client_cntx_p->ftpc_mempool, (U32 *) &u4Index, &u4BuffSize);
				GOEP_Report("[BT][FTP] get nbuffer for write. index:(%d) bufsize:(%d)", u4Index, u4BuffSize);

				if (u4BuffSize >= len) {
					GOEP_MEMCPY( pBuffer, rsp->frag_ptr, len);

					if (FALSE != ext_nbuffer_return_tail_buffer(&act_client_cntx_p->ftpc_mempool, u4Index, len)) {
						GOEP_Report("[BT][FTP] write to nbuffer fail. return_tail_buffer return false. index:(%d) len:(%d)",
								u4Index, len);
						ret = EXT_FS_OK;

					} else {
						ret = EXT_FS_ERR;
					}

				} else {
					ret = EXT_FS_ERR;
				}
			}

		} else {
			ret = btmtk_goep_write_wstream(h, rsp->frag_ptr, rsp->frag_len, (S32*) &len_written);
			act_client_cntx_p->ftpc_fs_status = ret;
		}

		// GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_WRITE_OBJ_WRITE, ret));

		// Write error
		if (ret < 0) {
			bt_ftpc_set_fs_flag((int) ret);
			if ((rsp->pkt_type == GOEP_FINAL_PKT) || (rsp->pkt_type == GOEP_SINGLE_PKT)) {
				btmtk_ftpc_ui_callback(BT_FTPCUI_PULLED, (void *) FS_ERR_FOLDER_ERR); 
			} else {
				// PULLED will be invoked in abort_cnf. 
			}
			return -1;
		}

		} else {
		// FTP_DEBUG(kal_print("FTPC WRITE OBJ (LEN <= 0)\n"););       
		}

		// knows the total len
		if ((act_client_cntx_p + i)->total_len_type == BT_FTPC_TOTAL_LEN_NO_ZERO) {
			(act_client_cntx_p + i)->ftpc_data_len += rsp->frag_len;
			(act_client_cntx_p + i)->ftpc_reamin_len -= rsp->frag_len;

			// GOEP_Report("[FTPC] update data info. total:%d, data_len:(%d) remain:(%d)",
			//		(act_client_cntx_p + i)->ftpc_total_len,
			//		(act_client_cntx_p + i)->ftpc_data_len,
			//		(act_client_cntx_p + i)->ftpc_reamin_len);

		} else {
			(act_client_cntx_p + i)->ftpc_data_len += rsp->frag_len;
		}

		// check final packet -- if final -> notify pulled
		if ((rsp->pkt_type == GOEP_FINAL_PKT) || (rsp->pkt_type == GOEP_SINGLE_PKT)) {
			//FTP_DEBUG(kal_print("FTPC WRITE OBJ (FINAL PKT)\n"););       

			// close file
			if ((act_client_cntx_p + i)->fh_ftpc_recv) {
				GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_CLOSE_RECV_FILE));
			
				if (FALSE != act_client_cntx_p->ftpc_use_nbuffer) {
					bt_ftpc_flush_all_nbuffer_to_file();
				}

				btmtk_goep_close_wstream((act_client_cntx_p + i)->fh_ftpc_recv);
				(act_client_cntx_p + i)->fh_ftpc_recv = FHANDLE_INVALID_VALUE;
			}

			btmtk_ftpc_ui_callback(BT_FTPCUI_PULLING, (void *) act_client_cntx_p->ftpc_total_len);
			btmtk_ftpc_ui_callback(BT_FTPCUI_PULLED, (void *)0);
			BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);

		} else {
			// Get next object body
			req = (goep_pull_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_pull_req_struct), TD_CTRL);
			// memory init
			GOEP_MEMSET((U8 *)req, 0, sizeof(goep_pull_req_struct));
			req->goep_conn_id = ftpc_cntx_p->goep_conn_id[i];
			req->pkt_type = GOEP_NORMAL_PKT;
			GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
			req->uuid_len = 16;
			bt_ftp_send_msg(MSG_ID_BT_FTPC_PULL_REQ, req ,sizeof(goep_pull_req_struct));
			btmtk_ftpc_ui_callback(BT_FTPCUI_PULLING, (void *) ((act_client_cntx_p + i)->ftpc_data_len)); 
		}
	return 1;
}


/**
* @brief This function is to write the pkt from 2nd pkt, and send pull_req for next 
* @return SUCCESS ok. Fail file handle has been closed
**/
int bt_ftpc_write_obj_continue(U8 i , goep_pull_cnf_struct *rsp)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	goep_pull_req_struct *req;
	S32 u4Index, u4BuffSize, len;  // for nbuffer
	U8 *pBuffer;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_WRITE_OBJ_CONTINUE, rsp->pkt_type, rsp->frag_len));

	// Write object context to file
	if (rsp->frag_len > 0) {
		U32 len_written;
		int ret;

		len = rsp->frag_len;
		BT_FTP_ASSERT((act_client_cntx_p + i)->fh_ftpc_recv);
		if (FALSE != (act_client_cntx_p + i)->ftpc_use_nbuffer) {
			// write to nbuffer the
			if (FALSE != ext_nbuffer_isFull(&act_client_cntx_p->ftpc_mempool)) {
				// TRUE: full. write all data and
				bt_ftpc_flush_all_nbuffer_to_file();
				ret = (act_client_cntx_p + i)->ftpc_fs_status;

				if (EXT_FS_OK == (act_client_cntx_p + i)->ftpc_fs_status) {
					// X: FS_Write(handle, data, len, written);
					ret = btmtk_goep_write_wstream(
							(act_client_cntx_p + i)->fh_ftpc_recv,
							rsp->frag_ptr,
							rsp->frag_len,
							(S32*) &len_written);
				}

			} else {
				if (FALSE != ext_nbuffer_isEmpty(&(act_client_cntx_p + i)->ftpc_mempool)) {
					bt_ftp_send_msg_toself( MSG_ID_BT_FTPC_CLIENT_INTERNAL_RW, NULL, 0);
				}

				// put to end of mempool
				pBuffer = (U8 *) ext_nbuffer_get_tail_buffer(&act_client_cntx_p->ftpc_mempool, (U32 *) &u4Index, &u4BuffSize);
				if (u4BuffSize >= len) {
					GOEP_MEMCPY( pBuffer, rsp->frag_ptr, len);
					if (FALSE != ext_nbuffer_return_tail_buffer(&act_client_cntx_p->ftpc_mempool, u4Index, len)) {
						ret = EXT_FS_OK;
					} else {
						ret = EXT_FS_ERR;
						return -1;
					}
				} else {
					ret = EXT_FS_ERR;
					return -1;
				}
			}

		} else {
			ret = btmtk_goep_write_wstream((act_client_cntx_p)->fh_ftpc_recv,
					rsp->frag_ptr,
					rsp->frag_len,
					(S32*) &len_written);
		}

		// GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_WRITE_OBJ_CONTINUE_WRITE, ret));

		// Write error
		if (ret < 0) {
			// FTP_DEBUG(kal_print("FTPC WRITE OBJ ERROR -- btmtk_goep_write_wstream ERROR\n");)
			bt_ftpc_set_fs_flag((int) ret);

			// ALPS00127542:
			//		"return -1;" will trigger sending a abort-request to server.
			//		So, don't duplicate ui callback which will be invoked in abort_cnf_handler().
			// btmtk_ftpc_ui_callback(BT_FTPCUI_PULLED, (void *) FS_ERR_FOLDER_ERR);
			return -1;
		}

	} else {
		//FTP_DEBUG(kal_print("FTPC WRITE OBJ (LEN <= 0)\n"););       
	}

	(act_client_cntx_p)->ftpc_data_len += rsp->frag_len;

	// knows the total len
	if ((act_client_cntx_p + i)->total_len_type == BT_FTPC_TOTAL_LEN_NO_ZERO) {
		if ((act_client_cntx_p + i)->ftpc_reamin_len >= rsp->frag_len) {
			(act_client_cntx_p + i)->ftpc_reamin_len = (act_client_cntx_p + i)->ftpc_reamin_len - rsp->frag_len;
		}
		// get uploading file
		else {
			/* keep bar at position 99% */
			(act_client_cntx_p + i)->ftpc_reamin_len = 1;
		}

	} else {
		// if ((act_client_cntx_p)->ftpc_data_len > (act_client_cntx_p)->ftpc_total_len) {
		// 	(act_client_cntx_p)->ftpc_total_len = (act_client_cntx_p)->ftpc_data_len;
		// }
		(act_client_cntx_p + i)->ftpc_reamin_len = 1;
	}

	// GOEP_Report("[FTPC] update data info. total:%d, data_len:(%d) remain:(%d)",
	//	    (act_client_cntx_p + i)->ftpc_total_len,
	//	    (act_client_cntx_p + i)->ftpc_data_len,
	//	    (act_client_cntx_p + i)->ftpc_reamin_len);

	// check final packet -- if final -> notify MMI
	if ((rsp->pkt_type == GOEP_FINAL_PKT) || (rsp->pkt_type == GOEP_SINGLE_PKT)) {
		// FTP_DEBUG(kal_print("FTPC WRITE OBJ CONTINUE (FINAL PKT)\n"););       
		(act_client_cntx_p + i)->ftpc_reamin_len = 0;

		// close file
		if (FALSE != btmtk_goep_is_valid_handle((act_client_cntx_p + i)->fh_ftpc_recv)) {
			GOEP_TRACE((BT_FTP_TRC_CLASS,BT_FTPC_CLOSE_RECV_FILE));

			if (FALSE != act_client_cntx_p->ftpc_use_nbuffer) {
				bt_ftpc_flush_all_nbuffer_to_file();
			}

			btmtk_goep_close_wstream((act_client_cntx_p + i)->fh_ftpc_recv);
			(act_client_cntx_p + i)->fh_ftpc_recv = FHANDLE_INVALID_VALUE;
		}

		btmtk_ftpc_ui_callback(BT_FTPCUI_PULLED, (void *) GOEP_STATUS_SUCCESS);
		BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);

	} else {
		// GOEP_Report("[FTPC]_write_obj_continue i:%d build send_msg ", i);
		// Get next object body
		req = (goep_pull_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_pull_req_struct),TD_CTRL);
		GOEP_MEMSET( (U8 *) req, 0, sizeof(goep_pull_req_struct));    // memory init
		req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
		req->pkt_type = GOEP_NORMAL_PKT;
		GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
		req->uuid_len = 16;
		bt_ftp_send_msg(MSG_ID_BT_FTPC_PULL_REQ, req ,sizeof(goep_pull_req_struct));
		btmtk_ftpc_ui_callback( BT_FTPCUI_PULLING, (void *) ((act_client_cntx_p)->ftpc_data_len) );
	}

	return 1;
}


/**
* This function is to delete the temp object file when error occurs  
* The file is temp file for getting obj, not or getting folder.
* @param i useless. pass 0
**/
void bt_ftpc_delete_pulling_file(U32 i)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U8 *path_buf_tmp;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	//FTP_DEBUG(kal_print("FTPC DELETE TEMP OBJECT-- DOWNLOAD ERROR\n");)

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_DEL_TMP_FILE));

	// current only support only one connection!
	if (i != 0) {
		i = 0;
	}

	// Temp file name
	path_buf_tmp = GET_CTRL_BUFFER(FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
	BT_FTP_ASSERT(path_buf_tmp != NULL);
	GOEP_MEMSET(path_buf_tmp, 0, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
	((local_para_struct*)path_buf_tmp)->ref_count = 1;  // APPI target 091109: fix memory leak

	// pulling file handle is in 
	if (FALSE != ext_fs_is_valid_handle_ucs2((act_client_cntx_p + i)->fh_ftpc_recv)) {
		// close it and delete it
		btmtk_goep_close_wstream((act_client_cntx_p + i)->fh_ftpc_recv);
		(act_client_cntx_p + i)->fh_ftpc_recv = FHANDLE_INVALID_VALUE;

		//delete it by the name in 
		GOEP_TRACE_PATH(BT_FTP_TRC_CLASS, BT_FTPC_DEL_RECV_FILE, act_client_cntx_p->ftpc_filepath);
		btmtk_goep_delete_file((const U16*) act_client_cntx_p->ftpc_filepath);
		btmtk_goep_clear_filepath((U8 *) act_client_cntx_p->ftpc_filepath);
		btmtk_ftpc_ui_callback(BT_FTPCUI_PULLED, (void *)FS_ERR_FOLDER_ERR);
	}

	if (path_buf_tmp) {
		FREE_CTRL_BUFFER((void *)path_buf_tmp);
	}
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_get_folder
 * DESCRIPTION
 *  This function is to handel the getting folder operation
 * PARAMETERS
 *  rsp     [?]         
 *  i       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/

void bt_ftpc_get_folder(U8 i, goep_pull_cnf_struct *rsp)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	int ret;
	// bt_ftp_file_writer_struct *writer;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_GET_FOLDER, rsp->rsp_code, rsp->pkt_type, rsp->to_self));

	switch (rsp->rsp_code) {
		case GOEP_STATUS_SUCCESS:
		{
			// FHANDLE h;
			S32 h;

			if ((rsp->pkt_type == GOEP_FIRST_PKT) || (rsp->pkt_type == GOEP_SINGLE_PKT)) {
				//FTP_DEBUG(kal_print("FTPC GET FOLDER -- FIRST PKT\n");)

				// close file first if necessary
				if ((act_client_cntx_p + i)->fh_ftpc_recv) {
					GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_CLOSE_RECV_FILE));

					btmtk_goep_close_wstream((act_client_cntx_p + i)->fh_ftpc_recv);
					(act_client_cntx_p + i)->fh_ftpc_recv = (FHANDLE) FHANDLE_INVALID_VALUE;
				}

				// Create folder listing object file
				GOEP_TRACE_PATH(BT_FTP_TRC_CLASS, BT_FTPC_TARGET_FILEPATH, (act_client_cntx_p + i)->folder_obj);
				h = btmtk_goep_open_wstream((U16*) ((act_client_cntx_p + i)->folder_obj),
						(EXT_FS_READ_WRITE | EXT_FS_CREATE_ALWAYS));
				
				GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_GET_FOLDER_OPEN, h));

				if (FALSE == ext_fs_is_valid_handle_ucs2(h)) {
					GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_OPEN_FILE_FAIL, h));
					//FTP_DEBUG(kal_print("FTPC GET FOLDER -- btmtk_goep_open_rstream ERROR\n");)
					btmtk_ftpc_ui_callback(BT_FTPCUI_BROWSED, (void *) FS_ERR_FOLDER_ERR);
					goto FTPC_GET_FOLDER_ERROR;

				} else {
					(act_client_cntx_p + i)->fh_ftpc_recv = h;
				}

				bt_ftpc_reset_abort_req_sent_flag();
			}

			// Write object context to file
			if (rsp->frag_len > 0) {
				U32 len_written;

				if (FALSE != btmtk_goep_is_valid_handle((act_client_cntx_p + i)->fh_ftpc_recv)) {
					ret = btmtk_goep_write_wstream((act_client_cntx_p + i)->fh_ftpc_recv,
							rsp->frag_ptr,
							rsp->frag_len,
							(S32*) & len_written);

					GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_GET_FOLDER_WRITE, (act_client_cntx_p + i)->fh_ftpc_recv, ret) );
					if (ret < 0) {
						// FTP_DEBUG(kal_print("FTPC GET FOLDER -- btmtk_goep_write_wstream ERROR\n");)
						btmtk_ftpc_ui_callback(BT_FTPCUI_BROWSED, (void *) FS_ERR_WRITE_FILE_FAIL);
						goto FTPC_GET_FOLDER_ERROR;
					}

				} else {
					//FTP_DEBUG(kal_print("FTPC GET FOLDER ERROR -- WRITE OBJ BUT FH IN CNTX IS NULL\n"););
					BT_FTP_ASSERT(0);
				}
			}

			// Final packet, Receive Finished !!
			if ((rsp->pkt_type == GOEP_FINAL_PKT) || (rsp->pkt_type == GOEP_SINGLE_PKT)) {
				//FTP_DEBUG(kal_print("FTPC GET FOLDER FINAL PKT\n"););
				ftpc_cntx_p->ftpc_flag = 0;

				// close file
				if (FALSE !=  btmtk_goep_is_valid_handle((act_client_cntx_p + i)->fh_ftpc_recv)) {
					GOEP_TRACE((BT_FTP_TRC_CLASS,BT_FTPC_CLOSE_RECV_FILE));
					btmtk_goep_close_wstream((act_client_cntx_p + i)->fh_ftpc_recv);
					(act_client_cntx_p + i)->fh_ftpc_recv = (FHANDLE) FHANDLE_INVALID_VALUE;
				}

				BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);
				// BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_PARSING_FOLDER);
				// parse folder content
				btmtk_ftpc_ui_callback( BT_FTPCUI_BROWSED, (void *) GOEP_STATUS_SUCCESS);

			} else {
				//FTP_DEBUG(kal_print("FTPC GET FOLDER NOT FINAL PKT -- SEND REQ\n"););
				// Get next list folder object body
				bt_ftpc_send_pull_folder_continue_msg(0);
				btmtk_ftpc_ui_callback(BT_FTPCUI_BROWSING, 0);
			}

			return;
		}

		default:
		{
			btmtk_ftpc_ui_callback( BT_FTPCUI_BROWSED, (void *) (U32) rsp->rsp_code);
			goto FTPC_GET_FOLDER_ERROR;
		}
		break;
	}
	return;

FTPC_GET_FOLDER_ERROR:
	{
		// close file and delete local folder-listing object
		if (FALSE != ext_fs_is_valid_handle_ucs2(act_client_cntx_p->fh_ftpc_recv)) {
			// GOEP_TRACE((BT_FTP_TRC_CLASS,BT_FTPC_CLOSE_RECV_FILE));
			btmtk_goep_close_wstream((act_client_cntx_p + i)->fh_ftpc_recv);
			(act_client_cntx_p + i)->fh_ftpc_recv = (FHANDLE) FHANDLE_INVALID_VALUE;
			GOEP_TRACE_PATH( BT_FTP_TRC_CLASS, BT_FTPC_DEL_RECV_FILE, (act_client_cntx_p + i)->folder_obj );
			// btmtk_goep_delete_file((const U16*)(act_client_cntx_p + i)->folder_obj);
		}

		// If any error occurs when getting folder content, just delete the file.
		btmtk_goep_delete_file((const U16*)(act_client_cntx_p + i)->folder_obj);

		// Not last packet of folder content, we have to send abort out
		if (GOEP_STATUS_SUCCESS != rsp->rsp_code) { 
			// APPI: del ftp server's current uploading folder cause return fail at normal pkt.
			BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_CONNECTED);

		} else {
			if ((rsp->pkt_type == GOEP_SINGLE_PKT) || (rsp->pkt_type == GOEP_FINAL_PKT)) {
				BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_CONNECTED);
			} else {
				// send abort packet
				bt_ftpc_reset_abort_req_sent_flag();
				bt_ftpc_send_abort_message(rsp->goep_conn_id);

				// add disconnecting screen to history
				BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_ABORTING);
				// bt_ftpc_AddHistory(SCR_FTP_ABORTING);
				btmtk_ftpc_ui_callback(BT_FTPCUI_ABORTING, 0);
			}
		}
		return;
	}
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_get_obj
 * DESCRIPTION
 *  This function is to handel the getting obj operation
 * PARAMETERS
 *  rsp     [?]         
 *  i       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftpc_get_obj(U32 i, goep_pull_cnf_struct *rsp)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	int ret;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_GET_OBJ, rsp->rsp_code, rsp->pkt_type));

	switch (rsp->rsp_code) {

		case GOEP_STATUS_SUCCESS: {
			if ((rsp->pkt_type == GOEP_FIRST_PKT) || (rsp->pkt_type == GOEP_SINGLE_PKT)) {
				// build default path
				if (0 == ext_ucs2strlen((const S8 *) (act_client_cntx_p + i)->push_local_path)) {
					GOEP_Report("[ftpc][err] func:get_obj local path is empty !");
					btmtk_goep_create_folder((const U16*) (act_client_cntx_p + i)->push_local_path);
				}

				if (rsp->total_obj_len > 0) {
					(act_client_cntx_p + i)->ftpc_total_len = rsp->total_obj_len;
					(act_client_cntx_p + i)->ftpc_reamin_len = rsp->total_obj_len;
					(act_client_cntx_p + i)->total_len_type = BT_FTPC_TOTAL_LEN_NO_ZERO;

				} else {
					// NOTE ! CAN NOT KNOW THE TOTOAL FILESIZE
					GOEP_Report("[FTPC][WRN] func: get_obj total_len is 0 ( unknow file size ) ! \n");
					(act_client_cntx_p + i)->ftpc_total_len = 0;
					(act_client_cntx_p + i)->total_len_type = BT_FTPC_TOTAL_LEN_ZERO;
				}

				// write file or nbuffer
				ret = bt_ftpc_write_obj((U8)i, rsp);
				if (ret < 0) {
					goto FTPC_GET_OBJ_ERR;
				}

				return;
				// end of first packet
			}

			// write file or nbuffer, has ui_callback inside.
			ret = bt_ftpc_write_obj_continue((U8)0, rsp);
			if (ret < 0) {
				// FTP_DEBUG(kal_print("FTPC GET OBJ( write obj continue fail )\n"););
				goto FTPC_GET_OBJ_ERR;
			}
			return;
		}

		default:
			// GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, (act_client_cntx_p+i)->ftpc_state) ); 
			// not wrong state. it is the return value is fail.
			// FTP_DEBUG(kal_print("FTPC GET OBJ RSP ERROR\n");)
			if ((act_client_cntx_p+i)->ftpc_state != BT_FTPC_STATE_IDLE) {
				// TODO: UI bt_ftpc_notify_recv_err(BT_FTPC_NOTIFY_REJECT_FROM_SERVER);
			}

			if (FALSE != btmtk_goep_is_valid_handle((act_client_cntx_p + i)->fh_ftpc_recv )) {
				GOEP_TRACE((BT_FTP_TRC_CLASS,BT_FTPC_CLOSE_RECV_FILE));
				btmtk_goep_close_wstream((act_client_cntx_p + i)->fh_ftpc_recv);
				(act_client_cntx_p + i)->fh_ftpc_recv = (FHANDLE) FHANDLE_INVALID_VALUE;
				// bt_ftpc_delete_pulling_file(0);
				btmtk_goep_delete_file((const U16*) act_client_cntx_p->ftpc_filepath);
				btmtk_goep_clear_filepath((U8 *) act_client_cntx_p->ftpc_filepath);
			}

			btmtk_ftpc_ui_callback(BT_FTPCUI_PULLED, (void *) (rsp->reason == 0 ? 1 : rsp->reason));
			BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);
	}
	return;

FTPC_GET_OBJ_ERR:
	// reset variables after send abort_req
	btmtk_ftpc_receiving_aborted_routine(0);
	BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);    

	GOEP_Report("[FTPC] get_obj_err rsp:%d fread-ret:%d", rsp->rsp_code, ret);
	if ((rsp->pkt_type == GOEP_FINAL_PKT) || (rsp->pkt_type == GOEP_SINGLE_PKT)) {
		btmtk_ftpc_ui_callback(BT_FTPCUI_PULLED, (void *) (rsp->rsp_code != 0 ? rsp->rsp_code : ret));

	} else {
		// not finish. send abort to notify server the pull has been stoped.
		bt_ftpc_reset_abort_req_sent_flag();
		bt_ftpc_send_abort_message(rsp->goep_conn_id);
		BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_ABORTING);

		// no ui_callback. Wait for abort_cnf to call pulled UIEVENT
	}
	// notify the user the fs error
	ftpc_cntx_p->ftpc_flag = 0;
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_parse_folder_content
 * DESCRIPTION
 *  This function is to parse the folder content
 * PARAMETERS
 *  i       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static int bt_ftpc_parse_folder_content(U32 i, const U8 *ucFilePath)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	int result = 0;
	//TODO parsing xml. APPI use itself parser 

	return result;
}




/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_set_fs_flag
 * DESCRIPTION
 *  
 * PARAMETERS
 *  index       [IN]        
 * RETURNS
 *  U32 index
 *****************************************************************************/
void bt_ftpc_set_fs_flag(int ret)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //GOEP_TRACE(BT_FTP_TRC_CLASS, BT_FTPC_SET_FS_FLAG, ret);

    ftpc_cntx_p->ftpc_flag = 0;
    //TODO setup the error
#if 0    
    if (ret == FS_DISK_FULL)
    {
        BT_FTPC_SET_FLAG(FS_ERR_DISK_FULL);
    }
#endif
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_fldrinfo_rollback
 * DESCRIPTION
 *  This function is to update folder info when not all parsed out result can
 *  be written into file
 * PARAMETERS
 *  buff            [IN]            parsed result buffer
 *  buflen          [IN]            buffer length to be written
 *  written         [IN]            data length has been written
 * RETURNS
 *  NONE
 *****************************************************************************/
static void bt_ftpc_fldrinfo_rollback(const S8 *buff, U32 buflen, U32 written)
{
	U32 fcnt = 0, total_fcnt = 0, total_cnt, avail_cnt, i;
	bt_ftp_obj_entity_struct *entity = (bt_ftp_obj_entity_struct *)buff;

	total_cnt = buflen/sizeof(*entity);
	avail_cnt = written/sizeof(*entity);
	act_client_cntx_p[0].entity_cnt_cur_folder -= (U16)(total_cnt - avail_cnt);

	for (i = 0; i < total_cnt; i++)
	{
		if (entity[i].type == BT_FTP_OBJ_TYPE_FILE)
		{
			total_fcnt ++;
			if (i < avail_cnt)
			{
				fcnt ++;
			}
		}
	}

#ifdef __BT_FTC_MULTIPULL_SUPPORT__
	act_client_cntx_p[0].file_cnt_cur_folder -= (total_fcnt - fcnt);
#endif
}


#endif
 
//#define END_FTP_CORE_FUNCTION

//#define FTP_EVENT_HDLR




/****************** Routine *****************/

/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_multi_file_continue
 * DESCRIPTION
 *  
 * PARAMETERS
 *  i       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftpc_receiving_aborted_routine(U32 i)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_ABORTED_HDL) );

	// release fh before abort_req send
	if((act_client_cntx_p+i)->fh_ftpc_recv)
	{
		btmtk_goep_close_wstream((act_client_cntx_p+i)->fh_ftpc_recv);
		(act_client_cntx_p + i)->fh_ftpc_recv = (FHANDLE) FHANDLE_INVALID_VALUE;
		btmtk_goep_delete_file((const U16*) act_client_cntx_p->ftpc_filepath);
		btmtk_goep_clear_filepath((U8 *) act_client_cntx_p->ftpc_filepath);
	}
	// bt_ftpc_delete_pulling_file(0);

	act_client_cntx_p->ftpc_total_len = 0;
	act_client_cntx_p->ftpc_reamin_len = 0;
	(act_client_cntx_p)->ftpc_data_len = 0;
	(act_client_cntx_p)->total_len_type = BT_FTPC_TOTAL_LEN_NO_FIRST_PKT;

	bt_ftpc_reset_abort_req_sent_flag();
}


/**
* @brief Abort the current request
***/
void bt_ftpc_set_abort_req_sent_flag(void)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_SET_FLAG_ABORT_REQ_SENT) );

	act_client_cntx_p->flag_abort_req_sent = TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_reset_abort_req_sent_flag
 * DESCRIPTION
 *  
 * PARAMETERS
 *  i       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftpc_reset_abort_req_sent_flag(void)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_RESET_FLAG_ABORT_REQ_SENT));

	act_client_cntx_p->flag_abort_req_sent = FALSE;
}

void bt_ftpc_connection_terminated(U8 index, U8 role) {

	/* 
	 * Getting folder:  open temp xhtml file;
	 * Parsing: open parsed file, (temp xhtml file is opened by xml parser;  (parsing state)
	 * Parsed achieved: temp xhtml file is deleted;
	 * 
	 * Getting file: open a temp file;
	 */

	// temp file for getting obj
	// bt_ftpc_delete_pulling_file(0);

	// Receiving
	if (FALSE != btmtk_goep_is_valid_handle((act_client_cntx_p + index)->fh_ftpc_recv)) {
		GOEP_TRACE((BT_FTP_TRC_CLASS,BT_FTPC_CLOSE_RECV_FILE));
		// lost RF signal
		btmtk_goep_close_wstream((act_client_cntx_p+index)->fh_ftpc_recv);
		(act_client_cntx_p + index)->fh_ftpc_recv = (FHANDLE) FHANDLE_INVALID_VALUE;

		//delete it by the name in 
		btmtk_goep_delete_file((const U16*) act_client_cntx_p->ftpc_filepath);
		btmtk_goep_clear_filepath((U8 *) act_client_cntx_p->ftpc_filepath);
		btmtk_ftpc_ui_callback(BT_FTPCUI_PULLED, (void *) GOEP_USER_CANCEL);
	}

	if (FALSE != btmtk_goep_is_valid_handle((act_client_cntx_p + index)->fh_ftpc_push)) {
		GOEP_TRACE((BT_FTP_TRC_CLASS,BT_FTPC_CLOSE_PUSH_FILE));
		// lost RF signal
		btmtk_goep_close_rstream((act_client_cntx_p+index)->fh_ftpc_push);
		(act_client_cntx_p + index)->fh_ftpc_push = (FHANDLE) FHANDLE_INVALID_VALUE;
		btmtk_ftpc_ui_callback(BT_FTPCUI_PUSHED, (void *) GOEP_USER_CANCEL);
	}

	// reset the flag, False: abort_req not sent
	bt_ftpc_reset_abort_req_sent_flag();

	ftpc_cntx_p->g_conn_id[index] = 0xFF;
	(act_client_cntx_p + index)->got_root = 0;
	(act_client_cntx_p + index)->discon_from_cm = FALSE;

	// Clear shared buffer
	GOEP_MEMSET((act_client_cntx_p + index)->ftpc_obj_buff, 0, FTPC_MEM_BUFF_SIZE);

	btmtk_ftpc_ui_callback(BT_FTPCUI_DISCONNECTED, 0); //TODO add rsp_code
	// btmtk_ftpc_ui_callback(BT_FTPCUI_ACTIVE, 0); //TODO add rsp_code    
	return;
}


BT_BOOL bt_ftpc_delete_folder_routine(const U16 *l_u2FolderName){ //const U16* foldername) {
	goep_push_req_struct *req;

	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_MEMSET((U8 *)act_client_cntx_p->ftpc_obj_name, 0, sizeof(act_client_cntx_p->ftpc_obj_name));

	ext_ucs2ncpy((S8 *) act_client_cntx_p->ftpc_obj_name,
			(S8 *)l_u2FolderName,
			(sizeof(act_client_cntx_p->ftpc_obj_name)-2)/2);

	GOEP_TRACE_PATH(BT_FTP_TRC_CLASS, BT_FTPC_ACTION_DELETE_FOLDER, act_client_cntx_p->ftpc_obj_name);

	req = (goep_push_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_push_req_struct), TD_CTRL);
	GOEP_MEMSET( (U8 *)req, 0, sizeof(goep_push_req_struct));

	req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
	req->pkt_type = GOEP_SINGLE_PKT;
	req->put_type = GOEP_PUT_DELETE;
	ext_ucs2ncpy((S8 *) req->obj_name, (S8 *) l_u2FolderName,GOEP_MAX_OBJ_NAME);
	bt_ftp_ucs2_htons((U8 *)req->obj_name, (U8 *)req->obj_name);
	GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
	req->uuid_len = 16;

	BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_DEL_FOLDER);
	bt_ftp_send_msg(MSG_ID_BT_FTPC_PUSH_REQ, req, sizeof(goep_push_req_struct));
	return TRUE;
}

BT_BOOL  bt_ftpc_create_folder_routine(const U16 * l_u2FolderName) {
	//(const U16* foldername){
	/// init a request to obex set-path to create remote's folder
	/// after create is success, we need to goback a level
	goep_set_folder_req_struct *req;

	//MMI_FTPC_STATE_TRANS(0, MMI_FTPC_STATE_SETTING_BACK_FOLDER);
	GOEP_MEMSET((U8 *)act_client_cntx_p->ftpc_obj_name, 0,
	sizeof(act_client_cntx_p->ftpc_obj_name));

	ext_ucs2ncpy((S8 *) act_client_cntx_p->ftpc_obj_name,
			(S8 *)l_u2FolderName,
			(sizeof(act_client_cntx_p->ftpc_obj_name)-2)/2);

	GOEP_TRACE_PATH(BT_FTP_TRC_CLASS, BT_FTPC_ACTION_CREATE_FOLDER,
			act_client_cntx_p->ftpc_obj_name);

	// construct set folder req ilm
	req = (goep_set_folder_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_set_folder_req_struct), TD_CTRL);

	// memory init
	GOEP_MEMSET((U8 *)req, 0, sizeof(goep_set_folder_req_struct));
	req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
	req->setpath_flag = GOEP_CREATE_FOLDER;
	ext_ucs2ncpy((S8 *) req->folder_name, (S8 *) act_client_cntx_p->ftpc_obj_name, GOEP_MAX_FOLDER_NAME);
	bt_ftp_ucs2_htons((U8 *)req->folder_name, (U8 *)req->folder_name);
	GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
	req->uuid_len = 16;

	// send set folder req
	BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_CREATE_FOLDER);

	bt_ftp_send_msg(MSG_ID_BT_FTPC_SET_FOLDER_REQ, req, sizeof(goep_set_folder_req_struct));
	return TRUE;
}


BT_BOOL bt_ftpc_start_pushfile_routine(const U8 *ucFolderpath,
		const U8 *ucFilename, U8 srm_enable) {
	bt_ftpc_pushfile_req_struct *req;
	U32 state = (U32)(act_client_cntx_p)->ftpc_state;

	if (state == BT_FTPC_STATE_CONNECTED) {
	} else {
		//OS_Report( "[oppc] busy ! push a file fail  state=%d", (U32) ( (act_client_cntx_p + i)->ftpc_state) );
		return FALSE;
	}

	bt_ftp_compose_path((const U8 *) ucFolderpath,
			(const U8 *) ucFilename, (U8 *) act_client_cntx_p->ftpc_filepath,
			sizeof(act_client_cntx_p->ftpc_filepath) / 2);
	btmtk_goep_get_file_basename((const U8 *) act_client_cntx_p->ftpc_filepath,
			(U8 *) act_client_cntx_p->ftpc_obj_name,
			sizeof(act_client_cntx_p->ftpc_obj_name) / 2);

	req = (bt_ftpc_pushfile_req_struct*) CONSTRUCT_LOCAL_PARAM(
			sizeof(bt_ftpc_pushfile_req_struct), TD_CTRL);
	FTPC_MEMSET(req, sizeof(bt_ftpc_pushfile_req_struct)); /* memory init */
	req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];

	//name and msime
	ext_ucs2ncpy((S8 *) req->obj_name, (const S8 *) act_client_cntx_p->ftpc_obj_name,
			(sizeof(req->obj_name) - 2 / 2));
	bt_ftp_ucs2_htons((U8 *) req->obj_name, (U8 *) req->obj_name);

	//filepath
	ext_ucs2ncpy((S8 *) req->filepath, (const S8 *) act_client_cntx_p->ftpc_filepath,
			sizeof(req->filepath));
	req->filepath_len = ext_ucs2strlen((const S8 *) req->filepath) * 2;
	req->srm_enable = srm_enable;

	BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_PUSHING_OBJ);
	bt_ftp_send_msg(MSG_ID_BT_FTPC_PUSHFILE_REQ, req,
			sizeof(bt_ftpc_pushfile_req_struct));

	return TRUE;
}

BT_BOOL bt_ftpc_start_push_a_file_routine(const U8 *ucFolderpath, const U8 *ucFilename) {
	goep_push_req_struct *req;
	U32 i4Filesize;

	U32 state = (U32) (act_client_cntx_p)->ftpc_state;
	GOEP_TRACE((BT_FTP_TRC_CLASS,BT_FTPC_ACTION_PUSH_FILE, state));

	if (state == BT_FTPC_STATE_CONNECTED) {
	} else {
		// GOEP_Report( "[oppc] busy! push a file fail. state=%d", (U32) ((act_client_cntx_p + i)->ftpc_state));
		GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_ACTION_PUSH_FILE_FAIL, state, 0));
		return FALSE;
	}

	bt_ftp_compose_path((const U8 *) ucFolderpath,
			(const U8 *) ucFilename,
			(U8 *) act_client_cntx_p->ftpc_filepath,
			sizeof(act_client_cntx_p->ftpc_filepath)/2);

	GOEP_TRACE_PATH(BT_FTP_TRC_CLASS, BT_FTPC_TARGET_FILEPATH, act_client_cntx_p->ftpc_filepath);

	// check file
	if (FALSE == btmtk_goep_is_file_exist((const U16 *) act_client_cntx_p->ftpc_filepath)) {
		GOEP_TRACE((BT_FTP_TRC_CLASS,
				BT_FTPC_ACTION_PUSH_FILE_FAIL,
				state,
				btmtk_goep_is_file_exist((const U16 *)act_client_cntx_p->ftpc_filepath)));
		return FALSE;

	} else {
		// file exist 
		if (ext_ucs2strlen((S8 *)ucFilename) > (FMGR_MAX_PATH_LEN - 2)) {
			GOEP_TRACE((BT_FTP_TRC_CLASS,BT_FTPC_ACTION_PUSH_FILE_CHECKSIZE_FAIL));
			return FALSE;
		}

		ext_ucs2ncpy((S8*) act_client_cntx_p->ftpc_obj_name, (S8 *)ucFilename, FMGR_MAX_PATH_LEN);

		/// if filename is NULL, check the ucFolderpath has a filename or not
		if (NULL == ucFilename || 0 == ext_ucs2strlen((const S8 *) ucFilename))	{
			if (EXT_FS_OK != btmtk_goep_get_file_basename((const U8 *)act_client_cntx_p->ftpc_filepath,
					(U8 *) act_client_cntx_p->ftpc_obj_name, GOEP_MAX_OBJ_NAME - 2)) {

				GOEP_TRACE_PATH(BT_FTP_TRC_CLASS,
						BT_FTPC_ACTION_PUSH_FILE_GETFILENAME_FAIL,
						act_client_cntx_p->ftpc_filepath);

				return FALSE;
			}

			if (0 == ext_ucs2strlen((const S8 *)act_client_cntx_p->ftpc_obj_name)) {
				GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_ACTION_PUSH_FILE_CHECKSIZE_FAIL));
				return FALSE;
			}
		}

		// prepare data
		act_client_cntx_p->flag_abort_req_sent = FALSE;
		(act_client_cntx_p)->ftpc_fs_status = EXT_FS_OK; 
		i4Filesize = 0;

		if (FALSE == btmtk_goep_get_filesize(FHANDLE_INVALID_VALUE,
				(U8 *)act_client_cntx_p->ftpc_filepath, &i4Filesize)) {
			GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_GET_FILESIZE_FAIL));
			return FALSE;
		}

		if (0 < i4Filesize) {
			act_client_cntx_p->fh_ftpc_push = btmtk_goep_open_rstream((const U16 *)act_client_cntx_p->ftpc_filepath, 0);

			if (FALSE == ext_fs_is_valid_handle_ucs2(act_client_cntx_p->fh_ftpc_push)) {
				return FALSE;
			}

			if (FALSE != act_client_cntx_p->ftpc_use_nbuffer) {
				ext_nbuffer_cleanup(&act_client_cntx_p->ftpc_mempool);
			}

			act_client_cntx_p->ftpc_total_len = i4Filesize;
			act_client_cntx_p->ftpc_data_len = 0;
			act_client_cntx_p->ftpc_reamin_len = act_client_cntx_p->ftpc_push_remain = i4Filesize;

			if (TRUE == (act_client_cntx_p)->ftpc_use_nbuffer) {
				bt_ftp_send_msg_toself(MSG_ID_BT_FTPC_CLIENT_INTERNAL_RW, NULL, 0);
			}

		} else if (0 == i4Filesize) {
			act_client_cntx_p->ftpc_total_len = 0;
			act_client_cntx_p->ftpc_data_len = 0;
			act_client_cntx_p->ftpc_reamin_len = act_client_cntx_p->ftpc_push_remain = 0;
		}
	}

	req = (goep_push_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_push_req_struct), TD_CTRL);

	// memory init
	GOEP_MEMSET((U8 *)req, 0, sizeof(goep_push_req_struct));
	req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];

	if (TRUE) { // i4Filesize > 0
		req->pkt_type = GOEP_FIRST_PKT; 
	} else {
		// Current Single is not ready. Avoid to use it. Even push a empty file
		req->pkt_type = GOEP_SINGLE_PKT;
	}

	req->put_type = GOEP_PUT_NORMAL;
	req->total_obj_len = i4Filesize;
	req->frag_ptr = act_client_cntx_p->ftpc_obj_buff;
	req->frag_len = 0;
	GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
	req->uuid_len = 16;

	// name and mime
	ext_ucs2ncpy((S8 *) req->obj_name, (const S8*) act_client_cntx_p->ftpc_obj_name, (sizeof(req->obj_name)-2)/2);
	bt_ftp_ucs2_htons((U8 *)req->obj_name, (U8 *)req->obj_name);

	BT_FTPC_STATE_TRANS(0,BT_FTPC_STATE_PUSHING_OBJ);
	bt_ftp_send_msg(MSG_ID_BT_FTPC_PUSH_REQ, req, sizeof(goep_push_req_struct));

	return TRUE;
}

BT_BOOL bt_ftpc_start_single_pushfile_routine(const U8 *ucFolderpath,
		const U8 *ucFilename) {
	bt_ftpc_pushfile_req_struct *req;

	U32 state = (U32)(act_client_cntx_p)->ftpc_state;

	if (state == BT_FTPC_STATE_CONNECTED) {
	} else {
		//OS_Report( "[oppc] busy ! push a file fail  state=%d", (U32) ( (act_client_cntx_p + i)->ftpc_state) );
		return FALSE;
	}
	OS_Report("[FTPC] bt_ftpc_start_single_pushfile_routine");

	btmtk_goep_compose_filepath((const U8 *) ucFolderpath,
			(const U8 *) ucFilename, (U8 *) act_client_cntx_p->ftpc_filepath,
			sizeof(act_client_cntx_p->ftpc_filepath) / 2);

	req = (bt_ftpc_pushfile_req_struct*) CONSTRUCT_LOCAL_PARAM(
			sizeof(bt_ftpc_pushfile_req_struct), TD_CTRL);
	FTPC_MEMSET(req, sizeof(bt_ftpc_pushfile_req_struct)); /* memory init */
	req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];

	//name and mime
	ext_ucs2ncpy((S8 *) req->obj_name, (const S8 *) act_client_cntx_p->ftpc_obj_name,
			(sizeof(req->obj_name) - 2 / 2));
	bt_ftp_ucs2_htons((U8 *) req->obj_name, (U8 *) req->obj_name);

	BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_PUSHING_OBJ);
	bt_ftp_send_msg(MSG_ID_BT_FTPC_PUSHFILE_REQ, req,
			sizeof(bt_ftpc_pushfile_req_struct));

	return TRUE;
}

S32 bt_ftpc_read_rstream_routine(U8 i, U32 *pi4Read){
	S32 ret;
	U32 len_read = 0;
	BT_BOOL bSendSelf = FALSE;

	ret = (act_client_cntx_p + i)->ftpc_fs_status;
	*pi4Read = 0;

	if (EXT_FS_OK == ret || EXT_FS_ERR_EOF == ret) {
		if (FALSE != ext_nbuffer_isFull(&(act_client_cntx_p + i)->ftpc_mempool)) {
			bSendSelf = TRUE;
		}

		if (FALSE != ext_nbuffer_isEmpty(&(act_client_cntx_p + i)->ftpc_mempool)) {
			bSendSelf = TRUE;
			(act_client_cntx_p + i)->ftpc_fs_status = btmtk_goep_read_rstream (
					(act_client_cntx_p + i)->fh_ftpc_push, // open file's handle
					(act_client_cntx_p + i)->ftpc_obj_buff, // pull buffer
					(act_client_cntx_p + i)->ftpc_peer_mru, // remote max packet size. 2048 test pass
					(S32*) &len_read);

		} else {
			if (FALSE == ext_nbuffer_pop_data(&(act_client_cntx_p + i)->ftpc_mempool,
					(act_client_cntx_p+i)->ftpc_obj_buff,
					(act_client_cntx_p+i)->ftpc_obj_buff_size,(S32*) &len_read)) {

				GOEP_Report("[ftps][err] pop data return FALSE");
				(act_client_cntx_p+i)->ftpc_fs_status = EXT_FS_ERR;
				return (act_client_cntx_p+i)->ftpc_fs_status;
			}
		}

		*pi4Read = len_read;
		if (FALSE != (act_client_cntx_p + i)->ftpc_use_nbuffer && FALSE != bSendSelf) {
			bt_ftp_send_msg_toself(MSG_ID_BT_FTPC_CLIENT_INTERNAL_RW, NULL, 0);
		}
	}

	return ((act_client_cntx_p+i)->ftpc_fs_status);
}

void bt_ftpc_pushing_file_routine(void *msg) {
	goep_push_cnf_struct *cnf;
	goep_push_req_struct *req;
	S32 i4Read;
	S32 i4ret;

	BT_BOOL l_bEof = FALSE;
	cnf = (goep_push_cnf_struct* ) msg;

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_PUSH_FILE_HDL, cnf->rsp_code,
			act_client_cntx_p->ftpc_state, act_client_cntx_p->fh_ftpc_push));

	/// check status
	if (GOEP_STATUS_SUCCESS == cnf->rsp_code) {
		// push the next file
		GOEP_TRACE((BT_FTP_TRC_CLASS,
				BT_FTPC_FILE_STATE_CHECK,
				(act_client_cntx_p)->ftpc_fs_status,
				(act_client_cntx_p)->ftpc_total_len,
				(act_client_cntx_p)->ftpc_push_remain,
				(act_client_cntx_p)->fh_ftpc_push));

		if (act_client_cntx_p->ftpc_push_remain > 0) {
			// read data from nbuffer then file

			if (FALSE != btmtk_goep_is_valid_handle((act_client_cntx_p)->fh_ftpc_push)) {
				i4ret = bt_ftpc_read_rstream_routine(0, (U32*) &i4Read);

				if (EXT_FS_OK != (act_client_cntx_p)->ftpc_fs_status &&
					EXT_FS_ERR_EOF != (act_client_cntx_p)->ftpc_fs_status) {
					goto pushfile_error;

				} else {
					//read more than remain. use the remain size
					if ((U32) i4Read > act_client_cntx_p->ftpc_push_remain) {
						i4Read = act_client_cntx_p->ftpc_push_remain;
						GOEP_Report("[ftpc][wrn] read more data !\n");
					}

					act_client_cntx_p->ftpc_push_remain -= i4Read;

					if (0 == i4Read) {
						goto pushfile_error;
					}
				}

			} else {
				goto pushfile_error;
			}

			req = (goep_push_req_struct*) CONSTRUCT_LOCAL_PARAM (sizeof(goep_push_req_struct), TD_CTRL);

			// memory init
			GOEP_MEMSET((U8 *)req, 0, sizeof(goep_push_req_struct));
			req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
			if (FALSE == l_bEof && act_client_cntx_p->ftpc_push_remain > 0) {
				req->pkt_type = GOEP_NORMAL_PKT;
				BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_PUSHING_OBJ);

			} else {
				req->pkt_type = GOEP_FINAL_PKT;

				// close the file
				if (FALSE != btmtk_goep_is_valid_handle((act_client_cntx_p)->fh_ftpc_push)) {
					GOEP_TRACE((BT_FTP_TRC_CLASS,BT_FTPC_CLOSE_PUSH_FILE));
					btmtk_goep_close_rstream((act_client_cntx_p)->fh_ftpc_push);
					(act_client_cntx_p)->fh_ftpc_push = FHANDLE_INVALID_VALUE;
				}
			}

			req->frag_len = i4Read;
			req->frag_ptr = act_client_cntx_p->ftpc_obj_buff;
			GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
			req->uuid_len = 16;
			bt_ftp_send_msg(MSG_ID_BT_FTPC_PUSH_REQ, req, sizeof(goep_push_req_struct));
			
			btmtk_ftpc_ui_callback(BT_FTPCUI_PUSHING,
					(void *) (act_client_cntx_p->ftpc_total_len - act_client_cntx_p->ftpc_push_remain));

		} else {
			// send done. one more callback is passed (after GOEP_FINAL_PKT)
			btmtk_ftpc_ui_callback(BT_FTPCUI_PUSHED, (void *) (U32) cnf->rsp_code);

			goto pushfile_done;
		}

	} else {
		/// UI notification error status
		btmtk_ftpc_ui_callback(BT_FTPCUI_PUSHED, (void *) (cnf->reason != 0 ? cnf->reason : 1));
		goto pushfile_done;
	}
	return;

pushfile_error:
	// send out the abort 
	// send abort packet
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_ACTION_PUSH_FILE_FAIL));
	bt_ftpc_reset_abort_req_sent_flag();
	bt_ftpc_send_abort_message(cnf->goep_conn_id);
	// add disconnecting screen to history
	BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_ABORTING);
	// bt_ftpc_AddHistory(SCR_FTP_ABORTING);
	if (FALSE != ext_fs_is_valid_handle_ucs2((act_client_cntx_p)->fh_ftpc_push)) {
		GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_CLOSE_PUSH_FILE) );
		btmtk_goep_close_rstream((act_client_cntx_p)->fh_ftpc_push);
		(act_client_cntx_p)->fh_ftpc_push = FHANDLE_INVALID_VALUE;
	}
	btmtk_ftpc_ui_callback(BT_FTPCUI_ABORTING, 0);
	return;

pushfile_done:
	if (FALSE != ext_fs_is_valid_handle_ucs2((act_client_cntx_p)->fh_ftpc_push)) {
		GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_CLOSE_PUSH_FILE));
		btmtk_goep_close_rstream((act_client_cntx_p)->fh_ftpc_push);
		(act_client_cntx_p)->fh_ftpc_push = FHANDLE_INVALID_VALUE;
	}
	BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_CONNECTED);
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_ACTION_PUSH_FILE_DONE) );
}

/*************************************** Internal function ************************/
static void bt_ftpc_flush_one_nbuffer_to_file() {
	S32 i4Written;

	// write all the mem block in nbufer
	if (FALSE != ext_nbuffer_pop_data_to_file(&act_client_cntx_p->ftpc_mempool,
			act_client_cntx_p->fh_ftpc_recv,
			(U32 *) &act_client_cntx_p->ftpc_fs_status,
			(U32 *) &i4Written)) {

		if (EXT_FS_OK != act_client_cntx_p->ftpc_fs_status) {
			return;
		}

	} else {

	}
}

static void bt_ftpc_flush_all_nbuffer_to_file() {
	S32 i4Written;
	U8  index;
	// write all the mem block in nbufer
	for (index = 0; index < NBUFFER_NUM_OF_MEM; index++) {
		//if( FALSE != ext_nbuffer_pop_data( &act_server_cntx_p->ftps_mempool, GOPP(ops_push_buff), OPPS_MAX_OBEX_PACKET_LENGTH, &i4Read) ){

		if (FALSE != ext_nbuffer_pop_data_to_file(&act_client_cntx_p->ftpc_mempool,
				act_client_cntx_p->fh_ftpc_recv,
				(U32 *) &act_client_cntx_p->ftpc_fs_status,
				(U32 *) &i4Written)) {

			if (EXT_FS_OK != act_client_cntx_p->ftpc_fs_status) {
				return;
			}

		} else {
			break;
		}
	}
}

static void bt_ftpc_send_pull_folder_continue_msg(){
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	goep_pull_req_struct *req;

	// send get current folder req
	// construct get folder req ilm
	req = (goep_pull_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_pull_req_struct),TD_CTRL);
	GOEP_MEMSET( (U8 *)req, 0, sizeof(goep_pull_req_struct));
	req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
	req->pkt_type = GOEP_NORMAL_PKT;
	GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
	req->uuid_len = 16;

	// send get folder req
	bt_ftp_send_msg(MSG_ID_BT_FTPC_PULL_REQ, req ,sizeof(goep_pull_req_struct));
}

void bt_ftpc_send_auth_req_msg(U8 goep_conn_id,const U8 *realm,const U8 *ucPwd, U32 u4PwdLen)
{
	goep_auth_req_struct *req;
	req = (goep_auth_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_auth_req_struct),TD_CTRL);
	GOEP_MEMSET( (U8 *)req, 0, sizeof(goep_auth_req_struct));

	req->goep_conn_id = goep_conn_id;
	req->uuid_len = sizeof(OBEX_FTP_UUID) ;
	GOEP_MEMCPY( (void *)req->uuid, (void *)OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID) );

	ext_strncpy( (S8 *) req->realm_str, (S8 *)realm, sizeof(req->realm_str) );
	req->realm_len = (U8)ext_strlen((const S8 *) req->realm_str);

	GOEP_MEMCPY( (void *)req->passwd, (void *)ucPwd, u4PwdLen );
	req->passwd_len = (U8) u4PwdLen;
	req->goep_role = GOEP_CLIENT_ROLE;

//	bt_ftp_send_msg(MSG_ID_BT_FTPC_AUTH_REQ, req , sizeof(goep_auth_req_struct) );
}

void bt_ftpc_send_auth_rsp_msg(U8 goep_conn_id, const U8 *ucUserId, const U8 *ucPwd, U32 u4PwdLen)
{
	goep_auth_rsp_struct *rsp;

	rsp = (goep_auth_rsp_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_auth_rsp_struct),TD_CTRL);
	GOEP_MEMSET((U8 *)rsp, 0, sizeof(goep_auth_rsp_struct));

	rsp->goep_conn_id = goep_conn_id;
	rsp->uuid_len = sizeof(OBEX_FTP_UUID);
	GOEP_MEMCPY((void *)rsp->uuid, (void *)OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID));

	ext_strncpy((S8 *) rsp->userid, (S8 *)ucUserId, sizeof(rsp->userid));
	rsp->userid_len = (U8) ext_strlen((const S8 *) rsp->userid);
	GOEP_MEMCPY((void *)rsp->passwd, (void *)ucPwd, u4PwdLen);
	rsp->passwd_len = (U8) u4PwdLen;

	bt_ftp_send_msg(MSG_ID_BT_FTPC_AUTH_RSP, rsp , sizeof(goep_auth_rsp_struct));
}

/**
* @brief This function is to get the folder content
* FTP Client Browse Directory
* @return TRUE send request success. Wait for BT_FTPCUI_BROWSED event
**/
BT_BOOL bt_ftpc_start_get_folder_content_routine(U8 index)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	goep_pull_req_struct *req;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_GET_FOLDER_CONTENT, act_client_cntx_p->ftpc_state, ftpc_cntx_p->goep_conn_id[0]));

	// Change MMI FTPC state
	switch (act_client_cntx_p->ftpc_state) {
		case BT_FTPC_STATE_CONNECTED:
			BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_GETTING_FOLDER);

			// send get current folder req
			// construct get folder req ilm
			req = (goep_pull_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_pull_req_struct),TD_CTRL);
			GOEP_MEMSET( (U8 *)req, 0, sizeof(goep_pull_req_struct));

			req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
			req->pkt_type = GOEP_FIRST_PKT;
			ext_strncpy((S8 *) req->obj_mime_type, "x-obex/folder-listing", GOEP_MAX_MIME_TYPE);
			req->uuid_len = sizeof(OBEX_FTP_UUID) ;
			GOEP_MEMCPY((void *)req->uuid, (void *)OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID));

			// send get folder req
			bt_ftp_send_msg(MSG_ID_BT_FTPC_PULL_REQ, req ,sizeof(goep_pull_req_struct));
			break;

		// Incorrect state, callback and return error
		default:
			GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));
			return FALSE;
	}

	return TRUE;
}

/***************************** Client ************************/
void btmtk_ftpc_push_cnf_handler(void *msg) {

	/// send a push req to continue or send UI notification
	/// read a file until eof
	BT_BOOL l_bEof = TRUE;
	U32 state = (U32)(act_client_cntx_p->ftpc_state);
	goep_push_cnf_struct *cnf;
	cnf = (goep_push_cnf_struct *) msg;

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_PUSH_CNF_HDLR, cnf->rsp_code, state, act_client_cntx_p->flag_abort_req_sent));

	switch (state) {
		case BT_FTPC_STATE_DEL_FOLDER:
			// GOEP_Report("[oppc] delete folder ack");
			BT_FTPC_STATE_TRANS(0,BT_FTPC_STATE_CONNECTED);    
			btmtk_ftpc_ui_callback(BT_FTPCUI_FILE_DELETED, (void *) (cnf->rsp_code == 1 ? cnf->reason : cnf->rsp_code));
			break;

		case BT_FTPC_STATE_PUSHING_OBJ:
			// ABORT check
			if (TRUE == act_client_cntx_p->flag_abort_req_sent) {
				if (GOEP_STATUS_SUCCESS == cnf->rsp_code && act_client_cntx_p->ftpc_push_remain > 0) {
					goto ABORT_HANDLE;
				}
			}
			bt_ftpc_pushing_file_routine(msg);
			break;

		default:
			GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, state));
			// BT_FTP_ASSERT(0);
			break;
	}
	return;

ABORT_HANDLE:
	// send the abort obex
	bt_ftpc_reset_abort_req_sent_flag();
	bt_ftpc_send_abort_message(cnf->goep_conn_id);
	// add disconnecting screen to history
	BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_ABORTING);
	// bt_ftpc_AddHistory(SCR_FTP_ABORTING);
	btmtk_ftpc_ui_callback(BT_FTPCUI_ABORTING, 0);
}

void btmtk_ftpc_internal_rw_handler(void *data) {

	U8 *pBuffer;
	U32 u4Read, u4BuffSize, u4Index;
	static U32 interval = 0;
	static U32 u4Try = 0;

	if (FALSE  == ext_nbuffer_isInit(&act_client_cntx_p->ftpc_mempool)) {
		return;
	}    

	switch (act_client_cntx_p->ftpc_state) {
		case BT_FTPC_STATE_GETTING_OBJ:
			/// write nbuffer to file
			if (FALSE != ext_fs_is_valid_handle_ucs2(act_client_cntx_p->fh_ftpc_recv)) {
				if (FALSE == ext_nbuffer_isEmpty(&act_client_cntx_p->ftpc_mempool)) {
					bt_ftpc_flush_one_nbuffer_to_file();
					bt_ftp_send_msg_toself( MSG_ID_BT_FTPC_CLIENT_INTERNAL_RW, NULL, 0);

#ifdef GOEP_TEST
					// for test
					interval = (interval + 29 )% 511; // two prime number
					btmtk_goep_sleep_test(interval);
#endif
				}
			}
			break;

		case BT_FTPC_STATE_PUSHING_OBJ:
			/// read nbuffer from file until nbuffer full
			if (FALSE != ext_fs_is_valid_handle_ucs2(act_client_cntx_p->fh_ftpc_push)) {
				if (FALSE == ext_nbuffer_isFull(&act_client_cntx_p->ftpc_mempool) &&
					EXT_FS_ERR_EOF != act_client_cntx_p->ftpc_fs_status) {

					// read data to a free buffer
					pBuffer = (U8 *) ext_nbuffer_get_tail_buffer(&act_client_cntx_p->ftpc_mempool, &u4Index, (S32 *) &u4BuffSize);
					BT_FTP_ASSERT(NULL != pBuffer);
					BT_FTP_ASSERT(u4BuffSize >= act_client_cntx_p->ftpc_peer_mru);

					if (NULL != pBuffer) {
						act_client_cntx_p->ftpc_fs_status = btmtk_goep_read_rstream(act_client_cntx_p->fh_ftpc_push,
								pBuffer, act_client_cntx_p->ftpc_peer_mru,(S32*) &u4Read);

						if (u4Read > 0) {
							ext_nbuffer_return_tail_buffer(&act_client_cntx_p->ftpc_mempool, u4Index, u4Read);
							bt_ftp_send_msg_toself(MSG_ID_BT_FTPC_CLIENT_INTERNAL_RW, NULL, 0);

						} else {
							if (0 == u4Read && EXT_FS_OK == act_client_cntx_p->ftpc_fs_status) {
								// reach eof
								act_client_cntx_p->ftpc_fs_status = EXT_FS_ERR_EOF;

							} else {
								//internal error
								btmtk_ftpc_ui_callback(BT_FTPCUI_ERROR, NULL);
							}
						}
						// send internal rw data
					}

#ifdef GOEP_TEST
					// for test
					if (0 == (u4Try++%7)) {
						interval = (interval + 290 )% 5110; // two prime number
						btmtk_goep_sleep_test(interval);
					}
#endif

				}
			}
			break;

		default:
			GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));
			break;
	}
}


void btmtk_ftpc_pushfile_cnf_handler(void *msg) {

	/// send a push req to continue or send UI notification
	/// read a file until eof
	BT_BOOL l_bEof = TRUE;
	U32 state = (U32)(act_client_cntx_p->ftpc_state);
	bt_ftpc_pushfile_cnf_struct *cnf;
	cnf = (bt_ftpc_pushfile_cnf_struct *) msg;

	switch (state) {
	case BT_FTPC_STATE_DEL_FOLDER:
		BTMTK_Report("[oppc] delete folder ack");
		BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_CONNECTED); //TODO check this status ! and pass to ui
		btmtk_ftpc_ui_callback(BT_FTPCUI_FILE_DELETED, (void *) (U32) cnf->rsp_code);
		break;
	case BT_FTPC_STATE_PUSHING_OBJ:
		// ABORT check
		act_client_cntx_p->ftpc_state = BT_FTPC_STATE_CONNECTED;
		btmtk_ftpc_ui_callback(BT_FTPCUI_PUSHED, (void *) (U32) cnf->rsp_code);
		break;
	case BT_FTPC_STATE_IDLE:
	case BT_FTPC_STATE_TPDISCONNECTING: // ALPS00247051
	case BT_FTPC_STATE_DISCONNECTING:
		// ignore it
		break;
	default:
		BT_FTP_ASSERT(0);
		break;
	}
}

void btmtk_ftpc_pullfile_cnf_handler(void *msg) {

	/// send a push req to continue or send UI notification
	/// read a file until eof
	BT_BOOL l_bEof = TRUE;
	int l_pState;
	U32 state = (U32)(act_client_cntx_p->ftpc_state);
	bt_ftpc_pullfile_cnf_struct *cnf;
	cnf = (bt_ftpc_pullfile_cnf_struct *) msg;

	l_pState = act_client_cntx_p->ftpc_state;
	
	act_client_cntx_p->ftpc_state = BT_FTPC_STATE_CONNECTED;
	
	switch( l_pState ){
		case BT_FTPC_STATE_GETTING_FOLDER:
			btmtk_ftpc_ui_callback(BT_FTPCUI_BROWSED, (void *) (U32) cnf->rsp_code);
			break;
		default:
			bt_ext_log("pullfile cnf previous state:%d", l_pState);
			btmtk_ftpc_ui_callback(BT_FTPCUI_PULLED, (void *) (U32) cnf->rsp_code);
		break;
	}
	
}

void btmtk_ftpc_file_progress_ind_handler(void *msg) {
	bt_ftpc_file_progress_ind_struct *ind;
	ind = (bt_ftpc_file_progress_ind_struct *) msg;

	OS_Report("[FTP] update total curr remain");
	act_client_cntx_p->ftpc_total_len = ind->total_length;
	(act_client_cntx_p)->ftpc_data_len = ind->curr_length;
	if( ind->total_length > ind->curr_length ){
		act_client_cntx_p->ftpc_push_remain = ind->total_length - ind->curr_length;	
	}else{
		act_client_cntx_p->ftpc_push_remain = act_client_cntx_p->ftpc_total_len;
	}
	if( ind->total_length > 0 ){
		(act_client_cntx_p)->ftpc_reamin_len = ind->total_length - ind->curr_length;
		act_client_cntx_p->total_len_type = BT_FTPC_TOTAL_LEN_NO_ZERO;
	}else{
		(act_client_cntx_p)->ftpc_reamin_len = 0xFFFFFFFF;
		act_client_cntx_p->total_len_type = BT_FTPC_TOTAL_LEN_ZERO;
	}
	// bt_ext_log("file_progress_ind len_type:%d total:%d cur:%d", act_client_cntx_p->total_len_type, act_client_cntx_p->ftpc_total_len, (act_client_cntx_p)->ftpc_data_len);

	switch (ind->operation) {
	case GOEP_OPER_PUSH:
		{
			btmtk_ftpc_ui_callback(BT_FTPCUI_PUSHING, (void *) ind->curr_length);
		}
		break;
	case GOEP_OPER_PULL:
		if( BT_FTPC_STATE_GETTING_FOLDER == act_client_cntx_p->ftpc_state ){
			btmtk_ftpc_ui_callback(BT_FTPCUI_BROWSING, (void *) ind->curr_length);
		}else{
			btmtk_ftpc_ui_callback(BT_FTPCUI_PULLING, (void *) ind->curr_length);
		}
		break;
	default:
		break;
	}
}

/**
 * When receiving abortfile_cnf msg, it means the state back to normal(connected/idle)
 */
void btmtk_ftpc_abortfile_cnf_handler(void *msg){
	/* abort fail - finish the current state */
	bt_ftpc_abortfile_cnf_struct *cnf;
	cnf = (bt_ftpc_abortfile_cnf_struct *) msg;
	bt_ext_log("abortfile cnf  state:%d - test1", (act_client_cntx_p )->ftpc_state);
	switch ((act_client_cntx_p )->ftpc_state) {
		case BT_FTPC_STATE_GETTING_FOLDER:
			BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_CONNECTED);
			
			bt_ext_log("getting folder");
			btmtk_ftpc_ui_callback(BT_FTPCUI_BROWSED,
					cnf->rsp_code!=0 ? (void *) (U32) cnf->rsp_code : (void *) GOEP_USER_CANCEL);
			bt_ext_log("getting folder end");
			break;
		case BT_FTPC_STATE_GETTING_OBJ:
			BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_CONNECTED);
			
			bt_ext_log("getting");
			btmtk_ftpc_ui_callback(BT_FTPCUI_PULLED,
					cnf->rsp_code!=0 ? (void *) (U32) cnf->rsp_code : (void *) GOEP_USER_CANCEL);
			bt_ext_log("getting end");
			break;
		case BT_FTPC_STATE_PUSHING_OBJ:
			BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_CONNECTED);
			
			bt_ext_log("pushing");
			btmtk_ftpc_ui_callback(BT_FTPCUI_PUSHED,
					cnf->rsp_code!=0 ? (void *) (U32) cnf->rsp_code : (void *) GOEP_USER_CANCEL);
			bt_ext_log("pushing end");
			break;
		case BT_FTPC_STATE_IDLE:
		case BT_FTPC_STATE_TPDISCONNECTING:
		case BT_FTPC_STATE_DISCONNECTING:
			bt_ext_log("warning ! abortfile cnf  state:%d ignore", (act_client_cntx_p )->ftpc_state);
			break;
		case BT_FTPC_STATE_ABORTING: // only in buffer style
		default:
			BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_CONNECTED);
			/* normal - abort fail */
			bt_ext_log("error ! abortfile cnf  state:%d", (act_client_cntx_p )->ftpc_state);
			break;
	}
}

void btmtk_ftpc_disconnect_ind_handler(void *data) {
	U32 i;
	goep_disconnect_ind_struct *ind = (goep_disconnect_ind_struct*) data;

	i = 0;
	if (NULL == act_client_cntx_p) {
		GOEP_Report("[FTPC][WRN] No active cntx. ignore ");
		return;
	}

	GOEP_TRACE((BT_FTP_TRC_CLASS,BT_FTPC_DISCONNECT_IND_HDLR,
			ftpc_cntx_p->goep_conn_id[0],
			ftpc_cntx_p->req_id[0],
			(act_client_cntx_p)->ftpc_state));

	// FTP_DEBUG(kal_print("DISCONNECT IND HDLR CHECK CLIENT CONNECTION\n");)
	switch ((act_client_cntx_p )->ftpc_state) {
		case BT_FTPC_STATE_IDLE:
			// When OBEX client-challenge fails, connect_rsp will fail and do a TpDisconnect.
			return;
		// other connected states
		default:
			BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_IDLE);   /* state -> active */
			// reset context
			// notify UI and post-handler            
			bt_ftpc_connection_terminated(0, 1);
			return;
	}
}

/**
* @brief Timeout to create a browse file for FTP client to fetch folder content
*  btmtk_ftpc_browse_files_failed_timer_expire_handler
**/
void btmtk_ftpc_browse_files_failed_timer_expire_handler(void)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U8 i = 0; 

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_BROWSE_FILES_TIMER_TIMEOUT_FAILED_HDLR) );
	if ((act_client_cntx_p + i)->fh_ftpc_recv)
	{
		btmtk_goep_close_wstream((act_client_cntx_p + i)->fh_ftpc_recv);
		(act_client_cntx_p + i)->fh_ftpc_recv = (FHANDLE) FHANDLE_INVALID_VALUE;
	}
	(act_client_cntx_p + i)->browse_depth = 0;

	if (act_client_cntx_p->ftpc_state != BT_FTPC_STATE_IDLE)
	{
		bt_ftpc_send_disconnect_message();
		BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_DISCONNECTING);
	}
}


/************************** Handler *********************************/

/*****************************************************************************
 * FUNCTION
 *  btmtk_ftpc_connect_cnf_handler
 * DESCRIPTION
 *  This function is to handle server MSG_ID_BT_FTPC_CONNECT_CNF msg
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftpc_connect_cnf_handler(void *msg)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i = 0;
	goep_connect_cnf_struct *rsp = (goep_connect_cnf_struct*) msg;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	// find the corresponding connection
	// i = bt_ftpc_find_goep_id(rsp->goep_conn_id);

	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_CONNECT_CNF_HDLR, rsp->rsp_code, rsp->goep_conn_id));

	for (i = 0; i < FTP_MAX_SERVER_SUPPORT; i++) {
		if (ftpc_cntx_p->req_id[i] == rsp->req_id) {
			break;
		}
	}

	if (i == FTP_MAX_SERVER_SUPPORT) {
		// without corresponding connect req for this connect rsp
		// reset
		return;
		// BT_FTP_ASSERT(0);
	}

	// Connection is established successfully
	if (rsp->rsp_code == GOEP_STATUS_SUCCESS) {
		//FTP_DEBUG(kal_print("FTPC CONNECT RSP SUCCESS\n");)
		switch ((act_client_cntx_p + i)->ftpc_state) {
			// when connect cancel, there is a conect_rsp successed in MMI queue.
			case BT_FTPC_STATE_DISCONNECTING:
				ftpc_cntx_p->goep_conn_id[i] = rsp->goep_conn_id;
				return;
				// reset

			case BT_FTPC_STATE_IDLE:
				return;

			case BT_FTPC_STATE_CONNECTING: {
				// The connection is established
				// set ftpc context
				(act_client_cntx_p + i)->browse_depth = 0;
				ftpc_cntx_p->goep_conn_id[i] = rsp->goep_conn_id;
				ftpc_cntx_p->g_conn_id[i] = rsp->cm_conn_id;

				// (act_client_cntx_p + i)->ftpc_peer_mru = rsp->peer_mru;
				(act_client_cntx_p + i)->ftpc_peer_mru =
						(rsp->peer_mru > (act_client_cntx_p + i)->ftpc_obj_buff_size) ?
						(act_client_cntx_p + i)->ftpc_obj_buff_size :
						rsp->peer_mru ;

#ifdef __BT_GOEP_V2__			
			// conntype & srm default value
			(act_client_cntx_p + i)->ftpc_remote_srm = rsp->srmdefault;
			(act_client_cntx_p + i)->ftpc_conntype = rsp->tptype;
			(act_client_cntx_p + i)->ftpc_remote_mtu = rsp->peer_mru;
#endif			

				BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED );
				btmtk_ftpc_ui_callback(BT_FTPCUI_CONNECTED, (void *) (U32) rsp->rsp_code); 
					
				// call connection cnf to CM and send first get list folder packet
				if (TRUE == btmtk_ftpc_auto_get_listing_xml()) {
					bt_ftpc_start_get_folder_content_routine(0);
				}

				break;
			}
			
			// Connection establish fail
			default:
				GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));

				// free memory if using dynamic memory mechanism
				// (act_client_cntx_p+i)->obj_buff_p = NULL;
				BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_IDLE);

				//btmtk_ftpc_ui_callback( BT_FTPCUI_ERROR, (void *) __LINE__); 
				GOEP_Report("[FTPC] wrong state:%d", (act_client_cntx_p + i)->ftpc_state );
				return;
		}

	} else {
		btmtk_ftpc_ui_callback(BT_FTPCUI_DISCONNECTED, (void *) (U32) rsp->rsp_code);

		// FTP_DEBUG(kal_print("FTPC CONNECT RSP ERROR\n");)
		switch ((act_client_cntx_p+i)->ftpc_state) {
			// when connect_cancel, the conn_rsp failed already comes
			case BT_FTPC_STATE_DISCONNECTING:
			case BT_FTPC_STATE_CONNECTING:
				BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_IDLE);
				// Notify MMI Screen
				if (rsp->rsp_code == GOEP_STATUS_SCO_REJECT) { }
				return;

			// reset
			case BT_FTPC_STATE_IDLE:
				return;

			default:
				GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, (act_client_cntx_p+i)->ftpc_state));
				BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_IDLE);
				break;
		}
	}
}

BT_BOOL bt_ftpc_should_send_abort(U8 index,  goep_pkt_type_enum pkt) {
	if (GOEP_FIRST_PKT == pkt ||  GOEP_NORMAL_PKT == pkt) {
		if (TRUE == (act_client_cntx_p+index)->flag_abort_req_sent) {
			return TRUE;
		}
	}

	return FALSE;
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_ftpc_pull_cnf_handler
 * DESCRIPTION
 *  This function is to handle server MSG_ID_BT_FTPC_PULL_CNF msg
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftpc_pull_cnf_handler(void *msg)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	goep_pull_cnf_struct *rsp = (goep_pull_cnf_struct*) msg;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	i = bt_ftpc_find_goep_id(rsp->goep_conn_id);

	GOEP_TRACE((BT_FTP_TRC_CLASS,
			BT_FTPC_PULL_CNF_HDLR,
			(act_client_cntx_p)->ftpc_state,
			(act_client_cntx_p)->flag_abort_req_sent));

	if (i == 0xff) {
		// reset
		return;
	}

	// Points to shared memory
	rsp->frag_ptr = (act_client_cntx_p + i)->ftpc_obj_buff;

	switch ((act_client_cntx_p + i)->ftpc_state) {
		case BT_FTPC_STATE_ABORTING:
			// omit all the pull_rsp after send abort_req, and wait pull_rsp only
			if (act_client_cntx_p->flag_abort_req_sent != FALSE) {
				return;

			} else {
				// omit this pkt, and send abort req, and set the flag, and reset the vars
				bt_ftpc_reset_abort_req_sent_flag();
				btmtk_ftpc_receiving_aborted_routine(0);
				bt_ftpc_send_abort_message(rsp->goep_conn_id);
				return;
			}

		case BT_FTPC_STATE_GETTING_FOLDER:
			// continue parsing
			if (TRUE == bt_ftpc_should_send_abort((U8)i, rsp->pkt_type)) {
				goto ABORT_HANDLE;
			}
			bt_ftpc_get_folder((U8)i, rsp);
			return;

		case BT_FTPC_STATE_GETTING_OBJ:
			if (TRUE == bt_ftpc_should_send_abort((U8)i, rsp->pkt_type)) {
				goto ABORT_HANDLE;
			}
			bt_ftpc_get_obj(i, rsp);
			return;

		case BT_FTPC_STATE_DISCONNECTING:
		case BT_FTPC_STATE_IDLE:
		case BT_FTPC_STATE_TPDISCONNECTING:
			// Omit it, the connection maybe lost
			return;

		// wrong states
		default:
			// error handling
			GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));

			if ((act_client_cntx_p + i)->fh_ftpc_recv) {
				btmtk_goep_close_wstream((act_client_cntx_p + i)->fh_ftpc_recv);
				(act_client_cntx_p + i)->fh_ftpc_recv = (FHANDLE) FHANDLE_INVALID_VALUE;
			}

			BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);
			// Notify MMI Screen
			GOEP_Report("[FTPC] wrong state:%d", (act_client_cntx_p + i)->ftpc_state );
			return;
	}

	return;

ABORT_HANDLE:
	// send the abort obex
	bt_ftpc_reset_abort_req_sent_flag();
	bt_ftpc_send_abort_message(rsp->goep_conn_id);
	// add disconnecting screen to history
	BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_ABORTING);
	// bt_ftpc_AddHistory(SCR_FTP_ABORTING);
	btmtk_ftpc_ui_callback(BT_FTPCUI_ABORTING, 0);
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_ftpc_set_folder_cnf_handler
 * DESCRIPTION
 *  This function is to handle server MSG_ID_BT_FTPC_SET_FOLDER_CNF msg
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftpc_set_folder_cnf_handler(void *msg)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	goep_set_folder_cnf_struct *rsp = (goep_set_folder_cnf_struct*) msg;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	i = bt_ftpc_find_goep_id(rsp->goep_conn_id);
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_SET_FOLDER_CNF_HDLR, act_client_cntx_p->ftpc_state, i));

	if (i == 0xff)
	{
		// reset
		return;
	}

	if (rsp->rsp_code == GOEP_STATUS_SUCCESS) {
		// Set the folder successfully, then start to get folder content

		switch ((act_client_cntx_p + i)->ftpc_state) {
			case BT_FTPC_STATE_SETTING_FOLDER:
				(act_client_cntx_p + i)->browse_depth++;
				break;

			case BT_FTPC_STATE_SETTING_BACK_FOLDER:
				(act_client_cntx_p + i)->browse_depth--;
				break;

			case BT_FTPC_STATE_SETTING_ROOT_FOLDER:
				(act_client_cntx_p + i)->browse_depth = 0;
				break;

			case BT_FTPC_STATE_TPDISCONNECTING:
				// omit it, tpdisconn_req have sent
				return;

			case BT_FTPC_STATE_CREATE_FOLDER:
			{
				// after create a folder. get back to its parent folder !
				   
				// send a back folder req
				(act_client_cntx_p + i)->browse_depth++;

#ifndef FTPC_AUTO_CREATE_BACK_FOLDER
				BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_CONNECTED);
#else
				goep_set_folder_req_struct* req; 
				// Regarding to FTP spec. after creating a new folder, the current path will be inside the new folder
				BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_SETTING_BACK_FOLDER);

				// construct set folder req ilm
				req = (goep_set_folder_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_set_folder_req_struct), TD_CTRL);

				// memory init
				GOEP_MEMSET(req, 0, sizeof(goep_set_folder_req_struct));
				req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
				req->setpath_flag = GOEP_BACK_FOLDER;
				GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
				req->uuid_len = 16;

				// send set folder req
				bt_ftp_send_msg(MSG_ID_BT_FTPC_SET_FOLDER_REQ, req, sizeof(goep_set_folder_req_struct));
				return;
#endif
			} 
			break;

			default:
				// error state
				GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_STATE_ERROR, act_client_cntx_p->ftpc_state));
				BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);
				// Notify MMI Screen
				return;
		}

		BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_CONNECTED);
		if (TRUE == btmtk_ftpc_auto_get_listing_xml()) {
			bt_ftpc_start_get_folder_content_routine(0);
		}

		btmtk_ftpc_ui_callback(BT_FTPCUI_SETPATHED, (void *) (U32) rsp->rsp_code);

	} else {

		switch ((act_client_cntx_p + i)->ftpc_state) {
			case BT_FTPC_STATE_SETTING_FOLDER:
				{
					// forward folder failed, not disconnect
					// FTP_DEBUG(kal_print("FTPC SET FORWARD FOLDER RSP HDLR ERROR\n");)
					BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);

					if ((act_client_cntx_p + i)->fh_ftpc_recv) {
						btmtk_goep_close_wstream((act_client_cntx_p + i)->fh_ftpc_recv);
						(act_client_cntx_p + i)->fh_ftpc_recv = (FHANDLE) FHANDLE_INVALID_VALUE;
					}
					btmtk_ftpc_ui_callback(BT_FTPCUI_SETPATHED, (void *)(rsp->rsp_code !=0 ? rsp->reason : 1));
				}
				break;

			case BT_FTPC_STATE_CREATE_FOLDER:
				{
					BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);
					btmtk_ftpc_ui_callback(BT_FTPCUI_FOLDER_CREATED, (void *)(rsp->rsp_code != 0 ? rsp->reason : 1));
				}
				break;

			case BT_FTPC_STATE_SETTING_BACK_FOLDER:
			case BT_FTPC_STATE_SETTING_ROOT_FOLDER:
				{
					// when setting back/root  folder failed, popup and disconnect it
					// FTP_DEBUG(kal_print("FTPC SET FOLDER RSP HDLR ERROR\n");)

					BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);

					btmtk_ftpc_ui_callback(BT_FTPCUI_SETPATHED, (void *) (U32) rsp->rsp_code);

					// StartTimer(BT_FTPC_BROWSE_FILES_FAILED_TIMER,
					//	    FTPC_BROWSE_FILES_FAILED_DELAY_DUR,
					//	    btmtk_ftpc_browse_files_failed_timer_expire_handler);
				}
				break;

			case BT_FTPC_STATE_DISCONNECTING:
				// bt_ftpc_send_disconnect_message(); // ignore this fail.
				return;

			case BT_FTPC_STATE_TPDISCONNECTING:
				// Omit it, when setting_folder, tpdiscon may be sent
				return;

			default:
				// error state
				GOEP_Report("[FTPC] set_folder_cnf_handler fail state rsp_code:(%d) state:(%d)",
						rsp->rsp_code, (act_client_cntx_p + i)->ftpc_state);

				BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);
				btmtk_ftpc_ui_callback(BT_FTPCUI_SETPATHED, (void *)(rsp->rsp_code!=0?rsp->reason:1) );
				return;
		}
	}
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_ftpc_action_cnf_handler
 * DESCRIPTION
 *  This function is to handle server MSG_ID_BT_FTPC_SET_FOLDER_CNF msg
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftpc_action_cnf_handler(void *msg) {
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	//goep_set_folder_req_struct *req;
	goep_action_cnf_struct *rsp = (goep_action_cnf_struct*) msg;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	//FTP_DEBUG(kal_print("FTPC SET FOLDER RSP HDLR\n");)
	i = bt_ftpc_find_goep_id(rsp->goep_conn_id);

	if (i == 0xff) {
		/* reset */
		return;
	}

	if (rsp->rsp_code == GOEP_STATUS_SUCCESS) {
		BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);
	} else {
		BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);
	}
	OS_Report("[FTPC] btmtk_ftpc_action_cnf_handler rsp:%d", rsp->rsp_code);
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_ftpc_abort_cnf_handler
 * DESCRIPTION
 *  This function is to handle server MSG_ID_BT_FTPC_ABORT_CNF msg
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_ftpc_abort_cnf_handler(void *msg)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	goep_abort_cnf_struct *rsp = (goep_abort_cnf_struct*) msg;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	i = bt_ftpc_find_goep_id(rsp->goep_conn_id);
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPC_ABORT_CNF_HDLR, (act_client_cntx_p)->ftpc_state));

	if (i == 0xff) {
		// reset. Not found the context
		return;
	}

	if ((act_client_cntx_p + i)->ftpc_state == BT_FTPC_STATE_ABORTING) {
		//FTP_DEBUG(kal_print("FTPC ABORT RSP HDLR IN STATE ABORTING\n");)

		(act_client_cntx_p + i)->ftpc_total_len = 0;
		(act_client_cntx_p + i)->ftpc_reamin_len = 0;
		(act_client_cntx_p + i)->ftpc_data_len = 0;
		(act_client_cntx_p + i)->total_len_type = BT_FTPC_TOTAL_LEN_NO_FIRST_PKT;

		BT_FTPC_STATE_TRANS(i, BT_FTPC_STATE_CONNECTED);

		if (FALSE == ext_fs_is_valid_handle_ucs2((act_client_cntx_p + i)->fh_ftpc_recv) &&
			FALSE == ext_fs_is_valid_handle_ucs2((act_client_cntx_p+i)->fh_ftpc_push)) {

			// not pull data yet. it is the same as pull file
			GOEP_Report("[BT][FTP] Abort in  PULL mode but file is not open yet");
#ifdef __BT_GOEP_V2__
                        // ALPS00127542: If no file is open when aborting, do not callback to UI.
                        // btmtk_ftpc_ui_callback(BT_FTPCUI_PULLED, (void *)GOEP_USER_CANCEL);
#else
			btmtk_ftpc_ui_callback(BT_FTPCUI_PULLED, (void *) GOEP_USER_CANCEL);
#endif
		}

		if (FALSE != ext_fs_is_valid_handle_ucs2((act_client_cntx_p + i)->fh_ftpc_recv)) {
			// Abort the current transmission	        
			btmtk_goep_close_wstream((act_client_cntx_p + i)->fh_ftpc_recv);
			(act_client_cntx_p + i)->fh_ftpc_recv = FHANDLE_INVALID_VALUE;

			//delete it by the name in 
			btmtk_goep_delete_file((const U16*) act_client_cntx_p->ftpc_filepath);
			btmtk_goep_clear_filepath((U8 *)act_client_cntx_p->ftpc_filepath);
			btmtk_ftpc_ui_callback(BT_FTPCUI_PULLED, (void *) GOEP_USER_CANCEL);  
		}

		if (FALSE != ext_fs_is_valid_handle_ucs2((act_client_cntx_p+i)->fh_ftpc_push)) {
			// Abort the current transmission
			btmtk_goep_close_rstream((act_client_cntx_p+i)->fh_ftpc_push);
			(act_client_cntx_p + i)->fh_ftpc_push = (FHANDLE) FHANDLE_INVALID_VALUE;
			btmtk_ftpc_ui_callback(BT_FTPCUI_PUSHED, (void *) GOEP_USER_CANCEL); 
		}

		// record the first state and send the abort by user confirm
	} else {
		GOEP_Report("[FTPC] wrong state:%d", (act_client_cntx_p + i)->ftpc_state );
	}
}



/*****************************************************************************
 * FUNCTION
 *  bt_ftp_usb_plugin_hdle
 * DESCRIPTION
 *  the call back func when usb plug in 
 * PARAMETERS
 *  msg     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftp_usb_plugin_hdle(void)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	// MAUI. abort current transmission. disconnect the ftpc connection
}


void btmtk_ftpc_register_client_cnf(void *data)
{
	BT_FTP_ASSERT(0); // no support
}

void btmtk_ftpc_deregister_client_cnf(void *data)
{
	BT_FTP_ASSERT(0); // no support
}

void btmtk_ftpc_auth_ind(void *data) {
	goep_auth_ind_struct *ind;

	ind = (goep_auth_ind_struct *) data;

	switch (act_client_cntx_p->ftpc_state) {
		case BT_FTPC_STATE_IDLE:
		case BT_FTPC_STATE_CONNECTING:
			BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_CONNECTING);

			// save the goep_conn_id to 
			ftpc_cntx_p->goep_conn_id[0] = ind->goep_conn_id;
			act_client_cntx_p->challeng_options = ind->options;
			GOEP_MEMCPY(act_client_cntx_p->realm,  ind->realm, sizeof(act_client_cntx_p->realm));
			btmtk_ftpc_ui_callback( BT_FTPCUI_AUTHEN_WAIT, (void *) 0);

		default:
			// disconnect it ?
			GOEP_Report( "[FTPC][WRN] wrng state:%d receive auth_ind", act_client_cntx_p->ftpc_state );
			break;
	}
}

void btmtk_ftpc_auth_cnf(void *data) {
	BT_FTP_ASSERT(0); // no support
}

void btmtk_ftpc_disconnect_cnf_handler(void *data) {
	// nothing
}

void btmtk_ftpc_handle_message(ilm_struct *message) {
	void *data;

	// data = (void *)message->local_para_ptr;
	data = (void *)message->ilm_data;

	
#ifndef __BT_GOEP_V2__
	// convert GOEP ID to FTP ID. Add server interval or clietn interval 
	if (message->msg_id <= MSG_ID_BT_GOEPC_GROUP_END && message->msg_id >= MSG_ID_BT_GOEPC_GROUP_START) {
		message->msg_id += ((U32)MSG_ID_BT_FTPC_GROUP_START - (U32) MSG_ID_BT_GOEPC_GROUP_START );
	} else if (message->msg_id <= MSG_ID_BT_GOEPS_GROUP_END && message->msg_id >= MSG_ID_BT_GOEPS_GROUP_START) {
		message->msg_id += ((U32)MSG_ID_BT_FTPS_GROUP_START - (U32) MSG_ID_BT_GOEPS_GROUP_START );
	}

	btmtk_goep_show_id(message->msg_id, FALSE, MOD_MMI);
#else
	if (message->msg_id <= MSG_ID_BT_GOEPC_GROUP_END && message->msg_id >= MSG_ID_BT_GOEPC_GROUP_START) {
        	bt_ext_log("[FTPC][ERR] goepc wrong id:%d", message->msg_id);
	} else if (message->msg_id <= MSG_ID_BT_GOEPS_GROUP_END && message->msg_id >= MSG_ID_BT_GOEPS_GROUP_START) {	
		bt_ext_log("[FTPS][ERR] goeps wrong id:%d", message->msg_id);
	}
#endif    

	switch(message->msg_id) {
		// Client
		case MSG_ID_BT_FTPC_REGISTER_CLIENT_CNF:
			bt_ext_log("MSG_ID_BT_FTPC_REGISTER_CLIENT_CNF");
			btmtk_ftpc_register_client_cnf(data);
			break;
		case MSG_ID_BT_FTPC_DEREGISTER_CLIENT_CNF:
			bt_ext_log("MSG_ID_BT_FTPC_DEREGISTER_CLIENT_CNF");
			btmtk_ftpc_deregister_client_cnf(data);
			break;
		case MSG_ID_BT_FTPC_CONNECT_CNF:
			bt_ext_log("MSG_ID_BT_FTPC_CONNECT_CNF");
			btmtk_ftpc_connect_cnf_handler(data);
			break;
		case MSG_ID_BT_FTPC_PUSH_CNF:
			// bt_ext_log("MSG_ID_BT_FTPC_PUSH_CNF");
			btmtk_ftpc_push_cnf_handler(data);
			break;
		case MSG_ID_BT_FTPC_PULL_CNF:
			// bt_ext_log("MSG_ID_BT_FTPC_PULL_CNF");
			btmtk_ftpc_pull_cnf_handler(data);
			break;
		case MSG_ID_BT_FTPC_SET_FOLDER_CNF:
			bt_ext_log("MSG_ID_BT_FTPC_SET_FOLDER_CNF");
			btmtk_ftpc_set_folder_cnf_handler(data); 
			break;
		case MSG_ID_BT_FTPC_ABORT_CNF:
			bt_ext_log("MSG_ID_BT_FTPC_ABORT_CNF");
			btmtk_ftpc_abort_cnf_handler(data); 
			break;
		case MSG_ID_BT_FTPC_SUPPORTED_FORMATS_IND: // client
			bt_ext_log("MSG_ID_BT_FTPC_SUPPORTED_FORMATS_IND");
			// do nothing
			break;
		case MSG_ID_BT_FTPC_DISCONNECT_CNF: // server & client
			bt_ext_log("MSG_ID_BT_FTPC_DISCONNECT_CNF");
			btmtk_ftpc_disconnect_cnf_handler(data);
			break;
		case MSG_ID_BT_FTPC_TPDISCONNECT_IND: // server & client
			bt_ext_log("MSG_ID_BT_FTPC_TPDISCONNECT_IND");
			btmtk_ftpc_disconnect_ind_handler(data);
			break;
		case MSG_ID_BT_FTPC_CLIENT_INTERNAL_RW:
			bt_ext_log("MSG_ID_BT_FTPC_CLIENT_INTERNAL_RW");
			btmtk_ftpc_internal_rw_handler(data);
			break;
		case MSG_ID_BT_FTPC_AUTH_IND: // Server or Client
			bt_ext_log("MSG_ID_BT_FTPC_AUTH_IND");
			btmtk_ftpc_auth_ind(data);
			break;        
		case MSG_ID_BT_FTPC_ACTION_CNF:
			btmtk_ftpc_action_cnf_handler(data);
			break;
		case MSG_ID_BT_FTPC_PUSHFILE_CNF:
			btmtk_ftpc_pushfile_cnf_handler(data);
			break;
		case MSG_ID_BT_FTPC_PULLFILE_CNF:
			btmtk_ftpc_pullfile_cnf_handler(data);
			break;
		case MSG_ID_BT_FTPC_FILE_PROGRESS_IND:
			bt_ext_log("[ftpc] MSG_ID_BT_FTPC_FILE_PROGRESS_IND receive ind.");
			btmtk_ftpc_file_progress_ind_handler(data);
			break;
		case MSG_ID_BT_FTPC_ABORTFILE_CNF:
			bt_ext_log("[ftpc] MSG_ID_BT_FTPC_ABORTFILE_CNF received.");
			btmtk_ftpc_abortfile_cnf_handler(data);
			break;
		default:
			GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_UNKNOW_MSG_HDLR,  message->msg_id) );
			break;
		}
}
//#endif


/********************************* Message ****************************/



/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_send_tpdisconnect_message
 * DESCRIPTION
 *  This is common function to disconnect
 * PARAMETERS
 *  void
 *  cause(?)        [IN]        The cause to disconnect current session(?)
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftpc_send_tpdisconnect_message(U32 i)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	goep_disconnect_req_struct *req;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_SEND_DISCONNECT_MSG, act_client_cntx_p->ftpc_state) );

	// construct disconnect req ilm
	req = (goep_disconnect_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_disconnect_req_struct),TD_CTRL);
	GOEP_MEMSET( (U8 *)req, 0, sizeof(goep_disconnect_req_struct) );    /* memory init */
	req->req_id = ftpc_cntx_p->req_id[i];
	req->goep_conn_id = ftpc_cntx_p->goep_conn_id[i];
	GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
	req->uuid_len = 16;

	bt_ftp_send_msg(MSG_ID_BT_FTPC_DISCONNECT_REQ, req,sizeof(goep_disconnect_req_struct));
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_send_abort
 * DESCRIPTION
 *  This function is to abort the getting obj operation
 * PARAMETERS
 *  rsp     [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftpc_send_abort_message(U8 goep_conn_id)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	goep_abort_req_struct *req;

	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_SEND_ABORT_MSG, act_client_cntx_p->ftpc_state) );

	// construct abort req ilm
	req = (goep_abort_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_abort_req_struct),TD_CTRL);
	GOEP_MEMSET( (U8 *)req, 0, sizeof(goep_abort_req_struct));    /* memory init */
	req->goep_conn_id = goep_conn_id;
	GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, 16);
	req->uuid_len = 16;

	// send abort req
	bt_ftp_send_msg(MSG_ID_BT_FTPC_ABORT_REQ, req ,sizeof(goep_abort_req_struct));
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_send_disconnect_message
 * DESCRIPTION
 *  This is common function to disconnect
 * PARAMETERS
 *  void
 *  cause(?)        [IN]        The cause to disconnect current session(?)
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftpc_send_disconnect_message(void)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	goep_disconnect_req_struct *req;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPC_SEND_DISCONNECT_MSG, act_client_cntx_p->ftpc_state) );

	// construct disconnect req ilm
	req = (goep_disconnect_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_disconnect_req_struct),TD_CTRL);
	GOEP_MEMSET( (U8 *)req, 0, sizeof(goep_disconnect_req_struct) );    /* memory init */
	req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
	req->uuid_len = sizeof(OBEX_FTP_UUID);
	GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID));

	// send disconnect req and connect cnf fail to CM
	bt_ftp_send_msg(MSG_ID_BT_FTPC_DISCONNECT_REQ, req ,sizeof(goep_disconnect_req_struct));
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftpc_send_disconnect_message
 * DESCRIPTION
 *  This is common function to disconnect
 * PARAMETERS
 *  void
 *  cause(?)        [IN]        The cause to disconnect current session(?)
 * RETURNS
 *  void
 *****************************************************************************/
BT_BOOL bt_ftpc_send_action_routine(U8 action_id, U32 permission,
		const U16 * l_u2FolderName, const U16 *l_u2DstName) {
	/// init a request to obex set-path to create remote's folder
	/// after create is success, we need to goback a level
	goep_action_req_struct *req;
	U16 len;

	//MMI_FTPC_STATE_TRANS(0, MMI_FTPC_STATE_SETTING_BACK_FOLDER);
	GOEP_MEMSET(act_client_cntx_p->ftpc_obj_name, 0,
			sizeof(act_client_cntx_p->ftpc_obj_name));
	ext_ucs2ncpy((S8 *) act_client_cntx_p->ftpc_obj_name, (S8 *) l_u2FolderName,
			(sizeof(act_client_cntx_p->ftpc_obj_name) - 2 / 2));

	/* construct set folder req ilm */
	req = (goep_action_req_struct*) CONSTRUCT_LOCAL_PARAM(
			sizeof(goep_action_req_struct), TD_CTRL);
	FTPC_MEMSET(req, sizeof(goep_action_req_struct)); /* memory init */
	req->goep_conn_id = ftpc_cntx_p->goep_conn_id[0];
	req->action_id = action_id;
	req->permission = permission;

	ext_ucs2ncpy((S8 *) req->folder_name,
			(S8 *) act_client_cntx_p->ftpc_obj_name, GOEP_MAX_FOLDER_NAME);
	bt_ftp_ucs2_htons((U8 *) req->folder_name, (U8 *) req->folder_name);

	len = ext_ucs2strlen((const S8 *) l_u2DstName) * 2;
	if (len > (GOEP_MAX_FOLDER_NAME - 2)) {
		len = GOEP_MAX_FOLDER_NAME - 2;
	}
	ext_ucs2ncpy((S8 *) req->dst_name, (S8 *) l_u2DstName, len);
	bt_ftp_ucs2_htons((U8 *) req->dst_name, (U8 *) req->dst_name);

	req->uuid_len = sizeof(OBEX_FTP_UUID);
	GOEP_MEMCPY(req->uuid, OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID));

	/* send set folder req */
	BT_FTPC_STATE_TRANS(0, BT_FTPC_STATE_ACTION);

	{
		int x;
		x = MSG_ID_BT_GOEPC_GROUP_START;
		x = MSG_ID_BT_GOEPC_GROUP_END;
		OS_Report("[ILM] GOEPC start:0x%x end:0x%x",
				MSG_ID_BT_GOEPC_GROUP_START, MSG_ID_BT_GOEPC_GROUP_END);
		x = MSG_ID_BT_OPPC_GROUP_START;
		x = MSG_ID_BT_OPPC_GROUP_END;
		OS_Report("[ILM] OPPC start:0x%x end:0x%x", MSG_ID_BT_OPPC_GROUP_START,
				MSG_ID_BT_OPPC_GROUP_END);
		x = MSG_ID_BT_FTPC_GROUP_START;
		x = MSG_ID_BT_FTPC_GROUP_END;
		OS_Report("[ILM] FTPC start:0x%x end:0x%x", MSG_ID_BT_FTPC_GROUP_START,
				MSG_ID_BT_FTPC_GROUP_END);

	}
	bt_ftp_send_msg(MSG_ID_BT_FTPC_ACTION_REQ, req,
			sizeof(goep_action_req_struct));
	return TRUE;
}

void bt_ftpc_send_abortfile_req_msg(U8 goep_conn_id, U8 reason) {
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	bt_ftpc_abortfile_req_struct *req;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/

	/* construct disconnect req ilm */
	req = (bt_ftpc_abortfile_req_struct*) CONSTRUCT_LOCAL_PARAM(
			sizeof(bt_ftpc_abortfile_req_struct), TD_CTRL);
	GOEP_MEMSET(req, 0, sizeof(bt_ftpc_abortfile_req_struct)); /* memory init */
	req->goep_conn_id = goep_conn_id;
	req->rsp_code = reason;

	/* send disconnect req and connect cnf fail to CM */
	bt_ftp_send_msg(MSG_ID_BT_FTPC_ABORTFILE_REQ, req,
			sizeof(bt_ftpc_abortfile_req_struct));
}

