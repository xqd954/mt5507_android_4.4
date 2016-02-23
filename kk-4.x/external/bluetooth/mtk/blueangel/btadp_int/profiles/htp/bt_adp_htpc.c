/*****************************************************************************
 *
 * Filename:
 * ---------
 * bt_adp_htpc.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Health Thermometer Profile Collector Role Internal Adaptation Layer.
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
 * 10 15 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_HTPC_PROFILE__
#include <assert.h>

#include "bt_os_api.h"
#include "l2cap_adp.h"
#include "htp.h"

#include "bluetooth_htp_message.h"
#include "bluetooth_htp_struct.h"
#include "bt_adp_htp_common.h"

typedef struct _htpc_context_struct
{
    BT_BOOL	bConnTerminator;
    BT_BOOL 	bActive;    // true: if connection is active
    BT_BOOL     bReconnect;
    BD_ADDR	bd_addr;
    BT_BOOL	in_use;	
    HtpChannel	*channel;
    //HtpOpFlag	flags;
} htpc_context_struct;

htpc_context_struct htpc_cntx[HTP_MAX_DEV_NUM];



static htpc_context_struct *htpc_find_free_context(void)
{
    htpc_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < HTP_MAX_DEV_NUM; i++)
    {
        if (!htpc_cntx[i].in_use)
        {
            cntx = &htpc_cntx[i];
            break;
        }
    }
    return cntx;	
}

static htpc_context_struct *htpc_find_free_context_by_addr(BD_ADDR *bt_addr_p)
{
    htpc_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < HTP_MAX_DEV_NUM; i++)
    {
        if (HTP_DEVICE_ADDR_EQUAL(&htpc_cntx[i].bd_addr, bt_addr_p) && 
             !htpc_cntx[i].in_use)
        {
            cntx = &htpc_cntx[i];
            break;
        }
    }
    return cntx;	
}

static htpc_context_struct *htpc_find_context_by_addr(BD_ADDR *bt_addr_p)
{
    htpc_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < HTP_MAX_DEV_NUM; i++)
    {
        if (HTP_DEVICE_ADDR_EQUAL(&htpc_cntx[i].bd_addr, bt_addr_p) && 
             htpc_cntx[i].in_use)
        {
            cntx = &htpc_cntx[i];
            break;
        }
    }
    return cntx;	
}

static htpc_context_struct *htpc_find_context_by_channel(HtpChannel *ch)
{
    htpc_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < HTP_MAX_DEV_NUM; i++)
    {
        if (ch == htpc_cntx[i].channel)
        {
            cntx = &htpc_cntx[i];
            break;
        }
    }
    return cntx;	
}

static void htpc_clear_connection(htpc_context_struct *cntx)
{
    cntx->bActive = FALSE;

    // free resources
    
    btmtk_os_memset((U8 *)cntx, 0, sizeof(htpc_context_struct));

}

static void htpc_reconnect()
{

}

static void htpc_construct_app_temperature(HtpTemperature *tp, bt_htp_temperature_struct *tp_ap)
{
    htp_convert_IEEE_to_float(&tp->temperature_value, &tp_ap->value);

    if (tp->flags & HTP_TEMPERATURE_FLAG_FAHRENHEIT)
    {
        tp_ap->unit_type = BT_HTP_MEASUREMENT_FAHRENHEIT;
    }
    else
    {
        tp_ap->unit_type = BT_HTP_MEASUREMENT_CELSIUS;
    }

    if (tp->flags & HTP_TEMPERATURE_FLAG_TIMESTAMP)
    {
        tp_ap->flags |= BT_HTP_TP_TIMESTAMP_FLAG;
        tp_ap->timestamp.year = tp->time_stamp.year[0] | ((tp->time_stamp.year[1] << 8) & 0xff00);
        tp_ap->timestamp.month = tp->time_stamp.month;
        tp_ap->timestamp.day = tp->time_stamp.day;
        tp_ap->timestamp.hour = tp->time_stamp.hour;
        tp_ap->timestamp.minute = tp->time_stamp.minute;
        tp_ap->timestamp.second = tp->time_stamp.second;
    }

    if (tp->flags & HTP_TEMPERATURE_FLAG_NONSTATIC)
    {
        tp_ap->flags |= BT_HTP_TP_TEMPERATURE_TYPE_FLAG;
        tp_ap->temperature_type = tp->temperature_type;
    }
}

void htpc_adp_init(void)
{
    btmtk_os_memset((U8 *)&htpc_cntx, 0, sizeof(htpc_cntx));
}

void htpc_handle_connected(HtpChannel *channel, BtRemoteDevice *dev, BtStatus status)
{
    htpc_context_struct *cntx = htpc_find_context_by_channel(channel);
    bt_htpc_connect_cnf_struct *msg_p;

    cntx->bActive = TRUE;
    cntx->bd_addr = dev->bdAddr;

    msg_p = (bt_htpc_connect_cnf_struct*) construct_local_para(sizeof(bt_htpc_connect_cnf_struct), TD_UL);
    msg_p->bt_addr = dev->bdAddr; 
    msg_p->result = TRUE;
    htp_send_msg(MSG_ID_BT_HTPC_CONNECT_CNF, MOD_EXTERNAL, (local_para_struct*)msg_p);

    HTPC_QueryGattService(channel);
}

void htpc_handle_disconnected(HtpChannel *channel, BtRemoteDevice *dev, BtStatus status)
{
    htpc_context_struct *cntx = htpc_find_context_by_channel(channel);
    L2capDiscReason discReason = status;

    if (cntx == NULL)
        return;

    /* If it is in reconnecting, it may need to try to reconnect one more time */
    if (cntx->bReconnect)
    {
        // TODO: try to reconnect again and return here
    }

    /* Check if it is not connected yet, then notify the connection failed instead of disconnection msg */
    if (cntx->bActive)
    {
        if (cntx->bConnTerminator)
        {
            bt_htpc_disconnect_cnf_struct *msg_p;
            msg_p = (bt_htpc_disconnect_cnf_struct*) construct_local_para(sizeof(bt_htpc_disconnect_cnf_struct), TD_UL);
            msg_p->bt_addr = dev->bdAddr; 
            msg_p->result = TRUE;
            htp_send_msg(MSG_ID_BT_HTPC_DISCONNECT_CNF, MOD_EXTERNAL, 
                (local_para_struct*)msg_p);
        }
        else
        {
            bt_htpc_disconnect_ind_struct *msg_p;
            msg_p = (bt_htpc_disconnect_ind_struct*) construct_local_para(sizeof(bt_htpc_disconnect_ind_struct), TD_UL);
            msg_p->bt_addr = dev->bdAddr; 
            htp_send_msg(MSG_ID_BT_HTPC_DISCONNECT_IND, MOD_EXTERNAL, 
                (local_para_struct*)msg_p);
        
            /* When a connection is terminated due to a link loss, a Collector should attempt 
            *   to reconnect to Thermometer. 
            */
            if (discReason == L2DISC_LINK_DISCON)
            {
                cntx->bReconnect = TRUE;
                // TODO: set timer to reconnect
            }
        }
        
    }
    else
    {
        bt_htpc_connect_cnf_struct *msg_p;
        msg_p = (bt_htpc_connect_cnf_struct*) construct_local_para(sizeof(bt_htpc_connect_cnf_struct), TD_UL);
        msg_p->bt_addr = cntx->bd_addr; 
        msg_p->result = FALSE;
        htp_send_msg(MSG_ID_BT_HTPC_CONNECT_CNF, MOD_EXTERNAL, (local_para_struct*)msg_p);
    }

    htpc_clear_connection(cntx);
}

