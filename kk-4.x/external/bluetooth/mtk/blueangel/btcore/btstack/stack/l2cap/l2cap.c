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
 *     $Workfile:l2cap.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains the functions that comprise the
 *     L2CAP upper layer API. This includes group management,
 *     PSM registration and channel control functions.
 *
 * Created:
 *     July 15, 1999
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
#include "bt_feature.h"
#include "bt_common.h"
#include "btalloc.h"
#include "sys/l2cap_i.h"
#include "sys/mei.h"

/****************************************************************************
 *
 * Internal Function prototypes
 *
 ****************************************************************************/
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
BtStatus L2Cap_SendFeaturesReq(L2CAP_Channel *Channel, BtPacket *Request);
#endif 
BOOL L2Cap_IsConfigedOpen(L2CAP_Channel *Channel);

BtStatus L2Cap_SendSignalPacket(L2CAP_Channel *Channel, BtPacket *Packet, U8 priority);

/// Internal API
BtStatus L2CAP_CreateChannelRsp(U16 ChannelId, U16 ResponseCode, U16 ResponseStatus, L2capConnSettings *Settings);
BtStatus L2CAP_DisconnectHighPriorityReq(U16 ChannelId);
void L2cap_BLESignCallback(const BtEvent *event);
BtStatus L2Cap_SendConfigRspInternal(L2CAP_Channel *Channel, BtPacket *Rsp, U16 Result, L2capConfigFlags Flags);

void L2Cap_DumpHex(const char *prefix, const char *data, U32 datalen){
//#ifdef __L2CAP_CONFIG_DUMP__
	S8 strtemp[128];
	S8 hextemp[32];
	U32 i, j;

	/* print the raw data */
	sprintf( strtemp, "L: dump (%s) datalen:%u ", prefix, (unsigned int)datalen);
	OS_Report( strtemp );

	if( datalen == 0 ){
		return;
	}
	for( i = 0; i <  ((datalen/16)+1) ; i++){
		memset( strtemp, 0, sizeof(strtemp) );
		sprintf( strtemp, "[%02u] ", (unsigned int)i);
		for( j = 0; j < 16 ; j ++ ){
			if( i*16+j < datalen ){
				sprintf(hextemp, " 0x%02x", data[i*16+j]);
				strncat(strtemp, hextemp, sizeof(hextemp));
			}
		}
		if( strlen(strtemp) > 5){
	     	OS_Report( strtemp );
		}
	}
//#endif
}
/****************************************************************************
 *
 * L2CAP Upper Layer API
 *
 ****************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  L2CAP_RegisterPsm
 * DESCRIPTION
 *  Used to register a protocol/service multiplexor with the L2CAP
 *  protocol. The callback function registered here receives all
 *  event notifications from L2CAP pertaining to the registered
 *  protocol/service PSM value.
 * PARAMETERS
 *  Protocol        [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS  - The protocol/service was successfully
 *  registered with L2CAP.
 *  BT_STATUS_FAILED   - The 'Psm' parameter was invalid.
 *  BT_STATUS_NO_RESOUR- The max number of PSM's has been reached.
 *  BT_STATUS_IN_USE   - The Protocol struct is already registered.
 *****************************************************************************/
BtStatus L2CAP_RegisterPsm(L2capPsm *Protocol)
{
    BtStatus status;
    U16 p;

    OS_LockStack();

    if (!Protocol || !((Protocol->psm == BT_CLIENT_ONLY_PSM) || (Protocol->psm & 0x0001)))
    {
        status = BT_STATUS_FAILED;
        goto Exit;
    }
    if (L2Cap_IsValidProtocol(Protocol))
    {
        status = BT_STATUS_IN_USE;
        goto Exit;
    }
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    Report(("Protocol->inLinkMode:%d",Protocol->inLinkMode));
    Report(("Protocol->outLinkMode:%d",Protocol->outLinkMode));
    if((Protocol->inLinkMode ==0) && (Protocol->outLinkMode ==0))
    {
        
        Protocol->inLinkMode = L2MODE_BASIC;
        Protocol->outLinkMode = L2MODE_BASIC;
    }
    if ((Protocol->inTransmitMax < 1) && (Protocol->inLinkMode != L2MODE_BASIC))
    {
        status = BT_STATUS_INVALID_PARM;
        goto Exit;
    }
    if((Protocol->inTransmitMax == 0xFF) && (Protocol->inLinkMode == L2MODE_ENHANCEDMENT_RETRANSMISSION))
    {
        /* Infinite retransmitted */
        Protocol->inTransmitMax = 0x00;
    }
    if(((Protocol->inLinkMode & L2MODE_ENHANCEDMENT_RETRANSMISSION) !=0) &&
        (Protocol->op_state == L2OPERATORSTATE_STATE0))
    {
        // Assert(0); //UPF
    }
    if (Protocol->inLinkMode != L2MODE_BASIC)
    {
        if ((Protocol->inBufferSize < Protocol->localMtu) || (Protocol->localMtu > L2CAP_MAXIMUM_MTU_V12))
        {
            status = BT_STATUS_INVALID_PARM;
            goto Exit;
        }
    }
    else
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */
    if (Protocol->localMtu > L2CAP_MTU)
    {
        status = BT_STATUS_FAILED;
        goto Exit;
    }

    /* Find free slot for PSM */
    for (p = 0; p < L2CAP_NUM_PROTOCOLS; p++)
    {
        if (L2C(protocols)[p] == 0)
        {
            L2C(protocols)[p] = Protocol;

            /* Assign a dynamic PSM if necessary */
            if (Protocol->psm == BT_DYNAMIC_PSM)
            {
                Protocol->psm = (((p & 0x3F80) << 2) | ((p & 0x007F) << 1) | 0x1001);
            }

            bt_trace(TRACE_GROUP_1, L2CAP_PSM_REGISTERED_BT_DEBUGx04X, Protocol->psm);
            status = BT_STATUS_SUCCESS;
            goto Exit;
        }
    }
    status = BT_STATUS_NO_RESOURCES;

  Exit:
    OS_UnlockStack();
    return status;
}



BtStatus L2CAP_InitDefaultPSM(L2capPsm *Protocol, L2capLinkMode mode){
    BtStatus status = BT_STATUS_FAILED;
    OS_MemSet((U8 *)Protocol, 1, sizeof(L2capPsm));

#ifdef __BT_2_1_ERTM__    
    /* No need to do lockconfigure */
    Protocol->lockStepBRNeeded = 0;
    Protocol->lockStepNeeded = 0;  
    Protocol->highSpeedRequired = 0;
    
    Protocol->l2capExtFlowSpec.identifier = 0x01;
    Protocol->l2capExtFlowSpec.serviceType = 0x01;
    Protocol->l2capExtFlowSpec.maxSduSize = 670;
    Protocol->l2capExtFlowSpec.sduIntArrialTime = 0xFFFFFFFF;
    Protocol->l2capExtFlowSpec.accessLatency = 0xFFFFFFFF;
    Protocol->l2capExtFlowSpec.flushTimeout = 0xFFFFFFFF;        
#endif    
    status = BT_STATUS_SUCCESS;
    return status;
}

void L2CAP_InitPacketData(BtPacket *packet){
    packet->priority_type = 0;
    
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    packet->segStart = 0;
    packet->segNum = 0;
    packet->internalUsed = 0;
    packet->pollFinal = 0;
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */
}

