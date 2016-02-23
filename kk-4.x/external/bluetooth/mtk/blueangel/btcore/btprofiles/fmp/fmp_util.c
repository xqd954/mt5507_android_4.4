/*****************************************************************************
 *
 * Filename:
 * ---------
 * fmp_util.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Find Me Profile utility function definition.
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
#include "bt_adp_fs.h"

FmpChannel *FmpNewChannel(void)
{
    FmpChannel *ch;
    U8 i;

    ch = (FmpChannel *)fmp_malloc(sizeof(FmpChannel));
    if (ch != NULL)
    {
        OS_MemSet((U8 *)ch, 0, sizeof(FmpChannel));
        InsertTailList(&FMP(dev_list), &(ch->node));
        CMGR_RegisterHandler(&ch->cmgr_handler, FmpCmgrCallback);	   

        /* Initialize packet pool for each device */
        InitializeListHead(&ch->packet_pool);
        for (i = 0; i < FMP_NUM_TX_PACKETS; i++) 
        {
            InsertTailList(&ch->packet_pool, &ch->packets[i].node);
        }
    }

    return ch;	
}

void FmpFreeChannel(FmpChannel *channel)
{
    CMGR_DeregisterHandler(&channel->cmgr_handler);
    RemoveEntryList(&(channel->node));
    fmp_free(channel->servicedb);
    fmp_free(channel);
}

FmpChannel *FmpFindChannelByLink(BtRemoteDevice *remote_dev)
{
    FmpChannel *channel;

    channel = (FmpChannel *)GetHeadList(&FMP(dev_list));
    while ((ListEntry *)channel != &FMP(dev_list))
    {
        if ((channel->link == remote_dev) ||
            ((channel->cmgr_handler.bdc != 0) &&
            (channel->cmgr_handler.bdc->link == remote_dev)) )
        {
            return channel;
        }

        /* try the next node in the device list */
        channel = (FmpChannel *)GetNextNode(&channel->node);		
    }

    return NULL;
}

FmpChannel *FmpFindChannelByAddr(BD_ADDR *bd_addr)
{
    FmpChannel *channel;

    channel = (FmpChannel *)GetHeadList(&FMP(dev_list));
    while ((ListEntry *)channel != &FMP(dev_list))
    {
        if ((channel->link != NULL) &&
            fmp_dev_addr_equal(bd_addr, &channel->link->bdAddr))
        {
            return channel;
        }

        /* try the next node in the device list */
        channel = (FmpChannel *)GetNextNode(&channel->node);		
    }

    return NULL;
}

FmpPacket *FmpGetPacket(FmpChannel *channel)
{
    FmpPacket *pkt = NULL;

    if (!IsListEmpty(&channel->packet_pool))
    {
        pkt = (FmpPacket *)RemoveHeadList(&channel->packet_pool);
    }
    else
    {
        kal_trace(BT_TRACE_BLE_PROFILES, FMP_PACKETPOOL_EMPTY);
    }
    Report(("[FMP] get packet: %x"));
    return pkt;
}

void FmpReleasePacket(FmpChannel *channel, FmpPacket *packet)
{
    Report(("[FMP] release packet: %x"));

    OS_MemSet((U8 *)packet, 0, sizeof(FmpPacket));
    InsertTailList(&channel->packet_pool, &packet->node);
}

void FmpClearConnection(FmpChannel *channel, BtStatus status)
{
    kal_trace(BT_TRACE_BLE_PROFILES, FMP_CLEAR_CONNECTION_STATUS, status);

    /* Release all resources in packet pool */


    /* Notify the application */
    FmpAppCallback(channel, FMPEVENT_CLOSED, status, 
			channel->link, sizeof(BtRemoteDevice));

    CMGR_RemoveDataLink(&channel->cmgr_handler);
    CMGR_RemoveCmgrWithDevice(&channel->cmgr_handler);

    FmpFreeChannel(channel);
}

void FmpAppCallback(FmpChannel *channel, FmpEvent event, U32 status, void *ptr, U16 len)
{
    FmpCallbackParms parms;

    switch (event)
    {
        case FMPEVENT_OPEN:
        case FMPEVENT_CLOSED:
            parms.parm.rem_dev = ptr;
            break;
        case FMPEVENT_GATT_QUERY_CNF:
            if (status != BT_STATUS_SUCCESS)
            {
                fmp_free(channel->servicedb);
            }
            break;
        case FMPEVENT_SET_ALERT_LEVEL_REQ:
        case FMPEVENT_SET_ALERT_LEVEL_CNF:
            parms.parm.level = *(U8 *)ptr;
            break;
        default:
            break;
    }
    
    parms.event = event;
    parms.len = len;
    parms.status = status;
    
    FMP(fmp_callback)(channel, &parms);
}

void FmpWriteServiceDB(FmpServiceDB *servicedb)
{
    S32 fd;
    U32 mode = BTMTK_FS_READ_WRITE;

    if (!btmtk_fs_is_dir_exist((const U8*)FMP_SERVICE_DATABASE_FILE))
    {
        mode |= BTMTK_FS_CREATE;
    }

    fd = btmtk_fs_open((const U8*)FMP_SERVICE_DATABASE_FILE, mode);
    if (fd < 0)
    {
        return;
    }

    btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_END);
    /* There might be paddings in file caused by 4 bytes alignment in structure */
    btmtk_fs_write(fd, (U8 *)servicedb, sizeof(FmpServiceDB));
    btmtk_fs_close(fd);

}

