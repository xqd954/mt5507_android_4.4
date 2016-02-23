#include "bt_hdp_common.h"
#include "bt_session.h"

btmtk_hdp_context ext_hdp_cntx;

static int hdp_app_id = 0;
static int hdp_channel_id = 0;
static U16 hdp_channel_fd_id = 0;

int hdp_get_id(int *id)
{
	if ((*id)++ < 0)
	{
		(*id) = 0;
	}
	return *id;	
}

int hdp_get_app_id()
{
	return hdp_get_id(&hdp_app_id);
}

int hdp_get_channel_id()
{
	return hdp_get_id(&hdp_channel_id);
}	
U16 btmtk_hdp_get_context_id()
{
	return PROFILE_HDP_DATA + (hdp_channel_fd_id++)%SESSION_ID_RANGE;
}


void convert_bd_addr(bt_addr_struct* dest, U8 *src)
{
    dest->lap = src[5] + (src[4] << 8) + (src[3] << 16);
    dest->uap = src[2];
    dest->nap = src[1] + (src[0] << 8);	
}

void revert_bd_addr(U8 *dest, bt_addr_struct *source)
{
	dest[0] = ((source->nap & 0xFF00) >> 8);
	dest[1] = (U8) (source->nap & 0x00FF);
	dest[2] = source->uap;
	dest[3] = (U8) ((source->lap & 0x00FF0000) >> 16);
	dest[4] = (U8) ((source->lap & 0x0000FF00) >> 8);
	dest[5] = (U8) (source->lap & 0x000000FF);
}


btmtk_hdp_config *hdp_ext_allocate_config()
{
	int index = 0;
	btmtk_hdp_config *config = NULL;
	for(index = 0; index < BTMTK_HDP_MAX_INSTANCE_NUM; index ++)
	{	
		config = &ext_hdp_cntx.config[index];
		if (config->used == FALSE)
		{
			config->used = TRUE;
			return config;
		}
	}
	return NULL;
}

btmtk_hdp_config *hdp_ext_find_config_by_app_id(int app_id)
{
	int index = 0;
	btmtk_hdp_config *config = NULL;
	for(index = 0; index < BTMTK_HDP_MAX_INSTANCE_NUM; index ++)
	{	
		config = &ext_hdp_cntx.config[index];
		if (config->used &&
			config->app_id == app_id)
		{
			return config;
		}
	}
	return NULL;
}

btmtk_hdp_config *hdp_ext_find_config_by_mdep_id(U8 mdep_id)
{
	int index = 0;
	btmtk_hdp_config *config = NULL;
	for(index = 0; index < BTMTK_HDP_MAX_INSTANCE_NUM; index ++)
	{	
		config = &ext_hdp_cntx.config[index];
		if (config->used &&
			config->mdep_id == mdep_id)
		{
			return config;
		}
	}
	return NULL;
}

btmtk_hdp_channel *hdp_ext_allocate_channel()
{
	int index = 0;
	btmtk_hdp_channel *channel = NULL;
	for(index = 0; index < BTMTK_HDP_MAX_CHANNEL_NUM; index ++)
	{	
		channel = &ext_hdp_cntx.channel[index];
		if (channel->used == FALSE)
		{
			channel->used = TRUE;
			return channel;
		}
	}
	return NULL;
}


void hdp_ext_free_channel(btmtk_hdp_channel *channel)
{
	if(NULL == channel)
	{
		return;
	}
	channel->used = FALSE;
	memset(channel, 0x0, sizeof(btmtk_hdp_channel));
}


btmtk_hdp_channel *hdp_ext_find_channel_by_id(int channel_id)
{
	int index = 0;
	btmtk_hdp_channel *channel = NULL;
	for(index = 0; index < BTMTK_HDP_MAX_CHANNEL_NUM; index ++)
	{	
		channel = &ext_hdp_cntx.channel[index];
		if (channel->used &&
			channel->channel_id == channel_id)
		{
			return channel;
		}
	}
	return NULL;
}

btmtk_hdp_channel *hdp_ext_find_channel_by_addr_and_mdl(bt_addr_struct *addr, U16 mdlid)
{
	int index = 0;
	btmtk_hdp_channel *channel = NULL;
	for(index = 0; index < BTMTK_HDP_MAX_CHANNEL_NUM; index ++)
	{	
		channel = &ext_hdp_cntx.channel[index];
		if (channel->used &&
			channel->bdaddr.lap == addr->lap &&
			channel->bdaddr.nap == addr->nap &&
			channel->bdaddr.uap == addr->uap &&
			channel->mdlId == mdlid)
		{
			return channel;
		}
	}
	return NULL;
}

SESSION_RESULT hdp_ext_channel_attach(U32 *sessionid, int *sessionfd)
{
	SESSION_RESULT sret = SR_NOERROR;
	int fd = -1;
	U16 id = 0;
	U32 index = 0;

	id = btmtk_hdp_get_context_id();
	sret = bt_session_connect(id ,&index);
	if (SR_NOERROR != sret)
	{
		goto exit;
	}
	
	sret = bt_session_get_fd(index, &fd);
	if (sret == SR_NOERROR)
	{
		*sessionid = index;
		*sessionfd = fd;
		bt_ext_log("[JSR82][EXT]btmtk_hdp_session_attach fd:%d", fd);
	}

	//the ownership of this fd has been transferred to app, so do not operate on this fd
	sret = bt_session_lock(index, TRUE);
	if (sret != SR_NOERROR)
	{
		bt_ext_log("[JSR82][EXT]bt_session_lock fail %d", sret);
	}
exit:
	return sret;
}
void hdp_ext_channel_detach(U32 sessionid)
{
	bt_session_destroy(sessionid);
}

BOOL hdp_update_channel_config(btmtk_hdp_channel *channel, U8 mdepid)
{
	if (channel->config == NULL ||
		channel->config->mdep_id != mdepid)
	{
		//update config for this channel
		channel->config = hdp_ext_find_config_by_mdep_id(mdepid);
	}

	if (channel->config == NULL)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void btmtk_hdp_mmi_cb(void *context, BTMTK_HDP_EVENT event, U8 result)
{
	btmtk_hdp_cb_param parms;
	btmtk_hdp_config *config = NULL;
	btmtk_hdp_channel *channel = NULL;
	
	memset(&parms, 0x0, sizeof(btmtk_hdp_cb_param));
	parms.event = event;
	parms.result = result;

	switch(event)
	{
	case BTMTK_HDP_EVENT_REGISTERED:
	case BTMTK_HDP_EVENT_DEREGISTERED:
		config = (btmtk_hdp_config *)context;
		parms.app_id = config->app_id;
		break;

	case BTMTK_HDP_EVENT_CONNECT_CNF:
	case BTMTK_HDP_EVENT_DISCONNECT_CNF:
	case BTMTK_HDP_EVENT_CLOSE_IND:
	case BTMTK_HDP_EVENT_OPEN_IND:
		channel = (btmtk_hdp_channel *)context;		
		parms.app_id = channel->config->app_id;
		revert_bd_addr(parms.bd_addr, &channel->bdaddr);
		parms.fd = channel->fd;
		parms.mdep_cfg_index = 0;
		parms.channel_id = channel->channel_id;
		break;

	default:
		bt_ext_err("[HDP] invalid event");
		return;
		
	}
	ext_hdp_cntx.cb(&parms);
}
