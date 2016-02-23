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

#ifndef __OBEXCMD_H
#define __OBEXCMD_H
/****************************************************************************
 *
 * File:          obexcmd.h
 *
 * Description:   This file specifies the command parser context structures.
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
#include <sys/obparse.h>
#include <sys/obstack.h>
#include <sys/obstkif.h>
#include <sys/obprotif.h>
#include <eventmgr.h>
/*--------------------------------------------------------------------------
 * 
 * Internally used defines for Client and Server header block read state. 
 * Used with ReadHeaderBlock() functions.
 */
#define HB_IDLE                     0
#if OBEX_ROLE_CLIENT == XA_ENABLED
#define HB_CLIENT_HEADERS           1
#define HB_CLIENT_OBJECT            2
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
#define HB_BOUNDRY                  9
#if OBEX_ROLE_SERVER == XA_ENABLED
#define HB_SERVER_HEADERS           10
#define HB_SERVER_OBJECT            11
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

/*--------------------------------------------------------------------------
 * 
 * Internally used defines for Client and Server event handler states. 
 */
#define NO_OPER_EVENTS              0
#define SIMPLE_OPER_EVENTS          1
#define PUT_OPER_EVENTS             2
#define GET_OPER_EVENTS             3
#if OBEX_SESSION_SUPPORT == XA_ENABLED
#define SESSION_OPER_EVENTS         4

/* This flag is or'd into the currentOperation on the server when the
 * operation completion (both Aborted and Complete) has been indicated
 * to the applicaiton. It is used during session resume to avoid double
 * completion of an operation.
 */
#define OPER_COMPLETE_FLAG          0x80

/*--------------------------------------------------------------------------
 * 
 * Internally used defines for Client and Server Session flags.
 */

/*--------------------------------------------------------------------------
 * Session Error values
 */
/* Session critical error: This flag is only set if the session encounters
 * an error which reders the session or the operation invalid. The client
 * will always disconnect the link after the active operation is aborted.
 * The server will always respond to the client with an failure response
 * code, then disconnect the link.
 */
#define OB_SESSION_ERROR            0x01

/* Session parameters header received */
#define OB_SESSION_PARMS            0x02

/* Active Session is closing or suspending */
#define OB_SESSION_DEACTIVATE       0x04

/* Resuming an operation on the session */
#define OB_SESSION_RESUME           0x08

/*--------------------------------------------------------------------------
 * Ignore Timeout parameter mask. Used with session parameters "fields".
 */
#define RX_SESSION_PARMS_IGNORE_MASK 0x2F

/*--------------------------------------------------------------------------
 * 
 * Definitions of the possible OBEX reliable session opcodes. Including
 * the OBEX Session Opcode.
 */
#define OB_SESSION_OPCODE_CREATE        0x00    /* Create a reliable OBEX session */
#define OB_SESSION_OPCODE_CLOSE         0x01    /* Close an existing OBEX session */
#define OB_SESSION_OPCODE_SUSPEND       0x02    /* Suspend an active OBEX session */
#define OB_SESSION_OPCODE_RESUME        0x03    /* Activate a suspended OBEX session */
#define OB_SESSION_OPCODE_SET_TIMEOUT   0x04    /* Set the suspend timeout for an OBEX session */

#define OB_OPCODE_SESSION               0x07    /* OBEX Session operation opcode. */

#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/****************************************************************************
 *
 * Structures used internally by the OBEX Command Interpreter
 *
 ***************************************************************************/
#if OBEX_ROLE_CLIENT == XA_ENABLED
/*--------------------------------------------------------------------------
 *
 * Internal client event handler for specific operation in progress.
 */
typedef void (*ClientProcessEvent)(ObexClientApp *ObexApp, 
                                   ObexParserEvent event);
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/*--------------------------------------------------------------------------
 *
 * Internal server event handler for specific operation in progress.
 */
typedef void (*ServerProcessEvent)(ObexServerApp *ObexApp, 
                                   ObexParserEvent event);

