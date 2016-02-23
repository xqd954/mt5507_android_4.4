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
 *     $Workfile:mecon.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This is file contains the code for managing ACL connections. 
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
#include "sys/hci.h"
#include "utils.h"
#include "btutils.h"
#include "l2cap.h"
#include "ddb.h"
#include "bt_feature.h"
#include "x_file.h"
#include "meamp.h"
#include "mei.h"
#ifdef __BT_SINGLE_ANTENNA__
#include "bt_wmt_api.h"
#endif
#include "stdlib.h"

/****************************************************************************
 *
 * Macros
 *
 ****************************************************************************/

/* Macro for accessing a RemoteDevice structure given the curOp */
#define RDEV ((BtRemoteDevice*)MEC(curOp))
#define REJECT ((BtConRejectOp*)MEC(curOp))
#define METOKEN ((MeCommandToken*)MEC(curOp))

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/
void MeHandleAclConComplete(BtRemoteDevice *remDev);
void MeHandleSpecialCreateLinkProc(EvmTimer *Timer);
void MeHandleConnectLocalReport(BtRemoteDevice *remDev, U8 error_code);
BOOL MeCheckDeviceStateNotIdle(U8 *bd_addr);

static void StartSniffModeTask(void);
static void StartExitSniffModeTask(void);
static void StartExitParkModeTask(void);
static void StartSwitchRoleTask(void);
static void ReportModeChange(BtRemoteDevice *remDev);

//static void ReportRoleChange(BtRemoteDevice* remDev);
//2006-0911 : both BOP_DISCOVER_ROLE and BOP_SWITCH_ROLE operations use the same callback function
static void ReportRoleChange(BtRemoteDevice *remDev, BtOpType opType);

static void StartRemNameReqTask(void);
static void StartLinkRejectTask(U8 *bdAddr, U8 rejectReason);
static void StartLinkDisconnectTask(void);
void CallAndRemoveHandlers(BtRemoteDevice *remDev);
static void StartLinkAcceptTask(void);
static void StartLinkConnectTask(void);
void MeFreeRemoteDevice(BtRemoteDevice *remDev);
static void MeFailPendingRemDevCommands(BtRemoteDevice *remDev);
static BOOL MeFailRemDevCommand(BtRemoteDevice *remDev, BtOpType opType, void *parm);
static void MeLinkReadyToSend(EvmTimer *Timer);

#if !defined (__BT_CHIP_VERSION_0__)
static void MeHandleAclLinkChangePacketType(EvmTimer *Timer);
#endif 
static void MeHandleDisconnectACLCheckCreateACL(EvmTimer *Timer);
static void MeConHandleErrorTaskEnd(void);
static void MeConVerifyHCIGenCommand(void);
static void MeConVerifyRemDevOpOnList(BtRemoteDevice* remDev);

extern void BTBMLocalWriteScanEnabled(U8 mode);
extern BOOL BTBMCheckSpecialSlaveProcedure(void);
extern BtStatus BTBMDoRoleSwitch(BtRemoteDevice *link);
extern BOOL btbmCheckBondingState(void);
extern BOOL BTBMCheckAclConnectRequestNotAddPendingCon(void);
extern U8 btbmUtilGetNameTokenID(MeCommandToken *nameToken);

#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
BtStatus ME_StartWriteEIR(void);
#endif 

#if defined (__BT_2_1_AUTO_FLUSH__)
extern void MeWriteAutomaticallyFlushedTimeout(BtRemoteDevice *remDev);
#endif 

extern void BTBMGetScattnetStatus(U8 *piconet, U8 *scatternet, U8 *sco, U8 *esco, U8 *msco, U8 *ssco);
extern U16 BT_XFile_EncryptionCommandOpcode(U16 opcode);
extern U8 Me_WriteAudioSetting(void);

/*
 * Memory Manager function prototypes and macros. These macros may be
 * replaced with dynamic functions as part of the porting effort if desired.
 */
U8 *MM_AllocBuffer(U32 Size, void *Context);
void MM_FreeBuffer(U8 *Buffer);

#define MM_AllocBuffer(_Size, _IdxPtr)  \
        (Assert(*(I8 *)(_IdxPtr) < NUM_BT_DEVICES), MEC(rxBuffTable)[*(I8 *)(_IdxPtr)])
#define MM_FreeBuffer(_Buff)


/*****************************************************************************
 * FUNCTION
 *  MeHandleLinkConnectOp
 * DESCRIPTION
 *  Handle the link connection operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleLinkConnectOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /*
             * For now do not check if connections already exist. In the future
             * we may need to put existing connections on hold but for now we 
             * will not
             */
            StartLinkConnectTask();
            return;

        case BOE_TASK_END:
            if (MEC(btEvent).errCode != 0)
            {
                U8 param[10];
                U8 reqPending = RDEV->linkReqPending;
                
                bt_trace(TRACE_GROUP_1, IN_MEHANDLELINKCONNECTOPMECPENDCONSxD, MEC(pendCons));
                if(reqPending){
                    BtCodRecord rec;
                    if(DDB_COD_FindRecord(&RDEV->bdAddr, &rec) == BT_STATUS_SUCCESS){
                        OS_MemCopy(param, RDEV->bdAddr.addr, 6);
                        param[6] = (U8)(rec.cod & 0xFF);
                        param[7] = (U8)((rec.cod>>8) & 0xFF);
                        param[8] = (U8)((rec.cod>>16) & 0xFF);
                    }else{
                        OS_Report("Can not fin COD record. DIscard pending request");
                        reqPending = 0;
                    }
                    param[9] = BLT_ACL;
                }
                RDEV->linkReqPending = 0;
                /* We got an error so report it */
                MeReportLinkUp(RDEV, BTEVENT_LINK_CONNECT_CNF);
                if(MEC(pendCons) == 0)
                {
                    Assert(0);
                }
                MEC(pendCons) -= 1;
                if(reqPending){
                    OS_Report("Rehandle link connect request");
                    MeHandleLinkConnectReq(sizeof(param), param);
                }
            }
            break;
        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleLinkAcceptOp
 * DESCRIPTION
 *  Handle the link accept operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleLinkAcceptOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            StartLinkAcceptTask();
            return;

        case BOE_TASK_END:
            if (MEC(btEvent).errCode != 0)
            {
                bt_trace(TRACE_GROUP_1, IN_MEHANDLELINKACCEPTOPMECPENDCONSxD, MEC(pendCons));
                bt_trace(TRACE_GROUP_1, MECONLINKACCEPTERROR__xX, MEC(btEvent).errCode);
                MeReportLinkUp(RDEV, BTEVENT_LINK_CONNECT_IND);
                if (MEC(pendCons) > 0)
                {
                    MEC(pendCons) -= 1;
                }
            }
            break;

        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleLinkRejectOp
 * DESCRIPTION
 *  Handle the link reject operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleLinkRejectOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            if (MEC(curOp)->opType == BOP_LINK_IN_REJECT)
            {
                StartLinkRejectTask(REJECT->bdAddr.addr, REJECT->rejectReason);
            }
            else
            {
                StartLinkRejectTask(RDEV->bdAddr.addr, RDEV->parms.rejectReason);
            }
            return;

        case BOE_TASK_END:
            if (MEC(btEvent).errCode != 0)
            {
                if (MEC(curOp)->opType == BOP_LINK_REJECT)
                {
                    /* We received an error so report it */
                    bt_trace(TRACE_GROUP_1, MECONLINKREJECTERROR__xX, MEC(btEvent).errCode);
                    bt_trace(TRACE_GROUP_1, IN_MEHANDLELINKREJECTOPMECPENDCONSxD, MEC(pendCons));
                    MeReportLinkUp(RDEV, BTEVENT_LINK_CONNECT_IND);
                    if(MEC(pendCons) == 0)
                    {
                        Assert(0);
                    }
                    MEC(pendCons) -= 1;
                    bt_trace(TRACE_GROUP_1, IN_MEHANDLELINKREJECTOP2ND_MECPENDCONSxD, MEC(pendCons));
                }
                else
                {
                    bt_trace(TRACE_GROUP_1, IN_MEHANDLELINKREJECTOPIN_REJECTMECPENDCONSxD, MEC(pendCons));
                    bt_trace(TRACE_GROUP_1, MECON_INTERNAL_LINK_REJECT__xX, MEC(btEvent).errCode);
                }
            }
            break;

        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleLinkDisconnectOp
 * DESCRIPTION
 *  Handle the link disconnect operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleLinkDisconnectOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            StartLinkDisconnectTask();
            return;

        case BOE_TASK_END:
            bt_trace(TRACE_GROUP_1, MECON_MEHANDLELINKDISCONNECTOP_TASK_END_ERRCODE__xX, MEC(btEvent).errCode);
            /*
             * If we are disconnecting, advance the state of the disconnect to indicate
             * status received. We're still waiting for disconnect complete.
             */
            if (RDEV->state == BDS_OUT_DISC)
            {
                RDEV->state = BDS_OUT_DISC2;
            }

            /* If we haven't started another op, clear the op type */
            if (RDEV->op.opType == BOP_LINK_DISCONNECT)
            {
                RDEV->op.opType = BOP_NOP;
            }

            if (MEC(btEvent).errCode != 0)
            {
                /*
                 * This link could already be disconnected by the other side. In this case
                 * we will get an error indicating no connection.
                 */
                /* BEC_INVALID_HCI_PARM is handled special for MT6601 */
                if ((MEC(btEvent).errCode != BEC_NO_CONNECTION) && (MEC(btEvent).errCode != BEC_INVALID_HCI_PARM))
                {
                    /*
                     * We got some other error and must notify the global handlers. Depending on
                     * the error, they may need to force a disconnect to clear the link.
                     */
                    MEC(btEvent).eType = BTEVENT_LINK_DISCONNECT;
                    MEC(btEvent).p.remDev = RDEV;
                    MeReportResults(BEM_LINK_DISCONNECT);
                }
            }
            break;

        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleRemNameReqOp
 * DESCRIPTION
 *  Start an Inquiry process.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleRemNameReqOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Put the remote name event on the command queue */
            InsertHeadList(&MEC(cmdList), &(METOKEN->op.node));

            /* Start the inquiry task */
            StartRemNameReqTask();
            return;

        case BOE_TASK_END:
            if (MEC(btEvent).errCode != 0)
            {
                /* We got an error so report it */
                bt_trace(TRACE_GROUP_1, ME_REMOTE_NAME_REQUEST_OPERATION_ERROR_STATUS__xX, MEC(btEvent).errCode);
                MeReportMeCommandComplete(METOKEN);
            }
            break;

        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleSniffModeOp
 * DESCRIPTION
 *  Handle the sniff mode operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleSniffModeOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            bt_trace(TRACE_GROUP_1, SNIFF_REQ);
            StartSniffModeTask();
            return;

        case BOE_TASK_END:
            bt_trace(TRACE_GROUP_1, SNIFF_END);
            if (MEC(btEvent).errCode != 0)
            {
                /* We got an error so report it */
                bt_trace(TRACE_GROUP_1, ME_SNIFF_MODE_OPERATION_ERROR_STATUS__xX, MEC(btEvent).errCode);
                MEC(btEvent).p.modeChange.curMode = RDEV->mode;
                ReportModeChange(RDEV);
            }
            break;

        default:
            MeConHandleErrorTaskEnd();
            break;
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleExitSniffModeOp
 * DESCRIPTION
 *  Handle the exit sniff mode operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleExitSniffModeOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            StartExitSniffModeTask();
            return;

        case BOE_TASK_END:
            if (MEC(btEvent).errCode != 0)
            {
                /* We got an error so report it */
                bt_trace(TRACE_GROUP_1, ME_EXIT_SNIFF_MODE_OPERATION_ERROR_STATUS__xX, MEC(btEvent).errCode);
                MEC(btEvent).p.modeChange.curMode = RDEV->mode;
                ReportModeChange(RDEV);
            }
            break;

        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleExitParkModeOp
 * DESCRIPTION
 *  Handle the exit park mode operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleExitParkModeOp(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            StartExitParkModeTask();
            return;

        case BOE_TASK_END:
            if (MEC(btEvent).errCode != 0)
            {
                /* We got an error so report it */
                bt_trace(TRACE_GROUP_1, ME_EXIT_PARK_MODE_OPERATION_ERROR_STATUS__xX, MEC(btEvent).errCode);
                MEC(btEvent).p.modeChange.curMode = RDEV->mode;
                ReportModeChange(RDEV);
            }
            break;

        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleQoSSetupOp
 * DESCRIPTION
 *  Handle the QoS setup operation.
 * PARAMETERS
 *  event       [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleQoSSetupOp(BtOpEvent event)
{
    BtRemoteDevice *remDev;

    /* Get a pointer to the remote device */
    remDev = ContainingRecord(RDEV, BtRemoteDevice, qosOp);
    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleConnectionStatusOnly;
            MEC(taskEvent) = HCE_COMMAND_STATUS;
        
            bt_trace(TRACE_GROUP_1, MECON_QOS_SETUP, remDev->hciHandle, 
                remDev->qos.flags, remDev->qos.serviceType, remDev->qos.tokenRate, 
                remDev->qos.peakBandwidth, remDev->qos.latency, remDev->qos.delayVar);
            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle);
            MEC(hciGenCmd).parms[2] = remDev->qos.flags;
            MEC(hciGenCmd).parms[3] = remDev->qos.serviceType;
            StoreLE32(MEC(hciGenCmd).parms + 4, remDev->qos.tokenRate);
            StoreLE32(MEC(hciGenCmd).parms + 8, remDev->qos.peakBandwidth);
            StoreLE32(MEC(hciGenCmd).parms + 12, remDev->qos.latency);
            StoreLE32(MEC(hciGenCmd).parms + 16, remDev->qos.delayVar);
            MeConVerifyHCIGenCommand();
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 20);
            MeSendHciCommand(HCC_QOS_SETUP, 20);
            return;

        case BOE_TASK_END:
            if (MEC(btEvent).errCode != 0)
            {
                /* We got an error so report it */
                bt_trace(TRACE_GROUP_1, MECON_QOS_SETUP_ERROR_STATUS__xX, MEC(btEvent).errCode);
            }
            break;

        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}

/*****************************************************************************
 * FUNCTION
 *  MeHandleSwitchRoleOp
 * DESCRIPTION
 *  Handle the switch role operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleSwitchRoleOp(BtOpEvent event)
{
    U32 report = 0;
    BtRemoteDevice *remDev = RDEV;
    switch (event)
    {
        case BOE_START:
            if(RDEV->role != RDEV->req_role){
                StartSwitchRoleTask();
                return;
            }else{
                OS_Report("MeHandleSwitchRoleOp : reqRole(%u) == role(%u). Just report role change", RDEV->req_role, RDEV->role);
                MEC(btEvent).errCode = HC_STATUS_ROLE_CHANGE_DISALLOWED;
                report = 1;
                break;
            }
        case BOE_TASK_END:
            bt_trace(TRACE_GROUP_1, BTLOG_ME_SWITCH_ROLE_OPERATION_ENDED_STATUS, MEC(btEvent).errCode);
            if (MEC(btEvent).errCode != 0){
                if(RDEV->req_role == RDEV->role){
                    /*  if reqRole is equal to role, that means remote successfully 
                         change the role after command is sent to FW.
                    */
                    MEC(btEvent).errCode = 0;
                }                
                report = 1;
            }
            break;

        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

    /* Move role change report after operation end, so that we can judge 
        if the role change is from remote or local generate report 
    */
    if (report)
    {
        /* We got an error so report it */
        MEC(btEvent).p.roleChange.newRole = remDev->role;
        ReportRoleChange(remDev, BOP_SWITCH_ROLE);
    }
}



/*****************************************************************************
 * FUNCTION
 *  MeHandleReadRSSIOp
 * DESCRIPTION
 *  Handle the read RSSI operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleReadRSSIOp(BtOpEvent event)
{

    BtRemoteDevice *remDev = 0;
    U8 *parms;
    U8 i = 0;

    switch (event)
    {
        case BOE_START:
            remDev = ContainingRecord(RDEV, BtRemoteDevice, readRSSIOp);
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle);
            bt_trace(TRACE_GROUP_1, ME_START_RSSIOP, remDev->hciHandle);
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
            MeSendHciCommand(0xfc61, 2);
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            /* bt_trace(TRACE_GROUP_1,BTLOG_ERR_OR_END_MEHANDLEREADRSSIOP); */
            bt_trace(TRACE_GROUP_1, ME_END_RSSIOP);
          
            parms = MEC(btEvent).raw_data;
            for (i = 0; i < NUM_BT_DEVICES; i++)
            {
                remDev = &(MEC(devTable)[i]);
                if ((remDev->state >= BDS_OUT_DISC) || (remDev->state == BDS_CONNECTED))
                {
                    if (remDev->hciHandle == LEtoHost16(parms + 4))
                    {
                        break;
                    }
                }
            }
            if (i != NUM_BT_DEVICES)
            {
                MEC(btEvent).errCode = parms[3];
                MEC(btEvent).p.rssi.remDev = remDev;
                MEC(btEvent).p.rssi.RSSI = parms[6];
                MEC(btEvent).eType = BTEVENT_READ_RSSI_COMPLETED;
                MeReportResults(BEM_ALL_EVENTS);
            }
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleReadTxPowerOp
 * DESCRIPTION
 *  Handle the read RSSI operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleReadTxPowerOp(BtOpEvent event)
{
    BtRemoteDevice *remDev = 0;
    U8 *parms;
    U8 i = 0;

    switch (event)
    {
        case BOE_START:
            remDev = ContainingRecord(RDEV, BtRemoteDevice, readTxPowerOp);
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle);
            MEC(hciGenCmd).parms[2] = 0x00;
            Report(("remDev->hciHandle:%d",remDev->hciHandle));
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 3);
            MeSendHciCommand(HCC_READ_XMIT_POWER_LEVEL, 3);
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            /* bt_trace(TRACE_GROUP_1,BTLOG_ERR_OR_END_MEHANDLEREADRSSIOP); */
            Report(("Read Tx Power Result"));
          
            parms = MEC(btEvent).raw_data;
            for (i = 0; i < NUM_BT_DEVICES; i++)
            {
                remDev = &(MEC(devTable)[i]);
                if ((remDev->state >= BDS_OUT_DISC) || (remDev->state == BDS_CONNECTED))
                {
                    if (remDev->hciHandle == LEtoHost16(parms + 4))
                    {
                        break;
                    }
                }
            }
            if (i != NUM_BT_DEVICES)
            {
                MEC(btEvent).errCode = parms[3];
                MEC(btEvent).p.txPower.remDev = remDev;
                MEC(btEvent).p.txPower.txPower = parms[6];
                MEC(btEvent).eType = BTEVENT_READ_TX_POWER_COMPLETED;
                MeReportResults(BEM_ALL_EVENTS);
            }
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();

}



