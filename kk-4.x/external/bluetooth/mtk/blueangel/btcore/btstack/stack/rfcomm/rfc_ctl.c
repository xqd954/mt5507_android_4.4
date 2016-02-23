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
 *     $Workfile:rfc_ctl.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains the control channel code for the 
 *     RFCOMM protocol.
 *
 * Created:
 *     Jan 10, 2000
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "bt_common.h"
#include "btalloc.h"
#include "btconfig.h"

#ifdef __RFCOMM_TESTER_
extern BOOL RfcommTesterStateQueuy(void);
#endif 

#if RFCOMM_PROTOCOL == XA_ENABLED

/* Function prototypes */
static BtStatus RfSendNSC(U8 MuxId, U8 mType);
static BtStatus RfCtlParseParms(U8 MuxId, U8 *mType, U8 **parms, U16 *length);
static BtStatus RfCtlSendInternalPacket(BtPacket *packet, U8 MuxId);

/*****************************************************************************
 * FUNCTION
 *  RfCtlHandlePNReq
 * DESCRIPTION
 *  Handle port negotiation request.
 * PARAMETERS
 *  MuxId       [IN]        
 *  DlcId       [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfCtlHandlePNReq(U8 MuxId, U8 DlcId, U8 *Parms)
{
    RfChannel *channel;
    U16 maxFrameSize;

#if NUM_RF_SERVERS != 0
    U8 serverId;
#endif /* NUM_RF_SERVERS != 0 */

    /* A Port negotiation was received */
    if (DlcId == INVALID_DLC_ID)
    {
    #if NUM_RF_SERVERS != 0
        /* This request is for a new channel */
        serverId = (U8) (Parms[0] >> 1);
        if (RfGetFreeServiceChannel(MuxId, serverId, &channel) != BT_STATUS_SUCCESS)
    #endif /* NUM_RF_SERVERS != 0 */
        {
            /* No registered service */
            AssertEval(RfSendAck(DM_F, MuxId, Parms[0]) == BT_STATUS_PENDING);
            return;
        }
        channel->dlci = Parms[0];
        channel->muxId = MuxId;
    }
    else
    {
        /* This request is for an existing channel */
        channel = DLC(MuxId, DlcId).channel;
    }

    if ((channel->rfChFlags & CHNL_FLAG_USE_PENDING) || !(MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW))
    {
        /*
         * This PN request has either been received before connection 
         * establishment, or this connection does not support credit flow
         * control.  In the latter case, we don't re-negotiate after the 
         * connection is up).
         */
        if (Parms[2] < 64)
        {
            channel->priority = Parms[2];
        }
        else
        {
            channel->priority = 0;
        }

        maxFrameSize = (U16) (Parms[4] | (Parms[5] << 8));
        if ((maxFrameSize >= RF_MIN_FRAME_SIZE) && (maxFrameSize <= channel->maxFrameSize))
        {
            /* The frame size is valid, accept it */
            channel->maxFrameSize = maxFrameSize;
        }
        else
        {
            channel->maxFrameSize = channel->maxFrameSize;
        }

        if ((Parms[1] & 0xF0) == CREDIT_FLOW_REQ)
        {
            /*
             * Credit flow control has been requested.
             * If this is not the first DLC being established, and negotiation
             * has not already been successful, this must be rejected.
             */
            if ((MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW) || MUX(MuxId).dlcCount < 1)
            {
                /* Accept the credit flow control */
                MUX(MuxId).flags |= MUX_FLAG_CREDIT_FLOW;
                channel->txCredit = (U16) (Parms[7] & 0x07);
                goto RF_SEND_PN;
            }
        }

        /*
         * Credit Flow Control is not in effect, 
         * set the initial flow state.
         */
        if (!(MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW))
        {
            if (channel->rxCredit)
            {
                channel->rfSignals &= ~RF_FLOW;
            }
            else
            {
                channel->rfSignals |= RF_FLOW;
            }
        }
    }

  RF_SEND_PN:
#ifdef __RFCOMM_TESTER_
    if ((MUX(MuxId).state == MUX_STATE_OPEN) && (RfcommTesterStateQueuy()))
    {
        if (Parms[2] < 64)
        {
            channel->priority = Parms[2];
        }
        else
        {
            channel->priority = 0;
        }
    }
#endif /* __RFCOMM_TESTER_ */ 

    channel->rfChFlags |= CHNL_FLAG_NEG_DONE;
    RfSendParmNegotiation(channel, 0);
}


