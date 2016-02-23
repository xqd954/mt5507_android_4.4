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

#ifndef __A2MP_CTL_H__
#define __A2MP_CTL_H__

#include "btconfig.h"
#include "a2mp.h"
#include "l2cap.h"
#include "eventmgr.h"
#include "a2mp_adp.h"

#if A2MP_PROTOCOL == XA_ENABLED
void BTA2MP_RecvDataInd(BtRemoteDevice *link, U8 *data, U16 data_len);
BtStatus BTA2MP_SendDiscoveryRequest(BtRemoteDevice *link, BtA2MPDiscoveryReq *a2mp_parm);
void BTA2MP_PhysicalLinkClose(BtRemoteDevice *link);
BtStatus BTA2MP_SendCreatePhysicalLinkRequest(BtRemoteDevice *link, BtA2MPCreatePhysicalLinkReq *a2mp_parm);
BtStatus BTA2MP_SendGetAMPAssocResponse(BtRemoteDevice *link, BtA2MPGetAMPAssocRsp *a2mp_parm);
BtStatus BTA2MP_SendGetInfoResponse(BtRemoteDevice *link, BtA2MPGetInfoRsp *a2mp_parm);
BtStatus BTA2MP_SendChangeNotify(BtRemoteDevice *link, BtA2MPChangeNotify *a2mp_parm);
void BtA2MP_HandlePhysicalChannelCreatedDuringCreateChannel(A2MP_MAIN_CONN *a2mp_main_channel);

#endif /* A2MP_PROTOCOL == XA_ENABLED */
#endif /* __A2MP_CTL_H__ */ 

