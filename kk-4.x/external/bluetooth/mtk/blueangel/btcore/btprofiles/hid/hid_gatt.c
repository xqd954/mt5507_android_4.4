/*****************************************************************************
 *
 * Filename:
 * ---------
 * hid_gatt.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth HID Profile over GATT related handling.
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
#ifdef __BT_4_0_BLE__
#include "utils.h"
#include "attdef.h"
#include "att.h"
#include "gatt.h"
#include "gattcb.h"
#include "hidi.h"
#include "ble_utils.h"
#include "hid_blescan.h"


extern void Hid_Init_Parser(HidParseData*	tmpHidParser);
extern BtStatus HidAddDescriptor(HidChannel *channel);
extern void Hid_Free_Parser(HidParseData *hpd);
extern BtStatus GattDisconnect(BtRemoteDevice *link);
/*
extern BtStatus LeUtilParmsUpdate(BtRemoteDevice *remDev, 
                                                U8 lockLeConnectionUpdate,
                                                U16 interval_min, 
                                                U16 interval_max, 
                                                U16 slave_latency, 
                                                U16 timeout);
*/

//BtGattOp op;

#define BT_GATT_CLIENT_CONFIG_NOTIFICATION      0x0001
int g_client_config_descriptor_count;

#ifdef __BT_HOGP_PTS_TEST__ 
U8 g_input_data[]   = {0xAA, 0xB3, 0xF8, 0xA6, 0xCD};
U8 g_output_data[]  = {0xEF, 0x90, 0x78, 0x56, 0x34, 0x12, 0x00};
U8 g_feature_data[] = {0xEA, 0x45, 0x3F, 0x2D, 0x87};
#endif

void HidHandleNotification(HidChannel *channel, BtGattValueNotificationEvent *parm)
{
    HidInterrupt intr;
    BtStatus     status;
    HidGattReport *report;
    BT_BOOL found = FALSE;

    report = (HidGattReport *)GetHeadList(&channel->reportGattTable);
    while((ListEntry *)report != &channel->reportGattTable)
    {
        if (report->handle == parm->attributeHandle)
        {
            found = TRUE;
            break;
        }
        report = (HidGattReport *)GetNextNode(&report->node);
    }

    if (!found)
    {   
        kal_trace(BT_TRACE_BLE_PROFILES, HID_NOTIFYCALLBACK_NOMATCH);           
        return;
    }
    
    /* Set up the event for the application */
    intr.data = (U8 *)BLE_UTILS_MALLOC(parm->attributeValueLen + 1);

    if (0 == report->reportId)
    {
        OS_MemCopy(intr.data, parm->attributeValue, parm->attributeValueLen);
        intr.dataLen = parm->attributeValueLen;    
    }
    else
    {
        intr.data[0] = report->reportId;
        OS_MemCopy(intr.data + 1, parm->attributeValue, parm->attributeValueLen);
        intr.dataLen = parm->attributeValueLen + 1;        
    }

    intr.reportType = report->reportType;
    intr.serviceInstance = report->serviceInstance;
    if (parm->attributeValueLen == BT_GATT_GetNegotiationMTU(channel->cmgrHandler.remDev)) {
        //intr.offset = parm->attributeValueLen;
        //status = HidReadLongReport(channel, parm->attributeHandle, intr.offset);
    } else {
        status = BT_STATUS_SUCCESS;
    }


#ifdef HID_DEBUG_LEVEL_2
    {
        U8 i;        
        Report(("[HID] interrupt data, len: %d, report id: %d, report type: %d, handle: %d, service instance: %d", parm->attributeValueLen, report->reportId, report->reportType,
                    parm->attributeHandle, report->serviceInstance));
        for (i = 0; i < parm->attributeValueLen; i++)
        {
            Report(("[HID] interrupt data, report: %x", parm->attributeValue[i]));
        }
    }
#endif

    /* Make the callback */
    HidAppCallback(channel, HIDEVENT_INTERRUPT, 
                   status, &intr, 
                   sizeof(HidInterrupt));


    BLE_UTILS_FREE(intr.data);
}


/*****************************************************************************
 * FUNCTION
 *  HidGattEventCallback
 *
 * DESCRIPTION
 *  It is used to handle GATT event, including BT_GATT_APP_EVENT_CONNECTED, BT_GATT_APP_EVENT_DISCONNECTED.
 *
 *****************************************************************************/
