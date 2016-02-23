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
 *     $Workfile:hci_evnt.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains code for sending commands and handling
 *     events from the Host Controller Interface.
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
#include "btconfig.h"
#include "btalloc.h"
#include "hci_meta.h"
#include "bt_feature.h"
#include "x_file.h"

/* Function prototypes */
static void HciHandleCompletedPackets(U8 *parms);
static void HciHandleConnectComplete(U8 *parms);
static void HciHandleDisconnectComplete(U8 *parms);
static void HciHandleCommandComplete(U8 *parms);
static void HciHandleReadBufferComplete(U8 *parms);

#if HCI_HOST_FLOW_CONTROL == XA_ENABLED
static void HciHandleSetFctlComplete(U8 *parms);
#endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */
static void HciHandleBuffSizeComplete(U8 *parms);

#if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
static void HciHandleWriteScoFcEnableComplete(U8 *parms);
#endif /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */

extern BtSdpServContext btCtxsdps;
#ifdef __BT_MP_RELEASE__
static void HciRxCallback(U8 event, HciCallbackParms *Parms);
#endif
extern void HciNumCompletedPktTimeoutHandler(EvmTimer* timer);
extern BtStatus HciTransmitCommand(U16 opCode, U8 parmLen, HciCommand *cmd, BOOL internal, BOOL insertTail);
#ifdef __ENABLE_SLEEP_MODE__
extern void BTUartRadioWakeupCompleted(void);
#endif /* __ENABLE_SLEEP_MODE__ */
/* 2009-0424: Add for POWER ON/HCI_RESET Retry Mechanism */
void HciUpdateMaxCommandFlag()
{
    Report(("HCI RESET Failure: radiomod updates maxCommands flag"));
    HCC(maxCommands) = 1;	
}

/*****************************************************************************
 * FUNCTION
 *  HciProcessEvent
 * DESCRIPTION
 *  Called when an event has been received from the Host Controller.
 *  Some events are processed before indicating them to the next
 *  layer.  Others are handled completely here and never passed up.
 * PARAMETERS
 *  hciBuffer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void HciProcessEvent(HciBuffer *hciBuffer)
{
    U8 event;
    U8 parmLen;
    U8 *parms;
    U8 data[260];
    HciCallbackParms cbParms;
    HciEvent hciEvent;

    OS_MemCopy((U8 *)data, hciBuffer->buffer, hciBuffer->len);
    BT_XFile_DeEncryptionEvent(data, hciBuffer->len);
    event = data[0];
    parmLen = data[1];
    parms = &(data[2]);

    if (parmLen != (hciBuffer->len - 2))
    {
        /*
         * CSR Radio responds to HCC_HOST_NUM_COMPLETED_PACKETS with an
         * ill-formed command complete event.   They should not respond at
         * all, but to keep the stack for crashing, it will be allowed.
         */
        if (LEtoHost16(&(parms[1])) != HCC_HOST_NUM_COMPLETED_PACKETS)
        {
            /* Invalid length */
            HciTransportError1(BT_PANIC_WRONG_NUM_OF_COMPLETE_PKT);
        }
    }
#ifdef __BT_MP_RELEASE__
    if (!HCC(radioEntry))
    {
        if (OS_MemCmp(data, hciBuffer->len, (U8 *)hciBuffer->buffer, hciBuffer->len) == TRUE)
        {
            OS_MemSet((U8 *)&btCtxsdps, 0, sizeof(BtSdpServContext));
        }
    }
