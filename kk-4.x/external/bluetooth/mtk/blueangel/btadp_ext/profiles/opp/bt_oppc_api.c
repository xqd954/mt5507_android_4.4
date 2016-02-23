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
 * bt_oppc_api.c
 *
 * Project:
 * --------
 *
 *
 * Description:
 * ------------
 *   Export API of OPP Client
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

/* Basic header */
#include "bt_goep_porting.h"

#include "ext_osal.h"
#include "bt_opp_porting.h"

#include "bluetooth_gap_struct.h"
#include "bluetooth_struct.h"
#include "bluetooth_ftpc_struct.h"
#include "bt_os_api.h"

#include "ext_nbuffer.h"
#include "bt_opp_comm.h"
#include "bt_oppc_api.h"
#include "bt_oppc_struct.h"
#include "bt_oppc_hdl.h"
#include "bt_oppc_msg.h"
#include "bt_goep_fs.h"
#include "bt_ext_debug.h"
#include "ext_ucs2.h"


/* API in bt_oppc_hdl.h */
BT_BOOL bt_oppc_conn_server_routine(void);
BT_BOOL bt_opc_send_file_routine(void);
BT_BOOL bt_oppc_is_running();

/* local API */
void bt_dump_oppc_status(bt_oppc_status_struct *status);
BT_BOOL oppc_send_file_to_dev(const U8 *path, BT_BOOL is_sfn, void* callback, void *dst_dev);

/* OPPC UI conexte */
BTMTK_OPP_CALLBACK g_oppc_ui_callback = NULL;
U32 g_oppc_active = FALSE;

/**
* @brief Only invoke once. Init the OPP Client context
* Init functions
* This function is used to init the context of opp client
**/
BT_BOOL btmtk_oppc_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if( FALSE != g_oppc_active ){
         BT_OPP_LOGD("[EXT] btmtk_oppc_init  g_oppc_active is TRUE !ignore Init");
         return TRUE;
    }
    g_oppc_active = TRUE;
    GOPC(opc_state) = BT_OPC_IDLE;
	GOPC(opc_feature) = BT_OPP_READY_FEATURE;

    GOPC(h_opc_snd) = (FHANDLE)FHANDLE_INVALID_VALUE;

	GOPC(req_id) = OPP_DEFAULT_REQ_ID;    //For ALPS00270904, each obex-based profile use req_id for aborting while connecting.
    GOPC(opc_goep_conn_id) = OPP_INVALID_CONN_ID;
    GOPC(opc_goep_req_id) = OPP_INVALID_REQ_ID;
    GOPC(opc_cm_conn_id) = OPP_INVALID_CM_ID;

    GOPC(total_obj_len) = 0;
    GOPC(remain_put_len) = 0;
    GOPC(opc_mtu) = 0;
    GOPC(flag) = 0x00000000;
    GOPC(opc_disconnflag) = FALSE;
    GOPC(opc_snd_mem) = NULL;
    GOPC(opc_recv_mem) = NULL;
    GOPC(opc_is_pushmem) = FALSE;
    GOPC(opc_is_pullmem) = FALSE;

    /// static U8 mmi_bt_obex_profile_share_buffer[MMI_OBEX_PROFILE_SHARE_BUF_SIZE];
    GOPC(opc_push_buff_size) = btmtk_oppc_get_profile_buffer_size();
    GOPC(opc_push_buff) = btmtk_oppc_get_profile_buffer( GOPC(opc_push_buff_size) ); /// X: mmi_bt_obex_get_profile_shared_buffer

    GOEP_MEMSET( (U8 *)GOPC(opc_obj_path), 0, sizeof(GOPC(opc_obj_path)) );
    ext_nbuffer_init( &GOPC(opc_mempool), OPPC_MAX_OBEX_PACKET_LENGTH) ;
    GOPC(opc_use_nbuffer) = FALSE;

    OPPC_CS_INIT();
    return TRUE;
}

/**
* @brief deinit the OPP Client
*/
U8 btmtk_oppc_deinit(void){
    if( FALSE == g_oppc_active ){
        BT_OPP_LOGD("[EXT] btmtk_oppc_deinit  g_oppc_active is FALSE ! ignore ");
        return TRUE;
    }
    g_oppc_active = FALSE;
    if( NULL != GOPC(opc_push_buff) ){
	    btmtk_oppc_return_buffer(GOPC(opc_push_buff));
	    GOPC(opc_push_buff_size) = 0;
	    GOPC(opc_push_buff) = NULL;
    }
    g_oppc_ui_callback = NULL;
	switch( GOPC(opc_state) ){
		case BT_OPC_IDLE:
		case BT_OPC_DISCONNECTING:
            break;
		default:
            BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
            bt_oppc_disconnect_req(OPP_DEFAULT_CONN_ID);
			break;
	}

    return TRUE;
}

