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
 *     $Workfile:hci_proc.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains code to process rx/tx and timer events
 *     for the HCI.
 *
 * Created:
 *     September 20, 1999
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
#include "btalloc.h"
#include "radiomgr.h"
#include "hci_meta.h"
#include "bt_jsr82_setting.h"
#include "x_file.h"
#include "btconfig.h"  /* for UART_DUMP */
#include "bt_feature.h"
#include "uarttran.h"
#include "pal_hci_struct.h"
#include "hci_amp.h"
static BtStatus SendAMPHciPackets(HciHandle hciHandle);
static BtStatus HciGetMaxPackets(U16 entry, U16 *maxPackets);
static BtStatus SendHciPackets(HciHandle hciHandle);
U8 HciGetControllerType(U16 con_hdl);
HciConnection *HciGetAMPHciConnection(U16 hdl);

extern U8 MeWriteAFH(void);
extern U8 MeWritePTA(void);

extern BtStatus HciTransmitCommand(U16 opCode, U8 parmLen, HciCommand *cmd, BOOL internal, BOOL insertTail);
extern void L2CAP_HciReceiveAMP(HciBuffer *BuffDesc, HciHandle HciHndl);
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
extern void UARTTRAN_EnableMT66xxFlowControl(void);
#endif

#ifdef __ENABLE_NUM_COMPLETED_PKT_EVENT_TIMER__
/*****************************************************************************
 * FUNCTION
 *  HciNumCompletedPktTimeoutHandler
 * DESCRIPTION
 *  Called when the monitor_num_completed_pkt_timer of a link is expired.
 *
 * PARAMETERS
 *  EvmTimer*
 * RETURNS
 *  void
 *****************************************************************************/
void HciNumCompletedPktTimeoutHandler(EvmTimer* timer)
{
    BtRemoteDevice* link = (BtRemoteDevice*) timer->context;
    bt_prompt_trace(MOD_BT, "[BT] HciNumCompletedPktTimeoutHandler");
    timer->func = 0;
    if (link != NULL)
    {
        int entry=0;
        entry = HciGetTableEntry(link->hciHandle);
        bt_trace(TRACE_GROUP_1,BTLOG_0X13_EVENT_TIMEOUT,link,link->state,link->hciHandle,HCC(con)[entry].packetsSent);
        if (link->state == BDS_CONNECTED)
        {
#ifdef __ENABLE_DISC_LINK_NUM_COMPLETED_PKT_EVENT_TIMER_EXPIRED__
            ME_ForceDisconnectLinkWithReason(0, link, BEC_USER_TERMINATED, TRUE);
#else
            link->monitorNumCompletedPktEventTimer.func = HciNumCompletedPktTimeoutHandler;
            link->monitorNumCompletedPktEventTimer.context = (void*)link;
            /* Start a 20sec timer to wait for HCE_NUM_COMPLETED_PACKETS (0x13) Event */
            /* bt_trace(TRACE_GROUP_1, BTLOG_START_MONITOR_NUM_COMPLETED_PKTS_TIMER,1,(hciHandle & 0x0FFF),HCC(con)[entry].packetsSent); */
            EVM_StartTimer(&link->monitorNumCompletedPktEventTimer, 20000);
#endif
        }
    }
}
#endif /* __ENABLE_NUM_COMPLETED_PKT_EVENT_TIMER__ */

/*****************************************************************************
 * FUNCTION
 *  HciTransportError
 * DESCRIPTION
 *  Called when the transport driver detects an error condition.
 *  This should be called at interrupt time.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void HciTransportError(void)
{
    bt_prompt_trace(MOD_BT, "[BT] Failed !! HciTransportError");

    // disable exit(1) => terminate program and no HciTransportReset()
    // disable Assert(0) => call back itself and cause infinite loop
    //exit(1);
    //Assert(0);    /* open for non-4N branch only*/

    HciTransportReset();
}

/*****************************************************************************
 * FUNCTION
 *  HciTransportError1
 * DESCRIPTION
 *  Called when the transport driver detects an error condition.
 *  This should be called at interrupt time.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void HciTransportError1(U8 index)
{
    Report(("PANIC index:%d",index));
    bt_fw_assert(index);
    Assert(0);    /* open for non-4N branch only*/
    //HciTransportReset();
}


/*****************************************************************************
 * FUNCTION
 *  HciTransportReset
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void HciTransportReset(void)
{
    HCC(transportError) = TRUE;
    OS_ResetInd();
    OS_NotifyEvm();
}

void HciReturnOneNoneSentACL(U8 hciConEntryIndex)
{
    HCC(aclTxPacketsLeft)++;
    HCC(con)[hciConEntryIndex].packetsSent--;
    Report(("A1 aclTxPacketsLeft:%d",HCC(aclTxPacketsLeft)));

}

void HciReturnAMPWifiOneNoneSentACL(U8 hciConEntryIndex)
{
#ifndef __BT_30_HS_INFINITE_SENT__
    HCC(con)[hciConEntryIndex].packetsSent--;
    Report(("A1 HCC(con)[hciConEntryIndex].packetsSent:%d",HCC(con)[hciConEntryIndex].packetsSent));
#endif    
}



/*****************************************************************************
 * FUNCTION
 *  HciResetReceived
 * DESCRIPTION
 *  Called when the transport driver detects an error condition.
 *  This should be called at interrupt time.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void HciResetReceived(void)
{
    HCC(reset_received) = TRUE;
    OS_NotifyEvm();
}


/*****************************************************************************
 * FUNCTION
 *  HciFactoryMode
 * DESCRIPTION
 *  Called when poweron.
 *  This should be called at interrupt time.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void HciFactoryMode(void)
{
    HCC(FactoryMode) = TRUE;
    HCC(metaMode) = TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  HciBufferInd
 * DESCRIPTION
 *  Called when a buffer has been received from the Host Controller.
 *  This should be called at interrupt time.
 * PARAMETERS
 *  buffer      [IN]
 *  status      [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void HciBufferInd(HciBuffer *buffer, BtStatus status)
{
    if (status == BT_STATUS_SUCCESS)
    {
        /* Data was received from the Host Controller */
        InsertTailList(&(HCC(rxBuffList)), &(buffer->node));
        OS_NotifyEvm();
    }
    else
    {
        /* Free the buffer, it contains no data */
        RXBUFF_Free(buffer);
        HCC(tranEntry).buffAvail(buffer->flags);
    }
}


/*****************************************************************************
 * FUNCTION
 *  HciPacketSent
 * DESCRIPTION
 *  Called when a packet has been sent to the Host Controller.
 *  This should be called at interrupt time.
 * PARAMETERS
 *  packet      [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void HciPacketSent(HciPacket *packet)
{
//    bt_trace(TRACE_FUNC, FUNC_HCI_PACKET_SENT);
    /* Put the buffer back on the available list */
    InsertTailList(&(HCC(sentPacketList)), &(packet->node));
    if (!(packet->flags & HCI_BUFTYPE_INTERMEDIATE))
    {
        OS_NotifyEvm();
    }
}



/*****************************************************************************
 * FUNCTION
 *  HciGetMaxPackets
 * DESCRIPTION
 *  Returns the maximum number of HCI packets that can be sent on
 *  the specified connection;
 * PARAMETERS
 *  entry           [IN]
 *  maxPackets      [OUT]
 * RETURNS
 *  BT_STATUS_SUCCESS - the Host Controller can receive more data.
 *  The maxPackets value shows how much data to send on this
 *  connection (can be 0 in some cases).
 *
 *  BT_STATUS_FAILED - the Host Controller cannot receive any more
 *  data.  maxPackets will always be 0.
 *****************************************************************************/