/*****************************************************************************
 * FUNCTION
 *  RfCtlHandlePNCnf
 * DESCRIPTION
 *  Process a port negotiation response.
 * PARAMETERS
 *  MuxId       [IN]        
 *  DlcId       [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfCtlHandlePNCnf(U8 MuxId, U8 DlcId, U8 *Parms)
{
    if (DlcId != INVALID_DLC_ID)
    {
        if (DLC(MuxId, DlcId).flags & DLC_FLAG_REQ_SENT)
        {

            EVM_CancelTimer(&(DLC(MuxId, DlcId).channel->timer));
            DLC(MuxId, DlcId).flags &= ~DLC_FLAG_REQ_SENT;
            RfCheckForCreditToSend(DLC(MuxId, DlcId).channel);

            if (DLC(MuxId, DlcId).flags & DLC_FLAG_CREDIT_FLOW)
            {
                /* We requested credit flow control */
                if ((Parms[1] & 0xF0) != CREDIT_FLOW_RSP)
                {
                    /* The other end does not support credit based flow ctrl */
                    if ((MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW) && MUX(MuxId).dlcCount)
                    {
                        /*
                         * Once flow control has been negotiated, it
                         * cannot be rejected.
                         */
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_RE_FLOWCTL_DISC);
                        RfDisconnectL2CAP(MuxId);
                        return;
                    }

                    DLC(MuxId, DlcId).flags &= ~DLC_FLAG_CREDIT_FLOW;

                    /* Set the initial flow state */
                    DLC(MuxId, DlcId).channel->rxCredit = DLC(MuxId, DlcId).channel->initialRxCredit;
                    if (DLC(MuxId, DlcId).channel->rxCredit)
                    {
                        DLC(MuxId, DlcId).channel->rfSignals &= ~RF_FLOW;
                    }
                    else
                    {
                        DLC(MuxId, DlcId).channel->rfSignals |= RF_FLOW;
                    }
                }
                else
                {
                    /* The remote device supports credit flow control */
                    DLC(MuxId, DlcId).channel->txCredit = (U16) (Parms[7] & 0x07);
                    MUX(MuxId).flags |= MUX_FLAG_CREDIT_FLOW;
                }
            }

            /* Validate the negotiated parameters */
            if (((Parms[4] | (Parms[5] << 8)) <= DLC(MuxId, DlcId).channel->maxFrameSize) &&
                ((Parms[4] | (Parms[5] << 8)) >= RF_MIN_FRAME_SIZE))
            {

                /* Parms are OK, advance the state */
                DLC(MuxId, DlcId).channel->maxFrameSize = (U16) (Parms[4] | (Parms[5] << 8));
                RfAdvanceNegPending(DLC(MuxId, DlcId).channel, TRUE);
            }
            else
            {
                /*
                 * Invalid frame size received.  An attempt is made here to
                 * continue with the connection, by using a frame size no larger
                 * than the default RFCOMM frame size.
                 */
                DLC(MuxId, DlcId).channel->maxFrameSize = (U16)
                    min(DLC(MuxId, DlcId).channel->maxFrameSize, RF_DEFAULT_FRAMESIZE);
                RfAdvanceNegPending(DLC(MuxId, DlcId).channel, TRUE);
            }
        }
    }
    else
    {
        /* No valid DLC found */
        RfSendAck(DM_F, MuxId, Parms[0]);
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfCtlHandleMSCReq
 * DESCRIPTION
 *  Process an modem status request.
 * PARAMETERS
 *  MuxId       [IN]        
 *  DlcId       [IN]        
 *  Parms       [IN]        
 *  Length      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfCtlHandleMSCReq(U8 MuxId, U8 DlcId, U8 *Parms, U16 Length)
{
    RfModemStatus modemStatus;
    U8 breakLen = 0;
    BtStatus status;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_RECV_MSC_REQ_DLC_FLAG, DLC(MuxId, DlcId).flags);
    /* Respond (if possible) with the same values that were received */
    status = RfSendModemStatus(DLC(MuxId, DlcId).channel, Parms[1], breakLen, 0);
    if (status != BT_STATUS_PENDING)
    {
        return;
    }

    modemStatus.signals = (RfSignals) (Parms[1] & 0xCE);
    /* Set the remote status */
    if (!(Parms[1] & EA_BIT) && (Length > 2) && (Parms[2] & 0x02))
    {
        /* A break field was received */
        modemStatus.breakLen = (U8) (Parms[2] >> 4);
    }
    else
    {
        modemStatus.breakLen = 0;
    }

    breakLen = modemStatus.breakLen;
    DLC(MuxId, DlcId).flags |= DLC_FLAG_MSC_RCVD;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_RX_MSC, DLC(MuxId, DlcId).flags);

    if (MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW)
    {
        RF_AdvanceCredit(DLC(MuxId, DlcId).channel, 0);
    }
    if (!(MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW))
    {
        if ((modemStatus.signals & RF_FLOW))
        {
            /* Flow control is off */
            DLC(MuxId, DlcId).flags |= DLC_FLAG_FCOFF;
        }
        else
        {
            /* If flow is back on, send any data on this DLC */
            DLC(MuxId, DlcId).flags &= ~DLC_FLAG_FCOFF;
            RfMUXCheckTxQueues(MuxId);
        }
    }
    else
    {
        RfMUXCheckTxQueues(MuxId);
    }

    /* Indicate status to the application */
    RfAppCallback(DLC(MuxId, DlcId).channel, BT_STATUS_SUCCESS, RFEVENT_MODEM_STATUS_IND, &modemStatus);
}


