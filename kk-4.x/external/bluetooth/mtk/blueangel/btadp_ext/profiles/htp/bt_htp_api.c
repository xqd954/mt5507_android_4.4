/*****************************************************************************
 *
 * Filename:
 * ---------
 * bt_htp_api.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Health Thermometer Profile Collector Role External Adaptation Layer.
 *
 * Author:
 * -------
 * Ting Zheng
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 * 
 * 10 16 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#if defined(__BT_HTPC_PROFILE__) || defined(__BT_HTPT_PROFILE__)

#include <assert.h>

#include "bt_mmi.h"
#include "bt_struct.h"
#include "ext_os_api.h"

#include "bt_message.h"
#ifdef BTMTK_ON_LINUX
#include "bt_ext_debug.h"
#include <errno.h>
#else
#include "bt_mmi_msg.h"
#endif

#include "bluetooth_htp_struct.h"
#include "bluetooth_htp_message.h"
#include "bt_htp_struct.h"


BTMTK_EventCallBack g_HTP_MMI_Callback;

typedef struct
{
    LOCAL_PARA_HDR
} bt_htp_request_hdr_struct;


#ifdef BTMTK_ON_LINUX
int g_htpSockfd;
static void BTCMD_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size);
#endif

void btmtk_htp_register_mmi_callback(BTMTK_EventCallBack callback)
{
    g_HTP_MMI_Callback = callback;
}

static void htp_process_cmd(bt_htp_op_struct *msg, U32 size)
{
#ifdef __BT_HTP_CMD_AGENT__
    BTCMD_SendMessage(MSG_ID_BT_HTP_CMD, MOD_EXTERNAL, msg, size);
#else
    htp_handle_cmd(msg, size);
#endif
}


/*
*  Collector External API Definitions
*/
#ifdef __BT_HTPC_PROFILE__
void btmtk_htpc_activate_req(void)
{
    bt_htp_op_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htp_op_struct));
    msg.op = BT_HTPC_OP_ACTIVATE;
    htp_process_cmd(&msg, sizeof(bt_htp_op_struct));
}

void btmtk_htpc_deactivate_req(void)
{
    bt_htp_op_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htp_op_struct));
    msg.op = BT_HTPC_OP_DEACTIVATE;
    htp_process_cmd(&msg, sizeof(bt_htp_op_struct));
}

void btmtk_htpc_connect_req(U8 *device_addr)
{
    bt_htp_op_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htp_op_struct));
    msg.op = BT_HTPC_OP_CONNECT;
    btmtk_os_memcpy((U8 *)&msg.addr, device_addr, sizeof(msg.addr));
    
    htp_process_cmd(&msg, sizeof(bt_htp_op_struct));
}

void btmtk_htpc_disconnect_req(U8 *device_addr)
{
    bt_htp_op_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htp_op_struct));
    msg.op = BT_HTPC_OP_DISCONNECT;
    btmtk_os_memcpy((U8 *)&msg.addr, device_addr, sizeof(msg.addr));
    
    htp_process_cmd(&msg, sizeof(bt_htp_op_struct));
}

void btmtk_htpc_set_config_req(U8 *device_addr, bt_htp_desc_config_struct *config)
{
    bt_htp_op_struct *msg;
    U8 msg_len = sizeof(bt_htp_op_struct) + sizeof(bt_htp_desc_config_struct);

    msg = (bt_htp_op_struct *)HTP_MEM_MALLOC(msg_len);
    if (msg == NULL)
    {
        return;
    }

    btmtk_os_memset((U8 *)msg, 0, msg_len);
    msg->op = BT_HTPC_OP_SET_CONFIG;
    btmtk_os_memcpy((U8 *)&msg->addr, device_addr, sizeof(msg->addr));
    btmtk_os_memcpy((U8 *)msg + sizeof(bt_htp_op_struct), (U8 *)config, sizeof(bt_htp_desc_config_struct));
    
    htp_process_cmd(msg, msg_len);

    HTP_MEM_FREEIF(msg);
}

