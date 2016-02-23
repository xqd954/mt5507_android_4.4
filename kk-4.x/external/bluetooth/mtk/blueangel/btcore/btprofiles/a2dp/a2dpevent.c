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
#ifdef __BT_A2DP_PROFILE__
#ifndef __BT_A2DP_EVENT_C
#define __BT_A2DP_EVENT_C

#include "bttypes.h"
#include "bluetooth_a2dp_struct.h"
#include "bt_a2dp_util.h"
#include "bt_adp_a2dp.h"
#include "a2dp.h"
#include "avrcp.h"

extern bt_a2dp_ctx_struct bt_a2dp_ctx;

#define A2DPAdpCallbackFunc(f) bt_a2dp_ctx.a2dpCBfunc(f)

/* AVRCP avrcp.h */
extern void AVRCP_HandleA2DPInfo(U32 info, U8 *data);


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleActivateCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleActivateCnf(U16 result)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_ACTIVATE_CNF;
    parms.result = result;
    parms.p.A2DPEventActivateCnf.result = result;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  A2dpHandleDeactivateCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleDeactivateCnf(U16 result)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_DEACTIVATE_CNF;
    parms.result = result;
    parms.p.A2DPEventDeactivateCnf.result = result;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
    return;
}

#if 0
/*****************************************************************************
 * FUNCTION
 *  A2dpHandleConnectCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  connect_id      [IN]        
 *  result          [IN]        
 *  device_addr     [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleConnectCnf(
        U16 connect_id,
        U16 result,
        bt_addr_struct *device_addr)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_CONNECT_CNF;
    parms.result = result;
    parms.p.A2DPEventSignalConnectCnf.connect_id = connect_id;
    parms.p.A2DPEventSignalConnectCnf.device_addr = device_addr;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleConnectInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  connect_id      [IN]        
 *  device_addr     [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleConnectInd(bt_addr_struct *device_addr, U8 size, U8 *name)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_CONNECT_IND;
    parms.result = BT_A2DP_RESULT_OK;
    parms.p.A2DPEventSignalConnectInd.device_addr = device_addr;
    if(size !=0)
        parms.p.A2DPEventSignalConnectInd.device_name = name;
    else
        parms.p.A2DPEventSignalConnectInd.device_name = 0;
    parms.p.A2DPEventSignalConnectInd.device_name_size = size;    
    bt_a2dp_ctx.a2dpCBfunc(&parms);
    return;
    
}

/*****************************************************************************
 * FUNCTION
 *  A2dpHandleDisconnectCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  connect_id      [IN]        
 *  result          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleDisconnectCnf(U16 connect_id, U16 result)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_DISCONNECT_CNF;
    parms.result = result;
    parms.p.A2DPEventSignalDisconnectCnf.connect_id = connect_id;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleDisconnectInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  connect_id      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleDisconnectInd(U16 connect_id)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_DISCONNECT_IND;
    parms.result = BT_A2DP_RESULT_OK;
    parms.p.A2DPEventSignalDisconnectInd.connect_id = connect_id;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleSepDiscoverCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  connect_id      [IN]        
 *  result          [IN]        
 *  sep_num         [IN]        
 *  sep_list        [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleSepDiscoverCnf(
        U16 connect_id,
        U16 result,
        U8 sep_num,
        bt_sep_info_struct *sep_list)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_DISCOVERY_SEID_CNF;
    parms.result = result;
    parms.p.A2DPEventSepDiscoveryCnf.connect_id = connect_id;
    parms.p.A2DPEventSepDiscoveryCnf.sep_num = sep_num;
    parms.p.A2DPEventSepDiscoveryCnf.sep_list = sep_list;    
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleSepDiscoverInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  connect_id      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleSepDiscoverInd(U16 connect_id)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_DISCOVERY_SEID_IND;
    parms.result = BT_A2DP_RESULT_OK;
    parms.p.A2DPEventSepDiscoveryInd.connect_id = connect_id;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}

/*****************************************************************************
 * FUNCTION
 *  A2dpHandleCapability
 * DESCRIPTION
 *  
 * PARAMETERS
 *  connect_id          [IN]               
 *  avdtp_codec         [IN]         
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleCapability(
        U16 connect_id,
        AvdtpCodec *codec)
{
    BTA2DPCallbackParms parms;
    
    parms.event = BTA2DP_EVENT_CAPABILITY;
    parms.p.A2DPEventCapability.connect_id = connect_id;
    parms.p.A2DPEventCapability.audio_cap = (A2dpCodec *)codec;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}

/*****************************************************************************
 * FUNCTION
 *  A2dpHandleCapabilitiesGetCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  connect_id          [IN]        
 *  result              [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleCapabilitiesGetCnf(
        U16 connect_id,
        U16 result)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_GET_CAPABILITY_CNF;
    parms.result = result;
    parms.p.A2DPEventGetCapabilitiesCnf.connect_id = connect_id;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleCapabilitiesGetInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  connect_id      [IN]        
 *  acp_seid        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleCapabilitiesGetInd(U16 connect_id, U8 acp_seid)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_GET_CAPABILITY_IND;
    parms.p.A2DPEventGetCapabilitiesInd.connect_id = connect_id;
    parms.p.A2DPEventGetCapabilitiesInd.acp_seid = acp_seid;
    bt_a2dp_ctx.a2dpCBfunc(&parms);

}

/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamConfigCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  connect_id          [IN]        
 *  result              [IN]        
 *  stream_handle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamConfigCnf(
        U16 connect_id,
        U16 result,
        U8 stream_handle)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_SET_CONFIGURATION_CNF;
    parms.result = result;
    parms.p.A2DPEventSetConfigurationCnf.connect_id = connect_id;
    parms.p.A2DPEventSetConfigurationCnf.stream_handle = stream_handle;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamConfigInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  connect_id          [IN]        
 *  acp_seid            [IN]        
 *  int_seid            [IN]        
 *  stream_handle       [IN]        
 *  audio_cap           [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamConfigInd(
        U16 connect_id,
        U8 acp_seid,
        U8 int_seid,
        U8 stream_handle,
        A2dpCodec *audio_cap)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_SET_CONFIGURATION_IND;
    parms.p.A2DPEventSetConfigurationInd.connect_id = connect_id;
    parms.p.A2DPEventSetConfigurationInd.acp_seid = acp_seid;
    parms.p.A2DPEventSetConfigurationInd.int_seid = int_seid;
    parms.p.A2DPEventSetConfigurationInd.stream_handle = stream_handle;
    parms.p.A2DPEventSetConfigurationInd.audio_cap = audio_cap;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
    return;

}
#endif

/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamReconfigCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result              [IN]        
 *  stream_handle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamReconfigCnf(
                            U16 result, 
                            U8 stream_handle)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_SET_RECONFIGURATION_CNF;
    parms.result = result;
    parms.p.A2DPEventSetReConfigurationCnf.stream_handle = stream_handle;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamReconfigInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  stream_handle       [IN]        
 *  audio_cap           [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamReconfigInd(
        U8 stream_handle,
        A2dpCodec *audio_cap)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_SET_RECONFIGURATION_IND;
    parms.p.A2DPEventSetReConfigurationInd.stream_handle = stream_handle;
    parms.p.A2DPEventSetReConfigurationInd.audio_cap = audio_cap;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamOpenCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result              [IN]        
 *  stream_handle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamOpenCnf(U16 result, 
    U8 stream_handle, 
    bt_addr_struct *device_addr, 
    U8 *dev_name, U8 name_len)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_OPEN_CNF;
    parms.result = result;
    parms.p.A2DPEventOpenStreamCnf.stream_handle = stream_handle;
    parms.p.A2DPEventOpenStreamCnf.device_addr = device_addr;

    if (name_len >= 60) name_len = 59;
    
    if (name_len > 0 && dev_name != NULL) {
        memcpy(parms.p.A2DPEventOpenStreamCnf.device_name, dev_name, name_len);
        parms.p.A2DPEventOpenStreamCnf.device_name[name_len] = '\0';
        parms.p.A2DPEventOpenStreamCnf.device_name_len = name_len;
    }
    else {
        memcpy(parms.p.A2DPEventOpenStreamCnf.device_name, "Unknown", 7);
        parms.p.A2DPEventOpenStreamCnf.device_name[7] = '\0';
        parms.p.A2DPEventOpenStreamCnf.device_name_len = 7;
    }
    
    bt_a2dp_ctx.a2dpCBfunc(&parms);

    // Pass to AVRCP
#ifdef __BT_AVRCP_PROFILE__    
    if( parms.result == BT_A2DP_RESULT_OK ){
        AVRCP_HandleA2DPInfo(0, (U8*)device_addr); // 0 is connected
    }
#endif
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamOpenInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  stream_handle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamOpenInd(U8 stream_handle,
    bt_addr_struct *device_addr, 
    U8 *dev_name, U8 name_len)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_OPEN_IND;
    parms.p.A2DPEventOpenStreamInd.stream_handle = stream_handle;
    parms.p.A2DPEventOpenStreamInd.device_addr = device_addr;

    if (name_len >= 60) name_len = 59;
    
    if (name_len > 0 && dev_name != NULL) {
        memcpy(parms.p.A2DPEventOpenStreamInd.device_name, dev_name, name_len);
        parms.p.A2DPEventOpenStreamInd.device_name[name_len] = '\0';
        parms.p.A2DPEventOpenStreamInd.device_name_len = name_len;
    }
    else {
        memcpy(parms.p.A2DPEventOpenStreamInd.device_name, "Unknown", 7);
        parms.p.A2DPEventOpenStreamInd.device_name[7] = '\0';
        parms.p.A2DPEventOpenStreamInd.device_name_len = 7;
    }
    bt_a2dp_ctx.a2dpCBfunc(&parms);
    
    // Pass to AVRCP. 0 is connected
#ifdef __BT_AVRCP_PROFILE__    
    AVRCP_HandleA2DPInfo(0, (U8*)device_addr);
#endif
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamStartCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result              [IN]        
 *  stream_handle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamStartCnf(U16 result, U8 stream_handle, U16 prefer_size, bt_a2dp_audio_cap_struct *config)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_START_CNF;
    parms.result = result;
    parms.p.A2DPEventStartStreamCnf.stream_handle = stream_handle;
    parms.p.A2DPEventStartStreamCnf.prefer_size = prefer_size;

    if (config)
        memcpy(&parms.p.A2DPEventStartStreamCnf.config, config, sizeof(bt_a2dp_audio_cap_struct));
    
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamStartInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  stream_handle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamStartInd(U8 stream_handle, U16 prefer_size, bt_a2dp_audio_cap_struct *config)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_START_IND;
    parms.p.A2DPEventStartStreamInd.stream_handle = stream_handle;
    parms.p.A2DPEventStartStreamInd.prefer_size = prefer_size;
    memcpy(&parms.p.A2DPEventStartStreamInd.config, config, sizeof(bt_a2dp_audio_cap_struct));
    
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}
#ifndef BTMTK_ON_WISE
/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamStartInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  stream_handle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamStartIndCfm(void)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_START_IND_CNF;
    
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}
#endif

/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamSuspendCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result              [IN]        
 *  stream_handle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamSuspendCnf(U16 result, U8 stream_handle)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_SUSPEND_CNF;
    parms.result = result;
    parms.p.A2DPEventSuspendStreamCnf.stream_handle = stream_handle;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamSuspendInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  stream_handle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamSuspendInd(U8 stream_handle)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_SUSPEND_IND;
    parms.p.A2DPEventSuspendStreamInd.stream_handle = stream_handle;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}



/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamQosInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  stream_handle       [IN]        
 *  qos                 [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamQosInd(U8 stream_handle, U8 qos)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_QOS_IND;
    parms.p.A2DPEventQualityOfServiceInd.stream_handle = stream_handle;
    parms.p.A2DPEventQualityOfServiceInd.qos = qos;    
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamCloseCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result              [IN]        
 *  stream_handle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamCloseCnf(U16 result, U8 stream_handle)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_CLOSE_CNF;
    parms.result = result;
    parms.p.A2DPEventCloseStreamCnf.stream_handle = stream_handle;
    bt_a2dp_ctx.a2dpCBfunc(&parms);

    // pass to AVRCP. 1 is disconnected
#ifdef __BT_AVRCP_PROFILE__    
    if( parms.result == BT_A2DP_RESULT_OK ){
        AVRCP_HandleA2DPInfo(1, 0);
    }
#endif
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamCloseInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  stream_handle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamCloseInd(U8 stream_handle)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_CLOSE_IND;
    parms.p.A2DPEventCloseStreamCnf.stream_handle = stream_handle;
    bt_a2dp_ctx.a2dpCBfunc(&parms);

    // pass to AVRCP. 1 is disconnected
#ifdef __BT_AVRCP_PROFILE__
    AVRCP_HandleA2DPInfo(1, 0);
#endif
}



/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamAbortCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  stream_handle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamAbortCnf(U8 stream_handle)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_ABORT_CNF;
    parms.result = BT_A2DP_RESULT_OK;
    parms.p.A2DPEventAbortStreamCnf.stream_handle = stream_handle;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  A2dpHandleStreamAbortInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  stream_handle       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleStreamAbortInd(U8 stream_handle)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_ABORT_IND;
    parms.p.A2DPEventAbortStreamInd.stream_handle = stream_handle;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
    return;

}

void A2dpHandleStreamDataSentOut(U8 stream_handle)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_DATA_OUT;
    parms.p.A2DPEventStreamDataSentOut.stream_handle = stream_handle;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
    return;

}

/*****************************************************************************
 * FUNCTION
 *  A2dpHandleConfigChange
 * DESCRIPTION
 *  
 * PARAMETERS
 *  connect_id      [IN]        
 *  acp_seid        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleConfigChange(bt_a2dp_audio_cap_struct *audio_cap)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_DATA_CONFIG_CHANGE;
    parms.p.A2DPEventStreamDataConfigChange.audio_cap = audio_cap;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}

/*****************************************************************************
 * FUNCTION
 *  A2dpHandleConfigGet
 * DESCRIPTION
 *  
 * PARAMETERS
 *  connect_id      [IN]        
 *  acp_seid        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void A2dpHandleConfigGet(bt_a2dp_audio_cap_struct *audio_cap)
{
    BTA2DPCallbackParms parms;

    parms.event = BTA2DP_EVENT_STREAM_DATA_CONFIG_CNF;
    parms.p.A2DPEventStreamDataConfigCnf.audio_cap = audio_cap;
    bt_a2dp_ctx.a2dpCBfunc(&parms);
}
#endif

#endif /* __BT_A2DP_PROFILE__ */





