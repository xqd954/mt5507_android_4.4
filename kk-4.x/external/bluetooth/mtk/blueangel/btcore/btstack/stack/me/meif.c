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
 *     $Workfile:meif.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This is file contains the code for the Bluetooth 
 *     Management Entity interface. These are the functions
 *     called by applications and ME clients.
 *
 * Created:
 *     October 18, 1999
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
#include "bt_common.h"
#include "btconfig.h"
#include "btalloc.h"
#include "radiomgr.h"
#include "ddb.h"
#include "btproperty.h"
#include "bt_feature.h"
#include "btutils.h"
#ifdef __BT_4_0_BLE__
#include "btconfig_le.h"
#include "le_adp.h"
#endif
#include "bt_adp_system.h"
#include "attdbmain.h"
/****************************************************************************
 *
 * external Functions
 *
 ****************************************************************************/
extern void BTBMLocalWriteScanEnabled(U8 mode);
extern void MeHandleSpecialCreateLinkProc(EvmTimer *Timer);
extern BOOL BTBMCheckAclConnectRequestNotAddPendingCon(void);
extern U8 btbmUtilGetNameTokenID(MeCommandToken *nameToken);

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  ME_RegisterGlobalHandler
 * DESCRIPTION
 *  Register a handler with the Management Enity
 * PARAMETERS
 *  handler     [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS - if successful.
 *  
 *  BT_STATUS_FAILED - (error check only) if handler not
 *  initialized or callback set to 0;
 *****************************************************************************/
BtStatus ME_RegisterGlobalHandler(BtHandler *handler)
{
    if ((handler == 0) || !IsEntryAvailable(&(handler->node)) || (handler->callback == 0))
    {
        return (BT_STATUS_FAILED);
    }

    /* Lock the stack */
    OS_LockStack();

    /* Add handler to the reg list */
    InsertTailList(&(MEC(regList)), &(handler->node));

    /* Unlock the stack */
    OS_UnlockStack();

    /* Initialize fields of the handler */
    return BT_STATUS_SUCCESS;

}


/*****************************************************************************
 * FUNCTION
 *  ME_UnregisterGlobalHandler
 * DESCRIPTION
 *  Unregister a handler previously registered with the Management
 *  Enity
 * PARAMETERS
 *  handler     [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS - if successful.
 *  
 *  BT_STATUS_FAILED - (error check only) if handler not
 *  registered or 0.
 *****************************************************************************/
BtStatus ME_UnregisterGlobalHandler(BtHandler *handler)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    /* If the handler is 0 or not registered then fail. */
    if ((handler == 0) || (!IsNodeOnList(&(MEC(regList)), &(handler->node))))
    {
        status = BT_STATUS_FAILED;
    }

    RemoveEntryList(&(handler->node));
    status = BT_STATUS_SUCCESS;

    /* Unlock the stack */
    OS_UnlockStack();
    return status;

}

#if XA_ERROR_CHECK == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  ME_SetEventMask
 * DESCRIPTION
 *  Set the event mask for the handler.
 * PARAMETERS
 *  handler     [IN]        
 *  mask        [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS - if successful.
 *  
 *  BT_STATUS_FAILED - (error check only) if handler not
 *  registered or 0.
 *****************************************************************************/
BtStatus ME_SetEventMask(BtHandler *handler, BtEventMask mask)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    /* If the handler is 0 or not registered then fail. */
    if ((handler == 0) || (!IsNodeOnList(&(MEC(regList)), &(handler->node))))
    {
        status = BT_STATUS_FAILED;
        goto done;
    }

    handler->eMask = mask;
    status = BT_STATUS_SUCCESS;

  done:
    /* Unlock the stack */
    OS_UnlockStack();
    return status;

}
#endif /* XA_ERROR_CHECK == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  ME_Inquiry
 * DESCRIPTION
 *  Start an Inquiry process.
 * PARAMETERS
 *  lap         [IN]        
 *  len         [IN]        
 *  maxResp     [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus ME_Inquiry(BtIac lap, U8 len, U8 maxResp)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto done;
    }

    /* If an Inquiry operation is already in progress then fail */
    if (MEC(opMask) & (BOM_INQUIRY | BOM_CANCEL_INQUIRY))
    {
        status = BT_STATUS_IN_PROGRESS;
        goto done;
    }

    /* Put the Inquiry request onto the operation queue */
    if (!IsEntryAvailable(&(MEC(inquiryOp).op.node)))
    {
        Assert(0);
        goto done;
    }

    MEC(opMask) |= BOM_INQUIRY;
    MEC(inquiryOp).op.opType = BOP_INQUIRY;
    MEC(inquiryOp).lap = lap;
    MEC(inquiryOp).len = len;
    MEC(inquiryOp).max = maxResp;

    MeAddOperation(&MEC(inquiryOp));

    /* Attempt to start the operation */
    MeStartOperation();
    status = BT_STATUS_PENDING;

  done:
    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_InquiryTxPwr
 * DESCRIPTION
 *  Start an Inquiry tx power process.
 * PARAMETERS
 *  lap         [IN]        
 *  len         [IN]        
 *  maxResp     [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus ME_InquiryTxPwr(I8 pwr)
{
    BtStatus status = BT_STATUS_FAILED;
    
    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto done;
    }

    /* Put the Inquiry request onto the operation queue */
    if (!IsEntryAvailable(&(MEC(inquiryTxPwrOp).op.node)))
    {
        OS_Report("ME_InquiryTxPwr : MEC(inquiryTxPwrOp) is queued but not sent yet. Just fill the pwr level and return");
        Assert(MEC(inquiryTxPwrOp).op.opType == BOP_INQUIRY_TX_PWR);
        MEC(inquiryTxPwrOp).pwr = pwr;
        goto done;
    }
    MEC(inquiryTxPwrOp).op.opType = BOP_INQUIRY_TX_PWR;
    MEC(inquiryTxPwrOp).pwr = pwr;

    MeAddOperation(&MEC(inquiryTxPwrOp));
    /* Attempt to start the operation */
    MeStartOperation();
    status = BT_STATUS_PENDING;

  done:
    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}


#ifdef __BT_4_0_BLE__
#ifdef __BT_LE_STANDALONE__
//to simulate no BR/EDR during initialization
/*****************************************************************************
 * FUNCTION
 *  ME_SetAccessibleModeNC
 * DESCRIPTION
 *  Set the accessability mode used when not connected.
 * PARAMETERS
 *  mode        [IN]        
 *  info        [IN]        
 * RETURNS
 *  status of the operation
 *****************************************************************************/
BtStatus ME_SetInitAccessibleModeNC(BtAccessibleMode mode)
{
    BtStatus status;
    U32 cod = 0;
    U8 changeIacValue = 0;
    
    bt_trace(TRACE_GROUP_1, SET_ME_SETACCESSIBLEMODENCBT_DEBUGx02X, mode);

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlock;
    }

    /* Set the COD Limited Discoverable bit appropriately */
    switch (mode)
    {
        case BAM_LIMITED_ACCESSIBLE:
            cod |= COD_LIMITED_DISC_MASK;
            break;
        case BAM_GENERAL_ACCESSIBLE:
            cod &= ~COD_LIMITED_DISC_MASK;
            break;
    }

    /* If an access mode change operation is already in progress then fail  */
    MEC(accModeExpValue) = mode;

    if(!(IsEntryAvailable(&(MEC(writeScanModeOp).op.node))))
    {
        bt_trace(TRACE_GROUP_1, WRITESCANxD, 0xf1);
        MEC(accModeCur) |= (mode&0x3);
        OS_Report("[ME] write scan mode op already queued. Change mode to %u", MEC(accModeCur));
        MEC(writeScanModeOp).mode = mode;
        return BT_STATUS_SUCCESS;
    }
    if(!(IsEntryAvailable(&(MEC(writeIacOp).op.node))))
    {
        bt_trace(TRACE_GROUP_1, WRITESCANxD, 0xf2);
        return BT_STATUS_SUCCESS;
    }    
    bt_trace(TRACE_GROUP_1, WRITESCANxD, mode);
    /* Start up an operation */
    if(MEC(isLiac) == TRUE)
    {
        /* In limited inquiry access code */
        if((MEC(accModeExpValue) & 0x10) == 0x00)
        {
            changeIacValue = 1;
            MEC(isLiac) = FALSE;
            /* Want to change to General */
        }
    }
    else
    {
        /* In limited inquiry access code */
        if((MEC(accModeExpValue) & 0x10) == 0x10)
        {
            changeIacValue = 1;
            /* Want to change to General */
            MEC(isLiac) = TRUE;
        }
    }
    if(changeIacValue == 1)
    {
        Report(("INQ Access Code has been changed."));
        MEC(writeIacOp).op.opType = BOP_WRITE_IAC_VALUE;
        MEC(writeIacOp).mode = mode;
        MeAddOperation(&MEC(writeIacOp));
    }
    MEC(accModeCur) = mode;
    MEC(writeScanModeOp).op.opType = BOP_WRITE_SCAN_MODE;
    MEC(writeScanModeOp).mode = mode;
    MeAddOperation(&MEC(writeScanModeOp));

    
    /* Attempt to start the operation */
    MeStartOperation();
    status = BT_STATUS_PENDING;

  unlock:
    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}

BtStatus ME_LE_SetDirectedConnectable(LeAddrType initAddrType, BD_ADDR *initAddr)
{
    BtStatus status = BT_STATUS_SUCCESS;
    OS_Report("ME_LE_SetDirectedConnectable(): stackState=%d, advCur=0x%x", MEC(stackState), btCtxLe.advCur);
    BTBMDumpBdAddr(initAddr->addr);
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        return BT_STATUS_HCI_INIT_ERR;
    }

    if (btCtxLe.advCur != HCI_ADV_DIRECT_IND)
    {
        if (btCtxLe.advCur != 0xFF)  /* change to new adv type, so disable current adv */
        {
            LeHciSetAdvertiseEnable(FALSE);
        }

        LeHciSetAdvertiseParm(0x400/*BT_LE_DEFAULT_MIN_ADV_INTERVAL*/, 
                              0x800/*BT_LE_DEFAULT_MAX_ADV_INTERVAL*/,
                              HCI_ADV_DIRECT_IND,
                              LE_ADDR_PUBLIC,
                              initAddrType,
                              *initAddr,
                              LE_ADV_CHANNEL_ALL,
                              ADV_ALLOW_ALL);
    }
    status = LeHciSetAdvertiseEnable(TRUE);
    btCtxLe.advCur = HCI_ADV_DIRECT_IND;
    return BT_STATUS_PENDING;
}
#endif

BtDevType ME_GetDeviceType(BD_ADDR *BdAddr)
{
    BtDeviceContext *bdc = DS_FindDevice(BdAddr);
    BtDeviceRecord rec;
    BtRemoteDevice *link = 0;
    
#ifndef __BT_LE_STANDALONE__    
    BtDevType type = BT_DEV_TYPE_UNKNOWN;
#else
    BtDevType type = BT_DEV_TYPE_LE;
#endif

    if(BdAddr !=0)
        link = ME_FindRemoteDeviceP(BdAddr->addr);
    
    if (bdc)
    {
        type = bdc->devType;
        OS_Report("bdc path: %d", type);        
    }
    else if(SEC_FindDeviceRecord(BdAddr, &rec) == BT_STATUS_SUCCESS)
    {
        type = rec.devType;
        OS_Report("sec path: %d", type);        
    }
    else if(DDB_DEV_TYPE_FindRecord(*BdAddr, &type) == BT_STATUS_SUCCESS)
    {
        OS_Report("find device type from device info db : %d", type);
    }
    else if(link !=0)
    {
        Report(("handle :%04x",link->hciHandle));
        if((link->hciHandle >= 0x32) && (link->hciHandle <=0x40))
        {
            type = BT_DEV_TYPE_BR_EDR;
        }
        OS_Report("link path: %d", type);        
        
    }

    OS_Report("ME_GetDeviceType(): %d", type);
    BTBMDumpBdAddr(BdAddr->addr);
    return type;
}

BtConnType ME_GetConnType(BtRemoteDevice *remDev){
    BtConnType type = BT_CONN_TYPE_UNKNOWN;
    if(remDev){
        /* According to the range (MTK chip specific) of connection handle to decide the type of connection (LE or BREDR) */
        if((remDev->hciHandle >= 0x0200) && (remDev->hciHandle <=0x0300))
        {
            type = BT_CONN_TYPE_LE;
        }else if((remDev->hciHandle >= 0x0032) && (remDev->hciHandle <=0x00ff)){
            type = BT_CONN_TYPE_BR_EDR;
        }
    }
    OS_Report("ME_GetConnType : hcihandle = %u, type = %d", remDev->hciHandle, type);
    return type;
}

BtStatus ME_LE_SetAccessible(BtAccessibleMode mode, BtLEAdvParm *parm)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BD_ADDR addr = {{0, 0, 0, 0, 0, 0}};
    HciLeAdvertiseType adv = 0;

    OS_Report("ME_LE_SetAccessible(): stackState=%d, mode=0x%x, advCur-0x%x", MEC(stackState), mode, btCtxLe.advCur);
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto done;
    }

    if(btGetCustvalue(CUSTID_LE_SUPPORT) == 0){
        OS_Report("LE is swiched off. just return");
        goto done;
    }

#ifdef __BT_LE_STANDALONE__
    MEC(accModeCur) = mode;
    MEC(accModeExpValue) = mode;
#else
    if (BTCoreIsInitializing())
    {
        LeHciSetAdvertiseEnable(FALSE);
        btCtxLe.advCur = 0xFF;
        status = BT_STATUS_PENDING;
        goto done;
        
    }
    Report(("Continue going"));
#endif

    if (mode == BAM_NOT_ACCESSIBLE)
    {
        LeHciSetAdvertiseEnable(FALSE);
        btCtxLe.advCur = 0xFF;
        status = BT_STATUS_PENDING;
        goto done;
    }

    leUtilUpdateAdvertiseData();
    switch (mode)
    {
    case BAM_GENERAL_ACCESSIBLE:
    case BAM_LIMITED_ACCESSIBLE:
#ifdef __BT_LE_STANDALONE__
        adv = HCI_ADV_IND;
#else
        adv = HCI_ADV_NONCONN_IND;
        //adv = HCI_ADV_IND;

#endif
        break;
    case BAM_CONNECTABLE_ONLY:
        btCtxLe.advCur = 0xFF;
        return LeHciSetAdvertiseEnable(FALSE);
    case BAM_DISCOVERABLE_ONLY:
    case BAM_LIMITED_DISCOVERABLE:
        adv = HCI_ADV_NONCONN_IND;
        break;
    default:
        Assert(0);
    }

    if (btCtxLe.advCur != adv)
    {
        if (btCtxLe.advCur != 0xFF)  /* change to new adv type, so disable current adv */
        {
            LeHciSetAdvertiseEnable(FALSE);
        }

        if (parm)
        {
            LeHciSetAdvertiseParm(parm->intervalMin, 
                                  parm->intervalMax,
                                  adv,
                                  LE_ADDR_PUBLIC,
                                  0,
                                  addr,
                                  LE_ADV_CHANNEL_ALL,
                                  ADV_ALLOW_ALL);
        }
        else
        {
            LeHciSetAdvertiseParm(0x100/*BT_LE_DEFAULT_MIN_ADV_INTERVAL*/, 
                                  0x200/*BT_LE_DEFAULT_MAX_ADV_INTERVAL*/,
                                  adv,
                                  LE_ADDR_PUBLIC,
                                  0,
                                  addr,
                                  LE_ADV_CHANNEL_ALL,
                                  ADV_ALLOW_ALL);
        }
    }
    status = LeHciSetAdvertiseEnable(TRUE);
    btCtxLe.advCur = adv;
    status = BT_STATUS_PENDING;
done:
    return status;
}

