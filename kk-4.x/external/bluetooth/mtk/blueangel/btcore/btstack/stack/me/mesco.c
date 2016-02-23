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
 *     $Workfile:mesco.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:   These are the functions called internally by ME clients 
 *                to manage and control SCO data links.
 *
 * Created:       March 28, 2000
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
#include "bt_common.h"
#include "btalloc.h"
#include "ddb.h"
#include "bt_feature.h"
#ifdef __BT_SINGLE_ANTENNA__
#include "bt_wmt_api.h"
#endif
#if NUM_SCO_CONNS > 0
#include "mesco.h"
#include "x_file.h"
#include "stdlib.h"


BtScoConnect *ScoGetFreeConnect(void);
static void ScoFailPendingCommands(BtScoConnect *scocon);
static BOOL ScoFailCommand(BtScoConnect *scocon, MeCommandToken *token);
static void MeSCOVerifyHCIGenCommand(void);

/****************************************************************************
 *
 * extern Functions
 *
 ****************************************************************************/
extern void BTBMLocalWriteScanEnabled(U8 mode);
extern U16 BT_XFile_EncryptionCommandOpcode(U16 opcode);
extern void Me_ScoHandleVoiceSettings(BtOpEvent event);
extern U8 Me_WriteAudioSetting(void);

/*****************************************************************************
 * FUNCTION
 *  StartScoLinkSetupTask
 * DESCRIPTION
 *  Start the BT v1.2 SCO link setup task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartScoLinkSetupTask(void)
{
    BtRemoteDevice *remDev;
    BtScoConnect *scocon;
    BtScoTxParms *scoTxParms;

    scocon = (BtScoConnect*) MEC(curOp);
    remDev = scocon->scord;
    scoTxParms = &scocon->scoTxParms;
    bt_trace(TRACE_GROUP_1, STARTSCOLINKSETUPTASK);
    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    if (scocon->scostate != BDS_CONNECTED)
    {
        bt_trace(TRACE_GROUP_1, MESCO_STARTING_ESCO_LINK_TO_ACL_LINK_xX, remDev->hciHandle);
        /* ACL Connection handle */
        StoreLE16(MEC(hciGenCmd).parms, remDev->hciHandle);
    }
    else
    {
        bt_trace(TRACE_GROUP_1, MESCO_RECONFIGURING_ESCO_LINK_xX, scocon->scoHciHandle);
        /* SCO Connection handle for reconfig */
        StoreLE16(MEC(hciGenCmd).parms, scocon->scoHciHandle);
    }

    /* HFP 1.6 */
    if(scoTxParms->isExtendedSco){
        BtEnhancedScoAudioSettings *scoSettings = &scoTxParms->extScoSettings;
        /* Create extended eSCO */
        StoreLE32(MEC(hciGenCmd).parms + 2, scoSettings->transmitBandwidth);
        StoreLE32(MEC(hciGenCmd).parms + 6, scoSettings->receiveBandwidth);
        MEC(hciGenCmd).parms[10] = scoSettings->txCoding.id;
        StoreLE16(MEC(hciGenCmd).parms + 11, scoSettings->txCoding.vendor.comanyId);
        StoreLE16(MEC(hciGenCmd).parms + 13, scoSettings->txCoding.vendor.id);
        MEC(hciGenCmd).parms[15] = scoSettings->rxCoding.id;
        StoreLE16(MEC(hciGenCmd).parms + 16, scoSettings->rxCoding.vendor.comanyId);
        StoreLE16(MEC(hciGenCmd).parms + 18, scoSettings->rxCoding.vendor.id);
        StoreLE16(MEC(hciGenCmd).parms + 20, scoSettings->txCodecFrameSize);
        StoreLE16(MEC(hciGenCmd).parms + 22, scoSettings->rxCodecFrameSize);
        StoreLE32(MEC(hciGenCmd).parms + 24, scoSettings->inputBandwith);
        StoreLE32(MEC(hciGenCmd).parms + 28, scoSettings->outputBandwith);
        MEC(hciGenCmd).parms[32] = scoSettings->inputCoding.id;
        StoreLE16(MEC(hciGenCmd).parms + 33, scoSettings->inputCoding.vendor.comanyId);
        StoreLE16(MEC(hciGenCmd).parms + 35, scoSettings->inputCoding.vendor.id);
        MEC(hciGenCmd).parms[37] = scoSettings->outputCoding.id;
        StoreLE16(MEC(hciGenCmd).parms + 38, scoSettings->outputCoding.vendor.comanyId);
        StoreLE16(MEC(hciGenCmd).parms + 40, scoSettings->outputCoding.vendor.id);
        StoreLE16(MEC(hciGenCmd).parms + 42, scoSettings->inputCodedDataSize);        
        StoreLE16(MEC(hciGenCmd).parms + 44, scoSettings->outputCodedDataSize);  
        MEC(hciGenCmd).parms[46] = scoSettings->inputPCMDataFormat;
        MEC(hciGenCmd).parms[47] = scoSettings->outputPCMDataFormat;
        MEC(hciGenCmd).parms[48] = scoSettings->inputPCMSamplePayloadMSBPos;
        MEC(hciGenCmd).parms[49] = scoSettings->outputPCMSamplePayloadMSBPos;
        MEC(hciGenCmd).parms[50] = scoSettings->inputDataPath;
        MEC(hciGenCmd).parms[51] = scoSettings->outputDataPath;
        MEC(hciGenCmd).parms[52] = scoSettings->inputTransportUnitSize;
        MEC(hciGenCmd).parms[53] = scoSettings->outputTransportUnitSize;
        StoreLE16(MEC(hciGenCmd).parms + 54, scoSettings->maxLatency);  
        StoreLE16(MEC(hciGenCmd).parms + 56, scoSettings->packetType);
        MEC(hciGenCmd).parms[58] = scoSettings->retransmissionEffort;
        bt_trace(TRACE_GROUP_1, BTESCOPKTTYPEx04X, scoSettings->packetType);
        if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
        {
            Assert(0);
        }
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 59);
        MeSendHciCommand(HCC_ENHANCED_SETUP_SYNC_CONNECTION, 59);
    }else{
        /* Create normal eSCO/SCO */
        StoreLE32(MEC(hciGenCmd).parms + 2, scocon->scoTxParms.transmitBandwidth);
        StoreLE32(MEC(hciGenCmd).parms + 6, scocon->scoTxParms.receiveBandwidth);
        StoreLE16(MEC(hciGenCmd).parms + 10, scocon->scoTxParms.maxLatency);
        StoreLE16(MEC(hciGenCmd).parms + 12, scocon->scoTxParms.voiceSetting);
        MEC(hciGenCmd).parms[14] = scocon->scoTxParms.retransmissionEffort;
        StoreLE16(MEC(hciGenCmd).parms + 15, scocon->scoTxParms.eScoPktType);
        bt_trace(TRACE_GROUP_1, BTESCOPKTTYPEx04X, scocon->scoTxParms.eScoPktType);
        if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
        {
            Assert(0);
        }
        BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 17);
        MeSendHciCommand(HCC_SETUP_SYNC_CONNECTION, 17);
    }
}


