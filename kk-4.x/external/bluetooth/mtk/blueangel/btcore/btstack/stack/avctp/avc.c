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
 *     $Workfile:avc.c$ for XTNDAccess Blue SDK, Version 2.0
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
#include "sys/avc.h"

/* Function Prototypes */
static BtStatus AvcSendPacket(AvcChannel *avcChnl);
static void AvcTxTimeout(EvmTimer *Timer);

/* AVRCPv14 Should not check the TRANSID */
#define __AVRCP_REMOVE_LIMITATION__

/*****************************************************************************
 * FUNCTION
 *  AVC_InitChannel
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
void AVC_InitChannel(AvcChannel *chnl, L2capChannelId L2ChannelId, AvcCallback Callback, TimeT Timeout)
{
    OS_MemSet((U8*) chnl, 0, sizeof(AvcChannel));
    InitializeListHead(&chnl->avPacketList);
    chnl->l2ChannelId = L2ChannelId;
    chnl->callback = Callback;
    chnl->txTimeout = Timeout*10;
    chnl->txCmdTimeout = FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  AVC_DeinitChannel
 * DESCRIPTION
 *  Deinitialize the Channel.
 * PARAMETERS
 *  chnl        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AVC_DeinitChannel(AvcChannel *chnl)
{
    if (chnl->txTimer.context)
    {
        chnl->txTimer.context = 0;
        OS_Report("[AVC][TIMER] EVM_CancelTimer 0x%x", &chnl->txTimer);
        //1 EVM_CancelTimer(&chnl->txTimer);
    }
}


/*****************************************************************************
 * FUNCTION
 *  AVC_SendStart
 * DESCRIPTION
 *  Start sending a packet.
 *  Determines how many L2CAP packets are required.
 *  chnl->txPacketsLeft is set to number of L2CAP packets required
 *  to send the AV packet.
 * PARAMETERS
 *  chnl            [IN]        
 *  avcPacket      [IN]        
 * RETURNS
 *  BT_STATUS_PENDING - packet passed successfully to L2CAP.
 *****************************************************************************/
