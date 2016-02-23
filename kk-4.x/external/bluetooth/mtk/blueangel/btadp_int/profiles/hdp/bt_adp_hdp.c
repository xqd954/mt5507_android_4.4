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

#include "bt_adp_hdp.h"
#include "hdp.h"
#include "bt_mmi.h"
#include <sys/socket.h>
#include <errno.h>
#include "cutils/sockets.h"
#include "bluetooth_hdp_struct.h"
#include "bt_message.h"
#include "bt_adp_hdp_util.h"
#include "btutils.h"
#include "bt_session.h"

hdp_adp_context_struct hdp_adp_context;
extern int g_serverSocket;

#define DEBUG FALSE

void hdp_adp_send_data(U16 watermark,void * data,U32 len);

/*****************************************************************************
 * FUNCTION
 *  hdp_send_msg
 * DESCRIPTION
 *  send message to external task.
 * PARAMETERS
 *  msg             [IN]        
 *  dstMod          [IN]        
 *  sap             [IN]        
 *  local_para      [IN]         
 *  peer_buff       [IN]         
 * RETURNS
 *  void
 *****************************************************************************/
void hdp_send_msg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{
    if (local_para != NULL)
    {
        BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
    }
    else
    {
        BT_SendMessage(msg, dstMod, NULL, 0);
    }
}


//TRUE: the data is from HDP instance
//FALSE: the data is not from HDP intance

BOOL hdp_adp_check_socket_address(const char *source)
{
	if (NULL != source && 
		!OS_StrnCmp(source, BT_SOCK_NAME_EXT_ADP_HDP_DATA_PREFIX, OS_StrLen(BT_SOCK_NAME_EXT_ADP_HDP_DATA_PREFIX)))
	{
		return TRUE;
	} 
	else
	{
		return FALSE;
	}

}

