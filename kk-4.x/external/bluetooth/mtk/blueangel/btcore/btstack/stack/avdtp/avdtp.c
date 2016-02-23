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
 *     $Workfile:avdtp.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains public API for the Audio/Video
 *     Distribution Transport Protocol (AVDTP).
 *             
 * Created:     Mar 10, 2004
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
#include "btconfig.h"
#include "sec.h"
#include "sys/avalloc.h"
#include "l2cap.h"
#include "sys/avsigmgr.h"
#include "sys/avdtpcon.h"
#include "bt_feature.h"
#ifdef __BT_SINGLE_ANTENNA__
#include "bt_wmt_api.h"
#endif



/****************************************************************************
 *
 * external Functions
 *
 ****************************************************************************/
extern void bt_a2dp_stream_sent_data_free(U8 *buffer, U16 len);
extern U8 L2Cap_FlushOnePktTransmits(L2capChannelId channel_id);


/*****************************************************************************
 * FUNCTION
 *  AvdtpFindContentProtection
 * DESCRIPTION
 *  Finds a registered content protection structure based on the type
 * PARAMETERS
 *  Stream      [IN]        
 *  type        [IN]        
 * RETURNS
 *  Pointer to a registered content protection structure.
 *****************************************************************************/
AvdtpContentProt *AvdtpFindContentProtection(AvdtpStream *Stream, AvdtpContentProtType type)
{
    AvdtpContentProt *cp;

    cp = (AvdtpContentProt*) GetHeadList(&Stream->cpList);
    while (cp != (AvdtpContentProt*) & Stream->cpList)
    {
        if (cp->cpType == type)
        {
            break;
        }

        /* Get the next structure */
        cp = (AvdtpContentProt*) GetNextNode(&cp->node);
    }

    if (cp == (AvdtpContentProt*) & Stream->cpList)
    {
        cp = 0;
    }

    return cp;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_Init
 * DESCRIPTION
 *  Initialize the AVDTP SDK.  This function should only be called
 *  once, normally at initialization time.  The calling of this
 *  function can be specified in overide.h using the XA_LOAD_LIST
 *  macro (i.e. #define XA_LOAD_LIST XA_MODULE(AV)).
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BOOL AVDTP_Init(void)
{
    I8 i;

    BOOL status = TRUE;

    /* Initialize Memory */
    status = AvdtpAlloc();
    /* Initialize the connection state machine */
    AvdtpConnInitStateMachine();

    /* Register AVDTP with L2CAP */
    AVDTP(psm).callback = AvL2Callback;
    AVDTP(psm).psm = BT_PSM_AVDTP;
    AVDTP(psm).localMtu = L2CAP_MAXIMUM_MTU;
    AVDTP(psm).minRemoteMtu = L2CAP_MINIMUM_MTU;
    AVDTP(psm).authorizedSetting = FALSE;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    AVDTP(psm).inLinkMode = AVDTP(psm).outLinkMode = L2MODE_BASIC;
#ifdef __BT_2_1_ERTM__
    AVDTP(psm).lockStepNeeded = 0;
    AVDTP(psm).highSpeedRequired = 0;
#endif
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */

    if (L2CAP_RegisterPsm(&(AVDTP(psm))) != BT_STATUS_SUCCESS)
    {
        status = FALSE;
        goto exit;
    }
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    InitializeListEntry(&(AVDTP(secRecord).node));
    AVDTP(secRecord).id = SEC_L2CAP_ID;
    AVDTP(secRecord).channel = BT_PSM_AVDTP;
    AVDTP(secRecord).level = BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT;
#ifndef __BT_2_1_SIMPLE_PAIRING__
    AVDTP(secRecord).level_type = BT_SEC_LOW;
#else 
    AVDTP(secRecord).level_type = BT_SEC_MED;
#endif 

    SEC_RegisterRecord(&AVDTP(secRecord));
#endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 
    /* Initialized lists */
    InitializeListHead(&AVDTP(chnlList));
    InitializeListHead(&AVDTP(streamList));
    InitializeListHead(&AVDTP(txPacketList));
    for (i = 0; i < AVDTP_NUM_TX_PACKETS; i++)
    {
        AVDTP(txPackets)[i].txData = AVDTP(txData)[i];
        InsertTailList(&AVDTP(txPacketList), &AVDTP(txPackets)[i].node);
    }

  exit:

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_Register
 * DESCRIPTION
 *  Registers an application callback to create/receive AVDTP
 *  connections.  This function must be called before any other
 *  AVDTP functions.
 * PARAMETERS
 *  Chnl            [IN]        
 *  Callback        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_Register(AvdtpChannel *Chnl, AvdtpCallback Callback)
{
    BtStatus status = BT_STATUS_SUCCESS;
    U8 i = 0;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Chnl != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Callback != 0);

    OS_LockStack();

    /* Register this connection in the list */
    if (!IsNodeOnList(&AVDTP(chnlList), &Chnl->node))
    {

        /* Clear the connection structure */
        OS_MemSet((U8*) Chnl, 0, sizeof(AvdtpChannel));
        InitializeListHead(&Chnl->rtp_pkt_List);
        /* Register the Avdtp connection */
        Chnl->callback = Callback;

        /* Initialize the Signal Manager handler */
        AvdtpSigInit(Chnl);
        Chnl->rxAbortCmd = AVDTP_NOT_RX_ABORT_CMD;
        /* Insert the channel on the list */
        InsertTailList(&AVDTP(chnlList), &Chnl->node);
        for (i = 0; i < MAX_AVDTP_PKT_NO; i++)
        {
            InsertTailList(&Chnl->rtp_pkt_List, &Chnl->rtp_packet[i].node);
        }
        /* Initialize the channel's stream list */
        InitializeListHead(&Chnl->streamList);

    }
    else
    {

        /* Already registered */
        status = BT_STATUS_IN_USE;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_Deregister
 * DESCRIPTION
 *  De-registers the AVDTP callback. After making this call
 *  successfully, the callback specified in AVDTP_Register will
 *  receive no further events.
 * PARAMETERS
 *  Chnl        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_Deregister(AvdtpChannel *Chnl)
{
    BtStatus status = BT_STATUS_SUCCESS;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Chnl != 0);

    OS_LockStack();

    if (IsNodeOnList(&AVDTP(chnlList), &Chnl->node))
    {
        if (!AvdtpSigIsConnected(Chnl))
        {
            /* No connection is up, remove from list */
            RemoveEntryList(&Chnl->node);
        }
        else
        {
            /* A connection is up */
            status = BT_STATUS_BUSY;
        }
    }
    else
    {
        /* Connection is not registered */
        status = BT_STATUS_NOT_FOUND;
    }
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    SEC_Unregister(&AVDTP(secRecord));
#endif 

    InitializeListHead(&Chnl->rtp_pkt_List);
    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_Connect
 * DESCRIPTION
 *  Connect to the remote device.
 * PARAMETERS
 *  Chnl        [IN]        
 *  RemDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_Connect(AvdtpChannel *Chnl, BtRemoteDevice *RemDev)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Chnl != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, RemDev != 0);

    OS_LockStack();

    /* See if the Chnl is registered */
    if (IsNodeOnList(&AVDTP(chnlList), &Chnl->node))
    {
        if (!AvdtpSigIsConnected(Chnl))
        {
            /* Start the connection */
            status = AvdtpSigConnect(Chnl, RemDev);
        }
        else
        {
            /* Already connected */
            status = BT_STATUS_BUSY;
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_ConnectRsp
 * DESCRIPTION
 *  Respond to a connect request from the remote device.
 * PARAMETERS
 *  Chnl        [IN]        
 *  Accept      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_ConnectRsp(AvdtpChannel *Chnl, BOOL Accept)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Chnl != 0);

    OS_LockStack();

    /* See if the Chnl is registered */
    if (IsNodeOnList(&AVDTP(chnlList), &Chnl->node))
    {
        if (!AvdtpSigIsConnected(Chnl))
        {
            /* Start the connection */
            status = AvdtpSigConnectRsp(Chnl, Accept);
        }
        else
        {
            /* Already connected */
            status = BT_STATUS_BUSY;
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_Disconnect
 * DESCRIPTION
 *  Disconnect from the remote device.
 * PARAMETERS
 *  Chnl        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_Disconnect(AvdtpChannel *Chnl)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Chnl != 0);

    OS_LockStack();

    /* See if the Chnl is registered */
    if (IsNodeOnList(&AVDTP(chnlList), &Chnl->node))
    {
        if (AvdtpSigIsConnected(Chnl))
        {
            /* Disconnect */
            status = AvdtpSigDisconnect(Chnl);
        }
        else
        {
            /* No connection exists */
            status = BT_STATUS_NO_CONNECTION;
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_RegisterStream
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 *  Codec       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_RegisterStream(AvdtpStream *Stream, AvdtpCodec *Codec)
{
    BtStatus status = BT_STATUS_SUCCESS;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Codec != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (Stream->mediaType & ~0xF) == 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, (Stream->endPointType & ~0x1) == 0);

    OS_LockStack();

    /* Register this stream in the list */
    if (!IsNodeOnList(&AVDTP(streamList), &Stream->node))
    {

        if (AVDTP(numStreams) < MAX_NUM_STREAMS)
        {
            /* Initialize the stream structure */
            Stream->state = 0;
            Stream->chnl = 0;
            OS_MemSet((U8*) & Stream->cpCfg, 0, sizeof(AvdtpContentProt));
            OS_MemSet((U8*) & Stream->codecCfg, 0, sizeof(AvdtpCodec));
            Stream->cpCfg.data = Stream->cpCfgValue;
            Stream->codecCfg.elements = Stream->codecCfgElem;
            OS_MemSet((U8*) & Stream->cfgReq.cp, 0, sizeof(AvdtpContentProt));
            OS_MemSet((U8*) & Stream->cfgReq.codec, 0, sizeof(AvdtpCodec));
            Stream->cfgReq.cp.data = Stream->cpReqValue;
            Stream->cfgReq.codec.elements = Stream->codecReqElem;
            Stream->codec = Codec;
            Stream->abError = AVDTP_ERR_NO_ERROR;

            /* Initialize content protection list */
            InitializeListHead(&Stream->cpList);

            /* Register the Avdtp stream connection */
            AvdtpInitConn(&Stream->conn, AvdtpStrmConnCallback);
            Stream->locStrmId = 0xFF;
            /* Insert on the free list */
            InsertTailList(&AVDTP(streamList), &Stream->node);

            /* Count the registered streams */
            AVDTP(numStreams)++;
        }
        else
        {
            status = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        status = BT_STATUS_IN_USE;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_DeregisterStream
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_DeregisterStream(AvdtpStream *Stream)
{
    BtStatus status = BT_STATUS_SUCCESS;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);

    OS_LockStack();

    if (IsNodeOnList(&AVDTP(streamList), &Stream->node))
    {
        if (Stream->state == AVDTP_STRM_STATE_IDLE)
        {
            InitializeListHead(&Stream->cpList);
            RemoveEntryList(&Stream->node);
            AVDTP(numStreams)--;
        }
        else
        {
            /* The stream is in use */
            status = BT_STATUS_BUSY;
        }
    }
    else
    {
        /* Stream is not registered */
        status = BT_STATUS_NOT_FOUND;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_AddContentProtection
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 *  Cp          [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_AddContentProtection(AvdtpStream *Stream, AvdtpContentProt *Cp)
{
    BtStatus status = BT_STATUS_SUCCESS;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Cp != 0);

    OS_LockStack();

    if (IsNodeOnList(&AVDTP(streamList), &Stream->node))
    {
        if (Stream->state == AVDTP_STRM_STATE_IDLE)
        {
            /* Add content protection to the registration list */
            if (!IsNodeOnList(&Stream->cpList, &Cp->node))
            {
                InsertTailList(&Stream->cpList, &Cp->node);
            }
            else
            {
                status = BT_STATUS_IN_USE;
            }
        }
        else
        {
            /* The stream is in use */
            status = BT_STATUS_BUSY;
        }
    }
    else
    {
        /* Stream is not registered */
        status = BT_STATUS_NOT_FOUND;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_DiscoverStreams
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Chnl        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_DiscoverStreams(AvdtpChannel *Chnl)
{
    BtStatus status;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_DISCOVERY_REQ);

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Chnl != 0);
    OS_LockStack();
    if (IsNodeOnList(&AVDTP(chnlList), &Chnl->node))
    {
        if (AvdtpSigIsConnected(Chnl))
        {
            /* Do a search for a stream with matching capabilities */
            status = AvdtpSigStreamDiscover(Chnl);
        }
        else
        {
            status = BT_STATUS_NO_CONNECTION;
        }
    }
    else
    {
        /* No connection exists */
        status = BT_STATUS_NOT_FOUND;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_GetCapabilities
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Chnl        [IN]        
 *  StrmId      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_GetCapabilities(AvdtpChannel *Chnl, AvdtpStreamId StrmId)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Chnl != 0);

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_GET_CAP_REQ);

    OS_LockStack();
    if (IsNodeOnList(&AVDTP(chnlList), &Chnl->node))
    {
        if (AvdtpSigIsConnected(Chnl))
        {
            /* Do a search for a stream with matching capabilities */
            status = AvdtpSigStreamGetCapabilities(Chnl, StrmId);
        }
        else
        {
            status = BT_STATUS_NO_CONNECTION;
        }
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_GetConfig
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_GetConfig(AvdtpStream *Stream)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);

    OS_LockStack();

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_GET_CONFIG_REQ);
    if (Stream->chnl)
    {
        if (IsNodeOnList(&AVDTP(chnlList), &Stream->chnl->node))
        {
            if (IsNodeOnList(&Stream->chnl->streamList, &Stream->node))
            {
                status = AvdtpSigStreamGetConfig(Stream->chnl, Stream);
            }
            else
            {
                status = BT_STATUS_NOT_FOUND;
            }
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_NO_CONNECTION;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_SetConfig
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Chnl        [IN]        
 *  Stream      [IN]        
 *  StrmId      [IN]        
 *  Codec       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_SetConfig(AvdtpChannel *Chnl, AvdtpStream *Stream, AvdtpStreamId StrmId, AvdtpCodec *Codec)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Chnl != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Codec != 0);

    OS_LockStack();
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SET_CONFIG_REQ);
    if (IsNodeOnList(&AVDTP(chnlList), &Chnl->node))
    {
        if (IsNodeOnList(&AVDTP(streamList), &Stream->node))
        {
            if (AvdtpSigIsConnected(Chnl))
            {
                /* Set the configuration */
                if (Stream->state == AVDTP_STRM_STATE_IDLE)
                {
                    Stream->remStrmId = StrmId;
                    status = AvdtpSigStreamSetConfig(Chnl, Stream, Codec);
                }
                else
                {
                    status = BT_STATUS_IN_USE;
                }
            }
            else
            {
                status = BT_STATUS_NO_CONNECTION;
            }
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }
    OS_UnlockStack();

    return status;

}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_OpenStream
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Chnl        [IN]        
 *  Stream      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_OpenStream(AvdtpChannel *Chnl, AvdtpStream *Stream)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Chnl != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);

    OS_LockStack();
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_OPEN_REQ);
    if (IsNodeOnList(&AVDTP(chnlList), &Chnl->node))
    {
        if (IsNodeOnList(&Chnl->streamList, &Stream->node))
        {
            if (AvdtpSigIsConnected(Chnl))
            {
                /* Set the configuration */
                if (Stream->state == AVDTP_STRM_STATE_CONFIGURED)
                {
                    status = AvdtpSigStreamOpen(Chnl, Stream);
                }
                else
                {
                    status = BT_STATUS_IN_USE;
                }
            }
            else
            {
                status = BT_STATUS_NO_CONNECTION;
            }
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_NOT_FOUND;
    }
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_CloseStream
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_CloseStream(AvdtpStream *Stream)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);

    OS_LockStack();
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_CLOSE_REQ);
    if (Stream->chnl)
    {
        if (IsNodeOnList(&AVDTP(chnlList), &Stream->chnl->node))
        {
            if (IsNodeOnList(&Stream->chnl->streamList, &Stream->node))
            {
                if ((Stream->state == AVDTP_STRM_STATE_OPEN) || (Stream->state == AVDTP_STRM_STATE_STREAMING))
                {
#ifdef __BT_SINGLE_ANTENNA__
                    BT_WMT_A2DP_Send_Status(Stream->chnl->conn.remDev, 0);
#endif
                    status = AvdtpSigStreamClose(Stream->chnl, Stream);
                    if (status == BT_STATUS_PENDING)
                    {
                        Stream->state = AVDTP_STRM_STATE_CLOSING;
                    }
                }
                else
                {
                    status = BT_STATUS_FAILED;
                }
            }
            else
            {
                status = BT_STATUS_NOT_FOUND;
            }
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_NO_CONNECTION;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_Reconfig
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 *  Codec       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_Reconfig(AvdtpStream *Stream, AvdtpCodec *Codec)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Codec != 0);

    OS_LockStack();
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_RECONFIG_REQ);
    if (Stream->chnl)
    {
        if (IsNodeOnList(&AVDTP(chnlList), &Stream->chnl->node))
        {
            if (IsNodeOnList(&Stream->chnl->streamList, &Stream->node))
            {
                if (Stream->state == AVDTP_STRM_STATE_OPEN)
                {
                    status = AvdtpSigStreamReconfig(Stream->chnl, Stream, Codec);
                }
                else
                {
                    status = BT_STATUS_FAILED;
                }
            }
            else
            {
                status = BT_STATUS_NOT_FOUND;
            }
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_NO_CONNECTION;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_StartStream
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_StartStream(AvdtpStream *Stream)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);

    OS_LockStack();
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_START_REQ);
    if (Stream->chnl)
    {
        if (IsNodeOnList(&AVDTP(chnlList), &Stream->chnl->node))
        {
            if (IsNodeOnList(&Stream->chnl->streamList, &Stream->node))
            {
                if (Stream->state == AVDTP_STRM_STATE_OPEN)
                {
#ifdef __BT_SINGLE_ANTENNA__
                    BT_WMT_A2DP_Send_Status(Stream->chnl->conn.remDev, 1);
#endif
                    status = AvdtpSigStreamStart(Stream->chnl, Stream);
                }
                else
                {
                    status = BT_STATUS_FAILED;
                }
            }
            else
            {
                status = BT_STATUS_NOT_FOUND;
            }
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_NO_CONNECTION;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_SuspendStream
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_SuspendStream(AvdtpStream *Stream)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);

    OS_LockStack();
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SUSPEND_REQ);
    if (Stream->chnl)
    {
        if (IsNodeOnList(&AVDTP(chnlList), &Stream->chnl->node))
        {
            if (IsNodeOnList(&Stream->chnl->streamList, &Stream->node))
            {
                if (Stream->state == AVDTP_STRM_STATE_STREAMING)
                {
#ifdef __BT_SINGLE_ANTENNA__
                    BT_WMT_A2DP_Send_Status(Stream->chnl->conn.remDev, 0);
#endif
                    status = AvdtpSigStreamSuspend(Stream->chnl, Stream);
                }
                else
                {
                    status = BT_STATUS_FAILED;
                }
            }
            else
            {
                status = BT_STATUS_NOT_FOUND;
            }
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_NO_CONNECTION;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_AbortStream
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_AbortStream(AvdtpStream *Stream)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);

    OS_LockStack();
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_ABORT_REQ);
    if (Stream->chnl)
    {
        if (IsNodeOnList(&AVDTP(chnlList), &Stream->chnl->node))
        {
            if (IsNodeOnList(&Stream->chnl->streamList, &Stream->node))
            {
                if (Stream->state != AVDTP_STRM_STATE_IDLE)
                {
#ifdef __BT_SINGLE_ANTENNA__
                    BT_WMT_A2DP_Send_Status(Stream->chnl->conn.remDev, 0);
#endif
                    status = AvdtpSigStreamAbort(Stream->chnl, Stream);
                }
                else
                {
                    status = BT_STATUS_FAILED;
                }
            }
            else
            {
                status = BT_STATUS_NOT_FOUND;
            }
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_NO_CONNECTION;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_DiscoverStreamsRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rsptype             [IN]        
 *  Chnl                [IN]        
 *  stream_info_num     [IN]        
 *  stream_info         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_DiscoverStreamsRsp(U8 rsptype, AvdtpChannel *Chnl, U8 stream_info_num, AvdtpStreamInfo *stream_info)
{
    U8 i = 0;
    U8 stream_number = 0;
    U8 msgType = AVTP_MSG_TYPE_REJECT;
    BtStatus status;
    AvtpPacket *rspPacket;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_DISCOVERY_RSP);
    if (IsListEmpty(&AVDTP(txPacketList)))
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_NO_RESOURCE);
        return BT_STATUS_NO_RESOURCES;
    }
    else
    {
        rspPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
        Chnl->rspSigPacket = rspPacket;
    }
    rspPacket->txId = AVDTP_SIG_DISCOVER;
    rspPacket->txIdSize = 1;
    rspPacket->txDataLen = 0;
    rspPacket->useMsgHdr = FALSE;
    rspPacket->internal_used = 0;
    if (rsptype == AVDTP_RSP_REJECT)
    {
        msgType = AVTP_MSG_TYPE_REJECT;
        rspPacket->txDataLen = 0;
    }
    else
    {
        for (stream_number = 0; stream_number < stream_info_num; stream_number++)
        {
            rspPacket->txData[i] = stream_info[stream_number].id << 2;
            rspPacket->txData[i] |= stream_info[stream_number].inUse << 1;
            rspPacket->txData[i + 1] = stream_info[stream_number].mediaType << 4;
            rspPacket->txData[i + 1] |= stream_info[stream_number].streamType << 3;
            rspPacket->txDataLen += 2;
            i += 2;
        }
        msgType = AVTP_MSG_TYPE_ACCEPT;
    }

    if (msgType == AVTP_MSG_TYPE_REJECT)
    {
        /* No streams available; reject with anonymous error code. */
        rspPacket->txData[0] = 0xC0;
        rspPacket->txDataLen = 1;
    }
    rspPacket->msgType = msgType;
    status = AVTP_SendStart(&Chnl->sigChnl, rspPacket);
    if (status != BT_STATUS_PENDING)
    {
        InsertTailList(&AVDTP(txPacketList), &rspPacket->node);
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_GetCapabilitiesRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rsptype     [IN]        
 *  Chnl        [IN]        
 *  Strm        [IN]        
 *  Error       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_GetCapabilitiesRsp(U8 rsptype, AvdtpChannel *Chnl, AvdtpStream *Strm, AvdtpError Error)
{
    AvtpPacket *rspPacket;
    BtStatus status;
    U8 msgType = AVTP_MSG_TYPE_REJECT;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_GET_CAP_RSP);
    if (IsListEmpty(&AVDTP(txPacketList)))
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_NO_RESOURCE);
        return BT_STATUS_NO_RESOURCES;
    }
    else
    {
        rspPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
        Chnl->rspSigPacket = rspPacket;
    }
    rspPacket->txId = AVDTP_SIG_GET_CAPABILITIES;
    rspPacket->txIdSize = 1;
    rspPacket->useMsgHdr = FALSE;
    rspPacket->internal_used = 0;
    if (rsptype == AVDTP_RSP_REJECT)
    {
        msgType = AVTP_MSG_TYPE_REJECT;
        rspPacket->txData[0] = Error;
        rspPacket->txDataLen = 1;
    }
    else
    {
        msgType = AVTP_MSG_TYPE_ACCEPT;
        rspPacket->txDataLen = 0;
        AvdtpSigEncodeCapabilities(rspPacket, Strm, AVDTP_SIG_GET_CAPABILITIES);
    }
    rspPacket->msgType = msgType;
    status = AVTP_SendStart(&Chnl->sigChnl, rspPacket);
    if (status != BT_STATUS_PENDING)
    {
        InsertTailList(&AVDTP(txPacketList), &rspPacket->node);
    }
    if ((rsptype == AVDTP_RSP_REJECT) && (Error > 0xBF))
    {
        status = BT_STATUS_INVALID_PARM;
    }
    return status;

}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_GetCapabilitiesLongRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Chnl        [IN]        
 *  Strm        [IN]        
 *  data        [IN]        
 *  length      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_GetCapabilitiesLongRsp(AvdtpChannel *Chnl, AvdtpStream *Strm, U8 *data, U8 length)
{
    AvtpPacket *rspPacket;
    BtStatus status;
    U8 msgType = AVTP_MSG_TYPE_ACCEPT;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_GET_CAP_RSP);
    if (IsListEmpty(&AVDTP(txPacketList)))
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_NO_RESOURCE);
        return BT_STATUS_NO_RESOURCES;
    }
    else
    {
        rspPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
        Chnl->rspSigPacket = rspPacket;
    }
    rspPacket->txId = AVDTP_SIG_GET_CAPABILITIES;
    rspPacket->txIdSize = 1;
    rspPacket->useMsgHdr = FALSE;
    rspPacket->internal_used = 0;
    msgType = AVTP_MSG_TYPE_ACCEPT;
    rspPacket->txDataLen = length;
    OS_MemCopy((U8*) rspPacket->txData, (U8*) data, length);
    rspPacket->msgType = msgType;
    status = AVTP_SendStart(&Chnl->sigChnl, rspPacket);
    if (status != BT_STATUS_PENDING)
    {
        InsertTailList(&AVDTP(txPacketList), &rspPacket->node);
    }
    return status;

}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_SetConfigRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rsptype     [IN]        
 *  Chnl        [IN]        
 *  Stream      [IN]        
 *  Error       [IN]        
 *  CapType     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_SetConfigRsp(
            U8 rsptype,
            AvdtpChannel *Chnl,
            AvdtpStream *Stream,
            AvdtpError Error,
            AvdtpCapabilityType CapType)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Chnl != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);

    OS_LockStack();
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SET_CONFIG_RSP);
    if (Stream->chnl)
    {
        if (IsNodeOnList(&AVDTP(chnlList), &Chnl->node))
        {
            if (IsNodeOnList(&Chnl->streamList, &Stream->node))
            {
                Chnl->currentTxStream = Stream;
                if (rsptype == AVDTP_RSP_ACCEPT)
                {
                    if (!IsListEmpty(&AVDTP(txPacketList)))
                    {
                        /* Save configuration state configured parms */
                        Chnl->rspSigPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
                        Stream->codecCfg.codecType = Stream->cfgReq.codec.codecType;
                        Stream->codecCfg.elemLen = Stream->cfgReq.codec.elemLen;
                        OS_MemCopy(
                            (U8*) Stream->codecCfg.elements,
                            (U8*) Stream->cfgReq.codec.elements,
                            Stream->cfgReq.codec.elemLen);

                        /* Accept the Open request */
                        Chnl->rspSigPacket->msgType = AVTP_MSG_TYPE_ACCEPT;
                        Chnl->rspSigPacket->txIdSize = 1;
                        Chnl->rspSigPacket->txId = AVDTP_SIG_SET_CONFIG;
                        Chnl->rspSigPacket->txDataLen = 0;
                        Chnl->rspSigPacket->useMsgHdr = FALSE;
                        Chnl->rspSigPacket->internal_used = 0;
                        status = AVTP_SendStart(&Chnl->sigChnl, Chnl->rspSigPacket);
                        if (status == BT_STATUS_PENDING)
                        {
                            Stream->state = AVDTP_STRM_STATE_CONFIGURED;
                        }
                        else
                        {
                            InsertTailList(&AVDTP(txPacketList), &Chnl->rspSigPacket->node);
                        }
                    }
                    else
                    {
                        status = BT_STATUS_NO_RESOURCES;
                    }
                }
                else
                {
                    if (!IsListEmpty(&AVDTP(txPacketList)))
                    {
                        /* Reject the Open request */
                        Chnl->rspSigPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
                        Chnl->rspSigPacket->msgType = AVTP_MSG_TYPE_REJECT;
                        Chnl->rspSigPacket->txIdSize = 1;
                        Chnl->rspSigPacket->txId = AVDTP_SIG_SET_CONFIG;
                        Chnl->rspSigPacket->txData[0] = CapType;
                        Chnl->rspSigPacket->txData[1] = Error;
                        Chnl->rspSigPacket->txDataLen = 2;
                        status = AVTP_SendStart(&Chnl->sigChnl, Chnl->rspSigPacket);
                        if (status != BT_STATUS_PENDING)
                        {
                            InsertTailList(&AVDTP(txPacketList), &Chnl->rspSigPacket->node);
                        }
                    }
                    else
                    {
                        status = BT_STATUS_NO_RESOURCES;
                    }
                }
            }
            else
            {
                status = BT_STATUS_NOT_FOUND;
            }
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_NO_CONNECTION;
    }

    OS_UnlockStack();
    if ((rsptype == AVDTP_RSP_REJECT) && (Error > 0xBF))
    {
        status = BT_STATUS_INVALID_PARM;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_OpenStreamRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rsptype     [IN]        
 *  Stream      [IN]        
 *  Error       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_OpenStreamRsp(U8 rsptype, AvdtpStream *Stream, AvdtpError Error)
{
    U8 msgType = AVTP_MSG_TYPE_REJECT;
    AvdtpChannel *chnl;
    BtStatus status;
    AvtpPacket *rspPacket;

    chnl = Stream->chnl;
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_OPEN_RSP);
    if (IsListEmpty(&AVDTP(txPacketList)))
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_NO_RESOURCE);
        return BT_STATUS_NO_RESOURCES;
    }
    else
    {
        rspPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
        chnl->rspSigPacket = rspPacket;
    }
    rspPacket->txId = AVDTP_SIG_OPEN;
    rspPacket->txIdSize = 1;
    rspPacket->useMsgHdr = FALSE;
    rspPacket->internal_used = 0;
    if (rsptype == AVDTP_RSP_REJECT)
    {
        msgType = AVTP_MSG_TYPE_REJECT;
        rspPacket->txData[0] = Error;
        rspPacket->txDataLen = 1;
    }
    else
    {
        Stream->state = AVDTP_STRM_STATE_OPEN;
        rspPacket->txDataLen = 0;
        msgType = AVTP_MSG_TYPE_ACCEPT;
    }
    rspPacket->msgType = msgType;
    status = AVTP_SendStart(&chnl->sigChnl, rspPacket);
    if (status != BT_STATUS_PENDING)
    {
        InsertTailList(&AVDTP(txPacketList), &rspPacket->node);
    }
    if ((rsptype == AVDTP_RSP_REJECT) && (Error > 0xBF))
    {
        status = BT_STATUS_INVALID_PARM;
    }
    return status;

}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_CloseStreamRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rsptype     [IN]        
 *  Stream      [IN]        
 *  Error       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_CloseStreamRsp(U8 rsptype, AvdtpStream *Stream, AvdtpError Error)
{
    U8 msgType = AVTP_MSG_TYPE_REJECT;
    AvdtpChannel *chnl;
    BtStatus status;
    AvtpPacket *rspPacket;

    chnl = Stream->chnl;
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_CLOSE_RSP);
    if (IsListEmpty(&AVDTP(txPacketList)))
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_NO_RESOURCE);
        return BT_STATUS_NO_RESOURCES;
    }
    else
    {
        rspPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
        chnl->rspSigPacket = rspPacket;
    }
    rspPacket->txId = AVDTP_SIG_CLOSE;
    rspPacket->txIdSize = 1;
    rspPacket->useMsgHdr = FALSE;
    rspPacket->internal_used = 0;
    if (rsptype != AVDTP_RSP_REJECT)
    {
        msgType = AVTP_MSG_TYPE_ACCEPT;
        rspPacket->txDataLen = 0;
    }
    else
    {
        msgType = AVTP_MSG_TYPE_REJECT;
        Assert(Stream->locStrmId != 0xFF);
        rspPacket->txData[0] = Error;
        rspPacket->txDataLen = 1;
    }
    rspPacket->msgType = msgType;
    status = AVTP_SendStart(&chnl->sigChnl, rspPacket);
    if (status != BT_STATUS_PENDING)
    {
        InsertTailList(&AVDTP(txPacketList), &rspPacket->node);
    }
    if ((rsptype == AVDTP_RSP_REJECT) && (Error > 0xBF))
    {
        status = BT_STATUS_INVALID_PARM;
    }
    return status;

}