void htpc_handle_gatt_query_cnf(HtpChannel *channel, HtpCharFeature feature, BtStatus status)
{
    htpc_context_struct *cntx = htpc_find_context_by_channel(channel);
    bt_htpc_search_attribute_cnf_struct *msg_p;

    msg_p = (bt_htpc_search_attribute_cnf_struct*) construct_local_para(sizeof(bt_htpc_search_attribute_cnf_struct), TD_UL);
    msg_p->bt_addr = cntx->bd_addr; 

    if (status == BT_STATUS_SUCCESS)
    {
        msg_p->result = TRUE;
        msg_p->remote_features = feature;

        HTPC_GetCharClientConfigDesciptor(channel, BT_UUID_HTS_CHAR_TEMPERATURE_MEASUREMENT, &msg_p->temperature_config);
        if (feature & HTP_CHAR_FEATURE_TEMPERATURE_TYPE_READABLE)
        {
            HTPC_GetTemperatureType(channel, (HtpTemperatureType *)&msg_p->temperature_type);
        }
        if (feature & HTP_CHAR_FEATURE_INTERMEDIATE_MEASUREMENT_NOTIFIABLE)
        {
            HTPC_GetCharClientConfigDesciptor(channel, BT_UUID_HTS_CHAR_INTERMEDIATE_TEMPERATURE, &msg_p->intermediate_temperature_config);
        }
        if (feature & HTP_CHAR_FEATURE_MEASUREMENT_INTERVAL_READABLE)
        {
            HTPC_GetMeasurementIntervalValue(channel, &msg_p->interval);
        }
        if (feature & HTP_CHAR_FEATURE_MEASUREMENT_INTERVAL_INDICATABLE)
        {
            HTPC_GetCharClientConfigDesciptor(channel, BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL, &msg_p->interval_config);
        }
        if (feature & HTP_CHAR_FEATURE_MEASUREMENT_INTERVAL_WRITABLE)
        {
            HTPC_GetMeasurementIntervalRange(channel, &msg_p->interval_range_min, &msg_p->interval_range_max);
        }
    }
    else
    {
        msg_p->result = FALSE;
    }
    htp_send_msg(MSG_ID_BT_HTPC_SEARCH_ATTRIBUTE_CNF, MOD_EXTERNAL, (local_para_struct*)msg_p);
}

