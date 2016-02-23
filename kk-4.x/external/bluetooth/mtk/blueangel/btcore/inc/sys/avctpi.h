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
 *     $Workfile:avctpi.h$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains internal definitions for the Audio/Video
 *     Remote Control Transport Protocol (AVCTP).
 *             
 * Created:     Mar 11, 2004
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

#ifndef __AVCTPI_H_
#define __AVCTPI_H_

#include "btconfig.h"
#include "avctp.h"
#include "sec.h"

#define AVCTP_MAX_RETRANS_NUM 3

/* The BtAvctpContext structure contains the global context of AVDTP. */
typedef struct _BtAvctpContext
{
    ListEntry chnlList; /* For keeping the registered users. ex. AvctpChannel */
    AvctpConnState connState[AVCTP_CONN_NUM_STATES]; /* statemachine for AVCTP l2cap */
    L2capPsm psm; /* L2cap */
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    BtSecurityRecord secRecord;
#endif 

    /* AV13: duplicate for browse l2cap channel */
    AvctpConnState browseConnState[AVCTP_CONN_NUM_STATES]; /* statemachine for AVCTP l2cap */
    L2capPsm browse_psm; /* L2cap Browse psm*/
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    BtSecurityRecord browseSecRecord;
#endif 
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
    ListEntry rxPacketsBufferList;
    ListEntry receivedPacketsList;
    BtRxPacket rfcRxPackets[AVCTP_MAX_RETRANS_NUM]; //RF_NUM_RETRANSMISSION_WINDOW_SIZE];
    U8 l2capRawData[AVCTP_MAX_RETRANS_NUM][L2CAP_MAXIMUM_MTU+20]; //RF_NUM_RETRANSMISSION_WINDOW_SIZE][RF_MAX_FRAME_SIZE+20];
    U8 txMaxCountPerIFrame[128];
    U32 txSrejList[RF_L2CAP_MAX_SREJ_SIZE];
#endif    
} BtAvctpContext;

#if XA_CONTEXT_PTR == XA_ENABLED
extern BtAvctpContext *avctpContext;

#define AVCTP(s) (avctpContext->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern BtAvctpContext avctpContext;

#define AVCTP(s) (avctpContext.s)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

BOOL AvctpAlloc(void);
BtStatus AvctpMsgSendCommand(AvctpChannel *chnl);
void AvtpMsgInit(AvctpChannel *Chnl);

#endif /* __AVCTPI_H_ */ 

