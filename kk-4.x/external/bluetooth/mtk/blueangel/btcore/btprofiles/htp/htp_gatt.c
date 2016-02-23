/*****************************************************************************
 *
 * Filename:
 * ---------
 * htp_gatt.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Health Thermometer Profile GATT related handling.
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

#include "sys/htpi.h"
#include "attdef.h"
#include "attdb.h"
#include "att.h"
#include "btconfig_le.h"
#include "gattcb.h"



#ifdef __BT_HTPC_PROFILE__

#define HTPC_READ_CHAR_RETRY_TIMER      (1500)
#define HTPC_READ_CHAR_RETRY_COUNT      (10)

/*****************************************************************************
 * FUNCTION
 *  HtpcFindAllPrimaryServices
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  After GATT bearer is connected, try to discover HTS and DIS service group handle range.
 *
 *****************************************************************************/
BtStatus HtpcFindPrimaryServicesByUuid(HtpChannel *channel, U16 uuid, U16 start_handle, U16 end_handle)
{
    HtpPacket *packet;
    BtGattOp *op;
    BtGattDiscoveryPrimaryServiceByUUID *parm;
    BtStatus status;

    packet = HtpGetPacket(channel);
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    op = &packet->gatt_op;

    InitializeListEntry(&op->op.node);	
    op->apCb = HtpGattOpCallback;

    parm = &packet->parm.primaryServiceByUUID;
    parm->startHandle = start_handle;
    parm->endHandle = end_handle;
    parm->type = sizeof(parm->uuid16); 	
    parm->uuid16[0] = uuid & 0xff;		/* low  order*/
    parm->uuid16[1] = uuid >> 8;		/* high  order*/
    op->parms.p.primaryServiceByUUID = parm;
	
    status = BT_GATT_DiscoveryPrimaryServiceByUUID(channel->cmgr_handler.remDev, op);
    if (status != BT_STATUS_PENDING)
    {
        HtpReleasePacket(channel, packet);
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HtpcFindAllPrimaryServices
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  After service group handle range is available, try to discover all characteristic 
 *  declarations in the range.
 *
 *****************************************************************************/
BtStatus HtpcFindAllCharacteristics(HtpChannel *channel, U16 start_handle, U16 end_handle)
{
    HtpPacket *packet;
    BtGattOp *op;
    BtGattDiscoveryAllCharacteristicOfService *parm;
    BtStatus status;
    
    packet = HtpGetPacket(channel);
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    op = &packet->gatt_op;
    
    InitializeListEntry(&op->op.node);	
    op->apCb = HtpGattOpCallback;
    
    parm = &packet->parm.allChars;
    parm->startHandle = start_handle;
    parm->endHandle = end_handle;
    op->parms.p.allCharOfServices = parm;
    
    status = BT_GATT_DiscoveryAllCharacteristicOfService(channel->cmgr_handler.remDev, op);
    if (status != BT_STATUS_PENDING)
    {
        HtpReleasePacket(channel, packet);
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HtpcFindAllCharacteristicDescriptors
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  After characteristic declaration is obtained, try to find all descriptors
 *  in each characteristic group range.
 *
 *****************************************************************************/
BtStatus HtpcFindAllCharacteristicDescriptors(HtpChannel *channel, U16 start_handle, U16 end_handle)
{
    HtpPacket *packet;
    BtGattOp *op;
    BtGattDiscoveryAllCharacteristicDescriptors *parm;
    BtStatus status;

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_FIND_ALLCHARDESC, start_handle, end_handle);
	
    packet = HtpGetPacket(channel);
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    op = &packet->gatt_op;
    
    InitializeListEntry(&op->op.node);	
    op->apCb = HtpGattOpCallback;
    
    parm = &packet->parm.allCharDescriptors;
    parm->startHandle = start_handle;
    parm->endHandle = end_handle;
    op->parms.p.allCharDescriptors = parm;
    
    status = BT_GATT_DiscoveryAllCharacteristicDescriptors(channel->cmgr_handler.remDev, op);
    if (status != BT_STATUS_PENDING)
    {
        HtpReleasePacket(channel, packet);
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HtpcReadCharacteristic
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  After characteristic declaration is obtained, try to read characteristic value.
 *
 *****************************************************************************/
BtStatus HtpcReadCharacteristic(HtpChannel *channel, U16 handle)
{
    HtpPacket *packet;
    BtGattOp *op;
    BtGattReadCharacteristicValue *parm;
    BtStatus status;
    
    packet = HtpGetPacket(channel);
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_READ_CHAR_HANDLE, handle);

    op = &packet->gatt_op;
    
    InitializeListEntry(&op->op.node);	
    op->apCb = HtpGattOpCallback;
    
    parm = &packet->parm.readCharValue;
    parm->handle = handle;
    op->parms.p.readCharValue = parm;
    
    status = BT_GATT_ReadCharacteristicValue(channel->cmgr_handler.remDev, op);
    if (status != BT_STATUS_PENDING)
    {
        HtpReleasePacket(channel, packet);
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HtpcReadCharacteristicDescriptor
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  After descriptor handle is obtained, try to read descriptor value.
 *
 *****************************************************************************/
BtStatus HtpcReadCharacteristicDescriptor(HtpChannel *channel, U16 handle)
{
    HtpPacket *packet;
    BtGattOp *op;
    BtGattReadCharacteristicDescriptors *parm;
    BtStatus status;
    
    packet = HtpGetPacket(channel);
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_READ_CHARDESC_HANDLE, handle);

    op = &packet->gatt_op;
    
    InitializeListEntry(&op->op.node);	
    op->apCb = HtpGattOpCallback;
    
    parm = &packet->parm.readCharDescriptors;
    parm->handle = handle;
    op->parms.p.readCharDescriptors = parm;
    
    status = BT_GATT_ReadCharacteristicDescriptors(channel->cmgr_handler.remDev, op);
    if (status != BT_STATUS_PENDING)
    {
        HtpReleasePacket(channel, packet);
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HtpcReadCharacteristicEx
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  Try to read characteristic value, if it is not read permitted, try to read the next one.
 *  If no more characteristic value is read permitted, try to find descriptors in the first
 *  characteristic definition.
 *
 *****************************************************************************/
void HtpcReadCharacteristicEx(HtpChannel *channel, HtpCharListNode *char_node, HtpService *service, HtpServiceDB *servicedb)
{
    BtStatus status;

    while ((ListEntry *)char_node != &service->char_list)
    {
        /* check if it is read permitted */
        if ((char_node->properties & GATT_CHAR_PROP_READ) &&(char_node->value_size == 0))
        {
            kal_trace(BT_TRACE_BLE_PROFILES, HTP_READ_CHAR_NEXTONE, char_node->value_handle);
            status = HtpcReadCharacteristic(channel, char_node->value_handle);
            if (status != BT_STATUS_PENDING && status !=  BT_STATUS_SUCCESS)
            {
                HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, status, 0, 0);
            }
            return;
        }
        char_node = (HtpCharListNode *)GetNextNode(&char_node->node);
    }
    
    /* Here, no more characteristic value is read permitted
    *   HTP: Continue to find descriptor or read descriptor if descriptor handle is existed
    *   DIS:  Query done.
    */
    kal_trace(BT_TRACE_BLE_PROFILES, HTP_READ_CHAR_LASTONE);
    if (service == &servicedb->hts_service)
    {
        HtpCharListNode *char_next_node;
        char_node = (HtpCharListNode *)GetHeadList(&service->char_list);
        if (char_node->value_uuid == BT_UUID_HTS_CHAR_TEMPERATURE_TYPE)
        {
            // Only Temperature Type Characteristic does not include descriptor, skip it...
            char_node = (HtpCharListNode *)GetNextNode(&char_node->node);
        }
        if (IsListEmpty(&char_node->char_desc_list)) 
        {
            char_next_node = (HtpCharListNode *)GetNextNode(&char_node->node);  
            /* There might be only one characteristic in HTS */
            if ((ListEntry *)char_next_node == &service->char_list)
            {
                status = HtpcFindAllCharacteristicDescriptors(channel, char_node->value_handle + 1, service->end_handle);
            }
            else
            {
                status = HtpcFindAllCharacteristicDescriptors(channel, char_node->value_handle + 1, char_next_node->handle - 1);
            }
        }
        else
        {
            /* Descriptor has been queried before, just read descriptor value directly */
            HtpCharDescListNode *char_desc_node;
            char_desc_node = (HtpCharDescListNode *)GetHeadList(&char_node->char_desc_list); 
            status = HtpcReadCharacteristicDescriptor(channel, char_desc_node->handle);
        }
        
        if (status != BT_STATUS_PENDING && status !=  BT_STATUS_SUCCESS)
        {
            HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, status, 0, 0);
        }
    }
    else
    {
        BtDeviceRecord record;
        if (SEC_FindDeviceRecord(&channel->link->bdAddr, &record) == BT_STATUS_SUCCESS)
        {
            HtpWriteServiceDB(channel->servicedb);
        }
        HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, BT_STATUS_SUCCESS, 
                (void *)&servicedb->features, sizeof(servicedb->features));
    }
}


/*****************************************************************************
 * FUNCTION
 *  HtpcWriteCharacteristic
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  After characteristic declaration is obtained, try to write characteristic value.
 *
 *****************************************************************************/
BtStatus HtpcWriteCharacteristic(HtpChannel *channel, U16 handle, U8 *data, U16 len)
{
    HtpPacket *packet;
    BtGattOp *op;
    BtGattWriteCharValueRequest *parm;
    BtStatus status;
    
    packet = HtpGetPacket(channel);
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_WRITE_CHAR_HANDLE, handle);

    op = &packet->gatt_op;
    
    InitializeListEntry(&op->op.node);	
    op->apCb = HtpGattOpCallback;
    
    parm = &packet->parm.writeRequest;
    parm->handle = handle;
    parm->size = len;
    parm->data = data;
    op->parms.p.writeRequest = parm;
    
    status = BT_GATT_WriteCharacteristicValue(channel->cmgr_handler.remDev, op);
    if (status != BT_STATUS_PENDING)
    {
        HtpReleasePacket(channel, packet);
    }
    else
    {
        HtpIdleConnectionStopTimer();
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HtpcWriteCharacteristicDescriptor
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  After descriptor handle is obtained, try to write descriptor value.
 *
 *****************************************************************************/
BtStatus HtpcWriteCharacteristicDescriptor(HtpChannel *channel, U16 handle, U8 *data, U16 len)
{
    HtpPacket *packet;
    BtGattOp *op;
    BtGattWriteDescriptors *parm;
    BtStatus status;
    
    packet = HtpGetPacket(channel);
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    kal_trace(BT_TRACE_BLE_PROFILES, HTP_WRITE_CHARDESC_HANDLE, handle);

    op = &packet->gatt_op;
    
    InitializeListEntry(&op->op.node);	
    op->apCb = HtpGattOpCallback;
    
    parm = &packet->parm.writeDescriptors;
    parm->handle = handle;
    parm->size = len;
    parm->data = data;
    op->parms.p.writeDescriptors = parm;
    
    status = BT_GATT_WriteCharacteristicDescriptors(channel->cmgr_handler.remDev, op);
    if (status != BT_STATUS_PENDING)
    {
        HtpReleasePacket(channel, packet);
    }
    else
    {
        HtpIdleConnectionStopTimer();
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HtpcHandleGattServiceChangedCb
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  If service handle is changed on server, the cache shall be updated on client.
 *
 *****************************************************************************/
void HtpcHandleGattServiceChangedCb(BtGattOp *op, BtGattEvent *event)
{

}

/*****************************************************************************
 * FUNCTION
 *  HtpcWriteCharacteristicDescriptor
 *
 * DESCRIPTION
 *  It is for Collector only.
 *
 *****************************************************************************/
void HtpcHandleFindPrimaryServicesByUuidCb(BtGattOp *op, BtGattEvent *event)
{
    BtGattDiscoveryPrimaryServiceByUUIDResultEvent *result;
    BtGattDiscoveryPrimaryServiceByUUID *parm = op->parms.p.primaryServiceByUUID;
    HtpChannel *channel;
    HtpServiceDB *servicedb;
    HtpPacket *packet;
    U16 uuid;				
    BtStatus status;
    
    /* It is FALSE when only "attribute not found" error response is received, 
    *   which is the real error response but not end-up sign.
    */ 
    static BT_BOOL found = FALSE;   

    result = (BtGattDiscoveryPrimaryServiceByUUIDResultEvent *) event->parms;

    channel = HtpFindChannelByLink(event->link);
    servicedb = channel->servicedb;

    packet = ContainingRecord(op, HtpPacket, gatt_op);

    /* obtain uuid from operation node in gatt */
    uuid = parm->uuid16[0] | (parm->uuid16[1] << 8); 
	
    kal_trace(BT_TRACE_BLE_PROFILES, HTP_FINDPRIMARYSERVICE_UUID, uuid);	

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            found = TRUE;
            if (uuid == BT_UUID_GATT_SERVICE_HEALTH)
            {
                servicedb->hts_service.start_handle = result->startHandle;
                servicedb->hts_service.end_handle = result->endHandle;
                servicedb->hts_service.size += HTP_SERVICE_SIZE;
            }
            else if (uuid == BT_UUID_GATT_SERVICE_DEVICE)
            {
                servicedb->dis_service.start_handle = result->startHandle;
                servicedb->dis_service.end_handle = result->endHandle;
                servicedb->dis_service.size += HTP_SERVICE_SIZE;
            }
            //servicedb->size += 4;
            break;

        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:   // end up when "attribute not found" error response received
            HtpReleasePacket(channel, packet);

            if (!found)
            {
                HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, BT_STATUS_FAILED, 0, 0);           
            }
            else
            {
                found = FALSE;
                if (uuid == BT_UUID_GATT_SERVICE_HEALTH)
                {
                    status = HtpcFindAllCharacteristics(channel, servicedb->hts_service.start_handle,
                        servicedb->hts_service.end_handle);
                }
                else
                {
                    status = HtpcFindAllCharacteristics(channel, servicedb->dis_service.start_handle,
                        servicedb->dis_service.end_handle);
                }
                if (status != BT_STATUS_PENDING && status !=  BT_STATUS_SUCCESS)
                {
                    HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, status, 0, 0);
                }
            }
            break;

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            found = FALSE;
            HtpReleasePacket(channel, packet);
            HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, BT_STATUS_FAILED, 0, 0);			
            break;
    }

}

/*****************************************************************************
 * FUNCTION
 *  HtpcHandleFindAllCharacteristicsCb
 *
 * DESCRIPTION
 *  It is for Collector only.
 *  Result with characteristic declaration
 *
 *****************************************************************************/
void HtpcHandleFindAllCharacteristicsCb(BtGattOp *op, BtGattEvent *event)
{
    BtGattDiscoveryAllCharacteristicOfServiceResultEvent *result;
    BtGattDiscoveryAllCharacteristicOfService *parm = op->parms.p.allCharOfServices;
    HtpServiceDB *servicedb;
    HtpPacket *packet;
    HtpChannel *channel;
    HtpService *service;
    BtStatus status;
    /* It is FALSE when only "attribute not found" error response is received, 
    *   which is the real error response but not end-up sign.
    */ 
    static BT_BOOL found = FALSE;   

    result = (BtGattDiscoveryAllCharacteristicOfServiceResultEvent *) event->parms;

    channel = HtpFindChannelByLink(event->link);
    servicedb = channel->servicedb;

    packet = ContainingRecord(op, HtpPacket, gatt_op);

    /* get the indicated service list */
    if ((service = HtpFindServicebyHandleRange(servicedb, parm->startHandle)) == NULL)
    {
        kal_trace(BT_TRACE_BLE_PROFILES, HTP_FIND_ALLCHAR_ERROR_HANDLE);
        return;
    }


    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            {
                HtpCharListNode *char_node;

                found = TRUE;
                char_node = (HtpCharListNode *)htp_malloc(sizeof(HtpCharListNode));
                OS_MemSet((U8 *)char_node, 0, sizeof(HtpCharListNode));
                char_node->handle = result->attributeHandle;
                char_node->properties = result->charProperty;
                char_node->value_handle = result->charValueAttributeHandle;
                char_node->value_uuid = result->charUUID;
                InitializeListHead(&char_node->char_desc_list);
                char_node->size = HTP_CHARACTERISTIC_SIZE;

                /* Insert characteristic node into service characteristic list */
                InsertTailList(&service->char_list, &char_node->node);
                service->size += char_node->size;
                //servicedb->size += char_node->size;

                kal_trace(BT_TRACE_BLE_PROFILES, HTP_FIND_ALLCHAR_INFO, result->attributeHandle, result->charProperty,
                                result->charValueAttributeHandle, result->charUUID);

                /* Set characteristic feature according to UUID and properties */
                switch (result->charUUID)
                {
                    case BT_UUID_HTS_CHAR_TEMPERATURE_TYPE:
                        servicedb->features |= HTP_CHAR_FEATURE_TEMPERATURE_TYPE_READABLE;
                        break;						
                    case BT_UUID_HTS_CHAR_INTERMEDIATE_TEMPERATURE:
                        servicedb->features |= HTP_CHAR_FEATURE_INTERMEDIATE_MEASUREMENT_NOTIFIABLE;
                        break;						
                    case BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL:
                        servicedb->features |= HTP_CHAR_FEATURE_MEASUREMENT_INTERVAL_READABLE;
                        if (result->charProperty & 0x08) // write
                        {
                            servicedb->features |= HTP_CHAR_FEATURE_MEASUREMENT_INTERVAL_WRITABLE;
                        }
                        if (result->charProperty & 0x20) // indicate
                        {
                            servicedb->features |= HTP_CHAR_FEATURE_MEASUREMENT_INTERVAL_INDICATABLE;
                        }
                        break;						
                    default:
                        break;						
                }
            }
            break;			

        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            {
                HtpCharListNode *char_node;
    
                HtpReleasePacket(channel, packet);

                if (!found)
                {
                    HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, BT_STATUS_FAILED, 0, 0);     
                }
                else
                {
                    found = FALSE;
                    /* Begin to Read characteristic value */
                    char_node = (HtpCharListNode *)GetHeadList(&service->char_list);
                    HtpcReadCharacteristicEx(channel, char_node, service, servicedb);
                }
            }
            break;			

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            found = FALSE;
            HtpReleasePacket(channel, packet);
            HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, BT_STATUS_FAILED, 0, 0);			
            break;			
    }

}

