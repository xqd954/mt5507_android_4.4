/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bt_adp_msg.h"
#include "bt_adp_system.h"

#ifdef __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__
extern void CMGR_SetDualPCMSCOAcceptValue(U8 value);
extern void MeWriteDualPCMSetValue(U8 *value, U16 len);
extern void MeWriteDualPCMSwitch(U8 *value, U16 len);

void BTSetSCOAcceptSettingCnf(void)
{
    bt_set_sco_accept_cnf_struct *cnf_p = NULL;
    cnf_p = construct_local_para(sizeof(bt_set_sco_accept_cnf_struct), TD_UL);
    cnf_p->value = 0;
    BT_SendMessage(MSG_ID_BT_SET_SCO_ACCEPT_CNF, MOD_MMI, cnf_p, sizeof(bt_set_sco_accept_cnf_struct));
}

void BTSetDualPCMSettingCnf(void)
{
    BT_SendMessage(MSG_ID_BT_SET_DUAL_PCM_SETTING_CNF, MOD_MMI, NULL, 0);
}

void BTSetDualPCMSwitchCnf(void)
{
    BT_SendMessage(MSG_ID_BT_SET_DUAL_PCM_SWITCH_CNF, MOD_MMI, NULL, 0);
}

void BTSetSCOAcceptSetting(U8 setting)
{
    CMGR_SetDualPCMSCOAcceptValue(setting);
    BTSetSCOAcceptSettingCnf();
}

void BTSetDualPCMSetting(U8 *value, U16 len)
{
    MeWriteDualPCMSetValue(value, len);
}

void BTSetDualPCMSwitch(U8 *value, U16 len)
{
    MeWriteDualPCMSwitch(value, len);
}
#endif

