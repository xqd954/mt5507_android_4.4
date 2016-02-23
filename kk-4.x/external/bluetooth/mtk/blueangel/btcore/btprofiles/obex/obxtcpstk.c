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
 * File:          obxtcpstk.c
 *
 * Description:   This file contains the code for OBEX TCP protocol stack
 *                interface module.
 * 
 * Created:       February 20, 2000
 *
 * Version:       MTObex 3.4
 *
 * Copyright 2000-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
#include <obex.h>
#include <sys/obxtcpstk.h>

#if defined(TCP_STACK) && TCP_STACK == XA_ENABLED
#include <winsock.h>

/****************************************************************************
 *
 * Prototypes for Internal Functions
 *
 ***************************************************************************/
static HWND InitAsyncCallback(void);
static LRESULT CALLBACK AsyncEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/* Prototypes for functions provided in the ObTransFuncTable */
static ObStatus TCPSTACK_DisconnectReq(ObexTransportContext con);
static U16      TCPSTACK_MaxTxSize(ObexTransportContext con);
static ObStatus TCPSTACK_SendTxBuffer(ObexTransportContext con, U8 *buff, U16 len);
static BOOL TCPSTACK_GetTpConnInfo(ObexTransportContext  con,
                                   ObexTpConnInfo   *tpConnInfo);

static void     TcpCallback(ObTcpCommonTransport *txp, U8 EventType, void *Parm1, U16 Parm2);
static ObTcpCommonTransport *GetTrans(SOCKET skt);

/****************************************************************************
 *
 * RAM data
 *
 ****************************************************************************/
static ListEntry clientList;       /* List of clients */
static ListEntry serverList;       /* List of servers */
static U8 flags = 0;              /* Client/Server list initialization flag */
static HWND     asyncEventHandle = 0;
/****************************************************************************
 *
 * ROMable data
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * TCP Transport function call table. These are the functions exported to
 * OBEX Parser by this transport.
 */
static const ObTransFuncTable TcpStkFuncTable = {
    TCPSTACK_DisconnectReq,
    TCPSTACK_MaxTxSize,
    TCPSTACK_SendTxBuffer,
    TCPSTACK_GetTpConnInfo,
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
     0, 0, 0
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
};

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            TCPSTACK_ClientConnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate a transport connection to the specified an OBEX device.
 *            This function will usually take the address of the target device
 *            as a parameter. Where necessary, this function will usually
 *            initiate service discovery and lower layer connection procedures.
 *
 * Return:    OB_STATUS_PENDING - operation is started successfully
 *            OB_STATUS_SUCCESS - operation is successful
 *            OB_STATUS_BUSY - operation failed because the client is busy.
 */
