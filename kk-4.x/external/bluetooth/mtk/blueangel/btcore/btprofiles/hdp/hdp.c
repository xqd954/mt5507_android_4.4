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
 *   hdp.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file contains public API functions for the  Health device profile (HDP).
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
#include "bt_common.h"
#include "sdpdb.h"
#include "hdp_util.h"

HdpContext hdp_cntx;

extern  void hdp_cmgr_callback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status);
extern void  hdp_mcap_callback ( McapCallbackParms  *parms );


/* HDP SDP functions */
extern BtStatus hdp_sdp_query(HdpChannel *channel);

/* Local functions */
void hdp_destroy_channel(HdpChannel *channel);
BtStatus hdp_close_channel_all(MdepId id);


void hdp_tp_disconnect(HdpChannel *channel)
{
	if (NULL == channel)
	{
		return;
	}
	CMGR_RemoveDataLink(&(channel->cmgrHdlr));
	CMGR_RemoveCmgrWithDevice(&(channel->cmgrHdlr));
	CMGR_DeregisterHandler(&(channel->cmgrHdlr));
}
BtStatus hdp_tp_connect(HdpChannel *channel)
{
	BtStatus ret;
	OS_Report("[HDP]hdp_tp_connect");

	ret = CMGR_RegisterHandler(&channel->cmgrHdlr, hdp_cmgr_callback);
	if (BT_STATUS_SUCCESS == ret || BT_STATUS_IN_USE == ret)
	{
		ret = CMGR_CreateDataLink(&channel->cmgrHdlr, &channel->bdAddr);	
	}	
	OS_Report("[HDP]hdp_tp_connect result %d", ret);
	
	return ret;
}

BtStatus hdp_mcl_connect(HdpChannel *channel)
{
	HdpMcl* mcl = hdp_find_mcl(&channel->bdAddr);
	BtStatus ret;
	
	OS_Report("[HDP]hdp_mcl_connect:");

	Assert (channel != NULL && channel->cmgrHdlr.remDev != NULL);
	if (NULL == mcl)
	{
		mcl = hdp_mcl_new(&channel->bdAddr);
		if (NULL == mcl)
		{			
			ret = BT_STATUS_FAILED;
			goto exit;		
		}
	}

	ret = MCAP_OpenMcl(channel->cmgrHdlr.remDev, channel->rmSdpRecord.ctrChnPsm);		
	if (BT_STATUS_SUCCESS == ret)
	{
		//even result is success, MCAP still will callback
		ret = BT_STATUS_PENDING;
	}
	
exit:
	OS_Report("[HDP]hdp_mcl_connect result:%d", ret); 
	return ret;
}

BtStatus hdp_mdl_connect(HdpChannel *channel) 
{
	BtStatus ret;
	HdpMcl* mcl = hdp_find_mcl(&channel->bdAddr);
	Assert(mcl != NULL && mcl->mcapMcl != NULL);

	switch(channel->state)
	{			
		case HDP_CHANNEL_STATE_CONNECTED:	
			ret = BT_STATUS_SUCCESS;
			break;
			
		case HDP_CHANNEL_STATE_IDLE:
			hdp_set_channel_state(channel, HDP_CHANNEL_STATE_CONNECTING);
		case HDP_CHANNEL_STATE_CONNECTING:
			if (channel->mcapmdl != NULL)
			{				
				ret = MCAP_ReconnectMdl(mcl->mcapMcl, channel->mcapmdl);
			}
			else
			{
				//mdl does not exist, create it
				ret = MCAP_Config(mcl->mcapMcl, channel->rmSdpRecord.mdepId, channel->type);
				if (BT_STATUS_SUCCESS == ret) 
				{
					//ret = MCAP_OpenMdl(mcl->mcapMcl, channel->rmSdpRecord.dataChnPsm, channel->cmgrHdlr.remDev, &channel->config);
				}
			}
			break;
			
		case HDP_CHANNEL_STATE_DISCONNECTING:
			ret = BT_STATUS_FAILED;
			break;
			
		default:
			OS_Report("[HDP]Err: invalid state");
			ret = BT_STATUS_FAILED;
	}

	return ret;
} 

