#include "bt_adp_jsr82.h"
#include "bt_adp_jsr82_session.h"
#include "bt_session.h"
#include "bt_os_api.h"
#include "bt_os_api.h"

#include "xatypes.h"
#include "bt_osal.h"

#include "jsr82_session.h"
#include "session_os.h"

extern BT_ADP_JSR82_CONTEXT jsr82_adp_cntx;
extern BOOL BTCoreVerifySysInitState(void);
extern BOOL BTCoreIsDeinitializing(void);
extern void JSR82_sendMsg(msg_type msg, module_type dstMod, local_para_struct *local_para, peer_buff_struct *peer_buff);


/*****************************************************************************/
/*							Utility functions							   					 */
/*****************************************************************************/

BT_ADP_JSR82_SESSION *btadp_jsr82_find_session_by_id(U32 sessionid)
{
	int index = 0;
	BT_ADP_JSR82_SESSION * session = NULL;
	BT_ADP_JSR82_SESSION * target = NULL;

	for(index = 0; index < BT_ADP_JSR82_MAX_SESSION_COUNT; index ++)
	{			
		session = &jsr82_adp_cntx.session[index];
		if (session->conn_state  != BT_ADP_JSR82_STATE_IDLE &&
			session->sessionid == sessionid)
		{
			target = session;
			break;
		}
	}
	return target;	
}


//find channel by rfcomm channel
BT_ADP_JSR82_SESSION *btadp_jsr82_find_session_by_l2cap(U16 l2cap)
{
	int index = 0;
	BT_ADP_JSR82_SESSION * session = NULL;
	BT_ADP_JSR82_SESSION * target = NULL;

	for(index = 0; index < BT_ADP_JSR82_MAX_SESSION_COUNT; index ++)
	{
		session = &jsr82_adp_cntx.session[index];
		if (session->conn_state  != BT_ADP_JSR82_STATE_IDLE &&
			session->l2cap_id == l2cap)
		{
			target = session;
			break;
		}
	}
	return target;	
}

BT_ADP_JSR82_SESSION *btadp_jsr82_find_session_by_role_index(BT_ADP_JSR82_SESSION_ROLE role, U8 session_index)
{
	int index = 0;
	BT_ADP_JSR82_SESSION * session = NULL;
	BT_ADP_JSR82_SESSION * target = NULL;

	for(index = 0; index < BT_ADP_JSR82_MAX_SESSION_COUNT; index ++)
	{
		session = &jsr82_adp_cntx.session[index];
		if (session->conn_state  != BT_ADP_JSR82_STATE_IDLE &&
			session->role == role &&
			session->index == session_index)
		{
			target = session;
			break;
		}
	}
	return target;	
}


//find channel by rfcomm channel
BT_ADP_JSR82_SESSION *btadp_jsr82_find_session_by_index(U8 session_index)
{
	int index = 0;
	BT_ADP_JSR82_SESSION * session = NULL;
	BT_ADP_JSR82_SESSION * target = NULL;

	for(index = 0; index < BT_ADP_JSR82_MAX_SESSION_COUNT; index ++)
	{
		session = &jsr82_adp_cntx.session[index];
		if (session->conn_state  != BT_ADP_JSR82_STATE_IDLE &&
			session->index == session_index)
		{
			target = session;
			break;
		}
	}
	return target;	
}
BT_ADP_JSR82_SESSION *btadp_jsr82_find_session_by_role_identify(BT_ADP_JSR82_SESSION_ROLE role, U32 identify)
{
	int index = 0;
	BT_ADP_JSR82_SESSION * session = NULL;
	BT_ADP_JSR82_SESSION * target = NULL;

	for(index = 0; index < BT_ADP_JSR82_MAX_SESSION_COUNT; index ++)
	{
		session = &jsr82_adp_cntx.session[index];
		if (session->conn_state  != BT_ADP_JSR82_STATE_IDLE  &&
			session->identify == identify &&
			session->role == role)
		{
			target = session;
			break;
		}
	}
	return target;	
}