#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*--------------------------------------------------------------------------
 *
 * OBEX Client Context - This structure manages the overall behavior of the
 * client protocol layers. It encapsulates the data structures used by the
 * command interperter and protocol parser.
 *
 */
typedef struct _ObClientContext {
    void                *obsh;          /* Pointer to client's Object Store */
    U32                 objLen;
    /* Group: Transmit Header Block */
    U8                  *txBuffer;      /* Pointer to current transmit block */
    U16                 txLength;       /* length of current transmit block */
    U16                 txOffset;       /* offset over transmitted data */

    /* Group: Command Interpreter data */
    U8                  currentOperation; /* Event handler function table index */
    ObClientAppCb       appCallback;    /* Application event callback */
    ObexAbortReason     status;
    ObexOpcode          opcode;         /* Opcode we last sent from the Client (excludes Abort)*/
    U8                  unackedPackets; /* Number of unacknowledged packets */ 
    ObClientEvent       storedResp;     /* Stored response to indicate to the app later */

#if OBEX_SESSION_SUPPORT == XA_ENABLED
    /* Group: Protocol Transmit Header Block */
    U8                  protoTxLen;     /* Length Session Parms headers */
    U8                  protoTxBuff[56];

    U8                  seqNumError;    /* Any error with the received sequence number */
    U8                  sessionOpcode;  /* Opcode in session command */

    ObexClientSession  *activeSession;
    U8                  headerOffset;
    U8                  sessionFlags;

    /* Temporary holders for resume parameters while session resume
     * command is active and before suspended operation is resumed.
     */
    void               *resumeObsh;
    U8                 *resumeHeaders;
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
    U8                 srm_flags; /* 0 is no one wait OBSF_CLIENT_ENABLE and ... */
    U8                 srm_client; /* 0 is disable, OBSH_SRM */
    U32                obConnId;
    U8                 idFlag;
} ObClientContext;
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */


#if OBEX_ROLE_SERVER == XA_ENABLED
/*--------------------------------------------------------------------------
 *
 * OBEX Server context flags
 */
typedef U8 ObServerFlags;

#define OBSF_BODYH     0x01  /* BODY header received */
#define OBSF_ENDBODYH  0x20  /* END-OF-BODY header received */
#define OBSF_LENGTH    0x40  /* Length header received */


/*--------------------------------------------------------------------------
 *
 * OBEX Server Context - This structure manages the overall behavior of the
 * server protocol layers. It encapsulates the data structures used by the
 * command interperter and protocol parser.
 *
 */
typedef struct _ObServerContext {
    void               *obsh;           /* Pointer to servers' Object Store */
    U32                 objLen;
    U32                 rxOffset;
    ObServerFlags       flags;          /* flags : if recived BODY header */

#if OBEX_BODYLESS_GET == XA_ENABLED
    BOOL                obshAllowEmpty; /* Permit an empty Object Store Handle */
#endif /* OBEX_BODYLESS_GET == XA_ENABLED */

    BOOL                canFitOnePacket; //For Get Resp;

    /* Group: Transmit Header Block */
    U8                  *txBuffer;      /* Pointer to current transmit block */
    U16                 txLength;       /* length of current transmit block */
    U16                 txOffset;       /* offset over transmitted data */

    /* Group: Command Interpreter data */
    U8                  currentOperation; /* Event handler function table index */
    ObServerAppCb       appCallback;    /* Application event callback */
    ObexOpcode          opcode;         /* Put or Get opcode in progress */
    ObexRespCode        resp;           /* Response code to send */
    ObHeaderBlock       headerBlock;    /* transmit state */
    BOOL                doCallback;     /* have indicated start, so indicate end */
    BOOL                rejectResp;     /* We are issuing a server reject */

#if OBEX_PACKET_FLOW_CONTROL == XA_ENABLED
    U8                  sendResponse;   /* Check to protect OBEX_SendResponse() */
#endif /* OBEX_PACKET_FLOW_CONTROL == XA_ENABLED */

#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
    ObServerEvent       startEvent;     /* pending start event */

    /* Group: Protocol Transmit Header Block */
    U8                  protoTxLen;     /* Length of Who, ConnId & Session Parms headers */
    U8                  protoTxBuff[OBEX_MAX_SERVICE_UUID_SIZE+8+56];
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */

#if OBEX_SERVER_CONS_SIZE > 0
    /* Group: Connection mulitplexing and tracking */
    U32                 activeConnId;   /* active connection id */
    ObexConnection     *conn[OBEX_SERVER_CONS_SIZE];
#endif /* OBEX_SERVER_CONS_SIZE > 0 */

#if OBEX_SESSION_SUPPORT == XA_ENABLED
    /* Group: Session state information. */
    ObexServerSession  *activeSession;
    U8                  sessionFlags;
    U8                  seqNumError;

    /* Temporary holders for resume parameters while session resume
     * command is active and before suspended operation is resumed.
     */
    void               *resumeObsh;
    U8                 *resumeHeaders;
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
    U8                srm_flags;  /* receive peer's enable flag */
    U8                srm_server; /* 0 is disable, 1 enter srm mode, 2 enter srm but wait*/

} ObServerContext;
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

