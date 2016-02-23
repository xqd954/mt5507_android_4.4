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
/*******************************************************************************
 *
 * Filename:
 * ---------
 *   timec.h
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file contains public API functions for the Time Profile (Client).
 *
 * Author:
 * -------
 *   Jacob Lee
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
#ifndef __TIMEC_H
#define __TIMEC_H

#include "l2cap.h"	// for transport
#include "conmgr.h"	// for transport
#include "rfcomm.h"	// for transport
#include "sdap.h"

typedef struct {
	void *timecContext;	// Time client context
	U8 id;
} TimeCApp;

typedef struct {
	U8 event_id;
	void *data;
} TimeCEvent;

/* 
 * @brief callback prototype 
 */
typedef void (*BTMTK_TIMEC_CALLBACK)(TimeCEvent *event, void *apdata);

typedef enum {
	// Current Time service handles
	CURRENT_TIME_SERVICE_START = 0,
	CT_TIME,
	CT_TIME_CHAR_CONFIG,
	LOCAL_TIME_INFO,
	REF_TIME_INFO,
	CURRENT_TIME_SERVICE_END,
	// Next DST service handles
	NEXT_DST_SERVICE_START,
	TIME_WITH_DST,
	NEXT_DST_SERVICE_END,
	// Reference Time Update service handles
	REF_TIME_UPDATE_SERVICE_START,
	TIME_UPDATE_CTRL_POINT,
	TIME_UPDATE_STATE,
	REF_TIME_UPDATE_SERVICE_END,
	CACHED_HANDLE_COUNT,
} TimeCHandles;

typedef struct _TimeCTransport {
	BD_ADDR bdaddr;
	CmgrHandler handler;
	U8 check_byte;

	// Cached ATT DB handle
	U16 cached_handles[CACHED_HANDLE_COUNT];
} TimeCTransport;

typedef struct _TimeCContext {
	BTMTK_TIMEC_CALLBACK callback;
	TimeCTransport chnl;
	U8 state;				// Time client state
	U8 cancel_update;		// For keeping the cancel server update.
	U8 addr[6];				// Remote address
	U8 in_use;				// In use check
	U16 date_time[6];		// Contains: year, month, day of month, hours, minutes, seconds
	U8 day_of_week;
	U8 frac;					// Fraction of a second
	U8 frac_type;			// Fraction type: 1/100 or 1/256
	U8 adj_reason;			// The reason of the adjustment
	U8 dst;					// DST type
	S8 time_zone;			// Time zone
	U8 accuracy;				// Accuracy information of reference time
	U8 time_source;			// Type of time source
	U8 days_since_update;
	U8 hours_since_update;
	U8 update_state;			// Status of server updating
	U8 update_result;
	U16 cttime_notify_config;
	void *app;				// Application data
} TimeCContext;

typedef U8 TimeClientState;

#define TIME_CLIENT_IDLE				0
#define TIME_CLIENT_ENABLED				1
#define TIME_CLIENT_LINK_CONNECT		2
#define TIME_CLIENT_QUERY				3
#define TIME_CLIENT_CONNECTING			4
#define TIME_CLIENT_CONNECTED			5
#define TIME_CLIENT_GETCAP				6
#define TIME_CLIENT_DISCONNECTING		7

BT_BOOL TIMEC_ResetContext();

BtStatus TIMEC_Init(TimeCApp *app, BTMTK_TIMEC_CALLBACK callback);
BtStatus TIMEC_Deinit(TimeCApp *app);
BtStatus TIMEC_Connect(TimeCApp *app, U8 addr[6], BTMTK_TIMEC_CALLBACK callback);
BtStatus TIMEC_Disconnect(TimeCApp *app);
BtStatus TIMEC_Read_Remote_Value(TimeCApp *app, U8 value_id);
BtStatus TIMEC_Write_Remote_Value(TimeCApp *app, U8 value_id, U16 value);

void timec_notify_link_result(TimeCContext *cntx, U32 rspcode);
void timec_notify_incoming_link(TimeCContext *cntx);
void timec_notify_query_result(TimeCContext *cntx, U32 rspcode);
void timec_notify_connect_result(TimeCContext *cntx, U32 rspcode);
void timec_notify_disconnect_result(TimeCContext *cntx, U32 rspcode);
void timec_notify_read_remote_value_result(TimeCContext *cntx, U32 rspcode, TimePacket *pkt);
void timec_notify_write_remote_value_result(TimeCContext *cntx, U32 rspcode, TimePacket *pkt);

void timec_notify_server_notification(TimeCContext *cntx, U8 value_id);
#endif
