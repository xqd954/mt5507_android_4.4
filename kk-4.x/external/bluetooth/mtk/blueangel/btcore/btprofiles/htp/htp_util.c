/*****************************************************************************
 *
 * Filename:
 * ---------
 * htp_util.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Health Thermometer Profile utility function definition.
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
 * 12 30 2011 dlight.ting
 * NULL
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#if defined(__BT_HTPT_PROFILE__) || defined(__BT_HTPC_PROFILE__)

#include "sys/htpi.h"
#include "bt_adp_fs.h"

//#define __HTP_IDLE_CONNECTION_TIMER__

HtpChannel *HtpNewChannel(void)
{
    HtpChannel *ch;
    U8 i;

    ch = (HtpChannel *)htp_malloc(sizeof(HtpChannel));
    if (ch != NULL)
    {
        OS_MemSet((U8 *)ch, 0, sizeof(HtpChannel));
        EVM_ResetTimer(&ch->readCharTimer);      
        ch->readCharRetryCount = 0;
        InsertTailList(&HTP(dev_list), &(ch->node));
        CMGR_RegisterHandler(&ch->cmgr_handler, HtpCmgrCallback);	   

        /* Initialize packet pool for each device */
        InitializeListHead(&ch->packet_pool);
        for (i = 0; i < HTP_NUM_TX_PACKETS; i++) 
        {
            InsertTailList(&ch->packet_pool, &ch->packets[i].node);
        }
    }

    return ch;	
}

void HtpFreeChannel(HtpChannel *channel)
{
    CMGR_DeregisterHandler(&channel->cmgr_handler);
    RemoveEntryList(&(channel->node));
    EVM_ResetTimer(&channel->readCharTimer);      
    channel->readCharRetryCount = 0;
    htp_free(channel);
}

HtpChannel *HtpFindChannelByLink(BtRemoteDevice *remote_dev)
{
    HtpChannel *channel;

    channel = (HtpChannel *)GetHeadList(&HTP(dev_list));
    while ((ListEntry *)channel != &HTP(dev_list))
    {
        if ((channel->link == remote_dev) ||
            ((channel->cmgr_handler.bdc != 0) &&
            (channel->cmgr_handler.bdc->link == remote_dev)) )
        {
            return channel;
        }

        /* try the next node in the device list */
        channel = (HtpChannel *)GetNextNode(&channel->node);		
    }

    return NULL;
}

HtpChannel *HtpFindChannelByAddr(BD_ADDR *bd_addr)
{
    HtpChannel *channel;

    channel = (HtpChannel *)GetHeadList(&HTP(dev_list));
    while ((ListEntry *)channel != &HTP(dev_list))
    {
        if ((channel->link != NULL) &&
            htp_dev_addr_equal(bd_addr, &channel->link->bdAddr))
        {
            return channel;
        }

        /* try the next node in the device list */
        channel = (HtpChannel *)GetNextNode(&channel->node);		
    }

    return NULL;
}

HtpService *HtpFindServicebyHandleRange(HtpServiceDB *servicedb, U16 handle)
{
    HtpService *service = NULL;

    if ((handle >= servicedb->hts_service.start_handle) &&
        (handle <= servicedb->hts_service.end_handle))
    {
        service = &servicedb->hts_service;
    }
    else if ((handle >= servicedb->dis_service.start_handle) &&
        (handle <= servicedb->dis_service.end_handle))
    {
        service = &servicedb->dis_service;
    }
    return service;
}

HtpCharListNode *HtpFindCharbyHandleRange(HtpService *service, U16 handle)
{
    HtpCharListNode *char_node;
    HtpCharListNode *char_next_node;

    char_node = (HtpCharListNode *)GetHeadList(&service->char_list);
    while ((ListEntry *)char_node != &service->char_list)
    {
        char_next_node = (HtpCharListNode *)GetNextNode(&char_node->node);		

        if (((ListEntry *)char_next_node == &service->char_list) &&
            (handle > char_node->handle) &&
            (handle <= service->end_handle))
        {
            return char_node;                
        }
        
        if ((handle > char_node->handle) &&
              (handle < char_next_node->handle))
        {
            return char_node;
        }
        /* try the next node in the list */
        char_node = char_next_node;		
    }
    return NULL;
}

