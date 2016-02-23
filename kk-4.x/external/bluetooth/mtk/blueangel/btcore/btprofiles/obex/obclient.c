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
 * File:        obclient.c
 *
 * Description: This file contains the code for the Client portion of 
 *              OBEX Command Interpreter and OBEX Layer API.
 *
 * Created:     May 7, 1997
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
 * Aug 7 2008 mbj06032
 * [MAUI_01099468] [BT OBEX] Client send PUT opcode with FinalBit in the las data packet
 *  Client send PUT opcode with FinalBit in the las data packet
 *
 * Jun 17 2008 mbj06032
 * [MAUI_01077981] [BT]ASSERT Fail: observer.c 1085 -BT
 * 
 *
 * Jun 11 2008 mbj06053
 * [MAUI_01070863] BPP revise check in
 * 
 *
 * Mar 20 2008 mbj06032
 * [MAUI_00096220] BIP gets disconnected in the mentioned scenario.
 * add trace
 *
 * Jan 9 2008 mbj06032
 * [MAUI_00515012] [BT][BIP, GOEP, OBEX] add "$Log$" in file header
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include <sys/obxalloc.h>
#include <sys/obex_utl.h>
#undef OSC
#undef OSH
#undef OSS

#include "bluetooth_trc.h"

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

#if OBEX_ROLE_CLIENT == XA_ENABLED
/****************************************************************************
 *
 * Function prototypes
 *
 ****************************************************************************/

static void ClientComplete(ObexClientApp *ObexApp, ObClientEvent event);
void ProcessSimpleOpEvents(ObexClientApp *ObexApp, ObexParserEvent event);
void ProcessPutEvents(ObexClientApp *ObexApp, ObexParserEvent event);
void ProcessGetEvents(ObexClientApp *ObexApp, ObexParserEvent event);
#if OBEX_SESSION_SUPPORT == XA_ENABLED
static BOOL ObClientSessionLayerEventInd(ObexClientApp *ObexApp, ObexParserEvent Event);
void ProcessSessionEvents(ObexClientApp *ObexApp, ObexParserEvent event);
static void ClientSaveFirst(ObexClientApp *ObexApp, ObexOpcode opcode, U32 param2);
static void ClientSessionCommandAcked(ObexClientApp *ObexApp);
static void ClientSessionResponseReceived(ObexClientApp *ObexApp);
static void ClientRestoreState(ObexClientApp *ObexApp);
static void ClientClearSavedState(ObexClientApp *ObexApp);
static void SendNextPacket(ObexClientApp *ObexApp);
static void SuspendSession(ObexClientApp *ObexApp);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            OBEX_ClientInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX Client command interpreter.
 *
 * Return:    OB_STATUS_SUCCESS - Initialized successfully
 *            OB_STATUS_FAILED - Unable to initialize the OBEX Components.
 *            OB_STATUS_INVALID_PARM - A parameter is invalid or ther client
 *                is already registered.
 */
ObStatus OBEX_ClientInit(ObexClientApp *ObexApp, ObClientAppCb ClientCb, 
                         ObexTpType Protocols)
{
    ObStatus    status;
#if BT_GOEP_V2 == XA_ENABLED
	U8 i;
#endif

    OS_LockObex();

    /* Check if the Client is already in use */
#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObexApp || !ClientCb) {
        status = OB_STATUS_INVALID_PARM;
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(ObexApp && ClientCb);

    /* Initialize the Transport Component */
    if ((status = OBSTACK_ClientInit(ObexApp, Protocols)) != OB_STATUS_SUCCESS) {
        goto Done;
    }
    
    /* Initialize the Client Context - OBSTACK_ClientInit will make sure we aren't
     * trying to initialize a duplicate client
     */
    OS_MemSet((U8 *)(&ObexApp->client), 0, sizeof(ObClientContext));

    /* Verify the AppHandle and Object Store Function table */
#if XA_ERROR_CHECK == XA_ENABLED
    if (!OBSTACK_VerifyClientAppHandle(&ObexApp->handle)) {
        status = OB_STATUS_INVALID_PARM;
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(OBSTACK_VerifyClientAppHandle(&ObexApp->handle));

    /* Initialize some command interpreter variables */
    OCC(appCallback) = ClientCb;

    OCH(txLength) = 0;
    
    OCC(currentOperation) = NO_OPER_EVENTS;

    OCC(obConnId) = OBEX_INVALID_CONNID;

#if OBEX_SESSION_SUPPORT == XA_ENABLED
    OCC(protoTxLen) = 0;
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

    /* Initialize the client parser */
    ObParserSetModeClient(OCH(parser), ObClientEventInd);
    ObParserSetInitState(&OCH(parser));
    
#if BT_GOEP_V2 == XA_ENABLED
    /* init all l2buffer member and assign raw buffer*/
    OS_MemSet( (U8 *)&ObexApp->l2buffer, 0, sizeof(ObexApp->l2buffer) );
    InitializeListHead( &ObexApp->l2buffer.rxPacketsBufferList );
    InitializeListHead( &ObexApp->l2buffer.receivedPacketsList );
    ObexApp->l2buffer.rxNum = 0;
        
    for (i = 0; i < GOEPL2_MAX_RETRANS_NUM ; i++)
	{
		InitializeListEntry( &(ObexApp->l2buffer.rfcRxPackets[i]).node );
		ObexApp->l2buffer.rfcRxPackets[i].data = NULL;
	}
    
    /* configure the transport init l2cap data */
    ObexApp->trans.ObexClientBtTrans.client.pl2Buffer = &ObexApp->l2buffer;
#endif

    status = OB_STATUS_SUCCESS;

Done:
    OS_UnlockObex();
    return status;

}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBEX_ClientDeinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  De-initializes OBEX client parser.
 *
 * Return:    OB_STATUS_SUCCESS - Successfully deinitialized.
 *            OB_STATUS_BUSY - Unable to deinitialize because the client is 
 *                active.
 *            OB_STATUS_INVALID_PARM - Invalid parameters.
 */
ObStatus OBEX_ClientDeinit(ObexClientApp *ObexApp)
{
    ObStatus status;

    OS_LockObex();

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_DEINIT);
    

#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObexApp || !OBSTACK_VerifyClientAppHandle(&ObexApp->handle)) {
        status = OB_STATUS_INVALID_PARM;
        goto Done;
    }
/* For GOEP deinit
    if (OCC(currentOperation) != NO_OPER_EVENTS) {
        status = OB_STATUS_BUSY;
        goto Done;
    }
*/
#endif /* XA_ERROR_CHECK == XA_ENABLED */    
/* For GOEP deinit
    ASSERT(ObexApp && OCC(currentOperation) == NO_OPER_EVENTS && 
           OBSTACK_VerifyClientAppHandle(&ObexApp->handle));
*/
    if (OBSTACK_ClientDeinit(ObexApp) == FALSE) {
        status = OB_STATUS_FAILED;
        goto Done;
    }        

    OCC(appCallback) = 0;
    status = OB_STATUS_SUCCESS;

Done:    
    OS_UnlockObex();
    return status;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            OBEX_ClientSendRequest()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Issues an OBEX command. Used to initiate client OBEX
 *            Connect, Disconnect, Get & SetPath Operations.
 *
 * Return:    OB_STATUS_PENDING
 *            OB_STATUS_BUSY
 *            OB_STATUS_INVALID_HANDLE
 *            OB_STATUS_PACKET_TOO_SMALL
 *            OB_STATUS_NO_CONNECT - No transport connection exists.
 *            OB_STATUS_INVALID_PARM - Invalid parameters.
 */
ObStatus OBEX_ClientSendRequest(ObexClientApp *ObexApp, ObexOpcode Opcode, 
                                U32 param1, U32 param2)
{
    ObexSetPathFlags    pathFlags;
    ObStatus            status = OB_STATUS_PENDING;
    U16                 hbLength;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_SENDREQUEST, Opcode);

    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    /* Verify the AppHandle is registered */
    if (!ObexApp || !OBSTACK_VerifyClientAppHandle(&ObexApp->handle)) {
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_SENDREQUEST_INVALIDPARAM);
        status = OB_STATUS_INVALID_PARM;
        goto Exit;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(ObexApp && OBSTACK_VerifyClientAppHandle(&ObexApp->handle));

    hbLength = OCH(txLength);

#if XA_ERROR_CHECK == XA_ENABLED
    if (OCC(currentOperation) != NO_OPER_EVENTS) {
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_SENDREQUEST_BUSY);
        status = OB_STATUS_BUSY;
        goto Exit;
    }
    
    /* Assume failure, reset header buffer */
    OCH(txLength) = 0;

    if (!OBSTACK_IsConnectionUp(OCH(parser.stack))) {
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_SENDREQUEST_NOCONNECT);
        status = OB_STATUS_NO_CONNECT;
        goto Exit;
    }
    
    if (hbLength > ObClientMaxHeaderBlockSize(Opcode)) {
        /* Cannot send headers in a single packet so return an error. */
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_SENDREQUEST_TOOSMALL);
        status = OB_STATUS_PACKET_TOO_SMALL;
        goto Exit;
    } 
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    
    ASSERT(OCC(currentOperation) == NO_OPER_EVENTS && OBSTACK_IsConnectionUp(OCH(parser.stack)));
    ASSERT(hbLength <= ObClientMaxHeaderBlockSize(Opcode));
    
    OCH(txLength) = 0;

    /* Initialize the operation */
    switch (Opcode) {

        case OB_OPCODE_GET:

            OCC(obsh) = (void *)param1;
            OCC(currentOperation) = GET_OPER_EVENTS;
            break;
            
        case OB_OPCODE_SET_PATH:
            pathFlags = (U8)param1 & (OSPF_BACKUP|OSPF_DONT_CREATE);
            ObClientSetPathFlags(pathFlags);
            
        default:
            ASSERT( (Opcode == OB_OPCODE_CONNECT) || 
                    (Opcode == OB_OPCODE_DISCONNECT) ||
                    (Opcode == OB_OPCODE_SET_PATH) );
            
            OCC(currentOperation) = SIMPLE_OPER_EVENTS;
            break;
    }

    OCC(opcode) = Opcode;

#if OBEX_SESSION_SUPPORT == XA_ENABLED
    /* Save the current client state, just in case the transport disconnects */
    ClientSaveFirst(ObexApp, Opcode, param2);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

    OCC(txOffset) = 0;
    OCC(status) = OBRC_SUCCESS;
    OCC(txLength) = hbLength;

    ObParserSetInitState(&OCH(parser));

    if (param2 == 0)
        Opcode |= OB_FINAL_BIT;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENTSENDCOMMAND, Opcode);

    /* Send the command */
    OCC(unackedPackets)++;
    ObClientSendCommand(&ObexApp->handle, (ObexOpcode)Opcode,
                        (ObHeaderBlock)(OCC(txLength) > 0 ? HB_CLIENT_HEADERS : HB_IDLE));

#if XA_ERROR_CHECK == XA_ENABLED
Exit:
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    OS_UnlockObex();
    return status;
}

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBEX_ClientSendSessionRequest()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Issues an OBEX Session command over connected transports.
 *
 * Return:    OB_STATUS_PENDING
 *            OB_STATUS_FAILED
 *            OB_STATUS_BUSY
 *            OB_STATUS_PACKET_TOO_SMALL
 *            OB_STATUS_INVALID_PARM - Invalid parameters.
 *            OB_STATUS_NO_CONNECT - No transport connection exists.
 */
