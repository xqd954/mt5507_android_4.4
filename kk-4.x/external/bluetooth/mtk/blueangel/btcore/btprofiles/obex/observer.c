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
 * File:        observer.c
 *
 * Description: This file contains the code for the Server portion of 
 *              OBEX Command Interpreter and OBEX Layer API.
 *
 * Created:     May 4, 1997
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
 * Dec 5 2008 mbj06032
 * [MAUI_01347154] [BT OBEX] code revisement: assign value of server member in OBEX_ServerSendResponse
 * 
 *
 * Aug 7 2008 mbj06032
 * [MAUI_01099468] [BT OBEX] Client send PUT opcode with FinalBit in the las data packet
 * move OBEX_GET_USE_END_OF_BODY  defination to obconfig.h
 *
 * Jun 17 2008 mbj06032
 * [MAUI_01077981] [BT]ASSERT Fail: observer.c 1085 -BT
 * 
 *
 * Jun 13 2008 mbj06032
 * [MAUI_01072847] [BT OBEX] add trace info
 * 
 *
 * Jun 6 2008 mbj06032
 * [MAUI_01063800] [BT]Long time display "Connecting" and connect failed.
 * 
 *
 * Jun 2 2008 mbj06032
 * [MAUI_00678189] [PBAP]Can't get the missed call wth Nokia BH-902
 * 
 *
 * May 28 2008 mbj06032
 * [MAUI_00678189] [PBAP]Can't get the missed call wth Nokia BH-902
 * Server support fitting whole data into one packet with header, if possible
 *
 * May 15 2008 mbj06032
 * [MAUI_00259549] [BIP]_29 handset connect M8223 BIP handset popup "unfinished"
 * Server send Partial Content rsp code for partial content
 *
 * May 13 2008 mbj06032
 * [MAUI_01038033] [BT]Can connect server when press button no on Server.
 * OBEX_ServerAbort: remove event checking
 *
 * May 13 2008 mbj06032
 * [MAUI_00093122] Screen always displays recieving X%, x/x.
 * modify trace info
 *
 * May 7 2008 mbj06032
 * [MAUI_00318047] MTK:Bluetooth_Assert Fail:OSC(resp)!=(OBRC_CONTINUEIOB_FINAL_observer.c 872-BT
 * 
 *
 * May 6 2008 mbj06032
 * [MAUI_00318047] MTK:Bluetooth_Assert Fail:OSC(resp)!=(OBRC_CONTINUEIOB_FINAL_observer.c 872-BT
 * remove assert
 *
 * Apr 11 2008 mbj06032
 * [MAUI_00653702] [BT OBEX & GOEP ] add some trace info
 * 
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
#include "bluetooth_trc.h"
#include "bt_os_api.h"
#include "goep.h"

#undef OCC
#undef OCH
#undef OCS

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif


#if OBEX_ROLE_SERVER == XA_ENABLED

#define  IS_PBAP_SERVER(ObexApp) \
    (ObexApp->goep_service_type == GOEP_PROFILE_PBAP)

/* For PTS issue 6361-P workaround */
#define  IS_MAP_SERVER(ObexApp) \
    (ObexApp->goep_service_type >= GOEP_PROFILE_MAP_SERVER1 && ObexApp->goep_service_type <= GOEP_PROFILE_MNS_CLIENT2)

/****************************************************************************
 *
 * Function prototypes
 *
 ****************************************************************************/
void ProcessGeneral(ObexServerApp *ObexApp, ObexParserEvent event);
void ProcessPut(ObexServerApp *ObexApp, ObexParserEvent event);
void ProcessGet(ObexServerApp *ObexApp, ObexParserEvent event);
#if OBEX_SESSION_SUPPORT == XA_ENABLED 
void ProcessSession(ObexServerApp *ObexApp, ObexParserEvent event);
static void SessionSaveResponse(ObexServerApp *ObexApp);
static BOOL ObServerSessionLayerEventInd(ObexServerApp *ObexApp, ObexParserEvent Event);
static void SuspendSession(ObexServerApp *ObexApp);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
static void CancelOperation(ObexServerApp *ObexApp);
#if OBEX_SERVER_CONS_SIZE > 0
static void AssociateConn(ObexServerApp *ObexApp);
static BOOL CheckforConnId(ObexServerApp *ObexApp);
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
static BOOL AppCallback(ObexServerApp *ObexApp, ObServerEvent Event);

/*---------------------------------------------------------------------------
 *            OBEX_ServerInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize the OBEX server command interpreter.
 *
 * Return:    OB_STATUS_SUCCESS - if successful.
 *            OB_STATUS_FAILED - otherwise
 *            OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ServerInit(ObexServerApp *ObexApp, ObServerAppCb ServerCb, 
                         ObexTpType Protocols, ObexL2BufferData *pl2data)
{
    ObStatus    status = OB_STATUS_FAILED;
#if OBEX_SERVER_CONS_SIZE > 0 
    U8          i;
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
#if BT_GOEP_V2 == XA_ENABLED
    U8 idx;
#endif

    OS_LockObex();

    /* Check if the Server is already in use */
#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObexApp || !ServerCb) {
        status = OB_STATUS_INVALID_PARM;
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    
    ASSERT(ObexApp && ServerCb);
    
#if BT_GOEP_V2 == XA_ENABLED
    kal_trace(BT_TRACE_G6_OBEX, OBEX_L2CAP_INIT, &ObexApp->l2buffer);
    
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
    // ObexApp->trans.ObexServerBtTrans.server.pl2Buffer = &ObexApp->l2buffer; /*will be init and clear */
#endif
#if BT_GOEP_V2 == XA_ENABLED
	/* put l2data into ObexApp */
    ObexApp->l2buffer.rxNum = 0;
    if( NULL != pl2data && pl2data->l2capRawDataNum > 0 ){
        GOEP_Report("[GOEP][SERV] RegisterServer l2capRawDataNum is %d ", pl2data->l2capRawDataNum);
        for( idx = 0; idx < pl2data->l2capRawDataNum && idx < GOEPL2_MAX_RETRANS_NUM ; idx++){
            kal_trace(BT_TRACE_G6_OBEX, GOEP_SERVER_L2CAP_RAWDATA_BUFF, pl2data->pl2capRawData[idx]);
            ASSERT( pl2data->l2capRawData[idx] != NULL );
            ObexApp->l2buffer.rfcRxPackets[idx].data = pl2data->pl2capRawData[idx];
            InsertTailList( &ObexApp->l2buffer.rxPacketsBufferList, &ObexApp->l2buffer.rfcRxPackets[idx].node);
            ObexApp->l2buffer.rxNum = idx+1;
        }
    }

	/* put l2data into transport */
	if( NULL != pl2data ){
		// ObexApp->trans.ObexServerBtTrans.server.bGoepL2cap = pl2data->tpType; /* will be inited and clear */ // TODO: remove this
		ObexApp->bSupportSRM = pl2data->bSRM;
		ObexApp->tptype = pl2data->tpType;
	}
#endif 

    /* Initialize the transport component */
    if ((status = OBSTACK_ServerInit(ObexApp, Protocols)) != OB_STATUS_SUCCESS) {
        goto Done;
    }

    /* Initialize the Server Context - OBSTACK_ServerInit will make sure we aren't
     * trying to initialize a duplicate Server
     */
    OS_MemSet((U8 *)(&ObexApp->server), 0, sizeof(ObServerContext));

    /* Verify the AppHandle and Object Store Function table */
#if XA_ERROR_CHECK == XA_ENABLED
    if (!OBSTACK_VerifyServerAppHandle(&ObexApp->handle)) {
        status = OB_STATUS_INVALID_PARM;
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(OBSTACK_VerifyServerAppHandle(&ObexApp->handle));

    /* Initialize some command interpreter variables */
    OSC(doCallback) = FALSE;
    OSC(appCallback) = ServerCb;

    OSH(txLength) = 0;

    OSC(txLength) = 0;
    OSC(currentOperation) = NO_OPER_EVENTS;

#if OBEX_SERVER_CONS_SIZE > 0
    OSC(protoTxLen) = 0;
    for (i = 0; i < OBEX_SERVER_CONS_SIZE; i++)
        OSC(conn[i]) = 0;

    OSC(activeConnId) = OBEX_INVALID_CONNID;
#endif /* OBEX_SERVER_CONS_SIZE > 0 */

    /* Initialize the server parser */
    ObParserSetModeServer(OSH(parser), ObServerEventInd);
    ObParserSetInitState(&OSH(parser));

    status = OB_STATUS_SUCCESS;

Done:    
    OS_UnlockObex();
    return status;
}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBEX_ServerDeinit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize the OBEX Server command interpreter.
 *
 * Return:    OB_STATUS_SUCCESS  - The parser was deinitialized.
 *            OB_STATUS_BUSY - An Operation is in progress.
 *            OB_STATUS_FAILED - The transport layer is active.
 *            OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ServerDeinit(ObexServerApp *ObexApp)
{
    ObStatus    status;

    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObexApp || !OBSTACK_VerifyServerAppHandle(&ObexApp->handle)) {
        status = OB_STATUS_INVALID_PARM;
        goto Done;
    }
    if (OSC(currentOperation) != NO_OPER_EVENTS) {
        status = OB_STATUS_BUSY;
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(ObexApp && OSC(currentOperation) == NO_OPER_EVENTS && 
           OBSTACK_VerifyServerAppHandle(&ObexApp->handle));
    
    if (OBSTACK_ServerDeinit(ObexApp) == FALSE) {
        status = OB_STATUS_FAILED;
        goto Done;
    }

    OSC(appCallback) = 0;

    status = OB_STATUS_SUCCESS;

Done:    
    OS_UnlockObex();
    return status;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            OBEX_ServerAbort()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Abort the current server operation. An operation must be in 
 *            progress in order to be aborted.
 *
 * Return:    OB_STATUS_SUCCESS - Abort operation was successful.
 *            OB_STATUS_FAILED - Abort operation failed.
 *            OB_STATUS_NO_CONNECT - No transport connection exists.
 *            OB_STATUS_INVALID_PARM - Invalid parameters.
 */
ObStatus OBEX_ServerAbort(ObexServerApp *ObexApp, ObexRespCode Resp)
{
    ObStatus status;

    OS_LockObex();

    kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_ABORT, Resp);
	
#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObexApp || !OBSTACK_VerifyServerAppHandle(&ObexApp->handle)) {
        status = OB_STATUS_INVALID_PARM;
        goto Done;
    }

    if (!OBSTACK_IsConnectionUp(OSH(parser.stack))) {
        /* No transport connection exists */
        status = OB_STATUS_NO_CONNECT;
        goto Done;
    }

    if ((Resp & ~OB_FINAL_BIT) <= OBRC_SUCCESS) {
        /* Invalid abort response code */
        kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_ABORT_RSP_INVALID, Resp);
        status = OB_STATUS_FAILED;
        goto Done;
    }

    /* for CR[MAUI_01038033]: 
     * when the second server(PBAP server) disconnected, 
     * OBD(serverParms).event is set to OBSE_ABORTED.
     */
#if 0 
    if ((OSC(currentOperation) == NO_OPER_EVENTS) && 
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
        (OBD(serverParms).event != OBSE_SEND_RESPONSE) &&
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
        (OBD(serverParms).event != OBSE_PRECOMPLETE) &&
        (OBD(serverParms).event != OBSE_DELETE_OBJECT)) {
        /* The current operation is cleared upon receipt of the last
         * request packet.  In this case there are three event possibilities 
         * that could occur while the operation is still in progress -
         * OBSE_SEND_RESPONSE, OBSE_PRECOMPLETE, and OBSE_DELETE_OBJECT.
         */
        kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_ABORT_EVENT_INVALID, OSC(currentOperation),OBD(serverParms).event);
        status = OB_STATUS_FAILED;
        goto Done;
   }
#endif

#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(ObexApp && OBSTACK_VerifyServerAppHandle(&ObexApp->handle));
    ASSERT(OBSTACK_IsConnectionUp(OSH(parser.stack)));
    ASSERT((Resp & ~OB_FINAL_BIT) > OBRC_SUCCESS);

	if ((Resp & ~OB_FINAL_BIT) != OBRC_PARTIAL_CONTENT)
	{
		/* maybe OBRC_PARTIAL_CONTENT has data to send */
        CancelOperation(ObexApp);
	}

    OSC(resp) = (Resp | OB_FINAL_BIT);
    OSC(rejectResp) = TRUE;
    status = OB_STATUS_SUCCESS;

#if XA_ERROR_CHECK == XA_ENABLED
Done:
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    OS_UnlockObex();
    return status;
}

#if OBEX_BODYLESS_GET == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBEX_ServerAcceptNoObject()
 *---------------------------------------------------------------------------
 * Synopsis:  Accepts the current operation but does not specify an object
 *            handle (used in a GET operation which results in no later BODY/
 *            END-OF-BODY headers).
 *
 * Return:    void
 */
void OBEX_ServerAcceptNoObject(ObexServerApp *ObexApp)
{
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObexApp || !OBSTACK_VerifyServerAppHandle(&ObexApp->handle)) {
        goto Done;
    }
	
    /* Detect if the server tries to accidentally do an accept after
     * the operation has been cancelled.
     */
    if (OSC(currentOperation) != GET_OPER_EVENTS) {
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(ObexApp && OBSTACK_VerifyServerAppHandle(&ObexApp->handle));
    ASSERT((OSC(currentOperation) == GET_OPER_EVENTS));

    OSC(obshAllowEmpty) = TRUE;
    
#if XA_ERROR_CHECK == XA_ENABLED
Done:
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    OS_UnlockObex();
}
#endif /* OBEX_BODYLESS_GET == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            OBEX_ServerAccept()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Accepts the current operation indicated to the 
 *            server by providing an object store handle that
 *            is necessary to proceed with the operation.
 *
 * Return:    void
 */
