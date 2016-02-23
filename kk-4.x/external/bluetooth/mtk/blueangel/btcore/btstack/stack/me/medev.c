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
 *     $Workfile:medev.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This is file contains the functionality for the Bluetooth Device 
 *     Selection manager. This API provides a simplified set of functions
 *     for managing non-connected devices known to the local device.
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
#include "bt_feature.h"
#include "bt_common.h"
#include "btalloc.h"
#include "ddb.h"

#if NUM_KNOWN_DEVICES > 0

#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
extern void btbmUtilParseInquiryEirResponse(BtDeviceContext *bdc, U8 *eir_reponse);
#endif 

#define AllocEntryList(_freelist, _structtype) \
            (_structtype *)(!IsListEmpty(_freelist) ? RemoveHeadList(_freelist) : 0)

#define FreeEntryList(_freelist, _node) \
            InsertTailList(_freelist, _node)

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  DS_GetDDBNumber
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 DS_GetDDBNumber(void)
{
    return (MEC(allocated_number));
}


/*****************************************************************************
 * FUNCTION
 *  DS_AddDevice
 * DESCRIPTION
 *  Add device to known devices list, if device already exists
 *  failure is returned.
 * PARAMETERS
 *  Template        [IN]        
 *  NewDevice       [IN]        
 * RETURNS
 *  BtStatus
 *****************************************************************************/
