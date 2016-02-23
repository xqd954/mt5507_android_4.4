/*****************************************************************************
 *
 * Filename:
 * ---------
 * ble_utils.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK BLE Utility Definition.
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
 * 08 08 2013 liang.cheng
 * [ALPS00931922] [MR2][BT]Source code check in
 * Easy Migration default profile check in.
 *
 * 08 08 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
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
#ifdef __BT_4_0_BLE__

#include <stdlib.h>
#include "attdef.h"
#include "attdb.h"
#include "att.h"
#include "gattcb.h"
#include "gatt.h"
#include "sec.h"

#include "ble_utils.h"
#include "bt_adp_fs.h"
#include "bt_ucs2.h"
#include "btutils.h"

#define BLE_UUID_IS_EQUAL(uuid_union, uuid, size)     \
    (OS_MemCmp((U8 *)((size == 2) ? uuid_union.uuid16 : uuid_union.uuid128),  size, (U8 *)(uuid), \
                                                                                                         (size)) ? TRUE : FALSE)
                                                                                                         

#define BLE_UUID_SET(uuid16, uuid128, uuid, size)       \
    (OS_MemCopy((U8 *)((size == 2) ? uuid16 : uuid128), (uuid), (size)))


#define BLE_REMOTE_SERVICE_DATABASE_FOLDER       "/data/@btmtk/bleRemoteDb"  


/*---------------------------------------------------------------------------
 * g_BleServiceDBTable 
 *
 *     It is a table of BleServiceDB
 */
ListEntry g_BleServiceDBTable;

BtGattHandler g_GattHandler;
BtBleBondCbHandler  g_BondHandler;



extern void ATTDB_RegisterBondStatus(BtBleBondCbHandler *cb_func);

BtStatus BleFindIncludeServices(BlePacket *packet, U16 start_handle, U16 end_handle);

void BleHandleFindPrimaryServicesByUuidCb(void *userdata, BtGattEvent *event);
void BleHandleFindIncludeServicesCb(void *userdata, BtGattEvent *event);
void BleHandleFindAllCharacteristicsCb(void *userdata, BtGattEvent *event);
void BleHandleFindAllCharacteristicDescriptorsCb(void *userdata, BtGattEvent *event);
void BleHandleReadCharacteristicCb(void *userdata, BtGattEvent *event);
void BleHandleReadLongCharacteristicCb(void *userdata, BtGattEvent *event);
void BleHandleReadCharacteristicDescriptorCb(void *userdata, BtGattEvent *event);
void BleHandleReadLongCharacteristicDescriptorCb(void *userdata, BtGattEvent *event);

#define BLE_VALID_UUID_LEN(len) (len == 2 || len == 16)

extern const U8  bluetooth_base_uuid[16];

static void BleConvertUuid16toUuid128(U8 *uuid128, const U8 *uuid16){
    if(uuid16 && uuid128){
        OS_MemCopy(uuid128, bluetooth_base_uuid, 16);
        /*
        uuid128[2] = uuid16[1];
        uuid128[3] = uuid16[0];
        */
        uuid128[13] = uuid16[1];
        uuid128[12] = uuid16[0];
    }
}

U8 BLE_CompareUuid(const U8 *uuid1, U8 len1, const U8 *uuid2, U8 len2){
    U8  uuid128[16];
    const U8 *p128 = NULL;
    const U8 *p16 = NULL;

    if(!uuid1 || !uuid2){
        OS_Report("BLE_CompareUuid : NULL uuid, uuid1=%p, uuid2=%p", uuid1, uuid2);
        return 0;
    }

    if(!BLE_VALID_UUID_LEN(len1) || !BLE_VALID_UUID_LEN(len2)){
        OS_Report("BLE_CompareUuid : invalid uuid len, len1=%u, len2=%u", len1, len2);
        return 0;
    }

    if(len1 == len2){
        return OS_MemCmp(uuid1, len1, uuid2, len2);
    }else if(len1 == 2){
        p16 = uuid1;
        p128 = uuid2;
    }else{
        p16 = uuid2;
        p128 = uuid1;
    }
    BleConvertUuid16toUuid128(uuid128, p16);
    return OS_MemCmp(p128, 16, uuid128, 16);

}

BleServiceDB *BleNewServiceDB(const BD_ADDR *bd_addr)
{
    BleServiceDB *servicedb;

    servicedb = (BleServiceDB *)BLE_UTILS_MALLOC(sizeof(BleServiceDB));
    if (servicedb == NULL)
    {
        return NULL;
    }
    
    OS_MemSet((U8 *)servicedb, 0, sizeof(BleServiceDB));
    servicedb->bt_addr = *bd_addr;

    /* Initialize List */
    InitializeListHead(&servicedb->service_list);

    return servicedb;

}


static BleServiceNode *BleNewService(U8 uuid_size, U8 *uuid, U16 startHandle, U16 endHandle)
{
    BleServiceNode *service_node = NULL; 

    service_node = (BleServiceNode *)BLE_UTILS_MALLOC(sizeof(BleServiceNode));
    if (service_node)
    {
        OS_MemSet((U8 *)service_node, 0, sizeof(BleServiceNode));
        InitializeListHead(&service_node->include_list);
        InitializeListHead(&service_node->char_list);
        
        service_node->service.start_handle = startHandle;
        service_node->service.end_handle = endHandle;
        service_node->service.uuid_size = uuid_size;
        BLE_UUID_SET(service_node->service.uuid.uuid16, service_node->service.uuid.uuid128, uuid, service_node->service.uuid_size);
    }
    return service_node;
}

static void BleFreeService(BleServiceNode *service_node)
{
    BleIncludeServiceNode *include_node;
    BleCharNode *char_node; 
    BleCharDescNode *char_desc_node;

    while (!IsListEmpty(&service_node->include_list))
    {
        include_node = (BleIncludeServiceNode *)RemoveHeadList(&service_node->include_list);
        BLE_UTILS_FREE(include_node);
    }

    while (!IsListEmpty(&service_node->char_list))
    {
        char_node = (BleCharNode *)RemoveHeadList(&service_node->char_list);
        while (!IsListEmpty(&char_node->char_desc_list))
        {
            char_desc_node = (BleCharDescNode *)RemoveHeadList(&char_node->char_desc_list);
            BLE_UTILS_FREE(char_desc_node->descriptor.desc);
            BLE_UTILS_FREE(char_desc_node);
        }
        BLE_UTILS_FREE(char_node->characteristic.value);         
        BLE_UTILS_FREE(char_node);
    }

    BLE_UTILS_FREE(service_node);
}


static void BleFreeServiceDB(BleServiceDB *servicedb)
{
    BleServiceNode *service_node;

    while (!IsListEmpty(&servicedb->service_list))
    {
        service_node = (BleServiceNode *)RemoveHeadList(&servicedb->service_list);
        BleFreeService(service_node);
    }

    BLE_UTILS_FREE(servicedb);	
}

static void BleConvertBdAddr2FileName(const BD_ADDR *bd_addr, S8 **file_name)
{
    S8 bt_addr_string[18];

    OS_MemSet((U8 *)bt_addr_string, 0, 18);
    sprintf(bt_addr_string, "%02X_%02X_%02X_%02X_%02X_%02X",
            bd_addr->addr[0],
            bd_addr->addr[1],
            bd_addr->addr[2],
            bd_addr->addr[3],
            bd_addr->addr[4],
            bd_addr->addr[5]);

    *file_name = (S8 *)BLE_UTILS_MALLOC(OS_StrLen(BLE_REMOTE_SERVICE_DATABASE_FOLDER) + 19);
    OS_MemSet((U8 *)*file_name, 0, OS_StrLen(BLE_REMOTE_SERVICE_DATABASE_FOLDER) + 19);
    OS_StrCpy(*file_name, BLE_REMOTE_SERVICE_DATABASE_FOLDER);
    OS_StrCat(*file_name, "/");
    OS_StrCat(*file_name, bt_addr_string);

    Report(("[BleUtils] filename: %s", *file_name));

}


/**********************************************************************
service database        // database name is bt address: XX_XX_XX_XX_XX_XX
{
	service
	{
		service_size			// 2 bytes, , not include itself
		start_handle		// 2 bytes
		end_handle		// 2 bytes
		uuid_size		// 1 byte
		uuid			// 2 bytes or 16 bytes
		
		include_size		// 2 bytes, , not include itself
		
		include service	//present only if include_size is more than 0
		{
			include_service_handle		// 2 bytes
			end_group_handle		// 2 bytes
			uuid_size			// 1 byte
			uuid				// 2 bytes or 16 bytes
		}
		
		characteristic
		{
			char_size			// 2 bytes, not include itself
			handle			// 2 bytes
			properties		// 1 bytes
			value_handle		// 2 bytes
			value_uuid_size		// 1 byte
			value_uuid		// 2 bytes or 16 bytes
			value_size		// 1 byte
			value			// value_size
			
			descriptor
			{
				handle			//2 bytes
				desc_uuid_size		//1 byte
				desc_uuid		//2 bytes or 16 bytes
				desc_size		//1 byte
				desc			//desc_size
			}
		}
	}
}
***********************************************************************/