static BtStatus HciGetMaxPackets(U16 entry, U16 *maxPackets)
{
#if HCI_SCO_ENABLED
    BtPacket *btPacket;
    HciCallbackParms cbParms;
#endif /* HCI_SCO_ENABLED */

    *maxPackets = 0;

    /*
     * Determine the number of HCI packets to be sent
     * for the current BtPacket.
     */
    if (HCC(con)[entry].conFlags & HCI_DATA_TYPE_ACL)
    {
        /* ACL data */
        Assert(HCC(aclTxPacketLen) != 0);
        *maxPackets = HCC(con)[entry].remaining / HCC(aclTxPacketLen);
        if (HCC(con)[entry].remaining % HCC(aclTxPacketLen))
        {
            /* A partial packet is left over */
            (*maxPackets)++;
        }
    }
#if HCI_SCO_ENABLED
    else if (HCC(con)[entry].conFlags & HCI_DATA_TYPE_SCO)
    {
        /* SCO data */
        if (HCC(scoTxPacketLen) == 0)
        {
            /* Radio does not support SCO Data, fail the Tx request */
            btPacket = (BtPacket*) RemoveHeadList(&(HCC(con)[entry].btPacketList));

            HCC(con)[entry].remaining = 0;
            HCC(con)[entry].btPacket = 0;

            cbParms.hciHandle = HCC(con)[entry].hciHandle;
            cbParms.status = BT_STATUS_NOT_SUPPORTED;
            cbParms.ptr.packet = btPacket;
            if (HCC(radioEntry))
            {
                HCC(radioEntry) (HCI_PACKET_HANDLED, &cbParms);
            }
            else
            {
                ME_Callback(HCI_PACKET_HANDLED, &cbParms);
            }
            goto Error_Exit;
        }

        *maxPackets = HCC(con)[entry].remaining / HCC(scoTxPacketLen);
        if (HCC(con)[entry].remaining % HCC(scoTxPacketLen))
        {
            /* A partial packet is left over */
            (*maxPackets)++;
        }
    }
#endif /* HCI_SCO_ENABLED */
    else
    {
        Assert(0);
        goto Error_Exit;
    }

    /*
     * Determine the maximum number of frames that can be
     * sent on this connection at this time.
     */
    if (HCC(con)[entry].conFlags & HCI_DATA_TYPE_ACL)
    {
        /* ACL data */
#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER
        if(HCC(fmovera2dp_enabled)){
            if( HCC(aclTxPacketsLeft) == HCC(maxAclTxPacketsLeft) )
                *maxPackets = min(*maxPackets, 1);
            else
                *maxPackets= 0;
        }else{
            *maxPackets = min(*maxPackets, HCC(aclTxPacketsLeft));
        }
#else
        *maxPackets = min(*maxPackets, HCC(aclTxPacketsLeft));
#endif
    }
#if HCI_SCO_ENABLED
    else if (HCC(con)[entry].conFlags & HCI_DATA_TYPE_SCO)
    {
        /* SCO data */
    #if HCI_SCO_FLOW_CONTROL == XA_ENABLED
        *maxPackets = min(*maxPackets, HCC(scoTxPacketsLeft));
    #endif /* HCI_SCO_FLOW_CONTROL == XA_ENABLED */
    }
#endif /* HCI_SCO_ENABLED */
    else
    {
        Assert(0);
        goto Error_Exit;
    }

#if XA_STATISTICS == XA_ENABLED
    if (*maxPackets > HCC(hciPacketsLeft))
    {
        XASTAT_CounterInc(hci_num_no_hci_packets, *maxPackets - HCC(hciPacketsLeft));
    }
#endif /* XA_STATISTICS == XA_ENABLED */

    *maxPackets = min(*maxPackets, HCC(hciPacketsLeft));

  Error_Exit:

#if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
    if ((HCC(aclTxPacketsLeft) + HCC(scoTxPacketsLeft)) == 0)
    {
#else /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */
    if (HCC(aclTxPacketsLeft) == 0)
    {
#endif /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */
        return BT_STATUS_FAILED;
    }
    else
    {
        return BT_STATUS_SUCCESS;
    }
}

#ifdef __BT_3_0_HS__
/*****************************************************************************
 * FUNCTION
 *  HciGetMaxAMPPackets
 * DESCRIPTION
 *  Returns the maximum number of HCI packets that can be sent on
 *  the specified connection;
 * PARAMETERS
 *  entry           [IN]        
 *  maxPackets      [OUT]       
 * RETURNS
 *  BT_STATUS_SUCCESS - the Host Controller can receive more data.
 *  The maxPackets value shows how much data to send on this
 *  connection (can be 0 in some cases).
 *  
 *  BT_STATUS_FAILED - the Host Controller cannot receive any more
 *  data.  maxPackets will always be 0.
 *****************************************************************************/
static BtStatus HciGetMaxAMPPackets(U16 entry, U16 *maxPackets, U8 controllerId)
{
    U16 maxACLTxDataLength;
    U8 txAvailable;
#ifdef __BT_30_HS_INFINITE_SENT__
    *maxPackets = 1;
    return BT_STATUS_SUCCESS;
#endif

    
    *maxPackets = 0;
    if(controllerId == HCC(wifiController).controllerId)
    {
        maxACLTxDataLength = HCC(wifiController).maxACLTxDataLength;
    }
    if (HCC(ampCon)[entry].conFlags & HCI_DATA_TYPE_ACL)
    {
        *maxPackets = HCC(ampCon)[entry].remaining / maxACLTxDataLength;
        if (HCC(ampCon)[entry].remaining % maxACLTxDataLength)
        {
            (*maxPackets)++;
        }
    }
    else
    {
        Assert(0);
        goto Error_Exit;
    }

    if (HCC(ampCon)[entry].conFlags & HCI_DATA_TYPE_ACL)
    {
        txAvailable= HciGetAMPControllerTxAvailable(controllerId);
        *maxPackets = min(*maxPackets, txAvailable);
    }
    else
    {
        Assert(0);
        goto Error_Exit;
    }

  Error_Exit:

    if (HciGetAMPControllerTxAvailable(controllerId) == 0)
    {
        return BT_STATUS_FAILED;
    }
    else
    {
        return BT_STATUS_SUCCESS;
    }
}
#endif
/*****************************************************************************
 * FUNCTION
 *  SendHciPackets
 * DESCRIPTION
 *  Send Hci packets on the specified connection.
 * PARAMETERS
 *  hciHandle       [IN]
 * RETURNS
 *  BT_STATUS_SUCCESS if more data can be sent to the Host Controller.
 *  BT_STATUS_FAILED if no more data can be sent.
 *****************************************************************************/
static BtStatus SendHciPackets(HciHandle hciHandle)
{
    HciPacket *hciPacket;
    U16 maxPackets = 0;
    U16 entry;
    I16 i;
    U8 fragment;
    U16 dataLen = 0;
    BtStatus status;
    BOOL done;
    const U8 *ptr;
    U16 len = 0;
#ifdef __ENABLE_NUM_COMPLETED_PKT_EVENT_TIMER__
    BtRemoteDevice* link = NULL;
#endif

//    bt_trace(TRACE_FUNC, FUNC_SEND_HCI_PACKETS);
    entry = HciGetTableEntry(hciHandle);
    if (entry == INVALID_HCI_HANDLE)
    {
        Assert(0);
    }
    if( entry >= HCI_NUM_HANDLES ){
        return BT_STATUS_FAILED; // INVALID_HCI_HANDLE
    }

    /* See how many packet must be sent */
    status = HciGetMaxPackets(entry, &maxPackets);

    for (i = 0; i < maxPackets; i++)
    {

        /* Get a transmit packet */
        if (!IsListEmpty(&(HCC(hciPacketPool))))
        {
            hciPacket = (HciPacket*) RemoveHeadList(&(HCC(hciPacketPool)));
#ifdef __BT_3_0_HS__
            hciPacket->controllerType = HCI_AMP_CONTROLLER_BREDR;
#endif
        }
        else
        {
            /* No transmit packets left */
            break;
        }

        /* Initialize */
        if (HCC(con)[entry].conFlags & HCI_DATA_TYPE_ACL)
        {
            hciPacket->flags = HCI_BUFTYPE_ACL_DATA;
        }
        else
        {
            hciPacket->flags = HCI_BUFTYPE_SCO_DATA;
        }

        hciPacket->flags |= HCI_BUFTYPE_INTERMEDIATE;
        fragment = 0;
        dataLen = 0;

        if (HCC(con)[entry].buffPtr == HCC(con)[entry].btPacket->header)
        {
            /* This is the first packet */
            if (hciPacket->flags & HCI_BUFTYPE_ACL_DATA)
            {
            #if defined (__BT_2_1_AUTO_FLUSH__)
                hciHandle &= 0xFEFF;
                if (HCC(con)[entry].btPacket->timebound == 0xFF)
                {
                    hciHandle |= FIRST_AUTO_FLUSH_PACKET;
                }
                else
                {
                    hciHandle |= FIRST_PACKET;
                }

            #else /* defined (__BT_2_1_AUTO_FLUSH__) */
                hciHandle |= FIRST_PACKET;
                hciHandle &= ~CONTINUING_PACKET;
            #endif /* defined (__BT_2_1_AUTO_FLUSH__) */
            }
            HCC(con)[entry].txState = HCI_TX_STATE_SEND_HEADER;
            ptr = HCC(con)[entry].btPacket->header + (BT_PACKET_HEADER_LEN - HCC(con)[entry].btPacket->headerLen);
            HCC(con)[entry].fragRemain = HCC(con)[entry].btPacket->headerLen;
        }
        else
        {
            /* This is a continuation packet */
            if (hciPacket->flags & HCI_BUFTYPE_ACL_DATA)
            {
            #if defined (__BT_2_1_AUTO_FLUSH__)
                hciHandle &= 0xFEFF;
                hciHandle |= CONTINUING_PACKET;
            #else /* defined (__BT_2_1_AUTO_FLUSH__) */
                hciHandle |= CONTINUING_PACKET;
                hciHandle &= ~FIRST_PACKET;
            #endif /* defined (__BT_2_1_AUTO_FLUSH__) */
            }
            ptr = HCC(con)[entry].buffPtr;
        }

        done = FALSE;
        while (!done)
        {

            /* Set up the transmit packet */
            switch (HCC(con)[entry].txState)
            {
                case HCI_TX_STATE_SEND_HEADER:

                    /* Send the BtPacket header */

                    if (HCC(con)[entry].fragRemain > 0)
                    {
                        /* Not done sending header */
                        break;
                    }

                    /* Set up the next state */
                    HCC(con)[entry].txState = HCI_TX_STATE_SEND_DATA;

                    if (HCC(con)[entry].btPacket->dataLen > 0)
                    {
                        /* There is data to send */
                        ptr = HCC(con)[entry].btPacket->data;
                        HCC(con)[entry].fragRemain = HCC(con)[entry].btPacket->dataLen;
                    }

                    /* Fall through to next state */

                case HCI_TX_STATE_SEND_DATA:

                    /* Send the BtPacket data */

                    if (HCC(con)[entry].fragRemain > 0)
                    {
                        /* Not done sending data */
                        break;
                    }

                    /* Set up the next state */
                    HCC(con)[entry].txState = HCI_TX_STATE_SEND_TAIL;

                    if (HCC(con)[entry].btPacket->flags & BTP_FLAG_TAIL)
                    {
                        /* There is a tail that must be sent */
                        ptr = HCC(con)[entry].btPacket->tail;
                        HCC(con)[entry].fragRemain = HCC(con)[entry].btPacket->tailLen;

                    }

                    /* Fall through to next state */

                case HCI_TX_STATE_SEND_TAIL:

                    /* Send the BtPacket tail */

                    if (HCC(con)[entry].fragRemain > 0)
                    {
                        /* Not done sending tail */
                        break;
                    }

                #if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
                    /* Set up the next state */
                    HCC(con)[entry].txState = HCI_TX_STATE_SEND_FCS;

                    if (HCC(con)[entry].btPacket->flags & BTP_FLAG_FCS)
                    {
                        /* There is a FCS that must be sent */
                        ptr = HCC(con)[entry].btPacket->fcs;
                        HCC(con)[entry].fragRemain = 2;
                    }

                    /* Fall through to next state */

                case HCI_TX_STATE_SEND_FCS:

                    /* Send the BtPacket L2CAP FCS */

                    if (HCC(con)[entry].fragRemain > 0)
                    {
                        /* Not done sending FCS */
                        break;
                    }
                #endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */

                    hciPacket->flags &= ~HCI_BUFTYPE_INTERMEDIATE;
                    done = TRUE;
                    break;
            }

            if (!done)
            {
                /* Initialize the length of the current fragment */
                if (hciPacket->flags & HCI_BUFTYPE_ACL_DATA)
                {
                    /* Initialize the length of the current fragment */
                    len = min(HCC(aclTxPacketLen) - dataLen, HCC(con)[entry].fragRemain);
                }
            #if HCI_SCO_ENABLED
                else if (hciPacket->flags & HCI_BUFTYPE_SCO_DATA)
                {
                    len = min(HCC(scoTxPacketLen) - dataLen, HCC(con)[entry].fragRemain);
                }
            #endif /* HCI_SCO_ENABLED */
                else
                {
                    Assert(0);
                }

                /* Set up the fragment */
                hciPacket->f[fragment].len = len;
                hciPacket->f[fragment].buffer = ptr;

                HCC(con)[entry].fragRemain -= len;
                HCC(con)[entry].remaining -= len;
                HCC(con)[entry].buffPtr = ptr + len;
                dataLen += len;

                fragment++;

                if (hciPacket->flags & HCI_BUFTYPE_ACL_DATA)
                {
                    Assert(dataLen <= HCC(aclTxPacketLen));
                    if (dataLen == HCC(aclTxPacketLen))
                    {
                        hciPacket->flags &= ~HCI_BUFTYPE_INTERMEDIATE;
                        done = TRUE;
                    }
                }

            #if HCI_SCO_ENABLED
                else
                {
                    Assert(dataLen <= HCC(scoTxPacketLen));
                    if (dataLen == HCC(scoTxPacketLen))
                    {
                        hciPacket->flags &= ~HCI_BUFTYPE_INTERMEDIATE;
                        done = TRUE;
                    }
                }
            #endif /* HCI_SCO_ENABLED */
            }
        }
        if (hciPacket->flags & HCI_BUFTYPE_ACL_DATA)
        {
            /* Update the connection values */
            Assert(HCC(aclTxPacketsLeft) > 0);
            HCC(aclTxPacketsLeft)--;
        }
    #if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
        else
        {
            /* Update the connection values */
            Assert(HCC(scoTxPacketsLeft) > 0);
            HCC(scoTxPacketsLeft)--;
        }
    #endif /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */

        hciPacket->fCount = fragment;
        HCC(con)[entry].packetsSent++;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
        hciPacket->hciConEntryindex = entry;
        if(HCC(con)[entry].btPacket->flags & BTP_FLAG_RDEV_RETX)
        {
            hciPacket->txCounter = HCC(con)[entry].btPacket->ulpContext;
        }
        else
        {
            hciPacket->txCounter = 0;
        }
#endif

#ifdef __ENABLE_NUM_COMPLETED_PKT_EVENT_TIMER__
        /* MAUI_01482962: Check if the monitor timer is running or not */
        link = MeMapHciToRemoteDevice((HciHandle) (hciHandle & 0x0FFF));
        if (link!= NULL && link->monitorNumCompletedPktEventTimer.func == 0)
        {
            link->monitorNumCompletedPktEventTimer.func = HciNumCompletedPktTimeoutHandler;
            link->monitorNumCompletedPktEventTimer.context = (void*)link;
            /* Start a 20sec timer to wait for HCE_NUM_COMPLETED_PACKETS (0x13) Event */
            /* bt_trace(TRACE_GROUP_1, BTLOG_START_MONITOR_NUM_COMPLETED_PKTS_TIMER,1,(hciHandle & 0x0FFF),HCC(con)[entry].packetsSent); */
            EVM_StartTimer(&link->monitorNumCompletedPktEventTimer, 20000);
        }
#endif /* __ENABLE_NUM_COMPLETED_PKT_EVENT_TIMER__ */

        /* Set up the HCI packet header */
        StoreLE16(hciPacket->header, hciHandle);

    #if HCI_SCO_ENABLED
        if (hciPacket->flags & HCI_BUFTYPE_ACL_DATA)
        {
            /* ACL data */
            StoreLE16(hciPacket->header + 2, dataLen);
        }
        else
        {
            /*
             * SCO data, header length == 3,  the transport must
             * * be aware of this and send only the first 3 bytes.
             */
            Assert(dataLen <= 255);
            hciPacket->header[2] = (U8) dataLen;
            hciPacket->header[3] = 0;
        }
    #else /* HCI_SCO_ENABLED */
        StoreLE16(hciPacket->header + 2, dataLen);
    #endif /* HCI_SCO_ENABLED */

        /* Reference count the HCI packets sent */
        HCC(con)[entry].btPacket->hciPackets++;
        HCC(hciPacketsLeft)--;

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
        if (HCC(aclTxPacketsLeft) == 0x01)
        {
            Report(("Almost empty"));
        }
        if(HCC(con)[entry].btPacket->flags & BTP_FLAG_RDEV_RETX)
        {
            /* If the remote side ack the L2CAP retransmission, the L2CAP will report to upper layer */
            /* The buffer may be released by upper layer, the retransmission packet may still in HCI layer */
            /* To aviod the condition, we need to check if the retransmission packet is been ack in L2CAP */
            /* If the packet is ack, we will remove the packet in tx packet queue. */
            Report(("Btpacket retransmission context:%d", ((U8 *)(HCC(con)[entry].btPacket->ulpContext))[0]));
            if(((U8 *)(HCC(con)[entry].btPacket->ulpContext))[0] == 0x00)
            {
                Report(("DEBUG ACK from remote side,HCI still retransmitted"));
                HCC(con)[entry].btPacket = 0;
                HCC(con)[entry].remaining = 0;
                HCC(con)[entry].packetsSent--;
                HCI_PacketSent(hciPacket);
                HCC(aclTxPacketsLeft)++;
                return BT_STATUS_SUCCESS;
            }
        }
#endif
        HCC(tranEntry).sendData(hciPacket);
        if (HCC(con)[entry].remaining == 0)
        {
            /* The entire packet was sent */
            HCC(con)[entry].btPacket = 0;
        }
        else
        {
            HCC(con)[entry].conFlags |= HCI_DATA_REQ;
        }
    }

    if (dataLen == 0)
    {
        return status;
    }
    else
    {
        return BT_STATUS_SUCCESS;
    }
}
#ifdef __BT_3_0_HS__
/*****************************************************************************
 * FUNCTION
 *  SendHciPackets
 * DESCRIPTION
 *  Send Hci packets on the specified connection.
 * PARAMETERS
 *  hciHandle       [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS if more data can be sent to the Host Controller.
 *  BT_STATUS_FAILED if no more data can be sent.
 *****************************************************************************/
static BtStatus SendAMPHciPackets(HciHandle hciHandle)
{
    HciPacket *hciPacket;
    U16 maxPackets = 0;
    U16 entry;
    I16 i;
    U8 fragment;
    U16 dataLen = 0;
    BtStatus status = BT_STATUS_FAILED;
    BOOL done;
    const U8 *ptr = NULL;
    U16 len = 0;
    U8 controllerType;
    U16 aclTxPacketLen;
    HciConnection *hciConn;

    entry = HciGetAMPTableEntry(hciHandle);

    if (entry == INVALID_HCI_HANDLE)
    {
        Assert(0);
    }

    status = HciGetMaxAMPPackets(entry, &maxPackets, HCI_AMP_CONTROLLER_WIFI);
    aclTxPacketLen = HCC(wifiController).maxACLTxDataLength;
    hciConn = &(HCC(ampCon)[entry]);
    hciConn->ampTxFailed = 0;
    for (i = 0; i < maxPackets; i++)
    {

        /* Get a transmit packet */
        if (!IsListEmpty(&(HCC(hciPacketPool))))
        {
            hciPacket = (HciPacket*) RemoveHeadList(&(HCC(hciPacketPool)));
            hciPacket->controllerType = HCI_AMP_CONTROLLER_WIFI;
        }
        else
        {
            /* No transmit packets left */
            break;
        }
        hciPacket->flags = HCI_BUFTYPE_ACL_DATA;
        hciPacket->flags |= HCI_BUFTYPE_INTERMEDIATE;
        fragment = 0;
        dataLen = 0;

        if (hciConn->buffPtr == hciConn->btPacket->header)
        {
            /* This is the first packet */
            if (hciPacket->flags & HCI_BUFTYPE_ACL_DATA)
            {
                hciHandle |= FIRST_AUTO_FLUSH_PACKET;
                hciHandle &= ~CONTINUING_PACKET;
            }
            hciConn->txState = HCI_TX_STATE_SEND_HEADER;
            ptr = hciConn->btPacket->header + (BT_PACKET_HEADER_LEN - hciConn->btPacket->headerLen);
            hciConn->fragRemain = hciConn->btPacket->headerLen;
        }
        done = FALSE;
        while (!done)
        {

            /* Set up the transmit packet */
            switch (hciConn->txState)
            {
                case HCI_TX_STATE_SEND_HEADER:

                    /* Send the BtPacket header */

                    if (hciConn->fragRemain > 0)
                    {
                        /* Not done sending header */
                        break;
                    }

                    /* Set up the next state */
                    hciConn->txState = HCI_TX_STATE_SEND_DATA;

                    if (hciConn->btPacket->dataLen > 0)
                    {
                        /* There is data to send */
                        ptr = hciConn->btPacket->data;
                        hciConn->fragRemain = hciConn->btPacket->dataLen;
                    }

                    /* Fall through to next state */

                case HCI_TX_STATE_SEND_DATA:

                    /* Send the BtPacket data */

                    if (hciConn->fragRemain > 0)
                    {
                        /* Not done sending data */
                        break;
                    }

                    /* Set up the next state */
                    hciConn->txState = HCI_TX_STATE_SEND_TAIL;

                    if (hciConn->btPacket->flags & BTP_FLAG_TAIL)
                    {
                        /* There is a tail that must be sent */
                        ptr = hciConn->btPacket->tail;
                        hciConn->fragRemain = hciConn->btPacket->tailLen;

                    }

                    /* Fall through to next state */

                case HCI_TX_STATE_SEND_TAIL:

                    /* Send the BtPacket tail */

                    if (hciConn->fragRemain > 0)
                    {
                        /* Not done sending tail */
                        break;
                    }

                    /* Set up the next state */
                    hciConn->txState = HCI_TX_STATE_SEND_FCS;

                    if (hciConn->btPacket->flags & BTP_FLAG_FCS)
                    {
                        /* There is a FCS that must be sent */
                        ptr = hciConn->btPacket->fcs;
                        hciConn->fragRemain = 2;
                    }

                    /* Fall through to next state */

                case HCI_TX_STATE_SEND_FCS:

                    /* Send the BtPacket L2CAP FCS */

                    if (hciConn->fragRemain > 0)
                    {
                        /* Not done sending FCS */
                        break;
                    }

                    hciPacket->flags &= ~HCI_BUFTYPE_INTERMEDIATE;
                    done = TRUE;
                    break;
            }

            if (!done)
            {
                /* Initialize the length of the current fragment */
                /* Initialize the length of the current fragment */
                len = min(aclTxPacketLen - dataLen, hciConn->fragRemain);
                /* Set up the fragment */
                hciPacket->f[fragment].len = len;
                hciPacket->f[fragment].buffer = ptr;

                hciConn->fragRemain -= len;
                hciConn->remaining -= len;
                hciConn->buffPtr = ptr + len;
                dataLen += len;

                fragment++;

                Assert(dataLen <= aclTxPacketLen);
                if (dataLen == aclTxPacketLen)
                {
                    hciPacket->flags &= ~HCI_BUFTYPE_INTERMEDIATE;
                    done = TRUE;
                }
            }
        }

        hciPacket->fCount = fragment;
        hciPacket->hciConEntryindex = entry;
        if(hciConn->btPacket->flags & BTP_FLAG_RDEV_RETX)
        {
            hciPacket->txCounter = hciConn->btPacket->ulpContext;
        }
        else
        {
            hciPacket->txCounter = 0;
        }

       
        /* Set up the HCI packet header */
        StoreLE16(hciPacket->header, hciHandle);

        StoreLE16(hciPacket->header + 2, dataLen);

        /* Reference count the HCI packets sent */
        hciConn->btPacket->hciPackets++;
//        HCC(hciPacketsLeft)--;

        if(hciConn->btPacket->flags & BTP_FLAG_RDEV_RETX)
        {
            /* If the remote side ack the L2CAP retransmission, the L2CAP will report to upper layer */
            /* The buffer may be released by upper layer, the retransmission packet may still in HCI layer */
            /* To aviod the condition, we need to check if the retransmission packet is been ack in L2CAP */
            /* If the packet is ack, we will remove the packet in tx packet queue. */
            Report(("Btpacket retransmission context:%d", ((U8 *)(hciConn->btPacket->ulpContext))[0]));
            if(((U8 *)(hciConn->btPacket->ulpContext))[0] == 0x00)
            {
                Report(("DEBUG ACK from remote side,HCI still retransmitted"));
                hciConn->btPacket = 0;
                hciConn->remaining = 0;
                hciConn->packetsSent--;
                HCI_PacketSent(hciPacket);
                return BT_STATUS_SUCCESS;
            }
        }
        //Report(("hciConn->remaining:%d",hciConn->remaining));
        HCC(tranEntry).sendAMPData(hciPacket);
        HCI_PacketSent(hciPacket);
        if(hciConn->remaining == 0)
        hciConn->btPacket = 0;
    }

    if (hciConn->remaining  == 0)
    {
        return BT_STATUS_SUCCESS;
    }
    else
    {
        return BT_STATUS_PENDING;
    }
}

#endif


/*****************************************************************************
 * FUNCTION
 *  HciProcessSentPackets
 * DESCRIPTION
 *  Process any HCI packets that have been sent.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void HciProcessSentPackets(void)
{
    HciPacket *hciPacket;
    HciCommand *hciCommand;
    BtPacket *btPacket;
    HciCallbackParms cbParms;
    U16 entry;
    HciConnection *hciConn;

    //bt_trace(TRACE_FUNC, FUNC_HCI_PROCESS_SENT_PACKETS);
    OS_StopHardware();
    while (!IsListEmpty(&(HCC(sentPacketList))))
    {
        /*
         * Remove the transmit packet from the list and put it back
         * on the free list.
         */
        hciPacket = (HciPacket*) RemoveHeadList(&(HCC(sentPacketList)));
        if (!IsNodeOnList(&HCC(hciPacketPool), &hciPacket->node))
        {
            InsertTailList(&(HCC(hciPacketPool)), &(hciPacket->node));
        }
        else
        {
            /* Ignore this packet */
            OS_ResumeHardware();
            continue;
        }
        OS_ResumeHardware();

        if (hciPacket->flags & (HCI_BUFTYPE_ACL_DATA | HCI_BUFTYPE_SCO_DATA))
        {
            HCC(hciPacketsLeft)++;

            /* Get the table entry for the returned packet */
#ifdef __BT_3_0_HS__            
            if(((LEtoHost16(hciPacket->header) & 0x0FFF) >= 0x0100) &&
                (((LEtoHost16(hciPacket->header) & 0x0FFF) < 0x0200)))
            {
                /* AMP data */
                if(hciPacket->controllerType != 0x01)
                {
                    Assert(0);
                }
                entry = HciGetAMPTableEntry(LEtoHost16(hciPacket->header));
                hciConn = &HCC(ampCon)[entry];
#ifdef __BT_30_HS_INFINITE_SENT__
                HCC(ampCon)[entry].packetsSent -= 1;
#endif
            }
            else
            {
                /* BR/EDR */

                entry = HciGetTableEntry(LEtoHost16(hciPacket->header));
                hciConn = &HCC(con)[entry];
            }
#else
            /* BR/EDR only */
            entry = HciGetTableEntry(LEtoHost16(hciPacket->header));
            hciConn = &HCC(con)[entry];

#endif
            if (entry == INVALID_HCI_HANDLE)
            {
                /* The connection went away, ignore this event */
                bt_trace(TRACE_GROUP_1, HCI_INVALID_HCI_HANDLE_IN_SENT_PACKET_LIST);
                return;
            }
            if (!(IsListEmpty(&(hciConn->btPacketList))))
            {
            
                /* Update the number of outstanding HCI packets for this BtPacket */
                btPacket = (BtPacket*) GetHeadList(&(hciConn->btPacketList));
                if( 0 == btPacket->hciPackets ){
                    btPacket->hciPackets++;
                    Report( ("[ERR] May assert here !!!") );
                }
                Assert(btPacket->hciPackets > 0);
                btPacket->hciPackets--;
                
                if (btPacket->hciPackets == 0)
                {
                    /*
                     * All outstanding HCI packets have been completed.
                     * * Now see if all HCI packets for this BtPacket have
                     * * been sent.
                     */
                    if (!((btPacket == hciConn->btPacket) && (hciConn->remaining)))
                    {
                        /* All the data for this transmit packet has been sent */
                        btPacket = (BtPacket*) RemoveHeadList(&(hciConn->btPacketList));

                        /* Convert a dynamic broadcast handle to a static one */
                        if (hciConn->hciHandle == HCC(piconetBcHandle))
                        {
                            cbParms.hciHandle = PICONET_BC_HANDLE;
                        }
                        else if (hciConn->hciHandle == HCC(activeBcHandle))
                        {
                            cbParms.hciHandle = ACTIVE_BC_HANDLE;
                        }
                        else
                        {
                            cbParms.hciHandle = hciConn->hciHandle;
                        }

                        XASTAT_PeakSet(hci_max_tx_packet_size, btPacket->dataLen + btPacket->headerLen);
                        XASTAT_TimerStop(btPacket->dataLen, hci_tx_timer, btPacket->hci_timer);

                        cbParms.status = BT_STATUS_SUCCESS;
                        cbParms.ptr.packet = btPacket;
                        
                        if (HCC(radioEntry))
                        {
                            if (HCC(metaMode))
                            {
                                Hci_MetaDataCallback(HCI_PACKET_HANDLED, &cbParms);
                            }
                            else
                            {
                                HCC(radioEntry) (HCI_PACKET_HANDLED, &cbParms);
                            }
                        }
                        else
                        {
                            ME_Callback(HCI_PACKET_HANDLED, &cbParms);
                        }
                    }
                }
            }
        }
        else
        {
            /* A command was handled */
            Assert(hciPacket->flags & HCI_BUFTYPE_COMMAND);

            /* Tell upper layer */
            if (!IsListEmpty(&HCC(sentCommandList)))
            {
                hciCommand = (HciCommand*) RemoveHeadList(&HCC(sentCommandList));
                if (hciCommand != &HCC(cmd))
                {
                    cbParms.status = BT_STATUS_SUCCESS;
                    cbParms.ptr.cmd = hciCommand;
                    if (HCC(radioEntry))
                    {
                        HCC(radioEntry) (HCI_COMMAND_HANDLED, &cbParms);
                    }
                    else
                    {
                        /* Not an internal command, notify ME */
                        ME_Callback(HCI_COMMAND_HANDLED, &cbParms);
                    }
                }
            }
        }
        OS_StopHardware();
    }
    OS_ResumeHardware();
}


