/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
#include "bt_common.h"
#include "btalloc.h"
#include "bt_feature.h"
#include "btrom.h"
#include "att_i.h"
#include "att.h"
#include "attdb.h"
#include "attdbmain.h"
#include "attdef.h"
#include "bluetooth_gap_struct.h"
#include "gattBQE.h"
#include "gatti.h"
#include "ble_utils.h"

#if ATT_PROTOCOL == XA_ENABLED

#if defined (__ATT_TS_LARGE_DATABASE1__) || defined(__ATT_TS_LARGE_DATABASE2__) || defined(__ATT_TS_LARGE_DATABASE3__) || defined(__ATT_TS_SMALL_DATABASE__)
BtStatus ATTDB_AddRecordBQE(const U8 *data, U16 size);
void ATTDB_AddRecordCBBQE(U16 handle, AttdbCallback cb);
BtStatus ATTDB_AddGroupRecordBQE(U16 start_hdl, U16 end_hdl, U16 group_uuid);
#endif

extern void BleUtilsInitialize(void);
extern void BleServiceDbInitialize(void);
extern void BT_ATTHandleApplicationResponse(BtRemoteDevice *link, U8 error_code, U8 *data, U16 length);


const U8 attMainRecord[] = 
{

    /* Group from base BT_ATT_BASE_RECORD_HANDLE */
    
    /* Handle =   0x0001, Type16 = Primary service, Value = GAP Service*/
    /* Last define is the max variable size for buffer */
#ifdef __GATT_SERVICE_CHANGED_CHARACTERISTIC_SUPPORT__
    ATT_HANDLE16(ATT_HANDLE_PRIMARY_SERVICE_GATT, BT_UUID_GATT_TYPE_PRIMARY_SERVICE, BT_ATT_CONST_VARIABLE, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETUUID16(BT_UUID_GATT_SERVICE_GATT),
        
    ATT_HANDLE16(ATT_HANDLE_GATT_CHARACTERISTIC_SERVICE_CHANGED, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETCHARACTERISTIC(ATT_CHAR_PROP_INDICATE, ATT_HANDLE_GATT_CHARACTERISTIC_SERVICE_CHANGED_VALUE, BT_UUID_GATT_CHAR_TYPE_SERIVCE_CHANGED),
        
    ATT_HANDLE16(ATT_HANDLE_GATT_CHARACTERISTIC_SERVICE_CHANGED_VALUE, BT_UUID_GATT_CHAR_TYPE_SERIVCE_CHANGED, BT_ATT_FIX_VARIABLE, 0x04), 
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0),  
#endif
};

#if defined (__ATT_TS_LARGE_DATABASE1__) || defined(__ATT_TS_LARGE_DATABASE2__) || defined(__ATT_TS_LARGE_DATABASE3__) || defined(__ATT_TS_SMALL_DATABASE__)
U8 database0023(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
U8 database0024(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);

#else
U8 attBaseRecordHandeVariableValueArray[] = 
{
#ifdef __GATT_SERVICE_CHANGED_CHAR_SUPPORTED__

    /* First Variable Record */
    /* Size must exactly the same as MainRecord Defined */
    BT_UUID16_SIZE, ATT_SETHANDLE(ATT_HANDLE_GATT_CHARACTERISTIC_SERVICE_CHANGED_VALUE),     /* Characteristic UUID */
    0x04, /* Define the variable size*/
    0x00, 0x01, 0xFF, 0xFF, /* Variable Value define*/
#endif
};
#endif
typedef struct _BtATTDatabaseRecordHandle
{
    U16 record_hdl;
    U16 value_size;
    U8 *record_ptr;
    U8 *value_ptr;
    AttdbCallback cb; 
}BtATTDatabaseRecordHandle;

typedef struct _BtATTDatabaseGroupRecord
{
    U16 start_hdl;
    U16 end_hdl;
    U8 uuid[16];
}BtATTDatabaseGroupRecord;

BtATTDatabaseRecordHandle AttDatabaseHandle[ATT_MAX_RECORD];
BtATTDatabaseGroupRecord attGroupRecord[ATT_MAX_GROUP_RECORD];
U32 AttTotalRecordNum = 0;
U8 AttTotalGroupRecordNum = 0;
BtBleBondCbHandler AttDBBondNotification;

void ATTDB_DumpRecord(void);
BtStatus ATTBD_ParseRecord(U8 **data, BtATTDatabaseRecordHandle *record);


extern void HtsInitialize(void); /* For health thermometer service */
extern void DissInitialize(void);  /* For device info service */
extern void CtsInitialize(void);    /* For time service */
extern void NdcsInitialize(void);   /* For Next DST Change service */
extern void RtusInitialize(void);   /*For Reference Time Update Service */
extern void IasInitialize(void);
extern void LlsInitialize(void);
extern void TpsInitialize(void);


U16 AttDbGet16(const U8 *ptr)
{
    return (U16) (((U16) * (ptr) << 8) | ((U16) * (ptr + 1)));
}

void AttPromoteUUIDto32(const U8 *uuid, U16 len, U8 *buff)
{
    Assert((len == 2) || (len == 4));

    if (len == 2)
    {
        buff[0] = 0;
        buff[1] = 0;
        buff[2] = uuid[0];
        buff[3] = uuid[1];
    }
    else
    {
        Assert(len == 4);
        OS_MemCopy(buff, uuid, 4);
    }
}


BOOL AttUuidCmp(U8 *uuid1, U16 len1,U8 *uuid2, U16 len2)
{
    U8 temp[16];

    /* If same size then just compare */
    if (len1 == len2)
    {
        return StrnCmp(uuid1, len1, uuid2, len2);
    }

    /*
     * They are not the same size so verify that both of them
     * * are legal size for promotion
     */
    if ((len1 != 2) && (len1 != 16))
    {
        return FALSE;
    }

    if ((len2 != 2) && (len2 != 16))
    {
        return FALSE;
    }

    /* If neither one is 16 bytes then promote to 32 bits and compare */
    if ((len1 < 16) && (len2 < 16))
    {
        AttPromoteUUIDto32(uuid1, len1, temp);
        AttPromoteUUIDto32(uuid2, len2, temp + 4);
        return StrnCmp(temp, 4, temp + 4, 4);
    }

    /* At this point one of the entities is 16 bytes and one is not */
    OS_MemCopy(temp, (const U8*)BT_BASE_UUID, 16);

    /* If the first UUID is 16 bytes then promote the second to 16 and compare */
    if (len1 == 16)
    {
        AttPromoteUUIDto32(uuid2, len2, temp);
        return StrnCmp(uuid1, len1, temp, 16);
    }
    AttPromoteUUIDto32(uuid1, len1, temp);
    return StrnCmp(temp, 16, uuid2, len2);
}

U16 ATTDb_GetAttributeValueMaxSize(U8 *ptr)
{
    return (AttDbGet16(ptr+4));
}

void ATTDB_Init(void)
{
    AttTotalRecordNum = 0;
    AttTotalGroupRecordNum = 0;

    OS_MemSet((U8 *)&AttDatabaseHandle[0], 0x00, sizeof(BtATTDatabaseRecordHandle) * ATT_MAX_RECORD);
    OS_MemSet((U8 *)&attGroupRecord[0], 0x00, sizeof(BtATTDatabaseGroupRecord) * ATT_MAX_GROUP_RECORD);

#ifdef __ATT_TS_SMALL_DATABASE__
    ATTDB_AddRecordBQE(attBQESmallRecord, sizeof(attBQESmallRecord));

    ATTDB_AddGroupRecordBQE(0x01, 0x07, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x10, 0x10, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x20, 0x22, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);    
//	ATTDB_AddRecordCBBQE(0x23, database0023);
//	ATTDB_AddRecordCBBQE(0x24, database0024);   
   	
    //ATTDB_DumpRecord();
    InitializeListHead((ListEntry *)&(AttDBBondNotification.node));
    BleServiceDbInitialize();   /* For BLE service record database saved in file */
	gatt_sdp_init();
    return;
#endif
#ifdef __ATT_TS_LARGE_DATABASE1__
    ATTDB_AddRecordBQE(attBQELarge1Record, sizeof(attBQELarge1Record));

    ATTDB_AddGroupRecordBQE(0x0001, 0x0005, BT_UUID_GATT_TYPE_SECONDARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0010, 0x0012, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0020, 0x0028, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0030, 0x0032, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0040, 0x0046, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0050, 0x0052, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0060, 0x0062, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0070, 0x0076, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0080, 0x0082, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);    
    ATTDB_AddGroupRecordBQE(0x0090, 0x0096, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x00A0, 0x00AE, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);    
    ATTDB_AddRecordCBBQE(0x0012, databaseB006);

    //ATTDB_DumpRecord();
    InitializeListHead((ListEntry *)&(AttDBBondNotification.node));
    BleServiceDbInitialize();   /* For BLE service record database saved in file */
	gatt_sdp_init();

    return;

#endif

#ifdef __ATT_TS_LARGE_DATABASE2__
    ATTDB_AddRecordBQE(attBQELarge2Record, sizeof(attBQELarge2Record));

    ATTDB_AddGroupRecordBQE(0x0001, 0x000F, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0010, 0x0016, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0020, 0x0022, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0030, 0x0034, BT_UUID_GATT_TYPE_SECONDARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0040, 0x0042, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0050, 0x0052, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0060, 0x0068, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0070, 0x0072, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0080, 0x0086, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);    
    ATTDB_AddGroupRecordBQE(0x0090, 0x0096, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x00A0, 0x00A2, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);    
    ATTDB_AddGroupRecordBQE(0xfffd, 0xffff, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);    
    OS_Report("[ATT] ATTDB_AddRecordCB() addr: %08x ",databaseB006);
	
    ATTDB_AddRecordCBBQE(0x0072, databaseB006);

    //ATTDB_DumpRecord();
    InitializeListHead((ListEntry *)&(AttDBBondNotification.node));
    BleServiceDbInitialize();   /* For BLE service record database saved in file */
	gatt_sdp_init();

    return;

#endif
#ifdef __ATT_TS_LARGE_DATABASE3__
    ATTDB_AddRecordBQE(attBQELarge3Record, sizeof(attBQELarge3Record));

    ATTDB_AddGroupRecordBQE(0x0001, 0x0003, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0010, 0x0014, BT_UUID_GATT_TYPE_SECONDARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0020, 0x0022, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0030, 0x0032, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0040, 0x0042, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0050, 0x0056, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0060, 0x0068, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0070, 0x0076, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x0080, 0x0086, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);    
    ATTDB_AddGroupRecordBQE(0x0090, 0x0092, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_AddGroupRecordBQE(0x00A0, 0x00AE, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);    
    ATTDB_AddGroupRecordBQE(0xffff, 0xffff, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);  
    ATTDB_AddRecordCBBQE(0x0042, databaseB006);
	gatt_sdp_init();
    //ATTDB_DumpRecord();
    InitializeListHead((ListEntry *)&(AttDBBondNotification.node));
    BleServiceDbInitialize();   /* For BLE service record database saved in file */
    return;
#endif


#ifdef __GATT_SERVICE_CHANGED_CHARACTERISTIC_SUPPORT__
    ATTDB_AddRecord(attMainRecord, sizeof(attMainRecord));
    ATTDB_AddGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_GATT, 
                         ATT_HANDLE_PRIMARY_SERVICE_GATT_END, 
                         BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
    ATTDB_DumpRecord();
#endif
    InitializeListHead((ListEntry *)&(AttDBBondNotification.node));
    BleUtilsInitialize();

    BleServiceDbInitialize();   /* For BLE service record database saved in file */
#ifdef __BT_HTS_SERVICE__
    HtsInitialize(); /* For health thermometer service */
#endif
#ifdef __BT_HTPT_PROFILE__
    DissInitialize();  /* For device info service */
#endif
#ifdef __BT_TIMES_PROFILE__
    CtsInitialize();    /* For time service */
#endif
#ifdef __BT_TIMES_PROFILE__
    NdcsInitialize();   /* For Next DST Change service */
#endif
#ifdef __BT_TIMES_PROFILE__
    RtusInitialize();   /*For Reference Time Update Service */
#endif
    IasInitialize();
    LlsInitialize();
    TpsInitialize();
     
}



void ATTDB_RegisterBondStatus(BtBleBondCbHandler *cb_func)
{
    InitializeListEntry(&(cb_func->node)); 
    InsertTailList((ListEntry *)&(AttDBBondNotification.node), &(cb_func->node));
}

void ATTDB_UnRegisterBondStatus(BtBleBondCbHandler *cb_func)
{
    RemoveEntryList((ListEntry *)&(cb_func->node));

}

void ATTDB_NotifyBondStatus(const U8 *bd_addr, U8 status)
{

    ListEntry *entry;
    BtBleBondCbHandler *handler;

    entry = AttDBBondNotification.node.Flink;
    while (entry && entry != (ListEntry *)&(AttDBBondNotification.node))
    {
		handler = ((BtBleBondCbHandler *)entry);
        entry = entry->Flink;
        handler->callback(status, bd_addr);
    }
}


BtStatus ATTDB_AddRecord(const U8 *data, U16 size)
{
    U8 *ptr = (U8 *)data;
    BtATTDatabaseRecordHandle record;
    BtStatus status = BT_STATUS_SUCCESS;
#if defined (__ATT_TS_LARGE_DATABASE1__) || defined(__ATT_TS_LARGE_DATABASE2__) || defined(__ATT_TS_LARGE_DATABASE3__) || defined(__ATT_TS_SMALL_DATABASE1__)
    return BT_STATUS_FAILED;
#endif    
    while (ptr < data + size - 1)
    {
        if (ATTBD_ParseRecord(&ptr, &record) == BT_STATUS_SUCCESS)
        {
            OS_MemCopy((U8 *)&AttDatabaseHandle[AttTotalRecordNum], (U8 *)&record, sizeof(BtATTDatabaseRecordHandle));
            AttTotalRecordNum++;
            Assert(AttTotalRecordNum <= ATT_MAX_RECORD);
        }
        else
        {
            Assert(0);
        }
    }
    return status;
}

void ATTDB_AddRecordCB(U16 handle, AttdbCallback cb)
{
    U32 i;    
#if defined (__ATT_TS_LARGE_DATABASE1__) || defined(__ATT_TS_LARGE_DATABASE2__) || defined(__ATT_TS_LARGE_DATABASE3__) || defined(__ATT_TS_SMALL_DATABASE1__)
    return;
#endif    
    
    for (i = 0; i < AttTotalRecordNum; i++)
    {
        if (AttDatabaseHandle[i].record_hdl == handle)
        {
            AttDatabaseHandle[i].cb = cb;
            return;
        }
    }
    OS_Report("[ATT] ATTDB_AddRecordCB(): handle(%d) not found ", handle);
}

void ATTDB_RemoveRecordCB(U16 handle)
{
    U32 i;    
#if defined (__ATT_TS_LARGE_DATABASE1__) || defined(__ATT_TS_LARGE_DATABASE2__) || defined(__ATT_TS_LARGE_DATABASE3__) || defined(__ATT_TS_SMALL_DATABASE1__)
    return;
#endif    

    for (i = 0; i < AttTotalRecordNum; i++)
    {
        if (AttDatabaseHandle[i].record_hdl == handle)
        {
            AttDatabaseHandle[i].cb = 0;
            return;
        }
    }
}


BtStatus ATTDB_AddGroupRecord(U16 start_hdl, U16 end_hdl, U16 group_uuid)
{
    BtStatus status = BT_STATUS_SUCCESS;
    U8 buf[2];

#if defined (__ATT_TS_LARGE_DATABASE1__) || defined(__ATT_TS_LARGE_DATABASE2__) || defined(__ATT_TS_LARGE_DATABASE3__) || defined(__ATT_TS_SMALL_DATABASE1__)
    return BT_STATUS_FAILED;
#endif    

    OS_Report("[ATT] ATTDB_AddGroupRecord(): start(0x%x), end(0x%x), uuid(0x%x) ", start_hdl, end_hdl, group_uuid);
    Assert (AttTotalGroupRecordNum <= ATT_MAX_GROUP_RECORD);
    switch (group_uuid)
    {
    case BT_UUID_GATT_TYPE_PRIMARY_SERVICE:
    case BT_UUID_GATT_TYPE_SECONDARY_SERVICE:
        buf[0] = (U8)((group_uuid & 0xff00) >> 8);
        buf[1] = (U8)(group_uuid & 0x00ff);
        OS_MemCopy(attGroupRecord[AttTotalGroupRecordNum].uuid, (const U8*)BT_BASE_UUID, 16);
        AttPromoteUUIDto32(buf, BT_UUID16_SIZE, attGroupRecord[AttTotalGroupRecordNum].uuid);
        attGroupRecord[AttTotalGroupRecordNum].start_hdl = start_hdl;
        attGroupRecord[AttTotalGroupRecordNum].end_hdl = end_hdl;
        AttTotalGroupRecordNum++;
        break;
    default:
        OS_Report("[ATT] ATTDB_AddGroupRecord(): group_uuid(0x%x) not allowed", group_uuid);
        status = BT_STATUS_FAILED;
        break;
    }

    return status;
}

#if defined (__ATT_TS_LARGE_DATABASE1__) || defined(__ATT_TS_LARGE_DATABASE2__) || defined(__ATT_TS_LARGE_DATABASE3__) || defined(__ATT_TS_SMALL_DATABASE__)


BtStatus ATTDB_AddRecordBQE(const U8 *data, U16 size)
{
    U8 *ptr = (U8 *)data;
    BtATTDatabaseRecordHandle record;
    BtStatus status = BT_STATUS_SUCCESS;
    
    while (ptr < data + size - 1)
    {
		Report(("Att total record no:%d, max record:%d",AttTotalRecordNum,ATT_MAX_RECORD));    
        if (ATTBD_ParseRecord(&ptr, &record) == BT_STATUS_SUCCESS)
        {
            OS_MemCopy((U8 *)&AttDatabaseHandle[AttTotalRecordNum], (U8 *)&record, sizeof(BtATTDatabaseRecordHandle));
            AttTotalRecordNum++;
            Assert(AttTotalRecordNum <= ATT_MAX_RECORD);
        }
        else
        {
            Assert(0);
        }
    }
    return status;
}

void ATTDB_AddRecordCBBQE(U16 handle, AttdbCallback cb)
{
    int i;    
    for (i = 0; i < AttTotalRecordNum; i++)
    {
        if (AttDatabaseHandle[i].record_hdl == handle)
        {
            AttDatabaseHandle[i].cb = cb;
            return;
        }
    }
}

void ATTDB_RemoveRecordCBBQE(U16 handle)
{
    int i;    
    for (i = 0; i < AttTotalRecordNum; i++)
    {
        if (AttDatabaseHandle[i].record_hdl == handle)
        {
            AttDatabaseHandle[i].cb = 0;
            return;
        }
    }
}


BtStatus ATTDB_AddGroupRecordBQE(U16 start_hdl, U16 end_hdl, U16 group_uuid)
{
    BtStatus status = BT_STATUS_SUCCESS;
    U8 buf[2];

    OS_Report("[ATT] ATTDB_AddGroupRecord(): start(0x%x), end(0x%x), uuid(0x%x) ", start_hdl, end_hdl, group_uuid);
    Assert (AttTotalGroupRecordNum <= ATT_MAX_GROUP_RECORD);
    switch (group_uuid)
    {
    case BT_UUID_GATT_TYPE_PRIMARY_SERVICE:
    case BT_UUID_GATT_TYPE_SECONDARY_SERVICE:
        buf[0] = (U8)((group_uuid & 0xff00) >> 8);
        buf[1] = (U8)(group_uuid & 0x00ff);
        OS_MemCopy(attGroupRecord[AttTotalGroupRecordNum].uuid, (const U8*)BT_BASE_UUID, 16);
        AttPromoteUUIDto32(buf, BT_UUID16_SIZE, attGroupRecord[AttTotalGroupRecordNum].uuid);
        attGroupRecord[AttTotalGroupRecordNum].start_hdl = start_hdl;
        attGroupRecord[AttTotalGroupRecordNum].end_hdl = end_hdl;
        AttTotalGroupRecordNum++;
        break;
    default:
        OS_Report("[ATT] ATTDB_AddGroupRecord(): group_uuid(0x%x) not allowed", group_uuid);
        status = BT_STATUS_FAILED;
        break;
    }

    return status;
}
#endif

void ATTDB_DumpRecord(void)
{
    U32 j=0;
	U8 dumpsize, offset;

    Report(("[ATT]===DUMP ATT record==="));
    for(j=0;j<ATT_MAX_RECORD; j++)
    {
        Report(("[ATT]Record index:%d",j+1));        
        Report(("[ATT]Record record handle:%08x",AttDatabaseHandle[j].record_hdl));                    
        Report(("[ATT]Record record valueSize:%08x",AttDatabaseHandle[j].value_size));

        dumpsize = AttDatabaseHandle[j].value_size;
		offset = 0;
		while( dumpsize > offset ){
			if( (dumpsize-offset) >= 4 ){
				Report(("[ATT]Record record value:%02x,%02x,%02x,%02x,",
					AttDatabaseHandle[j].value_ptr[offset],
					AttDatabaseHandle[j].value_ptr[offset+1],
					AttDatabaseHandle[j].value_ptr[offset+2],
					AttDatabaseHandle[j].value_ptr[offset+3]
					));
                offset +=4;
			}				

			if( (dumpsize-offset) > 0 ){
				switch( (dumpsize-offset) ){
				case 1:
				Report(("[ATT]Record record value:%02x",
					AttDatabaseHandle[j].value_ptr[offset]
					));
				offset +=1;
					break;
				case 2:
				Report(("[ATT]Record record value:%02x,%02x",
					AttDatabaseHandle[j].value_ptr[offset],
					AttDatabaseHandle[j].value_ptr[offset+1]
					));
				offset +=2;
					break;
				case 3:
				Report(("[ATT]Record record value:%02x,%02x,%02x",
					AttDatabaseHandle[j].value_ptr[offset],
					AttDatabaseHandle[j].value_ptr[offset+1],
					AttDatabaseHandle[j].value_ptr[offset+2]
					));
				offset +=3;
					break;
				}
			}
		}
    }
}


/*****************************************************************************
 * FUNCTION
 *  ATTBD_ParseRecord
 * DESCRIPTION
 *  Parse an ATT record from raw data
 * PARAMETERS
 *  data    [IN/OUT]
 *  record  [OUT]
 * RETURNS
 *  BtStatus
 *****************************************************************************/
BtStatus ATTBD_ParseRecord(U8 **data, BtATTDatabaseRecordHandle *record)
{
    U16 record_size=0;
    U8 *read_ptr;
    U16 variableValueSize = 0;
    U8 fixVariable = 0;

    read_ptr = (U8 *)*data;
    record->record_ptr = read_ptr;
    record->record_hdl = AttDbGet16(read_ptr);
    variableValueSize = AttDbGet16(read_ptr + 4);
    fixVariable = read_ptr[3];
    record->cb = 0;

    if(read_ptr[2] == BT_UUID16_SIZE)
    {
        /*UUID type 2 bytes*/
        record_size = 8 + 3 + variableValueSize;
    }
    else if(read_ptr[2] == BT_UUID128_SIZE)
    {
        /*UUID type 16 bytes*/
        record_size = 22 + 3 + variableValueSize;
    }
    else
    {
        Assert(0);
    }

	if( read_ptr[6] == 0x28 && read_ptr[7] == 0x00 ){ // BT_UUID_GATT_TYPE_PRIMARY_SERVICE
		Report(("[ATT] Start a primary service"));
	}

    record->value_size = variableValueSize;
    /* Group inside base record handle */
    if (fixVariable == BT_ATT_CONST_VARIABLE)
    {
        record->value_ptr = read_ptr + ((read_ptr[2] == BT_UUID16_SIZE) ? 11 : 25);
    }
    else if(fixVariable == BT_ATT_FIX_VARIABLE_IN_UPPER_AP)
    {
        record->value_ptr = NULL;  /* will be assign in callback */
        record_size -= variableValueSize;
    }
    else
    {
        U16 offset = 0, storage_size = sizeof(attBaseRecordHandeVariableValueArray);
        while (offset < storage_size)
        {
            if (read_ptr[2] == attBaseRecordHandeVariableValueArray[offset] &&
                OS_MemCmp(&read_ptr[0], read_ptr[2], &attBaseRecordHandeVariableValueArray[offset + 1], read_ptr[2]))
            {
                Report(("Dlight check match of handle:%02x, %02x", read_ptr[0], read_ptr[1]));
                record->value_ptr = attBaseRecordHandeVariableValueArray + offset + read_ptr[2] + 2;
                break;
            }
            offset += (attBaseRecordHandeVariableValueArray[offset] + 1);
            offset += (attBaseRecordHandeVariableValueArray[offset] + 1);
        }
        record_size -= variableValueSize;
        
    }

    *data = *data + record_size;
    Report(("[ATT]record size: %d", record_size));
    Report(("[ATT]record value type: %0d", fixVariable));
    Report(("[ATT]variableValueSize: %d", variableValueSize));
    return BT_STATUS_SUCCESS;
}

BtStatus ATTDB_RemoveRecord(U16 start_hdl, U16 end_hdl)
{
    U32 i, idx, count = 0;
    int size = sizeof(BtATTDatabaseRecordHandle);
    for (i = 0, idx = 0; i < AttTotalRecordNum; i++)
    {
        if (AttDatabaseHandle[idx].record_hdl >= start_hdl && AttDatabaseHandle[idx].record_hdl <= end_hdl)
        {
            count++;
            OS_MemCopy((U8 *)&AttDatabaseHandle[idx], (U8 *)&AttDatabaseHandle[idx + 1], size * (AttTotalRecordNum - i - 1));
            OS_MemSet((U8 *)&AttDatabaseHandle[AttTotalRecordNum - count], 0x00, size);
        }
        else
        {
            idx++;
        }
    }

    AttTotalRecordNum -= count;
    return BT_STATUS_SUCCESS;
}

BtStatus ATTDB_RemoveGroupRecord(U16 start_hdl, U16 end_hdl)
{
    int i;
    int size = sizeof(BtATTDatabaseGroupRecord);
    for (i = 0; i < AttTotalGroupRecordNum; i++)
    {
        if (attGroupRecord[i].start_hdl == start_hdl && attGroupRecord[i].end_hdl == end_hdl)
        {
            OS_MemCopy((U8 *)&attGroupRecord[i], (U8 *)&attGroupRecord[i + 1], size * (AttTotalGroupRecordNum - i - 1));
            OS_MemSet((U8 *)&attGroupRecord[AttTotalGroupRecordNum - 1], 0x00, size);
            AttTotalGroupRecordNum--;
            break;
        }
    }

    return BT_STATUS_SUCCESS;
}

/* Type == 0x00, get group start */
/* Type == 0x01, get group end */
U16 ATTDb_GetGroupStartEndHandle(U16 handle, U8 type)
{
    U16 i = 0;

    for(i = 0; i < AttTotalGroupRecordNum; i++)
    {
        if((attGroupRecord[i].start_hdl <= handle) && (attGroupRecord[i].end_hdl >= handle))
        {
            if(type == 0x00)
                return attGroupRecord[i].start_hdl;
            else
                return attGroupRecord[i].end_hdl;
        }
    }
    
    return 0x00;
}

U8 ATTDb_CheckGattDefinedPermission(bt_att_op op, bt_gatt_char_properties prop)
{
    U8 mask = 0;

    Report(("ATTDb_CheckGattDefinedPermission: op=0x%x, prop=0x%x", op, prop));
    switch (op)
    {
    case ATT_OP_BROADCAST:
        mask = (prop & GATT_CHAR_PROP_BROADCAST);
        break;
    case ATT_OP_READ:
    case ATT_OP_READ_LONG:
    case ATT_OP_READ_MULTIPLE:
    case ATT_OP_READ_BY_TYPE:        
        mask = (prop & GATT_CHAR_PROP_READ);
        if (mask == 0)
        {
            return BT_ATT_ERROR_CODE_READ_NOT_PERMITTED;
        }
        break;
    case ATT_OP_WRITE_WO_RESPONSE:
        mask = (prop & GATT_CHAR_PROP_WRITE_WO_RESPONSE);
        if (mask == 0)
        {
            return BT_ATT_ERROR_CODE_WRITE_NOT_PERMITTED;
        }
        break;
    case ATT_OP_WRITE:
    case ATT_OP_EXECUTE_WRITE:
        mask = (prop & GATT_CHAR_PROP_WRITE);
        if (mask == 0)
        {
            return BT_ATT_ERROR_CODE_WRITE_NOT_PERMITTED;
        }
        break;
    case ATT_OP_NOTIFY:
        mask = (prop & GATT_CHAR_PROP_NOTIFY);
        break;
    case ATT_OP_INDICATE:
        mask = (prop & GATT_CHAR_PROP_INDICATE);
        break;
    case ATT_OP_SIGNED_WRITE:
        mask = (prop & GATT_CHAR_PROP_SIGNED_WRITE);
        break;
    default:
        Assert(0);
    }

    return ((mask) ? BT_ATT_ERROR_CODE_NO_ERROR : BT_ATT_ERROR_CODE_UNLIKELY_ERROR);
}

U8 ATTDb_CheckHandlePermission(BtRemoteDevice *link, U16 recordHandle, bt_att_op op)
{
    U8 result = BT_ATT_ERROR_CODE_NO_ERROR;
    U32 i=0;
    U8 read_attribute_offset = 8;
    U8 recordUuidType;
    bt_gatt_char_properties prop;
    U8 securityFlagWrite;
    U8 securityFlagRead;

    if(recordHandle == 0x00)
        return BT_ATT_ERROR_CODE_INVALID_HANDLE;
    
    for(i = 0; i< ATT_MAX_RECORD; i++)
    {
        if(recordHandle == AttDatabaseHandle[i].record_hdl)
        {
            break;
        }
    }

    if(i == ATT_MAX_RECORD)
    {
        result = BT_ATT_ERROR_CODE_INVALID_HANDLE;
    }
    else
    {
        recordUuidType = AttDatabaseHandle[i].record_ptr[2];
        if(recordUuidType == BT_UUID128_SIZE)
        {
            read_attribute_offset = 0x08 + BT_UUID128_SIZE - BT_UUID16_SIZE; // UUID 16 - UUID2 
        }      
        securityFlagWrite = AttDatabaseHandle[i].record_ptr[read_attribute_offset+2];
        securityFlagRead = AttDatabaseHandle[i].record_ptr[read_attribute_offset+1];        

        switch (op)
        {
            case ATT_OP_READ:
            case ATT_OP_READ_LONG:
            case ATT_OP_READ_MULTIPLE:
            case ATT_OP_READ_BY_TYPE:
                switch (securityFlagRead)
                {
                    case ATT_SECURITY_READ_NOT_REQUIRED:
                        break;
                    case ATT_SECURITY_READ_AUTHENTICATION:
                        if(link->authState != BAS_AUTHENTICATED)
                        {
                                result = BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE; 
                            //                            result = BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
                        }
                        goto check_done;
                    default:
                        break;
                }

            case ATT_OP_WRITE_WO_RESPONSE:
            case ATT_OP_WRITE:
            case ATT_OP_EXECUTE_WRITE:
                switch (securityFlagWrite)
                {
                    case ATT_SECURITY_WRITE_NOT_REQUIRED:
                        break;
                    case ATT_SECURITY_WRITE_AUTHENTICATION:
                        if(link->authState != BAS_AUTHENTICATED)
                        {
                            result = BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE; 
                        //                            result = BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 

                        }
                        goto check_done;
                    default:
                        break;
                }
                break;
                default:
                    return BT_ATT_ERROR_CODE_REQUEST_NOT_SUPPORTED;
        }
    
        switch (AttDatabaseHandle[i].record_ptr[read_attribute_offset])
        {
        case ATT_PERMISSIONS_READ_ONLY_BLE_ONLY:
            if(ME_GetDeviceType(&link->bdAddr) != BT_DEV_TYPE_LE)
            {
                result = BT_ATT_ERROR_CODE_INVALID_HANDLE;
                break;
            }
            /* Continue check */
        case ATT_PERMISSIONS_READ_ONLY:
            if (op != ATT_OP_READ &&
                op != ATT_OP_READ_LONG &&
                op != ATT_OP_READ_MULTIPLE &&
                op != ATT_OP_READ_BY_TYPE)
            {
                if (op == ATT_OP_WRITE || op == ATT_OP_WRITE_WO_RESPONSE || op == ATT_OP_EXECUTE_WRITE)
                {
                    result = BT_ATT_ERROR_CODE_WRITE_NOT_PERMITTED;
                }
                else
                {
                    result = BT_ATT_ERROR_CODE_UNLIKELY_ERROR;
                }
            }
            break;
        case ATT_PERMISSIONS_WRITE_ONLY_BLE_ONLY:
            if(ME_GetDeviceType(&link->bdAddr) != BT_DEV_TYPE_LE)
            {
                result = BT_ATT_ERROR_CODE_INVALID_HANDLE;
                break;
            }
            /* Continue check */
        case ATT_PERMISSIONS_WRITE_ONLY:
            if (op != ATT_OP_WRITE && op != ATT_OP_WRITE_WO_RESPONSE && op != ATT_OP_EXECUTE_WRITE)
            {
                if (op == ATT_OP_READ || op == ATT_OP_READ_LONG || op == ATT_OP_READ_MULTIPLE || op == ATT_OP_READ_BY_TYPE)
                {
                    result = BT_ATT_ERROR_CODE_READ_NOT_PERMITTED;    
                }
                else
                {
                    result = BT_ATT_ERROR_CODE_UNLIKELY_ERROR;
                }
            }
            break;
        case ATT_PERMISSIONS_READ_WRITE_BLE_ONLY:
            if(ME_GetDeviceType(&link->bdAddr) != BT_DEV_TYPE_LE)
            {
                result = BT_ATT_ERROR_CODE_INVALID_HANDLE;
                break;
            }
            /* Continue check */
        case ATT_PERMISSIONS_READ_WRITE:
            break;
        case ATT_PERMISSIONS_NO_READ_WRITE:
            result = BT_ATT_ERROR_CODE_REQUEST_NOT_SUPPORTED;
            break;
        case ATT_PERMISSIONS_GATT_DEFINED_BLE_ONLY:
            if(ME_GetDeviceType(&link->bdAddr) != BT_DEV_TYPE_LE)
            {
                result = BT_ATT_ERROR_CODE_INVALID_HANDLE;
                break;
            }
            /* Continue check */
        case ATT_PERMISSIONS_GATT_DEFINED:
            if (i > 0)
            {
                /* Characteristic declaration */
                if (AttDatabaseHandle[i - 1].record_ptr[2] == BT_UUID16_SIZE)
                {
                    prop = AttDatabaseHandle[i - 1].record_ptr[11];
                }
                else
                {
                    prop = AttDatabaseHandle[i - 1].record_ptr[25];
                }
                result = ATTDb_CheckGattDefinedPermission(op, prop);
            }
            break;
        }
    }

check_done:
    OS_Report("ATTDb_CheckHandlePermission: result=0x%x, recordHandle=0x%x, op=%d", result, recordHandle, op);
    return result;
}

U16 ATTDb_CheckHandleSize(U16 recordHandle)
{
    U32 i=0;
    U8 read_attribute_offset = 8;
    U8 recordUuidType;

    
    for(i = 0; i< ATT_MAX_RECORD; i++)
    {
        if(recordHandle == AttDatabaseHandle[i].record_hdl)
        {
            break;
        }
    }
    if(i == ATT_MAX_RECORD)
    {
        return 0xFFFF;
    }
    return AttDatabaseHandle[i].value_size;

}


U16 ATTDb_HandleGetInfoReq(U8 *data, U16 mtu, U8 *format, BtATTFindInfoRequestStruct *ptr)
{
    U32 i=0;
    U16 writeOffset=0;
    U8 uuidType = 0;
    U8 recordUuidType;
    
    for(i = 0; i< ATT_MAX_RECORD; i++)
    {
        if((ptr->startingHandle <= AttDatabaseHandle[i].record_hdl) &&
            (ptr->endingHandle >= AttDatabaseHandle[i].record_hdl))
        {
            /* Meet requirements */
            recordUuidType = AttDatabaseHandle[i].record_ptr[2];
            if(recordUuidType == BT_UUID16_SIZE)
            {
                if((uuidType !=0) && (uuidType != recordUuidType))
                {
                    /* Stop here, UUID not match. */
                    break;
                }
                uuidType = BT_UUID16_SIZE;
                if(writeOffset + 2 + BT_UUID16_SIZE > mtu)
                {
                    /* Can not write to buffer */
                    break;
                }
                StoreLE16(data + writeOffset, AttDbGet16(AttDatabaseHandle[i].record_ptr));
                StoreLE16(data + writeOffset + 2, AttDbGet16(AttDatabaseHandle[i].record_ptr + 6));
                writeOffset += (2 + BT_UUID16_SIZE);
            }
            else if(recordUuidType == BT_UUID128_SIZE)
            {
                if((uuidType !=0) && (uuidType != recordUuidType))
                {
                    /* Stop here, UUID not match. */
                    break;
                }
                uuidType = BT_UUID128_SIZE;
                if(writeOffset + 2 + BT_UUID128_SIZE > mtu)
                {
                    break;
                }
                StoreLE16(data + writeOffset, AttDbGet16(AttDatabaseHandle[i].record_ptr));
                memcpy(data + writeOffset + 2, AttDatabaseHandle[i].record_ptr + 6, BT_UUID128_SIZE);
                writeOffset += (2 + BT_UUID128_SIZE);
            }
            else
            {
                Assert(0);
            }
            
        }
        else if(ptr->startingHandle < AttDatabaseHandle[i].record_hdl)
        {
            /* Continue search */
        }
        else if(ptr->endingHandle < AttDatabaseHandle[i].record_hdl)
        {
            /* Search done */
            break;
        }
    }
    if(uuidType == 0x02)
        *format = 0x01;
    else
        *format = 0x02;
    return writeOffset;
}

U16 ATTDb_HandleGetFindByTypeValueReq(U8 *data, U16 mtu, BtATTFindByTypeValueRequestStruct *ptr, BtRemoteDevice *link)
{
    U32 i=0;
    U16 writeOffset=0;
    U8 uuidType = 0;
    U8 recordUuidType = 0;
    U16 uuid;
    U16 groudEndHandler;
    U16 currentWriteLen = 0;
    U16 read_attribute_offset = 8;
    Report(("ptr->startingHandle:%08x", ptr->startingHandle));
    Report(("ptr->endingHandle:%08x", ptr->endingHandle));
    
    for(i = 0; i< ATT_MAX_RECORD; i++)
    {
        if((ptr->startingHandle <= AttDatabaseHandle[i].record_hdl) &&
            (ptr->endingHandle >= AttDatabaseHandle[i].record_hdl))
        {
            Report(("AttDatabaseHandle[i].record_hdl:%08x", AttDatabaseHandle[i].record_hdl));
            /* Meet requirements */
            if (ATTDb_CheckHandlePermission(link, AttDatabaseHandle[i].record_hdl, ATT_OP_READ) != BT_ATT_ERROR_CODE_NO_ERROR)
            {
                continue;
            }
            recordUuidType = AttDatabaseHandle[i].record_ptr[2];
            Report(("recordUuidType%08x", recordUuidType));

            if(writeOffset + 4 > mtu)
            {
                /* Can not write to buffer */
                Report(("can not write"));
                break;
            }
            if(recordUuidType != BT_UUID16_SIZE)
            {
                Report(("recordUuidType not 0x02"));
                continue;
            }
            Report(("ptr->attributeType %02x",ptr->attributeType));

            if(AttDbGet16(AttDatabaseHandle[i].record_ptr + 6) != ptr->attributeType)
            {
                Report(("continue"));
                continue;
            }
            Report(("ptr->attributeValueLen:%d",ptr->attributeValueLen));
            Report(("AttDatabaseHandle[i].value_size:%d",AttDatabaseHandle[i].value_size));

            if(AttDatabaseHandle[i].cb !=0)
            {
                AttDatabaseHandle[i].cb(ATT_OP_READ, link, AttDatabaseHandle[i].record_hdl, &AttDatabaseHandle[i].value_size, &AttDatabaseHandle[i].value_ptr);
            }
            //if(StrnCmp(ptr->attributeValue, ptr->attributeValueLen ,
            //    AttDatabaseHandle[i].value_ptr, AttDatabaseHandle[i].value_size) == FALSE)
            if(BLE_CompareUuid(ptr->attributeValue, ptr->attributeValueLen ,
                AttDatabaseHandle[i].value_ptr, AttDatabaseHandle[i].value_size) == FALSE)
            {
                Report(("match result failed"));
                continue;
            }
#if 0
           recordUuidType = AttDatabaseHandle[i].record_ptr[2];
           Report(("recordUuidType:%d",recordUuidType));

            if(recordUuidType == 0x10)
            {
                read_attribute_offset = 0x08+14; // UUID 16 - UUID2 
            }
#endif
            groudEndHandler = ATTDb_GetGroupStartEndHandle(AttDatabaseHandle[i].record_hdl, 1);
            Report(("TING Zheng:%04x",AttDatabaseHandle[i].record_hdl));
            Report(("TING Zheng1:%04x",groudEndHandler));            
//            memcpy(data+writeOffset, AttDatabaseHandle[i].record_ptr , 2);
            StoreLE16(data+writeOffset,AttDbGet16(AttDatabaseHandle[i].record_ptr));

            if(groudEndHandler == 0x00)
            {
            Report(("TING Zheng2:%04x",AttDbGet16(AttDatabaseHandle[i].record_ptr)));            
                /* Record handle not in group field */
                StoreLE16(data+writeOffset+2,AttDbGet16(AttDatabaseHandle[i].record_ptr));

            }
            else
            {
                Report(("TING Zheng32:%04x",groudEndHandler));

                StoreLE16(data+writeOffset+2,groudEndHandler);
                /* Record handle in group field */
            }
            writeOffset+=4;
        }
        else if(ptr->startingHandle < AttDatabaseHandle[i].record_hdl)
        {
            /* Continue search */
        }
        else if(ptr->endingHandle < AttDatabaseHandle[i].record_hdl)
        {
            /* Search done */
        }
    }
    return writeOffset;
}


U16 ATTDb_HandleReadByTypeReq(U8 *data, U16 mtu, BtATTReadByTypeRequestStruct *ptr, /*U8 *attribute_pair_len, */BtRemoteDevice *link, U8 *error_code, U16 *error_hdl)
{
    U32 i=0;
    U16 writeOffset=0;
    U8 uuidType = 0;
    U8 recordUuidType = 0;
    U8 availableWriteSize =0;
    U8 uuid[2];
    U16 currentWriteLen = 0;
    U16 read_attribute_offset = 8;
    BtDeviceContext *bdc = DS_FindDevice(&link->bdAddr);
    
    *error_hdl = ptr->startingHandle;
    *error_code = BT_ATT_ERROR_CODE_ATTRIBUTE_NOT_FOUND;

    if(ptr->attrPairLen >= 2){
        currentWriteLen = ptr->attrPairLen - 2;
    }

    for(i = ptr->currHandle; i< ATT_MAX_RECORD; i++, ptr->currHandle++)
    {
        if((ptr->startingHandle <= AttDatabaseHandle[i].record_hdl) &&
           (ptr->endingHandle >= AttDatabaseHandle[i].record_hdl))
        {
            U8 err;

            /* Meet requirements */
            recordUuidType = AttDatabaseHandle[i].record_ptr[2];
            if(recordUuidType == BT_UUID128_SIZE)
            {
                read_attribute_offset = 0x08 + BT_UUID128_SIZE - BT_UUID16_SIZE; // UUID 16 - UUID2 
            }
            if(ptr->type == BT_UUID16_SIZE)
            {
                uuid[1] = (U8)((ptr->attributeType2 & 0x00ff) );
                uuid[0] = (U8)((ptr->attributeType2 & 0xff00) >> 8);
                if (AttUuidCmp(AttDatabaseHandle[i].record_ptr + 6, 
                                recordUuidType,
                                uuid, BT_UUID16_SIZE) == FALSE)
                {
                    continue;
                }
            }
            else
            {
                if (AttUuidCmp(AttDatabaseHandle[i].record_ptr + 6, 
                                recordUuidType,
                                ptr->attributeType, BT_UUID128_SIZE) == FALSE)
                {
                    continue;
                }
            }
            /* Check permission */
            if ((err = ATTDb_CheckHandlePermission(link, AttDatabaseHandle[i].record_hdl, ATT_OP_READ)) != BT_ATT_ERROR_CODE_NO_ERROR)
            {
                /* Read only or Read/Write*/
                *error_hdl = AttDatabaseHandle[i].record_hdl;
                *error_code = err;
                continue;
            }
            
            if(writeOffset + 2 > mtu)
            {
                /* Can not write to buffer */
                break;
            }            
            if((mtu- writeOffset) <= 2)
            {
                /* Unable to fill a complete format*/
                break;
            }
            if(AttDatabaseHandle[i].cb !=0)
            {
                *error_code = AttDatabaseHandle[i].cb(ATT_OP_READ_BY_TYPE, link, AttDatabaseHandle[i].record_hdl, &AttDatabaseHandle[i].value_size, &AttDatabaseHandle[i].value_ptr);
                switch(*error_code){
                case BT_ATT_ERROR_CODE_NO_ERROR:
                    break;
                case BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE:
                    return writeOffset;
                default:
                    return 0;
                }
            }            
            if((currentWriteLen != 0) &&
                (AttDatabaseHandle[i].value_size != currentWriteLen))
            {
                break;
            }
            currentWriteLen = AttDatabaseHandle[i].value_size;
            ptr->attrPairLen = currentWriteLen+2;
            if(mtu - writeOffset > (AttDatabaseHandle[i].value_size + 2))
            {
                availableWriteSize = AttDatabaseHandle[i].value_size + 2;
            }
            else
            {
                //availableWriteSize = mtu - writeOffset;
                break;
            }
            StoreLE16(data+writeOffset, AttDbGet16(AttDatabaseHandle[i].record_ptr));
            if(availableWriteSize > 2){
                memcpy(data+writeOffset + 2, AttDatabaseHandle[i].value_ptr, availableWriteSize - 2);            
            }
#if 0            
            if(availableWriteSize == 2)
            {
                StoreLE16(data+writeOffset, AttDbGet16(AttDatabaseHandle[i].record_ptr));
            }
            else if(availableWriteSize > 2)
            {
                StoreLE16(data+writeOffset, AttDbGet16(AttDatabaseHandle[i].record_ptr));
                memcpy(data+writeOffset + 2, AttDatabaseHandle[i].value_ptr, availableWriteSize - 2);
            }
            else if(availableWriteSize == 1)
            {
                data[writeOffset] = AttDatabaseHandle[i].record_ptr[1];
            }
            else if(availableWriteSize == 0)
            {
                break;
            }
#endif
            writeOffset += availableWriteSize;

        }
        else if(ptr->startingHandle < AttDatabaseHandle[i].record_hdl)
        {
            /* Continue search */
        }
        else if(ptr->endingHandle < AttDatabaseHandle[i].record_hdl)
        {
            /* Search done */
        }
    }
    //*attribute_pair_len = currentWriteLen+2;
    return writeOffset;
}

U16 ATTDb_HandleReadReq(U8 *data, U16 mtu, BtATTReadRequestStruct *ptr, U8 *error_code, BtRemoteDevice *link)
{
    U32 i=0;
    U8 availableWriteSize =0;
    U8 read_attribute_offset = 8;
    U8 recordUuidType;

    for(i = 0; i< ATT_MAX_RECORD; i++)
    {
        if(ptr->handle == AttDatabaseHandle[i].record_hdl)
        {
            U8 err;
           
            if ((err = ATTDb_CheckHandlePermission(link, AttDatabaseHandle[i].record_hdl, ATT_OP_READ)) != BT_ATT_ERROR_CODE_NO_ERROR)
            {
                /* Read only or Read/Write*/
                *error_code = err;
                return 0;
            }
            if(AttDatabaseHandle[i].cb !=0)
            {
                *error_code = AttDatabaseHandle[i].cb(ATT_OP_READ, link, AttDatabaseHandle[i].record_hdl, &AttDatabaseHandle[i].value_size, &AttDatabaseHandle[i].value_ptr);
                switch(*error_code){
                case BT_ATT_ERROR_CODE_NO_ERROR:
                    break;
                case BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE:
                default:
                    return 0;
                }                
            }          
            recordUuidType = AttDatabaseHandle[i].record_ptr[2];
            if(recordUuidType == BT_UUID128_SIZE)
            {
                read_attribute_offset = 0x08 + BT_UUID128_SIZE - BT_UUID16_SIZE; // UUID 16 - UUID2 
            }

            if(mtu > AttDatabaseHandle[i].value_size)
            {
                availableWriteSize = AttDatabaseHandle[i].value_size;
            }
            else
            {
                availableWriteSize = mtu;
            }
            if(availableWriteSize !=0)
            {
                memcpy(data, AttDatabaseHandle[i].value_ptr, availableWriteSize);
            }
            *error_code = BT_ATT_ERROR_CODE_NO_ERROR;
            break;
        }
    }
    if((i == ATT_MAX_RECORD) && (availableWriteSize ==0))
    {
        *error_code = BT_ATT_ERROR_CODE_INVALID_HANDLE;
    }
    return availableWriteSize;
}


U16 ATTDb_HandleReadBlobReq(U8 *data, U16 mtu, BtATTReadBlobRequestStruct *ptr, U8 *error_code, BtRemoteDevice *link)
{
    U32 i=0;
    U8 availableWriteSize =0;
    U8 read_attribute_offset = 8;
    U8 recordUuidType;

    for(i = 0; i< ATT_MAX_RECORD; i++)
    {
        if(ptr->attributeHandle == AttDatabaseHandle[i].record_hdl)
        {
            U8 err;
            
            if ((err = ATTDb_CheckHandlePermission(link, AttDatabaseHandle[i].record_hdl, ATT_OP_READ)) != BT_ATT_ERROR_CODE_NO_ERROR)
            {
                /* Read only or Read/Write*/
                *error_code = err;
                return 0;
            }
            if(AttDatabaseHandle[i].cb !=0)
            {
                *error_code = AttDatabaseHandle[i].cb(ATT_OP_READ_LONG, link, AttDatabaseHandle[i].record_hdl, &AttDatabaseHandle[i].value_size, &AttDatabaseHandle[i].value_ptr);
                switch(*error_code){
                case BT_ATT_ERROR_CODE_NO_ERROR:
                    break;
                case BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE:
                default:
                    return 0;
                }                
            }
        	if((AttDatabaseHandle[i].record_ptr[3] != BT_ATT_NON_FIX_VARIABLE) &&
                (AttDatabaseHandle[i].value_size <= (mtu-3)))
            {
                *error_code = BT_ATT_ERROR_CODE_ATTRIBUTE_NOT_LONG;
                return 0;
            }
            recordUuidType = AttDatabaseHandle[i].record_ptr[2];
            if(recordUuidType == BT_UUID128_SIZE)
            {
                read_attribute_offset = 0x08 + BT_UUID128_SIZE - BT_UUID16_SIZE; // UUID 16 - UUID2 
            }
            
            if(ptr->valueOffset == AttDatabaseHandle[i].value_size)
            {
                *error_code = BT_ATT_ERROR_CODE_NO_ERROR;
                return 0;
            }
            if(ptr->valueOffset >  AttDatabaseHandle[i].value_size)
            {
                *error_code = BT_ATT_ERROR_CODE_INVALID_OFFSET;
                return 0;
            }
          

            if(mtu > (AttDatabaseHandle[i].value_size - ptr->valueOffset))
            {
                availableWriteSize = AttDatabaseHandle[i].value_size - ptr->valueOffset;
            }
            else
            {
                availableWriteSize = mtu;
            }
            memcpy(data, AttDatabaseHandle[i].value_ptr + ptr->valueOffset, availableWriteSize);
            break;
        }
    }
    if((i == ATT_MAX_RECORD) && (availableWriteSize ==0))
    {
        *error_code = BT_ATT_ERROR_CODE_INVALID_HANDLE;
    }
    return availableWriteSize;
}



U16 ATTDb_HandleReadMultipleReq(U8 *data, U16 mtu, BtATTReadMultipleRequestStruct *ptr, U8 *error_code, U16 *error_hdl, BtRemoteDevice *link)
{
    U32 i=0;
    U32 j=(ptr->currHandle-ptr->setOfHandles)/2;
    U8 availableWriteSize =0;
    U16 recordHandle = 0;
    U16 writeOffset=0;
    U8 read_attribute_offset = 8;
    U8 recordUuidType;

    for(; j< ptr->setOfHandlesLen/2; j++, ptr->currHandle+=2)
    {
        recordHandle = LEtoHost16(ptr->currHandle);
        *error_hdl = recordHandle;
     
        for(i = 0; i< ATT_MAX_RECORD; i++)
        {
            if(recordHandle == AttDatabaseHandle[i].record_hdl)
            {
                U8 err;

                if ((err = ATTDb_CheckHandlePermission(link, recordHandle, ATT_OP_READ)) != BT_ATT_ERROR_CODE_NO_ERROR)
                {
                    /* Read only or Read/Write*/
                    *error_hdl = recordHandle;
                    *error_code = err;
                    return 0;
                }
                if(AttDatabaseHandle[i].cb !=0)
                {
                    *error_code = AttDatabaseHandle[i].cb(ATT_OP_READ_MULTIPLE, link, AttDatabaseHandle[i].record_hdl, &AttDatabaseHandle[i].value_size, &AttDatabaseHandle[i].value_ptr);
                    switch(*error_code){
                    case BT_ATT_ERROR_CODE_NO_ERROR:
                        break;
                    case BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE:
                        return writeOffset;
                    default:
                        return 0;
                    }
                }  
                recordUuidType = AttDatabaseHandle[i].record_ptr[2];
                if(recordUuidType == BT_UUID128_SIZE)
                {
                    read_attribute_offset = 0x08 + BT_UUID128_SIZE - BT_UUID16_SIZE; // UUID 16 - UUID2 
                }                
                if(mtu - writeOffset > (AttDatabaseHandle[i].value_size))
                {
                    availableWriteSize = AttDatabaseHandle[i].value_size;
                }
                else
                {
                    availableWriteSize = mtu - writeOffset;
                }
                memcpy(data+writeOffset, AttDatabaseHandle[i].value_ptr, availableWriteSize);
                writeOffset += availableWriteSize;
                if(writeOffset >= mtu)
                {
                    *error_code = BT_ATT_ERROR_CODE_NO_ERROR;
                    return writeOffset;
                }
                break;
            }
        }
        if(i == ATT_MAX_RECORD)
        {
            Report(("[ATT]Record:%04x not found",recordHandle));
            *error_hdl = recordHandle;
            *error_code = BT_ATT_ERROR_CODE_INVALID_HANDLE;
            return 0;
        }
    }
    *error_code = BT_ATT_ERROR_CODE_NO_ERROR;
    return writeOffset;
}






U16 ATTDb_HandleReadGroupReq(U8 *data, U16 mtu, BtATTReadByGroupTypeRequestStruct *ptr, U8 *error_code, U16 *error_hdl, BtRemoteDevice *link)
{
    U32 i=0;
    U32 j=0;
    U8 availableWriteSize =0;
    U16 writeOffset=0;
    U16 groupStartHandle = 0;
    U16 groupEndHandle = 0;
    U8 uuid[2];
    U8 uuidCompareResult = 0;
    U8 attributeSize = 0;
    U8 read_attribute_offset = 8;
    U8 recordUuidType;
    
    for(i = 0; i < AttTotalGroupRecordNum; i++)
    {
        if(ptr->type == BT_UUID16_SIZE)
        {
            uuid[0] = (U8)((ptr->attributeType2 & 0xff00) >> 8);
            uuid[1] = (U8)(ptr->attributeType2 & 0x00ff);                
            if(AttUuidCmp(uuid, BT_UUID16_SIZE, attGroupRecord[i].uuid, BT_UUID128_SIZE) == TRUE)
            {
                uuidCompareResult = 1;
            }
        }
        else
        {
            if(AttUuidCmp(ptr->attributeType, BT_UUID128_SIZE, attGroupRecord[i].uuid, BT_UUID128_SIZE) == TRUE)
            {
                uuidCompareResult = 1;
            }
        }
        if(uuidCompareResult == 1)
        {
            groupStartHandle = attGroupRecord[i].start_hdl;
            groupEndHandle = attGroupRecord[i].end_hdl;
            if((ptr->startingHandle <= groupStartHandle) &&
                ((ptr->endingHandle <= groupEndHandle)))
            {
                ptr->startingHandle = groupStartHandle;
                break;
            }
            else if((ptr->startingHandle <= groupStartHandle) &&
                ((ptr->endingHandle > groupEndHandle)))
            {
                ptr->startingHandle = groupStartHandle;
                ptr->endingHandle = groupEndHandle;
                break;
            }
            else if((ptr->startingHandle > groupStartHandle) &&
                ((ptr->endingHandle > groupEndHandle)))
            {
                /* Not possible */
                uuidCompareResult = 0;

            }
            else if((ptr->startingHandle > groupStartHandle) &&
                ((ptr->endingHandle <= groupEndHandle)))
            {
                /* Do nothing*/
                break;
            }
        }
    }
    if(uuidCompareResult == 0)
    {
        *error_code = BT_ATT_ERROR_CODE_ATTRIBUTE_NOT_FOUND;
        return 0x00;
    }
    writeOffset = 1;

    for(i = 0; i< ATT_MAX_RECORD; i++)
    {
        if((ptr->startingHandle <= AttDatabaseHandle[i].record_hdl) &&
            (ptr->endingHandle >= AttDatabaseHandle[i].record_hdl))
        {
            U8 err;
            if ((err = ATTDb_CheckHandlePermission(link, AttDatabaseHandle[i].record_hdl, ATT_OP_READ)) != BT_ATT_ERROR_CODE_NO_ERROR)
            {
                /* Read only or Read/Write*/
                if(writeOffset == 1) /* Byte 0 is reserved for length */
                {
                    *error_hdl = AttDatabaseHandle[i].record_hdl;
                    *error_code = err;
                    return 0;
                }
            }
            if(AttDatabaseHandle[i].cb !=0)
            {
                AttDatabaseHandle[i].cb(ATT_OP_READ, link, AttDatabaseHandle[i].record_hdl, &AttDatabaseHandle[i].value_size, &AttDatabaseHandle[i].value_ptr);
            }          
            if((groupStartHandle > AttDatabaseHandle[i].record_hdl) ||
                (groupEndHandle < AttDatabaseHandle[i].record_hdl))
            {
                /* Handle not in group */
                continue;
            }
            recordUuidType = AttDatabaseHandle[i].record_ptr[2];
            if(recordUuidType == BT_UUID128_SIZE)
            {
                read_attribute_offset = 0x08 + BT_UUID128_SIZE - BT_UUID16_SIZE; // UUID 16 - UUID2 
            }  
            if(writeOffset == 1)
            {
                attributeSize = AttDatabaseHandle[i].value_size;
            }
            if(attributeSize != AttDatabaseHandle[i].value_size)
            {
                /* Format length is different*/
                break;
            }
            if((mtu - writeOffset) <= 4)
            {
                /* Unable to format a complete attribute data*/
                break;
            }
            if((mtu - writeOffset) > (AttDatabaseHandle[i].value_size+4))
            {
                availableWriteSize = AttDatabaseHandle[i].value_size+4;
            }
            else
            {
                availableWriteSize = mtu - writeOffset;
            }
            if(availableWriteSize == 1)
            {
                data[writeOffset] = (U8)(AttDatabaseHandle[i].record_hdl >> 8);
            }
            else if(availableWriteSize > 1)
            {
                StoreLE16(data+writeOffset,AttDatabaseHandle[i].record_hdl);
            }
            if(availableWriteSize == 3)
            {
                data[writeOffset] = (U8)(groupEndHandle >> 8);
            }
            else if(availableWriteSize > 3)
            {
                StoreLE16(data+writeOffset+2,groupEndHandle);
            }
            if(availableWriteSize > 4)
            {
                memcpy(data+writeOffset+4, AttDatabaseHandle[i].value_ptr, availableWriteSize-4);
            }
            writeOffset += availableWriteSize;
            if(writeOffset >= mtu)
            {
                break;
            }
        }
        else if(ptr->startingHandle < AttDatabaseHandle[i].record_hdl)
        {
            /* Continue search */
        }
        else if(ptr->endingHandle < AttDatabaseHandle[i].record_hdl)
        {
            /* Search done */
        }
    }
    if(writeOffset == 1) /* Byte 0 is reserved for length */
    {
        *error_hdl = AttDatabaseHandle[i].record_hdl;
        *error_code = BT_ATT_ERROR_CODE_INVALID_HANDLE;
        return 0;
    }    
    *error_code = BT_ATT_ERROR_CODE_NO_ERROR;
    data[0] = attributeSize+4;
    return writeOffset;
}



U8 ATTDb_HandleWriteRequest(BtATTWriteRequestStruct *ptr, BtRemoteDevice *link, BOOL need_rsp)
{
    U32 i=0;
    U16 attributeSize;
    U8 error_code;
    U8 read_attribute_offset = 8;
    U8 recordUuidType;
    bt_att_op op = (need_rsp ? ATT_OP_WRITE : ATT_OP_WRITE_WO_RESPONSE);

    OS_Report("ATTDb_HandleWriteRequest(handle=%u)", ptr->attributeHandle);
    for(i = 0; i< ATT_MAX_RECORD; i++)
    {
        if(ptr->attributeHandle == AttDatabaseHandle[i].record_hdl)
        {
            OS_Report("Attribute found : i=%u", i);
            recordUuidType = AttDatabaseHandle[i].record_ptr[2];
            if(recordUuidType == BT_UUID128_SIZE)
            {
                read_attribute_offset = 0x08 + BT_UUID128_SIZE - BT_UUID16_SIZE; // UUID 16 - UUID2 
            }  
        
            if ((error_code = ATTDb_CheckHandlePermission(link, AttDatabaseHandle[i].record_hdl, op)) != BT_ATT_ERROR_CODE_NO_ERROR)
            {
                OS_Report("check permission failed : %u", error_code);
                return error_code;
            }
            attributeSize = ATTDb_GetAttributeValueMaxSize(AttDatabaseHandle[i].record_ptr);
            switch(AttDatabaseHandle[i].record_ptr[3])
            {
                case BT_ATT_CONST_VARIABLE:
                    error_code = BT_ATT_ERROR_CODE_WRITE_NOT_PERMITTED;
                    return error_code;
                case BT_ATT_FIX_VARIABLE:
                    if(ptr->attributeValueLen <= attributeSize)
                    {
                        if(ptr->attributeValue !=0)
                            memcpy(AttDatabaseHandle[i].value_ptr, ptr->attributeValue, ptr->attributeValueLen);
                        AttDatabaseHandle[i].value_size = ptr->attributeValueLen;
                    }
                    else
                    {
                        error_code = BT_ATT_ERROR_CODE_INVALID_ATTRIBUTE_VALUE_LEN;
                    }
                    return BT_ATT_ERROR_CODE_NO_ERROR;
                case BT_ATT_NON_FIX_VARIABLE:
                    if(ptr->attributeValueLen <= attributeSize)
                    {
                        memcpy(AttDatabaseHandle[i].value_ptr, ptr->attributeValue, ptr->attributeValueLen);
                    }
                    else
                    {
                        error_code = BT_ATT_ERROR_CODE_INVALID_ATTRIBUTE_VALUE_LEN;
                    }
                    return BT_ATT_ERROR_CODE_NO_ERROR;
                case BT_ATT_FIX_VARIABLE_IN_UPPER_AP:
                    error_code = AttDatabaseHandle[i].cb(ATT_OP_WRITE, link, ptr->attributeHandle, &ptr->attributeValueLen, &(ptr->attributeValue));
                    return error_code;
                default:
                    Assert(0);
                    break;
            }

        }
    }
    if(i == ATT_MAX_RECORD)
    {
        error_code = BT_ATT_ERROR_CODE_INVALID_HANDLE;
    }
    else
    {
        error_code = BT_ATT_ERROR_CODE_NO_ERROR;
    }
    return error_code;
}

void ATTDb_HandleSignedWriteCommand(U8 *data, BtATTSignedWriteCommandStruct *ptr, BtRemoteDevice *link)
{
    U32 i=0;
    U16 attributeSize;
    
    for(i = 0; i< ATT_MAX_RECORD; i++)
    {
        if(ptr->attributeHandle == AttDatabaseHandle[i].record_hdl)
        {
            if (ATTDb_CheckHandlePermission(link, AttDatabaseHandle[i].record_hdl, ATT_OP_SIGNED_WRITE) != BT_ATT_ERROR_CODE_NO_ERROR)
            {
                return;
            }
            attributeSize = ATTDb_GetAttributeValueMaxSize(AttDatabaseHandle[i].record_ptr);
            switch(AttDatabaseHandle[i].record_ptr[3])
            {
                case BT_ATT_CONST_VARIABLE:
                    return;
                case BT_ATT_FIX_VARIABLE:
                    if(ptr->attributeValueLen <= attributeSize)
                    {
                        if(ptr->attributeValue !=0)
                            memcpy(AttDatabaseHandle[i].value_ptr, ptr->attributeValue, ptr->attributeValueLen);
                        AttDatabaseHandle[i].value_size= ptr->attributeValueLen;
                    }
                    return;
                case BT_ATT_NON_FIX_VARIABLE:
                    if(ptr->attributeValueLen <= attributeSize)
                    {
                        memcpy(AttDatabaseHandle[i].value_ptr, ptr->attributeValue, ptr->attributeValueLen);
                    }
                    return;
                case BT_ATT_FIX_VARIABLE_IN_UPPER_AP:
                    if (AttDatabaseHandle[i].cb(ATT_OP_WRITE, link, ptr->attributeHandle, &ptr->attributeValueLen, &ptr->attributeValue) == BT_STATUS_SUCCESS)
                    {
                        return;// BT_ATT_ERROR_CODE_NO_ERROR;
                    }
                    else
                    {
                        return;// BT_ATT_ERROR_CODE_INVALID_ATTRIBUTE_VALUE_LEN;
                    }
            }

        }
    }
    return;
}

U8 ATTDb_HandlePrepareWriteRequest(BtATTPrepareWriteRequestStruct *ptr, BtRemoteDevice *link)
{
    U32 i=0;
    U16 attributeSize;
    U8 err = BT_ATT_ERROR_CODE_NO_ERROR;
    
    for(i = 0; i< ATT_MAX_RECORD; i++)
    {
        if(ptr->attributeHandle == AttDatabaseHandle[i].record_hdl)
        {
            err = ATTDb_CheckHandlePermission(link, AttDatabaseHandle[i].record_hdl, ATT_OP_WRITE);
            if (err != BT_ATT_ERROR_CODE_NO_ERROR)
            {
                break;
            }

            attributeSize = ATTDb_GetAttributeValueMaxSize(AttDatabaseHandle[i].record_ptr);
            switch(AttDatabaseHandle[i].record_ptr[3])
            {
            case BT_ATT_CONST_VARIABLE:
                err = BT_ATT_ERROR_CODE_WRITE_NOT_PERMITTED;
                break;
            case BT_ATT_FIX_VARIABLE:
            case BT_ATT_NON_FIX_VARIABLE:                
                if(ptr->valueOffset > attributeSize){
                    err = BT_ATT_ERROR_CODE_INVALID_OFFSET;
                }
                break;
            case BT_ATT_FIX_VARIABLE_IN_UPPER_AP:
                err = AttDatabaseHandle[i].cb(ATT_OP_PREPARE_WRITE, link, ptr->attributeHandle, &ptr->partAttributeValueLen, &ptr->partAttributeValue);
                break;
            }
        }
    }
    return err;
}


U8 ATTDB_HandleExecuteWriteRequest(BtAttPrepareWrite *ptr, BtRemoteDevice *link, U8 cancel)
{
    U32 i=0;
    U16 attributeSize;
    U8 error_code = BT_ATT_ERROR_CODE_NO_ERROR;
    bt_att_op op = cancel ? ATT_OP_EXECUTE_WRITE_CANCEL : ATT_OP_EXECUTE_WRITE;

    for(i = 0; i< ATT_MAX_RECORD; i++)
    {
        if(ptr->handle == AttDatabaseHandle[i].record_hdl)
        {
            if(AttDatabaseHandle[i].cb != NULL){
                error_code = AttDatabaseHandle[i].cb(op, link, ptr->handle, &ptr->length, (U8 **)&ptr->writeBuffer);
                return error_code;
            }

            if(!cancel){
            attributeSize = ATTDb_GetAttributeValueMaxSize(AttDatabaseHandle[i].record_ptr);
            if((ptr->offset > attributeSize) ||
                (ptr->offset > AttDatabaseHandle[i].value_size))
            {
                 error_code = BT_ATT_ERROR_CODE_INVALID_OFFSET;
                 return error_code;
            }
            switch(AttDatabaseHandle[i].record_ptr[3])
            {
                case BT_ATT_FIX_VARIABLE:
                    if((ptr->offset + ptr->length ) <= attributeSize)
                    {
                        if(ptr->length !=0)
                            memcpy(AttDatabaseHandle[i].value_ptr + ptr->offset, ptr->writeBuffer, ptr->length);
                        if((ptr->offset + ptr->length) > AttDatabaseHandle[i].value_size)
                        {
                            AttDatabaseHandle[i].value_size = ptr->offset + ptr->length;
                        }
                    }
                    else
                    {
                        error_code = BT_ATT_ERROR_CODE_INVALID_ATTRIBUTE_VALUE_LEN;
                        return error_code;
                    }
                    break;
                case BT_ATT_NON_FIX_VARIABLE:
                    if((ptr->offset + ptr->length ) <= attributeSize)
                    {
                        memcpy(AttDatabaseHandle[i].value_ptr+ ptr->offset, ptr->writeBuffer, ptr->length);
                    }
                    else
                    {
                        error_code = BT_ATT_ERROR_CODE_INVALID_ATTRIBUTE_VALUE_LEN;
                        return error_code;
                    }
                    break;
                case BT_ATT_FIX_VARIABLE_IN_UPPER_AP:
                        if (AttDatabaseHandle[i].cb(ATT_OP_WRITE, link, ptr->handle, &ptr->length, (U8 **)&ptr->writeBuffer) == BT_STATUS_SUCCESS)
                    {
                        return BT_ATT_ERROR_CODE_NO_ERROR;
                    }
                    else
                    {
                        return BT_ATT_ERROR_CODE_INVALID_ATTRIBUTE_VALUE_LEN;
                    }
                }
            }
            break;
        }
    }
    return error_code;
}

void ATTDB_HandleApplicationResponse(BtRemoteDevice *link, U8 error_code, U8 *data, U16 length)
{
    BT_ATTHandleApplicationResponse(link, error_code, data, length);
}
#endif /* ATT_PROTOCOL == XA_ENABLED */
