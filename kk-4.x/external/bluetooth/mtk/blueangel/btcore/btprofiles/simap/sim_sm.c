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

/***************************************************************************
 *
 * File:
 *     $Workfile:sim_sm.c$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description:
 *     State machine functions for the SIM Access protocol.
 *
 * Created:
 *     May 3, 2002
 *
 * Copyright 2001 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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

#ifdef __SIMAP_MAUI__
#include "kal_release.h"        /* Basic data type */
#include "stack_common.h"       /* message and module IDs */
#include "stack_msgs.h" /* enum for message IDs */
#include "app_ltlcom.h" /* Task message communiction */
#include "syscomp_config.h"     /* type of system module components */
#include "task_config.h"        /* Task creation */
#include "stacklib.h"   /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h" /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "app_buff_alloc.h"
#include "uart_sw.h"
//#include "bluetooth_bm_struct.h"
#include "kal_trace.h"  /* Trace Header Files */
//#include "bt_maui_utility.h"
#endif
#include "bt_common.h"
#include "stdio.h"      /* Basic C I/O functions */
#include "bt.h"
#include "bluetooth_struct.h"
#include "bluetooth_trc.h"
#include "conmgr.h"
#include "btalloc.h"
#include "bt_simi.h"
//#include "bt_adp_simap.h"
#include "bt_feature.h"
#include "bluetooth_gap_struct.h"
#include "bt_os_api.h"


extern SIMAPCallback SIMAPCB;

extern void bt_fakesim_connect_req_hdlr(void);
extern void CONVERT_ARRAY2BDADDR(btbm_bd_addr_t *dest, U8 *src);
#ifdef __SIMAP_MAUI__
extern void bt_simap_start_timer(eventid *timer_id_p, kal_uint32 timer_duration, kal_timer_func_ptr funct_ptr);
#else
extern void bt_simap_start_timer(void *timer_id_p, kal_uint32 timer_duration, kal_timer_func_ptr funct_ptr);
#endif
#ifdef __SIMAP_MAUI__
extern void bt_simap_stop_timer(eventid *timer_id_p);
#else
extern void bt_simap_stop_timer(void *timer_id_p);
#endif

extern void SimServerAuthorize(RfChannel *Channel, BD_ADDR bdAddr);


/*****************************************************************************
 * FUNCTION
 *  SimParseParms
 * DESCRIPTION
 *  Parses received parameters in Client/Server message into the msg struct.
 *  Unexpected or malformed parameters cause the msg to be indicated as invalid.
 * PARAMETERS
 *  parser      [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS when the message is ready to be delivered.
 *  BT_STATUS_PENDING when we need more data to be staged.
 *****************************************************************************/
static BtStatus SimParseParms(SimMsgParser *parser)
{
    U16 len;
    U8 i;

    switch (parser->state)
    {
        case SIM_PARSE_PARM:
            /* Control arrives here when it's time to parse a new param. */
          check_parm:
            if (parser->msg->numParms)
            {
                /* Set up to parse a parameter */
                SimStageInit(&parser->stager, parser->stageBuff, 4);
                parser->state = SIM_PARSE_PARM_HEADER;
                parser->curParm = SIM_PARM_INVALID;

            }
            else
            {

                /* Scan to see if there are missing parms */
                for (i = 0; i < parser->parmsExpectedLen; i++)
                {
                    if (parser->parmsExpected[i] != SIM_PARM_INVALID)
                    {
                        break;
                    }
                }
                if (i < parser->parmsExpectedLen)
                {
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_LACK_EXPECTED_PARM);
                    parser->msg->type = SIM_ERROR_IND;
                }

                return BT_STATUS_SUCCESS;
            }
            break;

        case SIM_PARSE_PARM_HEADER:
            /* Control arrives here when we have staged the param header. */

            /* Next state after header is always PARM_VALUE. */
            parser->state = SIM_PARSE_PARM_VALUE;
            parser->curParm = parser->stager.buff[0];
            len = BEtoHost16(parser->stager.buff + 2);

            /* Are we expecting this parameter? Search. */
            for (i = 0; i < parser->parmsExpectedLen; i++)
            {
                /*
                 * For scanning purposes, consider the two APDU types
                 * equivalent.
                 */
                if ((parser->parmsExpected[i] == SIM_PARM_CMD_APDU) && (parser->curParm == SIM_PARM_CMD_APDU_7816))
                {
                    break;
                }

                /* Otherwise look for an exact match. */
                if (parser->parmsExpected[i] == parser->curParm)
                {
                    break;
                }
            }

            if (i == parser->parmsExpectedLen)
            {
                /* No we are not. Downgrade the parameter. */
                parser->curParm = SIM_PARM_INVALID;

            }
            else
            {
                /* Mark the parameter handled */
                parser->parmsExpected[i] = SIM_PARM_INVALID;

                /* By default, set up parsing of a four-byte value */
                SimStageInit(&parser->stager, parser->stageBuff, 4);

                /* Validate parameter and set up to read value */
                switch (parser->curParm)
                {
                    case SIM_PARM_MAX_MSG_SIZE:
                        if (len != 2)
                        {
                            parser->curParm = SIM_PARM_INVALID;
                        }
                        break;

                    case SIM_PARM_CONN_STATUS:
                    case SIM_PARM_RESULT_CODE:
                    case SIM_PARM_DISC_TYPE:
                    case SIM_PARM_CARD_RDR_STATUS:
                    case SIM_PARM_STATUS_CHANGE:
                    case SIM_PARM_TRANS_PROT:
                        if (len != 1)
                        {
                            parser->curParm = SIM_PARM_INVALID;
                        }
                        break;

                    case SIM_PARM_CMD_APDU:
                    case SIM_PARM_CMD_APDU_7816:
                    case SIM_PARM_RSP_APDU:
                        /* Make sure APDU length is within reason */
                        if ((len > 0) && (len <= SIM_MAX_APDU))
                        {
                            parser->msg->parm.apdu.dataLen = len;
                            SimStageInit(&parser->stager, parser->msg->parm.apdu.data, len);

                            /* Set APDU type appropriately */
                            if (parser->curParm != SIM_PARM_CMD_APDU_7816)
                            {
                                parser->msg->parm.apdu.type = SIM_APDU_GSM;
                            }
                            else
                            {
                                parser->msg->parm.apdu.type = SIM_APDU_IEC_7816;
                            }
                        }
                        else
                        {
                            parser->curParm = SIM_PARM_INVALID;
                        }
                        break;

                    case SIM_PARM_ATR:
                        /* Make sure ATR length is within reason */
                        if ((len > 0) && (len <= SIM_MAX_APDU))
                        {
                            parser->msg->parm.atr.dataLen = len;
                            SimStageInit(&parser->stager, parser->msg->parm.atr.data, len);
                        }
                        else
                        {
                            parser->curParm = SIM_PARM_INVALID;
                        }
                        break;

                    default:
                        Assert(0);
                }
            }

            /* If the curParm is invalid, set the stager to skip over it. */
            if (parser->curParm == SIM_PARM_INVALID)
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_CONTAIN_INVALID_PARM);
                /* Also downgrade the message itself */
                parser->msg->type = SIM_ERROR_IND;
                SimStageSkip(&parser->stager, len);
            }
            break;

        case SIM_PARSE_PARM_VALUE:
            /* Control arrives here when the param data is completely staged */

            switch (parser->curParm)
            {
                case SIM_PARM_MAX_MSG_SIZE:
                    parser->msg->parm.maxMsgSize = BEtoHost16(parser->stageBuff);
                    break;

                case SIM_PARM_CONN_STATUS:
                    parser->msg->connStatus = parser->stageBuff[0];

                    /*
                     * If the error was MSG_SIZE then convert the current expected
                     * parm to MsgSize
                     */
                    if (parser->msg->connStatus == SIM_CONN_STATUS_MSG_SIZE)
                    {
                        parser->parmsExpected[0] = SIM_PARM_MAX_MSG_SIZE;
                    }
                    break;

                case SIM_PARM_RESULT_CODE:
                    parser->msg->resultCode = parser->stageBuff[0];

                    if (parser->msg->resultCode != SIM_RESULT_OK)
                    {
                        switch (parser->msg->type)
                        {
                            case SIM_TRANSFER_APDU_RSP:
                            case SIM_TRANSFER_ATR_RSP:
                            case SIM_CARD_RDR_STATUS_RSP:
                                /* In the event of error, don't expect second param. */
                                parser->parmsExpectedLen = 1;
                        }
                    }

                    break;

                case SIM_PARM_DISC_TYPE:
                    parser->msg->parm.discType = parser->stageBuff[0];
                    break;

                case SIM_PARM_CARD_RDR_STATUS:
                    parser->msg->parm.cardRdrStatus = parser->stageBuff[0];
                    break;

                case SIM_PARM_STATUS_CHANGE:
                    parser->msg->parm.statusChange = parser->stageBuff[0];
                    break;

                case SIM_PARM_TRANS_PROT:
                    parser->msg->parm.protocolId = parser->stageBuff[0];
                    break;
            }

            /* Note: other params (APDU/ATR) are staged right into the msg */

            /* Parameter has been handled */
            parser->msg->numParms--;

            /* If necessary, consume any padding bytes based on current length */
            len = (4 - (SimStageBytesRead(&parser->stager) % 4)) % 4;
            if (len)
            {
                /* Set the stager to skip past padding bytes. */
                SimStageSkip(&parser->stager, len);
                parser->state = SIM_PARSE_PARM;
            }
            else
            {
                /* Immediately check for new parms */
                goto check_parm;
            }
            break;
    }

    return BT_STATUS_PENDING;
}

#if SIM_SERVER == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  ShowAPDUReqContent
 * DESCRIPTION
 *  
 * PARAMETERS
 *  msg     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ShowAPDUReqContent(SimMsg *msg)
{
    /* Send APDU request to SIM task */
    kal_uint16 i = 0;

    /* Report(("ShowAPDUReqContent():")); */
    kal_trace(TRACE_GROUP_1, BT_SIMAP_SHOW_APDUREQCONTENT);
    for (i = 0; i < msg->parm.apdu.dataLen; i++)
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_SHOW_ONE_BYTE, msg->parm.apdu.data[i]);
    }
    /* Report(("\n")); */
}


