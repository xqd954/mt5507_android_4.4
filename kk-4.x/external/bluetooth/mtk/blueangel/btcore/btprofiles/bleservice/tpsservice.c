/*****************************************************************************
 *
 * Filename:
 * ---------
 * tpsservice.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth tx power service
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
 * 12 14 2011 dlight.ting
 * NULL
 * .
 *
 * 11 04 2011 dlight.ting
 * NULL
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#if defined(__BT_PRXR_PROFILE__) || defined(__BT_PRXM_PROFILE__)
#include "attdb.h"
#include "attdef.h"
#include "attdbmain.h"
#include "tpsservice.h"

BleTps_T BleTpxServiceCtx;

#define BleTpsCtx(f)  (BleTpxServiceCtx.f)

/* Immediate alert service */

    
const U8 attTpsRecord[] = 
{
    /* TX Power  */  
    ATT_HANDLE16(ATT_HANDLE_PRIMARY_SERVICE_TX_POWER, BT_UUID_GATT_TYPE_PRIMARY_SERVICE, BT_ATT_CONST_VARIABLE, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETUUID16(BT_UUID_GATT_SERVICE_TXPOWER),

    ATT_HANDLE16(ATT_HANDLE_TP_CHARACTERISTIC_TX_POWER_LEVEL, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ, ATT_HANDLE_TP_CHARACTERISTIC_TX_POWER_LEVEL_VALUE, BT_UUID_PROXIMITY_TXPOWER_LEVEL),
        
    ATT_HANDLE16(ATT_HANDLE_TP_CHARACTERISTIC_TX_POWER_LEVEL_VALUE, BT_UUID_PROXIMITY_TXPOWER_LEVEL, BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x01), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
};



void TpsRegisterAttdbCallback(U16 handle, BtTpsServiceCbHandler *handler)
{
    switch (handle)
    {
        case ATT_HANDLE_TP_CHARACTERISTIC_TX_POWER_LEVEL_VALUE:
            InsertTailList((ListEntry *)&(BleTpsCtx(TxPowerDatabaseList)), (ListEntry *)&(handler->node));
            break;
              
    }

}


void TpsDeregisterAttdbCallback(BtTpsServiceCbHandler *handler)
{
    RemoveEntryList(&(handler->node));
}


U8 TpsTxPowerDatabaseServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtTpsServiceCbHandler *handler;

    entry = BleTpsCtx(TxPowerDatabaseList).node.Flink;
    while (entry && entry != (ListEntry *)&BleTpsCtx(TxPowerDatabaseList))
    {
        handler = ((BtTpsServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, handle, len, raw);
    }
    return BT_STATUS_SUCCESS;

}

void TpsInitialize(void)
{
    OS_MemSet((U8 *)&BleTpxServiceCtx, 0, sizeof(BleTpxServiceCtx));
    InitializeListHead((ListEntry *)&(BleTpsCtx(TxPowerDatabaseList)));
    
}


U8 TpsActivateService(void)
{
    BtStatus status; 
    
    Report(("BleTpsCtx(registerCount) in TpsActivateService:%d",BleTpsCtx(registerCount)));
    if(BleTpsCtx(registerCount) == 0)
    {
        status = ATTDB_AddRecord(attTpsRecord, sizeof(attTpsRecord));
        ATTDB_AddRecordCB(ATT_HANDLE_TP_CHARACTERISTIC_TX_POWER_LEVEL_VALUE, TpsTxPowerDatabaseServiceCb);

        ATTDB_AddGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_TX_POWER, 
                             ATT_HANDLE_PRIMARY_SERVICE_TX_POWER_END, 
                             BT_UUID_GATT_TYPE_PRIMARY_SERVICE);        
    }
    BleTpsCtx(registerCount)++;
    return BT_STATUS_SUCCESS;
}

void TpsDeactivateService(void)
{
    Report(("BleTpsCtx(registerCount) in TpsDeactivateService:%d",BleTpsCtx(registerCount)));

    if(BleTpsCtx(registerCount) == 1)
    {
        ATTDB_RemoveRecordCB(ATT_HANDLE_TP_CHARACTERISTIC_TX_POWER_LEVEL_VALUE);
        ATTDB_RemoveGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_TX_POWER, 
                                ATT_HANDLE_PRIMARY_SERVICE_TX_POWER_END);
        ATTDB_RemoveRecord(ATT_HANDLE_PRIMARY_SERVICE_TX_POWER, ATT_HANDLE_PRIMARY_SERVICE_TX_POWER);
    }
    if(BleTpsCtx(registerCount) == 0)
    {
        Assert(0);
    }
    BleTpsCtx(registerCount)--;
}




#endif