#if L2CAP_DEREGISTER_FUNC == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  L2CAP_DeregisterPsm
 * DESCRIPTION
 *  Used to deregister a protocol/service multiplexor registered
 *  with the L2CAP protocol. The 'Protocol' parameter is the same
 *  as the one passed to the register function.
 * PARAMETERS
 *  Protocol        [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS  - The protocol/service was successfully
 *  deregistered from L2CAP.
 *  BT_STATUS_FAILED   - The 'Protocol' parameter is invalid.
 *  BT_STATUS_NOT_FOUND - The 'Protocol' is not registered with L2CAP.
 *****************************************************************************/
BtStatus L2CAP_DeregisterPsm(const L2capPsm *Protocol)
{
    I16 p, c;

    if (Protocol == 0)
    {
        return BT_STATUS_FAILED;
    }

    OS_LockStack();
    /* Find the protocol in the PSM table */
    for (p = 0; p < L2CAP_NUM_PROTOCOLS; p++)
    {
        if (L2C(protocols)[p] == Protocol)
        {

            /* See if any it has any active channels */
            for (c = 0; c < L2CAP_NUM_CHANNELS; c++)
            {
                if ((L2C(channels)[c].state != L2C_CLOSED) && (L2C(channels)[c].psmInfo == Protocol))
                {
                    /* Cannot deregister a PSM with active channels */
                    Report(("l2cap index:%d status:%d return busy!", c, L2C(channels)[c].state));
                    OS_UnlockStack();
                    return BT_STATUS_BUSY;
                }
            }

            L2C(protocols)[p] = 0;
            OS_UnlockStack();
            return BT_STATUS_SUCCESS;
        }
    }
    OS_UnlockStack();
    return BT_STATUS_NOT_FOUND;
}
#endif /* L2CAP_DEREGISTER_FUNC == XA_ENABLED */

#ifdef __BT_2_1_ERTM__
BtStatus L2CAP_AllocateFixChannel(const L2capPsm *Protocol, 
										BtRemoteDevice *link, 
										L2capConnSettings *Settings,
										U16 *localCid)
{
	L2CAP_Channel *channel;
	U16 reason;
	
	channel = L2Cap_AllocPsmChannel(Protocol->psm, &reason);
	Report(("L2CAP_AllocateFixChannel PSM:%04x", Protocol->psm));
	if (channel)
	{
		if(Protocol->psm == BT_PSM_A2MP) /* AMP Manager Protocol*/
		{
			channel->remoteIdent = 0x00;
			channel->link = link;
			channel->remoteCid = 0x03;
			channel->freeSduBuffer = Settings->reasmBuffer;
			channel->rxSduBuffer = Settings->receivedBuffer;
			channel->rxWindowSize = Settings->rxWindowSize;
			channel->txIframeCount = Settings->txMaxTransmitCount; 
			channel->txSrejList = (U32 *)Settings->txSrejList;
			channel->enhancedMaxMissingSrejNo = Settings->maxSrejNo;
			channel->state = L2C_OPEN;
			channel->txWindow = 1;
			channel->transmitMax = 0xff;
			channel->hciAMPHandler = 0;
			channel->retransTimeout = 2000;
			channel->monitorTimeout = 12000;
			channel->AMPInfo.flushTimeOut = 0xFFFFFFFF;
			channel->LastConfigStateRejectCount = 0;
            Report(("A2MP Dlight a1:%d",channel->LastConfigStateRejectCount));
			channel->inLinkMode = LINK_MODE_ENHANCED_RETRANSMISSION;
			channel->inFcsUsed = TRUE;
			channel->outLinkMode = LINK_MODE_ENHANCED_RETRANSMISSION;
			channel->outMpu = 670;			
			channel->outMtu = 670;
			channel->outFcsUsed = TRUE;
			channel->supportExtControl = FALSE;
            channel->infoReqDone = 1;
            *localCid = channel->localCid;
		}
		else
		{
			/* Invalid PSM for Fix Channel */
			Assert(0);
		}
	}
	else
	{
		return BT_STATUS_FAILED;
	}
    return BT_STATUS_SUCCESS;
}


void L2CAP_FreeFixChannel(L2capChannelId id)
{
	L2CAP_Channel *channel;
	
	channel = &L2C(channels)[CID_TO_INDEX(id)];
	
	L2Cap_FreeChannel(channel);

}
#endif

/*****************************************************************************
 * FUNCTION
 *  L2CAP_ConnectReq
 * DESCRIPTION
 *  Send an L2CAP Connect request to the specified device and PSM.
 * PARAMETERS
 *  Protocol        [IN]        
 *  Psm             [IN]        
 *  Device          [IN]        
 *  Settings        [IN]        
 *  LocalCid        [IN]        
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_ConnectReq(
            const L2capPsm *Protocol,
            U16 Psm,
            BtRemoteDevice *Device,
            L2capConnSettings *Settings,
            U16 *LocalCid)
{
    L2CAP_Channel *new_channel;
    BtStatus status;

    OS_LockStack();

    if (!L2Cap_IsValidProtocol(Protocol))
    {
        OS_UnlockStack();
        return BT_STATUS_INVALID_PARM;
    }
    if ((new_channel = L2Cap_AllocLinkChannel(Protocol, Device, &status)) != 0)
    {
        /* Finish setup of channel and post transmit request */
        new_channel->state = L2C_W4_CONN_CNF;
        new_channel->remotePsm = Psm;
#ifdef __BT_2_1_ERTM__            
        OS_Report("l2cap: remote psm:0x%x outMode:%d inMode:%d", Psm, Protocol->outLinkMode, Protocol->inLinkMode);
#endif

    #if BT_SECURITY == XA_ENABLED
        /*
         * Query the Security Manager to see if this protocol is
         * authorized to establish an outgoing connection.
         */
        new_channel->authorization.remDev = Device;
        new_channel->authorization.channel = Protocol->psm;
        new_channel->authorization.incoming = FALSE;
        new_channel->authorization.id = SEC_L2CAP_ID;

        status = SEC_AccessRequest(&new_channel->authorization);
        if (status == BT_STATUS_PENDING)
        {
            /*
             * Return pending for now. We will continue when the security 
             * authorization is completed. 
             */
            new_channel->flags |= L2CHF_SEC_PENDING;
        }
        else if (status != BT_STATUS_SUCCESS)
        {
            status = BT_STATUS_RESTRICTED;
        }
    #endif /* BT_SECURITY == XA_ENABLED */

        if (status == BT_STATUS_SUCCESS)
        {
            /* Security okay, send the packet */
            status = L2Cap_SendConnReqInternal(new_channel);
        }

#ifdef __BT_2_1_ERTM__            
        OS_Report("L2Cap: ConnectReq r-psm:0x%x outMode:%d inMode:%d", Psm, Protocol->outLinkMode, Protocol->inLinkMode);
#endif

        if (status == BT_STATUS_PENDING)
        {
            *LocalCid = new_channel->localCid;
            Report(("Allocate local channel cid:%04x",*LocalCid));
        }
        else
        {
            L2Cap_FreeChannel(new_channel);
        }
    }
    OS_UnlockStack();
    return status;
}

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
void L2CAP_ConnectReqAssignFlowControlBuffer(
            L2capConnSettings *Settings,
            U16 ChannelId)
{
    L2CAP_Channel *channel;

    /* for the client connect request, 
       The RFCOMM buffer is allocated after the register PSM to L2CAP,
       so the dataBuffer shall associate with the ChannelId again */
    channel = L2Cap_GetValidChannel(ChannelId);
    if(channel !=0)
    {
        channel->freeSduBuffer = Settings->reasmBuffer;
        channel->rxSduBuffer = Settings->receivedBuffer;
        channel->rxWindowSize = Settings->rxWindowSize;
        channel->txIframeCount = Settings->txMaxTransmitCount; 
		channel->enhancedMaxMissingSrejNo = Settings->maxSrejNo;
		channel->txSrejList = (U32 *)Settings->txSrejList;
		OS_Report("l2cap rxWindowSize:%d", Settings->rxWindowSize);
    }
    else
        Assert(0);
}

#endif
/*****************************************************************************
 * FUNCTION
 *  L2CAP_ConnectRsp
 * DESCRIPTION
 *  Send a Response to a L2CAP Connect Request.
 * PARAMETERS
 *  ChannelId           [IN]        
 *  ResponseCode        [IN]        
 *  Settings            [IN]        
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_ConnectRsp(U16 ChannelId, U16 ResponseCode, L2capConnSettings *Settings)
{
    L2CAP_Channel *channel;
    BtStatus status;

    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);
    if(ResponseCode == L2CONN_ACCEPTED)
    {
        Report(("l2cap: Upper accept"));
    }else{
        Report(("l2cap: Upper reject"));
    }

#ifdef __BT_3_0_HS__
    if(( channel != 0) && channel->state == L2C_W4_CREATE ){
        status = L2CAP_CreateChannelRsp(ChannelId, ResponseCode, L2CONN_ACCEPTED, Settings);
        OS_Report("l2cap: pass to createchannel status:%d", status);
        return status;
    }
#endif
    if ((channel == 0) || (channel->state != L2C_W4_CONN_RSP) || (ResponseCode > L2CONN_REJECT_NO_RESOURCES))
    {
        OS_UnlockStack();
        return BT_STATUS_FAILED;
    }
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    if(ResponseCode <= L2CONN_PENDING)
    {
        if ((channel->psmInfo->inLinkMode != L2MODE_BASIC) && (!Settings || !Settings->reasmBuffer))
    {
        OS_Report("[L2CAP][ERR] wrong state at l2cap.c #455");
        return BT_STATUS_FAILED;
    }
    }
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */

    /* Make sure the user doesn't try send the same response twice. */
    if ((channel->flags & L2CHF_CONN_RSP_SENT) &&
        ((channel->result != L2CONN_PENDING) || (ResponseCode == L2CONN_PENDING)))
    {
        OS_UnlockStack();
        return BT_STATUS_IN_PROGRESS;
    }
    channel->result = ResponseCode;
    channel->flags |= L2CHF_CONN_RSP_SENT;

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    if (Settings)
    {
        /* Only the flow control mode will enable the Settings for Connect Response */
        channel->freeSduBuffer = Settings->reasmBuffer;
        channel->rxSduBuffer = Settings->receivedBuffer;
        channel->rxWindowSize = Settings->rxWindowSize;
        channel->txIframeCount = Settings->txMaxTransmitCount; 
		channel->txSrejList = (U32 *)Settings->txSrejList;
		channel->enhancedMaxMissingSrejNo = Settings->maxSrejNo;
    }
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */

    status = L2Cap_SendConnRspInternal(channel);

    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_AuthorizeConnectRsp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ChannelId       [IN]        
 *  result          [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2Cap_AuthorizeConnectRsp(U16 ChannelId, BOOL result)
{
    L2CAP_Channel *channel;

    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);

#if XA_ERROR_CHECK == XA_ENABLED
    if ((channel == 0) || (channel->state != L2C_W4_CONN_RSP))
    {
        OS_UnlockStack();
        return BT_STATUS_FAILED;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    channel->flags &= ~L2CHF_SEC_PENDING;

    if (result)
    {
        L2Cap_StateMachine(channel, LLC_ACCESS_ACCEPT);
    }
    else
    {
        L2Cap_StateMachine(channel, LLC_ACCESS_REJ);
    }
    return BT_STATUS_SUCCESS;

}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_DisconnectReq
 * DESCRIPTION
 *  Disconect a L2CAP channel.
 * PARAMETERS
 *  ChannelId       [IN]        
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_DisconnectReq(U16 ChannelId)
{
    L2CAP_Channel *channel;
    BtStatus status;

    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);

    if ((channel == 0) || (channel->state < L2C_W4_CONN_CNF) ||
        ((channel->state == L2C_W4_CONN_RSP) && ((channel->flags & L2CHF_CONN_RSP_SENT) == 0)) ||
        (channel->remoteCid == L2CID_INVALID))
    {
        status = BT_STATUS_FAILED;
        Report(("Error 1"));
        goto Done;
    }
    if (channel->state > L2C_OPEN && channel->state < L2C_W4_ECHO_CNF)
    {
        status = BT_STATUS_IN_PROGRESS;
        Report(("Error 2 state:%d", channel->state)); // ALPS00248749 movechannel and upper layer ask disconect
        goto Done;
    }

    OS_Report("l2Cap: DisconnectReq cid:%d", ChannelId);

    /* Set the disconnect callback reason */
    L2C(callbackParms).status = L2DISC_USER_REQUEST;

    status = L2Cap_SendDiscReqInternal(channel);

#if XA_ERROR_CHECK == XA_ENABLED
  Done:
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_DisconnectHighPriorityReq
 * DESCRIPTION
 *  Disconect a L2CAP channel.
 * PARAMETERS
 *  ChannelId       [IN]        
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_DisconnectHighPriorityReq(U16 ChannelId)
{
    L2CAP_Channel *channel;
    BtStatus status = BT_STATUS_FAILED;

    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);

    if ((channel == 0) || (channel->state < L2C_W4_CONN_CNF) ||
        ((channel->state == L2C_W4_CONN_RSP) && ((channel->flags & L2CHF_CONN_RSP_SENT) == 0)) ||
        (channel->remoteCid == L2CID_INVALID))
    {
        status = BT_STATUS_FAILED;
        Report(("Error 1"));
        goto Done;
    }
    if (channel->state > L2C_OPEN)
    {
        status = BT_STATUS_IN_PROGRESS;
        Report(("Error 2"));
        goto Done;
    }

    /* Set the disconnect callback reason */
    L2C(callbackParms).status = L2DISC_USER_REQUEST;

    //status = L2Cap_SendDiscReqHighInternal(channel);

#if XA_ERROR_CHECK == XA_ENABLED
  Done:
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    OS_UnlockStack();
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  L2CAP_SendData
 * DESCRIPTION
 *  Send Data over an L2CAP channel. Support both connection
 *  oriented and group channels.
 * PARAMETERS
 *  ChannelId       [IN]        
 *  Packet          [IN]        
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_SendData(L2capChannelId ChannelId, BtPacket *Packet)
{
    L2CAP_Channel *channel;
    BtStatus status;

#if L2CAP_NUM_GROUPS > 0
    L2capGroup *group;
#endif 
    OS_LockStack();

#if L2CAP_NUM_GROUPS > 0
    if (!L2Cap_GetValidChannel(ChannelId) && !L2Cap_GetValidGroup(ChannelId))
    {
#else /* L2CAP_NUM_GROUPS > 0 */
    if (!L2Cap_GetValidChannel(ChannelId))
    {
#endif /* L2CAP_NUM_GROUPS > 0 */
        OS_UnlockStack();
        return BT_STATUS_FAILED;
    }

    /* Make sure no private flags are set */
    if (Packet->flags & ~(BTP_FLAG_TAIL | BTP_FLAG_INUSE))
    {
        OS_UnlockStack();
        return BT_STATUS_FAILED;
    }

    /* Is this a Group channel? */
#if L2CAP_NUM_GROUPS > 0
    if (ChannelId >= BASE_GROUP_CID)
    {
        group = L2C(groups)[GROUP_CID_TO_INDEX(ChannelId)];

        /* Could check what the signalling MTU is using an InfoReq */
        status = L2Cap_GroupSend(group, Packet);
    }
    else
#endif /* L2CAP_NUM_GROUPS > 0 */
    {
        /* It's a connection oriented channel */
        channel = &L2C(channels)[CID_TO_INDEX(ChannelId)];

        //if ((channel->state != L2C_OPEN) && (channel->state < L2C_W4_CREATE))
        if ( FALSE == L2Cap_IsConfigedOpen(channel) )
        {
            OS_UnlockStack();
            OS_Report("L2CAP_SendData state:%d return:%d", L2C(channels)[CID_TO_INDEX(ChannelId)].state, BT_STATUS_FAILED);
            return BT_STATUS_FAILED;
        }
        status = L2Cap_Send(channel, Packet, channel->remoteCid);
    }
    OS_UnlockStack();
    return status;
}

#ifdef __BT_3_0_UNICAST__
/*****************************************************************************
 * FUNCTION
 *  L2CAP_SendUnicastData
 * DESCRIPTION
 *  Send Data over an L2CAP channel. Support both connection
 *  oriented and group channels.
 * PARAMETERS
 *  ChannelId       [IN]        
 *  Packet          [IN]        
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_SendUnicastData(const L2capPsm *Protocol, BtRemoteDevice *link, BtPacket *Packet)
{
    L2CAP_Channel *channel = 0;
    BtStatus status;
    I16 i;
    U8 channel_find = 0;
    
    OS_LockStack();

    for (i = 0; i < L2CAP_NUM_CHANNELS; i++)
    {

        if (L2C(channels)[i].localCid != L2CID_INVALID)
        {
            if(L2C(channels)[i].link == link)
            {
                channel = &L2C(channels)[i];
                Report(("L2CAP channel allocated before."));
                channel_find = 1;
                break;
            }
        }
    }
    if(i == L2CAP_NUM_CHANNELS)
    {

        if ((channel = L2Cap_AllocLinkChannel(Protocol, link, &status)) != 0)
        {
            Report(("Try to allocate L2CAP channel"));
            /* Finish setup of channel and post transmit request */
            channel->state = L2C_CLOSED;
            channel->remotePsm = Protocol->psm;
        }
    }
    status = L2Cap_Send(channel, Packet, 0x0002);
    if(status == BT_STATUS_PENDING)
    {
        if(channel_find ==0)
            L2Cap_StartErtx(channel);
    }
    else
    {
        L2Cap_FreeChannel(channel);
    }
    OS_UnlockStack();
    return status;
}
#endif

