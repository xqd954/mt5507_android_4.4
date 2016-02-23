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

#ifdef __BT_HDP_PROFILE__

#include "hdp.h"
extern HdpContext hdp_cntx;
static U8 currentMdepId = HDP_MDEP_ID_ECHO_TEST;
HdpPkt hdpPktPool[HDP_PACKET_POOL_MAX_VALUE];

/*---------------------------------------------------------------------------
 *           memory allocate/release utility
 *---------------------------------------------------------------------------
 */

void *hdp_util_malloc(U32 size)
{
	void *value = malloc (size);
	return value;
}

void hdp_util_free(void *value)
{
	if (NULL != value)
	{
		free(value);
	}
}



/*---------------------------------------------------------------------------
 *           device address comparison utility
 *---------------------------------------------------------------------------
 */

BOOL hdp_device_addr_cmp (BD_ADDR *bdaddr1, BD_ADDR *bdaddr2)
{
	U8 index = 0;
	if (NULL == bdaddr1 || NULL == bdaddr2)
	{
		return FALSE;
	}
	for (index = 0; index < 6; index ++)
	{
		if (bdaddr1->addr[index] != bdaddr2->addr[index])
		{
			return FALSE;
		}
	}
	return TRUE;
}

I8 hdp_util_convert_task2event(U8 task)
{
	switch(task)
	{
		case HDP_OPERATION_CONNECT:
			return HDP_EVENT_CONNECT_CNF;
			
		case HDP_OPERATION_DISCONNECT:
			return HDP_EVENT_DISCONNECT_CNF;

		default:
			return -1;			
	}
}

U8 hdp_util_convert_type2mode(HdpChannelType type)
{
	U8 l2capmode;
	switch(type)
	{
		case HDP_CHANNEL_NO_PREFERENCE:
		case HDP_CHANNEL_RELIABLE:
			l2capmode = L2MODE_ENHANCEDMENT_RETRANSMISSION;
			break;
			
		case HDP_CHANNEL_STREAMING:
			l2capmode = L2MODE_STREAMMING;
			break;
		
		default:
			l2capmode = HDP_INVALID_L2CAP_MODE;
	}
	return l2capmode;
}
/*---------------------------------------------------------------------------
 *           instance utility
 *---------------------------------------------------------------------------
 */

HdpInstance* hdp_find_instance(MdepId id)
{
	U8 index; 
	HdpInstance *instance;
	if (!hdp_cntx.initialized) 
	{
		return NULL;
	}
	for (index = 0; index < HDP_INSTANCE_MAX_NUM; index ++)
	{
		instance = &(hdp_cntx.instance[index]);
		if (HDP_INSTANCE_STATE_REGISTERED == instance->state && instance->feature.mdepId == id)
		{
			return instance;
		}
	}
	return NULL;
}
HdpInstance* hdp_find_registered_instance(HdpConfig *config)
{
	U8 index; 
	HdpInstance *instance;
	if (!hdp_cntx.initialized) 
	{
		return NULL;
	}
	for (index = 0; index < HDP_INSTANCE_MAX_NUM; index ++)
	{
		instance = &(hdp_cntx.instance[index]);
		if (HDP_INSTANCE_STATE_REGISTERED == instance->state && 
			instance->feature.dataType == config->datatype &&
			instance->feature.role == config->role &&
			instance->channelType == config->channelType &&
			(strncmp(instance->feature.descriotion, config->description, HDP_INSTNACE_DESC_MAX_LEN) == 0))
		{
			return instance;
		}
	}
	return NULL;
}
HdpInstance* hdp_find_echo_instance(HdpRole role)
{
	U8 index; 
	HdpInstance *instance;
	if (!hdp_cntx.initialized) 
	{
		return NULL;
	}
	for (index = 0; index < HDP_INSTANCE_MAX_NUM; index ++)
	{
		instance = &(hdp_cntx.instance[index]);
		if (HDP_INSTANCE_STATE_REGISTERED == instance->state 
			&& HDP_MDEP_ID_ECHO_TEST == instance->feature.mdepId
			&& role == instance->feature.role)
		{
			return instance;
		}
	}
	return NULL;
}