/*****************************************************************************
 * FUNCTION
 *  RfCtlHandleRPNReq
 * DESCRIPTION
 *  Process a remote port negotiation request.
 * PARAMETERS
 *  MuxId       [IN]        
 *  DlcId       [IN]        
 *  Parms       [IN]        
 *  length      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfCtlHandleRPNReq(U8 MuxId, U8 DlcId, U8 *Parms, U16 length)
{
    RfPortSettings portSettings;
    RfChannel *channel;

#if NUM_RF_SERVERS != 0
    U8 serverId;
#endif /* NUM_RF_SERVERS != 0 */

    OS_MemSet((U8*) & portSettings, 0, sizeof(RfPortSettings));
    if (length > 1)
    {
        /* This is a request to set port settings */
        portSettings.baudRate = Parms[1];
        portSettings.dataFormat = Parms[2];
        portSettings.flowControl = Parms[3];
        portSettings.xonChar = Parms[4];
        portSettings.xoffChar = Parms[5];
        portSettings.parmMask = LEtoHost16(&(Parms[6]));
    }
    else
    {
        /* This is a request to get port settings */
        portSettings.baudRate = RF_BAUD_230400;
        portSettings.dataFormat = RF_DATA_BITS_8 | RF_STOP_BITS_1 | RF_PARITY_NONE;
        /* portSettings.parmMask = RF_PARM_BAUDRATE | RF_PARM_DATA_FORMAT; */
        portSettings.flowControl = RF_XON_ON_INPUT | RF_RTC_ON_INPUT | RF_RTC_ON_OUTPUT;
        portSettings.parmMask = RF_PARM_BAUDRATE | RF_PARM_DATABITS | RF_PARM_XON_INPUT | RF_PARM_FLOW_DTR_DSR;
    }

    /* Process the request */
    if (DlcId == INVALID_DLC_ID)
    {
        /* This request is for a new channel */
    #if NUM_RF_SERVERS != 0
        serverId = (U8) (Parms[0] >> 3);
        if (RfGetFreeServiceChannel(MuxId, serverId, &channel) != BT_STATUS_SUCCESS)
    #endif /* NUM_RF_SERVERS != 0 */
        {
            /* No registered service found */
            AssertEval(RfSendAck(DM_F, MuxId, (U8) (Parms[0] >> 2)) == BT_STATUS_PENDING);
            return;
        }
    #if NUM_RF_SERVERS != 0
        channel->dlci = (U8) (Parms[0] >> 2);
        channel->muxId = MuxId;
        AssertEval(RfSendRemotePortNeg(channel, &portSettings, 0) == BT_STATUS_PENDING);
        return;
    #endif /* NUM_RF_SERVERS != 0 */
    }
    else
    {
        /* This is an existing channel */
        channel = DLC(MuxId, DlcId).channel;
    }

    DLC(MuxId, DlcId).flags |= DLC_FLAG_REQ_INCOMING;

    if (length > 1)
    {
        /* A Remote Port Negotiation was received */
    #if RF_SEND_CONTROL == XA_ENABLED
        RfAppCallback(channel, BT_STATUS_SUCCESS, RFEVENT_PORT_NEG_IND, &portSettings);
    }
    else
    {
        /* This is a request for port settings */
        RfAppCallback(channel, BT_STATUS_SUCCESS, RFEVENT_PORT_STATUS_IND, 0);
    #endif /* RF_SEND_CONTROL == XA_ENABLED */
    }

    if ((DLC(MuxId, DlcId).flags & DLC_FLAG_REQ_INCOMING) &&
        !(DLC(MuxId, DlcId).channel->rfChFlags & CHNL_FLAG_RSP_DELAYED))
    {
        /* The app didn't respond, so respond for it (accept settings) */
        DLC(MuxId, DlcId).flags &= ~DLC_FLAG_REQ_INCOMING;
        RfSendRemotePortNeg(channel, &portSettings, 0);
    }
}

#if RF_SEND_CONTROL == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  RfCtlHandleRPNCnf
 * DESCRIPTION
 *  Process a remote port negotiation confirmation.
 * PARAMETERS
 *  MuxId       [IN]        
 *  DlcId       [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfCtlHandleRPNCnf(U8 MuxId, U8 DlcId, U8 *Parms)
{
    RfPortSettings portSettings;

    if (DlcId != INVALID_DLC_ID)
    {
        if (DLC(MuxId, DlcId).flags & DLC_FLAG_REQ_SENT)
        {

            portSettings.baudRate = Parms[1];
            portSettings.dataFormat = Parms[2];
            portSettings.flowControl = Parms[3];
            portSettings.xonChar = Parms[4];
            portSettings.xoffChar = Parms[5];
            portSettings.parmMask = LEtoHost16(&(Parms[6]));

            EVM_CancelTimer(&(DLC(MuxId, DlcId).channel->timer));
            DLC(MuxId, DlcId).flags &= ~DLC_FLAG_REQ_SENT;
            RfCheckForCreditToSend(DLC(MuxId, DlcId).channel);

            /* Notify app of confirmation */
            if (DLC(MuxId, DlcId).flags & DLC_FLAG_RPN_STATUS)
            {
                DLC(MuxId, DlcId).flags &= ~DLC_FLAG_RPN_STATUS;
                RfAppCallback(DLC(MuxId, DlcId).channel, BT_STATUS_SUCCESS, RFEVENT_PORT_STATUS_CNF, &portSettings);
            }
            else
            {
                RfAppCallback(DLC(MuxId, DlcId).channel, BT_STATUS_SUCCESS, RFEVENT_PORT_NEG_CNF, &portSettings);
            }
        }
    }
    else
    {
        /* No valid DLC found */
        AssertEval(RfSendAck(DM_F, MuxId, (U8) (Parms[0] >> 2)) == BT_STATUS_PENDING);
    }
}
#endif /* RF_SEND_CONTROL == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  RfCtlHandleRLSReq
 * DESCRIPTION
 *  Process a remote line status request.
 * PARAMETERS
 *  MuxId       [IN]        
 *  DlcId       [IN]        
 *  Parms       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfCtlHandleRLSReq(U8 MuxId, U8 DlcId, U8 *Parms)
{
#if RF_SEND_CONTROL == XA_ENABLED
    RfLineStatus lineStatus;
#else 
    /* UNUSED_PARAMETER(DlcId); */
#endif /* RF_SEND_CONTROL == XA_DISABLED */

    /* Respond with the same values that were received */
    AssertEval(RfSendLineStatus(DLC(MuxId, DlcId).channel, Parms[1], 0) == BT_STATUS_PENDING);

#if RF_SEND_CONTROL == XA_ENABLED
    /* Set the Remote line Status */
    lineStatus = (RfLineStatus) (Parms[1] & 0x0E);

    /* Indicate status to the application */
    RfAppCallback(DLC(MuxId, DlcId).channel, BT_STATUS_SUCCESS, RFEVENT_LINE_STATUS_IND, &lineStatus);
#endif /* RF_SEND_CONTROL == XA_ENABLED */
}


/*****************************************************************************
 * FUNCTION
 *  RfCtlHandleAggregateFlow
 * DESCRIPTION
 *  Process the FCON/FCOFF commands.
 * PARAMETERS
 *  MuxId       [IN]        
 *  mType       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfCtlHandleAggregateFlow(U8 MuxId, U8 mType)
{
#if RF_SEND_CONTROL == XA_ENABLED || UPF_TWEAKS == XA_ENABLED
    U8 event;
    I8 i;
#endif /* RF_SEND_CONTROL == XA_ENABLED || UPF_TWEAKS == XA_ENABLED */

    if (MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALID_FLOWCTRL);
        AssertEval(RfSendNSC(MuxId, mType) == BT_STATUS_PENDING);
        return;
    }

    if (mType & CR_BIT)
    {
        /* Set the aggregate flow control */
        if ((mType & 0xFD) == FCOFF)
        {
            MUX(MuxId).flags |= MUX_FLAG_FCOFF;
            AssertEval(RfSendAggregateFlow(MuxId, FALSE, 0) == BT_STATUS_PENDING);
        #if RF_SEND_CONTROL == XA_ENABLED || UPF_TWEAKS == XA_ENABLED
            event = RFEVENT_FLOW_OFF_IND;
        #endif /* RF_SEND_CONTROL == XA_ENABLED */
        }
        else
        {
            MUX(MuxId).flags &= ~MUX_FLAG_FCOFF;
            AssertEval(RfSendAggregateFlow(MuxId, TRUE, 0) == BT_STATUS_PENDING);
            RfMUXCheckTxQueues(MuxId);
        #if RF_SEND_CONTROL == XA_ENABLED || UPF_TWEAKS == XA_ENABLED
            event = RFEVENT_FLOW_ON_IND;
        #endif /* RF_SEND_CONTROL == XA_ENABLED */
        }

    #if RF_SEND_CONTROL == XA_ENABLED
        for (i = 0; i < NUM_RF_CHANNELS; i++)
        {
            if (DLC(MuxId, i).flags & DLC_FLAG_ALLOCATED)
            {
                RfAppCallback(DLC(MuxId, i).channel, BT_STATUS_SUCCESS, event, 0);
            }
        }
    #endif /* RF_SEND_CONTROL == XA_ENABLED */
    }

    /* Added for unplugFest testing */
