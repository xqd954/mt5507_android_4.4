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
 *     $Workfile:avsigmgr.c$ for XTNDAccess Blue SDK, Version 1.4
 *     $Revision: #1 $
 *
 * Description: This file contains the Signal Manager.
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
#include "btconfig.h"
#include "avdtp.h"
#include "sys/avalloc.h"
#include "sys/avsigmgr.h"
#include "bt_feature.h"

#include "btbm.h"
#include "bt_devices_list.h"



#ifdef __BT_SINGLE_ANTENNA__
#include "bt_wmt_api.h"
#endif


extern DEVICE_MODEL BTBMGetDeviceModel(BD_ADDR *addr);


/* Function Prototypes */
static void AvdtpSigMgrSignalCallback(AvtpChannel *SigChnl, AvtpCallbackParms *Parms);
static void AvdtpSigMgrConnCallback(AvdtpConn *Conn, AvdtpConnCallbackParms *Parms);


/*****************************************************************************
 * FUNCTION
 *  AvdtpInsertStreamList
 * DESCRIPTION
 *  Inserts a stream (in order) onto the unused stream list.
 * PARAMETERS
 *  Stream      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvdtpInsertStreamList(AvdtpStream *Stream)
{
    AvdtpStream *strm = (AvdtpStream*) GetHeadList(&AVDTP(streamList));

    while (strm != (AvdtpStream*) & AVDTP(streamList))
    {
        if (Stream->locStrmId > strm->locStrmId)
        {
            Assert(Stream->locStrmId != 0xFF);
            strm = (AvdtpStream*) GetNextNode(&strm->node);
            continue;
        }
        break;
    }

    InsertTailList(&strm->node, &Stream->node);
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigInit
 * DESCRIPTION
 *  Initialize the Signal Manager handler.
 * PARAMETERS
 *  Chnl        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvdtpSigInit(AvdtpChannel *Chnl)
{
    AvdtpInitConn(&Chnl->conn, AvdtpSigMgrConnCallback);
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigSendCommand
 * DESCRIPTION
 *  Send a signal command.
 * PARAMETERS
 *  Chnl        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus AvdtpSigSendCommand(AvdtpChannel *Chnl)
{
    BtStatus status;

    if ((Chnl->txState == AVTP_SIG_MGR_TXSTATE_IDLE) || (Chnl->cmdSigPacket->txId == AVDTP_SIG_ABORT))
    {
        Chnl->txState = AVTP_SIG_MGR_TXSTATE_WAIT_RSP;
        status = AVTP_SendStart(&Chnl->sigChnl, Chnl->cmdSigPacket);
    }
    else
    {
        status = BT_STATUS_BUSY;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigStreamSendSimpleCmd
 * DESCRIPTION
 *  Send a simple command on the signal channel.
 * PARAMETERS
 *  Chnl        [IN]        
 *  SigCmd      [IN]        
 *  Stream      [IN]        
 *  StrmId      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AvdtpSigStreamSendSimpleCmd(AvdtpChannel *Chnl, U8 SigCmd, AvdtpStream *Stream, AvdtpStreamId StrmId)
{
    BtStatus status = BT_STATUS_NO_RESOURCES;

    if (!IsListEmpty(&AVDTP(txPacketList)))
    {
        Chnl->cmdSigPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
        if (SigCmd != AVDTP_SIG_DISCOVER)
        {
            Chnl->cmdSigPacket->txData[0] = StrmId << 2;
            Chnl->cmdSigPacket->txDataLen = 1;
        }
        else
        {
            Chnl->cmdSigPacket->txDataLen = 0;
        }

        Chnl->cmdSigPacket->msgType = AVTP_MSG_TYPE_COMMAND;
        Chnl->cmdSigPacket->useMsgHdr = FALSE;
        Chnl->cmdSigPacket->txIdSize = 1;
        Chnl->cmdSigPacket->txId = SigCmd;
        Chnl->cmdSigPacket->internal_used = 0;
        Chnl->currentTxStream = Stream;
        status = AvdtpSigSendCommand(Chnl);
        if (status != BT_STATUS_PENDING)
        {
            InsertTailList(&AVDTP(txPacketList), &Chnl->cmdSigPacket->node);
        }
    }
    else
    {
        bt_trace(TRACE_GROUP_1, NO_SIGNAL_PKT1);
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigStreamSetConfig
 * DESCRIPTION
 *  Set the stream configuration.
 * PARAMETERS
 *  Chnl        [IN]        
 *  Stream      [IN]        
 *  Codec       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AvdtpSigStreamSetConfig(AvdtpChannel *Chnl, AvdtpStream *Stream, AvdtpCodec *Codec)
{
    BtStatus status = BT_STATUS_NO_RESOURCES;

    if (!IsListEmpty(&AVDTP(txPacketList)))
    {
        Chnl->cmdSigPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
        Chnl->cmdSigPacket->txData[0] = Stream->remStrmId << 2;
        Chnl->cmdSigPacket->txData[1] = Stream->locStrmId << 2;
        Assert(Stream->locStrmId != 0xFF);
        Chnl->cmdSigPacket->txDataLen = 2;
        Chnl->cmdSigPacket->msgType = AVTP_MSG_TYPE_COMMAND;
        Chnl->cmdSigPacket->useMsgHdr = FALSE;
        Chnl->cmdSigPacket->txIdSize = 1;
        Chnl->cmdSigPacket->internal_used = 0;
        Chnl->cmdSigPacket->txId = AVDTP_SIG_SET_CONFIG;

        /* No content protection */
        Stream->cfgReq.cp.cpType = 0;
        Stream->cfgReq.cp.dataLen = 0;

        /* Save the codec request */
        Stream->cfgReq.codec.codecType = Codec->codecType;
        Stream->cfgReq.codec.elemLen = min(Codec->elemLen, AVDTP_MAX_CODEC_ELEM_SIZE);
        OS_MemCopy((U8*) Stream->cfgReq.codec.elements, (U8*) Codec->elements, Codec->elemLen);

        /* Encode the packet */
        Stream->chnl = Chnl;
        AvdtpSigEncodeCapabilities(Chnl->cmdSigPacket, Stream, AVDTP_SIG_SET_CONFIG);
        Chnl->currentTxStream = Stream;
        status = AvdtpSigSendCommand(Chnl);
        if (status != BT_STATUS_PENDING)
        {
            InsertTailList(&AVDTP(txPacketList), &Chnl->cmdSigPacket->node);
        }
    }
    else
    {
        bt_trace(TRACE_GROUP_1, NO_SIGNAL_PKT2);
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigStreamReconfig
 * DESCRIPTION
 *  Reconfigure the stream.
 * PARAMETERS
 *  Chnl        [IN]        
 *  Stream      [IN]        
 *  Codec       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AvdtpSigStreamReconfig(AvdtpChannel *Chnl, AvdtpStream *Stream, AvdtpCodec *Codec)
{
    BtStatus status = BT_STATUS_NO_RESOURCES;

    if (!IsListEmpty(&AVDTP(txPacketList)))
    {
        Chnl->cmdSigPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
        Chnl->cmdSigPacket->txData[0] = Stream->remStrmId << 2;
        Chnl->cmdSigPacket->txDataLen = 1;
        Chnl->cmdSigPacket->msgType = AVTP_MSG_TYPE_COMMAND;
        Chnl->cmdSigPacket->useMsgHdr = FALSE;
        Chnl->cmdSigPacket->txIdSize = 1;
        Chnl->cmdSigPacket->internal_used = 0;
        Chnl->cmdSigPacket->txId = AVDTP_SIG_RECONFIG;

        /* Save the content protection request */
        /* No content protection */
        Stream->cfgReq.cp.cpType = 0;
        Stream->cfgReq.cp.dataLen = 0;

        /* Save the codec request */
        Stream->cfgReq.codec.codecType = Codec->codecType;
        Stream->cfgReq.codec.elemLen = min(Codec->elemLen, AVDTP_MAX_CODEC_ELEM_SIZE);
        OS_MemCopy((U8*) Stream->cfgReq.codec.elements, (U8*) Codec->elements, Codec->elemLen);

        /* Encode the packet */
        AvdtpSigEncodeCapabilities(Chnl->cmdSigPacket, Stream, AVDTP_SIG_RECONFIG);
        Chnl->currentTxStream = Stream;
        status = AvdtpSigSendCommand(Chnl);
        if (status != BT_STATUS_PENDING)
        {
            InsertTailList(&AVDTP(txPacketList), &Chnl->cmdSigPacket->node);
        }
    }
    else
    {
        bt_trace(TRACE_GROUP_1, NO_SIGNAL_PKT3);
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigStreamReconfigRsp
 * DESCRIPTION
 *  Send start stream response.
 * PARAMETERS
 *  rsptype     [IN]        
 *  Chnl        [IN]        
 *  Stream      [IN]        
 *  Error       [IN]        
 *  Type        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AvdtpSigStreamReconfigRsp(
            U8 rsptype,
            AvdtpChannel *Chnl,
            AvdtpStream *Stream,
            AvdtpError Error,
            AvdtpCapabilityType Type)
{
    BtStatus status = BT_STATUS_NO_RESOURCES;

    if (!IsListEmpty(&AVDTP(txPacketList)))
    {
        Chnl->rspSigPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
        Chnl->rspSigPacket->useMsgHdr = FALSE;
        Chnl->rspSigPacket->txIdSize = 1;
        Chnl->rspSigPacket->internal_used = 0;
        Chnl->rspSigPacket->txId = AVDTP_SIG_RECONFIG;
        if (rsptype == AVDTP_RSP_ACCEPT)
        {

            /* Configuration was accepted, move it to configured variables */
            Stream->codecCfg.codecType = Stream->cfgReq.codec.codecType;
            Stream->codecCfg.elemLen = Stream->cfgReq.codec.elemLen;
            OS_MemCopy(
                (U8*) Stream->codecCfg.elements,
                (U8*) Stream->cfgReq.codec.elements,
                Stream->cfgReq.codec.elemLen);

            Chnl->rspSigPacket->msgType = AVTP_MSG_TYPE_ACCEPT;
            Chnl->rspSigPacket->txDataLen = 0;
        }
        else
        {
            Chnl->rspSigPacket->msgType = AVTP_MSG_TYPE_REJECT;
            Chnl->rspSigPacket->txData[0] = Type;
            Chnl->rspSigPacket->txData[1] = Error;
            Chnl->rspSigPacket->txDataLen = 2;
        }

        Chnl->currentTxStream = Stream;
        status = AVTP_SendStart(&Chnl->sigChnl, Chnl->rspSigPacket);
        if (status != BT_STATUS_PENDING)
        {
            InsertTailList(&AVDTP(txPacketList), &Chnl->rspSigPacket->node);
        }
    }
    else
    {
        bt_trace(TRACE_GROUP_1, NO_SIGNAL_PKT4);
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigStreamSecurityCtrl
 * DESCRIPTION
 *  Send security control information.
 * PARAMETERS
 *  Chnl        [IN]        
 *  Stream      [IN]        
 *  Len         [IN]        
 *  Data        [IN]        
 *  msgType     [IN]        
 *  Error       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AvdtpSigStreamSecurityCtrl(
            AvdtpChannel *Chnl,
            AvdtpStream *Stream,
            U16 Len,
            U8 *Data,
            U8 msgType,
            AvdtpError Error)
{
    BtStatus status = BT_STATUS_NO_RESOURCES;
    U8 offset = 0;
    AvtpPacket *sigPacket;

    if (msgType == AVTP_MSG_TYPE_COMMAND)
    {
        /* Encode the stream ID */
        if (!IsListEmpty(&AVDTP(txPacketList)))
        {
            Chnl->cmdSigPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
            sigPacket = Chnl->cmdSigPacket;
            sigPacket->txData[offset++] = Stream->remStrmId << 2;
        }
        else
        {
            goto exit;
        }
    }
    else
    {
        if (!IsListEmpty(&AVDTP(txPacketList)))
        {
            Chnl->rspSigPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
            sigPacket = Chnl->rspSigPacket;
        }
        else
        {
            goto exit;
        }
    }

    if (!Error)
    {
        /* Encode the security data */
        OS_MemCopy(&sigPacket->txData[offset], Data, Len);
        offset += Len;
    }
    else
    {
        sigPacket->txData[offset] = Error;
        offset++;
    }

    sigPacket->txDataLen = offset;
    sigPacket->msgType = msgType;
    sigPacket->useMsgHdr = FALSE;
    sigPacket->txIdSize = 1;
    sigPacket->internal_used = 0;
    sigPacket->txId = AVDTP_SIG_SECURITY_CTRL;
    Chnl->currentTxStream = Stream;
    if (msgType == AVTP_MSG_TYPE_COMMAND)
    {
        status = AvdtpSigSendCommand(Chnl);
    }
    else
    {
        status = AVTP_SendStart(&Chnl->sigChnl, sigPacket);
    }

    if (status != BT_STATUS_PENDING)
    {
        InsertTailList(&AVDTP(txPacketList), &sigPacket->node);
    }

  exit:

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigStreamStartRsp
 * DESCRIPTION
 *  Send start stream response.
 * PARAMETERS
 *  rsptype     [IN]        
 *  Chnl        [IN]        
 *  Stream      [IN]        
 *  Error       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AvdtpSigStreamStartRsp(U8 rsptype, AvdtpChannel *Chnl, AvdtpStream *Stream, AvdtpError Error)
{
    BtStatus status = BT_STATUS_PENDING;

    if (Chnl->streamsLeft)
    {
        if (!IsListEmpty(&AVDTP(txPacketList)))
        {
            /* Allocated the response packet */
            Chnl->rspSigPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
            Chnl->rspSigPacket->useMsgHdr = FALSE;
            Chnl->rspSigPacket->txIdSize = 1;
            Chnl->rspSigPacket->internal_used = 0;
            Chnl->rspSigPacket->txId = AVDTP_SIG_START;
            if (rsptype == AVDTP_RSP_ACCEPT)
            {
                Chnl->rspSigPacket->msgType = AVTP_MSG_TYPE_ACCEPT;
                Chnl->rspSigPacket->txDataLen = 0;
            }
            else
            {
                Chnl->rspSigPacket->msgType = AVTP_MSG_TYPE_REJECT;
                Chnl->rspSigPacket->txData[0] = Stream->locStrmId << 2;
                Assert(Stream->locStrmId != 0xFF);
                Chnl->rspSigPacket->txData[1] = Error;
                Chnl->rspSigPacket->txDataLen = 2;
            }

            Chnl->currentTxStream = Stream;
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
    else
    {
        status = BT_STATUS_FAILED;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigEncodeCapabilities
 * DESCRIPTION
 *  Encode capabilties into the signal channel.
 * PARAMETERS
 *  SigPacket       [IN]        
 *  Stream          [IN]        
 *  signal          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvdtpSigEncodeCapabilities(AvtpPacket *SigPacket, AvdtpStream *Stream, U8 signal)
{
    /* AvdtpContentProt *cp; */
    AvdtpCodec *codec;

    if (signal != AVDTP_SIG_RECONFIG)
    {
        /* Add the Media Transport */
        SigPacket->txData[SigPacket->txDataLen] = AVDTP_SRV_CAT_MEDIA_TRANSPORT;
        SigPacket->txData[SigPacket->txDataLen + 1] = 0;
        SigPacket->txDataLen += 2;
    }

    if ((signal == AVDTP_SIG_SET_CONFIG) || (signal == AVDTP_SIG_RECONFIG))
    {
        /* Requested codec */
        codec = &Stream->cfgReq.codec;
    }
    else if (signal == AVDTP_SIG_GET_CONFIG)
    {
        /* Configured codec */
        codec = &Stream->codecCfg;
    }
    else
    {
        /* Registered codec */
        codec = Stream->codec;
    }

    /* Add the Codec */
    SigPacket->txData[SigPacket->txDataLen] = AVDTP_SRV_CAT_MEDIA_CODEC;
    SigPacket->txData[SigPacket->txDataLen + 1] = codec->elemLen + 2;
    SigPacket->txData[SigPacket->txDataLen + 2] = Stream->mediaType << 4;
    SigPacket->txData[SigPacket->txDataLen + 3] = codec->codecType;
    OS_MemCopy((U8*) & SigPacket->txData[SigPacket->txDataLen + 4], (U8*) codec->elements, codec->elemLen);
    SigPacket->txDataLen += codec->elemLen + 4;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigFindStream
 * DESCRIPTION
 *  Find a stream for based on the stream ID.
 * PARAMETERS
 *  Chnl        [IN]        
 *  StrmId      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static AvdtpStream *AvdtpSigFindStream(AvdtpChannel *Chnl, AvdtpStreamId StrmId)
{
    AvdtpStream *strm;

    /* Look on current channel first */
    strm = (AvdtpStream*) GetHeadList(&Chnl->streamList);
    while (strm != (AvdtpStream*) & Chnl->streamList)
    {
        Assert(strm->locStrmId != 0xFF);
        if (strm->locStrmId == StrmId)
        {
            /* Found it */
            break;
        }

        /* Get the next connection structure */
        strm = (AvdtpStream*) GetNextNode(&strm->node);
    }

    if (strm == (AvdtpStream*) & Chnl->streamList)
    {
        /* Look for registered stream */
        strm = (AvdtpStream*) GetHeadList(&AVDTP(streamList));
        while (strm != (AvdtpStream*) & AVDTP(streamList))
        {
            Assert(strm->locStrmId != 0xFF);
            if (strm->locStrmId == StrmId)
            {
                /* Found it */
                break;
            }

            /* Get the next connection structure */
            strm = (AvdtpStream*) GetNextNode(&strm->node);
        }

        if (strm == (AvdtpStream*) & AVDTP(streamList))
        {
            /* Not Found */
            strm = 0;
        }
    }

    return strm;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigParseDiscoveries
 * DESCRIPTION
 *  Parse discovered streams from the signal channel.
 * PARAMETERS
 *  Chnl        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpSigParseDiscoveries(AvdtpChannel *Chnl, AvtpCallbackParms *Parms)
{
    AvdtpCallbackParms info;
    AvdtpStreamInfo streamInfo;
    U16 rxLen = Parms->len;
    U16 n;
    U8 i;

    /* Store the receive buffer */
    Chnl->parser.rxBuff = Parms->data;
    info.status = Parms->status;
    info.error = AVDTP_ERR_NO_ERROR;

    /* Process the data in Chnl->parser.rxBuff of length rxLen */
    while (rxLen > 0)
    {
        Assert(Chnl->parser.stageLen <= 2);
        Assert((Chnl->parser.curStageOff == 0) || (Chnl->parser.curStageOff < Chnl->parser.stageLen));

        n = min((U16) (Chnl->parser.stageLen - Chnl->parser.curStageOff), rxLen);

        /* Stage the data */
        for (i = Chnl->parser.curStageOff; n > 0; n--, i++)
        {
            Chnl->parser.stageBuff[i] = *(Chnl->parser.rxBuff);
            (Chnl->parser.rxBuff)++;
            rxLen--;
            Chnl->parser.curStageOff++;
        }

        /*
         * Only call the state machine if the data has been completely
         * * staged.
         */
        if (Chnl->parser.curStageOff == Chnl->parser.stageLen)
        {

            /* Discovery responses */
            info.event = AVDTP_EVENT_STREAM_INFO;
            info.channel = Chnl;
            streamInfo.id = (Chnl->parser.stageBuff[0] & 0xFC) >> 2;
            streamInfo.inUse = (Chnl->parser.stageBuff[0] & 0x02) >> 1;
            streamInfo.mediaType = Chnl->parser.stageBuff[1] >> 4;
            streamInfo.streamType = (Chnl->parser.stageBuff[1] & 0x08) >> 3;

            /* Make the stream info callback */
            info.len = sizeof(AvdtpStreamInfo);
            info.p.streamInfo = &streamInfo;
            Chnl->callback(Chnl, &info);

            Chnl->parser.curStageOff = 0;
        }
    }

    if (Parms->packetsLeft == 0)
    {
        /* No more streams, Make the discover complete callback */
        info.event = AVDTP_EVENT_DISCOVER_CNF;
        info.len = 0;
        Chnl->callback(Chnl, &info);
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigParseCapabilityRsp
 * DESCRIPTION
 *  Parse capabilties from the signal channel.
 * PARAMETERS
 *  Chnl        [IN]        
 *  Parms       [IN]        
 *  config      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpSigParseCapabilityRsp(AvdtpChannel *Chnl, AvtpCallbackParms *Parms, BOOL config)
{
    AvdtpCallbackParms info;
    U16 rxLen = Parms->len;
    BOOL makeCallback;
    U16 n;
    U8 i;

    /* Store the receive buffer */
    Chnl->parser.rxBuff = Parms->data;

    /* Start setting callback parms */
    info.channel = Chnl;
    info.status = Parms->status;
    info.error = AVDTP_ERR_NO_ERROR;
    info.rawData = Parms->data;
    info.rawDataLen = Parms->len;

    /* Process the data in Chnl->parser.rxBuff of length rxLen */
    while (rxLen > 0)
    {
        //Assert(Chnl->parser.stageLen <= MAX_CAPABILITY_SIZE);
        Assert((Chnl->parser.curStageOff == 0) || (Chnl->parser.curStageOff < Chnl->parser.stageLen));

        n = min((U16) (Chnl->parser.stageLen - Chnl->parser.curStageOff), rxLen);
        Assert(n <= MAX_CAPABILITY_SIZE);

        /* Stage the data */
        for (i = Chnl->parser.curStageOff; n > 0; n--, i++)
        {
            Chnl->parser.stageBuff[i] = *(Chnl->parser.rxBuff);
            (Chnl->parser.rxBuff)++;
            rxLen--;
            Chnl->parser.curStageOff++;
        }

        /*
         * Only call the state machine if the data has been completely
         * * staged.
         */
        if (Chnl->parser.curStageOff == Chnl->parser.stageLen)
        {
            switch (Chnl->parser.rxState)
            {
                case 0: /* Service Category and length */
                    Chnl->parser.stageLen = Chnl->parser.stageBuff[1];
                    Chnl->capability.type = Chnl->parser.stageBuff[0];
                    Chnl->parser.rxState++;
                    break;
                case 1: /* Elements */
                    switch (Chnl->capability.type)
                    {
                        case AVDTP_SRV_CAT_MEDIA_CODEC:
                            Chnl->capability.p.codec.codecType = Chnl->parser.stageBuff[1];
                            Chnl->capability.p.codec.elemLen = Chnl->parser.stageLen - 2;
                            Chnl->capability.p.codec.elements = &Chnl->parser.stageBuff[2];
                            makeCallback = TRUE;
                            break;
                        default:
                            /* This capability is transport specific */
                            makeCallback = FALSE;
                            break;
                    }

                    Chnl->parser.rxState = 0;
                    Chnl->parser.stageLen = 2;

                    if (makeCallback)
                    {
                        /* Set up the callback parms */
                        info.p.capability = &Chnl->capability;
                        info.len = sizeof(AvdtpCapability);
                        if (config)
                        {
                            info.event = AVDTP_EVENT_CFG_CAPABILITY;
                        }
                        else
                        {
                            info.event = AVDTP_EVENT_CAPABILITY;
                        }
                        Chnl->callback(Chnl, &info);
                    }
                    break;
            }

            Chnl->parser.curStageOff = 0;
        }
    }
#ifdef __AVDTP_TESTER_
    info.rawData = Parms->data;
    info.rawDataLen = Parms->len;
    if (config)
    {
        info.event = AVDTP_EVENT_GET_CFG_TS_CNF;
    }
    else
    {
        info.event = AVDTP_EVENT_GET_CAP_TS_CNF;
    }
    info.len = 0;
    Chnl->callback(Chnl, &info);

#endif /* __AVDTP_TESTER_ */ 
    if (Parms->packetsLeft == 0)
    {
        /* No more capabilities, make the Get Capabilities complete callback */
        if (config)
        {
            info.event = AVDTP_EVENT_GET_CFG_CNF;
        }
        else
        {
            info.event = AVDTP_EVENT_GET_CAP_CNF;
        }
        info.len = 0;
        Chnl->callback(Chnl, &info);
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigParseConfigCmd
 * DESCRIPTION
 *  Parse config parameters from the signal channel.
 * PARAMETERS
 *  Chnl            [IN]        
 *  Stream          [IN]        
 *  Parms           [IN]        
 *  reconfig        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BOOL AvdtpSigParseConfigCmd(AvdtpChannel *Chnl, AvdtpStream *Stream, AvtpCallbackParms *Parms, BOOL reconfig)
{
    AvdtpCallbackParms info;
    U16 rxLen = Parms->len;
    U16 n;
    U8 i;

    /* Store the receive buffer */
    Chnl->parser.rxBuff = Parms->data;

    /* Start setting callback parms */
    info.channel = Chnl;
    info.stream = Stream;
    info.error = AVDTP_ERR_NO_ERROR;
    info.status = Parms->status;

    /* Initialize the config request */
    Stream->cfgReq.cp.cpType = 0;
    Stream->cfgReq.cp.dataLen = 0;

    /* Process the data in Chnl->parser.rxBuff of length rxLen */
    while (rxLen > 0)
    {
        //Assert(Chnl->parser.stageLen <= MAX_CAPABILITY_SIZE);
        Assert((Chnl->parser.curStageOff == 0) || (Chnl->parser.curStageOff < Chnl->parser.stageLen));

        n = min((U16) (Chnl->parser.stageLen - Chnl->parser.curStageOff), rxLen);
        Assert(n <= MAX_CAPABILITY_SIZE);

        /* Stage the data */
        for (i = Chnl->parser.curStageOff; n > 0; n--, i++)
        {
            Chnl->parser.stageBuff[i] = *(Chnl->parser.rxBuff);
            (Chnl->parser.rxBuff)++;
            rxLen--;
            Chnl->parser.curStageOff++;
        }

        /*
         * Only call the state machine if the data has been completely
         * staged.
         */
        if (Chnl->parser.curStageOff == Chnl->parser.stageLen)
        {
            switch (Chnl->parser.rxState)
            {
                case 0: /* Stream ID's */
                    if (!reconfig)
                    {
                        Stream->remStrmId = Chnl->parser.stageBuff[1] >> 2;
                    }
                    else
                    {
                        Chnl->parser.stageLen = 2;
                    }
                    Chnl->parser.rxState++;
                    break;
                case 1: /* Service Category and length */
                    Chnl->parser.stageLen = Chnl->parser.stageBuff[1];
                    Chnl->capability.type = Chnl->parser.stageBuff[0];
                    Chnl->parser.rxState++;
                    if (Chnl->parser.stageLen != 0)
                    {
                        break;
                    }
                    /* Fall Through */
                case 2: /* Elements */
                    switch (Chnl->capability.type)
                    {
                        case AVDTP_SRV_CAT_MEDIA_TRANSPORT:
                            /* This is supported */
                            if (reconfig)
                            {
                                /* Cannot reconfig transport */
                                Chnl->rspSigPacket->txData[0] = Chnl->capability.type;
                                Chnl->rspSigPacket->txData[1] = AVDTP_ERR_INVALID_CAPABILITIES;
                                Chnl->rspSigPacket->txDataLen = 2;

                                /* Abort parsing */
                                Chnl->parser.curStageOff = 0;
                                Chnl->parser.rxState = 0;
                                return FALSE;
                            }
                            else if (Chnl->parser.stageLen)
                            {
                                /* Invalid - Reject */
                                Chnl->rspSigPacket->txData[0] = Chnl->capability.type;
                                Chnl->rspSigPacket->txData[1] = AVDTP_ERR_BAD_MEDIA_TRANSPORT_FORMAT;
                                Chnl->rspSigPacket->txDataLen = 2;

                                /* Abort parsing */
                                Chnl->parser.curStageOff = 0;
                                Chnl->parser.rxState = 0;
                                return FALSE;
                            }
                            break;
                        case AVDTP_SRV_CAT_MEDIA_CODEC:
                            /*
                             * Media Codec is supported, verify codec type and 
                             * * call the application to validate codec parameters.
                             */
                            if (Stream->codec->codecType == Chnl->parser.stageBuff[1])
                            {
                                Stream->cfgReq.codec.codecType = Chnl->parser.stageBuff[1];
                                Stream->cfgReq.codec.elemLen = min(Chnl->parser.stageLen - 2, AVDTP_MAX_CODEC_ELEM_SIZE);
                                OS_MemCopy(
                                    (U8*) Stream->cfgReq.codec.elements,
                                    (U8*) & Chnl->parser.stageBuff[2],
                                    Stream->cfgReq.codec.elemLen);
                            }
                            else
                            {
                                /* Invalid codec - Reject */
                                Chnl->rspSigPacket->txData[0] = Chnl->capability.type;
                                Chnl->rspSigPacket->txData[1] = AVDTP_ERR_NOT_SUPPORTED_CODEC_TYPE;
                                Chnl->rspSigPacket->txDataLen = 2;

                                /* Abort parsing */
                                Chnl->parser.curStageOff = 0;
                                Chnl->parser.rxState = 0;
                                return FALSE;
                            }
                            break;
                        case AVDTP_SRV_CAT_CONTENT_PROTECTION:
                        case AVDTP_SRV_CAT_REPORTING:
                        case AVDTP_SRV_CAT_RECOVERY:
                        case AVDTP_SRV_CAT_HEADER_COMPRESSION:
                        case AVDTP_SRV_CAT_MULTIPLEXING:
                            /* Error, unsupported capability */
                            Chnl->rspSigPacket->txData[0] = Chnl->capability.type;
                            Chnl->rspSigPacket->txData[1] = AVDTP_ERR_UNSUPPORTED_CONFIGURATION;
                            Chnl->rspSigPacket->txDataLen = 2;

                            /* Abort parsing */
                            Chnl->parser.curStageOff = 0;
                            Chnl->parser.rxState = 0;
                            return FALSE;
                        default:
                            /* Error, unknown capability */
                            Chnl->rspSigPacket->txData[0] = Chnl->capability.type;
                            Chnl->rspSigPacket->txData[1] = AVDTP_ERR_BAD_SERV_CATEGORY;
                            Chnl->rspSigPacket->txDataLen = 2;

                            /* Abort parsing */
                            Chnl->parser.curStageOff = 0;
                            Chnl->parser.rxState = 0;
                            return FALSE;
                    }

                    Chnl->parser.rxState = 1;
                    Chnl->parser.stageLen = 2;
                    break;
            }

            Chnl->parser.curStageOff = 0;
        }
    }

    if (Parms->packetsLeft == 0)
    {

        if (Chnl->parser.rxState != 1)
        {
            /* Bad Length in a capability */
            Chnl->rspSigPacket->txData[0] = Chnl->capability.type;
            if (Chnl->capability.type == AVDTP_SRV_CAT_MEDIA_TRANSPORT)
            {
                Chnl->rspSigPacket->txData[1] = AVDTP_ERR_BAD_MEDIA_TRANSPORT_FORMAT;
            }
            else
            {
                Chnl->rspSigPacket->txData[1] = AVDTP_ERR_BAD_LENGTH;
            }
            Chnl->rspSigPacket->txDataLen = 2;

            /* Abort parsing */
            Chnl->parser.curStageOff = 0;
            Chnl->parser.rxState = 0;
            return FALSE;
        }

        Chnl->parser.rxState = 0;
        if (reconfig)
        {
            /* No more capabilities, make the Config callback */
            info.event = AVDTP_EVENT_STREAM_RECONFIG_IND;
        }
        else
        {
            /* Change state and register the stream on the channel */

            /* info.event = AVDTP_EVENT_STREAM_OPEN_IND; */
            info.event = AVDTP_EVENT_SET_CFG_IND;
            OS_Report("[AvdtpSigParseConfigCmd]insert stream to channl stream list");	
            Stream->chnl = Chnl;
            RemoveEntryList(&Stream->node);
            InsertTailList(&Chnl->streamList, &Stream->node);
        }
        info.rawData = Parms->data;
        info.rawDataLen = Parms->len;
        info.p.configReq = &Stream->cfgReq;
        info.len = sizeof(AvdtpConfigRequest);
        Chnl->callback(Chnl, &info);
    }

    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigParseStreamStart
 * DESCRIPTION
 *  Parse the Stream Start command.
 * PARAMETERS
 *  Chnl        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BOOL AvdtpSigParseStreamStart(AvdtpChannel *Chnl, AvtpCallbackParms *Parms)
{
    AvdtpCallbackParms info;
    U16 rxLen = Parms->len;
    U8 i = 0;

    /* Store the receive buffer */
    Chnl->parser.rxBuff = Parms->data;
    Chnl->streamsLeft = 0;
    Chnl->rspSigPacket->msgType = 0;
    Chnl->streamsLeft = Parms->len;

    /* Process the data in Chnl->parser.rxBuff of length rxLen */
    while (rxLen-- > 0)
    {

        info.status = Parms->status;
        info.error = AVDTP_ERR_NO_ERROR;
        info.event = AVDTP_EVENT_STREAM_START_IND;
        info.channel = Chnl;

        info.stream = AvdtpSigFindStream(Chnl, (AvdtpStreamId) (Parms->data[i++] >> 2));
        if (!info.stream)
        {
            /* UPF: Fail here for BI-20-C */
            Chnl->rspSigPacket->txData[0] = Parms->data[i - 1];
            Chnl->rspSigPacket->txData[1] = AVDTP_ERR_BAD_ACP_SEID;
            Chnl->rspSigPacket->txDataLen = 2;
            Chnl->streamsLeft = 0;
            return FALSE;
        }
        else if (info.stream->state != AVDTP_STRM_STATE_OPEN)
        {
            Chnl->rspSigPacket->txData[0] = Parms->data[i - 1];
            Chnl->rspSigPacket->txData[1] = AVDTP_ERR_BAD_STATE;
            Chnl->rspSigPacket->txDataLen = 2;
            Chnl->streamsLeft = 0;
            return FALSE;
        }

        /* Make the stream info callback */
        info.p.data = Parms->data;
        info.len = Parms->len;
        Chnl->callback(Chnl, &info);
    }

    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigParseStreamSuspend
 * DESCRIPTION
 *  Parse the Stream Suspend command.
 * PARAMETERS
 *  Chnl        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BOOL AvdtpSigParseStreamSuspend(AvdtpChannel *Chnl, AvtpCallbackParms *Parms)
{
    AvdtpCallbackParms info;
    U16 rxLen = Parms->len;
    U8 i = 0;

    /* Store the receive buffer */
    Chnl->parser.rxBuff = Parms->data;

    /* Process the data in Chnl->parser.rxBuff of length rxLen */
    while (rxLen-- > 0)
    {

        info.stream = AvdtpSigFindStream(Chnl, (AvdtpStreamId) (Parms->data[i++] >> 2));
        if (!info.stream)
        {
            /* UPF: Fail here for BI-20-C */
            Chnl->rspSigPacket->txData[0] = Parms->data[i - 1];
            Chnl->rspSigPacket->txData[1] = AVDTP_ERR_BAD_ACP_SEID;
            Chnl->rspSigPacket->txDataLen = 2;
            return FALSE;
        }
        else if (info.stream->state != AVDTP_STRM_STATE_STREAMING)
        {
            Chnl->rspSigPacket->txData[0] = Parms->data[i - 1];
            Chnl->rspSigPacket->txData[1] = AVDTP_ERR_BAD_STATE;
            Chnl->rspSigPacket->txDataLen = 2;
            return FALSE;
        }

        info.channel = Chnl;
        info.event = AVDTP_EVENT_STREAM_SUSPEND_IND;
        info.error = AVDTP_ERR_NO_ERROR;
        info.len = Parms->len;
        info.p.data = Parms->data;
        Chnl->callback(Chnl, &info);
    }

    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpCloseStream
 * DESCRIPTION
 *  Close the stream.
 * PARAMETERS
 *  Stream      [IN]        
 *  Error       [IN]        
 *  Cap         [IN]        
 *  Len         [IN]        
 *  Status      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvdtpCloseStream(AvdtpStream *Stream, AvdtpError Error, AvdtpCapability *Cap, U16 Len, BtStatus Status)
{
    AvdtpCallbackParms info;

    /* Reset Stream State */
    Stream->state = AVDTP_STRM_STATE_IDLE;
    OS_MemSet((U8*) & Stream->cpCfg, 0, sizeof(AvdtpContentProt));
    OS_MemSet((U8*) & Stream->codecCfg, 0, sizeof(AvdtpCodec));
    Stream->cpCfg.data = Stream->cpCfgValue;
    Stream->codecCfg.elements = Stream->codecCfgElem;
    OS_MemSet((U8*) & Stream->cfgReq, 0, sizeof(AvdtpConfigRequest));
    Stream->cfgReq.cp.data = Stream->cpReqValue;
    Stream->cfgReq.codec.elements = Stream->codecReqElem;
    Stream->abError = AVDTP_ERR_NO_ERROR;

    /* Indicate closed event to the application */
    info.channel = Stream->chnl;
    info.event = AVDTP_EVENT_STREAM_CLOSED;
    info.status = Status;
    info.error = Error;
    info.stream = Stream;
    info.p.capability = Cap;
    info.len = Len;

    if (Stream->chnl)
    {
        Stream->chnl->callback(Stream->chnl, &info);
        Stream->chnl = 0;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigHandleCommand
 * DESCRIPTION
 *  Handle a signal command.
 * PARAMETERS
 *  Chnl        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpSigHandleCommand(AvdtpChannel *Chnl, AvtpCallbackParms *Parms)
{
    AvdtpStream *strm = 0;
    U8 msgType = AVTP_MSG_TYPE_REJECT;
    BOOL respond = TRUE;
    AvdtpCallbackParms info;
    BtStatus status;
    AvtpPacket *rspPacket;

    if (IsListEmpty(&AVDTP(txPacketList)))
    {
        /* No packet resource available, the command will be ignored */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_NO_RESOURCE_AVAILABLE);
        return;
    }
    else
    {
        rspPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
        Chnl->rspSigPacket = rspPacket;
    }
    Chnl->rxAbortCmd = AVDTP_NOT_RX_ABORT_CMD;
    if (Chnl->txState == AVTP_SIG_MGR_TXSTATE_WAIT_RSP)
    {
        if (Parms->rxId == 0)
        {
            /* Received a General Reject */
            Chnl->txState = AVTP_SIG_MGR_TXSTATE_IDLE;
            respond = FALSE;
            goto error_send;
        }
        else
        {
            switch (Chnl->cmdSigPacket->txId)
            {
                case AVDTP_SIG_OPEN:
                case AVDTP_SIG_CLOSE:
                    respond = TRUE;
                    rspPacket->txData[0] = AVDTP_ERR_BAD_STATE;
                    goto error_send;
                    break;
                case AVDTP_SIG_SET_CONFIG:
            #ifdef BT_AVDTP_V12
                    respond = TRUE;
                    rspPacket->txData[0] = 0;
                    rspPacket->txData[1] = AVDTP_ERR_BAD_STATE;
                    goto error_send;
                    break;
            #endif /* BT_AVDTP_V12 */ 
                case AVDTP_SIG_RECONFIG:
                case AVDTP_SIG_SUSPEND:
                case AVDTP_SIG_START:
                    respond = TRUE;
                    rspPacket->txData[0] = Parms->data[0];
                    rspPacket->txData[1] = AVDTP_ERR_BAD_STATE;
                    goto error_send;
                    break;

                default:
                    break;
            }
        }
    }
    if (Parms->rxId == AVDTP_SIG_DISCOVER)
    {
        strm = (AvdtpStream*) GetHeadList(&AVDTP(streamList));
    }
    else
    {
        if (Chnl->rxState == AVTP_SIG_MGR_RXSTATE_BEGIN)
        {
            strm = AvdtpSigFindStream(Chnl, (U8) (Parms->data[0] >> 2));
        #ifdef BT_AVDTP_V12
            if ((strm == 0) && (Parms->rxId == AVDTP_SIG_ABORT))
            {
                bt_trace(TRACE_GROUP_1, AVDTP_RX_ABORT_CMD_WITH_NO_VALID_STREAM_IDxD, Parms->data[0] >> 2);
                respond = FALSE;
                goto error_send;
            }
            else
        #endif /* BT_AVDTP_V12 */ 
                Chnl->currentRxStream = strm;
        }
        else
        {
            strm = Chnl->currentRxStream;
        }
    }

    info.status = Parms->status;
    bt_trace(TRACE_GROUP_1, AVDTP_HDL_CMDBT_DEBUGx02X, Parms->rxId);
    switch (Parms->rxId)
    {
        case AVDTP_SIG_DISCOVER:
            if (Parms->len != 0)
            {
                /* Reject the request (BAD LENGTH) */
                rspPacket->txData[0] = AVDTP_ERR_BAD_LENGTH;
                rspPacket->txDataLen = 1;
                break;
            }
            /* Return the registered (unused) streams */
            Assert(Parms->packetsLeft == 0);
            respond = FALSE;
            msgType = AVTP_MSG_TYPE_COMMAND;
            info.channel = Chnl;
            info.event = AVDTP_EVENT_DISCOVER_IND;
            info.error = AVDTP_ERR_NO_ERROR;
            info.len = 0;
            Chnl->callback(Chnl, &info);
            break;
        case AVDTP_SIG_GET_CAPABILITIES:
            if (Parms->len != 1)
            {
                /* Reject the request (BAD LENGTH) */
                rspPacket->txData[0] = AVDTP_ERR_BAD_LENGTH;
                rspPacket->txDataLen = 1;
                break;
            }
            /* Return the capabilities of the specified stream */
            Assert(Parms->packetsLeft == 0);
            msgType = AVTP_MSG_TYPE_COMMAND;
            info.channel = Chnl;
            info.event = AVDTP_EVENT_GET_CAP_IND;
            info.error = AVDTP_ERR_NO_ERROR;
            info.stream = strm;
            info.len = Parms->len;
            info.p.data = Parms->data;
            Chnl->callback(Chnl, &info);
            respond = FALSE;
            break;
        case AVDTP_SIG_GET_CONFIG:
            rspPacket->txDataLen = 1;
            if (strm)
            {
                if (Parms->len != 1)
                {
                    /* Reject the request (BAD LENGTH) */
                    rspPacket->txData[0] = AVDTP_ERR_BAD_LENGTH;
                    break;
                }

                if ((strm->state > AVDTP_STRM_STATE_IDLE) || (strm->state <= AVDTP_STRM_STATE_STREAMING))
                {
                    /* Return the current configuration */
                    /* Reject the request (BAD LENGTH) */
                    rspPacket->txData[0] = AVDTP_ERR_NOT_SUPPORTED_COMMAND;
                    break;
                }
                else
                {
                    /* Reject the request (BAD STATE) */
                    rspPacket->txData[0] = AVDTP_ERR_BAD_STATE;
                }
            }
            else
            {
                rspPacket->txData[0] = AVDTP_ERR_BAD_ACP_SEID;
            }
            break;

        case AVDTP_SIG_RECONFIG:    /* UPF: Fail here for SMG/BI-15-C */
            if (strm)
            {
                if (strm->state == AVDTP_STRM_STATE_OPEN)
                {
                    if (Chnl->rxState == AVTP_SIG_MGR_RXSTATE_BEGIN)
                    {
                        Chnl->parser.rxState = 0;
                        Chnl->parser.stageLen = 1;

                        /* Preset the request to existing configuration */
                        strm->cfgReq.codec.codecType = strm->codecCfg.codecType;
                        strm->cfgReq.codec.elemLen = strm->codecCfg.elemLen;
                        OS_MemCopy(
                            (U8*) strm->cfgReq.codec.elements,
                            (U8*) strm->codecCfg.elements,
                            strm->codecCfg.elemLen);
                        strm->cfgReq.cp.cpType = strm->cpCfg.cpType;
                        strm->cfgReq.cp.dataLen = strm->cpCfg.dataLen;
                        OS_MemCopy((U8*) strm->cfgReq.cp.data, (U8*) strm->cfgReq.cp.data, strm->cpCfg.dataLen);
                    }
                    if (AvdtpSigParseConfigCmd(Chnl, strm, Parms, TRUE))
                    {
                        respond = FALSE;
                    }
                }
                else
                {
                    /* Reject the request (BAD STATE) */
                    rspPacket->txData[0] = Parms->data[1];
                    rspPacket->txData[1] = AVDTP_ERR_BAD_STATE;
                    rspPacket->txDataLen = 2;
                }
            }
            else
            {
                /* Reject the request (BAD SEID) */
                rspPacket->txData[0] = Parms->data[1];
                rspPacket->txData[1] = AVDTP_ERR_NOT_IN_USE;
                rspPacket->txDataLen = 2;
            }
            break;
        case AVDTP_SIG_SET_CONFIG:  /* UPF: Fail here for BTR/BI-01-C */
            if (strm)
            {
                if (strm->state == AVDTP_STRM_STATE_IDLE)
                {
                    if (Chnl->rxState == AVTP_SIG_MGR_RXSTATE_BEGIN)
                    {
                        Chnl->parser.rxState = 0;
                        Chnl->parser.stageLen = 2;

                        /* Preset the request to existing configuration */
                        strm->cfgReq.codec.codecType = strm->codecCfg.codecType;
                        strm->cfgReq.codec.elemLen = strm->codecCfg.elemLen;
                        OS_MemCopy(
                            (U8*) strm->cfgReq.codec.elements,
                            (U8*) strm->codecCfg.elements,
                            strm->codecCfg.elemLen);
                        strm->cfgReq.cp.cpType = strm->cpCfg.cpType;
                        strm->cfgReq.cp.dataLen = strm->cpCfg.dataLen;
                        OS_MemCopy((U8*) strm->cfgReq.cp.data, (U8*) strm->cfgReq.cp.data, strm->cpCfg.dataLen);
                    }
                    if (AvdtpSigParseConfigCmd(Chnl, strm, Parms, FALSE))
                    {
                        respond = FALSE;
                    }
                }
                else
                {
                    /* Reject the request (IN USE) */
                    rspPacket->txData[0] = Parms->data[2];
                    rspPacket->txData[1] = AVDTP_ERR_IN_USE;
                    rspPacket->txDataLen = 2;
                }
            }
            else
            {
                /* Reject the request (BAD SEID) */
                rspPacket->txData[0] = Parms->data[2];
                rspPacket->txData[1] = AVDTP_ERR_BAD_ACP_SEID;
                rspPacket->txDataLen = 2;
            }
            break;
        case AVDTP_SIG_OPEN:    /* UPF: Fail here for SMG/BI-17-C */
            if (strm)
            {
                if (Parms->len != 1)
                {
                    /* Reject the request (BAD LENGTH) */
                    rspPacket->txData[0] = AVDTP_ERR_BAD_LENGTH;
                    rspPacket->txDataLen = 1;
                    break;
                }

                /* Change state and register the stream on the channel */
                if (strm->state == AVDTP_STRM_STATE_CONFIGURED)
                {
                    info.channel = Chnl;
                    info.event = AVDTP_EVENT_STREAM_OPEN_IND;
                    info.error = AVDTP_ERR_NO_ERROR;
                    info.stream = strm;
                    info.len = Parms->len;
                    info.p.data = Parms->data;
                    Chnl->callback(Chnl, &info);
                    respond = FALSE;
                }
                else
                {
                    /* Reject the request (BAD STATE) */
                    rspPacket->txData[0] = AVDTP_ERR_BAD_STATE;
                    rspPacket->txDataLen = 1;
                }
            }
            else
            {
                /* Reject the request (BAD SEID) */
                rspPacket->txData[0] = AVDTP_ERR_BAD_STATE;
                rspPacket->txDataLen = 1;
            }
            break;
        case AVDTP_SIG_START:
            if (AvdtpSigParseStreamStart(Chnl, Parms))
            {
#ifdef __BT_SINGLE_ANTENNA__
                BT_WMT_A2DP_Send_Status(Chnl->conn.remDev, 1);
#endif
                respond = FALSE;
            }
            break;
        case AVDTP_SIG_CLOSE:   /* UPF: Fail here for SMG/BI-23,24-C */
            if (strm)
            {
                if (Parms->len != 1)
                {
                    /* Reject the request (BAD LENGTH) */
                    rspPacket->txData[0] = AVDTP_ERR_BAD_LENGTH;
                    rspPacket->txDataLen = 1;
                    break;
                }

                if ((strm->state == AVDTP_STRM_STATE_OPEN) || (strm->state == AVDTP_STRM_STATE_STREAMING))
                {
                    info.channel = Chnl;
                    info.event = AVDTP_EVENT_STREAM_CLOSE_IND;
                    strm->state = AVDTP_STRM_STATE_CLOSING;
                    info.error = AVDTP_ERR_NO_ERROR;
                    info.stream = strm;
                    info.len = Parms->len;
                    info.p.data = Parms->data;
#ifdef __BT_SINGLE_ANTENNA__
                    BT_WMT_A2DP_Send_Status(Chnl->conn.remDev, 0);
#endif
                    Chnl->callback(Chnl, &info);
                    respond = FALSE;

                }
                else
                {
                    /* Reject the request (BAD STATE) */
                    rspPacket->txData[0] = AVDTP_ERR_BAD_STATE;
                    rspPacket->txDataLen = 1;
                }
            }
            else
            {
                rspPacket->txData[0] = AVDTP_ERR_BAD_ACP_SEID;
                rspPacket->txDataLen = 1;
            }
            break;
        case AVDTP_SIG_SUSPEND: /* UPF: Fail here for SMG/BI-26,27-C */
            if (AvdtpSigParseStreamSuspend(Chnl, Parms) == FALSE)
            {
                msgType = AVTP_MSG_TYPE_REJECT;
                Chnl->rspSigPacket->txData[0] = Parms->data[0];
                Chnl->rspSigPacket->txData[1] = AVDTP_ERR_BAD_STATE;
                Chnl->rspSigPacket->txDataLen = 2;
            }
            else
            {
                respond = FALSE;
#ifdef __BT_SINGLE_ANTENNA__
                BT_WMT_A2DP_Send_Status(Chnl->conn.remDev, 0);
#endif
                
            }
            break;
        case AVDTP_SIG_ABORT:
            if (strm)
            {
                if (Parms->len != 1)
                {
                    respond = FALSE;
                    break;
                }
                if (strm->state == AVDTP_STRM_STATE_IDLE)
                {
                    msgType = AVTP_MSG_TYPE_REJECT;
                    Chnl->rspSigPacket->txDataLen = 0;
                    respond = TRUE;
                }
                else
                {
                    if (strm->state == AVDTP_STRM_STATE_CONFIGURED)
                    {
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_REMOVE_INUSE_LIST);

                        strm->state = AVDTP_STRM_STATE_IDLE;						
                        /* Remove the stream from the in-use list */
                        RemoveEntryList(&strm->node);
                        AvdtpInsertStreamList(strm);						
                    }
                    if (Chnl->txState == AVTP_SIG_MGR_TXSTATE_WAIT_RSP)
                    {
                        Chnl->rxAbortCmd = AVDTP_RX_ABORT_CMD;
                        Chnl->rxCmdWhileAborted = (U8)Chnl->cmdSigPacket->txId;
                        Chnl->rxAbortAcked = 0x00;
                    }
                    msgType = AVTP_MSG_TYPE_ACCEPT;
                    info.channel = Chnl;
                    info.event = AVDTP_EVENT_STREAM_ABORT_IND;
                    info.error = AVDTP_ERR_NO_ERROR;
                    info.stream = strm;
                    info.len = Parms->len;
                    info.p.data = Parms->data;
                    Chnl->callback(Chnl, &info);
                    respond = FALSE;
#ifdef __BT_SINGLE_ANTENNA__
                    BT_WMT_A2DP_Send_Status(Chnl->conn.remDev, 0);
#endif

                }

                break;
            }
        case AVDTP_SIG_SECURITY_CTRL:
            rspPacket->txData[0] = AVDTP_ERR_NOT_SUPPORTED_COMMAND;
            rspPacket->txDataLen = 1;
            break;
        default:
            /* General Reject */
            msgType = AVTP_MSG_TYPE_COMMAND;
            rspPacket->txData[0] = 0;
            Parms->rxId = 0;
            break;
    }

  error_send:

    /* Send the response */
    if (respond)
    {
        rspPacket->msgType = msgType;
        rspPacket->useMsgHdr = FALSE;
        rspPacket->txIdSize = 1;
        rspPacket->txId = Parms->rxId;
        if ((msgType == AVTP_MSG_TYPE_REJECT) && (Parms->packetsLeft > 0))
        {
            /* Error ignore rest of packet */
            Chnl->rxState = AVTP_SIG_MGR_RXSTATE_IGNORE;
        }
        else
        {
            if (msgType != AVTP_MSG_TYPE_REJECT)
            {
                Chnl->currentTxStream = strm;
            }
            rspPacket->internal_used = 1;
            status = AVTP_SendStart(&Chnl->sigChnl, rspPacket);
            if (status != BT_STATUS_PENDING)
            {
                InsertTailList(&AVDTP(txPacketList), &rspPacket->node);
            }
        }
    }
    else
    {
        InsertTailList(&AVDTP(txPacketList), &rspPacket->node);
    }
}

/*---------------------------------------------------------------------------
 *            AvdtpSigHandleResponse()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handle a signal response.
 *
 */


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigHandleResponse
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Chnl        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpSigHandleResponse(AvdtpChannel *Chnl, AvtpCallbackParms *Parms)
{
    AvdtpCallbackParms info;
    BOOL stateChanged = TRUE;

    info.channel = Chnl;
    info.stream = Chnl->currentTxStream;
    info.len = 0;
    info.status = Parms->status;
    info.error = AVDTP_ERR_NO_ERROR;
    if ((Chnl->rxAbortCmd == AVDTP_RX_ABORT_CMD) && (Chnl->rxCmdWhileAborted == Parms->rxId))
    {
        info.error = AVDTP_ERR_RACE_CONDITION;
        Chnl->rxAbortAcked |= AVDTP_ABORT_CFM_PATH_RESPONDED;
        stateChanged = FALSE;
        if (Chnl->rxAbortAcked == (AVDTP_ABORT_RSP_PATH_RESPONDED | AVDTP_ABORT_CFM_PATH_RESPONDED))
        {
            Chnl->rxAbortCmd = AVDTP_NOT_RX_ABORT_CMD;
            Chnl->rxAbortAcked = 0;
        }
    }
    else if (Chnl->rxAbortCmd == AVDTP_RX_ABORT_CMD)
    {
        Chnl->rxAbortAcked = 0x00;
        Chnl->rxAbortCmd = AVDTP_NOT_RX_ABORT_CMD;
        Chnl->rxAbortAcked = 0;
    }
    bt_trace(TRACE_GROUP_1, AVDTP_HDL_RSPBT_DEBUGx02X, Parms->rxId);
    switch (Parms->rxId)
    {
        case AVDTP_SIG_DISCOVER:
            if (Chnl->rxState == AVTP_SIG_MGR_RXSTATE_BEGIN)
            {
                Chnl->parser.rxState = 0;
                Chnl->parser.stageLen = 2;
            }
            AvdtpSigParseDiscoveries(Chnl, Parms);
            break;
        case AVDTP_SIG_GET_CAPABILITIES:
            if (Chnl->rxState == AVTP_SIG_MGR_RXSTATE_BEGIN)
            {
                Chnl->parser.rxState = 0;
                Chnl->parser.stageLen = 2;
            }
            AvdtpSigParseCapabilityRsp(Chnl, Parms, FALSE);
            break;
        case AVDTP_SIG_SET_CONFIG:
            if (Parms->packetsLeft == 0)
            {
                /* Configuration was accepted, move it to configured variables */
                info.stream->cpCfg.cpType = info.stream->cfgReq.cp.cpType;
                info.stream->cpCfg.dataLen = info.stream->cfgReq.cp.dataLen;
                OS_MemCopy(
                    (U8*) info.stream->cpCfg.data,
                    (U8*) info.stream->cfgReq.cp.data,
                    info.stream->cfgReq.cp.dataLen);
                info.stream->codecCfg.codecType = info.stream->cfgReq.codec.codecType;
                info.stream->codecCfg.elemLen = info.stream->cfgReq.codec.elemLen;
                OS_MemCopy(
                    (U8*) info.stream->codecCfg.elements,
                    (U8*) info.stream->cfgReq.codec.elements,
                    info.stream->cfgReq.codec.elemLen);

                /* Set the state and make the callback */
                if (stateChanged)
                {
                    Chnl->currentTxStream->state = AVDTP_STRM_STATE_CONFIGURED;
                }
                RemoveEntryList(&Chnl->currentTxStream->node);
                InsertTailList(&Chnl->streamList, &Chnl->currentTxStream->node);
                info.event = AVDTP_EVENT_SET_CFG_CNF;
                info.p.configReq = &info.stream->cfgReq;
                info.len = sizeof(AvdtpConfigRequest);
                info.rawData = Parms->data;
                info.rawDataLen = Parms->len;
                Chnl->callback(Chnl, &info);
            }
            break;
        case AVDTP_SIG_GET_CONFIG:
            if (Chnl->rxState == AVTP_SIG_MGR_RXSTATE_BEGIN)
            {
                Chnl->parser.rxState = 0;
                Chnl->parser.stageLen = 2;
            }
            AvdtpSigParseCapabilityRsp(Chnl, Parms, TRUE);
            break;
        case AVDTP_SIG_RECONFIG:
            if (Parms->packetsLeft == 0)
            {

                /* Configuration was accepted, move it to configured variables */
                info.stream->cpCfg.cpType = info.stream->cfgReq.cp.cpType;
                info.stream->cpCfg.dataLen = info.stream->cfgReq.cp.dataLen;
                OS_MemCopy(
                    (U8*) info.stream->cpCfg.data,
                    (U8*) info.stream->cfgReq.cp.data,
                    info.stream->cfgReq.cp.dataLen);
                info.stream->codecCfg.codecType = info.stream->cfgReq.codec.codecType;
                info.stream->codecCfg.elemLen = info.stream->cfgReq.codec.elemLen;
                OS_MemCopy(
                    (U8*) info.stream->codecCfg.elements,
                    (U8*) info.stream->cfgReq.codec.elements,
                    info.stream->cfgReq.codec.elemLen);

                info.event = AVDTP_EVENT_STREAM_RECONFIG_CNF;
                info.p.configReq = &info.stream->cfgReq;
                info.len = sizeof(AvdtpConfigRequest);
                info.rawData = Parms->data;
                info.rawDataLen = 0;
                Chnl->callback(Chnl, &info);
            }
            break;
        case AVDTP_SIG_OPEN:
            /* Open the L2CAP channel for the stream */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SIGNAL_OPEN);
            Assert(Chnl->currentTxStream != 0);
            if (Chnl->currentTxStream->state == AVDTP_STRM_STATE_CONFIGURED)
            {
                if (AvdtpConnect(&Chnl->currentTxStream->conn, Chnl->conn.remDev) != BT_STATUS_PENDING)
                {
                    /* Could not start new L2CAP channel, abort */
                    Chnl->currentTxStream->abError = 0xFF;
                    (void)AvdtpSigStreamAbort(Chnl, Chnl->currentTxStream);
                }
                else
                {
                    if (stateChanged)
                    {
                        Chnl->currentTxStream->state = AVDTP_STRM_STATE_OPEN;
                    }
                }
            }
            else
            {
                AVDTP_AbortStream(Chnl->currentTxStream);
            }
            break;
        case AVDTP_SIG_CLOSE:
            Assert(Chnl->currentTxStream != 0);
            if (stateChanged)
            {
                Chnl->currentTxStream->state = AVDTP_STRM_STATE_CLOSING;
            }
            if (AvdtpDisconnect(&Chnl->currentTxStream->conn) != BT_STATUS_PENDING)
            {
                /* Could not close the stream L2CAP channel, disconnect channel */
                (void)AvdtpDisconnect(&Chnl->conn);
            }
            break;
        case AVDTP_SIG_START:
            info.event = AVDTP_EVENT_STREAM_STARTED;
            info.error = AVDTP_ERR_NO_ERROR;
            Assert(Chnl->currentTxStream != 0);
            bt_trace(TRACE_GROUP_1,BT_LOG_AVDTP_RECV_SIGRSP_CURRENT_STATE,AVDTP_SIG_START,info.stream->state,stateChanged);
            if (info.stream->state != AVDTP_STRM_STATE_IDLE)
            {
                if (stateChanged)
                {	
                    info.stream->state = AVDTP_STRM_STATE_STREAMING;
                }
                Chnl->callback(Chnl, &info);
            }
            else
            {
                bt_trace(TRACE_GROUP_1,BT_LOG_AVDTP_RECV_RSP_BUT_STREAM_ALREADY_DISC,AVDTP_SIG_START);
            }    
            break;
        case AVDTP_SIG_SUSPEND:
            info.event = AVDTP_EVENT_STREAM_SUSPENDED;
            info.error = AVDTP_ERR_NO_ERROR;
            Assert(Chnl->currentTxStream != 0);
            bt_trace(TRACE_GROUP_1,BT_LOG_AVDTP_RECV_SIGRSP_CURRENT_STATE,AVDTP_SIG_SUSPEND,info.stream->state,stateChanged);
            if (info.stream->state != AVDTP_STRM_STATE_IDLE)
            {	
                if (stateChanged)
                {
                    info.stream->state = AVDTP_STRM_STATE_OPEN;
                }
                Chnl->callback(Chnl, &info);
            }
            else
            {
                bt_trace(TRACE_GROUP_1,BT_LOG_AVDTP_RECV_RSP_BUT_STREAM_ALREADY_DISC,AVDTP_SIG_SUSPEND);                	
            }    
            break;
        case AVDTP_SIG_ABORT:
            Assert(Chnl->currentTxStream != 0);
            if ((info.stream->state > AVDTP_STRM_STATE_CONFIGURED) && (info.stream->state < AVDTP_STRM_STATE_ABORTING))
            {
                if (AvdtpDisconnect(&Chnl->currentTxStream->conn) != BT_STATUS_PENDING)
                {
                    /*
                     * Could not close the stream L2CAP channel, 
                     * * disconnect channel 
                     */
                    (void)AvdtpDisconnect(&Chnl->conn);
                }
                info.stream->state = AVDTP_STRM_STATE_ABORTING;
                info.event = AVDTP_EVENT_STREAM_ABORTED;
                Chnl->callback(Chnl, &info);
            }
            else
            {
                /* Close the stream */
                if (info.stream->state == AVDTP_STRM_STATE_CONFIGURED)
                {
                    RemoveEntryList(&info.stream->node);
                    AvdtpInsertStreamList(info.stream);
                }

                if (Chnl->currentTxStream->abError == 0xFF)
                {
                    Chnl->currentTxStream->abError = AVDTP_ERR_NO_ERROR;
                }

                info.event = AVDTP_EVENT_STREAM_ABORTED;
                Chnl->callback(Chnl, &info);
                if (info.stream->state == AVDTP_STRM_STATE_CONFIGURED)
                {
                    info.stream->state = AVDTP_STRM_STATE_IDLE;
                    OS_MemSet((U8*) & info.stream->cpCfg, 0, sizeof(AvdtpContentProt));
                    OS_MemSet((U8*) & info.stream->codecCfg, 0, sizeof(AvdtpCodec));
                    info.stream->cpCfg.data = info.stream->cpCfgValue;
                    info.stream->codecCfg.elements = info.stream->codecCfgElem;
                    OS_MemSet((U8*) & info.stream->cfgReq, 0, sizeof(AvdtpConfigRequest));
                    info.stream->cfgReq.cp.data = info.stream->cpReqValue;
                    info.stream->cfgReq.codec.elements = info.stream->codecReqElem;
                    info.stream->abError = AVDTP_ERR_NO_ERROR;
                }
            }
            break;
    }
}

/*---------------------------------------------------------------------------
 *            AvdtpSigHandleReject()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handle a signal reject.
 *
 */


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigHandleReject
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Chnl        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvdtpSigHandleReject(AvdtpChannel *Chnl, AvtpCallbackParms *Parms)
{
    AvdtpCallbackParms info;
    AvdtpCapability capability;

    info.channel = Chnl;
    info.stream = Chnl->currentTxStream;
    info.len = 0;
    info.status = Parms->status;
    info.error = Parms->data[0];
    if ((Chnl->rxAbortCmd == AVDTP_RX_ABORT_CMD) && (Chnl->rxCmdWhileAborted == Parms->rxId))
    {
        Chnl->rxAbortAcked = AVDTP_ABORT_CFM_PATH_RESPONDED;
    }
    /* If error code was not provided in response, backfill */
    if (!info.error)
    {
        info.error = AVDTP_ERR_NOT_SUPPORTED_COMMAND;
    }
    bt_trace(TRACE_GROUP_1, AVDTP_HDL_REJBT_DEBUGx02X, Parms->rxId);
    switch (Parms->rxId)
    {
        case AVDTP_SIG_DISCOVER:
            info.event = AVDTP_EVENT_DISCOVER_CNF;
            info.error = Parms->data[0];
            if (!info.error)
            {
                info.error = AVDTP_ERR_NOT_SUPPORTED_COMMAND;
            }
            break;
        case AVDTP_SIG_GET_CAPABILITIES:
            info.event = AVDTP_EVENT_GET_CAP_CNF;
            info.error = Parms->data[0];
            if (!info.error)
            {
                info.error = AVDTP_ERR_NOT_SUPPORTED_COMMAND;
            }
            break;
        case AVDTP_SIG_SET_CONFIG:
            capability.type = Parms->data[0];
            info.event = AVDTP_EVENT_SET_CFG_CNF;
            info.error = Parms->data[1];
            info.rawData = Parms->data;
            info.rawDataLen = Parms->len;
            if (!info.error)
            {
                info.error = AVDTP_ERR_NOT_SUPPORTED_COMMAND;
            }
            info.p.capability = &capability;
            info.len = sizeof(AvdtpCapability);
            break;
        case AVDTP_SIG_GET_CONFIG:
            info.event = AVDTP_EVENT_GET_CFG_CNF;
            info.error = Parms->data[0];
            info.rawData = NULL;
            info.rawDataLen = 0;
            if (!info.error)
            {
                info.error = AVDTP_ERR_NOT_SUPPORTED_COMMAND;
            }
            break;
        case AVDTP_SIG_RECONFIG:
            capability.type = Parms->data[0];
            info.event = AVDTP_EVENT_STREAM_RECONFIG_CNF;
            info.error = Parms->data[1];
            if (!info.error)
            {
                info.error = AVDTP_ERR_NOT_SUPPORTED_COMMAND;
            }
            info.p.capability = &capability;
            info.len = sizeof(AvdtpCapability);
            info.rawData = Parms->data;
            info.rawDataLen = 2;
            break;
        case AVDTP_SIG_OPEN:
            /* Abort the stream, so we don't stay in the 'configured' state */
            info.event = AVDTP_EVENT_STREAM_OPEN;
            info.error = Parms->data[0];
            if (!info.error)
            {
                info.error = AVDTP_ERR_NOT_SUPPORTED_COMMAND;
            }
            break;
        case AVDTP_SIG_START:
            info.error = Parms->data[1];
            if (!info.error)
            {
                info.error = AVDTP_ERR_NOT_SUPPORTED_COMMAND;
            }
            info.event = AVDTP_EVENT_STREAM_STARTED;
            break;
        case AVDTP_SIG_CLOSE:
            info.error = Parms->data[0];
            if (!info.error)
            {
                info.error = AVDTP_ERR_NOT_SUPPORTED_COMMAND;
            }
            info.event = AVDTP_EVENT_STREAM_CLOSED;
            break;
        case AVDTP_SIG_SUSPEND:
            info.event = AVDTP_EVENT_STREAM_SUSPENDED;
            info.error = Parms->data[1];
            if (!info.error)
            {
                info.error = AVDTP_ERR_NOT_SUPPORTED_COMMAND;
            }
            break;
        case AVDTP_SIG_ABORT:
            info.event = AVDTP_EVENT_STREAM_ABORTED;
            break;
        case AVDTP_SIG_SECURITY_CTRL:
            info.event = AVDTP_EVENT_STREAM_SECURITY_CNF;
            info.error = Parms->data[0];
            if (!info.error)
            {
                info.error = AVDTP_ERR_NOT_SUPPORTED_COMMAND;
            }
            break;
    }

    Chnl->callback(Chnl, &info);
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigHandleReserved
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Chnl        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvdtpSigHandleReserved(AvdtpChannel *Chnl, AvtpCallbackParms *Parms)
{
    AvdtpStream *strm = 0;
    U8 msgType = AVTP_MSG_TYPE_REJECT;
    BOOL respond = TRUE;
    BtStatus status;
    AvtpPacket *rspPacket;

    if ((Chnl->rxAbortCmd == AVDTP_RX_ABORT_CMD) && (Chnl->rxCmdWhileAborted == Parms->rxId))
    {
        Chnl->rxAbortAcked = AVDTP_ABORT_CFM_PATH_RESPONDED;
    }
    if (IsListEmpty(&AVDTP(txPacketList)))
    {
        /* No packet resource available, the command will be ignored */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_NO_RESOURCE_AVAILABLE);
        return;
    }
    else
    {
        rspPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
        Chnl->rspSigPacket = rspPacket;
    }
    if (Chnl->txState == AVTP_SIG_MGR_TXSTATE_WAIT_RSP)
    {
        if (Parms->rxId == 0)
        {
            /* Received a General Reject */
            Chnl->txState = AVTP_SIG_MGR_TXSTATE_IDLE;
            respond = FALSE;
            InsertTailList(&AVDTP(txPacketList), &rspPacket->node);
            return;
        }
    }

    if (Parms->rxId == AVDTP_SIG_DISCOVER)
    {
        strm = (AvdtpStream*) GetHeadList(&AVDTP(streamList));
    }
    else
    {
        if (Chnl->rxState == AVTP_SIG_MGR_RXSTATE_BEGIN)
        {
            strm = AvdtpSigFindStream(Chnl, (U8) (Parms->data[0] >> 2));
            Chnl->currentRxStream = strm;
        }
        else
        {
            strm = Chnl->currentRxStream;
        }
    }

    rspPacket->txData[0] = AVDTP_ERR_BAD_HEADER_FORMAT;
    rspPacket->txDataLen = 1;
    /* Send the response */
    rspPacket->msgType = msgType;
    rspPacket->useMsgHdr = FALSE;
    rspPacket->txIdSize = 1;
    rspPacket->internal_used = 0;
    rspPacket->txId = Parms->rxId;
    if ((msgType == AVTP_MSG_TYPE_REJECT) && (Parms->packetsLeft > 0))
    {
        /* Error ignore rest of packet */
        Chnl->rxState = AVTP_SIG_MGR_RXSTATE_IGNORE;
    }
    else
    {
        Chnl->currentTxStream = strm;
        status = AVTP_SendStart(&Chnl->sigChnl, rspPacket);
        if (status != BT_STATUS_PENDING)
        {
            InsertTailList(&AVDTP(txPacketList), &rspPacket->node);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigMgrSignalCallback
 * DESCRIPTION
 *  Signal channel callback.
 * PARAMETERS
 *  SigChnl     [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpSigMgrSignalCallback(AvtpChannel *SigChnl, AvtpCallbackParms *Parms)
{
    AvdtpChannel *chnl = ContainingRecord(SigChnl, AvdtpChannel, sigChnl);
    AvdtpCallbackParms info;
    BtStatus status;

    info.status = Parms->status;

    switch (Parms->event)
    {
        case AVTP_EVENT_TX_DONE:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SIGNAL_TX_DONE);
            /* Free the packet */
            InsertTailList(&AVDTP(txPacketList), &Parms->packet->node);

            /* Handle the event */
            info.channel = chnl;
            info.stream = chnl->currentTxStream;
            info.len = 0;
            info.error = AVDTP_ERR_NO_ERROR;
            if ((chnl->currentTxStream) && (Parms->packet->msgType != AVTP_MSG_TYPE_COMMAND))
            {
                switch (Parms->packet->txId)
                {
                    case AVDTP_SIG_START:
                        info.event = AVDTP_EVENT_STREAM_STARTED;
                        if (Parms->packet->msgType == AVTP_MSG_TYPE_ACCEPT)
                        {
                            info.stream->state = AVDTP_STRM_STATE_STREAMING;
                            info.error = AVDTP_ERR_NO_ERROR;
                        }
                        else if (Parms->packet->msgType == AVTP_MSG_TYPE_REJECT)
                        {
                            info.error = AVDTP_ERR_REJECTED_BY_LOCAL;
                            break;
                        }
                        if (Parms->packet->internal_used == 0)
                        {
                            chnl->callback(chnl, &info);
                        }
                        else
                        {
                            Parms->packet->internal_used = 0;
                        }
                        break;
                    case AVDTP_SIG_SUSPEND:
                        info.event = AVDTP_EVENT_STREAM_SUSPENDED;
                        if (Parms->packet->msgType == AVTP_MSG_TYPE_ACCEPT)
                        {
                            info.stream->state = AVDTP_STRM_STATE_OPEN;
                            info.error = AVDTP_ERR_NO_ERROR;
                        }
                        else if (Parms->packet->msgType == AVTP_MSG_TYPE_REJECT)
                        {
                            info.error = AVDTP_ERR_REJECTED_BY_LOCAL;
                        }
                        if (Parms->packet->internal_used == 0)
                        {
                            chnl->callback(chnl, &info);
                        }
                        else
                        {
                            Parms->packet->internal_used = 0;
                        }
                        break;
                    case AVDTP_SIG_CLOSE:
                        info.event = AVDTP_EVENT_STREAM_CLOSED;
                        if (Parms->packet->msgType == AVTP_MSG_TYPE_ACCEPT)
                        {
                            info.stream->state = AVDTP_STRM_STATE_IDLE;
                            info.error = AVDTP_ERR_NO_ERROR;
                        }
                        else if (Parms->packet->msgType == AVTP_MSG_TYPE_REJECT)
                        {
                            info.error = AVDTP_ERR_REJECTED_BY_LOCAL;
                        }
                        if (Parms->packet->internal_used == 0)
                        {
                            chnl->callback(chnl, &info);
                        }
                        else
                        {
                            Parms->packet->internal_used = 0;
                        }
                        break;
                    case AVDTP_SIG_ABORT:
                        info.event = AVDTP_EVENT_STREAM_ABORTED;
                        if (Parms->packet->msgType == AVTP_MSG_TYPE_ACCEPT)
                        {
                            info.error = AVDTP_ERR_NO_ERROR;
                        }
                        else if (Parms->packet->msgType == AVTP_MSG_TYPE_REJECT)
                        {
                            info.error = AVDTP_ERR_REJECTED_BY_LOCAL;
                        }
                        chnl->callback(chnl, &info);
                        break;
                }
            }
            break;
        case AVTP_EVENT_TX_ERROR:
            bt_trace(TRACE_GROUP_1, AVTP_EVENT_TX_ERROR_TRC);
            /* Free the packet */
            InsertTailList(&AVDTP(txPacketList), &Parms->packet->node);
            Parms->packet->internal_used = 0;
        case AVTP_EVENT_TX_TIMEOUT:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SIGNAL_TX_ERROR);
            (void)AvdtpDisconnect(&chnl->conn);
            break;
        case AVTP_EVENT_RX_IND:
            if (chnl->rxState != AVTP_SIG_MGR_RXSTATE_IGNORE)
            {
                switch (Parms->msgType)
                {
                    case AVTP_MSG_TYPE_COMMAND:
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SIGNAL_CMD_RECV);
                        AvdtpSigHandleCommand(chnl, Parms);
                        break;
                    case AVTP_MSG_TYPE_ACCEPT:
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SIGNAL_RSP_RECV);
                        if (Parms->packetsLeft == 0)
                        {
                            chnl->txState = AVTP_SIG_MGR_TXSTATE_IDLE;
                        }
                        AvdtpSigHandleResponse(chnl, Parms);
                        break;
                    case AVTP_MSG_TYPE_REJECT:
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SIGNAL_REJ_RECV);
                        if (Parms->packetsLeft == 0)
                        {
                            chnl->txState = AVTP_SIG_MGR_TXSTATE_IDLE;
                        }
                        AvdtpSigHandleReject(chnl, Parms);
                        break;
                    case AVTP_MSG_TYPE_RESERVED:
                        AvdtpSigHandleReserved(chnl, Parms);
                        break;
                }
                /* Set the receive state */
                if (Parms->packetsLeft > 0)
                {
                    chnl->rxState = AVTP_SIG_MGR_RXSTATE_CONTINUE;
                }
                else
                {
                    chnl->rxState = AVTP_SIG_MGR_RXSTATE_BEGIN;
                }
            }
            else if (Parms->packetsLeft == 0)
            {
                chnl->rxState = AVTP_SIG_MGR_RXSTATE_BEGIN;
                if (Parms->msgType == AVTP_MSG_TYPE_COMMAND)
                {
                    chnl->currentTxStream = chnl->currentRxStream;
                    status = AVTP_SendStart(&chnl->sigChnl, chnl->rspSigPacket);
                    if (status != BT_STATUS_PENDING)
                    {
                        InsertTailList(&AVDTP(txPacketList), &chnl->rspSigPacket->node);
                    }
                }
            }
            break;

        case AVTP_EVENT_RX_UNKNOWN_MESSAGE:
        case AVTP_EVENT_RX_BAD_TRANS_ID:
        case AVTP_EVENT_RX_UNKNOWN_PACKET:
        case AVTP_EVENT_RX_BUFFER_UNDERRUN:
            /*
             * Ignore unknown messages, transaction ID's, bad packets,
             * * and incomplete packets
             */
            break;

        case AVTP_EVENT_RX_BUFFER_OVERRUN:
            if (Parms->msgType == AVTP_MSG_TYPE_COMMAND)
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SIGNAL_CMD_LENGTH_ERROR);
                if (!IsListEmpty(&AVDTP(txPacketList)))
                {
                    chnl->rspSigPacket = (AvtpPacket*) RemoveHeadList(&AVDTP(txPacketList));
                    chnl->parser.rxState = 0;
                    chnl->parser.curStageOff = 0;
                    chnl->rxState = AVTP_SIG_MGR_RXSTATE_BEGIN;
                    chnl->rspSigPacket->txData[0] = AVDTP_ERR_BAD_LENGTH;
                    chnl->rspSigPacket->txDataLen = 1;
                    chnl->rspSigPacket->msgType = AVTP_MSG_TYPE_REJECT;
                    chnl->rspSigPacket->useMsgHdr = FALSE;
                    chnl->rspSigPacket->txIdSize = 1;
                    chnl->rspSigPacket->internal_used = 0;
                    chnl->rspSigPacket->txId = Parms->rxId;
                    chnl->currentTxStream = chnl->currentRxStream;
                    status = AVTP_SendStart(SigChnl, chnl->rspSigPacket);
                    if (status != BT_STATUS_PENDING)
                    {
                        InsertTailList(&AVDTP(txPacketList), &chnl->rspSigPacket->node);
                    }
                }
                break;
            }
            else
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SIGNAL_RSP_LENGTH_ERROR);
                (void)AvdtpDisconnect(&chnl->conn);
            }
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigMgrClearConnection
 * DESCRIPTION
 *  Connection callback.
 * PARAMETERS
 *  Chnl        [IN]        
 *  RemDev      [IN]        
 *  status      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpSigMgrClearConnection(AvdtpChannel *Chnl, BtRemoteDevice *RemDev, BtStatus status)
{
    AvdtpCallbackParms info;

    Chnl->parser.rxState = 0;
    Chnl->parser.curStageOff = 0;
    Chnl->rxState = AVTP_SIG_MGR_RXSTATE_BEGIN;
    Chnl->txState = AVTP_SIG_MGR_TXSTATE_IDLE;

    /* Indicate the disconnect to the application */
    info.event = AVDTP_EVENT_DISCONNECT;
    info.status = status;
    info.error = AVDTP_ERR_NO_ERROR;
    info.channel = Chnl;
    info.p.remDev = RemDev;
    info.len = sizeof(BtRemoteDevice);
    Chnl->callback(Chnl, &info);
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpSigMgrConnCallback
 * DESCRIPTION
 *  Connection callback.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void AvdtpSigMgrConnCallback(AvdtpConn *Conn, AvdtpConnCallbackParms *Parms)
{
    AvdtpChannel *chnl = ContainingRecord(Conn, AvdtpChannel, conn);
    AvdtpCallbackParms info;
    AvdtpStream *strm, *nextStrm;

    info.status = Parms->status;

    switch (Parms->event)
    {
        case AVDTP_CONN_EVENT_CONNECT:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SIGNAL_CONNECTED);
            AVTP_InitChannel(&chnl->sigChnl, chnl->conn.l2ChannelId, AvdtpSigMgrSignalCallback, AVDTP_RTX_SIG_TIMEOUT);

            /* Indicate the connection to the application */
            info.event = AVDTP_EVENT_CONNECT;
            info.error = AVDTP_ERR_NO_ERROR;
            info.channel = chnl;
            info.len = sizeof(BtRemoteDevice);
            info.p.remDev = Parms->ptrs.remDev;
            chnl->callback(chnl, &info);
            break;
        case AVDTP_CONN_EVENT_CONNECT_IND:
            /* Indicate the incoming connection to the application */
            info.event = AVDTP_EVENT_CONNECT_IND;
            info.error = AVDTP_ERR_NO_ERROR;
            info.channel = chnl;
            info.len = sizeof(BtRemoteDevice);
            info.p.remDev = Parms->ptrs.remDev;
            chnl->callback(chnl, &info);
            break;
        case AVDTP_CONN_EVENT_DISCONNECT:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_SIGNAL_DISCONNECTED);
            AVTP_DeinitChannel(&chnl->sigChnl);

            if (!IsListEmpty(&chnl->streamList))
            {
                /* First disconnect any streams that failed config */
                strm = (AvdtpStream*) GetHeadList(&chnl->streamList);
                while (strm != (AvdtpStream*) & chnl->streamList)
                {
                    nextStrm = (AvdtpStream*) GetNextNode(&strm->node);

                    OS_Report("[AvdtpSigMgrConnCallback]AVDTP_CONN_EVENT_DISCONNECT strm conn stat:%d",strm->conn.state);
	
                    if (!AvdtpIsConnected(&strm->conn))
                    {
                        RemoveEntryList(&strm->node);
                        AvdtpInsertStreamList(strm);
                        AvdtpCloseStream(strm, strm->abError, 0, 0, Parms->status);
                    }




                    if ( AVDTP_STATE_CONN_INCOMING == strm->conn.state )
                    {
                        strm->state = AVDTP_STRM_STATE_IDLE;
                        OS_MemSet((U8*) & strm->cpCfg, 0, sizeof(AvdtpContentProt));
                        OS_MemSet((U8*) & strm->codecCfg, 0, sizeof(AvdtpCodec));
                        strm->cpCfg.data = strm->cpCfgValue;
                        strm->codecCfg.elements = strm->codecCfgElem;
                        OS_MemSet((U8*) & strm->cfgReq, 0, sizeof(AvdtpConfigRequest));
                        strm->cfgReq.cp.data = strm->cpReqValue;
                        strm->cfgReq.codec.elements = strm->codecReqElem;
                        strm->abError = AVDTP_ERR_NO_ERROR;  
		    }

                    strm = nextStrm;
                }

                /* Now disconnect the rest */
                if (!IsListEmpty(&chnl->streamList))
                {
                    strm = (AvdtpStream*) GetHeadList(&chnl->streamList);
                    (void)AvdtpDisconnect(&strm->conn);
                    return;
                }
            }

            AvdtpSigMgrClearConnection(chnl, Parms->ptrs.remDev, Parms->status);
            break;
        case AVDTP_CONN_EVENT_DATA_SENT:
            strm = chnl->currentTxStream;
            AVTP_SendContinue(&chnl->sigChnl);
            break;
        case AVDTP_CONN_EVENT_DATA_IND:
            AVTP_Receive(&chnl->sigChnl, Parms->ptrs.data, Parms->dataLen, 1);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvdtpStrmConnCallback
 * DESCRIPTION
 *  Connection callback.
 * PARAMETERS
 *  Conn        [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvdtpStrmConnCallback(AvdtpConn *Conn, AvdtpConnCallbackParms *Parms)
{
    AvdtpStream *strm = ContainingRecord(Conn, AvdtpStream, conn);
    AvdtpChannel *chnl = strm->chnl;
    AvdtpCallbackParms info;

    info.status = Parms->status;

    switch (Parms->event)
    {
        case AVDTP_CONN_EVENT_CONNECT:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_STREAM_CONNECTED);
            /* Indicate the connection to the application */
            info.event = AVDTP_EVENT_STREAM_OPEN;
            info.error = AVDTP_ERR_NO_ERROR;
            info.channel = chnl;
            info.stream = strm;
            info.p.configReq = &strm->cfgReq;
            info.len = sizeof(AvdtpConfigRequest);
            chnl->callback(chnl, &info);
            break;
        case AVDTP_CONN_EVENT_CONNECT_IND:
            (void)AvdtpConnectRsp(Conn, TRUE);
            break;
        case AVDTP_CONN_EVENT_DISCONNECT:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_STREAM_DISCONNECTED);
            Assert(!IsListEmpty(&chnl->streamList));

            RemoveEntryList(&strm->node);
            AvdtpInsertStreamList(strm);

            /* Indicate closed event to the application */
            AvdtpCloseStream(strm, strm->abError, 0, 0, Parms->status);

            /*
             * If the signal channel is disconnected disconnect any
             * remaining streams.
             */
            if (!AvdtpIsConnected(&chnl->conn))
            {
                if (!IsListEmpty(&chnl->streamList))
                {
                    /* Disconnect the next stream */
                    strm = (AvdtpStream*) GetHeadList(&chnl->streamList);
                    (void)AvdtpDisconnect(&strm->conn);
                }
                else
                {
                    /*
                     * The signal channel was disconnected, indicate this to the
                     * application.
                     */
                    AvdtpSigMgrClearConnection(chnl, Parms->ptrs.remDev, Parms->status);
                }
            }
	     else
	     {
	         if ( DEVICE_JABRA_BT3030 == BTBMGetDeviceModel(&Parms->ptrs.remDev->bdAddr))
	         {
                     if (IsListEmpty(&chnl->streamList))
                     {
		           (void)AvdtpDisconnect(&chnl->conn);
                         AvdtpSigMgrClearConnection(chnl, Parms->ptrs.remDev, Parms->status);
                     }
	         }					 
	     }				   
            break;
        case AVDTP_CONN_EVENT_DATA_SENT:
            info.event = AVDTP_EVENT_STREAM_PACKET_SENT;
            info.error = AVDTP_ERR_NO_ERROR;
            info.channel = chnl;
            info.stream = strm;
            info.p.data = Parms->ptrs.packet->data;
            info.len = Parms->ptrs.packet->dataLen;
            info.status = Parms->status;
            InsertHeadList(&chnl->rtp_pkt_List, &(Parms->ptrs.packet->node));
            chnl->callback(chnl, &info);
            break;
        case AVDTP_CONN_EVENT_DATA_IND:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_DATA_RECEIVED, Parms->dataLen);
            info.event = AVDTP_EVENT_STREAM_DATA_IND;
            info.error = AVDTP_ERR_NO_ERROR;
            info.channel = chnl;
            info.stream = strm;
            info.p.data = Parms->ptrs.data;
            info.len = Parms->dataLen;
            chnl->callback(chnl, &info);
            break;
    }
}

