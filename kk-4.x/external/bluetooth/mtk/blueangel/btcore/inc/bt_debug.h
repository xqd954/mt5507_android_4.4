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
 * bt_debug.h
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
#ifndef __BT_DEBUG_H__
#define __BT_DEBUG_H__

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "MTKBT"

#if defined(BTMTK_ON_LINUX)
#ifndef GEN_FOR_PC
#include <cutils/log.h>
#endif
#endif

#define XLOGV ALOGV
#define XLOGI ALOGI
#define XLOGD ALOGD
#define XLOGW ALOGW
#define XLOGE ALOGE
/* +VSNIFF */
typedef int (*vsniff_write)(const unsigned char *, unsigned int);
typedef int (*vsniff_read)(unsigned char, unsigned char *, unsigned int);

extern vsniff_write p_sniffwrite;
extern vsniff_read p_sniffread;
void initVirtualSniff(void);
void deinitVirtualSniff(void);
/* -VSNIFF */

#ifdef BTMTK_ON_WISE
#define bt_trace kal_trace
#define bt_log_primitive tst_log_primitive
#define bt_prompt_trace kal_prompt_trace
#else
#define kal_prompt_trace bt_prompt_trace
#define kal_trace bt_trace
#endif

#define DEBUG_LEVEL_3 3
#define DEBUG_LEVEL_2 2
#define DEBUG_LEVEL_1 1

#define bt_tst_log(...)
#define bt_tst_err(...)            ALOGE(__VA_ARGS__)
#define bt_tst_dbg(...)           ALOGD(__VA_ARGS__)
#define bt_android_log(fmt, ...)    ALOGD("%s : " fmt, __FUNCTION__, ## __VA_ARGS__)

#if defined(DEBUG_LEVEL)
#if DEBUG_LEVEL >= DEBUG_LEVEL_1
#define bt_trace_lv1                   bt_trace
#define bt_prompt_trace_lv1     bt_prompt_trace
#else
#define bt_trace_lv1(...)
#define bt_prompt_trace_lv1(...)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_2
#define bt_trace_lv2                   bt_trace
#define bt_prompt_trace_lv2     bt_prompt_trace
#else
#define bt_trace_lv2(...)
#define bt_prompt_trace_lv2(...)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_3
#define bt_trace_lv3                   bt_trace
#define bt_prompt_trace_lv3     bt_prompt_trace
#else
#define bt_trace_lv3(...)
#define bt_prompt_trace_lv3(...)
#endif
#else /* DEBUG_LEVEL */
#define bt_trace_lv1(...)
#define bt_prompt_trace_lv1(...)
#define bt_trace_lv2(...)
#define bt_prompt_trace_lv2(...)
#define bt_trace_lv3(...)
#define bt_prompt_trace_lv3(...)
#endif /* DEBUG_LEVEL */

typedef U8 BtPanic;

#define BT_PANIC_WRONG_NUM_OF_COMPLETE_PKT       1
#define BT_PANIC_HW_ERROR                                           2
#define BT_PANIC_DATA_BUFFER_OVERFLOW                    3
#define BT_PANIC_RESET_FAILED                                      4
#define BT_PANIC_READ_BUF_FAILED                               5
#define BT_PANIC_WRITE_SCO_FC_ENABLE_FAILED         6
#define BT_PANIC_WRITE_HOST_BUFFER_SIZE_FAILED    7
#define BT_PANIC_SET_FLOW_CTL_FAILED                              8
#define BT_PANIC_HOST_WAKEUP_FAILED                        9
#define BT_PANIC_INVALID_HCI_BUF_TYPE                              11
#define BT_PANIC_GET_ACL_BUF_FAILED                              12
#define BT_PANIC_GET_SCO_BUF_FAILED                              13
#define BT_PANIC_GET_EVENT_BUF_FAILED                              14
#define BT_PANIC_INVALID_HCI_FRAME                              15
#define BT_PANIC_UART_WAKEUP_TIMEOUT                              19
#define BT_PANIC_HCI_CMD_NO_ACK                              20
#define BT_PANIC_POWER_ON_FAIL                              21
#define BT_PANIC_HOST_ASSERT                                             31 /* Special usage in bt_assert to indicate host assert */


#endif