void btmtk_deinit_opp_client(void){
	BT_OPP_LOGD("[EXT] btmtk_deinit_opp_client - clean up active to FALSE");
	g_oppc_active = FALSE;
    if( NULL != GOPC(opc_push_buff) ){
	    btmtk_oppc_return_buffer(GOPC(opc_push_buff));
	    GOPC(opc_push_buff_size) = 0;
	    GOPC(opc_push_buff) = NULL;
    }
    g_oppc_ui_callback = NULL;
}

//BT_BOOL btmtk_oppc_push_feature(const U8 *ppshPath, void *dst_dev ){
//    /*----------------------------------------------------------------*/
//    /* Local Variables                                                */
//    /*----------------------------------------------------------------*/
//    btbm_bd_addr_t *bt_dev_p = (btbm_bd_addr_t *)dst_dev;
//    /*----------------------------------------------------------------*/
//    /* Code Body                                                      */
//    /*----------------------------------------------------------------*/
//
//    OPPC_CS_ENTER();
//    BT_OPP_ASSERT(path);
//    /* Check if the OPC channel is already used */
//    /* Add forward path compare to avoid re-entry case */
//    if ( FALSE == g_oppc_active || GOPC(opc_state) > BT_OPC_IDLE|| BT_OPP_READY_FEATURE != GOPC(opc_feature)) {
//        BT_OPP_LOGD("[OPPC] btmtk_oppc_push_feature wrong state:%d active:%d", GOPC(opc_state) , g_oppc_active);
//        OPPC_CS_LEAVE();
//        return FALSE;
//    }
//
//    GOPC(opc_obj_path)[0] = 0;
//    ext_ucs2ncpy( (U8 *)GOPC(opc_obj_path), ppshPath, sizeof(GOPC(opc_obj_path)) ) ;
//
//    if( NULL == btmtk_opp_util_copy_filepath( GOPC(opc_obj_path), (const U8 *) ppshPath, OPP_MAX_PATH_LENGTH)  ){
//        //TODO log
//        OPPC_CS_LEAVE();
//        return FALSE;
//    }
//
//    if( 0 != btmtk_goep_get_file_basename( (const U8 *)GOPC(opc_obj_path), (U8 *) GOPC(opc_obj_name), sizeof(GOPC(opc_obj_name)) ) ){
//        // filepath is empty
//		BT_OPP_LOGD("[oppc][err] func:send_file_to_dev cannot get basename \n");
//		GOPC(opc_obj_path)[0] = GOPC(opc_obj_path)[1] = 0;
//		return FALSE;
//    }
//
//    GOPC(opc_feature) = BT_OPP_PUSH_FEATURE;
//    GOPC(opc_dev_addr).lap = bt_dev_p->lap;
//    GOPC(opc_dev_addr).uap = bt_dev_p->uap;
//    GOPC(opc_dev_addr).nap = bt_dev_p->nap;
//
//    GOPC(opc_is_pushmem) = FALSE;
//    OPPC_CS_LEAVE();
//
//    return bt_oppc_conn_server_routine();
//}

/**
 @brief This is common function to abort sending object
* old name: bt_opp_snd_abort
**/
BT_BOOL btmtk_oppc_disconnect_feature(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    //BT_OPP_LOGD(bt_opp_OPP_SND_ABORT);
    BT_OPP_LOG_STATE();
    if( BT_OPP_READY_FEATURE == GOPC(opc_feature) ){
        switch (GOPC(opc_state))
        {
        case BT_OPC_CONNECTING:
        case BT_OPC_CONNECTED:
        case BT_OPC_SENDING:
        case BT_OPC_RECEIVING:
        case BT_OPC_ABORTING:
            BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
            bt_oppc_disconnect_req(OPP_DEFAULT_CONN_ID);
            break;
        case BT_OPC_DISCONNECTING:
            break;
        default:
            BT_OPP_ASSERT(0);
            return FALSE;
        }
    }else{
        switch (GOPC(opc_state))
        {
        case BT_OPC_CONNECTED:
		case BT_OPC_CONNECTING:
            /* we send tp_disconnect */
            BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
            bt_oppc_disconnect_req(GOPC(opc_goep_conn_id));
            break;
        case BT_OPC_SENDING:
        case BT_OPC_RECEIVING:
        case BT_OPC_ABORTING:
            /* we send tp_disconnect */
            BT_OPC_STATE_TRANS(BT_OPC_ABORTING);
            btmtk_oppc_ui_notify(BT_OPPCUI_ABORTING,0);
            BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
            bt_oppc_disconnect_req(GOPC(opc_goep_conn_id));
            break;

        /* May because sending finished, Disconnect already sent, do nothing */
        case BT_OPC_DISCONNECTING:
            break;

        default:
            BT_OPP_ASSERT(0);
            return FALSE;
        }
    }

    BT_OPP_SET_FLAG(BT_OPP_MASK_ABORT_PRESS);
    return TRUE;
}