void OBEX_ServerAccept(ObexServerApp *ObexApp, void *Obsh)
{
    U8 i;
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObexApp || !Obsh || !OBSTACK_VerifyServerAppHandle(&ObexApp->handle)) {
        goto Done;
    }
	
    /* Detect if the server tries to accidentally do an accept after
     * the operation has been cancelled.
     */
    i = OSC(currentOperation); 
    if ((OSC(currentOperation) != PUT_OPER_EVENTS) &&
        (OSC(currentOperation) != GET_OPER_EVENTS)) {
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(ObexApp && Obsh && OBSTACK_VerifyServerAppHandle(&ObexApp->handle));
    ASSERT((OSC(currentOperation) == GET_OPER_EVENTS) ||
           (OSC(currentOperation) == PUT_OPER_EVENTS));

    /* ASSERT that we don't already have an Object store handle 
     */
/*    Since we use buffer as interface to transfer the data, remove this assert */
/*    ASSERT(OSC(obsh) == 0);*/
    
    OSC(obsh) = Obsh;

    /* If this is for a GET operation, initialize the object length. */
    if (OSC(currentOperation) == GET_OPER_EVENTS)
        OSC(objLen) = OSH(obsf)->GetObjectLen(OSC(obsh));

#if XA_ERROR_CHECK == XA_ENABLED
Done:
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    OS_UnlockObex();
}

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBEX_ServerAcceptSession()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Accepts the current session operation (Create or Resume) 
 *            indicated to the server by providing a Server Session that
 *            is necessary to proceed with the operation.
 *
 * Return:    OB_STATUS_SUCCESS - The session was accepted successfully.
 *            OB_STATUS_FAILED - The server was not ready to accept a session.
 *            OB_STATUS_INVALID_HANDLE - An object store handle is required but
 *                not specified in the resume parameters.
 *            OB_STATUS_INVALID_PARM - A provided session parameter was invalid
 *                or the session does not match the one the client requested.
 */
ObStatus OBEX_ServerAcceptSession(ObexServerApp *ObexApp, ObexServerSession *Session, 
                                  ObexSessionResumeParms *ResumeParms)
{
#if XA_ERROR_CHECK == XA_ENABLED
    U8          sessId[16];
#if OBEX_SERVER_CONS_SIZE > 0
    I8          i;
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ObStatus    status = OB_STATUS_INVALID_PARM;

    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObexApp || !Session || !OBSTACK_VerifyServerAppHandle(&ObexApp->handle)) {
        goto Done;
    }

    /* Detect if the server tries to accidentally do an accept after
     * the operation has been cancelled.
     */
    if (OSC(currentOperation) != SESSION_OPER_EVENTS) {
        status = OB_STATUS_FAILED;
        goto Done;
    }

    if (OSH(sessionParms).opcode != OB_SESSION_OPCODE_CREATE) {
        if (OSH(sessionParms).opcode != OB_SESSION_OPCODE_RESUME) {
            status = OB_STATUS_FAILED;
            goto Done;
        }

        /* Verify Device Address */
        if (!OS_MemCmp(OSH(sessionParms).device, OSH(sessionParms).deviceLen,
                       Session->clientDevAddr, (U16)GetDevAddrLen(Session->tpType)))
            goto Done;

        /* Verify Nonce */
        if (!OS_MemCmp(OSH(sessionParms).nonce, OSH(sessionParms).nonceLen, 
                       Session->clientNonce, Session->clientNonceLen)) 
            goto Done;

        /* Verify Session Id */
        OBEX_CreateSessionId(Session, sessId);

        if (!OS_MemCmp(OSH(sessionParms).sessionId, 16, sessId, 16)) 
            goto Done;

        if (Session->currentOperation && ResumeParms) {
            /* We are attempting to resume an OBEX operation. If were not
             * done we must have a valid Object Store Handle and if there
             * were headers to send they must also be provided here.
             */
            if (((Session->currentOperation == PUT_OPER_EVENTS) ||
                 (Session->currentOperation == GET_OPER_EVENTS)) &&
                (Session->ackedObjBytes || Session->unackedObjBytes) &&
                (ResumeParms->obsh == 0)) {
                status = OB_STATUS_INVALID_HANDLE;
                goto Done;
            }

            if (((Session->unackedHeaderLen != ResumeParms->headerLen)) ||
                ((Session->unackedHeaderLen > 0) && (ResumeParms->headerBuff == 0))) {
                goto Done;
            }

            if (Session->unackedHeaderLen > OSH(length)) {
                /* There are more resume headers then we have buffer space for. */
                status = OB_STATUS_FAILED;
                goto Done;
            }

#if OBEX_SERVER_CONS_SIZE > 0
            /* There must be an OBEX Connection registered for each slot that was
             * in-use when the suspend occurred.
             */
            for (i = 0; i < OBEX_SERVER_CONS_SIZE; i++) {
                if ((Session->connId[i] != 0) && (OSC(conn)[i] == 0)) {
                    goto Done;
                }
            }
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
        }
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(ObexApp && Session && OBSTACK_VerifyServerAppHandle(&ObexApp->handle));
    ASSERT(OSC(currentOperation) == SESSION_OPER_EVENTS);
    ASSERT(OSH(sessionParms).opcode == OB_SESSION_OPCODE_RESUME || 
           OSH(sessionParms).opcode == OB_SESSION_OPCODE_CREATE);

    /* ASSERT that we don't already have an Object store handle 
     * and that there is no active session.
     */
    ASSERT((OSC(obsh) == 0) && (OSC(activeSession) == 0));
    
    if (OSH(sessionParms).opcode == OB_SESSION_OPCODE_CREATE) {
        /* Initialize this session with the client DevAddr & Nonce. */
        OS_MemSet((U8 *)Session, 0, sizeof(ObexServerSession));

        Session->timeout = Session->ourTimeout = 0xFFFFFFFF;

        OS_MemCopy(Session->clientDevAddr, OSH(sessionParms).device, 
                   OSH(sessionParms).deviceLen);
        OS_MemCopy(Session->clientNonce, OSH(sessionParms).nonce, 
                   OSH(sessionParms).nonceLen);
        Session->clientNonceLen = OSH(sessionParms).nonceLen;
    }

    /* Update the timeout if the client specified one. */
    if (OSH(sessionParms).fields & OBSP_FIELD_TIMEOUT)
        Session->timeout = min(OSH(sessionParms).timeout, Session->ourTimeout);

    /* Set the response code to positive, since we assumed failure 
     * before (in ProcessSession). Clear out possibly stale resumeObsh value.
     */
    if (!OSC(rejectResp)) 
        OSC(resp) = (OBRC_SUCCESS | OB_FINAL_BIT);
    OSC(activeSession) = Session;
    OSC(resumeObsh) = 0;

    if (OSH(sessionParms).opcode == OB_SESSION_OPCODE_RESUME) {
        if (ResumeParms != 0) {
            /* Save the restore information for use when the session opcode is complete. */
            OSC(resumeObsh) = ResumeParms->obsh;
            OSC(resumeHeaders) = ResumeParms->headerBuff;
        }
        /* Set the resume operation flag, for later. */
        OSC(sessionFlags) |= OB_SESSION_RESUME;
    } 
    status = OB_STATUS_SUCCESS;

#if XA_ERROR_CHECK == XA_ENABLED
Done:
#endif
    OS_UnlockObex();
    return status;
}
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

#if OBEX_SERVER_CONS_SIZE > 0
/*---------------------------------------------------------------------------
 *            OBEX_ServerRegisterTarget()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Allows the server application to specify a target header
 *            used for by OBEX to establish a directed connection.
 *
 * Return:    OB_STATUS_SUCCESS - Target header accepted
 *            OB_STATUS_FAILED  - Insufficient conn table space.
 *            OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ServerRegisterTarget(ObexServerApp *ObexApp, 
                                   ObexConnection *conn)
{
    U8 i;

    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObexApp || !conn || !OBSTACK_VerifyServerAppHandle(&ObexApp->handle)) {
        OS_UnlockObex();
        return OB_STATUS_INVALID_PARM;
    }

    if (conn->targetLen > OBEX_MAX_SERVICE_UUID_SIZE) {
        OS_UnlockObex();
        return OB_STATUS_FAILED;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    
    ASSERT(ObexApp && conn && OBSTACK_VerifyServerAppHandle(&ObexApp->handle));
    ASSERT(conn->targetLen <= OBEX_MAX_SERVICE_UUID_SIZE);

    for (i = 0; i < OBEX_SERVER_CONS_SIZE; i++) {
        if (OSC(conn[i]) == 0) {
            OSC(conn[i]) = conn;
            conn->connId = OBEX_INVALID_CONNID;
            OS_UnlockObex();
            return OB_STATUS_SUCCESS;
        }
    }

    OS_UnlockObex();
    return OB_STATUS_FAILED;
}

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBEX_ServerDeregisterTarget()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Removes a registered target header.
 *
 * Return:    OB_STATUS_SUCCESS - Target header removed.
 *            OB_STATUS_BUSY - Obex connection exists to this target.
 *            OB_STATUS_FAILED  - Target header not found or conn busy.
 *            OB_STATUS_INVALID_PARM - invalid parameters.
 */
ObStatus OBEX_ServerDeregisterTarget(ObexServerApp *ObexApp, 
                                     ObexConnection *conn)
{
    U8 i;

    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObexApp || !conn || !OBSTACK_VerifyServerAppHandle(&ObexApp->handle)) {
        OS_UnlockObex();
        return OB_STATUS_INVALID_PARM;
    }

    /* Can't deregister a connection while it's active */
    if (OBEX_ServerGetConnInfo(ObexApp) == conn) {
        OS_UnlockObex();
        return OB_STATUS_BUSY;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    
    ASSERT(ObexApp && conn && OBSTACK_VerifyServerAppHandle(&ObexApp->handle));
    ASSERT(OBEX_ServerGetConnInfo(ObexApp) != conn);

    for (i = 0; i < OBEX_SERVER_CONS_SIZE; i++) {
        if (OSC(conn[i]) == conn) {
            OSC(conn[i]) = 0;
            conn->connId = OBEX_INVALID_CONNID;
            OS_UnlockObex();
            return OB_STATUS_SUCCESS;
        }
    }

    OS_UnlockObex();
    return OB_STATUS_FAILED;
}
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            OBEX_ServerGetConnInfo()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns a pointer to the currently active OBEX Connection.
 *            This function is valid from the OBSE_XXX_START indication
 *            through the OBSE_COMPLETE or OBSE_ABORTED indication.
 *
 * Return:    non-zero if successful.
 *            zero if active connection is default/inbox connection.
 */
ObexConnection *OBEX_ServerGetConnInfo(ObexServerApp *ObexApp)
{
    OS_LockObex();
    ASSERT(ObexApp && OBSTACK_VerifyServerAppHandle(&ObexApp->handle));
    /* Return the current connection structure if one is active. */
    if (OSC(activeConnId) != OBEX_INVALID_CONNID) {
        ASSERT(OSC(conn[OSC(activeConnId)]) != 0);
        
        OS_UnlockObex();
        return OSC(conn[OSC(activeConnId)]);
    }
    
    /* Default connection */
    OS_UnlockObex();
    return 0;
}
#endif /* OBEX_SERVER_CONS_SIZE > 0 */

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBEX_ServerSetSessionTimeout()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets the session timeout for the active session on the OBEX
 *            server.  This timeout value will be sent to the client to 
 *            indicate the required OBEX Session Suspend timeout on the server.  
 *            If this value is lower than the value requested by the client, 
 *            this value will be set as the OBEX Session Suspend timeout.  At 
 *            every negotiation, this timeout will be requested, regardless of
 *            the last negotiated OBEX Session Suspend timeout.
 *
 * Return:    OB_STATUS_SUCCESS - Timeout successfully set.
 *            OB_STATUS_INVALID_PARM - A parameter is invalid.
 */
ObStatus OBEX_ServerSetSessionTimeout(ObexServerApp *ObexApp, U32 Timeout)
{
    ObStatus status = OB_STATUS_SUCCESS;

    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObexApp || !OBSTACK_VerifyServerAppHandle(&ObexApp->handle) ||
        !OSC(activeSession)) {
        status = OB_STATUS_INVALID_PARM;
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(ObexApp && OBSTACK_VerifyServerAppHandle(&ObexApp->handle) &&
           OSC(activeSession));

    if (Timeout != 0) {
        OSS(ourTimeout) = Timeout;
        OSS(timeout) = min(OSS(ourTimeout), OSS(timeout));
    }

#if XA_ERROR_CHECK == XA_ENABLED
Done:
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    OS_UnlockObex();
    return status;
}
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            OBEX_ServerSendResponse()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sends PUT or GET response to the client. This function is
 *            public only if OBEX_PACKET_FLOW_CONTROL is enabled.
 *
 * Return:    void
 */
ObStatus OBEX_ServerSendResponse(ObexServerApp *ObexApp)
{
	ObStatus status = BT_STATUS_FAILED;
    OS_LockObex();

    kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_SEND_RESPONSE, ObexApp, OSC(resp),ObServerGetOpcode());

#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObexApp || !OBSTACK_VerifyServerAppHandle(&ObexApp->handle)) {
        goto Done;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(ObexApp && OBSTACK_VerifyServerAppHandle(&ObexApp->handle));

#ifndef __BT_GOEP_V2__
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
    //ASSERT(OSC(sendResponse) == 1);
    if (OSC(sendResponse) == 0) {
        OS_UnlockObex();
        return status;
    }
    OSC(sendResponse)--;
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
#endif