/*Notes: force flag is neccessary for hdp_mdl_disconnect  to identify request source(upper layer or MCAP callback)*/
/*          PS: not necessary for hdp_mdl_connect, which only followes SDP query result*/
BtStatus hdp_mdl_disconnect(HdpChannel *channel, BOOL force)
{
	HdpMcl* mcl;
	BtStatus ret;
	OS_Report("[HDP]hdp_mdl_disconnect");
	
	mcl = hdp_find_mcl(&channel->bdAddr); 

	/*Release pending packet based on l2cap id*/
	//TODO: mutex protection
	if (NULL != channel->mcapmdl)
	{
		hdp_data_packat_list_remove_by_channel(&channel->bdAddr, channel->mcapmdl->mdepId);
	}

	switch(channel->state)
	{
		case HDP_CHANNEL_STATE_DISCONNECTING:
			if (! force)
			{
				ret = BT_STATUS_PENDING;
				break;
			}
			
		case HDP_CHANNEL_STATE_CONNECTED:	
			Assert(mcl != NULL && mcl->mcapMcl != NULL);
			hdp_set_channel_state(channel, HDP_CHANNEL_STATE_DISCONNECTING);
			ret = MCAP_CloseMdl(mcl->mcapMcl, channel->mcapmdl);
			if (BT_STATUS_PENDING != ret)
			{				
				hdp_set_channel_state(channel, HDP_CHANNEL_STATE_IDLE);
			}
			break;
			
		case HDP_CHANNEL_STATE_CONNECTING:
			if (mcl != NULL && mcl->mcapMcl != NULL)
			{
				ret = MCAP_AbortMdl(mcl->mcapMcl, channel->mcapmdl);
			}
			else
			{
				ret = BT_STATUS_PENDING;
			}
			hdp_set_channel_state(channel, HDP_CHANNEL_STATE_DISCONNECTING);
			break;

		/*create mcl and disconnect mdl*/
		case HDP_CHANNEL_STATE_IDLE:
		/*	if (channel->mcapmdl != NULL)
			{	
				//Notes: In some case that MCAP is still alive, MCAPCallBack will be immediately before this function exit.
				// So, channel state has to be set before MCAP API is called
				hdp_set_channel_state(channel, HDP_CHANNEL_STATE_DISCONNECTING);				
				ret = hdp_tp_connect(channel);
				if (BT_STATUS_SUCCESS == ret)
				{	
					ret = MCAP_OpenMcl(channel->cmgrHdlr.remDev, channel->rmSdpRecord.ctrChnPsm);
				}
				else
				{
					OS_Report("[HDP]ERR in opening ACL connection");
				}
			}
			else
			{
				hdp_destroy_channel(channel);
				ret = BT_STATUS_SUCCESS;
			}
			*/
			hdp_destroy_channel(channel);
			ret = BT_STATUS_SUCCESS;
			break;
			
		default:
			OS_Report("[HDP]Err: invalid state");
			ret = BT_STATUS_FAILED;
	}
	return ret;
}

BtStatus hdp_mdl_send_data(HdpChannel *channel, HdpPkt* pkt, HdpDataPacket * txPkt)
{
	BtStatus ret;
	HdpMcl *mcl = NULL;
	OS_Report("[HDP]hdp_mdl_send_data: data len %d", txPkt->data.dataLen);
	if (NULL == channel || channel->mcapmdl == NULL)
	{
		return BT_STATUS_INVALID_PARM;
	}
	switch (channel->mcapmdl->mdlState)
	{			
		case MCAP_MDL_STATE_OPEN:
			ret = MCAP_SendData(channel->mcapmdl, &pkt->btPkt, &txPkt->data);					
			break;		
		
		case MCAP_MDL_STATE_CLOSED:
		/*ml = hdp_find_mcl(&mdl->bdAddr);
			if (NULL == mcl)
			{
				//Notes: maybe this is not a better solution to discard the packet. why not connect again?
				//But, it really needs extra effort, so leave it in futhure.
				return BT_STATUS_FAILED;
			}
			ret = MCAP_ReconnectMdl(mcl->mcapMcl, mdl);	
			break;	
			*/
		case MCAP_MDL_STATE_DISC_PENDING:
		case MCAP_MDL_STATE_CONN_PENDING:
			ret = BT_STATUS_PENDING;
			break;
		default:
			//unexpected state
			ret = BT_STATUS_FAILED; 		
	}
	return ret;

	 
}



