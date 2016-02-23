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
 * bt_opps_api.c
 *
 * Project:
 * --------
 *
 *
 * Description:
 * ------------
 *   Export API of OPP Server
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
 * 09 03 2012 dexiang.jiang
 * [ALPS00344419] 蓝牙在接受android.bluetooth.profilemanager.action.DISABLE_PROFILES广播时出现了ANR
 * OPP external callback reset to avoid NE.
 *
 * 04 24 2012 dexiang.jiang
 * [ALPS00270904] [ICS TDD][BT] happen NE at system/bin/mtkbt
 * Use special req_id(large value) to pervent multi obex-based profiles conexistence conflict.
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
#include "bt_opp_porting.h"
//#include "goep.h"

#include "bt_goep_porting.h"

#include "bt_opp_comm.h"
#include "bt_opps_api.h"
#include "bt_opps_hdl.h"
#include "bt_opps_msg.h"
#include "bt_opps_struct.h"
#include "ext_ucs2.h"
#include "bt_ext_debug.h"


/* OPPC UI conexte */
BTMTK_OPP_CALLBACK g_opps_ui_callback = NULL;

///// INTERNAL FUNCTION /////
static int g_opps_is_active = FALSE;

BT_BOOL bt_opps_is_running(){
    U32 status;
    status = GOPS(ops_state);
    if( BT_OPS_ACCEPT_WAIT == status || BT_OPS_RECEIVING == status){
        return TRUE;
    }
    return FALSE;
}

BT_BOOL btmtk_opps_get_status(bt_opps_status_struct *status){
    U32 len;

    if( NULL == status || status->size != sizeof(bt_opps_status_struct) ){
    	BT_OPP_LOGD("[EXT] size=%d size =%d\n", status->size, sizeof(bt_opps_status_struct));
        return FALSE;
    }
    GOEP_MEMSET( (U8*) status, 0, sizeof(bt_opps_status_struct) );
    status->size = sizeof(bt_opps_status_struct);
    GOEP_MEMCPY( (U8*)&status->remote_dev_addr, (const U8*)&GOPS(ops_dev_addr), sizeof(GOPS(ops_dev_addr)) );
    GOEP_MEMCPY( (U8*)status->dev_name, (const U8*)GOPS(dev_name), sizeof( GOPC(dev_name) ) );
    GOEP_MEMCPY( (U8*)status->obj_name, (const U8*)GOPS(ops_obj_name), sizeof(GOPC(ops_obj_name)) );
    GOEP_MEMCPY( (U8*)status->obj_mime, (const U8*)GOPS(ops_obj_mime), sizeof(GOPC(ops_obj_mime)) );
    if( NULL != GOPS(ops_obj_path) ){
        len = 2 + ext_ucs2strlen( (const S8 *)GOPC(opc_obj_path)) * 2;
        GOEP_MEMCPY( (U8 *)status->obj_path, (const U8 *) GOPC(opc_obj_path), min(sizeof(status->obj_path),len) );
    }
    status->state = GOPC(ops_state);
    status->totalsize = GOPS(ops_total_obj_len) ;
    status->remainsize = GOPS(ops_remain_put_len);
    // TODO BT_OPP_LOGD( "[EXT][ADP][btmtk_opps_get_status]: total[%d], remaining[%d]", status->totalsize, status->remainsize );
    return TRUE;
}

void btmtk_opps_dump_status(bt_opps_status_struct *status){
}

BT_BOOL btmtk_opps_setup_auto_response(U32 a){
    return TRUE;
}

/*
 * Init functions
 */
