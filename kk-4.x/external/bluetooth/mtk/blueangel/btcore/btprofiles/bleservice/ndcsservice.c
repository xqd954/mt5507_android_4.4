/*****************************************************************************
 *
 * Filename:
 * ---------
 * ndcsservice.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Next DST change service
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
#include "ndcsservice.h"

BleNdcs_T BleNdcxServiceCtx;

#define BleNdcsCtx(f)  (BleNdcxServiceCtx.f)


/* Next DST Change service */
const U8 attNextDstRecord[] =
{
	ATT_HANDLE16(ATT_HANDLE_PRIMARY_SERVICE_NEXT_DST_CHANGE,
			BT_UUID_GATT_TYPE_PRIMARY_SERVICE,
			BT_ATT_CONST_VARIABLE, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
	ATT_SETUUID16(BT_UUID_GATT_SERVICE_DST),

	/* Time with DST characteristic */
	ATT_HANDLE16(ATT_HANDLE_CHARACTERISTIC_TIME_WITH_DST,
			BT_UUID_GATT_TYPE_CHARACTERISTIC,
			BT_ATT_CONST_VARIABLE, 0x05),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
	ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ,
			ATT_HANDLE_CHARACTERISTIC_TIME_WITH_DST_VALUE,
			BT_UUID_TIME_WITH_DST),

	ATT_HANDLE16(ATT_HANDLE_CHARACTERISTIC_TIME_WITH_DST_VALUE,
			BT_UUID_TIME_WITH_DST,
			BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x08),
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
};


void NdcsRegisterAttdbCallback(U16 handle, BtNdcsServiceCbHandler *handler)
{
    switch (handle)
    {
        case ATT_HANDLE_CHARACTERISTIC_TIME_WITH_DST_VALUE:
            InsertTailList((ListEntry *)&(BleNdcsCtx(NdcsTimeWithDst)), &(handler->node));
            break;
              
    }

}

void NdcsDeregisterAttdbCallback(BtNdcsServiceCbHandler *handler)
{
    RemoveEntryList((ListEntry *)&(handler->node));
}


U8 NdcsWithDscServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtNdcsServiceCbHandler *handler;

    entry = BleNdcsCtx(NdcsTimeWithDst).node.Flink;
    while (entry && entry != (ListEntry *)&BleNdcsCtx(NdcsTimeWithDst))
    {
        handler = ((BtNdcsServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, len, raw);
    }
    return BT_STATUS_SUCCESS;

}

void NdcsInitialize(void)
{
    OS_MemSet((U8 *)&BleNdcxServiceCtx, 0, sizeof(BleNdcxServiceCtx));
    InitializeListHead((ListEntry *)&(BleNdcsCtx(NdcsTimeWithDst)));
    
}


U8 NdcsActivateService(void)
{
    BtStatus status; 
    
    Report(("BleNdcsCtx(registerCount) in NdcsActivateService:%d",BleNdcsCtx(registerCount)));
    if(BleNdcsCtx(registerCount) == 0)
    {
        status = ATTDB_AddRecord(attNextDstRecord, sizeof(attNextDstRecord));
        ATTDB_AddRecordCB(ATT_HANDLE_CHARACTERISTIC_TIME_WITH_DST_VALUE, NdcsWithDscServiceCb);
        ATTDB_AddGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_NEXT_DST_CHANGE, 
                ATT_HANDLE_PRIMARY_SERVICE_NEXT_DST_CHANGE_END, 
                BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    }
    BleNdcsCtx(registerCount)++;
    return BT_STATUS_SUCCESS;
}

void NdcsDeactivateService(void)
{
    Report(("BleNdcsCtx(registerCount) in NdcsDeactivateService:%d",BleNdcsCtx(registerCount)));

    if(BleNdcsCtx(registerCount) == 1)
    {
        ATTDB_RemoveRecordCB(ATT_HANDLE_CHARACTERISTIC_TIME_WITH_DST_VALUE);
        ATTDB_RemoveRecord(ATT_HANDLE_PRIMARY_SERVICE_NEXT_DST_CHANGE,
                ATT_HANDLE_PRIMARY_SERVICE_NEXT_DST_CHANGE_END);
        ATTDB_RemoveGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_NEXT_DST_CHANGE, 
                ATT_HANDLE_PRIMARY_SERVICE_NEXT_DST_CHANGE_END);
        
    }
    if(BleNdcsCtx(registerCount) == 0)
    {
        Assert(0);
    }
    BleNdcsCtx(registerCount)--;
}




#endif
