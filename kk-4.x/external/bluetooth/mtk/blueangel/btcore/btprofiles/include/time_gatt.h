/*****************************************************************************
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of MediaTek Inc. (C) 2005
 *
 *  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 *  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
 *  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 *  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 *  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 *  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
 *  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
 *  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
 *  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 *  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 *  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
 *  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
 *  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
 *  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
 *  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
 *  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
 *
 *****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 * time_gatt.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Time profile
 *
 * Author:
 * -------
 * Jacob Lee
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 * 
 * 10 16 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 * 
 * 10 15 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef __TIME_GATT_H
#define __TIME_GATT_H

#include "conmgr.h"
#include "gatt.h"

// Time packet type definitions
#define TIME_PKT_READ			0x00
#define TIME_PKT_WRITE			0x01
#define TIME_PKT_NOTIFY			0x02

// Valid values for Time Update Control Point
#define TIME_UPDATE_CTRL_REQUEST				0x01
#define TIME_UPDATE_CTRL_CANCEL					0x02

// Valid values for Time Update Status, Current State
#define TIME_UPDATE_STATE_IDLE				0
#define TIME_UPDATE_STATE_UPDATE_PENDING	1

// Valid values for Time Update Status, Result
#define TIME_UPDATE_RESULT_SUCCESSFUL		0
#define TIME_UPDATE_RESULT_CANCELED			1
#define TIME_UPDATE_RESULT_NO_CONNECTION	2
#define TIME_UPDATE_RESULT_ERROR			3
#define TIME_UPDATE_RESULT_TIMEOUT			4
#define TIME_UPDATE_RESULT_NOT_ATTEMPETED	5

typedef struct _TimePacket {
	ListEntry node;
	BtPacket packet;
	U8 check_byte;
	U8 seq_num;
	U32	value_id;
	U8 type;
	U16 value;
	void *chnl;
	
	BtGattOp gatt_op;
	U16 start_handle;
	U16 end_handle;
	U8 success_flag;
	union {
		BtGattDiscoveryPrimaryServiceByUUID dps_uuid;
		BtGattDiscoveryCharacteristicByUUID dch_uuid;
		BtGattDiscoveryAllCharacteristicDescriptors dds;
		BtGattReadCharacteristicValue rch;
		BtGattWriteWithoutResponse wch_wo_rsp;
		BtGattValueNotifications vno;
		BtGattReadCharacteristicDescriptors rds;
		BtGattWriteDescriptors wds;
	} gatt_params;
} TimePacket;

// Shared information, will be prepared when enabling server
typedef struct {
	// Current Time :: CT Time
	struct {
		U16 year;
		U8 month;
		U8 day;
		U8 hours;
		U8 minutes;
		U8 seconds;
		U8 wday;
		U8 frac;
		U8 adj_reason;
	} ct_time;

	// Current Time :: Local Time Information
	struct {
		S8 time_zone;
		U8 curr_dst_offset;
	} local_time_info;

	// Current Time :: Reference Time Information
	struct {
		U8 time_source;
		U8 time_accuracy;
		U8 days_since_update;
		U8 hours_since_update;
	} ref_time_info;

	// Next DST :: Time with DST
	struct {
		U16 year;
		U8 month;
		U8 day;
		U8 hours;
		U8 minutes;
		U8 seconds;
		U8 next_dst_offset;
	} time_with_dst;
} TimeServerSharedInfo;

// For every connection
typedef struct {
	BtRemoteDevice *link;
	U8 in_use;
	U16 cttime_notify_config;
	U8 update_ctrl_point;
	struct {
		U8 cur_state;
		U8 result;
	} update_status;
} TimeServerConfigInfo;

#endif
