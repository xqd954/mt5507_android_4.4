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
 * Bt_adp_system.h
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
 * Dlight Ting
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
#ifndef __BT_ADP_SYSTEM_H__
#define __BT_ADP_SYSTEM_H__

#include "osapi.h"
#include "bttypes.h"

void BTCoreCancelTimer(void);
void BTCoreStartTimer(TimeT time, OsTimerNotify func);
TimeT BTCoreGetSystemTick(void);
TimeT BTCoreGetSystemTick1(void);
void BTCoreSetSysInitState(U8 state);
BOOL BTCoreVerifySysInitState(void);
BOOL BTCoreIsInitializing(void);
BOOL BTCoreIsDeinitializing(void);
BOOL BTCoreReadyToPanic(void);
void BTCoreHandlePanic(void);
void BTCorePowerOnCnf(kal_bool result);
void BTCorePowerOffCnf(kal_bool result);
void BTCoreEnabledSleepMode(U8 state);
void BTCoreHostSleep(kal_uint8 sleep);
BOOL BTCoreCheckNotTestMode(void);
void BTCoreRFTestResult(U8 result);
U8 BTCoreGetNVRMBDAddr(BD_ADDR *addr);
U8 BTCoreSetNVRMBDAddr(U8 *addr);
U8 BTCoreCheckNVRMBDAddr();
void Radio_hostWakeup(void);
#ifndef BTMTK_ON_WISE 
BOOL BTCoreVerifyNotifyEvm(void);
#endif
void BTSysAdpHostSleep(U8 sleep);

#ifdef ANDROID
/* 0 : not, others : yes */
U8 BTCoreIsEngLoad(void);
/* 0 : not internal load, others : yes */
U8 BTCoreIsInternalLoad(void);
#endif
U32 BTCoreGetFWDump(void);
/* 0 : failed, others : success */
U32 BTCoreFWDump(kal_uint32 panic);

void btmtk_adp_system_init(void);
void btmtk_adp_system_power_on_req(void);
void btmtk_adp_system_power_off_req(void);
void btmtk_adp_system_main_loop(void);
void btmtk_adp_system_timer_handler(void);

void bt_adp_profiles_init(void);
void bt_adp_profiles_deinit(void);

kal_bool bt_main(ilm_struct *ilm_ptr);
void bm_adp_message_handler(ilm_struct *ilm_ptr);
void a2dp_adp_message_handler(ilm_struct *ilm_ptr);
void avrcp_adp_message_handler(ilm_struct *ilm_ptr);
void hfga_handler(ilm_struct *ilm_ptr);
void btmtk_adp_spp_handle_message(ilm_struct *ilm_ptr);
#if defined(__BT_OPP_PROFILE__) 
void btmtk_adp_opp_handle_message(ilm_struct *ilm_ptr);
#endif
#if defined(__BT_FTS_PROFILE__) || defined(__BT_FTC_PROFILE__)   
void btmtk_adp_ftp_handle_message(ilm_struct *ilm_ptr);
#endif
#if defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__)
void btmtk_adp_hid_handle_message(void *msg);
#endif
#if defined (__BT_BIPI_PROFILES__) || defined (__BT_BIPR_PROFILE__)
void bip_adp_msg_hdlr(ilm_struct *ilm_ptr);
#endif
#if defined(__BT_PBAP_PROFILE__) 
void bt_adp_pbap_handle_message(ilm_struct *ilm_ptr);
#endif
#if defined(__BT_SIM_PROFILE__)
void btmtk_adp_simap_handle_message(ilm_struct *ilm_ptr);
#endif
#if defined(__BT_BPP_PROFILE__)
void bpp_adp_msg_hdlr(ilm_struct *ilm_ptr);
#endif
#if defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__)
void map_adp_message_handler(ilm_struct *ilm_ptr);
#endif
#if defined(__BT_HDP_PROFILE__)
void btmtk_adp_hdp_handle_message(ilm_struct *ilm_ptr);
#endif

void sdpdba_handler(ilm_struct *ilm_ptr);

void bt_em_test_cmd_handler(ilm_struct *ilm_ptr);

 U32 bt_dbg_manual_dump_req_hdlr(ilm_struct *ilm_ptr);
#ifdef __EXTRA_DEBUG_FEATURE__
 void bt_dbg_query_req_hdlr(ilm_struct *ilm_ptr);
#endif

 #endif /* __BT_ADP_SYSTEM_H */
