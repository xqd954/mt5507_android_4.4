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

#ifndef __BT_SIM_H
#define __BT_SIM_H
/*****************************************************************************
 *
 * File:
 *     $Workfile:sim.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: Public types, defines, and prototypes for accessing the
 *              SIM Access layer.
 * 
 * Created:     May 1, 2002
 *
 * Copyright 1999-2002 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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

#include "bttypes.h"
#include "eventmgr.h"
#include "rfcomm_adp.h"
#include "sdp_adp.h"
#include "conmgr.h"
#include "bluetooth_simap_struct.h"

/*---------------------------------------------------------------------------
 * SIM Access Profile API layer
 *
 * This API allows an application to implement the Client and/or
 * Server role of the SIM Access Profile. The Server is typically a
 * mobile phone with a SIM card installed. The SIM Access Client is a
 * device (such as a car phone) that accesses the Server's SIM card
 * information over a Bluetooth link.
 *
 * This application interface defines both Server and Client APIs.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SIM_SERVER constant
 *     Defines the role of the device as a server.  Server code is included
 *     if SIM_SERVER is defined as XA_ENABLED.  A device can have both the
 *     server and client roles, if necessary.
 */
#ifndef SIM_SERVER
#define SIM_SERVER XA_ENABLED
#endif 

/*---------------------------------------------------------------------------
 * SIM_CLIENT constant
 *     Defines the role of the device as a client.  Client code is included
 *     if SIM_CLIENT is defined as XA_ENABLED.  A device can have both the
 *     server and client roles, if necessary.
 */
#ifndef SIM_CLIENT
/* Up to now, it does not support the role of SIM_CLIENT yet : 2006-10-15 */
#define SIM_CLIENT XA_DISABLED
#endif /* SIM_CLIENT */ 

#if (SIM_SERVER != XA_ENABLED) && (SIM_CLIENT != XA_ENABLED)
#error SIM_SERVER or SIM_CLIENT must be enabled
#endif 

/*---------------------------------------------------------------------------
 * SIM_MAX_MSG_SIZE constant
 *     Defines the maximum frame size supported by the SIM Access profile.
 *     This value can be up to 64K bytes.  The default size is the same
 *     as the RFCOMM frame size.
 */
#ifndef SIM_MAX_MSG_SIZE
#define SIM_MAX_MSG_SIZE RF_MAX_FRAME_SIZE
#endif 

/*---------------------------------------------------------------------------
 * SIM_MAX_APDU and SIM_MAX_ATR constant
 *     Defines the maximum APDU and ATR size to be sent/received
 *     by the SIM Access profile.
 */
#ifndef SIM_MAX_APDU
#define SIM_MAX_APDU 276
#define SIM_MAX_ATR 276
#endif /* SIM_MAX_APDU */ 

/* Maximum number of parms expected in a message */
#define SIM_MAX_MSG_PARMS 3

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------- 
 * Define the constant number of SIM Server 
 */

#if SIM_SERVER == XA_ENABLED
#define MAX_NUM_SIM_SERVER     1
#define MAX_WAIT_ATR_REQ_MSEC  10000    /* 10sec */
#define MAX_WAIT_REQ_MSEC_IN_GRACEFUL_DISC_MODE 10000   /* 10sec */
#define SPEC_CONNECT_MODE  1
#define NOKIA_CONNECT_MODE 2
#define FAST_CONNECT_MODE  NOKIA_CONNECT_MODE
#define SLOW_CONNECT_MODE  SPEC_CONNECT_MODE
#define CONNECTION_MODE SPEC_CONNECT_MODE       /* 1: RCV ATR_REQ, switch to OPEN state; 2: SEND STATUS_IND, switch to OPEN state (Nokia N91 solution) */
#define TESTMODE1_TURN_ON FALSE  /* TESTMODE1: used to test BT SIM Access Profile itself, fakeSim testing code will be used */
#define SET_T_SUPPORTED   TRUE

#if (MAX_NUM_SIM_SERVER >1)
#error The value of MAX_NUM_SIM_SERVER must be 1
#endif 

#endif /* SIM_SERVER == XA_ENABLED */ 

#if TESTMODE1_TURN_ON == TRUE
#define TRANS_PRO_TO        0
#define TRANS_PRO_T1        1
#define TRANS_PRO_TO_T1     2
#endif /* TESTMODE1_TURN_ON == TRUE */ 

/*---------------------------------------------------------------------------
 * SimEvent type
 *
 *     All event indications and confirmations are passed to a callback
 *     function of type SimServerCallback or SimClientCallback. The
 *     "SimCallbackParms.event" field will contain one of the event types
 *     below.
 */
typedef U8 SimEvent;

#if SIM_SERVER == XA_ENABLED
/** A client has requested a connection to the server.  During processing of 
 *  this event, call SIM_ServerRespondOpenInd to indicate whether the connection 
 *  should be allowed.  When this callback is received, the 
 *  "SimCallbackparms.ptrs.remDev" field contains a pointer to the remote device.
 */
#define SAEVENT_SERVER_OPEN_IND                 0x01

/** A connection is open.  This can come as a result of a call to 
 *  SIM_ServerRespondOpenInd.  When this event has been received, the  
 *  connection is available for receiving requests and sending responses.
 *  When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
 *  contains a pointer to the remote device.
 */
#define SAEVENT_SERVER_OPEN                     0x02

/** The remote device is closing the connection.  Once the connection is closed, 
 *  a SAEVENT_SERVER_CLOSED event will be received.  Requests will not be 
 *  received and responses cannot be sent in this state.
 *  When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
 *  contains a pointer to the remote device.
 */
#define SAEVENT_SERVER_CLOSE_IND                0x03

/** The connection is closed.  This can come as a result of calling 
 *  SIM_ServerClose, if the remote device has closed the connection, or if an 
 *  incoming connection is rejected by a call to SIM_ServerRespondOpenInd.  
 *  Requests will not be received and responses cannot be sent in this state.  
 *  When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
 *  contains a pointer to the remote device.
 */
#define SAEVENT_SERVER_CLOSED                   0x04

/** The ATR is being requested by the client.  This can happen at any time
 *  during a connection (after SAEVENT_SERVER_OPEN is received).  It can also
 *  happen before the connection (after SAEVENT_SERVER_OPEN_IND is received).
 *  The server must respond by calling SIM_ServerAtrRsp with a valid ATR
 *  as specified by the GSM spec, or with an error code describing the reason
 *  the response cannot be made.  During this callback, the
 *  "SimCallbackParms.ptrs" field is not valid.
 */
#define SAEVENT_SERVER_ATR_REQ                  0x05

/** An APDU has been received from the client.  This can happen at any time
 *  during a connection (after SAEVENT_SERVER_OPEN is received).
 *  The server must respond by calling SIM_ServerApduRsp with a valid APDU
 *  as specified by the GSM spec, or with an error code describing the reason
 *  the response cannot be made.  During this callback, the 
 *  "SimCallbackParms.ptrs.msg.parms.apdu" field points to the command APDU.
 */
#define SAEVENT_SERVER_APDU_REQ                 0x06

/** During the connect process, the server must send the status of the SIM
 *  card.  The application will be prompted to send the status with this
 *  event.  During the processing of this event, the application must call 
 *  SIM_ServerSendStatus.  During this callback, the "SimCallbackParms.ptrs" 
 *  field is not valid.
 */
