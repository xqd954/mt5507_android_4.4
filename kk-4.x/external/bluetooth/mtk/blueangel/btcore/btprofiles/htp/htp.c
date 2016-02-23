/*****************************************************************************
 *
 * Filename:
 * ---------
 * htp.c
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
#include "attdbmain.h"
#include "attdb.h"
#include "sec.h"
#include "htsservice.h"

#if XA_CONTEXT_PTR == XA_ENABLED
static HtpContext      tempContext;
HtpContext          *htpcx = &tempContext;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
HtpContext           htpcx;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */


extern U8 HtpAttDB_Registration(HtpThermometerParm *params);
extern U8 HtpAttDB_Deregistration(void);


/*****************************************************************************
 * FUNCTION
 *  HTP_Init
 *
 * DESCRIPTION
 *  Initialize the context of HTP.
 *
 *****************************************************************************/
void HTP_Init(void)
{
    OS_LockStack();

#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8 *)htpcx, 0, sizeof(HtpContext));
#else /* XA_CONTEXT_PTR == XA_ENABLED */
    OS_MemSet((U8 *)&htpcx, 0, sizeof(HtpContext));
#endif /* XA_CONTEXT_PTR == XA_ENABLED */
    
    /* Initialize Lists */
    InitializeListHead(&HTP(dev_list));

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_INITIALIZED);
    
    OS_UnlockStack();

}

/*****************************************************************************
 * FUNCTION
 *  HTP_Register
 *
 * DESCRIPTION
 *  Collector: 
 *        Register application callback function.
 *  Thermometer: 
 *        Register GATT service record and callback function for both GATT and application.
 *
 *****************************************************************************/
BtStatus HTP_Register(HtpServiceParm *service)
{
    BtStatus status = BT_STATUS_FAILED;

    OS_LockStack();

    if ((service->service_flag & HTP_SERVICE_TYPE_COLLECTOR_FLAG) &&
	((HTP(service_flag) & HTP_SERVICE_TYPE_COLLECTOR_FLAG) == 0))
    {
        HTP(htpc_callback) = service->callback;
        status = BT_STATUS_SUCCESS;		
    }

    if ((service->service_flag & HTP_SERVICE_TYPE_THERMOMETER_FLAG) &&
	((HTP(service_flag) & HTP_SERVICE_TYPE_THERMOMETER_FLAG) == 0))
    {
        status = HtpAttDB_Registration(&service->server_parms);
        if (status == BT_STATUS_SUCCESS)
        {
            HTP(htpt_callback) = service->callback;
            //HTP(server_parms) = service->server_parms;
        }
    }

    if (status == BT_STATUS_SUCCESS)
    {
        if (HTP(service_flag) == 0)
        {
            /* register the GATT callback to handle connection */
            BT_GattRegisterConnectedEvent(&(HTP(gatt_handler)), HtpGattEventCallback);

            /* register bond callback to obtain bond status */
            HTP(bond_handler).callback = HtpBondingCallback;
            ATTDB_RegisterBondStatus(&HTP(bond_handler));
        }
        HTP(service_flag) |= service->service_flag;
    }

    OS_UnlockStack();
    return status;	
}

/*****************************************************************************
 * FUNCTION
 *  HTP_Deregister
 *
 * DESCRIPTION
 *  Collector: 
 *        Deregister application callback function.
 *  Thermometer: 
 *        Deregister GATT service record and callback function.
 *
 *****************************************************************************/
