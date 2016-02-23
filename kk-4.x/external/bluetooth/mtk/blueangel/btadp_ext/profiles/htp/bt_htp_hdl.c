/*****************************************************************************
 *
 * Filename:
 * ---------
 * bt_htp_hdl.c
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

#include "bt_struct.h"
#include "bt_message.h"
#include "ext_os_api.h"

#include "bluetooth_htp_message.h"
#include "bluetooth_htp_struct.h"

#include "bt_htp_api.h"
#include "bt_htp_struct.h"

 BT_HTPC_APP_CTX g_htpc_cntx;
 BT_HTPT_APP_CTX g_htpt_cntx;


BT_HTP_DEVICE *htp_find_device_by_addr(U8 *device_addr)
{
    BT_HTP_DEVICE *dev = NULL;
    U8 i = 0;

    for (i = 0; i < HTP_MAX_DEV_NUM; i++)
    {
        if (HTP_DEVICE_ADDR_EQUAL((U8 *)&g_htpc_cntx.device[i].addr, device_addr)) 
        {
            dev = &g_htpc_cntx.device[i];
            break;
        }
    }
    return dev;	
}

BT_HTP_DEVICE *htp_find_free_device(U8 *device_addr)
{
    BT_HTP_DEVICE *dev = NULL;
    U8 i = 0;

    if ((dev = htp_find_device_by_addr(device_addr)) != NULL)
    {
        // if it is already in the device list, just pick up
        return dev;
    }
    
    for (i = 0; i < HTP_MAX_DEV_NUM; i++)
    {
        if (g_htpc_cntx.device[i].state == BT_HTP_APP_STAT_NO_CONNECTION)
        {
            dev = &g_htpc_cntx.device[i];
            break;			
        }
    }

    return dev;	
}

BT_HTP_DEVICE *htp_find_connected_device(void)
{
    BT_HTP_DEVICE *dev = NULL;
    U8 i = 0;

    for (i = 0; i < HTP_MAX_DEV_NUM; i++)
    {
        if (g_htpc_cntx.device[i].state != BT_HTP_APP_STAT_NO_CONNECTION)
        {
            dev = &g_htpc_cntx.device[i];
        }
    }
	
    return dev;
}

void htp_set_state(BT_HTP_DEVICE *dev, BT_HTP_APP_STATE state)
{
    if (dev == NULL)
    {
        return;
    }
    dev->state = state;
}


/*
*  Collector  Message Handling API Definitions
*/
#ifdef __BT_HTPC_PROFILE__
static void htpc_init(void)
{
    btmtk_os_memset((U8 *)&g_htpc_cntx, 0, sizeof(g_htpc_cntx));
}

void htpc_deactivate(void)
{
    BT_HTP_DEVICE *dev_conn = NULL;
	
    /* disconnect all the other devices before disable the service */
    if ((dev_conn = htp_find_connected_device()) != NULL)
    {
        if (dev_conn->state == BT_HTP_APP_STAT_CONNECTED)
        {
            // deactivate later, after the connection is disconnected
            htpc_op_disconnect(&dev_conn->addr);
            htp_set_state(dev_conn, BT_HTP_APP_STAT_DISCONNECTING);
        }
        else 
        {
            // deactivate later, after the connection is done or disconnection is done
            // BT_HTP_APP_STAT_CONNECTING
            // BT_HTP_APP_STAT_DISCONNECTING
            // BT_HTP_APP_STAT_CONNCANCEL
        }
    }
    else
    {
        htpc_op_deactivate();
    }
}

static void btmtk_htpc_handle_activate_cnf(void *msg)
{
    bt_htpc_activate_cnf_struct *msg_p = (bt_htpc_activate_cnf_struct *)msg;

    htpc_init();

    if (msg_p->result)
    {
        g_htpc_cntx.enabled = TRUE;
    }
    g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_ACTIVATE_CNF, msg, sizeof(bt_htpc_activate_cnf_struct));
}

