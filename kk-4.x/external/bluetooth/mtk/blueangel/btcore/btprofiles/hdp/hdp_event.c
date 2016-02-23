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

/*******************************************************************************
 *
 * Filename:
 * ---------
 *   hdp_event.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file contains goep event handler of Health Device Profile (HDP).
 *
 * Author:
 * -------
 *   Adam Chen
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#ifdef __BT_HDP_PROFILE__

#include "mcap.h"
#include "hdp.h"
#include "conmgr.h"
#include "config.h"
#include "hdp_util.h"

extern HdpContext hdp_cntx;
extern BtStatus hdp_handle_packet(void);
extern HdpChannel *hdp_create_channel(HdpInstance *instance, BD_ADDR *bdaddr, HdpChannelType type);
extern BtStatus hdp_sdp_query(HdpChannel *channel);
extern BtStatus hdp_mdl_disconnect(HdpChannel *channel, BOOL force);
extern BtStatus hdp_mdl_connect(HdpChannel *channel);
extern void hdp_destroy_channel(HdpChannel *channel);

void hdp_adp_callback(void *buffer, HdpEvent event, BtStatus status)
{
	BOOL notify = TRUE;
	HdpChannel * channel = NULL;
	HdpInstance* instance = NULL;
	HdpCallbackParm parm;
	OS_MemSet(&parm, 0x0, sizeof(HdpCallbackParm));
	
	parm.event	= event;
	parm.result = status;
	OS_Report("[hdp]hdp_adp_callback, event is %d, status is %d", event, status);

	Assert(NULL != hdp_cntx.callback);	
	switch(event)
	{
		case HDP_EVENT_INSTANCE_DEREGISTERED:
			instance = (HdpInstance *)buffer;
			parm.content.instance.mdepId = instance->feature.mdepId;
			notify = TRUE;
			break;
				
		case HDP_EVENT_CONNECT_CNF:
		case HDP_EVENT_DISCONNECT_CNF:
			channel = (HdpChannel *)buffer;			
	//		Assert(channel != NULL && (NULL != channel->instance));
			if (channel == NULL || NULL == channel->instance )
			{
				notify = FALSE;
				break;
			}
			parm.content.channel.mdepId = channel->instance->feature.mdepId;				
			OS_MemCopy(&(parm.content.connection.addr), &(channel->bdAddr), sizeof(BD_ADDR));
			if (BT_STATUS_SUCCESS == status && HDP_EVENT_CONNECT_CNF == event)
			{
				parm.content.connection.mdlId = channel->mcapmdl->mdlId;

				hdp_set_channel_state(channel, HDP_CHANNEL_STATE_CONNECTED);
			}
			else
			{
				hdp_set_channel_state(channel, HDP_CHANNEL_STATE_IDLE );
				hdp_destroy_channel(channel);
				channel = NULL;
			}
			break;
			
		case HDP_EVENT_CHANNEL_CLOSED:	
			channel = (HdpChannel *)buffer;
	//		Assert(channel != NULL && (NULL != channel->instance));
			if (channel == NULL || NULL == channel->instance )
			{
				notify = FALSE;
				break;
			}
			OS_MemCopy(&(parm.content.channel.addr), &(channel->bdAddr), sizeof(BD_ADDR));
		//	parm.content.channel.mdepId 	= channel->mcapmdl->mdepId;
			parm.content.channel.mdepId 	= channel->instance->feature.mdepId;
			parm.content.channel.mdlId		= channel->mcapmdl->mdlId;
			parm.content.channel.l2capId	= channel->mcapmdl->l2ChannelId;
			parm.content.channel.delFlag	= channel->mcapmdl->delFlag;
			hdp_set_channel_state(channel, HDP_CHANNEL_STATE_IDLE);
			notify = TRUE;
			if (channel->mcapmdl->delFlag)
			{
				hdp_destroy_channel(channel);
				channel = NULL;
			}
			break;

			
		case HDP_EVENT_CHANNEL_OPENED:	
			channel = (HdpChannel *)buffer;
	//		Assert(channel != NULL && (NULL != channel->instance));
			if (channel == NULL || NULL == channel->instance )
			{
				notify = FALSE;
				break;
			}
			parm.content.channel.mdepId = channel->instance->feature.mdepId;	
				
			OS_MemCopy(&(parm.content.channel.addr), &(channel->bdAddr), sizeof(BD_ADDR));
			
			if (BT_STATUS_SUCCESS == status)
			{
				parm.content.channel.mdlId		= channel->mcapmdl->mdlId;
				parm.content.channel.l2capId	= channel->mcapmdl->l2ChannelId;
				parm.content.channel.mtu		= channel->mtu;

				/*NOTES: if remote device initiate connection, set channel state if neccessary, because connect cnf is impossible*/
				if (HDP_CHANNEL_STATE_CONNECTED != channel->state)
				{
					hdp_set_channel_state(channel, HDP_CHANNEL_STATE_CONNECTED);
				}
			}
			else
			{
				hdp_destroy_channel(channel);
				channel = NULL;
			}
			break;

		case HDP_EVENT_DATA_RCV_IND:
			channel = (HdpChannel *)buffer;
	//		Assert(channel != NULL && (channel->instance));
			if (channel == NULL || NULL == channel->instance )
			{
				notify = FALSE;
				break;
			}
			parm.content.data.l2capChnId = channel->mcapmdl->l2ChannelId;
			parm.content.data.value = channel->data;
			parm.content.data.size = channel->size;
			break;

		case HDP_EVENT_CONNECTION_REMOVED:		
			OS_MemCopy(&(parm.content.connection.addr), (BD_ADDR *)buffer, sizeof(BD_ADDR));
			break;
		case HDP_EVENT_ECHO_CNF:
			break;
			
		default:
			OS_Report("[HDP][EVENT]invalid call back event: %d", event);
			notify = FALSE;
			
	}

	if (notify)
	{
		parm.event = event;
		parm.result = status;

		hdp_cntx.callback(&parm);
	}
}