BT_ADP_JSR82_SESSION *btadp_jsr82_find_session_by_identify(U32 identify)
{
	int index = 0;
	BT_ADP_JSR82_SESSION * session = NULL;
	BT_ADP_JSR82_SESSION * target = NULL;

	for(index = 0; index < BT_ADP_JSR82_MAX_SESSION_COUNT; index ++)
	{
		session = &jsr82_adp_cntx.session[index];
		if (session->conn_state  != BT_ADP_JSR82_STATE_IDLE  &&
			session->identify == identify)
		{
			target = session;
			break;
		}
	}
	return target;	
}

BT_ADP_JSR82_SESSION *btadp_jsr82_find_available_session()
{
	int index = 0;
	BT_ADP_JSR82_SESSION * session = NULL;
	BT_ADP_JSR82_SESSION * target = NULL;

	for(index = 0; index < BT_ADP_JSR82_MAX_SESSION_COUNT; index ++)
	{
		session = &jsr82_adp_cntx.session[index];
		if (session->conn_state  == BT_ADP_JSR82_STATE_IDLE)
		{
			target = session;
			break;
		}
	}
	return target;	
}

kal_uint8 convert_session_type(kal_uint8 ps_type)
{
	switch(ps_type)
	{
	case JSR82_SESSION_PS_RFCOMM:
		return JBT_SESSION_TYPE_SPP;

	case JSR82_SESSION_PS_L2CAP:
		return JBT_SESSION_TYPE_L2CAP;

	default:
		return 0xFF;
	}
}


BOOL btadp_jsr82_session_task_ongoing(BT_ADP_JSR82_SESSION *session, BT_JSR82_TASK_CODE code)
{
	ListEntry *header = NULL;
	ListEntry *next = NULL;
	BT_Jsr82_Task *task = NULL; 
	BOOL bRet = FALSE;
	ASSERT(session);
	
	session_mutex_lock(session->mutex);
	header = &session->pending_task;
	next = &session->pending_task;	
	if(IsListEmpty(header))	
	{
		goto exit;
	}
	do {
		next = GetNextNode(next);
		task = (BT_Jsr82_Task *)next;
		if (task->code == code)	
		{	
			bRet = TRUE;	
			break;
		}
	} while (next != header);	
exit:
	session_mutex_unlock(session->mutex);
	return bRet;

}


BOOL btadp_jsr82_session_add_task(BT_ADP_JSR82_SESSION *session, BT_JSR82_TASK_CODE code, void *context)
{
	BT_Jsr82_Task *task = NULL; 

	if (NULL == session ||
		BT_ADP_JSR82_STATE_IDLE == session->conn_state)
	{
		return FALSE;
	}
	if (session->pending_task_num == 0xFF)
	{
		OS_Report("[JSR82] task list is full");
		return FALSE;
	}

	task = (BT_Jsr82_Task *)malloc (sizeof(BT_Jsr82_Task));
	if (NULL == task)
	{
		return FALSE;
	}
	memset(task, 0x0, sizeof(BT_Jsr82_Task));
	task->code = code;
	task->context = context;
	session_mutex_lock(session->mutex);
	InitializeListEntry(&task->node);
	InsertTailList(&session->pending_task, &task->node);
	session->pending_task_num ++;
	session_mutex_unlock(session->mutex);
	return TRUE;
}

void btadp_jsr82_session_process_task(BT_ADP_JSR82_SESSION *session)
{
	BT_Jsr82_Task *task = NULL; 
	U8 original_task_num = session->pending_task_num;

	if (NULL == session ||
		BT_ADP_JSR82_STATE_IDLE == session->conn_state)
	{
		return;
	}

	session_mutex_lock(session->mutex);
	while ((original_task_num > 0) &&
		FALSE == IsListEmpty(&session->pending_task))
	{		
		task = (BT_Jsr82_Task *)RemoveHeadList(&session->pending_task);

		//release lock and permit to process pending task
		session_mutex_unlock(session->mutex);
		switch (task->code)
		{
		case JSR82_TASK_CHANNEL_CREATED:
			btadp_jsr82_channel_connected(session, NULL, 0);
			break;	

		case JSR82_TASK_RECEIVE:
			btadp_jsr82_session_receive(session);
			break;

		default:
			OS_Report("[JSR82][ADP]unexpected event %d", task->code);
		}	

		//free resource
		free(task);
		
		session_mutex_lock(session->mutex);
		
		//check the list node number with node record 
		original_task_num --;
	}	
	session_mutex_unlock(session->mutex);
}


