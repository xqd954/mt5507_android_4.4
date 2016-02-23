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

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*******************************************************************************
 *
 * Filename:
 * ---------
 *   prxr.h
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file contains public API functions for the Proximity Profile.
 *
 * Author:
 * -------
 *   Daylong Chen
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#ifndef __PRXM_H
#define __PRXM_H
#include "bttypes.h"
#include "l2cap.h"   // for transport
#include "conmgr.h"  // for transport
#include "rfcomm.h"  // for transport
#include "gatt.h" // for transport

/* proximity monitor header */
typedef struct
{
	void *prxContext; /* used by PRXR */
	U8 id;
	U8 state;
} PrxMApp;


typedef struct
{
	U8 event;
	void *data;
} PrxMEvent;
/* 
 * @brief callback prototype 
 * @param prxm context
 * @param param parameter
 */
typedef void (*BTMTKPRXMCALLBACK)(PrxMEvent *event, void *apdata);

typedef struct _PrxMTransport {
	L2capPsm psm; /* Used by ATT module */
	L2capChannelId id; /* use for connection */
	CmgrHandler handler; // used by CMGR module
	BD_ADDR bdaddr; // use for connection remote's address
	U16 psmPort;
	BtGattHandler hdl; /* GATT handler */
	U8 trantype; // always 0x5a Assertion check
#ifdef PRX_TEST	
	RfChannel rfchnl;
#endif
	U16 remotePort; // remoet rf com port found by query
    BtGattOp gattop;	
	SdpQueryToken   sdpQueryToken; // sdp search
	U8 regFlag; // use to record the callback register
} PrxMTransport;

typedef struct _PrxMContext {
	BTMTKPRXMCALLBACK callback;
	PrxMTransport chnl; /* transport layer data */
	U8 isuse;
	U8 state; /* proximity monitor state */
	U8 linkloss;
	U8 pathloss;
	U8 addr[6]; /* remote address */
	S8 txPower; /* remote txpower */
	S8 rssi; /* local rssi value */
	void *app;
} PrxMContext;

typedef U8 PRXMState;
#define PRXMS_IDLE             0
#define PRXMS_LINK_CONNECT     1
#define PRXMS_QUERY            2
#define PRXMS_CONNECTING       3
#define PRXMS_CONNECTED        4
#define PRXMS_GETCAP           5  /* get the configure value */
#define PRXMS_DISCONNECTING    6

BtStatus RPXM_Init(PrxMApp *app);
BtStatus PRXM_Deinit(PrxMApp *app);
BtStatus PRXM_Connect(PrxMApp *app, U8 addr[6], BTMTKPRXMCALLBACK callback);
BtStatus PRXM_Disconnect(PrxMApp *app);
BtStatus RPXM_GetCapability(PrxMApp *app);
BtStatus RPXM_WritePathLossLevel(PrxMApp *app, U8 level, U8 seqid);
BtStatus RPXM_WriteLinkLossLevel(PrxMApp *app, U8 level, U8 seqid);
BtStatus RPXM_GetLocalRssi(PrxMApp *app);
S8 PRXR_GetCurrentCapabilityValue(PrxMApp *app);

BtStatus PRXM_Init(PrxMApp *app);
BtStatus PRXM_IsDisconnecting(PrxMApp *app);
S32 PRXM_GetCurrentCapabilityValue(PrxMApp *app);
BtStatus RPXM_ReadRemoteTxPower(PrxMApp *app, U8 seqid);
BtStatus RPXM_ReadRemoteLinkLoss(PrxMApp *app, U8 seqid);
void PRXM_ResetContext();

#endif