#define SAEVENT_SERVER_STATUS_REQ               0x07

/** When the client requests that the SIM card be turned on, the server will
 *  receive the SAEVENT_SERVER_SIM_ON_REQ event.  The server should respond
 *  to this request by calling SIM_ServerSimOnRsp with the result of the
 *  action (see SimResultCode).  During this callback, the "SimCallbackParms.ptrs" 
 *  field is not valid.
 */
#define SAEVENT_SERVER_SIM_ON_REQ               0x08

/** When the client requests that the SIM card be turned off, the server will
 *  receive the SAEVENT_SERVER_SIM_OFF_REQ event.  The server should respond
 *  to this request by calling SIM_ServerSimOffRsp with the result of the
 *  action (see SimResultCode).  During this callback, the "SimCallbackParms.ptrs" 
 *  field is not valid.
 */
#define SAEVENT_SERVER_SIM_OFF_REQ              0x09

/** When the client requests that the SIM card be reset, the server will
 *  receive the SAEVENT_SERVER_RESET_SIM_REQ event.  The server should respond
 *  to this request by calling SIM_ServerResetSimRsp with the result of the
 *  action (see SimResultCode).  During this callback, the "SimCallbackParms.ptrs" 
 *  field is not valid.
 */
#define SAEVENT_SERVER_RESET_SIM_REQ            0x0A

/** When the client requests the status of the card reader, the server will
 *  receive the SAEVENT_SERVER_CARD_READER_STATUS_REQ event.  The server should 
 *  respond to this request by calling SIM_ServerCardReaderStatusRsp with the 
 *  result of the action (see SimResultCode and SimCardReaderStatus).
 *  During this callback, the "SimCallbackParms.ptrs" field is not valid.
 */
#define SAEVENT_SERVER_CARD_READER_STATUS_REQ   0x0B

/** When the server sends the status of the SIM card, this event will indicate
 *  that the status has been sent successfully.  The SIM card status is the
 *  only unsolicited message that the server can send, therefore, it is 
 *  important to know when it is safe to send the command.  After receiving this
 *  event, the application can send another SIM card status in necessary.
 */
#define SAEVENT_SERVER_STATUS_SENT              0x0C

/** When the client requests that the transport protocol identifier be set,
 * the server will receive this event. The server should respond by calling
 * SIM_ServerSetTransProtRsp() to indicate its acceptance or rejection
 * of the request. During this callback, the "SimCallbackParms.ptrs" field
 * is not valid.
 */
#define SAEVENT_SERVER_SET_TRANS_PROT_REQ           0x0D

#define NUM_OF_SIMPACKETS 3     

#endif /* SIM_SERVER == XA_ENABLED */

#if SIM_CLIENT == XA_ENABLED
/** A connection is open.  This can come as a result of a call to 
 *  SIM_ClientOpen.  When this event has been received, the connection is 
 *  available for receiving requests and transmitting responses.
 *  When this callback is received, the "SimCallbackParms.ptrs.remDev" field 
 *  contains a pointer to the remote device.
 */
#define SAEVENT_CLIENT_OPEN                     0x81

/** The server is requesting that the connection be closed.  This is a graceful
 *  termination, and it is up to the application to finish up any work and then
 *  close the connection by calling SIM_ClientClose.  When this callback is 
 *  received, the "SimCallbackParms.ptrs.remDev" field contains a pointer to the 
 *  remote device.
 */
#define SAEVENT_CLIENT_CLOSE_IND                0x82

/** The connection is closed.  This can come as a result of calling 
 *  SIM_ClientClose, or if the remote device has closed (or rejected) the 
 *  connection.  Requests cannot be sent in this state.  When this callback is 
 *  received, the "SimCallbackParms.ptrs.remDev" field contains a pointer to the 
 *  remote device.
 */
#define SAEVENT_CLIENT_CLOSED                   0x83

/** An APDU response has been received as a result of a call to 
 *  SIM_ClientApduReq.  When this event is received with "status" equal
 *  to BT_STATUS_SUCCESS, the "ptrs.msg" field is valid. If "ptrs.msg.result"
 *  is SIM_RESULT_OK, then the "ptrs.msg.parms.apdu" field contains the 
 *  response data.
 */
#define SAEVENT_CLIENT_APDU_RESP                0x84

/** An ATR Response has been received as a result of a call to
 *  SIM_ClientAtrReq or SIM_ClientOpen.  When this event is received with
 *  "status" equal to BT_STATUS_SUCCESS, the "ptrs.msg" field is valid.
 *  If "ptrs.msg.result" is SIM_RESULT_OK, then the "ptrs.msg.parms.atr"
 *  field contains the response data.
 */
#define SAEVENT_CLIENT_ATR_RESP                 0x85

/** When the server responds to a request by the client to turn the 
 *  SIM card off, the client will receive the SAEVENT_CLIENT_SIM_OFF_RESP event.
 *  During this callback, the "SimCallbackParms.ptrs.msg.result" field contains
 *  the results of the action (see SimResultCode).
 */
#define SAEVENT_CLIENT_SIM_OFF_RESP             0x86

/** When the server responds to a request by the client to turn the 
 *  SIM card on, the client will receive the SAEVENT_CLIENT_SIM_ON_RESP event.
 *  During this callback, the "SimCallbackParms.ptrs.msg.result" field contains
 *  the results of the action (see SimResultCode).  This event will always be
 *  followed by an SAEVENT_CLIENT_ATR_RESP event, because the client 
 *  automatically requests the ATR after the SIM is turned on.
 */
#define SAEVENT_CLIENT_SIM_ON_RESP              0x87

/** When the server responds to a request by the client to reset the SIM card,
 *  the client will receive the SAEVENT_CLIENT_RESET_SIM_RESP event.  During 
 *  this callback, the "SimCallbackParms.ptrs.msg.result" field contains the 
 *  results of the action (see SimResultCode).  This event will always be
 *  followed by an SAEVENT_CLIENT_ATR_RESP event, because the client 
 *  automatically requests the ATR after the SIM is reset.
 */
#define SAEVENT_CLIENT_RESET_SIM_RESP           0x88

/** When the server responds to a request by the client to retrieve the card
 *  reader status, the client will receive the 
 *  SAEVENT_CLIENT_CARD_READER_STATUS_RESP event.  During this callback, the 
 *  "SimCallbackParms.ptrs.msg.cardRdrStatus" field contains the status of the
 *  card (see SimCardReaderStatus).
 */
#define SAEVENT_CLIENT_CARD_READER_STATUS_RESP  0x89

/** When the server responds to the client's request to set the transport
 * protocol ID (SIM_ClientSetTransProtReq()), this event occurs.
 * If "status" is BT_STATUS_SUCCESS then "ptrs.msg.result" will contain the
 * results of the action.
 */
#define SAEVENT_CLIENT_SET_TRANS_PROT_RESP          0x8A

/** If the state of the SIM card changes, the client will receive an
 *  SAEVENT_CLIENT_STATUS_IND event.  During this callback, the 
 *  "SimCallbackParms.ptrs.msg.statusChange" field contains the new state of
 *  the SIM card (see SimCardStatus).
 */
#define SAEVENT_CLIENT_STATUS_IND               0x8B

