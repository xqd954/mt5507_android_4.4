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
 * Bt_adp_profile_init.c
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
#ifndef _BT_ADP_PROFILE_INIT_C__
#define _BT_ADP_PROFILE_INIT_C__

#include "bt_common.h"
#include "btconfig.h"
#include "osapi.h"

#include "bluetooth_gap_struct.h"
#include "btbm_adp.h"

#include "bt_maui_btbm.h"
#ifdef __BT_GAP_PROFILE__
extern void bm_adp_init(void);
extern void bm_adp_deinit(void);
#endif

#ifdef __BT_A2DP_PROFILE__
extern void A2DP_Init(void);
extern void a2dp_adp_init(void);
#endif

#ifdef __BT_AVRCP_PROFILE__
extern BOOL AVRCP_Init(void);
extern void bt_adp_avrcp_init(void);
extern void AVRCP_UtilInit(void);
#endif 

#ifdef __BT_HFG_PROFILE__
extern BOOL hfg_adp_init(void);
#endif

extern void btmtk_ashm_start_service();

#ifdef __BT_SPP_PROFILE__
extern void spp_adp_init(void);
#endif

#ifdef __BT_GOEP_PROFILE__        
extern BOOL OBEX_Init(void);
extern BOOL GOEP_Init(void);
#endif    

#ifdef __BT_OPP_PROFILE__ 
extern void opps_adp_init(void);
extern void oppc_adp_init(void);
#endif

#if defined(__BT_FTS_PROFILE__)
extern void ftps_adp_init(void);
#endif

#if defined(__BT_FTC_PROFILE__)
extern void ftpc_adp_init(void);
#endif 

#if defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__)
extern BOOL HID_Init(void);
#endif

#if defined(__BT_HIDD_PROFILE__)
extern void hidda_init(void);
#endif

#if defined(__BT_HIDH_PROFILE__)
extern void hidha_init(void);
#endif

#ifdef __BT_SIM_PROFILE__
extern void simap_adp_init(void);
extern void simap_adp_deinit(void);
extern void simap_adp_reset(void);
#endif
//mtk03036
#if defined(__BT_BIPI_PROFILE__) || defined(__BT_BIPR_PROFILE__)
extern void bip_adp_init(void);
#endif
//mtk03036 end

#if defined(__BT_PBAP_PROFILE__)
extern void pbap_adp_init(void);
#endif

#ifdef __BT_BPP_PROFILE__
extern void bpp_init(void);
extern void bpp_adp_init(void);
#endif

#if defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__)
extern void map_adp_init();
extern void map_adp_deinit();
#endif

#if (BT_JSR82_ENABLED == 1) || defined(__BT_JSR82_L2RF__)
extern void jsr82_adp_init(void);
extern void jsr82_adp_deinit(void);
#endif

#ifdef __BT_PAN_PROFILE__
extern BOOL PAN_Init(void);
extern void pan_adp_init(void);
#endif

#ifdef __BT_3_0_HS__
extern void check_pal_struct(void);
#endif

#if defined(__BT_PRXR_PROFILE__) || defined(__BT_PRXM_PROFILE__)
extern void prx_adp_init(void);
#endif
#if defined(__BT_PRXR_PROFILE__) 
extern void prxr_adp_reset(void);
#endif
#if defined(__BT_PRXM_PROFILE__)
extern void prxm_adp_reset(void);
#endif
#if defined(__BT_HDP_PROFILE__)
extern void hdp_adp_init();
extern void hdp_adp_deinit();
#endif
#if defined(__BT_HTPC_PROFILE__) || defined(__BT_HTPT_PROFILE__)
extern void HTP_Init(void);
#endif
#if defined(__BT_HTPC_PROFILE__)
extern void htpc_adp_init(void);
#endif
#if defined(__BT_HTPT_PROFILE__)
extern void htpt_adp_init(void);
#endif
#if defined(__BT_FMPL_PROFILE__) || defined(__BT_FMPT_PROFILE__)
extern void FMP_Init(void);
extern void fmp_adp_init(void);
#endif
#if defined(__BT_GATTC_PROFILE__)
extern void GATTC_Init(void);
#endif
#if defined(__BT_GATTS_PROFILE__)
extern void GATTS_Init(void);
#endif
#if __BT_GOEP_PROFILE__
void goep_deinit(void);
#endif
/*****************************************************************************
 * FUNCTION
 *  bt_adp_profiles_init
 * DESCRIPTION
 *  This is for profiles and adaptor init funciton
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_adp_profiles_init(void)
{
    /* Adaptor and profile init */
