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
 *     $Workfile:avctp.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains public API for the Audio/Video
 *     Remote Control Transport Protocol (AVCTP).
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
#include "l2cap.h"
#include "sdp.h"
#include "avctpi.h"
#include "avctpcon.h"
#include "avctp.h"
#include "sec.h"
#include "bt_feature.h"

#if XA_CONTEXT_PTR == XA_ENABLED
/*
 * The stack context is accessed through a pointer. Normally in this case
 * * the stack context would be dynamically allocated. For testing purposes
 * * the pointer points to a global structure.
 */
BtAvctpContext avTmp;
BtAvctpContext *avctpContext = &avTmp;
#else /* XA_CONTEXT_PTR == XA_ENABLED */ 
BtAvctpContext avctpContext;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */ 

void AvcMsgInit(AvctpChannel *chnl);

/*****************************************************************************
 * FUNCTION
 *  AVCTP_Init
 * DESCRIPTION
 *  Initialize the AVCTP SDK.  This function should only be called
 *  once, normally at initialization time.  The calling of this
 *  function can be specified in overide.h using the XA_LOAD_LIST
 *  macro (i.e. #define XA_LOAD_LIST XA_MODULE(AV)).
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BOOL AVCTP_Init(void)
{
    BOOL status = TRUE;
    BtStatus bsResult;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
	U8 i;
#endif

    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_INIT_LOG );
    
    /* Initialize Memory */
    status = AvctpAlloc();

    /* Initialize the connection state machine */
    AvctpConnInitStateMachine();

    /* Register AVCTP with L2CAP */
    AVCTP(psm).callback = AvctpL2Callback;
    AVCTP(psm).psm = BT_PSM_AVCTP;
    AVCTP(psm).localMtu = L2CAP_MAXIMUM_MTU;
    AVCTP(psm).minRemoteMtu = L2CAP_MINIMUM_MTU;
    AVCTP(psm).authorizedSetting = FALSE;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    AVCTP(psm).inLinkMode = AVCTP(psm).outLinkMode = L2MODE_BASIC;
#ifdef __BT_2_1_ERTM__
    AVCTP(psm).lockStepNeeded = 0;
    AVCTP(psm).highSpeedRequired = 0;
#endif    
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */

    bsResult = L2CAP_RegisterPsm(&(AVCTP(psm)));
    OS_Report("[AVCTP] register psm 0x%x status:%d",  AVCTP(psm).psm, bsResult );
    if ( bsResult != BT_STATUS_SUCCESS)
    {
        /* Unable to register */
        status = FALSE;
    }


#ifdef __BT_AVRCP_V14__
    /* AV14 register browsing psm */
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED	
    AVCTP(browse_psm).psm = BT_PSM_AVCTP_BROWSING;
    AVCTP(browse_psm).callback = AvctpL2Callback;
    AVCTP(browse_psm).localMtu =675; //L2CAP_MAXIMUM_MTU;
    AVCTP(browse_psm).minRemoteMtu = L2CAP_MINIMUM_MTU;
    AVCTP(browse_psm).authorizedSetting = FALSE;

    AVCTP(browse_psm).inLinkMode = AVCTP(browse_psm).outLinkMode = L2MODE_ENHANCEDMENT_RETRANSMISSION;
    AVCTP(browse_psm).inTransmitMax = AVCTP_MAX_RETRANS_NUM;
    AVCTP(browse_psm).channelUsage = L2CAP_CHANNEL_GENERAL;
    AVCTP(browse_psm).inBufferSize = (L2CAP_MAXIMUM_MTU + 7)*10;
#ifdef __BT_2_1_ERTM__    
    AVCTP(browse_psm).lockStepNeeded = 0;
    AVCTP(browse_psm).highSpeedRequired = 0;
#endif
    InitializeListHead( &AVCTP(rxPacketsBufferList) );
    InitializeListHead( &AVCTP(receivedPacketsList) );
    OS_MemSet((U8 *)&AVCTP(l2capRawData), 0, sizeof(AVCTP(l2capRawData)));
    for (i = 0; i < AVCTP_MAX_RETRANS_NUM ; i++)
	{
		InitializeListEntry(&(AVCTP(rfcRxPackets[i]).node));
		AVCTP(rfcRxPackets[i]).data = (U8 *)(AVCTP(l2capRawData[i]));
		InsertTailList(&AVCTP(rxPacketsBufferList), &AVCTP(rfcRxPackets[i]).node);
	}        

    if( TRUE == status ){
        bsResult = L2CAP_RegisterPsm(&(AVCTP(browse_psm))) ;
        if ( bsResult != BT_STATUS_SUCCESS){
            OS_Report("[AVCTP] Fail to register browsing channel bsResult:%d", bsResult);
            status = FALSE;
        }
    }else{
        OS_Report("[AVCTP] Fail to register AVCTP channel bsResult:%d", bsResult);
    }