/*****************************************************************************
 * FUNCTION
 *  HtpcHandleFindAllCharacteristicDescriptorsCb
 *
 * DESCRIPTION
 *  It is for Collector only.
 *
 *****************************************************************************/
void HtpcHandleFindAllCharacteristicDescriptorsCb(BtGattOp *op, BtGattEvent *event)
{
    BtGattDiscoveryAllCharacteristicDescriptorsResultEvent *result;
    BtGattDiscoveryAllCharacteristicDescriptors *parm = op->parms.p.allCharDescriptors;
    HtpChannel *channel;
    HtpServiceDB *servicedb;
    HtpService *service;	
    HtpPacket *packet;
    HtpCharListNode *char_node;

    /* It is FALSE when only "attribute not found" error response is received, 
    *   which is the real error response but not end-up sign.
    */ 
    static BT_BOOL found = FALSE;   

    result = (BtGattDiscoveryAllCharacteristicDescriptorsResultEvent *) event->parms;

    channel = HtpFindChannelByLink(event->link);
    servicedb = channel->servicedb;
    service = &servicedb->hts_service;
    char_node = HtpFindCharbyHandleRange(service, result->attributeHandle);

    Report(("[HTP] find descriptor, result handle: 0x%x, param handle: 0x%x", result->attributeHandle, parm->startHandle));
    Report(("[HTP] find descriptor, char : 0x%x", char_node));

    packet = ContainingRecord(op, HtpPacket, gatt_op);

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            {
                HtpCharDescListNode *desc_node;
                U8 desc_len = sizeof(HtpClientCharConfig);				

                found = TRUE;
                desc_node = (HtpCharDescListNode *)htp_malloc(sizeof(HtpCharDescListNode));
                OS_MemSet((U8 *)desc_node, 0, sizeof(HtpCharDescListNode));
                desc_node->handle = result->attributeHandle;
                desc_node->desc_uuid = result->uuid16;

                kal_trace(BT_TRACE_BLE_PROFILES, HTP_FIND_ALLCHARDESC_INFO, result->attributeHandle, result->uuid16);

                /* Insert descriptor into charateristic descriptor list */
                InsertTailList(&char_node->char_desc_list, &desc_node->node);
                char_node->size += HTP_CHARACTERISTIC_DESCRIPTOR_SIZE; 
                service->size += HTP_CHARACTERISTIC_DESCRIPTOR_SIZE;
                //servicedb->size += HTP_CHARACTERISTIC_DESCRIPTOR_SIZE;
            }
            break;			

        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            {
                BtStatus status;				
                HtpReleasePacket(channel, packet);

                if (!found)
                {
                    HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, BT_STATUS_FAILED, 0, 0);  
                    break;
                }
                found = FALSE;

NextChar:
                char_node = (HtpCharListNode *)GetNextNode(&char_node->node);
                if ((ListEntry *)char_node == &service->char_list)
                {
                    HtpCharDescListNode *desc_node;

                    kal_trace(BT_TRACE_BLE_PROFILES, HTP_FIND_ALLCHARDESC_COMPLETE);					
                    /* All descriptors <handle, uuid> have been obtained. Read each descriptor value by handle. */
                    char_node = (HtpCharListNode *)GetHeadList(&service->char_list);
                    desc_node = (HtpCharDescListNode *)GetHeadList(&char_node->char_desc_list);					
                    status = HtpcReadCharacteristicDescriptor(channel, desc_node->handle);
                }
                else
                {
                    if (char_node->value_uuid == BT_UUID_HTS_CHAR_TEMPERATURE_TYPE)
                    {
                        /* there is no descriptor for temperature type characteristic, just try the next node */
                        goto NextChar;
                    }
                    else
                    {
                        HtpCharListNode *char_next_node;
                        char_next_node = (HtpCharListNode *)GetNextNode(&char_node->node);	
                        /* It might be the last one characteristic in HTS */
                        if ((ListEntry *)char_next_node == &service->char_list)
                        {
                            status = HtpcFindAllCharacteristicDescriptors(channel, char_node->value_handle + 1, service->end_handle);
                        }
                        else
                        {
                            status = HtpcFindAllCharacteristicDescriptors(channel, char_node->value_handle + 1, char_next_node->handle - 1);
                        }
                    }
                }
				
                if (status != BT_STATUS_PENDING && status !=  BT_STATUS_SUCCESS)
                {
                    HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, status, 0, 0);
                }
            }
            break;			

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            found = FALSE;
            HtpReleasePacket(channel, packet);
            HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, BT_STATUS_FAILED, 0, 0);			
            break;			
    }
}

