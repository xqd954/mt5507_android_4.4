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
 *     $Workfile:hfg_at.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains functions for transmitting AT responses.
 *             
 * Created:     February 11, 2005
 *
 * Copyright 2000-2005 Extended Systems, Inc.

 * Portions copyright 2005 iAnywhere Solutions, Inc.

 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,  
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
/*****************************************************************************
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * 08 08 2013 liang.cheng
 * [ALPS00931922] [MR2][BT]Source code check in
 * Easy Migration default profile check in.
 *
 * 08 08 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 10 05 2010 sh.lai
 * [ALPS00127207] [MTK BT]The audio cannot transfer to BT headset when disconnect/connect BT headset.
 * Fix CR ALPS00127207 : [MTK BT]The audio cannot transfer to BT headset when disconnect/connect BT headset.
 * [Cause] After respone of AT+CHLD? is sent the profile state is transfferred to connected and the SCO is allowed. But AP think the HFP is connected when the response of AT+CHLD? is sent to profile and request SCO immediatly. HFP will reject the SCO connection due to the state is still conencting.
 * [Solution] Change state into connected after the response of AT+CHLD? is sent to stack so that profile can accept the SCO connection request.
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 * 
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *  *
 ****************************************************************************/

#include "atp.h"
#include "sys/hfgalloc.h"
#include "btalloc.h"
#include "hfg.h"

#define __BT_FILE__ BT_FILE_HFG_AT
/*--------------------------------------------------------------------------- 
 * sendResults
 * 
 *     Pump the results to send through BT stack
 */ 
BtStatus sendResults(HfgChannel *Channel, AtResults *Results)
{
    BtStatus  status;
    AtStatus  atStatus;

    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_SEND_RESULT, Channel, Results, Results->type);

    /* Encode AT Results */
    Channel->atBuffer.buff = Channel->atTxData;
    Channel->atBuffer.readOffset = 0;
    Channel->atBuffer.writeOffset = 0;
    Channel->atBuffer.buffSize = HFG_TX_BUFFER_SIZE;

    atStatus = ME_Encode(&HFG(atContext), Results, &Channel->atBuffer);
    Channel->atBuffer.buff[Channel->atBuffer.writeOffset] = '\0';    
    if (atStatus != AT_STATUS_OK) 
    {
        kal_trace(TRACE_GROUP_5, BT_HFG_WARN_ME_ENCODE_FAIL, atStatus);     
        status = BT_STATUS_FAILED;
        goto exit;
    }

    /* Send the packet */
    Channel->bytesToSend = Channel->atBuffer.writeOffset;
    Channel->atTxPacket.dataLen = Channel->bytesToSend;
    Channel->atTxPacket.data = Channel->atTxData;
    status = AtSendRfPacket(Channel, &Channel->atTxPacket);
    if (status == BT_STATUS_PENDING) 
    {         
        DBG_PRINT_AT(("[HFG][AT] Set ongoing flag"));
        Results->flag |= AT_RESULT_FLAG_ONGOING;
        /* MMI will consider the negotiation is done and will start to create SCO link. So
             We have to change to SLC ASAP response from MMI is received or the following 
             SCO creation will be rejected.
        */
        HfgCheckNegotiateDone(Channel);
    }
    
exit:
    return status;
}

/*--------------------------------------------------------------------------- 
 * AtSendresponse
 * 
 *     Encodes and sends an AT Results.
 */ 
BtStatus AtSendResults(HfgChannel *Channel, AtResults *Results, BOOL bAutoOK)
{
    BtStatus  status = BT_STATUS_PENDING;

    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_AT_SEND_RESULT, Channel, Results, bAutoOK ? 1 : 0, Results->type);

    if (Channel->state < HFG_STATE_OPEN) 
    {
        kal_trace(TRACE_GROUP_5, BT_HFG_WARN_NO_CONNECTION, Channel);
        status = BT_STATUS_NO_CONNECTION;
        goto exit;
    }

    /* Clear flag */
    DBG_PRINT_AT(("[HFG][AT] Clear AT flag"));
    Results->flag = 0;

    /* Queue the AT Results when a response is outstanding */
    if (IsNodeOnList(&Channel->rspQueue, &Results->node)) 
    {
        kal_trace(TRACE_GROUP_5, BT_HFG_WARN_RESPONSE_ALREADY_QUEUED, Results);
        status = BT_STATUS_IN_USE;
        goto exit;
    }

    if(IsListEmpty(&Channel->rspQueue))
    {
        kal_trace(TRACE_GROUP_5, BT_HFG_INFO_RESPONSE_QUEUE_EMPTY);
        status = sendResults(Channel, Results);
        if(status != BT_STATUS_PENDING)
            goto exit;
    }

    InsertTailList(&Channel->rspQueue, &Results->node);
    if(bAutoOK)
    {
        DBG_PRINT_AT(("[HFG][AT] Auto OK response : Yes"));
        DBG_PRINT_AT(("[HFG][AT] Set auto OK flag"));
        Results->flag |= AT_RESULT_FLAG_AUTO_OK;
    }
    else
    {
        DBG_PRINT_AT(("[HFG][AT] Auto OK response : No"));
    }
    
 exit:
    return status;
}

