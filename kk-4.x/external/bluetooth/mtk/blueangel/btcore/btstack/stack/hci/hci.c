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
 *     $Workfile:hci.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains the API for the Host Controller Interface.
 *
 * Created:
 *     July 20, 1999
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
#include "btalloc.h"
#include "hci_meta.h"
#include "bt_feature.h"
#include "x_file.h"

extern void HCI_WifiCommandTimeoutHandler(EvmTimer *Timer);
extern U16 MetaBT_MakeCommandOpcode(U16 opcode);
extern void MetaBT_MakeCommand(U8 *data, U16 length);
BtStatus HciTransmitCommand(U16 opCode, U8 parmLen, HciCommand * cmd, BOOL internal, BOOL insertTail);

/*****************************************************************************
 * FUNCTION
 *  HCI_Command_Init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus HCI_Command_Init(void)
{
    /* Initialize HCI Command Queues */

    InitializeListHead(&(HCC(commandList)));
    InitializeListHead(&(HCC(deferredList)));
    InitializeListHead(&(HCC(sentCommandList)));
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  HCI_Init
 * DESCRIPTION
 *  Initialize the HCI queues, receive buffers, and broadcast
 *  handles.
 * PARAMETERS
 *  void
 * RETURNS
 *  BT_STATUS_SUCCESS - HCI is initialized.
 *  BT_STATUS_FAILED - failed to initialize.
 *****************************************************************************/