BtStatus AVC_SendStart(AvcChannel *chnl, AvcPacket *avcPacket)
{
    BtStatus status = BT_STATUS_IN_USE;
    U8 hdrDiff;
    U16 remainder;
    U16 l2cap_mtu = 0;

    l2cap_mtu = ((L2CAP_GetTxMtu((chnl)->l2ChannelId) < 48) ? (48) : (L2CAP_GetTxMtu((chnl)->l2ChannelId)));
    if (chnl->txState == AVC_TRANS_STATE_FIRST)
    {
        /* Send a packet */
        chnl->curAvPacket = avcPacket;
        chnl->offset = 0;

        if (chnl->curAvPacket->useMsgHdr == TRUE)
        {
            hdrDiff = 6; /* ?? AV/C frame ctype(1B) */
        }
        else
        {
            hdrDiff = 2; /* AVRCP avctp frame: PDUId(1B) packet-type(1B) */
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
        status = AvcSendPacket(chnl);
        if (status == BT_STATUS_PENDING)
        {
            chnl->txState = AVC_TRANS_STATE_CONTINUE;
            if (avcPacket->msgType == AVC_MSG_TYPE_COMMAND)
            {
                OS_Report("[AVC][TIMER] StartTimer 0x%x", &chnl->txTimer);
                /* Start Timer */
                chnl->txTimer.func = AvcTxTimeout;
                chnl->txTimer.context = chnl;
                chnl->resetTimer = FALSE;
                //1 EVM_StartTimer(&chnl->txTimer, chnl->txTimeout);
            }
        }
    }
    else
    {
        if (!IsNodeOnList(&chnl->avPacketList, &avcPacket->node))
        {
            InsertTailList(&chnl->avPacketList, &avcPacket->node);
            status = BT_STATUS_PENDING;
        }
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVC_SendContinue
 * DESCRIPTION
 *  Continue sending a packet.
 * PARAMETERS
 *  chnl        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AVC_SendContinue(AvcChannel *chnl)
{
    BtStatus status = BT_STATUS_FAILED;
    AvcCallbackParms parms;
    AvcPacket *avcPacket;

    parms.status = BT_STATUS_SUCCESS;
    bt_trace(TRACE_GROUP_1, AVC_SENDCONTINUEBT_DEBUGx02X, chnl->txState);
    if (chnl->txState == AVC_TRANS_STATE_CONTINUE)
    {
        if (chnl->txPacketsLeft == 0)
        {
            /* Set up the Event */
            bt_trace(TRACE_GROUP_1, CHNLTXSTATEBT_DEBUGx02X, chnl->txState);
            chnl->txState = AVC_TRANS_STATE_FIRST;
            parms.event = AVC_EVENT_TX_DONE;
            parms.packet = chnl->curAvPacket;

            /* Acknowledge the last packet */
            chnl->callback(chnl, &parms);

            /* Check for another packet */
            while (!IsListEmpty(&chnl->avPacketList))
            {
                /* There is a new packet to send */
                avcPacket = (AvcPacket*) RemoveHeadList(&chnl->avPacketList);
                status = AVC_SendStart(chnl, avcPacket);
                if (status != BT_STATUS_PENDING)
                {
                    /* Failed to send next packet */
                    parms.event = AVC_EVENT_TX_ERROR;
                    parms.status = status;
                    parms.packet = avcPacket;
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
            /* Continue to Send a packet */
            status = AvcSendPacket(chnl);
            if (status != BT_STATUS_PENDING)
            {
                /* Could not send */
                if (chnl->txTimer.context)
                {
                    OS_Report("[AVC][TIMER] CancelTimer 0x%x", &chnl->txTimer);
                    chnl->txTimer.context = 0;
                    //1 EVM_CancelTimer(&chnl->txTimer);
                }
                chnl->txState = AVC_TRANS_STATE_FIRST;
                parms.event = AVC_EVENT_TX_ERROR;
                parms.status = status;
                chnl->callback(chnl, &parms);
            }
        }
    }
    else
    {
        bt_trace(TRACE_GROUP_1, SEND_CONTINUE_CHNL_STATE_IS_AVC_TRANS_STATE_FIRST);
        if (chnl->txCmdTimeout)
        {
            chnl->txState = AVC_TRANS_STATE_FIRST;
            parms.event = AVC_EVENT_TX_ERROR;
            parms.status = status;
            parms.packet = chnl->curAvPacket;
            chnl->callback(chnl, &parms);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  AVC_Receive
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
void AVC_Receive(AvcChannel *chnl, U8 *Buffer, U16 Len, U8 rxIdSize)
{
    U8 messageType = Buffer[0] & 0x03;
    U8 packetType = (Buffer[0] & 0x0C) >> 2;
    U8 transId = Buffer[0] >> 4;
    U16 rxId = 0;
    U8 offset = 0;
    U8 event = AVC_EVENT_RX_IND;
    AvcCallbackParms parms;

    /* Parse AVCTP frame header and get number of packets */
    switch (packetType)
    {
        case AVC_PACKET_TYPE_SINGLE:
            chnl->rxState = AVC_TRANS_STATE_FIRST;
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
            offset += 1 + rxIdSize; /* AVCTP single frame is transLabel(1Octet) ProfileId(2Octet)*/
            break;

        case AVC_PACKET_TYPE_START:
            chnl->rxState = AVC_TRANS_STATE_CONTINUE;
            chnl->rxPacketsLeft = Buffer[1] - 1;
            OS_Report("[AVC] ptype:start packetLeft:%d", chnl->rxPacketsLeft);
            rxId = rxIdSize == 1 ? Buffer[2] : BEtoHost16(Buffer + 2);
            chnl->rxId = rxId;
            chnl->resetTimer = FALSE;
            if ((rxIdSize == 2) && ((Buffer[4] & 0x0f) == 0x0f))
            {
                /* This is an interim response, reset timer. */
                chnl->resetTimer = TRUE;
            }

            /* Set pointer to the packet data */
            offset += 2 + rxIdSize; /* AVCTP start frame is transLabel(1Octet) NumOfPacket(1Octet) ProfileId(2Octet)*/
            break;

        case AVC_PACKET_TYPE_CONTINUE:
            /* Continue Packet */
            rxId = chnl->rxId;
            if (chnl->rxPacketsLeft == 0)
            {
                /* Received too many packets */
                chnl->rxState = AVC_TRANS_STATE_FIRST;
                event = AVC_EVENT_RX_BUFFER_OVERRUN;
                goto call_back;
            }
            chnl->rxPacketsLeft--;

            /* Set pointer to the packet data */
            offset++; /* AVCTP continue frame is transLabel(1Octet)  */
            break;

        case AVC_PACKET_TYPE_END:
            /* Last Packet */
            chnl->rxState = AVC_TRANS_STATE_FIRST;
            rxId = chnl->rxId;
            chnl->rxPacketsLeft--;
            if (chnl->rxPacketsLeft != 0)
            {
                OS_Report("[AVC] ptype:end but packetLeft:%d is not zero", chnl->rxPacketsLeft);
                /* Received too few packets */
                event = AVC_EVENT_RX_BUFFER_UNDERRUN;
                // goto call_back; // for avctp controller pts
            }

            /* Set pointer to the packet data */
            offset++; /* AVCTP  end frame is transLabel(1Octet)  */
            break;

        default:
            /* Unknown packet type */
            chnl->rxState = AVC_TRANS_STATE_FIRST;
            event = AVC_EVENT_RX_UNKNOWN_PACKET;
            goto call_back;
    }

    /* Call the Channel Manager callback */
    if (messageType == AVC_MSG_TYPE_COMMAND)
    {
        /* Receiving a command */
        chnl->acpTransId = transId;
    }
    else if ((messageType == AVC_MSG_TYPE_ACCEPT) || (messageType == AVC_MSG_TYPE_REJECT))
    {
        /* Receiving a response */
        if (chnl->intTransId == transId)
        {
            if (!chnl->resetTimer && ((packetType == AVC_PACKET_TYPE_SINGLE) || (packetType == AVC_PACKET_TYPE_END)))
            {
                /* Transaction ID matches */
                int tmpTransId = 0;
                chnl->intTransId++;
                tmpTransId = chnl->intTransId;
                chnl->intTransId = tmpTransId % 16;
                //chnl->intTransId = ++chnl->intTransId % 16;
            }
        }
#ifndef __AVRCP_REMOVE_LIMITATION__
        else
        {
            chnl->rxState = AVC_TRANS_STATE_FIRST;
            event = AVC_EVENT_RX_BAD_TRANS_ID;
            goto call_back;
        }
#else
		// remove the limitation
		if (!chnl->resetTimer && ((packetType == AVC_PACKET_TYPE_SINGLE) || (packetType == AVC_PACKET_TYPE_END))){
		}else{
			int tmpTransId = 0;
			chnl->intTransId++;
      tmpTransId = chnl->intTransId;
      chnl->intTransId = tmpTransId % 16;
      //chnl->intTransId = ++chnl->intTransId % 16;
		}
#endif

        if (messageType == AVC_MSG_TYPE_ACCEPT)
        {
            /* Accept Message */
            if (chnl->rxPacketsLeft == 0)
            {
                /* Response Received */
                if (chnl->txTimer.context)
                {
                    if (chnl->resetTimer)
                    {
                        OS_Report("[AVC][TIMER] StartTimer 0x%x", &chnl->txTimer);
                        chnl->txTimer.func = AvcTxTimeout;
                        chnl->txTimer.context = chnl;
                        //1 EVM_StartTimer(&chnl->txTimer, chnl->txTimeout);
                    }
                    else
                    {
                        OS_Report("[AVC][TIMER] CancelTimer 0x%x", &chnl->txTimer);
                        chnl->txTimer.context = 0;
                        //1 EVM_CancelTimer(&chnl->txTimer);
                    }
                }
            }
        }
        else if (messageType == AVC_MSG_TYPE_REJECT)
        {
            /* Reject Message */
            if (chnl->rxPacketsLeft == 0)
            {
                /* Response Received */
                if (chnl->txTimer.context)
                {
                    OS_Report("[AVC][TIMER] CancelTimer 0x%x", &chnl->txTimer);
                    chnl->txTimer.context = 0;
                    //1 EVM_CancelTimer(&chnl->txTimer);
                }
            }
        }
        else
        {
            /* Unknown message type */
            chnl->rxState = AVC_TRANS_STATE_FIRST;
            event = AVC_EVENT_RX_UNKNOWN_MESSAGE;
        }
    }
    else
    {
        /* Unknown message type */
        chnl->acpTransId = transId;
        chnl->rxState = AVC_TRANS_STATE_FIRST;
        /* event = AVC_EVENT_RX_UNKNOWN_MESSAGE; */
        event = AVC_EVENT_RX_IND;
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
 *  AvcSendPacket
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
static BtStatus AvcSendPacket(AvcChannel *chnl)
{
    BtStatus status;
    U8 headerLen;
    U8 transId;
    U8 *hdrPos;
    U16 l2cap_mtu = 0;

    l2cap_mtu = ((L2CAP_GetTxMtu((chnl)->l2ChannelId) < 48) ? (48) : (L2CAP_GetTxMtu((chnl)->l2ChannelId)));

    if ((chnl->curAvPacket->msgType == AVC_MSG_TYPE_COMMAND) && (chnl->curAvPacket->txId != 0))
    {
        transId = chnl->intTransId; // Assign the TransId 
    }
    else
    {
        transId = chnl->acpTransId; // Last received cmd's TransId
    }

    if (chnl->txState == AVC_TRANS_STATE_FIRST)
    {
        /* Start State */
        if (chnl->txPacketsLeft > 1)
        {
            /* Start Packet */
            hdrPos = chnl->packet.header + BT_PACKET_HEADER_LEN;

            if (chnl->curAvPacket->useMsgHdr == TRUE)
            {
                hdrPos -= 3; /* AVRCP spec. AV/C frame format. Include ctype, subunit_type, subunit_id fields */
                OS_MemCopy(hdrPos, chnl->curAvPacket->msgHdr, 3);
            }

            if (chnl->curAvPacket->txIdSize == 2)
            {
                hdrPos -= 2; /* AVCTP txId is profile's id. it always is 0x110E*/
                StoreBE16(hdrPos, chnl->curAvPacket->txId);
            }
            else
            {
                hdrPos--; /* AVDTP */
                *hdrPos = (U8) chnl->curAvPacket->txId;
            }

            hdrPos--; /* AVCTP spec. fragment packets. number of packets */
            *hdrPos = chnl->txPacketsLeft;

            hdrPos--; /* AVCTP spec. packet format octet 0: Transtion label */
            *hdrPos = transId << 4;
            *hdrPos += (U8) (AVC_PACKET_TYPE_START) << 2;
            *hdrPos += chnl->curAvPacket->msgType;

            headerLen = chnl->packet.header + BT_PACKET_HEADER_LEN - hdrPos;
        }
        else
        {
            /* Single Packet */
            hdrPos = chnl->packet.header + BT_PACKET_HEADER_LEN;

            if (chnl->curAvPacket->useMsgHdr == TRUE) /* AV13 only AVRCP set this to TRUE */
            {
                hdrPos -= 3; /* AVRCP spec. AV/C frame header format. Include ctype, subunit_type, subunit_id fields */
                OS_MemCopy(hdrPos, chnl->curAvPacket->msgHdr, 3);
#ifdef MTK_AVRCP_DEBUG_TRACE
                OS_Report("[AVCTP] msgHdr:0x%x 0x%x 0x%x", chnl->curAvPacket->msgHdr[0], chnl->curAvPacket->msgHdr[1], chnl->curAvPacket->msgHdr[2]);
#endif
            }

            if (chnl->curAvPacket->txIdSize == 2)  /* AVCTP spec. Not-Fragment packet. PID */
            {
                hdrPos -= 2; /* AVCTP txId is profile's id 0x110E */
                StoreBE16(hdrPos, chnl->curAvPacket->txId); 
            }
            else
            {
                hdrPos--; /* AVDTP txId is signal event id ex.AVDTP_SIG_SET_CONFIG */
                *hdrPos = (U8) chnl->curAvPacket->txId;
            }

            hdrPos--;  /* AVCTP spec. packet format octet 0: Transtion label */
            *hdrPos = transId << 4;
            *hdrPos += (U8) (AVC_PACKET_TYPE_SINGLE) << 2;
            *hdrPos += chnl->curAvPacket->msgType;

            headerLen = chnl->packet.header + BT_PACKET_HEADER_LEN - hdrPos;
        }
    }
    else
    {
        /* Continue State - only 1Byte header (TransId value) */
        hdrPos = chnl->packet.header + BT_PACKET_HEADER_LEN;

        hdrPos--;
        *hdrPos = transId << 4;
        if (chnl->txPacketsLeft > 1)
        {
            /* Continue Packet */
            *hdrPos += (U8) (AVC_PACKET_TYPE_CONTINUE) << 2;
        }
        else
        {
            /* End Packet */
            *hdrPos += (U8) (AVC_PACKET_TYPE_END) << 2;
        }

        *hdrPos += chnl->curAvPacket->msgType;

        // hdrPos is the beginning of REAL data. header is able to fix max BT_PACKET_HEADER_LEN data
        headerLen = chnl->packet.header + BT_PACKET_HEADER_LEN - hdrPos;
    }

    /* Setup the packet - L2CAP will send data in chnl->packet.data (packet.datelen) & chnl->packet.header (packet.headerlen) */
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

#ifdef MTK_AVRCP_DEBUG_TRACE
    // dump l2cap data
    {
        char datamsg[256], tempdata[64];
        unsigned char i;
        sprintf(datamsg, "[AVCTP] headerdata(%d): ", chnl->packet.headerLen);
        for( i = 0; i< chnl->packet.headerLen && i< 10; i++){
            sprintf(tempdata, "%d:0x%x ", i, hdrPos[i]);
			if( strlen(tempdata) + strlen(datamsg) < sizeof(datamsg) ){
	            strcat(datamsg, tempdata);
			}
        }
        OS_Report(datamsg);
		
        sprintf(datamsg, "[AVCTP] rawdata(%d): ", chnl->packet.dataLen);
        for( i = 0; i< chnl->packet.dataLen&& i< 10; i++){
            sprintf(tempdata, "%d:0x%x ", i, chnl->packet.data[i]);
			if( strlen(tempdata) + strlen(datamsg) < sizeof(datamsg) ){
                strcat(datamsg, tempdata);
			}
        }
        OS_Report(datamsg);
    }
#endif	

    OS_Report("[AVC] L2CAP_SendData channelId:%d packet.headerLen:%d packet.dataLen:%d", chnl->l2ChannelId, chnl->packet.headerLen, chnl->packet.dataLen);
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
 *  AvcTxTimeout
 * DESCRIPTION
 *  Timeout handler for commands.
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvcTxTimeout(EvmTimer *Timer)
{
    AvcChannel *chnl = (AvcChannel*) Timer->context;
    AvcCallbackParms parms;

    chnl->txTimer.context = 0;
    chnl->txState = AVC_TRANS_STATE_FIRST;
    parms.event = AVC_EVENT_TX_TIMEOUT;
    parms.status = BT_STATUS_SUCCESS;
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVC_TX_TIMEOUT,HCC(aclTxPacketsLeft));
    chnl->txCmdTimeout = TRUE;
    chnl->callback(chnl, &parms); /* Pass to avctp callback and disconnect the transport*/
}