BtStatus hdp_connect_channel(HdpChannel *channel, BOOL reopen)
{
	BtStatus ret;
	OS_Report("[HDP]hdp_connect_channel: reopen ? %d", reopen);
	if (NULL == channel)
	{
		ret = BT_STATUS_INVALID_PARM;
		goto exit;
	}
	hdp_set_channel_state(channel,HDP_CHANNEL_STATE_CONNECTING);
	ret = hdp_tp_connect(channel);
	if (BT_STATUS_SUCCESS != ret)
	{
		goto exit;		
	}
	if (reopen)
	{
		channel->reopening = TRUE;	
		ret = hdp_mcl_connect(channel); 
		if (BT_STATUS_SUCCESS == ret)
		{
			ret = hdp_mdl_connect(channel);
		}
	}
	else
	{
		ret = hdp_sdp_query(channel);
	}
		
exit:
	if (BT_STATUS_PENDING == ret || BT_STATUS_SUCCESS == ret)
	{
	}
	else if( NULL != channel)
	{
		hdp_set_channel_state(channel,HDP_CHANNEL_STATE_IDLE);	
		hdp_destroy_channel(channel);
		OS_Report("[HDP] Err, fail to reopen channel, so destroy it for ever");
	}
	return ret;
}



/*id: local instance id*/
/*bdaddr: remote device address*/
/*type: expected channel type located in remote device*/
/*Notes: the type is critical for sink connection*/
//HdpChannel *hdp_create_channel(MdepId id, BD_ADDR *bdaddr, HdpChannelType type, HdpRole role)
HdpChannel *hdp_create_channel(HdpInstance *instance, BD_ADDR *bdaddr, HdpChannelType type)
{
	HdpChannel *channel = NULL;
	L2capLinkMode mode;
	U16 rmUuid = 0;
	OS_Report("[HDP]hdp_create_channel");

//	instance = hdp_find_instance(id);		
	if (NULL == instance)
	{
		OS_Report("[HDP]instance is NULL");
		goto error;
	}
	channel = (HdpChannel *)hdp_util_malloc(sizeof(HdpChannel));
	Assert(channel != NULL);

	OS_MemSet(channel, 0x0, sizeof(HdpChannel));

	OS_MemCopy(&channel->bdAddr, bdaddr, sizeof(BD_ADDR));

	channel->instance = instance;
	channel->type = type;

	mode = hdp_util_convert_type2mode(instance->channelType);
	if (HDP_INVALID_L2CAP_MODE == mode)
	{
		OS_Report("[HDP] invalid l2cap mode");
		goto error;
	}
	channel->config.inLinkMode = mode;
	rmUuid = hdp_get_target_uuid(instance->feature.role);
	if (0 == rmUuid)
	{
		OS_Report("[HDP] invalid remote device uuid");
		goto error;
	}
	channel->rmUuid = rmUuid;

	InsertTailList(&hdp_cntx.channel, &(channel->node));
	return channel;
error:
	hdp_util_free(channel);
	return NULL;
}

void hdp_destroy_channel(HdpChannel *channel)
{	
	OS_Report("[HDP]hdp_destroy_channel: %d", channel);
	if (NULL == channel || FALSE == hdp_channel_check_active(channel))
	{
		return;	
	}
	RemoveEntryList(&(channel->node));
	hdp_tp_disconnect(channel);
	hdp_util_free(channel);
}