#endif
    /* Parse the events */
    switch (event)
    {
        case HCE_NUM_COMPLETED_PACKETS:
        #ifdef UART_DUMP
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_ME_RX_EVENT, event);
        #endif
            /* Process the completed packets event */
            HciHandleCompletedPackets(parms);

            /* This event is never passed up */
            return;
        case HCE_FLUSH_OCCURRED:
            bt_trace(TRACE_GROUP_1, FLUSH_OCCURED);
            return;
        case HCE_SYNC_CONNECT_COMPLETE:
        case HCE_CONNECT_COMPLETE:
            {
                U8 *temp;
                U32 temp_tick;
            
                temp_tick = OS_GetSystemTime();
                if((temp_tick %3) == 1)
                {
                    if(MEC(BT_Key) != 0)
                    {
                        temp = (U8 *)get_ctrl_buffer(4);
                        OS_MemCopy(temp, MEC(BT_Key), 4);
                        free_ctrl_buffer(MEC(BT_Key));
                        MEC(BT_Key) = temp;
                    }
                }
            }
            /* Process the connection completed event */
            HciHandleConnectComplete(parms);

            /* Pass the event up */
            break;

        case HCE_DISCONNECT_COMPLETE:
            /* Clean up on disconnect */
            HciHandleDisconnectComplete(parms);

            /* Pass the event up */
            break;

        case HCE_COMMAND_STATUS:
            /*
             * Get the maximum outstanding commands value from the status event.
             * Look at the second byte.
             */

            /*
              * Dump HCI_COMMAND_STATUS Event's Parameter Value: For Chip Debug Purpose and requested from the controller team
              * P1(Status): 1byte (valid value = 0x00; others are err code); P2 (# of HCI Cmd Pkts): 1byte; P3(opCode): 2bytes 
              */
            /*
            Report(("command status event:%02x, %02x, %02x",parms[0],parms[1],LEtoHost16(&(parms[2]))));
            */
            bt_trace(TRACE_GROUP_1, BTLOG_HCI_COMMAND_STATUS, parms[0],parms[1],LEtoHost16(&(parms[2])));
            parms++;

    #if HCI_HOST_FLOW_CONTROL == XA_ENABLED
            if (LEtoHost16(&(parms[1])) == HCC_HOST_NUM_COMPLETED_PACKETS)
            {
                /* There should be no response to this command */
                return;
            }
    #endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

            /* Fall through to next case */
        case HCE_COMMAND_COMPLETE:
            if (event == HCE_COMMAND_COMPLETE)
            {
                bt_trace(TRACE_GROUP_1, BTLOG_HCI_COMMAND_COMPLETE_EVENT_OPCODE,LEtoHost16(&(parms[1])));
            }
            /*
             * If the command field, is zero, eat the event, 
             * ME didn't send it, doesn't care.
             * 
             * Also, ignore a command complete for HOST_NUM_COMPLETED PACKETS.
             * CSR sends this, even though they shouldn't.
             */
            if ((LEtoHost16(&(parms[1])) == 0xFCC0))
            {
#ifdef __ENABLE_SLEEP_MODE__            
                BTUartRadioWakeupCompleted();
                return;
#else
                Assert(0);
#endif /* __ENABLE_SLEEP_MODE__ */
            }

#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER
            if ((LEtoHost16(&(parms[1])) == 0xFC84)){
                bt_prompt_trace(MOD_BT, "[FmOverA2DP] receive 0xFC84 command complete");
                HCC(fmovera2dp_enabled) = 0;
            }