static void btmtk_htpc_handle_deactivate_cnf(void *msg)
{
    bt_htpc_deactivate_cnf_struct *msg_p = (bt_htpc_deactivate_cnf_struct *)msg;

    g_htpc_cntx.do_disable = FALSE;

    if (msg_p->result)
    {
        g_htpc_cntx.enabled = FALSE;
    }
    g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_DEACTIVATE_CNF, msg, sizeof(bt_htpc_deactivate_cnf_struct));
}

static void btmtk_htpc_handle_connect_cnf(void *msg)
{
    bt_htpc_connect_cnf_struct *msg_p = (bt_htpc_connect_cnf_struct *)msg;
    BT_HTP_DEVICE *dev = htp_find_device_by_addr((U8 *)&msg_p->bt_addr);

    if (dev == NULL)
    {
        return;
    }

    if ((g_htpc_cntx.do_disable) ||
         (dev->state == BT_HTP_APP_STAT_CONN_CANCEL))
    {
        htpc_op_disconnect(&msg_p->bt_addr);
        htp_set_state(dev, BT_HTP_APP_STAT_DISCONNECTING);
    }
    else
    {
        if (msg_p->result)
        {
            htp_set_state(dev, BT_HTP_APP_STAT_CONNECTED);
        }
        else
        {
            htp_set_state(dev, BT_HTP_APP_STAT_NO_CONNECTION);
        }
        g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_CONNECT_CNF, msg, sizeof(bt_htpc_connect_cnf_struct));
    }
}

static void btmtk_htpc_handle_search_attribute_cnf(void *msg)
{
    g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_SEARCH_ATTIBUTE_CNF, msg, sizeof(bt_htpc_search_attribute_cnf_struct));
}

static void btmtk_htpc_handle_disconnect_cnf(void *msg)
{
    bt_htpc_disconnect_cnf_struct *msg_p = (bt_htpc_disconnect_cnf_struct *)msg;
    BT_HTP_DEVICE *dev = htp_find_device_by_addr((U8 *)&msg_p->bt_addr);

    if (dev == NULL)
    {
        return;
    }
	
    if (g_htpc_cntx.do_disable)
    {
        htp_set_state(dev, BT_HTP_APP_STAT_NO_CONNECTION);
        htpc_deactivate();
    }
    else
    {
        if (msg_p->result)
        {
            htp_set_state(dev, BT_HTP_APP_STAT_NO_CONNECTION);
        }
        else
        {
            htp_set_state(dev, BT_HTP_APP_STAT_CONNECTED);
        }
        g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_DISCONNECT_CNF, msg, sizeof(bt_htpc_disconnect_cnf_struct));
    }
}

static void btmtk_htpc_handle_set_config_cnf(void *msg)
{
    g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_SET_CONFIG_CNF, msg, sizeof(bt_htpc_set_config_cnf_struct));
}

static void btmtk_htpc_handle_set_interval_value_cnf(void *msg)
{
    g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_SET_INTERVAL_VALUE_CNF, msg, sizeof(bt_htpc_set_interval_value_cnf_struct));
}

static void btmtk_htpc_handle_disconnect_ind(void *msg)
{
    bt_htpc_disconnect_ind_struct *msg_p = (bt_htpc_disconnect_ind_struct *)msg;
    BT_HTP_DEVICE *dev = htp_find_device_by_addr((U8 *)&msg_p->bt_addr);

    if (dev == NULL)
    {
        return;
    }
	
    if (g_htpc_cntx.do_disable)
    {
        htp_set_state(dev, BT_HTP_APP_STAT_NO_CONNECTION);
        htpc_deactivate();
    }
    else
    {
        htp_set_state(dev, BT_HTP_APP_STAT_NO_CONNECTION);
        g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_DISCONNECT_IND, msg, sizeof(bt_htpc_disconnect_ind_struct));
    }
}

static void btmtk_htpc_handle_temperature_ind(void *msg)
{
    g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_TEMPERATURE_IND, msg, sizeof(bt_htpc_temperature_ind_struct));
}

