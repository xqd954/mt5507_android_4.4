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
 * bt_opps_hdl.c
 *
 * Project:
 * --------
 *
 *
 * Description:
 * ------------
 *   This file is for MTK bluetooth MMI OPP server
 *
 * Author:
 * -------
 * Daylong Chen
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 11 09 2012 dexiang.jiang
 * [ALPS00378673] [BT][OPP] stopped file is not removed
 * Patch for remove unfinished files.
 *
 * 09 03 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integration BT solution.
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#include "bt_goep_porting.h"

#include "bluetooth_ftpc_struct.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_struct.h"
#include "bt_os_api.h"
#include "ext_osal.h"
#include "ext_nbuffer.h"

#include "bt_goep_porting.h"
#include "bt_goep_fs.h"
#include "bt_opp_porting.h"
#include "bt_opp_comm.h"
#include "bt_oppc_api.h"
#include "bt_opps_api.h"
#include "bt_oppc_hdl.h"
#include "bt_opps_hdl.h"
#include "bt_oppc_struct.h"
#include "bt_opps_struct.h"
#include "ext_ucs2.h"
#include "bt_opps_msg.h"
#include "bt_ext_debug.h"

/*****************************************************************************
* External Functions
*****************************************************************************/

/*****************************************************************************
* Internal Functions
*****************************************************************************/
static void bt_opps_flush_one_nbuffer();
static void bt_opps_flush_all_nbuffer();
static void bt_opps_connection_terminated(U8 cause);

/*****************************************************************************
* Internal Utilities Functions: No change the context of OPPS
*****************************************************************************/

/*****************************************************************************
* External Variable
*****************************************************************************/

/*****************************************************************************
* Local Variable
*****************************************************************************/

#define BT_OPP_AUTO_CHECK 1

void btmtk_opps_handle_message(void *msg)
{
	goep_disconnect_ind_struct *disc_ind;
	U32 msg_id;
	void *data;

	ilm_struct *p = (ilm_struct *)msg;
	msg_id = p->msg_id;
	data = &p->ilm_data; //data = p->local_para_ptr; change for Android porting

    btmtk_goep_show_id( p->msg_id, FALSE, MOD_MMI);

    BT_OPP_LOG_STATE();

    switch(msg_id)
    {
        /* OPP server event */
        case MSG_ID_BT_OPPS_REGISTER_SERVER_CNF:
            btmtk_opps_obex_register_server_cnf_handler(data);
            break;
        case MSG_ID_BT_OPPS_DEREGISTER_SERVER_CNF:
            btmtk_opps_obex_deregister_server_cnf_handler(data);
            break;
        case MSG_ID_BT_OPPS_CONNECT_IND:
            btmtk_opps_connect_ind_handler(data);
            break;
        case MSG_ID_BT_OPPS_PUSH_IND:
            btmtk_opps_push_ind_handler(data);
            break;
        case MSG_ID_BT_OPPS_AUTHORIZE_IND:
            btmtk_opps_authorize_ind_handler(data);
            break;
        case MSG_ID_BT_OPPS_PULL_IND:
            btmtk_opps_pull_ind_handler(data);
            break;
        case MSG_ID_BT_OPPS_DISCONNECT_IND:
            btmtk_opps_disconnect_ind_handler(data);
            break;
        case MSG_ID_BT_OPPS_ABORT_IND:
            btmtk_opps_abort_ind_handler(data);
            break;
        case MSG_ID_BT_OPPS_TPDISCONNECT_IND:
			{
				disc_ind = (goep_disconnect_ind_struct*) data;
				if( NULL != disc_ind ){
					BT_OPP_LOGD("[EXT] disconnect_ind_handler goep_id:%d reason:%d req_id:%d", disc_ind->goep_conn_id, disc_ind->reason, disc_ind->req_id);
				}
				bt_opps_connection_terminated(disc_ind->reason);
			}
            break;
        case MSG_ID_BT_OPPS_SERVER_INTERNAL_RW:
            btmtk_opps_internal_rw_handler(data);
            break;
        default:
            //printf("err msg=%d %s\n", msg_id, bt_goep_get_id_name(msg_id) );
            break;
    }
}

/*
 * OPP Server Out Event MMI -> OBEX
 */
/*****************************************************************************
 * FUNCTION
 *  btmtk_opps_obex_register_server_cnf_handler
 * DESCRIPTION
 *  This function is event handler for OPP server OBEX server register rsp (MSG_ID_OBEX_REGISTER_SERVER_RSP)
 * PARAMETERS
 *  msg         [?]
 *  a(?)        [IN/OUT]        First variable, used as returns
 *  b(?)        [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_opps_obex_register_server_cnf_handler(void *msg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_register_server_cnf_struct *rsp = (goep_register_server_cnf_struct*)msg;

    BT_OPP_LOGD("[EXT] btmtk_opps_obex_register_server_cnf_handler: %d", rsp);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_OPP_LOGD("[EXT] btmtk_opps_obex_register_server_cnf_handler(id[%d], rsp[%d], waiting[%d])", rsp->req_id, rsp->rsp_code, GOPS(ops_goep_req_id));

    /* check request id if match */
    if (rsp->req_id != GOPS(ops_goep_req_id))
    {
        BT_OPP_ASSERT(0);
    }

    BT_OPP_LOGD("[EXT] btmtk_opps_obex_register_server_cnf_handler: state:[%d] == [%d]or[%d]", GOPS(ops_state), BT_OPS_OBEX_REGISTERING, BT_OPS_SDP_REGISTERING );

    /* the state shall be BT_OPS_SDP_REGISTERING */
    if (GOPS(ops_state) == BT_OPS_OBEX_REGISTERING || GOPS(ops_state) == BT_OPS_SDP_REGISTERING)
    {
        if (rsp->rsp_code == GOEP_STATUS_SUCCESS)
        {
        	BT_OPP_LOGD("[EXT] btmtk_opps_obex_register_server_cnf_handler(): server activating success");
            /* OPP server activating successfully */
            GOPS(ops_goep_conn_id) = rsp->goep_conn_id;
            BT_OPS_STATE_TRANS(BT_OPS_ACTIVE);
        }
        else
        {
        	bt_ext_err("[EXT] btmtk_opps_obex_register_server_cnf_handler(): server activating fail");
            /* OPP must server activating successfully, otherwise assert(0) */
            BT_OPP_ASSERT(0);
        }
    }
    else
    {
    	bt_ext_err("[EXT] btmtk_opps_obex_register_server_cnf_handler(): server state error");
        BT_OPP_ASSERT(0);
    }
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_opps_obex_deregister_server_cnf_handler
 * DESCRIPTION
 *
 * PARAMETERS
 *  msg     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_opps_obex_deregister_server_cnf_handler(void *msg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_deregister_server_cnf_struct *rsp = (goep_deregister_server_cnf_struct*)msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (GOPS(ops_state) == BT_OPS_DEACTIVATING)
    {
        if (rsp->goep_conn_id == GOPS(ops_goep_conn_id))
        {
            /* invoke CM deactivate confirm callback */
            btmtk_opps_ui_notify( BT_OPPSUI_SHUTDOWNING, NULL );
            btmtk_opps_ui_notify( BT_OPPSUI_SHUTDOWNED, NULL );
        }
        else
        {
            BT_OPP_ASSERT(0);
        }
    }
    else
    {
        BT_OPP_ASSERT(0);
    }
}



