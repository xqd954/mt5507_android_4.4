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
 *     $Workfile:avsigmgr.h$ for XTNDAccess Blue SDK, Version 1.4
 *     $Revision: #1 $
 *
 * Description: This file contains the API for the Signal Manager.
 *             
 * Created:     Mar 23, 2004
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

#ifndef __AVSIGMGR_H_
#define __AVSIGMGR_H_

#include "avdtp.h"

/* Signal Commands */
#define AVDTP_SIG_DISCOVER          0x01
#define AVDTP_SIG_GET_CAPABILITIES  0x02
#define AVDTP_SIG_SET_CONFIG        0x03
#define AVDTP_SIG_GET_CONFIG        0x04
#define AVDTP_SIG_RECONFIG          0x05
#define AVDTP_SIG_OPEN              0x06
#define AVDTP_SIG_START             0x07
#define AVDTP_SIG_CLOSE             0x08
#define AVDTP_SIG_SUSPEND           0x09
#define AVDTP_SIG_ABORT             0x0A
#define AVDTP_SIG_SECURITY_CTRL     0x0B

/* Constants */
#define MAX_NUM_STREAMS             63

/* Sig Manager Transmit States */
#define AVTP_SIG_MGR_TXSTATE_IDLE            0
#define AVTP_SIG_MGR_TXSTATE_WAIT_RSP        1

/* Sig Manager Receive States */
#define AVTP_SIG_MGR_RXSTATE_BEGIN           0
#define AVTP_SIG_MGR_RXSTATE_CONTINUE        1
#define AVTP_SIG_MGR_RXSTATE_IGNORE          2

void AvdtpSigInit(AvdtpChannel *Chnl);

BtStatus AvdtpSigStreamSendSimpleCmd(AvdtpChannel *Chnl, U8 SigCmd, AvdtpStream *Stream, AvdtpStreamId StrmId);

BtStatus AvdtpSigConnect(AvdtpChannel *Chnl, BtRemoteDevice *RemDev);

#define AvdtpSigConnect(s, r) AvdtpConnect(&(s)->conn, r);

BtStatus AvdtpSigConnectRsp(AvdtpChannel *Chnl, BOOL Accept);

#define AvdtpSigConnectRsp(s, a) AvdtpConnectRsp(&(s)->conn, a);

BtStatus AvdtpSigDisconnect(AvdtpChannel *Chnl);

#define AvdtpSigDisconnect(s) AvdtpDisconnect(&(s)->conn)

BOOL AvdtpSigIsConnected(AvdtpChannel *Chnl);

#define AvdtpSigIsConnected(s) AvdtpIsConnected(&(s)->conn)

BtStatus AvdtpSigStreamDiscover(AvdtpChannel *Chnl);

#define AvdtpSigStreamDiscover(s) AvdtpSigStreamSendSimpleCmd(s,    \
                                                AVDTP_SIG_DISCOVER, \
                                                0, 0)

BtStatus AvdtpSigStreamGetCapabilities(AvdtpChannel *Chnl, AvdtpStreamId StrmId);

#define AvdtpSigStreamGetCapabilities(s, strmId) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_GET_CAPABILITIES,     \
                                                0,                              \
                                                strmId)

BtStatus AvdtpSigStreamSetConfig(AvdtpChannel *Chnl, AvdtpStream *Stream, AvdtpCodec *Codec);

BtStatus AvdtpSigStreamGetConfig(AvdtpChannel *Chnl, AvdtpStream *Stream);

#define AvdtpSigStreamGetConfig(s, strm) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_GET_CONFIG,   \
                                                strm,                   \
                                                strm->remStrmId)

BtStatus AvdtpSigStreamReconfig(AvdtpChannel *Chnl, AvdtpStream *Stream, AvdtpCodec *Codec);

BtStatus AvdtpSigStreamReconfigRsp(
            U8 rsptype,
            AvdtpChannel *Chnl,
            AvdtpStream *Stream,
            AvdtpError Error,
            AvdtpCapabilityType Type);

BtStatus AvdtpSigStreamOpen(AvdtpChannel *Chnl, AvdtpStream *Stream);

#define AvdtpSigStreamOpen(s, strm) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_OPEN,    \
                                                strm,              \
                                                strm->remStrmId)

BtStatus AvdtpSigStreamStart(AvdtpChannel *Chnl, AvdtpStream *Stream);

#define AvdtpSigStreamStart(s, strm) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_START,    \
                                                strm,               \
                                                strm->remStrmId)

BtStatus AvdtpSigStreamStartRsp(U8 rsptype, AvdtpChannel *Chnl, AvdtpStream *Stream, AvdtpError Error);

BtStatus AvdtpSigStreamClose(AvdtpChannel *Chnl, AvdtpStream *Stream);

#define AvdtpSigStreamClose(s, strm) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_CLOSE,    \
                                                strm,               \
                                                strm->remStrmId)

BtStatus AvdtpSigStreamSuspend(AvdtpChannel *Chnl, AvdtpStream *Stream);

#define AvdtpSigStreamSuspend(s, strm) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_SUSPEND,    \
                                                strm,                 \
                                                strm->remStrmId)

BtStatus AvdtpSigStreamAbort(AvdtpChannel *Chnl, AvdtpStream *Stream);

#define AvdtpSigStreamAbort(s, strm) AvdtpSigStreamSendSimpleCmd(s, \
                                                AVDTP_SIG_ABORT,    \
                                                strm,               \
                                                strm->remStrmId)

BtStatus AvdtpSigStreamSecurityCtrl(
            AvdtpChannel *Chnl,
            AvdtpStream *Stream,
            U16 Len,
            U8 *Data,
            U8 msgType,
            AvdtpError Error);

void AvdtpStrmConnCallback(AvdtpConn *Conn, AvdtpConnCallbackParms *Parms);

#endif /* __AVSIGMGR_H_ */ 

