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
 * bt_oppc_hdl.c
 *
 * Project:
 * --------
 *
 *
 * Description:
 * ------------
 *   This file is for MTK bluetooth MMI OPP client/server
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
 * 05 24 2013 dexiang.jiang
 * [ALPS00711863] [G610-T00]??????MATE?????????100%?????????
 * Do not notity MMI pushing event while object size less than mtu.
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

#include "bt_goep_fs.h"
#include "bt_opp_porting.h"
#include "bt_opp_comm.h"
#include "bt_oppc_api.h"
#include "bt_oppc_hdl.h"
#include "bt_oppc_struct.h"
#include "bt_oppc_msg.h"
#include "ext_ucs2.h"
#include "bt_ext_debug.h"

/*****************************************************************************
* Internal Functions
*****************************************************************************/
/// @deprecated
//extern BOOL static_oppc_fs_tester(U8 *ptr, U32 *retval);

/// Private method of OPPC context
static void bt_oppc_flush_one_nbuffer();
static void bt_oppc_flush_all_nbuffer();
static void bt_oppc_connection_terminated(U8 cause);

/// Internal Utilities
static FHANDLE bt_oppc_util_gen_temp_obj(const U16 *obj_name, U16 *final_filepath, U16 maxSize);

void BTOPP_Report(U32 id, U8 *prefix, U8 *param1){
#if defined(BTMTK_ON_WISE)
	char msg[128];
	btmtk_os_memset(msg, 0, 128);
	ext_chset_ucs2_to_utf8_string( (U8 *) msg, sizeof(msg) - 1, (const U8 *)param1);
	BT_OPP_LOGD( prefix, msg );
#endif
}

/*****************************************************************************
* External Functions
*****************************************************************************/
void btmtk_oppc_handle_message(void *msg){

	void* data;
	U32 msg_id;

	ilm_struct *p = (ilm_struct *)msg;
	msg_id = p->msg_id;
	data = &p->ilm_data;

	BT_OPP_LOG_STATE();

	//BT_OPP_LOGD("[EXT] btmtk_oppc_handle_message: msg_id[%s]", bt_goep_get_id_name(msg_id));

    switch(msg_id)
    {
        /* OPP client specific event */
        case MSG_ID_BT_OPPC_CONNECT_CNF:
            btmtk_oppc_connect_cnf_handler(data);
            break;
        case MSG_ID_BT_OPPC_PULL_CNF:
            btmtk_oppc_pull_cnf_handler(data);
            break;
        case MSG_ID_BT_OPPC_PUSH_CNF:
            btmtk_oppc_push_cnf_handler(data);
            break;
        case MSG_ID_BT_OPPC_ABORT_CNF:
            btmtk_oppc_abort_cnf_handler(data);
            break;
        case MSG_ID_BT_OPPC_OPP_SUPPORTED_FORMATS_IND:
            btmtk_oppc_supported_formats_ind_handler(data);
            break;
        case MSG_ID_BT_OPPC_DISCONNECT_CNF:
            btmtk_oppc_inject_msg_handler(data);
            break;
        case MSG_ID_BT_OPPC_AUTH_IND:
            btmtk_oppc_auth_ind_handler(data);
            break;
        case MSG_ID_BT_OPPC_AUTH_RSP:
            btmtk_oppc_auth_rsp_handler(data);
            break;
        case MSG_ID_BT_OPPC_TPDISCONNECT_IND:
            btmtk_oppc_disconnect_ind_handler(data);
            break;
        case MSG_ID_BT_OPPC_CLIENT_INTERNAL_RW:
            btmtk_oppc_internal_rw_handler(data);
            break;
        default:
            BT_OPP_LOGD("[EXT] btmtk_oppc_handle_message() err: [%d][%s]", msg_id, bt_goep_get_id_name(msg_id));
            GOEP_ASSERT(0);
            break;
    }
}