HdpInstance* hdp_find_echo_instance_by_remote_config(HdpChannelType remotetype)
{
	U8 index; 
	HdpInstance *instance;
	HdpRole localrole = HDP_ROLE_RESERVED;
	switch (remotetype)
	{
		case HDP_CHANNEL_NO_PREFERENCE:
			localrole = HDP_ROLE_SOURCE;
			break;
			
		case HDP_CHANNEL_STREAMING:
		case HDP_CHANNEL_RELIABLE:
			localrole = HDP_ROLE_SINK;
			break;
		
		default:
			localrole = HDP_ROLE_RESERVED;
	}
	if (HDP_ROLE_RESERVED != localrole)
	{
		return hdp_find_echo_instance(localrole);
	}
	else
	{
		return NULL;
	}
}




HdpInstance* hdp_find_available_instance()
{
	U8 index; 
	if (!hdp_cntx.initialized) 
	{
		return NULL;
	}
	for (index = 0; index < HDP_INSTANCE_MAX_NUM; index ++)
	{
		if (/*HDP_MDEP_ID_ECHO_TEST == hdp_cntx.instance[index].feature.mdepId && */
			HDP_INSTANCE_STATE_IDLE == hdp_cntx.instance[index].state)
		{
			return &(hdp_cntx.instance[index]);
		}
	}
	return NULL;
}
/*---------------------------------------------------------------------------
 *           mdep id allocate utility
 *---------------------------------------------------------------------------
 */

MdepId hdp_assign_mdep_id()
{
	while (TRUE)
	{
		currentMdepId = currentMdepId%HDP_MDEP_ID_MAX_VALUE + 1;
		if (NULL == hdp_find_instance(currentMdepId))
		{
			break;
		}
	}
	return currentMdepId;	
}

/*---------------------------------------------------------------------------
 *           mcl utility
 *---------------------------------------------------------------------------
 */

void show_address(BD_ADDR *bdaddr)
{
	OS_Report("[HDP]show_address: %x, %x, %x, %x, %x, %x"
		, bdaddr->addr[0], bdaddr->addr[1], bdaddr->addr[2], bdaddr->addr[3], bdaddr->addr[4], bdaddr->addr[5]);
}
HdpMcl* hdp_find_mcl(BD_ADDR *bdaddr)
{	
	HdpMcl* nextMcl = (HdpMcl*) GetHeadList(&hdp_cntx.mcl);
	OS_Report("[HDP]hdp_find_mcl");
	if (IsListEmpty(&hdp_cntx.mcl))
	{
		OS_Report("[HDP]mcl list is null");
		return NULL;
	}
	do {
		if (hdp_device_addr_cmp(&(nextMcl->bdAddr), bdaddr)) 
		{
			return nextMcl;
		}
   		nextMcl = (HdpMcl*) GetNextNode(&nextMcl->node);
	} while (nextMcl != (HdpMcl*)&hdp_cntx.mcl);
	return NULL;
}

HdpMcl *hdp_mcl_new(BD_ADDR *bdaddr)
{
	HdpMcl *mcl = (HdpMcl *)hdp_util_malloc(sizeof(HdpMcl));
	if (NULL != mcl)
	{
		OS_MemSet(mcl, 0x0, sizeof(HdpMcl));		
		InsertTailList(&hdp_cntx.mcl, &mcl->node);
		OS_MemCopy(&mcl->bdAddr, bdaddr, sizeof(BD_ADDR));
	}
	OS_Report("[HDP]hdp_mcl_new: %d", mcl);
	return mcl;
}

void hdp_mcl_delete(HdpMcl *mcl)
{
	OS_Report("[HDP]hdp_mcl_delete: %d", mcl);
	if(!IsNodeOnList(&hdp_cntx.mcl,&mcl->node))
	{
		OS_Report("[HDP]the MCL is not in list");
		return;		
	}
	RemoveEntryList(&mcl->node);
	if (NULL != mcl)
	{
		hdp_util_free(mcl); //Notes: mcapmcl will be released by mcap
	}	
}
/*---------------------------------------------------------------------------
 *           hdp channel utility
 *---------------------------------------------------------------------------
 */

BOOL hdp_channel_check_active(HdpChannel *channel)
{
	return IsNodeOnList(&hdp_cntx.channel ,&(channel->node));
}