BtStatus HCI_Init(void)
{
    I16 i;

    OS_LockStack();
    #if defined(__ENABLE_AFH_PTA_HANDLING__)
    BTWIFIC(pta_cmd_wait) = 1;
    BTWIFIC(afh_cmd_wait) = 1;
    if (HCC(wifisettingTimer).func == 0)
    {
        EVM_CancelTimer(&HCC(wifisettingTimer));
    }
    #endif
    HCC(metaMode) = FALSE;
    HCC(metaTxCounter) = 0;
    HCC(metaTxSentCounter) = 0;
    HCC(metaTxDataBuffer) = 0;
    HCC(metaTxRealSentCounter) = 0;
    /* Initialize the broadcast handles */
    HCC(piconetBcHandle) = PICONET_BC_HANDLE - 2;
    HCC(activeBcHandle) = ACTIVE_BC_HANDLE - 2;

    /* Initialize HCI Command Queues */
    OS_StopHardware();
    InitializeListHead(&(HCC(commandList)));
    InitializeListHead(&(HCC(deferredList)));
    InitializeListHead(&(HCC(sentCommandList)));

    /* Initialized Data Queues */
    InitializeListHead(&(HCC(rxBuffList)));
    InitializeListHead(&(HCC(hciPacketPool)));
    InitializeListHead(&(HCC(sentPacketList)));
    InitializeListHead(&(HCC(metaTxDataList)));
    InitializeListHead(&(HCC(metaTxQueueList)));
    for (i = 0; i < (HCI_NUM_PACKETS + HCI_NUM_COMMANDS); i++)
    {
        InsertTailList(&(HCC(hciPacketPool)), &(HCC(hciPackets)[i].node));
    }
    for (i = 0; i < BT_META_TX_TEST_NO; i++)
    {
        InsertTailList(&(HCC(metaTxDataList)), &(HCC(metaPackets)[i].node));
    }
    /* Initialize receive buffers */
    if (RXBUFF_Init() != BT_STATUS_SUCCESS)
    {
        OS_ResumeHardware();
        OS_UnlockStack();
        return BT_STATUS_FAILED;
    }
    OS_ResumeHardware();

    /* Initialize Internal TX Flow State */
    HCC(hciPacketsLeft) = HCI_NUM_PACKETS;
    HCC(maxCommands) = 1;
    HCC(reset_received) = TRUE;
    HCC(FactoryMode) = FALSE;
    #if defined(__ENABLE_AFH_PTA_HANDLING__)
    HCC(wifisettingTimer).func = HCI_WifiCommandTimeoutHandler;
    EVM_StartTimer(&HCC(wifisettingTimer), 1000);
    #endif
#ifdef __ENABLE_SLEEP_MODE__
    HCC(hostwakeupTimer).func = 0;
    HCC(hostwakeupCounter) = 0;
#endif /* __ENABLE_SLEEP_MODE__ */
#ifdef __BT_3_0_HS__    
    HCC(wifiController).controllerId = 0x01;
#endif
    OS_UnlockStack();

    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  HciResetRetryHandler
 * DESCRIPTION
 *  After a reset command is issued, if the radio doesn't resond
 *  within 10 seconds, then an error is generated which allows
 *  the Radio Manager to retry synchronizing flow control with
 *  the Radio Module.
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HciResetRetryHandler(EvmTimer *timer)
{
    /* UNUSED_PARAMETER(timer); */
bt_prompt_trace(MOD_BT, "[BT] HciResetRetryHandler");
    HCC(retryTimer).func = 0;
    HCI_TransportError();
}


/*****************************************************************************
 * FUNCTION
 *  HciHostWakeupRetryHandler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
#ifdef  __ENABLE_SLEEP_MODE__
void HciHostWakeupRetryHandler(EvmTimer *timer)
{
bt_prompt_trace(MOD_BT, "[BT] HciHostWakeupRetryHandler");
    HCC(hostwakeupTimer).func = 0;
    HCC(hostwakeupCounter)++;
    if (HCC(hostwakeupCounter) >= 3)
    {
        HciTransportError1(BT_PANIC_HOST_WAKEUP_FAILED);
        return;
    }
    HCC(maxCommands) = 1;
    Radio_hostWakeup();
}
#endif /* __ENABLE_SLEEP_MODE__ */

/*****************************************************************************
 * FUNCTION
 *  HCI_Reset
 * DESCRIPTION
 *  Sends the HCC_RESET command only. Called by Radio_Init.
 * PARAMETERS
 *  void
 * RETURNS
 *  BT_STATUS_PENDING - HCI is initializing the radio to a known
 *  state. The status will indicate successful
 *  or unsucessfull attempt.
 *****************************************************************************/
BtStatus HCI_Reset(void)
{
    OS_LockStack();
    /* Reset the Host Controller */
    HCC(state) = HCI_STATE_INIT;
    bt_trace(TRACE_GROUP_1, HCI_SENDING_HCC_RESET);
    #if defined(BTMTK_ON_LINUX)
    #ifdef __MTK_BT_EXTERNAL_PLATFORM__
    (void)HciTransmitCommand(HCC_RESET, 0, &(HCC(cmd)), TRUE, TRUE);
    #else
    (void)HciTransmitCommand(HCC_READ_LOCAL_VERSION, 0, &(HCC(cmd)), TRUE, TRUE);
    #endif
    #else
    (void)HciTransmitCommand(HCC_RESET, 0, &(HCC(cmd)), TRUE, TRUE);
    #endif
    bt_prompt_trace(MOD_BT, "[BT] HCI_Reset : HciTransmitCommand success");
    EVM_ResetTimer(&HCC(retryTimer));
    bt_prompt_trace(MOD_BT, "[BT] HCI_Reset : EVM_ResetTimer success");
    HCC(retryTimer).func = HciResetRetryHandler;
    EVM_StartTimer(&HCC(retryTimer), HCI_RESET_TIMEOUT);
    bt_prompt_trace(MOD_BT, "[BT] HCI_Reset : EVM_StartTimer success");
    OS_UnlockStack();
    return BT_STATUS_PENDING;
}


/*****************************************************************************
 * FUNCTION
 *  HCI_HostWakeupStartTimer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
#ifdef __ENABLE_SLEEP_MODE__ 
BtStatus HCI_HostWakeupStartTimer(void)
{
    bt_trace(TRACE_GROUP_1, IF_HOST_WAKEUP_NO_RESPONSE_WE_WILL_GEN_A_FAKE_EVENT_AND_TRY_TO_WAKEUP_AGAIN);
    EVM_ResetTimer(&HCC(hostwakeupTimer));
    HCC(hostwakeupTimer).func = HciHostWakeupRetryHandler;
    EVM_StartTimer(&HCC(hostwakeupTimer), 2000);
    return BT_STATUS_PENDING;
}
#endif /* __ENABLE_SLEEP_MODE__ */


/*****************************************************************************
 * FUNCTION
 *  HCI_Config
 * DESCRIPTION
 *  Initiates the sequence commands for negotiating HCI Flow
 *  Control.
 * PARAMETERS
 *  void
 * RETURNS
 *  BT_STATUS_PENDING - HCI is initializing the Host Controller.
 *  When the Host Controller is initialized,
 *  the Management Entity will be called with a
 *  HCI_INIT_STATUS event.  The status will
 *  indicate successful or unsucessful
 *  initialization.
 *****************************************************************************/
BtStatus HCI_Config(void)
{
    OS_LockStack();

    /* Re-Initialize Command Flow State */
    HCC(maxCommands) = 1;

    HCC(reset_received) = FALSE;

    /* Read the buffer sizes */
    
    (void)HciTransmitCommand(BT_XFile_EncryptionCommandOpcode(HCC_READ_BUFFER_SIZE), 
                            0, &(HCC(cmd)), TRUE, TRUE);

    OS_UnlockStack();
    return BT_STATUS_PENDING;
}


/*****************************************************************************
 * FUNCTION
 *  HCI_Deinit
 * DESCRIPTION
 *  Deinitialize the HCI.  Return all resources to the upper
 *  layers.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void HCI_Deinit(void)
{
    HciCallbackParms cbParms;
    HciCommand *hciCommand;
    HciBuffer hciBuffer;
    U8 buffer[6];
    U16 i;
    U8 data[6];
    OS_LockStack();

    HCC(state) = HCI_STATE_DEINIT;

    /* Set up disconnect complete event */
    buffer[0] = HCE_DISCONNECT_COMPLETE;
    buffer[1] = 4;
    buffer[2] = 0;
    buffer[5] = HC_STATUS_CONN_TERM_LOCAL_HOST;
    hciBuffer.buffer = buffer;
    hciBuffer.flags = HCI_BUFTYPE_EVENT;
    hciBuffer.len = 6;

    /* Return all outstanding packets */
    for (i = 0; i < HCI_NUM_HANDLES; i++)
    {
        if (HCC(con)[i].conFlags & HCI_ACTIVE_CON)
        {

            /* Complete any packets that have not been completely sent */
            while (!IsListEmpty(&(HCC(con)[i].btPacketList)))
            {
                cbParms.ptr.packet = (BtPacket*) RemoveHeadList(&(HCC(con)[i].btPacketList));
                cbParms.hciHandle = HCC(con)[i].hciHandle;
                cbParms.status = BT_STATUS_NO_CONNECTION;
                if (HCC(metaMode))
                {
                    Hci_MetaDataCallback(HCI_PACKET_HANDLED, &cbParms);
                }
                else
                {
                    ME_Callback(HCI_PACKET_HANDLED, &cbParms);
                }
            }
        }
    }

    for (i = 0; i < HCI_NUM_AMP_HANDLES; i++)
    {
        if (HCC(ampCon)[i].conFlags & HCI_ACTIVE_CON)
        {

            /* Complete any packets that have not been completely sent */
            while (!IsListEmpty(&(HCC(ampCon)[i].btPacketList)))
            {
                cbParms.ptr.packet = (BtPacket*) RemoveHeadList(&(HCC(ampCon)[i].btPacketList));
                cbParms.hciHandle = HCC(ampCon)[i].hciHandle;
                cbParms.status = BT_STATUS_NO_CONNECTION;
                if (HCC(metaMode))
                {
                    Hci_MetaDataCallback(HCI_PACKET_HANDLED, &cbParms);
                }
                else
                {
                    ME_Callback(HCI_PACKET_HANDLED, &cbParms);
                }
            }
        }
    }

#if NUM_SCO_CONNS != 0
    /* Free all active SCO connections */
    for (i = 0; i < HCI_NUM_HANDLES; i++)
    {
        if ((HCC(con)[i].conFlags & HCI_ACTIVE_CON) && (HCC(con)[i].conFlags & HCI_DATA_TYPE_SCO))
        {

            /* Generate the disconnect complete event */
            StoreLE16(&(buffer[3]), HCC(con)[i].hciHandle);
            OS_MemCopy((U8 *)data, buffer, hciBuffer.len);
            BT_XFile_DeEncryptionEvent(data, hciBuffer.len);
            hciBuffer.buffer = data;
            HciProcessEvent(&hciBuffer);
        }
    }
#endif /* NUM_SCO_CONNS != 0 */ 

    /* Free all active ACL connections */
    for (i = 0; i < HCI_NUM_AMP_HANDLES; i++)
    {
        if ((HCC(ampCon)[i].conFlags & HCI_ACTIVE_CON) &&
            (HCC(ampCon)[i].conFlags & HCI_DATA_TYPE_ACL))
        {

            /* Generate the disconnect complete event */
            StoreLE16(&(buffer[3]), HCC(ampCon)[i].hciHandle);
            OS_MemCopy((U8 *)data, buffer, hciBuffer.len);
            BT_XFile_DeEncryptionEvent(data, hciBuffer.len);
            hciBuffer.buffer = data;
            HciProcessEvent(&hciBuffer);
        }
    }
    /* Free all active ACL connections */
    for (i = 0; i < HCI_NUM_HANDLES; i++)
    {
        if ((HCC(con)[i].conFlags & HCI_ACTIVE_CON) &&
            (!(HCC(con)[i].hciHandle & (PICONET_BROADCAST | ACTIVE_BROADCAST))) &&
            (HCC(con)[i].conFlags & HCI_DATA_TYPE_ACL))
        {

            /* Generate the disconnect complete event */
            StoreLE16(&(buffer[3]), HCC(con)[i].hciHandle);
            OS_MemCopy((U8 *)data, buffer, hciBuffer.len);
            BT_XFile_DeEncryptionEvent(data, hciBuffer.len);
            hciBuffer.buffer = data;
            HciProcessEvent(&hciBuffer);
        }
    }
    /* Return any outstanding commands */
    while (!IsListEmpty(&(HCC(sentCommandList))))
    {
        hciCommand = (HciCommand*) RemoveHeadList(&HCC(sentCommandList));
        if (hciCommand != &HCC(cmd))
        {
            cbParms.status = BT_STATUS_NO_CONNECTION;
            cbParms.ptr.cmd = hciCommand;
            ME_Callback(HCI_COMMAND_HANDLED, &cbParms);
        }
    }

    while (!IsListEmpty(&(HCC(deferredList))))
    {
        hciCommand = (HciCommand*) RemoveHeadList(&HCC(deferredList));
        cbParms.status = BT_STATUS_NO_CONNECTION;
        cbParms.ptr.cmd = hciCommand;
        ME_Callback(HCI_COMMAND_HANDLED, &cbParms);
    }

    while (!IsListEmpty(&(HCC(commandList))))
    {
        hciCommand = (HciCommand*) RemoveHeadList(&HCC(commandList));
        if (hciCommand != &HCC(cmd))
        {
            cbParms.status = BT_STATUS_NO_CONNECTION;
            cbParms.ptr.cmd = hciCommand;
            ME_Callback(HCI_COMMAND_HANDLED, &cbParms);
        }
    }

    /* Throw away any received data */
    InitializeListHead(&(HCC(rxBuffList)));

    /* Initialize Internal TX Flow State */
    HCC(hciPacketsLeft) = HCI_NUM_PACKETS;
    HCC(maxCommands) = 1;
    EVM_ResetTimer(&HCC(wifisettingTimer));
    OS_UnlockStack();
}