/*****************************************************************************
 * FUNCTION
 *  StartScoLinkAcceptTask
 * DESCRIPTION
 *  Start the SCO link accept task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartScoLinkAcceptTask(void)
{
    BtRemoteDevice *remDev;
    BtScoConnect *scocon;
    U8 len;
    HciCommandType command;

    scocon = (BtScoConnect*) MEC(curOp);
    remDev = scocon->scord;

    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    OS_MemCopy(MEC(hciGenCmd).parms, remDev->bdAddr.addr, 6);   /* BD_ADDR */
    StoreLE32(MEC(hciGenCmd).parms + 6, scocon->scoTxParms.transmitBandwidth);
    StoreLE32(MEC(hciGenCmd).parms + 10, scocon->scoTxParms.receiveBandwidth);
    StoreLE16(MEC(hciGenCmd).parms + 14, scocon->scoTxParms.maxLatency);
    StoreLE16(MEC(hciGenCmd).parms + 16, scocon->scoTxParms.voiceSetting);
    MEC(hciGenCmd).parms[18] = scocon->scoTxParms.retransmissionEffort;
    StoreLE16(MEC(hciGenCmd).parms + 19, scocon->scoTxParms.eScoPktType);
    command = HCC_ACCEPT_SYNC_CON_REQ;
    len = 21;

    bt_trace(TRACE_GROUP_1, STARTSCOLINKACCEPTTASK);
    bt_trace(TRACE_GROUP_1, BTESCOPKTTYPEx04X, scocon->scoTxParms.eScoPktType);
#if 0
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
#endif
    MeSCOVerifyHCIGenCommand();
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, len);
    MeSendHciCommand(command, len);
}


/*****************************************************************************
 * FUNCTION
 *  StartScoLinkRejectTask
 * DESCRIPTION
 *  Start the SCO link reject task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartScoLinkRejectTask(void)
{
    BtRemoteDevice *remDev;
    BtScoConnect *scocon;
    HciCommandType command;

    scocon = (BtScoConnect*) MEC(curOp);
    remDev = scocon->scord;

    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    OS_MemCopy(MEC(hciGenCmd).parms, remDev->bdAddr.addr, 6);   /* BD_ADDR */

    /* Most often HC_STATUS_HOST_REJ_NO_RESOURCES */
    MEC(hciGenCmd).parms[6] = scocon->scoRejectReason;

    if ((MEC(btVersion) >= 2) && (MEC(btFeatures[3]) & 0x80))
    {
        /* This is a BT v1.2 eSCO reject. */
        command = HCC_REJECT_SYNC_CON_REQ;
    }
    else
    {
        /* For BT version >=1.2: If it uses this cmd to reject SCO link req: a Cmd Status Event with 0x0C (Cmd Disallowed) will be received */
        command = HCC_REJECT_CON_REQ;
    }
#if 0
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
#endif
    MeSCOVerifyHCIGenCommand();
    bt_trace(TRACE_GROUP_1, MESCO_STARTING_SCO_LINK_REJECT_COMMAND);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 7);
    MeSendHciCommand(command, 7);
}


