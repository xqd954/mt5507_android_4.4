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

#ifndef _BT_PROFILE_INIT_C
#define _BT_PROFILE_INIT_C

#include "bt_common.h"
#include "btconfig.h"
#include "osapi.h"
#include "sdap.h"
#include "btbm.h"
#include "btaccctrl.h"
#include "conmgr.h"

/*****************************************************************************
 * FUNCTION
 *  bt_profiles_init
 * DESCRIPTION
 *  This is for profiles and adaptor init funciton
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_profiles_init(void)
{

#ifdef BTMTK_ON_WISE 
        /* Adaptor and profile init */
    #if (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED)
    #ifdef __BT_SPP_PROFILE__
        SPPDRV_Init();
    #endif 
    #endif /* (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED) */
        SDAP_Init();
        BTBMInit();
        BTAccCtrlInit();
        CMGR_Init();
#else   /* BTMTK_ON_WISE */
    /* Adaptor and profile init */
    SDAP_Init();
    BTBMInit();
    BTAccCtrlInit();
    CMGR_Init();
#endif  /* BTMTK_ON_WISE */
}

/*****************************************************************************
 * FUNCTION
 *  bt_profiles_deinit
 * DESCRIPTION
 *  This is for profiles and adaptor init funciton
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_profiles_deinit(void)
{
    bt_prompt_trace(MOD_BT, "bt_profiles_deinit");
    CMGR_Deinit();
}

#if 0
/*****************************************************************************
 * FUNCTION
 *  bt_profiles_reset
 * DESCRIPTION
 *  This is for profiles and adaptor reset funciton
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_profiles_reset(void)
{
    return;
}
#endif
#endif