HdpChannel *hdp_find_channel_by_mdlId(BD_ADDR *bdaddr, U16 mdlId)
{

	OS_Report("[HDP]hdp_find_channel_by_mdlId: id  %d",mdlId);

	if (IsListEmpty(&hdp_cntx.channel))
	{
		OS_Report("hdp channel does not init");
		return NULL;
	}
	HdpChannel* nextChn = (HdpChannel *) GetHeadList(&hdp_cntx.channel);
	do {
   		if (hdp_device_addr_cmp(&(nextChn->bdAddr), bdaddr) &&
			nextChn->mcapmdl &&
			mdlId == nextChn->mcapmdl->mdlId)
		{
			return nextChn;
		}
		nextChn = (HdpChannel *) GetNextNode(&nextChn->node);		
	} while ((ListEntry *)nextChn != &hdp_cntx.channel);
	
	return NULL;
}


void dump_channel()
{
	OS_Report("[HDP]dump_channel");
	if (IsListEmpty(&hdp_cntx.channel))
		{
			return;
		}
		HdpChannel* nextChn = (HdpChannel *) GetHeadList(&hdp_cntx.channel);
		do {			
			OS_Report("[HDP]channel[%p], state[%d],l2capid[%d], mdl[%d], addr[%x]", 
						nextChn, 
						nextChn->state, 
						nextChn->mcapmdl->l2ChannelId,
						nextChn->mcapmdl->mdlId,
						nextChn->bdAddr);
			nextChn = (HdpChannel *) GetNextNode(&nextChn->node);		
		} while ((ListEntry *)nextChn != &hdp_cntx.channel);
		

}

HdpChannel *hdp_find_channel_by_l2capId(U16 l2capId)
{
	if (IsListEmpty(&hdp_cntx.channel))
	{
		return NULL;
	}
	HdpChannel* nextChn = (HdpChannel *) GetHeadList(&hdp_cntx.channel);
	do {
   		if (nextChn->mcapmdl &&
			l2capId == nextChn->mcapmdl->l2ChannelId)
		{
			return nextChn;
		}
		nextChn = (HdpChannel *) GetNextNode(&nextChn->node);		
	} while ((ListEntry *)nextChn != &hdp_cntx.channel);
	
	return NULL;
}

HdpChannel *hdp_find_channel_by_state(BD_ADDR *bdaddr, HdpChannelState state)
{
	OS_Report("[HDP]hdp_find_channel_by_state: state  %d",state);

	if (IsListEmpty(&hdp_cntx.channel))
	{
	OS_Report("hdp channel does not init");
		return NULL;
	}
	HdpChannel* nextChn = (HdpChannel *) GetHeadList(&hdp_cntx.channel);
	do {
   		if (hdp_device_addr_cmp(&(nextChn->bdAddr), bdaddr) &&
			state == nextChn->state)
		{
			return nextChn;
		}
		nextChn = (HdpChannel *) GetNextNode(&nextChn->node);		
	} while ((ListEntry *)nextChn != &hdp_cntx.channel);
	
	return NULL;

}

HdpChannel *hdp_find_channel_by_mdep_id(U8 mdepid)
{
	OS_Report("[HDP]hdp_find_channel_by_mdep_id: state  %d",mdepid);

	if (IsListEmpty(&hdp_cntx.channel))
	{
		OS_Report("hdp channel is empty");
		return NULL;
	}
	HdpChannel* nextChn = (HdpChannel *) GetHeadList(&hdp_cntx.channel);
	do {
   		if (nextChn->instance->feature.mdepId == mdepid)
		{
			return nextChn;
		}
		nextChn = (HdpChannel *) GetNextNode(&nextChn->node);		
	} while ((ListEntry *)nextChn != &hdp_cntx.channel);
	
	return NULL;

}



/*---------------------------------------------------------------------------
 *           l2cap packet pool utility
 *---------------------------------------------------------------------------
 */