/*****************************************************************************
 * FUNCTION
 *  CheckPacketIsFromList
 * DESCRIPTION
 *  
 * PARAMETERS
 *  packet      [IN]        
 *  SPacket     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_bool CheckPacketIsFromList(BtPacket *packet, SimPacket **SPacket)
{
    U8 i = 0;

    for (i = 0; i < NUM_OF_SIMPACKETS; i++)
    {
        if (packet == (&(SIM(simServer)[0].simPacketList[i].packet)))
        {
            /* Report(("In CheckPacketIsFromList(): find returned packet in inx=%d",i)); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_CHECK_PACKET_IS_GOT_FROM_LIST, i);

            (*SPacket) = &((SIM(simServer)[0]).simPacketList[i]);
            return KAL_TRUE;
        }

    }
    return KAL_FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  ClearPacketList
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Server      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void ClearPacketList(SimServer *Server)
{
    U8 i = 0;

    for (i = 0; i < NUM_OF_SIMPACKETS; i++)
    {
        Server->simPacketList[i].packet.flags = 0;
    }
}


/*****************************************************************************
 * FUNCTION
 *  SetPacketListMsgSize
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Server          [IN]        
 *  msg_size        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SetPacketListMsgSize(SimServer *Server, kal_uint16 msg_size)
{
    U8 i = 0;

    for (i = 0; i < NUM_OF_SIMPACKETS; i++)
    {
        Server->simPacketList[i].maxMsgSize = msg_size;
    }
}


/*****************************************************************************
 * FUNCTION
 *  SimApCloseRFCommChnl
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rfChannel       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
BtStatus SimApCloseRFCommChnl(RfChannel *rfChannel)
{
    BtStatus status = BT_STATUS_FAILED;

	kal_trace(TRACE_GROUP_1, BT_SIMAP_CLOSE_RFCOMM_CHNL);
	//Report(("[BT SIMAP]RF_CloseChannel... "));
	
    status = RF_CloseChannel(rfChannel);
    if ((status != BT_STATUS_PENDING) && (status != BT_STATUS_IN_PROGRESS) && (status != BT_STATUS_SUCCESS))
    {
        /* Report(("In SimServerStateOpen(): status of RF_CloseChannel=%d",status)); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_CLOSE_RFCOMM_FAILED, status);
        Assert(0);
    }
	return status;
}


/*****************************************************************************
 * FUNCTION
 *  SimServerStateOpen
 * DESCRIPTION
 *  Open state for the SIM server.
 * PARAMETERS
 *  Server      [IN]        
 *  Msg         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void SimServerStateOpen(SimServer *Server, SimMsg *Msg)
{
    SimEvent event = 0;
    SimMsg *msg = 0;
    U16 msgSize = 0;
    BtStatus status;
    SimPacket *SPacket = NULL;

    kal_trace(
        TRACE_GROUP_1,
        BT_SIMAP_OPEN_STATE_MSGTYPE_CARDSTATUS_SUBSTATE,
        Msg->type,
        SIM(simServer)[0].current_simcardreader_status,
        Server->substate);

    switch (Msg->type)
    {
        case SIM_DISCONNECT_REQ:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_DISCONNECT_REQ_IN_OPEN);
            Server->state = SIM_SERVER_STATE_DISC_PENDING;
            SimServerAppCallback(
                Server,
                SAEVENT_SERVER_CLOSE_IND,
                BT_STATUS_SUCCESS,
                Server->remDev,
                sizeof(BtRemoteDevice));

            if (SIM(simServer)[0].substate & BT_APP_DISCONNECT_REQ_RECV)
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_GET_DISC_REQ_BEFORE_GRACEFUL_DISC_TIMER_TIMEOUT);
                bt_simap_stop_timer(&SIM(simap_event_id));
            }

            status = SimServerSendDiscRsp(Server);
            if (status == BT_STATUS_PENDING)
            {
                goto no_error_exit;
            }
            else
            {
                /* Report(("In SimServerStateOpen() status of SendDiscRsp=%d",status)); */
                kal_trace(TRACE_GROUP_1, BT_SIMAP_OPENSTATE_SEND_DISCRSP_FAILED, status);
                goto error_exit;
            }
            break;
        case SIM_TRANSFER_APDU_REQ:

            kal_trace(TRACE_GROUP_1, BT_SIMAP_APDU_REQ_IN_OPEN);
            event = SAEVENT_SERVER_APDU_REQ;
            Server->flags |= SIM_SERVER_FLAG_APDU;
            msg = Msg;
            msgSize = sizeof(msg);
            ShowAPDUReqContent(msg);
            if (SIM(simServer)[0].substate & BT_APP_DISCONNECT_REQ_RECV)
            {
                /*
                 * In this case: means that SimServer has sent DISC with Graceful type,
                 * so each time the APDU req arrived, just stop the guard timer
                 */
                kal_trace(TRACE_GROUP_1, BT_SIMAP_GET_APDU_CMD_BEFORE_GRACEFUL_DISC_TIMER_TIMEOUT);
                bt_simap_stop_timer(&SIM(simap_event_id));
                /*
                 * Remove bt_simap_start_timer() from here to PACKET_HANDLED for APDU_RSP
                 * We will restart timer again when this APDU_RSP has been returned here
                 */
            }

            break;
        case SIM_TRANSFER_ATR_REQ:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_ATR_REQ_IN_OPEN);
            event = SAEVENT_SERVER_ATR_REQ;
            Server->flags |= SIM_SERVER_FLAG_ATR;
            break;
        case SIM_POWER_ON_REQ:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_POWER_ON_REQ_IN_OPEN);
            event = SAEVENT_SERVER_SIM_ON_REQ;
            Server->flags |= SIM_SERVER_FLAG_SIM_ON;
            break;
        case SIM_CARD_RDR_STATUS_REQ:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_CARD_RDR_STATUS_REQ_IN_OPEN);
            event = SAEVENT_SERVER_CARD_READER_STATUS_REQ;
            Server->flags |= SIM_SERVER_FLAG_CARD_RDR_STATUS;
            break;
        case SIM_SET_TRANS_PROT_REQ:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_TRANSPORT_PROTOCOL_REQ_IN_OPEN);
            event = SAEVENT_SERVER_SET_TRANS_PROT_REQ;
            Server->flags |= SIM_SERVER_FLAG_SET_TRANS_PROT;
            msg = Msg;
            msgSize = sizeof(msg);
            break;
        case SIM_POWER_OFF_REQ:
            if (!(Server->flags & SIM_SERVER_FLAG_SIM_OFF))
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_POWER_OFF_REQ_IN_OPEN);
                event = SAEVENT_SERVER_SIM_OFF_REQ;
                Server->flags |= SIM_SERVER_FLAG_SIM_OFF | SIM_SERVER_FLAG_PROCESSING;
                SimServerAppCallback(Server, event, BT_STATUS_SUCCESS, 0, 0);
            }
            goto no_error_exit;
            break;
        case SIM_RESET_REQ:
            if (!(Server->flags & SIM_SERVER_FLAG_RESET_SIM))
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_RESET_REQ_IN_OPEN);
                event = SAEVENT_SERVER_RESET_SIM_REQ;
                Server->flags |= SIM_SERVER_FLAG_RESET_SIM | SIM_SERVER_FLAG_PROCESSING;
                SimServerAppCallback(Server, event, BT_STATUS_SUCCESS, 0, 0);
            }
            goto no_error_exit;
            break;

        case SIM_ERROR_IND:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_ERROR_IND_IN_OPEN);
            /*
             * Do not assert on this; if our incoming data stream gets corrupted
             * we may reach this point more frequently. 
             */
            status = SimServerSendErrorRsp(Server);
            if (status == BT_STATUS_PENDING)
            {
                goto no_error_exit;
            }
            else
            {
                /* Report(("In SimServerStateOpen(): send ErrRsp status=%d",status)); */
                Assert(0);
            }
            break;

        case SIM_PACKET_HANDLED:
            /*
             * Ignore wrong packets: can't break, because in this situration, flags with on SIM_SERVER_FLAG_PROCESSING
             * But after break this case SIM_PACKET_HANDLED, it set SIM_SERVER_FLAG_PROCESSING flag ! (But this behavior is wrong)
             */

            if (Server->msg.parm.packet == (&(Server->discGracePacket.packet)))
            {
                if (Server->discGracePacket.dataToSend == 0)
                {
                    /* Report(("Open State: discGracePacket returned")); */
                    /* Clear flag=0, means clear the SIM_SERVER_FLAG_XXX and BTP_FLAG_INUSE : because the SimPacket or discGracePacket is returned */
                    Server->discGracePacket.packet.flags = 0;

                    kal_trace(
                        TRACE_GROUP_1,
                        BT_SIMAP_GET_DISCGRACE_PACKET_HANDLED_WITH_SUBSTATE,
                        SIM(simServer)[0].substate);
                    if (SIM(simServer)[0].substate & BT_APP_DISCONNECT_REQ_RECV)
                    {
                        /* Start timer to wait the request(i.e., APDU cmd or DISC req) from the sim client */
                        bt_simap_start_timer(
                            &SIM(simap_event_id),
                            MAX_WAIT_REQ_MSEC_IN_GRACEFUL_DISC_MODE,
                            bt_simap_wait_req_in_graceful_disconnect_procedure_timeout_handler);

                    }
                }
                else
                {
                    /* Send more data */
                    if (SimSendPacketSegment(&Server->discGracePacket, &Server->rfChannel) != BT_STATUS_PENDING)
                    {
                        Server->discGracePacket.dataToSend = 0;
                        Server->discGracePacket.packet.flags = 0;
                        kal_trace(TRACE_GROUP_1, BT_SIMAP_SEGMENTS_OF_DISCGRACE_PACKET_CAN_NOT_SEND_IN_OPEN);
                        goto error_exit;
                        /* SimServerAppCallback(Server, SAEVENT_SERVER_STATUS_SENT, BT_STATUS_FAILED, 0, 0); */
                    }
                }

            }   /* end of Server->discGracePacket.packet */

            /* Check if this packet is from simPackList */
            /* else if(Server->msg.parm.packet == (&(Server->simPacket.packet))) */
            else if (CheckPacketIsFromList(Server->msg.parm.packet, &SPacket) == KAL_TRUE)
            {
                if (SPacket == NULL)
                {
                    Assert(0);
                }

                /* Report(("SPacket=%x, another=%x",SPacket, &((SIM(simServer)[0]).simPacketList[0]))); */

                /* if (Server->simPacket.dataToSend == 0) */
                if (SPacket->dataToSend == 0)
                {
                    /* Report(("Open State: simPacket returned")); */
                    /* Clear flag=0, means clear the SIM_SERVER_FLAG_XXX and BTP_FLAG_INUSE : because the SimPacket or discGracePacket is returned */
                    /* Server->simPacket.packet.flags = 0; */
                    SPacket->packet.flags = 0;

                    kal_trace(TRACE_GROUP_1, BT_SIMAP_GET_SIM_PACKET_HANDLED_WITH_SUBSTATE, SIM(simServer)[0].substate);
                    /* Only rsp from SIM task is successful and call corresponding Sim_XXXRSP is successful, then its substate is reserved for checking here */
                    if ((Server->substate & BT_SIM_RESET_REQ_SENT_SET_T) && (Server->flags & SIM_SERVER_FLAG_STATUS))
                    {
                        if (Server->flags & SIM_SERVER_FLAG_STATUS_PENDING)
                        {
                            kal_trace(TRACE_GROUP_1, BT_SIMAP_STATUS_IN_SEND_DUE_TO_SET_T);
                            /* When sending Status_IND to client through invoking SIM_ServerSendStatus(), the flag SIM_SERVER_FLAG_STATUS_PENDING is turned on, now clear it! */
                            Server->flags &= ~(SIM_SERVER_FLAG_STATUS | SIM_SERVER_FLAG_STATUS_PENDING);
                            Server->substate &= (~BT_SIM_RESET_REQ_SENT_SET_T);
                            SimServerAppCallback(Server, SAEVENT_SERVER_STATUS_SENT, BT_STATUS_SUCCESS, 0, 0);
                        }
                    }
                    else if (Server->substate & BT_SIM_RESET_REQ_SENT_SET_T)
                    {
                        /* After sending SET_TRANSPORT_PROTOCOL_TYPE_RSP with result code=OK to client, then STATUS_IND with reset is also needed be sent */
                        /* Ask of Upper layer for ths SIM status */
                        kal_uint8 result = SIM_RESULT_NO_REASON;
                        BtStatus status = BT_STATUS_FAILED;

                        /* Check Parameter Type */
                        kal_trace(TRACE_GROUP_1, BT_SIMAP_SET_T_PACKET_HANDLED_WITH_PARMTYPE, SPacket->packet.data[4]);

                        //if ((Server->simPacket.packet.data[4] != SIM_PARM_RESULT_CODE) ||
                        if ((SPacket->packet.data[4] != SIM_PARM_RESULT_CODE) ||
                            (SPacket->packet.data[6] != 0x00) || (SPacket->packet.data[7] != 0x01))
                        {
                            Assert(0);
                        }

                        /* Get Parameter value */
                        /* result = Server->simPacket.packet.data[8]; */
                        result = SPacket->packet.data[8];

                        /*
                         * Because SimMsg's parm filed is union type, so in this case, parm.packet is used for this function,
                         * the parm.protocolId set when receiving client's SET_T req is invalid
                         */

                        if (result == SIM_RESULT_OK)
                        {
                            status = BT_STATUS_SUCCESS;
                        }
                        else
                        {
                            if (SIM(simServer)[0].client_requested_transport_protocol_type != SIM(simServer)[0].supported_transport_protocol_type_capability)
                            {
                                status = BT_STATUS_FAILED;  /* This means that SET T cmd failed */
                            }
                        }
                        Server->flags |= SIM_SERVER_FLAG_STATUS;
                        SimServerAppCallback(
                            Server,
                            SAEVENT_SERVER_STATUS_REQ,
                            status,
                            Server->remDev,
                            sizeof(BtRemoteDevice));
                    }
                    else
                    {
                        if (Server->substate & BT_SIM_CONNECT_PROCEDURE_COMPLETED)
                        {
                            /* Report(("In SimServerOpen(), SIM_PACKET_HANDLED for substate=%04X",Server->substate)); */
                            /* Clear the corresponding request command's bit to zero when sending response to sim Client successfully */
                            if (Server->substate & BT_SIM_RESET_REQ_SENT)
                            {
                                Server->substate &= (~BT_SIM_RESET_REQ_SENT);
                            }
                            else if (Server->substate & BT_SIM_RESET_REQ_SENT_POWER_ON)
                            {
                                Server->substate &= (~BT_SIM_RESET_REQ_SENT_POWER_ON);
                            }
                            else if (Server->substate & BT_SIM_APDU_REQ_SENT)
                            {
                                Server->substate &= (~BT_SIM_APDU_REQ_SENT);
                                /* Check if Server has sent DISC with graceful packet type */
                                /* Modify from BT_SIM_APDU_REQ_SENT to BT_APP_DISCONNECT_REQ_RECV */
                                if (Server->substate & BT_APP_DISCONNECT_REQ_RECV)
                                {
                                    /* Report(("Send APDU_RSP and its packet handled, start timer again for DISC with Graceful type")); */
                                    kal_trace(TRACE_GROUP_1, BT_SIMAP_ALREADY_SENT_APDU_RSP_RECV_DISC_REQ);
                                    bt_simap_start_timer(
                                        &SIM(simap_event_id),
                                        MAX_WAIT_REQ_MSEC_IN_GRACEFUL_DISC_MODE,
                                        (kal_timer_func_ptr) bt_simap_wait_req_in_graceful_disconnect_procedure_timeout_handler);
                                }

                            }
                            else if (Server->substate & BT_SIM_POWER_OFF_REQ_SENT)
                            {
                                Server->substate &= (~BT_SIM_POWER_OFF_REQ_SENT);
                            }
                            else if (Server->substate & BT_SIM_DISCONNECT_REQ_SENT)
                            {
                                Server->substate &= (~BT_SIM_DISCONNECT_REQ_SENT);
                            }
                            else
                            {
                                kal_trace(TRACE_GROUP_1, BT_SIMAP_SIM_PACKET_HANDLED_DUE_TO_SIMSERVER_RSP);
                            }
                        }

                    }

                }
                else
                {
                    /* Send more data */
                    //if (SimSendPacketSegment(&Server->simPacket, 
                    //                     &Server->rfChannel) != BT_STATUS_PENDING)

                    if (SimSendPacketSegment(SPacket, &Server->rfChannel) != BT_STATUS_PENDING)
                    {
                        //Server->simPacket.dataToSend = 0;
                        //Server->simPacket.packet.flags = 0;

                        SPacket->dataToSend = 0;
                        SPacket->packet.flags = 0;

                        if (Server->flags & SIM_SERVER_FLAG_STATUS_PENDING)
                        {
                            kal_trace(
                                TRACE_GROUP_1,
                                BT_SIMAP_SEGMENTS_OF_SIM_PACKET_FOR_STATUS_IND_CAN_NOT_SEND_IN_OPEN);
                            Server->flags &= ~(SIM_SERVER_FLAG_STATUS | SIM_SERVER_FLAG_STATUS_PENDING);
                            SimServerAppCallback(Server, SAEVENT_SERVER_STATUS_SENT, BT_STATUS_FAILED, 0, 0);
                        }
                    }
                }
            }   /* end of Server->simPacket.packet */
            else
            {
                U16 i = 0;

                for (i = 0; i < Server->msg.parm.packet->dataLen; i++)
                    /* Report(("%02X ,",Server->msg.parm.packet->data[i])); */
                {
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_SHOW_ONE_BYTE);
                }

                /* Report(("\nIn Open State, not simPacket or discGracePacket handled received")); */
                kal_trace(TRACE_GROUP_1, BT_SIMAP_NOT_CORRECT_PKT_RECV_IN_OPEN_STATE);

            }
            goto no_error_exit;
            break;
        default:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_GET_UNHANDLED_EVENT_IN_OPEN, Msg->type);
            goto error_exit;
            break;
    }
    /* Report(("Open state,flags=%02x",Server->flags)); */
    kal_trace(TRACE_GROUP_1, BT_SIMAP_IN_OPEN_STATE_CHECK_SRVFLAGS, Server->flags);
    if (!(Server->flags & SIM_SERVER_FLAG_PROCESSING))
    {
        Server->flags |= SIM_SERVER_FLAG_PROCESSING;
        SimServerAppCallback(Server, event, BT_STATUS_SUCCESS, msg, msgSize);
        goto no_error_exit;
    }
    else
    {
        kal_trace(TRACE_GROUP_1, BT_SIMAP_GET_ALREADY_PROCESSED_CMD_IN_OPEN);
    }

  error_exit:

    /* Return value from RF_CloseChannel() may be BT_STATUS_IN_PROGRRESS(presenting rfchannel state is DLC_STATE_DISC_PENDING) */
    SimApCloseRFCommChnl(&(Server->rfChannel));
  no_error_exit:

    return;
}