/** If the server receives an unknown message, or a message with an error, the
 *  client will receive the SAEVENT_CLIENT_ERROR_RESP event.  During this 
 *  callback, the "SimCallbackParms.ptrs" is not valid.
 */
#define SAEVENT_CLIENT_ERROR_RESP               0x8C

#endif /* SIM_CLIENT == XA_ENABLED */

/* End of SimEvent */

/*---------------------------------------------------------------------------
 * SimConnStatus type
 *
 *     When the client wishes to connect to the server, the server can respond
 *     with different status codes.  These codes indicate whether the connection
 *     can be accepted or not at the time of the request.
 */
typedef U8 SimConnStatus;

/** The connection is accepted.  When this status code is used, the connection 
 *  will progress normally.  When the connection is established, the 
 *  SAEVENT_SERVER_OPEN event will be received.  Previous to the 
 *  SAEVENT_SERVER_OPEN event, a SAVEVENT_SERVER_STATUS_REQ event will be
 *  received.  The application must respond to this by sending the current
 *  card status by calling SIM_ServerSendStatus.  The application may also
 *  receive a SAEVENT_SERVER_ATR_REQ event.  If this event is received,
 *  the application must call SIM_ServerAtrRsp with the ATR response.
 */
#define SIM_CONN_STATUS_OK           0

/** The connection is rejected.  When this status code is used, the connection
 *  is rejected and the connection remains closed.
 */
#define SIM_CONN_STATUS_UNABLE       1

/* End of SimConnStatus */

#if 0
/*---------------------------------------------------------------------------
 * SimDisconnectType type
 *
 *     When the server wishes to disconnect, it can send a disconnect 
 *     indication to the client.  There are two types of disconnect that can
 *     be initiated by the server.  
 */
typedef U8 SimDisconnectType;

/** A graceful disconnect allows the client to continue sending APDU's until
 *  while the GSM connection is terminated.  The client will then initiate
 *  the disconnect process.  Until the client initiates the disconnect process,
 *  the connection stays open.
 */
#define SIM_DISCONNECT_GRACEFUL   0x00

/** An immediate disconnect terminates the connection immediately.  No response
 *  is required from the client.  The GSM connection should be terminated
 *  immediately.
 */
#define SIM_DISCONNECT_IMMEDIATE  0x01

/* End of SimDisconnectType */
#endif

#if 0
/*---------------------------------------------------------------------------
 * SimCardStatus type 
 *     This type of the parameter StatusChangeis defined in the command STATUS_IND 
 *     When the status of the SIM changes, the server must send the new status
 *     to the client.  During the connect procedure, the status must also be
 *     sent.
 */
typedef U8 SimCardStatus;

/** The status of the card is unknown.
 */
#define SIM_CARD_STATUS_UNKNOWN          0x00

/** The card has been reset and is available.
 */
#define SIM_CARD_STATUS_RESET            0x01

/** The card is not accessible.
 */
#define SIM_CARD_STATUS_NOT_ACCESSIBLE   0x02

/** The card has been removed.
 */
#define SIM_CARD_STATUS_REMOVED          0x03

/** The card has just been inserted.
 */
#define SIM_CARD_STATUS_INSERTED         0x04

/** The card has been recovered.
 */
#define SIM_CARD_STATUS_RECOVERED        0x05

/* End of SimCardStatus */
#endif

/*---------------------------------------------------------------------------
 * SimResultCode type
 *
 *     Several requests require a result code.  This result code describes
 *     the result of the action initiated by a particular request.
 */
typedef U8 SimResultCode;

/** The request processed correctly.
 */
#define SIM_RESULT_OK                   0x00

/** Error, no reason defined.
 */
#define SIM_RESULT_NO_REASON            0x01

/** Error, card not accessible 
 */
#define SIM_RESULT_CARD_NOT_ACCESSIBLE  0x02

/** Error, the card is already off.
 */
#define SIM_RESULT_CARD_ALREADY_OFF     0x03

/** Error, the card is removed.
 */
#define SIM_RESULT_CARD_REMOVED         0x04

/** Error, the card is already on.
 */
#define SIM_RESULT_CARD_ALREADY_ON      0x05

/** Error, data is not available.
 */
#define SIM_RESULT_DATA_NOT_AVAILABLE   0x06

/* This constant is also defined in bluetooth SAP spec */
#define SIM_RESULT_NOT_SUPPORTED    0x07

/* End of SimResultCode */

/*---------------------------------------------------------------------------
 * SimCardReaderStatus type
 *
 *      Specifies the identity of the card reader, and flags to indicate the
 *      status of the reader.
 */
typedef U8 SimCardReaderStatus;

/*
 * From Spec: Card Reader Status is formated as follows: add following comments
 * bit 8: Card in reader is powered on or not (powered on, this bit=1)
 * bit 7: Card inserted or not (Card inserted, this bit=1)
 * bit 6: Card reader is ID-1 size or not (our device is not ID-1 size, so this bit =0)
 * bit 5: Card reader is present or not (for our device, this bit=1)
 * bit 4: Card reader is removable or not (for our device, it is not removable, so this bit=0)
 * bit 3-1: Identifier of the Card reader (for our device: ID=0) 
 * The normal case, card reader status of our device = 0b11010000 = 0xD0)
 * 
 */


/* SIM_CRS_IDx : specify bit3-1 */
#define SIM_CRS_ID0                     0x00
#define SIM_CRS_ID1                     0x01
#define SIM_CRS_ID2                     0x02
#define SIM_CRS_ID3                     0x03
#define SIM_CRS_ID4                     0x04
#define SIM_CRS_ID5                     0x05
#define SIM_CRS_ID6                     0x06
#define SIM_CRS_ID7                     0x07

#define CARD_ID(x) SIM_CRS_ID##x

/* Specify bit 4: Card reader is removable or not */
#define SIM_CRS_NOT_REMOVABLE           0x00
#define SIM_CRS_REMOVABLE               0x08

/* Specify bit5: Card reader is present or not */
#define SIM_CRS_NOT_PRESENT             0x00
#define SIM_CRS_PRESENT                 0x10

/* Specify bit6: Card reader is ID-1 size or not */
#define SIM_CRS_NOT_ID1_SIZE            0x00
#define SIM_CRS_ID1_SIZE                0x20

/* Specify bit7: Card is inserted or not */
#define SIM_CRS_NO_CARD_PRESENT         0x00
#define SIM_CRS_CARD_PRESENT            0x40

/* Specify bit8: Card is powered on or not */
#define SIM_CRS_CARD_POWERED_OFF 0x00
#define SIM_CRS_CARD_POWERED_ON 0x80

/* End of SimCardReaderStatus */

/*---------------------------------------------------------------------------
 * SimApduType type
 *
 *     Describes the Application Protocol Data Unit type (used in SimApdu).
 */
typedef U8 SimApduType;

/* APDU defined according to GSM 11.11 or 11.14 */
#define SIM_APDU_GSM        0x00

/* APDU defined according to ISO/IEC 7816-4 */
#define SIM_APDU_IEC_7816   0x01

/* End of SimApduType */

/* Forward references */
typedef struct _SimCallbackParms SimCallbackParms;