#ifdef __BT_A2DP_PROFILE__
    A2DP_Init();
    a2dp_adp_init();
#endif

#ifdef __BT_AVRCP_PROFILE__
    AVRCP_Init();
    bt_adp_avrcp_init();
    AVRCP_UtilInit();
#endif 


#ifdef __BT_HFG_PROFILE__
	hfg_adp_init();
#endif

	btmtk_ashm_start_service();

#ifdef __BT_SPP_PROFILE__
	spp_adp_init();
#endif

#ifdef __BT_GOEP_PROFILE__        
        // PROTOCOL init
    OBEX_Init();
    GOEP_Init();
#endif    

#ifdef __BT_OPP_PROFILE__ 
    opps_adp_init();
    oppc_adp_init();
#endif
#if defined(__BT_FTS_PROFILE__)
    ftps_adp_init();
#endif
#if defined(__BT_FTC_PROFILE__)
    ftpc_adp_init();
#endif 
#if defined(__BT_TESTING_PROFILE__)
	chn_adp_init();
#endif

#if defined(__BT_PRXR_PROFILE__) || defined(__BT_PRXM_PROFILE__)
	prx_adp_init();
#endif

#ifdef __BT_BPP_PROFILE__
    bpp_init();
    bpp_adp_init();
#endif 
#if defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__)
    HID_Init();
#endif
#if defined(__BT_HIDD_PROFILE__)
    hidda_init();
#endif
#if defined(__BT_HIDH_PROFILE__)
    hidha_init();
#endif
#ifdef __BT_SIM_PROFILE__
	simap_adp_init();
#endif

#if defined __BT_BIPI_PROFILE__ || defined __BT_BIPR_PROFILE
    bip_adp_init();
#endif
#if defined(__BT_PBAP_PROFILE__)
	pbap_adp_init();
#endif
#if defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__)
    map_adp_init();
#endif
#if defined(__BT_JSR82__)
#if (BT_JSR82_ENABLED == 1) || defined(__BT_JSR82_L2RF__)
	jsr82_adp_init();
#endif
#endif /* __BT_JSR82__ */
#if (__BT_PAN_PROFILE__)
    PAN_Init();
    pan_adp_init();
#endif
#ifdef __BT_3_0_HS__
    check_pal_struct();
#endif
#if defined __BT_PRX_PROFILE__ 
    prx_adp_init();
#endif
#if defined(__BT_TIMEC_PROFILE__) || defined(__BT_TIMES_PROFILE__)
	time_adp_init();
#endif
#if defined(__BT_HDP_PROFILE__)
	hdp_adp_init();
#endif
#if defined(__BT_HTPC_PROFILE__) || defined(__BT_HTPT_PROFILE__)
    HTP_Init();
#endif
#if defined(__BT_HTPC_PROFILE__)
    htpc_adp_init();
#endif
#if defined(__BT_HTPT_PROFILE__)
    htpt_adp_init();
#endif
#if defined(__BT_FMPL_PROFILE__) || defined(__BT_FMPT_PROFILE__)
    FMP_Init();
    fmp_adp_init();
#endif
#if defined(__BT_GATTC_PROFILE__)
    GATTC_Init();
#endif
#if defined(__BT_GATTS_PROFILE__)
        GATTS_Init();
#endif
    return;
}


/*****************************************************************************
 * FUNCTION
 *  bt_adp_profiles_deinit
 * DESCRIPTION
 *  This is for profiles and adaptor init funciton
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bt_adp_profiles_deinit(void)
{
#if defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__)
//    map_adp_deinit();
#endif
#ifdef __BT_SIM_PROFILE__
	simap_adp_deinit();
#endif

#if __BT_GOEP_PROFILE__
    goep_deinit();
#endif
#if defined(__BT_HDP_PROFILE__)
	hdp_adp_deinit();
#endif

#if defined(__BT_JSR82__)
#if (BT_JSR82_ENABLED == 1) || defined(__BT_JSR82_L2RF__)
	jsr82_adp_deinit();
	OS_Report("jsr82_adp_deinit");
#endif
#endif /* __BT_JSR82__ */
    return;
}


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
#ifdef __BT_GAP_PROFILE__
    bm_adp_deinit();
#endif
#ifdef __BT_SIM_PROFILE__
	simap_adp_reset();
#endif

#if defined(__BT_PRXR_PROFILE__) 
	prxr_adp_reset();
#endif
#if defined(__BT_PRXM_PROFILE__)
	prxm_adp_reset();
#endif


    return;
}

#endif
