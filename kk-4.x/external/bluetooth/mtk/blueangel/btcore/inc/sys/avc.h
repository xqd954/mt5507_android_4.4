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
 *     $Workfile:avc.h$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file copied from avtp.c/avtp.h
 *              This file is used to parsed avctp header
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

#ifndef __AVC_H_
#define __AVC_H_

#include "bttypes.h"
#include "l2cap_adp.h"

/* Maximum length of a packet (Based on the maximum AVDTP/AVCTP packet) */
#define AVC_MAX_LEN 522

/* Transmission States */
#define AVC_TRANS_STATE_FIRST      0
#define AVC_TRANS_STATE_CONTINUE   1

/* Packet Types */
#define AVC_PACKET_TYPE_SINGLE     0
#define AVC_PACKET_TYPE_START      1
#define AVC_PACKET_TYPE_CONTINUE   2
#define AVC_PACKET_TYPE_END        3

/* Message Types - AVRCP header "C/R" & "IPID" AVRCP spec table 6.2 - 2bits*/
#define AVC_MSG_TYPE_COMMAND       0
#define AVC_MSG_TYPE_RESERVED        1
#define AVC_MSG_TYPE_ACCEPT        2
#define AVC_MSG_TYPE_REJECT        3

/* Events */
#define AVC_EVENT_TX_DONE            0
#define AVC_EVENT_TX_TIMEOUT         1
#define AVC_EVENT_TX_ERROR           2
#define AVC_EVENT_RX_IND             3
#define AVC_EVENT_RX_UNKNOWN_PACKET  4
#define AVC_EVENT_RX_UNKNOWN_MESSAGE 5
#define AVC_EVENT_RX_BUFFER_UNDERRUN 6
#define AVC_EVENT_RX_BUFFER_OVERRUN  7
#define AVC_EVENT_RX_BAD_TRANS_ID    8

/* Status Codes */

/* Forward reference */
typedef struct _AvcChannel AvcChannel;
typedef struct _AvcCallbackParms AvcCallbackParms;

/* Channel Manager Callback */
typedef void (*AvcCallback) (AvcChannel *chnl, AvcCallbackParms *Parms);

/* Channel Packet */
typedef struct _AvcPacket
{
    ListEntry node;

    U8 msgType; /* packet type value */

    BOOL useMsgHdr;
    U8 msgHdr[3];

    U8 txIdSize; /* the size of ID field for avctp(2B) and avdtp(1B) frame header*/
    U16 txId; /* packet id field value*/
    U8 internal_used; /* AV13 used by AVDTP */
    U16 txDataLen;
    U8 *txData;
} AvcPacket;

/* Channel */
struct _AvcChannel
{

    U32 context;

    /* Identifier */
    U16 rxId;

    /* Transmit Packet */
    ListEntry avPacketList;
    AvcPacket *curAvPacket;

    /* Transaction ID */
    U8 intTransId; // CT role: Assign the TransId. Increase this when receiving a rsp
    U8 acpTransId; // TG role: Last received cmd's TransId

    /* Transmit State */
    U8 txState;
    U16 offset;
    U16 packetSize;
    U8 txPacketsLeft;

    /* Channel Receive State */
    U8 rxState;
    U8 rxPacketsLeft;

    /* Channel Resources */
    BtPacket packet;
    L2capChannelId l2ChannelId;

    /* Command Timeout */
    TimeT txTimeout;
    EvmTimer txTimer;
    BOOL resetTimer;
    BOOL txCmdTimeout;

    /* Channel Callback */
    AvcCallback callback;
};

/* Channel Callback Parameters */
struct _AvcCallbackParms
{

    U8 event;           /* Callback event type               */
    BtStatus status;    /* Transport status                  */
    U8 msgType;         /* Message type                      */
    U8 pktType;         /* Packet type                       */
    U16 rxId;           /* RX ID                             */

    U8 packetsLeft;     /* Number of packet still to receive */
    U16 len;            /* Length of the current data        */
    AvcPacket *packet; /* Pointer to the transmitted packet */
    U8 *data;           /* Pointer to the received data      */
    U8 transid;         /* Receive packet's transId          */
};

/* Initialize the channel structure */
void AVC_InitChannel(AvcChannel *chnl, L2capChannelId L2ChannelId, AvcCallback Callback, TimeT Timeout);

/* Deinitializes the channel structure */
void AVC_DeinitChannel(AvcChannel *chnl);

/* Start transmission of a packet */
BtStatus AVC_SendStart(AvcChannel *chnl, AvcPacket *packet);

/* Continue and check completion of a packet */
void AVC_SendContinue(AvcChannel *chnl);

/* Parse received data */
void AVC_Receive(AvcChannel *chnl, U8 *Buffer, U16 Len, U8 rxIdSize);

#endif /* __AVC_H_ */ 