/*****************************************************************************
 * FUNCTION
 *  SimServerStateConnPending
 * DESCRIPTION
 *  Connect Pending state for the SIM Access server.
 * PARAMETERS
 *  Server      [IN]        
 *  Msg         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void SimServerStateConnPending(SimServer *Server, SimMsg *Msg)
{
    /* BtStatus status; */
    SimPacket *SPacket = NULL;

    switch (Msg->type)
    {
        case SIM_PACKET_HANDLED:
            /* Ignore wrong packets */

            /* if(Server->msg.parm.packet != &Server->simPacket.packet) break; */
            if (CheckPacketIsFromList(Server->msg.parm.packet, &SPacket) == KAL_FALSE)
            {
                break;
            }
            if (SPacket == NULL)
            {
                Assert(0);
            }

            /* Report(("SPacket=%x, another=%x",SPacket, &((SIM(simServer)[0]).simPacketList[0]))); */

            /* if(Server->simPacket.dataToSend == 0) */
            if (SPacket->dataToSend == 0)
            {
                /* Server->simPacket.packet.flags = 0; */
                SPacket->packet.flags = 0;

                if (Server->flags & SIM_SERVER_FLAG_STATUS)
                {
                    if (Server->flags & SIM_SERVER_FLAG_STATUS_PENDING)
                    {
                        /* In this case: means that STATUS_IND packet is sent back to client completely */
                        kal_trace(TRACE_GROUP_1, BT_SIMAP_CARD_STATUS_SENT_IN_CONPENDING);
                        Server->flags &= ~SIM_SERVER_FLAG_STATUS_PENDING;
                        SimServerAppCallback(Server, SAEVENT_SERVER_STATUS_SENT, BT_STATUS_SUCCESS, 0, 0);

                        /* only FAST_CONNECT_MODe will enter this case */
                        Assert(SIM(simServer)[0].connect_mode == FAST_CONNECT_MODE);

                        if (SIM(simServer)[0].connect_mode == FAST_CONNECT_MODE)
                        {
                            /* Complete of Connection Procedure */
                            Server->flags &= ~SIM_SERVER_FLAG_STATUS;
                            Server->state = SIM_SERVER_STATE_OPEN;
                            SimServerAppCallback(
                                Server,
                                SAEVENT_SERVER_OPEN,
                                BT_STATUS_SUCCESS,
                                Server->remDev,
                                sizeof(BtRemoteDevice));
                        }
                    }
                    else if ((!(Server->flags & SIM_SERVER_FLAG_ATR)) &&
                             (SIM(simServer)[0].connect_mode == SLOW_CONNECT_MODE))
                    {
                        /*
                         * In this case: means that "STATUS_IND with reset then ATR_RSP" packet is sent back to client completely in SLOW_CONNECT_MODE 
                         * or "STATUS_IND with CARD_NOT_ACCESSIBLE is sent and no ATR req is expected" in SLOW_CONNECT_MODE
                         */
                        /* Complete of Connection Procedure */
                        kal_trace(TRACE_GROUP_1, BT_SIMAP_ATR_SENT_CON_IS_UP);
                        Server->flags &= ~SIM_SERVER_FLAG_STATUS;
                        Server->state = SIM_SERVER_STATE_OPEN;
                        SimServerAppCallback(
                            Server,
                            SAEVENT_SERVER_OPEN,
                            BT_STATUS_SUCCESS,
                            Server->remDev,
                            sizeof(BtRemoteDevice));
                    }
                }
                else
                {
                    /* Ask for the SIM status: when sending CONNECT_RSP packet back to client completely */
                    /* Check this sent packet is CONN_RSP and with ConnectionStatus 0x00 */
                    if (SPacket->packet.data[0] == SIM_CONNECT_RSP && SPacket->packet.data[4] == SIM_PARM_CONN_STATUS &&
                        SPacket->packet.data[5] == 0x00 && SPacket->packet.data[6] == 0x00 &&
                        SPacket->packet.data[7] == 0x01 && SPacket->packet.data[8] == SIM_CONN_STATUS_OK)

                    {
                        /* data[0]:msg type; data[4]:param_id,[5]:reserved; data[6],[7]:para_len; */
                        /* Note: CONNECT_RSP sent to client, but in connection procedure: Card not accessible and reset successful are possible */
                        BtStatus status;

                        if (SIM(simServer)[0].current_transport_protocol_type == 0)
                        {
                            /*
                             * This means that the T=0 is used and card is reset successfully 
                             * or Card is Removed, we did not to set T. So its default value is still T=0
                             */
                            status = BT_STATUS_SUCCESS;
                        }
                        else
                        {
                            /* This means that the default T=0 is not used for this sim card, so T=1 is used */
                            /* 
                             * Notify the upper layer so that STATUS_IND with card not accessible is sent to client,
                             * in this way, the client has a chance to use T=1 to reset and use this sim card 
                             */
                            status = BT_STATUS_FAILED;
                        }
                        Server->flags |= SIM_SERVER_FLAG_STATUS;
                        SimServerAppCallback(
                            Server,
                            SAEVENT_SERVER_STATUS_REQ,
                            status,
                            Server->remDev,
                            sizeof(BtRemoteDevice));
                    }
                    else
                        /* Report(("Not CONN_RSP with ConStatus OK packet returned, No Status_IND to Client")); */
                    {
                        kal_trace(TRACE_GROUP_1, BT_SIMAP_NO_CONRSP_WITH_OK_RETURNED);
                    }

                }
            }
            else
            {
                /* Send more data */
                if (SimSendPacketSegment(SPacket, &Server->rfChannel) != BT_STATUS_PENDING)

                {
                    kal_trace(TRACE_GROUP_1, BT_SIM_SEND_FAILED_IN_CONNECT_PROCESS);
                    //Server->simPacket.dataToSend = 0;
                    //Server->simPacket.packet.flags = 0;

                    SPacket->dataToSend = 0;
                    SPacket->packet.flags = 0;
                    goto err_exit;
                }
            }
            break;

        case SIM_TRANSFER_ATR_REQ:
            /*
             * Client is requesting an ATR request, the connection will not 
             * progress until an ATR is sent by the server.
             */
            if (SIM(simServer)[0].connect_mode == SLOW_CONNECT_MODE)
            {
                if ((!(Server->flags & SIM_SERVER_FLAG_PROCESSING)) && (Server->flags & SIM_SERVER_FLAG_ATR))
                {
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_GET_ATR_REQ_IN_CONPENDING);
                    bt_simap_stop_timer(&SIM(simap_event_id));
                    Server->flags |= SIM_SERVER_FLAG_PROCESSING;
                    SimServerAppCallback(Server, SAEVENT_SERVER_ATR_REQ, BT_STATUS_SUCCESS, 0, 0);
                }
                else if (Server->flags & SIM_SERVER_FLAG_ATR)
                {
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_CMD_ALREADY_PROCESSED_IN_CONPENDING);
                    goto err_exit;
                }
                else
                {
                    /* In this case: meaning that Server forgets to set SIM_SERVER_FLAG_ATR flag during connection procedure */
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_NO_EXPECT_REC_ATR_REQ_IN_CONPENDING);
                    Assert(0);
                }
            }
            else
            {
                /* Report(("In this connect_mode, not expect ATR_REQ in connPending state")); */
                kal_trace(TRACE_GROUP_1, BT_SIMAP_SLOW_CON_MODE_NOT_EXPECT_RECV_ATR_REQ);
                goto err_exit;
            }
            break;

        default:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RCV_UNHANDLED_EVENT_IN_CONPENDING, Msg->type);
          err_exit:
            SimApCloseRFCommChnl(&(Server->rfChannel));

            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  SimServerStateDiscPending
 * DESCRIPTION
 *  Disconnect Pending state for the SIM Access server.
 * PARAMETERS
 *  Server      [IN]        
 *  Msg         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void SimServerStateDiscPending(SimServer *Server, SimMsg *Msg)
{
    /* BtStatus status; */
    switch (Msg->type)
    {
        case SIM_PACKET_HANDLED:
            /* Ignore wrong packets */
            if (Server->msg.parm.packet != &Server->discPacket.packet)
            {
                break;
            }

            if (Server->discPacket.dataToSend == 0)
            {
                Server->discPacket.packet.flags = 0;
                kal_trace(TRACE_GROUP_1, BT_SIMAP_PACKET_HANDLED_IN_DISCPENDING);
                /* RF_CLOSED event is reported later, then in SimServerClearConnection(): Clean up Server's state and flags */
                goto err_exit;
            }
            else
            {
                /* Send more data */
                if (SimSendPacketSegment(&Server->discPacket, &Server->rfChannel) != BT_STATUS_PENDING)
                {
                    Server->discPacket.dataToSend = 0;
                    Server->discPacket.packet.flags = 0;
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_SEGMENTS_CAN_NOT_SEND_IN_DISCPENDING);
                    goto err_exit;
                }
            }
            break;
        default:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RCV_UNHANDLED_EVENT_IN_DISCPENDING, Msg->type);
          err_exit:
		  	{
				BtDeviceContext *bdc = NULL;

				bdc = DS_FindDevice(&Server->remDev->bdAddr);
				Report(("Before Close Rfcomm Channel need set acl can be disc!\n"));
				if(bdc != NULL)
				{
					//Force disconnect ACL Link when disable simap
					bdc->link->discFlag = TRUE;
				}
				else
				{
					Report(("Before Close Rfcomm Channel DS_FindDevice failed!\n"));
				}
          	}
            SimApCloseRFCommChnl(&(Server->rfChannel));
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  SimServerStateClosed
 * DESCRIPTION
 *  Closed state for the SIM Access server.
 * PARAMETERS
 *  Server      [IN]        
 *  Msg         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void SimServerStateClosed(SimServer *Server, SimMsg *Msg)
{
    BtStatus status;
    SimConnStatus connStatus;
    U8 maxMsgSize[2];
    SimPacket *SPacket = NULL;

    switch (Msg->type)
    {
        case SIM_CONNECT_REQ:

            if ((Server->available) || ((Server->available == FALSE) && (Server->substate & BT_SIMAP_WAIT_RETRY_MTU)))
            {

                /* Each time to start a new SAP connection, remember to clear packet's flags for usage */
                SIM(simServer)[0].simPacket.packet.flags = 0;
                SIM(simServer)[0].discPacket.packet.flags = 0;
                /* Add this on 2007-0117 */
                ClearPacketList(&SIM(simServer)[0]);

                /* check availabke == 0: ensure that this server is not occupied by other client */
                Server->available = FALSE;

                if (Msg->parm.maxMsgSize <= SIM_MAX_MSG_SIZE)
                {
#if 0 // Reserved for MAUI
                    bt_simap_auth_req_struct *bt_simap_auth_req = construct_local_para(sizeof(bt_simap_auth_req_struct), TD_UL);
#endif
		      SIMAPCallbackParms parms;
                    btbm_bd_addr_t bd_addr;

                    btmtk_os_memset((kal_uint8*) & bd_addr, 0, sizeof(btbm_bd_addr_t));
                    CONVERT_ARRAY2BDADDR(&bd_addr, Server->remDev->bdAddr.addr);

                    if (Server->substate & BT_SIMAP_WAIT_RETRY_MTU)
                    {
                        Server->substate &= ~BT_SIMAP_WAIT_RETRY_MTU;
                        /* Report(("Wait retry SAP MTU and this MTU is acceptable")); */
                        kal_trace(TRACE_GROUP_1, BT_SIMAP_RESENT_MTU_ACCEPTED);
                    }

                    SIM(simServer)[0].simPacket.maxMsgSize = Msg->parm.maxMsgSize;
                    SIM(simServer)[0].discPacket.maxMsgSize = Msg->parm.maxMsgSize;
                    SIM(simServer)[0].discGracePacket.maxMsgSize = Msg->parm.maxMsgSize;
                    /* Add this on 2007-0117 */
                    SetPacketListMsgSize(&SIM(simServer)[0], Msg->parm.maxMsgSize);

#if 0  // authorize time is changed to RFCOMM_OPEN_IND
//<!----yufeng
		Server->substate |= BT_MMI_AUTH_REQ_SENT;

#if 0 // Reserved for MAUI
                    bt_simap_auth_req->lap = 0;
                    bt_simap_auth_req->uap = 0;
                    bt_simap_auth_req->nap = 0;
                    bt_simap_auth_req->lap = bd_addr.lap;
                    bt_simap_auth_req->uap = bd_addr.uap;
                    bt_simap_auth_req->nap = bd_addr.nap;
#endif
                    parms.p.simap_auth_req.lap = 0;
                    parms.p.simap_auth_req.uap = 0;
                    parms.p.simap_auth_req.nap = 0;
                    parms.p.simap_auth_req.lap = bd_addr.lap;
                    parms.p.simap_auth_req.uap = bd_addr.uap;
                    parms.p.simap_auth_req.nap = bd_addr.nap;

                    /* Send AUTH_REQ msg to MMI */
#if 0 // Reserved for MAUI
                    bt_send_msg(
                        MSG_ID_BT_SIMAP_AUTH_REQ,
                        MOD_MMI,
                        BT_APP_SAP,
                        (local_para_struct*) bt_simap_auth_req,
                        NULL);
#endif
		      parms.event = SIMAP_EVENT_AUTH_REQ;
		      parms.dstMod = MOD_MMI;
		      SIMAPCB(&parms);
//-->			  
#else
                        /* CONNECT with SIM card */
			kal_trace(TRACE_GROUP_1, BT_SIMAP_RCV_CON_REQ_IN_CLOSE, SIM(simServer)[0].simPacketList[0].maxMsgSize);

			SIM(simServer)[0].state = SIM_SERVER_STATE_CONN_PENDING;

			/* 2006-10-18: Send BT_SIM_CONNECT_REQ msg to SIM task to notify it that SIM Acces Profile connection is incoming */
			SIM(simServer)[0].substate |= BT_SIM_CONNECT_REQ_SENT;

			if (SIM(simServer)[0].exec_mode == 0)
			{
#ifdef __SIMAP_MAUI__
			    bt_send_msg(MSG_ID_BT_SIM_CONNECT_REQ, MOD_SIM, PS_SIM_SAP, NULL, NULL);
#else
			 SIMAPCallbackParms parms;
			 parms.event = SIM_EVENT_CONNECT_REQ;
			 parms.dstMod = MOD_SIM;
			 SIMAPCB(&parms);
#endif
			}
#ifdef __SIMAP_UT_
			else if (SIM(simServer)[0].exec_mode == 1)
			{
			    bt_fakesim_connect_req_hdlr();
			}
#endif /* __SIMAP_UT_ */ 


#endif
                }
                else
                {
                    Server->substate |= BT_SIMAP_WAIT_RETRY_MTU;
                    /* Message size is too large, reject the connection */
                    StoreBE16(maxMsgSize, SIM_MAX_MSG_SIZE);
                    Server->simPacket.maxMsgSize = SIM_MAX_MSG_SIZE;
                    Server->discPacket.maxMsgSize = SIM_MAX_MSG_SIZE;
                    Server->discGracePacket.maxMsgSize = Msg->parm.maxMsgSize;

                    /* Add this on 2007-0117 */
                    SetPacketListMsgSize(&SIM(simServer)[0], Msg->parm.maxMsgSize);

                    kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_CON_RSP_WITH_REJ_MSGSIZE);
                    connStatus = SIM_CONN_STATUS_MSG_SIZE;
                    status = SimServerSendConnRspErr(Server, &connStatus, maxMsgSize);
                    if (status != BT_STATUS_PENDING)
                    {
                        /* Report(("Status from SendConnRspErr of reject MTU =%d",status)); */
                        kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_CONRSPERR_FAILED, status);
                        Assert(0);
                    }

                }
            }
            else
            {
                kal_trace(TRACE_GROUP_1, BT_SIMAP_SIMSERVER_BUSY_FOR_ONE_CLIENT);

                Server->substate |= BT_REJECT_SECOND_CLIENT_SESSION;

                /* Send Connect_RSP with error to SIM Client! */
                connStatus = SIM_CONN_STATUS_SERVER_UNABLE_ESTABLISH_CONNECTION;
                StoreBE16(maxMsgSize, SIM_MAX_MSG_SIZE);
                /* When simServer is serving one client, simServer->simPacket has already maxMsgSize value */
                status = SimServerSendConnRspErr(Server, &connStatus, maxMsgSize);
                if (status != BT_STATUS_PENDING)
                {
                    /* Report(("Status from SendConnRspErr of unable establish connection=%d",status)); */
                    kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_CONRSPERR_FAILED, status);
                    Assert(0);
                }

            }
            break;
        case SIM_PACKET_HANDLED:
            /* Ignore wrong packets */
            /* if (Server->msg.parm.packet != &Server->simPacket.packet) break; */
            if (CheckPacketIsFromList(Server->msg.parm.packet, &SPacket) == KAL_FALSE)
            {
                break;
            }
            if (SPacket == NULL)
            {
                Assert(0);
            }

            /* Report(("SPacket=%x, another=%x",SPacket, &((SIM(simServer)[0]).simPacketList[0]))); */

            /* if (Server->simPacket.dataToSend != 0) { */
            if (SPacket->dataToSend != 0)
            {
                /* Send more data */
                //if (SimSendPacketSegment(&Server->simPacket, 
                //                         &Server->rfChannel) != BT_STATUS_PENDING)

                if (SimSendPacketSegment(SPacket, &Server->rfChannel) != BT_STATUS_PENDING)

                {
                    //Server->simPacket.dataToSend = 0;
                    //Server->simPacket.packet.flags = 0;
                    SPacket->dataToSend = 0;
                    SPacket->packet.flags = 0;
                    goto err_exit;
                }
            }
            else
            {
                /* Server->simPacket.packet.flags = 0; */
                SPacket->packet.flags = 0;
            }
            break;
        default:
            kal_trace(TRACE_GROUP_1, BT_SIMAP_RCV_UNEXPECTED_EVENT_IN_CLOSED, Msg->type);
          err_exit:
            /* Return value from RF_CloseChannel() may be BT_STATUS_IN_PROGRRESS(presenting rfchannel state is DLC_STATE_DISC_PENDING) */
            SimApCloseRFCommChnl(&(Server->rfChannel));
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  SimServerResetParser
 * DESCRIPTION
 *  Clears state in the server parser so that it is ready for a new msg.
 * PARAMETERS
 *  Server      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void SimServerResetParser(SimServer *Server)
{
    /* Reset the SAR variables, and the state */
    Server->parser.state = SIM_PARSE_HEADER;
    SimStageInit(&Server->parser.stager, Server->parser.stageBuff, 4);
}


