#ifdef __BT_HDP_PROFILE__

#include "bt_session.h"
#include "session_api.h"
#include "bt_adp_hdp_util.h"
#include "btutils.h"
extern void hdp_send_msg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff);
//lower --> upper layer
//TODO: improve it
int hdp_channel_lower_layer_incoming(hdp_adp_channel_struct *channel, void *data, U32 size)
{
	int length = 0;
	session_mutex_lock(channel->mutex);
	if (channel->sessionId != 0)
	{
		if (channel->rxlen > 0)
		{
			length += bt_session_write(channel->sessionId, channel->rxbuff, channel->rxlen);	
			channel->rxlen = 0;
		}	
		if(size > 0 &&
			NULL != data)
		{
			length += bt_session_write(channel->sessionId, data, size);	
		}
	}
	else
	{
		//queue data
		memcpy(channel->rxbuff + channel->rxlen, data, size);
		channel->rxlen += size;
	}
	session_mutex_unlock(channel->mutex);
	return length;
}

void hdp_channel_detach(hdp_adp_channel_struct *channel)
{
	if (channel)
	{
		bt_session_destroy(channel->sessionId);
		channel->sessionId = 0;
	}
}



void hdp_channel_detached(U32 id)
{
	hdp_adp_channel_struct *channel = NULL;
	bt_hdp_disconnect_req_struct *req = NULL;
	
	OS_Report("[HDP]hdp_session_detached: id[%8x]",id);

	channel = hdp_util_find_channel_by_session_id(id);
	if(NULL == channel)
	{
		OS_Report("[HDP]no channel match this id");
		return;
	}
	req = (bt_hdp_disconnect_req_struct *) construct_local_para(sizeof(bt_hdp_disconnect_req_struct), TD_UL | TD_RESET);
	if (req == NULL)
	{
		OS_Report("[HDP]fail to allocate memory");
		goto exit;
	}
	memset(req, 0x0, sizeof(bt_hdp_disconnect_req_struct));
	memcpy(&req->bdaddr, &channel->bdaddr, sizeof(bt_addr_struct));
	req->index = channel->index;
	req->mdlId = channel->mdlId;	
	hdp_send_msg(MSG_ID_BT_HDP_DISCONNECT_REQ, MOD_BT, BT_APP_SAP, (local_para_struct *)req, NULL);
exit:
	return;
}

void hdp_channel_upper_data_incoming(U32 id)
{
	hdp_adp_channel_struct *channel = NULL;
	U8						buffer[1024];
	int						length = 0;
	BD_ADDR					bdaddr;
	
	bt_hdp_send_data_req_struct *req = NULL;
	

	OS_Report("[HDP]hdp_session_upper_data_incoming: session id[%8x]",id);
	channel = hdp_util_find_channel_by_session_id(id);
	if (NULL == channel)
	{
		OS_Report("[HDP]no session is found");
		goto exit;
	}
	req = (bt_hdp_send_data_req_struct *) construct_local_para(sizeof(bt_hdp_send_data_req_struct), TD_UL | TD_RESET);
	if (req == NULL)
	{
		OS_Report("[HDP]fail to allocate memory");
		goto exit;
	}
	memset(req, 0x0, sizeof(bt_hdp_send_data_req_struct));
	memcpy(&req->bdaddr, &channel->bdaddr, sizeof(bt_addr_struct));
	req->index = channel->index;
	req->mdlId = channel->mdlId;	
	hdp_send_msg(MSG_ID_BT_HDP_SEND_DATA_REQ, MOD_BT, BT_APP_SAP, (local_para_struct *)req, NULL);
	
exit:
	return;
}

void hdp_channel_upper_buffer_writable(U32 id)
{
	//TODO
}

SESSION_RESULT hdp_channel_monitor(Session_Event_PARM *parm)
{
	if (parm == NULL || parm->event == SESSION_EVENT_INVALID)
	{
		return SR_E_INVALID_PARAM;
	}
	
	if (parm->event & SESSION_EVENT_CREATED)
	{
//		hdp_session_attached(parm->index);
	}
	if (parm->event & SESSION_EVENT_DATA_INCOMING)
	{
		hdp_channel_upper_data_incoming(parm->index);		
	}
	if (parm->event & SESSION_EVENT_BUFFER_WRITABLE)
	{
		hdp_channel_upper_buffer_writable(parm->index);		
	}
	if (parm->event & SESSION_EVENT_DESTROYED)
	{
		hdp_channel_detached(parm->index);		
	}

	return SR_NOERROR;		
}

BOOL hdp_channel_init(hdp_adp_channel_struct *channel)
{
	channel->inUse = TRUE;
	memset(channel, 0x0, sizeof(hdp_adp_channel_struct));
	session_mutex_create(&channel->mutex);
	return TRUE;
}
BOOL hdp_channel_deinit(hdp_adp_channel_struct *channel)
{
	channel->inUse = FALSE;
	session_mutex_destroy(channel->mutex);
	if(channel->sessionId > 0)
	{
		bt_session_destroy(channel->sessionId);
	}
	channel->mutex = NULL;
	return TRUE;
}

#endif /* __BT_HDP_PROFILE__ */
