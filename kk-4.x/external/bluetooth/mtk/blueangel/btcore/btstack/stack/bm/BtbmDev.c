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

/*******************************************************************************
 *
 * Filename:
 * ---------
 * btbmInq.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions provide the service to MMI or JAVA to 
 *   make the operation of command and event
 *
 * Author:
 * -------
 * Dlight Ting(mtk01239)
 *      Create 2008/5/20
 *
  *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#include "bt_common.h"
#include "bt_feature.h"
#include "bluetooth_gap_struct.h"
#include "ddb.h"
#include "bttypes.h"
#include "btalloc.h"
#include "btconfig.h"
#include "sec.h"
#include "sdp.h"
#include "btproperty.h"
#include "btbm_adp.h"
#include "btbm.h"
#include "sdap.h"
#include "btaccctrl.h"
#include "btutils.h"
#include "osapi.h"
#include "btbm_int.h"

#if defined (__BT_2_1_BLOCK_DEVICE__)
#include "btbmblocklist.h"
#endif 

#ifdef MTK_BB_TESTING
#include "BtBBtesting.h"
#endif 
#include "stdlib.h"

#include "BtbmLeAdv.h"

#if !defined(__MTK_TARGET__)
#define BTBM_TIMER_TICKS (50)
#else 
#define BTBM_TIMER_TICKS (4.615)
#endif 

#ifdef MTK_UPF_TESTING
U8 UPF_BD_ADDR[6];
extern U8 UPF_SETADDR;
#endif /* MTK_UPF_TESTING */ 

/***************
Internal Globals 
****************/
/* MTK Bluetooth Manager Context */
extern BMCONTENT_T BMController;

extern BtHandler globalbtbmHandler;
extern BtHandler linkbmHandler;
extern const U8 SAMSUNG_410[];
extern U8 gaptester_discovery_ask_without_name;
extern void btbmGattHandleRemNameReqTimeout(EvmTimer *timer);
extern BtStatus btbmBleNameReq(BtDeviceContext *bdc, BtCallBack callback);
extern BtStatus BTBMGATT_Cancel(BtRemoteDevice *link);
BtStatus BTBMGATT_ServiceSearchAll(BD_ADDR *bdAddr, U8 *uuid, U8 size);

#ifdef __BT_4_0_BLE__
BtAdvEventHandler btbmDevBleEventHandler;
#endif

static void btbmUtilClearInquiryList(void)
{
    BtDeviceContext *bdc = 0;
    BD_ADDR addr;

    OS_MemSet((U8*) & BMController.inquiredDevices, 0, sizeof(BTBMInquiredDevice) * BTBM_ADP_MAX_INQUIRY_NO);
    /* Clear "In Range" status for all devices.  Clear before inquiry */
    while (DS_EnumDeviceList(&bdc) == BT_STATUS_SUCCESS)
    {
        if ((bdc->link == 0) && (bdc->under_used == BDS_NONE))
        {
            addr = bdc->addr;
            DS_DeleteDevice(&addr);
            bdc = 0;
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  btbmUtilCheckInquiryTime
 * DESCRIPTION
 *  The function is used to check the inquiry procedure timeout or not.
 * PARAMETERS
 *  void
 * RETURNS
 *  The time interval between the start inquiry time and current time.
 *****************************************************************************/
U32 btbmUtilCheckInquiryTime(void)
{
    U32 abs_time = 0;
    U32 inquiry_time = OS_GetSystemTime();

    if (inquiry_time >= BMController.mmi_inquiry_time)
    {
        abs_time = (inquiry_time - BMController.mmi_inquiry_time);
    }
    else
    {
        abs_time = ((0xFFFFFFFF - inquiry_time) + BMController.mmi_inquiry_time);
    }
    return abs_time;
}


/*****************************************************************************
 * FUNCTION
 *  btbmUtilGetNameTokenID
 * DESCRIPTION
 *  
 * PARAMETERS
 *  nameToken       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 btbmUtilGetNameTokenID(MeCommandToken *nameToken)
{
    if (nameToken == (&BMController.nameTok))
    {
        return 1;
    }
    else if (nameToken == (&BMController.nameTok2))
    {
        return 2;
    }
    else if (nameToken == (&BMController.nameTok3))
    {
        return 3;
    }
    #ifdef __INDEPENDENT_INQUIRY_FLOW__
    else if (nameToken == (&BMController.nameTok4))
    {
        return 4;
    }
    #endif
    else
    {
        return 0;
    }

}

#ifdef __INDEPENDENT_INQUIRY_FLOW__
U8 btbmIsInquiryOngoing(void){
    return (BMController.inquiry_state != BM_INQUIRY_NONE) ? 1 : 0;
}
#endif

BTBMInquiredDevice *btbmUtilGetInquiredDevice(BD_ADDR *bdAddr)
{
    int i;
    for (i = 0; i < BTBM_ADP_MAX_INQUIRY_NO; i++)
    {
        if (BMController.inquiredDevices[i].used == TRUE &&
            OS_MemCmp(BMController.inquiredDevices[i].remote_addr.addr, 6, bdAddr->addr, 6))
        {
            return &BMController.inquiredDevices[i];
        }
    }
    return NULL;
}


static BTBMInquiredDevice *btbmUtilAddInquiredDevice(BtInquiryResult *inqResult)
{
    int i;
    BtDeviceContext *bdc = 0;
    BTBMInquiredDevice *entry;

    bt_trace(BT_TRACE_FUNC, BT_BTBM_ADD_INQUIRED_DEVICE);
    BTBMDumpBdAddr(inqResult->bdAddr.addr);

    if ((entry = btbmUtilGetInquiredDevice(&inqResult->bdAddr)) != NULL)
    {
        /* entry->deviceNameSearchCount = 1; */
        bt_trace(BT_TRACE_FUNC, BT_BTBM_ADD_INQUIRED_DEVICE_DUPLICATE);
        return NULL;
    }
    
    bdc = DS_FindDevice(&inqResult->bdAddr);
    OS_Report("bdc=0x%x", bdc);
#ifdef __BT_4_0_BLE__
    if (bdc->devType != BT_DEV_TYPE_LE)
    {
        OS_Report("TP/DM/GIN/BV-01-C: Discovery result on BR/EDR: %02x,%02x,%02x,%02x,%02x,%02x",
            inqResult->bdAddr.addr[0],inqResult->bdAddr.addr[1],inqResult->bdAddr.addr[2],
            inqResult->bdAddr.addr[3],inqResult->bdAddr.addr[4],inqResult->bdAddr.addr[5]);
    }
#endif
    for (i = 0; i < BTBM_ADP_MAX_INQUIRY_NO; i++)
    {
        if (BMController.inquiredDevices[i].used == FALSE)
        {
            BMController.discovered_device_no++;
            BMController.inquiredDevices[i].used = TRUE;
            BMController.inquiredDevices[i].remote_addr = inqResult->bdAddr;
#ifdef __BT_4_0_BLE__
            BMController.inquiredDevices[i].addrType = inqResult->addrType;
            BMController.inquiredDevices[i].devType = inqResult->devType;
#endif
            BMController.inquiredDevices[i].bdc = bdc;
            return &BMController.inquiredDevices[i];
        }
    }

    if (i >= BTBM_ADP_MAX_INQUIRY_NO)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_INQ_RESULT_DDB_FULL, 1);
    }
    return NULL;
}

/*****************************************************************************
 * FUNCTION
 *  btbmUtilConnectedDeviceNameEntryFree
 * DESCRIPTION
 *  Check if there is any free stored name entry or existing entry for the bdAddr.
 * PARAMETERS
 *  bdAddr      [IN]        The address for the stored name entry
 * RETURNS
 *  void
 *****************************************************************************/
void btbmUtilConnectedDeviceNameEntryFree(BD_ADDR *bdAddr)
{
    BTBMRemoteNameSetting_T *entry = 0;
    Report(("btbmUtilConnectedDeviceNameEntryFree"));

    entry = btbmUtilConnectedDeviceNameEntryLookUp(bdAddr);
    BTBMDumpBdAddr(bdAddr->addr);

    if (entry != 0)
    {
        entry->link = 0;
        entry->dev_name_len = 0;
        entry->dev_name[0] = 0;
    }
    btbmHandlerHFPInquiryBlackList();
}


/*****************************************************************************
 * FUNCTION
 *  btbmUtilConnectedDeviceNameEntryLookUp
 * DESCRIPTION
 *  The function is used to find out the map to BM name setting struct by address.
 * PARAMETERS
 *  bdAddr      [IN]        The input Bluetooth address
 * RETURNS
 *  BTBMRemoteNameSetting_T The stored name sturct for the address or none.
 *****************************************************************************/
BTBMRemoteNameSetting_T *btbmUtilConnectedDeviceNameEntryLookUp(BD_ADDR *bdAddr)
{
    int i = 0;

    BTBMRemoteNameSetting_T *entry = 0;
    Report(("btbmUtilConnectedDeviceNameEntryLookUp"));

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        entry = &BMController.DevRemNameTbl[i];
        if (entry->link != 0)
        {
            if (OS_MemCmp(BMController.DevRemNameTbl[i].link->bdAddr.addr, 6, bdAddr->addr, 6))
            {
                OS_Report("btbmUtilConnectedDeviceNameEntryLookUp i=%d", i);                
                BTBMDumpBdAddr(bdAddr->addr);
                return entry;
            }
        }
    }
    return 0;
}

/*****************************************************************************
 * FUNCTION
 *  btbmUtilConnectedDeviceNameEntryAllocate
 * DESCRIPTION
 *  The function is used to find out the connected device map to BM name setting struct.
 * PARAMETERS
 *  remDev      [IN]        Active ACL connected device
 * RETURNS
 *  BTBMRemoteNameSetting_T The stored name sturct for the ACL link.
 *****************************************************************************/
BTBMRemoteNameSetting_T *btbmUtilConnectedDeviceNameEntryAllocate(BtRemoteDevice *remDev)
{
    int i = 0;

    Report(("btbmUtilConnectedDeviceNameEntryAllocate"));
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if (BMController.DevRemNameTbl[i].link == remDev)
        {
            if (BMController.DevRemNameTbl[i].OwnerofNameTok2 == TRUE)
            {
                return 0;
            }
            else
            {
                BMController.DevRemNameTbl[i].wait_for_name_req = TRUE;
                return &(BMController.DevRemNameTbl[i]);
            }
        }
    }
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if (BMController.DevRemNameTbl[i].link == 0)
        {
            BMController.DevRemNameTbl[i].link = remDev;
            BMController.DevRemNameTbl[i].wait_for_name_req = TRUE;
            return &(BMController.DevRemNameTbl[i]);
        }
    }
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_LINK_DEVICE_TABLE_FULL);
    return 0;
}

/*****************************************************************************
 * FUNCTION
 *  btbmUtilConnectedDeviceNameEntryUpdate
 * DESCRIPTION
 *  The function is used to find out the connected device map to BM name setting struct.
 * PARAMETERS
 *  remDev      [IN]        Active ACL connected device
 * RETURNS
 *  BTBMRemoteNameSetting_T The stored name sturct for the ACL link.
 *****************************************************************************/
BtStatus btbmUtilConnectedDeviceNameEntryUpdate(BD_ADDR *bdAddr, char *name)
{
    int i, len;
    BTBMRemoteNameSetting_T *entry;

    Report(("btbmUtilConnectedDeviceNameEntryUpdate"));

    if (name && (len = OS_StrLen(name)) > 0)
    {
        len = min(len, BTBM_ADP_MAX_NAME_LEN);
        for (i = 0; i < NUM_BT_DEVICES; i++)
        {
            entry = &BMController.DevRemNameTbl[i];
            if (entry->link != 0 && OS_MemCmp(entry->link->bdAddr.addr, 6, bdAddr->addr, 6) == TRUE)
            {
                OS_MemSet((U8*) entry->dev_name, 0, BTBM_ADP_MAX_NAME_LEN + 1);
                OS_MemCopy((U8*) entry->dev_name, (U8*) name, len);
                entry->dev_name_len = len;
                break;
            }
        }
        return BT_STATUS_SUCCESS;
    }
    else
    {
        return BT_STATUS_INVALID_PARM;
    }
}



#ifdef __BT_4_0_BLE__

/*****************************************************************************
 * FUNCTION
 *  btbmUtilConnectedDeviceNameEntryAllocate
 * DESCRIPTION
 *  The function is used to find out the connected device map to BM name setting struct.
 * PARAMETERS
 *  remDev      [IN]        Active ACL connected device
 * RETURNS
 *  BTBMRemoteNameSetting_T The stored name sturct for the ACL link.
 *****************************************************************************/
