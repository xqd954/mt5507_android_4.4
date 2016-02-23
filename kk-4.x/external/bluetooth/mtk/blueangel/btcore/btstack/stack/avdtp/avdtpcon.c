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
 *     $Workfile:avdtpcon.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains the L2CAP/ACL connection state machine 
 *              for AVDTP connections.
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
#include "bt_common.h"
#include "l2cap.h"
#include "avdtp.h"
#include "sys/avdtpcon.h"
#include "sys/avalloc.h"
#include "bt_feature.h"
#ifdef __BT_SINGLE_ANTENNA__
#include "bt_wmt_api.h"

#endif        


/* Function Prototypes */
static void AvdtpFreeConn(AvdtpConn *Conn, BtStatus Status);


/*****************************************************************************
 * FUNCTION
 *  AvdtpConnDisconnected
 * DESCRIPTION
 *  Disconnected state.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpConnDisconnected(AvdtpConn *Conn, L2capCallbackParms *Parms)
{
    AvdtpConnCallbackParms connParms;

    switch (Parms->event)
    {
        case L2EVENT_CONNECT_IND:

            /* Notify the application */
            Conn->remDev = Parms->aclLink;
            connParms.event = AVDTP_CONN_EVENT_CONNECT_IND;
            connParms.ptrs.remDev = Conn->remDev;
            Conn->callback(Conn, &connParms);
            break;
        case L2EVENT_DISCONNECTED:
            /* Conn disconnected, notify the app */
            AvdtpFreeConn(Conn, Parms->status);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpConnConnPending
 * DESCRIPTION
 *  Conn Pending state.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpConnConnPending(AvdtpConn *Conn, L2capCallbackParms *Parms)
{
    AvdtpConnCallbackParms connParms;

    connParms.status = Parms->status;

    switch (Parms->event)
    {
        case L2EVENT_CONNECTED:
            /* Connection is up */
            Conn->state = AVDTP_STATE_CONNECTED;
            Conn->remDev = Parms->aclLink;

            /* Notify the application */
            connParms.event = AVDTP_CONN_EVENT_CONNECT;
            connParms.ptrs.remDev = Conn->remDev;
            Conn->callback(Conn, &connParms);
            break;
        case L2EVENT_DISCONNECTED:
            /* Conn disconnected, notify the app */
            AvdtpFreeConn(Conn, Parms->status);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpConnConnIncoming
 * DESCRIPTION
 *  Connect Incoming state.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpConnConnIncoming(AvdtpConn *Conn, L2capCallbackParms *Parms)
{
    AvdtpConnCallbackParms connParms;

    connParms.status = Parms->status;

    switch (Parms->event)
    {
        case L2EVENT_CONNECTED:
            /* Connection is up */
            Conn->state = AVDTP_STATE_CONNECTED;
            Conn->remDev = Parms->aclLink;

            /* Notify the application */
            connParms.event = AVDTP_CONN_EVENT_CONNECT;
            connParms.ptrs.remDev = Conn->remDev;
            Conn->callback(Conn, &connParms);
            break;
        case L2EVENT_DISCONNECTED:
            /* Conn disconnected, notify the app */
            AvdtpFreeConn(Conn, Parms->status);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpConnDiscPending
 * DESCRIPTION
 *  Disconnect Pending state.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpConnDiscPending(AvdtpConn *Conn, L2capCallbackParms *Parms)
{
    AvdtpConnCallbackParms connParms;

    connParms.status = Parms->status;

    switch (Parms->event)
    {
        case L2EVENT_DISCONNECTED:
            /* Conn disconnected, notify the app */
            AvdtpFreeConn(Conn, Parms->status);
            break;
        case L2EVENT_PACKET_HANDLED:
            /* Notify the application */
            connParms.event = AVDTP_CONN_EVENT_DATA_SENT;
            connParms.ptrs.packet = Parms->ptrs.packet;
            Conn->callback(Conn, &connParms);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpConnDiscIncoming
 * DESCRIPTION
 *  Disconnected state.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpConnDiscIncoming(AvdtpConn *Conn, L2capCallbackParms *Parms)
{
    AvdtpConnCallbackParms connParms;

    connParms.status = Parms->status;

    switch (Parms->event)
    {
        case L2EVENT_DISCONNECTED:
            /* Conn disconnected, notify the app */
            AvdtpFreeConn(Conn, Parms->status);
            break;
        case L2EVENT_PACKET_HANDLED:
            /* Notify the application */
            connParms.event = AVDTP_CONN_EVENT_DATA_SENT;
            connParms.ptrs.packet = Parms->ptrs.packet;
            Conn->callback(Conn, &connParms);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpConnConnected
 * DESCRIPTION
 *  Connected state.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpConnConnected(AvdtpConn *Conn, L2capCallbackParms *Parms)
{
    AvdtpConnCallbackParms connParms;

    connParms.status = Parms->status;

    switch (Parms->event)
    {
        case L2EVENT_DISCONNECTED:
            /* Conn disconnected, notify the app */
            AvdtpFreeConn(Conn, Parms->status);
            break;
        case L2EVENT_PACKET_HANDLED:
            /* Notify the application */
            connParms.event = AVDTP_CONN_EVENT_DATA_SENT;
            connParms.ptrs.packet = Parms->ptrs.packet;
            Conn->callback(Conn, &connParms);
            break;
        case L2EVENT_DATA_IND:
            connParms.event = AVDTP_CONN_EVENT_DATA_IND;
            connParms.dataLen = Parms->dataLen;
            connParms.ptrs.data = Parms->ptrs.data;
            Conn->callback(Conn, &connParms);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpConnInitStateMachine
 * DESCRIPTION
 *  Initializes the state table.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void AvdtpConnInitStateMachine(void)
{
    AVDTP(connState)[0] = AvdtpConnDisconnected;
    AVDTP(connState)[1] = AvdtpConnConnPending;
    AVDTP(connState)[2] = AvdtpConnConnIncoming;
    AVDTP(connState)[3] = AvdtpConnDiscPending;
    AVDTP(connState)[4] = AvdtpConnDiscIncoming;
    AVDTP(connState)[5] = AvdtpConnConnected;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpConnFindChannel
 * DESCRIPTION
 *  Find the AV channel matching the remote device.
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
AvdtpChannel *AvdtpConnFindChannel(BtRemoteDevice *remDev)
{
    AvdtpChannel *chnl;

    /* Find the AV channel associated with this device */
    chnl = (AvdtpChannel*) GetHeadList(&AVDTP(chnlList));
    while (chnl != (AvdtpChannel*) & AVDTP(chnlList))
    {
        if ((chnl->conn.remDev != 0) && (chnl->conn.remDev == remDev))
        {
            /* Found it */
            break;
        }

        /* Get the next connection structure */
        chnl = (AvdtpChannel*) GetNextNode(&chnl->node);
    }

    if (chnl == (AvdtpChannel*) & AVDTP(chnlList))
    {
        /* Channel was not found */
        chnl = 0;
    }

    return chnl;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpConnFindFreeChannel
 * DESCRIPTION
 *  Find a free AV channel.
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
AvdtpChannel *AvdtpConnFindFreeChannel(void)
{
    AvdtpChannel *chnl;

    /* Find the AV channel associated with this device */
    chnl = (AvdtpChannel*) GetHeadList(&AVDTP(chnlList));
    while (chnl != (AvdtpChannel*) & AVDTP(chnlList))
    {
        if (chnl->conn.state == AVDTP_STATE_DISCONNECTED)
        {
            if (chnl->currentTxStream != 0)
            {
                bt_trace(TRACE_GROUP_1, CURRENT_TX_STREAM_STATEBT_DEBUGx02X, chnl->currentTxStream->state);
                if (chnl->currentTxStream->state == AVDTP_STRM_STATE_IDLE)
                {
                    break;
                }
                else
                {
                    bt_trace(TRACE_GROUP_1, CURRENT_TX_STREAM_IS_NOT_IDLE);
                }
            }
            else
            {
                break;
            }
            /* Found it */
        }

        /* Get the next connection structure */
        chnl = (AvdtpChannel*) GetNextNode(&chnl->node);
    }

    if (chnl == (AvdtpChannel*) & AVDTP(chnlList))
    {
        /* Channel was not found */
        chnl = 0;
    }

    return chnl;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpConnFindConn
 * DESCRIPTION
 *  Find the AV connection matching the L2CAP Channel ID.
 * PARAMETERS
 *  Chnl            [IN]        
 *  L2ChannelId     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
AvdtpConn *AvdtpConnFindConn(AvdtpChannel *Chnl, L2capChannelId L2ChannelId)
{
    AvdtpConn *conn;
    AvdtpStream *strm;

    conn = &Chnl->conn;
    if (Chnl->conn.l2ChannelId != L2ChannelId)
    {
        /* This channel belongs to one of the media transports */
        strm = (AvdtpStream*) GetHeadList(&Chnl->streamList);
        while (strm != (AvdtpStream*) & Chnl->streamList)
        {
            if (strm->conn.l2ChannelId == L2ChannelId)
            {
                //                (strm->state >= AVDTP_STRM_STATE_CONFIGURED)) {
                //                (strm->state >= AVDTP_STRM_STATE_OPEN)) {
                /* Found It */
                conn = &strm->conn;
                break;
            }

            /* Get the next connection structure */
            strm = (AvdtpStream*) GetNextNode(&strm->node);
        }

        if (strm == (AvdtpStream*) & Chnl->streamList)
        {
            conn = 0;
        }
    }

    return conn;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpConnFindOrphanedStreamConn
 * DESCRIPTION
 *  Find the stream connection where no channel connection exists.
 *  This state can happen when the ACL link is terminated.  Since
 *  the first L2CAP disconnect indication we receive is for the
 *  first connection created (the signal channel connection), any
 *  streams associated with that signal channel are orphaned when
 *  the signal channel resources are cleaned up.
 * PARAMETERS
 *  RemDev          [IN]        
 *  L2ChannelId     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
AvdtpConn *AvdtpConnFindOrphanedStreamConn(BtRemoteDevice *RemDev, L2capChannelId L2ChannelId)
{
    AvdtpChannel *chnl;
    AvdtpConn *conn = 0;

    /* Find the AV channel associated with this device */
    chnl = (AvdtpChannel*) GetHeadList(&AVDTP(chnlList));
    while (chnl != (AvdtpChannel*) & AVDTP(chnlList))
    {
        if (!IsListEmpty(&chnl->streamList))
        {
            conn = AvdtpConnFindConn(chnl, L2ChannelId);
            if (conn && (conn->remDev == RemDev))
            {
                break;
            }
        }

        /* Get the next connection structure */
        chnl = (AvdtpChannel*) GetNextNode(&chnl->node);
    }

    if (chnl == (AvdtpChannel*) & AVDTP(chnlList))
    {
        /* Channel was not found */
        conn = 0;
    }

    return conn;
}


/*****************************************************************************
 * FUNCTION
 *  AvL2Callback
 * DESCRIPTION
 *  L2CAP callback routine.
 * PARAMETERS
 *  L2ChannelId     [IN]        
 *  Parms           [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvL2Callback(L2capChannelId L2ChannelId, L2capCallbackParms *Parms)
{
    AvdtpChannel *chnl;
    AvdtpConn *conn = 0;

    chnl = AvdtpConnFindChannel(Parms->aclLink);
    if (chnl)
    {
        /* Found a matching channel */
        if (Parms->event != L2EVENT_CONNECT_IND)
        {
            /* Connection exists */
            conn = AvdtpConnFindConn(chnl, L2ChannelId);
            if (!conn)
            {
                /* Ignore this event, unknown channel */
                return;
            }
        }
        else
        {
            /* New incoming stream */
            conn = AvdtpConnFindConn(chnl, 0);
            if (!conn)
            {
                /* No stream to connect to */
                (void)L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
                return;
            }
            conn->l2ChannelId = L2ChannelId;
        }
    }
    else
    {
        /* No channel found */
        if (Parms->event == L2EVENT_CONNECT_IND)
        {
            /* No channel, search for connection */
            chnl = AvdtpConnFindFreeChannel();
            if (chnl)
            {
                conn = &chnl->conn;
                conn->l2ChannelId = L2ChannelId;
            }
            else
            {
                /* No channel to connect to */
                (void)L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
                return;
            }
        }
        else
        {
            /* No channel, search all allocated streams */
            conn = AvdtpConnFindOrphanedStreamConn(Parms->aclLink, L2ChannelId);
            if (!conn)
            {
                /* No channels for this event */
                return;
            }
        }
    }
    /* Call the state machine */
    L2Cap_EnableHeavyRTTraffic();
    #ifdef __BT_SINGLE_ANTENNA__
    if ((Parms->event == L2EVENT_DISCON_IND) || (Parms->event == L2EVENT_DISCONNECTED))
    {
        BT_WMT_A2DP_Send_Status(conn->remDev, 3);
    }
    #endif
    AVDTP(connState)[conn->state] (conn, Parms);
    if ((Parms->event == L2EVENT_DISCON_IND) || (Parms->event == L2EVENT_DISCONNECTED))
    {
        //#ifdef __BT_SINGLE_ANTENNA__
        //BT_WMT_A2DP_Send_Status(conn->remDev, 3);
        //#endif        
        L2Cap_DisableHeavyRTTraffic();
    }

}


/*****************************************************************************
 * FUNCTION
 *  AvdtpFreeConn
 * DESCRIPTION
 *  Clear the connection state and notify the app.
 * PARAMETERS
 *  Conn        [IN]        
 *  Status      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpFreeConn(AvdtpConn *Conn, BtStatus Status)
{
    AvdtpConnCallbackParms connParms;

    connParms.status = Status;

    if (Conn->state != AVDTP_STATE_DISCONNECTED)
    {

        /* Clean up the state */
        Conn->state = AVDTP_STATE_DISCONNECTED;
        Conn->l2ChannelId = 0;

        /* Notify the application */
        connParms.event = AVDTP_CONN_EVENT_DISCONNECT;
        connParms.ptrs.remDev = Conn->remDev;
        Conn->remDev = 0;
        Conn->callback(Conn, &connParms);
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpInitConn
 * DESCRIPTION
 *  Initialize the connection structure.
 * PARAMETERS
 *  Conn            [IN]        
 *  Callback        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvdtpInitConn(AvdtpConn *Conn, AvdtpConnCallback Callback)
{
    /* Initialize the connection structure */
    Conn->remDev = 0;
    Conn->callback = Callback;
    Conn->state = AVDTP_STATE_DISCONNECTED;
    Conn->l2ChannelId = 0;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpDisconnect
 * DESCRIPTION
 *  Disconnect the L2CAP channel.
 * PARAMETERS
 *  Conn        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AvdtpDisconnect(AvdtpConn *Conn)
{
    BtStatus status = BT_STATUS_SUCCESS;

    if (Conn->state != AVDTP_STATE_DISCONNECTED)
    {
        status = L2CAP_DisconnectReq(Conn->l2ChannelId);
        if (status == BT_STATUS_PENDING)
        {
            /* Disconnect Started, wait for L2CAP callback */
            Conn->state = AVDTP_STATE_DISC_PENDING;
        }
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpConnect
 * DESCRIPTION
 *  Create an L2CAP channel.
 * PARAMETERS
 *  Conn        [IN]        
 *  RemDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AvdtpConnect(AvdtpConn *Conn, BtRemoteDevice *RemDev)
{
    BtStatus status;

    /* Establish the L2CAP link */
    status = L2CAP_ConnectReq(&AVDTP(psm), BT_PSM_AVDTP, RemDev, 0, &Conn->l2ChannelId);

    if (status == BT_STATUS_PENDING)
    {
        /* Connection Initiated */
        Conn->state = AVDTP_STATE_CONN_PENDING;
        Conn->remDev = RemDev;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpConnectRsp
 * DESCRIPTION
 *  Respond to the incoming L2CAP connection.
 * PARAMETERS
 *  Conn        [IN]        
 *  Accept      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AvdtpConnectRsp(AvdtpConn *Conn, BOOL Accept)
{
    BtStatus status;

    /* Incoming connection, accept it, go to conn incoming state */
    if (Accept)
    {
        status = L2CAP_ConnectRsp(Conn->l2ChannelId, L2CONN_ACCEPTED, 0);
    }
    else
    {
        status = L2CAP_ConnectRsp(Conn->l2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
    }

    if (status == BT_STATUS_PENDING)
    {
        Conn->state = AVDTP_STATE_CONN_INCOMING;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpIsConnected
 * DESCRIPTION
 *  Determine if AVDTP is connected.
 * PARAMETERS
 *  Conn        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL AvdtpIsConnected(AvdtpConn *Conn)
{
    if (Conn->state == AVDTP_STATE_DISCONNECTED)
    {
        return FALSE;
    }

    return TRUE;
}