void HtpcHandleReadAuthencationFailed(EvmTimer *timer)
{
    HtpChannel *channel;
    HtpServiceDB *servicedb;
    HtpService *service;
    HtpCharListNode *char_node;	
    
    channel = (HtpChannel*)timer->context;
    channel->readCharRetryCount++;
    Report(("HtpcHandleReadAuthencationFailed retry count:%d", channel->readCharRetryCount));
    if(channel->readCharRetryCount > HTPC_READ_CHAR_RETRY_COUNT)
    {
        HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, BT_STATUS_FAILED, 0, 0);      
    }
    else
    {
        servicedb = channel->servicedb;
        service = &servicedb->hts_service; 
        char_node = (HtpCharListNode *)GetHeadList(&service->char_list);
        HtpcReadCharacteristicEx(channel, char_node, service, servicedb);    
    }
   
}


/*****************************************************************************
 * FUNCTION
 *  HtpcHandleReadCharacteristicCb
 *
 * DESCRIPTION
 *  It is for Collector only.
 *
 *****************************************************************************/
void HtpcHandleReadCharacteristicCb(BtGattOp *op, BtGattEvent *event)
{
    BtGattReadCharacteristicValueResultEvent *result;
    BtGattReadCharacteristicValue *parm = op->parms.p.readCharValue;
    HtpChannel *channel;
    HtpServiceDB *servicedb;
    HtpService *service;
    HtpCharListNode *char_node;	
    HtpPacket *packet;

    result = (BtGattReadCharacteristicValueResultEvent *) event->parms;

    channel = HtpFindChannelByLink(event->link);
    servicedb = channel->servicedb;

    packet = ContainingRecord(op, HtpPacket, gatt_op);

    /* get the indicated service list */
    if ((service = HtpFindServicebyHandleRange(servicedb, parm->handle)) == NULL)
    {
        kal_trace(BT_TRACE_BLE_PROFILES, HTP_READ_CHAR_ERROR_HANDLE);
        return;
    }
	
    if ((char_node = HtpFindCharbyHandle(service, parm->handle)) == NULL)				
    {
        return;
    }

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            kal_trace(BT_TRACE_BLE_PROFILES, HTP_READ_CHAR_INFO, parm->handle, result->data[0]);            
            char_node->value = (U8 *)htp_malloc(result->length);
            OS_MemCopy(char_node->value, result->data, result->length);
            char_node->value_size = result->length;
            
            HtpReleasePacket(channel, packet);
            /* read the next characteristic value */
            char_node = (HtpCharListNode *)GetNextNode(&char_node->node);
            HtpcReadCharacteristicEx(channel, char_node, service, servicedb);
            break;			

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            HtpReleasePacket(channel, packet);
            if(result->error_code == BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION)
            {
                EVM_ResetTimer(&channel->readCharTimer);
                channel->readCharTimer.func = HtpcHandleReadAuthencationFailed;
                channel->readCharTimer.context = (void*)channel;
                EVM_StartTimer(&channel->readCharTimer,HTPC_READ_CHAR_RETRY_TIMER);
            }
            else
            {
                HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, BT_STATUS_FAILED, 0, 0);           
            }
            break;			
    }
}

