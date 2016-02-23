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
#if defined(__BT_JSR82__)

#include "bt_common.h"
#include "jsr82_session.h"
#include "bt_adp_jsr82.h"
#include "bt_adp_jsr82_event.h"
#include "bt_adp_jsr82_session.h"
#if (BT_JSR82_ENABLED == 1)
#include "bluetooth_sdp_struct.h"
#include "appsdpdb.h"
#endif
#include "bt_os_api.h"
#include "session_api.h"
#include "session_os.h"
#include "bt_session.h"

//test
#include "session_stream.h"

BT_ADP_JSR82_CONTEXT jsr82_adp_cntx;
extern SESSION_RESULT jsr82_session_monitor(Session_Event_PARM *parm);

void jsr82_adp_init(void)
{
	SESSION_RESULT sret = SR_NOERROR;
	int index = 0;
	
	if (jsr82_adp_cntx.init)
	{
		OS_Report("[JSR82]the context has been initialized");
		return;
	}
	JSR82_Register(btmtk_adp_jsr82_int_event_callback);
#if (BT_JSR82_ENABLED == 1)
	BTAppSdpDbInit();
#endif
	BTJSR82_SessionApInit();

	memset(&jsr82_adp_cntx, 0x0, sizeof(BT_ADP_JSR82_CONTEXT));
	jsr82_adp_cntx.init = TRUE;

	//set up connection for incoming request from external layer
	sret = bt_session_setup();
	if ((sret != SR_NOERROR) && (sret != SR_E_INUSE))
	{
		return;
	}
	sret = bt_session_register_handler(PROFILE_JSR82_DATA, 
									   PROFILE_JSR82_DATA_RANGE, 
									   jsr82_session_monitor, 
									   &jsr82_adp_cntx.handler_id);

	session_mutex_create(&jsr82_adp_cntx.mutex);
	
}

void jsr82_adp_deinit(void)
{
	BTJSR82_DeInitAll();

	bt_session_unregister_handler(jsr82_adp_cntx.handler_id);
	jsr82_adp_cntx.init = FALSE;
	if(jsr82_adp_cntx.mutex)
	{
		session_mutex_destroy(jsr82_adp_cntx.mutex);
		jsr82_adp_cntx.mutex = NULL;
	}
}

void btadp_jsr82_connect_req(bt_jsr82_connect_req_struct *req)
{
	BT_ADP_JSR82_SESSION *session = NULL;
	BOOL ret = TRUE;
	
	OS_Report("[JSR82][ADP]btadp_jsr82_connect_req: sessionid[%8x]", req->sessionid);

	if (NULL != btadp_jsr82_find_session_by_identify(req->identify))
	{
		ret = FALSE;
		OS_Report("[JSR82][ADP]idetify conflicts with existing context: %d", req->identify);
		goto exit;
	}

	session_mutex_lock(jsr82_adp_cntx.mutex);
	session = btadp_jsr82_find_session_by_id(req->sessionid);
	if (session == NULL)
	{
		session = btadp_jsr82_find_available_session();
		if (NULL != session)
		{
			btadp_jsr82_session_init(session);
			session->sessionid = req->sessionid;
			session->conn_state = BT_ADP_JSR82_STATE_ACTIVE;
		}
		else
		{
			OS_Report("[JSR82] no available session context");
		}
	}
	session_mutex_unlock(jsr82_adp_cntx.mutex);

	if (NULL == session)
	{
		ret = FALSE;
		goto exit;
	}

	//TODO: check and decide whether to do sdp		
	session->channel = 0;
	
	btmtk_os_memcpy(session->bd_addr, req->bd_addr, 6);
	btmtk_os_memcpy(session->service, req->uuid_128, 16);
	session->identify = req->identify;
	session->mtu = req->mtu;
	session->ps_type = req->ps_type;
	session->security_value = req->security_value;
	session->role = JSR82_SESSION_ROLE_CLIENT;
	
	btadp_jsr82_session_connect(session);
	
exit:
	if (TRUE != ret)
	{	
    	bt_jsr82_connect_cnf_struct *ptr;
		ptr = (bt_jsr82_connect_cnf_struct*) construct_local_para(sizeof(bt_jsr82_connect_cnf_struct), TD_UL);
	    btmtk_os_memcpy((U8*) & ptr->bd_addr, req->bd_addr, 6);
	    ptr->channel = req->channel;
	    ptr->ps_type = req->ps_type;
	    ptr->index = 0;
	    ptr->identify = req->identify;
	    ptr->l2cap_id = 0;
	    ptr->mtu = req->mtu;
	    ptr->in_mtu = 0;
	    ptr->result = JSR82_SESSION_CONNECT_CLIENT_FAILED;

	    JSR82_sendMsg(MSG_ID_BT_JSR82_CONNECT_CNF, MOD_MMI, (local_para_struct*) ptr, NULL);
		bt_session_destroy(req->sessionid);
	}
}