#if UPF_TWEAKS == XA_ENABLED
    else
    {
        if ((mType & 0xFD) == FCOFF)
        {
            event = RFEVENT_FLOW_OFF_CNF;
        }
        else
        {
            event = RFEVENT_FLOW_ON_CNF;
        }

        for (i = 0; i < NUM_RF_CHANNELS; i++)
        {
            if ((DLC(MuxId, i).flags & DLC_FLAG_ALLOCATED) && (DLC(MuxId, i).flags |= DLC_FLAG_REQ_SENT))
            {

                /* Notify the app */
                EVM_CancelTimer(&(DLC(MuxId, i).channel->timer));
                RfAppCallback(DLC(MuxId, i).channel, BT_STATUS_SUCCESS, event, 0);
            }
        }
    }
#endif /* UPF_TWEAKS == XA_ENABLED */
}


/*****************************************************************************
 * FUNCTION
 *  RfCtlHandleTEST
 * DESCRIPTION
 *  Process the TEST command.
 * PARAMETERS
 *  MuxId       [IN]        
 *  Parms       [IN]        
 *  Len         [IN]        
 *  CR          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfCtlHandleTEST(U8 MuxId, U8 *Parms, U16 Len, U8 CR)
{
#if RF_SEND_TEST == XA_ENABLED
    U8 testPattern;
#endif /* RF_SEND_TEST == XA_ENABLED */

    if (CR)
    {
        /* Respond to the test */
        if (!(MUX(MuxId).flags & MUX_FLAG_TEST_RECV))
        {
            OS_MemCopy(&(MUX(MuxId).testRspData[2]), Parms, (U16) min(Len, RF_DEFAULT_FRAMESIZE));
            AssertEval(RfSendTest(MuxId, (U8) Len, 0) == BT_STATUS_PENDING);
        }
        else
        {
            Assert(0);
        }
    }
#if RF_SEND_TEST == XA_ENABLED
    else
    {
        /* Validate the response */
        if (DLC(MuxId, MUX(MuxId).testDlcId).flags & DLC_FLAG_REQ_SENT)
        {
            EVM_CancelTimer(&(DLC(MuxId, MUX(MuxId).testDlcId).channel->timer));
            MUX(MuxId).flags &= ~MUX_FLAG_TEST_SENT;
            DLC(MuxId, MUX(MuxId).testDlcId).flags &= ~DLC_FLAG_REQ_SENT;
            RfCheckForCreditToSend(DLC(MuxId, MUX(MuxId).testDlcId).channel);

            testPattern = TEST_PATTERN;
            if (OS_MemCmp(Parms, Len, &testPattern, 1) == TRUE)
            {
                /* Contents compare OK */
                RfAppCallback(DLC(MuxId, MUX(MuxId).testDlcId).channel, BT_STATUS_SUCCESS, RFEVENT_TEST_CNF, 0);
            }
            else
            {
                /* Contents are different */
                RfAppCallback(DLC(MuxId, MUX(MuxId).testDlcId).channel, BT_STATUS_FAILED, RFEVENT_TEST_CNF, 0);
            }
        }
    }
#endif /* RF_SEND_TEST == XA_ENABLED */
}


