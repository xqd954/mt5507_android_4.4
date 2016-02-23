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
 *     $Workfile:avctpcon.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains the L2CAP/ACL connection state machine 
 *              for AVCTP connections.
 *             
 * Created:     Mar 12, 2004
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
#include "l2cap.h"
#include "avctp.h"
#include "sys/avctpcon.h"
#include "sys/avctpi.h"

/* Function Prototypes */
static void AvctpFreeConn(AvctpConn *Conn);


/*****************************************************************************
 * FUNCTION
 *  AvctpConnDisconnected
 * DESCRIPTION
 *  Disconnected state.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvctpConnDisconnected(AvctpConn *Conn, L2capCallbackParms *Parms)
{
    AvctpConnCallbackParms connParms;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_STATE_DISCONNECTED_LOG, Conn->l2ChannelId);


    switch (Parms->event)
    {
        case L2EVENT_CONNECT_IND:

            /* Notify the application */
            Conn->remDev = Parms->aclLink;
            connParms.event = AVCTP_CONN_EVENT_CONNECT_IND;
            connParms.ptrs.remDev = Conn->remDev;
            Conn->callback(Conn, &connParms);
            /* AV13 */
			Conn->state = AVCTP_STATE_CONN_INCOMING; // AV13. it is a incoming state
            break;
        case L2EVENT_DISCONNECTED:
            /* Conn disconnected, notify the app */
            AvctpFreeConn(Conn);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvctpConnConnPending
 * DESCRIPTION
 *  Conn Pending state.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvctpConnConnPending(AvctpConn *Conn, L2capCallbackParms *Parms)
{
    AvctpConnCallbackParms connParms;
    BtStatus status = BT_STATUS_FAILED;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_STATE_CONN_PENDING_LOG , Conn->l2ChannelId, Parms->event);


    switch (Parms->event)
    {
        case L2EVENT_CONNECTED:
            /* Connection is up */
            Conn->state = AVCTP_STATE_CONNECTED;
            Conn->remDev = Parms->aclLink;

            /* Notify the application */
            connParms.event = AVCTP_CONN_EVENT_CONNECT;
            connParms.ptrs.remDev = Conn->remDev;
            Conn->callback(Conn, &connParms);
            break;
        case L2EVENT_CONNECT_IND:
            /* accept the incoming connection */
            status = L2CAP_ConnectRsp(Conn->l2ChannelId, L2CONN_ACCEPTED, 0);
            OS_Report("[AVCTP] ConnectRsp channelid:%d status:%d", Conn->l2ChannelId, status);
            break;

        case L2EVENT_DISCONNECTED:
            /* Conn disconnected, notify the app */
            AvctpFreeConn(Conn);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvctpConnConnIncoming
 * DESCRIPTION
 *  Connect Incoming state.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvctpConnConnIncoming(AvctpConn *Conn, L2capCallbackParms *Parms)
{
    AvctpConnCallbackParms connParms;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_STATE_CONN_INCOMING_LOG, Conn->l2ChannelId, Parms->event);

    switch (Parms->event)
    {
        case L2EVENT_CONNECTED:
            /* Connection is up */
            Conn->state = AVCTP_STATE_CONNECTED;
            Conn->remDev = Parms->aclLink;

            /* Notify the application */
            connParms.event = AVCTP_CONN_EVENT_CONNECT;
            connParms.ptrs.remDev = Conn->remDev;
            Conn->callback(Conn, &connParms);
            break;

        case L2EVENT_DISCONNECTED:
            /* Conn disconnected, notify the app */
            AvctpFreeConn(Conn);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvctpConnDiscPending
 * DESCRIPTION
 *  Disconnect Pending state.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvctpConnDiscPending(AvctpConn *Conn, L2capCallbackParms *Parms)
{
    AvctpConnCallbackParms connParms;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_STATE_DISC_PENDING_LOG, Conn->l2ChannelId, Parms->event);

    switch (Parms->event)
    {
        case L2EVENT_DISCONNECTED:
            /* Conn disconnected, notify the app */
            AvctpFreeConn(Conn);
            break;

        case L2EVENT_PACKET_HANDLED:
            /* Notify the application */
            connParms.event = AVCTP_CONN_EVENT_DATA_SENT;
            Conn->callback(Conn, &connParms);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvctpConnDiscIncoming
 * DESCRIPTION
 *  Disconnected state.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvctpConnDiscIncoming(AvctpConn *Conn, L2capCallbackParms *Parms)
{
    AvctpConnCallbackParms connParms;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_STATE_DISC_INCOMING_LOG, Conn->l2ChannelId, Parms->event);

    switch (Parms->event)
    {
        case L2EVENT_DISCONNECTED:
            /* Conn disconnected, notify the app */
            AvctpFreeConn(Conn);
            break;

        case L2EVENT_PACKET_HANDLED:
            /* Notify the application */
            connParms.event = AVCTP_CONN_EVENT_DATA_SENT;
            Conn->callback(Conn, &connParms);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvctpConnConnected
 * DESCRIPTION
 *  Connected state.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvctpConnConnected(AvctpConn *Conn, L2capCallbackParms *Parms)
{
    AvctpConnCallbackParms connParms;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_STATE_CONNECTED_LOG, Conn->l2ChannelId, Parms->event);

    switch (Parms->event)
    {
        case L2EVENT_DISCONNECTED:
            /* Conn disconnected, notify the app */
            AvctpFreeConn(Conn);
            break;

        case L2EVENT_PACKET_HANDLED:
            /* Notify the application */
            connParms.event = AVCTP_CONN_EVENT_DATA_SENT;
            Conn->callback(Conn, &connParms);
            break;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
        case L2EVENT_FLOW_DATA_IND:
            {
                BtRxPacket *rxPacket;
                
                if(IsListEmpty(&AVCTP(receivedPacketsList)) )
                {
                    Report(("Data Flow ind, rx buffer shall not be empty"));
                    Assert(0);
                }
                rxPacket = (BtRxPacket *)RemoveHeadList(&AVCTP(receivedPacketsList) );
                connParms.event = AVCTP_CONN_EVENT_DATA_IND;
                connParms.dataLen = rxPacket->dataLen;
                connParms.ptrs.data = rxPacket->data;
                Conn->callback(Conn, &connParms);
                rxPacket->dataLen = 0;
                InsertTailList(&AVCTP(rxPacketsBufferList), &rxPacket->node);
                L2CAP_UpdateSduDataBufferSeq( Conn->l2ChannelId );
            }
			break;
#endif

        case L2EVENT_DATA_IND:
            connParms.event = AVCTP_CONN_EVENT_DATA_IND;
            connParms.dataLen = Parms->dataLen;
            connParms.ptrs.data = Parms->ptrs.data;
	            Conn->callback(Conn, &connParms);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvctpConnInitStateMachine
 * DESCRIPTION
 *  Initializes the state table.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void AvctpConnInitStateMachine(void)
{
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_STATE_INIT_MACHINE_LOG);

    AVCTP(connState)[0] = AvctpConnDisconnected;
    AVCTP(connState)[1] = AvctpConnConnPending;
    AVCTP(connState)[2] = AvctpConnConnIncoming;
    AVCTP(connState)[3] = AvctpConnDiscPending;
    AVCTP(connState)[4] = AvctpConnDiscIncoming;
    AVCTP(connState)[5] = AvctpConnConnected;

    AVCTP(browseConnState)[0] = AvctpConnDisconnected;
    AVCTP(browseConnState)[1] = AvctpConnConnPending;
    AVCTP(browseConnState)[2] = AvctpConnConnIncoming;
    AVCTP(browseConnState)[3] = AvctpConnDiscPending;
    AVCTP(browseConnState)[4] = AvctpConnDiscIncoming;
    AVCTP(browseConnState)[5] = AvctpConnConnected;    
}


/*****************************************************************************
 * FUNCTION
 *  AvctpConnFindChannel
 * DESCRIPTION
 *  Find the AV channel matching the remote device.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
AvctpChannel *AvctpConnFindChannel(BtRemoteDevice *remDev, const U16 psmValue)
{
    AvctpChannel *chnl;
    
    // AV13 choose by the bd address and channel type

    /* Find the AV channel associated with this device */
    chnl = (AvctpChannel*) GetHeadList(&AVCTP(chnlList));
    while (chnl != (AvctpChannel*) & AVCTP(chnlList))
    {
        if (chnl->conn.remDev == remDev) 
        {
            if( psmValue == chnl->channelType ){
                /* Found it */
                break;
            }
        }

        /* Get the next connection structure */
        chnl = (AvctpChannel*) GetNextNode(&chnl->node);
    }

    if (chnl == (AvctpChannel*) & AVCTP(chnlList))
    {
        /* Channel was not found */
        chnl = 0;
    }

    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_CONN_FIND_CHANNEL_LOG, chnl, psmValue);
    return chnl;
}


/*****************************************************************************
 * FUNCTION
 *  AvctpConnFindFreeChannel
 * DESCRIPTION
 *  Find a free AV channel.
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
AvctpChannel *AvctpConnFindFreeChannel(const U16 psmValue)
{
    AvctpChannel *chnl;

    /* Find the AV channel associated with this device */
    chnl = (AvctpChannel*) GetHeadList(&AVCTP(chnlList));
    while (chnl != (AvctpChannel*) & AVCTP(chnlList))
    {
        if (chnl->conn.state == AVCTP_STATE_DISCONNECTED && chnl->channelType == psmValue ) 
        {
            /* Found it */
            break;
        }

        /* Get the next connection structure */
        chnl = (AvctpChannel*) GetNextNode(&chnl->node);
    }

    if (chnl == (AvctpChannel*) & AVCTP(chnlList))
    {
        /* Channel was not found */
        chnl = 0;
    }
    
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_CONN_FIND_FREECHANNEL_LOG, chnl );
    return chnl;
}