/*****************************************************************************
 * FUNCTION
 *  HtpcHandleReadCharacteristicDescriptorCb
 *
 * DESCRIPTION
 *  It is for Collector only.
 *
 *****************************************************************************/
void HtpcHandleReadCharacteristicDescriptorCb(BtGattOp *op, BtGattEvent *event)
{
    BtGattReadCharacteristicDescriptorsResultEvent *result;
    BtGattReadCharacteristicDescriptors *parm = op->parms.p.readCharDescriptors;
    HtpChannel *channel;
    HtpServiceDB *servicedb;
    HtpPacket *packet;
    HtpService *service;

    result = (BtGattReadCharacteristicDescriptorsResultEvent *) event->parms;

    channel = HtpFindChannelByLink(event->link);
    servicedb = channel->servicedb;
    service = &servicedb->hts_service;

    packet = ContainingRecord(op, HtpPacket, gatt_op);

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            {
                BtStatus status;
                HtpCharListNode *char_node;
                HtpCharDescListNode *desc_node; 

                char_node = HtpFindCharbyHandleRange(service, parm->handle);
                
                /* If there is one more descriptor in characteristic, then GetNextNode is necessary */
                desc_node = (HtpCharDescListNode *)GetHeadList(&char_node->char_desc_list); 

                if (desc_node->handle != parm->handle)
                {
                    /* It might be Measurement Interval descriptor */
                    desc_node = (HtpCharDescListNode *)GetNextNode(&desc_node->node);
                    if (desc_node->handle != parm->handle)
                    {
                        HtpReleasePacket(channel, packet);
                        Report(("[HTP] read characteristic descriptor error handle!"));
                        break;						
                    }
                }
                kal_trace(BT_TRACE_BLE_PROFILES, HTP_READ_CHARDESC_INFO, char_node->value_uuid, desc_node->desc_uuid);
                desc_node->desc = (U8 *)htp_malloc(result->length);
                OS_MemCopy(desc_node->desc, result->data, result->length);
                desc_node->desc_size = result->length;

                HtpReleasePacket(channel, packet);

                desc_node = (HtpCharDescListNode *)GetNextNode(&desc_node->node);               				
                if ((ListEntry *)desc_node != &char_node->char_desc_list)
                {
                    kal_trace(BT_TRACE_BLE_PROFILES, HTP_READ_CHARDESC_ONEMORE, char_node->value_uuid, desc_node->desc_uuid);

                    status = HtpcReadCharacteristicDescriptor(channel, desc_node->handle);
                    if (status != BT_STATUS_PENDING && status !=  BT_STATUS_SUCCESS)
                    {
                        HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, status, 0, 0);
                    }
                    return;					
                }

NextChar:
                char_node = (HtpCharListNode *)GetNextNode(&char_node->node);
                kal_trace(BT_TRACE_BLE_PROFILES, HTP_READ_CHARDESC_NEXTCHAR);
				
                if ((ListEntry *)char_node == &service->char_list)
                {
                    /* Here, this is the last characteristic descriptor
                    *   Continue to query DIS service characteristic
                    */
                    kal_trace(BT_TRACE_BLE_PROFILES, HTP_READ_CHARDESC_COMPLETE);
                    if (IsListEmpty(&channel->servicedb->dis_service.char_list))
                    {
                        status = HtpcFindPrimaryServicesByUuid(channel, BT_UUID_GATT_SERVICE_DEVICE, 0x0001, 0xffff);
                    }
                    else
                    {
                        HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, BT_STATUS_SUCCESS, 
                                (void *)&servicedb->features, sizeof(servicedb->features));
                        break;
                    }
                }
                else
                {
                    /* Read descriptor value by handle in next characteristic. */
                    if (IsListEmpty((ListEntry *)&char_node->char_desc_list))
                    {
                        goto NextChar;
                    }
                    desc_node = (HtpCharDescListNode *)GetHeadList(&char_node->char_desc_list);					
                    status = HtpcReadCharacteristicDescriptor(channel, desc_node->handle);
                }

                if (status != BT_STATUS_PENDING && status !=  BT_STATUS_SUCCESS)
                {
                    HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, status, 0, 0);
                }
            }
            break;			

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            HtpReleasePacket(channel, packet);
            HtpAppCallback(channel, HTPEVENT_GATT_QUERY_CNF, BT_STATUS_FAILED, 0, 0);			
            break;			
    }
}

