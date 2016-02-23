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

#ifndef __BT_SIMI_H
#define __BT_SIMI_H
/*****************************************************************************
 *
 * File:
 *     $Workfile:simi.h$
 *     $Revision: #1 $
 *
 * Description: Private types, defines, and prototypes for use by the SIM Access
 *              profile.
 * 
 * Created:     May 2, 2002
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

#include "rfcomm_adp.h"
#include "sdp_adp.h"
#include "bt_sim.h"

/* Message Types */
#define SIM_CONNECT_REQ              0x00
#define SIM_CONNECT_RSP              0x01
#define SIM_DISCONNECT_REQ           0x02
#define SIM_DISCONNECT_RSP           0x03
#define SIM_DISCONNECT_IND           0x04
#define SIM_TRANSFER_APDU_REQ        0x05
#define SIM_TRANSFER_APDU_RSP        0x06
#define SIM_TRANSFER_ATR_REQ         0x07
#define SIM_TRANSFER_ATR_RSP         0x08
#define SIM_POWER_OFF_REQ            0x09
#define SIM_POWER_OFF_RSP            0x0A
#define SIM_POWER_ON_REQ             0x0B
#define SIM_POWER_ON_RSP             0x0C
#define SIM_RESET_REQ                0x0D
#define SIM_RESET_RSP                0x0E
#define SIM_CARD_RDR_STATUS_REQ      0x0F
#define SIM_CARD_RDR_STATUS_RSP      0x10
#define SIM_STATUS_IND               0x11
#define SIM_ERROR_RSP                0x12
#define SIM_SET_TRANS_PROT_REQ       0x13
#define SIM_SET_TRANS_PROT_RSP       0x14

/* Internal message types */
#define SIM_ERROR_IND                0x80
#define SIM_PACKET_HANDLED           0x81
#define SIM_TRANSPORT_OPEN           0x82

/* Internal conn status */
#define SIM_CONN_STATUS_SERVER_UNABLE_ESTABLISH_CONNECTION  1
#define SIM_CONN_STATUS_MSG_SIZE     2
#define SIM_CONN_STATUS_SMALL_MSG    3
#define SIM_CONN_STATUS_ERROR        4

/* Parameter Types */
#define SIM_PARM_MAX_MSG_SIZE        0x00
#define SIM_PARM_CONN_STATUS         0x01
#define SIM_PARM_RESULT_CODE         0x02
#define SIM_PARM_DISC_TYPE           0x03
#define SIM_PARM_CMD_APDU            0x04
#define SIM_PARM_RSP_APDU            0x05
#define SIM_PARM_ATR                 0x06
#define SIM_PARM_CARD_RDR_STATUS     0x07
#define SIM_PARM_STATUS_CHANGE       0x08
#define SIM_PARM_TRANS_PROT          0x09       /* TBD */
#define SIM_PARM_CMD_APDU_7816       0x10

#define SIM_PARM_INVALID             0xFF       /* Internal code */

#if SIM_CLIENT == XA_ENABLED
/* Flags for client states */
#define SIM_CLIENT_FLAG_QUERY_SDP       0x0001
#define SIM_CLIENT_FLAG_STATUS          0x0002
#define SIM_CLIENT_FLAG_ATR             0x0004
#define SIM_CLIENT_FLAG_APDU            0x0008
#define SIM_CLIENT_FLAG_SIM_ON          0x0010
#define SIM_CLIENT_FLAG_SIM_OFF         0x0020
#define SIM_CLIENT_FLAG_RESET_SIM       0x0040
#define SIM_CLIENT_FLAG_CARD_RDR_STATUS 0x0080
#define SIM_CLIENT_FLAG_SET_TRANS_PROT  0x0100
#define SIM_CLIENT_FLAG_PROCESSING      0x8000

/* Client states. Must align with SimInitStateMachine */
#define SIM_CLIENT_STATE_CLOSED       0
#define SIM_CLIENT_STATE_CONN_PENDING 1
#define SIM_CLIENT_STATE_OPEN         2
#define SIM_CLIENT_STATE_DISC_PENDING 3

#endif /* SIM_CLIENT == XA_ENABLED */

