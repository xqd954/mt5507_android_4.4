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
 *     $Workfil$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description:
 *     Utility functions for the SIM Access protocol.
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

/* if the bluetooth_struct.h is included in your C file, also the uart_sw.h must be included. */
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
#include "kal_trace.h"  /* Trace Header Files */
//#include "bt_maui_utility.h"
#endif
#include "bt_common.h"
#include "stdio.h"      /* Basic C I/O functions */
#include "bt.h"
#include "bluetooth_struct.h"
#include "bluetooth_trc.h"
#include "rfcomm.h"
#include "bt_simi.h"

#if SIM_SERVER == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  SimServerAppCallback
 * DESCRIPTION
 *  Calls the application with Server events.
 * PARAMETERS
 *  Server      [IN]        
 *  Event       [IN]        
 *  Status      [IN]        
 *  ptr         [IN]        
 *  len         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SimServerAppCallback(SimServer *Server, U8 Event, BtStatus Status, void *ptr, U16 len)
{

    SimCallbackParms parms;

    parms.event = Event;
    parms.status = Status;
    parms.ptrs.msg = ptr;
    parms.len = len;
    Server->callback(Server, &parms);

}


/*****************************************************************************
 * FUNCTION
 *  SimServerClearConnection
 * DESCRIPTION
 *  Clears the connection state and notifies the application.
 * PARAMETERS
 *  Server      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SimServerClearConnection(SimServer *Server)
{
    /* Clean up the server state */
    Server->state = SIM_SERVER_STATE_CLOSED;

    /* Handle any unsent/unacknowledged requests */
    if (Server->flags & SIM_SERVER_FLAG_STATUS_PENDING)
    {
        SimServerAppCallback(Server, SAEVENT_SERVER_STATUS_SENT, BT_STATUS_NO_CONNECTION, 0, 0);
    }

    /* Clear up the server's flag */
    Server->flags = 0;

    /* Report(("In SimServerClearConnection(), substate=%d",SIM(simServer)[0].substate)); */

    /* Call the application */
    {
        /*
         * Note: SimServer in CLOSED state, but it needs to check CLOSED event!
         * Because when SimServer is in CLOSED state, but Car kit connects it then user press Deactivate immediately
         * So RFCOMM is in CON_PENDING, but user requests to deactivate
         * RF_CloseChannel() is used to close RFCOMM channel and free its DLC, so RFCLOSED_EVENT is callbacked 
         */
        SimServerAppCallback(Server, SAEVENT_SERVER_CLOSED, BT_STATUS_SUCCESS, Server->remDev, sizeof(BtRemoteDevice));
    }
}

#endif /* SIM_SERVER == XA_ENABLED */

#if SIM_CLIENT == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  SimClientAppCallback
 * DESCRIPTION
 *  Calls the application with Client events.
 * PARAMETERS
 *  Client      [IN]        
 *  Event       [IN]        
 *  Status      [IN]        
 *  ptr         [IN]        
 *  len         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SimClientAppCallback(SimClient *Client, U8 Event, BtStatus Status, void *ptr, U16 len)
{

    SimCallbackParms parms;

    parms.event = Event;
    parms.status = Status;
    parms.ptrs.msg = ptr;
    parms.len = len;
    Client->callback(Client, &parms);

}


