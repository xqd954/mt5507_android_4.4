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
 * File:        obxirstk.c
 *
 * Description: This file contains code for the IrDA transport portion
 *              of the OBEX layer.
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
 ****************************************************************************/
#include <obex.h>
#include <sys/obxirstk.h>
#include <sys/obxalloc.h>

#if defined(IRDA_STACK) && IRDA_STACK == XA_ENABLED

/****************************************************************************
 *
 * Internal Function prototypes
 *
 ***************************************************************************/
#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
static void      ObServerCallBack(IrConnect* con, IrCallBackParms* parms);
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
static void      LsapQueryCallBack(IrStatus status);
static void      ObClientCallBack(IrConnect* con, IrCallBackParms* parms);
static IrStatus  ObStartLsapQuery(ObIrClientTransport *irxp);
static void      DiscoveryTimeout(EvmTimer *Timer);
static IrDeviceAddr FindObexDevice(IrDeviceList* list);
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED)
static BOOL IsLmConnected(IrConnect *con);
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED) */

static void      ObTxDone(ObexAppHandle *AppHndl, IrPacket *packet);
static IrPacket *ObGetConnMgmtPacket(ObexAppHandle *AppHndl);

/* Functions used in ObTransFuncTable */
static ObStatus IRSTACK_DisconnectReq(ObexTransportContext context);
static U16      IRSTACK_MaxTxSize(ObexTransportContext context);
static ObStatus IRSTACK_SendTxBuffer(ObexTransportContext context, U8 *buff, U16 len);

static BOOL IRSTACK_GetTpConnInfo(ObexTransportContext context, 
                                  ObexTpConnInfo *tpConnInfo);
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
static BOOL IRSTACK_IsRxFlowAvailable(ObexTransportContext context);
static void IRSTACK_PauseRxFlow(ObexTransportContext context);
static void IRSTACK_ResumeRxFlow(ObexTransportContext context);
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

/****************************************************************************
 *
 * OBEX IrDA Transport RAM data
 *
 ****************************************************************************/

/****************************************************************************
 *
 * OBEX IrDA Transport ROMable data
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 *
 * IrDA Transport function call table
 */
const ObTransFuncTable IrStkFuncTable = {
    IRSTACK_DisconnectReq, IRSTACK_MaxTxSize, 
    IRSTACK_SendTxBuffer, IRSTACK_GetTpConnInfo,
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
    IRSTACK_IsRxFlowAvailable, IRSTACK_PauseRxFlow, IRSTACK_ResumeRxFlow
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
};

/****************************************************************************
 *
 * OBEX Client "IrDA Stack specific" Application Programming Interface
 *
 ****************************************************************************/

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            IRSTACK_ClientConnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Start a Tiny TP connection to the selected device.
 *
 * Return:    OB_STATUS_PENDING - operation is started successfully
 *            OB_STATUS_SUCCESS - operation is successful
 *            OB_STATUS_BUSY - operation failed because the client is busy.
 *            OB_STATUS_MEDIA_BUSY - operation failed because the media
 *            is busy. Retry the request later.
 *            OB_STATUS_FAILED - operation failed because the IAS query
 *            failed to start. Retry the request later.
 */
ObStatus IRSTACK_ClientConnect( struct _ObexIrTarget *DevAddr,
                                ObIrClientTransport  *irxp,
                                ObexTransport          **Trans)
{
    ObStatus status = OB_STATUS_BUSY;

    ASSERT(DevAddr);

    *Trans = &irxp->client.transport;

    if (irxp->client.state == OTS_CONNECTED) {
        /* The client is already connected */
        return OB_STATUS_SUCCESS;
    } 
    
    if ((irxp->client.state != OTS_DISCONNECT) && 
        (irxp->client.state != OTS_SERVICE_DISCONNECT)) {
        /* The client is busy performing an IAS query, discovery, etc. */
        return OB_STATUS_BUSY;
    }

    if (!DevAddr->iasQuery || DevAddr->iasQueryLen == 0) {
        /* We must have an IAS Query to execute */
        return OB_STATUS_FAILED;
    }

    /* Setup the IAS Query, just in case we need to 
     * execute the Query outside this function
     */
    irxp->IasQuery.queryBuf = DevAddr->iasQuery;
    irxp->IasQuery.queryLen = DevAddr->iasQueryLen;

    /* IrLAP is already up, perform an IAS query. */
    if (IR_IsIrLapConnected()) {
        if (DevAddr->addr && (DevAddr->addr != IRC(devAddr))) {
            /* The IrLAP connection is to the wrong device. */
            status = OB_STATUS_BUSY;
        } else {
            /* Send the IAS Query */
            status = (ObStatus)ObStartLsapQuery(irxp);
            if (status == OB_STATUS_PENDING)
                irxp->client.state = OTS_IAS_QUERY;
        
            ASSERT(status != IR_STATUS_NO_IRLAP);
        }
    } else {
        /* No IrLAP, do a discovery if no device was specified */
        if (DevAddr->addr == 0) {
                
            irxp->client.state = OTS_DISCOVERY;
            status = IR_DiscoverReq(&irxp->client.conn);
            
            ASSERT(status != IR_STATUS_FAILED);
            if (status == IR_STATUS_MEDIA_BUSY) {
                /* Start a timer to see if the 
                 * discovery can complete later. 
                 */
                irxp->timer.func = DiscoveryTimeout;
                EVM_StartTimer(&irxp->timer, OBEX_DISCOVERY_TIMEOUT);
            }
            status = OB_STATUS_PENDING;
            
        } else {
            status = (ObStatus)IR_ConnectIrLap(DevAddr->addr);
            if (status == OB_STATUS_PENDING)
                irxp->client.state = OTS_CONNECTING;
        }
    }

    return status;
}