BtStatus FmpReadServiceDB(BD_ADDR *bd_addr, FmpServiceDB **servicedb)
{
    S32 fd;
    BD_ADDR addr;
    FmpServiceDB *db;
    S32 pos_file_end;
    S32 pos_end;
    U16 service_size;
    

    fd = btmtk_fs_open((const U8*)FMP_SERVICE_DATABASE_FILE, BTMTK_FS_READ_ONLY);
    if (fd < 0)
    {
        // file is not exist
        return BT_STATUS_FAILED;
    }

    service_size = sizeof(FmpServiceDB);
    pos_file_end = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_END);
    pos_end = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_BEGIN);

    /* Seach database file util the end of the file */
    while (pos_end != pos_file_end)
    {
        btmtk_fs_read(fd, (U8 *)&addr, 6);

        Report(("[FMP] read service db, addr: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", addr.addr[0], 
            addr.addr[1], addr.addr[2], addr.addr[3], addr.addr[4], addr.addr[5]));

        if (!fmp_dev_addr_equal(bd_addr, &addr))
        {
            pos_end = btmtk_fs_seek(fd, service_size - 6, BTMTK_FS_SEEK_CURRENT);
            continue;
        }
        break;
    }

    if (pos_end == pos_file_end)
    {
        /* No matched device database has been found */
        btmtk_fs_close(fd);
        return BT_STATUS_FAILED;
    }

    db = (FmpServiceDB *)fmp_malloc(sizeof(FmpServiceDB));
    if (db == NULL)
    {
        return BT_STATUS_NO_RESOURCES;
    }

    btmtk_fs_seek(fd, pos_end, BTMTK_FS_SEEK_BEGIN);
    btmtk_fs_read(fd, (U8 *)db, service_size);
    Report(("[FMP] read service db, handle: 0x%x 0x%x 0x%x ", db->start_handle, db->end_handle, db->char_value_handle));

    btmtk_fs_close(fd);
    
    *servicedb = db;
    return BT_STATUS_SUCCESS;
}

void FmpRemoveServiceDB(BD_ADDR *bd_addr)
{
    S32 fd;
    S32 fd_temp;
    BD_ADDR addr;
    U16 service_size;
    S32 pos_file_end;
    S32 pos_start;
    S32 pos_end;
    U8 *buffer;

    fd = btmtk_fs_open((const U8*)FMP_SERVICE_DATABASE_FILE, BTMTK_FS_READ_ONLY);
    if (fd < 0)
    {
        // file is not exist
        return;
    }

    service_size = sizeof(FmpServiceDB);
    pos_file_end = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_END);
    pos_start = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_BEGIN);
    pos_end = pos_start;

    /* Seach database file util the end of the file */
    while (pos_end != pos_file_end)
    {
        btmtk_fs_read(fd, (U8 *)&addr, 6);

        Report(("[FMP] write service db, addr: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", addr.addr[0], 
            addr.addr[1], addr.addr[2], addr.addr[3], addr.addr[4], addr.addr[5]));

        if (!fmp_dev_addr_equal(bd_addr, &addr))
        {
            pos_end = btmtk_fs_seek(fd, service_size - 6, BTMTK_FS_SEEK_CURRENT);
            continue;
        }
        break;
    }

    if (pos_end == pos_file_end)
    {
        /* No matched device database has been found */
        kal_trace(BT_TRACE_BLE_PROFILES, FMP_REMOVEDATABASE_NO_MATCHED);
        btmtk_fs_close(fd);
        return;
    }

    /* Case 1:  If there is only one database, delete the file directly */
    if (pos_file_end - pos_start == service_size)
    {
        kal_trace(BT_TRACE_BLE_PROFILES, HTP_REMOVEDATABASE_DELETEFILE);
        btmtk_fs_delete((const U8 *)FMP_SERVICE_DATABASE_FILE);
        return;
    }

    /* Case 2:  If there is more than one database, create a new temp file, 
    *   move the left database to the temp file, delete the original database
    *   file, and rename the temp file as the new database file. 
    */
    fd_temp = btmtk_fs_open((const U8*)FMP_SERVICE_DATABASE_TEMP_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
    if (fd_temp < 0)
    {
        return;
    }

    if (pos_end != pos_start)
    {
        /* Move first half of the database to temp file */
        buffer = (U8 *)fmp_malloc(pos_end - pos_start);
        btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_BEGIN);
        btmtk_fs_read(fd, buffer, pos_end - pos_start);
        btmtk_fs_write(fd_temp, buffer, pos_end - pos_start);
        fmp_free(buffer);

        pos_start = btmtk_fs_seek(fd, service_size, BTMTK_FS_SEEK_CURRENT);
    }
    else
    {
        pos_start = btmtk_fs_seek(fd, service_size - 6, BTMTK_FS_SEEK_CURRENT);
    }

    if (pos_start != pos_file_end)
    {
        /* Move first half of the database to temp file */
        buffer = (U8 *)fmp_malloc(pos_file_end - pos_start);
        btmtk_fs_read(fd, buffer, pos_file_end - pos_start);
        btmtk_fs_write(fd_temp, buffer, pos_file_end - pos_start);
        fmp_free(buffer);
    }

    btmtk_fs_close(fd);
    btmtk_fs_close(fd_temp);

    btmtk_fs_delete((const U8 *)FMP_SERVICE_DATABASE_FILE);
    btmtk_fs_rename((const U8 *)FMP_SERVICE_DATABASE_TEMP_FILE, (const U8 *)FMP_SERVICE_DATABASE_FILE);

}
#endif
