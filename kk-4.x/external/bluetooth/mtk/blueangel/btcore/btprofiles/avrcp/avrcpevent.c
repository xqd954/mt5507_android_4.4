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
#ifdef __BT_AVRCP_PROFILE__

#ifndef __BT_AVRCP_EVENT_C
#define __BT_AVRCP_EVENT_C
#include "bt_adp_avrcp.h"
#include "bluetooth_avrcp_struct.h"
#include "btbm.h"
#include "btbm_int.h"

extern avrcp_adp_context avrcp_cntx[BT_AVRCP_MAX_CHANNEL];
//extern kal_bool A2dpOpenStreamReq(bt_addr_struct *device_addr);

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleActivateCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl_num        [IN]        
 *  result              [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleActivateCnf(U8 chnl_num, U16 result)
{
    BTAVRCPCallbackParms parms;

    parms.event = BTAVRCP_EVENT_ACTIVATE_CNF;
    parms.result = result;
    parms.p.AVRCPEventActivateCnf.chnl_num = chnl_num;
    avrcp_cntx[chnl_num].avrcpCBfunc(&parms);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleDeactivateCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl_num        [IN]        
 *  result              [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleDeactivateCnf(U8 chnl_num, U16 result)
{
    BTAVRCPCallbackParms parms;

    parms.event = BTAVRCP_EVENT_DEACTIVATE_CNF;
    parms.result = result;
    parms.p.AVRCPEventDeactivateCnf.chnl_num = chnl_num;
    avrcp_cntx[chnl_num].avrcpCBfunc(&parms);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleConnectCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl_num        [IN]        
 *  result              [IN]        
 *  connection_id   [IN]        
 *  dev_addr         [IN]         
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleConnectCnf(U8 chnl_num, U16 result, bt_avrcp_dev_addr_struct *dev_addr, U8 connection_id, U16 browse_channel, U16 ctFeature,U16 tgFeature)
{
    BTAVRCPCallbackParms parms;
    kal_bool ret;

    parms.event = BTAVRCP_EVENT_CONNECT_CNF;
    parms.result = result;
    parms.p.AVRCPEventConnectCnf.chnl_num = chnl_num;
    parms.p.AVRCPEventConnectCnf.connection_id = connection_id;
    OS_MemCopy((U8*)&parms.p.AVRCPEventConnectCnf.dev_addr, (U8*)dev_addr, sizeof(bt_avrcp_dev_addr_struct));
    
    parms.p.AVRCPEventConnectCnf.browse_channel = browse_channel;
    parms.p.AVRCPEventConnectCnf.remote_ct_support_feature =ctFeature;
    parms.p.AVRCPEventConnectCnf.remote_tg_support_feature =tgFeature;
    
    avrcp_cntx[chnl_num].avrcpCBfunc(&parms);
    
    /* infor a2dp to start a connection request */
    
    //ret = A2dpOpenStreamReq((bt_addr_struct *)dev_addr);
    //kal_prompt_trace(MOD_BT, "[AVRCP] try to connect a2dp ret:%d", ret);
    
    return;
}


/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleConnectInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl_num      [IN]        
 *  device_addr  [IN]         
 *  connect_id    [IN]         
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleConnectInd(U8 chnl_num, bt_avrcp_dev_addr_struct* device_addr, U8 connect_id)
{
    BTAVRCPCallbackParms parms;
    BTBMRemoteNameSetting_T *deviceName = 0;
    BD_ADDR addr;
    

    parms.event = BTAVRCP_EVENT_CONNECT_IND;
    parms.result = BT_AVRCP_RESULT_SUCCESS;
    parms.p.AVRCPEventConnectInd.chnl_num = chnl_num;
    parms.p.AVRCPEventConnectInd.connection_id = connect_id;
    OS_MemCopy((U8*)&parms.p.AVRCPEventConnectInd.dev_addr, (U8*)device_addr, sizeof(bt_avrcp_dev_addr_struct));

    AVRCP_CONVERT_BDADDR2ARRAY(addr.addr, device_addr->lap, device_addr->uap, device_addr->nap);
    deviceName = btbmUtilConnectedDeviceNameEntryLookUp(&addr);
    if(deviceName !=0)
    {
        parms.p.AVRCPEventConnectInd.device_name_size = deviceName->dev_name_len;
        parms.p.AVRCPEventConnectInd.device_name = (U8 *)deviceName->dev_name;
    }
    else
    {
        parms.p.AVRCPEventConnectInd.device_name_size = 0;
        parms.p.AVRCPEventConnectInd.device_name = 0;

    }
    avrcp_cntx[chnl_num].avrcpCBfunc(&parms);
    return;
    
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleDisconnectCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl_num      [IN]        
 *  result            [IN]        
 *  connect_id     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleDisconnectCnf(U8 chnl_num, U16 result, U8 connect_id)
{
    BTAVRCPCallbackParms parms;

    parms.event = BTAVRCP_EVENT_DISCONNECT_CNF;
    parms.result = result;
    parms.p.AVRCPEventDisconnectCnf.chnl_num = chnl_num;
    parms.p.AVRCPEventDisconnectCnf.connection_id = connect_id;
    avrcp_cntx[chnl_num].avrcpCBfunc(&parms);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleDisconnectInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl_num      [IN]        
 *  connect_id    [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleDisconnectInd(U8 chnl_num, U8 connect_id)
{
    BTAVRCPCallbackParms parms;

    parms.event = BTAVRCP_EVENT_DISCONNECT_IND;
    parms.result = BT_AVRCP_RESULT_SUCCESS;
    parms.p.AVRCPEventDisconnectInd.chnl_num = chnl_num;
    parms.p.AVRCPEventDisconnectInd.connection_id = connect_id;
    avrcp_cntx[chnl_num].avrcpCBfunc(&parms);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleConnectCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl_num        [IN]        
 *  result              [IN]        
 *  connection_id   [IN]        
 *  dev_addr         [IN]         
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleBrowseConnectCnf(U8 chnl_num, U16 result, U16 bws_mtu)
{
    BTAVRCPCallbackParms parms;

    parms.event = BTAVRCP_EVENT_BROWSE_CONNECT_CNF;
    parms.result = result;
    parms.p.AVRCPEventBrowseConnectCnf.chnl_num = chnl_num;
	parms.p.AVRCPEventBrowseConnectCnf.result = result;
    parms.p.AVRCPEventBrowseConnectCnf.bws_mtu = bws_mtu;    
    avrcp_cntx[chnl_num].avrcpCBfunc(&parms);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleConnectInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl_num      [IN]        
 *  device_addr  [IN]         
 *  connect_id    [IN]         
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleBrowseConnectInd(U8 chnl_num, U16 bws_mtu)
{
    BTAVRCPCallbackParms parms;
    BTBMRemoteNameSetting_T *deviceName = 0;

    parms.event = BTAVRCP_EVENT_BROWSE_CONNECT_IND;
    parms.result = BT_AVRCP_RESULT_SUCCESS;
    parms.p.AVRCPEventBrowseConnectInd.chnl_num = chnl_num;
    parms.p.AVRCPEventBrowseConnectInd.bws_mtu = bws_mtu;
    avrcp_cntx[chnl_num].avrcpCBfunc(&parms);
    return;
    
}


/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleDisconnectInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl_num      [IN]        
 *  connect_id    [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleBrowseDisconnectInd(U8 chnl_num)
{
    BTAVRCPCallbackParms parms;

    parms.event = BTAVRCP_EVENT_BROWSE_DISCONNECT_IND;
    parms.result = BT_AVRCP_RESULT_SUCCESS;
    parms.p.AVRCPEventBrowseDisconnectInd.chnl_num = chnl_num;
    avrcp_cntx[chnl_num].avrcpCBfunc(&parms);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCommandFrameCnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl_num      [IN]        
 *  result            [IN]        
 *  seq_id           [IN]        
 *  c_type           [IN]        
 *  subunit_type  [IN]        
 *  subunit_id      [IN]        
 *  data_len        [IN]        
 *  frame_data    [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCommandFrameCnf(U8 chnl_num, U16 result, U8 seq_id, U8 c_type, U8 subunit_type, U8 subunit_id, U16 data_len, U8 *frame_data)
{
    BTAVRCPCallbackParms parms;

    parms.event = BTAVRCP_EVENT_COMMAND_FRAME_CNF;
    parms.result = result; //don't care result
    parms.p.AVRCPEventCmdFrameCnf.chnl_num = chnl_num;
    parms.p.AVRCPEventCmdFrameCnf.seq_id = seq_id;
    parms.p.AVRCPEventCmdFrameCnf.ctype = c_type;
    parms.p.AVRCPEventCmdFrameCnf.subunit_type = subunit_type;
    parms.p.AVRCPEventCmdFrameCnf.subunit_id = subunit_id;
    parms.p.AVRCPEventCmdFrameCnf.data_len = data_len;
    parms.p.AVRCPEventCmdFrameCnf.frame_data = frame_data;
    avrcp_cntx[chnl_num].avrcpCBfunc(&parms);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCommandFrameInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl_num      [IN]        
 *  result            [IN]        
 *  seq_id           [IN]        
 *  profile_id       [IN]        
 *  c_type           [IN]        
 *  subunit_type  [IN]        
 *  subunit_id      [IN]        
 *  data_len        [IN]        
 *  frame_data    [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCommandFrameInd(U8 chnl_num, U16 result, U8 seq_id, U8 profile_id, U8 c_type, U8 subunit_type, U8 subunit_id, U16 data_len,const U8 *frame_data)
{
    BTAVRCPCallbackParms parms;

    parms.event = BTAVRCP_EVENT_COMMAND_FRAME_IND;
    parms.result = result; //don't care result
    parms.p.AVRCPEventCmdFrameInd.chnl_num = chnl_num;
    parms.p.AVRCPEventCmdFrameInd.seq_id = seq_id;
    parms.p.AVRCPEventCmdFrameInd.profile_id = profile_id;
    parms.p.AVRCPEventCmdFrameInd.ctype = c_type;
    parms.p.AVRCPEventCmdFrameInd.subunit_type = subunit_type;
    parms.p.AVRCPEventCmdFrameInd.subunit_id = subunit_id;
    parms.p.AVRCPEventCmdFrameInd.data_len = data_len;
    parms.p.AVRCPEventCmdFrameInd.frame_data = (U8 *) frame_data;
    avrcp_cntx[chnl_num].avrcpCBfunc(&parms);
    return;
}



/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCommandFrameInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl_num      [IN]        
 *  result            [IN]        
 *  seq_id           [IN]        
 *  profile_id       [IN]        
 *  c_type           [IN]        
 *  subunit_type  [IN]        
 *  subunit_id      [IN]        
 *  data_len        [IN]        
 *  frame_data    [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleAVRCPCommandInd(U8 chnl_num, U16 result, U8 seq_id, U8 profile_id, U8 c_type, U8 subunit_type, U8 subunit_id, U16 data_len, U8 *frame_data)
{
    BTAVRCPCallbackParms parms;

    parms.event = BTAVRCP_EVENT_COMMAND_FRAME_IND;
    parms.result = result; //don't care result
    parms.p.AVRCPEventCmdFrameInd.chnl_num = chnl_num;
    parms.p.AVRCPEventCmdFrameInd.seq_id = seq_id;
    parms.p.AVRCPEventCmdFrameInd.profile_id = profile_id;
    parms.p.AVRCPEventCmdFrameInd.ctype = c_type;
    parms.p.AVRCPEventCmdFrameInd.subunit_type = subunit_type;
    parms.p.AVRCPEventCmdFrameInd.subunit_id = subunit_id;
    parms.p.AVRCPEventCmdFrameInd.data_len = data_len;
    parms.p.AVRCPEventCmdFrameInd.frame_data = frame_data;
    
    avrcp_cntx[chnl_num].avrcpCBfunc(&parms);
    return;
}



/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCBAVRCPCmdInd_Capabilities
 * DESCRIPTION
 *  Post GetCapabilites indication Event to AVRCP user 
 * PARAMETERS
 *  chnl    :  avrcp channel
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCBAVRCPCmdInd_Capabilities(AvrcpChnlInfo *chnl_info, U16 result, U8 seq_id, U8 profile_id, U8 c_type, U8 subunit_type, U8 subunit_id, U8 capability_id)
{
    BTAVRCPCallbackParms parms;

    parms.event = BTAVRCP_EVENT_GET_CAPABILITIES_IND;
    parms.result = result; //don't care result
    parms.p.AVRCPGetCapabilitiesInd.chnl_num = chnl_info->chnl_num;
    parms.p.AVRCPGetCapabilitiesInd.seq_id = chnl_info->transId;
    parms.p.AVRCPGetCapabilitiesInd.capability_id = capability_id;
    avrcp_cntx[chnl_info->chnl_num].avrcpCBfunc(&parms);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCBAVRCPCmdInd_Capabilities
 * DESCRIPTION
 *  Post List-Player-Setting-Values indication Event to AVRCP user 
 * PARAMETERS
 *  chnl    :  avrcp channel
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCBAVRCPCmdInd_ListPlayerValues(AvrcpChnlInfo *chnl_info, U16 result, U8 seq_id, U8 attribute_id)
{
    BTAVRCPCallbackParms parms;

    parms.event = BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGVALUE_IND;
    parms.result = result; //don't care result
    parms.p.AVRCPListPlayerappValueInd.chnl_num = chnl_info->chnl_num;
    parms.p.AVRCPListPlayerappValueInd.seq_id = chnl_info->transId;
    parms.p.AVRCPListPlayerappValueInd.attribute_id = attribute_id;
    avrcp_cntx[chnl_info->chnl_num].avrcpCBfunc(&parms);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCBAVRCPCmdInd_ListPlayerAttrs
 * DESCRIPTION
 *  Post List Player attributes indication Event to AVRCP user 
 * PARAMETERS
 *  chnl    :  avrcp channel
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCBAVRCPCmdInd_ListPlayerAttrs(AvrcpChnlInfo *chnl_info, U16 result, U8 seq_id)
{
    BTAVRCPCallbackParms parms;

    parms.event = BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGATTRIBUTE_IND;
    parms.result = result; //don't care result
    parms.p.AVRCPListPlayerappAttributeInd.chnl_num = chnl_info->chnl_num;
    parms.p.AVRCPListPlayerappAttributeInd.seq_id =   chnl_info->transId;
    avrcp_cntx[chnl_info->chnl_num].avrcpCBfunc(&parms);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCBAVRCPCmdInd_ListPlayerAttrs
 * DESCRIPTION
 *  Post List Player attributes indication Event to AVRCP user 
 * PARAMETERS
 *  chnl    :  avrcp channel
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCBAVRCPCmdInd_RegisterNotification(AvrcpChnlInfo *chnl_info, U16 result, U8 seq_id, U8 event_id)
{
    BTAVRCPCallbackParms parms;

    parms.event = BTAVRCP_EVENT_REGISTER_NOFITICATION_IND;
    parms.result = result; //don't care result
    parms.p.AVRCPListPlayerappAttributeInd.chnl_num = chnl_info->chnl_num;
    parms.p.AVRCPListPlayerappAttributeInd.seq_id   = chnl_info->transId;

    avrcp_cntx[chnl_info->chnl_num].avrcpCBfunc(&parms);
    return;
}

void AvrcpHandleCBAVRCPCmdInd_Common(AvrcpChnlInfo *chnl_info, U16 result, U8 event_id, U16 data_len, U8 *data){
    BTAVRCPCallbackParms parms;

    OS_MemSet( (U8 *)&parms, 0, sizeof(BTAVRCPCallbackParms));
    if( data_len > sizeof(parms.p) ){
        Assert(0);
    }
    OS_MemCopy( (U8 *)&parms.p.AVRCPListPlayerappAttributeInd, data, data_len);

    /// fill up the default attribute
    parms.event = event_id;
    parms.result = result; //don't care result
    parms.p.AVRCPListPlayerappAttributeInd.chnl_num = chnl_info->chnl_num;
    parms.p.AVRCPListPlayerappAttributeInd.seq_id   = chnl_info->transId;

    avrcp_cntx[chnl_info->chnl_num].avrcpCBfunc(&parms);
    return;

}

void AvrcpHandleCBAVRCPRsp_Common(AvrcpChnlInfo *chnl_info, U16 result, U8 event_id, U16 data_len, U8 *data){
    BTAVRCPCallbackParms parms;

    OS_MemSet( (U8 *)&parms, 0, sizeof(BTAVRCPCallbackParms));
    if( data_len > sizeof(parms.p) ){
        Assert(0);
    }
    OS_MemCopy( (U8 *)&parms.p.AVRCPListPlayerappAttributeInd, data, data_len);

    /// fill up the default attribute
    parms.event = event_id;
    parms.result = result; //don't care result
    parms.p.AVRCPListPlayerappAttributeInd.chnl_num = chnl_info->chnl_num;
    parms.p.AVRCPListPlayerappAttributeInd.seq_id   = chnl_info->transId;

    avrcp_cntx[chnl_info->chnl_num].avrcpCBfunc(&parms);
    return;

}


void AvrcpHandleCBAVRCPRsp_ReportError(AvrcpChnlInfo *chnl_info, U16 result, U8 error_code, U8 event_id, U16 data_len, U8 *data){
    BTAVRCPCallbackParms parms;

    OS_MemSet( (U8 *)&parms, 0, sizeof(BTAVRCPCallbackParms));
    parms.p.ABTAVRCPListPlayerappAttributeCnf.result = result;
    parms.p.ABTAVRCPListPlayerappAttributeCnf.error_code = error_code;

    if( data_len > sizeof(parms.p) ){
        Assert(0);
    }
    OS_MemCopy( (U8 *)&parms.p.AVRCPListPlayerappAttributeInd, data, data_len);

    /// fill up the default attribute
    parms.event = event_id;
    parms.result = result; //don't care result
    parms.error_code = error_code;
    parms.p.ABTAVRCPListPlayerappAttributeCnf.chnl_num = chnl_info->chnl_num;
    parms.p.ABTAVRCPListPlayerappAttributeCnf.seq_id   = chnl_info->transId;


    avrcp_cntx[chnl_info->chnl_num].avrcpCBfunc(&parms);
    return;

}


/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleAVRCPCnf_common
 * DESCRIPTION
 *  Reponse a structure to MMI
 * PARAMETERS
 *  chnl_num        [IN]        
 *  result              [IN]        
 *        
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleAVRCPCnf_common(AvrcpChnlInfo *chnl_info, U16 result, U8 event_id, U16 data_len, U8 *data)
{
    BTAVRCPCallbackParms parms;
	Assert( chnl_info->chnl_num < BT_AVRCP_MAX_CHANNEL );
	AvrcpAssert( data_len < 2048 );

    if( data_len > sizeof(parms.p) ){
        Assert(0);
    }
    OS_MemCopy( (U8 *)&parms.p.AVRCPListPlayerappAttributeInd, data, data_len);

    /// fill up the default attribute
    parms.event = event_id;
    parms.result = result; //don't care result
    parms.p.AVRCPListPlayerappAttributeInd.chnl_num = chnl_info->chnl_num;
    parms.p.AVRCPListPlayerappAttributeInd.seq_id   = chnl_info->transId;
    
    avrcp_cntx[chnl_info->chnl_num].avrcpCBfunc(&parms);
    return;
}

#endif

#endif /* __BT_AVRCP_PROFILE__ */