static void btmtk_htpc_handle_intermediate_temperature_ind(void *msg)
{
    g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_INTERMEDIATE_TEMPERATURE_IND, msg, sizeof(bt_htpc_intermediate_temperature_ind_struct));
}

static void btmtk_htpc_handle_set_interval_value_ind(void *msg)
{
    g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_SET_INTERVAL_VALUE_IND, msg, sizeof(bt_htpc_set_interval_value_ind_struct));
}
#endif

/*
*  Thermometer Message Handling API Definitions
*/
#ifdef __BT_HTPT_PROFILE__
static void htpt_init(void)
{
    btmtk_os_memset((U8 *)&g_htpt_cntx, 0, sizeof(g_htpt_cntx));
}

void htpt_deactivate(void)
{
    if (g_htpt_cntx.device.state == BT_HTP_APP_STAT_NO_CONNECTION)
    {
        htpt_op_deactivate();
    }
    else if (g_htpt_cntx.device.state == BT_HTP_APP_STAT_CONNECTED)
    {
        // deactivate later, after the connection is disconnected
        htpt_op_disconnect(&g_htpt_cntx.device.addr);
        htp_set_state(&g_htpt_cntx.device, BT_HTP_APP_STAT_DISCONNECTING);
    }
    else 
    {
        // deactivate later, after the connection is done or disconnection is done
        // BT_HTP_APP_STAT_CONNECTING
        // BT_HTP_APP_STAT_DISCONNECTING
        // BT_HTP_APP_STAT_CONNCANCEL
    }
}

static void btmtk_htpt_handle_activate_cnf(void *msg)
{
    bt_htpt_activate_cnf_struct *msg_p = (bt_htpt_activate_cnf_struct *)msg;

    htpt_init();

    if (msg_p->result)
    {
        g_htpt_cntx.enabled = TRUE;
    }
    g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_ACTIVATE_CNF, msg, sizeof(bt_htpt_activate_cnf_struct));
}

static void btmtk_htpt_handle_deactivate_cnf(void *msg)
{
    bt_htpt_deactivate_cnf_struct *msg_p = (bt_htpt_deactivate_cnf_struct *)msg;

    g_htpt_cntx.do_disable = FALSE;

    if (msg_p->result)
    {
        g_htpt_cntx.enabled = FALSE;
    }
    g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_DEACTIVATE_CNF, msg, sizeof(bt_htpt_deactivate_cnf_struct));
}

static void btmtk_htpt_handle_disconnect_cnf(void *msg)
{
    bt_htpt_disconnect_cnf_struct *msg_p = (bt_htpt_disconnect_cnf_struct *)msg;

    if (g_htpt_cntx.do_disable)
    {
        htp_set_state(&g_htpt_cntx.device, BT_HTP_APP_STAT_NO_CONNECTION);
        htpt_deactivate();
    }
    else
    {
        if (msg_p->result)
        {
            htp_set_state(&g_htpt_cntx.device, BT_HTP_APP_STAT_NO_CONNECTION);
        }
        else
        {
            htp_set_state(&g_htpt_cntx.device, BT_HTP_APP_STAT_CONNECTED);
        }
        g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_DISCONNECT_CNF, msg, sizeof(bt_htpt_disconnect_cnf_struct));
    }
}

static void btmtk_htpt_handle_send_temperature_cnf(void *msg)
{
    g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_SEND_TEMPERATURE_CNF, msg, sizeof(bt_htpt_send_temperature_cnf_struct));
}

static void btmtk_htpt_handle_send_intermediate_temperature_cnf(void *msg)
{
    g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_SEND_INTERMEDIATE_TEMPERATURE_CNF, msg, 
                        sizeof(bt_htpt_send_intermediate_temperature_cnf_struct));
}

static void btmtk_htpt_handle_set_interval_value_cnf(void *msg)
{
    g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_SET_INTERVAL_VALUE_CNF, msg, sizeof(bt_htpt_set_interval_value_cnf_struct));
}