#if SIM_SERVER == XA_ENABLED
/* Flags for server states: for peer's (BT client between BT server) message used flags */
/* 
 * These flags are turned on when receiving client's request, and cleared when call Sim_XXXRsp() to send corresponding response to the client
 */
#define SIM_SERVER_FLAG_STATUS          0x0001
#define SIM_SERVER_FLAG_ATR             0x0002
#define SIM_SERVER_FLAG_APDU            0x0004
#define SIM_SERVER_FLAG_SIM_ON          0x0008
#define SIM_SERVER_FLAG_SIM_OFF         0x0010
#define SIM_SERVER_FLAG_RESET_SIM       0x0020
#define SIM_SERVER_FLAG_CARD_RDR_STATUS 0x0040
#define SIM_SERVER_FLAG_STATUS_PENDING  0x0080
#define SIM_SERVER_FLAG_SET_TRANS_PROT  0x0100
#define SIM_SERVER_FLAG_PROCESSING      0x8000

/* Server states. Must align with SimInitStateMachine */
#define SIM_SERVER_STATE_CLOSED       0
#define SIM_SERVER_STATE_CONN_PENDING 1
#define SIM_SERVER_STATE_OPEN         2
#define SIM_SERVER_STATE_DISC_PENDING 3

/* Add substates used in each state to record the interaction with the SIM task module */
/*
 * The purpose of these substates are different from the flags defined previously
 * These are used to:
 * 1. Check the interactions with Sim task during connection setup procedure
 * 2. Check which rsp packet is already sent to client successfully when in SimServerOpen state
 *     -> Some interactions in open state, must check the req/rsp sequences, so these substates are helpful . 
 *     E.g., After sending SET_TRANSPORT_PROTOCOL_TYPE_RSP with result = OK, (ie. packet handled event is reported to upper layer),
 *             the STATUS_IND  must be also sent to client!
 * [Note]
 * In Connection Setup procedure, the field substate must be recorded when interacting with SIM task
 * But in most case in open state, the substate is not used, because for most request, only its corresponding rsp sent is enough!
 * SET_TRANSPORT_PROTOCOL_TYPE is a special case !  
 */
#define BT_SIM_CONNECT_REQ_SENT 0x0001
#define BT_SIM_CONNECT_CNF_RECV 0x0002
#define BT_SIM_CONNECT_RESET_REQ_SENT   0x0004  /* This flag state is due to recevie client's RESET_REQ in open state and connection procedure */
#define BT_SIM_CONNECT_RESET_CNF_RECV   0x0008
#define BT_SIM_CONNECT_PROCEDURE_COMPLETED  0x0010

/*
 * When the connection procedure is completed, simServer is in OPEN state!
 * The following defined constants are used in open state 
 */

/* The following three commands related to reset are used in simServer's open state */
#define BT_SIM_RESET_REQ_SENT 0x0020    /* This flag state is used in simServer's open state */
#define BT_SIM_RESET_REQ_SENT_POWER_ON  0x0040  /* This flag state () is due to receive client's POWER_ON_REQ and send this command to sim task */
#define BT_SIM_RESET_REQ_SENT_SET_T 0x0080      /* This flag state is due to receive client's SET_TRANSPORT_PROTOCOL_REQ and send this command to sim task */

#define BT_SIM_APDU_REQ_SENT    0x0100
#define BT_SIM_POWER_OFF_REQ_SENT   0x0200
#define BT_SIM_DISCONNECT_REQ_SENT  0x0400

/* For MMI: When disconnection procedure is completed, if this flag is set, notify MMI with DISCONNECT_CNF */
#define BT_APP_DISCONNECT_REQ_RECV  0x0800
#define BT_APP_DEACTIVATE_REQ_RECV  0x1000

/* Add this substate to ask upper layer to accept SAP connection or not */
#define BT_MMI_AUTH_REQ_SENT             0x2000

/* Add this substate to ensure the car kit will resend msg to config msg */
#define BT_SIMAP_WAIT_RETRY_MTU       0x4000

/*
 * Add this substate for detecting closed event is casued by rejecting another client session : 2006-12-29
 * This substate is used to avoid to send DISC_REQ to SIM task due to the first session has been connected but close procedure is still ongoing
 */
#define BT_REJECT_SECOND_CLIENT_SESSION 0x8000