BtStatus L2CAP_SendFixChannelData(U16 local_cid, U16 remote_cid, BtRemoteDevice *link, BtPacket *Packet)
{
    L2CAP_Channel *channel = 0;
    BtStatus status;
    
    OS_LockStack();

	channel = &L2C(channels)[CID_TO_INDEX(local_cid)];
	
	if (channel->state != L2C_OPEN)
	{
		OS_UnlockStack();
		return BT_STATUS_FAILED;
	}
    Packet->headerLen = 0;
    Packet->tailLen = 0;
	status = L2Cap_Send(channel, Packet, remote_cid);

    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_AvSendData
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ChannelId       [IN]        
 *  Packet          [IN]
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2CAP_AvSendData(L2capChannelId ChannelId, BtPacket *Packet)
{
    L2CAP_Channel *channel;
    I16 packetLen;

    if( ChannelId < BASE_DYNAMIC_CID || CID_TO_INDEX(ChannelId) > L2CAP_NUM_CHANNELS ){
        // out of range
        return BT_STATUS_NO_CONNECTION;
    }
    
    channel = &L2C(channels)[CID_TO_INDEX(ChannelId)];
    XASTAT_TimerStart(l2cap_tx_timer, Packet->l2cap_timer);

    packetLen = Packet->dataLen + Packet->headerLen + Packet->tailLen;
    if (channel->link->state != BDS_CONNECTED)
    {
        return BT_STATUS_NO_CONNECTION;
    }
    Packet->llpContext = channel->localCid;
    Packet->remoteCid = channel->remoteCid;
    channel->l2capTxCount++;
    if (L2C(highPriorityNo) < 0xFFFFFFFF)
    {
        L2C(highPriorityNo)++;
    }
    InsertTailList(&(channel->link->txHighPriorityQueue), &(Packet->node));

#ifdef __BT_2_1_ERTM__
    if( Packet->dataLen  > channel->outMtu){
        OS_Report("[WRN] AvSendData (%d)(%d)(%d) > localMpu:%d", Packet->dataLen , Packet->headerLen , Packet->tailLen , channel->outMtu );
    }
#endif    
    
    if (channel->link->state == BDS_CONNECTED)
    {
        if (channel->link->mode != BLM_ACTIVE_MODE)
        {
            /* Alert the ME that we have data to send. */
            MeHandleDataReqWhileNotActive(channel->link);
        }

        /* Need both checks to cover MeSuspendTransmitters logic too. */
        if ((channel->link->mode != BLM_HOLD_MODE) && (channel->link->okToSend))
        {
            AssertEval(HCI_RequestToSend(channel->link->hciHandle) == BT_STATUS_SUCCESS);
        }
    }
    return BT_STATUS_PENDING;
}


#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
BtStatus L2CAP_SendLockStepConfigReq(L2capChannelId ChannelId)
{
    L2CAP_Channel *channel;
    BtStatus status = BT_STATUS_FAILED;

    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);
    if ((channel == 0) || (channel->state != L2C_OPEN))
    {
        OS_UnlockStack();
        return BT_STATUS_FAILED;
    }
    if ((channel->state == L2C_CONFIG) && (channel->flags & (L2CHF_OUT_CONFIG_OK | L2CHF_CONFIG_REQ_SENT)))
    {
        OS_UnlockStack();
        return BT_STATUS_IN_PROGRESS;
    }

    if (channel->state == L2C_OPEN)
    {
        channel->reConfigState = TRUE;
        channel->lockstepRxPendingDone = FALSE;
        channel->lockstepRxPendingDone = FALSE;
        channel->lockstepNegoDone = FALSE;
        channel->state = L2C_CONFIG;
        channel->flags &= ~(L2CHF_IN_CONFIG_OK | L2CHF_OUT_CONFIG_OK);
    }
    status = SendAutoLockStepConfigReq(channel);

    OS_UnlockStack();
    return status;
}

BtStatus L2CAP_SendLockStepConfigRsp(L2capChannelId ChannelId, U16 result)
{
    L2CAP_Channel *channel;
    BtStatus status = BT_STATUS_FAILED;
    U16 result_code;
    
    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);
    if ((channel == 0) || (channel->state != L2C_CONFIG))
    {
        OS_UnlockStack();
        return BT_STATUS_FAILED;
    }
    if(result == BT_STATUS_SUCCESS)
        result_code = L2CFG_RESULT_SUCCESS;
    else
        result_code = L2CFG_RESULT_REJECTED;
    status = SendLockStepConfigRsp(channel, result);

    OS_UnlockStack();
    return status;
}

#endif



/*****************************************************************************
 * FUNCTION
 *  L2CAP_SendConfigReq
 * DESCRIPTION
 *  
 *  Sends a Configuration request packet.
 * PARAMETERS
 *  ChannelId       [IN]        
 *  Req             [IN]        
 *  Flags           [IN]        
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_SendConfigReq(L2capChannelId ChannelId, BtPacket *Req, L2capConfigFlags Flags)
{
    L2CAP_Channel *channel;
    BtStatus status;

    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);

    if( (channel == 0) || (Req == 0) || (channel->state != L2C_OPEN) ){
        if( channel != 0 ){
            OS_Report("wrn check3 flags:0x%x state:%d", (channel->flags & L2CHF_AUTO_CONFIG), (channel->state));
        }
        OS_UnlockStack();
        return BT_STATUS_IN_PROGRESS;
    }

    OS_Report("l2cap: SendConfigReq state:%d flags:0x%x txcount:%d", channel->state, channel->flags, channel->l2capTxCount);
    if ((channel->state == L2C_CONFIG) && (channel->flags & (L2CHF_OUT_CONFIG_OK | L2CHF_CONFIG_REQ_SENT)))
    {
        if( channel != 0 ){
            OS_Report("wrn check2 flags:0x%x state:%d", (channel->flags & L2CHF_AUTO_CONFIG), (channel->state));
        }
        OS_UnlockStack();
        return BT_STATUS_IN_PROGRESS;
    }

    /* If were open, then were restarting the whole configuration. */
    if (channel->state == L2C_OPEN)
    {
#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
        channel->reConfigState = TRUE;
        channel->lockstepRxPendingDone = FALSE;
        channel->lockstepRxPendingDone = FALSE;
        channel->lockstepNegoDone = FALSE;
#endif
        channel->state = L2C_CONFIG;
        channel->flags &= ~(L2CHF_IN_CONFIG_OK | L2CHF_OUT_CONFIG_OK);
    }

    status = L2Cap_SendConfigReqInternal(channel, Req, Flags);

    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_SendConfigRsp
 * DESCRIPTION
 *  Sends a Configuration response packet.
 * PARAMETERS
 *  ChannelId       [IN]        
 *  Resp            [IN]        
 *  Result          [IN]        
 *  Flags           [IN]        
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_IN_PROGRESS - A config response has already been sent.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_SendConfigRsp(L2capChannelId ChannelId, BtPacket *Resp, U16 Result, L2capConfigFlags Flags)
{
    L2CAP_Channel *channel;
    BtStatus status;

    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);

    if ((channel == 0) || (channel->flags & L2CHF_AUTO_CONFIG) || (channel->state != L2C_CONFIG))
    {
        OS_UnlockStack();
        return BT_STATUS_FAILED;
    }

    if (channel->flags & L2CHF_IN_CONFIG_OK)
    {
        OS_UnlockStack();
        return BT_STATUS_IN_PROGRESS;
    }

    status = L2Cap_SendConfigRspInternal(channel, Resp, Result, Flags);
    OS_UnlockStack();
    return status;
}