void hdp_process_ongoing_operation(BD_ADDR *bdAddr, BtStatus mclResult)
{
	HdpChannel *channel = NULL;
	BtStatus status = BT_STATUS_NOT_FOUND;
	HdpEvent event = 0;
	OS_Report("[HDP]hdp_process_ongoing_operation");
	channel = hdp_find_channel_by_state(bdAddr, HDP_CHANNEL_STATE_CONNECTING);
	if (NULL != channel)
	{
		if (mclResult == BT_STATUS_SUCCESS)
		{
			status = hdp_mdl_connect(channel);
		}
		else
		{
			status = mclResult;
		}

		event = HDP_EVENT_CONNECT_CNF;
		goto exit;
	}

	channel = hdp_find_channel_by_state(bdAddr, HDP_CHANNEL_STATE_DISCONNECTING);
	if (NULL != channel)
	{
		if (mclResult == BT_STATUS_SUCCESS)
		{
			status = hdp_mdl_disconnect(channel, TRUE);
		}
		else
		{
			status = mclResult;
		}
		event = HDP_EVENT_DISCONNECT_CNF;
	}
exit:
	if (BT_STATUS_NOT_FOUND != status && BT_STATUS_PENDING != status && BT_STATUS_SUCCESS!= status )
	{
		hdp_adp_callback(channel, event, status);
	}
}

void hdp_cmgr_callback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status)
{
	HdpChannel *channel = ContainingRecord(Handler, HdpChannel, cmgrHdlr);
	HdpPendingTask * task = NULL;
	BtStatus ret = BT_STATUS_FAILED;
	OS_Report("[HDP]hdp_cmgr_callback:Event %d, status %d",Event, Status);

	if (NULL == channel || FALSE == hdp_channel_check_active(channel)) 

	{
		OS_Report("[HDP]channel is null or not in channel list");
		return;
	}
	
    switch (Event) {
    case CMEVENT_DATA_LINK_CON_CNF:
		if (BT_STATUS_SUCCESS != Status) 
		{
		/*	CMGR_RemoveDataLink(&(channel->cmgrHdlr));
			CMGR_RemoveCmgrWithDevice(&(channel->cmgrHdlr));
			CMGR_DeregisterHandler(&(channel->cmgrHdlr));*/
			hdp_adp_callback(channel, HDP_EVENT_CONNECT_CNF, Status);	
			
		}
		else 
		{
			if (channel->reopening == TRUE && channel ->mcapmdl != NULL)
			{
				ret = hdp_mdl_connect(channel);
			}
			else
			{
				ret = hdp_sdp_query(channel);
				channel->reopening = FALSE;
			}
		
			if (ret != BT_STATUS_PENDING)
			{
				hdp_adp_callback(channel, HDP_EVENT_CONNECT_CNF, BT_STATUS_FAILED);	
			}
		}
		
        break;

	case CMEVENT_DATA_LINK_DIS:

		switch (channel->state)
		{
			case HDP_CHANNEL_STATE_CONNECTING:
				hdp_adp_callback(channel, HDP_EVENT_CONNECT_CNF, BT_STATUS_FAILED);	
				break;
			case HDP_CHANNEL_STATE_DISCONNECTING:
				hdp_adp_callback(channel, HDP_EVENT_DISCONNECT_CNF, BT_STATUS_FAILED);
				break;
			case HDP_CHANNEL_STATE_CONNECTED:
				hdp_set_channel_state(channel, HDP_CHANNEL_STATE_IDLE );
				break;
			case HDP_CHANNEL_STATE_IDLE:
				break;

			default:
				OS_Report("[HDP]invalid state");
		}
		
		break;

    default:
        break;
    }
}

