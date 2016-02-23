/*****************************************************************************
 *
 * Filename:
 * ---------
 * htp_sm.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Health Thermometer Profile.
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
#if defined(__BT_HTPT_PROFILE__) || defined(__BT_HTPC_PROFILE__)

#include "sys/htpi.h"
#include "att.h"
#include "gattcb.h"
#include "bt_adp_fs.h"

/*****************************************************************************
 * FUNCTION
 *	HtpCmgrCallback
 *
 * DESCRIPTION
 *	It is called by connection manager with link state events.
 *
 *****************************************************************************/
void HtpCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status) 
{
    HtpChannel *channel = ContainingRecord(Handler, HtpChannel, cmgr_handler);
    kal_trace(BT_TRACE_BLE_PROFILES, HTP_CMGRCALLBACK_EVENT_STATUS, Event, Status);
    switch (Event)
    {
        case CMEVENT_DATA_LINK_CON_CNF:
            if (channel == NULL)
            {
                kal_trace(BT_TRACE_BLE_PROFILES, HTP_NOT_FOR_THE_DEVICE);
                return;			
            }
    
            if (channel->cmgr_handler.bdc == 0)
            {
                kal_trace(BT_TRACE_BLE_PROFILES, HTP_CMGRCALLBACK_BDADDR0);
                return;			
            }
    
            if (Status != BT_STATUS_SUCCESS)
            {
                HtpClearConnection(channel, Status);
                return;			
            }
            else
            {
                if (channel->state == HTP_STATE_OPENING)
                {
                    BtStatus status;
                    status = GattClientConnect(channel->cmgr_handler.remDev);
                    
                    kal_trace(BT_TRACE_BLE_PROFILES, HTP_OPENGATT_STATUS, status); 
                    if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING)
                    {
                        HtpClearConnection(channel, Status);
                    }
                }
            }
            break;	
    
        default:
            break; 		
    }
}

#ifdef __BT_HTPC_PROFILE__
/*****************************************************************************
 * FUNCTION
 *  HtpcHandleTemperatureMeasurementIndication
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  Callback function registered to GATT by handle.
 *
 *****************************************************************************/
static void HtpcHandleTemperatureMeasurementIndication(HtpChannel *channel, U8 *data, U16 size)
{
    HtpTemperature temperature;

    Report(("[HTP] temperature indication, 0x%x, 0x%x, 0x%x, 0x%x", data[1],
        data[2], data[3], data[4]));

    temperature.flags = data[0];
    OS_MemCopy((U8 *)&temperature.temperature_value, data + 1, sizeof(HtpTemperatureValue));
    if (temperature.flags & HTP_TEMPERATURE_FLAG_TIMESTAMP)
    {
        OS_MemCopy((U8 *)&temperature.time_stamp, data + 1 + sizeof(HtpTemperatureValue), sizeof(HtpTimestamp));
    }
    if (temperature.flags & HTP_TEMPERATURE_FLAG_NONSTATIC)
    {
        temperature.temperature_type = data[size - 1];  // last byte
    }
    
    HtpAppCallback(channel, HTPEVENT_MEASUREMENT_IND, BT_STATUS_SUCCESS, (void *)&temperature, sizeof(temperature));          
    HtpIdleConnectionStartTimer(channel, HTP_IDLE_TIMEOUT_COLLECTOR);
}

/*****************************************************************************
 * FUNCTION
 *  HtpcIntermediateTemperatureNotification
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  Callback function registered to GATT by handle.
 *
 *****************************************************************************/
static void HtpcHandleIntermediateTemperatureNotification(HtpChannel *channel, U8 *data, U16 size)
{
    HtpTemperature temperature;

    temperature.flags = data[0];
    OS_MemCopy((U8 *)&temperature.temperature_value, data + 1, sizeof(HtpTemperatureValue));
    if (temperature.flags & HTP_TEMPERATURE_FLAG_TIMESTAMP)
    {
        OS_MemCopy((U8 *)&temperature.time_stamp, data + 1 + sizeof(HtpTemperatureValue), sizeof(HtpTimestamp));
    }
    if (temperature.flags & HTP_TEMPERATURE_FLAG_NONSTATIC)
    {
        temperature.temperature_type = data[size - 1];  // last byte
    }
    
    HtpAppCallback(channel, HTPEVENT_MEASUREMENT_NOTIFY, BT_STATUS_SUCCESS, (void *)&temperature, sizeof(temperature));          
    HtpIdleConnectionStartTimer(channel, HTP_IDLE_TIMEOUT_COLLECTOR);
}

/*****************************************************************************
 * FUNCTION
 *  HtpcIntervalIndication
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  Callback function registered to GATT by handle.
 *
 *****************************************************************************/
static void HtpcHandleIntervalIndication(HtpChannel *channel, U8 *data, U16 size)
{
    HtpMeasurementInterval interval;
    HtpCharListNode *char_node;

    interval = ((data[1] << 8) & 0xff00) | data[0];
    HtpAppCallback(channel, HTPEVENT_INTERVAL_VALUE_IND, BT_STATUS_SUCCESS, (void *)&interval, sizeof(interval));          

    HtpIdleConnectionStartTimer(channel, HTP_IDLE_TIMEOUT_COLLECTOR);

    // update in database
    char_node = HtpFindCharByUuid(&channel->servicedb->hts_service, BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL);
    OS_MemCopy(char_node->value, data, size);
}
#endif