BtStatus hdp_handle_packet()
{	
	BtStatus ret = BT_STATUS_FAILED;
	HdpPkt* pkt;
	HdpDataPacket * txPkt;
	HdpChannel * channel = NULL;

	
	OS_Report("[HDP]hdp_handle_packet:");

	pkt= hdp_pool_pkt_remove();
	if (NULL == pkt)
	{
		ret = BT_STATUS_NO_RESOURCES;
		goto exit;
	}
	while (BT_STATUS_PENDING != ret) {		
		txPkt = hdp_data_packat_list_remove();
		if (NULL == txPkt)
		{
			ret = BT_STATUS_SUCCESS;
			OS_Report("[HDP]No pending packet is in list");
			break;
		}
		channel = hdp_find_channel_by_mdlId(&txPkt->addr, txPkt->mdlId);
		if (NULL == channel)
		{
			//shit, garbage pakcet, remove it forcely
			hdp_data_packat_delete(txPkt, TRUE);
			OS_Report("[HDP]The packet loses its channel info, what a pity");
		}
		else
		{
			ret = hdp_mdl_send_data(channel, pkt, txPkt);	
			hdp_data_packat_delete(txPkt, (BT_STATUS_PENDING != ret));
		}		
	 }

	if (BT_STATUS_PENDING != ret)
	{
		hdp_pool_pkt_add(pkt);
	}

exit:
	return ret;
}




/*when HDP instance request to register itself*/
/*1.assign MDEP ID, and add it to features list*/
/*2.  update SDP record*/
MdepId hdp_register(HdpConfig *config)
{
	BtStatus ret;
	HdpInstance * instance = NULL;
	MdepId id = HDP_INVALID_MDEP_ID;
	OS_Report("[HDP]hdp_register, role is %d, datatype %d, channel type %d, desc %s",
				config->role, config->datatype, config->channelType, config->description);
	if (NULL == config || id <= 0) 
	{
		OS_Report("[HDP]Err, config is null? %d, id is %d",
					(NULL == config), id);
		id = HDP_INVALID_MDEP_ID;
		goto exit;
	}	

	if(FALSE == hdp_check_config_valid(config->role, config->channelType))
	{
		goto exit;
	}

	instance = hdp_find_registered_instance(config);
	if (NULL != instance)
	{
		id = instance->feature.mdepId;
		goto exit;
	}
		
	instance = hdp_find_available_instance();
	if (NULL == instance)
	{
		goto exit;
	}	

	id = hdp_assign_mdep_id();

	/*Update SDP*/
	ret = HdpSdpDB_AddFeature(id, config->role, config->datatype, config->description);
	if (BT_STATUS_SUCCESS != ret)
	{
		OS_Report("[HDP]it fails to add instance to SDP");
		id = HDP_INVALID_MDEP_ID;
		goto exit;
	}
	OS_MemSet(instance, 0x0, sizeof(HdpInstance));	
	instance->state = HDP_INSTANCE_STATE_REGISTERED;
	instance->channelType = config->channelType;
	instance->feature.mdepId = id;
	instance->feature.role = config->role;
	instance->feature.dataType = config->datatype;
	strncpy(instance->feature.descriotion, config->description, HDP_INSTNACE_DESC_MAX_LEN);
	
exit:
	OS_Report("[HDP]instance id is %d", id);
	return id;
}

BtStatus hdp_register_echo(HdpRole role)
{
	BtStatus ret = BT_STATUS_SUCCESS;
	HdpInstance * instance = NULL;		
	OS_Report("[HDP]hdp_register_echo");

	if (!hdp_util_role_valid(role))
	{
		ret = BT_STATUS_INVALID_PARM;
		goto exit;
	}

	instance = hdp_find_instance(HDP_MDEP_ID_ECHO_TEST);
	if (NULL != instance && role == instance->feature.role)
	{
		ret = BT_STATUS_SUCCESS;
		goto exit;
	}
	instance = hdp_find_available_instance();
	if(NULL == instance)
	{
		ret = BT_STATUS_NO_RESOURCES;
		goto exit;
	}
	OS_MemSet(instance, 0x0, sizeof(HdpInstance));
	
	instance->state = HDP_INSTANCE_STATE_REGISTERED;
	instance->feature.mdepId = HDP_MDEP_ID_ECHO_TEST;
	instance->feature.role = role;
	switch(role)
	{
		case HDP_ROLE_SINK:
			instance->channelType = HDP_CHANNEL_NO_PREFERENCE;
			break;
			
		case HDP_ROLE_SOURCE:
			instance->channelType = HDP_CHANNEL_RELIABLE;
			break;
			
		default:
			OS_MemSet(instance, 0x0, sizeof(HdpInstance));		
			ret = BT_STATUS_INVALID_PARM;
	}	
	
exit:
	return ret;
}