ObStatus OBEX_ClientSendSessionRequest(ObexClientApp *ObexApp, 
                                       void *Param,
                                       ObexSessionResumeParms *ResumeParms,
                                       U8 Opcode, U32 Timeout)
{
    U16                 hbLength;
    U8                  len, sessId[16], *buffer;
    ObexTpConnInfo      tpInfo;
    ObexClientSession  *session = 0;
    ObStatus            status = OB_STATUS_INVALID_PARM;

    OS_LockObex();

    if (Opcode != OB_SESSION_OPCODE_CLOSE) 
        session = (ObexClientSession *)Param;

#if XA_ERROR_CHECK == XA_ENABLED
    /* Verify the AppHandle is registered */
    if (!ObexApp || !OBSTACK_VerifyClientAppHandle(&ObexApp->handle) || !Param)
        goto Exit;

    switch (Opcode) {
    case OB_SESSION_OPCODE_RESUME:
        if (session->currentOperation && ResumeParms) {
            /* We are attempting to resume an OBEX operation. We must have a
             * valid Object Store Handle and if there were headers to send they
             * must also be provided here.
             */
            if (((session->unackedObjBytes > 0) && (ResumeParms->obsh == 0)) || 
                ((session->unackedHeaderLen != ResumeParms->headerLen)) ||
                ((session->unackedHeaderLen > 0) && (ResumeParms->headerBuff == 0))) {
                goto Exit;
            }

            if (session->unackedHeaderLen > OCH(length)) {
                status = OB_STATUS_FAILED;
                goto Exit;
            }

            /* The session better have a valid opcode in it. */
            ASSERT((((session->curOpcode & ~OB_FINAL_BIT) <= OB_OPCODE_SET_PATH) ||
                    ((session->curOpcode & ~OB_FINAL_BIT) == OB_OPCODE_ABORT)) &&
                   ((session->curOpcode & ~OB_FINAL_BIT) != 4));
                
        }
        /* Drop into next case */

    case OB_SESSION_OPCODE_CREATE:
        /* Create and Resume must not have an active session */
        if (OCC(activeSession)) {
            status = OB_STATUS_BUSY;
            goto Exit;
        }
        break;

    case OB_SESSION_OPCODE_SUSPEND:
    case OB_SESSION_OPCODE_SET_TIMEOUT:
        /* Suspend and Set Timeout require an active session */
        if (OCC(activeSession) == 0) {
            status = OB_STATUS_NO_SESSION;
            goto Exit;
        }

        /* The active session must have been passed in */
        if (OCC(activeSession) != session) {
            goto Exit;
        }
        break;

    case OB_SESSION_OPCODE_CLOSE:
        break;

    default:
        goto Exit;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(ObexApp && OBSTACK_VerifyClientAppHandle(&ObexApp->handle) && Param);

    /* Any headers already formed */
    hbLength = OCH(txLength);

#if XA_ERROR_CHECK == XA_ENABLED
    if (OCC(currentOperation) != NO_OPER_EVENTS) {
        status = OB_STATUS_BUSY;
        goto Exit;
    }

    /* Assume failure, reset header buffer */
    OCH(txLength) = 0;

    if (!OBSTACK_IsConnectionUp(OCH(parser.stack))) {
        status = OB_STATUS_NO_CONNECT;
        goto Exit;
    }
    
    if (hbLength > ObClientMaxHeaderBlockSize(Opcode)) {
        /* Cannot fit headers in a single OBEX packet so return an error. */
        status = OB_STATUS_PACKET_TOO_SMALL;
        goto Exit;
    } 
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(OCC(currentOperation) == NO_OPER_EVENTS);
    ASSERT(OBSTACK_IsConnectionUp(OCH(parser.stack)));
    ASSERT(hbLength <= ObClientMaxHeaderBlockSize(Opcode));

    OS_MemSet((U8 *)&OCH(sessionParms), 0, sizeof(ObexSessionParms));

    OCH(txLength) = 0;
    OCC(sessionFlags) = 0;

    if ((Opcode == OB_SESSION_OPCODE_CREATE) || 
        (Opcode == OB_SESSION_OPCODE_RESUME)) {
        OCC(activeSession) = session;
    } else {
        /* Assume we received parameters since they are optional
         * for opcodes other than Create and Resume.
         */
        OCC(sessionFlags) |= OB_SESSION_PARMS;
    }

    if ((Opcode == OB_SESSION_OPCODE_RESUME) && ResumeParms) {
        /* Restore the Object Store Handle and Application Headers */
        OCC(resumeObsh) = ResumeParms->obsh;
        OCC(resumeHeaders) = ResumeParms->headerBuff;
    }

    /* Build Session Params header (plus Session Opcode) */
    len = 0;
    buffer = OCC(protoTxBuff) + OCC(protoTxLen);

    /* Build the OBEX Session Parameters Header */
    buffer[len++] = OBEXH_SESSION_PARAMS;   /* Header identifier */
    len += 2;                               /* Skip over the length field */

    /* Write the first parameter: Session Opcode */
    buffer[len++] = SESSION_TAG_OPCODE;
    buffer[len++] = 1;
    buffer[len++] = Opcode;

    switch (Opcode) {
    case OB_SESSION_OPCODE_CREATE: /* Device Address, Nonce, timeout (optional) */
        /* We need information about our current transport */
        if (!OBEX_GetTpConnInfo(&ObexApp->handle, &tpInfo)) {
            status = OB_STATUS_FAILED;
            goto Exit;
        }

        OS_MemSet((U8 *)(session), 0, sizeof(ObexClientSession));
        session->timeout = session->ourTimeout = 0xFFFFFFFF;

        /* Set the Transport Type for this session */
        OCS(tpType) = tpInfo.tpType;
        /* Copy the Device Address to the Client Session Context */
        OS_MemCopy(OCS(clientDevAddr), tpInfo.devAddr, (U16)GetDevAddrLen(OCS(tpType)));
        /* Create the Client Session Nonce for this session. */
        ObCreateNonce(OCS(clientNonce));
        /* We use 16 byte Nonces */
        OCS(clientNonceLen) = 16; 
        /* NOTE: The Device Address and Nonce cannot change for this session, even 
         *       if the transport changes when the session is resumed.
         */
        
    case OB_SESSION_OPCODE_RESUME: /* Device Address, Nonce, Session Id, timeout (optional) */
        /* Device Address */
        buffer[len++] = SESSION_TAG_DEV_ADDR;
        buffer[len++] = GetDevAddrLen(OCS(tpType)); /* Length */
        /* Copy the Device Address based on the Created Session */
        OS_MemCopy(buffer+len, OCS(clientDevAddr), (U16)GetDevAddrLen(OCS(tpType)));
        len += GetDevAddrLen(OCS(tpType));  /* Add the Device Address Length */

        /* Nonce */
        buffer[len++] = SESSION_TAG_NONCE;
        buffer[len++] = OCS(clientNonceLen); /* Length */
        /* Copy the already created Client Nonce */
        OS_MemCopy(buffer+len, OCS(clientNonce), OCS(clientNonceLen));
        len += OCS(clientNonceLen); /* Add the Nonce */
        /* Fall into the next case to put the Session Id in */
        
    case OB_SESSION_OPCODE_CLOSE: /* Session Id */
        if ((Opcode == OB_SESSION_OPCODE_RESUME) || (Opcode == OB_SESSION_OPCODE_CLOSE)) {
            /* Session Id */
            buffer[len++] = SESSION_TAG_ID;
            buffer[len++] = 16; /* Length */
            if (OCC(activeSession))
                OBEX_CreateSessionId(OCC(activeSession), sessId);
            if (Opcode == OB_SESSION_OPCODE_CLOSE)
                OS_MemCopy(buffer+len, (U8*)Param, 16);
            else
                OS_MemCopy(buffer+len, sessId, 16);
            len += 16;  /* Add the Session ID */
        }
        if (Opcode == OB_SESSION_OPCODE_CLOSE) {
            if (OCC(activeSession) && OS_MemCmp((U8*)Param, 16, sessId, 16)) {
                /* Were closing the active session, do it now. */
                OCC(sessionFlags) |= OB_SESSION_DEACTIVATE;
            }
            break;
        }
        /* Fall into the next case to put the Timeout in */

    case OB_SESSION_OPCODE_SUSPEND:     /* Timeout (optional) */
    case OB_SESSION_OPCODE_SET_TIMEOUT: /* Timeout (optional) - Infinite assumed if none sent */
        /* Timeout */
        buffer[len++] = SESSION_TAG_TIMEOUT;
        buffer[len++] = 4; /* Length */
        if (Timeout != 0) {
            /* Copy the new timeout into the Client Session Context if
             * we included a nonzero timeout, otherwise use the old
             * timeout value.
             */
            OCS(ourTimeout) = Timeout;
            OCS(timeout) = min(OCS(ourTimeout), OCS(timeout));
        }
        StoreBE32(buffer+len, OCS(ourTimeout));
        len += 4; /* Add the timeout */
        break;

    default:
        ASSERT((Opcode >= OB_SESSION_OPCODE_CREATE) && 
               (Opcode <= OB_SESSION_OPCODE_SET_TIMEOUT));
        status = OB_STATUS_FAILED;
        goto Exit;
    }

    /* Now fill in the length of the Session Params header, since we know it now */
    StoreBE16(buffer+1, (U16)len);
    OCC(protoTxLen) = len;

    OCC(objLen) = 0;
    OCC(txLength) = hbLength; /* Prior existing application headers */
    OCC(headerOffset) = 0;
    OCC(txOffset) = 0;
    OCC(status) = OBRC_SUCCESS;
    OCC(currentOperation) = SESSION_OPER_EVENTS;

    ObParserSetInitState(&OCH(parser));
    
    /* Send the session command and headers */
    OCC(unackedPackets)++;
    OCC(opcode) = (OB_OPCODE_SESSION|OB_FINAL_BIT);
    OCC(sessionOpcode) = Opcode;

    status = ObClientSendCommand(&ObexApp->handle, (OB_OPCODE_SESSION|OB_FINAL_BIT), 
                                 HB_CLIENT_HEADERS);
    if (status == OB_STATUS_SUCCESS)
        status = OB_STATUS_PENDING;
Exit:
    OS_UnlockObex();
    return status;
}
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            OBEX_ClientPut()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Issues an OBEX PUT over connected transports.
 *
 * Return:    OB_STATUS_PENDING - Operation was successfully started. 
 *                Completion will be signalled via a call to the 
 *                ObClientAppCb callback.
 *            OB_STATUS_FAILED - Operation was not started because
 *                of an underlying transport problem.
 *            OB_STATUS_BUSY - Operation was not started because
 *                the client is currently executing another operation.
 *            OB_STATUS_INVALID_HANDLE - The object store handle was invalid.
 *            OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *                because the OBEX packet size was insufficient to transmit
 *                the provided headers.
 *            OB_STATUS_NO_CONNECT - Operation was not started because
 *                the client is not connected.
 *            OB_STATUS_INVALID_PARM - Invalid parameters.
 */
ObStatus OBEX_ClientPut(ObexClientApp *ObexApp, void *Obsh)
{
    ObStatus      status;
    ObHeaderBlock hb;
    U16           hbLength;
    ObexOpcode    opcode = OB_OPCODE_PUT;

    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    /* Verify the AppHandle is registered */
    if (!ObexApp || !OBSTACK_VerifyClientAppHandle(&ObexApp->handle)) {
        status = OB_STATUS_INVALID_PARM;
        goto Exit;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(ObexApp && OBSTACK_VerifyClientAppHandle(&ObexApp->handle));

    hbLength = OCH(txLength);

#if XA_ERROR_CHECK == XA_ENABLED
    if (OCC(currentOperation) != NO_OPER_EVENTS) {
        status = OB_STATUS_BUSY;
        goto Exit;
    }

    /* Assume failure, reset header buffer */
    OCH(txLength) = 0;

    if (!OBSTACK_IsConnectionUp(OCH(parser.stack))) {
        status = OB_STATUS_NO_CONNECT;
        goto Exit;
    }
    
    if (hbLength > ObClientMaxHeaderBlockSize(opcode)) {
        /* Cannot fit headers in a single OBEX packet so return an error. */
        kal_trace(BT_TRACE_G6_OBEX, OBEX_PUT_PACKET_TOO_SMALL, hbLength , ObClientMaxHeaderBlockSize(opcode));
        status = OB_STATUS_PACKET_TOO_SMALL;
        goto Exit;
    } 

#if OBEX_PUT_DELETE == XA_DISABLED
    if (Obsh == 0) {
        status = OB_STATUS_INVALID_HANDLE;
        goto Exit;
    }
#endif  /* OBEX_PUT_DELETE == XA_DISABLED */
#endif  /* XA_ERROR_CHECK == XA_ENABLED */
    
    ASSERT(OCC(currentOperation) == NO_OPER_EVENTS);
    ASSERT(OBSTACK_IsConnectionUp(OCH(parser.stack)));
    ASSERT(hbLength <= ObClientMaxHeaderBlockSize(opcode));

    OCH(txLength) = 0;

#if OBEX_PUT_DELETE == XA_DISABLED
    ASSERT(Obsh != 0);
#endif  /* OBEX_PUT_DELETE == XA_DISABLED */
    
    /* Error checking done, Initialize PUT request */
#if OBEX_PUT_DELETE == XA_ENABLED
    if (Obsh == 0) {
        /* Handle put-delete case */
        opcode |= OB_FINAL_BIT;
        OCC(objLen) = 0;
        GOEP_Report("[OBEX] OBEX_ClientPut OCC(objLen):%d = 0 ", OCC(objLen) );
    } else
#endif /* OBEX_PUT_DELETE == XA_ENABLED */
    {
        if(ObexApp->goep_buff_ptr == NULL)
        {
            /* Setup the Object for exchange */
            OCC(objLen) = OCH(obsf)->GetObjectLen(Obsh);
            GOEP_Report("[OBEX] OBEX_ClientPut OCC(objLen):%d", OCC(objLen) );
        }
		else if (ObexApp->client.objLen == 0)
		{
			/* Create-Empty PUSH */
			opcode |= OB_FINAL_BIT;
			OCC(objLen) = 0;
		}
    }

    /*
      OCH(txLength): Length of Buffered Headers
      OCC(objLen): Object's Length

      Setup basic OCC() data
    */
    OCC(txLength) = hbLength;
    OCC(obsh) = Obsh;
    OCC(txOffset) = 0;
    OCC(status) = OBRC_SUCCESS;
    OCC(currentOperation) = PUT_OPER_EVENTS;

    OCC(opcode) = opcode;

#if OBEX_SESSION_SUPPORT == XA_ENABLED
    /* Save the current client state, just in case the transport disconnects */
    ClientSaveFirst(ObexApp, OB_OPCODE_PUT, 0);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

    ObParserSetInitState(&OCH(parser));
    
    /* If their are headers, send them first. If their none, check if we
     * have an object to send. If so, prep it for transmit. Otherwise
     * we have no body or headers to send (Put-Delete case).
     */
    if (OCC(txLength) > 0) {
        /* Send Headers */
        hb = HB_CLIENT_HEADERS;
    } else {
        if (Obsh != 0) {
            /* Jump right to body */
            hb = HB_CLIENT_OBJECT;
            if(ObexApp->goep_buff_ptr == NULL)
            {
                /* obstyle bip */
                if( OCC(idFlag) == 0){
                OCC(txLength) = (U16) min( (OCC(objLen) + OBEXH_HI_BODY_SIZE),
                                      (U32)ObClientMaxHeaderBlockSize(OB_OPCODE_PUT));
                }else{
                OCC(txLength) = (U16) min( (OCC(objLen) + OBEXH_HI_BODY_SIZE + OBEXH_HI_CONNID_HV_SIZE),
                                      (U32)ObClientMaxHeaderBlockSize(OB_OPCODE_PUT));
                }
            }
            else
            {
                /* bufstyle opp, ftp, bpp */
                if( OCC(idFlag) == 0){
                OCC(txLength) = (U16) min( (OCC(objLen) + OBEXH_HI_BODY_SIZE),
                                      (U32)ObexApp->goep_buff_size );
                }else{
                OCC(txLength) = (U16) min( (OCC(objLen) + OBEXH_HI_BODY_SIZE + OBEXH_HI_BODY_SIZE),
                                      (U32)ObexApp->goep_buff_size + OBEXH_HI_BODY_SIZE);
                }
            }
            SRM_Report("[CONN] OBEX_ClientPut OCC(txLength):%d", OCC(txLength));
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
            if (OCC(objLen) == UNKNOWN_OBJECT_LENGTH)
                OCC(txLength) = 0;
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */
        } else {
            /* Obscure, Default object deletion behavior */
            hb = HB_IDLE;
        }
    }
  
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    if (OCC(activeSession)) {
        /* If OCS(ackedObjBytes) is non-zero then this must be a retransmit
         * without headers. Update the objLen to reflect the transmission of
         * the acked bytes, since we don't actually retransmit them.
         */
        ASSERT(OCS(ackedObjBytes) ? OCS(ackedObjBytes) == (U16)(OCC(txLength)-3) : 1);
        ASSERT(OCS(ackedObjBytes) ? OCC(seqNumError) == ERR_RETRANSMIT_NO_HEADERS : 1);

        OCC(objLen) -= OCS(ackedObjBytes);
    }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

    /* Send the command */
    OCC(unackedPackets)++;
    status = ObClientSendCommand(&ObexApp->handle, opcode, hb);
    if (status == OB_STATUS_SUCCESS)
        status = OB_STATUS_PENDING;

#if XA_ERROR_CHECK == XA_ENABLED
Exit:
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    OS_UnlockObex();
    return status;
}

extern BOOL ObParserIsSendingData(ObexParser *obp);

/*---------------------------------------------------------------------------
 *            OBEX_ClientAbort()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Aborts a PUT or GET operation.  Aborting a single 
 *            request/response operation (CONNECT, DISCONNECT, SETPATH, or
 *            SESSION) or while no operation is in progress is not allowed. 
 *            The client abort packet will be sent immediately, if possible. 
 *
 *            No OBCE_COMPLETE will be generated for this function.  Instead,
 *            the client event returned in response to this function will
 *            indicate the status of the operation in progress (either
 *            OBCE_COMPLETE (if our Abort occurred too late) or
 *            OBCE_ABORTED (the usual behavior))
 *
 * Return:    OB_STATUS_SUCCESS - Abort operation was successful.
 *            OB_STATUS_FAILED - Abort operation failed.
 *            OB_STATUS_INVALID_PARM - Invalid parameters.
 */
ObStatus OBEX_ClientAbort(ObexClientApp *ObexApp)
{
    ObStatus status = OB_STATUS_INVALID_PARM;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_ABORT);

    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObexApp || !OBSTACK_VerifyClientAppHandle(&ObexApp->handle)) {
        goto Exit;
    }

    /* Only allow PUT and GET operations to be aborted */
    if ((OCC(currentOperation) != PUT_OPER_EVENTS) &&
        (OCC(currentOperation) != GET_OPER_EVENTS)) {
        status = OB_STATUS_FAILED;
        goto Exit;
    }

    if (!OBSTACK_IsConnectionUp(OCH(parser.stack))) {
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_ABORT_NOCONNECT);
        status = OB_STATUS_NO_CONNECT;
        goto Exit;
    }