static BtStatus BleReadServiceDB(BD_ADDR *bd_addr, BleServiceDB *servicedb)
{
    S32 fd;
    BleServiceNode *service_node;
    BleIncludeServiceNode *include_node;
    BleCharNode *char_node;
    BleCharDescNode *char_desc_node;
    U8 *uuid;
    U32 ret;
    S8 *file_name;
    U16 service_size;
    U16 include_size;
    U16 char_size;
    S32 pos_cur;
    S32 pos_file_end;


    BleConvertBdAddr2FileName(bd_addr, &file_name);

    if (!btmtk_fs_is_dir_exist((const U8*)file_name))
    {
        BLE_UTILS_FREE(file_name);
        return BT_STATUS_FAILED;
    }

    fd = btmtk_fs_open((const U8*)file_name, BTMTK_FS_READ_ONLY);
    BLE_UTILS_FREE(file_name);

    if (fd < 0)
    {
        return BT_STATUS_FAILED;
    }

    pos_file_end = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_END);
    pos_cur = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_BEGIN);

    /* Seach database file util the end of the file */
    while (pos_cur != pos_file_end)
    {
        /************* service ************/

        btmtk_fs_read(fd, (U8 *)&service_size, 2);

        service_node = (BleServiceNode *)BLE_UTILS_MALLOC(sizeof(BleServiceNode));
        OS_MemSet((U8 *)service_node, 0, sizeof(BleServiceNode));
        InitializeListHead(&service_node->include_list);
        InitializeListHead(&service_node->char_list);

        btmtk_fs_read(fd, (U8 *)&service_node->service.start_handle, 2);
        btmtk_fs_read(fd, (U8 *)&service_node->service.end_handle, 2);
        btmtk_fs_read(fd, (U8 *)&service_node->service.uuid_size, 1);
        
        uuid = (service_node->service.uuid_size == 2) ? service_node->service.uuid.uuid16 : service_node->service.uuid.uuid128;
        btmtk_fs_read(fd, uuid, service_node->service.uuid_size);


        Report(("[BleUtils] read database, start hd: %d, end hd: %d", service_node->service.start_handle,
                            service_node->service.end_handle));
        Report(("[BleUtils] read database, uuid: %x, %x", service_node->service.uuid.uuid16[0],
                            service_node->service.uuid.uuid16[1]));


        /************* include service ************/
        
        btmtk_fs_read(fd, (U8 *)&include_size, 2);

        service_size -= ((5 + service_node->service.uuid_size) + (2 + include_size));
        
        while (include_size > 0)
        {
            include_node = (BleIncludeServiceNode *)BLE_UTILS_MALLOC(sizeof(BleIncludeServiceNode));
            OS_MemSet((U8 *)include_node, 0, sizeof(BleIncludeServiceNode));

            btmtk_fs_read(fd, (U8 *)&include_node->include_service.include_service_handle, 2);
            btmtk_fs_read(fd, (U8 *)&include_node->include_service.end_group_handle, 2);
            btmtk_fs_read(fd, (U8 *)&include_node->include_service.uuid_size, 1);
            
            uuid = (include_node->include_service.uuid_size == 2) ? 
                            include_node->include_service.uuid.uuid16 : include_node->include_service.uuid.uuid128;
            btmtk_fs_read(fd, uuid, include_node->include_service.uuid_size);

            include_size -= (5 + include_node->include_service.uuid_size);

            InsertTailList(&service_node->include_list, &include_node->node);
        }

        while (service_size > 0)
        {
            /************* characteristic ************/

            btmtk_fs_read(fd, (U8 *)&char_size, 2);
            service_size -= (char_size + 2);    // 2 = char_size itself 

            char_node = (BleCharNode *)BLE_UTILS_MALLOC(sizeof(BleCharNode));
            OS_MemSet((U8 *)char_node, 0, sizeof(BleCharNode));
            InitializeListHead(&char_node->char_desc_list);

            btmtk_fs_read(fd, (U8 *)&char_node->characteristic.handle, 2);
            btmtk_fs_read(fd, (U8 *)&char_node->characteristic.properties, 1);
            btmtk_fs_read(fd, (U8 *)&char_node->characteristic.value_handle, 2);
            btmtk_fs_read(fd, (U8 *)&char_node->characteristic.value_uuid_size, 1);

            uuid = (char_node->characteristic.value_uuid_size == 2) ? 
                            char_node->characteristic.value_uuid.uuid16 : char_node->characteristic.value_uuid.uuid128;
            btmtk_fs_read(fd, uuid, char_node->characteristic.value_uuid_size);

            btmtk_fs_read(fd, (U8 *)&char_node->characteristic.value_size, 1);

            char_size -= (7 + char_node->characteristic.value_uuid_size + char_node->characteristic.value_size);

            if (char_node->characteristic.value_size > 0)
            {
                char_node->characteristic.value = (U8 *)BLE_UTILS_MALLOC(char_node->characteristic.value_size);
                btmtk_fs_read(fd, char_node->characteristic.value, char_node->characteristic.value_size);
            }

            Report(("[BleUtils] read database , handle: %d, value handle: %d", char_node->characteristic.handle,
                char_node->characteristic.value_handle));

            Report(("[BleUtils] value size: %d", char_node->characteristic.value_size));

            while (char_size > 0)
            {
                /************* characteristic descriptor ************/

                char_desc_node = (BleCharDescNode *)BLE_UTILS_MALLOC(sizeof(BleCharDescNode));
                OS_MemSet((U8 *)char_desc_node, 0, sizeof(BleCharDescNode));

                btmtk_fs_read(fd, (U8 *)&char_desc_node->descriptor.handle, 2);
                btmtk_fs_read(fd, (U8 *)&char_desc_node->descriptor.desc_uuid_size, 1);

                uuid = (char_desc_node->descriptor.desc_uuid_size == 2) ? 
                                char_desc_node->descriptor.desc_uuid.uuid16 : char_desc_node->descriptor.desc_uuid.uuid128;
                btmtk_fs_read(fd, uuid, char_desc_node->descriptor.desc_uuid_size);

                btmtk_fs_read(fd, (U8 *)&char_desc_node->descriptor.desc_size, 1);

                char_size -= (4 + char_desc_node->descriptor.desc_uuid_size + char_desc_node->descriptor.desc_size);
                
                if (char_desc_node->descriptor.desc_size > 0)
                {
                    char_desc_node->descriptor.desc = (U8 *)BLE_UTILS_MALLOC(char_desc_node->descriptor.desc_size);
                    btmtk_fs_read(fd, char_desc_node->descriptor.desc, char_desc_node->descriptor.desc_size);
                }

                Report(("[BleUtils] read database, desciptor handle: %d", char_desc_node->descriptor.handle));
                Report(("[BleUtils] descriptor value size: %d", char_desc_node->descriptor.desc_size));

                InsertTailList(&char_node->char_desc_list, &char_desc_node->node);
            }

            InsertTailList(&service_node->char_list, &char_node->node);

        }

        pos_cur = btmtk_fs_seek(fd, 0, BTMTK_FS_SEEK_CURRENT);

        InsertTailList(&servicedb->service_list, &service_node->node);

        Report(("[BleUtils] pos cur: %d", pos_cur));
        
    }

    btmtk_fs_close(fd);
    return BT_STATUS_SUCCESS;
}