#ifdef __BT_GOEP_V2__
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
    if( OBSH_SRM_ENABLE != OSC(srm_server) ){
        if (OSC(sendResponse) == 0) {
    		kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_SENDRESPONSE_COUNT_REJECT, OSC(sendResponse));
            OS_UnlockObex();
            return status;
        }
        OSC(sendResponse)--;
    }else{
        // flush out all packets before response the ok
        if( 0xff == ObServerGetOpcode() ){
            OS_Report("[OBEX] server flush +");
            OBSTACK_ServerSRMAbort(ObexApp);
            OS_Report("[OBEX] server flush -");            
        }
    }
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
#endif
    
    OSC(headerBlock) = HB_IDLE;

    /* Assign the OBEX server to the callback parms */
    OBD(serverParms).server = ObexApp;

    /* Get and Session process functions utilize the OBPE_BUILD_RESPONSE
     * event to setup and build headers for response packets.
     */
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    if ((OSC(currentOperation) == GET_OPER_EVENTS) ||
        (OSC(currentOperation) == SESSION_OPER_EVENTS)) {
#else
    if (OSC(currentOperation) == GET_OPER_EVENTS) {
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

        ServerEventHandlerTable[OSC(currentOperation)](ObexApp, OBPE_BUILD_RESPONSE);
    }

    /* Do we need to send any headers built by the app (or OBEX)? */
    if (OSC(txLength) == 0) {
        OSC(txLength) = OSH(txLength);
        OSH(txLength) = 0;

#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
        if ((OSC(txLength) + OSC(protoTxLen)) > 0) {
            if ((OSC(txLength) + OSC(protoTxLen)) > ObServerMaxHeaderBlockSize()) {
                /* The application managed to pile on more headers than we can
                 * respond with (because of protocol response headers), ABORT.
                 */
                kal_trace(BT_TRACE_G6_OBEX,OBEX_TOOMUCH_RESPHEADERS, OSC(txLength) + OSC(protoTxLen), ObServerMaxHeaderBlockSize()); 
                CancelOperation(ObexApp);
                OSC(resp) = (OBRC_INTERNAL_SERVER_ERR | OB_FINAL_BIT);
            } else {
                OSC(headerBlock) = HB_SERVER_HEADERS;
            }
        }
#else /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
        if (OSC(txLength) > 0) {
            OSC(headerBlock) = HB_SERVER_HEADERS;
        }
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
    }
    OSC(txOffset) = 0;

    /*
     * If were done, give the application a chance to complete the request
     * and abort if it encounters an error. This is only necessary for
     * successfully completing, Put, Get and Setpath operations.
     * Don't generate PRECOMPLETE if we are retransmitting this response.
     */
    if ((OSC(resp) == (OBRC_SUCCESS|OB_FINAL_BIT)) &&
#if OBEX_SESSION_SUPPORT == XA_ENABLED
        (OSC(seqNumError) != ERR_RETRANSMIT_AND_IGNORE) &&
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
        (((ObServerGetOpcode() & 0x7f) == OB_OPCODE_PUT) ||
        ((ObServerGetOpcode() & 0x7f) == OB_OPCODE_GET) ||
        ((ObServerGetOpcode() & 0x7f) == OB_OPCODE_SET_PATH))) {

        OBD(serverParms).event = OBSE_PRECOMPLETE;
        OSC(appCallback)(&OBD(serverParms));
    }

#if OBEX_SESSION_SUPPORT == XA_ENABLED
    if (OSC(activeSession) && (OSC(currentOperation) != SESSION_OPER_EVENTS))
        SessionSaveResponse(ObexApp);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

    /* Send the response. */
    OSC(rejectResp) = FALSE;
    status = ObServerSendResponse(&ObexApp->handle, OSC(resp), OSC(headerBlock));

#if XA_ERROR_CHECK == XA_ENABLED
Done:
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    OS_UnlockObex();
	return status;
}


/*---------------------------------------------------------------------------
 *            ObServerEventInd()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  OBEX Server callback.
 *
 * Return:    void
 */
void ObServerEventInd(ObexAppHandle *AppHndl, ObexParserEvent event)
{
    ObexServerApp      *ObexApp = (ObexServerApp*)AppHndl;

    ASSERT(AppHndl);

#ifdef __BT_GOEP_V2__
    kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_EVENT_IND, event, OSC(currentOperation) );
#else
    if (event != OBPE_PUT_HEADER && event != OBPE_WRITE_HEADER)
    {
        kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_EVENTIND, event);
    }
#endif    
   
    /* Assign the OBEX server to the callback parms */
    OBD(serverParms).server = ObexApp;
    OBD(serverParms).opcode = (ObServerGetOpcode() & 0x7f);
    /* Save the last Put or Get opcode in progress */
    if ((OBD(serverParms).opcode == OB_OPCODE_PUT) || 
        (OBD(serverParms).opcode == OB_OPCODE_GET))
        OSC(opcode) = OBD(serverParms).opcode;

#if OBEX_SESSION_SUPPORT == XA_ENABLED
    if (OBD(serverParms).opcode == OB_OPCODE_SESSION)
        OBD(serverParms).opcode = 0x10 + OSH(sessionParms).opcode;

    if (ObServerSessionLayerEventInd(ObexApp, event) == FALSE) {
        return;
    }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

    switch (event) {
    case OBPE_OPER_COMPLETE:
        /* The final response packet has been successfully sent. */
        OSC(txLength) = 0;

        if (OSC(doCallback) == TRUE) {
            OSC(doCallback) = FALSE;
            AppHndl->txLength = 0;

            /* Indicate operation completion to the application. */
            if ((OSC(resp) == (OBRC_SUCCESS|OB_FINAL_BIT)) &&
                ((ObServerGetOpcode() & 0x7f) != OB_OPCODE_ABORT)) {

#if OBEX_SESSION_SUPPORT == XA_ENABLED
                /* See if we just completed a successful session operation. */
                if ((OSC(currentOperation) == SESSION_OPER_EVENTS)) {
                    if (OSH(sessionParms).opcode == OB_SESSION_OPCODE_RESUME) {
                        /* Setup any suspended operation stuff that we have. */
                        if (OSS(unackedHeaderLen) > 0)
                            OS_MemCopy(OSH(buffer), OSC(resumeHeaders), OSS(unackedHeaderLen));

                        /* Restore the OBEX Packet size. */
                        if ((OSH(parser).maxTxPacket = OSS(obPacketSize)) > 0)
                            OSH(parser).flags |= OBPF_CONNECTED;
                        else OSH(parser).flags &= ~OBPF_CONNECTED;

                    }
                    else if (OSH(sessionParms).opcode == OB_SESSION_OPCODE_SUSPEND) {
                        SuspendSession(ObexApp);
                    }

                    if (OSC(sessionFlags) & OB_SESSION_DEACTIVATE) {
                        /* Active session was suspended or closed. No active session now. */
                        OSC(activeSession) = 0;
                    }
                }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

                OBD(serverParms).event = OBSE_COMPLETE;
                OSC(appCallback)(&OBD(serverParms));

            } else {
                /* Indicate the Put or Get operation that was aborted */
                kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_EVENTIND_OPER_ABORTED,OSC(resp),ObServerGetOpcode(),OSC(opcode));
                
                if ((OBD(serverParms).opcode == OB_OPCODE_ABORT) && OSC(opcode))
                    OBD(serverParms).opcode = OSC(opcode);

                 /* modified for CR [MAUI_00318047] */
				//ASSERT(OSC(resp) != (OBRC_CONTINUE|OB_FINAL_BIT));
				
			    OBD(serverParms).u.abortReason = OSC(resp);
                OBD(serverParms).event = OBSE_ABORTED;
                OSC(appCallback)(&OBD(serverParms));
            }
        }
#if OBEX_SERVER_CONS_SIZE > 0
        /* The operation is over, reset the current conn id */
        OSC(activeConnId) = OBEX_INVALID_CONNID;
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
        /* Operation finished. */
        OSC(opcode) = 0;
        return;

    case OBPE_OPER_IND_START:
        /* This indicates the start of an operation. Initialize the 
         * server response code to continue and the processing function
         * based on the OBEX Packet Opcode.
         */
        OSC(headerBlock) = HB_IDLE;

#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
        OSC(startEvent) = OBSE_NO_EVENT;
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
        
#ifdef __BT_GOEP_V2__        
       kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_EVENT_START_IND, (ObServerGetOpcode() & 0x7f), ObexApp->trans.ObexServerBtTrans.server.bConnType );
       if( OB_OPCODE_CONNECT == (ObServerGetOpcode() & 0x7f) ){
            // check mtu
            if( OBEX_TP_L2CAP_ONLY == ObexApp->trans.ObexServerBtTrans.server.bConnType ){
                if(  OSH(parser).maxTxPacket > (OBSTACK_MaxTxSize(OSH(parser).stack)-10)   ){
                    kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_REMOTE_MTU_TOO_LARGE_WRN , OSH(parser).maxTxPacket, OBSTACK_MaxTxSize( OSH(parser).stack));
                    OSH(parser).maxTxPacket = OBSTACK_MaxTxSize( OSH(parser).stack)  - 10; //QDAY room for l2cap
                }
            }
        }
#endif       
        
        switch (ObServerGetOpcode() & 0x7f) {
        case OB_OPCODE_CONNECT:
        case OB_OPCODE_DISCONNECT:
        case OB_OPCODE_SET_PATH:
	  case OB_OPCODE_ACTION:
            if (!OSC(rejectResp)) 
                OSC(resp) = (OBRC_SUCCESS | OB_FINAL_BIT);
            OSC(currentOperation) = SIMPLE_OPER_EVENTS;
            break;

#if OBEX_SESSION_SUPPORT == XA_ENABLED
        case OB_OPCODE_SESSION:
            OSC(currentOperation) = SESSION_OPER_EVENTS;
            break;
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

        case OB_OPCODE_ABORT:
            /* If we have an operation in progress, Abort it.
             * Otherwise process the Abort Packet.
             */
            if (OSC(currentOperation) != NO_OPER_EVENTS) {
                CancelOperation(ObexApp);

#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
               /* OBEX must always indicate the ABORT event when
                * packet flow control is enabled because the app.
                * needs to know to stop processing now!
                */
                OSC(currentOperation) = SIMPLE_OPER_EVENTS;
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */
            } 
            else OSC(currentOperation) = SIMPLE_OPER_EVENTS;
            break;

        case OB_OPCODE_PUT:
            OSC(currentOperation) = PUT_OPER_EVENTS;
            break;

        case OB_OPCODE_GET:
            OSC(currentOperation) = GET_OPER_EVENTS;
            break;

        default:
            /* We have no handler for this Opcode.
             * The server will respond with OBRC_NOT_IMPLEMENTED.
             */
            OSC(resp) = (OBRC_NOT_IMPLEMENTED | OB_FINAL_BIT);
            OSC(currentOperation) = NO_OPER_EVENTS;
            break;
        }
        break;
    
    case OBPE_DISCONNECT:
        /* This is a Transport Layer disconnect event. If there is an active
         * operation terminate  it.
         */
        OSC(txLength) = AppHndl->txLength = 0;

        /* Inform the App that the Operation was aborted (or suspended).
         * If the event handler index == NO_EVENT_HANDLER we may be
         * aborting the current operation or we may be idle.
         */
        if (OSC(doCallback) == TRUE) {
            OSC(doCallback) = FALSE;
            if ((OBD(serverParms).opcode == OB_OPCODE_ABORT) && OSC(opcode))
                OBD(serverParms).opcode = OSC(opcode);
			OBD(serverParms).u.abortReason = OBRC_LINK_DISCONNECT;
            OBD(serverParms).event = OBSE_ABORTED;
            OSC(appCallback)(&OBD(serverParms));
        }
            
        /* Clean up the operation */
        CancelOperation(ObexApp);
        OSC(opcode) = 0;

#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
        OSC(sendResponse) = 0;
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */

#if OBEX_SERVER_CONS_SIZE > 0
        OSC(activeConnId) = OBEX_INVALID_CONNID;
#endif /* OBEX_SERVER_CONS_SIZE > 0 */

        /* Now report that the link is down. */
        OBD(serverParms).event = OBSE_DISCONNECT;
        OSC(appCallback)(&OBD(serverParms));
        break;

    case OBPE_PUT_HEADER:
        if (OSC(currentOperation) == NO_OPER_EVENTS)
            return;
            
        /* Indicate all 1 & 4 byte headers to the application here. */
        if (OBEXH_Is4Byte(ObServerGetHeaderType()) ||
            OBEXH_Is1Byte(ObServerGetHeaderType()) ) {
#if OBEX_SERVER_CONS_SIZE > 0
            if (CheckforConnId(ObexApp))
                return;
#endif /* OBEX_SERVER_CONS_SIZE > 0 */

            AppCallback(ObexApp, OBSE_HEADER_RX);
            return;
        } 
        break;
#ifdef __BT_GOEP_V2__
    case OBPE_TX_COMPLETE:
		GOEP_Report("[SRM] ObServerEventInd Tx_COMPLETE! srm_server:%d curOp:%d", OSC(srm_server), OSC(currentOperation));
		/* Process abort */
		if( OSC(srm_server) == OBSH_SRM_ENABLE && OSC(currentOperation) == OBPE_NO_EVENT){
			// receive the abort and check again!
			OSC(sendResponse)++;
			AppCallback(ObexApp, OBSE_SEND_RESPONSE);
		}
		break;
#endif
    } /* End Switch() */
    
#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
    /* Be sure to indicate the operation start before possibly indicating
     * any headers or other events. The first time through here the start
     * event will be NO_EVENT. This allows us to put off indicating the
     * event at the moment the packet arrives. So that if there is a connId
     * header next, we will retrieve it before we indicate the start event.
     */
    AppCallback(ObexApp, OBSE_NO_EVENT);    /* FireCallback(ObexApp); */
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
    
    OSC(canFitOnePacket) = FALSE;

#ifdef __BT_GOEP_V2__
	if( OSC(srm_server) == OBSH_SRM_ENABLE && (ObServerGetOpcode() & OB_FINAL_BIT)){
		if ( OSC(currentOperation) == PUT_OPER_EVENTS ) {
			GOEP_Report("[SRM][SERVER] Receive final bit. disable SRM");
			OSC(srm_server) = OBSH_SRM_DISABLE;
		}
	}
#endif

    /* Process the event */
    if (OSC(currentOperation) != NO_OPER_EVENTS)
        ServerEventHandlerTable[OSC(currentOperation)](ObexApp, event);
    else 
        OSC(headerBlock) = HB_IDLE;

    if (event == OBPE_OPER_IND) {
        /* The OBEX packet has been completely received.
         * Time to send a response.
         */
#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
        OSC(sendResponse)++;
        //ASSERT(OSC(sendResponse) == 1);
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */

        /* Abort opcode always responds with success */
        if ((ObServerGetOpcode() & 0x7f) == OB_OPCODE_ABORT) {
            OSC(resp) = (OBRC_SUCCESS | OB_FINAL_BIT);
        }

#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
        /*
         * We allow the server to manage when the response packet is sent.
         * The server application must call OBEX_ServerSendResponse()
         * in response to this event. If the server doesn't know about the
         * operation, drop down to automatically send the response.
         */
        if (OSC(doCallback)) {
            OBD(serverParms).event = OBSE_SEND_RESPONSE;
            OSC(appCallback)(&OBD(serverParms));
            return;
        }
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */

        /* Send the response */
        OBEX_ServerSendResponse(ObexApp);
    }
}

#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            ProcessSession()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process Session operations.
 *
 * Return:    void
 */