/*--------------------------------------------------------------------------- 
 * AtSendRfPacket
 * 
 *     Sends a packet over RFCOMM.
 */ 
BtStatus AtSendRfPacket(HfgChannel *Channel, BtPacket *Packet)
{
    BtStatus status;

    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_SEND_RF_PACKET, Channel, Packet);   
    DBG_PRINT_AT(("[HFG][AT] Send AT: %s", (S8*)(Packet->data + 2)));       

#ifdef BT_HFG_UT_TEST
    status = hfg_adp_hfg_send_ut_rx_ind(Channel, Packet->data, Packet->dataLen);
#else
    if (Packet->dataLen > RF_FrameSize(Channel->rfChannel)) 
    {
        Packet->dataLen = RF_FrameSize(Channel->rfChannel);
    }
    status = RF_SendData(Channel->rfChannel, Packet);
#endif

    if (status == BT_STATUS_PENDING) 
    {
        Channel->bytesToSend -= Packet->dataLen;
    }
    else
    {
        /* Something wrong with RFCOMM channel, disconnect it. */
        kal_trace(TRACE_GROUP_5, BT_HFG_WARN_SEND_RF_PACKET_FAILED, status);
        HfgDisconnecting(Channel);
    }
    return status;
}

/*--------------------------------------------------------------------------- 
 * AtSendComplete
 * 
 *     Completes the response and sends a new one if queued.
 */ 
BtStatus AtSendComplete(HfgChannel *Channel, BtPacket *Packet)
{
    AtResults *atResults;
    BtStatus   status = BT_STATUS_SUCCESS;

    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_AT_SEND_COMPLETE, Channel, Packet);
    
    if (!IsListEmpty(&Channel->rspQueue)) 
    {
        kal_trace(TRACE_GROUP_5, BT_HFG_INFO_RESPONSE_QUEUE_NOT_EMPTY);
        atResults = (AtResults*)(Channel->rspQueue.Flink);
        status = AtSendResults(Channel, atResults, FALSE);
    }

    return status;
}

/*--------------------------------------------------------------------------- 
 * AtParseCommand
 * 
 *     Parses the result code.
 */ 
AtStatus AtParseCommand(HfgChannel *Channel, U8 *Input, 
                    U16 InputLen, AtCommands *Command)
{
    AtStatus   atStatus;

    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_PARSE_COMMAND, Channel, Input, InputLen, Command);

    atStatus = ME_Decode(&HFG(atContext), Command, &Channel->atRxBufDesc);    
    DBG_PRINT_AT(("[HFG][AT] Recv AT: %s", (S8*)(Channel->atRxBufDesc.buff)));    
    if (atStatus != AT_STATUS_OK && atStatus !=AT_STATUS_CONTINUE) 
    {
        /* Unsuppored command, indicate unknown AT data */
        kal_trace(TRACE_GROUP_5, BT_HFG_WARN_ME_DECODE_FAIL, atStatus);
        Command->type = AT_UNKNOWN;
    }

    if(atStatus != AT_STATUS_CONTINUE)
    {
        /* Reset pointer */
        Channel->atRxBufDesc.readOffset = 0;
        Channel->atRxBufDesc.writeOffset = 0;
    }
    else
    {
        kal_trace(TRACE_GROUP_5, BT_HFG_INFO_AT_DECODE_CONTINUE);
        Channel->atRxBufDesc.readOffset = Channel->atRxBufDesc.writeOffset;
    }

    return atStatus;
}
