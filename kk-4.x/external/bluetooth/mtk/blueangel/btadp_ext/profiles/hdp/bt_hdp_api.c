//#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_hdp_struct.h"
#include "bt_hdp_common.h"
#include "bt_hdp_api.h"
#include "bt_ext_apis.h"

extern btmtk_hdp_context ext_hdp_cntx;

/*******************************************************************************
**
** Function         btmtk_hdp_register_instance
**
** Description    btmtk_hdp_register_instance
**
** Returns          bool
**
*******************************************************************************/
BOOL btmtk_hdp_register_instance(bt_hdp_role role, 
										bt_hdp_channel_type channelType, 
										U16 dataType,
										const char *description,
										int *app_id)
{
	bt_hdp_register_instance_req_struct *req = NULL;	
	ilm_struct ilm;
	btmtk_hdp_config *config = NULL;
	bt_ext_log("[HDP] btmtk_hdp_register_instance: on = 0x%X");

	config = hdp_ext_allocate_config();
	if (NULL == config)
	{
		return FALSE;
	}

	*app_id = hdp_get_app_id();
	config->app_id = *app_id;
	
	memset(&ilm, 0x0, sizeof(ilm_struct));	
        ilm.msg_id = MSG_ID_BT_HDP_REGISTER_INSTANCE_REQ;
	req = (bt_hdp_register_instance_req_struct *)ilm.ilm_data;
	req->role = role;
	req->channelType = channelType;
	req->dataType = dataType;
	req->app_id = *app_id;
	if (NULL != description)
	{
		strncpy(req->description, description, (BT_HDP_INSTANCE_MAX_DESC_LEN-1));
	}

	return btmtk_sendmsg(PROFILE_HDP, (void* )&ilm, sizeof(bt_hdp_register_instance_req_struct));

}

/*******************************************************************************
**
** Function         btmtk_hdp_deregister_instance
**
** Description    btmtk_hdp_deregister_instance
**
** Returns          bool
**
*******************************************************************************/
BOOL btmtk_hdp_deregister_instance(int app_id)
{
	bt_hdp_deregister_instance_req_struct *req;	
	btmtk_hdp_config *config = NULL;
    ilm_struct ilm;
	bool ret = FALSE;

	bt_ext_log("[HDP] btmtk_hdp_deregister_instance: 0x%X", app_id);
	config = hdp_ext_find_config_by_app_id(app_id);
	if (NULL == config)
	{
		return FALSE;
	}
	
	memset(&ilm, 0x0, sizeof(ilm_struct));	
    ilm.msg_id = MSG_ID_BT_HDP_DEREGISTER_INSTANCE_REQ;
	req = (bt_hdp_deregister_instance_req_struct *)ilm.ilm_data;
	req->mdepId = config->mdep_id;	
	return btmtk_sendmsg(PROFILE_HDP, (void* )&ilm, sizeof(bt_hdp_deregister_instance_req_struct));	
}

/*******************************************************************************
**
** Function         btmtk_hdp_connect
**
** Description    btmtk_hdp_connect
**
** Returns          bool
**
*******************************************************************************/
BOOL btmtk_hdp_connect(U8 *addr, int app_id, U8 cfg_idx, int *index)
{
	bt_hdp_connect_req_struct *req;
	btmtk_hdp_channel *channel = NULL;
	btmtk_hdp_config *config = NULL;
	
    ilm_struct ilm;

	bt_ext_log("[HDP] btmtk_hdp_connect: %d", app_id);
	
	config = hdp_ext_find_config_by_app_id(app_id);
	if (NULL == config)
	{
		return FALSE;
	}
	
	channel = hdp_ext_allocate_channel();
	if (channel == NULL)
	{
		return FALSE;
	}

	config->mdep_cfg = cfg_idx;

	*index = hdp_get_channel_id();
	channel->channel_id = *index;
	convert_bd_addr(&channel->bdaddr, addr);

	//TODO: channel config
	channel->config = config;
	
	memset(&ilm, 0x0, sizeof(ilm_struct));	
    ilm.msg_id = MSG_ID_BT_HDP_CONNECT_REQ;
	req = (bt_hdp_connect_req_struct *)ilm.ilm_data;
	req->mdepId = config->mdep_id;
	memcpy(&req->bdaddr, &channel->bdaddr, sizeof(bt_addr_struct));
	req->config = cfg_idx;
	req->index = *index;
	
	return btmtk_sendmsg(PROFILE_HDP, (void* )&ilm, sizeof(bt_hdp_connect_req_struct));	
}