/*****************************************************************************
 * FUNCTION
 *  HciProcessReceivedData
 * DESCRIPTION
 *  Process any received ACL, SCO or event data.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void HciProcessReceivedData(void)
{
    HciBuffer *hciRxBuffer;
    HciCallbackParms cbParms;
    U16 entry = INVALID_HCI_HANDLE;
    ListEntry tmpRxBuffList;
    U8 headerSize = 0;
    U16 dataLen = 0;
#ifdef __BT_3_0_HS__
    U8 ampData = 0;
    U16 tempHandler;
#endif   


    OS_StopHardware();

    /*
     * Move all elements of the receive buffer list to a temporary
     * list head for processing.  This limits the processing of
     * receive buffers to the buffers already received.  New
     * received data will be processed next time.
     */
    MoveList(&tmpRxBuffList, &(HCC(rxBuffList)));

    while (!IsListEmpty(&tmpRxBuffList))
    {

        /* Remove the data buffer from the list */
        hciRxBuffer = (HciBuffer*) RemoveHeadList(&tmpRxBuffList);
        OS_ResumeHardware();

        /* Make sure all packets are handled first */
        HciProcessSentPackets();

        if (hciRxBuffer->flags & HCI_BUFTYPE_EVENT)
        {
            /* Parse and handle the event */
            HciProcessEvent(hciRxBuffer);
        }
        else
        {
            /* ACL or SCO data received */
        #if HCI_SCO_ENABLED
            Assert((hciRxBuffer->flags & HCI_BUFTYPE_ACL_DATA) || (hciRxBuffer->flags & HCI_BUFTYPE_SCO_DATA));
        #else
            Assert(hciRxBuffer->flags & HCI_BUFTYPE_ACL_DATA);
        #endif
            if (hciRxBuffer->flags & HCI_BUFTYPE_ACL_DATA)
            {
                headerSize = 4; /* ACL Header size */
                dataLen = LEtoHost16(hciRxBuffer->buffer + 2);
            #ifdef UART_DUMP
                bt_trace(TRACE_GROUP_1, BTLOG_RECV_UART_ACL_DATA, hciRxBuffer->buffer[0] & 0xFF, hciRxBuffer->buffer[1] & 0x0F, hciRxBuffer->buffer[1] & 0x30, dataLen);
            #endif
            }
            else if (hciRxBuffer->flags & HCI_BUFTYPE_SCO_DATA)
            {
                headerSize = 3; /* SCO Header size */
                dataLen = *(U8*) (hciRxBuffer->buffer + 2);
            }
            else
            {
                Assert(0);
            }

            if (dataLen > 0)
            {
                /* Adjust the pointer and length */
                hciRxBuffer->len -= headerSize;
                Assert(hciRxBuffer->len == dataLen);

                /* Set up callback parms */
                cbParms.status = BT_STATUS_SUCCESS;
                cbParms.hciHandle = LEtoHost16(hciRxBuffer->buffer);

                /* Skip over header */
                hciRxBuffer->buffer += headerSize;
                cbParms.ptr.rxBuff = hciRxBuffer;
#ifdef __BT_3_0_HS__                
                tempHandler = cbParms.hciHandle & 0x0fff;
                if(tempHandler < 32)
                {
                    /* AMP data*/
                    ampData = 1;
                    entry = INVALID_HCI_HANDLE;
                }
                else
                {
                    /* Get the entry in the connection table */
                    entry = HciGetTableEntry(cbParms.hciHandle);
                }
#else
                /* Get the entry in the connection table */
                entry = HciGetTableEntry(cbParms.hciHandle);
#endif

                /* Pass up the data to ME */
#ifdef __BT_3_0_HS__                
                if ((entry != INVALID_HCI_HANDLE) || (ampData == 1))
#else
                if (entry != INVALID_HCI_HANDLE)
#endif
                {
                    if (HCC(radioEntry))
                    {
                        if (HCC(metaMode))
                        {
                            Hci_MetaDataCallback(HCI_DATA_IND, &cbParms);
                        }
                        else
                        {
                            HCC(radioEntry) (HCI_DATA_IND, &cbParms);
                        }
                    }
                    else
                    {
                        ME_Callback(HCI_DATA_IND, &cbParms);
                    }
                }
                else
                {
                    bt_trace(TRACE_GROUP_1, HCI_RECEIVED_DATA_FOR_AN_INVALID_HCI_HANDLE_IGNORED);
                }

                hciRxBuffer->buffer -= headerSize;
            }

        #if HCI_HOST_FLOW_CONTROL == XA_ENABLED
#ifdef __BT_3_0_HS__                
            if ((hciRxBuffer->flags & HCI_BUFTYPE_ACL_DATA) && (ampData ==0))
#else
            if (hciRxBuffer->flags & HCI_BUFTYPE_ACL_DATA)
#endif
            {
                /* Update the number of receive packets left */
                if (entry != INVALID_HCI_HANDLE)
                {
                #if 1
                    /* Assert(HCC(aclRxPacketsLeft) != 0); */
                    if (HCC(aclRxPacketsLeft) == 0)
                    {
                        bt_trace(TRACE_GROUP_1, BT_PANIC_MESSAGE7);
                        HciTransportError();
                        return;
                    }
                    HCC(aclRxPacketsLeft)--;
                    HCC(flags) |= HCI_FLAG_PACKETS_COMPLETE;
                    HCC(con)[entry].packetsComplete++;
                #endif /* 1 */
                }
            }
        #if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
#ifdef __BT_3_0_HS__                
            else (ampData ==0)
#else
            else
#endif
            {
                /* Update the number of receive packets left */
                if (entry != INVALID_HCI_HANDLE)
                {
                    Assert(HCC(scoRxPacketsLeft) != 0);
                    HCC(scoRxPacketsLeft)--;
                    HCC(flags) |= HCI_FLAG_PACKETS_COMPLETE;
                    HCC(con)[entry].packetsComplete++;
                }
            }
        #endif /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */
        #endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

        }

        /* Return the buffer to the driver */
        OS_StopHardware();
        RXBUFF_Free(hciRxBuffer);
        HCC(tranEntry).buffAvail(hciRxBuffer->flags);
    }
    OS_ResumeHardware();
}


