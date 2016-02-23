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
 *     $Workfile:hfi.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains internal definitions for the Hands-free SDK.
 *             
 * Created:     February 11, 2005
 *
 * Copyright 2001-2005 Extended Systems, Inc.

 * Portions copyright 2005 iAnywhere Solutions, Inc.

 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __HFI_H_
#define __HFI_H_

#include "hf.h"

/* Channel States */
#define HF_STATE_CLOSED         0
#define HF_STATE_CONN_PENDING   1
#define HF_STATE_CONN_INCOMING  2
#define HF_STATE_NEGOTIATE      3
#define HF_STATE_OPEN           4

/* Link Flags */
#define HF_LINK_ACL              0x01
#define HF_LINK_HANDSFREE        0x08
#define HF_LINK_REMOVE_HF        0x10
#define HF_LINK_HS_CONNECTING    0x20
#define HF_LINK_HS_DISCONNECTING 0x40
#define HF_LINK_REM_DISC         0x80

/* Internal State Machine Events */
#define EVENT_OPEN_IND         0x80
#define EVENT_OPEN             0x81
#define EVENT_AT_DATA          0x82
#define EVENT_CLOSED           0x83
#define EVENT_COMMAND_TIMEOUT  0x84
#define EVENT_RF_PACKET_SENT   0x85
#define EVENT_COMMAND_COMPLETE 0x89
#define EVENT_POLL_COMPLETE    0x9A
#define EVENT_RING_TIMEOUT     0x9B

/* Channel Flags */
#define CHANNEL_FLAG_OUTGOING            0x0001
#define CHANNEL_FLAG_TEST_INDICATORS     0x0002
#define CHANNEL_FLAG_READ_INDICATORS     0x0004
#define CHANNEL_FLAG_EVENT_REPORT        0x0008
#define CHANNEL_FLAG_TEST_HOLD           0x0010
#define CHANNEL_FLAG_NEG_DONE            0x0040
#define CHANNEL_FLAG_TX_IN_PROGRESS      0x0080
#define CHANNEL_FLAG_COPS_SET            0x0200
#define CHANNEL_FLAG_SNIFFING            0x0400

/* SDP Query States */
#define SDP_QUERY_FLAG_PROTOCOL  0x01
#define SDP_QUERY_FLAG_PROFILE   0x02
#define SDP_QUERY_FLAG_NETWORK   0x04
#define SDP_QUERY_FLAG_FEATURES  0x08
#define SDP_QUERY_FLAG_HF        0x10

/* The following twelve defines are event identifiers related to 
 * indicator values used with the CIND and CIEV messages.  The first 
 * nine are specific to CIEV messages, and must match the index values 
 * for the HfIndMsg array so that lookups in the indMap can be performed.
 */ 
#define AT_IND_CALL            0 /* Associated with a CIEV message */ 
#define AT_IND_SIGNAL          1
#define AT_IND_SERVICE         2
#define AT_IND_ROAM            3
#define AT_IND_SMS             4
#define AT_IND_BATTCHG         5
#define AT_IND_SETUP           6 /* Associated with a CIEV message */ 
#define AT_IND_SETUP2          7 /* Associated with a CIEV message */ 
#define AT_IND_CALLHELD        8 /* Associated with a CIEV message */ 

/* The next three defines are used in conjunction with with CIND 
/* messages.  Note that since we only poll with CLCC messages, these 
/* events only occur at initialization time.  Previous versions of the 
/* code polled with CIND messages if CLCC messages were not supported.  
/* However, since many of the AGWs did not consistently report CIEV and 
/* CIND information, it was decided to drop the CIND polling mechanism 
/* and solely rely on what was reported by the CIEV messages.
 */ 
#define AT_IND_POLL_CALL     100 /* Associated with a CIND message */ 
#define AT_IND_POLL_SETUP    101 /* Associated with a CIND message */ 
#define AT_IND_POLL_CALLHELD 102 /* Associated with a CIND message */ 

#define AT_TIMEOUT 0xFFFE

/* Unknown AT data received */
#define AT_UNKNOWN 0xFFFF

/* State machine function prototype */                     
typedef void (*HfState)(HfChannel *Channel, U8 Event, AtResults *Parms);

/* Indicator variables */
extern const char *HfIndMsg[];

/* General utility functions */
void HfCloseChannel(HfChannel *Channel);
BtStatus HfParseRfcommData(HfChannel *channel, RfCallbackParms *parms, U16 *offset, AtResults *atParms);
void HfAppCallback(HfChannel *Channel, HfEvent Event, BtStatus Status, void *data);
void HfPhonebookCallback(HfChannel *Channel, AtResults *Parms);
void HfIndicatorCallback(HfChannel *Channel, AtResults * Parms);
void HfEventCallback(HfChannel *Channel, AtResults *Parms);
HfChannel * HfFindChannel(BtRemoteDevice *remDev);
RfChannel *HfAllocRfChannel(void);
void HfFreeRfChannel(RfChannel *rfChannel);
HfChannel * HfGetClosedChannel(void);

/* SDP utility functions */
BtStatus HfRegisterSdpServices(HfChannel *Channel);
BtStatus HfDeregisterSdpServices(void);
BtStatus HfStartServiceQuery(HfChannel *Channel, SdpQueryMode mode);
BtStatus HfVerifySdpQueryRsp(HfChannel *Channel, SdpQueryToken *token);

/* Handsfree functions */
void HfRfCallback(RfChannel *Channel, RfCallbackParms *Parms);
void HfInitStateMachine(void);

/* At send functions */
BtStatus AtSendCommand(HfChannel *Channel, AtCommands *AtCommand);
BtStatus AtSendRaw(HfChannel *Channel, const char *AtString);
BtStatus AtSendRfPacket(HfChannel *Channel, BtPacket *Packet);
void AtParseResultCode(HfChannel *Channel, U8 *input, 
                       U16 inputLen, AtResults *results);

#endif /* __HFI_H_ */