void htpc_handle_set_config_cnf(HtpChannel *channel, HtpEvent event, U8 *pkt, BtStatus status)
{
    htpc_context_struct *cntx = htpc_find_context_by_channel(channel);
    bt_htpc_set_config_cnf_struct *msg_p;

    msg_p = (bt_htpc_set_config_cnf_struct*) construct_local_para(sizeof(bt_htpc_set_config_cnf_struct), TD_UL);
    msg_p->bt_addr = cntx->bd_addr; 
    msg_p->result = (status == BT_STATUS_SUCCESS) ? TRUE : FALSE;

    if (event == HTPEVENT_SET_TEMPERATURE_CONFIG_CNF)
    {
        msg_p->char_type = BT_HTP_TEMPERATURE_MEASUREMENT;
    }
    else if (event == HTPEVENT_SET_INTERMEDIATE_TEMPERATURE_CONFIG_CNF)
    {
        msg_p->char_type = BT_HTP_INTERMEDIATE_TEMPERATURE;
    }
    else if (event == HTPEVENT_SET_INTERVAL_CONFIG_CNF)
    {
        msg_p->char_type = BT_HTP_MEASUREMENT_INTERVAL;
    }

    htp_send_msg(MSG_ID_BT_HTPC_SET_CONFIG_CNF, MOD_EXTERNAL, (local_para_struct*)msg_p);

    HTP_MEM_FREEIF(pkt);
}

void htpc_handle_set_interval_cnf(HtpChannel *channel, U8 *pkt, BtStatus status)
{
    htpc_context_struct *cntx = htpc_find_context_by_channel(channel);
    bt_htpc_set_interval_value_cnf_struct *msg_p;

    msg_p = (bt_htpc_set_interval_value_cnf_struct*) construct_local_para(sizeof(bt_htpc_set_interval_value_cnf_struct), TD_UL);
    msg_p->bt_addr = cntx->bd_addr; 
    msg_p->result = (status == BT_STATUS_SUCCESS) ? TRUE : FALSE;
    htp_send_msg(MSG_ID_BT_HTPC_SET_INTERVAL_VALUE_CNF, MOD_EXTERNAL, (local_para_struct*)msg_p);

    HTP_MEM_FREEIF(pkt);
}