/*****************************************************************************
 * FUNCTION
 *  btmtk_opps_connect_ind_handler
 * DESCRIPTION
 *  This function is event handler for OBEX server connect indication (MSG_ID_BT_GOEP_CONNECT_IND)
 * PARAMETERS
 *  msg         [?]
 *  a(?)        [IN/OUT]        First variable, used as returns
 *  b(?)        [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_opps_connect_ind_handler(void *msg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_connect_ind_struct *ind = (goep_connect_ind_struct*)msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GOPS(ops_dev_addr).lap = ind->bd_addr.lap;
    GOPS(ops_dev_addr).uap = ind->bd_addr.uap;
    GOPS(ops_dev_addr).nap = ind->bd_addr.nap;
    GOPS(ops_goep_conn_id) = ind->goep_conn_id;
    GOPS(ops_cm_conn_id) = ind->cm_conn_id;
    GOPS(ops_mtu) = ind->peer_mru;
    if( GOPS(ops_mtu) > OPPS_MAX_OBEX_PACKET_LENGTH ){
        GOPS(ops_mtu) = OPPS_MAX_OBEX_PACKET_LENGTH;
    }
    if( GOPS(ops_mtu) < 256 ){
        GOPS(ops_mtu) = 256;
    }

    ext_strncpy((S8*)GOPS(dev_name), (S8*)ind->dev_name, sizeof(GOPS(dev_name)) );

    switch (GOPS(ops_state))
    {
    case BT_OPS_ACTIVE: // no auth before
    case BT_OPS_CONNECTED: // auth done
        BT_OPS_STATE_TRANS(BT_OPS_CONNECTED);
        bt_opps_connect_rsp(GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS);
        btmtk_opps_ui_notify(BT_OPPSUI_CONNECTED, (void *) 0);
        break;

    case BT_OPS_DISCONNECTING: /* Fall through */
    case BT_OPS_DEACTIVATING:
        /* Do Nothing */
        break;
    default:
        BT_OPS_LOG_STATE_ERR();
        bt_opps_connect_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR);
        break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_opps_disconnect_ind_handler
 * DESCRIPTION
 *  This function is OPP server event handler for OBEX disconnect indication (MSG_ID_BT_GOEP_DISCONNECT_IND)
 * PARAMETERS
 *  msg     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_opps_disconnect_ind_handler(void *msg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_disconnect_ind_struct *ind = (goep_disconnect_ind_struct*)msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_LOGD(BT_OPP_OPPS_DISC_CONN_IND);

    if (GOPS(ops_goep_conn_id) != ind->goep_conn_id)
    {
        BT_OPP_ASSERT(0);
    }

    if (GOPS(ops_state) > BT_OPS_ACTIVE)
    {
        /* UI notify in this function below */
        bt_opps_connection_terminated(BT_OPP_ERR_USER);
    }
    else
    {
        BT_OPS_LOG_STATE_ERR();
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_opps_push_ind_write_error
 * DESCRIPTION
 *  This function is OPP server event handler for OBEX disconnect indication (MSG_ID_BT_GOEP_DISCONNECT_IND)
 * PARAMETERS
 *  msg     [?]
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_opps_push_ind_write_error(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //U16 string_id;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    /* Send response to OBEX */
    // TODO: bad smell. rewrite this one.

    //send error back
    if( FALSE != ext_fs_is_valid_handle_ucs2(GOPS(h_ops_recv)) ){
        bt_goep_clean_tempfile( GOPS(h_ops_recv), (U8 *)GOPS(recv_path), FALSE);
        GOPS(h_ops_recv) = FHANDLE_INVALID_VALUE;
    }

    BT_OPP_LOGD("[EXT] push_ind_write internal_server_err ");
    bt_opps_push_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR);
    BT_OPS_STATE_TRANS(BT_OPS_CONNECTED);
}

#if 0
/**
* @brief This function is to generate temporary object for writing
* @param obj_name suggestion name
* @param final_filepath the give name and OPP receive path
* @return file handle which filename is the final name
**/
FHANDLE bt_opps_util_gen_temp_obj(const U16 *obj_name, U16 *final_filepath, U16 maxSize)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 cnt = 0;
    S32 retval = FS_STATUS_OK;
    S32 l_ret = FHANDLE_INVALID_VALUE; /// default return
    U32 l_u4Count;
    BT_BOOL l_found = FALSE;
    U8 FolderPath[OPP_MAX_PATH_LENGTH], FilePath[OPP_MAX_PATH_LENGTH], NewFileName[OPP_MAX_PATH_LENGTH];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if( btmtk_opps_get_default_local_folderpath(FolderPath, 128) ){
        btmtk_goep_create_folder( (const U16 *)FolderPath );

        if( FALSE == btmtk_goep_is_folder_exist( (const U16*) FolderPath ) ){
            return l_ret;
        }

        // compose the path
        for( l_u4Count = 0; l_u4Count < BT_OPPS_MAX_NUM_TEMP_FILE; l_u4Count++ ){
            if( 0 != btmtk_goep_gen_temp_filename(l_u4Count, (const U8 *)obj_name,  (U8 *) NewFileName, OPP_MAX_PATH_LENGTH) ){
                if( 0 != btmtk_goep_compose_filepath(FolderPath, NewFileName, FilePath, OPP_MAX_PATH_LENGTH) ){
                    if( FALSE == btmtk_goep_is_file_exist( (const U16*) FilePath) ){
                        // copy filnal_name from filepath and open the file
                        //GOEP_MEMCPY(final_filepath);
                        ext_ucs2ncpy( (S8 *)final_filepath, (S8 *)FilePath, OPP_MAX_PATH_LENGTH/sizeof(U16));
                        l_found = TRUE;
                        break;
                    }
                }
            }
        }

        if( TRUE == l_found){
            // open the file and return it
            return btmtk_goep_open_wstream( (const U16*) FilePath, 0 );
        }else{
            return FHANDLE_INVALID_VALUE;
        }
    }

    return l_ret;

}
#endif