/*****************************************************************************
 * FUNCTION
 *  AvctpL2Callback
 * DESCRIPTION
 *  L2CAP callback routine.
 * PARAMETERS
 *  L2ChannelId     [IN]        
 *  Parms           [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvctpL2Callback(L2capChannelId L2ChannelId, L2capCallbackParms *Parms)
{
    AvctpChannel *chnl;
    AvctpConn *conn = 0;

    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_L2CALLBACK_LOG, Parms->event);
    OS_Report("[AVCTP] chid:%d", L2ChannelId);

    chnl = AvctpConnFindChannel(Parms->aclLink, Parms->owner.psm->psm ); 
    if (chnl)
    {
        /* Found a matching channel */
        if (Parms->event != L2EVENT_CONNECT_IND)
        {
            conn = &chnl->conn;
        }
        else
        {
            kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_L2CALLBACK_REJECT_LOG, Parms->owner.psm->psm );
            OS_Report("[AVCTP] recject l2channelid:%d", L2ChannelId);
            chnl->retryFlag = 1;
            /* Already connected. Fail this one. */
            (void)L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
            return;
        }
    }
    else
    {
        /* No channel found */
        if (Parms->event == L2EVENT_CONNECT_IND)
        {
            kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_L2CALLBACK_NEW_CONNECTE_LOG );

            /* No channel, search for connection */
            chnl = AvctpConnFindFreeChannel(Parms->owner.psm->psm );
            if (chnl)
            {
                conn = &chnl->conn;
                conn->l2ChannelId = L2ChannelId;
                kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_CONN_ASSIGN_CHANNEL_LOG, Parms->owner.psm->psm, L2ChannelId);
            }
            else
            {
                kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_L2CALLBACK_REJECT_LOG, Parms->owner.psm->psm);
                
                /* No channel to connect to */
                (void)L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
                return;
            }
        }
        else
        {
            return;
        }
    }

    /* Call the state machine */
    AVCTP(connState)[conn->state] (conn, Parms);
}