void btadp_remove_all_tasks(BT_ADP_JSR82_SESSION *session)
{
	BT_Jsr82_Task *task = NULL; 
	
	if (NULL == session ||
		BT_ADP_JSR82_STATE_IDLE == session->conn_state)
	{
		return;
	}

	session_mutex_lock(session->mutex);
	while (FALSE == IsListEmpty(&session->pending_task))
	{		
		task = (BT_Jsr82_Task *)RemoveHeadList(&session->pending_task);
		free(task);
	}	
	session_mutex_unlock(session->mutex);
}


/*****************************************************************************/
/*							Interface for lower layer to access							    */
/*****************************************************************************/

void btadp_jsr82_session_enable_service(BT_ADP_JSR82_SESSION *session)
{
	bt_jsr82_enable_service_req_struct req;

	ASSERT(session);

	memset(&req, 0x0, sizeof(bt_jsr82_enable_service_req_struct));
	
	req.channel_context = NULL;
	req.identify = session->identify;
	req.security_value = session->security_value;
	req.ps_type = session->ps_type;
	req.mtu = session->mtu;
	req.existing_psm_chnl_num = session->existing_psm_chnl_num;
	bt_jsr82_HandleSessionApEnableService(&req);
}

void btadp_jsr82_session_disable_service(BT_ADP_JSR82_SESSION *session)
{
	bt_jsr82_disable_service_req_struct req;

	ASSERT(session);
	memset(&req, 0x0, sizeof(bt_jsr82_disable_service_req_struct));

	if(session->active_conn > 0)
	{
		//set a flag, when all active channels are disconnected, disable service
		session->conn_state = BT_ADP_JSR82_STATE_ZOMBIA;
		return;
	}
	
	req.identify = session->identify;
	req.index = session->index;
	bt_jsr82_HandleSessionApDisableService(&req);
}

void btadp_jsr82_session_connect(BT_ADP_JSR82_SESSION *session)
{
	bt_jsr82_connect_req_struct req;
	ASSERT(session);	
	
	memset(&req, 0x0, sizeof(bt_jsr82_connect_req_struct));
	
	session->conn_state = BT_ADP_JSR82_STATE_CONNECTING;
	btmtk_os_memcpy(req.bd_addr, session->bd_addr, 6);
	btmtk_os_memcpy(req.uuid_128, session->service, 16);
	req.channel = session->channel;
	req.channel_context = NULL;
	req.identify = session->identify;
	req.mtu = session->mtu;
	req.ps_type = session->ps_type;
	req.security_value = session->security_value;
	bt_jsr82_HandleSessionApConnectReq(&req);
}

void btadp_jsr82_session_connect_rsp(BT_ADP_JSR82_SESSION *session, kal_uint8 result)
{
	bt_jsr82_connect_rsp_struct rsp;

	memset(&rsp, 0x0, sizeof(bt_jsr82_connect_rsp_struct));
	btmtk_os_memcpy(rsp.bd_addr, session->bd_addr, 6);
	rsp.channel = session->channel;
	rsp.identify = session->identify;
	rsp.index = session->index;
	rsp.l2cap_id = session->l2cap_id;
	rsp.mtu = session->mtu;
	rsp.ps_type = session->ps_type;
	rsp.result = result;
	bt_jsr82_HandleSessionApConnectRsp(&rsp);		
}


void btadp_jsr82_session_turn_on_service(BT_ADP_JSR82_SESSION *session)
{
	bt_jsr82_turnon_service_req_struct req;
	ASSERT(session);

	memset(&req, 0x0, sizeof(bt_jsr82_turnon_service_req_struct));

	req.identify = session->identify;
	req.index = session->index;
	req.ps_type = session->ps_type;
	bt_jsr82_HandleSessionApTurnOnService(&req);	
}

void btadp_jsr82_session_turn_off_service(BT_ADP_JSR82_SESSION *session)
{
	bt_jsr82_turnoff_service_req_struct req;
	ASSERT(session);

//	session->conn_state = BT_ADP_JSR82_STATE_ZOMBIA;
	
	memset(&req, 0x0, sizeof(bt_jsr82_turnoff_service_req_struct));
	
	req.identify = session->identify;
	req.index = session->index;
	req.ps_type = session->ps_type;	
	bt_jsr82_HandleSessionApTurnOffService(&req);	
}