/**
* @brief save vcard to local's default storage
* @param dst_dev remote device's bd address
* @return TRUE init the pull action success. FALSE current is busying.
*/
//BT_BOOL btmtk_oppc_pull_feature(const bt_oppc_object *pullObject, void *dst_dev){
//    /*----------------------------------------------------------------*/
//    /* Local Variables                                                */
//    /*----------------------------------------------------------------*/
//    btbm_bd_addr_t *bt_dev_p = (btbm_bd_addr_t *)dst_dev;
//
//
//    /*----------------------------------------------------------------*/
//    /* Code Body                                                      */
//    /*----------------------------------------------------------------*/
//
//    OPPC_CS_ENTER();
//    BT_OPP_ASSERT(path);
//    /* Check if the OPC channel is already used */
//    /* Add forward path compare to avoid re-entry case */
//    if ( FALSE == g_oppc_active || GOPC(opc_state) > BT_OPC_IDLE || BT_OPP_READY_FEATURE != GOPC(opc_feature)) {
//        BT_OPP_LOGD("[oppc][api][wan] func: pull_feature  state:%d active:%d\n", GOPC(opc_state), g_oppc_active);
//        OPPC_CS_LEAVE();
//        return FALSE;
//    }
//
//    if ( NULL == pullObject ) {
//        BT_OPP_LOGD("[oppc][api][err] func: pull_feature  state:%d  user data is null !\n", GOPC(opc_state));
//        OPPC_CS_LEAVE();
//        return FALSE;
//    }
//
//    GOPC(opc_feature) = BT_OPP_PULL_FEATURE;
//
//    GOPC(opc_dev_addr).lap = bt_dev_p->lap;
//    GOPC(opc_dev_addr).uap = bt_dev_p->uap;
//    GOPC(opc_dev_addr).nap = bt_dev_p->nap;
//
//    bt_oppc_reset_pull_info();
//
//    /// copy push object data
//    if( TRUE != pullObject->isFileMode ){
//        GOPC(opc_is_pullmem) = TRUE;
//        GOPC(opc_recv_mem) = pullObject->ucBuffer;
//        GOPC(opc_recv_mem_offset) = 0;
//        GOPC(opc_recv_mem_size) = pullObject->u4BufferLen;
//    }else{
//        GOPC(opc_is_pullmem) = FALSE;
//        ext_ucs2ncpy( (S8 *) GOPC(opc_pull_path), pullObject->ucFilePath, sizeof(GOPC(opc_pull_path)) );
//        GOPC(opc_recv_mem) = NULL;
//    }
//    if( NULL != pullObject->ucMime){
//        ext_strncpy( (S8 *) GOPC(opc_pull_mime), pullObject->ucMime, sizeof(GOPC(opc_pull_mime)));
//    }
//    if( NULL!= pullObject->ucObjName){
//        ext_ucs2ncpy( (S8 *)  GOPC(opc_pull_name), (const S8 * )pullObject->ucObjName, sizeof( GOPC(opc_pull_name )) );
//    }
//
//
//    OPPC_CS_LEAVE();
//
//    return bt_oppc_conn_server_routine();
//}