void ProcessSession(ObexServerApp *ObexApp, ObexParserEvent event)
{
    U16                 len;
    ObexTpConnInfo      tpInfo;
    U8                 *headerBuff, sessId[16];
    BOOL                ok;

    ASSERT(event != OBPE_DISCONNECT);

    switch (event) {
    case OBPE_OPER_IND_START:
        ASSERT((ObServerGetOpcode() & 0x7F) == OB_OPCODE_SESSION);

        OSC(sessionFlags) = 0;

        if (!(ObServerGetOpcode() & OB_FINAL_BIT)) {     
            /* Session commands must have the final bit is set. */
            kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_EXPECT_FINALBIT);
            OSC(resp) = (OBRC_BAD_REQUEST | OB_FINAL_BIT);
            OSC(currentOperation) = NO_OPER_EVENTS;
            break;
        }

        OS_MemSet((U8 *)&OSH(sessionParms), 0, sizeof(ObexSessionParms));

        if (!OSC(rejectResp)) 
            OSC(resp) = (OBRC_SUCCESS | OB_FINAL_BIT);
        break;
    
    case OBPE_OPER_IND:
        switch (OSH(sessionParms).opcode) {
        case OB_SESSION_OPCODE_CREATE:
        case OB_SESSION_OPCODE_RESUME:
            /* Generate the OBSE_PROVIDE_SESSION event, assume failure. */
            OSC(resp) = (OBRC_SERVICE_UNAVAILABLE | OB_FINAL_BIT);
            OBD(serverParms).event = OBSE_PROVIDE_SESSION;
            OSC(appCallback)(&OBD(serverParms));
            break;

        case OB_SESSION_OPCODE_CLOSE:
            /* See if were closing the active session. */
            if (OSC(activeSession) == 0)
                break;

            OBEX_CreateSessionId(OSC(activeSession), sessId);
            if (OS_MemCmp(sessId, 16, OSH(sessionParms).sessionId, 16) == FALSE) {
                kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_RECEIVED_CLOSE_FOR_SESSION_OTHER_THAN_THE_ACTIVE_ONE);
                break;
            }
            /* Drop into next case */

        case OB_SESSION_OPCODE_SUSPEND:
            /* We are closing or suspending the active session.  */
            OSC(sessionFlags) |= OB_SESSION_DEACTIVATE;
            break;

        case OB_SESSION_OPCODE_SET_TIMEOUT:
            if ((OSH(sessionParms).fields & OBSP_FIELD_TIMEOUT) == 0) {
                /* Spec says: Assume infinite Timeout value. */
                OSH(sessionParms).fields |= OBSP_FIELD_TIMEOUT;
                OSH(sessionParms).timeout = 0xFFFFFFFF;
            }
            break;
        }
        break;
        
    case OBPE_BUILD_RESPONSE:
        /* No Session Parameters header in failed commands. */
        if (OSC(resp) != (OBRC_SUCCESS|OB_FINAL_BIT))
            break;

        /* No Session Parameters header in response to close commands. */
        if (OSH(sessionParms).opcode == OB_SESSION_OPCODE_CLOSE) 
            break;

        ASSERT(OSC(activeSession));     /* We must have an active session. */

        /* Update the timeout if one was provided. We do this here because we
         * need to wait until we've been provided with the session structure 
         * in the cases where the operation is a Create or Resume.
         */
        if (OSH(sessionParms).fields & OBSP_FIELD_TIMEOUT) {
            OSS(timeout) = min(OSH(sessionParms).timeout, OSS(ourTimeout));
        }

        /* We need information about our current transport */
        if (!OBEX_GetTpConnInfo(&ObexApp->handle, &tpInfo)) {
            break;
        }

        /* Prepare to build the Session Parameters header */
        headerBuff = OSC(protoTxBuff) + OSC(protoTxLen);
        len = 0;

        /* Build the OBEX Session Parameters Header Identifier */
        headerBuff[len++] = OBEXH_SESSION_PARAMS;
        len += 2;  /* Add the header length field (we haven't inserted it yet). */

        switch (OSH(sessionParms).opcode) {
        case OB_SESSION_OPCODE_CREATE: 
            /* New Session: Initialize our Device Address, Nonce and Session Id */
            /* Set the Transport Type for this session */
            OSS(tpType) = tpInfo.tpType;
            /* Copy the Device Address to the Server Session Context */
            OS_MemCopy(OSS(serverDevAddr), tpInfo.devAddr, (U16)GetDevAddrLen(OSS(tpType)));
            /* Create the Server Session Nonce for this session */
            ObCreateNonce(OSS(serverNonce));
            /* We use 16 byte Nonces */
            OSS(serverNonceLen) = 16; 
            /* NOTE: The Device Address and Nonce cannot change for this session, even 
             *       if the transport changes when the session is resumed.
             */

        case OB_SESSION_OPCODE_RESUME: 
            /* Device Address, Nonce, Session Id, Next Sequence Number, timeout (optional) */
            /* Device Address */
            headerBuff[len++] = SESSION_TAG_DEV_ADDR;
            headerBuff[len++] = GetDevAddrLen(OSS(tpType)); /* Length */
            /* Copy the Device Address based on the Created Session */
            OS_MemCopy(headerBuff+len, OSS(serverDevAddr), (U16)GetDevAddrLen(OSS(tpType)));
            len += (U16)GetDevAddrLen(OSS(tpType));
                
            /* Nonce */
            headerBuff[len++] = SESSION_TAG_NONCE;
            headerBuff[len++] = OSS(serverNonceLen); /* Length */
            /* Copy the already created Server Nonce */
            OS_MemCopy(headerBuff+len, OSS(serverNonce), OSS(serverNonceLen));
            len += OSS(serverNonceLen); /* Add the Nonce */
            headerBuff[len++] = SESSION_TAG_ID;
            headerBuff[len++] = 16; /* Length */
            /* Create the Server Session Id */
            OBEX_CreateSessionId(OSC(activeSession), headerBuff+len);
            len += 16;  /* Add the Session ID */
            if (OSH(sessionParms).opcode == OB_SESSION_OPCODE_RESUME) {
                /* Next Sequence Number */
                headerBuff[len++] = SESSION_TAG_SEQ_NUM;
                headerBuff[len++] = 0x01; /* Length */
                headerBuff[len++] = OSS(seqNum);
            }
    
        case OB_SESSION_OPCODE_SUSPEND:
        case OB_SESSION_OPCODE_SET_TIMEOUT:
            /* Timeout - non infinite values */
            headerBuff[len++] = SESSION_TAG_TIMEOUT;
            headerBuff[len++] = 4; /* Length */
            StoreBE32(headerBuff+len, OSS(timeout));    /* Sending negotiated value. */
            len += 4; /* Add the timeout */
            break;
        }

        /* Now fill in the length of the Session Params header, since we know it now. */
        StoreBE16(headerBuff+1, len);
        OSC(protoTxLen) += (U8)len;
        break;

    case OBPE_PUT_HEADER:
    case OBPE_WRITE_HEADER:
#if OBEX_SERVER_CONS_SIZE > 0
        if (ObServerGetHeaderType() == OBEXH_TARGET) {
            /* Associate a connection with the Target header (if one exists). */
            AssociateConn(ObexApp);
        }
#endif /* OBEX_SERVER_CONS_SIZE > 0 */

        if (ObServerGetHeaderType() == OBEXH_SESSION_PARAMS) {

            /* Parse the incoming Session Parameters */
            ok = ObexParseSessionParms( &ObexApp->handle, &OSH(sessionParms) );

            if (ok == FALSE) {
                OSC(resp) = (OBRC_FORBIDDEN|OB_FINAL_BIT);
                CancelOperation(ObexApp);
                break;
            }

            if (ObServerGetRemainHeaderLen() == 0) {
                /* We've received the entire header, indicate it to the applicaiton. */
                if (((OSH(sessionParms).fields & OBSP_FIELD_OPCODE) == 0) ||
                    ((OSH(sessionParms).fields & RX_SESSION_PARMS_IGNORE_MASK) !=
                      ObServRxSessionParmsTable[OSH(sessionParms).opcode])) {
                    /* Missing or unexpected session header parameter, fail the operation. */
                    OSC(resp) = (OBRC_FORBIDDEN|OB_FINAL_BIT);
                    CancelOperation(ObexApp);
                    break;
                }

                /* Now that we've retrieved the opcode from the session
                 * parameters header. Do some opcode validation.
                 */
                if (((OSH(sessionParms).opcode == OB_SESSION_OPCODE_CREATE) || 
                     (OSH(sessionParms).opcode == OB_SESSION_OPCODE_RESUME)) && 
                    OSC(activeSession)) {
                    /* Can't create or resume a session if an active one exists. */
                    OSC(resp) = (OBRC_FORBIDDEN | OB_FINAL_BIT);
                    OSC(currentOperation) = NO_OPER_EVENTS;
                    break;
                }

                if (((OSH(sessionParms).opcode == OB_SESSION_OPCODE_SET_TIMEOUT) ||
                     (OSH(sessionParms).opcode == OB_SESSION_OPCODE_SUSPEND)) && 
                    (OSC(activeSession) == 0)) {
                    /* Can't set the timeout or suspend if we have no active session. */
                    OSC(resp) = (OBRC_FORBIDDEN | OB_FINAL_BIT);
                    OSC(currentOperation) = NO_OPER_EVENTS;
                    break;
                }
                
                if (OSH(sessionParms).opcode > OB_SESSION_OPCODE_SET_TIMEOUT) {
                    /* We have no knowledge of this session opcode. */
                    OSC(resp) = (OBRC_NOT_IMPLEMENTED | OB_FINAL_BIT);
                    OSC(currentOperation) = NO_OPER_EVENTS;
                    break;
                }

                /* Generate a start event for the application */
                OBD(serverParms).opcode = 0x10 + OSH(sessionParms).opcode;
                AppCallback(ObexApp, OBSE_SESSION_START);

                /* Generate a Session Parameters event for the application. */
                OBD(serverParms).u.sessionParms = &OSH(sessionParms);
                AppCallback(ObexApp, OBSE_SESSION_PARMS_RX);
            }

        } else {
            /* Byte Sequence and Unicode are indicated to the application here. */
            ASSERT(OBEXH_IsByteSeq(ObServerGetHeaderType()) || 
                   OBEXH_IsUnicode(ObServerGetHeaderType()));

            AppCallback(ObexApp, OBSE_HEADER_RX);
        }
        break;
    }
    return;
}
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            ProcessGeneral()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process Connect/Disconnect/SetPath/Abort operations.
 *
 * Return:    void
 */
void ProcessGeneral(ObexServerApp *ObexApp, ObexParserEvent event)
{
    ObServerEvent appEvent = OBSE_NO_EVENT;

    ASSERT(event != OBPE_DISCONNECT);
#ifdef __BT_GOEP_V2__
    kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_PROCESS_GENERAL, event);
#endif
    switch (event) {
        case OBPE_OPER_IND_START:
     
            /* Verify that the final bit is set. */
            if (!(ObServerGetOpcode() & OB_FINAL_BIT)) {
        
                kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_EXPECT_FINALBIT,ObServerGetOpcode());
                OSC(resp) = (OBRC_BAD_REQUEST | OB_FINAL_BIT);
                OSC(currentOperation) = NO_OPER_EVENTS;
                break;
            }
            
            /* Generate a start event for the application */
            switch (ObServerGetOpcode()) {
                
                case OB_OPCODE_CONNECT|OB_FINAL_BIT:
                    appEvent = OBSE_CONNECT_START;
                    break;
            
                case OB_OPCODE_DISCONNECT|OB_FINAL_BIT:
                    appEvent = OBSE_DISCONNECT_START;
                    break;
                
                case OB_OPCODE_SET_PATH|OB_FINAL_BIT:   
                    OBD(serverParms).u.setPathFlags = ObServerGetPathFlags();
                    appEvent = OBSE_SET_PATH_START;
                    break;
                
                case OB_OPCODE_ABORT|OB_FINAL_BIT:
                    appEvent = OBSE_ABORT_START;
                    break;
                case OB_OPCODE_ACTION|OB_FINAL_BIT:   
                    appEvent = OBSE_ACTION_START;
                    break;
                default:
                    ASSERT(0);
            }
            if (appEvent != OBSE_NO_EVENT)
                AppCallback(ObexApp, appEvent);
            break;
    
        case OBPE_OPER_IND:
            OSC(currentOperation) = NO_OPER_EVENTS;
            break;
        
        case OBPE_PUT_HEADER:
        case OBPE_WRITE_HEADER:
#if OBEX_SERVER_CONS_SIZE > 0
            if (ObServerGetHeaderType() == OBEXH_TARGET) {
                /* Associate a connection with the Target header (if one exists). */
                AssociateConn(ObexApp);
            }
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
            /* Byte Sequence and Unicode are indicated to the application here. */
            ASSERT(OBEXH_IsByteSeq(ObServerGetHeaderType()) || 
                   OBEXH_IsUnicode(ObServerGetHeaderType()));

            AppCallback(ObexApp, OBSE_HEADER_RX);
            break;
    }
    return;
}

/*---------------------------------------------------------------------------
 *            ProcessPut()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process the put operation.
 *
 * Return:    void
 */