/*****************************************************************************
 * FUNCTION
 *  StartScoVoiceSettings
 * DESCRIPTION
 *  Start the SCO voice settings command.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartScoVoiceSettings(void)
{
    MeCommandToken *token;

    token = (MeCommandToken*) MEC(curOp);

    MEC(taskHandler) = MeHandleCompleteTask;
    MEC(taskEvent) = HCE_COMMAND_COMPLETE;

    MEC(hciGenCmd).parms[0] = (U8) token->p.vsettings;
    MEC(hciGenCmd).parms[1] = (U8) (token->p.vsettings >> 8);
#if 0
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
#endif
    MeSCOVerifyHCIGenCommand();
    bt_trace(TRACE_GROUP_1, MESCO_STARTING_SCO_VOICE_SETTINGS_COMMAND);
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
    MeSendHciCommand(HCC_WRITE_VOICE_SETTING, 2);
}


/*****************************************************************************
 * FUNCTION
 *  StartScoLinkDisconnectTask
 * DESCRIPTION
 *  Start the link disconnect task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void StartScoLinkDisconnectTask(void)
{
    BtScoConnect *scocon;

    scocon = (BtScoConnect*) MEC(curOp);

    if(((scocon->scoHciHandle & 0x0FFF) != 0x0E00) &&
        ((scocon->scoHciHandle & 0x0FFF) != 0x0E01) &&
        (random() %4 == 0))
    {
#ifdef __BT_DLIGHT1_DEBUG__
        Report(("autumn start to disconnect SCO Link"));
#endif    
        return;
    }
    MEC(taskHandler) = MeHandleConnectionStatusOnly;
    MEC(taskEvent) = HCE_COMMAND_STATUS;

    bt_trace(TRACE_GROUP_1, MESCO_STARTING_DISCONNECT_TO_SCO_HCIHANDLE_xX, scocon->scoHciHandle);

    /* Local SCO Connection handle */
    StoreLE16(MEC(hciGenCmd).parms, scocon->scoHciHandle);
    MEC(hciGenCmd).parms[2] = BEC_USER_TERMINATED;  /* Reason */
#if 0
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
#endif
    MeSCOVerifyHCIGenCommand();
    BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 3);
    MeSendHciCommand(HCC_DISCONNECT, 3);
}


/*****************************************************************************
 * FUNCTION
 *  ScoHandleConnectComplete
 * DESCRIPTION
 *  Handle completion of a SCO link connection
 *  (incoming and outgoing).
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ScoHandleConnectComplete(U8 len, U8 *parm)
{
    BtScoConnect *scocon = 0;
    I8 i;

    /* UNUSED_PARAMETER(len); */

    bt_trace(TRACE_GROUP_1, MESCO_SCO_CONNECTION_COMPLETE_EVENT);

    /*
     * SCO link.  Locate the pending BtScoConnect that matches the 
     * complete event and is pending.
     */
    for (i = 0; i < NUM_SCO_CONNS; i++)
    {
        scocon = &(MEC(scoTable)[i]);
        /* Hunt for an outstanding connection */
        if ((MEC(scoTable)[i].scostate == BDS_OUT_CON2) || (MEC(scoTable)[i].scostate == BDS_IN_CON))
        {
            break;
        }
    }

    if (i < NUM_SCO_CONNS)
    {
        MEC(btEvent).errCode = parm[0];
        scocon->scoHciHandle = LEtoHost16(parm + 1);
#ifdef __BT_SINGLE_ANTENNA__
        BT_WMT_SCO_Send_Status(scocon->scord, 1);
#endif        
        bt_trace(TRACE_GROUP_1, BTLOG_ME_SCO_CONNECTION_COMPLETE_RESULT_HCIHANDLE, parm[0],scocon->scoHciHandle);
        /* Report the results of the link up */
        Me_WriteAudioSetting();
        if (scocon->scostate == BDS_OUT_CON2)
        {
            ScoReportScoLinkUp(scocon, BTEVENT_SCO_CONNECT_CNF);
            BTBMLocalWriteScanEnabled(BM_SCAN_MODE_SET_NORMAL);
        }
        else
        {
            ScoReportScoLinkUp(scocon, BTEVENT_SCO_CONNECT_IND);
        }
    }
    else
    {
        /* Probably the response to an internal reject so ignore it */
        bt_trace(TRACE_GROUP_1, MESCO_SCO_CONNECTION_COMPLETE_INTERNAL_REJECT);
    }
}


/*****************************************************************************
 * FUNCTION
 *  ScoHandleConnChanged
 * DESCRIPTION
 *  Handle completion of a SCO link connection
 *  (incoming and outgoing).
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ScoHandleConnChanged(U8 len, U8 *parm)
{
    BtScoConnect *scocon = 0;
    BtRemoteDevice *remDev;
    I8 i;

    /* UNUSED_PARAMETER(len); */

    bt_trace(TRACE_GROUP_1, MESCO_ESCO_CONNECTION_CHANGED_EVENT);

    /* Locate the pending SCO connection that matches the change event. */
    for (i = 0; i < NUM_SCO_CONNS; i++)
    {
        scocon = &(MEC(scoTable)[i]);
        if (scocon->scostate == BDS_CONNECTED)
        {
            /* We have found a device in the correct state */
            if (scocon->scoHciHandle == LEtoHost16(parm + 1))
            {

                /*
                 * Be sure thar the scocon->scoop is not in MEC(opList):  
                 * otherwise its opType is reset here and it(ie., scocon->scoop) still exists in MEC(opList),
                 * For later, to execute this opType will assert due to opType is equal zero!
                 */
                if(IsNodeOnList(&MEC(opList), &(scocon->scoop.node)))
                {
                    Assert(0);
                }
                scocon->scoop.opType = BOP_NOP;
                break;
            }
        }
    }

    if (i < NUM_SCO_CONNS)
    {
        MEC(btEvent).errCode = parm[0];
        scocon->scoHciHandle = LEtoHost16(parm + 1);
        bt_trace(TRACE_GROUP_1, MESCO_ESCO_CONNECTION_CHANGED_HCIHANDLE__xX, scocon->scoHciHandle);
        /* Report the results */
        remDev = scocon->scord;
        if (remDev->scoHandler != 0)
        {
            MEC(btEvent).p.scoConnect.remDev = remDev;
            scocon->scoRxParms.scoTransmissionInterval = parm[3];
            scocon->scoRxParms.scoRetransmissionWindow = parm[4];
            scocon->scoRxParms.scoRxPacketLen = LEtoHost16(parm + 5);
            scocon->scoRxParms.scoTxPacketLen = LEtoHost16(parm + 7);
            MEC(btEvent).p.scoConnect.scoCon = scocon;
            MEC(btEvent).p.scoConnect.scoHandle = scocon->scoHciHandle;
            MEC(btEvent).p.scoConnect.scoLinkType = BLT_ESCO;
            MEC(btEvent).p.scoConnect.scoTxParms = &scocon->scoTxParms;
            MEC(btEvent).p.scoConnect.scoRxParms = &scocon->scoRxParms;
            MEC(btEvent).eType = BTEVENT_SCO_CONN_CHNG;

            remDev->scoHandler->callback(&MEC(btEvent));
        }

    }
    else
    {
        /* Got a connection changed event for an unknown handle. */
        bt_trace(TRACE_GROUP_1, MESCO_ESCO_CONNECTION_CHANGED_UNKNOWN_HCIHANDLE__xX, LEtoHost16(parm + 1));
    }
}