static void BleWriteServiceDB(BleServiceDB *servicedb)
{
    S32 fd;
    BleServiceNode *service_node;
    BleIncludeServiceNode *include_node;
    BleCharNode *char_node;
    BleCharDescNode *char_desc_node;
    U8 *service_buffer;
    U8 *char_buffer;
    U8 *desc_buffer;
    S8 *file_name;
    S32 ret;
    U8 *uuid;
    U16 service_size;
    U16 include_size;
    U16 char_size;
    U16 desc_size;
    U8 *ptr;

    
    BleConvertBdAddr2FileName(&servicedb->bt_addr, &file_name);

    if (!btmtk_fs_is_dir_exist((const U8*)BLE_REMOTE_SERVICE_DATABASE_FOLDER))
    {
        ret = btmtk_fs_create_dir((const U8*)BLE_REMOTE_SERVICE_DATABASE_FOLDER);
        if (ret != BTMTK_FS_OK)
        {
            BLE_UTILS_FREE(file_name);
            return;
        }
    }
    else
    {
        if (btmtk_fs_is_dir_exist((const U8*)file_name))
        {
            // rewrite all service record anyway
            btmtk_fs_delete((const U8*)file_name);
        }
    }

    fd = btmtk_fs_open((const U8*)file_name, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
    BLE_UTILS_FREE(file_name);
    
    if (fd < 0)
    {
        return;
    }


    /************* service ************/
   
    service_node = (BleServiceNode *)GetHeadList(&servicedb->service_list);
    while ((ListEntry *)service_node != &servicedb->service_list)
    {
        // start_handle + end_handle + uuid_size + uuid
        service_size = (5 + service_node->service.uuid_size);  
        service_buffer = (U8 *)BLE_UTILS_MALLOC(service_size);

        OS_MemCopy(service_buffer, (U8 *)&service_node->service.start_handle, 2);
        OS_MemCopy(service_buffer + 2, (U8 *)&service_node->service.end_handle, 2);
        OS_MemCopy(service_buffer + 4, (U8 *)&service_node->service.uuid_size, 1);
        
        uuid = (service_node->service.uuid_size == 2) ? service_node->service.uuid.uuid16 : service_node->service.uuid.uuid128;
        OS_MemCopy(service_buffer + 5, uuid, service_node->service.uuid_size);
        
        btmtk_fs_seek(fd, 2, BTMTK_FS_SEEK_CURRENT);        // skip service total size here
        btmtk_fs_write(fd, service_buffer, service_size);

        
        Report(("[BleUtils] write database , service start hd: %d, end hd: %d", service_node->service.start_handle,
            service_node->service.end_handle));

        /************* include service ************/
        
        include_size = 0;
        service_size += 2;
        btmtk_fs_seek(fd, 2, BTMTK_FS_SEEK_CURRENT);        // skip include service total size here

        include_node = (BleIncludeServiceNode *)GetHeadList(&service_node->include_list);
        while ((ListEntry *)include_node != &service_node->include_list)
        {
            // include_service_handle + end_group_handle + uuid_size + uuid
            include_size += (5 + include_node->include_service.uuid_size);  

            OS_MemCopy(service_buffer, (U8 *)&include_node->include_service.include_service_handle, 2);
            OS_MemCopy(service_buffer + 2, (U8 *)&include_node->include_service.end_group_handle, 2);
            OS_MemCopy(service_buffer + 4, (U8 *)&include_node->include_service.uuid_size, 1);


            uuid = (include_node->include_service.uuid_size == 2) ? 
                            include_node->include_service.uuid.uuid16 : include_node->include_service.uuid.uuid128;
            OS_MemCopy(service_buffer + 5, uuid, include_node->include_service.uuid_size);

            btmtk_fs_write(fd, service_buffer, include_size);

            service_size += include_size;
            include_node = (BleIncludeServiceNode *)GetNextNode(&include_node->node);
        }

        btmtk_fs_seek(fd, -(include_size + 2), BTMTK_FS_SEEK_CURRENT);
        btmtk_fs_write(fd, (U8 *)&include_size, 2);
        btmtk_fs_seek(fd, include_size, BTMTK_FS_SEEK_CURRENT);

        BLE_UTILS_FREE(service_buffer);

        /************* characteristic ************/

        char_node = (BleCharNode *)GetHeadList(&service_node->char_list);
        while ((ListEntry *)char_node != &service_node->char_list)
        {
            btmtk_fs_seek(fd, 2, BTMTK_FS_SEEK_CURRENT);        // skip characteristic total size here

            // handle + properties + value_handle + value_uuid_size + value_uuid + value_size + value(O)
            char_size = 7 + char_node->characteristic.value_uuid_size + char_node->characteristic.value_size;
            char_buffer = (U8 *)BLE_UTILS_MALLOC(char_size);
            ptr = char_buffer;

            OS_MemCopy(ptr, (U8 *)&char_node->characteristic.handle, 2);
            ptr += 2;
            OS_MemCopy(ptr, (U8 *)&char_node->characteristic.properties, 1);
            ptr += 1;
            OS_MemCopy(ptr, (U8 *)&char_node->characteristic.value_handle, 2);
            ptr += 2;
            OS_MemCopy(ptr, (U8 *)&char_node->characteristic.value_uuid_size, 1);
            ptr += 1;

            uuid = (char_node->characteristic.value_uuid_size == 2) ? 
                            char_node->characteristic.value_uuid.uuid16 : char_node->characteristic.value_uuid.uuid128;

            OS_MemCopy(ptr, uuid, char_node->characteristic.value_uuid_size);
            ptr += char_node->characteristic.value_uuid_size;

            Report(("[BleUtils] write database , handle: %d, value handle: %d", char_node->characteristic.handle,
                char_node->characteristic.value_handle));

            OS_MemCopy(ptr, (U8 *)&char_node->characteristic.value_size, 1);
            ptr += 1;
            if (char_node->characteristic.value_size > 0)
            {
                OS_MemCopy(ptr, char_node->characteristic.value, char_node->characteristic.value_size);
            }

            btmtk_fs_write(fd, char_buffer, char_size);
            BLE_UTILS_FREE(char_buffer);

            Report(("[BleUtils] write database, value size: %d", char_node->characteristic.value_size));
            Report(("[BleUtils] write database, char_size: %d", char_size));

            /************* characteristic descriptor ************/

            char_desc_node = (BleCharDescNode *)GetHeadList(&char_node->char_desc_list);
            while ((ListEntry *)char_desc_node != &char_node->char_desc_list)
            {
                // handle + uuid_size + uuid + desc_size + desc(O)
                desc_size = 4 + char_desc_node->descriptor.desc_uuid_size + char_desc_node->descriptor.desc_size;
                desc_buffer = (U8 *)BLE_UTILS_MALLOC(desc_size);
                ptr = desc_buffer;

                OS_MemCopy(ptr, (U8 *)&char_desc_node->descriptor.handle, 2);
                ptr += 2;
                OS_MemCopy(ptr, (U8 *)&char_desc_node->descriptor.desc_uuid_size, 1);
                ptr += 1;
                
                uuid = (char_desc_node->descriptor.desc_uuid_size == 2) ? 
                                char_desc_node->descriptor.desc_uuid.uuid16 : char_desc_node->descriptor.desc_uuid.uuid128;
                OS_MemCopy(ptr, uuid, char_desc_node->descriptor.desc_uuid_size);
                ptr += char_desc_node->descriptor.desc_uuid_size;
                
                OS_MemCopy(ptr, (U8 *)&char_desc_node->descriptor.desc_size, 1);
                ptr += 1;
                if (char_desc_node->descriptor.desc_size > 0)
                {
                    OS_MemCopy(ptr, char_desc_node->descriptor.desc, char_desc_node->descriptor.desc_size);
                }

                btmtk_fs_write(fd, desc_buffer, desc_size);
                BLE_UTILS_FREE(desc_buffer);
                
                char_size += desc_size;

                Report(("[BleUtils] write database, desciptor handle: %d", char_desc_node->descriptor.handle));
                Report(("[BleUtils] write database, desciptor value size: %d", char_desc_node->descriptor.desc_size));
                
                char_desc_node = (BleCharDescNode *)GetNextNode(&char_desc_node->node);

                Report(("[BleUtils] write database, char_size: %d, desc_size: %d", char_size, desc_size));
            }

            
            service_size += (char_size + 2);

            btmtk_fs_seek(fd, -(char_size + 2), BTMTK_FS_SEEK_CURRENT);
            btmtk_fs_write(fd, (U8 *)&char_size, 2);
            btmtk_fs_seek(fd, char_size, BTMTK_FS_SEEK_CURRENT);
            
            char_node = (BleCharNode *)GetNextNode(&char_node->node);

            Report(("[BleUtils] write database, service_size: %d", service_size));
        }

        btmtk_fs_seek(fd, -(service_size + 2), BTMTK_FS_SEEK_CURRENT);
        btmtk_fs_write(fd, (U8 *)&service_size, 2);
        btmtk_fs_seek(fd, service_size, BTMTK_FS_SEEK_CURRENT);
        
        service_node = (BleServiceNode *)GetNextNode(&service_node->node);
    }

    btmtk_fs_close(fd);

}

static void BleDeleteServiceDB(const BD_ADDR *bd_addr)
{
    S8 *file_name;
    
    BleConvertBdAddr2FileName((BD_ADDR *)bd_addr, &file_name);
    btmtk_fs_delete((const U8 *)file_name);
    BLE_UTILS_FREE(file_name);
}


BleServiceDB *BleFindServiceDBbyAddr(const BD_ADDR *bd_addr)
{
    BleServiceDB *servicedb;

    servicedb = (BleServiceDB *)GetHeadList(&g_BleServiceDBTable);
    while ((ListEntry *)servicedb != &g_BleServiceDBTable)
    {
        if (BLE_DEV_ADDR_IS_EQUAL(&servicedb->bt_addr, bd_addr))
        {
            return servicedb;
        }
        servicedb = (BleServiceDB *)GetNextNode(&servicedb->node);
    }
    return NULL;
}

static BleServiceNode *BleFindServicebyUuid(BleServiceDB *servicedb, U8 uuid_size, U8 *uuid)
{
    BleServiceNode *service_node;

    service_node = (BleServiceNode *)GetHeadList(&servicedb->service_list);
    while ((ListEntry *)service_node != &servicedb->service_list)
    {
        //if ((service_node->service.uuid_size == uuid_size) &&
        //    BLE_UUID_IS_EQUAL(service_node->service.uuid, uuid, uuid_size))
        if(BLE_CompareUuid(uuid, uuid_size, service_node->service.uuid.uuid128, service_node->service.uuid_size))
        {
            return service_node;
        }
        /* try the next node in the list */
        service_node = (BleServiceNode *)GetNextNode(&service_node->node);		
    }
    return NULL;
}

BleServiceNode *BleFindServicebyHandleRange(ListEntry *service_list, U16 handle)
{
    BleServiceNode *service_node;

    service_node = (BleServiceNode *)GetHeadList(service_list);
    while ((ListEntry *)service_node != service_list)
    {
        if ((handle >= service_node->service.start_handle) &&
            (handle <= service_node->service.end_handle))
        {
            return service_node;
        }
        /* try the next node in the list */
        service_node = (BleServiceNode *)GetNextNode(&service_node->node);		
    }
    return NULL;
}

static BleIncludeServiceNode *BleFindIncludebyUuid(BleServiceNode *service_node, U8 uuid_size, U8 *uuid)
{
    BleIncludeServiceNode *include_node;

    include_node = (BleIncludeServiceNode *)GetHeadList(&service_node->include_list);
    while ((ListEntry *)include_node != &service_node->include_list)
    {
        //if ((include_node->include_service.uuid_size == uuid_size) &&
        //    BLE_UUID_IS_EQUAL(include_node->include_service.uuid, uuid, uuid_size))
        if(BLE_CompareUuid(uuid, uuid_size, include_node->include_service.uuid.uuid128, include_node->include_service.uuid_size))
        {
            return include_node;
        }
        /* try the next node in the list */
        include_node = (BleIncludeServiceNode *)GetNextNode(&include_node->node);		
    }
    return NULL;
}

static BleCharNode *BleFindCharbyUuid(BleServiceNode *service_node, U8 uuid_size, U8 *uuid)
{
    BleCharNode *char_node;

    char_node = (BleCharNode *)GetHeadList(&service_node->char_list);
    while ((ListEntry *)char_node != &service_node->char_list)
    {
        //if ((char_node->characteristic.value_uuid_size == uuid_size) &&
        //    BLE_UUID_IS_EQUAL(char_node->characteristic.value_uuid, uuid, uuid_size))
        if(BLE_CompareUuid(uuid, uuid_size, char_node->characteristic.value_uuid.uuid128, char_node->characteristic.value_uuid_size))
        {
            return char_node;
        }
        /* try the next node in the list */
        char_node = (BleCharNode *)GetNextNode(&char_node->node);		
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
BleCharNode *BleFindCharbyHandle(BleServiceNode *service_node, U16 value_handle)
{
    BleCharNode *char_node;

    char_node = (BleCharNode *)GetHeadList(&service_node->char_list);
    while ((ListEntry *)char_node != &service_node->char_list)
    {
        if (char_node->characteristic.value_handle == value_handle)
        {
            return char_node;
        }
        /* try the next node in the list */
        char_node = (BleCharNode *)GetNextNode(&char_node->node);		
    }
    return NULL;
}


static BleCharNode *BleFindCharbyHandleRange(BleServiceNode *service_node, U16 handle)
{
    BleCharNode *char_node;
    BleCharNode *char_next_node;

    char_node = (BleCharNode *)GetHeadList(&service_node->char_list);
    while ((ListEntry *)char_node != &service_node->char_list)
    {
        char_next_node = (BleCharNode *)GetNextNode(&char_node->node);		

        if (((ListEntry *)char_next_node == &service_node->char_list) &&
            (handle > char_node->characteristic.handle) &&
            (handle <= service_node->service.end_handle))
        {
            return char_node;                
        }
        
        if ((handle > char_node->characteristic.handle) &&
              (handle < char_next_node->characteristic.handle))
        {
            return char_node;
        }
        /* try the next node in the list */
        char_node = char_next_node;		
    }
    return NULL;
}

static BleCharDescNode *BleFindCharDescbyUuid(BleCharNode *char_node, U8 uuid_size, U8 *uuid)
{
    BleCharDescNode *desc_node;

    desc_node = (BleCharDescNode *)GetHeadList(&char_node->char_desc_list);
    while ((ListEntry *)desc_node != &char_node->char_desc_list)
    {
        //if ((desc_node->descriptor.desc_uuid_size == uuid_size) &&
        //    BLE_UUID_IS_EQUAL(desc_node->descriptor.desc_uuid, uuid, uuid_size))
        if(BLE_CompareUuid(uuid, uuid_size, desc_node->descriptor.desc_uuid.uuid128, desc_node->descriptor.desc_uuid_size))
        {
            return desc_node;
        }
        /* try the next node in the list */
        desc_node = (BleCharDescNode *)GetNextNode(&desc_node->node);		
    }
    return NULL;
}

static BleCharDescNode *BleFindCharDescbyHandle(BleCharNode *char_node, U16 handle)
{
    BleCharDescNode *desc_node;

    desc_node = (BleCharDescNode *)GetHeadList(&char_node->char_desc_list);
    while ((ListEntry *)desc_node != &char_node->char_desc_list)
    {
        if (desc_node->descriptor.handle == handle)
        {
            return desc_node;
        }
        /* try the next node in the list */
        desc_node = (BleCharDescNode *)GetNextNode(&desc_node->node);		
    }
    return NULL;
}

void BleGattEventCallback(void *handler, BtGattEvent *event)
{
    if (event->eType == BT_GATT_APP_EVENT_DISCONNECTED)
    {
        BleServiceDB *servicedb;
        servicedb = BleFindServiceDBbyAddr(&event->link->bdAddr);
        if (servicedb != NULL)
        {
            Report(("[BleUtils] Remove service database from table!"));
            RemoveEntryList(&servicedb->node);
            BleFreeServiceDB(servicedb);
        }
    }
}

void BleBondingCallback(U8 type, const U8 *bd_addr)
{
    switch (type)
    {
        case ATT_SEC_DEVICE_BOND:
            /* Device Bonded or Bonded again*/
            break;
        case ATT_SEC_DEVICE_UNBOND:
            /* Device Un Bonded */
            BleDeleteServiceDB((BD_ADDR *)bd_addr);
            break;
        case ATT_SEC_DEVICE_ALL_UNBOND:
            /* All Device Un Bonded */
            btmtk_fs_delete((const U8 *)BLE_REMOTE_SERVICE_DATABASE_FOLDER);
            break;
    }
}



void BleQueryServiceDefinitionCallback(BlePacket *packet, BtStatus status)
{
    BleServiceDB *servicedb;
    BleServiceNode *service_node;
    BtStatus ret;

    servicedb = BleFindServiceDBbyAddr(&packet->link->bdAddr);
    Report(("[BleUtils] query callback, status: %d", status));

    if (status == BT_STATUS_SUCCESS)
    {
        service_node = (BleServiceNode *)RemoveHeadList(&packet->service_list);
        InsertTailList(&servicedb->service_list, &service_node->node);
        
        // multiple instance services shall be queried continually
        if (!IsListEmpty(&packet->service_list))
        {
#ifdef __BT_HOGP_PTS_TEST__        
            U32 value = 0;
            OS_GetSystemPropertyUint("hogpptstest", &value, 1);             
            if (value)
            {
                sleep(1000);  
            }  
#endif            
            service_node = (BleServiceNode *)GetHeadList(&packet->service_list);
            Report(("[BleUtils] query callback, more than one instance service, start handle: %d, end handle: %d", 
                    service_node->service.start_handle, service_node->service.end_handle));
            ret = BleFindIncludeServices(packet, service_node->service.start_handle, service_node->service.end_handle);
            if (ret == BT_STATUS_PENDING)
            {
                return;
            }
            else
            {
                /* If there is any error happened, remove the current primary service query result */
                while (!IsListEmpty(&packet->service_list))
                {
                    service_node = (BleServiceNode *)RemoveHeadList(&packet->service_list);
                    BleFreeService(service_node);
                }
            }
        }
        
        // BleWriteServiceDB(servicedb);
    }
    else
    {
        if (servicedb != NULL)
        {
            /* If there is any error happened, remove the current primary service query result */
            while (!IsListEmpty(&packet->service_list))
            {
                service_node = (BleServiceNode *)RemoveHeadList(&packet->service_list);
                BleFreeService(service_node);
            }
        }
    }

    if (packet->cb != NULL)
    {
        packet->cb(packet->context, status);
    }
    BLE_UTILS_FREE(packet);
}


BtStatus BleFindPrimaryServicesByUuid(BlePacket *packet,
             U8 uuid_size,
             U8 *uuid)
{
    BtGattOp *op;
    BtGattDiscoveryPrimaryServiceByUUID *parm;
    BtStatus status;

    op = &packet->gatt_op;
    OS_MemSet((U8 *)op, 0, sizeof(BtGattOp));

    InitializeListEntry(&op->op.node);	
    op->apCb = BleHandleFindPrimaryServicesByUuidCb;

    parm = &packet->parm.primaryServiceByUUID;
    parm->startHandle = 0x0001;
    parm->endHandle = 0xffff;
    parm->type = uuid_size; 	
    BLE_UUID_SET(parm->uuid16, parm->uuid128, uuid, uuid_size);
    op->parms.p.primaryServiceByUUID = parm;
	
    return BT_GATT_DiscoveryPrimaryServiceByUUID(packet->link, op);
}

BtStatus BleFindIncludeServices(BlePacket *packet,
             U16 start_handle,
             U16 end_handle)
{
    BtGattOp *op;
    BtGattFindIncludeServices *parm;
    BtStatus status;

    op = &packet->gatt_op;
    OS_MemSet((U8 *)op, 0, sizeof(BtGattOp));

    InitializeListEntry(&op->op.node);	
    op->apCb = BleHandleFindIncludeServicesCb;

    parm = &packet->parm.findIncludeServices;
    parm->startHandle = start_handle;
    parm->endHandle = end_handle;
    op->parms.p.findIncludeServices = parm;
	
    return BT_GATT_FindIncludeService(packet->link, op);

}
 
BtStatus BleFindAllCharacteristics(BlePacket *packet,
             U16 start_handle,
             U16 end_handle)
{
    BtGattOp *op;
    BtGattDiscoveryAllCharacteristicOfService *parm;
    BtStatus status;
    
    op = &packet->gatt_op;
    OS_MemSet((U8 *)op, 0, sizeof(BtGattOp));
    
    InitializeListEntry(&op->op.node);	
    op->apCb = BleHandleFindAllCharacteristicsCb;
    
    parm = &packet->parm.allCharOfServices;
    parm->startHandle = start_handle;
    parm->endHandle = end_handle;
    op->parms.p.allCharOfServices = parm;
    
    return BT_GATT_DiscoveryAllCharacteristicOfService(packet->link, op);
}

BtStatus BleFindAllCharacteristicDescriptors(BlePacket *packet,
             U16 start_handle,
             U16 end_handle)
{
    BtGattOp *op;
    BtGattDiscoveryAllCharacteristicDescriptors *parm;
    BtStatus status;

    kal_trace(BT_TRACE_BLE_PROFILES, BLEUTILS_FIND_ALLCHARDESC_INFO, start_handle, end_handle);
	
    op = &packet->gatt_op;
    OS_MemSet((U8 *)op, 0, sizeof(BtGattOp));
    
    InitializeListEntry(&op->op.node);	
    op->apCb = BleHandleFindAllCharacteristicDescriptorsCb;
    
    parm = &packet->parm.allCharDescriptors;
    parm->startHandle = start_handle;
    parm->endHandle = end_handle;
    op->parms.p.allCharDescriptors = parm;
    
    return BT_GATT_DiscoveryAllCharacteristicDescriptors(packet->link, op);
}

BtStatus BleReadCharacteristic(BlePacket *packet,
             U16 handle)
{
    BtGattOp *op;
    BtGattReadCharacteristicValue *parm;
    BtStatus status;
    
    kal_trace(BT_TRACE_BLE_PROFILES, BLEUTILS_READ_CHAR_HANDLE, handle);

    op = &packet->gatt_op;
    OS_MemSet((U8 *)op, 0, sizeof(BtGattOp));
    
    InitializeListEntry(&op->op.node);	
    op->apCb = BleHandleReadCharacteristicCb;
    
    parm = &packet->parm.readCharValue;
    parm->handle = handle;
    op->parms.p.readCharValue = parm;
    
    return BT_GATT_ReadCharacteristicValue(packet->link, op);
}

BtStatus BleReadLongCharacteristic(BlePacket *packet,
             U16 handle,
             U16 offset)
{
    BtGattOp *op;
    BtGattReadLongCharacteristicValue *parm;
    BtStatus status;
    
    kal_trace(BT_TRACE_BLE_PROFILES, BLEUTILS_READ_LONG_CHAR_HANDLE, handle, offset);

    op = &packet->gatt_op;
    OS_MemSet((U8 *)op, 0, sizeof(BtGattOp));
    
    InitializeListEntry(&op->op.node);	
    op->apCb = BleHandleReadLongCharacteristicCb;
    
    parm = &packet->parm.readLongCharValue;
    parm->handle = handle;
    parm->offset = offset;
    op->parms.p.readLongCharValue = parm;
    
    return BT_GATT_ReadLongCharacteristicValues(packet->link, op);
}


BtStatus BleReadCharacteristicDescriptor(BlePacket *packet,
             U16 handle)
{
    BtGattOp *op;
    BtGattReadCharacteristicDescriptors *parm;
    BtStatus status;
    
    kal_trace(BT_TRACE_BLE_PROFILES, BLEUTILS_READ_CHARDESC_HANDLE, handle);

    op = &packet->gatt_op;
    OS_MemSet((U8 *)op, 0, sizeof(BtGattOp));
    
    InitializeListEntry(&op->op.node);	
    op->apCb = BleHandleReadCharacteristicDescriptorCb;
    
    parm = &packet->parm.readCharDescriptors;
    parm->handle = handle;
    op->parms.p.readCharDescriptors = parm;
    
    return BT_GATT_ReadCharacteristicDescriptors(packet->link, op);
}

BtStatus BleReadLongCharacteristicDescriptor(BlePacket *packet,
             U16 handle,
             U16 offset)
{
    BtGattOp *op;
    BtGattReadLongCharacteristicDescriptors *parm;
    BtStatus status;
    
    kal_trace(BT_TRACE_BLE_PROFILES, BLEUTILS_READ_LONG_CHARDESC_HANDLE, handle, offset);

    op = &packet->gatt_op;
    OS_MemSet((U8 *)op, 0, sizeof(BtGattOp));
    
    InitializeListEntry(&op->op.node);	
    op->apCb = BleHandleReadLongCharacteristicDescriptorCb;
    
    parm = &packet->parm.readLongCharDescriptors;
    parm->handle = handle;
    parm->offset = offset;
    op->parms.p.readLongCharDescriptors = parm;
    
    return BT_GATT_ReadLongCharacteristicDescriptors(packet->link, op);
}


/*****************************************************************************
 * FUNCTION
 *  HtpcReadCharacteristicEx
 *
 * DESCRIPTION
 *  Try to read characteristic value, if it is not read permitted, try to read the next one.
 *  If no more characteristic value is read permitted, try to read descriptors from the first
 *  characteristic definition.
 *
 *****************************************************************************/
BtStatus BleReadCharacteristicDefinition(BlePacket *packet, BleServiceNode *service_node, BleCharNode *char_node)
{
    BleCharDescNode *char_desc_node;

    Report(("[BleUtils] BleReadCharacteristicDefinition, value_size: %d", char_node->characteristic.value_size));
    while ((ListEntry *)char_node != &service_node->char_list)
    {
        /* check if it is read permitted */
        if ((char_node->characteristic.properties & GATT_CHAR_PROP_READ) && 
                (char_node->characteristic.value_size == 0)) 
        {
            kal_trace(BT_TRACE_BLE_PROFILES, BLEUTILS_READ_CHAR, char_node->characteristic.value_handle);
            return BleReadCharacteristic(packet, char_node->characteristic.value_handle);
        }
        char_node = (BleCharNode *)GetNextNode(&char_node->node);
    }
    
    /* Here, no more characteristic value is read permitted
    *   Then continue to read descriptor if descriptor handle is existed
    */
    kal_trace(BT_TRACE_BLE_PROFILES, BLEUTILS_READ_CHAR_DESCRIPTOR);
    
    char_node = (BleCharNode *)GetHeadList(&service_node->char_list);
    while ((ListEntry *)char_node != &service_node->char_list)
    {
        //if (!IsListEmpty(&char_node->char_desc_list))
        char_desc_node = (BleCharDescNode *)GetHeadList(&char_node->char_desc_list); 
        while ((ListEntry *)char_desc_node != &char_node->char_desc_list)
        {
            if (char_desc_node->descriptor.desc_size == 0)
            {
                /* Descriptor has been queried before, just read descriptor value directly */
                return BleReadCharacteristicDescriptor(packet, char_desc_node->descriptor.handle);
            }
            char_desc_node = (BleCharDescNode *)GetNextNode(&char_desc_node->node);
        }
        char_node = (BleCharNode *)GetNextNode(&char_node->node);
    }

    kal_trace(BT_TRACE_BLE_PROFILES, BLEUTILS_READCHAR_DONE);   

    return BT_STATUS_SUCCESS;
}

BtStatus BleReadServiceDefinitionByUuid(BlePacket *packet,
             BleServiceDB *servicedb,            
             U8 uuid_size,
             U8 *uuid)
{
    BleServiceNode *service_node;
    BleCharNode *char_node;
    BtStatus status;

    service_node = BleFindServicebyUuid(servicedb, uuid_size, uuid);
    if (service_node == NULL)
    {
        return BT_STATUS_NOT_FOUND;
    }    

    if (IsListEmpty(&service_node->char_list))
    {
        return BT_STATUS_FAILED;
    }

    char_node = (BleCharNode *)GetHeadList(&service_node->char_list);
    status = BleReadCharacteristicDefinition(packet, service_node, char_node);
    if (status == BT_STATUS_PENDING)
    {
        RemoveEntryList(&service_node->node);
        InsertTailList(&packet->service_list, &service_node->node);
    }
    return status;
}

void BleHandleFindPrimaryServicesByUuidCb(void *userdata, BtGattEvent *event)
{
    BtGattOp *op = (BtGattOp *)userdata;
    BtGattDiscoveryPrimaryServiceByUUIDResultEvent *result;
    BtGattDiscoveryPrimaryServiceByUUID *parm = op->parms.p.primaryServiceByUUID;
    BlePacket *packet;
    BleServiceNode *service_node;
    BtStatus status;
    BT_BOOL b_found = FALSE;
    U8 *uuid;

    result = (BtGattDiscoveryPrimaryServiceByUUIDResultEvent *) event->parms;
    packet = ContainingRecord(op, BlePacket, gatt_op);
    if (packet->bCancelled && (result->flag != BT_GATT_EVENT_RESULT_CONTINUE_DATA))
    {
        BleQueryServiceDefinitionCallback(packet, BT_STATUS_CANCELLED);
        return;
    }


    if (parm->type == 2)
    {
        uuid = parm->uuid16;
    }
    else
    {
        uuid = parm->uuid128;
    }
    
    kal_trace(BT_TRACE_BLE_PROFILES, BLEUTILS_FINDPRIMARYSERVICE_UUID, parm->uuid16[1], parm->uuid16[0]);	

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            //service_node = (BleServiceNode *)BLE_UTILS_MALLOC(sizeof(BleServiceNode));
            //OS_MemSet((U8 *)service_node, 0, sizeof(BleServiceNode));
            //InitializeListHead(&service_node->include_list);
            //InitializeListHead(&service_node->char_list);
            
            //service_node->service.start_handle = result->startHandle;
            //service_node->service.end_handle = result->endHandle;
            //service_node->service.uuid_size = parm->type;
            //BLE_UUID_SET(service_node->service.uuid.uuid16, service_node->service.uuid.uuid128, uuid, service_node->service.uuid_size);

            Report(("[BleUtils] find primary service, start handle: %d, end handle: %d", result->startHandle, result->endHandle));
            service_node = BleNewService(parm->type, uuid, result->startHandle, result->endHandle);  
            if (service_node)
            {
                InsertTailList(&packet->service_list, &service_node->node);
            }
            break;

        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:   // end up when "attribute not found" error response received
            service_node = (BleServiceNode *)GetHeadList(&packet->service_list);
            while ((ListEntry *)service_node != &packet->service_list)
            {
                if ((service_node->service.uuid_size == parm->type) &&
                    (BLE_UUID_IS_EQUAL(service_node->service.uuid, uuid, parm->type)))
                {
                    b_found = TRUE;
                    break;
                }
                /* try the next node in the list */
                service_node = (BleServiceNode *)GetNextNode(&service_node->node);      
            }

            if (b_found)
            {
                status = BleFindIncludeServices(packet, service_node->service.start_handle, service_node->service.end_handle);
                if (status != BT_STATUS_PENDING)
                {
                    BleQueryServiceDefinitionCallback(packet, status);
                }
            }
            else
            {
                BleQueryServiceDefinitionCallback(packet, BT_STATUS_FAILED);
            }
            break;

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            BleQueryServiceDefinitionCallback(packet, BT_STATUS_FAILED);
            break;
    }


}
 
void BleHandleFindIncludeServicesCb(void *userdata, BtGattEvent *event)
{
    BtGattOp *op = (BtGattOp *)userdata;
    BtGattFindIncludeServicesResultEvent *result;
    BtGattFindIncludeServices *parm = op->parms.p.findIncludeServices;
    BlePacket *packet;
    BleServiceNode *service_node;
    BleIncludeServiceNode *include_node;
    BtStatus status;

    result = (BtGattFindIncludeServicesResultEvent *) event->parms;
    packet = ContainingRecord(op, BlePacket, gatt_op);
    if (packet->bCancelled && (result->flag != BT_GATT_EVENT_RESULT_CONTINUE_DATA))
    {
        BleQueryServiceDefinitionCallback(packet, BT_STATUS_CANCELLED);
        return;
    }

    service_node = BleFindServicebyHandleRange(&packet->service_list, parm->startHandle);        

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            {
                U8 *pUuid;
                kal_trace(BT_TRACE_BLE_PROFILES, BLEUTILS_FINDINCLUDESERVICES_UUID, result->type, result->serviceUUID);   
                include_node = (BleIncludeServiceNode *)BLE_UTILS_MALLOC(sizeof(BleIncludeServiceNode));
                OS_MemSet((U8 *)include_node, 0, sizeof(BleIncludeServiceNode));
                
                include_node->include_service.include_service_handle = result->includeServiceHandle;
                include_node->include_service.end_group_handle = result->endGroupHandle;
                include_node->include_service.uuid_size = result->type;
                if (result->type == 2)
                {
                    pUuid = include_node->include_service.uuid.uuid16;
                    pUuid[0] = result->serviceUUID & 0xff;
                    pUuid[1] = (result->serviceUUID >> 8) & 0xff;
                }
                else
                {
                    pUuid = include_node->include_service.uuid.uuid128;
                    OS_MemCopy(pUuid, result->serviceUUID16, result->type);
                }
                
                InsertTailList(&service_node->include_list, &include_node->node);
            }
            break;

        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:   // end up when "attribute not found" error response received
            status = BleFindAllCharacteristics(packet, service_node->service.start_handle, service_node->service.end_handle);
            if (status != BT_STATUS_PENDING)
            {
                BleQueryServiceDefinitionCallback(packet, status);
            }
            break;

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            BleQueryServiceDefinitionCallback(packet, BT_STATUS_FAILED);
            break;
    }

}

 
void BleHandleFindAllCharacteristicsCb(void *userdata, BtGattEvent *event)
{
    BtGattOp *op = (BtGattOp *)userdata;
    BtGattDiscoveryAllCharacteristicOfServiceResultEvent *result;
    BtGattDiscoveryAllCharacteristicOfService *parm = op->parms.p.allCharOfServices;
    BlePacket *packet;
    BleServiceNode *service_node;
    BleCharNode *char_node;
    BtStatus status = BT_STATUS_SUCCESS;

    result = (BtGattDiscoveryAllCharacteristicOfServiceResultEvent *) event->parms;
    packet = ContainingRecord(op, BlePacket, gatt_op);
    if (packet->bCancelled && (result->flag != BT_GATT_EVENT_RESULT_CONTINUE_DATA))
    {
        BleQueryServiceDefinitionCallback(packet, BT_STATUS_CANCELLED);
        return;
    }

    service_node = BleFindServicebyHandleRange(&packet->service_list, parm->startHandle);        

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            {
                U8 *pUuid;
                kal_trace(BT_TRACE_BLE_PROFILES, BLEUTILS_FINDALLCHAR_UUID, result->type, result->charUUID);   
                char_node = (BleCharNode *)BLE_UTILS_MALLOC(sizeof(BleCharNode));
                OS_MemSet((U8 *)char_node, 0, sizeof(BleCharNode));
                
                char_node->characteristic.handle = result->attributeHandle;
                char_node->characteristic.properties = result->charProperty;
                char_node->characteristic.value_handle = result->charValueAttributeHandle;
                char_node->characteristic.value_uuid_size = result->type;
                if (result->type == 2)
                {
                    pUuid = char_node->characteristic.value_uuid.uuid16;
                    pUuid[0] = result->charUUID & 0xff;
                    pUuid[1] = (result->charUUID >> 8) & 0xff;
                }
                else
                {
                    pUuid = char_node->characteristic.value_uuid.uuid128;
                    OS_MemCopy(pUuid, result->charUUID16, result->type);
                }
                InitializeListHead(&char_node->char_desc_list);
                Report(("[BleUtils] find all char cb, handle: %d, prop: %d, value handle: %d", result->attributeHandle,
                                                result->charProperty, result->charValueAttributeHandle));
                InsertTailList(&service_node->char_list, &char_node->node);
            }
            break;

        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:   // end up when "attribute not found" error response received
            /* No characteristic in service */ 
            //if (IsListEmpty(&service_node->char_list))
            //{
            //    BleQueryServiceDefinitionCallback(packet, BT_STATUS_FAILED);
            //    return;
            //}

            char_node = (BleCharNode *)GetHeadList(&service_node->char_list);
            /* Find all characteristic descriptors */ 
            while ((ListEntry *)char_node != &service_node->char_list)
            {
                BleCharNode *char_next_node;
                U16 start_handle;
                U16 end_handle;

                start_handle = char_node->characteristic.value_handle + 1;
                char_next_node = (BleCharNode *)GetNextNode(&char_node->node);  
                /* This one might be the last characteristic in service */
                if ((ListEntry *)char_next_node == &service_node->char_list)
                {
                    end_handle = service_node->service.end_handle;
                }
                else
                {
                    end_handle = char_next_node->characteristic.handle - 1;
                }
                Report(("[BleUtils] find all characteristics descriptor --> find char cb, start handle: %d, end handle: %d",
                                start_handle, end_handle));
                if (start_handle <= end_handle)
                {
                    status = BleFindAllCharacteristicDescriptors(packet, start_handle, end_handle);
                    if (status != BT_STATUS_PENDING)
                    {
                        BleQueryServiceDefinitionCallback(packet, status);
                    }
                    return;
                }
                char_node = char_next_node;
            }

            if(packet->explore_only == 0){
                /* No descriptor is existed, then read characteristic value */
                char_node = (BleCharNode *)GetHeadList(&service_node->char_list);
                status = BleReadCharacteristicDefinition(packet, service_node, char_node);
            }else{
                status = BT_STATUS_SUCCESS;
            }
            if (status != BT_STATUS_PENDING)
            {
                BleQueryServiceDefinitionCallback(packet, BT_STATUS_SUCCESS);
            }
            break;

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            BleQueryServiceDefinitionCallback(packet, BT_STATUS_FAILED);
            break;
    }

}


