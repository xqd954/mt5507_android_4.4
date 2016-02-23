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
 *     $Workfile:l2cap_if.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains the lower layer functions L2CAP exports
 *     to the HCI and the control functions used by the Management
 *     entity.
 *
 * Created:
 *     July 15, 1999
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
#include "bt_feature.h"
#include "btalloc.h"
#include "sys/l2cap_i.h"
#include "sys/mei.h"

extern BOOL BTCheckPktServicedTimeLong(U32 int_time, U32 time_value);
extern L2CAP_Channel *L2Cap_GetValidChannelFixChannelCid03(BtRemoteDevice *link);
extern BtPacket *L2Cap_BuildSFrameSREJ(L2CAP_Channel *Channel, BtPacket *Packet);
extern BOOL L2Cap_IsConfigedOpen(L2CAP_Channel *Channel);

/****************************************************************************
 *
 * Internal Function prototypes
 *
 ****************************************************************************/
static void L2Cap_ProcessSignal(U8 *RxData, U16 RxDataLen, BtRemoteDevice *Link);

#if L2CAP_NUM_GROUPS > 0
static void L2Cap_ProcessGroupData(U8 *RxData, U16 RxDataLen, BtRemoteDevice *Link);
#endif 

#ifdef __BT_4_0_BLE__
static void L2Cap_ProcessBLESignal(U8 *RxData, U16 RxDataLen, BtRemoteDevice *Link);
static void L2Cap_ProcessBLEAttData(U8 *RxData, U16 RxDataLen, BtRemoteDevice *Link, U16 cid);
#endif

static void L2Cap_ProcessChannelData(U8 *RxData, U16 RxDataLen, L2CAP_Channel *Channel);
static BOOL L2Cap_HandleRxError(BtRemoteDevice *link);

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
static void L2Cap_ChannelReceive(L2CAP_Channel *Channel, HciBuffer *BuffDesc);
static void L2Cap_ProcessIFrame(L2CAP_Channel *Channel, U8 *rxBuffer);
static void L2Cap_ProcessStreamingIFrame(L2CAP_Channel *Channel, U8 *rxBuffer);
static void L2CAP_ProcessReceiveFlowPDU(L2CAP_Channel *Channel, HciBuffer *BuffDesc, U8 source);
static void L2Cap_ProcessSFrame(L2CAP_Channel *Channel, U8 *rxBuffer);
static BOOL VerifyAndUpdateSar(L2CAP_Channel *Channel, U8 Sar);
void L2Cap_ReturnflowRxBuffer(L2CAP_Channel *Channel);

#ifdef __BT_4_0_BLE__
void L2CAP_LinkConnectedByCid(BtRemoteDevice *Link, U16 cid);
#endif
U16 L2Cap_GetSequenceWrapValue(L2CAP_Channel *Channel);
void L2Cap_ProcessBLERsp(U8 *RxData, U16 RxDataLen);
void L2Cap_ProcessBLEReq(U8 *RxData, U16 RxDataLen, BtRemoteDevice *Source);

void L2CAP_AMPResumeFrameResponder(L2CAP_Channel *Channel);
void L2Cap_SendREJ(L2CAP_Channel *Channel);
BOOL L2Cap_EnhancedVerifyReqSeqInRxMissingRejectTableHead(L2CAP_Channel *Channel, U16 ReqSeq);
void L2Cap_EnhancedRemoveAllFromRxMissingRejectTable(L2CAP_Channel *Channel);
void L2Cap_EnhancedRetransmissionSendAck(L2CAP_Channel *Channel, U8 sendFlag);
void L2Cap_ProcessRecvPorFBit(L2CAP_Channel *Channel, U8 f_control, U8 p_control);
void L2Cap_EnhancedRemoveFromRxMissingRejectTable(L2CAP_Channel *Channel, U16 ReqSeq);
BOOL L2Cap_EnhancedVerifyReqSeqInRxMissingRejectTable(L2CAP_Channel *Channel, U16 ReqSeq);
void BTCoreGetSystemTick5(void);
BOOL L2Cap_ProcessReqSeqNotUpdateValue(L2CAP_Channel *Channel, U16 Control, U16 *ReqSeq);
void L2CAP_AMPResumeFrameInitiator(L2CAP_Channel *Channel);
U16 L2Cap_GetUnAckNumber(L2CAP_Channel *Channel, U16 wrapValue);
U16 L2Cap_EnhancedGetRxMissingRejectTableHead(L2CAP_Channel *Channel);
void L2Cap_SendSREJ(L2CAP_Channel *Channel, U16 ReqSeq);
void L2Cap_EnhancedRetransmissionPassToTxFBit(L2CAP_Channel *Channel, U8 f_control, U16 ReqSeq);
void L2Cap_EnhancedRetransmissionSrejResentIFrame(L2CAP_Channel *Channel, U16 ReqSeq);
void BTCoreGetSystemTick6(void);



    



#if UPF_TWEAKS == XA_ENABLED
BOOL DropFrame = FALSE;
#endif 
#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */ 

#define __Dlight_TEST_TX__

/*****************************************************************************
 * FUNCTION
 *  L2CAP_HciReceive
 * DESCRIPTION
 *  Called by HCI when data arrives on an ACL Connection.
 * PARAMETERS
 *  BuffDesc        [IN]        
 *  HciHndl         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2CAP_HciReceive(const HciBuffer *BuffDesc, HciHandle HciHndl)
{
    U16 len, cid, copy;
    BtRemoteDevice *link;
    HciBuffer hciBuffer = *BuffDesc;
    L2CAP_Channel *channel = 0;

    link = MeMapHciToRemoteDevice((HciHandle) (HciHndl & 0x0FFF));
#if BT_SCO_HCI_DATA == XA_DISABLED
    if (link == 0)
    {
        /*
         * This is possible if BT_SCO_HCI_DATA support is disabled but
         * the radio still indicates SCO packets to the HCI instead of
         * directing them to the PCM or other audio hardware.
         */
        bt_trace(TRACE_GROUP_1, L2CAP_DISCARDING_DATA_FROM_UNKNOWN_HCIHANDLE);
        return;
    }
#endif /* BT_SCO_HCI_DATA == XA_DISABLED */ 
    if((link == 0) && (HciHndl < 32))
    {
        return;
    }
    Assert(link);

    if (link->rxState == BRXS_DISCONNECTING)
    {
        return;
    }
#if defined (__BT_2_1_AUTO_FLUSH__)
    if (HciHndl & FIRST_AUTO_FLUSH_PACKET)
    {
#else /* defined (__BT_2_1_AUTO_FLUSH__) */ 
    if (HciHndl & FIRST_PACKET)
    {
#endif /* defined (__BT_2_1_AUTO_FLUSH__) */ 
#if 0
        if ((link->rxState == BRXS_COPYING) || (link->rxState == BRXS_FORWARDING))
        {
            /*
             * Error! Throw away buffered data and handle possible QoS violation.
             * * Since this is the start of a new packet we can accept this
             * * packet if the handler doesn't disconnect the link. 
             */
            Report(("RX error 1"));
            if (L2Cap_HandleRxError(link))
            {
                return;
            }
        }
#endif        
        link->rxBufferLen = 0;
        link->rxState = BRXS_COPYING;
    }
    link->lastTxRxTime = OS_GetSystemTime();
    if ((link->rxState != BRXS_COPYING) && (link->rxState != BRXS_FORWARDING))
    {
        return;
    }
    Assert(link->rxBuffer);

    /* We only need 4 bytes to proceed, but try to stage 8 bytes for BT1.2 PDU's. */
    if ((link->rxBufferLen < 4) && (link->rxState == BRXS_COPYING))
    {
        copy = min((4 - link->rxBufferLen), hciBuffer.len);
        OS_MemCopy(link->rxBuffer + link->rxBufferLen + L2CAP_PRELUDE_SIZE, hciBuffer.buffer, copy);

        hciBuffer.len -= copy;
        hciBuffer.buffer += copy;
        link->rxBufferLen += copy;
    }
    if (link->rxBufferLen < 4)
    {
        /* Need a minimum of 4 bytes in the receive buffer to proceed. */
        return;
    }

    /* We've received enough data to pull out the PDU length and channel id. */
    len = LEtoHost16((link->rxBuffer + L2CAP_PRELUDE_SIZE));
    cid = LEtoHost16((link->rxBuffer + L2CAP_PRELUDE_SIZE + 2));

    /* If this is a channel data packet, make sure the channel is valid. */
    if ((cid >= BASE_DYNAMIC_CID && cid <= LAST_DYNAMIC_CID) ||
		(cid == 0x0003))
    {
#ifdef __BT_3_0_HS__
        if (cid == 0x0003)
        {
            Report(("L2Cap_Process A2MP Protocol"));
            channel = L2Cap_GetValidChannelFixChannelCid03(link);
        }
        else
            channel = L2Cap_GetValidChannel(cid);
#else
        channel = L2Cap_GetValidChannel(cid);
#endif
        if ((channel == 0) || ((FALSE == L2Cap_IsConfigedOpen(channel)) && (channel->state != L2C_CONFIG)))
        {
            if( 0 != channel ){
                bt_trace(TRACE_GROUP_1, STATUSBT_DEBUGx02X, channel->state);
            }
            bt_trace(TRACE_GROUP_1, L2CAP_CHANNELRXBT_DEBUGx04X__DROPPED_DATA__NO_CHANNEL, cid);
            link->rxState = BRXS_FLUSH;
            return;
        }

    #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
        if (channel->inLinkMode != LINK_MODE_BASIC)
        {
            /* Need to make sure the control bytes are in the stage buffer. */
            link->rxState = BRXS_FORWARDING;
            L2CAP_ProcessReceiveFlowPDU(channel, &hciBuffer, 0);
            /* The I freame will be sent to upper or keep in buffer */
            return;
        }

        if (channel->rxSduBuffer)
        {
            /* The Basic Mode */
            link->rxState = BRXS_FORWARDING;
#ifdef __BT_3_0_HS__                    
            if(cid >= 0x40)
            {
                link->lastTxRxOverDataChannel = OS_GetSystemTime();            
            }
#endif  
            L2Cap_ChannelReceive(channel, &hciBuffer);
            return;
        }
    #endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */
    }

    Assert(link->rxState == BRXS_COPYING);

    /* BT1.1 PDU's are received into common remote device buffer. */
    if (link->rxBufferLen + hciBuffer.len > L2CAP_MTU + 6)
    {
//        Report(("link->rxBufferLen + hciBuffer.len > L2CAP_MTU + 6"));
        /* Error! MTU violation, drop link, indicate violation to channels */
        Report(("RX error 2"));
        L2Cap_HandleRxError(link);
        return;
    }

    /* If this is a broadcast packet and broadcasts are disabled, silently flush. */
    if ((cid == 0x0002) && ((L2C(flags) & L2CCF_BROADCASTS_ENABLED) == 0))
    {
        link->rxState = BRXS_FLUSH;
        return;
    }

    /* Copy data into receive buffer */
    OS_MemCopy(link->rxBuffer + link->rxBufferLen + L2CAP_PRELUDE_SIZE, hciBuffer.buffer, hciBuffer.len);

    link->rxBufferLen += hciBuffer.len;

    /* If we have received the entire packet, pass it along. */
    if ((len + L2CAP_HEADER_MIN) == link->rxBufferLen)
    {
        link->rxState = BRXS_COMPLETE;

        XASTAT_Max(l2cap_mtu, len);

        /* Setup common callback parameters */
        L2C(callbackParms).event = L2EVENT_DATA_IND;
        L2C(callbackParms).status = BT_STATUS_SUCCESS;
        L2C(callbackParms).aclLink = link;

        if (channel)
        {
#ifdef __BT_3_0_HS__                    
            if(cid >= 0x40)
            {
                link->lastTxRxOverDataChannel = OS_GetSystemTime();            
            }
#endif  
            L2Cap_ProcessChannelData(link->rxBuffer + L2CAP_HEADER_MIN + L2CAP_PRELUDE_SIZE, len, channel);
        }
        else if (cid == 0x0001)
        {
            Report(("L2Cap_ProcessSignal"));
            L2Cap_ProcessSignal(link->rxBuffer + L2CAP_HEADER_MIN + L2CAP_PRELUDE_SIZE, len, link);
        }
#ifdef __BT_3_0_HS__
		else if (cid == 0x0003)
		{
			Assert(0);
		}
#endif
#ifdef __BT_4_0_BLE__
        else if ((cid == 0x0004))
        {
            L2CAP_LinkConnectedByCid(link, 0x0004);
            L2Cap_ProcessBLEAttData(link->rxBuffer + L2CAP_HEADER_MIN + L2CAP_PRELUDE_SIZE, len, link, 0x0004);
        }
        else if ((cid == 0x0006))
        {
            L2CAP_LinkConnectedByCid(link, 0x0006);
            L2Cap_ProcessBLEAttData(link->rxBuffer + L2CAP_HEADER_MIN + L2CAP_PRELUDE_SIZE, len, link, 0x0006);
        }

        else if (cid == 0x0005)
        {
            L2Cap_ProcessBLESignal(link->rxBuffer + L2CAP_HEADER_MIN + L2CAP_PRELUDE_SIZE, len, link);
        }
#endif
    #if L2CAP_NUM_GROUPS > 0
        else if (cid == 0x0002)
        {
            L2Cap_ProcessGroupData(link->rxBuffer + L2CAP_HEADER_MIN + L2CAP_PRELUDE_SIZE, len, link);
        }
    #endif /* L2CAP_NUM_GROUPS > 0 */ 
    }
    else if ((len + L2CAP_HEADER_MIN) < link->rxBufferLen)
    {
        /* Lost some fragments, received packet is larger than header claims. */
        Report(("RX error 3"));

        L2Cap_HandleRxError(link);
        return;
    }
}