/* characteristic value may not continously follow the declaration
*
*   <characteristic 1's declaration> 
*   <characteristic 2's declaration> 
*   <characteristic 1's value> 
*   <characteristic 2's value>
*/
HtpCharListNode *HtpFindCharbyHandle(HtpService *service, U16 value_handle)
{
    HtpCharListNode *char_node;

    char_node = (HtpCharListNode *)GetHeadList(&service->char_list);
    while ((ListEntry *)char_node != &service->char_list)
    {
        if (char_node->value_handle == value_handle)
        {
            return char_node;
        }
        /* try the next node in the list */
        char_node = (HtpCharListNode *)GetNextNode(&char_node->node);		
    }
    return NULL;
}

HtpCharListNode *HtpFindCharByUuid(HtpService *service, U16 char_uuid)
{
    HtpCharListNode *char_node;

    char_node = (HtpCharListNode *)GetHeadList(&service->char_list);
    while ((ListEntry *)char_node != &service->char_list)
    {
        if (char_node->value_uuid == char_uuid)
        {
            return char_node;
        }
        /* try the next node in the list */
        char_node = (HtpCharListNode *)GetNextNode(&char_node->node);		
    }

    return NULL;
}

HtpCharDescListNode * HtpFindCharDescByUuid(HtpService *service, U16 char_uuid, U16 desc_uuid)
{
    HtpCharListNode *char_node;
    HtpCharDescListNode	*desc_node;

    char_node = (HtpCharListNode *)GetHeadList(&service->char_list);
    while ((ListEntry *)char_node != &service->char_list)
    {
        if (char_node->value_uuid == char_uuid)
        {
            desc_node = (HtpCharDescListNode *)GetHeadList(&char_node->char_desc_list);
            while ((ListEntry *)desc_node != &char_node->char_desc_list)  	
            {
                if (desc_node->desc_uuid == desc_uuid)
                {
                    return desc_node;
                }
                /* try the next node in the list */
                desc_node = (HtpCharDescListNode *)GetNextNode(&desc_node->node);		
            }
        }
        /* try the next node in the list */
        char_node = (HtpCharListNode *)GetNextNode(&char_node->node);		
    }

    return NULL;
}

HtpServiceDB *HtpNewServiceDB(BD_ADDR *addr)
{
    BtStatus status;
    HtpServiceDB *servicedb;

    servicedb = (HtpServiceDB *)htp_malloc(sizeof(HtpServiceDB));
    if (servicedb == NULL)
    {
        return NULL;
    }
    
    OS_MemSet((U8 *)servicedb, 0, sizeof(HtpServiceDB));
    servicedb->bt_addr = *addr;
    //servicedb->size = sizeof(servicedb->bt_addr) + sizeof(servicedb->features); 

    /* Initialize Lists */
    InitializeListHead(&servicedb->hts_service.char_list);
    InitializeListHead(&servicedb->dis_service.char_list);
    
    return servicedb;
}

void HtpFreeServiceDB(HtpServiceDB *servicedb)
{
    HtpCharListNode *char_node;
    HtpCharDescListNode *char_desc_node;	

    /* 1st: free hts service memory, including all characteristic nodes and descriptor nodes */
    while (!IsListEmpty(&servicedb->hts_service.char_list))
    {
        char_node = (HtpCharListNode *)RemoveHeadList(&servicedb->hts_service.char_list);
        while (!IsListEmpty(&char_node->char_desc_list))
        {
            char_desc_node = (HtpCharDescListNode *)RemoveHeadList(&char_node->char_desc_list);
            htp_free(char_desc_node->desc);
            htp_free(char_desc_node);
        }
        htp_free(char_node->value); 		
        htp_free(char_node);
    }

    /* 2st: free dis service memory, including all characteristic nodes */
    while (!IsListEmpty(&servicedb->dis_service.char_list))
    {
        char_node = (HtpCharListNode *)RemoveHeadList(&servicedb->dis_service.char_list);
        htp_free(char_node->value); 		
        htp_free(char_node);
    }

    htp_free(servicedb);	

}

HtpPacket *HtpGetPacket(HtpChannel *channel)
{
    HtpPacket *pkt = NULL;

    if (!IsListEmpty(&channel->packet_pool))
    {
        pkt = (HtpPacket *)RemoveHeadList(&channel->packet_pool);
    }
    else
    {
        kal_trace(BT_TRACE_BLE_PROFILES, HTP_PACKETPOOL_EMPTY);
    }
    Report(("[HTP] get packet: %x"));
    return pkt;
}

