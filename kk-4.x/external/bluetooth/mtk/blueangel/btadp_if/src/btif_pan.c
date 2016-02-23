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
 *  Filename:      btif_pan.c
 *
 *  Description:   PAN Profile Bluetooth Interface
 *
 *
 ***********************************************************************************/
#include <hardware/bluetooth.h>
#include <hardware/bt_pan.h>
//#include <signal.h>
//#include <ctype.h>
//#include <sys/poll.h>
//#include <sys/ioctl.h>
//#include <netinet/in.h>
//#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
//#include "cutils/sockets.h"
#include <sys/select.h>
#include <fcntl.h>
//#include <fcntl.h>
//#include <sys/socket.h>
//#include <sys/wait.h>
//#include <net/if.h>
//#include <linux/sockios.h>
//#include <sys/prctl.h>
//#include <linux/if.h>
//#include <linux/if_tun.h>
//#include <linux/if_ether.h>

#define LOG_TAG "BTIF_PAN"
#include "btif_common.h"
//#include "btif_util.h"
//#include "btm_api.h"
//#include "bd.h"

//#include "bta_api.h"
//#include "bta_pan_api.h"
//#include "btif_sock_thread.h"
//#include "btif_sock_util.h"
//#include "btif_pan_internal.h"

#include "bt_mmi.h"
#include "bt_struct.h"
#include "bt_message.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_pan_struct.h"
#include "bttypes.h"
#include "bt_pan_api.h"
#include "bt_pan_hdl.h"
#include "bt_ext_common.h"
#include "bt_ext_apis.h"


//#define PANU_DISABLED TRUE

#if (PAN_NAP_DISABLED == TRUE) && (PANU_DISABLED == TRUE)
#define BTPAN_LOCAL_ROLE BTPAN_ROLE_NONE
#elif PAN_NAP_DISABLED == TRUE
#define BTPAN_LOCAL_ROLE BTPAN_ROLE_PANU
#elif PANU_DISABLED == TRUE
#define BTPAN_LOCAL_ROLE BTPAN_ROLE_PANNAP
#else
#define BTPAN_LOCAL_ROLE (BTPAN_ROLE_PANU | BTPAN_ROLE_PANNAP)
#endif

//#define asrt(s) if(!(s)) BTIF_TRACE_ERROR3("btif_pan: ## %s assert %s failed at line:%d ##",__FUNCTION__, #s, __LINE__)

//btpan_cb_t btpan_cb;
typedef struct
{
    int apisock;
    volatile int local_role;
    int enabled;
} btpan_cntx_t;

btpan_cntx_t btpan_cntx;

//BD_ADDR local_addr;
//static int jni_initialized, stack_initialized;


static btpan_callbacks_t callback;


static bt_status_t btpan_jni_init(const btpan_callbacks_t* callbacks);
static void btpan_jni_cleanup();
static bt_status_t btpan_connect(const bt_bdaddr_t *bd_addr, int local_role, int remote_role);
static bt_status_t btpan_disconnect(const bt_bdaddr_t *bd_addr);
static bt_status_t btpan_enable(int local_role);
static int btpan_get_local_role(void);

static void pan_jni_cb(void* handle, BT_CALLBACK_EVENT pan_ui_event, void *msg, unsigned short datasize);
static void pan_disable();

/*******************************************************************************
 **
 ** Function         btpan_ini
 **
 ** Description     initializes the pan interface
 **
 ** Returns         bt_status_t
 **
 *******************************************************************************/
static btpan_interface_t pan_if = {
    sizeof(pan_if),
    btpan_jni_init,
    btpan_enable,   // not called from PanService
    btpan_get_local_role,
    btpan_connect,
    btpan_disconnect,
    btpan_jni_cleanup
};

btpan_interface_t *btif_pan_get_interface()
{
    return &pan_if;
}