/*****************************************************************************
 * FUNCTION
 *  HCI_MetaReset
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void HCI_MetaReset(void)
{
    U8 oldState;

    oldState = HCC(state);
    HCI_Deinit();
    HCC(state) = oldState;
#if 0
    HciCallbackParms cbParms;
    HciCommand *hciCommand;
    HciBuffer hciBuffer;
    U8 buffer[6];
    U16 i;

    OS_LockStack();

    /* Set up disconnect complete event */
    buffer[0] = HCE_DISCONNECT_COMPLETE;
    buffer[1] = 4;
    buffer[2] = 0;
    buffer[5] = HC_STATUS_CONN_TERM_LOCAL_HOST;
    hciBuffer.buffer = buffer;
    hciBuffer.flags = HCI_BUFTYPE_EVENT;
    hciBuffer.len = 6;

    /* Return all outstanding packets */
    for (i = 0; i < HCI_NUM_HANDLES; i++)
    {
        if (HCC(con)[i].conFlags & HCI_ACTIVE_CON)
        {

            /* Complete any packets that have not been completely sent */
            while (!IsListEmpty(&(HCC(con)[i].btPacketList)))
            {
                cbParms.ptr.packet = (BtPacket*) RemoveHeadList(&(HCC(con)[i].btPacketList));
                cbParms.hciHandle = HCC(con)[i].hciHandle;
                cbParms.status = BT_STATUS_NO_CONNECTION;
                if (HCC(metaMode))
                {
                    Hci_MetaDataCallback(HCI_PACKET_HANDLED, &cbParms);
                }
                else
                {
                    ME_Callback(HCI_PACKET_HANDLED, &cbParms);
                }
            }
        }
    }