//matching connnect ind
void btadp_jsr82_connect_rsp(bt_jsr82_connect_rsp_struct* rsp)
{
	BT_ADP_JSR82_SESSION *session = NULL;
	BT_ADP_JSR82_SESSION *server_session = NULL;
	int fd = rsp->fd;


	//the server session context has to be searched firstly
	session_mutex_lock(jsr82_adp_cntx.mutex);
	server_session = btadp_jsr82_find_session_by_role_index(JSR82_SESSION_ROLE_SERVER, rsp->index);
	session_mutex_unlock(jsr82_adp_cntx.mutex);
	if (NULL == server_session)
	{
		OS_Report("[JSR82]fail to find server session context");
		goto exit;
	}

	if (rsp->result != JSR82_SESSION_CONNECT_ACCEPT)
	{
		server_session->active_conn --;
		goto exit;
	}
	
	session_mutex_lock(jsr82_adp_cntx.mutex);
	
	session = btadp_jsr82_find_session_by_id(rsp->sessionid);
	if (session == NULL)
	{
		session = btadp_jsr82_find_available_session();
		if (NULL != session)
		{
			btadp_jsr82_session_init(session);
			session->sessionid = rsp->sessionid;
			session->conn_state = BT_ADP_JSR82_STATE_ACTIVE;
		}
	}	
	session_mutex_unlock(jsr82_adp_cntx.mutex);
	
	if (NULL == session)
	{
		OS_Report("[JSR82] no available session context");
		goto exit;
	}

	btmtk_os_memcpy(session->bd_addr, rsp->bd_addr, 6);
	session->channel = rsp->channel;
	session->mtu = rsp->mtu;
	session->ps_type = rsp->ps_type;
	session->index = rsp->index;
	session->l2cap_id = rsp->l2cap_id;
	session->identify = rsp->identify;
	//this kind of session is not server session
	session->role = JSR82_SESSION_ROLE_CLIENT;
	session->conn_state = BT_ADP_JSR82_STATE_CONNECTED;

exit:

	if(session)
	{	
		btadp_jsr82_session_connect_rsp(session, rsp->result);
	}
	
	return;
}


void btadp_jsr82_disconnect_req(bt_jsr82_disconnect_req_struct *req)
{
	BT_ADP_JSR82_SESSION *session = NULL;
	BOOL result = TRUE;
	
	session = btadp_jsr82_find_session_by_role_index(JSR82_SESSION_ROLE_CLIENT, req->index);
	if (NULL == session ||
		session->l2cap_id != req->l2cap_id ||
		session->ps_type != req->ps_type)
	{
		OS_Report("[JSR82][ADP]btadp_jsr82_disconnect_req with invalid parameters");
		result = FALSE;
		goto exit;
	}

	btadp_jsr82_session_disconnect(session);

	
exit:
	if (FALSE == result)
	{
		bt_jsr82_disconnect_ind_struct *ptr = NULL;
    	ptr = (bt_jsr82_disconnect_ind_struct*) construct_local_para(sizeof(bt_jsr82_disconnect_ind_struct), TD_UL);
	    ptr->ps_type = req->ps_type;
	    ptr->index = req->index;
	    ptr->identify = req->identify;
	    ptr->l2cap_id = req->l2cap_id;
    	JSR82_sendMsg(MSG_ID_BT_JSR82_DISCONNECT_IND, MOD_MMI, (local_para_struct*) ptr, NULL);
	}
}