BtStatus HTP_Deregister(HtpServiceTypeFlag service_flag)
{
    BtStatus status = BT_STATUS_SUCCESS;

    OS_LockStack();

    if (HTP(service_flag) == 0)
    {
        /* It is actually invalid deregistration, return directly */
        return status;
    }

    if ((service_flag & HTP_SERVICE_TYPE_COLLECTOR_FLAG) &&
	(HTP(service_flag) & HTP_SERVICE_TYPE_COLLECTOR_FLAG))
    {
        HTP(htpc_callback) = NULL;
        HTP(service_flag) &= ~HTP_SERVICE_TYPE_COLLECTOR_FLAG;
    }
	
    if ((service_flag & HTP_SERVICE_TYPE_THERMOMETER_FLAG) &&
	(HTP(service_flag) & HTP_SERVICE_TYPE_THERMOMETER_FLAG))
    {
        status = HtpAttDB_Deregistration();
        if (status == BT_STATUS_SUCCESS)
        {
            HTP(htpt_callback) = NULL;
            HTP(service_flag) &= ~HTP_SERVICE_TYPE_THERMOMETER_FLAG;
        }
    }

	
    if (HTP(service_flag) == 0)
    {
        BT_GattUnregisterConnectedEvent(&HTP(gatt_handler));
        ATTDB_UnRegisterBondStatus(&HTP(bond_handler));        
    }

    OS_UnlockStack();
    return status;	
}

/*****************************************************************************
 * FUNCTION
 *  HTP_Open
 *
 * DESCRIPTION
 *  Connect ATT bearer. Since Collector acts as GAP Central and Thermometer acts as
 *  Peripherial role, it is actually only available for Collector.
 *
 *****************************************************************************/
BtStatus HTP_Open(HtpChannel **channel, BD_ADDR *addr)
{
    BtStatus status = BT_STATUS_FAILED;
    HtpChannel *ch;	

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, addr != 0);

    OS_LockStack();

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_OPENCONNECTION);
    if ((*channel != NULL) && IsNodeOnList(&HTP(dev_list), &((*channel)->node)))
    {
        ch = *channel;
    }
    else
    {
        ch = HtpNewChannel();
        if (ch == NULL)
        {
            return BT_STATUS_NO_RESOURCES;
        }
    }

    Report(("[HTP] htp open, channel: %x, handler: %x", ch, &ch->cmgr_handler));

    if (ch->state == HTP_STATE_CLOSED)
    {
        status = CMGR_CreateDataLink(&ch->cmgr_handler, addr);
//        status = CMGR_CreateBLEDataLink(&ch->cmgr_handler, addr);

        kal_trace(BT_TRACE_BLE_PROFILES, HTP_CREATEDATALINK_STATUS, status);
        if (status == BT_STATUS_SUCCESS)
        {
            status = GattClientConnect(ch->cmgr_handler.remDev);
            kal_trace(BT_TRACE_BLE_PROFILES, HTP_OPENGATT_STATUS, status); 
        }


        if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING)
        {
            HtpFreeChannel(ch);
            return status;			
        }

        ch->state = HTP_STATE_OPENING;
        ch->role = HTP_ROLE_COLLECTOR;		
        *channel = ch;		
    }

    OS_UnlockStack();
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HTP_Close
 *
 * DESCRIPTION
 *  Disconnect ATT bearer for both Collector and Thermometer.
 *
 *****************************************************************************/
BtStatus HTP_Close(HtpChannel *channel)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, channel != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, 
                    IsNodeOnList(&HTP(dev_list), &(channel->node)));

    // check state
    if (channel->state == HTP_STATE_OPEN)
    {
        status = GattDisconnect(channel->link);
    }

    if (status == BT_STATUS_SUCCESS)
    {
        HtpClearConnection(channel, status);
    }

    OS_UnlockStack();
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HTPC_QueryGattService
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *  After ATT bearer is established, try to initiate primary service and 
 *  characteristic discovery procedure and read procedure. Cache the 
 *  query result for the latest device.
 *
 *****************************************************************************/
