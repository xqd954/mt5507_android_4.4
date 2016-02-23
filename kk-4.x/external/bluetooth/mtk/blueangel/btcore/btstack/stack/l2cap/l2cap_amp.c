/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include "bt_common.h"
#include "bt_feature.h"
#include "btalloc.h"
#include "btrom.h"
#include "sys/l2cap_i.h"
#include "sys/l2capxp.h"
#include "sys/mei.h"
#include "bt_devices_list.h"
#include "btaccctrl.h"


#ifdef __BT_3_0_HS__
#include "L2cap_AMP.h"

void L2CAP_AMPListenController(L2CAP_Channel *Channel, U8 newControllerId);
void L2CAP_AMPStopSendFrame(L2CAP_Channel *Channel);
void L2CAP_AMPResumeFrameInitiatorOnOldController();
void L2CAP_AMPResumeFrameInitiatorOnNewController();
void L2CAP_AMPInitiatorSendRRWithP1(L2CAP_Channel *Channel);
void L2CAP_AMPResponderResumeFrameWaitSorIFrameP1(L2CAP_Channel *Channel);
extern BtStatus L2Cap_SendMoveChannelCnfRspInternal(L2CAP_Channel *Channel);
extern BtStatus L2Cap_SendMoveChannelReqInternal(L2CAP_Channel *Channel, U8 InitiatorControllerId);
extern BtStatus L2Cap_SendMoveChannelRspInternal(	L2CAP_Channel *Channel);
extern U8 A2MP_GetActivatedControllerId(BtRemoteDevice *link);
extern BtStatus L2Cap_SendCreateChannelRspInternal(L2CAP_Channel *Channel, U8 rsp_status);
extern BtStatus L2Cap_SendCreateChannelReqInternal(L2CAP_Channel *Channel, U8 RemoteControllerId);
extern BtStatus L2Cap_SendMoveChannelCnfReqInternal(L2CAP_Channel *Channel);
 