BtStatus hdp_pool_pkt_add(HdpPkt* pkt)
{
	BtStatus ret = BT_STATUS_FAILED;
	OS_Report("[HDP][UTIL] hdp_pool_pkt_add: IsNodeOnList %d", IsNodeOnList(&hdp_cntx.pktPool, &pkt->node));
	if ((NULL != pkt) &&
		(FALSE == IsNodeOnList(&hdp_cntx.pktPool, &pkt->node))) 
	{
		pkt->state = HDP_PKT_IDLE;
		InsertTailList(&hdp_cntx.pktPool,&pkt->node);
		ret = BT_STATUS_SUCCESS;
	}
	else
	{
		OS_Report("[HDP][UTIL]fail to add pkt to list");
	}
	OS_Report("[HDP][UTIL] IsListEmpty %d", IsListEmpty(&hdp_cntx.pktPool));
	return ret;
}
HdpPkt* hdp_pool_pkt_remove(void)
{
	HdpPkt* pkt = NULL;
	if (0 == IsListEmpty(&hdp_cntx.pktPool)) 
	{
		pkt = (HdpPkt*)RemoveHeadList(&hdp_cntx.pktPool);
		pkt->state = HDP_PKT_IN_USE;
	}
	else
	{
		OS_Report("[HDP][UTIL]hdp_pool_pkt_remove: pktPool is empty");
	}
	return pkt;
}
BtStatus hdp_pool_init()
{
	U8 index = 0;
	OS_MemSet(hdpPktPool, 0x0, sizeof(HdpPkt)*HDP_PACKET_POOL_MAX_VALUE);
	InitializeListHead(&hdp_cntx.pktPool);
	for (index = 0; index < HDP_PACKET_POOL_MAX_VALUE; index ++)
	{
		hdp_pool_pkt_add(&hdpPktPool[index]);
	}
	return BT_STATUS_SUCCESS;
}
BtStatus hdp_pool_deinit()
{
	U8 index = 0;
	HdpPkt* pkt = NULL;
	do {
		pkt = hdp_pool_pkt_remove();
	} while (NULL != pkt);	
	OS_MemSet(hdpPktPool, 0x0, sizeof(HdpPkt)*HDP_PACKET_POOL_MAX_VALUE);
	return BT_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *           data packet pool utility
 *---------------------------------------------------------------------------
 */

BtStatus hdp_data_packat_list_add(HdpDataPacket * pkt)
{	
	BtStatus ret = BT_STATUS_FAILED;
	OS_Report("[HDP][UTIL]hdp_data_packat_list_add: addr is %d", pkt);
	if (NULL != pkt)
	{
		InsertTailList(&hdp_cntx.dataPkt,&pkt->node);
		ret = BT_STATUS_SUCCESS;
	}
	return ret;
}
HdpDataPacket *hdp_data_packat_list_remove()
{	
	HdpDataPacket * pkt = NULL;
	if (0 == IsListEmpty(&hdp_cntx.dataPkt))
	{
		pkt = (HdpDataPacket*)RemoveHeadList(&hdp_cntx.dataPkt);
	}
	return pkt;
}

BtStatus hdp_data_packat_delete(HdpDataPacket *pkt, BOOL forceDeleteData)
{
	OS_Report("[HDP][UTIL]hdp_data_packat_delete: addr is %d, force to Delete? %d", pkt, forceDeleteData);

	if (NULL != pkt)
	{
		hdp_util_free(pkt);
		if (forceDeleteData && NULL != pkt->data.data)
		{
			hdp_util_free(pkt->data.data);
			pkt->data.data = NULL;
		}
		else
		{
			OS_Report("[HDP][UTIL]The packet content is still alive"); 
		}
	}	
	return BT_STATUS_SUCCESS;
}


HdpDataPacket *hdp_data_packat_new(BD_ADDR *addr, U16 mdlid,void* value, U16 len)
{
	HdpDataPacket * pkt = NULL;
	if (NULL == value || 0 == len)
	{
		return NULL;
	}
	pkt = (HdpDataPacket *)hdp_util_malloc(sizeof(HdpDataPacket));
	if (NULL != pkt)
	{
		memcpy(pkt->addr.addr, addr->addr, BD_ADDR_SIZE);
		pkt->mdlId = mdlid;
		pkt->data.data	= (U8 *)hdp_util_malloc(len);
		if (NULL == pkt->data.data)
		{
			hdp_data_packat_delete(pkt, TRUE);
			pkt = NULL;
		}
		else
		{
			OS_MemCopy(pkt->data.data, value, len);
			pkt->data.dataLen = len;
		}
	}
	return pkt;
}

void hdp_data_packat_list_remove_by_channel(BD_ADDR *addr, U16 mdlid)
{	
	HdpDataPacket * pkt = NULL;
	HdpDataPacket * nextpkt = NULL;

	OS_Report("[HDP][UTIL]hdp_data_packat_list_remove_by_channel: mdl id is %d", mdlid);
	
	if (IsListEmpty(&hdp_cntx.dataPkt))
	{
		OS_Report("packet list is empty");
		return;
	}
	
	nextpkt = (HdpDataPacket *) GetHeadList(&hdp_cntx.dataPkt);
	do
	{
		pkt = nextpkt;
		nextpkt = (HdpDataPacket *) GetNextNode(&nextpkt->node);	
   		if (hdp_device_addr_cmp(&(pkt->addr), addr) &&
			pkt->mdlId == mdlid)
		{
			OS_Report("[HDP][UTIL]HDP packet is found");
			RemoveEntryList(&pkt->node);
			hdp_data_packat_delete(pkt, TRUE);
		}	
	} while ((ListEntry *)nextpkt != &hdp_cntx.dataPkt);
}

BtStatus hdp_data_packat_list_int()
{
	InitializeListHead(&hdp_cntx.dataPkt);
	return BT_STATUS_SUCCESS;
}
BtStatus hdp_data_packat_list_deint()
{
	HdpDataPacket* pkt = NULL;
	do {
		pkt = hdp_data_packat_list_remove();
		hdp_data_packat_delete(pkt, TRUE);
	} while (NULL != pkt);	
	return BT_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *           uuid conversion utility
 *---------------------------------------------------------------------------
 */

/*role: local role*/
/*return expected uuid*/
U16 hdp_get_target_uuid(HdpRole role)
{
	switch(role)
	{
		case HDP_ROLE_SINK:
			return SC_HDP_SOURCE;
		case HDP_ROLE_SOURCE:
			return SC_HDP_SINK;
		default:
			return 0;
	}
}
/*---------------------------------------------------------------------------
 *           HDP configuration check utility
 *---------------------------------------------------------------------------
 */


BOOL hdp_check_outgoing_config(HdpRole localRole, HdpChannelType localSetting, HdpChannelType remoteSetting)
{
	switch(localRole)
	{
		case HDP_ROLE_SINK:
			return HDP_CHANNEL_RELIABLE == remoteSetting ||
					HDP_CHANNEL_STREAMING == remoteSetting;
			
		case HDP_ROLE_SOURCE:
			return localSetting == remoteSetting;
			break;
			
		default:
			return FALSE;
	}
}



BOOL hdp_check_incoming_config(HdpRole localRole, HdpChannelType localSetting, HdpChannelType remoteSetting)
{
	switch(localRole)
	{
		case HDP_ROLE_SINK:
			return HDP_CHANNEL_RELIABLE == remoteSetting ||
					HDP_CHANNEL_STREAMING == remoteSetting;
			
		case HDP_ROLE_SOURCE:
			return HDP_CHANNEL_NO_PREFERENCE == remoteSetting;
			
		default:
			return FALSE;
	}
}

HdpChannelType hdp_convert_config_response(HdpRole localRole, HdpChannelType localSetting, HdpChannelType remoteSetting)
{
	switch(localRole)
	{
		case HDP_ROLE_SINK:
			return remoteSetting;
			
		case HDP_ROLE_SOURCE:
			return localSetting;
			
		default:
			return -1;
	}
}

/*Notes: The first MDL channel must be Reliable*/
HdpChannelType hdp_get_local_config(HdpInstance *instance, BD_ADDR *bdaddr)
{
	if (instance->feature.role == HDP_ROLE_SOURCE &&
		hdp_find_channel_by_state(bdaddr, HDP_CHANNEL_STATE_CONNECTED) == NULL)
	{
		return HDP_CHANNEL_RELIABLE;
	}
	else
	{
		return instance->channelType;
	}
}

BOOL hdp_check_config_valid(HdpRole localRole, HdpChannelType localSetting)
{
	switch(localRole)
	{
		case HDP_ROLE_SOURCE:
			return HDP_CHANNEL_RELIABLE == localSetting ||
					HDP_CHANNEL_STREAMING == localSetting;
			
		case HDP_ROLE_SINK:
			return HDP_CHANNEL_NO_PREFERENCE == localSetting;
			
		default:
			return FALSE;
	}
}


void hdp_set_channel_state(HdpChannel *channel, HdpChannelState state)
{
	Assert(channel != NULL);
	channel->state = state;
}

BOOL hdp_util_role_valid(HdpRole role)
{
	return (role == HDP_ROLE_SINK) || (role == HDP_ROLE_SOURCE);
}

U16 hdp_util_min(U16 value1, U16 value2)
{
	return (value1 < value2) ? value1 : value2;
}

#endif /* __BT_HDP_PROFILE__ */