void btmtk_htpc_set_interval_value_req(U8 *device_addr, U16 interval)
{
    bt_htp_op_struct *msg;
    U8 msg_len = sizeof(bt_htp_op_struct) + sizeof(U16);

    msg = (bt_htp_op_struct *)HTP_MEM_MALLOC(msg_len);
    if (msg == NULL)
    {
        return;
    }

    btmtk_os_memset((U8 *)msg, 0, msg_len);
    msg->op = BT_HTPC_OP_SET_INTERVAL_VALUE;
    btmtk_os_memcpy((U8 *)&msg->addr, device_addr, sizeof(msg->addr));
    btmtk_os_memcpy((U8 *)msg + sizeof(bt_htp_op_struct), (U8 *)&interval, sizeof(U16));
    
    htp_process_cmd(msg, msg_len);

    HTP_MEM_FREEIF(msg);
}

void btmtk_htpc_remove_device_req(U8 *device_addr)
{
    bt_htp_op_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htp_op_struct));
    msg.op = BT_HTPC_OP_REMOVE_DEV;
    btmtk_os_memcpy((U8 *)&msg.addr, device_addr, sizeof(msg.addr));
    
    htp_process_cmd(&msg, sizeof(bt_htp_op_struct));
}

void htpc_op_activate(void)
{
    BTCMD_SendMessage(MSG_ID_BT_HTPC_ACTIVATE_REQ, MOD_EXTERNAL, NULL, 0);
}

void htpc_op_deactivate(void)
{
    BTCMD_SendMessage(MSG_ID_BT_HTPC_DEACTIVATE_REQ, MOD_EXTERNAL, NULL, 0);
}

void htpc_op_connect(BD_ADDR *bt_addr)
{
    bt_htpc_connect_req_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpc_connect_req_struct));
    btmtk_os_memcpy((U8 *)&msg.bt_addr, bt_addr, sizeof(msg.bt_addr));

    BTCMD_SendMessage(MSG_ID_BT_HTPC_CONNECT_REQ, MOD_EXTERNAL, &msg, sizeof(msg));
}

void htpc_op_disconnect(BD_ADDR *bt_addr)
{
    bt_htpc_disconnect_req_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpc_disconnect_req_struct));
    btmtk_os_memcpy((U8 *)&msg.bt_addr, bt_addr, sizeof(msg.bt_addr));

    BTCMD_SendMessage(MSG_ID_BT_HTPC_DISCONNECT_REQ, MOD_EXTERNAL, &msg, sizeof(msg));
}

void htpc_op_set_config(BD_ADDR *bt_addr, bt_htp_desc_config_struct *config)
{
    bt_htpc_set_config_req_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpc_set_config_req_struct));
    btmtk_os_memcpy((U8 *)&msg.bt_addr, bt_addr, sizeof(msg.bt_addr));
    btmtk_os_memcpy((U8 *)&msg.config, config, sizeof(msg.config));

    BTCMD_SendMessage(MSG_ID_BT_HTPC_SET_CONFIG_REQ, MOD_EXTERNAL, &msg, sizeof(msg));
}

void htpc_op_set_interval(BD_ADDR *bt_addr, U16 interval)
{
    bt_htpc_set_interval_value_req_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpc_set_interval_value_req_struct));
    btmtk_os_memcpy((U8 *)&msg.bt_addr, bt_addr, sizeof(msg.bt_addr));
    msg.interval = interval;

    BTCMD_SendMessage(MSG_ID_BT_HTPC_SET_INTERVAL_VALUE_REQ, MOD_EXTERNAL, &msg, sizeof(msg));
}

void htpc_op_remove_dev(BD_ADDR *bt_addr)
{
    bt_htpc_remove_dev_req_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpc_remove_dev_req_struct));
    btmtk_os_memcpy((U8 *)&msg.bt_addr, bt_addr, sizeof(msg.bt_addr));

    BTCMD_SendMessage(MSG_ID_BT_HTPC_REMOVE_DEV_REQ, MOD_EXTERNAL, &msg, sizeof(msg));
}


#endif

/*
*  Thermometer External API Definitions
*/
#ifdef __BT_HTPT_PROFILE__
void btmtk_htpt_activate_req(bt_htp_server_parm_struct *parms)
{
    bt_htp_op_struct *msg;
    U8 msg_len = sizeof(bt_htp_op_struct) + sizeof(bt_htp_server_parm_struct);

    msg = (bt_htp_op_struct *)HTP_MEM_MALLOC(msg_len);
    if (msg == NULL)
    {
        return;
    }

    btmtk_os_memset((U8 *)msg, 0, msg_len);
    msg->op = BT_HTPT_OP_ACTIVATE;
    btmtk_os_memcpy((U8 *)msg + sizeof(bt_htp_op_struct), (U8 *)parms, sizeof(bt_htp_server_parm_struct));

    bt_prompt_trace(MOD_BT, "[HTP EXT] thermometer register, interval: %d, range: %d ~ %d", parms->interval,
                parms->interval_range_min, parms->interval_range_max);

    
    htp_process_cmd(msg, msg_len);

    HTP_MEM_FREEIF(msg);
}