void BleHandleFindAllCharacteristicDescriptorsCb(void *userdata, BtGattEvent *event)
{
    BtGattOp *op = (BtGattOp *)userdata;
    BtGattDiscoveryAllCharacteristicDescriptorsResultEvent *result;
    BtGattDiscoveryAllCharacteristicDescriptors *parm = op->parms.p.allCharDescriptors;
    BlePacket *packet;
    BleServiceNode *service_node;
    BleCharNode *char_node;
    BleCharDescNode *char_desc_node;
    BtStatus status = BT_STATUS_SUCCESS;

    result = (BtGattDiscoveryAllCharacteristicDescriptorsResultEvent *) event->parms;
    packet = ContainingRecord(op, BlePacket, gatt_op);
    if (packet->bCancelled && (result->flag != BT_GATT_EVENT_RESULT_CONTINUE_DATA))
    {
        BleQueryServiceDefinitionCallback(packet, BT_STATUS_CANCELLED);
        return;
    }

    service_node = BleFindServicebyHandleRange(&packet->service_list, parm->startHandle);        
    char_node = BleFindCharbyHandle(service_node, parm->startHandle - 1);

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            {
                U8 *pUuid;
                kal_trace(BT_TRACE_BLE_PROFILES, BLEUTILS_FINDALLCHARDESC_UUID, result->type, result->uuid16);   

                char_desc_node = (BleCharDescNode *)BLE_UTILS_MALLOC(sizeof(BleCharDescNode));
                OS_MemSet((U8 *)char_desc_node, 0, sizeof(BleCharDescNode));
                
                char_desc_node->descriptor.handle = result->attributeHandle;
                char_desc_node->descriptor.desc_uuid_size = result->type;
                if (result->type == 2)
                {
                    pUuid = char_desc_node->descriptor.desc_uuid.uuid16;
                    pUuid[0] = result->uuid16 & 0xff;
                    pUuid[1] = (result->uuid16 >> 8) & 0xff;
                }
                else
                {
                    pUuid = char_desc_node->descriptor.desc_uuid.uuid128;
                    OS_MemCopy(pUuid, result->uuid128, result->type);
                }
                
                InsertTailList(&char_node->char_desc_list, &char_desc_node->node);
            }
            break;

        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:   // end up when "attribute not found" error response received
            char_node = (BleCharNode *)GetNextNode(&char_node->node);
            /* Find all descriptors in the next characteristic */ 
            while ((ListEntry *)char_node != &service_node->char_list)
            {
                BleCharNode *char_next_node;
                U16 start_handle;
                U16 end_handle;

                start_handle = char_node->characteristic.value_handle + 1;
                char_next_node = (BleCharNode *)GetNextNode(&char_node->node);  
                /* This one might be the last characteristic in service */
                if ((ListEntry *)char_next_node == &service_node->char_list)
                {
                    end_handle = service_node->service.end_handle;
                }
                else
                {
                    end_handle = char_next_node->characteristic.handle - 1;
                }
                Report(("[BleUtils] find all characteristics descriptor --> find descriptor cb, start handle: %d, end handle: %d",
                                start_handle, end_handle));
                if (start_handle <= end_handle)
                {
                    status = BleFindAllCharacteristicDescriptors(packet, start_handle, end_handle);
                    if (status != BT_STATUS_PENDING)
                    {
                        BleQueryServiceDefinitionCallback(packet, status);
                    }
                    return;
                }
                char_node = char_next_node;
            }

            if(packet->explore_only == 0){
                /* No More descriptor to be found, then read characteristic value */
                char_node = (BleCharNode *)GetHeadList(&service_node->char_list);
                status = BleReadCharacteristicDefinition(packet, service_node, char_node);
            }else{
                status = BT_STATUS_SUCCESS;
            }
            if (status != BT_STATUS_PENDING)
            {
                BleQueryServiceDefinitionCallback(packet, BT_STATUS_SUCCESS);
            }
            break;

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            BleQueryServiceDefinitionCallback(packet, BT_STATUS_FAILED);
            break;
    }

}

 
void BleHandleReadCharacteristicCb(void *userdata, BtGattEvent *event)
{
    BtGattOp *op = (BtGattOp *)userdata;
    BtGattReadCharacteristicValueResultEvent *result;
    BtGattReadCharacteristicValue *parm = op->parms.p.readCharValue;
    BlePacket *packet;
    BleServiceNode *service_node;
    BleCharNode *char_node;
    BtStatus status;

    result = (BtGattReadCharacteristicValueResultEvent *) event->parms;
    packet = ContainingRecord(op, BlePacket, gatt_op);
    if (packet->bCancelled)
    {
        BleQueryServiceDefinitionCallback(packet, BT_STATUS_CANCELLED);
        return;
    }


    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:   
            service_node = BleFindServicebyHandleRange(&packet->service_list, parm->handle);
            char_node = BleFindCharbyHandle(service_node, parm->handle);

            char_node->characteristic.value = (U8 *)BLE_UTILS_MALLOC(result->length);
            OS_MemSet(char_node->characteristic.value, 0, result->length);
            OS_MemCopy(char_node->characteristic.value, result->data, result->length);
            char_node->characteristic.value_size = result->length;

            Report(("[BleUtils] read characteristic, MTU: %d", BT_GATT_GetNegotiationMTU(packet->link)));
            /* Long Characteristic value */            
            if (result->length == (BT_GATT_GetNegotiationMTU(packet->link) - 1))
            {
                status = BleReadLongCharacteristic(packet, parm->handle, result->length);
                if (status != BT_STATUS_PENDING)
                {
                    BleQueryServiceDefinitionCallback(packet, status);
                }
            }
            else    /* Read the next one */
            {
                char_node = (BleCharNode *)GetNextNode(&char_node->node);
                status = BleReadCharacteristicDefinition(packet, service_node, char_node);
                if (status != BT_STATUS_PENDING)
                {
                    BleQueryServiceDefinitionCallback(packet, BT_STATUS_SUCCESS);
                }
            }
            break;

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            BleQueryServiceDefinitionCallback(packet, BT_STATUS_FAILED);
            break;
    }

}