void HidGattEventCallback(void *handler, BtGattEvent *event)
{
    HidChannel *channel;
    BtStatus status;

    channel = HidFindChannelByLink(event->link);

    switch (event->eType)
    {
        case BT_GATT_APP_EVENT_CONNECTED:
            kal_trace(BT_TRACE_BLE_PROFILES, HID_GATTEVENTCALLBACK_EVENT, event->eType);           
            if (channel == NULL)
            {
                Report(("[HID] unexpected GATT connection!"));
                return;
/*
                channel = HidNewChannel();
                if (channel == NULL)		
                {
                    GattDisconnect(event->link);
                    return;					
                }
*/                
            }
            channel->state = HID_STATE_OPEN;
            HidAppCallback(channel, HIDEVENT_OPEN, BT_STATUS_SUCCESS, event->link, sizeof(BtRemoteDevice));	
            Report(("Channel->flasg:%02x",channel->flags));
            if (channel->flags & CHNL_FLAG_SERVICE_QUERY)
            {
                status = HidStartGattServiceQuery(channel);
                Report(("HidStartGattServiceQuery status: %d", status));
                if (status != BT_STATUS_PENDING)
                {
                    channel->flags &= ~CHNL_FLAG_SERVICE_QUERY;
                    HidAppCallback(channel, HIDEVENT_QUERY_CNF, status,
                                   &channel->queryRsp, (U16)(sizeof(HidQueryRsp)));
                }
            }
            break;
            
        case BT_GATT_APP_EVENT_DISCONNECTED:    
            kal_trace(BT_TRACE_BLE_PROFILES, HID_GATTEVENTCALLBACK_EVENT, event->eType);           
            if (channel == NULL)
               return;
            HidClearConnection(channel, BT_STATUS_SUCCESS, 0);			
            break;

        case BT_GATT_APP_EVENT_VALUE_NOTIFICATION:
            if (channel == NULL)
               return;
            /* Report (Input) or Boot Keyboard Input Report */
            HidHandleNotification(channel, event->parms);
            break;

        default:
            kal_trace(BT_TRACE_BLE_PROFILES, HID_GATTEVENTCALLBACK_UNEXPECTED_EVENT);           
            break;
                
    }

}

void HidBondingCallback(U8 type, U8 *bd_addr)
{
    switch (type)
    {
        case ATT_SEC_DEVICE_BOND:
            HidAddWhiteList(bd_addr);
            break;
        case ATT_SEC_DEVICE_UNBOND:
            /* Device Un Bonded */
            HidScanAdvUpdate(bd_addr, FALSE);
            HidRemoveWhiteList(bd_addr);
            break;
    }
}

/* Read Characteristic shall only be mapped to  Get Report. And Other Read operation result
*   can be obtained from database directly.
*/
void HidReadCharacteristicCallback(void *userdata, BtGattEvent *event)
{
    BtGattOp *op = (BtGattOp *)userdata;
    BtGattReadCharacteristicValueResultEvent *result;
    HidChannel *channel;
    HidGattPacket *packet;
    HidTransaction *transReq;
    HidTransaction transRsp;
    HidReport report;

    result = (BtGattReadCharacteristicValueResultEvent *) event->parms;
    channel = HidFindChannelByLink(event->link);
    if (channel == NULL)
    {
        return;
    }

    packet = ContainingRecord(op, HidGattPacket, gatt_op);
    BLE_UTILS_FREE(packet);

    transReq = (HidTransaction *)GetHeadList(&channel->transQueue);

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            report.dataLen = result->length;
            report.reportType = transReq->parm.reportReq->reportType;
            report.data = (U8 *)BLE_UTILS_MALLOC(result->length + 1);
            report.data[0] = transReq->parm.reportReq->reportId;
            OS_MemCopy(report.data + 1, result->data, result->length);

            transRsp.parm.report = &report;
            transRsp.type = HID_TRANS_GET_REPORT_RSP;

            HidAppCallback(channel, HIDEVENT_TRANSACTION_RSP, 
                           BT_STATUS_SUCCESS, &transRsp, 
                           sizeof(HidTransaction));
            
            
            transReq->resultCode = HID_RESULT_SUCCESS;
            HidCompleteTransaction(channel, transReq, 
                                   BT_STATUS_SUCCESS);
            break;			

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            HidCompleteTransaction(channel, transReq, 
                                   BT_STATUS_FAILED);
            break;			
    }

}

void HidWriteCharacteristicCallback(void *userdata, BtGattEvent *event)
{
    BtGattOp *op = (BtGattOp *)userdata;
    BtGattWriteCharValueResultEvent *result;
    HidChannel *channel;
    HidGattPacket *packet;
    HidTransaction *transReq;
    HidTransaction transRsp;
    BtStatus status = BT_STATUS_FAILED;

    result = (BtGattWriteCharValueResultEvent *) event->parms;
    channel = HidFindChannelByLink(event->link);
    if (channel == NULL)
    {
        return;
    }

    packet = ContainingRecord(op, HidGattPacket, gatt_op);
    BLE_UTILS_FREE(packet);

#ifdef __BT_HOGP_PTS_TEST__ 
    if (channel->uncompl_indicate_cnt)
    {
        channel->uncompl_indicate_cnt--;
    }
    if (channel->uncompl_indicate_cnt)
    {
        return;
    }
#endif    

    transReq = (HidTransaction *)GetHeadList(&channel->transQueue);
    
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

    HidCompleteTransaction(channel, transReq, status);
}

void HidWriteWithoutResponseCallback(void *userdata, BtGattEvent *event)
{
    BtGattOp *op = (BtGattOp *)userdata;
    BtGattWriteWithoutResponseResultEvent *result;
    HidChannel *channel;
    HidGattPacket *packet;
    HidTransaction *transReq;
    HidTransaction transRsp;
    BtStatus status = BT_STATUS_FAILED;

    result = (BtGattWriteWithoutResponseResultEvent *) event->parms;
    channel = HidFindChannelByLink(event->link);
    if (channel == NULL)
    {
        return;
    }

    packet = ContainingRecord(op, HidGattPacket, gatt_op);
    BLE_UTILS_FREE(packet);


    transReq = (HidTransaction *)GetHeadList(&channel->transQueue);
    Report(("[HID] HidWriteWithoutResponseCallback, request: %d", transReq->type));

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

    HidCompleteTransaction(channel, transReq, status);

}