#if NUM_SCO_CONNS != 0
    /* Free all active SCO connections */
    for (i = 0; i < HCI_NUM_HANDLES; i++)
    {
        if ((HCC(con)[i].conFlags & HCI_ACTIVE_CON) && (HCC(con)[i].conFlags & HCI_DATA_TYPE_SCO))
        {

            /* Generate the disconnect complete event */
            StoreLE16(&(buffer[3]), HCC(con)[i].hciHandle);
            HciProcessEvent(&hciBuffer);
        }
    }
#endif /* NUM_SCO_CONNS != 0 */ 

    /* Free all active ACL connections */
    for (i = 0; i < HCI_NUM_HANDLES; i++)
    {
        if ((HCC(con)[i].conFlags & HCI_ACTIVE_CON) &&
            (!(HCC(con)[i].hciHandle & (PICONET_BROADCAST | ACTIVE_BROADCAST))) &&
            (HCC(con)[i].conFlags & HCI_DATA_TYPE_ACL))
        {

            /* Generate the disconnect complete event */
            StoreLE16(&(buffer[3]), HCC(con)[i].hciHandle);
            HciProcessEvent(&hciBuffer);
        }
    }

    /* Return any outstanding commands */
    while (!IsListEmpty(&(HCC(sentCommandList))))
    {
        hciCommand = (HciCommand*) RemoveHeadList(&HCC(sentCommandList));
        if (hciCommand != &HCC(cmd))
        {
            cbParms.status = BT_STATUS_NO_CONNECTION;
            cbParms.ptr.cmd = hciCommand;
            ME_Callback(HCI_COMMAND_HANDLED, &cbParms);
        }
    }

    while (!IsListEmpty(&(HCC(deferredList))))
    {
        hciCommand = (HciCommand*) RemoveHeadList(&HCC(deferredList));
        cbParms.status = BT_STATUS_NO_CONNECTION;
        cbParms.ptr.cmd = hciCommand;
        ME_Callback(HCI_COMMAND_HANDLED, &cbParms);
    }

    while (!IsListEmpty(&(HCC(commandList))))
    {
        hciCommand = (HciCommand*) RemoveHeadList(&HCC(commandList));
        if (hciCommand != &HCC(cmd))
        {
            cbParms.status = BT_STATUS_NO_CONNECTION;
            cbParms.ptr.cmd = hciCommand;
            ME_Callback(HCI_COMMAND_HANDLED, &cbParms);
        }
    }

    /* Throw away any received data */
    InitializeListHead(&(HCC(rxBuffList)));

    /* Initialize Internal TX Flow State */
    HCC(hciPacketsLeft) = HCI_NUM_PACKETS;
    HCC(maxCommands) = 1;
    OS_UnlockStack();