#endif

            /*
             * Get the maximum outstanding commands value from the command
             * complete event (or command status event).
             */
            HCC(maxCommands) = parms[0];

            /*
             * If the command field, is zero, eat the event, 
             * ME didn't send it, doesn't care.
             * 
             * Also, ignore a command complete for HOST_NUM_COMPLETED PACKETS.
             * CSR sends this, even though they shouldn't.
             */
            if ((LEtoHost16(&(parms[1])) == 0) || (LEtoHost16(&(parms[1])) == HCC_HOST_NUM_COMPLETED_PACKETS))
            {
                return;
            }

            /* Handle the command complete for init commands */
            if (HCC(state) == HCI_STATE_INIT)
            {
                if (event == HCE_COMMAND_COMPLETE)
                {
                    HciHandleCommandComplete(parms);
                }
                if (!HCC(radioEntry))
                {
                    /* No radio handler, ignore any other events */
                    return;
                }
            }

            /* Pass all other events up */
            break;

        case HCE_HARDWARE_ERROR:
            {
        	  bt_trace(TRACE_GROUP_1, BTLOG_HCI_HARDWARE_ERROR_EVENT_ERRCODE,parms[0]);
                /* 
                  * Note: HW Error Event with 0x08 (invalid hcihandle)
                  * For MAUI_01226420: Do connect/disconnect too fast with the headset Plantronics
                  * Signal cmd is sent before ACL data and this disc cmd is not responded from peer device
                  * After timeout happens, controller will remove hcihandle but thinks the remaining ACL data is valid
                  * In this case: Two HW Error Events with errCode 0x08 and 0x02 will be reported to host!
                  * But, these two errCodes are used for debug purpose: Host can skip them and not to reset 
                  */
                if (parms[0] != 0x08 && parms[0] != 0x02)
                {
                    HciTransportError1(BT_PANIC_HW_ERROR);
                }
            }
            break;
        case HCE_DATA_BUFFER_OVERFLOW:
            /* Fatal error has occured in the radio */
            HciTransportError1(BT_PANIC_DATA_BUFFER_OVERFLOW);
            break;
        case HCE_INQUIRY_COMPLETE:
        case HCE_INQUIRY_RESULT:
        case HCE_CONNECT_REQUEST:
        case HCE_AUTH_COMPLETE:
        case HCE_REMOTE_NAME_REQ_COMPLETE:
        case HCE_ENCRYPT_CHNG:
        case HCE_CHNG_CONN_LINK_KEY_COMPLETE:
        case HCE_MASTER_LINK_KEY_COMPLETE:
        case HCE_READ_REMOTE_FEATURES_COMPLETE:
        case HCE_READ_REMOTE_VERSION_COMPLETE:
        case HCE_QOS_SETUP_COMPLETE:
        case HCE_ROLE_CHANGE:
        case HCE_MODE_CHNG:
        case HCE_RETURN_LINK_KEYS:
        case HCE_PIN_CODE_REQ:
        case HCE_LINK_KEY_REQ:
        case HCE_LINK_KEY_NOTIFY:
        case HCE_LOOPBACK_COMMAND:
        case HCE_MAX_SLOTS_CHNG:
        case HCE_READ_CLOCK_OFFSET_COMPLETE:
        case HCE_CONN_PACKET_TYPE_CHNG:
        case HCE_QOS_VIOLATION:
        case HCE_PAGE_SCAN_MODE_CHANGE:
        case HCE_PAGE_SCAN_REPETITION_MODE:
        case HCE_FLOW_SPECIFICATION_COMPLETE:
        case HCE_INQUIRY_RESULT_WITH_RSSI:
        case HCE_READ_REMOTE_EXT_FEAT_COMPLETE:
        case HCE_FIXED_ADDRESS:
        case HCE_ALIAS_ADDRESS:
        case HCE_GENERATE_ALIAS_REQ:
        case HCE_ACTIVE_ADDRESS:
        case HCE_ALLOW_PRIVATE_PAIRING:
        case HCE_ALIAS_ADDRESS_REQ:
        case HCE_ALIAS_NOT_RECOGNIZED:
        case HCE_FIXED_ADDRESS_ATTEMPT:
        case HCE_SYNC_CONN_CHANGED:
        case HCE_SNIFF_SUBRATING_EVENT:
        case HCE_INQUIRY_RESULT_WITH_EIR:
        case HCE_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT:
        case HCE_IO_CAPABILITY_REQUEST:
        case HCE_IO_CAPABILITY_RESPONSE:
        case HCE_USER_CONFIRM_REQUSEST:
        case HCE_USER_PASSKEY_REQUEST_EVENT:
        case HCE_REMOTE_OOB_DATA_REQUEST_EVENT:
        case HCE_SIMPLE_PAIRING_COMPLETE:
        case HCE_LINK_SUPERVISION_TIMEOUT_CHNG:
        case HCE_ENHANCED_FLUSH_COMPLETE:
        case HCE_USER_PASSKEY_NOTIFICATION:
        case HCE_USER_KEYPRESS:
        case HCE_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT:
        case HCE_UART_SETTING_COMPLETE:
        case HCE_BLUETOOTH_LOGO:
        case HCE_VENDOR_SPECIFIC:
#ifdef __BT_4_0_BLE__
        case HCE_LE_META_EVENT:
#endif
            break;
        default:
            Assert("Please assign to CS6 Posh" == 0);            
            Report(("Unexpected event=%02x", event));
    }
#ifdef __BT_MP_RELEASE__    
    if (!HCC(radioEntry))
    {
        if (OS_MemCmp(data, hciBuffer->len, (U8 *)hciBuffer->buffer, hciBuffer->len) == TRUE)
        {
            OS_MemSet((U8 *)&btCtxsdps, 0, sizeof(BtSdpServContext));
        }
    }
#endif    
    cbParms.status = BT_STATUS_SUCCESS;
    hciEvent.event = event;
    hciEvent.len = parmLen;

    /* Set parm pointer back to the correct place if necesary */
    if (event == HCE_COMMAND_STATUS)
    {
        parms--;
    }

    hciEvent.parms = parms;
    cbParms.ptr.hciEvent = &hciEvent;
    if (HCC(radioEntry))
    {
        EVM_ResetTimer(&HCC(retryTimer));
        HCC(radioEntry) (HCI_CONTROLLER_EVENT, &cbParms);
    }
    else
    {
#ifdef __BT_MP_RELEASE__
        if (OS_MemCmp(data, hciBuffer->len, (U8 *)hciBuffer->buffer, hciBuffer->len) == TRUE)
        {
            return;
        }
#endif        
        ME_Callback(HCI_CONTROLLER_EVENT, &cbParms);
#ifdef __BT_MP_RELEASE__
        if (OS_MemCmp(data, hciBuffer->len, hciBuffer->buffer, hciBuffer->len) == TRUE)
        {
            HCC(radioEntry) = (RadioHandlerCallback)HciRxCallback;
            return;
        }
#endif
    }

}