/* Write Characteristic Without Response for Output Report, which shall  
*   be mapped to Interrupt channel.
*/
void HidWriteOutputReportCallback(void *userdata, BtGattEvent *event)
{
    BtGattOp *op = (BtGattOp *)userdata;
    BtGattWriteWithoutResponseResultEvent *result;
    HidChannel *channel;
    HidGattPacket *packet;
    HidInterrupt *intrReq;
    BtStatus status = BT_STATUS_FAILED;

    result = (BtGattWriteWithoutResponseResultEvent *) event->parms;
    channel = HidFindChannelByLink(event->link);
    if (channel == NULL)
    {
        return;
    }

    packet = ContainingRecord(op, HidGattPacket, gatt_op);
    BLE_UTILS_FREE(packet);

#ifdef __BT_HOGP_PTS_TEST__ 
    if (channel->uncompl_notify_cnt)
    {
        channel->uncompl_notify_cnt--;
    }
    if (channel->uncompl_notify_cnt)
    {
        return;
    }
#endif    

    intrReq = (HidInterrupt *)GetHeadList(&channel->intrQueue);

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

    HidCompleteInterrupt(channel, intrReq, status);
}


void HidSetReportNotificationCallback(void *userdata, BtGattEvent *event)
{
    BtGattOp *op = (BtGattOp *)userdata;
    BtGattWriteCharDescriptorResultEvent *result;
    BtGattWriteDescriptors *parm = op->parms.p.writeDescriptors;
    HidChannel *channel;
    HidGattPacket *packet;

    result = (BtGattWriteCharDescriptorResultEvent *) event->parms;
    channel = HidFindChannelByLink(event->link);
    if (channel == NULL)
    {
        return;
    }

    packet = ContainingRecord(op, HidGattPacket, gatt_op);

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
            {
                Report(("[HID] GATT Write done, count: %d", g_client_config_descriptor_count));
                g_client_config_descriptor_count--;
                if (g_client_config_descriptor_count == 0)
                {
                    BLE_UpdateServiceDatabase(&channel->cmgrHandler.remDev->bdAddr);
                }
            }
            break;          

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            // TODO:
            break;          
    }
    
    BLE_UTILS_FREE(packet);
}

BtStatus HidReadCharacteristic(HidChannel *channel, U16 handle)
{
    HidGattPacket *packet;
    BtGattOp *op;
    BtGattReadCharacteristicValue *parm;
    BtStatus status;
    
    packet = (HidGattPacket *)BLE_UTILS_MALLOC(sizeof(HidGattPacket));
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    op = &packet->gatt_op;
    OS_MemSet((U8 *)op, 0, sizeof(BtGattOp));
    
    InitializeListEntry(&op->op.node);	
    op->apCb = HidReadCharacteristicCallback;
    
    parm = &packet->parm.readCharValue;
    parm->handle = handle;
    op->parms.p.readCharValue = parm;
    
    return BT_GATT_ReadCharacteristicValue(channel->cmgrHandler.remDev, op);

}

BtStatus HidWriteCharacteristic(HidChannel *channel, U16 handle, U8 *data, U16 len)
{
    HidGattPacket *packet;
    BtGattOp *op;
    BtGattWriteCharValueRequest *parm;
    BtStatus status;
    
    packet = (HidGattPacket *)BLE_UTILS_MALLOC(sizeof(HidGattPacket));
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    op = &packet->gatt_op;
    OS_MemSet((U8 *)op, 0, sizeof(BtGattOp));
    
    InitializeListEntry(&op->op.node);	
    op->apCb = HidWriteCharacteristicCallback;
    
    parm = &packet->parm.writeRequest;
    parm->handle = handle;
    parm->size = len;
    parm->data = data;
    op->parms.p.writeRequest = parm;
    
    return BT_GATT_WriteCharacteristicValue(channel->cmgrHandler.remDev, op);
}

BtStatus HidWriteWithoutResponse(HidChannel *channel, GattAPCallBack cb, U16 handle, U8 *data, U16 len)
{
    HidGattPacket *packet;
    BtGattOp *op;
    BtGattWriteWithoutResponse *parm;
    BtStatus status;
    
    packet = (HidGattPacket *)BLE_UTILS_MALLOC(sizeof(HidGattPacket));
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    op = &packet->gatt_op;
    OS_MemSet((U8 *)op, 0, sizeof(BtGattOp));
    
    InitializeListEntry(&op->op.node);	
    op->apCb = cb;
    
    parm = &packet->parm.writeWithoutResponse;
    parm->handle = handle;
    parm->size = len;
    parm->data = data;
    op->parms.p.writeWithoutResponse = parm;
    
    return BT_GATT_WriteWithoutResponse(channel->cmgrHandler.remDev, op);
}