void HtpReleasePacket(HtpChannel *channel, HtpPacket *packet)
{
    Report(("[HTP] release packet: %x"));

    OS_MemSet((U8 *)packet, 0, sizeof(HtpPacket));
    InsertTailList(&channel->packet_pool, &packet->node);
}

void HtpAppCallback(HtpChannel *channel, HtpEvent event, U32 status, void *ptr, U16 len)
{
    HtpCallbackParms parms;

    switch (event)
    {
        case HTPEVENT_GATT_QUERY_CNF:
            {
                if (status == BT_STATUS_SUCCESS)
                {
                    parms.parm.rem_features = channel->servicedb->features;
                }
                else
                {
                    if (channel->servicedb != NULL)
                    {
                        /* If there is any error happened, do gatt service query again later */
                        HtpFreeServiceDB(channel->servicedb);
                        channel->servicedb = NULL;
                    }
                }
            }
            break;		
            
        case HTPEVENT_SET_TEMPERATURE_CONFIG_CNF:
        case HTPEVENT_SET_INTERMEDIATE_TEMPERATURE_CONFIG_CNF:
        case HTPEVENT_SET_INTERVAL_CONFIG_CNF:
        case HTPEVENT_SET_INTERVAL_CNF:
        case HTPEVENT_MEASUREMENT_IND_CNF:
        case HTPEVENT_MEASUREMENT_NOTIFY_CNF:
            parms.parm.pkt_handled = (U8 *)ptr;
            break;          

        case HTPEVENT_OPEN:
        case HTPEVENT_CLOSED:
            parms.parm.rem_dev = (BtRemoteDevice *)ptr;
            break;			

        /* It is for Collector event only */
        case HTPEVENT_MEASUREMENT_IND:		
        case HTPEVENT_MEASUREMENT_NOTIFY:	
            parms.parm.temperature = (HtpTemperature *)ptr;
            break;
        case HTPEVENT_INTERVAL_VALUE_IND:	
            parms.parm.interval = *(HtpMeasurementInterval *)ptr;
            break;			

        /* It is for Thermometer event only */
        case HTPEVENT_SET_TEMPERATURE_CONFIG_REQ:
        case HTPEVENT_SET_INTERMEDIATE_TEMPERATURE_CONFIG_REQ:
        case HTPEVENT_SET_INTERVAL_CONFIG_REQ:
            parms.parm.flag = *(HtpClientCharConfig *)ptr;
            break;			
        case HTPEVENT_SET_INTERVAL_REQ:
            parms.parm.interval = *(HtpMeasurementInterval *)ptr;
            break;			
        default:
            break;			
    }

    parms.event = event;
    parms.status = status;
    parms.len = len;

    if (channel->role == HTP_ROLE_COLLECTOR)
    {
        HTP(htpc_callback)(channel, &parms);
    }
    else
    {
        HTP(htpt_callback)(channel, &parms);
    }
}