#if 0
/*****************************************************************************
 * FUNCTION
 *  bt_opps_proc_first_push_data_routine
 * DESCRIPTION
 *  This function is process first push data
 * PARAMETERS
 *  ind [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_opps_proc_first_push_data_routine(goep_push_ind_struct *ind, BT_BOOL bSentRsp)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 retval;
    FHANDLE h;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_LOGD(BT_OPP_OPPS_FIRST_PUSH_IND);

    /* Close previous file handle and delete temp file */
    retval = bt_goep_clean_tempfile( GOPS(h_ops_recv),(U8 *) GOPS(recv_path), FALSE);
    if ( FS_STATUS_OK != retval )
    {
    	BT_OPP_LOGD("[EXT][err] proc_first_push_data fail to clean_tempfile ret:%d", retval);
        /* Close fail may because other AP doing file operation */
        bt_opps_push_ind_write_error();
        return;
    }

    /* Save object name to context */
    bt_goep_truncate_ntoh_filename(GOPS(ops_obj_name), (U16 *)ind->obj_name, sizeof(GOPS(ops_obj_name)));
    if( 0 == ext_ucs2strlen( (const S8 *)GOPS(ops_obj_name)) ){
    	BT_OPP_LOGD("[EXT][wrn] proc_first_push_data obj_name is empty");
        ext_ucs2ncpy( (S8 *) GOPS(ops_obj_name), (const S8 *)BT_OPP_SND_DEFAULT_OBJNAME, sizeof(GOPS(ops_obj_name)) );
    }

    /* Generate temporary object */
    h = bt_opps_util_gen_temp_obj(GOPS(ops_obj_name), GOPS(recv_path), sizeof(GOPS(recv_path)) );

    //TODO BT_OPP_LOG1(bt_opp_OPPS_GEN_TEMP_OBJ, retval);
    if ( FALSE == ext_fs_is_valid_handle(h) )
    {
        bt_opps_set_fs_flag_routine(retval);
        return;
    }
    GOPS(h_ops_recv) = h;
    if( FALSE != ext_nbuffer_isInit(&GOPS(ops_mempool)) ){
    	ext_nbuffer_cleanup( &GOPS(ops_mempool) );
    }
    GOPS(ops_fs_status) = FS_STATUS_OK;
    GOPS(ops_u4RecvSize) = ind->frag_len;


    BT_OPS_STATE_TRANS(BT_OPS_RECEIVING);

    //BT_OPP_LOG2(BT_OPP_OPPS_FIRST_PUSH_IND_FRAG_LEN, ind->frag_len, ind->total_obj_len);

    GOPS(stop_recv) = FALSE;

    /* Write object context to file */
    if (ind->frag_len > 0)
    {
        U32 len_written;

        // auto response fail if write_data fail
        retval = bt_opps_write_data_routine( ind->frag_ptr,
            ind->frag_len,
            &len_written,
            ind->pkt_type, FALSE);

        if (retval ==  EXT_FS_ERR)
        {
            return;
        }
    }

    if( FALSE != bSentRsp ){
        /* It is first packet and also final packet, Receive Finished !! */
           btmtk_opps_ui_notify( BT_OPPSUI_PUSHING, (void *) GOPS(ops_u4RecvSize) );
        if ( ind->pkt_type == GOEP_SINGLE_PKT )
        {
            bt_opps_flush_all_nbuffer();
            btmtk_goep_close_wstream(GOPS(h_ops_recv));
            GOPS(h_ops_recv) = FHANDLE_INVALID_VALUE;

            /* nofify register appications */
            BT_OPS_STATE_TRANS(BT_OPS_CONNECTED);
            btmtk_opps_ui_notify( BT_OPPSUI_PUSHED, (void *) GOEP_STATUS_SUCCESS );
            bt_opps_push_rsp(GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS);
            btmtk_opps_ui_notify( BT_OPPSUI_PUSHED, (void *) GOEP_STATUS_SUCCESS );
        }else{
            // send req for next packet
            bt_opps_push_rsp(GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS);
        }
    }else{
        // wait for user's to accept the incoming or reject it
        GOPS(ops_pkt_type) = ind->pkt_type;
        BT_OPS_STATE_TRANS(BT_OPS_ACCEPT_WAIT);
        //btmtk_os_start_timer(BT_OPP_PUSH_START_TIMER);
        btmtk_opps_ui_notify( BT_OPPSUI_PUSH_START, NULL );
    }
}
#endif

/*****************************************************************************
 * FUNCTION
 *  bt_opps_first_push_ind_routine
 * DESCRIPTION
 *  This function is OPP server event handler
 *        for first push object indication (MSG_ID_BT_GOEP_PUSH_IND)
 * PARAMETERS
 *  ind [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_opps_first_push_ind_routine(goep_push_ind_struct *ind)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char msg[128];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (GOPS(ops_state))
    {
    case BT_OPS_CONNECTED:
        // keep the total size, mime, objname
        bt_goep_truncate_ntoh_filename(GOPS(ops_obj_name), (U16 *)ind->obj_name, sizeof(GOPS(ops_obj_name)));
        GOEP_MEMSET(msg, 0, 128);
        ext_chset_ucs2_to_utf8_string( (U8 *)msg, sizeof(msg) - 1, (U8 *)GOPS(ops_obj_name) );
        BT_OPP_LOGD("[EXT] push_ind obj_name:(%s) ", msg);

        ext_strncpy( (S8 *)GOPS(ops_obj_mime) , (const S8 *)ind->obj_mime_type, OPP_MAX_OBJ_MIME_LENGTH);
        GOPS(ops_total_obj_len) = GOPS(ops_remain_put_len) = ind->total_obj_len;

        if( TRUE == bt_opps_is_auto( BT_OPP_AUTO_PUSH ) ){
            GOPS(ops_state) = BT_OPS_RECEIVING; // auto accept it.
            BT_OPP_LOGD("[EXT] unsupport feture:(%");
            bt_opps_push_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR);
        }else{
            BT_OPP_LOGD("[EXT] first push_ind and notify user to accept it obj_len:%d", GOPS(ops_total_obj_len));
            GOPS(ops_state) = BT_OPS_ACCEPT_WAIT;
            /// @see bt_opps_push_mem_routine
            btmtk_opps_ui_notify( BT_OPPSUI_PUSH_START, (void *) 0 );
            /// wait for invoke the data accept_push_mem or accept_push_file
        }

        break;

    case BT_OPS_DISCONNECTING:
    case BT_OPS_DEACTIVATING:
        /* Do Nothing */
        break;

    default:
        /* Wrong OPS state */
        BT_OPS_LOG_STATE_ERR();
        bt_opps_push_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR);
        btmtk_opps_ui_notify( BT_OPPSUI_ERROR, (void *) __LINE__ );
        break;
    }


}


/*****************************************************************************
 * FUNCTION
 *  bt_opps_next_push_ind_handler
 * DESCRIPTION
 *  This function is event handler for continuous next push object indication (MSG_ID_BT_GOEP_PUSH_IND)
 * PARAMETERS
 *  msg         [?]
 *  a(?)        [IN/OUT]        First variable, used as returns
 *  b(?)        [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_opps_normal_push_ind_routine(goep_push_ind_struct *ind)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    void * data;
    U32 datalen, written;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_LOGD(BT_OPP_OPPS_NEXT_PUSH_IND);
    // TODO BT_OPP_LOGD( "[EXT][ADP][bt_opps_normal_push_ind_routine][+]" );

    if ((GOPS(ops_state) == BT_OPS_DISCONNECTING) ||
        (GOPS(ops_state) == BT_OPS_DEACTIVATING))
    {
        // do nothing
        return;
    }

    if (GOPS(ops_state) != BT_OPS_RECEIVING && GOPS(ops_state) != BT_OPS_RECEIVING)
    {
        // wrong OPS state
        BT_OPS_LOG_STATE_ERR();
        bt_opps_push_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR);
        return;
    }

    GOPS(ops_state) = BT_OPS_RECEIVING;

    //BT_OPP_LOGD( "[OPP][ADP][bt_opps_normal_push_ind_routine]: buf-addr[%p]", ind->frag_ptr );
    data = ind->frag_ptr;
    datalen = ind->frag_len;

    // update content
    GOPS(ops_u4RecvSize) += datalen;
    GOPS(ops_remain_put_len) -= datalen;

    if (datalen > 0 && NULL != data)
    {
        //S32 retval;
        bt_opps_write_data_routine( data, datalen, &written, ind->pkt_type, FALSE );
    }

    bt_opps_push_rsp(GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS);
    btmtk_opps_ui_notify( BT_OPPSUI_PUSHING, (void *) GOPS(ops_u4RecvSize) );
}


/*****************************************************************************
 * FUNCTION
 *  bt_opps_final_push_ind_routine
 * DESCRIPTION
 *  This function is event handler for continuous final push object indication (MSG_ID_BT_GOEP_PUSH_IND)
 * PARAMETERS
 *  msg         [?]
 *  a(?)        [IN/OUT]        First variable, used as returns
 *  b(?)        [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_opps_final_push_ind_routine(goep_push_ind_struct *ind)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 l_rsp;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_LOGD(BT_OPP_OPPS_FINAL_PUSH_IND);

    if (GOPS(ops_state) == BT_OPS_DISCONNECTING ||
        GOPS(ops_state) == BT_OPS_DEACTIVATING)
    {
        /* Do Nothing */
        return;
    }

    if (GOPS(ops_state) != BT_OPS_RECEIVING && GOPS(ops_state) != BT_OPS_ACCEPT_WAIT )
    {
        /* Wrong OPS state */
        BT_OPS_LOG_STATE_ERR();
        bt_opps_push_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR);
        return;
    }

    //BT_OPP_LOG2(BT_OPP_OPPS_FINAL_PUSH_IND_FRAG_LEN, ind->frag_len, ind->total_obj_len);

    if (GOPS(stop_recv) || FALSE == ext_fs_is_valid_handle_ucs2(GOPS(h_ops_recv)) )
    {
        bt_opps_push_ind_write_error();
        return;
    }

    /* Final packet, Receive Finished !! */
    if (ind->frag_len > 0)
    {
        U32 len_written;
        S32 retval;

	 GOPS(ops_remain_put_len) -=ind->frag_len ;
	 GOPS(ops_u4RecvSize)+=ind->frag_len;

        retval = bt_opps_write_data_routine(
                ind->frag_ptr,
                ind->frag_len,
                &len_written,
                ind->pkt_type, FALSE);
        if( EXT_FS_OK != retval ){
            BT_OPS_STATE_TRANS(BT_OPS_CONNECTED);
            return;
        }
    }
    l_rsp = GOEP_STATUS_SUCCESS;

    goto FINAL_PUSH_END;