BtStatus HidSetReportNotification(HidChannel *channel, U16 handle, U8 *data)
{
    HidGattPacket *packet;
    BtGattOp *op;
    BtGattWriteDescriptors *parm;
    BtStatus status;
    
    packet = (HidGattPacket *)BLE_UTILS_MALLOC(sizeof(HidGattPacket));
    if (packet == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    kal_trace(BT_TRACE_BLE_PROFILES, HID_WRITE_CHARDESC_HANDLE, handle);
    OS_MemSet((U8 *)packet, 0, sizeof(HidGattPacket));

    op = &packet->gatt_op;
    OS_MemSet((U8 *)op, 0, sizeof(BtGattOp));
    
    InitializeListEntry(&op->op.node);  
    op->apCb = HidSetReportNotificationCallback;
    
    parm = &packet->parm.writeDescriptors;
    parm->handle = handle;
    parm->size = 2;
    parm->data = data;
    op->parms.p.writeDescriptors = parm;
    
    status = BT_GATT_WriteCharacteristicDescriptors(channel->cmgrHandler.remDev, op);
    if (status != BT_STATUS_PENDING)
    {
        BLE_UTILS_FREE(packet);
    }
    return status;
}

void HidRestoreHidReportMappingTable(HidChannel *channel, BleChar *report_char, U8 service_instance)
{
    BleCharDesc *report_desc;
    U8 desc_uuid[2];
    HidGattReport *report;

    
    /* HID Report Reference Descriptor --> Report ID, Report Type, Report Handle mapping */
    StoreLE16(desc_uuid, BT_UUID_HID_CHAR_DESC_REPORT_REF);
    report_desc = BLE_FindCharDescByUuid(report_char, 2, desc_uuid);
    if (report_desc != NULL)
    {   
        report = (HidGattReport *)BLE_UTILS_MALLOC(sizeof(HidGattReport));
        OS_MemSet((U8 *)report, 0, sizeof(HidGattReport));
        report->handle = report_char->value_handle;
        report->reportId = report_desc->desc[0];
        report->reportType = report_desc->desc[1];
        report->serviceInstance = service_instance;
        InsertTailList(&channel->reportGattTable, &report->node);
        kal_trace(BT_TRACE_BLE_PROFILES, HID_GATTREPORT_INFO, report->handle, report->reportId, report->reportType);
    }
}

BtStatus HidRestoreHidService(HidChannel *channel)
{
    BleService *hidservice;
    BleChar *blechar;
    U8 service_uuid[2];
    U8 char_uuid[2];
    HidQueryRsp *rsp = &channel->queryRsp;
    U8 service_instance = 0;
    HidParseData *hpd;
    BtStatus status = BT_STATUS_SUCCESS;

    OS_MemSet((U8 *)rsp, 0, sizeof(HidQueryRsp));

    /* HID Service */
    StoreLE16(service_uuid, BT_UUID_GATT_SERVICE_HID);
    hidservice = BLE_FindServiceByUuid(&channel->cmgrHandler.remDev->bdAddr, 2, service_uuid);
    while (hidservice != NULL)
    {
        /* HID Report Map --> HID Descriptor */
        StoreLE16(char_uuid, BT_UUID_HID_CHAR_REPORT_MAP);
        blechar = BLE_FindCharByUuid(hidservice, 2, char_uuid);
        if (blechar != NULL)

        {
            rsp->descriptorLen = blechar->value_size;
            rsp->descriptorList = (U8 *)BLE_UTILS_MALLOC(blechar->value_size);
            OS_MemCopy(rsp->descriptorList, blechar->value, blechar->value_size);
        }

#ifdef HID_DEBUG_LEVEL_1
        {
            U8 *p = rsp->descriptorList;
            U8 i, j;
            Report(("[HID] descriptor len, %d", rsp->descriptorLen));

            for (i = 0; i < rsp->descriptorLen/5; i++)
            {
                j = i*5;
                Report(("[HID] descriptor from gatt, %x, %x, %x, %x, %x", p[j], p[j+1], p[j+2], p[j+3], p[j+4]));
            }
        }
#endif
        /* HID Information */
        StoreLE16(char_uuid, BT_UUID_HID_CHAR_INFORMATION);
        blechar = BLE_FindCharByUuid(hidservice, 2, char_uuid);
        if (blechar != NULL)

        {
            rsp->parserVersion = LEtoHost16(blechar->value);
            rsp->countryCode = blechar->value[2];
            if (blechar->value[3] & 0x1)
            {
                rsp->remoteWakeup = TRUE;
            }
            if (blechar->value[3] & 0x2)
            {
                rsp->normConnectable = TRUE;
            }
            Report(("[HID] remoteWakeup: %d, normConn: %d", rsp->remoteWakeup, rsp->normConnectable));
        }

        /* HID Report
        *   There might be more than one Report in server
        */
        StoreLE16(char_uuid, BT_UUID_HID_CHAR_REPORT);
        blechar = BLE_FindCharByUuid(hidservice, 2, char_uuid);
        if (blechar != NULL)
        {
            HidRestoreHidReportMappingTable(channel, blechar, service_instance);
            while ((blechar = BLE_FindNextCharByUuid(hidservice, blechar, 2, char_uuid)) != NULL)
            {
                HidRestoreHidReportMappingTable(channel, blechar, service_instance);
            }            
        }

        /* Parse HID Descriptor */
        hpd = (HidParseData*)BLE_UTILS_MALLOC(sizeof(HidParseData));
        Hid_Init_Parser(hpd);
        channel->reportCons[service_instance] = (HidReportConstructor*)hid_malloc(HID_REPORT_ID_MAX * sizeof(HidReportConstructor));
        OS_MemSet((U8*) channel->reportCons[service_instance], 0, HID_REPORT_ID_MAX * sizeof(HidReportConstructor));
        bt_prompt_trace(MOD_BT,"[HID] parse descriptor, service_instance = %d", service_instance);

        if(HidAddDescriptor(channel) != BT_STATUS_SUCCESS)
        {
            bt_prompt_trace(MOD_BT,"[HID]HidAddDescriptor Failed!");
            status = BT_STATUS_FAILED;
            Hid_Free_Parser(hpd);
            break;
        }
        Hid_Free_Parser(hpd);

        hidservice = BLE_FindNextServiceByUuid(&channel->cmgrHandler.remDev->bdAddr, hidservice, 2, service_uuid);
        service_instance ++;
    }

    return status;
}

void HidRestoreBatteryService(HidChannel *channel)
{
    BleService *bleservice;
    BleChar *blechar;
    U8 service_uuid[2];
    U8 char_uuid[2];

    /* Battery Power */
    StoreLE16(service_uuid, BT_UUID_GATT_SERVICE_BATTERY);
    bleservice = BLE_FindServiceByUuid(&channel->cmgrHandler.remDev->bdAddr, 2, service_uuid);

    if (bleservice != NULL)
    {
        StoreLE16(char_uuid, BT_UUID_BATTERY_CHAR_POWER_STATE);
        blechar = BLE_FindCharByUuid(bleservice, 2, char_uuid);
        if (blechar != NULL)        
        {
            Report(("[HID] Battery power state: %d, len: %d", blechar->value[0], blechar->value_size));    
        }
    }
}

void HidConfigHidReportDescriptor(HidChannel *channel, BleChar *report_char)
{
    BleCharDesc *report_desc;
    U8 desc_uuid[2];
    HidGattReport *report;

    /* Client Configuration Descriptor */
    if (report_char->properties & GATT_CHAR_PROP_NOTIFY)
    {
        StoreLE16(desc_uuid, BT_UUID_GATT_CHAR_DESC_CLIENT_CHAR_CONFIG);
        report_desc = BLE_FindCharDescByUuid(report_char, 2, desc_uuid);
        if ((report_desc != NULL) && !(LEtoHost16(report_desc->desc) & BT_GATT_CLIENT_CONFIG_NOTIFICATION))
        {
            report_desc->desc[0] |= (U8)BT_GATT_CLIENT_CONFIG_NOTIFICATION;
            g_client_config_descriptor_count++;
            HidSetReportNotification(channel, report_desc->handle, report_desc->desc);
        }
    }
}

/*---------------------------------------------------------------------------
 *            HidConfigHidService()
 *---------------------------------------------------------------------------
 */
void HidConfigHidService(HidChannel *channel)
{
    BleService *hidservice;
    BleChar *blechar;
    BleCharDesc *blechardesc;
    U8 service_uuid[2];
    U8 char_uuid[2];
    U8 desc_uuid[2];
    HidGattReport *report;

    StoreLE16(service_uuid, BT_UUID_GATT_SERVICE_HID);
    hidservice = BLE_FindServiceByUuid(&channel->cmgrHandler.remDev->bdAddr, 2, service_uuid);
    while (hidservice != NULL)
    {
        /* Read all Report Characteristics. There might be more than one Report in server */
        StoreLE16(char_uuid, BT_UUID_HID_CHAR_REPORT);
        blechar = BLE_FindCharByUuid(hidservice, 2, char_uuid);
        if (blechar != NULL)
        {
            HidConfigHidReportDescriptor(channel, blechar);
        }
        
        while ((blechar = BLE_FindNextCharByUuid(hidservice, blechar, 2, char_uuid)) != NULL)
        {
            HidConfigHidReportDescriptor(channel, blechar);
        }

        hidservice = BLE_FindNextServiceByUuid(&channel->cmgrHandler.remDev->bdAddr, hidservice, 2, service_uuid);
    }
}

void HidConfigScanParamService(HidChannel *channel)
{

}

void HidGattServiceQueryCb(void *context, BtStatus status)
{
    HidChannel *channel = (HidChannel *)context;
    HidGattUuid *uuid_node;
    U16 uuid;
    BtStatus result = status;

    uuid_node = (HidGattUuid *)GetHeadList(&channel->uuidQueue);
    uuid = LEtoHost16(uuid_node->uuid);

    if ((status != BT_STATUS_SUCCESS) &&
        (uuid == BT_UUID_GATT_SERVICE_HID))
    {
        /* Remove remaining uuid node in uuidQueue */
        while (!IsListEmpty(&channel->uuidQueue))
        {
            uuid_node = (HidGattUuid *)RemoveHeadList(&channel->uuidQueue);
            BLE_UTILS_FREE(uuid_node);
        }
        goto exit;
    }
    result = BT_STATUS_SUCCESS;
    
    uuid_node = (HidGattUuid *)RemoveHeadList(&channel->uuidQueue);
    uuid = LEtoHost16(uuid_node->uuid);
    if (uuid == BT_UUID_GATT_SERVICE_HID)
    {
        HidGattUuid *uuid_node_next = (HidGattUuid *)GetHeadList(&channel->uuidQueue);
        uuid = LEtoHost16(uuid_node_next->uuid);
        if (uuid == BT_UUID_GATT_SERVICE_HID)
        {
            Report(("[HID] more hid instances, continue to query..."));
        }
        else
        {
            /* Read HID Descriptor from Report Map Characteristic */
            result = HidRestoreHidService(channel);
            if (result == BT_STATUS_FAILED)
            {
                goto exit;
            }
        }
    }

    BLE_UTILS_FREE(uuid_node);


    while (!IsListEmpty(&channel->uuidQueue))
    {
        uuid_node = (HidGattUuid *)GetHeadList(&channel->uuidQueue);
        result = BLE_QueryServiceDefinitionByUuid(channel, channel->cmgrHandler.remDev, 
                                        HidGattServiceQueryCb, 2, uuid_node->uuid, &channel->pQueryHandle);

        kal_trace(BT_TRACE_BLE_PROFILES, HID_QUERYSERVICE_UUID, uuid_node->uuid[1], uuid_node->uuid[0]);
        Report(("[HID] query result: %d", result));
        if (result == BT_STATUS_PENDING)
        {
            return;
        }
        else if (result != BT_STATUS_SUCCESS)
        {
            /* Remove remaining uuid node in uuidQueue */
            while (!IsListEmpty(&channel->uuidQueue))
            {
                uuid_node = (HidGattUuid *)RemoveHeadList(&channel->uuidQueue);
                BLE_UTILS_FREE(uuid_node);
            }
            goto exit;
        }
        
        uuid_node = (HidGattUuid *)RemoveHeadList(&channel->uuidQueue);
        BLE_UTILS_FREE(uuid_node);
    }
    

    /* GATT query service done. Send HIDEVENT_QUERY_CNF to upper layer. The 
    *  "ptrs.queryRsp" field contains a pointer to the query response. 
    */    
    kal_trace(BT_TRACE_BLE_PROFILES, HID_QUERYSERVICE_DONE);
    BLE_UpdateServiceDatabase(&channel->cmgrHandler.remDev->bdAddr);
    

    /* Set HID service configuration */
    HidConfigHidService(channel);

    HidRestoreBatteryService(channel);

exit:
/*
    LeUtilParmsUpdate(channel->cmgrHandler.remDev, 
                            FALSE,
                            HID_LE_NORMAL_CONN_INTERVAL_MIN, 
                            HID_LE_NORMAL_CONN_INTERVAL_MAX, 
                            HID_LE_NORMAL_CONN_LATENCY, //U16 slave_latency, 
                            0x500);  //U16 timeout
*/
    channel->flags &= ~CHNL_FLAG_SERVICE_QUERY;
    HidAppCallback(channel, HIDEVENT_QUERY_CNF, result,
                   &channel->queryRsp, (U16)(sizeof(HidQueryRsp)));
}

/*---------------------------------------------------------------------------
 *            HidStartGattServiceQuery()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Start to query services on HID device
 *
 *          1. Exchange MTU first, to make ATT_MTU large enough to transfer the largest possible 
 *              Report characteristic value in a single transaction
 *          2. After MTU exchange is done, start to query GATT services on HID device
 *
 */
BtStatus HidStartGattServiceQuery(HidChannel *channel)
{
    U16 uuid_list_all[] = {
        BT_UUID_GATT_SERVICE_HID,
        BT_UUID_GATT_SERVICE_BATTERY,
        BT_UUID_GATT_SERVICE_DEVICE,
        BT_UUID_GATT_SERVICE_SCANPARAM
    };
    U16 uuid_list_max[16];
    U16 *uuid_list;

    HidGattUuid *uuid_node;
    BtStatus status;
    U16 i;
    U16 count = 0;
    BleService *service;
    BD_ADDR * bdAddr = &channel->cmgrHandler.remDev->bdAddr;
    
/*
{
    OS_MemSet((U8 *)&op, 0, sizeof(BtGattOp));
    InitializeListEntry(&op.op.node);	

    BT_GATT_ExchangeMTU(channel->cmgrHandler.remDev, &op);
}
*/
    Report(("[HID] HidStartGattServiceQuery..."));
    uuid_list = uuid_list_max;

    // HID service is mandatory service, so use it to check whether service record has already been obtained.
    for (i = 0; i < sizeof(uuid_list_all)/sizeof(uuid_list_all[0]); i++)
    {
        service = BLE_FindServiceByUuid(bdAddr, 2, uuid_list_all+i);
        if (service == NULL)
        {
            if (uuid_list_all[i] == BT_UUID_GATT_SERVICE_HID)
            {
                Report(("[HID] It is first time query, need to query all!"));
                uuid_list = uuid_list_all;
                count = sizeof(uuid_list_all)/sizeof(uuid_list_all[0]);
                break;
            }
            else
            {
                Report(("[HID] service %x not supported on remote, skip it...", uuid_list_all[i]));
                continue;
            }
        }
        else
        {
            uuid_list_max[count++] = uuid_list_all[i];
            while ((service = BLE_FindNextServiceByUuid(bdAddr, service, 2, uuid_list_all+i)) != NULL)
            {
                Report(("[HID] More than one instance, uuid: %x", uuid_list_all[i]));
                uuid_list_max[count++] = uuid_list_all[i];
            }
        }
        
    }
        
    Report(("[HID] query service count: %d", count));


    for (i = 0; i < count; i++)
    {
        uuid_node = (HidGattUuid *)BLE_UTILS_MALLOC(sizeof(HidGattUuid));
        OS_MemSet((U8 *)uuid_node, 0, sizeof(HidGattUuid));
        StoreLE16(uuid_node->uuid, uuid_list[i]);
        InsertTailList(&channel->uuidQueue, &uuid_node->node);
    }

    uuid_node = (HidGattUuid *)GetHeadList(&channel->uuidQueue);
    status = BLE_QueryServiceDefinitionByUuid(channel, channel->cmgrHandler.remDev, 
                                    HidGattServiceQueryCb, 2, uuid_node->uuid, &channel->pQueryHandle);
    if (status == BT_STATUS_SUCCESS)
    {
        HidGattServiceQueryCb((void *)channel, status);
        status = BT_STATUS_PENDING;
    }
    else if (status == BT_STATUS_PENDING)
    {
/*
        LeUtilParmsUpdate(channel->cmgrHandler.remDev, 
                                TRUE,
                                HID_LE_FAST_CONN_INTERVAL_MIN, 
                                HID_LE_FAST_CONN_INTERVAL_MAX, 
                                HID_LE_FAST_CONN_LATENCY, //U16 slave_latency, 
                                0x500);  //U16 timeout
*/
    }
    else
    {
        while (!IsListEmpty(&channel->uuidQueue))
        {
            uuid_node = (HidGattUuid *)RemoveHeadList(&channel->uuidQueue);
            BLE_UTILS_FREE(uuid_node);
        }
    }
    return status;
}

BtStatus HidSendLeTransPacket(HidChannel *Channel, 
                            HidTransaction *Trans)
{
    BtStatus       status = BT_STATUS_SUCCESS;

    kal_trace(BT_TRACE_BLE_PROFILES,HID_SENDLETRANSPACKET, Trans->type); 
    
    switch (Trans->type) {
    case HID_TRANS_CONTROL:
        {
            BleService *hidservice;
            BleChar *ctrl_char;
            U8 service_uuid[2];
            U8 char_uuid[2];
            
            // BLE HID Control Point Characteristic
            if (Trans->parm.control == HID_CTRL_SUSPEND)
            {
                Trans->parm.control = 0;
            }
            else if (Trans->parm.control == HID_CTRL_EXIT_SUSPEND)
            {
                Trans->parm.control = 1;
            }
            else
            {
                status = BT_STATUS_NOT_SUPPORTED;
                break;
            }
            
            StoreLE16(service_uuid, BT_UUID_GATT_SERVICE_HID);
            hidservice = BLE_FindServiceByUuid(&Channel->cmgrHandler.remDev->bdAddr, 2, service_uuid);
            StoreLE16(char_uuid, BT_UUID_HID_CHAR_CONTROL_POINT);
            ctrl_char = BLE_FindCharByUuid(hidservice, 2, char_uuid);

            status = HidWriteWithoutResponse(Channel, HidWriteWithoutResponseCallback, 
                                ctrl_char->handle, &Trans->parm.control, 1);
        }
        break;
        
    case HID_TRANS_GET_REPORT:      // may be only for PTS test
        {
            HidGattReport *report;
            BT_BOOL found = FALSE;
            report = (HidGattReport *)GetHeadList(&Channel->reportGattTable);
            while((ListEntry *)report != &Channel->reportGattTable)
            {
                if ((report->reportId == Trans->parm.reportReq->reportId) && 
                    (report->reportType == Trans->parm.reportReq->reportType))
                {
                    found = TRUE;
                    break;
                }
                report = (HidGattReport *)GetNextNode(&report->node);
            }

            if (!found)
            {
                status = BT_STATUS_FAILED;
                break;
            }

            status = HidReadCharacteristic(Channel, report->handle);
        }
        break;
        
    case HID_TRANS_SET_REPORT:
        {
            HidGattReport *report;
            BT_BOOL found = FALSE;
            U8 reportId = Trans->parm.report->data[0];
#ifdef __BT_HOGP_PTS_TEST__              
            U32 value = 0;

            OS_GetSystemPropertyUint("hogpptstest", &value, 1);             
            if (value)
            {
                Channel->uncompl_indicate_cnt = 0;
                report = (HidGattReport *)GetHeadList(&Channel->reportGattTable);                
                while ((ListEntry *)report != &Channel->reportGattTable)
                {
                    Report(("[HID]HID_TRANS_SET_REPORT: reportId=%d, reportType=%d, serviceInstance=%d",
                        reportId, Trans->parm.report->reportType, Trans->parm.report->serviceInstance));             
                    if (report->reportType == HID_REPORT_INPUT)
                    {
                        status = HidWriteCharacteristic(Channel, report->handle, g_input_data, sizeof(g_input_data)); 
                    }
                    else if (report->reportType == HID_REPORT_OUTPUT)
                    {             
                        status = HidWriteCharacteristic(Channel, report->handle, g_output_data, sizeof(g_output_data));                     
                    }
                    else if (report->reportType == HID_REPORT_FEATURE)
                    {
                        status = HidWriteCharacteristic(Channel, report->handle, g_feature_data, sizeof(g_feature_data)); 
                    }    
                    if (status == BT_STATUS_PENDING)
                    {
                        Channel->uncompl_indicate_cnt++;
                    }                     
                    report = (HidGattReport *)GetNextNode(&report->node);
                }
                if (!Channel->uncompl_indicate_cnt)
                {
                    status = BT_STATUS_FAILED;
                }
                break;
            }               
#endif            
            report = (HidGattReport *)GetHeadList(&Channel->reportGattTable);
            while((ListEntry *)report != &Channel->reportGattTable)
            {
                if ((report->reportId == reportId) && 
                    (report->reportType == Trans->parm.report->reportType) &&
                    (report->serviceInstance == Trans->parm.report->serviceInstance))
                {
                    found = TRUE;
                    break;
                }
                report = (HidGattReport *)GetNextNode(&report->node);
            }

            if (!found)
            {
                status = BT_STATUS_FAILED;
                break;
            }

            status = HidWriteCharacteristic(Channel, report->handle, 
                                    Trans->parm.report->data + 1, Trans->parm.report->dataLen - 1);
        }
        break;
        
    case HID_TRANS_GET_PROTOCOL:
        {
            BleService *hidservice;
            BleChar *protocol_char;
            U8 service_uuid[2];
            U8 char_uuid[2];
            HidTransaction trans;

            StoreLE16(service_uuid, BT_UUID_GATT_SERVICE_HID);
            hidservice = BLE_FindServiceByUuid(&Channel->cmgrHandler.remDev->bdAddr, 2, service_uuid);
            StoreLE16(char_uuid, BT_UUID_HID_CHAR_PROTOCOL_MODE);
            protocol_char = BLE_FindCharByUuid(hidservice, 2, char_uuid);

            trans.type = HID_TRANS_GET_PROTOCOL_RSP;
            trans.parm.protocol = protocol_char->value[0];
            HidAppCallback(Channel, HIDEVENT_TRANSACTION_RSP, 
                           BT_STATUS_SUCCESS, &trans, 
                           sizeof(HidTransaction));
            
            
            Trans->resultCode = HID_RESULT_SUCCESS;
            HidCompleteTransaction(Channel, Trans, 
                                   BT_STATUS_SUCCESS);
                        
        }
        break;
        
    case HID_TRANS_SET_PROTOCOL:
        {
            BleService *hidservice;
            BleChar *protocol_char;
            U8 service_uuid[2];
            U8 char_uuid[2];

            StoreLE16(service_uuid, BT_UUID_GATT_SERVICE_HID);
            hidservice = BLE_FindServiceByUuid(&Channel->cmgrHandler.remDev->bdAddr, 2, service_uuid);
            StoreLE16(char_uuid, BT_UUID_HID_CHAR_PROTOCOL_MODE);
            protocol_char = BLE_FindCharByUuid(hidservice, 2, char_uuid);

            status = HidWriteWithoutResponse(Channel, HidWriteWithoutResponseCallback, 
                                    protocol_char->value_handle, 
                                    &Trans->parm.protocol, sizeof(HidProtocol));
                        
        }
        break;
        
    case HID_TRANS_GET_IDLE_RATE:
    case HID_TRANS_SET_IDLE_RATE:
        // Unsupported command on BLE
        status = BT_STATUS_NOT_SUPPORTED;
        break;
        
    }

    return status;
}

BtStatus HidSendLeIntrPacket(HidChannel *Channel, HidInterrupt *Intr)
{
    BtStatus       status;
    HidGattReport *report;
    BT_BOOL found = FALSE;
    U8 reportId = Intr->data[0];
#ifdef __BT_HOGP_PTS_TEST__     
    U32 value = 0;

    OS_GetSystemPropertyUint("hogpptstest", &value, 1);             
    if (value)
    {
        Channel->uncompl_notify_cnt = 0;
        report = (HidGattReport *)GetHeadList(&Channel->reportGattTable);        
        while ((ListEntry *)report != &Channel->reportGattTable)
        {
            if (report->reportType == HID_REPORT_OUTPUT) 
            {
                status = HidWriteWithoutResponse(Channel, HidWriteOutputReportCallback,
                                            report->handle, 
                                            g_output_data, sizeof(g_output_data));
            }
            if (status == BT_STATUS_PENDING)
            {
                Channel->uncompl_notify_cnt++;
            }
            report = (HidGattReport *)GetNextNode(&report->node);
        }  
        if (!Channel->uncompl_notify_cnt)
        {
            return BT_STATUS_FAILED;
        }  
        else
        {
            return BT_STATUS_PENDING; 
        }       
    }    
#endif    

    report = (HidGattReport *)GetHeadList(&Channel->reportGattTable);
    while((ListEntry *)report != &Channel->reportGattTable)
    {
        if ((report->reportId == reportId) && 
            (report->reportType == Intr->reportType) &&
            (report->serviceInstance == Intr->serviceInstance))
        {
            found = TRUE;
            break;
        }
        report = (HidGattReport *)GetNextNode(&report->node);
    }
    
    if (!found)
    {
        return BT_STATUS_FAILED;
    }
    
    return HidWriteWithoutResponse(Channel, HidWriteOutputReportCallback,
                            report->handle, 
                            Intr->data + 1, Intr->dataLen - 1);

}
#endif  //__BT_4_0_BLE__
