/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/***************************************************************************
 *
 * File:
 *     $Workfile:ddb4w.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This is file contains an implementation of the device
 *     database operating in the Windows environment. 
 *
 * Created:
 *     January 26, 2000
 *
 * Copyright 2000-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
/***************************************************************************** 
* Include
*****************************************************************************/
/* basic system service headers */
#include "bt_common.h"
#include "bttypes.h"
#include "ddb.h"
#include "btconfig.h"
#include "bt_adp_fs.h"
#ifdef __BT_4_0_BLE__
#include "medev.h"  //DS_FindDevice
#include "le_adp.h"
#endif
#include "attdbmain.h"
#include "btutils.h"
/****************************************************************************
 *
 * Macro
 *
 ****************************************************************************/
#define __ENABLE_DB_DEBUG__
/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

#define DDB_MAX_ENTRIES  40
#define DDB_COD_MAX_ENTRIES  40
#define DDB_LE_DEV_MAX_ENTRIES 100
#define DDB_NOT_FOUND    0xffff

/****************************************************************************
 *
 * Data
 *
 ****************************************************************************/


/* The device database is kept in memory. */

typedef struct _bt_ddb_data {
    I16                  numDdbRecs;
    BtDeviceRecord       devDb[DDB_MAX_ENTRIES];
} bt_ddb_data;

typedef struct _bt_ddb_cod_data {
    I16                  numcodDdbRecs;
    BtCodRecord          codDb[DDB_COD_MAX_ENTRIES];
} bt_ddb_cod_data;

typedef struct _bt_ddb_le_dev_data {
    I16                  numDevDdbRecs;
    BtLERecord           devDb[DDB_LE_DEV_MAX_ENTRIES];
} bt_ddb_le_dev_data;

typedef struct _bt_ddb_le_data {
    bt_ddb_le_dev_data devices;

    U8 csrk[SM_CSRK_VALUE_LENGTH];
    U32 signCounter;  /* with CSRK */
#ifdef __BT_4_0_PRIVACY__
    U8 irk[SM_IRK_VALUE_LENGTH];
#endif
} bt_ddb_le_data;

static bt_ddb_data ddb_linkkey = {.numDdbRecs = 0};
static bt_ddb_cod_data ddb_cod = {.numcodDdbRecs = 0};
#ifdef __BT_4_0_BLE__
static bt_ddb_le_data ddb_le;
#endif



/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

static I16 DdbFindRecord(const BD_ADDR *bdAddr);
static I16 DdbCodFindRecord(const BD_ADDR *bdAddr);


static void init_db(const U8 *db, U32 file_size, U8 *buffer){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 file_handle;
    S32 size;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check path */
    if (btmtk_fs_is_dir_exist((const U8*)BT_DB_PATH) == FALSE)
    {
        btmtk_fs_create_dir((const U8*)BT_DB_PATH);
    }
    
    file_handle = btmtk_fs_open(db, BTMTK_FS_READ_ONLY);

    if (file_handle < BTMTK_FS_OK)   /* first time, create the file */
    {
        OS_Report("file %s not exist", db);
    }
    else
    {
        OS_Report("init_db : db file opened");
        size = btmtk_fs_seek(file_handle, 0, BTMTK_FS_SEEK_END);
        if(size != (S32)file_size){
            OS_Report("init_db : file size %d incorrect, expected %u", size, file_size);
            btmtk_fs_close(file_handle);
        }else{
            btmtk_fs_seek(file_handle, 0, BTMTK_FS_SEEK_BEGIN);        
            btmtk_fs_read(file_handle, buffer, file_size);
            btmtk_fs_close(file_handle);
            return;
        }
    }
    file_handle = btmtk_fs_open(db, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
    btmtk_fs_write(file_handle, buffer, file_size);
    btmtk_fs_close(file_handle);    
}
/*****************************************************************************
 * FUNCTION
 *  ddb_init_link_key
 * DESCRIPTION
 *  
 * PARAMETERS
 *  file_size       [IN]        
 *  buffer          [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void ddb_init_link_key(U32 file_size, U8 *buffer)
{
    OS_Report("ddb_init_link_key : size=%u", file_size);
    init_db((const U8*)BT_DB_FILE, file_size, buffer);
}

/*****************************************************************************
 * FUNCTION
 *  ddb_write_link_key
 * DESCRIPTION
 *  
 * PARAMETERS
 *  file_size       [IN]        
 *  buffer          [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void ddb_write_link_key(U32 file_size, U8 *buffer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 file_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check path */
    if (btmtk_fs_is_dir_exist((const U8*)BT_DB_PATH) == FALSE)
    {
        btmtk_fs_create_dir((const U8*)BT_DB_PATH);
    }

    kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_OPENED);
    file_handle = btmtk_fs_open((const U8*)BT_DB_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
    btmtk_fs_write(file_handle, buffer, file_size);
    btmtk_fs_close(file_handle);
}


/*****************************************************************************
 * FUNCTION
 *  ddb_init_cod
 * DESCRIPTION
 *  
 * PARAMETERS
 *  file_size       [IN]        
 *  buffer          [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void ddb_init_cod(U32 file_size, U8 *buffer)
{
    OS_Report("ddb_init_cod : size=%u", file_size);
    init_db((const U8*)BT_DB_COD_FILE, file_size, buffer);
}


/*****************************************************************************
 * FUNCTION
 *  ddb_write_cod
 * DESCRIPTION
 *  
 * PARAMETERS
 *  file_size       [IN]        
 *  buffer          [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void ddb_write_cod(U32 file_size, U8 *buffer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 file_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check path */
    if (btmtk_fs_is_dir_exist((const U8*)BT_DB_PATH) == FALSE)
    {
        btmtk_fs_create_dir((const U8*)BT_DB_PATH);
    }

    file_handle = btmtk_fs_open((const U8*)BT_DB_COD_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
    btmtk_fs_write(file_handle, buffer, file_size);
    btmtk_fs_close(file_handle);
}