kal_int16 btadp_jsr82_session_send_data(BT_ADP_JSR82_SESSION *session)
{
	bt_jsr82_put_bytes_req_struct req;
	kal_int16 sentLen = 0;
	memset(&req, 0x0, sizeof(bt_jsr82_put_bytes_req_struct));
	req.index = session->index;
	req.l2cap_id = session->l2cap_id;
	req.ps_type = session->ps_type;
	req.buffAddr = session->txbuffer;
	req.length = session->txlength;

	sentLen = jsr82_session_PutBytes(&req);
	if (sentLen < session->txlength)
	{
	OS_Report("[JSR82]send data imcompletely total[%d], actual[%d]", session->txlength, sentLen);
	}
	return sentLen;
}

kal_int16 btadp_jsr82_session_receive_data(BT_ADP_JSR82_SESSION *session)
{
	bt_jsr82_get_bytes_req_struct req;
	kal_int16 length = 0;
	
	memset(&req, 0x0, sizeof(bt_jsr82_get_bytes_req_struct));
	req.index = session->index;
	req.l2cap_id = session->l2cap_id;
	req.ps_type = session->ps_type;
	req.buffAddr = session->rxbuffer;
	req.length = BT_ADP_JSR82_MAX_BUFFER_SIZE;
	length = jsr82_session_GetBytes(&req);

	if (length < 0 ||
		length > BT_ADP_JSR82_MAX_BUFFER_SIZE)
	{
		OS_Report("[JSR82]receive data with wrong size[%d]", session->rxlength);
		length = 0;
	}
	session->rxlength = length;
	return length;
}


/*****************************************************************************/
/*							Interface for upper layer or self to access							    */
/*****************************************************************************/
void btadp_jsr82_session_disconnect_timeout(EvmTimer *Timer)
{
	BT_ADP_JSR82_SESSION *session = Timer->context;
	bt_jsr82_disconnect_req_struct req;
	ASSERT(session);

	memset(&req, 0x0, sizeof(bt_jsr82_disconnect_req_struct));
	session->conn_state = BT_ADP_JSR82_STATE_DISCONNECTING;
	req.identify = session->identify;
	req.index = session->index;
	req.l2cap_id = session->l2cap_id;
	req.ps_type = session->ps_type;	
	bt_jsr82_HandleSessionApDisConnectReq(&req);
}
void btadp_jsr82_session_notify_to_send(BT_ADP_JSR82_SESSION *session)
{
	bt_jsr82_tx_data_req_struct *msg=
		(bt_jsr82_tx_data_req_struct*) construct_local_para(sizeof(bt_jsr82_tx_data_req_struct), TD_UL);

	msg->index = session->index;
	msg->l2cap_id = session->l2cap_id;
	msg->data = NULL;
	msg->length = 0;
	JSR82_sendMsg(MSG_ID_BT_JSR82_TX_DATA_REQ, MOD_BT, (local_para_struct*) msg, NULL);  
}
void btadp_jsr82_session_notify_to_receive(BT_ADP_JSR82_SESSION *session)
{	
    bt_jsr82_rx_ready_ind_struct *msg=
		(bt_jsr82_rx_ready_ind_struct*) construct_local_para(sizeof(bt_jsr82_rx_ready_ind_struct), TD_UL);

	msg->index = session->index;
	msg->l2cap_id = session->l2cap_id;
	msg->ps_type = session->ps_type;
	JSR82_sendMsg(MSG_ID_BT_JSR82_RX_READY_IND, MOD_BT, (local_para_struct*) msg, NULL);  
}

void btadp_jsr82_session_notify_to_disconnect(BT_ADP_JSR82_SESSION *session)
{
	bt_jsr82_disconnect_req_struct *ptr = NULL;
	ptr = (bt_jsr82_disconnect_req_struct*) construct_local_para(sizeof(bt_jsr82_disconnect_req_struct), TD_UL);
	ptr->ps_type = session->ps_type;
	ptr->index = session->index;
	ptr->identify = session->identify;
	ptr->l2cap_id = session->l2cap_id;
	JSR82_sendMsg(MSG_ID_BT_JSR82_DISCONNECT_REQ, MOD_BT, (local_para_struct*) ptr, NULL); 
}