/*****************************************************************************
 * FUNCTION
 *  HtpcHandleWriteCharacteristicCb
 *
 * DESCRIPTION
 *  It is for Collector only. Only available for HTS.
 *  When write response is received, or timeout, or link loss.
 *
 *****************************************************************************/
void HtpcHandleWriteCharacteristicCb(BtGattOp *op, BtGattEvent *event)
{
    BtGattWriteCharValueResultEvent *result;
    BtGattWriteCharValueRequest *parm = op->parms.p.writeRequest;
    HtpChannel *channel;
    HtpPacket *packet;

    result = (BtGattWriteCharValueResultEvent *) event->parms;
    channel = HtpFindChannelByLink(event->link);

    packet = ContainingRecord(op, HtpPacket, gatt_op);

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            {
                HtpServiceDB *servicedb;
                HtpCharListNode *char_node; 

                servicedb = channel->servicedb;
                if ((char_node = HtpFindCharbyHandle(&servicedb->hts_service, parm->handle)) == NULL)               
                {
                    break;
                }
                
                /* update value in database */
                OS_MemCopy(char_node->value, parm->data, parm->size);
                HtpAppCallback(channel, HTPEVENT_SET_INTERVAL_CNF, BT_STATUS_SUCCESS, parm->data, parm->size);          
            }
            break;			

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            HtpAppCallback(channel, HTPEVENT_SET_INTERVAL_CNF, BT_STATUS_FAILED, parm->data, parm->size);			
            break;			
    }
    
    HtpReleasePacket(channel, packet);
    HtpIdleConnectionStartTimer(channel, HTP_IDLE_TIMEOUT_COLLECTOR);
}