/**
* @brief Push a file and pull remte's vcard
* @param ppshPath unicode path of a push file. ex. L"c:\\autoexec.bat"
* @param pullPath the file path of pull object ex. L"d:\\temp\\1.vcf"
* @param dst_dev the remote device's bd address
*/
//BT_BOOL btmtk_oppc_exchange_feature(const U8 *ppshPath, const U8 *pullPath, const U8 *ucObjName, const U8 *ucMime, void *dst_dev){
//     btbm_bd_addr_t *bt_dev_p = (btbm_bd_addr_t *)dst_dev;
//
//    /// open the local file
//    OPPC_CS_ENTER();
//
//    /* Check if the OPC channel is already used */
//    /* Add forward path compare to avoid re-entry case */
//    if ( FALSE == g_oppc_active || GOPC(opc_state) > BT_OPC_IDLE || BT_OPP_READY_FEATURE != GOPC(opc_feature) ) {
//        BT_OPP_LOGD("[OPPC] fail to exchange state:%d active:%d", GOPC(opc_state), g_oppc_active);
//        OPPC_CS_LEAVE();
//        return FALSE;
//    }
//
//    GOPC(opc_feature) = BT_OPP_EXCHANGE_FEATURE;
//    BT_OPC_STATE_TRANS(BT_OPC_SENDING);
//    GOPC(opc_is_pushmem) = FALSE;
//    GOPC(opc_is_pullmem) = FALSE;
//
//    GOPC(opc_obj_path)[0] = 0;
//    GOEP_MEMSET( (U8 *)GOPC(opc_obj_name), 0, sizeof( GOPC(opc_obj_name) ) );
//	if( NULL != ucObjName ){
//		GOEP_MEMCPY( (U8 *)GOPC(opc_obj_name), ucObjName, OPP_MAX_OBJ_NAME_LENGTH);
//	}
//    GOEP_MEMSET( (U8 *)GOPC(opc_obj_mime), 0, sizeof( GOPC(opc_obj_mime) ) );
//	if( NULL != ucMime ){
//		GOEP_MEMCPY( (U8 *)GOPC(opc_obj_mime), ucMime, OPP_MAX_OBJ_MIME_LENGTH);
//	}
//
//    if( NULL ==  btmtk_opp_util_copy_filepath( GOPC(opc_obj_path), (const U8*) ppshPath, OPP_MAX_PATH_LENGTH)  ){
//		BT_OPP_LOGD("[OPPC] fail to copy filepath");
//		OPPC_CS_LEAVE();
//		return FALSE;
//    }
//    GOPC(opc_dev_addr).lap = bt_dev_p->lap;
//    GOPC(opc_dev_addr).uap = bt_dev_p->uap;
//    GOPC(opc_dev_addr).nap = bt_dev_p->nap;
//    OPPC_CS_LEAVE();
//
//    return bt_oppc_conn_server_routine();
//
//}




/**
* @brief Get current OPP Client status
* @param status Output. Current OPP Client status. Once OPP Client is inited, UI could get status by this function.
* @return FALSE  current OPP Client is not inited.
*/
BT_BOOL btmtk_oppc_get_status(bt_oppc_status_struct *status){
    U32 len;
    if( NULL == status ){
        return FALSE;
    }
    GOEP_MEMSET( (U8 *) status, 0, sizeof(bt_oppc_status_struct) );
    GOEP_MEMCPY( (U8 *)&status->remote_dev_addr, (const U8 *)&GOPC(opc_dev_addr), sizeof(GOPC(opc_dev_addr)) );
    GOEP_MEMCPY( (U8 *)status->dev_name, (const U8 *)GOPC(dev_name), sizeof( GOPC(dev_name) ) );
    GOEP_MEMCPY( (U8 *)status->obj_name, (const U8 *)GOPC(opc_obj_name), sizeof(GOPC(opc_obj_name)) );
    GOEP_MEMCPY( (U8 *)status->obj_mime, (const U8 *)GOPC(opc_obj_mime), sizeof(GOPC(opc_obj_mime)) );
    if( NULL != GOPC(opc_obj_path) ){
        len = 2 + ext_ucs2strlen( (const S8 *)GOPC(opc_obj_path)) * 2;
        GOEP_MEMCPY( (U8 *)status->obj_path, (const U8 *) GOPC(opc_obj_path), min(sizeof(status->obj_path),len) );
    }


    status->state = GOPC(opc_state);
    status->feature = GOPC(opc_feature);

    status->totalsize = GOPC(total_obj_len) ;
    status->remainsize = GOPC(remain_put_len);
    return TRUE;
}

/**
* @brief Register callback for notifying UI
* @param pCallback the UI callback. NULL means deregister
*/
BT_BOOL btmtk_oppc_set_callback(BTMTK_OPP_CALLBACK pCallback){
      g_oppc_ui_callback = pCallback;
    return TRUE;
}

