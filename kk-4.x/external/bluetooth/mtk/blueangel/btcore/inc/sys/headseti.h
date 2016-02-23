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
 *     $Workfile:headseti.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: This file contains the internal function prototypes and data
 *              structures for the headset profile
 *             
 * Created:     October 17, 2000
 *
 * Copyright 2000-2002 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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

#ifndef __HEADSETI_H_
#define __HEADSETI_H_

#include <headset.h>
#include <mesco.h>

#if BT_HSP_HEADSET == XA_ENABLED

#define HS_NUM_RFCOMM_PACKETS 3


/*---------------------------------------------------------------------------
 * Headset structure
 *
 * The Headset structure contains all of the variables used in headset.c
 *
 */
typedef struct _Headset {

    HsState         State;
    BOOL            Initialized;
    BOOL            HdcRegistered;
    BOOL            OutGoingConn;
    U8              MicVol;
    U8              SpkVol;
    U8              ckpdbuff[12];
    U8              volbuff[10];
    HsDeviceContext Hdc;           
    HsCallback      AppCallback;  
    HsCallbackInfo  Callbackinfo;
    U8              ConnectLevel;
    BtHandler       globalHandler;

    /* RFCOMM Variables */
    RfService       rfService;
    RfChannel       rfSrvChannel;
    RfChannel       rfCltChannel;
    RfChannel       *DefaultChannel;
    U8              remoteRfcServer;
    ListEntry       packetList;
    BtPacket        packet[HS_NUM_RFCOMM_PACKETS];
    U8              lastPacket;

    /* SDP Variables */
    U8              sdpProtoDescList[14];
    SdpAttribute    SdpAttribute[5];
    SdpRecord       sdprecord;
    SdpQueryToken   sdpQueryToken;
    U8              serviceSearchAttributeReq[18];

    /* SCO Variables */
    BtScoConnect         *scoConnect;

    /* Security Variables */
#if HS_SECURITY == XA_ENABLED
    U8*             pinCode;
    U8              pinLength;
    BOOL            security;
    U8              trustLevel;
#endif /* HS_SECURITY */

    /* The following fields are only valid when there is a connection */
    BtRemoteDevice  *link;
    BtHandler       handler;


    U8              parms[6];
    MeCommandToken  token[2];

} Headset;


#if XA_CONTEXT_PTR == XA_ENABLED
#define HS(s) (hs->s)
extern Headset *hs;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
#define HS(s) (hs.s)
extern Headset hs;
#endif /* XA_CONTEXT_PTR */

/****************************************************************************
 *
 * Internal Types
 *
 ***************************************************************************/
#define HS_ACLPENDING       5
#define HS_ACLOPEN          6
#define HS_SDPPENDING       7
#define HS_SDPSUCCESS       8
#define HS_RFCOMMPENDING    9
#define HS_RFCOMMOPEN       10
#define HS_SCOOPEN          11

/****************************************************************************
 *
 * Prototypes
 *
 ***************************************************************************/
void HsLastPacketType(U8  type);
BtStatus HsDisconnect(void);
BtStatus HsSendCKPD(void);


/* Headset lock/unlock functions */
#if XA_DEBUG == XA_ENABLED 
void OS_LockHs(void);
void OS_UnlockHs(void);
#else /* XA_DEBUG == XA_ENABLED */
#define OS_LockHs()   OS_LockStack()
#define OS_UnlockHs() OS_UnlockStack()
#endif /* XA_DEBUG == XA_ENABLED */

#endif /* BT_HSP_HEADSET == XA_ENABLED */
#endif /* _HEADSETI_H_ */
