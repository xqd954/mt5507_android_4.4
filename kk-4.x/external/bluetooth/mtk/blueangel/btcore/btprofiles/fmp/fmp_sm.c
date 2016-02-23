/*****************************************************************************
 *
 * Filename:
 * ---------
 * fmp_sm.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Find Me Profile Data link and GATT connection handling.
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
#include "gattcb.h"
#include "bt_adp_fs.h"

/*****************************************************************************
 * FUNCTION
 *	HtpCmgrCallback
 *
 * DESCRIPTION
 *	It is called by connection manager with link state events.
 *
 *****************************************************************************/
void FmpCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status) 
{
    FmpChannel *channel = ContainingRecord(Handler, FmpChannel, cmgr_handler);
    kal_trace(BT_TRACE_BLE_PROFILES, FMP_CMGRCALLBACK_EVENT_STATUS, Event, Status);
    switch (Event)
    {
        case CMEVENT_DATA_LINK_CON_CNF:
            if (channel == NULL)
            {
                kal_trace(BT_TRACE_BLE_PROFILES, FMP_NOT_FOR_THE_DEVICE);
                return;			
            }
    
            if (channel->cmgr_handler.bdc == 0)
            {
                kal_trace(BT_TRACE_BLE_PROFILES, FMP_CMGRCALLBACK_BDADDR0);
                return;			
            }
    
            if (Status != BT_STATUS_SUCCESS)
            {
                FmpClearConnection(channel, Status);
                return;			
            }
            else
            {
                if (channel->state == FMP_STATE_OPENING)
                {
                    BtStatus status;
                    status = GattClientConnect(channel->cmgr_handler.remDev);
                    
                    kal_trace(BT_TRACE_BLE_PROFILES, FMP_OPENGATT_STATUS, status); 
                    if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING)
                    {
                        FmpClearConnection(channel, Status);
                    }
                }
            }
            break;	
    
        default:
            break; 		
    }
}

/*****************************************************************************
 * FUNCTION
 *  FmpGattEventCallback
 *
 * DESCRIPTION
 *  It is used to handle GATT event, including BT_GATT_APP_EVENT_CONNECTED, BT_GATT_APP_EVENT_DISCONNECTED.
 *
 *****************************************************************************/
void FmpGattEventCallback(void *handler, BtGattEvent *event)
{
    FmpChannel *channel;

    kal_trace(BT_TRACE_BLE_PROFILES, FMP_GATTEVENTCALLBACK_EVENT, event->eType);
    channel = FmpFindChannelByLink(event->link);

    switch (event->eType)
    {
        case BT_GATT_APP_EVENT_CONNECTED:
            if (channel == NULL)
            {
                if (FMP(service_flag) == 0)
                {
                    return;
                }

                channel = FmpNewChannel();
                if (channel == NULL)		
                {
                    GattDisconnect(event->link);
                    return;					
                }
            }
            channel->state = FMP_STATE_OPEN;
            channel->link = event->link;
            FmpAppCallback(channel, FMPEVENT_OPEN, BT_STATUS_SUCCESS, event->link, 0);			
            break;			

        case BT_GATT_APP_EVENT_DISCONNECTED:
            if (channel == NULL)
               return;
            FmpClearConnection(channel, BT_STATUS_SUCCESS);			
            break;

        default:
            kal_trace(BT_TRACE_BLE_PROFILES, FMP_GATTEVENTCALLBACK_UNEXPECTED_EVENT);			
            break;
    }

}

void FmpBondingCallback(U8 status, U8 *bd_addr)
{
    switch (status)
    {
        case ATT_SEC_DEVICE_BOND:
            /* Device Bonded or Bonded again*/
            {
                FmpChannel *channel;
                if (((channel = FmpFindChannelByAddr((BD_ADDR *)bd_addr)) != NULL) &&
                    (channel->servicedb != NULL))
                {
                    FmpRemoveServiceDB((BD_ADDR *)bd_addr);     // in case that it is bonded again without unbond procedure
                    FmpWriteServiceDB(channel->servicedb);
                }
            }
            break;
        case ATT_SEC_DEVICE_UNBOND:
            /* Device Un Bonded */
            FmpRemoveServiceDB((BD_ADDR *)bd_addr);
            break;
        case ATT_SEC_DEVICE_ALL_UNBOND:
            /* All Device Un Bonded */
            btmtk_fs_delete((const U8 *)FMP_SERVICE_DATABASE_FILE);
            break;
    }
}

#endif