static void btmtk_htpt_handle_send_interval_value_indication_cnf(void *msg)
{
    g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_SEND_INTERVAL_VALUE_INDICATION_CNF, 
                        msg, sizeof(bt_htpt_send_interval_value_indication_cnf_struct));
}

static void btmtk_htpt_handle_connect_ind(void *msg)
{
    bt_htpt_connect_ind_struct *msg_p = (bt_htpt_connect_ind_struct *)msg;

    g_htpt_cntx.device.addr = msg_p->bt_addr;

    if (!g_htpt_cntx.do_disable)
    {
        htp_set_state(&g_htpt_cntx.device, BT_HTP_APP_STAT_CONNECTED);
        g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_CONNECT_IND, msg, sizeof(bt_htpt_connect_ind_struct));
    }
    else
    {
        htpt_op_disconnect(&msg_p->bt_addr);
        htp_set_state(&g_htpt_cntx.device, BT_HTP_APP_STAT_DISCONNECTING);
    }
}

static void btmtk_htpt_handle_disconnect_ind(void *msg)
{
    bt_htpt_disconnect_ind_struct *msg_p = (bt_htpt_disconnect_ind_struct *)msg;

    if (g_htpt_cntx.do_disable)
    {
        htp_set_state(&g_htpt_cntx.device, BT_HTP_APP_STAT_NO_CONNECTION);
        htpt_deactivate();
    }
    else
    {
        htp_set_state(&g_htpt_cntx.device, BT_HTP_APP_STAT_NO_CONNECTION);
        g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_DISCONNECT_IND, msg, sizeof(bt_htpt_disconnect_ind_struct));
    }

}

static void btmtk_htpt_handle_set_config_ind(void *msg)
{
    g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_SET_CONFIG_IND, msg, sizeof(bt_htpt_set_config_ind_struct));
}

static void btmtk_htpt_handle_set_interval_value_ind(void *msg)
{
    g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_SET_INTERVAL_VALUE_IND, msg, sizeof(bt_htpt_set_interval_value_ind_struct));
}
#endif