/* Activate_Status */
#define SIM_SERVER_STATUS_NOT_DEACTIVATED 0
#define SIM_SERVER_STATUS_ACTIVATED 1
#define SIM_SERVER_STATUS_DEACTIVATING 2
#define SIM_SERVER_STATUS_DEACTIVATED 3
/* 
 * [MAUI_01811752]
 * Bingyi 2009.12.16: use SIM_SERVER_STATUS_DEACTIVATING the following case that 
 * 1. AUTH_REQ is rejected
 * 2. DEACTIVATE is call by MMI
 * 3. RFCOMM channel has not been closed
 * ==> the DEACTIVATE will be delayed after RFCOMM channel has been closed.
 */

#endif /* SIM_SERVER == XA_ENABLED */

/* Parser States */
#define SIM_PARSE_HEADER              0
#define SIM_PARSE_PARM                1
#define SIM_PARSE_PARM_HEADER         2
#define SIM_PARSE_PARM_VALUE          3
#define SIM_PARSE_PARM_PAD            4

/* Transport state */
#define SIM_TRANSPORT_DOWN  0
#define SIM_TRANSPORT_UP    1

/* Types for state functions */
#if SIM_SERVER == XA_ENABLED
typedef void (*SimServerState) (SimServer *Server, SimMsg *Event);
#endif /* SIM_SERVER == XA_ENABLED */

#if SIM_CLIENT == XA_ENABLED
typedef void (*SimClientState) (SimClient *Client, SimMsg *Event);
#endif /* SIM_CLIENT == XA_ENABLED */

/* SIM Access Context */
typedef struct _BtSimContext
{

#if SIM_SERVER == XA_ENABLED
    ListEntry serverList;
    SimServer simServer[MAX_NUM_SIM_SERVER];
    U8 connect_id[MAX_NUM_SIM_SERVER];
    kal_bool allocated_connect_id[MAX_NUM_SIM_SERVER];
    SimServerState serverState[4];
    RfService rfService;
    U8 serviceCount;
    BtSecurityRecord secRec;

#endif /* SIM_SERVER == XA_ENABLED */

#if SIM_CLIENT == XA_ENABLED
    ListEntry clientList;
    SimClientState clientState[4];
#endif /* SIM_CLIENT == XA_ENABLED */

    U8 transportState;
#ifdef __SIMAP_MAUI__
    eventid simap_event_id; /* Using this event_id to represent event of event scheduler */
#else
    EvmTimer simap_event_id;
#endif
} BtSimContext;

/* Context Pointer declarations */
#if XA_CONTEXT_PTR == XA_ENABLED
#define SIM(field)         (simContext->field)
extern BtSimContext *simContext;
#else /* XA_CONTEXT_PTR == XA_ENABLED */ 
#define SIM(field)         (simContext.field)
extern BtSimContext simContext;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

/* SIM State Machine functions */
void SimInitStateMachine(void);

/* Send routines */
BtStatus SimSendPacket(
            SimPacket *SPacket,
            RfChannel *Channel,
            U8 MsgType,
            U8 Parm1Type,
            U16 Parm1Len,
            U8 *Parm1,
            U8 Parm2Type,
            U16 Parm2Len,
            U8 *Parm2);

BtStatus SimFindFreeAndSendPacket(
            SimServer *Server,
            RfChannel *Channel,
            U8 MsgType,
            U8 Parm1Type,
            U16 Parm1Len,
            U8 *Parm1,
            U8 Parm2Type,
            U16 Parm2Len,
            U8 *Parm2);

BtStatus SimSendPacketSegment(SimPacket *SPacket, RfChannel *Channel);

/* Stager routines */

/* Sets the stager to read "stageLen" input bytes into "stageBuff" */
void SimStageInit(SimStager *stager, U8 *stageBuff, U16 stageLen);

#define SimStageInit(s,b,l) do { (s)->cur = 0; (s)->target = (l); (s)->skip=FALSE; (s)->buff = (b); } while (0)

/* Sets the stager to skip over "stageLen" input bytes */
void SimStageSkip(SimStager *stager, U16 stageLen);

#define SimStageSkip(s,l) do { (s)->cur = 0; (s)->target = (l); (s)->skip=TRUE; (s)->buff = 0; } while (0)

/* Returns number of bytes read into current target */
U16 SimStageBytesRead(SimStager *s);

