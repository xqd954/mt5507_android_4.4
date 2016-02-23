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


#include "bt_common.h"
#include "jsr82_session.h"
#include "bt_adp_jsr82_event.h"
#include "bt_adp_jsr82.h"
#include "bt_adp_jsr82_session.h"
#include "bt_os_api.h"


/*****************************************************************************
 * FUNCTION
 *  JSR82_sendMsg
 * DESCRIPTION
 *  
 * PARAMETERS
 *  msg             [IN]        
 *  dstMod          [IN]        
 *  local_para      [IN]        
 *  peer_buff       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void JSR82_sendMsg(msg_type msg, module_type dstMod, local_para_struct *local_para, peer_buff_struct *peer_buff)
{
    //ilm_struct *ilmPtr;

    if ((0 > (int)dstMod) || (END_OF_MOD_ID <= dstMod))
    {
        return;
    }
	if (NULL != local_para)
	{
		BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
		//free_local_para(local_para);
	}
	else
	{
		BT_SendMessage(msg, dstMod, NULL, 0);
	}

}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionApEnableServiceCfn
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result          [IN]        
 *  ps_type         [IN]        
 *  channel         [IN]        
 *  index           [IN]        
 *  identify        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionApEnableServiceCfn(JSR82CallbackParms *Parms)
{
    bt_jsr82_enable_service_cnf_struct *ptr = NULL;
	BT_ADP_JSR82_SESSION * session = NULL;
	
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SESSIONAPENABLESERVICECFN);

	session = btadp_jsr82_find_session_by_identify(Parms->p.enable_service_cnf.identify);
	if (session == NULL)
	{
		goto error;
	}

	if (JSR82_SESSION_TURNOFF_SUCCESS == Parms->p.enable_service_cnf.result)
	{
		session->identify = Parms->p.enable_service_cnf.identify;
		session->index = Parms->p.enable_service_cnf.index;
		session->channel = Parms->p.enable_service_cnf.channel;

		//TODO: implement it in external
//		btadp_jsr82_channel_registered(session);
	}
	else
	{
		bt_session_destroy(session->sessionid);
		btadp_jsr82_session_deinit(session);
	}


    ptr =
        (bt_jsr82_enable_service_cnf_struct*) construct_local_para(sizeof(bt_jsr82_enable_service_cnf_struct), TD_UL);
    ptr->result = Parms->p.enable_service_cnf.result;
    ptr->channel = Parms->p.enable_service_cnf.channel;
    ptr->ps_type = Parms->p.enable_service_cnf.ps_type;
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SESSIONAPENABLESERVICECFN_PS_TYPEx02X, Parms->p.enable_service_cnf.ps_type);
    ptr->index = Parms->p.enable_service_cnf.index;
    ptr->identify = Parms->p.enable_service_cnf.identify;

    JSR82_sendMsg(MSG_ID_BT_JSR82_ENABLE_SERVICE_CNF, MOD_MMI, (local_para_struct*) ptr, NULL);

error:	
	if (session == NULL &&
		JSR82_SESSION_TURNOFF_SUCCESS == Parms->p.enable_service_cnf.result)
	{
		bt_jsr82_disable_service_req_struct req;
		req.identify = Parms->p.enable_service_cnf.identify;
		req.index = Parms->p.enable_service_cnf.index;
		bt_jsr82_HandleSessionApDisableService(&req);
	}
    return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionApTurnOnServiceCfn
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result          [IN]        
 *  ps_type         [IN]        
 *  con_id          [IN]        
 *  identify        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionApTurnOnServiceCfn(JSR82CallbackParms *Parms)
{
    bt_jsr82_turnon_service_cnf_struct *ptr;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SESSIONAPTURNONSERVICECFN);

    ptr =
        (bt_jsr82_turnon_service_cnf_struct*) construct_local_para(sizeof(bt_jsr82_turnon_service_cnf_struct), TD_UL);
    ptr->result = Parms->p.turn_on_service_cnf.result;
    ptr->index = Parms->p.turn_on_service_cnf.con_id;
    ptr->ps_type = Parms->p.turn_on_service_cnf.ps_type;
    ptr->identify = Parms->p.turn_on_service_cnf.identify;

    JSR82_sendMsg(MSG_ID_BT_JSR82_TURNON_SERVICE_CNF, MOD_MMI, (local_para_struct*) ptr, NULL);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionApTurnOffServiceCfn
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result          [IN]        
 *  ps_type         [IN]        
 *  con_id          [IN]        
 *  identify        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionApTurnOffServiceCfn(JSR82CallbackParms *Parms)
{
    bt_jsr82_turnoff_service_cnf_struct *ptr;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SESSIONAPTURNOFFSERVICECFN);

    ptr =
        (bt_jsr82_turnoff_service_cnf_struct*) construct_local_para(
                                                sizeof(bt_jsr82_turnoff_service_cnf_struct),
                                                TD_UL);
    ptr->result = Parms->p.turn_off_service_cnf.result;
    ptr->index = Parms->p.turn_off_service_cnf.con_id;
    ptr->ps_type = Parms->p.turn_off_service_cnf.ps_type;
    ptr->identify = Parms->p.turn_off_service_cnf.identify;

    JSR82_sendMsg(MSG_ID_BT_JSR82_TURNOFF_SERVICE_CNF, MOD_MMI, (local_para_struct*) ptr, NULL);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionApDisableServiceCfn
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ps_type         [IN]        
 *  con_id          [IN]        
 *  identify        [IN]        
 *  result          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionApDisableServiceCfn(JSR82CallbackParms *Parms)
{
    bt_jsr82_disable_service_cnf_struct *ptr;
	BT_ADP_JSR82_SESSION * session = NULL;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SESSIONAPDISABLESERVICECFN);


    ptr =
        (bt_jsr82_disable_service_cnf_struct*) construct_local_para(
                                                sizeof(bt_jsr82_disable_service_cnf_struct),
                                                TD_UL);
    ptr->ps_type = Parms->p.disable_service_cnf.ps_type;
    ptr->index = Parms->p.disable_service_cnf.con_id;
    ptr->identify = Parms->p.disable_service_cnf.identify;
    ptr->result = Parms->p.disable_service_cnf.result;
    JSR82_sendMsg(MSG_ID_BT_JSR82_DISABLE_SERVICE_CNF, MOD_MMI, (local_para_struct*) ptr, NULL);
	
	session = btadp_jsr82_find_session_by_role_index(JSR82_SESSION_ROLE_SERVER, 
													Parms->p.disable_service_cnf.con_id);
	if (session == NULL)
	{
		goto error;
	}

	//no session in server role
	//bt_session_destroy(session->sessionid);
	btadp_jsr82_session_deinit(session);
error:
    return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionApConnectInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bd_addr         [IN]        
 *  ps_type         [IN]        
 *  mtu             [IN]        
 *  channel         [IN]        
 *  index           [IN]        
 *  identify        [IN]        
 *  l2cap_id        [IN]        
 *  rsp_result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionApConnectInd(JSR82CallbackParms *Parms)
{
    bt_jsr82_connect_ind_struct *ptr;
	BT_ADP_JSR82_SESSION *session = NULL;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SESSIONAPCONNECTIND);

	session = btadp_jsr82_find_session_by_role_identify(JSR82_SESSION_ROLE_SERVER, Parms->p.connect_ind.identify);
	if (session == NULL)
	{		
		OS_Report("[JSR82] error: no session context for identify[%d]", Parms->p.connect_ind.identify);
		//No service is listening on this channel, so ignore it
		goto error;
	}

	//Notes:do not change the server connection state
//	session->conn_state = BT_ADP_JSR82_STATE_CONNECTED;

	session->active_conn ++;

	session->index = Parms->p.connect_ind.index;
	session->l2cap_id = Parms->p.connect_ind.l2cap_id;
	session->in_mtu = Parms->p.connect_ind.in_mtu;
	session->mtu = Parms->p.connect_ind.mtu;


    ptr = (bt_jsr82_connect_ind_struct*) construct_local_para(sizeof(bt_jsr82_connect_ind_struct), TD_UL);
    
    btmtk_os_memcpy((U8*) & ptr->bd_addr, Parms->p.connect_ind.bd_addr, 6);
    ptr->channel = Parms->p.connect_ind.channel;
    ptr->ps_type = Parms->p.connect_ind.ps_type;
    ptr->index = Parms->p.connect_ind.index;
    ptr->identify = Parms->p.connect_ind.identify;
    ptr->l2cap_id = Parms->p.connect_ind.l2cap_id;
    ptr->mtu = Parms->p.connect_ind.mtu;
    ptr->in_mtu = Parms->p.connect_ind.in_mtu;
    ptr->rsp_result = Parms->p.connect_ind.result;

    JSR82_sendMsg(MSG_ID_BT_JSR82_CONNECT_IND, MOD_MMI, (local_para_struct*) ptr, NULL);

error:
	if (session == NULL)
	{
		bt_jsr82_disconnect_req_struct req;
		req.identify = Parms->p.connect_ind.identify;
		req.index = Parms->p.connect_ind.index;
		req.l2cap_id = Parms->p.connect_ind.l2cap_id;
		req.ps_type = Parms->p.connect_ind.ps_type;
		bt_jsr82_HandleSessionApDisConnectReq(&req);
	}

    return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionApConnectCfn
 * DESCRIPTION
 *  Notify JBT: Jsr82 Session is connected for its corressponding request from JBT
 * PARAMETERS
 *  bd_addr         [IN]         
 *  ps_type         [IN]        
 *  mtu             [IN]        
 *  channel         [IN]        
 *  index           [IN]        
 *  identify        [IN]        
 *  l2cap_id        [IN]        
 *  result          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionApConnectCfn(JSR82CallbackParms *Parms)
{
    bt_jsr82_connect_cnf_struct *ptr;
	BT_ADP_JSR82_SESSION *session = NULL;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SESSIONAPCONNECTCFN);

	session = btadp_jsr82_find_session_by_identify(Parms->p.connect_cnf.identify);
	if (session == NULL)
	{		
		OS_Report("[JSR82] error: no session context for identify[%d]", Parms->p.connect_cnf.identify);
		//No service is listening on this channel, so ignore it
		goto error;
	}

	if (session->conn_state == BT_ADP_JSR82_STATE_CONNECTED)
	{
		//the channel has been connected
		OS_Report("[JSR82] warning:the channel has been connected");
		return;
	}

	session->conn_state = BT_ADP_JSR82_STATE_CONNECTED;
	session->index = Parms->p.connect_cnf.index;
	session->l2cap_id = Parms->p.connect_cnf.l2cap_id;
	session->in_mtu = Parms->p.connect_cnf.in_mtu;
	session->mtu = Parms->p.connect_cnf.mtu;
	session->channel = Parms->p.connect_cnf.channel;
	
	if (JSR82_SESSION_CONNECT_CLIENT_SUCCESS == Parms->p.connect_cnf.result)
	{
		btadp_jsr82_channel_connected(session, NULL, 0);
	}
	else
	{
		btadp_jsr82_session_disconnected(session);		
	}

	ptr = (bt_jsr82_connect_cnf_struct*) construct_local_para(sizeof(bt_jsr82_connect_cnf_struct), TD_UL);
    
    btmtk_os_memcpy((U8*) & ptr->bd_addr, Parms->p.connect_cnf.bd_addr, 6);
    ptr->channel = Parms->p.connect_cnf.channel;
    ptr->ps_type = Parms->p.connect_cnf.ps_type;
    /* Add index field by mtk01411: 2007-0911 */
    ptr->index = Parms->p.connect_cnf.index;
    ptr->identify = Parms->p.connect_cnf.identify;
    ptr->l2cap_id = Parms->p.connect_cnf.l2cap_id;
    ptr->mtu = Parms->p.connect_cnf.mtu;
    ptr->in_mtu = Parms->p.connect_cnf.in_mtu;
    ptr->result = Parms->p.connect_cnf.result;

    JSR82_sendMsg(MSG_ID_BT_JSR82_CONNECT_CNF, MOD_MMI, (local_para_struct*) ptr, NULL);

