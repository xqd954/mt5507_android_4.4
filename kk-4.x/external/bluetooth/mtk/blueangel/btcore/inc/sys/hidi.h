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

#ifndef __HIDI_H
#define __HIDI_H
/*****************************************************************************
 *
 * File:
 *     $Workfile:hidi.h$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: Private types, defines, and prototypes for use by the HID layer.
 * 
 * Created:     August 7, 2002
 *
 * Copyright 2002-2004 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems,
 * Inc.  This work contains confidential and proprietary information of
 * Extended Systems, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include <l2cap.h>
#include <sdp_adp.h>
#include <hid.h>
#include "gatt.h"

/* Define debug level, better to config by file */
#define HID_DEBUG_LEVEL_1   0    
#define HID_DEBUG_LEVEL_2   0

#ifdef __NORDIC_VOICE_COMMAND__
#define DTV_POWER_OFF_KEY	0x46
BT_BOOL bPowerOff;
#endif

/****************************************************************************
 *
 * Messages and Parameters
 *
 ****************************************************************************/

/* Transaction Message Types */
typedef U8 HidMessageType;

#define HID_MSG_HANDSHAKE     0x00
#define HID_MSG_CONTROL       0x10
//Add by stanley:2007-0607
#define HID_UNSUPPORTED_TRANS_TYPE_20   0x20
//Add by stanley:2007-0607
#define HID_UNSUPPORTED_TRANS_TYPE_30   0x30
#define HID_MSG_GET_REPORT    0x40
#define HID_MSG_SET_REPORT    0x50
#define HID_MSG_GET_PROTOCOL  0x60
#define HID_MSG_SET_PROTOCOL  0x70
#define HID_MSG_GET_IDLE_RATE 0x80
#define HID_MSG_SET_IDLE_RATE 0x90
#define HID_MSG_DATA          0xA0
#define HID_MSG_DATC          0xB0

//Add by stanley:2007-0607: C0 ~ F0
#define HID_UNSUPPORTED_TRANS_TYPE_C0   0xC0
#define HID_UNSUPPORTED_TRANS_TYPE_D0   0xD0    
#define HID_UNSUPPORTED_TRANS_TYPE_E0   0xE0
#define HID_UNSUPPORTED_TRANS_TYPE_F0   0xF0

/* GET_ and SET_ REPORT parameters */
#define HID_REPORT_2_BYTE_FLAG          0x04
#define HID_REPORT_TYPE_INPUT           0x01
#define HID_REPORT_TYPE_OUTPUT          0x02
#define HID_REPORT_TYPE_FEATURE         0x03

/* GET_ and SET_ PROTOCOL parameters */
#define HID_PROTOCOL_TYPE_BOOT          0x00
#define HID_PROTOCOL_TYPE_REPORT        0x01

/* GET_ and SET_ IDLE parameters */
#define HID_IDLE_RATE_INFINITE          0x00

/* DATA and DATC parameters */
#define HID_DATA_OTHER                  0x00
#define HID_DATA_INPUT                  0x01
#define HID_DATA_OUTPUT                 0x02
#define HID_DATA_FEATURE                0x03

/****************************************************************************
 *
 * State Machine and Context Defnintions
 *
 ****************************************************************************/

/* HID States */
#define HID_STATE_CLOSED           0
#define HID_STATE_OPEN             1
#define HID_STATE_CONN_PENDING     2
#define HID_STATE_DISC_PENDING     3

/* HID Channel Flags  */
#define CHNL_FLAG_INITIATOR        0x01
#define CHNL_FLAG_TERMINATOR       0x02
#define CHNL_FLAG_TRANS_PENDING    0x04
#define CHNL_FLAG_TRANS_INCOMING   0x08
#define CHNL_FLAG_INTR_PENDING     0x10
#define CHNL_FLAG_SERVICE_QUERY        0x20

/* Transaction Flags */
#define TRANS_FLAG_START           0x01
#define TRANS_FLAG_ERROR           0x02
#define TRANS_FLAG_SENT            0x04

/* Interrupt Flags */
#define INTR_FLAG_START            0x01
#define INTR_FLAG_ERROR            0x02
#define INTR_FLAG_SENT             0x04