#define SimStageBytesRead(s) ((s)->cur)

/*
 * Stages input bytes according to current stager setup.
 * 
 * Returns - BT_STATUS_PENDING = Need more input data
 *           BT_STATUS_SUCCESS = Requested data staged, or no bytes needed
 */
BtStatus SimStageInput(SimStager *stager, U8 **inputBuff, U16 *inputLen);

#if SIM_CLIENT == XA_ENABLED
/* CMGR callback */
void SimClientCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status);

/* RFCOMM Callback */
void SimClientRfCallback(RfChannel *Channel, RfCallbackParms *Parms);

/* SDP Functions */
BtStatus SimStartServiceQuery(SimClient *Client, SdpQueryMode mode);
BtStatus SimVerifySdpQueryRsp(SimClient *Client, SdpQueryToken *token);
void SimClientSdpCallback(const BtEvent *Event);

/* Utility functions */
void SimClientAppCallback(SimClient *Client, U8 Event, BtStatus Status, void *ptr, U16 len);
void SimClientClearConnection(SimClient *Client);

/* Client transmit routines */
BtStatus SimClientSendConnReq(SimClient *Client, U8 *msgSize);

#define SimClientSendConnReq(Client, m) SimSendPacket(           \
                                    &Client->simPacket,          \
                                    &Client->rfChannel,          \
                                    SIM_CONNECT_REQ,             \
                                    SIM_PARM_MAX_MSG_SIZE, 2, m, \
                                    0, 0, 0)

BtStatus SimClientSendDiscReq(SimClient *Client);

#define SimClientSendDiscReq(Client) SimSendPacket(              \
                                    &Client->simPacket,          \
                                    &Client->rfChannel,          \
                                    SIM_DISCONNECT_REQ,          \
                                    0, 0, 0,                     \
                                    0, 0, 0)

BtStatus SimClientSendAtrReq(SimClient *Client);

#define SimClientSendAtrReq(Client) SimSendPacket(               \
                                    &Client->simPacket,          \
                                    &Client->rfChannel,          \
                                    SIM_TRANSFER_ATR_REQ,        \
                                    0, 0, 0,                     \
                                    0, 0, 0)

BtStatus SimClientSendApdu7816Req(SimClient *Client, U16 Len, U8 *Apdu);

#define SimClientSendApdu7816Req(Client, Len, Apdu) SimSendPacket(      \
                                    &Client->simPacket,                 \
                                    &Client->rfChannel,                 \
                                    SIM_TRANSFER_APDU_REQ,              \
                                    SIM_PARM_CMD_APDU_7816, Len, Apdu,  \
                                    0, 0, 0)

BtStatus SimClientSendApduReq(SimClient *Client, U16 Len, U8 *Apdu);

#define SimClientSendApduReq(Client, Len, Apdu) SimSendPacket(     \
                                    &Client->simPacket,            \
                                    &Client->rfChannel,            \
                                    SIM_TRANSFER_APDU_REQ,         \
                                    SIM_PARM_CMD_APDU, Len, Apdu,  \
                                    0, 0, 0)

BtStatus SimClientSendSimOnReq(SimClient *Client);

#define SimClientSendSimOnReq(Client) SimSendPacket(             \
                                    &Client->simPacket,          \
                                    &Client->rfChannel,          \
                                    SIM_POWER_ON_REQ,            \
                                    0, 0, 0,                     \
                                    0, 0, 0)

BtStatus SimClientSendSimOffReq(SimClient *Client);

#define SimClientSendSimOffReq(Client) SimSendPacket(            \
                                    &Client->simPacket,          \
                                    &Client->rfChannel,          \
                                    SIM_POWER_OFF_REQ,           \
                                    0, 0, 0,                     \
                                    0, 0, 0)

BtStatus SimClientSendResetSimReq(SimClient *Client);

#define SimClientSendResetSimReq(Client) SimSendPacket(          \
                                    &Client->simPacket,          \
                                    &Client->rfChannel,          \
                                    SIM_RESET_REQ,               \
                                    0, 0, 0,                     \
                                    0, 0, 0)

BtStatus SimClientSendCardRdrStatusReq(SimClient *Client);