void htpc_handle_measurement_ind(HtpChannel *channel, HtpTemperature *temperature, U16 size)
{
    htpc_context_struct *cntx = htpc_find_context_by_channel(channel);
    bt_htpc_temperature_ind_struct *msg_p;
    bt_htp_temperature_struct *tp_ap;

    msg_p = (bt_htpc_temperature_ind_struct*) construct_local_para(sizeof(bt_htpc_temperature_ind_struct), TD_UL);
    msg_p->bt_addr = cntx->bd_addr; 

    htpc_construct_app_temperature(temperature, &msg_p->temperature);
    htp_send_msg(MSG_ID_BT_HTPC_TEMPERATURE_IND, MOD_EXTERNAL, (local_para_struct*)msg_p);
}

void htpc_handle_measurement_notify(HtpChannel *channel, HtpTemperature *temperature, U16 size)
{
    htpc_context_struct *cntx = htpc_find_context_by_channel(channel);
    bt_htpc_intermediate_temperature_ind_struct *msg_p;

    msg_p = (bt_htpc_intermediate_temperature_ind_struct*) construct_local_para(sizeof(bt_htpc_intermediate_temperature_ind_struct), TD_UL);
    msg_p->bt_addr = cntx->bd_addr; 

    htpc_construct_app_temperature(temperature, &msg_p->temperature);
    htp_send_msg(MSG_ID_BT_HTPC_INTERMEDIATE_TEMPERATURE_IND, MOD_EXTERNAL, (local_para_struct*)msg_p);
}

void htpc_handle_interval_value_ind(HtpChannel *channel, HtpMeasurementInterval interval)
{
    htpc_context_struct *cntx = htpc_find_context_by_channel(channel);
    bt_htpc_set_interval_value_ind_struct *msg_p;

    msg_p = (bt_htpc_set_interval_value_ind_struct*) construct_local_para(sizeof(bt_htpc_set_interval_value_ind_struct), TD_UL);
    msg_p->bt_addr = cntx->bd_addr; 
    msg_p->interval = interval;
    htp_send_msg(MSG_ID_BT_HTPC_SET_INTERVAL_VALUE_IND, MOD_EXTERNAL, (local_para_struct*)msg_p);
}


void htpc_adp_callback(HtpChannel *channel, HtpCallbackParms *parms)
{
    kal_trace(BT_TRACE_BLE_PROFILES, HTP_INTADP_CALLBACK, parms->event, parms->status);

    switch (parms->event)
    {
        case HTPEVENT_OPEN:
            htpc_handle_connected(channel, parms->parm.rem_dev, parms->status);		
            break;			
        case HTPEVENT_CLOSED:
            htpc_handle_disconnected(channel, parms->parm.rem_dev, parms->status);			
            break;			
        case HTPEVENT_GATT_QUERY_CNF:
            htpc_handle_gatt_query_cnf(channel, parms->parm.rem_features, parms->status);			
            break;			
        case HTPEVENT_SET_TEMPERATURE_CONFIG_CNF:
        case HTPEVENT_SET_INTERMEDIATE_TEMPERATURE_CONFIG_CNF:
        case HTPEVENT_SET_INTERVAL_CONFIG_CNF:
            htpc_handle_set_config_cnf(channel, parms->event, parms->parm.pkt_handled, parms->status);            
            break;          
        case HTPEVENT_SET_INTERVAL_CNF:
            htpc_handle_set_interval_cnf(channel, parms->parm.pkt_handled, parms->status);			
            break;			
        case HTPEVENT_MEASUREMENT_IND:
            htpc_handle_measurement_ind(channel, parms->parm.temperature, parms->len);			
            break;			
        case HTPEVENT_MEASUREMENT_NOTIFY:
            htpc_handle_measurement_notify(channel, parms->parm.temperature, parms->len);			
            break;			
        case HTPEVENT_INTERVAL_VALUE_IND:
            htpc_handle_interval_value_ind(channel, parms->parm.interval);			
            break;			
        default:
            break;			
    }

}