//TRUE: the data is from HDP instance
//FALSE: the data is not from HDP intance
BOOL hdp_adp_get_data_channel(const char *source, U16 *watermark)
{
 	if (NULL != source &&
		!OS_StrnCmp(source, BT_SOCK_NAME_EXT_ADP_HDP_DATA_PREFIX, OS_StrLen(BT_SOCK_NAME_EXT_ADP_HDP_DATA_PREFIX)))
	{
		*watermark = atoi(source + OS_StrLen(BT_SOCK_NAME_EXT_ADP_HDP_DATA_PREFIX));
		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

void hdp_print_channel()
	{
		U8 index = 0;

		if (!DEBUG)
		{
			return;
		}
		OS_Report("hdp_print_channel");
		hdp_adp_channel_struct * channel = NULL;
		for (index = 0; index < HDP_ADP_MAX_CHANNEL_NUM; index++)
		{
			channel = &hdp_adp_context.channels[index];
			OS_Report("index :%d", index);
			OS_Report("state :%d", channel->state);
			OS_Report("BDARR: %x %x %x", channel->bdaddr.lap, channel->bdaddr.uap, channel->bdaddr.nap);
			OS_Report("l2cap id :%d", channel->l2capChnId );
			OS_Report("mdepId :%d", channel->mdepId);
			OS_Report("mdlId :%d", channel->mdlId);
			
		}
	}

BOOL hdp_deQueue_data(U8 *source, void *data, U32 size)
{
	struct sockaddr_un addr;
	socklen_t   addrLen;
	U16 watermark;
	BOOL ret = FALSE;
	
	OS_Report("hdp_deQueue_data: source is %s, data size is %d", source, size);
	if(hdp_adp_get_data_channel((const char*)source, &watermark))
    {
    	hdp_adp_send_data(watermark , data, size);
		ret = TRUE;
    }
	else
	{
		OS_Report("[HDP][ADP]fail to get l2cap channel id");
	}
    return ret;
}





void hdp_adp_register_instance(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	bt_hdp_register_instance_req_struct *msg = (bt_hdp_register_instance_req_struct *) ilm_ptr->local_para_ptr;
    bt_hdp_register_instance_cnf_struct *cnf = 
        (bt_hdp_register_instance_cnf_struct *) construct_local_para(sizeof(bt_hdp_register_instance_cnf_struct), TD_UL | TD_RESET);
	hdp_adp_instance_struct *instance = NULL;
	U8 id = HDP_INVALID_MDEP_ID;
	HdpConfig config;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	
	instance = hdp_util_find_available_instance();
	if (NULL == instance)
	{
		cnf->result = BT_HDP_FAIL_NO_RESOURCE;
		goto exit;
	}
	OS_MemSet((kal_uint8 *)&config , 0x0, sizeof(HdpConfig));
	config.role = msg->role;
	config.channelType =  msg->channelType;
	config.datatype = msg->dataType;
	if (OS_StrLen(msg->description) > 0)
	{
		OS_StrnCpy(config.description, msg->description, HDP_INSTNACE_DESC_MAX_LEN);
	}
	id = hdp_register(&config);
	if (HDP_INVALID_MDEP_ID != id)
	{
		instance->single = TRUE;
		instance->mdepId = id;
		
		cnf->result = BT_HDP_SUCCESS;
		cnf->mdepId = id;		
	}
	else
	{
		cnf->result = BT_HDP_FAIL;
		OS_Report("fail to register instance");
	}
	cnf->app_id = msg->app_id;
exit:
	hdp_send_msg(MSG_ID_BT_HDP_REGISTER_INSTANCE_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);
	
}
void hdp_adp_deregister_instance(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	bt_hdp_deregister_instance_req_struct *msg = (bt_hdp_deregister_instance_req_struct *) ilm_ptr->local_para_ptr;
    bt_hdp_deregister_instance_cnf_struct *cnf = 
        (bt_hdp_deregister_instance_cnf_struct *) construct_local_para(sizeof(bt_hdp_deregister_instance_cnf_struct), TD_UL | TD_RESET);
	hdp_adp_instance_struct *instance = hdp_util_find_instance(msg->mdepId);
	BtStatus ret;
	HdpConfig config;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	if (NULL == instance)
	{
		ret = BT_STATUS_INVALID_PARM;
		goto exit;
	}
	ret = hdp_deregister(msg->mdepId);
	if (BT_STATUS_SUCCESS == ret)
	{
		instance->single = FALSE;
		OS_Report("[HDP]deregister instance: %d", ret);
	}
	else
	{
		OS_Report("[HDP]fail to deregister instance: %d", ret);
	}
exit:
	if (BT_STATUS_PENDING != ret)
	{
		cnf->result = hdp_util_convert_status(ret);
		cnf->mdepId = msg->mdepId;
		hdp_send_msg(MSG_ID_BT_HDP_DEREGISTER_INSTANCE_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);
	}

}

void hdp_adp_connect(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	bt_hdp_connect_req_struct *msg = (bt_hdp_connect_req_struct *) ilm_ptr->local_para_ptr;
    bt_hdp_connect_cnf_struct *cnf = NULL;        
	BD_ADDR bdaddr;
	hdp_adp_channel_struct *channel = NULL;	
	BtStatus ret;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	if (FALSE == hdp_util_instance_exist(msg->mdepId))
	{
		ret = BT_STATUS_NOT_FOUND;
		OS_Report("[HDP][ADP]Err No instance is found");
		goto exit;
	}
	
	CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &msg->bdaddr);

	//check channel state
/*	if (hdp_util_find_main_channel(&msg->bdaddr) != NULL)
	{
		//the main channel already exist
		ret = BT_STATUS_SUCCESS;
		goto exit;
	}*/
	channel = hdp_util_find_available_channel();	
	if (NULL == channel)
	{
		OS_Report("[HDP][ADP]Err No available channel is found");
		ret = BT_STATUS_NO_RESOURCES;
		goto exit;
	}
	hdp_channel_init(channel);
	OS_MemCopy((kal_uint8 *)&channel->bdaddr, (kal_uint8 *)&msg->bdaddr, sizeof(bt_addr_struct));
	channel->mdepId = msg->mdepId;
	channel->state = HDP_ADP_CHANNEL_CONNECTING;
	channel->index = msg->index;
	channel->waterMark = hdp_util_get_channel_water_mark();
	hdp_util_add_channel(channel);
	//debug
	hdp_print_channel();
	
	ret = hdp_open_channel(msg->mdepId, &bdaddr);
	if (BT_STATUS_PENDING == ret)
	{
		return;
	} 
	else if (BT_STATUS_SUCCESS == ret)
	{
		channel->state = HDP_ADP_CHANNEL_CONNECTED;
	}
	else
	{
		hdp_util_remove_channel(channel);
		OS_MemSet((kal_uint8 *)channel, 0x0, sizeof(hdp_adp_channel_struct));
	}
	//debug
	hdp_print_channel();
exit:
	cnf = (bt_hdp_connect_cnf_struct *) construct_local_para(sizeof(bt_hdp_connect_cnf_struct), TD_UL | TD_RESET);
	cnf->result = hdp_util_convert_status(ret);
	cnf->mdepId = msg->mdepId;
	cnf->index = msg->index;
	OS_MemCopy((kal_uint8 *)&cnf->bdaddr, (kal_uint8 *)&msg->bdaddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_CONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);

}

void hdp_adp_connect_rsp(ilm_struct  *ilm)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bt_hdp_channel_opened_rsp_struct *rsp = (bt_hdp_channel_opened_rsp_struct *)ilm->ilm_data;
	hdp_adp_channel_struct* channel = hdp_util_find_channel_by_mdl_id(&rsp->bdaddr, rsp->mdlId);
	BtStatus ret;

	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP] hdp_adp_connect_rsp");

	if (NULL == channel || channel->state == HDP_ADP_CHANNEL_IDLE)
	{		
		OS_Report("[HDP]no channel for this connection response");
		ret = BT_STATUS_FAILED;
		return;
		//TODO: disconnect the channel??
	}

	channel->sessionId = rsp->sessionId;
	hdp_channel_lower_layer_incoming(channel, NULL, 0);
	hdp_channel_upper_data_incoming(channel->sessionId);
}