FINAL_PUSH_END:
    bt_opps_flush_all_nbuffer();
    btmtk_goep_close_rstream( GOPS(h_ops_recv) );
    GOPS(h_ops_recv) = FHANDLE_INVALID_VALUE;

    bt_opps_push_rsp(GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS);
    BT_OPS_STATE_TRANS(BT_OPS_CONNECTED);
    btmtk_opps_ui_notify( BT_OPPSUI_PUSHED, (void *) GOEP_STATUS_SUCCESS );
    /* nofify register appications */
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_opps_push_ind_handler
 * DESCRIPTION
 *  This function is OPP server event handler for push object indication (MSG_ID_BT_GOEP_PUSH_IND)
 * PARAMETERS
 *  msg         [?]
 *  a(?)        [IN/OUT]        First variable, used as returns
 *  b(?)        [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_opps_push_ind_handler(void *data)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	goep_push_ind_struct *ind = (goep_push_ind_struct*)data;

	// TODO BT_OPP_LOGD("[EXT] push_ind type:%d frag_len:%d mode:%d", ind->pkt_type, ind->frag_len, GOPS(ops_mem_mode));

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	if (ind->put_type != GOEP_PUT_NORMAL &&
	    ind->put_type != GOEP_CREATEEMPTY)
	{
	    // wrong put type, need clarify
		BT_OPP_LOGD("[EXT] wrong put-type:%d", ind->put_type);
	    bt_opps_push_rsp( GOPS(ops_goep_conn_id), GOEP_STATUS_NOT_SUPPORTED );
	    return;
	}

	GOPS(ops_pkt_type) = ind->pkt_type;
	GOPS(ops_pkt_len) = ind->frag_len;

	if( FALSE == GOPS(ops_mem_mode) ){
		//default value
		switch (ind->pkt_type)
		{
		case GOEP_FIRST_PKT:
		case GOEP_SINGLE_PKT:
		    // save object name to context
		    //bt_goep_truncate_ntoh_filename(GOPS(ops_obj_name),  (U16 *)ind->obj_name, sizeof(GOPS(ops_obj_name)));
		    bt_opps_first_push_ind_routine(ind);
		    break;
		case GOEP_NORMAL_PKT:
		    bt_opps_normal_push_ind_routine(ind);
		    break;
		case GOEP_FINAL_PKT:
		    bt_opps_final_push_ind_routine(ind);
		    break;
		default:
		    break;
		}
	}
	else {
		switch (ind->pkt_type)
		{
		case GOEP_FIRST_PKT:
		case GOEP_SINGLE_PKT:
			bt_opps_first_push_ind_routine(ind);
		    break;
		case GOEP_NORMAL_PKT:
		    bt_opps_push_mem_continue_routine(GOEP_STATUS_SUCCESS, ind->frag_ptr, ind->frag_len);
		    break;
		case GOEP_FINAL_PKT:
		    bt_opps_push_mem_continue_routine(GOEP_STATUS_SUCCESS, ind->frag_ptr, ind->frag_len);
		    btmtk_opps_ui_notify( BT_OPPSUI_PUSHED, (void *) GOEP_STATUS_SUCCESS);
		    BT_OPS_STATE_TRANS(BT_OPS_CONNECTED);
		    break;
		default:
		    break;
		}
	}
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_opps_abort_ind_handler
 * DESCRIPTION
 *
 * PARAMETERS
 *  msg     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_opps_abort_ind_handler(void *data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_LOGD(BT_OPP_OPPS_ABORT_IND);
    // TODO: error check the data is a abort_ind struct or not

    if (GOPS(ops_state) == BT_OPS_RECEIVING)
    {
        /* close file */
        bt_goep_clean_tempfile(GOPS(h_ops_recv), (U8 *)GOPS(recv_path), FALSE);
        GOPS(h_ops_recv) = FHANDLE_INVALID_VALUE;
        BT_OPS_STATE_TRANS(BT_OPS_CONNECTED);
        btmtk_opps_ui_notify(BT_OPPSUI_PUSHED, (void *)GOEP_FORBIDDEN);
    }
    else if(GOPS(ops_state) == BT_OPS_SENDING)
    {
        /* close vcard */
        if(  FALSE != ext_fs_is_valid_handle_ucs2(GOPS(h_ops_snd)) ){
            btmtk_goep_close_rstream( GOPS(h_ops_snd) );
            GOPS(h_ops_snd) = FHANDLE_INVALID_VALUE;
        }

        BT_OPS_STATE_TRANS(BT_OPS_CONNECTED);
    }
    else
    {
        bt_ext_err("[EXT] btmtk_opps_abort_ind_handler(): invalid ops_state[%d]", GOPS(ops_state));
        BT_OPS_STATE_TRANS(BT_OPS_CONNECTED);
    }

    /* send abort res to opp client */
    bt_opps_abort_rsp(GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS);
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_opps_pull_ind_handler
 * DESCRIPTION
 *
 * PARAMETERS
 *  msg     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_opps_pull_ind_handler(void *data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_pull_ind_struct *ind;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    /* do not support pull indication */
    /// check the receive msg
    ind = (goep_pull_ind_struct*) data;
    GOPS(ops_pkt_type) = ind->pkt_type;

    if( BT_OPS_ACTIVE == GOPS(ops_state) || BT_OPS_CONNECTED == GOPS(ops_state) || BT_OPS_SENDING == GOPS(ops_state) || BT_OPS_PREPARE_WAIT == GOPS(ops_state) ){
    }else{
        bt_opps_pull_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR, 0, 0 ,0, NULL);
        return;
    }
    switch( GOPS(ops_state) ){
		case BT_OPS_CONNECTED:
			{
				bt_goep_truncate_ntoh_filename( (U16 *)GOPS(ops_obj_name) , (const U16 *) ind->obj_name,  sizeof(GOPS(ops_obj_name)) );
				ext_strncpy( (S8 *)GOPS(ops_obj_mime) , (const S8 *)ind->obj_mime_type, OPP_MAX_OBJ_MIME_LENGTH); // TODO: mime is a ascii

				if( bt_opps_is_auto( BT_OPP_AUTO_PUSH ) || BT_OPS_PREPARE_WAIT == GOPS(ops_state) ){
					// first pull vcard ind
					BT_OPP_LOGD("[EXT][ERR] btmtk_opps_pull_ind_handler reject pull vcard");
					bt_opps_pull_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR, 0, 0, 0, NULL);
				}else{
					// UI notify @see
					GOPS(ops_state) = BT_OPS_PREPARE_WAIT;
					btmtk_opps_ui_notify( BT_OPPSUI_PULL_START, NULL );
				}
			}
			break;
		case BT_OPS_SENDING:
			if( FALSE == GOPS(ops_mem_mode) ){
				if( FALSE == ext_fs_is_valid_handle_ucs2(GOPS(h_ops_snd)) ){
					GOEP_ASSERT(0);
	            }else{
					bt_opps_pull_file_continue_routine(GOEP_STATUS_SUCCESS);
				}
			}else{
				bt_opps_pull_mem_continue_routine();
			}
			break;
		default:
			bt_opps_pull_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR, 0, 0, 0, NULL);
			break;
	}
}


