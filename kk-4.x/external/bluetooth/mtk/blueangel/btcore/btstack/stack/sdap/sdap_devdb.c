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

#include "sdap_devdb.h"

static ddbStruct g_sdap_devdb;

static SdapDevDBRecord g_sdap_devdb_records[SDAP_DEVDB_MAX_RECORD];

static U8 sdap_devdb_comparator(const U8* record, const void* key)
{
    U8 ret;
    SdapDevDBRecord* sdap_devdb_record = (SdapDevDBRecord*) record;
    ret = OS_MemCmp(sdap_devdb_record->bdaddr.addr, 6, (U8*) key, 6);

    OS_Report("[SDAP][DB] %s() ret: %d", __FUNCTION__, ret);
    return ret;
}

BtStatus SDAP_OpenDB(void)
{
    BtStatus ret = BT_STATUS_FAILED;

    ret = DDB_OpenDB(SDAP_DEVDB_NAME, &g_sdap_devdb,
        (U8*)g_sdap_devdb_records, sizeof(SdapDevDBRecord), SDAP_DEVDB_MAX_RECORD,
        sdap_devdb_comparator);

    if (ret == BT_STATUS_SUCCESS){
        OS_Report("[SDAP][DB] Open SDAP Devic DB: Success");
    } else {
        OS_Report("[SDAP][DB] Open SDAP Devic DB: Fail with %d", ret);
    }

    return ret;
}

BtStatus SDAP_CloseDB(void)
{
    BtStatus ret = BT_STATUS_FAILED;

    ret = DDB_CloseDB(&g_sdap_devdb);
    
    if (ret == BT_STATUS_SUCCESS){
        OS_Report("[SDAP][DB] Close SDAP Devic DB: Success");
    } else {
        OS_Report("[SDAP][DB] Close SDAP Devic DB: Fail with %d", ret);
    }

    return ret;
}

BtStatus SDAP_InsertDevDBRecord(SdapDevDBRecord* record)
{
    BtStatus ret = BT_STATUS_FAILED;

    if (record != NULL) {
        OS_Report("[SDAP][DB] %s() %02x:%02x:%02x:%02x:%02x:%02x", __FUNCTION__,
            record->bdaddr.addr[0],
            record->bdaddr.addr[1],
            record->bdaddr.addr[2],
            record->bdaddr.addr[3],
            record->bdaddr.addr[4],
            record->bdaddr.addr[5]);

        ret = DDB_AddDBRecord(&g_sdap_devdb, (U8*) record, (void*) record->bdaddr.addr);
        if (ret == BT_STATUS_SUCCESS) {
            ret = DDB_FlushDB(&g_sdap_devdb);
            OS_Report("[SDAP][DB] %s() DDB_FlushDB() returns: %d", __FUNCTION__, ret);

        } else {
            OS_Report("[SDAP][DB] %s() DDB_AddDBRecord() returns: %d", __FUNCTION__, ret);
        }    
    } else {
        OS_Report("[SDAP][DB] %s() Null record", __FUNCTION__);
    }

    return ret;
}

BtStatus SDAP_DeleteDevDBRecord(BD_ADDR* bdaddr)
{
    BtStatus ret = BT_STATUS_FAILED;

    if (bdaddr != NULL) {
        OS_Report("[SDAP][DB] %s() %02x:%02x:%02x:%02x:%02x:%02x", __FUNCTION__,
            bdaddr->addr[0],
            bdaddr->addr[1],
            bdaddr->addr[2],
            bdaddr->addr[3],
            bdaddr->addr[4],
            bdaddr->addr[5]);

        ret = DDB_DeleteDBRecord(&g_sdap_devdb, (const void*) bdaddr->addr);
        if (ret == BT_STATUS_SUCCESS) {
            ret = DDB_FlushDB(&g_sdap_devdb);
            OS_Report("[SDAP][DB] %s() DDB_FlushDB() returns: %d", __FUNCTION__, ret);

        } else {
            OS_Report("[SDAP][DB] %s() DDB_DeleteDBRecord() returns: %d", __FUNCTION__, ret);
        }    
    } else {
        OS_Report("[SDAP][DB] %s() Null bdaddr", __FUNCTION__);
    }

    return ret;

}

BtStatus SDAP_QueryDevDBRecord(SdapDevDBRecord* record)
{
    BtStatus ret = BT_STATUS_FAILED;

    if (record != NULL) {
        OS_Report("[SDAP][DB] %s() %02x:%02x:%02x:%02x:%02x:%02x", __FUNCTION__,
            record->bdaddr.addr[0],
            record->bdaddr.addr[1],
            record->bdaddr.addr[2],
            record->bdaddr.addr[3],
            record->bdaddr.addr[4],
            record->bdaddr.addr[5]);

        ret = DDB_FindDBRecord(&g_sdap_devdb, (const void*) record->bdaddr.addr, (U8*) record);
        OS_Report("[SDAP][DB] %s() DDB_FindDBRecord() returns: %d", __FUNCTION__, ret);
        
    } else {
        OS_Report("[SDAP][DB] %s() Null record", __FUNCTION__);
    }

    return ret;
}

