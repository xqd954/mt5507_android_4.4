/*****************************************************************************
 *
 * Filename:
 * ---------
 * bleservice_db.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth BLE Service database management.
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
#include "attdef.h"
#include "attdbmain.h"

#include "gatt.h"
#include "gattcb.h"
#include "sec.h"

#include "bleservice_db.h"
#include "bt_adp_fs.h"

#if defined(BTMTK_ON_LINUX)
#define BLE_SERVICE_DATABASE_FILE       "/data/@btmtk/bleservicedb"  
#define BLE_SERVICE_DATABASE_TEMP_FILE       "/data/@btmtk/bleservicedb_temp"  
#endif

typedef struct _ble_service_db_context
{
    ListEntry servicedbList;
    BtGattHandler gattHandler;
    U8 loadCount;
    BtBleBondCbHandler  bondingHandler;
} ble_service_db_context;

ble_service_db_context BleServiceDbCtx;

#define BleDbCtx(f)         (BleServiceDbCtx.f)


extern void BT_GattRegisterConnectedEvent(BtGattHandler *hdl, GattAPCallBack cb);
extern void BT_GattUnregisterConnectedEvent(BtGattHandler *hdl);

static ble_service_db *BleServiceDbFindRecord(BD_ADDR *bd_addr)
{
    ble_service_db *servicedb;

    servicedb = (ble_service_db *)GetHeadList(&BleDbCtx(servicedbList));
    while ((ListEntry *)servicedb != &BleDbCtx(servicedbList))
    {
        if (OS_MemCmp(bd_addr->addr, 6, servicedb->bdAddr.addr, 6))
        {
            return servicedb;
        }
        servicedb = (ble_service_db *)GetNextNode(&servicedb->node);
    }

    return NULL;
}

void BleServiceDbUpdateRecord(ble_service_db *servicedb)
{
    S32 fd;
    S32 pos_current;
    S32 pos_end;
    U32 mode = BTMTK_FS_READ_WRITE;
    U8 *db_buff;

    if (!btmtk_fs_is_dir_exist((const U8*)BLE_SERVICE_DATABASE_FILE))
    {
        mode |= BTMTK_FS_CREATE;
    }

    fd = btmtk_fs_open((const U8*)BLE_SERVICE_DATABASE_FILE, mode);
    if (fd < 0)
    {
        return;
    }

    pos_end = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_END);
    pos_current = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_BEGIN);

    db_buff = (U8 *)get_ctrl_buffer(sizeof(ble_service_db) - sizeof(ListEntry));
    
    while (pos_current < pos_end)
    {
        btmtk_fs_read(fd, db_buff, sizeof(ble_service_db) - sizeof(ListEntry));
        if (OS_MemCmp(db_buff, 6, servicedb->bdAddr.addr, 6))
        {
            btmtk_fs_seek(fd, pos_current, BTMTK_FS_SEEK_BEGIN);
            break;
        }
        pos_current += (sizeof(ble_service_db) - sizeof(ListEntry));
    }

    free_ctrl_buffer(db_buff);

    Report(("[BleDB] Update record in file, seek offset: %d", pos_current));
//    kal_trace(BT_TRACE_BLE_PROFILES, BLEDB_UPDATERECORD_OFFSET, pos_current);
    btmtk_fs_write(fd, ((U8 *)servicedb) + sizeof(ListEntry), sizeof(ble_service_db) - sizeof(ListEntry));
    btmtk_fs_close(fd);
    
}

static void BleServiceDbRemoveRecord(const BD_ADDR *bd_addr)
{
    S32 fd;
    S32 fd_temp;
    S32 pos_current;
    S32 pos_end;
    U8 *db_buff;
    U8 *buffer;
    
    if (!btmtk_fs_is_dir_exist((const U8*)BLE_SERVICE_DATABASE_FILE))
    {
        return;
    }
    
    fd = btmtk_fs_open((const U8*)BLE_SERVICE_DATABASE_FILE, BTMTK_FS_READ_WRITE);
    if (fd < 0)
    {
        return;
    }

    pos_end = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_END);
    pos_current = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_BEGIN);

    db_buff = (U8 *)get_ctrl_buffer(sizeof(ble_service_db) - sizeof(ListEntry));
    while (pos_current < pos_end)
    {
        btmtk_fs_read(fd, db_buff, sizeof(ble_service_db) - sizeof(ListEntry));
        if (OS_MemCmp(db_buff, 6, bd_addr->addr, 6))
        {
            break;
        }
        pos_current += (sizeof(ble_service_db) - sizeof(ListEntry));
    }
    free_ctrl_buffer(db_buff);

    if (pos_current == pos_end)
    {
        Report(("[BleDB] Remove record, NOT FOUND!"));    
//        kal_trace(BT_TRACE_BLE_PROFILES, BLEDB_REMOVERECORD_NOTFOUND);.
        return;
    }

    /* Case 1:  If there is only one database, delete the file directly */
    if (pos_end - pos_current == sizeof(ble_service_db) - sizeof(ListEntry))
    {
        Report(("[BleDB] Remove record, delete file directly..."));    
    
//        kal_trace(BT_TRACE_BLE_PROFILES, BLEDB_REMOVERECORD_DELETEFILE);
        btmtk_fs_delete((const U8 *)BLE_SERVICE_DATABASE_FILE);
        return;
    }

    /* Case 2:  If there is more than one database, create a new temp file, 
    *   move the left database to the temp file, delete the original database
    *   file, and rename the temp file as the new database file. 
    */
    fd_temp = btmtk_fs_open((const U8*)BLE_SERVICE_DATABASE_TEMP_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
    if (fd_temp < 0)
    {
        return;
    }

    if (pos_current > 0)
    {
        /* Move first half of the database to temp file */
        buffer = (U8 *)get_ctrl_buffer(pos_current);
        btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_BEGIN);
        btmtk_fs_read(fd, buffer, pos_current);
        btmtk_fs_write(fd_temp, buffer, pos_current);
        free_ctrl_buffer(buffer);
        
    }
    pos_current = btmtk_fs_seek(fd, sizeof(ble_service_db) - sizeof(ListEntry), BTMTK_FS_SEEK_CURRENT);

    if (pos_current < pos_end)
    {
        /* Move first half of the database to temp file */
        buffer = (U8 *)get_ctrl_buffer(pos_end - pos_current);
        btmtk_fs_read(fd, buffer, pos_end - pos_current);
        btmtk_fs_write(fd_temp, buffer, pos_end - pos_current);
        free_ctrl_buffer(buffer);
    }

    Report(("[BleDB] pos cur: %d, pos end: %d", pos_current, pos_end));

    btmtk_fs_close(fd);
    btmtk_fs_close(fd_temp);

    btmtk_fs_delete((const U8 *)BLE_SERVICE_DATABASE_FILE);
    btmtk_fs_rename((const U8 *)BLE_SERVICE_DATABASE_TEMP_FILE, (const U8 *)BLE_SERVICE_DATABASE_FILE);
}

