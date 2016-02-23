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
 * File:          obxoemstk.c
 *
 * Description:   This file contains the code for OBEX OEM protocol stack
 *                interface module.
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
#include <sys/obxoemstk.h>

#if defined(OEM_STACK) && OEM_STACK == XA_ENABLED

/****************************************************************************
 *
 * Prototypes for Internal Functions
 *
 ***************************************************************************/
/* Prototypes for functions provided in the ObTransFuncTable */
static ObStatus OEMSTACK_DisconnectReq(ObexTransportContext con);
static U16      OEMSTACK_MaxTxSize(ObexTransportContext con);
static ObStatus OEMSTACK_SendTxBuffer(ObexTransportContext con, U8 *buff, U16 len);
static BOOL     OEMSTACK_GetTpConnInfo(ObexTransportContext  con,
                                       ObexTpConnInfo       *tpConnInfo);
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
static BOOL     OEMSTACK_IsRxFlowAvailable(ObexTransportContext context);
static void     OEMSTACK_PauseRxFlow(ObexTransportContext context);
static void     OEMSTACK_ResumeRxFlow(ObexTransportContext context);
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

/****************************************************************************
 *
 * RAM data
 *
 ****************************************************************************/

/****************************************************************************
 *
 * ROMable data
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * OEM Transport function call table. These are the functions exported to the
 * OBEX Parser by this transport.
 */
static const ObTransFuncTable OemStkFuncTable = {
    OEMSTACK_DisconnectReq,
    OEMSTACK_MaxTxSize,
    OEMSTACK_SendTxBuffer,
    OEMSTACK_GetTpConnInfo,
#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
    OEMSTACK_IsRxFlowAvailable, OEMSTACK_PauseRxFlow, OEMSTACK_ResumeRxFlow
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */
};

#if (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            OEMSTACK_ClientConnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initiate a transport connection to the specified OBEX device.
 *            This function will usually take the address of the target device
 *            as a parameter. Where necessary, this function will usually
 *            initiate service discovery and lower layer connection procedures.
 *
 * Return:    OB_STATUS_PENDING - operation is started successfully
 *            OB_STATUS_SUCCESS - operation is successful
 *            OB_STATUS_BUSY - operation failed because the client is busy.
 */