/*****************************************************************************
 * FUNCTION
 *  HtpcHandleWriteCharacteristicDescriptorCb
 *
 * DESCRIPTION
 *  It is for Collector only. Only available for HTS.
 *  When write response is received, or timeout, or link loss.
 *
 *****************************************************************************/
void HtpcHandleWriteCharacteristicDescriptorCb(BtGattOp *op, BtGattEvent *event)
{
    BtGattWriteCharDescriptorResultEvent *result;
    BtGattWriteDescriptors *parm = op->parms.p.writeDescriptors;
    HtpServiceDB *servicedb;
    HtpCharListNode *char_node; 
    HtpChannel *channel;
    HtpPacket *packet;
    HtpEvent cb_event;

    result = (BtGattWriteCharDescriptorResultEvent *) event->parms;
    channel = HtpFindChannelByLink(event->link);

    packet = ContainingRecord(op, HtpPacket, gatt_op);
    
    servicedb = channel->servicedb;
    char_node = HtpFindCharbyHandleRange(&servicedb->hts_service, parm->handle);           
    
    if (char_node->value_uuid == BT_UUID_HTS_CHAR_TEMPERATURE_MEASUREMENT)
    {
        cb_event = HTPEVENT_SET_TEMPERATURE_CONFIG_CNF;
    }
    else if (char_node->value_uuid == BT_UUID_HTS_CHAR_INTERMEDIATE_TEMPERATURE)
    {
        cb_event = HTPEVENT_SET_INTERMEDIATE_TEMPERATURE_CONFIG_CNF;
    }
    else
    {
        cb_event = HTPEVENT_SET_INTERVAL_CONFIG_CNF;
    }

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            {
                /* update value in database */
                HtpCharDescListNode *desc_node = (HtpCharDescListNode *)GetHeadList(&char_node->char_desc_list);
                if (desc_node->handle != parm->handle)
                {
                    /* It might be Measurement Interval descriptor */
                    desc_node = (HtpCharDescListNode *)GetNextNode(&desc_node->node);
                    if (desc_node->handle != parm->handle)
                    {
                        Report(("[HTP] write characteristic descriptor error handle!"));
                        break;						
                    }
                }
                OS_MemCopy(desc_node->desc, parm->data, parm->size);

                HtpAppCallback(channel, cb_event, BT_STATUS_SUCCESS, parm->data, parm->size);          
            }
            break;			

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            HtpAppCallback(channel, cb_event, BT_STATUS_FAILED, parm->data, parm->size);			
            break;			
    }
    
    HtpReleasePacket(channel, packet);
    HtpIdleConnectionStartTimer(channel, HTP_IDLE_TIMEOUT_COLLECTOR);
}

#endif

#ifdef __BT_HTPT_PROFILE__
/*****************************************************************************
 * FUNCTION
 *  HtptSendIndication
 *
 * DESCRIPTION
 *  It is for Thermometer only.
 *  Send charateristic indication to Collector.
 *
 *****************************************************************************/