void HtpClearConnection(HtpChannel *channel, BtStatus status)
{
    kal_trace(BT_TRACE_BLE_PROFILES, HTP_CLEAR_CONNECTION_STATUS, status);

    /* Release all resources in packet pool */


    /* Notify the application */
    HtpAppCallback(channel, HTPEVENT_CLOSED, status, 
			channel->link, sizeof(BtRemoteDevice));

    CMGR_RemoveDataLink(&channel->cmgr_handler);
    CMGR_RemoveCmgrWithDevice(&channel->cmgr_handler);

    if ((channel->role == HTP_ROLE_COLLECTOR) &&
        (channel->servicedb != NULL))
    {
        HtpFreeServiceDB(channel->servicedb);
    }
    
    HtpFreeChannel(channel);
}
/*
void htp_test(void)
{
    S32 fd;
    U8 *buffer;
    S32 pos_file_end;
    U8 addr1[] = {0x21, 0x22, 0x46, 0x66, 0xaa, 0x1a};
    U8 addr2[] = {0x20, 0x20, 0x66, 0x46, 0xae, 0x7a};
    U8 addr3[] = {0x23, 0x23, 0x46, 0x66, 0xaa, 0x1a};

    fd = btmtk_fs_open((const U8*)HTP_SERVICE_DATABASE_FILE, BTMTK_FS_READ_WRITE);
    if (fd < 0)
    {
        return;
    }

    pos_file_end = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_END);
    buffer = (U8 *)htp_malloc(pos_file_end);

    btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_BEGIN);
    btmtk_fs_read(fd, buffer, pos_file_end);

    btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_BEGIN);

    OS_MemCopy(buffer + 2, addr1, 6);
    btmtk_fs_write(fd, buffer, pos_file_end);
    OS_MemCopy(buffer + 2, addr2, 6);
    btmtk_fs_write(fd, buffer, pos_file_end);
    OS_MemCopy(buffer + 2, addr3, 6);
    btmtk_fs_write(fd, buffer, pos_file_end);
    btmtk_fs_close(fd);
    htp_free(buffer);
}
*/
void HtpWriteServiceDB(HtpServiceDB *servicedb)
{
    S32 fd;
    U8 *buffer;
    U8 *ptr;
    HtpCharListNode *char_node;
    HtpCharDescListNode *char_desc_node;
    HtpService *service;
    U32 mode = BTMTK_FS_READ_WRITE;
    U16 size;

    if (!btmtk_fs_is_dir_exist((const U8*)HTP_SERVICE_DATABASE_FILE))
    {
        mode |= BTMTK_FS_CREATE;
    }

    fd = btmtk_fs_open((const U8*)HTP_SERVICE_DATABASE_FILE, mode);
    if (fd < 0)
    {
        return;
    }

    size = servicedb->hts_service.size + servicedb->dis_service.size 
                + sizeof(servicedb->features) + sizeof(BD_ADDR);
    Report(("[HTP] write service database to file, size: %d", size));

    buffer = (U8 *)htp_malloc(size + 2);
    ptr = buffer;

    OS_MemCopy(ptr, (U8 *)&size, 2);
    ptr += 2;
    OS_MemCopy(ptr, (U8 *)&servicedb->bt_addr, 6);
    ptr += 6;
    OS_MemCopy(ptr, (U8 *)&servicedb->features, sizeof(servicedb->features));
    ptr += sizeof(servicedb->features);
  
    // copy hts service
    service = &servicedb->hts_service;
    while ((U16)(ptr - buffer) != (size + 2))
    {
        OS_MemCopy(ptr, (U8 *)&service->size, sizeof(service->size));
        ptr += sizeof(service->size);
        OS_MemCopy(ptr, (U8 *)&service->start_handle, 2);
        ptr += 2;
        OS_MemCopy(ptr, (U8 *)&service->end_handle, 2);
        ptr += 2;

        Report(("[HTP] start handle: %d, end handle: %d", service->start_handle, service->end_handle));

        char_node = (HtpCharListNode *)GetHeadList(&service->char_list);
        while ((ListEntry *)char_node != &service->char_list)
        {
            OS_MemCopy(ptr, (U8 *)&char_node->size, sizeof(char_node->size));
            ptr += sizeof(char_node->size);
            OS_MemCopy(ptr, (U8 *)&char_node->handle, 2);
            ptr += 2;
            *ptr++ = char_node->properties;
            OS_MemCopy(ptr, (U8 *)&char_node->value_handle, 2);
            ptr += 2;
            OS_MemCopy(ptr, (U8 *)&char_node->value_uuid, 2);
            ptr += 2;

            char_desc_node = (HtpCharDescListNode *)GetHeadList(&char_node->char_desc_list);
            while ((ListEntry *)char_desc_node != &char_node->char_desc_list)
            {
                OS_MemCopy(ptr, (U8 *)&char_desc_node->handle, 2);
                ptr += 2;
                OS_MemCopy(ptr, (U8 *)&char_desc_node->desc_uuid, 2);
                ptr += 2;

                char_desc_node = (HtpCharDescListNode *)GetNextNode(&char_desc_node->node);
            }
            
            char_node = (HtpCharListNode *)GetNextNode(&char_node->node);
        }

        service = (HtpService *)((U8 *)service + sizeof(HtpService));
    }

    btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_END);
    btmtk_fs_write(fd, buffer, size + 2);
    btmtk_fs_close(fd);
    
    htp_free(buffer);

}

