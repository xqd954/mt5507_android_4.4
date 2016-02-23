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

#ifndef __BT_HDP_ADP_H__
#define __BT_HDP_ADP_H__

#include "hdp.h"
#include "bluetooth_struct.h"
#include "session_os.h"
#include "session_api.h"

#define HDP_ADP_MAX_INSTANCE_NUM 	10
#define HDP_ADP_MAX_CHANNEL_NUM 	20
#define HDP_ADP_INSTANCE_SOCKET_ADDR_LEN 255
#define HDP_ADP_MAX_DATA_LEN		1024

#define HDP_ADP_DATA_BUFFER_SIZE	5*1024

typedef enum
{
	HDP_ADP_CHANNEL_IDLE,			//the channel is idle
	HDP_ADP_CHANNEL_CONNECTING,			//the channel is in connecting or disconnecting
	HDP_ADP_CHANNEL_DISCONNECTING,			//the channel is in connecting or disconnecting
	HDP_ADP_CHANNEL_CONNECTED,			//the channel is connected
} hdp_adp_channel_state;

typedef struct
{	
	ListEntry  				node;
	BOOL 					inUse;
	hdp_adp_channel_state 	state;
	bt_addr_struct			bdaddr;
	U8						mdepId; // the instance ID
	U16 					mdlId;
	L2capChannelId 			l2capChnId;
	BOOL 					mainChn;
	U32						index;

	/*Notes: the non-conflict water mark is very important to identify l2cap channel instead of real L2CAP channel ID,*/
	/* 		due to the fact that the L2CAP channel ID may be reused.*/
	/*		If upper layer transit data using socket based on L2CAP ID, it will fail to create socket with same socket address again */
	/*		when L2CAP channel is reused in short duration */
	U16			        waterMark;  

	Session_Mutex			*mutex;
	U32						sessionId;
	U16 					mtu;
	U8						rxbuff[HDP_ADP_DATA_BUFFER_SIZE];
	U16						rxlen;
} hdp_adp_channel_struct;

typedef struct
{
	BOOL 	single;  //TRUE: some instance has registered in the record
	U8 		mdepId;
	
} hdp_adp_instance_struct;

typedef struct
{
	BOOL initialized;
	hdp_adp_instance_struct instance[HDP_ADP_MAX_INSTANCE_NUM];
	ListEntry  				active_channel;
	
	hdp_adp_channel_struct 	channels[HDP_ADP_MAX_CHANNEL_NUM];

	U16						monitor_id;
} hdp_adp_context_struct;

SESSION_RESULT hdp_channel_monitor(Session_Event_PARM *parm);
BOOL hdp_channel_init(hdp_adp_channel_struct *channel);
BOOL hdp_channel_deinit(hdp_adp_channel_struct *channel);
int hdp_channel_lower_layer_incoming(hdp_adp_channel_struct *channel, void *data, U32 size);
void hdp_channel_upper_data_incoming(U32 id);
void hdp_channel_detach(hdp_adp_channel_struct *channel);

#endif