/*****************************************************************************
 * FUNCTION
 *  SimClientClearConnection
 * DESCRIPTION
 *  Clears the connection state and notifies the application.
 * PARAMETERS
 *  Client      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SimClientClearConnection(SimClient *Client)
{
    SimEvent event = 0;

    /* Clean up the server state */
    Client->state = SIM_CLIENT_STATE_CLOSED;

    /* Handle any unsent/unacknowledged requests */
    if (Client->flags & SIM_CLIENT_FLAG_APDU)
    {
        event = SAEVENT_CLIENT_APDU_RESP;
    }
    else if (Client->flags & SIM_CLIENT_FLAG_ATR)
    {
        event = SAEVENT_CLIENT_ATR_RESP;
    }
    else if (Client->flags & SIM_CLIENT_FLAG_SIM_ON)
    {
        event = SAEVENT_CLIENT_SIM_ON_RESP;
    }
    else if (Client->flags & SIM_CLIENT_FLAG_SIM_OFF)
    {
        event = SAEVENT_CLIENT_SIM_OFF_RESP;
    }
    else if (Client->flags & SIM_CLIENT_FLAG_RESET_SIM)
    {
        event = SAEVENT_CLIENT_RESET_SIM_RESP;
    }
    else if (Client->flags & SIM_CLIENT_FLAG_CARD_RDR_STATUS)
    {
        event = SAEVENT_CLIENT_CARD_READER_STATUS_RESP;
    }
    else if (Client->flags & SIM_CLIENT_FLAG_SET_TRANS_PROT)
    {
        event = SAEVENT_CLIENT_SET_TRANS_PROT_RESP;
    }

    if (event)
    {
        SimClientAppCallback(Client, event, BT_STATUS_NO_CONNECTION, 0, 0);
    }

    Client->flags = 0;

    /* Disassociate the connection from the connection manager */
    AssertEval(BT_STATUS_SUCCESS == CMGR_RemoveDataLink(&Client->cmgrHandler));

    /* Call the application */
    SimClientAppCallback(
        Client,
        SAEVENT_CLIENT_CLOSED,
        BT_STATUS_SUCCESS,
        CMGR_GetRemoteDevice(&Client->cmgrHandler),
        sizeof(BtRemoteDevice));
}
#endif /* SIM_CLIENT == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  SimSendPacket
 * DESCRIPTION
 *  Sends an internal packet.
 * PARAMETERS
 *  SPacket         [IN]        
 *  Channel         [IN]        
 *  MsgType         [IN]        
 *  Parm1Type       [IN]        
 *  Parm1Len        [IN]        
 *  Parm1           [IN]        
 *  Parm2Type       [IN]        
 *  Parm2Len        [IN]        
 *  Parm2           [IN]        
 * RETURNS
 *  BT_STATUS_BUSY - The packet is already in use.
 *****************************************************************************/