/*****************************************************************************
 * FUNCTION
 *  bt_opps_activate
 * DESCRIPTION
 *  This function is to activate OPS module
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
BT_BOOL btmtk_opps_activate_server(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	BT_OPP_LOGD("[EXT] func:opps_activate enter");

	if( TRUE == g_opps_is_active ){
		BT_OPP_LOGD("[EXT] func:opps_activate ok g_opps_is_active is true\n");
		return TRUE;
	}
    GOPS(ops_state) = BT_OPS_IDLE;

    GOPS(h_ops_recv) = (FHANDLE) FHANDLE_INVALID_VALUE;
    GOPS(h_ops_snd) = (FHANDLE) FHANDLE_INVALID_VALUE;

	GOPS(req_id) = OPP_DEFAULT_REQ_ID;
    GOPS(ops_goep_conn_id) = OPP_INVALID_CONN_ID;
    GOPS(ops_goep_req_id) = OPP_INVALID_REQ_ID;
    GOPS(ops_cm_conn_id) = OPP_INVALID_CM_ID;

    GOPS(ops_disconnflag) = FALSE;
    GOEP_MEMSET( (U8*) GOPS(recv_path), 0, sizeof( GOPS(recv_path) ));

    GOPS(ops_auto_accept) = BT_OPP_AUTO_AUTHORIZATION;//  0; ///answer the ind with ok rsp
    GOPS(ops_support_feature) = 0xff; ///all support

    OPPS_CS_INIT();

    //BT_OPP_LOGD(BT_OPP_OPPS_ACTIVATE);
    BT_OPP_LOGD( "[EXT] Active \n");

    if (GOPS(ops_state) != BT_OPS_IDLE)
    {
        return FALSE;
    }
    GOPS(ops_push_buff) = btmtk_opps_get_profile_buffer(btmtk_opps_get_profile_buffer_size());
    GOPS(ops_push_buff_size) = btmtk_opps_get_profile_buffer_size();

    if( NULL == GOPS(ops_push_buff)  || 256 >  GOPS(ops_push_buff_size)){
        BT_OPP_LOGD("[EXT][err] func: activate push_buf:0x%08x push_size:%d\n",  GOPS(ops_push_buff), GOPS(ops_push_buff_size));
        return FALSE;
    }
    GOEP_MEMSET( (U8 *)GOPS(recv_path), 0, sizeof( GOPS(recv_path) ));
    GOEP_MEMSET( (U8 *) &GOPS(ops_mempool), 0, sizeof(GOPS(ops_mempool)));

    if( FALSE == ext_nbuffer_init(&GOPS(ops_mempool), OPPS_MAX_OBEX_PACKET_LENGTH) ){
        BT_OPP_LOGD("[EXT][WRN] nbuffer init fail !");
        //return;
    }

	g_opps_is_active = TRUE;

    /* Register OBEX server instance first, then register SDP service record first */
	BT_OPS_STATE_TRANS(BT_OPS_OBEX_REGISTERING);	// change state first, or register_cnf may meet a error state
    bt_opps_obex_register_server_req();
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  bt_opps_deactivate
 * DESCRIPTION
 *  This function is to deactivate object push profile
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
BT_BOOL btmtk_opps_deactivate_server(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_LOGD(BT_OPP_OPPS_ACTIVATE);
	if( TRUE != g_opps_is_active){
		BT_OPP_LOGD("[EXT] func:opps_deactivate g_opps_is_active is not true. has been deactive\n");
		return TRUE;
	}
	g_opps_is_active = FALSE;


    BT_OPP_LOG_STATE();
	g_opps_ui_callback = NULL;

    switch (GOPS(ops_state))
    {
    case BT_OPS_IDLE:
    case BT_OPS_DEACTIVATING:
        /* nothing to do */
        return FALSE;
        break;
    case BT_OPS_SDP_REGISTERING:
    case BT_OPS_OBEX_REGISTERING:
        /* it must not happen, because CM guarantee no deactivation happen while activating */
        BT_OPP_ASSERT(0);
        break;
    case BT_OPS_ACTIVE:
        /* deregister SDP directly, because no connection yet */
        BT_OPS_STATE_TRANS(BT_OPS_DEACTIVATING);
        //bt_opps_sdpdb_deregister_req();
		bt_opps_obex_deregister_server_req();
        if( FALSE != ext_nbuffer_isInit(&GOPS(ops_mempool)) ){
            ext_nbuffer_deinit(&GOPS(ops_mempool));
        }
        btmtk_opps_return_buffer(GOPS(ops_push_buff) );
        GOPS(ops_push_buff_size) = 0;
        break;
    case BT_OPS_ACCEPT_WAIT:
    case BT_OPS_PREPARE_WAIT:
    case BT_OPS_CONNECTED:
        /* disconnect OBEX connection */
        BT_OPS_STATE_TRANS(BT_OPS_DEACTIVATING);
        /* Send OPP disconnect request to OBEX */
        bt_opps_disconnect_req(GOPS(ops_goep_conn_id));
        break;
    case BT_OPS_RECEIVING:
        /* need to abort ? because we already receive
            disconnect indication before CM invoke bt_opps_deactivate */
        BT_OPP_ASSERT(0);
        break;
    case BT_OPS_ABORTING:
        /* Impossible, because there is UI for OPPS aborting */
        BT_OPP_ASSERT(0);
        break;
    case BT_OPS_DISCONNECTING:
        /* wait disconnect response */
        break;
    default:
        BT_OPP_ASSERT(0);
        break;
    }

    return TRUE;
}