void btmtk_htpt_deactivate_req(void)
{
    bt_htp_op_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htp_op_struct));
    msg.op = BT_HTPT_OP_DEACTIVATE;
    htp_process_cmd(&msg, sizeof(bt_htp_op_struct));
}

void btmtk_htpt_disconnect_req(U8 *device_addr)
{
    bt_htp_op_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htp_op_struct));
    msg.op = BT_HTPT_OP_DISCONNECT;
    btmtk_os_memcpy((U8 *)&msg.addr, device_addr, sizeof(msg.addr));
    
    htp_process_cmd(&msg, sizeof(bt_htp_op_struct));
}

void btmtk_htpt_send_temperature(U8 *device_addr, bt_htp_temperature_struct *temperature)
{
    bt_htp_op_struct *msg;
    U8 msg_len = sizeof(bt_htp_op_struct) + sizeof(bt_htp_temperature_struct);

    msg = (bt_htp_op_struct *)HTP_MEM_MALLOC(msg_len);
    if (msg == NULL)
    {
        return;
    }

    btmtk_os_memset((U8 *)msg, 0, msg_len);
    msg->op = BT_HTPT_OP_SEND_TEMPERATURE;
    btmtk_os_memcpy((U8 *)&msg->addr, device_addr, sizeof(msg->addr));
    btmtk_os_memcpy((U8 *)msg + sizeof(bt_htp_op_struct), (U8 *)temperature, sizeof(bt_htp_temperature_struct));
    
    htp_process_cmd(msg, msg_len);

    HTP_MEM_FREEIF(msg);
}

void btmtk_htpt_send_intermediate_temperature(U8 *device_addr, bt_htp_temperature_struct *temperature)
{
    bt_htp_op_struct *msg;
    U8 msg_len = sizeof(bt_htp_op_struct) + sizeof(bt_htp_temperature_struct);

    msg = (bt_htp_op_struct *)HTP_MEM_MALLOC(msg_len);
    if (msg == NULL)
    {
        return;
    }

    btmtk_os_memset((U8 *)msg, 0, msg_len);
    msg->op = BT_HTPT_OP_SEND_INTERMEDIATE_TEMPERATURE;
    btmtk_os_memcpy((U8 *)&msg->addr, device_addr, sizeof(msg->addr));
    btmtk_os_memcpy((U8 *)msg + sizeof(bt_htp_op_struct), (U8 *)temperature, sizeof(bt_htp_temperature_struct));
    
    htp_process_cmd(msg, msg_len);

    HTP_MEM_FREEIF(msg);
}

void btmtk_htpt_set_interval_value_req(U8 *device_addr, U16 interval)
{
    bt_htp_op_struct *msg;
    U8 msg_len = sizeof(bt_htp_op_struct) + sizeof(U16);

    msg = (bt_htp_op_struct *)HTP_MEM_MALLOC(msg_len);
    if (msg == NULL)
    {
        return;
    }

    btmtk_os_memset((U8 *)msg, 0, msg_len);
    msg->op = BT_HTPT_OP_SET_INTERVAL_VALUE;
    btmtk_os_memcpy((U8 *)&msg->addr, device_addr, sizeof(msg->addr));
    btmtk_os_memcpy((U8 *)msg + sizeof(bt_htp_op_struct), (U8 *)&interval, sizeof(U16));
    
    htp_process_cmd(msg, msg_len);

    HTP_MEM_FREEIF(msg);
}

void btmtk_htpt_send_interval_value_indication(U8 *device_addr)
{
    bt_htp_op_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htp_op_struct));
    msg.op = BT_HTPT_OP_SEND_INTERVAL_VALUE_INDICAITON;
    btmtk_os_memcpy((U8 *)&msg.addr, device_addr, sizeof(msg.addr));
    
    htp_process_cmd(&msg, sizeof(bt_htp_op_struct));
}

void htpt_op_activate(bt_htp_server_parm_struct *parm)
{
    bt_htpt_activate_req_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpt_activate_req_struct));
    btmtk_os_memcpy((U8 *)&msg.parms, parm, sizeof(msg.parms));

    BTCMD_SendMessage(MSG_ID_BT_HTPT_ACTIVATE_REQ, MOD_EXTERNAL, &msg, sizeof(msg));
}

