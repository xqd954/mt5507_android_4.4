/*****************************************************************************
 *
 * Filename:
 * ---------
 * rtusservice.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth reference time service
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
#include "rtusservice.h"

BleRtus_T BleRtuxServiceCtx;

#define BleRtusCtx(f)  (BleRtuxServiceCtx.f)



/* Reference Time Update service */
const U8 attRefTimeUpdateRecord[] =
{
	ATT_HANDLE16(ATT_HANDLE_PRIMARY_SERVICE_REFERENCE_TIME_UPDATE,
			BT_UUID_GATT_TYPE_PRIMARY_SERVICE,
			BT_ATT_CONST_VARIABLE, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0),  
	ATT_SETUUID16(BT_UUID_GATT_SERVICE_TIMEUPDATE),

	/* Time Update Control Point characteristic */
	ATT_HANDLE16(ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_CONTROL_POINT,
			BT_UUID_GATT_TYPE_CHARACTERISTIC,
			BT_ATT_CONST_VARIABLE, 0x05),
//        ATT_PERMINSIONS(ATT_PERMISSIONS_WRITE_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_WRITE, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
	ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_WRITE_WO_RESPONSE,
			ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_CONTROL_POINT_VALUE,
			BT_UUID_TIME_UPDATE_CONTROL_POINT),

	ATT_HANDLE16(ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_CONTROL_POINT_VALUE,
			BT_UUID_TIME_UPDATE_CONTROL_POINT,
			BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x01),
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0),  

	/* Time Update State characteristic */
	ATT_HANDLE16(ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_STATE,
			BT_UUID_GATT_TYPE_CHARACTERISTIC,
			BT_ATT_CONST_VARIABLE, 0x05),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
	ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ,
			ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_STATE_VALUE,
			BT_UUID_TIME_UPDATE_STATE),

	ATT_HANDLE16(ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_STATE_VALUE,
			BT_UUID_TIME_UPDATE_STATE,
			BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x01),
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0),  
};



U8 RtusUpdateControlPointServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtRtusServiceCbHandler *handler;

    entry = BleRtusCtx(RtusUpdateControlPointList).node.Flink;
    while (entry && entry != (ListEntry *)&BleRtusCtx(RtusUpdateControlPointList))
    {
        handler = ((BtRtusServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, len, raw);
    }
    return BT_STATUS_SUCCESS;

}

U8 RtusUpdateStateServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtRtusServiceCbHandler *handler;

    entry = BleRtusCtx(RtusUpdateStateList).node.Flink;
    while (entry && entry != (ListEntry *)&BleRtusCtx(RtusUpdateStateList))
    {
        handler = ((BtRtusServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, len, raw);
    }
    return BT_STATUS_SUCCESS;

}    

void RtusRegisterAttdbCallback(U16 handle, BtRtusServiceCbHandler *handler)
{
    switch (handle)
    {
        case ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_CONTROL_POINT_VALUE:
            InsertTailList((ListEntry *)&(BleRtusCtx(RtusUpdateControlPointList)), &(handler->node));
            break;
        case ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_STATE_VALUE:
            InsertTailList((ListEntry *)&(BleRtusCtx(RtusUpdateStateList)), &(handler->node));
            break;            
    }

}

void RtusDeregisterAttdbCallback(BtRtusServiceCbHandler *handler)
{
    RemoveEntryList((ListEntry *)&(handler->node));
}


void RtusInitialize(void)
{
    OS_MemSet((U8*)&BleRtuxServiceCtx, 0, sizeof(BleRtuxServiceCtx));
  
    InitializeListHead((ListEntry *)&(BleRtusCtx(RtusUpdateControlPointList)));
    InitializeListHead((ListEntry *)&(BleRtusCtx(RtusUpdateStateList)));
    
}


U8 RtusActivateService(void)
{
    BtStatus status; 
    
    Report(("BleRtusCtx(registerCount) in RtusActivateService:%d",BleRtusCtx(registerCount)));
    if(BleRtusCtx(registerCount) == 0)
    {
        status = ATTDB_AddRecord(attRefTimeUpdateRecord, sizeof(attRefTimeUpdateRecord));
        ATTDB_AddRecordCB(ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_CONTROL_POINT_VALUE, RtusUpdateControlPointServiceCb);
        ATTDB_AddRecordCB(ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_STATE_VALUE, RtusUpdateStateServiceCb);
        ATTDB_AddGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_REFERENCE_TIME_UPDATE, 
                ATT_HANDLE_PRIMARY_SERVICE_REFERENCE_TIME_UPDATE_END, 
                BT_UUID_GATT_TYPE_PRIMARY_SERVICE);

    }
    BleRtusCtx(registerCount)++;
    return BT_STATUS_SUCCESS;
}

void RtusDeactivateService(void)
{
    Report(("BleRtusCtx(registerCount) in RtusDeactivateService:%d",BleRtusCtx(registerCount)));

    if(BleRtusCtx(registerCount) == 1)
    {

        ATTDB_RemoveRecordCB(ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_CONTROL_POINT_VALUE);
        ATTDB_RemoveRecordCB(ATT_HANDLE_CHARACTERISTIC_TIME_UPDATE_STATE_VALUE);    
        ATTDB_RemoveGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_REFERENCE_TIME_UPDATE, 
                ATT_HANDLE_PRIMARY_SERVICE_REFERENCE_TIME_UPDATE_END);

        ATTDB_RemoveRecord(ATT_HANDLE_PRIMARY_SERVICE_REFERENCE_TIME_UPDATE,
                ATT_HANDLE_PRIMARY_SERVICE_REFERENCE_TIME_UPDATE_END);
        
    }
    if(BleRtusCtx(registerCount) == 0)
    {
        Assert(0);
    }
    BleRtusCtx(registerCount)--;
}




#endif
