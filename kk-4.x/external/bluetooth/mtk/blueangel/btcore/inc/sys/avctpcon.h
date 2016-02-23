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
 *     $Workfile:avctpcon.h$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains the header file for the L2CAP/ACL 
 *              connection state machine for AVCTP connections.
 *             
 * Created:     Mar 31, 2004
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

#ifndef __AVCTPCON_H_
#define __AVCTPCON_H_


#include "l2cap_adp.h"

/* Connection Flags */
#define AVCTP_FLAG_LINK_ACL       0x01

/* Conn Events */
#define AVCTP_CONN_EVENT_CONNECT     1
#define AVCTP_CONN_EVENT_CONNECT_IND 2
#define AVCTP_CONN_EVENT_DISCONNECT  3
#define AVCTP_CONN_EVENT_DATA_SENT   4
#define AVCTP_CONN_EVENT_DATA_IND    5

/* Connection States */
#define AVCTP_CONN_NUM_STATES     6

#define AVCTP_STATE_DISCONNECTED   0
#define AVCTP_STATE_CONN_PENDING   1
#define AVCTP_STATE_CONN_INCOMING  2
#define AVCTP_STATE_DISC_PENDING   3
#define AVCTP_STATE_DISC_INCOMING  4
#define AVCTP_STATE_CONNECTED      5

/* Forward references */
typedef struct _AvctpConnCallbackParms AvctpConnCallbackParms;
typedef struct _AvctpConn AvctpConn;

/* State machine function prototype */
typedef void (*AvctpConnState) (AvctpConn *Conn, L2capCallbackParms *Parms);

/* Conn callback function */
typedef void (*AvctpConnCallback) (AvctpConn *Conn, AvctpConnCallbackParms *Parms);

/* Conn calback parameters */
struct _AvctpConnCallbackParms
{
    U8 event;
    U16 dataLen;
    union
    {
        BtRemoteDevice *remDev;
        U8 *data;
    } ptrs;
};

/* Connection State */
struct _AvctpConn
{
    L2capChannelId l2ChannelId;
    U8 state;
    AvctpConnCallback callback;
    BtRemoteDevice *remDev;
};

/* State machine functions */
void AvctpConnInitStateMachine(void);
void AvctpL2Callback(L2capChannelId l2ChannelId, L2capCallbackParms *parms);

/* Function Prototypes */
void AvctpInitConn(AvctpConn *conn, AvctpConnCallback callback);
BtStatus AvctpConnect(AvctpConn *conn, BtRemoteDevice *RemDev, U16 psmValue);
BtStatus AvctpConnectRsp(AvctpConn *Conn, BOOL Accept, U16 psmValue);
BtStatus AvctpDisconnect(AvctpConn *conn);
BOOL AvctpIsConnected(AvctpConn *conn);

#endif /* __AVCTPCON_H_ */ 

