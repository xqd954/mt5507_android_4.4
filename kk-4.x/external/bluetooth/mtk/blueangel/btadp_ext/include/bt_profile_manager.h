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
 * bt_profile_manager.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to expose common api use to manager profile connection
 *
 * Author:
 * -------
 * Liang Cheng
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
#ifndef _BT_PROFILE_MANAGER_H_
#define _BT_PROFILE_MANAGER_H_

#include <hardware/bluetooth.h>
#include <hardware/bt_hf.h>
#include <hardware/bt_av.h>
#include "bt_hfg_struct.h"
#include "utils.h"
#include "bt_mmi.h"
#include "bt_message.h"

#include "utils.h"
#include "bt_mmi.h"
#include "bluetooth_struct.h"

#define MAX_DEV_NUM        4

/*******************************************************************************
*   Due to current there is no BT common headser files, I define some common BT
*   used types here
********************************************************************************/
/*---------------------------------------------------------------------------
 * ProfileStatus type
 *
 *     This type is profile common status
 */

typedef int  ProfileStatus;

#define PROFILE_STATE_DISCONNECTED   0
#define PROFILE_STATE_CONNECTING     1
#define PROFILE_STATE_CONNECTED      2
#define PROFILE_STATE_DISCONNECTING  3

/*---------------------------------------------------------------------------
 * ProfileOpType type
 *
 *     This type is profile operate type
 */
typedef int ProfileOpType;

#define PROFILE_OP_CONNECT           0
#define PROFILE_OP_DISCONNECT        1

/*---------------------------------------------------------------------------
 * ProfileConnType
 *
 *     This type is profile Connecttion type
 */
typedef int ProfileConnType;

#define SUPPORT_SINGLE_CONNECTION           0
#define SUPPORT_MULTI_CONNECT               1

typedef struct {
	PROFILE_enum     profile_id;
	ProfileOpType    op;
}profile_op_node_t;

typedef struct profile_op_queue{
	struct profile_op_queue *next;
	profile_op_node_t        node;
}profile_op_queue_t;

typedef struct {
	LOCAL_PARA_HDR
	bt_addr_struct   bd_addr;
	PROFILE_enum     profile_id;
} btmtk_profile_connect_request_struct;

typedef btmtk_profile_connect_request_struct btmtk_profile_disconnect_request_struct;

void notifyProfileStateChanged(bt_addr_struct *bd_addr, PROFILE_enum id, ProfileStatus state);

int isConnected(bt_addr_struct *bd_addr);

int isDisconnected(bt_addr_struct *bd_addr);

ProfileStatus getProfileState(bt_addr_struct *bd_addr, PROFILE_enum id);

void init_profile_manager();

void deinit_profile_manager();

BtStatus connect_profile(bt_addr_struct *bd_addr, PROFILE_enum id);

BtStatus disconnect_profile(bt_addr_struct *bd_addr, PROFILE_enum id);

BtStatus handle_profile_connect(bt_addr_struct *bd_addr, PROFILE_enum id);

BtStatus handle_profile_disconnect(bt_addr_struct *bd_addr, PROFILE_enum id);

BtStatus connect_profile_int(bt_addr_struct *bd_addr, PROFILE_enum id);

BtStatus disconnect_profile_int(bt_addr_struct *bd_addr, PROFILE_enum id);

BtStatus disconnect_all();

typedef void (*ProfileStateChanged)(bt_addr_struct *bd_addr, PROFILE_enum id, ProfileStatus state);

typedef struct profile_state_changed_callback{
	struct profile_state_changed_callback *next;
	ProfileStateChanged func;
}profile_state_changed_callback_t;

void registerProfileStateChangedCB(ProfileStateChanged pCB);

void unregisterProfileStateChangedCB(ProfileStateChanged pCB);

#endif