#ifdef __BT_4_0_BLE__
/*****************************************************************************
 * FUNCTION
 *  ddb_init_le_key
 * DESCRIPTION
 *  
 * PARAMETERS
 *  file_size       [IN]        
 *  buffer          [?]         
 * RETURNS
 *  void
 *****************************************************************************/
static void ddb_le_init(bt_ddb_le_data *buffer)
{
    OS_Report("ddb_le_init : size=%u", sizeof(bt_ddb_le_data));
    /* Set init csrk & irk */
    SM_GenCSRK(buffer->csrk);
    buffer->signCounter = 0;
#ifdef __BT_4_0_PRIVACY__
    SM_GenIRK(buffer->irk);
#endif
    init_db((const U8*)BT_DB_LE_FILE, sizeof(bt_ddb_le_data), (U8*)buffer);
}

/*****************************************************************************
 * FUNCTION
 *  ddb_le_flush
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void ddb_le_flush(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 file_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check path */
    if (btmtk_fs_is_dir_exist((const U8*)BT_DB_PATH) == FALSE)
    {
        btmtk_fs_create_dir((const U8*)BT_DB_PATH);
    }

    OS_Report("ddb_le_flush");
    file_handle = btmtk_fs_open((const U8*)BT_DB_LE_FILE, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
    btmtk_fs_write(file_handle, (U8 *)&ddb_le, sizeof(bt_ddb_le_data));
    btmtk_fs_close(file_handle);
}

void DDB_ReadLeKey(U8 *csrk, U32 *counter, U8 *irk)
{
    if (csrk)
    {
        OS_MemCopy(csrk, ddb_le.csrk, SM_CSRK_VALUE_LENGTH);
    }

    if (counter)
    {
        *counter = ddb_le.signCounter;
    }

#ifdef __BT_4_0_PRIVACY__
    if (irk)
    {
        OS_MemCopy(irk, ddb_le.irk, SM_IRK_VALUE_LENGTH);
    }
#endif
}

void DDB_WriteLeKey(U8 *csrk, U32 *counter, U8 *irk)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    if (csrk)
    {
        OS_MemCopy(ddb_le.csrk, csrk, SM_CSRK_VALUE_LENGTH);
    }

    if (counter)
    {
        ddb_le.signCounter = *counter;
    }

#ifdef __BT_4_0_PRIVACY__
    if (irk)
    {
        OS_MemCopy(ddb_le.irk, irk, SM_IRK_VALUE_LENGTH);
    }
#endif
    ddb_le_flush();
}

/*****************************************************************************
 * FUNCTION
 *  DDB_LE_FindRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr    [IN]
 *  rec       [OUT]
 * RETURNS
 *  record index
 *****************************************************************************/
S32 DDB_LE_FindRecord(const BD_ADDR *bdAddr, BtLERecord *rec)
{
    S32 count;

    OS_Report("DDB_LE_FindRecord:%d",ddb_le.devices.numDevDdbRecs);
    for (count = 0; count < ddb_le.devices.numDevDdbRecs; count++)
    {
        BtLERecord *leDev = &ddb_le.devices.devDb[count];
        BTBMDumpBdAddr(leDev->bdAddr.addr);
        if (OS_MemCmp(bdAddr->addr, 6, leDev->bdAddr.addr, 6))
        {
            OS_Report("Total=%d, devType=%d, addrType=%d, used profiles=0x%x", ddb_le.devices.numDevDdbRecs, leDev->devType, leDev->addrType, leDev->profiles);
            if (rec)
            {
                OS_MemCopy(rec, leDev, sizeof(BtLERecord));
            }
            return count;
        }
    }
    return -1;
}

BtLERecord *DDB_LE_AddRecord(const BD_ADDR *bdAddr, LeProfile owner)
{
    BtDeviceContext *bdc = NULL;
    BtLERecord *leDev = NULL;
    I16 count;
    S32 index;

    OS_Report("DDB_LE_AddRecord: profile=0x%x", owner);
    BTBMDumpBdAddr(bdAddr->addr);
    if ((bdc = DS_FindDevice(bdAddr)) == NULL)
    {
        OS_Report("DDB_LE_AddRecord failed");
        return NULL;
    }
    else
    {
        if ((index = DDB_LE_FindRecord(bdAddr, NULL)) == -1)
        {
            /* Add */
            if (ddb_le.devices.numDevDdbRecs == DDB_LE_DEV_MAX_ENTRIES)
            {
                OS_Report("LE DB full! Wrap around");
                count = ddb_le.devices.numDevDdbRecs - 1;
                OS_MemCopy((U8*) &ddb_le.devices.devDb[0], (U8*) &ddb_le.devices.devDb[1], (U16) count * sizeof(BtLERecord));
                leDev = &ddb_le.devices.devDb[count];
            }
            else
            {
                count = ddb_le.devices.numDevDdbRecs;
                leDev = &ddb_le.devices.devDb[count];
                ddb_le.devices.numDevDdbRecs++;
            }
            OS_MemSet((U8 *) leDev, 0x0, sizeof(BtLERecord));
            leDev->bdAddr = bdc->addr;
            leDev->devType = bdc->devType;
            leDev->addrType = bdc->addrType;
        }
        else
        {
            leDev = &ddb_le.devices.devDb[index];
        }
        leDev->profiles |= owner;
        OS_Report("Total=%d, devType=%d, addrType=%d, used profiles=0x%x", ddb_le.devices.numDevDdbRecs, leDev->devType, leDev->addrType, leDev->profiles);
        ddb_le_flush();
    }
    return leDev;
}