void btbmBLEUtilConnectedDeviceNameEntryAllocate(BtRemoteDevice *remDev, U8 *name, U16 length)
{
    int i = 0;
    BTBMRemoteNameSetting_T *entry;

    entry = btbmUtilConnectedDeviceNameEntryLookUp(&remDev->bdAddr);
    Report(("btbmBLEUtilConnectedDeviceNameEntryAllocate"));

    if(entry !=0)
    {
        Report(("btbmBLEUtilConnectedDeviceNameEntryAllocate not empty"));
        OS_MemSet((U8 *)entry->dev_name,0, sizeof(entry->dev_name));
        OS_MemCopy((U8 *)entry->dev_name, name, length);
        entry->dev_name_len = length;
    }
    else
    {
        for (i = 0; i < NUM_BT_DEVICES; i++)
        {
            if (BMController.DevRemNameTbl[i].link == 0)
            {
                Report(("btbmBLEUtilConnectedDeviceNameEntryAllocate empty, assign name"));
            
                BMController.DevRemNameTbl[i].link = remDev;
                entry = &BMController.DevRemNameTbl[i];
                OS_MemSet((U8 *)entry->dev_name,0, sizeof(entry->dev_name));
                OS_MemCopy((U8 *)entry->dev_name, name, length);
                entry->dev_name_len = length;    
                break;
            }
        }

    }
    return;
}


U8 btbmBLEUtilCheckDeviceNameAvailable(BD_ADDR *bdAddr)
{
    BTBMRemoteNameSetting_T *nameResult;
    BTBMInquiredDevice *inqResult;

    nameResult = btbmUtilConnectedDeviceNameEntryLookUp(bdAddr);
    Report(("btbmBLEUtilCheckDeviceNameAvailable item:%08x",nameResult));
    if(nameResult !=0)
        return 1;
    else
	    return 0;
}
#endif

static BtStatus btbmUtilIsMatchCoD(U32 filter_cod, U32 cod)
{
    U32 major_service;
    U32 major_device;
    U32 minor_device;

    bt_trace(TRACE_FUNC, BT_BTBM_MATCH_COD, filter_cod, cod);
    if (filter_cod == 0xFFFFFFFF)
    {
        return BT_STATUS_SUCCESS;
    }
    else
    {
        major_service = 0x00FFE000 & filter_cod;
        major_device = 0x00001F00 & filter_cod;
        minor_device = 0x000000FC & filter_cod;
        if ((major_service & cod) != 0)
        {
            if ((major_device == 0) && (minor_device == 0))
            {
                return BT_STATUS_SUCCESS;
            }
            else if (((cod & 0x1F00) == major_device) && ((minor_device == 0) || ((cod & 0xFF) == minor_device)))
            {
                return BT_STATUS_SUCCESS;
            }
        }
    }
    return BT_STATUS_FAILED;
}

#ifdef MTK_UPF_TESTING
static BOOL btbmUtilIsMatchUPFAddr(U8 *addr)
{
    if (UPF_SETADDR == 1)
    {
        BTBMDumpBdAddr(UPF_BD_ADDR);
        if (UPF_BD_ADDR[0] != 0 || UPF_BD_ADDR[1] != 0 || UPF_BD_ADDR[2] != 0 ||
            UPF_BD_ADDR[3] != 0 || UPF_BD_ADDR[4] != 0 || UPF_BD_ADDR[5] != 0)
        {
            if (OS_MemCmp(addr, 6, (U8*) UPF_BD_ADDR, 6))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}
#endif

#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
/*****************************************************************************
 * FUNCTION
 *  btbmUtilParseInquiryEirResponse
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdc             [IN]        
 *  eir_reponse     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void btbmUtilParseInquiryEirResponse(BtDeviceContext *bdc, U8 *eir_reponse)
{
    U8 eir_value;
    U8 eir_length;
    U8 *eir_ptr = NULL;
    U8 loop_counter = 0;
    U8 total_count = 0;
    U32 service_list[6];
    U8 uuid_size;
    U8 info_more = 0;
    U16 uuid_16;
    U8 i = 0;
    U8 uuid_offset = 0;
    BOOL parse_uuid;
    BOOL support_service_list = FALSE;

    eir_ptr = eir_reponse;
    uuid_size = 0;
    bdc->support_service_list = BT_EIR_SERVICE_INFO_NONE;
    bdc->device_name_length = 0;
    bdc->device_name = NULL;
    
    bt_trace(TRACE_GROUP_1, BTLOG_EIR_RESPONSE);
    for (i = 0; i < 240; i += 10)
    {
        bt_trace(
            TRACE_GROUP_1,
            BTLOG_x02Xx02Xx02Xx02Xx02Xx02Xx02Xx02Xx02Xx02X,
            eir_reponse[i],
            eir_reponse[i + 1],
            eir_reponse[i + 2],
            eir_reponse[i + 3],
            eir_reponse[i + 4],
            eir_reponse[i + 5],
            eir_reponse[i + 6],
            eir_reponse[i + 7],
            eir_reponse[i + 8],
            eir_reponse[i + 9]);
    }
    OS_MemSet((U8*) service_list, 0, sizeof(service_list));

    while (total_count < 240)
    {
        eir_length = eir_ptr[0];
        if (eir_length != 0x00)
        {
            eir_value = eir_ptr[1];
            uuid_size = 0;
            parse_uuid = FALSE;
            OS_Report("LE: parse eir_length:%d eir_value:0x%x", eir_length, eir_value);
            switch (eir_value)
            {
#ifdef __BT_4_0_BLE__
                case BT_EIR_FLAGS:
                    bdc->adFlag = eir_ptr[2];
                    bdc->devType = ((eir_ptr[2] & BEF_BR_EDR_NOT_SUPPORTED) ||\
                                    (bdc->addrType == LE_ADDR_RANDOM)) ? BT_DEV_TYPE_LE : BT_DEV_TYPE_BR_EDR_LE;
                    DDB_DEV_TYPE_AddRecord(bdc->addr, bdc->devType);
                    bt_trace(TRACE_GROUP_1, BT_LE_ADFLAG, bdc->adFlag, bdc->devType);
                    break;
#endif
                case BT_EIR_NAME_SHORTENED:
                case BT_EIR_NAME_COMPLETE:
                    bdc->device_name = eir_ptr + 2;
                    bdc->device_name_length = eir_length - 1;
                    if (bdc->device_name_length > BTBM_ADP_MAX_NAME_LEN)
                    {
                        bdc->device_name_length = BTBM_ADP_MAX_NAME_LEN;
                    }
                    break;
                case BT_EIR_SERVICE_CLASS_16UUID_MORE:
                    info_more = 1;
                    /* CONTINUE TO NEXT case */
                case BT_EIR_SERVICE_CLASS_16UUID_COMPLETE:
                    uuid_offset = 2;
                    parse_uuid = TRUE;
                    break;
                case BT_EIR_SERVICE_CLASS_32UUID_MORE:
                    info_more = 1;
                    /* CONTINUE TO NEXT case */
                case BT_EIR_SERVICE_CLASS_32UUID_COMPLETE:
                    uuid_offset = 4;
                    parse_uuid = TRUE;
                    break;
                case BT_EIR_SERVICE_CLASS_128UUID_MORE:
                    info_more = 1;
                    /* CONTINUE TO NEXT case */
                case BT_EIR_SERVICE_CLASS_128UUID_COMPLETE:
                    uuid_offset = 16;
                    parse_uuid = TRUE;
                    break;
                default:
                    break;
            }
            if (parse_uuid)
            {
                for (uuid_size = 0; uuid_size < (eir_length - 1); uuid_size += uuid_offset)
                {
                    uuid_16 = (eir_ptr[2 + uuid_size] | ((U16) eir_ptr[3 + uuid_size] << 8));
                    bt_trace(TRACE_GROUP_1, BTLOG_UUID16x04X, uuid_16);
                    if ((uuid_16 < 0x1120) && (uuid_16 >= 0x1100))
                    {
                        support_service_list = TRUE;
                        service_list[0] |= ((U32) 1 << (uuid_16 - 0x1100));
                    }
                    else if ((uuid_16 < 0x1140) && (uuid_16 >= 0x1120))
                    {
                        support_service_list = TRUE;
                        service_list[1] |= ((U32) 1 << (uuid_16 - 0x1120));
                    }
                    else if ((uuid_16 < 0x1220) && (uuid_16 >= 0x1200))
                    {
                        support_service_list = TRUE;
                        service_list[2] |= ((U32) 1 << (uuid_16 - 0x1200));
                    }
                    else if ((uuid_16 < 0x1320) && (uuid_16 >= 0x1300))
                    {
                        support_service_list = TRUE;
                        service_list[3] |= ((U32) 1 << (uuid_16 - 0x1300));
                    }
                    else if ((uuid_16 < 0x1420) && (uuid_16 >= 0x1400))
                    {
                        support_service_list = TRUE;
                        service_list[4] |= ((U32) 1 << (uuid_16 - 0x1400));
                    }
                    else if ((uuid_16 < 0x1820) && (uuid_16 >= 0x1800))
                    {
                        support_service_list = TRUE;
                        service_list[5] |= ((U32) 1 << (uuid_16 - 0x1800));
                    }
                    else
                    {
                        info_more = 1;
                    }
                }
            }

            eir_ptr += eir_length + 1;  /* Header move to the next value */
            total_count += (eir_length + 1);
        }
        else
        {
            bt_trace(TRACE_GROUP_1, BTLOG_EIR_THE_LAST_ONE);
            break;
        }
        loop_counter++;
        if (loop_counter > 30)
        {
            break;
        }
    }

    if (support_service_list)
    {
        if (info_more == 1)
        {
            bdc->support_service_list = BT_EIR_SERVICE_INFO_MORE;
        }
        else
        {
            bdc->support_service_list = BT_EIR_SERVICE_INFO_COMPLETED;
        }
        OS_MemCopy((U8*) bdc->eir_response, (U8*) service_list, sizeof(service_list));
    }
}
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 

static void btbmSendDiscoveryCancelCfm(U8 result, U8 total_no)
{
    BMCallbackParms parms;
    
    parms.event = BTBM_EVENT_DISCOVERY_CANCEL_COMPLETE_RESULT;
    parms.p.DiscoveryCancelCompleteResult.result = result;
    parms.p.DiscoveryCancelCompleteResult.total_no = total_no;
    BMController.bmCB(&parms); 
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
}


static void btbmSendReadRemoteNameCfm(
        U8 result,
        btbm_bd_addr_t bd_addr,
        U8 name_len,
        U8 *name,
        U32 cod)
{

    BMCallbackParms parms;
    
    parms.event = BTBM_EVENT_READ_REMOTE_DEVICE_NAME_RESULT;
    parms.p.ReadRemoteNameResult.result = result;
    parms.p.ReadRemoteNameResult.bd_addr = bd_addr;
    parms.p.ReadRemoteNameResult.name_len = name_len;
    parms.p.ReadRemoteNameResult.name = name;
    parms.p.ReadRemoteNameResult.cod = cod;
    BMController.bmCB(&parms); 
}

void btbmSendRemoteNameInd(
        btbm_bd_addr_t bd_addr,
        U8 name_len,
        U8 *name)
{

    BMCallbackParms parms;
    
    parms.event = BTBM_EVENT_REMOTE_DEVICE_NAME_INDICATION;
    parms.p.RemoteNameIndication.bd_addr = bd_addr;
    parms.p.RemoteNameIndication.name_len = name_len;
    parms.p.RemoteNameIndication.name = name;
    BMController.bmCB(&parms); 
}

static void btbmSendReadRemoteNameCancelCfm(U8 result)
{
    BMCallbackParms parms;
    
    parms.event = BTBM_EVENT_READ_REMOTE_DEVICE_NAME_CANCEL_RESULT;
    parms.p.ReadRemoteNameResult.result = result;
    BMController.bmCB(&parms); 
}


#ifdef __BT_4_0_BLE__
static void btbmSendDiscoveryResultCfm(BOOL is_update,
                                  btbm_bd_addr_t bd_addr,
                                  btbm_device_type dev_type,
                                  U8 rssi, U8 eir_supported, 
                                  U8 uuid_list_supported, U8 servlist_completed,
                                  U32 *uuid_list,
                                  U32 cod, U8 name_len, U8 *name)
#elif defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
static void btbmSendDiscoveryResultCfm(BOOL is_update,
                                  btbm_bd_addr_t bd_addr,
                                  U8 rssi, U8 eir_supported, 
                                  U8 uuid_list_supported, U8 servlist_completed,
                                  U32 *uuid_list,
                                  U32 cod, U8 name_len, U8 *name)
#else
static void btbmSendDiscoveryResultCfm(BOOL is_update, btbm_bd_addr_t bd_addr,
                                  U32 cod, U8 name_len, U8 *name)
