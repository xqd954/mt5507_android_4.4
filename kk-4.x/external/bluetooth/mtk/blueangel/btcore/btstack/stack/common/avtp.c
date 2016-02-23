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

/****************************************************************************
 *
 * File:
 *     $Workfile:avtp.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains code for encoding AVDTP and AVCTP packets.
 *             
 * Created:     Mar 22, 2004
 *
 * Copyright 2004 - 2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, Inc. 
 * This work contains confidential and proprietary information of Extended 
 * Systems, Inc. which is protected by copyright, trade secret, trademark and 
 * other intellectual property rights.
 *
 ****************************************************************************/
#include "bt_common.h"
#include "btalloc.h"
#include "sys/avtp.h"

/* Function Prototypes */
static BtStatus AvtpSendPacket(AvtpChannel *avtpChnl);
static void AvtpTxTimeout(EvmTimer *Timer);


/*****************************************************************************
 * FUNCTION
 *  AVTP_InitChannel
 * DESCRIPTION
 *  Initialize the Channel.
 * PARAMETERS
 *  chnl            [IN]        
 *  L2ChannelId     [IN]        
 *  Callback        [IN]        
 *  Timeout         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AVTP_InitChannel(AvtpChannel *chnl, L2capChannelId L2ChannelId, AvtpCallback Callback, TimeT Timeout)
{
    OS_MemSet((U8*) chnl, 0, sizeof(AvtpChannel));
    InitializeListHead(&chnl->avPacketList);
    chnl->l2ChannelId = L2ChannelId;
    chnl->callback = Callback;
    chnl->txTimeout = Timeout;
    chnl->txCmdTimeout = FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  AVTP_DeinitChannel
 * DESCRIPTION
 *  Deinitialize the Channel.
 * PARAMETERS
 *  chnl        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AVTP_DeinitChannel(AvtpChannel *chnl)
{
    if (chnl->txTimer.context)
    {
        chnl->txTimer.context = 0;
        EVM_CancelTimer(&chnl->txTimer);
    }
}


/*****************************************************************************
 * FUNCTION
 *  AVTP_SendStart
 * DESCRIPTION
 *  Start sending a packet.
 *  Determines how many L2CAP packets are required.
 *  chnl->txPacketsLeft is set to number of L2CAP packets required
 *  to send the AV packet.
 * PARAMETERS
 *  chnl            [IN]        
 *  avtpPacket      [IN]        
 * RETURNS
 *  BT_STATUS_PENDING - packet passed successfully to L2CAP.
 *****************************************************************************/