error:
	if (session == NULL)
	{
		bt_jsr82_disconnect_req_struct req;
		req.identify = Parms->p.connect_cnf.identify;
		req.index = Parms->p.connect_cnf.index;
		req.l2cap_id = Parms->p.connect_cnf.l2cap_id;
		req.ps_type = Parms->p.connect_cnf.ps_type;
		bt_jsr82_HandleSessionApDisConnectReq(&req);
	}
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionApCloseInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ps_type         [IN]        
 *  con_id          [IN]        
 *  identify        [IN]        
 *  l2cap_id        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionApCloseInd(JSR82CallbackParms *Parms)
{
    bt_jsr82_disconnect_ind_struct *ptr;
	BT_ADP_JSR82_SESSION *session = NULL;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SESSIONAPCLOSEINDx02X_x02X_x04X, Parms->p.disconnect_ind.con_id, Parms->p.disconnect_ind.identify, Parms->p.disconnect_ind.l2cap_id);

	ptr = (bt_jsr82_disconnect_ind_struct*) construct_local_para(sizeof(bt_jsr82_disconnect_ind_struct), TD_UL);
    ptr->ps_type = Parms->p.disconnect_ind.ps_type;
    ptr->index = Parms->p.disconnect_ind.con_id;
    ptr->identify = Parms->p.disconnect_ind.identify;
    ptr->l2cap_id = Parms->p.disconnect_ind.l2cap_id;
    JSR82_sendMsg(MSG_ID_BT_JSR82_DISCONNECT_IND, MOD_MMI, (local_para_struct*) ptr, NULL);

	//TODO: fix the issue that the identify is zero
	session = btadp_jsr82_find_session_by_role_index(JSR82_SESSION_ROLE_CLIENT, Parms->p.disconnect_ind.con_id);	
	if (session == NULL)
	{
		OS_Report("[JSR82] can not find session context for index %d", Parms->p.disconnect_ind.con_id);
		return;
	}
	else
	{		
		btadp_jsr82_session_disconnected(session);
	}
    return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionApTxCfn
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_inx     [IN]        
 *  l2cap_id        [IN]        
 *  result          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionApTxCfn(JSR82CallbackParms *Parms)
{
    bt_jsr82_tx_data_cfn_struct *ptr;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SESSIONAPTXCFN);

    ptr = (bt_jsr82_tx_data_cfn_struct*) construct_local_para(sizeof(bt_jsr82_tx_data_cfn_struct), TD_UL);
    ptr->index = Parms->p.tx_data_cnf.session_inx;
    ptr->result = Parms->p.tx_data_cnf.result;
    ptr->l2cap_id = Parms->p.tx_data_cnf.l2cap_id;

    JSR82_sendMsg(MSG_ID_BT_JSR82_TX_DATA_CFN, MOD_MMI, (local_para_struct*) ptr, NULL);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionApRxInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  con_id          [IN]        
 *  l2cap_id        [IN]        
 *  length          [IN]        
 *  data            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionApRxInd(JSR82CallbackParms *Parms)
{
    bt_jsr82_rx_data_ind_struct *ptr;
	BT_ADP_JSR82_SESSION *session = NULL;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SESSIONAPRXIND);

	session = btadp_jsr82_find_session_by_role_index(JSR82_SESSION_ROLE_CLIENT, Parms->p.rx_data_ind.con_id);
	if (session == NULL)
	{		
		OS_Report("[JSR82] bt_jsr82_SessionApRxInd: ne session context for index[%d]",
			Parms->p.rx_data_ind.con_id);
		
	}
	else
	{
		btadp_jsr82_session_receive(session);	
		//cnf for available indication is useless to sent to lower layer,
		//because the incoing data remains in lower cahce to wait for reading
	}
    return;
}


