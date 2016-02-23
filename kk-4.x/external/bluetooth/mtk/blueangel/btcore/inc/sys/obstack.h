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

#ifndef __OBSTACK_H
#define __OBSTACK_H
/****************************************************************************
 *
 * File:        obstack.h
 *
 * Description: This file specifies the interface between the system 
 *              dependent portion of the OBEX transport and the OBEX packet 
 *              parsers.
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
#include <sys/obstkif.h>
#include <sys/obprotif.h>
#include <osapi.h>
#include <utils.h>
#if IRDA_STACK == XA_ENABLED
#include <irias.h>
#endif /* IRDA_STACK == XA_ENABLED */

#if !defined(IR_LMP_DISCONNECT)
#error Missing define for IR_LMP_DISCONNECT
#endif /* !defined(IR_LMP_DISCONNECT) */

/*---------------------------------------------------------------------------
 * OBEX System Independent Transport Adapter API layer
 *
 *     Transport adapters are used to adapt the OBEX protocol layer to
 *     various transport layer interfaces. Multi-Transport OBEX enables
 *     multiple transports to exist concurrently. The OBEX
 *     protocol layer accesses the transports through this API.
 *
 *     Obstack.h defines the interface between OBEX and the transport
 *     multiplexer.  This API is used directly by the OBEX Client & Server
 *     protocol components. This separation facilitates isolation of the
 *     OBEX protocol layers from the transport itself.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

#if JETTEST == XA_ENABLED

/*---------------------------------------------------------------------------
 * ObexSniffType type
 *
 *     Allows a sniffer to determine whether the data was detected during
 *     tx or rx.
 */
typedef U8 ObexSniffType;

#define OBSNIFF_RX 0
#define OBSNIFF_TX 1

/* End of ObexSniffType */

/*---------------------------------------------------------------------------
 * OB_Sniff() function
 *
 *      A function of this type is stored in ObInternalData, and is called
 *      whenever a data transmission/reception event is detected. 
 *
 * Parameters:
 *      handle - Handle that uniquely identifies the OBEX entity involved
 *
 *      type - Type of data (tx or rx).
 *
 *      buff - Pointer to data. Data in "buff" is valid only during this call.
 *
 *      len - Length of data in "buff".
 */
typedef void (*OB_Sniff)(void *obs, ObexSniffType type,
                         const U8 *buff, U16 len);
#endif

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

typedef struct _ObPacketSendingStatus
{
    ObexPacket      *obxPacket;
    EvmTimer        timerId;
    void            *con;
    U8               timerStart;
    U8               boundTest; /// always 0
}ObSendingPacket;

/*--------------------------------------------------------------------------
 * ObInternalData structure
 * 
 *     Structure for managing internal Obex data.  This includes:
 *     - the transmit packet pool, which is shared by all clients and 
 *       servers.  
 *     - list entries to manage a list of app handles for each registered 
 *       client and server registered.  
 *     - an IAS query reference to keep track of the transport 
 *       structure used in the IrDA transport.
 */
typedef struct _ObInternalData
{
    BOOL           initialized;
    U8             obexLock;

    /* Group: Obex Packet Pool */

    /* Packet Pool shared by all stack connections */
    ListEntry      packetList;
    ObexPacket     packets[OBEX_NUM_TX_PACKETS];
    U8             packetnum;

    /* Group: List entries for app handle lists */
    ListEntry      clientAppHndlList; /* List entry for client app handle list */
    ListEntry      serverAppHndlList; /* List entry for server app handle list */
    /* Group: Current node for app handle lists */
    ListEntry      *curClientNode; /* Current node for client app handle list */
    ListEntry      *curServerNode; /* Current node for server app handle list */

    ObexClientCallbackParms     clientParms;              
    ObexServerCallbackParms     serverParms;              

#if IRDA_STACK == XA_ENABLED
    /* IAS query reference for IrDA transport */
    IrIasQuery     *iasQuery;
#endif /* IRDA_STACK == XA_ENABLED */

#if JETTEST == XA_ENABLED
    /* Callback to receive OBEX transport data events */
    OB_Sniff        sniffer;
#endif /* JETTEST == XA_ENABLED */


    ObSendingPacket    p_sending_pkt[OBEX_NUM_TX_PACKETS];
    
} ObInternalData;