/*****************************************************************************/
/*							Interface for JSR82 session module							    */
/*****************************************************************************/
//send_data_to_lower
void btadp_jsr82_session_send(BT_ADP_JSR82_SESSION *session)
{
	int free_space_size = 0;
	int size = 0;
	int len = 0;
	kal_int16 sentLen = 0;
	
	ASSERT(session);
	OS_Report("[JSR82]btadp_jsr82_session_send");

	if (session->conn_state != BT_ADP_JSR82_STATE_CONNECTED)
	{
		OS_Report("[JSR82]SR_E_INVALID_PARAM");	
		return;
	}

	//this should not happen
	if (BT_JSR82_SESSION_STATE_ATTACHED != session->attach_state)
	{
		if (FALSE == btadp_jsr82_session_add_task(session, JSR82_TASK_SEND, NULL))
		{
			OS_Report("[JSR82]fail to add pending task");	
		}
		goto exit;
	}

	session_mutex_lock(session->mutex);

	free_space_size = jsr82_session_BufFreeSpace(session->ps_type, 
													session->index,
													session->l2cap_id,
													JBT_SESSION_TX_BUF_TYPE);
	if(free_space_size <= 0)
	{
		OS_Report("[JSR82]no free space in TX buffer: %d", free_space_size);
		goto exit;
	}
	
	len = bt_session_read(session->sessionid, session->txbuffer, free_space_size);	
	if (len <= 0)
	{
		OS_Report("[JSR82]read fail %d", len);	
		goto exit;
	}

	session->txlength = len;
	sentLen = btadp_jsr82_session_send_data(session);
	if (sentLen < len)
	{
		OS_Report("[JSR82][warning]only partial data is sent to lower layer");
	}

	//TODO: store remained data to buffer
	session->txlength = sentLen;
	
exit:
	session_mutex_unlock(session->mutex);
	return;
}
//read_data_from_lower
void btadp_jsr82_session_receive(BT_ADP_JSR82_SESSION *session)
{
	kal_int16 		length = 0;
	SESSION_RESULT	sret = SR_NOERROR;

	ASSERT(session);

	OS_Report("[JSR82]btadp_jsr82_session_receive");

	if (session->conn_state != BT_ADP_JSR82_STATE_CONNECTED )
	{
		OS_Report("[JSR82]SR_E_INVALID_PARAM");	
		sret = SR_E_INVALID_PARAM;
		goto exit;
	}

	if (BT_JSR82_SESSION_STATE_ATTACHED != session->attach_state)
	{
		if (FALSE == btadp_jsr82_session_task_ongoing(session, JSR82_TASK_RECEIVE))
		{
			if (FALSE == btadp_jsr82_session_add_task(session, JSR82_TASK_RECEIVE, NULL))
			{
				OS_Report("[JSR82]fail to add pending task");	
			}
		}
		goto exit;
	}
	
	while(TRUE)
	{
		//flush the pending packet firstly		
		if (0 == session->rxlength)
		{
			if(btadp_jsr82_session_receive_data(session) <= 0)
			{
				break;
			}
		}

		/*flush to upper layer*/
		length = bt_session_write(session->sessionid, session->rxbuffer, session->rxlength);

		if (length < 0)
		{
			sret = SR_E_NO_CONNECTION;
			break;
		}

		if (length < session->rxlength && 
			length > 0)
		{
			//save the remaining data
			U8 temp[BT_ADP_JSR82_MAX_BUFFER_SIZE];
			memcpy(temp, session->rxbuffer + length, (session->rxlength - length));
			memcpy( session->rxbuffer, temp, (session->rxlength - length));
			session->rxlength = (session->rxlength - length);
		}
		else 
		{
			session->rxlength = 0; 	
		}		
	}


exit:
	
	if (sret != SR_NOERROR && session && (session->conn_state == BT_ADP_JSR82_STATE_CONNECTED))
	{
		bt_session_destroy(session->sessionid);
	}
	return;
}
void btadp_jsr82_session_disconnect(BT_ADP_JSR82_SESSION *session)
{
	SESSION_RESULT	sret = SR_NOERROR;

	ASSERT(session);

	OS_Report("[JSR82]btadp_jsr82_session_disconnect");

	if (session->conn_state != BT_ADP_JSR82_STATE_CONNECTED )
	{
		OS_Report("[JSR82]SR_E_INVALID_PARAM");	
		sret = SR_E_INVALID_PARAM;
		goto exit;
	}
	session->attach_state == BT_JSR82_SESSION_STATE_DETACHED;

	if (BT_ADP_JSR82_STATE_DISCONNECTING == session->conn_state)
	{
		OS_Report("[JSR82]channel has already been being disconnecting");
		goto exit;
	}

	//TODO: these code should be remove to btcore layer
	if (TRUE == jsr82_session_DataRemainInBuff(session->ps_type, 
													session->index,
													session->l2cap_id))
	{
		EVM_ResetTimer(&(session->timer));
		session->timer.func = btadp_jsr82_session_disconnect_timeout;
		session->timer.context = session;
		EVM_StartTimer(&(session->timer), 1000);
	}
	else
	{
		session->timer.context = session;
		btadp_jsr82_session_disconnect_timeout(&(session->timer));
	}
exit:
	bt_session_destroy(session->sessionid);

}