ble_service_db *BleServiceDbGetRecord(BD_ADDR *bd_addr)
{
    ble_service_db *servicedb;
    BtStatus bonding_status;
    BtDeviceRecord record;

    bonding_status = SEC_FindDeviceRecord(bd_addr, &record);
    servicedb = BleServiceDbFindRecord(bd_addr);
    if (servicedb != NULL)
    {
        if (bonding_status == BT_STATUS_SUCCESS)
        {
            goto exit;
        }
        else
        {
            /* Here the device is unbonded */
            BleServiceDbRemoveRecord(bd_addr);
            //RemoveEntryList(&servicedb->node);
            OS_MemSet((U8 *)servicedb + sizeof(ListEntry) + sizeof(BD_ADDR), 0, 
                                sizeof(ble_service_db) - sizeof(ListEntry) - sizeof(BD_ADDR));
            goto exit;
        }
    }

    servicedb = (ble_service_db *)get_ctrl_buffer(sizeof(ble_service_db));
    OS_MemSet((U8 *)servicedb, 0, sizeof(ble_service_db));
    servicedb->bdAddr = *bd_addr;
    InsertTailList(&BleDbCtx(servicedbList), (ListEntry *)&servicedb->node);
    if (bonding_status == BT_STATUS_SUCCESS)
    {
        BleServiceDbUpdateRecord(servicedb);
    }
    Report(("[BleDB] new record, bonding status: 0x%x",bonding_status));    
//    kal_trace(BT_TRACE_BLE_PROFILES, BLEDB_GETRECORD_NEW, bonding_status);

exit:
    return servicedb;
}

/*****************************************************************************
 * FUNCTION
 *  BleDbGattEventCallback
 *
 * DESCRIPTION
 *  It is used to handle BT_GATT_APP_EVENT_DISCONNECTED, to check whether it is necessary
 *  to remove unbonded device service db.
 *
 *****************************************************************************/
