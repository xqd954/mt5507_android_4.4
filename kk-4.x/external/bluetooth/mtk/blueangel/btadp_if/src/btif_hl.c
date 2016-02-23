/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/************************************************************************************
 *
 *  Filename:      btif_hl.c
 *
 *  Description:   Health Device Profile Bluetooth Interface
 *
 *
 ***********************************************************************************/
#define LOG_TAG "BTIF_HL"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <cutils/sockets.h>
#include <cutils/log.h>

#include <hardware/bluetooth.h>
#include <hardware/bt_hl.h>

#include "btif_common.h"
#include "btif_hl.h"
#include <hardware/bluetooth.h>
#include "bt_hdp_api.h"

/************************************************************************************
**  Static variables
************************************************************************************/
static bthl_callbacks_t *bt_hl_callbacks=NULL;

#define INVALID_STATE 0xFF


static bt_status_t register_application(bthl_reg_param_t *p_reg_param, int *app_id);
static bt_status_t unregister_application(int app_id);
static bt_status_t connect_channel(int app_id, bt_bdaddr_t *bd_addr, int mdep_cfg_index, int *channel_id);
static bt_status_t destroy_channel(int channel_id);

bt_hdp_channel_type convert_channel_type(bthl_channel_type_t type)
{
	switch(type)
	{
	case BTHL_CHANNEL_TYPE_RELIABLE:
		return BT_HDP_CHANNEL_TYPE_RELIABLE;
		
	case BTHL_CHANNEL_TYPE_STREAMING:
		return BT_HDP_CHANNEL_TYPE_STREAMING;
		
	case BTHL_CHANNEL_TYPE_ANY:
		return BT_HDP_CHANNEL_TYPE_NO_PREFERENCE;

	default:
		return 0xFF;
	}
}

bt_hdp_role convert_role(bthl_mdep_role_t role)
{
	switch(role)
	{
	case BTHL_MDEP_ROLE_SOURCE:
		return BT_HDP_ROLE_SOURCE;

	case BTHL_MDEP_ROLE_SINK:
		return BT_HDP_ROLE_SINK;

	default:
		return BT_HDP_ROLE_INVALID;
	}
}


int convert_event_result(BTMTK_HDP_EVENT event, BTMTK_HDP_RESULT result)
{
	if (result > BTMTK_HDP_RESULT_FAIL)
		return INVALID_STATE;
	
	switch(event)
	{
	case BTMTK_HDP_EVENT_REGISTERED:
		return (result == BTMTK_HDP_RESULT_SUCCES) ? 
			BTHL_APP_REG_STATE_REG_SUCCESS : BTHL_APP_REG_STATE_REG_FAILED;
		
		
	case BTMTK_HDP_EVENT_DEREGISTERED:
		return (result == BTMTK_HDP_RESULT_SUCCES) ? 
			BTHL_APP_REG_STATE_DEREG_SUCCESS : BTHL_APP_REG_STATE_DEREG_FAILED;

	//NOTES:
	//when connectiong fails, directly return the "destroyed" event
	//which will help to clear Java layer context, see HealthServiceMessageHandler@HealthService.java
	//TODO: when connections fails, should we return connecting before disconnected?
	case BTMTK_HDP_EVENT_CONNECT_CNF:
		return (result == BTMTK_HDP_RESULT_SUCCES) ? 
			BTHL_CONN_STATE_CONNECTING : BTHL_CONN_STATE_DESTROYED;
		
	case BTMTK_HDP_EVENT_DISCONNECT_CNF:
		return (result == BTMTK_HDP_RESULT_SUCCES) ? 
			BTHL_CONN_STATE_DISCONNECTED : BTHL_CONN_STATE_CONNECTED;
		

	case BTMTK_HDP_EVENT_OPEN_IND:
		return BTHL_CONN_STATE_CONNECTED;
		
	case BTMTK_HDP_EVENT_CLOSE_IND:
		return BTHL_CONN_STATE_DESTROYED;
		
	default:
		return INVALID_STATE;
	}
}