/*---------------------------------------------------------------------------
 * AVDTP_ReconfigRsp()
 *---------------------------------------------------------------------------
 *
 * Synopsis:
 *
 * Return:    (See header file)
 */


/*****************************************************************************
 * FUNCTION
 *  AVDTP_ReconfigRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rsptype     [IN]        
 *  Stream      [IN]        
 *  Error       [IN]        
 *  Type        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_ReconfigRsp(U8 rsptype, AvdtpStream *Stream, AvdtpError Error, AvdtpCapabilityType Type)
{
    AvdtpChannel *chnl;
    BtStatus status;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_RECONFIG_RSP);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);

    OS_LockStack();
    if (Stream->chnl)
    {
        chnl = Stream->chnl;
        if (IsNodeOnList(&AVDTP(chnlList), &Stream->chnl->node))
        {
            if (IsNodeOnList(&Stream->chnl->streamList, &Stream->node))
            {
                status = AvdtpSigStreamReconfigRsp(rsptype, Stream->chnl, Stream, Error, Type);
            }
            else
            {
                status = BT_STATUS_NOT_FOUND;
            }
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_NO_CONNECTION;
    }

    if ((rsptype == AVDTP_RSP_REJECT) && (Error > 0xBF))
    {
        status = BT_STATUS_INVALID_PARM;
    }

    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 * AVDTP_StartStreamRsp()
 *---------------------------------------------------------------------------
 *
 * Synopsis:
 *
 * Return:    (See header file)
 */