/*****************************************************************************
 * FUNCTION
 *  MeHandleReadRSSIOp
 * DESCRIPTION
 *  Handle the read RSSI operation.
 * PARAMETERS
 *  event       [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleReadWIFIRSSIOp(BtOpEvent event)
{
    BtRemoteDevice *remDev = 0;
    U8 *parms;
    U8 i = 0;

    switch (event)
    {
        case BOE_START:
            remDev = ContainingRecord(RDEV, BtRemoteDevice, readWIFIRSSIOp);
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle);
            bt_trace(TRACE_GROUP_1, ME_START_RSSIOP, remDev->hciHandle);
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
            MeSendHciCommand(0xfccb, 2);
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            /* bt_trace(TRACE_GROUP_1,BTLOG_ERR_OR_END_MEHANDLEREADRSSIOP); */
            bt_trace(TRACE_GROUP_1, ME_END_RSSIOP);
            parms = MEC(btEvent).raw_data;

            for (i = 0; i < NUM_BT_DEVICES; i++)
            {
                remDev = &(MEC(devTable)[i]);
                if ((remDev->state >= BDS_OUT_DISC) || (remDev->state == BDS_CONNECTED))
                {
                    if (remDev->hciHandle == LEtoHost16(parms + 4))
                    {
                        break;
                    }
                }
            }
            if (i != NUM_BT_DEVICES)
            {
#ifdef __BT_SINGLE_ANTENNA__
                BT_WMT_Send_RSSI_Indication((U8*)remDev->bdAddr.addr, parms[6], 1);
#endif
            }
            else
            {
#ifdef __BT_SINGLE_ANTENNA__
                BT_WMT_Send_RSSI_Indication((U8*)remDev->bdAddr.addr, 4, 0x00);
#endif
            }

            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleWriteLinkPolicyOp
 * DESCRIPTION
 *  Handle the write link policy operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleWriteLinkPolicyOp(BtOpEvent event)
{
    BtRemoteDevice *remDev;

    /* Get a pointer to the remote device */
    remDev = ContainingRecord(RDEV, BtRemoteDevice, policyOp);

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;

            bt_trace(TRACE_GROUP_1, MECON_WRITE_LINK_POLICY_TO_HCIHANDLE_xX, remDev->hciHandle);
            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle); /* Connection handle */
            StoreLE16(MEC(hciGenCmd).parms + 2, remDev->LinkPolicyParms);       /* Policy */
            MeConVerifyHCIGenCommand();
            bt_trace(TRACE_GROUP_1,BTLOG_THE_LINK_POLICYx04X , remDev->LinkPolicyParms);
            remDev->LinkPolicySetting = remDev->LinkPolicyParms;
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 4);
            MeSendHciCommand(HCC_WRITE_LINK_POLICY, 4);
            return;

        case BOE_TASK_END:
        case BOE_TASK_ERR:
            /* For now ignore the result of the link policy */
            bt_trace(TRACE_GROUP_1, BTLOG_ME_WRITE_LINK_POLICY_STATUS__xX, MEC(btEvent).errCode);
            bt_trace(TRACE_GROUP_1, BTLOG_LINK_POLICY_IS_DIFFERENTx04Xx04X , remDev->LinkPolicySetting, remDev->LinkPolicyParms);
            MeOperationEnd();
            if (remDev->LinkPolicySetting != remDev->LinkPolicyParms)
            {
                if (IsEntryAvailable(&(remDev->policyOp.node))){
                    remDev->policyOp.opType = BOP_WRITE_LINK_POLICY;
                    InsertHeadList(&MEC(opList), &(remDev->policyOp.node));
                    MeStartOperation();
                }
            }
            return;
        default:
            MeConHandleErrorTaskEnd();
    }
    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleChangePacketTypeOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleChangePacketTypeOp(BtOpEvent event)
{
    BtRemoteDevice *remDev;
    U16 packet_type;

    /* Get a pointer to the remote device */
    remDev = ContainingRecord(RDEV, BtRemoteDevice, packetTypeOp);

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;

            bt_trace(TRACE_GROUP_1, MECON_CHANGE_PACKET_TYPE_TO_HCIHANDLE_xX, remDev->hciHandle);
            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle); /* Connection handle */
            packet_type = BAPT_STANDARD;
            StoreLE16(MEC(hciGenCmd).parms + 2, packet_type);
            MeConVerifyHCIGenCommand();
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 4);
            MeSendHciCommand(HCC_CHNG_CONN_PACKET_TYPE, 4);
            return;

        case BOE_TASK_END:
        case BOE_TASK_ERR:
            /* For now ignore the result of the link policy */
            bt_trace(TRACE_GROUP_1, ME_CHANGE_PACKET_TYPE__xX, MEC(btEvent).errCode);
            remDev->packetTypeOp.opType = BOP_NOP;
            break;

        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleWriteSupervisionTimeoutOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleWriteSupervisionTimeoutOp(BtOpEvent event)
{
    BtRemoteDevice *remDev;
    U16 link_supervision_timeout = 0;

    /* Get a pointer to the remote device */
    remDev = ContainingRecord(RDEV, BtRemoteDevice, supervisionOp);
    #ifdef __BT_USE_CUST_DATA__
    link_supervision_timeout = (U32)btGetDevCustvalueById(remDev->devId, CUSTID_CONNECTION_TIMEOUT);
    #else /* __BT_USE_CUST_DATA__ */
    link_supervision_timeout = BT_LINK_SUPERVISION_TIMEOUT;
    #endif /* __BT_USE_CUST_DATA__ */
    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            bt_trace(TRACE_GROUP_1, LINK_SUPERVISION_HDLBT_DEBUGx02X, remDev->hciHandle);
            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle); /* Connection handle */
            StoreLE16(MEC(hciGenCmd).parms + 2, link_supervision_timeout);
            MeConVerifyHCIGenCommand();
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 4);
            MeSendHciCommand(HCC_WRITE_LINK_SUPERV_TIMEOUT, 4);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            /* For now ignore the result of the link policy */
            bt_trace(TRACE_GROUP_1, LINK_SUPERVISION_TIMEOUTBT_DEBUGx04X, MEC(btEvent).errCode);
            remDev->supervisionOp.opType = BOP_NOP;
            break;
        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}

#if defined (__BT_2_1_AUTO_FLUSH__)


/*****************************************************************************
 * FUNCTION
 *  MeHandleWriteAutomaticallyFlushOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleWriteAutomaticallyFlushOp(BtOpEvent event)
{
    BtRemoteDevice *remDev;
    U16 flush_timeout = 0;

    /* Get a pointer to the remote device */
    bt_trace(TRACE_GROUP_1, BTLOG_MEHANDLEWRITEAUTOMATICALLYFLUSHOP);
    remDev = ContainingRecord(RDEV, BtRemoteDevice, automaticallyFlushedOp);
    flush_timeout = BT_AUTOMATICALLY_FLUSHED_TIMEOUT;
    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            bt_trace(TRACE_GROUP_1, LINK_FLUSHED_TIMEOUT_DEBUGx02X, remDev->hciHandle);
            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle); /* Connection handle */
            StoreLE16(MEC(hciGenCmd).parms + 2, flush_timeout);
            MeConVerifyHCIGenCommand();
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 4);
            MeSendHciCommand(HCC_WRITE_AUTO_FLUSH_TIMEOUT, 4);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            bt_trace(TRACE_GROUP_1, LINK_FLUSHED_TIMEOUT_ERROR_CODE, MEC(btEvent).errCode);
            /* For now ignore the result of the link policy */
            remDev->automaticallyFlushedOp.opType = BOP_NOP;
            break;
        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}
#endif /* defined (__BT_2_1_AUTO_FLUSH__) */ 

#if defined (__BT_2_1_SNIFF_SUBRATING__)


/*****************************************************************************
 * FUNCTION
 *  MeHandleWriteSniffSubratingOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleWriteSniffSubratingOp(BtOpEvent event)
{
    BtRemoteDevice *remDev;

    /* Get a pointer to the remote device */
    bt_trace(TRACE_GROUP_1, BTLOG_MEHANDLEWRITESNIFFSUBRATINGOP);
    remDev = ContainingRecord(RDEV, BtRemoteDevice, sniffSubrateOp);
    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle); /* Connection handle */
            StoreLE16(MEC(hciGenCmd).parms + 2, remDev->sniffSubrateMaxLatency);
            StoreLE16(MEC(hciGenCmd).parms + 4, remDev->sniffSubrateMinRemoteTimeout);
            StoreLE16(MEC(hciGenCmd).parms + 6, remDev->sniffSubrateMinLocalTimeout);
            MeConVerifyHCIGenCommand();
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 8);
            MeSendHciCommand(HCC_SNIFF_SUBRATING, 8);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            bt_trace(TRACE_GROUP_1, LINK_SNIFF_SUBRATING_ERROR_CODE, MEC(btEvent).errCode);
            remDev->sniffSubrateOp.opType = BOP_NOP;
            remDev->sniffSubrateState &= (~MTK_SNIFF_SUBRATING_CMD_MAKE);
            break;
        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();
    if ((remDev->sniffSubrateState & MTK_SNIFF_SUBRATING_PARAM_CHANGED)
        && (ME_GetCurrentMode(remDev) == BLM_SNIFF_MODE))
    {
        ME_StartSniffSubrating(remDev);
    }

}
#endif /* defined (__BT_2_1_SNIFF_SUBRATING__) */ 


/*****************************************************************************
 * FUNCTION
 *  MeHandleReadRemoteFeatureTypeOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleReadRemoteFeatureTypeOp(BtOpEvent event)
{
    BtRemoteDevice *remDev;

    /* Get a pointer to the remote device */
    remDev = ContainingRecord(RDEV, BtRemoteDevice, remoteFeatureOp);
    OS_Report("MeHandleReadRemoteFeatureTypeOp : event=%u, remDev=%p, hciHandle=%u", event, remDev, remDev->hciHandle);
    switch (event)
    {
        case BOE_START:
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_TRY_TO_READ_REMOTE_LMP_FEATURE);
            OS_MemSet((U8*) & remDev->remoteFeature, 0, sizeof(remDev->remoteFeature));
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_STATUS;

            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle); /* Connection handle */
            MeConVerifyHCIGenCommand();
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
            MeSendHciCommand(HCC_READ_REMOTE_FEATURES, 2);
            return;

        case BOE_TASK_END:
            /* For now ignore the result of the link policy */
            remDev->remoteFeatureOp.opType = BOP_NOP;
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_TRY_TO_READ_REMOTE_LMP_FEATURE_EVENT_COMPLETED);
            break;
        case BOE_TASK_ERR:
            /* For now ignore the result of the link policy */
            remDev->remoteFeatureOp.opType = BOP_NOP;
            bt_trace(
                BT_TRACE_G1_PROTOCOL,
                BTLOG_TRY_TO_READ_REMOTE_LMP_FEATURE_ERRORBT_DEBUGx02X,
                MEC(btEvent).errCode);
            break;
        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}

/*****************************************************************************
 * FUNCTION
 *  MeHandleReadRemoteExtFeatureTypeOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleReadRemoteExtFeatureTypeOp(BtOpEvent event)
{
    BtRemoteDevice *remDev;

    /* Get a pointer to the remote device */
    remDev = ContainingRecord(RDEV, BtRemoteDevice, remoteExtFeatureOp);
    OS_Report("[ME] Handle read remote ext feature : %u", event);
    switch (event)
    {
        case BOE_START:
            OS_MemSet((U8*) & remDev->remoteExtFeature, 0, sizeof(remDev->remoteExtFeature));
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_STATUS;

            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle); /* Connection handle */
            MEC(hciGenCmd).parms[2] = 0x01;
            MeConVerifyHCIGenCommand();
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 3);
            MeSendHciCommand(HCC_READ_REMOTE_EXT_FEATURES, 3);
            return;

        case BOE_TASK_END:
            /* For now ignore the result of the link policy */
            remDev->remoteExtFeatureOp.opType = BOP_NOP;
            break;
        case BOE_TASK_ERR:
            /* For now ignore the result of the link policy */
            remDev->remoteExtFeatureOp.opType = BOP_NOP;
            OS_Report("[ME] Handle read remote ext feature failed : %u", MEC(btEvent).errCode);
            break;
        default:
            MeConHandleErrorTaskEnd();
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}

/*****************************************************************************
 * FUNCTION
 *  MeHandleReadRemoteVersionTypeOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleReadRemoteVersionTypeOp(BtOpEvent event)
{
    BtRemoteDevice *remDev;

    /* Get a pointer to the remote device */
    remDev = ContainingRecord(RDEV, BtRemoteDevice, remoteVersionOp);
    OS_Report("MeHandleReadRemoteVersionTypeOp : event=%u, remDev=%p, hciHandle=%u", event, remDev, remDev->hciHandle);
    switch (event)
    {
        case BOE_START:
            bt_trace(TRACE_GROUP_1, REMOTE_VERSION);
            OS_MemSet((U8*) & remDev->remoteVersion, 0, sizeof(remDev->remoteVersion));
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_STATUS;

            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle); /* Connection handle */
            MeConVerifyHCIGenCommand();
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
            MeSendHciCommand(HCC_READ_REMOTE_VERSION, 2);
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            /* For now ignore the result of the link policy */
            remDev->remoteVersionOp.opType = BOP_NOP;
            bt_trace(TRACE_GROUP_1, VERSION_COMPLETEDBT_DEBUGx02X, MEC(btEvent).errCode);
            break;
        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}

/*****************************************************************************
 * FUNCTION
 *  MeHandleReadRemoteClockOffsetTypeOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleReadRemoteClockOffsetTypeOp(BtOpEvent event)
{
    BtRemoteDevice *remDev;

    /* Get a pointer to the remote device */
    remDev = ContainingRecord(RDEV, BtRemoteDevice, remoteClockOffsetOp);

    switch (event)
    {
        case BOE_START:
            OS_Report("Read remote clock offset");
            OS_MemSet((U8*) & remDev->remoteClockOffsetOp, 0, sizeof(remDev->remoteClockOffsetOp));
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_STATUS;

            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle); /* Connection handle */
            MeConVerifyHCIGenCommand();
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
            MeSendHciCommand(HCC_READ_CLOCK_OFFSET, 2);
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            /* For now ignore the result of the link policy */
            remDev->remoteClockOffsetOp.opType = BOP_NOP;
            OS_Report("Read remote clock offset failed : %u", MEC(btEvent).errCode);
            break;
        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}

