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

#ifndef __OBXIRSTK_H
#define __OBXIRSTK_H
/****************************************************************************
 *
 * File:          obxirstk.h
 *
 * Description:   This file contains the definitions and typedefs used
 *                by the OBEX IrDA transport module. It is not
 *                used by any other component of the OBEX protocol.
 * 
 * Created:       February 10, 1997
 *
 * Version:       MTObex 3.4
 *
 * Copyright 1997-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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

#if IRDA_STACK == XA_ENABLED
#include <irlmpttp.h>
#include <irias.h>
#include <irstack.h>

/* Forward type declarations  */
typedef struct _ObIrClientTransport ObIrClientTransport;
typedef struct _ObIrServerTransport ObIrServerTransport;

/*---------------------------------------------------------------------------
 *
 * Functions exported for the Application to manage the client connection.
 */
ObStatus IRSTACK_ClientConnect(struct _ObexIrTarget *Device,
                               ObIrClientTransport  *irxp,
                               ObexTransport          **Trans);
ObStatus IRSTACK_ClientDisconnect(ObIrClientTransport *irxp, ObexTransportContext con, BOOL Force);

/****************************************************************************
 *
 * Types and Constants used internally by the OBEX IrDA Transport 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * OBEX Transport state
 */
typedef U8 TpState;

#define OTS_UNINITIALIZED       OCS_UNINITIALIZED
#define OTS_DISCONNECT          OCS_IDLE
#define OTS_DISCOVERY           OCS_DISCOVERY      
#define OTS_LAP_CONNECT         OCS_LINK_CONNECT    
#define OTS_IAS_QUERY           OCS_SERVICE_QUERY      
#define OTS_CONNECTING          OCS_CONNECTING     
#define OTS_CONNECTED           OCS_CONNECTED      
#define OTS_DISCONNECTING       OCS_DISCONNECTING
#define OTS_SERVICE_DISCONNECT  20  /* Leave room for other OCS events */

/*---------------------------------------------------------------------------
 *
 * IAS Query Response buffer size. The minimum is the size of an IAS Integer
 * response (for the OBEX LSap).
 */

#define IAS_RESP_BUFF_SIZE       8

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
 * IRSTACK_ClientInit() type
 *
 *     Initializes an instance of the OBEX IrDA client stack transport
 *     driver.  This function will be called directly by OBSTACK_ClientInit().
 *     Initialization may include opening endpoints and registering services. 
 *     If successful, the new stack transport instance will be "reserved" for 
 *     the specified application.
 *     
 * Parameters:
 *     trans - IrDA client transport structure.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The specified instance was initialized.
 *
 *     OB_STATUS_xxxxxxx - The instance could not be initialized. Do
 *                          not return OB_STATUS_PENDING.
 */
ObStatus (IRSTACK_ClientInit)(ObIrClientTransport *irxp);
/* End of IRSTACK_ClientInit */

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * IRSTACK_ClientDeinit() type
 *
 *     Shuts down an instance of the OBEX IrDA client stack transport 
 *     driver.  This function will be called directly by 
 *     OBSTACK_ClientDeinit().
 *
 * Requires:
 *     OBEX_DEINIT_FUNCS == XA_ENABLED.
 *
 * Parameters:
 *     trans - IrDA client transport structure.
 *
 * Returns:
 *     TRUE - If the specified instance was deinitialized.
 *
 *     FALSE - If the instance cannot be deinitialized at this time.
 */
BOOL (IRSTACK_ClientDeinit)(ObIrClientTransport *irxp);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
/* End of IRSTACK_ClientDeinit */
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * IRSTACK_ServerInit() type
 *
 *     Initializes an instance of the OBEX IrDA server stack transport
 *     driver.  This function will be called directly by OBSTACK_ServerInit().
 *     Initialization may include opening endpoints and registering services. 
 *     If successful, the new stack transport instance will be "reserved" for 
 *     the specified application.
 *     
 * Parameters:
 *     trans - IrDA server transport structure.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The specified instance was initialized.
 *
 *     OB_STATUS_xxxxxxx - The instance could not be initialized. Do
 *                          not return OB_STATUS_PENDING.
 */
