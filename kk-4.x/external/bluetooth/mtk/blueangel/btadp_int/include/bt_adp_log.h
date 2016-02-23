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
 * bt_adp_log.h
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
 * Autumn Li
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
#ifndef __BT_ADP_LOG_H__
#define __BT_ADP_LOG_H__

#ifndef BTMTK_ON_WISE
#define MAX_MSG_NBR_IN_MODULE       20
#define MAX_ARGUMENT_SIZE      128
#define	MAX_INT_BUF_LENGTH  16

#define PS_TRACE_INDEX_TRACE_TYPE   0x61
#define PS_PROMPT_TRACE_TYPE        0x62

#ifdef GEN_FOR_PC
#ifndef GEN_FOR_CPARSER
   #define BEGIN_TRACE_MAP(MOD)     START_MODULE_MSG MOD MAX_MSG_NBR_IN_MODULE
   #define TRC_MSG(MSG,STR)         MSG STR
   #define END_TRACE_MAP(MOD)       END_MODULE_MSG
#else
   #define BEGIN_TRACE_MAP(MOD)     enum MOD##_MSG_ENUM{\
                                       MOD##_empty_string=MOD<<MAX_MSG_NBR_IN_MODULE,
   #define TRC_MSG(MSG,STR)         MSG##__enum,
   #define  END_TRACE_MAP(MOD)      MOD##__end};
#endif
#else /* GEN_FOR_PC */
    #ifndef MAKE_FORMAT_STRING
   #define BEGIN_TRACE_MAP(MOD)     enum MOD##_MSG_ENUM{\
                                       MOD##_empty_string=MOD<<MAX_MSG_NBR_IN_MODULE,
   #define TRC_MSG(MSG,STR)         MSG##__enum,
   #define  END_TRACE_MAP(MOD)      MOD##__end};
   #else
   #define BEGIN_TRACE_MAP(MOD)     const char* MOD##_MSG_FMT[] = {
   #define TRC_MSG(MSG,STR)         STR,
   #define  END_TRACE_MAP(MOD)      };
   #endif
#endif /* GEN_FOR_PC */

typedef enum
{
   TRACE_FUNC,
   TRACE_STATE,
   TRACE_INFO,
   TRACE_WARNING,
   TRACE_ERROR,
   TRACE_GROUP_1,
   TRACE_GROUP_2,
   TRACE_GROUP_3,
   TRACE_GROUP_4,
   TRACE_GROUP_5,
   TRACE_GROUP_6,
   TRACE_GROUP_7,
   TRACE_GROUP_8,
   TRACE_GROUP_9,
   TRACE_GROUP_10,
   TRACE_PEER
}trace_class_enum;

/*!! Only for MAUI platform !! */
typedef enum {
   INVALID_SAP = 0,
/* PS_BEGIN */
   BCHS_L4C_SAP,
   BCHS_MMI_SAP,
   BT_A2DP_SAP,
   BT_APP_SAP,
   BT_AVRCP_SAP,
   BT_HFG_SAP,
   BT_BPP_SAP,
   BT_BIP_SAP,
   BT_PBAP_SAP,
   BT_OBEX_SAP,
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) || defined(__LINUX_SUPPRESS_ERROR__)
   PS_SIM_SAP,
#endif
   LAST_SAP_CODE = 0xFFFFFFFF
} sap_type;

typedef struct
{
   kal_uint32        frame_number;
   kal_uint32        time_stamp;
   kal_uint32        msg_index;
   trace_class_enum  trace_class;
   kal_uint16        buf_length;
} tst_index_trace_header_struct;

typedef struct
{
   kal_uint32     frame_number;
   kal_uint32     time_stamp;
   module_type    module_id;
   kal_uint16     buf_length;
} tst_prompt_trace_header_struct;

typedef struct
{
	const kal_uint8* pTraceHeader;
	const kal_uint8* pData;
	const kal_uint8* pData2;
	kal_uint32 lenTraceHeader;
	kal_uint32 lenData;
	kal_uint32 lenData2;
	kal_uint32 frameMessageType;
} tst_trace_frame_arg;

typedef struct
{
   kal_uint32     frame_number;
   kal_uint32     time_stamp;
   module_type    src_mod_id;
   module_type    dest_mod_id;
   sap_type       sap_id;
   msg_type       msg_id;
   kal_uint16     local_len;
   kal_uint16     peer_len;
   kal_uint32     local_addr;
   kal_uint32     peer_addr;
} tst_log_prim_header_struct;
#ifdef __LINUX_SUPPRESS_ERROR__
void bt_trace( trace_class_enum trc_class, kal_uint32 msg_index, const char *arg_type,...);
#else
void bt_trace( trace_class_enum trc_class, kal_uint32 msg_index, ...);
#endif
#if !defined(__FOOTPRINT_MEASUREMENT__)
void bt_prompt_trace(module_type mod_id, const char *fmt,...);
#else
#define bt_prompt_trace(mod_id, fmt,...) (void)0
#endif

void bt_log_primitive(ilm_struct* ilm_ptr);
void bt_fw_assert(kal_uint32 panic);
void bt_assert(char* expr, char* file, kal_uint32 line);
void bt_assert_reset(char* expr, char* file, kal_uint32 line);
void bt_log_setting(kal_uint32 trace, kal_uint8 primitive, kal_uint8 prompt);
#endif  /* BTMTK_ON_WISE */

#endif /* __BT_ADP_LOG_H__ */