/*****************************************************************************
 * FUNCTION
 *  MeHandleCancelConnectOp
 * DESCRIPTION
 *  Handle the Cancel Connect operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleCancelConnectOp(BtOpEvent event)
{
    BtRemoteDevice *remDev;

    /* Get a pointer to the remote device */
    remDev = ContainingRecord(RDEV, BtRemoteDevice, cancelOp);

    switch (event)
    {
        case BOE_START:
            bt_trace(TRACE_GROUP_1, MECON_STARTING_CREATE_LINK_CANCEL_COMMAND);
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            
            OS_MemCopy(MEC(hciGenCmd).parms, remDev->bdAddr.addr, 6);   /* BD_ADDR */
            
            MeConVerifyHCIGenCommand();
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 6);
            MeSendHciCommand(HCC_CREATE_CONNECTION_CANCEL, 6);
            return;

        case BOE_TASK_END:
        case BOE_TASK_ERR:
            bt_trace(TRACE_GROUP_1, ME_CREATE_LINK_CANCEL_OPERATION_COMPLETE_STATUS__xX, MEC(btEvent).errCode);
            MEC(btEvent).eType = BTEVENT_LINK_CREATE_CANCEL;
            MEC(btEvent).p.remDev = remDev;
            MeReportNMResults();
            break;

        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleCancelRemNameOp
 * DESCRIPTION
 *  Handle the Cancel Connect operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleCancelRemNameOp(BtOpEvent event)
{
    MeCommandToken *cancelToken;
    MeCommandToken token;

    /* Get a pointer to the token */
    cancelToken = ContainingRecord(METOKEN, MeCommandToken, cancelOp);

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;

            OS_MemCopy(MEC(hciGenCmd).parms, cancelToken->p.name.bdAddr.addr, 6);       /* BD_ADDR */

            MeConVerifyHCIGenCommand();
            bt_trace(TRACE_GROUP_1, CANCEL_REMOTE_NAME_OPERATION);
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 6);
            MeSendHciCommand(HCC_REM_NAME_REQ_CANCEL, 6);
            return;

        case BOE_TASK_END:
            bt_trace(
                TRACE_GROUP_1,
                ME_GET_REMOTE_DEVICE_NAME_CANCEL_OPERATION_COMPLETE_STATUS__xX,
                MEC(btEvent).errCode);
            break;
        case BOE_TASK_ERR:
            bt_trace(
                TRACE_GROUP_1,
                ME_GET_REMOTE_DEVICE_NAME_CANCEL_OPERATION_COMPLETE_STATUS__xX,
                MEC(btEvent).errCode);
            /*
             * Call all the callbacks with the results. We do this by setting up
             * a token and finding all that match
             */
    #if 1
            token.flags = MCTF_NEED_CHECK;
            token.eType = BTEVENT_NAME_RESULT;
            if (MEC(btEvent).errCode == 0)
            {
                MEC(btEvent).errCode = BEC_REQUEST_CANCELLED;
            }
            OS_MemCopy(token.p.name.bdAddr.addr, cancelToken->p.name.bdAddr.addr, 6);
            token.op.opType = BOP_CANCEL_REM_NAME;
            MeReportMeCommandComplete(&token);
    #endif /* 1 */ 
            break;

        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  StartLinkConnectTask
 * DESCRIPTION
 *  Start the link connect task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartLinkConnectTask(void)
{
    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    OS_MemCopy(MEC(hciGenCmd).parms, RDEV->bdAddr.addr, 6); /* BD_ADDR */
    StoreLE16(MEC(hciGenCmd).parms + 6, (U16) BAPT_STANDARD);   /* Packet_Type */
    MEC(hciGenCmd).parms[8] = RDEV->parms.con.psi.psRepMode;    /* Page_Scan_Rep_Mode */
    MEC(hciGenCmd).parms[9] = RDEV->parms.con.psi.psMode;   /* Page_Scan_Mode */
    StoreLE16(MEC(hciGenCmd).parms + 10, RDEV->parms.con.psi.clockOffset);      /* Clock offset */
    if (RDEV->parms.con.psi.clockOffset != 0)
    {
        MEC(hciGenCmd).parms[11] |= 0x80; // set the ClockOffset Valid Flag
    }  
    /* We can debug page timeout problem with this trace. */
    bt_trace(TRACE_GROUP_1, BTLOG_CREATE_CONNECTION_CLOCK_OFFSET_SR_MODE, MEC(hciGenCmd).parms[11], MEC(hciGenCmd).parms[10], MEC(hciGenCmd).parms[8]);
    #ifdef __BT_USE_CUST_DATA__
    if(btGetCustvalue(CUSTID_CREATE_CONN_ROLE_SWITCH_REQ) == 0){
        MEC(hciGenCmd).parms[12] = 0x00; /* allowRoleChange */
    }else{
        MEC(hciGenCmd).parms[12] = RDEV->parms.con.allowRoleChange; /* allowRoleChange */
    }
    #else /* __BT_USE_CUST_DATA__ */
    MEC(hciGenCmd).parms[12] = RDEV->parms.con.allowRoleChange; /* allowRoleChange */
    #endif
    Report(("allowRoleChange=0x%x", MEC(hciGenCmd).parms[12]));
    MeConVerifyHCIGenCommand();
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 13);
    MeSendHciCommand(HCC_CREATE_CONNECTION, 13);
}


/*****************************************************************************
 * FUNCTION
 *  StartLinkAcceptTask
 * DESCRIPTION
 *  Start the link accept task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartLinkAcceptTask(void)
{
    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    OS_MemCopy(MEC(hciGenCmd).parms, RDEV->bdAddr.addr, 6); /* BD_ADDR */
    MEC(hciGenCmd).parms[6] = RDEV->parms.acceptRole;

    MeConVerifyHCIGenCommand();
    bt_trace(TRACE_GROUP_1, MECON_STARTING_LINK_ACCEPT_COMMAND_ACCEPT_ROLE__xD, RDEV->parms.acceptRole);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 7);
    MeSendHciCommand(HCC_ACCEPT_CON_REQ, 7);
}


/*****************************************************************************
 * FUNCTION
 *  StartLinkRejectTask
 * DESCRIPTION
 *  Start the link accept task.
 * PARAMETERS
 *  bdAddr              [IN]        
 *  rejectReason        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void StartLinkRejectTask(U8 *bdAddr, U8 rejectReason)
{
    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    OS_MemCopy(MEC(hciGenCmd).parms, bdAddr, 6);    /* BD_ADDR */
    MEC(hciGenCmd).parms[6] = rejectReason;

    MeConVerifyHCIGenCommand();
    bt_trace(TRACE_GROUP_1, MECON_STARTING_LINK_REJECT_COMMAND);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 7);
    MeSendHciCommand(HCC_REJECT_CON_REQ, 7);
}


/*****************************************************************************
 * FUNCTION
 *  StartLinkDisconnectTask
 * DESCRIPTION
 *  Start the link disconnect task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartLinkDisconnectTask(void)
{
    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    bt_trace(TRACE_GROUP_1, MECON_STARTING_DISCONNECT_TO_HCIHANDLE_xX, RDEV->hciHandle);
    StoreLE16(MEC(hciGenCmd).parms, RDEV->hciHandle);   /* Connection handle */
    if (RDEV->discReason != 0)
    {
        MEC(hciGenCmd).parms[2] = RDEV->discReason; /* Provided Reason */
    }
    else
    {
        MEC(hciGenCmd).parms[2] = BEC_USER_TERMINATED;      /* Default Reason */
    }

    MeConVerifyHCIGenCommand();
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 3);
    MeSendHciCommand(HCC_DISCONNECT, 3);
}


/*****************************************************************************
 * FUNCTION
 *  StartRemNameReqTask
 * DESCRIPTION
 *  Start the remote name request task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartRemNameReqTask(void)
{
    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    BTBMDumpBdAddr(METOKEN->p.name.bdAddr.addr);
    OS_MemCopy(MEC(hciGenCmd).parms, METOKEN->p.name.bdAddr.addr, 6);   /* BD_ADDR */
    MEC(hciGenCmd).parms[6] = METOKEN->p.name.io.in.psi.psRepMode;      /* Page_Scan_Rep_Mode */
    MEC(hciGenCmd).parms[7] = METOKEN->p.name.io.in.psi.psMode; /* Page_Scan_Mode */
    StoreLE16(MEC(hciGenCmd).parms + 8, METOKEN->p.name.io.in.psi.clockOffset);
    if (METOKEN->p.name.io.in.psi.clockOffset != 0)
    {
        MEC(hciGenCmd).parms[9] |= 0x80; // set the ClockOffset Valid Flag
    }  
    MeConVerifyHCIGenCommand();
    /* We can debug page timeout problem with this trace. */
    bt_trace(TRACE_GROUP_1, MECON_STARTING_REMOTE_NAME_REQUEST_COMMAND_CLOCK_OFFSET_SR_MODE, MEC(hciGenCmd).parms[9], MEC(hciGenCmd).parms[8], MEC(hciGenCmd).parms[6]);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 10);
    MeSendHciCommand(HCC_REM_NAME_REQ, 10);

}


/*****************************************************************************
 * FUNCTION
 *  StartSniffModeTask
 * DESCRIPTION
 *  Start the sniff mode task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartSniffModeTask(void)
{
    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    StoreLE16(MEC(hciGenCmd).parms, RDEV->hciHandle);   /* Connection handle */
    StoreLE16(MEC(hciGenCmd).parms + 2, RDEV->parms.sniff.maxInterval);
    StoreLE16(MEC(hciGenCmd).parms + 4, RDEV->parms.sniff.minInterval);
    StoreLE16(MEC(hciGenCmd).parms + 6, RDEV->parms.sniff.attempt);
    StoreLE16(MEC(hciGenCmd).parms + 8, RDEV->parms.sniff.timeout);

    MeConVerifyHCIGenCommand();
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 10);
    MeSendHciCommand(HCC_SNIFF_MODE, 10);
}


/*****************************************************************************
 * FUNCTION
 *  StartExitSniffModeTask
 * DESCRIPTION
 *  Start the exit sniff mode task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartExitSniffModeTask(void)
{
    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    StoreLE16(MEC(hciGenCmd).parms, RDEV->hciHandle);   /* Connection handle */
    MeConVerifyHCIGenCommand();
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
    MeSendHciCommand(HCC_EXIT_SNIFF_MODE, 2);
}


/*****************************************************************************
 * FUNCTION
 *  StartExitParkModeTask
 * DESCRIPTION
 *  Start the exit park mode task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartExitParkModeTask(void)
{
    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    bt_trace(TRACE_GROUP_1, MECON_EXIT_PARK_MODE_TO_HCIHANDLE_xX, RDEV->hciHandle);
    StoreLE16(MEC(hciGenCmd).parms, RDEV->hciHandle);   /* Connection handle */
    MeConVerifyHCIGenCommand();
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
    MeSendHciCommand(HCC_EXIT_PARK_MODE, 2);
}


/*****************************************************************************
 * FUNCTION
 *  StartSwitchRoleTask
 * DESCRIPTION
 *  Start the switch role task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartSwitchRoleTask(void)
{
    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    bt_trace(TRACE_GROUP_1, MECON_SWITCH_ROLE_TO_HCIHANDLE_xX, RDEV->hciHandle);
    OS_MemCopy(MEC(hciGenCmd).parms, RDEV->bdAddr.addr, 6); /* BD_ADDR */

    if (RDEV->role == BCR_MASTER)
    {
        /* Currently master so make slave */
        MEC(hciGenCmd).parms[6] = 0x01;
    }
    else
    {
        /* Currently slave so make master */
        MEC(hciGenCmd).parms[6] = 0x00;
    }

    MeConVerifyHCIGenCommand();
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 7);
    MeSendHciCommand(HCC_SWITCH_ROLE, 7);
}


/*****************************************************************************
 * FUNCTION
 *  ME_FindRemoteDeviceP
 * DESCRIPTION
 *  Return a pointer to the RemoteDevice structure which corresponds
 *  to the given bdAddr. If not found return 0.
 * PARAMETERS
 *  bdAddr      [IN]        
 * RETURNS
 *  BtRemoteDevice*
 *****************************************************************************/
BtRemoteDevice *ME_FindRemoteDeviceP(const U8 *bdAddr)
{
    I8 i;

    /*
     * Go through the RemoteDevice array looking for non free entries
     * and determine if one matches the bdAddr
     */
    OS_Report("ME_FindRemoteDeviceP: 0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,",bdAddr[0],bdAddr[1],bdAddr[2],bdAddr[3],bdAddr[4],bdAddr[5]);
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if (MEC(devTable)[i].state != BDS_DISCONNECTED)
        {
            OS_Report("devTable[i]: 0x%X,0x%X,0x%X,0x%X,0x%X,0x%X,",MEC(devTable)[i].bdAddr.addr[0],
                MEC(devTable)[i].bdAddr.addr[1],
                MEC(devTable)[i].bdAddr.addr[2],
                MEC(devTable)[i].bdAddr.addr[3],
                MEC(devTable)[i].bdAddr.addr[4],
                MEC(devTable)[i].bdAddr.addr[5]);
            /* We have found an active remote device */
            if (OS_MemCmp(MEC(devTable)[i].bdAddr.addr, 6, bdAddr, 6))
            {
                OS_Report("found");
                return &(MEC(devTable)[i]);
            }
        }
    }
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  MeGetFreeRemoteDevice
 * DESCRIPTION
 *  Return a pointer to a free remote device structure or 0 if
 *  one is not found.
 * PARAMETERS
 *  void
 * RETURNS
 *  BtRemoteDevice*
 *****************************************************************************/
BtRemoteDevice *MeGetFreeRemoteDevice(void)
{
    I8 i;

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
    I8 x;
#endif 
    BtRemoteDevice *rDev;

    bt_trace(TRACE_GROUP_1, MEGETFREEREMOTEDEVICE);
    /* Go through the RemoteDevice array looking for the first free entry. */
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if (MEC(devTable)[i].state == BDS_DISCONNECTED)
        {
            if (MEC(devTable)[i].sdpServInfo != 0)
            {
                Assert(0);
            }
            rDev = &MEC(devTable)[i];

            EVM_ResetTimer(&rDev->connectTimer);
            EVM_ResetTimer(&rDev->discTimer);
            EVM_ResetTimer(&rDev->disconnectACLCheckCreatACLTimer);
            rDev->discTimeoutLevel = BT_L2CAP_DISCONNECT_TIMEOUT_500MS;
            EVM_ResetTimer(&rDev->readyToSendTimer);
            EVM_ResetTimer(&rDev->sdapTimer);
            EVM_ResetTimer(&rDev->changePacketTimer);
            OS_MemSet((U8*) rDev, 0, (U16) sizeof(BtRemoteDevice));
            /* Initialize the RemoteDevice structure */
            InitializeListHead(&(rDev->handlers));
            InitializeListHead(&(rDev->txHighPriorityQueue));
            InitializeListHead(&(rDev->txLowPriorityQueue));
#ifdef __BT_4_0_BLE__
            InitializeListHead(&(rDev->txSignQueue));
#endif
            InitializeListHead(&(rDev->queryList));
            InitializeListHead(&(rDev->secTokens));
            InitializeListEntry(&(rDev->op.node));
            rDev->disconnectACLCheckCreatACLCount = 0;
            rDev->disconnectACLCheckCreatACLAlwaysForce = 0;
            rDev->roleSwitchCounter = 0;
            rDev->scOutoConnectState = 0;
            rDev->sniffCounter = 0;
            rDev->connectCounter = 0;
            OS_MemSet((U8*) & rDev->remoteFeature, 0, sizeof(rDev->remoteFeature));
            OS_MemSet((U8*) & rDev->remoteVersion, 0, sizeof(rDev->remoteVersion));

        #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
            InitializeListHead(&(rDev->txPacketList));
            InitializeListHead(&(rDev->txNonAckQueue));
            for (x = 0; x < BT_LINK_RETRANSMISSION_TXPACKET_NO; x++)
            {
                InsertTailList(&(rDev->txPacketList), &(rDev->txPackets[x].node));
            }
        #endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */
            if (rDev->rxBuffer != 0)
            {
                Assert(0);
            }
            rDev->rxBuffer = MM_AllocBuffer(L2CAP_MTU + 6, (void*)&i);
            if (rDev->rxBuffer == 0)
            {
                return 0;
            }
            rDev->qos.serviceType = BT_QOS_BEST_EFFORT;

            return rDev;
        }
    }
    bt_trace(TRACE_ERROR, MEGETFREEREMOTEDEVICENULL);
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  MeCheckDeviceStateNotIdle
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bd_addr     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL MeCheckDeviceStateNotIdle(U8 *bd_addr)
{
    I8 i;
    BtRemoteDevice *rDev;
    U8 temp_event[4];

    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_MECHECKDEVICESTATENOTIDLE_CHECK_CURRENT_MODE);
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if (MEC(devTable)[i].state != BDS_DISCONNECTED)
        {
            rDev = &MEC(devTable)[i];
            if (OS_MemCmp((U8*) rDev->bdAddr.addr, 6, bd_addr, 6))
            {
                switch (MEC(devTable)[i].state)
                {
                    case BDS_OUT_DISC:
                    case BDS_OUT_DISC2:
                    case BDS_CONNECTED:
                        temp_event[0] = BEC_LOCAL_TERMINATED;
                        StoreLE16(&temp_event[1], rDev->hciHandle);
                        temp_event[3] = BEC_LOCAL_TERMINATED;
                        MeHandleDisconnectComplete(4, temp_event);
                        break;
                    case BDS_OUT_CON:
                        OS_Report("MeCheckDeviceStateNotIdle : remDev state is BDS_OUT_CON");
                        if(IsNodeOnList(&MEC(opList), &rDev->op.node)){
                            OS_Report("op is on opList");
                            RemoveEntryList(&rDev->op.node);
                            MEC(btEvent).errCode = BEC_ACL_ALREADY_EXISTS;
                            MeReportLinkUp(rDev, BTEVENT_LINK_CONNECT_CNF);
                            if(MEC(pendCons) == 0){
                                Assert(0);
                            }
                            MEC(pendCons) -= 1;
                            return TRUE;
                        }else if(MEC(curOp) == &rDev->op.node){
                            rDev->linkReqPending = 1;
                        }else{
                            OS_Report("[WARN] remDev state is BDS_OUT_CON, but op is neigher on opList nor equal curOp");
                        }
                        break;
                }
                return FALSE;
            }
        }
    }
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  MeFreeRemoteDevice
 * DESCRIPTION
 *  Return a remote device structure to the pool.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeFreeRemoteDevice(BtRemoteDevice *remDev)
{
    BtDeviceContext *bdc = 0;
    BtEventType old_eType = 0;
    BtEventMask mask = 0;

    bdc = DS_FindDevice(&(remDev->bdAddr));
    if (bdc != 0x00)
    {
        bdc->link = 0x00;
    }
    remDev->lastHighPriorityTxTime = 0;
    remDev->lastLowPriorityTxTime = 0;
    remDev->lastTxRxTime = OS_GetSystemTime();
    remDev->state = BDS_DISCONNECTED;
    remDev->roleSwitchCounter = 0;
    remDev->scOutoConnectState = 0;
    remDev->sniffCounter = 0;
    OS_MemSet((U8*) & remDev->remoteFeature, 0, sizeof(remDev->remoteFeature));
    OS_MemSet((U8*) & remDev->remoteVersion, 0, sizeof(remDev->remoteVersion));

    MM_FreeBuffer(remDev->rxBuffer);

    remDev->rxBuffer = 0;

    /* Ask bm to check all link state in power case. */
    old_eType = MEC(btEvent).eType;
    MEC(btEvent).eType = BTEVENT_CHECK_ALL_LINK_POWER_OFF;
    /* We only want to report to bm. Set to BEM_ALL_EVENTS to avoid checking too much callback function. */
    mask = BEM_ALL_EVENTS;

    MeReportResults(mask);
    MEC(btEvent).eType = old_eType;
}


/*****************************************************************************
 * FUNCTION
 *  MeMapHciToRemoteDevice
 * DESCRIPTION
 *  Return a pointer to the BtRemoteDevice structure that matches
 *  the given hci handle. If no structure is found then return 0.
 *  The remote device must be active.
 * PARAMETERS
 *  hciHandle       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtRemoteDevice *MeMapHciToRemoteDevice(HciHandle hciHandle)
{
    I8 i;

#ifndef __BT_3_0_HS__
    /* Go through the RemoteDevice array looking for a match. */
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if (MEC(devTable)[i].state != BDS_DISCONNECTED)
        {
            if (MEC(devTable)[i].hciHandle == hciHandle)
            {
                return &(MEC(devTable)[i]);
            }
        }
    }
    return 0;
