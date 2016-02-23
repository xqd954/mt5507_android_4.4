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

#include "btconfig.h"
#ifdef __SDAP_TESTER_
#include "sec_adp.h"
#include "bluetooth_gap_struct.h"
#include "conmgr.h"

#define SDAP_TYPE_SERVICE_SEARCH 0x01
#define SDAP_TYPE_SERVICE_SEARCH_ATTRIBUTE 0x02

typedef struct _BtSdapTesterContext
{
    SdpQueryToken sdpQueryToken;
    CmgrHandler cmgrHandler;
    U8 search_stage;
    U8 search_type;
    BD_ADDR addr;
} BtSdapTesterContext;

void sdapTesterCallback(ilm_struct *ilm_ptr);
void sdapTesterInit(void);
void sdapTesterGeneralInquiry(void);
void sdapTesterLimitedInquiry(void);
void sdapTesterNameDiscovery(void);
void sdapTesterCreateAclLink(btbm_bd_addr_t bd_addr);
void sdapTesterCreateAclLinkWithPIN(btbm_bd_addr_t bd_addr);
void sdapTesterCheckAclLinkState(btbm_bd_addr_t bd_addr);
void sdapTesterCloseChannel(btbm_bd_addr_t bd_addr);
void sdapTesterServiceSearchUnConnected(btbm_bd_addr_t bd_addr);
void sdapTesterServiceSearchConnectedAttributeUnConnected(btbm_bd_addr_t bd_addr);
void sdapTesterServiceSearchConnected(btbm_bd_addr_t bd_addr);
void sdapTesterServiceSearchConnectedAttributeConnected(btbm_bd_addr_t bd_addr);
void sdapTesterServiceSearchConnectedUtility(void);
void sdapTesterCmgrCallback(CmgrHandler *cHandler, CmgrEvent Event, BtStatus Status);
void sdapTesterQueryCallback(SdpQueryToken *sqt, U8 result, U8 serv_chn);
void sdaptester_handler(ilm_struct *ilm_ptr);
void sdap_tester_handler_inject_msg(char *string);

#endif /* __SDAP_TESTER_ */ 

