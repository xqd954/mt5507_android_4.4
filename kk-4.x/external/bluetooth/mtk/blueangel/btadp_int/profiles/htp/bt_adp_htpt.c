/*****************************************************************************
 *
 * Filename:
 * ---------
 * bt_adp_htpt.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Health Thermometer Profile Thermometer Role Internal Adaptation Layer.
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
#ifdef __BT_HTPT_PROFILE__
#include <assert.h>

#include "bt_os_api.h"
	 
#include "htp.h"

#include "bluetooth_htp_message.h"
#include "bluetooth_htp_struct.h"
#include "bt_adp_htp_common.h"

/*
*   According to Date Time Characteristic Minimum and Maximum Value definition
*   Year:   1582~9999
*   Month:  1~12
*   Day:    1~31
*   Hour:   0~23
*   Minute: 0~59
*   Second: 0~59
*/
#define HTP_CHECK_TIMESTAMP_VALID(year, month, day, hour, minute, second) \
                    (((year) >= 1582) && ((year) <= 9999) && \
                        ((month) >= 1) && ((month) <= 12) && \
                        ((day) >= 1) && ((day) <= 31) && \
                        ((hour) <= 23) && \
                        ((minute) <= 59) && \
                        ((second) <= 59)) 


typedef struct _htpt_context_struct
{
    BT_BOOL	bConnTerminator;
    BT_BOOL 	bActive;    // true: if connection is active
    BD_ADDR	bd_addr;
    HtpChannel	*channel;
} htpt_context_struct;

htpt_context_struct htpt_cntx;

static void htpt_clear_connection(void)
{

    btmtk_os_memset((U8 *)&htpt_cntx, 0, sizeof(htpt_context_struct));
}

static U16 htpt_construct_temperature(bt_htp_temperature_struct *tp_ap, HtpTemperature *tp)
{
    U16 tp_len = sizeof(HtpTemperatureFlag) + sizeof(HtpTemperatureValue);

    htp_convert_float_to_IEEE(tp_ap->value, &tp->temperature_value);

    if (tp_ap->unit_type == BT_HTP_MEASUREMENT_FAHRENHEIT)
    {
        tp->flags |= HTP_TEMPERATURE_FLAG_FAHRENHEIT;
    }

    if (tp_ap->flags & BT_HTP_TP_TIMESTAMP_FLAG)
    {
        if (HTP_CHECK_TIMESTAMP_VALID(tp_ap->timestamp.year, 
                                        tp_ap->timestamp.month,
                                        tp_ap->timestamp.day,
                                        tp_ap->timestamp.hour,
                                        tp_ap->timestamp.minute,
                                        tp_ap->timestamp.second))
        {
            tp_len += sizeof(HtpTimestamp);
            tp->flags |= HTP_TEMPERATURE_FLAG_TIMESTAMP;
            tp->time_stamp.year[0] = tp_ap->timestamp.year & 0xff;
            tp->time_stamp.year[1] = (tp_ap->timestamp.year & 0xff00) >> 8;
            tp->time_stamp.month = tp_ap->timestamp.month;
            tp->time_stamp.day = tp_ap->timestamp.day;
            tp->time_stamp.hour = tp_ap->timestamp.hour;
            tp->time_stamp.minute = tp_ap->timestamp.minute;
            tp->time_stamp.second = tp_ap->timestamp.second;
        }
    }

    if (tp_ap->flags & BT_HTP_TP_TEMPERATURE_TYPE_FLAG)
    {
        if ((tp_ap->temperature_type > BT_HTP_TEMPERATURE_TYPE_UNKNOWN) &&
            (tp_ap->temperature_type < BT_HTP_TEMPERATURE_TYPE_MAX))
        {
            tp_len += sizeof(HtpTemperatureType);
            tp->flags |= HTP_TEMPERATURE_FLAG_NONSTATIC;
            tp->temperature_type = tp_ap->temperature_type;
        }
    }

    return tp_len;
}

void htpt_adp_init(void)
{
    btmtk_os_memset((U8 *)&htpt_cntx, 0, sizeof(htpt_context_struct));

}

