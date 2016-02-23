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

#ifndef __OBXBTSTK_H
#define __OBXBTSTK_H
/****************************************************************************
 *
 * File:          obxbtstk.h
 *
 * Description:   This file contains the definitions and typedefs used
 *                by the OBEX Bluetooth transport module. It is not
 *                used by any other component of the OBEX protocol.
 * 
 * Created:       October 21, 1999
 *
 * Version:       MTObex 3.4
 *
 * Copyright 1999-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
#include <sys/obstkif.h>
#include <sys/obstack.h>

#if BT_STACK == XA_ENABLED
#include <rfcomm_adp.h>
#include <sdp_adp.h>
#include <me_adp.h>
#include "conmgr.h"
#if BT_GOEP_V2 == XA_ENABLED
#include <l2cap_adp.h>
#include <obex.h>
#endif

#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
#include <btalloc.h>
#include <sys/sdpi.h>
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */

#define OBEX_RF_OPEN_TIMEOUT  40000  /* 40s */

/* Forward type declarations  */
typedef struct _ObBtClientTransport ObBtClientTransport;
typedef struct _ObBtServerTransport ObBtServerTransport;

/*---------------------------------------------------------------------------
 *
 * Functions exported for the Application to manage the client connection.
 */
ObStatus BTSTACK_ClientConnect(ObexBtTarget         *Target,
                               ObBtClientTransport  *btxp,
                               ObexTransport          **Trans);
ObStatus BTSTACK_ClientDisconnect(ObBtClientTransport  *btxp);

/****************************************************************************
 *
 * Prototypes for Required Transport Functions (called from obstack.c)
 *
 ****************************************************************************/

/*
 * These functions are used by the 'obstack.c' Init and Deinit functions.
 * Note that when this stack layer is disabled, macros are defined to 
 * replace these functions. See the end of this file for the macros.
 */

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTSTACK_ClientInit() type
 *
 *     Initializes an instance of the OBEX Bluetooth client stack transport
 *     driver.  This function will be called directly by OBSTACK_ClientInit().
 *     Initialization may include opening endpoints and registering services. 
 *     If successful, the new stack transport instance will be "reserved" for 
 *     the specified application.
 *     
 * Parameters:
 *     trans - Bluetooth client transport structure.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The specified instance was initialized.
 *
 *     OB_STATUS_xxxxxxx - The instance could not be initialized. Do
 *                          not return OB_STATUS_PENDING.
 */
ObStatus (BTSTACK_ClientInit)(ObBtClientTransport *btxp);
/* End of BTSTACK_ClientInit */

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTSTACK_ClientDeinit() type
 *
 *     Shuts down an instance of the OBEX Bluetooth client stack transport 
 *     driver.  This function will be called directly by 
 *     OBSTACK_ClientDeinit().
 *
 * Requires:
 *     OBEX_DEINIT_FUNCS == XA_ENABLED.
 *
 * Parameters:
 *     trans - Bluetooth client transport structure.
 *
 * Returns:
 *     TRUE - If the specified instance was deinitialized.
 *
 *     FALSE - If the instance cannot be deinitialized at this time.
 */
BOOL (BTSTACK_ClientDeinit)(ObBtClientTransport *btxp);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
/* End of BTSTACK_ClientDeinit */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTSTACK_ServerInit() type
 *
 *     Initializes an instance of the OBEX Bluetooth server stack transport
 *     driver.  This function will be called directly by OBSTACK_ServerInit().
 *     Initialization may include opening endpoints and registering services. 
 *     If successful, the new stack transport instance will be "reserved" for 
 *     the specified application.
 *     
 * Parameters:
 *     trans - Bluetooth server transport structure.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The specified instance was initialized.
 *
 *     OB_STATUS_xxxxxxx - The instance could not be initialized. Do
 *                          not return OB_STATUS_PENDING.
 */
ObStatus (BTSTACK_ServerInit)(ObBtServerTransport *btxp);
/* End of BTSTACK_ServerInit */

ObStatus (BTSTACK_ServerFlushChannel)(ObBtServerTransport *btxp);


#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * BTSTACK_ServerDeinit() type
 *
 *     Shuts down an instance of the OBEX Bluetooth server stack transport 
 *     driver.  This function will be called directly by 
 *     OBSTACK_ServerDeinit().
 *
 * Requires:
 *     OBEX_DEINIT_FUNCS == XA_ENABLED
 *
 * Parameters:
 *     trans - Bluetooth server transport structure.
 *
 * Returns:
 *     TRUE - If the specified instance was deinitialized.
 *
 *     FALSE - If the instance cannot be deinitialized at this time.
 */
BOOL (BTSTACK_ServerDeinit)(ObBtServerTransport *btxp);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
/* End of BTSTACK_ServerDeinit */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

/****************************************************************************
 *
 * Structures used internally by the OBEX Bluetooth Transport
 *
 ****************************************************************************/