#ifdef __BT_3_0_HS__

/*****************************************************************************
 * FUNCTION
 *  HciProcessReceivedData
 * DESCRIPTION
 *  Process any received ACL, SCO or event data.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void HciProcessAMPReceivedData(U16 hci_con, U16 len, U8 *buffer)
{
    HciBuffer hciRxBuffer;

    hciRxBuffer.len = len;
    hciRxBuffer.buffer = buffer;
    L2CAP_HciReceiveAMP(&hciRxBuffer, hci_con);
    HCC(flags) |= HCI_FLAG_PACKETS_COMPLETE;
}
#endif

#if HCI_HOST_FLOW_CONTROL == XA_ENABLED
/*****************************************************************************
 * FUNCTION
 *  HciSendCompletedPackets
 * DESCRIPTION
 *  When Controller to Host flow control is enabled, the
 *  number of packets completed must be sent to the controller.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void HciSendCompletedPackets(void)
{
    U8 numHandles;
    I16 i;

#if 1
    if (((HCC(flags) & HCI_FLAG_PACKETS_COMPLETE) &&
        ((HCC(aclRxPacketsLeft) <= (HCI_NUM_RX_ACL_BUFFERS / 2))
#else /* 0 */
    if (((HCC(flags) & HCI_FLAG_PACKETS_COMPLETE) && ((HCC(aclRxPacketsLeft) == 0)
#endif /* 0 */
#if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
                                                      || (HCC(scoRxPacketsLeft) == 0)
#endif /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */
         )) && (!IsNodeOnList(&(HCC(commandList)), &(HCC(cmd).node))) &&
        (!IsNodeOnList(&(HCC(sentCommandList)), &(HCC(cmd).node))) &&
        (BT_CheckSendHostNumberOfCompletedPacket() == 0x00)
        )
    {

        /* Set up the command */
        numHandles = 0;
        for (i = 0; i < HCI_NUM_HANDLES; i++)
        {
            if ((HCC(con)[i].conFlags & HCI_ACTIVE_CON) && (HCC(con)[i].packetsComplete))
            {

                /* Add an entry to the command parameters */
                StoreLE16(&(HCC(cmd).parms[(numHandles * 4) + 1]), HCC(con)[i].hciHandle);
                StoreLE16(&(HCC(cmd).parms[(numHandles * 4) + 3]), HCC(con)[i].packetsComplete);
            #ifdef UART_DUMP
                bt_trace(TRACE_GROUP_1, BTLOG_HCI_HOST_NUM_COMPLETED_PACKETS_HANDLE_PACKETS, HCC(con)[i].hciHandle, HCC(con)[i].packetsComplete);
            #endif
                HCC(con)[i].packetsComplete = 0;
                numHandles++;
            }
        }
        HCC(flags) &= ~HCI_FLAG_PACKETS_COMPLETE;

        if (numHandles)
        {
            HCC(cmd).parms[0] = numHandles;
            BT_XFile_EncryptionCommand(HCC(cmd).parms, (U8) (numHandles * 4 + 1));
            HciTransmitCommand(BT_XFile_EncryptionCommandOpcode(HCC_HOST_NUM_COMPLETED_PACKETS),
                            (U8) (numHandles * 4 + 1), &HCC(cmd), TRUE, TRUE);
        }

        HCC(aclRxPacketsLeft) = HCI_NUM_RX_ACL_BUFFERS;

    #if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
        HCC(scoRxPacketsLeft) = HCI_NUM_SCO_BUFFERS;
    #endif /* HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED) */
    }
}
#endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  HciSendCommands
 * DESCRIPTION
 *  Send any pending HCI commands.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void HciSendCommands(void)
{
    HciCommand *hciCommand;
    HciPacket *hciPacket;
    U8 cmdsSent = 0;

    //bt_prompt_trace(MOD_BT, "[BT] +HciSendCommands");
    while (!IsListEmpty(&(HCC(commandList))))
    {
        //bt_prompt_trace(MOD_BT, "[BT] HCC(commandList) is not empty");
        if (HCC(maxCommands) && (cmdsSent < HCI_NUM_COMMANDS) && (HCC(tranEntry).sendData != 0))
        {
            /* Send the command */
            hciCommand = (HciCommand*) RemoveHeadList(&(HCC(commandList)));

            /* Get a transmit packet */
            if (!IsListEmpty(&(HCC(hciPacketPool))))
            {
                hciPacket = (HciPacket*) RemoveHeadList(&(HCC(hciPacketPool)));
            }
            else
            {
                /* No transmit packets left */
                InsertHeadList(&HCC(commandList), &(hciCommand->node));
                break;
            }

            if (LEtoHost16(hciCommand->header) == HCC_CREATE_CONNECTION)
            {
                HCC(conReqs)++;
            }

            /* Initialize the transmit packet */
            hciPacket->flags = HCI_BUFTYPE_COMMAND;
            OS_MemCopy(hciPacket->header, hciCommand->header, 3);
            hciPacket->f[0].buffer = hciCommand->parms;
            hciPacket->f[0].len = hciCommand->header[2];
            if (hciPacket->f[0].len)
            {
                hciPacket->fCount = 1;
            }
            else
            {
                hciPacket->fCount = 0;
            }

            HCC(tranEntry).sendData(hciPacket);

            cmdsSent++;

        #ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER
        {
            U16 opcode_encrypted;
            U16 opcode_decrypted;
        
            opcode_encrypted = LEtoHost16(hciCommand->header);
            opcode_decrypted = ((opcode_encrypted & 0x00FF) ^ BT_XFileGetKey0()) | ((((opcode_encrypted & 0xFF00)>>8)^ BT_XFileGetKey1()) << 8);
            if (opcode_decrypted == 0xFC83){
                bt_prompt_trace(MOD_BT, "[FmOverA2DP] Send 0xFC83 (Enable FmOverA2DP)");
                HCC(fmovera2dp_enabled) = 1;
            }
        }        
        #endif


        #if HCI_HOST_FLOW_CONTROL == XA_ENABLED
            {
                U16 opcode_encrypted;
                U16 opcode_decrypted;

                opcode_encrypted = LEtoHost16(hciCommand->header);
                opcode_decrypted = ((opcode_encrypted & 0x00FF) ^ BT_XFileGetKey0()) | ((((opcode_encrypted & 0xFF00)>>8)^ BT_XFileGetKey1()) << 8);
                if (opcode_decrypted != HCC_HOST_NUM_COMPLETED_PACKETS)
                {
                    HCC(maxCommands)--;
                    Report(("HCC(maxCommands):%d",HCC(maxCommands)));
                }
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
                if(opcode_decrypted == HCC_MT66XX_Enabled_FLOW_CONTROL)
                    UARTTRAN_EnableMT66xxFlowControl();
#endif
            }
        #else /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */
            HCC(maxCommands)--;
        #endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

            /* Insert the command on the sent list, to be completed later */
            InsertTailList(&(HCC(sentCommandList)), &(hciCommand->node));

        }
        else
        {
            /* Cannot send right now */
            break;
        }
    }
    //bt_prompt_trace(MOD_BT, "[BT] -HciSendCommands");
}