void L2Cap_ReturnSysPkt(BtPacket *packet){
    if( NULL == packet ){
        return;
    }
    ReturnSysPkt(packet);
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_SetAutoConfigMode
 * DESCRIPTION
 *  Enables/Disables Auto-Configuration mode.
 * PARAMETERS
 *  ChannelId       [IN]        
 *  On              [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2CAP_SetAutoConfigMode(L2capChannelId ChannelId, BOOL On)
{
    L2CAP_Channel *channel;

    OS_LockStack();

    if (0 != (channel = L2Cap_GetValidChannel(ChannelId)))
    {
        if (On)
        {
            channel->flags |= L2CHF_AUTO_CONFIG;
        }
        else
        {
            channel->flags &= ~L2CHF_AUTO_CONFIG;
        }
    }
    OS_UnlockStack();
}




/*****************************************************************************
 * FUNCTION
 *  L2CAP_GetConnectionMode
 * DESCRIPTION
 *  Returns the selected link mode for the connected channel.
 * PARAMETERS
 *  ChannelId       [IN]        
 *  In              [IN]        
 * RETURNS
 *  
 *****************************************************************************/
L2capLinkMode L2CAP_GetConnectionMode(L2capChannelId ChannelId, BOOL In)
{
    L2capLinkMode mode;

    OS_LockStack();
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    if ((!L2Cap_GetValidChannel(ChannelId)) || (L2C(channels)[CID_TO_INDEX(ChannelId)].state != L2C_OPEN))
    {
        OS_UnlockStack();
        return 0;
    }

    if (In)
    {
        mode = (1 << L2C(channels)[CID_TO_INDEX(ChannelId)].inLinkMode);
    }
    else
    {
        mode = (1 << L2C(channels)[CID_TO_INDEX(ChannelId)].outLinkMode);
    }
#else
    mode = LINK_MODE_BASIC;
#endif    

    OS_UnlockStack();
    return mode;
}

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED

/*****************************************************************************
 * FUNCTION
 *  L2CAP_UpdateSduDataBufferSeq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Cid             [IN]        
 *  NumBytes        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2CAP_UpdateSduDataBufferSeq(L2capChannelId Cid)
{
    L2CAP_Channel *channel;
    BtRxPacket *tempPacket = 0;

    OS_LockStack();

    channel = L2Cap_GetValidChannel(Cid);
    if (!channel || (channel->inLinkMode == LINK_MODE_BASIC))
    {
        OS_UnlockStack();
        return BT_STATUS_INVALID_PARM;
    }
    if (IsListEmpty(channel->rxSduBuffer))
    {
        channel->buffSeq = channel->recvSeq;
    }
    else
    {
        tempPacket = (BtRxPacket *)RemoveHeadList(channel->rxSduBuffer);
        /* It is also possible that the channel->buffSeq >= receive reqSeq */
        /* when we move channel->buffSeq advanced one. */
        if(channel->buffSeq < tempPacket->reqSeq)
            channel->buffSeq = tempPacket->reqSeq;
        InsertHeadList(channel->rxSduBuffer, &tempPacket->node);
    }
    if((channel->inLinkMode != LINK_MODE_STREAMMING) &&
       (channel->inLinkMode != LINK_MODE_ENHANCED_RETRANSMISSION)) 
        L2Cap_SendSFrame(channel);
    OS_UnlockStack();
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_SetRetransDisableFlag
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ChannelId       [IN]        
 *  Disable         [IN]        
 * RETURNS
 *  Returns the selected link mode for the connected channel.
 *****************************************************************************/
BtStatus L2CAP_SetRetransDisableFlag(L2capChannelId ChannelId, BOOL Disable)
{
    U16 flags;
    L2CAP_Channel *channel = L2Cap_GetValidChannel(ChannelId);

    OS_LockStack();

    if ((channel == 0) || (channel->state != L2C_OPEN) || (channel->inLinkMode != LINK_MODE_RETRANSMISSION))
    {
        OS_UnlockStack();
        return BT_STATUS_FAILED;
    }

    /* Save current flags */
    flags = channel->flags2;

    if (Disable)
    {
        channel->flags2 |= L2CHF2_NO_RETRANS_IN;
    }
    else
    {
        channel->flags2 &= ~L2CHF2_NO_RETRANS_IN;
    }

    if ((flags != channel->flags2) && ((flags & L2CHF2_SEND_RR) == 0))
    {
        /* 
        (flags != channel->flags2)
        It means that the RetransDisableFlag is different from previous one.
        (flags & L2CHF2_SEND_RR) == 0 -> current send frame is not RR.
        Then send Sframe with Receive Ready.
        */
        /* Send flag change to remote peer. */
        channel->flags2 |= L2CHF2_SEND_RR;
        L2Cap_SendSFrame(channel);
    }

    OS_UnlockStack();
    return BT_STATUS_SUCCESS;
}

#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */

/*****************************************************************************
 * FUNCTION
 *  L2Cap_IsConfigedOpen
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL L2Cap_IsConfigedOpen(L2CAP_Channel *Channel){
    if(  0 == Channel ){
        return FALSE;
    }
    switch(Channel->state){
        case L2C_OPEN:
        case L2C_W4_MOVE:
        case L2C_W4_MOVE_RSP:
        case L2C_W4_MOVE_CNF:
        case L2C_W4_MOVE_CNF_RSP:
        return TRUE;
        case L2C_CONFIG:
        if( Channel->flags & L2CHF_CONNECTED ){
            OS_Report("l2cap reconfig state flags:0x%x", Channel->flags);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

/*****************************************************************************
 * FUNCTION
 *  L2CAP_GetTxMtu
 * DESCRIPTION
 *  Retrieves the channels transmit MTU.
 * PARAMETERS
 *  ChannelId       [IN]        
 * RETURNS
 *  U16 - Tramsmit MTU.
 *****************************************************************************/
U16 L2CAP_GetTxMtu(U16 ChannelId)
{
    U16 mtu;

    OS_LockStack();

    if( CID_TO_INDEX(ChannelId) >= L2CAP_NUM_CHANNELS ){
        // out of range
        OS_Report("l2cap Cid out-of-range cid:0x%x", ChannelId);
        return 0;
    }
    //OS_Report("l2cap Cid cid:0x%x", ChannelId);

    if ( NULL != L2Cap_GetValidChannel(ChannelId) && L2C(channels)[CID_TO_INDEX(ChannelId)].state == L2C_CONFIG){
        // it is in re-config or config state
        OS_Report("L2CAP_GetTxMtu state:%d return:%d", L2C(channels)[CID_TO_INDEX(ChannelId)].state, L2CAP_DEFAULT_MTU);
        OS_UnlockStack();
        return L2CAP_DEFAULT_MTU;
    }


    if ((!L2Cap_GetValidChannel(ChannelId)) || ( TRUE != L2Cap_IsConfigedOpen(&L2C(channels)[CID_TO_INDEX(ChannelId)])) )
    {
        if( CID_TO_INDEX(ChannelId) >= 0 ){
        OS_Report("L: L2CAP_GetTxMtu state:%d", L2C(channels)[CID_TO_INDEX(ChannelId)].state);
        }
        OS_UnlockStack();
        return 0;
    }

    mtu = L2C(channels)[CID_TO_INDEX(ChannelId)].outMtu;

    OS_UnlockStack();
    return mtu;
}

#if L2CAP_PING_SUPPORT == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  L2CAP_Ping
 * DESCRIPTION
 *  Sends an L2CAP Echo request packet to the specified device.
 * PARAMETERS
 *  PingClient      [IN]        
 *  Device          [IN]        
 *  Data            [IN]        
 *  DataLen         [IN]        
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_Ping(const L2capPsm *PingClient, BtRemoteDevice *Device, const U8 *Data, U16 DataLen)
{
    L2CAP_Channel *new_channel;
    BtPacket *echo;
    BtStatus status;

    OS_LockStack();
    XASTAT_Set(l2cap_ping_support, TRUE);

    new_channel = L2Cap_AllocLinkChannel(0, Device, &status);
    if (new_channel == 0)
    {
        OS_UnlockStack();
        return status;
    }
    new_channel->psmInfo = PingClient;

    if ((echo = L2Cap_GetSysPkt(TRUE)) == 0)
    {
        L2Cap_FreeChannel(new_channel);
        OS_UnlockStack();
        return BT_STATUS_NO_RESOURCES;
    }

    new_channel->state = L2C_W4_ECHO_CNF;
    new_channel->localIdent = GetNextIdent();

    echo->data = (U8*) Data;
    echo->dataLen = DataLen;

    /*
     * Build Echo Request Header in BtPacket Header. This allows
     * us to easily send Echo Data. We know that no one put any
     * headers in this packet so there is plenty of header space.
     */
    echo->header[BT_PACKET_HEADER_LEN - 4] = LLC_ECHO_REQ;
    echo->header[BT_PACKET_HEADER_LEN - 3] = new_channel->localIdent;
    echo->header[BT_PACKET_HEADER_LEN - 2] = (U8) (DataLen);
    echo->header[BT_PACKET_HEADER_LEN - 1] = (U8) (DataLen >> 8);
    echo->headerLen = 4;

    bt_trace(
        TRACE_GROUP_1,
        L2CAP_PINGBT_DEBUGx04X_SENT_PACKET_IDENT_xD,
        new_channel->localCid,
        new_channel->localIdent);
    echo->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2Cap_Send(new_channel, echo, 0x0001);
    if (status == BT_STATUS_PENDING)
    {
        L2Cap_StartRtx(new_channel);
    }
    else
    {
        ReturnSysPkt(echo);
        L2Cap_FreeChannel(new_channel);
    }

    OS_UnlockStack();
    return status;
}
#endif /* L2CAP_PING_SUPPORT == XA_ENABLED */

#if L2CAP_GET_INFO_SUPPORT == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  L2CAP_GetInfo
 * DESCRIPTION
 *  Requests the specified information from the specified device.
 * PARAMETERS
 *  InfoClient      [IN]        
 *  Device          [IN]        
 *  InfoType        [IN]        
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_GetInfo(const L2capPsm *InfoClient, BtRemoteDevice *Device, U16 InfoType)
{
    L2CAP_Channel *new_channel;
    BtPacket *info;
    BtStatus status;

    OS_LockStack();

    new_channel = L2Cap_AllocLinkChannel(0, Device, &status);
    if (new_channel == 0)
    {
        OS_UnlockStack();
        return status;
    }
    new_channel->psmInfo = InfoClient;

    if ((info = L2Cap_GetSysPkt(TRUE)) == 0)
    {
        L2Cap_FreeChannel(new_channel);
        OS_UnlockStack();
        return BT_STATUS_NO_RESOURCES;
    }

    new_channel->state = L2C_W4_INFO_CNF;
    new_channel->localIdent = GetNextIdent();

    info->header[BT_PACKET_HEADER_LEN - 6] = LLC_INFO_REQ;
    info->header[BT_PACKET_HEADER_LEN - 5] = new_channel->localIdent;
    info->header[BT_PACKET_HEADER_LEN - 4] = 2;
    info->header[BT_PACKET_HEADER_LEN - 3] = 0;
    info->header[BT_PACKET_HEADER_LEN - 2] = (U8) (InfoType);
    info->header[BT_PACKET_HEADER_LEN - 1] = (U8) (InfoType >> 8);
    info->headerLen = 6;

    bt_trace(
        TRACE_GROUP_1,
        L2CAP_GETINFOBT_DEBUGx04X_SENT_PACKET_IDENT_xD,
        new_channel->localCid,
        new_channel->localIdent);
    info->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2Cap_Send(new_channel, info, 0x0001);
    if (status == BT_STATUS_PENDING)
    {
        L2Cap_StartRtx(new_channel);
    }
    else
    {
        ReturnSysPkt(info);
        L2Cap_FreeChannel(new_channel);
    }

    OS_UnlockStack();
    return status;
}
#endif /* L2CAP_GET_INFO_SUPPORT == XA_ENABLED */

#if L2CAP_NUM_GROUPS > 0
/* **************************************************************************
 *
 * L2CAP Group Management API
 *
 * **************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  L2CAP_RegisterGroup
 * DESCRIPTION
 *  Registers a Group within L2CAP.
 * PARAMETERS
 *  Group           [IN]        
 *  ChannelId       [IN]        
 * RETURNS
 *  BT_STATUS_INVALID_PARM - A parameter was invalid.
 *  BT_STATUS_SUCCESS - The group is registered.
 *  BT_STATUS_NO_RESOURCES - The group table is full.
 *****************************************************************************/
BtStatus L2CAP_RegisterGroup(L2capGroup *Group, U16 *ChannelId)
{
    I8 i;

    if (Group == 0 || ChannelId == 0 || Group->psm == 0 || Group->callback == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();
    for (i = 0; i < L2CAP_NUM_GROUPS; i++)
    {
        if (L2C(groups)[i] == 0)
        {
            L2C(groups)[i] = Group;

            Group->l2capTxCount = 0;
            
            Group->cid = BASE_GROUP_CID + (U16) i;
            *ChannelId = BASE_GROUP_CID + (U16) i;

            for (i = 0; i < Group->maxMembers; i++)
            {
                OS_MemSet((U8*) & Group->members[i], 0, sizeof(BD_ADDR));
            }

            XASTAT_PeakInc(l2cap_num_groups, 1);
            OS_UnlockStack();
            return BT_STATUS_SUCCESS;
        }
    }

    *ChannelId = L2CID_INVALID;

    OS_UnlockStack();
    return BT_STATUS_NO_RESOURCES;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_GroupDestroy
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Group       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2CAP_GroupDestroy(L2capGroup *Group)
{
    BtStatus status = BT_STATUS_FAILED;
    I16 i;

    if (!Group)
    {
        return BT_STATUS_FAILED;
    }

    OS_LockStack();
    for (i = 0; i < L2CAP_NUM_GROUPS; i++)
    {
        if ((L2C(groups)[i] == Group) && (GROUP_CID_TO_INDEX(Group->cid) == (U16) i))
        {

            if (Group->l2capTxCount)
            {
                status = BT_STATUS_BUSY;
                break;
            }

            L2C(groups)[i] = 0;
            Group->cid = L2CID_INVALID;

            XASTAT_PeakDec(l2cap_num_groups, 1);
            status = BT_STATUS_SUCCESS;
            break;
        }
    }

    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_GroupAddMember
 * DESCRIPTION
 *  
 * PARAMETERS
 *  GroupId         [IN]        
 *  NewMember       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2CAP_GroupAddMember(U16 GroupId, const BD_ADDR *NewMember)
{
    L2capGroup *group;
    I8 i;

    OS_LockStack();
    group = L2Cap_GetValidGroup(GroupId);

    if (!group || !NewMember)
    {
        OS_UnlockStack();
        return BT_STATUS_FAILED;
    }

    for (i = 0; i < group->maxMembers; i++)
    {
        if (!IsValidBdAddr(&group->members[i]))
        {
            OS_MemCopy(group->members[i].addr, NewMember->addr, sizeof(BD_ADDR));
            OS_UnlockStack();
            return BT_STATUS_SUCCESS;
        }
    }
    OS_UnlockStack();
    return BT_STATUS_NO_RESOURCES;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_GroupRemoveMember
 * DESCRIPTION
 *  Removes a member from a group.
 * PARAMETERS
 *  GroupId     [IN]        
 *  Member      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2CAP_GroupRemoveMember(U16 GroupId, const BD_ADDR *Member)
{
    L2capGroup *group;
    I8 i;

    OS_LockStack();
    group = L2Cap_GetValidGroup(GroupId);

    if (!group || !Member)
    {
        OS_UnlockStack();
        return BT_STATUS_FAILED;
    }

    for (i = 0; i < group->maxMembers; i++)
    {
        if (AreBdAddrsEqual(&group->members[i], Member))
        {
            OS_MemSet(group->members[i].addr, 0, sizeof(BD_ADDR));
            OS_UnlockStack();
            return BT_STATUS_SUCCESS;
        }
    }

    OS_UnlockStack();
    return BT_STATUS_NOT_FOUND;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_GroupGetMembers
 * DESCRIPTION
 *  Gets member list for a given group. Compacts group before
 *  returning list pointer.
 * PARAMETERS
 *  GroupId     [IN]        
 *  List        [IN]        
 *  Count       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2CAP_GroupGetMembers(U16 GroupId, BD_ADDR **List, U8 *Count)
{
    L2capGroup *group;
    I8 i, j;

    OS_LockStack();
    group = L2Cap_GetValidGroup(GroupId);

    if (!group || !List || !Count)
    {
        OS_UnlockStack();
        return BT_STATUS_FAILED;
    }

    for (i = 0; i < group->maxMembers; i++)
    {
        if (IsValidBdAddr(&group->members[i]))
        {
            continue;
        }

        /* Found an empty slot, try to fill it */
        for (j = i + 1; j < group->maxMembers; j++)
        {
            if (!IsValidBdAddr(&group->members[j]))
            {
                continue;
            }
            /* Copy this member into the open slot */
            OS_MemCopy(group->members[i].addr, group->members[j].addr, sizeof(BD_ADDR));
            OS_MemSet(group->members[j].addr, 0, sizeof(BD_ADDR));
            break;
        }

        /* If this slot is still empty, were done */
        if (!IsValidBdAddr(&group->members[i]))
        {
            break;
        }

    }

    *Count = (U8) i;
    *List = group->members;
    OS_UnlockStack();
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_DisableCLT
 * DESCRIPTION
 *  Disables connectionless traffic.
 * PARAMETERS
 *  Psm     [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS - Broadcasts have been disabled.
 *****************************************************************************/
BtStatus L2CAP_DisableCLT(U16 Psm)
{
    /* UNUSED_PARAMETER(Psm); */
    OS_LockStack();
    /* We only support general blocking (ie. Psm = 0) */
    L2C(flags) &= ~L2CCF_BROADCASTS_ENABLED;
    OS_UnlockStack();

    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_EnableCLT
 * DESCRIPTION
 *  Enables Connectionless traffic.
 * PARAMETERS
 *  Psm     [IN]        
 * RETURNS
 *  BT_STATUS_SUCCESS - Broadcasts have been disabled.
 *****************************************************************************/
BtStatus L2CAP_EnableCLT(U16 Psm)
{
    /* UNUSED_PARAMETER(Psm); */
    OS_LockStack();
    /* We only support general unblocking (ie. Psm = 0) */
    L2C(flags) |= L2CCF_BROADCASTS_ENABLED;
    OS_UnlockStack();

    return BT_STATUS_SUCCESS;
}

#endif /* L2CAP_NUM_GROUPS > 0 */

#ifdef __BT_4_0_BLE__
BtStatus L2CAP_RegisterFixCid(L2capCallback callback, U16 cid)
{

    L2C(fixCid)[cid-0x0004].cid = cid;
    L2C(fixCid)[cid-0x0004].inUse = 0;
    L2C(fixCid)[cid-0x0004].callback = callback;    
    return BT_STATUS_SUCCESS;
}
#endif

/*****************************************************************************
 * FUNCTION
 *  L2Cap_Send
 * DESCRIPTION
 *  Internal Connection oriented channel Send function.
 * PARAMETERS
 *  Channel         [IN]        
 *  Packet          [IN]        
 *  RemoteCid       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2Cap_Send(L2CAP_Channel *Channel, BtPacket *Packet, U16 RemoteCid)
{
    I16 packetLen;
    BtStatus status = BT_STATUS_FAILED;
    
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    Assert(((Channel->outLinkMode != LINK_MODE_BASIC) &&
            (RemoteCid != 0x0001)) ? (BT_PACKET_HEADER_LEN >= (Packet->headerLen + 6)) : (BT_PACKET_HEADER_LEN >=
                                                                                          (Packet->headerLen + 4)));
#else /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */ 
    Assert(BT_PACKET_HEADER_LEN >= (Packet->headerLen + 4));
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */ 

    /* If the tail is unused, zero the length for convenience. */
    if ((Packet->flags & BTP_FLAG_TAIL) == 0)
    {
        Packet->tailLen = 0;
    }

    packetLen = Packet->dataLen + Packet->headerLen + Packet->tailLen;
    if (RemoteCid > 0x0003 && packetLen > Channel->outMtu)
    {
        return BT_STATUS_FAILED;
    }
    if (Channel->link->state != BDS_CONNECTED)
    {
        switch(Channel->link->state){
            case BDS_OUT_DISC:
            case BDS_OUT_DISC2:
                OS_Report("l2cap: keep queue packet state:%d", Channel->link->state);
                break;
            default:
                OS_Report("l2cap: return no-connection state:%d", Channel->link->state);                
                return BT_STATUS_NO_CONNECTION;
                break;
        }
    }

    Packet->llpContext = Channel->localCid;
    Packet->remoteCid = RemoteCid;

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    Packet->segNum = 0;
#endif 

    if (RemoteCid == 0x0001)
    {
    	U8 opCode = 0;
    #if L2CAP_FLEXIBLE_CONFIG == XA_DISABLED
        Assert(Packet->flags & BTP_FLAG_LSYSTEM);
    #endif 
        Assert(!(Packet->flags & BTP_FLAG_TAIL));

        /* An L2CAP signal packet, check for workspace overrun */
        Assert(((Packet->flags & BTP_FLAG_LSYSTEM)
                ? (Packet->data != ((SignalPacket*) Packet)->workspace) ||
                (Packet->dataLen <= L2SIG_DATA_LEN) : TRUE));
        if( BT_PACKET_HEADER_LEN >= Packet->headerLen && Packet->headerLen > 0){
		opCode = Packet->header[BT_PACKET_HEADER_LEN - Packet->headerLen];
		Report(("L2Cap: L2Cap_Send signal packet OpCode:0x%x len(%d)", opCode, Packet->headerLen+Packet->dataLen) );
        }
    }
    Channel->l2capTxCount++;
#ifdef __BT_2_1_ERTM__
    if ((Channel->outLinkMode != LINK_MODE_BASIC) && (RemoteCid >= 0x0040))
    {    
        Channel->txCountNormalSent++;
    }
    status = L2Cap_QueueTxPacket(Channel->hciAMPHandler, Channel->link, Packet);
#else    
	status = L2Cap_QueueTxPacket(0, Channel->link, Packet);
#endif    
    if( status != BT_STATUS_SUCCESS ){
        Channel->l2capTxCount--;
        OS_Report("l2cap QueueTx fail at cid:0x%x", Channel->localCid);
    }
    
    return BT_STATUS_PENDING;
}

#ifdef __BT_2_1_ERTM__
BtStatus L2Cap_SendSFrameData(L2CAP_Channel *Channel, BtPacket *Packet, U16 RemoteCid)
{
    I16 packetLen;
    BtStatus status = BT_STATUS_FAILED;
    

    /* If the tail is unused, zero the length for convenience. */
    if ((Packet->flags & BTP_FLAG_TAIL) == 0)
    {
        Packet->tailLen = 0;
    }

    packetLen = Packet->dataLen + Packet->headerLen + Packet->tailLen;

    if (Channel->link->state != BDS_CONNECTED)
    {
        return BT_STATUS_NO_CONNECTION;
    }

    Packet->llpContext = Channel->localCid;
    Packet->remoteCid = RemoteCid;

    Packet->segNum = 0;

    Channel->l2capTxCount++;
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("L2CAP_Send Sframe"));
    Report(("Channel->outLinkMode:%d",Channel->outLinkMode));    
    Report(("RemoteCid:%d",RemoteCid));        
#endif
    status = L2Cap_QueueTxPacket(Channel->hciAMPHandler, Channel->link, Packet);
    if( BT_STATUS_SUCCESS != status ){
        Channel->l2capTxCount--;
        OS_Report("l2cap QueueTx sframe fail at cid:0x%x", Channel->localCid);
    }
    return BT_STATUS_PENDING;
}
#endif

#if L2CAP_NUM_GROUPS > 0


/*****************************************************************************
 * FUNCTION
 *  L2Cap_GroupSend
 * DESCRIPTION
 *  Internal group send function.
 * PARAMETERS
 *  Group       [IN]        
 *  Packet      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2Cap_GroupSend(L2capGroup *Group, BtPacket *Packet)
{
    I8 offset = BT_PACKET_HEADER_LEN - Packet->headerLen - 2;
    I16 packetLen;

    /*
     * The minimum connectionless MTU that can be supported is 670 bytes.
     * * However, we don't know what the maximum is. So no check is made.
     */

    if (HCI_RequestToSend(L2C(broadcastLink).hciHandle) != BT_STATUS_SUCCESS)
    {
        return BT_STATUS_NO_CONNECTION;
    }

    /* If the tail is unused, zero the length for convenience. */
    if ((Packet->flags & BTP_FLAG_TAIL) == 0)
    {
        Packet->tailLen = 0;
    }

    packetLen = Packet->dataLen + Packet->headerLen + Packet->tailLen;

    Assert(BT_PACKET_HEADER_LEN >= (Packet->headerLen + 6));

    /* Add Packet Length, Connectionless CID and PSM fields */

    /*
     * Add PSM to connectionless datagram. The lower transmit routine
     * * will add the remainder of the B-frame header.
     */
    Packet->header[offset + 0] = (U8) (Group->psm);
    Packet->header[offset + 1] = (U8) (Group->psm >> 8);

    Packet->headerLen += 2;

    Packet->llpContext = Group->cid;
    Packet->remoteCid = 0x0002;

    Group->l2capTxCount++;

    MeSuspendTransmitters();

    InsertTailList(&(L2C(broadcastLink).txLowPriorityQueue), &(Packet->node));

    return BT_STATUS_PENDING;
}
#endif /* L2CAP_NUM_GROUPS > 0 */

#ifdef __BT_4_0_BLE__
BtStatus L2Cap_FixCidSend(U8 cid, BtRemoteDevice *link, BtPacket *Packet)
{
    I8 offset = BT_PACKET_HEADER_LEN - Packet->headerLen - 2;
    I16 packetLen;
    L2capFixCid *fixCid;
    
    U8 i=0;
    
    if((link->state == BDS_CONNECTED) && 
        (link->okToSend == TRUE))
    {
        if (HCI_RequestToSend(link->hciHandle) != BT_STATUS_SUCCESS)
        {
            return BT_STATUS_NO_CONNECTION;
        }
    }
        

    /* If the tail is unused, zero the length for convenience. */
    if ((Packet->flags & BTP_FLAG_TAIL) == 0)
    {
        Packet->tailLen = 0;
    }
    
    if((cid - 0x0004) > L2CAP_FIX_CID_NO)
    {
        Assert(0);
    }
    if(cid < 0x0003)
    {
        Assert(0);
    }
    fixCid = &L2C(fixCid)[cid-0x0004];
    packetLen = Packet->dataLen + Packet->headerLen + Packet->tailLen;

    Assert(BT_PACKET_HEADER_LEN >= (Packet->headerLen + 6));

    Packet->llpContext = cid;
    Packet->remoteCid = cid;

    fixCid->inUse++;

    InsertTailList(&(link->txHighPriorityQueue), &(Packet->node));

    return BT_STATUS_PENDING;
}

void L2cap_BLESignCallback(const BtEvent *event)
{
    BtPacket *packet = 0;
    BtRemoteDevice *link = ME_FindRemoteDevice(&event->p.smSignData.bdAddr);

    if (!IsListEmpty(&(link->txSignQueue)))
    {
        packet = (BtPacket*) RemoveHeadList(&(link->txSignQueue));
    }
    if( 0 != packet ){
    packet->dataLen = event->p.smSignData.dataLen;
    L2Cap_FixCidSend(packet->remoteCid, link, packet);
    }
}

BtStatus L2Cap_FixCidSendSigned(U8 cid, BtRemoteDevice *link, BtPacket *Packet, U16 PacketBufLen)
{
    BtStatus status;

    Assert(PacketBufLen >= Packet->dataLen + SM_SIGNATURE_LENGTH);
    if ((status = ME_LE_SignData(L2cap_BLESignCallback, &link->bdAddr, Packet->data, Packet->dataLen + SM_SIGNATURE_LENGTH, Packet->dataLen)) == BT_STATUS_PENDING)
    {
        Packet->remoteCid = cid;
        InsertTailList(&(link->txSignQueue), &(Packet->node));
    }
    return status;
}


#endif /* __BT_4_0_BLE__ */

/*
 * L2Cap_QueueTxPacket
 */


/*****************************************************************************
 * FUNCTION
 *  L2Cap_QueueTxPacket
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 *  Packet      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
BtStatus L2Cap_QueueTxPacket(U16 controllerId, BtRemoteDevice *remDev, BtPacket *Packet)
{
    BtStatus status = BT_STATUS_SUCCESS;
    
    if (remDev->state == BDS_DISCONNECTED)
    {
        OS_Report("l2cap QueueTx fail BDS_DISC"); // ALPS00240113
        return BT_STATUS_FAILED;
    }
    if (Packet->priority_type == BTP_FLAG_HIGH_PRIPORTY)
    {
        InsertTailList(&(remDev->txHighPriorityQueue), &(Packet->node));
        if (L2C(highPriorityNo) < 0xFFFFFFFF)
        {
            L2C(highPriorityNo)++;
        }
    }
    else
    {
        if( Packet->priority_type != BTP_FLAG_NONE_PRIPORTY ){
            /* Not a internal none-priority packet. */
            if( Packet->priority_type != BTP_FLAG_LOW_PRIPORTY ){
                /* Upper layer packet always set it as low pri  */
                OS_Report("L2Cap_QueueTxPacket error wrong pri type:%d", Packet->priority_type);
                Packet->priority_type = BTP_FLAG_LOW_PRIPORTY;
            }
        }
        InsertTailList(&(remDev->txLowPriorityQueue), &(Packet->node));
        if (L2C(lowPriorityNo) < 0xFFFFFFFF)
        {
            L2C(lowPriorityNo)++;
        }
    }

    if (remDev->state == BDS_CONNECTED)
    {
        if ((remDev->mode != BLM_ACTIVE_MODE) &&
            (controllerId ==0))
        {
            /* Alert the ME that we have data to send. */
            MeHandleDataReqWhileNotActive(remDev);
        }

        /* Need both checks to cover MeSuspendTransmitters logic too. */
        if ((remDev->mode != BLM_HOLD_MODE) && (remDev->okToSend))
        {
            AssertEval(HCI_RequestToSend(remDev->hciHandle) == BT_STATUS_SUCCESS);
        }
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendDiscReqInternal
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2Cap_SendDiscReqInternal(L2CAP_Channel *Channel)
{
    BtPacket *req;
    BtStatus status;

    /*
     * It may be necessary to stop the timer because we could be sending
     * a disconnect request while another request is outstanding.
     */
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    if ((Channel->flags & (L2CHF_RTX_ACTIVE | L2CHF_ERTX_ACTIVE)) ||
        (Channel->flags2 & (L2CHF2_MONITOR_ACTIVE | L2CHF2_RETRANS_ACTIVE)))
    {
#else /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */
    if (Channel->flags & (L2CHF_RTX_ACTIVE | L2CHF_ERTX_ACTIVE))
    {
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */
        bt_trace(TRACE_GROUP_1, L2CAP_TIMER_HALTED_BY_DISCONNECTREQ_STATE_xD, Channel->state);
        L2Cap_StopTimer(Channel);
    }

    Channel->state = L2C_W4_DISC_CNF;

    /*
     * We need to advance the localIdent now to avoid the risk of
     * accepting an outstanding response packet after we've changed state.
     */
    Channel->localIdent = GetNextIdent();

    /*
     * If we have an outstanding response to send, forget about it! Also
     * clear the SEND_REQUEST flag now (for convenience) in case it is set.
     */
    Channel->flags &= ~(L2CHF_SEND_RESPONSE | L2CHF_SEND_REQUEST);


#ifdef __BT_2_1_ERTM__
    if ((Channel->inLinkMode != LINK_MODE_BASIC) &&
        (Channel->inLinkMode != LINK_MODE_STREAMMING))
    {
        Channel->flags2 |= L2CHF2_SEND_RR;
        L2Cap_SendSFrame(Channel);
    }
    L2Cap_StopTimer(Channel);
#endif 
    if ((req = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        /*
         * The L2C_W4_DISC_CNF state function will send the
         * request once a system packet is available.
         */
        Channel->flags |= L2CHF_SEND_REQUEST;
        L2C(flags) |= L2CCF_NEED_SYSPACKET;
        return BT_STATUS_PENDING;
    }

    req->header[BT_PACKET_HEADER_LEN - 8] = LLC_DISC_REQ;
    req->header[BT_PACKET_HEADER_LEN - 7] = Channel->localIdent;
    req->header[BT_PACKET_HEADER_LEN - 6] = 4;
    req->header[BT_PACKET_HEADER_LEN - 5] = 0;
    req->header[BT_PACKET_HEADER_LEN - 4] = (U8) (Channel->remoteCid);
    req->header[BT_PACKET_HEADER_LEN - 3] = (U8) (Channel->remoteCid >> 8);
    req->header[BT_PACKET_HEADER_LEN - 2] = (U8) (Channel->localCid);
    req->header[BT_PACKET_HEADER_LEN - 1] = (U8) (Channel->localCid >> 8);

    req->headerLen = 8;

    bt_trace(TRACE_GROUP_1, L2CAP_SENDDISCONNECTREQBT_DEBUGx04X_SENT_PACKET, Channel->localCid);
    /* req->priority_type = BTP_FLAG_HIGH_PRIPORTY; */
    status = L2Cap_SendSignalPacket(Channel, req, BTP_FLAG_LOW_PRIPORTY);
#if 0
    req->priority_type = BTP_FLAG_LOW_PRIPORTY;

    status = L2Cap_Send(Channel, req, 0x0001);
    if (status == BT_STATUS_PENDING)
    {
        L2Cap_StartRtx(Channel);
    }
    else
    {
        ReturnSysPkt(req);
    }
#endif    
    bt_trace(TRACE_GROUP_1, STATUSBT_DEBUGx02X, status);
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendDiscRspInternal
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2Cap_SendDiscRspInternal(L2CAP_Channel *Channel)
{
    BtPacket *rsp;
    BtStatus status;

    /*
     * If we have an outstanding request to send, forget about it! Also
     * clear the SEND_RESPONSE flag now (for convenience) in case it is set.
     */
    Channel->flags &= ~(L2CHF_SEND_RESPONSE | L2CHF_SEND_REQUEST);

    if ((rsp = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        Channel->flags |= L2CHF_SEND_RESPONSE;
        L2C(flags) |= L2CCF_NEED_SYSPACKET;
        return BT_STATUS_PENDING;
    }

    rsp->header[BT_PACKET_HEADER_LEN - 8] = LLC_DISC_RSP;
    rsp->header[BT_PACKET_HEADER_LEN - 7] = Channel->remoteIdent;
    rsp->header[BT_PACKET_HEADER_LEN - 6] = 4;
    rsp->header[BT_PACKET_HEADER_LEN - 5] = 0;
    rsp->header[BT_PACKET_HEADER_LEN - 4] = (U8) (Channel->localCid);
    rsp->header[BT_PACKET_HEADER_LEN - 3] = (U8) (Channel->localCid >> 8);
    rsp->header[BT_PACKET_HEADER_LEN - 2] = (U8) (Channel->remoteCid);
    rsp->header[BT_PACKET_HEADER_LEN - 1] = (U8) (Channel->remoteCid >> 8);

    rsp->headerLen = 8;

    rsp->priority_type = BTP_FLAG_LOW_PRIPORTY;
    status = L2Cap_Send(Channel, rsp, 0x0001);
    if (status != BT_STATUS_PENDING)
    {
        ReturnSysPkt(rsp);
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendConfigReqInternal
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Req         [IN]        
 *  Flags       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2Cap_SendConfigReqInternal(L2CAP_Channel *Channel, BtPacket *Req, L2capConfigFlags Flags)
{
    BtStatus status;

    Channel->flags |= L2CHF_CONFIG_REQ_SENT;

    Channel->localIdent = GetNextIdent();

    /* Build an L2CAP Config Request header in the header area. */
    Req->header[BT_PACKET_HEADER_LEN - 8] = LLC_CONFIG_REQ;
    Req->header[BT_PACKET_HEADER_LEN - 7] = Channel->localIdent;
    Req->header[BT_PACKET_HEADER_LEN - 6] = (U8) (Req->dataLen + 4);
    Req->header[BT_PACKET_HEADER_LEN - 5] = (U8) ((Req->dataLen + 4) >> 8);
    Req->header[BT_PACKET_HEADER_LEN - 4] = (U8) (Channel->remoteCid);
    Req->header[BT_PACKET_HEADER_LEN - 3] = (U8) (Channel->remoteCid >> 8);
    Req->header[BT_PACKET_HEADER_LEN - 2] = (U8) (Flags);
    Req->header[BT_PACKET_HEADER_LEN - 1] = (U8) (Flags >> 8);

    Req->headerLen = 8;
#if 0
    Req->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2Cap_Send(Channel, Req, 0x0001);
    if (status == BT_STATUS_PENDING)
    {                
        bt_trace(TRACE_GROUP_1, L2CAP_SEND_CONFIG_REQUEST, Channel->localCid, (Channel->psmInfo==0?0:Channel->psmInfo->psm));
        L2Cap_StartRtx(Channel);
    }
#else
    status = L2Cap_SendSignalPacket(Channel, Req, BTP_FLAG_HIGH_PRIPORTY);
    if (status == BT_STATUS_PENDING)
    {                
        bt_trace(TRACE_GROUP_1, L2CAP_SEND_CONFIG_REQUEST, Channel->localCid, (Channel->psmInfo==0?0:Channel->psmInfo->psm));
    }
#endif

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendConfigRspInternal
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Rsp         [IN]        
 *  Result      [IN]        
 *  Flags       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2Cap_SendConfigRspInternal(L2CAP_Channel *Channel, BtPacket *Rsp, U16 Result, L2capConfigFlags Flags)
{
    /* We could just test for this since the caller should have set it. */
    if (Result == L2CFG_RESULT_SUCCESS)
    {
        /* Inbound channel configuration is complete */
        if ((Channel->flags & L2CHF_MORE_CONFIG) == 0)
        {
            Channel->flags |= L2CHF_IN_CONFIG_OK;
        }
    }

    if( Result != L2CFG_RESULT_SUCCESS){
        OS_Report("l2cap: sendconfigrsp result:%d flags:0x%x", Result, Channel->flags);
    }

    /* Build an L2CAP Config Response header in the header area. */
    Rsp->header[BT_PACKET_HEADER_LEN - 10] = LLC_CONFIG_RSP;
    Rsp->header[BT_PACKET_HEADER_LEN - 9] = Channel->remoteIdent;
    Rsp->header[BT_PACKET_HEADER_LEN - 8] = (U8) (Rsp->dataLen + 6);
    Rsp->header[BT_PACKET_HEADER_LEN - 7] = (U8) ((Rsp->dataLen + 6) >> 8);
    Rsp->header[BT_PACKET_HEADER_LEN - 6] = (U8) (Channel->remoteCid);
    Rsp->header[BT_PACKET_HEADER_LEN - 5] = (U8) (Channel->remoteCid >> 8);
    Rsp->header[BT_PACKET_HEADER_LEN - 4] = (U8) (Flags);
    Rsp->header[BT_PACKET_HEADER_LEN - 3] = (U8) (Flags >> 8);
    Rsp->header[BT_PACKET_HEADER_LEN - 2] = (U8) (Result);
    Rsp->header[BT_PACKET_HEADER_LEN - 1] = (U8) (Result >> 8);
    Rsp->headerLen = 10;

    Rsp->priority_type = BTP_FLAG_HIGH_PRIPORTY;

    /* Rsp->data is configure option */
    L2Cap_DumpHex("configRsp option", (const char*)Rsp->data, Rsp->dataLen);

    return L2Cap_Send(Channel, Rsp, 0x0001);
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendConnRspInternal
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2Cap_SendConnRspInternal(L2CAP_Channel *Channel)
{
    BtPacket *rsp;
    BtStatus status;

    if ((rsp = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        Channel->flags |= L2CHF_SEND_RESPONSE;
        L2C(flags) |= L2CCF_NEED_SYSPACKET;
        return BT_STATUS_PENDING;
    }

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    if ((Channel->result == L2CONN_ACCEPTED) && (Channel->psmInfo->inLinkMode != L2MODE_BASIC))
    {

        if (Channel->link->linkModes == L2MODE_UNKNOWN)
        {
            /* Need to request the devices extended features before we can respond. */
            Channel->infoReqDone = 0;
            return L2Cap_SendFeaturesReq(Channel, rsp);
        }
        else{
            OS_Report("L: linkModes:0x%x FixedSupported[0]:%d", Channel->link->linkModes, Channel->link->FixedChannelSupported[0]);
            if(Channel->link->FixedChannelSupported[0] & L2FIX_CHANNEL_SUPPORTING_AMP_CHANNEL){
                Channel->a2mpSupported = 1;
            }
        }
		
        if (((Channel->psmInfo->inLinkMode & Channel->link->linkModes) == 0) &&
			(Channel->link->linkModes != 1))
        {
            /* No shared link modes, drop down to fail the connect. */
            Channel->result = L2CONN_REJECT_NO_RESOURCES;   
            /* Not the true reason! */
        }
    }
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */

    /*
     * We need to split the Connect Response command into two pieces
     * * because its size exceeds the individual capacity of either
     * * the packet header or the signal packet workspace.
     */
    rsp->header[BT_PACKET_HEADER_LEN - 8] = LLC_CONN_RSP;
    rsp->header[BT_PACKET_HEADER_LEN - 7] = Channel->remoteIdent;
    rsp->header[BT_PACKET_HEADER_LEN - 6] = 8;
    rsp->header[BT_PACKET_HEADER_LEN - 5] = 0;
    rsp->header[BT_PACKET_HEADER_LEN - 4] = (U8) (Channel->localCid);
    rsp->header[BT_PACKET_HEADER_LEN - 3] = (U8) (Channel->localCid >> 8);
    rsp->header[BT_PACKET_HEADER_LEN - 2] = (U8) (Channel->remoteCid);
    rsp->header[BT_PACKET_HEADER_LEN - 1] = (U8) (Channel->remoteCid >> 8);
    rsp->headerLen = 8;

    rsp->data[0] = (U8) (Channel->result);
    rsp->data[1] = (U8) (Channel->result >> 8);
    rsp->data[2] = (U8) 0;  /* Status */
    rsp->data[3] = (U8) 0;  /* Status >> 8 */
    rsp->dataLen = 4;
    rsp->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2Cap_Send(Channel, rsp, 0x0001);
    if (status != BT_STATUS_PENDING)
    {
        ReturnSysPkt(rsp);
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendConnReqInternal
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2Cap_SendConnReqInternal(L2CAP_Channel *Channel)
{
    BtPacket *req;
    BtStatus status;

    if ((req = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        Channel->flags |= L2CHF_SEND_REQUEST;
        L2C(flags) |= L2CCF_NEED_SYSPACKET;
        return BT_STATUS_PENDING;
    }

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    if (Channel->psmInfo->inLinkMode != L2MODE_BASIC)
    {

        if (Channel->link->linkModes == L2MODE_UNKNOWN)
        {
            /* Need to request the devices extended features before we can connect. */
            Channel->infoReqDone = 0;
            return L2Cap_SendFeaturesReq(Channel, req);
        }
        else{
            OS_Report("L: linkModes:0x%x FixedSupported[0]:%d", Channel->link->linkModes, Channel->link->FixedChannelSupported[0]);
            if(Channel->link->FixedChannelSupported[0] & L2FIX_CHANNEL_SUPPORTING_AMP_CHANNEL){
                Channel->a2mpSupported = 1;
            }
        }

        if ((Channel->psmInfo->inLinkMode & Channel->link->linkModes) == 0)
        {
            Report(("L2CAP PSM in mode:%02x",Channel->psmInfo->inLinkMode));
            Report(("L2CAP link mode:%02x",Channel->link->linkModes));
            /* No shared link modes, fail the connect */
            return BT_STATUS_NOT_SUPPORTED;
        }
    }
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */

    Channel->localIdent = GetNextIdent();

    /* Build an L2CAP Connect Request header in the header area. */
    req->header[BT_PACKET_HEADER_LEN - 8] = LLC_CONN_REQ;
    req->header[BT_PACKET_HEADER_LEN - 7] = Channel->localIdent;
    req->header[BT_PACKET_HEADER_LEN - 6] = 4;
    req->header[BT_PACKET_HEADER_LEN - 5] = 0;
    req->header[BT_PACKET_HEADER_LEN - 4] = (U8) (Channel->remotePsm);
    req->header[BT_PACKET_HEADER_LEN - 3] = (U8) (Channel->remotePsm >> 8);
    req->header[BT_PACKET_HEADER_LEN - 2] = (U8) (Channel->localCid);
    req->header[BT_PACKET_HEADER_LEN - 1] = (U8) (Channel->localCid >> 8);
    req->headerLen = 8;
    status = L2Cap_SendSignalPacket(Channel, req, BTP_FLAG_HIGH_PRIPORTY);
#if 0
    req->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2Cap_Send(Channel, req, 0x0001);
    if (status == BT_STATUS_PENDING)
    {
        L2Cap_StartRtx(Channel);
    }
    else
    {
        ReturnSysPkt(req);
    }
#endif
    return status;
}

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
/*
 * L2Cap_SendFeaturesReq()
 */


/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendFeaturesReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Request     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2Cap_SendFeaturesReq(L2CAP_Channel *Channel, BtPacket *Request)
{
    BtStatus status;

    Channel->localIdent = GetNextIdent();

    Channel->flags2 |= L2CHF2_FEATURES_REQ;

    Request->header[BT_PACKET_HEADER_LEN - 6] = LLC_INFO_REQ;
    Request->header[BT_PACKET_HEADER_LEN - 5] = Channel->localIdent;
    Request->header[BT_PACKET_HEADER_LEN - 4] = 2;
    Request->header[BT_PACKET_HEADER_LEN - 3] = 0;
    Request->header[BT_PACKET_HEADER_LEN - 2] = (U8) (L2INFO_EXTENDED_FEATURES);
    Request->header[BT_PACKET_HEADER_LEN - 1] = 0;
    Request->headerLen = 6;

    bt_trace(TRACE_GROUP_1, L2CAP_SENDFEATURESREQBT_DEBUGx04X_SENT_PACKET, Channel->localCid);
    status = L2Cap_SendSignalPacket(Channel, Request, BTP_FLAG_HIGH_PRIPORTY);
#if 0
    Request->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2Cap_Send(Channel, Request, 0x0001);
    if (status == BT_STATUS_PENDING)
    {
        L2Cap_StartRtx(Channel);
    }
    else
    {
        ReturnSysPkt(Request);
    }
#endif

    return status;
}
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */

#ifdef __BT_2_1_ERTM__
BtStatus L2Cap_SendFixChannelReq(L2CAP_Channel *Channel)
{
    BtStatus status;
    BtPacket *Request;

    Request = L2Cap_GetSysPkt(TRUE);
    Channel->localIdent = GetNextIdent();

    Channel->flags2 |= L2CHF2_FIX_CHANNEL_REQ;

    Request->header[BT_PACKET_HEADER_LEN - 6] = LLC_INFO_REQ;
    Request->header[BT_PACKET_HEADER_LEN - 5] = Channel->localIdent;
    Request->header[BT_PACKET_HEADER_LEN - 4] = 2;
    Request->header[BT_PACKET_HEADER_LEN - 3] = 0;
    Request->header[BT_PACKET_HEADER_LEN - 2] = (U8) (L2INFO_FIXED_CHANNEL_SUPPORTED);
    Request->header[BT_PACKET_HEADER_LEN - 1] = 0;
    Request->headerLen = 6;

    bt_trace(TRACE_GROUP_1, L2CAP_SENDFEATURESREQBT_DEBUGx04X_SENT_PACKET, Channel->localCid);
    status = L2Cap_SendSignalPacket(Channel, Request, BTP_FLAG_HIGH_PRIPORTY);
#if 0
    Request->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2Cap_Send(Channel, Request, 0x0001);
    if (status == BT_STATUS_PENDING)
    {
        L2Cap_StartRtx(Channel);
    }
    else
    {
        ReturnSysPkt(Request);
    }
#endif

    return status;
}


#endif
/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendSignalPacket
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel    [IN]        
 *  Packet     [IN]        
 *  priority   [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus L2Cap_SendSignalPacket(L2CAP_Channel *Channel, BtPacket *Packet, U8 priority)
{
    BtStatus status;
    OS_Report("L: signal:0x%x", Packet->header[BT_PACKET_HEADER_LEN - Packet->headerLen] );
    Packet->priority_type = priority;
    status = L2Cap_Send(Channel, Packet, 0x0001);
    if (status == BT_STATUS_PENDING)
    {
        L2Cap_StartRtx(Channel);
    }
    else
    {
        ReturnSysPkt(Packet);
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  L2CAP_IsHeavyLoading
 * DESCRIPTION
 *  Check the a2dp is connected or not
 * PARAMETERS
 *     
 * RETURNS 0~9 
 *  
 *****************************************************************************/
U8 L2CAP_IsHeavyLoading(){
	if ( L2C(heavyTrafficSource) == L2C_HEAVY_TRAFFIC_EXIST ){
		/* a2dp is connected */
		return 6;
    }
	return 0;
}


#ifdef __BT_4_0_BLE__
BtStatus L2CAP_ConnParmsUpdate(
            U16 int_min,
            U16 int_max,
            U16 slave_latency,
            U16 timeout_multiple,
            BtRemoteDevice *Device)
{
    L2CAP_Channel *Channel;
    BtPacket *req;
    BtStatus status;

    OS_LockStack();
    if(Device->role == BCR_MASTER)
    {
        return BT_STATUS_FAILED;
    }
    if ((Channel = L2Cap_AllocLinkChannel(0, Device, &status)) != 0)
    {
        /* Finish setup of channel and post transmit request */
        Channel->state = L2C_W4_CONN_PARM_UPDATE;
    }
    if ((req = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        return BT_STATUS_FAILED;
    }
    
    Channel->localIdent = GetNextIdent();

    /* Build an L2CAP Connect Request header in the header area. */
    req->header[BT_PACKET_HEADER_LEN - 12] = LLC_CONN_PARAM_UPDATE_REQ;
    req->header[BT_PACKET_HEADER_LEN - 11] = Channel->localIdent;
    req->header[BT_PACKET_HEADER_LEN - 10] = 8;
    req->header[BT_PACKET_HEADER_LEN - 9] = 0;
    req->header[BT_PACKET_HEADER_LEN - 8] = (U8) (int_min);
    req->header[BT_PACKET_HEADER_LEN - 7] = (U8) (int_min >> 8);
    req->header[BT_PACKET_HEADER_LEN - 6] = (U8) (int_max);
    req->header[BT_PACKET_HEADER_LEN - 5] = (U8) (int_max >> 8);
    req->header[BT_PACKET_HEADER_LEN - 4] = (U8) (slave_latency);
    req->header[BT_PACKET_HEADER_LEN - 3] = (U8) (slave_latency >> 8);
    req->header[BT_PACKET_HEADER_LEN - 2] = (U8) (timeout_multiple);
    req->header[BT_PACKET_HEADER_LEN - 1] = (U8) (timeout_multiple >> 8);

    req->headerLen = 12;
    
    req->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    
    status = L2Cap_Send(Channel, req, 0x0005);
    if (status == BT_STATUS_PENDING)
    {
        L2Cap_StartRtx(Channel);
    }
    else
    {
        ReturnSysPkt(req);
    }

    OS_UnlockStack();
    return status;
}
#endif