/**
* @brief This function is to handle authorize indication from OBEX stack
* @param msg goep_authorize_ind_struct
**/
void btmtk_opps_authorize_ind_handler(void *data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_authorize_ind_struct *ind = (goep_authorize_ind_struct*)data;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ind->uuid_len != 0 || ind->uuid[0] != '\0')
    {
        BT_OPP_ASSERT(0);
    }
    if (GOPS(ops_state) == BT_OPS_ACTIVE)
    {
        GOPS(ops_dev_addr).lap = (U32)ind->bd_addr.lap;
        GOPS(ops_dev_addr).uap = (U8)ind->bd_addr.uap;
        GOPS(ops_dev_addr).nap = (U16)ind->bd_addr.nap;
        GOPS(ops_goep_conn_id) = ind->goep_conn_id;
    }
#ifdef BT_OPP_AUTO_CHECK
    if( bt_opps_is_auto(BT_OPP_AUTO_AUTHORIZATION) ){
        if (GOPS(ops_state) == BT_OPS_ACTIVE)
        {
            /* accept without query user */
            GOPS(ops_state) = BT_OPS_CONNECTED;
            bt_opps_authorize_rsp(GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS);
        }
        else
        {
            bt_opps_authorize_rsp(ind->goep_conn_id, GOEP_SERVICE_UNAVAILABLE);
            /* Wrong OPS state */
            BT_OPS_LOG_STATE_ERR();
        }
    }
    else
#endif
    {
        /* send notify and wait for btmtk_authoriz_response */
        GOPS(ops_state) = BT_OPS_CONNECTING;
        btmtk_opps_ui_notify( BT_OPPSUI_AUTHORIZING, (void *)  &GOPS(ops_dev_addr) );
    }
}


/******************** Internal functions ********************/

/**
* @brief bt_opps_connection_terminated
* This is common function to deal with ops disconnect
* @param cause the terminate cause
**/
static void bt_opps_connection_terminated(U8 cause)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //TODO BT_OPP_LOG1(BT_OPP_OPPS_CONN_TERM, cause);
    BT_OPP_LOG_STATE();

    if( FALSE != ext_fs_is_valid_handle_ucs2(GOPS(h_ops_recv)) ){
        btmtk_goep_close_wstream(GOPS(h_ops_recv));
        GOPS(h_ops_recv) = FHANDLE_INVALID_VALUE;
		btmtk_goep_delete_file( GOPS(recv_path) );
		GOPS(recv_path)[0] = GOPS(recv_path)[1] = 0;
    }

    if(  FALSE != ext_fs_is_valid_handle_ucs2(GOPS(h_ops_snd)) ){
        btmtk_goep_close_wstream(GOPS(h_ops_snd));
        GOPS(h_ops_snd) = FHANDLE_INVALID_VALUE;
    }

	if( BT_OPS_RECEIVING == GOPS(ops_state) ){
		btmtk_opps_ui_notify( BT_OPPSUI_PUSHED, (void *) (cause==0?GOEP_USER_UNKNOW:cause) );
	}
    if( BT_OPS_SENDING == GOPS(ops_state) ){
		if( GOPS(ops_remain_put_len) != 0 ){
		    btmtk_opps_ui_notify( BT_OPPSUI_PULLED, (void *) (cause==0?GOEP_USER_UNKNOW:cause));
		}else{
			btmtk_opps_ui_notify( BT_OPPSUI_PULLED, (void *) 0); // pull a empty vcard from OPP server MMI
		}
	}

    /* reset OPPS connection related context */
    GOPS(ops_goep_req_id) = OPP_INVALID_REQ_ID;
    GOPS(ops_cm_conn_id) = OPP_INVALID_CM_ID;
    GOPS(ops_disconnflag) = FALSE;

    BT_OPS_STATE_TRANS(BT_OPS_ACTIVE);

    btmtk_opps_ui_notify( BT_OPPSUI_DISCONNECTED, (U8 *)((U32)cause) );
}


static void bt_opps_flush_one_nbuffer(){
    S32 i4Read, i4Written;
	if( FALSE != ext_nbuffer_isInit(&GOPS(ops_mempool)) ){
    if( FALSE != ext_nbuffer_pop_data(&GOPS(ops_mempool), GOPS(ops_push_buff), OPPS_MAX_OBEX_PACKET_LENGTH, &i4Read) ){
        GOPS(ops_fs_status) = btmtk_goep_write_wstream( GOPS(h_ops_recv), GOPS(ops_push_buff), i4Read, &i4Written);
        BT_OPP_ASSERT(i4Read != i4Written);
        if( EXT_FS_OK !=  GOPS(ops_fs_status)){
            return;
        }
    }else{
    }
	}
}

static void bt_opps_flush_all_nbuffer(){
	S32 i4Read, i4Written;
	U8  index;
	// write all the mem block in nbufer
	if( FALSE != ext_nbuffer_isInit(&GOPS(ops_mempool)) ){
		for( index = 0; index < NBUFFER_NUM_OF_MEM; index++){
		    if( FALSE != ext_nbuffer_pop_data(&GOPS(ops_mempool), GOPS(ops_push_buff), OPPS_MAX_OBEX_PACKET_LENGTH, &i4Read) ){
		        GOPS(ops_fs_status) = btmtk_goep_write_wstream( GOPS(h_ops_recv), GOPS(ops_push_buff), i4Read, &i4Written);
		        BT_OPP_ASSERT(i4Read != i4Written);
		        if( EXT_FS_OK !=  GOPS(ops_fs_status)){
		            return;
		        }
		    }else{
		        break;
		    }
		}
	}
}



/******************* INIT *****************************/


/******************* OPPS Content Methods ***********************/
BT_BOOL bt_opps_reject_pull_vcard_routine(U32 u4Rsp){

    if(GOPS(ops_state) != BT_OPS_PREPARE_WAIT){
        return FALSE;
    }
    GOPS(ops_state) = BT_OPS_CONNECTED;
    if( GOEP_STATUS_SUCCESS == u4Rsp ){
    	  u4Rsp = GOEP_INTERNAL_SERVER_ERR; //
    }
    bt_opps_pull_rsp( GOPS(ops_goep_conn_id), (U8 )u4Rsp, GOEP_FINAL_PKT, 0, 0, "" );
    return TRUE;
}