ObStatus (IRSTACK_ServerInit)(ObIrServerTransport *irxp);
/* End of IRSTACK_ServerInit */

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * IRSTACK_ServerDeinit() type
 *
 *     Shuts down an instance of the OBEX IrDA server stack transport 
 *     driver.  This function will be called directly by 
 *     OBSTACK_ServerDeinit().
 *
 * Requires:
 *     OBEX_DEINIT_FUNCS == XA_ENABLED
 *
 * Parameters:
 *     trans - IrDA server transport structure.
 *
 * Returns:
 *     TRUE - If the specified instance was deinitialized.
 *
 *     FALSE - If the instance cannot be deinitialized at this time.
 */
BOOL (IRSTACK_ServerDeinit)(ObIrServerTransport *irxp);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
/* End of IRSTACK_ServerDeinit */

/*---------------------------------------------------------------------------
 * IRSTACK_ServerGetIasLsap() type
 *
 *     This function provides the IAS LSAP value for the IrDA server
 *     transport.  The application is free to register as many
 *     services as necessary over this LSAP, however, only one 
 *     service can be connected to this LSAP at a time.
 *
 *     IRSTACK_ServerInit must be called prior to checking this value,
 *     otherwise the result value must be considered invalid.
 *
 * Requires:
 *     OBEX_DEINIT_FUNCS == XA_ENABLED
 *
 * Parameters:
 *     trans - IrDA server transport structure.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - LSAP successfully retrieved.
 *
 *     OB_STATUS_FAILED - LSAP retrieval failed.  Server may not be
 *         initialized and bound to the IrDA stack yet.
 */
ObStatus IRSTACK_ServerGetIasLsap(ObIrServerTransport *irxp, U8 *lsap);
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * IRSTACK_ClientFastConEnable()
 *
 *     Enables FastConnect discovery for the transport layer. After this 
 *     function returns successfully, subsequent transport discoveries are 
 *     performed according to the Fast Connect application note from IrDA. 
 *     FastConnect can be disabled by calling IRSTACK_ClientFastConDisable().
 *
 * Parameters:
 *     initiator - TRUE: discoveries are performed without regard for
 *               media busy. Only fixed devices should set this to TRUE
 *               as it can block or negatively impact the IR space.
 *
 *               FALSE: Fast Connect logic will be used but media busy 
 *               rules are honored.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Subsequent transport discoveries will use Fast 
 *         Connect.
 *
 *     OB_STATUS_FAILED - Failure to set Fast Connect mode.
 *
 *     OB_STATUS_NOT_SUPPORTED - Fast Connect is not supported by the transport.
 */
ObStatus IRSTACK_ClientFastConEnable(BOOL initiator);
#define IRSTACK_ClientFastConEnable(_INITIATOR) IR_FastConEnable(_INITIATOR)

/*---------------------------------------------------------------------------
 *            IRSTACK_ClientFastConDisable()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disables FastConnect transport discovery. If a transport 
 *            discovery is already in progress, it will be allowed to 
 *            complete.
 */
void IRSTACK_ClientFastConDisable(void);
#define IRSTACK_ClientFastConDisable() IR_FastConDisable()
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            IRSTACK_SetForceFlag()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Force the disconnection of the underlying LAP connection.
 *
 * Parameters:
 *     context - Transport context structure.
 *
 * Return:    void
 */
void IRSTACK_SetForceFlag(ObexTransportContext context);
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED) */

/*---------------------------------------------------------------------------
 *            IRSTACK_IsIrLapConnected()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the status of the IrLAP connection.  Can be used to 
 *            distinguish a LMP disconnect from a LAP disconnect indication.
 */
BOOL IRSTACK_IsIrLapConnected(void);
#define IRSTACK_IsIrLapConnected() IR_IsIrLapConnected()

/*---------------------------------------------------------------------------
 *            IRSTACK_DisconnectIrLap()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnects the IrLAP connection.  This is useful when forcing
 *            the transport down due to the service not being found.
 */
BOOL IRSTACK_DisconnectIrLap(void);
#define IRSTACK_DisconnectIrLap() IR_DisconnectIrLap()

/****************************************************************************
 *
 * Structures used internally by the OBEX Transport
 *
 ****************************************************************************/