#endif    
}


/*****************************************************************************
 * FUNCTION
 *  HciTransmitCommand
 * DESCRIPTION
 *  Send a command to the Host Controller.
 * PARAMETERS
 *  opCode          [IN]        
 *  parmLen         [IN]        
 *  cmd             [IN]        
 *  internal        [IN]        
 *  insertTail      [IN]        
 * RETURNS
 *  BT_STATUS_PENDING
 *  BT_STATUS_INVALID_PARM
 *****************************************************************************/
BtStatus HciTransmitCommand(U16 opCode, U8 parmLen, HciCommand *cmd, BOOL internal, BOOL insertTail)
{
    BtStatus status = BT_STATUS_PENDING;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, cmd != 0);

    OS_LockStack();

    StoreLE16(cmd->header, opCode);
    //bt_trace(TRACE_GROUP_1, BTLOG_HCI_INSERT_HCI_COMMAND_TO_LIST, opCode);
    cmd->header[2] = parmLen;

    if ((HCC(state) == HCI_STATE_INIT) && (!internal))
    {
        //bt_prompt_trace(MOD_BT, "[BT] HciTransmitCommand : Insert HCC(deferredList)");
        /* Defer this command until initialization is complete */
        InsertTailList(&(HCC(deferredList)), &(cmd->node));
    }
    else if ((HCC(state) == HCI_STATE_READY) || (internal))
    {
        //bt_prompt_trace(MOD_BT, "[BT] HciTransmitCommand : Insert HCC(commandList)");
        /* Queue the command for transmission */
        if (insertTail)
        {
            InsertTailList(&(HCC(commandList)), &(cmd->node));
        }
        else
        {
            InsertHeadList(&(HCC(commandList)), &(cmd->node));
        }
        //bt_prompt_trace(MOD_BT, "[BT] HciTransmitCommand : OS_NotifyEvm");
        OS_NotifyEvm();
        //bt_prompt_trace(MOD_BT, "[BT] HciTransmitCommand : OS_NotifyEvm success");
    }
    else
    {
        status = BT_STATUS_FAILED;
    }
    
    OS_UnlockStack();

    return status;
}