void btmtk_adp_htpc_handle_activate_req(ilm_struct *ilm_ptr)
{
    bt_htpc_activate_cnf_struct *msg_p;
    HtpServiceParm parm;

    msg_p = (bt_htpc_activate_cnf_struct*) construct_local_para(sizeof(bt_htpc_activate_cnf_struct), TD_UL);

    parm.callback = htpc_adp_callback;
    parm.service_flag = HTP_SERVICE_TYPE_COLLECTOR_FLAG;

    if (HTP_Register(&parm) == BT_STATUS_SUCCESS)

    {
        msg_p->result = TRUE;
    }
    else
    {
        msg_p->result = FALSE;
    }

    htp_send_msg(MSG_ID_BT_HTPC_ACTIVATE_CNF, MOD_EXTERNAL, (local_para_struct*) msg_p);
}

void btmtk_adp_htpc_handle_deactivate_req(ilm_struct *ilm_ptr)
{
    bt_htpc_deactivate_cnf_struct *msg_p;

    msg_p = (bt_htpc_deactivate_cnf_struct*) construct_local_para(sizeof(bt_htpc_deactivate_cnf_struct), TD_UL);

    if (HTP_Deregister(HTP_SERVICE_TYPE_COLLECTOR_FLAG) == BT_STATUS_SUCCESS)

    {
        msg_p->result = TRUE;
    }
    else
    {
        msg_p->result = FALSE;
    }
    htp_send_msg(MSG_ID_BT_HTPC_DEACTIVATE_CNF, MOD_EXTERNAL, (local_para_struct*) msg_p);
}