/*****************************************************************************
 * FUNCTION
 *  HciHandleCompletedPackets
 * DESCRIPTION
 *  Handles the completed packets event from the host controller.
 *  This event is used for flow control between the Host and the Host Controller.
 * PARAMETERS
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HciHandleCompletedPackets(U8 *parms)
{
    U16 entry;
    U8 numHandles;
    U8 *parmTable;
    U16 packetsComplete;
    I16 i;
    HciCallbackParms cbParms;
#ifdef __ENABLE_NUM_COMPLETED_PKT_EVENT_TIMER__    
    BtRemoteDevice* link = NULL;
#endif
    /* Handle the completed packets event */
    numHandles = *(parms);
    parmTable = (U8*) (parms + 1);

    for (i = 0; i < numHandles; i++)
    {
        /*
         * Get the HCI handle and the number of completed
         * packets for that handle.
         */
        entry = HciGetTableEntry(LEtoHost16(&(parmTable[i * 4])));
        if (entry != INVALID_HCI_HANDLE)
        {
            packetsComplete = LEtoHost16((U8*) (&(parmTable[i * 4])) + 2);

            if (HCC(con)[entry].conFlags & HCI_DATA_TYPE_ACL)
            {
                /* Update the total number of ACL packets available. */
                HCC(aclTxPacketsLeft) += packetsComplete;
                if (HCC(metaMode))
                {
                    if (HCC(radioEntry))
                    {
                        cbParms.ptr.txNumber = (U8)packetsComplete;
                        Hci_MetaDataCallback(HCI_META_PKT_SENT_OUT, &cbParms);
                    }
                }
            }
        #if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
            else if (HCC(con)[entry].conFlags & HCI_DATA_TYPE_SCO)
            {
                /* Update the total number of SCO packets available. */
                HCC(scoTxPacketsLeft) += packetsComplete;
            }
        #endif /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */
            else
            {
                Assert(0);
            }

            HCC(con)[entry].packetsSent -= packetsComplete;

#ifdef __ENABLE_NUM_COMPLETED_PKT_EVENT_TIMER__            
            /* MAUI_01482962 */
            link = MeMapHciToRemoteDevice((HciHandle)(HCC(con)[entry].hciHandle & 0x0FFFF));
            if (link != NULL)
            {
                if (HCC(con)[entry].packetsSent == 0)
                {    
                    /* bt_trace(TRACE_GROUP_1, BTLOG_STOP_MONITOR_NUM_COMPLETED_PKTS_TIMER,1,(HCC(con)[entry].hciHandle & 0x0FFF)); */
                    EVM_ResetTimer(&link->monitorNumCompletedPktEventTimer);
                }
                else
                {
                    /* bt_trace(TRACE_GROUP_1, BTLOG_START_MONITOR_NUM_COMPLETED_PKTS_TIMER,2,(HCC(con)[entry].hciHandle & 0x0FFF),HCC(con)[entry].packetsSent); */
                    link->monitorNumCompletedPktEventTimer.func = HciNumCompletedPktTimeoutHandler;
                    link->monitorNumCompletedPktEventTimer.context = (void*)link;
                    EVM_StartTimer(&link->monitorNumCompletedPktEventTimer, 20000);
                }
            }
#endif /* __ENABLE_NUM_COMPLETED_PKT_EVENT_TIMER__ */            
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  HciHandleConnectComplete
 * DESCRIPTION
 *  Handles a connect complete event.
 * PARAMETERS
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HciHandleConnectComplete(U8 *parms)
{
    if (HCC(state) != HCI_STATE_READY)
    {
        /* Don't add connections if HCI is not initialized */
        return;
    }

    if (HCC(conReqs))
    {
        HCC(conReqs)--;
    }

    /* Process the connect complete event */
    if (parms[0] == 0)
    {
        /* Connect is complete, add connection to table */
        AssertEval(HciAddConnection((HciHandle) (LEtoHost16(&(parms[1])) & 0x0FFF), parms[9]) != INVALID_HCI_HANDLE);
    #if L2CAP_NUM_GROUPS > 0
        /* If this is the first connection, add the broadcast handles */
        if ((HCC(activeAclCons) == 1) && (parms[9] == 1))
        {
            /* Initialize broadcast handles */
            AssertEval(HciAddConnection(HCC(piconetBcHandle), 1) != INVALID_HCI_HANDLE);

            AssertEval(HciAddConnection(HCC(activeBcHandle), 1) != INVALID_HCI_HANDLE);
        }
    #endif /* L2CAP_NUM_GROUPS > 0 */

    }
}


/*****************************************************************************
 * FUNCTION
 *  HciHandleDisconnectComplete
 * DESCRIPTION
 *  Handles a disconnect.
 * PARAMETERS
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HciHandleDisconnectComplete(U8 *parms)
{
    BtStatus status;

    if ((HCC(state) != HCI_STATE_READY) && (HCC(state) != HCI_STATE_DEINIT))
    {
        /*
         * Don't add connections if HCI is not initialized or
         * deinitializing (HCI_Deinit generates disconnect
         * complete events).
         */
        return;
    }

    /* Process the disconnect complete event */
    if (parms[0] == 0)
    {
        /* Delete the entry */
        status = HciDeleteConnection(LEtoHost16(&(parms[1])));
        if (status == BT_STATUS_FAILED)
        {
            /*
             * We might have just deleted this connection just return
             * the job is done! 
             */
            return;
        }

    #if L2CAP_NUM_GROUPS > 0
        if (HCC(activeAclCons) == 0)
        {
            /* The last link went down, delete broadcast entries */
            HCC(flags) &= ~(HCI_FLAG_PICONET_BC | HCI_FLAG_ACTIVE_BC | HCI_FLAG_DATA_QUEUED);
            AssertEval(HciDeleteConnection(HCC(piconetBcHandle)) != BT_STATUS_FAILED);
            AssertEval(HciDeleteConnection(HCC(activeBcHandle)) != BT_STATUS_FAILED);
        }
    #endif /* L2CAP_NUM_GROUPS > 0 */
    }
}


