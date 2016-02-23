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

#ifndef __OBXOEMSTK_H
#define __OBXOEMSTK_H
/****************************************************************************
 *
 * File:          obxoemstk.h
 *
 * Description:   This file contains the definitions and typedefs used
 *                by the OBEX OEM transport module. It is not
 *                used by any other component of the OBEX protocol.
 * 
 * Created:       January 12, 2004
 *
 * Version:       MTObex 3.4
 *
 * Copyright 2003-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
 ****************************************************************************/
#include <sys/obstkif.h>
#include <sys/obprotif.h>

#if OEM_STACK == XA_ENABLED

/* Forward type declarations  */
typedef struct _ObOemTransport      ObOemClientTransport;
typedef struct _ObOemTransport      ObOemServerTransport;

/*---------------------------------------------------------------------------
 *
 * Functions exported for the Application to manage the client connection.
 */
ObStatus OEMSTACK_ClientConnect(ObexOemTarget         *Target,
                                ObOemClientTransport  *Xpt,
                                ObexTransport        **Trans);

ObStatus OEMSTACK_ClientDisconnect(ObOemClientTransport  *Xpt);

/****************************************************************************
 *
 * Prototypes for Required Transport Functions (called from obstack.c)
 *
 ****************************************************************************/

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*
 * These functions are used by the 'obstack.c' Init and Deinit functions.
 * Note that when this stack layer is disabled, macros are defined to 
 * replace these functions. See the end of this file for the macros.
 */
/*---------------------------------------------------------------------------
 * OEMSTACK_ClientInit() type
 *
 *     Initializes an instance of the OBEX client stack transport
 *     driver.  This function will be called directly by OBSTACK_ClientInit().
 *     Initialization may include opening endpoints and registering services. 
 *     If successful, the new stack transport instance will be "reserved" for 
 *     the specified application.
 *     
 * Parameters:
 *     Xpt - Client transport structure.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The specified instance was initialized.
 *
 *     OB_STATUS_xxxxxxx - The instance could not be initialized. Do
 *                          not return OB_STATUS_PENDING.
 */
ObStatus (OEMSTACK_ClientInit)(ObOemClientTransport *Xpt, ObexAppHandle *app);
/* End of TRANS_ClientInit */

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * OEMSTACK_ClientDeinit() type
 *
 *     Shuts down an instance of the OBEX client stack transport 
 *     driver.  This function will be called directly by 
 *     OBSTACK_ClientDeinit().
 *
 * Requires:
 *     OBEX_DEINIT_FUNCS == XA_ENABLED.
 *
 * Parameters:
 *     Xpt - Client transport structure.
 *
 * Returns:
 *     TRUE - If the specified instance was deinitialized.
 *
 *     FALSE - If the instance cannot be deinitialized at this time.
 */
BOOL (OEMSTACK_ClientDeinit)(ObOemClientTransport *Xpt);
/* End of TRANS_ClientDeinit */
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * OEMSTACK_ServerInit() type
 *
 *     Initializes an instance of the OBEX server stack transport
 *     driver.  This function will be called directly by OBSTACK_ServerInit().
 *     Initialization may include opening endpoints and registering services. 
 *     If successful, the new stack transport instance will be "reserved" for 
 *     the specified application.
 *     
 * Parameters:
 *     Xpt - Server transport structure.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The specified instance was initialized.
 *
 *     OB_STATUS_xxxxxxx - The instance could not be initialized. Do
 *                          not return OB_STATUS_PENDING.
 */
ObStatus (OEMSTACK_ServerInit)(ObOemServerTransport *Xpt, ObexAppHandle *app);
/* End of TRANS_ServerInit */

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * OEMSTACK_ServerDeinit() type
 *
 *     Shuts down an instance of the OBEX server stack transport 
 *     driver.  This function will be called directly by 
 *     OBSTACK_ServerDeinit().
 *
 * Requires:
 *     OBEX_DEINIT_FUNCS == XA_ENABLED
 *
 * Parameters:
 *     Xpt - Server transport structure.
 *
 * Returns:
 *     TRUE - If the specified instance was deinitialized.
 *
 *     FALSE - If the instance cannot be deinitialized at this time.
 */
BOOL (OEMSTACK_ServerDeinit)(ObOemServerTransport *Xpt);
/* End of TRANS_ServerDeinit */
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

/****************************************************************************
 *
 * OBEX Transport defines
 *
 ****************************************************************************/

/* Transport Flow Control Flag Bit */
#define OTF_FLOW        0x01

/****************************************************************************
 *
 * Structures used internally by the OBEX Transport
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * ObOemTransport structure
 *
 *     This structure manages the OEM transport components
 *     for the OBEX client/server roles.  Add fields as necessary 
 *     to support the OEM transport.
 */
typedef struct _ObOemTransport {
    /* Transport state presented to OBEX protocol. */
    ObexConnState       state;

    /* Handle provided to OBEX Protocol during transport initialization. */
    ObexTransport       trans;

    /* Application handle for OBEX Client or Server instance. */
    ObexAppHandle      *appHandle;

    /* Transport flags for controlling such things as flow control */
    U8                  flags;

    /* Add any additional transport fields here */
} ObOemTransport;

/*---------------------------------------------------------------------------
 * ObexOemTarget structure
 *
 *     This structure manages the target device information for the OEM 
 *     transport. The ObexTpAddr structure in obex.h is defined with a 
 *     pointer to the ObexOemTarget structure. This structure is passed to 
 *     OEMSTACK_ClientConnect and should contain information to connect to
 *     a remote device over the OEM transport. This structure might contain
 *     address, port, or other such pertinent information used to connect 
 *     devices over the OEM transport.
 */
struct ObexOemTarget {

    /* Add fields needed to connect to the OEM transport */

    /* Sample address field for the OEM target address.  Change as necessary. */
    U32     addr;
};

#else /* OEM_STACK == XA_ENABLED */

/*
 * Simplify Multi-Transport Init & Deinit code by providing resolution of
 * the Init & Deinit functions even when the transport is not present. In
 * debug builds, calling a missing transport's Init function will generate
 * an assertion. In non-debug the call will return failure. Deinit functions
 * can be called when the transport is not installed and must return TRUE.
 */
#define OEMSTACK_ClientInit(_TRANS)      (Assert(0), OB_STATUS_FAILED)
#define OEMSTACK_ServerInit(_TRANS)      (Assert(0), OB_STATUS_FAILED)
#define OEMSTACK_ClientDeinit(_TRANS)    (TRUE)
#define OEMSTACK_ServerDeinit(_TRANS)    (TRUE)

#endif /* OEM_STACK == XA_ENABLED */
#endif /* __OBXOEMSTK_H */