BtStatus DDB_LE_DeleteRecord(const BD_ADDR *bdAddr, LeProfile owner)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtLERecord *leDev = NULL;
    S32 index;

    OS_Report("DDB_LE_DeleteRecord: profile=0x%x", owner);
    BTBMDumpBdAddr(bdAddr->addr);
    if ((index = DDB_LE_FindRecord(bdAddr, NULL)) != -1)
    {
        leDev = &ddb_le.devices.devDb[index];
        OS_Report("all profiles=0x%x", leDev->profiles);
        leDev->profiles &= ~owner;
        if (leDev->profiles == LE_PROFILE_NONE)
        {
            /* Compact array */
            ddb_le.devices.numDevDdbRecs--;
            OS_MemCopy(
                (U8*) (&ddb_le.devices.devDb[index]),
                (U8*) (&ddb_le.devices.devDb[index + 1]),
                (U16) ((ddb_le.devices.numDevDdbRecs - index) * sizeof(BtLERecord)));
            OS_MemSet((U8*) (&ddb_le.devices.devDb[ddb_le.devices.numDevDdbRecs]), 0, sizeof(BtLERecord));
        }
        OS_Report("Total dev no=%d", ddb_le.devices.numDevDdbRecs);
        ddb_le_flush();
    }
    return status;
}

#endif

/*---------------------------------------------------------------------------
 *            DDB_Open()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open the device database. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_Open
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_Open(void)
{
    ddb_linkkey.numDdbRecs = 0;
    ddb_init_link_key((U32) sizeof(bt_ddb_data), (void*)&ddb_linkkey);
#ifdef __BT_4_0_BLE__
    ddb_le.devices.numDevDdbRecs = 0;
    ddb_le_init(&ddb_le);
    ddb_le_flush();
    
    OS_Report("DDB_Open ddb_le.devices.numDevDdbRecs:%d",ddb_le.devices.numDevDdbRecs);

#endif
    OS_Report("DDB_Open ddb_linkkey.numDdbRecs:%d",ddb_linkkey.numDdbRecs);

    return BT_STATUS_FAILED;
}

/*---------------------------------------------------------------------------
 *            DDB_Close()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close the device database. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_Close
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_Close(void)
{

    BtStatus status;

    status = BT_STATUS_FAILED;

    if (DDB_Flush() == BT_STATUS_SUCCESS)
    {
        status = BT_STATUS_SUCCESS;
    }

    ddb_linkkey.numDdbRecs = 0;
    return status;

}

/*---------------------------------------------------------------------------
 *            DDB_Flush()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Force the database to be written to disk. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_Flush
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_Flush(void)
{
    kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_FLUSHED);
    Assert(ddb_linkkey.numDdbRecs <= DDB_MAX_ENTRIES);
    ddb_write_link_key((U32) sizeof(bt_ddb_data), (void*)&ddb_linkkey);
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  DDB_FlushAllRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void DDB_FlushAllRecord(void)
{
    ddb_linkkey.numDdbRecs = 0;
#ifdef __BT_4_0_BLE__
    ATTDB_NotifyBondStatus(NULL, 0x03);
#endif    
    OS_MemSet((U8*) & ddb_linkkey.devDb, 0, sizeof(ddb_linkkey.devDb));
    DDB_Flush();
}

/*---------------------------------------------------------------------------
 *            DDB_AddRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Add a record to the database. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_AddRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  record      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_AddRecord(const BtDeviceRecord *record)
{

    I16 count;
    BtDeviceRecord temp_ddb[DDB_MAX_ENTRIES];

    count = DdbFindRecord(&(record->bdAddr));
    if (count == DDB_NOT_FOUND)
    {
        /* A record with this bdAddr does not exist so add it */
        if (ddb_linkkey.numDdbRecs == DDB_MAX_ENTRIES)
        {
            count = ddb_linkkey.numDdbRecs - 1;
            OS_MemCopy((U8*) (&temp_ddb[0]), (U8*) (&ddb_linkkey.devDb[1]), (U16) ((ddb_linkkey.numDdbRecs - 1) * sizeof(BtDeviceRecord)));
            OS_MemCopy((U8*) (&ddb_linkkey.devDb[0]), (U8*) (&temp_ddb[0]), (U16) ((ddb_linkkey.numDdbRecs - 1) * sizeof(BtDeviceRecord)));
        }
        else
        {
            count = ddb_linkkey.numDdbRecs;
            ddb_linkkey.numDdbRecs++;
        }
    }
    Report(("DDB_AddRecord ddb_linkkey.numDdbRecs:%d",ddb_linkkey.numDdbRecs));
    OS_MemCopy((U8*) (&ddb_linkkey.devDb[count]), (U8*)record, sizeof(BtDeviceRecord));
    if (DDB_Flush() != BT_STATUS_SUCCESS)
    {
        kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_SAVEING_ERROR, count);
    }

    return BT_STATUS_SUCCESS;

}