/*---------------------------------------------------------------------------
 *            IRSTACK_ClientDisconnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect the client's Tiny TP connection if it is up.
 *
 * Return:    IR_STATUS_PENDING - operation is started successfully
 *            IR_STATUS_FAILED - operation failed because Server is has a
 *            connection (only in no LMP Disconnect build). 
 *            IR_STATUS_NO_CONNECT - operation failed because there is not
 *            a client connection.
 *
 */
ObStatus IRSTACK_ClientDisconnect(ObIrClientTransport *irxp, ObexTransportContext context, BOOL Force )
{
#if IR_LMP_DISCONNECT == XA_ENABLED
    ObexAppHandle *appHndl;
    ObexClientTransports *transport = ContainingRecord(irxp, ObexClientTransports, ObexClientIrTrans);
    appHndl = (ObexAppHandle *)ContainingRecord(transport, ObexClientApp, trans);

    if (irxp->client.state == OTS_CONNECTED) {
        IR_Disconnect(&irxp->client.conn, ObGetConnMgmtPacket(appHndl));
        
        irxp->client.state = OTS_DISCONNECTING;

        if (Force) {
            /* Application wants us to force the IrLAP connection down also */
            irxp->client.flags |= TP_FORCE_DISCONNECT;
        }
        
        return OB_STATUS_PENDING;
    }
    return OB_STATUS_NO_CONNECT;

#else /* IR_LMP_DISCONNECT == XA_ENABLED */
    if (IR_IsIrLapConnected()) {
        if (!IsLmConnected(&irxp->client.conn) || Force) {
            /* Since the client state tells the transport whether or not
             * to indicate a DISCONNECT to the application. It is important
             * that the client state not be DISCONNECT in order to insure that
             * the stack will generate a completion event for this PENDING call.
             */
            if ((irxp->client.state == OTS_DISCONNECT) ||
                (irxp->client.state == OTS_SERVICE_DISCONNECT))
                irxp->client.state = OTS_DISCONNECTING;

            IR_DisconnectIrLap();
            return OB_STATUS_PENDING;
        } else {
            /* Don't disconnect because the server is probably busy */
            return OB_STATUS_FAILED;
        }
    } 
    return OB_STATUS_NO_CONNECT;
#endif /* IR_LMP_DISCONNECT == XA_ENABLED */
}
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

/****************************************************************************
 *
 * OBEX Server "IrDA Stack specific" Application Programming Interface
 *
 ****************************************************************************/

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            IRSTACK_ServerGetIasLsap()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function provides the IAS LSAP value for the IrDA server
 *            transport.  The application is free to register as many
 *            services as necessary over this LSAP, however, only one 
 *            service can be connected to this LSAP at a time.
 *
 *            IRSTACK_ServerInit must be called prior to checking this value,
 *            otherwise the result value must be considered invalid.
 *
 * Return:    OB_STATUS_SUCCESS - LSAP successfully retrieved.
 *            OB_STATUS_FAILED - LSAP retrieval failed.  Server may not be
 *                initialized and bound to the IrDA stack yet.
 */
ObStatus IRSTACK_ServerGetIasLsap(ObIrServerTransport *irxp, U8 *lsap)
{
    if (irxp->server.state == OTS_UNINITIALIZED) {
        /* IR_Bind has not bee called yet */
        return OB_STATUS_FAILED;
    }

    *lsap = irxp->server.conn.lLsap;
    return OB_STATUS_SUCCESS;
}
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

/****************************************************************************
 *
 * OBEX Transport - Used by OBEX parser and command interpreter.
 *
 ****************************************************************************/

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            IRSTACK_ClientInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Client.
 *
 * Return:    TRUE - Initialized successfully
 *            FALSE - Unable to bind to the stack.
 *
 */