#endif                                  
{
    BMCallbackParms parms;
    
    parms.event = BTBM_EVENT_DISCOVERY_DEVICE_RESULT;
    parms.p.DiscoveryDeviceResult.is_update = is_update;
    parms.p.DiscoveryDeviceResult.bd_addr = bd_addr;
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
    parms.p.DiscoveryDeviceResult.rssi = rssi;
    parms.p.DiscoveryDeviceResult.eir_supported = eir_supported;
    parms.p.DiscoveryDeviceResult.uuid_list_supported = uuid_list_supported;
    parms.p.DiscoveryDeviceResult.servlist_completed = servlist_completed;
    parms.p.DiscoveryDeviceResult.uuid_list = uuid_list;
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
    parms.p.DiscoveryDeviceResult.cod = cod;
    parms.p.DiscoveryDeviceResult.name_len = name_len;
    parms.p.DiscoveryDeviceResult.name = name;
#ifdef __BT_4_0_BLE__
    parms.p.DiscoveryDeviceResult.dev_type = dev_type;
#endif
    BMController.bmCB(&parms); 

}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapInquiry
 * DESCRIPTION
 *  This function is used to inquiry device.
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 *  bt_bm_discovery_req_struct
 *  cod:            The device class of device
 *  inquiry_length:    The max inquiry time * 1.28 s
 *  inquiry_number:  The max inquiry number
 *  access_mode:     Device to listen the Generic access channel or Limited access channel
 *  discovery_mode: Discovery with name discovery or no name discovery
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapInquiry(bt_bm_discovery_req_struct *btbm_adp_input_msg)
{
    if (BMController.inquiry_loop_timer.func != 0)
    {
        bt_trace(TRACE_GROUP_1, INQUIRY_IDLE_TIMER_TIMEOUT);
        Assert(0);
    }
    #ifdef __INDEPENDENT_INQUIRY_FLOW__
    if (BMController.inquiry_state != BM_INQUIRY_NONE)
    #else
    if (BMController.bm_opactivity != BM_OP_NONE)
    #endif
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_PREVIOUS_ONGOING, BMController.bm_opactivity);
        btbmHandleDiscoveryComplete(BTBM_ADP_FAILED, 0x00);
        return;
    }
    if(!HCI_IsValidAclPacketLen())
    {
        BMController.inquiry_data = 0x10;
    }
    else
        BMController.inquiry_data = 0x0;

#ifdef MTK_UPF_TESTING
    CONVERT_BDADDRSRC2ARRAY(UPF_BD_ADDR, &btbm_adp_input_msg->bd_addr);
    BTBMDumpBdAddr(UPF_BD_ADDR);
#endif

    btbmUtilClearInquiryList();
    #ifndef __INDEPENDENT_INQUIRY_FLOW__
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);
    BMController.bm_opactivity = BM_OP_INQUIRY;
    #endif
    BMController.discovered_device_no = 0x00;
    BMController.filter_cod = btbm_adp_input_msg->cod;
    if (BMController.filter_cod == 0x00)
    {
        BMController.filter_cod = 0xFFFFFFFF;
    }

#ifdef __BQE_TESTER_
    if (gaptester_discovery_ask_without_name == 1)
    {
        BMController.inquiry_with_name = FALSE;   /* without ask name */
    }
    else
    {
        BMController.inquiry_with_name = (btbm_adp_input_msg->discovery_mode == BTBM_DISCOVERY_WITH_NAME) ? TRUE : FALSE;
    }
#else /* __BQE_TESTER_ */ 
    BMController.inquiry_with_name = (btbm_adp_input_msg->discovery_mode == BTBM_DISCOVERY_WITH_NAME) ? TRUE : FALSE;
#endif /* __BQE_TESTER_ */ 
    BMController.mmi_inquiry_length = btbm_adp_input_msg->inquiry_length;
    BMController.mmi_inquiry_number = min(btbm_adp_input_msg->inquiry_number, BTBM_ADP_MAX_INQUIRY_NO);
    BMController.mmi_inquiry_time = OS_GetSystemTime();
    BMController.access_mode = btbm_adp_input_msg->access_mode;

#ifdef __BT_4_0_BLE__
    BMController.dual_discovery_type = btbm_adp_input_msg->discovery_type;
#endif
    /* Initialize the index of cust data */
#ifdef __BT_USE_CUST_DATA__
    BMController.loop_inquiry_cust_tx_pwr_idx = 0;
    BMController.loop_inquiry_cust_len_idx = 0;
    BMController.loop_inquiry_cust_num_idx = 0;
#endif
    btbmStartSpecialSlaveProcedure();
    btbmHandlerInquiryLoop();
    return;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapInquiryCancel
 * DESCRIPTION
 *  This function is used to make the command to cancel inquiry operation.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapInquiryCancel(void)
{
    BtStatus status;
    BTBMInquiredDevice *inqResult = NULL;
    BtEvent event;
    U8 deviceNo;
    MeCommandToken *token;

    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTINQUIRY_CANCEL_BM_OPACTIVITYBT_DEBUGx02X, BMController.bm_opactivity);

#ifdef __INDEPENDENT_INQUIRY_FLOW__
    token = &BMController.nameTok4;
#else
    token = &BMController.nameTok;