/**
* @brief Aborting the current pushing or pulling
*/
BT_BOOL btmtk_oppc_abort(void){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BT_BOOL l_bRet;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OPPC_CS_ENTER();
    /* Check if the OPC channel is already used */
    if( BT_OPC_RECEIVING != GOPC(opc_state) && BT_OPC_SENDING != GOPC(opc_state)){
        BT_OPP_LOGD("[EXT} btmtk_oppc_abort abort in wrong state opc_state:%d", GOPC(opc_state) );
        l_bRet = FALSE;
        goto OPPC_ABORT_EXIT;
    }

    l_bRet = TRUE;

    /// set up the abort flag
    GOPC(isAbort) = TRUE;

OPPC_ABORT_EXIT:
    OPPC_CS_LEAVE();
    return l_bRet;
}

BT_BOOL btmtk_oppc_connect(void *dst_dev){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    btbm_bd_addr_t *bt_dev_p = (btbm_bd_addr_t *)dst_dev;
    BT_BOOL l_bRet;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    OPPC_CS_ENTER();
    BT_OPP_ASSERT(path);
    OS_Report("[BT30] btmtk_oppc_connect");    

    /* Check if the OPC channel is already used */
    /* Add forward path compare to avoid re-entry case */
    if ( GOPC(opc_state) > BT_OPC_IDLE|| BT_OPP_READY_FEATURE != GOPC(opc_feature)) {
        OPPC_CS_LEAVE();
        return FALSE;
    }

    GOPC(opc_dev_addr).lap = bt_dev_p->lap;
    GOPC(opc_dev_addr).uap = bt_dev_p->uap;
    GOPC(opc_dev_addr).nap = bt_dev_p->nap;
    l_bRet = bt_oppc_conn_server_routine();

    OPPC_CS_LEAVE();
    return l_bRet;
}

BT_BOOL btmtk_oppc_push_object( const bt_oppc_object *pushObject ){

    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BT_BOOL l_ret = FALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    OPPC_CS_ENTER();
    BT_OPP_ASSERT(pushObject);

    /* Check if the OPC channel is already used */
    /* Add forward path compare to avoid re-entry case */
    if ( GOPC(opc_state) != BT_OPC_CONNECTED || BT_OPP_READY_FEATURE != GOPC(opc_feature)) {
        // TODO: Error log
        goto PUSH_OBJECT_EXIT;
    }
    // check
    if ( NULL == pushObject )
    {
        BT_OPP_LOGI("[EXT][btmtk_oppc_push_object] - push object is null");
        goto PUSH_OBJECT_EXIT;
    }

    // don't turn on 'feature' or it will be disconnected when push finished
    GOPC(opc_feature) = BT_OPP_READY_FEATURE;

    /// copy push object data
    bt_oppc_reset_push_info();

    if( TRUE != pushObject->isFileMode ){
        GOPC(opc_is_pushmem) = TRUE;
        GOPC(opc_snd_mem) = pushObject->ucBuffer;
        GOPC(opc_snd_mem_offset) = 0;
        GOPC(opc_snd_mem_size) = pushObject->u4BufferLen; //NOTE! the size is the avaiable data of mem size
    }else{
        GOPC(opc_is_pushmem) = FALSE;
        ext_ucs2ncpy( (S8 *)GOPC(opc_obj_path), (S8 *)pushObject->ucFilePath, sizeof(GOPC(opc_obj_path)) );
        GOPC(opc_snd_mem) = NULL;
    }
    if( NULL != pushObject->ucMime ){
        ext_strncpy( (S8 *)GOPC(opc_obj_mime), (S8 *)pushObject->ucMime, sizeof(GOPC(opc_obj_mime)));
    }
    if( NULL != pushObject->ucObjName){
        ext_ucs2ncpy( (S8 *)  GOPC(opc_obj_name), (const S8 * )pushObject->ucObjName, sizeof( GOPC(opc_obj_name ))/2 );
    }

    // start push file
    if( FALSE == bt_opc_send_file_routine() ){
        goto PUSH_OBJECT_EXIT;
    }
    l_ret = TRUE;

PUSH_OBJECT_EXIT:
    OPPC_CS_LEAVE();
    return l_ret;
}