#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(ObexApp && OBSTACK_VerifyClientAppHandle(&ObexApp->handle));
    ASSERT(OBSTACK_IsConnectionUp(OCH(parser.stack)));
    /*
    ASSERT((OCC(currentOperation) == PUT_OPER_EVENTS) || 
           (OCC(currentOperation) == GET_OPER_EVENTS));
    */
    if ((OCC(currentOperation) != PUT_OPER_EVENTS) &&
        (OCC(currentOperation) != GET_OPER_EVENTS)) {
        status = OB_STATUS_FAILED;
        goto Exit;
    }

    status = OB_STATUS_PENDING;

#if OBEX_SESSION_SUPPORT == XA_ENABLED
    /* Save the current client state, just in case the transport disconnects */
    ClientSaveFirst(ObexApp, OB_OPCODE_ABORT, 0);

    if (OCC(activeSession)) {
        /* Force the abort packet to be sent in the normal 
         * request/response sequence while we have an 
         * active reliable session. 
         */
        OCC(status) = OBRC_SESSION_USER_ABORT;
        goto Exit;
    }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

    if (OCC(status) == OBRC_CLIENT_RW_ERROR || OCC(status) == OBRC_USER_ABORT) {
        /* Prevent another Abort from going out if we are already 
         * aborting due to an Object Store failure.
         */
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_ABORT_BUSY);
        status = OB_STATUS_BUSY;
        goto Exit;
    }

    OCC(status) = OBRC_USER_ABORT;

    /* We have been asked to abort.  If we are currently sending data, 
     * wait until we are finished and then send an Abort packet.  Otherwise,
     * Abort immediately.
     */
    if (ObParserIsSendingData(&(OCH(parser))))  {
        /* We're still sending data, ABORT when we're finished */
        ObParserSetAbort(&OCH(parser));
    }
    else {
        OCC(txLength) = OCH(txLength);
        OCH(txLength) = 0;
        OCC(txOffset) = 0;
        GOEP_Report("[OBEX] OCC(txLength) reset - ClientAbort");

        /* We're not currently sending data, ABORT IMMEDIATELY */
        ObClientSendCommand(&ObexApp->handle, (OB_OPCODE_ABORT|OB_FINAL_BIT), 
                            (ObHeaderBlock)(OCC(txLength) > 0 ? HB_CLIENT_HEADERS : HB_IDLE));
    }
    OCC(unackedPackets)++;
Exit:
    OS_UnlockObex();
    return status;
}

/*---------------------------------------------------------------------------
 *            OBEX_ClientSendCommand()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Continues the sending of a PUT or GET command. This function is
 *            public only if OBEX_PACKET_FLOW_CONTROL is enabled.
 *
 * Return:    void
 */