#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
extern void L2Cap_InitLockStepControlSettings(L2CAP_Channel *Channel);
#endif
extern void L2Cap_SendRR(L2CAP_Channel *Channel, U8 index);
extern void L2Cap_EnhancementRetransmissionSetPollBit(L2CAP_Channel *Channel, U8 index);
extern void L2Cap_EnhancedRemoveAllFromRxMissingRejectTable(L2CAP_Channel *Channel);
extern void L2CAP_AssignChannelAMPHandler(L2CAP_Channel *channel, U8 controllerId);
BOOL L2CapAMPPriorizationAlgorithm(BtRemoteDevice *link)
{
	U8 *local_addr;
	U8 *remote_addr;
	U8 i=0;

	local_addr = (U8*) MEC(bdaddr).addr;
	remote_addr = (U8*) link->bdAddr.addr;
	for(i=0;i<6;i++)
	{
		if(local_addr[i] > remote_addr[i])
		{
			return TRUE;
		}
		else if(local_addr[i] < remote_addr[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}

void L2CAP_NotifyAMPChannelClosed(U16 ChannelId)
{
    L2CAP_Channel *channel = 0;
    BtStatus status = BT_STATUS_FAILED;

    OS_LockStack();
	Report(("L2CAP_NotifyAMPChannelClosed ChannelId:%d", ChannelId));

    channel = L2Cap_GetValidChannel(ChannelId);
	L2Cap_FreeChannel(channel);
    OS_UnlockStack();
}


void L2CapAMPState_W4_Create(L2CAP_Channel *Channel, I8 Event)
{
	BtStatus status;

	Report(("L2CapAMPState_W4_Create Opcode:%02x", Event));
	switch(Event)
	{
	    case LLC_ACCESS_RSP:
            // notify upper layer to accept it
            L2Cap_StopTimer(Channel); 
            Channel->flags &= ~L2CHF_NOT_VISIBLE;
            L2C(callbackParms).aclLink = Channel->link;
            L2Cap_NotifyPsm(Channel, L2EVENT_CONNECT_IND, BT_STATUS_SUCCESS); // L2EVENT_CONNECT_IND
            break;
		case LLC_INFO_RSP:
			Assert((Event == LLC_SEND_PACKET) ? Channel->flags & L2CHF_SEND_RESPONSE: TRUE);
			Channel->flags &= ~L2CHF_SEND_RESPONSE;
			status = L2Cap_SendCreateChannelRspInternal(Channel, L2CONN_ACCEPTED);
			Report(("L2Cap_SendCreateChannelRspInternal:%d",status));
			return;
		case LLC_CREATE_CHANNEL_REQ:
            break;

        case LLC_TX_DONE:
            {
				if (Channel->flags2 & L2CHF2_FEATURES_REQ)
				{
					/* The info request has been sent, ignore this event. */
					return;
				}
				if (Channel->flags2 & L2CHF2_FIX_CHANNEL_REQ)
				{
					/* The info request has been sent, ignore this event. */
					return;
				}
                if (Channel->result == L2CONN_ACCEPTED)
                {
                    Channel->state = L2C_CONFIG;
                    OS_Report("inflag:%d outflag:%d psm:0x%x origin:%d", Channel->psmInfo->inLinkMode, Channel->psmInfo->outLinkMode, Channel->psmInfo->psm, Channel->inLinkMode);
                    if( Channel->psmInfo->inLinkMode != L2MODE_BASIC ){
                    Channel->outLinkMode = LINK_MODE_ENHANCED_RETRANSMISSION;
                    Channel->inLinkMode = LINK_MODE_ENHANCED_RETRANSMISSION;
                    }

    #if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
                    L2Cap_InitFlowControlSettings(Channel);
    #endif
                    L2Cap_InitLockStepControlSettings(Channel);
                    SendAutoLockStepConfigReq(Channel);
                    return;
                }

                /* If we sent a pending response, there is nothing to do here. */
                if (Channel->result == L2CONN_PENDING)
                {
                    return;
                }
                /* We sent a Negative Confirmation, close the channel. */
                L2Cap_NotifyUserAndCloseChannel(Channel, L2DISC_USER_REQUEST);
                return;
            }
            break;
		/* Casae Wait for upper layer response */
		case LLC_CREATE_CHANNEL_RSP:
		case LLC_MOV_CHANNEL_REQ:
		case LLC_MOV_CHANNEL_RSP:
		case LLC_MOV_CHANNEL_CNF_REQ:
		case LLC_MOV_CHANNEL_CNF_RSP:
		default:
			/* Ignore */
			break;
	}
}

void L2CapAMPState_W4_CreateRsp(L2CAP_Channel *Channel, I8 Event)
{
	BtStatus status;

	Report(("L2CapAMPState_W4_CreateRsp Opcode:%02x", Event));
	switch(Event)
	{
		case LLC_INFO_RSP:
			Assert((Event == LLC_SEND_PACKET) ? Channel->flags & L2CHF_SEND_REQUEST : TRUE);
			
			Channel->flags &= ~L2CHF_SEND_REQUEST;
			status = L2Cap_SendCreateChannelReqInternal(Channel, Channel->remoteAmpControllerId);
			Report(("L2Cap_SendCreateChannelReqInternal:%d",status));
			return;
		case LLC_CREATE_CHANNEL_REQ:
			/* Ignore */
			break;
		case LLC_CREATE_CHANNEL_RSP:
			/* Handle the Create Channel Response from peer side */
			/* Result may be success or pending */
			L2C(callbackParms).ptrs.createChannelRsp.result = LEtoHost16(L2C(currRxData) + 8);
			L2C(callbackParms).ptrs.createChannelRsp.status = LEtoHost16(L2C(currRxData) + 10);
			switch (LEtoHost16(L2C(currRxData) + 8))
			{
					/* Result */
				case L2CONN_ACCEPTED:
					Channel->state = L2C_CONFIG;
					Channel->setupByCreateChannel = 1;
					Channel->remoteCid = LEtoHost16(L2C(currRxData) + 4);
                    Channel->outLinkMode = LINK_MODE_ENHANCED_RETRANSMISSION;

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
					L2Cap_InitFlowControlSettings(Channel);
#endif
                    L2C(callbackParms).aclLink = Channel->link;
					L2Cap_NotifyPsm(Channel, L2EVENT_CREATE_CHANNEL_RSP, BT_STATUS_SUCCESS);
                    L2Cap_InitLockStepControlSettings(Channel);
					SendAutoLockStepConfigReq(Channel);
					break;
				case L2CONN_PENDING:
					Channel->remoteCid = LEtoHost16(L2C(currRxData) + 4);
                    L2C(callbackParms).aclLink = Channel->link;
					L2Cap_NotifyPsm(Channel, L2EVENT_CREATE_CHANNEL_RSP, BT_STATUS_PENDING);
					L2Cap_StartErtx(Channel);
					break;
				case L2CONN_REJECT_NO_RESOURCES:
					status = L2DISC_CONN_NO_RESOURCES;
					L2Cap_NotifyUserAndCloseChannel(Channel, (U16) status); /* aka L2CAP_CONN_REJ */
					break;
				default:
					/* Connection Rejected, set Result */
					status = LEtoHost16(L2C(currRxData) + 10);
					L2Cap_NotifyUserAndCloseChannel(Channel, (U16) status); /* aka L2CAP_CONN_REJ */
					break;
			}

			return;
		case LLC_MOV_CHANNEL_REQ:
		case LLC_MOV_CHANNEL_RSP:
		case LLC_MOV_CHANNEL_CNF_REQ:
		case LLC_MOV_CHANNEL_CNF_RSP:
		default:
			/* Ignore */
			break;
	}
}

void L2CapAMPState_W4_Move(L2CAP_Channel *Channel, I8 Event)
{
	BtStatus stauts;
	Report(("L2CapAMPState_W4_Move Opcode:%02x", Event));
	switch(Event)
	{
		/* Need a case to Handle the Logical Link Indication */
		case LLC_AMP_LOGICAL_LINK_IND_SUCCESS:
			L2CAP_MoveChannelRsp(Channel->localCid, L2MOVE_ACCEPTED, A2MP_GetActivatedControllerId(Channel->link));
			break;
		case LLC_AMP_LOGICAL_LINK_IND_FAILED:
			L2CAP_MoveChannelRsp(Channel->localCid, L2MOVE_CONTROLER_NOT_SUPPORTED, Channel->currentAmpControllerId);
			break;
		case LLC_CREATE_CHANNEL_REQ:
		case LLC_CREATE_CHANNEL_RSP:
		case LLC_MOV_CHANNEL_REQ:
		case LLC_MOV_CHANNEL_RSP:
		case LLC_MOV_CHANNEL_CNF_REQ:
		case LLC_MOV_CHANNEL_CNF_RSP:
			break;
		default:
			break;
	}
}

void L2CapAMPState_W4_MoveRsp(L2CAP_Channel *Channel, I8 Event)
{
	BtStatus stauts;

	Report(("L2CapAMPState_W4_MoveRsp Opcode:%02x", Event));
	switch(Event)
	{
		case LLC_MOV_CHANNEL_REQ:
			/* Handle the move channel request */
			if((L2C(currRxData)[6] !=0) && (A2MP_GetActivatedControllerId(Channel->link) != L2C(currRxData)[6]))
			{
			    Report(("Change to W4 MOVE CNF, controller Id is not supported"));
				L2CAP_MoveChannelRsp(Channel->localCid, L2MOVE_CONTROLER_NOT_SUPPORTED, Channel->currentAmpControllerId);
				Channel->state = L2C_W4_MOVE_CNF;
				/* We are waiting for MOVE Response, we reject the MOVE REQ, not reuqired to notify the upper layer */
				return;
			}
			if(Channel->currentAmpControllerId == L2C(currRxData)[6])
			{
			    Report(("Change to L2C_W4_MOVE_CNF"));
				Channel->state = L2C_W4_MOVE_CNF;
				L2CAP_MoveChannelRsp(Channel->localCid, L2MOVE_CONTROLER_IS_OLD, Channel->currentAmpControllerId);
				/* We are waiting for MOVE Response, we reject the MOVE REQ, not reuqired to notify the upper layer */
				return;
			}
			else
			{
				if(L2CapAMPPriorizationAlgorithm(Channel->link) == TRUE)
				{
			        Report(("L2CapAMPPriorizationAlgorithm TRUE"));

					/* Remain the initiator */
					L2CAP_MoveChannelRsp(Channel->localCid, L2MOVE_CONTROLER_MOVE_COLLISION, 0);
					Channel->state = L2C_W4_MOVE_RSP;
					L2CAP_AMPStopSendFrame(Channel);
					/* Not need to notify the upper layer because we are still the initiator */
					return;

				}
				else
				{
			        Report(("L2CapAMPPriorizationAlgorithm FALSE"));
					/* Become the responder */
					Channel->result = L2MOVE_PENDING;
					Channel->state = L2C_W4_MOVE;
					L2C(callbackParms).ptrs.moveChannelReq.controllerId = L2C(currRxData)[6];
					L2C(callbackParms).ptrs.moveChannelReq.oldControllerId = Channel->currentAmpControllerId;
					L2CAP_AMPStopSendFrame(Channel);
					L2Cap_NotifyPsm(Channel, L2EVENT_MOVE_CHANNEL_REQ, BT_STATUS_SUCCESS);
					return;

				}
			}
			break;
        case LLC_AMP_LOGICAL_LINK_IND_SUCCESS:
/* If the create logical link failed, the remote side will response failed to us. */
       //     L2CAP_MoveChannelCnfReq(Channel->link, Channel->localCid, L2MOVE_CNF_RESULT_SUCCESS);
            break;
        case LLC_AMP_LOGICAL_LINK_IND_FAILED:
            /* We are not necessary to response failed confirmed to the remote side because
                      the remote device will receive the create logical channel failed event also.
                      They have to response the move cnf with failed.
                      If the remote side reponse success, we shall also response success.
                      */
//            L2CAP_MoveChannelCnfReq(Channel->link, Channel->localCid, L2MOVE_CNF_RESULT_FAILED);
            break;
		case LLC_MOV_CHANNEL_RSP:
			/* Handle the move channel response */
			L2C(callbackParms).ptrs.moveChannelRsp.result = LEtoHost16(L2C(currRxData) + 6);
			L2C(callbackParms).ptrs.moveChannelRsp.controllerId = LEtoHost16(L2C(currRxData) + 4);
			L2CAP_AMPStopSendFrame(Channel);
			L2Cap_NotifyPsm(Channel, L2EVENT_MOVE_CHANNEL_RSP, BT_STATUS_SUCCESS);
			break;
		case LLC_CREATE_CHANNEL_REQ:
		case LLC_CREATE_CHANNEL_RSP:
		case LLC_MOV_CHANNEL_CNF_REQ:
		case LLC_MOV_CHANNEL_CNF_RSP:
		default:
			break;
	}
}

void L2CapAMPState_W4_MoveCnf(L2CAP_Channel *Channel, I8 Event)
{
	BtStatus status;

	Report(("L2CapAMPState_W4_MoveCnf Opcode:%02x", Event));
	switch(Event)
	{
		case LLC_MOV_CHANNEL_CNF_REQ:
			/* Handle move channel cfm */
			switch (LEtoHost16(L2C(currRxData) + 6))
			{
					/* Result */
				case L2MOVE_CNF_RESULT_SUCCESS:
					Channel->state = L2C_OPEN;
   					L2CAP_AMPListenController(Channel, Channel->MoveOperationAmpControllerId); /* Listen to New Controller */
 					Channel->currentAmpControllerId = Channel->MoveOperationAmpControllerId;
 					L2C(callbackParms).ptrs.moveChannelCnfReq.result = LEtoHost16(L2C(currRxData) + 6);
					L2CAP_AMPResponderResumeFrameWaitSorIFrameP1(Channel);
					/* After rx */
					L2Cap_NotifyPsm(Channel, L2EVENT_MOVE_CHANNEL_CNF_REQ, BT_STATUS_SUCCESS);
					/* Move to new channel success. */
					break;
				case L2MOVE_CNF_RESULT_FAILED:
                default:
					Channel->state = L2C_OPEN;
					/* Move failed on old channel. */
					L2C(callbackParms).ptrs.moveChannelCnfReq.result = LEtoHost16(L2C(currRxData) + 6);
					L2Cap_NotifyPsm(Channel, L2EVENT_MOVE_CHANNEL_CNF_REQ, BT_STATUS_FAILED);
					L2CAP_AMPListenController(Channel, Channel->currentAmpControllerId); /* Listen to Old Controller */
                    L2CAP_AMPResponderResumeFrameWaitSorIFrameP1(Channel);
 					break;
			}
			break;
		case LLC_CREATE_CHANNEL_REQ:
		case LLC_CREATE_CHANNEL_RSP:
		case LLC_MOV_CHANNEL_REQ:
		case LLC_MOV_CHANNEL_RSP:
		case LLC_MOV_CHANNEL_CNF_RSP:
		default:
			break;
	}
}

void L2CapAMPState_W4_MoveCnfRsp(L2CAP_Channel *Channel, I8 Event)
{
	BtStatus status;

	Report(("L2CapAMPState_W4_MoveCnfRsp Opcode:%02x", Event));
	switch(Event)
	{
		case LLC_MOV_CHANNEL_CNF_RSP:
			/* Handle move channel response */
			Channel->state = L2C_OPEN;
            L2CAP_AMPListenController(Channel, Channel->currentAmpControllerId); /* Listen to New Controller */
			L2Cap_NotifyPsm(Channel, L2EVENT_MOVE_CHANNEL_CNF_RSP, BT_STATUS_SUCCESS);
			L2CAP_AMPInitiatorSendRRWithP1(Channel);
            L2Cap_IssueHciRequestToSend(Channel->link);
			break;
		case LLC_CREATE_CHANNEL_REQ:
		case LLC_CREATE_CHANNEL_RSP:
		case LLC_MOV_CHANNEL_REQ:
		case LLC_MOV_CHANNEL_RSP:
		case LLC_MOV_CHANNEL_CNF_REQ:
		default:
			break;
	}
}

/*****************************************************************************
 * FUNCTION
 *  L2CAP_ConnectReq
 * DESCRIPTION
 *  Send an L2CAP L2CAP_CreateChannelReq request to the specified device and PSM.
 * PARAMETERS
 *  Protocol        [IN]
 *  Psm             [IN]
 *  Device          [IN]
 *  LocalCid        [IN]
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_CreateChannelReq(
 				const L2capPsm *Protocol,
				U16 Psm,
				BtRemoteDevice *Device,
				L2capAMPConnSettings *con_setting,
				U16 *LocalCid)
{
    L2CAP_Channel *new_channel = 0;
    BtStatus status = BT_STATUS_FAILED;

    OS_LockStack();

    if (!L2Cap_IsValidProtocol(Protocol))
    {
        OS_UnlockStack();
        return BT_STATUS_INVALID_PARM;
    }
    if( NULL == con_setting) {
        OS_Report("[WRN] con_setting is NULL");
        return status;
    }

	if(con_setting->Device->state == BDS_CONNECTED)
	{
		if ((new_channel = L2Cap_AllocLinkChannel(Protocol, con_setting->Device, &status)) != 0)
		{
			/* Finish setup of channel and post transmit request */
			new_channel->remotePsm = con_setting->Psm;
            new_channel->setupByCreateChannel = 1;
			new_channel->ampPsm = con_setting->ampPsm;
			/* Finish setup of channel and post transmit request */
			new_channel->state = L2C_W4_CREATE_RSP;
			new_channel->currentAmpControllerId = con_setting->currentAmpControllerId;
			new_channel->remoteAmpControllerId = con_setting->remoteAmpControllerId;
            new_channel->flags2 |= L2CHF2_FOUND_FC_OPTION;
            new_channel->createChannelInitiator = 0;
            Report(("new_channel->createChannelInitiator:%d",new_channel->createChannelInitiator));
#if BT_SECURITY == XA_ENABLED
			 /*
			  * Query the Security Manager to see if this protocol is
			  * authorized to establish an outgoing connection.
			  */
			 new_channel->authorization.remDev = Device;
			 new_channel->authorization.channel = Protocol->psm;
			 new_channel->authorization.incoming = FALSE;
			 new_channel->authorization.id = SEC_L2CAP_AMP_ID;
						 
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
	 			status = L2Cap_SendCreateChannelReqInternal(new_channel, con_setting->remoteAmpControllerId);
				if (status == BT_STATUS_PENDING)
				{
					*LocalCid = new_channel->localCid;
				}
				else
				{
					L2Cap_FreeChannel(new_channel);
				}
			}
		}
    }

    OS_UnlockStack();
    return status;
}


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
BtStatus L2CAP_CreateChannelRsp(U16 ChannelId, U16 ResponseCode, U16 ResponseStatus, L2capConnSettings *Settings)
{
    L2CAP_Channel *channel;
    BtStatus status;

    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);

	if ((channel == 0) || (channel->state != L2C_W4_CREATE) || (ResponseCode > L2CONN_REJECT_NO_RESOURCES))
	{
		OS_UnlockStack();
		return BT_STATUS_FAILED;
	}
	/* Make sure the user doesn't try send the same response twice. */
	if ((channel->flags & L2CHF_AMP_CREATE_RSP_SENT) &&
		((channel->result != L2CONN_PENDING) || (ResponseCode == L2CONN_PENDING)))
	{
		OS_UnlockStack();
		return BT_STATUS_IN_PROGRESS;
	}
	channel->result = ResponseCode;
	channel->flags |= L2CHF_AMP_CREATE_RSP_SENT;

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

	status = L2Cap_SendCreateChannelRspInternal(channel, ResponseStatus);

	OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_ConnectReq
 * DESCRIPTION
 *  Send an L2CAP L2CAP_CreateChannelReq request to the specified device and PSM.
 * PARAMETERS
 *  Protocol        [IN]
 *  Psm             [IN]
 *  Device          [IN]
 *  LocalCid        [IN]
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_MoveChannelReq(
            BtRemoteDevice *Device,
            U16 ChannelId,
            U8 localControllerId,
            U8 remoteControllerId)
{
    L2CAP_Channel *channel = 0;
    BtStatus status = BT_STATUS_FAILED;
	U8 i=0;

    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);
    Report(("Try to move channel"));
    if(channel ==0)
    {
        Report(("Unabled to find ChannelId:%d",ChannelId));
        return BT_STATUS_FAILED;
    }
    if(channel->state != L2C_OPEN)
    {
        Report(("Channel state:%d, Not able to move", channel->state));
        return BT_STATUS_FAILED;
    }
	if(channel->a2mpSupported == 0)
	{
		Report(("AMP not supported"));
		return BT_STATUS_FAILED;
	}

    if(localControllerId == 0x00)
    {
        /* Move back to BR/EDR */
        Report(("Channel Move to BR/EDR"));
    }
    else
    {
        Report(("channel->link->remoteWifiAMP.exists:%d",channel->link->remoteWifiAMP.exists));
        Report(("channel->link->remoteWifiAMP.physical_link_sate:%d",channel->link->remoteWifiAMP.physical_link_sate));
    }
	L2CAP_AMPStopSendFrame(channel);
	channel->MoveOperationAmpControllerId = localControllerId;
	L2CAP_AMPListenController(channel, channel->currentAmpControllerId); /* Listen to old controller */

	if(Device->state == BDS_CONNECTED)
	{
		channel->state = L2C_W4_MOVE_RSP;
		L2Cap_SendMoveChannelReqInternal(channel, remoteControllerId);
    }

    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_ConnectReq
 * DESCRIPTION
 *  Send an L2CAP L2CAP_CreateChannelReq request to the specified device and PSM.
 * PARAMETERS
 *  Protocol        [IN]
 *  Psm             [IN]
 *  Device          [IN]
 *  LocalCid        [IN]
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_MoveChannelRsp(
            U16 ChannelId,
            U16 result,
            U8 controllerId)
{
    L2CAP_Channel *channel = 0;
    BtStatus status = BT_STATUS_FAILED;

    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);
	channel->result = result;
    switch (result)
    {
        case L2MOVE_ACCEPTED:
            L2CAP_AMPStopSendFrame(channel);
            channel->state = L2C_W4_MOVE_CNF;
            channel->MoveOperationAmpControllerId = controllerId;
            break;
        case L2MOVE_PENDING:
            L2CAP_AMPStopSendFrame(channel);
            channel->state = L2C_W4_MOVE;
            break;
        default:
            L2CAP_AMPStopSendFrame(channel);
            channel->state = L2C_W4_MOVE_CNF;
            break;
    }
	L2Cap_SendMoveChannelRspInternal(channel);
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_ConnectReq
 * DESCRIPTION
 *  Send an L2CAP L2CAP_CreateChannelReq request to the specified device and PSM.
 * PARAMETERS
 *  Protocol        [IN]
 *  Psm             [IN]
 *  Device          [IN]
 *  LocalCid        [IN]
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_MoveChannelCnfReq(
            BtRemoteDevice *Device,
            U16 ChannelId,
            U16 result)
{
    L2CAP_Channel *channel = 0;
    BtStatus status = BT_STATUS_FAILED;

    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);

	if(Device->state == BDS_CONNECTED)
	{
	    if(result == L2MOVE_CNF_RESULT_SUCCESS)
        {
			L2CAP_AMPListenController(channel, channel->MoveOperationAmpControllerId); /* Listen to New Controller */
            channel->currentAmpControllerId = channel->MoveOperationAmpControllerId;
        }
		channel->state = L2C_W4_MOVE_CNF_RSP;
		channel->result = result;
 		L2Cap_SendMoveChannelCnfReqInternal(channel);
    }

    OS_UnlockStack();
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  L2CAP_ConnectReq
 * DESCRIPTION
 *  Send an L2CAP L2CAP_CreateChannelReq request to the specified device and PSM.
 * PARAMETERS
 *  Protocol        [IN]
 *  Psm             [IN]
 *  Device          [IN]
 *  LocalCid        [IN]
 * RETURNS
 *  BT_STATUS_FAILED - The request was invalid.
 *  BT_STATUS_NO_CONNECTION - No ACL connection exists.
 *  BT_STATUS_PENDING - The request has started successfully.
 *****************************************************************************/
