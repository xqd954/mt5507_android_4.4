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

#ifndef __DDBH
#define __DDBH

/****************************************************************************
 *
 * File:
 *     $Workfile:ddb.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Definitions the API used by the stack to access a system-specific
 *     device database.
 * 
 * Copyright 1999-2005 Extended Systems, Inc.
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
#include "btconfig.h"
#include "bttypes.h"
#include "me_adp.h"
#ifdef __BT_4_0_BLE__
#include "le_adp.h"
#endif

/*---------------------------------------------------------------------------
 * Device Database API layer
 *
 *     The Device Database is used by the Management Entity to store link
 *     keys and other information in a non-volatile way. This allows security
 *     information about a device to be used in later sessions, even if the
 *     device is switched off.
 *
 *     The database accessed by these calls must be maintained in
 *     non-volatile memory, such as flash RAM or a hard disk drive.
 */

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * DDB_Open()
 *
 *     Called by the stack to open the device database. This function is
 *     called during stack initialization, before any other DDB_ calls
 *     are made.
 *
 * Parameters:
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful to
 *         indicate that other device database calls may be made.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed. Overall
 *         stack initialization (EVM_Init) will fail as a result.
 */
BtStatus DDB_Open(void);

/*---------------------------------------------------------------------------
 * DDB_Close()
 *
 *     Called by the stack to close the device database. This function is
 *     called during stack deinitialization.
 *
 * Returns:
 *     Ignored by the stack.
 */
BtStatus DDB_Close(void);

/*---------------------------------------------------------------------------
 * DDB_Flush()
 *
 *     Called by the stack to flush the device database to disk. This function 
 *     is called when new devices are added to the database. 
 *
 * Returns:
 *     Ignored by the stack.
 */
BtStatus DDB_Flush(void);
void DDB_FlushAllRecord(void);

/*---------------------------------------------------------------------------
 * DDB_AddRecord()
 *
 *     Called by the stack to add or replace a record in the database.
 *     If the device record indicates a BD_ADDR that is already in the
 *     database, this function should replace the existing record.
 *
 * Parameters:
 *     record - Pointer to the record which is to be copied into the
 *         database. The structure should not be used by DDB after this call
 *         has returned.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this to indicate that the record
 *         was successfully written to the database.
 *
 *     BT_STATUS_FAILED - Return this to indicate that the record could
 *         not be written to the database.
 */
BtStatus DDB_AddRecord(const BtDeviceRecord *record);

/*---------------------------------------------------------------------------
 * DDB_FindRecord()
 *
 *     Called by the stack to copy a record out of the database. The record
 *     is identified by the BD_ADDR that represents the device.
 *
 * Parameters:
 *     bdAddr - 48-bit address that identifies the desired record.
 * 
 *     record - pointer to the record structure to fill with the
 *         specified record's information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if the record was found and copied
 *         into the "record" parameter.
 *
 *     BT_STATUS_FAILED - Returns this if the record could not be found
 *         or an error occurred while accessing non-volatile memory.
 */
BtStatus DDB_FindRecord(const BD_ADDR *bdAddr, BtDeviceRecord *record);

/*---------------------------------------------------------------------------
 * DDB_DeleteRecord()
 *
 *     Called by the stack to remove a record from the database. The record
 *     is identified by the BD_ADDR that represents the device.
 * 
 * Parameters:
 *     bdAddr - 48-bit address contained in the record to delete.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if the record was found and deleted.
 *
 *     BT_STATUS_FAILED - Return this if the record could not be found or
 *         deleted.
 */
BtStatus DDB_DeleteRecord(const BD_ADDR *bdAddr);

/*---------------------------------------------------------------------------
 * DDB_EnumDeviceRecords()
 *
 *     Called by the stack to read a specific record from the device
 *     database. Records are stored with indexes between 0 and n-1 where n
 *     is the number of records in the database.
 *
 *     The stack calls this function to read device records from 0 to n-1
 *     in increasing sequence. The stack does not call other DDB functions
 *     during enumeration.
 *
 * Parameters:
 *     index - index of desired record. The first record is at index 0.
 *
 *     record - pointer to the record structure to fill with the
 *         specified record's information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if the record was located and copied
 *         into the "record" parameter.
 *
 *     BT_STATUS_FAILED - Return this if the record could not be found or
 *         copied. For example, BT_STATUS_FAILED should be returned if the
 *         specified index is invalid.
 */
BtStatus DDB_EnumDeviceRecords(I16 index, BtDeviceRecord *record);

BtStatus DDB_COD_AddRecord(BD_ADDR bdAddr, U32 cod);
BtStatus DDB_COD_Open(void);
BtStatus DDB_COD_Close(void);
BtStatus DDB_COD_Flush(void);
BtStatus DDB_COD_FindRecord(const BD_ADDR *bdAddr, BtCodRecord *record);

#ifdef __BT_4_0_BLE__
void DDB_ReadLeKey(U8 *csrk, U32 *counter, U8 *irk);
void DDB_WriteLeKey(U8 *csrk, U32 *counter, U8 *irk);
S32 DDB_LE_FindRecord(const BD_ADDR *bdAddr, BtLERecord *rec);
BtLERecord *DDB_LE_AddRecord(const BD_ADDR *bdAddr, LeProfile owner);
BtStatus DDB_LE_DeleteRecord(const BD_ADDR *bdAddr, LeProfile owner);
#endif

/***************************************************
*    Common DB API
***************************************************/
#define MAX_PATH 256
#define DB_FLUSH_TIMER 10000

/* 0 : not match, otherwize match */
typedef U8(*comparator)(const U8 *record, const void *key);

typedef struct _ddbStruct{
    char name[MAX_PATH];
    EvmTimer flushTimer;
    U8 dirtyFlag;
    U16 size;
    U16 count;
    U16 used;
    comparator pfnComparator;
    U8 *db;
}ddbStruct;

BtStatus DDB_OpenDB(const char *name, ddbStruct *db, U8 *mem, U16 size, U16 count, comparator pf_comparator);
BtStatus DDB_FlushDB(ddbStruct *db);
void DDB_FlushAllDBRecord(ddbStruct *db);
BtStatus DDB_FindDBRecord(ddbStruct *db, const void *key, U8 *record);
BtStatus DDB_AddDBRecord(ddbStruct *db, U8* record, const void *key);
BtStatus DDB_DeleteDBRecord(ddbStruct *db, const void *key);
BtStatus DDB_CloseDB(ddbStruct *db);

BtStatus DDB_OpenBTDB(BTDB db);
BtStatus DDB_CloseBTDB(BTDB db);
BtStatus DDB_PSI_AddRecord(BD_ADDR bdAddr, BtPageScanInfo *psi);
BtStatus DDB_PSI_FindRecord(BD_ADDR bdAddr, BtPageScanInfo *psi);
BtStatus DDB_VER_AddRecord(BD_ADDR bdAddr, U8 *version);
BtStatus DDB_VER_FindRecord(BD_ADDR bdAddr, U8 *version);
BtStatus DDB_DEV_TYPE_AddRecord(BD_ADDR bdAddr, BtDevType devType);
BtStatus DDB_DEV_TYPE_FindRecord(BD_ADDR bdAddr, BtDevType *devType);
BtStatus DDB_FlushDevInfoRecord();
#endif /* __DDBH */ 

