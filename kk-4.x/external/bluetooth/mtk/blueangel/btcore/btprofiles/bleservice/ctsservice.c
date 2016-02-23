/*****************************************************************************
 *
 * Filename:
 * ---------
 * ctsservice.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Current time service
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
 * 12 15 2011 dlight.ting
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
#ifdef __BT_TIMES_PROFILE__
#include "attdb.h"
#include "attdef.h"
#include "attdbmain.h"
#include "ctsservice.h"

BleCts_T BleCtxServiceCtx;

#define BleCtsCtx(f)  (BleCtxServiceCtx.f)


/* Current Time service */
const U8 attCurrTimeRecord[] = 
{
	ATT_HANDLE16(ATT_HANDLE_PRIMARY_SERVICE_CURRENT_TIME,
				BT_UUID_GATT_TYPE_PRIMARY_SERVICE,
				BT_ATT_CONST_VARIABLE, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
	ATT_SETUUID16(BT_UUID_GATT_SERVICE_TIME),

	/* CT Time characteristic */
	ATT_HANDLE16(ATT_HANDLE_CHARACTERISTIC_CT_TIME,
			BT_UUID_GATT_TYPE_CHARACTERISTIC,
			BT_ATT_CONST_VARIABLE, 0x05),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
	ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY,
			ATT_HANDLE_CHARACTERISTIC_CT_TIME_VALUE,
			BT_UUID_CT_TIME),

	ATT_HANDLE16(ATT_HANDLE_CHARACTERISTIC_CT_TIME_VALUE,
			BT_UUID_CT_TIME,
			BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x0A),
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 

	ATT_HANDLE16(ATT_HANDLE_CLIENT_CHARACTERISTIC_CONFIG_CT_TIME_VALUE,
			BT_UUID_GATT_CHAR_DESC_CLIENT_CHAR_CONFIG,
			BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_WRITE, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 

	/* Local Time Information characteristic */
	ATT_HANDLE16(ATT_HANDLE_CHARACTERISTIC_LOCAL_TIME_INFO,
			BT_UUID_GATT_TYPE_CHARACTERISTIC,
			BT_ATT_CONST_VARIABLE, 0x05),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
	ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ,
			ATT_HANDLE_CHARACTERISTIC_LOCAL_TIME_INFO_VALUE,
			BT_UUID_LOCAL_TIME_INFO),

	ATT_HANDLE16(ATT_HANDLE_CHARACTERISTIC_LOCAL_TIME_INFO_VALUE,
			BT_UUID_LOCAL_TIME_INFO,
			BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 

	/* Reference Time Information characteristic */
	ATT_HANDLE16(ATT_HANDLE_CHARACTERISTIC_REF_TIME_INFO,
			BT_UUID_GATT_TYPE_CHARACTERISTIC,
			BT_ATT_CONST_VARIABLE, 0x05),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
	ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ,
			ATT_HANDLE_CHARACTERISTIC_REF_TIME_INFO_VALUE,
			BT_UUID_REF_TIME_INFO),

	ATT_HANDLE16(ATT_HANDLE_CHARACTERISTIC_REF_TIME_INFO_VALUE,
			BT_UUID_REF_TIME_INFO,
			BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x04),
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0),  
};


U8 CtsCttimeServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtCtsServiceCbHandler *handler;

    entry = BleCtsCtx(CtsCttimeList).node.Flink;
    while (entry && entry != (ListEntry *)&BleCtsCtx(CtsCttimeList))
    {
        handler = ((BtCtsServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, len, raw);
    }
    return BT_STATUS_SUCCESS;

}
        

U8 CtsCttimeConfigServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtCtsServiceCbHandler *handler;

    entry = BleCtsCtx(CtsCttimeConfigList).node.Flink;
    while (entry && entry != (ListEntry *)&BleCtsCtx(CtsCttimeConfigList))
    {
        handler = ((BtCtsServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, len, raw);
    }
    return BT_STATUS_SUCCESS;

}
        

U8 CtsLocalTimeInfoServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtCtsServiceCbHandler *handler;

    entry = BleCtsCtx(CtsLocaltimeInfoList).node.Flink;
    while (entry && entry != (ListEntry *)&BleCtsCtx(CtsLocaltimeInfoList))
    {
        handler = ((BtCtsServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, len, raw);
    }
    return BT_STATUS_SUCCESS;

}
    

U8 CtsRefTimeInfoServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtCtsServiceCbHandler *handler;

    entry = BleCtsCtx(CtsReftimeInfoList).node.Flink;
    while (entry && entry != (ListEntry *)&BleCtsCtx(CtsReftimeInfoList))
    {
		handler = ((BtCtsServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, len, raw);
    }
    return BT_STATUS_SUCCESS;

}