#endif
    
    if ((BMController.inquiry_loop_timer.func != 0) || (BMController.inquiry_preprocess_timer.func != 0))
    {
        bt_trace(TRACE_GROUP_1, INQUIRY_IN_GUIDE_TIME);
        EVM_ResetTimer(&BMController.inquiry_loop_timer);
        EVM_ResetTimer(&BMController.inquiry_preprocess_timer);
        if (BMController.inquiry_preprocess_timer.func != 0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTINQUIRY_CANCEL_WHEN_INQUIRY_PREPROCESS_TIMER_RUNNING);
        }
        if (token->tokenTimer.func != 0)
        {
            bt_trace(TRACE_GROUP_1, MTKBMCNAME_CANCEL_TIMERFUNC__0);
            /* During inquiry procedure, asking name is not yet responsed from controller in 10sec */
            EVM_ResetTimer(&token->tokenTimer);
        }
        #ifdef __INDEPENDENT_INQUIRY_FLOW__
        OS_Report("inquiry_state is %u", BMController.inquiry_state);
        if (BMController.inquiry_state == BM_INQUIRY_DELAY || BMController.inquiry_state == BM_INQUIRY_PREPROCESS){
            BMController.inquiry_state = BM_INQUIRY_NONE;
        }
        #else
        if (BMController.bm_opactivity == BM_OP_INQUIRY || BMController.bm_opactivity == BM_OP_NAMEREQ)
        {
            BMController.bm_opactivity = BM_OP_NONE;
        }
        #endif
        //BMController.inquiry_preprocess_timer.func = 0;
        // BT_State_Report2WLAN(0, BT_STATE_INQUIRY);
        btbmSendDiscoveryCancelCfm(BTBM_ADP_SUCCESS, BMController.discovered_device_no);
        BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
        btbmHandleDiscoveryComplete(BTBM_ADP_SUCCESS, BMController.discovered_device_no);
    }
    else
    {
        #ifdef __INDEPENDENT_INQUIRY_FLOW__
        switch (BMController.inquiry_state)
        #else
        switch (BMController.bm_opactivity)
        #endif
        {
            #ifdef __INDEPENDENT_INQUIRY_FLOW__
            case BM_INQUIRY_SEARCH:
            #else
            case BM_OP_INQUIRY:
            #endif
                #ifdef __INDEPENDENT_INQUIRY_FLOW__
                BMController.inquiry_state = BM_INQUIRY_CANCEL;
                #else
                BMController.bm_opactivity = BM_OP_INQCANCEL;
                #endif
#ifdef __BT_4_0_BLE__
                bt_trace(TRACE_GROUP_1, BT_LE_CURRENT_INQUIRY_TYPE, BMController.dual_inquiry_type_cur);
                if (BMController.dual_inquiry_type_cur & BT_BM_DUAL_INQUIRY_BR_EDR)
                {
                    ME_CancelInquiry();
                }
                if (BMController.dual_inquiry_type_cur & BT_BM_DUAL_INQUIRY_LE)
                {
                    BtBmLeAdvDisabled(BMController.LeAdvIndex, &btbmDevBleEventHandler);
                    event.eType = BTEVENT_INQUIRY_CANCELED;
                    btbmHandlerInquiryCancel(&event);
                }
#else
                ME_CancelInquiry();
#endif
                break;
            #ifdef __INDEPENDENT_INQUIRY_FLOW__
            case BM_INQUIRY_NAME_QUERY:
            #else
            case BM_OP_NAMEREQ:
            #endif
                bt_trace(TRACE_GROUP_1, MAKE_REMOTE_NAME_REQUEST_CANCEL_COMMAND);
                #ifdef __INDEPENDENT_INQUIRY_FLOW__
                BMController.inquiry_state = BM_INQUIRY_CANCEL;
                #else
                BMController.bm_opactivity = BM_OP_INQCANCEL;
                #endif
                status = ME_CancelGetRemoteDeviceName(token);

                if (status != BT_STATUS_PENDING){
                    token->callback = NULL;
                }

                bt_trace(TRACE_GROUP_1, REMOTE_NAME_REQUEST_STATUSBT_DEBUGx02X, status);
                if (status != BT_STATUS_SUCCESS)
                {
                    if (status == BT_STATUS_FAILED)
                    {
                            event.eType = BTEVENT_INQUIRY_CANCELED;
                            btbmHandlerInquiryCancel(&event);
                            break;
                    }
                    #ifndef __INDEPENDENT_INQUIRY_FLOW__
                    else
                    {
                        BMController.bm_opactivity = BM_OP_INQCANCEL;
                    }
                    #endif
                    break;
                }
                else
                {
                    #ifdef __INDEPENDENT_INQUIRY_FLOW__
                    BMController.inquiry_state = BM_INQUIRY_NONE;
                    #else
                    BMController.bm_opactivity = BM_OP_NONE;
                    #endif
                    btbmSendDiscoveryCancelCfm(BTBM_ADP_SUCCESS, BMController.discovered_device_no);
                    btbmHandleDiscoveryComplete(BTBM_ADP_SUCCESS, BMController.discovered_device_no);
                }
                break;
            default:
                #ifdef __INDEPENDENT_INQUIRY_FLOW__
                BMController.inquiry_state = BM_INQUIRY_NONE;
                btbmSendDiscoveryCancelCfm(BTBM_ADP_SUCCESS, 0);
                #else
                if (BMController.nameTok.tokenTimer.func != 0)
                {
                    OS_Report("Current inquiry cancel opactivity:%d",BMController.bm_opactivity);
                    if(BMController.bm_opactivity == BM_OP_BONDING){
                        btbmSendDiscoveryCancelCfm(BTBM_ADP_SUCCESS, 0);
                        btbmHandleDiscoveryComplete(BTBM_ADP_FAILED, 0);
                    }else{
                        Assert(0);
                    }
                }else{
                    btbmSendDiscoveryCancelCfm(BTBM_ADP_SUCCESS, 0);
                }
                #endif
                break;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapRemoteNameReq
 * DESCRIPTION
 *  This function is used to make the read the remote device name
 * PARAMETERS
 *  btbm_adp_input_msg      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapRemoteNameReq(bt_bm_read_remote_name_req_struct *btbm_adp_input_msg)
{
    btbm_bd_addr_t *remoteaddr;
    BtCodRecord record;
    BtStatus status = BT_STATUS_FAILED;
    BTBMRemoteNameSetting_T *entry = 0;
    BD_ADDR bdaddr;
    U32 cod = 0;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_PREVIOUS_ONGOING, BMController.bm_opactivity);
    remoteaddr = &(btbm_adp_input_msg->bd_addr);
    CONVERT_BDADDR2ARRAY(bdaddr.addr, remoteaddr->lap, remoteaddr->uap, remoteaddr->nap);
    if ((entry = btbmUtilConnectedDeviceNameEntryLookUp(&bdaddr)) != 0 && entry->dev_name_len > 0)
    {
        if (DDB_COD_FindRecord(&bdaddr, &record) == BT_STATUS_SUCCESS)
        {
            cod = record.cod;
        }
        btbmSendReadRemoteNameCfm(
            BTBM_ADP_SUCCESS,
            btbm_adp_input_msg->bd_addr,
            entry->dev_name_len,
            (U8*) entry->dev_name,
            cod);
    }
    else
    {
        if (BMController.bm_opactivity == BM_OP_NONE)
        {
            BtDeviceContext *bdc = 0;
            BMController.bm_opactivity = BM_OP_NAMEREQ;

            if ((bdc = DS_FindDevice(&(bdaddr))) == NULL)
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_PSI_INFO_NOT_IN_DDB);
                status = BtbmAllocateFreeDevices(bdaddr, &bdc);
                if (status != BT_STATUS_SUCCESS)
                {
                    OS_Report("[BM][ERR] BTBMGapRemoteNameReq : BtbmAllocateFreeDevices failed");
                    return;
                }
            }
            bdc->under_used |= BDS_NAME_REQESTING;
            Report(("bdc->under_used=0x%x", bdc->under_used));
            /* Start a name query for this device */
#ifdef __BT_4_0_BLE__
  #ifdef __BT_LE_STANDALONE__
            status = btbmBleNameReq(bdc, btbmHandlerSingleNameQuery);
  #else
            OS_Report("btbmNameReq: devType=%d", bdc->devType);
            if (bdc->devType == BT_DEV_TYPE_LE)
            {
                status = btbmBleNameReq(bdc, btbmHandlerSingleNameQuery);
            }
            else
            {
            status = btbmNameReq(&BMController.nameTok3, bdc, btbmHandlerSingleNameQuery);
            }
  #endif
#else
            status = btbmNameReq(&BMController.nameTok3, bdc, btbmHandlerSingleNameQuery);

#endif

        }
        if (status != BT_STATUS_PENDING)
        {
            BMController.bm_opactivity = BM_OP_NONE;
            btbmSendReadRemoteNameCfm(BTBM_ADP_FAILED, btbm_adp_input_msg->bd_addr, 0x00, 0x00, 0x00);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapRemoteNameCancelReq
 * DESCRIPTION
 *  This function is used to cancel reading the remote device name
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapRemoteNameCancelReq(void)
{
    BtStatus status = BT_STATUS_FAILED;
    if (BMController.bm_opactivity == BM_OP_NAMEREQ && BMController.nameTok3.callback != NULL)
    {
        bt_trace(TRACE_GROUP_1, MAKE_REMOTE_NAME_REQUEST_CANCEL_COMMAND);
        status = ME_CancelGetRemoteDeviceName(&BMController.nameTok3);
        bt_trace(TRACE_GROUP_1, REMOTE_NAME_REQUEST_STATUSBT_DEBUGx02X, status);
        if (status == BT_STATUS_PENDING){
            BMController.bm_opactivity = BM_OP_NAMECANCEL;
        }else{
            BMController.nameTok3.callback = NULL;
            btbmSendReadRemoteNameCancelCfm(BTBM_ADP_SUCCESS);
            Assert (status != BT_STATUS_FAILED);
        }
    }
    else
    {
        btbmSendReadRemoteNameCancelCfm(BTBM_ADP_FAILED);
    }
    //Assert (BMController.nameTok3.tokenTimer.func == 0);
}
#ifdef __BQE_TESTER_


/*****************************************************************************
 * FUNCTION
 *  BTBMGapSetFilterCoD
 * DESCRIPTION
 *  To set the filter cod
 * PARAMETERS
 *  setcod      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapSetFilterCoD(U32 setcod)
{
    MTKBMC(filter_cod) = setcod;
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapSetDiscoveryWithoutName
 * DESCRIPTION
 *  To set the discovery mode as Discovery without name, the pure inquiry procedure.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapSetDiscoveryWithoutName(void)
{
    MTKBMC(discovery_type) = BTBM_DISCOVERY_WITHOUT_NAME;
}

#endif /* __BQE_TESTER_ */ 


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerInquiryResult
 * DESCRIPTION
 *  To handle the inquiry result
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerInquiryResult(const BtEvent *Event)
{
    btbm_bd_addr_t addr;
    BtDeviceContext *bdc = 0;
    BtInquiryResult *inqResult = (BtInquiryResult *)&Event->p.inqResult;
    BTBMInquiredDevice *entry;
    BTBMRemoteNameSetting_T *nameEntry = 0;
    
    Report(("BMController.inquiry_data:%d",BMController.inquiry_data));
    if ((BMController.inquiry_data > 0) &&
        (random()%2 == 0x00))    
    {
#ifdef __BT_DLIGHT1_DEBUG__
        Report(("autumn btbmHandlerInquiryResult"));
#endif 
        return;
    }
    bt_trace(TRACE_GROUP_1, INQUIRY_RESULT, BMController.discovered_device_no);
    #ifdef __INDEPENDENT_INQUIRY_FLOW__
    if (BMController.inquiry_state != BM_INQUIRY_SEARCH)
    #else
    if (BMController.bm_opactivity != BM_OP_INQUIRY)
    #endif
    {
        OS_Report("btbmHandlerInquiryResult(): bm_opactivity=0x%x, ", BMController.bm_opactivity);
        return;
    }

    if (BMController.discovered_device_no >= BMController.mmi_inquiry_number)
    {
        bt_trace(TRACE_GROUP_1, BT_BTBM_INQ_RESULT_TOO_MUCH);
        return;
    }
    if ((bdc = DS_FindDevice(&inqResult->bdAddr)) == 0)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_INQ_RESULT_DDB_FULL, 0);
        return;
    }

    /* dev_name hold a name in temporary buffer, it should be reused. */
    bdc->device_name = NULL;
    bdc->device_name_length = 0;
    
    if (btbmUtilIsMatchCoD(BMController.filter_cod, inqResult->classOfDevice) != BT_STATUS_SUCCESS)
    {
        bt_trace(TRACE_GROUP_1, COD_NOT_MATCH);
        if (bdc && (bdc->link == 0) && (bdc->under_used == BDS_NONE))
        {
            DS_DeleteDevice(&bdc->addr);  /* so that other device type won't occupied the inquired list */
        }
        return;
    }
#ifdef MTK_UPF_TESTING
    if (btbmUtilIsMatchUPFAddr(inqResult->bdAddr.addr) == FALSE)
    {
        DS_DeleteDevice(&inqResult->bdAddr);  /* so that other device type won't occupied the inquired list */
        return;
    }
#endif
#ifdef MTK_BB_TESTING
    if (BTBBTestingCases(BTBBTESTING_AUTO_SLEEP_INQUIRY))
    {
        return;
    }
#endif

#ifdef __BT_4_0_BLE__
    /* inqResult->devType only indicate LE or BR/EDR channel */
    if (inqResult->devType == BT_DEV_TYPE_LE)
    {
        OS_Report("Discovery result on LE: %02x,%02x,%02x,%02x,%02x,%02x, ADV Type=0x%x, AD Flag=0x%x",
            inqResult->bdAddr.addr[0],inqResult->bdAddr.addr[1],inqResult->bdAddr.addr[2],
            inqResult->bdAddr.addr[3],inqResult->bdAddr.addr[4],inqResult->bdAddr.addr[5], inqResult->advType, bdc->adFlag);
    }
    if (inqResult->devType == BT_DEV_TYPE_LE && inqResult->advType != HCI_ADV_IND)
    {
        OS_Report("Ignore advType=%d from LE device", inqResult->advType);
        /* Non-connectable LE device, no need to notify MMI */
        if ((bdc->link == 0) && (bdc->under_used == BDS_NONE))
        {
        DS_DeleteDevice(&inqResult->bdAddr);  /* so that other device type won't occupied the inquired list */
        }
        return;
    }
#endif

#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
    if (inqResult->supported_eir == 1)
    {
        btbmUtilParseInquiryEirResponse(bdc, inqResult->eirResponse);
#ifdef __BT_4_0_BLE__
        OS_Report("inqResult->advType=%d, bdc->advType=%d", inqResult->advType, bdc->devType);

        if(bdc->devType == BT_DEV_TYPE_BR_EDR_LE && inqResult->devType == BT_DEV_TYPE_LE){
            OS_Report("Ignore Dual mode device adv indication");
            if ((bdc->link == 0) && (bdc->under_used == BDS_NONE))
            {
                DS_DeleteDevice(&inqResult->bdAddr);  /* so that other device type won't occupied the inquired list */
            }
            return;
        }

        /* Ignore LE_DISCOVERABLE_MODE bit if received on BR/EDR channel */
        if (BMController.access_mode == BTBM_GENERAL_INQUIRY && 
            inqResult->devType != BT_DEV_TYPE_BR_EDR &&
            (bdc->adFlag & (BEF_LE_LIMITED_DISCOVERABLE_MODE | BEF_LE_GENERAL_DISCOVERABLE_MODE)) == 0)
        {
            OS_Report("Ignore non-discoverable LE/dual mode device");
            /* Non-discoverable LE/dual mode device, no need to notify MMI */
            if ((bdc->link == 0) && (bdc->under_used == BDS_NONE))
            {
                DS_DeleteDevice(&inqResult->bdAddr);  /* so that other device type won't occupied the inquired list */
            }
            else
            {
                OS_Report("bdc->link = 0x%x, bdc->under_used=0x%x", bdc->link, bdc->under_used);
            }
            return;
        }

        if (BMController.access_mode == BTBM_LIMITED_INQUIRY && 
            inqResult->devType != BT_DEV_TYPE_BR_EDR &&
            (bdc->adFlag & BEF_LE_LIMITED_DISCOVERABLE_MODE) == 0)
        {
            OS_Report("Our discovery mode is limited inquiry");
            OS_Report("Remote device type is LE or dual mode device");
            OS_Report("Remote device is not in limited dicsoverable mode");            
            OS_Report("Ignore non-limited discoverable LE/dual mode device");
            /* Non-discoverable LE/dual mode device, no need to notify MMI */
            if ((bdc->link == 0) && (bdc->under_used == BDS_NONE))
            {
                DS_DeleteDevice(&inqResult->bdAddr);  /* so that other device type won't occupied the inquired list */
            }
            else
            {
                OS_Report("bdc->link = 0x%x, bdc->under_used=0x%x", bdc->link, bdc->under_used);
            }
            return;
        }
#endif
    }
#endif

    if ((entry = btbmUtilAddInquiredDevice(inqResult)) != NULL)
    {
        U8 device_name_length = 0;
        U8 *device_name = NULL;

        CONVERT_ARRAY2BDADDR(&addr, bdc->addr.addr);
        /* ALPS00257388 : BtDeviceContext.device_name hold local variable or MEC(btEvent).p.inqResult 
          *                         that shall be not used later.
        */
        nameEntry = btbmUtilConnectedDeviceNameEntryLookUp(&bdc->addr);
        if (bdc->device_name || (nameEntry && nameEntry->dev_name_len))
        {
            if(bdc->device_name)
                OS_Report("bdc->device_name=%s", bdc->device_name);
            if(nameEntry && nameEntry->dev_name_len)
                OS_Report("nameEntry->dev_name_len=%u, nameEntry->dev_name=%s", nameEntry->dev_name_len, nameEntry->dev_name);
            //entry->istate |= BT_BM_ISTATE_NAME_REQ_FINISHED | BT_BM_ISTATE_NAME_REQ_SENT;
            entry->deviceNameAvailable = 1;
        }

        if (BMController.inquiry_with_name == TRUE)
        {
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
            /* ALPS00257388 : BtDeviceContext.device_name hold local variable or MEC(btEvent).p.inqResult 
              *                         that shall be not used later.
            */
            if (bdc->device_name)
            {
                device_name_length = bdc->device_name_length;
                device_name = bdc->device_name;
            }
            else if(nameEntry && nameEntry->dev_name_len)
            {
                device_name_length = nameEntry->dev_name_len;
                device_name = (U8 *)nameEntry->dev_name;
            }
#endif
        }

#ifdef __BT_4_0_BLE__
        Report(("Discovery devType:%d",bdc->devType));
        Report(("Discovery bdc->support_service_list:%d",bdc->support_service_list));
        if(bdc->devType == BT_DEV_TYPE_LE)
        {
            if (bdc->device_name)
            {
                device_name_length = bdc->device_name_length;
                device_name = bdc->device_name;
                OS_MemCopy(entry->queuedGattDevName, bdc->device_name, bdc->device_name_length);			
                entry->queuedGattDevNameLen = bdc->device_name_length;
                bdc->device_name = entry->queuedGattDevName;
            }		
        }

#endif
        if(BMController.inquiryShoAddress != 0
            || (device_name && device_name_length))
        {
            btbmSendDiscoveryResultCfm(FALSE, addr,
#ifdef __BT_4_0_BLE__
                                  bt_util_convert_dev(bdc->devType),
#endif
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
                                  bdc->rssi, 
                                  bdc->support_eir, 
                                  (U8)((bdc->support_service_list == BT_EIR_SERVICE_INFO_NONE) ? 0 : 1), 
                                  (U8)((bdc->support_service_list == BT_EIR_SERVICE_INFO_MORE) ? 0 : 1),
                                  bdc->eir_response,
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
                                  bdc->classOfDevice, 
                                  device_name_length, 
                                  device_name); 
        }
#ifdef __BT_4_0_BLE__		
        if (BMController.dual_discovery_type == BTBM_DISCOVERY_LE_ONLY)
        {
                /* 
                If search LE only, there is no inquiry completed event.
	        Try to fake an inquiry event, stop advertising, search device name;
	        If just find one device, then stop inquiry.
	        */
	        BtEvent event;
                event.errCode = BEC_NO_ERROR;
                event.eType = BTEVENT_INQUIRY_COMPLETE;
                btbmHandlerInquiryComplete(&event);
        }			
#endif
    }
}