/* Prototype for state functions */                    
typedef void (*HidState)(HidChannel *Channel, L2capChannelId Cid, L2capCallbackParms *Parms);
                                 
/* Search types */
#define HID_FIND_CTRL_CID  1
#define HID_FIND_INTR_CID  2

/* BLE HID Scan Parameter */
#define HID_LE_FAST_CONN_INTERVAL_MIN       0x0a /* 12.5ms: 0xa * 1.25ms*/
#define HID_LE_FAST_CONN_INTERVAL_MAX       0x0a /* 12.5ms: 0xa * 1.25ms*/
#define HID_LE_FAST_CONN_LATENCY        0x0

#define HID_LE_NORMAL_CONN_INTERVAL_MIN       0x0a /* 12.5ms: 0xa * 1.25ms*/
#define HID_LE_NORMAL_CONN_INTERVAL_MAX       0x0a /* 12.5ms: 0xa * 1.25ms*/
#define HID_LE_NORMAL_CONN_LATENCY      0x64



#define hid_malloc(size)		get_ctrl_buffer(size)
#define hid_free(p)				do{if(p){free_ctrl_buffer(p); p = NULL;}}while(0)		


/****************************************************************************
 *
 * Function Prototypes
 *
 ****************************************************************************/

/* HID callback functions */
void HidL2Callback(L2capChannelId Cid, L2capCallbackParms *Parms);
void HidCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status);

/* HID states */
void HidStateClosed(HidChannel *Channel, L2capChannelId Cid, L2capCallbackParms *Parms);
void HidStateOpen(HidChannel *Channel, L2capChannelId Cid, L2capCallbackParms *Parms);
void HidStateConnPending(HidChannel *Channel, L2capChannelId Cid, L2capCallbackParms *Parms);
void HidStateDiscPending(HidChannel *Channel, L2capChannelId Cid, L2capCallbackParms *Parms);

/* Utility functions */
void HidAppCallback(HidChannel *Channel, U8 Event, U32 Status, void *ptr, U16 len);
void HidClearConnection(HidChannel *Channel, U16 Status, U16 reason);
HidChannel * HidFindChannelByCid(L2capChannelId Cid, BtRemoteDevice *RemDev);
HidChannel *HidFindChannelByLink(BtRemoteDevice *RemDev);
HidChannel *HidFindChannelByAddr(BD_ADDR *bdAddr);
HidChannel * HidFindUnusedChannel(U8 searchType, BtRemoteDevice *RemDev);
BT_BOOL HidCheckValidConnect(U8 searchType, L2capChannelId Cid, BtRemoteDevice *RemDev);
HidChannel * HidNewChannel(void);
void HidFreeChannel(HidChannel * channel);
BtStatus HidSendTransPacket(HidChannel *Channel, HidTransaction *Trans, BtPacket *packet);
void HidCompleteTransaction(HidChannel *Channel, HidTransaction *Trans, BtStatus status);
BtStatus HidSendNextTransaction(HidChannel *Channel);
BtStatus HidSendIntrPacket(HidChannel *Channel, HidInterrupt *Intr, BtPacket *packet);
void HidCompleteInterrupt(HidChannel *Channel, HidInterrupt *Intr, BtStatus status);
BtStatus HidSendNextInterrupt(HidChannel *Channel);
void HidShowBytes(void *s, int n);


/* Channel parsers */
void HidInterruptChannel(HidChannel *Channel, L2capChannelId Cid, L2capCallbackParms *Parms);
void HidControlChannel(HidChannel *Channel, L2capChannelId Cid, L2capCallbackParms *Parms);


BtStatus HidStartServiceQuery(HidChannel *Channel);
void HidSdpCallback(HidChannel *Channel, BtStatus Status);

#ifdef __BT_4_0_BLE__
/* For BLE HID */
BtStatus HidStartGattServiceQuery(HidChannel *channel);
void HidGattEventCallback(void *handler, BtGattEvent *event);
void HidBondingCallback(U8 type, U8 *bd_addr);
BtStatus HidSendLeTransPacket(HidChannel *Channel, HidTransaction *Trans);
BtStatus HidSendLeIntrPacket(HidChannel *Channel, HidInterrupt *Intr);
#endif

#endif /* __HIDI_H */


