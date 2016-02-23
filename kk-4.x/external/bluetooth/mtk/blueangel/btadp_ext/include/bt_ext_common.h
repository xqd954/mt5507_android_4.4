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
 * Bt_ext_common.h
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

#ifndef _BT_EXT_COMMON_H_
#define _BT_EXT_COMMON_H_

#include "bt_mmi.h"

typedef enum{
	BTMTK_RES_SERVICE_ID = 0,      /* Reserved */
	BTMTK_SPP_SERVICE_ID,          /* Serial port profile. */
	BTMTK_DUN_SERVICE_ID,          /* Dial-up networking profile. */
	BTMTK_A2DP_SOURCE_SERVICE_ID,  /* A2DP Source profile. */
	BTMTK_LAP_SERVICE_ID,          /* LAN access profile. */
	BTMTK_HSP_SERVICE_ID,          /* Headset profile. */
	BTMTK_HFP_SERVICE_ID,          /* Hands-free profile. */
	BTMTK_OPP_SERVICE_ID,          /* Object push  */
	BTMTK_FTP_SERVICE_ID,          /* File transfer */
	BTMTK_CTP_SERVICE_ID,          /* Cordless Terminal */
	BTMTK_ICP_SERVICE_ID,          /* Intercom Terminal */
	BTMTK_SYNC_SERVICE_ID,         /* Synchronization */
	BTMTK_BPP_SERVICE_ID,          /* Basic printing profile */
	BTMTK_BIP_SERVICE_ID,          /* Basic Imaging profile */
	BTMTK_PAN_SERVICE_ID,         /* PAN profile */
	BTMTK_SAP_SERVICE_ID,          /* SIM Access profile */
	BTMTK_A2DP_SERVICE_ID,         /* A2DP Sink */
	BTMTK_AVRCP_SERVICE_ID,        /* A/V remote control */
	BTMTK_HID_SERVICE_ID,          /* HID */
	BTMTK_VDP_SERVICE_ID,          /* Video distribution */
	BTMTK_PBAP_SERVICE_ID,         /* PhoneBook Access Server*/
	BTMTK_HSP_HS_SERVICE_ID,       /* HFP HS role */
	BTMTK_HFP_HS_SERVICE_ID,       /* HSP HS role */
	BTMTK_MAP_SERVICE_ID,          /* Message Access Profile */
	BTMTK_MN_SERVICE_ID,           /* Message Notification Service */
	BTMTK_HDP_SERVICE_ID,          /* Health Device Profile */
	BTMTK_PCE_SERVICE_ID,          /* PhoneBook Access Client*/
#ifdef __BT_4_0_BLE__
	BTMTK_BLE_SERVICE_ID,             /* GATT profile */
#endif
	BTMTK_MAX_SERVICE_ID,
}BTMTK_SERVICE_ID;

#define BTMTK_SDP_UUID_SERIAL_PORT              0X1101
#define BTMTK_SDP_UUID_DIALUP_NETWORKING        0X1103
#define BTMTK_SDP_UUID_IRMC_SYNC                0X1104
#define BTMTK_SDP_UUID_OBEX_OBJECT_PUSH         0X1105
#define BTMTK_SDP_UUID_OBEX_FILE_TRANSFER       0X1106
#define BTMTK_SDP_UUID_HEADSET                  0X1108
#define BTMTK_SDP_UUID_AUDIO_SOURCE             0X110A
#define BTMTK_SDP_UUID_AUDIO_SINK               0X110B
#define BTMTK_SDP_UUID_AV_REMOTE_CONTROL_TARGET 0X110C
#define BTMTK_SDP_UUID_ADV_AUDIO_DISTRIBUTION   0X110D
#define BTMTK_SDP_UUID_AV_REMOTE_CONTROL        0X110E
#define BTMTK_SDP_UUID_FAX                      0X1111
#define BTMTK_SDP_UUID_HEADSET_AUDIO_GATEWAY    0X1112
#define BTMTK_SDP_UUID_PANU   									0X1115
#define BTMTK_SDP_UUID_PAN_NAP  								0X1116
#define BTMTK_SDP_UUID_PAN_GN   								0X1117
#define BTMTK_SDP_UUID_DIRECT_PRINTING          0X1118
#define BTMTK_SDP_UUID_IMAGING_RESPONDER        0X111B
#define BTMTK_SDP_UUID_HF_HANDSFREE             0X111E
#define BTMTK_SDP_UUID_AG_HANDSFREE             0X111F
#define BTMTK_SDP_UUID_BASIC_PRINTING           0X1122
#define BTMTK_SDP_UUID_HUMAN_INTERFACE          0X1124
#define BTMTK_SDP_UUID_SAP                      0X112D
#define BTMTK_SDP_UUID_PBAP_PCE                 0X112E
#define BTMTK_SDP_UUID_PBAP_PSE                 0X112F
#define BTMTK_SDP_UUID_GENERIC_AUDIO            0X1203
#define BTMTK_SDP_UUID_MAP                      0x1134

#define BTMTK_SDP_UUID_PRX						0x1803  /* Link loss (Mandatory) */

#define BTMTK_SDP_UUID_INSECURE					0x2001
#define BTMTK_SDP_UUID_SECURE					0x2002

typedef struct _btmtk_list_header_struct
{
    struct _btmtk_list_header_struct *next;
} btmtk_list_header_struct;

typedef void (*btmtk_ext_msg_handler)(void *pContext, ilm_struct *data);

typedef struct _btmtk_ext_msg_handler_struct{
	struct _btmtk_ext_msg_handler_struct *next;
	int profile_id;
	int socket;
	btmtk_ext_msg_handler handler; 
	void *pContext;
}btmtk_ext_msg_handler_struct;

typedef void (*btmtk_profile_disconnect)(void);

typedef struct _btmtk_profile_disconnect_callback{
	struct _btmtk_profile_disconnect_callback *next;
	int profile_id;
	btmtk_profile_disconnect func;
}btmtk_profile_disconnect_callback;

#endif





