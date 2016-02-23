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
 * File:        obstack.c
 *
 * Description: This file contains code for the stack independent transport
 *              interface portion of the OBEX layer. It is used by the app.
 *              and the protocol parser to manage transport connections and
 *              transmit data, respectively.
 *
 * Created:     February 10, 1997
 *
 * Version:     MTObex 3.4
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
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * Dec 19 2008 mbj06032
 * [MAUI_01481941] MTK:Bluetooth_it the FTP can't disconenct
 * 
 *
 * Dec 18 2008 mbj06038
 * [MAUI_01353880] [OBEX] connect fail with CHIP_REASON, the same client could not connect again.
 * 
 *
 * Jul 24 2008 mbj06032
 * [MAUI_00875438] [1] ASSERT Fail: (*eid)-> data!=NULL event_shed.c 748 - BT
 * Tx Timer func use ObexPacket pointer as keyword to search the ObSendingPacket list 
 * 
 *
 * Jul 18 2008 mbj06032
 * [MAUI_01178462] MTK:Bluetooth FTP_handset always show "cancelling" screen
 * check pointer
 *
 * Jul 15 2008 mbj06032
 * [MAUI_01178462] MTK:Bluetooth FTP_handset always show "cancelling" screen
 * start Tx timer after call RF_SendData; when timeout, disconnect and return the tx buffer
 *
 * Jul 15 2008 mbj06032
 * [MAUI_01178462] MTK:Bluetooth FTP_handset always show "cancelling" screen
 * start Tx timer after call RF_SendData; when timeout, disconnect and return the tx buffer
 *
 * May 16 2008 mbj06032
 * [MAUI_00259549] [BIP]_29 handset connect M8223 BIP handset popup "unfinished"
 * revise code
 *
 * May 13 2008 mbj06032
 * [MAUI_00093122] Screen always displays recieving X%, x/x.
 * use enum value not constant number
 *
 * Apr 10 2008 mbj06032
 * [MAUI_00653223] [BT OBEX]modify trace info
 * 
 *
 * Mar 21 2008 mbj06032
 * [MAUI_00096220] BIP gets disconnected in the mentioned scenario.
 * modify OBPROT_TxDone func: when no free packet, still need "break"
 *
 * Mar 20 2008 mbj06032
 * [MAUI_00096220] BIP gets disconnected in the mentioned scenario.
 * modify OBPROT_TxDone : when no more free packet, just return: don't call ObParserTxDone
 *
 * Jan 9 2008 mbj06032
 * [MAUI_00515012] [BT][BIP, GOEP, OBEX] add "$Log$" in file header
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include <sys/obxalloc.h>
#ifdef __BT_GOEP_V2__
#include "goep.h"
#endif
#if BT_STACK == XA_ENABLED
#include <sys/obxbtstk.h>
#endif /* BT_STACK == XA_ENABLED */
#if IRDA_STACK == XA_ENABLED
#include <sys/obxirstk.h>
#endif /* IRDA_STACK == XA_ENABLED */
#if TCP_STACK == XA_ENABLED
#include <sys/obxtcpstk.h>
#endif /* TCP_STACK == XA_ENABLED */
#if OEM_STACK == XA_ENABLED
#include <sys/obxoemstk.h>
#endif /* OEM_STACK == XA_ENABLED */
#include "bluetooth_trc.h"

#include "obex_utl.h"

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

#if BT_STACK == XA_ENABLED
extern ObStatus BTSTACK_ClientFlushChannel(ObBtClientTransport *btxp);
#endif
/*---------------------------------------------------------------------------
 *            OBEX_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initializes the OBEX component's internal data.  This must be
 *            the first OBEX function called by the application layer, or if 
 *            multiple OBEX applications exist, this function should be 
 *            called at system startup (see XA_LOAD_LIST in config.h).
 *
 * Return:    TRUE or FALSE
 */
BOOL OBEX_Init(void)
{
    I8  i;

    OS_LockStack();

#if XA_CONTEXT_PTR == XA_DISABLED
    OS_MemSet((U8 *)&ObexData, 0, sizeof(ObInternalData));
#else
    OS_MemSet((U8 *)ObexData, 0, sizeof(ObInternalData));
#endif /* XA_CONTEXT_PTR == XA_DISABLED */

    /* Initialize the Packet List */
    InitializeListHead(&(OBD(packetList)));
    OBD(packetnum) = 0;

    
    /* Fill in the Packet List according to the number of 
     * available transmit packets.
     */
    for (i = 0; i < OBEX_NUM_TX_PACKETS; i++) {
        InsertTailList(&(OBD(packetList)), &(OBD(packets)[i].pkt.node));
        OBD(packetnum)++;
    }

    /* Initialize the client App Handle List */
    InitializeListHead(&(OBD(clientAppHndlList)));
    OBD(curClientNode) = &(OBD(clientAppHndlList));

    /* Initialize the server App Handle List */
    InitializeListHead(&(OBD(serverAppHndlList)));
    OBD(curServerNode) = &(OBD(serverAppHndlList));

    /* OBEX Initialized */
    OBD(initialized) = TRUE;

    OS_UnlockStack();
    return TRUE;
}


#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            OBEX_TpConnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate a transport connection to the specified an OBEX device.
 *            This function takes a TpAddr type which allows it to receive
 *            a variety of addresses to connect to.
 *
 * Return:    OB_STATUS_PENDING - connection was started successfully.
 *            OB_STATUS_SUCCESS - connection was established.
 *            OB_STATUS_BUSY - connection failed because the client is busy.
 *            OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_TpConnect(ObexClientTransports *Transports, ObexAppHandle *AppHndl, 
                        ObexTpAddr *Target)
{
    ObStatus     status = OB_STATUS_BUSY;
    ObexTransport *trans = 0;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_TPCONNECT);
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if ((!Transports) || (!AppHndl) || (!Target) || (Target->type == OBEX_TP_NONE) || 
        ((Target->type & OBEX_EnumTransports()) != Target->type)) { 
        kal_trace(BT_TRACE_G6_OBEX,OBEX_TPCONNECT_INVALIDPARAM);
        status = OB_STATUS_INVALID_PARM;
        goto done;
    }
    
    /* Only allow a connection to one of the Obex Client's transports */
    if (AppHndl->parser.stack)
    {
        kal_trace(BT_TRACE_G6_OBEX,OBEX_TPCONNECT_CONNECTED);
        goto done;
    }