/*****************************************************************************
 * FUNCTION
 *  AVDTP_StartStreamRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rsptype     [IN]        
 *  Stream      [IN]        
 *  Error       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_StartStreamRsp(U8 rsptype, AvdtpStream *Stream, AvdtpError Error)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);

    OS_LockStack();
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_START_RSP);
    if (Stream->chnl)
    {
        if (IsNodeOnList(&AVDTP(chnlList), &Stream->chnl->node))
        {
            if (IsNodeOnList(&Stream->chnl->streamList, &Stream->node))
            {
                status = AvdtpSigStreamStartRsp(rsptype, Stream->chnl, Stream, Error);
            }
            else
            {
                status = BT_STATUS_NOT_FOUND;
            }
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_NO_CONNECTION;
    }
    if ((rsptype == AVDTP_RSP_REJECT) && (Error > 0xBF))
    {
        status = BT_STATUS_INVALID_PARM;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_SuspendStreamRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rsptype     [IN]        
 *  Stream      [IN]        
 *  Error       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_SuspendStreamRsp(U8 rsptype, AvdtpStream *Stream, AvdtpError Error)
{
    U8 msgType = AVTP_MSG_TYPE_REJECT;
    AvdtpChannel *chnl;
    BtStatus status;
    AvtpPacket *rspPacket;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SUSPEND_RSP);
    chnl = Stream->chnl;
    if (IsListEmpty(&AVDTP(txPacketList)))
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_NO_RESOURCE);
        return BT_STATUS_NO_RESOURCES;
    }
    else
    {
        rspPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
        chnl->rspSigPacket = rspPacket;
    }
    rspPacket->txId = AVDTP_SIG_SUSPEND;
    rspPacket->txIdSize = 1;
    rspPacket->useMsgHdr = FALSE;
    rspPacket->internal_used = 0;
    if (rsptype != AVDTP_RSP_REJECT)
    {
        msgType = AVTP_MSG_TYPE_ACCEPT;
        rspPacket->txDataLen = 0;
    }
    else
    {
        Stream->state = AVDTP_STRM_STATE_OPEN;
        msgType = AVTP_MSG_TYPE_REJECT;
        Assert(Stream->locStrmId != 0xFF);
        rspPacket->txData[0] = Stream->locStrmId << 2;
        rspPacket->txData[1] = Error;
        rspPacket->txDataLen = 2;
    }
    rspPacket->msgType = msgType;
    status = AVTP_SendStart(&chnl->sigChnl, rspPacket);
    if (status != BT_STATUS_PENDING)
    {
        InsertTailList(&AVDTP(txPacketList), &rspPacket->node);
    }
    if ((rsptype == AVDTP_RSP_REJECT) && (Error > 0xBF))
    {
        status = BT_STATUS_INVALID_PARM;
    }
    return status;

}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_AbortStreamRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_AbortStreamRsp(AvdtpStream *Stream)
{
    BtStatus status;
    U8 msgType = AVTP_MSG_TYPE_REJECT;
    AvdtpChannel *chnl;
    AvtpPacket *rspPacket;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_ABORT_RSP);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Stream != 0);
    OS_LockStack();

    if (Stream->chnl)
    {
        if (IsNodeOnList(&AVDTP(chnlList), &Stream->chnl->node))
        {
            if (Stream->chnl->rxAbortCmd == AVDTP_RX_ABORT_CMD)
            {
                Stream->chnl->rxAbortAcked |= AVDTP_ABORT_RSP_PATH_RESPONDED;
                if (Stream->chnl->rxAbortAcked == (AVDTP_ABORT_RSP_PATH_RESPONDED | AVDTP_ABORT_CFM_PATH_RESPONDED))
                {
                    Stream->chnl->rxAbortCmd = AVDTP_NOT_RX_ABORT_CMD;
                    Stream->chnl->rxAbortAcked = 0;
                }
            }
            if ((Stream->state >= AVDTP_STRM_STATE_CONFIGURED) && (Stream->state <= AVDTP_STRM_STATE_CLOSING))
            {
                chnl = Stream->chnl;
                if (IsListEmpty(&AVDTP(txPacketList)))
                {
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_NO_RESOURCE);
                    return BT_STATUS_NO_RESOURCES;
                }
                else
                {
                    rspPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
                    chnl->rspSigPacket = rspPacket;
                }
                msgType = AVTP_MSG_TYPE_ACCEPT;
                rspPacket->txId = AVDTP_SIG_ABORT;
                rspPacket->txDataLen = 0;
                rspPacket->msgType = msgType;
                status = AVTP_SendStart(&chnl->sigChnl, rspPacket);
                if (status != BT_STATUS_PENDING)
                {
                    InsertTailList(&AVDTP(txPacketList), &rspPacket->node);
                }
                else if (status == BT_STATUS_PENDING)
                {
                    if (Stream->state != AVDTP_STRM_STATE_CONFIGURED)
                    {
                        Stream->state = AVDTP_STRM_STATE_ABORTING;
                    }
                    else
                    {
                        Stream->state = AVDTP_STRM_STATE_IDLE;
                    }
                }
            }
            else
            {
                status = BT_STATUS_FAILED;
            }
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_NO_CONNECTION;
    }

    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 * AVDTP_StreamSendRawPacket()
 *---------------------------------------------------------------------------
 *
 * Synopsis:
 *
 * Return:    (See header file)
 */