/*****************************************************************************
 * FUNCTION
 *  HciSendCommand
 * DESCRIPTION
 *  Send a command to the Host Controller.
 * PARAMETERS
 *  opCode          [IN]        
 *  parmLen         [IN]        
 *  cmd             [IN]        
 *  internal        [IN]        
 *  insertTail      [IN]        
 * RETURNS
 *  BT_STATUS_PENDING
 *  BT_STATUS_INVALID_PARM
 *****************************************************************************/
BtStatus HciSendCommand(U16 opCode, U8 parmLen, HciCommand *cmd, BOOL internal, BOOL insertTail)
{
    BtStatus status = BT_STATUS_PENDING;
    U16 new_code;
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, cmd != 0);

    OS_LockStack();
    new_code = MetaBT_MakeCommandOpcode(opCode);
    MetaBT_MakeCommand(cmd->parms, parmLen);
    
    StoreLE16(cmd->header, new_code);
    //bt_trace(TRACE_GROUP_1, BTLOG_HCI_INSERT_HCI_COMMAND_TO_LIST, opCode);
    cmd->header[2] = parmLen;

    if ((HCC(state) == HCI_STATE_INIT) && (!internal))
    {
        /* Defer this command until initialization is complete */
        InsertTailList(&(HCC(deferredList)), &(cmd->node));
    }
    else if ((HCC(state) == HCI_STATE_READY) || (internal))
    {
        /* Queue the command for transmission */
        if (insertTail)
        {
            InsertTailList(&(HCC(commandList)), &(cmd->node));
        }
        else
        {
            InsertHeadList(&(HCC(commandList)), &(cmd->node));
        }

        OS_NotifyEvm();
    }
    else
    {
        status = BT_STATUS_FAILED;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  HciSendCommand
 * DESCRIPTION
 *  Send a command to the Host Controller.
 * PARAMETERS
 *  opCode          [IN]        
 *  parmLen         [IN]        
 *  cmd             [IN]        
 *  internal        [IN]        
 *  insertTail      [IN]        
 * RETURNS
 *  BT_STATUS_PENDING
 *  BT_STATUS_INVALID_PARM
 *****************************************************************************/
BtStatus HciSendMetaCommand(U16 opCode, U8 parmLen, HciCommand *cmd, BOOL internal, BOOL insertTail)
{
    BtStatus status = BT_STATUS_PENDING;
    U16 new_code;
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, cmd != 0);

    OS_LockStack();
    new_code = BT_XFile_EncryptionCommandOpcode(opCode);
    BT_XFile_EncryptionCommand(cmd->parms, parmLen);
    
    StoreLE16(cmd->header, new_code);
    bt_trace(TRACE_GROUP_1, BTLOG_HCI_INSERT_HCI_COMMAND_TO_LIST, opCode);
    cmd->header[2] = parmLen;

    if ((HCC(state) == HCI_STATE_INIT) && (!internal))
    {
        /* Defer this command until initialization is complete */
        InsertTailList(&(HCC(deferredList)), &(cmd->node));
    }
    else if ((HCC(state) == HCI_STATE_READY) || (internal))
    {
        /* Queue the command for transmission */
        if (insertTail)
        {
            InsertTailList(&(HCC(commandList)), &(cmd->node));
        }
        else
        {
            InsertHeadList(&(HCC(commandList)), &(cmd->node));
        }

        OS_NotifyEvm();
    }
    else
    {
        status = BT_STATUS_FAILED;
    }

    OS_UnlockStack();

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  HCI_RegisterTransport
 * DESCRIPTION
 *  Register the transport interface.
 * PARAMETERS
 *  tranEntry       [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS
 *  BT_STATUS_INVALID_PARM
 *****************************************************************************/
BtStatus HCI_RegisterTransport(TranEntry *tranEntry)
{
    OS_LockStack();

    if (tranEntry != 0)
    {

        /* Parm check */
        CheckLockedParm(BT_STATUS_INVALID_PARM, (tranEntry->sendData != 0));

        /* Register the entry points */
        HCC(tranEntry).sendData = tranEntry->sendData;
        HCC(tranEntry).buffAvail = tranEntry->buffAvail;
        HCC(tranEntry).setSpeed = tranEntry->setSpeed;
        HCC(tranEntry).reSendData = tranEntry->reSendData;
        HCC(tranEntry).sendAMPData = tranEntry->sendAMPData;

    }
    else
    {
        /* Deregister the entry points */
        HCC(tranEntry).sendData = 0;
        HCC(tranEntry).buffAvail = 0;
        HCC(tranEntry).setSpeed = 0;
        HCC(tranEntry).reSendData = 0;
        HCC(tranEntry).sendAMPData = 0;

    }

    OS_UnlockStack();
    return BT_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            HCI_RegisterRadioHandler()
 *---------------------------------------------------------------------------
 *
 *   Register a radio handler for radio module initialization.
 *
 * Return:    BT_STATUS_SUCCESS
 *            BT_STATUS_FAILED
 */


/*****************************************************************************
 * FUNCTION
 *  HCI_RegisterRadioHandler
 * DESCRIPTION
 *  Register a radio handler for radio module initialization.
 * PARAMETERS
 *  radioHandlerCallback        [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS
 *  BT_STATUS_FAILED
 *****************************************************************************/
void HCI_RegisterRadioHandler(RadioHandlerCallback radioHandlerCallback)
{
    bt_trace(TRACE_FUNC, FUNC_HCI_REGISTER_RADIO_HANDLER);      
    OS_LockStack();

    /* Register the radio handler callback */
    HCC(radioEntry) = radioHandlerCallback;

    OS_UnlockStack();

    return;
}


/*****************************************************************************
 * FUNCTION
 *  QueueDeferredCommands
 * DESCRIPTION
 *  Queues any deferred commands for transmission.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void QueueDeferredCommands(void)
{
    HciCommand *cmd;

    while (!IsListEmpty(&HCC(deferredList)))
    {
        cmd = (HciCommand*) RemoveHeadList(&HCC(deferredList));
        InsertTailList(&HCC(commandList), &(cmd->node));
    }
}


/*****************************************************************************
 * FUNCTION
 *  HCI_RadioInitialized
 * DESCRIPTION
 *  Tells the HCI that radio initialization has completed.
 * PARAMETERS
 *  Status      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void HCI_RadioInitialized(BtStatus Status)
{
    HciCallbackParms cbParms;

    OS_LockStack();

    /* Tell ME that initialization is complete */
    if (HCC(state) == HCI_STATE_INIT)
    {
        if (Status == BT_STATUS_SUCCESS)
        {
            QueueDeferredCommands();
            HCC(state) = HCI_STATE_READY;
        }
        cbParms.status = Status;
        ME_Callback(HCI_INIT_STATUS, &cbParms);
    }

    OS_UnlockStack();
}


/*****************************************************************************
 * FUNCTION
 *  HCI_RadioDeinitialized
 * DESCRIPTION
 *  Tells the HCI that radio deinitialization has completed.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void HCI_RadioDeinitialized(void)
{
    HciCallbackParms cbParms;

    OS_LockStack();

    if (HCC(state) != HCI_STATE_INIT)
    {
        /* Generate the deinit complete event */
        HCC(state) = HCI_STATE_SHUTDOWN;
        cbParms.status = BT_STATUS_SUCCESS;
        ME_Callback(HCI_DEINIT_STATUS, &cbParms);
    }
    else
    {
        HCC(state) = HCI_STATE_SHUTDOWN;
    }

    OS_UnlockStack();
}