bt_status_t btif_pan_execute_service(BT_BOOL b_enable)
{   
     PAN_LOG("b_enable: %d", b_enable);
     if (b_enable)
     {
         btpan_enable(BTPAN_LOCAL_ROLE);
     }
     else 
     {
         pan_disable();
     }
     return BT_STATUS_SUCCESS;
}

static inline int btadp_role_to_btpan(int btadp_pan_role)
{
    int btpan_role = 0;
    PAN_LOG("btadp_pan_role:0x%x", btadp_pan_role);
    if(btadp_pan_role & PAN_SERVICE_NAP)
    {
        btpan_role |= BTPAN_ROLE_PANNAP;
    }
    if(btadp_pan_role & PAN_SERVICE_PANU)
    {
        btpan_role |= BTPAN_ROLE_PANU;
    }
    return btpan_role;
}

static inline int btpan_role_to_btadp(int btpan_role)
{
    int btadp_pan_role = 0;
    PAN_LOG("btpan_role:0x%x", btpan_role);
    if(btpan_role & BTPAN_ROLE_PANNAP)
    {
        btadp_pan_role |= PAN_SERVICE_NAP;
    }
    if(btpan_role & BTPAN_ROLE_PANU)
    {
        btadp_pan_role |= PAN_SERVICE_PANU;
    }
    return btadp_pan_role;
}



static U8 pan_init_socket()
{
    char buff[20];
    int sock_fl;
    static struct sockaddr_un g_int_adp_sockaddr;
    static socklen_t g_int_adp_sockaddr_len;
	
    g_int_adp_sockaddr.sun_family = AF_UNIX;
    strcpy(g_int_adp_sockaddr.sun_path, BT_SOCK_NAME_INT_ADP);
    g_int_adp_sockaddr_len = (offsetof(struct sockaddr_un, sun_path) + strlen(g_int_adp_sockaddr.sun_path) + 1);

    btpan_cntx.apisock = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (btpan_cntx.apisock < 0) {
	PAN_ERR("Create server socket failed.");
	return 0;
    }

    /* Get current socket flags */
    sock_fl = fcntl(btpan_cntx.apisock, F_GETFL);
    if (sock_fl == -1) {
	PAN_ERR("Get socket flags failed. Reason: [%d]%s", errno, strerror(errno));
	return 0;
    }

    /* Set socket as non-block */
    if (fcntl(btpan_cntx.apisock, F_SETFL, sock_fl | O_NONBLOCK) == -1) {
	PAN_ERR("Set O_NONBLOCK flag to socket failed. Reason: [%d]%s", errno, strerror(errno));
	return 0;
    }

    /* Read data from socket till empty */
    while(0 < recvfrom(btpan_cntx.apisock, (void*) buff, sizeof(buff), 0, NULL, NULL)) {
	PAN_LOG("Clearing socket...");
    }
    PAN_LOG("[BT_PAN]Socket clear.");

    /* Set back original flags */
    if (fcntl(btpan_cntx.apisock, F_SETFL, sock_fl) == -1) {
	PAN_ERR("Set O_NONBLOCK flag to socket failed. Reason: [%d]%s", errno, strerror(errno));
	return 0;
    }

    if (connect(btpan_cntx.apisock, (struct sockaddr *)&g_int_adp_sockaddr, g_int_adp_sockaddr_len) < 0)
    {
        PAN_ERR("[SPP][ERR] connect to /data/bt.int.adp failed: %s(%d)", strerror(errno), errno);
        return 0;
    }
    return 1;
}

static void pan_disable()
{
    if (btpan_cntx.enabled)
    {
        btpan_cntx.enabled = FALSE;
        btmtk_pan_deactivate_req();
    }
}