/*****************************************************************************
 * FUNCTION
 *  HciSendPendingTxQueue
 * DESCRIPTION
 *  See if any pending shall send to UART.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void HciSendPendingTxQueue(void)
{
    //bt_prompt_trace(MOD_BT, "[BT] +HciSendPendingTxQueue");
    HCC(tranEntry).reSendData();
    //bt_prompt_trace(MOD_BT, "[BT] -HciSendPendingTxQueue");
}

/*****************************************************************************
 * FUNCTION
 *  HciCheckForTxDataSend
 * DESCRIPTION
 *  See if any connections have data to send.  Data is
 *  taken one packet at a time from each connection and transmitted.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void HciCheckForTxDataSend(void)
{
    HciCallbackParms cbParms;
    I16 i;
    
    //bt_trace(TRACE_FUNC, FUNC_HCI_CHECK_FOR_TX_DATA_SEND);
    HCC(flags) &= ~HCI_FLAG_DATA_QUEUED;
    for (i = 0; i < HCI_NUM_HANDLES; i++)
    {
        if ((HCC(state) == HCI_STATE_READY) && (HCC(con)[HCC(txCon)].conFlags & HCI_ACTIVE_CON) &&
            (HCC(con)[HCC(txCon)].conFlags & HCI_DATA_REQ) && (HCC(tranEntry).sendData != 0))
        {

            /* Data is available to be sent on this (active) connection */

            if (HCC(con)[HCC(txCon)].remaining != 0)
            {
                /*
                 * There is still data to be sent in the current BtPacket,
                 * * continue sending the data.
                 */
                HCC(flags) |= HCI_FLAG_DATA_QUEUED;
                if (SendHciPackets(HCC(con)[HCC(txCon)].hciHandle) != BT_STATUS_SUCCESS)
                {
                    break;
                }
            }
            else if (HCC(con)[HCC(txCon)].btPacket == 0)
            {
                /*
                 * There is no outstanding data to be sent from the current BtPacket
                 * * Data must be requested from an upper layer.
                 */

                Assert((HCC(con)[HCC(txCon)].conFlags & HCI_DATA_TYPE_ACL) ||
                       (HCC(con)[HCC(txCon)].conFlags & HCI_DATA_TYPE_SCO));

                /* Call for data from ME */
                cbParms.status = BT_STATUS_SUCCESS;

                if (HCC(con)[HCC(txCon)].hciHandle & (PICONET_BROADCAST | ACTIVE_BROADCAST))
                {

                    /*
                     * Don't send broadcast data if a connect request
                     * is outstanding.  This should avoid the broadcast handle
                     * race condition.
                     */
                    if (HCC(conReqs))
                    {
                        continue;
                    }

                    /* Convert a dynamic broadcast handle to a static one */
                    if (HCC(con)[HCC(txCon)].hciHandle == HCC(activeBcHandle))
                    {
                        cbParms.hciHandle = ACTIVE_BC_HANDLE;
                    }
                    else
                    {
                        cbParms.hciHandle = PICONET_BC_HANDLE;
                    }
                }
                else
                {
                    cbParms.hciHandle = HCC(con)[HCC(txCon)].hciHandle;
                }
                cbParms.morePacketWaiting = 0;
                if (HCC(radioEntry))
                {
                    if (HCC(metaMode))
                    {
                        Hci_MetaDataCallback(HCI_SEND_IND, &cbParms);
                    }
                    else
                    {
                        HCC(radioEntry) (HCI_SEND_IND, &cbParms);
                    }
                }
                else
                {
                    ME_Callback(HCI_SEND_IND, &cbParms);
                }

                if (cbParms.ptr.packet)
                {

                    /* There is a new packet to send, set it up */
                    XASTAT_TimerStart(hci_tx_timer, cbParms.ptr.packet->hci_timer);
                    Assert(HCC(con)[HCC(txCon)].btPacket == 0);
                    HCC(con)[HCC(txCon)].btPacket = cbParms.ptr.packet;
                    HCC(con)[HCC(txCon)].btPacket->hciPackets = 0;
                    HCC(con)[HCC(txCon)].buffPtr = HCC(con)[HCC(txCon)].btPacket->header;
                    HCC(con)[HCC(txCon)].remaining = HCC(con)[HCC(txCon)].btPacket->dataLen + HCC(con)[HCC(txCon)].btPacket->headerLen;

                    if (HCC(con)[HCC(txCon)].btPacket->flags & BTP_FLAG_TAIL)
                    {
                        HCC(con)[HCC(txCon)].remaining += HCC(con)[HCC(txCon)].btPacket->tailLen;
                    }

                #if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
                    if (HCC(con)[HCC(txCon)].btPacket->flags & BTP_FLAG_FCS)
                    {
                        HCC(con)[HCC(txCon)].remaining += 2;
                    }
                #endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */

                    /* Insert the packet on the list of packets for this connection */

                    InsertTailList(&(HCC(con)[HCC(txCon)].btPacketList), &(cbParms.ptr.packet->node));
                }

                if (cbParms.ptr.packet)
                {
                    /* A packet was received, send it */
                    HCC(flags) |= HCI_FLAG_DATA_QUEUED;
                    if (SendHciPackets(HCC(con)[HCC(txCon)].hciHandle) != BT_STATUS_SUCCESS)
                    {
                        break;
                    }
                }
                else
                {
                    /* No more data to send on this connection */
                    if (cbParms.morePacketWaiting != 1)
                    {
                        HCC(con)[HCC(txCon)].conFlags &= ~HCI_DATA_REQ;
                    }
                    if (HCC(con)[HCC(txCon)].hciHandle & (HCC(flags) << 12))
                    {
                        HCC(flags) &= ~(HCI_FLAG_PICONET_BC | HCI_FLAG_ACTIVE_BC);
                    }
                }
            }
        }

        /* Setup the next connection for transmit */
        HCC(txCon)++;
        if (HCC(txCon) >= HCI_NUM_HANDLES)
        {
            HCC(txCon) = 0;
        }
    }
    //bt_prompt_trace(MOD_BT,"[BT] -HciCheckForTxDataSend");
}