void btmtk_htp_handle_message(ilm_struct *msg)
{
    void *msg_p = NULL;  //point to local parameter in ilm_struct

#ifdef BTMTK_ON_WISE
    msg_p = (void *)msg->local_para_ptr;
#else 
    msg_p = (void *)msg->ilm_data;
#endif

    switch (msg->msg_id)
    {
#ifdef __BT_HTP_CMD_AGENT__
        case MSG_ID_BT_HTP_CMD:
            htp_handle_cmd((bt_htp_op_struct *)msg_p);
            break;			
#endif

        /* Collector message handling */
#ifdef __BT_HTPC_PROFILE__
        case MSG_ID_BT_HTPC_ACTIVATE_CNF:
            btmtk_htpc_handle_activate_cnf(msg_p);			
            break;			
        case MSG_ID_BT_HTPC_DEACTIVATE_CNF:
            btmtk_htpc_handle_deactivate_cnf(msg_p);			
            break;			
        case MSG_ID_BT_HTPC_CONNECT_CNF:
            btmtk_htpc_handle_connect_cnf(msg_p);			
            break;			
        case MSG_ID_BT_HTPC_SEARCH_ATTRIBUTE_CNF:
            btmtk_htpc_handle_search_attribute_cnf(msg_p);           
            break;          
        case MSG_ID_BT_HTPC_DISCONNECT_CNF:
            btmtk_htpc_handle_disconnect_cnf(msg_p);			
            break;			
        case MSG_ID_BT_HTPC_SET_CONFIG_CNF:
            btmtk_htpc_handle_set_config_cnf(msg_p);            
            break;          
        case MSG_ID_BT_HTPC_SET_INTERVAL_VALUE_CNF:
            btmtk_htpc_handle_set_interval_value_cnf(msg_p);			
            break;			
        case MSG_ID_BT_HTPC_DISCONNECT_IND:
            btmtk_htpc_handle_disconnect_ind(msg_p);			
            break;			
        case MSG_ID_BT_HTPC_TEMPERATURE_IND:
            btmtk_htpc_handle_temperature_ind(msg_p);			
            break;			
        case MSG_ID_BT_HTPC_INTERMEDIATE_TEMPERATURE_IND:
            btmtk_htpc_handle_intermediate_temperature_ind(msg_p);			
            break;			
        case MSG_ID_BT_HTPC_SET_INTERVAL_VALUE_IND:
            btmtk_htpc_handle_set_interval_value_ind(msg_p);			
            break;			
#endif

        /* Thermometer message handling */
#ifdef __BT_HTPT_PROFILE__
        case MSG_ID_BT_HTPT_ACTIVATE_CNF:
            btmtk_htpt_handle_activate_cnf(msg_p);			
            break;			
        case MSG_ID_BT_HTPT_DEACTIVATE_CNF:
            btmtk_htpt_handle_deactivate_cnf(msg_p);			
            break;			
        case MSG_ID_BT_HTPT_DISCONNECT_CNF:
            btmtk_htpt_handle_disconnect_cnf(msg_p);			
            break;			
        case MSG_ID_BT_HTPT_SEND_TEMPERATURE_CNF:
            btmtk_htpt_handle_send_temperature_cnf(msg_p);			
            break;			
        case MSG_ID_BT_HTPT_SEND_INTERMEDIATE_TEMPERATURE_CNF:
            btmtk_htpt_handle_send_intermediate_temperature_cnf(msg_p);			
            break;			
        case MSG_ID_BT_HTPT_SET_INTERVAL_VALUE_CNF:
            btmtk_htpt_handle_set_interval_value_cnf(msg_p);			
            break;	
        case MSG_ID_BT_HTPT_SEND_INTERVAL_VALUE_INDICATION_CNF:
            btmtk_htpt_handle_send_interval_value_indication_cnf(msg_p);            
            break;  
        case MSG_ID_BT_HTPT_CONNECT_IND:
            btmtk_htpt_handle_connect_ind(msg_p);			
            break;			
        case MSG_ID_BT_HTPT_DISCONNECT_IND:
            btmtk_htpt_handle_disconnect_ind(msg_p);			
            break;			
        case MSG_ID_BT_HTPT_SET_CONFIG_IND:
            btmtk_htpt_handle_set_config_ind(msg_p);            
            break;          
        case MSG_ID_BT_HTPT_SET_INTERVAL_VALUE_IND:
            btmtk_htpt_handle_set_interval_value_ind(msg_p);			
            break;			
#endif
        default:			
            break;			
    }
}

#ifdef __BT_HTPC_PROFILE__

void htpc_handle_activate_cmd(void)
{
    if (!g_htpc_cntx.enabled)
    {
        htpc_op_activate();
    }
    else 
    {
        bt_htpc_activate_cnf_struct msg;
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpc_activate_cnf_struct));		
        msg.result = TRUE;
        g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_ACTIVATE_CNF, &msg, sizeof(bt_htpc_activate_cnf_struct));
    }
}

static void htpc_handle_deactivate_cmd(void)
{
    if (g_htpc_cntx.enabled)
    {
        g_htpc_cntx.do_disable = TRUE;
        htpc_deactivate();
    }
    else
    {
        bt_htpc_deactivate_cnf_struct msg;
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpc_deactivate_cnf_struct));		
        msg.result = TRUE;
        g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_DEACTIVATE_CNF, &msg, sizeof(bt_htpc_deactivate_cnf_struct));
    }
}

