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
 * File:
 *     $Workfile:avctpmsg.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains the Message handler.
 *             
 * Created:     Mar 23, 2004
 *
 * Copyright 2004 - 2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
#include "bt_common.h"
#include "sdp.h"
#include "btconfig.h"
#include "sys/avctpi.h"
#include "avctp.h"

/* Function Prototypes */
static void AvctpConnCb(AvctpConn *Conn, AvctpConnCallbackParms *parms);


/*****************************************************************************
 * FUNCTION
 *  AvcMsgInit
 * DESCRIPTION
 *  Initialize the Message handler.
 * PARAMETERS
 *  chnl        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvcMsgInit(AvctpChannel *chnl)
{
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_MSG_INIT );
    AvctpInitConn(&chnl->conn, AvctpConnCb);
}


/*****************************************************************************
 * FUNCTION
 *  AvctpMsgSendCommand
 * DESCRIPTION
 *  Send a command.
 * PARAMETERS
 *  chnl        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AvctpMsgSendCommand(AvctpChannel *chnl)
{
    BtStatus status;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_MSG_SEND_COMMAND, chnl->txState);

#ifdef __BT_AVRCP_V14__ 
    chnl->txState = AVCTP_MSG_TXSTATE_WAIT_RSP;
    status = AVC_SendStart(&chnl->avcChnl, &chnl->cmdPacket);
#else

#ifdef __BT_AVRCP_V13__ 
    chnl->txState = AVCTP_MSG_TXSTATE_WAIT_RSP;
    status = AVC_SendStart(&chnl->avcChnl, &chnl->cmdPacket);
#else
    if (chnl->txState == AVCTP_MSG_TXSTATE_IDLE)
    {
        chnl->txState = AVCTP_MSG_TXSTATE_WAIT_RSP;
        status = AVC_SendStart(&chnl->avcChnl, &chnl->cmdPacket);
    }
    else
    {
        status = BT_STATUS_BUSY;
    }
#endif
#endif

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvctpMsgHandleCommand
 * DESCRIPTION
 *  Handle a command.
 * PARAMETERS
 *  chnl        [IN]        
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvctpMsgHandleCommand(AvctpChannel *chnl, AvcCallbackParms *parms)
{
    AvctpCallbackParms info;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_MSG_HANDLE_COMMAND, parms->pktType, parms->rxId);

    if (parms->rxId != SC_AV_REMOTE_CONTROL)
    {
        bt_trace(TRACE_GROUP_1, AVCTP_INVALID_PROFILE_ID);
        chnl->rspPacket.msgType = AVC_MSG_TYPE_REJECT;
        chnl->rspPacket.txId = parms->rxId;
        chnl->rspPacket.txIdSize = 2;
        chnl->rspPacket.txData = 0;
        chnl->rspPacket.txDataLen = 0;
        chnl->rspPacket.useMsgHdr = FALSE;
        /*
         * There's no frame to send along; completion of this message
         * should not be indicated back to the app
         */
        chnl->curFrame = 0;
        chnl->rxState = AVCTP_MSG_RXSTATE_IGNORE;
        AssertEval(AVC_SendStart(&chnl->avcChnl, &chnl->rspPacket) == BT_STATUS_PENDING);
        return;
    }

    if (parms->pktType == AVC_PACKET_TYPE_START || parms->pktType == AVC_PACKET_TYPE_SINGLE)
    {

        if ( BT_PSM_AVCTP== chnl->channelType && parms->len < 3) // AV13 channeltype is AVCTP psm
        {
            bt_trace(TRACE_GROUP_1, AVCTP_MISSING_PARAMETERS_DISCONNECTING);
            (void)AvctpDisconnect(&chnl->conn);
            return;
        }

        if( BT_PSM_AVCTP_BROWSING != chnl->channelType ){ 
            // Parse the av/c header
            /* Move channel state into COMMAND-processing */
            chnl->state = AVCTP_CHAN_STATE_COMMAND;

            /* Decode parameters buffer AV/C header */
            chnl->rx.cmdFrame.ctype = parms->data[0] & 0x0f;
            chnl->rx.cmdFrame.subunitType = parms->data[1] >> 3;
            chnl->rx.cmdFrame.subunitId = parms->data[1] & 0x07;
            chnl->rx.cmdFrame.opcode = parms->data[2];
            
            chnl->rx.cmdFrame.operands = parms->data + 3;
            chnl->rx.cmdFrame.operandLen = parms->len - 3;
            chnl->rx.cmdFrame.more = (parms->pktType == AVC_PACKET_TYPE_START);
            chnl->rx.cmdFrame.transId = parms->transid;
            
#ifdef __AVCTP_TESTER_
            chnl->rxRawData = parms->data;
            chnl->rxRawLength = parms->len;
            chnl->rxRawType = parms->pktType;
#endif /* __AVCTP_TESTER_ */ 
        }else{
            // browse packet - avctp header + avrcp browse header
            chnl->rx.cmdFrame.ctype = 0x0;         // NOT use
            chnl->rx.cmdFrame.subunitType = 0x0;   // NOT use
            chnl->rx.cmdFrame.subunitId = 0x0;     // NOT use
            chnl->rx.cmdFrame.opcode = AVCTP_OPCODE_BROWSE_CAHNNEL;
            
            chnl->rx.cmdFrame.operands = parms->data ;
            chnl->rx.cmdFrame.operandLen = parms->len ;
            chnl->rx.cmdFrame.more = (parms->pktType == AVC_PACKET_TYPE_START);
            chnl->rx.cmdFrame.transId = parms->transid;
            
            OS_Report("[AVRCP][BWS] Receive browse-packet operandLen:%d more:%d", chnl->rx.cmdFrame.operandLen, chnl->rx.cmdFrame.more);
        }
        /* Prepare to indicate this command to the application */
        info.event = AVCTP_EVENT_COMMAND;
    }
    else
    {
        /* Prepare to indicate more operand data to the app */
        chnl->rx.cmdFrame.operandLen = parms->len;
        chnl->rx.cmdFrame.operands = parms->data;
    #ifdef __AVCTP_TESTER_
        chnl->rxRawData = parms->data;
        chnl->rxRawLength = parms->len;
        chnl->rxRawType = parms->pktType;
    #endif /* __AVCTP_TESTER_ */ 
        chnl->rx.cmdFrame.more = (parms->pktType == AVC_PACKET_TYPE_CONTINUE);
        info.event = AVCTP_EVENT_OPERANDS;
    }

    /* Indicate command (or operands) to application */
    info.channel = chnl;
    info.p.cmdFrame = &chnl->rx.cmdFrame;

    chnl->callback(chnl, &info);
}


