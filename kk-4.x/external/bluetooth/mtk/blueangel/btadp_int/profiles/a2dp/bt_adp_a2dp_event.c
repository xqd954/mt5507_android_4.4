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
*  permission of MediaTek Inc. (C) 2006
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
 * bt_adp_a2dp_event.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions which provide 
 *
 * Author:
 * -------
 *  Tina Shen
 *      Create 2006/2/9
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/


//#include "stdio.h"      /* Basic C I/O functions */
#include "bt_common.h"
#include "a2dp.h"

/***************
Internal Globals 
****************/

/***************
External Globals 
****************/
extern void BTA2DPAdpActivateCfm(U16 result);
extern void BTA2DPAdpDeactivateCfm(U16 result);
extern void BTA2DPAdpConnectCfm(U16 connect_id, U16 result, bt_addr_struct *device_addr);
extern void BTA2DPAdpConnectInd( 
                    bt_addr_struct *device_addr,
                    U8 device_name_size,
                    U8 *device_name);
extern void BTA2DPAdpDisconnectCfm(U16 connect_id, U16 result);
extern void BTA2DPAdpDisconnectInd(U16 connect_id);
extern void BTA2DPAdpDiscoverySEIDCfm(U16 connect_id, U16 result, U8 sep_num, bt_sep_info_struct *sep_list);
extern void BTA2DPAdpDiscoverySEIDInd(U16 connect_id);
extern void BTA2DPAdpGetCapabilityResultCfm(U16 connect_id, A2dpCodec *codec);
extern void BTA2DPAdpGetCapabilityCfm(U16 connect_id, U16 result);
extern void BTA2DPAdpGetCapabilityInd(U16 connect_id, U8 acp_seid);
extern void BTA2DPAdpSetConfigurationCfm(U16 connect_id, U16 result, U8 stream_handle);
extern void BTA2DPAdpSetConfigurationInd(U16 connect_id, U8 acp_seid, U8 int_seid, U8 stream_handle, A2dpCodec *audio_cap);
extern void BTA2DPAdpSetReconfigurationCfm(U16 result, U8 stream_handle);
extern void BTA2DPAdpSetReconfigurationInd(U8 stream_handle, A2dpCodec *audio_cap);
extern void BTA2DPAdpStreamOpenCfm(U16 result, U8 stream_handle, bt_addr_struct *device_addr, U8 *dev_name, U8 name_len);
extern void BTA2DPAdpStreamOpenInd(U8 stream_handle, bt_addr_struct *device_addr, U8 *dev_name, U8 name_len);
extern void BTA2DPAdpStreamStartCfm(U16 result, U8 stream_handle, U16 prefer_size, bt_a2dp_audio_cap_struct *config);
extern void BTA2DPAdpStreamStartInd(U8 stream_handle, U16 prefer_size, bt_a2dp_audio_cap_struct *config);
extern void BTA2DPAdpStreamSuspendCfm(U16 result, U8 stream_handle);
extern void BTA2DPAdpStreamSuspendInd(U8 stream_handle);
extern void BTA2DPAdpStreamCloseCfm(U16 result, U8 stream_handle);
extern void BTA2DPAdpStreamCloseInd(U8 stream_handle);
extern void BTA2DPAdpStreamAbortCfm(U8 stream_handle);
extern void BTA2DPAdpStreamAbortInd(U8 stream_handle);
extern void BTA2DPAdpStreamQoSInd(U8 stream_handle, U8 qos);
extern void BTA2DPAdpStreamDataOut(U8 stream_handle);
extern void BTA2DPAdpStreamDataConfigCnf(bt_a2dp_audio_cap_struct *audio_cap);

#ifdef MTKA2DP_WISE
#ifndef BTMTK_ON_WISE
void BTA2DPADPStartDummySendStream(void);
extern U8 MBT_SDK_Streamming_Package_Ongoing;
extern EvmTimer a2dp_send_stream_timer;
#endif
#endif
extern void BTA2DPAdpStreamDataConfigChange(bt_a2dp_audio_cap_struct *audio_cap);


/***************************
Internal function prototypes
****************************/

void a2dp_app_callback(BTA2DPCallbackParms *Parms);


/*********************
Function Definitions 
**********************/

void a2dp_adp_event_init(void)
{
    A2DP_Register(a2dp_app_callback);
}