/*****************************************************************************
 * FUNCTION
 *  SimServerParseReq
 * DESCRIPTION
 *  Parse the request from the client and call the state machine.
 * PARAMETERS
 *  Server      [IN]        
 *  rxLen       [IN]        
 *  rxBuff      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void SimServerParseReq(SimServer *Server, U16 rxLen, U8 *rxBuff)
{
    /*
     * Loop until the current staging buffer cannot be filled or until
     * we are out of input.
     */
    while ((SimStageInput(&Server->parser.stager, &rxBuff, &rxLen) == BT_STATUS_SUCCESS))
    {

        switch (Server->parser.state)
        {
            case SIM_PARSE_HEADER:
                /* Initialize the server message */
                OS_MemSet((U8*) & Server->msg, 0, sizeof(SimMsg));
                Server->msg.type = Server->parser.stageBuff[0];
                Server->msg.numParms = Server->parser.stageBuff[1];
                Server->parser.msg = &Server->msg;

                /* By default, expect no parameters */
                Server->parser.parmsExpectedLen = 0;

                /* Setup default param requirements according to Msg ID */
                switch (Server->msg.type)
                {

                    case SIM_TRANSFER_APDU_REQ:
                        Server->parser.parmsExpected[0] = SIM_PARM_CMD_APDU;
                        Server->parser.parmsExpectedLen = 1;
                        break;

                    case SIM_CONNECT_REQ:
                        Server->parser.parmsExpected[0] = SIM_PARM_MAX_MSG_SIZE;
                        Server->parser.parmsExpectedLen = 1;
                        break;

                    case SIM_DISCONNECT_REQ:
                    case SIM_TRANSFER_ATR_REQ:
                    case SIM_POWER_OFF_REQ:
                    case SIM_POWER_ON_REQ:
                    case SIM_RESET_REQ:
                    case SIM_CARD_RDR_STATUS_REQ:
                        /* No expected params */
                        break;

                    case SIM_SET_TRANS_PROT_REQ:
                        Server->parser.parmsExpected[0] = SIM_PARM_TRANS_PROT;
                        Server->parser.parmsExpectedLen = 1;
                        break;

                    default:
                        kal_trace(TRACE_GROUP_1, BT_SIMAP_UNKNOWN_MSGID, Server->msg.type);
                        Server->msg.type = SIM_ERROR_IND;
                        break;
                }

                /* NO BREAK; continue by checking for any parms */
                Server->parser.state = SIM_PARSE_PARM;

            case SIM_PARSE_PARM:
            case SIM_PARSE_PARM_HEADER:
            case SIM_PARSE_PARM_VALUE:
                if (SimParseParms(&Server->parser) == BT_STATUS_SUCCESS)
                {
                    /* Reset parser for a later message */
                    SimServerResetParser(Server);

                    /* Send message up to the state machine */
                    SIM(serverState)[Server->state] (Server, &Server->msg);
                }
                break;

            default:
                /* Invalid state */
                Assert(0);
                break;
        }
    }
    if (rxLen != 0)
    {
        Assert(0);
    }
}