/*****************************************************************************
 * FUNCTION
 *  AVDTP_StreamSendRawPacket
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 *  Packet      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_StreamSendRawPacket(AvdtpStream *Stream, BtPacket *Packet)
{
    BtStatus status;

    OS_LockStack();

    if (Stream->chnl)
    {
        if (IsNodeOnList(&AVDTP(chnlList), &Stream->chnl->node))
        {
            if (IsNodeOnList(&Stream->chnl->streamList, &Stream->node))
            {
                if (Stream->state == AVDTP_STRM_STATE_STREAMING)
                {
                    Packet->priority_type = BTP_FLAG_HIGH_PRIPORTY;
                    status = L2CAP_AvSendData(Stream->conn.l2ChannelId, Packet);
                }
                else
                {
                    status = BT_STATUS_FAILED;
                }
            }
            else
            {
                status = BT_STATUS_NOT_FOUND;
            }
        }
        else
        {
            status = BT_STATUS_NOT_FOUND;
        }
    }
    else
    {
        status = BT_STATUS_NO_CONNECTION;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_SendStreamData
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 *  length      [IN]        
 *  buffer      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL AVDTP_SendStreamData(AvdtpStream *Stream, U16 length, U8 *buffer)
{
    AvdtpChannel *chnl = 0;
    BtPacket *btp = 0;
    BtStatus status;

    chnl = Stream->chnl;
    if (chnl == 0)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_CHANNEL_IS_ZERO);
        return FALSE;
    }
    if (Stream->state != AVDTP_STRM_STATE_STREAMING)
    {
        bt_trace(TRACE_GROUP_1, SENDRAWPACKETA);
        return FALSE;
    }
    if (IsListEmpty(&chnl->rtp_pkt_List))
    {
        if (AVDTP_Flush_Transmitted_Pkt(Stream) == 0)
        {
            bt_trace(TRACE_GROUP_1, SENDRAWPACKETB);
            return FALSE;
        }
    }
    btp = (BtPacket*) RemoveHeadList(&chnl->rtp_pkt_List);
    btp->dataLen = length;
    btp->data = buffer;
    btp->headerLen = 0;
    btp->tailLen = 0;
    btp->flags = BTP_FLAG_INUSE;
    status = AVDTP_StreamSendRawPacket(Stream, btp);
    if (status != BT_STATUS_PENDING)
    {
        bt_trace(TRACE_GROUP_1, AVDTP_STREAMSENDRAWPACKETBT_DEBUGx02X, status);
#if 1
        if(status != BT_STATUS_SUCCESS)
        {
            InsertTailList(&chnl->rtp_pkt_List, &btp->node);
            return FALSE;
        }
#endif        
    }
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_Flush_Transmitted_Pkt
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 AVDTP_Flush_Transmitted_Pkt(AvdtpStream *Stream)
{
    U8 flushed_number = 0;

    flushed_number = L2Cap_FlushOnePktTransmits(Stream->conn.l2ChannelId);
    return flushed_number;
}

/*---------------------------------------------------------------------------
 * AVDTP_GetRemoteDevice()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the remote device structure used by this channel.  Only 
 *            valid when a connection exists.
 *
 * Return:    (See header file)
 */