#if SIM_SERVER == XA_ENABLED
typedef struct _SimServer SimServer;
#endif /* SIM_SERVER == XA_ENABLED */
#if SIM_CLIENT == XA_ENABLED
typedef struct _SimClient SimClient;
#endif /* SIM_CLIENT == XA_ENABLED */
typedef struct _SimApdu SimApdu;
typedef struct _SimAtr SimAtr;
typedef struct _SimMsg SimMsg;

/* SIM packet */
typedef struct _SimPacket
{
    BtPacket packet;
    U8 packetData[SIM_MAX_APDU];
    U16 dataToSend;
    U16 maxMsgSize;
} SimPacket;

#if SIM_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * SimServerCallback type
 *
 *     Represents a callback function called by the SIM Access layer to 
 *     indicate events and data to the server application. The event is 
 *     targeted to the SIM Access Server identified by the "Server" parameter. 
 *     Information about the event is contained in the "Parms" parameter.
 */
typedef void (*SimServerCallback) (SimServer *Server, SimCallbackParms *Parms);
#endif /* SIM_SERVER == XA_ENABLED */

/* End of SimServerCallback */

#if SIM_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SimClientCallback type
 *
 *     Represents a callback function called by the SIM Access layer to 
 *     indicate events and data to the client application. The event is 
 *     targeted to the SIM Access Client identified by the "Client" parameter. 
 *     Information about the event is contained in the "Parms" parameter.
 */
typedef void (*SimClientCallback) (SimClient *Client, SimCallbackParms *Parms);
#endif /* SIM_CLIENT == XA_ENABLED */

/* End of SimClientCallback */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SimApdu structure
 *
 *     Describes APDU data.
 */
struct _SimApdu
{
    U16 dataLen;            /* Length of the APDU data */
    U8 data[SIM_MAX_APDU];  /* Pointer to APDU data    */

    /* Style of APDU data (not valid during SAEVENT_CLIENT_APDU_RESP) */
    SimApduType type;

};

/*---------------------------------------------------------------------------
 * SimAtr structure
 *
 *     Describes ATR data.
 */
struct _SimAtr
{
    U16 dataLen;            /* Length of the ATR data */
    U8 data[SIM_MAX_ATR];  /* Pointer to ATR data    */
};

/*---------------------------------------------------------------------------
 * SimMsg structure
 *
 *     Describes a SIM message.  The SIM messages are passed between the client
 *     and the server in order to perform specific functions.  During a callback,
 *     this information is provided to the application in the SimCallbackParms
 *     structure.
 */
struct _SimMsg
{
    /*
     * Results code for the message.  Only valid during the following
     * responses:
     *
     *     SAEVENT_CLIENT_APDU_RESP,
     *     SAEVENT_CLIENT_ATR_RESP,               
     *     SAEVENT_CLIENT_SIM_OFF_RESP,           
     *     SAEVENT_CLIENT_SIM_ON_RESP,            
     *     SAEVENT_CLIENT_RESET_SIM_RESP, and          
     *     SAEVENT_CLIENT_CARD_READER_STATUS_RESP
     */
    U8 resultCode;

    /* Parameters */
    union
    {
        /*
         * During an SAEVENT_SERVER_APDU_IND or SAEVENT_CLIENT_APDU_RESP event, 
         * contains the APDU data received from the remote device.
         */
        SimApdu apdu;

        /*
         * During an SAEVENT_CLIENT_ATR_RESP event, contains the ATR data 
         * received from the SIM server.
         */
        SimAtr atr;

        /*
         * During an SAEVENT_CLIENT_CARD_READER_STATUS_RESP event, contains the
         * card reader status.
         */
        SimCardReaderStatus cardRdrStatus;

        /*
         * During an SAEVENT_CLIENT_STATUS_IND, contains the status of the SIM
         * card.
         */
        SimCardStatus statusChange;

        /*
         * During an SAEVENT_SERVER_SET_TRANS_PROT_REQ, contains
         * protocol ID specified by client.
         */
        U8 protocolId;

        /* === Internal use only === */
        U16 maxMsgSize;
        SimDisconnectType discType;
        BtPacket *packet;
    } parm;

    /* === Internal use only === */
    U8 type;        /* Message type             */
    U8 numParms;    /* Number of parms returned */
    U8 connStatus;
};

/*---------------------------------------------------------------------------
 * SimCallbackParms structure
 *
 *     Describes a callback event and any data that relates to the event.
 *     Various fields in the structure may be valid or not, depending
 *     on the "event" field.
 */
struct _SimCallbackParms
{
    SimEvent event; /* Type of the SIM Access event */

    BtStatus status;    /* Link status or error information */

    U16 len;    /*
                 * Length of the object pointed to by
                 * 'ptrs' 
                 */

    SimResultCode results;  /* Result code for the SIM operation */

    /*
     * Group: The event type determines whether a field in this
     * union is valid.
     */
    union
    {
        /*
         * During the following events, contains information specific to the
         * message that was exchanged:
         *
         *     SAEVENT_CLIENT_APDU_RESP,
         *     SAEVENT_CLIENT_ATR_RESP,               
         *     SAEVENT_CLIENT_SIM_OFF_RESP,
         *     SAEVENT_CLIENT_SIM_ON_RESP,          
         *     SAEVENT_CLIENT_RESET_SIM_RESP, 
         *     SAEVENT_CLIENT_CARD_READER_STATUS_RESP,
         *     SAEVENT_CLIENT_STATUS_IND
         */
        SimMsg *msg;

        /*
         * During the following events, indicates the relevant remote device:
         *
         * SAEVENT_SERVER_OPEN_IND, SAEVENT_SERVER_OPEN,
         * SAEVENT_CLIENT_OPEN, SAEVENT_SERVER_CLOSED, SAEVENT_CLIENT_CLOSE_IND,
         * and SAEVENT_CLIENT_CLOSED.
         */
        BtRemoteDevice *remDev;

    } ptrs;
};

/* 
 * Internal events used to notify Internal ADP
 * SIMAP --> INT ADP
 */
typedef enum
{
	/* Event for MMI */
	SIMAP_EVENT_ACTIVATE_CNF,
	SIMAP_EVENT_DEACTIVATE_CNF,
	SIMAP_EVENT_CONNECT_IND,
	SIMAP_EVENT_DISCONNECT_IND, 
	SIMAP_EVENT_DISCONNECT_CNF, 
	SIMAP_EVENT_AUTH_REQ,
	SIMAP_EVENT_ATR_RSP_SENT_IND,
	SIMAP_EVENT_APDU_RSP_SENT_IND, 
	SIMAP_EVENT_SIM_ON_RSP_SENT_IND, 
	SIMAP_EVENT_SIM_OFF_RSP_SENT_IND, 
	SIMAP_EVENT_SIM_RESET_RSP_SENT_IND, 
	SIMAP_EVENT_CARD_STATUS_IND_SENT_IND, 
	SIMAP_EVENT_CARD_READER_STATUS_RSP_SENT_IND, 
	SIMAP_EVENT_SET_TRANSPORT_PROTOCOL_RSP_SENT_IND, 
	SIMAP_EVENT_AUTH_REQ_TIMEOUT_RFCLOSED_IND, 
	SIMAP_EVENT_SIM_CONNECT_FAILED_IND, 
	SIMAP_EVENT_BT_RESET_IND,

	/* Event for SIM task */
	SIM_EVENT_CONNECT_REQ, 
	SIM_EVENT_DISCONNECT_REQ, 
	SIM_EVENT_APDU_REQ, 
	SIM_EVENT_POWER_OFF_REQ, 
	SIM_EVENT_RESET_REQ
} SIMAP_event;