void BleHandleReadLongCharacteristicCb(void *userdata, BtGattEvent *event)
{
    BtGattOp *op = (BtGattOp *)userdata;
    BtGattReadLongCharacteristicValueResultEvent *result;
    BtGattReadLongCharacteristicValue *parm = op->parms.p.readLongCharValue;
    BlePacket *packet;
    BleServiceNode *service_node;
    BleCharNode *char_node;
    BtStatus status;

    result = (BtGattReadLongCharacteristicValueResultEvent *) event->parms;
    packet = ContainingRecord(op, BlePacket, gatt_op);
    if (packet->bCancelled && (result->flag != BT_GATT_EVENT_RESULT_CONTINUE_DATA))
    {
        BleQueryServiceDefinitionCallback(packet, BT_STATUS_CANCELLED);
        return;
    }

    service_node = BleFindServicebyHandleRange(&packet->service_list, parm->handle);
    char_node = BleFindCharbyHandle(service_node, parm->handle);

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            {
                U8 *value;

                value = (U8 *)BLE_UTILS_MALLOC(char_node->characteristic.value_size + result->length);
                OS_MemSet(value, 0, char_node->characteristic.value_size + result->length);
                OS_MemCopy(value, char_node->characteristic.value, char_node->characteristic.value_size);
                OS_MemCopy(value + char_node->characteristic.value_size, result->data, result->length);

                BLE_UTILS_FREE(char_node->characteristic.value);
                char_node->characteristic.value = value;
                char_node->characteristic.value_size += result->length;

                Report(("[BleUtils] read long characteristic callback continue, len: %d", char_node->characteristic.value_size));
/*
                {
                    U8 *p = result->data;
                    if (result->length == 22)
                    {
                        Report(("[HID] descriptor from gatt, %x, %x, %x, %x, %x, %x", p[0], p[1], p[2], p[3], p[4], p[5]));
                        Report(("[HID] descriptor from gatt, %x, %x, %x, %x, %x, %x", p[6], p[7], p[8], p[9], p[10], p[11]));
                        Report(("[HID] descriptor from gatt, %x, %x, %x, %x, %x, %x", p[12], p[13], p[14], p[15], p[16], p[17]));
                        Report(("[HID] descriptor from gatt, %x, %x, %x, %x", p[18], p[19], p[20], p[21]));
                    }
                    else
                    {
                        Report(("[HID] descriptor from gatt, %x, %x", p[0], p[1]));
                    }
                }
*/                
             }
            break;

        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:   
            /* Read the next one */    
            char_node = (BleCharNode *)GetNextNode(&char_node->node);
            status = BleReadCharacteristicDefinition(packet, service_node, char_node);
            if (status != BT_STATUS_PENDING)
            {
                BleQueryServiceDefinitionCallback(packet, BT_STATUS_SUCCESS);
            }
            break;

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            BleQueryServiceDefinitionCallback(packet, BT_STATUS_FAILED);
            break;
    }

}