static void pan_jni_cb(void* handle, BT_CALLBACK_EVENT pan_ui_event, void *msg, unsigned short datasize) 
{

    switch(pan_ui_event)
    {
        case EVENT_PAN_MMI_ACTIVATE_CNF:
            {
                bt_pan_activate_cnf_struct *msg_p = (bt_pan_activate_cnf_struct *)msg;
                PAN_LOG("EVENT_PAN_MMI_ACTIVATE_CNF, result: %d", msg_p->result);
                if (!msg_p->result)
                {
                    btpan_cntx.enabled = FALSE;
                }
            }
            break;
        case EVENT_PAN_MMI_DEACTIVATE_CNF:
            {
                bt_pan_deactivate_cnf_struct *msg_p = (bt_pan_deactivate_cnf_struct *)msg;
                PAN_LOG("EVENT_PAN_MMI_DEACTIVATE_CNF, result: %d", msg_p->result);			
            }
            break;
        case EVENT_PAN_MMI_CONNECTION_AUTHORIZE_IND:
            {
                PAN_LOG("EVENT_PAN_MMI_CONNECTION_AUTHORIZE_IND");
                bt_pan_connection_authorize_ind_struct *msg_p = (bt_pan_connection_authorize_ind_struct *)msg;
                btmtk_pan_connection_authorize_rsp((U8*)&msg_p->bt_addr, TRUE);
            }
            break;
        case EVENT_PAN_MMI_CONNECT_IND:
            {
                PAN_LOG("EVENT_PAN_MMI_CONNECT_IND");
                bt_pan_connect_ind_struct *msg_p = (bt_pan_connect_ind_struct *)msg;
                int btpan_conn_local_role = btadp_role_to_btpan(msg_p->local_service);
                int btpan_remote_role = btadp_role_to_btpan(msg_p->remote_service);
                PAN_LOG("connect ind local role: %d, remote role: %d", btpan_conn_local_role, btpan_remote_role);
                callback.control_state_cb(BTPAN_STATE_ENABLED, btpan_conn_local_role, BT_STATUS_SUCCESS, PAN_TAP_NAME);      
                callback.connection_state_cb(BTPAN_STATE_CONNECTED, BT_STATUS_SUCCESS, (const bt_bdaddr_t*)&msg_p->bt_addr,
                        btpan_conn_local_role, btpan_remote_role);
                
            }
            break;
        case EVENT_PAN_MMI_CONNECT_CNF:
            {
                bt_pan_connect_cnf_struct *msg_p = (bt_pan_connect_cnf_struct *)msg;
                btpan_connection_state_t state;
                bt_status_t status;

                PAN_LOG("EVENT_PAN_MMI_CONNECT_CNF, result: %d", msg_p->result);
                int btpan_conn_local_role = btadp_role_to_btpan(msg_p->local_service);
                int btpan_remote_role = btadp_role_to_btpan(msg_p->remote_service);
                PAN_LOG("connect cnf local role: %d, remote role: %d", btpan_conn_local_role, btpan_remote_role);
                if(msg_p->result)
                {
                    state = BTPAN_STATE_CONNECTED;
                    status = BT_STATUS_SUCCESS;
                    
                    callback.control_state_cb(BTPAN_STATE_ENABLED, btpan_conn_local_role, BT_STATUS_SUCCESS, PAN_TAP_NAME);
                }
                else
                {
                    state = BTPAN_STATE_DISCONNECTED;
                    status = BT_STATUS_FAIL;
                }
                callback.connection_state_cb(state, status, (const bt_bdaddr_t*)&msg_p->bt_addr,
                        btpan_conn_local_role, btpan_remote_role);
            }
            break;
        case EVENT_PAN_MMI_DISCONNECT_IND:
            {
                PAN_LOG("EVENT_PAN_MMI_DISCONNECT_IND");
                bt_pan_disconnect_ind_struct *msg_p = (bt_pan_disconnect_ind_struct *)msg;

                int btpan_conn_local_role = btadp_role_to_btpan(msg_p->local_service);
                int btpan_remote_role = btadp_role_to_btpan(msg_p->remote_service);
                PAN_LOG("disconnect ind local role: %d, remote role: %d", btpan_conn_local_role, btpan_remote_role);
                callback.connection_state_cb(BTPAN_STATE_DISCONNECTED, BT_STATUS_SUCCESS, (const bt_bdaddr_t*)&msg_p->bt_addr,
                        btpan_conn_local_role, btpan_remote_role);
            }
            break;
        case EVENT_PAN_MMI_DISCONNECT_CNF:
            {
                PAN_LOG("EVENT_PAN_MMI_DISCONNECT_CNF");
                bt_pan_disconnect_cnf_struct *msg_p = (bt_pan_disconnect_cnf_struct *)msg;
                btpan_connection_state_t state;
                bt_status_t status;
                
                if(msg_p->result)
                {
                    state = BTPAN_STATE_DISCONNECTED;
                    status = BT_STATUS_SUCCESS;
                }
                else
                {
                    state = BTPAN_STATE_CONNECTED;
                    status = BT_STATUS_FAIL;
                }
                int btpan_conn_local_role = btadp_role_to_btpan(msg_p->local_service);
                int btpan_remote_role = btadp_role_to_btpan(msg_p->remote_service);
                PAN_LOG("disconnect cnf local role: %d, remote role: %d", btpan_conn_local_role, btpan_remote_role);
                callback.connection_state_cb(state, status, (const bt_bdaddr_t*)&msg_p->bt_addr,
                        btpan_conn_local_role, btpan_remote_role);
            }
            break;
        default:
            PAN_LOG("Unknown PAN event");
            break;			
    }	
}