/****************************************************************************
 *
 * Section: Generalized Transport Functions
 *
 ****************************************************************************
 *
 *     These are generalized OBEX Transport API functions which are used
 *     to initialize the transports(s), send data, and manage connections.
 *     They either internally call transport-specific versions of the same
 *     functions or are defined as macros which map to transport-specific
 *     functions (except for OBSTACK_GetTxBuffer, which has no equivalent).
 *     For example, OBSTACK_ClientInit() in turn calls BTSTACK_ClientInit.
 *     These OBSTACK_* functions are called by both the application (via
 *     the OBEX API) and/or the OBEX parser.
 *
 ****************************************************************************/

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBSTACK_ClientInit()
 *
 *     Initialize the OBEX Stack Client Component, including the transport
 *     client component. The client is normally dormant after initialization
 *     until a connection request is made.
 *
 * Parameters:
 *     AppHndl - Handle of application "reserving" this instance. This
 *         handle should be used by the transport to
 *         identify which instance is referenced during future calls
 *         to ClientDeinit(), ClientConnect() or ClientDisconnect().
 *
 *     Protocols - Bit mask specifying which transport protocols to 
 *         initialize for the clients use.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The specified instance was initialized.
 *
 *     OB_STATUS_xxxxxxx - The instance could not be initialized. Do
 *                          not return OB_STATUS_PENDING.
 */
ObStatus OBSTACK_ClientInit(ObexClientApp *ObexApp, U8 Protocols);

ObStatus OBSTACK_ClientSRMAbort(ObexClientApp *ObexApp);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBSTACK_ClientDeinit()
 *
 *     Shutdown the OBEX Stack Client Component. Deinitializes the transport
 *     interface and frees any resources. This function can fail if the
 *     transport is busy managing an active connection.
 *
 * Requires:
 *      OBEX_DEINIT_FUNCS set to XA_ENABLED.
 *
 * Parameters:
 *     AppHndl - Application handle provided during init. This handle is
 *         used by the transport to identify which
 *         instance to deinitialize.
 * Returns:
 *     TRUE - The stack component was shutdown successfully.
 *
 *     FALSE - The client is busy and cannot be shutdown.
 */
BOOL OBSTACK_ClientDeinit(ObexClientApp *ObexApp);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBSTACK_ServerInit()
 *
 *     Initialize the OBEX Stack Server Component, including the transport
 *     server component. The server will normally perform service
 *     registration in this function. The server then sits idle until a
 *     connection is established to it from a remote device.
 *
 * Parameters:
 *     AppHndl - Handle of application "reserving" this instance. This
 *         handle should be used by the transport to
 *         identify which instance is referenced during future calls
 *         to ServerDeinit().
 *
 *     Protocols - Bit mask specifying which transport protocols to 
 *                initialize for the servers use.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The specified instance was initialized.
 *
 *     OB_STATUS_xxxxxxx - The instance could not be initialized. Do
 *                          not return OB_STATUS_PENDING.
 */
ObStatus OBSTACK_ServerInit(ObexServerApp *ObexApp, U8 Protocols);

ObStatus OBSTACK_ServerSRMAbort(ObexServerApp *ObexApp);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBSTACK_ServerDeinit()
 *
 *     Shutdown the OBEX Stack Server Component. Deinitializes the transport
 *     interface and frees any resources. This function can fail if the
 *     transport is busy managing an active connection.
 *
 * Requires:
 *      OBEX_DEINIT_FUNCS set to XA_ENABLED.
 *
 * Parameters:
 *     AppHndl - Application handle provided during init. This handle is
 *         used by the transport to identify which
 *         instance to deinitialize.
 * Returns:
 *     TRUE - The stack component was shutdown successfully.
 *
 *     FALSE - The server is busy and cannot be shutdown.
 */
BOOL OBSTACK_ServerDeinit(ObexServerApp *ObexApp);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

/*---------------------------------------------------------------------------
 * OBSTACK_GetTxBuffer()
 *
 *     Return a pointer to a buffer in which to transmit an OBEX packet.
 *     The OBEX packet may be sent in multiple buffers so the buffer does
 *     not have to be big enough to hold a whole OBEX packet. The size of
 *     buffer is returned by OBSTACK_MaxTxSize.
 *
 * Returns:
 *     Pointer to transmit buffer, or 0 if no buffers are available.
 */