/*****************************************************************************
 * FUNCTION
 *  L2CAP_HciReceive
 * DESCRIPTION
 *  Called by HCI when data arrives on an ACL Connection.
 * PARAMETERS
 *  BuffDesc        [IN]        
 *  HciHndl         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2CAP_HciReceiveAMP(HciBuffer *BuffDesc, HciHandle HciHndl)
{
    U16 len, cid;
    BtRemoteDevice *link;
    L2CAP_Channel *channel = 0;

    link = MeMapHciToRemoteDevice((HciHandle) (HciHndl & 0x0FFF));

    if(link == 0)
    {
        return;
    }

    link->lastTxRxTime = OS_GetSystemTime();

    /* We've received enough data to pull out the PDU length and channel id. */
    len = LEtoHost16((BuffDesc->buffer));
    cid = LEtoHost16((BuffDesc->buffer + 2));
    /* If this is a channel data packet, make sure the channel is valid. */
    if ((cid >= BASE_DYNAMIC_CID && cid <= LAST_DYNAMIC_CID) ||
		(cid == 0x0003))
    {

        channel = L2Cap_GetValidChannel(cid);
        if ((channel == 0) || ((FALSE == L2Cap_IsConfigedOpen(channel)) && (channel->state != L2C_CONFIG)))
        {
            if( 0 != channel ){
                bt_trace(TRACE_GROUP_1, STATUSBT_DEBUGx02X, channel->state);
            }
            bt_trace(TRACE_GROUP_1, L2CAP_CHANNELRXBT_DEBUGx04X__DROPPED_DATA__NO_CHANNEL, cid);
            return;
        }
        /* Need to make sure the control bytes are in the stage buffer. */
        L2CAP_ProcessReceiveFlowPDU(channel, BuffDesc,1);
        /* The I freame will be sent to upper or keep in buffer */
    }

}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_HciTxDone
 * DESCRIPTION
 *  Called by HCI to indicate that the packet was sent
 * PARAMETERS
 *  Packet      [IN]        
 *  Status      [IN]        
 *  HciHndl     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2CAP_HciTxDone(BtPacket *Packet, BtStatus Status, HciHandle HciHndl)
{
    L2CAP_Channel *channel;
    BtRemoteDevice *link;
    U16 cid;
    I16 i;

#if L2CAP_NUM_GROUPS > 0
    L2capGroup *group;
#endif 

    Assert(Packet && Packet->llpContext);

    /* Grab the Channel / Group Id */
    cid = Packet->llpContext;
    link = MeMapHciToRemoteDevice((HciHandle) (HciHndl & 0x0FFF));
    if (Packet->priority_type == BTP_FLAG_HIGH_PRIPORTY)
    {
        if (L2C(highPriorityNo) != 0)
        {
            L2C(highPriorityNo)--;
        }
    }
    else if (Packet->priority_type == BTP_FLAG_LOW_PRIPORTY)
    {
        if (L2C(lowPriorityNo) != 0)
        {
            L2C(lowPriorityNo)--;
        }
        if (L2C(lowPriorityInBB) != 0)
        {
            L2C(lowPriorityInBB)--;
        }
    }
    else if(Packet->priority_type == BTP_FLAG_NONE_PRIPORTY )
    {
        /* It is a internal packet */    
    }else{
        bt_prompt_trace(MOD_BT, "none l2cap priority packet:0x%x priotiry:%d", Packet, Packet->priority_type);
    }
    if (cid >= BASE_DYNAMIC_CID && cid <= LAST_DYNAMIC_CID)
    {
        /* It's a channel packet */
        channel = &L2C(channels)[CID_TO_INDEX(cid)];
        //OS_Report("L2Cap: cid:0x%x state:%d flag:0x%x link:0x%x link2:0x%x", cid, channel->state, Packet->flags, channel->link, link );
        if( channel->state == L2C_CLOSED ){
            OS_Report("L2Cap: closed cid:0x%x state:%d flag:0x%x link:0x%x link2:0x%x", cid, channel->state, Packet->flags, channel->link, link );
        }else{
            if( channel->link != link ){
                OS_Report("L2Cap: link not right cid:0x%x state:%d flag:0x%x HciHndl:0x%x link:0x%x link2:0x%x", cid, channel->state, Packet->flags, HciHndl, channel->link, link );
            }
        }
        Assert((channel->state == L2C_CLOSED) ? (Packet->flags & (BTP_FLAG_RDEV_FISRT | BTP_FLAG_RDEV_RETX| BTP_FLAG_LSYSTEM)) : (channel->link == link));

        if (((Packet->flags & BTP_FLAG_RDEV_FISRT) == 0) && (channel->state != L2C_CLOSED) && ((Packet->flags & BTP_FLAG_RDEV_RETX) == 0))
        {
            channel->l2capTxCount--;
        }

        L2C(callbackParms).aclLink = link;
        L2C(callbackParms).owner.psm = channel->psmInfo;

        if ((Packet->flags & BTP_FLAG_LSYSTEM))
        {

            /* It's a Signal Packet, run the state machine. */
            Packet->flags &= ~BTP_FLAG_INUSE;

            ReturnSysPkt(Packet);

            if ((Status == BT_STATUS_SUCCESS) && (channel->state != L2C_CLOSED))
            {
                L2Cap_StateMachine(channel, LLC_TX_DONE);
            }
        }
    #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
        else if ((Packet->flags & BTP_FLAG_RDEV_FISRT) || (Packet->flags & BTP_FLAG_RDEV_RETX))
        {
            /* It's a BtRemoteDevice packet. Return it. */
            InsertTailList(&link->txPacketList, &Packet->node);
            if(Packet->flags & BTP_FLAG_RDEV_RETX)
            {
                //                Report(("Dlight: BB send retrans"));
                /* 
                        It is possible txIFrameRetransmissionInBasebandCount is 0x00 
                        because txIFrameRetransmissionInBasebandCount will set to 0 during  
                        move channel process;
                        */
                if(channel->txIFrameRetransmissionInBasebandCount > 0)
                channel->txIFrameRetransmissionInBasebandCount--;
            }
            if(channel->inLinkMode == LINK_MODE_STREAMMING)
            {
                channel->ackSeq = (Packet->internalUsed +1) & (L2Cap_GetSequenceWrapValue(channel));
                L2Cap_ReturnSentPackets(channel, BT_STATUS_SUCCESS);
                channel->flagReportReturnPacketToUpper = 0;                
            }
            else if((channel->txIFrameRetransmissionInBasebandCount ==0) &&
                (channel->state <= L2C_OPEN) && (channel->state != L2C_CLOSED))
            {
//                Report(("Dlight: Report to upper 1"));            
                /* Only when no retransmission is in baseband, we can update the tx count to the upper layer */
                /* It can avoid tx packet is retransmission during the tx packet is been ack to the l2cap layer */
                L2Cap_ReturnSentPackets(channel, BT_STATUS_SUCCESS);
                channel->flagReportReturnPacketToUpper = 0;                
            }
            /* Tell the HCI that we want to send data. */
            L2Cap_IssueHciRequestToSend(link);
            goto CheckForResources;
        }
    #endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */
        else
        {
            /* It's an application packet. Return it to them. */
//            Assert(channel->state != L2C_CLOSED);

            XASTAT_TimerStop(Packet->dataLen, l2cap_tx_timer, Packet->l2cap_timer);

            /* Return packet to upper layer (application). */
            L2C(callbackParms).ptrs.packet = Packet;
            L2Cap_NotifyPsm(channel, L2EVENT_PACKET_HANDLED, (U16) Status);

        #if L2CAP_FLEXIBLE_CONFIG == XA_ENABLED
            if (channel->state == L2C_CONFIG)
            {
                /* Now run the State machine for this ConfigReq/Rsp Packet. */
                if (Status == BT_STATUS_SUCCESS)
                {
                    L2Cap_StateMachine(channel, LLC_TX_DONE);
                }
            }
        #endif /* L2CAP_FLEXIBLE_CONFIG == XA_ENABLED */ 
            return;
        }
        goto CheckForResources;
    }

    if (cid == 0x0001)
    {
        /* System packet used by L2CAP for a non-channel based signal. */

        Assert(Packet->flags & BTP_FLAG_LSYSTEM);

        Packet->flags &= ~BTP_FLAG_INUSE;

        ReturnSysPkt(Packet);

        goto CheckForResources;
    }
#ifdef __BT_4_0_BLE__
    if((cid >= 0x0004) || (cid <BASE_DYNAMIC_CID))
    {
        L2capFixCid *fixCid;

        fixCid = &L2C(fixCid)[cid-0x0004];
        L2C(callbackParms).aclLink = link;
        L2C(callbackParms).ptrs.packet = Packet;
        fixCid->inUse--;
        L2Cap_NotifyBLE(fixCid, L2EVENT_PACKET_HANDLED, (U16) Status, cid);
    }
#endif
#if L2CAP_NUM_GROUPS > 0
    /* It is a group packet that is done. Return it to the application. */

    group = L2C(groups)[GROUP_CID_TO_INDEX(cid)];
    Assert(group);

    group->l2capTxCount--;

    L2C(callbackParms).event = L2EVENT_PACKET_HANDLED;
    L2C(callbackParms).status = (U16) Status;
    L2C(callbackParms).ptrs.packet = Packet;
    L2C(callbackParms).owner.group = group;
    L2C(callbackParms).aclLink = 0;

    group->callback(cid, &L2C(callbackParms));

    if (IsListEmpty(&L2C(broadcastLink).txLowPriorityQueue))
    {
        MeRestartTransmitters();
    }

    return;
#endif /* L2CAP_NUM_GROUPS > 0 */ 

    /* Here we check if any Channels are waiting for system packets. */
  CheckForResources:

    if (L2C(flags) & L2CCF_NEED_SYSPACKET)
    {
        L2C(flags) &= ~L2CCF_NEED_SYSPACKET;

        for (i = 0; i < L2CAP_NUM_CHANNELS; i++)
        {
            if ((L2C(channels)[i].state > L2C_CLOSING) &&
                (L2C(channels)[i].flags & (L2CHF_SEND_REQUEST | L2CHF_SEND_RESPONSE)))
            {

                if (IsListEmpty(&L2C(signalPackets)))
                {
                    L2C(flags) |= L2CCF_NEED_SYSPACKET;
                    break;
                }

                L2Cap_StateMachine(&L2C(channels)[i], LLC_SEND_PACKET);

                if (L2C(channels)[i].flags & (L2CHF_SEND_REQUEST | L2CHF_SEND_RESPONSE))
                {
                    L2C(flags) |= L2CCF_NEED_SYSPACKET;
                }
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_HciTxReady
 * DESCRIPTION
 *  Called by the HCI when transmit resources are available.
 *  L2CAP returns the next packet in the transmit queue.
 * PARAMETERS
 *  parms       [IN]        
 * RETURNS
 *  transmit packet or null if the queue is empty
 *****************************************************************************/
#if L2CAP_FLOW_NON_BASIC_MODE== XA_DISABLED
BtPacket *L2CAP_HciTxReady(HciCallbackParms *parms, U8 controllerType)
{
    BtRemoteDevice *link;
    BtPacket *packet;
    BtPacket *txPacket = 0;
    ListEntry *queue = 0;
    I16 packetLen;
    I8 offset;
    HciHandle HciHndl;

    HciHndl = parms->hciHandle;
    parms->morePacketWaiting = 0;
    if (0 != (link = MeMapHciToRemoteDevice((HciHandle) (HciHndl & 0x0FFF))))
    {
        if (IsListEmpty(&(link->txHighPriorityQueue)))
        {

            /* Always guarantee high priority packet to send */
            if (IsListEmpty(&(link->txLowPriorityQueue)))
            {
                return 0;
            }
            /* no high priority traffic waiting to send */
            if (L2C(heavyTrafficSource) == L2C_HEAVY_TRAFFIC_EXIST)
            {
                /* A2DP existing, reserved the Bandwidth, 
                   only allow low priority to send 1 packet */

                if (L2C(highPriorityNo) > 2)
                {
                    if (!BTCheckPktServicedTimeLong(link->lastLowPriorityTxTime, L2C_LOW_PRIORITY_MAX_ACCEPT_TIMEOUT))
                    {
                        parms->morePacketWaiting = 1;
                        return 0;
                    }
                }
                if (L2C(lowPriorityInBB) >= 0x01)
                {
                    if (!BTCheckPktServicedTimeLong(link->lastLowPriorityTxTime, L2C_LOW_PRIORITY_MAX_ACCEPT_TIMEOUT))
                    {
                        parms->morePacketWaiting = 1;
                        return 0;
                    }
                }
                if (L2C(lowPriorityInBB) >= 0x03)
                {
                    parms->morePacketWaiting = 1;
                    return 0;
                }
            }
            queue = &(link->txLowPriorityQueue);
            link->lastLowPriorityTxTime = OS_GetSystemTime();
            if (L2C(lowPriorityInBB) != 0xFF)
            {
                L2C(lowPriorityInBB)++;
            }

        }
        else
        {
            queue = &(link->txHighPriorityQueue);
            link->lastHighPriorityTxTime = OS_GetSystemTime();
        }
    }
    else if (HciHndl == PICONET_BC_HANDLE)
    {
        queue = &(L2C(broadcastLink).txLowPriorityQueue);
        L2C(broadcastLink).lastLowPriorityTxTime = OS_GetSystemTime();
    }
    else
    {
        /*
         * This should never happen, so if were running debug, Assert.
         * In release code we'll just ignore the request, no harm done.
         */
        Assert(0);
    }
    if(link !=0)
        link->lastTxRxTime = OS_GetSystemTime();
    for (packet = (BtPacket*) GetHeadList(queue); packet != (BtPacket*) queue;
         packet = (BtPacket*) GetNextNode(&packet->node))
    {
    	        if((packet->llpContext == 0x0004) ||
                   (packet->llpContext == 0x0005) ||
                   (packet->llpContext == 0x0006))
                {
                    if(controllerType !=0)
                    {
                        /* The channel data is for AMP data */
                        parms->morePacketWaiting = 1;
                        continue;
                    }                
                    txPacket = packet;
                    RemoveEntryList(&packet->node);
                    break;
                }


        if ((packet->llpContext >= BASE_DYNAMIC_CID) && (packet->llpContext <= LAST_DYNAMIC_CID))
        {
            /* It's an L2CAP signalling/control packet or BT1.1 channel */
            txPacket = (BtPacket*) packet;
            RemoveEntryList(&packet->node);
            break;
        }
        else
        {
            Assert(((packet->llpContext >= BASE_GROUP_CID) && (packet->llpContext <= LAST_GROUP_CID)) ||
                   (packet->llpContext == 0x0001));
            /* It's a connectionless packet or a signalling reject-response */
            txPacket = packet;
            RemoveEntryList(&packet->node);
            break;
        }
    }

    if (txPacket)
    {
        /* Append the B-Frame header and send the packet. */
        offset = BT_PACKET_HEADER_LEN - txPacket->headerLen - 4;
        packetLen = txPacket->dataLen + txPacket->headerLen + txPacket->tailLen;

        if (txPacket->flags & BTP_FLAG_FCS)
        {
            packetLen += 2;
        }

        /* Add the Packet Length and Remote CID fields. */
        txPacket->header[offset + 0] = (U8) (packetLen);
        txPacket->header[offset + 1] = (U8) (packetLen >> 8);
        txPacket->header[offset + 2] = (U8) (txPacket->remoteCid);
        txPacket->header[offset + 3] = (U8) (txPacket->remoteCid >> 8);
        txPacket->headerLen += 4;
    }
    /* Pass the packet to the HCI */
    return txPacket;
}
#endif

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
BtPacket *L2CAP_HciTxReady(HciCallbackParms *parms, U8 controllerType)
{
    BtRemoteDevice *link;
    BtPacket *packet;
    BtPacket *txPacket = 0;
    L2CAP_Channel *channel = 0;
    ListEntry *queue = 0;
    ListEntry *retransmissionQueue = 0;
    I16 packetLen;
    I8 offset;
    HciHandle HciHndl;
    U8 findTxPacket = 0;
    U16 txSendCount =0;
    U16 SrejReqSeq = 0;
    U8 sFrameflag = 0;
    U8 lowPriorityQueuePlusFlag = 0;
    BOOL l_expired = FALSE;
    
    HciHndl = parms->hciHandle;
    parms->morePacketWaiting = 0;

    do
    {
        if (0 != (link = MeMapHciToRemoteDevice((HciHandle) (HciHndl & 0x0FFF))))
        {

            /* Try to resent the Srej frame fist */
            if (!IsListEmpty(&link->txNonAckQueue))
            {
#if __BT_30_DEBUG__ == XA_ENABLED	            
            	Report(("L2cap resent the Srej frame"));
#endif

                /* Verify the txPacketList is empty or not. */
                /* Check any TxPacket needs to retransmitted. */
                retransmissionQueue = &link->txNonAckQueue;
                for (packet= (BtPacket*) GetHeadList(retransmissionQueue); packet != (BtPacket*) retransmissionQueue;
                     packet = (BtPacket*) GetNextNode(&packet->node))
                {
                    if ((packet->llpContext >= BASE_DYNAMIC_CID) && (packet->llpContext <= LAST_DYNAMIC_CID))
                    {
                        /* It's a connection oriented packet */
                        channel = &L2C(channels)[CID_TO_INDEX(packet->llpContext)];
                        if(channel->enhancedSrejListTxCount == 0)
                        {
                            findTxPacket = 0;
                            break;
                        }
#ifdef __BT_3_0_HS__                        
                        if(controllerType == 0x00)
                        {
                            /* Search for normal data */
                            if(channel->currentAmpControllerId !=0)
                            {
                                /* The channel data is for AMP data */
                                findTxPacket = 0;
                                break;
                            }
                        }                        
                        else
                        {
                            /* Search for AMP data */
                            if(channel->currentAmpControllerId ==0)
                            {
                                /* The channel data is for normal data */
                                findTxPacket = 0;
                                break;
                            }
                        }
#endif
                        if ((channel->flagReportReturnPacketToUpper == 0))
                        {
                            /* Only there is waiting update I frame to upper layer, we will not do the retransmission at that time. */
                            /* L2CHF2_NO_RETRANS_OUT -> No retransmission disable bit enabled */
                            if(IsListEmpty(&link->txPacketList))
                            {
//                                Report(("The retransmission txPacketList is empty"));
                                txPacket = 0;
                                goto L2CAPHCIReceiveReadysendPacket;
                            }
                            txPacket = (BtPacket*) RemoveHeadList(&link->txPacketList);  
                            if (L2Cap_GetNextSegmentBySrej(txPacket, packet, &SrejReqSeq) == TRUE)
                            {
                                findTxPacket = 1;
                                txPacket->priority_type = BTP_FLAG_NONE_PRIPORTY;
                                /* TxPacket contains the segment to send. */
                                L2Cap_BuildIFrameHeaderSrej(txPacket, SrejReqSeq);
                                txPacket->flags |= BTP_FLAG_RDEV_RETX;
                                channel->txIFrameRetransmissionInBasebandCount++;
                                L2Cap_StartRetransmitTimer(channel);
                                link->lastTxRxTime = OS_GetSystemTime();
                                break;								
                            }
                            else
                            {
                                /* No more segments in this packet to send. */
                                InsertTailList(&link->txPacketList, &txPacket->node);
                                txPacket = 0;
                            }
                        }
                    }
                }
                if(findTxPacket ==1)
                {
                    goto L2CAPHCIReceiveReadysendPacket;
                }
            }   
            
            /* select packet from txNonAckQueue */
            if (!IsListEmpty(&link->txNonAckQueue))
            {
                /* Verify the txPacketList is empty or not. */
                /* Check any TxPacket needs to retransmitted. */
                retransmissionQueue = &link->txNonAckQueue;
                for (packet= (BtPacket*) GetHeadList(retransmissionQueue); packet != (BtPacket*) retransmissionQueue;
                     packet = (BtPacket*) GetNextNode(&packet->node))
                {
                    if ((packet->llpContext >= BASE_DYNAMIC_CID) && (packet->llpContext <= LAST_DYNAMIC_CID))
                    {
                        /* It's a connection oriented packet */
                        channel = &L2C(channels)[CID_TO_INDEX(packet->llpContext)];
#ifdef __BT_3_0_HS__                        
                        if(controllerType == 0x00)
                        {
                            /* Search for normal data */
                            if(channel->currentAmpControllerId !=0)
                            {
                                /* The channel data is for AMP data */
                                parms->morePacketWaiting = 1;
                                continue;
                            }
                        }                        
                        else
                        {
                            /* Search for AMP data */
                            if(channel->currentAmpControllerId ==0)
                            {
                                /* The channel data is for normal data */
                                parms->morePacketWaiting = 1;
                                continue;
                            }
                        }
#endif                        
#if 0
                        Report(("L2cap:%02x,%02x,%02x,%02x,",channel->verifyRetransmissionQueueState,
                                    channel->state,
                                    channel->flags2,
                                    channel->flagReportReturnPacketToUpper));
#endif
                        if ((L2Cap_IsRetransTxAvailable(channel) == TRUE) &&
                            (channel->verifyRetransmissionQueueState == 1) &&
                            (channel->state == L2C_OPEN) &&
                            ((channel->flags2 & L2CHF2_NO_RETRANS_OUT) == 0) &&
                            (channel->flagReportReturnPacketToUpper == 0))
                        {
                            /* Only there is waiting update I frame to upper layer, we will not do the retransmission at that time. */
                            /* L2CHF2_NO_RETRANS_OUT -> No retransmission disable bit enabled */
                            if(IsListEmpty(&link->txPacketList))
                            {
                                txPacket = 0;
                                parms->morePacketWaiting = 1;                                
                                goto L2CAPHCIReceiveReadysendPacket;
                            }
                            txPacket = (BtPacket*) RemoveHeadList(&link->txPacketList);  
                            if (L2Cap_GetNextSegment(txPacket, packet, 0) == TRUE)
                            {
                                findTxPacket = 1;
                                txPacket->priority_type = BTP_FLAG_NONE_PRIPORTY;
                                /* TxPacket contains the segment to send. */
                                L2Cap_BuildIFrameHeader(txPacket, L2IFRAMERETRANSMITTED);
                                txPacket->flags |= BTP_FLAG_RDEV_RETX;
                                //Report(("Dlight: BB send retrans out"));                            
                                channel->txIFrameRetransmissionInBasebandCount++;
                                L2Cap_StartRetransmitTimer(channel);
                                link->lastTxRxTime = OS_GetSystemTime();
                                break;
                            }
                            else
                            {
                                /* No more segments in this packet to send. */
                                InsertTailList(&link->txPacketList, &txPacket->node);
                                txPacket = 0;
                            }
                        }
                        else{
                            break;
                        }
                    }
                }
                if(findTxPacket ==1)
                {
                    goto L2CAPHCIReceiveReadysendPacket;
                }
            }    

            if(controllerType !=0)
            { 
                if (IsListEmpty(&(link->txHighPriorityQueue)))
                {
                    if (IsListEmpty(&(link->txLowPriorityQueue)))
                    { 
                        return 0;
                    }                
                    queue = &(link->txLowPriorityQueue);

                    //link->lastLowPriorityTxTime = OS_GetSystemTime(); //don't update the last time
                    if (L2C(lowPriorityInBB) != 0xFF)
                    {
                        lowPriorityQueuePlusFlag = 1;
                        L2C(lowPriorityInBB)++;
                    }
                }
                else
                {
                    queue = &(link->txHighPriorityQueue);
                    // Report(("Dlight high priority queue"));
                    for (packet = (BtPacket*) GetHeadList(queue); packet != (BtPacket*) queue;
                         packet = (BtPacket*) GetNextNode(&packet->node))
                    {
                    
                        if ((packet->llpContext >= BASE_DYNAMIC_CID) && (packet->llpContext <= LAST_DYNAMIC_CID))
                        {
                    
                            /* It's a connection oriented packet */
                            channel = &L2C(channels)[CID_TO_INDEX(packet->llpContext)];
                            if(channel->hciAMPHandler ==0)
                            {
                                if (IsListEmpty(&(link->txLowPriorityQueue)))
                                { 
                                    parms->morePacketWaiting = 1;
                                    return 0;
                                }                
                                queue = &(link->txLowPriorityQueue);
                                //link->lastLowPriorityTxTime = OS_GetSystemTime();
                                if (L2C(lowPriorityInBB) != 0xFF)
                                {
                                    lowPriorityQueuePlusFlag = 1;
                                    L2C(lowPriorityInBB)++;
                                }
                            }
                            else
                            {
                                link->lastHighPriorityTxTime = OS_GetSystemTime();
                            }
                            break;
                        }
                    }
                         
                }
            }
            else
            {
                if (IsListEmpty(&(link->txHighPriorityQueue)))
                {
                    /* Always guarantee high priority packet to send */
                    if (IsListEmpty(&(link->txLowPriorityQueue)))
                    { 
                        return 0;
                    }
               
                    /* no high priority traffic waiting to send */
                    if ((L2C(heavyTrafficSource) == L2C_HEAVY_TRAFFIC_EXIST))
                    {
                        /* A2DP existing, reserved the Bandwidth, only allow low priority to send 1 packet */
                        l_expired = BTCheckPktServicedTimeLong(link->lastLowPriorityTxTime, (L2C_LOW_PRIORITY_MAX_ACCEPT_TIMEOUT/2) ); // ALPS00315560: 3s to 1.5s
                        /* For BR headset, 1 reservation is not enough. Reserve 2 slots. */
                        if(HCC(aclTxPacketsLeft) <= 2){
                            if (!l_expired)
                            {
#ifdef __BT_SCHEDULE_DEBUG__
                               OS_Report("[SH] 1. %u, %u", L2C(lowPriorityInBB), L2C(highPriorityNo));
#endif
                                parms->morePacketWaiting = 1;
                                return 0; // don't select low queue
                            }
                        }
                        
                        if (L2C(highPriorityNo) > 2)  // l2cap_avsendData ++ this
                        {
                            if (!l_expired)
                            {
                                parms->morePacketWaiting = 1;
                                return 0; // don't select low queue
                            }else{
#ifdef __BT_SCHEDULE_DEBUG__                            
                                OS_Report("l: select low");
#endif                                
                            }
                        }
#if 0                    
                        if (L2C(lowPriorityInBB) >= 0x05)
                        {
                            /* RR(PF=0) is 4. When selecting lowqueue, it is 5. */
                            if (!BTCheckPktServicedTimeLong(link->lastLowPriorityTxTime, L2C_LOW_PRIORITY_MAX_ACCEPT_TIMEOUT))
                            {
                                parms->morePacketWaiting = 1;

                                return 0;
                            }
                        }
                        if (L2C(lowPriorityInBB) >= 0x05)
                        {
                            parms->morePacketWaiting = 1;
                            return 0;
                        }
#endif                        
                    }
                    queue = &(link->txLowPriorityQueue);
                    //link->lastLowPriorityTxTime = OS_GetSystemTime(); //move to selected packet part
                    if (L2C(lowPriorityInBB) != 0xFF)
                    {
                        lowPriorityQueuePlusFlag = 1;
                        L2C(lowPriorityInBB)++;
                    }

                }
                else
                {
                    queue = &(link->txHighPriorityQueue);
                    link->lastHighPriorityTxTime = OS_GetSystemTime();
                }
            }

        }
        else if (HciHndl == PICONET_BC_HANDLE)
        {
            queue = &(L2C(broadcastLink).txLowPriorityQueue);
            L2C(broadcastLink).lastLowPriorityTxTime = OS_GetSystemTime();
        }
        else
        {
            /*
             * This should never happen, so if were running debug, Assert.
             * In release code we'll just ignore the request, no harm done.
             */
            Assert(0);
        }

        /* Loop the selected queue */
        link->lastTxRxTime = OS_GetSystemTime();
        for (packet = (BtPacket*) GetHeadList(queue); packet != (BtPacket*) queue;
             packet = (BtPacket*) GetNextNode(&packet->node))
        {

            if ((packet->llpContext >= BASE_DYNAMIC_CID) && (packet->llpContext <= LAST_DYNAMIC_CID))
            {

                /* It's a connection oriented packet */
                channel = &L2C(channels)[CID_TO_INDEX(packet->llpContext)];
#ifdef __BT_3_0_HS__                        
                if((controllerType != 0x00) && (packet->remoteCid == 0x0001))
                {
                    /* Signalling data on Cid 0x00, controller Id is AMP, the packet shall not be found*/
                    parms->morePacketWaiting = 1;
                    continue;
                }
#endif
                if ((channel->outLinkMode != LINK_MODE_BASIC) && (packet->remoteCid != 0x0001))
                {
#ifdef __BT_3_0_HS__                   
                    /* Skip the packet if it is not the sampe controllerType */
                    if(controllerType == 0x00)
                    {
                        /* Search for normal data */
                        if(channel->currentAmpControllerId !=0)
                        {
                            /* The channel data is for AMP data */
                            parms->morePacketWaiting = 1;
                            continue;
                        }
                    }                        
                    else
                    {
                        /* Search for AMP data */
                        if(channel->currentAmpControllerId ==0)
                        {
                            /* The channel data is for normal data */
                            parms->morePacketWaiting = 1;
                            continue;
                        }
                    }

#endif
                    /* if it is S-frame, send it */
                    if (packet->flags & BTP_FLAG_LSYSTEM)
                    {
//                        Assert((packet->dataLen == 0) && (packet->headerLen == 0));
                        /*
                         * It's a BT 1.2 S-Frame. If an RR or REJ still needs
                         * to be sent, L2Cap_BuildSFrame() will build it.
                         */
                        txPacket = packet;
                        packet = (BtPacket*) packet->node.Blink;
                        RemoveEntryList(&txPacket->node);
                        if(channel->flags2 & L2CHF2_SEND_SREJ)
                        {
                            if (0 != (txPacket = L2Cap_BuildSFrameSREJ(channel, txPacket)))
                            {
                                /* txPacket = 0, ReqSeq sent in I frame*/
                                /* 0 != 1 will break. Really want to sent S*/
                                /* 0 != 0 will do continue, ReqSeq sent in S frame */
                                Report(("l2data: Send S SREJ frame"));
                                sFrameflag = 1;
                                goto L2CAPHCIReceiveReadysendPacket; //txPacket is available
                            }
                            continue;

                        }
                        else
                        {
                            if (0 != (txPacket = L2Cap_BuildSFrame(channel, txPacket)))
                            {
                                /* txPacket = 0, ReqSeq sent in I frame*/
                                /* 0 != 1 will break. Really want to sent S*/
                                /* 0 != 0 will do continue, ReqSeq sent in S frame */
#if __BT_30_DEBUG__ == XA_ENABLED								
                                Report(("l2data: L2Cap Send S frame (%d) lcid:%d pkt:0x%x", controllerType, channel->localCid, txPacket));
#endif
                                sFrameflag = 1;
                                goto L2CAPHCIReceiveReadysendPacket; //txPacket is available
                            }
                            parms->morePacketWaiting = 1;
                            continue;
                    	}
                    }

                    /* It's an BT1.2 connection oriented data packet. */




                    if (L2Cap_IsTxWindowAvailable(channel) && (channel->state != L2C_CLOSED) &&
                        ((channel->flags2 & L2CHF2_NO_RETRANS_OUT) == 0))
                        /* L2CHF2_NO_RETRANS_OUT -> No retransmission disable bit enabled */
                    {
                        /* We can transmit another segment, grab one. */
                        if (!IsListEmpty(&link->txPacketList))
                        {
                            txPacket = (BtPacket*) RemoveHeadList(&link->txPacketList);
                            if (L2Cap_GetNextSegment(txPacket, packet, 1) == TRUE)
                            {
#if __BT_30_DEBUG__ == XA_ENABLED
                                Report(("Send I frame (%d)", controllerType));
#endif
                                txPacket->priority_type = BTP_FLAG_NONE_PRIPORTY;
                                /* TxPacket contains the segment to send. */
                                L2Cap_BuildIFrameHeader(txPacket,L2IFRAMEINITCREATE);
                                txPacket->flags |= BTP_FLAG_RDEV_FISRT;
                                /* */
                                if(channel->inLinkMode != LINK_MODE_STREAMMING)
                                {
                                    /* Streaming mode is not required to open retransmission timer*/
                                    L2Cap_StartRetransmitTimer(channel);
                                }
                                RemoveEntryList(&packet->node);                            
                                InsertTailList(&link->txNonAckQueue, &packet->node);
                                if ((channel->outLinkMode != LINK_MODE_FLOW_CONTROL) &&
                                    (channel->outLinkMode != LINK_MODE_STREAMMING))
                                    channel->verifyRetransmissionQueueState = 1;
                                goto L2CAPHCIReceiveReadysendPacket; //txPacket is available
                            }
                            //Report(("L2Cap_GetNextSegment false"));
                            /* No more segments in this packet to send. */
                            InsertTailList(&link->txPacketList, &txPacket->node);
                            txPacket = 0;
                        }
                    }

                    /* not-basic channel */
                    continue;
                }
                
#ifdef __BT_3_0_HS__                        
                if(controllerType == 0x00)
                {
                    /* Search for normal data */
                    if((channel->currentAmpControllerId !=0) && (packet->remoteCid > 0x0003))
                    {
                        /* The channel data is for AMP data */
                        parms->morePacketWaiting = 1;
                        continue;
                    }
                }                        
                else
                {
                    /* Search for AMP data */
                    if(channel->currentAmpControllerId ==0)
                    {
                        /* The channel data is for normal data */
                        parms->morePacketWaiting = 1;
                        continue;
                    }
                }
#endif                
                /* It's an L2CAP signalling/control packet or BT1.1 channel */
                txPacket = (BtPacket*) packet;
                RemoveEntryList(&packet->node);
//                Report(("Send normal frame"));
                break;
            }
            else
            {
    	        if((packet->llpContext == 0x0004) ||
                   (packet->llpContext == 0x0005) ||
                   (packet->llpContext == 0x0006))
                {
                    if(controllerType !=0)
                    {
                        /* The channel data is for AMP data */
                        parms->morePacketWaiting = 1;
                        continue;
                    }                
                    txPacket = packet;
                    RemoveEntryList(&packet->node);
                    break;
                }

                if(!(((packet->llpContext >= BASE_GROUP_CID) && (packet->llpContext <= LAST_GROUP_CID)) ||
                (packet->llpContext == 0x0001)))
                {
                    Assert(0);
                }
#ifdef __BT_3_0_HS__                        
                if(controllerType != 0x00)
                {
                    /* The channel data is for normal data */
                    parms->morePacketWaiting = 1;
                    continue;
                }
#endif                

                /* It's a connectionless packet or a signalling reject-response */
                txPacket = packet;
                RemoveEntryList(&packet->node);
                break;
            }
        }
        txSendCount++;
        if((txPacket ==0) && (lowPriorityQueuePlusFlag == 1))
        {
            L2C(lowPriorityInBB)--;
            lowPriorityQueuePlusFlag = 0;
        }
    }while((txPacket ==0) && (txSendCount <3));
    /* It is possible that HciTxReady check one queue is no packet to send, we have to check another queue. */
L2CAPHCIReceiveReadysendPacket:
    if (txPacket)
    {
        /* Append the B-Frame header and send the packet. */
        offset = BT_PACKET_HEADER_LEN - txPacket->headerLen - 4;
        packetLen = txPacket->dataLen + txPacket->headerLen + txPacket->tailLen;

        if (txPacket->flags & BTP_FLAG_FCS)
        {
            packetLen += 2;
        }

#ifdef __BT_3_0_HS__                    
        if((txPacket->llpContext >= 0x40) && 
            (sFrameflag == 0) &&
            ((packet->flags & BTP_FLAG_LSYSTEM)==0))
        {
            if((channel !=0) && (channel->remoteCid != 0x03))
                link->lastTxRxOverDataChannel = OS_GetSystemTime();            
        }

#endif            

        /* Add the Packet Length and Remote CID fields. */
        txPacket->header[offset + 0] = (U8) (packetLen);
        txPacket->header[offset + 1] = (U8) (packetLen >> 8);
        txPacket->header[offset + 2] = (U8) (txPacket->remoteCid);
        txPacket->header[offset + 3] = (U8) (txPacket->remoteCid >> 8);
#ifdef __BT_3_0_HS__                   
        if(channel !=0)     
        {
#if __BT_30_DEBUG__ == XA_ENABLED
//            Report(("channel->hciAMPHandler:%d",channel->hciAMPHandler));
#endif
            parms->ampHandle = channel->hciAMPHandler;
        }
        else
            parms->ampHandle = 0;
#endif
        txPacket->headerLen += 4;

        if ( (NULL != channel) && (NULL != txPacket) && (sFrameflag !=1))
        {
            if((channel->outLinkMode != LINK_MODE_BASIC) && 
                (txPacket->remoteCid >= 0x0040))
            {
#if __BT_30_DEBUG__ == XA_ENABLED
                 Report(("l2data: L2CAP Build a I frame (%d) lcid:%d TxSeq:%d... TxCompose:%d", controllerType, channel->localCid, txPacket->segStart, channel->txComposeSeq));
#endif
            }
            if((channel->outLinkMode != LINK_MODE_BASIC) && 
                (txPacket->remoteCid >= 0x0040) &&
                (channel->txCountNormalSent == 0))
            {
                // L2Cap_ForceToGetAnAck(channel); // suggest to remove this
            }
        }
        // Update the TxTime before return packet
        if( BTP_FLAG_LOW_PRIPORTY == txPacket->priority_type ){
            link->lastLowPriorityTxTime = OS_GetSystemTime();
        }

    }
    else
    {
        if(lowPriorityQueuePlusFlag == 1)
        {
            L2C(lowPriorityInBB)--;
        }
    
    }
    /* Pass the packet to the HCI */
    return txPacket;
}
#endif

#ifdef __BT_4_0_BLE__
void L2CAP_LinkConnectedByCid(BtRemoteDevice *Link, U16 cid)
{
    U8 i =0;
    if(cid < 4)
        Assert(0);
    if((L2C(fixCid)[cid-4].callback !=0) &&
        (Link->fixCidState[cid-4] == 0))
    {
        Report(("L2CAP_LinkConnected: %d", cid));
        Link->fixCidState[cid-4] = 1;
        L2C(callbackParms).event = L2EVENT_CONNECTED;
        L2C(callbackParms).status = BT_STATUS_SUCCESS;
        L2C(callbackParms).aclLink = Link;
        L2C(fixCid)[cid-4].callback(cid, &L2C(callbackParms));
        L2C(callbackParms).event = 0;
    }
}


#endif

/*****************************************************************************
 * FUNCTION
 *  L2CAP_LinkDisconnect
 * DESCRIPTION
 *  Called by Management entity when a link goes down.
 * PARAMETERS
 *  Link        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2CAP_LinkDisconnect(BtRemoteDevice *Link)
{
    BtPacket *packet;
    I16 i;

    /* First we need to flush the transmit queue for this link */
    while (!IsListEmpty(&(Link->txLowPriorityQueue)))
    {
        packet = (BtPacket*) RemoveHeadList(&(Link->txLowPriorityQueue));

        L2CAP_HciTxDone(packet, BT_STATUS_NO_CONNECTION, Link->hciHandle);
    }
    while (!IsListEmpty(&(Link->txHighPriorityQueue)))
    {
        packet = (BtPacket*) RemoveHeadList(&(Link->txHighPriorityQueue));

        L2CAP_HciTxDone(packet, BT_STATUS_NO_CONNECTION, Link->hciHandle);
    }
    /*
     * Now, Walk list of channels to find which channels are using
     * * this link. Then indicate the event to the state machine.
     */
    for (i = 0; i < L2CAP_NUM_CHANNELS; i++)
    {

        if (L2C(channels)[i].link == Link)
        {
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
            L2Cap_ReturnSentPackets(&L2C(channels)[i], BT_STATUS_NO_CONNECTION);
#endif
            L2Cap_StateMachine(&L2C(channels)[i], LLC_DISCONNECT_IND);
        }
    }

#ifdef __BT_4_0_BLE__
    if (Link != &L2C(broadcastLink))
    {
        for (i=0;i< L2CAP_FIX_CID_NO; i++)
        {
            if(L2C(fixCid)[i].callback !=0)
            {
            OS_Report("L2C(fixCid)[%d].callback",i);
                L2C(callbackParms).event = L2EVENT_DISCONNECTED;
                L2C(callbackParms).status = BT_STATUS_SUCCESS;
                L2C(callbackParms).aclLink = Link;
                L2C(fixCid)[i].callback(i+0x0004, &L2C(callbackParms));
                L2C(callbackParms).event = 0;
            }
        }
    }
#endif
    Link->rxBufferLen = 0;
    Link->rxState = BRXS_COMPLETE;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessSignal
 * DESCRIPTION
 *  Called when a complete L2CAP signalling packet has been
 *  received.
 * PARAMETERS
 *  RxData          [IN]        
 *  RxDataLen       [IN]        
 *  Link            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2Cap_ProcessSignal(U8 *RxData, U16 RxDataLen, BtRemoteDevice *Link)
{
    U16 cmdLen;

    if (L2Cap_ValidateCommand(RxData, RxDataLen) == FALSE)
    {
        Report(("RX error 4"));

        L2Cap_HandleRxError(Link);
        return;
    }

    while (RxDataLen)
    {
        cmdLen = LEtoHost16(RxData + 2);
        /* This condition was checked by ValidateCommand */
        //Assert(RxDataLen >= (cmdLen + SIGNAL_HEADER_LEN));
        if( RxDataLen < (cmdLen + SIGNAL_HEADER_LEN) ){
            Report(("MTU violation 5 RxDataLen:%d cmdLen:%d", RxDataLen, cmdLen));
            L2Cap_HandleRxError(Link);
            return;
        }

        /* Pass each signalling command up individually */
        if ((RxData[0] & 0x01) && (RxData[0] <= 0x11))
        {
            Report(("L2Cap_ProcessRsp"));
            L2Cap_ProcessRsp(RxData, cmdLen);
        }
        else
        {
            Report(("L2Cap_ProcessReq"));
            L2Cap_ProcessReq(RxData, cmdLen, Link);
        }

        RxDataLen -= SIGNAL_HEADER_LEN + cmdLen;
        RxData += SIGNAL_HEADER_LEN + cmdLen;
    }
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessChannelData
 * DESCRIPTION
 *  A complete L2CAP Data Packet has been received. Indicate it
 *  to the application or layered protocol.
 * PARAMETERS
 *  RxData          [IN]        
 *  RxDataLen       [IN]        
 *  Channel         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2Cap_ProcessChannelData(U8 *RxData, U16 RxDataLen, L2CAP_Channel *Channel)
{
    Assert(Channel && ((Channel->state == L2C_OPEN) || (Channel->state == L2C_CONFIG)));

    /* Verify protocols specific MTU */
#ifdef __BT_2_1_ERTM__
    if((Channel->inLinkMode != LINK_MODE_BASIC)) 
    {
        if (RxDataLen > Channel->psmInfo->localMtu+12)
        {
            Report(("A Rx Data len:%d, local MTU:%d",RxDataLen, Channel->psmInfo->localMtu));
            L2Cap_HandleRxError(Channel->link);
            return;
        }
    }
    else
    {
        if (RxDataLen > Channel->psmInfo->localMtu)
        {
            Report(("B Rx Data len:%d, local MTU:%d",RxDataLen, Channel->psmInfo->localMtu));
            L2Cap_HandleRxError(Channel->link);
            return;
        }    
    }
#else
    if (RxDataLen > Channel->psmInfo->localMtu)
    {
        Report(("B Rx Data len:%d, local MTU:%d",RxDataLen, Channel->psmInfo->localMtu));
        L2Cap_HandleRxError(Channel->link);
        return;
    }    
#endif

    Channel->remoteIdent = 0;
    L2C(callbackParms).aclLink = Channel->link;
    L2C(callbackParms).ptrs.data = RxData;
    L2C(callbackParms).dataLen = RxDataLen;
    L2C(callbackParms).owner.psm = Channel->psmInfo;

    L2Cap_NotifyPsm(Channel, L2EVENT_DATA_IND, BT_STATUS_SUCCESS);
}

#if L2CAP_NUM_GROUPS > 0


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessGroupData
 * DESCRIPTION
 *  Complete L2CAP Connectionless Data Packet has been received,
 *  indicate it to the upper layer group PSM.
 * PARAMETERS
 *  RxData          [IN]        
 *  RxDataLen       [IN]        
 *  Link            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2Cap_ProcessGroupData(U8 *RxData, U16 RxDataLen, BtRemoteDevice *Link)
{
    L2capGroup *group;
    U16 psm = LEtoHost16(RxData);
    I16 i;

    /* UNUSED_PARAMETER(Link); */

    /* Find corresponding PSM and deliver data */
    for (i = 0; i < L2CAP_NUM_GROUPS; i++)
    {
        if (L2C(groups)[i] && L2C(groups)[i]->psm == psm)
        {
            bt_trace(TRACE_GROUP_1, L2CAP_PROCESSGROUPDATABT_DEBUGx04X_DATALEN_xD, L2C(groups)[i]->cid, RxDataLen - 2);

            if (0 != (group = L2Cap_GetValidGroup(L2C(groups)[i]->cid)))
            {

                L2C(callbackParms).ptrs.data = (RxData + 2);
                L2C(callbackParms).dataLen = (U16) (RxDataLen - 2);
                L2C(callbackParms).owner.group = group;

                group->callback(L2C(groups)[i]->cid, &L2C(callbackParms));
            }
            break;
        }
    }
}
#endif /* L2CAP_NUM_GROUPS > 0 */ 

#ifdef __BT_4_0_BLE__
/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessBLESignal
 * DESCRIPTION
 *  Called when a complete L2CAP signalling packet has been
 *  received.
 * PARAMETERS
 *  RxData          [IN]        
 *  RxDataLen       [IN]        
 *  Link            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2Cap_ProcessBLESignal(U8 *RxData, U16 RxDataLen, BtRemoteDevice *Link)
{
    U16 cmdLen;
    BtDevType devType = ME_GetDeviceType(&Link->bdAddr);

    while (RxDataLen)
    {
        cmdLen = LEtoHost16(RxData + 2);
        /* This condition was checked by ValidateCommand */
        Assert(RxDataLen >= (cmdLen + SIGNAL_HEADER_LEN));

        /* Pass each signalling command up individually */
        if ((RxData[0] & 0x01) && (RxData[0] <= 0x13))
        {
            if (devType == BT_DEV_TYPE_LE)
            {
                Report(("L2Cap_ProcessBLERsp"));
                L2Cap_ProcessBLERsp(RxData, cmdLen);
            }
            else
            {
                Report(("L2Cap_ProcessRsp"));
                L2Cap_ProcessRsp(RxData, cmdLen);
            }
        }
        else
        {
            if (devType == BT_DEV_TYPE_LE)
            {
                Report(("L2Cap_ProcessBLEReq"));
                L2Cap_ProcessBLEReq(RxData, cmdLen, Link);
            }
            else
            {
                Report(("L2Cap_ProcessReq"));
                L2Cap_ProcessReq(RxData, cmdLen, Link);
            }
        }

        RxDataLen -= SIGNAL_HEADER_LEN + cmdLen;
        RxData += SIGNAL_HEADER_LEN + cmdLen;
    }
}

static void L2Cap_ProcessBLEAttData(U8 *RxData, U16 RxDataLen, BtRemoteDevice *Link, U16 cid)
{
    L2capFixCid *fixCid;

    fixCid = &L2C(fixCid[cid-0x0004]);

    L2C(callbackParms).aclLink = Link;

    L2C(callbackParms).ptrs.data = (RxData);
    L2C(callbackParms).dataLen = (U16) (RxDataLen);
    fixCid->callback(cid, &L2C(callbackParms));
}
#endif

#ifdef __BT_2_1_ERTM__    


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ChannelReceive
 * DESCRIPTION
 *  Copies incoming HCI data into channel based receive buffer.
 *  This is generally for BT1.2 channels operating in BT1.1 mode.
 * PARAMETERS
 *  Channel         [IN]        
 *  BuffDesc        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2Cap_ChannelReceive(L2CAP_Channel *Channel, HciBuffer *BuffDesc)
{
    U16 len;

    /* The function is used to handle the Basic mode. */
    len = LEtoHost16(Channel->link->rxBuffer + L2CAP_PRELUDE_SIZE);     /* SDU length */

    if (Channel->sduLength + BuffDesc->len > Channel->localMtu)
    {
        /* Error! MTU violation, drop link, indicate violation to channels */
        Report(("MTU violation 1"));
        L2Cap_HandleRxError(Channel->link);
        return;
    }
    if(Channel->sduLength ==0)
    {
        if (IsListEmpty(Channel->freeSduBuffer))
        {
            Report(("MTU violation 2"));
            L2Cap_HandleRxError(Channel->link);
            return;
        }
        else
        {
            Channel->flowRxBuffer = (BtRxPacket *)RemoveHeadList(Channel->freeSduBuffer);
        }
    }

    /* Copy data into receive buffer */
    OS_MemCopy((U8 *)Channel->flowRxBuffer->data + Channel->sduLength, BuffDesc->buffer, BuffDesc->len);

    Channel->sduLength += BuffDesc->len;
    Channel->flowRxBuffer->dataLen = Channel->sduLength;
    /* If we have received the entire packet, pass it along. */
    if (len == Channel->sduLength)
    {
        L2C(callbackParms).aclLink = Channel->link;
        L2Cap_ProcessChannelData(Channel->flowRxBuffer->data, Channel->sduLength, Channel);
        Channel->link->rxState = BRXS_COMPLETE;
        Channel->sduLength = 0;
        Channel->flowRxBuffer->dataLen = 0;
        InsertTailList(Channel->freeSduBuffer, &(Channel->flowRxBuffer->node));
        Channel->flowRxBuffer = 0;
        
    }
    else if (len < Channel->sduLength)
    {
        /* Lost some fragments, received packet is larger than header claims. */
        Report(("RX error 5"));

        L2Cap_HandleRxError(Channel->link);
        return;
    }
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessIFrame
 * DESCRIPTION
 *  Process the received information frame.
 * PARAMETERS
 *  Channel         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_ProcessIFrame(L2CAP_Channel *Channel, U8 *rxBuffer1)
{
    U16 fcs;
    U8 *rxbuff = rxBuffer1;
    U8 sar;
    U8 controlField;
    U16 control_byte_msb;
    U16 control_byte_lsb;  
    U16 txSeq = 0;
    U16 rxSeq = 0;
    U8 p_control = 0;
    U8 f_control = 0;
    U8 tempSrejList = 0;
    BOOL expectedTxSrej = FALSE;
    BOOL receiveExpectedTxSeqSrej = FALSE;
    U16 temp_u16;
    U8 recvExpectedTxSeq = 0;
    U8 sFrameSend = 0;

    controlField = L2CAP_GetControlFieldType(Channel);

    if(controlField == L2CAP_CONTROL_FIELD_EXTENDED)
    {
        control_byte_msb = (((U16)Channel->tempControlHdr[3] << 8) | Channel->tempControlHdr[2]);
        control_byte_lsb = (((U16)Channel->tempControlHdr[1] << 8) | Channel->tempControlHdr[0]);
    }
    else
    {
        control_byte_lsb = Channel->tempControlHdr[0];
        control_byte_msb = Channel->tempControlHdr[1];
    }

#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("Channel->flowRxBuffer->dataLen:%d",Channel->flowRxBuffer->dataLen));
#endif

//    if((Channel->outFcsUsed != L2FCS_OPTION_NO_FCS) || (Channel->inFcsUsed != L2FCS_OPTION_NO_FCS))
    if(0)
    {
        /* Received entire segment, verify FCS and process control bytes. */
        fcs = L2Cap_ComputeCrc(rxbuff, 4, L2Cap_InitCrc(0));
        if(controlField == L2CAP_CONTROL_FIELD_EXTENDED)
        {        
            fcs = L2Cap_ComputeCrc(Channel->tempControlHdr, 4, fcs);
            sar = control_byte_msb & 0x03;
        }        
        else
        {
            fcs = L2Cap_ComputeCrc(Channel->tempControlHdr, 2, fcs);
            sar = Channel->tempControlHdr[1] >>6;
        }
        if(sar == 0x01)
        {
            fcs = L2Cap_ComputeCrc(Channel->tempSDULengthHdr, 2, fcs);
        }
        fcs = L2Cap_ComputeCrc(Channel->flowRxBuffer->data + Channel->frameStartOffset, 
                Channel->frameOffset, fcs);

#if 0
        if (fcs != LEtoHost16(Channel->tempFCS))
        {
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
        
            /* Invalid FCS: Flush the segment. (Send a REJ?) */
            L2Cap_ChannelRxError(Channel, L2ERR_INVALID_FCS);
            L2Cap_ReturnflowRxBuffer(Channel);
            return;
        }
#endif		
    }
    if (Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
    {
        switch(controlField)
        {
            case L2CAP_CONTROL_FIELD_ENHANCED:
                p_control = 0;
                f_control = (control_byte_lsb >> 7) & 0x01;            
                break;
            case L2CAP_CONTROL_FIELD_EXTENDED:
                p_control = 0;
                f_control = (control_byte_lsb >> 1) & 0x01;             
                break;
            default:
                p_control = 0;
                f_control = 0;
                break;
        }
        L2Cap_ProcessRecvPorFBit(Channel, f_control, p_control);
    }

    if(controlField == L2CAP_CONTROL_FIELD_EXTENDED)
    {
        temp_u16 = control_byte_msb;
    }
    else
    {
        temp_u16 = control_byte_lsb;
    }
    if (L2Cap_ProcessTxSeq(Channel, (U16)temp_u16) == FALSE)
    {
        /* Invalid N(S): No action, just flush the segment. */
        Report(("L2Cap_ProcessTxSeq failed"));
		if (Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)		
	        L2Cap_ChannelRxError(Channel, L2ERR_INVALID_NR);
		else
        L2Cap_ChannelRxError(Channel, L2ERR_INVALID_NS);
        L2Cap_ReturnflowRxBuffer(Channel);        
        return;
    }
    if(controlField == L2CAP_CONTROL_FIELD_EXTENDED)
    {
        temp_u16 = control_byte_lsb;
    }
    else
    {
        temp_u16 = control_byte_msb;
    }

    if (L2Cap_ProcessReqSeq(Channel, (U16)temp_u16, f_control, &sFrameSend) == FALSE)
    {
        /* Invalid N(R): Close the channel & flush the segment. */
        L2Cap_ChannelRxError(Channel, L2ERR_INVALID_NR);
        Report(("L2Cap_ProcessReqSeq failed cid:%d state:%d", Channel->localCid, Channel->state));
        return;
    }
#ifdef __BT_3_0_HS__
    if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
    {
    	if(f_control == 1)
		{
			/* If in AMP, we can resume the channel */
			L2CAP_AMPResumeFrameResponder(Channel);
		}
	}
#endif
    if(controlField != L2CAP_CONTROL_FIELD_EXTENDED)
    {
        txSeq = ((U16)(Channel->tempControlHdr[0]>>1)) & L2Cap_GetSequenceWrapValue(Channel);
        sar = Channel->tempControlHdr[1] >> 6;
    }
    else
    {
        txSeq = control_byte_msb >> 2;
        sar = control_byte_msb & 0x03;
    }
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("Rx Sequence:0x%02x",txSeq));

	Report(("l2data: I frame cid:%d recvSeq:0x%02x sar:%d txSeq:%d",Channel->localCid, Channel->recvSeq, sar, txSeq));
#endif

    if ((((Channel->recvSeq - txSeq) & (L2Cap_GetSequenceWrapValue(Channel))) <= Channel->rxWindow) &&
        (txSeq != Channel->recvSeq))
    {
        Report(("Rx duplicate I frame ignore it"));
        L2Cap_ReturnflowRxBuffer(Channel);
        return;
    }
    if (txSeq != Channel->recvSeq)
    {
        /* The frame is out of sequence or a duplicate (N(S) != V(R)), just flush the segment. */
        Report(("L2Cap:The frame is out of sequence"));
        L2Cap_ReturnflowRxBuffer(Channel);
        if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
        {

#if __BT_30_DEBUG__ == XA_ENABLED
            Report(("L2Cap enhanced try to send REJ or SREJ"));
#endif
#if L2CAP_ENHANCED_SELECTED_REJECT == XA_DISABLED
			Report(("Send REJ1"));
            L2Cap_SendREJ(Channel);
#else
			if(Channel->enhancedSrejSent == 0)
			{
				Report(("Try to sent SREJ ReqSeq:%02x",Channel->recvSeq));
				L2Cap_SendSREJ(Channel, Channel->recvSeq);
			}
			else
			{
				if(Channel->enhancedSrejSent == 1)
				{
					/* in SREJ state. */
					if(Channel->enhancedSrejSendRej == 0) 
					{
//						L2Cap_SendSREJ(Channel, Channel->recvSeq);
						Channel->enhancedSrejSendRej = 1;
						/* Channel->enhancedSrejSendRej */
					}
					else
					{
						Report(("SREJ Sent out, just Pass to Tx"));
					}
				}
			}
#endif
        }
        return;
    }

    /* Check for retransmission disable flag changes. */
    if(controlField == L2CAP_CONTROL_FIELD_STANDARD)    
        L2Cap_ProcessRbit(Channel, Channel->tempControlHdr[0]);

    if (VerifyAndUpdateSar(Channel, sar) == FALSE)
    {
        /* Invalid SAR order, close the channel & flush the SDU. */
        L2Cap_ChannelRxError(Channel, L2ERR_INVALID_SAR);
        Report(("L2Cap: Invalid SAR order"));
        return;
    }
    if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
    {
        if(Channel->enhancedSrejList !=0)
        {
            expectedTxSrej = L2Cap_EnhancedVerifyReqSeqInRxMissingRejectTableHead(Channel, txSeq);
        }
        tempSrejList = Channel->enhancedSrejList;
        /* Rx Srej is expected index */
        if((expectedTxSrej == TRUE) &&   
            (Channel->enhancedSrejSent == 1) && 
            (Channel->enhancedSrejList == 1))
        {
            L2Cap_EnhancedRemoveFromRxMissingRejectTable(Channel, txSeq);
            Channel->enhancedSrejSent = 0;
            if(Channel->enhancedSrejSendRej == 1)
            {
				Channel->enhancedSrejSendRej = 0;
				L2Cap_EnhancedRemoveAllFromRxMissingRejectTable(Channel);
                L2Cap_SendREJ(Channel);
            }
        }
        else if(Channel->enhancedSrejSent == 1)
        {
            if(expectedTxSrej == TRUE)
            {
                /* With expected TxSeq but SrejList > 1, PopSrjeList  */
                L2Cap_EnhancedRemoveFromRxMissingRejectTable(Channel, txSeq);
            }
            else if(L2Cap_EnhancedVerifyReqSeqInRxMissingRejectTable(Channel, txSeq) == TRUE)
            {
                /* With Unexpected TxSeq - Srej */
                L2Cap_SendREJ(Channel);
            }
        }
    }
    else
        tempSrejList = 0;

        
    if(txSeq == Channel->recvSeq)
    {
#if __BT_30_DEBUG__ == XA_ENABLED
        Report(("I frame in order"));
#endif
        recvExpectedTxSeq = 1;
        /* The frame is valid and in order, commit it to the SDU. */
        Channel->recvSeq = (U16) ((Channel->recvSeq + 1) & (L2Cap_GetSequenceWrapValue(Channel)));
        Channel->remoteIdent = 0;
        Channel->flowRxBuffer->dataLen += Channel->frameOffset;
        Channel->flowRxBuffer->reqSeq = Channel->recvSeq;
        Channel->enhancedRejSent = 0; /* Received Ack means Rej clear */
        if ((U16) ((Channel->recvSeq - Channel->buffSeq) & (L2Cap_GetSequenceWrapValue(Channel))) == Channel->rxWindowSize)
        {
            /* Force the buffSeq plus one. */
            /* It can avoid that if the Txwindow is too small and 
            the buffer full before upper layer pull the sdu data. */
            Channel->buffSeq++;
        }
        if((sar == 0x00) || (sar == 0x02))
        {
            if(sar == 0x02)
            {
                Report(("RX data SAR == 2"));
            }
            InsertTailList(Channel->rxSduBuffer, &Channel->flowRxBuffer->node);
            L2C(callbackParms).owner.psm = Channel->psmInfo;
            L2C(callbackParms).status = BT_STATUS_SUCCESS;
            L2C(callbackParms).aclLink = Channel->link;   
            L2Cap_ReturnSentPackets(Channel,BT_STATUS_SUCCESS);
            Channel->flagReportReturnPacketToUpper = 0;
//            Report(("Rx data length:%d",Channel->flowRxBuffer->dataLen));
            L2Cap_NotifyPsm(Channel, L2EVENT_FLOW_DATA_IND, BT_STATUS_SUCCESS);
            if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
            {
                if(sFrameSend == 0)
                {
                    L2Cap_EnhancedRetransmissionSendAck(Channel, 0);
                    sFrameSend = 1;
                }
            }
            else
                L2Cap_SendSFrame(Channel);
            Channel->flowRxBuffer = 0;
        }
    }
    else
        Report(("l2data: I frame not in order"));

    /* Send an RR-Acknowledgement for this segment if necessary. */
    if (Channel->state == L2C_OPEN)
    {
        if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
        {
            if(Channel->enhancedSrejSent == 1)
                 /* In SREJ Sent state */
            {
                if((expectedTxSrej == TRUE) && (tempSrejList <= 1))
                {
                    /* If SrejList <=1, send Ack */
                    Report(("L2Cap: Send RR a1 expectedTxSrej"));
                    L2Cap_EnhancedRetransmissionSendAck(Channel, 1);
                }
            }
            else if(Channel->enhancedRejSent == 1) /* In rej sent state */
            {
                if(f_control == 1)
                {
                    if(Channel->enhancedRejActioned == 0) 
                    {
                            Channel->sendSeq = Channel->ackSeq;
                            L2Cap_IssueHciRequestToSend(Channel->link);
                            Channel->verifyRetransmissionQueueState = 1;
                    }
                    else
                    {
                        Channel->enhancedRejActioned = 0;             
                    }
                }
                if(recvExpectedTxSeq == 1)
                {
                    Report(("L2Cap: Send RR a2 recvExpectedTxSeq"));
                    L2Cap_EnhancedRetransmissionSendAck(Channel, 1);
                }
            }
            else
            {
                if(sFrameSend ==0)
                    L2Cap_EnhancedRetransmissionSendAck(Channel, 0);
            }
        }
        else
            L2Cap_SendSFrame(Channel);
    }
    
}

/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessStreamingIFrame
 * DESCRIPTION
 *  Process the received information frame.
 * PARAMETERS
 *  Channel         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_ProcessStreamingIFrame(L2CAP_Channel *Channel, U8 *rxBuffer1)
{
    U16 fcs;
    U8 *rxbuff = rxBuffer1;
    U8 sar;
    U16 txSeq;
    U8 controlFieldType;
    U16 offset;
    U16 wrapValue;
    
    controlFieldType = L2CAP_GetControlFieldType(Channel);
    wrapValue = L2Cap_GetSequenceWrapValue(Channel);
    if(controlFieldType == L2CAP_CONTROL_FIELD_EXTENDED)
        offset = 4;
    else
        offset = 2;
    if((Channel->outFcsUsed != L2FCS_OPTION_NO_FCS) || (Channel->inFcsUsed != L2FCS_OPTION_NO_FCS))
    {
        /* Received entire segment, verify FCS and process control bytes. */
        fcs = L2Cap_ComputeCrc(rxbuff, 4, L2Cap_InitCrc(0));
        fcs = L2Cap_ComputeCrc(Channel->tempControlHdr, offset, fcs);
        
        if(controlFieldType == L2CAP_CONTROL_FIELD_EXTENDED)
        {
            if(((Channel->tempControlHdr[2] & 0x03)) == 0x01)

            {
                fcs = L2Cap_ComputeCrc(Channel->tempSDULengthHdr, 2, fcs);
            }
        }
        else
        {
            if((Channel->tempControlHdr[1] >> 6) == 0x01)
            {
                fcs = L2Cap_ComputeCrc(Channel->tempSDULengthHdr, 2, fcs);
            }
        }
        
        fcs = L2Cap_ComputeCrc(Channel->flowRxBuffer->data + Channel->frameStartOffset, 
                Channel->frameOffset, fcs);
        if (0)
        //if (fcs != LEtoHost16(Channel->tempFCS))
        {
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
            Report(("FCS error"));
        
            /* Invalid FCS: Flush the segment. (Send a REJ?) */
            L2Cap_ChannelRxError(Channel, L2ERR_INVALID_FCS);
            L2Cap_ReturnflowRxBuffer(Channel);
            return;
        }
    }
    if(controlFieldType == L2CAP_CONTROL_FIELD_EXTENDED)
    {
        txSeq = (LEtoHost16(Channel->tempControlHdr + 2) >> 2);

    }
    else
    {
        txSeq = (U16) (((U16)(Channel->tempControlHdr[0] >> 1)) & wrapValue);
    }

    Report(("Channel->recvSeq:0x%02x",Channel->recvSeq));
    Report(("Rx Sequence:0x%02x",txSeq));

    if (txSeq == Channel->recvSeq)
    {
        /* N(S) is in-sequence, process the frame. V(R) is updated later. */
        Report(("DEBUG Rx expected data"));
        Channel->recvSeq = (U16) ((txSeq + 1) & wrapValue);
    }
    else if (((txSeq - Channel->recvSeq) & wrapValue) < Channel->rxWindow)
    {
        /* ns < recvSeq  + rxWindow, received exceeed our expected. It means a packet lost. */
        /* N(S) is valid but it's out-of-sequence. Send a reject if we haven't already. */
        Channel->recvSeq = (U16) ((txSeq + 1) & wrapValue);
    }
    else if (((Channel->recvSeq - txSeq) & wrapValue) <= Channel->rxWindow)
    {
        /*
            * N(S) is valid but it's a duplicate. Allow processing 
            * of the frame's control fields. 
            */
       L2Cap_ReturnflowRxBuffer(Channel);
       return;
    }
    else
    {
        Report(("Process TxSeq unpected error"));
        L2Cap_ChannelRxError(Channel, L2ERR_INVALID_NS);
        L2Cap_ReturnflowRxBuffer(Channel);        
        return;
    }

    /* The frame is valid , commit it to the SDU. */
    Channel->remoteIdent = 0;
    Channel->flowRxBuffer->dataLen += Channel->frameOffset;
    Channel->flowRxBuffer->reqSeq = Channel->recvSeq;
    Report(("Rx data length:%d",Channel->flowRxBuffer->dataLen));
    InsertTailList(Channel->rxSduBuffer, &Channel->flowRxBuffer->node);
    L2C(callbackParms).owner.psm = Channel->psmInfo;
    L2C(callbackParms).status = BT_STATUS_SUCCESS;
    L2C(callbackParms).aclLink = Channel->link;   
    L2Cap_ReturnSentPackets(Channel,BT_STATUS_SUCCESS);
    Channel->flagReportReturnPacketToUpper = 0;
    L2Cap_NotifyPsm(Channel, L2EVENT_FLOW_DATA_IND, BT_STATUS_SUCCESS);
    Channel->flowRxBuffer = 0;
}

//extern U32 g_inpal_count;

/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessSFrame
 * DESCRIPTION
 *  Process the received supervisory frame.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2Cap_ProcessSFrame(L2CAP_Channel *Channel, U8 *rxBuffer1)
{
    U16 fcs;
    U8 *rxbuff = rxBuffer1;
    U16 control_byte_msb;
    U16 control_byte_lsb;  
    U8 controlField;
    U8 s_control = 0;
    U8 p_control = 0;
    U8 f_control = 0;
    U16 nonAckNo;
    U16 wrapValue;
    U8 flagPassToTx = 1;
    U16 sRejReqSeq = 0;
    U16 sRejReqSeqTail = 0;
    U16 temp_u16;
    U8 sFrameSend = 0;
    controlField = L2CAP_GetControlFieldType(Channel);
#if __BT_30_DEBUG__ == XA_ENABLED	
    Report(("Rx a S frame cid:%d", Channel->localCid));
#endif
    BTCoreGetSystemTick5();

    if(controlField == L2CAP_CONTROL_FIELD_EXTENDED)
    {
        control_byte_lsb = (((U16)Channel->tempControlHdr[1] << 8) | Channel->tempControlHdr[0]);
        control_byte_msb = (((U16)Channel->tempControlHdr[3] << 8) | Channel->tempControlHdr[2]);
    }
    else
    {
        control_byte_lsb = Channel->tempControlHdr[0];
        control_byte_msb = Channel->tempControlHdr[1];
    }
    wrapValue = L2Cap_GetSequenceWrapValue(Channel);

    /* Received entire segment, verify FCS and process control bytes. */
    if((Channel->outFcsUsed != L2FCS_OPTION_NO_FCS) || (Channel->inFcsUsed != L2FCS_OPTION_NO_FCS))
    {   
        if(controlField == L2CAP_CONTROL_FIELD_EXTENDED)
        {
            /* Use extended header with fcs */
            fcs = L2Cap_ComputeCrc(rxbuff, 4, L2Cap_InitCrc(0));
            fcs = L2Cap_ComputeCrc(Channel->tempControlHdr, 4, fcs);
        }
        else
        {
            /* Use normal header with fcs */
            fcs = L2Cap_ComputeCrc(rxbuff, 4, L2Cap_InitCrc(0));
            fcs = L2Cap_ComputeCrc(Channel->tempControlHdr, 2, fcs);
        }
#if 0
        /// check FCS
        if (fcs != LEtoHost16(Channel->tempFCS))
        {
            /* Invalid FCS: Flush the segment. (Send a REJ?) */
            L2Cap_ChannelRxError(Channel, L2ERR_INVALID_FCS);
            
            Report(("L2ERR_INVALID_FCS"));
            return;
        }
#endif		
    }        
    if (Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
    {
        switch(controlField)
        {
            case L2CAP_CONTROL_FIELD_ENHANCED:
                f_control = (control_byte_lsb >> 7) & 0x01;
                p_control = (control_byte_lsb >> 4) & 0x01;            
                break;
            case L2CAP_CONTROL_FIELD_EXTENDED:
                p_control = (control_byte_msb >> 2) & 0x01;
                f_control = (control_byte_lsb >> 1) & 0x01;             
                break;
            default:
                p_control = 0;
                f_control = 0;
                break;
                
        }
        L2Cap_ProcessRecvPorFBit(Channel, f_control, p_control);
    }
    /* Process S bit */
    if(controlField != L2CAP_CONTROL_FIELD_EXTENDED)    
    {
        s_control = (control_byte_lsb & 0x0D) >> 2;
        temp_u16 = control_byte_msb;
    }
    else
    {
        s_control = (control_byte_msb & 0x03);
        temp_u16 = control_byte_lsb;
    }
    if ((Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION) &&
        (s_control == 0x03)) /* Select Reject*/
    {
        if(p_control == 0)
            flagPassToTx = 0;
    }
    
    if(flagPassToTx == 1)
    {
        /* Handle the ReqSeq */
        if (L2Cap_ProcessReqSeq(Channel, temp_u16, f_control, &sFrameSend) == FALSE)
        {
            /* Invalid N(R): Close the channel. */
            Report(("call L2Cap_ChannelRxError 1"));
            
            L2Cap_ChannelRxError(Channel, L2ERR_INVALID_NR);
            return;
        }
        if(controlField == L2CAP_CONTROL_FIELD_EXTENDED)
        {
            sRejReqSeq = (U16) (temp_u16 >> 2);
        }
        else
        {
            sRejReqSeq = (U16) (temp_u16 & 0x3f);
        }			
    /* Check for retransmission disable flag changes. */
        if(controlField == L2CAP_CONTROL_FIELD_STANDARD)    
            L2Cap_ProcessRbit(Channel, control_byte_lsb);
    }
    else
    {
        if (L2Cap_ProcessReqSeqNotUpdateValue(Channel, temp_u16, &sRejReqSeq) == FALSE)
        {
            L2Cap_ChannelRxError(Channel, L2ERR_INVALID_NR);
        }
    }

    /*
     * All the RR processing logic is covered by the ProcessNr() and
     * ProcessRbit() functions. So just check if this is a Reject frame.
     */

    if (Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
    {
    	if( s_control != 0x00 ){
	        Report(("l2data: Rx a S frame s_control:%d", s_control));
    	}
        switch (s_control) 
        {
            case 0x00: /* Receiver Ready */
#ifdef __BT_3_0_HS__
				if(f_control == 1)
				{
					L2CAP_AMPResumeFrameResponder(Channel);
				}
				if(p_control == 1)
				{
					L2CAP_AMPResumeFrameInitiator(Channel);
				}				
#endif
                if (Channel->txIframeCount[Channel->ackSeq] >= Channel->transmitMax)
                {
                    /* Follow test spec ERM/BV/12_C */
                    Report(("L2Cap: SFrame:RR txIframecount:%d ackSeq:%d transMax:%d", Channel->txIframeCount[Channel->ackSeq] , Channel->ackSeq, Channel->transmitMax));					
                    L2Cap_SendDiscReqInternal(Channel);
                    break;
                }

                if((p_control == 0) && (f_control == 0))
                {
                    nonAckNo = L2Cap_GetUnAckNumber(Channel, wrapValue);
                    if((Channel->enhancedRetransRemoteBusy == 1) &&
                        (nonAckNo !=0))
                    {
                        L2Cap_StartRetransmitTimer(Channel);
                    }
                    Channel->enhancedRetransRemoteBusy = 0;
                    if(Channel->enhancedRejSent == 1)
                    {
                        L2Cap_EnhancedRetransmissionSendAck(Channel, 1);
                    }
                        
                }
#if __BT_30_DEBUG__ == XA_ENABLED
                Report(("RR f control:%d", f_control));
#endif 
                if(f_control == 1)
                {
                    Channel->enhancedRetransRemoteBusy = 0;
#if __BT_30_DEBUG__ == XA_ENABLED					
                    Report(("Channel->enhancedRejActioned:%d",Channel->enhancedRejActioned));
                    Report(("l2data: Channel->ackSeq:%d lcid:%d",Channel->ackSeq, Channel->localCid));
#endif					
                    if(Channel->enhancedRejActioned == 0)
                    {
                        Channel->sendSeq = Channel->ackSeq;
                        L2Cap_IssueHciRequestToSend(Channel->link);
                        Channel->verifyRetransmissionQueueState = 1;
                    }
                    else
                    {
                        Channel->enhancedRejActioned = 0;
                    }
                }
#if __BT_30_DEBUG__ == XA_ENABLED
                Report(("RR p control:%d", p_control));
#endif
                if(p_control == 1)
                {   
                    if((Channel->enhancedRejSent == 1) ||
                        (Channel->enhancedSrejSent == 1))
                    {
                        /*In REJ_SENT state */
                        nonAckNo = L2Cap_GetUnAckNumber(Channel, wrapValue);
                        if((Channel->enhancedRetransRemoteBusy == 1) &&
                            (nonAckNo !=0))
                        {
                            L2Cap_StartRetransmitTimer(Channel);
                        }
                    }
                    else if(Channel->enhancedRetransRemoteBusy == 1)
                    {
                        Channel->sendSeq = Channel->ackSeq;
                        L2Cap_IssueHciRequestToSend(Channel->link);
                    }
                        
                    Channel->enhancedRetransRemoteBusy = 0;
                    Channel->enhancedNeedToSendFinal = 1;
                    Report(("C channel->enhancedNeedToSendFinal:%d",Channel->enhancedNeedToSendFinal));    

                    if(Channel->enhancedSrejSent == 0)
                    {
                        Report(("Send RR a5"));
                        L2Cap_EnhancedRetransmissionSendAck(Channel, 1);
                    }
                    else
                    {
                        sRejReqSeqTail = L2Cap_EnhancedGetRxMissingRejectTableHead(Channel);
						/* Actually, we invalid the spec because the spec want to send tail. 
						    However, the tail means the List are all ack to the remote side, it may not be correct.
						*/
						if(sRejReqSeqTail != 0xFFFF)
                        {
							/* Spec write the P = 1, send SREJ tail */
							L2Cap_SendSREJ(Channel, sRejReqSeqTail);
						}
                    }
                }
                break;
            case 0x01: /* Receiver Reject */
                Report(("l2data: SFrame:Reject send from:%d already:%d", Channel->ackSeq, Channel->sendSeq));
                /* Reject: Initiate I-frame retransmission. */
                if (Channel->txIframeCount[Channel->ackSeq] >= Channel->transmitMax)
                {
                	Report(("l2cap: SFrame:Reject txIframecount:%d ackSeq:%d transMax:%d", Channel->txIframeCount[Channel->ackSeq] , Channel->ackSeq, Channel->transmitMax));
                    L2Cap_SendDiscReqInternal(Channel);
                    break;
                }
                Channel->enhancedRetransRemoteBusy = 0;
                if(f_control == 0)
                {
                    Channel->sendSeq = Channel->ackSeq;
                    Report(("L2Cap enhanced rx REJ, send from:%d",Channel->ackSeq));
                    L2Cap_IssueHciRequestToSend(Channel->link);
                    Channel->verifyRetransmissionQueueState = 1;
                    if(Channel->enhancedWait_F == 1)
                    {
                        Channel->enhancedRejActioned = 1;
                    }
                }
                else
                {
                    if(Channel->enhancedRejActioned == 0)
                    {
                        Channel->sendSeq = Channel->ackSeq;
                        L2Cap_IssueHciRequestToSend(Channel->link);
                        Channel->verifyRetransmissionQueueState = 1;
                    }
                    else
                    {
                        Channel->enhancedRejActioned = 0;
                    }
                }
                break;
            case 0x02: /* Receiver not Ready */
                Report(("l2data: SFrame:RnR"));				
                Channel->enhancedRetransRemoteBusy = 1;
                L2Cap_StopTimer(Channel);
                if(p_control == 1)
                {
                    Channel->enhancedNeedToSendFinal = 1;
                    Report(("D channel->enhancedNeedToSendFinal:%d",Channel->enhancedNeedToSendFinal));    
                    
                    if(Channel->enhancedSrejSent == 0)
                    {
                        Report(("Send RR a6"));
                        L2Cap_EnhancedRetransmissionSendAck(Channel, 1);          
                    }
                    else
                    {
                        /* in SREJ SENT state, send SREJ with tail */
                        sRejReqSeqTail = L2Cap_EnhancedGetRxMissingRejectTableHead(Channel);
						/* Actually, we invalid the spec because the spec want to send tail. 
						    However, the tail means the List are all ack to the remote side, it may not be correct.
						*/
						if(sRejReqSeqTail != 0xFFFF)
						{
	                        L2Cap_SendSREJ(Channel, sRejReqSeqTail);
						}
                    }
                }
                else /* p = 0*/
                {
                    if((Channel->enhancedRejSent == 1) ||
                        (Channel->enhancedSrejSent == 1))
                    {
                        Report(("Send RR a7"));
                        L2Cap_EnhancedRetransmissionSendAck(Channel, 1);          
                    }
                }
                break;
            case 0x03:  /* Select Reject*/
                Report(("l2data: SFrame:SRej "));				
                if (Channel->txIframeCount[Channel->ackSeq] >= Channel->transmitMax)
                {
                	Report(("L2Cap: SFrame:Srej txIframecount:%d ackSeq:%d transMax:%d", Channel->txIframeCount[Channel->ackSeq] , Channel->ackSeq, Channel->transmitMax));                
                    L2Cap_SendDiscReqInternal(Channel);
                    break;
                }                
                Channel->enhancedRetransRemoteBusy = FALSE;
                if ((p_control ==0))
                {
                    if(f_control == 0)
                    {
                        L2Cap_EnhancedRetransmissionPassToTxFBit(Channel, f_control, sRejReqSeq);
                        /* Retransmitted I fream with sRejReqSeq */
                        L2Cap_EnhancedRetransmissionSrejResentIFrame(Channel, sRejReqSeq);
                        if(Channel->enhancedWait_F == 1)
                        {
                            Channel->enhancedSrejActioned = 1;
                            Channel->enhancedSrejSaveReqSeq = sRejReqSeq;
                        }
                    }
                    else /* f_control == 1*/
                    {
                        L2Cap_EnhancedRetransmissionPassToTxFBit(Channel, f_control, sRejReqSeq);
                        if((Channel->enhancedSrejActioned == 1) &&
                            (Channel->enhancedSrejSaveReqSeq == sRejReqSeq))
                        {
                            /* SrejSaveReqSeq == ReqSeq*/
                            Channel->enhancedSrejActioned = 0;
                        }
                        else
                        {
                            /* Retransmitted I fream with sRejReqSeq */
                            L2Cap_EnhancedRetransmissionSrejResentIFrame(Channel, sRejReqSeq);
                        }
                    }
                }
                else /* p_control == 1*/
                {
                    /* Retransmitted I fream with sRejReqSeq */
                    L2Cap_EnhancedRetransmissionSrejResentIFrame(Channel, sRejReqSeq);
                    if(Channel->enhancedWait_F == 1)
                    {
                        Channel->enhancedSrejActioned = 1;
                        Channel->enhancedSrejSaveReqSeq = sRejReqSeq;                        
                    }
                }

                break;
        }
    }
    else
    {
        switch (s_control) 
        {
            case 0x01: /* Receiver Reject */
                Report(("l2data: SFrame:RR s_control "));								
                /* Reject: Initiate I-frame retransmission. */
                Channel->sendSeq = Channel->ackSeq;
                L2Cap_IssueHciRequestToSend(Channel->link);
                break;
        }
    
    }
    BTCoreGetSystemTick6();

}


/*****************************************************************************
 * FUNCTION
 *  VerifyAndUpdateSar
 * DESCRIPTION
 *  Checks the received packets SAR state against the expected
 *  SAR state. Updates the state if the SAR is acceptable.
 * PARAMETERS
 *  Channel     [IN]        
 *  Sar         [IN]        
 * RETURNS
 *  TRUE  - SAR is valid.
 *  FALSE - SAR is invalid.
 *****************************************************************************/
static BOOL VerifyAndUpdateSar(L2CAP_Channel *Channel, U8 Sar)
{
    switch (Channel->sar)
    {
        case 0x00:  /* Unsegmented */
        case 0x02:  /* End */
            if ((Sar == 0x00) || (Sar == 0x01))
            {
                /* Unsegmented or Start */
                Channel->sar = Sar;
                return TRUE;
            }
            break;

        case 0x01:  /* Start */ 
        case 0x03:  /* Continue */
            if ((Sar == 0x03) || (Sar == 0x02))
            {
                /* Continue or End */
                Channel->sar = Sar;
                return TRUE;
            }
            break;
        default:
            Assert(0);
            break;
    }
    return FALSE;
}
#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */

/*****************************************************************************
 * FUNCTION
 *  L2Cap_HandleRxError
 * DESCRIPTION
 *  Handles a L2CAP packet reassembly error by disconnecting the
 *  faulty link if it is supposed to have an infinite flush.
 * PARAMETERS
 *  Link        [IN]        
 * RETURNS
 *  TRUE  - Link Disconnected
 *  FALSE - Link not disconnected.
 *****************************************************************************/
static BOOL L2Cap_HandleRxError(BtRemoteDevice *Link)
{
    bt_trace(TRACE_GROUP_1, L2CAP_HANDLERXERRORxX_HCIHANDLE_BT_DEBUGx04X, Link, Link->hciHandle);

    Link->rxState = BRXS_DISCONNECTING;

    /*
     * Restricted links have not yet been fully implemented so at this time
     * we will assume that all links have infinite flush timeout. Therefore,
     * we need to disconnect the link. Later we will handle the case where
     * non infinite flush timeout.
     */
    Link->disconnectACLCheckCreatACLAlwaysForce = 1;
    MeDisconnectLink(Link, 18);
    return TRUE;
}

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ChannelRxError
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Error       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_ChannelRxError(L2CAP_Channel *Channel, U8 Error)
{
    /* Always flush the segment from the link reassembly buffer. */
    Channel->link->rxState = BRXS_FLUSH;
    if (Error == L2ERR_INVALID_NR)
    {
        /* Invalid N(R) causes the channel to close. */
        Report(("L2Cap: L2ERR_INVALID_NR"));		
		L2Cap_SendDiscReqInternal(Channel);
		return;
	}
    if ((Error == L2ERR_INVALID_NR) || (Error == L2ERR_INVALID_PDU) 
		|| (Error == L2ERR_INVALID_SAR) || (Error == L2ERR_FORCE_DISCONNECT))
    {
        /* Invalid PDU length consistency check or length exceeds MTU. */
        /* Invalid SAR field causes the channel to close. */
        L2Cap_NotifyUserAndCloseChannel(Channel, L2DISC_PROTOCOL_ERROR);
        return;
    }

    if (Error == L2ERR_INVALID_NS)
    {
        /* On invalid N(S) just to flush the segment, that's all. */
        return;
    }

    if (Error == L2ERR_INVALID_MPU)
    {
        /*
         * Invalid MPU, could be caused by duplicate or out-of-sequence
         * MPU's so just flush the segment.
         */
        return;
    }

    if (Error == L2ERR_INVALID_FCS)
    {
        /* On invalid FCS, just flush the segment. */
        /* Could optionally send a REJ. */
        return;
    }
}
#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */

#if BT_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 *            L2Cap_SecAccessRsp
 *---------------------------------------------------------------------------
 *
 *   Receives responses to SEC_AccessRequest() calls.
 *
 */


/*****************************************************************************
 * FUNCTION
 *  L2Cap_SecAccessRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_SecAccessRsp(const BtEvent *Event)
{
    L2CAP_Channel *channel;

    channel = ContainingRecord(Event->p.secToken, L2CAP_Channel, authorization);
    Assert(Event->p.secToken == &channel->authorization);
    Assert(channel->flags & L2CHF_SEC_PENDING);
    Assert((channel->state == L2C_W4_CONN_RSP) || (channel->state == L2C_W4_CONN_CNF) ||
           (channel->state == L2C_CLOSED) || (channel->state == L2C_W4_CREATE));

    channel->flags &= ~L2CHF_SEC_PENDING;
    OS_Report("l2cap L2Cap_SecAccessRsp eType:%d", Event->eType);
    if (Event->eType == BTEVENT_ACCESS_APPROVED)
    {
        L2Cap_StateMachine(channel, LLC_ACCESS_RSP);
        return;
    }

    if (Event->eType == BTEVENT_ACCESS_DENIED)
    {
        L2Cap_StateMachine(channel, LLC_ACCESS_REJ);
        return;
    }

    Assert(0);
}

#ifdef __BT_3_0_HS__
/*****************************************************************************
 * FUNCTION
 *  L2Cap_SecAMPAccessRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_SecAMPAccessRsp(const BtEvent *Event)
{
    L2CAP_Channel *channel;

    channel = ContainingRecord(Event->p.secToken, L2CAP_Channel, authorization);
    Assert(Event->p.secToken == &channel->authorization);
    Assert(channel->flags & L2CHF_SEC_PENDING);
    Assert((channel->state == L2C_W4_CREATE_RSP) || (channel->state == L2C_W4_CREATE) ||
           (channel->state == L2C_CLOSED));

    channel->flags &= ~L2CHF_SEC_PENDING;
    OS_Report("l2cap L2Cap_SecAccessRsp eType:%d", Event->eType);

    if (Event->eType == BTEVENT_ACCESS_APPROVED)
    {
        L2Cap_StateMachine(channel, LLC_AMP_ACCESS_RSP);
        return;
    }

    if (Event->eType == BTEVENT_ACCESS_DENIED)
    {
        L2Cap_StateMachine(channel, LLC_AMP_ACCESS_REJ);
        return;
    }

    Assert(0);
}
#endif

#endif /* BT_SECURITY == XA_ENABLED */

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
static void L2CAP_ProcessReceiveFlowPDU(L2CAP_Channel *Channel, HciBuffer *BuffDesc, U8 source)
{
    U16 read_offset = 0;
    U16 sdu_length_offset = 0;
    U16 control_length_offset = 2; /* RFC and FL control header 2 bytes */
    U16 fcs_length_offset = 0;
    U16 copy_length;
    U8 sar = 0;
    U16 legal_frame_size = 0;
    U8 controlFieldType = 0;
    U8 buffer_offset = 0;
    U8 *l2capHeaderPointer = 0;
    U16 previousParsePDUOffset = 0; // save privous EDR's parse status
    
    controlFieldType = L2CAP_GetControlFieldType(Channel);
    
    if(BuffDesc->len ==0)
        return;

    if((Channel->outFcsUsed != L2FCS_OPTION_NO_FCS) || (Channel->inFcsUsed != L2FCS_OPTION_NO_FCS))
    {
        fcs_length_offset = 0x02;
    }
    else
    {
        fcs_length_offset = 0x00;
    }
    if(controlFieldType != L2CAP_CONTROL_FIELD_EXTENDED)
    {
        control_length_offset = 2;
        legal_frame_size = 6+ fcs_length_offset;
    }
    else
    {
        control_length_offset = 4;
        legal_frame_size = 8+ fcs_length_offset;
    }
    /* frameLength doesn't include the 'Basic L2cAP header 4Byte) */
    if(source == 0x00)
        Channel->frameLength = LEtoHost16((Channel->link->rxBuffer + L2CAP_PRELUDE_SIZE));
    else
    {
        Channel->frameLength = LEtoHost16((BuffDesc->buffer));
        l2capHeaderPointer = BuffDesc->buffer;
        BuffDesc->buffer += 4;
        BuffDesc->len -= 4;
    }
    // EDR may only get part of PDU. HighSpeed will get a whole PDU.
    if( 0 != source && 0 != Channel->parsePDUOffset ){
        OS_Report("[T:] parsePDUOffset:%d sar:%d controlFieldType:%d", Channel->parsePDUOffset, sar, controlFieldType);
        previousParsePDUOffset = Channel->parsePDUOffset;
        Channel->parsePDUOffset = 0;
    }
    
    do
    {
        switch (Channel->parsePDUOffset)
        {
            case L2C_READ_FRAME_STAGE_CONTROL_BYTE0: /* Control 1 bytes */
                Channel->tempControlHdr[0] = BuffDesc->buffer[read_offset];
                if(Channel->tempControlHdr[0] & 0x01)
                {
                    /* S Frame*/
                    Channel->frameType = L2C_FRAME_TYPE_S;
                    if((Channel->frameLength != 2) &&
                       (Channel->frameLength != 4) &&
                       (Channel->frameLength != 6) &&
                       (Channel->frameLength != 8) && 
                       (Channel->frameLength != 10))
                    {
#if __BT_30_DEBUG__ == XA_ENABLED
                        Report(("frame length:%d",Channel->frameLength));
#endif
                        Report(("RX error 8"));
                        L2Cap_HandleRxError(Channel->link);
                        return;
                    }

                }
                else
                {
                    /* I Frame */
                    Channel->frameType = L2C_FRAME_TYPE_I;
                    Channel->frameOffset =0;
                    Channel->frameStartOffset = Channel->segReceivedStart;
#ifdef __BT_3_0_HS__      

                    if(Channel->remoteCid != 0x03)
                        Channel->link->lastTxRxOverDataChannel = OS_GetSystemTime();            
#endif            
#if __BT_30_DEBUG__ == XA_ENABLED
                    Report(("frame length:%d, local_mtu:%d",Channel->frameLength, Channel->localMtu));
                    Report(("frame length:%d, legal_frame_size:%d",Channel->frameLength, legal_frame_size));
#endif
                    if((Channel->frameLength > (Channel->localMtu+8)) || 
                       ( (Channel->frameLength+4+1) < legal_frame_size))  
                       // Channel->frameLength doesnot include basic-header. legal_frame_size has basic-header
                       
                       /* localMtu + 8,  */
                       /* Max up to 4 bytes Control,*/
                       /* Max up to 2 bytes SDU length,*/
                       /* Max up to 2 bytes FCS */
                    {
                        Report(("RX error 9"));                    
                        L2Cap_HandleRxError(Channel->link);
                        return;
                    }
                }
                Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_CONTROL_BYTE1;
                read_offset++;
                break;
            case L2C_READ_FRAME_STAGE_CONTROL_BYTE1: /* Control 2 bytes */
                Channel->tempControlHdr[1] = BuffDesc->buffer[read_offset];
                
                read_offset++;
                if(controlFieldType != L2CAP_CONTROL_FIELD_EXTENDED)
                {
                    /* Control Field 2 bytes */
                    if(Channel->frameType == L2C_FRAME_TYPE_S)
                        Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_S_FCS_BYTE0;
                    else
                        Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_I_SDU_LENGTH_BYTE0;
                }
                else
                {
                    /* Control Field 4 bytes */
                    Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_CONTROL_BYTE2;
                }
                break;
            case L2C_READ_FRAME_STAGE_CONTROL_BYTE2:
                Channel->tempControlHdr[2] = BuffDesc->buffer[read_offset];
                Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_CONTROL_BYTE3;
                read_offset++;
                break;
            case L2C_READ_FRAME_STAGE_CONTROL_BYTE3:
                Channel->tempControlHdr[3] = BuffDesc->buffer[read_offset];
                read_offset++;
                if(fcs_length_offset == 2)
                {
                    if(Channel->frameType == L2C_FRAME_TYPE_S)                
                    {
                        Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_S_FCS_BYTE0;
                    }
                    else
                    {
                        Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_I_SDU_LENGTH_BYTE0;
                    }                        
                }
                else
                {   
                    if(Channel->frameType == L2C_FRAME_TYPE_S)                
                    {
                        /* No FCS header */
                        /* Process S frame */
                        Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_CONTROL_BYTE0;
                        read_offset++;
                        if(source == 0)
                            L2Cap_ProcessSFrame(Channel, Channel->link->rxBuffer + L2CAP_PRELUDE_SIZE);
                        else
                            L2Cap_ProcessSFrame(Channel, l2capHeaderPointer);
                        if(source == 0x00)
                            Channel->link->rxState = BRXS_COMPLETE;
                    }
                    else
                    {
                        Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_I_SDU_LENGTH_BYTE0;
                    }
                }
                break;
            case L2C_READ_FRAME_STAGE_S_FCS_BYTE0:
                if(Channel->frameType == L2C_FRAME_TYPE_S)
                {
                    /* S frame, FCS part 1*/
                    Channel->tempFCS[0] = BuffDesc->buffer[read_offset];
                    Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_S_FCS_BYTE1;
                    read_offset++;
                }
                break;
            case L2C_READ_FRAME_STAGE_I_SDU_LENGTH_BYTE0:
                if(controlFieldType != L2CAP_CONTROL_FIELD_EXTENDED)
                {
                    sar = (Channel->tempControlHdr[1] & 0xC0) >> 6;
                }
                else
                {
                    sar = (Channel->tempControlHdr[2] & 0x03);
                }
                if(sar == 0x01)
                {

                    /* I Frame SAR Start PDU*/
                    if(Channel->frameLength < (legal_frame_size+2))
                    {
                        L2Cap_ChannelRxError(Channel, L2ERR_INVALID_PDU);
                        return;
                    }
                    if (IsListEmpty(Channel->freeSduBuffer))
                    {
                        Report(("MTU violation 3"));
                        L2Cap_HandleRxError(Channel->link);
                        return;
                    }
                    else
                    {
                        // received twice the start sar iframe in wifi , ignore the pdu
                        if(Channel->flowRxBuffer !=0)
                        {
                            Report(("new fragment, the flowRxBuffer shall be 0. Restart SDU offset"));
                            // ALPS00426568 - customer internal cellphone, 
                            //L2Cap_HandleRxError(Channel->link);
                            //return;
                            
                        }else{
                            Channel->flowRxBuffer = (BtRxPacket *)RemoveHeadList(Channel->freeSduBuffer);
                        }
                        
                        Channel->flowRxBuffer->dataLen = 0;
                        Channel->flowRxBuffer->reqSeq = 0;
                    }         
                    Channel->tempSDULengthHdr[0] = BuffDesc->buffer[read_offset];
                    Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_I_SDU_LENGTH_BYTE1;
                    read_offset++;
                }
                else if((sar == 0x02) || (sar == 0x03))
                {
                    /* I Frame SAR Continue PDU*/
                    /* I Frame SAR End PDU*/
                    Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_I_COPY_DATA;
                    /* read_offset will not ++;*/
                }
                else
                {
                    Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_I_COPY_DATA;
                    /* I Frame no segment */
                    if (IsListEmpty(Channel->freeSduBuffer))
                    {
                        Report(("MTU violation 3"));
                        L2Cap_HandleRxError(Channel->link);
                        return;
                    }
                    else
                    {
                        if(Channel->flowRxBuffer !=0)
                        {
                            Report(("new fragment, the flowRxBuffer shall be 0"));
                            L2Cap_HandleRxError(Channel->link);
                            return;
                        }                        
                        Channel->flowRxBuffer = (BtRxPacket *)RemoveHeadList(Channel->freeSduBuffer);
                        Channel->flowRxBuffer->dataLen = 0;
                        Channel->flowRxBuffer->reqSeq = 0;
                    }
                    Channel->segReceivedStart = 0;
                    Channel->sduLength = Channel->frameLength;
                    /* read_offset will not ++;*/
                }
                break;
            case L2C_READ_FRAME_STAGE_S_FCS_BYTE1:
                Assert(Channel->frameType == L2C_FRAME_TYPE_S);
                /* S frame, FCS part 2*/
                Channel->tempFCS[1] = BuffDesc->buffer[read_offset];
                /* S frame read completed */
                /* Process S frame */
                Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_CONTROL_BYTE0;
                read_offset++;
                if(source == 0)
                    L2Cap_ProcessSFrame(Channel, Channel->link->rxBuffer + L2CAP_PRELUDE_SIZE);
                else
                    L2Cap_ProcessSFrame(Channel, l2capHeaderPointer);

                if(source == 0x00)
                    Channel->link->rxState = BRXS_COMPLETE;
                break;
            case L2C_READ_FRAME_STAGE_I_SDU_LENGTH_BYTE1:
                /* Read SAR = 0x01, read the second length */
                Channel->tempSDULengthHdr[1] = BuffDesc->buffer[read_offset];
                Channel->sduLength = LEtoHost16(Channel->tempSDULengthHdr);
#if __BT_30_DEBUG__ == XA_ENABLED
                Report(("Channel->sduLength:%d",Channel->sduLength));
                Report(("Channel->localMpu:%d",Channel->localMpu));                
#endif                
                if(Channel->sduLength > Channel->localMpu)
                {
                    Report(("RX error 6"));
                    L2Cap_HandleRxError(Channel->link);
                    return;
                }                
                Channel->segReceivedStart = 0;     
				Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_I_COPY_DATA;                
                read_offset++;
                break;
            case L2C_READ_FRAME_STAGE_I_COPY_DATA:
                /* Channel->segReceivedStart: Received buffer offset */
                /* Channel->sduLength: Expected total SDU length */
                /* Channel->frameLength: Current I frame length */
                /* BuffDesc->len: Buffer remain length */
                /* BuffDesc->buffer: Buffer remain data */
                if(controlFieldType != L2CAP_CONTROL_FIELD_EXTENDED)
                {
                    if((Channel->tempControlHdr[1] >>6) == 0x01)
                    {
                        sdu_length_offset = 2;
                    }                    
                }
                else
                {
                    if((Channel->tempControlHdr[2] & 0x03) == 0x01)
                    {
                        sdu_length_offset = 2;
                    }                    
                }                
                if((Channel->frameLength - sdu_length_offset - control_length_offset - fcs_length_offset) 
                    < ( Channel->frameOffset+ (BuffDesc->len - read_offset)))
                {
                    /* Received (payload size + fcs) larger than I frame payload size */
                    copy_length = (Channel->frameLength - sdu_length_offset - control_length_offset - fcs_length_offset) -
                                  (Channel->frameOffset);
                    Channel->parsePDUOffset = L2C_READ_FRAME_I_FCS_BYTE0;
                }
                else
                {
                    /* Copy all remain data */
                    copy_length = BuffDesc->len - read_offset;
                }
                if(copy_length !=0)
                {                
                    if(sar != 0x00)
                    {
                        Report(("SAR not 0x00"));
                    }

                    if( 0 != Channel->localMpu &&  (Channel->segReceivedStart+Channel->frameOffset+copy_length) > Channel->localMpu )
                    {
                        Report(("Channel->localMpu:%d segReceivedStart:%d frameOffset:%d copylen:%d",Channel->localMpu, Channel->segReceivedStart, Channel->frameOffset, copy_length));  
                        Report(("RX error 7 "));
                        L2Cap_HandleRxError(Channel->link);
                        return;
                    }                

                    OS_MemCopy(
                        Channel->flowRxBuffer->data + 
                        Channel->segReceivedStart +
                        Channel->frameOffset,
                        BuffDesc->buffer + read_offset, 
                        copy_length);
                    read_offset += copy_length;
                    Channel->frameOffset += copy_length;
                }
                if((fcs_length_offset ==0) &&
                    (Channel->frameOffset == 
                    (Channel->frameLength
                    - sdu_length_offset - control_length_offset)))
                {
                    /* We shall process I frame here */
                    /* No FCS I Frame */
                    if (Channel->inLinkMode != LINK_MODE_STREAMMING)
                    {
                        if(source == 0)
                            L2Cap_ProcessIFrame(Channel, Channel->link->rxBuffer + L2CAP_PRELUDE_SIZE);
                        else
                            L2Cap_ProcessIFrame(Channel, l2capHeaderPointer);
                        
                    }
                    else
                    {
                        if(source == 0)
                            L2Cap_ProcessStreamingIFrame(Channel, Channel->link->rxBuffer + L2CAP_PRELUDE_SIZE);
                        else
                            L2Cap_ProcessStreamingIFrame(Channel, l2capHeaderPointer);
                    }

                    Channel->segReceivedStart += Channel->frameOffset;
                    Channel->frameOffset = 0;
                    Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_CONTROL_BYTE0;
                    if(source == 0x00)
                        Channel->link->rxState = BRXS_COMPLETE;
                    if((Channel->sar == 0x02) || (Channel->sar == 0x00))
                        Channel->segReceivedStart = 0;
                }
                break;
            case L2C_READ_FRAME_I_FCS_BYTE0:
                /* parse FCS first one byte */
                Channel->tempFCS[0] = BuffDesc->buffer[read_offset];
                Channel->parsePDUOffset = L2C_READ_FRAME_I_FCS_BYTE1;
                read_offset++;
                break;
            case L2C_READ_FRAME_I_FCS_BYTE1:
                /* parse FCS second byte */
                if((Channel->tempControlHdr[1] >>6) == 0x01)
                {
                    sdu_length_offset = 2;
                }                
                Channel->tempFCS[1] = BuffDesc->buffer[read_offset];
                Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_CONTROL_BYTE0;
                if(((Channel->frameOffset + 2) != 
                    (Channel->frameLength
                    - sdu_length_offset - control_length_offset)))
                {
                    Assert(0=="It shall not happens");
                }
                if (Channel->inLinkMode != LINK_MODE_STREAMMING)
                {
                    if(source == 0)
                        L2Cap_ProcessIFrame(Channel, Channel->link->rxBuffer + L2CAP_PRELUDE_SIZE);
                    else
                        L2Cap_ProcessIFrame(Channel, l2capHeaderPointer);
                }
                else
                {
                    if(source == 0)
                        L2Cap_ProcessStreamingIFrame(Channel, Channel->link->rxBuffer + L2CAP_PRELUDE_SIZE);
                    else
                        L2Cap_ProcessStreamingIFrame(Channel, l2capHeaderPointer);
                }
                Channel->segReceivedStart += Channel->frameOffset;
                if((Channel->sar == 0x02) || (Channel->sar == 0x00))
                    Channel->segReceivedStart = 0;

                if(Channel->flowRxBuffer !=0)
                {
                    Report(("The flowRxBuffer is not zero"));
                }
                /* We shall process I frame here */
                Channel->frameOffset = 0;
                Channel->parsePDUOffset = L2C_READ_FRAME_STAGE_CONTROL_BYTE0;
                if(source == 0x00)
                    Channel->link->rxState = BRXS_COMPLETE;
                read_offset++;                
                break;
        }
    
    }while (read_offset < BuffDesc->len);

    // restore the br/edr's Channel->parsePDUOffset
    if( 0 != source && 0 != previousParsePDUOffset ){
        Channel->parsePDUOffset = previousParsePDUOffset;
    }
        
}

void L2Cap_ReturnflowRxBuffer(L2CAP_Channel *Channel)
{
    if(Channel->flowRxBuffer !=0)
    {
        Channel->flowRxBuffer->dataLen = 0;
        InsertTailList(Channel->freeSduBuffer, &Channel->flowRxBuffer->node);
        Channel->flowRxBuffer = 0;
    }

}

#endif
