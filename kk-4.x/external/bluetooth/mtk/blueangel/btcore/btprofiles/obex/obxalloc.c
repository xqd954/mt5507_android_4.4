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
 * File:          obxalloc.c
 *
 * Description:   This file contains the RAM allocation for the OBEX
 *                Protocol layer. All allocations are performed here,
 *                from the Client & Server Parsers to the transport
 *                adapters.
 * 
 * Created:       February 12, 2000
 *
 * Version:     MTObex 3.4
 *
 * Copyright 2000-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems,
 * Inc.  This work contains confidential and proprietary information of
 * Extended Systems, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * Jan 9 2008 mbj06032
 * [MAUI_00515012] [BT][BIP, GOEP, OBEX] add "$Log$" in file header
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include <sys/obxalloc.h>

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

/****************************************************************************
 *
 * OBEX ROM
 *
 ****************************************************************************/
#if OBEX_ROLE_SERVER == XA_ENABLED
const ServerProcessEvent ServerEventHandlerTable[5] = {
    0, ProcessGeneral, ProcessPut, ProcessGet, 
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    ProcessSession
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
};
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
const ClientProcessEvent ClientEventHandlerTable[5] = {
    0, ProcessSimpleOpEvents, ProcessPutEvents, ProcessGetEvents, 
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    ProcessSessionEvents
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
};
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */


#if OBEX_SESSION_SUPPORT == XA_ENABLED
/* Parm bit positions: DevAddr(0), Nonce(1), SessionId(2), SeqNum(3), Timeout(4), Opcode(5) */
/* Table is indexed by Opcode:         Creat  Clos  Susp Resum  Timeout */
const U8 ObClntRxSessionParmsTable[] = {0x07, 0x00, 0x00, 0x0F, 0x00};
const U8 ObServRxSessionParmsTable[] = {0x23, 0x24, 0x20, 0x27, 0x20};
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/****************************************************************************
 *
 * OBEX RAM
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * The RAM for all of the core OBEX components are located via the RAM
 * pointer(s) or structure(s) defined in this section. RAM allocations
 * for the transport layers are performed in their private 'C' files.
 *
 * Note that the entire OBEX RAM allocation must be initialized to zero
 * before the protocol can be used.
 *
 */
#if XA_CONTEXT_PTR == XA_DISABLED
/*
 * Each OBEX modules data is stored in a global structure. This structure
 * is allocated in static RAM which is reserved in the executable.
 */
ObInternalData              ObexData;  /* Internal OBEX data */

#else   /* XA_CONTEXT_PTR == XA_DISABLED */
/*
 * The module data is accessed through a global pointer. Normally in this
 * case the data would be dynamically allocated. For testing purposes
 * the pointer points to a global structure.
 */
static ObInternalData       tempData;
ObInternalData              *ObexData = &tempData;

#endif /* XA_CONTEXT_PTR == XA_DISABLED */

void OS_LockObex(void)
{
    OS_LockStack();
    OBD(obexLock)++;
}

void OS_UnlockObex(void)
{
    ASSERT(OBD(obexLock) > 0);
    OBD(obexLock)--;
    OS_UnlockStack();
}

BOOL IsObexLocked(void)
{
    return (OBD(obexLock) > 0);
}
