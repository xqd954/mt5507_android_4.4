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

#ifndef __BTALLOCH
#define __BTALLOCH
/***************************************************************************
 *
 * File:
 *     $Workfile:btalloc.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Contains the definitions for all the memory allocations
 *     (both ROM and RAM) for the Bluetooth stack.
 *
 * Created:
 *     October 19, 1999
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
#include "bt_common.h"
#include "bt_feature.h"
#include "sys/btstats.h"

#include "sys/mei.h"
#include "sys/host.h"
#include "sys/rxbuff.h"
#include "sys/radiomgi.h"
#include "sys/l2cap_i.h"
#if RFCOMM_PROTOCOL == XA_ENABLED
#include "sys/rfc.h"
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#include "sys/sdpi.h"
#include "sys/a2mp.h"
#include "sys/att_i.h"
#include "sys/gatt_i.h"
#ifdef __BT_4_0_BLE__
#include "sys/lei.h"
#endif

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
typedef struct
{
    kal_bool afh_cmd_made;
    kal_bool pta_cmd_made;
    kal_bool afh_cmd_wait;
    kal_bool pta_cmd_wait;
    U8 ref_count;
    #ifdef __DIRECT_CHNL_MAP__
    U8 chnl_map[10] ;
    #endif
    U32 freq;
    U8 current_pta_mode;
    U8 pta_action;
#if defined (WIFI_BB_MT5921) && (defined (__BT_CHIP_VERSION_1__))
    kal_uint8 hb;
#endif        
} BtWifiCoexistContext;

typedef struct
{
    BtMeContext me;
    BtHciContext hci;
    BtRxbuffContext rxbf;
    BtRmgContext rmg;
    BtL2CapContext l2cap;
    BtAttContext att;
    BtGattContext gatt;
#if SDP_SERVER_SUPPORT == XA_ENABLED
    BtSdpServContext sdps;
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
#if SDP_CLIENT_SUPPORT == XA_ENABLED
    BtSdpClientContext sdpc;
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#if RFCOMM_PROTOCOL == XA_ENABLED
    BtRfcContext rfc;
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
    ListEntry timerList;
#if XA_STATISTICS == XA_ENABLED
    BtStatistics stats;
#endif /* XA_STATISTICS == XA_ENABLED */
#ifdef __BT_4_0_BLE__
    BtLeContext le;
#endif
} BtContext;

/****************************************************************************
 *
 * Functions and Macros
 *
 ****************************************************************************/

/*
 * The stack context can be either a global structure or a pointer
 * to a global structure. If XA_CONTEXT_PTR is set to XA_ENABLED then the
 * the context is a pointer otherwise it is a global structure.
 */
#if XA_CONTEXT_PTR == XA_ENABLED
extern BtContext *bt;

#define BTC(s)  (bt->s)
#define MEC(s)  (bt->me.s)
#define HCC(s)  (bt->hci.s)
#define RMG(s)  (bt->rmg.s)
#define L2C(s)  (bt->l2cap.s)
#define ATTC(s)  (bt->att.s)
#define GATTC(s)  (bt->gatt.s)

#if SDP_SERVER_SUPPORT == XA_ENABLED
#define SDPS(s) (bt->sdps.s)
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
#if SDP_CLIENT_SUPPORT == XA_ENABLED
#define SDPC(s) (bt->sdpc.s)
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#if RFCOMM_PROTOCOL == XA_ENABLED
#define RFC(s)  (bt->rfc.s)
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#define RXBUF(s) (bt->rxbf.s)

#else /* XA_CONTEXT_PTR == XA_ENABLED */

extern BtWifiCoexistContext bwcc;

#if 0
#define BTC(s)  (bt.s)
#define MEC(s)  (bt.me.s)
#define HCC(s)  (bt.hci.s)
#define RMG(s)  (bt.rmg.s)
#define L2C(s)  (bt.l2cap.s)
#if SDP_SERVER_SUPPORT == XA_ENABLED
#define SDPS(s) (bt.sdps.s)
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
#if SDP_CLIENT_SUPPORT == XA_ENABLED
#define SDPC(s) (bt.sdpc.s)
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#if RFCOMM_PROTOCOL == XA_ENABLED
#define RFC(s)  (bt.rfc.s)
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#define RXBUF(s) (bt.rxbf.s)
#endif /* 0 */ 
#define BTWIFIC(s) (bwcc.s)

extern BtMeContext btCtxMe;
extern BtHciContext btCtxHci;
extern BtRxbuffContext btCtxrxbf;
extern BtRmgContext btCtxrmg;
extern BtL2CapContext btCtxl2cap;
#ifdef __BT_4_0_BLE__
extern BtLeContext btCtxLe;
#endif

#if SDP_SERVER_SUPPORT == XA_ENABLED
extern BtSdpServContext btCtxsdps;
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
#if SDP_CLIENT_SUPPORT == XA_ENABLED
extern BtSdpClientContext btCtxsdpc;
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#if RFCOMM_PROTOCOL == XA_ENABLED
extern BtRfcContext btCtxrfc;
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */

#if A2MP_PROTOCOL == XA_ENABLED
extern BtA2mpContext btCtxa2mp;
#endif /* A2MP_PROTOCOL == XA_ENABLED */

#if ATT_PROTOCOL == XA_ENABLED
extern BtAttContext btCtxatt;
extern BtGattContext btCtxgatt;

#endif /* ATT_PROTOCOL == XA_ENABLED */


extern ListEntry timerList;

#if XA_STATISTICS == XA_ENABLED
extern BtStatistics btCtxstats;
#endif /* XA_STATISTICS == XA_ENABLED */

#define BTC(s)  (timerList)
#define BTCSTAT(s)  (btCtxstats)
#define MEC(s)  (btCtxMe.s)
#define HCC(s)  (btCtxHci.s)
#define RMG(s)  (btCtxrmg.s)
#define L2C(s)  (btCtxl2cap.s)
#if SDP_SERVER_SUPPORT == XA_ENABLED
#define SDPS(s) (btCtxsdps.s)
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
#if SDP_CLIENT_SUPPORT == XA_ENABLED
#define SDPC(s) (btCtxsdpc.s)
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#if RFCOMM_PROTOCOL == XA_ENABLED
#define RFC(s)  (btCtxrfc.s)
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#if A2MP_PROTOCOL == XA_ENABLED
#define A2MPC(s)  (btCtxa2mp.s)
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */

#if ATT_PROTOCOL == XA_ENABLED
#define ATTC(s)  (btCtxatt.s)
#define GATTC(s)  (btCtxgatt.s)

#endif /* ATT_PROTOCOL == XA_ENABLED */

#define RXBUF(s) (btCtxrxbf.s)

#endif /* XA_CONTEXT_PTR == XA_ENABLED */

BOOL BtInitMemory(void);

#endif /* __BTALLOCH */ 