BtStatus ME_LE_Inquiry(void)
{
    BtStatus status;

    /* Make sure the stack does not have an HCI initialization error */
    bt_trace(TRACE_FUNC, BT_MEIF_ME_LE_INQUIRY, MEC(stackState));
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto done;
    }

#ifdef __BT_4_0_ACTIVE_SCAN__
    LeHciSetScanParm(HCI_SCAN_ACTIVE, 
                     BT_LE_DEFAULT_DISC_DUAL_SCAN_INTERVAL, 
                     BT_LE_DEFAULT_DISC_SCAN_WINDOW,
  #ifdef __BT_4_0_PRIVACY__
                     LE_ADDR_RANDOM,
  #else
                     LE_ADDR_PUBLIC,
  #endif
                     SCAN_ALLOW_ALL);
#else
    LeHciSetScanParm(HCI_SCAN_PASSIVE, 
                     BT_LE_DEFAULT_DISC_DUAL_SCAN_INTERVAL, 
                     BT_LE_DEFAULT_DISC_SCAN_WINDOW,
                     LE_ADDR_PUBLIC,
                     SCAN_ALLOW_ALL);
#endif
    status = LeHciSetScanEnable(TRUE, TRUE);

done:
    return status;
}

BtStatus ME_LE_CancelInquiry(void)
{
    BtStatus status;

    /* Make sure the stack does not have an HCI initialization error */
    bt_trace(TRACE_FUNC, BT_MEIF_ME_LE_CANCELINQUIRY, MEC(stackState));
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto done;
    }

    status = LeHciSetScanEnable(FALSE, FALSE);

done:
    return status;
}

BtStatus ME_LE_CreateLink(BtHandler *handler, BD_ADDR *bdAddr, BtLELinkParm *parm, BtRemoteDevice **remDevice)
{
    BtStatus status;
    BtDeviceContext *bdc = 0;
    BtRemoteDevice *remDev;
    U32 connLatency = BT_LE_DEFAULT_CONN_LATENCY;
    U32 supervisionTO = BT_LE_DEFAULT_SUPERVISION_TO; 

    /* Make sure the stack does not have an HCI initialization error */
    bt_trace(TRACE_FUNC, BT_MEIF_ME_LE_CREATELINK, MEC(stackState), parm);
    BTBMDumpBdAddr(bdAddr->addr);
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto done;
    }
    if ((handler == 0) || (handler->callback == 0) || (remDevice == 0) ||
        (bdAddr == 0) || !IsEntryAvailable(&(handler->node)))
    {

        if (handler == 0)
        {
            bt_trace(TRACE_GROUP_1, BT_ME_CREATELINK_CASE, 1);
        }

        if (handler->callback == 0)
        {
            bt_trace(TRACE_GROUP_1, BT_ME_CREATELINK_CASE, 2);
        }

        if (remDevice == 0)
        {
            bt_trace(TRACE_GROUP_1, BT_ME_CREATELINK_CASE, 3);
        }

        if (bdAddr == 0)
        {
            bt_trace(TRACE_GROUP_1, BT_ME_CREATELINK_CASE, 4);
        }

        if (!IsEntryAvailable(&(handler->node)))
        {
            bt_trace(TRACE_GROUP_1, BT_ME_CREATELINK_CASE, 5);
        }

        status = BT_STATUS_FAILED;
        goto done;
    }

    handler->pending = BT_CONN_PENDING_IDLE;
    
    if ((bdc = DS_FindDevice(bdAddr)) == NULL)
    {
        BtDeviceContext device;
        OS_Report("bdc null");
        OS_MemSet((U8*) &device, 0, (U32) sizeof(BtDeviceContext));
        device.addr = *bdAddr;
        device.classOfDevice = 0;
        DS_AddDevice(&device, &bdc);
    }
    /* First determine if a connection already exists to this device */
    if ((remDev = ME_FindRemoteDevice(bdAddr)) != 0)
    {
        bdc->link = remDev;
        *remDevice = remDev;
        if (handler)
        {
            InsertTailList(&(remDev->handlers), &(handler->node));
        }
        bt_trace(TRACE_GROUP_1, BT_MEIF_ME_LE_CREATELINK1, remDev->state);
        if (remDev->state == BDS_CONNECTED)
        {
            return BT_STATUS_SUCCESS;
        }
        else if (remDev->state > BDS_CONNECTED)
        {
            /* ME Link Pending */
            bt_prompt_trace(MOD_BT, "[ME][LINK] create le link when disconnection is ongoing");
            handler->pending = BT_CONN_PENDING_LE;   
            return BT_STATUS_PENDING;
            //RemoveEntryList(&(handler->node));
            //return BT_STATUS_IN_PROGRESS;
        }       
        else
        {
            return BT_STATUS_PENDING;
        }
    }

    if ((remDev = MeGetFreeRemoteDevice()) == 0)
    {
        return BT_STATUS_NO_RESOURCES;
    }
    bdc->link = remDev;
    bdc->devType = BT_DEV_TYPE_LE;
    Report(("bdc:%08x", bdc));
    Report(("bdc->link:%08x", bdc->link));    
    *remDevice = remDev;
    remDev->state = BDS_OUT_CON;
    remDev->bdAddr = *bdAddr;
    if (handler)
    {
        InsertTailList(&(remDev->handlers), &(handler->node));
        remDev->discFlag = TRUE;
    }
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);
    Report(("Try to create link and address type:%d",bdc->addrType));

#if 0
    connLatency = (U32)btGetCustvalue(CUSTID_LE_CONN_LATENCY);
    supervisionTO = (U32)btGetCustvalue(CUSTID_LE_SUPERVISION_TO); 

    if (parm)
    {
        status = LeHciCreateConnection(remDev,
                                       parm->scanInterval,
                                       parm->scanWindow,
                                       INITIAOR_NOT_USE_WL,
                                       bdc->addrType,
                                       bdAddr,
                                       ((parm->privacy) ? LE_ADDR_RANDOM : LE_ADDR_PUBLIC),
                                       parm->connIntervalMin,
                                       parm->connIntervalMax,
                                       connLatency, //BT_LE_DEFAULT_CONN_LATENCY,
                                       supervisionTO, //BT_LE_DEFAULT_SUPERVISION_TO,
                                       BT_LE_DEFAULT_MIN_CE_LENGTH,
                                       BT_LE_DEFAULT_MAX_CE_LENGTH);
    }
    else
    {
        U32 connIntervalMax = (U32)btGetCustvalue(CUSTID_LE_CONN_INTERVAL_MAX);
        U32 connIntervalMin = (U32)btGetCustvalue(CUSTID_LE_CONN_INTERVAL_MIN);        
        status = LeHciCreateConnection(remDev,
                                       0x80 /*BT_LE_DEFAULT_CONN_EST_SCAN_INTERVAL*/,
                                       BT_LE_DEFAULT_CONN_EST_SCAN_WINDOW,
                                       INITIAOR_NOT_USE_WL,
                                       bdc->addrType,
                                       bdAddr,
                                       LE_ADDR_PUBLIC,
                                       connIntervalMin, //0x100, //BT_LE_DEFAULT_MIN_CONN_INTERVAL,
                                       connIntervalMax, //0x200, //BT_LE_DEFAULT_MAX_CONN_INTERVAL,
                                       connLatency, //BT_LE_DEFAULT_CONN_LATENCY,
                                       supervisionTO, //BT_LE_DEFAULT_SUPERVISION_TO,
                                       BT_LE_DEFAULT_MIN_CE_LENGTH,
                                       BT_LE_DEFAULT_MAX_CE_LENGTH);
    }
#endif    
    status = LeAddConnDevice(bdc->addrType, bdAddr);
    if (status == BT_STATUS_SUCCESS)
    {
        remDev->interval_min  = (U32)btGetCustvalue(CUSTID_LE_CONN_INTERVAL_MIN);
        remDev->interval_max  = (U32)btGetCustvalue(CUSTID_LE_CONN_INTERVAL_MAX);    
        remDev->slave_latency = (U32)btGetCustvalue(CUSTID_LE_CONN_LATENCY);    
        remDev->timeout       = (U32)btGetCustvalue(CUSTID_LE_SUPERVISION_TO);     
        EVM_ResetTimer(&remDev->connectTimer);
        remDev->connectTimer.func = LeHandleConnectTimeout;
        remDev->connectTimer.context = remDev;
        EVM_StartTimer(&remDev->connectTimer, BT_LE_DEFAULT_CONN_TIMEOUT);
        status = BT_STATUS_PENDING;
    }
    else
    {
        OS_Report("[ME][LE] LeAddConnDevice failed 0x%02x", status);     
        status = BT_STATUS_FAILED;
    }

done:
    return status;
}

BtStatus ME_LE_DiscLink(BtHandler *handler, BD_ADDR *bdAddr, BtErrorCode reason)
{
    BtRemoteDevice *remDev;
    bt_trace(TRACE_FUNC, BT_MEIF_ME_LE_DISCLINK, MEC(stackState), reason);

    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        return BT_STATUS_HCI_INIT_ERR;
    }
    if ((remDev = ME_FindRemoteDevice(bdAddr)) != 0)
    {
        return ME_ForceDisconnectLinkWithReason(handler, remDev, reason, FALSE);
    }
    return BT_STATUS_SUCCESS;
}



BtStatus ME_LE_ForceDiscLink(BtHandler *handler, BD_ADDR *bdAddr, BtErrorCode reason)
{
    BtRemoteDevice *remDev;
    bt_trace(TRACE_FUNC, BT_MEIF_ME_LE_DISCLINK, MEC(stackState), reason);

    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        return BT_STATUS_HCI_INIT_ERR;
    }
    if ((remDev = ME_FindRemoteDevice(bdAddr)) != 0)
    {
        return ME_ForceDisconnectLinkWithReason(handler, remDev, reason, TRUE);
    }
    return BT_STATUS_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  ME_LE_EncryptLink
 * DESCRIPTION
 *  Encrypt link with remote LE-only device (Ref. SEC_SetLinkEncryption)
 * PARAMETERS
 *  remDev   [IN]
 *  parm     [IN]  Specified encrypting parameters. If NULL, use bonded parameters
 * RETURNS
 *  Status of the operation
 *****************************************************************************/
BtStatus ME_LE_EncryptLink(BtRemoteDevice *remDev, BtLEEncParm *parm)
{
    BtDeviceRecord record;

    if (remDev == NULL)
    {
        return BT_STATUS_INVALID_PARM;
    }
    
    bt_trace(TRACE_FUNC, BT_MEIF_ME_LE_ENCRYPTLINK, MEC(stackState), remDev->state, remDev->authState, remDev->encryptState);
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        return BT_STATUS_HCI_INIT_ERR;
    }

    if ((remDev->state != BDS_CONNECTED) ||
        (remDev->encryptState == BES_START_ENCRYPT) || 
        (remDev->encryptState == BES_END_ENCRYPT))
    {
        return BT_STATUS_IN_PROGRESS;
    }

    if (parm == NULL)
    {
        if (SEC_FindDeviceRecord(&(remDev->bdAddr), &record) != BT_STATUS_SUCCESS ||
            (record.smKeyType & SM_KEY_LTK) == 0)
        {
            return BT_STATUS_NOT_FOUND;
        }
    }
    
    if (remDev->encryptState == BES_ENCRYPTED)
    {
        bt_trace(TRACE_GROUP_1, BT_MEIF_ME_LE_ENCRYPTLINK_RESTART);
    }

    remDev->encryptState = BES_START_ENCRYPT;

    if (parm == NULL)
    {
        LeHciStartEncryption(remDev->hciHandle, record.rand, record.ediv, record.linkKey);
    }
    else
    {
        LeHciStartEncryption(remDev->hciHandle, parm->random, parm->ediv, parm->ltk);
    }
    return BT_STATUS_PENDING;
}

/*****************************************************************************
 * FUNCTION
 *  ME_LE_SignData
 * DESCRIPTION
 *  Sign data by appending 96-bit signature to data buffer.
       ---------------------------------------------- 
       |               buf[bufLen]                   |
       ----------------------------------------------
       |        dataLen                      | 96bit |
       ----------------------------------------------
 * PARAMETERS
 *  callback  [IN]
 *  bdAddr    [IN]
 *  buf       [IN/OUT] Must be valid til receiving response
 *  bufLen    [IN]
 *  dataLen   [IN]
 * RETURNS
 *  Status of the operation
 *****************************************************************************/
BtStatus ME_LE_SignData(BtCallBack callback, BD_ADDR *bdAddr, U8 *buf, U32 bufLen, U32 dataLen)
{
    U32 counter;
    U8 csrk[SM_CSRK_VALUE_LENGTH];
    BtDeviceRecord rec;
    
    bt_trace(TRACE_FUNC, BT_MEIF_ME_LE_SIGNDATA, MEC(stackState));
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        return BT_STATUS_HCI_INIT_ERR;
    }
    if (buf == NULL || bufLen != dataLen + SM_SIGNATURE_LENGTH)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if (SEC_FindDeviceRecord(bdAddr, &rec) == BT_STATUS_SUCCESS)
    {
        if (rec.distSmKeyType & SM_KEY_CSRK)
        {
            DDB_ReadLeKey(csrk, &counter, NULL);
            bt_trace(TRACE_GROUP_1, BT_MEIF_ME_LE_SIGNDATA1, counter);

            /* M = data || SignCounter */
            StoreLE32(buf + dataLen, counter);
            counter++;
            DDB_WriteLeKey(NULL, &counter, NULL);
            SM_CMAC(callback, bdAddr, csrk, buf, dataLen + 4, SM_MAC_LENGTH);
            return BT_STATUS_PENDING;
        }
        else
        {
            return BT_STATUS_KEY_ERR;
        }
    }
    else
    {
        return BT_STATUS_DEVICE_NOT_FOUND;
    }
}

/*****************************************************************************
 * FUNCTION
 *  ME_LE_VerifySignedData
 * DESCRIPTION
 *  Verify signed data by checking the 96-bit signature at buffer end
       ---------------------------------------------- 
       |               buf[bufLen]                   |
       ----------------------------------------------
       |        dataLen                      | 96bit |
       ----------------------------------------------
 * PARAMETERS
 *  buf       [IN/OUT] Must be valid til receiving response
 *  bufLen    [IN]
 *  dataLen   [IN]
 * RETURNS
 *  Status of the operation
 *****************************************************************************/
BtStatus ME_LE_VerifySignedData(BtCallBack callback, BD_ADDR *bdAddr, U8 *buf, U32 dataLen)
{
    BtDeviceRecord rec;
    BtRemoteDevice *remDev = ME_FindRemoteDevice(bdAddr);
    
    bt_trace(TRACE_FUNC, BT_MEIF_ME_LE_VERIFYSIGNDATA, MEC(stackState));
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        return BT_STATUS_HCI_INIT_ERR;
    }
    if (buf == NULL || dataLen < SM_SIGNATURE_LENGTH || remDev == NULL)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if (SEC_FindDeviceRecord(bdAddr, &rec) == BT_STATUS_SUCCESS)
    {
        if (rec.smKeyType & SM_KEY_CSRK)
        {
            U32 counter = LEtoHost32(&buf[dataLen - SM_SIGNATURE_LENGTH]);
            if (counter < remDev->signCounter + 1 && remDev->signCounter != 0)
            {
                return BT_STATUS_FAILED;
            }
            bt_trace(TRACE_GROUP_1, BT_MEIF_ME_LE_SIGNDATA1, counter);
            remDev->signCounter = counter;
            SM_CMAC_Verify(callback, bdAddr, rec.csrk, buf, dataLen - SM_MAC_LENGTH, SM_MAC_LENGTH, &buf[dataLen - SM_MAC_LENGTH]);
            return BT_STATUS_PENDING;
        }
        else
        {
            return BT_STATUS_KEY_ERR;
        }
    }
    else
    {
#if 0
#ifdef __BT_LE_STANDALONE__
        BtRemoteDevice *remDev = ME_FindRemoteDevice(bdAddr);
        SM_SendSecurityReq(remDev, SM_DEFAULT_BOND_FLAG, FALSE);
#else
        BtDeviceContext *bdc = DS_FindDevice(bdAddr);
        SmPairAttribute attr = {SM_DEFAULT_MITM_PROTECT, SM_DEFAULT_OOB_FLAG, SM_DEFAULT_BOND_FLAG};
        SM_Bond(bdc, &attr);
#endif
#endif
        return BT_STATUS_DEVICE_NOT_FOUND;
    }
}
#endif