void hdp_event_mcl_opend_cnf(McapCallbackParms  *parms )
{
	BD_ADDR * bdAddr = NULL;
	HdpMcl *mcl = NULL;
	BtStatus status;

	/*there is a bug in MCAP. In this case, directly get address from mcl context */
	bdAddr = &parms->mcl->remDev->bdAddr;
	mcl = hdp_find_mcl(bdAddr);
	show_address(bdAddr);
	if (mcl == NULL)
	{
		status = BT_STATUS_FAILED;
		return;
	}
	if (BT_STATUS_SUCCESS == parms->status)
	{
		OS_MemCopy(&mcl->bdAddr, bdAddr, sizeof(BD_ADDR));
		mcl->mcapMcl = parms->mcl;
	}
	else
	{
		hdp_mcl_delete(mcl);
	}
	hdp_process_ongoing_operation(bdAddr, parms->status);

}

void hdp_event_mcl_opend_ind(McapCallbackParms  *parms)
{
	HdpMcl *mcl = hdp_find_mcl(&parms->remDev->bdAddr);

	if (NULL == mcl)
	{
		mcl = hdp_mcl_new(&parms->remDev->bdAddr);								
	}
	else
	{
		OS_Report("[HDP][WARNING] the orignal mcl exist, but new mcl opended indication is recevied");
	}
	mcl->mcapMcl = parms->mcl;
				
	MCAP_AcceptMcl(parms->mcl, TRUE);

}
void hdp_event_mcl_closed(McapCallbackParms  *parms)
{
	HdpMcl *mcl = hdp_find_mcl(&parms->remDev->bdAddr);
	if (NULL != mcl)
	{	
		if (mcl->state == HDP_CHANNEL_STATE_DISCONNECTING)
		{
			hdp_adp_callback(&parms->remDev->bdAddr, HDP_EVENT_CONNECTION_REMOVED,parms->status);
		}
		hdp_mcl_delete(mcl);
		OS_Report("[HDP]release mcl memory");
	}

}
void hdp_event_mdl_opend_cnf(McapCallbackParms  *parms)
{
	BD_ADDR *addr = &parms->remDev->bdAddr;
	HdpChannel *channel = NULL;

	if (NULL == parms->mdl)
	{
		OS_Report("[HDP]parms->mdl is null");
		return;
	}

	channel = hdp_find_channel_by_mdlId(addr, parms->mdl->mdlId); 
	if (NULL == channel)
	{
		/*no hdp instnace is found, so close it */
		Assert(NULL != parms->mcl);
		MCAP_CloseMdl(parms->mcl,  parms->mdl);
		return;
	}
	if (BT_STATUS_SUCCESS == parms->status)
	{
		channel->mcapmdl = parms->mdl;			
		
		channel->mtu = L2CAP_GetTxMtu(parms->mdl->l2ChannelId)- BT_PACKET_HEADER_LEN;
		OS_Report("[HDP]mtu is %d", channel->mtu);
	}	
	
	if (HDP_MDEP_ID_ECHO_TEST == parms->mdl->mdepId)
	{
		if (BT_STATUS_SUCCESS == parms->status)
		{
			hdp_set_channel_state(channel, HDP_CHANNEL_STATE_CONNECTED);
			hdp_send_data(addr, parms->mdl->mdlId, HDP_ECHO_TEST_PACKET, HDP_ECHO_TEST_PACKET_LEN);
			channel->launchEcho = TRUE;
		}
		else
		{
			hdp_adp_callback(addr, HDP_EVENT_ECHO_CNF, BT_STATUS_FAILED);
		}		
	} 
	else
	{
		hdp_adp_callback(channel, HDP_EVENT_CHANNEL_OPENED, parms->status);
	}

}