/*****************************************************************************
 * FUNCTION
 *  AvctpMsgHandleResponse
 * DESCRIPTION
 *  Handle a response.
 * PARAMETERS
 *  chnl        [IN]        
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvctpMsgHandleResponse(AvctpChannel *chnl, AvcCallbackParms *parms)
{
    AvctpCallbackParms info;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_MSG_HANDLE_RESPONSE, parms->pktType);
    

    if (parms->rxId != SC_AV_REMOTE_CONTROL)
    {
        bt_trace(TRACE_GROUP_1, AVCTP_INVALID_PID_DISCONNECTING);
        (void)AvctpDisconnect(&chnl->conn);
        return;
    }

    if (parms->pktType == AVC_PACKET_TYPE_START || parms->pktType == AVC_PACKET_TYPE_SINGLE)
    {

        if (parms->len < 3)
        {
            bt_trace(TRACE_GROUP_1, AVCTP_MISSING_PARAMETERS_DISCONNECTING);
            (void)AvctpDisconnect(&chnl->conn);
            return;
        }

        if( BT_PSM_AVCTP_BROWSING != chnl->channelType ){ 
            // Parse the av/c header

            /* Decode parameters buffer */
            chnl->rx.rspFrame.response = parms->data[0] & 0x0f;
            chnl->rx.rspFrame.subunitType = parms->data[1] >> 3;
            chnl->rx.rspFrame.subunitId = parms->data[1] & 0x07;
            chnl->rx.rspFrame.opcode = parms->data[2];

            chnl->rx.rspFrame.operands = parms->data + 3;
            chnl->rx.rspFrame.operandLen = parms->len - 3;
            chnl->rx.rspFrame.more = (parms->pktType == AVC_PACKET_TYPE_START);
            chnl->rx.rspFrame.transId = parms->transid;