/*****************************************************************************
 * FUNCTION
 *  ME_CancelInquiry
 * DESCRIPTION
 *  Cancel an Inquiry process.
 * PARAMETERS
 *  void
 * RETURNS
 *  status of the operation
 *****************************************************************************/
BtStatus ME_CancelInquiry(void)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    /* See if we are already performing a cancel operation */
    if (MEC(opMask) & BOM_CANCEL_INQUIRY)
    {
        status = BT_STATUS_IN_PROGRESS;
        goto done;
    }

    if (MEC(opMask) & BOM_INQUIRY)
    {
        /*
         * An inquiry operation is progress so we can cancel it. First see
         * if it is on the list
         */
        if (IsEntryAvailable(&(MEC(inquiryOp).op.node)))
        {
            /* Entry is not on the list so start a cancel operation */
            MEC(opMask) |= BOM_CANCEL_INQUIRY;
            MEC(inquiryOp).op.opType = BOP_CANCEL_INQUIRY;
            MeAddOperation(&MEC(inquiryOp));

            /* Attempt to start the operation */
            MeStartOperation();
            status = BT_STATUS_PENDING;
        }
        else
        {
            /* Entry is on the list so remove it and report results */
            RemoveEntryList(&(MEC(inquiryOp).op.node));
            MEC(btEvent).errCode = 0;
            MeReportInqCancelComplete();
            status = BT_STATUS_SUCCESS;
        }
    }
    else
    {
        /* No inquiry operation in progress so operation failed */
        status = BT_STATUS_FAILED;
    }

  done:
    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_SetAccessibleModeNC
 * DESCRIPTION
 *  Set the accessability mode used when not connected.
 * PARAMETERS
 *  mode        [IN]        
 *  info        [IN]        
 * RETURNS
 *  status of the operation
 *****************************************************************************/
BtStatus ME_SetAccessibleModeNC(BtAccessibleMode mode)
{
#ifdef __BT_LE_STANDALONE__
    return BT_STATUS_SUCCESS;
#else
    BtStatus status;
    U32 cod = 0;
    U8 changeIacValue = 0;
    
    bt_trace(TRACE_GROUP_1, SET_ME_SETACCESSIBLEMODENCBT_DEBUGx02X, mode);

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlock;
    }

    /* Set the COD Limited Discoverable bit appropriately */
    switch (mode)
    {
        case BAM_LIMITED_ACCESSIBLE:
            cod |= COD_LIMITED_DISC_MASK;
            break;
        case BAM_GENERAL_ACCESSIBLE:
            cod &= ~COD_LIMITED_DISC_MASK;
            break;
    }
    /* If the values are the same as the current values then return success */
    if (mode == MEC(accModeExpValue))
    {
        status = BT_STATUS_SUCCESS;
        goto unlock;
    }
    ME_SetClassOfDevice(MEC(classOfDevice) | cod);
    /*
     * If there are any connections up then save the information and 
     * return BT_STATUS_SUCCESS.
     */
    /* If an access mode change operation is already in progress then fail  */
    MEC(accModeExpValue) = mode;

    if(!(IsEntryAvailable(&(MEC(writeScanModeOp).op.node))))
    {
        bt_trace(TRACE_GROUP_1, WRITESCANxD, 0xf1);
        MEC(accModeCur) |= (mode&0x3);
        OS_Report("[ME] write scan mode op already queued. Change mode to %u", MEC(accModeCur));
        MEC(writeScanModeOp).mode = mode;        
        return BT_STATUS_SUCCESS;
    }
    if(!(IsEntryAvailable(&(MEC(writeIacOp).op.node))))
    {
        bt_trace(TRACE_GROUP_1, WRITESCANxD, 0xf2);
        return BT_STATUS_SUCCESS;
    }    
    bt_trace(TRACE_GROUP_1, WRITESCANxD, mode);
    /* Start up an operation */
    if(MEC(isLiac) == TRUE)
    {
        /* In limited inquiry access code */
        if((MEC(accModeExpValue) & 0x10) == 0x00)
        {
            changeIacValue = 1;
            MEC(isLiac) = FALSE;
            /* Want to change to General */
        }
    }
    else
    {
        /* In limited inquiry access code */
        if((MEC(accModeExpValue) & 0x10) == 0x10)
        {
            changeIacValue = 1;
            /* Want to change to General */
            MEC(isLiac) = TRUE;
        }
    }
    if(changeIacValue == 1)
    {
        Report(("INQ Access Code has been changed."));
        MEC(writeIacOp).op.opType = BOP_WRITE_IAC_VALUE;
        MEC(writeIacOp).mode = mode;
        MeAddOperation(&MEC(writeIacOp));
    }
    MEC(accModeCur) = mode;
    MEC(writeScanModeOp).op.opType = BOP_WRITE_SCAN_MODE;
    MEC(writeScanModeOp).mode = mode;
    MeAddOperation(&MEC(writeScanModeOp));

    
    /* Attempt to start the operation */
    MeStartOperation();
    status = BT_STATUS_PENDING;

  unlock:
    /* Unlock the stack */
    OS_UnlockStack();

    return status;
#endif
}


/*****************************************************************************
 * FUNCTION
 *  ME_GetAccessibleModeNC
 * DESCRIPTION
 *  Get the accessibility mode used when not connected.
 * PARAMETERS
 *  mode        [IN]        
 *  info        [IN]        
 * RETURNS
 *  status of the operation
 *****************************************************************************/
BtStatus ME_GetAccessibleModeNC(BtAccessibleMode *mode)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    if (mode != 0)
    {
        *mode = MEC(accModeExpValue);
    }

    status = BT_STATUS_SUCCESS;

    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_GetCurAccessibleMode
 * DESCRIPTION
 *  Get the current accessibility mode.
 * PARAMETERS
 *  mode        [IN]        
 *  info        [IN]        
 * RETURNS
 *  status of the operation
 *****************************************************************************/