/*****************************************************************************
 * FUNCTION
 *  AVDTP_GetRemoteDevice
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtRemoteDevice *AVDTP_GetRemoteDevice(AvdtpChannel *Channel)
{
    BtRemoteDevice *remDev = 0;

    OS_LockStack();

    if (Channel != 0)
    {
        if (IsNodeOnList(&AVDTP(chnlList), &Channel->node) && (Channel->conn.state != AVDTP_STATE_DISCONNECTED))
        {

            /* This is a registered/connected channel */
            remDev = Channel->conn.remDev;
        }
    }

    OS_UnlockStack();

    return remDev;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_IsConnected
 * DESCRIPTION
 *  Returns the current state of an AVDTP connection.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL AVDTP_IsConnected(AvdtpChannel *Channel)
{
    BOOL connected = FALSE;

    OS_LockStack();

    if (Channel != 0)
    {
        if (IsNodeOnList(&AVDTP(chnlList), &Channel->node))
        {
            connected = (BOOL)((Channel->conn.state == AVDTP_STATE_CONNECTED) ? TRUE : FALSE);
        }
    }

    OS_UnlockStack();

    return connected;
}

/*---------------------------------------------------------------------------
 * AVDTP_StreamState()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the current state of a stream.
 *
 * Return:    (See header file)
 */


