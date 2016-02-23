/*****************************************************************************
 *
 * Filename:
 * ---------
 * llsservice.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Link Lost Service
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
#include "llsservice.h"

BleLls_T BleLlxServiceCtx ;

#define BleLlsCtx(f)  (BleLlxServiceCtx.f)


/* Link Lost service */

const U8 attLlsRecord[] = 
{
    /* Link Loss  */  
    ATT_HANDLE16(ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS, BT_UUID_GATT_TYPE_PRIMARY_SERVICE, BT_ATT_CONST_VARIABLE, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETUUID16(BT_UUID_GATT_SERVICE_LINKLOSS),

    ATT_HANDLE16(ATT_HANDLE_LL_CHARACTERISTIC_ALERT_LEVEL, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE, ATT_HANDLE_LL_CHARACTERISTIC_ALERT_LEVEL_VALUE, BT_UUID_PROXIMITY_ALERT_LEVEL),
        
    ATT_HANDLE16(ATT_HANDLE_LL_CHARACTERISTIC_ALERT_LEVEL_VALUE, BT_UUID_PROXIMITY_ALERT_LEVEL, BT_ATT_FIX_VARIABLE_IN_UPPER_AP, 0x01), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
};


void LlsRegisterAttdbCallback(U16 handle, BtLlsServiceCbHandler *handler)
{
    switch (handle)
    {
        case ATT_HANDLE_LL_CHARACTERISTIC_ALERT_LEVEL_VALUE:
            InsertTailList((ListEntry *)&(BleLlsCtx(LinkLostAlertList)), &(handler->node));
            break;
              
    }

}


void LlsDeregisterAttdbCallback(BtLlsServiceCbHandler *handler)
{
    RemoveEntryList((ListEntry *)&(handler->node));
}


U8 LlsLinkLostAlertServiceCb(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    ListEntry *entry;
    BtLlsServiceCbHandler *handler;

    entry = BleLlsCtx(LinkLostAlertList).node.Flink;
    while (entry && entry != (ListEntry *)&BleLlsCtx(LinkLostAlertList))
    {
        handler = ((BtLlsServiceCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(type, link, handle, len, raw);
    }
    return BT_STATUS_SUCCESS;

}

void LlsInitialize(void)
{
    OS_MemSet((U8 *)&BleLlxServiceCtx, 0, sizeof(BleLlxServiceCtx));
    InitializeListHead((ListEntry *)&(BleLlsCtx(LinkLostAlertList)));
    
}


U8 LlsActivateService(void)
{
    BtStatus status; 
    
    Report(("BleLlsCtx(registerCount) in LlsActivateService:%d",BleLlsCtx(registerCount)));
    if(BleLlsCtx(registerCount) == 0)
    {
        status = ATTDB_AddRecord(attLlsRecord, sizeof(attLlsRecord));
        ATTDB_AddRecordCB(ATT_HANDLE_LL_CHARACTERISTIC_ALERT_LEVEL_VALUE, LlsLinkLostAlertServiceCb);
        ATTDB_AddGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS, 
                             ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS_END, 
                             BT_UUID_GATT_TYPE_PRIMARY_SERVICE);


    }
    BleLlsCtx(registerCount)++;
    Report(("LL2 active done"));
    return BT_STATUS_SUCCESS;
}

void LlsDeactivateService(void)
{
    Report(("BleLlsCtx(registerCount) in LlsDeactivateService:%d",BleLlsCtx(registerCount)));

    if(BleLlsCtx(registerCount) == 1)
    {
        ATTDB_RemoveRecordCB(ATT_HANDLE_LL_CHARACTERISTIC_ALERT_LEVEL_VALUE);
        ATTDB_RemoveGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS, 
                                ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS_END);
        ATTDB_RemoveRecord(ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS, ATT_HANDLE_PRIMARY_SERVICE_LINK_LOSS_END);
    }
    if(BleLlsCtx(registerCount) == 0)
    {
        Assert(0);
    }
    BleLlsCtx(registerCount)--;
}




#endif