BT_BOOL bt_opps_pull_file_routine(U32 u4Rsp,const U8 *filepath){
    U32 i4Size;
    if(GOPS(ops_state) != BT_OPS_PREPARE_WAIT){
        return FALSE;
    }

    if( GOEP_STATUS_SUCCESS != u4Rsp ){
        return bt_opps_reject_pull_vcard_routine( u4Rsp );
    }

   if( FALSE == ext_fs_is_valid_handle_ucs2(GOPS(h_ops_snd)) ){
        // first pull vcard ind
        GOPS(h_ops_snd) = (FHANDLE) btmtk_goep_open_rstream( (const U16 *)filepath, 0);

        if( FALSE == ext_fs_is_valid_handle_ucs2(GOPS(h_ops_snd)) ){
        	// TODO: log
            bt_opps_pull_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR, 0, 0, 0, NULL);
            GOPS(ops_state) = BT_OPS_CONNECTED;
            btmtk_opps_ui_notify(BT_OPPSUI_PULLED, (void *) GOEP_INTERNAL_SERVER_ERR);
            return FALSE;
        }
        if( FALSE == btmtk_goep_get_filesize( GOPS(h_ops_snd), (U8 *)filepath, &i4Size) ){
        	// TODO: log
            bt_opps_pull_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR, 0, 0, 0, NULL);
            GOPS(ops_state) = BT_OPS_CONNECTED;
            btmtk_opps_ui_notify(BT_OPPSUI_PULLED, (void *) GOEP_INTERNAL_SERVER_ERR);
            return FALSE;
        }
        GOPS(ops_mem_mode) = FALSE;
        GOPS(ops_state) = BT_OPS_SENDING;
        GOPS(ops_remain_put_len) = GOPS(ops_total_obj_len) = i4Size;

        bt_opps_pull_rsp( GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS, GOEP_FIRST_PKT, i4Size, 0, "text/x-vcard" );
        btmtk_opps_ui_notify( BT_OPPSUI_PULLING, (void *) 0 );

    }else{
        // close file and pull rsp error
        btmtk_goep_close_rstream(GOPS(h_ops_snd));
        GOPS(h_ops_snd) = FHANDLE_INVALID_VALUE;
        bt_opps_pull_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR, 0, 0, 0, NULL);
        btmtk_opps_ui_notify(BT_OPPSUI_PULLED, (void *) GOEP_INTERNAL_SERVER_ERR);
        return FALSE;
    }

    return TRUE;
}

BT_BOOL bt_opps_pull_file_continue_routine(U32 u4RspCode){
	S32 i4Read;
	S32 fs_status;

	if( GOEP_STATUS_SUCCESS != u4RspCode ){
		return TRUE;
	}

	if( BT_OPS_SENDING != GOPS(ops_state) ){
		GOEP_ASSERT(0);
		return FALSE;
	}

	// get from file into obj_buffer
	if( FALSE != ext_fs_is_valid_handle_ucs2(GOPS(h_ops_snd)) ){
		fs_status = btmtk_goep_read_rstream(GOPS(h_ops_snd), GOPS(ops_push_buff) , GOPS(ops_mtu), &i4Read);
		switch( fs_status ){

			case EXT_FS_OK:
				GOPS(ops_state) = BT_OPS_SENDING;
				if( GOPS(ops_remain_put_len) >= (U32)i4Read ){
					GOPS(ops_remain_put_len) -= i4Read;
				}else{
					GOEP_ASSERT(0);
				}
				if( 0 != GOPS(ops_remain_put_len) ){
					bt_opps_pull_rsp( GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS, GOEP_NORMAL_PKT, 0, i4Read, NULL );
					btmtk_opps_ui_notify( BT_OPPSUI_PULLING, (void *) (GOPS(ops_total_obj_len)-GOPS(ops_remain_put_len)) );
				}else{

			 		btmtk_goep_close_rstream( GOPS(h_ops_snd) );
			 		GOPS(ops_remain_put_len) = 0;
					GOPS(h_ops_snd) = FHANDLE_INVALID_VALUE;
					GOPS(ops_state) = BT_OPS_CONNECTED;
					bt_opps_pull_rsp( GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS, GOEP_FINAL_PKT, 0, i4Read, NULL );
					btmtk_opps_ui_notify( BT_OPPSUI_PULLED, (void *) GOEP_STATUS_SUCCESS );
				}
			break;
			case EXT_FS_ERR_EOF:
		 		btmtk_goep_close_rstream( GOPS(h_ops_snd) );
		 		GOPS(ops_remain_put_len) = 0;
				GOPS(h_ops_snd) = FHANDLE_INVALID_VALUE;
				GOPS(ops_state) = BT_OPS_CONNECTED;
				bt_opps_pull_rsp( GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS, GOEP_FINAL_PKT, 0, i4Read, NULL );
			break;
			default:
				btmtk_goep_close_rstream( GOPS(h_ops_snd) );
				bt_opps_pull_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR, 0, 0, 0, NULL);
				btmtk_opps_ui_notify( BT_OPPSUI_PULLED, (void *) GOEP_INTERNAL_SERVER_ERR );
				return FALSE;
			break;
		}
	}else{
		bt_opps_pull_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR, 0, 0, 0, NULL);
		return FALSE;
	}
	return TRUE;
}

void btmtk_opps_internal_rw_handler(void *data){
	switch( GOPS(ops_fs_status) ){
		case BT_OPS_RECEIVING:
			if( FALSE ==  GOPS(ops_mem_mode) && FALSE != ext_nbuffer_isInit(&GOPS(ops_mempool)) ){
			    if( FALSE == ext_nbuffer_isEmpty(&GOPS(ops_mempool)) ){
			        bt_opps_flush_one_nbuffer();
			        bt_opps_send_msg_toself( MSG_ID_BT_OPPS_SERVER_INTERNAL_RW, NULL, 0);
			    }
			}
		break;
		default:
		break;
	}
}