BtStatus HtpReadServiceDB(BD_ADDR *bd_addr, HtpServiceDB **servicedb)
{
    S32 fd;
    BD_ADDR addr;
    HtpServiceDB *db = NULL;
    HtpService *service;
    HtpCharListNode *char_node;
    HtpCharDescListNode *char_desc_node;
    U16 size;
    S32 pos_db_start;
    S32 pos_cur;
    S32 pos_file_end;
    U16 service_offset;
    U16 char_offset;
    //U16 end_handle;


    fd = btmtk_fs_open((const U8*)HTP_SERVICE_DATABASE_FILE, BTMTK_FS_READ_ONLY);
    if (fd < 0)
    {
        // file is not exist
        return BT_STATUS_FAILED;
    }

    pos_file_end = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_END);
    pos_cur = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_BEGIN);

    /* Seach database file util the end of the file */
    while (pos_cur != pos_file_end)
    {
        btmtk_fs_read(fd, (U8 *)&size, 2);

        pos_db_start = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_CURRENT);
        btmtk_fs_read(fd, (U8 *)&addr, 6);

        if (!htp_dev_addr_equal(bd_addr, &addr))
        {
            pos_cur = btmtk_fs_seek(fd, size - 6, BTMTK_FS_SEEK_CURRENT);
            continue;
        }
        break;
    }

    if (pos_cur == pos_file_end)
    {
        /* No matched device database has been found */
        btmtk_fs_close(fd);
        return BT_STATUS_FAILED;
    }

    /* The indicated database is found in database file. 
    *   Read all data related with the service database from file. 
    */
    
    db = HtpNewServiceDB(bd_addr);
    if (db == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }
    
    //db->size = size;
    db->bt_addr = addr;
    btmtk_fs_read(fd, (U8 *)&db->features, sizeof(db->features));
    
    Report(("[HTP] read service db, size: %d, feature: 0x%x", size, db->features));
    service = &db->hts_service;
    
    Report(("[HTP] pos start: %d", pos_db_start));
    /* Read all data related with the database */
    while (pos_cur - pos_db_start != size)
    {
        btmtk_fs_read(fd, (U8 *)&service->size, sizeof(service->size));
        btmtk_fs_read(fd, (U8 *)&service->start_handle, 2);
        btmtk_fs_read(fd, (U8 *)&service->end_handle, 2);
    
        Report(("[HTP] start handle: %d, end handle: %d", service->start_handle, service->end_handle));
        
        //end_handle = service->start_handle;
        service_offset = service->size - HTP_SERVICE_SIZE;
        /* Read all characteristics related with the service */
        while (service_offset > 0)
        {
            char_node = (HtpCharListNode *)htp_malloc(sizeof(HtpCharListNode));
            OS_MemSet((U8 *)char_node, 0, sizeof(HtpCharListNode));
    
            InitializeListHead(&char_node->char_desc_list);
            btmtk_fs_read(fd, (U8 *)&char_node->size, sizeof(char_node->size));
            btmtk_fs_read(fd, (U8 *)&char_node->handle, 2);
            btmtk_fs_read(fd, &char_node->properties, 1);
            btmtk_fs_read(fd, (U8 *)&char_node->value_handle, 2);
            btmtk_fs_read(fd, (U8 *)&char_node->value_uuid, 2);

            /* Insert characteristic node into service characteristic list */
            InsertTailList(&service->char_list, &char_node->node);
    
  
            //end_handle = char_node->value_handle;
            char_offset = char_node->size - HTP_CHARACTERISTIC_SIZE;
            /* Read all characteristic descriptors related with the characteristic */
            while (char_offset > 0)
            {
                char_desc_node = (HtpCharDescListNode *)htp_malloc(sizeof(HtpCharDescListNode));
                OS_MemSet((U8 *)char_desc_node, 0, sizeof(HtpCharDescListNode));
    
                btmtk_fs_read(fd, (U8 *)&char_desc_node->handle, 2);
                btmtk_fs_read(fd, (U8 *)&char_desc_node->desc_uuid, 2);

                /* Insert descriptor into charateristic descriptor list */
                InsertTailList(&char_node->char_desc_list, &char_desc_node->node);
                
                //end_handle = char_desc_node->handle;
                char_offset -= HTP_CHARACTERISTIC_DESCRIPTOR_SIZE;
            }
            service_offset -= char_node->size;
        }
        
        service = (HtpService *)((U8 *)service + sizeof(HtpService));
        pos_cur = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_CURRENT);
    
        Report(("[HTP] pos current: %d", pos_cur));
    }

    btmtk_fs_close(fd);
    
    *servicedb = db;
    return BT_STATUS_SUCCESS;
}