#else

	/* 0x0~x32:              */
	/* 0x0032~0x00ff: edr    */
	/* 0x0100~0x01ff: bt30hs */
	/* 0x0200~0x0300: ble    */

    if((hciHandle >= 32) &&(hciHandle < 0x0100))
    {
        /* Go through the RemoteDevice array looking for a match. */
        for (i = 0; i < NUM_BT_DEVICES; i++)
        {
            if (MEC(devTable)[i].state != BDS_DISCONNECTED)
            {
                if (MEC(devTable)[i].hciHandle == hciHandle)
                {
                    return &(MEC(devTable)[i]);
                }
            }
        }
        return 0;
    }
    else if((hciHandle >= 0x0200) &&(hciHandle < 0x0300))
    {
        /* Go through the RemoteDevice array looking for a match. */
        for (i = 0; i < NUM_BT_DEVICES; i++)
        {
            if (MEC(devTable)[i].state != BDS_DISCONNECTED)
            {
                if (MEC(devTable)[i].hciHandle == hciHandle)
                {
                    return &(MEC(devTable)[i]);
                }
            }
        }
        return 0;
    }    
#if !defined(__MTK_BT_EXTERNAL_PLATFORM__)   
    else if(hciHandle < 0x32)
    {
        U8 j=0;
        /* The link handler is logical link handler*/
        /* Go through the RemoteDevice array looking for a match. */
        for (i = 0; i < NUM_BT_DEVICES; i++)
        {
            if (MEC(devTable)[i].state != BDS_DISCONNECTED)
            {
                if((MEC(devTable)[i].remoteWifiAMP.physical_link_sate == BDS_CONNECTED) &&
                    (hciHandle & 0x0fff) == (MEC(devTable)[i].remoteWifiAMP.physical_link_handle & 0x0fff))
                return &(MEC(devTable)[i]);

            }
        }
        return 0;    
    }
    else
    {
        /* bt30hs */
        U8 j=0;
        /* The link handler is logical link handler*/
        /* Go through the RemoteDevice array looking for a match. */
        for (i = 0; i < NUM_BT_DEVICES; i++)
        {
            if (MEC(devTable)[i].state != BDS_DISCONNECTED)
            {
                for(j=0; j< BT_AMP_SUPPORTED_LOGICAL_CHANNEL;j++)
                {
                	if( (hciHandle & 0x0fff) == (MEC(devTable)[i].remoteWifiAMP.logicalLink[j].logical_link_handle & 0x0fff) ){
                	    // Report((" hciHandle:0x%x logical_link_handle:0x%x link_state:%x", hciHandle, MEC(devTable)[i].remoteWifiAMP.logicalLink[j].logical_link_handle, MEC(devTable)[i].remoteWifiAMP.logicalLink[j].logical_link_sate));
	                    if( MEC(devTable)[i].remoteWifiAMP.logicalLink[j].logical_link_sate == BDS_CONNECTED ) 
	                    {
	                    	return &(MEC(devTable)[i]);	
	                    }
	                }

                }
            }
        }
        return 0;

    }
#else
    else{
        /* Go through the RemoteDevice array looking for a match. */
        for (i = 0; i < NUM_BT_DEVICES; i++)
        {
            if (MEC(devTable)[i].state != BDS_DISCONNECTED)
            {
                if (MEC(devTable)[i].hciHandle == hciHandle)
                {
                    return &(MEC(devTable)[i]);
                }
            }
        }
        return 0;    
    }
#endif

#endif

}