BtStatus L2CAP_MoveChannelCnfRsp(
            BtRemoteDevice *Device,
            U16 ChannelId)
{
    L2CAP_Channel *channel = 0;
    BtStatus status = BT_STATUS_FAILED;

    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);

	if(Device->state == BDS_CONNECTED)
	{
		channel->state = L2C_OPEN;
		L2Cap_SendMoveChannelCnfRspInternal(channel);
    }

    OS_UnlockStack();
    return status;
}

void L2CAP_AMPListenController(L2CAP_Channel *Channel, U8 newControllerId)
{
    L2CAP_AssignChannelAMPHandler(Channel, newControllerId);
}

void L2CAP_AMPStopSendFrame(L2CAP_Channel *Channel)
{
	Channel->ampSuspend = 1;
    Report(("Set channel a:%08x to %d",Channel, Channel->ampSuspend));
	/* Suspend all tx data */
	L2Cap_StopTimer(Channel);
	L2Cap_EnhancedRemoveAllFromRxMissingRejectTable(Channel);
    Channel->enhancedRetransRemoteBusy = FALSE;
    Channel->enhancedRetransRNRSent = 0;
    Channel->enhancedSFrameRetryCount = 0;
    Channel->enhancedWait_F = 0;
    Channel->enhancedAckWaiting = 0;
    Channel->enhancedNeedToSendFinal = 0;
    Report(("A channel->enhancedNeedToSendFinal:%d",Channel->enhancedNeedToSendFinal));    
    Channel->enhancedRejActioned = 0;
    Channel->enhancedSrejActioned = 0;
    Channel->enhancedSrejSent = 0;
    Channel->enhancedRejSent = 0;
    Channel->enhancedSrejSaveReqSeq = 0;
    Channel->enhancedSrejListTxCount = 0;
    Channel->enhancedPollFinal = 0;
    Channel->enhancedSrejSendRej = 0;
}