/*---------------------------------------------------------------------------
 *            DDB_FindRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Find the record that has the given BD_ADDR. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_FindRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 *  record      [IN]
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_FindRecord(const BD_ADDR *bdAddr, BtDeviceRecord *record)
{

    I16 count;
    Report(("DDB_FindRecord ddb_linkkey.numDdbRecs:%d",ddb_linkkey.numDdbRecs));

    count = DdbFindRecord(bdAddr);

    if (count != DDB_NOT_FOUND)
    {
        *record = ddb_linkkey.devDb[count];
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAILED;

}

/*---------------------------------------------------------------------------
 *            DDB_DeleteRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Delete a record from the database. 
 *
 * Return:    BT_STATUS_SUCCESS if success otherwise BT_STATUS_FAILED.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_DeleteRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_DeleteRecord(const BD_ADDR *bdAddr)
{

    I16 count;
    BtDeviceRecord temp_ddb[DDB_MAX_ENTRIES];

    count = DdbFindRecord(bdAddr);
    if (count != DDB_NOT_FOUND)
    {
        /*
         * If it is the last record then all we need to do is reduce
         * * numDdbRecs by 1. Otherwise we need to shift the array
         */
        ddb_linkkey.numDdbRecs--;
        if (count < ddb_linkkey.numDdbRecs)
        {
            /* We need to shift the array */
            OS_MemCopy(
                (U8*) (&temp_ddb[0]),
                (U8*) (&ddb_linkkey.devDb[count + 1]),
                (U16) ((ddb_linkkey.numDdbRecs - count) * sizeof(BtDeviceRecord)));

            OS_MemCopy(
                (U8*) (&ddb_linkkey.devDb[count]),
                (U8*) (&temp_ddb[0]),
                (U16) ((ddb_linkkey.numDdbRecs - count) * sizeof(BtDeviceRecord)));
            OS_MemSet((U8*) (&ddb_linkkey.devDb[ddb_linkkey.numDdbRecs]), 0, sizeof(BtDeviceRecord));
        }
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAILED;

}

/*---------------------------------------------------------------------------
 *            DDB_EnumDeviceRecords()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Enumerate the records in the device database. 
 *
 * Return:    status of the operation.
 */


/*****************************************************************************
 * FUNCTION
 *  DDB_EnumDeviceRecords
 * DESCRIPTION
 *  
 * PARAMETERS
 *  i           [IN]        
 *  record      [IN]
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_EnumDeviceRecords(I16 i, BtDeviceRecord *record)
{
    if (i >= ddb_linkkey.numDdbRecs)
    {
        return BT_STATUS_FAILED;
    }
    else
    {
        *record = ddb_linkkey.devDb[i];
    }
    return BT_STATUS_SUCCESS;

}

/*---------------------------------------------------------------------------
 *            DdbFindRecord()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Find the record that has the given BD_ADDR. 
 *
 * Return:    Index of BtDeviceRecord if found otherwise DDB_NOT_FOUND.
 */


/*****************************************************************************
 * FUNCTION
 *  DdbFindRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static I16 DdbFindRecord(const BD_ADDR *bdAddr)
{

    I16 count;

    kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_FIND_RECORD, bdAddr->addr[0], bdAddr->addr[1], bdAddr->addr[2]);
    kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_FIND_RECORD, bdAddr->addr[3], bdAddr->addr[4], bdAddr->addr[5]);

    for (count = 0; count < ddb_linkkey.numDdbRecs; count++)
    {
        kal_trace(TRACE_GROUP_1,
            BT_DDB4W_DB_STORED_RECORD,
            ddb_linkkey.devDb[count].bdAddr.addr[0],
            ddb_linkkey.devDb[count].bdAddr.addr[1],
            ddb_linkkey.devDb[count].bdAddr.addr[2]);
        kal_trace(TRACE_GROUP_1,
            BT_DDB4W_DB_STORED_RECORD,
            ddb_linkkey.devDb[count].bdAddr.addr[3],
            ddb_linkkey.devDb[count].bdAddr.addr[4],
            ddb_linkkey.devDb[count].bdAddr.addr[5]);
        kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_LINK_KEY, 
            ddb_linkkey.devDb[count].linkKey[0], 
            ddb_linkkey.devDb[count].linkKey[1], 
            ddb_linkkey.devDb[count].linkKey[2],
            ddb_linkkey.devDb[count].linkKey[3],
            ddb_linkkey.devDb[count].linkKey[4],
            ddb_linkkey.devDb[count].linkKey[5],
            ddb_linkkey.devDb[count].linkKey[6],
            ddb_linkkey.devDb[count].linkKey[7],
            ddb_linkkey.devDb[count].linkKey[8],
            ddb_linkkey.devDb[count].linkKey[9],
            ddb_linkkey.devDb[count].linkKey[10],
            ddb_linkkey.devDb[count].linkKey[11],
            ddb_linkkey.devDb[count].linkKey[12],
            ddb_linkkey.devDb[count].linkKey[13],
            ddb_linkkey.devDb[count].linkKey[14],
            ddb_linkkey.devDb[count].linkKey[15]);        
        if (OS_MemCmp(bdAddr->addr, 6, ddb_linkkey.devDb[count].bdAddr.addr, 6))
        {
            /* The record is found so return it */
            return count;
        }
    }
    return DDB_NOT_FOUND;

}