void ProcessPut(ObexServerApp *ObexApp, ObexParserEvent event)
{
    U16          len;
    U8          *buff;
    ObexRespCode rcode;

    ASSERT(OBSTACK_VerifyServerAppHandle(&ObexApp->handle));
#ifdef __BT_GOEP_V2__
    bt_trace(TRACE_FUNC, GOEP_SERVER_PROCESS_PUT_OPER, event);
#endif

    switch (event) {
        case OBPE_OPER_IND_START:
            if (!OSC(rejectResp)) 
                OSC(resp) = (OBRC_CONTINUE | OB_FINAL_BIT);
#ifdef __BT_GOEP_V2__
            OSC(objLen) = OSC(rxOffset) = 0; /* init the default objLen */
            OSC(flags) &= ~OBSF_LENGTH; /* remove the length header flag */
#endif
            AppCallback(ObexApp, OBSE_PUT_START);

            /* CR ALPS00129944 PUT-Abort and Delete */
            GOEP_Report("[OBEX] OSC(flags) is %d. Clear it ", OSC(flags));
            OSC(flags) &= ~(OBSF_BODYH | OBSF_ENDBODYH);
            break;

        case OBPE_OPER_IND:
            /* This packet is complete. See if the client is done. */
#ifdef __BT_GOEP_V2__
//	    SRM_Report("[OBEX][SERVER] ProcessPut OPER_IND %x", ObServerGetOpcode());
#endif
            if (ObServerGetOpcode() & OB_FINAL_BIT) {
                
                if (!OSC(rejectResp)) 
                    OSC(resp) = (OBRC_SUCCESS | OB_FINAL_BIT);
                
                /* By removing the handler now we protect ourselves
                 * from delivering an ABORT event after COMPLETE.
                 */
                OSC(currentOperation) = NO_OPER_EVENTS;
                if(ObexApp->goep_buff_ptr == NULL)
                {
                    if (OSC(obsh) == 0) 
                    {
                        /* Never got a body header: PUT-DELETE */
                        kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_PUTDELETE);
                        OBD(serverParms).event = OBSE_DELETE_OBJECT;
                        OSC(appCallback)(&OBD(serverParms));
                    }
                }
                else
                {
                    if ((ObexApp->goep_buff_wrote_len == 0) && (ObexApp->goep_first_packet_received == KAL_FALSE))
                    {
                        if (((OSC(flags) & OBSF_BODYH) == 0) && ((OSC(flags) & OBSF_ENDBODYH) == 0))
                        {
                            /* Never got a BODY or END-OF-BODY header: PUT-DELETE */
                            kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_PUTDELETE);
                        OBD(serverParms).event = OBSE_DELETE_OBJECT;
                        OSC(appCallback)(&OBD(serverParms));
                    }
                        else if ((OSC(flags) & OBSF_ENDBODYH))
						{
                            kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_CREATEEMPTY);
                            OBD(serverParms).event = OBSE_CREATE_EMPTY_OBJECT;
                            OSC(appCallback)(&OBD(serverParms));
						}
                    }
                }
                OSC(obsh) = 0;
                OSC(flags) &= ~(OBSF_BODYH | OBSF_ENDBODYH);
            }
            break;

        case OBPE_PUT_HEADER:
        case OBPE_WRITE_HEADER:
            if ((ObServerGetHeaderType() == OBEXH_BODY) ||
                (ObServerGetHeaderType() == OBEXH_END_BODY)) 
            {
                /* added by yfchu on 2007.5.11 */
                if (ObServerGetHeaderType() == OBEXH_BODY) 
                {
                    /* body header recieved */
                    //Report(("======OBEX::PUT with BODY header==="));
                    OSC(flags) |= OBSF_BODYH; 
                }
				else 
				{
					/* End-of-Body header */
                    //Report(("======OBEX::PUT with END-OF-BODY header==="));
                    OSC(flags) |= OBSF_ENDBODYH; 
                }
                
                if(ObexApp->goep_buff_ptr == NULL)
                {
                    if (OSC(obsh) == 0) {
                        OBD(serverParms).event = OBSE_PROVIDE_OBJECT;
                        OSC(appCallback)(&OBD(serverParms));
             
                        /* when appCallback return, it should have provided obsh*/
                        if (OSC(obsh) == 0) {
                            /* Check if App set Abort reason before we clobber it.*/
                            if (OSC(resp) == (OBRC_CONTINUE|OB_FINAL_BIT))
                            {
                                kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_NO_OBJECT);
                                OSC(resp) = (OBRC_NOT_FOUND|OB_FINAL_BIT);
                            }
                        
                            CancelOperation(ObexApp);
                            break;
                        }
                    }
                    ASSERT(OSC(obsh) != 0);
                }
                len = ObServerGetHeaderWriteLen();
                buff = ObServerGetHeaderBuff();

                /* Write the body. If we fail then report an error and call
                 * cancel, which clears the event handler so we will not be
                 * called again to write more data.
                 */
                if (len > 0) 
                {
#ifdef __BT_GOEP_V2__
                    OSC(rxOffset) += len;
#endif                    
                    if(ObexApp->goep_buff_ptr == NULL)
                    {
#ifdef __BT_GOEP_V2__
                        if( OSC(objLen) != 0 && OSC(rxOffset) >= OSC(objLen) ){
                            if( OSC(srm_server) == OBSH_SRM_ENABLE  ){
                                if ( (OSC(flags)&OBSF_LENGTH) != 0 ){
                                    if( OSC(rxOffset) >= OSC(objLen) ){
                                        kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_DISABLE_SRM_ENABLE, OSC(rxOffset), OSC(objLen));
                                    }
                                }
                            }
                        }
                    
                        if( len > ObexApp->handle.mru ){
                            /* Remote send too much data */
                            kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_REMOTE_SEND_LARGE_THEN_MTU, ObexApp->handle.mru, len);
                            OSC(resp) = (OBRC_NOT_ACCEPTABLE | OB_FINAL_BIT);
                            CancelOperation(ObexApp);
                            break;
                        }
#endif
                        if ((rcode = OSH(obsf)->Write(OSC(obsh), buff, len)) != OBRC_SUCCESS) 
                        {
                            kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_WRITE_OBJECT_FAILED, (int)rcode);
                            OSC(resp) = (ObexRespCode)(rcode | OB_FINAL_BIT);
                            CancelOperation(ObexApp);
                            break;
                        }
                    }
                    else
                    {
                        if(ObexApp->goep_buff_wrote_len + len > ObexApp->handle.mru)
                        {
                            // ASSERT(0);
                            /* Remote send too much data */
                            kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_REMOTE_SEND_LARGE_THEN_MTU, ObexApp->handle.mru, ObexApp->goep_buff_wrote_len + len );
                            OSC(resp) = (OBRC_NOT_ACCEPTABLE | OB_FINAL_BIT);
                            CancelOperation(ObexApp);
                            break;
                        }

                        /* copy header value to buff or raw header */
                        if( KAL_TRUE != ObexApp->goep_buff_rawheader ){
                        btmtk_os_memcpy(&(ObexApp->goep_buff_ptr[ObexApp->goep_buff_wrote_len]), buff, len);
                        ObexApp->goep_buff_wrote_len += len;/* remember to reset when new packet comes */
                        }else{
                            /* copy raw header */
                            
                        }
                    }        
#if OBEX_SESSION_SUPPORT == XA_ENABLED
                    if (OSC(activeSession)) {
                        /* Add the length written to the object store into our 
                         * current object offset.  This value will be added into
                         * the object offset (ackedObjBytes) when we receive confirmation
                         * that this data was entirely received and written. 
                         */
                        OSS(unackedObjBytes) += len;
                    }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
                }
                break;
            }
            /* Not a BODY or END-OF-BODY header */
#if OBEX_SERVER_CONS_SIZE > 0
            if (ObServerGetHeaderType() == OBEXH_TARGET) {
                /* Associate a connection with the Target header (if one exists). */
                AssociateConn(ObexApp);
            }
#endif /* OBEX_SERVER_CONS_SIZE > 0 */

#ifdef __BT_GOEP_V2__
            SRM_Report("[OBEX][H] type:0x%x len:%d value:%x", ObServerGetHeaderType(), ObServerGetHeaderLen(), ObServerGetHeaderInt32());
            if (ObServerGetHeaderType() == OBEXH_LENGTH) {
                OSC(objLen) = ObServerGetHeaderInt32();
				GOEP_Report("[OBEX] Receive a Length header objLen:%d", OSC(objLen) );
                OSC(flags) |= OBSF_LENGTH;
            }            

            if( OSC(srm_server) == OBSH_SRM_ENABLE && (OSC(flags)&OBSF_ENDBODYH) != 0){
                kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_DISABLE_SRM_ENABLE_ENDBODY);
                OSC(srm_server) = 0;
            }
#endif
            /* Byte Sequence and Unicode are indicated to the application here. */
            ASSERT(OBEXH_IsByteSeq(ObServerGetHeaderType()) || 
                   OBEXH_IsUnicode(ObServerGetHeaderType()));

            AppCallback(ObexApp, OBSE_HEADER_RX);
            break;
    }
}


/*---------------------------------------------------------------------------
 *            ProcessGet()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process the get operation.
 *
 * Return:    void
 */
void ProcessGet(ObexServerApp *ObexApp, ObexParserEvent event)
{
    U16 appHeaderLen = 0;
    U32 txTotalLen = 0;
    U16 maxTxLen = 0;
    
    ASSERT(event != OBPE_DISCONNECT);
    ASSERT(OBSTACK_VerifyServerAppHandle(&ObexApp->handle));
#ifdef __BT_GOEP_V2__
    kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_PROCESS_GET, ObexApp, event);
#endif
    /* Process the event */
    switch (event) {
        case OBPE_OPER_IND_START:
            if (!OSC(rejectResp)) 
                OSC(resp) = (OBRC_CONTINUE | OB_FINAL_BIT);
            OSC(objLen) = OSC(rxOffset) = 0; /* init the default objLen */
            AppCallback(ObexApp, OBSE_GET_START);
#if OBEX_BODYLESS_GET == XA_ENABLED
            OSC(obshAllowEmpty) = FALSE;
#endif /* OBEX_BODYLESS_GET == XA_ENABLED */
            break;

        case OBPE_OPER_IND:
            /* Things are going ok.  */
            if(ObexApp->goep_buff_ptr == NULL)
            {
            if (ObServerGetOpcode() & OB_FINAL_BIT) {
                /* This is the last packet of the request sequence. */
                    if (OSC(obsh) == 0) {
                        /* Request the object store handle from the application. */
                        OBD(serverParms).event = OBSE_PROVIDE_OBJECT;
                        OSC(appCallback)(&OBD(serverParms));
                        break;
                    }

                    ASSERT(OSC(obsh));
            } else {
                /* If the final bit is not set we need to return  the default
                 * (OBRC_CONTINUE | OB_FINAL_BIT). However, due to an error
                 * in the OBEX v1.0 Spec, the GETs to pull the object body
                 * may not have the final bit set. If this is the case,
                 * patch the opcode by adding the final bit.
                 */
                    if (OSC(obsh) != 0) {
                        ObServerGetOpcode() |= OB_FINAL_BIT;
                    }
                }
            }
            else
            {
                if (ObServerGetOpcode() & OB_FINAL_BIT) 
                {
                    /* This is the last packet of the request sequence. */
                } 
                else
                {
                    /* If the final bit is not set we need to return  the default
                    * (OBRC_CONTINUE | OB_FINAL_BIT). However, due to an error
                    * in the OBEX v1.0 Spec, the GETs to pull the object body
                    * may not have the final bit set. If this is the case,
                    * patch the opcode by adding the final bit.
                    */
                    if (OSC(objLen) == 0) 
                    {
                    ObServerGetOpcode() |= OB_FINAL_BIT;
                }
            }
            }
            break;

        case OBPE_BUILD_RESPONSE:
            /* Don't run this code until the client is ready for us to 
             * start responding with data.
             */
            if ((ObServerGetOpcode() & OB_FINAL_BIT) == 0) 
                break;

#if OBEX_BODYLESS_GET == XA_ENABLED
            /* In the case of an allowed empty body go straight to success */
            if (OSC(obshAllowEmpty)) {
                OSC(resp) = (OBRC_SUCCESS | OB_FINAL_BIT);
            } else
#endif /* OBEX_BODYLESS_GET == XA_ENABLED */                
            if(ObexApp->goep_buff_ptr == NULL)
            {
                if (OSC(obsh) == 0) {
                    /* If we don't have an object store handle abort. */
                    kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_NO_OBJECT);
                    CancelOperation(ObexApp);
                    OSC(resp) = (OBRC_NOT_FOUND | OB_FINAL_BIT);
                    break;
                }
            }

            OSC(canFitOnePacket) = FALSE;

#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
            if ((appHeaderLen = (OSH(txLength) + OSC(protoTxLen))) > 0) {
#else
            if ((appHeaderLen = OSH(txLength)) > 0) {
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
                /* The application provided some headers,
                 * break to avoid going into sendBody. 
                 */

                /* For PTS issue 6361-P workaround */
                if (IS_MAP_SERVER(ObexApp))
                {
                    if (OSC(objLen))
                    {
                        ObexAppHandle *AppHndl = &ObexApp->handle;
                        AppHndl->buffer[AppHndl->txLength++] = OBEXH_BODY;
                        AppHndl->buffer[AppHndl->txLength++] = 0x00;
                        AppHndl->buffer[AppHndl->txLength++] = 0x03;
                    }
                }

               /* revised for PBAP server */
               if (IS_PBAP_SERVER(ObexApp))
               {
                    txTotalLen = appHeaderLen + OSC(objLen) + 3;

                    if(ObexApp->goep_buff_ptr == NULL)
                    {                       
                        maxTxLen = ObServerMaxHeaderBlockSize();
                    }
                    else
                    {
                        maxTxLen = ObexApp->goep_buff_size;
                    }

					kal_trace(BT_TRACE_G6_OBEX,OBEX_PBAP_PARAMS,appHeaderLen,OSC(objLen), txTotalLen,maxTxLen);
						
                    if (txTotalLen < (U32)maxTxLen)
                    {
                        /* can fit one packet */
                        OSC(canFitOnePacket) = TRUE;
                        kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_CANFITONEPACKET, txTotalLen, maxTxLen);
                   }
               }

                break;
            }

            /* We have already been provided the object so this
             * is a get packet with the final bit set being used
             * to pull over the body of the object.
             */
            if (OSC(objLen) == 0) {
                /* This is the last packet, we have no more data
                 * to send so tell the client that we are done.
                 */
                OSC(obsh) = 0;
                OSC(currentOperation) = NO_OPER_EVENTS;
                    
                OSC(txLength) = 3;
                if (!OSC(rejectResp)) 
					OSC(resp) = (OBRC_SUCCESS | OB_FINAL_BIT);
            } else {
                /* Need to send a packet */
                if(ObexApp->goep_buff_ptr == NULL)
                {
                    OSC(txLength) = (U16) min((OSC(objLen) + 3),
                                          (U32)ObServerMaxHeaderBlockSize());
                }
                else
                {
                    OSC(txLength) = (U16) min((OSC(objLen) + 3),
                                          (U32)ObexApp->goep_buff_size + 3);
                }
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
                if (OSC(objLen) == UNKNOWN_OBJECT_LENGTH)
                    OSC(txLength) = 0;
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */
#if OBEX_GET_USE_END_OF_BODY == XA_ENABLED
                if ((OSC(objLen) + 3) <= OSC(txLength)) {
                    /* We can fit the rest of the object data into the last packet */
                    if (!OSC(rejectResp)) 
                        OSC(resp) = (OBRC_SUCCESS | OB_FINAL_BIT);
                }
#endif /* OBEX_GET_USE_END_OF_BODY == XA_ENABLED */
            }
            kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_PROCGET_SENDOBJ,OSC(objLen), OSC(txLength), OSC(resp));
            OSC(headerBlock) = HB_SERVER_OBJECT;
            break;

        case OBPE_PUT_HEADER:
        case OBPE_WRITE_HEADER:
#if OBEX_SERVER_CONS_SIZE > 0
            if (ObServerGetHeaderType() == OBEXH_TARGET) {
                /* Associate a connection with the Target header (if one exists). */
                AssociateConn(ObexApp);
            }
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
            /* Byte Sequence and Unicode are indicated to the application here. */
            ASSERT(OBEXH_IsByteSeq(ObServerGetHeaderType()) || 
                   OBEXH_IsUnicode(ObServerGetHeaderType()));

            AppCallback(ObexApp, OBSE_HEADER_RX);
            break;
