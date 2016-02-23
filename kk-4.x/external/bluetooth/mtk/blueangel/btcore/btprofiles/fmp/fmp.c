/*****************************************************************************
 *
 * Filename:
 * ---------
 * fmp.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Find Me Profile.
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
#if defined(__BT_FMPL_PROFILE__) || defined(__BT_FMPT_PROFILE__)

#include "sys/fmpi.h"
#include "attdb.h"
#include "attdbmain.h"
#include "sec.h"

#if XA_CONTEXT_PTR == XA_ENABLED
static FmpContext      tempContext;
FmpContext          *fmpcx = &tempContext;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
FmpContext           fmpcx;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

extern U8 FmpAttDB_Registration(void);
extern void FmpAttDB_Deregistration(void);



void FMP_Init(void)
{
    OS_LockStack();

#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8 *)fmpcx, 0, sizeof(FmpContext));
#else /* XA_CONTEXT_PTR == XA_ENABLED */
    OS_MemSet((U8 *)&fmpcx, 0, sizeof(FmpContext));
#endif /* XA_CONTEXT_PTR == XA_ENABLED */
    
    /* Initialize Lists */
    InitializeListHead(&FMP(dev_list));
    
    kal_trace(BT_TRACE_BLE_PROFILES, FMP_INITIALIZED);
    
    OS_UnlockStack();
    
}

BtStatus FMP_Register(FmpCallback callback, FmpServiceTypeFlag service_flag)
{
    BtStatus status = BT_STATUS_FAILED;

    OS_LockStack();

    if ((service_flag & FMP_SERVICE_TYPE_LOCATOR_FLAG) &&
	((FMP(service_flag) & FMP_SERVICE_TYPE_LOCATOR_FLAG) == 0))
    {
        status = BT_STATUS_SUCCESS;		
    }

    if ((service_flag & FMP_SERVICE_TYPE_TARGET_FLAG) &&
	((FMP(service_flag) & FMP_SERVICE_TYPE_TARGET_FLAG) == 0))
    {
        status = FmpAttDB_Registration();
    }

    if (status == BT_STATUS_SUCCESS)
    {
        if (FMP(service_flag) == 0)
        {
            /* register the GATT callback to handle connection */
            BT_GattRegisterConnectedEvent(&(FMP(gatt_handler)), FmpGattEventCallback);

            /* register bond callback to obtain bond status */
            FMP(bond_handler).callback = FmpBondingCallback;
            ATTDB_RegisterBondStatus(&FMP(bond_handler));

            FMP(fmp_callback) = callback;
        }
        FMP(service_flag) |= service_flag;
    }

    OS_UnlockStack();
    return status;	
}

BtStatus FMP_Deregister(FmpServiceTypeFlag service_flag)
{
    BtStatus status = BT_STATUS_SUCCESS;

    OS_LockStack();

    if (FMP(service_flag) == 0)
    {
        /* It is actually invalid deregistration, return directly */
        return status;
    }

    if ((service_flag & FMP_SERVICE_TYPE_LOCATOR_FLAG) &&
	(FMP(service_flag) & FMP_SERVICE_TYPE_LOCATOR_FLAG))
    {
        FMP(service_flag) &= ~FMP_SERVICE_TYPE_LOCATOR_FLAG;
    }
	
    if ((service_flag & FMP_SERVICE_TYPE_TARGET_FLAG) &&
	(FMP(service_flag) & FMP_SERVICE_TYPE_TARGET_FLAG))
    {
        FmpAttDB_Deregistration();
        FMP(service_flag) &= ~FMP_SERVICE_TYPE_TARGET_FLAG;
    }

	
    if (FMP(service_flag) == 0)
    {
        BT_GattUnregisterConnectedEvent(&FMP(gatt_handler));
        ATTDB_UnRegisterBondStatus(&FMP(bond_handler));        
        FMP(fmp_callback) = NULL;
    }

    OS_UnlockStack();
    return status;	
}