/*****************************************************************************
 * FUNCTION
 *  DDB_COD_Open
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_COD_Open(void)
{
    ddb_cod.numcodDdbRecs = 0;
    ddb_init_cod((U32) sizeof(bt_ddb_cod_data), (void*)&ddb_cod);
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  DDB_COD_Close
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_COD_Close(void)
{

    BtStatus status;

    status = BT_STATUS_FAILED;

    if (DDB_COD_Flush() == BT_STATUS_SUCCESS)
    {
        status = BT_STATUS_SUCCESS;
    }

    ddb_cod.numcodDdbRecs = 0;
    return status;

}


/*****************************************************************************
 * FUNCTION
 *  DDB_COD_Flush
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_COD_Flush(void)
{
    Assert(ddb_cod.numcodDdbRecs <= DDB_COD_MAX_ENTRIES);
    ddb_write_cod((U32) sizeof(bt_ddb_cod_data), (void*)&ddb_cod);
    OS_Report("DDB_COD_Flush : numcodDdbRecs=%d", ddb_cod.numcodDdbRecs);
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  DDB_COD_AddRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  record      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_COD_AddRecord(BD_ADDR bdAddr, U32 cod)
{

    I16 count;
    BtCodRecord temp_ddb[DDB_COD_MAX_ENTRIES];

    count = DdbCodFindRecord(&bdAddr);

    if (count == DDB_NOT_FOUND)
    {
        /* A record with this bdAddr does not exist so add it */
        if (ddb_cod.numcodDdbRecs == DDB_MAX_ENTRIES)
        {
            count = ddb_cod.numcodDdbRecs - 1;
            OS_MemCopy((U8*) (&temp_ddb[0]), (U8*) (&ddb_cod.codDb[1]), (U16) ((ddb_cod.numcodDdbRecs - 1) * sizeof(BtCodRecord)));
            OS_MemCopy((U8*) (&ddb_cod.codDb[0]), (U8*) (&temp_ddb[0]), (U16) ((ddb_cod.numcodDdbRecs - 1) * sizeof(BtCodRecord)));
        }
        else
        {
            count = ddb_cod.numcodDdbRecs;
            ddb_cod.numcodDdbRecs++;
        }
    }
    kal_trace(TRACE_GROUP_1,BT_DDB4W_COD_NUMBER, ddb_cod.numcodDdbRecs);
    ddb_cod.codDb[count].bdAddr = bdAddr;
    ddb_cod.codDb[count].cod = cod;
    return BT_STATUS_SUCCESS;

}


/*****************************************************************************
 * FUNCTION
 *  DDB_COD_FindRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 *  record      [IN]
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DDB_COD_FindRecord(const BD_ADDR *bdAddr, BtCodRecord *record)
{

    I16 count;

    count = DdbCodFindRecord(bdAddr);

    if (count != DDB_NOT_FOUND)
    {
        *record = ddb_cod.codDb[count];
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAILED;

}


/*****************************************************************************
 * FUNCTION
 *  DdbCodFindRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdAddr      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static I16 DdbCodFindRecord(const BD_ADDR *bdAddr)
{

    I16 count;

    for (count = 0; count < ddb_cod.numcodDdbRecs; count++)
    {
        if (OS_MemCmp(bdAddr->addr, 6, ddb_cod.codDb[count].bdAddr.addr, 6))
        {
            /* The record is found so return it */
            kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_FIND_RECORD, bdAddr->addr[0], bdAddr->addr[1], bdAddr->addr[2]);
            kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_FIND_RECORD, bdAddr->addr[3], bdAddr->addr[4], bdAddr->addr[5]);
            return count;
        }
    }
    return DDB_NOT_FOUND;

}

/*********************************
*    Interface for common DDB usage
**********************************/
static void ddb_init_db(ddbStruct *db)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 file_handle;
    S32 size;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check path */
    if (btmtk_fs_is_dir_exist((const U8*)BT_DB_PATH) == FALSE)
    {
        btmtk_fs_create_dir((const U8*)BT_DB_PATH);
    }

    file_handle = btmtk_fs_open((const U8*)db->name, BTMTK_FS_READ_ONLY);

    if (file_handle < BTMTK_FS_OK)   /* first time, create the file */
    {
        bt_prompt_trace(MOD_BT, "[ERR] db %s not exist", db->name);
    }
    else
    {
        kal_trace(TRACE_GROUP_1,BT_DDB4W_DB_OPENED);
        size = btmtk_fs_seek(file_handle, 0, BTMTK_FS_SEEK_END);
        if(size != (S32)(6+db->size*db->count))
        {
            btmtk_fs_close(file_handle);
            OS_Report("db %s file size %d incorrect : expected %u", db->name, size, (6+db->size*db->count));
        }
        else
        {
            btmtk_fs_seek(file_handle, 0, BTMTK_FS_SEEK_BEGIN);
            btmtk_fs_read(file_handle, (U8*)&db->size, 6);
            btmtk_fs_read(file_handle, (U8*)db->db, db->size*db->count);
            btmtk_fs_close(file_handle);
            return;
        }
    }
    file_handle = btmtk_fs_open((const U8*)db->name, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
    OS_MemSet(db->db, 0, db->size*db->count);
    db->used = 0;
    btmtk_fs_write(file_handle, (U8*)&db->size, 6);
    btmtk_fs_write(file_handle, (U8*)db->db, db->size*db->count);
    btmtk_fs_close(file_handle);
}

static void ddb_write_db(ddbStruct *db)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 file_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check path */
    if (btmtk_fs_is_dir_exist((const U8*)BT_DB_PATH) == FALSE)
    {
        btmtk_fs_create_dir((const U8*)BT_DB_PATH);
    }

    file_handle = btmtk_fs_open((const U8*)db->name, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
    btmtk_fs_write(file_handle, (U8*)&db->size, 6);
    btmtk_fs_write(file_handle, db->db, db->size*db->count);
    btmtk_fs_close(file_handle);
}

static I16 DdbFindDBRecord(ddbStruct *db, const void *key)
{
    I16 count;

    for (count = 0; count < db->used; count++)
    {
        if (db->pfnComparator(&db->db[db->size*count], key))
        {
            /* The record is found so return it */
            bt_prompt_trace(MOD_BT, "DdbFindDBRecord(0x%x) : %d record found", db, count);
            return count;
        }
    }
    return DDB_NOT_FOUND;
}

static void flushDBTimeoutHandler(EvmTimer *timer){
    Assert(timer && timer->context);
    #ifdef __ENABLE_DB_DEBUG__
    OS_Report("[DB] flushDBTimeoutHandler(%p), db=%s", timer, ((ddbStruct*)timer->context)->name);
    #endif
    if(timer && timer->context){
        DDB_FlushDB((ddbStruct*)timer->context);
    }
}