BtStatus OBEX_ClientSendCommand(ObexClientApp *ObexApp)
{
    ObHeaderBlock hb;
    BtStatus status = BT_STATUS_FAILED;

    OS_LockObex();

    ASSERT(ObexApp && OBSTACK_VerifyClientAppHandle(&ObexApp->handle));
    ASSERT(OCC(txLength) == 0);

	/* If we have already aborted the operation, don't process this call */
	if (OCC(status) == OBRC_USER_ABORT) {
        OS_UnlockObex();
        return BT_STATUS_FAILED;
	}

    /* Only allow PUT and GET operations to use this command */
    if (OCC(currentOperation) == PUT_OPER_EVENTS)
        OCC(opcode) = OB_OPCODE_PUT;
    else if (OCC(currentOperation) == GET_OPER_EVENTS)
        OCC(opcode) = OB_OPCODE_GET;
    else {
        OS_UnlockObex();
        return BT_STATUS_FAILED;
    }

    /* Need to keep track of current header block to avoid sending 
     * headers once we've moved onto body.
     */
    kal_trace(BT_TRACE_G6_OBEX, GOEP_CLIENT_SENDCOMMAND, OCH(txLength), OCC(txLength));
    if (OCH(txLength) > 0) {
        /* Send application headers */
        OCC(txLength) = OCH(txLength);
        OCH(txLength) = 0;
        hb = HB_CLIENT_HEADERS;
        GOEP_Report("[OBEX] OCC(txLength) reset - OBEX_ClientSendComm");
        if ((OCC(opcode) == OB_OPCODE_GET) && OCH(appHeaderIsEndBody)) {
            /* We sent an End Of Body header in the Get Request, set
             * the Final Bit
             */
            OCC(opcode) |= OB_FINAL_BIT;
        }
        
        if( OCC(idFlag) == 1 && (OCC(opcode) == OB_OPCODE_GET) && OCC(txLength) == OBEXH_HI_CONNID_HV_SIZE ){
        	OCC(opcode) |= OB_FINAL_BIT;
        }
        /* Make sure we clear this flag for all cases */
        OCH(appHeaderIsEndBody) = FALSE;
    } else {
        /* GET or PUT Object body. */
        /* OCC(txLength) the HI+HV of BODY of current packet */
        if (OCC(opcode) == OB_OPCODE_PUT) {
            if (OCC(objLen) == 0) {
                /* Send a packet with an empty end of body header. */
                OCC(txLength) = 3;
                OCC(opcode) |= OB_FINAL_BIT;
                ObexApp->goep_buff_wrote_len=0;
                
                /* TODO: Append CONNID */
                if( 0 !=  OCC(idFlag) ){
                    OCC(txLength) += OBEXH_HI_CONNID_HV_SIZE;
                    GOEP_Report("[OBEX] put empty. add OCC(txLength):%d", OCC(txLength));
                }
            } else {
                /* We have more body data to send. */
                if(ObexApp->goep_buff_ptr == NULL)
                {
                    /* obstyle bip */
                    if( 0 == OCC(idFlag) ){
                    OCC(txLength) = (U16)min((OCC(objLen) + OBEXH_HI_BODY_SIZE),
                                         (U32)ObClientMaxHeaderBlockSize(OB_OPCODE_PUT));
                    }else{
                    OCC(txLength) = (U16)min((OCC(objLen) + OBEXH_HI_BODY_SIZE + OBEXH_HI_CONNID_HV_SIZE),
                                         (U32)ObClientMaxHeaderBlockSize(OB_OPCODE_PUT));
                    } 
                    kal_trace( BT_TRACE_G6_OBEX, OBEX_CLIENT_CALC_OBJ_TXLENGTH_OBSTYLE, OCC(txLength), OCC(objLen), OCC(idFlag));
                    
                }
                else
                {
                    /* bufstyle opp, ftp, bpp */
                    OCC(txLength) = (U16)min((OCC(objLen) + OBEXH_HI_BODY_SIZE),
                                      (U32)ObexApp->goep_buff_size + OBEXH_HI_BODY_SIZE);
                    if(OCC(objLen) == ObexApp->goep_buff_size)
                    {
                        OCC(opcode) |= OB_FINAL_BIT;
                        ObexApp->goep_buff_wrote_len=0;
                    }
                    
                    if( OCC(idFlag) == 1 ){
                        OCC(txLength) += OBEXH_HI_CONNID_HV_SIZE;
                    }
                    kal_trace( BT_TRACE_G6_OBEX, OBEX_CLIENT_CALC_OBJ_TXLENGTH_BUFSTYLE, OCC(txLength), OCC(objLen), OCC(idFlag));
                }
                //GOEP_Report("[OBEX] OCC(txLength) reset - OBEX_ClientSendCommand");

                /* check if this packet is the last data packet */
                if ( OCC(idFlag) == 0 && ((OCC(objLen) + OBEXH_HI_BODY_SIZE) <= OCC(txLength)))
                {
                    GOEP_Report("[OBEX] final_bit in body&connid noidflag");
                    OCC(opcode) |= OB_FINAL_BIT;
                    ObexApp->goep_buff_wrote_len=0;
                }
                if( OCC(idFlag) == 1 ){
					if ((OCC(objLen) + OBEXH_HI_BODY_SIZE + OBEXH_HI_CONNID_HV_SIZE) <= OCC(txLength))
					{
						GOEP_Report("[OBEX] final_bit in body&connid");
						OCC(opcode) |= OB_FINAL_BIT;
						ObexApp->goep_buff_wrote_len=0;
					}
				}
                
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
                if (OCC(objLen) == UNKNOWN_OBJECT_LENGTH)
                    OCC(txLength) = 0;
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */
            }
            hb = HB_CLIENT_OBJECT;
#ifdef __BT_GOEP_V2__
            /* Send done. disable SRM mode*/
            if( (OCC(opcode)&OB_FINAL_BIT) != 0 && OCC(srm_client) == OBSH_SRM_ENABLE ){
                GOEP_Report("[OBEX][CLIENT] escape srm mode ");
                OCC(srm_client) = 0;
            }
#endif
        }
        else {

                /* Get more object from server. */
            OCC(opcode) |= OB_FINAL_BIT;
            hb = HB_IDLE;        

        }
    }

    OCC(txOffset) = 0;
    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENTSENDCOMMAND_TXLENGTH, OCC(txOffset), OCC(txLength), hb, OCC(objLen), OCC(opcode));

    OCC(unackedPackets)++;
    status = ObClientSendCommand(&ObexApp->handle, OCC(opcode), hb);
    OS_UnlockObex();
    
    return status;
}

/*---------------------------------------------------------------------------
 *            ObClientEventInd()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  OBEX Client callback from the OBEX parser.
 *
 * Return:    void
 */
void ObClientEventInd(ObexAppHandle *AppHndl, ObexParserEvent event)
{
    ObexClientApp *ObexApp = (ObexClientApp *)AppHndl;

    ASSERT(AppHndl);

    if (event != OBPE_PUT_HEADER && event != OBPE_WRITE_HEADER)
    {
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_EVENTIND, event);
    }

    /* Assign the OBEX client & opcode for the callback parms. */
    OBD(clientParms).client = ObexApp;
    OBD(clientParms).opcode = (ObClientGetOpcode() & ~OB_FINAL_BIT);
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    if (OBD(clientParms).opcode == OB_OPCODE_SESSION)
        OBD(clientParms).opcode = 0x10 + OCC(sessionOpcode);

    if (ObClientSessionLayerEventInd(ObexApp, event) == FALSE) {
        return;
    }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

    if (OCC(currentOperation) == NO_OPER_EVENTS) {
        /* No operation in progress, ignore event. */
        return;
    }

    switch (event) {
    case OBPE_PUT_HEADER:
        /* 1-byte and 4-byte headers are indicated to the application here. */
        if (OBEXH_Is4Byte(ObClientGetHeaderType()) ||
            OBEXH_Is1Byte(ObClientGetHeaderType())) {
            if (OBEXH_Is4Byte(ObClientGetHeaderType())) {
                OBD(clientParms).u.headerRx.value = ObClientGetHeaderInt32();
            }
            if (OBEXH_Is1Byte(ObClientGetHeaderType())) {
                OBD(clientParms).u.headerRx.value = ObClientGetHeaderByte();
            }
            OBD(clientParms).u.headerRx.type = ObClientGetHeaderType();
            OBD(clientParms).event = OBCE_HEADER_RX;    
            OCC(appCallback)(&OBD(clientParms));
            return;
        } 
        break;

    case OBPE_DISCONNECT:
        /* Clear out any unacknowledged packets */
        OCC(unackedPackets) = 0;
        
        OCH(txLength) = 0;

        /* Report the disconnect, no need to pass the event on. */
        OBD(clientParms).u.abortReason = OCC(status) = OBRC_LINK_DISCONNECT;
        ClientComplete(ObexApp, OBCE_ABORTED);
        return;

    case OBPE_OPER_IND:
        /* Disconnect if the final bit is not set in the response. */
        if ((ObClientGetResponse() & OB_FINAL_BIT) == 0) {
			kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_RECV_RESPONSE_WITH_NO_FINALBIT,ObClientGetResponse());
            ObClientDisconnect();
            return;
        }
        
#ifdef __BT_GOEP_V2__        
        if( OCC(unackedPackets) > 0 ){
        	OCC(unackedPackets)--;
        }
        kal_trace(BT_TRACE_G6_OBEX, OBEX_UNACK_NUMBER, OCC(unackedPackets));
        
        ASSERT(OCC(unackedPackets) <= 1); /* U8 type... */
#else
        OCC(unackedPackets)--;
        ASSERT(OCC(unackedPackets) <= 1);
#endif
        break;
    }
    

    /* Call client operation specific handler */
    ClientEventHandlerTable[OCC(currentOperation)](ObexApp, event);
}


#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            ObClientSessionLayerEventInd()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  OBEX Client callback from the OBEX parser.
 *
 * Return:    void
 */
static BOOL ObClientSessionLayerEventInd(ObexClientApp *ObexApp, ObexParserEvent Event)
{
    /* If this is a session command (and it's not a disconnect event) or
     * there is no active session then we have no processing to do here. 
     */
    if ((OCC(activeSession) == 0) || ((Event != OBPE_DISCONNECT) && 
        (OCC(currentOperation) == SESSION_OPER_EVENTS))) {
        return TRUE;
    }

    switch (Event) {

    case OBPE_OPER_IND_START:
        /* Reset sequence number error to indicate no sequence 
         * number header has been received.
         */
        OCC(seqNumError) = ERR_NO_SEQ_NUM_HEADER;
        break;

    case OBPE_OPER_IND:
        switch (OCC(seqNumError)) {
        case ERR_NONE:
            /* Normal Case: We've received a response from the server with
             * a sequence number that acknowledges our last command. Update
             * the client's saved state (in case the transport disconnects).
             */
            ClientSessionResponseReceived(ObexApp);
            return TRUE;

        case ERR_NO_SEQ_NUM_HEADER:
            /* We didn't get a Sequence Number Header in the last
             * response. This is out of spec. Close the session!
             */
            OCC(sessionFlags) |= OB_SESSION_ERROR;
            OBD(clientParms).u.abortReason = OCC(status) = OBRC_SESSION_NO_SEQ_NUM;
            ClientComplete(ObexApp, OBCE_ABORTED);
            break;

        case ERR_DISCONNECT:
            OCC(sessionFlags) |= OB_SESSION_ERROR;
            OBD(clientParms).u.abortReason = OCC(status) = OBRC_SESSION_ERROR;
            ClientComplete(ObexApp, OBCE_ABORTED);
            break;

        default:
            /* Resend the next appropriate packet. */
            ClientRestoreState(ObexApp);
            SendNextPacket(ObexApp);
            break;
        }
        return FALSE;
        
    case OBPE_PUT_HEADER:
        if (ObClientGetHeaderType() == OBEXH_SESSION_SEQ_NUM) {
            OCC(seqNumError) = VerifyClientSequenceNum(ObexApp, ObClientGetHeaderByte()); 
            /* Report(("OBEX: Client Sequence: Status=%d, Expect: SeqNum=%d, NextSeqNum=%d, Got: SeqNum=%d.\n", 
                   OCC(seqNumError), OCS(seqNum), OCS(nextSeqNum), ObClientGetHeaderByte())); */

            if (OCC(seqNumError) == ERR_NONE)
                ClientSessionCommandAcked(ObexApp);

            /* Consume the SessionSeqNum header. */
            return FALSE;
        }
        /* Drop into next case */

    case OBPE_WRITE_HEADER:
        if (OCC(seqNumError) == ERR_NONE) {
            /* Allow application to receive the headers. */
            return TRUE;
        }
        return FALSE;

    case OBPE_DISCONNECT:
        /* Clear out any unacknowledged packets */
        OCC(unackedPackets) = 0;

        OCH(txLength) = 0;

        if ((OCC(currentOperation) == SESSION_OPER_EVENTS) &&
            ((OCC(sessionOpcode) == OB_SESSION_OPCODE_CREATE) ||
             (OCC(sessionOpcode) == OB_SESSION_OPCODE_RESUME))) {
            /* Disconnect during a Create or Resume Session operation. 
             * Don't suspend the session! Just remove it.
             */
            OCC(activeSession) = 0;
            return TRUE;
        }

        if (OCC(currentOperation) != NO_OPER_EVENTS) {
            ASSERT((OCC(sessionFlags) & OB_SESSION_ERROR) == 0);
            /* Operation is in progress, indicate SUSPENDED to
             * suspend the active session and active operation.
             */
            OBD(clientParms).u.suspended.obsh = OCC(obsh);
            OBD(clientParms).u.suspended.headerLen = OCS(unackedHeaderLen);
            OBD(clientParms).u.suspended.session = OCC(activeSession);
            if (OCS(unackedHeaderLen) > 0)
                OBD(clientParms).u.suspended.headerBuff = OCH(buffer);
            else OBD(clientParms).u.suspended.headerBuff = 0;
            
            SuspendSession(ObexApp);
            ClientComplete(ObexApp, OBCE_SUSPENDED);
            return FALSE;
        }

        if ((OCC(sessionFlags) & OB_SESSION_ERROR) == 0) {
            /* Suspend the active session. */
            OBD(clientParms).event = OBCE_SESSION_SUSPENDED;
        } else {
            /* Indicate critical error on active session. */
            OBD(clientParms).event = OBCE_SESSION_ERROR;
        }
        OBD(clientParms).u.session = OCC(activeSession);

        /* Its okay that we call suspend on session error. */
        SuspendSession(ObexApp);
        
        OCC(appCallback)(&OBD(clientParms));

        /* We've handled this event, don't allow ObClientEventInd to continue. */
        return FALSE;
    }

    /* If we didn't process the event, assume its safe to proceed. */
    return TRUE;
}
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            ProcessSimpleOpEvents()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process events for Simple Operations. This includes any
 *            opcode that is restricted to one packet in each direction.
 *            Including the Telecom Query response, which funnels through here.
 *
 * Return:    void
 */