#ifdef __AVCTP_TESTER_
            chnl->rxRawData = parms->data;
            chnl->rxRawLength = parms->len;
            chnl->rxRawType = parms->pktType;
#endif /* __AVCTP_TESTER_ */ 
        }else{

            // browse packet - avctp header + avrcp browse header
            chnl->rx.rspFrame.response = 0x0;         // NOT use
            chnl->rx.rspFrame.subunitType = 0x0;   // NOT use
            chnl->rx.rspFrame.subunitId = 0x0;     // NOT use
            chnl->rx.rspFrame.opcode = AVCTP_OPCODE_BROWSE_CAHNNEL;
            
            chnl->rx.rspFrame.operands = parms->data ;
            chnl->rx.rspFrame.operandLen = parms->len ;
            chnl->rx.rspFrame.more = (parms->pktType == AVC_PACKET_TYPE_START);
            chnl->rx.rspFrame.transId = parms->transid;
            
            OS_Report("[AVRCP][BWS] No av/c parse operandLen:%d more:%d", chnl->rx.cmdFrame.operandLen, chnl->rx.cmdFrame.more);
        }
        
        /* Prepare to indicate response to the application */
        info.event = AVCTP_EVENT_RESPONSE;
    }
    else
    {
        /* Prepare to indicate additional operands to the application */
        chnl->rx.rspFrame.operands = parms->data;
        chnl->rx.rspFrame.operandLen = parms->len;
    #ifdef __AVCTP_TESTER_
        chnl->rxRawData = parms->data;
        chnl->rxRawLength = parms->len;
        chnl->rxRawType = parms->pktType;
    #endif /* __AVCTP_TESTER_ */ 
        chnl->rx.rspFrame.more = (parms->pktType == AVC_PACKET_TYPE_CONTINUE);
        info.event = AVCTP_EVENT_OPERANDS;
    }

    /* Indicate this event to the application */
    info.channel = chnl;
    info.p.rspFrame = &chnl->rx.rspFrame;
    chnl->callback(chnl, &info);

    if (chnl->rx.rspFrame.response == AVCTP_RESPONSE_INTERIM &&
        chnl->rx.rspFrame.opcode == AVCTP_OPCODE_VENDOR_DEPENDENT)
    {
        chnl->txState = AVCTP_MSG_TXSTATE_WAIT_RSP;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvctpMsgHandleReject
 * DESCRIPTION
 *  Handle a reject.
 * PARAMETERS
 *  chnl        [IN]        
 *  parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvctpMsgHandleReject(AvctpChannel *chnl, AvcCallbackParms *parms)
{
    AvctpCallbackParms info;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_MSG_HANDLE_REJECT, parms->pktType);

    if (parms->pktType == AVC_PACKET_TYPE_START || parms->pktType == AVC_PACKET_TYPE_SINGLE)
    {
        if (parms->len >= 3)
        {
            chnl->rx.rspFrame.response = parms->data[0] & 0x0f;
            chnl->rx.rspFrame.subunitType = parms->data[1] >> 3;
            chnl->rx.rspFrame.subunitId = parms->data[1] & 0x07;
            chnl->rx.rspFrame.opcode = parms->data[2];
            chnl->rx.rspFrame.operandLen = ((parms->packetsLeft + 1) * (L2CAP_MTU - 1)) - 6;
            if (parms->pktType == AVC_PACKET_TYPE_SINGLE)
            {
                chnl->rx.rspFrame.operandLen++;
            }
            chnl->rx.rspFrame.operands = parms->data + 3;
        }

        /* Indicate the response to the application */
        info.event = AVCTP_EVENT_REJECT;
        info.channel = chnl;
        info.p.rspFrame = &chnl->rx.rspFrame;
        chnl->callback(chnl, &info);
    }
    else
    {
        chnl->rx.rspFrame.operands = parms->data;
        chnl->rx.rspFrame.operandLen = parms->len;
    }

    if (chnl->rx.rspFrame.operandLen)
    {
        /* Indicate the operands to the application */
        info.event = AVCTP_EVENT_OPERANDS;
        info.channel = chnl;
        info.p.rspFrame = &chnl->rx.rspFrame;
        chnl->callback(chnl, &info);
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvctpMsgCb
 * DESCRIPTION
 *  Message channel callback.
 * PARAMETERS
 *  avcChnl        [IN]        
 *  parms           [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvctpMsgCb(AvcChannel *avcChnl, AvcCallbackParms *parms)
{
    AvctpChannel *chnl = ContainingRecord(avcChnl, AvctpChannel, avcChnl);
    AvctpCallbackParms info;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_MSG_CB, parms->event);
    info.transId = parms->transid;

    switch (parms->event)
    {
        case AVC_EVENT_TX_DONE:
            if (chnl->curFrame)
            {
                info.status = BT_STATUS_SUCCESS;
                info.event = AVCTP_EVENT_TX_DONE;
                info.channel = chnl;
                info.p.cmdFrame = chnl->curFrame;
                chnl->callback(chnl, &info);
            }
            chnl->rxState = AVCTP_MSG_RXSTATE_BEGIN;
            break;

        case AVC_EVENT_TX_ERROR:
            bt_trace(TRACE_GROUP_1, AVCTP_ERROR_SENDING_MESSAGE);
            if (chnl->curFrame)
            {
                info.status = BT_STATUS_FAILED;
                info.event = AVCTP_EVENT_TX_DONE;
                info.channel = chnl;
                info.p.cmdFrame = chnl->curFrame;
                chnl->callback(chnl, &info);
            }
            break;

        case AVC_EVENT_TX_TIMEOUT:
            (void)AvctpDisconnect(&chnl->conn);
            break;

        case AVC_EVENT_RX_IND:
            if (chnl->rxState != AVCTP_MSG_RXSTATE_IGNORE)
            {
                switch (parms->msgType)
                {
                    case AVC_MSG_TYPE_COMMAND:
                        AvctpMsgHandleCommand(chnl, parms);
                        break;

                    case AVC_MSG_TYPE_ACCEPT:
                    /* AV13 Ignore the wrong Controller rxState */
                    /*  ignore the wrong state
                        if (chnl->txState != AVCTP_MSG_TXSTATE_WAIT_RSP)
                        {
                            
                            chnl->rxState = AVCTP_MSG_RXSTATE_IGNORE;
                            kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_MSG_CB_IGNORE_EVENT, chnl->txState);
                            break;
                        }
                    */
                        if (parms->packetsLeft == 0)
                        {
                            chnl->txState = AVCTP_MSG_TXSTATE_IDLE;
                            chnl->rxState = AVCTP_MSG_RXSTATE_BEGIN;
                        }
                        AvctpMsgHandleResponse(chnl, parms);
                        break;

                    case AVC_MSG_TYPE_REJECT:
                    /* AV13 Ignore the wrong Controller rxState */
                    /*  ignore the wrong state                    
                        if (chnl->txState != AVCTP_MSG_TXSTATE_WAIT_RSP)
                        {
                            chnl->rxState = AVCTP_MSG_RXSTATE_IGNORE;
                            kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_MSG_CB_IGNORE_EVENT, chnl->txState);
                            break;
                        }
                    */                        
                        if (parms->packetsLeft == 0)
                        {
                            chnl->txState = AVCTP_MSG_TXSTATE_IDLE;
                            chnl->rxState = AVCTP_MSG_RXSTATE_BEGIN;
                        }
                        AvctpMsgHandleReject(chnl, parms);
                        break;

                    default:
                        break;
                }
            }
            else if (parms->packetsLeft == 0)
            {
                chnl->rxState = AVCTP_MSG_RXSTATE_BEGIN;
            }
            break;

        case AVC_EVENT_RX_UNKNOWN_MESSAGE:
        case AVC_EVENT_RX_UNKNOWN_PACKET:
        case AVC_EVENT_RX_BAD_TRANS_ID:
            OS_Report("[AVCTP] RX__EVENT %d error", parms->event);
            break;

        case AVC_EVENT_RX_BUFFER_UNDERRUN:
            OS_Report("[AVCTP] RX_BUFFER_UNDERRUN");
        case AVC_EVENT_RX_BUFFER_OVERRUN:
            (void)AvctpDisconnect(&chnl->conn);
            break;

        default:
            break;
    }

    if (chnl->rxState != AVCTP_MSG_RXSTATE_IGNORE)
    {
        if ((parms->event != AVC_EVENT_TX_DONE) && parms->packetsLeft)
        {
            chnl->rxState = AVCTP_MSG_RXSTATE_CONTINUE;
        }
        else
        {
            chnl->rxState = AVCTP_MSG_RXSTATE_BEGIN;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvctpConnCb
 * DESCRIPTION
 *  Connection callback.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvctpConnCb(AvctpConn *Conn, AvctpConnCallbackParms *Parms)
{
    AvctpChannel *chnl = ContainingRecord(Conn, AvctpChannel, conn);
    AvctpCallbackParms info;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_CONN_CB, Parms->event);

    switch (Parms->event)
    {
        case AVCTP_CONN_EVENT_CONNECT:
            chnl->rxState = AVCTP_MSG_RXSTATE_BEGIN;
            chnl->txState = AVCTP_MSG_TXSTATE_IDLE;
            AVC_InitChannel(&chnl->avcChnl, chnl->conn.l2ChannelId, AvctpMsgCb, AVCTP_RTX_TIMEOUT);

            /* Indicate the connection to the application */
            info.event = AVCTP_EVENT_CONNECT;
            info.channel = chnl;
            info.p.remDev = Parms->ptrs.remDev;
            chnl->callback(chnl, &info);
            break;

        case AVCTP_CONN_EVENT_CONNECT_IND:
            /* Indicate the incoming connection to the application */
            info.event = AVCTP_EVENT_CONNECT_IND;
            info.channel = chnl;
            info.p.remDev = Parms->ptrs.remDev;
            chnl->callback(chnl, &info);
            break;

        case AVCTP_CONN_EVENT_DISCONNECT:
            chnl->rxState = AVCTP_MSG_RXSTATE_BEGIN;
            chnl->txState = AVCTP_MSG_TXSTATE_IDLE;

            /* Indicate the connection to the application */
            info.event = AVCTP_EVENT_DISCONNECT;
            info.channel = chnl;
            info.p.remDev = Parms->ptrs.remDev;
            chnl->callback(chnl, &info);
            break;

        case AVCTP_CONN_EVENT_DATA_SENT:
            AVC_SendContinue(&chnl->avcChnl); /* AV13 keep to send a avctp packets*/
            break;

        case AVCTP_CONN_EVENT_DATA_IND:
            AVC_Receive(&chnl->avcChnl, Parms->ptrs.data, Parms->dataLen, 2);
            break;
    }
}