static void updateDB(ddbStruct *db){
    #ifdef __ENABLE_DB_DEBUG__
    OS_Report("[DB] updateDB : db=%s\n", db->name);
    #endif
    if(!db->dirtyFlag){
        db->dirtyFlag = 1;
        EVM_StartTimer(&db->flushTimer, DB_FLUSH_TIMER);
    }
}

BtStatus DDB_OpenDB(const char *name, ddbStruct *db, U8 *mem, U16 size, U16 count, comparator pf_comparator){
    bt_prompt_trace(MOD_BT, "DDB_OpenDB(0x%x) : name=%s, size=%u, count=%u", db, name, size, count);
    if(name && db && mem && size && count && pf_comparator){
        U32 len;
        OS_StrCpy(db->name, BT_DB_PATH);
        len = OS_StrLen(db->name);
        if(db->name[len-1] != '\\' && db->name[len-1] != '/'){
            #ifdef BTMTK_ON_LINUX
            db->name[len++] = '/';
            #else
            db->name[len++] = '\\';
            #endif
            db->name[len++] = '\0';
        }
        OS_StrCat(db->name, name);
        db->flushTimer.context = db;
        db->flushTimer.func = flushDBTimeoutHandler;
        db->size = size;
        db->count = count;
        db->db = mem;
        db->pfnComparator = pf_comparator;
        ddb_init_db(db);
    }else{
        bt_prompt_trace(MOD_BT, "[DB][ERR] DDB_OpenDB(0x%x, 0x%x, 0x%x, %u, %u)", name, db, mem, size, count);
        return BT_STATUS_INVALID_PARM;
    }
    return BT_STATUS_SUCCESS;
}

BtStatus DDB_FlushDB(ddbStruct *db)
{
    bt_prompt_trace(MOD_BT, "DDB_FlushDB(0x%x)", db);
    if(db->dirtyFlag){
        ddb_write_db(db);
        db->dirtyFlag = 0;
        EVM_CancelTimer(&db->flushTimer);
    }    
    return BT_STATUS_SUCCESS;
}

void DDB_FlushAllDBRecord(ddbStruct *db)
{
    bt_prompt_trace(MOD_BT, "DDB_FlushAllDBRecord(0x%x)", db);
    db->used = 0;
    DDB_Flush();
}