BtStatus SimSendPacket(
            SimPacket *SPacket,
            RfChannel *Channel,
            U8 MsgType,
            U8 Parm1Type,
            U16 Parm1Len,
            U8 *Parm1,
            U8 Parm2Type,
            U16 Parm2Len,
            U8 *Parm2)
{
    /* See if the packet is in use already */
    if (SPacket->packet.flags & BTP_FLAG_INUSE)
    {
        /* Report(("In SimSendPacket(): its flags is BTP_FLAG_INUSE")); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_PKT_INUSE);
        return BT_STATUS_BUSY;
    }

    /* Clear the packet */
    SPacket->packet.data = SPacket->packetData;
    OS_MemSet(SPacket->packet.data, 0, SIM_MAX_APDU);

    /* Set up the message header */
    SPacket->packet.data[0] = MsgType;
    SPacket->packet.dataLen = 4;

    if (Parm1 != 0)
    {
        SPacket->packet.data[1] = 1;

        /* Set up the first parameter */
        SPacket->packet.data[4] = Parm1Type;
        StoreBE16(&(SPacket->packet.data[6]), Parm1Len);
        OS_MemCopy(&(SPacket->packet.data[8]), Parm1, Parm1Len);
        SPacket->packet.dataLen = 8 + Parm1Len;
        if (Parm1Len % 4)
        {
            SPacket->packet.dataLen += (4 - Parm1Len % 4);
        }

        if (Parm2 != 0)
        {
            /* Set up the second parameter */
            SPacket->packet.data[1]++;
            SPacket->packet.data[SPacket->packet.dataLen] = Parm2Type;
            StoreBE16(&(SPacket->packet.data[SPacket->packet.dataLen + 2]), Parm2Len);
            OS_MemCopy(&(SPacket->packet.data[SPacket->packet.dataLen + 4]), Parm2, Parm2Len);
            SPacket->packet.dataLen += 4 + Parm2Len;
            if (Parm2Len % 4)
            {
                SPacket->packet.dataLen += (4 - Parm2Len % 4);
            }
        }
    }

    /* Send the packet */
    SPacket->packet.flags = BTP_FLAG_INUSE;
    SPacket->dataToSend = SPacket->packet.dataLen;
    SPacket->packet.dataLen = min(SPacket->packet.dataLen, SPacket->maxMsgSize);
    SPacket->packet.dataLen = min(SPacket->packet.dataLen, RF_FrameSize(Channel));
    /* Add the priority type setting : 2007-0115 */
    SPacket->packet.priority_type = BTP_FLAG_HIGH_PRIPORTY;
    SPacket->dataToSend -= SPacket->packet.dataLen;
    return RF_SendData(Channel, &SPacket->packet);
}


/*****************************************************************************
 * FUNCTION
 *  SimFindFreeAndSendPacket
 * DESCRIPTION
 *  Sends an internal packet.
 * PARAMETERS
 *  Server          [IN]        
 *  Channel         [IN]        
 *  MsgType         [IN]        
 *  Parm1Type       [IN]        
 *  Parm1Len        [IN]        
 *  Parm1           [IN]        
 *  Parm2Type       [IN]        
 *  Parm2Len        [IN]        
 *  Parm2           [IN]        
 * RETURNS
 *  BT_STATUS_BUSY - The packet is already in use.
 *****************************************************************************/
BtStatus SimFindFreeAndSendPacket(
            SimServer *Server,
            RfChannel *Channel,
            U8 MsgType,
            U8 Parm1Type,
            U16 Parm1Len,
            U8 *Parm1,
            U8 Parm2Type,
            U16 Parm2Len,
            U8 *Parm2)
{

    SimPacket *SPacket = NULL;
    U8 i = 0;
	BtStatus rfSendRst;

	Report(("[BT SIMAP]SimFindFreeAndSendPacket...MsgType=%d", MsgType));
	/*
	kal_trace(TRACE_GROUP_1, BT_SIMAP_SEND_PACKET, MsgType, 
					Parm1Type, Parm1==NULL?-1:*Parm1,;	
					Parm2Type, Parm2==NULL?-1:*Parm2); 
	*/

    /* Find if any free (i.e., unused packet) */
    for (i = 0; i < NUM_OF_SIMPACKETS; i++)
    {
        if (Server->simPacketList[i].packet.flags == 0)
        {
            /* Report(("In SimFindFreeAndSendPacket():Find Free packet in inx=%d",i)); */
            kal_trace(TRACE_GROUP_1, BT_SIMAP_FIND_FREE_PKT_INX, i);
            SPacket = &(Server->simPacketList[i]);
            /* Mark this packet is in used */
            SPacket->packet.flags = BTP_FLAG_INUSE;
            break;
        }
    }

    if (i == NUM_OF_SIMPACKETS)
    {
        /* Report(("In SimFindFreeAndSendPacket(): All packets of SimServer are bust ans used")); */
        kal_trace(TRACE_GROUP_1, BT_SIMAP_ALLPKT_USED);
        return BT_STATUS_BUSY;
    }

    /* Clear the packet */
    SPacket->packet.data = SPacket->packetData;
    OS_MemSet(SPacket->packet.data, 0, SIM_MAX_APDU);

    /* Set up the message header */
    SPacket->packet.data[0] = MsgType;
    SPacket->packet.dataLen = 4;

    if (Parm1 != 0)
    {
        SPacket->packet.data[1] = 1;

        /* Set up the first parameter */
        SPacket->packet.data[4] = Parm1Type;
        StoreBE16(&(SPacket->packet.data[6]), Parm1Len);
        OS_MemCopy(&(SPacket->packet.data[8]), Parm1, Parm1Len);
        SPacket->packet.dataLen = 8 + Parm1Len;
        if (Parm1Len % 4)
        {
            SPacket->packet.dataLen += (4 - Parm1Len % 4);
        }

        if (Parm2 != 0)
        {
            /* Set up the second parameter */
            SPacket->packet.data[1]++;
            SPacket->packet.data[SPacket->packet.dataLen] = Parm2Type;
            StoreBE16(&(SPacket->packet.data[SPacket->packet.dataLen + 2]), Parm2Len);
            OS_MemCopy(&(SPacket->packet.data[SPacket->packet.dataLen + 4]), Parm2, Parm2Len);
            SPacket->packet.dataLen += 4 + Parm2Len;
            if (Parm2Len % 4)
            {
                SPacket->packet.dataLen += (4 - Parm2Len % 4);
            }
        }
    }

    /* Send the packet */
    SPacket->packet.flags = BTP_FLAG_INUSE;
    SPacket->dataToSend = SPacket->packet.dataLen;
    SPacket->packet.dataLen = min(SPacket->packet.dataLen, SPacket->maxMsgSize);
    SPacket->packet.dataLen = min(SPacket->packet.dataLen, RF_FrameSize(Channel));
    /* Add the priority type setting : 2007-0115 */
    SPacket->packet.priority_type = BTP_FLAG_HIGH_PRIPORTY;
    SPacket->dataToSend -= SPacket->packet.dataLen;

	Report(("[BT SIMAP]RF_SendData ..."));
	rfSendRst = RF_SendData(Channel, &SPacket->packet);
	//kal_trace(TRACE_GROUP_1, BT_SIMAP_RFSEND_RETURN, rfSendRst);
	Report(("[BT SIMAP]RF_SendData return: %d", rfSendRst));

    return rfSendRst;
}


/*****************************************************************************
 * FUNCTION
 *  SimSendPacketSegment
 * DESCRIPTION
 *  Sends a segment of an internal packet.
 * PARAMETERS
 *  SPacket     [IN]    
 *  Channel     [IN]    
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SimSendPacketSegment(SimPacket *SPacket, RfChannel *Channel)
{
	BtStatus rfSendRst;

    /* Send more data */
    SPacket->packet.data += SPacket->packet.dataLen;
    if (SPacket->maxMsgSize == 0)
    {
        Assert(0);
    }
    SPacket->packet.dataLen = min(SPacket->dataToSend, SPacket->maxMsgSize);
    SPacket->packet.dataLen = min(SPacket->packet.dataLen, RF_FrameSize(Channel));

    if (SPacket->dataToSend < SPacket->packet.dataLen)
    {
        Assert(0);
    }
    SPacket->dataToSend -= SPacket->packet.dataLen;

    if (SPacket->packet.dataLen == 0)
    {
        Assert(0);
    }

	
	rfSendRst = RF_SendData(Channel, &SPacket->packet);
	//kal_trace(TRACE_GROUP_1, BT_SIMAP_RFSEND_RETURN, rfSendRst);
	Report(("[BT SIMAP]RF_SendData return: %d", rfSendRst));
	
    return rfSendRst;
}


/*****************************************************************************
 * FUNCTION
 *  SimApServerInCloseState
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remdev              [IN]        
 *  simsrv_state        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL SimApServerInCloseState(BtRemoteDevice *remdev, U8 *simsrv_state)
{
    SimServer *simSrv = NULL;

    simSrv = &(SIM(simServer)[0]);
    *simsrv_state = simSrv->state;

    if (remdev != (simSrv->remDev))
    {
        return TRUE;
    }
    else if ((remdev == (simSrv->remDev)) && ((simSrv->state) == SIM_SERVER_STATE_CLOSED))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