void btmtk_deinit_opp_server(void){
	BT_OPP_LOGD("[EXT] btmtk_deinit_opp_server");
	g_opps_is_active = FALSE;
    if( NULL != GOPS(ops_push_buff) ){
	    btmtk_opps_return_buffer(GOPS(ops_push_buff));
	    GOPS(ops_push_buff_size) = 0;
	    GOPS(ops_push_buff) = NULL;
    }
	GOEP_MEMSET( (U8 *) &g_bt_opp_cntx, 0, sizeof(g_bt_opp_cntx) );
}

/*****************************************************************************
 * FUNCTION
 *  bt_opps_disconnect_client
 * DESCRIPTION
 *  This function is to disconnect the OPS connection
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
BT_BOOL btmtk_opps_disconnect(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	U32 status;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //TODO BT_OPP_LOG1(BT_OPP_OPPS_DISC_CLIENT, g_conn_id);
    BT_OPP_LOG_STATE();

	status = GOPS(ops_state);
	BT_OPS_STATE_TRANS(BT_OPS_DISCONNECTING);

    GOPS(ops_disconnflag) = TRUE;

	if( BT_OPS_RECEIVING == status ){
		btmtk_opps_ui_notify( BT_OPPSUI_PUSHED, (void *) GOEP_USER_CANCEL);
	}else if( BT_OPS_SENDING == status ){
		btmtk_opps_ui_notify( BT_OPPSUI_PULLED, (void *) GOEP_USER_CANCEL);
	}

    /* Send OPP disconnect request to OBEX */
    bt_opps_disconnect_req(GOPS(ops_goep_conn_id));

    btmtk_opps_ui_notify( BT_OPPSUI_DISCONNECTING, NULL);

    return TRUE;
}

BT_BOOL btmtk_opps_authoriz_response( U8 u4RspCode )
{
    if(  BT_OPS_CONNECTING  == GOPS(ops_state)  ){
        /// response with RspCode
        if( GOEP_STATUS_SUCCESS == u4RspCode){
            BT_OPS_STATE_TRANS(BT_OPS_CONNECTED);
        }else{
            BT_OPS_STATE_TRANS(BT_OPS_ACTIVE);
        }
        bt_opps_authorize_rsp(GOPS(ops_goep_conn_id), u4RspCode);
        return TRUE;
    }
    return FALSE;
}

/**
* @brief Accept the pushing file when receiving a BT_OPPCUI_PUSHING EVENT
* @param filename The filename which is passed to btmtk_goep_open_wstream
* @return TRUE Ok to accept
* @see btmtk_goep_open_wstream
*/
BT_BOOL btmtk_opps_accept_push_file(U8 u4RspCode, const U8 *ucFilePath)
{
//    char msg[1028];
//    GOEP_MEMSET(msg, 0, sizeof(msg) );
//    ext_chset_ucs2_to_utf8_string( (U8 *)msg, sizeof(msg) - 1, (U8 *)ucFilePath );
//    BT_OPP_LOGI("[EXT] accept_push_file rspcode:%d filepath:(%s)", u4RspCode, msg);

    // receive data to file
    if( GOEP_STATUS_SUCCESS == u4RspCode ){
        bt_opps_push_file_routine(0,ucFilePath);
    }else{
        bt_opps_push_file_routine((U8)u4RspCode, NULL);
    }
    return TRUE;
}

/**
* @brief Accept the pulling file when receiving a BT_OPPCUI_PUSHING EVENT
* @param filename The filename which is passed to btmtk_goep_open_rstream
* @return TRUE Ok to accept
* @see btmtk_goep_open_rstream
*/
BT_BOOL btmtk_opps_accept_pull_file(U8 u4RspCode, const U8 *ucFilePath){
	BT_BOOL l_ret ;
    if( GOEP_STATUS_SUCCESS == u4RspCode ){
        l_ret = bt_opps_pull_file_routine(u4RspCode, ucFilePath);
    }else{
        l_ret = bt_opps_pull_file_routine(u4RspCode, NULL);
    }
    return l_ret;

}