typedef struct _SIMAP_event_connect_ind_struct
{        
    kal_uint8 cid;      /* connection id */
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;   
} SIMAP_event_connect_ind_struct;

typedef struct _SIMAP_event_activate_cnf_struct
{        
    kal_bool result;   
} SIMAP_event_activate_cnf_struct;

typedef struct _SIMAP_event_deactivate_cnf_struct
{        
    kal_bool result;   
} SIMAP_event_deactivate_cnf_struct;

typedef struct _SIMAP_event_disconnect_ind_struct
{        
    kal_uint8 cid;  /* connection id */
} SIMAP_event_disconnect_ind_struct;

typedef struct _SIMAP_event_disconnect_cnf_struct
{        
    kal_bool result;
    kal_uint8 cid;  /* connection id */
} SIMAP_event_disconnect_cnf_struct;

typedef struct _SIMAP_event_auth_req_struct
{        
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
} SIMAP_event_auth_req_struct;

typedef struct _SIMAP_event_card_status_ind_sent_ind_struct
{
    SimCardStatus status;
} SIMAP_event_card_status_ind_sent_ind_struct;

typedef struct _SIMAP_event_auth_req_timeout_rfclose_ind_struct
{        
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
} SIMAP_event_auth_req_timeout_rfclose_ind_struct;

typedef struct _SIM_event_apdu_req_struct
{        
    kal_uint8 transport_protocol_type;
    kal_uint16 apdu_req_len;
    kal_uint8 apdu_req[APDU_REQ_MAX_LEN];
} SIM_event_apdu_req_struct;

typedef struct _SIM_event_reset_req_struct
{        
    kal_uint8 transport_protocol_type;
} SIM_event_reset_req_struct;

typedef struct _SIMAPCallbackParms
{
    SIMAP_event event; /* Type of the SIM Access event */
    module_type dstMod;

    union
    {
       /* SIMAP --> MMI */
    	SIMAP_event_connect_ind_struct simap_connect_ind;
       SIMAP_event_activate_cnf_struct simap_activate_cnf;
    	SIMAP_event_deactivate_cnf_struct simap_deactivate_cnf;
    	SIMAP_event_disconnect_ind_struct simap_disconnect_ind;
    	SIMAP_event_disconnect_cnf_struct simap_disconnect_cnf;
    	SIMAP_event_auth_req_struct simap_auth_req;
    	SIMAP_event_card_status_ind_sent_ind_struct simap_card_status_ind_sent_ind;
    	SIMAP_event_auth_req_timeout_rfclose_ind_struct simap_auth_req_timeout_rfclose_ind;

	/* SIMAP --> SIM task */
    	SIM_event_apdu_req_struct sim_apdu_req;
	SIM_event_reset_req_struct sim_reset_req;
    } p;
}SIMAPCallbackParms;

typedef void (*SIMAPCallback) (SIMAPCallbackParms *Parms);

/* Structure for internal use only */
typedef struct _SimStager
{
    U8 *buff;
    U16 cur;
    U16 target;
    BOOL skip;
} SimStager;

/* Structure for internal use only */
typedef struct _SimMsgParser
{
    SimStager stager;
    U8 stageBuff[4];
    U8 curParm;
    U8 state;
    U8 parmsExpected[SIM_MAX_MSG_PARMS];
    U8 parmsExpectedLen;
    SimMsg *msg;
} SimMsgParser;

#if SIM_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * SimServer structure
 *
 *     The server channel.  This structure is used to identify a SIM Access
 *     Server.
 */
struct _SimServer
{

    /* === Internal use only === */
    /* Add supported_by_sim filed: to determine if this entry (ID_x,x=0-7: multiple cards are possible) is supported by sim card reader */
    BOOL supported_by_sim;
    /* Ad available field: to determine this if this simServer is already connected by one client or not: 2006-10-27 */
    BOOL available;             /* It will be cleaned up as TRUE when is not used by original client */
    /* used_card_id this field : is used to identify which id of card is inserted in card reader of this SIM Server */
    U8 used_card_id;
    ListEntry node;
    BtRemoteDevice *remDev;
    RfChannel rfChannel;
    /* Add cmgrHandler field */
    CmgrHandler cmgrHandler;
    U8 previous_security_mode; 

    /* Add some fields to record the information returen from SIM task:2006-10-16 */
    /* activate_status to determine the following status: not_activated/activated/deactivating/deactivated */

    U8 exec_mode;                                       /* Using fake sim or SIM task: 2006-11-28 */
    U8 discType;                                        /* Store the discType: Graceful disc or Immediate disc Type */
#ifdef __SIMAP_UT_
    SimResultCode result_from_simtask;
    SimResultCode concode;                              /* result code of CONNECT_RSP from fake sim */
    SimResultCode resetcode;                            /* result code of RESET_RSP from fake sim */
    SimResultCode disccode;                             /* result code of DISCCONNECT_RSP from fake sim */
    SimResultCode offcode;                              /* result code of POWEROFF_RSP from fake sim */
    SimResultCode oncode;                               /* result code of POWERON_RSP from fake sim */
    SimResultCode tcode;                                /* result code of SET_T_RSP from fake sim */
#endif /* __SIMAP_UT_ */ 
    U8 connect_mode;
    U8 simserver_activate_status;
    U8 current_transport_protocol_type;
    U8 supported_transport_protocol_type_capability;
    U8 client_requested_transport_protocol_type;
    SimCardReaderStatus current_simcardreader_status;   /* only bit8 and bit7 is possible to set in our current design */
    SimAtr atr;                                         /* Record the ATR_RSP from the SIM Task module */

    /* Internal packet variable */
    SimPacket simPacketList[NUM_OF_SIMPACKETS]; /*
                                                 * Org ESI codes: DISC_IND with graceful type also uses simPacket, 
                                                 * but if simPacket is also used and wait for SIM task's rsp(e.g., wait reset_cnf for set transport protocol)
                                                 * then MMI sends DISC_IND with graceful type also try to use simPacke with flag BUSY,
                                                 * It will cause error for MMI operation
                                                 */

    SimPacket simPacket;
    SimPacket discPacket;       /* Comment: used for DISCONNECT_IND with immediate type and DiscRsp packet */
    SimPacket discGracePacket;  /* used only for DISCONNECT_IND with graceful type and this packet returned in open state */

    /* SDP variables for server */
    U8 sdpProtoDescList[14];
    SdpAttribute sdpAttribute[5];
    SdpRecord sdpRecord;

    /* Other state variables */
    SimServerCallback callback;
    U16 flags;      /* It will be cleaned up in SimServerClearConnection() */
    U8 state;       /* It will be cleaned up in SimServerClearConnection() */
    /* In each state, it will expect to recive certain request or response */
    U16 substate;

    /* Parser state information */
    SimMsgParser parser;

    /* Parser message structure */
    SimMsg msg;

};
#endif /* SIM_SERVER == XA_ENABLED */

#if SIM_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SimClient structure
 *
 *     The client channel.  This structure is used to identify a SIM Access
 *     Client.
 */