/*****************************************************************************
 * FUNCTION
 *  MeCallLinkHandlers
 * DESCRIPTION
 *  Call the callback of all the handlers bound to the remote device.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeCallLinkHandlers(BtRemoteDevice *remDev, BtEvent *event)
{
    ListEntry *entry;
    BtHandler *handler;

    if (!IsListCircular(&(remDev->handlers)))
    {
        Assert(0);
    }
    entry = remDev->handlers.Flink;
    while (entry != &(remDev->handlers))
    {
        /*
         * Point to the next entry before calling the callback in case the
         * callback removes the current handler
         */
        handler = (BtHandler*) entry;
        entry = entry->Flink;
        event->handler = handler;
        /* ME Link Pending */
        OS_Report("[ME][CON] MeCallLinkHandlers : handler=%p, callback=%p, pending=%u", handler, handler->callback, handler->pending);
        if(!handler->pending){
            handler->callback(event);
        }
        if (!IsListCircular(entry))
        {
            Assert(0);
        }
            
    }
    if (!IsListCircular(&(remDev->handlers)))
    {
        Assert(0);
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleLinkConnectReq
 * DESCRIPTION
 *  Handle an incoming link connection.
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleLinkConnectReq(U8 len, U8 *parm)
{
    U8 *p;
    BtRemoteDevice *remDev = 0;
    U32 cod;
    BD_ADDR addr;

    /* UNUSED_PARAMETER(len); */

    /*
     * Verify that the reject entry is available and copy the in the
     * the BdAddr just in case
     */

    /* Determine the type of link */
    if (parm[9] == BLT_SCO || parm[9] == BLT_ESCO)
    {
        /* This is an SCO connection request */
        bt_trace(TRACE_GROUP_1, MECON_RECEIVED_SCO_CONNECT_REQUEST);
    #if NUM_SCO_CONNS > 0
        /* Deal with the SCO link request */
        if (ScoHandleLinkConnectReq(len, parm) == TRUE)
        {
            return;
        }
        else
        {
            bt_trace(TRACE_GROUP_1, HANDLE_SCOHANDLELINKCONNECTREQ_ERROR);
        }
    #endif /* NUM_SCO_CONNS > 0 */
    }
    else
    {
        /*
         * This is a request for an ACL link. We cannot have more than one
         * ACL link per device. We assume that the radio will not allow this
         */
        bt_trace(TRACE_GROUP_1, MECON_RECEIVED_ACL_CONNECT_REQUEST);

        OS_MemCopy(addr.addr, parm, 6);
		p = parm + 6;
        cod = (U32) (p[0]) | ((U32) (p[1]) << 8) | ((U32) (p[2]) << 16);
        bt_trace(TRACE_GROUP_1, MECON_COD__0XxX, cod);
        DDB_COD_AddRecord(addr, cod);
        DDB_COD_Flush();    /* DDB_COD */
        
        if (!MeCheckDeviceStateNotIdle(parm))
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_DEVICE_IN_NOT_DISCONNECT_MODE_IGNORE_IT);
        }
        /* First get a free remote device structure */
        if ((MEC(pendCons) == 0) &&
            ((remDev = MeGetFreeRemoteDevice()) != 0) && (BTBMCheckAclConnectRequestNotAddPendingCon() == FALSE))
        {
            /* Store the proper values */
            OS_MemCopy(remDev->bdAddr.addr, parm, 6);
            p = parm + 6;
            remDev->cod = cod;
            remDev->state = BDS_IN_CON;
            MEC(pendCons) += 1;
            if (MEC(pendCons) == 0)
            {
                Assert(0);
            }

            /*
             * If we have an accept handler then call it. Otherwise accept
             * the connection automatically
             */
            if (MEC(acceptHandler) != 0)
            {
                /* Call the accept handler */
                MEC(btEvent).p.remDev = remDev;
                MEC(btEvent).eType = BTEVENT_LINK_CONNECT_REQ;
                MeDevEventHandler();
                MEC(acceptHandler)->callback(&MEC(btEvent));
                return;
            }
            else
            {
                /* Accept the connection automatcially */

                /*
                 * Be sure thar the remDev->op is not in MEC(opList):  2006-0911
                 * otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
                 * For later, to execute this opType will assert due to opType is equal zero!
                 */
#if 0
                if (IsNodeOnList(&MEC(opList), &(remDev->op.node)))
                {
                    Assert(0);
                }
#endif
                U8 piconet_no, scatternet_no, sco_no, esco_no, msco_no, ssco_no;

                BTBMGetScattnetStatus(&piconet_no, &scatternet_no, &sco_no, &esco_no, &msco_no, &ssco_no);
                MeConVerifyRemDevOpOnList(remDev); 
                remDev->op.opType = BOP_LINK_ACCEPT;
                remDev->restricted = FALSE;
                if (sco_no + esco_no)
                {
                    remDev->parms.acceptRole = remDev->role = BCR_MASTER;
                }
                else
                {
                    remDev->parms.acceptRole = remDev->role = BCR_SLAVE;
                }
            }
            /* Add the operation to the queue */
            if (!IsEntryAvailable(&(remDev->op.node)))
            {
                Assert(0);
            }
            MeAddOperationFront(remDev);
            goto startOp;
        }
    }
    if (remDev == 0)
    {
        bt_trace(TRACE_GROUP_1, REMDEV_0);
    }
    if (MEC(pendCons) != 0)
    {
        bt_trace(TRACE_GROUP_1, MECPENDCONS_0);
    }
  startOp:
    /* Attempt to start the operation */
    MeStartOperation();

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleConnectComplete
 * DESCRIPTION
 *  Handle completion of a link connection (incoming and outgoing).
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleConnectComplete(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev = 0;
    I8 i;
    U8 *temp_parm;
    BD_ADDR bdAddr;

    /* UNUSED_PARAMETER(len); */

    bt_trace(TRACE_GROUP_1, BTLOG_ME_CONNECTION_COMPLETE_EVENT_STATUS, parm[0], parm[10]);
    /* 
     * Delete the invalid link key while authentication failed 
     * to trigger new link key generation.
     */
    if (parm[0] == HC_STATUS_AUTH_FAILURE)
    {    
        BtStatus status = BT_STATUS_FAILED;    

        OS_MemCopy(bdAddr.addr, parm + 3, 6);    
        status = SEC_DeleteDeviceRecord(&bdAddr);
        bt_trace(TRACE_GROUP_1, BT_LOG_BTBM_AUTH_CFM_DEL_DEV_RECORD_STATUS, status);
    }    
    BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
    /* Determine the type of connection */
    if (parm[9] == BLT_SCO || parm[9] == BLT_ESCO)
    {
        bt_trace(TRACE_GROUP_1, BTLOG_ME_SCO_CONNECT_COMPLETE_EVENT);
        /*
         * If SCO is supported then have the SCO code handle it. Otherwise
         * we will ignore it since we should have rejected it anyway.
         */
    #if NUM_SCO_CONNS > 0
        /* Pass it to SCO code */
        ScoHandleConnectComplete(len, parm);
    #endif /* NUM_SCO_CONNS > 0 */
        return;
    }

    /* This is for an ACL link */
    bt_trace(TRACE_GROUP_1, BTLOG_ME_ACL_CONNECT_COMPLETE_EVENT);
    /*
     * Locate the pending RemoteDevice that matches the complete event and
     * is pending.
     */
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        remDev = &(MEC(devTable)[i]);
        if ((remDev->state == BDS_OUT_CON) || (remDev->state == BDS_IN_CON))
        {
            /* We have found a pending remote device */
            if (StrnCmp(remDev->bdAddr.addr, 6, parm + 3, 6))
            {
                break;
            }
            /* Ericsson Workaround!!! Handles incorrect returned BdAddr case */
            /*
            if (MEC(pendCons) == 1)
            {
                break;
            }
            */
        }
    }

    if (i < NUM_BT_DEVICES)
    {
        /* We have found a device */
        if (MEC(pendCons) == 0)
        {
            Assert(0);
        }
        MEC(pendCons) -= 1;

        MEC(btEvent).errCode = parm[0];
#ifdef __BT_4_0_BLE__
        OS_MemSet(remDev->fixCidState, 0, sizeof(remDev->fixCidState));
#endif
        remDev->hciHandle = LEtoHost16(parm + 1);
        if (parm[10] != 0)
        {
            remDev->encryptState = BES_ENCRYPTED;
        }
        else
        {
            remDev->encryptState = BES_NOT_ENCRYPTED;
        }
        remDev->maxTxCount = HCC(maxAclTxPacketsLeft);
        remDev->roleSwitchCounter = 0;
        remDev->scOutoConnectState = 0;
        remDev->sniffCounter = 0;
        remDev->supportESCO = 1;
    #if defined (__BT_2_1_SNIFF_SUBRATING__)
        remDev->sniffSubrateState = 0;
        remDev->sniffSubrateMaxLatency = 0;
        remDev->sniffSubrateMinLocalTimeout = 0;
        remDev->sniffSubrateMinRemoteTimeout = 0;
    #endif /* defined (__BT_2_1_SNIFF_SUBRATING__) */ 
    #if defined (__BT_2_1_SIMPLE_PAIRING__)
        remDev->gapKeyType = BT_LINK_KEY_GAP_TYPE_NO_KEY;
        /* Initiate the gapSecurityLevel to MED, so that if no securityRecord is found,
            MITM required, general Bonding will be used
        */
        remDev->gapSecurityLevel = BT_SEC_UNKNOWN;
        remDev->ioCapabilitiyState = BIO_NOT_REQUESTD;
        OS_SetBit(BDEF_SSP_HOST, remDev->remoteExtFeature);
    #endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
    #if defined (__BT_2_1_SUPERVISION_TIMEOUT_EVENT__)
        remDev->supervisionTimeout = BT_SUPERVISION_TIMEOUT_DEFAULT_VALUE;
    #endif 
        remDev->flushTimeOut = 0xFFFFFFFF;
        if(remDev->hciHandle > 0x210)
        {
            remDev->remoteCapability = 1;
        }
        else
        {
            remDev->remoteCapability = 0;
        }

        bt_trace(TRACE_GROUP_1, BTLOG_ME_ACL_CONNECTION_COMPLETE_EVENT_HCIHANDLE, remDev->hciHandle);
    #ifndef MTK_BB_LOOPBACK_MODE
        EVM_ResetTimer(&remDev->changePacketTimer);
        if (MEC(btEvent).errCode == 0)
        {
            MeReadRemoteSupportedFeature(remDev);
            MeReadRemoteVersion(remDev);
            MeReadRemoteClockOffset(remDev);
        #if defined (__BT_CHIP_VERSION_0__)
            MeChangePacketType(remDev);
        #else /* defined (__BT_CHIP_VERSION_0__) */
            remDev->changePacketTimer.func = MeHandleAclLinkChangePacketType;
            remDev->changePacketTimer.context = remDev;
            EVM_StartTimer(&remDev->changePacketTimer, 1000);
        #endif /* defined (__BT_CHIP_VERSION_0__) */
        #if defined (__BT_2_1_AUTO_FLUSH__)
            MeWriteAutomaticallyFlushedTimeout(remDev);
        #endif 
        }
    #endif /* MTK_BB_LOOPBACK_MODE */ 
        Me_WriteAudioSetting();
        EVM_ResetTimer(&remDev->connectTimer);
        EVM_ResetTimer(&remDev->discTimer);
        remDev->discTimeoutLevel = BT_L2CAP_DISCONNECT_TIMEOUT_500MS;
        EVM_ResetTimer(&remDev->readyToSendTimer);
        EVM_ResetTimer(&remDev->sdapTimer);

        MeHandleAclConComplete(remDev);
    }
    else
    {
        /* We do not have remote device so this must be an internal reject */
        bt_trace(TRACE_GROUP_1, MECON_ACL_CONNECT_COMPLETE_INTERNAL_REJECT);
        temp_parm = parm + 3;
        BTBMDumpBdAddr(temp_parm);
        if (!IsListEmpty(&(MEC(rejectOp).op.node)))
        {
            bt_trace(TRACE_GROUP_1, REJECT_OP_IS_NOT_EMPTY);
            if (OS_MemCmp(MEC(rejectOp).bdAddr.addr, 6, parm + 3, 6))
            {
                bt_trace(TRACE_GROUP_1, REJECT_OP_WITH_THE_SAME_ADDR);
                if (IsNodeOnList(&MEC(opList), &(MEC(rejectOp).op.node)))
                {
                    bt_trace(TRACE_GROUP_1, REJECT_OP_IN_LIST_REMOTE_THE_REJECT_OP_FROM_LIST);
                    RemoveEntryList(&(MEC(rejectOp).op.node));
                }
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleDisconnectComplete
 * DESCRIPTION
 *  Handle the Disconnect Complete event.
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleDisconnectComplete(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev = 0;
    I8 i;
    /* ME Link Pending */
    ListEntry pendingHandlers;
    BtHandler *handler;
    BtPageScanInfo psi;
    BtDeviceContext *bdc;
    BD_ADDR addr;
    BtStatus status = BT_STATUS_FAILED;

    /* UNUSED_PARAMETER(len); */

    bt_trace(TRACE_GROUP_1, BTLOG_ME_DISCONNECT_COMPLETE_EVENT_STATUS_REASON, parm[0], parm[3]);
    
#if NUM_SCO_CONNS > 0
    /* Determine if this handle is for SCO. If so call SCO to handle it */
    if (ScoHandleDisconnectComplete(len, parm) == TRUE)
    {
        return;
    }
#endif /* NUM_SCO_CONNS > 0 */

    /*
     * Locate the pending RemoteDevice that matches the complete event and
     * is pending.
     */
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        remDev = &(MEC(devTable)[i]);
        if ((remDev->state >= BDS_OUT_DISC) || (remDev->state == BDS_CONNECTED))
        {
            /* We have found a device in the correct state */
            if (remDev->hciHandle == LEtoHost16(parm + 1))
            {
                break;
            }
        }
    }

    /*
     * It is possible that we tried to connect at the same time as the remote device.
     * If the remote devices got in first then we should not find a device in the
     * the correct state. In that case we ignore this event.
     */
    if (i < NUM_BT_DEVICES)
    {
        /*
         * We found the device to disconnect. The status should always be 0 in
         * this case.
         */

        if (parm[0] != 0 && parm[0] != BEC_NO_CONNECTION)
        {
            /* The disconnect failed. Try to requeue. */
            if (remDev->op.opType == BOP_NOP)
            {
                /*
                 * No operation is in progress so put the disconnect operation 
                 * on the queue.
                 */

                //Be sure thar the remDev->op is not in MEC(opList):  2006-0911
                //otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
                //For later, to execute this opType will assert due to opType is equal zero!
#if 0                
                if (IsNodeOnList(&MEC(opList), &(remDev->op.node)))
                {
                    Assert(0);
                }
#endif
                MeConVerifyRemDevOpOnList(remDev);
                remDev->op.opType = BOP_LINK_DISCONNECT;

                /*
                 * Queue the operation, but don't start it.
                 * Let the next event complete start it.
                 */
                MeAddOperation(remDev);
            }
            else
            {
                /* We can't requeue, so return error to app */
                MEC(btEvent).eType = BTEVENT_LINK_DISCONNECT;
                MEC(btEvent).errCode = parm[0];
                MEC(btEvent).p.remDev = remDev;
                MeReportResults(BEM_LINK_DISCONNECT);
            }
            return;
        }

        bt_trace(TRACE_GROUP_1, BTLOG_ME_ACL_DISCONNECT_COMPLETE_EVENT_HCIHANDLE, remDev->hciHandle);


        /* MAUI_01498320: Cancel MPR Timer mtk01411 */
        if (remDev->monitorPairResultTimer.func != 0)
        {
            bt_trace(TRACE_GROUP_1,BT_LOG_MPR_CANCEL_TIMER_IN_AUTHSTATE_CASE,remDev->authState,3);
            EVM_ResetTimer(&remDev->monitorPairResultTimer);
        }


        /* Make sure the state is in the pending disconnect. */
        remDev->state = BDS_OUT_DISC;

        if (MEC(activeCons) == 0)
        {
            Assert(0);
        }
        MEC(activeCons) -= 1;

        /*
         * If this is the last connection going down then put a change access mode 
         * operation on the queue
         */
        if ((MEC(stackState) == BTSS_INITIALIZED) && (HCI_GetState() != HCI_STATE_DEINIT))
        {
            BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
        }

        /* Put the reason code into the error code */
        MEC(btEvent).errCode = parm[3];

        /* Clear any pending operations */
        MeFailPendingRemDevCommands(remDev);

        /* Make sure state is not authorized */
        remDev->authorizeState = BAS_NOT_AUTHORIZED;
        remDev->authState = BAS_NOT_AUTHENTICATED;
        /*
        if (remDev->pairingType == BPT_NOT_SAVED)
        {
            OS_Report("[SEC] No bonding, delete record");
            SEC_DeleteDeviceRecord(&remDev->bdAddr);
        }
        */

    #if BT_SECURITY == XA_ENABLED
        /* Fail all security access requests */
        SecProcessAllSecurityTokens(remDev);
    #endif /* BT_SECURITY == XA_ENABLED */

        MeRemovePendingRemoteOperation(remDev);
        //Be sure thar the remDev->op is not in MEC(opList): 
        //otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
        //For later, to execute this opType will assert due to opType is equal zero!
#if 0
        if (IsNodeOnList(&MEC(opList), &(remDev->op.node)))
        {
            Assert(0);
        }
#endif
        MeConVerifyRemDevOpOnList(remDev); 
        /* Clear the operation type */
        remDev->op.opType = BOP_NOP;

        /* Make sure this op is not still curOp */
        if (&(remDev->op) == MEC(curOp))
        {
            MeOperationEnd();
        }

    #if NUM_SCO_CONNS > 0
        /*
         * Tell SCO that the ACL link is down thus, all its SCO connections
         * are also down.
         */
        ScoHandleDisconnectAcl(remDev);
    #endif /* NUM_SCO_CONNS > 0 */
#ifdef __BT_3_0_HS__
        if( remDev ){
            OS_Report( "[test] state:%d", remDev->remoteWifiAMP.state );
        }

        if((remDev !=0) &&
           (remDev->remoteWifiAMP.state > BDS_DISCONNECTED) &&
            (remDev->remoteWifiAMP.state <= BDS_CONNECTED))
            ME_DisconnectPhysicalLink(&remDev->remoteWifiAMP, 0x13);
#endif
        /* Tell L2CAP that the link has gone down */
        L2CAP_LinkDisconnect(remDev);

        /* Verify that SDP has cleaned up the remote device */
        if (remDev->sdpServInfo != 0)
        {
            Assert(0);
        }

        if (MEC(activeCons) == 0)
        {
            /* Tell L2CAP that broadcast link is down */
            L2CAP_LinkDisconnect(&L2C(broadcastLink));
        }

        EVM_ResetTimer(&remDev->connectTimer);
        EVM_ResetTimer(&remDev->discTimer);
        remDev->discTimeoutLevel = BT_L2CAP_DISCONNECT_TIMEOUT_500MS;
        EVM_ResetTimer(&remDev->readyToSendTimer);
        EVM_ResetTimer(&remDev->sdapTimer);
        EVM_ResetTimer(&remDev->changePacketTimer);
        EVM_ResetTimer(&remDev->disconnectACLCheckCreatACLTimer);
        EVM_ResetTimer(&remDev->changePacketTimer);

        bt_trace(TRACE_GROUP_1, MEHANDLEDISCONNECTCOMPLETE_REPORT_TO_UPERxX);
        /* Tell all the bound handlers that the link has been disconnected */
        MEC(btEvent).errCode = parm[3];
        MEC(btEvent).eType = BTEVENT_LINK_DISCONNECT;
        MEC(btEvent).p.remDev = remDev;
        CallAndRemoveHandlers(remDev);

        /* Now report to all global handlers */
        MeReportResults(BEM_LINK_DISCONNECT);
#ifdef __BT_SINGLE_ANTENNA__
        BT_WMT_ACL_Send_Status(remDev, 0);
#endif

        if (remDev->pairingType == BPT_NOT_SAVED)
        {
            OS_Report("[SEC] No bonding, delete record");
            SEC_DeleteDeviceRecord(&remDev->bdAddr);
            //TODO: notify MMI
            MEC(btEvent).eType = BTEVENT_LINK_KEY_DELETED;
            OS_MemCopy(MEC(btEvent).p.bdAddr.addr, remDev->bdAddr.addr, sizeof(remDev->bdAddr.addr));
            MeReportNMResults();
        }

        /* ME Link Pending */
        psi = remDev->parms.con.psi;
        addr = remDev->bdAddr;
        OS_Report("[TEST] remDev->handlers=%p (%p, %p)", &remDev->handlers, remDev->handlers.Flink, remDev->handlers.Blink);

        /* Set the state to disconnected */
        MeFreeRemoteDevice(remDev);

        MoveList(&pendingHandlers, &remDev->handlers);
        OS_Report("[TEST] pendingHandlers=%p (%p, %p)", &pendingHandlers, pendingHandlers.Flink, pendingHandlers.Blink);

        /* ME Link Pending */
        bdc = DS_FindDevice(&addr);
        if(bdc == NULL)
            OS_Report("[ME][CONN][WARN] bdc is NULL when doing ACL reconnect");
        handler = (BtHandler*)pendingHandlers.Flink;
        MEC(btEvent).eType = BTEVENT_LINK_CONNECT_CNF;
        MEC(btEvent).errCode = BEC_COMMAND_DISALLOWED;
        
        while (!IsListEmpty(&pendingHandlers)){
            handler = (BtHandler*) RemoveHeadList(&pendingHandlers);
            InitializeListEntry(&handler->node);
            OS_Report("[ME][CONN] ME reconnect : handler=%p", handler);
            if (bdc && handler->pending){
                if (handler->pending == BT_CONN_PENDING_BREDR){
                    status = ME_CreateLink(handler, &addr, &psi, &(bdc->link));
                }else{
                    #ifdef __BT_4_0_BLE__
                    status = ME_LE_CreateLink(handler, &addr, NULL, &(bdc->link));
                    #endif
                }
            }
            if( bdc == NULL || status != BT_STATUS_PENDING ){
                OS_Report("[ME][CONN][WARN] Doing reconnect : bdc=%p, status=%d", bdc, status);
                MEC(btEvent).handler = handler;
                handler->callback(&MEC(btEvent));
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeFailPendingRemDevCommands
 * DESCRIPTION
 *  Fail all pending ME commands for remote device.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeFailPendingRemDevCommands(BtRemoteDevice *remDev)
{
    BtOperation *op, *nextOp, *saveCurOp;
#ifdef __BT_4_0_BLE__
    BtDeviceContext *bdc = DS_FindDevice(&remDev->bdAddr);
#endif

    saveCurOp = 0;
    /* First, fail the current remDev operation, if it's not disconnect. */
    if (MEC(curOp) != 0 && MEC(curOp)->opType != BOP_LINK_DISCONNECT)
    {
        if (MeFailRemDevCommand(remDev, MEC(curOp)->opType, (void*)MEC(curOp)) == TRUE)
        {
            saveCurOp = MEC(curOp);
            if (IsNodeOnList(&MEC(cmdList), &saveCurOp->node))
            {
                /* This is an async command */
                RemoveEntryList(&saveCurOp->node);
            }
            /* Calling operation end after pending commands in opList & cmdList are removed */
            //MeOperationEnd();
        }
    }

    /* Go through the operations queue and fail operations for remDev */
    if (!IsListEmpty(&MEC(opList)))
    {
        op = (BtOperation*) GetHeadList(&MEC(opList));
        do
        {
            nextOp = (BtOperation*) GetNextNode(&op->node);
            RemoveEntryList(&op->node);
            OS_Report("[ME][CON] Fail pendiong op : op=%p, type=%u", op, op->opType);
            if (MeFailRemDevCommand(remDev, op->opType, (void*)op) == TRUE)
            {
                /* op->node may be reset in MeFailRemDevCommand(), so check it is still on MEC(opList) or not first */
                /*
                if (IsNodeOnList(&MEC(opList), &op->node))
                {
                    RemoveEntryList(&op->node);
                }
                */
            }else{
                OS_Report("[ME][CON] not dev related cmd, re-insert into opList");
                InsertTailList(&(nextOp->node), &(op->node));
            }
            /* ALPS00804960  :when power off, nextOp may be removed when MeStartOperation is called. 
                If nextOp is not on opList, restart the loop. */
            if(nextOp == (BtOperation*) &MEC(opList) || IsNodeOnList(&MEC(opList), &nextOp->node)){
            op = nextOp;
            }else{
                OS_Report("nextOp is removed from opList. Restart.");
                op = (BtOperation*) GetHeadList(&MEC(opList));
            }
        } while ((BtOperation*) &MEC(opList) != op);
    }

    /* Clear remDev from the command queue */
    if (!IsListEmpty(&MEC(cmdList)))
    {
        op = (BtOperation*) GetHeadList(&MEC(cmdList));
        if (op == 0)
        {
            Assert(0);
        }
        do
        {
            if (IsListEmpty(&MEC(cmdList)))
            {
                break;
            }
            if (op == 0)
            {
                op = (BtOperation*) GetHeadList(&MEC(cmdList));
            }
            nextOp = (BtOperation*) GetNextNode(&op->node);
            if (op != saveCurOp)
            {
                /* This is not the current remDev operation so check it */
                if (MeFailRemDevCommand(remDev, op->opType, (void*)op) == TRUE)
                {
                    if (IsNodeOnList(&MEC(cmdList), &op->node))
                    {
                        RemoveEntryList(&op->node);
                    }
                }
            }
            op = nextOp;
        } while ((BtOperation*) &MEC(cmdList) != op);
    }

    if(saveCurOp){
        OS_Report("curOp is removed. Call MeOperationEnd to continue");
        MeOperationEnd();
    }

#if BT_SECURITY == XA_ENABLED
  #ifdef __BT_4_0_BLE__
    if (bdc)
    {
        /* Handle in SM which is callbacked from L2CAP */
        OS_Report("Handle security in SM which is callbacked from L2CAP: %d", bdc->devType);
        #ifdef __BT_LE_STANDALONE__
            return;
        #else
        if (bdc->devType == BT_DEV_TYPE_LE)
        {
            return;
        }
        #endif
    }
    else
    {
        OS_Report("MeFailPendingRemDevCommands: bdc is NULL");
    }
  #endif
    /* Fail Authenticate op when between status and complete events. */
    if (remDev->authState > BAS_NOT_AUTHENTICATED && remDev->authState < BAS_AUTHENTICATED)
    {
        MEC(btEvent).errCode = BEC_NO_CONNECTION;
        SecReportAuthenticateResult(remDev);
    }

    /* Fail Authorize op when between status and complete events. */
    if (remDev->authorizeState > BAS_NOT_AUTHORIZED && remDev->authorizeState < BAS_AUTHORIZED)
    {
        MEC(btEvent).errCode = BEC_NO_CONNECTION;
        SecHandleAuthorizeRequest(remDev);
    }

    /* Fail Encrypt op when between status and change events. */
    if (remDev->encryptState == BES_END_ENCRYPT || remDev->encryptState == BES_START_ENCRYPT)
    {
        MEC(btEvent).errCode = BEC_NO_CONNECTION;
        SecReportEncryptionResult(remDev, 0);
    }
#endif /* BT_SECURITY == XA_ENABLED */
}


/*****************************************************************************
 * FUNCTION
 *  MeFailRemDevCommand
 * DESCRIPTION
 *  Fail the command pointed to by the operation if it belongs
 *  to remDev.
 * PARAMETERS
 *  remDev      [IN]        
 *  opType      [IN]        
 *  parm        [IN]        
 * RETURNS
 *  TRUE if the command was failed.
 *****************************************************************************/
BOOL MeFailRemDevCommand(BtRemoteDevice *remDev, BtOpType opType, void *parm)
{
    MeCommandToken *token = (MeCommandToken*) parm;
    BtRemoteDevice *tremDev = (BtRemoteDevice*) parm;

#if BT_SECURITY == XA_ENABLED
    BtSecurityOp *secop = (BtSecurityOp*) parm;
#endif /* BT_SECURITY == XA_ENABLED */
#if NUM_SCO_CONNS > 0
    BtScoConnect *scocon = (BtScoConnect*) parm;
#endif 

    MEC(btEvent).errCode = BEC_NO_CONNECTION;

    switch (opType)
    {
        case BOP_REM_NAME_REQ:
            if (token->remDev == remDev)
            {
                token->flags &= ~MCTF_NEED_CHECK;
                MeReportMeCommandComplete(token);
                /*
                 * MeReportMeCommandComplete removed the token from
                 * the list so, return FALSE here to prevent trying to
                 * remove it again.
                 */
                //return FALSE;
                return TRUE;
            }
            break;

        case BOP_WRITE_LINK_POLICY:
            if (&remDev->policyOp == (BtOperation*) parm)
            {
                return TRUE;
            }
            break;

        case BOP_DISCOVER_ROLE:
            if (&remDev->roleOp == (BtOperation*) parm)
            {
                /*
                 * Role discovery is done at connection, no need to indicate it
                 * if the connection is gone. 
                 */
                return TRUE;
            }
            break;

        case BOP_GEN_COMMAND:
            if (token->remDev == remDev)
            {
                token->p.general.out.status = BT_STATUS_FAILED;
                MEC(btEvent).p.meToken = token;
                MEC(btEvent).eType = token->eType;
                token->callback(&MEC(btEvent));
                return TRUE;
            }
            break;

        case BOP_LINK_CONNECT:
            if (tremDev == remDev)
            {
                MeReportLinkUp(tremDev, BTEVENT_LINK_CONNECT_CNF);
                return TRUE;
            }
            break;

        case BOP_LINK_ACCEPT:
        case BOP_LINK_REJECT:
            if (tremDev == remDev)
            {
                MeReportLinkUp(tremDev, BTEVENT_LINK_CONNECT_IND);
                return TRUE;
            }
            break;

        case BOP_LINK_DISCONNECT:
            if (tremDev == remDev)
            {
                /*
                 * We received the Disconnect_Complete from the remDev
                 * before we were able to start ours. Remove our
                 * Disconnect from the list.
                 */
                return TRUE;
            }
            break;

        #if BT_SECURITY == XA_ENABLED
        case BOP_AUTHENTICATE:
            if (secop->remDev == remDev)
            {
                SecReportAuthenticateResult(remDev);
                secop->op.opType = BOP_NOP;
                return TRUE;
            }
            break;

        case BOP_ENCRYPTION:
            if (secop->remDev == remDev)
            {
                remDev->encryptState = BES_START_ENCRYPT;
                SecReportEncryptionResult(remDev, 0);
                secop->op.opType = BOP_NOP;
                return TRUE;
            }
            break;
        #endif /* BT_SECURITY == XA_ENABLED */

        case BOP_SNIFF_MODE:
        case BOP_EXIT_SNIFF_MODE:
         case BOP_EXIT_PARK_MODE:
            if (tremDev == remDev)
            {
                tremDev->op.opType = BOP_LINK_DISCONNECT;
                MEC(btEvent).p.modeChange.curMode = BLM_ACTIVE_MODE;
                tremDev->mode = BLM_ACTIVE_MODE;

                ReportModeChange(tremDev);
                return TRUE;
            }
            break;

        case BOP_SWITCH_ROLE:
            if (tremDev == remDev)
            {
                tremDev->op.opType = BOP_LINK_DISCONNECT;
                ReportRoleChange(tremDev, BOP_SWITCH_ROLE);
                return TRUE;
            }
            break;

        #if NUM_SCO_CONNS > 0
        case BOP_SCO_CONNECT:
            if (scocon->scord == remDev)
            {
                ScoReportScoLinkUp(scocon, BTEVENT_SCO_CONNECT_CNF);
                return TRUE;
            }
            break;

        case BOP_SCO_ACCEPT:
        case BOP_SCO_REJECT:
            if (scocon->scord == remDev)
            {
                ScoReportScoLinkUp(scocon, BTEVENT_SCO_CONNECT_IND);
                return TRUE;
            }
            break;
        #endif /* NUM_SCO_CONNS > 0 */

#if (SPP_SCO_SUPPORT == XA_ENABLED) || (BT_READ_RSSI == XA_ENABLED)
        case BOP_READ_RSSI:
            if (&remDev->readRSSIOp == (BtOperation*) parm)
            {
                BtEvent event = MEC(btEvent);
                Report(("MeFailRemDevCommand(): BOP_READ_RSSI"));
                /* MEC(btEvent).errCode is set as BEC_NO_CONNECTION when entering this function */
                MEC(btEvent).p.rssi.remDev = remDev;
                MEC(btEvent).p.rssi.RSSI = 0;
                MEC(btEvent).eType = BTEVENT_READ_RSSI_COMPLETED;
                MeReportResults(BEM_ALL_EVENTS);
                MEC(btEvent) = event;
            }
            break;
#endif            
        case BOP_READ_WIFI_RSSI:
            if (&remDev->readWIFIRSSIOp == (BtOperation*) parm)
            {
#ifdef __BT_SINGLE_ANTENNA__
                BT_WMT_Send_RSSI_Indication(remDev->bdAddr.addr,  3, 0x00);
#endif
                return TRUE;
            }
            break;
        default:
            if (remDev)
            {
                if (&remDev->cancelOp == (BtOperation*)parm)            
                {
                    OS_Report("Remove cancelOp OP");
                    return TRUE;
                }            
                else if (&remDev->packetTypeOp == (BtOperation*)parm)            
                {
                    OS_Report("Remove packetTypeOp OP");
                    return TRUE;
                }
                else if (&remDev->remoteFeatureOp == (BtOperation*)parm)   
                {
                    OS_Report("Remove remoteFeatureOp OP");
                    return TRUE;
                }
                else if (&remDev->remoteVersionOp == (BtOperation*)parm)                   
                {
                    OS_Report("Remove remoteVersionOp OP");
                    return TRUE;
                }
                else if (&remDev->remoteClockOffsetOp == (BtOperation*)parm)                   
                {
                    OS_Report("Remove remoteClockOffsetOp OP");
                    return TRUE;
                }
                else if (&remDev->supervisionOp == (BtOperation*)parm)                  
                {
                    OS_Report("Remove supervisionOp OP");
                    return TRUE;
                }
#if defined (__BT_2_1_AUTO_FLUSH__)
                else if (&remDev->automaticallyFlushedOp == (BtOperation*)parm)    
                {
                    OS_Report("Remove automaticallyFlushedOp OP");
                    return TRUE;
                }
#endif
#if defined (__BT_2_1_SNIFF_SUBRATING__)
                else if (&remDev->sniffSubrateOp == (BtOperation*)parm)  
                {
                    OS_Report("Remove sniffSubrateOp OP");
                    return TRUE;
                }
#endif
                else if (&remDev->readTxPowerOp == (BtOperation*)parm)  
                {
                    OS_Report("Remove readTxPowerOp OP");
                    return TRUE;
                }
                else if (&remDev->qosOp == (BtOperation*)parm)                  
                {
                    OS_Report("Remove QOS OP");
                    return TRUE;
                }
                else
                {
                    /* Ignore others. */
                }
            }            
            break;
    }

    return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleRemNameReqTimeoutNoRspCancelNameReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleRemNameReqTimeoutNoRspCancelNameReq(EvmTimer *timer)
{
    BtStatus status;
    MeCommandToken *nameToken = (MeCommandToken*) timer->context;
    U8 tokenID = btbmUtilGetNameTokenID(nameToken);

    timer->func = 0;
    bt_trace(TRACE_GROUP_1, BTLOG_MEHANDLEREMNAMEREQTIMEOUTNORSPCANCELNAMEREQ_TRY_CANCEL_NAME_REQUEST_OF_TOKENxD , tokenID);
    status = ME_CancelGetRemoteDeviceName(nameToken);

    bt_trace(TRACE_GROUP_1, REMOTE_NAME_REQUEST_STATUSBT_DEBUGx02X, status);

    switch (status)
    {
        case BT_STATUS_SUCCESS:
        {
            BtEvent nameCbEvent;

            OS_MemSet((U8*) & nameCbEvent, 0, sizeof(nameCbEvent));
            bt_trace(TRACE_GROUP_1, REMOTE_NAME_CANCEL_SUCCESS);
            /*
             * Returned result is success, it has two possible situations: 
             * 1) Another nameTok for asking the same device will be executed later.
             *     For this nameTok, it wants to cancel name procedure issued by itself, so result with success is returned. 
             * 2) This nameTok is still on opList and has no chance to be issued while timer is expired, removes it from list successfully
             * For both cases, it should report event to nameTok's callback function 
             */
            nameCbEvent.p.meToken = nameToken;
            nameCbEvent.p.meToken->p.name.io.out.len = 0;
            /* Local name timer is expired, no name rsp from controller or no time to issue this nameTok */
            nameCbEvent.errCode = BEC_HOST_TIMEOUT;
            nameCbEvent.eType = BTEVENT_NAME_RESULT;
            nameToken->callback(&nameCbEvent);
        }
            break;
        case BT_STATUS_PENDING:
            bt_trace(TRACE_GROUP_1, REMOTE_NAME_CANCEL_PENDING);
            break;
        default:
            Assert(0);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleRemNameReqChipNoResponse
 * DESCRIPTION
 *  
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleRemNameReqChipNoResponse(EvmTimer *timer)
{
    MeCommandToken *nameToken;
    U8 tokenID = 0;

    nameToken = (MeCommandToken*) timer->context;
    tokenID = btbmUtilGetNameTokenID(nameToken);
    bt_trace(TRACE_GROUP_1, BTLOG_REQUEST_DEVICE_NAME_TIMEOUT_OF_TOKENxD , tokenID);
    timer->func = 0;
    MEC(btEvent).errCode = BEC_CONNECTION_TIMEOUT;
    MeReportMeCommandComplete(nameToken);
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleRemNameReqComplete
 * DESCRIPTION
 *  Handle the Remote Name Request Complete event.
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleRemNameReqComplete(U8 len, U8 *parm)
{
    I8 i;
    MeCommandToken token;

    /* UNUSED_PARAMETER(len); */

    bt_trace(TRACE_GROUP_1, MECON_REMOTE_NAME_REQUEST_COMPLETE_STATUS__0XxX, parm[0]);
    /* Get the error code */
    MEC(btEvent).errCode = parm[0];

    if (parm[0] == 0)
    {
        /*
         * No errors so find the length of the name and add a 0 at
         * the end if needed
         */
        for (i = 0; i < BT_MAX_REM_DEV_NAME; i++)
        {
            if (*(parm + i + 7) == 0)
            {
                break;
            }
        }

        if (i == BT_MAX_REM_DEV_NAME)
        {
            /* Add the null termination */
            *(parm + i + 7) = 0;
        }

        /* Add the output parameters to the token */
        token.p.name.io.out.len = (U8) i;
        token.p.name.io.out.name = parm + 7;
    }

    /*
     * Call all the callbacks with the results. We do this by setting up
     * a token and finding all that match
     */
    token.op.opType = BOP_REM_NAME_REQ;
    token.flags = MCTF_NEED_CHECK;
    token.eType = BTEVENT_NAME_RESULT;
    OS_MemCopy(token.p.name.bdAddr.addr, parm + 1, 6);
    BTBMDumpBdAddr(token.p.name.bdAddr.addr);
    #ifdef __BT_USE_CUST_DATA__
    if(parm[0] == 0){
        BtRemoteDevice *remDev = ME_FindRemoteDevice(&token.p.name.bdAddr);
        if(remDev){
            remDev->devId = btGetDevID((const char*)token.p.name.io.out.name);
            bt_prompt_trace(MOD_BT, "[CUST] get remDev id %u when name req completed", remDev->devId);
        }else{
            bt_prompt_trace(MOD_BT, "[CUST] can not get remDev when name req completed");
        }
    }
    #endif
    MeReportMeCommandComplete(&token);
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleModeChange
 * DESCRIPTION
 *  Handle a mode change event.
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleModeChange(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;

    /* UNUSED_PARAMETER(len); */

    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_ME_HANDLE_MOD_CHANGE, LEtoHost16(parm + 1), parm[3]);    
    /* Locate the remote device */
    remDev = MeMapHciToRemoteDevice((HciHandle) (LEtoHost16(parm + 1) & 0x0fff));
    if (remDev != 0)
    {
        MEC(btEvent).errCode = parm[0];
        MEC(btEvent).p.modeChange.curMode = parm[3];
        remDev->mode = parm[3];

        if (parm[0] == 0)
        {
            MEC(btEvent).p.modeChange.interval = LEtoHost16(parm + 4);
        }

        
        /* Set okToSend according to mode */
        if (parm[3] == BLM_ACTIVE_MODE)
        {
            remDev->okToSend = TRUE;
            HCI_RequestToSend(remDev->hciHandle);
        }
        else if (parm[3] == BLM_HOLD_MODE || parm[3] == BLM_PARK_MODE)
        {
            remDev->okToSend = FALSE;
        }

        /* Report the results */
        ReportModeChange(remDev);
    }

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleRoleChange
 * DESCRIPTION
 *  Handle a role change event.
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleRoleChange(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;

    /* UNUSED_PARAMETER(len); */
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_ME_EVENT_STATUS, parm[0]);

    /* Locate the remote device */
    remDev = ME_FindRemoteDeviceP(parm + 1);
    if (remDev != 0)
    {
        MEC(btEvent).errCode = parm[0];
        MEC(btEvent).p.roleChange.newRole = parm[7];
        remDev->role = parm[7];

        /* Report the results */
        ReportRoleChange(remDev, BOP_SWITCH_ROLE);
    }

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleDiscoverRoleTask
 * DESCRIPTION
 *  Handle HCI events for the Role Discovery Task.
 * PARAMETERS
 *  event       [IN]        
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void MeHandleDiscoverRoleTask(U8 event, U8 len, U8 *parm)
{
    U16 opcode = 0;
    BtRemoteDevice *remDev;

    /* Get a pointer to the remote device */
    remDev = ContainingRecord(MEC(curOp), BtRemoteDevice, roleOp);

    /* Save the status in case it is an error */
    opcode = LEtoHost16((parm + 1));
    MEC(btEvent).errCode = parm[3];

    /* Make sure the opcode matches. If not ignore the response */
    if (opcode == MEC(curTask))
    {
        /* Clear the task if this is the end. */
        MEC(curTask) = 0;

        /* Call the operation handler */
        if (MEC(btEvent).errCode == 0)
        {
            remDev->role = parm[6];
            MEC(opHandler) (BOE_TASK_END);
            /* Enable role switch if we are slave */
            if (remDev->role == BCR_SLAVE) 
            {
                remDev->LinkPolicyParms |= BLP_MASTER_SLAVE_SWITCH;
                if(!IsEntryAvailable(&(remDev->policyOp.node)))
                {
                    bt_trace(TRACE_GROUP_1,BTLOG_CAN_NOT_PERFORM_LINK_POLICY);
                }
                else
                {
                    remDev->policyOp.opType = BOP_WRITE_LINK_POLICY;
                    InsertHeadList(&MEC(opList), &(remDev->policyOp.node));
                }
            }
        }
        else
        {
            MEC(opHandler) (BOE_TASK_ERR);
        }

    }
    else
    {

        bt_trace(
            TRACE_GROUP_1,
            ME_MEHANDLEDISCOVERROLETASK_OPCODE_MISMATCH_OPCODE__xX_CURTASK__xX,
            opcode,
            MEC(curTask));
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleDiscoverRole
 * DESCRIPTION
 *  Discover the current role.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleDiscoverRole(BtOpEvent event)
{
    BtRemoteDevice *remDev;

    /* Get a pointer to the remote device */
    remDev = ContainingRecord(MEC(curOp), BtRemoteDevice, roleOp);

    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleDiscoverRoleTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            MeConVerifyHCIGenCommand();
            StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle);
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
            MeSendHciCommand(HCC_ROLE_DISCOVERY, 2);
            return;

        case BOE_TASK_ERR:
            /* The command failed. Assume we are what we expect. */
            /* remDev->role = (remDev->role == BCR_PSLAVE ? BCR_SLAVE : BCR_MASTER); */
            /* drop through */

        case BOE_TASK_END:
            MEC(channelOp).opType = BOP_NOP;

            /* Report the results */
            MEC(btEvent).p.roleChange.newRole = remDev->role;
            ReportRoleChange(remDev, BOP_DISCOVER_ROLE);

            /* Operation done. */
            bt_trace(TRACE_GROUP_1, ME_DISCOVER_ROLE_OPERATION_COMPLETED_ERROR__0XxX, MEC(btEvent).errCode);
            break;

        default:
            MeConHandleErrorTaskEnd();
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleReadRemoteFeature
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleReadRemoteFeature(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    U16 hci_handle;
    U8 i = 0;

    hci_handle = LEtoHost16(parm + 1);
    remDev = MeMapHciToRemoteDevice(LEtoHost16(parm + 1));
    if (parm[0] != 0)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_READ_REMOTE_SUPPORTED_FEATURE_FAILEDBT_DEBUGx02X, parm[0]);
    }
    else
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_READ_REMOTE_SUPPORTED_FEATURE_SUCCESSBT_DEBUGx04X, hci_handle);
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_SUPPORTED_FEATUREBT_DEBUGx02X_BT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02X_BT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02X,
            parm[3], parm[4], parm[5], parm[6], parm[7], parm[8], parm[9], parm[10]);
        if(remDev){
            OS_MemCopy((U8*) & remDev->remoteFeature, parm + 3, 8);
            if (!OS_TestBit(BDF_EV3_PACKET, remDev->remoteFeature))  
            {
                OS_Report("Remote does not support eSCO");
                remDev->supportESCO = 0;
            }
            if(OS_TestBit(BDF_EXTENDED_FEATURE, remDev->remoteFeature)){
                OS_Report("Ext feature is supported");
                MeReadRemoteSupportedExtFeature(remDev);
            }
        }else{
            OS_Report("[ME] Can not find remDev for hci handle %u", LEtoHost16(parm + 1));
        }
    }
    if(remDev){
        /* No matter the status is success or not, we set feature is get. So that the security can 
             Keep go on.
        */
        OS_SetBit(BT_REMOTE_LINK_INFO_FEATURE, remDev->remoteLinkInfo);
        if(!OS_TestBit(BDF_EXTENDED_FEATURE, remDev->remoteFeature)){
            SecProcessAllSecurityTokens(remDev);
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  MeHandleReadRemoteExtFeature
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleReadRemoteExtFeature(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    U16 hci_handle;
    U8 i = 0;

    hci_handle = LEtoHost16(parm + 1);
    remDev = MeMapHciToRemoteDevice(LEtoHost16(parm + 1));
    if (parm[0] != 0){
        OS_Report("[ME] Read remote ext feature failed : %u", parm[0]);
    }else{
        OS_Report("[ME] Read remote ext feature success : handle=%u", hci_handle);
        OS_Report("[ME] Ext feature is : %u, %u, %u, %u, %u, %u, %u, %u",
            parm[5], parm[6], parm[7], parm[8], parm[9], parm[10], parm[11], parm[12]);
        if(remDev){
            OS_MemCopy((U8*) & remDev->remoteExtFeature, parm + 5, 8);
            if(OS_TestBit(BDEF_LE_SUPPORT_HOST, remDev->remoteExtFeature)){
                DDB_DEV_TYPE_AddRecord(remDev->bdAddr, BT_DEV_TYPE_BR_EDR_LE); // Set to Dual
        }else{
                DDB_DEV_TYPE_AddRecord(remDev->bdAddr, BT_DEV_TYPE_BR_EDR);     // Set to BR EDR only
            }
        }else{
            OS_Report("[ME] Can not find remDev for hci handle %u", LEtoHost16(parm + 1));
        }
    }
    if(remDev){
        /* No matter the status is success or not, we set ext feature is get. So that the security can 
             Keep go on.
        */
        OS_SetBit(BT_REMOTE_LINK_INFO_EXT_FEATURE, remDev->remoteLinkInfo);
        SecProcessAllSecurityTokens(remDev);
    }
}

/*****************************************************************************
 * FUNCTION
 *  MeHandleReadRemoteVersion
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleReadRemoteVersion(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    U16 hci_handle;
    U8 i = 0;

    remDev = ContainingRecord(MEC(curOp), BtRemoteDevice, remoteVersionOp);
    bt_trace(TRACE_GROUP_1, R_VERSION_BT_DEBUGx02X, parm[0]);
    if (parm[0] == 0)
    {
        hci_handle = LEtoHost16(parm + 1);
        OS_Report("[ME] Read remote version success");
        //bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_READ_REMOTE_SUPPORTED_FEATURE_SUCCESSBT_DEBUGx04X, hci_handle);
#if 0    
        bt_trace(
            BT_TRACE_G1_PROTOCOL,
            BTLOG_SUPPORTED_FEATUREBT_DEBUGx02X_BT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02X_BT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02X,
            parm[3],
            parm[4],
            parm[5],
            parm[6],
            parm[7],
            parm[8],
            parm[9],
            parm[10]);
#endif
        for (i = 0; i < NUM_BT_DEVICES; i++)
        {
            remDev = &(MEC(devTable)[i]);
            if (remDev->state == BDS_CONNECTED)
            {
                /* We have found a device in the correct state */
                if (remDev->hciHandle == LEtoHost16(parm + 1))
                {
                    OS_MemCopy((U8*) & remDev->remoteVersion, parm + 3, 5);
                    DDB_VER_AddRecord(remDev->bdAddr, remDev->remoteVersion);
                    OS_SetBit(BT_REMOTE_LINK_INFO_VERSION, remDev->remoteLinkInfo);
                    break;
                }
            }
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  MeHandleReadRemoteClockOffset
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleReadRemoteClockOffset(U8 len, U8 *parm)
{
    BtRemoteDevice *remDev;
    U16 hci_handle;
    U8 i = 0;

    remDev = ContainingRecord(MEC(curOp), BtRemoteDevice, remoteClockOffsetOp);
    OS_Report("Read remote clock offset : status=%u", parm[0]);
    if (parm[0] == 0)
    {
        hci_handle = LEtoHost16(parm + 1);
        OS_Report("Read remote clock offset of handle=%u", hci_handle);
        for (i = 0; i < NUM_BT_DEVICES; i++)
        {
            remDev = &(MEC(devTable)[i]);
            if (remDev->state == BDS_CONNECTED)
            {
                /* We have found a device in the correct state */
                if (remDev->hciHandle == LEtoHost16(parm + 1))
                {
                    BtPageScanInfo psi;
                    if(DDB_PSI_FindRecord(remDev->bdAddr, &psi) != BT_STATUS_SUCCESS){
                        /* No record found. Set to default. */
                        psi.psMode = 0;
                        psi.psRepMode = 1;
                    }
                    psi.clockOffset = LEtoHost16(parm + 3);
                    DDB_PSI_AddRecord(remDev->bdAddr, &psi);
                    break;
                }
            }
        }
    }
}

#if defined (__BT_2_1_SUPERVISION_TIMEOUT_EVENT__)
/*****************************************************************************
 * FUNCTION
 *  MeHandleSupervisionTimeoutChange
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleSupervisionTimeoutChange(U8 len, U8 *parm)
{
    U16 hci_handle;
    U16 timeout;
    U8 i;
    BtRemoteDevice *remDev;

    hci_handle = LEtoHost16(parm);
    timeout = LEtoHost16(parm + 2);
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_ENHANCED_FLUSHED_COMMAND_COMPLETED_HDL, hci_handle, timeout);
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        remDev = &(MEC(devTable)[i]);
        if (remDev->state == BDS_CONNECTED)
        {
            /* We have found a pending remote device */
            if (remDev->hciHandle == hci_handle)
            {
                remDev->supervisionTimeout = timeout;
                break;
            }
        }
    }
}
#endif /* defined (__BT_2_1_SUPERVISION_TIMEOUT_EVENT__) */ 

#if defined (__BT_2_1_AUTO_FLUSH__)


/*****************************************************************************
 * FUNCTION
 *  MeHandleEnhancedFlush
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleEnhancedFlush(U8 len, U8 *parm)
{
    U16 hci_handle;

    hci_handle = LEtoHost16(parm);
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_ENHANCED_FLUSHED_COMMAND_COMPLETED_HDL, hci_handle);
}
#endif /* defined (__BT_2_1_AUTO_FLUSH__) */ 


/*****************************************************************************
 * FUNCTION
 *  MeDisconnectLink
 * DESCRIPTION
 *  Add a disconnect operation to queue.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeDisconnectLink(BtRemoteDevice *remDev, U8 index)
{
    /*
     * See if this link is already in the process of being disconnected
     * or coming up
     */
    BtDeviceContext *bdc;

    bdc = DS_FindDevice(&(remDev->bdAddr));
    
    OS_Report("MeDis from indx:%d", index);

    if ((bdc) 
#ifdef __BT_4_0_BLE__
	&& (bdc->devType == BT_DEV_TYPE_BR_EDR)
#endif
	   )
    {
        if((!HCI_IsValidAclPacketCount(remDev->maxTxCount)) && ((random() % 5) ==0))
        {
#ifdef __BT_DLIGHT1_DEBUG__
            Report(("autumn try to do MeDisconnectLink"));
#endif
            return;
        }
        if((remDev->remoteCapability > 0) && ((random() % 3) ==0))
        {
#ifdef __BT_DLIGHT1_DEBUG__
            Report(("autumn try to do MeDisconnectLink"));
#endif
            return;
        }
    }


    if (IsNodeOnList(&(MEC(opList)), &(remDev->op.node)))
    {
        /*
         * Item is on the list. If it is for a connect operation then simply 
         * remove it from the list. If it already being disconnected then 
         * we are done. 
         */
        if(!((remDev->op.opType != BOP_LINK_ACCEPT) && (remDev->op.opType != BOP_LINK_REJECT)))
        {
            Assert(0);
        }

        if ((remDev->op.opType >= BOP_HOLD_MODE) && (remDev->op.opType <= BOP_SWITCH_ROLE))
        {
            /*
             * We have a mode/role change operation on the queue. Remove it
             * and disconnect the link.
             */
            remDev->op.opType = BOP_NOP;
            RemoveEntryList(&(remDev->op.node));
        }
        else
        {
            if (remDev->op.opType == BOP_LINK_CONNECT)
            {
                /*
                 * Remove the operation from the list and make it free. There 
                 * should be no handlers since it could not have gotten this 
                 * far if there was.
                 */
                if(!(IsListEmpty(&(remDev->handlers)) && (remDev->refCount == 0)))
                {
                    Assert(0);
                }
                
                RemoveEntryList(&(remDev->op.node));

                if(MEC(pendCons) == 0)
                {
                    Assert(0);
                }

                MEC(pendCons) -= 1;

                MeFreeRemoteDevice(remDev);
            }
            return;
        }
    }

    /*
     * If the connection is in the process of being disconnected then do nothing otherwise
     * it must be active so we need to put the disconnect operation on the queue.
     */
    if (remDev->state < BDS_CONNECTED)
    {
        Assert(0);
    }
    EVM_ResetTimer(&remDev->readyToSendTimer);
    bt_trace(
        TRACE_GROUP_1,
        BTLOG_MEDISCONNECTLINKx02Xx02Xx02X,
        remDev->state,
        remDev->disconnectACLCheckCreatACLCount,
        MEC(pendCons));

    if (remDev->state == BDS_CONNECTED)
    {
        if ((remDev->disconnectACLCheckCreatACLCount < 6) && (MEC(pendCons) != 0))
        {
            remDev->disconnectACLCheckCreatACLTimer.func = MeHandleDisconnectACLCheckCreateACL;
            remDev->disconnectACLCheckCreatACLTimer.context = remDev;
            EVM_StartTimer(&remDev->disconnectACLCheckCreatACLTimer, 1000);
            return;
        }

        /* Were connected so we need to disconnect */
        remDev->state = BDS_OUT_DISC;
        remDev->okToSend = FALSE;

        /*
         * If we are performing a mode change operation then we have to wait
         * until it is done before we can actually disconnect. In this case
         * we will set the state and wait for the mode change operation to
         * occur then we will disconnect.
         */
        if (remDev->op.opType == BOP_NOP)
        {
            /*
             * No operation is in progress so put the disconnect operation 
             * on the queue.
             */

            //Be sure thar the remDev->op is not in MEC(opList):  
            //otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
            //For later, to execute this opType will assert due to opType is equal zero!
#if 0
            if(IsNodeOnList(&MEC(opList), &(remDev->op.node)))
            {
                Assert(0);
            }
#endif
            MeConVerifyRemDevOpOnList(remDev);
            remDev->op.opType = BOP_LINK_DISCONNECT;
            MeAddOperation(remDev);
            MeStartOperation();
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeReportLinkUp
 * DESCRIPTION
 *  Report results of an ACL link coming up.
 * PARAMETERS
 *  remDev      [IN]        
 *  eType       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeReportLinkUp(BtRemoteDevice *remDev, BtEventType eType)
{
    BtEventMask mask;
    BtOpType saveOp;
    BtErrorCode errCode;
    BtEventType old_eType;

    mask = BEM_LINK_CONNECT_CNF;

    saveOp = remDev->op.opType;

    //Be sure thar the remDev->op is not in MEC(opList):  
    //otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
    //For later, to execute this opType will assert due to opType is equal zero!
#if 0
    if(IsNodeOnList(&MEC(opList), &(remDev->op.node)))
    {
        Assert(0);
    }
#endif
    MeConVerifyRemDevOpOnList(remDev); 
    remDev->op.opType = BOP_NOP;

    /* If an auto-role-switch is necessary, this is a good place to put it. */

    /*
     * If we have an accept handler and the operation is reject or accept
     * then call the accept handler with the results.
     */
    if ((MEC(acceptHandler) != 0) && ((saveOp == BOP_LINK_ACCEPT) || (saveOp == BOP_LINK_REJECT)))
    {
        MEC(btEvent).p.remDev = remDev;
        MEC(btEvent).handler = MEC(acceptHandler);

        if (saveOp == BOP_LINK_ACCEPT)
        {
            MEC(btEvent).eType = BTEVENT_LINK_ACCEPT_RSP;
        }
        else
        {
            if(saveOp != BOP_LINK_REJECT)
            {
                Assert(0);
            }
            MEC(btEvent).eType = BTEVENT_LINK_REJECT_RSP;
        }
        MEC(acceptHandler)->callback(&MEC(btEvent));
    }

    /*
     * If we were rejecting the link then success does not mean the link
     * is up. It actually means the link is down. We will set the errorCode
     * so the link does not come up.
     */
    if (saveOp == BOP_LINK_REJECT)
    {
        bt_trace(TRACE_GROUP_1, MECON_SETTING_ERROR_FOR_LINK_REJECT_OLD_ERROR__0XxX, MEC(btEvent).errCode);
        MEC(btEvent).errCode = BEC_LOCAL_TERMINATED;
    }

    /*
     * We need to call the bound handlers. Any connection inbound or
     * outbound will result in the same event being passed to the bound
     * handlers. Setup the things common to either success or failure.
     */
    MEC(btEvent).eType = BTEVENT_LINK_CONNECT_CNF;
    MEC(btEvent).p.remDev = remDev;

    if (MEC(btEvent).errCode == 0)
    {
        /*
         * Operation succeeded. Put the proper value into the Event and
         * report the event to all bound handlers.
         */
        /* remDev->okToSend = TRUE; */
        MeSuspendPacket(remDev, 40);
        /*
        EVM_ResetTimer(&remDev->readyToSendTimer);
        remDev->readyToSendTimer.func = MeLinkReadyToSend;
        remDev->readyToSendTimer.context = remDev;
        EVM_StartTimer(&remDev->readyToSendTimer, 40);
        bt_trace(TRACE_GROUP_1, MELINKREADYTOSEND_START_TIMER);
        */
        remDev->state = BDS_CONNECTED;
        /*
         * If we are in security mode 3 then set authState. The encryptState
         * is set in MeHandleConnectComplete.
         */
        if (MEC(secModeState) == BSMS_LEVEL3)
        {
            remDev->authState = BAS_AUTHENTICATED;
        }

        /* Update activeCons and send event to all bound handlers */
        MEC(activeCons) += 1;
#ifdef __BT_SINGLE_ANTENNA__
        BT_WMT_ACL_Send_Status(remDev, 1);
#endif
        if(MEC(activeCons) == 0)
        {
            Assert(0);
        }
        MeCallLinkHandlers(remDev, &MEC(btEvent));
    }
    else
    {
        /* Operation failed. Report the error. */
        remDev->state = BDS_OUT_DISC;
        /* CallAndRemoveHandlers(remDev); */
    }
    old_eType = MEC(btEvent).eType;
    /* The connect operation is complete so report results to all global handlers */
    MEC(btEvent).eType = eType;

    if (eType == BTEVENT_LINK_CONNECT_IND)
    {
        mask = BEM_LINK_CONNECT_IND;
    }

    errCode = MEC(btEvent).errCode;
    MeReportResults(mask);

    /* If an error occured set the state to disconnected */
    if (errCode != 0)
    {
        MEC(btEvent).eType = old_eType;
        MEC(btEvent).errCode = errCode;
        CallAndRemoveHandlers(remDev);
        MeFreeRemoteDevice(remDev);
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeLinkReadyToSend
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void MeLinkReadyToSend(EvmTimer *Timer)
{
    BtRemoteDevice *link;

    link = (BtRemoteDevice*) Timer->context;
    link->readyToSendTimer.func = 0;
    bt_trace(TRACE_GROUP_1, MELINKREADYTOSENDBT_DEBUGx02X, link->state);
    if (link->state == BDS_CONNECTED)
    {
        link->okToSend = TRUE;
        if (link->mode == BLM_ACTIVE_MODE)
        {
            HCI_RequestToSend(link->hciHandle);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  ReportModeChange
 * DESCRIPTION
 *  Report results of a mode change.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void ReportModeChange(BtRemoteDevice *remDev)
{
    if ((remDev->state == BDS_OUT_DISC) && (remDev->op.opType != BOP_LINK_DISCONNECT))
    {
        /*
         * A disconnect request occured while trying to enter a mode change
         * so we need to disconnect. Add a disconnect operation to the queue.
         */

        //Be sure thar the remDev->op is not in MEC(opList):  
        //otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
        //For later, to execute this opType will assert due to opType is equal zero!
#if 0
        if(IsNodeOnList(&MEC(opList), &(remDev->op.node)))
        {
            Assert(0);
        }
#endif
        MeConVerifyRemDevOpOnList(remDev);
        remDev->op.opType = BOP_LINK_DISCONNECT;
        MeAddOperation(remDev);
        MeStartOperation();
    }
    else
    {
        /*
         * Only reset the opType if the requested operation is a
         * mode change operation.
         */
        if ((remDev->op.opType >= BOP_HOLD_MODE) && (remDev->op.opType <= BOP_EXIT_PARK_MODE))
        {
            switch (remDev->op.opType)
            {
                case BOP_PARK_MODE:
                    remDev->op.opType = BOP_NOP;
                    break;
                case BOP_HOLD_MODE:
                    remDev->op.opType = BOP_NOP;
                    break;
                case BOP_SNIFF_MODE:
                    if (remDev->mode == BLM_SNIFF_MODE)
                    {
                        if (IsNodeOnList(&MEC(opList), &(remDev->op.node)))
                        {
                            RemoveEntryList(&(remDev->op.node));
                        }
                        remDev->op.opType = BOP_NOP;
                    }
                    break;
                case BOP_EXIT_SNIFF_MODE:
                case BOP_EXIT_PARK_MODE:
                    if (remDev->mode == BLM_ACTIVE_MODE)
                    {
                        if (IsNodeOnList(&MEC(opList), &(remDev->op.node)))
                        {
                            RemoveEntryList(&(remDev->op.node));
                        }
                        remDev->op.opType = BOP_NOP;
                    }
                    break;
            }

            //Be sure thar the remDev->op is not in MEC(opList):  
            //otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
            //For later, to execute this opType will assert due to opType is equal zero!
#if 0
            if(IsNodeOnList(&MEC(opList), &(remDev->op.node)))
            {
                Assert(0);
            }
#endif
            MeConVerifyRemDevOpOnList(remDev);
            remDev->op.opType = BOP_NOP;
        }
    }

    /* We need to call the bound handlers. */
    MEC(btEvent).eType = BTEVENT_MODE_CHANGE;
    MEC(btEvent).p.modeChange.remDev = remDev;

    /* Report to bound handler */
    MeCallLinkHandlers(remDev, &MEC(btEvent));

    /* Report to global handlers */
    MeReportResults(BEM_MODE_CHANGE);
}


/*****************************************************************************
 * FUNCTION
 *  ReportRoleChange
 * DESCRIPTION
 *  Report results of a role change.
 * PARAMETERS
 *  remDev      [IN]        
 *  opType      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void ReportRoleChange(BtRemoteDevice *remDev, BtOpType opType)
{
    if ((remDev->state == BDS_OUT_DISC) && (remDev->op.opType != BOP_LINK_DISCONNECT) && (opType == BOP_SWITCH_ROLE))
    {
        /*
         * A disconnect request occured while trying to enter a mode change
         * so we need to disconnect. Add a disconnect operation to the queue.
         */

        /* In this case, we can know that a disconnection operation is pending util the end of this role switch operation */
#if 0
        if(IsNodeOnList(&MEC(opList), &(remDev->op.node)))
        {
            Assert(0);
        }
#endif
        MeConVerifyRemDevOpOnList(remDev); 
        remDev->op.opType = BOP_LINK_DISCONNECT;

        MeAddOperation(remDev);
        MeStartOperation();
    }
    else if (opType == BOP_SWITCH_ROLE)
    {
        /* Only clear the opType if we were performing a role switch */
        if (remDev->op.opType == BOP_SWITCH_ROLE)
        {

            //Be sure thar the remDev->op is not in MEC(opList):  
            //otherwise its opType is reset here and it(ie., remDev->op) still exists in MEC(opList),
            //For later, to execute this opType will assert due to opType is equal zero!
#if 0
            if(IsNodeOnList(&MEC(opList), &(remDev->op.node)))
            {
                Assert(0);
            }
#endif
            if (IsNodeOnList(&MEC(opList), &(remDev->op.node))){
                // Remote do role change first
                OS_Report("ReportRoleChange : remote do role change first, err=%u", MEC(btEvent).errCode);
                if(MEC(btEvent).errCode == 0){
                    RemoveEntryList(&remDev->op.node);                    
                }else{
                    return;
                }
            }else if(MEC(curOp) == &remDev->op){
                OS_Report("ReportRoleChange : role change success before command status of role change command. Delay report until command status");
                return;
            }
            //MeConVerifyRemDevOpOnList(remDev); 
            remDev->op.opType = BOP_NOP;
        }
    }
    else if (opType == BOP_DISCOVER_ROLE)
    {

        if (remDev->roleOp.opType == BOP_DISCOVER_ROLE)
        {
            //Be sure thar the remDev->roleOp is not in MEC(opList):  
            //otherwise its opType is reset here and it(ie., remDev->roleOp) still exists in MEC(opList),
            //For later, to execute this opType will assert due to opType is equal zero!
            if(IsNodeOnList(&MEC(opList), &(remDev->roleOp.node)))
            {
                Assert(0);
            }
            remDev->roleOp.opType = BOP_NOP;
        }

    }

    /* Do not report a role switch if we are in the middle of connecting */
    if (remDev->state >= BDS_CONNECTED)
    {
        /* We need to call the bound handlers. */
        MEC(btEvent).eType = BTEVENT_ROLE_CHANGE;
        MEC(btEvent).p.roleChange.remDev = remDev;
        bt_trace(TRACE_GROUP_1, BTLOG_BTBM_NEW_ROLE_IS_REPORTED_WITH_OPTYPE, remDev->role, remDev->hciHandle, opType);      /* role=0: Master */
        /* Report to bound handler */
        MeCallLinkHandlers(remDev, &MEC(btEvent));

        /* Report to global handlers */
        MeReportResults(BEM_ROLE_CHANGE);
    }
}


/*****************************************************************************
 * FUNCTION
 *  CallAndRemoveHandlers
 * DESCRIPTION
 *  Call the handlers bound to a BtRemoteDevice and remove
 *  them from the BtRemoteDevice's handler list.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void CallAndRemoveHandlers(BtRemoteDevice *remDev)
{
    BtHandler *handler, *nextHandler;

    remDev->state = BDS_DISCONNECTED;
    bt_trace(TRACE_GROUP_1, CALLANDREMOVEHANDLERS);
    if (IsListCircular(&(remDev->handlers)) == FALSE)
    {
        Assert(0);
    }

    handler = (BtHandler*)GetHeadList(&(remDev->handlers));
    while (handler != (BtHandler*)(&(remDev->handlers))){
        bt_trace(TRACE_GROUP_1, CALLANDREMOVEHANDLERS_1);
        nextHandler = (BtHandler*)GetNextNode(&(handler->node));
        /* ME Link Pending */
        if(!handler->pending){
            RemoveEntryList(&(handler->node)); 
            MEC(btEvent).handler = handler;
            handler->callback(&MEC(btEvent));
        }else{
            OS_Report("[ME][CON] pending handler, did not callback");
        }
        handler = nextHandler;
    }
    if(!IsListCircular(&(remDev->handlers)))
    {
        Assert(0);
    }    
}

#if L2CAP_NUM_GROUPS > 0


/*****************************************************************************
 * FUNCTION
 *  MeSuspendTransmitters
 * DESCRIPTION
 *  Suspends transmission on all links. Used when a broadcast packet
 *  is queue'd for transmit.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeSuspendTransmitters(void)
{
    I8 i;

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if (MEC(devTable)[i].state == BDS_CONNECTED)
        {
            MEC(devTable)[i].okToSend = FALSE;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeRestartTransmitters
 * DESCRIPTION
 *  Resumes transmission on all links. Used after the broadcast
 *  packet has been transmitted.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeRestartTransmitters(void)
{
    I8 i;

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if (MEC(devTable)[i].state == BDS_CONNECTED)
        {
            MEC(devTable)[i].okToSend = TRUE;

            if (!IsListEmpty(&(MEC(devTable)[i].txLowPriorityQueue)))
            {
                HCI_RequestToSend(MEC(devTable)[i].hciHandle);
            }
        }
    }
}
#endif /* L2CAP_NUM_GROUPS > 0 */


/*****************************************************************************
 * FUNCTION
 *  MeEnumerateRemoteDevices
 * DESCRIPTION
 *  Return a pointer to a remote device structure based on an
 *  interator. 0 is returned if the iterator exceeds the number of
 *  remote device structures.
 * PARAMETERS
 *  i       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtRemoteDevice *MeEnumerateRemoteDevices(I8 i)
{
    if (i < NUM_BT_DEVICES)
    {
        return &(MEC(devTable)[i]);
    }
    else
    {
        return 0;
    }
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleAclConComplete
 * DESCRIPTION
 *  Handle an ACL connection complete event.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleAclConComplete(BtRemoteDevice *remDev)
{
#ifndef MTK_BB_LOOPBACK_MODE
    BtEventType eType;

#if defined (__BT_2_1_SIMPLE_PAIRING__)
    BtDeviceRecord record;
#endif 

    /* Determine the proper event to send.  */
    if(!((remDev->state == BDS_IN_CON) ||
           (remDev->state == BDS_OUT_CON) ||
           (remDev->op.opType == BOP_LINK_CONNECT) ||
           (remDev->op.opType == BOP_LINK_ACCEPT) ||
           (remDev->op.opType == BOP_LINK_REJECT) || (remDev->op.opType == BOP_LINK_IN_REJECT)))
    {
        Assert(0);
    }

    bt_trace(TRACE_GROUP_1, MEHANDLEACLCONCOMPLETE);
    if ((remDev->op.opType == BOP_LINK_REJECT) || (remDev->op.opType == BOP_LINK_IN_REJECT))
    {
        remDev->op.opType = 0;
    }
    if (remDev->op.opType == BOP_LINK_CONNECT)
    {
        eType = BTEVENT_LINK_CONNECT_CNF;
        remDev->LinkPolicyParms = MEC(outAclPolicy);
    }
    else
    {
        eType = BTEVENT_LINK_CONNECT_IND;
        remDev->LinkPolicyParms = MEC(inAclPolicy);
    }

    /*
     * If a link has come up then we need to queue a policy command and an
     * accessibility command.
     */
    if ((MEC(btEvent).errCode == 0) && (remDev->op.opType != BOP_LINK_REJECT))
    {

        /* Queue the accessibility command */
    #if defined (__BT_2_1_SIMPLE_PAIRING__)
        if (SEC_FindDeviceRecord(&(remDev->bdAddr), &record) == BT_STATUS_SUCCESS)
        {
            remDev->gapKeyType = record.gapKeyType;
        }
        else
        {
            remDev->gapKeyType = BT_LINK_KEY_GAP_TYPE_NO_KEY;
        }
        bt_trace(TRACE_GROUP_1, BTLOG_CURRENT_KEYxD, remDev->gapKeyType);
    #endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 

        /* Queue the policy command */
        {
            U8 piconet_no = 0, scatternet_no = 0, sco_no = 0, esco_no = 0, msco_no = 0, ssco_no = 0;
            BTBMGetScattnetStatus(&piconet_no, &scatternet_no, &sco_no, &esco_no, &msco_no, &ssco_no);
            if (btbmCheckBondingState())
            {
                remDev->LinkPolicyParms = BLP_DISABLE_ALL;
            }
            else if ((sco_no != 0) || (esco_no != 0))
            {
                remDev->LinkPolicyParms &= ~BLP_MASTER_SLAVE_SWITCH;
            }

            remDev->policyOp.opType = BOP_WRITE_LINK_POLICY;
            if(!IsEntryAvailable(&(remDev->policyOp.node)))
            {
                Assert(0);
            }
            InsertHeadList(&MEC(opList), &(remDev->policyOp.node));
        }
        if (remDev->op.opType == BOP_LINK_ACCEPT)
        {
            /* Queue a role discovery */

            /*
             * Be sure thar the remDev->roleOp is not in MEC(opList):  
             * otherwise its opType is reset here and it(ie., remDev->roleOp) still exists in MEC(opList),
             * For later, to execute this opType will assert due to opType is equal zero!
             */
            if(IsNodeOnList(&MEC(opList), &(remDev->roleOp.node)))
            {
                Assert(0);
            }
            remDev->roleOp.opType = BOP_DISCOVER_ROLE;
            if(!IsEntryAvailable(&(remDev->roleOp.node)))
            {
                Assert(0);
            }
            InsertHeadList(&MEC(opList), &(remDev->roleOp.node));
            /* remDev->role = (remDev->role == BCR_SLAVE ? BCR_PSLAVE : BCR_PMASTER); */
        }

        /* Attempt to start the operation */
        MeStartOperation();
    }
    else if (IsNodeOnList(&MEC(opList), &(remDev->op.node)) == TRUE)
    {
        /*
         * remove aborted operation from opList (should this be
         * limited to accept/reject operations only?)
         */
        RemoveEntryList(&remDev->op.node);
    }

#if BT_SECURITY == XA_ENABLED
    if (remDev->authState == BAS_WAITING_FOR_PIN || remDev->authState == BAS_WAITING_FOR_PIN_R)
    {
        /* Cancel the PIN request */
        SecReportAuthenticateResult(remDev);
    }
#endif /* BT_SECURITY == XA_ENABLED */

    /* Report the results of the link up */
    MeReportLinkUp(remDev, eType);
#else /* MTK_BB_LOOPBACK_MODE */ 
    BtEventType eType;

    bt_trace(TRACE_GROUP_1, MEHANDLEACLCONCOMPLETE);
    if ((remDev->op.opType == BOP_LINK_REJECT) || (remDev->op.opType == BOP_LINK_IN_REJECT))
    {
        remDev->op.opType = 0;
    }
    if (remDev->op.opType == BOP_LINK_CONNECT)
    {
        eType = BTEVENT_LINK_CONNECT_CNF;
        if (btbmCheckBondingState())
        {
            remDev->parms.policy = BLP_DISABLE_ALL;
        }
        else
        {
            remDev->parms.policy = MEC(outAclPolicy);
        }
    }
    else
    {
        eType = BTEVENT_LINK_CONNECT_IND;
        remDev->parms.policy = MEC(inAclPolicy);
    }
    /* Report the results of the link up */
    MeReportLinkUp(remDev, eType);

#endif /* MTK_BB_LOOPBACK_MODE */ 
}


/*****************************************************************************
 * FUNCTION
 *  MeCheckModeChange
 * DESCRIPTION
 *  Common error checking for mode change API functions.
 * PARAMETERS
 *  remDev      [IN]        
 *  bop         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus MeCheckModeChange(BtRemoteDevice *remDev, BtOpType bop)
{
    BtLinkMode preMode;

#if XA_ERROR_CHECK == XA_ENABLED
    if (remDev == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    /* Make sure the stack does not have an HCI initialization error */
    if (MEC(stackState) >= BTSS_INITIALIZE_ERR)
    {
        return BT_STATUS_HCI_INIT_ERR;
    }

    if (bop == BOP_EXIT_PARK_MODE)
    {
        preMode = BLM_PARK_MODE;
    }
    else if (bop == BOP_EXIT_SNIFF_MODE)
    {
        preMode = BLM_SNIFF_MODE;
    }
    else
    {
        preMode = BLM_ACTIVE_MODE;
    }

    /* Must be connected and in the correct mode for the operation. */
    if ((remDev->state != BDS_CONNECTED) || (remDev->mode != preMode))
    {
        /* Not in the right state */
        return BT_STATUS_FAILED;
    }

    /* The link must not in the process of performing an operation. */
    if (remDev->op.opType != BOP_NOP)
    {
        return BT_STATUS_IN_PROGRESS;
    }

    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleDataReqWhileNotActive
 * DESCRIPTION
 *  Deliver data request while not active event to global handlers.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleDataReqWhileNotActive(BtRemoteDevice *remDev)
{
    /* Backup global event to avoid clobbering possible event in progress. */
    BtEvent event = MEC(btEvent);

    MEC(btEvent).eType = BTEVENT_ACL_DATA_NOT_ACTIVE;
    MEC(btEvent).p.remDev = remDev;

    MeReportResults(BEM_ALL_EVENTS);

    MEC(btEvent) = event;
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleSpecialCreateLinkProc
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleSpecialCreateLinkProc(EvmTimer *Timer)
{
    BtRemoteDevice *remDev;
    U8 i = 0;

    remDev = (BtRemoteDevice*) Timer->context;
    remDev->connectTimer.func = 0;
    remDev->connectCounter++;
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_REMDEVCONNECTCOUNTERxD, remDev->connectCounter);
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        if ((MEC(devTable)[i].state == BDS_OUT_DISC) || (MEC(devTable)[i].state == BDS_OUT_DISC2))
        {
            if (remDev->connectCounter < 8)
            {
                bt_trace(TRACE_GROUP_1, BTLOG_ACL_DISCONNECTING_CREATING_LINK_PENDING);
                EVM_ResetTimer(&remDev->connectTimer);
                remDev->connectTimer.func = MeHandleSpecialCreateLinkProc;
                remDev->connectTimer.context = remDev;
                EVM_StartTimer(&remDev->connectTimer, 1000);
            }
            else
            {
                MeHandleConnectLocalReport(remDev, 0x01);
            }
            return;
        }
    }
#if defined (__BT_CHIP_VERSION_1__)
    MeConVerifyRemDevOpOnList(remDev);
    remDev->op.opType = BOP_LINK_CONNECT;
    remDev->restricted = FALSE;
    remDev->role = BCR_MASTER;
    {
        U8 piconet_no, scatternet_no, sco_no, esco_no, msco_no, ssco_no;

        BTBMGetScattnetStatus(&piconet_no, &scatternet_no, &sco_no, &esco_no, &msco_no, &ssco_no);
        if (msco_no)
        {
            remDev->parms.con.allowRoleChange = 0x00;
        }
        else
        {
            remDev->parms.con.allowRoleChange = 0x01;
        }
        bt_trace(TRACE_GROUP_1, BTLOG_ALLOW_ROLE_CHANGE_PARMxD , remDev->parms.con.allowRoleChange);
    }
    if(!IsEntryAvailable(&(remDev->op.node)))
    {
        Assert(0);
    }
    MeAddOperation(remDev);
    /* Attempt to start the operation */
    MeStartOperation();
#else /* defined (__BT_CHIP_VERSION_1__)*/
    if (remDev->connectCounter < 8)
    {
        if (BTBMCheckSpecialSlaveProcedure())
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_RESTART_CONNECTTIMER);
            EVM_ResetTimer(&remDev->connectTimer);
            remDev->connectTimer.func = MeHandleSpecialCreateLinkProc;
            remDev->connectTimer.context = remDev;
            EVM_StartTimer(&remDev->connectTimer, 1000);
        }
        else
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_MAKE_CREATION_CONNECTION);
#if 0
            if (IsNodeOnList(&MEC(opList), &(remDev->op.node)))
            {
                Assert(0);
            }
#endif
            MeConVerifyRemDevOpOnList(remDev);
            remDev->op.opType = BOP_LINK_CONNECT;
            remDev->restricted = FALSE;
            remDev->role = BCR_MASTER;
            {
                U8 piconet_no, scatternet_no, sco_no, esco_no, msco_no, ssco_no;

                BTBMGetScattnetStatus(&piconet_no, &scatternet_no, &sco_no, &esco_no, &msco_no, &ssco_no);
                if (msco_no)
                {
                    remDev->parms.con.allowRoleChange = 0x00;
                }
                else
                {
                    remDev->parms.con.allowRoleChange = 0x01;
                }
                bt_trace(TRACE_GROUP_1, BTLOG_ALLOW_ROLE_CHANGE_PARMxD , remDev->parms.con.allowRoleChange);
            }
            if (!IsEntryAvailable(&(remDev->op.node)))
            {
                Assert(0);
            }
            MeAddOperation(remDev);
            /* Attempt to start the operation */
            MeStartOperation();
        }
    }
    else
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_CONNECTCOUNTER_EXCEEDS_THRESHOULD);
        MeHandleConnectLocalReport(remDev, 0x01);
    }

#endif /* defined (__BT_CHIP_VERSION_1__)*/

}


/*****************************************************************************
 * FUNCTION
 *  MeHandleConnectLocalReport
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev          [IN]        
 *  error_code      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleConnectLocalReport(BtRemoteDevice *remDev, U8 error_code)
{
    U8 parms[11];

    if (error_code != 0)
    {
        /* Report failed */
#if 0
        if(IsNodeOnList(&MEC(opList), &(remDev->op.node)))
        {
            Assert(0);
        }
#endif
        MeConVerifyRemDevOpOnList(remDev);
        remDev->op.opType = BOP_LINK_CONNECT;
        parms[0] = HC_STATUS_CHIP_LIMITATION;
        parms[1] = 0xFF;
        parms[2] = 0x0E;
        OS_MemCopy(&parms[3], remDev->bdAddr.addr, 6);
        parms[9] = 0x01;
        parms[10] = 0x00;
        MeHandleConnectComplete(11, parms);
        remDev->op.opType = BOP_NOP;
    }
}