/*****************************************************************************
 * FUNCTION
 *  SimServerRfCallback
 * DESCRIPTION
 *  Server control channel callback routine for RFCOMM connections.
 * PARAMETERS
 *  Channel     [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SimServerRfCallback(RfChannel *Channel, RfCallbackParms *Parms)
{
    SimServer *server = ContainingRecord(Channel, SimServer, rfChannel);

    switch (Parms->event)
    {

        case RFEVENT_CLOSED:
            Report(("SIMAP: RF Callback RFEVENT_CLOSED.\n"));
            /* Link losss, clean up */
            SIM(transportState) = SIM_TRANSPORT_DOWN;
            SimServerClearConnection(server);
            break;

        case RFEVENT_DATA_IND:
            Report(("SIMAP: RF Callback RFEVENT_DATA_IND.\n"));
            /* Parse incoming request */
            AssertEval(RF_AdvanceCredit(Channel, 1) == BT_STATUS_SUCCESS);
            SimServerParseReq(server, Parms->dataLen, Parms->ptrs.data);
            break;

        case RFEVENT_PACKET_HANDLED:
            Report(("SIMAP: RF Callback RFEVENT_PACKET_HANDLED.\n"));
            /* Packet handled advances some states */
            server->msg.type = SIM_PACKET_HANDLED;
            server->msg.parm.packet = Parms->ptrs.packet;

            /* Call the appropriate state function */
            SIM(serverState)[server->state] (server, &server->msg);
            break;

        case RFEVENT_OPEN_IND:
            Report(("SIMAP: RF Callback RFEVENT_OPEN_IND.\n"));
			