/*****************************************************************************
 * FUNCTION
 *  btbmHandlerInquiryResult
 * DESCRIPTION
 *  To handle the inquiry result
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
#ifdef __BT_4_0_BLE__

void btbmHandlerInquiryBLEResult(const BtEvent *Event)
{
    btbm_bd_addr_t addr;
    BtDeviceContext *bdc = 0;
    BtInquiryResult *inqResult = (BtInquiryResult *)&Event->p.inqResult;
    BTBMInquiredDevice *entry;
    BTBMRemoteNameSetting_T *nameEntry = 0;
    
    Report(("BMController.inquiry_data:%d",BMController.inquiry_data));
    bt_trace(TRACE_GROUP_1, INQUIRY_RESULT, BMController.discovered_device_no);

    if ((bdc = DS_FindDevice(&inqResult->bdAddr)) == 0)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_INQ_RESULT_DDB_FULL, 0);
        return;
    }

    /* dev_name hold a name in temporary buffer, it should be reused. */
    bdc->device_name = NULL;
    bdc->device_name_length = 0;

    /* To make sure the devType is correct, parse EIR as earlier as possible */
    if (inqResult->supported_eir == 1)
    {
        btbmUtilParseInquiryEirResponse(bdc, inqResult->eirResponse);
        OS_Report("inqResult->advType=%d, bdc->advType=%d", inqResult->advType, bdc->devType);
    }
    
    #ifdef __INDEPENDENT_INQUIRY_FLOW__
    if (BMController.inquiry_state != BM_INQUIRY_SEARCH &&
        BMController.inquiry_state != BM_INQUIRY_NAME_QUERY)
    {
        OS_Report("btbmHandlerInquiryResult(): inquiry_state=0x%x, ", BMController.inquiry_state);
        return;
    }        
    #else
    if ((BMController.bm_opactivity != BM_OP_INQUIRY) &&
		(BMController.bm_opactivity != BM_OP_NAMEREQ))
    {
        OS_Report("btbmHandlerInquiryResult(): bm_opactivity=0x%x, ", BMController.bm_opactivity);
        return;
    }		
    #endif

    if (BMController.discovered_device_no >= BMController.mmi_inquiry_number)
    {
        bt_trace(TRACE_GROUP_1, BT_BTBM_INQ_RESULT_TOO_MUCH);
        return;
    }

    if (btbmUtilIsMatchCoD(BMController.filter_cod, inqResult->classOfDevice) != BT_STATUS_SUCCESS)
    {
        bt_trace(TRACE_GROUP_1, COD_NOT_MATCH);
        if (bdc && (bdc->link == 0) && (bdc->under_used == BDS_NONE))
        {
            DS_DeleteDevice(&bdc->addr);  /* so that other device type won't occupied the inquired list */
        }
        return;
    }
    /* inqResult->devType only indicate LE or BR/EDR channel */
    if (inqResult->devType == BT_DEV_TYPE_LE)
    {
        OS_Report("Discovery result on LE: %02x,%02x,%02x,%02x,%02x,%02x, ADV Type=0x%x, AD Flag=0x%x",
            inqResult->bdAddr.addr[0],inqResult->bdAddr.addr[1],inqResult->bdAddr.addr[2],
            inqResult->bdAddr.addr[3],inqResult->bdAddr.addr[4],inqResult->bdAddr.addr[5], inqResult->advType, bdc->adFlag);
    }
    if ((inqResult->devType == BT_DEV_TYPE_LE) && 
#ifdef __BT_4_0_ACTIVE_SCAN__        
        (inqResult->advType != HCI_SCAN_RSP) &&  
#endif        
        (inqResult->advType != HCI_ADV_IND))
    {
        OS_Report("Ignore advType=%d from LE device", inqResult->advType);
        /* Non-connectable LE device, no need to notify MMI */
        if (bdc && (bdc->link == 0) && (bdc->under_used == BDS_NONE))
        {        
            DS_DeleteDevice(&inqResult->bdAddr);  /* so that other device type won't occupied the inquired list */
        }
        return;
    }
    if (inqResult->supported_eir == 1)
    {
        //btbmUtilParseInquiryEirResponse(bdc, inqResult->eirResponse);
        //OS_Report("inqResult->advType=%d, bdc->advType=%d", inqResult->advType, bdc->devType);

        if(bdc->devType == BT_DEV_TYPE_BR_EDR_LE && inqResult->devType == BT_DEV_TYPE_LE){
            OS_Report("Ignore Dual mode device adv indication");
            if ((bdc->link == 0) && (bdc->under_used == BDS_NONE))
            {
                DS_DeleteDevice(&inqResult->bdAddr);  /* so that other device type won't occupied the inquired list */
            }
            return;
        }

        /* Ignore LE_DISCOVERABLE_MODE bit if received on BR/EDR channel */
        if (BMController.access_mode == BTBM_GENERAL_INQUIRY && 
            inqResult->devType != BT_DEV_TYPE_BR_EDR &&
            (bdc->adFlag & (BEF_LE_LIMITED_DISCOVERABLE_MODE | BEF_LE_GENERAL_DISCOVERABLE_MODE)) == 0)
        {
            OS_Report("Ignore non-discoverable LE/dual mode device");
            /* Non-discoverable LE/dual mode device, no need to notify MMI */
            if ((bdc->link == 0) && (bdc->under_used == BDS_NONE))
            {
                DS_DeleteDevice(&inqResult->bdAddr);  /* so that other device type won't occupied the inquired list */
            }
            else
            {
                OS_Report("bdc->link = 0x%x, bdc->under_used=0x%x", bdc->link, bdc->under_used);
            }
            return;
        }

        if (BMController.access_mode == BTBM_LIMITED_INQUIRY && 
            inqResult->devType != BT_DEV_TYPE_BR_EDR &&
            (bdc->adFlag & BEF_LE_LIMITED_DISCOVERABLE_MODE) == 0)
        {
            OS_Report("Our discovery mode is limited inquiry");
            OS_Report("Remote device type is LE or dual mode device");
            OS_Report("Remote device is not in limited dicsoverable mode");            
            OS_Report("Ignore non-limited discoverable LE/dual mode device");
            /* Non-discoverable LE/dual mode device, no need to notify MMI */
            if ((bdc->link == 0) && (bdc->under_used == BDS_NONE))
            {
                DS_DeleteDevice(&inqResult->bdAddr);  /* so that other device type won't occupied the inquired list */
            }
            else
            {
                OS_Report("bdc->link = 0x%x, bdc->under_used=0x%x", bdc->link, bdc->under_used);
            }
            return;
        }
    }

#ifdef __BT_4_0_ACTIVE_SCAN__ 
    if ((inqResult->devType == BT_DEV_TYPE_LE) && bdc && (bdc->device_name == 0))
    {
        OS_Report("Ignore empty name from LE device");
        return;        
    }
#endif    

    if ((entry = btbmUtilAddInquiredDevice(inqResult)) != NULL)
    {
        U8 device_name_length = 0;
        U8 *device_name = NULL;

        CONVERT_ARRAY2BDADDR(&addr, bdc->addr.addr);

        nameEntry = btbmUtilConnectedDeviceNameEntryLookUp(&bdc->addr);
        if (bdc->device_name || (nameEntry && nameEntry->dev_name_len))
        {
            if(bdc->device_name)
                OS_Report("bdc->device_name=%s", bdc->device_name);
            if(nameEntry && nameEntry->dev_name_len)
                OS_Report("nameEntry->dev_name_len=%u, nameEntry->dev_name=%s", nameEntry->dev_name_len, nameEntry->dev_name);
        }

        if (BMController.inquiry_with_name == TRUE)
        {
            if (bdc->device_name)
            {
                device_name_length = bdc->device_name_length;
                device_name = bdc->device_name;
            }
			else if(nameEntry && nameEntry->dev_name_len)
            {
                device_name_length = nameEntry->dev_name_len;
                device_name = (U8 *)nameEntry->dev_name;
            }
        }

        Report(("Discovery devType:%d",bdc->devType));
        Report(("Discovery bdc->support_service_list:%d",bdc->support_service_list));
        if (bdc->device_name)
        {
            device_name_length = bdc->device_name_length;
            device_name = bdc->device_name;
			OS_MemCopy(entry->queuedGattDevName, bdc->device_name, bdc->device_name_length);			
			entry->queuedGattDevNameLen = bdc->device_name_length;
			bdc->device_name = entry->queuedGattDevName;
        }		
        btbmSendDiscoveryResultCfm(FALSE, addr,
                              bt_util_convert_dev(bdc->devType),
                              bdc->rssi, 
                              bdc->support_eir, 
                              (U8)((bdc->support_service_list == BT_EIR_SERVICE_INFO_NONE) ? 0 : 1), 
                              (U8)((bdc->support_service_list == BT_EIR_SERVICE_INFO_MORE) ? 0 : 1),
                              bdc->eir_response,
                              bdc->classOfDevice, 
                              device_name_length, 
                              device_name);        
		if (BMController.dual_discovery_type == BTBM_DISCOVERY_LE_ONLY)
		{
			/* 
				If search LE only, there is no inquiry completed event.
				Try to fake an inquiry event, stop advertising, search device name;
				If just find one device, then stop inquiry.
			*/
			BtEvent event;
			event.errCode = BEC_NO_ERROR;
			event.eType = BTEVENT_INQUIRY_COMPLETE;
			btbmHandlerInquiryComplete(&event);
		}			

    }
}
#endif



/*****************************************************************************
 * FUNCTION
 *  btbmHandlerInquiryComplete
 * DESCRIPTION
 *  To handle the inquiry completed event
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerInquiryComplete(const BtEvent *Event)
{
    BtStatus status = BT_STATUS_SUCCESS;

    bt_trace(BT_TRACE_FUNC, BT_BTBM_INQ_COMPLETED);
    DDB_COD_Flush();
    #ifdef __INDEPENDENT_INQUIRY_FLOW__
    if (BMController.inquiry_state == BM_INQUIRY_CANCEL)
    #else
    if (BMController.bm_opactivity == BM_OP_INQCANCEL)
    #endif
    {    
        #ifdef __BT_4_0_BLE__
        BMController.dual_inquiry_type_cur &= ~BT_BM_DUAL_INQUIRY_BR_EDR;
        OS_Report("inquiry complete when activity is INQCANCEL. dual_inquiry_type_cur=%d", BMController.dual_inquiry_type_cur);
        if(BMController.dual_inquiry_type_cur == BT_BM_DUAL_INQUIRY_NONE)
        #endif
        {
            btbmSendDiscoveryCancelCfm(BTBM_ADP_SUCCESS, BMController.discovered_device_no);
            BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
            #ifdef __INDEPENDENT_INQUIRY_FLOW__
            BMController.inquiry_state = BM_INQUIRY_NONE;
            #else
            BMController.bm_opactivity = BM_OP_NONE;
            #endif
        }
        return;
    }
    #ifdef __INDEPENDENT_INQUIRY_FLOW__
    if (BMController.inquiry_state != BM_INQUIRY_SEARCH)
    {
        OS_Report("BTlog BTMTKBMC(inquiry_state):%u", BMController.inquiry_state);
        /* In LE single mode search, it is possible to complete twice, just ignore the 2'nd one inquiry completed event */
        return;
    }        
    #else
    if (BMController.bm_opactivity != BM_OP_INQUIRY)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTMTKBMCBM_OPACTIVITYxD, BMController.bm_opactivity);        
        /* In LE single mode search, it is possible to complete twice, just ignore the 2'nd one inquiry completed event */
        return;
    }        
    #endif

#ifdef __BT_4_0_BLE__
    BMController.dual_inquiry_type_cur = BT_BM_DUAL_INQUIRY_NONE;
    BtBmLeAdvDisabled(BMController.LeAdvIndex, &btbmDevBleEventHandler);

#endif

    if (BMController.inquiry_with_name)
    {
        /* Name Discovery procedure.  */
        if (BMController.discovered_device_no != 0)
        {
            btbmStartSpecialSlaveProcedure();
            status = btbmMultipleNameQuery();
            if (status == BT_STATUS_PENDING)
            {
                #ifdef __INDEPENDENT_INQUIRY_FLOW__
                BMController.inquiry_state = BM_INQUIRY_NAME_QUERY;
                #else
                BMController.bm_opactivity = BM_OP_NAMEREQ;
                #endif
                return;
            }
            else if (status != BT_STATUS_SUCCESS)
            {
                #ifdef __INDEPENDENT_INQUIRY_FLOW__
                BMController.inquiry_state = BM_INQUIRY_NONE;
                #else
                BMController.bm_opactivity = BM_OP_NONE;
                #endif
                btbmHandleDiscoveryComplete(BTBM_ADP_SUCCESS, BMController.discovered_device_no);
                return;
            }
        }
    }
    btbmHandlerInquiryIntervalProcedure();
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerInquiryCancel
 * DESCRIPTION
 *  To handle the inquiry cancel completed event
 * PARAMETERS
 *  Event       [IN]        The event reported by ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerInquiryCancel(const BtEvent *Event)
{
    #ifdef __INDEPENDENT_INQUIRY_FLOW__
    if (BMController.inquiry_state == BM_INQUIRY_CANCEL)
    #else
    if (BMController.bm_opactivity == BM_OP_INQCANCEL)
    #endif
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_EVENT_INQ_CANCELED);
        DDB_COD_Flush();
