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
 * Bt_maui_btbm.h
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
void bm_adp_init(void);

void BTBMAdpGetReadVisibleModeCfm(U8 mode);
void BTBMAdpReadLocalDeviceClassCfm(U8 result, U32 cod);
void BTBMAdpReadLocalDeviceUuidListCfm(kal_uint8 result, kal_uint32 *uuid_list);
void BTBMAdpWriteLocalDeviceClassCfm(kal_uint8 result);
void BTBMAdpReadLocalNameCfm(kal_uint8 *name, kal_uint8 length);
void BTBMAdpReadLocalBDAddressCfm(kal_uint8 result, btbm_bd_addr_t bd_addr);
void BTBMAdpGapDiscoveryCompCfm(kal_uint8 result, kal_uint8 total_number);
void BTBMAdpWriteScanEnabledCfm(kal_uint8 result);
void BTBMAdpGapDiscoveryCancelCfm(kal_uint8 result, kal_uint8 total_number);
void BTBMGapDiscoveryResultCfm(BOOL is_update,
                               btbm_bd_addr_t bd_addr,
#ifdef __BT_4_0_BLE__
                               btbm_device_type device_type,
#endif
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
                                  kal_uint8 rssi, kal_uint8 eir_supported, kal_uint8 uuid_list_supported, kal_uint8 servlist_completed,
                                  kal_uint32 *uuid_list,
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
                                  kal_uint32 cod, kal_uint8 name_len, kal_uint8 *name);

void BTBMAdpWriteLocalNameCfm(kal_uint8 result);
void BTBMAdpGapSetAuthenticationCfm(kal_uint8 result);
void BTBMAdpDeleteTrustCfm(kal_uint8 result, btbm_bd_addr_t bd_addr);
void BTBMAdpDeleteTrustAllCfm(kal_uint8 result);
void BTBMAdpAllowAclLinkResult(void);
void BTBMAdpDisallowAclLinkResult(void);
void BTBMAdpBondingDeviceCfm(kal_uint8 result, btbm_bd_addr_t bd_addr);
void BTBMAdpBondingCancelCfm(kal_uint8 result, btbm_bd_addr_t bd_addr);
void BTBMAdpBlockDeviceDisconnectLinkCfm(void);
void BTBMAdpBlockListUpdateCfm(void);
void BTBMAdpBondingResultInd(kal_uint8 result, kal_uint8 type, btbm_bd_addr_t bd_addr, kal_uint32 cod);
void BTBMAdpReportLinkState(kal_uint8 active_no, btbm_bd_addr_t bd_addr);
void BTBMAdpReportScatternetState(kal_uint8 piconet, kal_uint8 scatternet, kal_uint8 sco, kal_uint8 esco);
void BTBMAdpLinkKeyGeneratedInd(
        kal_uint8 result, 
        kal_uint8 type, 
        btbm_bd_addr_t bd_addr, 
#ifdef __BT_4_0_BLE__
        btbm_device_type device_type,
#endif
        kal_uint32 cod, 
        kal_uint8 key_type,
        kal_uint8 *key);

void BTBMAdpPinCodeRequiredInd(
        U8 pin_code_16_digit_required,
        btbm_bd_addr_t bd_addr,
        U8 name_len,
        U8 *name,
        kal_uint32 cod);

void BTBMAdpReadRemoteNameCfm(
        kal_uint8 result,
        btbm_bd_addr_t bd_addr,
        kal_uint8 name_len,
        kal_uint8 *name,
        kal_uint32 cod);

void BTBMAdpReadRemoteNameCancelCfm(kal_uint8 result);
        
void BTBMAdpSSPNumericInd(
        btbm_bd_addr_t bd_addr,
        kal_uint8 ssp_type,
        kal_int32 numericValue,
        kal_uint8 name_len,
        kal_uint8 *name,
        kal_bool displayNumeric,
        kal_uint32 cod);

void BTBMAdpRemoteNameInd(
        btbm_bd_addr_t bd_addr,
        kal_uint8 name_len,
        kal_uint8 *name);

void BTBMAdpSSPPasskeyPressInd(btbm_bd_addr_t bd_addr, kal_uint8 event);

void BTBMAdpConnectAcceptInd(btbm_bd_addr_t addr);

void BTBMAdpReportNetworkState(kal_uint8 result, kal_bool is_master, kal_bool is_authenticated, kal_bool is_encrypted);

void BTBMAdpReportReadRSSIResult(U8 errCode, U8 rssi_value, U32 lap, U8 uap, U16 nap);

void BTBMAdpServiceSearchResult(kal_uint32 uuid, btbm_bd_addr_t addr);

void BTBMAdpServiceSearchComplete(kal_uint8 result, btbm_bd_addr_t addr, kal_uint8 no);

void BTBMAdpServiceSearchCancel(kal_uint8 result, btbm_bd_addr_t addr, kal_uint8 no);

void BTBMAdpAttributeSearchResult(kal_uint16 len, kal_uint8 *data, btbm_bd_addr_t addr);

#if defined(BTMTK_ON_LINUX)
void BTBMAdpAttributeSearchComplete(kal_uint8 result, btbm_bd_addr_t addr, kal_uint16 len, kal_uint8 *data);
#else
void BTBMAdpAttributeSearchComplete(kal_uint8 result, btbm_bd_addr_t addr);
#endif


void BTBMAdpGetLinkKeyResult(kal_uint8 result, 
                                                                btbm_bd_addr_t bd_addr, 
                                                                btbm_device_type device_type, 
                                                                kal_uint32 cod,
                                                                kal_uint8 key_type,
                                                                kal_uint8 *key,
                                                                kal_uint8 persist);

typedef struct _bm_adp_context
{
    module_type src_module;
    module_type direct_rsp_module;
    module_type ss_raw_module;
    BOOL operation_ongoing;
    U32 last_cmd;
    gap_tester callback;
    BOOL bt_panic;
#ifdef MTK_BTBM_TIMER_CHECK
    EvmTimer bmaTimer;
#endif /* MTK_BTBM_TIMER_CHECK */ 
} bm_adp_context;