void hdp_adp_send_data_req(ilm_struct  *ilm)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bt_hdp_send_data_req_struct *req = (bt_hdp_send_data_req_struct *)ilm->ilm_data;
	hdp_adp_channel_struct* channel = hdp_util_find_channel_by_mdl_id(&req->bdaddr, req->mdlId);
	BtStatus ret;
	int length = 0;
	U8						buffer[1024];
	BD_ADDR					bdaddr;

	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP] hdp_adp_send_data_req");

	if (NULL == channel || channel->state == HDP_ADP_CHANNEL_IDLE)
	{		
		OS_Report("[HDP]no channel for this connection response");
		ret = BT_STATUS_FAILED;
		return;
		//TODO: disconnect the channel??
	}

	//TODO: loop to read data
	while(TRUE)
	{
		length = bt_session_read(channel->sessionId, buffer, channel->mtu);
		if (length <= 0)
		{			
			break;
		}
		CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &channel->bdaddr);
		hdp_send_data(&bdaddr, channel->mdlId, buffer, length);
	}
}


void hdp_adp_disconnect(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bt_hdp_disconnect_req_struct *msg = (bt_hdp_disconnect_req_struct *) ilm_ptr->local_para_ptr;
    bt_hdp_disconnect_cnf_struct *cnf = NULL;     
	BD_ADDR bdaddr;
	BtStatus ret;
	hdp_adp_channel_struct *channel = hdp_util_find_channel_by_mdl_id(&msg->bdaddr, msg->mdlId);
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/

	if (NULL == channel || channel->state == HDP_ADP_CHANNEL_IDLE)
	{
		ret = BT_STATUS_FAILED;
		goto exit;
	}
	if (channel->state == HDP_ADP_CHANNEL_DISCONNECTING)
	{
		ret = BT_STATUS_IN_PROGRESS;
		goto exit;
	}
	CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &msg->bdaddr);
	ret = hdp_close_channel(&bdaddr, msg->mdlId);
	if (BT_STATUS_PENDING == ret)
	{
		channel->state = HDP_ADP_CHANNEL_DISCONNECTING;
		return;		
	} 
	else if (BT_STATUS_SUCCESS == ret)
	{
		channel->state = HDP_ADP_CHANNEL_IDLE;
		hdp_util_remove_channel(channel);
		OS_MemSet((kal_uint8 *)channel, 0x0, sizeof(hdp_adp_channel_struct));
	}
	else
	{
		OS_Report("[HDP][ADP]fail to disconnect with remote device");
		channel->state = HDP_ADP_CHANNEL_CONNECTED;
	}
exit:
	cnf = (bt_hdp_disconnect_cnf_struct *) construct_local_para(sizeof(bt_hdp_disconnect_cnf_struct), TD_UL | TD_RESET);
	cnf->result = hdp_util_convert_status(ret);
	cnf->mdlId = msg->mdlId;
	cnf->index = msg->index;
	OS_MemCopy((kal_uint8 *)&cnf->bdaddr, (kal_uint8 *)&msg->bdaddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_DISCONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);	

}

void hdp_adp_remove_connection(ilm_struct *ilm_ptr)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bt_hdp_remove_connection_req_struct *msg = (bt_hdp_remove_connection_req_struct *) ilm_ptr->local_para_ptr;
	BD_ADDR bdaddr;
	hdp_adp_channel_struct *channel = NULL;
  	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP] hdp_adp_remove_connection, type[%d], data_chn_index[%d]", msg->con_type, msg->value);

	if (BT_CONNECTION_TYPE_CONTROL == msg->con_type)
	{
		CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &msg->bdaddr);
		hdp_remove_connection(&bdaddr);
	}
	else if (BT_CONNECTION_TYPE_DATA == msg->con_type)
	{

		switch(msg->data_chn_type)
		{
		case BT_HDP_DATA_CONN_TYPE_DEFAULT:
			channel = hdp_util_find_main_channel(&msg->bdaddr);
			break;
		
		case BT_HDP_DATA_CONN_TYPE_INDEX:
			channel = hdp_util_find_channel_by_index(&msg->bdaddr, msg->value);
			break;
		
		case BT_HDP_DATA_CONN_TYPE_MDL_ID:
			//This API can not get channel with specified mdl id
			//	channel = hdp_util_find_channel_by_mdl_id(&msg->bdaddr, msg->value);
			break;
		
		default:
			OS_Report("[HDP][ADP] invalid data channel type");
		}
		
		if (NULL == channel)
		{
			OS_Report("[HDP][ADP] there is no matched channel");
		}
		else if (HDP_ADP_CHANNEL_CONNECTED != channel->state)
		{
			OS_Report("[HDP][ADP] matched channel is not connected, state is %d", channel->state);
		} else {
			OS_Report("[HDP][ADP] matched channel l2cap id is %d", channel->l2capChnId);
			L2CAP_DisconnectReq(channel->l2capChnId);		
		}
	}
	else
	{
		OS_Report("[HDP][ADP] invalid conn type %d", msg->con_type);
	}
		
} 
void hdp_adp_reopen_connection(ilm_struct *ilm_ptr)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bt_hdp_reopen_connection_req_struct *msg = (bt_hdp_reopen_connection_req_struct *) ilm_ptr->local_para_ptr;
	BD_ADDR bdaddr;
	BtStatus ret;
	hdp_adp_channel_struct *channel = NULL;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP] hdp_adp_reopen_connection: type = %d, value= %d", msg->data_chn_type, msg->value);
	switch(msg->data_chn_type)
	{
	case BT_HDP_DATA_CONN_TYPE_DEFAULT:
		channel = hdp_util_find_main_channel(&msg->bdaddr);
		break;
		
	case BT_HDP_DATA_CONN_TYPE_INDEX:
		channel = hdp_util_find_channel_by_index(&msg->bdaddr, msg->value);
		break;
		
	case BT_HDP_DATA_CONN_TYPE_MDL_ID:
		//This API can not get channel with specified mdl id
	//	channel = hdp_util_find_channel_by_mdl_id(&msg->bdaddr, msg->value);
		break;
		
	default:
		OS_Report("[HDP][ADP] invalid reopen type");
	}
	
	if (NULL == channel)
	{		
		OS_Report("[HDP][ADP] there is no matched channel");
	}	
	else if (HDP_ADP_CHANNEL_CONNECTED == channel->state)
	{
		OS_Report("[HDP][ADP] matched channel has been connected, %d", channel);
	} else {
		CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &msg->bdaddr);
		ret = hdp_reopen_channel(&bdaddr, channel->mdlId);
		if (BT_STATUS_SUCCESS != ret && BT_STATUS_PENDING != ret)
		{
			hdp_util_remove_channel(channel);
			OS_MemSet((kal_uint8 *)channel, 0x0, sizeof(hdp_adp_channel_struct));	
		}
	}
}