void a2dp_app_callback(BTA2DPCallbackParms *Parms)
{
    switch (Parms->event)
    {
        case BTA2DP_EVENT_ACTIVATE_CNF:
            BTA2DPAdpActivateCfm(Parms->result);
            break;
        case BTA2DP_EVENT_DEACTIVATE_CNF:
            BTA2DPAdpDeactivateCfm(Parms->result);
            break;
        case BTA2DP_EVENT_SET_RECONFIGURATION_CNF:
            BTA2DPAdpSetReconfigurationCfm(Parms->result, Parms->p.A2DPEventSetReConfigurationCnf.stream_handle);
            break;
        case BTA2DP_EVENT_SET_RECONFIGURATION_IND:
            BTA2DPAdpSetReconfigurationInd(Parms->p.A2DPEventSetReConfigurationInd.stream_handle, Parms->p.A2DPEventSetReConfigurationInd.audio_cap);
            break;
        case BTA2DP_EVENT_STREAM_OPEN_CNF:
            BTA2DPAdpStreamOpenCfm(Parms->result, 
                Parms->p.A2DPEventOpenStreamCnf.stream_handle,
                Parms->p.A2DPEventOpenStreamCnf.device_addr,
                Parms->p.A2DPEventOpenStreamCnf.device_name,
                Parms->p.A2DPEventOpenStreamCnf.device_name_len);
            break;
        case BTA2DP_EVENT_STREAM_OPEN_IND:
            BTA2DPAdpStreamOpenInd(Parms->p.A2DPEventOpenStreamInd.stream_handle,
                Parms->p.A2DPEventOpenStreamInd.device_addr,
                Parms->p.A2DPEventOpenStreamInd.device_name,
                Parms->p.A2DPEventOpenStreamInd.device_name_len);
            break;
        case BTA2DP_EVENT_STREAM_START_CNF:
            BTA2DPAdpStreamStartCfm(Parms->result, Parms->p.A2DPEventStartStreamCnf.stream_handle, Parms->p.A2DPEventStartStreamCnf.prefer_size, &Parms->p.A2DPEventStartStreamCnf.config);
            break;
        case BTA2DP_EVENT_STREAM_START_IND:
            BTA2DPAdpStreamStartInd(Parms->p.A2DPEventStartStreamInd.stream_handle, Parms->p.A2DPEventStartStreamInd.prefer_size, &Parms->p.A2DPEventStartStreamInd.config);
            break;
        case BTA2DP_EVENT_STREAM_SUSPEND_CNF:
            BTA2DPAdpStreamSuspendCfm(Parms->result, Parms->p.A2DPEventSuspendStreamCnf.stream_handle);
            break;
        case BTA2DP_EVENT_STREAM_SUSPEND_IND:
            BTA2DPAdpStreamSuspendInd(Parms->p.A2DPEventSuspendStreamInd.stream_handle);
            break;
        case BTA2DP_EVENT_STREAM_CLOSE_CNF:
            BTA2DPAdpStreamCloseCfm(Parms->result, Parms->p.A2DPEventCloseStreamCnf.stream_handle);
            break;
        case BTA2DP_EVENT_STREAM_CLOSE_IND:
            BTA2DPAdpStreamCloseInd(Parms->p.A2DPEventCloseStreamInd.stream_handle);
            break;
        case BTA2DP_EVENT_STREAM_ABORT_CNF:
            BTA2DPAdpStreamAbortCfm(Parms->p.A2DPEventAbortStreamInd.stream_handle);
            break;
        case BTA2DP_EVENT_STREAM_ABORT_IND:
            BTA2DPAdpStreamAbortInd(Parms->p.A2DPEventAbortStreamInd.stream_handle);
            break;
        case BTA2DP_EVENT_STREAM_QOS_IND:
            BTA2DPAdpStreamQoSInd(Parms->p.A2DPEventQualityOfServiceInd.stream_handle, Parms->p.A2DPEventQualityOfServiceInd.qos);
            break;

        case BTA2DP_EVENT_STREAM_DATA_CONFIG_CHANGE:
            BTA2DPAdpStreamDataConfigChange(Parms->p.A2DPEventStreamDataConfigChange.audio_cap);
            break;
        case BTA2DP_EVENT_STREAM_DATA_CONFIG_CNF:
            //BTA2DPAdpStreamDataConfigCnf(Parms->p.A2DPEventStreamDataConfigCnf.audio_cap);
            break;
    }
}