static void htpc_handle_connect_cmd(BD_ADDR *bt_addr)
{
    BT_HTP_DEVICE *dev = htp_find_free_device(bt_addr->addr);

    if (!g_htpc_cntx.enabled || (dev == NULL))
    {
        bt_htpc_connect_cnf_struct msg;
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpc_connect_cnf_struct));		
        msg.bt_addr = *bt_addr;
        msg.result = FALSE;
        g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_CONNECT_CNF, &msg, sizeof(bt_htpc_connect_cnf_struct));
        return;				
    }
	
    if (dev->state == BT_HTP_APP_STAT_NO_CONNECTION)
    {
        htpc_op_connect(bt_addr);
        dev->addr = *bt_addr;
        htp_set_state(dev, BT_HTP_APP_STAT_CONNECTING);
    }
    else
    {
        // do nothing, ignore the request
        kal_trace(BT_TRACE_BLE_PROFILES, HTP_EXT_ADP_HANDLE_CONNECT_CMD, (U8)dev->state);
    }
}

static void htpc_handle_disconnect_cmd(BD_ADDR *bt_addr)
{
    BT_HTP_DEVICE *dev = htp_find_device_by_addr(bt_addr->addr);
	
    if (!g_htpc_cntx.enabled || (dev == NULL))
    {
        bt_htpc_disconnect_cnf_struct msg;
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpc_disconnect_cnf_struct));		
        msg.bt_addr = *bt_addr;
        msg.result = FALSE;
        g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_DISCONNECT_CNF, &msg, sizeof(bt_htpc_disconnect_cnf_struct));
        return;				
    }
	
    if (dev->state == BT_HTP_APP_STAT_CONNECTED)
    {
        htpc_op_disconnect(bt_addr);
        htp_set_state(dev, BT_HTP_APP_STAT_DISCONNECTING);
    }
    else if (dev->state == BT_HTP_APP_STAT_CONNECTING)	
    {
        htp_set_state(dev, BT_HTP_APP_STAT_CONN_CANCEL);
    }
    else
    {
        //do nothing, ignore the request
        kal_trace(BT_TRACE_BLE_PROFILES, HTP_EXT_ADP_HANDLE_DISCONNECT_CMD, (U8)dev->state);
    }
}

static void htpc_handle_set_config_cmd(BD_ADDR *bt_addr, bt_htp_desc_config_struct *parm)
{
    BT_HTP_DEVICE *dev = htp_find_device_by_addr(bt_addr->addr);

    if (!g_htpc_cntx.enabled || (dev == NULL))
    {
        goto failed;				
    }

    if (dev->state != BT_HTP_APP_STAT_CONNECTED)
    {
        goto failed;
    }

    htpc_op_set_config(bt_addr, parm);
    return;

failed:
    {
        bt_htpc_set_config_cnf_struct msg;
        bt_htp_desc_config_struct *config = (bt_htp_desc_config_struct *)parm;

        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpc_set_config_cnf_struct));		
        msg.bt_addr = *bt_addr;
        msg.result = FALSE;
        msg.char_type = config->char_type;
        g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_SET_CONFIG_CNF, &msg, sizeof(bt_htpc_set_config_cnf_struct));
    }

}

static void htpc_handle_set_interval_value_cmd(BD_ADDR *bt_addr, U16 interval)
{
    BT_HTP_DEVICE *dev = htp_find_device_by_addr(bt_addr->addr);

    if (!g_htpc_cntx.enabled || (dev == NULL))
    {
        goto failed;                
    }

    if (dev->state != BT_HTP_APP_STAT_CONNECTED)
    {
        goto failed;
    }

    htpc_op_set_interval(bt_addr, interval);
    return;

failed:
    {
        bt_htpc_set_interval_value_cnf_struct msg;

        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpc_set_interval_value_cnf_struct));      
        msg.bt_addr = *bt_addr;
        msg.result = FALSE;
        g_HTP_MMI_Callback(NULL, EVENT_HTPC_MMI_SET_INTERVAL_VALUE_CNF, &msg, sizeof(bt_htpc_set_interval_value_cnf_struct));
    }
}

static void htpc_handle_remove_dev_cmd(BD_ADDR *bt_addr)
{
    // Unnecessary to check whether the service is ready or device is connected 
    htpc_op_remove_dev(bt_addr);
}
#endif

#ifdef __BT_HTPT_PROFILE__

