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
 *   times.h
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file contains public API functions for the Time Profile (Server).
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
#ifndef __TIMES_H
#define __TIMES_H

#include "l2cap.h"	// for transport
#include "conmgr.h"	// for transport
#include "rfcomm.h"	// for transport
#include "gatt.h"	// for transport
#include "utils.h"

typedef U8 TimeServerState;

#define TIME_SERVER_IDLE				0
#define TIME_SERVER_READY				1
#define TIME_SERVER_CONNECTING			2
#define TIME_SERVER_CONNECTED			3
#define TIME_SERVER_DISCONNECTING		4
#define TIME_SERVER_DEREGISTERING		5

typedef struct {
	void *timesContext;	//Time server context
	U8 id;
	U8 isReg;
} TimeSApp;

typedef struct {
	U8 event_id;
	void *data;
} TimeSEvent;

/* 
 * @brief callback prototype 
 */
typedef void (*BTMTK_TIMES_CALLBACK)(TimeSEvent *event, void *apdata);

typedef struct _TimeSTransport {
	ListEntry node;
	BD_ADDR bdaddr;
	CmgrHandler handler;
	BtGattHandler gatt_handler;
	TimePacket *pkt;
	U8 in_use;
	U8 check_byte;

	U16 cttime_notify_config;
	struct {
		U8 cur_state;
		U8 result;
	} update_status;
} TimeSTransport;

typedef struct _TimeSContext {
	BTMTK_TIMES_CALLBACK callback;
	TimeSTransport chnl;
	U8 state;				// Time server state
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
	U8 update_state;			// Status of server update
	U8 update_result;		// Result of server update
	void *app;				// Application data
	U8 gatt_cb_registered;
	EvmTimer times_gatt_connect_timer;
} TimeSContext;

BT_BOOL TIMES_Init();

BtStatus TIMES_Register(TimeSApp *app, TimeSTransport *chnl, BTMTK_TIMES_CALLBACK callback);
BtStatus TIMES_Deregister(TimeSApp *app);
BtStatus TIMES_AcceptIncoming(TimeSApp *app, BtStatus rspcode);
BtStatus TIMES_Connect(TimeSApp *app, U8 addr[6]);
BtStatus TIMES_Disconnect(TimeSApp *app);
BtStatus TIMES_Set_Local_Info(TimeSApp *app, U8 value_id);
BtStatus TIMES_Notify_Value(TimeSApp *app, U8 value_id);

void times_notify_incoming(BtRemoteDevice *link);
void times_notify_gatt_connected(BtRemoteDevice *link);
void times_notify_gatt_disconnected(BtRemoteDevice *link);
void times_notify_connected(TimeSContext *pCntx, U8 *addr);
void times_notify_disconnected(TimeSContext *pCntx);
void times_notify_request_server_update(TimeSContext *pCntx);
void times_notify_cancel_server_update(TimeSContext *pCntx);
void times_notify_notify_value_result(TimeSContext *pCntx);

#endif