void ProcessSimpleOpEvents(ObexClientApp *ObexApp, ObexParserEvent event)
{
#ifdef __BT_GOEP_V2__
    kal_trace(BT_TRACE_G6_OBEX, OBEX_CLIENT_PROCESS_SIMPLEOP, ObexApp, event);
#endif
    if (event == OBPE_OPER_IND) {
        kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_PROCESS_SIMPLEEVENT, ObClientGetResponse());
        if (ObClientGetResponse() == (OBRC_SUCCESS|OB_FINAL_BIT)) {
            ClientComplete(ObexApp, OBCE_COMPLETE);
        } else {
            OS_Report("[OBEX:] response:0x%x", ObClientGetResponse());
            if( (OCC(status) == OBRC_USER_ABORT) || 0 != (ObClientGetResponse()&0x80) ){
                OCC(status) = (ObexAbortReason)(ObClientGetResponse() & 0x7F);
                OBD(clientParms).u.abortReason = OCC(status);
                ClientComplete(ObexApp, OBCE_ABORTED);
            }else{
                OS_Report("[OBEX:] ignore abort_rsp not final-bit ");
            }
        }
    } 
    else if ((event == OBPE_WRITE_HEADER) || (event == OBPE_PUT_HEADER)) {
        /* Byte Sequence and Unicode are indicated to the application here. */
        ASSERT(OBEXH_IsByteSeq(ObClientGetHeaderType()) || 
            OBEXH_IsUnicode(ObClientGetHeaderType()));
        OBD(clientParms).u.headerRx.type = ObClientGetHeaderType();
        OBD(clientParms).u.headerRx.totalLen = ObClientGetHeaderLen();
        OBD(clientParms).u.headerRx.remainLen = ObClientGetRemainHeaderLen();
        OBD(clientParms).u.headerRx.buff = ObClientGetHeaderBuff();
        OBD(clientParms).u.headerRx.currLen = ObClientGetHeaderWriteLen();
        OBD(clientParms).event = OBCE_HEADER_RX;
        OCC(appCallback)(&OBD(clientParms));
    }
}

/*---------------------------------------------------------------------------
 *            ProcessPutEvents()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process events for the put operation. It is called every time
 *            their is a TTP packet available for xmit.
 *
 * Return:    void
 */