ObStatus TCPSTACK_ClientConnect( ObexTcpTarget *Target, ObTcpClientTransport *txp,
                                 ObexTransport **Trans)
{
    SOCKADDR_IN to;
    I32         rcode;

    /* Only allow legal TCP server port addresses */
    ASSERT(Target->customPort > 0 && Target->customPort <= 65535);

#if XA_ERROR_CHECK == XA_ENABLED
    if (txp->client.state != OCS_IDLE) {
        return OB_STATUS_BUSY;
    }

    if ((Target->addr == INADDR_ANY) || (Target->addr == INADDR_NONE) || 
        (Target->customPort <= 0) || (Target->customPort > 65535)) {
        return OB_STATUS_FAILED;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT((Target->addr != INADDR_ANY) && (Target->addr != INADDR_NONE));
    ASSERT(txp->client.state == OCS_IDLE);

    /* Build the TCP address and open the client socket */
    to.sin_addr.S_un.S_addr = Target->addr;
    to.sin_family = AF_INET;
    to.sin_port = htons(Target->customPort);

    txp->client.conn = socket(AF_INET, SOCK_STREAM, 0);
    if (txp->client.conn == INVALID_SOCKET) {
        return OB_STATUS_FAILED;
    }

    /* We use the asynchronous mode Connect */
    rcode = WSAAsyncSelect(txp->client.conn, asyncEventHandle, WM_SELECT, FD_CONNECT|FD_CLOSE);
    ASSERT(rcode != SOCKET_ERROR);

    rcode = connect(txp->client.conn, (struct sockaddr *)&to, sizeof(SOCKADDR_IN));
    if (rcode == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            closesocket(txp->client.conn);
            txp->client.conn = INVALID_SOCKET;
            return OB_STATUS_FAILED;
        }
        /* Connection is pending */
        txp->client.state = OCS_CONNECTING;
        *Trans = &txp->client.transport;
        return OB_STATUS_PENDING;
    }

    /* Connection was established */
    txp->client.state = OCS_CONNECTED;
    *Trans = &txp->client.transport;
    return OB_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            TCPSTACK_ClientDisconnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect the client's Transport connection if one exists.
 *
 * Return:    OB_STATUS_PENDING - disconnect was started successfully.
 *            OB_STATUS_FAILED - the connection could not be disconnected.
 *            OB_STATUS_NO_CONNECT - there is no connection to disconnect.
 *
 */
ObStatus TCPSTACK_ClientDisconnect(ObTcpClientTransport *txp)
{
    I32     rcode;

    if ((txp->client.state != OCS_CONNECTING) && 
        (txp->client.state != OCS_CONNECTED)) {

        return OB_STATUS_NO_CONNECT;
    }

    /* The transport connection exists. */
    rcode = closesocket(txp->client.conn);
    ASSERT(rcode != SOCKET_ERROR);
    
#if 0
    /* We want an asynchronous disconnect completion so post a
     * disconnect message to the event handler and return PENDING.
     */
    PostMessage(asyncEventHandle, WM_SELECT, txp->client.conn, FD_CLOSE);
    
    txp->client.state = OCS_DISCONNECTING;
    return OB_STATUS_PENDING;
#else
    /* Indicate immediate disconnect, return SUCCESS. */
    txp->client.state = OCS_IDLE;
    return OB_STATUS_SUCCESS;
#endif
}

/****************************************************************************
 *
 * Tcp Stack functions accessed via the obstack.c Init & Deinit functions.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            TCPSTACK_ClientInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Client Tcp transport. This function is
 *            called by OBSTACK_ClientInit() in obstack.c.
 *
 * Return:    OB_STATUS_SUCCESS - Initialized successfully
 *            OB_STATUS_FAILED - Unable to bind to the stack.
 *
 */
ObStatus TCPSTACK_ClientInit(ObTcpClientTransport *txp)
{
    ObexClientTransports *transport = ContainingRecord(txp, ObexClientTransports, ObexClientTcpTrans);
    ObexAppHandle *appHndl = (ObexAppHandle*)ContainingRecord(transport, ObexClientApp, trans);

    /* Initialize the TCP client transport */
    InitTcpClientTransport(txp);

    ASSERT(txp->client.state == OCS_UNINITIALIZED);
        
    /* Only initialize the socket list once */
    if (flags == 0) {
        InitializeListHead(&clientList);
        InitializeListHead(&serverList);
        flags = 1;
    }


    /* The same Async event handler is used for all clients. */
    if (asyncEventHandle == 0) {
        if ((asyncEventHandle = InitAsyncCallback()) == 0)
           return OB_STATUS_FAILED;
    }

    txp->client.conn = INVALID_SOCKET;
    txp->client.state = OCS_IDLE;
    txp->client.transport.connected = &txp->client.state;
    txp->client.transport.funcTab = &TcpStkFuncTable;
    txp->client.transport.context = (ObexTransportContext)&txp->client.conn;
    txp->client.transport.tpType = OBEX_TP_TCP;

    txp->client.app = appHndl;
    InsertTailList(&clientList, &txp->client.node);

    return OB_STATUS_SUCCESS;

}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            TCPSTACK_ClientDeinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Client transport. This function returns
 *            success if the client is not initialized.
 *
 * Return:    TRUE -  Client stack deinitialized.
 *            FALSE - Client cannot deinitialize.
 *
 */
BOOL TCPSTACK_ClientDeinit(ObTcpClientTransport *txp)
{
    ObexClientTransports *transport = ContainingRecord(txp, ObexClientTransports, ObexClientTcpTrans);
    ObexAppHandle *appHndl = (ObexAppHandle*)ContainingRecord(transport, ObexClientApp, trans);

    if (txp->client.state == OCS_UNINITIALIZED) {
        return TRUE;
    }

    ASSERT(txp->client.app == appHndl);
    
    if (txp->client.state != OCS_IDLE) {
        return FALSE;
    }

    txp->client.state = OCS_UNINITIALIZED;
    txp->client.app = 0;
    RemoveEntryList(&txp->client.node);

    return TRUE;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            TCPSTACK_ServerInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Server Tcp transport. This function will
 *            usually perform service registration, where necessary. This
 *            function is called by OBSTACK_ServerInit() in obstack.c.
 *
 * Return:    OB_STATUS_SUCCESS - Initialized successfully
 *            OB_STATUS_FAILED - Unable to bind to the stack.
 *            OB_STATUS_NO_RESOURCES - Out of resources
 *
 */
ObStatus TCPSTACK_ServerInit(ObTcpServerTransport *txp)
{
    I32         rcode;
    I32         myAddrLen;
    SOCKADDR_IN myAddr = {AF_INET, htons(650), INADDR_ANY, 0};
    ObexServerTransports *transport = ContainingRecord(txp, ObexServerTransports, ObexServerTcpTrans);
    ObexAppHandle *appHndl = (ObexAppHandle*)ContainingRecord(transport, ObexServerApp, trans);

    /* Initialize the TCP server transport */
    InitTcpServerTransport(txp);

    ASSERT (txp->server.state == OCS_UNINITIALIZED);

    /* Only initialize the socket list once */
    if (flags == 0) {
        InitializeListHead(&clientList);
        InitializeListHead(&serverList);
        flags = 1;
    }

    /* The same Async event handler is used for all servers. */
    if (asyncEventHandle == 0) {
        if ((asyncEventHandle = InitAsyncCallback()) == 0)
           return OB_STATUS_FAILED;
    }
   
    txp->server.conn = INVALID_SOCKET;
    txp->server.transport.connected = &txp->server.state;
    txp->server.transport.funcTab = &TcpStkFuncTable;
    txp->server.transport.context = (ObexTransportContext)&txp->server.conn;
    txp->server.transport.tpType = OBEX_TP_TCP;

    /* Register TCP Server by opening OBEX Server port */
    txp->serverReg = socket(AF_INET, SOCK_STREAM, 0);
    if (txp->serverReg == INVALID_SOCKET) {
        return OB_STATUS_NO_RESOURCES;
    }

    /* Try for the OBEX TCP Port first, if it's busy, any port will do. */
    rcode = bind(txp->serverReg, (struct sockaddr *)&myAddr, sizeof(SOCKADDR_IN));
    if (rcode == SOCKET_ERROR) {
        myAddr.sin_port = 0;
        rcode = bind(txp->serverReg, (struct sockaddr *)&myAddr, sizeof(SOCKADDR_IN));
    }

    /* Record the port the server is listening on */
    myAddrLen = sizeof(struct sockaddr);
    getsockname(txp->serverReg, (struct sockaddr *)&myAddr, &myAddrLen);
    txp->serverPort = ntohs(myAddr.sin_port);

    if (rcode != SOCKET_ERROR) {

        rcode = listen(txp->serverReg, 1);
        if (rcode != SOCKET_ERROR) {

            rcode = WSAAsyncSelect(txp->serverReg, asyncEventHandle, 
                                   WM_SELECT, FD_ACCEPT|FD_CLOSE);
            ASSERT(rcode != SOCKET_ERROR);
            
            txp->server.state = OCS_IDLE;
            txp->server.app = appHndl;
            InsertTailList(&serverList, &txp->server.node);
            return OB_STATUS_SUCCESS;
        }
    }
    kal_trace(BT_TRACE_G6_OBEX,TCPSTACK_SOCKET_FAILURExD ,WSAGetLastError());
    closesocket(txp->serverReg);
    txp->serverReg = INVALID_SOCKET;

    return OB_STATUS_FAILED;

}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            TCPSTACK_ServerDeinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Server transport. This function returns
 *            success if the server is not initialized.
 *
 * Return:    TRUE -  Server stack deinitialized.
 *            FALSE - Server cannot deinitialize.
 *
 */
BOOL TCPSTACK_ServerDeinit(ObTcpServerTransport *txp)
{
    ObexServerTransports *transport = ContainingRecord(txp, ObexServerTransports, ObexServerTcpTrans);
    ObexAppHandle *appHndl = (ObexAppHandle*)ContainingRecord(transport, ObexServerApp, trans);

    if (txp->server.state == OCS_UNINITIALIZED) {
        return TRUE;
    }
    
    ASSERT(txp->server.app == appHndl);

    if (txp->server.state != OCS_IDLE) {
        return FALSE;
    }

    /* Close registration socket */
    closesocket(txp->serverReg);
    txp->serverReg = INVALID_SOCKET;

    txp->server.state = OCS_UNINITIALIZED;
    txp->server.app = 0;
    RemoveEntryList(&txp->server.node);

    return TRUE;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */
   
/*---------------------------------------------------------------------------
 *            TCPSTACK_GetTpConnInfo()
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
static BOOL TCPSTACK_GetTpConnInfo(ObexTransportContext  con,
                                   ObexTpConnInfo   *tpConnInfo)
{
    SOCKADDR             name;
    U32                  len;
    ObTcpCommonTransport *txp;

    tpConnInfo->tpType = OBEX_TP_TCP;
    tpConnInfo->minRxBuffs = 1;
    tpConnInfo->maxPduSize = 1450;

    /* Get the local device address for the transport being used */
    txp = ContainingRecord((SOCKET *)con, ObTcpCommonTransport, conn);
    /* We do not have a transport connection */
    if (txp->state != OCS_CONNECTED) return FALSE;
    len = sizeof(SOCKADDR);
    if (getsockname(txp->conn, &name, &len) != 0) return FALSE;
    ASSERT(len == 16);
    /* Get the server IP address and store it Big Endian */
    OS_MemCopy(txp->devAddr, name.sa_data+2, 4);
    tpConnInfo->devAddr = txp->devAddr;
    return TRUE;
}

/****************************************************************************
 *
 * Internal TCP Stack functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            TcpCallback
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process events from the client and server transport.
 *
 */
static void TcpCallback(ObTcpCommonTransport *txp, U8 EventType, void *Parm1, U16 Parm2)
{
#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
    I32         rcode;
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

    OS_LockObex();
    switch (EventType) {

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
    case TCP_EVENT_CONNECT_IND: /* TCP Server Event */
        /* Accept the incoming connection to our server if it's idle. */
        if (OBPROT_LinkTransport(txp->app, &txp->transport)) {
            ASSERT(txp->state == OCS_IDLE);
            txp->state = OCS_CONNECTING;

            /* Save the connection handle */
            txp->conn = accept(((ObTcpServerTransport *)txp)->serverReg, 0, 0);
            ASSERT(txp->conn != INVALID_SOCKET);
            
            rcode = WSAAsyncSelect(txp->conn, asyncEventHandle, WM_SELECT, FD_WRITE|FD_CLOSE);
            ASSERT(rcode != SOCKET_ERROR);
        } else {
            /* Reject the connection, the parser is busy. */
            txp->conn = accept(((ObTcpServerTransport *)txp)->serverReg, 0, 0);
            ASSERT(txp->conn != INVALID_SOCKET);

            closesocket(txp->conn);
            txp->conn = INVALID_SOCKET;
        }            
        break;
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

    case TCP_EVENT_CONNECTED:
        /* Save the new connection handle (Client only) */
        txp->conn = Parm2;
        txp->state = OCS_CONNECTED;
        
        /* Tell the application that the server has accepted a connection */
        OBPROT_Connected(txp->app);
        break;

    case TCP_EVENT_DATA_IND:
        /* Pass data directly to the client parser */
        OBPROT_ParseRx(txp->app, Parm1, Parm2);
        break;

    case TCP_EVENT_DISCON:
        txp->conn = INVALID_SOCKET;

        /* This can happen if TCPSTACK_ClientDisconnect() is called while
         * a connect is in progress. When disconnect is handled synchronously.
         */
        if (txp->state == OCS_IDLE)
            break;

        txp->state = OCS_IDLE;
        txp->conn = INVALID_SOCKET;

        /* This call will deliver a disconnect indication to the app. */
        OBPROT_Disconnected(txp->app, ODR_UNKNOWN);
        break;
    }
    OS_UnlockObex();
}

/****************************************************************************
 *
 * TCP Stack functions accessed via the ObTransFuncTable.
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 *            TCPSTACK_SendTxBuffer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send a buffer over the transport connection.
 *
 * Return:    Status of the Send operation.
 */
static ObStatus TCPSTACK_SendTxBuffer(ObexTransportContext con, U8 *buff, U16 len)
{
    ObexPacket             *obp;
    I32                     rcode;
    ObTcpCommonTransport   *txp;

    ASSERT(len <= TCPSTACK_MaxTxSize(con));

    /* Get the correct the TCP transport structure for this callback */
    txp = ContainingRecord((SOCKET *)con, ObTcpCommonTransport, conn);

    rcode = send(*(SOCKET *)con, buff, len, 0);
    if (rcode == len) {
        /* Complete the transmit now */
        obp = ContainingRecord(buff, ObexPacket, buffer);
        ASSERT(obp->buffer == buff);

        /* NOTE: While it is permitted to complete a transmit while in the
         *       context of the request, there is some risk in doing this.
         *       The risk is associated with a recursive loop that develops
         *       between the transmit code and the transmit done code. This
         *       code will recursively loop until the entire OBEX packet
         *       has been sent. If stack overflow is at risk, you must
         *       complete the packets asynchronously or use small OBEX
         *       packet size multiples.
         */
        ASSERT(txp);
        OBPROT_TxDone(txp->app, obp);

        return OB_STATUS_SUCCESS;
    }

    /* Tx Failed! Put the packet back into the queue. */
    OBPROT_ReturnTxBuffer(buff);

    return OB_STATUS_FAILED;
}

/*---------------------------------------------------------------------------
 *            TCPSTACK_DisconnectReq()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect the underlying transport connection.
 *
 * Return:    Status of the Disconnect operation.
 */
static ObStatus TCPSTACK_DisconnectReq(ObexTransportContext con)
{
    I32     rcode;
    ObTcpCommonTransport   *txp;

    /* Get the correct the TCP transport structure for this callback */
    txp = ContainingRecord((SOCKET *)con, ObTcpCommonTransport, conn);

    rcode = closesocket(*(SOCKET *)con);
    ASSERT(rcode != SOCKET_ERROR);
    
    /* We want an asynchronous disconnect completion so post a
     * disconnect message to the event handler and return PENDING.
     */
    PostMessage(asyncEventHandle, WM_SELECT, txp->conn, FD_CLOSE);

    return OB_STATUS_PENDING;
}

/*---------------------------------------------------------------------------
 *            TCPSTACK_MaxTxSize()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the maximum transport protocol data unit that can be
 *            sent over the referenced transport connection.
 *
 * Return:    U16
 *            
 */
static U16 TCPSTACK_MaxTxSize(ObexTransportContext con)
{
    return min(OBEX_TX_BUFF_SIZE, TCP_MAX_TPDU);
}

/****************************************************************************
 *
 * Internal TCP Stack functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            AsyncEventHandler
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process async events from the TCP transports.
 *
 */
static LRESULT CALLBACK AsyncEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    I16    rcode;
    static BOOL busy =      FALSE;
    ObTcpCommonTransport *txp;

    txp = GetTrans((SOCKET)wParam);

    if (busy == TRUE) {
        kal_trace(BT_TRACE_G6_OBEX, OBEX_TRANSPORT_BUSY);
        return 0;
    }
    busy = TRUE;

    if (uMsg != WM_SELECT) {
        busy = FALSE;
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    switch (WSAGETSELECTEVENT(lParam)) {
    case FD_ACCEPT:
        ASSERT(txp);
        kal_trace(BT_TRACE_G6_OBEX,OBEX_ASYNCPROC_SOCKET_xX_RECEIVED_MESSAGE_ACCEPT , wParam);
        if ((WSAGETSELECTERROR(lParam) == 0) && (txp->conn == INVALID_SOCKET)) {
            /* Indicate the Connect request */
            TcpCallback(txp, TCP_EVENT_CONNECT_IND, 0, (U16)wParam);
        }
        break;
            
    case FD_READ:
/*      kal_trace(BT_TRACE_G6_OBEX,OBEX_ASYNCPROC_CLIENT_SOCKET_xX_RECEIVED_MESSAGE_READ , wParam); */
        if (WSAGETSELECTERROR(lParam) == 0) {
            /* receive the data ... */
            U8          buff[TCP_MAX_TPDU];
            int         len = TCP_MAX_TPDU;
            
            len = recv(wParam, buff, len, 0);
            if (len == SOCKET_ERROR) {
                if (WSAGetLastError() == WSAEWOULDBLOCK)
                    break;
                    
                kal_trace(BT_TRACE_G6_OBEX,DATAIND_RECV_FAILED_WSAGETLASTERROR__xD , WSAGetLastError());
                goto IndicateDiscon;    /* ASSERT(0); */
                break;
            }

            /* ASSERT(cTxp || sTxp); */
            if (txp && wParam == txp->conn) {
                TcpCallback(txp, TCP_EVENT_DATA_IND, buff, (U16)len);
            }
        }    
        break;

    case FD_CONNECT:
        kal_trace(BT_TRACE_G6_OBEX,OBEX_ASYNDPROC_SOCKET_xX_RECEIVED_MESSAGE , wParam);
        if (WSAGETSELECTERROR( lParam ) == 0) {
            kal_trace(BT_TRACE_G6_OBEX, OBEX_CONNECTED_TRC);
            /* Connection established */
            goto IndicateConnection;
        } else {
            kal_trace(BT_TRACE_G6_OBEX,OBEX_TCP_CONNECT_FAILED);
            /* Connection failed */
            goto IndicateDiscon;
        }       
        break;

    case FD_WRITE:
        kal_trace(BT_TRACE_G6_OBEX,OBEX_ASYNCPROC_SOCKET_xX_RECEIVED_MESSAGE_WRITE_CONN_ACCEPTED , wParam);
        ASSERT(WSAGETSELECTERROR(lParam) == 0);
IndicateConnection:
        /* Connection established */
        rcode = WSAAsyncSelect(wParam, asyncEventHandle, WM_SELECT, FD_READ|FD_CLOSE);
        ASSERT(rcode != SOCKET_ERROR);

        if (txp && wParam == txp->conn) {
            TcpCallback(txp, TCP_EVENT_CONNECTED, 0, (U16)wParam);
        }
        break;

    case FD_CLOSE:
        kal_trace(BT_TRACE_G6_OBEX,OBEX_ASYNDPROC_SOCKET_xX_RECEIVED_MESSAGE_CLOSE , wParam);
IndicateDiscon:
        if (txp && wParam == txp->conn) {
            TcpCallback(txp, TCP_EVENT_DISCON, 0, 0);
        }
        break;
            
    default:
        ASSERT(0);
        break;
    }  
    busy = FALSE;
    return 0;
}

static HWND InitAsyncCallback(void)
{
    WNDCLASS    wc;
    WSADATA     wsaData;
    BOOL        registered;
    HWND        hWnd = 0;

    /* Init the Winsock Layer */
    if (WSAStartup( 0x0101, &wsaData))
        ASSERT(0);

    OS_MemSet((U8 *)&wc, 0, sizeof(WNDCLASS));
        
    wc.lpfnWndProc = (WNDPROC)AsyncEventHandler;
    wc.lpszClassName = "OBEX TCP Class";
    wc.cbWndExtra = sizeof(LONG);

    registered = RegisterClass(&wc);
    if (!registered) {
        kal_trace(BT_TRACE_G6_OBEX,OBEX_SELECT_WINDOW_REGISTRATION_FAILED_xX , GetLastError());
        return 0;
    }
        
    hWnd = CreateWindow("OBEX TCP Class", NULL, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

    return hWnd;
}

static ObTcpCommonTransport *GetTrans(SOCKET skt) 
{
    ListEntry *curNode;
    ObTcpCommonTransport *txp;

    /* Look for a matching socket on the client list */
    if (!IsListEmpty(&clientList)) {
        curNode = clientList.Flink; /* First Element */
        while (curNode != &clientList) {
            txp = (ObTcpCommonTransport *)curNode;
            if (txp->conn == skt) {
                /* We found the client transport that uses this socket! */
                return txp;
            }
            curNode = curNode->Flink;
        }
    }

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
    /* Must be a socket on the server list */
    if (!IsListEmpty(&serverList)) {
        curNode = serverList.Flink; /* First Element */
        while (curNode != &serverList) {
            txp = (ObTcpCommonTransport *)curNode;
            if ((txp->conn == skt) || 
                (((ObTcpServerTransport *)txp)->serverReg == skt)) {
                /* We found the server transport that uses this socket! */
                return txp;
            }
            curNode = curNode->Flink;
        }
    }
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

    return 0;
}

#endif /* defined(TCP_STACK) && TCP_STACK == XA_ENABLED */