/*****************************************************************************
 * FUNCTION
 *  AVDTP_GetStreamState
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Stream      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
AvdtpStreamState AVDTP_GetStreamState(AvdtpStream *Stream)
{
    AvdtpStreamState state = AVDTP_STRM_STATE_IDLE;

    OS_LockStack();

    if (Stream != 0)
    {
        if (Stream->chnl)
        {
            if (IsNodeOnList(&Stream->chnl->streamList, &Stream->node))
            {
                /* This is a registered stream */
                state = Stream->state;
            }
        }
    }

    OS_UnlockStack();

    return state;
}

/*---------------------------------------------------------------------------
 * AVDTP_CreateMediaHeader()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Used to create a media packet header before transmiting media data 
 *            using the AVDTP_StreamSendRawPacket().
 *
 * Returns:   (See header file)
 */


/*****************************************************************************
 * FUNCTION
 *  AVDTP_CreateMediaHeader
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Header      [IN]        
 *  Buffer      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U16 AVDTP_CreateMediaHeader(AvdtpMediaHeader *Header, U8 *Buffer)
{
    /* I16 i = Header->csrcCount; */

    if (Buffer)
    {
        /* First Octet */
        Buffer[0] =
            ((Header->version << 6) & 0xC0) | ((Header->padding << 5) & 0x20) | 
             (Header->csrcCount > 0 ? 0x10 : 0x00) | (Header->csrcCount & 0x0F);
                                                                                                     

        /* 2nd Octet */
        Buffer[1] = ((Header->marker << 7) & 0x80) | (Header->payloadType & 0x7F);

        /* Sequence number */
        StoreBE16(&Buffer[2], Header->sequenceNumber);

        /* Timestamp */
        StoreBE32(&Buffer[4], Header->timestamp);

        /* Synch source */
        StoreBE32(&Buffer[8], Header->ssrc);
        /* Contributing Sources */
        //        for (i = 0; i < Header->csrcCount; i++) {
        //            if (Buffer) {
        //                StoreBE32(&Buffer[12 + i * 4], Header->csrcList[i]);
        //            }
        //        }
    }

    /* Return the header size */
    return 12;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_ParseMediaHeader
 * DESCRIPTION
 *  Used to parse a media packet header when a media packet has been received from the remote device.
 * PARAMETERS
 *  Header      [IN]        
 *  Buffer      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U16 AVDTP_ParseMediaHeader(AvdtpMediaHeader *Header, U8 *Buffer)
{
    I16 i = 0;

    Header->version = Buffer[0] >> 6;
    Header->padding = (Buffer[0] & 0x20) >> 5;
    Header->csrcCount = Buffer[0] & 0x0F;
    Header->marker = Buffer[1] >> 7;
    Header->payloadType = (Buffer[1] & 0x7F);
    Header->sequenceNumber = BEtoHost16(&Buffer[2]);
    Header->timestamp = BEtoHost32(&Buffer[4]);
    Header->ssrc = BEtoHost32(&Buffer[8]);

    if (Buffer[0] & 0x10)
    {
        /* An extention header exists */
        for (i = 0; i < Header->csrcCount; i++)
        {
            Header->csrcList[i] = BEtoHost32(&Buffer[12 + i]);
        }
    }

    /* Return the header size */
    return 12 + i * 4;
}