/****************************************************************************
 *
 * OBEX Client and Server macros
 *
 ****************************************************************************/
#if OBEX_ROLE_CLIENT == XA_ENABLED
/* Client context macro */
#define OCC(f)      (((ObexClientApp*)ObexApp)->client.f) /* OBEX Client Context */
#if OBEX_SESSION_SUPPORT == XA_ENABLED
/* Client session macro */
#define OCS(f)      (ObexApp->client.activeSession->f)  /* OBEX Client Active Session */
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
/* Client app handle macro */
#define OCH(f)      (ObexApp->handle.f)
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
/* Server context macro */
#define OSC(f)      (((ObexServerApp*)ObexApp)->server.f) /* OBEX Server Context */
#if OBEX_SESSION_SUPPORT == XA_ENABLED
/* Server session macro */
#define OSS(f)      (ObexApp->server.activeSession->f)  /* OBEX Server Active Session */
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
/* Server app handle macro */
#define OSH(f)      (ObexApp->handle.f)
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

/****************************************************************************
 *
 * Function reference - Functions exported by the Command Interpreter to
 *                      the OBEX Protocol Parser.
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ObClientEventInd(ObexAppHandle *AppHndl, ObexParserEvent event)
 *
 * Description:   Indicate a client event to the Command interepter.
 *
 * Parameters:    event - a parser event
 *
 * Returns:       void
 */
void ObClientEventInd(ObexAppHandle *AppHndl, ObexParserEvent event);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ObServerEventInd(ObexAppHandle *AppHndl, ObexParserEvent event)
 *
 * Description:   Indicate a server event to the Command interepter.
 *
 * Parameters:    event - a parser event
 *
 * Returns:       void
 */
void ObServerEventInd(ObexAppHandle *AppHndl, ObexParserEvent event);

/*---------------------------------------------------------------------------
 *
 * Prototype:     void ObReadHeaderBlock(ObHeaderBlock hb, U8 *buff, U16 len)
 *
 * Description:   Read len bytes of the header block bytes into the buffer
 *                pointed to by buff. This function must read the number of
 *                bytes specifed. The length given it is based on a call to
 *                ObHeaderBlockLen.
 *
 * Parameters:    ObexApp - handle to either the ObexClientApp or ObexServerApp
 *                
 *                hb - handle to header block
 *
 *                buff - pointer to buffer where header block data should be
 *                written.
 *
 *                len - number of bytes that must be written.
 *
 * Returns:       void.
 */
void ObReadHeaderBlock(void* ObexApp, ObHeaderBlock hb, U8* buff, U16 len);

#define ObReadHeaderBlock(_APP, _HB, _BUFF, _LEN) \
            ((_HB) > 9 ? ObServerReadHeaderBlock(_APP, _HB, _BUFF, _LEN) : \
                         ObClientReadHeaderBlock(_APP, _HB, _BUFF, _LEN) ) 

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 ObReadHeaderBlockFlex(U8 *buff, U16 len)
 *
 * Description:   Read up to len bytes of the header block bytes into the
 *                buffer pointed to by buff. This function can read any
 *                number of bytes <= whats specifed.
 *
 * Parameters:    ObexApp - handle to either the ObexClientApp or ObexServerApp
 *                
 *                hb - handle to header block
 *
 *                buff - pointer to buffer where header block data should be
 *                written.
 *
 *                len - number of bytes that can be written.
 *
 * Returns:       number of bytes written.
 */