/*before deregister intance, disconnect mdl, close mcl, and update SDP*/
/*Notes: we do not reset the related instance contex until mcap return confirmation*/
BtStatus hdp_deregister(MdepId id) 
{
	HdpInstance* instance = NULL;
	BtStatus ret;
	OS_Report("hdp_deregister: %d", id);

	instance = hdp_find_instance(id);
	if (NULL == instance)
	{
		ret = BT_STATUS_INVALID_PARM;
	}
	else if (instance->state == HDP_INSTANCE_STATE_DEREGISTERING)
	{
		ret = BT_STATUS_PENDING;
	}
	else if (instance->state == HDP_INSTANCE_STATE_IDLE)
	{
		ret = BT_STATUS_SUCCESS;
	}
	else
	{		
		if (HDP_MDEP_ID_ECHO_TEST != id)
		{
			ret = HdpSdpDB_RemoveFeature(instance->feature.mdepId);
			OS_Report("[HDP] the result to remove feature from SDP is %d", ret);
		}	

		ret = hdp_close_channel_all(id);
		if (BT_STATUS_PENDING == ret)
		{
			instance->state = HDP_INSTANCE_STATE_DEREGISTERING;
		}
		else
		{
			//Notes: if disconnect cnf has not been received but MMI request to deregister instance, MAYBE no chance to clear channel.
			//		SO leave clearing context to registering instance
			//	OS_MemSet(instance, 0x0, sizeof(HdpInstance));
			instance->state = HDP_INSTANCE_STATE_IDLE;
		}
		
	}
exit:
	return ret;	
}
BtStatus hdp_deregister_all()
{
	U8 index = 0;
	HdpInstance *instance = NULL;
	BtStatus ret = BT_STATUS_SUCCESS;

	for (index = 0; index < HDP_INSTANCE_MAX_NUM; index ++)
	{
		instance = &hdp_cntx.instance[index];
		if (instance->state == HDP_INSTANCE_STATE_REGISTERED)
		{
			ret = hdp_deregister(instance->feature.mdepId);
		}
	}

	return ret;
}

/*---------------------------------------------------------------------------
 *            hdp_open_channel()
 *---------------------------------------------------------------------------
 *  
 *open mdl channel
 *
 *---------------------------------------------------------------------------
 * MdepId:  (in)indication to specific instance
 * BD_ADDR: (in)inication to remote device
 * BtStatus: (out)   result
 *---------------------------------------------------------------------------
 */

BtStatus hdp_open_channel(MdepId id, BD_ADDR *bdaddr)
{
	BtStatus ret;
	HdpChannel *channel = NULL;
	HdpChannelType localtype;
	HdpInstance* instance = NULL; 
	OS_Report("[HDP]hdp_open_channel:id %d, addr 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x", 
				id, bdaddr->addr[0],bdaddr->addr[1],bdaddr->addr[2],bdaddr->addr[3],bdaddr->addr[4],bdaddr->addr[5]);

	instance = hdp_find_instance(id);
	if (NULL == instance)
	{
		ret = BT_STATUS_INVALID_PARM;
		goto exit;
	}

	localtype = hdp_get_local_config(instance, bdaddr);	
	
	channel = hdp_create_channel(instance, bdaddr, localtype);
	ret = hdp_connect_channel(channel, FALSE);	
exit:
	return ret;	
}
/*---------------------------------------------------------------------------
 *            hdp_reopen_channel()
 *---------------------------------------------------------------------------
 *  
 *reopen mdl channel
 *
 *---------------------------------------------------------------------------
 * BD_ADDR: (in)inication to remote device
 * mdlId: (in)   mdl id
 * BtStatus: (out)   result
 *---------------------------------------------------------------------------
 */