/*****************************************************************************
 * FUNCTION
 *  ScoHandleLinkSetupOp
 * DESCRIPTION
 *  Handle the BT v1.2 SCO link setup operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ScoHandleLinkSetupOp(BtOpEvent event)
{
    BtScoConnect *scocon;

    if (event == BOE_START)
    {
        StartScoLinkSetupTask();
        return;
    }

    if (event == BOE_TASK_END)
    {
        scocon = (BtScoConnect*) MEC(curOp);
        if (MEC(btEvent).errCode != 0)
        {
            /* We got an error so report it */
            ScoReportScoLinkUp(scocon, BTEVENT_SCO_CONNECT_CNF);
        }
        else
        {
            /* After COMMAND_STATUS, promote to _CON2 if necessary. */
            if (scocon->scostate == BDS_OUT_CON)
            {
                scocon->scostate = BDS_OUT_CON2;
            }
        }
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}


/*****************************************************************************
 * FUNCTION
 *  ScoHandleLinkDisconnectOp
 * DESCRIPTION
 *  Handle the link disconnect operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ScoHandleLinkDisconnectOp(BtOpEvent event)
{
    BtScoConnect *scocon;

    switch (event)
    {
        case BOE_START:
            StartScoLinkDisconnectTask();
            return;

        case BOE_TASK_END:
            bt_trace(TRACE_GROUP_1, MESCO_SCOHANDLELINKDISCONNECTOP_TASK_END_ERRCODE__xX, MEC(btEvent).errCode);
            /* The operation is done so clear the op type */
            scocon = (BtScoConnect*) MEC(curOp);
            scocon->scoop.opType = BOP_NOP;

            if (MEC(btEvent).errCode != 0)
            {
                /*
                 * This link could already be disconnected by the other side. 
                 * In this case we will get an error indicating no connection.
                 * Go ahead and remove the connection without checking the return
                 * code. When connections are unexpectedly dropped, they can 
                 * still exist in our data structures and no disconnect event
                 * will be received.
                 */
                U8 scratch[sizeof(HciHandle) + sizeof(BtErrorCode) + 1];

                /* Assert(MEC(btEvent).errCode == BEC_NO_CONNECTION); */
                if (MEC(btEvent).errCode == BEC_NO_CONNECTION)
                {
                    bt_trace(TRACE_GROUP_1, DISCONNECT_SCO_WITH_ERROR_NO_CONNECTION);
                    scocon = (BtScoConnect*) MEC(curOp);
                    (void)HciDeleteConnection(scocon->scoHciHandle);
                    OS_MemCopy(scratch + 1, (const U8*)&scocon->scoHciHandle, (U32) sizeof(HciHandle));
                    scratch[3] = BEC_NO_CONNECTION;
                    (void)ScoHandleDisconnectComplete((U8) sizeof(scratch), scratch);
                }
                else if (MEC(btEvent).errCode == BEC_INVALID_HCI_PARM)
                {
                    bt_trace(TRACE_GROUP_1, DISCONNECT_SCO_WITH_ERROR_BEC_INVALID_HCI_PARM);
                    scocon = (BtScoConnect*) MEC(curOp);
                    Assert(scocon->scostate == BDS_DISCONNECTED);
                }
            }
            break;

        default:
            Assert(0);
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}


/*****************************************************************************
 * FUNCTION
 *  ScoHandleLinkAcceptOp
 * DESCRIPTION
 *  Handle the link accept operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ScoHandleLinkAcceptOp(BtOpEvent event)
{
    BtScoConnect *scocon;

    switch (event)
    {
        case BOE_START:
            StartScoLinkAcceptTask();
            return;

        case BOE_TASK_END:
            /* The operation is done so clear the op type */
            scocon = (BtScoConnect*) MEC(curOp);
            scocon->scoop.opType = BOP_NOP;

            if (MEC(btEvent).errCode != 0)
            {
                /* We received an error so report it */
                /* bt_trace(TRACE_GROUP_1,MESCOLINKACCEPTERROR__xX , MEC(btEvent).errCode); */
                ScoReportScoLinkUp(scocon, BTEVENT_SCO_CONNECT_IND);
            }
            break;

        default:
            Assert(0);
    }

    /* Start another operation if one exists */
    MeOperationEnd();
}