/* Used by MTK Advtp Testing Layer Only */


/*****************************************************************************
 * FUNCTION
 *  AVDTP_DeregisterAllChnl
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_DeregisterAllChnl(void)
{
    BtStatus status = BT_STATUS_SUCCESS;
    AvdtpChannel *Chnl;
    AvdtpStream *Stream;

    OS_LockStack();
    while (!(IsListEmpty(&AVDTP(chnlList))))
    {
        Chnl = (AvdtpChannel*) RemoveHeadList(&AVDTP(chnlList));
    }
    while (!(IsListEmpty(&AVDTP(streamList))))
    {
        Stream = (AvdtpStream*) RemoveHeadList(&AVDTP(streamList));
        AVDTP(numStreams)--;
    }
    status = BT_STATUS_SUCCESS;
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVDTP_DiscoverStreamsWithErrorRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rsptype             [IN]        
 *  Chnl                [IN]        
 *  stream_info_num     [IN]        
 *  stream_info         [IN]        
 *  Error               [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVDTP_DiscoverStreamsWithErrorRsp(
            U8 rsptype,
            AvdtpChannel *Chnl,
            U8 stream_info_num,
            AvdtpStreamInfo *stream_info,
            AvdtpError Error)
{
    U8 i = 0;
    U8 stream_number = 0;
    U8 msgType = AVTP_MSG_TYPE_REJECT;
    BtStatus status;
    AvtpPacket *rspPacket;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_DISCOVERY_RSP);
    if (IsListEmpty(&AVDTP(txPacketList)))
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_NO_RESOURCE);
        return BT_STATUS_NO_RESOURCES;
    }
    else
    {
        rspPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
        Chnl->rspSigPacket = rspPacket;
    }
    rspPacket->txId = AVDTP_SIG_DISCOVER;
    rspPacket->txIdSize = 1;
    rspPacket->txDataLen = 0;
    rspPacket->useMsgHdr = FALSE;
    rspPacket->internal_used = 0;
    if (rsptype == AVDTP_RSP_REJECT)
    {
        msgType = AVTP_MSG_TYPE_REJECT;
        rspPacket->txDataLen = 0;
    }
    else
    {
        for (stream_number = 0; stream_number < stream_info_num; stream_number++)
        {
            rspPacket->txData[i] = stream_info[stream_number].id << 2;
            rspPacket->txData[i] |= stream_info[stream_number].inUse << 1;
            rspPacket->txData[i + 1] = stream_info[stream_number].mediaType << 4;
            rspPacket->txData[i + 1] |= stream_info[stream_number].streamType << 3;
            rspPacket->txDataLen += 2;
            i += 2;
        }
        msgType = AVTP_MSG_TYPE_ACCEPT;
    }

    if (msgType == AVTP_MSG_TYPE_REJECT)
    {
        /* No streams available; reject with anonymous error code. */
        rspPacket->txData[0] = 0xC0;
        rspPacket->txDataLen = 1;
    }
    rspPacket->msgType = msgType;
    status = AVTP_SendStart(&Chnl->sigChnl, rspPacket);
    if (status != BT_STATUS_PENDING)
    {
        InsertTailList(&AVDTP(txPacketList), &rspPacket->node);
    }
    if ((rsptype == AVDTP_RSP_REJECT) && (Error > 0xBF))
    {
        status = BT_STATUS_INVALID_PARM;
    }
    return status;
}