#ifdef __BT_GOEP_V2__
        case OBPE_TX_COMPLETE:
            /* if srm_mode , callabck */
            if( ObexApp->server.srm_server == OBSH_SRM_ENABLE ){
				OSC(sendResponse)++; // Need this to send response
                
                AppCallback(ObexApp, OBSE_SEND_RESPONSE);    
            }else if( ObexApp->server.srm_server != 0 ){
                SRM_Report("[SRM] Tx_COMPLETE wait ObexApp:0x%x srm_server:%d", ObexApp, ObexApp->server.srm_server);            
            }

            /* calculate the SRM MODE */
			if( OSC(srm_server) == OBSH_SRM_DISABLE ){
				SRM_Report("[SRM] check server srm_server:%d srm_flag:0x%x", OSC(srm_server), OSC(srm_flags) );
				if( (OSC(srm_flags)&(OBSF_CLIENT_ENABLE|OBSF_SERVER_ENABLE)) == (OBSF_CLIENT_ENABLE|OBSF_SERVER_ENABLE)){
					SRM_Report("[SRM] enter srm mode");
					OSC(srm_server) = OBSH_SRM_ENABLE;

					if( (OSC(srm_flags)&(OBSF_CLIENT_WAITING|OBSF_SERVER_WAITING)) != 0 ){
						SRM_Report("[SRM] enter srm wait mode");
						OSC(srm_server) = OBSH_SRM_WAITENABLE;
					}else{
						/* first into srm mode */
						SRM_Report("[SRM] first enter srm wait mode. call send_response");
						/* Note! need incress the sendResponse count */
						OSC(sendResponse)++;
						AppCallback(ObexApp, OBSE_SEND_RESPONSE);
					}
				}
			}else if( OSC(srm_server) == OBSH_SRM_WAITENABLE ){
				SRM_Report("[SRM] srmwait check server srm_server:%d srm_flag:0x%x", OSC(srm_server), OSC(srm_flags));
				if( (OSC(srm_flags)&(OBSF_CLIENT_WAITING|OBSF_SERVER_WAITING)) == 0 ){
					SRM_Report("[SRM] srmwait enter srm wait mode");
					OSC(srm_server) = OBSH_SRM_ENABLE;

					OSC(sendResponse)++;
					AppCallback(ObexApp, OBSE_SEND_RESPONSE);
				}
				

			}
				
			    /* remove client's wait flag, it only works on one req/rsp */
			    if( (OSC(srm_flags)&(OBSF_CLIENT_WAITING)) != 0 ){
			        SRM_Report("[SRM] remove client's wait orig:0x%x after:0x%x", OSC(srm_flags) , OSC(srm_flags)&(~OBSF_CLIENT_WAITING));
			        OSC(srm_flags) = OSC(srm_flags)&(~OBSF_CLIENT_WAITING);
			    }
			break;
#endif
    }
    return;
             
}


U16 ObServerGetHeaderBlockLen(ObexServerApp *ObexApp, ObHeaderBlock hb)
{
     U16 len = 0;
     
#if OBEX_ROLE_SERVER == XA_ENABLED
#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
     len = (OSC(txLength) + OSC(protoTxLen));
#else /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
     len = (OSC(txLength));
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
#else /* OBEX_ROLE_SERVER == XA_ENABLED */
     len = 0;
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

    kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_GET_HB_LEN, len);

     return len;
}

/*---------------------------------------------------------------------------
 *            ObServerReadHeaderBlock()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Read the server header block.
 *
 * Return:    void
 */
ObexRespCode ObServerReadHeaderBlock(ObexServerApp *ObexApp, ObHeaderBlock hb, U8* buff, U16 len)
{
    ObexRespCode    rcode = OBRC_SUCCESS;
#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
    U16             len2;
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */

    kal_trace(BT_TRACE_G6_OBEX, OBEX_SERVER_READ_HB, ObexApp, hb, len, ObServerGetResponse());

    ASSERT(OBSTACK_VerifyServerAppHandle(&ObexApp->handle));
    ASSERT((hb == HB_SERVER_OBJECT) || (hb == HB_SERVER_HEADERS));

    if (hb == HB_SERVER_HEADERS) {
        /* Send response headers */
#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
        /* Copy any connection/session headers first */
        if (OSC(protoTxLen)) {

            len2 = min(len, OSC(protoTxLen));
            OS_MemCopy(buff, OSC(protoTxBuff) + OSC(txOffset), len2);

            OSC(txOffset) += len2;
            OSC(protoTxLen) -= len2;
            
            if (OSC(protoTxLen) > 0) {
                return rcode;
            }
            
            len -= len2;
            buff += len2;
            
            OSC(txOffset) = 0;
        }
#else  /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
        ASSERT(OSC(txLength) > 0);
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */

        /* Copy the applications headers into the request packet */
        len = min(len, OSC(txLength));
        OS_MemCopy(buff, OSH(buffer) + OSC(txOffset), len);
        
        OSC(txOffset) += len;
        OSC(txLength) -= len;
        return rcode;
    }

    /* Read data from the disk into the buffer (GET Operation) */
    if (OSC(txOffset) == 0) {
        ASSERT(len >= 3);
        /* This is the beginning of the packet.
         * Append the data with the OBEX Body Header.
         */
        if ((ObServerGetResponse() == (OBRC_SUCCESS | OB_FINAL_BIT))
            || (ObServerGetResponse() == (OBRC_PARTIAL_CONTENT| OB_FINAL_BIT))) 
        {
            buff[0] = OBEXH_END_BODY;   /* This is the last packet. */
        } else {
            buff[0] = OBEXH_BODY;
        }
        /* Put the size of the BODY header into the packet */
        buff[1] = (U8) (OSC(txLength) >> 8);
        buff[2] = (U8) OSC(txLength);

        OSC(txLength) -= 3;
        buff += 3;
        OSC(txOffset) += 3;
        len -= 3;
    }
    
    /* Sometimes the protocol writes an empty EOB header in which case
     * there is no data to read.
     */
    if (len > 0) {
        /* If the operation is aborted, more data may be queued for reading
         * before the abort is returned to the client. The obsh handle is
         * cleared to prevent reading more data after an abort is initiated.
         */
            ASSERT(OSC(txLength) > 0);

            len = min(len, OSC(txLength));
                
            /* Read the data */
        if(ObexApp->goep_buff_ptr == NULL)
        {
            if (OSC(obsh)) {
                if ((rcode = OSH(obsf)->Read(OSC(obsh), buff, len)) != OBRC_SUCCESS) {
					kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_READ_HEADERBLOCK_ERROR, (int)rcode);
                    OSC(resp) = (ObexRespCode)(rcode | OB_FINAL_BIT);
                    CancelOperation(ObexApp);
                }
#if OBEX_SESSION_SUPPORT == XA_ENABLED
                else if (OSC(activeSession)) {
                    /* Add the length read from the object store into our 
                    * current object offset.  This value will be added into
                    * the object offset (ackedObjBytes) when we receive confirmation
                    * that this data was sent and received by the remote device 
                    */
                    OSS(unackedObjBytes) += len;
                }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

                OSC(rxOffset) += len;
            }
        }
        else
        {
            OS_MemCopy(buff, ObexApp->goep_buff_ptr + ObexApp->goep_buff_wrote_len, len);
            ObexApp->goep_buff_wrote_len += len;
        }
    }

    /* No matter what act as if the data was consumed */
    OSC(txLength) -= len;
    OSC(objLen) -= len;
	
#if OBEX_GET_USE_END_OF_BODY == XA_ENABLED
	if (OSC(objLen) == 0) {
	    OSC(obsh) = 0;
	    ObexApp->goep_buff_wrote_len = 0;
	    ObexApp->goep_first_packet_received = KAL_FALSE;
        OSC(currentOperation) = NO_OPER_EVENTS;
#ifdef __BT_GOEP_V2__
        GOEP_Report("[OBEX] ObServerReadHeaderBlock first_packet_received:%d", ObexApp->goep_first_packet_received);                            
#endif
	}
#endif /* OBEX_GET_USE_END_OF_BODY == XA_ENABLED */

    return rcode;
}

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            ObServerReadHeaderBlockFlex()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Read the server header block.
 *
 * Return:    U16 - number of bytes read into buff (includes header)
 */
U16 ObServerReadHeaderBlockFlex(ObexServerApp *ObexApp, U8 *buff, U16 len)
{
    ObexRespCode    rcode;
    BOOL            more = TRUE;

    ASSERT(OBSTACK_VerifyServerAppHandle(&ObexApp->handle));
    ASSERT(OSC(headerBlock) == HB_SERVER_OBJECT);
    ASSERT(len > 0);

    /* Save space for the body header identifier */
    len -= 3;

    /* The object is of unknown length. We can never span the body across
     * multiple packets so we know that this packet has the body header. 
     */
    rcode = OSH(obsf)->ReadFlex(OSC(obsh), buff+3, &len, &more);
    if (rcode != OBRC_SUCCESS) {
        OSC(resp) = (ObexRespCode)(rcode | OB_FINAL_BIT);
        CancelOperation(ObexApp);
    }
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    else if (OSC(activeSession)) {
        /* Add the length read from the object store into our 
         * current object offset.  This value will be added into
         * the object offset (ackedObjBytes) when we receive confirmation
         * that this data was sent and received by the remote device 
         */
        OSS(unackedObjBytes) += len;
    }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
    len += 3;

    if (!more) {
        /* Were done! */
        OSC(objLen) = 0;
#if OBEX_GET_USE_END_OF_BODY == XA_ENABLED
        OSC(obsh) = 0;
        OSC(currentOperation) = NO_OPER_EVENTS;
		/* We have already passed the response code to the OBEX parser, but
		 * we now know that we have the last portion of data.  The OBEX 
		 * parser will detect that OSC(resp) has changed and will pass
		 * along the new response code along with the End Of Body header.
		 */
        if (!OSC(rejectResp))
            OSC(resp) = (OBRC_SUCCESS | OB_FINAL_BIT);
	 
		/*
		 * If were done, give the application a chance to complete the request
		 * and abort if it encounters an error. This is only necessary for
		 * successfully completing Get operations.
         * Don't generate PRECOMPLETE if we are retransmitting this response.
         */
		if ((OSC(resp) == (OBRC_SUCCESS|OB_FINAL_BIT)) &&
#if OBEX_SESSION_SUPPORT == XA_ENABLED
			(OSC(seqNumError) != ERR_RETRANSMIT_AND_IGNORE) &&
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
			((ObServerGetOpcode() & 0x7f) == OB_OPCODE_GET)) {
			OBD(serverParms).event = OBSE_PRECOMPLETE;
			OSC(appCallback)(&OBD(serverParms));
		}
#endif /* OBEX_GET_USE_END_OF_BODY == XA_ENABLED */
    }

    /* Write the BODY header identifier */
#if OBEX_GET_USE_END_OF_BODY == XA_ENABLED
	if (OSC(objLen) == 0) {
	    buff[0] = OBEXH_END_BODY;
	} else
#endif /* OBEX_GET_USE_END_OF_BODY == XA_ENABLED */
    {
		buff[0] = OBEXH_BODY;
	}
    buff[1] = (U8)(len >> 8);
    buff[2] = (U8)(len);

    return len;
}
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *            CancelOperation()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Stops processing of the current operation.
 *
 * Return:    void
 */
static void CancelOperation(ObexServerApp *ObexApp)
{
    OSC(obsh) = 0;
    ObexApp->goep_buff_wrote_len = 0;
    ObexApp->goep_first_packet_received = KAL_FALSE;
#ifdef __BT_GOEP_V2__
    GOEP_Report("[OBEX] CancelOperation first_packet_received:%d", ObexApp->goep_first_packet_received);                            
#endif    
    OSC(currentOperation) = NO_OPER_EVENTS;

#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
    /* Avoid sending any positive acknowledgement of a directed connection. */
    OSC(protoTxLen) = 0;
    /* Clear the pending indication if on exists */
    OSC(startEvent) = OBSE_NO_EVENT;
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
}


/*---------------------------------------------------------------------------
 *            AppCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Delivers events to the application.
 *
 * Return:    TRUE -  Event was delivered and the operation to be aborted.
 *            FALSE - No event was delivered, the operation is still active.
 */
static BOOL AppCallback(ObexServerApp *ObexApp, ObServerEvent Event)
{
    /* This should already have been set. */
    ASSERT(OBD(serverParms).server == ObexApp);

#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
    /* If we're holding a start event, indicate it before we proceed. */
    if (OSC(startEvent) != OBSE_NO_EVENT) {
        OSC(doCallback) = TRUE;
        OBD(serverParms).event = OSC(startEvent);

        OSC(startEvent) = OBSE_NO_EVENT;
        OSC(appCallback)(&OBD(serverParms));
    }
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */

    switch (Event) {
    /* Setup callback parameters for Header Rx events. */
    case OBSE_HEADER_RX:
        switch (ObServerGetHeaderType() & 0xC0) {
        case 0x00:  /* UNICODE */
        case 0x40:  /* Byte-Sequence */
            OBD(serverParms).u.headerRx.totalLen = ObServerGetHeaderLen();
            OBD(serverParms).u.headerRx.buff = ObServerGetHeaderBuff();
            OBD(serverParms).u.headerRx.remainLen = ObServerGetRemainHeaderLen();
            OBD(serverParms).u.headerRx.currLen = ObServerGetHeaderWriteLen();
            break;
        case 0xC0:  /* 4-byte */
            OBD(serverParms).u.headerRx.value = ObServerGetHeaderInt32();
            break;
        case 0x80:  /* 1-byte */
            OBD(serverParms).u.headerRx.value = ObServerGetHeaderByte();
            break;
        }
        OBD(serverParms).u.headerRx.type = ObServerGetHeaderType();
        break;

    case OBSE_PUT_START:
    case OBSE_GET_START:
#ifdef __BT_GOEP_V2__
	kal_trace(BT_TRACE_G6_OBEX, OBEX_RESET_SRM_ENABLE, ObexApp);
        OSC(srm_server) = OBSH_SRM_DISABLE;
#endif
    case OBSE_SET_PATH_START:
    case OBSE_ACTION_START:        
    case OBSE_CONNECT_START:
    case OBSE_DISCONNECT_START:
    case OBSE_ABORT_START:
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    case OBSE_SESSION_START:
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
        /*
         * If server connections are enabled, the server defers the start event
         * notification in anticipation of getting a ConnId header. That way it
         * knows what connection the operation is for when it notifies the server.
         * OBEX Session support does the same thing, waiting for the sequence
         * number header.
         */
        OSC(startEvent) = Event;
        Event = OBSE_NO_EVENT;
#else /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
        OSC(doCallback) = TRUE;
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
        break;
    }

    /* Deliver the current event. */
    if (Event != OBSE_NO_EVENT) {
        ASSERT(OSC(doCallback) == TRUE);

        OBD(serverParms).event = Event;
        OSC(appCallback)(&OBD(serverParms));
    }

    return OSC(currentOperation) == NO_OPER_EVENTS;
}