void bt_jsr82_SessionPutBytesCNF(JSR82CallbackParms *Parms)
{
    bt_jsr82_put_bytes_cnf_struct *ptr;
	OS_Report("[JSR82]bt_jsr82_SessionPutBytesCNF");

    ptr = (bt_jsr82_put_bytes_cnf_struct*) construct_local_para(sizeof(bt_jsr82_put_bytes_cnf_struct), TD_UL);
    ptr->index = Parms->p.put_bytes_cnf.index;
    ptr->length = Parms->p.put_bytes_cnf.length;
    ptr->l2cap_id = Parms->p.put_bytes_cnf.l2cap_id;
    ptr->ps_type = Parms->p.put_bytes_cnf.ps_type;
    JSR82_sendMsg(MSG_ID_BT_JSR82_PUT_BYTES_CNF, MOD_MMI, (local_para_struct*) ptr, NULL);
}


void bt_jsr82_SessionGetBytesCNF(JSR82CallbackParms *Parms)
{
    bt_jsr82_get_bytes_cnf_struct *ptr;

    ptr = (bt_jsr82_get_bytes_cnf_struct*) construct_local_para(sizeof(bt_jsr82_get_bytes_cnf_struct), TD_UL);
    ptr->index = Parms->p.get_bytes_cnf.index;
    ptr->length = Parms->p.get_bytes_cnf.length;
    ptr->l2cap_id = Parms->p.get_bytes_cnf.l2cap_id;
    ptr->ps_type = Parms->p.get_bytes_cnf.ps_type;
    ptr->buffAddr = Parms->p.get_bytes_cnf.buffAddr;
    JSR82_sendMsg(MSG_ID_BT_JSR82_GET_BYTES_CNF, MOD_MMI, (local_para_struct*) ptr, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionApRxReadyInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_inx     [IN]        
 *  l2cap_id        [IN]        
 *  length          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionApRxReadyInd(JSR82CallbackParms *Parms)
{
    bt_jsr82_rx_ready_ind_struct *ptr;
	BT_ADP_JSR82_SESSION *session = NULL;

    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SESSIONAPRXREADYIND);
	session = btadp_jsr82_find_session_by_role_index(JSR82_SESSION_ROLE_CLIENT, Parms->p.rx_ready_ind.session_inx);	
	if (session == NULL)
	{
		OS_Report("[JSR82] bt_jsr82_SessionApTxReadyInd: ne session context for index[%d]",
			Parms->p.rx_ready_ind.session_inx);
		goto error;
	}
	btadp_jsr82_session_receive(session);	
error:
	if (session == NULL)
	{
		bt_jsr82_disconnect_req_struct req;
		req.index = Parms->p.rx_ready_ind.session_inx;
		req.l2cap_id = Parms->p.rx_ready_ind.l2cap_id;
		req.ps_type = Parms->p.rx_ready_ind.ps_type;
		bt_jsr82_HandleSessionApDisConnectReq(&req);
	}
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionApTxReadyInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_inx     [IN]        
 *  l2cap_id        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionApTxReadyInd(JSR82CallbackParms *Parms)
{
    bt_jsr82_tx_ready_ind_struct *ptr;
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SESSIONAPTXREADYIND);
    ptr =
        (bt_jsr82_tx_ready_ind_struct*) construct_local_para(
                                                    sizeof(bt_jsr82_tx_ready_ind_struct),
                                                    TD_UL);
	
	ptr->index = Parms->p.tx_ready_ind.session_inx;
	ptr->l2cap_id = Parms->p.tx_ready_ind.l2cap_id;
	ptr->ps_type = Parms->p.tx_ready_ind.ps_type;
	JSR82_sendMsg(MSG_ID_BT_JSR82_TX_READY_IND, MOD_BT, (local_para_struct*) ptr, NULL); 
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SetACLSecurityCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result              [IN]        
 *  transaction_id      [IN]        
 *  addr                [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SetACLSecurityCnf(JSR82CallbackParms *Parms)
{
    bt_jsr82_set_acl_security_cnf_struct *ptr;

    ptr =
        (bt_jsr82_set_acl_security_cnf_struct*) construct_local_para(
                                                    sizeof(bt_jsr82_set_acl_security_cnf_struct),
                                                    TD_UL);
    ptr->result = Parms->p.set_acl_security_cnf.result;
    ptr->transaction_id = Parms->p.set_acl_security_cnf.transaction_id;
    btmtk_os_memcpy(ptr->bdAddr, Parms->p.set_acl_security_cnf.addr, 6);
    JSR82_sendMsg(MSG_ID_BT_JSR82_SET_ACL_SECURITY_CNF, MOD_MMI, (local_para_struct*) ptr, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  bt_jsr82_SessionApDisconnectAllAclCfn
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_jsr82_SessionApDisconnectAllAclCfn(void)
{
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SESSIONAPDISCONNECTALLACLCFN);
    JSR82_sendMsg(MSG_ID_BT_JSR82_RESTROE_ACL_CNF, MOD_MMI, NULL, NULL);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_sendData
 * DESCRIPTION
 *  This function is used to send SEND_DATA_REQ to BT stack
 *  This function is similar to function sppa_sendData()
 *  input_parm: session_index: It is used in btstack (For 3rd, it must use the id passed due to connection_cnf or conneciton_ind)
 * PARAMETERS
 *  session_inx     [IN]        
 *  l2cap_id        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jsr82_session_sendData(JSR82CallbackParms *Parms)
{
	bt_jsr82_tx_data_req_struct *msg=
		(bt_jsr82_tx_data_req_struct*) construct_local_para(sizeof(bt_jsr82_tx_data_req_struct), TD_UL);
		
	/*
	 * For Read and Write procedure, only session_inx is meaningful
	 * identify field can be removed !
	 */
	OS_Report("[JSR82]jsr82_session_sendData");
	msg->index = Parms->p.send_data_req.session_inx;
	msg->l2cap_id = Parms->p.send_data_req.l2cap_id;
	msg->length = 0;
	msg->data = NULL;
	JSR82_sendMsg(MSG_ID_BT_JSR82_TX_DATA_REQ, MOD_BT, (local_para_struct*) msg, NULL);    
}


/*****************************************************************************
 * FUNCTION
 *  jsr82_spp_getData
 * DESCRIPTION
 *  This function is used to send GET_DATA_REQ to BT stack
 *  This function is similar to function sppa_getData()
 *  input_parm: session_index: It is used in btstack (For 3rd, it must use the id passed due to connection_cnf or conneciton_ind)
 * PARAMETERS
 *  session_inx     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jsr82_spp_getData(JSR82CallbackParms *Parms)
{
	bt_jsr82_spp_get_data_req_struct *msg=
		(bt_jsr82_spp_get_data_req_struct*) construct_local_para(sizeof(bt_jsr82_spp_get_data_req_struct), TD_UL);
		
	/*
	 * Because this function is used to check credit internally, only session_inx for its corresponding SPP connection is enough 
	 * identify field can be removed
	 */
	msg->index = Parms->p.get_data_req.session_inx;
	msg->identify = 0;
	msg->l2cap_id = 0;
	msg->length = 0;
	JSR82_sendMsg(MSG_ID_BT_JSR82_SPP_GET_DATA_REQ, MOD_BT, (local_para_struct*) msg, NULL);    
}


void bt_jsr82_DataAvailableInd(JSR82CallbackParms *Parms)
{
	BT_ADP_JSR82_SESSION *session = NULL;
//	session = btadp_jsr82_find_session_by_l2cap(Parms->p.data_available_ind.l2cap_id);
	session = btadp_jsr82_find_session_by_role_index(JSR82_SESSION_ROLE_CLIENT, Parms->p.data_available_ind.index);	
	if (session == NULL)
	{
		OS_Report("[JSR82] bt_jsr82_DataAvailableInd: ne session context for index[%d]", Parms->p.data_available_ind.index);
		goto error;
	}
	btadp_jsr82_session_receive(session);	
	//cnf for available indication is useless to sent to lower layer,
	//because the incoing data remains in lower cahce to wait for reading
	
error:
	if (session == NULL)
	{
		bt_jsr82_disconnect_req_struct req;
		req.index = Parms->p.data_available_ind.index;
		req.l2cap_id = Parms->p.data_available_ind.l2cap_id;
		req.ps_type = Parms->p.data_available_ind.ps_type;
		bt_jsr82_HandleSessionApDisConnectReq(&req);
	}


}


void bt_jsr82_AssignBufferCnf(JSR82CallbackParms *Parms)
{
    bt_jsr82_assign_buffer_cnf_struct *ptr;

    ptr =
        (bt_jsr82_assign_buffer_cnf_struct*) construct_local_para(
                                                    sizeof(bt_jsr82_assign_buffer_cnf_struct),
                                                    TD_UL);
    ptr->index = Parms->p.assign_buffer_cnf.index;
    ptr->l2cap_id = Parms->p.assign_buffer_cnf.l2cap_id;
    JSR82_sendMsg(MSG_ID_BT_JSR82_ASSIGN_BUFFER_CNF, MOD_MMI, (local_para_struct*) ptr, NULL);
}


void btmtk_adp_jsr82_int_event_callback(JSR82CallbackParms *Parms)
{
    switch (Parms->event)        
    {
    	 /* JSR82 --> JBT */
        case JSR82_EVENT_ENABLE_SERVICE_CNF:
            bt_jsr82_SessionApEnableServiceCfn(Parms);
            break;
        case JSR82_EVENT_TURN_ON_SERVICE_CNF:
            bt_jsr82_SessionApTurnOnServiceCfn(Parms);
            break;
        case JSR82_EVENT_TURN_OFF_SERVICE_CNF:
            bt_jsr82_SessionApTurnOffServiceCfn(Parms);
            break;
        case JSR82_EVENT_DISABLE_SERVICE_CNF:
            bt_jsr82_SessionApDisableServiceCfn(Parms);
            break;
        case JSR82_EVENT_CONNECT_IND:
            bt_jsr82_SessionApConnectInd(Parms);
            break;
        case JSR82_EVENT_CONNECT_CNF:
            bt_jsr82_SessionApConnectCfn(Parms);
            break;
        case JSR82_EVENT_DISCONNECT_IND:
            bt_jsr82_SessionApCloseInd(Parms);
            break;
        case JSR82_EVENT_TX_DATA_CNF:
            bt_jsr82_SessionApTxCfn(Parms);
            break;
#if 0
        case JSR82_EVENT_RX_DATA_IND:
            bt_jsr82_SessionApRxInd(Parms);
            break;
#endif
	 case JSR82_EVENT_PUT_BYTES_CNF:
	 //	bt_jsr82_SessionPutBytesCNF(Parms);
	 	break;
	 case JSR82_EVENT_GET_BYTES_CNF:
	 //	bt_jsr82_SessionGetBytesCNF(Parms);
	 	break;
        case JSR82_EVENT_RX_READY_IND:
            bt_jsr82_SessionApRxReadyInd(Parms);
            break;
        case JSR82_EVENT_TX_READY_IND:
            bt_jsr82_SessionApTxReadyInd(Parms);
            break;
        case JSR82_EVENT_SET_ACL_SECURITY_CNF:
            bt_jsr82_SetACLSecurityCnf(Parms);
            break;
        case JSR82_EVENT_DISCONNECT_ALL_ACL_CNF:
            bt_jsr82_SessionApDisconnectAllAclCfn();
            break;
        case JSR82_EVENT_SEND_DATA_REQ:
            jsr82_session_sendData(Parms);
            break;
        case JSR82_EVENT_GET_DATA_REQ:
            jsr82_spp_getData(Parms);
            break;
        case JSR82_EVENT_DATA_AVAILABLE_IND:
            bt_jsr82_DataAvailableInd(Parms);
            break;
        case JSR82_EVENT_ASSIGN_BUFFER_CNF:
            bt_jsr82_AssignBufferCnf(Parms);
            break;

        default:
			OS_Report("[jsr82]invalid event: %d", Parms->event);
            Assert(0);
    }
}