void btmtk_adp_htpc_handle_connect_req(ilm_struct *ilm_ptr)
{
    bt_htpc_connect_req_struct *req_p = (bt_htpc_connect_req_struct*) ilm_ptr->local_para_ptr;
    htpc_context_struct *cntx = htpc_find_free_context_by_addr(&req_p->bt_addr);    
    BtStatus status = BT_STATUS_FAILED;
	
    if (cntx != NULL)
    {
        // it is in reconnecting?
    }
    else
    {
        if ((cntx = htpc_find_free_context()) == NULL)
        {
            goto exit;
        }
    }

    status = HTP_Open(&cntx->channel, &req_p->bt_addr);
    if ((status == BT_STATUS_PENDING) || (status == BT_STATUS_SUCCESS))
    {
        cntx->bd_addr = req_p->bt_addr;
        cntx->in_use = TRUE;
        if (status == BT_STATUS_PENDING)
        {
            return;
        }
    }


exit:
    {
        bt_htpc_connect_cnf_struct *msg_p;
        msg_p = (bt_htpc_connect_cnf_struct*) construct_local_para(sizeof(bt_htpc_connect_cnf_struct), TD_UL);
        msg_p->bt_addr = req_p->bt_addr; 
        msg_p->result = (status == BT_STATUS_SUCCESS) ? TRUE : FALSE;
        htp_send_msg(MSG_ID_BT_HTPC_CONNECT_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
    }	
}

void btmtk_adp_htpc_handle_disconnect_req(ilm_struct *ilm_ptr)
{
    bt_htpc_disconnect_req_struct *req_p = (bt_htpc_disconnect_req_struct*) ilm_ptr->local_para_ptr;
    htpc_context_struct *cntx = htpc_find_context_by_addr(&req_p->bt_addr);    
    BtStatus status;

    if (cntx == NULL)
    {
        goto fail;
    }

    cntx->bConnTerminator = TRUE;

    /* Note that, if status is success, the callback function with close event shall be called before 
    *   HTP_Close returned.
    */
    status = HTP_Close(cntx->channel);
    if ((status == BT_STATUS_SUCCESS) || (status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS))
    {
        return;     
    }

fail:
    {
        bt_htpc_disconnect_cnf_struct *msg_p;
        msg_p = (bt_htpc_disconnect_cnf_struct*) construct_local_para(sizeof(bt_htpc_disconnect_cnf_struct), TD_UL);
        msg_p->bt_addr = req_p->bt_addr; 
        msg_p->result = FALSE;
        htp_send_msg(MSG_ID_BT_HTPC_DISCONNECT_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
    }
}

/*
void btmtk_adp_htpc_handle_search_attribute_req(ilm_struct *ilm_ptr)
{
    // Do GATT query
}
*/

void btmtk_adp_htpc_handle_get_temperature_type_req(ilm_struct *ilm_ptr)
{
    bt_htpc_get_temperature_type_req_struct *req_p = (bt_htpc_get_temperature_type_req_struct*) ilm_ptr->local_para_ptr;
    htpc_context_struct *cntx = htpc_find_context_by_addr(&req_p->bt_addr);    
    HtpTemperatureType type;
    BtStatus status;

    if (cntx == NULL)
    {
        return;
    }
    else
    {
        status= HTPC_GetTemperatureType(cntx->channel, &type);
    }
}

void btmtk_adp_htpc_handle_get_config_req(ilm_struct *ilm_ptr)
{
    bt_htpc_get_config_req_struct *req_p = (bt_htpc_get_config_req_struct*) ilm_ptr->local_para_ptr;
    htpc_context_struct *cntx = htpc_find_context_by_addr(&req_p->bt_addr);    
    U16 uuid;
    HtpClientCharConfig config;

    switch (req_p->char_type)
    {
        case BT_HTP_TEMPERATURE_MEASUREMENT:
            uuid = BT_UUID_HTS_CHAR_TEMPERATURE_MEASUREMENT;
            break;
        case BT_HTP_INTERMEDIATE_TEMPERATURE:
            uuid = BT_UUID_HTS_CHAR_INTERMEDIATE_TEMPERATURE;
            break;
        case BT_HTP_MEASUREMENT_INTERVAL:
            uuid = BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL;
            break;
        default:
            return;
    }

    HTPC_GetCharClientConfigDesciptor(cntx->channel, uuid, &config);
}

void btmtk_adp_htpc_handle_get_interval_value_req(ilm_struct *ilm_ptr)
{
    bt_htpc_get_interval_value_req_struct *req_p = (bt_htpc_get_interval_value_req_struct*) ilm_ptr->local_para_ptr;
    htpc_context_struct *cntx = htpc_find_context_by_addr(&req_p->bt_addr);    
    HtpMeasurementInterval interval;

    HTPC_GetMeasurementIntervalValue(cntx->channel, &interval);
}

void btmtk_adp_htpc_handle_get_interval_range_req(ilm_struct *ilm_ptr)
{
    bt_htpc_get_interval_range_req_struct *req_p = (bt_htpc_get_interval_range_req_struct*) ilm_ptr->local_para_ptr;
    htpc_context_struct *cntx = htpc_find_context_by_addr(&req_p->bt_addr);    
    HtpMeasurementInterval range_min;
    HtpMeasurementInterval range_max;

    HTPC_GetMeasurementIntervalRange(cntx->channel, &range_min, &range_max);
}

void btmtk_adp_htpc_handle_set_config_req(ilm_struct *ilm_ptr)
{
    bt_htpc_set_config_req_struct *req_p = (bt_htpc_set_config_req_struct *) ilm_ptr->local_para_ptr;
    htpc_context_struct *cntx = htpc_find_context_by_addr(&req_p->bt_addr);    
    U16 uuid;
    U8 *flag;
    BtStatus status;

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_INTADP_SETCLIENTCONFIG, req_p->config.char_type, req_p->config.flag);

    switch (req_p->config.char_type)
    {
        case BT_HTP_TEMPERATURE_MEASUREMENT:
            uuid = BT_UUID_HTS_CHAR_TEMPERATURE_MEASUREMENT;
            break;
        case BT_HTP_INTERMEDIATE_TEMPERATURE:
            uuid = BT_UUID_HTS_CHAR_INTERMEDIATE_TEMPERATURE;
            break;
        case BT_HTP_MEASUREMENT_INTERVAL:
            uuid = BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL;
            break;
        default:
            return;
    }

    flag = (U8 *)HTP_MEM_MALLOC(sizeof(HtpClientCharConfig));
    flag[0] = req_p->config.flag & 0xff;
    flag[1] = (req_p->config.flag & 0xff00) >> 8;

    status = HTPC_SetCharClientConfigDesciptor(cntx->channel, uuid, flag);
    if (status != BT_STATUS_PENDING)
    {
        bt_htpc_set_config_cnf_struct *msg_p;
        msg_p = (bt_htpc_set_config_cnf_struct*) construct_local_para(sizeof(bt_htpc_set_config_cnf_struct), TD_UL);
        msg_p->bt_addr = req_p->bt_addr; 
        msg_p->result = FALSE;
        msg_p->char_type = req_p->config.char_type;
        htp_send_msg(MSG_ID_BT_HTPC_SET_CONFIG_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
        HTP_MEM_FREEIF(flag);
    }
}

void btmtk_adp_htpc_handle_set_interval_value_req(ilm_struct *ilm_ptr)
{
    bt_htpc_set_interval_value_req_struct *req_p = (bt_htpc_set_interval_value_req_struct *) ilm_ptr->local_para_ptr;
    htpc_context_struct *cntx = htpc_find_context_by_addr(&req_p->bt_addr);    
    BtStatus status;
    U8 *interval;

    interval = (U8 *)HTP_MEM_MALLOC(sizeof(HtpMeasurementInterval));
    interval[0] = req_p->interval & 0xff;
    interval[1] = (req_p->interval & 0xff00) >> 8;
    
    status = HTPC_SetMeasurementInterval(cntx->channel, interval);
    if (status != BT_STATUS_PENDING)
    {
        bt_htpc_set_interval_value_cnf_struct *msg_p;
        msg_p = (bt_htpc_set_interval_value_cnf_struct*) construct_local_para(sizeof(bt_htpc_set_interval_value_cnf_struct), TD_UL);
        msg_p->bt_addr = req_p->bt_addr; 
        msg_p->result = FALSE;
        htp_send_msg(MSG_ID_BT_HTPC_SET_INTERVAL_VALUE_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
        HTP_MEM_FREEIF(interval);
    }
}
/*
void btmtk_adp_htpc_handle_remove_dev_req(ilm_struct *ilm_ptr)
{
    bt_htpc_remove_dev_req_struct *req_p = (bt_htpc_remove_dev_req_struct *) ilm_ptr->local_para_ptr;

    HTPC_DeleteDevServiceDB(&req_p->bt_addr);
}*/

void btmtk_adp_htpc_handle_message(void *msg)
{
    ilm_struct *ilm_ptr = (ilm_struct *)msg;

    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_HTPC_ACTIVATE_REQ:
            btmtk_adp_htpc_handle_activate_req(ilm_ptr);			
            break;
        case MSG_ID_BT_HTPC_DEACTIVATE_REQ:
            btmtk_adp_htpc_handle_deactivate_req(ilm_ptr);			
            break;
        case MSG_ID_BT_HTPC_CONNECT_REQ:
            btmtk_adp_htpc_handle_connect_req(ilm_ptr);			
            break;
        case MSG_ID_BT_HTPC_DISCONNECT_REQ:
            btmtk_adp_htpc_handle_disconnect_req(ilm_ptr);			
            break;
        case MSG_ID_BT_HTPC_SET_CONFIG_REQ:
            btmtk_adp_htpc_handle_set_config_req(ilm_ptr);          
            break;
        case MSG_ID_BT_HTPC_SET_INTERVAL_VALUE_REQ:
            btmtk_adp_htpc_handle_set_interval_value_req(ilm_ptr);			
            break;
        case MSG_ID_BT_HTPC_REMOVE_DEV_REQ:
            //btmtk_adp_htpc_handle_remove_dev_req(ilm_ptr);			
            break;
        default:
            break;
    }
}


#endif
