/*****************************************************************************
 *
 * Filename:
 * ---------
 * fmp_gatt.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Find Me Profile GATT related handling.
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
#include "sys/fmpi.h"
#include "attdef.h"
#include "attdb.h"
#include "gattcb.h"
#include "sec.h"


#ifdef __BT_FMPL_PROFILE__
/*****************************************************************************
 * FUNCTION
 *  HtpcFindAllPrimaryServices
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  After GATT bearer is connected, try to discover HTS and DIS service group handle range.
 *
 *****************************************************************************/
BtStatus FmplFindPrimaryServicesByUuid(FmpChannel *channel, U16 uuid, U16 start_handle, U16 end_handle)
{
    FmpPacket *packet;
    BtGattOp *op;
    BtGattDiscoveryPrimaryServiceByUUID *parm;
    BtStatus status;

    packet = FmpGetPacket(channel);
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    op = &packet->gatt_op;

    InitializeListEntry(&op->op.node);	
    op->apCb = FmpGattOpCallback;

    parm = &packet->parm.primaryServiceByUUID;
    parm->startHandle = start_handle;
    parm->endHandle = end_handle;
    parm->type = sizeof(parm->uuid16); 	
    parm->uuid16[0] = uuid & 0xff;		/* low  order*/
    parm->uuid16[1] = uuid >> 8;		/* high  order*/
    op->parms.p.primaryServiceByUUID = parm;
	
    status = BT_GATT_DiscoveryPrimaryServiceByUUID(channel->link, op);
    if (status != BT_STATUS_PENDING)
    {
        FmpReleasePacket(channel, packet);
    }
    return status;
}

BtStatus FmplFindCharacteristicByUuid(FmpChannel *channel, U16 uuid, U16 start_handle, U16 end_handle)
{
    FmpPacket *packet;
    BtGattOp *op;
    BtGattDiscoveryCharacteristicByUUID *parm;
    BtStatus status;

    packet = FmpGetPacket(channel);
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    op = &packet->gatt_op;

    InitializeListEntry(&op->op.node);	
    op->apCb = FmpGattOpCallback;

    parm = &packet->parm.charByUUID;
    parm->startHandle = start_handle;
    parm->endHandle = end_handle;
    parm->type = sizeof(parm->uuid16); 	
    parm->uuid16[0] = uuid & 0xff;		/* low  order*/
    parm->uuid16[1] = uuid >> 8;		/* high  order*/
    op->parms.p.charByUUID = parm;

    status = BT_GATT_DiscoveryCharacteristicByUUID(channel->link, op);
    if (status != BT_STATUS_PENDING)
    {
        FmpReleasePacket(channel, packet);
    }
    return status;
}

BtStatus FmplWriteWithoutResponse(FmpChannel *channel, U16 handle, U8 *data, U16 len)
{
    FmpPacket *packet;
    BtGattOp *op;
    BtGattWriteWithoutResponse *parm;
    BtStatus status;

    packet = FmpGetPacket(channel);
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    op = &packet->gatt_op;

    InitializeListEntry(&op->op.node);	
    op->apCb = FmpGattOpCallback;

    parm = &packet->parm.writeWithoutResponse;
    parm->handle = handle;
    parm->data = data;
    parm->size = len; 	
    op->parms.p.writeWithoutResponse = parm;

    status = BT_GATT_WriteWithoutResponse(channel->link, op);
    if (status != BT_STATUS_PENDING)
    {
        FmpReleasePacket(channel, packet);
    }
    return status;
}



void FmplHandleFindPrimaryServicesByUuidCb(BtGattOp *op, BtGattEvent *event)
{
    BtGattDiscoveryPrimaryServiceByUUIDResultEvent *result;
    FmpChannel *channel;
    FmpPacket *packet;
    BtStatus status;
    /* It is FALSE when only "attribute not found" error response is received, 
    *   which is the real error response but not end-up sign.
    */ 
    static BT_BOOL found = FALSE;   

    result = (BtGattDiscoveryPrimaryServiceByUUIDResultEvent *) event->parms;

    channel = FmpFindChannelByLink(event->link);
    packet = ContainingRecord(op, FmpPacket, gatt_op);

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            found = TRUE;
            kal_trace(BT_TRACE_BLE_PROFILES, FMP_FINDPRIMARYSERVICE, result->startHandle, result->endHandle);
            channel->servicedb->start_handle = result->startHandle;
            channel->servicedb->end_handle = result->endHandle;
            break;
            
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:   // end up when "attribute not found" error response received
            FmpReleasePacket(channel, packet);
        
            if (!found)
            {
                FmpAppCallback(channel, FMPEVENT_GATT_QUERY_CNF, BT_STATUS_FAILED, 0, 0);           
            }
            else
            {
                found = FALSE;
                status = FmplFindCharacteristicByUuid(channel, BT_UUID_PROXIMITY_ALERT_LEVEL, 
                                            channel->servicedb->start_handle, channel->servicedb->end_handle);
                if (status != BT_STATUS_PENDING && status !=  BT_STATUS_SUCCESS)
                {
                    FmpAppCallback(channel, FMPEVENT_GATT_QUERY_CNF, status, 0, 0);
                }
            }
            break;

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            found = FALSE;
            FmpReleasePacket(channel, packet);
            FmpAppCallback(channel, FMPEVENT_GATT_QUERY_CNF, BT_STATUS_FAILED, 0, 0);           
            break;
    }
}