void BleHandleReadCharacteristicDescriptorCb(void *userdata, BtGattEvent *event)
{
    BtGattOp *op = (BtGattOp *)userdata;
    BtGattReadCharacteristicDescriptorsResultEvent *result;
    BtGattReadCharacteristicDescriptors *parm = op->parms.p.readCharDescriptors;
    BlePacket *packet;
    BleServiceNode *service_node;
    BleCharNode *char_node;
    BleCharDescNode *char_desc_node;
    BtStatus status = BT_STATUS_SUCCESS;

    result = (BtGattReadCharacteristicDescriptorsResultEvent *) event->parms;
    packet = ContainingRecord(op, BlePacket, gatt_op);
    if (packet->bCancelled)
    {
        BleQueryServiceDefinitionCallback(packet, BT_STATUS_CANCELLED);
        return;
    }

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:   
            service_node = BleFindServicebyHandleRange(&packet->service_list, parm->handle);
            char_node = BleFindCharbyHandleRange(service_node, parm->handle);
            char_desc_node = BleFindCharDescbyHandle(char_node, parm->handle);

            char_desc_node->descriptor.desc = (U8 *)BLE_UTILS_MALLOC(result->length);
            OS_MemSet(char_desc_node->descriptor.desc, 0, result->length);
            OS_MemCopy(char_desc_node->descriptor.desc, result->data, result->length);
            char_desc_node->descriptor.desc_size = result->length;

            /* Long Characteristic value */            
            if (result->length == (BT_GATT_GetNegotiationMTU(packet->link) - 1))
            {
                status = BleReadLongCharacteristicDescriptor(packet, parm->handle, result->length);
                if (status != BT_STATUS_PENDING)
                {
                    BleQueryServiceDefinitionCallback(packet, status);
                }
            }
            else    /* Read the next one */
            {
                char_desc_node = (BleCharDescNode *)GetNextNode(&char_desc_node->node);
                if ((ListEntry *)char_desc_node != &char_node->char_desc_list)
                {
                    status = BleReadCharacteristicDescriptor(packet, char_desc_node->descriptor.handle);
                    if (status != BT_STATUS_PENDING)
                    {
                        BleQueryServiceDefinitionCallback(packet, BT_STATUS_SUCCESS);
                    }
                    return;
                }

                char_node = (BleCharNode *)GetNextNode(&char_node->node);
                while ((ListEntry *)char_node != &service_node->char_list)
                {
                    if (!IsListEmpty(&char_node->char_desc_list))
                    {
                        /* Descriptor has been queried before, just read descriptor value directly */
                        char_desc_node = (BleCharDescNode *)GetHeadList(&char_node->char_desc_list); 
                        status = BleReadCharacteristicDescriptor(packet, char_desc_node->descriptor.handle);
                        break;
                    }
                    char_node = (BleCharNode *)GetNextNode(&char_node->node);
                }

                if (status != BT_STATUS_PENDING)
                {
                    BleQueryServiceDefinitionCallback(packet, BT_STATUS_SUCCESS);
                }
            }
            break;

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            BleQueryServiceDefinitionCallback(packet, BT_STATUS_FAILED);
            break;
    }

}