void htpt_op_deactivate(void)
{
    BTCMD_SendMessage(MSG_ID_BT_HTPT_DEACTIVATE_REQ, MOD_EXTERNAL, NULL, 0);
}

void htpt_op_disconnect(BD_ADDR *bt_addr)
{
    bt_htpt_disconnect_req_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpt_disconnect_req_struct));
    btmtk_os_memcpy((U8 *)&msg.bt_addr, bt_addr, sizeof(msg.bt_addr));

    BTCMD_SendMessage(MSG_ID_BT_HTPT_DISCONNECT_REQ, MOD_EXTERNAL, &msg, sizeof(msg));
}

void htpt_op_send_temperature(BD_ADDR *bt_addr, bt_htp_temperature_struct *temperature)
{
    bt_htpt_send_temperature_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpt_send_temperature_struct));
    btmtk_os_memcpy((U8 *)&msg.bt_addr, bt_addr, sizeof(msg.bt_addr));
    btmtk_os_memcpy((U8 *)&msg.temperature, temperature, sizeof(msg.temperature));

    BTCMD_SendMessage(MSG_ID_BT_HTPT_SEND_TEMPERATURE, MOD_EXTERNAL, &msg, sizeof(msg));
}

void htpt_op_send_intermediate_temperature(BD_ADDR *bt_addr, bt_htp_temperature_struct *temperature)
{
    bt_htpt_send_intermediate_temperature_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpt_send_intermediate_temperature_struct));
    btmtk_os_memcpy((U8 *)&msg.bt_addr, bt_addr, sizeof(msg.bt_addr));
    btmtk_os_memcpy((U8 *)&msg.temperature, temperature, sizeof(msg.temperature));

    BTCMD_SendMessage(MSG_ID_BT_HTPT_SEND_INTERMEDIATE_TEMPERATURE, MOD_EXTERNAL, &msg, sizeof(msg));
}

void htpt_op_set_interval(U16 interval)
{
    bt_htpt_set_interval_value_req_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpt_disconnect_req_struct));
    msg.interval = interval;

    BTCMD_SendMessage(MSG_ID_BT_HTPT_SET_INTERVAL_VALUE_REQ, MOD_EXTERNAL, &msg, sizeof(msg));
}


void htpt_op_send_interval_value_indication(BD_ADDR *bt_addr)
{
    bt_htpt_send_interval_value_indication_struct msg;

    btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpt_send_interval_value_indication_struct));
    btmtk_os_memcpy((U8 *)&msg.bt_addr, bt_addr, sizeof(msg.bt_addr));

    BTCMD_SendMessage(MSG_ID_BT_HTPT_SEND_INTERVAL_VALUE_INDICATION, MOD_EXTERNAL, &msg, sizeof(msg));
}

#endif


#ifdef BTMTK_ON_LINUX
void btmtk_htp_set_sockfd(int sockfd)
{
    g_htpSockfd = sockfd;
}

static void BTCMD_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size)
{
	int ret = -1;
	ilm_struct ilm;
	U16 *field;

	bt_ext_log("[HTP][ADP] BTCMD_SendMessage: %d", msg_id);

	memset(&ilm, 0, sizeof(ilm_struct));
	ilm.msg_id = msg_id;
	ilm.src_mod_id = MOD_EXTERNAL;
	ilm.dest_mod_id = dest_mod;

	assert (size <= MAX_ILM_BUFFER_SIZE);

	if(ptr != NULL)
		memcpy(ilm.ilm_data, ptr, size);

	/* LOCAL_PARA_HDR */
	field = (U16 *)ilm.ilm_data;
	*field = 1;
	field = (U16 *)&ilm.ilm_data[2];
	*field = size;

	// cal whole ilm size
	size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;

	if(g_htpSockfd > 0)
	{
		ret = send(g_htpSockfd, (void *)&ilm, size, 0);
		if(ret < 0)
		{
			bt_ext_err("[HTP][ADP] BTCMD_SendMessage: send fail: %s, %d", strerror(errno), errno);
		}
		bt_ext_log("[HTP][ADP] send done: %d", ret);
	}
	else
	{
		bt_ext_err("[HTP][ADP] BTCMD_SendMessage: socket not initialized");
	}
}
#endif



#endif
