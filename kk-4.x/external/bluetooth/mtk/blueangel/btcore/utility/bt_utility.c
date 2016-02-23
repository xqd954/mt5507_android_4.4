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

#ifndef _BT_UTILITY_C
#define _BT_UTILITY_C

/***************************************************************************** 
* Include
*****************************************************************************/
/* basic system service headers */
#include "bt_common.h"
#include "xatypes.h"
#include "osapi.h"

/* internal headers */

/***************************************************************************** 
* Define
*****************************************************************************/

/***************************************************************************** 
* Typedef 
*****************************************************************************/

/***************************************************************************** 
* Local Variable
*****************************************************************************/

/***************************************************************************** 
* Local Function
*****************************************************************************/



/*****************************************************************************
 * FUNCTION
 *  BTBMDumpBdAddr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  addr        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
void BTBMDumpBdAddr(const U8 *addr)
{
    kal_trace(
        TRACE_GROUP_1,
        BTLOG_INPUT_ADDRx02Xx02Xx02Xx02Xx02Xx02X,
        addr[0],
        addr[1],
        addr[2],
        addr[3],
        addr[4],
        addr[5]);
}



/*****************************************************************************
 * FUNCTION
 *  BTCheckPktServicedTimeLong
 * DESCRIPTION
 *  
 * PARAMETERS
 *  int_time        [IN]        
 *  time_value      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL BTCheckPktServicedTimeLong(U32 int_time, U32 time_value)
{
    U32 time_interval = 0;
    U32 current_time = 0;

    current_time = OS_GetSystemTime();

    if (current_time >= int_time)
    {
        time_interval = current_time - int_time;
    }
    else
    {
        time_interval = current_time + ((0xFFFFFFFF) - int_time);
    }
    if (time_interval >= time_value)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


#if 0
void bt_trace(trace_class_enum trc_class, U32 msg_index, const char* arg_type, ...)
{
    kal_trace(trc_class, msg_index, arg_type);
}
#endif


#endif /* _BT_UTILITY_C */ 