/*****************************************************************************
 * FUNCTION
 *  HciHandleCommandComplete
 * DESCRIPTION
 *  Handles a command complete event.
 * PARAMETERS
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HciHandleCommandComplete(U8 *parms)
{
    bt_trace(TRACE_GROUP_1, BTLOG_HCI_HCIINIT_HANDLE_COMMAND_COMPLETE_EVENT);
    /* Call the appropriate handler */
    switch (LEtoHost16(&(parms[1])))
    {
        case HCC_RESET:
            bt_trace(TRACE_GROUP_1, BTLOG_HCI_HCIINIT_HANDLE_COMMAND_COMPLETE_EVENT_HCC_RESET);
            /* No need to do anything if no error, Radio_EventHandler handles this */
            if (parms[3] != 0)
            {
                HciTransportError1(BT_PANIC_RESET_FAILED);
            }
            break;
        case HCC_READ_BUFFER_SIZE:
            bt_trace(TRACE_GROUP_1, BTLOG_HCI_HCIINIT_HANDLE_COMMAND_COMPLETE_EVENT_HCC_READ_BUFFER_SIZE);
            /* Handle the response to the HCC_READ_BUFFER_SIZE command */
            HciHandleReadBufferComplete(parms + 3);
            break;
        #if HCI_HOST_FLOW_CONTROL == XA_ENABLED
        case HCC_SET_CTRLR_TO_HOST_FLOW_CTRL:
            bt_trace(TRACE_GROUP_1, BTLOG_HCI_HCIINIT_HANDLE_COMMAND_COMPLETE_EVENT_HCC_SET_CTRLR_TO_HOST_FLOW_CTRL);
            /* Handle the response to the HCC_SET_CTRLR_TO_HOST_FLOW_CTRL command */
            HciHandleSetFctlComplete(parms + 3);
            break;
        #endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */
        case HCC_HOST_BUFFER_SIZE:
            bt_trace(TRACE_GROUP_1, BTLOG_HCI_HCIINIT_HANDLE_COMMAND_COMPLETE_EVENT_HCC_HOST_BUFFER_SIZE);
            /* Handle the response to the HCC_HOST_BUFFER_SIZE command */
            HciHandleBuffSizeComplete(parms + 3);
            break;
        #if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
        case HCC_WRITE_SCO_FC_ENABLE:
            bt_trace(TRACE_GROUP_1, BTLOG_HCI_HCIINIT_HANDLE_COMMAND_COMPLETE_EVENT_HCC_WRITE_SCO_FC_ENABLE);
            HciHandleWriteScoFcEnableComplete(parms + 3);
            break;
        #endif /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */
#ifdef __ENABLE_SLEEP_MODE__
        case 0xFCC0:
            BTUartRadioWakeupCompleted();
            break;
#endif /* __ENABLE_SLEEP_MODE__ */

    }

    /* Ignore any other commands */
}


