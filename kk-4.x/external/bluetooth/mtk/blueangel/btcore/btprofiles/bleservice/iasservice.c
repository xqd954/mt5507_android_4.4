/*****************************************************************************
 *
 * Filename:
 * ---------
 * iasservice.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Immediate alert service
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
#include "iasservice.h"

BleIas_T BleIaxServiceCtx;

#define BleIasCtx(f)  (BleIaxServiceCtx.f)


/* Immediate alert service */

const U8 attIasRecord[] = 
{
    /* ALERT  */  
    ATT_HANDLE16(ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT, BT_UUID_GATT_TYPE_PRIMARY_SERVICE, BT_ATT_CONST_VARIABLE, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETUUID16(BT_UUID_GATT_SERVICE_ALERT),

    ATT_HANDLE16(ATT_HANDLE_IA_CHARACTERISTIC_ALERT_LEVEL, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_WRITE_WO_RESPONSE, ATT_HANDLE_IA_CHARACTERISTIC_ALERT_LEVEL_VALUE, BT_UUID_PROXIMITY_ALERT_LEVEL),
        
    ATT_HANDLE16(ATT_HANDLE_IA_CHARACTERISTIC_ALERT_LEVEL_VALUE, BT_UUID_PROXIMITY_ALERT_LEVEL, BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x01), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
};


void IasRegisterAttdbCallback(U16 handle, BtIasServiceCbHandler *handler)
{
    switch (handle)
    {
        case ATT_HANDLE_IA_CHARACTERISTIC_ALERT_LEVEL_VALUE:
            InsertTailList((ListEntry *)&(BleIasCtx(AlertLevelList)), &(handler->node));
            break;
              
    }

}

void IasDeregisterAttdbCallback(BtIasServiceCbHandler *handler)
{
    RemoveEntryList((ListEntry *)&(handler->node));
}


U8 IasAlertLevelServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtIasServiceCbHandler *handler;

    Report(("[IAS] alert level callback"));
    entry = BleIasCtx(AlertLevelList).node.Flink;
    while (entry && entry != (ListEntry *)&BleIasCtx(AlertLevelList))
    {
        Report(("[IAS] alert level callback to profile"));
        handler = ((BtIasServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, handle, len, raw);
    }
    return BT_STATUS_SUCCESS;

}

void IasInitialize(void)
{
    OS_MemSet((U8 *)&BleIaxServiceCtx, 0, sizeof(BleIaxServiceCtx));
//    InitializeListEntry((ListEntry *)&(BleIasCtx(AlertLevelList)));
    InitializeListHead((ListEntry *)&(BleIasCtx(AlertLevelList)));
    
}


U8 IasActivateService(void)
{
    BtStatus status; 
    
    Report(("BleIasCtx(registerCount) in IasActivateService:%d",BleIasCtx(registerCount)));
    if(BleIasCtx(registerCount) == 0)
    {
        status = ATTDB_AddRecord(attIasRecord, sizeof(attIasRecord));
        ATTDB_AddRecordCB(ATT_HANDLE_IA_CHARACTERISTIC_ALERT_LEVEL_VALUE, IasAlertLevelServiceCb);
        ATTDB_AddGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT, 
                             ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT_END, 
                             BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    }
    BleIasCtx(registerCount)++;
    return BT_STATUS_SUCCESS;
}

void IasDeactivateService(void)
{
    Report(("BleIasCtx(registerCount) in IasDeactivateService:%d",BleIasCtx(registerCount)));

    if(BleIasCtx(registerCount) == 1)
    {
        ATTDB_RemoveRecordCB(ATT_HANDLE_IA_CHARACTERISTIC_ALERT_LEVEL_VALUE);
        ATTDB_RemoveGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT, 
                                ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT_END);
        ATTDB_RemoveRecord(ATT_HANDLE_PRIMARY_SERVICE_IMMEDIATE_ALERT, ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS_END);
    }
    if(BleIasCtx(registerCount) == 0)
    {
        Assert(0);
    }
    BleIasCtx(registerCount)--;
}




#endif