#endif /* XA_ERROR_CHECK == XA_ENABLED */
    
    ASSERT(Transports && AppHndl && Target && (Target->type != OBEX_TP_NONE) &&
           (Target->type & OBEX_EnumTransports()));

    switch (Target->type) {

#if IRDA_STACK == XA_ENABLED
    case OBEX_TP_IRDA:
        status = IRSTACK_ClientConnect(&Target->proto.ir, &Transports->ObexClientIrTrans, &trans);
        break;
#endif /* IRDA_STACK == XA_ENABLED */

#if BT_STACK == XA_ENABLED
    case OBEX_TP_BLUETOOTH:
        status = BTSTACK_ClientConnect(&Target->proto.bt, &Transports->ObexClientBtTrans, &trans);
        break;
#endif /* BT_STACK == XA_ENABLED */

#if TCP_STACK == XA_ENABLED
    case OBEX_TP_TCP:
        status = TCPSTACK_ClientConnect(&Target->proto.tcp, &Transports->ObexClientTcpTrans, &trans);
        break;
#endif /* TCP_STACK == XA_ENABLED */

#if OEM_STACK == XA_ENABLED
    case OBEX_TP_OEM:
        status = OEMSTACK_ClientConnect(Target->proto.oem, &Transports->ObexClientOemTrans, &trans);
        break;
#endif /* OEM_STACK == XA_ENABLED */
    
    default:
        status = OB_STATUS_INVALID_PARM;
        goto done;
        //break;
    }

done:
    kal_trace(BT_TRACE_G6_OBEX,OBEX_TPCONNECT_RETURN, status);
    
    if ((status == OB_STATUS_SUCCESS) || (status == OB_STATUS_PENDING)) {
        //Report(("[OBEX] OBEX_TpConnect call OBPROT_LinkTransport \n"));
        OBPROT_LinkTransport(AppHndl, trans);
        /* Assign the client transport type */
        AppHndl->tpType = Target->type;
        trans->tpType = Target->type;
    }

    if (status == OB_STATUS_SUCCESS) {
        /* Indicate the transport is connected */
        AppHndl->tpStatus = OB_TP_CONNECTED; /* Connected */
    }

    OS_UnlockObex();
    return status;
}
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            OBEX_TpDisconnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect the client's Transport connection if one exists.
 *
 * Return:    OB_STATUS_PENDING - disconnect was started successfully.
 *            OB_STATUS_SUCCESS - disconnect completed.
 *            OB_STATUS_FAILED - the connection could not be disconnected.
 *            OB_STATUS_NO_CONNECT - there is no connection to disconnect.
 *            OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_TpDisconnect(void *Transports, ObexAppHandle *AppHndl, 
                           BOOL Force)
{
    ObStatus    status = OB_STATUS_NO_CONNECT;
    
    kal_trace(BT_TRACE_G6_OBEX,OBEX_TPDISCONNECT);
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Transports || !AppHndl) {
        OS_UnlockObex();
        kal_trace(BT_TRACE_G6_OBEX,OBEX_TPDISCONNECT_INVALID_PARM);
        return OB_STATUS_INVALID_PARM;
    }

    if (AppHndl->parser.stack == 0) {
        OS_UnlockObex();
        kal_trace(BT_TRACE_G6_OBEX,OBEX_TPDISCONNECT_NOCONNECT);
        
        return status;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Transports && AppHndl->parser.stack);

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
    switch (AppHndl->tpType) {
#if IRDA_STACK == XA_ENABLED
    case OBEX_TP_IRDA:
        status = IRSTACK_ClientDisconnect(&((ObexClientTransports *)Transports)->ObexClientIrTrans, AppHndl->parser.stack, Force);
        break;
#endif /* IRDA_STACK == XA_ENABLED */

#if BT_STACK == XA_ENABLED
    case OBEX_TP_BLUETOOTH:
        status = BTSTACK_ClientDisconnect(&((ObexClientTransports *)Transports)->ObexClientBtTrans);
        break;
#endif /* BT_STACK == XA_ENABLED */

#if TCP_STACK == XA_ENABLED
    case OBEX_TP_TCP:
        status = TCPSTACK_ClientDisconnect(&((ObexClientTransports *)Transports)->ObexClientTcpTrans);
        break;
#endif /* TCP_STACK == XA_ENABLED */

#if OEM_STACK == XA_ENABLED
    case OBEX_TP_OEM:
        status = OEMSTACK_ClientDisconnect(&((ObexClientTransports *)Transports)->ObexClientOemTrans);
        break;
#endif /* OEM_STACK == XA_ENABLED */
    }
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

    if ((AppHndl->tpType == OBEX_TP_NONE) && (AppHndl->tpStatus & OB_TP_CONNECTED)) {
        /* We are requesting that the OBEX Transport Server disonnect, and the transport
         * connection is in fact up.
         */
#if IRDA_STACK == XA_ENABLED
        if ((AppHndl->parser.stack->tpType == OBEX_TP_IRDA) && Force) {
            /* Tell the IrDA transport that we are going to force down
             * the IrLAP link during our Server transport disconnect.
             */
            IRSTACK_SetForceFlag(AppHndl->parser.stack->context);
        }
#endif /* IRDA_STACK == XA_ENABLED */

        if (AppHndl->tpStatus == OB_TP_CONNECTED || AppHndl->tpStatus == OB_TP_CONNECTING)
        {
            status = OBSTACK_Disconnect(AppHndl->parser.stack);
        }
        else if (AppHndl->tpStatus == OB_TP_DISCONNECTING)
        {
            status = OB_STATUS_PENDING;
        }
    }

    if ((status == OB_STATUS_PENDING) || (status == OB_STATUS_SUCCESS)) {
        AppHndl->tpType = OBEX_TP_NONE;
        AppHndl->tpStatus = OB_TP_DISCONNECTING; /* Disconnecting */
    }

    if (status == OB_STATUS_SUCCESS) {
        /* Indicate the transport is disconnected */
        AppHndl->tpStatus = OB_TP_DISCONNECTED;  /* Disconnected */
        ObParserDisconnectInd(AppHndl);
    }

    OS_UnlockObex();
    return status;
}
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED) */

/*---------------------------------------------------------------------------
 *            OBEX_EnumTransports()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns a list of the installed OBEX transport layers.
 *
 * Return:    Bitwise ORing of OBEX_TP_ values.
 */
U8 OBEX_EnumTransports(void)
{
    U8 tp;

    tp = (OBEX_TP_NONE
#if BT_STACK == XA_ENABLED
          |OBEX_TP_BLUETOOTH
#endif /* BT_STACK == XA_ENABLED */
#if IRDA_STACK == XA_ENABLED
          |OBEX_TP_IRDA
#endif /* IRDA_STACK == XA_ENABLED */
#if TCP_STACK == XA_ENABLED
          |OBEX_TP_TCP
#endif /* TCP_STACK == XA_ENABLED */
#if OEM_STACK == XA_ENABLED
          |OBEX_TP_OEM
#endif /* OEM_STACK == XA_ENABLED */
    );

    return tp;
}

/*---------------------------------------------------------------------------
 *            OBEX_GetTpConnInfo()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the low water mark for application storage space
 *            that can be available before the applicaiton must tell OBEX
 *            to exert flow control on the Link.
 *
 * Return:    TRUE - The structure was successfully completed.
 *            FALSE - The transport is not connected.
 */