/*****************************************************************************
 * FUNCTION
 *  HtpGattEventCallback
 *
 * DESCRIPTION
 *  It is used to handle GATT event, including BT_GATT_APP_EVENT_CONNECTED, BT_GATT_APP_EVENT_DISCONNECTED.
 *
 *****************************************************************************/
void HtpGattEventCallback(void *handler, BtGattEvent *event)
{
    HtpChannel *channel;

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_GATTEVENTCALLBACK_EVENT, event->eType);
    channel = HtpFindChannelByLink(event->link);

    switch (event->eType)
    {
        case BT_GATT_APP_EVENT_CONNECTED:
            if (channel == NULL)
            {
                if ((HTP(service_flag) & HTP_SERVICE_TYPE_THERMOMETER_FLAG) == 0)
                {
                    kal_trace(BT_TRACE_BLE_PROFILES, HTP_GATTEVENTCALLBACK_IGNORE);
                    return;
                }
                channel = HtpNewChannel();
                if (channel == NULL)		
                {
                    GattDisconnect(event->link);
                    return;					
                }
                /* Thermometer is always connection acceptor */
                channel->role = HTP_ROLE_THERMOMETER;
            }
            channel->state = HTP_STATE_OPEN;
            channel->link = event->link;
            HtpAppCallback(channel, HTPEVENT_OPEN, BT_STATUS_SUCCESS, event->link, 0);	
            break;			

        case BT_GATT_APP_EVENT_DISCONNECTED:
            if (channel == NULL)
               return;
            HtpClearConnection(channel, BT_STATUS_SUCCESS);			
            break;

#ifdef __BT_HTPC_PROFILE__
        case BT_GATT_APP_EVENT_VALUE_INDICATION:
            {
                BtGattValueIndicationEvent *parm = event->parms;
                HtpServiceDB *servicedb;
                HtpCharListNode *char_node;

                if (channel == NULL)
                {
                    return;
                }
                servicedb = channel->servicedb;
                if ((parm->attributeHandle < servicedb->hts_service.start_handle) ||
                    (parm->attributeHandle > servicedb->hts_service.end_handle))
                {
                    // ignore the packet
                    kal_trace(BT_TRACE_BLE_PROFILES, HTP_GATTEVENTCALLBACK_INDICATION_IGNORE, parm->attributeHandle, 
                                                                servicedb->hts_service.start_handle, servicedb->hts_service.end_handle);
                    break;
                }
                kal_trace(BT_TRACE_BLE_PROFILES, HTP_GATTEVENTCALLBACK_INDICATION_HANDLE, parm->attributeHandle);
                char_node = HtpFindCharbyHandle(&servicedb->hts_service, parm->attributeHandle);
                if (char_node->value_uuid == BT_UUID_HTS_CHAR_TEMPERATURE_MEASUREMENT)
                {
                    HtpcHandleTemperatureMeasurementIndication(channel, parm->attributeValue, parm->attributeValueLen);
                }
                else if (char_node->value_uuid == BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL)
                {
                    HtpcHandleIntervalIndication(channel, parm->attributeValue, parm->attributeValueLen);
                }
            }
            break;

        case BT_GATT_APP_EVENT_VALUE_NOTIFICATION:
            {
                BtGattValueNotificationEvent *parm = event->parms;
                HtpServiceDB *servicedb;

                if (channel == NULL)
                {
                    return;
                }
                servicedb = channel->servicedb;
                if ((parm->attributeHandle < servicedb->hts_service.start_handle) ||
                    (parm->attributeHandle > servicedb->hts_service.end_handle))
                {
                    // ignore the packet
                    break;
                }
                kal_trace(BT_TRACE_BLE_PROFILES, HTP_GATTEVENTCALLBACK_NOTIFICAITON_HANDLE, parm->attributeHandle);
                HtpcHandleIntermediateTemperatureNotification(channel, parm->attributeValue, parm->attributeValueLen);
            }
            break;
#endif /* __BT_HTPC_PROFILE__ */

        default:
            kal_trace(BT_TRACE_BLE_PROFILES, HTP_GATTEVENTCALLBACK_UNEXPECTED_EVENT);			
            break;
    }

}

void HtpBondingCallback(U8 status, U8 *bd_addr)
{
    switch (status)
    {
        case ATT_SEC_DEVICE_BOND:
            /* Device Bonded or Bonded again*/
            /*{
                HtpChannel *channel;
                if (((channel = HtpFindChannelByAddr((BD_ADDR *)bd_addr)) != NULL) &&
                    (channel->servicedb != NULL))
                {
                    HtpRemoveServiceDB((BD_ADDR *)bd_addr);     // in case that it is bonded again without unbond procedure
                    HtpWriteServiceDB(channel->servicedb);
                }
            }*/
            break;
        case ATT_SEC_DEVICE_UNBOND:
            /* Device Un Bonded */
            HtpRemoveServiceDB((BD_ADDR *)bd_addr);
            break;
        case ATT_SEC_DEVICE_ALL_UNBOND:
            /* All Device Un Bonded */
            btmtk_fs_delete((const U8 *)HTP_SERVICE_DATABASE_FILE);
            break;
    }
}

#endif
