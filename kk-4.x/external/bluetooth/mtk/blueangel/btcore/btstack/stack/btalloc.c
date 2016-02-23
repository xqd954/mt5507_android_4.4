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

/***************************************************************************
 *
 * File:
 *     $Workfile:btalloc.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Contains the allocation for all the memory used in the
 *     the Bluetooth stack (both ROM and RAM).
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

#include "btalloc.h"

/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/

/*
 * The stack context
 */
#if XA_CONTEXT_PTR == XA_DISABLED
/* The stack context is a global structure */
BtWifiCoexistContext bwcc = {0};

/* BtContext bt; */

BtMeContext btCtxMe;
BtHciContext btCtxHci;
BtRxbuffContext btCtxrxbf;
BtRmgContext btCtxrmg;
BtL2CapContext btCtxl2cap;
#ifdef __BT_4_0_BLE__
BtLeContext btCtxLe;
#endif

#if SDP_SERVER_SUPPORT == XA_ENABLED
BtSdpServContext btCtxsdps;
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
#if SDP_CLIENT_SUPPORT == XA_ENABLED
BtSdpClientContext btCtxsdpc;
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#if RFCOMM_PROTOCOL == XA_ENABLED
BtRfcContext btCtxrfc;
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#if A2MP_PROTOCOL == XA_ENABLED
BtA2mpContext btCtxa2mp;
#endif /* A2MP_PROTOCOL == XA_ENABLED */
#if ATT_PROTOCOL == XA_ENABLED
BtAttContext btCtxatt;
BtGattContext btCtxgatt;
#endif
ListEntry timerList;

#if XA_STATISTICS == XA_ENABLED
BtStatistics btCtxstats;
#endif /* XA_STATISTICS == XA_ENABLED */

#else /* XA_CONTEXT_PTR == XA_DISABLED */
/*
 * The stack context is accessed through a pointer. Normally in this case
 * the stack context would be dynamically allocated. For testing purposes
 * the pointer points to a global structure.
 */
BtContext btTmp;
BtContext *bt = &btTmp;
#endif /* XA_CONTEXT_PTR == XA_DISABLED */


/*****************************************************************************
 * FUNCTION
 *  BtInitMemory
 * DESCRIPTION
 *  Intialize the memory used by the Bluetooth stack. This may
 *  include allocating the memory.
 * PARAMETERS
 *  void
 * RETURNS
 *  TRUE if success otherwise FALSE.
 *****************************************************************************/
BOOL BtInitMemory(void)
{
    OS_MemSet((U8*) & btCtxMe, 0, (U32) sizeof(BtMeContext));
    OS_MemSet((U8*) & btCtxHci, 0, (U32) sizeof(BtHciContext));
    OS_MemSet((U8*) & btCtxrxbf, 0, (U32) sizeof(BtRxbuffContext));
    OS_MemSet((U8*) & btCtxl2cap, 0, (U32) sizeof(BtL2CapContext));
    OS_MemSet((U8*) & btCtxrmg, 0, (U32) sizeof(BtRmgContext));
#if SDP_SERVER_SUPPORT == XA_ENABLED
    OS_MemSet((U8*) & btCtxsdps, 0, (U32) sizeof(BtSdpServContext));
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */
#if SDP_CLIENT_SUPPORT == XA_ENABLED
    OS_MemSet((U8*) & btCtxsdpc, 0, (U32) sizeof(BtSdpClientContext));
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */
#if RFCOMM_PROTOCOL == XA_ENABLED
    OS_MemSet((U8*) & btCtxrfc, 0, (U32) sizeof(BtRfcContext));
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#if A2MP_PROTOCOL == XA_ENABLED
    OS_MemSet((U8*) & btCtxa2mp, 0, (U32) sizeof(BtA2mpContext));
#endif
    OS_MemSet((U8*) & timerList, 0, (U32) sizeof(ListEntry));
#if XA_STATISTICS == XA_ENABLED
    OS_MemSet((U8*) & btCtxstats, 0, (U32) sizeof(BtStatistics));
#endif /* XA_STATISTICS == XA_ENABLED */
#ifdef __BT_4_0_BLE__
    OS_MemSet((U8*) & btCtxLe, 0, (U32) sizeof(BtLeContext));
#endif
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  BTInitWifiCoexistence
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTInitWifiCoexistence(void)
{
    U8 *ptr;

    ptr = (U8*) & bwcc;
    OS_MemSet(ptr, 0, (U32) sizeof(BtWifiCoexistContext));
}

