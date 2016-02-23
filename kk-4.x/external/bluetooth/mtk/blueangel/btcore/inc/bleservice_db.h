#ifndef __BLESERVICE_DB_H__
#define __BLESERVICE_DB_H__

typedef struct _ble_service_db
{
    ListEntry node;

    BD_ADDR bdAddr;
#ifdef __BT_HTS_SERVICE__
    U8 htsTemperatureClientCfgDesc[2];
    U8 htsImTemperatureClientCfgDesc[2];
    U8 htsIntervalClientCfgDesc[2];
#endif    
#ifdef __BT_HRS_SERVICE__
    U8 hrsHeartRateClientCfgDesc[2];
#endif
} ble_service_db;


void BleServiceDbInitialize(void);
void BleServiceDbLoadRecord(void);
void BleServiceDbUnloadRecord(void);
ble_service_db *BleServiceDbGetRecord(BD_ADDR *bd_addr);
void BleServiceDbUpdateRecord(ble_service_db *servicedb);

#endif