BtStatus HtptSendIndication(HtpChannel *channel, U16 handle, U8 *data, U16 len)
{
    HtpPacket *packet;
    BtGattOp *op;
    BtGattValueIndications *parm;
    BtStatus status;

    packet = HtpGetPacket(channel);
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    op = &packet->gatt_op;
    
    InitializeListEntry(&op->op.node);	
    op->apCb = HtpGattOpCallback;
	
    parm = &packet->parm.valueIndications;

    parm->handle = handle;
    parm->data = data;
    parm->size = len;	
    op->parms.p.valueIndications = parm;

    status = BT_GATT_Indications(channel->link, op);
    if (status != BT_STATUS_PENDING)
    {
        HtpReleasePacket(channel, packet);
    }
    else
    {
        HtpIdleConnectionStopTimer();
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HtptSendNotification
 *
 * DESCRIPTION
 *  It is for Thermometer only.
 *  Send charateristic notification to Collector.
 *
 *****************************************************************************/
BtStatus HtptSendNotification(HtpChannel *channel, U16 handle, U8 *data, U16 len)
{
    HtpPacket *packet;
    BtGattOp *op;
    BtGattValueNotifications *parm;
    BtStatus status;
    
    packet = HtpGetPacket(channel);
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    op = &packet->gatt_op;
    
    InitializeListEntry(&op->op.node);	
    op->apCb = HtpGattOpCallback;
	
    parm = &packet->parm.valueNotifications;

    parm->handle = handle;
    parm->data = data;
    parm->size = len;	
    op->parms.p.valueNotifications = parm;
    
    status = BT_GATT_Notifications(channel->link, op);
    if (status != BT_STATUS_PENDING)
    {
        HtpReleasePacket(channel, packet);
    }
    else
    {
        HtpIdleConnectionStopTimer();
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  HtptHandleIndicationCb
 *
 * DESCRIPTION
 *  It is for Thermometer only.
 *
 *****************************************************************************/
void HtptHandleIndicationCb(BtGattOp *op, BtGattEvent *event)
{
    BtGattValueIndicationsResultEvent *result;
    BtGattValueIndications *parm = op->parms.p.valueIndications;
    HtpChannel *channel;
    HtpPacket *packet;
    HtpEvent cb_event;

    result = (BtGattValueIndicationsResultEvent *) event->parms;
    channel = HtpFindChannelByLink(event->link);

    packet = ContainingRecord(op, HtpPacket, gatt_op);

    if (parm->handle == ATT_HANDLE_HTS_CHARACTERISTIC_TP_MEASURE_VALUE) 
    {
        cb_event = HTPEVENT_MEASUREMENT_IND_CNF;
    }
    else
    {
        cb_event = HTPEVENT_INTERVAL_VALUE_IND_CNF;
    }

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            HtpAppCallback(channel, cb_event, BT_STATUS_SUCCESS, parm->data, parm->size);          
            break;			

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            HtpAppCallback(channel, cb_event, BT_STATUS_FAILED, parm->data, parm->size);			
            break;			
    }
    HtpReleasePacket(channel, packet);
    HtpIdleConnectionStartTimer(channel, HTP_IDLE_TIMEOUT_THERMOMETER);
}

/*****************************************************************************
 * FUNCTION
 *  HtptHandleNotificationCb
 *
 * DESCRIPTION
 *  It is for Thermometer only.
 *
 *****************************************************************************/
void HtptHandleNotificationCb(BtGattOp *op, BtGattEvent *event)
{
    BtGattValueNotificationResultEvent *result;
    BtGattValueNotifications *parm = op->parms.p.valueNotifications;
    HtpChannel *channel;
    HtpPacket *packet;

    result = (BtGattValueNotificationResultEvent *) event->parms;
    channel = HtpFindChannelByLink(event->link);

    Report(("[HTP] handle notificaiton callback, handle: 0x%x ", parm->handle));

    packet = ContainingRecord(op, HtpPacket, gatt_op);

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            HtpAppCallback(channel, HTPEVENT_MEASUREMENT_NOTIFY_CNF, BT_STATUS_SUCCESS, parm->data, parm->size);          
            break;			

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            HtpAppCallback(channel, HTPEVENT_MEASUREMENT_NOTIFY_CNF, BT_STATUS_FAILED, parm->data, parm->size);			
            break;			
    }
    HtpReleasePacket(channel, packet);
    HtpIdleConnectionStartTimer(channel, HTP_IDLE_TIMEOUT_THERMOMETER);
}

/*****************************************************************************
 * FUNCTION
 *  HtptTemperatureClientConfigCb
 *
 * DESCRIPTION
 *  It is for Thermometer only.
 *  It is the callback function for Temperature Measurement Client 
 *  Characteristic Configuration to be read and written.
 *
 *****************************************************************************/
U8 HtptTemperatureClientConfigCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw)
{
    BtStatus status = BT_STATUS_SUCCESS;


    switch (type)
    {
        case ATT_OP_WRITE:
        case ATT_OP_EXECUTE_WRITE:
            {
                /* It shall be notified to upper layer, since the attribute is client device dependent */
                HtpClientCharConfig value = (0x00ff & **raw) | (0xff00 & (*(*raw+1) << 8));
                HtpChannel * channel = HtpFindChannelByLink(link);
                HtpAppCallback(channel, HTPEVENT_SET_TEMPERATURE_CONFIG_REQ, BT_STATUS_SUCCESS, 
                                (void *)&value, sizeof(HtpClientCharConfig));
                kal_trace(BT_TRACE_BLE_PROFILES, HTP_WRITE_TEMPERATURE_CLIENTCONFIG, value);
                HtpIdleConnectionStartTimer(channel, HTP_IDLE_TIMEOUT_THERMOMETER);
            }
            break;
        default:			
            break;
    }

    return (U8)status;
}

/*****************************************************************************
 * FUNCTION
 *  HtptTemperatureTypeCb
 *
 * DESCRIPTION
 *  It is for Thermometer only.
 *  It is the callback function for Temperature Type Characteristic to be read.
 *
 *****************************************************************************/
/*U8 HtptTemperatureTypeCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw)
{
    BtStatus status = BT_STATUS_SUCCESS;

    switch (type)
    {
        case ATT_OP_READ:
            // get value from context, it is set to profile in registration parameter
            *raw = (U8 *)&(HTP(server_parms).temperature_type);
            *len = sizeof(HtpTemperatureType);
            break;
        default:			
            break;
    }

    return (U8)status;

}
*/
/*****************************************************************************
 * FUNCTION
 *  HtptImtermediateTemperatureClientConfigCb
 *
 * DESCRIPTION
 *  It is for Thermometer only.
 *  It is the callback function for Intermediate Temperature Client
 *  Characteristic Configuration to be read and written.
 *
 *****************************************************************************/
U8 HtptImtermediateTemperatureClientConfigCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw)
{
    BtStatus status = BT_STATUS_SUCCESS;

    switch (type)
    {
        case ATT_OP_WRITE:
        case ATT_OP_EXECUTE_WRITE:
            {
                /* It shall be notified to upper layer, since the attribute is client device dependent */
                HtpClientCharConfig value = (0x00ff & **raw) | (0xff00 & (*(*raw+1) << 8));
                HtpChannel * channel = HtpFindChannelByLink(link);
                HtpAppCallback(channel, HTPEVENT_SET_INTERMEDIATE_TEMPERATURE_CONFIG_REQ, BT_STATUS_SUCCESS, 
                                (void *)&value, sizeof(HtpClientCharConfig));
                kal_trace(BT_TRACE_BLE_PROFILES, HTP_WRITE_INTERTEMPERATURE_CLIENTCONFIG, value);
                HtpIdleConnectionStartTimer(channel, HTP_IDLE_TIMEOUT_THERMOMETER);
            }
            break;
        default:			
            break;
    }
    return (U8)status;
}