#ifdef __BT_4_0_BLE__
        if (Event->eType == BTEVENT_INQUIRY_CANCELED)
        {
            BMController.dual_inquiry_type_cur &= ~BT_BM_DUAL_INQUIRY_BR_EDR;
            BMController.dual_inquiry_type_cur &= ~BT_BM_DUAL_INQUIRY_LE;
        }
        
        if (BMController.dual_inquiry_type_cur == BT_BM_DUAL_INQUIRY_NONE)
#endif
        {
            #ifdef __INDEPENDENT_INQUIRY_FLOW__
            BMController.inquiry_state = BM_INQUIRY_NONE;
            #else
            BMController.bm_opactivity = BM_OP_NONE;
            #endif
            btbmSendDiscoveryCancelCfm(BTBM_ADP_SUCCESS, BMController.discovered_device_no);
            #ifndef __INDEPENDENT_INQUIRY_FLOW__
            BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
            #endif
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerMultipleNameQuery
 * DESCRIPTION
 *  The handler to deal with the btbmMultipleNameQuery operation result (ask name during inquiry procedure)
 * PARAMETERS
 *  Event       [IN]        Event of ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerMultipleNameQuery(const BtEvent *Event)
{
    BTBMInquiredDevice *inqResult;
    BtDeviceContext *bdc;
    btbm_bd_addr_t rem_addr;
    U8 dev_name[BTBM_ADP_MAX_NAME_LEN];
    U8 len;
    BtStatus status;
    BTBMRemoteNameSetting_T *entry = 0;
    U8 i = 0;
    MeCommandToken *token = NULL;
    Report(("Dlight Ting btbmHandlerMultipleNameQuery"));

    bt_trace(BT_TRACE_FUNC, BT_LOG_MULTIPLE_NAME_CBFUNC);
    OS_MemSet(dev_name, 0, BTBM_ADP_MAX_NAME_LEN);
    token = Event->p.meToken;

    token->callback = 0;

    /* 2008-0326: Do Not cancel BMController.nameTok.tokenTimer here: Before callback to this handler, it is already canceled */
    if ((inqResult = btbmUtilGetInquiredDevice(&token->p.name.bdAddr)) != NULL)
    {
        bdc = inqResult->bdc;
        bdc->under_used &= ~BDS_NAME_REQESTING;
        Report(("bdc->under_used=0x%x", bdc->under_used));
        if (Event->errCode == BEC_NO_ERROR)
        {
            U8 device_name_length = 0;
            U8 *device_name = NULL;
            inqResult->deviceNameAvailable = 1;

            if (token->p.name.io.out.len == 0)
            {
                len = 0;
            }
            else
            {
                len = min(OS_StrLen((char*)token->p.name.io.out.name), BTBM_ADP_MAX_NAME_LEN);
                OS_MemCopy((U8*) dev_name, (U8*) token->p.name.io.out.name, len);
            }
            /*  We have searched the Name  */
            CONVERT_ARRAY2BDADDR(&rem_addr, bdc->addr.addr);
            btbmUtilConnectedDeviceNameEntryUpdate(&bdc->addr, (char *)dev_name);
            bdc->device_name_length = len;
            bdc->device_name = dev_name;
            
            Report(("EIR with Name Dlight1:%08x",bdc->device_name));
            btbmSendDiscoveryResultCfm(
                              BMController.inquiryShoAddress ? TRUE : FALSE, 
                              rem_addr,
#ifdef __BT_4_0_BLE__
                              bt_util_convert_dev(bdc->devType),
#endif
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
                              bdc->rssi, 
                              bdc->support_eir, 
                              (U8)((bdc->support_service_list == BT_EIR_SERVICE_INFO_NONE) ? 0 : 1), 
                              (U8)((bdc->support_service_list == BT_EIR_SERVICE_INFO_MORE) ? 0 : 1),
                              bdc->eir_response,
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
                              bdc->classOfDevice, 
                              len, 
                              dev_name);
        }
        else
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_UNABLE_TO_GET_NAME);
            /* Name request failed. State updated so next time start from the next */
            len = 0;
        }
    }
    #ifdef __INDEPENDENT_INQUIRY_FLOW__
    if (BMController.inquiry_state != BM_INQUIRY_CANCEL)
    #else    
    if (BMController.bm_opactivity != BM_OP_INQCANCEL)
    #endif
    {
        status = btbmMultipleNameQuery();
        Report(("The status:%d",status));
        if (status == BT_STATUS_SUCCESS)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_NAME_SEARCH_COMPLETED);
            btbmHandlerInquiryIntervalProcedure();
            /*  name query procedure completed. We can report to MMI layer */
        }
        else if (status == BT_STATUS_TIMEOUT)
        {
            #ifdef __INDEPENDENT_INQUIRY_FLOW__
            BMController.inquiry_state = BM_INQUIRY_NONE;
            #else
            BMController.bm_opactivity = BM_OP_NONE;
            #endif
            btbmHandleDiscoveryComplete(BTBM_ADP_SUCCESS, BMController.discovered_device_no);
        }
    }
    else
    {
        /* Name req shall stop here */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_NAME_SEARCH_CANCELED);
        #ifdef __INDEPENDENT_INQUIRY_FLOW__
        BMController.inquiry_state = BM_INQUIRY_NONE;
        #else        
        BMController.bm_opactivity = BM_OP_NONE;
        #endif
        btbmSendDiscoveryCancelCfm(BTBM_ADP_SUCCESS, BMController.discovered_device_no);
        BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerRemoteNameQueryAtNoAuthEnabled
 * DESCRIPTION
 *  The handler to deal with the btbmRemoteNameRequestAtNoAuthEnabled operation result.
 *  Only under the condition: "(for each incoming acl link) no authentication procedure is enabled on both device and our device is connected by remote firs",
 *  it is necessary to use the function  btbmRemoteNameRequestAtNoAuthEnabled to ask the remote device's name
 * PARAMETERS
 *  Event       [IN]        Event of ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerRemoteNameQueryAtNoAuthEnabled(const BtEvent *Event)
{
    U8 dev_name[BTBM_ADP_MAX_NAME_LEN];
    BtDeviceContext *bdc = 0;
    U8 len = 0;
    U8 result;
    btbm_bd_addr_t rem_addr;
    BD_ADDR *bdaddr;
    BTBMRemoteNameSetting_T *entry = 0;
    U8 i = 0;
    MeCommandToken *token = NULL;
    token = Event->p.meToken;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_LOG_NAMETOK2_CBFUNC);

    /* Update the BtDeviceContext with the name result. */
    OS_MemSet(dev_name, 0, BTBM_ADP_MAX_NAME_LEN);
    token->callback = 0;
    bdc = DS_FindDevice(&Event->p.meToken->p.name.bdAddr);
    if (bdc != 0)
    {
        bdc->under_used &= ~BDS_NAME_REQESTING;
        Report(("bdc->under_used=0x%x", bdc->under_used));
    }
    if (Event->errCode == BEC_NO_ERROR)
    {
        if (token->p.name.io.out.len == 0)
        {
            len = 0;
        }
        else
        {
            len = min(token->p.name.io.out.len, BTBM_ADP_MAX_NAME_LEN);
            OS_MemCopy((U8*) dev_name, (U8*) token->p.name.io.out.name, len);
        }

        result = BTBM_ADP_SUCCESS;
    }
    else
    {
        /*  Search Name failed or name length ==0  */
        len = 0x00;
        result = BTBM_ADP_FAILED;
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_NOT_AUTH_NAME_FAILED);
    }
    bdaddr = &token->p.name.bdAddr;
    CONVERT_ARRAY2BDADDR(&rem_addr, bdaddr->addr);

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        entry = &BMController.DevRemNameTbl[i];
        if (entry->OwnerofNameTok2 == TRUE)
        {
            entry->OwnerofNameTok2 = FALSE;
            if ((entry->link != 0) && (OS_MemCmp(entry->link->bdAddr.addr, 6, bdaddr->addr, 6)))
            {
                OS_MemSet((U8*) entry->dev_name, 0, BTBM_ADP_MAX_NAME_LEN + 1);
                OS_MemCopy((U8*) entry->dev_name, (U8*) dev_name, len);
                entry->dev_name_len = len;
            #ifndef MTK_BB_TESTING
                entry->wait_for_name_req = FALSE;
            #else /* MTK_BB_TESTING */ 
                if (BTBBTestingCases(BTBBTESTING_NAME_REQUEST_LOOP))
                {
                    entry->wait_for_name_req = TRUE;
                }
                else
                {
                    entry->wait_for_name_req = FALSE;
                }
            #endif /* MTK_BB_TESTING */ 
                Report(("name:%s",dev_name));
            }
            break;
        }
    }

    /* check if any entry is pending for nameTok2 */
    btbmHandlerHFPInquiryBlackList();

    btbmCheckPendingforNameToken2();
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerPinCodeAskNameQuery
 * DESCRIPTION
 *  The handler to deal with PIN code Name Request operation result.
 * PARAMETERS
 *  Event       [IN]        Event of ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerPinCodeAskNameQuery(const BtEvent *Event)
{
    U8 dev_name[BTBM_ADP_MAX_NAME_LEN];
    BtDeviceContext *bdc = 0;
    U8 len;
    U8 result;
    btbm_bd_addr_t rem_addr;
    BD_ADDR *bdaddr;
    BtCodRecord record;
    BTBMRemoteNameSetting_T *entry = 0;
    U8 i = 0;
    MeCommandToken *token = NULL;
    token = Event->p.meToken;
    token->callback = 0;

    Report(("token:%08x",token->remDev));
    /* Update the BtDeviceContext with the name result. */
    OS_MemSet(dev_name, 0, BTBM_ADP_MAX_NAME_LEN);
    bdc = DS_FindDevice(&token->p.name.bdAddr);

    if (bdc != 0)
    {
        bdc->under_used &= ~BDS_NAME_REQESTING;
        Report(("bdc->under_used=0x%x", bdc->under_used));
    }
    if (Event->errCode == BEC_NO_ERROR)
    {
        if (token->p.name.io.out.len == 0)
        {
            len = 0;
        }
        else
        {
            len = min(token->p.name.io.out.len, BTBM_ADP_MAX_NAME_LEN);
            OS_MemCopy((U8*) dev_name, (U8*) token->p.name.io.out.name, len);
        }
        result = BTBM_ADP_SUCCESS;
    }
    else
    {
        /*  Search Name failed or name length ==0  */
        len = 0x00;
        result = BTBM_ADP_FAILED;
    }
    bdaddr = &token->p.name.bdAddr;
    CONVERT_ARRAY2BDADDR(&rem_addr, bdaddr->addr);
	
    btbmUtilConnectedDeviceNameEntryUpdate(bdaddr, (char *)dev_name);
    bt_trace(TRACE_GROUP_1, BTLOG_SSP_MTKBMCBONDING_STATEx02X, BMController.bonding_state);
    if (BMController.bm_opactivity == BM_OP_BONDING)
    {
#ifdef __BT_4_0_BLE__
  #ifdef __BT_LE_STANDALONE__
        SM_LinkCallback(token->remDev, Event);
  #else

        if (bdc->devType == BT_DEV_TYPE_LE)
        {
            SM_LinkCallback(token->remDev, (BtEvent *) Event);
        }
        else
  #endif
#endif
        if ((BMController.bonding_state & BM_BONDING_PINCODE_REQ) != 0)
        {
            btbmUtilCheckBondingState(BM_BONDING_NAME_REQUESTED, Event->errCode);
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTWE_SHALL_START_PIN_IND_TIMER);
            btbmHandlePinReqStartOperation(rem_addr, len, dev_name);
        }

    }
    else if (BMController.bm_opactivity == BM_OP_PAIRING)
    {
#ifdef __BT_4_0_BLE__
  #ifdef __BT_LE_STANDALONE__
        SM_LinkCallback(token->remDev, Event);
  #else
        if (bdc->devType == BT_DEV_TYPE_LE)
        {
            SM_LinkCallback(token->remDev,(BtEvent *) Event);
        }
        else
  #endif
#endif
        if ((BMController.bonding_state & BM_BONDING_PINCODE_REQ) != 0)
        {
            btbmUtilCheckPairingState(bdaddr, BM_BONDING_NAME_REQUESTED, Event->errCode);
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTWE_SHALL_START_PIN_IND_TIMER);
            btbmHandlePinReqStartOperation(rem_addr, len, dev_name);
        }
    }
    else
    {
        BMController.bm_opactivity = BM_OP_NONE;
        if (DDB_COD_FindRecord(&(bdc->addr), &record) == BT_STATUS_SUCCESS)
        {
            bdc->classOfDevice = record.cod;
        }
        btbmSendReadRemoteNameCfm(result, rem_addr, len, dev_name, record.cod);

    }
    btbmHandlerHFPInquiryBlackList();

}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerSingleNameQuery
 * DESCRIPTION
 *  The handler to deal with the btbmRunSingleNameQuery(Normal Name Request) result.
 * PARAMETERS
 *  Event       [IN]        Event of ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerSingleNameQuery(const BtEvent *Event)
{
    U8 dev_name[BTBM_ADP_MAX_NAME_LEN];
    BtDeviceContext *bdc = 0;
    U8 len;
    U8 result;
    btbm_bd_addr_t rem_addr;
    BD_ADDR *bdaddr;
    BtCodRecord record;
    BTBMRemoteNameSetting_T *entry = 0;
    U8 i = 0;
    MeCommandToken *token = NULL;
    token = Event->p.meToken;

    token->callback = 0;
    /* Update the BtDeviceContext with the name result. */
    OS_MemSet(dev_name, 0, BTBM_ADP_MAX_NAME_LEN);
    bdc = DS_FindDevice(&token->p.name.bdAddr);
    if (bdc != 0)
    {
        bdc->under_used &= ~BDS_NAME_REQESTING;
        Report(("bdc->under_used=0x%x", bdc->under_used));
    }
    if (Event->errCode == BEC_NO_ERROR)
    {
        if (token->p.name.io.out.len == 0)
        {
            len = 0;
        }
        else
        {
            len = min(OS_StrLen((char*)token->p.name.io.out.name), BTBM_ADP_MAX_NAME_LEN);
            OS_MemCopy((U8*) dev_name, (U8*) token->p.name.io.out.name, len);
        }
        result = BTBM_ADP_SUCCESS;
    }
    else
    {
        /*  Search Name failed or name length ==0  */
        len = 0x00;
        result = BTBM_ADP_FAILED;
    }
    bdaddr = &token->p.name.bdAddr;
    CONVERT_ARRAY2BDADDR(&rem_addr, bdaddr->addr);
    btbmUtilConnectedDeviceNameEntryUpdate(bdaddr, (char *)dev_name);
    if (BMController.bm_opactivity == BM_OP_BONDING)
    {
        btbmUtilCheckBondingState(BM_BONDING_NAME_REQUESTED, Event->errCode);
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTWE_SHALL_START_PIN_IND_TIMER);
        btbmHandlePinReqStartOperation(rem_addr, len, dev_name);
    }
    else if (BMController.bm_opactivity == BM_OP_PAIRING)
    {
        btbmUtilCheckPairingState(bdaddr, BM_BONDING_NAME_REQUESTED, Event->errCode);
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTWE_SHALL_START_PIN_IND_TIMER);
        btbmHandlePinReqStartOperation(rem_addr, len, dev_name);
    }
    else
    {
        if (bdc == 0)  /* autumn stack */
        {
            record.cod = 0;
        }
        else if (DDB_COD_FindRecord(&(bdc->addr), &record) == BT_STATUS_SUCCESS)
        {
            bdc->classOfDevice = record.cod;
        }

        if (BMController.bm_opactivity == BM_OP_NAMECANCEL)
        {
            btbmSendReadRemoteNameCancelCfm(BTBM_ADP_SUCCESS);
        }
        else if (BMController.bm_opactivity == BM_OP_NAMEREQ)
        {
            btbmSendReadRemoteNameCfm(result, rem_addr, len, dev_name, record.cod);
        }

        BMController.bm_opactivity = BM_OP_NONE;
    }
    btbmHandlerHFPInquiryBlackList();

}