void BleHandleReadLongCharacteristicDescriptorCb(void *userdata, BtGattEvent *event)
{
    BtGattOp *op = (BtGattOp *)userdata;
    BtGattReadLongCharacteristicDescriptorsResultEvent *result;
    BtGattReadLongCharacteristicDescriptors *parm = op->parms.p.readLongCharDescriptors;
    BlePacket *packet;
    BleServiceNode *service_node;
    BleCharNode *char_node;
    BleCharDescNode *char_desc_node;
    BtStatus status = BT_STATUS_SUCCESS;

    result = (BtGattReadLongCharacteristicDescriptorsResultEvent *) event->parms;
    packet = ContainingRecord(op, BlePacket, gatt_op);
    if (packet->bCancelled && (result->flag != BT_GATT_EVENT_RESULT_CONTINUE_DATA))
    {
        BleQueryServiceDefinitionCallback(packet, BT_STATUS_CANCELLED);
        return;
    }

    service_node = BleFindServicebyHandleRange(&packet->service_list, parm->handle);
    char_node = BleFindCharbyHandleRange(service_node, parm->handle);
    char_desc_node = BleFindCharDescbyHandle(char_node, parm->handle);

    switch (result->flag)
    {
        case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
            {
                U8 *value;
                
                value = (U8 *)BLE_UTILS_MALLOC(char_desc_node->descriptor.desc_size + result->length);
                OS_MemSet(value, 0, char_desc_node->descriptor.desc_size + result->length);
                OS_MemCopy(value, char_desc_node->descriptor.desc, char_desc_node->descriptor.desc_size);
                OS_MemCopy(value + char_desc_node->descriptor.desc_size, result->data, result->length);

                BLE_UTILS_FREE(char_desc_node->descriptor.desc);
                char_desc_node->descriptor.desc = value;
                char_desc_node->descriptor.desc_size += result->length;
            }            
            break;

        case BT_GATT_EVENT_RESULT_DONE_COMPLETED:   
            {
                U8 *value;
                
                value = (U8 *)BLE_UTILS_MALLOC(char_desc_node->descriptor.desc_size + result->length);
                OS_MemSet(value, 0, char_desc_node->descriptor.desc_size + result->length);
                OS_MemCopy(value, char_desc_node->descriptor.desc, char_desc_node->descriptor.desc_size);
                OS_MemCopy(value + char_desc_node->descriptor.desc_size, result->data, result->length);

                BLE_UTILS_FREE(char_desc_node->descriptor.desc);
                char_desc_node->descriptor.desc = value;
                char_desc_node->descriptor.desc_size += result->length;
            }

            /* Try the next descriptor in the same char */
            char_desc_node = (BleCharDescNode *)GetNextNode(&char_desc_node->node);
            if ((ListEntry *)char_desc_node != &char_node->char_desc_list)
            {
                status = BleReadCharacteristicDescriptor(packet, char_desc_node->descriptor.handle);
                if (status != BT_STATUS_PENDING)
                {
                    BleQueryServiceDefinitionCallback(packet, BT_STATUS_SUCCESS);
                }
                return;
            }

            /* No more descriptor in the char, try the next char */
            char_node = (BleCharNode *)GetNextNode(&char_node->node);
            while ((ListEntry *)char_node != &service_node->char_list)
            {
                if (!IsListEmpty(&char_node->char_desc_list))
                {
                    /* Descriptor has been queried before, just read descriptor value directly */
                    char_desc_node = (BleCharDescNode *)GetHeadList(&char_node->char_desc_list); 
                    status = BleReadCharacteristicDescriptor(packet, char_desc_node->descriptor.handle);
                    break;
                }
                char_node = (BleCharNode *)GetNextNode(&char_node->node);
            }

            if (status != BT_STATUS_PENDING)
            {
                BleQueryServiceDefinitionCallback(packet, BT_STATUS_SUCCESS);
            }
            break;

        case BT_GATT_EVENT_RESULT_ERROR:
        case BT_GATT_EVENT_RESULT_DISCONNECT:
            BleQueryServiceDefinitionCallback(packet, BT_STATUS_FAILED);
            break;
    }

}


void BleUtilsInitialize(void)
{
    InitializeListHead(&g_BleServiceDBTable);

    /* register the GATT callback to handle connection */
    BT_GattRegisterConnectedEvent(&g_GattHandler, BleGattEventCallback);

    /* register bond callback to obtain bond status */
    g_BondHandler.callback = BleBondingCallback;
    ATTDB_RegisterBondStatus(&g_BondHandler);
    
}

static BtStatus BLE_QueryServiceDefinitionByUuid_internal(void *context,
             BtRemoteDevice *link,
             BleQueryServiceDefinitionCb cb,
             U8 uuid_size,
             U8 *uuid,
             U8 explore_only,
             U16 startHandle,
             U16 endHandle,           
             U8 **handle_out)
{
    BlePacket *packet;
    BleServiceDB *servicedb;
    BleServiceNode *service_node;      
    BtStatus status;
    BtDeviceRecord record;

    packet = (BlePacket *)BLE_UTILS_MALLOC(sizeof(BlePacket));
    OS_MemSet((U8 *)packet, 0, sizeof(BlePacket));
    
    packet->cb = cb;
    packet->context = context;
    packet->link = link;
    packet->explore_only = explore_only;
    InitializeListHead(&packet->service_list);

    if (handle_out != NULL)
    {
        *handle_out = &packet->hd;
    }

    if ((servicedb = BleFindServiceDBbyAddr(&link->bdAddr)) == NULL)
    {
        if ((servicedb = BleNewServiceDB(&link->bdAddr)) == NULL)
        {
            BLE_UTILS_FREE(packet);
            return BT_STATUS_NO_RESOURCES;      
        }

        /* 1st: check whether it is bonded device. If it is removed from bonded device
        *   list, remove it from database, and query service again 
        */
        if (SEC_FindDeviceRecord(&link->bdAddr, &record) == BT_STATUS_SUCCESS) // bonded device
        {
            /* 2nd: check service database first */
            if (BleReadServiceDB(&link->bdAddr, servicedb) == BT_STATUS_SUCCESS)
            {
                /* 3rd: read HTP characteristic and descriptors by handle */
                status = BleReadServiceDefinitionByUuid(packet, servicedb, uuid_size, uuid);
                Report(("[BleUtils] ReadService status: %d", status));
                if (status == BT_STATUS_PENDING)
                {
                    InsertTailList(&g_BleServiceDBTable, &servicedb->node);
                    return status;
                }
                else if (status == BT_STATUS_SUCCESS)
                {
                    InsertTailList(&g_BleServiceDBTable, &servicedb->node);
                    BLE_UTILS_FREE(packet);
                    return status;
                }
                else if (status == BT_STATUS_NOT_FOUND)
                {
                    /* The service is not queried before, start a new query procedure */
                }
                else
                {
                    BleFreeServiceDB(servicedb);
                    BLE_UTILS_FREE(packet);
                    return status;
                }
            }
        }
        else  // unbonded device
        {
            BleDeleteServiceDB(&link->bdAddr);
        }

        InsertTailList(&g_BleServiceDBTable, &servicedb->node);
    }
    else
    {
        status = BleReadServiceDefinitionByUuid(packet, servicedb, uuid_size, uuid);
        if (status == BT_STATUS_PENDING)
        {
            return status;
        }
        else if (status != BT_STATUS_NOT_FOUND)
        {
            BLE_UTILS_FREE(packet);
            return status;
        }
    }

    /* endHandle 0 indicates that handle range isn't supplied, 
       we need get it by "Find By Type Value" firstly */
    kal_trace(BT_TRACE_BLE_PROFILES, BLEUTILS_QUEREYGATTSERVICE);
    if (!endHandle)
    {
        status = BleFindPrimaryServicesByUuid(packet, uuid_size, uuid);
    }
    else
    {
        service_node = BleNewService(uuid_size, uuid, startHandle, endHandle);
        if (service_node)
        {
            InsertTailList(&packet->service_list, &service_node->node); 
            status = BleFindIncludeServices(packet, startHandle, endHandle);
            if (status != BT_STATUS_PENDING)
            {
                RemoveEntryList(&service_node->node); 
                BleFreeService(service_node);        
            } 
        }
        else
        {
            status = BT_STATUS_NOSERVICES;
        }       
    }

    if (status != BT_STATUS_PENDING)
    {
        RemoveEntryList(&servicedb->node);
        BleFreeServiceDB(servicedb);
        BLE_UTILS_FREE(packet);
    }

    return status;
}

/*****************************************************************************
* FUNCTION
*  BLE_QueryServiceDefinitionByUuid
*
* DESCRIPTION
*   Use GATT procedure to find all information in indicated service definition. It shall contain 
*   service declaration and may contain include definitions and characteristic definitions 
*   which shall contain characteristic declaration, characteristic value declaration and 
*   may contain characteristic descriptor declarations.
*
*   It is an asynchronous procedure with message confirmation in callback function.
*   1. New BlePacket
*   2. New BleServiceDB, and do not insert into g_BleServiceDBTable util the query is done.
*
*****************************************************************************/
BtStatus BLE_QueryServiceDefinitionByUuid(void *context,
             BtRemoteDevice *link,
             BleQueryServiceDefinitionCb cb,
             U8 uuid_size,
             U8 *uuid,
             U8 **handle_out){
    return BLE_QueryServiceDefinitionByUuid_internal(context, link, cb, uuid_size, uuid, 0, 0, 0, handle_out);
}


BtStatus BLE_QueryServiceDefinition(void *context,
             BtRemoteDevice *link,
             BleQueryServiceDefinitionCb cb,
             U8 uuid_size,
             U8 *uuid,
             U16 startHandle,
             U16 endHandle,
             U8 isPrimary,
             U8 **handle_out){
    return BLE_QueryServiceDefinitionByUuid_internal(context, link, cb, uuid_size, uuid, 1, startHandle, endHandle, handle_out);
}

/*****************************************************************************
 * FUNCTION
 *  BLE_FindServiceByUuid
 *
 * DESCRIPTION
 *
 *   It is synchronous call, and returns the data structure BleService.
 *
 *****************************************************************************/
