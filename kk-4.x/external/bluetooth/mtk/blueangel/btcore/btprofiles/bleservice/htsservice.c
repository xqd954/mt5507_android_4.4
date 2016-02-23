/*****************************************************************************
 *
 * Filename:
 * ---------
 * htsservice.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Health Thermometer service
 *
 * Author:
 * -------
 * Ting Zheng & Dlight Ting
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
 * 12 06 2011 dlight.ting
 * NULL
 * .
 *
 * 11 04 2011 dlight.ting
 * NULL
 * .
 *
 * 11 02 2011 dlight.ting
 * NULL
 * .
 *
 * 11 02 2011 dlight.ting
 * NULL
 * .
 *
 * 11 01 2011 dlight.ting
 * NULL
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_HTS_SERVICE__
#include "sec.h"
#include "attdef.h"
#include "attdb.h"
#include "attdbmain.h"
#include "bleservice_db.h"

#include "htsservice.h"

BleHts_T BleHtsServiceCtx;


#define BleHtsCtx(f)  (BleHtsServiceCtx.f)

#define BT_HTS_ATT_ERROR_CODE_OUT_OF_RANGE          (0x80)


//extern U16 HtsGetHtsRecord(U8 *ptr);


const U8 attHtsRecord[] = 
{
    ATT_HANDLE16(ATT_HANDLE_PRIMARY_SERVICE_HEALTH_THERMOMETER, BT_UUID_GATT_TYPE_PRIMARY_SERVICE, BT_ATT_CONST_VARIABLE, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY_BLE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETUUID16(BT_UUID_GATT_SERVICE_HEALTH),

    /* Temperature Measurement characteristic */
    ATT_HANDLE16(ATT_HANDLE_HTS_CHARACTERISTIC_TP_MEASURE, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY_BLE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_INDICATE, ATT_HANDLE_HTS_CHARACTERISTIC_TP_MEASURE_VALUE, BT_UUID_HTS_CHAR_TEMPERATURE_MEASUREMENT),

    // flag (1 octet) + temperature (4 octets) + time stamp (7 octets), with static temperature type (1 octet is obmited)
    ATT_HANDLE16(ATT_HANDLE_HTS_CHARACTERISTIC_TP_MEASURE_VALUE, BT_UUID_HTS_CHAR_TEMPERATURE_MEASUREMENT, BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x0c),
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED_BLE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 

    ATT_HANDLE16(ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_TP_MEASURE, BT_UUID_GATT_CHAR_DESC_CLIENT_CHAR_CONFIG, BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_WRITE_BLE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 

    /* Temperature Type characteristic */
    ATT_HANDLE16(ATT_HANDLE_HTS_CHARACTERISTIC_TP_TYPE, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY_BLE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ, ATT_HANDLE_HTS_CHARACTERISTIC_TP_TYPE_VALUE, BT_UUID_HTS_CHAR_TEMPERATURE_TYPE),

    ATT_HANDLE16(ATT_HANDLE_HTS_CHARACTERISTIC_TP_TYPE_VALUE, BT_UUID_HTS_CHAR_TEMPERATURE_TYPE, BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x01),
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED_BLE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 

    /* Intermediate Temperature characteristic */
    ATT_HANDLE16(ATT_HANDLE_HTS_CHARACTERISTIC_INTERMEDIATE_TP, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY_BLE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_NOTIFY, ATT_HANDLE_HTS_CHARACTERISTIC_INTERMEDIATE_TP_VALUE, BT_UUID_HTS_CHAR_INTERMEDIATE_TEMPERATURE),

    // flag (1 octet) + temperature (4 octets), with time stamp (7 octets) and temperature type (1 octet) obmited
    ATT_HANDLE16(ATT_HANDLE_HTS_CHARACTERISTIC_INTERMEDIATE_TP_VALUE, BT_UUID_HTS_CHAR_INTERMEDIATE_TEMPERATURE, BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x05),
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED_BLE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 

    ATT_HANDLE16(ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_INTERMEDIATE_TP, BT_UUID_GATT_CHAR_DESC_CLIENT_CHAR_CONFIG, BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_WRITE_BLE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 

    /* Measurement Interval characteristic */
    ATT_HANDLE16(ATT_HANDLE_HTS_CHARACTERISTIC_MEASURE_INTERVAL, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY_BLE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_INDICATE, 
                                                         ATT_HANDLE_HTS_CHARACTERISTIC_MEASURE_INTERVAL_VALUE, BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL),

    ATT_HANDLE16(ATT_HANDLE_HTS_CHARACTERISTIC_MEASURE_INTERVAL_VALUE, BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL, BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED_BLE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_AUTHENTICATION, 0), 
        // Read with no authentication, Write with authentication

    ATT_HANDLE16(ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_MEASURE_INTERVAL, BT_UUID_GATT_CHAR_DESC_CLIENT_CHAR_CONFIG, BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_WRITE_BLE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 

    ATT_HANDLE16(ATT_HANDLE_HTS_DESCRIPTOR_VALID_RANGE, BT_UUID_HTS_CHAR_DESC_VALID_RANGE, BT_ATT_CONST_VARIABLE, 0x04),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY_BLE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
};


U8 HtsTemperatureClientConfigServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtHtsServiceCbHandler *handler;

    if (type == ATT_OP_READ ||
        type == ATT_OP_READ_LONG ||
        type == ATT_OP_READ_MULTIPLE ||
        type == ATT_OP_READ_BY_TYPE)
    {
        ble_service_db *servicedb;
        servicedb = BleServiceDbGetRecord(&link->bdAddr);
        *raw = servicedb->htsTemperatureClientCfgDesc;
        *len = 2;            
        Report(("[HTS] read temperature descriptor: %d,%d", 
            servicedb->htsTemperatureClientCfgDesc[0], servicedb->htsTemperatureClientCfgDesc[1]));
    }
    else if (type == ATT_OP_WRITE || type == ATT_OP_EXECUTE_WRITE)
    {
        ble_service_db *servicedb;
        BtDeviceRecord record;
        servicedb = BleServiceDbGetRecord(&link->bdAddr);
        OS_MemCopy(servicedb->htsTemperatureClientCfgDesc, *raw, *len);
        Report(("[HTS] write temperature descriptor: %d,%d", 
            servicedb->htsTemperatureClientCfgDesc[0], servicedb->htsTemperatureClientCfgDesc[1]));
        
        if (SEC_FindDeviceRecord(&link->bdAddr, &record) == BT_STATUS_SUCCESS)
        {
            /* If it is a bonded device, update the record in service db file */
            BleServiceDbUpdateRecord(servicedb);
        }
    }

    entry = BleHtsCtx(HtsTemperatureClientConfigList).node.Flink;
    while (entry && entry != (ListEntry *)&BleHtsCtx(HtsTemperatureClientConfigList))
    {
		handler = ((BtHtsServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, len, raw);
    }
    return BT_STATUS_SUCCESS;

}

U8 HtsTemperatureTypeServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtHtsServiceCbHandler *handler;

    if (type == ATT_OP_READ ||
         type == ATT_OP_READ_LONG ||
         type == ATT_OP_READ_MULTIPLE ||
         type == ATT_OP_READ_BY_TYPE)
    {
        *raw = &BleHtsCtx(temperature_type);
        *len = 1;
    }

    entry = BleHtsCtx(HtsTemperatureTypeList).node.Flink;
    while (entry && entry != (ListEntry *)&BleHtsCtx(HtsTemperatureTypeList))
    {
		handler = ((BtHtsServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, len, raw);
    }
    return BT_STATUS_SUCCESS;

}

U8 HtsImtermediateTemperatureClientConfigServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtHtsServiceCbHandler *handler;

    if (type == ATT_OP_READ)
    {
        ble_service_db *servicedb;
        servicedb = BleServiceDbGetRecord(&link->bdAddr);
        *raw = servicedb->htsImTemperatureClientCfgDesc;
        *len = 2;            
        Report(("[HTS] read imtermediate temperature descriptor: %d,%d", 
            servicedb->htsImTemperatureClientCfgDesc[0], servicedb->htsImTemperatureClientCfgDesc[1]));
    }
    else if (type == ATT_OP_WRITE || type == ATT_OP_EXECUTE_WRITE)
    {
        ble_service_db *servicedb;
        BtDeviceRecord record;
        servicedb = BleServiceDbGetRecord(&link->bdAddr);
        OS_MemCopy(servicedb->htsImTemperatureClientCfgDesc, *raw, *len);
        Report(("[HTS] write imtermediate temperature descriptor: %d,%d", 
            servicedb->htsImTemperatureClientCfgDesc[0], servicedb->htsImTemperatureClientCfgDesc[1]));

        if (SEC_FindDeviceRecord(&link->bdAddr, &record) == BT_STATUS_SUCCESS)
        {
            /* If it is a bonded device, update the record in service db file */
            BleServiceDbUpdateRecord(servicedb);
        }
    }

    entry = BleHtsCtx(HtsIntermediateTemperatureClientConfigList).node.Flink;
    while (entry && entry != (ListEntry *)&BleHtsCtx(HtsIntermediateTemperatureClientConfigList))
    {
		handler = ((BtHtsServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, len, raw);
    }
    return BT_STATUS_SUCCESS;

}

U8 HtsIntervalValueServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtHtsServiceCbHandler *handler;

    if (type == ATT_OP_READ ||
         type == ATT_OP_READ_LONG ||
         type == ATT_OP_READ_MULTIPLE ||
         type ==ATT_OP_READ_BY_TYPE)
    {
        *raw = BleHtsCtx(interval);
        *len = 2;
    }
    else if (type == ATT_OP_WRITE || type == ATT_OP_EXECUTE_WRITE)
    {
        U16 interval_min = BleHtsCtx(valid_range)[0] | ((BleHtsCtx(valid_range)[1] << 8) & 0xff00);
        U16 interval_max = BleHtsCtx(valid_range)[2] | ((BleHtsCtx(valid_range)[3] << 8) & 0xff00);
        U16 interval;
        
        interval = (0x00ff & **raw) | (0xff00 & (*(*raw+1) << 8));
        kal_trace(BT_TRACE_BLE_PROFILES, HTP_WRITE_INTERVAL, interval);
        if ((interval > interval_max) || (interval < interval_min))
        {
            // GATT error code Out of Range handling
            return BT_HTS_ATT_ERROR_CODE_OUT_OF_RANGE;
        }
        OS_MemCopy(BleHtsCtx(interval), *raw, *len);
    }

    entry = BleHtsCtx(HtsIntervalValueList).node.Flink;
    while (entry && entry != (ListEntry *)&BleHtsCtx(HtsIntervalValueList))
    {
		handler = ((BtHtsServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, len, raw);
    }
    return BT_STATUS_SUCCESS;

}

U8 HtsIntervalClientConfigServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtHtsServiceCbHandler *handler;

    if (type == ATT_OP_READ ||
        type == ATT_OP_READ_LONG ||
        type == ATT_OP_READ_MULTIPLE ||
        type == ATT_OP_READ_BY_TYPE)
    {
        ble_service_db *servicedb;
        servicedb = BleServiceDbGetRecord(&link->bdAddr);
        *raw = servicedb->htsIntervalClientCfgDesc;
        *len = 2;            
    }
    else if (type == ATT_OP_WRITE || type == ATT_OP_EXECUTE_WRITE)
    {
        ble_service_db *servicedb;
        BtDeviceRecord record;
        servicedb = BleServiceDbGetRecord(&link->bdAddr);
        OS_MemCopy(servicedb->htsIntervalClientCfgDesc, *raw, *len);
        if (SEC_FindDeviceRecord(&link->bdAddr, &record) == BT_STATUS_SUCCESS)
        {
            /* If it is a bonded device, update the record in service db file */
            BleServiceDbUpdateRecord(servicedb);
        }
    }

    entry = BleHtsCtx(HtsIntervalClientConfigList).node.Flink;
    while (entry && entry != (ListEntry *)&BleHtsCtx(HtsIntervalClientConfigList))
    {
		handler = ((BtHtsServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, len, raw);
    }
    return BT_STATUS_SUCCESS;

}

U8 HtsIntervalRangeServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtHtsServiceCbHandler *handler;

    if (type == ATT_OP_READ ||
         type == ATT_OP_READ_LONG ||
         type == ATT_OP_READ_MULTIPLE ||
         type == ATT_OP_READ_BY_TYPE)
    {
        *raw = BleHtsCtx(valid_range);
        *len = 4;
    }

    entry = BleHtsCtx(HtsIntervalRangeList).node.Flink;
    while (entry && entry != (ListEntry *)&BleHtsCtx(HtsIntervalRangeList))
    {
        handler = ((BtHtsServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, len, raw);
    }
    return BT_STATUS_SUCCESS;
}


void HtsRegisterAttdbCallback(U16 handle, BtHtsServiceCbHandler *handler)
{
    switch (handle)
    {
        case ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_TP_MEASURE:
            InsertTailList((ListEntry *)&(BleHtsCtx(HtsTemperatureClientConfigList)), &(handler->node));
            break;
        case ATT_HANDLE_HTS_CHARACTERISTIC_TP_TYPE_VALUE:
            InsertTailList((ListEntry *)&(BleHtsCtx(HtsTemperatureTypeList)), &(handler->node));
            break;
        case ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_INTERMEDIATE_TP:
            InsertTailList((ListEntry *)&(BleHtsCtx(HtsIntermediateTemperatureClientConfigList)), &(handler->node));
            break;
        case ATT_HANDLE_HTS_CHARACTERISTIC_MEASURE_INTERVAL_VALUE:
            InsertTailList((ListEntry *)&(BleHtsCtx(HtsIntervalValueList)), &(handler->node));
            break;   
        case ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_MEASURE_INTERVAL:
            InsertTailList((ListEntry *)&(BleHtsCtx(HtsIntervalClientConfigList)), &(handler->node));
            break;
        case ATT_HANDLE_HTS_DESCRIPTOR_VALID_RANGE:
            InsertTailList((ListEntry *)&(BleHtsCtx(HtsIntervalRangeList)), &(handler->node));
            break;                
    }

}

void HtsDeregisterAttdbCallback(BtHtsServiceCbHandler *handler)
{
    RemoveEntryList(&(handler->node));
}


void HtsInitialize(void)
{
    OS_MemSet((U8 *)&BleHtsServiceCtx, 0, sizeof(BleHtsServiceCtx));
  
    InitializeListHead((ListEntry *)&(BleHtsCtx(HtsTemperatureClientConfigList)));
    InitializeListHead((ListEntry *)&(BleHtsCtx(HtsTemperatureTypeList)));
    InitializeListHead((ListEntry *)&(BleHtsCtx(HtsIntermediateTemperatureClientConfigList)));
    InitializeListHead((ListEntry *)&(BleHtsCtx(HtsIntervalValueList)));
    InitializeListHead((ListEntry *)&(BleHtsCtx(HtsIntervalClientConfigList)));
    InitializeListHead((ListEntry *)&(BleHtsCtx(HtsIntervalRangeList)));

}

U8 HtsActivateService(void)
{
    BtStatus status; 

    Report(("BleHtsCtx(registerCount) in HtsActivateService:%d",BleHtsCtx(registerCount)));
    if(BleHtsCtx(registerCount) == 0)
    {
        BleServiceDbLoadRecord();
        
        status = ATTDB_AddRecord(attHtsRecord, sizeof(attHtsRecord));    
        if (status != BT_STATUS_SUCCESS)
        {
            return BT_STATUS_FAILED;
        }
        ATTDB_AddGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_HEALTH_THERMOMETER, 
            ATT_HANDLE_PRIMARY_SERVICE_HEALTH_THERMOMETER_END,
            BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
        
        ATTDB_AddRecordCB(ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_TP_MEASURE, HtsTemperatureClientConfigServiceCb);
        ATTDB_AddRecordCB(ATT_HANDLE_HTS_CHARACTERISTIC_TP_TYPE_VALUE, HtsTemperatureTypeServiceCb);
        ATTDB_AddRecordCB(ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_INTERMEDIATE_TP, HtsImtermediateTemperatureClientConfigServiceCb);
        ATTDB_AddRecordCB(ATT_HANDLE_HTS_CHARACTERISTIC_MEASURE_INTERVAL_VALUE, HtsIntervalValueServiceCb);  
        ATTDB_AddRecordCB(ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_MEASURE_INTERVAL, HtsIntervalClientConfigServiceCb);
        ATTDB_AddRecordCB(ATT_HANDLE_HTS_DESCRIPTOR_VALID_RANGE, HtsIntervalRangeServiceCb);
    }
    BleHtsCtx(registerCount)++;
    return BT_STATUS_SUCCESS;
}

void HtsDeactivateService(void)
{
    Report(("BleHtsCtx(registerCount) in HtsDeactivateService:%d",BleHtsCtx(registerCount)));

    if(BleHtsCtx(registerCount) == 1)
    {
        BleServiceDbUnloadRecord();

        ATTDB_RemoveRecordCB(ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_TP_MEASURE);
        ATTDB_RemoveRecordCB(ATT_HANDLE_HTS_CHARACTERISTIC_TP_TYPE_VALUE);
        ATTDB_RemoveRecordCB(ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_INTERMEDIATE_TP);
        ATTDB_RemoveRecordCB(ATT_HANDLE_HTS_CHARACTERISTIC_MEASURE_INTERVAL_VALUE);   
        ATTDB_RemoveRecordCB(ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_MEASURE_INTERVAL);
        ATTDB_RemoveRecordCB(ATT_HANDLE_HTS_DESCRIPTOR_VALID_RANGE);
        /* Need to remove the callback function from Database. */
        ATTDB_RemoveRecord(ATT_HANDLE_PRIMARY_SERVICE_HEALTH_THERMOMETER,ATT_HANDLE_PRIMARY_SERVICE_HEALTH_THERMOMETER_END);
        ATTDB_RemoveGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_HEALTH_THERMOMETER,ATT_HANDLE_PRIMARY_SERVICE_HEALTH_THERMOMETER_END);
    }
    if(BleHtsCtx(registerCount) == 0)
    {
        Assert(0);
    }
    BleHtsCtx(registerCount)--;
}

BtStatus HtsSetMeasurementInterval(U16 interval)
{
    U16 interval_min = BleHtsCtx(valid_range)[0] | ((BleHtsCtx(valid_range)[1] << 8) & 0xff00);
    U16 interval_max = BleHtsCtx(valid_range)[2] | ((BleHtsCtx(valid_range)[3] << 8) & 0xff00);

    if ((interval < interval_min) || (interval > interval_max))
    {
        return BT_STATUS_FAILED;
    }

    BleHtsCtx(interval)[0] = interval & 0xff;
    BleHtsCtx(interval)[1] = (interval & 0xff00) >> 8;

    return BT_STATUS_SUCCESS;
}

U8 *HtsGetMeasurementInterval(void)
{
    return BleHtsCtx(interval);
}

void HtsSetValidRange(U16 valid_range_min, U16 valid_range_max)
{
    BleHtsCtx(valid_range)[0] = valid_range_min & 0xff;
    BleHtsCtx(valid_range)[1] = (valid_range_min & 0xff00) >> 8;
    BleHtsCtx(valid_range)[2] = valid_range_max & 0xff;
    BleHtsCtx(valid_range)[3] = (valid_range_max & 0xff00) >> 8;
}

void HtsSetTemperatureType(U8 type)
{
    BleHtsCtx(temperature_type) = type;
}

U8 *HtsGetClientConfigDescriptors(BD_ADDR *bd_addr, U16 char_uuid)
{
    ble_service_db *servicedb;
    U8 *descriptor = NULL;

    servicedb = BleServiceDbGetRecord(bd_addr);

    switch (char_uuid)
    {
        case BT_UUID_HTS_CHAR_TEMPERATURE_MEASUREMENT:
            descriptor = servicedb->htsTemperatureClientCfgDesc;
            break;
        case BT_UUID_HTS_CHAR_INTERMEDIATE_TEMPERATURE:
            descriptor = servicedb->htsImTemperatureClientCfgDesc;
            break;
        case BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL:
            descriptor = servicedb->htsIntervalClientCfgDesc;
            break;
        default:
            break;
    }
    return descriptor;    
}

#endif