int btadp_jsr82_session_init(BT_ADP_JSR82_SESSION *session)
{
	SESSION_RESULT sret = SR_NOERROR;
	ASSERT(session);

	memset(session, 0x0, sizeof(BT_ADP_JSR82_SESSION));
	session_mutex_create(&session->mutex);	
	InitializeListHead(&session->pending_task);
	
	return 0;
}

void btadp_jsr82_session_deinit(BT_ADP_JSR82_SESSION *session)
{	
	OS_Report("[JSR82] btadp_jsr82_session_deinit");
	ASSERT(session);
	if (BT_ADP_JSR82_STATE_IDLE == session->conn_state)
	{
		return;
	}

	btadp_remove_all_tasks(session);
	
	if (session->mutex)
	{
		session_mutex_destroy(session->mutex);
		session->mutex = NULL;
	}

	EVM_CancelTimer(&session->timer);

	//TODO:disconnect first if connection exist		
	session->conn_state = BT_ADP_JSR82_STATE_IDLE;
	session->attach_state = BT_JSR82_SESSION_STATE_DETACHED;
	
}

void btadp_jsr82_channel_connected(BT_ADP_JSR82_SESSION *session, void *ctl, int size)
{
	int length; 
	connection_signal cs;

	ASSERT(session);
	OS_Report("[JSR82]btadp_jsr82_channel_connected :id[%4x], conn_id[%d], identify[%d]",
				session->sessionid, session->index, session->identify);

	if (BT_JSR82_SESSION_STATE_ATTACHED != session->attach_state)
	{
		if (FALSE == btadp_jsr82_session_task_ongoing(session, JSR82_TASK_CHANNEL_CREATED))
		{
			if (FALSE == btadp_jsr82_session_add_task(session, JSR82_TASK_CHANNEL_CREATED, NULL))
			{
				OS_Report("[JSR82]fail to add pending task");	
			}
		}
		return;
	}
	
	//send signal to external layer
    length = bt_session_write(session->sessionid, &session->channel, sizeof(int));

	if (length < (int)sizeof(int))
	{
		OS_Report("[JSR82] send incomplete: %d, id[%8x]", length, session->sessionid);		
	}

    cs.size = sizeof(cs);
	memcpy(cs.bd_addr.address, session->bd_addr, 6);
    cs.channel = session->channel;
    cs.status = 0; //0 means success

	if (ctl)
	{
    	length = bt_session_write_control(session->sessionid,
									ctl,
									size,
									&cs, 
									sizeof(connection_signal));
		
	}
	else
	{
		length = bt_session_write(session->sessionid,
									&cs, 
									sizeof(connection_signal));
	}

	if (length < (int)sizeof(connection_signal))
	{
		OS_Report("[JSR82] send incomplete actual(%d), required(%d)", length, sizeof(connection_signal));
	}
	
	session->conn_state = BT_ADP_JSR82_STATE_CONNECTED;

	//at current, multi connection is not support in rfcomm and l2cap
	//so disableservice
	//TODO: upper layer will close fd if service is deregistered, 
	//so more patience to be need to process this case 
	
//	btadp_jsr82_session_disable_service(session);
	
}