#define SimClientSendCardRdrStatusReq(Client) SimSendPacket(     \
                                    &Client->simPacket,          \
                                    &Client->rfChannel,          \
                                    SIM_CARD_RDR_STATUS_REQ,     \
                                    0, 0, 0,                     \
                                    0, 0, 0)

BtStatus SimClientSendSetTransProtReq(SimClient *Client, U8 id);

#define SimClientSendSetTransProtReq(Client,id) SimSendPacket(      \
                                    &Client->simPacket,             \
                                    &Client->rfChannel,             \
                                    SIM_SET_TRANS_PROT_REQ,         \
                                    SIM_PARM_TRANS_PROT, 1, &id,    \
                                    0, 0, 0)

#endif /* SIM_CLIENT == XA_ENABLED */

#if SIM_SERVER == XA_ENABLED
/* RFCOMM callback */
void SimServerRfCallback(RfChannel *Channel, RfCallbackParms *Parms);

/* Utility functions */
void SimServerAppCallback(SimServer *Server, U8 Event, BtStatus Status, void *ptr, U16 len);
void SimServerClearConnection(SimServer *Server);

/* Server transmit routines */

/* Server transmit routines */
BtStatus SimServerSendConnRspErr(SimServer *Server, SimConnStatus *Status, U8 *MaxMsgSize);

#define SimServerSendConnRspErr(Server, Status, m) SimFindFreeAndSendPacket(        \
                                       Server,                                      \
                                       &Server->rfChannel,                          \
                                       SIM_CONNECT_RSP,                             \
                                       SIM_PARM_CONN_STATUS, 1, Status,             \
                                       SIM_PARM_MAX_MSG_SIZE, 2, m)

BtStatus SimServerSendConnRspNoErr(SimServer *Server, SimConnStatus *Status);

#define SimServerSendConnRspNoErr(Server, Status) SimFindFreeAndSendPacket(        \
                                       Server,                                     \
                                       &Server->rfChannel,                         \
                                       SIM_CONNECT_RSP,                            \
                                       SIM_PARM_CONN_STATUS, 1, Status,            \
                                       0, 0, 0)

BtStatus SimServerSendDiscRsp(SimServer *Server);

#define SimServerSendDiscRsp(Server) SimSendPacket(       \
                                    &Server->discPacket,  \
                                    &Server->rfChannel,   \
                                    SIM_DISCONNECT_RSP,   \
                                    0, 0, 0,              \
                                    0, 0, 0)

BtStatus SimServerSendDiscInd(SimServer *Server, SimDisconnectType *Type);

#define SimServerSendDiscInd(Server, Type) SimSendPacket(                                  \
                                    ((*Type == SIM_DISCONNECT_IMMEDIATE) ?                 \
                                     (&Server->discPacket) : (&Server->discGracePacket)),  \
                                    &Server->rfChannel,                                    \
                                    SIM_DISCONNECT_IND,                                    \
                                    SIM_PARM_DISC_TYPE, 1, Type,                           \
                                    0, 0, 0)

BtStatus SimServerSendStatus(SimServer *Server, SimCardStatus Status);

#define SimServerSendStatus(Server, Status) SimFindFreeAndSendPacket(              \
                                    Server,                                        \
                                    &Server->rfChannel,                            \
                                    SIM_STATUS_IND,                                \
                                    SIM_PARM_STATUS_CHANGE, 1, Status,             \
                                    0, 0, 0)

BtStatus SimServerSendAtrRspErr(SimServer *Server, SimResultCode *Result);

#define SimServerSendAtrRspErr(Server, Result) SimFindFreeAndSendPacket(     \
                                       Server,                               \
                                       &Server->rfChannel,                   \
                                       SIM_TRANSFER_ATR_RSP,                 \
                                       SIM_PARM_RESULT_CODE, 1, Result,      \
                                       0, 0, 0)

BtStatus SimServerSendAtrRspNoErr(SimServer *Server, U16 Len, U8 *Atr, SimResultCode *Result);

#define SimServerSendAtrRspNoErr(Server, Len, Atr, Result) SimFindFreeAndSendPacket(     \
                                       Server,                                           \
                                       &Server->rfChannel,                               \
                                       SIM_TRANSFER_ATR_RSP,                             \
                                       SIM_PARM_RESULT_CODE, 1, Result,                  \
                                       SIM_PARM_ATR, Len, Atr)