BT_BOOL btmtk_oppc_pull_object(const S8 *u8FilePath, BT_BOOL is_sfn){
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BOOL l_ret = FALSE;


    OPPC_CS_ENTER();
    BT_OPP_ASSERT(path);

    /* Check if the OPC channel is already used */
    /* Add forward path compare to avoid re-entry case */
    if ( GOPC(opc_state) != BT_OPC_CONNECTED|| BT_OPP_READY_FEATURE != GOPC(opc_feature)) {
        // TODO: Error log
        goto PULL_OBJECT_EXIT;
    }

    // check file is exist or not. (exist = fail)
    if( FALSE == is_sfn && TRUE == btmtk_goep_is_file_exist( (const U16 *) u8FilePath) ){
        // TODO: Error log
        goto PULL_OBJECT_EXIT;
    }

    // copy path to GOPC(opc_obj_path)
    if( NULL == btmtk_opp_util_copy_filepath(GOPC(opc_obj_path), u8FilePath, OPP_MAX_PATH_LENGTH) ){
        // TODO: Error log
        goto PULL_OBJECT_EXIT;
    }

    bt_oppc_send_pull_vcard_routine(TRUE);
    l_ret = TRUE;

PULL_OBJECT_EXIT:
    OPPC_CS_LEAVE();
    return l_ret;
}

BT_BOOL btmtk_oppc_disconnect(void){
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    OPPC_CS_ENTER();
    BT_OPP_ASSERT(path);

    /* Check if the OPC channel is already used */
    /* Add forward path compare to avoid re-entry case */
    if ( GOPC(opc_state)  <= BT_OPC_IDLE|| BT_OPP_READY_FEATURE != GOPC(opc_feature)) {
        // TODO: Error log
        OPPC_CS_LEAVE();
        return FALSE;
    }
    btmtk_oppc_disconnect_feature();
    OPPC_CS_LEAVE();
    return TRUE;
}

/**
* @brief Check the current state is pushing or pulling
*/
BT_BOOL bt_oppc_is_running(){
    U32 status;
    status = GOPC(opc_state);
    if( BT_OPC_SENDING == status || BT_OPC_RECEIVING == status){
        return TRUE;
    }
    return FALSE;
}

BT_BOOL btmtk_oppc_push_ex_feature(const bt_oppc_object *pushObject, void *dst_dev){
   /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    btbm_bd_addr_t *bt_dev_p = (btbm_bd_addr_t *)dst_dev;
	//U32 len;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    OPPC_CS_ENTER();
    BT_OPP_ASSERT(pushObject);
    /* Check if the OPC channel is already used */
    /* Add forward path compare to avoid re-entry case */
    if ( FALSE == g_oppc_active|| GOPC(opc_state) > BT_OPC_IDLE|| BT_OPP_READY_FEATURE != GOPC(opc_feature)) {
        BT_OPP_LOGI("[EXT][btmtk_oppc_push_ex_feature] - invalid state:[%d]", GOPC(opc_state));
        OPPC_CS_LEAVE();
        return FALSE;
    }

    if ( NULL == pushObject )
    {
        BT_OPP_LOGI("[EXT][btmtk_oppc_push_ex_feature] - push object is null");
        OPPC_CS_LEAVE();
        return FALSE;
    }

    GOPC(opc_feature) = BT_OPP_PUSH_FEATURE;

    GOPC(opc_dev_addr).lap = bt_dev_p->lap;
    GOPC(opc_dev_addr).uap = bt_dev_p->uap;
    GOPC(opc_dev_addr).nap = bt_dev_p->nap;

    /// copy push object data
    bt_oppc_reset_push_info();

    if( TRUE != pushObject->isFileMode ){
        GOPC(opc_is_pushmem) = TRUE;
        GOPC(opc_snd_mem) = pushObject->ucBuffer;
        GOPC(opc_snd_mem_offset) = 0;
        GOPC(opc_snd_mem_size) = pushObject->u4BufferLen; //NOTE! the size is the avaiable data of mem size
    }else{
        GOPC(opc_is_pushmem) = FALSE;
        ext_ucs2ncpy( (S8 *)GOPC(opc_obj_path), (S8 *)pushObject->ucFilePath, sizeof(GOPC(opc_obj_path)) );
        GOPC(opc_snd_mem) = NULL;
    }
    if( NULL != pushObject->ucMime ){
        ext_strncpy( (S8 *)GOPC(opc_obj_mime), (S8 *)pushObject->ucMime, sizeof(GOPC(opc_obj_mime)));
    }
    if( NULL != pushObject->ucObjName){
        ext_ucs2ncpy( (S8 *)GOPC(opc_obj_name), (const S8 *)pushObject->ucObjName, sizeof( GOPC(opc_obj_name )) );
    }

    OPPC_CS_LEAVE();

    return bt_oppc_conn_server_routine();
}

