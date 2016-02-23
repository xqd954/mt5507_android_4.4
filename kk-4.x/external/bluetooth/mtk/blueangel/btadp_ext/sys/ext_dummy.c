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


#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_trc.h"
#include "bt_adp_log.h"

#include "stdarg.h"
#include "stdlib.h"
#include <cutils/log.h>

static char msg_buffer[500] = "[MTKBT]";

void OS_Report(const char *format, ...)
{
    va_list args;
    msg_buffer[7] = 0;
    va_start(args, format);
    vsnprintf(msg_buffer+7, 200, format, args);
    va_end(args);

    ALOGI("%s",msg_buffer);
}

void bt_prompt_trace(module_type mod_id, const char *fmt,...)
{
    va_list args;
    msg_buffer[7] = 0;
    va_start(args, fmt);
    vsnprintf(msg_buffer+7, 300, fmt, args);
    va_end(args);

    ALOGI("%s",msg_buffer);
}

void bt_trace( trace_class_enum trc_class, kal_uint32 msg_index, const char *arg_type,... )
{
//    extern const char* MOD_BT_MSG_FMT[];
//
//    if(!(msg_index > MOD_BT_empty_string && msg_index < MOD_BT__end))
//    {
//        bt_prompt_trace(MOD_BT, "[BT] msg_index=%d out of range", msg_index);
//    }
//
//    va_list args;
//    msg_buffer[7] = 0;
//    va_start(args, arg_type);
//    vsnprintf(msg_buffer+7, 300, MOD_BT_MSG_FMT[msg_index-1], args);
//    va_end(args);
//    if(trc_class == TRACE_ERROR)
//        ALOGE(msg_buffer);
//    else if(trc_class == TRACE_WARNING)
//        ALOGW(msg_buffer);
//    else
//        ALOGI(msg_buffer);
}

char* OS_StrCpy(char *dest, const char *source)
{
      char *s = dest;

      while ((*dest++ = *source++) != 0)
        ;

      return s;
}