ObStatus OEMSTACK_ClientConnect( ObexOemTarget         *target, 
                                 ObOemClientTransport  *xpt,
                                 ObexTransport        **trans)
{
    /* Initiate a connection to the specified target, including 
     * service discovery, if applicable.
     */

    return OB_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            OEMSTACK_ClientDisconnect()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect the client's transport connection if one exists.
 *
 * Return:    OB_STATUS_PENDING - disconnect was started successfully.
 *            OB_STATUS_FAILED - the connection could not be disconnected.
 *            OB_STATUS_NO_CONNECT - there is no connection to disconnect.
 *
 */
ObStatus OEMSTACK_ClientDisconnect(ObOemClientTransport *xpt)
{
    /* Make sure we are either connected or in the process 
     * of connecting the transport.
     */
    if ((xpt->state != OCS_CONNECTING) && (xpt->state != OCS_CONNECTED)) {
        return OB_STATUS_NO_CONNECT;
    }

    /* Initiate a disonnection to the connected device. */

    return OB_STATUS_SUCCESS;
}

/****************************************************************************
 *
 * OEM Stack functions accessed via the obstack.c Init & Deinit functions.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            OEMSTACK_ClientInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Client OEM transport. This function is
 *            called by OBSTACK_ClientInit() in obstack.c.
 *
 * Return:    OB_STATUS_SUCCESS - Initialized successfully
 *            OB_STATUS_FAILED - Unable to bind to the stack.
 *
 */
ObStatus OEMSTACK_ClientInit(ObOemClientTransport *xpt, ObexAppHandle *app)
{
    /* Save the application handle for event indications. */
    xpt->appHandle = app;

    /* Initialize the ObexTransport structure passed to the OBEX protocol
     * when a connection is established.
     */
    xpt->flags = 0;
    xpt->state = OCS_IDLE;
    xpt->trans.connected = &xpt->state;
    xpt->trans.funcTab = &OemStkFuncTable;
    xpt->trans.context = (ObexTransportContext)xpt;
    xpt->trans.tpType = OBEX_TP_OEM;

    return OB_STATUS_SUCCESS;
}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OEMSTACK_ClientDeinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Client OEM transport. This function
 *            returns success if the client is not initialized. This function
 *            is called in obstack.c.
 *
 * Return:    TRUE -  Client stack deinitialized.
 *            FALSE - Client cannot deinitialize.
 *
 */
BOOL OEMSTACK_ClientDeinit(ObOemClientTransport *xpt)
{
    /* The client is already disconnected */
    if (xpt->state == OCS_UNINITIALIZED) {
        return TRUE;
    }

    /* The client device is still connected */
    if (xpt->state != OTS_DISCONNECT) {
        return FALSE;
    }

    xpt->state = OCS_UNINITIALIZED;
    xpt->appHandle = 0;

    return TRUE;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
#endif /* (OBEX_ROLE_CLIENT == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */

#if (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED)
/*---------------------------------------------------------------------------
 *            OEMSTACK_ServerInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Server OEM transport. This function will
 *            usually perform service registration, where necessary. This
 *            function is called by OBSTACK_ServerInit() in obstack.c.
 *
 * Return:    OB_STATUS_SUCCESS - Initialized successfully
 *            OB_STATUS_FAILED - Unable to bind to the stack.
 *            OB_STATUS_NO_RESOURCES - Out of resources
 *
 */
ObStatus OEMSTACK_ServerInit(ObOemServerTransport *xpt, ObexAppHandle *app)
{
    /* Save the application handle for event indications. */
    xpt->appHandle = app;

    /* Initialize the ObexTransport structure passed to the OBEX protocol
     * when a connection is established.
     */
    xpt->flags = 0;
    xpt->state = OCS_IDLE;
    xpt->trans.connected = &xpt->state;
    xpt->trans.funcTab = &OemStkFuncTable;
    xpt->trans.context = (ObexTransportContext)xpt;
    xpt->trans.tpType = OBEX_TP_OEM;

    return OB_STATUS_SUCCESS;
}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OEMSTACK_ServerDeinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Server OEM transport. This function returns
 *            success if the server is not initialized. This function is called
 *            in obstack.c.
 *
 * Return:    TRUE -  Server stack deinitialized.
 *            FALSE - Server cannot deinitialize.
 *
 */
BOOL OEMSTACK_ServerDeinit(ObOemServerTransport *xpt)
{
    /* The server is already disconnected */
    if (xpt->state == OCS_UNINITIALIZED) {
        return TRUE;
    }

    /* The server device is still connected */
    if (xpt->state != OTS_DISCONNECT) {
        return FALSE;
    }
   
    xpt->state = OCS_UNINITIALIZED;
    xpt->appHandle = 0;

    return TRUE;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
#endif /* (OBEX_ROLE_SERVER == XA_ENABLED) || (OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED) */


/****************************************************************************
 *
 * OEM Stack functions accessed via the ObTransFuncTable.
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 *            OEMSTACK_SendTxBuffer()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send a buffer over the existing OEM transport connection. An
 *            ObexPacket pool is allocated by OBEX and retrieved by calls
 *            to OBSTACK_GetTxBuffer() from within OBEX.  OBEX will then call 
 *            down to the transport to send out each OBEX packet through a 
 *            call to this function. This function should send the entire
 *            contents of the OBEX packet through the OEM transport and 
 *            signal OBEX when the packet has been completed, or failed, so
 *            the packet can be returned to the ObexPacket pool.
 *
 * Return:    OB_STATUS_PENDING - transmit operation is started successfully
 *            OB_STATUS_SUCCESS - transmit operation is successful
 *            OB_STATUS_FAILED - transmit operation failed
 */
static ObStatus OEMSTACK_SendTxBuffer(ObexTransportContext con, U8 *buff, U16 len)
{
    ObexPacket     *obp = ContainingRecord(buff, ObexPacket, buffer);
    ObOemTransport *xpt = (ObOemTransport *)con;

    /* The packet being sent should not exceed the maximum 
     * allowed transmit size by the OEM transport.
     */
    ASSERT(len <= OEMSTACK_MaxTxSize(con));

    /* Fill in the fields in the OemPacket structure in obprotif.h */
    obp->pkt.oem.length = len;
    obp->pkt.oem.buff = buff;

    /* Transmit the buffer utilizing the fields from the OemPacket structure
     * filled in above. Changes can be made to this structure and the 
     * initialization above if alternate fields are needed to send data 
     * through the OEM transport layer. 
     */

    /* NOTE: While it is permitted to complete a transmit while in the
     *       context of the request, there is some risk in doing this.
     *       The risk is associated with a recursive loop that develops
     *       between the transmit code and the transmit done code. This
     *       code will recursively loop until the entire OBEX packet
     *       has been sent. If stack overflow is at risk, you must
     *       complete the packets asynchronously or use small OBEX
     *       packet size multiples.
     */
    OBPROT_TxDone(xpt->appHandle, obp);

    return OB_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            OEMSTACK_DisconnectReq()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect the underlying OEM transport connection.
 *
 * Return:    OB_STATUS_PENDING - disconnect operation is started successfully
 *            OB_STATUS_SUCCESS - disconnect operation is successful
 *            OB_STATUS_FAILED - disconnect operation failed
 */
static ObStatus OEMSTACK_DisconnectReq(ObexTransportContext con)
{
    ObOemTransport *xpt = (ObOemTransport *)con;

    /* Close the OEM transport connection */

    return OB_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            OEMSTACK_MaxTxSize()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the maximum transport protocol data unit that can be
 *            sent over the referenced OEM transport connection.
 *
 * Return:    U16
 *            
 */
static U16 OEMSTACK_MaxTxSize(ObexTransportContext con)
{
    ObOemTransport *xpt = (ObOemTransport *)con;

    /* Return the maximum TPDU size. This should be the smaller of
     * the OBEX_TX_BUFF_SIZE and the transports PDU size.
     */
    return OBEX_TX_BUFF_SIZE;
}

/*---------------------------------------------------------------------------
 *            OEMSTACK_GetTpConnInfo()
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
static BOOL OEMSTACK_GetTpConnInfo(ObexTransportContext  con,
                                   ObexTpConnInfo       *tpConnInfo)
{
    ObOemTransport  *xpt = (ObOemTransport *)con;

    /* Make sure the transport is connected */
    if (xpt->state != OCS_CONNECTED) {
        return FALSE;
    }

    /* Fill in the ObexTpConnInfo structure located in obex.h. Any OEM
     * transport specific fields can be added in obex.h as necessary. Such
     * fields might be used to track additional information about the remote
     * device, transport connection, or the overall performance statistics 
     * of the transport connection. 
     */
    tpConnInfo->tpType = OBEX_TP_OEM;

    return TRUE;
}

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
/****************************************************************************
 *
 * NOTE: These functions are only required if transport flow control is to be
 * used over the OEM transport. It should be noted that another form of flow
 * control exists in OBEX, called OBEX packet flow control (see the
 * OBEX_PACKET_FLOW_CONTROL option in obconfig.h). Both flow control options
 * provide applications with the ability to control the flow of data in
 * order to minimize the amount of memory needed for larger OBEX operations.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            OEMSTACK_IsRxFlowAvailable()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Allows the application to determine if a transport based 
 *            receive flow control is available.
 *
 * Return:    TRUE -  Receive flow control is available.
 *            FALSE - Receive flow control is not available.
 */
static BOOL OEMSTACK_IsRxFlowAvailable(ObexTransportContext context)
{
    ObOemTransport  *xpt = (ObOemTransport *)con;

    /* Add code to determine transport flow control status */

    return FALSE;
}

/*---------------------------------------------------------------------------
 *            OEMSTACK_PauseRxFlow()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Tells the transport layer to stop the remote device from
 *            sending data by refusing the flow of data through the transport. 
 *            Once this function has been called, no more than the number of 
 *            bytes or packets reported by OBEX_GetTpConnInfo() should be
 *            received. This amount includes any currently indicated data 
 *            (if called during a data indication). Flow can be resumed
 *            with OBEX_ResumeRxFlow().
 *               
 * Return:    void
 *
 */
static void OEMSTACK_PauseRxFlow(ObexTransportContext context)
{
    ObOemTransport *xpt = (ObOemTransport *)con;
    
    /* Flag that transport flow has ceased */
    xpt->flags |= OTF_FLOW;

    /* Add any additional code to pause the flow of data through the transport */
}

/*---------------------------------------------------------------------------
 *            OEMSTACK_ResumeRxFlow()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Opens the receive window after flow control was exerted.
 *
 * Return:    void
 *
 */
static void OEMSTACK_ResumeRxFlow(ObexTransportContext context)
{
    ObOemTransport *xpt = (ObOemTransport *)con;

    ASSERT(xpt->state == OCS_CONNECTED);

    /* Flag that transport flow has resumed */
    xpt->flags &= ~OTF_FLOW;

    /* Add any additional code to resume the flow of data through the transport */
}
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

/****************************************************************************
 *
 * Internal OEM Stack functions
 *
 ****************************************************************************/

/* The transport layer communicates events to OBEX through the OBPROT_ API's
 * described in obprotif.h. Brief explanations of the most used OBPROT_
 * functions are described below.  See obprotif.h for full details on each
 * of these functions. The OEM transport must call these functions to properly 
 * interact with OBEX. In many cases, as in our IrDA, Bluetooth, and TCP
 * transports for OBEX, these OBPROT_ calls can be made within a transport 
 * callback function that is used to process events. Other methods of 
 * receiving events from the transport layer may be used, but the OBPROT_ 
 * functions must be called.
 *
 * OBPROT_LinkTransport - associates a server application handle with the 
 *                        OBEX transport being used.
 * OBPROT_Connected - notifies OBEX that the transport connection is up. 
 * OBPROT_Disconnected - notifies OBEX that the transport connection is down.
 * OBPROT_ParseRx - indicates received data to the OBEX protocol parser.
 * OBPROT_TxDone - returns an ObexPacket to the packet pool and signals more 
 *                 data can be sent.
 * OBPROT_RetRxBuffer - returns an ObexPacket to the packet pool if a 
 *                      transmit fails.
 */
#endif /* defined(OEM_STACK) && OEM_STACK == XA_ENABLED */
