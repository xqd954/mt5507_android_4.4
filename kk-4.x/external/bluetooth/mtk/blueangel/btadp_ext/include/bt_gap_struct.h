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
 * Bt_gap_struct.h
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Tina Shen
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

#ifndef _BT_GAP_STRUCT_H_
#define _BT_GAP_STRUCT_H_

#include <stdlib.h>
#include <hardware/bluetooth.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <cutils/properties.h>
#include <errno.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/poll.h>
#include <sys/un.h>
#include "cutils/sockets.h"


#include "bt_ext_timer.h"
#include "bt_ext_common.h"
#include "bt_ext_event.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"

#define BTMTK_CACHE_FOLDER  "data/@btmtk"
#define BTMTK_DEV_CACHE_PATH  "data/@btmtk/dev_cache"  /* paired device */
#define BTMTK_HOST_CACHE_PATH  "data/@btmtk/host_cache"  /* host setting, ex. name, mode */

#define MIN(a,b) ((a < b) ? a : b)

#define BTMTK_ANDROID_DEFAULT_LOCAL_NAME "ANDROID BT"
#define BTMTK_ANDROID_DEFAULT_REMOTE_NAME "UNKNOWN DEVICE"
#define BTMTK_ANDROID_ADAPTER_PREFIX "MTKBT"
#define BTMTK_MAX_DEVICE_NAME_SIZE 80
#define BTMTK_MAX_PAIRED_LIST_NO   20
#define BTMTK_MAX_OBJECT_PATH_SIZE 32 /* "MTKBT/dev_XX_XX_XX_XX_XX_XX" + '\0' */
#define BTMTK_MAX_ADDR_STR_SIZE 20 /* "XX:XX:XX:XX:XX:XX" + '\0' */
#define BTMTK_MAX_UUID_STR_SIZE 37 /* "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX" + '\0' */
#define BTMTK_SDP_UUID_16_BIT_SIZE 2
#define BTMTK_SDP_UUID_32_BIT_SIZE 4
#define BTMTK_SDP_UUID_128_BIT_SIZE 16
#define BTMTK_MAX_STACK_TIMEOUT 15000
#define BTMTK_MAX_REMOTE_TIMEOUT 90000

#define DEFAULT_INITIAL_POLLFD_COUNT NUM_OF_PROFILE

#define EVENT_LOOP_EXIT 1
#define EVENT_LOOP_ADD  2
#define EVENT_LOOP_REMOVE 3
#define EVENT_LOOP_WAKEUP 4

typedef enum
{
    BTMTK_GAP_ACT_NONE = 0x0,
    BTMTK_GAP_ACT_INQUIRY = 0x1,
} btmtk_gap_activity_enum;

typedef struct {
    LOCAL_PARA_HDR
    bt_addr_struct addr;
    bt_property_type_t type;
} btmtk_read_remote_property_struct;

typedef struct {
    LOCAL_PARA_HDR
    bt_property_type_t type;
} btmtk_read_adapter_property_struct;

typedef struct {
	LOCAL_PARA_HDR
	int status;
} btmtk_update_bt_status_struct;

typedef struct {
	LOCAL_PARA_HDR
	kal_uint32 scan_mode_timeout;
} btmtk_set_scan_mode_timeout_struct;

typedef struct
{
    LOCAL_PARA_HDR
    bt_addr_struct addr;
    int name_len;
    char name[BTBM_ADP_MAX_NAME_LEN];
} btmtk_paired_device_rename_struct;

typedef struct
{
    LOCAL_PARA_HDR 
    btbm_bd_addr_t bd_addr;
    kal_bool result;
} btmtk_bonding_req_struct;

typedef union
{
	bt_bm_security_user_confirm_rsp_struct   user_confirm_rsp;
	bt_bm_security_passkey_entry_rsp_struct  passkey_entry_rsp;
	bt_bm_security_passkey_notify_rsp_struct passkey_notify_rsp;
	bt_bm_security_oob_data_rsp_struct       oob_data_rsp;
}btmtk_ssp_reply_struct;

typedef enum
{
    EV_BT_CB_TASK_START = EV_BT_CB_GAP_GROUP_START,
	EV_BT_CB_ENABLE_SERVICE,
	EV_BT_CB_DISABLE_SERVICE,
	EV_BT_CB_ENABLE_ADVANCE_SERVICE,
    EV_BT_CB_DISABLE_ADVANCE_SERVICE,
	EV_BT_CB_REMOVE_BOND,
    EV_BT_CB_CANCEL_BOND,
    EV_BT_CB_CREATE_BOND,
    EV_BT_CB_DISCOVERY_STOPPED,
    EV_BT_CB_DISCOVERY_STARTED,
    EV_BT_CB_PAIRED_DEVICE_RENAME,
    EV_BT_CB_WRITE_SCAN_MODE_TIMEOUT,
    EV_BT_CB_READ_REMOTE_PROPERTY,
    EV_BT_CB_READ_REMOTE_PROPERTY_ALL,
    EV_BT_CB_READ_ADAPTER_PROPERTY,
    EV_BT_CB_READ_ADAPTER_PROPERTY_ALL,
    EV_BT_CB_BT_STATUS_IND,
    EV_BT_CB_PROFILE_CONNECT_REQ,
    EV_BT_CB_PROFILE_DISCONNECT_REQ,
    EV_BT_CB_DISABLE,
    EV_BT_CB_UNREGISTER_HANDLER,
    EV_BT_CB_TASK_EXIT,
} btmtk_android_gap_event_enum;

