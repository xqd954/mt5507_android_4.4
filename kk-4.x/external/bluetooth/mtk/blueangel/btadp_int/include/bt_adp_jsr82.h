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

#ifndef __BT_ADP_JSR82_H
#define __BT_ADP_JSR82_H


#include "bluetooth_jsr82_struct.h"
#include "eventmgr.h"
#include "session_os.h"
#include "jsr82_session.h"

#define BT_ADP_JSR82_MAX_SESSION_COUNT 20

#define BT_ADP_JSR82_MAX_BUFFER_SIZE	1024

typedef enum _BT_ADP_JSR82_SESSION_ROLE
{
	JSR82_SESSION_ROLE_CLIENT,
	JSR82_SESSION_ROLE_SERVER,		
} BT_ADP_JSR82_SESSION_ROLE;

//state for lower channel 
typedef enum _BT_ADP_JSR82_STATE
{
	BT_ADP_JSR82_STATE_IDLE,
	BT_ADP_JSR82_STATE_ACTIVE,	
	BT_ADP_JSR82_STATE_CONNECTING,
	BT_ADP_JSR82_STATE_CONNECTED,
	BT_ADP_JSR82_STATE_DISCONNECTING,

	BT_ADP_JSR82_STATE_ZOMBIA //for server, when service is deregistered, but active connetion exist, enter this state
							 //when all connections exist, deregister service 
} BT_ADP_JSR82_STATE;

//state for session manager with external layer
typedef enum _BT_JSR82_SESSION_STATE
{
	BT_JSR82_SESSION_STATE_DETACHED,
	BT_JSR82_SESSION_STATE_ATTACHING,
	BT_JSR82_SESSION_STATE_ATTACHED,
	BT_JSR82_SESSION_STATE_DETACHING		
} BT_JSR82_SESSION_STATE;

typedef enum _BT_JSR82_TASK_CODE
{
	JSR82_TASK_CHANNEL_CREATED = 0x01,		
	JSR82_TASK_SEND,	
	JSR82_TASK_RECEIVE,
}BT_JSR82_TASK_CODE;

typedef struct _BT_Jsr82_Task
{
	ListEntry 	node;
	U8			code;
	U8 			*context;
} BT_Jsr82_Task;


typedef struct _BT_ADP_JSR82_SESSION
{
	BT_ADP_JSR82_STATE					conn_state;

	//valid for IPC session
	BT_JSR82_SESSION_STATE				attach_state;
	U32 								sessionid;

	U8		 							bd_addr[6];
	
	ListEntry							pending_task;
	U8									pending_task_num;
	
	//valid for lower layer
	U8									index;
	U32									channel;
	U16									l2cap_id;
	U16									mtu;
	U16									in_mtu; 
	U16 								existing_psm_chnl_num;

	Session_Mutex						*mutex;

	//valid for upper layer
	U32									identify;	
	S32									fd;

	BT_ADP_JSR82_SESSION_ROLE 			role;
	U8									active_conn; //valid for main server serssion context
	U8									authorize;
	
    U8 									ps_type;
	U8									security_value;
	U8									service[16];  // uuid values

	//buffer to write/read data to lower layer
	U8									txbuffer[JSR82_SESSION_SPP_TX_BUFSIZE];
	U16									txlength;
	U8									rxbuffer[JSR82_SESSION_SPP_RX_BUFSIZE];
	U16									rxlength;

	EvmTimer 							timer;
}BT_ADP_JSR82_SESSION;

typedef struct _BT_ADP_JSR82_CONTEXT
{
	BOOL					init;
	BT_ADP_JSR82_SESSION 	session[BT_ADP_JSR82_MAX_SESSION_COUNT];
	U16						handler_id;
	Session_Mutex			*mutex;

}BT_ADP_JSR82_CONTEXT;




void jsr82_adp_init(void);
void btmtk_adp_jsr82_handle_message(ilm_struct *ilm_ptr);


#endif /* __BT_ADP_JSR82_H */ 

