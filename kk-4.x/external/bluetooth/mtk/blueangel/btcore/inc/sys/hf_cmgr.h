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
 *     $Workfile:hf_cmgr.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains the call state manager for the Hands-free SDK.
 *             
 * Created:     February 11, 2005
 *
 * Copyright 2000-2005 Extended Systems, Inc.

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

#ifndef __HF_CALLMGR_H_
#define __HF_CALLMGR_H_

#if HF_USE_CALL_MANAGER == XA_ENABLED

#include "sys/hfi.h"

/* Call state indicator values */ 
#define CALL_STATUS_NONE    0
#define CALL_STATUS_ACTIVE  1

/* Call setup indicator values */ 
#define CALL_SETUP_NONE     0
#define CALL_SETUP_INCOMING 1
#define CALL_SETUP_OUTGOING 2
#define CALL_SETUP_OUTALERT 3

/* Call manager flags */
#define CALLMGR_FLAG_HANGUP          0x0001
#define CALLMGR_FLAG_ACCEPT          0x0002
#define CALLMGR_FLAG_CALL_OUTGOING   0x0004
#define CALLMGR_FLAG_RELEASE         0x0008
#define CALLMGR_FLAG_HOLD            0x0010
#define CALLMGR_FLAG_REJECT          0x0020
#define CALLMGR_FLAG_REPLACE         0x0040
#define CALLMGR_FLAG_ACTIVATE        0x0080
#define CALLMGR_FLAG_CONFERENCE      0x0100

/* Polling flags */
#define CALLMGR_POLL_LIST_CALLS      0x01
#define CALLMGR_POLL_ATTEMPTED       0x02
#define CALLMGR_POLL_RESP_RECVD_1    0x08
#define CALLMGR_POLL_RESP_RECVD_2    0x10
#define CALLMGR_POLL_STARTED         0x40
#define CALLMGR_POLL_RELIABLE        0x80

/*---------------------------------------------------------------------------
 * Event Group type
 *
 *     Distinguishes groups of events handled by the call manager.
 */
typedef U16 EventGroupType;

/** Used for CIEV indications.
 */
#define EG_AT_INTERNAL      0x0000

/** Used for CIND polling indications.
 */
#define EG_AT_INTERNAL_POLL 0x1000

/** Used for service connection events.
 */
#define EG_HFEVENT          0x2000

/** Used for AT Command Processor events.
 */
#define EG_AT_PROCESSOR     0x3000

/** Used for Internal State Machine Processor events.
 */
#define EG_STATE_MACHINE    0x4000

/** Used for AT Command Hands Free events.
 */
#define EG_AT_HANDSFREE     0x5000

/* End of EventGroupType */

/* Exported call manager functions */ 
BOOL CallManager(HfChannel *Channel, 
                 EventGroupType EventGroup, 
                 U16 Event, 
                 U32 Parms);
BtStatus CallMgrAnswerCall(HfChannel *Channel, HfCommand *Command);
BtStatus CallMgrHangupCall(HfChannel *Channel, HfCommand *Command);
BtStatus CallMgrInitiateCall(HfChannel *Channel, HfCommand *Command);
BOOL CallMgrIsIncomingCall(HfChannel *Channel);
BOOL CallMgrIsActiveCall(HfChannel *Channel);
BtStatus CallMgrCallHold(HfChannel *Channel, HfHoldAction Action, U8 Line, HfCommand *Command);

#endif
#endif /* __HF_CALLMGR_H_ */
