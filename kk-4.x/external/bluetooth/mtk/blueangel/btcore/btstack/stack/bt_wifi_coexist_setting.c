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

#ifndef _BT_WIFI_COEXIST_SETTING_C
#define _BT_WIFI_COEXIST_SETTING_C

/***************************************************************************** 
* Include
*****************************************************************************/

/* basic system service headers */
#include "bt_common.h"
#include "btalloc.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"


/*****************************************************************************
 * FUNCTION
 *  write_wifi_setting
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ptr     [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void write_wifi_setting(bt_wifi_set_chnl_req_struct *ptr)
{
#ifndef __BTMODULE_RFMD3500__
#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, PTRAFHxDPTRPTAxD, ptr->afh, ptr->pta);
    OS_Report("ptr->chnl_map=%X%X%X%X%X%X%X%X%X%X", 
        ptr->chnl_map[0], ptr->chnl_map[1], ptr->chnl_map[2],
        ptr->chnl_map[3], ptr->chnl_map[4], ptr->chnl_map[5],
        ptr->chnl_map[6], ptr->chnl_map[7], ptr->chnl_map[8],
        ptr->chnl_map[9]);
#endif 
    if (ptr->afh != 0)
    {
        bt_prompt_trace(MOD_BT, "BTWIFIC(freq)=%d, ", BTWIFIC(freq));
        OS_Report("BTWIFIC(chnl_map)=%X%X%X%X%X%X%X%X%X%X", 
            BTWIFIC(chnl_map)[0], BTWIFIC(chnl_map)[1], BTWIFIC(chnl_map)[2],
            BTWIFIC(chnl_map)[3], BTWIFIC(chnl_map)[4], BTWIFIC(chnl_map)[5],
            BTWIFIC(chnl_map)[6], BTWIFIC(chnl_map)[7], BTWIFIC(chnl_map)[8],
            BTWIFIC(chnl_map)[9]);
        if (ptr->freq != BTWIFIC(freq)
            #ifdef __DIRECT_CHNL_MAP__
            || OS_MemCmp((const U8*)BTWIFIC(chnl_map), sizeof(BTWIFIC(chnl_map)), (const U8*)ptr->chnl_map, sizeof(ptr->chnl_map)) != 0
            #endif
            )
        {
            bt_prompt_trace(MOD_BT, "AFH command is needed");
            BTWIFIC(afh_cmd_wait) = 1;
            BTWIFIC(freq) = ptr->freq;
            #ifdef __DIRECT_CHNL_MAP__
            OS_MemCopy((U8*)BTWIFIC(chnl_map), (const U8*)ptr->chnl_map, sizeof(BTWIFIC(chnl_map)));
            #endif
            #if defined (WIFI_BB_MT5921) && (defined (__BT_CHIP_VERSION_1__))
            /* We set hb command while freq is different from the current setting. */
            BTWIFIC(hb) = ptr->hb;
            #endif
        }
    #ifdef MTK_BT_DEBUG_TRACE
        else
        {
            kal_trace(TRACE_GROUP_1, THE_SAME_FREQ);
        }
    #endif /* MTK_BT_DEBUG_TRACE */ 
    }
    else
    {
        /* BTWIFIC(afh_cmd_wait) = 0; */
        bt_prompt_trace(MOD_BT, "ptr->afh is 0. Nothing for afh");
    }
    if (ptr->pta != 0)
    {
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(TRACE_GROUP_1, WE_SHALL_CHECK_THE_PTA_COMMAND);
    #endif 
        if (ptr->pta_action != BTWIFIC(pta_action))
        {
            BTWIFIC(pta_cmd_wait) = 1;
            BTWIFIC(pta_action) = ptr->pta_action;
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, PTA_ACTIONxD, BTWIFIC(pta_action));
        #endif 
        }
    #ifdef MTK_BT_DEBUG_TRACE
        else
        {
            kal_trace(TRACE_GROUP_1, THE_SAME_PTA_ACTION);
        }
    #endif /* MTK_BT_DEBUG_TRACE */ 
    }
    else
    {
       /* BTWIFIC(pta_cmd_wait) = 0; */
       bt_prompt_trace(MOD_BT, "ptr->pta is 0. Nothing for pta");
    }
#endif /* __BTMODULE_RFMD3500__ */ 
}
#endif /* _BT_WIFI_COEXIST_SETTING_C */