#ifdef __BT_3_0_HS__
extern U8 UARTRAN_AMP_ForceTxData();
extern pal_ilm_struct dlight_ilm;
extern U16 dlight_sequence_no;
U8 amp_tx_result;
pal_bt_data_command_struct *amp_tx_bt_data_command = NULL;


void HciCheckForTxAMPDataSend(void)
{
    HciCallbackParms cbParms;
    I16 i;
    HciConnection *ampConn = NULL;
    U8 TxCount =0;
    
    HCC(flags) &= ~HCI_FLAG_DATA_QUEUED;

    for (i = 0; i < HCI_NUM_HANDLES; i++)
    {
        if ((HCC(con)[HCC(txCon)].conFlags & HCI_ACTIVE_CON) &&
            (HCC(con)[HCC(txCon)].conFlags & HCI_DATA_REQ))
        {
            if (HCC(con)[HCC(txCon)].btPacket == 0)
            {

                /* Call for data from ME */
                cbParms.status = BT_STATUS_SUCCESS;

                cbParms.hciHandle = HCC(con)[HCC(txCon)].hciHandle;

                for(TxCount=0; TxCount < 20; TxCount++)
                {
                    cbParms.morePacketWaiting = 0;
                    ME_Callback(HCI_SEND_IND_AMP, &cbParms);
                    if (cbParms.ptr.packet) 
                    {
                        ampConn = HciGetAMPHciConnection(cbParms.ampHandle);
                        if(ampConn ==0)
                        {
                            //Assert(0);
                            Report(("[ERR] Get a AMP con address is 0 ! ignore it!"));
                            return;
                        }
                        /* There is a new packet to send, set it up */
                        ampConn->btPacket = cbParms.ptr.packet;
                        ampConn->btPacket->hciPackets = 0;
                        ampConn->buffPtr = ampConn->btPacket->header;
                        ampConn->remaining = ampConn->btPacket->dataLen + ampConn->btPacket->headerLen;
                        if (ampConn->btPacket->flags & BTP_FLAG_TAIL)
                        {
                            ampConn->remaining += ampConn->btPacket->tailLen;
                        }
                    
                        if (ampConn->btPacket->flags & BTP_FLAG_FCS)
                        {
                            ampConn->remaining += 2;
                        }
                    
                        /* Insert the packet on the list of packets for this connection */
                        InsertTailList(&(ampConn->btPacketList), &(cbParms.ptr.packet->node));
                    }
                    if (cbParms.ptr.packet)
                    {
                        /* A packet was received, send it */
                        HCC(flags) |= HCI_FLAG_DATA_QUEUED;
                        if (SendAMPHciPackets(ampConn->hciHandle) == BT_STATUS_PENDING)
                        {
                            amp_tx_result = BT_SendPalMessage(MSG_ID_PAL_BT_DATA_COMMAND, &dlight_ilm, sizeof(pal_bt_data_command_struct));
                        	SendAMPHciPackets(ampConn->hciHandle);
                    	}
                        amp_tx_result = BT_SendPalMessage(MSG_ID_PAL_BT_DATA_COMMAND, &dlight_ilm, sizeof(pal_bt_data_command_struct));
                        
                    }
                    else 
                    {
                        if (cbParms.morePacketWaiting != 1)
                        {
                            HCC(con)[HCC(txCon)].conFlags &= ~HCI_DATA_REQ;
                        }                
                        break;
                    }
                }
            }
        }

        /* Setup the next connection for transmit */
        HCC(txCon)++;
        if (HCC(txCon) >= HCI_NUM_HANDLES)
        {
            HCC(txCon) = 0;
        }
    }
    return;
}
#endif

