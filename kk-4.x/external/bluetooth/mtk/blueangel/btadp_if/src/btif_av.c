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


/*****************************************************************************
 *
 *  Filename:      btif_av.c
 *
 *  Description:   Bluedroid AV implementation
 *
 *****************************************************************************/

#include <hardware/bluetooth.h>
#include "hardware/bt_av.h"

#define LOG_TAG "BTIF_AV"

#include "bt_a2dp_hdl.h"
#include "bt_a2dp_api.h"
#include "bt_avrcp_struct.h"
#include "bt_avrcp_api.h"
#include "bt_mmi.h"
#include "bt_ext_apis.h"
#include "bt_utils.h"
#include "bt_ext_debug.h"
#include "bt_profile_manager.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/*****************************************************************************
**  Constants & Macros
******************************************************************************/
#define BTIF_AV_SERVICE_NAME "Advanced Audio"

#define BTIF_TIMEOUT_AV_OPEN_ON_RC_SECS  2


/* Should not need dedicated suspend state as actual actions are no
   different than open state. Suspend flags are needed however to prevent
   media task from trying to restart stream during remote suspend or while
   we are in the process of a local suspend */

#define BTIF_AV_FLAG_LOCAL_SUSPEND_PENDING 0x1
#define BTIF_AV_FLAG_REMOTE_SUSPEND        0x2

/*****************************************************************************
**  Local type definitions
******************************************************************************/

typedef struct
{
    bt_bdaddr_t peer_bda;
    unsigned char flags;
} btif_av_cb_t;

/*****************************************************************************
**  Static variables
******************************************************************************/
static btav_callbacks_t *bt_av_callbacks = NULL;
static btif_av_cb_t btif_av_cb;

/* both interface and media task needs to be ready to alloc incoming request */
#define CHECK_BTAV_INIT() if ((bt_av_callbacks == NULL))\
{\
     bt_ext_log("%s: BTAV not initialized", __FUNCTION__);\
     return BT_STATUS_NOT_READY;\
}\
else\
{\
     bt_ext_log("%s", __FUNCTION__);\
}