void HTPC_QueryGattService(HtpChannel *channel)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtDeviceRecord record;

    OS_LockStack();
    
    /* 1st: check whether it is bonded device. If it is removed from bonded device
    *   list, remove it from database, and query service again 
    */
    if (SEC_FindDeviceRecord(&channel->link->bdAddr, &record) == BT_STATUS_SUCCESS) // bonded device
    {
        /* 2nd: check service database first */
        if (HtpReadServiceDB(&channel->link->bdAddr, &channel->servicedb) == BT_STATUS_SUCCESS)
        {
            /* 3rd: read HTP characteristic and descriptors by handle */
            HtpcReadCharacteristicEx(channel, (HtpCharListNode *) GetHeadList(&channel->servicedb->hts_service.char_list), 
                                        &channel->servicedb->hts_service, channel->servicedb);
            return;
        }
    }
    else  // unbonded device
    {
        HtpRemoveServiceDB(&channel->link->bdAddr);
    }

    if ((channel->servicedb = HtpNewServiceDB(&channel->link->bdAddr)) == NULL)
    {
        HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, BT_STATUS_NO_RESOURCES, 0, 0);
        return;		
    }


    kal_trace(BT_TRACE_BLE_PROFILES, HTP_QUEREYGATTSERVICE);
    /* search HTS service first, and search DIS service when all HTS attributes are obtained */
    status = HtpcFindPrimaryServicesByUuid(channel, BT_UUID_GATT_SERVICE_HEALTH, 0x0001, 0xffff);

    if (status != BT_STATUS_PENDING && status !=  BT_STATUS_SUCCESS)
    {
        HtpFreeServiceDB(channel->servicedb);
        channel->servicedb = NULL;
        HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, status, 0, 0);
    }

    OS_UnlockStack();
}

/*****************************************************************************
 * FUNCTION
 *  HTPC_GetTemperatureType
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *
 *****************************************************************************/