#if 0
/* HciCheckForTxData() and HciCheckForTxCmd() are used in old uart transmission mechanism before W0914 */

/*****************************************************************************
 * FUNCTION
 *  HciCheckForTxData
 * DESCRIPTION
 *  See if any connections have data to send.  Data is
 *  taken one packet at a time from each connection and transmitted.
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
static U8 HciCheckForTxData(void)
{
    I16 i;

    for (i = 0; i < HCI_NUM_HANDLES; i++)
    {
        if ((HCC(state) == HCI_STATE_READY) && (HCC(con)[HCC(txCon)].conFlags & HCI_ACTIVE_CON) &&
            (HCC(con)[HCC(txCon)].conFlags & HCI_DATA_REQ) && (HCC(tranEntry).sendData != 0))
        {

            /* Data is available to be sent on this (active) connection */
            HCC(txCon) = 0;
            return TRUE;
        }

        /* Setup the next connection for transmit */
        HCC(txCon)++;
        if (HCC(txCon) >= HCI_NUM_HANDLES)
        {
            HCC(txCon) = 0;
        }
    }
    return FALSE;
}

/* HciCheckForTxData() and HciCheckForTxCmd() are used in old uart transmission mechanism before W0914 */
/*****************************************************************************
 * FUNCTION
 *  HciCheckForTxCmd
 * DESCRIPTION
 *  See if any connections have command to send.  Data is
 *  taken one packet at a time from each connection and transmitted.
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
static U8 HciCheckForTxCmd(void)
{
    if (!IsListEmpty(&(HCC(commandList))))
    {
        bt_trace(TRACE_GROUP_1, HCI_PROCESS_HCICHECKFORTXCMD);
        return TRUE;

    }

    return FALSE;
}
#endif

/*****************************************************************************
 * FUNCTION
 *  HCI_Process
 * DESCRIPTION
 *  Process all receive data and events.  Also, process any request
 *  to send data.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void HCI_Process(void)
{
    HciCallbackParms cbParms;

#if L2CAP_NUM_GROUPS > 0
    U16 entry;
#endif /* L2CAP_NUM_GROUPS > 0 */

    //bt_trace(TRACE_FUNC, FUNC_HCI_PROCESS);
    if ((HCC(state) == HCI_STATE_SHUTDOWN) || (HCC(state) == HCI_STATE_DEINIT))
    {
        return;
    }

    /* See if there are any transport errors */
    OS_StopHardware();
    if (HCC(transportError))
    {
        OS_ResumeHardware();
        if (HCC(reset_received))
        {
            /* HCI Transport Error */
            cbParms.status = BT_STATUS_FAILED;
            if (HCC(state) == HCI_STATE_INIT)
            {
                /* Report init status */
                if (HCC(radioEntry))
                {
                    bt_trace(TRACE_GROUP_1, HCI_PROCESS_FALURE);
                    HCC(radioEntry) (HCI_INIT_STATUS, &cbParms);
                }
                else
                {
                    HCI_RadioInitialized(BT_STATUS_FAILED);
                }
            }
            else
            {
                /* Report the transport error */
                if (HCC(radioEntry))
                {
                    HCC(radioEntry) (HCI_TRANSPORT_ERROR, &cbParms);
                }
                else
                {
                    ME_Callback(HCI_TRANSPORT_ERROR, &cbParms);
                }

                /* Reset the HCI */
                if (RMGR_HciReset() == BT_STATUS_FAILED)
                {
                    HCI_RadioInitialized(BT_STATUS_FAILED);
                }
            }
            HCC(transportError) = FALSE;
            HCC(reset_received) = FALSE;
        }
        return;
    }
    OS_ResumeHardware();
    /* See if any HCI transmit packets were returned */
    HciProcessSentPackets();
    /* See if any data has been received */
    HciProcessReceivedData();