#define HAL_CBACK(P_CB, P_CBACK, ...)\
    if (P_CB && P_CB->P_CBACK) {            \
        ALOGD("HAL %s->%s", #P_CB, #P_CBACK); \
        P_CB->P_CBACK(__VA_ARGS__);         \
    }                                       \
    else {                                  \
        ALOGE("Callback is NULL");  \
    }



/*************************************************************************
** Extern functions
*************************************************************************/
extern void btmtk_a2dp_send_stream_pause_req(module_type src_mod_id, U8 stream_handle);
extern BT_A2DP_APP_STATE btmtk_a2dp_app_get_stream_state(U8 stream_handle);
extern void btmtk_a2dp_app_setsuspendflag(bool flag);
extern bt_status_t btif_enable_service(uint8_t service_id);
extern bt_status_t btif_disable_service(uint8_t service_id);


/*************************************************************************
** Extern varialbes
*************************************************************************/
int a2dpsock;

int btmtk_a2dp_suspend(void)
{
    BT_A2DP_APP_STATE state;
    A2dpScoStatus ret = BT_A2DP_DISCONNECTED;

	state = btmtk_a2dp_app_get_stream_state(0);

	switch (state)
	{
	    case BT_A2DP_APP_STATE_IDLE:
			ret = BT_A2DP_DISCONNECTED;
			break;

		case BT_A2DP_APP_STATE_OPENED:
			ret = BT_A2DP_CONNECTED;
			break;

		case BT_A2DP_APP_STATE_STREAMING:
			btmtk_a2dp_send_stream_pause_req(MOD_MMI, 0);
			ret = BT_A2dp_PENDING;
			break;

		default:
			ret = BT_A2DP_DISCONNECTED;
			break;
	}
	btmtk_a2dp_app_setsuspendflag(true);
	return ret;
}

void btmtk_a2dp_resume(void)
{
    btmtk_a2dp_app_setsuspendflag(false);
}


static bool a2dp_registerSocket()
{
    bt_ext_log("[A2DP][if] a2dp_registerSocket.");
 
    a2dpsock = socket(PF_LOCAL, SOCK_DGRAM, 0);
	
    bt_ext_log("[A2DP] initNative a2dpsock = %d", a2dpsock);
	
    if (a2dpsock < 0)
    {
        ALOGE("[A2DP][ERR] create a2dp api socket failed : %s, errno=%d", strerror(errno), errno);
        return FALSE;
    }

    return TRUE;
}


bt_status_t btif_av_execute_service(bool b_enable)
{
    bt_ext_log("[A2DP] bt interface enable run, b_enable: %d\n", b_enable);
	
    if (b_enable)
    {
        if (!a2dp_registerSocket()) 
        {
            bt_ext_log("[A2DP] bt interface register socket fail");
			return BT_STATUS_FAIL;
        }

		bt_ext_log("[AVRCP] := bt interface try to active avrcp.\n");
		btmtk_avrcp_send_activate_req(0, 0, 0, 0x13, 0);

		bt_ext_log("[A2DP] bt interface try to active a2dp.\n");
		btmtk_a2dp_send_activate_req(a2dpsock, a2dpsock);
    }
	else
	{
		btmtk_avrcp_send_deactivate_req(0, 0);
		
		btmtk_a2dp_deactive_req();

		close(a2dpsock);
		a2dpsock = -1;
	}

	return BT_STATUS_SUCCESS;
}



/*******************************************************************************
**
** Function         disconnect
**
** Description      Tears down the AV signalling channel with the remote headset
**
** Returns          bt_status_t
**
*******************************************************************************/

void btmtk_a2dp_disconnect_CbHdl(void)
{
    BT_A2DP_APP_STATE state = btmtk_a2dp_global_state();

    if (state == BT_A2DP_APP_STATE_OPENED ||
        state == BT_A2DP_APP_STATE_STREAMING)
    {
        bt_ext_log("[A2DP] disconnect a2dp before ACL...\n");

        btmtk_a2dp_send_stream_close_req(MOD_MMI, 0);
    }
}


/* This function is used for handling message send from internal */
void btmtk_avrcp_ext_msg_handler(void *pContext, ilm_struct *ilm)
{
    U8 iResult = 0;
	
	btmtk_avrcp_handle_message(ilm);
	
	bt_ext_log("avrcp:= ext hdl msg_id:%d", ilm->msg_id);
}



/* This function is used for handling message send from internal */
void btmtk_a2dp_ext_msg_handler(void *pContext, ilm_struct *ilm)
{
    U8 iResult = A2DP_STATUS_NO_CHANGE;
	bt_addr_struct  addr;
	
	iResult = btmtk_a2dp_handle_message(ilm);
	
	bt_ext_log("a2dp ext hdl result: %d", iResult);

	btmtk_a2dp_get_bdaddr(&addr);
	btmtk_util_convert_bdaddr2array(&(btif_av_cb.peer_bda), &addr);

	switch(iResult)
    {
        case A2DP_STATUS_CHANGE_DISCONNECTED:
        {
			//Notify Profile Manager A2DP connection status been changed
			notifyProfileStateChanged(&addr,PROFILE_A2DP,PROFILE_STATE_DISCONNECTED);
			//Callback status change to framework
			HAL_CBACK(bt_av_callbacks, connection_state_cb,
                      BTAV_CONNECTION_STATE_DISCONNECTED, &(btif_av_cb.peer_bda));
			break;
       	}

        case A2DP_STATUS_CHANGE_CONNECTING:
        {
			HAL_CBACK(bt_av_callbacks, connection_state_cb,
                      BTAV_CONNECTION_STATE_CONNECTING, &(btif_av_cb.peer_bda));

			break;
       	}
            
        case A2DP_STATUS_CHANGE_CONNECTED:
        {
			notifyProfileStateChanged(&addr,PROFILE_A2DP,PROFILE_STATE_CONNECTED);
			HAL_CBACK(bt_av_callbacks, connection_state_cb,
                      BTAV_CONNECTION_STATE_CONNECTED, &(btif_av_cb.peer_bda));

			break;
       	}
		
        case A2DP_STATUS_CHANGE_PLAYING:
        {
			HAL_CBACK(bt_av_callbacks, audio_state_cb,
                      BTAV_AUDIO_STATE_STARTED, &(btif_av_cb.peer_bda));
			break;
       	}
		
#ifdef __BTMTK__
#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER
		case A2DP_STATUS_FM_PLAYING_FAILED:
		{
			HAL_CBACK(bt_av_callbacks, audio_state_cb,
                      BTAV_AUDIO_STATE_REMOTE_SUSPEND, &(btif_av_cb.peer_bda));

			break;
       	}
		
#endif
#endif
    }
}


/*******************************************************************************
**
** Function         btif_av_init
**
** Description      Initializes btif AV if not already done
**
** Returns          bt_status_t
**
*******************************************************************************/

bt_status_t btif_av_init(void)
{
    bt_ext_log("[A2DP] btif init\n");
	
    //if (btif_av_cb.sm_handle == NULL)
    {
        //
        btif_enable_service(BTMTK_AVRCP_SERVICE_ID);
        
        btmtk_profile_register(PROFILE_AVRCP, btmtk_avrcp_ext_msg_handler, NULL);

        //if (btif_a2dp_start_media_task() != GKI_SUCCESS)
        //    return BT_STATUS_FAIL;

        // I am not sure whether we need to call this function or not, just mark
        btif_enable_service(BTMTK_A2DP_SERVICE_ID);

        /* Initialize the AVRC CB */
        //btif_rc_init();

        /* Also initialize the AV state machine */
        //btif_av_cb.sm_handle = btif_sm_init((const btif_sm_handler_t*)btif_av_state_handlers, BTIF_AV_STATE_IDLE);

		//a2dp_registerSocket();

		btmtk_profile_register(PROFILE_A2DP, btmtk_a2dp_ext_msg_handler, NULL);

        //btif_a2dp_on_init();
        
        

        return BT_STATUS_SUCCESS;
    }

    //return BT_STATUS_DONE;
}

/*******************************************************************************
**
** Function         init
**
** Description      Initializes the AV interface
**
** Returns          bt_status_t
**
*******************************************************************************/

static bt_status_t init(btav_callbacks_t* callbacks )
{
    int status;

    bt_ext_log("%s", __FUNCTION__);

    if (bt_av_callbacks)
        return BT_STATUS_DONE;

    bt_av_callbacks = callbacks;

    return btif_av_init();
}

/*******************************************************************************
**
** Function         connect_int
**
** Description      Interface provide for JNI to initial a connection of A2DP
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t connect_int(bt_bdaddr_t *bd_addr)
{
	bt_addr_struct addr;
	
	bt_ext_log("%s", __FUNCTION__);	
	
    btmtk_util_convert_array2bdaddr(&addr, bd_addr);
	connect_profile(&addr,PROFILE_A2DP);
	return BT_STATUS_SUCCESS;
}
/*******************************************************************************
**
** Function         btif_av_connect
**
** Description      call external API to really establish a A2DP connection
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_av_connect(bt_addr_struct *bd_addr)
{
    bt_ext_log("%s", __FUNCTION__);
    CHECK_BTAV_INIT();

    btmtk_a2dp_send_stream_open_req(MOD_MMI, bd_addr, 0);

	return BT_STATUS_SUCCESS;
}
/*******************************************************************************
**
** Function         disconnect
**
** Description      Interface provide for JNI to end a connection of A2DP
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t disconnect(bt_bdaddr_t *bd_addr)
{
    
	bt_addr_struct addr;
	
	bt_ext_log("%s", __FUNCTION__);	
	
    btmtk_util_convert_array2bdaddr(&addr, bd_addr);
    
    disconnect_profile(&addr,PROFILE_A2DP);
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_av_disconnect
**
** Description      call external API to really end a A2DP connection
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_av_disconnect(bt_addr_struct *bd_addr)
{
    bt_ext_log("%s", __FUNCTION__);
    CHECK_BTAV_INIT();
    btmtk_a2dp_close_device(bd_addr);

	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         cleanup
**
** Description      Shuts down the AV interface and does the cleanup
**
** Returns          None
**
*******************************************************************************/
static void cleanup(void)
{
    bt_ext_log("%s", __FUNCTION__);

    if (bt_av_callbacks)
    {
	    btif_disable_service(BTMTK_AVRCP_SERVICE_ID);

	    btmtk_profile_unregister(PROFILE_AVRCP);

	    btif_disable_service(BTMTK_A2DP_SERVICE_ID);

	    btmtk_profile_unregister(PROFILE_A2DP);
	
        bt_av_callbacks = NULL;
    }
		
    return;
}

static const btav_interface_t bt_av_interface = {
    sizeof(btav_interface_t),
    init,
    connect_int,
    disconnect,
    cleanup,
};

/*******************************************************************************
**
** Function         btif_av_get_sm_handle
**
** Description      Fetches current av SM handle
**
** Returns          None
**
*******************************************************************************/
#if 0

btif_sm_handle_t btif_av_get_sm_handle(void)
{
    return btif_av_cb.sm_handle;
}

/*******************************************************************************
**
** Function         btif_av_stream_ready
**
** Description      Checks whether AV is ready for starting a stream
**
** Returns          None
**
*******************************************************************************/

BOOLEAN btif_av_stream_ready(void)
{
    btif_sm_state_t state = btif_sm_get_state(btif_av_cb.sm_handle);

    BTIF_TRACE_DEBUG3("btif_av_stream_ready : sm hdl %d, state %d, flags %x",
                btif_av_cb.sm_handle, state, btif_av_cb.flags);

    /* also make sure main adapter is enabled */
    if (btif_is_enabled() == 0)
    {
        BTIF_TRACE_EVENT0("main adapter not enabled");
        return FALSE;
    }

    /* check if we are remotely suspended */
    if (btif_av_cb.flags & BTIF_AV_FLAG_REMOTE_SUSPEND)
        return FALSE;

    return (state == BTIF_AV_STATE_OPENED);
}

/*******************************************************************************
**
** Function         btif_av_stream_started_ready
**
** Description      Checks whether AV ready for media start in streaming state
**
** Returns          None
**
*******************************************************************************/

BOOLEAN btif_av_stream_started_ready(void)
{
    btif_sm_state_t state = btif_sm_get_state(btif_av_cb.sm_handle);

    BTIF_TRACE_DEBUG3("btif_av_stream_started : sm hdl %d, state %d, flags %x",
                btif_av_cb.sm_handle, state, btif_av_cb.flags);

    /* don't allow media task to start if we are suspending or
       remotely suspended (not yet changed state) */
    if (btif_av_cb.flags & (BTIF_AV_FLAG_LOCAL_SUSPEND_PENDING | BTIF_AV_FLAG_REMOTE_SUSPEND))
        return FALSE;

    return (state == BTIF_AV_STATE_STARTED);
}

/*******************************************************************************
**
** Function         btif_dispatch_sm_event
**
** Description      Send event to AV statemachine
**
** Returns          None
**
*******************************************************************************/

/* used to pass events to AV statemachine from other tasks */
void btif_dispatch_sm_event(btif_av_sm_event_t event, void *p_data, int len)
{
    /* Switch to BTIF context */
    btif_transfer_context(btif_av_handle_event, event,
                          (char*)p_data, len, NULL);
}

/*******************************************************************************
**
** Function         btif_av_execute_service
**
** Description      Initializes/Shuts down the service
**
** Returns          BT_STATUS_SUCCESS on success, BT_STATUS_FAIL otherwise
**
*******************************************************************************/

bt_status_t btif_av_execute_service(BOOLEAN b_enable)
{
     if (b_enable)
     {
         /* TODO: Removed BTA_SEC_AUTHORIZE since the Java/App does not
          * handle this request in order to allow incoming connections to succeed.
          * We need to put this back once support for this is added */

         /* Added BTA_AV_FEAT_NO_SCO_SSPD - this ensures that the BTA does not
          * auto-suspend av streaming on AG events(SCO or Call). The suspend shall
          * be initiated by the app/audioflinger layers */
         BTA_AvEnable(BTA_SEC_AUTHENTICATE, (BTA_AV_FEAT_RCTG | BTA_AV_FEAT_NO_SCO_SSPD),
                      bte_av_callback);
         BTA_AvRegister(BTA_AV_CHNL_AUDIO, BTIF_AV_SERVICE_NAME, 0);
     }
     else {
         BTA_AvDeregister(btif_av_cb.bta_handle);
         BTA_AvDisable();
     }
     return BT_STATUS_SUCCESS;
}
#endif
/*******************************************************************************
**
** Function         btif_av_get_interface
**
** Description      Get the AV callback interface
**
** Returns          btav_interface_t
**
*******************************************************************************/
const btav_interface_t *btif_av_get_interface(void)
{
    bt_ext_log("%s", __FUNCTION__);
    return &bt_av_interface;
}

#if 0
// this function is used for avrcp to get a2dp status.

/*******************************************************************************
**
** Function         btif_av_is_rc_open_without_a2dp
**
** Description      Checks if GAVDTP Open notification to app is pending (2 second timer)
**
** Returns          boolean
**
*******************************************************************************/
BOOLEAN btif_av_is_connected(void)
{
    btif_sm_state_t state = btif_sm_get_state(btif_av_cb.sm_handle);
    return ((state == BTIF_AV_STATE_OPENED) || (state ==  BTIF_AV_STATE_STARTED));
}
#endif