/*--------------------------------------------------------------------------
 *
 * OBEX Transport - This Structure manages the common IrDA transport 
 * components for the OBEX client/server roles.
 */
typedef struct _ObIrCommonTransport {
    U8              flags;
    TpState         state;

    /* Connection oriented endpoints */
    IrConnect       conn;

    /* Application context handles */
    ObexAppHandle   *app;

    ObexTransport   transport;

    /* Local IrDA device address */
    U8              devAddr[4];

    /* The client and server each need a connection maintenance
     * packet for handling connects & disconnects.
     */
    IrPacket        mgmtPkts[1];
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
    /* The Client and Server each need to know how many credits
     * to advance after flow control has been released.
     */
    U8              credits;
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
} ObIrCommonTransport;
 
#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*--------------------------------------------------------------------------
 *
 * OBEX Transport - This Structure manages the Stack Layer of the OBEX
 * protocol Component. It encapsulates the TinyTP connections as well
 * as the transmit buffer pool.
 */
struct _ObIrClientTransport {

    ObIrCommonTransport client;

    /* Group: IAS Query - for Lsap & Telecom params */
    IrIasQuery      IasQuery;
    U8              IasResponse[IAS_RESP_BUFF_SIZE];
    
    EvmTimer        timer;
};
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*--------------------------------------------------------------------------
 *
 * OBEX Transport - This Structure manages the Stack Layer of the OBEX
 * protocol Component. It encapsulates the TinyTP connections as well
 * as the transmit buffer pool.
 */
struct _ObIrServerTransport {
    ObIrCommonTransport server;
};
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

/****************************************************************************
 *
 * OBEX Transport macros
 *
 ****************************************************************************/

/* Make sure the IrDA Fast Connect macros are valid for IrDA stacks without
 * fast connect support.  IrDA SDK 3.1 introduces these macros.
 */
#if !defined(IR_FASTCONNECT)
#define IR_FastConEnable(initiator)     IR_STATUS_NOT_SUPPORTED
#define IR_FastConDisable()             0
#endif

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * InitIrClientTransport()
 *
 *     Initializes an ObIrClientTransport structure prior to registration.
 *
 * Parameters:
 *     irxp - The structure to initialize.
 *
 */
void InitIrClientTransport(ObIrClientTransport *irxp);

#define InitIrClientTransport(_IRXP)    do {      \
            OS_MemSet((U8 *)(_IRXP), 0, sizeof(ObIrClientTransport)); \
            } while (0)
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * InitIrServerTransport()
 *
 *     Initializes an ObIrServerTransport structure prior to registration.
 *
 * Parameters:
 *     irxp - The structure to initialize.
 *
 */
void InitIrServerTransport(ObIrServerTransport *irxp);

#define InitIrServerTransport(_IRXP)    do {      \
            OS_MemSet((U8 *)(_IRXP), 0, sizeof(ObIrServerTransport)); \
            } while (0)
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

/* Transport Packet Pool Flags */
#define TPP_M0_PKT_BUSY      0x01
/* Transport Flow Control Flags */
#define TPP_FLOW             0x02
/* Transport Force Disconnect Flag */
#define TP_FORCE_DISCONNECT  0x04

/* Server Transport structure */
#define IRSXP(f)    (((ObexServerApp*)ObexApp)->trans.ObexServerIrTrans.f)
/* Client Transport structure */
#define IRCXP(f)    (((ObexClientApp*)ObexApp)->trans.ObexClientIrTrans.f)

#else /* IRDA_STACK == XA_ENABLED */
/*
 * Simplify Multi-Transport Init & Deinit code by providing resolution of
 * the Init & Deinit functions even when the transport is not present. In
 * debug builds, calling a missing transport's Init function will generate
 * an assertion. In non-debug the call will return failure. Deinit functions
 * can be called when the transport is not installed and must return TRUE.
 */
#define IRSTACK_ClientInit(_APP)      (Assert(0), OB_STATUS_FAILED)
#define IRSTACK_ServerInit(_APP)      (Assert(0), OB_STATUS_FAILED)
#define IRSTACK_ClientDeinit(_APP)    (TRUE)
#define IRSTACK_ServerDeinit(_APP)    (TRUE)

#endif /* IRDA_STACK == XA_ENABLED */
#endif /* __OBXIRSTK_H */