void CtsRegisterAttdbCallback(U16 handle, BtCtsServiceCbHandler *handler)
{
    switch (handle)
    {
        case ATT_HANDLE_CHARACTERISTIC_CT_TIME_VALUE:
            InsertTailList((ListEntry *)&(BleCtsCtx(CtsCttimeList)), &(handler->node));
            break;
        case ATT_HANDLE_CLIENT_CHARACTERISTIC_CONFIG_CT_TIME_VALUE:
            InsertTailList((ListEntry *)&(BleCtsCtx(CtsCttimeConfigList)), &(handler->node));
            break;
        case ATT_HANDLE_CHARACTERISTIC_LOCAL_TIME_INFO_VALUE:
            InsertTailList((ListEntry *)&(BleCtsCtx(CtsLocaltimeInfoList)), &(handler->node));
            break;
        case ATT_HANDLE_CHARACTERISTIC_REF_TIME_INFO_VALUE:
            InsertTailList((ListEntry *)&(BleCtsCtx(CtsReftimeInfoList)), &(handler->node));
            break;   
    }

}

void CtsDeregisterAttdbCallback(BtCtsServiceCbHandler *handler)
{
    RemoveEntryList(&(handler->node));
}


void CtsInitialize(void)
{
    OS_MemSet((U8 *)&BleCtxServiceCtx, 0, sizeof(BleCtxServiceCtx));
  
    InitializeListHead((ListEntry *)&(BleCtsCtx(CtsCttimeList)));
    InitializeListHead((ListEntry *)&(BleCtsCtx(CtsCttimeConfigList)));
    InitializeListHead((ListEntry *)&(BleCtsCtx(CtsLocaltimeInfoList)));
    InitializeListHead((ListEntry *)&(BleCtsCtx(CtsReftimeInfoList)));
}


U8 CtsActivateService(void)
{
    BtStatus status; 
    
    Report(("BleCtsCtx(registerCount) in CtsActivateService:%d",BleCtsCtx(registerCount)));
    if(BleCtsCtx(registerCount) == 0)
    {

        status = ATTDB_AddRecord(attCurrTimeRecord, sizeof(attCurrTimeRecord));
        ATTDB_AddRecordCB(ATT_HANDLE_CHARACTERISTIC_CT_TIME_VALUE, CtsCttimeServiceCb);
        ATTDB_AddRecordCB(ATT_HANDLE_CLIENT_CHARACTERISTIC_CONFIG_CT_TIME_VALUE, CtsCttimeConfigServiceCb);
        ATTDB_AddRecordCB(ATT_HANDLE_CHARACTERISTIC_LOCAL_TIME_INFO_VALUE, CtsLocalTimeInfoServiceCb);
        ATTDB_AddRecordCB(ATT_HANDLE_CHARACTERISTIC_REF_TIME_INFO_VALUE, CtsRefTimeInfoServiceCb);


        ATTDB_AddGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_CURRENT_TIME, 
                ATT_HANDLE_PRIMARY_SERVICE_CURRENT_TIME_END, 
                BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    }
    BleCtsCtx(registerCount)++;
    return BT_STATUS_SUCCESS;
}

void CtsDeactivateService(void)
{
    Report(("BleCtsCtx(registerCount) in CtsDeactivateService:%d",BleCtsCtx(registerCount)));

    if(BleCtsCtx(registerCount) == 1)
    {

        ATTDB_RemoveRecordCB(ATT_HANDLE_CHARACTERISTIC_CT_TIME_VALUE);
        ATTDB_RemoveRecordCB(ATT_HANDLE_CLIENT_CHARACTERISTIC_CONFIG_CT_TIME_VALUE);
        ATTDB_RemoveRecordCB(ATT_HANDLE_CHARACTERISTIC_LOCAL_TIME_INFO_VALUE);
        ATTDB_RemoveRecordCB(ATT_HANDLE_CHARACTERISTIC_REF_TIME_INFO_VALUE);

        ATTDB_RemoveRecord(ATT_HANDLE_PRIMARY_SERVICE_CURRENT_TIME,
                ATT_HANDLE_PRIMARY_SERVICE_REFERENCE_TIME_UPDATE_END);
        
        ATTDB_RemoveGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_CURRENT_TIME, 
                ATT_HANDLE_PRIMARY_SERVICE_CURRENT_TIME_END);

    }
    if(BleCtsCtx(registerCount) == 0)
    {
        Assert(0);
    }
    BleCtsCtx(registerCount)--;
}




#endif