#if HCI_HOST_FLOW_CONTROL == XA_ENABLED
    /* Send the number of completed packets command if necessary */
    HciSendCompletedPackets();
#endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

    /* See if any commands must be sent */
    HciSendCommands();

    /* See if any connections have data to send */
    HciCheckForTxDataSend();
#ifdef __BT_3_0_HS__
    HciCheckForTxAMPDataSend();
#endif
    HciSendPendingTxQueue();

#if L2CAP_NUM_GROUPS > 0
    /* Check for a broadcast */
    if ((HCC(flags) & HCI_FLAG_PICONET_BC) || (HCC(flags) & HCI_FLAG_ACTIVE_BC))
    {
        /* A broadcast has been requested, see if all queues are empty */
        if (!(HCC(flags) & HCI_FLAG_DATA_QUEUED))
        {
            /*
             * No data is queued on any connection Set up the broadcast to be
             * transmitted.
             */
            if (HCC(flags) & HCI_FLAG_PICONET_BC)
            {
                entry = HciGetTableEntry(HCC(piconetBcHandle));
            }
            else
            {
                entry = HciGetTableEntry(HCC(activeBcHandle));
            }
            Assert(entry != INVALID_HCI_HANDLE);
            HCC(con)[entry].conFlags |= HCI_DATA_REQ;
            OS_NotifyEvm();
        }
    }
#endif /* L2CAP_NUM_GROUPS > 0 */
}


/*****************************************************************************
 * FUNCTION
 *  HciCheckWifiSetting
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *
 *****************************************************************************/
BOOL HciCheckWifiSetting(void)
{
    {
        U8 *temp;
        if(MEC(BT_TempKey) != 0)
        {
            temp = (U8 *)get_ctrl_buffer(4);
            OS_MemCopy(temp, MEC(BT_TempKey), 4);
            free_ctrl_buffer(MEC(BT_TempKey));
            MEC(BT_TempKey) = temp;
        }
    }
    if ((BTWIFIC(afh_cmd_made) == 1) || (BTWIFIC(pta_cmd_made) == 1))
    {
        return FALSE;
    }
    if (BTWIFIC(afh_cmd_wait) != 0)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTLOG_BTWIFICAFH_CMD_WAIT_0);
        if(MeWriteAFH())
        {
            BTWIFIC(afh_cmd_made) = 1;
            BTWIFIC(afh_cmd_wait) = 0;
            OS_NotifyEvm();
            return FALSE;
        }
    }
    if (BTWIFIC(pta_cmd_wait) == 1)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTLOG_MAKE_THE_PTA_ON);
        BTWIFIC(pta_cmd_wait) = 0;
        if (MeWritePTA())
        {
            BTWIFIC(pta_cmd_made) = 1;
            BTWIFIC(pta_cmd_wait) = 0;
            OS_NotifyEvm();
            return FALSE;
        }
    }
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  HCI_WifiCommandTimeoutHandler
 * DESCRIPTION
 *
 * PARAMETERS
 *  Timer       [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void HCI_WifiCommandTimeoutHandler(EvmTimer *Timer)
{
    if (!HCC(radioEntry))
    {
        if ((HCC(state) == HCI_STATE_READY) && (MEC(stackState) == BTSS_INITIALIZED))
        {
            if (!HciCheckWifiSetting())
            {
                bt_trace(TRACE_GROUP_1, WIFI_SETTING_IS_MADE);
            }
        }
    }
    HCC(wifisettingTimer).func = HCI_WifiCommandTimeoutHandler;
    EVM_StartTimer(&HCC(wifisettingTimer), 5000);
}

U8 HciGetControllerType(U16 con_hdl)
{
    if(con_hdl < 0x40)
    {
        return HCI_AMP_CONTROLLER_BREDR;
    }
    else
    {
        return HCI_AMP_CONTROLLER_WIFI;
    }
     
}

HciConnection *HciGetAMPHciConnection(U16 hdl)
{
    U8 i=0;
    if (HCC(state) != HCI_STATE_READY)
    {
        return 0;
    }
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("HciGetAMPHciConnection:%d", hdl));
#endif        
    for(i=0;i< HCI_NUM_HANDLES;i++)
    {
#if __BT_30_DEBUG__ == XA_ENABLED
        Report(("AMP con state:%d,%d",HCC(ampCon)[i].conFlags, HCC(ampCon)[i].hciHandle));
#endif
        if ((HCC(ampCon)[i].conFlags & HCI_ACTIVE_CON) &&
            (HCC(ampCon)[i].hciHandle == hdl))
        {
            return (HciConnection *)&(HCC(ampCon)[i]);
        }
    }
    return 0x00;
}