BleService* BLE_FindServiceByUuid(const BD_ADDR *bt_addr,
                                                                             U8 uuid_size,
                                                                             U8 *uuid)
{
    BleServiceDB *servicedb;
    BleServiceNode *service_node = NULL;
    BleService *service;

    servicedb = BleFindServiceDBbyAddr(bt_addr);
    if (servicedb == NULL)
    {
        servicedb = BleNewServiceDB(bt_addr);
        if (servicedb == NULL)
        return NULL;

        if (BleReadServiceDB(bt_addr, servicedb) != BT_STATUS_SUCCESS)
        {
            BLE_UTILS_FREE(servicedb);
            return NULL;
        }
        InsertTailList(&g_BleServiceDBTable, &servicedb->node);
    }

    /* If uuid is null, return the first service in database */
    if (uuid == NULL)
    {
        if(!IsListEmpty(&servicedb->service_list)){
            service_node = (BleServiceNode *)GetHeadList(&servicedb->service_list);
        }
    }
    else
    {
        service_node = BleFindServicebyUuid(servicedb, uuid_size, uuid);
    }

    if (service_node == NULL)
        return NULL;

    return &service_node->service;
}

 
/*****************************************************************************
 * FUNCTION
 *  BLE_FindNextServiceByUuid
 *
 * DESCRIPTION
 *
 *   It is used for multiple instance service to search more other service node with the 
 * same service UUID. BLE_FindServiceByUuid returns the first service that matches the 
 * specified service UUID.
 *
 *   It is synchronous call, and returns the data structure BleService.
 *
 *****************************************************************************/
BleService* BLE_FindNextServiceByUuid(const BD_ADDR *bt_addr,
                                                                             BleService* prev_service,
                                                                             U8 uuid_size,
                                                                             U8 *uuid)
{
    BleServiceDB *servicedb;
    BleServiceNode *service_node;
    BleService *service;

    servicedb = BleFindServiceDBbyAddr(bt_addr);
    if (servicedb == NULL)
        return NULL;

    service_node = ContainingRecord(prev_service, BleServiceNode, service);

    service_node = (BleServiceNode *)GetNextNode(&service_node->node);
    if (uuid == NULL)
    {
        if ((ListEntry *)service_node != &servicedb->service_list)
        {
            return &service_node->service;
        }
        else
        {
            // no more service in service list
            return NULL;
        }
    }
    else
    {
        while ((ListEntry *)service_node != &servicedb->service_list)
        {
            if ((service_node->service.uuid_size == uuid_size) &&
                BLE_UUID_IS_EQUAL(service_node->service.uuid, uuid, uuid_size))
            {
                return &service_node->service;
            }
            /* try the next node in the list */
            service_node = (BleServiceNode *)GetNextNode(&service_node->node);      
        }
        
        return NULL;
    }
}

 
/*****************************************************************************
* FUNCTION
*  BLE_FindIncludeServiceByUuid
*
* DESCRIPTION
*
*   It is synchronous call, and returns the data structure BleIncludeService.
*
*****************************************************************************/
BleIncludeService* BLE_FindIncludeServiceByUuid(BleService *in_service,
                                                                     U8 uuid_size,
                                                                     U8 *uuid)
{
    BleServiceNode *service_node;
    BleIncludeServiceNode *include_node = NULL;

    service_node = ContainingRecord(in_service, BleServiceNode, service);

    if (uuid == NULL)
    {
        if(!IsListEmpty(&service_node->include_list))
            include_node = (BleIncludeServiceNode *)GetHeadList(&service_node->include_list);
    }
    else
    {
        include_node = BleFindIncludebyUuid(service_node, uuid_size, uuid);
    }
    if(include_node == NULL)
        return NULL;
    
    return &include_node->include_service;
}

BleIncludeService* BLE_FindNextIncludeServiceByUuid(BleService *in_service,
                                                                    BleIncludeService* prev_incl_service,
                                                                     U8 uuid_size,
                                                                     U8 *uuid)
{
    BleServiceNode *service_node;
    BleIncludeServiceNode *include_node;

    service_node = ContainingRecord(in_service, BleServiceNode, service);
    include_node = ContainingRecord(prev_incl_service, BleIncludeServiceNode, include_service);

    include_node = (BleIncludeServiceNode *)GetNextNode(&include_node->node);
    if (uuid == NULL)
    {
        if ((ListEntry *)include_node != &service_node->include_list)
        {
            return &include_node->include_service;
        }
        else
        {
            // no more include service in list
            return NULL;
        }
    }
    else
    {
        while ((ListEntry *)include_node != &service_node->include_list)
        {
            if ((include_node->include_service.uuid_size == uuid_size) &&
                BLE_UUID_IS_EQUAL(include_node->include_service.uuid, uuid, uuid_size))
            {
                return &include_node->include_service;
            }
            /* try the next node in the list */
            include_node = (BleIncludeServiceNode *)GetNextNode(&include_node->node);       
        }
        return NULL;
    }

}

/*****************************************************************************
* FUNCTION
*  BLE_FindCharByUuid
*
* DESCRIPTION
*
*   It is synchronous call, and returns the data structure BleChar.
*
*****************************************************************************/
BleChar* BLE_FindCharByUuid(BleService *in_service,
                                                                     U8 char_uuid_size,
                                                                     U8 *char_uuid)
{
    BleServiceNode *service_node;
    BleCharNode *char_node = NULL;

    service_node = ContainingRecord(in_service, BleServiceNode, service);

    if (char_uuid == NULL)
    {
        if(!IsListEmpty(&service_node->char_list)){
            char_node = (BleCharNode *)GetHeadList(&service_node->char_list);
        }
    }
    else
    {
        char_node = BleFindCharbyUuid(service_node, char_uuid_size, char_uuid);
    }
    if (char_node == NULL)
        return NULL;
    
    return &char_node->characteristic;
}

BleChar* BLE_FindNextCharByUuid(BleService *in_service,
                                                                     BleChar *prev_char,                                                                
                                                                     U8 char_uuid_size,
                                                                     U8 *char_uuid)
{
    BleServiceNode *service_node;
    BleCharNode *char_node;
    
    service_node = ContainingRecord(in_service, BleServiceNode, service);
    char_node = ContainingRecord(prev_char, BleCharNode, characteristic);

    char_node = (BleCharNode *)GetNextNode(&char_node->node);
    if (char_uuid == NULL)
    {
        if ((ListEntry *)char_node != &service_node->char_list)
        {
            return &char_node->characteristic;
        }
        else
        {
            // no more characteristic in list
            return NULL;
        }
    }
    else
    {
        while ((ListEntry *)char_node != &service_node->char_list)
        {
            if ((char_node->characteristic.value_uuid_size == char_uuid_size) &&
                BLE_UUID_IS_EQUAL(char_node->characteristic.value_uuid, char_uuid, char_uuid_size))
            {
                return &char_node->characteristic;
            }
            /* try the next node in the list */
            char_node = (BleCharNode *)GetNextNode(&char_node->node);       
        }
        
        return NULL;
    }
}


/*****************************************************************************
* FUNCTION
*  BLE_FindCharDescByUuid
*
* DESCRIPTION
*
*   It is synchronous call, and returns the data structure BleCharDesc.
*
*****************************************************************************/
BleCharDesc* BLE_FindCharDescByUuid(BleChar *in_char,
                                                                                     U8 desc_uuid_size,
                                                                                     U8 *desc_uuid)
{
    BleCharNode *char_node;
    BleCharDescNode *desc_node = NULL;

    char_node = ContainingRecord(in_char, BleCharNode, characteristic);

    if (desc_uuid == NULL)
    {
        if(!IsListEmpty(&char_node->char_desc_list)){
            desc_node = (BleCharDescNode *)GetHeadList(&char_node->char_desc_list);
        }
    }
    else
    {
        desc_node = BleFindCharDescbyUuid(char_node, desc_uuid_size, desc_uuid);
    }
    if (desc_node == NULL)
        return NULL;

    return &desc_node->descriptor;
}

BleCharDesc* BLE_FindNextCharDescByUuid(BleChar *in_char,
                                                                                    BleCharDesc *prev_desc,
                                                                                     U8 desc_uuid_size,
                                                                                     U8 *desc_uuid)
{
    BleCharNode *char_node;
    BleCharDescNode *desc_node;

    char_node = ContainingRecord(in_char, BleCharNode, characteristic);
    desc_node = ContainingRecord(prev_desc, BleCharDescNode, descriptor);

    desc_node = (BleCharDescNode *)GetNextNode(&desc_node->node);   
    if (desc_uuid == NULL)
    {
        if ((ListEntry *)desc_node != &char_node->char_desc_list)
        {
            return &desc_node->descriptor;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        while ((ListEntry *)desc_node != &char_node->char_desc_list)
        {
            if ((desc_node->descriptor.desc_uuid_size == desc_uuid_size) &&
                BLE_UUID_IS_EQUAL(desc_node->descriptor.desc_uuid, desc_uuid, desc_uuid_size))
            {
                return &desc_node->descriptor;
            }
            /* try the next node in the list */
            desc_node = (BleCharDescNode *)GetNextNode(&desc_node->node);       
        }
        return NULL;
    }

}

/*****************************************************************************
* FUNCTION
*  BLE_UpdateServiceDatabase
*
* DESCRIPTION
*
*   Update service database to file if there is any characteristic value or descriptor value is changed.
*
*****************************************************************************/
void BLE_UpdateServiceDatabase(BD_ADDR *bt_addr)
{
    BleServiceDB *servicedb;

    servicedb = BleFindServiceDBbyAddr(bt_addr);
    if (servicedb == NULL)
        return;

    BleWriteServiceDB(servicedb);
}


/*****************************************************************************
* FUNCTION
*  BLE_StopQueryServiceDefinition
*
* DESCRIPTION
*
*   Stop query procedure.
*
*****************************************************************************/
void BLE_StopQueryServiceDefinition(U8 *handle)
{
    BlePacket *packet;
    
    packet = ContainingRecord(handle, BlePacket, hd);
    Report(("[BleUtils] Stop query service..."));

    if (packet != NULL)
    {
        packet->bCancelled = TRUE;
        packet->cb = NULL;
    }
}


U32 BLE_VerifyDB(BD_ADDR *bd_addr){
    BleServiceDB *servicedb;
    BleServiceNode *svc_node;
    BleCharNode *char_node;
    
    OS_Report("BLE_VerifyDB");
    BTBMDumpBdAddr(bd_addr->addr);
    if ((servicedb = BleFindServiceDBbyAddr(bd_addr)) == NULL){
        return 1;
    }else{
        Assert(IsListCircular(&servicedb->service_list));
        svc_node = (BleServiceNode*)&servicedb->service_list;
        while((svc_node = (BleServiceNode*)GetNextNode(&svc_node->node)) != (BleServiceNode*)&servicedb->service_list){
            Assert(IsListCircular(&svc_node->include_list));
            Assert(IsListCircular(&svc_node->char_list));
            char_node = (BleCharNode*)&svc_node->char_list;
            while((char_node=(BleCharNode*)GetNextNode(&char_node->node)) != (BleCharNode*)&svc_node->char_list){
                Assert(IsListCircular(&char_node->char_desc_list));
            }
        }
    }
    return 0;
}
#endif /* __BT_4_0_BLE__ */