/*****************************************************************************
 * FUNCTION
 *  ScoHandleLinkRejectOp
 * DESCRIPTION
 *  Handle the link accept operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ScoHandleLinkRejectOp(BtOpEvent event)
{
    BtScoConnect *scocon;

    switch (event)
    {
        case BOE_START:
            StartScoLinkRejectTask();
            return;

        case BOE_TASK_END:
            /* The operation is done so clear the op type */
            scocon = (BtScoConnect*) MEC(curOp);
            scocon->scoop.opType = BOP_NOP;

            if (MEC(btEvent).errCode != 0)
            {
                /* We received an error so report it */
                bt_trace(TRACE_GROUP_1, MESCO_LINKREJECTERROR__xX, MEC(btEvent).errCode);
                ScoReportScoLinkUp(scocon, BTEVENT_SCO_CONNECT_IND);
            }
            break;

        default:
            Assert(0);
    }

    /* Start another operation if one exists */
    MeOperationEnd();

}


/*****************************************************************************
 * FUNCTION
 *  ScoHandleVoiceSettingsOp
 * DESCRIPTION
 *  Handle the SCO Voice Settings operation.
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ScoHandleVoiceSettingsOp(BtOpEvent event)
{
    Me_ScoHandleVoiceSettings(event);

#if 0
    MeCommandToken *token;

    if (event == BOE_START)
    {
        StartScoVoiceSettings();
        return;
    }

    if ((event == BOE_TASK_END) || (event == BOE_TASK_ERR))
    {
        /* The operation is done so clear the op type */
        token = (MeCommandToken*) MEC(curOp);
        token->op.opType = BOP_NOP;

        if (MEC(btEvent).errCode == 0)
        {
            /* Save the new voice settings */
            MEC(vSettings) = token->p.vsettings;
        }

        /* We received an error so report it */
        bt_trace(TRACE_GROUP_1, MESCO_VOICESETTINGS_DONE_STATUS_xX, MEC(btEvent).errCode);
        MEC(btEvent).eType = BTEVENT_SCO_VSET_COMPLETE;
        token->callback(&MEC(btEvent));
    }

    /* Start another operation if one exists */
    MeOperationEnd();
#endif    
}