BtStatus DS_AddDevice(const BtDeviceContext *Template, BtDeviceContext **NewDevice)
{
    BtStatus status;
    BtDeviceContext *newDevice;
    BtDeviceContext *bdc = 0;
    BD_ADDR addr;
#ifdef __BT_4_0_BLE__
    BtDeviceRecord rec;
    BtLERecord leRec;
#endif 

    OS_LockStack();

    CheckParmExit(BT_STATUS_INVALID_PARM, (Template != 0));

    if (DS_FindDevice(&Template->addr))
    {
        /* Device already exists on list. */
        status = BT_STATUS_FAILED;
        goto exit;
    }

    /* Didn't find an existing match, allocate a new device context. */
    if ((newDevice = AllocEntryList(&MEC(btDeviceFreeList), BtDeviceContext)) == 0)
    {
        status = BT_STATUS_NO_RESOURCES;
        bt_trace(TRACE_GROUP_1, DS_ADDDEVICE_NO_RESOURCE);
        bt_trace(TRACE_GROUP_1, RELEASE_PREVIOUS_ONE);
        while (DS_EnumDeviceList(&bdc) == BT_STATUS_SUCCESS)
        {
            if ((bdc->link == 0) && (bdc->under_used == BDS_NONE))
            {
                addr = bdc->addr;
                status = DS_DeleteDevice(&addr);
                break;
            }
        }
        if (status == BT_STATUS_NO_RESOURCES)
        {
            goto exit;
        }
        if ((newDevice = AllocEntryList(&MEC(btDeviceFreeList), BtDeviceContext)) == 0)
        {
            goto exit;
        }        
    }
    MEC(allocated_number)++;
    /* Allocated a new entry, copy in template and add new device to the list. */
    *newDevice = *Template;
    /* We init the psRepMode to 1(R1 mode) here */
    newDevice->psi.psRepMode = 1; 
    InsertHeadList(&MEC(deviceList), &newDevice->node);
#if defined (__BT_2_1_SIMPLE_PAIRING__)
    newDevice->gapKeyType = BT_LINK_KEY_GAP_TYPE_NO_KEY;
#endif 
    newDevice->link = 0;
    newDevice->under_used = BDS_NONE;
    InitializeListEntry(&newDevice->active);
#ifdef __BT_4_0_BLE__
    /* During inquiry, will be overwritten when parsing ADV PDU */
    newDevice->adFlag = 0;
    if (SEC_FindDeviceRecord(&Template->addr, &rec) == BT_STATUS_SUCCESS)
    {
        newDevice->devType = rec.devType;
        newDevice->addrType = rec.addrType;
    }
    else if (DDB_LE_FindRecord(&Template->addr, &leRec) != -1)
    {
        Report(("LE found done"));    
        newDevice->devType = leRec.devType;
        newDevice->addrType = leRec.addrType;
    }
#endif

    status = BT_STATUS_SUCCESS;
    Report(("LE found done1"));    

    if (NewDevice)
    {
        *NewDevice = newDevice;
    }
  exit:
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  DS_DeleteDevice
 * DESCRIPTION
 *  Removes device from known devices list. This function will
 *  not remove devices from in the device security database.
 * PARAMETERS
 *  Addr        [IN]        
 * RETURNS
 *  BtStatus
 *****************************************************************************/
BtStatus DS_DeleteDevice(const BD_ADDR *Addr)
{
    BtDeviceContext *bdc;
    BtStatus status;

    OS_LockStack();

    CheckParmExit(BT_STATUS_INVALID_PARM, (Addr != 0));

    status = BT_STATUS_NOT_FOUND;

    if (0 != (bdc = DS_FindDevice(Addr)))
    {
        /* Found matching device, remove it. */
        RemoveEntryList(&bdc->node);
        FreeEntryList(&MEC(btDeviceFreeList), &bdc->node);
        MEC(allocated_number)--;
        status = BT_STATUS_SUCCESS;
    }

Check_Parm_Exit
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  DS_EnumDeviceList
 * DESCRIPTION
 *  Enumerates all devices on the known device list.
 * PARAMETERS
 *  LastDevice      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus DS_EnumDeviceList(BtDeviceContext **LastDevice)
{
    BtStatus status = BT_STATUS_SUCCESS;

    OS_LockStack();

    CheckParmExit(BT_STATUS_INVALID_PARM, LastDevice != 0);

    /* "LastDevice" points to the last entry returned or zero on first call. */
    if (*LastDevice == 0)
    {
        /* First call initialize "LastDevice" to first item on list. */
        *LastDevice = (BtDeviceContext*) GetHeadList(&MEC(deviceList));
    }
    else
    {
        /* Subsequent calls, make sure "LastDevice" is still on list. */
        if (!IsNodeOnList(&MEC(deviceList), &(*LastDevice)->node))
        {
            status = BT_STATUS_FAILED;
            goto exit;
        }

        /* Update "LastDevice" to point to next item on list. */
        *LastDevice = (BtDeviceContext*) GetNextNode(&(*LastDevice)->node);
    }

    /* "LastDevice" now points to the current entry being returned. */
    if (*LastDevice == (BtDeviceContext *)&MEC(deviceList))
    {
        /* Done walking list. Zero out the device ptr for safety. */
        *LastDevice = 0;
        status = BT_STATUS_NOT_FOUND;
    }

  exit:
    OS_UnlockStack();
   
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  DS_FindDevice
 * DESCRIPTION
 *  Find the device in the device list with a matching Bluetooth
 *  address.
 * PARAMETERS
 *  Addr        [IN]        
 * RETURNS
 *  BtDeviceContext
 *****************************************************************************/
BtDeviceContext *DS_FindDevice(const BD_ADDR *Addr)
{
    ListEntry *node;

    OS_LockStack();

    if (Addr)
    {
        node = GetHeadList(&MEC(deviceList));
        while (node != &MEC(deviceList))
        {
            if (OS_MemCmp(((BtDeviceContext*) node)->addr.addr, 6, Addr->addr, 6) == TRUE)
            {
                goto exit;
            }
            node = GetNextNode(node);
        }
    }
    node = 0;

  exit:
    OS_UnlockStack();
    return (BtDeviceContext*) node;
}


/*****************************************************************************
 * FUNCTION
 *  MeDevEventHandler
 * DESCRIPTION
 *  An internal function which monitors all ME Events before they
 *  are passed to the registered handlers.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeDevEventHandler(void)
{
    BtDeviceContext device, *currDevice;

#if BT_SECURITY == XA_ENABLED
    BtDeviceRecord rec;
    I8 i;
#endif /* BT_SECURITY == XA_ENABLED */

    OS_Report("MeDevEventHandler %d", MEC(btEvent).eType);
    switch (MEC(btEvent).eType)
    {
        case BTEVENT_LINK_CONNECT_CNF:
        case BTEVENT_LINK_CONNECT_IND:
            if (MEC(btEvent).errCode == 0)
            {
                /* Make sure the device is in the device selector database. */
                if ((currDevice = DS_FindDevice(&MEC(btEvent).p.remDev->bdAddr)) == 0)
                {
                    OS_MemSet((U8*) & device, 0, (U32) sizeof(BtDeviceContext));
                    device.addr = MEC(btEvent).p.remDev->bdAddr;
                    device.classOfDevice = MEC(btEvent).p.remDev->cod;

                    if (DS_AddDevice(&device, &currDevice) != BT_STATUS_SUCCESS)
                    {
                        break;
                    }
                }
                /* Link the connection to the device context. */
                currDevice->link = MEC(btEvent).p.remDev;
            }
            break;

        case BTEVENT_PIN_REQ:
        case BTEVENT_LINK_CONNECT_REQ:
            /* Make sure the device is in the device selector database. */
            if ((currDevice = DS_FindDevice(&MEC(btEvent).p.remDev->bdAddr)) == 0)
            {
                OS_MemSet((U8*) & device, 0, (U32) sizeof(BtDeviceContext));
                device.addr = MEC(btEvent).p.remDev->bdAddr;
                device.classOfDevice = MEC(btEvent).p.remDev->cod;
                if( BT_STATUS_SUCCESS != DS_AddDevice(&device, &currDevice) ){
                    OS_Report("MeDevEventHandler : DS_AddDevice failed");
                    break;
                }
            }
            if(MEC(btEvent).p.remDev){
                currDevice->link = MEC(btEvent).p.remDev;
            }
            if(MEC(btEvent).eType == BTEVENT_LINK_CONNECT_REQ){
                currDevice->under_used |= BDS_CONNECTING;
            }
            break;

        case BTEVENT_LINK_DISCONNECT:
            /* Unlink the connection from the device context. */
            if (0 != (currDevice = DS_FindDevice(&MEC(btEvent).p.remDev->bdAddr)))
            {
                currDevice->link = 0;
            }
            break;
        case BTEVENT_BLE_ADV_RESULT:
        case BTEVENT_INQUIRY_RESULT:
            /* If device is not in list yet, add it now. */
            Report(("BTEVENT_INQUIRY_RESULT"));
            if ((currDevice = DS_FindDevice(&MEC(btEvent).p.inqResult.bdAddr)) == 0)
            {
                Report(("BTEVENT_INQUIRY_RESULT A1"));
            
                OS_MemSet((U8*) & device, 0, (U32) sizeof(BtDeviceContext));
                device.addr = MEC(btEvent).p.inqResult.bdAddr;
#ifdef __BT_4_0_BLE__
                device.addrType = MEC(btEvent).p.inqResult.addrType;
                device.devType = MEC(btEvent).p.inqResult.devType;
                Report(("set address type:%02x device type:%02x", device.addrType, device.devType));
#endif
                if (NUM_KNOWN_DEVICES == (DS_GetDDBNumber()))
                {
                    bt_trace(TRACE_GROUP_1, MTK_NUM_KNOWN_DEVICES_FULL);
                    break;
                }
#ifdef __BT_4_0_BLE__                
                DDB_LE_DeleteRecord(&MEC(btEvent).p.inqResult.bdAddr, LE_PROFILE_NONE);
#endif
                /* To sync again, force to set again and write to LE database. */                
                /* remove the old LE database if exists. */

                if (DS_AddDevice(&device, &currDevice) != BT_STATUS_SUCCESS)
                {
                    break;
                }
#ifdef __BT_4_0_BLE__
                if(MEC(btEvent).p.inqResult.devType == BT_DEV_TYPE_LE){
                    if(MEC(btEvent).p.inqResult.supported_eir){
                        /* To make sure the devType is correct. Parse EIR before add LE record */
                        currDevice->adFlag = 0;
                        btbmUtilParseInquiryEirResponse(currDevice, MEC(btEvent).p.inqResult.eirResponse);
                        /* EIR may not contain adFlag, so we decide devType after parsing the EIR data */
                        currDevice->devType = ((currDevice->adFlag & BEF_BR_EDR_NOT_SUPPORTED) || \
                                               (currDevice->addrType == LE_ADDR_RANDOM)) ? \
                                                BT_DEV_TYPE_LE : BT_DEV_TYPE_BR_EDR_LE;
                    }
                    DDB_LE_AddRecord(&currDevice->addr, LE_PROFILE_NONE);
                }
                Report(("set device type:%02x",currDevice->devType));
#endif				
            }
            Report(("BTEVENT_INQUIRY_RESULT A2"));
#ifdef __BT_4_0_BLE__
            /* Mark device as in range, always update PSI & CoD */
            if(currDevice->devType == BT_DEV_TYPE_LE && MEC(btEvent).p.inqResult.classOfDevice == 0x00){
                OS_Report("devType is LE and COD is 0. set COD to 0xFFFFFFFF");
                currDevice->classOfDevice = 0xFFFFFFFF;
            }else
#endif            
            {
                currDevice->classOfDevice = MEC(btEvent).p.inqResult.classOfDevice;
            }
            if (MEC(btEvent).eType != BTEVENT_BLE_ADV_RESULT){
                currDevice->psi = MEC(btEvent).p.inqResult.psi;
            }
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
            bt_trace(TRACE_GROUP_1, BTLOG_RSSIx02X, MEC(btEvent).p.inqResult.rssi);
            currDevice->rssi = MEC(btEvent).p.inqResult.rssi;
            currDevice->support_eir = MEC(btEvent).p.inqResult.supported_eir;
            if (currDevice->support_eir)
            {
                currDevice->support_service_list = BT_EIR_SERVICE_INFO_NONE;
                currDevice->device_name_length = 0;
                currDevice->device_name = NULL;
            }
#endif
#ifdef __BT_4_0_BLE__
            if(MEC(btEvent).eType != BTEVENT_BLE_ADV_RESULT || currDevice->devType == BT_DEV_TYPE_LE){
                DDB_COD_AddRecord(currDevice->addr, currDevice->classOfDevice);
            }else{
                OS_Report("BLE adv report of dual mode device. Do not add COD record.");
            }
            if(MEC(btEvent).p.inqResult.devType == BT_DEV_TYPE_BR_EDR)
#endif
            {
                DDB_PSI_AddRecord(currDevice->addr, &currDevice->psi);
            }
            break;

        case BTEVENT_HCI_INITIALIZED:
        #if BT_SECURITY == XA_ENABLED
            /* Add devices from the security database to the active database */
            OS_MemSet((U8*) & device, 0, (U32) sizeof(BtDeviceContext));
            for (i = 0; SEC_EnumDeviceRecords(i, &rec) == BT_STATUS_SUCCESS; i++)
            {
                device.addr = rec.bdAddr;
                (void)DS_AddDevice(&device, 0);
            }
        #endif /* BT_SECURITY == XA_ENABLED */
            break;
        case BTEVENT_ROLE_CHANGE:
            if (MEC(btEvent).errCode == 0)
            {
                if (MEC(btEvent).p.remDev->qos.serviceType != BT_QOS_BEST_EFFORT)
                {
                    /* After role switch, the original QoS setting would be gone. Redo it. */
                    bt_trace(TRACE_GROUP_1, BTLOG_ME_SETUP_QOS_AFTER_ROLE_SWITCH);
                    ME_SetUpQoS(MEC(btEvent).p.remDev, &MEC(btEvent).p.remDev->qos);
                }
            }
            break;
    }
}


#else /* NUM_KNOWN_DEVICES > 0 */


/*****************************************************************************
 * FUNCTION
 *  dummy
 * DESCRIPTION
 *  Avoid compiler error of empty file. Calls itself to avoid to avoid
 *  unreferenced static function warning.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void dummy()
{
    dummy();
}

#endif /* NUM_KNOWN_DEVICES > 0 */