void hdp_adp_send_data(U16 watermark,void * data,U32 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/  
	BD_ADDR bdaddr;
	BtStatus ret;
	hdp_adp_channel_struct *channel = hdp_util_find_channel_by_water_mark(watermark);
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP] hdp_adp_send_data: channel watermark is %d", watermark);

	hdp_print_channel();

	if (NULL == channel)
	{
		OS_Report("[HDP] fail to find channel");
		return;
	}	
	CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &channel->bdaddr);
	hdp_send_data(&bdaddr, channel->mdlId, data, len);
}


void hdp_adp_get_main_channel(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	bt_hdp_get_main_channel_req_struct *msg = (bt_hdp_get_main_channel_req_struct *) ilm_ptr->local_para_ptr;
	bt_hdp_get_main_channel_cnf_struct *cnf = 
		(bt_hdp_get_main_channel_cnf_struct *) construct_local_para(sizeof(bt_hdp_get_main_channel_cnf_struct), TD_UL | TD_RESET);
	BtStatus ret;
	hdp_adp_channel_struct *channel = NULL;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/	
	channel = hdp_util_find_main_channel(&msg->bdaddr);
	if (NULL != channel)
	{
		ret = BT_STATUS_SUCCESS;
		cnf->mdlId = channel->mdlId;
		OS_MemCopy((kal_uint8 *)&cnf->bdaddr, (kal_uint8 *)&msg->bdaddr, sizeof(bt_addr_struct));
	}
	else
	{
		ret = BT_STATUS_FAILED;
	}
	cnf->result = hdp_util_convert_status(ret);
	hdp_send_msg(MSG_ID_BT_HDP_GET_MAIN_CHANNEL_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);

}

void hdp_adp_get_instance(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bt_hdp_get_instance_req_struct *msg = (bt_hdp_get_instance_req_struct *) ilm_ptr->local_para_ptr;
	bt_hdp_get_instance_cnf_struct *cnf = 
		(bt_hdp_get_instance_cnf_struct *) construct_local_para(sizeof(bt_hdp_get_instance_cnf_struct), TD_UL | TD_RESET);
	BtStatus ret;
	hdp_adp_channel_struct *channel = hdp_util_find_channel_by_mdl_id(&msg->bdaddr, msg->mdlId);

	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/

	if (NULL != channel)
	{
		ret = BT_STATUS_SUCCESS;
		cnf->mdepId = channel->mdepId;
	}
	else
	{
		ret = BT_STATUS_FAILED;
	}
	cnf->result = hdp_util_convert_status(ret);
	hdp_send_msg(MSG_ID_BT_HDP_GET_INSTANCE_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

void hdp_adp_get_channel_id(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	bt_hdp_get_l2cap_channel_req_struct *msg = (bt_hdp_get_l2cap_channel_req_struct *) ilm_ptr->local_para_ptr;
	bt_hdp_get_l2cap_channel_cnf_struct *cnf = 
		(bt_hdp_get_l2cap_channel_cnf_struct *) construct_local_para(sizeof(bt_hdp_get_l2cap_channel_cnf_struct), TD_UL | TD_RESET);
	BtStatus ret;
	hdp_adp_channel_struct *channel = hdp_util_find_channel_by_mdl_id(&msg->bdaddr, msg->mdlId);	
	BD_ADDR bdaddr;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP] hdp_adp_get_channel_id: addr is 0x%x : 0x%x:0x%x, mdl id is %d", 
				msg->bdaddr.lap, msg->bdaddr.uap, msg->bdaddr.nap, msg->mdlId);
	if (NULL != channel)
	{
		if (channel->state == HDP_ADP_CHANNEL_CONNECTED)
		{		
			ret = BT_STATUS_SUCCESS;
			cnf->l2capId = channel->waterMark;
			cnf->mdlId = channel->mdlId;
		}
		else
		{
			//TODO: reconnect mdl if mdl state is disconnected
			//hdp_open_channel(MdepId id,BD_ADDR * bdaddr);
			CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &msg->bdaddr);
			ret = hdp_reopen_channel(&bdaddr, channel->mdlId);
		}
	}
	else
	{
		ret = BT_STATUS_FAILED;
	}
	cnf->result = hdp_util_convert_status(ret);
	OS_MemCopy((kal_uint8 *)&cnf->bdaddr, (kal_uint8 *)&msg->bdaddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_GET_L2CAP_CHANNEL_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);

}