/**
* @brief Check the auto feature is on or not.
* @param feature bitmask of authorizating / pushing /pulling
* @param TRUE auto is on
* @see
*/
BT_BOOL bt_opps_is_auto(U32 feature){
    if( feature & GOPS(ops_auto_accept) ){
        return TRUE;
    }
    return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  bt_opps_set_fs_flag_routine
 * DESCRIPTION
 *  This function is to set fs error flag
 * PARAMETERS
 * err    [IN]
 * RETURNS
 *  void
 *****************************************************************************/
#if 0
void bt_opps_set_fs_flag_routine(S32 err)
{
    /// set the error read/write flag from file access function
    GOPS(ops_fs_status) = err;
}
#endif


#if 0
/*****************************************************************************
 * FUNCTION
 *  bt_opps_reject_file_recv_routine
 * DESCRIPTION
 *  This function is to reject receving file object.
 * PARAMETERS
 *  void
 *  a(?)        [IN/OUT]        First variable, used as returns
 *  b(?)        [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void bt_opps_reject_file_recv_routine()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_LOGD(bt_opps_reject_file_recv_routine);
    //BT_OPP_LOG_STATE();
}
#endif

BT_BOOL bt_opps_pull_mem_routine(const U8 *ucName, const U8 *ucMime, U8 *ucBuffer, U32 u4MaxSize){
	if( NULL == ucBuffer &&  u4MaxSize >0 ){
		BT_OPP_LOGD("[EXT] func:pull_mem_routine wrong parameter\n");
		return FALSE;
	}
	GOPS(ops_mem_mode) = TRUE;
	GOPS(ops_snd_buffer) = ucBuffer;
	GOPS(ops_snd_maxsize) = u4MaxSize;
	GOPS(ops_snd_offset) = 0;
	GOPS(stop_recv) = FALSE;
	GOPS(ops_total_obj_len) = GOPS(ops_remain_put_len) = u4MaxSize;

	// send the first packet back ( length, name, mime info) without data
	bt_opps_pull_rsp(GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS, GOEP_FIRST_PKT, u4MaxSize, 0, GOPS(ops_obj_mime));

	// change to pull state
	BT_OPS_STATE_TRANS(BT_OPS_SENDING);

	return TRUE;
}

BT_BOOL bt_opps_pull_mem_continue_routine(void){
	/* copy data into buffer GOPS(ops_push_buff) */
	U32 len;
	goep_pkt_type_enum pkt;

	if( GOPS(ops_snd_offset) > GOPS(ops_snd_maxsize)){

		GOEP_ASSERT(0);
		return FALSE;
	}

	if( FALSE == GOPS(ops_mem_mode) ){
		GOEP_ASSERT(0);
		return FALSE;
	}

	if( TRUE == GOPS(stop_recv)){
		// abort current transmission
		bt_opps_pull_rsp(GOPS(ops_goep_conn_id), GOEP_INTERNAL_SERVER_ERR, 0, 0 ,0, NULL);
		return FALSE;
	}

	len = min( (GOPS(ops_snd_maxsize) - GOPS(ops_snd_offset)), GOPS(ops_push_buff_size) );
	len = min( GOPS(ops_mtu), len );
	if( len >  0){
		GOEP_MEMCPY( (U8 *) GOPS(ops_push_buff), GOPS(ops_snd_buffer) + GOPS(ops_snd_offset) , len);
	}
	GOPS(ops_remain_put_len) -= len;

	if( (GOPS(ops_snd_offset) + len)>= GOPS(ops_snd_maxsize)){
		// final packet.
		pkt = GOEP_FINAL_PKT;
		BT_OPS_STATE_TRANS(BT_OPS_CONNECTED);
	}else{
		pkt = GOEP_NORMAL_PKT;
	}
	bt_opps_pull_rsp(GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS, pkt, GOPS(ops_snd_maxsize), len, GOPS(ops_obj_mime));

	GOPS(ops_snd_offset) += len;
	if( GOEP_FINAL_PKT == pkt ){
		btmtk_opps_ui_notify( BT_OPPSUI_PULLED, (void *) GOEP_STATUS_SUCCESS );
	}else{
		btmtk_opps_ui_notify( BT_OPPSUI_PULLING, (void *) ( GOPS(ops_total_obj_len)  - GOPS(ops_remain_put_len)));
	}
	return TRUE;

}

/*****************************************************************************
 * FUNCTION
 *  bt_opps_push_file_routine
 * DESCRIPTION
 *  This function is to accept receving file object.
 * PARAMETERS
 *  void
 *  a(?)        [IN/OUT]        First variable, used as returns
 *  b(?)        [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
BT_BOOL bt_opps_push_file_routine(U8 ucRspCode, const U8 *ucFilePath)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	U32 i4Written;
//	char filepath[1028];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_LOGD(bt_opps_push_file_routine);
    //BT_OPP_LOG_STATE();
    if (GOPS(ops_state) != BT_OPS_ACCEPT_WAIT){
        bt_ext_err( "[EXT][err] func:opps_access_file_recv wrong state state:%d\n",GOPS(ops_state) );
        return FALSE;
    }

//    GOEP_MEMSET(filepath, 0, sizeof(filepath) );
//    ext_chset_ucs2_to_utf8_string( (U8 *)filepath, sizeof(filepath) - 1, (U8 *)ucFilePath );
//    BT_OPP_LOGI("[EXT] push_file rspcode:%d filepath:(%s) ", ucRspCode, filepath);

	//btmtk_os_stop_timer(BT_OPP_PUSH_START_TIMER);
	if( GOEP_STATUS_SUCCESS == ucRspCode )
	{
		if (GOPS(ops_state) == BT_OPS_ACCEPT_WAIT)
		{
			BT_OPS_STATE_TRANS(BT_OPS_RECEIVING);
			/// open ucFilePath and save it
			GOPS(ops_fs_status) = EXT_FS_OK;
			GOPS(stop_recv) = FALSE;
			GOPS(ops_mem_mode) = FALSE;

			// try to create folder first. some platform AP don't create the folder first
			BT_OPP_LOGD( "[EXT] bt_opps_push_file_routine: try to create folder..." );
			ext_ucs2ncpy( (S8 *) GOPS(ops_obj_path), (const S8 *) L"", 4);
			if( TRUE == ext_goep_get_parent_folder_ucs2( (const U8 *) ucFilePath, (U8 *) GOPS(ops_obj_path), sizeof(GOPS(ops_obj_path)) ) )
			{
				BT_OPP_LOGD( "[EXT] bt_opps_push_file_routine: check foler exist:[%s]", GOPS(ops_obj_path) );
				if( FALSE == btmtk_goep_is_folder_exist( (const U16 *) GOPS(ops_obj_path)) && 0 < ext_ucs2strlen(GOPS(ops_obj_path)) ){
					/// create folder
					BT_OPP_LOGD( "[EXT] bt_opps_push_file_routine: create folder:[%s]", GOPS(ops_obj_path) );
					btmtk_goep_create_folder( (const U16 *) GOPS(ops_obj_path) );
				}
			}
			ext_ucs2ncpy( (S8 *)GOPS(ops_obj_path), (S8 *)ucFilePath, sizeof(GOPS(ops_obj_path))/2 );
            ext_ucs2ncpy( (S8 *)GOPS(recv_path), GOPS(ops_obj_path), sizeof(GOPS(ops_obj_path))/2 );
//			GOEP_MEMSET(filepath, 0, sizeof(filepath) );
//			ext_chset_ucs2_to_utf8_string( (U8 *)filepath, sizeof(filepath) - 1, (U8 *)GOPS(ops_obj_path) );
//			BT_OPP_LOGI( "[EXT] bt_opps_push_file_routine: opening write stream:[%s]", filepath );

			GOPS(h_ops_recv) = btmtk_goep_open_wstream( (const U16*) GOPS(ops_obj_path), BTMTK_FS_CREATE );
			if( FALSE == btmtk_goep_is_valid_handle(GOPS(h_ops_recv)) )
			{
				BT_OPP_LOGD("[EXT] push_file h_recv:%d", GOPS(h_ops_recv));
				GOPS(ops_fs_status) = GOPS(h_ops_recv);
				bt_opps_push_ind_write_error();
				btmtk_opps_ui_notify( BT_OPPSUI_PUSHED, (void *) GOEP_INTERNAL_SERVER_ERR );
				return FALSE;
			}

			// write data
			GOPS(ops_fs_status) = btmtk_goep_write_wstream( GOPS(h_ops_recv), GOPS(ops_push_buff), GOPS(ops_pkt_len) , (S32*)&i4Written);
			GOPS(ops_remain_put_len) -=i4Written;

			BT_OPP_LOGD("[EXT] write_wstream status:%d remain:%d",GOPS(ops_fs_status),GOPS(ops_remain_put_len) );

			if( GOPS(ops_pkt_type) == GOEP_FINAL_PKT || GOPS(ops_pkt_type) == GOEP_SINGLE_PKT ){
				btmtk_goep_close_wstream(GOPS(h_ops_recv));
				GOPS(h_ops_recv) = FHANDLE_INVALID_VALUE;

				BT_OPS_STATE_TRANS(BT_OPS_RECEIVING);
				btmtk_opps_ui_notify( BT_OPPSUI_PUSHING, (void *) GOPS(ops_u4RecvSize));
				BT_OPS_STATE_TRANS(BT_OPS_CONNECTED);
				btmtk_opps_ui_notify( BT_OPPSUI_PUSHED, (void *) GOEP_STATUS_SUCCESS );
			}else{
				btmtk_opps_ui_notify( BT_OPPSUI_PUSHING, 0);
			}
                bt_opps_push_rsp(GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS);
            }else{
                btmtk_opps_ui_notify( BT_OPPSUI_ERROR, (U8 *)GOPS(ops_state)); //
            }
      }else{
            BT_OPS_STATE_TRANS(BT_OPS_CONNECTED);
            bt_opps_push_rsp(GOPS(ops_goep_conn_id), ucRspCode);
      }
      return TRUE;
}

#if 0
void bt_opps_reject_connection_routine(BT_BOOL need_rsp)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    // send the tpdisconnect to current connection

}
#endif

/**
* @brief write data to opened pushing file handle
*  bt_opps_write_data_routine
*  This function is to write file
*
*/
S32 bt_opps_write_data_routine(void * data, U32 len, U32 * written, goep_pkt_type_enum pkt_type, BT_BOOL close_only)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 retval = EXT_FS_ERR;
    S32 u4Index, u4BuffSize;
    U8 *pBuffer;
    FHANDLE handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    handle = GOPS(h_ops_recv);
    if( NULL == written ){
        GOEP_ASSERT(0);
    }