/*****************************************************************************
 * FUNCTION
 *  AvctpFreeConn
 * DESCRIPTION
 *  Clear the connection state and notify the app.
 * PARAMETERS
 *  Conn        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvctpFreeConn(AvctpConn *Conn)
{
    AvctpConnCallbackParms connParms;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_CONN_FREE_CHANNEL_LOG, Conn->l2ChannelId);

    if (Conn->state != AVCTP_STATE_DISCONNECTED)
    {

        /* Clean up the state */
        Conn->state = AVCTP_STATE_DISCONNECTED;
        Conn->l2ChannelId = 0;

        /* Notify the application */
        connParms.event = AVCTP_CONN_EVENT_DISCONNECT;
        connParms.ptrs.remDev = Conn->remDev;
        Conn->remDev = 0;
        Conn->callback(Conn, &connParms);
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvctpInitConn
 * DESCRIPTION
 *  Initialize the connection structure.
 * PARAMETERS
 *  conn            [IN]        
 *  callback        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvctpInitConn(AvctpConn *conn, AvctpConnCallback callback)
{
    /* Initialize the connection structure */
    conn->remDev = 0;
    conn->callback = callback;
    conn->state = AVCTP_STATE_DISCONNECTED;
    conn->l2ChannelId = 0;
}


/*****************************************************************************
 * FUNCTION
 *  AvctpDisconnect
 * DESCRIPTION
 *  Disconnect the L2CAP channel.
 * PARAMETERS
 *  Conn        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AvctpDisconnect(AvctpConn *Conn)
{
    BtStatus status = BT_STATUS_SUCCESS;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_DISCONNECT_REQ_LOG, Conn->l2ChannelId);

    if (Conn->state != AVCTP_STATE_DISCONNECTED)
    {
        status = L2CAP_DisconnectReq(Conn->l2ChannelId);
        if (status == BT_STATUS_PENDING)
        {
            /* Disconnect Started, wait for L2CAP callback */
            Conn->state = AVCTP_STATE_DISC_PENDING;
        }
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvctpConnect
 * DESCRIPTION
 *  Create an L2CAP channel.
 * PARAMETERS
 *  Conn        [IN]        
 *  RemDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AvctpConnect(AvctpConn *Conn, BtRemoteDevice *RemDev, U16 psmValue)
{
    BtStatus status = BT_STATUS_FAILED;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_CONNECT_REQ_LOG, Conn->l2ChannelId);

	OS_Report("[AVRCP] AvctpConnect AvctpConn:0x%x psm:%d", Conn, psmValue);
    /* Establish the L2CAP link */
	if( BT_PSM_AVCTP == psmValue ){
		status = L2CAP_ConnectReq(&AVCTP(psm), psmValue, RemDev, 0, &Conn->l2ChannelId);
	}else if( BT_PSM_AVCTP_BROWSING == psmValue ){
		status = L2CAP_ConnectReq(&AVCTP(browse_psm), psmValue, RemDev, 0, &Conn->l2ChannelId);
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
        if( BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status )
        {
            L2capConnSettings Setting;

            Setting.reasmBuffer = (ListEntry *)&AVCTP(rxPacketsBufferList);
            Setting.receivedBuffer = (ListEntry *)&AVCTP(receivedPacketsList);
            Setting.rxWindowSize = AVCTP_MAX_RETRANS_NUM;  
            OS_MemSet((U8 *)AVCTP(txMaxCountPerIFrame), 0, 128); /* Always 128 */
            OS_MemSet((U8 *)AVCTP(txSrejList), 0, sizeof(AVCTP(txSrejList)) ); 
            Setting.txMaxTransmitCount = (U8 *)AVCTP(txMaxCountPerIFrame);
            Setting.txSrejList = (U8 *)AVCTP(txSrejList);         
            Setting.maxSrejNo = RF_L2CAP_MAX_SREJ_SIZE;

            L2CAP_ConnectReqAssignFlowControlBuffer(&Setting, Conn->l2ChannelId);

        }
#endif
	}else{
	    Assert(0);
	}

    if (status == BT_STATUS_PENDING)
    {
        /* Connection Initiated */
        Conn->remDev = RemDev;
        Conn->state = AVCTP_STATE_CONN_PENDING;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvctpConnectRsp
 * DESCRIPTION
 *  Respond to the incoming L2CAP connection.
 * PARAMETERS
 *  Conn        [IN]        
 *  Accept      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AvctpConnectRsp(AvctpConn *Conn, BOOL Accept, U16 psmValue)
{
    BtStatus status;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_CONNECT_RSP_LOG, Accept, Conn->l2ChannelId);
    

    /* Incoming connection, accept it, go to conn incoming state */
    if (Accept)
    {
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED		
        L2capConnSettings Setting;

        Setting.reasmBuffer = (ListEntry *)&AVCTP(rxPacketsBufferList);
        Setting.receivedBuffer = (ListEntry *)&AVCTP(receivedPacketsList);
        Setting.rxWindowSize = AVCTP_MAX_RETRANS_NUM;  
        OS_MemSet((U8 *)AVCTP(txMaxCountPerIFrame), 0, 128); /* Always 128 */
        OS_MemSet((U8 *)AVCTP(txSrejList), 0, sizeof(AVCTP(txSrejList)) ); 
        Setting.txMaxTransmitCount = (U8 *)AVCTP(txMaxCountPerIFrame);
        Setting.txSrejList = (U8 *)AVCTP(txSrejList);         
        Setting.maxSrejNo = RF_L2CAP_MAX_SREJ_SIZE;

        //L2CAP_ConnectReqAssignFlowControlBuffer(&Setting, Conn->l2ChannelId);
		
        if( BT_PSM_AVCTP_BROWSING == psmValue ){
            status = L2CAP_ConnectRsp(Conn->l2ChannelId, L2CONN_ACCEPTED, &Setting);
        }else{
            status = L2CAP_ConnectRsp(Conn->l2ChannelId, L2CONN_ACCEPTED, NULL);
        }
#else
            status = L2CAP_ConnectRsp(Conn->l2ChannelId, L2CONN_ACCEPTED, NULL);
#endif
    }
    else
    {
        status = L2CAP_ConnectRsp(Conn->l2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
    }

    if (status == BT_STATUS_PENDING)
    {
        Conn->state = AVCTP_STATE_CONN_INCOMING;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvctpIsConnected
 * DESCRIPTION
 *  Determine if AVCTP is connected.
 * PARAMETERS
 *  Conn        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL AvctpIsConnected(AvctpConn *Conn)
{
    if (Conn->state == AVCTP_STATE_DISCONNECTED)
    {
        return FALSE;
    }

    return TRUE;
}