BOOL btmtk_hdp_connect_rsp(bt_addr_struct *addr, U16 mdlid, U32 sessionid)
{
	bt_hdp_channel_opened_rsp_struct *rsp;
	btmtk_hdp_channel *channel = NULL;
	btmtk_hdp_config *config = NULL;
	
    ilm_struct ilm;

	bt_ext_log("[HDP] btmtk_hdp_connect_rsp");
		
	memset(&ilm, 0x0, sizeof(ilm_struct));	
    ilm.msg_id = MSG_ID_BT_HDP_CHANNEL_OPENED_RSP;
	rsp = (bt_hdp_channel_opened_rsp_struct *)ilm.ilm_data;
	memcpy(&rsp->bdaddr, addr, sizeof(bt_addr_struct));
	rsp->mdlId = mdlid;
	rsp->sessionId = sessionid;
	
	return btmtk_sendmsg(PROFILE_HDP, (void* )&ilm, sizeof(bt_hdp_connect_req_struct));	
}


/*******************************************************************************
**
** Function         btmtk_hdp_disconnect
**
** Description     Destroy a data channel
**
** Returns         bool
**
*******************************************************************************/
BOOL btmtk_hdp_disconnect(U32 index)
{
	btmtk_hdp_channel *channel= NULL;
	bt_hdp_disconnect_req_struct *req;	
    ilm_struct ilm;
	bool ret;

	bt_ext_log("[HDP] btmtk_hdp_disconnect");

	channel = hdp_ext_find_channel_by_id(index);
	if (channel == NULL)
	{
		return FALSE;
	}
	
	memset(&ilm, 0x0, sizeof(ilm_struct));	
    ilm.msg_id = MSG_ID_BT_HDP_DISCONNECT_REQ;
	req = (bt_hdp_disconnect_req_struct *)ilm.ilm_data;
	req->mdlId= channel->mdlId;
	memcpy(&req->bdaddr, &channel->bdaddr, sizeof(bt_addr_struct));	
	req->index = channel->channel_id;
	return btmtk_sendmsg(PROFILE_HDP, (void* )&ilm, sizeof(bt_hdp_disconnect_req_struct));	
}


void btmtk_hdp_init(btmtk_hdp_cb cb)
{
	bt_ext_log("[HDP] btmtk_hdp_init");
	if(ext_hdp_cntx.init)
	{
		return;
	}
	memset(&ext_hdp_cntx, 0x0, sizeof(btmtk_hdp_context));
	ext_hdp_cntx.init = TRUE;
	ext_hdp_cntx.cb = cb;
    btmtk_profile_register(PROFILE_HDP, btmtk_hdp_handle_message, NULL);
}
void btmtk_hdp_deinit()
{
	int index = 0;
	btmtk_hdp_channel *channel = NULL;
	btmtk_hdp_config *config = NULL;
	
    bt_ext_log("[HDP] btmtk_hdp_deinit");
    if(FALSE == ext_hdp_cntx.init)
	{
		return;
	}
	ext_hdp_cntx.init = FALSE;
	ext_hdp_cntx.cb = NULL;

	//TODO: force to clear channel context and app context
	for(index = 0; index < BTMTK_HDP_MAX_CHANNEL_NUM; index ++)
	{
		channel = &ext_hdp_cntx.channel[index];
		if(channel->used &&
			channel->session_id > 0)
		{
			hdp_ext_channel_detach(channel->session_id);
			hdp_ext_free_channel(channel);
		}
	}
    btmtk_profile_unregister(PROFILE_HDP);
	
}