#if 0
disable nbuffer
    if( FALSE != ext_nbuffer_isInit(&GOPS(ops_mempool)) ){

        // TODO BT_OPP_LOGD( "[EXT][ADP][nbuffer ready]" );

	    if( FALSE != ext_nbuffer_isFull( &GOPS(ops_mempool) ) ){
	        // write all data and
	        bt_opps_flush_all_nbuffer();
	        if( EXT_FS_OK == GOPS(ops_fs_status) ){
	            // TODO BT_OPP_LOGD( "[EXT][ADP][nbuffer ready]: call btmtk_goep_write_wstream: buf[%p]size[len]", data, len );
	            retval = btmtk_goep_write_wstream(handle, data, (S32) len, written); //X: FS_Write(handle, data, len, written);
	            // TODO BT_OPP_LOGD( "[EXT][ADP][btmtk_goep_write_wstream ok]" );
	        }
	    }else{
	        if( FALSE != ext_nbuffer_isEmpty( &GOPS(ops_mempool) )) {
	            bt_opps_send_msg_toself( MSG_ID_BT_OPPS_SERVER_INTERNAL_RW, NULL, 0);
	        }
	        // put to end of mempool
	        pBuffer = ext_nbuffer_get_tail_buffer( &GOPS(ops_mempool), &u4Index, &u4BuffSize);
	        BT_OPP_ASSERT( NULL != pBuffer );
	        BT_OPP_ASSERT( len > u4BuffSize );
	        GOEP_MEMCPY( pBuffer, data, min(len, (U16)u4BuffSize) );
	        if( FALSE != ext_nbuffer_return_tail_buffer( &GOPS(ops_mempool), u4Index, min(len, (U16) u4BuffSize) ) ){
	            retval = EXT_FS_OK;
	        }else{
	            retval = EXT_FS_ERR;
	        }
	    }
    }
    else
#endif
    {
        if( EXT_FS_OK == GOPS(ops_fs_status) ){

            retval = btmtk_goep_write_wstream(handle, data, (S32)len, (S32 *)written); //X: FS_Write(handle, data, len, written);
        }
    }

    // write data error handling
    if (EXT_FS_OK != retval || EXT_FS_OK != GOPS(ops_fs_status) )
    {
        bt_goep_clean_tempfile(GOPS(h_ops_recv), (U8 *) GOPS(recv_path) ,close_only);
        GOPS(ops_fs_status) = retval;
        if (pkt_type == GOEP_NORMAL_PKT)
        {
            GOPS(stop_recv) = TRUE;
            return retval;
        }
        bt_opps_push_ind_write_error();
    }
    return retval;
}


/**
* @brief first routine to accept received data
*/
BT_BOOL bt_opps_push_mem_routine(U8 ucRspcode, U8 *ucBuffer, U32 u4MaxSize){
    if( BT_OPS_ACCEPT_WAIT != GOPS(ops_state) ){
        GOEP_ASSERT(0);
    }

    if( GOEP_STATUS_SUCCESS != ucRspcode ){
	    bt_opps_push_rsp(GOPS(ops_goep_conn_id), ucRspcode);
	    return TRUE;
    }

    GOPS(ops_mem_mode) = TRUE;
    GOPS(ops_recv_buffer) = ucBuffer;
    GOPS(ops_recv_maxsize) = u4MaxSize;
    GOPS(ops_recv_offset)  = 0;

    // copy data into ucBuffer
    if( GOPS(ops_recv_maxsize) < GOPS(ops_total_obj_len) ){
        BT_OPP_LOGD("[EXT][err] func:push_mem buff too small total:%d ucMaxSize:%d\n", GOPS(ops_total_obj_len), u4MaxSize);
        bt_opps_push_rsp(GOPS(ops_goep_conn_id), GOEP_NOT_ACCEPTABLE);
        btmtk_opps_ui_notify( BT_OPPSUI_PUSHED, (void *)GOEP_NOT_ACCEPTABLE );
		GOPS(ops_state) = BT_OPS_CONNECTED;
        return FALSE; ///
    }

    GOEP_MEMCPY( (U8 *)GOPS(ops_recv_buffer), GOPS(ops_push_buff), GOPS(ops_pkt_len));
    GOPS(ops_recv_offset) += GOPS(ops_pkt_len);
    GOPS(ops_remain_put_len) -= GOPS(ops_pkt_len);

    // change to connected
    GOPS(ops_state) = BT_OPS_RECEIVING;
    // send rsp back
    btmtk_opps_ui_notify( BT_OPPSUI_PUSHING, (void *) GOPS(ops_u4RecvSize) );

    bt_opps_push_rsp(GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS);
    if( GOEP_SINGLE_PKT == GOPS(ops_pkt_type) || GOEP_FINAL_PKT == GOPS(ops_pkt_type) ){
        GOPS(ops_state) = BT_OPS_CONNECTED;
        btmtk_opps_ui_notify( BT_OPPSUI_PUSHED, GOEP_STATUS_SUCCESS );
    }else{
        // keep wait the next
    }
    btmtk_opps_ui_notify( BT_OPPSUI_PUSHING, (void *) GOPS(ops_recv_offset));
    return TRUE;
}

BT_BOOL bt_opps_push_mem_continue_routine(U8 ucRspcode, U8 *ucData, U32 u4DataLen)
{

	// save data into push memory and send rsp
	if( GOEP_STATUS_SUCCESS != ucRspcode ){
		btmtk_opps_ui_notify( BT_OPPSUI_PUSHED, (U8 *)((U32)ucRspcode) );
		return FALSE;
	}

	if( FALSE == GOPS(ops_mem_mode) ){
		GOEP_ASSERT(0);
		return FALSE;
	}

	// copy into memory
	if( u4DataLen > (GOPS(ops_recv_maxsize)  - GOPS(ops_recv_offset) )){
		btmtk_opps_ui_notify( BT_OPPSUI_PUSHED, (void *)GOEP_STATUS_NO_RESOURCES );
		return FALSE;
	}

	GOEP_MEMCPY( GOPS(ops_recv_buffer) + GOPS(ops_recv_offset), ucData, u4DataLen);
	GOPS(ops_recv_offset) += u4DataLen;
	GOPS(ops_remain_put_len) -= u4DataLen;

	bt_opps_push_rsp( GOPS(ops_goep_conn_id), GOEP_STATUS_SUCCESS);
	btmtk_opps_ui_notify( BT_OPPSUI_PUSHING, (void *) GOPS(ops_recv_offset));

	return TRUE;

}