BtStatus SimServerSendApduRspErr(SimServer *Server, SimResultCode *Result);

#define SimServerSendApduRspErr(Server, Result) SimFindFreeAndSendPacket(     \
                                       Server,                                \
                                       &Server->rfChannel,                    \
                                       SIM_TRANSFER_APDU_RSP,                 \
                                       SIM_PARM_RESULT_CODE, 1, Result,       \
                                       0, 0, 0)

BtStatus SimServerSendApduRspNoErr(SimServer *Server, U16 Len, U8 *Apdu, SimResultCode *Result);

#define SimServerSendApduRspNoErr(Server, Len, Apdu, Result) SimFindFreeAndSendPacket(     \
                                         Server,                                           \
                                         &Server->rfChannel,                               \
                                         SIM_TRANSFER_APDU_RSP,                            \
                                         SIM_PARM_RESULT_CODE, 1, Result,                  \
                                         SIM_PARM_RSP_APDU, Len, Apdu)

BtStatus SimServerSendSimOnRsp(SimServer *Server, SimResultCode *Result);

#define SimServerSendSimOnRsp(Server, Result) SimFindFreeAndSendPacket(            \
                                    Server,                                        \
                                    &Server->rfChannel,                            \
                                    SIM_POWER_ON_RSP,                              \
                                    SIM_PARM_RESULT_CODE, 1, Result,               \
                                    0, 0, 0)

BtStatus SimServerSendSimOffRsp(SimServer *Server, SimResultCode *Result);

#define SimServerSendSimOffRsp(Server, Result) SimFindFreeAndSendPacket(           \
                                    Server,                                        \
                                    &Server->rfChannel,                            \
                                    SIM_POWER_OFF_RSP,                             \
                                    SIM_PARM_RESULT_CODE, 1, Result,               \
                                    0, 0, 0)

BtStatus SimServerSendResetSimRsp(SimServer *Server, SimResultCode *Result);

#define SimServerSendResetSimRsp(Server, Result) SimFindFreeAndSendPacket(         \
                                    Server,                                        \
                                    &Server->rfChannel,                            \
                                    SIM_RESET_RSP,                                 \
                                    SIM_PARM_RESULT_CODE, 1, Result,               \
                                    0, 0, 0)

BtStatus SimServerSendCardRdrStatusRspErr(SimServer *Server, SimResultCode *Result);

#define SimServerSendCardRdrStatusRspErr(Server, Result) SimFindFreeAndSendPacket(    \
                                    Server,                                           \
                                    &Server->rfChannel,                               \
                                    SIM_CARD_RDR_STATUS_RSP,                          \
                                    SIM_PARM_RESULT_CODE, 1, Result,                  \
                                    0, 0, 0)

BtStatus SimServerSendCardRdrStatusRspNoErr(SimServer *Server, SimResultCode *Result, SimCardReaderStatus *Status);

#define SimServerSendCardRdrStatusRspNoErr(Server, Result, Status) SimFindFreeAndSendPacket(    \
                                    Server,                                                     \
                                    &Server->rfChannel,                                         \
                                    SIM_CARD_RDR_STATUS_RSP,                                    \
                                    SIM_PARM_RESULT_CODE, 1, Result,                            \
                                    SIM_PARM_CARD_RDR_STATUS, 1, Status)

BtStatus SimServerSendSetTransProtRsp(SimServer *Server, SimResultCode *Result);

#define SimServerSendSetTransProtRsp(Server, Result) SimFindFreeAndSendPacket(     \
                                    Server,                                        \
                                    &Server->rfChannel,                            \
                                    SIM_SET_TRANS_PROT_RSP,                        \
                                    SIM_PARM_RESULT_CODE, 1, Result,               \
                                    0, 0, 0)

BtStatus SimServerSendErrorRsp(SimServer *Server);

#define SimServerSendErrorRsp(Server) SimFindFreeAndSendPacket(                    \
                                    Server,                                        \
                                    &Server->rfChannel,                            \
                                    SIM_ERROR_RSP,                                 \
                                    0, 0, 0,                                       \
                                    0, 0, 0)



#endif /* SIM_SERVER == XA_ENABLED */

#endif /* __BT_SIMI_H */ 