/*****************************************************************************
 * FUNCTION
 *  ScoHandleLinkConnectReq
 * DESCRIPTION
 *  Handle an incoming link connection.
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL ScoHandleLinkConnectReq(U8 len, U8 *parm)
{
    I8 i;
    BtRemoteDevice *rdev = 0;
    BtScoConnect *scocon = 0;

    /* UNUSED_PARAMETER(len); */

    /*
     * Locate the RemoteDevice that matches the requested event.
     */
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        rdev = &(MEC(devTable)[i]);
        if (rdev->state == BDS_CONNECTED)
        {
            if (StrnCmp(rdev->bdAddr.addr, 6, parm, 6))
            {
                break;
            }
        }
    }

    if (i < NUM_BT_DEVICES)
    {
        /* If there is no accept handler then reject it. */
        /* Call the scoHandler to accept/reject connection */
        if (rdev->scoHandler != 0)
        {
            /* First get a free BtScoConnect structure */
            scocon = ScoGetFreeConnect();

            if (scocon == 0)
            {
                /* No BtScoConnect structure, see if outgoing connect is in progress */
                for (i = 0; i < NUM_SCO_CONNS; i++)
                {
                    scocon = (BtScoConnect*) & MEC(scoTable[i]);
                    if (scocon->scostate == BDS_OUT_CON || scocon->scostate == BDS_OUT_CON2)
                    {
                        if (rdev == scocon->scord)
                        {
                            /*
                             * Found an outgoing SCO connection to same device,
                             * handle race condition by indicating the incoming
                             * connection to the app. The app should accept, as
                             * the outgoing connection will fail.
                             */
                            bt_trace(TRACE_GROUP_1, MESCO_DETECTED_RACE_CONDITION);
                            /* Remove the outgoing SCO connect operation */
                            if (MEC(curOp)->opType == BOP_SCO_SETUP || MEC(curOp)->opType == BOP_SCO_CONNECT)
                            {
                                OS_Report("Out SCO connection is ongoing");
                                MEC(taskHandler) = 0;
                                MEC(curOp) = 0;
                            }else if(IsNodeOnList(&MEC(opList), &scocon->scoop.node)!= FALSE){
                                /* ALPS00297496 : we might encounter SCo connect request when scoop is inserted 
                                    but not the current executing op. Hust fail the connect operation.
                                */
                                OS_Report("Out SCO connection operation is inserted");
                                RemoveEntryList(&scocon->scoop.node);
                            }else{
                                OS_Report("[WARN] SCO connect is outgoing but scoop is not inserted. scoop=%u", scocon->scoop.opType);
                                break;
                            }
                            /* Tell the app the outgoing failed. */
                            MEC(btEvent).p.scoConnect.remDev = rdev;
                            MEC(btEvent).p.scoConnect.scoHandle = scocon->scoHciHandle;
                            MEC(btEvent).p.scoConnect.scoCon = scocon;
                            MEC(btEvent).eType = BTEVENT_SCO_CONNECT_CNF;
                            MEC(btEvent).errCode = BEC_REQUEST_CANCELLED;
                            rdev->scoHandler->callback(&MEC(btEvent));                            
                            break;
                        }
                    }
                }
                if (i >= NUM_SCO_CONNS)
                {
                    goto reject;
                }
            }

            bt_trace(TRACE_GROUP_1, MESCO_RECEIVED_SCO_LINK_CONNECT_REQUEST);
            /* Store the proper values */
            scocon->scord = rdev;
            scocon->scostate = BDS_IN_CON;
            scocon->scoLinkType = parm[9];

            MEC(btEvent).p.scoConnect.remDev = rdev;
            MEC(btEvent).p.scoConnect.scoHandle = scocon->scoHciHandle;

            scocon->scoRxParms.scoTransmissionInterval = parm[3];
            scocon->scoRxParms.scoRetransmissionWindow = parm[4];
            scocon->scoRxParms.scoRxPacketLen = LEtoHost16(parm + 5);
            scocon->scoRxParms.scoTxPacketLen = LEtoHost16(parm + 7);
            MEC(btEvent).p.scoConnect.scoRxParms = &scocon->scoRxParms;

            /*
             * Initialize to TX parms that accept most anything.
             * App may change them.
             */
            scocon->scoTxParms.transmitBandwidth = 0x1f40;
            scocon->scoTxParms.receiveBandwidth = 0x1f40;
            scocon->scoTxParms.voiceSetting = MEC(vSettings);
            scocon->scoTxParms.eScoPktType = 0;

            if (scocon->scoLinkType == BLT_ESCO)
            {
                scocon->scoTxParms.maxLatency = 0xFFFF;
                scocon->scoTxParms.retransmissionEffort = 0xFF;
                if (MEC(btFeatures[1]) & 0x08)
                {
                    scocon->scoTxParms.eScoPktType |= BESPT_HV1;
                }
                if (MEC(btFeatures[1]) & 0x10)
                {
                    scocon->scoTxParms.eScoPktType |= BESPT_HV2;
                }
                if (MEC(btFeatures[1]) & 0x20)
                {
                    scocon->scoTxParms.eScoPktType |= BESPT_HV3;
                }
                if (MEC(btFeatures[3]) & 0x80)
                {
                    scocon->scoTxParms.eScoPktType |= BESPT_EV3;
                }
                if (MEC(btFeatures[4]) & 0x01)
                {
                    scocon->scoTxParms.eScoPktType |= BESPT_EV4;
                }
                if (MEC(btFeatures[4]) & 0x02)
                {
                    scocon->scoTxParms.eScoPktType |= BESPT_EV5;
                }
            }
            else
            {
                scocon->scoTxParms.maxLatency = 0x000F;
                scocon->scoTxParms.retransmissionEffort = 0x00;
                if (MEC(btFeatures[1]) & 0x08)
                {
                    scocon->scoTxParms.eScoPktType |= BESPT_HV1;
                }
                if (MEC(btFeatures[1]) & 0x10)
                {
                    scocon->scoTxParms.eScoPktType |= BESPT_HV2;
                }
                if (MEC(btFeatures[1]) & 0x20)
                {
                    scocon->scoTxParms.eScoPktType |= BESPT_HV3;
                }
            }

            MEC(btEvent).p.scoConnect.scoTxParms = &scocon->scoTxParms;
            MEC(btEvent).p.scoConnect.scoCon = scocon;
            MEC(btEvent).eType = BTEVENT_SCO_CONNECT_REQ;
            MEC(btEvent).p.scoConnect.scoLinkType = parm[9];
            rdev->scoHandler->callback(&MEC(btEvent));
            return TRUE;
        }
    }

  reject:
    /* We need to reject the request so return FALSE */
    bt_trace(TRACE_GROUP_1, MESCO_RECEIVED_SCO_LINK_CONNECT_REQUEST_REJECT);
     /* First get a free BtScoConnect structure */
    scocon = ScoGetFreeConnect();
    if(scocon){
        OS_MemSet(scocon, 0, sizeof(BtScoConnect));
        scocon->scord = rdev;
        scocon->scostate = BDS_IN_CON;
        scocon->scoLinkType = parm[9];        
        scocon->scoRejectReason = BEC_NO_CONNECTION;
        scocon->scoop.opType = BOP_SCO_REJECT;
        /* To speed up the disconnect speed, insert to head */
        InsertHeadList(&MEC(opList), &scocon->scoop.node);
        //SCO_RejectIncomingConnection(BtScoConnect * scoCon, BtHandler * handler, BEC_NO_CONNECTION);
        MeStartOperation();
    }else{
        OS_Report("No free scocon to reject. Just leave it.");
    }
    return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  ScoHandleDisconnectComplete
 * DESCRIPTION
 *  Handle the Disconnect Complete event.
 * PARAMETERS
 *  len         [IN]        
 *  parm        [IN]        
 * RETURNS
 *  TRUE - SCO handled it otherwise FALSE.
 *****************************************************************************/