BtStatus ME_GetCurAccessibleMode(BtAccessibleMode *mode)
{
    /* Lock the stack */
    OS_LockStack();

    /* We can give back the results */
    if (mode != 0)
    {
        *mode = MEC(accModeCur);
    }

    /* Unlock the stack */
    OS_UnlockStack();
    return BT_STATUS_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  ME_GetRemoteDeviceName
 * DESCRIPTION
 *  Get the remote device name.
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus ME_GetRemoteDeviceName(MeCommandToken *token)
{
    BtStatus status;
    U8 tokenID = 0;

    /* Lock the stack */
    OS_LockStack();
    status = BT_STATUS_FAILED;
    if (token == 0)
    {
        goto done;
    }

    tokenID = btbmUtilGetNameTokenID(token);
    EVM_ResetTimer(&token->tokenTimer);
    if ((token->callback == 0) || IsNodeOnList(&MEC(opList), &(token->op.node)) ||
        IsNodeOnList(&MEC(cmdList), &(token->op.node)))
    {
        goto done;
    }

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
    }
    else
    {
        token->op.opType = BOP_REM_NAME_REQ;
        token->eType = BTEVENT_NAME_RESULT;
        token->flags = MCTF_NEED_CHECK;
        token->remDev = ME_FindRemoteDevice(&token->p.name.bdAddr);
        MeProcessToken(token);
        token->tokenTimer.func = MeHandleRemNameReqTimeoutNoRspCancelNameReq;
        token->tokenTimer.context = token;
        EVM_StartTimer(&token->tokenTimer, 10000);
        status = BT_STATUS_PENDING;
    }

#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    /* Unlock the stack */
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_CancelGetRemoteDeviceName
 * DESCRIPTION
 *  Request cancelation of a pending ME_GetRemoteDeviceName.
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus ME_CancelGetRemoteDeviceName(MeCommandToken *token)
{
    BtStatus status;
    U8 tokenID = btbmUtilGetNameTokenID(token);

    /* Lock the stack */
    OS_LockStack();

    EVM_ResetTimer((&token->tokenTimer));
    bt_trace(TRACE_GROUP_1,BTLOG_ME_CANCELGETREMOTEDEVICENAME_CANCEL_ORG_TIMER_OF_TOKENxD , tokenID);
    if (IsNodeOnList(&MEC(opList), &(token->op.node)) && (token->op.opType == BOP_REM_NAME_REQ))
    {
        /*
         * GetRemoteName hasn't started. Just remove it from the list
         * and return success.
         */
        RemoveEntryList(&token->op.node);
        //token->callback = NULL;
        status = BT_STATUS_SUCCESS;
        goto done;
    }
    else if (!IsNodeOnList(&MEC(cmdList), &(token->op.node)))
    {
        /* Get remote name operation is not in progress. Return error. */
        status = BT_STATUS_FAILED;
        goto done;
    }

    /* See if other name requests are in progress for this same address. */
    RemoveEntryList(&(token->op.node));
    if (MeFindMatchingToken(token, &MEC(cmdList)) != 0)
    {
        /*
         * Yes, others are on the list. Let them continue and return success
         * to this token only.
         */
        //token->callback = NULL;
        status = BT_STATUS_SUCCESS;
        goto done;
    }
    else
    {
        /*
         * This is the only one. Put the token back on the list to receive
         * the cancel event.
         */
        InsertHeadList(&MEC(cmdList), &(token->op.node));
    }

    token->cancelOp.opType = BOP_CANCEL_REM_NAME;
    InsertHeadList(&MEC(opList), &(token->cancelOp.node));

    /* Attempt to start the operation */
    MeStartOperation();
    bt_trace(TRACE_GROUP_1,BTLOG_ME_CANCELGETREMOTEDEVICENAME_START_A_TIMER_FOR_CANCEL_NAME_TOKENxD , tokenID);
    token->tokenTimer.func = MeHandleRemNameReqChipNoResponse;
    token->tokenTimer.context = token;
    EVM_StartTimer(&token->tokenTimer, 2000);

    status = BT_STATUS_PENDING;
  done:
    /* Unlock the stack */
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_SetLocalDeviceName
 * DESCRIPTION
 *  Set the local device name.
 * PARAMETERS
 *  name        [IN]        
 *  length      [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus ME_SetLocalDeviceName(const U8 *name, U8 length)
{
    /* Lock the stack */
    OS_LockStack();

    /*
     * Always keep the new local name MeWriteLocalName will check for
     * HCI initialization status and write it when the radio is ready.
     */
    MEC(localName) = name;
    MEC(localNameLen) = length;

    MeWriteLocalName();

    /* Unlock the stack */
    OS_UnlockStack();

    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  ME_ReadLocalBdAddr
 * DESCRIPTION
 *  Get the local device BD_ADDR.
 * PARAMETERS
 *  Addr        [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus ME_ReadLocalBdAddr(BD_ADDR *Addr)
{
    /* Lock the stack */
    OS_LockStack();
    *Addr = MEC(bdaddr);
    /* Unlock the stack */
    OS_UnlockStack();
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  ME_CreateLink
 * DESCRIPTION
 *  Create a link connection to the remote device specified by
 *  bdAddr.
 * PARAMETERS
 *  handler         [IN]        
 *  bdAddr          [IN]        
 *  psi             [IN]        
 *  remDevice       [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus ME_CreateLink(BtHandler *handler, BD_ADDR *bdAddr, BtPageScanInfo *psi, BtRemoteDevice **remDevice)
{
    BtRemoteDevice *remDev;
    BtStatus status;
    BtDeviceContext *bdc = 0;

    OS_Report("[ME][CON] ME_CreateLink : handler=%p", handler);

    if ((handler == 0) || (handler->callback == 0) || (remDevice == 0) ||
        (bdAddr == 0) || !IsEntryAvailable(&(handler->node)))
    {

        if (handler == 0)
        {
            bt_trace(TRACE_GROUP_1, BT_ME_CREATELINK_CASE, 1);
        }

        if (handler->callback == 0)
        {
            bt_trace(TRACE_GROUP_1, BT_ME_CREATELINK_CASE, 2);
        }

        if (remDevice == 0)
        {
            bt_trace(TRACE_GROUP_1, BT_ME_CREATELINK_CASE, 3);
        }

        if (bdAddr == 0)
        {
            bt_trace(TRACE_GROUP_1, BT_ME_CREATELINK_CASE, 4);
        }

        if (!IsEntryAvailable(&(handler->node)))
        {
            bt_trace(TRACE_GROUP_1, BT_ME_CREATELINK_CASE, 5);
        }

        status = BT_STATUS_FAILED;
        goto done;
    }

    /* Lock the stack */
    OS_LockStack();

    /* First reset the pending flag */
    handler->pending = BT_CONN_PENDING_IDLE;    

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlock;
    }
    if (BTBMCheckAclConnectRequestNotAddPendingCon() == TRUE)
    {
        status = BT_STATUS_NO_RESOURCES;
        goto unlock;
    }
    /* First determine if a connection already exists to this device */
    if ((remDev = ME_FindRemoteDevice(bdAddr)) != 0)
    {
        /*
         * We found a connection to this device. Now determine if another
         * device can bind to it.
         */
        if (remDev->restricted)
        {
            status = BT_STATUS_RESTRICTED;

            goto unlock;
        }

        if (remDev->state > BDS_CONNECTED)
        {
            /* ME Link Pending */
            bt_prompt_trace(MOD_BT, "[ME][LINK] create link when disconnection is ongoing");
            handler->pending = BT_CONN_PENDING_BREDR;
            /* 
            status = BT_STATUS_IN_PROGRESS;
            goto unlock;
            */
        }else{
            handler->pending = BT_CONN_PENDING_IDLE;
        }
        *remDevice = remDev;

        /*
         * We can bind to this device. Add the handler to the remote device's
         * handlers list.
         */
        bt_trace(TRACE_GROUP_1, MECREATELINK_INSERT_HANDLER_1);
        InsertTailList(&(remDev->handlers), &(handler->node));
        if (remDev->state != BDS_CONNECTED){
             remDev->discFlag = TRUE;
	}
					

        if (remDev->state == BDS_CONNECTED)
        {
            status = BT_STATUS_SUCCESS;
            if (remDev->disconnectACLCheckCreatACLTimer.func != 0)
            {
                if (remDev->disconnectACLCheckCreatACLAlwaysForce != 1)
                {
                    status = BT_STATUS_FAILED;
                    bt_trace(TRACE_GROUP_1, BTLOG_ME_CREATELINK_REMOVE_INSERTED_HDLRE_DUE_TO_RETURN_FAILED);
                    RemoveEntryList(&(handler->node));
                }
                else
                {
                    bt_trace(TRACE_GROUP_1, BTLOG_REMDEVDISCONNECTACLCHECKCREATACLTIMERFUNC_0);
                    remDev->disconnectACLCheckCreatACLCount = 0;
                    EVM_ResetTimer(&remDev->disconnectACLCheckCreatACLTimer);
                }
            }
        }
        else
        {
            /*
             * An out going or incoming connection is in progress so return
             * status pending.
             */
            status = BT_STATUS_PENDING;
        }
        goto unlock;
    }

    /*
     * There is no current remote device so see if we can start a connection.
     * First we need to get a free RemoteDevice structure
     */
    if ((remDev = MeGetFreeRemoteDevice()) == 0)
    {
        status = BT_STATUS_NO_RESOURCES;
        goto unlock;
    }
    *remDevice = remDev;

    /* We have RemoteDevice structure so put the operation on the queue */
    bt_trace(TRACE_GROUP_1, MECREATELINK_INSERT_HANDLER_2);
    InsertTailList(&(remDev->handlers), &(handler->node));
    remDev->state = BDS_OUT_CON;
    MEC(pendCons) += 1;
    Assert(MEC(pendCons) > 0);

    if (psi != 0)
    {
        /* if caller has passed the psi, we use the argument */
        remDev->parms.con.psi = *psi;
    }
    else if ((bdc = DS_FindDevice(bdAddr)) != 0)
    {
        /* 
         * If caller didn't pass the psi, we try to find one.
         * we add this code block to handle that 
         * HSP(headset side) can call Me_CreateLlink() but it can't get clockoffset info
         */
        remDev->parms.con.psi = bdc->psi;
    }
    else
    {
        /* Both caller and global context didn't contain the psi info */
        /* Set to default values */
        remDev->parms.con.psi.clockOffset = 0;
        remDev->parms.con.psi.psMode = 0;
        remDev->parms.con.psi.psRepMode = 1;
    }
    {
        BtPageScanInfo psiRecord;
        if( BT_STATUS_SUCCESS == DDB_PSI_FindRecord(*bdAddr, &psiRecord) ){
            remDev->parms.con.psi.clockOffset = psiRecord.clockOffset;
            OS_Report("[ME] Replace clockOffset in DB. clockoffset=%u", psiRecord.clockOffset);
        }
    }
    
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_UNPAGEABLE);

    remDev->bdAddr = *bdAddr;
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTMT6601_SPECIAL_CONNECTION_PROCEDURE);
    EVM_ResetTimer(&remDev->connectTimer);
    EVM_ResetTimer(&remDev->changePacketTimer);
    remDev->connectTimer.func = MeHandleSpecialCreateLinkProc;
    remDev->connectTimer.context = remDev;
    EVM_StartTimer(&remDev->connectTimer, 1);
    remDev->connectCounter = 0;

    status = BT_STATUS_PENDING;

  unlock:
    /* Unlock the stack */
    OS_UnlockStack();

#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;

}


/*****************************************************************************
 * FUNCTION
 *  ME_CheckCreatingLink
 * DESCRIPTION
 *  Check If the BB is create a new ACL link.
 * PARAMETERS
 *  void
 * RETURNS
 *  status of the current state.
 *****************************************************************************/
BOOL ME_CheckCreatingLink(void)
{
    U8 i = 0;
    BOOL status = FALSE;

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if ((MEC(devTable)[i].state == BDS_OUT_CON) || (MEC(devTable)[i].state == BDS_OUT_CON2))
        {
            status = TRUE;
            break;
        }
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_ForceDisconnectLinkWithReason
 * DESCRIPTION
 *  Disconnect the link. The link will only be disconnected when
 *  all clients have issued disconnect.
 * PARAMETERS
 *  handler             [IN]        
 *  remDev              [IN]        
 *  reason              [IN]        
 *  forceDisconnect     [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus ME_ForceDisconnectLinkWithReason(
            BtHandler *handler,
            BtRemoteDevice *remDev,
            BtErrorCode reason,
            BOOL forceDisconnect)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();
    bt_trace(TRACE_GROUP_1, ME_FORCEDISCONNECTLINKWITHREASONxD, forceDisconnect);
    if ((remDev == 0) || (handler == 0 && !forceDisconnect) ||
        (handler && !IsNodeOnList(&(remDev->handlers), &(handler->node))))
    {
        status = BT_STATUS_FAILED;
        if (remDev == 0)
        {
            bt_trace(TRACE_GROUP_1, REMDEV_0);
        }
        if (handler == 0)
        {
            bt_trace(TRACE_GROUP_1, HANDLER__0);
        }
        if (!forceDisconnect)
        {
            bt_trace(TRACE_GROUP_1, FORCEDISCONNECT);
        }
        if (!IsNodeOnList(&(remDev->handlers), &(handler->node)))
        {
            bt_trace(TRACE_GROUP_1, ISNODEONLISTREMDEVHANDLERS_HANDLERNODE);
        }

        Assert(0);
        goto done;
    }

    /* set the disconnect reason */
    remDev->discReason = reason;

    if (!IsNodeOnList(&MEC(opList), &(remDev->op.node)) &&
        ((remDev->state == BDS_OUT_CON) || (remDev->state == BDS_IN_CON)))
    {
        /*
         * Connection operation is in progress so we need to wait until it
         * is finished
         */
        Report(("Disconnect in progress"));
#ifdef __BT_4_0_BLE__        
        {
            BtDeviceContext *bdc = 0;
            if ((bdc = DS_FindDevice(&remDev->bdAddr)) != 0)
            {
                if(bdc->devType == BT_DEV_TYPE_LE)
                {
                    EVM_ResetTimer(&remDev->connectTimer);
                    remDev->connectTimer.func = LeHandleConnectTimeout;
                    remDev->connectTimer.context = remDev;
                    EVM_StartTimer(&remDev->connectTimer, 1);
                    /* Let it immediately timeout */
                }
            }
        }
#endif
        status = BT_STATUS_IN_PROGRESS;
        goto done;
    }
    Report(("remDev->state:%d",remDev->state));

    if (handler)
    {
        /* Remove the handler from the list */
        RemoveEntryList(&(handler->node));
    }

    /* Now see if all items have been removed and L2CAP has no channels */
    if ((forceDisconnect == TRUE || reason == BEC_POWER_OFF) || IsListEmpty(&(remDev->handlers)))
    {
        /* Set the discFlag */
        remDev->discFlag = TRUE;

        if ((MEC(stackState) < BTSS_INITIALIZE_ERR) &&
            ((remDev->refCount == 0) || (reason == BEC_POWER_OFF) || (forceDisconnect == TRUE)))
        {
            MeDisconnectLink(remDev, 7);
        }
    }

    status = BT_STATUS_SUCCESS;
  done:
    /* Unlock the stack */
    OS_UnlockStack();
    return status;

}


/*****************************************************************************
 * FUNCTION
 *  ME_CancelCreateLink
 * DESCRIPTION
 *  Request cancelation of a pending ME_CreateLink.
 * PARAMETERS
 *  handler     [IN]        
 *  remDev      [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus ME_CancelCreateLink(BtHandler *handler, BtRemoteDevice *remDev)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    if (IsNodeOnList(&MEC(opList), &(remDev->op.node)) && (remDev->op.opType == BOP_LINK_CONNECT))
    {
        /*
         * CreateLink hasn't started. Just remove it from the list
         * and return success.
         */
        RemoveEntryList(&remDev->op.node);

        /* Remove the handler from the list */
        RemoveEntryList(&(handler->node));

        status = BT_STATUS_SUCCESS;
        goto done;
    }
    else if (remDev->state != BDS_OUT_CON)
    {
        /* Connection operation is not in progress. Return error. */
        status = BT_STATUS_FAILED;
        goto done;
    }

    if (!IsListEmpty(&(remDev->handlers)))
    {
        /*
         * This is not the only pending connect. Handler is removed,
         * return success.
         */
        status = BT_STATUS_SUCCESS;
        goto done;
    }

    remDev->cancelOp.opType = BOP_CANCEL_CONNECT;
    InsertHeadList(&MEC(opList), &(remDev->cancelOp.node));

    /* Attempt to start the operation */
    MeStartOperation();
    status = BT_STATUS_SUCCESS;
  done:
    /* Unlock the stack */
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_RegisterAcceptHandler
 * DESCRIPTION
 *  Register a handler for dealing with incoming connections.
 * PARAMETERS
 *  handler     [IN]        
 * RETURNS
 *  Pointer to old handler.
 *****************************************************************************/
BtHandler *ME_RegisterAcceptHandler(BtHandler *handler)
{
    BtHandler *tmpHandler;

    /* Lock the stack */
    OS_LockStack();

    tmpHandler = MEC(acceptHandler);
    MEC(acceptHandler) = handler;

    /* Unlock the stack */
    OS_UnlockStack();
    return tmpHandler;
}


/*****************************************************************************
 * FUNCTION
 *  ME_AcceptIncomingLink
 * DESCRIPTION
 *  Accept an incoming connection.
 * PARAMETERS
 *  remDev      [IN]        
 *  role        [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus ME_AcceptIncomingLink(BtRemoteDevice *remDev, BtConnectionRole role)
{
    BtStatus status;

    status = BT_STATUS_INVALID_PARM;
    if (remDev == 0)
    {
        goto done;
    }

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlock;
    }

    /*
     * Determine if the remDev is in a state for dealing
     * with incoming connections
     */
    if ((remDev->state != BDS_IN_CON) || (remDev->op.opType != BOP_NOP))
    {
        status = BT_STATUS_FAILED;
    }
    else
    {
        /* Start the operation */

        /*
         * Be sure thar the remDev->op is not in MEC(opList):  
         * otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
         * For later, to execute this opType will assert due to opType is equal zero!
         */
        if(IsNodeOnList(&MEC(opList), &(remDev->op.node)))
        {
            Assert(0);
        }
        remDev->op.opType = BOP_LINK_ACCEPT;
        remDev->restricted = FALSE;
        remDev->parms.acceptRole = role;
        remDev->role = role;

        /* Add the operation to the queue */
        if(!IsEntryAvailable(&(remDev->op.node)))
        {
            Assert(0);
        }
        MeAddOperation(remDev);

        /* Attempt to start the operation */
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }

  unlock:
    /* Unlock the stack */
    OS_UnlockStack();

#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_RejectIncomingLink
 * DESCRIPTION
 *  Reject an incoming connection.
 * PARAMETERS
 *  remDev      [IN]        
 *  reason      [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus ME_RejectIncomingLink(BtRemoteDevice *remDev, BtErrorCode reason)
{
    BtStatus status;

    status = BT_STATUS_INVALID_PARM;
    if ((remDev == 0) || ((reason != BEC_LIMITED_RESOURCE) &&
                          (reason != BEC_SECURITY_ERROR) && reason != BEC_PERSONAL_DEVICE))
    {
        goto done;
    }

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlock;
    }

    /*
     * Determine if the remDev is in a state for dealing
     * with incoming connections
     */
    if ((remDev->state != BDS_IN_CON) || (remDev->op.opType != BOP_NOP))
    {
        /* Not in the correct state for incoming connections */
        status = BT_STATUS_FAILED;
    }
    else
    {
        /* Start the operation */

        /*
         * Be sure thar the remDev->op is not in MEC(opList):  
         * otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
         * /For later, to execute this opType will assert due to opType is equal zero!
         */
        Assert(!IsNodeOnList(&MEC(opList), &(remDev->op.node)));
        remDev->op.opType = BOP_LINK_REJECT;
        remDev->parms.rejectReason = reason;

        /* Add the operation to the queue */
        if(!IsEntryAvailable(&(remDev->op.node)))
        {
            Assert(0);
        }
        MeAddOperation(remDev);

        /* Attempt to start the operation */
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }
  unlock:
    /* Unlock the stack */
    OS_UnlockStack();

#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;
}

#if BT_SECURITY == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  SEC_RegisterPairingHandler
 * DESCRIPTION
 *  Register a handler for dealing with pairing.
 * PARAMETERS
 *  handler     [IN]        
 * RETURNS
 *  Pointer to old handler.
 *****************************************************************************/
BtHandler *SEC_RegisterPairingHandler(BtHandler *handler)
{
    BtHandler *tmpHandler;

    /* Lock the stack */
    OS_LockStack();

    tmpHandler = MEC(pairingHandler);
    MEC(pairingHandler) = handler;

    /* Unlock the stack */
    OS_UnlockStack();
    return tmpHandler;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_RegisterAuthorizeHandler
 * DESCRIPTION
 *  Register a handler for dealing with authorization.
 * PARAMETERS
 *  handler     [IN]        
 * RETURNS
 *  Pointer to old handler.
 *****************************************************************************/
BtHandler *SEC_RegisterAuthorizeHandler(BtHandler *handler)
{
    BtHandler *tmpHandler;

    /* Lock the stack */
    OS_LockStack();

    tmpHandler = MEC(authorizeHandler);
    MEC(authorizeHandler) = handler;

    /* Unlock the stack */
    OS_UnlockStack();
    return tmpHandler;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_SetPin
 * DESCRIPTION
 *  Provide a pin code for the pairing process.
 * PARAMETERS
 *  remDev      [IN]        
 *  pin         [IN]        
 *  len         [IN]        
 *  type        [IN]        
 * RETURNS
 *  Pointer to old handler.
 *****************************************************************************/
BtStatus SEC_SetPin(BtRemoteDevice *remDev, const U8 *pin, U8 len, BtPairingType type)
{
    BtStatus status;
    BtSecurityOp *secOp;

    if (remDev == 0)
    {
        status = BT_STATUS_INVALID_PARM;
        goto done;
    }

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlockStack;
    }

    /* Verify that the remote device is in a state to accept a pin */
    if ((remDev->state == BDS_DISCONNECTED) ||
        (remDev->state > BDS_CONNECTED) ||
        ((remDev->authState != BAS_WAITING_FOR_PIN) && (remDev->authState != BAS_WAITING_FOR_PIN_R)))
    {
        status = BT_STATUS_FAILED;
        goto unlockStack;
    }

    /* Get a security op */
    secOp = SecBtGetFreeSecurityOp();

    if (secOp == 0)
    {
        /* We couldn't get a security operation token so fail */
        status = BT_STATUS_NO_RESOURCES;
        goto unlockStack;
    }

    /* Copy the BD_ADDR */
    secOp->bdAddr = remDev->bdAddr;

    /* Verify the parameters */
    if (pin != 0)
    {
        if ((len == 0) || (len > 16))
        {
            status = BT_STATUS_INVALID_PARM;
            goto unlockStack;
        }
        /* We have a pin so copy into the security op */
        secOp->remDev = remDev;
        secOp->x.pLen = len;
        OS_MemSet(secOp->pin, 0, sizeof(secOp->pin));
        OS_MemCopy(secOp->pin, pin, len);
        remDev->pairingType = type;
    }
    else
    {
        /* Send a negative reply by setting secOp->remDev to 0. */
        secOp->remDev = 0;
        remDev->authState = BAS_NOT_AUTHENTICATED;
    }

    /* Add the operation to the queue */
    secOp->op.opType = BOP_PIN_CODE_RSP;
    MeAddOperation(secOp);

    /* Attempt to start the operation */
    MeStartOperation();
    status = BT_STATUS_PENDING;

  unlockStack:
    /* Unlock the stack */
    OS_UnlockStack();

#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_SetPinByAddr
 * DESCRIPTION
 *  Provide a pin code for the pairing process.
 * PARAMETERS
 *  addr        [IN]        
 *  pin         [IN]        
 *  len         [IN]        
 *  type        [IN]        
 * RETURNS
 *  The command status.
 *****************************************************************************/
BtStatus SEC_SetPinByAddr(BD_ADDR addr, const U8 *pin, U8 len, BtPairingType type)
{
    BtStatus status = BT_STATUS_NO_RESOURCES;
    BtSecurityOp *secOp;

    OS_Report("SEC_SetPinByAddr pin=%s addr=0x%X,0x%X,0x%X,0x%X,0x%X,0x%X", pin, addr.addr[0],addr.addr[1],addr.addr[2],addr.addr[3],addr.addr[4],addr.addr[5]);
    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlockStack;
    }

    /* Get a security op */
    secOp = SecBtGetFreeSecurityOp();

    if (secOp == 0)
    {
        /* We couldn't get a security operation token so fail */
        status = BT_STATUS_NO_RESOURCES;
        goto unlockStack;
    }

    /* Copy the BD_ADDR */
    secOp->bdAddr = addr;
    OS_Report("secOp->bdAddr=0x%X,0x%X,0x%X,0x%X,0x%X,0x%X", secOp->bdAddr.addr[0],secOp->bdAddr.addr[1],secOp->bdAddr.addr[2],secOp->bdAddr.addr[3],secOp->bdAddr.addr[4],secOp->bdAddr.addr[5]);

    /* Verify the parameters */
    if (pin != 0)
    {
        if ((len == 0) || (len > 16))
        {
            status = BT_STATUS_INVALID_PARM;
            goto unlockStack;
        }
        /* We have a pin so copy into the security op */
        secOp->x.pLen = len;
        secOp->remDev = 0;
        OS_MemSet(secOp->pin, 0, sizeof(secOp->pin));
        OS_MemCopy(secOp->pin, pin, len);
    }
    else
    {
        /* Send a negative reply by setting secOp->remDev to 0. */
        secOp->x.pLen = 0;
        secOp->remDev = 0;
    }

    /* Add the operation to the queue */
    secOp->op.opType = BOP_PIN_CODE_BY_ADDR_RSP;
    MeAddOperation(secOp);

    /* Attempt to start the operation */
    MeStartOperation();
    status = BT_STATUS_PENDING;

  unlockStack:
    /* Unlock the stack */
    OS_UnlockStack();

    OS_Report("SEC_SetPinByAddr status=%d", status);
    return status;
}

#if defined (__BT_2_1_SIMPLE_PAIRING__)
/*****************************************************************************
 * FUNCTION
 *  SEC_PasskeyEntry
 * DESCRIPTION
 *  Provide a passkey for the pairing process.
 * PARAMETERS
 *  addr                [IN]        
 *  auth_required       [IN]        
 *  accept              [IN]        
 * RETURNS
 *  The command status.
 *****************************************************************************/
BtStatus SEC_PasskeyEntry(BD_ADDR addr, BOOL accept, U32 passkey)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtDeviceContext *bdc;

    /* Make sure the stack does not have an HCI initialization error */
    bt_trace(TRACE_FUNC, BT_MEIF_SEC_PASSKEYENTRY, accept, passkey);
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto done;
    }
    if ((bdc = DS_FindDevice(&addr)) != NULL)
    {
#ifdef __BT_4_0_BLE__
  #ifdef __BT_LE_STANDALONE__
        status = SM_PasskeyEntry(bdc, accept, passkey);
  #else
        if (bdc->devType == BT_DEV_TYPE_LE)
        {
            status = SM_PasskeyEntry(bdc, accept, passkey);
        }
  #endif
#endif
    }
    else
    {
        status = BT_STATUS_DEVICE_NOT_FOUND;
        bt_trace(TRACE_ERROR, BT_DEVICE_NOT_FOUND, 2);
    }
done:
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  SEC_SetIoCapabilityByAddr
 * DESCRIPTION
 *  Provide a pin code for the pairing process.
 * PARAMETERS
 *  addr                [IN]        
 *  auth_required       [IN]        
 *  accept              [IN]        
 * RETURNS
 *  The command status.
 *****************************************************************************/
BtStatus SEC_SetIoCapabilityByAddr(BD_ADDR addr, U8 auth_required, BOOL accept)
{
    BtStatus status = BT_STATUS_NO_RESOURCES;
    BtSecurityOp *secOp;
    BtRemoteDevice *remDev = 0;

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlockStack;
    }
    /* Get a security op */
    secOp = SecBtGetFreeSecurityOp();
    if (secOp == 0)
    {
        /* We couldn't get a security operation token so fail */
        status = BT_STATUS_NO_RESOURCES;
        goto unlockStack;
    }

    if (accept == TRUE)
    {
        remDev = ME_FindRemoteDeviceP(addr.addr);
        if (remDev == 0)
        {
            bt_trace(TRACE_GROUP_1, BTLOG_REMDEV_0);
        }
        else
        {
            bt_trace(TRACE_GROUP_1, BTLOG_CURRENT_IOCAPABILITIYSTATE0Xx02X, remDev->ioCapabilitiyState);
            if ((remDev->ioCapabilitiyState & BIO_REQUESTD_LOCAL) != 0)
            {
                remDev->ioCapabilitiyState |= BIO_RESPONSE_LOCAL;
            }
            else
            {
                bt_trace(TRACE_GROUP_1, BTLOG_WRONG_STATE);
                goto unlockStack;
            }
        }

    }

    /* Copy the BD_ADDR */
    secOp->bdAddr = addr;
    /* We have a pin so copy into the security op */
    if (accept == TRUE)
    {
        secOp->x.ioCapability = 1;
        MEC(sspAuthenRequirements) = auth_required;
    }
    else
    {
        secOp->x.ioCapability = 0;
    }

    secOp->remDev = 0;
    /* Add the operation to the queue */
    secOp->op.opType = BOP_WRITE_IO_CAPABILITY_RESPONSE;
    MeAddOperation(secOp);

    /* Attempt to start the operation */
    MeStartOperation();
    status = BT_STATUS_PENDING;

  unlockStack:

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_SetUserConfirmByAddr
 * DESCRIPTION
 *  Provide a the user confirm result.
 * PARAMETERS
 *  addr            [IN]        
 *  userConfirm     [IN]        
 * RETURNS
 *  The command status.
 *****************************************************************************/
BtStatus SEC_SetUserConfirmByAddr(BD_ADDR addr, BOOL userConfirm)
{
    BtStatus status = BT_STATUS_NO_RESOURCES;
    BtSecurityOp *secOp;
    BtDeviceContext *bdc;

    OS_Report("SEC_SetUserConfirmByAddr() userConfirm=%d", userConfirm);
    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlockStack;
    }

    if ((bdc = DS_FindDevice(&addr)) != NULL)
    {
#ifdef __BT_4_0_BLE__
  #ifdef __BT_LE_STANDALONE__
        status = SM_JustWorkRsp(bdc, userConfirm);
        goto unlockStack;
  #else
        if (bdc->devType == BT_DEV_TYPE_LE)
        {
            status = SM_JustWorkRsp(bdc, userConfirm);
            goto unlockStack;
        }
  #endif
#endif
    }

    /* Get a security op */
    secOp = SecBtGetFreeSecurityOp();

    if (secOp == 0)
    {
        /* We couldn't get a security operation token so fail */
        status = BT_STATUS_NO_RESOURCES;
        goto unlockStack;
    }

    /* Copy the BD_ADDR */
    secOp->bdAddr = addr;
    secOp->userConfirm = userConfirm;
    secOp->remDev = 0;
    OS_MemSet(secOp->pin, 0, sizeof(secOp->pin));
    /* Add the operation to the queue */
    secOp->op.opType = BOP_USER_CONFIRM_BT_ADDR_RSP;
    MeAddOperation(secOp);

    /* Attempt to start the operation */
    MeStartOperation();
    status = BT_STATUS_PENDING;

  unlockStack:
    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  SEC_SetOobData
 * DESCRIPTION
 *  Provide a the user confirm result.
 * PARAMETERS
 *  addr            [IN]        
 *  userConfirm     [IN]        
 * RETURNS
 *  The command status.
 *****************************************************************************/
BtStatus SEC_SetOobData(BD_ADDR addr, BOOL userConfirm, U8 *c, U8 *r)
{
    BtStatus status = BT_STATUS_NO_RESOURCES;
    BtSecurityOp *secOp;
    BtDeviceContext *bdc;

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlockStack;
    }

    if ((bdc = DS_FindDevice(&addr)) != NULL)
    {
#ifdef __BT_4_0_BLE__
  #ifdef __BT_LE_STANDALONE__
        status = SM_SetOobData(bdc, userConfirm, c);
        goto unlockStack;
  #else
        if (bdc->devType == BT_DEV_TYPE_LE)
        {
            status = SM_SetOobData(bdc, userConfirm, c);
            goto unlockStack;
        }
  #endif
#endif
    }

    /* Get a security op */
    secOp = SecBtGetFreeSecurityOp();

    if (secOp == 0)
    {
        /* We couldn't get a security operation token so fail */
        status = BT_STATUS_NO_RESOURCES;
        goto unlockStack;
    }

    /* Copy the BD_ADDR */
    secOp->bdAddr = addr;
    secOp->userConfirm = userConfirm;
    secOp->remDev = 0;
    OS_MemCopy(secOp->c, c, 16);
    OS_MemCopy(secOp->r, r, 16);
    /* Add the operation to the queue */
    secOp->op.opType = BOP_USER_OOB_DATA_RSP;
    MeAddOperation(secOp);

    /* Attempt to start the operation */
    MeStartOperation();
    status = BT_STATUS_PENDING;

unlockStack:
    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}

#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 


/*****************************************************************************
 * FUNCTION
 *  SEC_AuthenticateLink
 * DESCRIPTION
 *  Authenticate an existing link.
 * PARAMETERS
 *  handler     [IN]        
 *  remDev      [IN]        
 * RETURNS
 *  Pointer to old handler.
 *****************************************************************************/
BtStatus SEC_AuthenticateLink(BtHandler *handler, BtRemoteDevice *remDev)
{
    BtStatus status;

    if (remDev == 0)
    {
        status = BT_STATUS_INVALID_PARM;
        goto done;
    }

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlock;
    }

    remDev->authHandler = handler;
    status = SecAddAuthenticateOpToQueue(remDev);
  unlock:
    /* Unlock the stack */
    OS_UnlockStack();

#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_Register
 * DESCRIPTION
 *  Register a security record.
 * PARAMETERS
 *  sr      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SEC_Register(BtSecurityRecord *sr)
{
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    return BT_STATUS_SUCCESS;
#else /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */
    SEC_RegisterRecord(sr);
    return BT_STATUS_SUCCESS;
#endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */
}


/*****************************************************************************
 * FUNCTION
 *  SEC_RegisterRecord
 * DESCRIPTION
 *  Register a security record.
 * PARAMETERS
 *  sr      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SEC_RegisterRecord(BtSecurityRecord *sr)
{
    BtStatus status;
    BtSecurityRecord *tsr;

    /* Lock the stack */
    OS_LockStack();

    status = BT_STATUS_INVALID_PARM;
    if (sr == 0)
    {
        goto done;
    }

    if (IsNodeOnList(&(MEC(secList)), &(sr->node)))
    {
        status = BT_STATUS_FAILED;
        goto done;
    }
#if defined (__BT_2_1_SIMPLE_PAIRING__)
    sr->level = BSL_DEFAULT_ALL;
#else /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
    if (sr->level & (BSL_AUTHORIZATION_IN | BSL_ENCRYPTION_IN))
    {
        if ((sr->level & BSL_AUTHENTICATION_IN) == 0)
        {
            goto done;
        }
    }

    if (sr->level & (BSL_AUTHORIZATION_OUT | BSL_ENCRYPTION_OUT))
    {
        if ((sr->level & BSL_AUTHENTICATION_OUT) == 0)
        {
            goto done;
        }
    }

    if (sr->level & (BSL_AUTHORIZATION_IN | BSL_ENCRYPTION_IN))
    {
        if ((sr->level & BSL_AUTHENTICATION_IN) == 0)
        {
            Assert(0);
        }
    }

    if (sr->level & (BSL_AUTHORIZATION_OUT | BSL_ENCRYPTION_OUT))
    {
        if ((sr->level & BSL_AUTHENTICATION_OUT) == 0)
        {
            Assert(0);
        }
    }
#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto done;
    }

    /*
     * Make sure we don't already have a record that contains the protocol ID
     * and channel
     */
    tsr = (BtSecurityRecord*) MEC(secList).Flink;
    while ((ListEntry*) tsr != &MEC(secList))
    {
        if ((tsr->id == sr->id) && (tsr->channel == sr->channel))
        {
            /* We have found a matching record */
            status = BT_STATUS_IN_USE;
            goto done;
        }
        /* Advance to the next record */
        tsr = (BtSecurityRecord*) tsr->node.Flink;
    }

    /* Add the item to the list */
    InsertTailList(&(MEC(secList)), &(sr->node));
    status = BT_STATUS_SUCCESS;

  done:
    /* Unlock the stack */
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_Unregister
 * DESCRIPTION
 *  Unregister a security record.
 * PARAMETERS
 *  sr      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SEC_Unregister(BtSecurityRecord *sr)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    if ((sr != 0) && IsNodeOnList(&(MEC(secList)), &(sr->node)))
    {
        RemoveEntryList(&(sr->node));
        status = BT_STATUS_SUCCESS;
    }
    else
    {
        status = BT_STATUS_FAILED;
    }

    /* Unlock the stack */
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_AccessRequest
 * DESCRIPTION
 *  Request accesss from the security manager.
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SEC_AccessRequest(BtSecurityToken *token)
{
    BtStatus status;
    BtSecurityRecord *sr;

    if ((token == 0) || (token->remDev == 0))
    {
        status = BT_STATUS_INVALID_PARM;
        goto done;
    }

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlock;
    }

    /*
     * Search the security records for a record that contains the protocol ID
     * and channel
     */
    sr = (BtSecurityRecord*) MEC(secList).Flink;
    while ((ListEntry*) sr != &MEC(secList))
    {
        if ((sr->id == token->id) && (sr->channel == token->channel))
        {
            /* We have found a matching record */
            bt_trace(TRACE_GROUP_1, BT_SEC_ACCESS_REQ_FIND_ONE);
            break;
        }
        /* Advance to the next record */
        sr = (BtSecurityRecord*) sr->node.Flink;
    }

    if ((ListEntry*) sr != &MEC(secList))
    {
        /*
         * We have a record so store the appropriate information into
         * the token.
         */
        token->record = sr;
        token->level = sr->level;
        bt_trace(TRACE_GROUP_1, BT_LOG_SEC_ACCESS_REQ_TOKEN_LEVEL, 1, token->level);
        /* Move the check into SecProcessSecurityToken */
        #if 0
        /* If SEC_LOW and remote does not support SSP, just pass SEC check */
        if((sr->level_type == BT_SEC_LOW){
            switch(ME_IsSSPSupported(token->remDev)){
            case BT_STATUS_NOT_SUPPORTED:
                OS_Report("SEC is low and SSP(host) is not supported");
                status = BT_STATUS_SUCCESS;
                goto unlock;                
            case BT_STATUS_PENDING:
            case BT_STATUS_SUCCESS:
            default:
                break;
            }
        }
        #endif
    }
    else
    {
        /* There is no security record for this protocol/channel. */
    #if BT_DEFAULT_SECURITY == XA_ENABLED
        token->level = BSL_DEFAULT;
    #else 
        token->level = BSL_NO_SECURITY;
    #endif 
        bt_trace(TRACE_GROUP_1, BT_LOG_SEC_ACCESS_REQ_TOKEN_LEVEL, 2, token->level);
    }
#if !defined (__BT_2_1_SIMPLE_PAIRING__)
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    if (token->level != BSL_NO_SECURITY)
    {
        bt_trace(TRACE_GROUP_1, SEC_LEVEL_TYPExD_SEC_LEVELxD, sr->level_type, MEC(secLevel));
        if ((sr->level_type == BT_SEC_LOW) && (MEC(secLevel) == BSM_SEC_LEVEL_1))
        {
            status = BT_STATUS_SUCCESS;
            goto unlock;
        }
    }
#endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */
#endif /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 
    token->result = BTEVENT_ACCESS_DENIED;
    status = SecProcessSecurityToken(token);

    if (status == BT_STATUS_PENDING)
    {

        /*
         * Set the result to 0. We do this to
         * prevent a re-entrant call into the token's callback
         */
        token->result = 0;

        /*
         * Add the token to the queue and start any operations
         * (especially the one that SecProcessSecurityToken added)
         */
        InsertTailList(&(token->remDev->secTokens), &(token->node));
        MeStartOperation();

        /*
         * Did the operation complete? If so, the operation will
         * have stored a different value into the token's result:
         */
        if (token->result == BTEVENT_ACCESS_APPROVED)
        {
            status = BT_STATUS_SUCCESS;
        }
        else if (token->result == BTEVENT_ACCESS_DENIED)
        {
            status = BT_STATUS_FAILED;
        }
        else
        {
            /* Any other value is invalid */
            Assert(token->result == 0);

        #if BT_SECURITY_TIMEOUT != 0
            /* Initiate watchdog timer. */
            token->timer.context = (void*)token;
            token->timer.func = SecAccessTimeoutHandler;
            EVM_StartTimer(&token->timer, (BT_SECURITY_TIMEOUT) * 1000);
        #endif /* BT_SECURITY_TIMEOUT != 0 */
        }

        /* Replace the token's result for normal callback behavior */
        token->result = BTEVENT_ACCESS_DENIED;
    }
  unlock:
    /* Unlock the stack */
    OS_UnlockStack();

#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_AuthorizeServiceB
 * DESCRIPTION
 *  Authorize the link in response to a BTEVENT_AUTHORIZATION_REQ event.
 * PARAMETERS
 *  remDev              [IN]        
 *  isAuthorized        [IN]        
 *  futureTrust         [IN]        
 *  authService         [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SEC_AuthorizeServiceB(BtRemoteDevice *remDev, BOOL isAuthorized, BOOL futureTrust, BOOL authService)
{
    BtStatus status;
    BtDeviceRecord record;

#if XA_ERROR_CHECK == XA_ENABLED
    if (remDev == 0)
    {
        status = BT_STATUS_INVALID_PARM;
        goto done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlock;
    }

    if ((remDev->state == BDS_CONNECTED) && (remDev->authorizeState == BAS_START_AUTHORIZE))
    {
        Assert(remDev->authState == BAS_AUTHENTICATED);

        /* Assume the operation is successful */
        status = BT_STATUS_SUCCESS;

        /* See if we need to update the device database. */
        if (futureTrust == TRUE)
        {
            status = BT_STATUS_DEVICE_NOT_FOUND;

            /* Set the trust flag in the device database */
            if (SEC_FindDeviceRecord(&(remDev->bdAddr), &record) == BT_STATUS_SUCCESS)
            {
                record.trusted = TRUE;
                if (SEC_AddDeviceRecord(&record) == BT_STATUS_SUCCESS)
                {
                    status = BT_STATUS_SUCCESS;
                }
            }
        }

        /* Set the authorization */
        if (isAuthorized == TRUE)
        {
            if (authService == TRUE)
            {
                remDev->authorizeState = BAS_AUTHORIZED_SERVICE;
            }
            else
            {
                remDev->authorizeState = BAS_AUTHORIZED;
            }
        }
        else
        {
            remDev->authorizeState = BAS_NOT_AUTHORIZED;
        }

        remDev->secAccessState |= BAS_AUTHORIZE_COMPLETE;
        SecProcessAllSecurityTokens(remDev);
    }
    else
    {
        /* Remote device is not in the proper state to receive this event */
        status = BT_STATUS_FAILED;
    }
  unlock:
    /* Unlock the stack */
    OS_UnlockStack();

#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_CancelAccessRequest
 * DESCRIPTION
 *  Cancel Access Request token.
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SEC_CancelAccessRequest(BtSecurityToken *token)
{
    BtStatus status = BT_STATUS_FAILED;
    BtSecurityOp *secOp;

    OS_LockStack();

    if (token->node.Flink && token->node.Blink)
    {
        RemoveEntryList(&token->node);

    #if BT_SECURITY_TIMEOUT != 0
        /* Cancel the watchdog timer if it is in operation. */
        EVM_CancelTimer(&token->timer);
    #endif /* BT_SECURITY_TIMEOUT != 0 */

        if (token->remDev->authState == BAS_WAITING_FOR_PIN || token->remDev->authState == BAS_WAITING_FOR_PIN_R)
        {

            bt_trace(TRACE_GROUP_1, MESEC_ADDING_CANCEL_AUTHENTICATION_OPERATION_TO_QUEUE);

            /* Setup a security op to send the rejection */
            secOp = SecBtGetFreeSecurityOp();
            Assert(secOp != 0);

            /* Set the remDev to 0 indicating a negative response */
            secOp->remDev = 0;

            /* Copy in the BD_ADDR */
            secOp->bdAddr = token->remDev->bdAddr;
            secOp->op.opType = BOP_PIN_CODE_RSP;

            if(!IsEntryAvailable(&(secOp->op.node)))
            {
                Assert(0);
            }
            MeAddOperationFront(secOp);

            /* Attempt to start the operation */
            MeStartOperation();
        }
        else if (token->remDev->authorizeState == BAS_START_AUTHORIZE)
        {

            bt_trace(TRACE_GROUP_1, MESEC_ADDING_CANCEL_AUTHORIZE_OPERATION_TO_QUEUE);

            /* Setup a security op to send the rejection */
            secOp = SecBtGetFreeSecurityOp();
            Assert(secOp != 0);

            secOp->remDev = token->remDev;
            secOp->op.opType = BOP_CANCEL_AUTHORIZE;

            token->remDev->authorizeState = BAS_NOT_AUTHORIZED;
            token->remDev->secRec = token->record;

            if (!IsEntryAvailable(&(secOp->op.node)))
            {
                Assert(0);
            }
            MeAddOperationFront(secOp);

            MeStartOperation();
        }

        status = BT_STATUS_SUCCESS;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_SetLinkEncryption
 * DESCRIPTION
 *  Set link encryption.
 * PARAMETERS
 *  handler     [IN]        
 *  remDev      [IN]        
 *  mode        [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SEC_SetLinkEncryption(BtHandler *handler, BtRemoteDevice *remDev, BtEncryptMode mode)
{
    BtStatus status;
    BtSecurityOp *secOp;

    if ((remDev == 0) || ((mode != BECM_ENCRYPT_DISABLE) && (mode != BECM_ENCRYPT_ENABLE)))
    {
        status = BT_STATUS_INVALID_PARM;
        goto done;
    }

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlock;
    }

    /* See if the link is in the proper state for setting encryption */
    if ((remDev->state != BDS_CONNECTED) ||
        (remDev->encryptState == BES_START_ENCRYPT) || (remDev->encryptState == BES_END_ENCRYPT))
    {
        status = BT_STATUS_IN_PROGRESS;
        goto unlock;
    }

    /* Now see if the desired state is already set */
    if (((mode == BECM_ENCRYPT_DISABLE) &&
         (remDev->encryptState == BES_NOT_ENCRYPTED)) ||
        ((mode == BECM_ENCRYPT_ENABLE) && (remDev->encryptState == BES_ENCRYPTED)))
    {
        status = BT_STATUS_SUCCESS;
        goto unlock;
    }

    /*
     * If we are enabling encryption then make sure that the link is
     * authenticated
     */
    if ((mode == BECM_ENCRYPT_ENABLE) && (remDev->authState != BAS_AUTHENTICATED))
    {

        status = BT_STATUS_FAILED;
        goto unlock;
    }

    /*
     * If we are disabling encryption then make sure that the link is
     * authenticated
     */
    if ((mode == BECM_ENCRYPT_DISABLE) && (remDev->authState != BAS_AUTHENTICATED))
    {

        status = BT_STATUS_FAILED;
        goto unlock;
    }

    /* Time to setup an encryption operation */
    secOp = SecBtGetFreeSecurityOp();

    if (secOp == 0)
    {
        status = BT_STATUS_NO_RESOURCES;
    }
    else
    {
        secOp->remDev = remDev;

        /* Add the encryption operation to the queue */
        secOp->op.opType = BOP_ENCRYPTION;
        secOp->x.eMode = mode;
        remDev->encryptHandler = handler;

        /* Set the encryption state */
        if (mode == BECM_ENCRYPT_DISABLE)
        {
            remDev->encryptState = BES_END_ENCRYPT;
        }
        else
        {
            remDev->encryptState = BES_START_ENCRYPT;
        }

        if (!IsEntryAvailable(&(secOp->op.node)))
        {
            Assert(0);
        }
        MeAddOperation(secOp);

        /* Attempt to start the operation */
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }
  unlock:
    /* Unlock the stack */
    OS_UnlockStack();

#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_EnableSecurityMode3
 * DESCRIPTION
 *  Enable security mode 3.
 * PARAMETERS
 *  handler         [IN]        
 *  encryption      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SEC_EnableSecurityMode3(BtHandler *handler, BOOL encryption)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto done;
    }

    /* See if we are already in security level 3 */
    if (MEC(secModeState) == BSMS_LEVEL3)
    {
        status = BT_STATUS_SUCCESS;
        if (encryption != MEC(secModeEncrypt))
        {
            status = SecSetEncryptMode(handler, encryption);
        }
        goto done;
    }

    /* See if we are in a state to switch to security level 3 */
    if (MEC(secModeState) != BSMS_LEVEL2)
    {
        status = BT_STATUS_FAILED;
        goto done;
    }

    /* Set the mode */
    status = SecSetSecurityMode(handler, BSM_SEC_LEVEL_3, encryption);

  done:
    /* Unlock the stack */
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_DisableSecurityMode3
 * DESCRIPTION
 *  Enable security mode 3.
 * PARAMETERS
 *  handler     [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SEC_DisableSecurityMode3(BtHandler *handler)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto done;
    }

    /* See if we are already in security level 2 */
    if (MEC(secModeState) == BSMS_LEVEL2)
    {
        status = BT_STATUS_SUCCESS;
        goto done;
    }

    /* See if we are in a state to switch to security level 2 */
    if (MEC(secModeState) != BSMS_LEVEL3)
    {
        status = BT_STATUS_FAILED;
        goto done;
    }

    /* Set the mode */
    status = SecSetSecurityMode(handler, BSM_SEC_LEVEL_2, MEC(secModeEncrypt));

  done:
    /* Unlock the stack */
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_DeleteDeviceRecord
 * DESCRIPTION
 *  Delete the record with the given bdAddr from the device
 *  database.
 * PARAMETERS
 *  bdAddr      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SEC_DeleteDeviceRecord(const BD_ADDR *bdAddr)
{
    BtStatus status;

    if (bdAddr == 0)
    {
        status = BT_STATUS_INVALID_PARM;
        goto done;
    }

    /* Lock the stack */
    OS_LockStack();
#ifdef __BT_4_0_BLE__
    ATTDB_NotifyBondStatus(bdAddr->addr, 0x02); 
#endif    

    status = DDB_DeleteRecord(bdAddr);
    DDB_Flush();

    /* Unlock the stack */
    OS_UnlockStack();
#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_AddDeviceRecord
 * DESCRIPTION
 *  Delete the record with the given bdAddr from the device
 *  database.
 * PARAMETERS
 *  record      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SEC_AddDeviceRecord(const BtDeviceRecord *record)
{
    BtStatus status;
    if (record == 0)
    {
        status = BT_STATUS_INVALID_PARM;
        goto done;
    }

    /* Lock the stack */
    OS_LockStack();
#ifdef __BT_4_0_BLE__
    ATTDB_NotifyBondStatus(record->bdAddr.addr, 0x01); 
#endif   

    status = DDB_AddRecord(record);

    /* Unlock the stack */
    OS_UnlockStack();
#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_FindDeviceRecord
 * DESCRIPTION
 *  Find the record with the given bdAddr in the device
 *  database.
 * PARAMETERS
 *  bdAddr      [IN]        
 *  record      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SEC_FindDeviceRecord(const BD_ADDR *bdAddr, BtDeviceRecord *record)
{
    BtStatus status;

    if ((bdAddr == 0) || (record == 0))
    {
        status = BT_STATUS_INVALID_PARM;
        goto done;
    }

    /* Lock the stack */
    OS_LockStack();

    status = DDB_FindRecord(bdAddr, record);

    /* Unlock the stack */
    OS_UnlockStack();
#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SEC_EnumDeviceRecords
 * DESCRIPTION
 *  Enerate record in the device database.
 * PARAMETERS
 *  i           [IN]        
 *  record      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus SEC_EnumDeviceRecords(I16 i, BtDeviceRecord *record)
{
    BtStatus status;

    if (record == 0)
    {
        status = BT_STATUS_INVALID_PARM;
        goto done;
    }

    /* Lock the stack */
    OS_LockStack();

    status = DDB_EnumDeviceRecords(i, record);

    /* Unlock the stack */
    OS_UnlockStack();
#if XA_ERROR_CHECK == XA_ENABLED
  done:
#endif 
    return status;
}
#endif /* BT_SECURITY == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  ME_Hold
 * DESCRIPTION
 *  Put the remote device into hold mode.
 * PARAMETERS
 *  remDev      [IN]        
 *  max         [IN]        
 *  min         [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_Hold(BtRemoteDevice *remDev, U16 max, U16 min)
{
    BtStatus status;

#if XA_ERROR_CHECK == XA_ENABLED
    if ((max == 0) || (min == 0) || (min > max))
    {
        return BT_STATUS_INVALID_PARM;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Assert((max != 0) && (min != 0) && (max >= min));

    /* Lock the stack */
    OS_LockStack();

    status = MeCheckModeChange(remDev, BOP_HOLD_MODE);
    if (status == BT_STATUS_SUCCESS)
    {
        /* Start the operation */

        /*
         * Be sure thar the remDev->op is not in MEC(opList):  
         * otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
         * For later, to execute this opType will assert due to opType is equal zero!
         */
        Assert(!IsNodeOnList(&MEC(opList), &(remDev->op.node)));

        remDev->op.opType = BOP_HOLD_MODE;
        remDev->parms.mode.max = max;
        remDev->parms.mode.min = min;

        Assert(IsEntryAvailable(&(remDev->op.node)));
        MeAddOperation(remDev);

        /* Attempt to start the operation */
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }

    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_StartSniff
 * DESCRIPTION
 *  Put the remote device into sniff mode.
 * PARAMETERS
 *  remDev      [IN]        
 *  info        [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_StartSniff(BtRemoteDevice *remDev, const BtSniffInfo *info)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    status = MeCheckModeChange(remDev, BOP_SNIFF_MODE);
    if (status == BT_STATUS_SUCCESS)
    {
        /* Start the operation */

        /* 
         * Be sure thar the remDev->op is not in MEC(opList):  
         * otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
         * For later, to execute this opType will assert due to opType is equal zero!
         */
        if (IsNodeOnList(&MEC(opList), &(remDev->op.node)))
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSTART_SNIFF_REQ_OP_ON_THE_LIST_RETURN);
            return BT_STATUS_FAILED;
        }

        remDev->op.opType = BOP_SNIFF_MODE;
        remDev->parms.sniff = *info;

        if (!IsEntryAvailable(&(remDev->op.node)))
        {
            Assert(0);
        }
        MeAddOperation(remDev);

        /* Attempt to start the operation */
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }

    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_StopSniff
 * DESCRIPTION
 *  Take the link out of sniff mode.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_StopSniff(BtRemoteDevice *remDev)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    status = MeCheckModeChange(remDev, BOP_EXIT_SNIFF_MODE);
    if (status == BT_STATUS_SUCCESS)
    {
        /* Start the operation */

        /*
         * Be sure thar the remDev->op is not in MEC(opList):  
         * otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
         * For later, to execute this opType will assert due to opType is equal zero!
         */
	#if defined (__SDP_EXIT_SNIFF_MODE__)
        if (remDev->op.opType == BOP_EXIT_SNIFF_MODE)
        {
            return BT_STATUS_PENDING; 
        }   
        else if (remDev->op.opType == BOP_SNIFF_MODE)
        {
            if (IsNodeOnList(&MEC(opList), &(remDev->op.node)))
            {
                remDev->op.opType = BOP_EXIT_SNIFF_MODE;
                return BT_STATUS_PENDING;
            }
            if (&(remDev->op) == MEC(curOp))
            {
                MeOperationEnd();
            }            
        }
        
        if (!IsEntryAvailable(&(remDev->op.node)))
        {
            return BT_STATUS_FAILED;
        }
        remDev->op.opType = BOP_EXIT_SNIFF_MODE;
	/* __SDP_EXIT_SNIFF_MODE__ */

	#else
        if (IsNodeOnList(&MEC(opList), &(remDev->op.node)))
        {
            Assert(0);
        }
        remDev->op.opType = BOP_EXIT_SNIFF_MODE;

        if (!IsEntryAvailable(&(remDev->op.node)))
        {
            Assert(0);
        }
	#endif

        MeAddOperation(remDev);

        /* Attempt to start the operation */
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }

    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}

#if defined (__BT_2_1_SNIFF_SUBRATING__)


/*****************************************************************************
 * FUNCTION
 *  ME_StartSniffSubrating
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus ME_StartSniffSubrating(BtRemoteDevice *remDev)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();
    bt_trace(TRACE_GROUP_1, BTLOG_REMDEVSNIFFSUBRATESTATEx02X, remDev->sniffSubrateState);
    if (IsNodeOnList(&MEC(opList), &(remDev->sniffSubrateOp.node)))
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_SET_SNIFF_SUBRATING_OP_IN_QUEUE);
        return BT_STATUS_FAILED;
    }
    if ((remDev->sniffSubrateState & MTK_SNIFF_SUBRATING_CMD_MAKE) != 0)
    {
        return BT_STATUS_FAILED;
    }
    if ((remDev->sniffSubrateState & MTK_SNIFF_SUBRATING_PARAM_CHANGED) == 0)
    {
        return BT_STATUS_SUCCESS;
    }
    remDev->sniffSubrateOp.opType = BOP_SNIFF_SUBRATING_MODE;
    remDev->sniffSubrateState |= MTK_SNIFF_SUBRATING_CMD_MAKE;
    remDev->sniffSubrateState &= ~MTK_SNIFF_SUBRATING_PARAM_CHANGED;
    if(!IsEntryAvailable(&(remDev->sniffSubrateOp.node)))
    {
        Assert(0);
    }
    InsertTailList(&MEC(opList), &((remDev->sniffSubrateOp).node));

    /* Attempt to start the operation */
    MeStartOperation();
    status = BT_STATUS_PENDING;
    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}
#endif /* defined (__BT_2_1_SNIFF_SUBRATING__) */ 

#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)


/*****************************************************************************
 * FUNCTION
 *  ME_StartWriteEIR
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus ME_StartWriteEIR(void)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();
    if (MEC(eirRecordUpdated) != 1)
    {
        bt_trace(TRACE_GROUP_1, BTLOG_NO_EIR_UPDATE_OPERATION);
        return BT_STATUS_SUCCESS;
    }

    if (IsNodeOnList(&MEC(opList), &(MEC(eirOp).node)))
    {
        bt_trace(TRACE_GROUP_1, BTLOG_EIROP_IN_OPLIST);
        return BT_STATUS_PENDING;
    }
    if ((MEC(curOp) != 0) && (MEC(curOp)->opType == BOP_EXTENDED_INQUIRY_RESPONSE))
    {
        bt_trace(TRACE_GROUP_1, BTLOG_EIROP_NOT_AVAILABLE);
        return BT_STATUS_PENDING;
    }

    MEC(eirOp).opType = BOP_EXTENDED_INQUIRY_RESPONSE;

    InsertTailList(&MEC(opList), &(MEC(eirOp).node));

    /* Attempt to start the operation */
    MeStartOperation();
    status = BT_STATUS_PENDING;
    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 


/*****************************************************************************
 * FUNCTION
 *  ME_StartPark
 * DESCRIPTION
 *  Put the link into park mode.
 * PARAMETERS
 *  remDev      [IN]        
 *  max         [IN]        
 *  min         [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_StartPark(BtRemoteDevice *remDev, U16 max, U16 min)
{
    BtStatus status;

#if XA_ERROR_CHECK == XA_ENABLED
    if ((max == 0) || (min == 0) || (min > max))
    {
        return BT_STATUS_INVALID_PARM;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    Assert((max != 0) && (min != 0) && (max >= min));

    /* Lock the stack */
    OS_LockStack();

    status = MeCheckModeChange(remDev, BOP_PARK_MODE);
    if (status == BT_STATUS_SUCCESS)
    {
        /* Start the operation */

        /*
         * Be sure thar the remDev->op is not in MEC(opList):  
         * otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
         * For later, to execute this opType will assert due to opType is equal zero!
         */
        Assert(!IsNodeOnList(&MEC(opList), &(remDev->op.node)));

        remDev->op.opType = BOP_PARK_MODE;
        remDev->parms.mode.max = max;
        remDev->parms.mode.min = min;

        Assert(IsEntryAvailable(&(remDev->op.node)));
        MeAddOperation(remDev);

        /* Attempt to start the operation */
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }

    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_StopPark
 * DESCRIPTION
 *  Take the link out of park mode.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_StopPark(BtRemoteDevice *remDev)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    status = MeCheckModeChange(remDev, BOP_EXIT_PARK_MODE);
    if (status == BT_STATUS_SUCCESS)
    {
        /* Start the operation */

        /*
         * Be sure thar the remDev->op is not in MEC(opList):  
         * otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
         * For later, to execute this opType will assert due to opType is equal zero!
         */
        Assert(!IsNodeOnList(&MEC(opList), &(remDev->op.node)));

        remDev->op.opType = BOP_EXIT_PARK_MODE;

        Assert(IsEntryAvailable(&(remDev->op.node)));
        MeAddOperation(remDev);

        /* Attempt to start the operation */
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }

    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_StartParkAll
 * DESCRIPTION
 *  Put all links into park mode.
 * PARAMETERS
 *  max     [IN]        
 *  min     [IN]        
 *  num     [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_StartParkAll(U16 max, U16 min, U8 *num)
{
    U8 i, n = 0;
    BtStatus status = BT_STATUS_FAILED;

    Assert(num);

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlock;
    }

    /* Go through the RemoteDevice array looking for connected devices. */
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if (MEC(devTable)[i].state == BDS_CONNECTED)
        {
            /* Let the main function do all the error checking. */
            status = ME_StartPark(MEC(devTable) + i, max, min);
            if (status == BT_STATUS_PENDING)
            {
                n++;
            }
        }
    }

  unlock:
    /* Unlock the stack */
    OS_UnlockStack();

    if ((*num = n) > 0)
    {
        return BT_STATUS_PENDING;
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_StopParkAll
 * DESCRIPTION
 *  Take all links out of park mode.
 * PARAMETERS
 *  num     [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_StopParkAll(U8 *num)
{
    U8 i, n = 0;
    BtStatus status = BT_STATUS_FAILED;

    Assert(num);

    /* Lock the stack */
    OS_LockStack();

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        status = BT_STATUS_HCI_INIT_ERR;
        goto unlock;
    }

    /* Go through the RemoteDevice array looking for connected devices. */
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if ((MEC(devTable)[i].state == BDS_CONNECTED) && (MEC(devTable)[i].mode == BLM_PARK_MODE))
        {
            /* Let the main function do all the error checking. */
            status = ME_StopPark(MEC(devTable) + i);
            if (status == BT_STATUS_PENDING)
            {
                n++;
            }
            else if (status != BT_STATUS_IN_PROGRESS)
            {
                break;
            }
        }
    }

  unlock:
    /* Unlock the stack */
    OS_UnlockStack();

    if ((*num = n) > 0)
    {
        return BT_STATUS_PENDING;
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_SwitchRole
 * DESCRIPTION
 *  Switch the role this device is performing.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_SwitchRole(BtRemoteDevice *remDev)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    status = MeCheckModeChange(remDev, BOP_SWITCH_ROLE);
    if (status == BT_STATUS_SUCCESS)
    {
        /* Start the operation */

        /*
         * Be sure thar the remDev->op is not in MEC(opList):  
         * otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
         * For later, to execute this opType will assert due to opType is equal zero!
         */
        remDev->roleSwitchCounter++;
        if ((IsNodeOnList(&MEC(opList), &(remDev->op.node))))
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTROLE_SWITCH_ON_THE_OPLIST_REUTRN);
            return BT_STATUS_FAILED;
        }
        if(!IsEntryAvailable(&(remDev->op.node)))
        {
            return BT_STATUS_FAILED;
        }
        /* To prevent the role switch failed due to link policy disable role switch */
        Me_ChangeLinkPolicy(remDev, BT_OP_ENABLE, BT_OP_DONT_CARE);

        remDev->op.opType = BOP_SWITCH_ROLE;
        remDev->req_role = (remDev->role==BCR_MASTER) ? BCR_SLAVE : BCR_MASTER;
        MeAddOperation(remDev);

        /* Attempt to start the operation */
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }

    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  ME_SetUpQoS
 * DESCRIPTION
 *  Set up link QoS
 * PARAMETERS
 *  remDev      [IN]        
 *  info        [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_SetUpQoS(BtRemoteDevice *remDev, const BtQoSInfo *info)
{
    BtStatus status;

    /* Lock the stack */
    OS_LockStack();

    /* 
     * Be sure thar the remDev->op is not in MEC(opList):  
     * otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
     * For later, to execute this opType will assert due to opType is equal zero!
     */
    if (IsNodeOnList(&MEC(opList), &(remDev->qosOp.node)))
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSTART_QOS_OP_ON_THE_LIST_RETURN);
        return BT_STATUS_FAILED;
    }

    if (!IsEntryAvailable(&(remDev->qosOp.node)))
    {
        Assert(0);
    }
    else
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_ME_SETUP_QOS, remDev->qos.role, remDev->role);
        remDev->qos = *info;
        if (remDev->qos.role == BCR_UNKNOWN || remDev->role == remDev->qos.role)
        {
            remDev->qosOp.opType = BOP_QOS_SETUP;
            InsertHeadList(&MEC(opList), &(remDev->qosOp.node));
            MeStartOperation();
        }
    }

    status = BT_STATUS_PENDING;

    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}



/*****************************************************************************
 * FUNCTION
 *  ME_SetClassOfDevice
 * DESCRIPTION
 *  Set the class of device.
 * PARAMETERS
 *  classOfDevice       [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_SetClassOfDevice(BtClassOfDevice classOfDevice)
{
    BtClassOfDevice cod;

    /* Lock the stack */
    OS_LockStack();

    /*
     * Always attempt to set the Class of Device
     * MeWriteClassOfDevice will check for HCI initialization error
     */
    cod = (MEC(classOfDevice) | MEC(sdpServCoD));
    MEC(classOfDevice) = classOfDevice;

    if (cod != (MEC(classOfDevice) | MEC(sdpServCoD)))
    {
        MeWriteClassOfDevice();
    }

    /* Unlock the stack */
    OS_UnlockStack();

    return BT_STATUS_SUCCESS;
}

#ifdef __GAP_TESTER_


/*****************************************************************************
 * FUNCTION
 *  ME_DisableAFH
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus ME_DisableAFH(void)
{

    MeDisableAFH();
    /* Unlock the stack */
    OS_UnlockStack();

    return BT_STATUS_SUCCESS;
}

#endif /* __GAP_TESTER_ */ 


/*****************************************************************************
 * FUNCTION
 *  ME_GetChannelClassification
 * DESCRIPTION
 *  Get the current classification of channels stored in ME.
 * PARAMETERS
 *  channelClass        [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_GetChannelClassification(BtChannelClass *channelClass)
{
    /* Lock the stack */
    OS_LockStack();

    OS_MemCopy(channelClass->map, MEC(channelClass.map), 10);

    /* Unlock the stack */
    OS_UnlockStack();

    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  ME_SetChannelClassification
 * DESCRIPTION
 *  Set the classification of channels for AFH.
 * PARAMETERS
 *  channelClass        [IN]        
 *  autoMode            [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_SetChannelClassification(BtChannelClass *channelClass, BOOL autoMode)
{
    BtStatus status = BT_STATUS_IN_PROGRESS;

    /* Lock the stack */
    OS_LockStack();

    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (MEC(channelOp).opType == BOP_NOP))
    {

        OS_MemCopy(MEC(channelClass.map), channelClass->map, 10);
        MEC(channelOp).opType = BOP_CHANNEL_CLASS;
        Assert(IsEntryAvailable(&(MEC(channelOp).node)));
        InsertTailList(&MEC(opList), &(MEC(channelOp).node));

        if ((MEC(autoMode) == 0 && autoMode == TRUE) ||
            (MEC(autoMode) == 1 && autoMode == FALSE) || (MEC(autoMode) == 0xff))
        {
            MEC(autoMode) = (U8) ((autoMode == TRUE) ? 1 : 0);
            /* Schedule the operation */
            MEC(autoMode) |= 0x80;
        }
        else
        {
            /* Make sure the operation is not scheduled */
            MEC(autoMode) &= 0x7f;
        }

        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
        status = BT_STATUS_SUCCESS;
    }

    /* Unlock the stack */
    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  ME_MarkWiFiChannel
 * DESCRIPTION
 *  Translate WiFi channel to BT channels for AFH.
 * PARAMETERS
 *  channelClass        [IN]        
 *  wifiChannel         [IN]        
 *  state               [IN]        
 *  initialize          [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_MarkWiFiChannel(BtChannelClass *channelClass, U8 wifiChannel, BOOL state, BOOL initialize)
{
    U8 i, count;

    if (wifiChannel < 1 || wifiChannel > 14)
    {
        return BT_STATUS_INVALID_PARM;
    }

    /* Calculate the 1st bit and count to be set in the map */
    i = (U8) ((wifiChannel - 1) * 5);
    count = (U8) min(79 - i, 25);
    return ME_MarkAfhChannels(channelClass, i, count, state, initialize);
}


/*****************************************************************************
 * FUNCTION
 *  ME_MarkAfhChannels
 * DESCRIPTION
 *  Mark a range of BT channels for AFH.
 * PARAMETERS
 *  channelClass        [IN]        
 *  begin               [IN]        
 *  count               [IN]        
 *  state               [IN]        
 *  initialize          [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_MarkAfhChannels(BtChannelClass *channelClass, U8 begin, U8 count, BOOL state, BOOL initialize)
{
    U8 *map = channelClass->map;
    U8 bit;

    if (begin > 78 || (begin + count) > 79)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if (initialize)
    {
        OS_MemSet(map, 0xff, 10);
    }

    /* Initialize only? */
    if (count == 0)
    {
        /* Yes */
        goto done;
    }

    /* Adjust count if too long */
    count = (U8) min(count, (79 - begin));

    /* Set the first bits in the map */
    map = channelClass->map + (begin / 8);
    begin %= 8;
    if (begin != 0)
    {
        bit = 8;
        if (count < (8 - begin))
        {
            bit = (U8) (8 - begin);
        }
        *map = (U8) MeSetBits(*map, bit, min((U8) (8 - begin), count), state);
        map++;
        if (count <= (8 - begin))
        {
            /* The bits were all in this byte */
            goto done;
        }
        count = (U8) (count - (8 - begin));
    }

    /* Set the middle bits of the map */
    if (count > 8)
    {
        OS_MemSet(map, (U8) (state ? 0xff : 0x00), count / 8);
    }

    /* Set the last bits in the map */
    map += (count / 8);
    begin = (U8) (count % 8);
    if (begin != 0)
    {
        *map = (U8) MeSetBits(*map, begin, begin, state);
    }

  done:
    /* Make sure the MSB is 0 */
    channelClass->map[9] &= 0x7f;

    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  ME_GetHciConnectionHandle
 * DESCRIPTION
 *  Get the HCI connection handle.
 * PARAMETERS
 *  rm      [IN]        
 * RETURNS
 *  The handle.
 *****************************************************************************/
U16 ME_GetHciConnectionHandle(BtRemoteDevice *rm)
{
    U16 handle;

    Assert(rm != 0);

    /* Lock the stack */
    OS_LockStack();

    handle = rm->hciHandle;

    /* Unlock the stack */
    OS_UnlockStack();

    return handle;
}


/*****************************************************************************
 * FUNCTION
 *  ME_GetBdAddr
 * DESCRIPTION
 *  Get the BdAddr.
 * PARAMETERS
 *  rm          [IN]        
 *  bdAddr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ME_GetBdAddr(BtRemoteDevice *rm, BD_ADDR *bdAddr)
{
    Assert(rm != 0);
    Assert(bdAddr != 0);

    /* Lock the stack */
    OS_LockStack();

    *bdAddr = rm->bdAddr;

    /* Unlock the stack */
    OS_UnlockStack();
}


/*****************************************************************************
 * FUNCTION
 *  ME_SetDefaultLinkPolicy
 * DESCRIPTION
 *  Set the default link policy setting.
 * PARAMETERS
 *  inACL       [IN]        
 *  outACL      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus ME_SetDefaultLinkPolicy(BtLinkPolicy inACL, BtLinkPolicy outACL)
{
    OS_Report("ME_SetDefaultLinkPolicy : inAcl=%d, outACL=%d", inACL, outACL);
    if ((inACL & BLP_MASK) || (outACL & BLP_MASK))
    {
        return BT_STATUS_FAILED;
    }

    /* Lock the stack */
    OS_LockStack();
    OS_Report("ME_SetDefaultLinkPolicy : btver=%u, saveInAcl=%d, inACL=%d", MEC(btVersion), MEC(inAclPolicy), inACL);
    if ((MEC(btVersion) >= 2) && (MEC(inAclPolicy) != inACL))
    {
    MEC(inAclPolicy) = inACL;
    MEC(outAclPolicy) = outACL;
        MeWriteDefaultLinkPolicy();
    }

    /* Unlock the stack */
    OS_UnlockStack();

    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  ME_GetStackInitState
 * DESCRIPTION
 *  Gets the stack/radio initialization state.
 * PARAMETERS
 *  void
 * RETURNS
 *  BtStackState
 *****************************************************************************/
BtStackState ME_GetStackInitState(void)
{
    return MEC(stackState);
}


/*****************************************************************************
 * FUNCTION
 *  ME_GetBtVersion
 * DESCRIPTION
 *  Gets the Bluetooth version of the local radio.
 * PARAMETERS
 *  void
 * RETURNS
 *  Version number
 *****************************************************************************/
U8 ME_GetBtVersion(void)
{
    return MEC(btVersion);
}


/*****************************************************************************
 * FUNCTION
 *  ME_GetBtFeatures
 * DESCRIPTION
 *  Gets the value of the specified byte of the Bluetooth features
 *  bitmask from the local radio.
 * PARAMETERS
 *  byte        [IN]        
 * RETURNS
 *  Value of the specified byte of the BT features bitmask.
 *****************************************************************************/
U8 ME_GetBtFeatures(U8 byte)
{
    if (byte > 7)
    {
        return 0;
    }

    return MEC(btFeatures)[byte];
}

/*---------------------------------------------------------------------------
 *            ME_HasPendingTask()
 *---------------------------------------------------------------------------
 *
 *   Determines if operations are pending.
 *
 * Return:    Pending HCI event associated with the current HCI command
 *            or 0 for none.
 */


/*****************************************************************************
 * FUNCTION
 *  ME_HasPendingTask
 * DESCRIPTION
 *  Determines if operations are pending.
 * PARAMETERS
 *  void
 * RETURNS
 *  Pending HCI event associated with the current HCI command
 *  or 0 for none.
 *****************************************************************************/
U16 ME_HasPendingTask(void)
{
    if (MEC(curTask) == 0)
    {
        return 0;
    }
    else
    {
        return 0xFF00;
    }
}


/*****************************************************************************
 * FUNCTION
 *  ME_RadioShutdownTime
 * DESCRIPTION
 *  Gracefully shuts down the radio module, including the
 *  transports and the HCI.
 * PARAMETERS
 *  time        [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS - The radio is down.  The application will be notified
 *  during the call that the shutdown was complete with
 *  BTEVENT_HCI_DEINITIALIZED.
 *  BT_STATUS_FAILED - The radio is already down.
 *  BT_STATUS_PENDING - The radio will be shut down.  The application will
 *  be notified when shutdown is complete with BTEVENT_HCI_DEINITIALIZED.
 *****************************************************************************/
BtStatus ME_RadioShutdownTime(TimeT time)
{
    U16 event;

    /* Stop operation processing. */
    MEC(stackState) = BTSS_DEINITIALIZE;

    /* Check for pending operations. */
    if ((time > 0) && (event = ME_HasPendingTask()) != 0)
    {
        bt_trace(TRACE_GROUP_1, RADIO_WAITING_FOR_EVENT_xX, event);
        MEC(pendingEvent) = event;
        MEC(timer).context = (void*)(time / 50);
        MEC(timer).func = MePendingCommandHandler;
        EVM_StartTimer(&MEC(timer), 50);
        return BT_STATUS_PENDING;
    }

    /* Now we are ready to take down HCI and transports. */
    return RMGR_RadioShutdown();
}

//#if (SPP_SCO_SUPPORT == XA_ENABLED) || (BT_READ_RSSI == XA_ENABLED)


/*****************************************************************************
 * FUNCTION
 *  ME_ReadRSSI
 * DESCRIPTION
 *  Put the remote device into sniff mode.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_ReadRSSI(BtRemoteDevice *remDev)
{
    BtStatus status;

    if (remDev->state != BDS_CONNECTED)
    {
        return BT_STATUS_FAILED;
    }
    remDev->readRSSIOp.opType = BOP_READ_RSSI;

    Assert(IsEntryAvailable(&(remDev->readRSSIOp.node)));
    InsertTailList(&MEC(opList), &((remDev)->readRSSIOp.node));
    MeStartOperation();
    status = BT_STATUS_PENDING;

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  ME_ReadTxPower
 * DESCRIPTION
 *  Put the remote device into sniff mode.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  status of operation.
 *****************************************************************************/
BtStatus ME_ReadTxPower(BtRemoteDevice *remDev)
{
    BtStatus status;

    if (remDev->state != BDS_CONNECTED)
    {
        return BT_STATUS_FAILED;
    }
    Report(("ME_ReadTxPower"));
    remDev->readTxPowerOp.opType = BOP_READ_TX_POWER;

    Assert(IsEntryAvailable(&(remDev->readTxPowerOp.node)));
    InsertTailList(&MEC(opList), &((remDev)->readTxPowerOp.node));
    MeStartOperation();
    status = BT_STATUS_PENDING;

    return status;
}

//#endif /* (SPP_SCO_SUPPORT == XA_ENABLED) || (BT_READ_RSSI == XA_ENABLED) */



BtStatus ME_ReadWifiRSSI(BtRemoteDevice *remDev)
{
    BtStatus status;

    if (remDev->state != BDS_CONNECTED)
    {
        return BT_STATUS_FAILED;
    }
    remDev->readWIFIRSSIOp.opType = BOP_READ_WIFI_RSSI;

    Assert(IsEntryAvailable(&(remDev->readWIFIRSSIOp.node)));
    InsertTailList(&MEC(opList), &((remDev)->readWIFIRSSIOp.node));
    MeStartOperation();
    status = BT_STATUS_PENDING;

    return status;
    
}




BtStatus ME_SetRangeRequest(U8 high_range, U8 low_range)
{
    BtStatus status = BT_STATUS_FAILED;

    MEC(setWifiRxRangeHigh) = high_range;
    MEC(setWifiRxRangeLow) = low_range;
    
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && 
        (MEC(setRxRangeOp).opType == BOP_NOP))
    {
        if(!IsEntryAvailable(&(MEC(setRxRangeOp).node)))
        {
            Assert(0);
        }
        MEC(setRxRangeOp).opType = BOP_SET_RX_RANGE;
        InsertTailList(&MEC(opList), &(MEC(setRxRangeOp).node));
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }

    return status;
    
}

#ifdef __BT_SINGLE_ANTENNA__
BtStatus ME_SetDefaultPower(U8 ucPower)
{
    BtStatus status = BT_STATUS_FAILED;

    MEC(setWifiTxDefaultPowerConHandle) = 0;
    MEC(ucPower) = ucPower;

   
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && 
        (MEC(setTxDefaultPowerOp).opType == BOP_NOP))
    {
        if(!IsEntryAvailable(&(MEC(setTxDefaultPowerOp).node)))
        {
            Assert(0);
        }
        MEC(setTxDefaultPowerOp).opType = BOP_SET_TX_DEFAULT_POWER;
        InsertTailList(&MEC(opList), &(MEC(setTxDefaultPowerOp).node));
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }

    return status;
    
}


BtStatus ME_SetUpdatePower(U32 con_hdl, U8 ucPower)
{
    BtStatus status = BT_STATUS_FAILED;

    MEC(setWifiTxDefaultPowerConHandle) = con_hdl;
    MEC(ucPower) = ucPower;
   
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && 
        (MEC(setTxUpdatePowerOp).opType == BOP_NOP))
    {
        if(!IsEntryAvailable(&(MEC(setTxUpdatePowerOp).node)))
        {
            Assert(0);
        }
        MEC(setTxUpdatePowerOp).opType = BOP_SET_TX_UPDATE_POWER;
        InsertTailList(&MEC(opList), &(MEC(setTxUpdatePowerOp).node));
        MeStartOperation();
        status = BT_STATUS_PENDING;
    }

    return status;
    
}


#endif

/*****************************************************************************
 * FUNCTION
 *  Me_ChangeLinkPolicyRoleSwitch
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev          [IN]        
 *  disabled        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void Me_ChangeLinkPolicy(BtRemoteDevice *remDev, BtEnableOption roleSwitch, BtEnableOption sniff)
{
#ifdef __BT_LE_STANDALONE__
    return;
#else
    BtLinkPolicy old;
#ifdef __BT_4_0_BLE__
    BtDeviceContext *bdc;
#endif
    /* It might be called when pairing (MODE 3). Just return and leave it after connection complete */
    if (remDev == NULL || ME_GetRemDevState(remDev) != BDS_CONNECTED){
        return;
    }

#ifdef __BT_4_0_BLE__
    if ((bdc = DS_FindDevice(&remDev->bdAddr)) != NULL && bdc->devType == BT_DEV_TYPE_LE)
    {
        return;
    }
#endif

    bt_trace(TRACE_GROUP_1,BTLOG_ME_CHANGELINKPOLICYROLESWITCHxD, remDev->LinkPolicyParms, roleSwitch, sniff);
    old = remDev->LinkPolicyParms;
    
    switch (roleSwitch)
    {
    case BT_OP_DISABLE:
        remDev->LinkPolicyParms &= ~BLP_MASTER_SLAVE_SWITCH;
        break;
    case BT_OP_ENABLE:
        remDev->LinkPolicyParms |= BLP_MASTER_SLAVE_SWITCH;
        break;
    default:
        break;
    }

    switch (sniff)
    {
    case BT_OP_DISABLE:
        remDev->LinkPolicyParms &= ~BLP_SNIFF_MODE;
        break;
    case BT_OP_ENABLE:
        remDev->LinkPolicyParms |= BLP_SNIFF_MODE;
        break;
    default:
        break;
    }

    if (old != remDev->LinkPolicyParms)
    {
        if (!IsEntryAvailable(&(remDev->policyOp.node)))
        {
            bt_trace(TRACE_GROUP_1,BTLOG_CAN_NOT_PERFORM_LINK_POLICY);
            return;
        }
        else
        {
            remDev->policyOp.opType = BOP_WRITE_LINK_POLICY;
            InsertHeadList(&MEC(opList), &(remDev->policyOp.node));
            MeStartOperation();
        }
    }
#endif
}

/*****************************************************************************
 * FUNCTION
 *  ME_ReadManufactureName
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev          [IN]
 * RETURNS
 *  U16               Manufacture ID, otherwise 0xFFFF.
 *****************************************************************************/
U16 ME_ReadManufactureName(BtRemoteDevice *remDev)
{
    U16 id = 0xFFFF;
    OS_Report("ME_ReadManufactureName(%p)", remDev);
    if(remDev){
        
        if(remDev->state >= BDS_CONNECTED){
            return LEtoHost16(&remDev->remoteVersion[1]);
        }else{
            OS_Report("remote device state=%u", remDev->state);
        }
    }
    return id;
}

/*****************************************************************************
 * FUNCTION
 *  ME_IsMtkBTChip
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev          [IN]
 * RETURNS
 *  U16               0 : No, otherwise Yes.
 *****************************************************************************/
U32 ME_IsMtkBTChip(BtRemoteDevice *remDev)
{
    U32 ret = ME_ReadManufactureName(remDev);
    OS_Report("ME_IsMtkBTChip=%u", ret);
    return (ret==0x46) ? 1 : 0;
}

#ifdef __BT_2_1_SIMPLE_PAIRING__
/*****************************************************************************
 * FUNCTION
 *  ME_IsSSPSupported
 * DESCRIPTION
 *  Check if SSP is supported by remote
 * PARAMETERS
 *  remDev          [IN]
 * RETURNS
 *  BtStatus
 *      BT_STATUS_SUCCESS : Yes
 *      BT_STATUS_NOT_SUPPORTED : No
 *      BT_STATUS_PENDING : Not get yet
 *****************************************************************************/
BtStatus ME_IsSSPSupported(BtRemoteDevice *remDev){
    BtStatus status;
    if(!OS_TestBit(BT_REMOTE_LINK_INFO_FEATURE, remDev->remoteLinkInfo) ||
        (OS_TestBit(BDF_EXTENDED_FEATURE, remDev->remoteFeature) && 
        !OS_TestBit(BT_REMOTE_LINK_INFO_EXT_FEATURE, remDev->remoteLinkInfo))){
        status = BT_STATUS_PENDING;
    }else{
        if(OS_TestBit(BT_REMOTE_LINK_INFO_EXT_FEATURE, remDev->remoteLinkInfo) && 
            OS_TestBit(BDEF_SSP_HOST, remDev->remoteExtFeature)){
            status = BT_STATUS_SUCCESS;
        }else{
            status = BT_STATUS_NOT_SUPPORTED;
        }
    }
    OS_Report("ME_IsSSPSupported : %d", status);
    return status;
}
#endif

BtStatus ME_CancelBonding(BtRemoteDevice *remDev){
    BtStatus status = BT_STATUS_SUCCESS;
    OS_Report("ME_CancelBonding : authState == %u", remDev->authState);
    switch(remDev->authState){
    case BAS_START_AUTHENTICATE:
    case BAS_WAITING_KEY_REQ:
    case BAS_SENDING_KEY:
        OS_Report("ME_CancelBonding : do nothing. Wait for ACL disconnect");
        break;
    case BAS_WAITING_FOR_PIN:
    case BAS_WAITING_FOR_PIN_R:
        OS_Report("[ERR] ME_CancelBonding : Cancel bond when waiting for pin code. MMI shall send 0 length pin to cancel bond.");
        status = BT_STATUS_FAILED;
        /* BAS_WAITING_FOR_KEY is not set. Currently call report auth directly. */
        //break;    
    case BAS_WAITING_FOR_KEY:
    case BAS_WAITING_FOR_KEY_R:
        MEC(btEvent).errCode = BEC_USER_TERMINATED;
        SecReportAuthenticateResult(remDev);
        /* mark state as cancel */
        remDev->authState = BAS_CANCEL_AUTHENTICATE;
        break;
    case BAS_NOT_AUTHENTICATED:
    case BAS_AUTHENTICATED:
        status = BT_STATUS_FAILED;
        break;
    default:
        OS_Report("[ERR] ME_CancelBonding : unknown authState");
        status = BT_STATUS_FAILED;
        break;
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  ME_IsEnhancedAudioSupported
 * DESCRIPTION
 *  Check if enhanced audio is supported by chip
 * PARAMETERS
 *  NULL
 * RETURNS
 *  BtStatus
 *      BT_STATUS_SUCCESS : Yes
 *      BT_STATUS_NOT_SUPPORTED : No
 *****************************************************************************/
BtStatus ME_IsEnhancedAudioSupported(){
    BtStatus status;
    if(OS_TestBit(29 * 8 + 3, MEC(btCommands))){
        status = BT_STATUS_SUCCESS;
    }else{
        status = BT_STATUS_NOT_SUPPORTED;
    }
    OS_Report("ME_IsEnhancedAudioSupported : %d", status);
    return status;
}