void htpt_handle_connected(HtpChannel *channel, BtRemoteDevice *dev, BtStatus status)
{
    bt_htpt_connect_ind_struct *msg_p;

    htpt_cntx.bActive = TRUE;
    htpt_cntx.bd_addr = dev->bdAddr;
    htpt_cntx.channel = channel;

    msg_p = (bt_htpt_connect_ind_struct*) construct_local_para(sizeof(bt_htpt_connect_ind_struct), TD_UL);
    msg_p->bt_addr = dev->bdAddr; 
    msg_p->temperature_config = HTPT_GetCharClientConfigDesciptor(channel, BT_UUID_HTS_CHAR_TEMPERATURE_MEASUREMENT);
    msg_p->intermediate_temperature_config = HTPT_GetCharClientConfigDesciptor(channel, BT_UUID_HTS_CHAR_INTERMEDIATE_TEMPERATURE);
    msg_p->interval_config = HTPT_GetCharClientConfigDesciptor(channel, BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL);
    htp_send_msg(MSG_ID_BT_HTPT_CONNECT_IND, MOD_EXTERNAL, (local_para_struct*)msg_p);
}

void htpt_handle_disconnected(HtpChannel *channel, BtRemoteDevice *dev, BtStatus status)
{
    /* Check if it is not connected yet, then notify the connection failed instead of disconnection msg */
    if (htpt_cntx.bConnTerminator)
    {
        bt_htpt_disconnect_cnf_struct *msg_p;
        msg_p = (bt_htpt_disconnect_cnf_struct*) construct_local_para(sizeof(bt_htpt_disconnect_cnf_struct), TD_UL);
        msg_p->bt_addr = dev->bdAddr; 
        msg_p->result = TRUE;
        htp_send_msg(MSG_ID_BT_HTPT_DISCONNECT_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
    }
    else
    {
        bt_htpt_disconnect_ind_struct *msg_p;
        msg_p = (bt_htpt_disconnect_ind_struct*) construct_local_para(sizeof(bt_htpt_disconnect_ind_struct), TD_UL);
        msg_p->bt_addr = dev->bdAddr; 
        htp_send_msg(MSG_ID_BT_HTPT_DISCONNECT_IND, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
    }
    
    htpt_clear_connection();

}

void htpt_handle_set_config_ind(HtpChannel *channel, HtpEvent event, HtpClientCharConfig flag)
{
    bt_htpt_set_config_ind_struct *msg_p;

    msg_p = (bt_htpt_set_config_ind_struct *) construct_local_para(sizeof(bt_htpt_set_config_ind_struct), TD_UL);
    msg_p->bt_addr = htpt_cntx.bd_addr; 
    msg_p->config.flag = flag;

    if (event == HTPEVENT_SET_TEMPERATURE_CONFIG_CNF)
    {
        msg_p->config.char_type = BT_HTP_TEMPERATURE_MEASUREMENT;
    }
    else if (event == HTPEVENT_SET_INTERMEDIATE_TEMPERATURE_CONFIG_CNF)
    {
        msg_p->config.char_type = BT_HTP_INTERMEDIATE_TEMPERATURE;
    }
    else if (event == HTPEVENT_SET_INTERVAL_CONFIG_CNF)
    {
        msg_p->config.char_type = BT_HTP_MEASUREMENT_INTERVAL;
    }

    htp_send_msg(MSG_ID_BT_HTPT_SET_CONFIG_IND, MOD_EXTERNAL, (local_para_struct*)msg_p);

}

void htpt_handle_set_interval_ind(HtpChannel *channel, HtpMeasurementInterval interval)
{
    bt_htpt_set_interval_value_ind_struct *msg_p;

    msg_p = (bt_htpt_set_interval_value_ind_struct *) construct_local_para(sizeof(bt_htpt_set_interval_value_ind_struct), TD_UL);
    msg_p->bt_addr = htpt_cntx.bd_addr; 
    msg_p->interval = interval;

    htp_send_msg(MSG_ID_BT_HTPT_SET_INTERVAL_VALUE_IND, MOD_EXTERNAL, (local_para_struct*)msg_p);
}

void htpt_handle_measurement_indication_cnf(HtpChannel *channel, U8 *pkt, BtStatus status)
{
    bt_htpt_send_temperature_cnf_struct *msg_p;

    msg_p = (bt_htpt_send_temperature_cnf_struct*) construct_local_para(sizeof(bt_htpt_send_temperature_cnf_struct), TD_UL);
    msg_p->bt_addr = htpt_cntx.bd_addr; 
    msg_p->result = (status == BT_STATUS_SUCCESS) ? TRUE : FALSE;
    htp_send_msg(MSG_ID_BT_HTPT_SEND_TEMPERATURE_CNF, MOD_EXTERNAL, (local_para_struct*)msg_p);

    HTP_MEM_FREEIF(pkt);
}

void htpt_handle_measurement_notification_cnf(HtpChannel *channel, U8 *pkt, BtStatus status)
{
    bt_htpt_send_intermediate_temperature_cnf_struct *msg_p;

    msg_p = (bt_htpt_send_intermediate_temperature_cnf_struct*) construct_local_para(sizeof(bt_htpt_send_intermediate_temperature_cnf_struct), TD_UL);
    msg_p->bt_addr = htpt_cntx.bd_addr; 
    msg_p->result = (status == BT_STATUS_SUCCESS) ? TRUE : FALSE;
    htp_send_msg(MSG_ID_BT_HTPT_SEND_INTERMEDIATE_TEMPERATURE_CNF, MOD_EXTERNAL, (local_para_struct*)msg_p);

    HTP_MEM_FREEIF(pkt);
}

void htpt_handle_interval_value_indication_cnf(HtpChannel *channel, BtStatus status)
{
    bt_htpt_send_interval_value_indication_cnf_struct *msg_p;

    msg_p = (bt_htpt_send_interval_value_indication_cnf_struct*) construct_local_para(sizeof(bt_htpt_send_interval_value_indication_cnf_struct), TD_UL);
    msg_p->bt_addr = htpt_cntx.bd_addr; 
    msg_p->result = (status == BT_STATUS_SUCCESS) ? TRUE : FALSE;
    htp_send_msg(MSG_ID_BT_HTPT_SEND_INTERVAL_VALUE_INDICATION_CNF, MOD_EXTERNAL, (local_para_struct*)msg_p);
}

void htpt_adp_callback(HtpChannel *channel, HtpCallbackParms *parms)
{
    Report(("[HTP ADP] callback event: 0x%x ", parms->event));
    switch (parms->event)
    {
        case HTPEVENT_OPEN:
            htpt_handle_connected(channel, parms->parm.rem_dev, parms->status);		
            break;			
        case HTPEVENT_CLOSED:
            htpt_handle_disconnected(channel, parms->parm.rem_dev, parms->status);			
            break;			
        case HTPEVENT_SET_TEMPERATURE_CONFIG_REQ:
        case HTPEVENT_SET_INTERMEDIATE_TEMPERATURE_CONFIG_REQ:
        case HTPEVENT_SET_INTERVAL_CONFIG_REQ:
            htpt_handle_set_config_ind(channel, parms->event, parms->parm.flag);			
            break;			
        case HTPEVENT_SET_INTERVAL_REQ:
            htpt_handle_set_interval_ind(channel, parms->parm.interval);			
            break;			
        case HTPEVENT_MEASUREMENT_IND_CNF:
            htpt_handle_measurement_indication_cnf(channel, parms->parm.pkt_handled, parms->status);			
            break;			
        case HTPEVENT_MEASUREMENT_NOTIFY_CNF:
            htpt_handle_measurement_notification_cnf(channel, parms->parm.pkt_handled, parms->status);			
            break;			
        case HTPEVENT_INTERVAL_VALUE_IND_CNF:
            htpt_handle_interval_value_indication_cnf(channel, parms->status);			
            break;	
        default:
            break;			
    }
}

void btmtk_adp_htpt_handle_activate_req(ilm_struct *ilm_ptr)
{
    bt_htpt_activate_req_struct *req_p = (bt_htpt_activate_req_struct*) ilm_ptr->local_para_ptr; 
    bt_htpt_activate_cnf_struct *msg_p;
    HtpServiceParm parm;

    msg_p = (bt_htpt_activate_cnf_struct*) construct_local_para(sizeof(bt_htpt_activate_cnf_struct), TD_UL);

    bt_prompt_trace(MOD_BT, "[HTP] thermometer register, interval: %d, range: %d ~ %d", req_p->parms.interval,
                        req_p->parms.interval_range_min, req_p->parms.interval_range_max);

    btmtk_os_memcpy((U8 *)&parm, 0, sizeof(HtpServiceParm));

#if HTP_MEASUREMENT_INTERVAL == XA_ENABLED
    if ((req_p->parms.interval_range_min <= req_p->parms.interval) &&
        (req_p->parms.interval <= req_p->parms.interval_range_max))
    {
        parm.server_parms.interval = req_p->parms.interval;
        
        parm.server_parms.interval_range_min = req_p->parms.interval_range_min;
        parm.server_parms.interval_range_max = req_p->parms.interval_range_max;
    }
    else
    {
        // invalid parameter
        msg_p->result = FALSE;
        goto exit;
    }
#endif

#if HTP_TEMPERATURE_TYPE == XA_ENABLED
    if ((req_p->parms.temperature_type > BT_HTP_TEMPERATURE_TYPE_UNKNOWN) &&
        (req_p->parms.temperature_type < BT_HTP_TEMPERATURE_TYPE_MAX))
    {
        parm.server_parms.temperature_type = req_p->parms.temperature_type;
    }
    else
    {
        // set default value
        parm.server_parms.temperature_type = BT_HTP_TEMPERATURE_TYPE_BODY;
    }
#endif
    
    parm.callback = htpt_adp_callback;
    parm.service_flag = HTP_SERVICE_TYPE_THERMOMETER_FLAG;

    if (HTP_Register(&parm) == BT_STATUS_SUCCESS)
    {
        msg_p->result = TRUE;
    }
    else
    {
        msg_p->result = FALSE;
    }

    bt_prompt_trace(MOD_BT, "[HTP] register result: %d", msg_p->result);

exit:
    htp_send_msg(MSG_ID_BT_HTPT_ACTIVATE_CNF, MOD_EXTERNAL, (local_para_struct*) msg_p);
}

void btmtk_adp_htpt_handle_deactivate_req(ilm_struct *ilm_ptr)
{
    bt_htpt_deactivate_cnf_struct *msg_p;

    msg_p = (bt_htpt_deactivate_cnf_struct*) construct_local_para(sizeof(bt_htpt_deactivate_cnf_struct), TD_UL);

    if (HTP_Deregister(HTP_SERVICE_TYPE_THERMOMETER_FLAG) == BT_STATUS_SUCCESS)
    {
        msg_p->result = TRUE;
    }
    else
    {
        msg_p->result = FALSE;
    }
    htp_send_msg(MSG_ID_BT_HTPT_DEACTIVATE_CNF, MOD_EXTERNAL, (local_para_struct*) msg_p);
}

void btmtk_adp_htpt_handle_disconnect_req(ilm_struct *ilm_ptr)
{
    bt_htpt_disconnect_req_struct *req_p = (bt_htpt_disconnect_req_struct*) ilm_ptr->local_para_ptr;
    BtStatus status;

    htpt_cntx.bConnTerminator = TRUE;

    status = HTP_Close(htpt_cntx.channel);
    if ((status == BT_STATUS_SUCCESS) || (status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS))
    {
        return;
    }
    else
    {
        bt_htpt_disconnect_cnf_struct *msg_p;
        msg_p = (bt_htpt_disconnect_cnf_struct*) construct_local_para(sizeof(bt_htpt_disconnect_cnf_struct), TD_UL);
        msg_p->bt_addr = req_p->bt_addr; 
        msg_p->result = FALSE;
        htp_send_msg(MSG_ID_BT_HTPT_DISCONNECT_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
    }
}

/* send temperature measurement indication */
void btmtk_adp_htpt_handle_send_temperature(ilm_struct *ilm_ptr)
{
    bt_htpt_send_temperature_struct *req_p = (bt_htpt_send_temperature_struct *) ilm_ptr->local_para_ptr;
    U16 tp_len;
    HtpTemperature *tp;
    BtStatus status;


    tp = (HtpTemperature *)HTP_MEM_MALLOC(sizeof(HtpTemperature));
    btmtk_os_memset((U8 *)tp, 0, sizeof(HtpTemperature));

    tp_len = htpt_construct_temperature(&req_p->temperature, tp);

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_INTADP_SEND_TEMPERATURE, tp->flags, tp_len);
    kal_trace(BT_TRACE_BLE_PROFILES, HTP_INTADP_SEND_TEMPERATURE_VALUE, tp->temperature_value.exponent,
                                tp->temperature_value.mantissa[0], tp->temperature_value.mantissa[1], tp->temperature_value.mantissa[2]);
    
    status = HTPT_SendTemperatureMeasurement(htpt_cntx.channel, tp, tp_len);
    if (status == BT_STATUS_PENDING)
    {
        return;
    }
    else if (status == BT_STATUS_NO_RESOURCES)
    {
        // TODO: packet queue
    }
    else
    {
        bt_htpt_send_temperature_cnf_struct *msg_p;
        msg_p = (bt_htpt_send_temperature_cnf_struct*) construct_local_para(sizeof(bt_htpt_send_temperature_cnf_struct), TD_UL);
        msg_p->bt_addr = req_p->bt_addr; 
        msg_p->result = FALSE;
        htp_send_msg(MSG_ID_BT_HTPT_SEND_TEMPERATURE_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
        
        HTP_MEM_FREEIF(tp);
    }
}

void btmtk_adp_htpt_handle_send_intermediate_temperature(ilm_struct *ilm_ptr)
{
    bt_htpt_send_intermediate_temperature_struct *req_p = (bt_htpt_send_intermediate_temperature_struct *) ilm_ptr->local_para_ptr;
    U16 tp_len;
    HtpTemperature *tp;
    BtStatus status;

    tp = (HtpTemperature *)HTP_MEM_MALLOC(sizeof(HtpTemperature));
    btmtk_os_memset((U8 *)tp, 0, sizeof(HtpTemperature));

    tp_len = htpt_construct_temperature(&req_p->temperature, tp);
    
    status = HTPT_SendIntermediateTemperature(htpt_cntx.channel, tp, tp_len);
    if (status == BT_STATUS_PENDING)
    {
        return;
    }
    else if (status == BT_STATUS_NO_RESOURCES)
    {
        // TODO: packet queue
    }
    else
    {
        bt_htpt_send_intermediate_temperature_cnf_struct *msg_p;
        msg_p = (bt_htpt_send_intermediate_temperature_cnf_struct*) construct_local_para(sizeof(bt_htpt_send_intermediate_temperature_cnf_struct), TD_UL);
        msg_p->bt_addr = req_p->bt_addr; 
        msg_p->result = FALSE;
        htp_send_msg(MSG_ID_BT_HTPT_SEND_INTERMEDIATE_TEMPERATURE_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
        
        HTP_MEM_FREEIF(tp);
    }

}

void btmtk_adp_htpt_handle_set_interval_value_req(ilm_struct *ilm_ptr)
{
    bt_htpt_set_interval_value_req_struct *req_p = (bt_htpt_set_interval_value_req_struct *) ilm_ptr->local_para_ptr;
    bt_htpt_set_interval_value_cnf_struct *msg_p;
    BtStatus status;

    status = HTPT_SetMeasurementInterval(req_p->interval);

    /* It is synchronized cnf */
    msg_p = (bt_htpt_set_interval_value_cnf_struct*) construct_local_para(sizeof(bt_htpt_set_interval_value_cnf_struct), TD_UL);
    msg_p->result = (status == BT_STATUS_SUCCESS) ? TRUE : FALSE;
    htp_send_msg(MSG_ID_BT_HTPT_SET_INTERVAL_VALUE_CNF, MOD_EXTERNAL, 
        (local_para_struct*)msg_p);
}

void btmtk_adp_htpt_handle_send_interval_value_indication(ilm_struct *ilm_ptr)
{
    bt_htpt_send_interval_value_indication_struct *req_p = (bt_htpt_send_interval_value_indication_struct *) ilm_ptr->local_para_ptr;
    BtStatus status;

    status = HTPT_SendMeasurementIntervalIndication(htpt_cntx.channel);

    if (status == BT_STATUS_PENDING)
    {
        return;
    }
    else
    {
        bt_htpt_send_interval_value_indication_cnf_struct *msg_p;
        msg_p = (bt_htpt_send_interval_value_indication_cnf_struct*) construct_local_para(sizeof(bt_htpt_send_interval_value_indication_cnf_struct), TD_UL);
        msg_p->bt_addr = req_p->bt_addr; 
        msg_p->result = FALSE;
        htp_send_msg(MSG_ID_BT_HTPT_SEND_INTERVAL_VALUE_INDICATION_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
    }
}


void btmtk_adp_htpt_handle_message(void *msg)
{
    ilm_struct *ilm_ptr = (ilm_struct *)msg;

    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_HTPT_ACTIVATE_REQ:
            btmtk_adp_htpt_handle_activate_req(ilm_ptr);			
            break;
        case MSG_ID_BT_HTPT_DEACTIVATE_REQ:
            btmtk_adp_htpt_handle_deactivate_req(ilm_ptr);			
            break;
        case MSG_ID_BT_HTPT_DISCONNECT_REQ:
            btmtk_adp_htpt_handle_disconnect_req(ilm_ptr);			
            break;
        case MSG_ID_BT_HTPT_SEND_TEMPERATURE:
            btmtk_adp_htpt_handle_send_temperature(ilm_ptr);			
            break;
        case MSG_ID_BT_HTPT_SEND_INTERMEDIATE_TEMPERATURE:
            btmtk_adp_htpt_handle_send_intermediate_temperature(ilm_ptr);			
            break;
        case MSG_ID_BT_HTPT_SET_INTERVAL_VALUE_REQ:
            btmtk_adp_htpt_handle_set_interval_value_req(ilm_ptr);			
            break;
        case MSG_ID_BT_HTPT_SEND_INTERVAL_VALUE_INDICATION:
            btmtk_adp_htpt_handle_send_interval_value_indication(ilm_ptr);
        default:
            break;
    }
}

	 
#endif