BOOL ScoHandleDisconnectComplete(U8 len, U8 *parm)
{
    BtScoConnect *scocon = 0;
    I8 i;

    /* UNUSED_PARAMETER(len); */

    /*
     * Locate the pending SCO connection that matches the complete event and
     * is pending.
     */
    for (i = 0; i < NUM_SCO_CONNS; i++)
    {
        scocon = &(MEC(scoTable)[i]);
        if ((scocon->scostate == BDS_OUT_DISC) || (scocon->scostate == BDS_CONNECTED))
        {
            /* We have found a device in the correct state */
            if (scocon->scoHciHandle == LEtoHost16(parm + 1))
            {
                break;
            }
        }
    }

    /* If we can't find match then it is probably an ACL link */
    if (i < NUM_SCO_CONNS)
    {
        bt_trace(TRACE_GROUP_1, BTLOG_ME_SCO_DISCONNECT_COMPLETE_EVENT_HCIHANDLE, scocon->scoHciHandle);
        /* Make sure the state is in pending disconnect when failing commands */
        scocon->scostate = BDS_OUT_DISC;

        /* Clear any pending operations */
        ScoFailPendingCommands(scocon);

        /* Now report to all global handlers */
        MEC(btEvent).errCode = parm[3]; /* Pass along the reason error code */
        MEC(btEvent).p.scoConnect.remDev = scocon->scord;
        MEC(btEvent).p.scoConnect.scoHandle = scocon->scoHciHandle;
        MEC(btEvent).p.scoConnect.scoCon = scocon;
        MEC(btEvent).eType = BTEVENT_SCO_DISCONNECT;
        MEC(btEvent).p.scoConnect.scoLinkType = scocon->scoLinkType;
#ifdef __BT_SINGLE_ANTENNA__
        BT_WMT_SCO_Send_Status(scocon->scord, 0);
#endif        

        MeReportResults(BEM_SCO_DISCONNECT);

        /*
         * The registered handler may need to reconnect, so
         * clear the handle and state.
         */
        scocon->scoHciHandle = 0x852;
        scocon->scostate = BDS_DISCONNECTED;

        /*
         * The application might be terminating so lets not require
         * the scoHandler here.
         */
        if (scocon->scord->scoHandler != 0)
        {
            /* Tell the SCO handler that the link has been disconnected */
            scocon->scord->scoHandler->callback(&MEC(btEvent));
        }
        return TRUE;
    }

    return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  ScoFailPendingCommands
 * DESCRIPTION
 *  Fail commands associated with this SCO link.
 * PARAMETERS
 *  scocon      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void ScoFailPendingCommands(BtScoConnect *scocon)
{
    BtOperation *op, *nextOp, *saveCurOp;

    saveCurOp = 0;
    /* First, check the current operation */
    if (MEC(curOp) != 0 && MEC(curOp)->opType == BOP_GEN_COMMAND)
    {
        if (ScoFailCommand(scocon, (MeCommandToken*) MEC(curOp)) == TRUE)
        {
            saveCurOp = MEC(curOp);
            if (IsNodeOnList(&MEC(cmdList), &saveCurOp->node))
            {
                /* This is an async command */
                RemoveEntryList(&saveCurOp->node);
            }
            MeOperationEnd();
        }
    }

    /* Check the operations queue */
    if (!IsListEmpty(&MEC(opList)))
    {
        op = (BtOperation*) GetHeadList(&MEC(opList));
        do
        {
            nextOp = (BtOperation*) GetNextNode(&op->node);

            if (op->opType == BOP_GEN_COMMAND)
            {
                if (ScoFailCommand(scocon, (MeCommandToken*) op) == TRUE)
                {
                    RemoveEntryList(&op->node);
                }
            }
            op = nextOp;

        } while ((BtOperation*) GetHeadList(&MEC(opList)) != op);
    }

    /* Check the command queue */
    if (!IsListEmpty(&MEC(cmdList)))
    {
        op = (BtOperation*) GetHeadList(&MEC(cmdList));
        do
        {
            nextOp = (BtOperation*) GetNextNode(&op->node);

            if (op != saveCurOp && op->opType == BOP_GEN_COMMAND)
            {
                /* This is not the current remDev operation so check it */
                if (ScoFailCommand(scocon, (MeCommandToken*) op) == TRUE)
                {
                    RemoveEntryList(&op->node);
                }
            }
            op = nextOp;

        } while ((BtOperation*) GetHeadList(&MEC(cmdList)) != op);
    }

}


/*****************************************************************************
 * FUNCTION
 *  ScoFailCommand
 * DESCRIPTION
 *  Fail the associated SCO command.
 * PARAMETERS
 *  scocon      [IN]        
 *  token       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BOOL ScoFailCommand(BtScoConnect *scocon, MeCommandToken *token)
{
    if (token->p.general.in.hciCommand == HCC_CHNG_CONN_PACKET_TYPE &&
        scocon == ScoMapHciToConnect((HciHandle) (LEtoHost16(token->p.general.in.parms) & 0x0fff)))
    {
        token->p.general.out.status = BT_STATUS_FAILED;
        MEC(btEvent).errCode = BEC_NO_CONNECTION;
        MEC(btEvent).p.meToken = token;
        MEC(btEvent).eType = token->eType;
        token->callback(&MEC(btEvent));
        return TRUE;
    }

    return FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  ScoHandleDisconnectAcl
 * DESCRIPTION
 *  Handle the Disconnect of the ACL link. Disconnect all SCO
 *  connections attached to the ACL link.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ScoHandleDisconnectAcl(BtRemoteDevice *remDev)
{
    BtScoConnect *scon;
    I8 i;

    MEC(btEvent).eType = BTEVENT_SCO_DISCONNECT;

    /*
     * GO through SCO's and disconnect all the SCO 
     * connections attached to this remote device.
     */
    for (i = 0; i < NUM_SCO_CONNS; i++)
    {
        scon = (BtScoConnect*) & MEC(scoTable[i]);
        if (remDev == scon->scord)
        {

            switch (scon->scostate)
            {
                case BDS_CONNECTED:
                    /* Found a SCO connection, notify handlers. */
                    scon->scostate = BDS_OUT_DISC;
                    MEC(btEvent).p.scoConnect.remDev = remDev;
                    MEC(btEvent).p.scoConnect.scoHandle = scon->scoHciHandle;
                    MEC(btEvent).p.scoConnect.scoCon = scon;
                    MEC(btEvent).p.scoConnect.scoLinkType = scon->scoLinkType;

                    /*
                     * Disconnect didn't happen normaly , so
                     * delete the HCI handle directly.
                     */
                    (void)HciDeleteConnection(scon->scoHciHandle);
                    scon->scoHciHandle = 0x852;
                    scon->scostate = BDS_DISCONNECTED;

                    if (remDev->scoHandler != 0)
                    {
                        /* Tell the SCO handler that the link has been disconnected */
                        remDev->scoHandler->callback(&MEC(btEvent));
                    }

                    /* Now report to all global handlers */
                    MeReportResults(BEM_SCO_DISCONNECT);
                    break;

                case BDS_OUT_CON:
                    OS_Report("ScoHandleDisconnectAcl (BDS_OUT_CON) : shall not come here");
                case BDS_OUT_CON2:
                    /*
                     * If in outgoing connection mode, indicate terminated
                     * connect CNF to app.
                     */
                    MEC(btEvent).errCode = BEC_LOCAL_TERMINATED;
                    ScoReportScoLinkUp(scon, BTEVENT_SCO_CONNECT_CNF);
                    /* No break! Fall through */

                default:
                    /* Make sure the state is in pending disconnect */
                    scon->scostate = BDS_OUT_DISC;

                    /* Clear out any pending operations */
                    ScoFailPendingCommands(scon);

                    /* Indicate SCO event is disconnected */
                    scon->scostate = BDS_DISCONNECTED;
            }
            scon->scord = 0;
        }
    }

    /*
     * Automactically unregister SCO handler. The posted disconnects will
     * happen after the RemDev is offically terminated so we must unregister
     * it here. This will prevent the SCO Handler from being called after the
     * remDev is invalid.
     */
    remDev->scoHandler = 0;
}