/**
* @brief MAUI leacy indication for handing remote's support format
*  This function is event handler for OBEX server supported formats indication ( MSG_ID_BT_GOEP_OPP_SUPPORTED_FORMATS_IND)
**/
void btmtk_oppc_supported_formats_ind_handler(void *msg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_opp_supported_formats_ind_struct *ind = (goep_opp_supported_formats_ind_struct*)msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_RESET_FLAG(BT_OPP_MASK_SDP_FORMAT);
    if (ind->server_supported_formats !=0xFF)
    {
        BT_OPP_SET_FLAG(BT_OPP_MASK_SDP_FORMAT);
    }
    else
    {
        return;
    }
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_oppc_connect_cnf_handler
 * DESCRIPTION
 *  This function is event handler for OBEX connect response (MSG_ID_BT_GOEP_CONNECT_RSP)
 * PARAMETERS
 *  msg         [?]
 *  a(?)        [IN/OUT]        First variable, used as returns
 *  b(?)        [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_oppc_connect_cnf_handler(void *msg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_connect_cnf_struct *rsp = (goep_connect_cnf_struct*)msg;
    BT_BOOL ret = TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    // check request id if match
    if (rsp->req_id != GOPC(opc_goep_req_id))
    {
    	BT_OPP_LOGI( "[EXT][HDL][btmtk_oppc_connect_cnf_handler]: invalid req_id - msg[%u], context[%u]", rsp->req_id, GOPC(opc_goep_req_id));
        return;
    }

    BT_OPP_LOGD( "[EXT][btmtk_oppc_connect_cnf_handler]: rsp[%d],success[%d], state[%d]", rsp->rsp_code, GOEP_STATUS_SUCCESS, GOPC(opc_state) );

    if (rsp->rsp_code == GOEP_STATUS_SUCCESS)
    {
        /* Connection is established successfully  */
        switch (GOPC(opc_state))
        {
            case BT_OPC_CONNECTING:
            GOPC(opc_goep_conn_id) = rsp->goep_conn_id;
            GOPC(opc_cm_conn_id) = rsp->cm_conn_id;
            BT_OPC_STATE_TRANS(BT_OPC_CONNECTED);
            BT_OPP_SET_FLAG(BT_OPP_MASK_CM_NOTIFY);

            /* The connection is establish for sending object */

            /* Store the max acceptable obex packet size */
            if (rsp->peer_mru != 0)
            {
                GOPC(opc_mtu) =
                    (rsp->peer_mru < OPPC_MAX_OBEX_PACKET_LENGTH ? rsp->peer_mru : OPPC_MAX_OBEX_PACKET_LENGTH);
            }
            else
            {
                GOPC(opc_mtu) = OPPC_MAX_OBEX_PACKET_LENGTH;
            }

            BT_OPP_LOGD("[EXT][btmtk_oppc_connect_cnf_handler]: peer_mru[%d], opc_mtu[%d]", rsp->peer_mru, GOPC(opc_mtu));
            btmtk_oppc_ui_notify(BT_OPPCUI_CONNECTED, 0);

            if (BT_OPP_FLAG_IS_ON(BT_OPP_MASK_MULTI_SEND))
            {
                // legacy wait for next instruction
            }
            else
            {
            	BT_OPP_LOGD("[EXT][btmtk_oppc_connect_cnf_handler]: try to process feature: feature[%d]", GOPC(opc_feature));
                switch( GOPC(opc_feature) ){
                    case BT_OPP_PUSH_FEATURE:
                    case BT_OPP_EXCHANGE_FEATURE:
                        if( FALSE == GOPC(opc_is_pushmem) ){
                            bt_opc_send_file_routine();
                        }else{
                            bt_opc_send_mem_routine();
                        }
                    break;
                    case BT_OPP_PULL_FEATURE:
                         if( FALSE == GOPC(opc_is_pullmem) ){
                             bt_oppc_send_pull_vcard_routine(TRUE); // send the first pull packet
                         }else{
                            bt_oppc_send_pull_mem_routine(TRUE);
                        }
                    break;
					default:
						break;
                }
            }
            break;

        case BT_OPC_ABORTING:
            /* store the connection id for disconnect response connection id check */
            GOPC(opc_goep_conn_id) = rsp->goep_conn_id;
            GOPC(opc_cm_conn_id) = rsp->cm_conn_id;
            /* disconnect the established connecttion */
            BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
            bt_oppc_disconnect_req(rsp->goep_conn_id);
            break;

        case BT_OPC_DISCONNECTING:
            /* race condition, just ignore */ ///< QQQ todo resolving this
            GOPC(opc_goep_conn_id) = rsp->goep_conn_id;
            break;
        default:
            BT_OPP_LOGI("[EXT][HDL][btmtk_oppc_connect_cnf_handler]: invalid state[%u]", GOPC(opc_state));
            BT_OPC_LOG_STATE_ERR();
            BT_OPP_ASSERT(0);
            break;
        }
    }
    else
    {
        /* Connection establish fail */
        BT_OPP_LOGI("[EXT][HDL][btmtk_oppc_connect_cnf_handler]: connect fail - response_code[%u]", rsp->rsp_code);
        bt_oppc_connection_terminated(rsp->rsp_code);
        BT_OPC_STATE_TRANS(BT_OPC_IDLE);
    }
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_oppc_disconnect_ind_handler
 * DESCRIPTION
 *  This function is event handler for OBEX client disconnect indication
 * PARAMETERS
 *  msg         [?]
 *  a(?)        [IN/OUT]        First variable, used as returns
 *  b(?)        [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_oppc_disconnect_ind_handler(void *msg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 cause;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_LOGD(bt_opp_OPPC_DISC_CONN_IND);

    if (BT_OPP_FLAG_IS_ON(BT_OPP_MASK_MULTI_SEND))
    {
        cause = BTSEND_RET_DONE;
    }
    else if (BT_OPP_FLAG_IS_ON(BT_OPP_MASK_BAD_FILE))
    {
        cause = BTSEND_RET_DISCONNECT;
        //BT_OPP_RESET_FLAG(BT_OPP_MASK_BAD_FILE);
    }
    else if (BT_OPP_FLAG_IS_ON(BT_OPP_MASK_REJECT))
    {
        cause = BTSEND_RET_REJECT;
        //BT_OPP_RESET_FLAG(BT_OPP_MASK_REJECT);
    }
    else
    {
        switch (GOPC(opc_state))
        {
        case BT_OPC_ABORTING:
            //btmtk_os_stop_timer(BT_OPP_ABORT_REQ_TIMER);
        case BT_OPC_DISCONNECTING:
            cause = BTSEND_RET_SUCCESS;
            break;
        case BT_OPC_SENDING:
            cause = BTSEND_RET_DISCONNECT;
            break;
        case BT_OPC_IDLE:
            /* To avoid BT_RESET case, just discard it */
            return;
        default:
            cause = BTSEND_RET_DISCONNECT;
            break;
        }
    }
    /*because when send obj, update progress,
      if the sending screen is in history,when disc,should stop the timer */
    //btmtk_os_stop_timer(FMGR_PROG_UPDATE_TIMER);

    bt_oppc_connection_terminated(cause);
}


/*****************************************************************************
 * FUNCTION
 *  bt_oppc_push_rsp_handler
 * DESCRIPTION
 *  This function is event handler for OBEX push response
 *  (MSG_ID_BT_GOEP_PUSH_RSP)
 * PARAMETERS
 *  msg         [?]
 *  a(?)        [IN/OUT]        First variable, used as returns
 *  b(?)        [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_oppc_push_cnf_handler(void *data)
{
     if( FALSE ==  GOPC(opc_is_pushmem) ){
          bt_opc_send_file_continue_routine( (goep_push_cnf_struct *) data );
     }else{
          bt_opc_send_mem_continue_routine( (goep_push_cnf_struct *) data );
     }
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_oppc_abort_cnf_handler
 * DESCRIPTION
 *  This function is event handler for OBEX abort response (MSG_ID_BT_GOEP_ABORT_RSP)
 * PARAMETERS
 *  msg     [?]
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_oppc_abort_cnf_handler(void *msg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_abort_rsp_struct *goep_abort_rsp = (goep_abort_rsp_struct*) msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (goep_abort_rsp->goep_conn_id != GOPC(opc_goep_conn_id))
    {
        /* do nothing because disconnect indication may already be received */
		BT_OPP_LOGD("[EXT][WRN] abort_rsp_handler rsp->goep_conn_id:%d conn_id:%d", goep_abort_rsp->goep_conn_id, GOPC(opc_goep_conn_id));
    }

    if (GOPC(opc_state) == BT_OPC_ABORTING)
    {
                BT_OPC_STATE_TRANS(BT_OPC_CONNECTED);

                if( btmtk_goep_is_valid_handle(GOPC(h_opc_snd)) )
                {
                    btmtk_goep_close_rstream(GOPC(h_opc_snd));
                }
                GOPC(h_opc_snd) = (FHANDLE)FHANDLE_INVALID_VALUE;

                if( btmtk_goep_is_valid_handle( GOPC(h_opc_recv) ) )
                {
                    btmtk_goep_close_wstream(GOPC(h_opc_recv));
                }
                GOPC(h_opc_recv) = (FHANDLE)FHANDLE_INVALID_VALUE;

        if( BT_OPP_READY_FEATURE != GOPC(opc_feature)){
            BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
			/* abort rsp may pass the 0 conn_id. Use local conn id*/
            bt_oppc_disconnect_req( GOPC(opc_goep_conn_id) );
        }else{
			btmtk_oppc_ui_notify( BT_OPPCUI_CONNECTED , (U8 *)((U32)goep_abort_rsp->rsp_code) );
        }
    }
    else if (GOPC(opc_state) == BT_OPC_DISCONNECTING)
    {
        /* already receive abort indication, it is race condition, do nothing */
        BT_OPP_LOGD("[EXT] ignore the abor_cnf during disconnecting");
    }
    else
    {
        /* do nothing because disconnect indication may already be received */
        BT_OPP_LOGD("[EXT][WRN] no handle state:%d", GOPC(opc_state) );
    }
}


/**
* @brief OPPC PULL rsp handler
*/
void btmtk_oppc_pull_cnf_handler(void * msg){
    /// handle the pull file
    goep_pull_cnf_struct *pCnf = (goep_pull_cnf_struct *)msg;
    if( NULL == pCnf ){
        BT_OPP_LOGD( "[EXT] fail to handle pull rsp");
        return;
    }
	if( GOEP_STATUS_SUCCESS == pCnf->rsp_code && ( GOEP_FIRST_PKT == pCnf->pkt_type || GOEP_SINGLE_PKT == pCnf->pkt_type ) ){
		// check the pull object size
		if( FALSE != GOPC(opc_is_pullmem) ){
			if( pCnf->total_obj_len > GOPC(opc_recv_mem_size)){
				/// change to FILE mode
				BT_OPP_LOGD("[EXT] mem to file mode. the object size is large then mem total:%d mem_size:%d", pCnf->total_obj_len, GOPC(opc_recv_mem_size));
				GOPC(opc_is_pullmem) = FALSE;
			}
			if( 0 != GOPC(opc_pull_acceptsize) && pCnf->total_obj_len > GOPC(opc_pull_acceptsize) ){
				/// exceed the max acceptable size
				BT_OPP_LOGD("[EXT] opc_pull_acceptsize:%d total_boj_len:%d", GOPC(opc_pull_acceptsize), pCnf->total_obj_len);
				if( GOEP_FIRST_PKT == pCnf->pkt_type ){
                    /// send abort req
					GOPC(opc_state) = BT_OPC_ABORTING;
                    bt_oppc_abort_req(GOPC(opc_goep_conn_id));
                    btmtk_oppc_ui_notify(BT_OPPCUI_PULLED, (void *)GOEP_DATABASE_FULL);
					return;
				}else{
				    btmtk_oppc_ui_notify(BT_OPPCUI_PULLED, (void *)GOEP_DATABASE_FULL);
				    BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
                    bt_oppc_disconnect_req( GOPC(opc_goep_conn_id) );
                    return;
				}
			}
		}else{
		    // file mode
			if( 0 != GOPC(opc_pull_acceptsize) && pCnf->total_obj_len > GOPC(opc_pull_acceptsize) ){
                BT_OPP_LOGD("[EXT] opc_pull_acceptsize:%d total_boj_len:%d", GOPC(opc_pull_acceptsize), pCnf->total_obj_len);
				if( GOEP_FIRST_PKT == pCnf->pkt_type ){
                    /// send abort req
					GOPC(opc_state) = BT_OPC_ABORTING;
                    bt_oppc_abort_req(GOPC(opc_goep_conn_id));
                    btmtk_oppc_ui_notify(BT_OPPCUI_PULLED, (void *)GOEP_DATABASE_FULL);
					return;
				}else{
                    btmtk_oppc_ui_notify(BT_OPPCUI_PULLED, (void *)GOEP_DATABASE_FULL);
				    BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
                    bt_oppc_disconnect_req( GOPC(opc_goep_conn_id) );
					return;
				}

		    }
		}
	}

    if( FALSE != GOPC(opc_is_pullmem) ){
        btmtk_oppc_pull_mem_continue_routine(pCnf);
    }else{
        btmtk_oppc_pull_file_continue_routine(pCnf);
    }
}

BT_BOOL btmtk_oppc_pull_mem_continue_routine(goep_pull_cnf_struct *pCnf){
    BT_BOOL l_ret;
    U32 len;

    l_ret = FALSE;
    // save data into buffer. if the buffer is not enough, notify error to ap
    if ( GOPC(opc_state) != BT_OPC_RECEIVING ){
        BT_OPP_LOGD( "[EXT] error state in pull_cnf_handler");
        return l_ret;
    }

    // copy data into mem and increase offset
   if( GOEP_STATUS_SUCCESS == pCnf->rsp_code ){
        if( 0 == GOPC(total_obj_len) && 0 != pCnf->total_obj_len){
            GOPC(total_obj_len) = GOPC(remain_put_len) = pCnf->total_obj_len;
            if( GOPC(total_obj_len) > GOPC(opc_recv_mem_size) ){
                // not enouth
                l_ret = FALSE;
                btmtk_oppc_ui_notify(BT_OPPCUI_PULLED, (void *)GOEP_INTERNAL_SERVER_ERR);
                BT_OPP_LOGD( "[EXT][err] func:pull_mem_cont no space total_obj_len:%d recv_mem_size:%d\n", GOPC(total_obj_len) , GOPC(opc_recv_mem_size) );
                goto PULL_DONE_NEXT_STAGE;
            }
        }

        if( 0 != GOPC(total_obj_len) && (GOPC(remain_put_len) >= pCnf->frag_len) ){
                GOPC(remain_put_len) -= pCnf->frag_len;
        }

        if( pCnf->frag_len >  (GOPC(opc_recv_mem_size) - GOPC(opc_recv_mem_offset)) ){
                l_ret = FALSE;
                btmtk_oppc_ui_notify(BT_OPPCUI_PULLED, (void *)GOEP_INTERNAL_SERVER_ERR);
                BT_OPP_LOGD( "[EXT][err] func:pull_mem_cont no space frag_len:%d recv_mem_space:%d\n", pCnf->frag_len, (GOPC(opc_recv_mem_size) - GOPC(opc_recv_mem_offset))  );
                goto PULL_DONE_NEXT_STAGE;
        }

        len = min( pCnf->frag_len ,  (GOPC(opc_recv_mem_size) - GOPC(opc_recv_mem_offset)));
        GOEP_MEMCPY( GOPC(opc_recv_mem), pCnf->frag_ptr, len);
        GOPC(opc_recv_mem_offset) += len;
        l_ret = TRUE;

        if(  0 == GOPC(remain_put_len) ){
		    /* notify current pulling state */
            btmtk_oppc_ui_notify(BT_OPPCUI_PULLING, (void *)GOPC(opc_recv_mem_offset) );

            // finish
            btmtk_oppc_ui_notify(BT_OPPCUI_PULLED, (U8 *)((U32)pCnf->rsp_code));
            goto PULL_DONE_NEXT_STAGE;
        }else{
            // continue
            bt_oppc_send_pull_mem_routine( FALSE );
            return l_ret;
        }
    }else{

        // error
        btmtk_oppc_ui_notify(BT_OPPCUI_PULLED, (U8 *)((U32)pCnf->rsp_code));
        l_ret = FALSE;
        goto PULL_DONE_NEXT_STAGE;
    }
    return l_ret;

PULL_DONE_NEXT_STAGE:
    switch( GOPC(opc_feature) ){
        case BT_OPP_EXCHANGE_FEATURE:
        case BT_OPP_PULL_FEATURE:
            BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
            bt_oppc_disconnect_req(pCnf->goep_conn_id);
            btmtk_oppc_ui_notify(BT_OPPCUI_DISCONNECTING, NULL);
        break;
        case BT_OPP_PUSH_FEATURE:
            BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
            bt_oppc_disconnect_req(pCnf->goep_conn_id);
            btmtk_oppc_ui_notify(BT_OPPCUI_DISCONNECTING, NULL);
        break;
        case BT_OPP_READY_FEATURE:
            break;
        default:
            BT_OPP_ASSERT(0);
            break;
    }
    return l_ret;
}

void btmtk_oppc_pull_file_continue_routine(goep_pull_cnf_struct *pCnf){
    char buffer[80];
    S32 i4Written;

    if ( GOPC(opc_state) != BT_OPC_RECEIVING ){
        BT_OPP_LOGD( "[EXT] error state in pull_cnf_handler");
        return;
    }

    if( 0 != pCnf->frag_len ){
        GOEP_MEMCPY( buffer,pCnf->frag_ptr , (pCnf->frag_len>79?79:pCnf->frag_len) );
        buffer[(pCnf->frag_len>79?79:pCnf->frag_len)] = 0;
    }else{
        BT_OPP_LOGD("[EXT] empty content\n");
    }

    if( GOEP_STATUS_SUCCESS == pCnf->rsp_code ){
        if( 0 == GOPC(total_obj_len) && 0 != pCnf->total_obj_len){
            GOPC(total_obj_len) = GOPC(remain_put_len) = pCnf->total_obj_len;
        }
        if( 0 != GOPC(total_obj_len) && (GOPC(remain_put_len) >= pCnf->frag_len) ){
            GOPC(remain_put_len) -= pCnf->frag_len;
        }

        //save data and clsoe file
        if( FALSE == ext_fs_is_valid_handle_ucs2(GOPC(h_opc_recv)) ){
            GOPC(h_opc_recv) = bt_oppc_first_pull_routine(pCnf->frag_ptr, pCnf->frag_len);

            /// fail to open file for receiving data
            if( FALSE == ext_fs_is_valid_handle_ucs2(GOPC(h_opc_recv)) ){
                btmtk_oppc_ui_notify(BT_OPPCUI_PULLED,  (void *)GOEP_STATUS_NO_RESOURCES);
                if( BT_OPP_READY_FEATURE != GOPC(opc_feature) ){
                    bt_oppc_disconnect_req(pCnf->goep_conn_id);
                    btmtk_oppc_ui_notify(BT_OPPCUI_DISCONNECTING, NULL);
                    return;
                }
            }
        }else{
            if( EXT_FS_OK != btmtk_goep_write_wstream(GOPC(h_opc_recv), pCnf->frag_ptr, pCnf->frag_len , &i4Written) ){
                // close it and delete it
                btmtk_goep_close_wstream(GOPC(h_opc_recv));
                GOPC(h_opc_recv) = FHANDLE_INVALID_VALUE;
                btmtk_oppc_ui_notify( BT_OPPCUI_PULLED, (void *)GOEP_STATUS_NO_RESOURCES);
                return;

            }else{
            }
        }

        if( GOEP_SINGLE_PKT  == pCnf->pkt_type || GOEP_FINAL_PKT == pCnf->pkt_type ){
				/* notify the last pulling pkt */
				btmtk_oppc_ui_notify( BT_OPPCUI_PULLING , (void *)(GOPC(total_obj_len) - GOPC(remain_put_len)) );

			    if( TRUE == GOPS(ops_use_nbuffer) ){
	                 bt_oppc_flush_all_nbuffer();
				}
	            btmtk_goep_close_wstream(GOPC(h_opc_recv));
	            GOPC(h_opc_recv) = FHANDLE_INVALID_VALUE;

	            BT_OPP_LOGD( "[EXT] func:pull_file_continue receive %d len data '%s' \n" , pCnf->frag_len, pCnf->frag_ptr );
	            BT_OPC_STATE_TRANS(BT_OPC_CONNECTED);
	            btmtk_oppc_ui_notify(BT_OPPCUI_PULLED, (U8 *)((U32)pCnf->rsp_code));

	            goto PULL_DONE_NEXT_STAGE;
		}else{
	            // send
	            if( FALSE != GOPC(isAbort) ){
	                /* Read error, we abort this operation */
	                BT_OPP_SET_FLAG(BT_OPP_MASK_ABORT_PRESS);
	                BT_OPC_STATE_TRANS(BT_OPC_ABORTING);
	                bt_oppc_abort_req(GOPC(opc_goep_conn_id));
	                return;
	            }else{
	                bt_oppc_send_pull_vcard_routine( FALSE );
	            }
		}
	}else{ // rsp != success
	        BT_OPC_STATE_TRANS(BT_OPC_CONNECTED);
			btmtk_oppc_ui_notify( BT_OPPCUI_PULLED , (void *)(pCnf->reason==0?pCnf->reason:1));

	        BT_OPP_LOGD( "[EXT] rsp code is fail rsp_code %d, reason %d", pCnf->rsp_code,pCnf->reason);

	        goto PULL_DONE_NEXT_STAGE;
	}

    return;

PULL_DONE_NEXT_STAGE:
    switch( GOPC(opc_feature) ){
        case BT_OPP_EXCHANGE_FEATURE:
        case BT_OPP_PULL_FEATURE:
            BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
            bt_oppc_disconnect_req(pCnf->goep_conn_id);
            btmtk_oppc_ui_notify(BT_OPPCUI_DISCONNECTING, NULL);
        break;
        case BT_OPP_PUSH_FEATURE:
            BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
            bt_oppc_disconnect_req(pCnf->goep_conn_id);
            btmtk_oppc_ui_notify(BT_OPPCUI_DISCONNECTING, NULL);
        break;
        case BT_OPP_READY_FEATURE:
            break;
        default:
            BT_OPP_ASSERT(0);
            break;
    }
}

void btmtk_oppc_internal_rw_handler(void *data){
    U8 *pBuffer;
    U32 u4Read, u4BuffSize, u4Index;
    static U32 interval = 0;

	if( FALSE == ext_nbuffer_isInit(&GOPC(opc_mempool)) ){
		return;  // Porting DEMO: 091023. Disable the Nbuffer
	}

    switch(GOPC(opc_state)){
        case BT_OPC_RECEIVING:
            if( FALSE != ext_fs_is_valid_handle_ucs2(GOPC(h_opc_recv)) ){
                if( FALSE == ext_nbuffer_isEmpty(&GOPC(opc_mempool)) ){
                    bt_oppc_flush_one_nbuffer();

                    bt_oppc_send_msg_toself(MSG_ID_BT_OPPC_CLIENT_INTERNAL_RW, NULL, 0);
                }
            }
            break;
        case BT_OPC_SENDING:
        if( FALSE != ext_fs_is_valid_handle_ucs2(GOPC(h_opc_snd)) ){
           if( ext_nbuffer_isFull(&GOPC(opc_mempool)) ){
                // stop to read file
           }else if( EXT_FS_OK != GOPC(opc_fs_status) ){
                // has a previous file manipulation ferror
                if( EXT_FS_ERR_EOF == GOPC(opc_fs_status) ){
                    // do nothing
                }else{
                    btmtk_oppc_ui_notify(BT_OPPCUI_ERROR, NULL); //internal error
                }
           }else{
                // read data to a free buffer
                pBuffer = ext_nbuffer_get_tail_buffer(&GOPC(opc_mempool), &u4Index, (S32 *)&u4BuffSize);
                BT_OPP_ASSERT( NULL != pBuffer );
                BT_OPP_ASSERT( u4BufferSize >= GOPC(opc_mtu));
                if( NULL != pBuffer ){
                    GOPC(opc_fs_status) = btmtk_goep_read_rstream(GOPC(h_opc_snd), pBuffer, GOPC(opc_mtu), (S32 *)&u4Read);
                    if( u4Read > 0 && EXT_FS_OK == GOPC(opc_fs_status) ){
                        ext_nbuffer_return_tail_buffer(&GOPC(opc_mempool), u4Index, u4Read);
                        bt_oppc_send_msg_toself(MSG_ID_BT_OPPC_CLIENT_INTERNAL_RW, NULL, 0);
                    }else
                    if( 0 == u4Read  && EXT_FS_OK == GOPC(opc_fs_status) ){
                        // reach eof
                        GOPC(opc_fs_status) = EXT_FS_ERR_EOF;
                    }else{
                        BT_OPP_LOGD("[EXT] u4Read:%d fs_status:%d", u4Read, GOPC(opc_fs_status) );
                    }
                    // send internal rw data
                }
           }
        }else{
            BT_OPP_LOGD("[EXT][WRN] state:%d h_opc_snd:%d is not valid", GOPC(opc_state), GOPC(h_opc_snd));
        }
        break;
        default:
            break;
    }
}


void btmtk_oppc_auth_ind_handler(void *data){
    goep_auth_ind_struct *auth_ind;
    auth_ind = (goep_auth_ind_struct *) data;
    // TODO: test this scenario. Current don't has a product using OPP auth
#if GOEP_TEST
    GOEP_ASSERT(0);
#endif
}


void btmtk_oppc_auth_rsp_handler(void *data){
    goep_auth_rsp_struct *auth_rsp;
    auth_rsp = (goep_auth_rsp_struct *) data;
    // TODO: connect the remote. Current don't has a product using OPP auth
#if GOEP_TEST
    GOEP_ASSERT(0);
#endif

}


void btmtk_oppc_inject_msg_handler(void * data){
	// MSG_ID_BT_OPPC_DISCONNECT_CNF use bt_test_cmd_req_struct
	bt_test_cmd_req_struct *test_msg;
	U8 *ptr;
	//U8 filepath[128];
	U32 ret = 0;

	test_msg = (bt_test_cmd_req_struct *)data;
	if( NULL != test_msg ){
		test_msg->test_cmd_content[sizeof(test_msg->test_cmd_content) - 1 ] = 0;
		ptr = test_msg->test_cmd_content;
		BT_OPP_LOGD("[EXT][test] start cmd:(%s)", ptr);

		//if( TRUE == static_oppc_fs_tester(ptr, &ret ) ){
		//}

		BT_OPP_LOGD("[EXT][test] end result: %d" , ret );
	}else{
		BT_OPP_LOGD("[EXT][test][err] NO cmd !");
	}
}

#if 0
/*****************************************************************************
 * FUNCTION
 *  bt_opp_is_sending
 * DESCRIPTION
 *  This function is to get current action.
 *
 *  PARAMETERS: void
 *  void
 *  RETURNS: received file counter.(?)
 *****************************************************************************/
BT_BOOL bt_oppc_is_sending(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (GOPC(opc_state) == BT_OPC_CONNECTING ||
        GOPC(opc_state) == BT_OPC_SENDING ||
        GOPC(opc_state) == BT_OPC_CONNECTED
        )
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
 *  bt_opp_is_aborting
 * DESCRIPTION
 *  This function is to get current action.
 *
 *  PARAMETERS: void
 *  void
 *  RETURNS: received file counter.(?)
 *****************************************************************************/
BT_BOOL bt_oppc_is_aborting(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (GOPC(opc_state) == BT_OPC_ABORTING ||
        GOPC(opc_state) == BT_OPC_DISCONNECTING)
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
 *  bt_opp_disconncting
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *  BT_BOOL
 *****************************************************************************/
BT_BOOL bt_oppc_is_disconncting(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (BT_OPC_DISCONNECTING== GOPC(opc_state))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


BT_BOOL bt_oppc_is_idle(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (GOPC(opc_state) == BT_OPC_IDLE )

    {
        return TRUE;
    }
    return FALSE;
}
#endif

void bt_oppc_reset_push_info(void){
    /// clean up the push object data
    GOPC(total_obj_len) = 0;
    GOPC(remain_put_len) = 0;
    GOPC(flag) = 0x00000000;
    GOPC(opc_disconnflag) = FALSE;

    GOPC(opc_snd_mem) = NULL;
    GOPC(opc_is_pushmem) = FALSE;
    GOPC(opc_obj_path)[0] = GOPC(opc_obj_path)[1] = 0;
    GOPC(opc_obj_mime)[0] = GOPC(opc_obj_mime)[1] = 0;
    GOPC(opc_obj_name)[0] = GOPC(opc_obj_name)[1] = 0;
}

void bt_oppc_reset_pull_info(void){
    /// clean up the pull object data
    GOPC(total_obj_len) = 0;
    GOPC(remain_put_len) = 0;
    GOPC(flag) = 0x00000000;
    GOPC(opc_disconnflag) = FALSE;

    GOPC(opc_recv_mem) = NULL;
    GOPC(opc_is_pullmem) = FALSE;
    GOPC(opc_recv_mem) = NULL;
    GOPC(opc_pull_path)[0] = GOPC(opc_pull_path)[1] = 0;
    GOPC(opc_pull_mime)[0] = GOPC(opc_pull_mime)[1] = 0;
    GOPC(opc_pull_name)[0] = GOPC(opc_pull_name)[1] = 0;
    GOPC(opc_pull_acceptsize) = 0;
}

/*****************************************************************************
 * FUNCTION
 *  BT_OPPsending_percent
 * DESCRIPTION
 *  This function is to retrieve the file put progress.
 * PARAMETERS
 *  void
 * RETURNS
 *  1 ~ 100  The percentage of put process.
 *****************************************************************************/
U8 bt_oppc_get_sending_percent(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Total length of a object */

    if (GOPC(total_obj_len) == 0)
    {
        return 0;
    }
    return (U8) (((double)(GOPC(total_obj_len) - GOPC(remain_put_len)) /
                  (double)GOPC(total_obj_len)) * 100.0);
}


/*****************************************************************************
 * FUNCTION
 *  BT_OPPget_obj_attrib
 * DESCRIPTION
 *  This function is to get file mimetype.
 *
 *  PARAMETERS: void
 *  void
 *  RETURNS: received file counter.(?)
 *****************************************************************************/
void bt_oppc_update_obj_attrib_routine(S8 *path)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	/* Using opc_obj_path to update obj_name, mime field */

    /* Get object name */
    if( 0 == ext_ucs2strlen( (const S8*) GOPC(opc_obj_name))){
	    if( 0 != btmtk_goep_get_file_basename( (const U8 *)GOPC(opc_obj_path), (U8 *) GOPC(opc_obj_name), sizeof(GOPC(opc_obj_name)) ) ){
	         // TODO: create a temp filename ?
	         /// setup the opc_obj_name as the default name
	         ext_ucs2ncpy( (S8 *) GOPC(opc_obj_name), (const S8 *)BT_OPP_SND_DEFAULT_OBJNAME, sizeof(GOPC(opc_obj_name)) );
	    }
	}

#if 0
    /* Get object mime type */
    if( 0 == ext_ucs2strlen( (const S8*) GOPC(opc_obj_mime) ) ){
		mime = bt_opp_util_get_file_mime_type(GOPC(opc_obj_name));

		if (mime != NULL)
		{
		    len = ext_strlen(mime);
		    BT_OPP_ASSERT(len <= OPP_MAX_OBJ_MIME_LENGTH);
		    ext_strncpy(GOPC(opc_obj_mime), mime, sizeof(GOPC(opc_obj_mime)) );
		}
	}
#endif
}


/*****************************************************************************
 * FUNCTION
 *  bt_oppc_connection_terminated
 * DESCRIPTION
 *  This is common function to deal with opc disconnect
 * PARAMETERS
 *  cause       [IN]        The cause for connection terminated
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_oppc_connection_terminated(U8 cause)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //TODO UI notification btsend_callback callback;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_LOG1(bt_opp_OPPC_CONN_TERM, cause);
    //BT_OPP_LOG_STATE();

    GOPC(opc_state) = BT_OPC_IDLE ;
    if( btmtk_goep_is_valid_handle(GOPC(h_opc_snd)) )//X: if (GOPC(h_opc_snd) >= BT_FS_NO_ERROR)
    {
        btmtk_goep_close_rstream(GOPC(h_opc_snd)); //FS_Close(GOPC(h_opc_snd));
    }
    GOPC(h_opc_snd) = (FHANDLE)FHANDLE_INVALID_VALUE;

    if( btmtk_goep_is_valid_handle( GOPC(h_opc_recv))  ){
        btmtk_goep_close_wstream(GOPC(h_opc_recv)); //FS_Close(GOPC(h_opc_snd));
    }
    GOPC(h_opc_recv) = (FHANDLE)FHANDLE_INVALID_VALUE;

    BT_OPC_STATE_TRANS(BT_OPC_IDLE);

    if (BT_OPP_FLAG_IS_ON(BT_OPP_MASK_CM_NOTIFY))
    {

         GOPC(opc_disconnflag) = FALSE;
        //BT_OPP_RESET_FLAG(BT_OPP_MASK_CM_NOTIFY);
    }
    GOEP_MEMSET((U8 *) &GOPC(opc_dev_addr), 0, sizeof(GOPC(opc_dev_addr)));

    if (GOPC(opc_push_buff))
    {
          btmtk_oppc_return_buffer(GOPC(opc_push_buff));
          GOPC(opc_push_buff) = NULL;
    }

    /*when user press end key or plug in usb,
        shall not popup "unfinished" */
    if (BT_OPP_FLAG_IS_ON(BT_OPP_MASK_ABORT_PRESS) ||
        BT_OPP_FLAG_IS_ON(BT_OPP_MASK_SENDING_DELE)||
        BT_OPP_FLAG_IS_ON(BT_OPP_MASK_USB_PLUG_IND))
    {
        cause = BTSEND_RET_USER_ABORT;
        //BT_OPP_RESET_FLAG(BT_OPP_MASK_ABORT_PRESS);
        //BT_OPP_RESET_FLAG(BT_OPP_MASK_SENDING_DELE);
        //BT_OPP_RESET_FLAG(BT_OPP_MASK_USB_PLUG_IND);
    }


    /* Execute application callback function if any */
    switch (GOPC(opc_state))
    {
    case BT_OPC_IDLE:
        break;

    case BT_OPC_CONNECTING:
    case BT_OPC_CONNECTED:
    case BT_OPC_SENDING:
    case BT_OPC_ABORTING:
    case BT_OPC_DISCONNECTING:
        GOPC(opc_goep_conn_id) = OPP_INVALID_CONN_ID;
        GOPC(opc_cm_conn_id) = OPP_INVALID_CM_ID;
        BT_OPC_STATE_TRANS(BT_OPC_IDLE);
        break;

    default:
        BT_OPC_LOG_STATE_ERR();
        break;
    }
    GOPC(opc_state) = BT_OPC_IDLE ;
    btmtk_oppc_ui_notify(BT_OPPCUI_DISCONNECTED, (U8 *)((U32)cause));
    GOPC(opc_feature) = BT_OPP_READY_FEATURE;
    btmtk_oppc_ui_notify(BT_OPPCUI_IDLE, (void *)GOEP_STATUS_SUCCESS);
}

static void bt_oppc_flush_one_nbuffer(){
    S32 i4Read, i4Written;
    if( FALSE != ext_nbuffer_pop_data(&GOPC(opc_mempool), GOPC(opc_push_buff), OPPC_MAX_OBEX_PACKET_LENGTH, &i4Read) ){
        GOPC(opc_fs_status) = btmtk_goep_write_wstream( GOPC(h_opc_recv), GOPC(opc_push_buff), i4Read, &i4Written);
        BT_OPP_ASSERT(i4Read != i4Written);
        if( EXT_FS_OK !=  GOPC(opc_fs_status)){
            return;
        }
    }else{
    }
}

static void bt_oppc_flush_all_nbuffer(){
    S32 i4Read, i4Written;
    S32 index;
    // write all the mem block in nbufer
    //[]
    for( index = 0; index < NBUFFER_NUM_OF_MEM; index++){
        if( FALSE != ext_nbuffer_pop_data(&GOPC(ops_mempool), GOPC(opc_push_buff), OPPC_MAX_OBEX_PACKET_LENGTH, &i4Read) ){
            GOPC(opc_fs_status) = btmtk_goep_write_wstream( GOPC(h_opc_recv), GOPC(opc_push_buff), i4Read, &i4Written);
            BT_OPP_ASSERT(i4Read != i4Written);
            if( EXT_FS_OK !=  GOPC(opc_fs_status)){
                return;
            }
        }else{
            break;
        }
    }
}


/**
* @brief This function is to generate temporary object for writing
* @param obj_name suggestion name
* @param final_filepath the suggested folder and output the object's path
* @return file handle which filename is the final name
**/
static FHANDLE bt_oppc_util_gen_temp_obj(const U16 *obj_name, U16 *final_filepath, U16 maxSize)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    // U16 *tname = g_oppc_tmp_buff;
    //U16 tname[OPPC_TMP_BUFF_SIZE];
    U16 cnt = 0;
    S32 retval = EXT_FS_OK;
    S32 l_ret = FHANDLE_INVALID_VALUE; /// default return
    U32 l_u4Count;
    BT_BOOL l_found = FALSE;
    U8 FolderPath[OPP_MAX_PATH_LENGTH], FilePath[OPP_MAX_PATH_LENGTH], NewFileName[OPP_MAX_PATH_LENGTH];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if( 0 != ext_ucs2strlen( (S8 *) final_filepath ) ){
        btmtk_goep_create_folder( (const U16 *)final_filepath );
    }

    if( TRUE == btmtk_goep_is_folder_exist( (const U16 *) final_filepath) ){
         /// the target path is exist. copy it to FolderPath
         ext_ucs2ncpy( (S8 *) FolderPath,  (const S8*) final_filepath, sizeof(FolderPath)  );
    }else if( 0 != btmtk_oppc_get_default_local_folderpath(FolderPath, 128) ){
         btmtk_goep_create_folder( (const U16 *)FolderPath );
    }else{
         // TODO: log error
         return FHANDLE_INVALID_VALUE;
    }

    if( FALSE == btmtk_goep_is_folder_exist( (const U16*) FolderPath ) ){
        return l_ret;
    }

    // compose the path
    for( l_u4Count = 0; l_u4Count < BT_OPPC_MAX_NUM_TEMP_FILE; l_u4Count++ ){
        if( 0 != btmtk_goep_gen_temp_filename(l_u4Count, (const U8 *)obj_name, (U8 *) NewFileName, OPP_MAX_PATH_LENGTH) ){
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

    if( l_found){
        // open the file and return it
        return btmtk_goep_open_wstream( (const U16*) FilePath, 0 );
    }else{
        return FHANDLE_INVALID_VALUE;
    }
    return l_ret;

}

/*
 * Exported Service functions
 */

/*****************************************************************************
 * FUNCTION
 *  bt_oppc_conn_server_routine
 *  legacy: bt_oppc_conn_server
 * DESCRIPTION
 *  This function is to connect opp server
 * PARAMETERS
 *  path        [IN]
 *  callback    [IN]
 *  is_sfn      [IN]
 *  dst_dev     [IN]
 * RETURNS
 *  BT_OPP_SND_ERR_TYPE
 *****************************************************************************/
BT_BOOL bt_oppc_conn_server_routine(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_LOGD(BT_OPP_CONN_SERVER); QQQ TODO log
    //BT_OPP_LOG_STATE();
	//TODO check this size !
	if( NULL == GOPC(opc_push_buff) ){

		GOPC(opc_push_buff_size) = btmtk_oppc_get_profile_buffer_size();
		GOPC(opc_push_buff) = btmtk_oppc_get_profile_buffer( GOPC(opc_push_buff_size) );
        if( NULL == GOPC(opc_push_buff) ){
            return FALSE;
        }
    }

    if (GOPC(opc_push_buff) == NULL)
    {
        return FALSE;
    }

    BT_OPC_STATE_TRANS(BT_OPC_CONNECTING);
    BT_OPP_LOGD( "[EXT][info] func:conn_server connect bufsize=%d\n", (U16)(btmtk_oppc_get_profile_buffer_size()));
    bt_oppc_connect_req((U16)btmtk_oppc_get_profile_buffer_size() , GOPC(opc_dev_addr));

    btmtk_oppc_ui_notify(BT_OPPCUI_CONNECTING, (void *)GOEP_STATUS_SUCCESS);
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  bt_oppc_disconnect_server_routine
 * DESCRIPTION
 *  This function is to disconnect the OPS connection
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_oppc_disconnect_server_routine() //btbm_bd_addr_t bd_addr, U32 g_conn_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_LOG1(BT_OPP_OPPC_DISC_CLIENT, g_conn_id);
    //BT_OPP_LOG_STATE();

    BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
    GOPC(opc_disconnflag) = TRUE;

    /* Send OPP disconnect request to OBEX */
    bt_oppc_disconnect_req(GOPC(opc_goep_conn_id));

    btmtk_oppc_ui_notify(BT_OPPCUI_DISCONNECTING, (void *) GOEP_USER_CANCEL ); // user cancel
}


/**
* @brief Open file and write data to it
*/
FHANDLE bt_oppc_first_pull_routine(U8 *data, U32 len){
    FHANDLE fh1;
    S32 i4Written = 0;
    fh1 = btmtk_goep_open_wstream( (U16 *)GOPC(opc_pull_path),  0);
    if( FALSE != ext_fs_is_valid_handle_ucs2(fh1) && len > 0 ){
        if( EXT_FS_OK != btmtk_goep_write_wstream(fh1, data, len, &i4Written) ){
            // close it and delete it
            bt_oppc_flush_all_nbuffer();
            btmtk_goep_close_wstream(fh1);
            fh1 = FHANDLE_INVALID_VALUE;
        }
    }
    return fh1;
}


/**
* @brief Send a pull vcard request out
* @param bNeedName Is append the default name to
* @param bFirstPkt Is the first request
*/
void bt_oppc_send_pull_vcard_routine(BT_BOOL bFirstPkt){

    BT_OPC_STATE_TRANS(BT_OPC_RECEIVING);
    if( FALSE != bFirstPkt ){
        GOPC(isAbort) = FALSE;
        GOPC(total_obj_len) = GOPC(remain_put_len) = 0 ;
        bt_oppc_pull_req( GOPC(opc_goep_conn_id), GOEP_FIRST_PKT, (U8 *)GOPC(opc_pull_name), (U8 *)GOPC(opc_pull_mime));
        btmtk_oppc_ui_notify(BT_OPPCUI_PULLING, (void *)0);
    }else{
        // receiving state
        btmtk_oppc_ui_notify(BT_OPPCUI_PULLING, (void *)(GOPC(total_obj_len)-GOPC(remain_put_len)));
        bt_oppc_pull_req( GOPC(opc_goep_conn_id), GOEP_NORMAL_PKT, NULL, NULL );
    }
}


void bt_oppc_send_pull_mem_routine(BT_BOOL bFirstPkt){
    // pull the data into mem
    BT_OPC_STATE_TRANS(BT_OPC_RECEIVING);
    if( FALSE != bFirstPkt ){
        GOPC(isAbort) = FALSE;
        GOPC(total_obj_len) = GOPC(remain_put_len) = 0 ;
        bt_oppc_pull_req( GOPC(opc_goep_conn_id), GOEP_FIRST_PKT, (U8 *)GOPC(opc_pull_name) , (U8 *)GOPC(opc_pull_mime) );
        btmtk_oppc_ui_notify(BT_OPPCUI_PULLING, (void *)0);
    }else{
        // receiving state
        bt_oppc_pull_req( GOPC(opc_goep_conn_id), GOEP_NORMAL_PKT, NULL, NULL );
 	    btmtk_oppc_ui_notify(BT_OPPCUI_PULLING, (void *)(GOPC(total_obj_len)-GOPC(remain_put_len)));
    }

}

/**
* @brief push data by memory pointer
*/
BT_BOOL bt_opc_send_mem_routine(void){
    BT_BOOL l_ret;
    l_ret = FALSE;

    if( FALSE == GOPC(opc_is_pushmem)){
        BT_OPP_LOGD("[EXT][wrn] func:send_mem state:%d pushmem:%d\n", GOPC(opc_state), GOPC(opc_is_pushmem)  );
        GOEP_ASSERT( FALSE == GOPC(opc_is_pushmem) );
        return FALSE;
    }

    if( NULL == GOPC(opc_snd_mem) || GOPC(opc_snd_mem_offset) > GOPC(opc_snd_mem_size)){
        BT_OPP_LOGD("[EXT][wrn] func:send_mem snd_mem:%08x offset:%d size:%d\n", GOPC(opc_snd_mem) , GOPC(opc_snd_mem_offset),  GOPC(opc_snd_mem_size));
        GOEP_ASSERT( 0 );
        return FALSE;
    }

    BT_OPP_LOGD("[EXT][info] func:send_mem snd_mem:%08x offset:%d size:%d\n", GOPC(opc_snd_mem) , GOPC(opc_snd_mem_offset),  GOPC(opc_snd_mem_size));

    GOPC(total_obj_len) = GOPC(opc_snd_mem_size);
    GOPC(opc_snd_mem_offset) = 0;

	/* no pulling UI event. The first push_req is header.*/

    bt_oppc_push_req(
        GOPC(opc_goep_conn_id),
        GOEP_FIRST_PKT,
        GOPC(opc_snd_mem_size),
        (S8 *)GOPC(opc_obj_name),
        GOPC(opc_obj_mime),
        NULL, 0);
    l_ret = TRUE;
    GOPC(opc_state) = BT_OPC_SENDING;

    return l_ret;
}


BT_BOOL bt_opc_send_mem_continue_routine(goep_push_cnf_struct *pCnf){
    BT_BOOL l_ret;
    U32 len;
    goep_pkt_type_enum pkt = 0;

    l_ret = FALSE;
    // check the pull cnf
    if( NULL == pCnf ){
        BT_OPP_LOGD("[EXT][err] func:send_mem_cont pCnf is NULL!\n" );
        GOEP_ASSERT( 0 );
        return FALSE;
    }

	if( GOPC(opc_snd_mem_size) == GOPC(opc_snd_mem_offset) ){
	  BT_OPP_LOGD("[EXT][info] last pkg ack. ");
	  BT_OPC_STATE_TRANS(BT_OPC_CONNECTED);
	  btmtk_oppc_ui_notify(BT_OPPCUI_PUSHED, (void *) (pCnf->rsp_code==0?0:pCnf->reason));
	  goto PUSH_ACTION_FINISH;
	}

    if (GOPC(opc_state) != BT_OPC_SENDING )
    {
        BT_OPP_LOGD("[EXT][err] func:send_mem_cont state:%d \n" , GOPC(opc_state) );
        goto PUSH_ACTION_FINISH;
    }
    BT_OPP_LOGD("[EXT][info] func:send_mem snd_mem:%08x offset:%d size:%d\n", GOPC(opc_snd_mem) , GOPC(opc_snd_mem_offset),  GOPC(opc_snd_mem_size));

    if ( GOEP_STATUS_SUCCESS == pCnf->rsp_code )
    {
        btmtk_oppc_ui_notify(BT_OPPCUI_PUSHING, (void *) GOPC(opc_snd_mem_offset) );

        // copy data from mem and increase the offset
        len = min( (GOPC(opc_snd_mem_size) - GOPC(opc_snd_mem_offset)),  GOPC(opc_mtu) );
        GOEP_MEMCPY( (U8 *) GOPC(opc_push_buff), (U8 *) ( GOPC(opc_snd_mem) + GOPC(opc_snd_mem_offset)), len);
        if( (GOPC(opc_snd_mem_size) - GOPC(opc_snd_mem_offset) - len) > 0 ){
            pkt = GOEP_NORMAL_PKT;
        }else{
            pkt = GOEP_FINAL_PKT;
        }

        bt_oppc_push_req(GOPC(opc_goep_conn_id),
            pkt,
            GOPC(opc_snd_mem_size),
            (S8 *)GOPC(opc_obj_name),
            GOPC(opc_obj_mime),
            (U32*)GOPC(opc_push_buff), (U16)len);
        GOPC(opc_snd_mem_offset) += len;

        l_ret = TRUE;
        if( GOEP_FINAL_PKT == pkt ){
	        // wait for next push_cnf to finish the transmission
	        // but notify the last received pushing data size
	        btmtk_oppc_ui_notify(BT_OPPCUI_PUSHING, (void *) GOPC(opc_snd_mem_offset) );
        }

        return l_ret;

    }else{
        /// end of sending
        BT_OPC_STATE_TRANS(BT_OPC_CONNECTED);
        btmtk_oppc_ui_notify(BT_OPPCUI_PUSHED, (void *) (pCnf->rsp_code!=0?pCnf->reason:1) );
    }


PUSH_ACTION_FINISH:
    // goto next action by current feature
    switch( GOPC(opc_feature) ) {
        case BT_OPP_EXCHANGE_FEATURE:
			if( FALSE == GOPC(opc_is_pullmem) ){
			 bt_oppc_send_pull_vcard_routine(TRUE); // send the first pull packet
			}else{
                bt_oppc_send_pull_mem_routine(TRUE);
            }
            return l_ret;
            break;
        case BT_OPP_READY_FEATURE:
            // do nothing
            break;
        default:
            if( pCnf && pCnf->rsp_code ==1 && pCnf->reason == (GOEP_STATUS_DISCONNECT)){
				// has been disconnected. wait for disconnect_ind
			}else{
                // send disconect req
                BT_OPC_STATE_TRANS(BT_OPC_CONNECTED);
                btmtk_oppc_disconnect_feature(); //disconnect during a feature
            }
        break;
    }
    return l_ret;
}

/**
* leacy api
* @brief This function is to send first packet from file
**/
BT_BOOL bt_opc_send_file_routine(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BT_BOOL ret = TRUE;
    U32 len;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    BT_OPP_LOGD("[EXT][ADP][bt_opc_send_file_routine][+]");

    //check
    if( 0 == GOPC(opc_obj_path) || 0 == GOPC(opc_obj_path)[0] )
    {
        BT_OPP_LOGD("[EXT][ADP][bt_opc_send_file_routine]: file path is NULL ");
        return FALSE;
    }

    if( TRUE == GOPC(opc_is_pushmem))
    {
    	BT_OPP_LOGD("[EXT][ADP][bt_opc_send_file_routine]: state[%d] pushmem[%d]", GOPC(opc_state), GOPC(opc_is_pushmem));
        GOEP_ASSERT( FALSE == GOPC(opc_is_pushmem) );
        return FALSE;
    }

    if( FALSE == ext_nbuffer_isInit( &GOPC(opc_mempool) ) )
    {
        ext_nbuffer_init(&GOPC(opc_mempool), OPPC_MAX_OBEX_PACKET_LENGTH);
    }

    // Open sending object
    GOPC(isAbort) = FALSE;
	GOPC(h_opc_snd) = btmtk_goep_open_rstream((U16*)GOPC(opc_obj_path), EXT_FS_READ_ONLY);

    if( TRUE == GOPC(opc_use_nbuffer)  && FALSE != ext_nbuffer_isInit(&GOPC(opc_mempool)) )
    {
        if( FALSE != ext_nbuffer_isEmpty(&GOPC(opc_mempool)) ){
            // send a internal rw msg to kick off the read/write worker
        	bt_oppc_send_msg_toself(MSG_ID_BT_OPPC_CLIENT_INTERNAL_RW, NULL, 0);
        }
    }

    if( FALSE != ext_nbuffer_isInit(&GOPC(opc_mempool)) )
    {
    	ext_nbuffer_cleanup(&GOPC(opc_mempool));
    }

    if( FALSE == btmtk_goep_is_valid_handle(GOPC(h_opc_snd)) )
    {
		ret = FALSE;
		BT_OPP_LOGD("[EXT][ADP][bt_opc_send_file_routine]: fail to open file[%s]", GOPC(opc_obj_path));
		goto PUSH_ACTION_FINISH;
    }
    GOPC(opc_fs_status) = EXT_FS_OK;

    // Get the object size
    len = 0;
    if( FALSE == btmtk_goep_get_filesize(GOPC(h_opc_snd), (U8 *)GOPC(opc_obj_path) ,&len) )
    {
        // fail to read filesize. read until it returns 0
        BT_OPP_LOGD("[EXT][ADP][bt_opc_send_file_routine]: fail to get file size.");
        GOPC(total_obj_len) = 0xffffffff; //NOTE! OBEX need size!
        GOPC(remain_put_len) = 0xfffffff;
    }
    else
    {
        GOPC(total_obj_len) = len;
        GOPC(remain_put_len) = len;
    }

    // fill the mime and obj_name to opc context
	bt_oppc_update_obj_attrib_routine(GOPC(opc_obj_path));

    // no pulling UI event. The first push_req is header.
    if (ret)
    {
    	BT_OPP_LOGD("[EXT][ADP][bt_opc_send_file_routine]: first push(%s)", (U8 *)GOPC(opc_obj_name));
        BT_OPC_STATE_TRANS(BT_OPC_SENDING);
        // We don't send real data in the first packet
        bt_oppc_push_req(
            GOPC(opc_goep_conn_id),
            GOEP_FIRST_PKT,
            GOPC(total_obj_len),
            (S8 *)GOPC(opc_obj_name),
            GOPC(opc_obj_mime),
            NULL, 0);
    }
    else
    {
    	BT_OPP_LOGD("[EXT][ADP][bt_opc_send_file_routine]: ret fail");

        if( btmtk_goep_is_valid_handle(GOPC(h_opc_snd)) )
        {
            btmtk_goep_close_rstream(GOPC(h_opc_snd));
        }
        GOPC(h_opc_snd) = (FHANDLE)FHANDLE_INVALID_VALUE;
		ret = FALSE;
		goto PUSH_ACTION_FINISH;
    }
    return ret ;

PUSH_ACTION_FINISH:
	btmtk_oppc_ui_notify(BT_OPPCUI_PUSHED, (void *) GOEP_INTERNAL_SERVER_ERR);
    // goto next action by current feature
    switch( GOPC(opc_feature) ) {
        case BT_OPP_EXCHANGE_FEATURE:
			if( FALSE == GOPC(opc_is_pullmem) ){
			 bt_oppc_send_pull_vcard_routine(TRUE); // send the first pull packet
			}else{
                bt_oppc_send_pull_mem_routine(TRUE);
            }
            return ret;
            break;
        case BT_OPP_READY_FEATURE:
            // do nothing
            break;
        default:
            // send disconect req
            BT_OPC_STATE_TRANS(BT_OPC_CONNECTED);
            btmtk_oppc_disconnect_feature(); //disconnect during a feature
        break;
    }
   return ret;
}

BT_BOOL bt_opc_send_file_continue_routine(goep_push_cnf_struct *cnf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                               */
    /*----------------------------------------------------------------*/
	BT_BOOL l_ret;
    U32 sent = 0, read = 0;
    goep_pkt_type_enum pkt = 0;
    S32 retval;

	l_ret = TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (GOPC(opc_goep_conn_id) != cnf->goep_conn_id)
    {
        BT_OPP_LOGD("[EXT][wrn] bt_opc_send_file_continue_routine: wrong goep_conn_id: ext[%d], cnf[%d]", GOPC(opc_goep_conn_id), cnf->goep_conn_id);
    }

    // push finished
    if (0 == GOPC(remain_put_len))
    {
		if( GOPC(opc_state) == BT_OPC_SENDING ){
			BT_OPC_STATE_TRANS(BT_OPC_CONNECTED);
			btmtk_oppc_ui_notify(BT_OPPCUI_PUSHED, (void *) (cnf->rsp_code==1?cnf->reason:cnf->rsp_code));
		}
		BT_OPP_LOGD("[EXT] remain_put_len is 0. final ack");
        goto PUSH_ACTION_FINISH; //send the last piece and result back
    }

    // response code not success => fail
    if (cnf->rsp_code != GOEP_STATUS_SUCCESS)
    {
        BT_OPP_SET_FLAG(BT_OPP_MASK_REJECT);
        // close the push file
        if( FALSE != ext_fs_is_valid_handle_ucs2(GOPC(h_opc_snd)) ){
            btmtk_goep_close_rstream( GOPC(h_opc_snd) );
            GOPC(h_opc_snd) = FHANDLE_INVALID_VALUE;
        }
        GOPC(opc_state) = BT_OPC_CONNECTED;

        btmtk_oppc_ui_notify(BT_OPPCUI_PUSHED, (void *)(cnf->reason==0?1:cnf->reason) );
		BT_OPP_LOGD("[EXT] send_file_continue rsp_code:%d", cnf->rsp_code );
        goto PUSH_ACTION_FINISH;
    }

    // continue
    if (GOPC(remain_put_len) > 0)
    {
        //BT_OPP_LOGD("[EXT] send_file_continue mtu:%d remain:%d", GOPC(opc_mtu), GOPC(remain_put_len) );
        //btmtk_oppc_ui_notify(BT_OPPCUI_PUSHING, (void *)(GOPC(total_obj_len) - GOPC(remain_put_len)));

        // object is bigger than MTU => normal packet
        if (GOPC(remain_put_len) > GOPC(opc_mtu))
        {
            sent = GOPC(opc_mtu);
            pkt = GOEP_NORMAL_PKT;
        }
        // object is equal to or smaller than MTU => final packet
        else
        {
            sent = GOPC(remain_put_len);
            pkt = GOEP_FINAL_PKT;
            // wait for server's ack to change status
        }

        // check abort
        if( FALSE != GOPC(isAbort) )
        {
            /* read error, we abort this operation */
            BT_OPP_LOGD("[EXT] send_file_continue isAbort(%d)", GOPC(isAbort) );
            BT_OPP_SET_FLAG(BT_OPP_MASK_ABORT_PRESS);
            BT_OPC_STATE_TRANS(BT_OPC_ABORTING);
            bt_oppc_abort_req(GOPC(opc_goep_conn_id));
            btmtk_oppc_ui_notify(BT_OPPCUI_ABORTING,0);
            goto PUSH_ACTION_FINISH;
        }
#if 0
disable nbuffer
        if( FALSE != ext_nbuffer_isInit(&GOPC(opc_mempool)) ){
	        if( FALSE != ext_nbuffer_isEmpty(&GOPC(opc_mempool)) ){

	            retval = btmtk_goep_read_rstream(GOPC(h_opc_snd), GOPC(opc_push_buff), sent, &read); // retval = FS_Read(GOPC(h_opc_snd), GOPC(opc_push_buff), sent, &read);
	        }else{
	        	//BT_OPP_LOGD("[EXT] bt_oppc_hdl->bt_opc_send_file_continue_routine: copy nbuffer..." );
	            /// copy one data from. data size == 0~GOPC(opc_mtu)
	            if( FALSE == ext_nbuffer_pop_data(&GOPC(opc_mempool), GOPC(opc_push_buff), OPPC_MAX_OBEX_PACKET_LENGTH, &read) ){
	                //BT_OPP_ASSERT( read == sent );
	                retval = EXT_FS_ERR;
	            }else{
	                retval = EXT_FS_OK;
	            }
	        }
	    }
        else
#endif
        {
#ifdef __BT_GOEPC_AUTO_ACK__
            retval = btmtk_goep_read_rstream(GOPC(h_opc_snd), GOPC(opc_push_buff) + (OPPC_MAX_OBEX_PACKET_LENGTH * cnf->buff_index), sent, &read);
#else
            retval = btmtk_goep_read_rstream(GOPC(h_opc_snd), GOPC(opc_push_buff), sent, (S32 *)&read);
#endif

	    }
#if 0
disable nbuffer
        if( FALSE != ext_nbuffer_isInit(&GOPC(opc_mempool)) && FALSE == ext_nbuffer_isFull(&GOPC(opc_mempool)) && GOPC(remain_put_len) > read ){
            /// send a internal rw msg to kick off the read/write worker
            bt_oppc_send_msg_toself(MSG_ID_BT_OPPC_CLIENT_INTERNAL_RW, NULL, 0);
        }
#endif

        // file io error
        if( EXT_FS_OK != retval || read != sent )
        {
        	BT_OPP_LOGD("[EXT][wrn] send_file_continue read data fail ret:(%d) read:(%d) sent:(%d)", retval , read, sent );
            /* Read error, we abort this operation */
            BT_OPP_SET_FLAG(BT_OPP_MASK_BAD_FILE);
            BT_OPC_STATE_TRANS(BT_OPC_ABORTING);
            bt_oppc_abort_req(GOPC(opc_goep_conn_id));
            btmtk_oppc_ui_notify(BT_OPPCUI_PUSHED, (void *)GOEP_INTERNAL_SERVER_ERR);
            goto PUSH_ACTION_FINISH;
        }
        else
        {
        	// TODO BT_OPP_LOGD("[EXT] send_file_continue push data total:(%d) sent:(%d) remain:(%d)", GOPC(total_obj_len), sent, GOPC(remain_put_len) );
            bt_oppc_push_req( GOPC(opc_goep_conn_id),
                                pkt,
                                GOPC(total_obj_len),
                                (S8 *)GOPC(opc_obj_name),
                                GOPC(opc_obj_mime),
                                (U32*)GOPC(opc_push_buff), (U16)sent);

            btmtk_oppc_ui_notify(BT_OPPCUI_PUSHING, (void *)(GOPC(total_obj_len) - GOPC(remain_put_len)));
            GOPC(remain_put_len) -= sent;

            // push finish
            if( 0 == GOPC(remain_put_len) )
            {
                BT_OPP_LOGD("[EXT] send_file_continue push data remain 0 ");
                if( FALSE != ext_fs_is_valid_handle_ucs2(GOPC(h_opc_snd)) )
                {
                    btmtk_goep_close_rstream(GOPC(h_opc_snd));
                    GOPC(h_opc_snd) = (FHANDLE)FHANDLE_INVALID_VALUE;
                    if( FALSE != ext_nbuffer_isInit(&GOPC(opc_mempool)) ){
                    	ext_nbuffer_cleanup(&GOPC(opc_mempool));
                	}
		            // If file size < mtu, MMI should not show 100% progressbar due to remote is doing authorize
		            // (some device start authorize while receiving file data not file info)
		            if (GOPC(total_obj_len) > GOPC(opc_mtu)){
						btmtk_oppc_ui_notify(BT_OPPCUI_PUSHING, (void *)(GOPC(total_obj_len)-GOPC(remain_put_len))); // single packet. send notify twice
					}		    
                }
            }
        }
    }
    else    // (GOPC(remain_put_len) <= 0)
    {
        btmtk_oppc_ui_notify(BT_OPPCUI_PUSHED, (void *)(cnf->rsp_code!=0?cnf->reason:1));
        BT_OPC_STATE_TRANS(BT_OPC_CONNECTED);
        if( FALSE != ext_fs_is_valid_handle_ucs2(GOPC(h_opc_snd)) )
        {
            // send the empty back
            bt_oppc_push_req(GOPC(opc_goep_conn_id),
                GOEP_FINAL_PKT,
                0,
                (S8 *)GOPC(opc_obj_name),
                GOPC(opc_obj_mime),
                (U32*)GOPC(opc_push_buff), (U16)0);
            btmtk_goep_close_rstream(GOPC(h_opc_snd));
            GOPC(h_opc_snd) = (FHANDLE)FHANDLE_INVALID_VALUE;
            if( FALSE != ext_nbuffer_isInit(&GOPC(opc_mempool)) ){
            	ext_nbuffer_cleanup(&GOPC(opc_mempool));
            }
        }
        goto PUSH_ACTION_FINISH;
    }

    return l_ret;

PUSH_ACTION_FINISH:
	BT_OPP_LOGD("[EXT] push done. feature:(%d) goto next level ", GOPC(opc_feature));
    // goto next action by current feature
    switch( GOPC(opc_feature) ) {
        case BT_OPP_EXCHANGE_FEATURE:
			if( FALSE == GOPC(opc_is_pullmem) ){
			 bt_oppc_send_pull_vcard_routine(TRUE); // send the first pull packet
			}else{
                bt_oppc_send_pull_mem_routine(TRUE);
            }
            return l_ret;
            break;
        case BT_OPP_READY_FEATURE:
            // do nothing
            break;
        default:
            if( cnf && cnf->rsp_code ==1 && cnf->reason == (GOEP_STATUS_DISCONNECT)){
				// has been disconnected. wait for disconnect_ind
			}else{
                // send disconect req
                BT_OPC_STATE_TRANS(BT_OPC_CONNECTED);
                btmtk_oppc_disconnect_feature(); //disconnect during a feature
            }
        break;
    }
   return l_ret;
}

#ifdef GOEP_TEST
/**
* @brief
* lagacy api: bt_opc_send_first_packet
* @brief for TESTING, This function is to send first packet from file as a single
**/
BT_BOOL bt_opc_send_file_singlepkg_routine(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BT_BOOL ret = TRUE;
    U32 len;
    U32 retval, sent, read;
    goep_pkt_type_enum pkt;


    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    //check
    if( 0 == GOPC(opc_obj_path)[0] && 0 == GOPC(opc_obj_path)[1]){
        return FALSE;
    }

    /* Open sending object */
    GOPC(isAbort) = FALSE;
    GOPC(h_opc_snd) = btmtk_goep_open_rstream((U16*)GOPC(opc_obj_path), FS_READ_ONLY); //GOPC(h_opc_snd) = FS_Open((U16*)GOPC(opc_obj_path), FS_READ_ONLY);

    //BT_OPP_LOG1(MMI_BT_OPP_SEND_FILE_OBJ_FS_OPEN, GOPC(h_opc_snd));

    if( FALSE == btmtk_goep_is_valid_handle(GOPC(h_opc_snd)) ) //X: if (GOPC(h_opc_snd) < FS_NO_ERROR)
    {
        BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
        bt_oppc_disconnect_req(GOPC(opc_goep_conn_id));
        return FALSE;
    }

    /* Get the object size */
    len = 0;
    if( FALSE == btmtk_goep_get_filesize(GOPC(h_opc_snd), GOPC(opc_obj_path) ,&len) ){
        // fail to read filesize. read until it returns 0
        GOPC(total_obj_len) = 0xffffffff;
        GOPC(remain_put_len) = 0xfffffff;
    }else{
        GOPC(total_obj_len) = len;
        GOPC(remain_put_len) = len;
    }

    bt_oppc_update_obj_attrib_routine(GOPC(opc_obj_path)); /// fill the mime and obj_name to opc context

    //BT_OPP_LOG1(MMI_BT_OPP_SEND_FIRST_PACKET,ret);
    btmtk_oppc_ui_notify(BT_OPPCUI_PUSHING, (void *) (GOPC(total_obj_len) - GOPC(remain_put_len)));  // prepare to pushing. the first one 0%

    if (ret)
    {
        BT_OPC_STATE_TRANS(BT_OPC_SENDING);
        if ( GOPC(total_obj_len) > 1024 ){
            /* We don't send real data in the first packet */
            bt_oppc_push_req(
                GOPC(opc_goep_conn_id),
                GOEP_FIRST_PKT,
                GOPC(total_obj_len),
                (S8 *)GOPC(opc_obj_name),
                GOPC(opc_obj_mime),
                NULL, 0);
        }else{
            // read data from file
            sent = GOPC(remain_put_len);
            retval = btmtk_goep_read_rstream(GOPC(h_opc_snd), GOPC(opc_push_buff), sent, &read); // retval = FS_Read(GOPC(h_opc_snd), GOPC(opc_push_buff), sent, &read);

            if( retval != EXT_FS_OK ){
                // cannot read file
                btmtk_oppc_ui_notify(BT_OPPCUI_PUSHED, (void *)GOEP_INTERNAL_SERVER_ERR);
                ret = FALSE;
            }else{
                GOPC(remain_put_len) -= sent;
                pkt = GOEP_FIRST_PKT;

                if(  0 ==  GOPC(remain_put_len) && sent > 0){
                    GOPC(opc_state) = BT_OPC_SENDING;
                    //pkt = GOEP_SINGLE_PKT;
                    bt_oppc_push_req(GOPC(opc_goep_conn_id),
                        pkt,
                        GOPC(total_obj_len),
                        (S8 *)GOPC(opc_obj_name),
                        GOPC(opc_obj_mime),
                        (U32*)GOPC(opc_push_buff), (U16)sent);
                    ret = TRUE;
                }else{
                    if( FALSE != ext_fs_is_valid_handle_ucs2(GOPC(h_opc_snd)) ){
                        btmtk_goep_close_rstream( GOPC(h_opc_snd) );
                        GOPC(h_opc_snd) = FHANDLE_INVALID_VALUE;
                    }
                    ret = FALSE;
                }


            }

        }
    }
    else
    {
        // fail check
        if( btmtk_goep_is_valid_handle(GOPC(h_opc_snd)) )
        {
            btmtk_goep_close_rstream(GOPC(h_opc_snd));
        }
        GOPC(h_opc_snd) = (FHANDLE)FHANDLE_INVALID_VALUE;

        if (GOPC(opc_state) != BT_OPC_DISCONNECTING)
        {
            BT_OPC_STATE_TRANS(BT_OPC_DISCONNECTING);
            bt_oppc_disconnect_req(GOPC(opc_goep_conn_id));
        }
    }
    return ret ;
}
#endif