typedef enum
{
    BTMTK_POWER_STATE_OFF,
	BTMTK_POWER_STATE_TURNING_ON,
    BTMTK_POWER_STATE_ON,
    BTMTK_POWER_STATE_TURNING_OFF
} btmtk_power_state_enum;

typedef struct
{
	kal_uint32  service_id;
	kal_bool bEnable;
}btmtk_execute_services_struct;

typedef struct
{
    unsigned char scan_mode;           /* default: BTBM_ADP_P_ON_I_ON */
    char name[BTBM_ADP_MAX_NAME_LEN];  /* default: BTMTK_ANDROID_DEFAULT_LOCAL_NAME */
    unsigned int scan_mode_timeout;
	bt_addr_struct local_addr;
} btmtk_host_cache_struct;

typedef enum
{
	BTMTK_BOND_STATE_UNBOND,
	BTMTK_BOND_STATE_BONDED,	
	BTMTK_BOND_STATE_BONDING,
}btmtk_bond_state;

typedef struct
{
    bt_addr_struct addr;
    char name[BTBM_ADP_MAX_NAME_LEN];
	char nickname[BTBM_ADP_MAX_NAME_LEN];
    /* icon */
    unsigned int cod;
    struct
    {
        unsigned char uuid_no;
        unsigned short uuid[BTBM_ADP_MAX_SDAP_UUID_NO];
        unsigned short channel[BTBM_ADP_MAX_SDAP_UUID_NO];  /* map to uuid */

        unsigned char app_uuid_no;
        unsigned char app_uuid[BTBM_ADP_MAX_SDAP_APP_UUID_NO][16];
        unsigned short app_channel[BTBM_ADP_MAX_SDAP_APP_UUID_NO];  /* map to uuid */
    } sdp;
    btmtk_bond_state paired;
    kal_bool connected;
    kal_bool trusted;
    /* alias */
    /* nodes */
    /* adapter */
    kal_bool legacy_pairing;
    short rssi;
    btbm_device_type device_type;
    /* TX */
//	bool pairing;
} btmtk_device_entry_struct;

typedef struct{
	char nameOfList;
	char list[];
}btmtk_auto_pair_blacklist;

typedef enum {
	BTMTK_AUTH_TYPE_NONE,
    BTMTK_AUTH_TYPE_PASSKEY_CONFIRMATION,
    BTMTK_AUTH_TYPE_PASSKEY_ENTRY,
    BTMTK_AUTH_TYPE_CONSENT,
    BTMTK_AUTH_TYPE_PASSKEY_NOTIFICATION,
    BTMTK_AUTH_TYPE_PIN_CODE,
} btmtk_auth_type_enum;

typedef enum
{
	BTMTK_BOND_ACTIVITY_NONE = 0,
	BTMTK_BOND_ACTIVITY_BONDING,
	BTMTK_BOND_ACTIVITY_SDP,
	BTMTK_BOND_ACTIVITY_NUM,
} btmtk_bond_activity_enum;

typedef struct _btmtk_bond_req_struct
{
    struct _btmtk_bond_req_struct *next;

    btmtk_bond_activity_enum activity;
	btmtk_auth_type_enum     auth_type;
    bt_addr_struct addr;
    unsigned char uuid_no;
    unsigned short uuid[BTBM_ADP_MAX_SDAP_UUID_NO];
    unsigned char app_uuid_no;
    unsigned char app_uuid[16];
	int autopair_attempts;
} btmtk_bond_req_struct;

#define BTMTK_READ_LOCAL_ADDR_MASK     0
#define BTMTK_READ_LOCAL_NAME_MASK     1
#define BTMTK_READ_LOCAL_UUID_MASK     2
#define BTMTK_READ_LOCAL_COD_MASK      3
#define BTMTK_SET_SCAN_MODE_MASK       4

typedef struct _btmtk_unbond_pending_list{
    struct _btmtk_unbond_pending_list *next;
	bt_addr_struct  bd_addr;
}btmtk_unbond_pending_list;

typedef struct{
	pthread_t				  bt_evt_task_id;
	pthread_mutex_t 		  thread_mutex;
	pthread_cond_t			  thread_cond;

	btbm_bd_addr_t            local_bd_addr;
	kal_uint32                local_service_list[6];
	kal_uint32                local_cod;
	
	struct pollfd            *pollData;
    int                       pollMemberCount;
    int                       pollDataSize;
	
	/* pair of sockets for event loop control, Reader and Writer */
    int                       controlFdR;
    int                       controlFdW;
	
	int                       appsock;

	kal_uint32                enabled_services;
	kal_uint32                conn_no; 
	kal_bool                  disc_acl_action;
	btmtk_power_state_enum	  bt_status;
	btmtk_gap_activity_enum   activity;
	btmtk_bond_state          bond_state;
	int                       discovery_cancel_pending;
	int                       read_local_feature_flag;
	int                       hcisnoop_enabled;
	btmtk_host_cache_struct   host_cache;
	btmtk_ext_msg_handler_struct *pHandler;

	btmtk_bond_req_struct     *requests; /* SDP request in progressing */
    btmtk_unbond_pending_list *unbond_pending_list;
	
	struct btTimer            disc_timer;
	struct btTimer            rebond_timer;
	bt_addr_struct            rebond_dev_addr;
	
    int paired_cache_no;
    int inquired_cache_no;
	btmtk_device_entry_struct paired_dev_cache[BTBM_ADP_MAX_PAIRED_LIST_NO];
    btmtk_device_entry_struct inquired_dev_cache[BTBM_ADP_MAX_INQUIRY_NO];
}bt_gap_context_t;

#endif