struct _SimClient
{

    /* === Internal use only === */
    ListEntry node;
    RfChannel rfChannel;
    U8 rfServerChannel;
    CmgrHandler cmgrHandler;

    /* SDP variables for client */
    SdpQueryToken sdpQueryToken;
    BOOL foundChannel;
    U8 sdpSearchAttribReq[18];

    /* Internal packet variable */
    SimPacket simPacket;

    /* Other state variables */
    SimClientCallback callback;
    U16 flags;
    U8 state;

    /* Parser message structure */
    SimMsg msg;

    /* Parser state information */
    SimMsgParser parser;
};
#endif /* SIM_CLIENT == XA_ENABLED */

/****************************************************************************
 *
 * Section: Server APIs
 *
 ****************************************************************************/

#if SIM_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * SIM_RegisterServer()
 *
 *     Registers a server for receiving SIM Access connections.  This function
 *     creates the required SDP entries based on the service that is being
 *     registered.  The Major Service Class of the Class of Device is also
 *     registered.  The Major and Minor Device Classes of the Class of Device
 *     are not registered, however.  The application or management software
 *     must register the appropriate values to be compliant with the
 *     SIM Access Profile.
 *
 * Parameters:
 *     Server - Server to register.  This structure is owned by the SIM Access 
 *     layer until the server is deregistered.
 *
 *
 *     Callback - Callback function for receiving events related to the
 *         specified server.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The server was successfully registered.
 *
 *     BT_STATUS_FAILED - The server could not be registered, probably 
 *         because it is already in use.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_USE - The Server structure is already in use 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - Could not register a channel with RFCOMM.
 */
BtStatus SIM_RegisterServer(SimServer *Server, SimServerCallback Callback);

/*---------------------------------------------------------------------------
 * SIM_DeregisterServer()
 *
 *     Deregisters a SIM Access server.  All SDP entries are removed.
 *
 * Parameters:
 *     Server - Server to deregister.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The server was successfully deregistered.
 *
 *     BT_STATUS_BUSY - The server has an open channel. All channels
 *         must be successfully closed with SIM_ServerClose before 
 *         calling this function (XA_ERROR_CHECK only).
 * 
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_DeregisterServer(SimServer *Server);

/*---------------------------------------------------------------------------
 * SIM_ServerRespondOpenInd()
 *
 *     Accepts or rejects an incoming connection in response to an 
 *     SAEVENT_SERVER_OPEN_IND event.  This event occurs when a remote client 
 *     attempts to connect to a registered server. This function must be 
 *     used to respond to the connection request.
 *
 * Parameters:
 *     Server - Identifies the server that is accepting the connection.  This 
 *         server is provided to the callback function as a parameter during the
 *         SAEVENT_SERVER_OPEN_IND event.
 *
 *     Status - Identifies the connection status response.  If Status is 
 *         set to SIM_CONN_STATUS_OK, the connection is accepted.  When the 
 *         connection is established, the SAEVENT_SERVER_OPEN event will be 
 *         received.  Previous to the SAEVENT_SERVER_OPEN event, a 
 *         SAVEVENT_SERVER_STATUS_REQ event will be received.  The application 
 *         must respond to this by sending the current card status by calling 
 *         SIM_ServerSendStatus.  The application may also receive a 
 *         SAEVENT_SERVER_ATR_REQ event.  If this event is received, the 
 *         application must call SIM_ServerAtrRsp with the ATR response.
 *
 *         If the Status is set to SIM_CONN_STATUS_UNABLE, the connection is
 *         rejected and the connection remains closed.
 *
 * Returns:
 *     BT_STATUS_PENDING - The accept message will be sent. The application
 *         will receive a SAEVENT_SERVER_OPEN when the accept message has been 
 *         sent and the channel is open.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - The specified server did not have a pending connection
 *         request (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection to accept.
 */
BtStatus SIM_ServerRespondOpenInd(SimServer *Server, SimConnStatus Status);

/*---------------------------------------------------------------------------
 * SIM_ServerSendStatus()
 *
 *     Sends the current status of the SIM card to the client.  Whenever the 
 *     the status of the card changes this function must be called to notify 
 *     the client.  
 *
 *     In addition, during the connection process the application will be
 *     prompted for the card status with a SAEVENT_SERVER_STATUS_REQ event.
 *     The application must respond to this request before the connection
 *     process can continue.
 *
 *     See the SIM Access profile for more information on the use of server
 *     status.
 *
 * Parameters:
 *     Server - Identifies the server that is sending status.
 *
 *     Status - Indicates the status of the card (see SimCardStatus).
 *
 * Returns:
 *     BT_STATUS_PENDING - The status was sent. The application will receive
 *         a SAEVENT_SERVER_STATUS_SENT event when the status has been sent to 
 *         the client.
 *
 *     BT_STATUS_FAILED - There was no open connection for sending status.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 *
 */
BtStatus SIM_ServerSendStatus(SimServer *Server, SimCardStatus Status);