#ifdef __BT_USE_CUST_DATA__
static void btbmGetCustInquiryData(S8 *inquiry_tx_pwr, U8 *inquiry_len, U8 *inquiry_num){
    S8 *tx_pwr = btGetCustvalue(CUSTID_INQUIRY_TX_POWER);
    U8 *len = btGetCustvalue(CUSTID_INQUIRY_LENGTH);
    U8 *num = btGetCustvalue(CUSTID_INQUIRY_RESPONSE);
    if(tx_pwr){
        if((S8)tx_pwr[BMController.loop_inquiry_cust_tx_pwr_idx] != (S8)(-128)){
            *inquiry_tx_pwr = tx_pwr[BMController.loop_inquiry_cust_tx_pwr_idx];
            BMController.loop_inquiry_cust_tx_pwr_idx++;
        }else if(BMController.loop_inquiry_cust_tx_pwr_idx > 0){
            *inquiry_tx_pwr = tx_pwr[BMController.loop_inquiry_cust_tx_pwr_idx-1];
        }
    }
    if(len){
        if(len[BMController.loop_inquiry_cust_len_idx] != 0){
            *inquiry_len = len[BMController.loop_inquiry_cust_len_idx];
            BMController.loop_inquiry_cust_len_idx++;
        }else if(BMController.loop_inquiry_cust_len_idx > 0){
            *inquiry_len = len[BMController.loop_inquiry_cust_len_idx-1];
        }
    }
    if(num){
        if(num[BMController.loop_inquiry_cust_num_idx] != 0){
            *inquiry_num = num[BMController.loop_inquiry_cust_num_idx];
            BMController.loop_inquiry_cust_num_idx++;
        }else if(BMController.loop_inquiry_cust_num_idx > 0){
            *inquiry_num = num[BMController.loop_inquiry_cust_num_idx-1];
        }
    }
    OS_Report("btbmGetCustInquiryData : idx(%u, %u, %u) : val(%d, %u, %u)", 
        BMController.loop_inquiry_cust_tx_pwr_idx,
        BMController.loop_inquiry_cust_len_idx,
        BMController.loop_inquiry_cust_num_idx,
        *inquiry_tx_pwr,
        *inquiry_len,
        *inquiry_num
        );
}
#endif

/*****************************************************************************
 * FUNCTION
 *  btbmHandlerInquiryLoop
 * DESCRIPTION
 *  This is the main body for inquiry procedure. This may enter due to MMI's inquiry request or timeout.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerInquiryLoop(void)
{
    S8 inquiry_tx_pwr = 6;
    U8 inquiry_len = 4;
    U8 inquiry_num = BMController.mmi_inquiry_number;

    /* MAUI_01885846: There are slave devices, need to do some preprocess. Start a timer waiting for procedure finished */
    if (BTBMCheckSpecialSlaveProcedure())
    {
        #ifdef __INDEPENDENT_INQUIRY_FLOW__
        BMController.inquiry_state = BM_INQUIRY_PREPROCESS;
        #endif
        EVM_ResetTimer(&BMController.inquiry_preprocess_timer);
        BMController.inquiry_preprocess_timer.func = btbmInquiryPreprocessTimeout;
        EVM_StartTimer(&BMController.inquiry_preprocess_timer, 1000);
        return;
    }

    bt_trace(TRACE_GROUP_1, START_INQUIRY_LOOP);

    #ifdef __INDEPENDENT_INQUIRY_FLOW__
    BMController.inquiry_state = BM_INQUIRY_SEARCH;
    #endif

#ifdef __BT_4_0_BLE__
    Assert(BMController.dual_inquiry_type_cur == BT_BM_DUAL_INQUIRY_NONE);
#ifdef __BT_USE_CUST_DATA__
    btbmGetCustInquiryData(&inquiry_tx_pwr, &inquiry_len, &inquiry_num);
#endif
    if (BMController.dual_discovery_type == BTBM_DISCOVERY_LE_ONLY)
    {
        BMController.dual_inquiry_type_cur |= BT_BM_DUAL_INQUIRY_LE;
        btbmDevBleEventHandler.callback = btbmHandlerInquiryBLEResult;
        btbmDevBleEventHandler.whiteList = NULL;
        btbmDevBleEventHandler.blackList = NULL;	
        btbmDevBleEventHandler.whiteListNo = NULL;
        btbmDevBleEventHandler.blackListNo = NULL;		

        BtBmLeAdvEnabled(BMController.LeAdvIndex, &btbmDevBleEventHandler);
    }
    else if (BMController.dual_discovery_type == BTBM_DISCOVERY_DUAL)
    {

        BMController.dual_inquiry_type_cur |= BT_BM_DUAL_INQUIRY_LE;
        BMController.dual_inquiry_type_cur |= BT_BM_DUAL_INQUIRY_BR_EDR;
        btbmDevBleEventHandler.callback = btbmHandlerInquiryBLEResult;
        btbmDevBleEventHandler.whiteList = NULL;
        btbmDevBleEventHandler.blackList = NULL;	
        btbmDevBleEventHandler.whiteListNo = NULL;
        btbmDevBleEventHandler.blackListNo = NULL;	
        BtBmLeAdvEnabled(BMController.LeAdvIndex, &btbmDevBleEventHandler);
        ME_InquiryTxPwr(inquiry_tx_pwr);
        if (BMController.access_mode == BTBM_GENERAL_INQUIRY)
        {
            ME_Inquiry(BT_IAC_GIAC, inquiry_len, inquiry_num);
        }
        else
        {
            ME_Inquiry(BT_IAC_LIAC, inquiry_len, inquiry_num);
        }
    }
    else if (BMController.dual_discovery_type == BTBM_DISCOVERY_BR_EDR_ONLY)
    {
        BMController.dual_inquiry_type_cur |= BT_BM_DUAL_INQUIRY_BR_EDR;
#endif
        ME_InquiryTxPwr(inquiry_tx_pwr);
        if (BMController.access_mode == BTBM_GENERAL_INQUIRY)
        {
            ME_Inquiry(BT_IAC_GIAC, inquiry_len, inquiry_num);
        }
        else
        {
            ME_Inquiry(BT_IAC_LIAC, inquiry_len, inquiry_num);
        }
#ifdef __BT_4_0_BLE__
    }
    else
    {
        Assert(!BMController.dual_discovery_type);
    }
#endif
    return;
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandlerInquiryIntervalProcedure
 * DESCRIPTION
 *  During the inquiry procedure, we may not preceed the inquiry in full speed. Sometimes, we need to
 *  start a timer for WIFI or BT connection to keep the connection alive. This is used to open a interval
 *  to keep other connections alive.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerInquiryIntervalProcedure(void)
{
    U32 guide_time;
    U8 link_no = 0;
    U32 curTime = btbmUtilCheckInquiryTime() * BTBM_TIMER_TICKS;
    U32 totalTime = 1000 * BMController.mmi_inquiry_length * 1.28;

    OS_Report("btbmHandlerInquiryIntervalProcedure:%d",BMController.bm_opactivity);
    OS_Report("mmi_inquiry_length=%d, curTime=%d, totalTime=%d", BMController.mmi_inquiry_length, curTime, totalTime);
    if (curTime >= totalTime)
    {
        #ifdef __INDEPENDENT_INQUIRY_FLOW__
        BMController.inquiry_state = BM_INQUIRY_NONE;
        #else
        BMController.bm_opactivity = BM_OP_NONE;
        #endif
        btbmHandleDiscoveryComplete(BTBM_ADP_SUCCESS, BMController.discovered_device_no);
        return;
    }
    #ifdef __INDEPENDENT_INQUIRY_FLOW__
    BMController.inquiry_state = BM_INQUIRY_DELAY;
    #else
    BMController.bm_opactivity = BM_OP_INQUIRY;
    #endif

    bt_trace(TRACE_GROUP_1, BTBMHANDLERINQUIRYINTERVALPROCEDURE);
    if (BMController.inquiry_loop_timer.func != 0)
    {
        bt_trace(TRACE_GROUP_1, INQUIRY_IDLE_TIMER_CALLBACK_FUNCTION_IS_NOT_0);
        Assert(0);
    }
    BMController.inquiry_loop_timer.func = btbmInquiryIntervalTimeout;
    link_no = btbmHandleCheckPiconetState();
    if (link_no != 0)
    {
        if (BMController.hfp_auto_disconnect_black_list != 0)
        {
            guide_time = BT_INQUIRY_GAP_INTERVAL_LINK_CONNECTED + 1500;
        }
        else
        {
            guide_time = BT_INQUIRY_GAP_INTERVAL_LINK_CONNECTED;
        }
    }
    else
    {
        guide_time = BT_INQUIRY_GAP_INTERVAL_NO_WIFI;
    }

    bt_trace(TRACE_GROUP_1, INQUIY_TIME_INTERVALxD, guide_time);
    EVM_StartTimer(&BMController.inquiry_loop_timer, guide_time);
}