#if (OBEX_SERVER_CONS_SIZE > 0)
/*---------------------------------------------------------------------------
 *            CheckforConnId()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Checks incoming packet for a ConnId header. If a valid one 
 *            is found the connection is marked active. If the value is
 *            invalid, the operation is rejected
 *
 * Return:    TRUE -  ConnId was Invalid so the operation was aborted.
 *            FALSE - Valid ConnId or no ConnId header.
 */
static BOOL CheckforConnId(ObexServerApp *ObexApp)
{
    /* Process Connection Id header */
    if (ObServerGetHeaderType() == OBEXH_CONNID) {

        /* If this check passes, the client sent us multiple ConnId
         * headers or a Target & ConnId header, both of which are
         * protocol violations. However, we only disconnect the link
         * if the ConnId doesn't match the current one.
         */
        if (OSC(activeConnId) != OBEX_INVALID_CONNID) {
            if (OSC(activeConnId) == (ObServerGetHeaderInt32()-7)) {
                return FALSE;
            }
            CancelOperation(ObexApp);
            kal_trace(BT_TRACE_G6_OBEX, OBEX_CONN_ID_NOT_VALIED);
            ObServerDisconnect();
            return TRUE;
        }

        OSC(activeConnId) = ObServerGetHeaderInt32() - 7;
        /* Validate connection: Id within range & registered */
        if ((OSC(activeConnId) >= OBEX_SERVER_CONS_SIZE) ||
            (OSC(conn[OSC(activeConnId)]) == 0)) {
            /* Invalid connection, reject operation */
            kal_trace(BT_TRACE_G6_OBEX,OBEX_INVALID_CONNECTIONID, OSC(activeConnId));

            CancelOperation(ObexApp);
            OSC(resp) = (OBRC_SERVICE_UNAVAILABLE | OB_FINAL_BIT);
            OSC(activeConnId) = OBEX_INVALID_CONNID;
            return TRUE;
        }
        /* Mark conn info to indicate received connid header */
        OSC(conn[OSC(activeConnId)])->connId = OSC(activeConnId)+7;
    }
    return FALSE;
}


/*---------------------------------------------------------------------------
 *            AssociateConn()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Attempts to locate a local service with a target UUID that
 *            matches the received Target Header. If a match is made, a
 *            connection id is assigned and the response headers are built.
 *
 *            We stage the Target header in the same place as our outgoing
 *            protocol Tx buffer holds the Who header. This saves RAM and
 *            makes target matching simpler.
 *            
 *            We allow one active connection per target. So if the matching
 *            target is already connected, we do not connect to it.
 *
 * Return:    void
 */
static void AssociateConn(ObexServerApp *ObexApp)
{
    U8  i;

    /* If we've already assigned a connection, ignore remaining headers */
    if (OSC(activeConnId) != OBEX_INVALID_CONNID)
        return;

    /* If the target header is > our max it definitely won't match. */
    if (ObServerGetHeaderLen() > OBEX_MAX_SERVICE_UUID_SIZE)
        return;

    OS_MemCopy(OSC(protoTxBuff)+OSC(protoTxLen), ObServerGetHeaderBuff(),
               ObServerGetHeaderWriteLen());
  
    OSC(protoTxLen) += (U8)ObServerGetHeaderWriteLen();

    /* If we haven't we staged the entire buffer yet, return. */
    if (OSC(protoTxLen) != ObServerGetHeaderLen())
        return;
    
    /* Find a matching Target UUID */
    for (i = 0; i < OBEX_SERVER_CONS_SIZE; i++) {
        if (OSC(conn[i]) == 0)
            continue;

        if (StrnCmp(OSC(protoTxBuff), OSC(protoTxLen),
                    OSC(conn[i])->target, OSC(conn[i])->targetLen)) {
                    
            /* Headers match! associate the connection */

            /* If were processing a Connect packet, mark the connection
             * to indicate that the packet arrived on a targeted connection.
             * Otherwise mark it as a targeted operation. Also, send a 
             * ConnId header back to the client.
             */

            if (ObServerGetOpcode() == (OB_OPCODE_CONNECT|OB_FINAL_BIT)) {
                OSC(conn[i])->connId = i+7;
                
                OSC(protoTxBuff)[0] = OBEXH_CONNID;
                OSC(protoTxBuff)[1] = 0;
                OSC(protoTxBuff)[2] = 0;
                OSC(protoTxBuff)[3] = 0;
                OSC(protoTxBuff)[4] = i+7;
                OSC(protoTxLen) = 5;
            }
            else {
                OSC(conn[i])->connId = OBEX_INVALID_CONNID;
                OSC(protoTxLen) = 0;
            }

            /* Mark connection active */
            OSC(activeConnId) = i;

            /* Build the WHO header: The data is already in place,
             * just attach header id. 
             */
            OSC(protoTxBuff)[OSC(protoTxLen)+0] = OBEXH_WHO;  
            OSC(protoTxBuff)[OSC(protoTxLen)+1] = (U8)((OSC(conn[i])->targetLen+3) >> 8);
            OSC(protoTxBuff)[OSC(protoTxLen)+2] = (U8)(OSC(conn[i])->targetLen+3);
            OS_MemCopy(OSC(protoTxBuff) + OSC(protoTxLen) + 3, 
                       OSC(conn)[i]->target, OSC(conn)[i]->targetLen);
            OSC(protoTxLen) += 3 + OSC(conn)[i]->targetLen;

            return;
        }
    }
    /* No match found. */
    OSC(protoTxLen) = 0;
}


/* revised for PBAP server */

U16 ObServerGetHeaderBlockLen_FitOnePkt(ObexServerApp *ObexApp, ObHeaderBlock hb )
{
     U16 block_len = 0;
     U16 overhead = 3 ; /* END-OF_BODY + Length */
     
#if OBEX_ROLE_SERVER == XA_ENABLED
#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
     block_len = (OSC(txLength) + OSC(protoTxLen));
#else /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
     block_len = (OSC(txLength));
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
#else /* OBEX_ROLE_SERVER == XA_ENABLED */
     block_len = 0;
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

     if ((OSC(currentOperation) == GET_OPER_EVENTS) && (OSC(canFitOnePacket))) 
     {
          block_len += (U16)(OSC(objLen) + overhead);
     }

     kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_GET_HB_LEN_FOP, OSC(currentOperation),OSC(canFitOnePacket), block_len);

     return block_len;
}


/*---------------------------------------------------------------------------
 *            ObServerReadHeaderBlock_FitOnePkt()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Read the server header block.
 *
 * Return:    void
 */
ObexRespCode ObServerReadHeaderBlock_FitOnePkt(ObexServerApp *ObexApp, ObHeaderBlock hb, U8* buff, U16 len)
{
    ObexRespCode    rcode = OBRC_SUCCESS;
    U16 len3;
#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
    U16             len2;
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */

    kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_READ_HEADERBLOCK_FITONEPKT,OSC(currentOperation),OSC(canFitOnePacket),hb,len);

    if ((OSC(currentOperation) != GET_OPER_EVENTS) || !OSC(canFitOnePacket))
    {
        rcode = ObServerReadHeaderBlock(ObexApp, hb, buff, len);
        return rcode;
    }

    /* the following code is only for can-fit-one-packet GET response */

    ASSERT(OBSTACK_VerifyServerAppHandle(&ObexApp->handle));
    ASSERT((hb == HB_SERVER_OBJECT) || (hb == HB_SERVER_HEADERS));

    if (hb == HB_SERVER_HEADERS) {
        /* Send response headers */
#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
        /* Copy any connection/session headers first */
        if (OSC(protoTxLen)) {

            len2 = min(len, OSC(protoTxLen));
            OS_MemCopy(buff, OSC(protoTxBuff) + OSC(txOffset), len2);

            OSC(txOffset) += len2;
            OSC(protoTxLen) -= len2;
/*            
            if (OSC(protoTxLen) > 0) {
                Report(("[OBEX]----ObServerReadHeaderBlock_FitOnePkt...OSC(protoTxLen) > 0, return!"));
                return rcode;
            }
*/            
            len -= len2;
            buff += len2;
            
            OSC(txOffset) = 0;
        }
#else  /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */
        ASSERT(OSC(txLength) > 0);
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */

        /* Copy the applications headers into the request packet */
        len3 = min(len, OSC(txLength));
        OS_MemCopy(buff, OSH(buffer) + OSC(txOffset), len3);
        
        OSC(txOffset) += len3;
        OSC(txLength) -= len3;

        len -= len3;
        buff += len3;
    }
    
    OSC(headerBlock) = HB_SERVER_OBJECT;
    OSH(parser).handle = OSC(headerBlock);
     
    /* Read data from the disk into the buffer (GET Operation) */
    {
        U16 EOB_len = (U16) OSC(objLen) + 3;

        ASSERT(len >= 3);
        /* 
         * Append the data with the OBEX End-Of-Body Header.
         */
        kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_READ_HEADERBLOCK_FITONEPKT_FILL_EOB);
         
        OSC(resp) = (OBRC_SUCCESS | OB_FINAL_BIT);
        buff[0] = OBEXH_END_BODY;   /* This is the last packet. */

        /* Put the size of the BODY header into the packet */
        buff[1] = (U8) (EOB_len >> 8);
        buff[2] = (U8) EOB_len;

		if (OSC(txLength) > 0)
            OSC(txLength) -= 3;

        buff += 3;
        OSC(txOffset) += 3;
        len -= 3;
    }

    if (OSC(txLength) == 0)
    {
	// Since the obj could be fit in a single packet. It should be less then a U16 range
        OSC(txLength) = (U16) OSC(objLen); 
    }
    
    /* Sometimes the protocol writes an empty EOB header in which case
     * there is no data to read.
     */
    if (len > 0) {
        /* If the operation is aborted, more data may be queued for reading
         * before the abort is returned to the client. The obsh handle is
         * cleared to prevent reading more data after an abort is initiated.
         */
            ASSERT(OSC(txLength) > 0);
            ASSERT(OSC(objLen) > 0);

		    len = min(len, (U16)OSC(objLen));
                
            /* Read the data */
        if(ObexApp->goep_buff_ptr == NULL)
        {
            if (OSC(obsh)) {
                if ((rcode = OSH(obsf)->Read(OSC(obsh), buff, len)) != OBRC_SUCCESS) {
					kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_READ_HEADERBLOCK_ERROR, (int)rcode);
                    OSC(resp) = (ObexRespCode)(rcode | OB_FINAL_BIT);
                    CancelOperation(ObexApp);
                }
#if OBEX_SESSION_SUPPORT == XA_ENABLED
                else if (OSC(activeSession)) {
                    /* Add the length read from the object store into our 
                    * current object offset.  This value will be added into
                    * the object offset (ackedObjBytes) when we receive confirmation
                    * that this data was sent and received by the remote device 
                    */
                    OSS(unackedObjBytes) += len;
                }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
            }
        }
        else
        {
            OS_MemCopy(buff, ObexApp->goep_buff_ptr + ObexApp->goep_buff_wrote_len, len);
            ObexApp->goep_buff_wrote_len += len;
        }
    }

    /* No matter what act as if the data was consumed */
    OSC(txLength) -= len;
    OSC(objLen) -= len;
	
#if OBEX_GET_USE_END_OF_BODY == XA_ENABLED
	if (OSC(objLen) == 0) {
	    OSC(obsh) = 0;
	    ObexApp->goep_buff_wrote_len = 0;
	    ObexApp->goep_first_packet_received = KAL_FALSE;
        OSC(currentOperation) = NO_OPER_EVENTS;
            GOEP_Report("[OBEX] ObServerReadHeaderBlock_FitOnePkt first_packet_received:%d", ObexApp->goep_first_packet_received);
	}
#endif /* OBEX_GET_USE_END_OF_BODY == XA_ENABLED */

    return rcode;
}


#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            SessionRestoreObexConn()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Restores the OBEX Server directed connection table. In addition
 *            if protocol headers were sent in the original response, this
 *            function will rebuild them.
 */
static void SessionRestoreObexConn(ObexServerApp *ObexApp)
{
    U8  i;

    for (i = 0; i < OBEX_SERVER_CONS_SIZE; i++) {
        if (OSC(conn)[i])
            OSC(conn)[i]->connId = OSS(connId)[i];
    }
    OSC(activeConnId) = OSS(activeConnId);

    if (OSS(unackedProtoLen) == 0) {
        return;
    }
    ASSERT(OSC(activeConnId) != OBEX_INVALID_CONNID);

    /* Rebuild protocol transmit headers */
    i = (U8)OSC(activeConnId);

    if (OSC(conn[i]) != 0) {
        /* Build the WHO header.*/
        OSC(protoTxBuff)[0] = OBEXH_WHO;  
        OSC(protoTxBuff)[1] = (U8)((OSC(conn[i])->targetLen+3) >> 8);
        OSC(protoTxBuff)[2] = (U8)(OSC(conn[i])->targetLen+3);
        OS_MemCopy(OSC(protoTxBuff)+3, OSC(conn[i])->target, OSC(conn[i])->targetLen);

        OSC(protoTxLen) = 3 + OSC(conn[i])->targetLen;
            
        /* If were restoring a Connect packet, send a ConnId header back to the client. */
        if (OSS(curOpcode) == OB_OPCODE_CONNECT) {
            ASSERT(OSC(conn[i])->connId == i);
                
            OSC(protoTxBuff)[OSC(protoTxLen)+0] = OBEXH_CONNID;
            OSC(protoTxBuff)[OSC(protoTxLen)+1] = 0;
            OSC(protoTxBuff)[OSC(protoTxLen)+2] = 0;
            OSC(protoTxBuff)[OSC(protoTxLen)+3] = 0;
            OSC(protoTxBuff)[OSC(protoTxLen)+4] = i;
            OSC(protoTxLen) += 5;
        }
    }
    ASSERT(OSC(protoTxLen) == OSS(unackedProtoLen));
}
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
#endif /* OBEX_SERVER_CONS_SIZE > 0 */


#if OBEX_SESSION_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *            SessionSaveResponse()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Save a snapshot of the current operation just before we send
 *            the response.
 */