#if !defined (__BT_CHIP_VERSION_0__)


/*****************************************************************************
 * FUNCTION
 *  MeHandleAclLinkChangePacketType
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void MeHandleAclLinkChangePacketType(EvmTimer *Timer)
{
    BtRemoteDevice *link;
bt_prompt_trace(MOD_BT, "[BT] MeHandleAclLinkChangePacketType");
    link = (BtRemoteDevice*) Timer->context;
    if (link->state == BDS_CONNECTED)
    {
        MeChangePacketType(link);
    }
}
#endif /* !defined (__BT_CHIP_VERSION_0__) */


/*****************************************************************************
 * FUNCTION
 *  MeHandleDisconnectACLCheckCreateACL
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void MeHandleDisconnectACLCheckCreateACL(EvmTimer *Timer)
{
    BtRemoteDevice *link;

    link = (BtRemoteDevice*) Timer->context;
    bt_trace(TRACE_GROUP_1, BTLOG_MEHANDLEDISCONNECTACLCHECKCREATEACL);
    bt_trace(
        TRACE_GROUP_1,
        BTLOG_MEDISCONNECTLINKx02Xx02Xx02X,
        link->state,
        link->disconnectACLCheckCreatACLCount,
        MEC(pendCons));
    if (link->state == BDS_CONNECTED)
    {
        link->disconnectACLCheckCreatACLCount++;
        MeDisconnectLink(link, 6);
    }
}

static void MeConHandleErrorTaskEnd(void)
{
    Assert(0); 
}


static void MeConVerifyHCIGenCommand(void)
{
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }

}

static void MeConVerifyRemDevOpOnList(BtRemoteDevice* remDev)
{
    if (IsNodeOnList(&MEC(opList), &(remDev->op.node)))
    {
        Assert(0);
    }
}

void MeSuspendPacket(BtRemoteDevice *remDev, U32 time){
    remDev->okToSend = FALSE;
    OS_Report("Suspend packet for %u ms", time);
    EVM_ResetTimer(&remDev->readyToSendTimer);        
    remDev->readyToSendTimer.func = MeLinkReadyToSend;
    remDev->readyToSendTimer.context = remDev;
    EVM_StartTimer(&remDev->readyToSendTimer, time);
    bt_trace(TRACE_GROUP_1, MELINKREADYTOSEND_START_TIMER);
}