static void BleServiceDbGattEventCallback(void *handler, BtGattEvent *event)
{
    BtDeviceRecord record;
    ble_service_db *servicedb;

    if ((event->eType == BT_GATT_APP_EVENT_DISCONNECTED) &&
        ((servicedb = BleServiceDbFindRecord(&event->link->bdAddr)) != NULL) &&
        (SEC_FindDeviceRecord(&event->link->bdAddr, &record) != BT_STATUS_SUCCESS)) // unbonded device
    {
        /* Remove the service record in service database list, if it is not a bonded device */
        Report(("[BleDB] Remove unbonded device record from list"));
//        kal_trace(BT_TRACE_BLE_PROFILES, BLEDB_REMOVE_UNBONDEDDEVICE_FROMLIST);
        RemoveEntryList(&servicedb->node);
    }
}

static void BleServiceDbBondingCb(U8 status, const U8 *bd_addr)
{
    ble_service_db *servicedb;
    Report(("[BleDB] bonding callback, status: 0x%x", status));

//    kal_trace(BT_TRACE_BLE_PROFILES, BLEDB_BONDINGCB_STATUS, status);

    switch (status)
    {
        //case ATT_SEC_DEVICE_BOND:
            /* Device Bonded or Bonded again*/
            //servicedb = BleServiceDbGetRecord((BD_ADDR *)bd_addr);
            //BleServiceDbUpdateRecord(servicedb);
            //break;
        case ATT_SEC_DEVICE_UNBOND:
            /* Device Un Bonded */
            BleServiceDbRemoveRecord((BD_ADDR *)bd_addr);
            break;
        case ATT_SEC_DEVICE_ALL_UNBOND:
            /* All Device Un Bonded */
            btmtk_fs_delete((const U8 *)BLE_SERVICE_DATABASE_FILE);
            break;
    }
}


void BleServiceDbInitialize(void)
{
    OS_MemSet((U8 *)&BleServiceDbCtx, 0, sizeof(BleServiceDbCtx));
    InitializeListHead(&BleDbCtx(servicedbList));
}

void BleServiceDbLoadRecord(void)
{
    if (BleDbCtx(loadCount) == 0)

    {
        Report(("[BleDB] Loading record..."));

//        kal_trace(BT_TRACE_BLE_PROFILES, BLEDB_LOADRECORD);

        BT_GattRegisterConnectedEvent(&BleDbCtx(gattHandler), BleServiceDbGattEventCallback);

        BleDbCtx(bondingHandler).callback = BleServiceDbBondingCb;
        ATTDB_RegisterBondStatus(&BleDbCtx(bondingHandler));

        if (btmtk_fs_is_dir_exist((const U8*)BLE_SERVICE_DATABASE_FILE))
        {
            S32 fd;
            S32 pos_current;
            S32 pos_end;
            ble_service_db *db_node;
            
            fd = btmtk_fs_open((const U8*)BLE_SERVICE_DATABASE_FILE, BTMTK_FS_READ_ONLY);
            if (fd < 0)
            {
                // file is not exist
                goto exit;
            }

            pos_end = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_END);
            pos_current = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_BEGIN);

            while (pos_current < pos_end)
            {
                db_node = (ble_service_db *)get_ctrl_buffer(sizeof(ble_service_db));
                OS_MemSet((U8 *)db_node, 0, sizeof(ble_service_db));
                
                btmtk_fs_read(fd, (U8 *)db_node + sizeof(ListEntry), sizeof(ble_service_db) - sizeof(ListEntry));
                pos_current += (sizeof(ble_service_db) - sizeof(ListEntry));

                InsertTailList(&BleDbCtx(servicedbList), &db_node->node);
            }
            btmtk_fs_close(fd);
        }
    }

exit:
    BleDbCtx(loadCount)++;
}

void BleServiceDbUnloadRecord(void)
{
    if (BleDbCtx(loadCount) == 1)
    {
        BT_GattUnregisterConnectedEvent(&BleDbCtx(gattHandler));
        ATTDB_UnRegisterBondStatus(&BleDbCtx(bondingHandler));

        while (!IsListEmpty(&BleDbCtx(servicedbList)))
        {
            free_ctrl_buffer(RemoveHeadList(&BleDbCtx(servicedbList)));
        }
    }
    BleDbCtx(loadCount)--;
}