/*****************************************************************************
 * FUNCTION
 *  HtptIntervalValueCb
 *
 * DESCRIPTION
 *  It is for Thermometer only.
 *  It is the callback function for Measurement Interval to be read and written.
 *
 *****************************************************************************/
U8 HtptIntervalValueCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw)
{
    BtStatus status = BT_STATUS_SUCCESS;

    switch (type)
    {
        case ATT_OP_WRITE:
        case ATT_OP_EXECUTE_WRITE:
            {
                HtpChannel * channel = HtpFindChannelByLink(link);
                HtpMeasurementInterval value;
                value = (0x00ff & **raw) | (0xff00 & (*(*raw+1) << 8));
                HtpAppCallback(channel, HTPEVENT_SET_INTERVAL_REQ, BT_STATUS_SUCCESS, 
                                (void *)&value, sizeof(HtpMeasurementInterval));
                HtpIdleConnectionStartTimer(channel, HTP_IDLE_TIMEOUT_THERMOMETER);
            }
            break;
        default:			
            break;
    }
    return (U8)status;
}

/*****************************************************************************
 * FUNCTION
 *  HtptIntervalClientConfigCb
 *
 * DESCRIPTION
 *  It is for Thermometer only.
 *  It is the callback function for Measurement Interval Client
 *  Characteristic Configuration to be read and written.
 *
 *****************************************************************************/
U8 HtptIntervalClientConfigCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw)
{
    BtStatus status = BT_STATUS_SUCCESS;

    switch (type)
    {
        case ATT_OP_WRITE:
        case ATT_OP_EXECUTE_WRITE:
            {
                /* It shall be notified to upper layer, since the attribute is client device dependent */
                HtpChannel * channel = HtpFindChannelByLink(link);
                HtpClientCharConfig value = (0x00ff & **raw) | (0xff00 & (*(*raw+1) << 8));
                HtpAppCallback(channel, HTPEVENT_SET_INTERVAL_CONFIG_REQ, BT_STATUS_SUCCESS, 
                                (void *)&value, sizeof(HtpClientCharConfig));
                kal_trace(BT_TRACE_BLE_PROFILES, HTP_WRITE_INTERVAL_CLIENTCONFIG, value);
                HtpIdleConnectionStartTimer(channel, HTP_IDLE_TIMEOUT_THERMOMETER);
            }
            break;
        default:			
            break;
    }
    return (U8)status;
}

/*****************************************************************************
 * FUNCTION
 *  HtptIntervalRangeCb
 *
 * DESCRIPTION
 *  It is for Thermometer only.
 *  It is the callback function for Measurement Interval Valid Range to be read.
 *
 *****************************************************************************/
/*U8 HtptIntervalRangeCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw)
{
    BtStatus status = BT_STATUS_SUCCESS;

    switch (type)
    {
        case ATT_OP_READ:
            // get value from context, it is set to profile in registration parameter
            {
                *raw = HTP(server_parms).interval_range;
                *len = sizeof(HtpMeasurementInterval) * 2;
                Report(("[HTP] interval range callback: %d, %d, %d, %d", HTP(server_parms).interval_range[0], 
                    HTP(server_parms).interval_range[1], HTP(server_parms).interval_range[2], HTP(server_parms).interval_range[3]));
            }
            break;
        default:			
            break;
    }
    return (U8)status;
}
*/
#endif

#if defined(__BT_HTPC_PROFILE__) || defined(__BT_HTPT_PROFILE__)

/*****************************************************************************
 * FUNCTION
 *  HtpGattOpCallback
 *
 * DESCRIPTION
 *  It is used to handle GATT operation response, including
 *  BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID, 
 *  BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_OF_SERVICE, 
 *  BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_DESCRIPTOR, 
 *  BT_GATT_OPERATOR_READ_CHAR_VALUE, 
 *  BT_GATT_OPERATOR_READ_CHAR_DESCRIPTORS,
 *  BT_GATT_OPERATOR_WRITE_CHAR_VALUE,
 *  BT_GATT_OPERATOR_WRITE_CHAR_DESCRIPTORS,
 *  BT_GATT_OPERATOR_INDICATIONS, 
 *  and BT_GATT_OPERATOR_NOTIFICATIONS.
 *
 *****************************************************************************/
void HtpGattOpCallback(void *userdata, BtGattEvent *event)
{
    kal_trace(BT_TRACE_BLE_PROFILES, HTP_GATTOPCALLBACK_EVENT, event->eType, ((U8 *)event->parms)[0]);

    switch (event->eType)
    {
        /* It is for Collector Operation callback */
        case BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID:
            HtpcHandleFindPrimaryServicesByUuidCb((BtGattOp *)userdata, event);
            break;
        case BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_OF_SERVICE:
            HtpcHandleFindAllCharacteristicsCb((BtGattOp *)userdata, event);
            break;
        case BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_DESCRIPTOR:
            HtpcHandleFindAllCharacteristicDescriptorsCb((BtGattOp *)userdata, event);
            break;
        case BT_GATT_OPERATOR_READ_CHAR_VALUE:
            HtpcHandleReadCharacteristicCb((BtGattOp *)userdata, event);
            break;
        case BT_GATT_OPERATOR_READ_CHAR_DESCRIPTORS:
            HtpcHandleReadCharacteristicDescriptorCb((BtGattOp *)userdata, event);
            break;			
        case BT_GATT_OPERATOR_WRITE_CHAR_VALUE:
            HtpcHandleWriteCharacteristicCb((BtGattOp *)userdata, event);
            break;
        case BT_GATT_OPERATOR_WRITE_CHAR_DESCRIPTORS:
            HtpcHandleWriteCharacteristicDescriptorCb((BtGattOp *)userdata, event);
            break;          
        /* It is for Thermometer Operation callback */
        case BT_GATT_OPERATOR_INDICATIONS: 		
            HtptHandleIndicationCb((BtGattOp *)userdata, event);
            break;			
        case BT_GATT_OPERATOR_NOTIFICATIONS: 		
            HtptHandleNotificationCb((BtGattOp *)userdata, event);
            break;			
        default:
            break;
    }
}
#endif