BOOL OBEX_GetTpConnInfo(ObexAppHandle *AppHndl, ObexTpConnInfo *TpConnInfo)
{
    const ObexTransport  *trans;
    BOOL                ok = FALSE;

    OS_LockObex();
#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl || !TpConnInfo) {
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(AppHndl && TpConnInfo);
    trans = (AppHndl->parser).stack;

#if XA_ERROR_CHECK == XA_ENABLED
    if (!trans || !trans->funcTab->GetTpConnInfo) {
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(trans && trans->funcTab->GetTpConnInfo);

    if (trans) {
        if (trans->funcTab->GetTpConnInfo)
            ok = trans->funcTab->GetTpConnInfo(trans->context, TpConnInfo);
        else ok = FALSE;
    }

#if XA_ERROR_CHECK == XA_ENABLED
Done:
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    OS_UnlockObex();
    return ok;
}

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBEX_IsRxFlowAvailable()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Allows the application to determine if a credit based 
 *            receive flow control is available.
 *
 * Return:    TRUE -  Receive flow control is available.
 *            FALSE - Receive flow control is not available.
 */
BOOL OBEX_IsRxFlowAvailable(ObexAppHandle *AppHndl)
{
    const ObexTransport   *trans;
    BOOL                 available = FALSE;

    OS_LockObex();
#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl) {
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(AppHndl);
    trans = (AppHndl->parser).stack;

#if XA_ERROR_CHECK == XA_ENABLED
    if (!trans || !trans->funcTab->IsRxFlowAvail) {
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(trans && trans->funcTab->IsRxFlowAvail);

    if (trans) {
        if (trans->funcTab->IsRxFlowAvail)
            available = trans->funcTab->IsRxFlowAvail(trans->context);
        else available = FALSE;
    }
    
#if XA_ERROR_CHECK == XA_ENABLED
Done:
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    OS_UnlockObex();
    return available;
}

/*---------------------------------------------------------------------------
 *            OBEX_PauseRxFlow()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Tells the transport layer to stop the remote device from
 *            sending data by refusing to advance transport credit. Once
 *            this function has been called, no more than the number of 
 *            bytes or packets reported by OBEX_GetTpConnInfo() will be
 *            received. This amount includes any currently indicated data 
 *            (if called during a data indication). Flow can be resumed
 *            with OBEX_ResumeRxFlow().
 *               
 * Return:    void
 */
void OBEX_PauseRxFlow(ObexAppHandle *AppHndl)
{
    const ObexTransport  *trans;

    OS_LockObex();
#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl) {
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(AppHndl);
    trans = (AppHndl->parser).stack;

#if XA_ERROR_CHECK == XA_ENABLED
    if (!trans || !trans->funcTab->PauseRxFlow) {
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(trans && trans->funcTab->PauseRxFlow);

    if (trans) {
        ASSERT(trans->funcTab->PauseRxFlow);
        trans->funcTab->PauseRxFlow(trans->context);
    }

#if XA_ERROR_CHECK == XA_ENABLED
Done:
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    OS_UnlockObex();
}

/*---------------------------------------------------------------------------
 *            OBEX_ResumeRxFlow()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Opens the receive window after flow control was exerted.
 *
 * Return:    void
 */
void OBEX_ResumeRxFlow(ObexAppHandle *AppHndl)
{
    const ObexTransport  *trans;

    OS_LockObex();
#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl) {
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(AppHndl);
    trans = (AppHndl->parser).stack;

#if XA_ERROR_CHECK == XA_ENABLED
    if (!trans || !trans->funcTab->ResumeRxFlow) {
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(trans && trans->funcTab->ResumeRxFlow);

    if (trans) {
        ASSERT(trans->funcTab->ResumeRxFlow);
        trans->funcTab->ResumeRxFlow(trans->context);
    }
#if XA_ERROR_CHECK == XA_ENABLED
Done:
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    OS_UnlockObex();
}
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

/****************************************************************************
 *
 * OBEX Transport functions used internally by the OBEX Parser
 *
 ****************************************************************************/
/*
 * A special note for Init and Deinit functions:
 * In order to minimize the clutter created by the ability to conditionally
 * compile in or out each transport layer. Each transport header file contains
 * definitions for its init and deinit functions that exist regardless of
 * whether the transport is enabled or not. See the individual include files
 * for more information.
 */

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBSTACK_ClientInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the requested OBEX Client Transport. A single Obex 
 *            Client can only support one transport, so only one will be
 *            initialized.  Multiple clients will need to be initialized if
 *            multiple transports are going to be used.
 *
 * Return:    OB_STATUS_SUCCESS - Initialized successfully
 *            OB_STATUS_FAILED - Unable to bind to the stack.
 *            OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBSTACK_ClientInit(ObexClientApp *ObexApp, ObexTpType Protocols)
{
    ObStatus    status = OB_STATUS_INVALID_PARM;

    ASSERT(IsObexLocked());

#if XA_ERROR_CHECK == XA_ENABLED
    if (OBD(initialized) != TRUE) {
        /* OBEX is not initialized */
        return OB_STATUS_FAILED;
    }

    if ((OBSTACK_VerifyClientAppHandle(&ObexApp->handle)) ||
       ((Protocols & OBEX_EnumTransports()) != Protocols) || 
       (Protocols == OBEX_TP_NONE)) {
           return status;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Protocols && (Protocols & OBEX_EnumTransports()) && 
        !OBSTACK_VerifyClientAppHandle(&ObexApp->handle));

#if IRDA_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_IRDA) {
        /* Initialize client for the IrDA Transport */
        if ((status = IRSTACK_ClientInit(&ObexApp->trans.ObexClientIrTrans)) != OB_STATUS_SUCCESS)
            goto InitFailure;
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        /* Initialize server for the IrDA Transport */
        if ((status = IRSTACK_ServerInit(&ObexApp->trans.ObexServerIrTrans)) != OB_STATUS_SUCCESS)
            goto InitFailure;
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* IRDA_STACK == XA_ENABLED */

#if BT_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_BLUETOOTH) {
        /* Initialize client for the Bluetooth Transport */
        if ((status = BTSTACK_ClientInit(&ObexApp->trans.ObexClientBtTrans)) != OB_STATUS_SUCCESS)
            goto InitFailure;
    }
#endif /* BT_STACK == XA_ENABLED */

#if TCP_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_TCP) {
        /* Initialize client for the TCP Transport */
        if ((status = TCPSTACK_ClientInit(&ObexApp->trans.ObexClientTcpTrans)) != OB_STATUS_SUCCESS)
            goto InitFailure;
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        /* Initialize server for the TCP Transport */
        if ((status = TCPSTACK_ServerInit(&ObexApp->trans.ObexServerTcpTrans)) != OB_STATUS_SUCCESS)
            goto InitFailure;
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* TCP_STACK == XA_ENABLED */
    
#if OEM_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_OEM) {
        /* Initialize client for the OEM Transport */
        if ((status = OEMSTACK_ClientInit(&ObexApp->trans.ObexClientOemTrans, &ObexApp->handle)) != OB_STATUS_SUCCESS)
            goto InitFailure;
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        /* Initialize server for the OEM Transport */
        if ((status = OEMSTACK_ServerInit(&ObexApp->trans.ObexServerOemTrans, &ObexApp->handle)) != OB_STATUS_SUCCESS)
            goto InitFailure;
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* OEM_STACK == XA_ENABLED */
    
    /* Insert this entry into the App Handle List */
    InsertTailList(&OBD(clientAppHndlList), &(ObexApp->handle.node));

    return OB_STATUS_SUCCESS;

InitFailure:
#if OBEX_DEINIT_FUNCS == XA_ENABLED

#if IRDA_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_IRDA) {
        IRSTACK_ClientDeinit(&ObexApp->trans.ObexClientIrTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        IRSTACK_ServerDeinit(&ObexApp->trans.ObexServerIrTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* IRDA_STACK == XA_ENABLED */

#if BT_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_BLUETOOTH) {
        BTSTACK_ClientDeinit(&ObexApp->trans.ObexClientBtTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        BTSTACK_ServerDeinit(&ObexApp->trans.ObexServerBtTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* BT_STACK == XA_ENABLED */

#if TCP_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_TCP) {
        TCPSTACK_ClientDeinit(&ObexApp->trans.ObexClientTcpTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        TCPSTACK_ServerDeinit(&ObexApp->trans.ObexServerTcpTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* TCP_STACK == XA_ENABLED */

#if OEM_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_OEM) {
        OEMSTACK_ClientDeinit(&ObexApp->trans.ObexClientOemTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        OEMSTACK_ServerDeinit(&ObexApp->trans.ObexServerOemTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* OEM_STACK == XA_ENABLED */
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

    return status;
    
}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBSTACK_ClientDeinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  DeInitialize the OBEX Client transport(s). All transports
 *            must deinitialize successfully or the function
 *            returns failure.
 *
 * Return:    TRUE -  Client stack deinitialized.
 *            FALSE - Client cannot deinitialize.
 */
BOOL OBSTACK_ClientDeinit(ObexClientApp *ObexApp)
{
    BOOL btStatus = TRUE;
    BOOL irStatus = TRUE;
    BOOL tcpStatus = TRUE;
    BOOL oemStatus = TRUE;

    ASSERT(IsObexLocked());

    if (OBD(initialized) != TRUE) {
        /* OBEX is not initialized, so there is nothing to deinit */
        return TRUE;
    }

    /* Deinitialize the transports for this client */
#if IRDA_STACK == XA_ENABLED
    irStatus &= IRSTACK_ClientDeinit(&ObexApp->trans.ObexClientIrTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    irStatus &= IRSTACK_ServerDeinit(&ObexApp->trans.ObexServerIrTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
#endif /* IRDA_STACK == XA_ENABLED */

#if BT_STACK == XA_ENABLED
    btStatus &= BTSTACK_ClientDeinit(&ObexApp->trans.ObexClientBtTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    btStatus &= BTSTACK_ServerDeinit(&ObexApp->trans.ObexServerBtTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
#endif /* BT_STACK == XA_ENABLED */

#if TCP_STACK == XA_ENABLED
    tcpStatus &= TCPSTACK_ClientDeinit(&ObexApp->trans.ObexClientTcpTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    tcpStatus &= TCPSTACK_ServerDeinit(&ObexApp->trans.ObexServerTcpTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
#endif /* TCP_STACK == XA_ENABLED */

#if OEM_STACK == XA_ENABLED
    oemStatus &= OEMSTACK_ClientDeinit(&ObexApp->trans.ObexClientOemTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    oemStatus &= OEMSTACK_ServerDeinit(&ObexApp->trans.ObexServerOemTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
#endif /* OEM_STACK == XA_ENABLED */

    /* Only remove the App Handle if we successfully disconnected 
     * the transport.
     */
    if (btStatus && irStatus && tcpStatus && oemStatus) {
        /* Remove this entry from the App Handle List */
        ASSERT(!IsListEmpty(&OBD(clientAppHndlList)));
        RemoveEntryList(&(ObexApp->handle.node));
        /* Reset the current client App Handle node */
        OBD(curClientNode) = &(OBD(clientAppHndlList));
    }

    return btStatus && irStatus && tcpStatus && oemStatus;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBSTACK_ServerInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Server Transport(s). All requested
 *            transports must initialize successfully or the function
 *            returns failure.  Multiple servers can be registered if 
 *            necessary.
 *
 * Return:    OB_STATUS_SUCCESS - Requested protocols initialized successfully
 *            OB_STATUS_FAILED - Unable to initialize a requested transport adapter.
 *            OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBSTACK_ServerInit(ObexServerApp *ObexApp, ObexTpType Protocols)
{
    ObStatus    status = OB_STATUS_INVALID_PARM;

    ASSERT(IsObexLocked());

#if XA_ERROR_CHECK == XA_ENABLED
    if (OBD(initialized) != TRUE) {
        /* OBEX is not initialized */
        return OB_STATUS_FAILED;
    }

    if ((OBSTACK_VerifyServerAppHandle(&ObexApp->handle)) ||
       ((Protocols & OBEX_EnumTransports()) != Protocols) ||
       (Protocols == OBEX_TP_NONE)) {
           return status;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT((Protocols & OBEX_EnumTransports()) && Protocols
           && !OBSTACK_VerifyServerAppHandle(&ObexApp->handle));

#if IRDA_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_IRDA) {
        if ((status = IRSTACK_ServerInit(&ObexApp->trans.ObexServerIrTrans)) != OB_STATUS_SUCCESS) {
            goto InitFailure;
        }
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        if ((status = IRSTACK_ClientInit(&ObexApp->trans.ObexClientIrTrans)) != OB_STATUS_SUCCESS) {
            goto InitFailure;
        }
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* IRDA_STACK == XA_ENABLED */

#if BT_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_BLUETOOTH) {
        if ((status = BTSTACK_ServerInit(&ObexApp->trans.ObexServerBtTrans)) != OB_STATUS_SUCCESS) {
            goto InitFailure;
        }
    }
#endif /* BT_STACK == XA_ENABLED */

#if TCP_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_TCP) {
        if ((status = TCPSTACK_ServerInit(&ObexApp->trans.ObexServerTcpTrans)) != OB_STATUS_SUCCESS) {
            goto InitFailure;
        }
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        if ((status = TCPSTACK_ClientInit(&ObexApp->trans.ObexClientTcpTrans)) != OB_STATUS_SUCCESS) {
            goto InitFailure;
        }
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* TCP_STACK == XA_ENABLED */

#if OEM_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_OEM) {
        if ((status = OEMSTACK_ServerInit(&ObexApp->trans.ObexServerOemTrans, &ObexApp->handle)) != OB_STATUS_SUCCESS) {
            goto InitFailure;
        }
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        if ((status = OEMSTACK_ClientInit(&ObexApp->trans.ObexClientOemTrans, &ObexApp->handle)) != OB_STATUS_SUCCESS) {
            goto InitFailure;
        }
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* OEM_STACK == XA_ENABLED */

    /* Insert this entry into the App Handle List */
    InsertTailList(&OBD(serverAppHndlList), &(ObexApp->handle.node));

    return OB_STATUS_SUCCESS;

InitFailure:
#if OBEX_DEINIT_FUNCS == XA_ENABLED

#if IRDA_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_IRDA) {
        IRSTACK_ServerDeinit(&ObexApp->trans.ObexServerIrTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        IRSTACK_ClientDeinit(&ObexApp->trans.ObexClientIrTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* IRDA_STACK == XA_ENABLED */

#if BT_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_BLUETOOTH) {
        BTSTACK_ServerDeinit(&ObexApp->trans.ObexServerBtTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        BTSTACK_ClientDeinit(&ObexApp->trans.ObexClientBtTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* BT_STACK == XA_ENABLED */

#if TCP_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_TCP) {
        TCPSTACK_ServerDeinit(&ObexApp->trans.ObexServerTcpTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        TCPSTACK_ClientDeinit(&ObexApp->trans.ObexClientTcpTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* TCP_STACK == XA_ENABLED */

#if OEM_STACK == XA_ENABLED
    if (Protocols & OBEX_TP_OEM) {
        OEMSTACK_ServerDeinit(&ObexApp->trans.ObexServerOemTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
        OEMSTACK_ClientDeinit(&ObexApp->trans.ObexClientOemTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
    }
#endif /* OEM_STACK == XA_ENABLED */

#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

    return status;
}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBSTACK_ServerDeinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Server transport(s). All transports
 *            must deinitialize successfully or the function returns
 *            failure.
 *
 * Return:    TRUE -  Server stack deinitialized.
 *            FALSE - Server cannot deinitialize.
 */
BOOL OBSTACK_ServerDeinit(ObexServerApp *ObexApp)
{
    BOOL btStatus = TRUE;
    BOOL irStatus = TRUE;
    BOOL tcpStatus = TRUE;
    BOOL oemStatus = TRUE;

    ASSERT(IsObexLocked());

    if (OBD(initialized) != TRUE) {
        /* OBEX is not initialized, so there is nothing to deinit */
        return TRUE;
    }

#if IRDA_STACK == XA_ENABLED
    irStatus &= IRSTACK_ServerDeinit(&ObexApp->trans.ObexServerIrTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    irStatus &= IRSTACK_ClientDeinit(&ObexApp->trans.ObexClientIrTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
#endif /* IRDA_STACK == XA_ENABLED */

#if BT_STACK == XA_ENABLED
    btStatus &= BTSTACK_ServerDeinit(&ObexApp->trans.ObexServerBtTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    btStatus &= BTSTACK_ClientDeinit(&ObexApp->trans.ObexClientBtTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
#endif /* BT_STACK == XA_ENABLED */

#if TCP_STACK == XA_ENABLED
    tcpStatus &= TCPSTACK_ServerDeinit(&ObexApp->trans.ObexServerTcpTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    tcpStatus &= TCPSTACK_ClientDeinit(&ObexApp->trans.ObexClientTcpTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
#endif /* TCP_STACK == XA_ENABLED */

#if OEM_STACK == XA_ENABLED
    oemStatus &= OEMSTACK_ServerDeinit(&ObexApp->trans.ObexServerOemTrans);
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    oemStatus &= OEMSTACK_ClientDeinit(&ObexApp->trans.ObexClientOemTrans);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
#endif /* OEM_STACK == XA_ENABLED */

    /* Only remove the App Handle if we successfully disconnected 
     * the transports.
     */
    if (btStatus && irStatus && tcpStatus && oemStatus) {
        /* Remove this entry from the App Handle List */
        ASSERT(!IsListEmpty(&OBD(serverAppHndlList)));
        RemoveEntryList(&(ObexApp->handle.node));
        /* Reset the current server App Handle node */
        OBD(curServerNode) = &(OBD(serverAppHndlList));
    }

    return btStatus && irStatus && tcpStatus && oemStatus;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            OBSTACK_IsConnectionUp()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Check if the transport is connected. This function can get all
 *            of its information directly from the ObexTransport structure that
 *            relates to the transport itself.
 *
 * Return:    TRUE - Transport is connected.
 *            FALSE - Transport is not connected.
 */
BOOL OBSTACK_IsConnectionUp(ObexTransport *con)
{
    ASSERT(IsObexLocked());

    if (!con) {
        return FALSE;
    }

    return (*(con->connected) == OCS_CONNECTED);
}

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBSTACK_VerifyClientAppHandle()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Determines whether ObexApp is a registered client app handle.
 *
 * Return:    Status of ObexApp in our list of registered app handles.
 */
BOOL OBSTACK_VerifyClientAppHandle(ObexAppHandle *ObexApp)
{
    return IsNodeOnList(&OBD(clientAppHndlList), &ObexApp->node);
}

/*---------------------------------------------------------------------------
 *            OBSTACK_ClientSRMAbort()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Flush the preivous SRM data.
 *
 * Return:    Status of Flush.
 */
ObStatus OBSTACK_ClientSRMAbort(ObexClientApp *ObexApp){
    ObStatus btStatus = BT_STATUS_FAILED; 
#if BT_STACK == XA_ENABLED
    btStatus &= BTSTACK_ClientFlushChannel(&ObexApp->trans.ObexClientBtTrans);
#endif
    return 0;
}
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBSTACK_ServerSRMAbort()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Flush the preivous SRM data.
 *
 * Return:    Status of Flush.
 */
ObStatus OBSTACK_ServerSRMAbort(ObexServerApp *ObexApp){
    ObStatus btStatus = BT_STATUS_FAILED; 
#if BT_STACK == XA_ENABLED
    btStatus &= BTSTACK_ServerFlushChannel(&ObexApp->trans.ObexServerBtTrans);
#endif
    return 0;
}

/*---------------------------------------------------------------------------
 *            OBSTACK_VerifyServerAppHandle()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Determines whether ObexApp is a registered server app handle.
 *
 * Return:    Status of ObexApp in our list of registered app handles.
 */
BOOL OBSTACK_VerifyServerAppHandle(ObexAppHandle *ObexApp)
{
    return IsNodeOnList(&OBD(serverAppHndlList), &ObexApp->node);
}
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            OBSTACK_GetTxBuffer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to a buffer that can be used for sending
 *            OBEX data.
 *
 * Return:    Pointer to transmit buffer.
 */
U8 *OBSTACK_GetTxBuffer(void)
{
    ObexPacket *txPacket;

    ASSERT(IsObexLocked());

    if (!IsListEmpty(&(OBD(packetList)))) {
        txPacket = (ObexPacket *)RemoveHeadList(&(OBD(packetList)));
        /* This assert is to test the ContainingRecord() Macro. Some
         * compilers have shown problems with implementing it correctly.
         */
        ASSERT(txPacket == ContainingRecord((txPacket->buffer), ObexPacket, buffer));
        OBD(packetnum)--;
        return (txPacket->buffer);
    }
    return 0;

}

U8 OBSTACK_IsTxBufferAvailable(void)
{
    if (!IsListEmpty(&(OBD(packetList)))) {
        return TRUE;
    }
    return FALSE;
}

/*---------------------------------------------------------------------------
 *            OBPROT_TxDone()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Processes completion of a transmitted packet.
 *            Return a packet to the packet pool
 *
 * Return:    void
 */
void OBPROT_TxDone(ObexAppHandle *AppHndl, ObexPacket *packet)
{
    ObexAppHandle *currHandle;
    BOOL      parsedClients = FALSE;
    BOOL      parsedServers = FALSE;
    ListEntry *begClientNode = OBD(curClientNode);
    ListEntry *begServerNode = OBD(curServerNode);

    ASSERT(IsObexLocked());
    ASSERT(AppHndl);

    /* Return data packet to pool */
    InsertTailList(&(OBD(packetList)), &(packet->pkt.node));
    OBD(packetnum)++;
        
    ASSERT(IsObexLocked());

    /* First, offer the free buffer to the parsers.  If our last parsing was
     * of a client, then pass the buffer to a server.  Likewise, if our last
     * parsing was of a server, pass the buffer to a client.  We will
     * continue until the packetList is empty, or until all clients and servers
     * have been exhausted.  The stopping place for both the client and server
     * lists will be saved, so we can return to that spot later.
     */
    do {
        if (IsListEmpty(&(OBD(clientAppHndlList))) && 
             IsListEmpty(&(OBD(serverAppHndlList)))) {
            /* If our lists of clients and servers are empty, 
             * pass the buffer back to the current app handle.
             */
            ObParserTxBuffAvail(AppHndl);
            break;
        }

        if (IsClientParser(&AppHndl->parser)) {
            /* Last packet was from a client - give the packet to a server if possible, 
             * otherwise give it to a client.
             */
            if (!IsListEmpty(&(OBD(serverAppHndlList))) && !parsedServers) {
                /* Pass to server */
                if (OBD(curServerNode) == &(OBD(serverAppHndlList))) {
                    /* Skip the head of the list and move to one of the elements */
                    OBD(curServerNode) = OBD(curServerNode)->Flink;
                }
        
                currHandle = (ObexAppHandle *)(OBD(curServerNode));
                /* Advance the current node pointer */
                OBD(curServerNode) = currHandle->node.Flink;

                /* We have gone through all the servers */
                if (OBD(curServerNode) == begServerNode) 
					parsedServers = TRUE;
            }
            else if (!IsListEmpty(&(OBD(clientAppHndlList))) && !parsedClients) {
                /* There must be a client available then */
                if (OBD(curClientNode) == &(OBD(clientAppHndlList))) {
                    /* Skip the head of the list and move to one of the elements */
                    OBD(curClientNode) = OBD(curClientNode)->Flink;
                }
        
                currHandle = (ObexAppHandle *)(OBD(curClientNode));
                /* Advance the current node pointer */
                OBD(curClientNode) = currHandle->node.Flink;

                /* We have gone through all the clients */
                if (OBD(curClientNode) == begClientNode) 
					parsedClients = TRUE;
            }
            else {
                /* Clients and servers are either parsed, or the lists are empty.
                 * We're done.
                 */
                break;
            }
        }
        else {
            /* Last packet was from a server - give the packet to a client if possible, 
             * otherwise give it to a server.
             */
            if (!IsListEmpty(&(OBD(clientAppHndlList))) && !parsedClients) {
                /* Pass to client */
                if (OBD(curClientNode) == &(OBD(clientAppHndlList))) {
                    /* Skip the head of the list and move to one of the elements */
                    OBD(curClientNode) = OBD(curClientNode)->Flink;
                }
        
                currHandle = (ObexAppHandle *)(OBD(curClientNode));
                /* Advance the current node pointer */
                OBD(curClientNode) = currHandle->node.Flink;

                /* We have gone through all the clients */
                if (OBD(curClientNode) == begClientNode) 
					parsedClients = TRUE;
            }
            else if (!IsListEmpty(&(OBD(serverAppHndlList))) && !parsedServers) {
                /* There must be a server available then */
                if (OBD(curServerNode) == &(OBD(serverAppHndlList))) {
                    /* Skip the head of the list and move to one of the elements */
                    OBD(curServerNode) = OBD(curServerNode)->Flink;
                }
        
                currHandle = (ObexAppHandle *)(OBD(curServerNode));
                /* Advance the current node pointer */
                OBD(curServerNode) = currHandle->node.Flink;

                /* We have gone through all the servers */
                if (OBD(curServerNode) == begServerNode) 
					parsedServers = TRUE;

            }
            else {
                /* Clients and servers are either parsed, or the lists are empty.
                 * We're done.
                 */
                break;
            }
        }

		if (currHandle->tpStatus == OB_TP_CONNECTED)
		{
            ObParserTxBuffAvail(currHandle);
		}
        
        /* If we ran out of packets, quit */
        if (IsListEmpty(&(OBD(packetList)))) 
        {
		    //kal_trace(BT_TRACE_G6_OBEX,OBEX_NO_FREE_PACKET);
            break;
        }
   
    } while (!parsedServers || !parsedClients);
    
    /* Now tell the sender that the transmit is complete. */
    ObParserTxDone(AppHndl);
}

/*---------------------------------------------------------------------------
 *            OBPROT_ReturnTxBuffer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns packet to the packet pool. Usually called when a 
 *            transmit request fails.
 *
 * Return:    void
 */
void OBPROT_ReturnTxBuffer(U8 *buff)
{
    ObexPacket *obp = ContainingRecord(buff, ObexPacket, buffer);

    ASSERT(IsObexLocked());

    /* Return data packet to pool */
    InsertTailList(&(OBD(packetList)), &(obp->pkt.node));
    OBD(packetnum)++;
}

/*---------------------------------------------------------------------------
 *            OBPROT_Connected()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function is called by the transport to
 *            notify the OBEX protocol that a transport connection has
 *            been established.
 *
 * Return:    void
 */
void OBPROT_Connected(ObexAppHandle *AppHndl)
{
    void*       ObexApp = AppHndl;

    ASSERT(IsObexLocked());
    ASSERT(AppHndl);

    /* The transport is in fact connected */
    AppHndl->tpStatus = OB_TP_CONNECTED;  /* Connected */

    if (IsServerParser(&AppHndl->parser)) {
#if OBEX_ROLE_SERVER == XA_ENABLED
        ASSERT(OSC(appCallback));
        /* Assign the OBEX client to the callback parms */
        OBD(serverParms).server = ObexApp;
        /* Assign the OBEX client event */
        OBD(serverParms).event = OBSE_CONNECTED;
        OSC(appCallback)(&OBD(serverParms));
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */
    }
#if OBEX_ROLE_CLIENT == XA_ENABLED
    else {
        /* ASSERT detects deinit() during disconnect ind, very bad. */
        ASSERT(OCC(appCallback));
        /* Assign the OBEX client to the callback parms */
        OBD(clientParms).client = ObexApp;
        /* Assign the OBEX client event */
        OBD(clientParms).event = OBCE_CONNECTED;
        OCC(appCallback)(&OBD(clientParms));
    }
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
}

/*---------------------------------------------------------------------------
 *            OBPROT_Disconnected()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function is called by the transport to notify the
 *            OBEX protocol that the transport connection has been
 *            disconnected. It is used to notify OBEX of asynchronous
 *            disconnects as well as the completion of a disconnect request
 *            call. This function automatically unlinks the transport
 *            from the OBEX protocol.
 *
 * Return:    void
 */
void OBPROT_Disconnected(ObexAppHandle *AppHndl, U8 Reason)
{
    void *ObexApp = AppHndl;

    ASSERT(IsObexLocked());
    ASSERT(AppHndl);

    /* The transport is in fact disconnected */
    AppHndl->tpStatus = OB_TP_DISCONNECTED;  /* Disconnected */
    /* Unassign the client transport type */
    AppHndl->tpType = OBEX_TP_NONE;

    if (IsClientParser(&AppHndl->parser)) {
        /* Assign the OBEX client to the callback parms */
        OBD(clientParms).client = ObexApp;
    }
#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
    else {
        /* Assign the OBEX server to the callback parms */
        OBD(serverParms).server = ObexApp;
    }
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */

#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_DISABLED
    if (IsServerParser(&AppHndl->parser)) {
        /* Abort the current operation. */
        ObParserDisconnectInd(AppHndl);
        return;
    }
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_DISABLED */

    /* We break the link between parser and transport anyway */
    ObParserUnlinkTransport(&AppHndl->parser);
    
    if ((Reason == ODR_NO_SERVICE_FOUND) || (Reason == ODR_DISCOVERY_FAILED)) {
        if (IsClientParser(&AppHndl->parser)) {
#if OBEX_ROLE_CLIENT == XA_ENABLED
            /* Completing a ClientTpConnect() request */
            /* Assign the OBEX client event */
            if (Reason == ODR_NO_SERVICE_FOUND)
                OBD(clientParms).event = OBCE_NO_SERVICE_FOUND;
            else OBD(clientParms).event = OBCE_DISCOVERY_FAILED;
            OCC(appCallback)(&OBD(clientParms));
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
        }
#if (OBEX_ROLE_SERVER == XA_ENABLED) && (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
        else {
            /* Completing a ServerTpConnect() request */
            /* Assign the OBEX server event */
            if (Reason == ODR_NO_SERVICE_FOUND)
                OBD(serverParms).event = OBSE_NO_SERVICE_FOUND;
            else OBD(serverParms).event = OBSE_DISCOVERY_FAILED;
            OSC(appCallback)(&OBD(serverParms));
        }
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) && (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */
    } else if (Reason == ODR_SCO_REJECT) {
            OBD(clientParms).event = OBCE_SCO_REJECT;
	     OCC(appCallback)(&OBD(clientParms));
     } else if (Reason == ODR_BTCHIP_REJECT) {
             OBD(clientParms).event = OBCE_BTCHIP_REJECT;
          OCC(appCallback)(&OBD(clientParms));
    } else {
        /* Abort the current operation. */
        ObParserDisconnectInd(AppHndl);

        if (IsClientParser(&AppHndl->parser)) {
#if OBEX_ROLE_CLIENT == XA_ENABLED
            if(OCC(appCallback))
            {
                OBD(clientParms).event = OBCE_DISCONNECT;
                OCC(appCallback)(&OBD(clientParms));
             }
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
        }
    }
}

/*---------------------------------------------------------------------------
 *            OBPROT_LinkTransport()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function is called by the transport to
 *            register itself with the OBEX protocol layer. It is usually
 *            called when a connection indication has been received. The
 *            transport must call OBPROT_Connected() when the
 *            transport connection is complete. OBEX will not call any of
 *            the provided ObTransFuncTable functions until a connection is
 *            indicated.
 *
 * Return:    TRUE -  If transport was linked to OBEX protocol.
 *
 *            FALSE - If transport was not linked because the
 *                    protocol already has a connection.
 */
BOOL OBPROT_LinkTransport(ObexAppHandle *AppHndl, ObexTransport *Trans)
{
    ASSERT(IsObexLocked());
    ASSERT(AppHndl && Trans);

    return ObParserLinkTransport(&AppHndl->parser, Trans);
}

/*---------------------------------------------------------------------------
 *            OBPROT_UnlinkTransport()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function is called by the transport to deregister 
 *            itself from the OBEX protocol layer. This function
 *            is not commonly used because the OBPROT_Disconnect() function
 *            automatically performs an unlink. However, in cases where the
 *            transport needs to unlink without generating a 
 *            disconnect this function can be used.
 *
 * Return:    void
 */
void OBPROT_UnlinkTransport(ObexAppHandle *AppHndl)
{
    ASSERT(IsObexLocked());
    ASSERT(AppHndl);

    ObParserUnlinkTransport(&AppHndl->parser);
}

/*---------------------------------------------------------------------------
 *            OBPROT_SetInitState()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function is used by the transport to reset the OBEX
 *            protocol parser. This function is optional because in most
 *            normal cases initialization is performed internally.
 *
 * Return:    void
 */
void OBPROT_SetInitState(ObexAppHandle *AppHndl)
{
    ASSERT(IsObexLocked());
    ASSERT(AppHndl);

    ObParserSetInitState(&AppHndl->parser);
}

/*---------------------------------------------------------------------------
 *            OBPROT_ParseRx()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function is used to indicate received data to the OBEX 
 *            protocol.
 *
 * Return:    void
 */
void OBPROT_ParseRx(ObexAppHandle *AppHndl, U8 *rxBuff, U16 rxLen)
{
    ASSERT(IsObexLocked());
    ASSERT(AppHndl);

    ObParseRx(AppHndl, rxBuff, rxLen);
}

#if OBEX_SUPPORT_TIMEOUT == XA_ENABLED

extern void BTSTACK_TxTimeout(void *con);

#if 0 // MAUI w0946
/*---------------------------------------------------------------------------
 *            OBPROT_GivePktToWaitingApp_IfNeed()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Processes completion of a transmitted packet.
 *            Return a packet to the packet pool
 *
 * Return:    void
 */
void OBPROT_GivePktToWaitingApp_IfNeed(ObexAppHandle *AppHndl)
{
    ObexAppHandle *currHandle;
    BOOL      parsedClients = FALSE;
    BOOL      parsedServers = FALSE;
    ListEntry *begClientNode = OBD(curClientNode);
    ListEntry *begServerNode = OBD(curServerNode);

    ASSERT(IsObexLocked());


    /* First, offer the free buffer to the parsers.  If our last parsing was
     * of a client, then pass the buffer to a server.  Likewise, if our last
     * parsing was of a server, pass the buffer to a client.  We will
     * continue until the packetList is empty, or until all clients and servers
     * have been exhausted.  The stopping place for both the client and server
     * lists will be saved, so we can return to that spot later.
     */
    do {
        if (IsListEmpty(&(OBD(clientAppHndlList))) && 
             IsListEmpty(&(OBD(serverAppHndlList)))) {
            /* If our lists of clients and servers are empty, 
             * pass the buffer back to the current app handle.
             */
            break;
        }

        if (AppHndl && IsClientParser(&AppHndl->parser)) {
            /* Last packet was from a client - give the packet to a server if possible, 
             * otherwise give it to a client.
             */
            if (!IsListEmpty(&(OBD(serverAppHndlList))) && !parsedServers) {
                /* Pass to server */
                if (OBD(curServerNode) == &(OBD(serverAppHndlList))) {
                    /* Skip the head of the list and move to one of the elements */
                    OBD(curServerNode) = OBD(curServerNode)->Flink;
                }
        
                currHandle = (ObexAppHandle *)(OBD(curServerNode));
                /* Advance the current node pointer */
                OBD(curServerNode) = currHandle->node.Flink;

                /* We have gone through all the servers */
                if (OBD(curServerNode) == begServerNode) 
					parsedServers = TRUE;
            }
            else if (!IsListEmpty(&(OBD(clientAppHndlList))) && !parsedClients) {
                /* There must be a client available then */
                if (OBD(curClientNode) == &(OBD(clientAppHndlList))) {
                    /* Skip the head of the list and move to one of the elements */
                    OBD(curClientNode) = OBD(curClientNode)->Flink;
                }
        
                currHandle = (ObexAppHandle *)(OBD(curClientNode));
                /* Advance the current node pointer */
                OBD(curClientNode) = currHandle->node.Flink;

                /* We have gone through all the clients */
                if (OBD(curClientNode) == begClientNode) 
					parsedClients = TRUE;
            }
            else {
                /* Clients and servers are either parsed, or the lists are empty.
                 * We're done.
                 */
                break;
            }
        }
        else {
            /* Last packet was from a server - give the packet to a client if possible, 
             * otherwise give it to a server.
             */
            if (!IsListEmpty(&(OBD(clientAppHndlList))) && !parsedClients) {
                /* Pass to client */
                if (OBD(curClientNode) == &(OBD(clientAppHndlList))) {
                    /* Skip the head of the list and move to one of the elements */
                    OBD(curClientNode) = OBD(curClientNode)->Flink;
                }
        
                currHandle = (ObexAppHandle *)(OBD(curClientNode));
                /* Advance the current node pointer */
                OBD(curClientNode) = currHandle->node.Flink;

                /* We have gone through all the clients */
                if (OBD(curClientNode) == begClientNode) 
					parsedClients = TRUE;
            }
            else if (!IsListEmpty(&(OBD(serverAppHndlList))) && !parsedServers) {
                /* There must be a server available then */
                if (OBD(curServerNode) == &(OBD(serverAppHndlList))) {
                    /* Skip the head of the list and move to one of the elements */
                    OBD(curServerNode) = OBD(curServerNode)->Flink;
                }
        
                currHandle = (ObexAppHandle *)(OBD(curServerNode));
                /* Advance the current node pointer */
                OBD(curServerNode) = currHandle->node.Flink;

                /* We have gone through all the servers */
                if (OBD(curServerNode) == begServerNode) 
					parsedServers = TRUE;

            }
            else {
                /* Clients and servers are either parsed, or the lists are empty.
                 * We're done.
                 */
                break;
            }
        }
        kal_trace(BT_TRACE_G6_OBEX, OBEX_GIVEPKT_TO_WAITINGAPP, currHandle);
        ObParserTxBuffAvail(currHandle);
        
        /* If we ran out of packets, quit */
        if (IsListEmpty(&(OBD(packetList)))) 
        {
		    //kal_trace(BT_TRACE_G6_OBEX,OBEX_NO_FREE_PACKET);
            break;
        }
   
    } while (!parsedServers || !parsedClients);
}


BOOL OBPROT_ReturnTxBuffer_IfNeed(void *con)
{
    int i;
    BOOL rst = FALSE;

    for (i = 0; i < OBEX_NUM_TX_PACKETS; i++)
    {
        if (OBD(p_sending_pkt)[i].con == con)
        {     
            if (OBD(p_sending_pkt)[i].obxPacket != NULL)
            {
                kal_trace(BT_TRACE_G6_OBEX, OBEX_RETURN_PENDING_PKT, con,OBD(p_sending_pkt)[i].obxPacket->buffer);
                //Report(("[OBEX] OBPROT_ReturnTxBuffer_IfNeed: return Tx Buffer index i=(%d)", i));
                /* Return data packet to pool */
                OBPROT_ReturnTxBuffer(OBD(p_sending_pkt)[i].obxPacket->buffer);
            
                OBD(p_sending_pkt)[i].con = NULL;
                OBD(p_sending_pkt)[i].obxPacket = NULL;

                rst = TRUE;
                break;
            }
        }
    }

    return rst;
}
#endif

void OBPROT_TxTimeout(void *param)
{
    ObexPacket *obxPkt = (ObexPacket *)((EvmTimer *)param)->context;
    int i;

    kal_trace(BT_TRACE_G6_OBEX, OBEX_OBPROT_TXTIMEOUT, obxPkt);
    for (i = 0; i < OBEX_NUM_TX_PACKETS; i++)
    {
        if (OBD(p_sending_pkt)[i].obxPacket == obxPkt)
        {
			kal_trace(BT_TRACE_G6_OBEX, OBEX_TX_TIMEOUT_INDEX, i);
			
            BTSTACK_TxTimeout(OBD(p_sending_pkt)[i].con);
            
             OBD(p_sending_pkt)[i].timerStart = 0; //has been launch
	     if( 0 != OBD(p_sending_pkt)[i].boundTest){
	     	ASSERT(0);
	     }
            break;
        }
    }
}

void OBPROT_StartTxTimer(void *con, ObexPacket *obxPkt)
{
   int i;

   for (i = 0; i < OBEX_NUM_TX_PACKETS; i++)
   {
       //if (OBD(p_sending_pkt)[i].timerId == 0)
       if ( 1 != OBD(p_sending_pkt)[i].timerStart ) // timer started
       {
           OBD(p_sending_pkt)[i].con = con;
           OBD(p_sending_pkt)[i].obxPacket = obxPkt;
           GOEP_Report("[OBEX][Timer] OBPROT_StartTxTimer con=0x%x obxpkt=0x%x", OBD(p_sending_pkt)[i].con, OBD(p_sending_pkt)[i].obxPacket);
	       obex_start_timer(&(OBD(p_sending_pkt)[i].timerId), 
		             OBEX_RFCOMM_PACKET_TIMER_DUR,
		             OBPROT_TxTimeout,
		             obxPkt);
              OBD(p_sending_pkt)[i].timerStart = 1;
	     if( 0 != OBD(p_sending_pkt)[i].boundTest){
	     	ASSERT(0);
	     }              
           break;
       }
   }
}

void OBPROT_StopTxTimer(ObexPacket *obxPkt)
{
    int i;

    for (i = 0; i < OBEX_NUM_TX_PACKETS; i++)
    {
        if (OBD(p_sending_pkt)[i].obxPacket == obxPkt)
        {
            if ( 1 == OBD(p_sending_pkt)[i].timerStart )
            {
                GOEP_Report("[OBEX][Timer] OBPROT_StopTxTimer con=0x%x obxpkt=0x%x", OBD(p_sending_pkt)[i].con, OBD(p_sending_pkt)[i].obxPacket);
                obex_stop_timer(&(OBD(p_sending_pkt)[i].timerId));
                OBD(p_sending_pkt)[i].timerStart = 0;
            }
            OBD(p_sending_pkt)[i].con = NULL;
            OBD(p_sending_pkt)[i].obxPacket = NULL;
	     if( 0 != OBD(p_sending_pkt)[i].boundTest){
	     	ASSERT(0);
	     }
            break;
        }
    }
}
#endif