BtStatus hdp_reopen_channel(BD_ADDR *bdaddr, U16 mdlId)
{
	BtStatus ret = BT_STATUS_FAILED;
	HdpMcl *mcl = NULL;
	HdpChannel *channel = NULL;
	OS_Report("[HDP]hdp_reopen_channel: address is 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x, mdl id is %d",
				bdaddr->addr[0], bdaddr->addr[1], bdaddr->addr[2], bdaddr->addr[3], bdaddr->addr[4], bdaddr->addr[5], mdlId);
	
	channel = hdp_find_channel_by_mdlId(bdaddr, mdlId);
	ret = hdp_connect_channel(channel, TRUE);	
exit:
	return ret;	
}

/*---------------------------------------------------------------------------
 *            hdp_close_channel()
 *---------------------------------------------------------------------------
 *  
 *close the data channel based on remote device and channeld id specified by instance.
 *
 *---------------------------------------------------------------------------
 * BD_ADDR: (in)inication to remote device
 * mdl id : (in)indication to mdl id
 * BtStatus: (out)   result
 *---------------------------------------------------------------------------
 */

BtStatus hdp_close_channel(BD_ADDR *bdaddr, U16 mdlId)
{
	HdpChannel* channel;
	HdpMcl* mcl;
	BtStatus ret;
	OS_Report("[HDP]hdp_close_channel");
	channel = hdp_find_channel_by_mdlId(bdaddr, mdlId);
	if (NULL == channel)
	{
		ret = BT_STATUS_FAILED;
		goto exit;
	}

	/*Release pending packet based on l2cap id*/
/*	if (NULL != channel->mcapmdl)
	{
		hdp_data_packat_list_remove_by_channel(channel->mcapmdl->l2ChannelId);
	}
	*/
	
	ret = hdp_mdl_disconnect(channel, FALSE);
		
exit:
	return ret;
}

/*---------------------------------------------------------------------------
 *            hdp_close_channel_all()
 *---------------------------------------------------------------------------
 *  
 *close all the data channels based on remote device specified by instance.
 *
 *---------------------------------------------------------------------------
 * MdepId:  (in)indication to specific instance
 * BtStatus: (out)   result
 *---------------------------------------------------------------------------
 */
BtStatus hdp_close_channel_all(MdepId id)
{

	BtStatus ret = BT_STATUS_SUCCESS;
	HdpChannel *channel = NULL;
	ListEntry* nextNode = NULL;
	
	OS_Report("[HDP] hdp_close_channel_all: id %d", id);

	if(IsListEmpty(&hdp_cntx.channel))
	{
		OS_Report("[HDP] channel list is empty");
		goto exit;
	}
	
	nextNode = GetHeadList(&hdp_cntx.channel);
	do {
		channel = (HdpChannel *) nextNode;
		nextNode = GetNextNode(nextNode);
		if (channel->instance->feature.mdepId == id)
   		{
			BtStatus iRet = hdp_mdl_disconnect(channel, FALSE);
			OS_Report("[HDP] ret %d", iRet);

			//if any result is pending, no need to update ret
			if (ret != BT_STATUS_PENDING)
			{
				ret = iRet;
			}			
		}
	} while ((ListEntry *)nextNode != &hdp_cntx.channel);
exit:	
	return ret;
}

/*---------------------------------------------------------------------------
 *            hdp_remove_connection()
 *---------------------------------------------------------------------------
 *  
 * FOR PTS TEST, force to remove mcl connection
 *
 *---------------------------------------------------------------------------
 * BD_ADDR: (in)inication to remote device
 * BtStatus: (out)   result
 * ---------------------------------------------------------------------------
 */