void hdp_adp_echo(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

	bt_hdp_echo_req_struct *msg = (bt_hdp_echo_req_struct *) ilm_ptr->local_para_ptr;
	bt_hdp_echo_cnf_struct *cnf = NULL;
	BtStatus ret;	
	BD_ADDR bdaddr;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP] hdp_adp_echo: addr is 0x%x : 0x%x:0x%x, role is %d", 
				msg->bdaddr.lap, msg->bdaddr.uap, msg->bdaddr.nap, msg->role);
	CONVERT_BDADDRSRC2ARRAY(bdaddr.addr, &msg->bdaddr);
	ret = hdp_echo(msg->role, &bdaddr);
	if (BT_STATUS_PENDING != ret)
	{
		cnf = (bt_hdp_echo_cnf_struct *) construct_local_para(sizeof(bt_hdp_echo_cnf_struct), TD_UL | TD_RESET);
		cnf->result = hdp_util_convert_status(ret);
		cnf->role = msg->role;
		OS_MemCopy((kal_uint8 *)&cnf->bdaddr, (kal_uint8 *)&msg->bdaddr, sizeof(bt_addr_struct));
		hdp_send_msg(MSG_ID_BT_HDP_ECHO_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);
	}

}
void hdp_adp_deregister_cnf(HdpCallbackParm *parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_hdp_deregister_instance_cnf_struct *cnf = 
        (bt_hdp_deregister_instance_cnf_struct *) construct_local_para(sizeof(bt_hdp_deregister_instance_cnf_struct), TD_UL | TD_RESET);
	hdp_adp_instance_struct *instance = NULL;

	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	instance = hdp_util_find_instance(parms->content.instance.mdepId);
	if (instance == NULL)
	{
		return;
	}
	cnf->result = hdp_util_convert_status(parms->result);
	cnf->mdepId = parms->content.instance.mdepId;
	instance->single = FALSE;
	hdp_send_msg(MSG_ID_BT_HDP_DEREGISTER_INSTANCE_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);	
}


void hdp_adp_connect_cnf(HdpCallbackParm *parms)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	BtStatus ret = parms->result;
	hdp_adp_channel_struct *channel = NULL;	
	bt_addr_struct btAddr;
	bt_hdp_connect_cnf_struct *cnf = NULL;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP]hdp_adp_connect_cnf: device addr = %x:%x:%x:%x:%x:%x, mdepId is %d, mdlId is %d",
				parms->content.connection.addr.addr[0], parms->content.connection.addr.addr[1],parms->content.channel.addr.addr[2],
				parms->content.connection.addr.addr[3], parms->content.connection.addr.addr[4],parms->content.channel.addr.addr[5],
				parms->content.connection.mdepId, parms->content.connection.mdlId);	
	OS_MemSet((kal_uint8 *)&btAddr, 0x0, sizeof(bt_addr_struct));
	CONVERT_ARRAY2BDADDR(&btAddr, parms->content.connection.addr.addr);

	//debug
	hdp_print_channel();
	OS_Report("[HDP][ADP] remode device: %x %x %x", btAddr.lap, btAddr.uap, btAddr.nap);
	
	channel = hdp_util_find_channel_by_state(&btAddr, HDP_ADP_CHANNEL_CONNECTING);
	if (NULL == channel)
	{
		OS_Report("[HDP][ADP]no connecting channel is found");
		return;
	}
	
	cnf = (bt_hdp_connect_cnf_struct *) construct_local_para(sizeof(bt_hdp_connect_cnf_struct), TD_UL | TD_RESET);
	if (BT_STATUS_SUCCESS == ret)
	{
		channel->state = HDP_ADP_CHANNEL_CONNECTED;
		channel->mdlId = parms->content.connection.mdlId;
//		channel->l2capChnId = parms->content.connection.l2capId;

		if (NULL == hdp_util_find_main_channel(&btAddr))
		{
			channel->mainChn = TRUE;		
		}
	}

	cnf->result = hdp_util_convert_status(ret);
	cnf->index = channel->index;
	cnf->mdepId = channel->mdepId;
	cnf->mainChannel = FALSE;
	cnf->mdlId = channel->mdlId;
	OS_MemCopy((kal_uint8 *)&cnf->bdaddr, (kal_uint8 *)&channel->bdaddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_CONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);	
	
	if (BT_STATUS_SUCCESS != ret)
	{
		hdp_util_remove_channel(channel);
		OS_MemSet((kal_uint8 *)channel, 0x0, sizeof(hdp_adp_channel_struct));
	}
}