#if 0
            /* Accept the connection */
            RF_AcceptChannel(Channel);
#else
            server->remDev = Parms->ptrs.remDev;
            SimServerAuthorize(Channel, Parms->ptrs.remDev->bdAddr);
#endif
            break;

        case RFEVENT_OPEN:
        	{
        		static U8 openCount = 0;
			Report(("SIMAP: RF Callback RFEVENT_OPEN (%d).\n", openCount));
					openCount++;
            /* Save the remote device */
            SIM(transportState) = SIM_TRANSPORT_UP;
            server->remDev = Parms->ptrs.remDev;
            CMGR_SetRemoteAsMaster(server->remDev);
            /* Set up the parser */
            SimServerResetParser(server);
        	}
            break;
    }
}

#endif /* SIM_SERVER == XA_ENABLED */

#if SIM_CLIENT == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  SimClientCmgrCallback
 * DESCRIPTION
 *  Handles connection manager messages received by the SIM Client.
 * PARAMETERS
 *  Handler     [IN]        
 *  Event       [IN]        
 *  Status      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SimClientCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status)
{
    SimClient *client = ContainingRecord(Handler, SimClient, cmgrHandler);

    switch (Event)
    {
        case CMEVENT_DATA_LINK_CON_CNF:

            if (Status == BT_STATUS_SUCCESS)
            {
                /* If connection is open, begin service query */
                Status = SimStartServiceQuery(client, BSQM_FIRST);
                if (Status == BT_STATUS_PENDING)
                {
                    client->flags |= SIM_CLIENT_FLAG_QUERY_SDP;
                    break;
                }
            }

            /* Fall through to failure case */
            SimClientClearConnection(client);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  SimClientStateOpen
 * DESCRIPTION
 *  Open state for the SIM client.
 * PARAMETERS
 *  Client      [IN]        
 *  Msg         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void SimClientStateOpen(SimClient *Client, SimMsg *Msg)
{
    SimEvent event;

    switch (Msg->type)
    {
        case SIM_DISCONNECT_IND:
            /* Disconnect */
            if (Msg->parm.discType == SIM_DISCONNECT_GRACEFUL)
            {
                /*
                 * Notify the application, remain open.  It is up to the app to
                 * close the connection by calling SIM_ClientClose.
                 */
                Report(("SIM:  Client received a graceful disconnect indication\n"));
                SimClientAppCallback(
                    Client,
                    SAEVENT_CLIENT_CLOSE_IND,
                    BT_STATUS_SUCCESS,
                    CMGR_GetRemoteDevice(&Client->cmgrHandler),
                    sizeof(BtRemoteDevice));
            }
            else
            {
                /* The server has terminated the connection. */
                Report(("SIM:  Client received an immediate disconnect indication\n"));
                RF_CloseChannel(&Client->rfChannel);
            }
            break;
        case SIM_TRANSFER_ATR_RSP:
            if (Client->flags & SIM_CLIENT_FLAG_ATR)
            {
                Report(("SIM:  Received a response to the ATR request\n"));
                Client->flags &= ~(SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_ATR);
                SimClientAppCallback(Client, SAEVENT_CLIENT_ATR_RESP, BT_STATUS_SUCCESS, Msg, sizeof(SimMsg));
            }
            else
            {
                Report(("SIM:  Received an ATR response, but no request was outstanding\n"));
                RF_CloseChannel(&Client->rfChannel);
            }
            break;
        case SIM_TRANSFER_APDU_RSP:
            if (Client->flags & SIM_CLIENT_FLAG_APDU)
            {
                Report(("SIM:  Received a response to the APDU request\n"));
                Client->flags &= ~(SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_APDU);
                SimClientAppCallback(Client, SAEVENT_CLIENT_APDU_RESP, BT_STATUS_SUCCESS, Msg, sizeof(SimMsg));
            }
            else
            {
                Report(("SIM:  Received an APDU response, but no request was outstanding\n"));
                RF_CloseChannel(&Client->rfChannel);
            }
            break;
        case SIM_POWER_OFF_RSP:
            if (Client->flags & SIM_CLIENT_FLAG_SIM_OFF)
            {
                Report(("SIM:  Received a response to the SIM Power Off request\n"));
                Client->flags &= ~(SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_SIM_OFF);
                SimClientAppCallback(Client, SAEVENT_CLIENT_SIM_OFF_RESP, BT_STATUS_SUCCESS, Msg, sizeof(SimMsg));
            }
            else
            {
                Report(("SIM:  Received an Power Off response, but no request was outstanding\n"));
                RF_CloseChannel(&Client->rfChannel);
            }
            break;
        case SIM_POWER_ON_RSP:
            if (Client->flags & SIM_CLIENT_FLAG_SIM_ON)
            {
                Report(("SIM:  Received a response to the SIM Power On request\n"));
                Client->flags &= ~(SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_SIM_ON);

                if (Msg->resultCode == SIM_RESULT_OK)
                {
                    /* Must now get the ATR */
                    Report(("SIM:  Getting ATR\n"));
                    AssertEval(SIM_ClientAtrReq(Client) == BT_STATUS_PENDING);
                }

                /* Notify the appliation */
                SimClientAppCallback(Client, SAEVENT_CLIENT_SIM_ON_RESP, BT_STATUS_SUCCESS, Msg, sizeof(SimMsg));
            }
            else
            {
                Report(("SIM:  Received an Power On response, but no request was outstanding\n"));
                RF_CloseChannel(&Client->rfChannel);
            }
            break;
        case SIM_RESET_RSP:
            if (Client->flags & SIM_CLIENT_FLAG_RESET_SIM)
            {
                Report(("SIM:  Received a response to the ResetSIM request\n"));
                Client->flags &= ~(SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_RESET_SIM);

                if (Msg->resultCode == SIM_RESULT_OK)
                {
                    /* Must now get the ATR */
                    Report(("SIM:  Getting ATR\n"));
                    AssertEval(SIM_ClientAtrReq(Client) == BT_STATUS_PENDING);
                }

                /* Notify the application */
                SimClientAppCallback(Client, SAEVENT_CLIENT_RESET_SIM_RESP, BT_STATUS_SUCCESS, Msg, sizeof(SimMsg));
            }
            else
            {
                Report(("SIM:  Received a Reset response, but no request was outstanding\n"));
                RF_CloseChannel(&Client->rfChannel);
            }
            break;
        case SIM_CARD_RDR_STATUS_RSP:
            if (Client->flags & SIM_CLIENT_FLAG_CARD_RDR_STATUS)
            {
                Report(("SIM:  Received a response to the Card Reader Status request\n"));
                Client->flags &= ~(SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_CARD_RDR_STATUS);
                SimClientAppCallback(
                    Client,
                    SAEVENT_CLIENT_CARD_READER_STATUS_RESP,
                    BT_STATUS_SUCCESS,
                    Msg,
                    sizeof(SimMsg));
            }
            else
            {
                Report(("SIM:  Received a Card Reader Status response, but no request was outstanding\n"));
                RF_CloseChannel(&Client->rfChannel);
            }
            break;
        case SIM_SET_TRANS_PROT_RSP:
            if (Client->flags & SIM_CLIENT_FLAG_SET_TRANS_PROT)
            {
                Report(("SIM:  Received a response to the Set Transport Protocol request\n"));
                Client->flags &= ~(SIM_CLIENT_FLAG_PROCESSING | SIM_CLIENT_FLAG_SET_TRANS_PROT);
                SimClientAppCallback(
                    Client,
                    SAEVENT_CLIENT_SET_TRANS_PROT_RESP,
                    BT_STATUS_SUCCESS,
                    Msg,
                    sizeof(SimMsg));
            }
            else
            {
                Report(("SIM:  Received a Set Transport Protocol response, but no request was outstanding\n"));
                RF_CloseChannel(&Client->rfChannel);
            }
            break;
        case SIM_STATUS_IND:
            Report(("SIM:  Received a Status Indication\n"));
            SimClientAppCallback(Client, SAEVENT_CLIENT_STATUS_IND, BT_STATUS_SUCCESS, Msg, sizeof(SimMsg));
            break;

        case SIM_PACKET_HANDLED:
            /* Ignore wrong packets */
            if (Client->msg.parm.packet != &Client->simPacket.packet)
            {
                break;
            }

            if (Client->simPacket.dataToSend != 0)
            {
                /* Send more data */
                if (SimSendPacketSegment(&Client->simPacket, &Client->rfChannel) != BT_STATUS_PENDING)
                {
                    Client->simPacket.dataToSend = 0;
                    Client->simPacket.packet.flags = 0;
                    Client->flags &= ~SIM_CLIENT_FLAG_PROCESSING;

                    if (Client->flags & SIM_CLIENT_FLAG_ATR)
                    {
                    }
                    else if (Client->flags & SIM_CLIENT_FLAG_ATR)
                    {
                        Client->flags &= ~SIM_CLIENT_FLAG_ATR;
                        event = SAEVENT_CLIENT_ATR_RESP;
                    }
                    else if (Client->flags & SIM_CLIENT_FLAG_APDU)
                    {
                        Client->flags &= ~SIM_CLIENT_FLAG_APDU;
                        event = SAEVENT_CLIENT_APDU_RESP;
                    }
                    else if (Client->flags & SIM_CLIENT_FLAG_SIM_ON)
                    {
                        Client->flags &= ~SIM_CLIENT_FLAG_SIM_ON;
                        event = SAEVENT_CLIENT_SIM_ON_RESP;
                    }
                    else if (Client->flags & SIM_CLIENT_FLAG_SIM_OFF)
                    {
                        Client->flags &= ~SIM_CLIENT_FLAG_SIM_OFF;
                        event = SAEVENT_CLIENT_SIM_OFF_RESP;
                    }
                    else if (Client->flags & SIM_CLIENT_FLAG_RESET_SIM)
                    {
                        Client->flags &= ~SIM_CLIENT_FLAG_RESET_SIM;
                        event = SAEVENT_CLIENT_RESET_SIM_RESP;
                    }
                    else if (Client->flags & SIM_CLIENT_FLAG_CARD_RDR_STATUS)
                    {
                        Client->flags &= ~SIM_CLIENT_FLAG_CARD_RDR_STATUS;
                        event = SAEVENT_CLIENT_CARD_READER_STATUS_RESP;
                    }
                    else if (Client->flags & SIM_CLIENT_FLAG_SET_TRANS_PROT)
                    {
                        Client->flags &= ~SIM_CLIENT_FLAG_SET_TRANS_PROT;
                        event = SAEVENT_CLIENT_SET_TRANS_PROT_RESP;
                    }
                    else
                    {
                        Report(("SIM:  Unknown APDU packet sent\n"));
                        RF_CloseChannel(&Client->rfChannel);
                        break;
                    }

                    SimClientAppCallback(Client, event, BT_STATUS_FAILED, Msg, sizeof(SimMsg));

                }
            }
            else
            {
                Client->simPacket.packet.flags = 0;
            }
            break;
        case SIM_ERROR_IND:
        case SIM_ERROR_RSP:
            Report(("SIM:  An error was detected in the request or the response\n"));
            /* Drop through to next state */
        default:
            Report(("SIM:  Client OPEN state received an unhandled event: %02X\n", Msg->type));
            RF_CloseChannel(&Client->rfChannel);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  SimClientStateConnPending
 * DESCRIPTION
 *  Connect Pending state for the SIM Access client.
 * PARAMETERS
 *  Client      [IN]        
 *  Msg         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void SimClientStateConnPending(SimClient *Client, SimMsg *Msg)
{
    U8 maxMsgSize[2];

    switch (Msg->type)
    {
        case SIM_CONNECT_RSP:
            if (Msg->connStatus == SIM_CONN_STATUS_OK)
            {
                /* Connect response was OK, now wait for STATUS_IND */
                Client->flags |= SIM_CLIENT_FLAG_STATUS;
            }
            else
            {
                /* Error on the connect response */
                if ((Msg->connStatus == SIM_CONN_STATUS_MSG_SIZE) && (Msg->numParms == 2))
                {
                    Report(("SIM:  Client received a connect response with new message size %d, retrying\n",
                            Msg->parm.maxMsgSize));
                    /* Retry with the message size suggested */
                    StoreBE16(maxMsgSize, Msg->parm.maxMsgSize);
                    Client->simPacket.maxMsgSize = Msg->parm.maxMsgSize;
                    AssertEval(SimClientSendConnReq(Client, maxMsgSize) == BT_STATUS_PENDING);
                }
                else
                {
                    /* Connection failed */
                    Report(("SIM:  Client connection failed\n"));
                    RF_CloseChannel(&Client->rfChannel);
                }
            }
            break;
        case SIM_STATUS_IND:
            if (Client->flags & SIM_CLIENT_FLAG_STATUS)
            {
                Client->flags &= ~SIM_CLIENT_FLAG_STATUS;
                if (Msg->parm.statusChange != SIM_CARD_STATUS_RESET)
                {
                    /* Connection established */
                    Report(("SIM:  Client connection established\n"));
                    Client->state = SIM_CLIENT_STATE_OPEN;
                    SimClientAppCallback(
                        Client,
                        SAEVENT_CLIENT_OPEN,
                        BT_STATUS_SUCCESS,
                        CMGR_GetRemoteDevice(&Client->cmgrHandler),
                        sizeof(BtRemoteDevice));
                }
                else
                {
                    /* Must get the ATR */
                    Report(("SIM:  Client getting ATR\n"));
                    AssertEval(SimClientSendAtrReq(Client) == BT_STATUS_PENDING);
                    Client->flags |= SIM_CLIENT_FLAG_ATR;
                }
            }
            else
            {
                /* Error */
                Report(("SIM:  Client CONN_PENDING state received an unexpected event: %02X\n", Msg->type));
                RF_CloseChannel(&Client->rfChannel);
            }
            break;
        case SIM_TRANSFER_ATR_RSP:
            if (Client->flags & SIM_CLIENT_FLAG_ATR)
            {
                /* Notify the application that an ATR response was received */
                SimClientAppCallback(Client, SAEVENT_CLIENT_ATR_RESP, BT_STATUS_SUCCESS, Msg, sizeof(SimMsg));

                Client->flags &= ~SIM_CLIENT_FLAG_ATR;

                /* Connection established */
                Report(("SIM:  Client connection established\n"));
                Client->state = SIM_CLIENT_STATE_OPEN;
                SimClientAppCallback(
                    Client,
                    SAEVENT_CLIENT_OPEN,
                    BT_STATUS_SUCCESS,
                    CMGR_GetRemoteDevice(&Client->cmgrHandler),
                    sizeof(BtRemoteDevice));
            }
            else
            {
                /* Error */
                Report(("SIM:  Client CONN_PENDING state received an unexpected event: %02X\n", Msg->type));
                RF_CloseChannel(&Client->rfChannel);
            }
            break;
        case SIM_PACKET_HANDLED:
            /* Ignore wrong packets */
            if (Client->msg.parm.packet != &Client->simPacket.packet)
            {
                break;
            }

            if (Client->simPacket.dataToSend != 0)
            {
                /* Send more data */
                if (SimSendPacketSegment(&Client->simPacket, &Client->rfChannel) != BT_STATUS_PENDING)
                {
                    Client->simPacket.packet.flags = 0;
                    Client->simPacket.dataToSend = 0;
                    RF_CloseChannel(&Client->rfChannel);
                }
            }
            else
            {
                Client->simPacket.packet.flags = 0;
            }
            break;
        case SIM_ERROR_IND:
        case SIM_ERROR_RSP:
            Report(("SIM:  An error was detected in the request or the response\n"));
            /* Drop through to next case */
        default:
            Report(("SIM:  Client CONN_PENDING state received an unexpected event: %02X\n", Msg->type));
            RF_CloseChannel(&Client->rfChannel);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  SimClientStateDiscPending
 * DESCRIPTION
 *  Disconnect Pending state for the SIM Access client.
 * PARAMETERS
 *  Client      [IN]        
 *  Msg         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void SimClientStateDiscPending(SimClient *Client, SimMsg *Msg)
{
    switch (Msg->type)
    {
        case SIM_DISCONNECT_RSP:
            /* The connection is down, clean up the connection state */
            Report(("SIM:  Client connection closed\n"));
            RF_CloseChannel(&Client->rfChannel);
            break;
        case SIM_PACKET_HANDLED:
            /* Ignore wrong packets */
            if (Client->msg.parm.packet != &Client->simPacket.packet)
            {
                break;
            }

            if (Client->simPacket.dataToSend != 0)
            {
                /* Send more data */
                if (SimSendPacketSegment(&Client->simPacket, &Client->rfChannel) != BT_STATUS_PENDING)
                {
                    Client->simPacket.packet.flags = 0;
                    Client->simPacket.dataToSend = 0;
                    RF_CloseChannel(&Client->rfChannel);
                }
            }
            else
            {
                Client->simPacket.packet.flags = 0;
            }
            break;
        case SIM_ERROR_IND:
        case SIM_ERROR_RSP:
            Report(("SIM:  An error was detected in the request or the response\n"));
            /* Drop through to next case */
        default:
            Report(("SIM:  Client DISC_PENDING state received an unexpected event: %02X\n", Msg->type));
            RF_CloseChannel(&Client->rfChannel);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  SimClientStateClosed
 * DESCRIPTION
 *  Closed state for the SIM Access client.
 * PARAMETERS
 *  Client      [IN]        
 *  Msg         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void SimClientStateClosed(SimClient *Client, SimMsg *Msg)
{
    U8 maxMsgSize[2];

    switch (Msg->type)
    {
        case SIM_TRANSPORT_OPEN:
            /* Send the connect request */
            Report(("SIM:  Client transport is up\n"));
            StoreBE16(maxMsgSize, SIM_MAX_MSG_SIZE);
            Client->simPacket.maxMsgSize = SIM_MAX_MSG_SIZE;
            AssertEval(SimClientSendConnReq(Client, maxMsgSize) == BT_STATUS_PENDING);
            Client->state = SIM_CLIENT_STATE_CONN_PENDING;
            break;
        default:
            Report(("SIM:  Client CLOSED state received an unexpected event: %02X\n", Msg->type));
            RF_CloseChannel(&Client->rfChannel);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  SimClientResetParser
 * DESCRIPTION
 *  Delivers the msg in Client to the state machine, and resets the parser
 *  for the next item.
 * PARAMETERS
 *  Client      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void SimClientResetParser(SimClient *Client)
{
    Client->parser.state = SIM_PARSE_HEADER;
    SimStageInit(&Client->parser.stager, Client->parser.stageBuff, 4);
}


/*****************************************************************************
 * FUNCTION
 *  SimClientParseRsp
 * DESCRIPTION
 *  Parse the response from the server and call the state machine.
 * PARAMETERS
 *  Client      [IN]        
 *  rxLen       [IN]        
 *  rxBuff      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void SimClientParseRsp(SimClient *Client, U16 rxLen, U8 *rxBuff)
{
    while ((SimStageInput(&Client->parser.stager, &rxBuff, &rxLen) == BT_STATUS_SUCCESS))
    {

        switch (Client->parser.state)
        {
            case SIM_PARSE_HEADER:
                /* Initialize the client message */
                OS_MemSet((U8*) & Client->msg, 0, sizeof(SimMsg));
                Client->msg.type = Client->parser.stageBuff[0];
                Client->msg.numParms = Client->parser.stageBuff[1];
                Client->parser.msg = &Client->msg;

                /* By default, expect no parameters */
                Client->parser.parmsExpectedLen = 0;

                /* Setup default param requirements according to Msg ID */
                switch (Client->msg.type)
                {
                    case SIM_CONNECT_RSP:
                        Client->parser.parmsExpected[0] = SIM_PARM_CONN_STATUS;
                        Client->parser.parmsExpectedLen = 1;

                        /*
                         * Note: SIM_PARM_MAX_MSG_SIZE is only expected if CONN_STATUS 
                         * * indicates it.
                         */
                        break;

                    case SIM_DISCONNECT_IND:
                        Client->parser.parmsExpected[0] = SIM_PARM_DISC_TYPE;
                        Client->parser.parmsExpectedLen = 1;
                        break;

                    case SIM_TRANSFER_APDU_RSP:
                        Client->parser.parmsExpected[0] = SIM_PARM_RESULT_CODE;
                        /* Note: if result is failure, APDU should not appear */
                        Client->parser.parmsExpected[1] = SIM_PARM_RSP_APDU;
                        Client->parser.parmsExpectedLen = 2;
                        break;

                    case SIM_TRANSFER_ATR_RSP:
                        Client->parser.parmsExpected[0] = SIM_PARM_RESULT_CODE;
                        /* Note: if result is failure, ATR should not appear */
                        Client->parser.parmsExpected[1] = SIM_PARM_ATR;
                        Client->parser.parmsExpectedLen = 2;
                        break;

                    case SIM_POWER_ON_RSP:
                    case SIM_POWER_OFF_RSP:
                    case SIM_RESET_RSP:
                    case SIM_SET_TRANS_PROT_RSP:
                        Client->parser.parmsExpected[0] = SIM_PARM_RESULT_CODE;
                        Client->parser.parmsExpectedLen = 1;
                        break;

                    case SIM_STATUS_IND:
                        Client->parser.parmsExpected[0] = SIM_PARM_STATUS_CHANGE;
                        Client->parser.parmsExpectedLen = 1;
                        break;

                    case SIM_CARD_RDR_STATUS_RSP:
                        Client->parser.parmsExpected[0] = SIM_PARM_RESULT_CODE;
                        /* Note: if result is failure, CARD_RDR_STATUS will not appear */
                        Client->parser.parmsExpected[1] = SIM_PARM_CARD_RDR_STATUS;
                        Client->parser.parmsExpectedLen = 2;
                        break;

                    case SIM_DISCONNECT_RSP:
                    case SIM_ERROR_RSP:
                        break;
                }

                /* NO BREAK; continue by checking for any parms */
                Client->parser.state = SIM_PARSE_PARM;

            case SIM_PARSE_PARM:
            case SIM_PARSE_PARM_HEADER:
            case SIM_PARSE_PARM_VALUE:
                if (SimParseParms(&Client->parser) == BT_STATUS_SUCCESS)
                {
                    /* Reset parser for a later message */
                    SimClientResetParser(Client);

                    /* Send message up to the state machine */
                    SIM(clientState)[Client->state] (Client, &Client->msg);
                }
                break;

            default:
                /* Invalid state */
                Assert(0);
                break;
        }
    }

    Assert(rxLen == 0);
}


/*****************************************************************************
 * FUNCTION
 *  SimClientRfCallback
 * DESCRIPTION
 *  Client control channel callback routine for RFCOMM connections.
 * PARAMETERS
 *  Channel     [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SimClientRfCallback(RfChannel *Channel, RfCallbackParms *Parms)
{
    SimClient *client = ContainingRecord(Channel, SimClient, rfChannel);

    switch (Parms->event)
    {

        case RFEVENT_CLOSED:
            /* Link losss, clean up */
            SIM(transportState) = SIM_TRANSPORT_DOWN;
            SimClientClearConnection(client);
            break;

        case RFEVENT_DATA_IND:
            /* Parse incoming request */
            AssertEval(RF_AdvanceCredit(Channel, 1) == BT_STATUS_SUCCESS);
            SimClientParseRsp(client, Parms->dataLen, Parms->ptrs.data);
            break;

        case RFEVENT_PACKET_HANDLED:
            /* This event advance some states */
            client->msg.type = SIM_PACKET_HANDLED;
            client->msg.parm.packet = Parms->ptrs.packet;

            /* Call the appropriate state function */
            SIM(clientState)[client->state] (client, &client->msg);
            break;

        case RFEVENT_OPEN:
            /* This event advance some states */
            SIM(transportState) = SIM_TRANSPORT_UP;
            client->msg.type = SIM_TRANSPORT_OPEN;
            SimClientResetParser(client);

            /* Call the appropriate state function */
            SIM(clientState)[client->state] (client, &client->msg);
            break;
    }

}

#endif /* SIM_CLIENT == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  SimInitStateMachine
 * DESCRIPTION
 *  Initializes state machine tables.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void SimInitStateMachine(void)
{

#if SIM_SERVER == XA_ENABLED
    SIM(serverState)[SIM_SERVER_STATE_CLOSED] = SimServerStateClosed;
    SIM(serverState)[SIM_SERVER_STATE_CONN_PENDING] = SimServerStateConnPending;
    SIM(serverState)[SIM_SERVER_STATE_OPEN] = SimServerStateOpen;
    SIM(serverState)[SIM_SERVER_STATE_DISC_PENDING] = SimServerStateDiscPending;
#endif /* SIM_SERVER == XA_ENABLED */

#if SIM_CLIENT == XA_ENABLED
    SIM(clientState)[SIM_CLIENT_STATE_CLOSED] = SimClientStateClosed;
    SIM(clientState)[SIM_CLIENT_STATE_CONN_PENDING] = SimClientStateConnPending;
    SIM(clientState)[SIM_CLIENT_STATE_OPEN] = SimClientStateOpen;
    SIM(clientState)[SIM_CLIENT_STATE_DISC_PENDING] = SimClientStateDiscPending;
#endif /* SIM_CLIENT == XA_ENABLED */

}


/*****************************************************************************
 * FUNCTION
 *  SimStageInput
 * DESCRIPTION
 *  Stages data into previously specified staging buffer.
 * PARAMETERS
 *  stager          [IN]        
 *  inputBuff       [IN]        
 *  inputLen        [IN]        
 * RETURNS
 *  BT_STATUS_PENDING or _SUCCESS.
 *****************************************************************************/
BtStatus SimStageInput(SimStager *stager, U8 **inputBuff, U16 *inputLen)
{
    U16 len;

    /*
     * If the target is zero we're in auto-success mode. This allows
     * state machines to call SimStageInput repeatedly with SUCCESS.
     */
    if (stager->target == 0)
    {
        return BT_STATUS_SUCCESS;
    }

    Assert(stager->target >= stager->cur);

    len = min(*inputLen, stager->target - stager->cur);

    if (!stager->skip)
    {
        OS_MemCopy(stager->buff + stager->cur, *inputBuff, len);
    }

    stager->cur += len;
    *inputBuff += len;
    *inputLen -= len;

    if (stager->cur == stager->target)
    {
        stager->target = 0;
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_PENDING;
}