BtStatus hdp_remove_connection(BD_ADDR *bdaddr)
{
	BtStatus ret = BT_STATUS_FAILED;
	OS_Report("[HDP]hdp_remove_connection: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
				bdaddr->addr[0], bdaddr->addr[1], bdaddr->addr[2], bdaddr->addr[3], bdaddr->addr[4], bdaddr->addr[5]);
	if (NULL == bdaddr)
	{
		ret = BT_STATUS_INVALID_PARM;
		goto exit;
	}
	HdpMcl *mcl = hdp_find_mcl(bdaddr);
	if (NULL == mcl)
	{
		ret = BT_STATUS_SUCCESS;
		OS_Report("No mcl is found for the device, which indicate success");
		goto exit;
	}
	if (NULL == mcl->mcapMcl)
	{
		OS_Report("MCAP mcl record is null, maybe previous operation is abnormal");
		hdp_mcl_delete(mcl);
		ret = BT_STATUS_SUCCESS;
		goto exit;
	}
	mcl->state = HDP_CHANNEL_STATE_DISCONNECTING;
	ret = MCAP_CloseMcl(mcl->mcapMcl);
	if (BT_STATUS_PENDING != ret)
	{
		hdp_mcl_delete(mcl);
	}	
	
exit:
	return ret;
}
/*---------------------------------------------------------------------------
 *            hdp_echo()
 *---------------------------------------------------------------------------
 *  
 * FOR PTS TEST, initiate to echo test
 *
 *---------------------------------------------------------------------------
 * BD_ADDR: (in)inication to remote device
 * HdpRole : (in) local role
 * BtStatus: (out)   result
 * ---------------------------------------------------------------------------
 */

BtStatus hdp_echo(HdpRole role, BD_ADDR *bdaddr)
{
	HdpChannel *channel = NULL;
	HdpInstance *instance = NULL;
	BtStatus ret;
	OS_Report("[HDP]hdp_echo:addr 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x, role is %d", 
				bdaddr->addr[0],bdaddr->addr[1],bdaddr->addr[2],bdaddr->addr[3],bdaddr->addr[4],bdaddr->addr[5],role);
	
	instance = hdp_find_echo_instance(role);
	if (NULL == instance) 
	{
		ret = BT_STATUS_INVALID_PARM;
	}
	else
	{
		channel = hdp_create_channel(instance, bdaddr, instance->channelType);
		ret = hdp_connect_channel(channel, FALSE);	
	}
	return ret;	
}



/*---------------------------------------------------------------------------
 *            hdp_send_data()
 *---------------------------------------------------------------------------
 *  
 *1. if channel is disconnected, reconnect and store packet to buffer. when channel is connected, the packet will be send automaticly (todo)
 *2. if channel is ready, directly send packet
 *3. if channel does not exist, return with error
 *
 *---------------------------------------------------------------------------
 * channelId : (in)indication to l2cap channel
 * BtStatus: (out)   result
 *---------------------------------------------------------------------------
 */

BtStatus hdp_send_data(BD_ADDR *bdaddr, U16 mdlId, void* data, U32 len)
{
	HdpMcl* mcl			= NULL;
	BtStatus ret;
	HdpDataPacket * dataPkt = NULL;
	HdpPkt* pkt 			= NULL;
	HdpChannel* channel = NULL;
	U16 offset = 0;
	OS_Report("[HDP]hdp_send_data:mdl id is %d, data len %d", mdlId, len);

	if (NULL == data || 0 == len)
	{
		ret = BT_STATUS_INVALID_PARM;
		OS_Report("data is null or len is zero");
		goto exit;
	}	
	channel = hdp_find_channel_by_mdlId(bdaddr, mdlId);
	if (NULL == channel || channel->state != HDP_CHANNEL_STATE_CONNECTED)
	{
		ret = BT_STATUS_INVALID_PARM;
		OS_Report("[HDP]err:channel is null or not in connected state");
		goto exit;	
	}

	//Notes: SAR is not available, so cut the data to apply for MTU before sending
	while(offset < len)
	{
		U16 sublen = hdp_util_min((len - offset), channel->mtu);
		dataPkt = hdp_data_packat_new(bdaddr, mdlId, ((U8*)data)+offset, sublen);
		if (NULL == data)
		{
			ret = BT_STATUS_NOSERVICES;
			OS_Report("[HDP] fail to add new packet to pool");
			goto exit;
		}
		else
		{
			ret = hdp_data_packat_list_add(dataPkt);
			if (BT_STATUS_SUCCESS != ret)
			{
				goto exit;
			}
		}
		offset += sublen;
	}
	
/*
	dataPkt = hdp_data_packat_new(l2capId, data, len);
	if (NULL == data)
	{
		ret = BT_STATUS_NOSERVICES;
		OS_Report("[HDP] fail to add new packet to pool");
		goto exit;
	}
	else
	{
		ret = hdp_data_packat_list_add(dataPkt);
		if (BT_STATUS_SUCCESS != ret)
		{
			goto exit;
		}
	}*/
	ret = hdp_handle_packet();
	
exit:
	return ret;
}