static void SessionSaveResponse(ObexServerApp *ObexApp)
{
    if (OSC(headerBlock) == HB_SERVER_HEADERS)
        OSS(unackedHeaderLen) = OSC(txLength);

    /* We received the entire OBEX packet from the client. Update session
     * state to reflect reception of the complete packet. 
     */
    if (OSC(currentOperation) == PUT_OPER_EVENTS) {
        ASSERT((OSS(lastSeqNum) == OSS(seqNum)) ? OSS(unackedObjBytes) == 0 : 1); 
        OSS(ackedObjBytes) += OSS(unackedObjBytes);
        OSS(unackedObjBytes) = 0;
    }

    /* Save this now because it'll get cleared on OBPE_OPER_COMPLETE event. */
#if OBEX_SERVER_CONS_SIZE > 0
    OSS(activeConnId) = (U8)OSC(activeConnId);
    OSS(unackedProtoLen) = OSC(protoTxLen);
#endif /* OBEX_SERVER_CONS_SIZE > 0 */

}


/*---------------------------------------------------------------------------
 *            SessionRestoreResponse()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This restore is triggered by the start of the client's command
 *            packet once the sequence number has been processed. It restores
 *            the server operation to reflect the state before or after the
 *            previous response was sent. The latter case occurs when this
 *            function is called after ServerSessionUpdateResponse().
 */
static void SessionRestoreResponse(ObexServerApp *ObexApp)
{
    ASSERT(OSC(sessionFlags) & OB_SESSION_RESUME);

    OSC(sessionFlags) &= ~OB_SESSION_RESUME;

    /* There was an operation in progress when we suspended.
     * We resume the operation by first indicating the resumption
     * to the applicaiton, then restoring the object and operation
     * state to where it was before the suspension. 
     */
    ASSERT((OSC(startEvent) >= OBSE_PUT_START) && (OSC(startEvent) <= OBSE_ABORT_START));

    OSC(resp) = OSS(resp);
    if ((OSC(resp) != (OBRC_CONTINUE|OB_FINAL_BIT)) &&
        (OSC(resp) != (OBRC_SUCCESS|OB_FINAL_BIT))) {
        /* The operation being resumed was also being aborted, stop now. */
        CancelOperation(ObexApp);
        return;
    }

    if ((OSS(currentOperation) & ~OPER_COMPLETE_FLAG) != OSC(currentOperation)) {
        /* Very bad client! Resuming an operation of a different type? */
        OSC(sessionFlags) |= OB_SESSION_ERROR;
        CancelOperation(ObexApp);
        OSC(resp) = (OBRC_CONFLICT|OB_FINAL_BIT);
        return;
    }

    /* Restore the previous response state. */
    OSC(obsh) = OSC(resumeObsh);
    
    /* Indicate the resume operation event if the operation was not completed. */
    if ((OSS(currentOperation) & OPER_COMPLETE_FLAG) == 0) {
        OBD(serverParms).opcode = (ObServerGetOpcode() & 0x7F);
        /* Restore a Put or Get operation in progress */
        OSC(opcode) = OSS(curOpcode);
        OSC(startEvent) = OBSE_RESUME_OPER;
        AppCallback(ObexApp, OBSE_NO_EVENT);
    } 
    else OSC(startEvent) = OBSE_NO_EVENT;

    if (OSC(obsh) != 0) {
        if (OSC(currentOperation) == GET_OPER_EVENTS) {
            OSC(objLen) = OSH(obsf)->GetObjectLen(OSC(obsh));

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
            if (OSC(objLen) != UNKNOWN_OBJECT_LENGTH)
#endif
                OSC(objLen) -= OSS(ackedObjBytes);
        }

        /* Always set offset even if ackedObjBytes == 0. */
        if (OSH(obsf)->SetOffset(OSC(obsh), OSS(ackedObjBytes)) != OBRC_SUCCESS) {
            /* Abort the operation */
            CancelOperation(ObexApp);
            OSC(resp) = (OBRC_CONFLICT|OB_FINAL_BIT);
            return;
        }
        /* Reset the unacknowledged object byte count because we are
         * restarting the transmit now and have no unacked bytes.
         */
        OSS(unackedObjBytes) = 0;
    } else {
        if ((OSS(currentOperation) == (PUT_OPER_EVENTS|OPER_COMPLETE_FLAG)) ||
            (OSS(currentOperation) == (GET_OPER_EVENTS|OPER_COMPLETE_FLAG))) {
            /* Were processing the last packet in a Put or Get operation.
             * The Process() functions null out the object store ptr because
             * they are done, but we'll need to put a fake a value in there
             * for this last run otherwise they'll mis-fire.
             */
            if ((ObServerGetOpcode() & OB_FINAL_BIT) == 0) {
                /* Error we're only expecting one more command. 
                 * Reject the resume. 
                 */
                CancelOperation(ObexApp);
                OSC(resp) = (OBRC_CONFLICT|OB_FINAL_BIT);
            }
            else OSC(obsh) = (void *)0xFEFE;
        }
    }
    
    /* Resume Headers were already copied in during the session resume. 
     * However it is not until now that we know that we need to send them.
     */
    OSH(txLength) = OSS(unackedHeaderLen);

    /* Restore OBEX Connections */
#if OBEX_SERVER_CONS_SIZE > 0
    SessionRestoreObexConn(ObexApp);
#endif /* OBEX_SERVER_CONS_SIZE > 0 */

    ASSERT(OSC(txLength) == 0);
}


/*---------------------------------------------------------------------------
 *            ServerSessionUpdateResponse()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Update the session status to reflect the clients confirmation
 *            of receipt of our last response packet. This update is triggered
 *            by the start of the client's command packet once the sequence
 *            number has been processed and is found to be in-sequence.
 */
static void ServerSessionUpdateResponse(ObexServerApp *ObexApp)
{
    /* Increment the current sequence number and the acknowledged byte counts. */

    ASSERT((ObServerGetOpcode() & 0x7F) != OB_OPCODE_SESSION);

    /* We received confirmation that data we sent was received by the client. */
    if (OSC(currentOperation) == GET_OPER_EVENTS) {
        ASSERT((OSS(lastSeqNum) == OSS(seqNum)) ? OSS(unackedObjBytes) == 0 : 1); 
        OSS(ackedObjBytes) += OSS(unackedObjBytes);
        OSS(unackedObjBytes) = 0;
    }

    if (OSS(lastSeqNum) != OSS(seqNum)) {
        /* We received confirmation that application headers were sent 
         * and received by the remote device.
         */
        OSS(unackedHeaderLen) = 0;
#if OBEX_SERVER_CONS_SIZE > 0
        OSS(unackedProtoLen) = 0;
#endif
    }

    if (OSC(sessionFlags) & OB_SESSION_RESUME) {
        /* We're resuming and in-sequence. To make sure that the operation
         * is truly being resumed, we need to check the response code.
         */
        if (OSS(resp) == (OBRC_CONTINUE|OB_FINAL_BIT)) {
            /* The response was in sequence but were are also resuming
             * from a suspended operation. Restore the operational values.
             */
            SessionRestoreResponse(ObexApp);
            return;
        }
        OSC(sessionFlags) &= ~OB_SESSION_RESUME;
    }

    if (OSS(currentOperation) & OPER_COMPLETE_FLAG) {
        /* This packet marks the beginning of a new operation. Clear out
         * any values leftover from the previous operation.
         */
        OSS(ackedObjBytes) = 0;
    }

    /* Keep these backup values current. */
    OSS(currentOperation) = OSC(currentOperation);
    /* Keep our opcode current for any Put or Get in progress */
    OSS(curOpcode) = OSC(opcode);
}


/*---------------------------------------------------------------------------
 *            ObServerSessionLayerEventInd()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Preprocess OBEX Parser events to perform session layer
 *            management. This function only applies to OBEX operations
 *            which may contain a Session Sequence Number header.
 *
 * Return:    
 *            TRUE - Continue normal processing of event.
 *            FALSE - Don't perform normal processing.
 */
static BOOL ObServerSessionLayerEventInd(ObexServerApp *ObexApp, ObexParserEvent Event)
{
    U8      seqNum;

    /* If this is a session command (and it's not a disconnect event) or
     * there is no active session then we have no processing to do here. 
     */
    if ((OSC(activeSession) == 0) || 
        ((Event != OBPE_DISCONNECT) && 
         ((ObServerGetOpcode() & 0x7F) == OB_OPCODE_SESSION))) {
        return TRUE;
    }

    switch (Event) {
    case OBPE_OPER_IND_START:
        /* This indicates the start of an operation.
         * Reset sequence number error to indicate no sequence 
         * number header has been received.
         */
        OSC(seqNumError) = ERR_NO_SEQ_NUM_HEADER;
        return TRUE;

    case OBPE_OPER_IND:
        /* NOTE: This case MUST return TRUE or the server will not send a response!
         * The only error case left to check here is "no sequence number" header.
         */
        if (OSC(seqNumError) == ERR_NO_SEQ_NUM_HEADER) {
            /* We did not get a sequence number header in the request. */
            OSC(sessionFlags) |= OB_SESSION_ERROR;

            CancelOperation(ObexApp);
            OSC(resp) = (OBRC_CONFLICT|OB_FINAL_BIT);
        }
        
        if (OSC(seqNumError) == ERR_NONE)
            OSS(seqNum)++;

        return TRUE;

    case OBPE_OPER_COMPLETE:
        OSS(currentOperation) |= OPER_COMPLETE_FLAG;
        
        if (OSC(sessionFlags) & OB_SESSION_ERROR) {
            /* We detected an error on the session, disconnect. We've
             * already sent our "error" response, now disconnect.
             */
            ObServerDisconnect();
        }
        return TRUE;

    case OBPE_PUT_HEADER:
        /* Process Session Sequence Number header */
        if (ObServerGetHeaderType() == OBEXH_SESSION_SEQ_NUM) {
            seqNum = ObServerGetHeaderByte();

            if (seqNum == OSS(seqNum)) {
                /* Normal behavior. We received the expected next sequence
                 * number and consume the header.
                 */
                OSC(seqNumError) = ERR_NONE;
                kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_SEQUENCE_NUMBER_VALID);

                /* Update our saved state for the command were executing.
                 * Soon, during the OBEX_ServerSendResponse() call we'll
                 * update the saved user state stuff too.
                 */
                ServerSessionUpdateResponse(ObexApp);
            }
            else if (((U8)(seqNum+1) == OSC(activeSession)->seqNum) &&
                     (OSC(sessionFlags) & OB_SESSION_RESUME)) {
                /* Client missed our response. Restore the previous
                 * state and retransmit our last response. Also, ignore
                 * any stuff in the client's request. Note this is also the
                 * case if we didn't get to send our response.
                 */
                OSC(seqNumError) = ERR_RETRANSMIT_AND_IGNORE;
                SessionRestoreResponse(ObexApp);
            } else {
                /* Unrecoverable sequence number error. Abort the operation. */
                OSC(seqNumError) = ERR_ABORT;
                OSC(sessionFlags) |= OB_SESSION_ERROR;

                CancelOperation(ObexApp);
                OSC(resp) = (OBRC_CONFLICT|OB_FINAL_BIT);
            }
            kal_trace(BT_TRACE_G6_OBEX,OBEX_SERVER_SEQUENCE_NUMBER_ERROR__xD , OSC(seqNumError));

            /* This must be done after we evaluate the sequence number. */
            OSS(lastSeqNum) = seqNum;
            /* Consume Sequence Number header. */
            return FALSE;
        }
        /* Drop into next case. */

    case OBPE_WRITE_HEADER:
        if (OSC(seqNumError) == ERR_NONE) {
            /* Allow application to receive the headers. */
            return TRUE;
        }

#if OBEX_SERVER_CONS_SIZE > 0
        /* Were not passing up headers, but the protocol needs to
         * process Target headers to rebuild the protoTxBuff.
         */
        if (ObServerGetHeaderType() == OBEXH_TARGET) {
            /* Associate a connection with the Target header (if one exists). */
            AssociateConn(ObexApp);
        }
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
        return FALSE;

    case OBPE_DISCONNECT:
        if (OSC(doCallback) == TRUE) {
            /* Inform the App that the Operation was suspended. If
             * the event handler index == NO_EVENT_HANDLER we may be
             * aborting the current operation or we may be idle.
             */
            ASSERT((OSC(sessionFlags) & OB_SESSION_ERROR) == 0);

            OSC(doCallback) = FALSE;
            OBD(serverParms).event = OBSE_SUSPENDED;
        } else {
            /* Inform the application that the active session was suspended. */
            OBD(serverParms).event = OBSE_SESSION_SUSPENDED;
        }

        if ((OSC(sessionFlags) & OB_SESSION_ERROR) == 0) {
            /* Valid session is suspending. */
            OBD(serverParms).u.suspended.session = OSC(activeSession);
            OBD(serverParms).u.suspended.obsh = OSC(obsh);

            /* Indicate any sent or to-be-sent headers to the applicaiton. */
            if (OSS(unackedHeaderLen) == 0) 
                OSS(unackedHeaderLen) = OSH(txLength);

            OBD(serverParms).u.suspended.headerLen = OSS(unackedHeaderLen);

            if (OSS(unackedHeaderLen))
                OBD(serverParms).u.suspended.headerBuff = OSH(buffer);
            else OBD(serverParms).u.suspended.headerBuff = 0;

            /* Save the current response code. */
            OSS(resp) = OSC(resp);

            SuspendSession(ObexApp);

        } else {
            /* Critical error on the session, end it don't suspend it. */
            OBD(serverParms).u.session = OSC(activeSession);
            OBD(serverParms).event = OBSE_SESSION_ERROR;
            OSC(activeSession) = 0;
        }
        
#if OBEX_SERVER_CONS_SIZE > 0
        OSC(activeConnId) = OBEX_INVALID_CONNID;
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
        OSC(appCallback)(&OBD(serverParms));
        return TRUE;
    }

    /* If we didn't process the event, assume its safe to proceed. */
    return TRUE;
}

/*---------------------------------------------------------------------------
 *            SuspendSession()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Save session state variables that are not operation specific.
 *
 */
static void SuspendSession(ObexServerApp *ObexApp)
{
#if OBEX_SERVER_CONS_SIZE > 0
    I8      i;

    /* Save active OBEX Connections */
    for (i = 0; i < OBEX_SERVER_CONS_SIZE; i++) {
        if (OSC(conn)[i])
            OSS(connId)[i] = (U8)OSC(conn)[i]->connId;
        else OSS(connId)[i] = 0;
    }
#endif /* OBEX_SERVER_CONS_SIZE > 0 */

    if (OSH(parser).flags & OBPF_CONNECTED) {
        /* We are connected; store the maxTxPacket value. */
        OSS(obPacketSize) = OSH(parser).maxTxPacket;
    } else {
        /* If we aren't connected, make sure we don't save 
         * an old maxTxPacket value. */
        OSS(obPacketSize) = 0;
    }

    OSC(activeSession) = 0;
}
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

#endif /* OBEX_ROLE_SERVER == XA_ENABLED */