void hdp_event_mdl_opend_ind(McapCallbackParms  *parms)
{
	MdepId id;
	HdpInstance *instance = NULL;
	HdpChannel *channel = NULL;
	BD_ADDR *bdAddr = &parms->remDev->bdAddr;
	
	if (NULL == parms->mdl)
	{
		OS_Report("[HDP]parms->mdl is null");
		return;
	}
	id = parms->mdl->mdepId;
	
//	instance = hdp_find_instance(id);
	channel = hdp_find_channel_by_mdlId(bdAddr, parms->mdl->mdlId); 
	
	if (NULL == channel)
	{
		Assert(NULL != parms->mcl);
		//no hdp instnace is found, so close it
		MCAP_CloseMdl(parms->mcl,  parms->mdl);
		OS_Report("[HDP]ERR: invalid mdepid or invalid channel");
	}
	else
	{
		channel->mcapmdl =	parms->mdl;
		channel->mtu = L2CAP_GetTxMtu(parms->mdl->l2ChannelId) - BT_PACKET_HEADER_LEN;
		OS_Report("[HDP]mtu is %d", channel->mtu);
		if (BT_STATUS_SUCCESS == parms->status)
		{					
			OS_Report("[HDP]MCAPEVENT_MDLOPENED_IND: l2cap channel id %d", parms->mdl->l2ChannelId);
			hdp_adp_callback(channel, HDP_EVENT_CHANNEL_OPENED, BT_STATUS_SUCCESS);
		}
	}			


}

void hdp_event_mdl_close(McapCallbackParms  *parms)
{
	BD_ADDR *bdAddr = &parms->remDev->bdAddr;
	HdpChannel *channel = NULL; 

	if (NULL == parms->mdl)
	{
		OS_Report("[HDP]parms->mdl is null");
		return;
	}
	channel = hdp_find_channel_by_mdlId(bdAddr, parms->mdl->mdlId); 
	if (NULL == channel)
	{
		//do nothing;
		OS_Report("[HDP]No channel is found for the mdl id %d", parms->mdl->mdlId);
	}
	else
	{
		HdpInstance *instance = channel->instance;
		BOOL deregistering = channel->instance->state == HDP_INSTANCE_STATE_DEREGISTERING;
		hdp_adp_callback(channel, HDP_EVENT_CHANNEL_CLOSED,  parms->status);
		if(deregistering &&
			hdp_find_channel_by_mdep_id(instance->feature.mdepId) == NULL)
		{
			hdp_adp_callback(instance, HDP_EVENT_INSTANCE_DEREGISTERED,  BT_STATUS_SUCCESS);
		}
		
	}

}