U8 *OBSTACK_GetTxBuffer(void);

/*---------------------------------------------------------------------------
 * OBSTACK_IsTxBufferAvailable()
 *
 * Returns:
 *     Is there at least one packets for l2cap sending.
 */
U8 OBSTACK_IsTxBufferAvailable(void);


/*---------------------------------------------------------------------------
 * OBSTACK_SendTxBuffer()
 *
 *     Send the transmit buffer over the transport connection.
 *
 * Parameters:
 *     trans - OBEX client transport to send buffer over.
 *
 *     buff - Pointer to buffer to send.
 *
 *     len - Number of bytes in the buffer.
 *
 * Returns:
 *     OB_STATUS_PENDING - Means operation successful
 *
 *     OB_STATUS_FAILED - Usually an IrUltra error or unexpected link drop.
 *
 *     OB_STATUS_MEDIA_BUSY - Operation failed because the media is busy.
 */
ObStatus OBSTACK_SendTxBuffer(ObexTransport *trans, U8 *buff, U16 len);

#define OBSTACK_SendTxBuffer(_SIF, _BUF, _LEN) \
        (Assert(_SIF),(_SIF)->funcTab->SendTxBuffer((_SIF)->context, (_BUF), (_LEN)))


/*---------------------------------------------------------------------------
 * OBSTACK_IsConnectionUp()
 *
 *     Returns status of specified Transport connection.
 *
 * Parameters:
 *     trans - OBEX client transport used to query connection state.
 *
 * Returns:
 *     TRUE - Connection is up.
 *
 *     FALSE - Connection is not up.
 */
BOOL OBSTACK_IsConnectionUp(ObexTransport *trans);


/*---------------------------------------------------------------------------
 * OBSTACK_Disconnect()
 *
 *     Disconnect the server connection. The disconnect is complete
 *     when the parser's ObDisconnectInd function is called.
 *
 * Parameters:
 *     trans - OBEX client transport used to disconnect connection.
 *
 * Returns:
 *     Status of Disconnect operation.
 */
ObStatus OBSTACK_Disconnect(ObexTransport *trans);

#define OBSTACK_Disconnect(_SIF) \
        (Assert(_SIF),(_SIF)->funcTab->Disconnect((_SIF)->context))


/*---------------------------------------------------------------------------
 * OBSTACK_MaxTxSize()
 *
 *     Return maximum TPDU size that can be sent on the connection.
 *
 * Parameters:
 *     trans - OBEX client transport used to return information.
 *
 * Returns:
 *     Size of transport packet data area.
 */
U16 OBSTACK_MaxTxSize(ObexTransport *trans);

#define OBSTACK_MaxTxSize(_SIF) \
        (Assert(_SIF),(_SIF)->funcTab->MaxTxSize((_SIF)->context))


#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBSTACK_VerifyClientAppHandle()
 *
 *     Verifies that the passed in AppHandle is actually on the list
 *     of registered client AppHandles.
 *
 * Parameters:
 *     ObexApp - AppHandle to verify.
 *
 * Returns:
 *     Status of AppHandle on the registered client list.
 */
BOOL OBSTACK_VerifyClientAppHandle(ObexAppHandle *ObexApp);
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * OBSTACK_VerifyServerAppHandle()
 *
 *     Verifies that the passed in AppHandle is actually on the list
 *     of registered server AppHandles.
 *
 * Parameters:
 *     ObexApp - AppHandle to verify.
 *
 * Returns:
 *     Status of AppHandle on the registered client list.
 */
BOOL OBSTACK_VerifyServerAppHandle(ObexAppHandle *ObexApp);
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

void OS_LockObex(void);
void OS_UnlockObex(void);
BOOL IsObexLocked(void);

/* RFCOMM Tx timer */
void OBPROT_StartTxTimer(void *con, ObexPacket *obxPkt);
void OBPROT_StopTxTimer(ObexPacket *obxPkt);
#if 0   // MAUI w0946
BOOL OBPROT_ReturnTxBuffer_IfNeed(void *con);
void OBPROT_GivePktToWaitingApp_IfNeed(ObexAppHandle *AppHndl);
#endif

#endif /* __OBSTACK_H */