/*---------------------------------------------------------------------------
 *            hdp_init()
 *---------------------------------------------------------------------------
 */

BtStatus hdp_init(HdpCallback callback)
{
	BtStatus status; 
	McapRegister mcapInstance;
	OS_Report("[HDP]hdp_init");
	
	if (hdp_cntx.initialized) {
		return BT_STATUS_SUCCESS;
	}
	OS_MemSet(&hdp_cntx, 0x0, sizeof(HdpContext));
	hdp_cntx.initialized = TRUE;

	MCAP_Init();

	OS_MemSet(&mcapInstance, 0x0, sizeof(McapRegister));
	mcapInstance.profile = MCAPPROFILE_HDP;
	mcapInstance.callback = hdp_mcap_callback;
	status = MCAP_Register(&mcapInstance);

	if (status != BT_STATUS_SUCCESS) {
		hdp_cntx.initialized = FALSE;
		status = MCAP_DeRegister(HDP_PROFILE);
		goto exit;
	} 
//	hdp_cntx.mclPsm = mcapInstance.mclPsm;
//	hdp_cntx.mdlPsm = mcapInstance.mdlPsm;

	status = HdpSdpDB_ReplacePsm(mcapInstance.mclPsm, mcapInstance.mdlPsm);
	if (status != BT_STATUS_SUCCESS) {
		goto exit;
	}
	
	hdp_cntx.callback = callback;
	InitializeListHead(&hdp_cntx.mcl);
	InitializeListHead(&hdp_cntx.channel);
//	InitializeListHead(&hdp_cntx.pktPool);
	hdp_pool_init();
	hdp_data_packat_list_int();

	/*when inititating, register ECHO instance*/
	hdp_register_echo(HDP_ROLE_SINK);
	hdp_register_echo(HDP_ROLE_SOURCE);

//	InitializeListHead(&hdp_cntx.dataPkt);
exit:
	return status;
}

/*---------------------------------------------------------------------------
 *            hdp_deinit()
 *---------------------------------------------------------------------------
 *
 * TODO: before reseting contex, check and deregister instance
 *---------------------------------------------------------------------------
 */
BtStatus hdp_deinit(void)
{
	BtStatus status; 
	U8 index = 0;
	
	OS_Report("[HDP]hdp_deinit");
	if (!hdp_cntx.initialized)
	{
		OS_Report("[HDP]hdp has been deinitiated");
		return BT_STATUS_SUCCESS;
	}
	hdp_cntx.initialized = FALSE;

	hdp_deregister_all();
	
	status = MCAP_DeRegister(HDP_PROFILE);
	MCAP_DeInit();

	status = HdpSdpDB_DeRegistration();
	hdp_pool_deinit();
	hdp_data_packat_list_deint();
	
//	OS_MemSet(hdp_cntx, 0x0, sizeof(HdpContext));
	OS_Report("[HDP]the result is %d to deinit HDP", status);
	return status;
}

#endif /* __BT_HDP_PROFILE__ */