void L2CAP_AMPResumeFrameInitiatorOnOldController()
{

}

void L2CAP_AMPResumeFrameInitiatorOnNewController()
{

}

void L2CAP_AMPInitiatorSendRRWithP1(L2CAP_Channel *Channel)
{
	Channel->ampSuspend = 2;
    Report(("Set channel b:%08x to %d",Channel, Channel->ampSuspend));

	/* When we receive the response after we send RR with P=1, we can resume the transmission */
	/* allow tx data but wait for response */
	L2Cap_EnhancementRetransmissionSetPollBit(Channel, 1);
	L2Cap_SendRR(Channel, 1);
	L2Cap_StartMonitorTimer(Channel);
}

void L2CAP_AMPResponderResumeFrameWaitSorIFrameP1(L2CAP_Channel *Channel)
{
	Channel->ampSuspend = 2;
    Report(("Set channel c:%08x to %d",Channel, Channel->ampSuspend));

	/* allow tx data but wait for response */

}

void L2CAP_AMPResumeFrameResponder(L2CAP_Channel *Channel)
{
	if(Channel->ampSuspend == 2)
	{
		Channel->ampSuspend = 0;
        Report(("Set channel d:%08x to %d",Channel, Channel->ampSuspend));
	}

}

void L2CAP_AMPResumeFrameInitiator(L2CAP_Channel *Channel)
{
	if(Channel->ampSuspend == 2)
	{
		Channel->ampSuspend = 0;
        Report(("Set channel e:%08x to %d",Channel, Channel->ampSuspend));

	}
}


void l2CAP_AMPStartAMPTrafficForCreateChannel(L2CAP_Channel *Channel)
{
    Channel->ampSuspend = 0;
}
#endif

