#include "bt_hdp_common.h"
#include "bluetooth_hdp_struct.h"
#include "bt_message.h"
#include "bt_ext_debug.h"

void btmtk_hdp_handle_register_cnf(ilm_struct *ilm)
{
	bt_hdp_register_instance_cnf_struct *cnf = (bt_hdp_register_instance_cnf_struct*)ilm->ilm_data;
	btmtk_hdp_config *config = NULL;

	config = hdp_ext_find_config_by_app_id(cnf->app_id);
	if (NULL == config)
	{
		//TODO: deregister it ?????
		bt_ext_err("[HDP] register fail: no config context for app_id[%d]", cnf->app_id);
		return;
	}
	config->mdep_id = cnf->mdepId;
	
    btmtk_hdp_mmi_cb(config, BTMTK_HDP_EVENT_REGISTERED, cnf->result);
}

void btmtk_hdp_handle_deregister_cnf(ilm_struct *ilm)
{
	bt_hdp_deregister_instance_cnf_struct *cnf = (bt_hdp_deregister_instance_cnf_struct*)ilm->ilm_data;
	btmtk_hdp_config *config = NULL;

	config = hdp_ext_find_config_by_mdep_id(cnf->mdepId);
	if (NULL == config)
	{
		//TODO: deregister it ?????
		bt_ext_err("[HDP] deregister fail: no config context for mdepId[%d]", cnf->mdepId);
		return;
	}
		
    btmtk_hdp_mmi_cb(config, BTMTK_HDP_EVENT_DEREGISTERED, cnf->result);
	
	memset(config, 0x0, sizeof(btmtk_hdp_config));
	config->used = FALSE;
}

void btmtk_hdp_handle_connect_cnf(ilm_struct *ilm)
{
	bt_hdp_connect_cnf_struct *cnf = (bt_hdp_connect_cnf_struct *)ilm->ilm_data;
	btmtk_hdp_channel* channel = NULL;
	btmtk_hdp_config *config = NULL;

	bt_ext_log("[HDP] btmtk_hdp_handle_connect_cnf : result=%d", cnf->result);
	channel = hdp_ext_find_channel_by_id(cnf->index);
	if (NULL == channel)
	{
		bt_ext_err("[HDP] no channel context for index[%d]", cnf->index);
		return;
	}

	//
	config = hdp_ext_find_config_by_mdep_id(cnf->mdepId);
	if (NULL == config)
	{
		bt_ext_err("[HDP] no config context for mdepId[%d]", cnf->mdepId);
		return;
	}
	channel->config = config;

	channel->mdlId = cnf->mdlId;
	
    btmtk_hdp_mmi_cb(channel, BTMTK_HDP_EVENT_CONNECT_CNF, cnf->result);

	if (cnf->result != BT_HDP_SUCCESS)
	{
		hdp_ext_free_channel(channel);
	}

}

void btmtk_hdp_handle_disconnect_cnf(ilm_struct *ilm)
{
	bt_hdp_disconnect_cnf_struct *cnf = (bt_hdp_disconnect_cnf_struct *)ilm->ilm_data;
	btmtk_hdp_channel* channel = NULL;

	bt_ext_log("[HDP] btmtk_hdp_handle_connect_cnf : result=%d", cnf->result);
	channel = hdp_ext_find_channel_by_id(cnf->index);
	if (NULL == channel)
	{
		bt_ext_err("[HDP] no channel context for index[%d]", cnf->index);
		return;
	}

    btmtk_hdp_mmi_cb(channel, BTMTK_HDP_EVENT_DISCONNECT_CNF, cnf->result);
}

void btmtk_hdp_handle_channel_opened_ind(ilm_struct *ilm)
{
	bt_hdp_channel_opened_ind_struct *ind = (bt_hdp_channel_opened_ind_struct *)ilm->ilm_data;
	SESSION_RESULT sret = SR_NOERROR;	
	btmtk_hdp_channel* channel = NULL;
	btmtk_hdp_config *config = NULL;

	bt_ext_log("[HDP] btmtk_hdp_handle_channel_opened_ind");
	channel = hdp_ext_find_channel_by_addr_and_mdl(&ind->bdaddr, ind->mdlId);
	if (NULL == channel)
	{
		channel = hdp_ext_allocate_channel();
		if (channel != NULL)
		{
			memcpy(&channel->bdaddr, &ind->bdaddr, sizeof(bt_addr_struct));
			channel->mdlId = ind->mdlId;
			channel->channel_id = hdp_get_channel_id();
		}
		else
		{
			bt_ext_err("[HDP] no available channel context for incoming connection");
			return;
		}
	}

	hdp_update_channel_config(channel, ind->mdepId);
	
	//fd needs to create
	sret = hdp_ext_channel_attach(&channel->session_id, &channel->fd);
	if (SR_NOERROR != sret)
	{
		//TODO: 1.need to disconnec the channel, 2, send disconnected state call back to MMI
		memset(channel, 0x0, sizeof(btmtk_hdp_channel));
		return;		
	}

	//send rsp to internal layer
	btmtk_hdp_connect_rsp(&ind->bdaddr, ind->mdlId, channel->session_id);
	
    btmtk_hdp_mmi_cb(channel, BTMTK_HDP_EVENT_OPEN_IND, 0);
}

void btmtk_hdp_handle_channel_closed_ind(ilm_struct *ilm)
{
	bt_hdp_channel_closed_ind_struct *ind = (bt_hdp_channel_closed_ind_struct *)ilm->ilm_data;
	btmtk_hdp_channel* channel = NULL;

	bt_ext_log("[HDP] btmtk_hdp_handle_channel_closed_ind");
	channel = hdp_ext_find_channel_by_addr_and_mdl(&ind->bdaddr, ind->mdlId);
	if(channel == NULL)
	{
		bt_ext_log("[HDP] btmtk_hdp_find_channel_by_addr_and_mdl return NULL");
		return;
	}

	//close fd
	hdp_ext_channel_detach(channel->session_id);
	
	btmtk_hdp_mmi_cb(channel, BTMTK_HDP_EVENT_CLOSE_IND, 0);

	hdp_ext_free_channel(channel);
}

void btmtk_hdp_handle_message(void *pContext, ilm_struct *ilm)
{
    switch (ilm->msg_id)
    {
       case MSG_ID_BT_HDP_REGISTER_INSTANCE_CNF:
	   	btmtk_hdp_handle_register_cnf(ilm);
		break;
	case MSG_ID_BT_HDP_DEREGISTER_INSTANCE_CNF:
	   	btmtk_hdp_handle_deregister_cnf(ilm);
		break;
	case MSG_ID_BT_HDP_CONNECT_CNF:
		btmtk_hdp_handle_connect_cnf(ilm);
		break;
		
	case MSG_ID_BT_HDP_DISCONNECT_CNF:
		btmtk_hdp_handle_disconnect_cnf(ilm);
		break;
		
	case MSG_ID_BT_HDP_CHANNEL_OPENED_IND:
		btmtk_hdp_handle_channel_opened_ind(ilm);
		break;
		
	case MSG_ID_BT_HDP_CHANNEL_CLOSED_IND:
		btmtk_hdp_handle_channel_closed_ind(ilm);
		break;
	default:
		break;
    }	
}