U16 ObReadHeaderBlockFlex(void* ObexApp, ObHeaderBlock hb, U8 *buff, U16 len);

#define ObReadHeaderBlockFlex(_APP, _HB, _BUFF, _LEN) \
            ((_HB) > 9 ? ObServerReadHeaderBlockFlex(_APP, _BUFF, _LEN) : \
                         ObClientReadHeaderBlockFlex(_APP, _BUFF, _LEN) ) 
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 *
 * Prototype:     U16 ObGetHeaderBlockLen(void *ObexApp, ObHeaderBlock hnd)
 *
 * Description:   Return the number of bytes left in the header block.
 *
 * Parameters:    hnd - handle to header block
 *
 * Returns:       Number of bytes remaining in the header block.
 */
U16 ObGetHeaderBlockLen(void *ObexApp, ObHeaderBlock hb);

U16 ObClientGetHeaderBlockLen(ObexClientApp *ObexApp, ObHeaderBlock hb);
U16 ObServerGetHeaderBlockLen(ObexServerApp *ObexApp, ObHeaderBlock hb);

U16 ObServerGetHeaderBlockLen_FitOnePkt(ObexServerApp *ObexApp, ObHeaderBlock hb);


#define ObGetHeaderBlockLen(_APP, _HB) \
            ((_HB) == 0 ? 0 : ((_HB) > HB_BOUNDRY ? \
            ObServerGetHeaderBlockLen((ObexServerApp*)_APP, _HB) : \
            ObClientGetHeaderBlockLen((ObexClientApp*)_APP, _HB)))
                              
/****************************************************************************
 *
 * Function prototypes of internal OBEX driver functions
 *
 ****************************************************************************/
ObexRespCode ObClientReadHeaderBlock(ObexClientApp *ObexApp, ObHeaderBlock hb, U8 *buff, U16 len);
U16  ObClientReadHeaderBlockFlex(ObexClientApp *ObexApp, U8 *buff, U16 len);
ObexRespCode ObServerReadHeaderBlock(ObexServerApp *ObexApp, ObHeaderBlock hb, U8 *buff, U16 len);
U16  ObServerReadHeaderBlockFlex(ObexServerApp *ObexApp, U8 *buff, U16 len);

ObexRespCode ObServerReadHeaderBlock_FitOnePkt(ObexServerApp *ObexApp, ObHeaderBlock hb, U8* buff, U16 len);


#if OBEX_ROLE_CLIENT == XA_ENABLED
#define IsClientBusy()      ((OCC(currentOperation) > 0) ? TRUE : FALSE)
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_DISABLED
#define ObClientReadHeaderBlock(ObexApp, hb, buff, len) (void)0
#define ObClientReadHeaderBlockFlex(ObexApp, buff, len) (0)
#define ObClientGetHeaderBlockLen(ObexApp, hb)  (0)
#endif /* OBEX_ROLE_CLIENT == XA_DISABLED */

#if OBEX_ROLE_SERVER == XA_DISABLED
#define ObServerReadHeaderBlock(ObexApp, hb, buff, len) (void)0
#define ObServerReadHeaderBlockFlex(ObexApp, buff, len) (0)
#endif /* OBEX_ROLE_SERVER == XA_DISABLED */
/****************************************************************************
 *
 * Types and Constants used by the command interpreter
 *
 ***************************************************************************/
/*--------------------------------------------------------------------------
 *
 * OBEX Command Opcodes not defined in obex.h 
 */
#define OB_OPCODE_COMMAND    0x04     /* This opcode is no longer in service */
#define OB_OPCODE_ABORT      0x7f

#define OB_FINAL_BIT         0x80

#endif /* __OBEXCMD_H */