/*--------------------------------------------------------------------------
 *
 * OBEX Transport - This Structure manages the common Bluetooth transport 
 * components for the OBEX client/server roles.
 */
typedef struct _ObBtCommonTransport {
    U8              flags;
    ObexConnState   state;

    /* Connection oriented endpoints */
    RfChannel       channel; /* channel parameters */

    /* Token passed to parser to identify transport */
    ObexTransport   transport;

    /* Token passed to parser to identify application */
    ObexAppHandle   *app;

    CmgrHandler cmgr_hdl;

    /* Local Bluetooth device address */
    BD_ADDR         devAddr;           

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
    /* How many credits to advance after flow control 
     * has been released.
     */
    U8              credits;
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

#if BT_GOEP_V2 == XA_ENABLED
    /* record the target connected type */
    U8              bConnType;     /* 0x0 rfcomm 0x1 l2cap */
    ObexTpType      bGoepL2cap;    /* 0x0 rfcomm 0x1 l2cap 0x2 rfcomm+l2acp */
    L2capPsm        goepl2cap_psm; /* L2cap Browse psm*/
    BtSecurityRecord        secRecord;
    L2capChannelId  l2ChannelId;   /* record the local channel id */
    BtPacket        l2packet;
    ObexL2capEnhanceBuffer *pl2Buffer;  /* Used by goepl2cap_psm and l2cap module */
#endif
} ObBtCommonTransport;

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*--------------------------------------------------------------------------
 *
 * OBEX Transport - This Structure manages the Stack Layer of the OBEX
 * protocol Component. It encapsulates all RFCOMM and ACL connections.
 */
struct _ObBtClientTransport {

    ObBtCommonTransport client;

    /* Handle to device which the client is connected to */
    BtRemoteDevice  *remoteDevice;
    
    U32 target_uuid;
    
    /* Client SDP Query Stuff */
    SdpQueryToken   sdpQueryToken;

    /* RFCOMM Service Channels retrieved by SDP */
    U8              remoteRfcServer[OBEX_RF_SERVICE_CHANNELS];

    /* Index into the RFCOMM Service Channels */
    U8              index;

    /* Number of RFCOMM Channels received by SDP */
    U8              serv_chn;

    U8              supported_list; /* Only used by OPP*/

#if BT_GOEP_V2 == XA_ENABLED
    EvmTimer            rfSDPTimer;     /* Timer used to workaround RFCOMM timeout limitation */
#endif
};
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*--------------------------------------------------------------------------
 *
 * OBEX Server Transport - This Structure manages the Stack Layer of the OBEX
 * protocol Component. It encapsulates all RFCOMM and ACL connections.
 */
struct _ObBtServerTransport {

    ObBtCommonTransport server;

    /* Group: Connection oriented endpoints */
    RfService           serverReg;      /* Server's Advertised channel */

    EvmTimer            rfOpenTimer;     /* Timer used to workaround RFCOMM timeout limitation */
};

#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

/****************************************************************************
 *
 * OBEX Transport macros
 *
 ****************************************************************************/

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * InitBtClientTransport()
 *
 *     Initializes an ObBtClientTransport structure prior to registration.
 *
 * Parameters:
 *     btxp - The structure to initialize.
 *
 */
void InitBtClientTransport(ObBtClientTransport *btxp);

#define InitBtClientTransport(_BTXP)    do {      \
            OS_MemSet((U8 *)(_BTXP), 0, sizeof(ObBtClientTransport)); \
            } while (0)

#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * InitBtServerTransport()
 *
 *     Initializes an ObBtServerTransport structure prior to registration.
 *
 * Parameters:
 *     btxp - The structure to initialize.
 *
 */
void InitBtServerTransport(ObBtServerTransport *btxp);

#define InitBtServerTransport(_BTXP)    do {      \
            OS_MemSet((U8 *)(_BTXP), 0, sizeof(ObBtServerTransport)); \
            } while (0)
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

/* Transport Flags */
#define BTF_FLOW        0x01

#else /* BT_STACK == XA_ENABLED */
/*
 * Simplify Multi-Transport Init & Deinit code by providing resolution of
 * the Init & Deinit functions even when the transport is not present. In
 * debug builds, calling a missing transport's Init function will generate
 * an assertion. In non-debug the call will return failure. Deinit functions
 * can be called when the transport is not installed and must return TRUE.
 */
#define BTSTACK_ClientInit(_TRANS)      (Assert(0), OB_STATUS_FAILED)
#define BTSTACK_ServerInit(_TRANS)      (Assert(0), OB_STATUS_FAILED)
#define BTSTACK_ClientDeinit(_TRANS)    (TRUE)
#define BTSTACK_ServerDeinit(_TRANS)    (TRUE)

#endif /* BT_STACK == XA_ENABLED */
#endif /* __OBXBTSTK_H */