void btif_hdp_callback(btmtk_hdp_cb_param *parm)
{
	int state = convert_event_result(parm->event, parm->result);
	if (NULL == bt_hl_callbacks || state == INVALID_STATE)
	{
		ALOGI("callback is empry or invalid state");
		return;
	}
	switch(parm->event)
	{
	case BTMTK_HDP_EVENT_REGISTERED:
	case BTMTK_HDP_EVENT_DEREGISTERED:
		bt_hl_callbacks->app_reg_state_cb(parm->app_id,state);
		break;
		
	case BTMTK_HDP_EVENT_DISCONNECT_CNF:
	case BTMTK_HDP_EVENT_CONNECT_CNF:
	case BTMTK_HDP_EVENT_OPEN_IND:
	case BTMTK_HDP_EVENT_CLOSE_IND:
		bt_hl_callbacks->channel_state_cb(parm->app_id,
                                   (bt_bdaddr_t *)parm->bd_addr, 
                                   parm->mdep_cfg_index,
                                   parm->channel_id,
                                   state, 
                                   parm->fd);
		break;

	default:
		return;
	}
}

/*******************************************************************************
**
** Function         init
**
** Description     initializes the hl interface
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t init( bthl_callbacks_t* callbacks ){
    bt_status_t status = BT_STATUS_SUCCESS;
    bt_hl_callbacks = callbacks;
	
	btmtk_hdp_init(btif_hdp_callback);
    return status;
}
/*******************************************************************************
**
** Function         cleanup
**
** Description      Closes the HL interface
**
** Returns          void
**
*******************************************************************************/
static void  cleanup( void ){
	btmtk_hdp_deinit();
	bt_hl_callbacks = NULL;
}

static const bthl_interface_t bthlInterface = {
    sizeof(bthl_interface_t),
    init,
    register_application,
    unregister_application,
    connect_channel,
    destroy_channel,
    cleanup,
};

/*******************************************************************************
**
** Function         connect_channel
**
** Description     connect a data channel
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t connect_channel(int app_id, bt_bdaddr_t *bd_addr, int mdep_cfg_index, int *channel_id)
{
    bt_status_t status;	
    ALOGI("%s", __FUNCTION__);
    status = btmtk_hdp_connect((U8 *)bd_addr, app_id, mdep_cfg_index, channel_id) ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
    ALOGI("%s status=%d channel_id=0x%08x", __FUNCTION__, status, *channel_id);

    return status;
}



/*******************************************************************************
**
** Function         destroy_channel
**
** Description      destroy a data channel
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t destroy_channel(int channel_id)
{
    bt_status_t status;	
    ALOGI("%s", __FUNCTION__);
    status = btmtk_hdp_disconnect(channel_id) ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
    ALOGI("%s status=%d channel_id=0x%08x", __FUNCTION__, status, channel_id);

    return status;
}
/*******************************************************************************
**
** Function         unregister_application
**
** Description     unregister an HDP application
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t unregister_application(int app_id)
{
    bt_status_t         status = BT_STATUS_SUCCESS;

    ALOGI("%s app_id=%d", __FUNCTION__, app_id);
	
    status = btmtk_hdp_deregister_instance(app_id) ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;

    ALOGI("de-reg return status=%d", status);
    return status;
}
/*******************************************************************************
**
** Function         register_application
**
** Description     register an HDP application
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t register_application(bthl_reg_param_t *p_reg_param, int *app_id)
{
    bool ret = FALSE;
    bt_status_t status = BT_STATUS_SUCCESS;
	bt_hdp_role role = convert_role(p_reg_param->mdep_cfg->mdep_role);
	bt_hdp_channel_type channel_type = convert_channel_type(p_reg_param->mdep_cfg->channel_type);

    ALOGI("[HDP]registerHealthApplicationNative: role-> %d, channel type->%d, name->%s",
				p_reg_param->mdep_cfg->mdep_role, p_reg_param->mdep_cfg->channel_type, p_reg_param->application_name);

	if (role == BT_HDP_ROLE_INVALID ||
		channel_type == 0xFF)
	{
		return BT_STATUS_FAIL;
	}
    ret = btmtk_hdp_register_instance(role, 
		 								channel_type, // from bthl_channel_type_t to bt_hdp_channel_type
		 								(U16)p_reg_param->mdep_cfg->data_type, 
		 								p_reg_param->application_name,
		 								app_id);	
	if(!ret)
	{
		ALOGI("[HDP]fail to register APP: result %d, id %d", ret, *app_id);
		status = BT_STATUS_FAIL;
	}
    
    ALOGI("register_application status=%d app_id=%d", status, *app_id);
    return status;
}




/*******************************************************************************
**
** Function         btif_hl_get_interface
**
** Description      Get the hl callback interface
**
** Returns          bthf_interface_t
**
*******************************************************************************/
const bthl_interface_t *btif_hl_get_interface(){
    return &bthlInterface;
}

