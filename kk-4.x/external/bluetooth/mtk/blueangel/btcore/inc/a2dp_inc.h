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

void A2DP_SignalConnectReq(bt_addr_struct *device_addr);
void A2DP_SignalConnectReq(bt_addr_struct *device_addr);
void A2DP_SignalDisconnectReq(U16 connect_id);
void A2DP_StreamDiscoverReq(U16 connect_id);
void A2DP_StreamDiscoverRes(
        U16 connect_id,
        U16 result,
        U8 sep_num,
        bt_sep_info_struct *sep_list);
void A2DP_StreamGetCapReq(U16 connect_id, U8 acp_seid);
void A2DP_StreamGetCapRes(
        U16 connect_id,
        U16 result,
        bt_a2dp_audio_cap_struct *audio_cap_list);
void A2DP_StreamSetConfigReq(
        U16 connect_id,
        U8 acp_seid,
        U8 int_seid,
        bt_a2dp_audio_cap_struct *audio_cap);
void A2DP_StreamSetConfigRes(U16 result, U8 stream_handle);
void A2DP_StreamOpenReq(bt_addr_struct *device_addr);
void A2DP_StreamOpenRes(U16 result, U8 stream_handle);
void A2DP_StreamReconfigReq(U8 stream_handle, bt_a2dp_audio_cap_struct *audio_cap);
void A2DP_StreamReconfigRes(U16 result, U8 stream_handle);

void A2DP_StreamStartReq(U8 stream_handle);
void A2DP_StreamStartRes(U16 result, U8 stream_handle);
void A2DP_StreamSuspendReq(U8 stream_handle);
void A2DP_StreamSuspendRes(U16 result, U8 stream_handle);
void A2DP_StreamCloseReq(U8 stream_handle);
void A2DP_StreamCloseRes(U16 result, U8 stream_handle);
void A2DP_StreamAbortReq(U8 stream_handle);
void A2DP_StreamAbortRes(U8 stream_handle);
void A2DP_StreamDataConfigGet(void);
void A2DP_SendStreamData(U8 stream_handle, U8 *data, U32 len, U32 sample_count);
BT_A2DP_RESULT_ENUM A2DP_StreamSendDataReq(U8 stream_handle, U8 *buffer, U32 buffer_size, U32 sample_count);
void A2DP_Activate(U8 local_role, U8 sep_num, bt_a2dp_audio_cap_struct *local_sep_list);
void A2DP_Deactivate(void);
U16 A2DP_GetPreferredPayloadSize(U8 stream_handle);
BT_A2DP_RESULT_ENUM A2DP_GetStreamBuffer(U8 stream_handle, U32 buffer_size, U8 **buffer);
#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER
U8 A2dpGetState(void);
void A2DP_FMContrllerStartReq(U8 stream_handle);
void A2DP_FMContrllerStopReq(U8 stream_handle);
void A2DP_FMContrllerSuspendReq(U8 stream_handle);
void A2DP_FMContrllerResumeReq(U8 stream_handle);
#endif