BtStatus AVTP_SendStart(AvtpChannel *chnl, AvtpPacket *avtpPacket)
{
    BtStatus status = BT_STATUS_IN_USE;
    U8 hdrDiff;
    U16 remainder;
    U16 l2cap_mtu = 0;

    l2cap_mtu = ((L2CAP_GetTxMtu((chnl)->l2ChannelId) < 48) ? (48) : (L2CAP_GetTxMtu((chnl)->l2ChannelId)));
    if (chnl->txState == AVTP_TRANS_STATE_FIRST)
    {
        /* Send a packet */
        chnl->curAvPacket = avtpPacket;
        chnl->offset = 0;

        if (chnl->curAvPacket->useMsgHdr == TRUE)
        {
            hdrDiff = 6;
        }
        else
        {
            hdrDiff = 2;
        }

        /* See if this is a single packet */
        if (chnl->curAvPacket->txDataLen <= l2cap_mtu - hdrDiff)
        {
            chnl->txPacketsLeft = 1;
            chnl->packetSize = chnl->curAvPacket->txDataLen;
        }
        else
        {
            /* Calculate number of packets to send */
            chnl->txPacketsLeft = (chnl->curAvPacket->txDataLen + hdrDiff) / (l2cap_mtu - 1) + 1;

            /* Adjust number of packets if last one only contains a header */
            remainder = (chnl->curAvPacket->txDataLen + hdrDiff) % (l2cap_mtu - 1);
            if (remainder == 0)
            {
                chnl->txPacketsLeft--;
            }

            /* Set the reference packet size */
            chnl->packetSize = l2cap_mtu - 1;
        }

        /* Start sending the packet */
        status = AvtpSendPacket(chnl);
        if (status == BT_STATUS_PENDING)
        {
            chnl->txState = AVTP_TRANS_STATE_CONTINUE;
            if (avtpPacket->msgType == AVTP_MSG_TYPE_COMMAND)
            {
                /* Start Timer */
                chnl->txTimer.func = AvtpTxTimeout;
                chnl->txTimer.context = chnl;
                chnl->resetTimer = FALSE;
                EVM_StartTimer(&chnl->txTimer, chnl->txTimeout);
            }
        }
    }
    else
    {
        if (!IsNodeOnList(&chnl->avPacketList, &avtpPacket->node))
        {
            InsertTailList(&chnl->avPacketList, &avtpPacket->node);
            status = BT_STATUS_PENDING;
        }
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVTP_SendContinue
 * DESCRIPTION
 *  Continue sending a packet.
 * PARAMETERS
 *  chnl        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AVTP_SendContinue(AvtpChannel *chnl)
{
    BtStatus status = BT_STATUS_FAILED;
    AvtpCallbackParms parms;
    AvtpPacket *avtpPacket;

    parms.status = BT_STATUS_SUCCESS;
    bt_trace(TRACE_GROUP_1, AVTP_SENDCONTINUEBT_DEBUGx02X, chnl->txState);
    if (chnl->txState == AVTP_TRANS_STATE_CONTINUE)
    {
        if (chnl->txPacketsLeft == 0)
        {
            /* Set up the Event */
            bt_trace(TRACE_GROUP_1, CHNLTXSTATEBT_DEBUGx02X, chnl->txState);
            chnl->txState = AVTP_TRANS_STATE_FIRST;
            parms.event = AVTP_EVENT_TX_DONE;
            parms.packet = chnl->curAvPacket;

            /* Acknowledge the last packet */
            chnl->callback(chnl, &parms);

            /* Check for another packet */
            while (!IsListEmpty(&chnl->avPacketList))
            {
                /* There is a new packet to send */
                avtpPacket = (AvtpPacket*) RemoveHeadList(&chnl->avPacketList);
                status = AVTP_SendStart(chnl, avtpPacket);
                if (status != BT_STATUS_PENDING)
                {
                    /* Failed to send next packet */
                    parms.event = AVTP_EVENT_TX_ERROR;
                    parms.status = status;
                    parms.packet = avtpPacket;
                    chnl->callback(chnl, &parms);
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            /* Send a packet */
            status = AvtpSendPacket(chnl);
            if (status != BT_STATUS_PENDING)
            {
                /* Could not send */
                if (chnl->txTimer.context)
                {
                    chnl->txTimer.context = 0;
                    EVM_CancelTimer(&chnl->txTimer);
                }
                chnl->txState = AVTP_TRANS_STATE_FIRST;
                parms.event = AVTP_EVENT_TX_ERROR;
                parms.status = status;
                chnl->callback(chnl, &parms);
            }
        }
    }
    else
    {
        bt_trace(TRACE_GROUP_1, SEND_CONTINUE_CHNL_STATE_IS_AVTP_TRANS_STATE_FIRST);
        if (chnl->txCmdTimeout)
        {
            chnl->txState = AVTP_TRANS_STATE_FIRST;
            parms.event = AVTP_EVENT_TX_ERROR;
            parms.status = status;
            parms.packet = chnl->curAvPacket;
            chnl->callback(chnl, &parms);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  AVTP_Receive
 * DESCRIPTION
 *  Parse received data.
 * PARAMETERS
 *  chnl            [IN]        
 *  Buffer          [IN]        
 *  Len             [IN]        
 *  rxIdSize        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AVTP_Receive(AvtpChannel *chnl, U8 *Buffer, U16 Len, U8 rxIdSize)
{
    U8 messageType = Buffer[0] & 0x03;
    U8 packetType = (Buffer[0] & 0x0C) >> 2;
    U8 transId = Buffer[0] >> 4;
    U16 rxId = 0;
    U8 offset = 0;
    U8 event = AVTP_EVENT_RX_IND;
    AvtpCallbackParms parms;

    /* Parse and get number of packets */
    switch (packetType)
    {
        case AVTP_PACKET_TYPE_SINGLE:
            chnl->rxState = AVTP_TRANS_STATE_FIRST;
            chnl->rxPacketsLeft = 0;
            rxId = rxIdSize == 1 ? Buffer[1] : BEtoHost16(Buffer + 1);
            chnl->rxId = rxId;
            chnl->resetTimer = FALSE;
            if ((rxIdSize == 2) && ((Buffer[3] & 0x0f) == 0x0f))
            {
                /* This is an interim response, reset timer. */
                chnl->resetTimer = TRUE;
            }

            /* Set pointer to the packet data */
            offset += 1 + rxIdSize;
            break;

        case AVTP_PACKET_TYPE_START:
            chnl->rxState = AVTP_TRANS_STATE_CONTINUE;
            chnl->rxPacketsLeft = Buffer[1] - 1;
            rxId = rxIdSize == 1 ? Buffer[2] : BEtoHost16(Buffer + 2);
            chnl->rxId = rxId;
            chnl->resetTimer = FALSE;
            if ((rxIdSize == 2) && ((Buffer[4] & 0x0f) == 0x0f))
            {
                /* This is an interim response, reset timer. */
                chnl->resetTimer = TRUE;
            }

            /* Set pointer to the packet data */
            offset += 2 + rxIdSize;
            break;

        case AVTP_PACKET_TYPE_CONTINUE:
            /* Continue Packet */
            rxId = chnl->rxId;
            if (chnl->rxPacketsLeft == 0)
            {
                /* Received too many packets */
                chnl->rxState = AVTP_TRANS_STATE_FIRST;
                event = AVTP_EVENT_RX_BUFFER_OVERRUN;
                goto call_back;
            }
            chnl->rxPacketsLeft--;

            /* Set pointer to the packet data */
            offset++;
            break;

        case AVTP_PACKET_TYPE_END:
            /* Last Packet */
            chnl->rxState = AVTP_TRANS_STATE_FIRST;
            rxId = chnl->rxId;
            chnl->rxPacketsLeft--;
            if (chnl->rxPacketsLeft != 0)
            {
                /* Received too few packets */
                event = AVTP_EVENT_RX_BUFFER_UNDERRUN;
                goto call_back;
            }

            /* Set pointer to the packet data */
            offset++;
            break;

        default:
            /* Unknown packet type */
            chnl->rxState = AVTP_TRANS_STATE_FIRST;
            event = AVTP_EVENT_RX_UNKNOWN_PACKET;
            goto call_back;
    }

    /* Call the Channel Manager callback */
    if (messageType == AVTP_MSG_TYPE_COMMAND)
    {
        /* Receiving a command */
        chnl->acpTransId = transId;
    }
    else if ((messageType == AVTP_MSG_TYPE_ACCEPT) || (messageType == AVTP_MSG_TYPE_REJECT))
    {

        /* Receiving a response */
        if (chnl->intTransId == transId)
        {
            if (!chnl->resetTimer && ((packetType == AVTP_PACKET_TYPE_SINGLE) || (packetType == AVTP_PACKET_TYPE_END)))
            {
                /* Transaction ID matches */
                int tmpTransId = 0;
                chnl->intTransId++;
                tmpTransId = chnl->intTransId;
                chnl->intTransId = tmpTransId % 16;
                //chnl->intTransId = ++chnl->intTransId % 16;
            }
        }
        else
        {
            chnl->rxState = AVTP_TRANS_STATE_FIRST;
            event = AVTP_EVENT_RX_BAD_TRANS_ID;
            goto call_back;
        }

        if (messageType == AVTP_MSG_TYPE_ACCEPT)
        {
            /* Accept Message */
            if (chnl->rxPacketsLeft == 0)
            {
                /* Response Received */
                if (chnl->txTimer.context)
                {
                    if (chnl->resetTimer)
                    {
                        chnl->txTimer.func = AvtpTxTimeout;
                        chnl->txTimer.context = chnl;
                        EVM_StartTimer(&chnl->txTimer, chnl->txTimeout);
                    }
                    else
                    {
                        chnl->txTimer.context = 0;
                        EVM_CancelTimer(&chnl->txTimer);
                    }
                }
            }
        }
        else if (messageType == AVTP_MSG_TYPE_REJECT)
        {
            /* Reject Message */
            if (chnl->rxPacketsLeft == 0)
            {
                /* Response Received */
                if (chnl->txTimer.context)
                {
                    chnl->txTimer.context = 0;
                    EVM_CancelTimer(&chnl->txTimer);
                }
            }
        }
        else
        {
            /* Unknown message type */
            chnl->rxState = AVTP_TRANS_STATE_FIRST;
            event = AVTP_EVENT_RX_UNKNOWN_MESSAGE;
        }
    }
    else
    {
        /* Unknown message type */
        chnl->acpTransId = transId;
        chnl->rxState = AVTP_TRANS_STATE_FIRST;
        /* event = AVTP_EVENT_RX_UNKNOWN_MESSAGE; */
        event = AVTP_EVENT_RX_IND;
    }

  call_back:

    parms.event = event;
    parms.status = BT_STATUS_SUCCESS;
    parms.msgType = messageType;
    parms.pktType = packetType;
    parms.rxId = rxId;
    parms.packetsLeft = chnl->rxPacketsLeft;
    parms.data = Buffer + offset;
    parms.len = Len - offset;
    parms.transid = transId;
    chnl->callback(chnl, &parms);
}


/*****************************************************************************
 * FUNCTION
 *  AvtpSendPacket
 * DESCRIPTION
 *  Send a single packet to L2CAP.
 *  AV packet is fragmented (if neccessary) into an L2CAP packet.
 *  Only 1 packet is sent. Subsequent ones will be sent when the
 *  one sent is confirmed.
 * PARAMETERS
 *  chnl        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus AvtpSendPacket(AvtpChannel *chnl)
{
    BtStatus status;
    U8 headerLen;
    U8 transId;
    U8 *hdrPos;
    U16 l2cap_mtu = 0;

    l2cap_mtu = ((L2CAP_GetTxMtu((chnl)->l2ChannelId) < 48) ? (48) : (L2CAP_GetTxMtu((chnl)->l2ChannelId)));

    if ((chnl->curAvPacket->msgType == AVTP_MSG_TYPE_COMMAND) && (chnl->curAvPacket->txId != 0))
    {
        transId = chnl->intTransId;
    }
    else
    {
        transId = chnl->acpTransId;
    }

    if (chnl->txState == AVTP_TRANS_STATE_FIRST)
    {
        /* Start State */
        if (chnl->txPacketsLeft > 1)
        {
            /* Start Packet */
            hdrPos = chnl->packet.header + BT_PACKET_HEADER_LEN;

            if (chnl->curAvPacket->useMsgHdr == TRUE)
            {
                hdrPos -= 3;
                OS_MemCopy(hdrPos, chnl->curAvPacket->msgHdr, 3);
            }

            if (chnl->curAvPacket->txIdSize == 2)
            {
                hdrPos -= 2;
                StoreBE16(hdrPos, chnl->curAvPacket->txId);
            }
            else
            {
                hdrPos--;
                *hdrPos = (U8) chnl->curAvPacket->txId;
            }

            hdrPos--;
            *hdrPos = chnl->txPacketsLeft;

            hdrPos--;
            *hdrPos = transId << 4;
            *hdrPos += (U8) (AVTP_PACKET_TYPE_START) << 2;
            *hdrPos += chnl->curAvPacket->msgType;

            headerLen = chnl->packet.header + BT_PACKET_HEADER_LEN - hdrPos;
        }
        else
        {
            /* Single Packet */
            hdrPos = chnl->packet.header + BT_PACKET_HEADER_LEN;

            if (chnl->curAvPacket->useMsgHdr == TRUE)
            {
                hdrPos -= 3;
                OS_MemCopy(hdrPos, chnl->curAvPacket->msgHdr, 3);
            }

            if (chnl->curAvPacket->txIdSize == 2)
            {
                hdrPos -= 2;
                StoreBE16(hdrPos, chnl->curAvPacket->txId);
            }
            else
            {
                hdrPos--;
                *hdrPos = (U8) chnl->curAvPacket->txId;
            }

            hdrPos--;
            *hdrPos = transId << 4;
            *hdrPos += (U8) (AVTP_PACKET_TYPE_SINGLE) << 2;
            *hdrPos += chnl->curAvPacket->msgType;

            headerLen = chnl->packet.header + BT_PACKET_HEADER_LEN - hdrPos;
        }
    }
    else
    {
        /* Continue State */
        hdrPos = chnl->packet.header + BT_PACKET_HEADER_LEN;

        hdrPos--;
        *hdrPos = transId << 4;
        if (chnl->txPacketsLeft > 1)
        {
            /* Continue Packet */
            *hdrPos += (U8) (AVTP_PACKET_TYPE_CONTINUE) << 2;
        }
        else
        {
            /* End Packet */
            *hdrPos += (U8) (AVTP_PACKET_TYPE_END) << 2;
        }

        *hdrPos += chnl->curAvPacket->msgType;

        headerLen = chnl->packet.header + BT_PACKET_HEADER_LEN - hdrPos;
    }

    /* Setup the packet */
    chnl->packet.headerLen = headerLen;
    if (chnl->curAvPacket->txData)
    {
        chnl->packet.data = chnl->curAvPacket->txData + chnl->offset;
    }
    chnl->packet.dataLen = min(chnl->packetSize, chnl->curAvPacket->txDataLen - chnl->offset);
    if ((chnl->packet.dataLen + headerLen) > l2cap_mtu)
    {
        chnl->packet.dataLen -= chnl->packet.dataLen + headerLen - l2cap_mtu;
    }
    chnl->packet.priority_type = BTP_FLAG_HIGH_PRIPORTY;
    /* Send it to L2CAP */
    status = L2CAP_SendData(chnl->l2ChannelId, &chnl->packet);

    if (status == BT_STATUS_PENDING)
    {
        /* Calculate bytes left to send */
        chnl->offset += chnl->packet.dataLen;
        chnl->txPacketsLeft--;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvtpTxTimeout
 * DESCRIPTION
 *  Timeout handler for commands.
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvtpTxTimeout(EvmTimer *Timer)
{
    AvtpChannel *chnl = (AvtpChannel*) Timer->context;
    AvtpCallbackParms parms;

    chnl->txTimer.context = 0;
    chnl->txState = AVTP_TRANS_STATE_FIRST;
    parms.event = AVTP_EVENT_TX_TIMEOUT;
    parms.status = BT_STATUS_SUCCESS;
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVTP_TX_TIMEOUT,HCC(aclTxPacketsLeft));
    chnl->txCmdTimeout = TRUE;
    chnl->callback(chnl, &parms);
}