/*****************************************************************************
 * FUNCTION
 *  ScoGetFreeConnect
 * DESCRIPTION
 *  Return a pointer to a free remote device structure or 0 if
 *  one is not found.
 * PARAMETERS
 *  void
 * RETURNS
 *  BtRemoteDevice*
 *****************************************************************************/
BtScoConnect *ScoGetFreeConnect(void)
{
    I8 i;

    /* Go through the ScoConnect array looking for the first free entry. */
    for (i = 0; i < NUM_SCO_CONNS; i++)
    {
        if (MEC(scoTable)[i].scostate == BDS_DISCONNECTED)
        {
            MEC(scoTable)[i].scopt = BSPT_HV3;
            MEC(scoTable)[i].scoHciHandle = 0x852;
            return &(MEC(scoTable)[i]);
        }
    }
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  ScoReportScoLinkUp
 * DESCRIPTION
 *  Report results of an outgoing SCO link coming up.
 * PARAMETERS
 *  scocon      [IN]        
 *  evnt        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ScoReportScoLinkUp(BtScoConnect *scocon, BtEventType evnt)
{
    BtEventMask mask;
    BtOpType saveOp;
    BtRemoteDevice *remDev;

    saveOp = scocon->scoop.opType;
    scocon->scoop.opType = BOP_NOP;

    /*
     * If we were rejecting the link then success does not mean the link
     * is up. It actually means the link is down. We will set the errorCode
     * so the link does not come up.
     */
    if (saveOp == BOP_SCO_REJECT)
    {
        bt_trace(TRACE_GROUP_1, MESCO_SETTING_ERROR_FOR_LINK_REJECT_OLD_ERROR__0XxX, MEC(btEvent).errCode);
        MEC(btEvent).errCode = BEC_LOCAL_TERMINATED;
    }

    if (MEC(btEvent).errCode)
    {
        scocon->scostate = BDS_DISCONNECTED;
    }
    else
    {
        /*
         * Operation succeeded. Put the proper value into the Event and
         * report the event to all bound handlers.
         */
        scocon->scostate = BDS_CONNECTED;
    }
    remDev = scocon->scord;
    if (remDev == 0)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTACL_AND_SCO_REMDEV__0);
        return;
    }
    remDev->scOutoConnectState = 0;
    MEC(btEvent).p.scoConnect.remDev = remDev;
    MEC(btEvent).p.scoConnect.scoHandle = scocon->scoHciHandle;
    MEC(btEvent).p.scoConnect.scoCon = scocon;
    MEC(btEvent).p.scoConnect.scoLinkType = scocon->scoLinkType;
    MEC(btEvent).eType = evnt;

    if (remDev->scoHandler != 0)
    {
        /*
         * The SCO connect operation is complete so report results to 
         * the SCO handler. 
         */
        if (remDev->scoHandler->callback != 0)
        {
            remDev->scoHandler->callback(&MEC(btEvent));
        }
    }

    if (evnt == BTEVENT_SCO_CONNECT_IND)
    {
        mask = BEM_SCO_CONNECT_IND;
    }
    else
    {
        mask = BEM_SCO_CONNECT_CNF;
    }
    MeReportResults(mask);
}


/*****************************************************************************
 * FUNCTION
 *  ScoMapHciToConnect
 * DESCRIPTION
 *  Return whether the given HciHandle is a currently connected
 *  SCO link.
 * PARAMETERS
 *  hcihandle       [IN]        
 * RETURNS
 *  BtRemoteDevice*
 *****************************************************************************/
BtScoConnect *ScoMapHciToConnect(HciHandle hcihandle)
{
    I8 i;

    /* Go through the ScoConnect array looking for the first free entry. */
    for (i = 0; i < NUM_SCO_CONNS; i++)
    {
        if (MEC(scoTable)[i].scostate != BDS_DISCONNECTED)
        {
            if (MEC(scoTable)[i].scoHciHandle == hcihandle)
            {
                return &MEC(scoTable[i]);
            }
        }
    }
    return 0;
}

static void MeSCOVerifyHCIGenCommand()
{
    if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
    {
        Assert(0);
    }
}

#else /* NUM_SCO_CONNS > 0 */


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
static void dummy(void)
{
    dummy();
}

#endif /* NUM_SCO_CONNS > 0 */