/* It is used to handle message from int-adp */
void pan_handle_message(void *pContext, ilm_struct *ilm)
{
    PAN_LOG("handle msg_id: %d", ilm->msg_id);
    btmtk_pan_handle_message(ilm);
}

static bt_status_t btpan_jni_init(const btpan_callbacks_t* callbacks)
{
    if(pan_init_socket()==0)
    {
        PAN_ERR("Create sending socket failed");
        return BT_STATUS_FAIL;
    }
    //btpan_enable(BTPAN_LOCAL_ROLE);
    btif_enable_service(BTMTK_PAN_SERVICE_ID);
    
    btmtk_profile_register(PROFILE_PAN, pan_handle_message, NULL);
    callback = *callbacks;
    return BT_STATUS_SUCCESS;
}

static void btpan_jni_cleanup()
{
    //pan_disable();
    btif_disable_service(BTMTK_PAN_SERVICE_ID);
    if (btpan_cntx.apisock >= 0) {
        close(btpan_cntx.apisock);
        btpan_cntx.apisock = -1;
    }

    btmtk_deinit_pan();
    
    btmtk_profile_unregister(PROFILE_PAN);
}

static bt_status_t btpan_enable(int local_role)
{
    PAN_LOG("btpan_enable, local_role: %d", local_role);
    int role;

    if (!btpan_cntx.enabled)
    {
        btpan_cntx.enabled = TRUE;
        btmtk_pan_register_mmi_callback(pan_jni_cb);
        btmtk_pan_set_sockfd(btpan_cntx.apisock);
        
        role = btpan_role_to_btadp(local_role);
        btmtk_pan_activate_req(role);
        btpan_cntx.local_role = local_role;
    }
    return BT_STATUS_SUCCESS;
}
static int btpan_get_local_role()
{
    PAN_LOG("btpan local role: %d", btpan_cntx.local_role);
    return btpan_cntx.local_role;
}
static bt_status_t btpan_connect(const bt_bdaddr_t *bd_addr, int local_role, int remote_role)
{
    PAN_LOG("local_role:%d, remote_role:%d", local_role, remote_role);
    int pan_local_role = btpan_role_to_btadp(local_role);
    int pan_remote_role = btpan_role_to_btadp(remote_role);

    btmtk_pan_connect_req(pan_local_role, pan_remote_role, (U8*)bd_addr);
    return BT_STATUS_SUCCESS;
}
static bt_status_t btpan_disconnect(const bt_bdaddr_t *bd_addr)
{
    btmtk_pan_disconnect_req((U8*)bd_addr);
    return BT_STATUS_SUCCESS;
}