void hdp_event_mdl_config_ind(McapCallbackParms  *parms)
{
	BOOL result = FALSE;
	BD_ADDR *bdAddr = &parms->remDev->bdAddr;
	HdpChannelType localtype = 0;
	HdpChannel *channel = NULL;
	HdpInstance *instance = NULL;

	if (NULL == parms->mdl)
	{
		OS_Report("[HDP]parms->mdl is null");
		goto exit;
	}
	
	//when remote device connect mdl with us, the mdep id in MDL should be local mdep id
	if (HDP_MDEP_ID_ECHO_TEST == parms->mdl->mdepId)
	{
		instance = hdp_find_echo_instance_by_remote_config(parms->l2config);
	}
	else
	{
	instance = hdp_find_instance(parms->mdl->mdepId);
	}
	if (NULL == instance)
	{
		OS_Report("[HDP]invalid mdepid");
		goto exit;
	}
	
	//check config
	localtype = hdp_get_local_config(instance, bdAddr);		
	result = hdp_check_incoming_config(instance->feature.role, localtype, parms->l2config);
	if (FALSE == result)
	{
		OS_Report("[HDP]remote config mismatch: local role %d, channel type %d, but remote config is %d",
				instance->feature.role, instance->channelType, parms->l2config);
		goto exit;
	}
	
	channel = hdp_find_channel_by_mdlId(bdAddr, parms->mdl->mdlId);
	if (NULL == channel)
	{
		//new channel, create it
		channel = hdp_create_channel(instance, bdAddr, parms->l2config);
	}
	else if (NULL == channel->instance)
	{
		OS_Report("[HDP]WARNIG: intance in the channel is null");
		channel->instance = instance;
	}
	else if (instance->feature.mdepId != channel->instance->feature.mdepId)
	{
		if(HDP_CHANNEL_STATE_IDLE != channel->state)
		{
			OS_Report("[HDP]Err: Another channel in use conflict with the current channel request ");
			result = FALSE;
		}
		else
		{
			//replace the instance info
			channel->instance = instance;		
		}
	}

	if (NULL == channel)
	{
		OS_Report("[HDP]error: no channel is found");	
		result = FALSE;
	}
	else 
	{
		localtype = hdp_convert_config_response(instance->feature.role, localtype, parms->l2config);
		channel->mcapmdl = parms->mdl;
	}
	OS_Report("[HDP]result is %d, local type is %d", result, localtype);	
exit:
	if (NULL != parms->mcl && NULL != parms->mdl)
	{
		MCAP_AcceptMdl(parms->mcl, parms->mdl, result, localtype);
	}
	
}
void hdp_event_mdl_config_cnf(McapCallbackParms  *parms)
{
	BD_ADDR *bdAddr = &parms->remDev->bdAddr;
	HdpChannel *channel = NULL;		
	HdpChannelType localtype;
	BtStatus status = parms->status;
	BOOL result = TRUE;
	
	if (NULL == parms->mdl || NULL == parms->mcl)
	{
		OS_Report("[HDP]invalid mcap call parms");
		goto exit;
	}
	
	channel = hdp_find_channel_by_state(bdAddr, HDP_CHANNEL_STATE_CONNECTING);	
	if (NULL == channel)
	{
		OS_Report("[HDP]no instance is found");
		goto exit;
	}
	
	if (BT_STATUS_SUCCESS == status)
	{
		//first check response config
		localtype = hdp_get_local_config(channel->instance, bdAddr);

		result = hdp_check_outgoing_config(channel->instance->feature.role, localtype, parms->l2config);
		if (result) 
		{
			HdpChannelType type = hdp_convert_config_response(channel->instance->feature.role, localtype, parms->l2config);
			channel->config.inLinkMode = hdp_util_convert_type2mode(type);
			status = MCAP_OpenMdl(parms->mdl, channel->rmSdpRecord.dataChnPsm, parms->remDev, &channel->config);					
			if (BT_STATUS_PENDING == status || (BT_STATUS_SUCCESS == status))
			{
				channel->mcapmdl = parms->mdl;	
				status = BT_STATUS_SUCCESS;
			}
		} 
		else
		{
			status = BT_STATUS_INVALID_TYPE;
			OS_Report("[HDP] remote config reponse does not match, local role is %d, registered type is %d, current type is %d, but remote config is %d",
				channel->instance->feature.role, channel->type, localtype, parms->l2config);
		
		}	
	}

exit:
	if (NULL != channel)
	{
		hdp_adp_callback(channel, HDP_EVENT_CONNECT_CNF, status);	
	}
	if (!result && NULL != parms->mcl && NULL != parms->mdl)
	{
		MCAP_AbortMdl(parms->mcl, parms->mdl);	
	}

}
void hdp_event_packet_handle_cnf(McapCallbackParms  *parms)
{
	//delete orignal data packet and restor l2cap packet to pool
	if (NULL == parms->txPkt)
	{
		OS_Report("[HDP]Error:the txPkt returned from MCAP is empty, check it right now");
		return;
	}
	if (NULL != parms->txPkt->data)
	{
		hdp_util_free(parms->txPkt->data);
		parms->txPkt->data = NULL;	
	}	
	else
	{
		OS_Report("[HDP]Warning:the data packet returned from MCAP is empty");
	}
	hdp_pool_pkt_add(ContainingRecord(parms->txPkt, HdpPkt, btPkt));
	hdp_handle_packet();
}