void ProcessPutEvents(ObexClientApp *ObexApp, ObexParserEvent event)
{
    ObexRespCode  rsp;
#ifdef __BT_GOEP_V2__
    kal_trace(BT_TRACE_G6_OBEX, OBEX_CLIENT_PROCESS_PUTOP, ObexApp, event, OCC(status));
#endif

    if (event == OBPE_OPER_IND) {
        OBD(clientParms).opcode = OB_OPCODE_PUT; 

        /* An OBEX Packet [response] has been completely received. */ 
        rsp = (ObexAbortReason)(ObClientGetResponse() & 0x7f); 

#ifdef __BT_GOEP_V2__
        if (OCC(status) == OBRC_USER_ABORT && OCC(srm_client) != OBSH_SRM_ENABLE ) { 
#else
        if (OCC(status) == OBRC_USER_ABORT) { 
#endif
            if (OCC(unackedPackets) > 0) { 
                /* Save the PUT response for completion later. */ 
                OCC(storedResp) = rsp; 
                return; 
            } 
            if (OCC(storedResp)) {
                rsp = OCC(storedResp); 
                OCC(storedResp) = 0;
            }
        }

        if ((rsp == OBRC_CONTINUE) || (rsp == OBRC_SUCCESS)) {
            if (OCC(opcode) & OB_FINAL_BIT) {
                /* The operation is complete, report it to the application.
                 * Check if we had an internal error which caused 
                 * an abort, in which case we should return ABORTED.
                 */
                if ((OCC(status) != OBRC_SUCCESS) &&
                    (OCC(status) != OBRC_USER_ABORT)) {
                    /* We aborted because of internal error, notify the user. */
                    OBD(clientParms).u.abortReason = OCC(status);
                    ClientComplete(ObexApp, OBCE_ABORTED);
                } else {
                    /* The operation completed */
                    ClientComplete(ObexApp, OBCE_COMPLETE);
                }
            } else {
                if (OCC(status) != OBRC_SUCCESS) {
                    if (OCC(status) == OBRC_USER_ABORT) {
                        /* We received a Put Continue, but we are aborting the operation. */
                        OBD(clientParms).u.abortReason = OCC(status);
                        ClientComplete(ObexApp, OBCE_ABORTED);
                    }
#if OBEX_SESSION_SUPPORT == XA_ENABLED
                    else if ((OCC(status) == OBRC_CLIENT_RW_ERROR) || 
                             (OCC(status) == OBRC_SESSION_USER_ABORT)) {
                        if (OCC(status) == OBRC_SESSION_USER_ABORT) {
                            /* Allow the next response to indicate the event */
                            OCC(status) = OBRC_USER_ABORT;
                        }
                        /* We have been asked to abort so send an abort packet. */
                        ClientSaveFirst(ObexApp, OB_OPCODE_ABORT, 0);
#else /* OBEX_SESSION_SUPPORT == XA_ENABLED */
                    else if (OCC(status) == OBRC_CLIENT_RW_ERROR) {
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
                        OCC(unackedPackets)++;
                        if (OCC(status) == OBRC_CLIENT_RW_ERROR) {
                            /* Assign the final bit to the opcode to prevent an Abort packet
                             * from being resent below. We know the operation is aborting, so this
                             * should always cause the OBCE_ABORTED event, which is expected.
                             */
                            OCC(opcode) |= OB_FINAL_BIT;
                        }
#if 1
                         // merge w0946 MAUI
						/* when OBRC_CLIENT_RW_ERROR, ABORT has been sent out */
                        //ObClientSendCommand(&ObexApp->handle, (OB_OPCODE_ABORT|OB_FINAL_BIT), HB_IDLE);

                        /* We received a Put Continue, but we are aborting the operation. */
                        OBD(clientParms).u.abortReason = OCC(status);
                        ClientComplete(ObexApp, OBCE_ABORTED);
#else
                        ObClientSendCommand(&ObexApp->handle, (OB_OPCODE_ABORT|OB_FINAL_BIT), HB_IDLE);
#endif
                    }
                } else {
                    OCC(txOffset) = 0;

#ifdef __BT_GOEP_V2__
		/* Recalculate the SRM state after receive remote's packet done */
                    if( OCC(srm_client) == OBSH_SRM_DISABLE ){
						SRM_Report("[SRM][CLIENT] check client srm_client:%d srm_flag:0x%x", OCC(srm_client), OCC(srm_flags) );
						if( (OCC(srm_flags)&(OBSF_CLIENT_ENABLE|OBSF_SERVER_ENABLE)) == (OBSF_CLIENT_ENABLE|OBSF_SERVER_ENABLE)){
							SRM_Report("[SRM][CLIENT] enter client srm enabled mode");
							OCC(srm_client) = OBSH_SRM_ENABLE;

							if( (OCC(srm_flags)&(OBSF_CLIENT_WAITING|OBSF_SERVER_WAITING)) != 0 ){
								SRM_Report("[SRM][CLIENT] enter client srm wait mode");
								OCC(srm_client) = OBSH_SRM_WAITENABLE;
							}
						}
					}else if( OCC(srm_client) == OBSH_SRM_WAITENABLE ){
						SRM_Report("[SRM][CLIENT] srmwait check client srm_client:%d srm_flag:0x%x", OCC(srm_client), OCC(srm_flags));
						if( (OCC(srm_flags)&(OBSF_CLIENT_WAITING|OBSF_SERVER_WAITING)) == 0 ){
							SRM_Report("[SRM][CLIENT] srmwait enter srm mode");
							OCC(srm_client) = OBSH_SRM_ENABLE;
						}
						
                        /* clear server's wait flag after sending request */
		}					
#endif
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
                    /* Instruct the client app to send the PUT packet */
                    OBD(clientParms).event = OBCE_SEND_COMMAND;
                    OCC(appCallback)(&OBD(clientParms));
#else  /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
                    OBEX_ClientSendCommand(ObexApp);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
                }
            }
        } else {
            /* The other device [Server] has aborted the operation. */
            if (OCC(status) == OBRC_USER_ABORT) {
                /* No outstanding Abort exists, we can indicate the Abort now */
#ifdef __BT_GOEP_V2__	
                // should ignore the 0x90 continue back 
                
                if( 0 != (OCC(srm_client)&OBSH_SRM_ENABLE) ){
                    kal_trace(BT_TRACE_G6_OBEX, GOEP_CLIENT_SRM_FLAG, OCC(srm_client));
                }else{
#else 
                {
#endif
                OBD(clientParms).u.abortReason = OCC(status);
                ClientComplete(ObexApp, OBCE_ABORTED);
                }
            } else {
                OCC(status) = (ObexAbortReason)(ObClientGetResponse() & 0x7F);
                OBD(clientParms).u.abortReason = OCC(status);
                ClientComplete(ObexApp, OBCE_ABORTED);
            }
        }
    }
    else if ((event == OBPE_WRITE_HEADER) || (event == OBPE_PUT_HEADER)) {
        /* Byte Sequence and Unicode are indicated to the application here. */
        ASSERT(OBEXH_IsByteSeq(ObClientGetHeaderType()) || 
               OBEXH_IsUnicode(ObClientGetHeaderType()));
        OBD(clientParms).u.headerRx.type = ObClientGetHeaderType();
        OBD(clientParms).u.headerRx.totalLen = ObClientGetHeaderLen();
        OBD(clientParms).u.headerRx.remainLen = ObClientGetRemainHeaderLen();
        OBD(clientParms).u.headerRx.buff = ObClientGetHeaderBuff();
        OBD(clientParms).u.headerRx.currLen = ObClientGetHeaderWriteLen();
        OBD(clientParms).event = OBCE_HEADER_RX;
        OCC(appCallback)(&OBD(clientParms));
    }
#ifdef __BT_GOEP_V2__
    else if( event == OBPE_TX_COMPLETE ){
        /* Tx packet request done */
        if( OCC(srm_client) == OBSH_SRM_ENABLE ){
            OBD(clientParms).event = OBCE_SEND_COMMAND;
            OCC(appCallback)(&OBD(clientParms));
        }
        
		// clear server' wait flag
		if( (OCC(srm_flags)&(OBSF_SERVER_WAITING)) != 0 ){
			kal_trace(BT_TRACE_G6_OBEX, OBEX_TXCOMPLETE_EVENT);
			OCC(srm_flags) = OCC(srm_flags) & (~OBSF_SERVER_WAITING);

			/* if server response keeps sending wait flag, server_wait flag will be setted */
		} 
           
    }
#endif
}

/*---------------------------------------------------------------------------
 *            ProcessGetEvents()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process events for the get operation.
 *
 * Return:    void
 */
void ProcessGetEvents(ObexClientApp *ObexApp, ObexParserEvent event)
{
    ObexRespCode    rsp;
    U16             len;
    U8             *buff;

    ASSERT(OBSTACK_VerifyClientAppHandle(&ObexApp->handle));
#ifdef __BT_GOEP_V2__
    kal_trace(BT_TRACE_G6_OBEX, OBEX_CLIENT_PROCESS_GETOP, ObexApp, event, OCC(status));
#endif

    switch (event) {
    case OBPE_OPER_IND:
        if ((OCC(status) == OBRC_USER_ABORT) && (OCC(unackedPackets) > 0)){
            GOEP_Report("[SRM] OBPE_OPER_IND USER_ABORT unack:%d", OCC(unackedPackets));
            return;
        }

        OBD(clientParms).opcode = OB_OPCODE_GET;
        rsp = (ObexAbortReason)(ObClientGetResponse() & 0x7f);
        if (rsp == OBRC_SUCCESS) {
            /* Operation is complete, Check if we were aborting */
            if (OCC(status) == OBRC_SUCCESS) {
                /* The GET operation was successful */
                ClientComplete(ObexApp, OBCE_COMPLETE);
            } else {
                /* Internal error or User request caused abort. 
                 * The reason is already set. */
                OBD(clientParms).u.abortReason = OCC(status);
                /* No outstanding Abort exists, we can indicate the Abort now */
                ClientComplete(ObexApp, OBCE_ABORTED);
            }
        } else if (rsp == OBRC_CONTINUE) {
            /* Operation is not complete. Request more data (if were not aborting). */
            if (OCC(status) == OBRC_SUCCESS) {
                OCC(txOffset) = 0;

	     /* Recalculate the SRM state after receive remote's packet done */
#ifdef __BT_GOEP_V2__			
            if( OCC(srm_client) == OBSH_SRM_DISABLE ){
				SRM_Report("[SRM][CLIENT] check client srm_client:%d srm_flag:0x%x", OCC(srm_client), OCC(srm_flags) );
				if( (OCC(srm_flags)&(OBSF_CLIENT_ENABLE|OBSF_SERVER_ENABLE)) == (OBSF_CLIENT_ENABLE|OBSF_SERVER_ENABLE)){
					SRM_Report("[SRM][CLIENT] enter client srm enabled mode");
					OCC(srm_client) = OBSH_SRM_ENABLE;

					if( (OCC(srm_flags)&(OBSF_CLIENT_WAITING|OBSF_SERVER_WAITING)) != 0 ){
						SRM_Report("[SRM][CLIENT] enter client srm wait mode");
						OCC(srm_client) = OBSH_SRM_WAITENABLE;
					}
				}
			}else if( OCC(srm_client) == OBSH_SRM_WAITENABLE ){
				SRM_Report("[SRM][CLIENT] srmwait check client srm_client:%d srm_flag:0x%x", OCC(srm_client), OCC(srm_flags));
				if( (OCC(srm_flags)&(OBSF_CLIENT_WAITING|OBSF_SERVER_WAITING)) == 0 ){
					SRM_Report("[SRM][CLIENT] srmwait enter srm wait mode");
					OCC(srm_client) = OBSH_SRM_ENABLE;
				}
				
                /* clear server's wait flag after sending request */
			}
#endif

#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
                /* Instruct the client app to send the GET packet. */
                OBD(clientParms).event = OBCE_SEND_COMMAND;
                OCC(appCallback)(&OBD(clientParms));
#else /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
                OBEX_ClientSendCommand(ObexApp);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            } else {
                /* We received a Get Continue, but we are aborting the operation. */
                if (OCC(status) == OBRC_USER_ABORT) {
                    /* No outstanding Abort exists, we can indicate the Abort now */
#ifdef __BT_GOEP_V2__	
                    // should ignore the 0x90 continue back 
				    
					if( 0 != (OCC(srm_client)&OBSH_SRM_ENABLE) ){
    					kal_trace(BT_TRACE_G6_OBEX, GOEP_CLIENT_SRM_FLAG, OCC(srm_client));    
					}else{
#else 
                    {
#endif
                    OBD(clientParms).u.abortReason = OCC(status);
                    ClientComplete(ObexApp, OBCE_ABORTED);
                    }
                }
#if OBEX_SESSION_SUPPORT == XA_ENABLED
                else if ((OCC(status) == OBRC_CLIENT_RW_ERROR) || 
                        (OCC(status) == OBRC_SESSION_USER_ABORT)) {
                    if (OCC(status) == OBRC_SESSION_USER_ABORT) {
                        /* Allow the next response to indicate the event */
                        OCC(status) = OBRC_USER_ABORT;
                    }
                    /* We have been asked to abort so send an abort packet. */
                    ClientSaveFirst(ObexApp, OB_OPCODE_ABORT, 0);
#else /* OBEX_SESSION_SUPPORT == XA_ENABLED */
                else if (OCC(status) == OBRC_CLIENT_RW_ERROR) {
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
                    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_SEND_ABORT_FOR_RW_ERROR, &ObexApp->handle);

                    OCC(unackedPackets)++;
                    ObClientSendCommand(&ObexApp->handle, (OB_OPCODE_ABORT|OB_FINAL_BIT), HB_IDLE);
                }
            }
            
        } else {
            /* The other device [Server] has aborted the operation. */
            if (OCC(status) == OBRC_USER_ABORT) {
                /* No outstanding Abort exists, we can indicate the Abort now */
                OBD(clientParms).u.abortReason = OCC(status);
                ClientComplete(ObexApp, OBCE_ABORTED);
            } else {
                OCC(status) = (ObexAbortReason)(ObClientGetResponse() & 0x7F);
                OBD(clientParms).u.abortReason = OCC(status);
                ClientComplete(ObexApp, OBCE_ABORTED);
            }
        }
        break;

    case OBPE_WRITE_HEADER:
    case OBPE_PUT_HEADER:
        /* We handle the body headers, everything else goes to the app. */
        if ((ObClientGetHeaderType() == OBEXH_BODY) ||
            (ObClientGetHeaderType() == OBEXH_END_BODY)) {

            len = ObClientGetHeaderWriteLen();
            buff = ObClientGetHeaderBuff();

            /* If the amount to read is non-zero and we are not in
             * the process of aborting the operation then read the data.
             */
            if ((len > 0) && OCC(status) == OBRC_SUCCESS) {

                /* Write the body. If we fail then abort, but only if we aren't already aborting */
                if(ObexApp->goep_buff_ptr == NULL)
                {
                    if ((OCH(obsf)->Write(OCC(obsh), buff, len) != OBRC_SUCCESS) &&
                        (OCC(status) != OBRC_USER_ABORT)) 
                        {
                            OCC(status) = OBRC_CLIENT_RW_ERROR;
                        }
#if OBEX_SESSION_SUPPORT == XA_ENABLED
                    else if (OCC(activeSession)) {
                        /* Unacknowledged bytes written to the object store. 
                         * These bytes aren't considered 'acked' until we've
                         * received the entire OBEX packet.
                         */
                        OCS(unackedObjBytes) += len;
                }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
                }
                else
                {
                OS_MemCopy(ObexApp->goep_buff_ptr + ObexApp->goep_buff_wrote_len, buff, len);
                ObexApp->goep_buff_wrote_len += len;
                }
            }
        } else {
            /* Byte Sequence and Unicode are indicated to the application here. */
            ASSERT(OBEXH_IsByteSeq(ObClientGetHeaderType()) || 
                   OBEXH_IsUnicode(ObClientGetHeaderType()));
            OBD(clientParms).u.headerRx.type = ObClientGetHeaderType();
            OBD(clientParms).u.headerRx.totalLen = ObClientGetHeaderLen();
            OBD(clientParms).u.headerRx.remainLen = ObClientGetRemainHeaderLen();
            OBD(clientParms).u.headerRx.buff = ObClientGetHeaderBuff();
            OBD(clientParms).u.headerRx.currLen = ObClientGetHeaderWriteLen();
            OBD(clientParms).event = OBCE_HEADER_RX;
            OCC(appCallback)(&OBD(clientParms));
        }
        break;
#ifdef __BT_GOEP_V2__
    case OBPE_TX_COMPLETE:
        /* Tx packet request done */
        // TODO: if get request not will final bit and be inside SRM mdoe
        // continue to send headers until final bit is set (all headers out)

		// clear server' wait flag
		if( (OCC(srm_flags)&(OBSF_SERVER_WAITING)) != 0 ){
			kal_trace(BT_TRACE_G6_OBEX, OBEX_TXCOMPLETE_EVENT);
			OCC(srm_flags) = OCC(srm_flags) & (~OBSF_SERVER_WAITING);
		} 
        break;
#endif
    }
}

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            ProcessSessionEvents()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process events for the session operation.
 *
 * Return:    void
 */
void ProcessSessionEvents(ObexClientApp *ObexApp, ObexParserEvent event)
{
    U8      sessId[16];
    BOOL    ok;
    U8      currentOperation = 0;

    if (event == OBPE_OPER_IND) {

        /* If we didn't get a session parameters header, abort the operation. */
        if ((OCC(sessionFlags) & OB_SESSION_PARMS) == 0)
            OCC(status) = OBRC_SESSION_INVALID_PARMS;

        if (ObClientGetResponse() == (OBRC_SUCCESS|OB_FINAL_BIT)) {
            /* The operation is complete, report it to the application.
             * Check if we had an internal error which caused 
             * an abort, in which case we should return OBCE_ABORTED.
             */
            if ((OCC(status) != OBRC_SUCCESS) && (OCC(status) != OBRC_USER_ABORT)) {
                /* We aborted because of an internal error, notify the user. */
                goto HandleAbort;
            }

            /* If the active is being suspended clear it. */
            if (OCC(sessionOpcode) == OB_SESSION_OPCODE_SUSPEND) {
                /* Clear the active session. */
                SuspendSession(ObexApp);
            }

            if (OCC(sessionOpcode) == OB_SESSION_OPCODE_RESUME) {
                /* Copy in the app's resume headers before we complete the resume. */
                if (OCS(unackedHeaderLen) > 0)
                    OS_MemCopy(OCH(buffer), OCC(resumeHeaders), OCS(unackedHeaderLen));

                /* Save the event handler index to protect us from having
                 * to look in OCS() after the OBCE_COMPLETE in case the
                 * application closes the session during this callback.
                 */
                currentOperation = OCS(currentOperation);

                /* Restore the OBEX Packet Size. */
                if ((OCH(parser).maxTxPacket = OCS(obPacketSize)) > 0)
                    OCH(parser).flags |= OBPF_CONNECTED;
                else OCH(parser).flags &= ~OBPF_CONNECTED;
            }

            if ((OCC(sessionOpcode) == OB_SESSION_OPCODE_CLOSE) && 
                (OCC(sessionFlags) & OB_SESSION_DEACTIVATE)) {
                /* Clear the active session if it was closed successfully */
                OCC(activeSession) = 0;
            }
            ASSERT((OCC(sessionFlags) & OB_SESSION_ERROR) == 0);

            ClientComplete(ObexApp, OBCE_COMPLETE);

            if ((OCC(sessionOpcode) == OB_SESSION_OPCODE_RESUME) && currentOperation) {
                ASSERT(OCC(activeSession));
                /* There was an operation in progress when we suspended.
                 * We resume the operation by first indicating the resumption
                 * to the applicaiton, then restoring the object and operation
                 * state to where it was before the suspension. 
                 */
                OBD(clientParms).opcode = OCS(curOpcode);
                OBD(clientParms).event = OBCE_RESUME_OPER;
                OCC(appCallback)(&OBD(clientParms));

                ClientRestoreState(ObexApp);

                /* Send the next packet in the current operation. */
                SendNextPacket(ObexApp);
            }
        } else {
            /* The operation is complete, but the server returned an error. 
             * Clean up the session operation and indicate the failure to 
             * the user.
             */
            OCC(status) = (ObexAbortReason)(ObClientGetResponse() & 0x7F);
HandleAbort:

            if ((OCC(sessionOpcode) == OB_SESSION_OPCODE_CREATE) || 
                (OCC(sessionOpcode) == OB_SESSION_OPCODE_RESUME)) {
                /* No active session because of an error. */
                OCC(activeSession) = 0;
            }

            OBD(clientParms).u.abortReason = OCC(status);
            ClientComplete(ObexApp, OBCE_ABORTED);
        }
        return;
    } 
    
    if ((event == OBPE_WRITE_HEADER) || (event == OBPE_PUT_HEADER)) {
        if (ObClientGetHeaderType() == OBEXH_SESSION_PARAMS) {

            ok = ObexParseSessionParms( &ObexApp->handle, &OCH(sessionParms) );

            if (ok && (ObClientGetRemainHeaderLen() == 0)) {
                /* We've received the entire header, if all the required fields
                 * are present, process them and indicate it to the applicaiton.
                 */
                if ((OCH(sessionParms).fields & RX_SESSION_PARMS_IGNORE_MASK) !=
                    ObClntRxSessionParmsTable[OCC(sessionOpcode)]) {
                    /* Missing or unexpected session header parameter, fail the operation. */
                    goto ParmsError;
                }

                /* Indicate the servers parameters to the application. */
                OBD(clientParms).u.sessionParms = &OCH(sessionParms);
                OBD(clientParms).event = OBCE_SESSION_PARMS_RX;
                OCC(appCallback)(&OBD(clientParms));

                /* Update the timeout if the server specified one. */
                if (OCH(sessionParms).fields & OBSP_FIELD_TIMEOUT)
                    OCS(timeout) = min(OCH(sessionParms).timeout, OCS(ourTimeout));

                if (OCC(sessionOpcode) == OB_SESSION_OPCODE_CREATE) {
                    /* Save the servers Device Address & Nonce. */
                    OS_MemCopy(OCS(serverDevAddr), OCH(sessionParms).device,
                               OCH(sessionParms).deviceLen);

                    OS_MemCopy(OCS(serverNonce), OCH(sessionParms).nonce, 
                               OCH(sessionParms).nonceLen);
                    OCS(serverNonceLen) = OCH(sessionParms).nonceLen;
                } 
                else if (OCC(sessionOpcode) == OB_SESSION_OPCODE_RESUME) {
                    /* Existing Server Device Address, Nonce and
                     * Session Id's should match received ones. 
                     */
                    ok = ok && OS_MemCmp(OCS(serverDevAddr), (U16)GetDevAddrLen(OCS(tpType)),
                                         OCH(sessionParms).device, OCH(sessionParms).deviceLen);
                    
                    ok = ok && OS_MemCmp(OCS(serverNonce), OCS(serverNonceLen),
                                         OCH(sessionParms).nonce, OCH(sessionParms).nonceLen);
                    
                    ok = ok && ProcessResumeSequenceNum(ObexApp, OCH(sessionParms).sequenceNum);
                } 
                else return;

                /* Create the Client Session Id */
                OBEX_CreateSessionId(OCC(activeSession), sessId);
                /* Existing Server Session ID should match received one */
                ok = ok && OS_MemCmp(sessId, 16, OCH(sessionParms).sessionId, 16);
            }            

            if (ok == FALSE) {
ParmsError:
                kal_trace(BT_TRACE_G6_OBEX,OBEX_INVALID_SESSION_PARAMETERS_HEADER);
                OCC(status) = OBRC_SESSION_INVALID_PARMS;
                OCC(sessionFlags) |= OB_SESSION_ERROR;
            } 
            else OCC(sessionFlags) |= OB_SESSION_PARMS;
            return;
        }

        /* Byte Sequence and Unicode are indicated to the application here. */
        ASSERT(OBEXH_IsByteSeq(ObClientGetHeaderType()) || 
               OBEXH_IsUnicode(ObClientGetHeaderType()));

        OBD(clientParms).u.headerRx.type = ObClientGetHeaderType();
        OBD(clientParms).u.headerRx.totalLen = ObClientGetHeaderLen();
        OBD(clientParms).u.headerRx.remainLen = ObClientGetRemainHeaderLen();
        OBD(clientParms).u.headerRx.buff = ObClientGetHeaderBuff();
        OBD(clientParms).u.headerRx.currLen = ObClientGetHeaderWriteLen();
        OBD(clientParms).event = OBCE_HEADER_RX;
        OCC(appCallback)(&OBD(clientParms));
    }
}
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            ObClientReadHeaderBlock()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Read the client header block.
 *
 * Return:    void
 */
ObexRespCode ObClientReadHeaderBlock(ObexClientApp *ObexApp, ObHeaderBlock hb, U8* buff, U16 len)
{
    ObexRespCode rcode = OBRC_SUCCESS;
	
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    U16     len2;

    if (OCC(activeSession) && 
       (OCC(seqNumError) == ERR_RETRANSMIT_NO_HEADERS)) {
        /* We have no data to send. */
        ASSERT(len == 0);

        OCC(txLength) = 0;
        return rcode;
    }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

    kal_trace(BT_TRACE_G6_OBEX, OBEX_CLIENT_READ_HB, ObexApp, hb, len);

    ASSERT(OBSTACK_VerifyClientAppHandle(&ObexApp->handle));
    ASSERT((hb == HB_CLIENT_HEADERS) || (hb == HB_CLIENT_OBJECT));
            
    if (hb == HB_CLIENT_HEADERS) {      
#if OBEX_SESSION_SUPPORT == XA_ENABLED        
        ASSERT(OCC(txLength) > 0 || OCC(protoTxLen) > 0);

        if (OCC(protoTxLen) > 0) {
            /* Copy the OBEX session headers into the request packet */
            len2 = min(len, OCC(protoTxLen));
            OS_MemCopy(buff, OCC(protoTxBuff) + OCC(txOffset), len2);

            OCC(txOffset) += len2;
            OCC(protoTxLen) -= len2;

            if (OCC(protoTxLen) > 0) {
                return rcode;
            }

            len -= len2;
            buff += len2;

            OCC(txOffset) = 0;
        }
#else /* OBEX_SESSION_SUPPORT == XA_ENABLED */
        ASSERT(OCC(txLength) > 0);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

#ifdef __BT_GOEP_V2__
        kal_trace(BT_TRACE_G6_OBEX, OBEX_CLIENT_READ_HB_IN_HEADER,OCC(txLength), OCC(txOffset), len  );
#endif        
        if (OCC(txLength) > 0) {
            /* Copy the applications headers into the request packet */
            len = min(len, OCC(txLength));
            OS_MemCopy(buff, OCH(buffer) + OCC(txOffset), len);

            OCC(txOffset) += len;
            OCC(txLength) -= len;

#if OBEX_SESSION_SUPPORT == XA_ENABLED
            if (OCC(activeSession)) {
                /* Unacknowledged application header bytes. */
                OCS(unackedHeaderLen) += len;
            }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
        }

        //Report(("[OBEX]ObClientReadHeaderBlock(): return with OCC(txLength)=%d\n", OCC(txLength)));

        return rcode;
    }
    
    /* hb is HB_CLIENT_OBJECT ! */

    /* No handle GET operation due to put all data inside one RFbuffer? */
    /* Read data from the disk into the buffer (PUT operation) */
#ifdef __BT_GOEP_V2__
    //SRM_Report("[SRM] ObClientReadHeaderBlock object OCC(txLength):%d OCC(txOffset):%d remainlen:%d",OCC(txLength), OCC(txOffset), len );
    //SRM_Report("[CONN] ObClientReadHeaderBlock OCC(txOffset):%d, OCC(txLength):%d", OCC(txOffset), OCC(txLength));
#endif
    /* Construct a BODY header's HI */
    if (OCC(txOffset) == 0) {
        /* Append CONN_ID */
		if( OCC(idFlag) == 1 && OCC(obConnId) != OBEX_INVALID_CONNID && OCC(txLength) >= 5 ){
		    if( ObClientGetOpcode() == OB_OPCODE_PUT || ObClientGetOpcode() == (OB_OPCODE_PUT|OB_FINAL_BIT) ){
    			buff[0] = OBEXH_CONNID; 
    			StoreBE32( &buff[1], OCC(obConnId));
                OCC(txLength) -= 5;
    			
    			buff += 5;
    			len -= 5;
    			kal_trace(BT_TRACE_G6_OBEX, OBEX_CLIENT_APPEND_CONNID, OCC(txLength));
    		}
		}

        /* This is the beginning of a new OBEX packet. 
         * Append the data with the OBEX Body Header.
         */
        if (ObClientGetOpcode() & OB_FINAL_BIT) {
            buff[0] = OBEXH_END_BODY;
        } else {
            buff[0] = OBEXH_BODY;
        }
        buff[1] = (U8) (OCC(txLength) >> 8); /* day:OCC(txLength) is the object header's HV-length */
        buff[2] = (U8) OCC(txLength);

        OCC(txLength) -= 3; /* Once put data into buffer. Reduce the OCC's txLength & txOffset */
        buff += 3;
        OCC(txOffset) += 3;
        len -= 3;
    }
          
#ifdef __BT_GOEP_V2__
    SRM_Report("[CONN] ObClientReadHeaderBlock OCC(txLength):%d remain-len:%d", OCC(txLength), len);
#endif    
    /* If the amount to read is non-zero and we are not in
     * the process of aborting the operation then read the data.
     */
    if ((len > 0) && OCC(status) == OBRC_SUCCESS) {
            
        ASSERT(OCC(txLength) > 0);
        len = min(len, OCC(txLength));

        /* Read the body. If we fail then abort, but only if we aren't already aborting */
        if(ObexApp->goep_buff_ptr == NULL)
        {
            if (((rcode = OCH(obsf)->Read(OCC(obsh), buff, len)) != OBRC_SUCCESS) &&
                (OCC(status) != OBRC_USER_ABORT)) {

                kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_READ_HEADERBLOCK_READOBJ_FAILED, rcode);
                
                OCC(status) = OBRC_CLIENT_RW_ERROR;
            }
#if OBEX_SESSION_SUPPORT == XA_ENABLED
            else if (OCC(activeSession)) {
                /* Unacknowledged bytes sent from the object store. */
                OCS(unackedObjBytes) += len;
            }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
        }
        else
        {   GOEP_Report("[OBEX] buffcopy buff:%x goep_buff_ptr:%x wrote_len:%d len:%d", buff, ObexApp->goep_buff_ptr, ObexApp->goep_buff_wrote_len, len);
            OS_MemCopy(buff, ObexApp->goep_buff_ptr + ObexApp->goep_buff_wrote_len, len);
            ObexApp->goep_buff_wrote_len += len;
        }
    }

    /* No matter what act as if the data was consumed */
    OCC(objLen) -= len;  /* Use for next continous PUT packet and ReadHeaderBlock of HANDLE_PACKET*/
    OCC(txLength) -= len; /* Use for next ReadHeaderBlock of HANDLE_PACKET */

    return rcode;
}

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            ObClientReadHeaderBlockFlex()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Read the client header block.
 *
 * Return:    U16 - number of bytes read into buff (includes header)
 */
U16 ObClientReadHeaderBlockFlex(ObexClientApp *ObexApp, U8 *buff, U16 len)
{
    BOOL    more = TRUE;

#if OBEX_SESSION_SUPPORT == XA_ENABLED
    if (OCC(activeSession) && 
       (OCC(seqNumError) == ERR_RETRANSMIT_NO_HEADERS)) {
        /* We have no data to send. */
        OCC(txLength) = 0;
        return 0;
    }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

    ASSERT(OBSTACK_VerifyClientAppHandle(&ObexApp->handle));

    /* Save space for the body header identifier */
    len -= 3;

    /* The object is of unknown length. We can never span the body across
     * multiple packets so we know that this packet has the body header. 
     */
    if (OCH(obsf)->ReadFlex(OCC(obsh), buff+3, &len, &more) != OBRC_SUCCESS) {
        OCC(status) = OBRC_CLIENT_RW_ERROR;
    }
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    else if (OCC(activeSession)) {
        /* Unacknowledged bytes sent from the object store. */
        OCS(unackedObjBytes) += len;
    }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
    len += 3;

    if (!more) {
        /* Were done! */
        OCC(objLen) = 0;
        GOEP_Report("[OBEX] ObClientReadHeaderBlockFlex OCC(objLen):%d", OCC(objLen) );
    }

    /* Write the BODY header identifier */
    buff[0] = OBEXH_BODY;
    buff[1] = (U8)(len >> 8);
    buff[2] = (U8)(len);

    return len;
}
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            ClientComplete()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Clean up a finished client operation and notify application.
 *
 * Return:    void
 */
static void ClientComplete(ObexClientApp *ObexApp, ObClientEvent event)
{
    U8              eventHandler;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_COMPLETE, event);

    ASSERT(OBSTACK_VerifyClientAppHandle(&ObexApp->handle));
    ASSERT(OCH(txLength) == 0);

    eventHandler = OCC(currentOperation);

    OCC(obsh) = 0;
    OCC(opcode) = 0;
    OCC(txLength) = 0;
    OCC(currentOperation) = NO_OPER_EVENTS;

    /* While were idle, keep the opcode set to PUT this way the header
     * functions will allow the maximum amount of headers to be written.
     */
    OCH(parser).opcode = OB_OPCODE_PUT;

#if OBEX_SESSION_SUPPORT == XA_ENABLED
    OCC(protoTxLen) = 0;
    if (eventHandler != SESSION_OPER_EVENTS) {
        /* If we aren't completing a session operation, clear any sequence number 
         * error that might have occurred in the last operation. This error is not 
         * cleared for session operations, since they don't send sequence numbers 
         * and since a resume operation may set a seqNumError (e.g. retransmit) based 
         * on the sequence number returned in the resume response.
         */
        OCC(seqNumError) = ERR_NONE;
    }

    if (OCC(activeSession)) {
        if (eventHandler == SESSION_OPER_EVENTS) {
            /* If we are resuming an operation, set the handler index to 
             * reflect the operation were about to restart. This is done
             * here to block the client from starting another operation.
             */
            if ((OCC(sessionOpcode) == OB_SESSION_OPCODE_RESUME) &&
                (event == OBCE_COMPLETE))
                OCC(currentOperation) = OCS(currentOperation);
        } 
        else if (OCC(status) != OBRC_LINK_DISCONNECT) {
            ASSERT(event != OBCE_SUSPENDED);
            /* Clear the saved state, since the operation is either 
             * complete or aborted. 
             */
            ClientClearSavedState(ObexApp);

            if (OCC(sessionFlags) & OB_SESSION_ERROR) {
                /* If a critical error occurred, disconnect. */
                ObClientDisconnect();
            }
        }
    }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

    /* Signal the application callback */
    OBD(clientParms).event = event;
    OCC(appCallback)(&OBD(clientParms));
}

    
#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            ClientSaveFirst()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Save the current client state at the beginning of an operation,
 *            so that we can start the operation over, if needed.
 *
 * Return:    void
 */
static void ClientSaveFirst(ObexClientApp *ObexApp, ObexOpcode opcode, U32 param2)
{
    ASSERT(OBSTACK_VerifyClientAppHandle(&ObexApp->handle));

    if (OCC(activeSession)) {
        /* These values are used to restart the first packet in an operation. */
        OCS(startSeqNum) = OCS(seqNum);
        /* Store the current opcode */
        OCS(curOpcode) = opcode;
        /* param1 is the object store handle, we don't store that. */
        OCS(param2) = param2;
        /* Need to have the eventHandler set in-case we need to resume. */
        OCS(currentOperation) = OCC(currentOperation);
    }
}


/*---------------------------------------------------------------------------
 *            ClientSessionCommandAcked()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function is called as soon as we process the sequence
 *            number in the server's response, if the sequence number is in
 *            sequence.
 */
static void ClientSessionCommandAcked(ObexClientApp *ObexApp)
{
    /* We received confirmation that data we sent was received by the server. */
    if (OCC(currentOperation) == PUT_OPER_EVENTS) {
        OCS(ackedObjBytes) += OCS(unackedObjBytes);
        OCS(unackedObjBytes) = 0;
    }

    /* We received confirmation that application headers we sent 
     * were received by the remote device. 
     */
    if (OCC(seqNumError) == ERR_NONE) {
        /* Notice that even in the RETRANSMIT_NO_HEADERS state we don't clear
         * this value because it affects the FINAL bit in a GET-Request retransmit
         * case. However, we still won't end up retransmitting the headers.
         */
        OCS(unackedHeaderLen) = 0;
    }

    /* Save the current event handler */
    OCS(currentOperation) = OCC(currentOperation);
}


/*---------------------------------------------------------------------------
 *            ClientSessionResponseReceived()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function is called when we've received the entire server
 *            response packet.
 */
static void ClientSessionResponseReceived(ObexClientApp *ObexApp)
{
    /* We received the entire OBEX Packet from the server. Update session state
     * to reflect reception of the complete packet.
     */
    if (OCC(currentOperation) == GET_OPER_EVENTS) {
        OCS(ackedObjBytes) += OCS(unackedObjBytes);
        OCS(unackedObjBytes) = 0;
    }

    if (OCC(seqNumError) == ERR_NONE)
        OCS(seqNum)++;
}


/*---------------------------------------------------------------------------
 *            ClientRestoreState()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Restore the current client from a past OBEX session (via
 *            suspension or transport disconnection).  Only called when a
 *            Session Resume command is successful.
 *            
 *            NOTE: The application must keep the application header 
 *                  buffer and Object Store Entry open until the interrupted
 *                  operation can be completed. 
 *
 * Return:    void
 */
static void ClientRestoreState(ObexClientApp *ObexApp)
{
    ASSERT(OBSTACK_VerifyClientAppHandle(&ObexApp->handle));
    ASSERT(OCC(activeSession));
    ASSERT(OCC(seqNumError) <= ERR_RETRANSMIT_NO_HEADERS);  /* No critical errors. */

    OCC(obsh) = OCC(resumeObsh);

    /* Regardless of whether this is the first packet or not. If were in 
     * retransmit-no-headers, add in the acked object and header count.
     */
    if (OCC(seqNumError) == ERR_RETRANSMIT_NO_HEADERS) {
        /* We know the server received our request, but we didn't get
         * the response so act like we got the start of the response now.
         */
        ClientSessionCommandAcked(ObexApp);

        /* No application headers to send. */
        OCH(txLength) = 0;
    }

    /* Only restore these fields if we are in the middle of an operation, 
     * and not resending the start of an operation.
     */
    if (OCS(startSeqNum) != OCS(seqNum)) {
        ASSERT(((OCS(curOpcode) & ~OB_FINAL_BIT) == OB_OPCODE_PUT) ||
               ((OCS(curOpcode) & ~OB_FINAL_BIT) == OB_OPCODE_GET));
        ASSERT(OCC(obsh));

        /* Restore the current opcode & event handler index. */
        OCC(opcode) = OCS(curOpcode);
        OCC(currentOperation) = OCS(currentOperation);

        /* Restore the object offset & remaining length to where we left off.
         * AckedObjBytes has already been adjusted for (no)retransmit cases.
         */
        if ((OCC(opcode) & ~OB_FINAL_BIT) == OB_OPCODE_PUT) {
            OCC(objLen) = OCH(obsf)->GetObjectLen(OCC(obsh));
            GOEP_Report("[OBEX] ClientRestoreState OCC(objLen):%d", OCC(objLen) );
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
            if (OCC(objLen) != UNKNOWN_OBJECT_LENGTH)
#endif
                OCC(objLen) -= OCS(ackedObjBytes);
        }

    } else if (OCS(curOpcode) == OB_OPCODE_ABORT) {
        /* In the case of Abort, we want to restore the currentOperation 
         * for the operation being aborted.
         */
        OCC(currentOperation) = OCS(currentOperation);

        /* Because we only allow one oustanding command when a session is
         * active, we know that the last Put or Get command prior to the
         * Abort wasn't the final Put or Get packet.
         */
        if (OCC(currentOperation) == PUT_OPER_EVENTS) {
            OCC(opcode) = OB_OPCODE_PUT;
        } else {
            ASSERT(OCC(currentOperation) == GET_OPER_EVENTS);
            OCC(opcode) = OB_OPCODE_GET;
        }
    } else {
        /* Need to zap this because we are restarting the operation from
         * the start. The currentOperation was temporarily set during
         * the OBCE_COMPLETE event for resume to block the client from
         * starting another operation.
         */
        OCC(currentOperation) = NO_OPER_EVENTS;
    }

    /* If there are any unacked application headers, restore them now. */
    OCH(txLength) = OCS(unackedHeaderLen);

    /* Always set offset even if ackedObjBytes == 0. Even when
     * (startSeqNum == seqNum) we may have sent some object, so roll it back.
     */
    if (OCC(obsh) && OCH(obsf)->SetOffset(OCC(obsh), OCS(ackedObjBytes)) != OBRC_SUCCESS) {
        /* Set Offset Error.  Abort the operation. */
        OCC(status) = OBRC_CLIENT_RW_ERROR;
    }

    /* Reset the object unacked bytes because we have none outstanding now. */
    OCS(unackedObjBytes) = 0;
}


/*---------------------------------------------------------------------------
 *            SendNextPacket()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send the next packet for the current operation.  This is only 
 *            used to restart an operation that was interrupted via a
 *            transport disconnection.
 *
 * Return:    void
 */
static void SendNextPacket(ObexClientApp *ObexApp)
{
    ObStatus    status;

    ASSERT(OCC(seqNumError) <= ERR_RETRANSMIT_NO_HEADERS);  /* No critical errors. */

    if (OCS(startSeqNum) == OCS(seqNum)) {
        /* Restart the operation from the beginning by calling the
         * client API function used to initiate the operation.
         */

        if (OCS(curOpcode) == OB_OPCODE_PUT) {
            ASSERT(OCC(currentOperation) == 0);
            /* Restart the Put operation */
            status = OBEX_ClientPut(ObexApp, ObexApp->client.objLen);
        }
        else if (OCS(curOpcode) == OB_OPCODE_ABORT) {
            ASSERT((OCC(currentOperation) == PUT_OPER_EVENTS) || 
                   (OCC(currentOperation) == GET_OPER_EVENTS));
            /* Restart the Abort operation - ABORT NOW */
            ClientSaveFirst(ObexApp, OB_OPCODE_ABORT, 0);
            OCC(status) = OBRC_USER_ABORT;
            status = ObClientSendCommand(&ObexApp->handle, 
                            (OB_OPCODE_ABORT|OB_FINAL_BIT), HB_IDLE);
            OCC(unackedPackets)++;                
            if (status == OB_STATUS_SUCCESS)
                status = OB_STATUS_PENDING;
        } else {
            ASSERT(OCC(currentOperation) == 0);
            /* Restart the SetPath, Get, Connect, or Disconnect operations. */
            status = OBEX_ClientSendRequest( ObexApp, OCS(curOpcode),
                (U32)OCC(obsh), OCS(param2));
        }
        ASSERT(status == OB_STATUS_PENDING);
        
        if (status != OB_STATUS_PENDING) {
            /* Cannot restart the command, Abort the operation. */
            OCC(status) = OBD(clientParms).u.abortReason = OBRC_SESSION_ERROR;
            ClientComplete(ObexApp, OBCE_ABORTED);
        }
    } else {
        ASSERT(OCS(seqNum) > OCS(startSeqNum));
        /* Continue an operation (Put or Get) */
        OBEX_ClientSendCommand(ObexApp);
    }
}

/*---------------------------------------------------------------------------
 *            ClientClearSavedState()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Clears all saved client session operation state information.
 *
 */
static void ClientClearSavedState(ObexClientApp *ObexApp)
{
    ASSERT(OBSTACK_VerifyClientAppHandle(&ObexApp->handle));
    ASSERT(OCC(activeSession));

    /* Clear these values, since the operation is complete. */
    OCS(currentOperation) = NO_OPER_EVENTS;
    OCS(unackedObjBytes) = 0;
    OCS(ackedObjBytes) = 0;
    OCS(unackedHeaderLen) = 0;
}

/*---------------------------------------------------------------------------
 *            SuspendSession()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Saves client session state information before suspension.
 *
 */
static void SuspendSession(ObexClientApp *ObexApp)
{
    if (OCH(parser).flags & OBPF_CONNECTED) {
        /* We are connected; store the maxTxPacket value. */
        OCS(obPacketSize) = OCH(parser).maxTxPacket;
    } else {
        /* If we aren't connected, make sure we don't save 
         * an old maxTxPacket value. */
        OCS(obPacketSize) = 0;
    }

    /* Clear the active session */
    OCC(activeSession) = 0;
}

#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            ObClientGetHeaderBlockLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Calculate the remaining length of the current OBEX packet.
 *
 * Return:    Header Block Length
 */
U16 ObClientGetHeaderBlockLen(ObexClientApp *ObexApp, ObHeaderBlock hb)
{
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    if ((OCC(activeSession)) && (OCC(seqNumError) == ERR_RETRANSMIT_NO_HEADERS)) {
        /* Don't bother resending the same packet if the server has already
         * received it. Indicate that we have nothing to send.
         */
        return 0;
    } else {
        return OCC(txLength) + OCC(protoTxLen);
    }
#else
    return OCC(txLength);
#endif
}

#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