void btadp_jsr82_enable_service_req(bt_jsr82_enable_service_req_struct *req)
{
	BT_ADP_JSR82_SESSION * session = NULL;
	BOOL result = TRUE;

	OS_Report("[JSR82][ADP]btadp_jsr82_enable_service_req: sessionid[%8x]", req->sessionid);

	
	//attach to session context
	session_mutex_lock(jsr82_adp_cntx.mutex);
	session = btadp_jsr82_find_session_by_id(req->sessionid);
	if (session == NULL)
	{
		session = btadp_jsr82_find_available_session();
		if (session == NULL)
		{
			result = FALSE;
		}
		else
		{
			btadp_jsr82_session_init(session);
			session->conn_state = BT_ADP_JSR82_STATE_ACTIVE;
		}
	}	
	session_mutex_unlock(jsr82_adp_cntx.mutex);
	if (NULL == session)
	{
		OS_Report("[JSR82] no available session context");
		goto exit;
	}
	//Notes: when lower API is called, maybe callback will be executed first, so session context has to be inited firstly
	session->sessionid = req->sessionid;
	session->ps_type = req->ps_type;
	session->mtu = req->mtu;
	session->identify = req->identify;
	session->security_value = req->security_value;
	session->existing_psm_chnl_num = req->existing_psm_chnl_num;
	session->role = JSR82_SESSION_ROLE_SERVER;

	btadp_jsr82_session_enable_service(session);
	
exit:
	if (TRUE != result)
	{
		bt_jsr82_enable_service_cnf_struct *ptr = NULL;
		ptr =
        (bt_jsr82_enable_service_cnf_struct*) construct_local_para(sizeof(bt_jsr82_enable_service_cnf_struct), TD_UL);
	    ptr->result = JSR82_SESSION_REGISTRARION_FAILED;
	    ptr->channel = 0;
	    ptr->ps_type = req->ps_type;
	    ptr->index = 0;
	    ptr->identify = req->identify;
	    JSR82_sendMsg(MSG_ID_BT_JSR82_ENABLE_SERVICE_CNF, MOD_MMI, (local_para_struct*) ptr, NULL);
	}
}

void btadp_jsr82_disable_service_req(bt_jsr82_disable_service_req_struct *req)
{
	BT_ADP_JSR82_SESSION * session = NULL;
	BOOL result = TRUE;
	//disconnect fisrtly

	session = btadp_jsr82_find_session_by_role_index(JSR82_SESSION_ROLE_SERVER, req->index);
	if (NULL == session)
	{
		result = FALSE;
		goto exit;
	}

	btadp_jsr82_session_disable_service(session);		
	
exit:
	if (TRUE != result)
	{
		bt_jsr82_disable_service_cnf_struct *ptr;
    	ptr =
        (bt_jsr82_disable_service_cnf_struct*) construct_local_para(
                                                sizeof(bt_jsr82_disable_service_cnf_struct),
                                                TD_UL);
		if(session)
		{
	    	ptr->ps_type = session->ps_type;
		}
	    ptr->index = req->index;
	    ptr->identify = req->identify;
	    ptr->result = BT_JSR82_SESSION_DISABLED_FAILED;
   		JSR82_sendMsg(MSG_ID_BT_JSR82_DISABLE_SERVICE_CNF, MOD_MMI, (local_para_struct*) ptr, NULL);
	}
}

void btadp_jsr82_tx_ready_ind(bt_jsr82_tx_ready_ind_struct *ind)
{
	BT_ADP_JSR82_SESSION *session = NULL;

	session = btadp_jsr82_find_session_by_role_index(JSR82_SESSION_ROLE_CLIENT, ind->index);	
	if (session == NULL)
	{
		OS_Report("[JSR82] bt_jsr82_SessionApTxReadyInd: ne session context for index[%d]",
			ind->index);
	}	
	else
	{
		btadp_jsr82_session_send(session);
	}
}

