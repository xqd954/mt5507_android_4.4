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

/****************************************************************************
 *
 * File:
 *     $Workfile:a2dp.h$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains public definitions for the Audio/Video
 *     Remote Control Profile (AVRCP).
 *             
 * Created:     Nov 19, 2003
 *
 * Copyright 2003 - 2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, Inc. 
 * This work contains confidential and proprietary information of Extended 
 * Systems, Inc. which is protected by copyright, trade secret, trademark and 
 * other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __A2DP_H_
#define __A2DP_H_

#include <assert.h>

#include "bluetooth_a2dp_struct.h"
#include "bt_adp_a2dp.h"
#include "sdp_adp.h"
#include "conmgr.h"
#include "gavdp.h"

#define MAX_QOS_IND_VAL 10
#define PACKET_BUFFER_SIZE (20*1024)
#define QOS_COUNTER_HIGH_THRESHOLD 50
#define QOS_COUNTER_LOW_THRESHOLD 5

#define ASSERT assert

typedef enum
{
    BT_A2DP_STATE_NON_INITIALIZED,
    BT_A2DP_STATE_INITIALIZED,
    BT_A2DP_STATE_ACTIVATED,
    BT_A2DP_STATE_CONNECTED,
    BT_A2DP_STATE_CONFIGURED,
    BT_A2DP_STATE_OPEN,
    BT_A2DP_STATE_STREAMING,
    NO_OF_BT_A2DP_STATE
} BT_A2DP_STATE_ENUM;

typedef enum
{
    BT_A2DP_WAIT_RES_NONE,
/*    BT_A2DP_WAIT_RES_DISCOVER,
    BT_A2DP_WAIT_RES_GET_CAP,
    BT_A2DP_WAIT_RES_CONNECT,
    BT_A2DP_WAIT_RES_CONFIG, */
    BT_A2DP_WAIT_RES_OPEN,
    BT_A2DP_WAIT_RES_RECONFIG,
    BT_A2DP_WAIT_RES_START,
    BT_A2DP_WAIT_RES_SUSPEND,
    BT_A2DP_WAIT_RES_CLOSE,
    BT_A2DP_WAIT_RES_ABORT,
    NO_OF_BT_A2DP_WAIT_RES
} BT_A2DP_WAIT_RES_ENUM;

typedef enum
{
    BT_A2DP_WAIT_CNF_NONE,
/*    BT_A2DP_WAIT_CNF_DISCOVER,
    BT_A2DP_WAIT_CNF_GET_CAP,*/
    BT_A2DP_WAIT_CNF_DATA_LINK,
    BT_A2DP_WAIT_CNF_SDP_QUERY,
/*    BT_A2DP_WAIT_CNF_CONNECT,
    BT_A2DP_WAIT_CNF_DISCONNECT,
    BT_A2DP_WAIT_CNF_CONFIG, */
    BT_A2DP_WAIT_CNF_OPEN,
    BT_A2DP_WAIT_CNF_RECONFIG,
    BT_A2DP_WAIT_CNF_START,
    BT_A2DP_WAIT_CNF_SUSPEND,
    BT_A2DP_WAIT_CNF_CLOSE,
    BT_A2DP_WAIT_CNF_ABORT,
    NO_OF_BT_A2DP_WAIT_CNF
} BT_A2DP_WAIT_CNF_ENUM;


typedef struct
{
    StreamEndPoint *local_sep;
    kal_bool in_use;
    BD_ADDR remote_bd_addr;
    
    U8 wait_res;
    U8 wait_cnf;
    BOOL wait_started;
    BOOL postpond_suspend;
    U16 data_seq_no;
    U32 data_timestamp;
    U8 packet_buffer_count;   
    U8 device_type;
    
    //Add for PTS testing 2007-0606: Because the first sdp query with AUDIP_DISTRIBUTION is no rsp, the next query with AUDIO_SINK must be started 
    U8 a2dp_sdp_query_count;
    
    SdpQueryToken sqt;
    CmgrHandler cmgr_hdlr;
} A2dpDevContext;

typedef struct 
{
    A2dpDevContext a2dp_device_list[MAX_A2DP_DEV_NUM];
    SdpRecord sdp_record;
    U8 a2dp_sdp_query_count;
    BTA2DPCallback a2dpCBfunc;
    U8 glb_qos_counter;
    EvmTimer qos_timer;
    U8 qos_index;
    U16 qos_counter;
    BOOL qos_reported;
    
    U8 packet_buffer[PACKET_BUFFER_SIZE];
    
    StreamEndPoint local_sep_List[MAX_A2DP_SEP_NUM];
    U8 local_sep_num;

    /* used for FM Over Bt Feature */
	U8 fm_over_bt_mode;
	U8 fm_over_sbc_enabled;
    AvdtpCodec codec;
    BOOL is_a2dp_active;
    U8 sbc_element[10];
    
} bt_a2dp_ctx_struct;


#endif /* __A2DP_H_ */