BT_BOOL btmtk_oppc_pull_ex_feature(const bt_oppc_object *pullObject, void *dst_dev)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    btbm_bd_addr_t *bt_dev_p = (btbm_bd_addr_t *)dst_dev;
	//U32 len;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    OPPC_CS_ENTER();
    BT_OPP_ASSERT(pullObject);
    /* Check if the OPC channel is already used */
    /* Add forward path compare to avoid re-entry case */
    if ( FALSE == g_oppc_active|| GOPC(opc_state) > BT_OPC_IDLE|| BT_OPP_READY_FEATURE != GOPC(opc_feature)) {
        BT_OPP_LOGD("[EXT}[api][wan] func: pull_feature  state:%d \n", GOPC(opc_state));
        OPPC_CS_LEAVE();
        return FALSE;
    }

    if ( NULL == pullObject ) {
        BT_OPP_LOGD("[EXT}[api][err] func: pull_feature  state:%d  user data is null !\n", GOPC(opc_state));
        OPPC_CS_LEAVE();
        return FALSE;
    }

    GOPC(opc_feature) = BT_OPP_PULL_FEATURE;

    GOPC(opc_dev_addr).lap = bt_dev_p->lap;
    GOPC(opc_dev_addr).uap = bt_dev_p->uap;
    GOPC(opc_dev_addr).nap = bt_dev_p->nap;

    /// copy push object data
    bt_oppc_reset_pull_info();

    if( TRUE != pullObject->isFileMode ){
        GOPC(opc_is_pullmem) = TRUE;
        GOPC(opc_recv_mem) = pullObject->ucBuffer;
        GOPC(opc_recv_mem_offset) = 0;
        GOPC(opc_recv_mem_size) = pullObject->u4BufferSize; //NOTE! the size is the mem size
    }else{
        GOPC(opc_is_pullmem) = FALSE;

        GOPC(opc_recv_mem) = NULL;
    }
    GOPC(opc_pull_acceptsize) = pullObject->u4MaxAcceptSize; // 0 is no-limit
    ext_ucs2ncpy( (S8 *)GOPC(opc_pull_path), (S8 *)pullObject->ucFilePath, sizeof(GOPC(opc_pull_path)) );

    if( NULL != pullObject->ucMime){
        ext_strncpy( (S8 *)GOPC(opc_pull_mime), (S8 *)pullObject->ucMime, sizeof(GOPC(opc_pull_mime)));
    }
    if( NULL!= pullObject->ucObjName){
        ext_ucs2ncpy( (S8 *)GOPC(opc_pull_name), (const S8 *)pullObject->ucObjName, sizeof(GOPC(opc_pull_name )) );
    }


    OPPC_CS_LEAVE();

    return bt_oppc_conn_server_routine();
}