/**
* @brief Offer the pulling object from a buffer when receiving a BT_OPPCUI_PULLING EVENT
* @param ucBuffer the pushing object. ucBuffer should be available until receive a BT_OPPCUI_PUSHED EVENT
* @return TRUE Ok to accept
*/
BT_BOOL btmtk_opps_accept_pull_memory(U8 u4RspCode,const U8 *ucName,const U8 *ucMime, U8 *ucBuffer, U32 u4Length){
    /// save to buffer content
    //    GOPS(ops_user_buffer) = ucBuffer;
    //    GOPS(ops_user_maxsize) = u4Length;
    //    GOPS(ops_user_offset) = 0;

    /// opps is in pulling state
    if( GOEP_STATUS_SUCCESS == u4RspCode ){
       	 bt_opps_pull_mem_routine(ucName, ucMime, ucBuffer, u4Length);
    }else{
		bt_opps_pull_rsp( GOPS(ops_goep_conn_id), u4RspCode, (goep_pkt_type_enum) 0, 0, 0, NULL);
    }
    return FALSE;
}

BT_BOOL btmtk_opps_accept_push_memory(U8 u4RspCode, U8 *ucBuffer, U32 u4Length){
    // TODO:  verify

    if( BT_OPS_ACCEPT_WAIT != GOPS(ops_state) ){
        BT_OPP_LOGD( "[EXT][wan] func:accept_push_mem state:%d\n" , GOPS(ops_state));
        return FALSE;
    }


    if( GOEP_STATUS_SUCCESS == u4RspCode ){

        if( NULL == ucBuffer || u4Length < GOPS(ops_pkt_len) ){
            BT_OPP_LOGD( "[EXT][wan] func:accept_push_mem ucBuffer:%d ucLen:%d pkt_len:%d\n" , ucBuffer, u4Length, GOPS(ops_pkt_len) );
            return FALSE;
        }

         bt_opps_push_mem_routine(GOEP_STATUS_SUCCESS, ucBuffer, u4Length);
    }else{
         bt_opps_push_mem_routine(u4RspCode, (U8 *) NULL, 0);
    }

    return FALSE;
}


/**
* MMI implementation
*/
void btmtk_opps_ui_notify(U32 u4OPPUIEvent, U8* ucdata){

	bt_opps_status_struct status;
    // printf( "[UI][EXT] Event:%d\n", u4OPPUIEvent);
    // dump status
    status.size = sizeof(bt_opps_status_struct);
    if( btmtk_opps_get_status(&status) ){

    	btmtk_opps_dump_status(&status);
    }else{

        GOEP_ASSERT(0);
    }

    if( NULL !=  g_opps_ui_callback){
        g_opps_ui_callback(u4OPPUIEvent, ucdata);
    }

#if 0
    switch( u4OPPUIEvent){
        case BT_OPPSUI_SDP_REGISTERED: // param: none
            // do nothing. register success or fail
            break;
        case BT_OPPSUI_AUTHORIZING: // param: dev
            btmtk_opps_authoriz_response(GOEP_STATUS_SUCCESS);
            break;
        case BT_OPPSUI_CONNECTED: // param:dev
            // do nothing
            break;
        case BT_OPPSUI_PUSH_START:  // param: filename
            // answer the first pushing. save to a specific path
            break;
       case BT_OPPSUI_PUSHING:  // param: percentage
             /// update UI
             break;
        case BT_OPPSUI_PUSHED: // param: rsp_code. Finish a push whether success or fail
            // rename the file or move the file
            break;
        case BT_OPPSUI_PULL_START:  // param: filename
            btmtk_opps_accept_pull_file(0, (const U8 *)L"d:\\temp\\opp");
            break;
        case BT_OPPSUI_PULLING:  // param: filename
            // update UI
            break;
        case BT_OPPSUI_PULLED: // param: rsp_code . Finish a pull whether success or fail
            // update record or something else
            break;
        case BT_OPPSUI_DISCONNECTING: // param: none
            // disconnect reqeust send
            break;
        case BT_OPPSUI_DISCONNECTED: //param: rsp_code
            // send disconnect
            break;
        case BT_OPPSUI_ABORTING: // param: rsp_code
            // receive a abort indication from client
            break;
        case BT_OPPSUI_SHUTDOWNING: // param: none
            // showdown the opps profile
            break;
        case BT_OPPSUI_ERROR: //param: ASCII error msg
            // unexpected error
            GOEP_ASSERT(0);
            break;
        default:
            GOEP_ASSERT(0);
            break;
    }
#endif
}


/**
* @brief callback for notifying UI
*/
BT_BOOL btmtk_opps_set_callback(BTMTK_OPP_CALLBACK pCallback){
    g_opps_ui_callback = pCallback;
    return TRUE;
}

/**
* @brief configure the OPP server to auto do something
* @see BT_OPPS_AUTO_BITMASK
*/
BT_BOOL btmtk_opps_configure_auto(U32 u4AutoMask){
    GOPS(ops_auto_accept) = u4AutoMask;
    return TRUE;
}