void btadp_jsr82_send_data_req(bt_jsr82_tx_data_req_struct *req)
{
	BT_ADP_JSR82_SESSION *session = NULL;
	
	session = btadp_jsr82_find_session_by_role_index(JSR82_SESSION_ROLE_CLIENT, req->index);	
	if (session == NULL)
	{
		OS_Report("[JSR82] btadp_jsr82_send_data_req: no session context for index[%d]",
			req->index);
	}	
	else
	{
		btadp_jsr82_session_send(session);
	}
}


void btadp_jsr82_receive_data_ind(bt_jsr82_rx_ready_ind_struct *req)
{
	BT_ADP_JSR82_SESSION *session = NULL;
	
	session = btadp_jsr82_find_session_by_role_index(JSR82_SESSION_ROLE_CLIENT, req->index);	
	if (session == NULL)
	{
		OS_Report("[JSR82] btadp_jsr82_receive_data_ind: ne session context for index[%d]",
			req->index);
	}	
	else
	{
		btadp_jsr82_session_receive(session);
	}
}

void btadp_jsr82_reset_req(bt_jsr82_reset_req_struct *req)
{
	bt_jsr82_reset_cnf_struct *ptr = NULL;
	BT_ADP_JSR82_SESSION *session = NULL;
	int index = 0;

	BTAppSdpDbDeactivate();

	for(index = 0; index < BT_ADP_JSR82_MAX_SESSION_COUNT; index ++)
	{
		session = &jsr82_adp_cntx.session[index];
		if (BT_ADP_JSR82_STATE_IDLE == session->conn_state)
		{
			continue;
		}
		
		if (JSR82_SESSION_ROLE_CLIENT == session->role )
		{
			btadp_jsr82_session_disconnect(session);
		}
		else
		{
			btadp_jsr82_session_disable_service(session);
		}
	}

	ptr = (bt_jsr82_reset_cnf_struct*) construct_local_para(
                                                sizeof(bt_jsr82_reset_cnf_struct),
                                                TD_UL);
	ptr->result = JSR82_SESSION_COMMON_SUCCESS;
	JSR82_sendMsg(MSG_ID_BT_JSR82_RESET_CNF, MOD_MMI, (local_para_struct*) ptr, NULL);
}