static void htpt_handle_activate_cmd(bt_htp_server_parm_struct *parm)
{
    if (!g_htpt_cntx.enabled)
    {
        htpt_op_activate(parm);
    }
    else 
    {
        bt_htpt_activate_cnf_struct msg;
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpt_activate_cnf_struct));		
        msg.result = TRUE;
        g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_ACTIVATE_CNF, &msg, sizeof(bt_htpt_activate_cnf_struct));
    }
}

static void htpt_handle_deactivate_cmd(void)
{
    if (g_htpt_cntx.enabled)
    {
        g_htpt_cntx.do_disable = TRUE;
        htpt_deactivate();
    }
    else
    {
        bt_htpt_deactivate_cnf_struct msg;
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpt_deactivate_cnf_struct));		
        msg.result = TRUE;
        g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_DEACTIVATE_CNF, &msg, sizeof(bt_htpt_deactivate_cnf_struct));
    }
}

static void htpt_handle_disconnect_cmd(BD_ADDR *bt_addr)
{
    if (!g_htpt_cntx.enabled)
    {
        bt_htpt_disconnect_cnf_struct msg;
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpt_disconnect_cnf_struct));		
        msg.bt_addr = *bt_addr;
        msg.result = FALSE;
        g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_DISCONNECT_CNF, &msg, sizeof(bt_htpt_disconnect_cnf_struct));
        return;				
    }
	
    if (g_htpt_cntx.device.state == BT_HTP_APP_STAT_CONNECTED)
    {
        htpt_op_disconnect(bt_addr);
        htp_set_state(&g_htpt_cntx.device, BT_HTP_APP_STAT_DISCONNECTING);
    }
    else if (g_htpt_cntx.device.state == BT_HTP_APP_STAT_CONNECTING)	
    {
        htp_set_state(&g_htpt_cntx.device, BT_HTP_APP_STAT_CONN_CANCEL);
    }
    else
    {
        //do nothing, ignore the request
        kal_trace(BT_TRACE_BLE_PROFILES, HTP_EXT_ADP_HANDLE_DISCONNECT_CMD, (U8)g_htpt_cntx.device.state);
    }
}

static void htpt_handle_send_temperature_cmd(BD_ADDR *bt_addr, bt_htp_temperature_struct *parm)
{
    if (!g_htpt_cntx.enabled || (g_htpt_cntx.device.state != BT_HTP_APP_STAT_CONNECTED))
    {
        bt_htpt_send_temperature_cnf_struct msg;
    
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpt_send_temperature_cnf_struct));      
        msg.bt_addr = *bt_addr;
        msg.result = FALSE;
        g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_SEND_TEMPERATURE_CNF, &msg, sizeof(bt_htpt_send_temperature_cnf_struct));
    }
    else
    {
        htpt_op_send_temperature(bt_addr, parm);
    }
}

static void htpt_handle_send_intermediate_temperature_cmd(BD_ADDR *bt_addr, bt_htp_temperature_struct *parm)
{
    if (!g_htpt_cntx.enabled || (g_htpt_cntx.device.state != BT_HTP_APP_STAT_CONNECTED))
    {
        bt_htpt_send_intermediate_temperature_cnf_struct msg;
    
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpt_send_intermediate_temperature_cnf_struct));      
        msg.bt_addr = *bt_addr;
        msg.result = FALSE;
        g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_SEND_INTERMEDIATE_TEMPERATURE_CNF, 
                            &msg, sizeof(bt_htpt_send_intermediate_temperature_cnf_struct));
    }
    else
    {
        htpt_op_send_intermediate_temperature(bt_addr, parm);
    }
}

static void htpt_handle_set_interval_value_cmd(U16 interval)
{
    if (!g_htpt_cntx.enabled || (g_htpt_cntx.device.state != BT_HTP_APP_STAT_CONNECTED))
    {
        bt_htpt_set_interval_value_cnf_struct msg;
    
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpt_set_interval_value_cnf_struct));      
        msg.result = FALSE;
        g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_SET_INTERVAL_VALUE_CNF, &msg, sizeof(bt_htpt_set_interval_value_cnf_struct));
    }
    else
    {
        htpt_op_set_interval(interval);
    }
}