void btadp_jsr82_server_disconnected(BT_ADP_JSR82_SESSION *session)
{
	//for server, default disable service
	if (session->conn_state == BT_ADP_JSR82_STATE_ZOMBIA)
	{
		btadp_jsr82_session_disable_service(session);
	}
}
void btadp_jsr82_session_disconnected(BT_ADP_JSR82_SESSION *session)
{
	int length = 0;
	BOOL closesession = FALSE;
	BT_ADP_JSR82_SESSION *server_session = NULL;
	ASSERT(session);

	OS_Report("[JSR82]btadp_jsr82_session_disconnected :id[%4x], conn_id[%d], identify[%d]",
				session->sessionid, session->index, session->identify);

	session->conn_state = BT_ADP_JSR82_STATE_DISCONNECTING;	

	if(!BTCoreVerifySysInitState() || BTCoreIsDeinitializing())
	{
		closesession = TRUE;
		goto exit;
	}
	server_session = btadp_jsr82_find_session_by_role_index(JSR82_SESSION_ROLE_SERVER, session->index);
	if(server_session)
	{
		server_session->active_conn--;
		btadp_jsr82_server_disconnected(server_session);
	}
	else
	{
		OS_Report("[JSR]no matching for this session:index[%d]");
	}
	
exit:
	bt_session_destroy(session->sessionid);
	btadp_jsr82_session_deinit(session);
}


void bt_session_attached(U32 id)
{
	BT_ADP_JSR82_SESSION *session = NULL;
	
	OS_Report("[JSR82]bt_session_attached: id[%8x]",id);

	//todo: add mutex protection
	
	session_mutex_lock(jsr82_adp_cntx.mutex);
	session = btadp_jsr82_find_session_by_id(id);
	if (NULL == session)
	{	
		session = btadp_jsr82_find_available_session();
		if (NULL != session)
		{
			btadp_jsr82_session_init(session);			
			session->sessionid = id;
			session->conn_state = BT_ADP_JSR82_STATE_ACTIVE;
		}
		else
		{
			OS_Report("[JSR82] No resource");			
		}
	}	
	else
	{
		//session attached
	}
	session_mutex_unlock(jsr82_adp_cntx.mutex);

	if (NULL == session)
	{
		bt_session_destroy(id);
	}
	session->attach_state = BT_JSR82_SESSION_STATE_ATTACHED;

	btadp_jsr82_session_process_task(session);
}

void bt_session_detached(U32 id)
{
	BT_ADP_JSR82_SESSION *session = NULL;
	
	OS_Report("[JSR82]bt_session_detached: session id[%8x]",id);
	session = btadp_jsr82_find_session_by_id(id);
	if (NULL == session )
	{
		OS_Report("[JSR82]no session is found");
	}
	else
	{
		btadp_jsr82_session_notify_to_disconnect(session);
	}
}

void bt_session_upper_data_incoming(U32 id)
{
	BT_ADP_JSR82_SESSION *session = NULL;

	OS_Report("[JSR82]bt_session_upper_data_incoming: session id[%8x]",id);
	session = btadp_jsr82_find_session_by_id(id);
	if (NULL == session)
	{
		OS_Report("[JSR82]no session is found");
	}
	else
	{
		btadp_jsr82_session_notify_to_send(session);
	}
}

void bt_session_upper_buffer_writable(U32 id)
{
	BT_ADP_JSR82_SESSION *session = NULL;
	
	OS_Report("[JSR82]bt_session_upper_buffer_writable: id[%8x]",id);
	session = btadp_jsr82_find_session_by_id(id);
	if (NULL != session)
	{	
		btadp_jsr82_session_notify_to_receive(session);
	}
	else
	{		
		OS_Report("[JSR82]no session contenxt is found");	
	}
}

SESSION_RESULT jsr82_session_monitor(Session_Event_PARM *parm)
{
	if (parm == NULL || parm->event == SESSION_EVENT_INVALID)
	{
		return SR_E_INVALID_PARAM;
	}
	
	if (parm->event & SESSION_EVENT_CREATED)
	{
		bt_session_attached(parm->index);
	}
	if (parm->event & SESSION_EVENT_DATA_INCOMING)
	{
		bt_session_upper_data_incoming(parm->index);		
	}
	if (parm->event & SESSION_EVENT_BUFFER_WRITABLE)
	{
		bt_session_upper_buffer_writable(parm->index);		
	}
	if (parm->event & SESSION_EVENT_DESTROYED)
	{
		bt_session_detached(parm->index);		
	}

	return SR_NOERROR;		
}