BtStatus DDB_FindDBRecord(ddbStruct *db, const void *key, U8 *record)
{
    I16 count;
    
    bt_prompt_trace(MOD_BT, "DDB_FindDBRecord(0x%x)", db);
    count = DdbFindDBRecord(db, key);

    if (count != DDB_NOT_FOUND)
    {
        OS_MemCopy(record, &db->db[db->size*count], db->size);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAILED;

}

BtStatus DDB_AddDBRecord(ddbStruct *db, U8* record, const void *key)
{
    I16 count;

    count = DdbFindDBRecord(db, key);
    if (count == DDB_NOT_FOUND)
    {
        /* A record with this bdAddr does not exist so add it */
        if (db->used == db->count)
        {
            count = db->count - 1;
            OS_MemCopy(db->db, &db->db[db->size], (db->count - 1)*db->size);
        }
        else
        {
            count = db->used;
            db->used++;
        }
    }
    OS_MemCopy(&db->db[db->size*count], record, db->size);
    updateDB(db);
    return BT_STATUS_SUCCESS;
}

static void DDB_DBRecordUpdated(ddbStruct *db){
    updateDB(db);
}

BtStatus DDB_DeleteDBRecord(ddbStruct *db, const void *key)
{
    I16 count;
    count = DdbFindDBRecord(db, key);
    if (count != DDB_NOT_FOUND)
    {
        /*
         * If it is the last record then all we need to do is reduce
         * * numDdbRecs by 1. Otherwise we need to shift the array
         */
        db->used--;
        if (count < db->used)
        {
            /* We need to shift the array */
            OS_MemCopy(&db->db[db->size*count], 
                                   &db->db[db->size*(count+1)],
                                   (db->size*(db->used-count)));
        }
        updateDB(db);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAILED;

}

BtStatus DDB_CloseDB(ddbStruct *db){
    BtStatus status;
    bt_prompt_trace(MOD_BT, "DDB_CloseDB(0x%x)", db);
    status = BT_STATUS_FAILED;
    
    if (DDB_FlushDB(db) == BT_STATUS_SUCCESS)
    {
        status = BT_STATUS_SUCCESS;
    }
    db->used = 0;
    return status;
}

/*************************************************************
*
*
**************************************************************/
static U8 BTDBCommonComparator(const U8 *record, const void *key){
    BtCommonRecord *btdbRecord = (BtCommonRecord*)record;
    return OS_MemCmp(btdbRecord->bdAddr.addr, 6, (U8*)key, 6);
}

/* BTDB_BTDEV_INFO */
#define NUM_OF_BTDEV_INFO_RECORD 100
static BtDeviceInfoRecord s_btdevinfoRecord[NUM_OF_BTDEV_INFO_RECORD];

struct {
    const char *name;
    ddbStruct db;
    void *mem;
    const U16 size;
    const U16 count;
    comparator pf_comparator;
} btdb_info[NUM_OF_BTDB] = 
{
    {   /* BTDB_BTDEV_INFO */
        "BTDEVINFO",
        {{0}},
        (void*)s_btdevinfoRecord,
        sizeof(BtDeviceInfoRecord),
        NUM_OF_BTDEV_INFO_RECORD,
        BTDBCommonComparator,
    },
};

BtStatus DDB_OpenBTDB(BTDB db){
    if(db >= NUM_OF_BTDB){
        Assert(db < NUM_OF_BTDB);
        return BT_STATUS_NOT_FOUND;
    }
    return DDB_OpenDB(btdb_info[db].name, 
                                    &btdb_info[db].db, 
                                    btdb_info[db].mem,
                                    btdb_info[db].size,
                                    btdb_info[db].count,
                                    btdb_info[db].pf_comparator);
}

BtStatus DDB_FlushBTDB(BTDB db){
    if(db >= NUM_OF_BTDB){
        Assert(db < NUM_OF_BTDB);
        return BT_STATUS_NOT_FOUND;
    }
    return DDB_FlushDB(&btdb_info[db].db);
}

void DDB_FlushAllBTDBRecord(BTDB db){
    if(db >= NUM_OF_BTDB){
        Assert(db < NUM_OF_BTDB);
        return;
    }
    DDB_FlushAllDBRecord(&btdb_info[db].db);
}

BtStatus DDB_FindBTDBRecord(BTDB db, const void *key, U8 *record){
    if(db >= NUM_OF_BTDB){
        Assert(db < NUM_OF_BTDB);
        return BT_STATUS_NOT_FOUND;
    }
    return DDB_FindDBRecord(&btdb_info[db].db, key, record);
}

static BtStatus DDB_GetBTDBRecord(BTDB db, const void *key, U8 **record){
    I16 count;
    ddbStruct *btdb = NULL;
    
    if(db >= NUM_OF_BTDB){
        Assert(db < NUM_OF_BTDB);
        return BT_STATUS_NOT_FOUND;
    }

    btdb = &btdb_info[db].db;
    count = DdbFindDBRecord(btdb, key);
    if(count != DDB_NOT_FOUND){
        *record = &btdb->db[btdb->size*count];
        return BT_STATUS_SUCCESS;
    }
    OS_Report("DDB_GetBTDBRecord failed\n");
    return BT_STATUS_NOT_FOUND;
}

static BtStatus DDB_allocateBTDBRecord(BTDB db, U8 **record){
    I16 count;
    ddbStruct *btdb = NULL;

    if(db >= NUM_OF_BTDB){
        Assert(db < NUM_OF_BTDB);
        return BT_STATUS_NOT_FOUND;
    }

    OS_Report("DDB_allocateBTDBRecord(%d,%p)\n", db, record);
    btdb = &btdb_info[db].db;
    /* A record with this bdAddr does not exist so add it */
    if (btdb->used == btdb->count)
    {
        count = btdb->count - 1;
        OS_MemCopy(btdb->db, &btdb->db[btdb->size], (btdb->count - 1)*btdb->size);
    }
    else
    {
        count = btdb->used;
        btdb->used++;
    }
    OS_Report("db=%p, size=%u, count=%d\n", btdb->db, btdb->size, count);
    *record = &btdb->db[btdb->size*count];
    return BT_STATUS_SUCCESS;
}

static void DDB_BTDBRecordUpdated(BTDB db, U8 immediate){
    if(immediate){
        DDB_FlushDB(&btdb_info[db].db);
    }else{
        DDB_DBRecordUpdated(&btdb_info[db].db);
    }
}
    
BtStatus DDB_AddBTDBRecord(BTDB db, U8* record, void *key){
    if(db >= NUM_OF_BTDB){
        Assert(db < NUM_OF_BTDB);
        return BT_STATUS_NOT_FOUND;
    }
    return DDB_AddDBRecord(&btdb_info[db].db, record, key);
}

BtStatus DDB_DeleteBTDBRecord(BTDB db, void *key){
    if(db >= NUM_OF_BTDB){
        Assert(db < NUM_OF_BTDB);
        return BT_STATUS_NOT_FOUND;
    }
    return DDB_DeleteDBRecord(&btdb_info[db].db, key);
}

BtStatus DDB_CloseBTDB(BTDB db){
    if(db >= NUM_OF_BTDB){
        Assert(db < NUM_OF_BTDB);
        return BT_STATUS_NOT_FOUND;
    }
    return DDB_CloseDB(&btdb_info[db].db);
}
/*********************************************
*    Easy interface for update device info
*********************************************/
static BtStatus ddbGetDevInfoRecord(BD_ADDR bdAddr, BtDeviceInfoRecord **record){
    if( DDB_GetBTDBRecord(BTDB_BTDEV_INFO, bdAddr.addr, (U8**)record) == BT_STATUS_SUCCESS ){
    }else if( DDB_allocateBTDBRecord(BTDB_BTDEV_INFO, (U8**)record) == BT_STATUS_SUCCESS ){
        OS_Report("*record=%p\n", *record);
        OS_Report("%u, %u,%u,%u,%u\n", 
            (*record)->bdAddr.addr[0],
            (*record)->bdAddr.addr[1],
            (*record)->bdAddr.addr[2],
            (*record)->bdAddr.addr[3],
            (*record)->bdAddr.addr[4],
            (*record)->bdAddr.addr[5]);
        OS_MemCopy((*record)->bdAddr.addr, bdAddr.addr, 6);
        OS_MemSet((*record)->info_mask, 0, sizeof((*record)->info_mask));
    }else{
        OS_Report("[DB] ddbGetDevInfoRecord failed");
        return BT_STATUS_FAILED;
    }
    return BT_STATUS_SUCCESS;
}

BtStatus DDB_PSI_AddRecord(BD_ADDR bdAddr, BtPageScanInfo *psi){
    BtDeviceInfoRecord *record;
    OS_Report("[DB] DDB_PSI_AddRecord");
    BTBMDumpBdAddr(bdAddr.addr);
    OS_Report("[DB] psi : psRepMode=%u, psMode=%u, clockOffset=%u", psi->psRepMode, psi->psMode, psi->clockOffset);
    if( ddbGetDevInfoRecord(bdAddr, &record) != BT_STATUS_SUCCESS ){
        OS_Report("[DB][ERR] ddbGetDevInfoRecord failed");
        return BT_STATUS_FAILED;
    }
    record->psi.clockOffset = psi->clockOffset;
    if(psi->psMode != 0xFF) record->psi.psMode = psi->psMode;
    if(psi->psRepMode != 0xFF) record->psi.psRepMode = psi->psRepMode;
    OS_SetBit(BTDEV_INFO_PSI, record->info_mask);
    DDB_BTDBRecordUpdated(BTDB_BTDEV_INFO, 0);
    return BT_STATUS_SUCCESS;
}

BtStatus DDB_PSI_FindRecord(BD_ADDR bdAddr, BtPageScanInfo *psi){
    BtDeviceInfoRecord *record;
    OS_Report("[DB] DDB_PSI_FindRecord");
    BTBMDumpBdAddr(bdAddr.addr);
    OS_Report("DDB_PSI_FindRecord : record=%p\n", record);
    if( DDB_GetBTDBRecord(BTDB_BTDEV_INFO, bdAddr.addr, (U8**)&record) == BT_STATUS_SUCCESS &&
        OS_TestBit(BTDEV_INFO_PSI, record->info_mask)){
        OS_Report("record found\n");
        *psi = record->psi;
        OS_Report("psi : psRepMode=%u, psMode=%u, clockOffset=%u", psi->psRepMode, psi->psMode, psi->clockOffset);
        return BT_STATUS_SUCCESS;
    }
    OS_Report("[DB][ERR] DDB_PSI_FindRecord failed");
    return BT_STATUS_NOT_FOUND;
}

BtStatus DDB_VER_AddRecord(BD_ADDR bdAddr, U8 *version){
    BtDeviceInfoRecord *record;
    OS_Report("[DB] DDB_VER_AddRecord");
    BTBMDumpBdAddr(bdAddr.addr);
    OS_Report("version=%u, %u, %u, %u, %u, %u, %u, %u", 
        version[0], version[1],version[2],version[3],version[4],version[5],version[6],version[7]);
    if( ddbGetDevInfoRecord(bdAddr, &record) != BT_STATUS_SUCCESS){
        OS_Report("[DB][ERR] ddbGetDevInfoRecord failed");
        return BT_STATUS_FAILED;
    }
    OS_MemCopy(record->version, version, sizeof(record->version));
    OS_SetBit(BTDEV_INFO_VERSION, record->info_mask);
    DDB_BTDBRecordUpdated(BTDB_BTDEV_INFO, 0);
    return BT_STATUS_SUCCESS;
}

BtStatus DDB_VER_FindRecord(BD_ADDR bdAddr, U8 *version){
    BtDeviceInfoRecord *record;
    OS_Report("[DB] DDB_VER_FindRecord");
    BTBMDumpBdAddr(bdAddr.addr);
    if( DDB_GetBTDBRecord(BTDB_BTDEV_INFO, bdAddr.addr, (U8**)&record) == BT_STATUS_SUCCESS &&
        OS_TestBit(BTDEV_INFO_VERSION, record->info_mask)){
        OS_Report("version=%u, %u, %u, %u, %u, %u, %u, %u", 
        version[0], version[1],version[2],version[3],version[4],version[5],version[6],version[7]);        
        OS_MemCopy(version, record->version, sizeof(record->version));
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_NOT_FOUND;
}

BtStatus DDB_DEV_TYPE_AddRecord(BD_ADDR bdAddr, BtDevType devType){
    BtDeviceInfoRecord *record;
    OS_Report("[DB] DDB_DEV_TYPE_AddRecord");
    BTBMDumpBdAddr(bdAddr.addr);
    OS_Report("[DB] devType = %d", devType);
    if( ddbGetDevInfoRecord(bdAddr, &record) != BT_STATUS_SUCCESS ){
        OS_Report("[DB][ERR] ddbGetDevInfoRecord failed");
        return BT_STATUS_FAILED;
    }
    if(/*devType >= BT_DEV_TYPE_UNKNOWN && */devType < NUM_OF_BT_DEV_TYPE){
        OS_SetBit(BTDEV_INFO_DEVTYPE, record->info_mask);
        DDB_BTDBRecordUpdated(BTDB_BTDEV_INFO, 0);
        return BT_STATUS_SUCCESS;
    }else{
        OS_Report("[DB][ERR] invalid devType %d", devType);
    }
    return BT_STATUS_FAILED;
}

BtStatus DDB_DEV_TYPE_FindRecord(BD_ADDR bdAddr, BtDevType *devType){
    BtDeviceInfoRecord *record;
    OS_Report("[DB] DDB_DEV_TYPE_FindRecord");
    BTBMDumpBdAddr(bdAddr.addr);
    if( DDB_GetBTDBRecord(BTDB_BTDEV_INFO, bdAddr.addr, (U8**)&record) == BT_STATUS_SUCCESS &&
        OS_TestBit(BTDEV_INFO_DEVTYPE, record->info_mask)){
        OS_Report("record found\n");
        OS_Report("DDB_DEV_TYPE_FindRecord : record=%p\n", record);        
        *devType = record->devType;
        return BT_STATUS_SUCCESS;
    }
    OS_Report("[DB][ERR] DDB_PSI_FindRecord failed");
    return BT_STATUS_NOT_FOUND;
}


BtStatus DDB_FlushDevInfoRecord(){
    return DDB_FlushBTDB(BTDB_BTDEV_INFO);
}