BtStatus HTPC_GetTemperatureType(HtpChannel *channel, HtpTemperatureType *type)
{
    HtpServiceDB *servicedb;
    HtpCharListNode *char_node;
    BtStatus status = BT_STATUS_FAILED;

    servicedb = channel->servicedb;
    if (servicedb != NULL)
    {
        char_node = HtpFindCharByUuid(&servicedb->hts_service, BT_UUID_HTS_CHAR_TEMPERATURE_TYPE);
        *type = *(char_node->value);
        status = BT_STATUS_SUCCESS;
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HTPC_GetCharClientConfigDesciptor
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *
 *****************************************************************************/
BtStatus HTPC_GetCharClientConfigDesciptor(HtpChannel *channel, U16 char_uuid, HtpClientCharConfig *flag)
{
    HtpServiceDB *servicedb;
    HtpCharDescListNode *desc_node;
    U8 *config;

    /* get description handle from database */
    servicedb = channel->servicedb;
    if (servicedb == NULL)
    {
        return BT_STATUS_FAILED;
    }

    desc_node = HtpFindCharDescByUuid(&servicedb->hts_service, char_uuid, BT_UUID_GATT_CHAR_DESC_CLIENT_CHAR_CONFIG);	
    if (desc_node == NULL)
    {
        return BT_STATUS_FAILED;
    }

    config = desc_node->desc;
    *flag = (config[0] & 0xff) | (config[1] << 8 & 0xff00);

    Report(("[HTP] get client config, handle, 0x%x, %d, %d", desc_node->handle, config[0], config[1]));
    return BT_STATUS_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  HTPC_GetMeasurementIntervalValue
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *
 *****************************************************************************/
BtStatus HTPC_GetMeasurementIntervalValue(HtpChannel *channel, HtpMeasurementInterval *interval)
{
    HtpServiceDB *servicedb;
    HtpCharListNode *char_node;
    BtStatus status = BT_STATUS_FAILED;

    servicedb = channel->servicedb;
    if (servicedb != NULL)
    {
        char_node = HtpFindCharByUuid(&servicedb->hts_service, BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL);
        *interval = (char_node->value)[0] | ((char_node->value)[1] << 8 & 0xff00);
        status = BT_STATUS_SUCCESS;
    }

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HTPC_GetMeasurementIntervalValue
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *
 *****************************************************************************/
BtStatus HTPC_GetMeasurementIntervalRange(HtpChannel *channel, HtpMeasurementInterval *interval_min, HtpMeasurementInterval *interval_max)
{
    HtpServiceDB *servicedb;
    HtpCharDescListNode *desc_node;
    BtStatus status = BT_STATUS_FAILED;
    U8 *value;

    servicedb = channel->servicedb;
    if (servicedb != NULL)
    {
        desc_node = HtpFindCharDescByUuid(&servicedb->hts_service, BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL, 
                                            BT_UUID_HTS_CHAR_DESC_VALID_RANGE);   
        value = desc_node->desc;
        *interval_min = value[0] | (value[1] << 8 & 0xff00);
        *interval_max = value[2] | (value[3] << 8 & 0xff00);
        
        status = BT_STATUS_SUCCESS;
    }

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HTPC_SetCharClientConfigDesciptor
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *  Write indication flag (0 or 1) in Client Characteristic Configuration
 *  Descriptor for Temperature Measurement.
 *
 *****************************************************************************/
BtStatus HTPC_SetCharClientConfigDesciptor(HtpChannel *channel, U16 char_uuid, U8 *flag)
{
    HtpServiceDB *servicedb;
    HtpCharDescListNode *desc_node;

    /* get description handle from database */
    servicedb = channel->servicedb;
    desc_node = HtpFindCharDescByUuid(&servicedb->hts_service, char_uuid, BT_UUID_GATT_CHAR_DESC_CLIENT_CHAR_CONFIG);	
    if (desc_node == NULL)
    {
        return BT_STATUS_FAILED;
    }

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_SET_CLIENT_CONFIG_DESCRIPTOR, flag[0], char_uuid, desc_node->handle);

    return HtpcWriteCharacteristicDescriptor(channel, desc_node->handle, flag, sizeof(HtpClientCharConfig));
}
 
/*****************************************************************************
 * FUNCTION
 *  HTPC_SetMeasurementInterval
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *  Write Measurement Interval value to Thermometer if the characteristic
 *  is available on Thermometer.
 *
 *****************************************************************************/
BtStatus HTPC_SetMeasurementInterval(HtpChannel *channel, U8 *interval)
{
    HtpServiceDB *servicedb;
    HtpCharListNode *char_node;

    /* get description handle from database */
    servicedb = channel->servicedb;
    char_node = HtpFindCharByUuid(&servicedb->hts_service, BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL);	
    if (char_node == NULL)
    {
        return BT_STATUS_FAILED;
    }

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_SET_MEASUREMENT_INTERVAL, interval[0], interval[1], char_node->value_handle);

    return HtpcWriteCharacteristic(channel, char_node->value_handle, interval, sizeof(HtpMeasurementInterval));
}

/*****************************************************************************
 * FUNCTION
 *  HTPT_GetCharClientConfigDesciptor
 *
 * DESCRIPTION
 *  It is for Thermometer only. 
 *  Since Client Configuration Descriptor is multiple instance, and it shall be saved for each
 *  bonded device. It is used to get characteristic client configuration descriptor for indicated
 *  device.
 *
 *****************************************************************************/
HtpClientCharConfig HTPT_GetCharClientConfigDesciptor(HtpChannel *channel, U16 char_uuid)
{
    U8 *descriptor;
    HtpClientCharConfig desc_config = 0;
        
    descriptor = HtsGetClientConfigDescriptors(&channel->link->bdAddr, char_uuid);
    if (descriptor != NULL)
    {
        desc_config = (descriptor[0] & 0xff) | (descriptor[1] << 8 & 0xff00);
    }

    return desc_config;
}

/*****************************************************************************
 * FUNCTION
 *  HTPT_SendTemperatureMeasurement
 *
 * DESCRIPTION
 *  It is for Thermometer only. 
 *  Send Temperature Measurement indication to Collector.
 *
 *****************************************************************************/
BtStatus HTPT_SendTemperatureMeasurement(HtpChannel *channel, HtpTemperature *temperature, U16 size)
{
    U8 *data = (U8 *)temperature;

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_SEND_TEMPERATURE_INDICATION, temperature->temperature_value.exponent, 
			temperature->temperature_value.mantissa[0], 
			temperature->temperature_value.mantissa[1],
			temperature->temperature_value.mantissa[2]);

    if (((temperature->flags & HTP_TEMPERATURE_FLAG_TIMESTAMP) == 0) &&
        ((temperature->flags & HTP_TEMPERATURE_FLAG_NONSTATIC) != 0))
    {
        data[size - 1] = temperature->temperature_type;
    }

#if HTP_TEMPERATURE_TYPE == XA_ENABLED /* Temperature Type Characteristic is included */
        /* There are two exclusive methods to enable a Thermometer to provide temperature type information
        *   to a Collector, via Temperature Type Characteristic, or Temperature Type Field. Either one method 
        *   or the other is used, but not both. 
        */
        if ((temperature->flags & HTP_TEMPERATURE_FLAG_NONSTATIC) != 0)
        {
            size -= 1;
            temperature->flags &= ~HTP_TEMPERATURE_FLAG_NONSTATIC;
        }
#endif

    return HtptSendIndication(channel, ATT_HANDLE_HTS_CHARACTERISTIC_TP_MEASURE_VALUE, 
                                    (U8 *)data, size);
}

/*****************************************************************************
 * FUNCTION
 *  HTPT_SendIntermediateTemperature
 *
 * DESCRIPTION
 *  It is for Thermometer only. 
 *  Send Intermediate Temperature notification to Collector.
 *
 *****************************************************************************/
BtStatus HTPT_SendIntermediateTemperature(HtpChannel *channel, HtpTemperature *temperature, U16 size)
{
    U8 *data = (U8 *)temperature;

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_SEND_TEMPERATURE_NOTIFICATION, temperature->temperature_value.exponent, 
			temperature->temperature_value.mantissa[0], 
			temperature->temperature_value.mantissa[1],
			temperature->temperature_value.mantissa[2]);

    if (((temperature->flags & HTP_TEMPERATURE_FLAG_TIMESTAMP) == 0) &&
        ((temperature->flags & HTP_TEMPERATURE_FLAG_NONSTATIC) != 0))
    {
        data[size - 1] = temperature->temperature_type;
    }

#if HTP_TEMPERATURE_TYPE == XA_ENABLED /* Temperature Type Characteristic is included */
    /* There are two exclusive methods to enable a Thermometer to provide temperature type information
    *   to a Collector, via Temperature Type Characteristic, or Temperature Type Field. Either one method 
    *   or the other is used, but not both. 
    */
    if ((temperature->flags & HTP_TEMPERATURE_FLAG_NONSTATIC) != 0)
    {
        size -= 1;
        temperature->flags &= ~HTP_TEMPERATURE_FLAG_NONSTATIC;
    }
#endif

    return HtptSendNotification(channel, ATT_HANDLE_HTS_CHARACTERISTIC_INTERMEDIATE_TP_VALUE, 
                            (U8 *)data, size);
}

/*****************************************************************************
 * FUNCTION
 *  HTPT_SetMeasurementInterval
 *
 * DESCRIPTION
 *  It is for Thermometer only. 
 *  Thermometer may reset Measurement Interval value locally.
 *  It shall  
 *     a. save the interval value in profile layer; 
 *     b. if client characteristic configuration for that Collector is configured to indicated,
 *         thermometer shall indicate the current interval value the next time there is a connection.
 *         (Note that, since the client charatertistic configuration is configured for each bonded 
 *         device which is saved in app, the indication shall be checked and triggered by app)
 *
 *****************************************************************************/
BtStatus HTPT_SetMeasurementInterval(U16 interval)
{
    return HtsSetMeasurementInterval(interval);
}

/*****************************************************************************
 * FUNCTION
 *  HTPT_SendMeasurementIntervalIndication
 *
 * DESCRIPTION
 *  It is for Thermometer only. 
 *  Thermometer shall send Measurement Interval value indication, if interval value is
 *  reset locally and client characteristic configuration for that Collector is configured 
 *  to indicated.
 *
 *****************************************************************************/
BtStatus HTPT_SendMeasurementIntervalIndication(HtpChannel *channel)
{
    return HtptSendIndication(channel, ATT_HANDLE_HTS_CHARACTERISTIC_MEASURE_INTERVAL_VALUE, 
                            HtsGetMeasurementInterval(), sizeof(HtpMeasurementInterval));
}

#endif