void hdp_adp_disconnect_cnf(HdpCallbackParm *parms)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	BtStatus ret = parms->result;
	hdp_adp_channel_struct *channel = NULL;	
	bt_addr_struct btAddr;
	bt_hdp_disconnect_cnf_struct *cnf = NULL;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP]hdp_adp_disconnect_cnf: device addr = %x:%x:%x:%x:%x:%x, mdepId is %d, mdlId is %d",
				parms->content.connection.addr.addr[0], parms->content.connection.addr.addr[1],parms->content.channel.addr.addr[2],
				parms->content.connection.addr.addr[3], parms->content.connection.addr.addr[4],parms->content.channel.addr.addr[5],
				parms->content.connection.mdepId, parms->content.connection.mdlId);	
	
	OS_MemSet((kal_uint8 *)&btAddr, 0x0, sizeof(bt_addr_struct));
	CONVERT_ARRAY2BDADDR(&btAddr, parms->content.connection.addr.addr);
	channel = hdp_util_find_channel_by_state(&btAddr, HDP_ADP_CHANNEL_DISCONNECTING);
	if (NULL == channel)
	{
		OS_Report("no connecting channel is found");
		return;
	}
	
	cnf = (bt_hdp_disconnect_cnf_struct *) construct_local_para(sizeof(bt_hdp_disconnect_cnf_struct), TD_UL | TD_RESET);

	cnf->result = hdp_util_convert_status(ret);
	cnf->index = channel->index;
	cnf->mdlId = channel->mdlId;
	OS_MemCopy((kal_uint8 *)&cnf->bdaddr, (kal_uint8 *)&channel->bdaddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_DISCONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);

	
	if (BT_STATUS_SUCCESS == ret)
	{
		hdp_util_remove_channel(channel);
		OS_MemSet((kal_uint8 *)channel, 0x0, sizeof(hdp_adp_channel_struct));
	}
	else
	{
		OS_Report("[HDP] fail to disconnect");
		channel->state = HDP_ADP_CHANNEL_CONNECTED;
	}
}

void hdp_adp_channel_opened_ind(HdpCallbackParm *parms)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	BtStatus ret = parms->result;
	hdp_adp_channel_struct *channel = NULL;	
	bt_addr_struct btAddr;
	bt_hdp_channel_opened_ind_struct *ind = NULL;
	hdp_adp_instance_struct *instance = NULL; 
	BOOL 					mainChannel = FALSE;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP]hdp_adp_channel_opened_ind: device addr = %x%x%x%x%x%x, mdepId is %d, mdlId is %d, l2cap id is %d",
				parms->content.channel.addr.addr[0], parms->content.channel.addr.addr[1],parms->content.channel.addr.addr[2],
				parms->content.channel.addr.addr[3], parms->content.channel.addr.addr[4],parms->content.channel.addr.addr[5],
				parms->content.channel.mdepId, parms->content.channel.mdlId, parms->content.channel.l2capId);

	//Notes: incorrect to find instance based on mdepID
	instance = hdp_util_find_instance(parms->content.channel.mdepId);
	if (NULL == instance)
	{
		OS_Report("no instance is found");
		return;
	}
	
	OS_MemSet((kal_uint8 *)&btAddr, 0x0, sizeof(bt_addr_struct));
	CONVERT_ARRAY2BDADDR(&btAddr, parms->content.channel.addr.addr);

	channel = hdp_util_find_channel_by_mdl_id(&btAddr, parms->content.channel.mdlId);

	if (BT_STATUS_SUCCESS != ret)
	{
		if (NULL != channel)
		{
			channel->state = HDP_ADP_CHANNEL_IDLE;
			hdp_util_remove_channel(channel);
			OS_MemSet((kal_uint8 *)channel, 0x0, sizeof(hdp_adp_channel_struct));
		}
		OS_Report("[HDP]Err to open hdp channel");
		return;
	}
	
	if (NULL == channel)
	{
		OS_Report("[HDP][ADP] No related channel is found, so add a new channel to list");
		channel = hdp_util_find_available_channel();
		if (NULL == channel)
		{
			OS_Report("no channel resource is found");
			return;
		}
		hdp_channel_init(channel);
		OS_MemCopy((kal_uint8 *)&channel->bdaddr, (kal_uint8 *)&btAddr, sizeof(bt_addr_struct));
		channel->mdlId = parms->content.channel.mdlId;
		channel->mdepId = parms->content.channel.mdepId;
			if (NULL == hdp_util_find_main_channel(&btAddr))
		{
			channel->mainChn = TRUE;
			OS_Report("[HDP][ADP]the mdl channel is the first connection for the device");
		}
		
		hdp_util_add_channel(channel);
	}	
	else if (instance->mdepId != channel->mdepId)
	{
		if (HDP_ADP_CHANNEL_IDLE != channel->state)
		{
		    //It is forbiden to access two instance for one hdp channel identified by MDL ID and BD_ADDR
		    OS_Report("[HDP] Err: the channel is being mapped to two intances.");
		    return;
	        }
		else
		{
		    //Notes: replace the mdep id with the new value if the orignal channel is in ilde state
		    channel->mdepId = parms->content.channel.mdepId;			
		}
	}

	//Notes: channel is connected every time, water mark has to be updated
	channel->waterMark = hdp_util_get_channel_water_mark();
	channel->state = HDP_ADP_CHANNEL_CONNECTED;
	channel->l2capChnId = parms->content.channel.l2capId;
	channel->mtu = parms->content.channel.mtu;
	hdp_print_channel();
	
	ind = (bt_hdp_channel_opened_ind_struct *) construct_local_para(sizeof(bt_hdp_channel_opened_ind_struct), TD_UL | TD_RESET);
	ind->mdepId = channel->mdepId;
	ind->mainChannel = channel->mainChn;
	ind->mdlId = channel->mdlId;
	ind->l2capId = channel->waterMark;
	OS_MemCopy((kal_uint8 *)&ind->bdaddr, (kal_uint8 *)&channel->bdaddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_CHANNEL_OPENED_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);
}

