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

#ifndef __HOST_H
#define __HOST_H

/***************************************************************************
 *
 * File:
 *     $Workfile:host.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains internal definitions for the Host
 *     Controller Interface.
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

#include "eventmgr.h"
#include "sys/hci.h"
#include "sys/rxbuff.h"
#include "btconfig.h"
#include "hcitrans.h"
#include "prescan.h"
#include "bt_feature.h"

/*--------------------------------------------------------------------------*
 * Typedefs and Data structures used internally by HCI                      *
 *--------------------------------------------------------------------------*/

/* SCO Support in HCI */
#define HCI_SCO_ENABLED ((NUM_SCO_CONNS != 0) && (BT_SCO_HCI_DATA == XA_ENABLED))

/* HCI States */
#define HCI_STATE_SHUTDOWN        0
#define HCI_STATE_INIT            1
#define HCI_STATE_DEINIT          2
#define HCI_STATE_READY           4

/* HCI Flags */
#define HCI_FLAG_DATA_QUEUED      0x01
#define HCI_FLAG_PACKETS_COMPLETE 0x02
#define HCI_FLAG_ACTIVE_BC        0x04
#define HCI_FLAG_PICONET_BC       0x08

/* HCI Transmit states */
#define HCI_TX_STATE_SEND_HEADER  0
#define HCI_TX_STATE_SEND_DATA    1
#define HCI_TX_STATE_SEND_TAIL    2
#define HCI_TX_STATE_SEND_FCS     3

/* HCI Connection Flags */
typedef U8 HciConFlags;

#define HCI_ACTIVE_CON         0x01
#define HCI_DATA_REQ           0x02
#define HCI_DATA_TYPE_ACL      0x04
#define HCI_DATA_TYPE_SCO      0x08


#define HCI_AMP_CONTROLLER_BREDR    0x00
#define HCI_AMP_CONTROLLER_WIFI     0x01

#ifndef BT_META_TX_TEST_NO
#if defined (__BT_CHIP_VERSION_1__)
#define BT_META_TX_TEST_NO          (0x03)
#else 
#define BT_META_TX_TEST_NO          (0x05)
#endif 
#endif /* BT_META_TX_TEST_NO */ 

/* HCI Connection Table */
typedef struct _HciConnection
{
    HciHandle hciHandle;
    HciConFlags conFlags;

    /* Transmit state variables */
    ListEntry btPacketList;
    BtPacket *btPacket;
    U16 remaining;
    U16 fragRemain;
    const U8 *buffPtr;
    U8 txState;
    U8 flowControl;
    /* Flow control variables */
    U16 packetsSent;
    U8 ampTxFailed;

#if HCI_HOST_FLOW_CONTROL == XA_ENABLED
    U16 packetsComplete;
#endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

} HciConnection;

typedef struct _BtHciWifiController
{
    U8 controllerId;
    U16 maxACLTxDataLength;
    U16 maxDataBlockLength;
    U16 numOfDataBlockbuffer;
    U8 ampTxPacketsSent;
}BtHciWifiController;

/* HCI Context */
typedef struct _BtHciContext
{

    /* HCI Queues */
    ListEntry rxBuffList;       /* Received HCI buffers         */
    ListEntry sentPacketList;   /* Handled HCI TX packets       */
    ListEntry commandList;      /* Commands queued for transmit */
    ListEntry deferredList;     /* Commands queued during init  */
    ListEntry sentCommandList;  /* Commands sent to the radio   */
    ListEntry hciPacketPool;    /* Avaialable HCI TX packets    */
    ListEntry metaTxDataList;
    ListEntry metaTxQueueList;
    BOOL metaMode;
    U16 metaTxCounter;
    U16 metaTxSentCounter;
    U16 metaTxRealSentCounter;
    U32 metaTxTimerCounter;
    U8 *metaTxDataBuffer;
    /* HCI Transmit and flow control variables */
    HciPacket hciPackets[HCI_NUM_PACKETS + HCI_NUM_COMMANDS];
    BtPacket metaPackets[BT_META_TX_TEST_NO];
    U16 hciPacketsLeft;
    U16 aclTxPacketLen;
    U16 aclTxPacketsLeft;
    U8 scoTxPacketLen;
    U16 scoTxPacketsLeft;
#if HCI_HOST_FLOW_CONTROL == XA_ENABLED
    U16 aclRxPacketsLeft;

#if HCI_SCO_ENABLED && (HCI_SCO_FLOW_CONTROL == XA_ENABLED)
    U16 scoRxPacketsLeft;
#endif 
    U16 maxAclTxPacketsLeft;

#endif /* HCI_HOST_FLOW_CONTROL == XA_ENABLED */

    /* Internal command packet */
    HciCommand cmd;

    /* Command flow control variable */
    U8 maxCommands;

    /* HCI connection table and state */
    HciConnection con[HCI_NUM_HANDLES];
    HciConnection ampCon[HCI_NUM_AMP_HANDLES];
    U16 activeAclCons;
    U16 txCon;

    /* Other HCI state variables */
    U8 state;
    U8 flags;
    U8 conReqs;
    U8 transportError;
    /* Received reset request from application */
    U8 reset_received;
    /* Factory mode */
    U8 FactoryMode;
#ifdef __ENABLE_SLEEP_MODE__    
    U8 hostwakeupCounter;
#endif /* __ENABLE_SLEEP_MODE__ */
    /* Dynamic broadcast handles */
    HciHandle piconetBcHandle;
    HciHandle activeBcHandle;

#if HCI_ALLOW_PRESCAN == XA_ENABLED
    PrescanContext psc;
#endif /* HCI_ALLOW_PRESCAN == XA_ENABLED */

    /* Reset delay and retry timers */
    EvmTimer resetTimer;
    EvmTimer retryTimer;
#ifdef  __ENABLE_SLEEP_MODE__    
    EvmTimer hostwakeupTimer;
#endif /* __ENABLE_SLEEP_MODE__ */
    EvmTimer wifisettingTimer;

    /* Transport driver entry points */
    TranEntry tranEntry;

    /* Radio handler entry point */
    RadioHandlerCallback radioEntry;
#ifdef __BT_3_0_HS__
    BtHciWifiController wifiController;
#endif
#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER
    U8 fmovera2dp_enabled;
#endif
} BtHciContext;

/*--------------------------------------------------------------------------*
 * Functions used internally by the driver                                  *
 *--------------------------------------------------------------------------*/

/* Internal references to HCI entry points */
void HciTransportError(void);
void HciTransportError1(U8 index);
void HciTransportReset(void);
void Hci_ReturnOneNoneSentACL(void);

void HciBufferInd(HciBuffer *buffer, BtStatus status);
void HciPacketSent(HciPacket *packet);
BtStatus HciSendCommand(U16 opCode, U8 parmLen, HciCommand *cmd, BOOL internal, BOOL insertTail);

/* HCI utility functions */
U16 HciGetTableEntry(HciHandle hciHandle);
HciHandle HciAddConnection(HciHandle hciHandle, U8 linkType);
BtStatus HciDeleteConnection(HciHandle hciHandle);
void HciSetNewBcHandle(HciHandle *handle);
U8 HCI_GetState(void);

#define HCI_GetState() (HCC(state))

/* HCI event processing functions */
void HciProcessEvent(HciBuffer *hciBuffer);

/* Prescan initialization  */
void HciInitPrescan(void);

#endif /* __HOST_H */ 