/*---------------------------------------------------------------------------
 * SIM_ServerAtrRsp()
 *
 *     Send an ATR response to a client's request.  When a 
 *     SAEVENT_SERVER_ATR_REQ is received, the server should respond using
 *     this function.
 *
 *     See the SIM Access profile for more information on the use of ATR
 *     responses.
 *
 * Parameters:
 *     Server - Identifies the server that is responding.
 *
 *     Len - Contains the length of the ATR response.
 *
 *     Atr - Contains a pointer to the ATR response
 *
 *     Result - Contains the result of ATR command (see SimResultCode).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The response was sent successfully to the remote
 *         device.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_FAILED - The response could not be sent because no request
 *         was outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting data 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerAtrRsp(SimServer *Server, U16 Len, U8 *Atr, SimResultCode Result);

/*---------------------------------------------------------------------------
 * SIM_ServerApduRsp()
 *
 *     Send an APDU response to a client's request.  When a 
 *     SAEVENT_SERVER_APDU_REQ is received, the server should respond using
 *     this function.
 *
 *     See the SIM Access profile for more information on the use of APDU
 *     responses.
 *
 * Parameters:
 *     Server - Identifies the server for this action.
 *
 *     Len - Identifies the length of the APDU.
 *
 *     Ptr - A pointer to the APDU data.
 *
 *     Result - Contains the result of APDU command (see SimResultCode).
 *
 * Returns:    
 *     BT_STATUS_SUCCESS - The response was sent successfully to the remote
 *         device.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_FAILED - The response could not be sent because no request
 *         was outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting data 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerApduRsp(SimServer *Server, U16 Len, U8 *Apdu, SimResultCode Result);

/*---------------------------------------------------------------------------
 * SIM_ServerSimOnRsp()
 *
 *     Send an SIM ON response to a client's request.  When a 
 *     SAEVENT_SERVER_SIM_ON_REQ is received, the server should respond using
 *     this function.  If the SIM card is off, this function must be called 
 *     after the SIM has been turned on.  If the SIM card was already on, or
 *     if an error occurs when turning on the SIM, then this function must
 *     be called with the appropriate result code.
 *
 *     See the SIM Access profile for more information on the use of SIM ON
 *     responses.
 *
 * Parameters:
 *     Server - Identifies the server that is responding.
 *
 *     Result - Contains the result of SIM ON command (see SimResultCode).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The response was sent successfully to the remote
 *         device.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_FAILED - The response could not be sent because no request
 *         was outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting data 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerSimOnRsp(SimServer *Server, SimResultCode Result);

/*---------------------------------------------------------------------------
 * SIM_ServerSimOffRsp()
 *
 *     Send an SIM OFF response to a client's request.  When a 
 *     SAEVENT_SERVER_SIM_OFF_REQ is received, the server should respond using
 *     this function.  If the SIM card is on, this function must be called 
 *     after the SIM has been turned off.  If the SIM card was already off, or
 *     if an error occurs when turning the SIM off, then this function must
 *     be called with the appropriate result code.
 *
 *     See the SIM Access profile for more information on the use of SIM OFF
 *     responses.
 *
 * Parameters:
 *     Server - Identifies the server that is responding.
 *
 *     Result - Contains the result of SIM OFF command (see SimResultCode).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The response was sent successfully to the remote
 *         device.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_FAILED - The response could not be sent because no request
 *         was outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting data 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerSimOffRsp(SimServer *Server, SimResultCode Result);

/*---------------------------------------------------------------------------
 * SIM_ServerResetSimRsp()
 *
 *     Send an RESET SIM response to a client's request.  When a 
 *     SAEVENT_SERVER_RESET_SIM_REQ is received, the server should respond using
 *     this function.  The SIM card should be reset (if possible) before
 *     responding to this request.  If an error occurs when resetting the SIM, 
 *     then this function must be called with the appropriate result code.
 *
 *     See the SIM Access profile for more information on the use of RESET SIM
 *     responses.
 *
 * Parameters:
 *     Server - Identifies the server that is responding.
 *
 *     Result - Contains the result of RESET SIM command (see SimResultCode).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The response was sent successfully to the remote
 *         device.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_FAILED - The response could not be sent because no request
 *         was outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting data 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerResetSimRsp(SimServer *Server, SimResultCode Result);

/*---------------------------------------------------------------------------
 * SIM_ServerCardReaderStatusRsp()
 *
 *     Send an CARD READER STATUS response to a client's request.  When a 
 *     SAEVENT_SERVER_CARD_READER_STATUS_REQ is received, the server should 
 *     respond using this function.
 *
 *     See the SIM Access profile for more information on the use of CARD
 *     READER STATUS responses.
 *
 * Parameters:
 *     Server - Identifies the server that is responding.
 *
 *     CardRdrStatus - Contains the status of the card reader (see 
 *         SimCardReaderStatus).
 *
 *     Result - Contains the result of CARD READER STATUS command (see 
 *         SimResultCode).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The response was sent successfully to the remote
 *         device.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_FAILED - The response could not be sent because no request
 *         was outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting data 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerCardReaderStatusRsp(SimServer *Server, SimCardReaderStatus CardRdrStatus, SimResultCode Result);

/*---------------------------------------------------------------------------
 * SIM_ServerSetTransProtRsp()
 *
 *     Responds to the client's request to set the transport protocol.

 *     See the SIM Access profile for more information on the use of 
 *     the Set Transport Protocol response.
 *
 * Parameters:
 *     Server - Identifies the server that is responding.
 *
 *     Result - Contains the result of Set Transport Protocol command (see 
 *         SimResultCode).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The response was sent successfully to the remote
 *         device.
 *
 *     BT_STATUS_BUSY - A response is already in progress.
 *
 *     BT_STATUS_FAILED - The response could not be sent because no request
 *         was outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists for transmitting data 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerSetTransProtRsp(SimServer *Server, SimResultCode Result);

/*---------------------------------------------------------------------------
 * SIM_ServerClose()
 *
 *     Closes a SIM Access connection between two devices.  When the connection
 *     is closed, the application will receive an SAEVENT_SERVER_CLOSED event.
 *
 * Parameters:
 *     Server - Identifies the server connection to be closed. The 
 *         SAEVENT_SERVER_CLOSED event indicates that the connection is closed 
 *         and a new client may attempt to connect to the server.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request to close the connection will be sent.
 *         The application will receive an SAEVENT_SERVER_CLOSED event when the
 *         connection is closed.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only). 
 *
 *     BT_STATUS_BUSY - The server is already in the process of disconnecting.
 *
 *      BT_STATUS_NO_CONNECTION - No connection exists on this Server 
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Server was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ServerClose(SimServer *Server, SimDisconnectType Type);

#endif /* SIM_SERVER == XA_ENABLED */

/****************************************************************************
 *
 * Section: Client APIs
 *
 ****************************************************************************/

#if SIM_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SIM_RegisterClient()
 *
 *     Registers a client for establishing SIM Access connections.  
 *
 * Parameters:
 *     Client - Client to register.  This structure is owned by the SIM Access 
 *     layer until the client is deregistered.
 *
 *     Callback - Callback function for receiving events related to the
 *         specified client.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The client was successfully registered.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_USE - The Client structure is already in use 
 *         (XA_ERROR_CHECK only).
 */
BtStatus SIM_RegisterClient(SimClient *Client, SimClientCallback Callback);

/*---------------------------------------------------------------------------
 * SIM_DeregisterClient()
 *
 *     Deregisters a SIM Access client.
 *
 * Parameters:
 *     Client - Client to deregister.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The Client was successfully deregistered.
 *
 *     BT_STATUS_BUSY - The Client has an open channel. All channels
 *         must be successfully closed with SIM_ClientClose before 
 *         calling this function (XA_ERROR_CHECK only).
 * 
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_DeregisterClient(SimClient *Client);

/*---------------------------------------------------------------------------
 * SIM_ClientOpen()
 *
 *     Attempts to establish a connection with a remote SIM Access Server.
 * 
 * Parameters:
 *     Client - A registered Client structure for this action.
 *
 *     Addr - Address of remote device, or 0 to allow the connection
 *         manager to select a device.
 *
 * Returns:
 *     BT_STATUS_FAILED - An error occurred and the connection could not be
 *         opened.
 * 
 *     BT_STATUS_PENDING - The request to open the client was sent.
 *         If the request is accepted by the remote device, a 
 *         SAEVENT_CLIENT_OPEN event will be generated. If the connection is 
 *         rejected, a SAEVENT_CLIENT_CLOSED event will be generated.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_NO_CONNECTION - No ACL connection exists.
 */
BtStatus SIM_ClientOpen(SimClient *Client, BD_ADDR *Addr);

/*---------------------------------------------------------------------------
 * SIM_ClientAtrReq()
 *
 *     Send an ATR request to a server.
 *
 * Parameters:
 *     Client - Identifies the Client for this action.
 *
 *     See the SIM Access profile for more information on the use of ATR
 *     requests.
 *
 * Returns:    
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_APDU_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientAtrReq(SimClient *Client);

/* Internal function */
BtStatus SimClientApduReq(SimClient *Client, U16 Len, U8 *Apdu, SimApduType type);