/*****************************************************************************
 * FUNCTION
 *  HciHandleReadBufferComplete
 * DESCRIPTION
 *  Handles the response to the HCC_READ_BUFFER_SIZE command.
 *  Upon initialization, the HCI must get the size and number of
 *  ACL and SCO buffers from the host controller.  This function
 *  parses out those values and then initializes flow control
 *  with the host controller.
 * PARAMETERS
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HciHandleReadBufferComplete(U8 *parms)
{
    if (parms[0] == 0)
    {
        /* Read values for the ACL and SCO buffers */
        HCC(aclTxPacketLen) = LEtoHost16(&(parms[1]));
        HCC(scoTxPacketLen) = (U8) parms[3];

        HCC(aclTxPacketsLeft) = LEtoHost16(&(parms[4]));
        HCC(scoTxPacketsLeft) = LEtoHost16(&(parms[6]));
        HCC(maxAclTxPacketsLeft) = LEtoHost16(&(parms[4]));

    #if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
        HCC(cmd).parms[0] = 1;
        BT_XFile_EncryptionCommand(HCC(cmd).parms, 1);
        HciTransmitCommand(BT_XFile_EncryptionCommandOpcode(HCC_WRITE_SCO_FC_ENABLE),
                                1, &(HCC(cmd)), TRUE, TRUE);
    #else /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */

        /* Initialize flow control */
    #if HCI_HOST_FLOW_CONTROL == XA_ENABLED
        HCC(aclRxPacketsLeft) = HCI_NUM_RX_ACL_BUFFERS;
    #endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

        /* Send the Hosts' buffer sizes */
        StoreLE16(&(HCC(cmd).parms[0]), HCI_ACL_DATA_SIZE);
        HCC(cmd).parms[2] = HCI_SCO_DATA_SIZE;
        StoreLE16(&(HCC(cmd).parms[3]), HCI_NUM_RX_ACL_BUFFERS);
        StoreLE16(&(HCC(cmd).parms[5]), HCI_NUM_SCO_BUFFERS);
        BT_XFile_EncryptionCommand(HCC(cmd).parms, 7);
        (void)HciTransmitCommand(BT_XFile_EncryptionCommandOpcode(HCC_HOST_BUFFER_SIZE),
                                7, &(HCC(cmd)), TRUE, TRUE);
    #endif /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */
    }
    else
    {
        /* Init failed */
        HciTransportError1(BT_PANIC_READ_BUF_FAILED);
    }
}

#if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)


/*****************************************************************************
 * FUNCTION
 *  HciHandleWriteScoFcEnableComplete
 * DESCRIPTION
 *  Handles the response to the HCC_WRITE_SCO_FLOW_CONTROL_ENABLE
 *  command.  This function determines whether the host controller
 *  will send number of completed packets events for SCO connection
 *  handles
 * PARAMETERS
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HciHandleWriteScoFcEnableComplete(U8 *parms)
{
    if (parms[0] == 0)
    {

        /* Initialize flow control */
    #if HCI_HOST_FLOW_CONTROL == XA_ENABLED
        HCC(aclRxPacketsLeft) = HCI_NUM_RX_ACL_BUFFERS;
        HCC(scoRxPacketsLeft) = HCI_NUM_SCO_BUFFERS;
    #endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

        bt_trace(TRACE_GROUP_1, HCI_WRITE_SCO_FLOW_CONTROL_ENABLE_COMPLETE);
        /* Send the Hosts' buffer sizes */
        StoreLE16(&(HCC(cmd).parms[0]), HCI_ACL_DATA_SIZE);
        HCC(cmd).parms[2] = HCI_SCO_DATA_SIZE;
        StoreLE16(&(HCC(cmd).parms[3]), HCI_NUM_RX_ACL_BUFFERS);
        StoreLE16(&(HCC(cmd).parms[5]), HCI_NUM_SCO_BUFFERS);
        BT_XFile_EncryptionCommand(HCC(cmd).parms, 7);
        HciTransmitCommand(BT_XFile_EncryptionCommandOpcode(HCC_HOST_BUFFER_SIZE), 7, &(HCC(cmd)), TRUE, TRUE);
    }
    else
    {
        /* Init failed */
        HciTransportError1(BT_PANIC_WRITE_SCO_FC_ENABLE_FAILED);
    }
}
#endif /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */


/*****************************************************************************
 * FUNCTION
 *  HciHandleBuffSizeComplete
 * DESCRIPTION
 *  Handles the response to the HCC_BUFFER_SIZE command.
 * PARAMETERS
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HciHandleBuffSizeComplete(U8 *parms)
{
#if HCI_HOST_FLOW_CONTROL == XA_ENABLED

    if (parms[0] == 0)
    {
        bt_trace(TRACE_GROUP_1, HCI_SET_BUFFER_SIZE_COMPLETE);
        /* Set up host controller to host flow control */
    #if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
        HCC(cmd).parms[0] = 3;
    #else 
        HCC(cmd).parms[0] = 1;
    #endif /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */
        BT_XFile_EncryptionCommand(HCC(cmd).parms, 1);
        HciTransmitCommand(BT_XFile_EncryptionCommandOpcode(HCC_SET_CTRLR_TO_HOST_FLOW_CTRL),
                        1, &(HCC(cmd)), TRUE, TRUE);
    }
    else
    {
        /* Init failed */
        HciTransportError1(BT_PANIC_WRITE_HOST_BUFFER_SIZE_FAILED);
    }

#else /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */
    /* UNUSED_PARAMETER(parms); */

    /* if (HCC(radioEntry)) { */
    EVM_CancelTimer(&HCC(retryTimer));

    /* Tell the HCI that initialization is complete */
    HCI_RadioInitialized(BT_STATUS_SUCCESS);
#endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

}

#if HCI_HOST_FLOW_CONTROL == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  HciHandleSetFctlComplete
 * DESCRIPTION
 *  Handles the response to the HCC_SET_CTRLR_TO_HOST_FLOW_CTRL
 *  command.
 * PARAMETERS
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void HciHandleSetFctlComplete(U8 *parms)
{
    if (parms[0] == 0)
    {

        bt_trace(TRACE_GROUP_1, HCI_SET_FLOW_CONTROL_COMPLETE);
        EVM_CancelTimer(&HCC(retryTimer));

        /* Tell the HCI that initialization is complete */
        HCI_RadioInitialized(BT_STATUS_SUCCESS);

    }
    else
    {
        /* Init failed */
        HciTransportError1(BT_PANIC_SET_FLOW_CTL_FAILED);
    
    }
}

#endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

#ifdef __BT_MP_RELEASE__
static void HciRxCallback(U8 event, HciCallbackParms *parms)
{
    switch (event)
    {
        case HCI_DATA_IND:
        case HCI_PACKET_HANDLED:
            /* Give the packet to L2CAP */
            L2CAP_HciTxDone(parms->ptr.packet, parms->status, parms->hciHandle);
            break;
        case HCI_COMMAND_HANDLED:
        case HCI_SEND_IND:
            parms->ptr.packet = L2CAP_HciTxReady(parms, 0);
            break;
        case HCI_CONTROLLER_EVENT:
            break;
        case HCI_INIT_STATUS:
        case HCI_DEINIT_STATUS:
            /* Set state to ensure no commands are queued. */
            MEC(stackState) = BTSS_INITIALIZE_ERR;

            /*
             * HCI deintialization is complete. All ACL and SCO connections
             * should have been disconnected by HCI. Return any existing commands.
             * and attempt to initialize HCI 
             */
            bt_trace(TRACE_GROUP_1, ME_HCI_DEINIT_COMPLETE_STATUS_xD, parms->status);

            MEC(curTask) = 0;

            MEC(stackState) = BTSS_NOT_INITIALIZED;
            MEC(btEvent).eType = BTEVENT_HCI_DEINITIALIZED;
            MeReportNMResults();
            break;

        case HCI_TRANSPORT_ERROR:
            bt_trace(TRACE_GROUP_1, ME_HCI_TRANSPORT_ERROR);
            MEC(btEvent).eType = BTEVENT_HCI_FATAL_ERROR;
            MeReportNMResults();
            HCI_Deinit();
            break;

    }
}
#endif