void hdp_event_packet_handle_ind(McapCallbackParms  *parms)
{
	BD_ADDR *bdAddr = &parms->remDev->bdAddr;

	if (NULL == parms->mdl)
	{
		OS_Report("[HDP]parms->mdl is null");
		return;
	}
	
	HdpChannel *channel = hdp_find_channel_by_mdlId(bdAddr, parms->mdl->mdlId);
	if(channel == NULL)
	{
		OS_Report("[HDP]Err, no channel is found");
		return;
	}
	if (HDP_MDEP_ID_ECHO_TEST == parms->mdl->mdepId)
	{
		if (TRUE == channel->launchEcho)
		{
			//verify whether the received data is the same with the sending data
			BOOL ret = OS_MemCmp(parms->rxPkt->data,parms->rxPkt->dataLen, HDP_ECHO_TEST_PACKET, HDP_ECHO_TEST_PACKET_LEN);
			BtStatus status = ret ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
			hdp_adp_callback(bdAddr, HDP_EVENT_ECHO_CNF, status);
			hdp_close_channel(bdAddr, parms->mdl->mdlId);
		}
		else
		{
			//Send a copy to initiator
			hdp_send_data(bdAddr, parms->mdl->mdlId ,parms->rxPkt->data, parms->rxPkt->dataLen);
		}

		
	}
	else
	{
		channel->data = parms->rxPkt->data;
		channel->size = parms->rxPkt->dataLen;
		hdp_adp_callback(channel, HDP_EVENT_DATA_RCV_IND, BT_STATUS_SUCCESS);	
	}
}
void hdp_event_abort(McapCallbackParms  *parms)
{
	BD_ADDR *bdAddr = &parms->remDev->bdAddr;
	HdpEvent event = HDP_EVENT_RANGE;

	if (NULL == parms->mdl)
	{
		OS_Report("[HDP]parms->mdl is null");
		return;
	}
	HdpChannel *channel = hdp_find_channel_by_mdlId(bdAddr, parms->mdl->mdlId);
	if (NULL == channel)
	{
		return;
	}
	if(HDP_CHANNEL_STATE_CONNECTING == channel->state)
	{
		event = HDP_EVENT_CONNECT_CNF;
	} else if (HDP_CHANNEL_STATE_DISCONNECTING == channel->state)
	{
		event = HDP_EVENT_DISCONNECT_CNF;
	}
	hdp_adp_callback(channel, event, BT_STATUS_FAILED);
		
	hdp_destroy_channel(channel);
	
}


void  hdp_mcap_callback ( McapCallbackParms  *parms )
{
	HdpContext *context = &hdp_cntx;
	if (FALSE == context->initialized) 
	{
		return;
	}
	OS_Report("[HDP]hdp_mcap_callback:Event %d, status %d",parms->event, parms->status);
	show_address(&parms->remDev->bdAddr);

	switch (parms->event)
	{
		/*The Event will be notified if DUT initiates mcl connection or remote device initiates mcl connection*/
  		case MCAPEVENT_MCLOPENED:
			hdp_event_mcl_opend_cnf(parms);
			break;
			
		case MCAPEVENT_MCLOPEN_IND:			
			hdp_event_mcl_opend_ind(parms);
			break;

		/*MCAP will delete mcl when mcl is closed, so hdp has to remove history from record*/
		case MCAPEVENT_MCLCLOSED:
			hdp_event_mcl_closed(parms);
			break;
			
		case MCAPEVENT_MDLOPENED_CNF:
			hdp_event_mdl_opend_cnf(parms);
			break;
			
		case MCAPEVENT_MDLOPENED_IND:
			hdp_event_mdl_opend_ind(parms);
			break;

		case MCAPEVENT_MDLCLOSED:
			hdp_event_mdl_close(parms);
			break;
			
		case MCAPEVENT_MDLCONFIG_IND:
			hdp_event_mdl_config_ind(parms);		
			break;
			
		case MCAPEVENT_MDLCONFIG_CNF:
			hdp_event_mdl_config_cnf(parms);
			break;
			
		case MCAPEVENT_ABORTED:
		case MCAPEVENT_ABORT_IND:
			hdp_event_abort(parms);
			break;
			
		case MCAPEVENT_PACKET_HANDLED:
			hdp_event_packet_handle_cnf(parms);
			break;
			
		case MCAPEVENT_PACKET_IND:
			hdp_event_packet_handle_ind(parms);
			break;
		default:
			break;
	}
		
}

#endif /* __BT_HDP_PROFILE__ */
