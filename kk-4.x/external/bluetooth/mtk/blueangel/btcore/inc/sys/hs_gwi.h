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
 *     $Workfile:hs_gwi.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: This file contains the internal function prototypes and 
 *              data structures for the headset audio gateway profile core.
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
/*****************************************************************************
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 * 
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *  *
 ****************************************************************************/

#ifndef __HS_GWI_H_
#define __HS_GWI_H_

#include "conmgr.h"
#include <hs_gw.h>
#include <medev_adp.h>

#if BT_HSP_GATEWAY == XA_ENABLED

#define AG_NUM_RFCOMM_PACKETS 3

/*---------------------------------------------------------------------------
 * Gateway structure
 *
 * The Gateway structure contains all of the variables used in headset_gw.c
 *
 */
typedef struct _Gateway {

    AgState         State;
    BOOL            Initialized;

    AgAuthorize	AuthState;

    BOOL            OutGoingConn;
    BOOL            fixedHeadset;
    EvmTimer        RingTimer;
    U8              ringCount;
    U8              volbuff[11];
    U8              MicVol;
    U8              SpkVol;
    BD_ADDR         devAddr;

    AgCallback      AppCallback;
    AgCallbackInfo  Callbackinfo;
    U8              ConnectLevel;

    /* RFCOMM Variables */
    RfService       rfService;
    RfChannel       rfSrvChannel;
    RfChannel       rfCltChannel;
    RfChannel       *DefaultChannel;
    BtPacket        btPacket;
    U8              remoteRfcServer;
    ListEntry       packetList;
    BtPacket        packet[AG_NUM_RFCOMM_PACKETS];

    /* Server SDP Registration Stuff */
    SdpRecord       sdprecord;
    SdpQueryToken   sdpQueryToken;

    /* SCO Variables */
#if AG_SCO_SETTINGS == XA_ENABLED
    BtScoAudioSettings   audioSettings;
#endif
    //BtScoConnect         *scoConnect;
    //BtScoPacketType      audioPacketType;

    /* Security Variables */
    BtSecurityRecord    secRec;
    BOOL                security;

    /* The following fields are only valid when there is a connection */
    BtRemoteDevice  *link;

    U8              parms[6];
    //MeCommandToken  token[2];

    //BtSelectDeviceToken deviceQuery;
    CmgrHandler         cmgrHandler;
} Gateway;

#if XA_CONTEXT_PTR == XA_ENABLED
#define AG(s) (ag->s)
extern Gateway *ag;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
#define AG(s) (ag.s)
extern Gateway ag;
#endif /* XA_CONTEXT_PTR */

/****************************************************************************
 *
 * Imported Function prototypes
 *
 ***************************************************************************/
extern void App_Report(char *format,...);


#if BT_SECURITY == XA_ENABLED
/*
 * Instructs the piconet driver to provide a pin code.
 * If the Piconet driver does not include PIN code support then 
 * should do nothing and return FALSE. If it does includes PIN 
 * code support then it MUST call SEC_SetPin() if it returns TRUE. 
 */
extern BOOL AgEnterPinCode(BtRemoteDevice *Device);

/* used by RFCOMM for Security Mode 2 */
void RfSecCallback(const BtEvent *Event);

#endif

/****************************************************************************
 *
 * Internal Types
 *
 ***************************************************************************/
typedef U8 AgMsgType;

#define AG_OK       1
#define AG_ERR      2
#define AG_RING     3


#define AG_ACLPENDING       5
#define AG_ACLOPEN          6
#define AG_SDPPENDING       7
#define AG_SDPSUCCESS       8
#define AG_RFCOMMPENDING    9
#define AG_RFCOMMOPEN       10
#define AG_RINGING          11
#define AG_SCOPENDING       12
#define AG_SCOOPEN          13


#define AG_SCO_NOT_SET      0
#define AG_SCO_SETPENDING   1
#define AG_SCO_SETSUCCESS   2
#define AG_SCO_SETFAILED    3

/****************************************************************************
 *
 * Prototypes
 *
 ***************************************************************************/
BtStatus AgSendMessage(AgMsgType type); 

/* Headset lock/unlock functions */
#if XA_DEBUG == XA_ENABLED 
void OS_LockAg(void);
void OS_UnlockAg(void);
#else /* XA_DEBUG == XA_ENABLED */
#define OS_LockAg()   OS_LockStack()
#define OS_UnlockAg() OS_UnlockStack()
#endif /* XA_DEBUG == XA_ENABLED */

#endif /* BT_HSP_GATEWAY */
#endif /* _HS_GWI_H_ */