ObStatus IRSTACK_ClientInit(ObIrClientTransport *irxp)
{
    ObexClientTransports *transport = ContainingRecord(irxp, ObexClientTransports, ObexClientIrTrans);
    ObexAppHandle *appHndl = (ObexAppHandle*)ContainingRecord(transport, ObexClientApp, trans);

    /* Initialize the IrDA client transport */
    InitIrClientTransport(irxp);

    ASSERT(irxp->client.state == OTS_UNINITIALIZED);
        
    /* Bind the client to the protocol stack */
    if (IR_Bind(&irxp->client.conn, ObClientCallBack) != IR_STATUS_SUCCESS) {
        DebugPrint(("Fatal Error: Unable to bind the client to the protocol stack\n"));
        return OB_STATUS_FAILED;
    }
        
    /* Set the type of the connections to TinyTP */
    IR_SetConTypeTTP(&irxp->client.conn);
    
    /* Init persistent IAS stuff */
    irxp->IasQuery.result = irxp->IasResponse;
    irxp->IasQuery.resultBufSize = IAS_RESP_BUFF_SIZE;
    irxp->client.state = OTS_DISCONNECT;
    irxp->client.transport.funcTab = &IrStkFuncTable;
    irxp->client.transport.context = (ObexTransportContext)&irxp->client.conn;
    irxp->client.transport.connected = &irxp->client.state;
    irxp->client.transport.tpType = OBEX_TP_IRDA;

    irxp->client.app = appHndl;

    return OB_STATUS_SUCCESS;

}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            IRSTACK_ClientDeinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  DeInitialize the OBEX Client.
 *
 * Return:    TRUE -  Client stack deinitialized.
 *            FALSE - Client cannot deinitialize.
 *
 */