void hdp_adp_channel_closed_ind(HdpCallbackParm *parms)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	BtStatus ret = parms->result;
	hdp_adp_channel_struct *channel = NULL;	
	hdp_adp_channel_struct *idle_channel = NULL;	
	bt_addr_struct btAddr;
	bt_hdp_channel_closed_ind_struct *ind = NULL;
	hdp_adp_instance_struct *instance = NULL; 
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	OS_Report("[HDP][ADP]hdp_adp_channel_closed_ind: device addr = %x%x%x%x%x%x, mdepId is %d, mdlId is %d, l2cap id is %d",
				parms->content.channel.addr.addr[0], parms->content.channel.addr.addr[1],parms->content.channel.addr.addr[2],
				parms->content.channel.addr.addr[3], parms->content.channel.addr.addr[4],parms->content.channel.addr.addr[5],
				parms->content.channel.mdepId, parms->content.channel.mdlId, parms->content.channel.l2capId);
	
	OS_MemSet((kal_uint8 *)&btAddr, 0x0, sizeof(bt_addr_struct));
	CONVERT_ARRAY2BDADDR(&btAddr, parms->content.channel.addr.addr);
	channel = hdp_util_find_channel_by_mdl_id(&btAddr, parms->content.channel.mdlId);
	if (NULL == channel)
	{
		OS_Report("no channel is found");
		return;
	}
	hdp_channel_detach(channel);
	if (parms->content.channel.delFlag)
	{
		hdp_util_remove_channel(channel);
		OS_MemSet((kal_uint8 *)channel, 0x0, sizeof(hdp_adp_channel_struct));
		OS_Report("[HDP] clear channel cache");
	}
	else
	{
		//1. remove original remain channel with the same mdl id in idle state
		idle_channel = hdp_util_find_idle_channel_by_mdl_id(&btAddr, parms->content.channel.mdlId);
		if (idle_channel)
		{
			hdp_util_remove_channel(idle_channel);
		}

		//2. set current channel state as idle state
		channel->state = HDP_ADP_CHANNEL_IDLE;
	}
	
	//NOTES: Android framework requires channel closed indication to remove channel.
	/*		So even if instance can not be found, the message should be sent*/
	//If no instance is found, it is not neccessary to notify upper layer 
/*	instance = hdp_util_find_instance(parms->content.channel.mdepId);
	if (NULL == instance)
	{
		OS_Report("[HDP][ADP]no instance is found");
		return;
	}
	*/
	
	ind = (bt_hdp_channel_closed_ind_struct *) construct_local_para(sizeof(bt_hdp_channel_closed_ind_struct), TD_UL | TD_RESET);
	ind->mdlId = parms->content.channel.mdlId;
	OS_MemCopy((kal_uint8 *)&ind->bdaddr, (kal_uint8 *)&btAddr, sizeof(bt_addr_struct));
	hdp_send_msg(MSG_ID_BT_HDP_CHANNEL_CLOSED_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);
}
void hdp_adp_data_ind(HdpCallbackParm *parms)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	hdp_adp_channel_struct *channel = NULL;	
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/

	OS_Report("hdp_adp_data_ind: l2cap id->%d", parms->content.data.l2capChnId);
	channel = hdp_util_find_channel_by_l2cap_Id(parms->content.data.l2capChnId) ;
	if (NULL != channel &&
		NULL != parms->content.data.value &&
		0 < parms->content.data.size)
	{	
		//todo: if only partial data is sent, queue remained data
		hdp_channel_lower_layer_incoming(channel,  parms->content.data.value, parms->content.data.size);
	}
	else
	{
		OS_Report("invalid data is received");
	}
}


void hdp_adp_connection_removed(HdpCallbackParm *parms)
{
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_hdp_remove_connection_cnf_struct *cnf = 
    			(bt_hdp_remove_connection_cnf_struct *) construct_local_para(sizeof(bt_hdp_remove_connection_cnf_struct), TD_UL | TD_RESET);;     
	bt_addr_struct bdaddr;
	BtStatus ret;
	/*----------------------------------------------------------------*/
    /* Code body                                                */
    /*----------------------------------------------------------------*/
	CONVERT_ARRAY2BDADDR(&bdaddr, parms->content.connection.addr.addr);

	cnf->result = hdp_util_convert_status(parms->result);	
	OS_MemCopy((kal_uint8 *)&cnf->bdaddr, (kal_uint8 *)&bdaddr, sizeof(bt_addr_struct));		
	hdp_send_msg(MSG_ID_BT_HDP_REMOVE_CONNECTION_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);		 	
}