/*****************************************************************************
 * FUNCTION
 *  btmtk_adp_jsr82_handle_message
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_jsr82_handle_message(ilm_struct *ilm_ptr)
{

    switch (ilm_ptr->msg_id)
    {
#if 0	// These cases are not used currently.
        case MSG_ID_BT_JSR_DEACTIVE_REQ:
            BTJSR82_DeInitAll();
            JSR82_sendMsg(MSG_ID_BT_JSR_DEACTIVE_CNF, MOD_MMI, (local_para_struct*) NULL, NULL);
            break;
        case MSG_ID_BT_JSR82_RESTROE_ACL_REQ:
            bt_jsr82_HandleSessionApDisconnectAllAcl();
            break;
        case MSG_ID_BT_JSR_PANIC:
        {
            /* Add this handler for JVM to send this message to trigger panic mechanism to test its error handling: 2007-1210 */
            bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_TRIGGER_PANIC_MECHANISM_BY_JVN_FOR_TESTING_ERROR_HANDLE);
            HCI_TransportReset();
            bma_panic();
        }
            break;
        case MSG_ID_BT_JSR82_ENABLE_SERVICE_REQ:
            btadp_jsr82_enable_service((bt_jsr82_enable_service_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_TURNON_SERVICE_REQ:
            bt_jsr82_HandleSessionApTurnOnService((bt_jsr82_turnon_service_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_TURNOFF_SERVICE_REQ:
            bt_jsr82_HandleSessionApTurnOffService((bt_jsr82_turnoff_service_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_CONNECT_REQ:
      //      bt_jsr82_HandleSessionApConnectReq((bt_jsr82_connect_req_struct*) ilm_ptr->local_para_ptr);
      		btadp_jsr82_connect((bt_jsr82_connect_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_CONNECT_RSP:
            bt_jsr82_HandleSessionApConnectRsp((bt_jsr82_connect_rsp_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_DISCONNECT_REQ:
            btadp_jsr82_disconnect((bt_jsr82_disconnect_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_TX_DATA_REQ:
            bt_jsr82_HandleSessionApTxReq((bt_jsr82_tx_data_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_DISABLE_SERVICE_REQ:
            bt_jsr82_HandleSessionApDisableService((bt_jsr82_disable_service_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_RX_DATA_RSP:
            bt_jsr82_HandleSessionApRxCfn((bt_jsr82_rx_data_rsp_struct*) ilm_ptr->local_para_ptr);
            break;
            /* Add by mtk01411:2007-0916 */
        case MSG_ID_BT_JSR82_SPP_GET_DATA_REQ:
            bt_jsr82_HandleSessionApGetSppDataReq((bt_jsr82_spp_get_data_req_struct*) ilm_ptr->local_para_ptr);
            break;
            /* Add by mtk01411: 2007-1119 */
        case MSG_ID_BT_JSR82_SET_ACL_SECURITY_REQ:
            bt_jsr82_HandleSetACLSecurityReq((bt_jsr82_set_acl_security_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_JSR82_PUT_BYTES_REQ:
        	jsr82_session_PutBytes((bt_jsr82_put_bytes_req_struct*) ilm_ptr->local_para_ptr);
        	break;
        case MSG_ID_BT_JSR82_GET_BYTES_REQ:
        	jsr82_session_GetBytes((bt_jsr82_get_bytes_req_struct*) ilm_ptr->local_para_ptr);
        	break;

        case MSG_ID_BT_JSR82_DATA_AVAILABLE_IND_RSP:
        	jsr82_session_DataAvailableIndRsp((bt_jsr82_data_available_ind_rsp_struct*) ilm_ptr->local_para_ptr);
        	break;

        case MSG_ID_BT_JSR82_ASSIGN_BUFFER_REQ:
        	jsr82_session_AssignDeliverBufferReq((bt_jsr82_assign_buffer_req_struct*) ilm_ptr->local_para_ptr);
        	break;
#endif

		case MSG_ID_BT_JSR82_ENABLE_SERVICE_REQ:
            btadp_jsr82_enable_service_req((bt_jsr82_enable_service_req_struct*) ilm_ptr->local_para_ptr);
            break;
			
		case MSG_ID_BT_JSR82_DISABLE_SERVICE_REQ:
			btadp_jsr82_disable_service_req((bt_jsr82_disable_service_req_struct*) ilm_ptr->local_para_ptr);
			break;
			
		case MSG_ID_BT_JSR82_CONNECT_REQ:
      		btadp_jsr82_connect_req((bt_jsr82_connect_req_struct*) ilm_ptr->local_para_ptr);
            break;
			
		case MSG_ID_BT_JSR82_CONNECT_RSP:
            btadp_jsr82_connect_rsp((bt_jsr82_connect_rsp_struct*) ilm_ptr->local_para_ptr);
            break;
			
		case MSG_ID_BT_JSR82_DISCONNECT_REQ:
            btadp_jsr82_disconnect_req((bt_jsr82_disconnect_req_struct*) ilm_ptr->local_para_ptr);
            break;
			
	    case MSG_ID_BT_JSR82_TX_READY_IND:
            btadp_jsr82_tx_ready_ind((bt_jsr82_tx_ready_ind_struct*) ilm_ptr->local_para_ptr);
            break;

		case MSG_ID_BT_JSR82_TX_DATA_REQ:
            btadp_jsr82_send_data_req((bt_jsr82_tx_data_req_struct *) ilm_ptr->local_para_ptr);
            break;
			
		case MSG_ID_BT_JSR82_RX_READY_IND:
			btadp_jsr82_receive_data_ind((bt_jsr82_rx_ready_ind_struct *)ilm_ptr->local_para_ptr);
			break;
			
		case MSG_ID_BT_JSR82_RESET_REQ:
			btadp_jsr82_reset_req((bt_jsr82_reset_req_struct *)ilm_ptr->local_para_ptr);
			break;
			
    }
}

#endif /* __BT_JSR82__ */