/*****************************************************************************
 * FUNCTION
 *  HCI_SetSpeed
 * DESCRIPTION
 *  Called by the radio handler to set the speed of the transport.
 * PARAMETERS
 *  Speed       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void HCI_SetSpeed(U16 Speed)
{
    OS_LockStack();

    if (HCC(tranEntry).setSpeed != 0)
    {
        HCC(tranEntry).setSpeed(Speed);
    }

    OS_UnlockStack();
}


/*****************************************************************************
 * FUNCTION
 *  HCI_RequestToSend
 * DESCRIPTION
 *  Allows upper layers to notify HCI that it has data available to
 *  send.  When the HCI gets the thread and is able to send,
 *  it will call ME with an HCI_SEND_IND event.  Must only be called
 *  when the stack is already locked.
 * PARAMETERS
 *  hciHandle       [IN]        
 * RETURNS
 *  BT_STATUS_FAILED - The connection handle is unknown.
 *  
 *  BT_STATUS_SUCESS - The request was recognized.  ME will
 *  be called to request data.
 *****************************************************************************/
BtStatus HCI_RequestToSend(HciHandle hciHandle)
{
    U16 entry;

    /*
     * If this is a broadcast, convert the static broadcast handle 
     * to the dynamic one.
     */
    if (hciHandle == ACTIVE_BC_HANDLE)
    {
        hciHandle = HCC(activeBcHandle);
    }
    else if (hciHandle == PICONET_BC_HANDLE)
    {
        hciHandle = HCC(piconetBcHandle);
    }

    /* Search the connection table for the specified handle */
    entry = HciGetTableEntry(hciHandle);
    if (entry == INVALID_HCI_HANDLE)
    {
        return BT_STATUS_FAILED;
    }

#if L2CAP_NUM_GROUPS > 0
    if ((hciHandle == HCC(activeBcHandle)) || (hciHandle == HCC(piconetBcHandle)))
    {
        /*
         * A broadcast was requested, set a flag and wait for
         * all outstanding data to be sent before sending the 
         * broadcast.
         */
        HCC(flags) |= (U8) (hciHandle >> 12);
    }
    else
    {
#endif /* L2CAP_NUM_GROUPS > 0 */

        /* Send was requested */
        HCC(con)[entry].conFlags |= HCI_DATA_REQ;

    #if L2CAP_NUM_GROUPS > 0
    }
    #endif /* L2CAP_NUM_GROUPS > 0 */

    OS_NotifyEvm();
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  BTQueryPowerOnState
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 BTQueryPowerOnState(void)
{
    if ((ME_GetStackInitState() == BTSS_INITIALIZED) && (HCI_GetState() == HCI_STATE_READY))
    {
        return 0x01;
    }
    else
    {
        return 0x00;
    }
}

U8 HCI_ReadyToSendCommand(void)
{
    return HCC(maxCommands);
}


/* 0 : no, others : yes */
U8 HCI_IsValidAclPacketCount(U16 packetLeft){
    if(packetLeft == 4
    || packetLeft == 6
    || packetLeft == 8)
        return 1;
    return 0;
}

/* 0 : no, others : yes */
U8 HCI_IsValidAclPacket(){
    return HCI_IsValidAclPacketCount(HCC(maxAclTxPacketsLeft));
}

/* 0 : no, others : yes */
U8 HCI_IsValidAclPacketLen(){
    if(HCC(aclTxPacketLen) == 1021)
        return 1;
    return 0;
}

/* 0 : no, others : yes */
U8 HCI_IsValidScoPacketLen(){
    if(HCC(scoTxPacketLen) == 0x1C 
        || HCC(scoTxPacketLen) == 0x3C 
        || HCC(scoTxPacketLen) == 0xF0
        || HCC(scoTxPacketLen) == 0xB8
        || HCC(scoTxPacketLen) == 0xF4 /* MT6630 */)
        return 1;
    return 0;
}