void hdp_adp_Callback(HdpCallbackParm *parms)
{	
	if (NULL == parms)
	{
		return;
	}
	OS_Report("[HDP][ADP]hdp_adp_Callback: event-> %d, status-> %d", parms->event, parms->result);
	switch(parms->event)
	{
		case HDP_EVENT_INSTANCE_DEREGISTERED:
			hdp_adp_deregister_cnf(parms);
			break;
			
		case HDP_EVENT_CONNECT_CNF:
			hdp_adp_connect_cnf(parms);
			break;
						
		case HDP_EVENT_DISCONNECT_CNF:
			hdp_adp_disconnect_cnf(parms);
			break;
						
		case HDP_EVENT_CHANNEL_OPENED:
			hdp_adp_channel_opened_ind(parms);
			break;
			
		case HDP_EVENT_CHANNEL_CLOSED:
			hdp_adp_channel_closed_ind(parms);
			break;
			
		case HDP_EVENT_DATA_RCV_IND:	
			hdp_adp_data_ind(parms);
			break;	

		case HDP_EVENT_CONNECTION_REMOVED:
			hdp_adp_connection_removed(parms);
			break;

		default:
			OS_Report("[HDP][ADP]invalid event: %d", parms->event);
	}
}



/********************************************************************************************
 * Entry function for message handlers
 ********************************************************************************************/
void hdp_adp_init()
{
	BtStatus ret;
	if (TRUE == hdp_adp_context.initialized)
	{
		return;
	}

	OS_MemSet((kal_uint8 *)&hdp_adp_context, 0x0, sizeof(hdp_adp_context_struct));
	ret = hdp_init(hdp_adp_Callback);
	if (BT_STATUS_SUCCESS == ret)
	{
		hdp_adp_context.initialized = TRUE;
		InitializeListHead(&hdp_adp_context.active_channel);
	}
	bt_session_register_handler(PROFILE_HDP_DATA,
								PROFILE_HDP_DATA_RANGE,
								hdp_channel_monitor,
								&hdp_adp_context.monitor_id);
}
void hdp_adp_deinit()
{
	hdp_adp_channel_struct *channel = NULL;	
	hdp_deinit();
	hdp_adp_context.initialized = FALSE;

	while(!IsListEmpty(&hdp_adp_context.active_channel))
	{
		channel = (hdp_adp_channel_struct *)RemoveHeadList(&hdp_adp_context.active_channel);
		hdp_channel_deinit(channel);
	}
	bt_session_unregister_handler(hdp_adp_context.monitor_id);
}



void btmtk_adp_hdp_handle_message(ilm_struct *ilm_ptr) {
	OS_Report("[Hdp] %s, msg_id: %d", __FUNCTION__, ilm_ptr->msg_id);

	switch (ilm_ptr->msg_id) {	
		case MSG_ID_BT_HDP_ACTIVATE_REQ :
			break;
			
		case MSG_ID_BT_HDP_DEACTIVATE_REQ:   
			break;
			     			 
		case MSG_ID_BT_HDP_REGISTER_INSTANCE_REQ: 
			hdp_adp_register_instance(ilm_ptr);
			break;
						
		case MSG_ID_BT_HDP_DEREGISTER_INSTANCE_REQ:
			hdp_adp_deregister_instance(ilm_ptr);
			break;
		
		case MSG_ID_BT_HDP_CONNECT_REQ:    
			hdp_adp_connect(ilm_ptr);
			break;

		case MSG_ID_BT_HDP_CHANNEL_OPENED_RSP: 
			hdp_adp_connect_rsp(ilm_ptr);
			break;
			        
		case MSG_ID_BT_HDP_DISCONNECT_REQ:    
			hdp_adp_disconnect(ilm_ptr); 
			break;

		case MSG_ID_BT_HDP_REMOVE_CONNECTION_REQ:
			hdp_adp_remove_connection(ilm_ptr);
			break;
			      			                                      
		case MSG_ID_BT_HDP_GET_MAIN_CHANNEL_REQ:     
			hdp_adp_get_main_channel(ilm_ptr);
			break;
			
		case MSG_ID_BT_HDP_GET_INSTANCE_REQ:       
			hdp_adp_get_instance(ilm_ptr);  
			break;
			 			  
		case MSG_ID_BT_HDP_GET_L2CAP_CHANNEL_REQ:     
			hdp_adp_get_channel_id(ilm_ptr);
			break;
			
		case MSG_ID_BT_HDP_ECHO_REQ:
			hdp_adp_echo(ilm_ptr);
			break;

		case MSG_ID_BT_HDP_RECONNECT_REQ:
			hdp_adp_reopen_connection(ilm_ptr);
			break;

		case MSG_ID_BT_HDP_SEND_DATA_REQ:
			hdp_adp_send_data_req(ilm_ptr);
			break;
			
		default:
			break;
	}
}

#endif /* __BT_HDP_PROFILE__ */