/**
* @brief Exchange by memory or file
* @
*/
BT_BOOL btmtk_oppc_exchange_ex_feature(const bt_oppc_object *pushObject, const bt_oppc_object *pullObject, void *dst_dev)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    btbm_bd_addr_t *bt_dev_p = (btbm_bd_addr_t *)dst_dev;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    OPPC_CS_ENTER();
    BT_OPP_ASSERT(path);
    /* Check if the OPC channel is already used */
    /* Add forward path compare to avoid re-entry case */
    if ( FALSE == g_oppc_active|| GOPC(opc_state) > BT_OPC_IDLE|| BT_OPP_READY_FEATURE != GOPC(opc_feature)) {
        BT_OPP_LOGD("[EXT}[api][wan] func: exchange_ex_feature  state:%d \n", GOPC(opc_state));
        OPPC_CS_LEAVE();
        return FALSE;
    }

    if ( NULL == pushObject || NULL == pullObject ) {
        BT_OPP_LOGD("[EXT}[api][err] func: exchange_ex_feature  state:%d  user data is null !\n", GOPC(opc_state));
        OPPC_CS_LEAVE();
        return FALSE;
    }

    GOPC(opc_feature) = BT_OPP_EXCHANGE_FEATURE;

    GOPC(opc_dev_addr).lap = bt_dev_p->lap;
    GOPC(opc_dev_addr).uap = bt_dev_p->uap;
    GOPC(opc_dev_addr).nap = bt_dev_p->nap;

    /// copy push object data
    bt_oppc_reset_push_info();
    bt_oppc_reset_pull_info();

    if( TRUE != pushObject->isFileMode ){
        GOPC(opc_is_pushmem) = TRUE;
        GOPC(opc_snd_mem) = pushObject->ucBuffer;
        GOPC(opc_snd_mem_offset) = 0;
        GOPC(opc_snd_mem_size) = pushObject->u4BufferLen; //NOTE! the size is the avaiable data of mem size
    }else{
        GOPC(opc_is_pushmem) = FALSE;
        ext_ucs2ncpy( (S8 *)GOPC(opc_obj_path), (S8 *)pushObject->ucFilePath, sizeof(GOPC(opc_obj_path)) );
        GOPC(opc_snd_mem) = NULL;
    }
    if( NULL != pushObject->ucMime ){
        ext_strncpy( (S8 *)GOPC(opc_obj_mime), (S8 *)pushObject->ucMime, sizeof(GOPC(opc_obj_mime)));
    }
    if( NULL != pushObject->ucObjName){
        ext_ucs2ncpy( (S8 *)GOPC(opc_obj_name), (const S8 *)pushObject->ucObjName, sizeof( GOPC(opc_obj_name )) );
    }


    if( TRUE != pullObject->isFileMode ){
        GOPC(opc_is_pullmem) = TRUE;
        GOPC(opc_recv_mem) = pullObject->ucBuffer;
        GOPC(opc_recv_mem_offset) = 0;
        GOPC(opc_recv_mem_size) = pullObject->u4BufferSize; //NOTE! the size is the mem size
    }else{
        GOPC(opc_is_pullmem) = FALSE;
        ext_ucs2ncpy( (S8 *)GOPC(opc_pull_path), (S8 *)pullObject->ucFilePath, sizeof(GOPC(opc_pull_path)) );
        GOPC(opc_recv_mem) = NULL;
    }
    GOPC(opc_pull_acceptsize) = pullObject->u4MaxAcceptSize;
    if( NULL != pullObject->ucMime){
        ext_strncpy( (S8 *)GOPC(opc_pull_mime), (S8 *)pullObject->ucMime, sizeof(GOPC(opc_pull_mime)));
    }
    if( NULL!= pullObject->ucObjName){
        ext_ucs2ncpy( (S8 *)GOPC(opc_pull_name), (const S8 *)pullObject->ucObjName, sizeof( GOPC(opc_pull_name )) );
    }

    OPPC_CS_LEAVE();

    return bt_oppc_conn_server_routine();
}


/**
* @brief Notify the OPP Client UI
*/
void btmtk_oppc_ui_notify(U32 u4OPPUIEvent, U8* ucdata){
#ifdef GOEP_TEST
    bt_oppc_status_struct status;
    btmtk_oppc_get_status( &status );
    bt_dump_oppc_status(&status);
#endif
    if( NULL !=  g_oppc_ui_callback){
        g_oppc_ui_callback(u4OPPUIEvent, ucdata);
    }
}

/**
* @brief Print out the current OPP Client State
*/
void bt_dump_oppc_status(bt_oppc_status_struct *status){
#ifdef GOEP_TEST
    if( NULL == status ){
        return;
    }

    printf("state: %d (%s)\n", status->state, getOPPCState(status->state) );
    printf("feature: %d (%s)\n", status->feature, getOPPFeature(status->feature) );
    printf("total: %d remain: %d\n", status->totalsize, status->remainsize);
#endif
}


BT_BOOL btmtk_oppc_inject_test(S8 *utfString){
	/// compose a msg and pass the utf8string to it

	bt_test_cmd_req_struct *test_msg;

	test_msg = ( bt_test_cmd_req_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(bt_test_cmd_req_struct),TD_UL);

	GOEP_MEMSET( test_msg->test_cmd_content, 0, sizeof(test_msg->test_cmd_content) );
	GOEP_MEMCPY((S8 *)test_msg->test_cmd_content, utfString, min(ext_strlen(utfString), sizeof(test_msg->test_cmd_content)-1) );

	BTCMD_SendMessage( MSG_ID_BT_OPPC_DISCONNECT_CNF, MOD_MMI, test_msg , sizeof(bt_test_cmd_req_struct));
	return TRUE;
}