BtStatus FMP_Open(FmpChannel **channel, BD_ADDR *addr)
{
    BtStatus status = BT_STATUS_FAILED;
    FmpChannel *ch; 

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, addr != 0);

    OS_LockStack();

    kal_trace(BT_TRACE_BLE_PROFILES, FMP_OPENCONNECTION);
    if ((*channel != NULL) && IsNodeOnList(&FMP(dev_list), &((*channel)->node)))
    {
        ch = *channel;
    }
    else
    {
        ch = FmpNewChannel();
        if (ch == NULL)
        {
            return BT_STATUS_NO_RESOURCES;
        }
    }

    if (ch->state == FMP_STATE_CLOSED)
    {
        status = CMGR_CreateDataLink(&ch->cmgr_handler, addr);
        kal_trace(BT_TRACE_BLE_PROFILES, FMP_CREATEDATALINK_STATUS, status);
        if (status == BT_STATUS_SUCCESS)
        {
            status = GattClientConnect(ch->cmgr_handler.remDev);
        }

        if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING)
        {
            FmpFreeChannel(ch);
            return status;          
        }

        ch->state = FMP_STATE_OPENING;
        *channel = ch;      
    }

    OS_UnlockStack();
    return status;
}


BtStatus FMP_Close(FmpChannel *channel)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, channel != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, 
                    IsNodeOnList(&FMP(dev_list), &(channel->node)));

    // check state
    if (channel->state == FMP_STATE_OPEN)
    {
        status = GattDisconnect(channel->link);
    }

    if (status == BT_STATUS_SUCCESS)
    {
        FmpClearConnection(channel, status);
    }


    OS_UnlockStack();
    return status;
}


void FMPL_QueryGattService(FmpChannel *channel)
{
    BtStatus status;
    BtDeviceRecord record;

    OS_LockStack();
    
    /* check service database first */
    if (SEC_FindDeviceRecord(&channel->link->bdAddr, &record) == BT_STATUS_SUCCESS) // bonded device
    {
        if (FmpReadServiceDB(&channel->link->bdAddr, &channel->servicedb) == BT_STATUS_SUCCESS)
        {
            FmpAppCallback(channel, FMPEVENT_GATT_QUERY_CNF, BT_STATUS_SUCCESS, 0, 0);
            return;     
        }
    }
    else
    {
        FmpRemoveServiceDB(&channel->link->bdAddr);
    }

    channel->servicedb = (FmpServiceDB *)fmp_malloc(sizeof(FmpServiceDB));
    if (channel->servicedb == NULL)
    {
        FmpAppCallback(channel, FMPEVENT_GATT_QUERY_CNF, BT_STATUS_NO_RESOURCES, 0, 0);
        return;		
    }

    OS_MemSet((U8 *)channel->servicedb, 0, sizeof(FmpServiceDB));
    channel->servicedb->bt_addr = channel->link->bdAddr;

    kal_trace(BT_TRACE_BLE_PROFILES, FMP_QUEREYGATTSERVICE);
    /* search IAS service */
    status = FmplFindPrimaryServicesByUuid(channel, BT_UUID_GATT_SERVICE_ALERT, 0x0001, 0xffff);

    if (status != BT_STATUS_PENDING && status !=  BT_STATUS_SUCCESS)
    {
        fmp_free(channel->servicedb);
        FmpAppCallback(channel, FMPEVENT_GATT_QUERY_CNF, status, 0, 0);
    }

    OS_UnlockStack();
}


BtStatus FMPL_SetAlertLevel(FmpChannel *channel, FmpAlertLevel level)
{
    U8 *level_data;

    level_data = (U8 *)fmp_malloc(sizeof(FmpAlertLevel));
    *level_data = level;

    if (channel->servicedb == NULL)
    {
        return BT_STATUS_FAILED;
    }

    return FmplWriteWithoutResponse(channel, channel->servicedb->char_value_handle, level_data, sizeof(FmpAlertLevel));
}

#endif