void HtpRemoveServiceDB(BD_ADDR *bd_addr)
{
    S32 fd;
    S32 fd_temp;
    BD_ADDR addr;
    U16 size;
    S32 pos_start;
    S32 pos_end;
    S32 pos_file_end;
    U8 *buffer;

    fd = btmtk_fs_open((const U8*)HTP_SERVICE_DATABASE_FILE, BTMTK_FS_READ_ONLY);
    if (fd < 0)
    {
        // file is not exist
        return;
    }

    pos_file_end = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_END);
    pos_start = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_BEGIN);
    pos_end = pos_start;

    /* Seach database file util the end of the file */
    while (pos_end != pos_file_end)
    {
        btmtk_fs_read(fd, (U8 *)&size, 2);
        btmtk_fs_read(fd, (U8 *)&addr, 6);

        if (!htp_dev_addr_equal(bd_addr, &addr))
        {
            pos_end = btmtk_fs_seek(fd, size - 6, BTMTK_FS_SEEK_CURRENT);
            continue;
        }
        break;
    }

    Report(("[HTP] pos start: %d, pos end: %d, file end: %d", pos_start, pos_end, pos_file_end));

    if (pos_end == pos_file_end)
    {
        /* No matched device database has been found */
        kal_trace(BT_TRACE_BLE_PROFILES, HTP_REMOVEDATABASE_NO_MATCHED);
        btmtk_fs_close(fd);
        return;
    }

    /* Case 1:  If there is only one database, delete the file directly */
    if (pos_file_end - pos_start == size + 2)
    {
        kal_trace(BT_TRACE_BLE_PROFILES, HTP_REMOVEDATABASE_DELETEFILE);
        btmtk_fs_delete((const U8 *)HTP_SERVICE_DATABASE_FILE);
        return;
    }

    /* Case 2:  If there is more than one database, create a new temp file, 
    *   move the left database to the temp file, delete the original database
    *   file, and rename the temp file as the new database file. 
    */
    fd_temp = btmtk_fs_open((const U8*)HTP_SERVICE_DATABASE_TEMP_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
    if (fd_temp < 0)
    {
        return;
    }

    if (pos_end != pos_start)
    {
        /* Move first half of the database to temp file */
        buffer = (U8 *)htp_malloc(pos_end - pos_start);
        btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_BEGIN);
        btmtk_fs_read(fd, buffer, pos_end - pos_start);
        btmtk_fs_write(fd_temp, buffer, pos_end - pos_start);
        htp_free(buffer);

        pos_start = btmtk_fs_seek(fd, size + 2, BTMTK_FS_SEEK_CURRENT);
    }
    else
    {
        pos_start = btmtk_fs_seek(fd, size - 6, BTMTK_FS_SEEK_CURRENT);
    }

    if (pos_start != pos_file_end)
    {
        /* Move first half of the database to temp file */
        buffer = (U8 *)htp_malloc(pos_file_end - pos_start);
        btmtk_fs_read(fd, buffer, pos_file_end - pos_start);
        btmtk_fs_write(fd_temp, buffer, pos_file_end - pos_start);
        htp_free(buffer);
    }

    Report(("[HTP] pos start: %d", pos_start));

    btmtk_fs_close(fd);
    btmtk_fs_close(fd_temp);
    
    btmtk_fs_delete((const U8 *)HTP_SERVICE_DATABASE_FILE);
    btmtk_fs_rename((const U8 *)HTP_SERVICE_DATABASE_TEMP_FILE, (const U8 *)HTP_SERVICE_DATABASE_FILE);
}

void HtpIdleConnectionStartTimer(HtpChannel *channel, U16 timer_ms)
{
#ifdef __HTP_IDLE_CONNECTION_TIMER__
    HTP(idle_timer).func = HTP_Close();
    HTP(idle_timer).context = (void *)channel;
    EVM_StartTimer(&HTP(idle_timer)), timer_ms);
#endif
}

void HtpIdleConnectionStopTimer(void)
{
#ifdef __HTP_IDLE_CONNECTION_TIMER__
    EVM_CancelTimer(&HTP(idle_timer));
#endif
}

#endif