BOOL IRSTACK_ClientDeinit(ObIrClientTransport *irxp)
{
    if (irxp->client.state == OTS_UNINITIALIZED) {
        return TRUE;
    }
    
    if ((irxp->client.state != OTS_DISCONNECT) &&
        (irxp->client.state != OTS_SERVICE_DISCONNECT)) {
        return FALSE;
    }
    
    /* Unbind connection oriented and Ultra Lsaps */
    IR_Unbind(&irxp->client.conn);

    irxp->client.state = OTS_UNINITIALIZED;
    irxp->client.app = 0;

    return TRUE;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            IRSTACK_ServerInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Server.
 *
 * Return:    TRUE - Initialized successfully
 *            FALSE - Unable to bind to the stack.
 *
 */
ObStatus IRSTACK_ServerInit(ObIrServerTransport *irxp)
{
    ObexServerTransports *transport = ContainingRecord(irxp, ObexServerTransports, ObexServerIrTrans);
    ObexAppHandle *appHndl = (ObexAppHandle*)ContainingRecord(transport, ObexServerApp, trans);

    /* Initialize the IrDA server transport */
    InitIrServerTransport(irxp);

    ASSERT (irxp->server.state == OTS_UNINITIALIZED);
        
    /* Bind the server to the protocol stack */
    if (IR_Bind(&irxp->server.conn, ObServerCallBack) != IR_STATUS_SUCCESS) {
        DebugPrint(("Fatal Error: Unable to bind the server to the protocol stack\n"));
        return OB_STATUS_FAILED;
    }
    IR_SetConTypeTTP(&irxp->server.conn);    

    irxp->server.state = OTS_DISCONNECT;
    irxp->server.transport.funcTab = &IrStkFuncTable;
    irxp->server.transport.context = (ObexTransportContext)&irxp->server.conn;
    irxp->server.transport.connected = &irxp->server.state;
    irxp->server.transport.tpType = OBEX_TP_IRDA;

    irxp->server.app = appHndl;
    
    return OB_STATUS_SUCCESS;
}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            IRSTACK_ServerDeinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  DeInitialize the OBEX Stack server.
 *
 * Return:    TRUE -  Server stack deinitialized.
 *            FALSE - Server cannot deinitialize.
 *
 */
BOOL IRSTACK_ServerDeinit(ObIrServerTransport *irxp)
{
    if (irxp->server.state == OTS_UNINITIALIZED) {
        return TRUE;
    }
    
    if ((irxp->server.state != OTS_DISCONNECT) &&
        (irxp->server.state != OTS_SERVICE_DISCONNECT)) {
        return FALSE;
    }
    
    /* Unbind connection oriented Lsaps */
    IR_Unbind(&irxp->server.conn);
    irxp->server.state = OTS_UNINITIALIZED;
    irxp->server.app = 0;

    return TRUE;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */   
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

/*---------------------------------------------------------------------------
 *            IRSTACK_GetTpConnInfo()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Retrieves OBEX transport layer connection information. This
 *            function can be called when a transport connection is active
 *            to retrieve connection specific information. It should be used
 *            in conjunction with the receive flow control API to retrieve 
 *            the minimum amount of application storage space (low water mark)
 *            used when deciding to pause and resume data flow.
 *
 * Return:    TRUE  - The structure was successfully completed.
 *            FALSE - The transport is not connected.
 */
static BOOL IRSTACK_GetTpConnInfo(ObexTransportContext  context,
                                  ObexTpConnInfo   *tpConnInfo)
{
    ObIrCommonTransport *irxp;

    tpConnInfo->tpType = OBEX_TP_IRDA;
    tpConnInfo->minRxBuffs = OB_INITIAL_CREDITS;
    tpConnInfo->maxPduSize = IR_MaxRxSize((IrConnect *)context);

    if (tpConnInfo->maxPduSize == 0) {
        /* Estimate the max PDU size when the call fails. */
        tpConnInfo->maxPduSize = 2045;      /* IrLAP 2048 frame - 3 for LMP+TTP */
    }
    
    /* Get the local device address for the transport being used */
    irxp = ContainingRecord((IrConnect*)context, ObIrCommonTransport, conn);
    /* We do not have a transport connection */
    if (irxp->state != OTS_CONNECTED) return FALSE;
    /* Get the local server address and store it Big Endian */
    StoreBE32(irxp->devAddr, IRC(devAddr));
    tpConnInfo->devAddr = irxp->devAddr;

    return TRUE;
}

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
/*---------------------------------------------------------------------------
 *            IRSTACK_IsRxFlowAvailable()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Allows the application to determine if a credit based 
 *            receive flow control is available.
 *
 * Return:    TRUE -  Receive flow control is available.
 *            FALSE - Receive flow control is not available.
 */
static BOOL IRSTACK_IsRxFlowAvailable(ObexTransportContext context)
{
    return TRUE;
}

/*---------------------------------------------------------------------------
 *            IRSTACK_PauseRxFlow()
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
 *
 */
static void IRSTACK_PauseRxFlow(ObexTransportContext context)
{
    ObIrCommonTransport *irxp;

    irxp = ContainingRecord((IrConnect*)context, ObIrCommonTransport, conn);
    ASSERT(irxp->state == OTS_CONNECTED);
    irxp->flags |= TPP_FLOW;
}

/*---------------------------------------------------------------------------
 *            IRSTACK_ResumeRxFlow()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Opens the servers receive window after flow control was exerted.
 *
 * Return:    void
 *
 */
static void IRSTACK_ResumeRxFlow(ObexTransportContext context)
{
    ObIrCommonTransport *irxp;

    irxp = ContainingRecord((IrConnect*)context, ObIrCommonTransport, conn);
    ASSERT(irxp->state == OTS_CONNECTED);

    irxp->flags &= ~TPP_FLOW;
    IR_AdvanceCredit(&irxp->conn, irxp->credits);
    irxp->credits = 0;
}
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            IRSTACK_SetForceFlag()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Force the disconnection of the underlying LAP connection.
 *
 * Return:    void
 */
void IRSTACK_SetForceFlag(ObexTransportContext context)
{
#if IR_LMP_DISCONNECT == XA_ENABLED
    ObIrCommonTransport *irxp;

    /* Get the correct the IrDA transport structure for this callback */
    irxp = ContainingRecord((IrConnect *)context, ObIrCommonTransport, conn);
    /* Application wants us to force the IrLAP connection down also */
    irxp->flags |= TP_FORCE_DISCONNECT;
#endif /* IR_LMP_DISCONNECT == XA_ENABLED */
    /* Do nothing */
    return;
}
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED) */

/****************************************************************************
 *
 * OBEX IrDA Transport - Internal functions
 *
 ****************************************************************************/

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            ObClientCallBack()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  The OBEX Client's stack callback function. The stack calls
 *            this function with stack events.
 *
 * Return:    void
 *
 */
static void ObClientCallBack(IrConnect* con, IrCallBackParms* parms)
{
    IrStatus            status;
    TpState             saveState;
    IrDeviceAddr        device;
    ObexDiscReason      reason;
    ObIrClientTransport *irxp;

    OS_LockObex();

    /* Get the correct the IrDA transport structure for this callback */
    irxp = (ObIrClientTransport *)ContainingRecord(con, ObIrCommonTransport, conn);

    /* DebugPrint(("ObClientCallback: event %d, status %d, state %d\n", parms->event, parms->status, irxp->client.state)); */

    switch (parms->event) {
        case LEVENT_STATUS_IND:
            if ((irxp->client.state == OTS_DISCOVERY) && 
                (parms->status == IR_STATUS_MEDIA_NOT_BUSY)) {
                /* Retry the discovery request now */
                status = IR_DiscoverReq(&irxp->client.conn);
                ASSERT(status != IR_STATUS_FAILED);
                if (status == IR_STATUS_PENDING) {
                    /* Cancel the discovery timer, since our
                     * request succeeded.
                     */
                    EVM_CancelTimer(&irxp->timer);
                    /* Timer is unused, clear the function pointer */
                    irxp->timer.func = 0;
                }
            }
            break;

        case LEVENT_DISCOVERY_CNF:
            /* Determine if an OBEX device or telecom device exists */
            ASSERT (irxp->client.state == OTS_DISCOVERY);
            /* This discovery is part of an OBEX_ClientIrConnect() call
             * establish IrLAP connection to suitable device.
             */
            if ((device = FindObexDevice(parms->deviceList))) {
                /* A suitable device was found */
                if (IR_ConnectIrLap(device) == IR_STATUS_PENDING) {
                    goto done;
                }
            }
            /* No devices are in range */
            irxp->client.state = OTS_DISCONNECT;
            OBPROT_Disconnected(irxp->client.app, ODR_DISCOVERY_FAILED);
            break;

        case LEVENT_LAP_CON_IND:
            /* Remote device initiated an IrLAP connection.  If we have been 
             * waiting to issue a discovery, skip it and proceed to the
             * IAS query, using this connection.
             */
            if (irxp->client.state != OTS_DISCOVERY) {
                goto done;
            }

            DebugPrint(("OBEX: Remote IrLAP connection is up. Looking for OBEX service.\n"));

            /* Cancel the discovery timer. */
            EVM_CancelTimer(&irxp->timer);
            /* Timer is unused, clear the function pointer */
            irxp->timer.func = 0;

            irxp->client.state = OTS_IAS_QUERY;
            
            /* Use a custom IAS Query, if possible */
            ASSERT((irxp->IasQuery.queryBuf) && (irxp->IasQuery.queryLen != 0));
            status = (ObStatus)ObStartLsapQuery(irxp);
            if (status != IR_STATUS_PENDING) {
                ASSERT(status != IR_STATUS_NO_IRLAP);
                irxp->client.state = OTS_SERVICE_DISCONNECT;
                OBPROT_Disconnected(irxp->client.app, ODR_NO_SERVICE_FOUND);
            }
            break;

        case LEVENT_LAP_CON_CNF:
            /* Don't try to connect if we didn't initiate the LAP connection. */
            if ((irxp->client.state != OTS_DISCOVERY) &&
                (irxp->client.state != OTS_CONNECTING)) {
                goto done;
            }

            /* The IrLAP connection is up. Perform an IAS query */
            DebugPrint(("OBEX: IrLAP connection is up. Looking for OBEX service.\n"));

            /* Use a custom IAS Query, if possible */
            ASSERT((irxp->IasQuery.queryBuf) && (irxp->IasQuery.queryLen != 0));
            status = (ObStatus)ObStartLsapQuery(irxp);
            if (status == IR_STATUS_PENDING) {
                irxp->client.state = OTS_IAS_QUERY;
            }
            else {
                ASSERT(status != IR_STATUS_NO_IRLAP);
                irxp->client.state = OTS_SERVICE_DISCONNECT;
                OBPROT_Disconnected(irxp->client.app, ODR_NO_SERVICE_FOUND);
            }
             break;

        case LEVENT_LAP_DISCON_IND:
            saveState = irxp->client.state;
            irxp->client.state = OTS_DISCONNECT;

            switch (saveState) {
            case OTS_DISCONNECT:
                /* Not connected, do nothing */
                goto done;

            case OTS_IAS_QUERY:
                /* We were trying to Query IAS. */
                reason = ODR_NO_SERVICE_FOUND;
                break;

            case OTS_DISCOVERY:
                if (irxp->timer.func != 0) {
                    /* Don't bother indicating a disconnect since we're still
                     * trying to connect to the remote device.  Keep the
                     * discovery timer running.  It will either succeed or we'll
                     * report a discovery failure when the timer expires.
                     */
                    irxp->client.state = OTS_DISCOVERY;
                    goto done;
                } else {
                    /* An internal error occurred while attempting the discovery
                     * process.  Indicate the failure.
                     */
                    reason = ODR_DISCOVERY_FAILED;
                    break;
                }

            /* We were connected, or connecting to an OBEX Server. */
            case OTS_DISCONNECTING:
                reason = ODR_USER_REQUEST;
                break;

            default:
                reason = ODR_UNKNOWN;
                break;
            }
            OBPROT_Disconnected(irxp->client.app, reason);
            break;

        case LEVENT_LM_CON_CNF:
            /* The IrLMP connection is now up. Call the application and
             * indicate that data can be written.
             */
            irxp->client.state = OTS_CONNECTED;
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
            irxp->client.credits = 0;
            irxp->client.flags &= ~TPP_FLOW;
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
            OBPROT_Connected(irxp->client.app);
            break;

        case LEVENT_LM_DISCON_IND:
            saveState = irxp->client.state;

            if ((saveState == OTS_DISCONNECT) || !(IRSTACK_IsIrLapConnected())) {
                /* Either this is not the active client, or IrLAP just went
                 * down and we are getting the IrLMP notification first, so do
                 * nothing.  If the latter situation occurred, ignore this event 
                 * and wait for the upcoming LAP disconnect indication.
                 */
                goto done;
            }

#if IR_LMP_DISCONNECT == XA_ENABLED
            if (irxp->client.flags & TP_FORCE_DISCONNECT) {
                /* Application wants to force down the IrLAP connection */
                irxp->client.flags &= ~TP_FORCE_DISCONNECT;
                IR_DisconnectIrLap();
                break;
            }
#else /* IR_LMP_DISCONNECT == XA_ENABLED */
            if ((saveState == OTS_CONNECTING) && !IsLmConnected(con)) {
                /* We initiated an IrLMP connection and failed. If no 
                 * other IrLMP connections exist, we will disconnect 
                 * IrLAP. That will generate the proper client event 
                 * when completed.
                 */
                irxp->client.state = OTS_LAP_CONNECT;
                IR_DisconnectIrLap();
                break;
            }
            ASSERT(saveState == OTS_CONNECTED);
#endif /* IR_LMP_DISCONNECT == XA_ENABLED */
            irxp->client.state = OTS_SERVICE_DISCONNECT;

            if (saveState == OTS_DISCONNECTING)
                reason = ODR_USER_REQUEST;
            else reason = ODR_UNKNOWN;

            OBPROT_Disconnected(irxp->client.app, reason);
            break;

        case LEVENT_DATA_IND:
            /* Process the received data */
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
            irxp->client.credits++;
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
            OBPROT_ParseRx(irxp->client.app, parms->rxBuff, parms->rxLen);

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
            /* The packet has been consumed so advance credit */
            if ((irxp->client.flags & TPP_FLOW) == 0) {
                IR_AdvanceCredit(con, irxp->client.credits);
                irxp->client.credits = 0;
            }
#else /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
            IR_AdvanceCredit(con, 1);
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
            break;

        case LEVENT_PACKET_HANDLED:
            ObTxDone(irxp->client.app, parms->packet);
            break;
    }

done:
    OS_UnlockObex();

}
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            ObServerCallBack()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  The OBEX Server's stack callback function. The stack calls
 *            this function with stack events.
 *
 * Return:    void
 *
 */
static void ObServerCallBack(IrConnect* con, IrCallBackParms* parms)
{
    IrStatus  status;
    ObIrServerTransport *irxp;

    OS_LockObex();

    /* Get the correct the IrDA transport structure for this callback */
    irxp = (ObIrServerTransport *)ContainingRecord(con, ObIrCommonTransport, conn);

/*    DebugPrint(("ObServerCallBack: event %d, status %d, state %d\n", parms->event, parms->status, irxp->server.state)); */

    switch (parms->event) {
        case LEVENT_LM_CON_IND:
            if (OBPROT_LinkTransport(irxp->server.app, &irxp->server.transport)) {

                /* Accept the incoming connection for the server */
                status = IR_ConnectRsp(con, ObGetConnMgmtPacket(irxp->server.app), OB_INITIAL_CREDITS);
                ASSERT(status == IR_STATUS_PENDING);
            
                irxp->server.state = OTS_CONNECTED;
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
                irxp->server.credits = 0;
                irxp->server.flags &= ~TPP_FLOW;
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
                
                /* Tell the application that the server has accepted a connection */
                OBPROT_Connected(irxp->server.app);
            } else {
                /* The server parser is already connected over another transport.
                 * We cannot accept this connection.
                 */
                IRSTACK_DisconnectReq(con);
            }
            break;

        case LEVENT_DATA_IND:
            /* Process the received data */
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
            irxp->server.credits++;
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
            OBPROT_ParseRx(irxp->server.app, parms->rxBuff, parms->rxLen);


#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
            /* The packet has been consumed so advance credit */
            if ((irxp->server.flags & TPP_FLOW) == 0) {
                IR_AdvanceCredit(con, irxp->server.credits);
                irxp->server.credits = 0;
            }
#else /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
            IR_AdvanceCredit(con,1);
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
            break;

        case LEVENT_LM_DISCON_IND:
            if ((irxp->server.state == OTS_DISCONNECT) || !(IRSTACK_IsIrLapConnected())) {
                /* Either this is not the active server, or IrLAP just went
                 * down and we are getting the IrLMP notification first, so do
                 * nothing.  If the latter situation occurred, ignore this event 
                 * and wait for the upcoming LAP disconnect indication.
                 */
                goto done;
            }
#if IR_LMP_DISCONNECT == XA_ENABLED
            if (irxp->server.flags & TP_FORCE_DISCONNECT) {
                /* Application wants to force down the IrLAP connection */
                irxp->server.flags &= ~TP_FORCE_DISCONNECT;
                IR_DisconnectIrLap();
                break;
            }
#endif /* IR_LMP_DISCONNECT == XA_ENABLED */

        case LEVENT_LAP_DISCON_IND:
            if (irxp->server.state == OTS_DISCONNECT) {
                /* This is not the active server, so do nothing */
                goto done;
            }
             
            if (parms->event == LEVENT_LAP_DISCON_IND)
                irxp->server.state = OTS_DISCONNECT;
            else irxp->server.state = OTS_SERVICE_DISCONNECT;
            /* This call aborts the current operation if there
             * is one and reports the link status to the server.
             */
            OBPROT_Disconnected(irxp->server.app, ODR_UNKNOWN);
            break;
    
        case LEVENT_PACKET_HANDLED:
            ObTxDone(irxp->server.app, parms->packet);
            break;

    }

done:
    OS_UnlockObex();

}
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

/*---------------------------------------------------------------------------
 *            ObTxDone()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Processes completion of client transmit.
 *
 * Return:    void
 *
 */
static void ObTxDone(ObexAppHandle *AppHndl, IrPacket *packet)
{
    ObexPacket  *obpacket;
    void        *ObexApp = AppHndl;

    ASSERT(AppHndl && &AppHndl->parser);

    if (IsServerParser(&AppHndl->parser)) {
#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
        if (packet == (IrPacket *)&IRSXP(server.mgmtPkts[0])) {
            ASSERT(IRSXP(server.flags) & TPP_M0_PKT_BUSY);
            IRSXP(server.flags) &= ~TPP_M0_PKT_BUSY;
            return;
        } 
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */
    }
    else {
#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
        if (packet == (IrPacket *)&IRCXP(client.mgmtPkts[0])) {
            ASSERT(IRCXP(client.flags) & TPP_M0_PKT_BUSY);
            IRCXP(client.flags) &= ~TPP_M0_PKT_BUSY;
            return;
        } 
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */
    }

    /* Return data packet to pool */
    obpacket = ContainingRecord(packet, ObexPacket, irpkt);

    OBPROT_TxDone(AppHndl, obpacket);
}

/****************************************************************************
 *
 * OBEX Stack functions required by parser and command interpreter.
 * These functions are accessed via the ObTransFuncTable.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            IRSTACK_SendTxBuffer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send a buffer over the Ir Connection.
 *
 * Return:    Status of the IR_DataReq() operation.
 *
 */
static ObStatus IRSTACK_SendTxBuffer(ObexTransportContext context, U8* buff, U16 len)
{
    IrStatus    status;
    ObexPacket *txPacket = ContainingRecord(buff, ObexPacket, buffer);

    ASSERT(len <= IRSTACK_MaxTxSize(context));

    txPacket->irpkt.buff = buff;
    txPacket->irpkt.len = len;
    
    status = IR_DataReq(context, &txPacket->irpkt);
    if (status != OB_STATUS_PENDING) {
        ASSERT(status != OB_STATUS_SUCCESS);
        /* Tx Failed! Put the packet back into the queue. */
        OBPROT_ReturnTxBuffer(buff);
    }
    return status;
}

/*---------------------------------------------------------------------------
 *            IRSTACK_DisconnectReq()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect the underlying LMP connection.
 *
 * Return:    OB_STATUS_PENDING
 */
static ObStatus IRSTACK_DisconnectReq(ObexTransportContext context)
{
#if IR_LMP_DISCONNECT == XA_ENABLED
    ObIrCommonTransport *irxp;
    ObStatus             status;
    IrPacket            *packet;

    /* Get the correct the IrDA transport structure for this callback */
    irxp = ContainingRecord((IrConnect *)context, ObIrCommonTransport, conn);
    packet = ObGetConnMgmtPacket(irxp->app);

    status = IR_Disconnect((IrConnect *)context, packet);
    if (status != OB_STATUS_PENDING) {
        ASSERT(status != OB_STATUS_SUCCESS);
        ObTxDone(irxp->app, packet);
    }

#else /* IR_LMP_DISCONNECT == XA_ENABLED */
    IR_DisconnectIrLap();
#endif /* IR_LMP_DISCONNECT == XA_ENABLED */
    return OB_STATUS_PENDING;
}

/*---------------------------------------------------------------------------
 *            IRSTACK_MaxTxSize()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the maximum transport protocol data unit that can be
 *            sent over the referenced transport connection.
 *
 * Return:    U16
 *            
 */
U16 IRSTACK_MaxTxSize(ObexTransportContext context)
{
    return min(OBEX_TX_BUFF_SIZE, IR_MaxTxSize((IrConnect *)context));
}

/****************************************************************************
 *
 * OBEX IrDA Transport - More Internal functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            ObGetConnMgmtPacket()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Return a pointer to a packet that can be used for sending
 *            connect/disconnect indications and acknowledgements.
 *
 * Return:    Pointer to transmit packet.
 *
 */
static IrPacket *ObGetConnMgmtPacket(ObexAppHandle *AppHndl)
{
    IrPacket *packet = 0;
    void     *ObexApp = AppHndl;

    ASSERT(AppHndl && &AppHndl->parser);

    if (IsServerParser(&AppHndl->parser)) {
#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
        ASSERT(!(IRSXP(server.flags) & TPP_M0_PKT_BUSY));
        packet = (IrPacket *)&IRSXP(server.mgmtPkts[0]);
        IRSXP(server.flags) |= TPP_M0_PKT_BUSY;
        packet->len = 0;
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */
    }
    else {
#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
        ASSERT(!(IRCXP(client.flags) & TPP_M0_PKT_BUSY));
        packet = (IrPacket *)&IRCXP(client.mgmtPkts[0]);
        IRCXP(client.flags) |= TPP_M0_PKT_BUSY;
        packet->len = 0;
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */
    }
   
    return packet;
}

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            FindObexDevice()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Find the first device in the deviceList that has the OBEX
 *            hint bit set.
 *
 * Return:    Pointer to OBEX device or 0 if no device is found.
 *
 */
static IrDeviceAddr FindObexDevice(IrDeviceList* list)
{
    IrDeviceInfo *info;
    U8 i;

    for (i = 0; i < list->nItems; i++) {
        info = &(list->dev[i]);
        if ((info->len > 1) && (info->xid[0] & IR_HINT_EXT) &&
            (info->xid[1] & IR_HINT_OBEX)) {
            return (info->hDevice);
        }
    }
    
    /* No devices are presenting the OBEX Hint bit, so just choose the 
     * first device. If this device does not support OBEX, nothing lost,
     * we were going to fail anyway.
     */
    if (list->nItems)
        return (list->dev[0].hDevice);
    
    return (0);

}

/*---------------------------------------------------------------------------
 *            ObStartLsapQuery()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Query the remote devices IAS database.
 *
 * Return:    Status of IRIAS_Query() call
 *
 */
static IrStatus ObStartLsapQuery(ObIrClientTransport *irxp)
{
    irxp->IasQuery.callBack = LsapQueryCallBack;
        
    /* Save off the current client doing the IAS Query. */
    OBD(iasQuery) = &irxp->IasQuery;
    return IRIAS_Query(&irxp->IasQuery);
}

/*---------------------------------------------------------------------------
 *            LsapQueryCallBack()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  The application's IAS query callback function. The stack
 *            this function when an IAS query started by the application is
 *            complete.
 *
 * Return:    void
 *
 */
static void LsapQueryCallBack(IrStatus status)
{
    IrIasQuery          *iasQuery;
    ObIrClientTransport *irxp;
    ObexAppHandle       *appHndl;

    OS_LockObex();

    /* Get the current IAS Query */
    iasQuery = OBD(iasQuery);
    irxp = ContainingRecord(iasQuery, ObIrClientTransport, IasQuery);
    appHndl = (ObexAppHandle*)ContainingRecord(irxp, ObexClientApp, trans);

    if ((status == IR_STATUS_SUCCESS) && 
        (irxp->IasQuery.retCode == IAS_RET_SUCCESS) &&
        (IRIAS_GetType(&irxp->IasQuery) == IAS_ATTRIB_INTEGER)) {
        /* The IAS query is successful. Attempt the IrLMP connection */
        irxp->client.conn.rLsap = IRIAS_GetIntLsap(&irxp->IasQuery);
        irxp->client.state = OTS_CONNECTING;

        DebugPrint(("OBEX: Found an OBEX device starting TinyTP connection to Lsap %d.\n", irxp->client.conn.rLsap));

        status = IR_ConnectReq(&irxp->client.conn, ObGetConnMgmtPacket(appHndl),
                               OB_INITIAL_CREDITS);
        
        ASSERT(status == IR_STATUS_PENDING);
    
    } else {
#if IR_LMP_DISCONNECT == XA_DISABLED
        /* Our IAS query did not find a device with an OBEX service 
         * so report the failure.
         *
         * If our server is not connected then disconnect IrLAP too.
         */
        if (IsLmConnected(&irxp->client.conn)) {
            irxp->client.state = OTS_SERVICE_DISCONNECT;
            OBPROT_Disconnected(irxp->client.app, ODR_NO_SERVICE_FOUND);
        } else {
            /* Disconnect IrLAP */
            irxp->client.state = OTS_IAS_QUERY;
            IR_DisconnectIrLap();
        }
#else /* IR_LMP_DISCONNECT == XA_DISABLED */
        if (status != IR_STATUS_DISCONNECT) {
            /* Indicate the IAS did not find a matching service */
            irxp->client.state = OTS_SERVICE_DISCONNECT;
            OBPROT_Disconnected(irxp->client.app, ODR_NO_SERVICE_FOUND);
        } else {
            /* Don't do anything let the IrLAP disconnect event handle it */
        }
#endif /* IR_LMP_DISCONNECT == XA_DISABLED */
    }
    OS_UnlockObex();

}

/*---------------------------------------------------------------------------
 *            DiscoveryTimeout()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Function that is called upon expiration of the discovery timer.
 *            The discovery timer dictates how long a discovery process will
 *            take if the media is busy for each call to IR_DiscoverReq.
 *
 * Return:    void
 *
 */
static void DiscoveryTimeout(EvmTimer *Timer)
{
    ObIrClientTransport *irxp;
    
    OS_LockObex();
bt_prompt_trace(MOD_BT, "[BT] DiscoveryTimeout");
    irxp = ContainingRecord(Timer, ObIrClientTransport, timer);

    /* Only indicate a discovery failure, if we haven't already */
    if (irxp->client.state == OTS_DISCONNECT) goto done;

    ASSERT(irxp->client.state == OTS_DISCOVERY);

    /* The Discovery process has timed out, reset the client state */
    irxp->client.state = OTS_DISCONNECT;
    /* Discovery timeout occurred, return an event to the application */
    OBPROT_Disconnected(irxp->client.app, ODR_DISCOVERY_FAILED);

done:
    OS_UnlockObex();
}
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            IsLmConnected()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Determines if any bound LSAP values are connected through the
 *            IrDA stack.  This could include LSAP values used over protocols
 *            other than OBEX.
 *
 * Return:    TRUE or FALSE
 *
 */
static BOOL IsLmConnected(IrConnect *con) 
{
    U8 i;
    BOOL isConnected = FALSE;

    OS_LockObex();

    for (i = 0; i < IR_CONS_SIZE; i++) {
        if ((IRC(lmCons)[i]) && (IRC(lmCons)[i]->lLsap != 0) && 
            (IRC(lmCons)[i]->rLsap != 0)) {
            /* Check this bound IrConnect structure for a LM connection. 
             * We don't consider an IAS Server or IAS Client connection
             */
            if ((IRC(lmCons)[i]->flags & LCON_FLAGS_BUSY) && 
                (con->lLsap != IRC(lmCons)[i]->lLsap)) {
                /* We found a LM connection to an LSAP other than the one we
                 * wish to disconnect from.
                 */
                isConnected = TRUE;
                goto Done;
            }
        }
    }

Done:
    OS_UnlockObex();
    return isConnected;
}
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED) */

#endif /* defined(IRDA_STACK) && IRDA_STACK == XA_ENABLED */