/*****************************************************************************
 * FUNCTION
 *  RfCtlHandleNSC
 * DESCRIPTION
 *  Process the NSC command.
 * PARAMETERS
 *  MuxId       [IN]        
 *  mType       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfCtlHandleNSC(U8 MuxId, U8 mType)
{
    I8 i;
    RfEvent event = 0;

    /* Find the first command sent, that matches this response */
    for (i = 0; i < NUM_RF_CHANNELS; i++)
    {
        if ((DLC(MuxId, i).flags & DLC_FLAG_REQ_SENT) && (DLC(MuxId, i).channel->cmdSent == mType))
        {
            /*
             * Found the connection that sent the request, set up
             * * the appropriate response callback
             */
            EVM_CancelTimer(&(DLC(MuxId, i).channel->timer));
            switch (mType & 0xFD)
            {
                case PN:
                    RfAdvanceNegPending(DLC(MuxId, i).channel, FALSE);
                    break;
            #if RF_SEND_TEST == XA_ENABLED
                case TEST:
                    event = RFEVENT_TEST_CNF;
                    break;
            #endif /* RF_SEND_TEST == XA_ENABLED */
            #if RF_SEND_CONTROL == XA_ENABLED
                case MSC:
                    event = RFEVENT_MODEM_STATUS_CNF;
                    break;
                case RLS:
                    event = RFEVENT_LINE_STATUS_CNF;
                    break;
                case RPN:
                    event = RFEVENT_PORT_NEG_CNF;
                    break;
            #endif /* RF_SEND_CONTROL == XA_ENABLED */
            }

            if (event != 0)
            {
                RfAppCallback(DLC(MuxId, i).channel, BT_STATUS_FAILED, event, 0);
            }

            /* Stop looking */
            break;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfCtlHandleCnf
 * DESCRIPTION
 *  Process various confirmations.
 * PARAMETERS
 *  MuxId       [IN]        
 *  DlcId       [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfCtlHandleCnf(U8 MuxId, U8 DlcId, RfEvent Event)
{
    if ((DLC(MuxId, DlcId).flags & DLC_FLAG_MSC_PENDING) && (Event == RFEVENT_MODEM_STATUS_CNF))
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_RECV_MSC_CNF_DLC_FLAG,DLC(MuxId, DlcId).flags);
        EVM_CancelTimer(&(DLC(MuxId, DlcId).channel->timer));
        DLC(MuxId, DlcId).flags &= ~DLC_FLAG_MSC_PENDING;
        RfCheckForCreditToSend(DLC(MuxId, DlcId).channel);
        RfMUXCheckTxQueues(MuxId);
    }
    else if (DLC(MuxId, DlcId).flags & DLC_FLAG_REQ_SENT)
    {

        EVM_CancelTimer(&(DLC(MuxId, DlcId).channel->timer));
        DLC(MuxId, DlcId).flags &= ~DLC_FLAG_REQ_SENT;
        RfCheckForCreditToSend(DLC(MuxId, DlcId).channel);

        /* Notify app of confirmation */
        RfAppCallback(DLC(MuxId, DlcId).channel, BT_STATUS_SUCCESS, Event, 0);

    }
}


/*****************************************************************************
 * FUNCTION
 *  RfCtlParseParms
 * DESCRIPTION
 *  Parser for multiplexer control channel
 * PARAMETERS
 *  MuxId       [IN]        
 *  mType       [IN]        
 *  parms       [IN]        
 *  length      [IN]        
 * RETURNS
 *  BtStatus
 *****************************************************************************/
static BtStatus RfCtlParseParms(U8 MuxId, U8 *mType, U8 **parms, U16 *length)
{
    U8 headerLen;

    /*
     * Check for a valid length, the control channel cannot handle frames
     * larger than the default frame size
     */
    if (MUX(MuxId).rxLen > RF_DEFAULT_FRAMESIZE)
    {
        /* Invalid RX length, disconnect */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALID_LENGTH);
        RfDisconnectL2CAP(MuxId);
        return BT_STATUS_INVALID_PARM;
    }

    /* Find the first part of the data payload */
    if ((U8) MUX(MuxId).rxBuff[2] & 0x01)
    {
        /* 1 Byte Length */
        headerLen = 3;
    }
    else
    {
        /* 2 Byte Length */
        headerLen = 4;
    }

    /* Get the message type */
    *mType = MUX(MuxId).rxBuff[headerLen];
    *parms = MUX(MuxId).rxBuff + headerLen;

    /* Find the length */
    if ((*parms)[1] & 0x01)
    {
        /* 1 Byte Length */
        *length = (*parms)[1] >> 1;
        headerLen = 2;
    }
    else
    {
        /* 2 Byte Length */
        *length = ((*parms)[1] >> 1) | ((*parms)[2] << 7);
        headerLen = 3;
    }

    /* Set a pointer to the parameter section */
    *parms += headerLen;

    /* Validate the length of the parameters */
    if (*length > (MUX(MuxId).rxLen - headerLen))
    {
        /* Invalid parm length, disconnect */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALUD_PARM_LENGTH);
        RfDisconnectL2CAP(MuxId);
        return BT_STATUS_INVALID_PARM;
    }
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  RfControlChannel
 * DESCRIPTION
 *  Handles multiplexer control channel commands.
 * PARAMETERS
 *  MuxId       [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfControlChannel(U8 MuxId, U8 Event)
{
    U8 dlcId;
    U8 mType;
    U16 length;
    U8 *parms;

    if (Event == UIH)
    {

        /* Get the parm info */
        if( BT_STATUS_SUCCESS != RfCtlParseParms(MuxId, &mType, &parms, &length) ){
            OS_Report("[RF][ERR] RfControlChannel : RfCtlParseParms failed");
            return;
        }

        switch (mType & 0xFD)
        {
            case TEST:
                /* Test command */
                RfCtlHandleTEST(MuxId, parms, length, (U8) (mType & CR_BIT));
                break;

            case FCON:
            case FCOFF:
                /* Aggregate flow control */
                RfCtlHandleAggregateFlow(MuxId, mType);
                break;

            case PN:
                /*
                 * Parameter Negotiation. 
                 * * Get the DLC ID, it is OK to get invalid DLC ID 
                 * * Since an PN command may be received before the
                 * * channel is actually up.
                 */
                (void)RfFindDlcIdByDlci(MuxId, parms[0], &dlcId);
                if (mType & CR_BIT)
                {
                    /* Request */
                    RfCtlHandlePNReq(MuxId, dlcId, parms);
                }
                else
                {
                    /* Response */
                    RfCtlHandlePNCnf(MuxId, dlcId, parms);
                }
                break;

            case RPN:
                /*
                 * Remote Port Negotiation.
                 * * Get the DLC ID, it is OK to get invalid DLC ID 
                 * * Since an PN command may be received before the
                 * * channel is actually up.
                 */
                (void)RfFindDlcIdByDlci(MuxId, (U8) (parms[0] >> 2), &dlcId);
                if (mType & CR_BIT)
                {
                    RfCtlHandleRPNReq(MuxId, dlcId, parms, length);
                }
            #if RF_SEND_CONTROL == XA_ENABLED
                else
                {
                    RfCtlHandleRPNCnf(MuxId, dlcId, parms);
                }
            #endif /* RF_SEND_CONTROL == XA_ENABLED */
                break;

            case MSC:
                /*
                 * Modem Status.
                 * Get the DLC ID, may return invalid DLC ID 
                 * if no channel exists.
                 */
                if (RfFindDlcIdByDlci(MuxId, (U8) (parms[0] >> 2), &dlcId) == BT_STATUS_SUCCESS)
                {

                    /* Found the DLC */
                    if (mType & CR_BIT)
                    {
                        RfCtlHandleMSCReq(MuxId, dlcId, parms, length);
                    }
                    else
                    {
                        RfCtlHandleCnf(MuxId, dlcId, RFEVENT_MODEM_STATUS_CNF);
                    }
                }
                else
                {
                    /* No valid DLC found */
                    RfSendAck(DM_F, MuxId, (U8) (parms[0] >> 2));
                }
                break;

            case RLS:
                /*
                 * Remote Line Status.
                 * Get the DLC ID, may return invalid DLC ID 
                 * if no channel exists.
                 */
                if (RfFindDlcIdByDlci(MuxId, (U8) (parms[0] >> 2), &dlcId) == BT_STATUS_SUCCESS)
                {

                    /* Found a DLC */
                    if (mType & CR_BIT)
                    {
                        RfCtlHandleRLSReq(MuxId, dlcId, parms);
                    }
                #if RF_SEND_CONTROL == XA_ENABLED
                    else
                    {
                        RfCtlHandleCnf(MuxId, dlcId, RFEVENT_LINE_STATUS_CNF);
                    }
                #endif /* RF_SEND_CONTROL == XA_ENABLED */
                }
                else
                {
                    /* No valid DLC found */
                    RfSendAck(DM_F, MuxId, (U8) (parms[0] >> 2));
                }
                break;

            case NSC:
                /*
                 * Non-Supported Command.
                 * A command the was sent is not supported by the 
                 * remote device.
                 */
                RfCtlHandleNSC(MuxId, parms[0]);
                break;

            default:
                /*
                 * The command the was received is not supported command, 
                 * send an NSC.
                 */
                RfSendNSC(MuxId, mType);
                break;
        }

    }
    else if (Event == L2EVENT_PACKET_HANDLED)
    {
        /* See if this is a packet handled event for a test response */
        if (MUX(MuxId).txPacket == &(MUX(MuxId).testRspPacket))
        {
            MUX(MuxId).flags &= ~MUX_FLAG_TEST_RECV;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfSendParmNegotiation
 * DESCRIPTION
 *  Send a parameter negotiation packet.  The values for dlci,
 *  priority, and frameSize must have been set previously.
 * PARAMETERS
 *  channel     [IN]        
 *  CR          [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfSendParmNegotiation(RfChannel *channel, U8 CR)
{
    BtPacket *packet;
    BtStatus status;

    /* Set up the packet */
    packet = RfCtlGetControlPacket(channel->muxId);
    if (packet == 0)
    {
        return BT_STATUS_NO_RESOURCES;
    }

#if 0
    if (!(IsListEmpty(&(MUX(channel->muxId).freeList))))
    {
        packet = (BtPacket*) RemoveHeadList(&(MUX(channel->muxId).freeList));
    }
    else
    {
        return BT_STATUS_NO_RESOURCES;
    }
#endif


    /* Set up the PN payload */
    packet->dataLen = 10;
    OS_MemSet(packet->data, 0, 10);
    packet->data[0] = PN | CR;
    channel->cmdSent = PN | CR;
    packet->data[1] = (8 << 1) | EA_BIT;
    packet->data[2] = channel->dlci;

    /* This is a valid negotiation */
    if (CR == CR_BIT)
    {
        /*
         * This is a request, check to see if credit flow 
         * must be negotiated.
         */
        if (DLC(channel->muxId, channel->dlcId).flags & DLC_FLAG_CREDIT_FLOW)
        {
            packet->data[3] = CREDIT_FLOW_REQ;
        }
    }
    else
    {
        /*
         * This is a response, check to see if credit flow 
         * control was requested 
         */
        if ((channel->rfChFlags & CHNL_FLAG_USE_PENDING) && (MUX(channel->muxId).flags & MUX_FLAG_CREDIT_FLOW))
        {
            packet->data[3] = CREDIT_FLOW_RSP;
        }
    #ifdef __RFCOMM_TESTER_
        if ((MUX(channel->muxId).flags & MUX_FLAG_CREDIT_FLOW) &&
            (MUX(channel->muxId).state == MUX_STATE_OPEN) && (RfcommTesterStateQueuy()))
        {
            packet->data[3] = CREDIT_FLOW_RSP;
        }
    #endif /* __RFCOMM_TESTER_ */ 

    }

    /* Advance initial credit if necessary */
    if (packet->data[3] != 0)
    {
        packet->data[9] = (U8) min(channel->initialRxCredit, 7);
        channel->grantedCredit = packet->data[9];
        channel->rxCredit = (S16) (channel->initialRxCredit - channel->grantedCredit);
        channel->BusyCredit = 0;
    }

    packet->data[4] = channel->priority;
    StoreLE16(&(packet->data[6]), channel->maxFrameSize);
    status = RfCtlSendInternalPacket(packet, channel->muxId);
#if 0
    packet->ulpContext = (void*)0;
    status = RfSendDataInternal(channel->muxId, 0, packet, BTP_FLAG_HIGH_PRIPORTY);
#endif    
    if (status != BT_STATUS_PENDING)
    {
        /* Send failed */
        InsertTailList(&(MUX(channel->muxId).freeList), &(packet->node));
    }
    else if (CR == CR_BIT)
    {
        EVM_StartTimer(&(channel->timer), RF_T2_TIMEOUT);
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_START_TIMER_SEND_CMD_ON_DLCI_OF_MUX_TIME, channel->cmdSent, channel->dlci, channel->muxId, RF_T2_TIMEOUT/1000);
        DLC(channel->muxId, channel->dlcId).flags |= DLC_FLAG_REQ_SENT;
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfSendModemStatus
 * DESCRIPTION
 *  Send a modem status command.
 * PARAMETERS
 *  Channel     [IN]        
 *  Status      [IN]        
 *  Break       [IN]        
 *  CR          [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfSendModemStatus(RfChannel *Channel, U8 Status, U8 Break, U8 CR)
{
    BtPacket *packet;
    U8 length;
    BtStatus status;

    packet = RfCtlGetControlPacket(Channel->muxId);
    if (packet == 0)
    {
        return BT_STATUS_NO_RESOURCES;
    }

#if 0
    if (!(IsListEmpty(&(MUX(Channel->muxId).freeList))))
    {
        packet = (BtPacket*) RemoveHeadList(&(MUX(Channel->muxId).freeList));
    }
    else
    {
        return BT_STATUS_NO_RESOURCES;
    }
#endif

    if (Break)
    {
        length = 3;
        packet->data[3] = Status & 0xCE;
        packet->data[4] = (Break << 4) | 0x03;
    }
    else
    {
        length = 2;
        packet->data[3] = (Status & 0xCE) | EA_BIT;
    }

    packet->dataLen = length + 2;
    packet->data[0] = MSC | CR;
    Channel->cmdSent = MSC | CR;
    packet->data[1] = (length << 1) | EA_BIT;
    packet->data[2] = (Channel->dlci << 2) | CR_BIT | EA_BIT;
    status = RfCtlSendInternalPacket(packet, Channel->muxId);
#if 0
    packet->ulpContext = (void*)0;
    status = RfSendDataInternal(Channel->muxId, 0, packet, BTP_FLAG_HIGH_PRIPORTY);
#endif    
    if (status != BT_STATUS_PENDING)
    {
        /* Send failed */
        InsertTailList(&(MUX(Channel->muxId).freeList), &(packet->node));
    }
    else if (CR == CR_BIT)
    {
        EVM_StartTimer(&(Channel->timer), RF_T2_TIMEOUT);
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_START_TIMER_SEND_CMD_ON_DLCI_OF_MUX_TIME, Channel->cmdSent, Channel->dlci, Channel->muxId, RF_T2_TIMEOUT/1000);
        DLC(Channel->muxId, Channel->dlcId).flags |= DLC_FLAG_REQ_SENT;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfSendLineStatus
 * DESCRIPTION
 *  Send a line status command.
 * PARAMETERS
 *  Channel     [IN]        
 *  Status      [IN]        
 *  CR          [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfSendLineStatus(RfChannel *Channel, RfLineStatus Status, U8 CR)
{
    BtPacket *packet;
    U8 length = 2;
    BtStatus status;

    packet = RfCtlGetControlPacket(Channel->muxId);
    if (packet == 0)
    {
        return BT_STATUS_NO_RESOURCES;
    }

#if 0
    if (!(IsListEmpty(&(MUX(Channel->muxId).freeList))))
    {
        packet = (BtPacket*) RemoveHeadList(&(MUX(Channel->muxId).freeList));
    }
    else
    {
        return BT_STATUS_NO_RESOURCES;
    }
#endif
    packet->dataLen = length + 2;
    packet->data[0] = RLS | CR;
    Channel->cmdSent = RLS | CR;
    packet->data[1] = (length << 1) | EA_BIT;
    packet->data[2] = (Channel->dlci << 2) | CR_BIT | EA_BIT;
    packet->data[3] = (Status & 0x0E) == 0 ? (Status & 0x0E) : (Status & 0x0E) | 0x01;

    status = RfCtlSendInternalPacket(packet, Channel->muxId);
#if 0
    packet->ulpContext = (void*)0;
    status = RfSendDataInternal(Channel->muxId, 0, packet, BTP_FLAG_HIGH_PRIPORTY);
#endif
    if (status != BT_STATUS_PENDING)
    {
        /* Send failed */
        InsertTailList(&(MUX(Channel->muxId).freeList), &(packet->node));
    }
    else if (CR == CR_BIT)
    {
        EVM_StartTimer(&(DLC(Channel->muxId, Channel->dlcId).channel->timer), RF_T2_TIMEOUT);
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_START_TIMER_SEND_CMD_ON_DLCI_OF_MUX_TIME, Channel->cmdSent, Channel->dlci, Channel->muxId, RF_T2_TIMEOUT/1000);
        DLC(Channel->muxId, Channel->dlcId).flags |= DLC_FLAG_REQ_SENT;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfSendRemotePortNeg
 * DESCRIPTION
 *  Send a remote port negotiation command.
 * PARAMETERS
 *  Channel     [IN]        
 *  Rpn         [IN]        
 *  CR          [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfSendRemotePortNeg(RfChannel *Channel, RfPortSettings *Rpn, U8 CR)
{
    BtPacket *packet;
    U8 length;
    BtStatus status;

    packet = RfCtlGetControlPacket(Channel->muxId);
    if (packet ==0)
    {
        return BT_STATUS_NO_RESOURCES;
    }

#if 0
    if (!(IsListEmpty(&(MUX(Channel->muxId).freeList))))
    {
        packet = (BtPacket*) RemoveHeadList(&(MUX(Channel->muxId).freeList));
    }
    else
    {
        return BT_STATUS_NO_RESOURCES;
    }
#endif

    if (Rpn != 0)
    {
        length = 8;
        packet->data[3] = Rpn->baudRate;
        packet->data[4] = Rpn->dataFormat;
        packet->data[5] = Rpn->flowControl;
        packet->data[6] = Rpn->xonChar;
        packet->data[7] = Rpn->xoffChar;
        StoreLE16(&(packet->data[8]), Rpn->parmMask);
    }
    else
    {
        length = 1;
    }

    packet->dataLen = length + 2;
    packet->data[0] = RPN | CR;
    Channel->cmdSent = RPN | CR;
    packet->data[1] = (length << 1) | EA_BIT;
    packet->data[2] = (Channel->dlci << 2) | CR_BIT | EA_BIT;
    status = RfCtlSendInternalPacket(packet, Channel->muxId);
#if 0    
    packet->ulpContext = (void*)0;
    status = RfSendDataInternal(Channel->muxId, 0, packet, BTP_FLAG_HIGH_PRIPORTY);
#endif    
    if (status != BT_STATUS_PENDING)
    {
        /* Send failed */
        InsertTailList(&(MUX(Channel->muxId).freeList), &(packet->node));
    }
    else if (CR == CR_BIT)
    {
        EVM_StartTimer(&(DLC(Channel->muxId, Channel->dlcId).channel->timer), RF_T2_TIMEOUT);
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_START_TIMER_SEND_CMD_ON_DLCI_OF_MUX_TIME, Channel->cmdSent, Channel->dlci, Channel->muxId, RF_T2_TIMEOUT/1000);
        DLC(Channel->muxId, Channel->dlcId).flags |= DLC_FLAG_REQ_SENT;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfSendAggregateFlow
 * DESCRIPTION
 *  Send an aggregate flow (FCON/FCOFF) command.
 * PARAMETERS
 *  MuxId       [IN]        
 *  Flow        [IN]        
 *  CR          [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfSendAggregateFlow(U8 MuxId, BOOL Flow, U8 CR)
{
    BtPacket *packet;
    BtStatus status;
    

    if (!(IsListEmpty(&(MUX(MuxId).freeList))))
    {
        packet = (BtPacket*) RemoveHeadList(&(MUX(MuxId).freeList));
    }
    else
    {
        return BT_STATUS_NO_RESOURCES;
    }

    packet->dataLen = 2;
    if (Flow)
    {
        packet->data[0] = FCON | CR;
    }
    else
    {
        packet->data[0] = FCOFF | CR;
    }
    packet->data[1] = EA_BIT;
    status = RfCtlSendInternalPacket(packet, MuxId);
#if 0
    packet->ulpContext = (void*)0;
    status = RfSendDataInternal(MuxId, 0, packet, BTP_FLAG_HIGH_PRIPORTY);
#endif    
    if (status != BT_STATUS_PENDING)
    {
        /* Send failed */
        InsertTailList(&(MUX(MuxId).freeList), &(packet->node));
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfSendTest
 * DESCRIPTION
 *  Send a remote port negotiation command.
 * PARAMETERS
 *  MuxId       [IN]        
 *  len         [IN]        
 *  CR          [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfSendTest(U8 MuxId, U8 len, U8 CR)
{
    BtPacket *packet;
    BtStatus status;

#if RF_SEND_TEST == XA_ENABLED
    if (CR == CR_BIT)
    {
        packet = &MUX(MuxId).testCmdPacket;
    }
    else
#endif /* RF_SEND_TEST == XA_ENABLED */
    {
        packet = &MUX(MuxId).testRspPacket;
    }

    packet->dataLen = len + 2;
    packet->data[0] = (U8) (TEST | CR);
    packet->data[1] = (U8) ((len << 1) | EA_BIT);
    status = RfCtlSendInternalPacket(packet, MuxId);
#if 0
    packet->ulpContext = (void*)0;
    status = RfSendDataInternal(MuxId, 0, packet, BTP_FLAG_HIGH_PRIPORTY);
#endif
    if (status == BT_STATUS_PENDING)
    {
        if (CR == CR_BIT)
        {
            MUX(MuxId).flags |= MUX_FLAG_TEST_SENT;
        }
        else
        {
            MUX(MuxId).flags |= MUX_FLAG_TEST_RECV;
        }
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfSendNSC
 * DESCRIPTION
 *  Send a NSC command.
 * PARAMETERS
 *  MuxId       [IN]        
 *  mType       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus RfSendNSC(U8 MuxId, U8 mType)
{
    BtPacket *packet;
    BtStatus status = BT_STATUS_NO_RESOURCES;

    if (mType & CR_BIT)
    {
        /* Respond to the unsupported request */
        if (!(IsListEmpty(&(MUX(MuxId).freeList))))
        {
            packet = (BtPacket*) RemoveHeadList(&(MUX(MuxId).freeList));
            packet->dataLen = 3;
            packet->data[0] = NSC;
            packet->data[1] = (1 << 1) | EA_BIT;
            packet->data[2] = mType;
            status = RfCtlSendInternalPacket(packet, MuxId);
#if 0
            packet->ulpContext = (void*)0;
            status = RfSendDataInternal(MuxId, 0, packet, BTP_FLAG_HIGH_PRIPORTY);
#endif            
            if (status != BT_STATUS_PENDING)
            {
                /* Send failed */
                InsertTailList(&(MUX(MuxId).freeList), &(packet->node));
            }
        }
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfCtlGetControlPacket
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtPacket *RfCtlGetControlPacket(U8 MuxId)
{
    BtPacket *packet = 0; 
    if (!(IsListEmpty(&(MUX(MuxId).freeList))))
    {
        packet = (BtPacket*) RemoveHeadList(&(MUX(MuxId).freeList));
        return (packet);
    }
    else
    {
        return 0;
    }
}

/*****************************************************************************
 * FUNCTION
 *  RfCtlSendInternalPacket
 * DESCRIPTION
 *  
 * PARAMETERS
 *  packet       [IN]        
 *  MuxId       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus RfCtlSendInternalPacket(BtPacket *packet, U8 MuxId)
{   
    BtStatus status;
    
    packet->ulpContext = (void*)0;
    status = RfSendDataInternal(MuxId, 0, packet, BTP_FLAG_HIGH_PRIPORTY);
    return status;
}

#else /* RFCOMM_PROTOCOL == XA_ENABLED */ 

/* Some compilers complain about empty files */
const U8 rfcomm_protocol = 0;

#endif /* RFCOMM_PROTOCOL == XA_ENABLED */