#else
    OS_Report("[AVCTP] No enhance mode. Ignore register ");
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */

#endif /* #ifdef __BT_AVRCP_V14__ */
    
    
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    InitializeListEntry(&(AVCTP(secRecord).node));
    AVCTP(secRecord).id = SEC_L2CAP_ID;
    AVCTP(secRecord).channel = BT_PSM_AVCTP;
    AVCTP(secRecord).level = BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT;
#ifndef __BT_2_1_SIMPLE_PAIRING__
    AVCTP(secRecord).level_type = BT_SEC_LOW;
#else 
    AVCTP(secRecord).level_type = BT_SEC_MED;
#endif 
    SEC_RegisterRecord(&AVCTP(secRecord));
#endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 
    /* Initialized lists */
    InitializeListHead(&AVCTP(chnlList));

    return status;
}

#if L2CAP_DEREGISTER_FUNC == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  AVCTP_Deinit
 * DESCRIPTION
 *  Deinitializes the AVCTP module. Init() must later be called
 *  again to use any AVCTP function call. This function does no
 *  error checking so do NOT call when AVCTP is in use.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void AVCTP_Deinit(void)
{

    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_DEINIT_LOG );

    (void)L2CAP_DeregisterPsm(&(AVCTP(psm)));
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    SEC_Unregister(&AVCTP(secRecord));
#endif 

    (void)L2CAP_DeregisterPsm(&(AVCTP(browse_psm)));

}
#endif /* L2CAP_DEREGISTER_FUNC == XA_ENABLED */ /* L2CAP_DEREGISTER_FUNC == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  AVCTP_Register
 * DESCRIPTION
 *  Registers an application callback to create/receive AVCTP connections.
 *  This function must be called before any other AVCTP functions.
 * PARAMETERS
 *  chnl            [IN]        
 *  callback        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVCTP_Register(AvctpChannel *chnl, AvctpCallback callback,U16 psmValue)
{
    BtStatus status = BT_STATUS_SUCCESS;

    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_REGISTER_LOG );

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, callback != 0);

    OS_LockStack();

    /* Register this connection in the list */
    if (!IsNodeOnList(&AVCTP(chnlList), &chnl->node))
    {

        /* Clear the connection structure */
        OS_MemSet((U8*) chnl, 0, sizeof(AvctpChannel));

        /* Register the Avctp connection */
        chnl->callback = callback;

        /* Initialize the message handler */
        AvcMsgInit(chnl);

        /* Insert the channel on the list */
        InsertTailList(&AVCTP(chnlList), &chnl->node);

        /* AV13 Check the browsing psm value */
        chnl->channelType= psmValue;

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
 *  AVCTP_Deregister
 * DESCRIPTION
 *  De-registers the AVCTP callback. After making this call
 *  successfully, the callback specified in AVCTP_Register will
 *  receive no further events.
 * PARAMETERS
 *  chnl        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVCTP_Deregister(AvctpChannel *chnl)
{
    BtStatus status = BT_STATUS_SUCCESS;

    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_DEREGISTER_LOG );

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();

    if (IsNodeOnList(&AVCTP(chnlList), &chnl->node))
    {
        if (!AvctpIsConnected(&chnl->conn))
        {
            /* No connection is up, remove from list */
            RemoveEntryList(&chnl->node);
        }
        else
        {
            /* A connection is up */
            status = BT_STATUS_IN_USE;
        }
    }
    else
    {
        /* Connection is not registered */
        status = BT_STATUS_NOT_FOUND;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVCTP_DeregisterAllChnl
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVCTP_DeregisterAllChnl(void)
{
    BtStatus status = BT_STATUS_SUCCESS;
    AvctpChannel *Chnl;
    AvctpChannel *Next_Chnl;

    OS_LockStack();
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_DEREGISTER_ALL_LOG );
    if (!IsListEmpty(&AVCTP(chnlList)))
    {
        Chnl = (AvctpChannel*) GetHeadList(&AVCTP(chnlList));
        do
        {
            Next_Chnl = (AvctpChannel*) GetNextNode(&Chnl->node);
            RemoveEntryList(&Chnl->node);
            Chnl = Next_Chnl;
        } while ((AvctpChannel*) GetHeadList(&AVCTP(chnlList)) != Chnl);
    }
    status = BT_STATUS_SUCCESS;
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVCTP_Connect
 * DESCRIPTION
 *  Connect to the remote device.
 * PARAMETERS
 *  chnl        [IN]        
 *  remDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVCTP_Connect(AvctpChannel *chnl, BtRemoteDevice *remDev)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, remDev != 0);

    OS_LockStack();
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_CONNECT_API_LOG, chnl->channelType);

    /* See if the chnl is registered */
    if (IsNodeOnList(&AVCTP(chnlList), &chnl->node))
    {
        if (!AvctpIsConnected(&chnl->conn))
        {
            /* Start the connection */
            status = AvctpConnect(&chnl->conn, remDev, chnl->channelType);
        }
        else
        {
            /* Already connected */
            status = BT_STATUS_IN_USE;
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVCTP_ConnectRsp
 * DESCRIPTION
 *  Respond to a connect request from the remote device.
 * PARAMETERS
 *  Chnl        [IN]        
 *  Accept      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVCTP_ConnectRsp(AvctpChannel *Chnl, BOOL Accept)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_CONNECT_RSP_API_LOG );
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, Chnl != 0);

    OS_LockStack();

    /* See if the Chnl is registered */
    if (IsNodeOnList(&AVCTP(chnlList), &Chnl->node))
    {
		if( AVCTP_STATE_CONN_INCOMING == Chnl->conn.state || AVCTP_STATE_DISCONNECTED == Chnl->conn.state )
        {
            /* Start the connection */
            status = AvctpConnectRsp(&Chnl->conn, Accept, Chnl->channelType);
        }
        else
        {
			OS_Report("[AVCTP] AVCTP_ConnectRsp not in incoming state:%d", Chnl->conn.state);
            /* Already connected */
            status = BT_STATUS_BUSY;
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVCTP_Disconnect
 * DESCRIPTION
 *  Disconnect from the remote device.
 * PARAMETERS
 *  chnl        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVCTP_Disconnect(AvctpChannel *chnl)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_DISCONNECT_API_LOG );
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();

    /* See if the chnl is registered */
    if (IsNodeOnList(&AVCTP(chnlList), &chnl->node))
    {
        if (AvctpIsConnected(&chnl->conn))
        {
            /* Disconnect */
            status = AvctpDisconnect(&chnl->conn);
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
 *  AVCTP_SendCommand
 * DESCRIPTION
 *  Send a command to the target.
 * PARAMETERS
 *  chnl            [IN]        
 *  cmdFrame        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVCTP_SendCommand(AvctpChannel *chnl, AvctpCmdFrame *cmdFrame)
{
    BtStatus status = BT_STATUS_NOT_FOUND;

    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_SEND_COMMAND_API_LOG );
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();

    if (IsNodeOnList(&AVCTP(chnlList), &chnl->node))
    {
      
            chnl->curFrame = cmdFrame;
            chnl->cmdPacket.msgType = AVC_MSG_TYPE_COMMAND;
            chnl->cmdPacket.txId = SC_AV_REMOTE_CONTROL; /* 0x110E */
            chnl->cmdPacket.txIdSize = 2;
            chnl->cmdPacket.useMsgHdr = TRUE;
            chnl->cmdPacket.internal_used = 0;
            chnl->cmdPacket.msgHdr[0] = cmdFrame->ctype & 0x0f;
            chnl->cmdPacket.msgHdr[1] = cmdFrame->subunitType << 3;
            chnl->cmdPacket.msgHdr[1] += cmdFrame->subunitId;
            chnl->cmdPacket.msgHdr[2] = cmdFrame->opcode;
            chnl->cmdPacket.txDataLen = cmdFrame->operandLen;
            chnl->cmdPacket.txData = cmdFrame->operands; /* AV13 cmdFrame's operands is the AVRCP specifc cmd format*/
            OS_Report("[AVCTP] cmdFrame->ctype:%d cmdFrame->opcode:%d", cmdFrame->ctype, cmdFrame->opcode );

            status = AvctpMsgSendCommand(chnl); /* AV13 no timer for response */
        

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
 *  AVCTP_SendResponse
 * DESCRIPTION
 *  Send a response to the controller.
 * PARAMETERS
 *  chnl            [IN]        
 *  rspFrame        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVCTP_SendResponse(AvctpChannel *chnl, AvctpRspFrame *rspFrame)
{
    BtStatus status = BT_STATUS_FAILED;

    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_SEND_RESPONSE_LOG );
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();

    if (IsNodeOnList(&AVCTP(chnlList), &chnl->node))
    {
        if (rspFrame->response == AVCTP_RESPONSE_INTERIM && rspFrame->opcode != AVCTP_OPCODE_VENDOR_DEPENDENT)
        {
            status = BT_STATUS_FAILED;
        }
		else
        {
            if (rspFrame->response != AVCTP_RESPONSE_INTERIM)
            {
                chnl->state = AVCTP_CHAN_STATE_OPEN;
            }
            chnl->curFrame = rspFrame;
            chnl->rspPacket.msgType = AVC_MSG_TYPE_ACCEPT;
            chnl->rspPacket.txId = SC_AV_REMOTE_CONTROL;
            chnl->rspPacket.txIdSize = 2;
            chnl->rspPacket.useMsgHdr = TRUE;
            chnl->rspPacket.internal_used = 0;
            chnl->rspPacket.msgHdr[0] = rspFrame->response & 0x0f;
            chnl->rspPacket.msgHdr[1] = rspFrame->subunitType << 3;
            chnl->rspPacket.msgHdr[1] += rspFrame->subunitId;
            chnl->rspPacket.msgHdr[2] = rspFrame->opcode;
            chnl->rspPacket.txDataLen = rspFrame->operandLen;
            chnl->rspPacket.txData = rspFrame->operands;
            /* AV13 AVCTP user could assign the TransId for RegisterNotification feature */
            chnl->avcChnl.acpTransId = rspFrame->transId;

            status = AVC_SendStart(&chnl->avcChnl, &chnl->rspPacket);
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
 *  AVCTP_SendRawMessage
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl            [IN]        
 *  rawFrame        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus AVCTP_SendRawMessage(AvctpChannel *chnl, AvctpRawFrame *rawFrame)
{
    BtStatus status = BT_STATUS_FAILED;

    kal_trace( BT_TRACE_G8_AVRCP, BT_AVCTP_SEND_RAWMESSAGE_LOG );
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, chnl != 0);

    OS_LockStack();

    if (IsNodeOnList(&AVCTP(chnlList), &chnl->node))
    {
        chnl->curFrame = rawFrame;
        OS_Report("[AVCTP] rawFrame->type:%d", rawFrame->type);
        if (rawFrame->type == 0x01)
        {
            chnl->rspPacket.msgType = AVC_MSG_TYPE_COMMAND;
            chnl->avcChnl.intTransId = rawFrame->transaction_id;
            chnl->txState = AVCTP_MSG_TXSTATE_WAIT_RSP;
        }
        else
        {
            chnl->rspPacket.msgType = AVC_MSG_TYPE_ACCEPT;
            chnl->avcChnl.acpTransId = rawFrame->transaction_id;
        }
        chnl->rspPacket.txId = rawFrame->pid;
        chnl->rspPacket.txIdSize = 2;
        chnl->rspPacket.useMsgHdr = FALSE;
        chnl->rspPacket.internal_used = 0;
        chnl->rspPacket.txDataLen = rawFrame->length;
        chnl->rspPacket.txData = rawFrame->data;
        OS_Report("[AVCTP] txIdSize:%d rawFrame->length:%d", chnl->rspPacket.txIdSize, chnl->rspPacket.txDataLen );

        status = AVC_SendStart(&chnl->avcChnl, &chnl->rspPacket);
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
 *  AVCTP_RemoteDevice
 * DESCRIPTION
 *  Returns a pointer to the current remote device.
 * PARAMETERS
 *  chnl        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtRemoteDevice *AVCTP_RemoteDevice(AvctpChannel *chnl)
{
    BtRemoteDevice *remDev;

    if (AvctpIsConnected(&chnl->conn))
    {
        remDev = chnl->conn.remDev;
    }
    else
    {
        remDev = 0;
    }

    return remDev;
}

/*---------------------------------------------------------------------------
 * AvctpAlloc()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initializes the global context for the AV profile.  This 
 *            function can be replaced if necessary.
 *
 * Return:    void
 */


/*****************************************************************************
 * FUNCTION
 *  AvctpAlloc
 * DESCRIPTION
 *  Initializes the global context for the AV profile.
 *  This function can be replaced if necessary.
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BOOL AvctpAlloc(void)
{
    U8 *ptr;

    /* Fill memory with 0. */
#if XA_CONTEXT_PTR == XA_ENABLED
    ptr = (U8*) avctpContext;
#else 
    ptr = (U8*) & avctpContext;
#endif /* XA_CONTEXT_PTR */

    OS_MemSet(ptr, 0, (U32) sizeof(BtAvctpContext));

    return TRUE;
}


U16 AVCTP_GetSinglePacketMtu(AvctpChannel *chnl, BT_BOOL bMsgHdr){
    // AV13 return the l2cap - avctp header (3B) - A/V header(3B)
    U16 l2cap_mtu = 0;
   
    l2cap_mtu = ((L2CAP_GetTxMtu(chnl->avcChnl.l2ChannelId) < 48) ? (48) : (L2CAP_GetTxMtu(chnl->avcChnl.l2ChannelId)));
    if( 0 == l2cap_mtu ){
        // AVCTP 
        Assert(0);
    }
    return l2cap_mtu;
}