static void htpt_handle_send_interval_value_indication_cmd(BD_ADDR *bt_addr)
{
    if (!g_htpt_cntx.enabled || (g_htpt_cntx.device.state != BT_HTP_APP_STAT_CONNECTED))
    {
        bt_htpt_send_interval_value_indication_cnf_struct msg;
    
        btmtk_os_memset((U8 *)&msg, 0, sizeof(bt_htpt_send_interval_value_indication_cnf_struct));      
        msg.bt_addr = *bt_addr;
        msg.result = FALSE;
        g_HTP_MMI_Callback(NULL, EVENT_HTPT_MMI_SEND_INTERVAL_VALUE_INDICATION_CNF, 
                                &msg, sizeof(bt_htpt_send_interval_value_indication_cnf_struct));
    }
    else
    {
        htpt_op_send_interval_value_indication(bt_addr);
    }
}
#endif

void htp_handle_cmd(bt_htp_op_struct *cmd, U32 size)
{
    U8 *parm = (U8 *)cmd + sizeof(bt_htp_op_struct);
    
    //kal_trace(BT_TRACE_BLE_PROFILES, HTP_EXT_ADP_HANDLE_CMD, cmd->op, size);
    bt_prompt_trace(MOD_BT, "[HTP EXT] handle command, op: %d, len: %d", cmd->op, size);

    switch (cmd->op)
    {
#ifdef __BT_HTPC_PROFILE__
        case BT_HTPC_OP_ACTIVATE:
            htpc_handle_activate_cmd();
            break;	
        case BT_HTPC_OP_DEACTIVATE:
            htpc_handle_deactivate_cmd();
            break;	
        case BT_HTPC_OP_CONNECT:
            htpc_handle_connect_cmd(&cmd->addr);
            break;	
        case BT_HTPC_OP_DISCONNECT:
            htpc_handle_disconnect_cmd(&cmd->addr);
            break;			
        case BT_HTPC_OP_SET_CONFIG:
            htpc_handle_set_config_cmd(&cmd->addr, (bt_htp_desc_config_struct *)parm);
            break;	
        case BT_HTPC_OP_SET_INTERVAL_VALUE:
            htpc_handle_set_interval_value_cmd(&cmd->addr, *(U16 *)parm);
            break;			
        case BT_HTPC_OP_REMOVE_DEV:
            htpc_handle_remove_dev_cmd(&cmd->addr);
            break;  
#endif            
#ifdef __BT_HTPT_PROFILE__
        case BT_HTPT_OP_ACTIVATE:
            htpt_handle_activate_cmd((bt_htp_server_parm_struct *)parm);
            break;  
        case BT_HTPT_OP_DEACTIVATE:
            htpt_handle_deactivate_cmd();
            break;  
        case BT_HTPT_OP_DISCONNECT:
            htpt_handle_disconnect_cmd(&cmd->addr);
            break;  
        case BT_HTPT_OP_SEND_TEMPERATURE:
            htpt_handle_send_temperature_cmd(&cmd->addr, (bt_htp_temperature_struct *)parm);
            break;  
        case BT_HTPT_OP_SEND_INTERMEDIATE_TEMPERATURE:
            htpt_handle_send_intermediate_temperature_cmd(&cmd->addr, (bt_htp_temperature_struct *)parm);
            break;  
        case BT_HTPT_OP_SET_INTERVAL_VALUE:
            htpt_handle_set_interval_value_cmd(*(U16 *)parm);
            break;  
        case BT_HTPT_OP_SEND_INTERVAL_VALUE_INDICAITON:
            htpt_handle_send_interval_value_indication_cmd(&cmd->addr);
            break;  
#endif
        default:
            break;			
    }
}
#endif  // __BT_HTPC_PROFILE__,  __BT_HTPT_PROFILE__