/*****************************************************************************
 * FUNCTION
 *  btbmHandleDiscoveryComplete
 * DESCRIPTION
 *  To handle the Discovery Complete and report to MMI
 * PARAMETERS
 *  result              [IN]        Discovery Completed result
 *  total_number        [IN]        Discovery searched number
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandleDiscoveryComplete(U8 result, U8 total_number)
{
    BMCallbackParms parms;
    btbm_bd_addr_t bd_addr;

    //#ifdef __INDEPENDENT_INQUIRY_FLOW__
    //BMController.inquiry_state = BM_INQUIRY_NONE;
    //#endif

#ifndef MTK_BB_TESTING
    CONVERT_ARRAY2BDADDR(&bd_addr, MEC(bdaddr).addr);
    parms.event = BTBM_EVENT_DISCOVERY_COMPLETE_RESULT;
    parms.p.DiscoveryCompleteResult.result = result;
    parms.p.DiscoveryCompleteResult.total_no = total_number;
    BMController.bmCB(&parms);
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
    
#else /* MTK_BB_TESTING */ 
    if (!BTBBTestingCases(BTBBTESTING_AUTO_SLEEP_INQUIRY))
    {
        CONVERT_ARRAY2BDADDR(&bd_addr, MEC(bdaddr).addr);
        parms.event = BTBM_EVENT_DISCOVERY_COMPLETE_RESULT;
        parms.p.DiscoveryCompleteResult.result = result;
        parms.p.DiscoveryCompleteResult.total_no = total_number;
        BMController.bmCB(&parms);
        BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
    }
    else
    {
        BTBB_InquiryCompleted();
    }
#endif /* MTK_BB_TESTING */ 
}



/*****************************************************************************
 * FUNCTION
 *  btbmHandlerHFPInquiryBlackList
 * DESCRIPTION
 *  Check the device of HFP BlackList is connected.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btbmHandlerHFPInquiryBlackList(void)
{
    BTBMRemoteNameSetting_T *entry = 0;
    U8 i = 0;

    BMController.hfp_auto_disconnect_black_list = 0;
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        entry = &BMController.DevRemNameTbl[i];
        if ((entry->link != 0) &&
            (OS_MemCmp((U8*) entry->dev_name, (U16)entry->dev_name_len, SAMSUNG_410, OS_StrLen((char*)SAMSUNG_410))))
        {
            bt_trace(TRACE_GROUP_1, BLACKLIST_SEARCHED);
            BMController.hfp_auto_disconnect_black_list = 1;
        }
    }
    bt_trace(TRACE_GROUP_1, BLACKLIST_x0D, BMController.hfp_auto_disconnect_black_list);
}


/*****************************************************************************
 * FUNCTION
 *  btbmMultipleNameQuery
 * DESCRIPTION
 *  The function to do the Name Discovery (One of the Device Descovery Procedure)
 *  To do the ME_GetRemoteDeviceName operation
 * PARAMETERS
 *  void
 * RETURNS
 *  BtStatus status of result
 *  BT_STATUS_SUCCESS, No device needed to do the name discovery
 *  BT_STATUS_PENDING,    No device needed to do the name discovery
 *****************************************************************************/
BtStatus btbmMultipleNameQuery(void)
{
    BtDeviceContext *bdc = 0, *bdcNear = 0;
    BtStatus status = BT_STATUS_SUCCESS;
    U8 i = 0, k = 0, bdcNearIdx = 0;;
    U8 searchBLEOnly = 0;
    Report(("Dlight Ting btbmMultipleNameQuery"));
    bt_trace(BT_TRACE_FUNC, BT_BTBM_RUN_MULTIPLE_NAME_QUERY);
    for (i = 0; i < BMController.discovered_device_no; i++)
    {
        BTBMDumpBdAddr(BMController.inquiredDevices[i].remote_addr.addr);
        Report(("BMController.inquiredDevices[%d].used:%d",i, BMController.inquiredDevices[i].used));
        bdc = BMController.inquiredDevices[i].bdc;
#ifdef __BT_4_0_BLE__
		if (bdc->devType == BT_DEV_TYPE_LE)
        {
            /* BLE device, no do name search */
            continue;
        }
#endif		
        if(BMController.inquiredDevices[i].deviceNameAvailable == 1)
        {
            continue;
        }
        if(BMController.inquiredDevices[i].deviceNameSearchCount >= 1)
        {
            continue;
        }
        /* Ask the closest device's name */

        if (bdcNear)
        {
            if (bdcNear->rssi < bdc->rssi)
            {
                bdcNear =  bdc;
                bdcNearIdx = i;
            }
        }
        else
        {
            bdcNear = bdc;
            bdcNearIdx = i;
        }
    }

    if (bdcNear)
    {
        BMController.inquiredDevices[bdcNearIdx].deviceNameSearchCount++;
        bdcNear->under_used |= BDS_NAME_REQESTING;
        Report(("bdcNear->under_used=0x%x", bdcNear->under_used));
#ifdef __BT_4_0_BLE__		
        OS_Report("btbmNameReq: devType=%d", bdcNear->devType);
#endif
		#ifdef __INDEPENDENT_INQUIRY_FLOW__
        Report(("Dlight name address:%08x", &BMController.nameTok4));					
        status = btbmNameReq(&BMController.nameTok4, bdcNear, btbmHandlerMultipleNameQuery);
        #else
        Report(("Dlight name address:%08x", &BMController.nameTok));					
        status = btbmNameReq(&BMController.nameTok, bdcNear, btbmHandlerMultipleNameQuery);
        #endif
        Report(("name request status:%d", status));
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  btbmRemoteNameRequestAtNoAuthEnabled
 * DESCRIPTION
 *  The function to do the Single name request when no authentication procedure is enabled and our device is connected by remote device first
 * PARAMETERS
 *  bdc     [IN]        Pointer points to the address of remote device
 * RETURNS
 *  void
 *****************************************************************************/
void btbmRemoteNameRequestAtNoAuthEnabled(BtDeviceContext *bdc)
{
    BTBMRemoteNameSetting_T *entry = 0;
    U8 i = 0;

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        entry = &BMController.DevRemNameTbl[i];
        if (entry->OwnerofNameTok2 == TRUE)
        {
            bt_trace(TRACE_GROUP_1, BT_LOG_EXISTING_NAMETOK2_NOT_RESET);
            break;
        }
    }
    if (i == NUM_BT_DEVICES)
    {
        entry = btbmUtilConnectedDeviceNameEntryLookUp(&(bdc->addr));
        entry->OwnerofNameTok2 = TRUE;

        if (BMController.nameTok.callback != 0)
        {
            bt_trace(TRACE_GROUP_1, PREVIOUS_NAME_REQUEST_ON_GOING_CANCEL_IT);
            if( BT_STATUS_PENDING != ME_CancelGetRemoteDeviceName(&BMController.nameTok) ){
                BMController.nameTok.callback = NULL;
            }
        }

#ifdef __BT_4_0_BLE__
#ifdef __BT_LE_STANDALONE__
        btbmBleNameReq(bdc, btbmHandlerRemoteNameQueryAtNoAuthEnabled);

#else
        OS_Report("btbmNameReq: devType=%d", bdc->devType);
        if (bdc->devType == BT_DEV_TYPE_LE)
        {
            btbmBleNameReq(bdc, btbmHandlerRemoteNameQueryAtNoAuthEnabled);
        }
        else
        {
            btbmNameReq(&BMController.nameTok2, bdc, btbmHandlerRemoteNameQueryAtNoAuthEnabled);
        }
#endif
#else
        btbmNameReq(&BMController.nameTok2, bdc, btbmHandlerRemoteNameQueryAtNoAuthEnabled);
        
#endif        
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmCheckPendingforNameToken2
 * DESCRIPTION
 *  The function is used when the previous name req is completed, we will check if there is another name requ.
 *  shall be preceeded.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btbmCheckPendingforNameToken2(void)
{
    int i = 0;
    BtDeviceContext *bdc = 0;

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if ((BMController.DevRemNameTbl[i].wait_for_name_req == TRUE) && (BMController.DevRemNameTbl[i].link != 0))
        {
            bdc = DS_FindDevice(&(BMController.DevRemNameTbl[i].link->bdAddr));
            if (bdc != 0)
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_START_NAMETOKEN);
                BTBMDumpBdAddr(bdc->addr.addr);
                btbmRemoteNameRequestAtNoAuthEnabled(bdc);
                return;
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbmNameReq
 * DESCRIPTION
 *  The utility function to do the name request operation
 * PARAMETERS
 *  bdc             [IN]        Pointer points to the address of remote device
 *  callback        [IN]        The callback function when the name request operation is finished
 * RETURNS
 *  void
 *****************************************************************************/
BtStatus btbmNameReq(MeCommandToken *token, BtDeviceContext *bdc, BtCallBack callback)
{
    BtStatus status;
    bt_trace(BT_TRACE_FUNC, BT_BTBM_NAME_REQ, btbmUtilGetNameTokenID(token));
    BTBMDumpBdAddr(bdc->addr.addr);

    if (token->callback != 0)
    {
        /* A RemoteDeviceName query is already active. */
        bt_trace(TRACE_GROUP_1, PREVIOUS_NAME_REQUEST_ON_GOING_CANCEL_IT);
        return BT_STATUS_FAILED;
    }

    /* Start a name query for this device */
    token->callback = callback;
    token->p.name.bdAddr = bdc->addr;
    token->p.name.io.in.psi = bdc->psi;
    token->remDev = ME_FindRemoteDevice(&bdc->addr);
    status = ME_GetRemoteDeviceName(token);

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  btbmNameReq
 * DESCRIPTION
 *  The utility function to do the name request operation
 * PARAMETERS
 *  bdc             [IN]        Pointer points to the address of remote device
 *  callback        [IN]        The callback function when the name request operation is finished
 * RETURNS
 *  void
 *****************************************************************************/
BtStatus btbmBleNameReq(BtDeviceContext *bdc, BtCallBack callback)
{
    BtStatus status;
    int i=0;
    
    BTBMDumpBdAddr(bdc->addr.addr);
    Report(("btbmBleNameReq"));

    status = BTBMGATT_GetRemoteDeviceName(bdc, callback);


    return status;
}

const U8 clip_801[] = "Clip Music 801";
    
U8 btbmCheckDeviceClipMusic801(U8 *addr)
{
    int i = 0;
    BTBMRemoteNameSetting_T *entry = 0;

    if(addr == 0)
    {
        return 0;
    }

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        entry = &BMController.DevRemNameTbl[i];
        if (entry->link != 0)
        {
            if (OS_MemCmp(BMController.DevRemNameTbl[i].link->bdAddr.addr, 6, addr, 6))
            {
                if (StrnCmp((const U8*)clip_801, OS_StrLen((char*)clip_801), (U8*) entry->dev_name, entry->dev_name_len))
                {
                    return 1;
                }
                else
                    return 0;
            }
        }
    }
    return 0;
}

/*****************************************************************************
 * FUNCTION
 *  btbmInquiryIntervalTimeout
 * DESCRIPTION
 *  The inquiry interval timeout handler function.
 * PARAMETERS
 *  Timer       [IN]        Information
 * RETURNS
 *  void
 *****************************************************************************/
void btbmInquiryIntervalTimeout(EvmTimer *Timer)
{
    bt_trace(TRACE_GROUP_1, INQUIRY_INTERVAL_TIMEOUT);
    BMController.inquiry_loop_timer.func = 0;
    btbmHandlerInquiryLoop();
}


/*****************************************************************************
 * FUNCTION
 *  btbmInquiryPreprocessTimeout
 * DESCRIPTION
 *  The inquiry timeout of special slave procedure handler function.
 * PARAMETERS
 *  Timer       [IN]        Information
 * RETURNS
 *  void
 *****************************************************************************/
void btbmInquiryPreprocessTimeout(EvmTimer *Timer)
{
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTBTBMINQUIRYPROCEDURETIMEOUT, BMController.bm_opactivity);
    BMController.inquiry_preprocess_timer.func = 0;
    if ((btbmUtilCheckInquiryTime() * BTBM_TIMER_TICKS) < (1000 * BMController.mmi_inquiry_length * 1.28))
    {
        #ifdef __INDEPENDENT_INQUIRY_FLOW__
        BMController.inquiry_state = BM_INQUIRY_SEARCH;
        #else
        BMController.bm_opactivity = BM_OP_INQUIRY;
        #endif
        btbmHandlerInquiryLoop();
    }
    else
    {
        #ifdef __INDEPENDENT_INQUIRY_FLOW__
        BMController.inquiry_state = BM_INQUIRY_NONE;
        #else
        BMController.bm_opactivity = BM_OP_NONE; /* MAUI_01610479 */ 
        #endif
        btbmHandleDiscoveryComplete(BTBM_ADP_SUCCESS, BMController.discovered_device_no);
    }
}
void btbmSdpSearchAttributePendingInquiry(U8 enabled)
{
    if(enabled == 1)
        BMController.ctsSdpAttributeSearchPendingInquiry = 1;
    else
        BMController.ctsSdpAttributeSearchPendingInquiry = 0;
    Report(("set btbmSdpSearchAttributePendingInquiry to :%d",BMController.ctsSdpAttributeSearchPendingInquiry));
}



#ifdef __BT_4_0_BLE__

void btbmUpdateBleDeviceNameForConnectedDevice(BtRemoteDevice *link, U8 *name, U16 length)
{
    U8 i=0;

    /* Update connected table */
    Report(("Copy to connected table list"));
    btbmBLEUtilConnectedDeviceNameEntryAllocate(link, name, length);
    btbmGattHandleRemNameReqTimeoutCompleted(&link->bleNameReqTimer);

}


#endif