void FmplHandleFindCharacteristicByUuidCb(BtGattOp *op, BtGattEvent *event)
{
    BtGattDiscoveryCharacteristicByUUIDResultEvent *result;
    FmpChannel *channel;
    FmpPacket *packet;
    /* It is FALSE when only "attribute not found" error response is received, 
    *   which is the real error response but not end-up sign.
    */ 
    static BT_BOOL found = FALSE;   

    result = (BtGattDiscoveryCharacteristicByUUIDResultEvent *) event->parms;

    channel = FmpFindChannelByLink(event->link);
    packet = ContainingRecord(op, FmpPacket, gatt_op);

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            found = TRUE;
            kal_trace(BT_TRACE_BLE_PROFILES, FMP_FINDCHARATERISTIC, result->charValueAttributeHandle, result->charProperty, result->charUUID);
            channel->servicedb->char_value_handle = result->charValueAttributeHandle;
            break;
            
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:   // end up when "attribute not found" error response received
            FmpReleasePacket(channel, packet);
        
            if (!found)
            {
                FmpAppCallback(channel, FMPEVENT_GATT_QUERY_CNF, BT_STATUS_FAILED, 0, 0);           
            }
            else
            {
                BtDeviceRecord record;
                found = FALSE;
                if (SEC_FindDeviceRecord(&channel->link->bdAddr, &record) == BT_STATUS_SUCCESS)
                {
                    FmpWriteServiceDB(channel->servicedb);
                }
                FmpAppCallback(channel, FMPEVENT_GATT_QUERY_CNF, BT_STATUS_SUCCESS, 0, 0);           
            }
            break;

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            found = FALSE;
            FmpReleasePacket(channel, packet);
            FmpAppCallback(channel, FMPEVENT_GATT_QUERY_CNF, BT_STATUS_FAILED, 0, 0);           
            break;
    }
}

void FmplHandleWriteWithoutResponseCb(BtGattOp *op, BtGattEvent *event)
{
    BtGattWriteWithoutResponseResultEvent *result;
    BtGattWriteWithoutResponse *parm = op->parms.p.writeWithoutResponse;
    FmpChannel *channel;
    FmpPacket *packet;
    BtStatus status = BT_STATUS_FAILED;

    result = (BtGattWriteWithoutResponseResultEvent *) event->parms;
    channel = FmpFindChannelByLink(event->link);

    packet = ContainingRecord(op, FmpPacket, gatt_op);

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            status = BT_STATUS_SUCCESS;
            break;			

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            status = BT_STATUS_FAILED;
            break;			
    }
    
    FmpAppCallback(channel, FMPEVENT_SET_ALERT_LEVEL_CNF, status, parm->data, parm->size);          
    fmp_free(parm->data);
    FmpReleasePacket(channel, packet);
}

#endif

#ifdef __BT_FMPT_PROFILE__

U8 FmptAlertLevelCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw)
{
    FmpChannel * channel = FmpFindChannelByLink(link);
    BtStatus status = BT_STATUS_SUCCESS;

    switch (type)
    {
        case ATT_OP_WRITE:
        case ATT_OP_EXECUTE_WRITE:
            kal_trace(BT_TRACE_BLE_PROFILES, FMP_ALERTLEVEL_WRITE, **raw);
            FmpAppCallback(channel, FMPEVENT_SET_ALERT_LEVEL_REQ, BT_STATUS_SUCCESS, (void *)*raw, *len);
            break;
        default:			
            break;
    }
    return (U8)status;
}

#endif

#if defined (__BT_FMPL_PROFILE__) || defined (__BT_FMPT_PROFILE__)

/*****************************************************************************
 * FUNCTION
 *  FmpGattOpCallback
 *
 * DESCRIPTION
 *  It is used to handle GATT operation response, including
 *  BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID, 
 *  BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID, 
 *  BT_GATT_OPERATOR_WRITE_WITHOUT_RESPONSE,
 *
 *****************************************************************************/
void FmpGattOpCallback(void *userdata, BtGattEvent *event)
{
    kal_trace(BT_TRACE_BLE_PROFILES, FMP_GATTOPCALLBACK_EVENT, event->eType, ((U8 *)event->parms)[0]);

    switch (event->eType)
    {
        /* It is for Locator Operation callback */
        case BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID:
            FmplHandleFindPrimaryServicesByUuidCb((BtGattOp *)userdata, event);
            break;
        case BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID:
            FmplHandleFindCharacteristicByUuidCb((BtGattOp *)userdata, event);
            break;
        case BT_GATT_OPERATOR_WRITE_WITHOUT_RESPONSE:
            FmplHandleWriteWithoutResponseCb((BtGattOp *)userdata, event);
            break;
        default:
            break;
    }
}
#endif