/*---------------------------------------------------------------------------
 * SIM_ClientApduReq()
 *
 *     Sends a GSM-style APDU request to the server.
 *
 *     See the SIM Access profile for more information on the use of APDU
 *     requests.
 *
 * Parameters:
 *     Client - Identifies the Client for this action.
 *
 *     Len - Identifies the length of the APDU.
 *
 *     Ptr - A pointer to the APDU data.
 *
 * Returns:    
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_APDU_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientApduReq(SimClient *Client, U16 Len, U8 *Apdu);

#define SIM_ClientApduReq(c,l,a) SimClientApduReq(c,l,a,SIM_APDU_GSM)

/*---------------------------------------------------------------------------
 * SIM_ClientApdu7816Req()
 *
 *     Sends an IEC 7816-4 style APDU request to the server.
 *
 *     See the SIM Access profile for more information on the use of APDU
 *     requests.
 *
 * Parameters:
 *     Client - Identifies the Client for this action.
 *
 *     Len - Identifies the length of the APDU.
 *
 *     Ptr - A pointer to the APDU data.
 *
 * Returns:    
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_APDU_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientApdu7816Req(SimClient *Client, U16 Len, U8 *Apdu);

#define SIM_ClientApdu7816Req(c,l,a) SimClientApduReq(c,l,a,SIM_APDU_IEC_7816)

/*---------------------------------------------------------------------------
 * SIM_ClientSimOnReq()
 *
 *     Send an SIM ON request to the server.  When the request is sent, the 
 *     server should respond after powering on the SIM card.
 *
 *     See the SIM Access profile for more information on the use of SIM ON
 *     requests.
 *
 * Parameters:
 *     Client - Identifies the client sending the request.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_SIM_ON_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientSimOnReq(SimClient *Client);

/*---------------------------------------------------------------------------
 * SIM_ClientSimOffReq()
 *
 *     Send an SIM OFF request to the server.  When the request is sent, the 
 *     server should respond after powering off the SIM card.
 *
 *     See the SIM Access profile for more information on the use of SIM OFF
 *     requests.
 *
 * Parameters:
 *     Client - Identifies the client sending the request.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_SIM_OFF_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientSimOffReq(SimClient *Client);

/*---------------------------------------------------------------------------
 * SIM_ClientResetSimReq()
 *
 *     Send a Reset SIM request to the server.  When the request is sent, the 
 *     server should respond after resetting the SIM.
 *
 *     See the SIM Access profile for more information on the use of Reset SIM
 *     requests.
 *
 * Parameters:
 *     Client - Identifies the client sending the request.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_RESET_SIM_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientResetSimReq(SimClient *Client);

/*---------------------------------------------------------------------------
 * SIM_ClientCardRdrStatusReq()
 *
 *     Send a Card Reader Status request to the server.  When the request is 
 *     sent, the server should respond with the status of the Card Reader.
 *
 *     See the SIM Access profile for more information on the use of Card Reader
 *     status requests.
 *
 * Parameters:
 *     Client - Identifies the client sending the request.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_CARD_READER_STATUS_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientCardRdrStatusReq(SimClient *Client);

/*---------------------------------------------------------------------------
 * SIM_ClientSetTransProtReq()
 *
 *     Requests server to set its transport protocol ID value.
 *
 *     See the SIM Access profile for more information on the use of the
 *     Set Transport Protocol request.
 *
 * Parameters:
 *     Client - Identifies the client sending the request.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request was sent successfully to the remote
 *         device.  When a response is received from the Server, a
 *         SAEVENT_CLIENT_TRANS_PROT_RESP event will be received.
 *
 *     BT_STATUS_FAILED - Either no connection exists for transmitting data, or
 *         a request is already outstanding.
 *
 *     BT_STATUS_BUSY - A request is already in progress.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 */
BtStatus SIM_ClientSetTransProtReq(SimClient *Client, U8 id);

/*---------------------------------------------------------------------------
 * SIM_ClientClose()
 *
 *     Closes a SIM Access connection between two devices.  When the connection
 *     is closed, the application will receive an SAEVENT_CLIENT_CLOSED event.
 *
 *     If there are any outstanding requests when a connection is closed,
 *     an event will be received for each request with the status code set to 
 *     BT_STATUS_NO_CONNECTION.
 *
 * Parameters:
 *     Client - Identifies the Client connection to be closed. The 
 *         SAEVENT_CLIENT_CLOSED event indicates that the connection is closed 
 *         and a new client may attempt to connect to the Client.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request to close the connection will be sent.
 *         The application will receive an SAEVENT_CLIENT_CLOSED event when the
 *         connection is closed.
 *
 *     BT_STATUS_BUSY - The Client is already in the process of disconnecting.
 *
 *     BT_STATUS_IN_PROGRESS - RFCOMM is already in the process of closing.
 *
 *     BT_STATUS_FAILED - The channel is not in the correct state.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only). 
 *
 *     BT_STATUS_NOT_FOUND - The specified Client was not found (XA_ERROR_CHECK 
 *         only).
 *
 *     BT_STATUS_NO_CONNECTION - No connection exists on this Client.
 */
BtStatus SIM_ClientClose(SimClient *Client);

#endif /* SIM_CLIENT == XA_ENABLED */

/* simap_api.c */
/* Define Init function for SIM Access Profile */
void bt_simap_config_simserver(void);
SimServer *bt_simap_allocated_one_simserver(U8 CARD_ID);
void bt_simap_activate_simserver_status(void);
void bt_simap_deactivate_simserver_status(void);
void bt_simap_activate_req_hdlr(void);
void bt_simap_deactivate_req_hdlr(void);
void bt_simap_auth_rsp_hdlr(ilm_struct *ilm_ptr);
void bt_simap_disconnect_req_hdlr(ilm_struct *ilm_ptr);
void bt_sim_connect_cnf_hdlr(ilm_struct *ilm_ptr);
void bt_sim_openstate_reset_cnf_hdlr(ilm_struct *ilm_ptr);
void bt_sim_openstate_power_on_cnf_hdlr(ilm_struct *ilm_ptr);
void bt_sim_openstate_set_transport_protocol_cnf_hdlr(ilm_struct *ilm_ptr);
void bt_sim_reset_cnf_hdlr(ilm_struct *ilm_ptr);
void bt_sim_apdu_cnf_hdlr(ilm_struct *ilm_ptr);
void bt_sim_disconnect_cnf_hdlr(ilm_struct *ilm_ptr);
void bt_sim_power_off_cnf_hdlr(ilm_struct *ilm_ptr);

void bt_simap_init(void);
void bt_simap_deinit(void);
void bt_simap_reset(void);

#ifdef __SIMAP_REG_GLOBAL_HDLER__
void Me2SimApCallback(const BtEvent *Event);
#endif /* __SIMAP_REG_GLOBAL_HDLER__ */ 
extern void bt_simap_activate(void);
extern void bt_simap_deactivate(void);
void bt_simap_wait_atr_in_connect_procedure_timeout_handler(void *timeout_cb_input);
void bt_simap_wait_req_in_graceful_disconnect_procedure_timeout_handler(void *timeout_cb_input);

/* sim_server.c */
void SIMAP_Register(SIMAPCallback Callback);
void SimSrvCallback(SimServer *Server, SimCallbackParms *Parms);
void SimSrvSetGracefulDiscType(void);
void SimSrvSetImmediateDiscType(void);
void SimSrvSimCardUnavailableSendStatusIndvoid(void);

#ifdef __SIMAP_UT_
/* simaptester.c */
void simaptester_handler(ilm_struct *ilm_ptr);
#endif


#endif /* __BT_SIM_H */ 

