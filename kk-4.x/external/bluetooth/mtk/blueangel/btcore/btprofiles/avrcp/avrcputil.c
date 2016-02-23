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
 *	bt_adp_avrcp.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file is the adaptor for AVRCP profile.
 *
 * Author:
 * -------
 *	Wilson Lien (mtk00676)
 *
 *==============================================================================
 * 				HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 09 03 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integration BT solution.
 *
 * 09 01 2010 sh.lai
 * NULL
 * Integration change. into 1036OF
 *
 * Mar 13 2009 mtk01393
 * [MAUI_01645202] [Bluetooth] Btadp folder restruct
 * 
 *
 * Dec 26 2007 mtk00676
 * [MAUI_00596125] [AVRCP] Change naming for AVRCP variable
 * 
 *
 * Oct 8 2007 mtk01239
 * [MAUI_00556508] [BT]Shrinking BT code size
 * 
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Sep 12 2006 mtk00676
 * [MAUI_00329303] [1]Assert fail:0BTMMICm.c3874-MMI
 * 
 *
 * Aug 14 2006 mtk00676
 * [MAUI_00321355] [WARNINGS] Fix compile warnings for 06.33
 * 
 *
 * Jul 31 2006 mtk01239
 * [MAUI_00214015] [BT] update BT code and add OPP, FTP SDP record
 * 
 *
 * Jul 17 2006 mtk00676
 * [MAUI_00210994] [AVRCP] Modification for ESI AVRCP
 * 
 *
 * Jul 17 2006 mtk00676
 * [MAUI_00210994] [AVRCP] Modification for ESI AVRCP
 * 
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#ifdef __BT_AVRCP_PROFILE__


#include "bt_common.h"
#include "osapi.h"
#include "bt_adp_avrcp.h"
#include "bluetooth_avrcp_struct.h"
#include "bt_adp_system.h"

#include "bt_ucs2.h"
#include "avrcp_inc.h"

avrcp_adp_context avrcp_cntx[BT_AVRCP_MAX_CHANNEL];  /* Global Context */
static U8 frame_data_buff[BT_AVRCP_MAX_CHANNEL][BT_AVRCP_MAX_RSP_FRAME_SIZE];  /* Reassemble AVRCP packets*/
static U16 frame_data_len[BT_AVRCP_MAX_CHANNEL];
static U8 g_avrcp_util_marshall_buffer[BT_AVRCP_MAX_RSP_FRAME_SIZE]; /* Buffer for create a packet's content. */
BTAVRCPCallbackParms g_avrcp_data;


extern U8 BTBMProfileAllocateConnIndex(U32 uuid, BD_ADDR addr);
extern BOOL BTBMProfileReleaseConnIndex(U8 index);

extern void AvrcpHandleActivateCnf(U8 chnl_num, U16 result, U8 version);
extern void AvrcpHandleDeactivateCnf(U8 chnl_num, U16 result);
extern void AvrcpHandleConnectCnf(U8 chnl_num, U16 result, bt_avrcp_dev_addr_struct *dev_addr, U8 connection_id, U16 browse_channel, U16 ctFeature,U16 tgFeature);
extern void AvrcpHandleConnectInd(U8 chnl_num, bt_avrcp_dev_addr_struct* device_addr, U8 connect_id);
extern void AvrcpHandleDisconnectCnf(U8 chnl_num, U16 result, U8 connect_id);
extern void AvrcpHandleDisconnectInd(U8 chnl_num, U8 connect_id);

extern void AvrcpHandleBrowseConnectCnf(U8 chnl_num, U16 result, U16 bws_mtu);
extern void AvrcpHandleBrowseConnectInd(U8 chnl_num, U16 bws_mtu);
extern void AvrcpHandleBrowseDisconnectInd(U8 chnl_num);

extern void AvrcpHandleCommandFrameCnf(U8 chnl_num, U16 result, U8 seq_id, U8 c_type, U8 subunit_type, U8 subunit_id, U16 data_len, U8 *frame_data);
extern void AvrcpHandleCommandFrameInd(U8 chnl_num, U16 result, U8 seq_id, U8 profile_id, U8 c_type, U8 subunit_type, U8 subunit_id, U16 data_len, const U8 *frame_data);

// Receive packets and dispatchers
extern void AvrcpHandleCBAVRCPCmdInd_Dispatcher(AvrcpChnlInfo *chnl_info, U8 ctype, U8 pdu_id,U16 pdu_parm_len, U16 parm_len, U8 *parms);
extern void AvrcpHandleCBAVRCPBrowseCmdInd_Dispatcher(AvrcpChnlInfo *chnl_info, AvctpCmdFrame *avctpCmd, U8 pdu_id, U16 pdu_parm_len, U16 parm_len, U8 *parms);
extern void AvrcpHandleCBAVCCommandInd(AvrcpChnlInfo *chnl_info, AvctpCmdFrame *avctpCmd, U16 operandLen, const U8 *operands);
extern void AvrcpHandleCBAVRCPBrowseRsp_Dispatcher(AvrcpChnlInfo *chnl_info, AvctpRspFrame *avctpRsp, U8 pdu_id,U16 pdu_parm_len, U16 parm_len, U8 *parms);
extern void AvrcpHandleCBAVRCPRsp_Dispatcher(AvrcpChnlInfo *chnl_info, U8 ctype, U8 pdu_id,U16 pdu_parm_len, U16 parm_len, U8 *parms);
extern void AvrcpHandleCBAVRCPRsp_Receiver(AvrcpChnlInfo *chnl_info, AvctpRspFrame *avctpRsp, U8 pdu_id,U16 pdu_parm_len, U16 parm_len, U8 *parms);
extern void AvrcpHandleCBAVCResponse(AvrcpChnlInfo *chnl_info, AvctpRspFrame *avctpRsp, U16 operandLen, U8 *operands);

// avrcpevent.c
void AvrcpHandleCBAVRCPCmdInd_Capabilities(AvrcpChnlInfo *chnl_info, U16 result, U8 seq_id, U8 profile_id, U8 c_type, U8 subunit_type, U8 subunit_id, U8 capability_id);
void AvrcpHandleCBAVRCPCmdInd_ListPlayerAttrs(AvrcpChnlInfo *chnl_info, U16 result, U8 seq_id);
void AvrcpHandleCBAVRCPCmdInd_ListPlayerValues(AvrcpChnlInfo *chnl_info, U16 result, U8 seq_id, U8 attribute_id);
void AvrcpHandleCBAVRCPCmdInd_Common(AvrcpChnlInfo *chnl_info, U16 result, U8 event_id, U16 data_len, U8 *data);
void AvrcpHandleCBAVRCPRsp_Common(AvrcpChnlInfo *chnl_info, U16 result, U8 event_id, U16 data_len, U8 *data);
void AvrcpHandleAVRCPCnf_common(AvrcpChnlInfo *chnl_info, U16 result, U8 event_id, U16 data_len, U8 *data);
void AvrcpUtilCallback(AvrcpChannel *chnl, AvrcpCallbackParms *Parms);
void AvrcpBrowseUtilCallback(AvrcpChannel *chnl, AvrcpCallbackParms *Parms);

// utils
U16 AVRTP_UtilGetBrowseMtu(U32 chnl_num);
U16 AvrcpGetSinglePacketMtu(AvctpChannel *chnl, U8 type);
void AvrcpHandleCBAVRCPRspInd_ShowErrorMsg(AvrcpChnlInfo *chnl_info, U8 pdu_id, U32 parm_len);
void AvrcpHandleCBAVRCPRsp_ReportError(AvrcpChnlInfo *chnl_info, U16 result, U8 error_code, U8 event_id, U16 data_len, U8 *data);
void AvrcpCancelContinueTimer(AvrcpChannel *chnl);
void AvrcpStartContinueTimer(AvrcpChannel *chnl);
BtStatus AVRCP_UtilAVRCPSDPQueryInd(AvrcpChannel *chnl, U8 status, U16 tg_version, U16 tg_feature, U16 ct_version, U16 ct_feature);

// avrcpsdp
extern BtStatus AvrcpStartMultiServiceQuery(AvrcpChannel *Chnl, SdpQueryMode mode, U16 local_role);
extern void bt_avrcp_send_sdp_query_ind(module_type dst_mod_id, U8 chnl_num, U8 seq_id, U8 status, U16 tg_version, U16 tg_feature, U16 ct_version, U16 ct_feature);

U8 g_avrcp_unit_send_data[6];  // support unit & subunit opcode

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilRegisterCB
 * DESCRIPTION
 *  AVRCP adaper registers callback function
 * PARAMETERS
 *  Callback : Callback function
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilRegisterCB(BTAVRCPCallback Callback)
{
    U8 i;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_REGISTERCB );
    
    for (i = 0; i < BT_AVRCP_MAX_CHANNEL; i++)
    {
        avrcp_cntx[i].avrcpCBfunc = Callback;
    }
}


/*****************************************************************************
 * FUNCTION
 *  AVRCP_UitlActivateReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr : message
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UitlActivateReq(U8 chnl_num, U8 local_role, U8 version, U8 sdpfeature)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //bt_avrcp_activate_req_struct *req = (bt_avrcp_activate_req_struct*) ilm_ptr->local_para_ptr;
    BtStatus status;
    AvrcpRole avrcp_uuid_local_role = 0;
    avrcp_adp_context *cntx_ptr;
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_ACTIVATE_REQ, chnl_num, local_role);
    OS_Report("AVRCP ver:%d sdpefature:%d", version, sdpfeature);
    
    if( BTCoreVerifySysInitState() == FALSE )
    {
        AvrcpHandleActivateCnf(chnl_num, BT_AVRCP_RESULT_BAD_STATE, 0);
        return;
    }  

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    cntx_ptr = &avrcp_cntx[chnl_num];

    AVRCP_CONVERT_UUID_FROM_ROLE(local_role, avrcp_uuid_local_role);

    /* Local Role as Target */
    if (local_role == BT_AVRCP_TG)
    {
        avrcp_uuid_local_role = AVRCP_TG;
    }
    /* Local Role as Controller */
    else if (local_role == BT_AVRCP_CT)
    {
        avrcp_uuid_local_role = AVRCP_CT;
    }
    else
    {
        ASSERT(0);
    }

    OS_Report("[AVRCP] AVRCP activate version:%d", version);

#ifdef __BT_AVRCP_V14__
    OS_Report("[AVRCP] AVRCP V14 compiled"); // allow version=10,13,14
#else
#ifdef __BT_AVRCP_V13__
    OS_Report("[AVRCP] AVRCP V13 compiled"); // allow version=10,13
#else
    OS_Report("[AVRCP] AVRCP V10 compiled"); // allow version=10
    if( version != 10 ){
        OS_Report("[AVRCP] AVRCP V10 compiled");
        version = 10;
    }
#endif
#endif

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ROLE_STATE, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    if (cntx_ptr->state != BT_AVRCP_STATE_IDLE)
    {
        AvrcpHandleActivateCnf(chnl_num, BT_AVRCP_RESULT_BAD_STATE, 0);
        return;
    }

    /* Deregister if necessary */
    if (cntx_ptr->rcp_role != BT_AVRCP_ROLE_UNDEF)
    {
        cntx_ptr->rcp_role = BT_AVRCP_ROLE_UNDEF;

        status = AVRCP_Deregister(&cntx_ptr->rcp_channel);
#ifdef MTK_BT_DEBUG_TRACE
        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_AVTIVATE_DEREGISTER_RESULT , status);
#endif        
    }

    /* Register AVRCP */
    cntx_ptr->rcp_channel.activeVersion = version;
    cntx_ptr->rcp_channel.sdpFeature = sdpfeature;
    status = AVRCP_Register(&cntx_ptr->rcp_channel, AvrcpUtilCallback, avrcp_uuid_local_role);

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_AVTIVATE_REGISTER_RESULT , local_role, status);
#endif    

    /* Send response */
    if (status == BT_STATUS_SUCCESS)
    {
        /* AV13 register browse channel */
        status = AVRCP_RegisterBrowse(&cntx_ptr->rcp_channel, AvrcpBrowseUtilCallback, avrcp_uuid_local_role);
        
        cntx_ptr->state = BT_AVRCP_STATE_ACTIVATED;
        cntx_ptr->rcp_role = local_role;

        AvrcpHandleActivateCnf(chnl_num, BT_AVRCP_RESULT_SUCCESS, cntx_ptr->rcp_channel.activeVersion);
    }
    else
    {
        AvrcpHandleActivateCnf(chnl_num, BT_AVRCP_RESULT_ERROR, 0);
    }
}


/*****************************************************************************
 * FUNCTION
 *  AVRCP_UitlDeactivateReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr : message
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UitlDeactivateReq(U8 chnl_num)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //bt_avrcp_deactivate_req_struct *req = (bt_avrcp_deactivate_req_struct*) ilm_ptr->local_para_ptr;
    BtStatus status;
    avrcp_adp_context *cntx_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_DEACTIVATE_REQ , chnl_num);

    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    cntx_ptr = &avrcp_cntx[chnl_num];
    if( BTCoreVerifySysInitState() == FALSE )
    {
        AvrcpHandleDeactivateCnf(chnl_num, BT_AVRCP_RESULT_BAD_STATE);
        return;
    }  

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ROLE_STATE, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    // pts44 TC_TG_CON_BV_04_C Need to disconnect browse & signal channel
    AVRCP_UitlDisconnectReq(chnl_num);
    AVRCP_UitlBrowseDisconnectReq(chnl_num);

    if (cntx_ptr->state == BT_AVRCP_STATE_IDLE)
    {
        AvrcpHandleDeactivateCnf(chnl_num, BT_AVRCP_RESULT_SUCCESS);
        return;
    }

    ASSERT(cntx_ptr->state == BT_AVRCP_STATE_ACTIVATED);

    /* Deregister AVRCP */
    status = AVRCP_Deregister(&cntx_ptr->rcp_channel);

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_DEAVTIVATE_DEREGISTER_RESULT , chnl_num, status);
#endif    

    /* Shall not deregister fail */
    ASSERT(status == BT_STATUS_SUCCESS || status == BT_STATUS_NOT_FOUND);

    /* Send response */
    if (status == BT_STATUS_SUCCESS || status == BT_STATUS_NOT_FOUND)
    {
        cntx_ptr->rcp_role = BT_AVRCP_ROLE_UNDEF;
        cntx_ptr->state = BT_AVRCP_STATE_IDLE;
        AvrcpHandleDeactivateCnf(chnl_num, BT_AVRCP_RESULT_SUCCESS);
    }
}


/*****************************************************************************
 * FUNCTION
 *  AVRCP_UitlConnectReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr : message
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UitlConnectReq(U8 chnl_num, U8 local_role, bt_avrcp_dev_addr_struct* avrcp_device_addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //bt_avrcp_connect_req_struct *req = (bt_avrcp_connect_req_struct*) ilm_ptr->local_para_ptr;
    BD_ADDR device_addr;
    BtStatus status;
    avrcp_adp_context *cntx_ptr = NULL;
    U32 uuid_role = 0;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_CONNECT_REQ , chnl_num, local_role);

    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ROLE_STATE, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    if (cntx_ptr->state != BT_AVRCP_STATE_ACTIVATED)
    {
        AvrcpHandleConnectCnf(chnl_num, BT_AVRCP_RESULT_BAD_STATE, avrcp_device_addr, 0, 0, 0, 0);
        return;
    }

    AVRCP_CONVERT_BDADDR2ARRAY(device_addr.addr, avrcp_device_addr->lap, avrcp_device_addr->uap, avrcp_device_addr->nap);

    // AV13: check the browse channel first. if the browse channel is connected. disconnect it
    if( TRUE == AVRCP_IsBrowseExisted(&cntx_ptr->rcp_channel) ){
        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_DISC_BWS_CHANNEL, &cntx_ptr->rcp_channel);
        AVRCP_DisconnectBrowse(&cntx_ptr->rcp_channel);
    }
    /* Prepare the retry */
    cntx_ptr->rcp_channel.chnl.retryCount = 0;
    cntx_ptr->rcp_channel.chnl.retryFlag = 0;
    OS_Report("[AVRCP] retry:%d Flag:%d", cntx_ptr->rcp_channel.chnl.retryCount, cntx_ptr->rcp_channel.chnl.retryFlag);

    status = AVRCP_Connect(&cntx_ptr->rcp_channel, &device_addr);

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CONNECT_RESULT , chnl_num, status);
#endif    

    /* Connected */
    if (status == BT_STATUS_SUCCESS)
    {
        AVRCP_CONVERT_UUID_FROM_ROLE(cntx_ptr->rcp_role, uuid_role);
        cntx_ptr->connect_id = BTBMProfileAllocateConnIndex(uuid_role, device_addr);
    
        cntx_ptr->state = BT_AVRCP_STATE_CONNECTED;
        AvrcpHandleConnectCnf(chnl_num, BT_AVRCP_RESULT_SUCCESS, avrcp_device_addr, cntx_ptr->connect_id,
            (cntx_ptr->rcp_channel.remoteTGSdp.tgSupportBrowse),
            cntx_ptr->rcp_channel.remoteCTSdp.ctFeatureFlag,
            cntx_ptr->rcp_channel.remoteTGSdp.tgFeatureFlag
            );
    }
    /* Connecting*/
    else if (status == BT_STATUS_PENDING)
    {
        cntx_ptr->state = BT_AVRCP_STATE_CONNECTING;
    }
    /* Connect Fail */
    else
    {
        AvrcpHandleConnectCnf(chnl_num, BT_AVRCP_RESULT_ERROR, avrcp_device_addr, 0, 0, 0, 0);
    }

}


/*****************************************************************************
 * FUNCTION
 *  AVRCP_UitlConnectIndRes
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr : message
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UitlConnectIndRes(U8 chnl_num, BOOL accept_conn)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //bt_avrcp_connect_ind_rsp_struct *req = (bt_avrcp_connect_ind_rsp_struct*) ilm_ptr->local_para_ptr;
    avrcp_adp_context *cntx_ptr = NULL;
    BtStatus status;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_CONNECT_IND_RES , chnl_num, accept_conn);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ROLE_STATE, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    if (cntx_ptr->state == BT_AVRCP_STATE_ACCEPTING_CONNECT)
    {
        if (accept_conn)
        {
            status = AVRCP_ConnectRsp(&cntx_ptr->rcp_channel, TRUE);
        }
        else
        {
            BTBMProfileReleaseConnIndex(cntx_ptr->connect_id);  
            status = AVRCP_ConnectRsp(&cntx_ptr->rcp_channel, FALSE);
        }
    }
    else
    {
        BTBMProfileReleaseConnIndex(cntx_ptr->connect_id);  
        status = AVRCP_ConnectRsp(&cntx_ptr->rcp_channel, FALSE);
    }

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CONNECT_IND_RES_RESULT , chnl_num, status);
#endif    
    
}


/*****************************************************************************
 * FUNCTION
 *  AVRCP_UitlDisconnectReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr : message
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UitlDisconnectReq(U8 chnl_num)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //bt_avrcp_disconnect_req_struct *req = (bt_avrcp_disconnect_req_struct*) ilm_ptr->local_para_ptr;
    BtStatus status;
    avrcp_adp_context *cntx_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_DISCONNECT_REQ , chnl_num);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ROLE_STATE, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    if (!(cntx_ptr->state == BT_AVRCP_STATE_CONNECTED || cntx_ptr->state == BT_AVRCP_STATE_CONNECTING))
    {
        AvrcpHandleDisconnectCnf(chnl_num, BT_AVRCP_RESULT_BAD_STATE, 0);
        return;
    }

    status = AVRCP_Disconnect(&cntx_ptr->rcp_channel);

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_DISCONNECT_RESULT , chnl_num, status);
#endif    

    /* Disconnected */
    if (status == BT_STATUS_NO_CONNECTION)
    {
        cntx_ptr->state = BT_AVRCP_STATE_ACTIVATED;

        BTBMProfileReleaseConnIndex(cntx_ptr->connect_id);
        AvrcpHandleDisconnectCnf(chnl_num, BT_AVRCP_RESULT_SUCCESS, cntx_ptr->connect_id);
        cntx_ptr->connect_id = 0;
    }
    /* Disconnecting, wait for AVCTP call back */
    else if (status == BT_STATUS_PENDING || status == BT_STATUS_IN_PROGRESS)
    {
        cntx_ptr->state = BT_AVRCP_STATE_DISCONNECTING;
    }
    /* Disconnect error */
    else
    {
        AvrcpHandleDisconnectCnf(chnl_num, BT_AVRCP_RESULT_ERROR, 0);
    }
}


/*****************************************************************************
 * FUNCTION
 *  AVRCP_UitlDisconnectReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr : message
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UitlInternalDisconnectReq(U8 chnl_num)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //bt_avrcp_disconnect_req_struct *req = (bt_avrcp_disconnect_req_struct*) ilm_ptr->local_para_ptr;
    BtStatus status;
    avrcp_adp_context *cntx_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_DISCONNECT_REQ , chnl_num);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ROLE_STATE, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    if (!(cntx_ptr->state == BT_AVRCP_STATE_CONNECTED || cntx_ptr->state == BT_AVRCP_STATE_CONNECTING))
    {
        return;
    }

    status = AVRCP_Disconnect(&cntx_ptr->rcp_channel);

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_DISCONNECT_RESULT , chnl_num, status);
#endif    

    /* Disconnected */
    if (status == BT_STATUS_NO_CONNECTION)
    {
        cntx_ptr->state = BT_AVRCP_STATE_ACTIVATED;

        BTBMProfileReleaseConnIndex(cntx_ptr->connect_id);
        AvrcpHandleDisconnectCnf(chnl_num, BT_AVRCP_RESULT_SUCCESS, cntx_ptr->connect_id);
        cntx_ptr->connect_id = 0;
    }
    /* Disconnecting, wait for AVCTP call back */
    else if (status == BT_STATUS_PENDING || status == BT_STATUS_IN_PROGRESS)
    {
        cntx_ptr->state = BT_AVRCP_STATE_DISCONNECTING;
    }
}



/*****************************************************************************
 * FUNCTION
 *  AVRCP_UitlCmdFrameReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr : message
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UitlCmdFrameReq(U8 chnl_num, U8 c_type, U8 subunit_type, U8 subunit_id, U16 data_len, U8* frame_data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //bt_avrcp_cmd_frame_req_struct *req = (bt_avrcp_cmd_frame_req_struct*) ilm_ptr->local_para_ptr;
    BtStatus status = BT_STATUS_FAILED;
#if defined( __BT_AVRCP_V14__ ) || defined( __BT_AVRCP_V13__ )
    AvrcpQueuePacketFrame *qPacket;
#endif
    avrcp_adp_context *cntx_ptr = NULL;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_CMD_FRAME_REQ , chnl_num, c_type);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ROLE_STATE, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    if (cntx_ptr->state != BT_AVRCP_STATE_CONNECTED)
    {
        AvrcpHandleCommandFrameCnf(
            chnl_num,
            BT_AVRCP_RESULT_BAD_STATE,
            0,  /* Not used */
            AVRCP_RESPONSE_REJECTED,
            subunit_type,
            subunit_id,
            data_len,
            frame_data);

        return;
    }

#if defined( __BT_AVRCP_V14__ ) || defined( __BT_AVRCP_V13__ )
    
    /* AV13 Use queue packet to send data */
    // pick up a queue cmd
	if( FALSE == IsListEmpty(&cntx_ptr->freePacketList ) ){
		qPacket = (AvrcpQueuePacketFrame *) RemoveHeadList(&cntx_ptr->freePacketList );
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_PICK_PACKET, qPacket);
    
		// prepare the cmd content
		// send it out
		if( NULL != qPacket ){
			Assert( (qPacket->data != NULL) );
		
			qPacket->cmd_type = 0; // pass through
			qPacket->is_cmd = TRUE;
			qPacket->transId = 0 ; //assign by avrcp
			qPacket->av_ctype = c_type; // AV/C c type
			qPacket->av_opcode = frame_data[0]; //0x7c; // pass through
			qPacket->data_len = data_len - 1 ; //
			OS_MemCopy( (U8 *) &qPacket->data[0], (const U8 *) &frame_data[1],  qPacket->data_len);
        
			status = AVRCP_SendQueuePacket(&cntx_ptr->rcp_channel, qPacket);

			if( BT_STATUS_PENDING != status ){
				// error to send ! back to packet pool
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_INSERT_PACKET, &cntx_ptr->freePacketList, qPacket, qPacket->data);
				InsertHeadList( &cntx_ptr->freePacketList, (ListEntry *)qPacket);
			}
		}
	}else{
		//Assert(0); //  list is empty ... enlarge the packet pool
		status = BT_STATUS_FAILED;
	}
#else
    cntx_ptr->cmdFrame.ctype = c_type;
    cntx_ptr->cmdFrame.subunitType = subunit_type;
    cntx_ptr->cmdFrame.subunitId = subunit_id;
    cntx_ptr->cmdFrame.opcode = frame_data[0];

    cntx_ptr->cmdFrame.operandLen = data_len - 1;
    OS_MemCopy((U8 *)&frame_data_buff[chnl_num][0], (U8 *)&frame_data[1], cntx_ptr->cmdFrame.operandLen);
    cntx_ptr->cmdFrame.operands = &frame_data_buff[chnl_num][0];

    status = AVRCP_SendCommand(&cntx_ptr->rcp_channel, &cntx_ptr->cmdFrame);
#endif    

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CMD_REQ_RESULT , chnl_num, status);
#endif    

    if (status == BT_STATUS_SUCCESS)
    {
        AvrcpHandleCommandFrameCnf(
            chnl_num,
            BT_AVRCP_RESULT_SUCCESS,
            0,  /* Not used */
            AVRCP_RESPONSE_ACCEPTED,
            subunit_type,
            subunit_id,
            data_len,
            frame_data);
    }
    /* Send Fail */
    else if (status != BT_STATUS_PENDING)
    {
        AvrcpHandleCommandFrameCnf(
            chnl_num,
            BT_AVRCP_RESULT_ERROR,
            0,  /* Not used */
            AVCTP_RESPONSE_REJECTED,
            subunit_type,
            subunit_id,
            data_len,
            frame_data);
    }

}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UitlCmdFrameRspInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr : message
 *  seq_id  : AV header's seq number
 *  c_type  : AV's response type (ex. AVCTP_RESPONSE_ACCEPTED)
 *  subunit_type  : AVRCP header subunit type
 *  subunit_id    : AVRCP header subunit id
 *  data_len      : len of frame_data
 *  frame_data    : first is AVRCP opcode others are operands
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UitlCmdFrameRspInd(U8 chnl_num, U8 seq_id, U8 c_type,  U8 subunit_type, U8 subunit_id, U16 data_len, const U8* frame_data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //bt_avrcp_cmd_frame_ind_rsp_struct *req = (bt_avrcp_cmd_frame_ind_rsp_struct*) ilm_ptr->local_para_ptr;
    BtStatus status = BT_STATUS_FAILED;
    avrcp_adp_context *cntx_ptr = NULL;
#if defined( __BT_AVRCP_V14__ ) || defined( __BT_AVRCP_V13__ )
    AvrcpQueuePacketFrame *qPacket;
#endif

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_CMD_FRAME_RSP_IND , chnl_num, c_type);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];
    OS_Report( "[AVRCP] transId:%d", seq_id);

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ROLE_STATE, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    if (cntx_ptr->state != BT_AVRCP_STATE_CONNECTED)
    {
        return;
    }

    if( data_len > 2 && AVCTP_RESPONSE_ACCEPTED == c_type && 0x80 == ( frame_data[1] & 0x80 ) ){
        OS_Report( "[AVRCP] _AVRCP release key:0x%x", (frame_data[1] & 0x7F));
    }
#if 0
    if( 0 != cntx_ptr && AVCTP_RESPONSE_ACCEPTED == c_type  && AVRCP_SUBUNIT_PANEL == subunit_type ){
        if( 1 == cntx_ptr->rcp_channel.ffInterceptRsp ){
            if( data_len > 2 ){
                if( 0x7c == frame_data[0] && (cntx_ptr->rcp_channel.ffKey | 0x80) == frame_data[1] ){
                    cntx_ptr->rcp_channel.ffInterceptRsp = 0;
                    cntx_ptr->rcp_channel.ffKey = 0;
                    OS_Report( "[AVRCP] _AVRCP ignore the passthrough response" );
                    return;
                }else{

                    OS_Report( "[AVRCP] _AVRCP len:%d 0x%x 0x%x ffKey:0x%x", data_len, frame_data[0], frame_data[1], cntx_ptr->rcp_channel.ffKey);
                }
            }
        }
    }
#endif
    
#if defined( __BT_AVRCP_V14__ ) || defined( __BT_AVRCP_V13__ )
    
    /* AV13 Use queue packet to send data */
    // pick up a queue cmd
	if( FALSE == IsListEmpty(&cntx_ptr->freePacketList ) ){
		qPacket = (AvrcpQueuePacketFrame *) RemoveHeadList(&cntx_ptr->freePacketList );
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_PICK_PACKET, qPacket);
    
		// prepare the cmd content
		// send it out
		if( NULL != qPacket ){
			Assert( (qPacket->data != NULL) );
		
			qPacket->cmd_type = 0; // pass through
			qPacket->is_cmd = FALSE;
			qPacket->transId = seq_id ; //assign by avrcp
			qPacket->av_ctype = c_type; //0x09 accept
			if( 0 == subunit_type ){
                subunit_type = 0x1F; /*PANEL SUBUNIT TYPE */
            }
			qPacket->av_addr = (subunit_type << 3) + (subunit_id & 0x7 );
			qPacket->av_opcode = frame_data[0]; //0x7c; // pass through
			qPacket->data_len = data_len - 1 ; //
			OS_MemCopy( (U8 *) &qPacket->data[0], (const U8 *) &frame_data[1],  qPacket->data_len);
        
			status = AVRCP_SendQueuePacket(&cntx_ptr->rcp_channel, qPacket);

			if( BT_STATUS_PENDING != status ){
				// error to send ! back to packet pool
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_INSERT_PACKET, &cntx_ptr->freePacketList, qPacket, qPacket->data);
				InsertHeadList( &cntx_ptr->freePacketList, (ListEntry *)qPacket);
			}
		}
	}else{
		//AvrcpAssert(0); //  list is empty ... enlarge the packet pool
		status = BT_STATUS_FAILED;
		OS_Report("WRN! AVRCP ignore the fail to send frame_ind_rsp");
	}
#else
    /* Send response */
    cntx_ptr->rspFrame.response = c_type; //AVCTP_RESPONSE_ACCEPTED
    cntx_ptr->rspFrame.transId  = seq_id;
    cntx_ptr->rspFrame.subunitType = subunit_type; //0x9 is panel
    cntx_ptr->rspFrame.subunitId = subunit_id; //0x0 -> subunitype(4bits) + subunitId(3bits)
    cntx_ptr->rspFrame.opcode = frame_data[0];

    cntx_ptr->rspFrame.operandLen = data_len - 1;
    OS_MemCopy((U8 *)&frame_data_buff[chnl_num][0], (U8 *)&frame_data[1], cntx_ptr->rspFrame.operandLen);
    cntx_ptr->rspFrame.operands = &frame_data_buff[chnl_num][0];

    status = AVRCP_SendResponse(&cntx_ptr->rcp_channel, &cntx_ptr->rspFrame);
#endif    

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CMD_RSP_IND_RESULT , chnl_num, status);
#endif    

}



/*****************************************************************************
 * FUNCTION
 *  AVRCP_UitlBrowseConnectReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr : message
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UitlBrowseConnectReq(U8 chnl_num)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    avrcp_adp_context *cntx_ptr = NULL;
    U32 uuid_role = 0;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
//    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_BROWSE_CONNECT_REQ , chnl_num);

    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ROLE_STATE, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    if( AVRCP_IsBrowseExisted(&cntx_ptr->rcp_channel) )
    {
        AvrcpHandleBrowseConnectCnf(chnl_num, BT_AVRCP_RESULT_SUCCESS, AVRTP_UtilGetBrowseMtu(chnl_num) );
        return;
    }

	if( AVRCP_ConnectBrowse(&cntx_ptr->rcp_channel) != BT_STATUS_PENDING ){
        AvrcpHandleBrowseConnectCnf(chnl_num, BT_AVRCP_RESULT_ERROR, 0);
        return;
	}
	
	cntx_ptr->rcp_channel.bwsConnFlag = TRUE;
    // wait for event callback
    
}


/*****************************************************************************
 * FUNCTION
 *  AVRCP_UitlBrowseDisconnectReq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr : message
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UitlBrowseDisconnectReq(U8 chnl_num)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status;
    avrcp_adp_context *cntx_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_DISCONNECT_REQ , chnl_num);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ROLE_STATE, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    if ( !AVRCP_IsBrowseExisted(&cntx_ptr->rcp_channel) )
    {
        AvrcpHandleBrowseDisconnectInd(chnl_num);
        return;
    }

    status = AVRCP_DisconnectBrowse(&cntx_ptr->rcp_channel);
    if( BT_STATUS_PENDING != status )
    {
        AvrcpHandleBrowseDisconnectInd(chnl_num);
    }
    
    // wait for event back
}

void AVRCP_UitlInternalBrowseDisconnectReq(U8 chnl_num)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status;
    avrcp_adp_context *cntx_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_DISCONNECT_REQ , chnl_num);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ROLE_STATE, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    if ( !AVRCP_IsBrowseExisted(&cntx_ptr->rcp_channel) )
    {
        return;
    }

    status = AVRCP_DisconnectBrowse(&cntx_ptr->rcp_channel);
    OS_Report("AVRCP_DisconnectBrowse status:%d ", status);    
    // wait for event back
}


/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilSendAVRCPReject
 * DESCRIPTION
 *  Reject remote's AV/C with error
 *  BT AVRCP 14 spec. 6.15.1 AVRCP Specific AV/C Commands
 * PARAMETERS
 *  pdu_id : avrcp pdu-id
 *  error_code : avrcp error code
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilSendAVRCPReject(AvrcpChnlInfo *chnl_info, U8 pdu_id, U8 error_code){
    AvrcpQueuePacketFrame *qPacket;
	BtStatus status = BT_STATUS_FAILED;
    avrcp_adp_context *cntx_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(chnl_info->chnl_num < BT_AVRCP_MAX_CHANNEL);
    cntx_ptr = &avrcp_cntx[chnl_info->chnl_num];
    
    if( FALSE == IsListEmpty(&cntx_ptr->freePacketList ) ){
		qPacket = (AvrcpQueuePacketFrame *) RemoveHeadList(&cntx_ptr->freePacketList );
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_PICK_PACKET, qPacket);
    
		// prepare the packet content (response - reject a command)
		// send it out
		if( NULL != qPacket ){
			Assert( (qPacket->data != NULL) );
		
			qPacket->cmd_type = AVRCP_CMD_FORMAT_AVRCP_SPECIFIC; // pass through
			qPacket->is_cmd   = FALSE;
			qPacket->transId  = chnl_info->transId ; //assign by avrcp
			qPacket->av_ctype = AVCTP_RESPONSE_REJECTED; // AV/C c type
			qPacket->av_opcode= AVRCP_OPCODE_VENDOR_DEPENDENT;
			qPacket->pdu_id   = pdu_id; 
			qPacket->data_len = 1;
			qPacket->data[0]  = error_code; // ex. BT_AVRCP_ERRCODE_INVALID_CMD
            
			status = AVRCP_SendQueuePacket(&cntx_ptr->rcp_channel, qPacket);

			if( BT_STATUS_PENDING != status ){
				// error to send ! back to packet pool
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_INSERT_PACKET, &cntx_ptr->freePacketList, qPacket, qPacket->data);
				InsertHeadList( &cntx_ptr->freePacketList, (ListEntry *)qPacket);
			}
		}
	}
	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_REJECT_PACKET, chnl_info->chnl_num, status);
}


/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilSendAVRCPNotImplement
 * DESCRIPTION
 *  Reject remote's AV/C with error
 *  BT AVRCP 14 spec. 6.15.1 AVRCP Specific AV/C Commands
 * PARAMETERS
 *  pdu_id : avrcp pdu-id
 *  error_code : avrcp error code
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilSendAVRCPNotImplement(AvrcpChnlInfo *chnl_info, U8 datalen, const U8 *data){
    AvrcpQueuePacketFrame *qPacket;
	BtStatus status = BT_STATUS_FAILED;
    avrcp_adp_context *cntx_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(chnl_info->chnl_num < BT_AVRCP_MAX_CHANNEL);
    cntx_ptr = &avrcp_cntx[chnl_info->chnl_num];
    
    if( FALSE == IsListEmpty(&cntx_ptr->freePacketList ) ){
		qPacket = (AvrcpQueuePacketFrame *) RemoveHeadList(&cntx_ptr->freePacketList );
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_PICK_PACKET, qPacket);
    
		// prepare the packet content (response - reject a command)
		// send it out
		if( NULL != qPacket ){
			Assert( (qPacket->data != NULL) );
		
			qPacket->cmd_type = AVRCP_CMD_FORMAT_PASS_THROUGH; // pass through
			qPacket->is_cmd   = FALSE;
			qPacket->transId  = chnl_info->transId ; //assign by avrcp
			qPacket->av_ctype = AVCTP_RESPONSE_NOT_IMPLEMENTED; // AV/C c type
			qPacket->av_opcode= AVRCP_OPCODE_VENDOR_DEPENDENT;
			qPacket->pdu_id   = 0;
			qPacket->data_len = 0;
			if( datalen > 128 ){
				datalen = 128;
			}
			if( datalen > 0 ){
				qPacket->pdu_id   = data[0];
				qPacket->data_len = datalen -1;
			}
			if( qPacket->data_len > 0 ){
				OS_MemCopy( (U8 *)qPacket->data, data+1, qPacket->data_len); // ex. BT_AVRCP_ERRCODE_INVALID_CMD
			}
            
			status = AVRCP_SendQueuePacket(&cntx_ptr->rcp_channel, qPacket);

			if( BT_STATUS_PENDING != status ){
				// error to send ! back to packet pool
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_INSERT_PACKET, &cntx_ptr->freePacketList, qPacket, qPacket->data);
				InsertHeadList( &cntx_ptr->freePacketList, (ListEntry *)qPacket);
			}
		}
	}
	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_REJECT_PACKET, chnl_info->chnl_num, status);
}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilSendAVRCPBrowseReject
 * DESCRIPTION
 *  Reject remote's AV/C with error
 *  BT AVRCP 14 spec. 6.15.1 AVRCP Specific AV/C Commands
 * PARAMETERS
 *  pdu_id : avrcp pdu-id
 *  error_code : avrcp error code
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilSendAVRCPBrowseReject(AvrcpChnlInfo *chnl_info, U8 pdu_id, U8 error_code){
    AvrcpQueuePacketFrame *qPacket;
	BtStatus status = BT_STATUS_FAILED;
    avrcp_adp_context *cntx_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(chnl_info->chnl_num < BT_AVRCP_MAX_CHANNEL);
    cntx_ptr = &avrcp_cntx[chnl_info->chnl_num];
    
    if( FALSE == IsListEmpty(&cntx_ptr->freePacketList ) ){
		qPacket = (AvrcpQueuePacketFrame *) RemoveHeadList(&cntx_ptr->freePacketList );
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_BWS_PICK_PACKET, qPacket);
    
		// prepare the packet content (response - reject a command)
		// send it out
		if( NULL != qPacket ){
			Assert( (qPacket->data != NULL) );
		
			qPacket->cmd_type = AVRCP_CMD_FORMAT_BROWSE; // pass through
			qPacket->is_cmd   = FALSE;
			qPacket->transId  = chnl_info->transId ; //assign by avrcp
			qPacket->av_ctype = AVCTP_RESPONSE_REJECTED; // AV/C c type
			qPacket->av_opcode= AVRCP_OPCODE_BROWSE_CAHNNEL;
			qPacket->pdu_id   = pdu_id; 
			qPacket->data_len = 1;
			qPacket->data[0]  = error_code; // ex. BT_AVRCP_ERRCODE_INVALID_CMD
            
			status = AVRCP_SendBrowseQueuePacket(&cntx_ptr->rcp_channel, qPacket);

			if( BT_STATUS_PENDING != status ){
				// error to send ! back to packet pool
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_INSERT_PACKET, &cntx_ptr->freePacketList, qPacket, qPacket->data);
				InsertHeadList( &cntx_ptr->freePacketList, (ListEntry *)qPacket);
			}
		}
	}
	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_SEND_REJECT_PACKET, chnl_info->chnl_num, status);
}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilSendBTSIGCapability
 * DESCRIPTION
 *  Send a BT SIG capability reponse packet
 * PARAMETERS
 *  chnl_num : avrcp channel index
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilSendBTSIGCapability(AvrcpChnlInfo *chnl_info, U8 chnl_num){
    AvrcpQueuePacketFrame *qPacket;
	BtStatus status;
    avrcp_adp_context *cntx_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];
    
    if( FALSE == IsListEmpty(&cntx_ptr->freePacketList ) ){
		qPacket = (AvrcpQueuePacketFrame *) RemoveHeadList(&cntx_ptr->freePacketList );
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_PICK_PACKET, qPacket);
    
		// prepare the cmd content
		// send it out
		if( NULL != qPacket ){
			Assert( (qPacket->data != NULL) );
		
			qPacket->cmd_type = AVRCP_CMD_FORMAT_AVRCP_SPECIFIC; // pass through
			qPacket->is_cmd   = FALSE;
			qPacket->transId  = chnl_info->transId ; //assign by avrcp
			qPacket->av_ctype = AVCTP_RESPONSE_IMPLEMENTED_STABLE; // AV/C c type
			qPacket->av_opcode= AVRCP_OPCODE_VENDOR_DEPENDENT;
			qPacket->pdu_id   = AVRCP_SPECIFIC_OPCODE_GET_CAPABILITIES; //0x7c; // pass through
			qPacket->data_len = 5;
			qPacket->data[0]  = 0x02;
			qPacket->data[1]  = 0x01;
			qPacket->data[2]  = 0x00;
			qPacket->data[3]  = 0x19;
            qPacket->data[4]  = 0x58;
            
			status = AVRCP_SendQueuePacket(&cntx_ptr->rcp_channel, qPacket);

			if( BT_STATUS_PENDING != status ){
				// error to send ! back to packet pool
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_INSERT_PACKET, &cntx_ptr->freePacketList, qPacket, qPacket->data);
				InsertHeadList( &cntx_ptr->freePacketList, (ListEntry *)qPacket);
			}
		}
	}

}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPCmdBufferRsp
 * DESCRIPTION
 *  A generic function to send a AVRCP reponse packet
 *  Each response has different payload.
 *  Pass the payload by a byte array
 * PARAMETERS
 *  chnl_info : information for AVCTP to pass a response packet back
 *  c_type : AV/C header response type
 *  pdu_id : AVRCP pdu_id
 *  buffer_len : payload length
 *  buffer : payload data
 * RETURNS
 *  BtStatus 
 *****************************************************************************/
BtStatus AVRCP_UtilAVRCPCmdBufferRsp(AvrcpChnlInfo *chnl_info, U8 c_type, U8 pdu_id, U16 buffer_len, U8* buffer){

    // compose a packet and send it through queue api
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status = BT_STATUS_SUCCESS;

    AvrcpQueuePacketFrame *qPacket;

    avrcp_adp_context *cntx_ptr = NULL;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_GENERIC_RSP, chnl_info->chnl_num, c_type, pdu_id);

    ASSERT(chnl_info->chnl_num < BT_AVRCP_MAX_CHANNEL);
    cntx_ptr = &avrcp_cntx[chnl_info->chnl_num];

    /* AV13 Use queue packet to send data */
    // pick up a respose cmd
	if( FALSE == IsListEmpty(&cntx_ptr->freePacketList ) ){
		qPacket = (AvrcpQueuePacketFrame *) RemoveHeadList(&cntx_ptr->freePacketList );
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_PICK_PACKET, qPacket);
    
		// prepare the cmd content
		// send it out
		if( NULL != qPacket ){
			Assert( (qPacket->data != NULL) );
		
			qPacket->cmd_type = AVRCP_CMD_FORMAT_AVRCP_SPECIFIC;              // avrcp specific
			if( c_type >= 7 ){
				qPacket->is_cmd   = FALSE;
			}else{
    			qPacket->is_cmd   = TRUE;
    		}
			qPacket->transId  = chnl_info->transId ;                        // assign by avrcp
			qPacket->av_ctype = c_type;                    // AV/C cmd type
			qPacket->av_opcode= 0x0;                       // vendor
			qPacket->av_addr = 0x48;                       // AV/C address subunit type and id. fixed value in spec			
			qPacket->pdu_id   = pdu_id;
			qPacket->data_len = buffer_len;
			
			if( buffer_len > AVRCP_MAX_PACKET_LEN ){
			    buffer_len = AVRCP_MAX_PACKET_LEN;
			}
			OS_MemCopy( (U8 *) qPacket->data, (const U8 *) buffer,  buffer_len);
        
			status = AVRCP_SendQueuePacket(&cntx_ptr->rcp_channel, qPacket);

			if( BT_STATUS_PENDING != status ){
				// error to send ! back to packet pool
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_INSERT_PACKET, &cntx_ptr->freePacketList, qPacket, qPacket->data);
				InsertHeadList( &cntx_ptr->freePacketList, (ListEntry *)qPacket);
			}
		}
	}else{
		//  list is empty . Unable to send
		if( c_type < 7 ){
			// send command request too quickly (CT role)
			AvrcpAssert(0); 
		}else{
			// recive to many command request (TG role)
		}
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_NO_AVAILABLE, c_type);
		status = BT_STATUS_FAILED;
	}
	return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVCmdBufferRsp
 * DESCRIPTION
 *  A  function to send a AV/C reponse packet
 *  To send UNIT response
 *  To send SUBUNIT response
 * PARAMETERS
 *  chnl_info : information for AVCTP to pass a response packet back
 *  c_type : AV/C header response type
 *  pdu_id : AVRCP pdu_id
 *  buffer_len : payload length
 *  buffer : payload data
 * RETURNS
 *  BtStatus 
 *****************************************************************************/
BtStatus AVRCP_UtilAVCmdBufferRsp(AvrcpChnlInfo *chnl_info, U8 c_type, U8 pdu_id, U16 buffer_len, U8* buffer){

    // compose a packet and send it through queue api
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status = BT_STATUS_SUCCESS;

    AvrcpQueuePacketFrame *qPacket;

    avrcp_adp_context *cntx_ptr = NULL;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_RSP, chnl_info->chnl_num, c_type, pdu_id);

    ASSERT(chnl_info->chnl_num < BT_AVRCP_MAX_CHANNEL);
    cntx_ptr = &avrcp_cntx[chnl_info->chnl_num];

    /* AV13 Use queue packet to send data */
    // pick up a respose cmd
	if( FALSE == IsListEmpty(&cntx_ptr->freePacketList ) ){
		qPacket = (AvrcpQueuePacketFrame *) RemoveHeadList(&cntx_ptr->freePacketList );
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_PICK_PACKET, qPacket);
    
		// prepare the cmd content
		// send it out
		if( NULL != qPacket ){
			Assert( (qPacket->data != NULL) );
		
			qPacket->cmd_type = AVRCP_CMD_FORMAT_AVRCP_SPECIFIC;              // avrcp specific
			if( c_type >= 7 ){
				qPacket->is_cmd   = FALSE;
			}else{
    			qPacket->is_cmd   = TRUE;
    		}
			qPacket->transId  = chnl_info->transId ;                        // assign by avrcp
			qPacket->av_ctype = c_type;                    // AV/C cmd type
			qPacket->av_opcode= 0x0;                       // vendor
                        qPacket->av_addr = 0x48;                       // AV/C address subunit type and id. fixed value in spec			
			qPacket->pdu_id   = pdu_id;
			qPacket->data_len = buffer_len;
			
			if( buffer_len > AVRCP_MAX_PACKET_LEN ){
			    Assert(0);
			}
			OS_MemCopy( (U8 *) qPacket->data, (const U8 *) buffer,  buffer_len);
        
			status = AVRCP_SendQueuePacket(&cntx_ptr->rcp_channel, qPacket);

			if( BT_STATUS_PENDING != status ){
				// error to send ! back to packet pool
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_INSERT_PACKET, &cntx_ptr->freePacketList, qPacket, qPacket->data);
				InsertHeadList( &cntx_ptr->freePacketList, (ListEntry *)qPacket);
			}
		}
	}else{
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_EMPTY_PACKET_LIST, &cntx_ptr->freePacketList);
		//  list is empty . Unable to send
		if( c_type < 7 ){
			// send command request too quickly (CT role)
			AvrcpAssert(0); 
		}else{
			// recive to many command request (TG role)
		}
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_NO_AVAILABLE, c_type);
		status = BT_STATUS_FAILED;
	}
	return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPCmdBufferRsp
 * DESCRIPTION
 *  A generic function to send a AVRCP reponse packet
 *  Each response has different payload.
 *  Pass the payload by a byte array
 * PARAMETERS
 *  chnl_info : information for AVCTP to pass a response packet back
 *  c_type : AV/C header response type
 *  pdu_id : AVRCP pdu_id
 *  buffer_len : payload length
 *  buffer : payload data
 * RETURNS
 *  BtStatus 
 *****************************************************************************/
BtStatus AVRCP_UtilAVRCPBrowseCmdBufferRsp(AvrcpChnlInfo *chnl_info, U8 c_type, U8 pdu_id, U16 buffer_len, U8* buffer){
    // compose a packet and send it through queue api
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status = BT_STATUS_SUCCESS;

    AvrcpQueuePacketFrame *qPacket;

    avrcp_adp_context *cntx_ptr = NULL;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_CMD_RSP, chnl_info->chnl_num, c_type, pdu_id);

    ASSERT(chnl_info->chnl_num < BT_AVRCP_MAX_CHANNEL);
    cntx_ptr = &avrcp_cntx[chnl_info->chnl_num];

    /* AV13 Use queue packet to send data */
    // pick up a respose cmd
	if( FALSE == IsListEmpty(&cntx_ptr->freePacketList ) ){
		qPacket = (AvrcpQueuePacketFrame *) RemoveHeadList(&cntx_ptr->freePacketList );
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_BWS_PICK_PACKET, qPacket);
    
		// prepare the cmd content
		// send it out
		if( NULL != qPacket ){
			Assert( (qPacket->data != NULL) );
		
			qPacket->cmd_type = AVRCP_CMD_FORMAT_BROWSE;   // avrcp specific
			if( c_type >= 7 ){
    			qPacket->is_cmd = FALSE;
    		}else{
    			qPacket->is_cmd = TRUE;    		
    		}
			qPacket->transId  = chnl_info->transId;        // Only work at is_cmd=FALSE
			qPacket->av_ctype = c_type;                    // AVCTP header
			qPacket->av_opcode= 0x0;                       // NOT use
			qPacket->av_addr = 0x48;                       // NOT use
			qPacket->pdu_id   = pdu_id;
			qPacket->data_len = buffer_len;
			if( buffer_len > AVRCP_MAX_PACKET_LEN ){
			    Assert(0);
			}
			OS_MemCopy( (U8 *) qPacket->data, (const U8 *) buffer,  buffer_len);

			status = AVRCP_SendBrowseQueuePacket(&cntx_ptr->rcp_channel, qPacket);

			if( BT_STATUS_PENDING != status ){

				// error to send ! back to packet pool
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_BWS_INSERT_PACKET, &cntx_ptr->freePacketList, qPacket, qPacket->data);
				InsertHeadList( &cntx_ptr->freePacketList, (ListEntry *)qPacket);

                if( FALSE == qPacket->is_cmd && status == BT_STATUS_INVALID_PARM ){
                    // packet too large
                    AVRCP_UtilSendAVRCPBrowseReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INTERNAL_ERROR);
                }
			}
		}
	}else{
		//  list is empty . Unable to send
		if( c_type < 7 ){
			// send command request too quickly (CT role)
			AvrcpAssert(0); 
		}else{
			// recive to many command request (TG role)
		}
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_NO_AVAILABLE, c_type);
		status = BT_STATUS_FAILED;
	}
	return status;
}


/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPCmdCapabiltiesRsp
 * DESCRIPTION
 *  Send a capability reponse packet when asking the support event list
 * PARAMETERS
 *  chnl_num : avrcp channel index
 *  c_type : AV/C response type
 *  count : support num of events
 *  events : event id list which are supported
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilAVRCPCmdCapabiltiesRsp(AvrcpChnlInfo *chnl_info, U8 c_type, U16 count, U8* events){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;

    // marshalling
    buffer[0] = 0x03;
    buffer[1]  = (U8) count;
    if( buffer[1] > BT_AVRCP_MAX_EVENT_NUM ){
        buffer[1] = BT_AVRCP_MAX_EVENT_NUM; // real length is buffer[1]
    }
    OS_MemCopy((U8 *)&buffer[2], events, buffer[1]);
    buffer_len = 2 + buffer[1];
    
    AVRCP_UtilAVRCPCmdBufferRsp(chnl_info, AVCTP_RESPONSE_IMPLEMENTED_STABLE, AVRCP_SPECIFIC_OPCODE_GET_CAPABILITIES, buffer_len, buffer);
}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPCmdListPlayerAttributesRsp
 * DESCRIPTION
 *  Convert a list_playerapp_attribute structure to byte array and
 *  invoke AVRCP_UtilAVRCPCmdBufferRsp to sent it
 * PARAMETERS
 *  rsp : list_playerapp_attribute data
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilAVRCPCmdListPlayerAttributesRsp(bt_avrcp_list_playerapp_attribute_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
	AvrcpChnlInfo myChnlInfo;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_LIST_PLAYERAPP_SETTINGATTRIBUTE;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling
        buffer[0] = rsp->count;
        if( buffer[0] > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
            buffer[0] = BT_AVRCP_MAX_ATTRIBUTE_NUM;
        }
        OS_MemCopy((U8 *)&buffer[1], rsp->attribute_id, buffer[0]);
        buffer_len = 1 + buffer[0];
        
        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_IMPLEMENTED_STABLE, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPCmdListPlayerValuesRsp
 * DESCRIPTION
 *  Convert a list_playerapp_value structure to byte array and
 *  invoke AVRCP_UtilAVRCPCmdBufferRsp to sent it
 * PARAMETERS
 *  rsp : list_playerapp_value data
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilAVRCPCmdListPlayerValuesRsp(bt_avrcp_list_playerapp_value_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_SPECIFIC_OPCODE_LIST_PLAYERAPP_SETTINGVALUE;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
		// marshalling
        buffer[0] = rsp->count;
        if( buffer[0] > BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM ){
            buffer[0] = BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM;
        }
        OS_MemCopy((U8 *)&buffer[1], rsp->attribute_values, buffer[0]);
        buffer_len = 1 + buffer[0];
        
        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_IMPLEMENTED_STABLE, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}

}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPCmdListPlayerValuesRsp
 * DESCRIPTION
 *  Convert a get_curplayerapp structure to byte array and
 *  invoke AVRCP_UtilAVRCPCmdBufferRsp to sent it
 * 
 *  The response packet is used for a AV/C status command
 * PARAMETERS
 *  rsp : get_curplayerapp data
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilAVRCPGetCurPlayerValueRsp( bt_avrcp_get_curplayerapp_value_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_SPECIFIC_OPCODE_GET_CURPLAYERAPP_SETTINGVALUE;
    U8 i;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling
        buffer[0] = rsp->count;
        if( buffer[0] > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
            buffer[0] = BT_AVRCP_MAX_ATTRIBUTE_NUM;
        }
        buffer_len = 1;

        // spec 6.5.3        
        for( i = 0 ; i < rsp->count; i++ ){
        	buffer[buffer_len] = rsp->attribute_id[i];
        	buffer_len++;
			buffer[buffer_len] = rsp->attribute_value[i];
        	buffer_len++;        	
        }
        /*
        OS_MemCopy((U8 *)&buffer[buffer_len], rsp->attribute_id, buffer[0]);
        buffer_len += buffer[0];

        OS_MemCopy((U8 *)&buffer[buffer_len], rsp->attribute_value, buffer[0]);
        buffer_len += buffer[0];
        */
        
        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_IMPLEMENTED_STABLE, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPCmdSetPlayerValueRsp
 * DESCRIPTION
 *  Convert a set_playerapp_value structure to byte array and
 *  invoke AVRCP_UtilAVRCPCmdBufferRsp to sent it
 * 
 *  The response packet is used for a AV/C control command
 * PARAMETERS
 *  rsp : set_playerapp_value data
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilAVRCPCmdSetPlayerValueRsp( bt_avrcp_set_playerapp_value_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_SPECIFIC_OPCODE_SET_PLAYERAPP_SETTINGVALUE;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling
    	// sepc 6.5.4 page 52 - None parameters
    	buffer_len = 0;
        
        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPCmdGetPlayerAttrTextRsp
 * DESCRIPTION
 *  Convert a get_playerapp_attributetext structure to byte array and
 *  invoke AVRCP_UtilAVRCPCmdBufferRsp to sent it
 * 
 *  The response packet is used for a AV/C status command
 * PARAMETERS
 *  rsp : get_playerapp_attributetext data
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilAVRCPCmdGetPlayerAttrTextRsp(bt_avrcp_get_playerapp_attributetext_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_SPECIFIC_OPCODE_GET_PLAYERAPP_SETTINGATTRIBUTETEXT;
    U8 i;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling
        buffer[0] = rsp->count;
        if( buffer[0] > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
            buffer[0] = BT_AVRCP_MAX_ATTRIBUTE_NUM;
        }
        buffer_len = 1;

        // spec 6.5.5 page 53
        for( i = 0; i< buffer[0] && i < BT_AVRCP_MAX_ATTRIBUTE_NUM; i++ ){
            buffer[buffer_len++] = rsp->attribute_id[i];
            StoreBE16( &buffer[buffer_len], rsp->charset[i]);
            buffer_len += 2;

            buffer[buffer_len++] = rsp->attribute_string_len[i];
            OS_MemCopy( (U8 *) &buffer[buffer_len], (const U8 *) rsp->attribute_string, rsp->attribute_string_len[i]);
            buffer_len += rsp->attribute_string_len[i];
        }
        
        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_IMPLEMENTED_STABLE, opcode, buffer_len, buffer);
    }else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPGetPlayerValueTextRsp
 * DESCRIPTION
 *  Convert a get_playerapp_valuetext structure to byte array and
 *  invoke AVRCP_UtilAVRCPCmdBufferRsp to sent it
 * 
 *  The response packet is used for a AV/C status command
 * PARAMETERS
 *  rsp : get_playerapp_valuetext data
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilAVRCPGetPlayerValueTextRsp(bt_avrcp_get_playerapp_valuetext_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 i;
    U8 opcode = AVRCP_SPECIFIC_OPCODE_GET_PLAYERAPP_SETTINGVALUETEXT;    
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling
        buffer[0] = rsp->count;
        if( buffer[0] > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
            buffer[0] = BT_AVRCP_MAX_ATTRIBUTE_NUM;
        }
        buffer_len = 1;

        // spec 6.5.5 page 54
        for( i = 0; i< buffer[0] && i < (U8)BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM && ((int)(buffer_len+4+rsp->value_string_len[i]) < (int)sizeof(buffer) ) ; i++ ){
            buffer[buffer_len++] = rsp->value_ids[i];
            StoreBE16( &buffer[buffer_len], rsp->charset[i]);
            buffer_len += 2;
            
            buffer[buffer_len++] = rsp->value_string_len[i];
            OS_MemCopy( (U8 *) &buffer[buffer_len], (const U8 *) rsp->value_string, rsp->value_string_len[i]);
            buffer_len += rsp->value_string_len[i];
        }
        
        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_IMPLEMENTED_STABLE, opcode, buffer_len, buffer);
    }else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPCmdInformCharsetRsp
 * DESCRIPTION
 *  Convert a inform_display_charset structure to byte array and
 *  invoke AVRCP_UtilAVRCPCmdBufferRsp to sent it
 * 
 *  The response packet is used for a AV/C control command
 * PARAMETERS
 *  rsp : inform_display_charset data
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilAVRCPCmdInformCharsetRsp(bt_avrcp_inform_display_charset_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_SPECIFIC_OPCODE_INFORM_DISPLAY_CHARSET;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling
    	// sepc 6.5.7 page 55 - None parameters
    	buffer_len = 0;
        
        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPCmdInformBattStatusOfCTRsp
 * DESCRIPTION
 *  Convert a inform_battery_statusofct structure to byte array and
 *  invoke AVRCP_UtilAVRCPCmdBufferRsp to sent it
 * 
 *  The response packet is used for a AV/C control command
 * PARAMETERS
 *  rsp : inform_battery_statusofct data
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilAVRCPCmdInformBattStatusOfCTRsp(bt_avrcp_inform_battery_statusofct_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_SPECIFIC_OPCODE_INFORM_BATTERY_STATUSOFCT;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling
    	// sepc 6.5.8 page 56 - None parameters - Inform the CT batter status is changed.
    	buffer_len = 0;
        
        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPCmdGetElementAttributesRsp
 * DESCRIPTION
 *  Convert a get_element_attributes structure to byte array and
 *  invoke AVRCP_UtilAVRCPCmdBufferRsp to sent it
 * 
 *  The response packet is used for a AV/C status command
 * PARAMETERS
 *  rsp : get_element_attributes data
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilAVRCPCmdGetElementAttributesRsp(bt_avrcp_get_element_attributes_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_GET_ELEMENT_ATTRIBUTES;
	AvrcpChnlInfo myChnlInfo;
	U8 i;
	U16 next_value_len;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
        // error handling for PCM. Avoid 0 count
        if( 0 == rsp->count ){
            rsp->count = 1;
            rsp->elements[0].value_len = 1;
            rsp->elements[0].value_text_offset = 0 ;
            rsp->element_data[rsp->elements[0].value_text_offset] = ' ';
            rsp->element_data[rsp->elements[0].value_text_offset+1] = 0;
        }
        
    	// marshalling
    	// sepc 6.6.1 page 58
        buffer[0] = rsp->count;
        if( buffer[0] > BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM ){
            buffer[0] = BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM;
        }
        buffer_len = 1;

        for( i = 0; i< buffer[0]; i++ ){
            // check the remain size
            next_value_len = rsp->elements[i].value_len;
            if( BT_AVRCP_MAX_RSP_FRAME_SIZE < (buffer_len + 4 + 2 + 2 + next_value_len)){
                kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_THROW_MORE_DATA, buffer_len, (4 + 2 + 2 + next_value_len) , BT_AVRCP_MAX_RSP_FRAME_SIZE);
                break;
            }
            if( (rsp->elements[i].value_len + rsp->elements[i].value_text_offset) > sizeof(rsp->element_data)){
            	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_THROW_MORE_DATA, rsp->elements[i].value_text_offset, rsp->elements[i].value_len, sizeof(rsp->element_data) );
                break;
            }
            if( (buffer_len + (4+2+rsp->elements[i].value_len)) > AVRCP_MAX_PACKET_LEN){
                OS_Report("[AVRCP] too many data buffer_len:%d next-attrib:%d max:%d", buffer_len, (4+2+rsp->elements[i].value_len), AVRCP_MAX_PACKET_LEN);   
                break;
            }
            
            StoreBE32( &buffer[buffer_len], rsp->elements[i].attribute_id);
            buffer_len += 4;
            StoreBE16( &buffer[buffer_len], rsp->elements[i].charset);
            buffer_len += 2;

			if( rsp->elements[i].value_len > 512 ){
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_REDUCE_ITEM_LENGTH, i, rsp->elements[i].value_len, 80);
				rsp->elements[i].value_len = 80;
			}
			if( rsp->elements[i].value_text_offset > sizeof(bt_avrcp_get_element_attributes_rsp_struct) ){
				// error 
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_WRONG_ITEM_DATA_OFFSET, i, rsp->elements[i].value_text_offset);
				rsp->elements[i].value_len = 1;
				rsp->elements[i].value_text_offset = 0;
			}
 
            if( rsp->elements[i].value_len != 0 ){
            StoreBE16( &buffer[buffer_len], rsp->elements[i].value_len);
            buffer_len += 2;

			OS_MemCopy( (U8 *) &buffer[buffer_len], (const U8 *) &rsp->element_data[rsp->elements[i].value_text_offset], rsp->elements[i].value_len);
            buffer_len += rsp->elements[i].value_len;
            }else{
                OS_Report("[AVRCP] warning attribute-element is 0 length ");
                rsp->elements[i].value_len = 1;
                StoreBE16( &buffer[buffer_len], rsp->elements[i].value_len);
                buffer_len += 2;
    
    			OS_MemCopy( (U8 *) &buffer[buffer_len], (const U8 *) " ", rsp->elements[i].value_len);
                buffer_len += rsp->elements[i].value_len;
            }
            
        }
        OS_Report("[AVRCP] change count:%d to i:%d buffer_len:%d", buffer[0], i, buffer_len);        
		buffer[0] = i;

        // error handling when no any attribute. GetElementAttributes in spec should be 1~255
        if( buffer[0] == 0 ){
           OS_Report("[AVRCP] no element ! regarding to spec. append a 1-length empty space song title return ");
           buffer[0] = 1; // spec define
           StoreBE32( &buffer[buffer_len], 0x1); // title
           buffer_len += 4;
           StoreBE16( &buffer[buffer_len], 0x6a); //UTF8
           buffer_len += 2;
           StoreBE16( &buffer[buffer_len], 1);
           buffer_len += 2;
           buffer[buffer_len] = ' ';
           buffer_len++;
        }

        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_IMPLEMENTED_STABLE, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPCmdGetPlayerStatusRsp
 * DESCRIPTION
 *  Convert a get_playstatus structure to byte array and
 *  invoke AVRCP_UtilAVRCPCmdBufferRsp to sent it
 * 
 *  The response packet is used for a AV/C status command
 * PARAMETERS
 *  rsp : get_playstatus data
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilAVRCPCmdGetPlayerStatusRsp(bt_avrcp_get_playerstatus_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
	AvrcpChnlInfo myChnlInfo;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_GET_PLAYERAPP_STATUS;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling
    	// sepc 6.5.4 page 9
    	buffer_len = 0;

        StoreBE32( &buffer[buffer_len], rsp->song_length);
        buffer_len += 4;
        StoreBE32( &buffer[buffer_len], rsp->song_position);
        buffer_len += 4;
        buffer[buffer_len++] = rsp->play_status;
        
        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_IMPLEMENTED_STABLE, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilAVRCPCmdListPlayerValuesRsp
 * DESCRIPTION
 *  Convert a get_curplayerapp structure to byte array and
 *  invoke AVRCP_UtilAVRCPCmdBufferRsp to sent it
 * 
 *  The response packet is used for a AV/C status command
 * PARAMETERS
 *  rsp : get_curplayerapp data
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(bt_avrcp_register_notification_rsp_struct *rsp, U16 data_len, U8 *data){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
	AvrcpChnlInfo myChnlInfo;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_REGISTER_NOFITICATION;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	// marshalling spec 6.7.2
	buffer_len = data_len;
	if( buffer_len > BT_AVRCP_MAX_RSP_FRAME_SIZE ){
	    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_MARSHAL_BUFFER_WRN, buffer_len, BT_AVRCP_MAX_RSP_FRAME_SIZE);
	    buffer_len = BT_AVRCP_MAX_RSP_FRAME_SIZE;
    }
	if( buffer_len > 0 ){
    	OS_MemCopy( buffer, data, buffer_len );
    }
    
	if( TRUE == rsp->is_interim  ){
		AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_INTERIM, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_CHANGED, opcode, buffer_len, buffer);
	}
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_PlaySettingChange(bt_avrcp_register_notification_rsp_struct *rsp){
    U8 i, len, data[2 + (BT_AVRCP_MAX_ATTRIBUTE_NUM*2)];
    
    len = 0;
    data[len++] = rsp->event_id;
    data[len++] = rsp->count;
    for(i =0 ; i < (U8) rsp->count && (len+2) < (U8) sizeof(data) ; i++ ){
        data[len++] = rsp->attribute_id[i];
        data[len++] = rsp->attribute_value[i];
        if( len > sizeof(data) ){
            break;
        }
    }
    AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(rsp, len, data);
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_PlayBackChange(bt_avrcp_register_notification_rsp_struct *rsp){
	// marshalling spec 6.7.2
    U8 len, data[2];
    
    len = 0;
    data[len++] = rsp->event_id;
    data[len++] = rsp->status;
    AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(rsp, len, data);
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_TrackChange(bt_avrcp_register_notification_rsp_struct *rsp){
	// marshalling spec 6.7.2
    U8 len, data[9];
    
    len = 0;
    data[len++] = rsp->event_id;
    len += 8;
    OS_MemCopy( &data[1], rsp->identifier, 8);
        
    AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(rsp, len, data);
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_ReachedEnd(bt_avrcp_register_notification_rsp_struct *rsp){
	// marshalling spec 6.7.2
    U8 len, data[9];
    
    len = 0;
    data[len++] = rsp->event_id;
        
    AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(rsp, len, data);
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_ReachedStart(bt_avrcp_register_notification_rsp_struct *rsp){
	// marshalling spec 6.7.2
    U8 len, data[9];
    
    len = 0;
    data[len++] = rsp->event_id;
        
    AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(rsp, len, data);
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_PosChanged(bt_avrcp_register_notification_rsp_struct *rsp){
	// marshalling spec 6.7.2
    U8 len, data[9];
    
    len = 0;
    data[len++] = (U8) rsp->event_id;
    StoreBE32( &data[len], rsp->pos );
    len += 4;
        
    AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(rsp, len, data);
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_BattChanged(bt_avrcp_register_notification_rsp_struct *rsp){
	// marshalling spec 6.7.2
    U8 len, data[9];
    
    len = 0;
    data[len++] = (U8) rsp->event_id;
    data[len++] = (U8) rsp->status;

    AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(rsp, len, data);
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_SystemChanged(bt_avrcp_register_notification_rsp_struct *rsp){
	// marshalling spec 6.7.2
    U8 len, data[9];
    
    len = 0;
    data[len++] = (U8) rsp->event_id;
    data[len++] = (U8) rsp->status;

    AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(rsp, len, data);
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_VolumeChanged(bt_avrcp_register_notification_rsp_struct *rsp){
	// marshalling spec 6.7.2
    U8 len, data[9];
    
    len = 0;
    data[len++] = (U8) rsp->event_id;
    data[len++] = (U8) rsp->volume;

    AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(rsp, len, data);
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_AddressedPlayerChanged(bt_avrcp_register_notification_rsp_struct *rsp){
	// marshalling spec 6.7.2
    U8 len, data[9];
    
    len = 0;
    data[len++] = (U8) rsp->event_id;
    
    StoreBE16( &data[len], rsp->id );
    len += 2;
    StoreBE16( &data[len], rsp->uid_counter );
    len += 2;    

    AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(rsp, len, data);
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_AvailPlayerChanged(bt_avrcp_register_notification_rsp_struct *rsp){
	// marshalling spec 6.7.2 page 70
    U8 len, data[9];
    
    len = 0;
    // no parameter
    data[len++] = (U8) rsp->event_id;

    AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(rsp, len, data);
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_UIDSChanged(bt_avrcp_register_notification_rsp_struct *rsp){
	// marshalling spec 6.7.2
    U8 len, data[9];
    
    len = 0;
    data[len++] = (U8) rsp->event_id;
    StoreBE16( &data[len], rsp->uid_counter );// uid_counter (2B)
    len += 2;     

    AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(rsp, len, data);
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp_PlayingContentChanged(bt_avrcp_register_notification_rsp_struct *rsp){
	// marshalling spec 6.7.2
    U8 len, data[9];
    
    len = 0;
    data[len++] = (U8) rsp->event_id;
	// NO content

    AVRCP_UtilAVRCPCmdRegisterNotificationRsp_common(rsp, len, data);
}

void AVRCP_UtilAVRCPCmdRegisterNotificationRsp(bt_avrcp_register_notification_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
		// marshalling
		switch( rsp->event_id ){
			case AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED:
				AVRCP_UtilAVRCPCmdRegisterNotificationRsp_PlayBackChange(rsp);
			break;
			case AVRCP_NOTIFY_EVENT_TRACK_CHANGED:
				AVRCP_UtilAVRCPCmdRegisterNotificationRsp_TrackChange(rsp);
			break;
			case AVRCP_NOTIFY_EVENT_TRACK_REACHED_END:
				AVRCP_UtilAVRCPCmdRegisterNotificationRsp_ReachedEnd(rsp);
			break;
			case AVRCP_NOTIFY_EVENT_TRACK_REACHED_START:
				AVRCP_UtilAVRCPCmdRegisterNotificationRsp_ReachedStart(rsp);
			break;
			case AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED:
				AVRCP_UtilAVRCPCmdRegisterNotificationRsp_PosChanged(rsp);
			break;
			case AVRCP_NOTIFY_EVENT_BATT_STATUS_CHANGED:
				AVRCP_UtilAVRCPCmdRegisterNotificationRsp_BattChanged(rsp);
			break;
			case AVRCP_NOTIFY_EVENT_SYSTEM_STATUS_CHANGED:
				AVRCP_UtilAVRCPCmdRegisterNotificationRsp_SystemChanged(rsp);
			break;
			case AVRCP_NOTIFY_EVENT_PLAYER_APPLICATION_SETTING_CHANGED:
				AVRCP_UtilAVRCPCmdRegisterNotificationRsp_PlaySettingChange(rsp);
			break;
			case AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED:
				AVRCP_UtilAVRCPCmdRegisterNotificationRsp_PlayingContentChanged(rsp);
			break;
			case AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED:
				AVRCP_UtilAVRCPCmdRegisterNotificationRsp_AvailPlayerChanged(rsp);
			break;
			case AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED:
				AVRCP_UtilAVRCPCmdRegisterNotificationRsp_AddressedPlayerChanged(rsp);
			break;
			case AVRCP_NOTIFY_EVENT_UIDS_CHANGED:
				AVRCP_UtilAVRCPCmdRegisterNotificationRsp_UIDSChanged(rsp);
			break;
			case AVRCP_NOTIFY_EVENT_VOLUME_CHANGED:
				AVRCP_UtilAVRCPCmdRegisterNotificationRsp_VolumeChanged(rsp);
			break;
			default:
			    AVRCP_UtilSendAVRCPReject( &myChnlInfo, AVRCP_SPECIFIC_OPCODE_REGISTER_NOFITICATION, BT_AVRCP_ERRCODE_INVALID_PARAM);
				break;
		}

	}else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, AVRCP_SPECIFIC_OPCODE_REGISTER_NOFITICATION, rsp->error_code);
	}
}

void AVRCP_UtilAVRCPCmdAbortContinueResponseRsp(bt_avrcp_abort_continueresponse_rsp_struct *rsp){
    
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_SPECIFIC_OPCODE_ABORT_CONTINUING;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

    // spec 6.8.2 page 65
    AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
}

void AVRCP_UtilAVRCPCmdSetAbsoluteVolumeRsp(bt_avrcp_set_absolute_volume_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_SPECIFIC_OPCODE_SET_ABSOLUTE_VOLUME;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling spec 6.13.2.1 page 91
    	buffer[buffer_len++] = rsp->volume;
        
        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

void AVRCP_UtilAVRCPCmdSetAddressedPlayerRsp(bt_avrcp_set_addressedplayer_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_SPECIFIC_OPCODE_SET_ADDRESSEDPLAYER;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling spec 6.9.1.2
    	// buffer[buffer_len++] = rsp->status;
    	buffer[buffer_len++] = BT_AVRCP_ERRCODE_OPERATION_COMPLETE; //Avoid Sending wrong state
        
        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

void AVRCP_UtilAVRCPCmdGetPlayerItemsRsp(bt_avrcp_get_playeritems_rsp_struct *rsp){
/// Browse cmd
/// Make sure the buffer size is less then max length of l2cap payload
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
	U32 remote_mtu;
    U8 opcode = AVRCP_BROWSE_OPCODE_GET_FOLDERITEMS; // scope is 0
	AvrcpChnlInfo myChnlInfo;
	U32 i;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	remote_mtu = AVRTP_UtilGetBrowseMtu(rsp->chnl_num);
    	
    	// marshalling spec 6.10.4.2 page 85
    	// buffer[buffer_len++] = rsp->status;
    	buffer[buffer_len++] = BT_AVRCP_ERRCODE_OPERATION_COMPLETE; //Avoid Sending wrong state
        StoreBE16(&buffer[buffer_len], rsp->uid_counter);
    	buffer_len += 2;
        StoreBE16(&buffer[buffer_len], rsp->counter);
    	buffer_len += 2;

    	/// spec 6.10.2.1 page 74 - media player item
		i = 0;
		while( i < rsp->counter && remote_mtu > ( (U32)buffer_len + 14 + rsp->players[i].name_len ) ) {
            // 31 = 1 + 2 + 2 + 4 + 16 + 2 + 2  ( name_len is variable )
			if( ((U32)buffer_len + 31 + rsp->players[i].name_len)> BT_AVRCP_MAX_RSP_FRAME_SIZE){
				break;
			}
			// add it to buffer
			buffer[buffer_len++] = rsp->players[i].item_type;
            StoreBE16(&buffer[buffer_len], rsp->players[i].item_len);
        	buffer_len += 2;
            StoreBE16(&buffer[buffer_len], rsp->players[i].player_id);
        	buffer_len += 2;
        	buffer[buffer_len++] = rsp->players[i].major_play_type;
            OS_MemCopy(&buffer[buffer_len], rsp->players[i].player_sub_type, 4);
        	buffer_len += 4;
        	buffer[buffer_len++] = rsp->players[i].play_status;
        	OS_MemCopy(&buffer[buffer_len], rsp->players[i].feature_bitmask, 16);
        	buffer_len += 16;
            if( rsp->players[i].charset == 0 ){
                rsp->players[i].charset = 0x6A; /* UTF8 */
                Report(("AVRCP: charset to 0x6a"));
            }            
            StoreBE16(&buffer[buffer_len], rsp->players[i].charset);
        	buffer_len += 2;
        	if( rsp->players[i].name_len > BT_AVRCP_MAX_PLAYER_NAME_SIZE ){
        	    rsp->players[i].name_len = BT_AVRCP_MAX_PLAYER_NAME_SIZE;
        	}
            StoreBE16(&buffer[buffer_len], rsp->players[i].name_len);
        	buffer_len += 2;
        	if( (buffer_len + rsp->players[i].name_len) < BT_AVRCP_MAX_RSP_FRAME_SIZE){
        	OS_MemCopy(&buffer[buffer_len], rsp->players[i].name, rsp->players[i].name_len);
        	buffer_len += rsp->players[i].name_len;
        	} 
       	
			i++;
			if( i >= rsp->counter ){
				break; // reach the last player record
			}
		}

        // if the num_of_item is too much, put it with the actual number 
        if( i != rsp->counter ){
            StoreBE32(&buffer[3], i);
        }

        AVRCP_UtilAVRCPBrowseCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPBrowseReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

void AVRCP_UtilAVRCPCmdGetFolderItemsRsp(bt_avrcp_get_folderitems_rsp_struct *rsp){
/// Browse cmd
/// Make sure the buffer size is less then max length of l2cap payload
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0, counter_offset;
    U8  opcode = AVRCP_BROWSE_OPCODE_GET_FOLDERITEMS;
	AvrcpChnlInfo myChnlInfo;
	U16 remote_mtu, i, handled_offset, j;
	U8 stop = FALSE;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
	    remote_mtu = AVRTP_UtilGetBrowseMtu(rsp->chnl_num);
	
    	// marshalling spec 6.10.4.2.1 page 85
        // buffer[buffer_len++] = rsp->status;
        buffer[buffer_len++]= BT_AVRCP_ERRCODE_OPERATION_COMPLETE; //Avoid MMI send a wrong state
        StoreBE16(&buffer[buffer_len], rsp->uid_counter);
    	buffer_len += 2;
    	counter_offset = buffer_len;
        StoreBE16(&buffer[buffer_len], rsp->counter);
    	buffer_len += 2;

        // browse packet size
        remote_mtu = AVRTP_UtilGetBrowseMtu(rsp->chnl_num);
    
        // player, folder, element items page 74 & 77 & 78
        i = 0;
        stop = FALSE;
        handled_offset = 0;
        for( i = 0; i < rsp->counter && FALSE == stop; i++){
            /// 3 is the element type(1B) and length field(2B)
            if( buffer_len + (rsp->items[i].item_length + 3)  > remote_mtu ){
                kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_THROW_MORE_DATA, buffer_len, rsp->items[i].item_length, remote_mtu);
                if( i > 0){
                    StoreBE16(&buffer[counter_offset], (U16)i);
                }else{
                    StoreBE16(&buffer[counter_offset], (U16)0);
                }
                break;
            }
                
            switch( rsp->items[i].item_type ){
                case 0x01: // play item
                {
                    // 1 + 2 + 2 + 1 + 4 + 1  + 16 + 2 + 2 + (name_length)
                    if( (buffer_len + 31 + rsp->items[i].name_length) > BT_AVRCP_MAX_RSP_FRAME_SIZE ){
                        stop = TRUE;
                        break;
                    }
                    buffer[buffer_len++] = rsp->items[i].item_type;
                    StoreBE16(&buffer[buffer_len], rsp->items[i].item_length);
                	buffer_len += 2;
                	StoreBE16(&buffer[buffer_len], rsp->items[i].player_id);
                	buffer_len += 2;
                	if( 0 == rsp->items[i].specific_type ){
                	    rsp->items[i].specific_type = 1;
                	    Report(("AVRCP: charset to audio type"));
                	}
                	buffer[buffer_len++] = rsp->items[i].specific_type;
                	StoreBE32(&buffer[buffer_len], rsp->items[i].sub_type);
                	buffer_len += 4;
                	buffer[buffer_len++] = rsp->items[i].status;
                	OS_MemCopy(&buffer[buffer_len], rsp->items[i].mask, 16);
                	buffer_len += 16;
                	if( 0 == rsp->items[i].charset ){
                	    rsp->items[i].charset = 0x6A;
                	    Report(("AVRCP: charset to 0x6A"));
                	}
                	StoreBE16(&buffer[buffer_len], rsp->items[i].charset);
                	buffer_len += 2;
                	StoreBE16(&buffer[buffer_len], rsp->items[i].name_length);
                	buffer_len += 2;
                	if( (buffer_len + rsp->items[i].name_length) < BT_AVRCP_MAX_RSP_FRAME_SIZE ){
                	OS_MemCopy(&buffer[buffer_len], (const U8 *) &rsp->data[rsp->items[i].name_offset], rsp->items[i].name_length);
                	buffer_len += rsp->items[i].name_length;
                	}
                	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ENCODE_FOLDER_ITEM, rsp->items[i].item_type,rsp->items[i].item_length, buffer_len);
                }
                break;
                case 0x02: // folder item
                {
					// spec 6.10.2.2 page 77
                    buffer[buffer_len++] = rsp->items[i].item_type;
                    StoreBE16(&buffer[buffer_len], rsp->items[i].item_length);
                	buffer_len += 2;
                	OS_MemCopy(&buffer[buffer_len], rsp->items[i].uid, 8);
                	buffer_len += 8;
                    buffer[buffer_len++] = rsp->items[i].specific_type;
                    buffer[buffer_len++] = rsp->items[i].is_playable;
                	StoreBE16(&buffer[buffer_len], rsp->items[i].charset);
                	buffer_len += 2;
                	StoreBE16(&buffer[buffer_len], rsp->items[i].name_length);
                	buffer_len += 2;
                	OS_MemCopy(&buffer[buffer_len], (const U8 *) &rsp->data[rsp->items[i].name_offset], rsp->items[i].name_length);
                	buffer_len += rsp->items[i].name_length;                
                	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ENCODE_FOLDER_ITEM, rsp->items[i].item_type,rsp->items[i].item_length, buffer_len);
                }
                break;
                case 0x03: // element ietm
                {
					U16 attribute_count_offset;
					U16 attr_value_offset;

					// spec 6.10.2.3 page 78
                    buffer[buffer_len++] = rsp->items[i].item_type;
                    StoreBE16(&buffer[buffer_len], rsp->items[i].item_length);
                	buffer_len += 2;
                	OS_MemCopy(&buffer[buffer_len], rsp->items[i].uid, 8);
                	buffer_len += 8;                	
                	buffer[buffer_len++] = rsp->items[i].specific_type;
                	StoreBE16(&buffer[buffer_len], rsp->items[i].charset);
                	buffer_len += 2;
                    if( rsp->items[i].name_length > BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE ){
                        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_TOO_LARGE_LENGTH, rsp->items[i].name_length, BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE);
                        rsp->items[i].name_length = BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE;
                    }
                    if( rsp->items[i].name_offset >  sizeof(rsp->data) ){
                        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_WRONG_DATA_OFFSET, rsp->items[i].name_offset);
                        rsp->items[i].name_length = 0;
                    }
                	StoreBE16(&buffer[buffer_len], rsp->items[i].name_length);
                	buffer_len += 2;
                	if( rsp->items[i].name_length > 0 ){
                	OS_MemCopy(&buffer[buffer_len], (const U8 *)&rsp->data[rsp->items[i].name_offset], rsp->items[i].name_length);
                	buffer_len += rsp->items[i].name_length;
                	}

					// loop for attribute value entry
					attribute_count_offset = buffer_len; // temp save the count index for changing count by actual numer
					if( rsp->items[i].attribute_count > BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID){
						rsp->items[i].attribute_count = 0; // invalid value
					}

					buffer[buffer_len++] = rsp->items[i].attribute_count;
					for( j = 0; j < rsp->items[i].attribute_count; j++){
						StoreBE32(&buffer[buffer_len], rsp->items[i].attributes[j].attribute_id);
                		buffer_len += 4;
						StoreBE16(&buffer[buffer_len], rsp->items[i].attributes[j].charset);
                		buffer_len += 2;
						StoreBE16(&buffer[buffer_len], rsp->items[i].attributes[j].attribute_value_len);
                		buffer_len += 2;
						
						attr_value_offset = rsp->items[i].attributes[j].attribute_value_string_offset;
						if( attr_value_offset > sizeof(rsp->data)){
							// invalid offset
							StoreBE16(&buffer[buffer_len-2], 0);
							break;
						}
						OS_MemCopy(&buffer[buffer_len], (const U8 *)&rsp->data[attr_value_offset], rsp->items[i].attributes[j].attribute_value_len);
						buffer_len += rsp->items[i].attributes[j].attribute_value_len;
					}
					if( j > 0 && j != (U16)(buffer[attribute_count_offset]) ){
						// not enough space to save attribute. adjust the attribute count
						buffer[attribute_count_offset] = (U8) j;
					}
                	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ENCODE_FOLDER_ITEM, rsp->items[i].item_type,rsp->items[i].item_length, buffer_len);
                }
                break;
                default:
                {
                    stop = TRUE;
                    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ENCODE_FOLDER_ITEM_FAIL, rsp->items[i].item_type );
                    AvrcpAssert(0);
                }
                break;
            }
        }
        
        AVRCP_UtilAVRCPBrowseCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPBrowseReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

void AVRCP_UtilAVRCPCmdSetBrowsedPlayerRsp(bt_avrcp_set_browsedplayer_rsp_struct *rsp){
/// Browse cmd
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0, i = 0;
    U8 opcode = AVRCP_BROWSE_OPCODE_SET_BROWSEDPLAYER;
	AvrcpChnlInfo myChnlInfo;
	U32 remote_mtu;	
	U16 size_offset = 9;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling spec 6.9.3 page 68
    	//buffer[buffer_len++] = rsp->status;
  	    buffer[buffer_len++] = BT_AVRCP_ERRCODE_OPERATION_COMPLETE; //Avoid Sending wrong state
        StoreBE16(&buffer[buffer_len], rsp->uid_counter);
    	buffer_len += 2;
        StoreBE32(&buffer[buffer_len], rsp->num_of_items);
    	buffer_len += 4;
        StoreBE16(&buffer[buffer_len], rsp->char_set);
    	buffer_len += 2;
		size_offset = buffer_len;
    	buffer[buffer_len++] = rsp->folder_depth;

        // browse packet size
        remote_mtu = AVRTP_UtilGetBrowseMtu(rsp->chnl_num);

		for( i = 0; i < rsp->folder_depth && i < BT_AVRCP_MAX_FOLDER_DEPTH_NUM ; i++ ){
			if( ( buffer_len+rsp->foldername_len[i]) > (U16)remote_mtu ){
				rsp->foldername_len[i] = 1;
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_NO_MORE_DATA_SPACE);
			}

		    if( (remote_mtu > (U32) 48) && ((U32)buffer_len + rsp->foldername_len[i]) > remote_mtu ){
	            // too much data
				break;
			}
            if(  (buffer_len+2+rsp->foldername_len[i]) > BT_AVRCP_MAX_RSP_FRAME_SIZE ){
                // too much data for buffer
                break;
            }

            if(  (U32)(buffer_len+2+rsp->foldername_len[i]) > remote_mtu ){
                // too much data for buffer
                break;
			}

			if( rsp->foldername_offset[i] >= sizeof(rsp->folder_data) ){
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_WRONG_DATA_OFFSET, rsp->foldername_offset[i] );
				break;
			}
			
			StoreBE16(&buffer[buffer_len], rsp->foldername_len[i]);
    		buffer_len += 2;

			OS_MemCopy( (U8 *) &buffer[buffer_len], (const U8 *) &rsp->folder_data[ rsp->foldername_offset[i] ], rsp->foldername_len[i]);
			buffer_len += rsp->foldername_len[i];
		}

		if( i > 0 && i != rsp->folder_depth){
			buffer[size_offset] = i-1; // too many depth
		}
        
        AVRCP_UtilAVRCPBrowseCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPBrowseReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

void AVRCP_UtilAVRCPCmdChangePathRsp(bt_avrcp_change_path_rsp_struct *rsp){
/// Browse cmd
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_BROWSE_OPCODE_CHANGE_PATH;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling spec 6.10.4 page 83
    	// buffer[buffer_len++] = rsp->status;
    	buffer[buffer_len++] = BT_AVRCP_ERRCODE_OPERATION_COMPLETE; //Avoid sending wrong status
        StoreBE32(&buffer[buffer_len], rsp->num_of_items);
    	buffer_len += 4;
  
        AVRCP_UtilAVRCPBrowseCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPBrowseReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

void AVRCP_UtilAVRCPCmdGetItemAttributesRsp(bt_avrcp_get_itemattributes_rsp_struct *rsp){
/// Browse cmd
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_BROWSE_OPCODE_GET_ITEMATTRIBUTES;
	AvrcpChnlInfo myChnlInfo;
	U32 remote_mtu, i;

	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling spec 6.10.4 page 83
    	// buffer[buffer_len++] = rsp->status;
    	buffer[buffer_len++] = BT_AVRCP_ERRCODE_OPERATION_COMPLETE; //Avoid Sending wrong state
    	buffer[buffer_len++] = rsp->counter;
    	
        remote_mtu = AVRTP_UtilGetBrowseMtu(rsp->chnl_num);
        
        // range check
        i = 0;
        while( i < rsp->counter && i < BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE ){
        	if( rsp->attribute_value_len[i] > BT_AVRCP_MAX_VALUE_STRING_SIZE ){
        	    rsp->attribute_value_len[i] = BT_AVRCP_MAX_VALUE_STRING_SIZE;
        	}            
            i++;
        }
        
		i = 0;
        while( i < rsp->counter && i < BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE &&  remote_mtu > ( (U32)buffer_len + 8 + rsp->attribute_value_len[i] ) ) {
            if( BT_AVRCP_MAX_RSP_FRAME_SIZE < ((U32)buffer_len + 8 + rsp->attribute_value_len[i]) ){
                break;
            }
			// add it to buffer
            StoreBE32(&buffer[buffer_len], rsp->attribute_id[i]);
        	buffer_len += 4;
            StoreBE16(&buffer[buffer_len], rsp->charset[i]);
        	buffer_len += 2;
            StoreBE16(&buffer[buffer_len], rsp->attribute_value_len[i] );
        	buffer_len += 2;
        	
        	if( rsp->attribute_value_len[i] > BT_AVRCP_MAX_VALUE_STRING_SIZE ){
        	    rsp->attribute_value_len[i] = BT_AVRCP_MAX_VALUE_STRING_SIZE;
        	}
        	OS_MemCopy(&buffer[buffer_len], rsp->attribute_value_string[i], rsp->attribute_value_len[i]);
        	buffer_len += rsp->attribute_value_len[i];
       	
			i++;
			if( i >= rsp->counter ){
				break; // reach the last player record
			}
		}

        // if the num_of_item is too much, put it with the actual number 
        if( i != rsp->counter ){
            buffer[1] = rsp->counter;
        }
    	
        AVRCP_UtilAVRCPBrowseCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPBrowseReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

void AVRCP_UtilAVRCPCmdPlayItemsRsp(bt_avrcp_play_items_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_SPECIFIC_OPCODE_PLAY_ITEMS;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling spec 6.12.1 page 88
        // buffer[buffer_len++] = rsp->status;
    	buffer[buffer_len++] = BT_AVRCP_ERRCODE_OPERATION_COMPLETE; //Avoid Sending wrong state
        
        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

void AVRCP_UtilAVRCPCmdSearchRsp(bt_avrcp_search_rsp_struct *rsp){
/// Browse cmd
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_BROWSE_OPCODE_SEARCH;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling spec 6.11.1 page 87
    	// buffer[buffer_len++] = rsp->status;
    	buffer[buffer_len++] = BT_AVRCP_ERRCODE_OPERATION_COMPLETE; //Avoid Sending wrong state
        StoreBE16(&buffer[buffer_len], rsp->uid_counter);
    	buffer_len += 2;
        StoreBE32(&buffer[buffer_len], rsp->found_num_of_items);
    	buffer_len += 4;

        AVRCP_UtilAVRCPBrowseCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPBrowseReject( &myChnlInfo, opcode, rsp->error_code);
	}
}

void AVRCP_UtilAVRCPCmdAddToNowPlayingRsp(bt_avrcp_add_tonowplaying_rsp_struct *rsp){
	U8 *buffer = g_avrcp_util_marshall_buffer;
    U16 buffer_len = 0;
    U8 opcode = AVRCP_SPECIFIC_OPCODE_ADD_TONOWPLAYING;
	AvrcpChnlInfo myChnlInfo;
	Assert(rsp);

	myChnlInfo.chnl_num = rsp->chnl_num;
	myChnlInfo.transId = rsp->seq_id;

	if( FALSE == rsp->reject ){
    	// marshalling spec 6.12.2.1 page 90
        buffer[buffer_len++] = BT_AVRCP_ERRCODE_OPERATION_COMPLETE; //Avoid Sending wrong state
        
        AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_RESPONSE_ACCEPTED, opcode, buffer_len, buffer);
	}else{
		AVRCP_UtilSendAVRCPReject( &myChnlInfo, opcode, rsp->error_code);
	}
}


/***** AVRCP control msg *****/

void AVRCP_UtilAVRCPSendCmd_FailReport(AvrcpChnlInfo *pChnlInfo, U8 opcode, U16 result, U8 error_code){
    switch( opcode ){
        case AVRCP_SPECIFIC_OPCODE_GET_CAPABILITIES:
        {
    		bt_avrcp_get_capabilities_cnf_struct *pcnf;
    		U8 event = BTAVRCP_EVENT_GET_CAPABILITIES_CNF;

            pcnf = (bt_avrcp_get_capabilities_cnf_struct *) &g_avrcp_data.p.ABTAVRCPGetCapabilitiesCnf;
    		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_get_capabilities_cnf_struct));

    		pcnf->result = result;
    		pcnf->error_code = error_code;

			AvrcpHandleAVRCPCnf_common( pChnlInfo, pcnf->result, event, sizeof(bt_avrcp_get_capabilities_cnf_struct), (U8 *)pcnf);
        }
        break;
		default:
			AvrcpAssert(0);
			break;
    }
    
}
void AVRCP_UtilAVRCPSendCmd_Get_Capabilities_Req(bt_avrcp_get_capabilities_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_GET_CAPABILITIES;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;
	
	// marshalling page 48
	buffer[buffer_len++] = req_p->capability_id;
        
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_STATUS, opcode, buffer_len, buffer) ){
        AVRCP_UtilAVRCPSendCmd_FailReport(&myChnlInfo, opcode, BT_AVRCP_RESULT_INTERNAL_ERROR, BT_AVRCP_ERRCODE_INTERNAL_ERROR);
	}

}

void AVRCP_UtilAVRCPSendCmd_List_Playerapp_Attribute_Req(bt_avrcp_list_playerapp_attribute_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_LIST_PLAYERAPP_SETTINGATTRIBUTE;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;
	
	// marshalling page 50 - no parameter
        
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_STATUS, opcode, buffer_len, buffer) ){
    	bt_avrcp_list_playerapp_attribute_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGATTRIBUTE_CNF;

        pcnf = (bt_avrcp_list_playerapp_attribute_cnf_struct *) &g_avrcp_data.p.ABTAVRCPListPlayerappAttributeCnf;
		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_list_playerapp_attribute_cnf_struct));
   	
    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_list_playerapp_attribute_cnf_struct), (U8 *)pcnf);
	    
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

void AVRCP_UtilAVRCPSendCmd_List_Playerapp_Value_Req(bt_avrcp_list_playerapp_value_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_LIST_PLAYERAPP_SETTINGVALUE;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;
	
	// marshalling page 50
	buffer[buffer_len++] = req_p->attribute_id;
        
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_STATUS, opcode, buffer_len, buffer) ){
    	bt_avrcp_list_playerapp_value_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGVALUE_CNF;

        pcnf = (bt_avrcp_list_playerapp_value_cnf_struct *) &g_avrcp_data.p.ABTAVRCPListPlayerappValueCnf;
		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_list_playerapp_value_cnf_struct));
    	
    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_list_playerapp_value_cnf_struct), (U8 *)pcnf);
	    
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}

}

void AVRCP_UtilAVRCPSendCmd_Get_Curplayerapp_Value_Req(bt_avrcp_get_curplayerapp_value_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0, i;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_GET_CURPLAYERAPP_SETTINGVALUE;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;
	
	// marshalling page 51
	buffer[buffer_len++] = req_p->count;
	for( i = 0 ; i< req_p->count && i < BT_AVRCP_MAX_ATTRIBUTE_NUM ; i++){
	    buffer[buffer_len++] = req_p->attribute_ids[i];	
	}
        
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_STATUS, opcode, buffer_len, buffer) ){
    	bt_avrcp_get_curplayerapp_value_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_GET_CURPLAYERAPP_SETTINGVALUE_CNF;
		
        pcnf = (bt_avrcp_get_curplayerapp_value_cnf_struct *) &g_avrcp_data.p.ABTAVRCPGetCURPlayerValueCnf;
		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_get_curplayerapp_value_cnf_struct));
    	
    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_get_curplayerapp_value_cnf_struct), (U8 *)pcnf);
	    
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}

}

void AVRCP_UtilAVRCPSendCmd_Set_Playerapp_Value_Req(bt_avrcp_set_playerapp_value_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0, i;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_SET_PLAYERAPP_SETTINGVALUE;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;
	
	// marshalling page 52
	buffer[buffer_len++] = req_p->count;
	for( i = 0 ; i< req_p->count && i < BT_AVRCP_MAX_ATTRIBUTE_NUM ; i++){
		// buffer index check. check the left buffer space
		if( (buffer_len+2) >= BT_AVRCP_MAX_RSP_FRAME_SIZE ){
			break;
		}
		buffer[buffer_len++] = req_p->attribute_ids[i];
		buffer[buffer_len++] = req_p->values[i];
	}
        
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_CONTROL, opcode, buffer_len, buffer) ){
    	bt_avrcp_set_playerapp_value_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_SET_PLAYERAPP_SETTINGVALUE_CNF;
        pcnf = (bt_avrcp_set_playerapp_value_cnf_struct *) &g_avrcp_data.p.ABTAVRCPSetPlayerValueCnf;
    	
    	OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_set_playerapp_value_cnf_struct));
    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_set_playerapp_value_cnf_struct), (U8 *)pcnf);
	    
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}

}

void AVRCP_UtilAVRCPSendCmd_Get_Playerapp_Attributetext_Req(bt_avrcp_get_playerapp_attributetext_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0, i;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_GET_PLAYERAPP_SETTINGATTRIBUTETEXT;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;
	
	// marshalling page 53
	buffer[buffer_len++] = req_p->count;
	for( i = 0 ; i< req_p->count && i < BT_AVRCP_MAX_ATTRIBUTE_NUM ; i++){
		buffer[buffer_len++] = req_p->attribute_ids[i];
	}
        
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_STATUS, opcode, buffer_len, buffer) ){
    	bt_avrcp_get_playerapp_attributetext_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGATTRIBUTETEXT_CNF;
        pcnf = (bt_avrcp_get_playerapp_attributetext_cnf_struct *) &g_avrcp_data.p.ABTAVRCPGetPlayerAttrTextCnf;
    	
    	OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_get_playerapp_attributetext_cnf_struct));
    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_get_playerapp_attributetext_cnf_struct), (U8 *)pcnf);
	    
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}

}

void AVRCP_UtilAVRCPSendCmd_Get_Playerapp_Valuetext_Req(bt_avrcp_get_playerapp_valuetext_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0, i;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_GET_PLAYERAPP_SETTINGVALUETEXT;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;
	
	// marshalling page 54
	buffer[buffer_len++] = req_p->attribute_id;
	buffer[buffer_len++] = req_p->count;
	for( i = 0 ; i< req_p->count && i < BT_AVRCP_MAX_ATTRIBUTE_NUM ; i++){
    	buffer[buffer_len++] = req_p->value_ids[i];
	}
        
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_STATUS, opcode, buffer_len, buffer) ){
    	bt_avrcp_get_playerapp_valuetext_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGVALUETEXT_CNF;
        pcnf = (bt_avrcp_get_playerapp_valuetext_cnf_struct *) &g_avrcp_data.p.ABTAVRCPGetPlayerValueTextCnf;
    	
    	OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_get_playerapp_valuetext_cnf_struct));
    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_get_playerapp_valuetext_cnf_struct), (U8 *)pcnf);
	    
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}

}

void AVRCP_UtilAVRCPSendCmd_Inform_Display_Charset_Req(bt_avrcp_inform_display_charset_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0, i;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_INFORM_DISPLAY_CHARSET;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;
	
	// marshalling spec 6.12.2.1 page 90
	buffer[buffer_len++] = req_p->count;
	for( i = 0 ; i< req_p->count && i < BT_AVRCP_MAX_ATTRIBUTE_NUM ; i++){
		StoreBE16( &buffer[buffer_len], req_p->charset_ids[i]);
		buffer_len += 2;
	}
        
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_CONTROL, opcode, buffer_len, buffer) ){
    	bt_avrcp_inform_display_charset_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_INFORM_DISPLAY_CHARSET_CNF;
		pcnf = (bt_avrcp_inform_display_charset_cnf_struct *) &g_avrcp_data.p;
    	
    	OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_inform_display_charset_cnf_struct));
    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_inform_display_charset_cnf_struct), (U8 *)pcnf);
	    
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

void AVRCP_UtilAVRCPSendCmd_Inform_Battery_Statusofct_Req(bt_avrcp_inform_battery_statusofct_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_INFORM_BATTERY_STATUSOFCT;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// marshalling spec 6.12.2.1 page 90
	buffer[buffer_len++] = req_p->battery_status;
    
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_CONTROL, opcode, buffer_len, buffer) ){
    	bt_avrcp_inform_battery_statusofct_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_INFORM_BATTERY_STATUSOFCT_CNF;

		pcnf = (bt_avrcp_inform_battery_statusofct_cnf_struct *) &g_avrcp_data.p;
    	OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_inform_battery_statusofct_cnf_struct));

    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_inform_battery_statusofct_cnf_struct), (U8 *)pcnf);

		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}

}

void AVRCP_UtilAVRCPSendCmd_Get_Element_Attributes_Req(bt_avrcp_get_element_attributes_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 i;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_GET_ELEMENT_ATTRIBUTES;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// page 58
	OS_MemCopy( (U8 *) &buffer[buffer_len], (const U8 *) req_p->identifier, 8);
	buffer_len += 8;
	buffer[buffer_len++] = req_p->count;
	for( i = 0; i < req_p->count ; i++){
		StoreBE32( &buffer[buffer_len], req_p->attributes[i]);
		buffer_len += 4;
    }

	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_STATUS, opcode, buffer_len, buffer) ){
    	bt_avrcp_get_element_attributes_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_GET_ELEMENT_ATTRIBUTES_CNF;

        pcnf = (bt_avrcp_get_element_attributes_cnf_struct *) &g_avrcp_data.p.ABTAVRCPGetElementAttrCnf;
		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_get_element_attributes_cnf_struct));

    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_get_element_attributes_cnf_struct), (U8 *)pcnf);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

void AVRCP_UtilAVRCPSendCmd_Get_Playerstatus_Req(bt_avrcp_get_playerstatus_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_GET_PLAYERAPP_STATUS;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// none parameter

	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_STATUS, opcode, buffer_len, buffer) ){
    	bt_avrcp_get_playerstatus_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_GET_PLAYERAPP_STATUS_CNF;

        pcnf = (bt_avrcp_get_playerstatus_cnf_struct *) &g_avrcp_data.p.ABTAVRCPGetPlayerStatusCnf;
    	OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_get_playerstatus_cnf_struct));
		
    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_get_playerstatus_cnf_struct), (U8 *)pcnf);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

void AVRCP_UtilAVRCPSendCmd_Register_Nofitication_Req(bt_avrcp_register_notification_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_REGISTER_NOFITICATION;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// page 60
	buffer[buffer_len++] = req_p->event_id;
	StoreBE32( &buffer[buffer_len], req_p->playback_interval);
	buffer_len += 4;
	
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_NOTIFY, opcode, buffer_len, buffer) ){
    	bt_avrcp_register_notification_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_REGISTER_NOFITICATION_CNF;
    	
        pcnf = (bt_avrcp_register_notification_cnf_struct *) &g_avrcp_data.p.ABTAVRCPRegisterNotifyCnf;
		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_register_notification_cnf_struct));

    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_register_notification_cnf_struct), (U8 *)pcnf);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

void AVRCP_UtilAVRCPSendCmd_Abort_Continueresponse_Req(bt_avrcp_abort_continueresponse_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_ABORT_CONTINUING;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// page 64
	buffer[buffer_len++] = req_p->pdu_id;
	
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_CONTROL, opcode, buffer_len, buffer) ){
    	bt_avrcp_abort_continueresponse_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_ABORT_CONTINUING_CNF;

		pcnf = (bt_avrcp_abort_continueresponse_cnf_struct *) &g_avrcp_data.p;
		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_abort_continueresponse_cnf_struct));

    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_abort_continueresponse_cnf_struct), (U8 *)pcnf);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

void AVRCP_UtilAVRCPSendCmd_Set_Absolute_Volume_Req(bt_avrcp_set_absolute_volume_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_SET_ABSOLUTE_VOLUME;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// page 64
	buffer[buffer_len++] = req_p->volume;
	
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_CONTROL, opcode, buffer_len, buffer) ){
    	bt_avrcp_set_absolute_volume_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_SET_ABSOLUTE_VOLUME_CNF;
    	
		pcnf = (bt_avrcp_set_absolute_volume_cnf_struct *) &g_avrcp_data.p;
		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_set_absolute_volume_cnf_struct));

    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_set_absolute_volume_cnf_struct), (U8 *)pcnf);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

void AVRCP_UtilAVRCPSendCmd_Set_Addressedplayer_Req(bt_avrcp_set_addressedplayer_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_SET_ADDRESSEDPLAYER;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// page 66
	StoreBE16( &buffer[buffer_len], req_p->player_id);
	buffer_len += 2;
	
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_CONTROL, opcode, buffer_len, buffer) ){
    	bt_avrcp_set_addressedplayer_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_SET_ADDRESSEDPLAYER_CNF;

		pcnf = (bt_avrcp_set_addressedplayer_cnf_struct *) &g_avrcp_data.p;
    	OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_set_addressedplayer_cnf_struct));

    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_set_addressedplayer_cnf_struct), (U8 *)pcnf);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

// browse cmd request
void AVRCP_UtilAVRCPSendCmd_Get_Folderitems_Req(bt_avrcp_get_folderitems_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_BROWSE_OPCODE_GET_FOLDERITEMS;
	AvrcpChnlInfo myChnlInfo;
	U8 i;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// page 83 & 142 - size 10
	buffer[buffer_len++] = req_p->scope;
	StoreBE32( &buffer[buffer_len], req_p->start_item);
	buffer_len += 4;
	StoreBE32( &buffer[buffer_len], req_p->end_item);
	buffer_len += 4;
	buffer[buffer_len++] = req_p->count;
    // 0xFF is no attribute request, 0 is all attributes. spec page 85
	for( i = 0; i< req_p->count && req_p->count != 0xFF; i++){
    	StoreBE32( &buffer[buffer_len], req_p->attribute_ids[i]);
	    buffer_len += 4;
	}
	
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPBrowseCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_CONTROL, opcode, buffer_len, buffer) ){
    	bt_avrcp_get_folderitems_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_GET_FOLDERITEMS_CNF;

		pcnf = (bt_avrcp_get_folderitems_cnf_struct *) &g_avrcp_data.p.ABTAVRCPGetFolderItemsCnf;
		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_get_folderitems_cnf_struct));

    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_get_folderitems_cnf_struct), (U8 *)pcnf);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

// browse cmd request
void AVRCP_UtilAVRCPSendCmd_Set_Browsedplayer_Req(bt_avrcp_set_browsedplayer_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_BROWSE_OPCODE_SET_BROWSEDPLAYER;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// page 68 - size 2
	StoreBE16( &buffer[buffer_len], req_p->player_id);
	buffer_len += 2;
	
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPBrowseCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_CONTROL, opcode, buffer_len, buffer) ){
    	bt_avrcp_set_browsedplayer_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_SET_BROWSEDPLAYER_CNF;
    	
        pcnf = (bt_avrcp_set_browsedplayer_cnf_struct *) &g_avrcp_data.p.ABTAVRCPSetBrowsePlayerCnf;
		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_set_browsedplayer_cnf_struct));

    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_set_browsedplayer_cnf_struct), (U8 *)pcnf);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

// brose cmd request
void AVRCP_UtilAVRCPSendCmd_Change_Path_Req(bt_avrcp_change_path_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_BROWSE_OPCODE_CHANGE_PATH;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// page 83 - size 11
	StoreBE16( &buffer[buffer_len], req_p->uid_counter);
	buffer_len += 2;
	buffer[buffer_len++] = req_p->direction;
	OS_MemCopy( (U8 *) &buffer[buffer_len], (const U8 *) req_p->folder_uid, 8);
	buffer_len += 8;
	
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPBrowseCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_CONTROL, opcode, buffer_len, buffer) ){
    	bt_avrcp_change_path_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_CHANGE_PATH_CNF;
    	
		pcnf = (bt_avrcp_change_path_cnf_struct *) &g_avrcp_data.p;
		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_change_path_cnf_struct));

    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_change_path_cnf_struct), (U8 *)pcnf);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

// browse request
void AVRCP_UtilAVRCPSendCmd_Get_Itemattributes_Req(bt_avrcp_get_itemattributes_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0, i;
	U8 opcode = AVRCP_BROWSE_OPCODE_GET_ITEMATTRIBUTES;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// page 86
	buffer[buffer_len++] = req_p->scope;
	OS_MemCopy( (U8 *)&buffer[buffer_len], req_p->uid, 8);
	buffer_len += 8;
	StoreBE16( &buffer[buffer_len], req_p->uid_counter);
	buffer_len += 2;

    if( req_p->count > BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID){
        req_p->count = BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID;
    }
	buffer[buffer_len++] = req_p->count;

	for( i  = 0 ; i < req_p->count; i++ ){
    	StoreBE32( &buffer[buffer_len], req_p->attribute_ids[i]);
	    buffer_len += 4;
	}
	
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPBrowseCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_CONTROL, opcode, buffer_len, buffer) ){
    	bt_avrcp_get_itemattributes_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_GET_ITEMATTRIBUTES_CNF;
    	
        pcnf = (bt_avrcp_get_itemattributes_cnf_struct *) &g_avrcp_data.p.ABTAVRCPGetItemAttrCnf;
        OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_get_itemattributes_cnf_struct));

    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_get_itemattributes_cnf_struct), (U8 *)pcnf);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

void AVRCP_UtilAVRCPSendCmd_Play_Items_Req(bt_avrcp_play_items_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_PLAY_ITEMS;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// page 88 - size 11
	buffer[buffer_len++] = req_p->scope;
	OS_MemCopy( (U8 *) &buffer[buffer_len], (const U8 *)req_p->uid, 8);
	buffer_len += 8;
	StoreBE16( &buffer[buffer_len], req_p->uid_counter);
	buffer_len += 2;
	
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_CONTROL, opcode, buffer_len, buffer) ){
    	bt_avrcp_play_items_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_PLAY_ITEMS_CNF;
    	
		pcnf = (bt_avrcp_play_items_cnf_struct *) &g_avrcp_data.p;
		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_play_items_cnf_struct));

    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_play_items_cnf_struct), (U8 *)pcnf);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

// browse cmd request
void AVRCP_UtilAVRCPSendCmd_Search_Req(bt_avrcp_search_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_BROWSE_OPCODE_SEARCH;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// page 89 - size 4 + s
	StoreBE16( &buffer[buffer_len], req_p->charset);
	buffer_len += 2;
	StoreBE16( &buffer[buffer_len], req_p->length);
	buffer_len += 2;
	OS_MemCopy( (U8 *) &buffer[buffer_len], (const U8 *) req_p->search_string, req_p->length);
	buffer_len += req_p->length;
	
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPBrowseCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_CONTROL, opcode, buffer_len, buffer) ){
    	bt_avrcp_search_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_SEARCH_CNF;
    	
		pcnf = (bt_avrcp_search_cnf_struct *) &g_avrcp_data.p;
		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_search_cnf_struct));

    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_search_cnf_struct), (U8 *)pcnf);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}

void AVRCP_UtilAVRCPSendCmd_Add_Tonowplaying_Req(bt_avrcp_add_tonowplaying_req_struct *req_p)
{
	U8 *buffer = g_avrcp_util_marshall_buffer;
	U16 buffer_len = 0;
	U8 opcode = AVRCP_SPECIFIC_OPCODE_ADD_TONOWPLAYING;
	AvrcpChnlInfo myChnlInfo;
	Assert(req_p);

	myChnlInfo.chnl_num = req_p->chnl_num;
	myChnlInfo.transId = req_p->seq_id;

	// page 90 - size 11
	buffer[buffer_len++] = req_p->scope;
	OS_MemCopy( (U8 *) &buffer[buffer_len], (const U8 *)req_p->uid, 8);
	buffer_len += 8;
	StoreBE16( &buffer[buffer_len], req_p->uid_counter);
	buffer_len += 2;
	
	if( BT_STATUS_PENDING != AVRCP_UtilAVRCPCmdBufferRsp( &myChnlInfo, AVCTP_CTYPE_CONTROL, opcode, buffer_len, buffer) ){
    	bt_avrcp_add_tonowplaying_cnf_struct *pcnf;
    	U8 event = BTAVRCP_EVENT_ADD_TONOWPLAYING_CNF;
    	
		pcnf = (bt_avrcp_add_tonowplaying_cnf_struct *) &g_avrcp_data.p;
		OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_add_tonowplaying_cnf_struct));

    	pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;

	    AvrcpHandleAVRCPCnf_common(&myChnlInfo, pcnf->result, event, sizeof(bt_avrcp_add_tonowplaying_cnf_struct), (U8 *)pcnf);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SEND_CMD_PDU_FAIL, opcode);
	}
}


void AVRCP_UtilAVRCPSDP_Query_Req(bt_avrcp_sdp_query_req_struct *req_p){
    avrcp_adp_context *cntx_ptr;
    AvrcpChannel *Chnl = 0;
    BtStatus status = AVCTP_STATE_DISCONNECTED;
    AvrcpChnlInfo info; 
    U8 state = 0;
    
    cntx_ptr = &avrcp_cntx[req_p->chnl_num];
    if( 0 != cntx_ptr){
        Chnl = &cntx_ptr->rcp_channel;
    }

    OS_Report("[AVRCP] SDP_Query_Req chnl_num:%d query_role:%d", req_p->chnl_num, req_p->query_role);
    if( 0 != Chnl ){
        state = Chnl->chnl.conn.state;
    }
    if( 0 != Chnl && AVCTP_STATE_CONNECTED == state ){
//        status = AvrcpStartServiceQuery(Chnl, BSQM_FIRST);
        status = AvrcpStartMultiServiceQuery( Chnl, BSQM_FIRST, Chnl->role );
        OS_Report("[AVRCP] SDP_Query_Req multi status:%d", status);

        if( 0 == status || 2 == status ){
        }else{
            // fail
            info.chnl_num = req_p->chnl_num;
            info.transId = req_p->seq_id;
            AVRCP_UtilAVRCPSDPQueryInd( &info, 1, 0, 0, 0, 0);
        }
    }else{
        if( Chnl ){
            OS_Report("[AVRCP] SDP_Query_Req warning state:%d", Chnl->chnl.state);
        }else{
            OS_Report("[AVRCP] SDP_Query_Req error ! no Chnl");
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  AVRCP_UtilInit
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void AVRCP_UtilInit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i, j;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_INIT );
    for (i = 0; i < BT_AVRCP_MAX_CHANNEL; i++)
    {
        avrcp_cntx[i].state = BT_AVRCP_STATE_IDLE;
        avrcp_cntx[i].rcp_role = BT_AVRCP_ROLE_UNDEF;
        avrcp_cntx[i].connect_id = 0;

        /* AV13 init the packet pools */
        OS_Report("[AVRCP] AVRCP_NUM_TX_PACKETS:%d AVRCP_MAX_PACKET_LEN:%d", AVRCP_NUM_TX_PACKETS, AVRCP_MAX_PACKET_LEN);
        for(j = 0; j < AVRCP_NUM_TX_PACKETS; j++){
			avrcp_cntx[i].txPackets[j].data = (U8 *) avrcp_cntx[i].txData[j];
        }
        InitializeListHead(&avrcp_cntx[i].freePacketList);
        for(j = 0; j < AVRCP_NUM_TX_PACKETS; j++){
			kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_INIT_PICK_PACKET, &avrcp_cntx[i].txPackets[j], avrcp_cntx[i].txData[j]);
            InsertHeadList( &avrcp_cntx[i].freePacketList, (ListEntry *)&avrcp_cntx[i].txPackets[j]);
        }
    }
}



/*****************************************************************************
 * FUNCTION
 *  AvrcpUtilCallbackFindChannel
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl    :  avrcp channel
 * RETURNS
 *  
 *****************************************************************************/
static U8 AvrcpUtilCallbackFindChannel(AvrcpChannel *chnl)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < BT_AVRCP_MAX_CHANNEL; i++)
    {
        if (chnl == &avrcp_cntx[i].rcp_channel)
        {
            return i;
        }
    }

    return BT_AVRCP_MAX_CHANNEL;
}


/*****************************************************************************
 * FUNCTION
 *  AvrcpUtilCallbackConnect
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  Parms       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
static void AvrcpUtilCallbackConnect(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#ifndef __AVRCP_V14_AP_TEST__
	BtStatus status;
#endif
    bt_avrcp_dev_addr_struct device_addr;
    avrcp_adp_context *cntx_ptr;
    U8 chnl_num;
    U32 uuid_role = 0;
//    BtStatus status;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    chnl_num = AvrcpUtilCallbackFindChannel(chnl);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( !(chnl_num < BT_AVRCP_MAX_CHANNEL) ){
        return;
    }
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, AVRCP_CALLBACK_EVENT_CONNECT, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    AVRCP_CONVERT_ARRAY2BDADDR(&device_addr, Parms->p.remDev->bdAddr.addr);
    AVRCP_CONVERT_UUID_FROM_ROLE(cntx_ptr->rcp_role, uuid_role);

    /* Connect Request Success */
    if (cntx_ptr->state == BT_AVRCP_STATE_CONNECTING)
    {
         
        cntx_ptr->state = BT_AVRCP_STATE_CONNECTED;
        cntx_ptr->connect_id = BTBMProfileAllocateConnIndex(uuid_role, Parms->p.remDev->bdAddr);

#ifndef __AVRCP_V14_AP_TEST__
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SDP_TG_INFO, cntx_ptr->rcp_channel.remoteCTSdp.ctAvailable,
		    cntx_ptr->rcp_channel.remoteCTSdp.ctVersion, 
		    cntx_ptr->rcp_channel.remoteCTSdp.ctFeatureFlag,
		    cntx_ptr->rcp_channel.remoteCTSdp.ctSupportBrowse);
		if( cntx_ptr->rcp_channel.remoteCTSdp.ctAvailable != FALSE && 0x00 != cntx_ptr->rcp_channel.remoteCTSdp.ctSupportBrowse ){
            // AV13 try to connect the browse channel
            status = AVRCP_ConnectBrowse( &cntx_ptr->rcp_channel );
            kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_STATUS_REPORT, status );
   		}
#endif

        AvrcpHandleConnectCnf(chnl_num, BT_AVRCP_RESULT_SUCCESS, &device_addr, cntx_ptr->connect_id,
            (cntx_ptr->rcp_channel.remoteTGSdp.tgSupportBrowse),
            cntx_ptr->rcp_channel.remoteCTSdp.ctFeatureFlag,
            cntx_ptr->rcp_channel.remoteTGSdp.tgFeatureFlag
            );
    }
    /* Connect Indication Success */
    else if (cntx_ptr->state == BT_AVRCP_STATE_ACCEPTING_CONNECT)
    {
        cntx_ptr->state = BT_AVRCP_STATE_CONNECTED;
//        cntx_ptr->connect_id = BTBMProfileAllocateConnIndex(uuid_role, Parms->p.remDev->bdAddr);        
        AvrcpHandleConnectCnf(chnl_num, BT_AVRCP_RESULT_SUCCESS, &device_addr, cntx_ptr->connect_id,
            (cntx_ptr->rcp_channel.remoteTGSdp.tgSupportBrowse),
            cntx_ptr->rcp_channel.remoteCTSdp.ctFeatureFlag,
            cntx_ptr->rcp_channel.remoteTGSdp.tgFeatureFlag
        );
//        AvrcpHandleConnectInd(chnl_num, &device_addr, cntx_ptr->connect_id);
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvrcpUtilCallbackConnectInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  Parms       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
static void AvrcpUtilCallbackConnectInd(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_avrcp_dev_addr_struct device_addr;
    avrcp_adp_context *cntx_ptr;
    U8 chnl_num;
    U32 uuid_role = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    chnl_num = AvrcpUtilCallbackFindChannel(chnl);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( !(chnl_num < BT_AVRCP_MAX_CHANNEL) ){
        return;
    }
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, AVRCP_CALLBACK_EVENT_CONNECT_IND, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    if (cntx_ptr->state == BT_AVRCP_STATE_ACTIVATED)
    {
        /* Always accept connect ind for current design, may use MSG_ID_BT_AVRCP_CONNECT_IND_RSP if need MMI response */
        cntx_ptr->state = BT_AVRCP_STATE_ACCEPTING_CONNECT;
//        status = AVRCP_ConnectRsp(&cntx_ptr->rcp_channel, TRUE);
        AVRCP_CONVERT_ARRAY2BDADDR(&device_addr, Parms->p.remDev->bdAddr.addr);
        AVRCP_CONVERT_UUID_FROM_ROLE(cntx_ptr->rcp_role, uuid_role);
        cntx_ptr->connect_id = BTBMProfileAllocateConnIndex(uuid_role, Parms->p.remDev->bdAddr);  
        AvrcpHandleConnectInd(chnl_num, &device_addr, cntx_ptr->connect_id);
    }
    else
    {
        AVRCP_ConnectRsp(&cntx_ptr->rcp_channel, FALSE);
    }
}


U8 AvrcpUtilIsRetryConnecting(AvrcpChannel *chnl){
    avrcp_adp_context *cntx_ptr;
    U8 chnl_num;    
    chnl_num = AvrcpUtilCallbackFindChannel(chnl);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( !(chnl_num < BT_AVRCP_MAX_CHANNEL) ){
        return FALSE;
    }
    cntx_ptr = &avrcp_cntx[chnl_num];
    /* Connect Request Fail */
    if (cntx_ptr->state == BT_AVRCP_STATE_CONNECTING)
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpUtilCallbackDisconnect
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  Parms       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
static void AvrcpUtilCallbackDisconnect(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_avrcp_dev_addr_struct device_addr;
    avrcp_adp_context *cntx_ptr;
    U8 chnl_num;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    chnl_num = AvrcpUtilCallbackFindChannel(chnl);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( !(chnl_num < BT_AVRCP_MAX_CHANNEL) ){
        return;
    }
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, AVRCP_CALLBACK_EVENT_DISCONNECT, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    
    /* Cancel timer */
    if( 0 != chnl->ffKey ){
        chnl->ffKey = 0;
    }

    /* Connect Request Fail */
    if (cntx_ptr->state == BT_AVRCP_STATE_CONNECTING)
    {
        cntx_ptr->state = BT_AVRCP_STATE_ACTIVATED;

        AVRCP_CONVERT_ARRAY2BDADDR(&device_addr,Parms->p.remDev->bdAddr.addr);
        AvrcpHandleConnectCnf(chnl_num, BT_AVRCP_RESULT_ERROR, &device_addr, 0, 0, 0, 0);
    }
    /* Connect Indication Response Fail */
    else if (cntx_ptr->state == BT_AVRCP_STATE_ACCEPTING_CONNECT)
    {
        cntx_ptr->state = BT_AVRCP_STATE_ACTIVATED;
        AvrcpHandleDisconnectInd(chnl_num, cntx_ptr->connect_id);

    }
    /* Disconnect Request Success */
    else if (cntx_ptr->state == BT_AVRCP_STATE_DISCONNECTING)
    {
        cntx_ptr->state = BT_AVRCP_STATE_ACTIVATED;

        BTBMProfileReleaseConnIndex(cntx_ptr->connect_id);
        AvrcpHandleDisconnectCnf(chnl_num, BT_AVRCP_RESULT_SUCCESS, cntx_ptr->connect_id);
        cntx_ptr->connect_id = 0;
    }
    /* Disconnect Indication */
    else if (cntx_ptr->state == BT_AVRCP_STATE_CONNECTED)
    {
        cntx_ptr->state = BT_AVRCP_STATE_ACTIVATED;

        BTBMProfileReleaseConnIndex(cntx_ptr->connect_id);
        AvrcpHandleDisconnectInd(chnl_num, cntx_ptr->connect_id);
        cntx_ptr->connect_id = 0;
    }

    if( TRUE == AVRCP_IsBrowseExisted(&cntx_ptr->rcp_channel) ){
        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_DISC_BWS_CHANNEL, &cntx_ptr->rcp_channel);
        AVRCP_DisconnectBrowse(&cntx_ptr->rcp_channel);
    }

    if( NULL != chnl->curQueueFrame && chnl->curTxState == AVRCP_TX_IDLE && chnl->curQueueFrame->data_len != chnl->txOffset && chnl_num < BT_AVRCP_MAX_CHANNEL){
        // retrieve the packet
        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_INSERT_PACKET, &(avrcp_cntx[chnl_num].freePacketList), chnl->curQueueFrame, chnl->curQueueFrame->data);
        Assert( (chnl->curQueueFrame != NULL) );
        InsertTailList( &(avrcp_cntx[chnl_num].freePacketList) , (ListEntry *)chnl->curQueueFrame);
        chnl->curQueueFrame = NULL;
    }
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpBrowseUtilCallbackConnect
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  Parms       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
static void AvrcpBrowseUtilCallbackConnect(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    avrcp_adp_context *cntx_ptr;
    U8 chnl_num;
    U32 uuid_role = 0;
//    BtStatus status;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    chnl_num = AvrcpUtilCallbackFindChannel(chnl);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( !(chnl_num < BT_AVRCP_MAX_CHANNEL) ){
        return;
    }
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BROWSE_CALLBACK_CONNECT, cntx_ptr->rcp_role, AVRTP_UtilGetBrowseMtu(0) );
#endif    

    if( cntx_ptr->rcp_channel.bwsConnFlag ){
        AvrcpHandleBrowseConnectCnf(chnl_num, BT_AVRCP_RESULT_SUCCESS, AVRTP_UtilGetBrowseMtu(0));
    }else{
        AvrcpHandleBrowseConnectInd(chnl_num, AVRTP_UtilGetBrowseMtu(0));
    }

}


/*****************************************************************************
 * FUNCTION
 *  AvrcpBrowseUtilCallbackDisconnect
 * DESCRIPTION
 *  
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  Parms       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
static void AvrcpBrowseUtilCallbackDisconnect(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    avrcp_adp_context *cntx_ptr;
    U8 chnl_num;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    chnl_num = AvrcpUtilCallbackFindChannel(chnl);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    if( !(chnl_num < BT_AVRCP_MAX_CHANNEL) ){
        return;
    }
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BROWSE_CALLBACK_DISCONNECT, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    
    

    AvrcpHandleBrowseDisconnectInd(chnl_num);

}


/*****************************************************************************
 * FUNCTION
 *  AvrcpUtilCallbackCmdCnf
 * DESCRIPTION
 *  Receive a response packet in AVCTP channel
 *
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  Parms   :  received packet's information and data    
 * RETURNS
 *  void
 *****************************************************************************/
static void AvrcpUtilCallbackCmdCnf(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    avrcp_adp_context *cntx_ptr;
    U8 chnl_num;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    chnl_num = AvrcpUtilCallbackFindChannel(chnl);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, AVRCP_CALLBACK_EVENT_CMD_RSP, cntx_ptr->rcp_role, cntx_ptr->state, Parms->event );
#endif    

    cntx_ptr->rspFrame.response = Parms->p.rspFrame->response;
    cntx_ptr->rspFrame.subunitType = Parms->p.rspFrame->subunitType;
    cntx_ptr->rspFrame.subunitId = Parms->p.rspFrame->subunitId;

    // Assemble the AVCTP packets 
    chnl->rxBuffer[0] = Parms->p.rspFrame->opcode;
    OS_MemCopy((U8 *)&chnl->rxBuffer[1], Parms->p.rspFrame->operands, Parms->p.rspFrame->operandLen);
    
    cntx_ptr->rspFrame.operandLen = Parms->p.rspFrame->operandLen + 1;  /* Put opcode in the buffer. */

    ASSERT(cntx_ptr->rspFrame.operandLen <= BT_AVRCP_MAX_RSP_FRAME_SIZE);

    if (Parms->p.rspFrame->more)
    {
        cntx_ptr->rsp_more_data = AVRCP_RSP_MORE;
    }
    else    /* has no more data */
    {
		AvrcpChnlInfo myChnlInfo;
        
		cntx_ptr->rsp_more_data = AVRCP_NO_DATA;
		myChnlInfo.chnl_num = chnl_num;
		myChnlInfo.transId = Parms->transId;

        AvrcpHandleCBAVCResponse(&myChnlInfo,
        Parms->p.rspFrame,
        (U16) (cntx_ptr->rspFrame.operandLen),  // array 0 is opcode
        chnl->rxBuffer);
    }

}


/*****************************************************************************
 * FUNCTION
 *  AvrcpUtilCallbackCmdInd
 * DESCRIPTION
 *  Receive a command packet in AVCTP channel
 *
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  Parms   :  received packet's information and data  
 * RETURNS
 *  void
 *****************************************************************************/
static void AvrcpUtilCallbackCmdInd(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    avrcp_adp_context *cntx_ptr;
    U8 chnl_num;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    chnl_num = AvrcpUtilCallbackFindChannel(chnl);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, AVRCP_CALLBACK_EVENT_CMD_IND, cntx_ptr->rcp_role, cntx_ptr->state );
#endif    

    cntx_ptr->cmdFrame.ctype = Parms->p.cmdFrame->ctype;
    cntx_ptr->cmdFrame.subunitType = Parms->p.cmdFrame->subunitType;
    cntx_ptr->cmdFrame.subunitId = Parms->p.cmdFrame->subunitId;

    // Assemble AVCTP packet. Put Rx data into rxBuffer and wait for remain data
    chnl->rxBuffer[0] = Parms->p.cmdFrame->opcode;
    OS_MemCopy(&chnl->rxBuffer[1], Parms->p.cmdFrame->operands, Parms->p.cmdFrame->operandLen);
    cntx_ptr->cmdFrame.operandLen = Parms->p.cmdFrame->operandLen + 1 ;  /* Put opcode in the buffer. */

    ASSERT(cntx_ptr->cmdFrame.operandLen <= BT_AVRCP_MAX_AV_FRAME_SIZE);

    if (Parms->p.cmdFrame->more)
    {
        // Timeout handle by AVCTP
        cntx_ptr->cmd_more_data = AVRCP_CMD_MORE;
    }
    else    /* has no more data */
    {
		AvrcpChnlInfo myChnlInfo;
        
		cntx_ptr->cmd_more_data = AVRCP_NO_DATA;
		myChnlInfo.chnl_num = chnl_num;
		myChnlInfo.transId = Parms->transId;
        
        AvrcpHandleCBAVCCommandInd(&myChnlInfo,
                Parms->p.cmdFrame,
                (U16) (cntx_ptr->cmdFrame.operandLen),  // array 0 is opcode
                chnl->rxBuffer);
    }
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCBAVRCPRsp_Receiver
 * DESCRIPTION
 *  Handle the parsing of AVRCP specific packets' response
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  avctpCmd:  avctp header
 *  pdu_id  :  avrcp pdu id
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCBAVRCPRsp_Receiver(AvrcpChnlInfo *chnl_info, AvctpRspFrame *avctpRsp, U8 pdu_id, U16 pdu_parm_len, U16 parm_len, U8 *parms){
    // compose response
	U8 send_data[9];
    U8 packetType;
    U8 chnl_num = chnl_info->chnl_num;
	U8 overhead = (1+3+1+1+2); // opcode + avrcp header = 8B
    U8 packettype_offset = 3+1+1; // company-id(3B) pdu_id(1B) packettype(1B)

	// index 0 opcode
	// index 1~parm_len avrcp header: company-id(3B) pdu_id(1B) packettype(1B) length(2B)
    if( parm_len >= overhead ){
        packetType = (parms[packettype_offset] & 0x3);
        
        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_RSP_PACKET_INFO, packetType, avctpRsp->response, parm_len, avctpRsp->more);
        
        if( AVRCP_PACKET_TYPE_SINGLE == packetType ){
            AvrcpHandleCBAVRCPRsp_Dispatcher(chnl_info, avctpRsp->response, pdu_id, pdu_parm_len, (U16)(parm_len-overhead), &parms[overhead]);
        }else{
            // 
            if( AVRCP_PACKET_TYPE_START == packetType ){
                frame_data_len[chnl_num] = 0;
                // copy the overhead of first packet only
                OS_MemCopy( &frame_data_buff[chnl_num][frame_data_len[chnl_num]], &parms[0], overhead);
                frame_data_len[chnl_num] += overhead;
            }
                
            // put it into frame. if the frame size is 
			if( parm_len > overhead && (frame_data_len	[chnl_num] + parm_len) < BT_AVRCP_MAX_RSP_FRAME_SIZE ){
                OS_MemCopy( &frame_data_buff[chnl_num][frame_data_len[chnl_num]], &parms[overhead], parm_len - overhead);
                frame_data_len[chnl_num] += parm_len - overhead;
            }else{
                kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CONTINUE_NO_BUFFER, parm_len, frame_data_len[chnl_num]);

				send_data[0] = 0x00;             // AVCTP opcode: vendor id
                send_data[1] = 0x00;             // 0x00 19 58 BT SIG company ID
                send_data[2] = 0x19;        
                send_data[3] = 0x58;
                send_data[4] = 0x41;             // pdu_id: 0x40 continue 0x41 abort
                send_data[5] = 0x00;             // packet type
                send_data[6] = 0x00;             // length 
                send_data[7] = 0x01;             // length 
                send_data[8] = pdu_id;          //frame_data_buff[chnl_num][0];  // continue previous pdu-id
				AVRCP_UitlCmdFrameReq(chnl_info->chnl_num, AVRCP_CTYPE_CONTROL, 0x07, 0x48, sizeof(send_data), send_data);

				return;
            }

            if( AVRCP_PACKET_TYPE_END == packetType  ){
                AvrcpHandleCBAVRCPRsp_Dispatcher(chnl_info, avctpRsp->response, pdu_id, frame_data_len[chnl_num], (U16)(frame_data_len[chnl_num]-overhead), &frame_data_buff[chnl_num][overhead]);
                // no auto response to remote peer
            }else{
                // request to get the next packet
                if( avctpRsp->response != AVCTP_RESPONSE_ACCEPTED && avctpRsp->response != AVCTP_RESPONSE_IMPLEMENTED_STABLE ){
                    AvrcpHandleCBAVRCPRsp_Dispatcher(chnl_info, avctpRsp->response, pdu_id, frame_data_len[chnl_num], (U16)(frame_data_len[chnl_num]-8), &frame_data_buff[chnl_num][8]);
                    return;
                }

				send_data[0] = 0x00;             // AVCTP opcode:vendor id
                send_data[1] = 0x00;             // 0x00 19 58 BT SIG company ID
                send_data[2] = 0x19;         
                send_data[3] = 0x58;
                send_data[4] = 0x40;             // pdu_id: 0x40 continue 0x41 abort
                send_data[5] = 0x00;             // packet type
                send_data[6] = 0x00;             // length 
                send_data[7] = 0x01;             // length 
                send_data[8] = pdu_id;          //frame_data_buff[chnl_num][0];  // continue previous pdu-id
				AVRCP_UitlCmdFrameReq(chnl_info->chnl_num, AVRCP_CTYPE_CONTROL, 0x07, 0x48, sizeof(send_data), send_data);
            }
            
        }
    }else{
        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SMALL_PACKET_SIZE_FAIL
, parm_len);
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCBAVRCPRsp_RegisterNoti
 * DESCRIPTION
 *  Handle the parsing of register notification of AVRCP specific response packet
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  avctpCmd:  avctp header
 *  pdu_id  :  avrcp pdu id
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCBAVRCPRsp_RegisterNoti(AvrcpChnlInfo *chnl_info, U8 ctype,BTAVRCPRegisterNotifyCnf *pcnf , U16 parm_len, U8 *parms){
		U8 isParable = FALSE;
	    pcnf->event_id = parms[0];
	    switch( pcnf->event_id ){
	        case AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED:
	            {
	            	if(parm_len >= 2){
	            		isParable = TRUE;
	            		pcnf->status = parms[1];
	            	}
	            }
	            break;
	        case AVRCP_NOTIFY_EVENT_TRACK_CHANGED:
	            {
	            	if(parm_len >= 9){
	            		isParable = TRUE;
	            		OS_MemCopy( pcnf->identifier,  &parms[1], 8);
	            	}
	            }
	            break;
	        case AVRCP_NOTIFY_EVENT_TRACK_REACHED_END:
	        	{
            		isParable = TRUE;
	        	}
	        	break;
	        case AVRCP_NOTIFY_EVENT_TRACK_REACHED_START:
	        	{
            		isParable = TRUE;
	        	}
	        	break;
	        case AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED:
	        	{
	            	if(parm_len >= 5){
	            		isParable = TRUE;
	            		pcnf->pos = BEtoHost32(&parms[1]);
	            	}

	        	}
	        	break;
	        case AVRCP_NOTIFY_EVENT_BATT_STATUS_CHANGED:
	        	{
	            	if(parm_len >= 2){
	            		isParable = TRUE;
	            		pcnf->status = parms[1];
	            	}
	        	}
	        	break;
	        case AVRCP_NOTIFY_EVENT_SYSTEM_STATUS_CHANGED:
	        	{
	            	if(parm_len >= 2){
	            		isParable = TRUE;
	            		pcnf->status = parms[1];
	            	}
	        	}
	        	break;
	        case AVRCP_NOTIFY_EVENT_PLAYER_APPLICATION_SETTING_CHANGED:
	            {
	                U8 i;
	                U8 offset;
	                
	            	if(parm_len >= 2){
	            		isParable = TRUE;
	            		pcnf->count = parms[1];
						offset = 2;
	            		if(pcnf->count > BT_AVRCP_MAX_ATTRIBUTE_NUM){
	            		    pcnf->count = BT_AVRCP_MAX_ATTRIBUTE_NUM;
	            		}
	            		    
	            		for( i = 0; i< pcnf->count; i++ ){
	            		    pcnf->attribute_id[i] = parms[offset++];
	            		    pcnf->attribute_value[i] = parms[offset++];
	            		}
	            	}
	            }
	            break;
	        case AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED:
	            {
	                isParable = TRUE;
	            }
	            break;
	        case AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED:
	            {
                    isParable = TRUE;
	            }
	            break;	            	        	
	        case AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED:
	            {
	                if(parm_len >= 5){
	            		isParable = TRUE;

	            		pcnf->id = BEtoHost16( &parms[1] );
						pcnf->uid_counter = BEtoHost16( &parms[3] );
	            	}
	            }
	            break;
	        case AVRCP_NOTIFY_EVENT_UIDS_CHANGED:
	            {
	            	if(parm_len >= 3){
	            		isParable = TRUE;
	            		pcnf->uid_counter = BEtoHost16( &parms[1] );
	            	}	                
	            }
	            break;
	        case AVRCP_NOTIFY_EVENT_VOLUME_CHANGED:
	            {
	                if(parm_len >= 2){
	            		isParable = TRUE;
	            		pcnf->volume = parms[1];
	            	}
	            }
	            break;	            	        	
	        default:
	            pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
	            break;
	    }

        if( FALSE == isParable ){	    
            pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
    	    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_REG_EVENT_FAIL, pcnf->event_id,parm_len);
        }else{
            pcnf->result = BT_AVRCP_RESULT_SUCCESS;
        }
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCBAVRCPRsp_Dispatcher
 * DESCRIPTION
 *  Handle the parsing of AVRCP specific response packet
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  avctpCmd:  avctp header
 *  pdu_id  :  avrcp pdu_id
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCBAVRCPRsp_Dispatcher(AvrcpChnlInfo *chnl_info, U8 ctype, U8 pdu_id, U16 pdu_parm_len, U16 parm_len, U8 *parms){
    U16 offset = 0, i;
    U8 result = (U8)BT_AVRCP_RESULT_SUCCESS;
    
	// Parse the response content and dispatch the result
	
	// dispatch the response indication to avrcpevents.c
	switch(pdu_id){
		case AVRCP_SPECIFIC_OPCODE_GET_CAPABILITIES:
			// capabiility response structure
			{
            	bt_avrcp_get_capabilities_cnf_struct *pcnf;
            	U8 event = BTAVRCP_EVENT_GET_CAPABILITIES_CNF;
            	
				pcnf = (bt_avrcp_get_capabilities_cnf_struct *) &g_avrcp_data.p.ABTAVRCPGetCapabilitiesCnf;
				OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_get_capabilities_cnf_struct));

                if( parm_len > 3 ){
                	pcnf->result = BT_AVRCP_RESULT_SUCCESS;
                	pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;
                	pcnf->capability_id = parms[0];
                	pcnf->count = parms[1]; 
                	if( 0x02 == parms[0] ){
                	    // company_Id response
                        pcnf->company_id_size = parm_len - 1;
                        if( pcnf->company_id_size > sizeof(pcnf->company_id) ){
                            pcnf->company_id_size = sizeof(pcnf->company_id);
                        }
                        OS_MemCopy( (U8 *) pcnf->company_id, (const U8 *) &parms[1], pcnf->company_id_size);
                        return;
                	}else if( 0x03 == parms[0] ){
                	    // events_supports
                	    if( pcnf->count > BT_AVRCP_MAX_EVENT_NUM ){
                	        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_TOO_MANY_ITEM, pcnf->count, BT_AVRCP_MAX_EVENT_NUM);
                	        pcnf->count = BT_AVRCP_MAX_EVENT_NUM;
                	    }
                  	    OS_MemCopy( (U8 *) pcnf->capability, (const U8 *) &parms[2], pcnf->count);
                	}else{
                        // drop it. ignore it
                        pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
                	}
                }else{
                    // fail to parse the packet
                    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_RSP_FAIL, pdu_id);	            		    
                    pcnf->result = BT_AVRCP_RESULT_ERROR;
                    if( parm_len >= 1 && ctype == AVCTP_RESPONSE_REJECTED ){
                		pcnf->error_code = parms[0];
                	}else{
                		pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;
                	}
                }
            	
        	    AvrcpHandleAVRCPCnf_common(chnl_info, pcnf->result, event, sizeof(bt_avrcp_get_capabilities_cnf_struct), (U8 *)pcnf);
			}
			break;
		case AVRCP_SPECIFIC_OPCODE_LIST_PLAYERAPP_SETTINGATTRIBUTE:
			{
				// list playe setting attributes
				U8 i;
            	bt_avrcp_list_playerapp_attribute_cnf_struct *pcnf;
            	U8 event = BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGATTRIBUTE_CNF;

				pcnf = (bt_avrcp_list_playerapp_attribute_cnf_struct *) &g_avrcp_data.p.ABTAVRCPListPlayerappAttributeCnf;
            	OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_list_playerapp_attribute_cnf_struct));

                // page 50
                if( parm_len >= 1 ){
                	pcnf->result = BT_AVRCP_RESULT_SUCCESS;

                	pcnf->count = parms[offset++];
                	for( i =0 ; i< pcnf->count; i++ ){
                	    pcnf->attribute_ids[i] = parms[offset++];
                	}
                }else{
                    // fail to parse the packet
                	pcnf->result = BT_AVRCP_RESULT_ERROR;
					if( parm_len >= 1 && ctype == AVCTP_RESPONSE_REJECTED ){
						pcnf->error_code = parms[0];
					}else{
						pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;
					}

                    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_RSP_FAIL, pdu_id);	            		    
                }
        	    AvrcpHandleAVRCPCnf_common(chnl_info, pcnf->result, event, sizeof(bt_avrcp_list_playerapp_attribute_cnf_struct), (U8 *)pcnf);
			}
			break;
		case AVRCP_SPECIFIC_OPCODE_LIST_PLAYERAPP_SETTINGVALUE:
    		{
				// list playe setting value
				U8 i;
            	bt_avrcp_list_playerapp_value_cnf_struct *pcnf;
            	U8 event = BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGVALUE_CNF;
				pcnf = (bt_avrcp_list_playerapp_value_cnf_struct *) &g_avrcp_data.p;
            	OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_list_playerapp_value_cnf_struct));

                // page 51
                if( parm_len >= 1 ){
                	pcnf->result = BT_AVRCP_RESULT_SUCCESS;

                	pcnf->count = parms[offset++];
                	for( i =0 ; i< pcnf->count; i++ ){
                	    pcnf->attribute_values[i] = parms[offset++];
                	}
                }else{
                    // fail to parse the packet
                	pcnf->result = BT_AVRCP_RESULT_ERROR;
					if( parm_len >= 1 && ctype == AVCTP_RESPONSE_REJECTED ){
						pcnf->error_code = parms[0];
					}else{
						pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;
					}

                    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_RSP_FAIL, pdu_id);	            		    
                }
        	    AvrcpHandleAVRCPCnf_common(chnl_info, pcnf->result, event, sizeof(bt_avrcp_list_playerapp_value_cnf_struct), (U8 *)pcnf);
            }
			break;
		case AVRCP_SPECIFIC_OPCODE_GET_CURPLAYERAPP_SETTINGVALUE:
    		{
				U8 i;
            	bt_avrcp_get_curplayerapp_value_cnf_struct *pcnf;
            	U8 event = BTAVRCP_EVENT_GET_CURPLAYERAPP_SETTINGVALUE_CNF;
				pcnf = (bt_avrcp_get_curplayerapp_value_cnf_struct *) &g_avrcp_data.p;
            	OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_get_curplayerapp_value_cnf_struct));

                // page 51
                if( parm_len >= 1 ){
                	pcnf->result = BT_AVRCP_RESULT_SUCCESS;

                	pcnf->count = parms[offset++];
                	for( i =0 ; i< pcnf->count; i++ ){
                    	pcnf->attribute_ids[i] = parms[offset++];
                	    pcnf->attribute_values[i] = parms[offset++];
                	}
                }else{
                    // fail to parse the packet
                	pcnf->result = BT_AVRCP_RESULT_ERROR;
					if( parm_len >= 1 && ctype == AVCTP_RESPONSE_REJECTED ){
						pcnf->error_code = parms[0];
					}else{
						pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;
					}

                    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_RSP_FAIL, pdu_id);	            		    
                }
        	    AvrcpHandleAVRCPCnf_common(chnl_info, pcnf->result, event, sizeof(bt_avrcp_get_curplayerapp_value_cnf_struct), (U8 *)pcnf);
            }
			break;
		case  AVRCP_SPECIFIC_OPCODE_SET_PLAYERAPP_SETTINGVALUE:
    		{
				// list playe setting value
            	bt_avrcp_set_playerapp_value_cnf_struct *pcnf;
            	U8 event = BTAVRCP_EVENT_SET_PLAYERAPP_SETTINGVALUE_CNF;

				pcnf = (bt_avrcp_set_playerapp_value_cnf_struct *) &g_avrcp_data.p;
            	OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_set_playerapp_value_cnf_struct));

                // page 52 - no parameter
               	pcnf->result = BT_AVRCP_RESULT_SUCCESS;
               	
				if( parm_len >= 1 && ctype == AVCTP_RESPONSE_REJECTED ){
					pcnf->result = BT_AVRCP_RESULT_ERROR;				
					pcnf->error_code = parms[0];
				}else{
					pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;
				}
				
        	    AvrcpHandleAVRCPCnf_common(chnl_info, pcnf->result, event, sizeof(bt_avrcp_set_playerapp_value_cnf_struct), (U8 *)pcnf);
            }
            break;
		case  AVRCP_SPECIFIC_OPCODE_GET_PLAYERAPP_SETTINGATTRIBUTETEXT:
		    {
		        U16 len;
            	bt_avrcp_get_playerapp_attributetext_cnf_struct *pcnf;
            	U8 event = BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGATTRIBUTETEXT_CNF;
				pcnf = (bt_avrcp_get_playerapp_attributetext_cnf_struct *) &g_avrcp_data.p;
            	OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_get_playerapp_attributetext_cnf_struct));

                // page 53
                if( parm_len >= 1 ){
                	pcnf->result = BT_AVRCP_RESULT_SUCCESS;

                	pcnf->count = parms[offset++];
                	if( pcnf->count > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
                	    pcnf->count = BT_AVRCP_MAX_ATTRIBUTE_NUM;
                	}
                	for( i =0 ; i< pcnf->count; i++ ){
                    	pcnf->attribute_id[i] = parms[offset++];
                    	pcnf->charset[i] = BEtoHost16( &parms[offset] );
                    	offset += 2;
                    	len = parms[offset++];
                    	pcnf->attribute_string_len[i] = (U8) len;
                        if( pcnf->attribute_string_len[i] > BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE){

                            pcnf->attribute_string_len[i] = BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE - 1;
                        }
                        OS_MemCopy( (U8 *) pcnf->attribute_string[i], (const U8 *) &parms[offset], pcnf->attribute_string_len[i]);
                        offset += len;
                	}
                }else{
                    // fail to parse the packet
                	pcnf->result = BT_AVRCP_RESULT_ERROR;
					if( parm_len >= 1 && ctype == AVCTP_RESPONSE_REJECTED ){
						pcnf->error_code = parms[0];
					}else{
						pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;
					}

                    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_RSP_FAIL, pdu_id);	            		    
                }
        	    AvrcpHandleAVRCPCnf_common(chnl_info, pcnf->result, event, sizeof(bt_avrcp_get_playerapp_attributetext_cnf_struct), (U8 *)pcnf);
            }
    		break;
		case  AVRCP_SPECIFIC_OPCODE_GET_PLAYERAPP_SETTINGVALUETEXT:
		    {
	        U16 len;
            	bt_avrcp_get_playerapp_valuetext_cnf_struct *pcnf;
            	U8 event = BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGVALUETEXT_CNF;
				pcnf = (bt_avrcp_get_playerapp_valuetext_cnf_struct *) &g_avrcp_data.p;
            	OS_MemSet( (U8 *) pcnf, 0, sizeof(bt_avrcp_get_playerapp_valuetext_cnf_struct));

                // page 54
                if( parm_len >= 1 ){
                	pcnf->result = BT_AVRCP_RESULT_SUCCESS;

                	pcnf->count = parms[offset++];
                	if( pcnf->count > BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM ){
                	    pcnf->count = BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM;
                	}
                	for( i =0 ; i< pcnf->count; i++ ){
                    	pcnf->value_ids[i] = parms[offset++];
                    	pcnf->charset[i] = BEtoHost16( &parms[offset] );
                    	offset += 2;
                    	len = parms[offset++];
                    	pcnf->value_string_len[i] = (U8) len;
                        if( pcnf->value_string_len[i] > BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE){
                            pcnf->value_string_len[i] = BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE - 1;
                        }
                        OS_MemCopy( (U8 *) pcnf->value_string[i], (const U8 *) &parms[offset], pcnf->value_string_len[i]);
                        offset += len;
                	}
                }else{
                    // fail to parse the packet
                	pcnf->result = BT_AVRCP_RESULT_ERROR;
					if( parm_len >= 1 && ctype == AVCTP_RESPONSE_REJECTED ){
						pcnf->error_code = parms[0];
					}else{
						pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;
					}

                    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_RSP_FAIL, pdu_id);	            		    
                }
        	    AvrcpHandleAVRCPCnf_common(chnl_info, pcnf->result, event, sizeof(bt_avrcp_get_playerapp_valuetext_cnf_struct), (U8 *)pcnf);		
    		}
            break;
		case  AVRCP_SPECIFIC_OPCODE_INFORM_DISPLAY_CHARSET:
		    {
		        BTAVRCPInformCharsetCnf *pcnf;
				U8 event = BTAVRCP_EVENT_INFORM_DISPLAY_CHARSET_CNF;

				pcnf = (BTAVRCPInformCharsetCnf *) &g_avrcp_data.p;
                OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPInformCharsetCnf));
			
				// No paremeter
				pcnf->result = BT_AVRCP_RESULT_SUCCESS;
				// Reject case
				if( parm_len == 1 && ctype == AVCTP_RESPONSE_REJECTED ){
					pcnf->result = BT_AVRCP_RESULT_ERROR;				
					pcnf->error_code = parms[0];
				}

				AvrcpHandleAVRCPCnf_common(chnl_info, result, event, sizeof(BTAVRCPInformCharsetCnf), (U8 *)pcnf);
    		}
            break;
		case  AVRCP_SPECIFIC_OPCODE_INFORM_BATTERY_STATUSOFCT:
		    {
		        BTAVRCPInformBattOfCTCnf *pcnf;
				U8 event = BTAVRCP_EVENT_INFORM_BATTERY_STATUSOFCT_CNF;

				pcnf = (BTAVRCPInformBattOfCTCnf *) &g_avrcp_data.p;
		        OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPInformBattOfCTCnf));
				pcnf->result = BT_AVRCP_RESULT_SUCCESS;
				// Reject case
				if( parm_len == 1 && ctype == AVCTP_RESPONSE_REJECTED ){
					pcnf->result = BT_AVRCP_RESULT_ERROR;	
					pcnf->error_code = parms[0];
				}

				AvrcpHandleAVRCPCnf_common(chnl_info, result, event, sizeof(BTAVRCPInformBattOfCTCnf), (U8 *)pcnf);
    		}
            break;

		case  AVRCP_SPECIFIC_OPCODE_GET_ELEMENT_ATTRIBUTES:
		    {
		        // save the response packet to structure
				U32 length;
		        BTAVRCPGetElementAttrCnf *pcnf;
				U8 event = BTAVRCP_EVENT_GET_ELEMENT_ATTRIBUTES_CNF;

				pcnf = (BTAVRCPGetElementAttrCnf *) &g_avrcp_data.p;
		        OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPGetElementAttrCnf));

				if( parm_len == 1 && ctype == AVCTP_RESPONSE_REJECTED ){
					pcnf->result = BT_AVRCP_RESULT_INTERNAL_ERROR;
					pcnf->error_code = parms[0];
				}else{
    				pcnf->result = BT_AVRCP_RESULT_ERROR;
				    if( parm_len >= 1 ){
				        offset = 0;
				        length = 0;
                        pcnf->count = parms[offset++];
                        if( pcnf->count > BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM ){

                            kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_TOO_MANY_ITEM, pcnf->count, BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM);
                            pcnf->count = BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM;
                        }
                        for( i = 0 ; i < pcnf->count; i++){
                            pcnf->elements[i].attribute_id = BEtoHost32(&parms[offset]);
                            offset += 4;
                            pcnf->elements[i].charset = BEtoHost16(&parms[offset]);
                            offset += 2;
                            pcnf->elements[i].value_len = BEtoHost16(&parms[offset]);
                            offset += 2;
                            // put text into data
                            if( (length + pcnf->elements[i].value_len) < sizeof(pcnf->element_data) ){
                                OS_MemCopy( (U8 *) &pcnf->element_data[length], &parms[offset], pcnf->elements[i].value_len);
								pcnf->elements[i].value_text_offset = (U16) length;
                                length += pcnf->elements[i].value_len ;
                                length += 2;
                                offset += pcnf->elements[i].value_len ;
                            }else{
                                kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_TOO_LARGE_LENGTH, pcnf->elements[i].value_len, (sizeof(pcnf->element_data)-length) );
                                pcnf->elements[i].value_len = 0;
                                if( i > 0){
                                    i--;
                                }
								pcnf->count = (U8)i;
                                break;
                            }
                        }
                        
    					pcnf->result = BT_AVRCP_RESULT_SUCCESS;
				    }
					
				}

				AvrcpHandleAVRCPCnf_common(chnl_info, result, event, sizeof(BTAVRCPGetElementAttrCnf), (U8 *)pcnf);
    		}
            break;
		case  AVRCP_SPECIFIC_OPCODE_GET_PLAYERAPP_STATUS:
		    {
    		    BTAVRCPGetPlayerStatusCnf *pcnf;//

				pcnf = (BTAVRCPGetPlayerStatusCnf *) &g_avrcp_data.p;
    		    OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPGetPlayerStatusCnf));
    		    if( parm_len >= 9 ){
        		    pcnf->result = BT_AVRCP_RESULT_SUCCESS;
                    offset = 0;
        		    pcnf->song_length = BEtoHost32(&parms[offset]);
        		    offset += 4;
        		    pcnf->song_position = BEtoHost32(&parms[offset]);
        		    offset += 4;
        		    pcnf->play_status = parms[offset++];
		        }else{
		            pcnf->result = BT_AVRCP_RESULT_ERROR;
					if( parm_len >= 1 && ctype == AVCTP_RESPONSE_REJECTED ){
						pcnf->error_code = parms[0];
					}else{
						pcnf->error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;
					}

		        }

		        AvrcpHandleAVRCPCnf_common(chnl_info, result, BTAVRCP_EVENT_GET_PLAYERAPP_STATUS_CNF, sizeof(BTAVRCPGetPlayerStatusCnf), (U8 *)pcnf);
		    }
		    break;
		    
		case  AVRCP_SPECIFIC_OPCODE_REGISTER_NOFITICATION:
		    {
    		    BTAVRCPRegisterNotifyCnf *pcnf;
				U8 event = BTAVRCP_EVENT_REGISTER_NOFITICATION_CNF;
				
				pcnf = (BTAVRCPRegisterNotifyCnf *) &g_avrcp_data.p;
				OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPRegisterNotifyCnf));
    		    
				if( parm_len >= 1 && ctype == AVCTP_RESPONSE_REJECTED ){
					pcnf->result = BT_AVRCP_RESULT_ERROR;
					pcnf->error_code = parms[0];
				}else if( parm_len >= 1 ){
        		    pcnf->result = BT_AVRCP_RESULT_SUCCESS;
        		    AvrcpHandleCBAVRCPRsp_RegisterNoti(chnl_info, result, pcnf, parm_len, parms);
		        }else{
		            pcnf->result = BT_AVRCP_RESULT_ERROR;
		            pcnf->error_code = BT_AVRCP_ERRCODE_INVALID_PARAM;
		        }
		        
				// NOTE!
				if( ctype == AVCTP_RESPONSE_INTERIM ){
					pcnf->is_interim = TRUE;
				}

		        AvrcpHandleAVRCPCnf_common(chnl_info, result, event, sizeof(BTAVRCPRegisterNotifyCnf), (U8 *)pcnf);
            }
		break;
		case  AVRCP_SPECIFIC_OPCODE_REQUEST_CONTINUING:
			{
				// Ext-ADP should not receives AVRCP_SPECIFIC_OPCODE_REQUEST_CONTINUING pdu-id
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UNKNOW_AV_TYPE, pdu_id);
			}
		break;
		case  AVRCP_SPECIFIC_OPCODE_ABORT_CONTINUING:
		    {
    		    BTAVRCPAbortContinueCnf *pcnf; 
    		    U8 event = BTAVRCP_EVENT_ABORT_CONTINUING_CNF;

				pcnf = (BTAVRCPAbortContinueCnf *) &g_avrcp_data.p;
    		    OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPAbortContinueCnf));

       		    pcnf->result = BT_AVRCP_RESULT_SUCCESS;
		        
				if( parm_len >= 1 && ctype == AVCTP_RESPONSE_REJECTED ){
					pcnf->result = BT_AVRCP_RESULT_ERROR;
					pcnf->error_code = parms[0];
				}

		        AvrcpHandleAVRCPCnf_common(chnl_info, result, event, sizeof(BTAVRCPAbortContinueCnf), (U8 *)pcnf);
    		        
		    }
        break;        
		case  AVRCP_SPECIFIC_OPCODE_SET_ABSOLUTE_VOLUME:
		    {
    		    BTAVRCPSetAbsoluteVolumeCnf *pcnf;
     		    U8 event = BTAVRCP_EVENT_SET_ABSOLUTE_VOLUME_CNF;

				pcnf = (BTAVRCPSetAbsoluteVolumeCnf *) &g_avrcp_data.p;
    		    OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPSetAbsoluteVolumeCnf));

    		    if( parm_len >= 1 && ctype == AVCTP_RESPONSE_REJECTED ){
					pcnf->result = BT_AVRCP_RESULT_ERROR;
					pcnf->error_code = parms[0];
				}else if( parm_len >= 1 ){
        		    pcnf->result = BT_AVRCP_RESULT_SUCCESS;
                    pcnf->volume = parms[0];
		        }else{
		            pcnf->result = BT_AVRCP_RESULT_ERROR;
		            pcnf->error_code = BT_AVRCP_ERRCODE_INVALID_PARAM;
		        }

		        AvrcpHandleAVRCPCnf_common(chnl_info, result, event, sizeof(BTAVRCPSetAbsoluteVolumeCnf), (U8 *)pcnf);
		    }		
		break;		
		case  AVRCP_SPECIFIC_OPCODE_SET_ADDRESSEDPLAYER:
		    {
    		    BTAVRCPSetAddrPlayerCnf *pcnf;
     		    U8 event = BTAVRCP_EVENT_SET_ADDRESSEDPLAYER_CNF;

				pcnf = (BTAVRCPSetAddrPlayerCnf *) &g_avrcp_data.p;
    		    OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPSetAddrPlayerCnf));

    		    if( parm_len >= 1 && ctype == AVCTP_RESPONSE_REJECTED ){
					pcnf->result = BT_AVRCP_RESULT_ERROR;
					pcnf->error_code = parms[0];
				}else if( parm_len >= 1 ){
        		    pcnf->result = BT_AVRCP_RESULT_SUCCESS;
                    pcnf->status = parms[0];
		        }else{
		            pcnf->result = BT_AVRCP_RESULT_ERROR;
		            pcnf->error_code = BT_AVRCP_ERRCODE_INVALID_PARAM;
		        }
		        
		        AvrcpHandleAVRCPCnf_common(chnl_info, result, event, sizeof(BTAVRCPSetAddrPlayerCnf), (U8 *)pcnf);
		    }		
		break;
		case  AVRCP_SPECIFIC_OPCODE_PLAY_ITEMS:
    	    {
    		    BTAVRCPPlayItemsCnf *pcnf;
     		    U8 event = BTAVRCP_EVENT_PLAY_ITEMS_CNF;

				pcnf = (BTAVRCPPlayItemsCnf *) &g_avrcp_data.p;
    		    OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPPlayItemsCnf));

    		    if( parm_len >= 1 ){
        		    pcnf->result = BT_AVRCP_RESULT_SUCCESS;
                    pcnf->status = parms[0];
		        }else{
		            pcnf->result = BT_AVRCP_RESULT_ERROR;
		            pcnf->error_code = BT_AVRCP_ERRCODE_INVALID_PARAM;
		        }
		        
		        AvrcpHandleAVRCPCnf_common(chnl_info, result, event, sizeof(BTAVRCPPlayItemsCnf), (U8 *)pcnf);
		    }
		break;
		case  AVRCP_SPECIFIC_OPCODE_ADD_TONOWPLAYING:
    	    {
    		    BTAVRCPAddNowPlayingCnf *pcnf;
     		    U8 event = BTAVRCP_EVENT_ADD_TONOWPLAYING_CNF;

				pcnf = (BTAVRCPAddNowPlayingCnf *) &g_avrcp_data.p;
    		    OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPAddNowPlayingCnf));
    		    if( parm_len >= 1 ){
        		    pcnf->result = BT_AVRCP_RESULT_SUCCESS;
                    pcnf->status = parms[0];
		        }else{
		            pcnf->result = BT_AVRCP_RESULT_ERROR;
		            pcnf->error_code = BT_AVRCP_ERRCODE_INVALID_PARAM;
		        }
		        
		        AvrcpHandleAVRCPCnf_common(chnl_info, result, event, sizeof(BTAVRCPAddNowPlayingCnf), (U8 *)pcnf);
		    }
		break;
		
		// browse channel's PDU-id
		case  AVRCP_BROWSE_OPCODE_SET_BROWSEDPLAYER:
		case  AVRCP_BROWSE_OPCODE_GET_FOLDERITEMS:
		case  AVRCP_BROWSE_OPCODE_CHANGE_PATH:
		case  AVRCP_BROWSE_OPCODE_GET_ITEMATTRIBUTES:
		case  AVRCP_BROWSE_OPCODE_SEARCH:
			{
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BWS_WRONG_CHANNEL, pdu_id);
			}
		default:
			{   // AV13:add
    			kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UNKNOW_AV_TYPE, pdu_id);
			}
			break;
		
	}
}	

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCBAVRCPBrowseRsp_Dispatcher
 * DESCRIPTION
 *  Handle the received browse packet. 
 *  Parse the response packets of AVRCP specific command
 *  Dispatch to other handler by pdu-id
 * PARAMETERS
 *  chnl    :  avrcp channel infomation
 *  avctpRsp:  received response packet
 *  pdu_id  :  av
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCBAVRCPBrowseRsp_Dispatcher(AvrcpChnlInfo *chnl_info, AvctpRspFrame *avctpRsp, U8 pdu_id, U16 pdu_parm_len, U16 parm_len, U8 *parms){
    U16 result = BT_AVRCP_RESULT_SUCCESS;
	U8 error_code = 0;
    U32 i, offset = 0;
    
	// Parse the browse response content and dispatch the result
	
	// NOTE: AVRCP browse packet doesn't has a reject type. 
	// It uses one-byte payload to present the error status
	switch(pdu_id){
        case AVRCP_BROWSE_OPCODE_SET_BROWSEDPLAYER:
	    {
		    BTAVRCPSetBrowsePlayerCnf *pcnf;
			U16 name_offset, len;

			pcnf = (BTAVRCPSetBrowsePlayerCnf *) &g_avrcp_data.p.ABTAVRCPSetBrowsePlayerCnf;
            if( parm_len >= 12 ){ // spec 6.9.3 page 69
				OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPSetBrowsePlayerCnf) );

				pcnf->result = BT_AVRCP_RESULT_SUCCESS;
                pcnf->status = parms[offset++];
                pcnf->uid_counter = BEtoHost16( &parms[offset] );
                offset += 2;
                pcnf->num_of_items = BEtoHost32( &parms[offset] );
                offset += 4;
                pcnf->char_set = BEtoHost16( &parms[offset] );
                offset += 2;
                pcnf->folder_depth = parms[offset++];

				name_offset = 0;
				for( i = 0; i < pcnf->folder_depth; i++){
					len = BEtoHost16( &parms[offset] );
					offset += 2;

					pcnf->foldername_len[i] = len;
					if( pcnf->foldername_len[i] > BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE ){
						pcnf->foldername_len[i] = BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE - 1;
					}
					OS_MemCopy( (U8 *) &pcnf->folder_data[name_offset], (const U8 *) &parms[offset], pcnf->foldername_len[i]);
					
					pcnf->foldername_offset[i] = name_offset;

					offset += len; // len is the actual length 
					name_offset += pcnf->foldername_len[i]; // foldername_len[i] is a part of actual length
					name_offset += 2;
				}
                
		        AvrcpHandleCBAVRCPRsp_Common(chnl_info, result, BTAVRCP_EVENT_SET_BROWSEDPLAYER_CNF, sizeof(BTAVRCPSetBrowsePlayerCnf), (U8 *)pcnf);
            }else{
				pcnf->result = result = BT_AVRCP_RESULT_ERROR;
				if( parm_len > 0 ){
				pcnf->error_code = error_code = parms[0];
				}

				AvrcpHandleCBAVRCPRsp_ReportError(chnl_info, result, error_code, BTAVRCP_EVENT_SET_BROWSEDPLAYER_CNF, 0, (U8 *)NULL);
                AvrcpHandleCBAVRCPRspInd_ShowErrorMsg(chnl_info, pdu_id, parm_len);
            }
	    }		
        break;
        case AVRCP_BROWSE_OPCODE_GET_FOLDERITEMS:
	    {
		    BTAVRCPGetFolderItemsCnf *pcnf;
		    U16 len, j;
		    U8 parseable = TRUE;

			pcnf = (BTAVRCPGetFolderItemsCnf *) &g_avrcp_data.p;

            if( parm_len >= 5 ){ // spec 6.10.4.2 page 84
				OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPGetFolderItemsCnf) );

                pcnf->status = parms[offset++];
                pcnf->uid_counter = BEtoHost16( &parms[offset] );
                offset += 2;
                pcnf->counter = BEtoHost16( &parms[offset] );
                offset += 2;
                if( pcnf->counter > BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE ){
                    pcnf->counter = BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE;
                }

                pcnf->data_len = 0;
				for( i = 0; (i < pcnf->counter && TRUE == parseable && i < BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE) ; i++ ){
					// 2 common fields
					pcnf->items[i].item_type = parms[offset++];
					pcnf->items[i].item_length = BEtoHost16( &parms[offset] );
					offset += 2;
					
                    switch( pcnf->items[i].item_type ){
                        case 0x01: // media player item
                        {
                            pcnf->items[i].player_id = BEtoHost16( &parms[offset] );
                            offset += 2;
                            pcnf->items[i].specific_type = parms[offset++]; // major player type
                            pcnf->items[i].sub_type = BEtoHost32( &parms[offset] );
                            offset += 4;
                            pcnf->items[i].status = parms[offset++];
                            OS_MemCopy( (U8 *) &pcnf->items[i].mask[0], (const U8 *) &parms[offset], 16);
                            offset += 16;
                            pcnf->items[i].charset = BEtoHost16( &parms[offset] );
                            offset += 2;
                            pcnf->items[i].name_length = BEtoHost16( &parms[offset] );
                            offset += 2;
                            pcnf->items[i].name_offset = pcnf->data_len;
                            OS_MemCopy( (U8 *) &pcnf->data[pcnf->data_len], (const U8 *) &parms[offset], pcnf->items[i].name_length);
                            offset += pcnf->items[i].name_length;

                            // shift the data_len to the next string's index
                            pcnf->data_len += pcnf->items[i].name_length + 2;
                        }
                        break;
                        case 0x02: // folder item
                        {

                            OS_MemCopy( (U8 *) &pcnf->items[i].uid, (const U8 *) &parms[offset], 8);
                            offset += 8;
                            pcnf->items[i].specific_type = parms[offset++];
                            pcnf->items[i].is_playable = parms[offset++];
                            pcnf->items[i].charset = BEtoHost16( &parms[offset] );
                            offset += 2;
                            pcnf->items[i].name_length = BEtoHost16( &parms[offset] );
                            offset += 2;
                            pcnf->items[i].name_offset = pcnf->data_len;
                            OS_MemCopy( (U8 *) &pcnf->data[pcnf->data_len], (const U8 *) &parms[offset], pcnf->items[i].name_length);
                            offset += pcnf->items[i].name_length;

                            // shift the data_len to the next string's index
                            pcnf->data_len += pcnf->items[i].name_length + 2;
                        
                        }
                        break;
                        case 0x03: // element item
                        {

                            OS_MemCopy( (U8 *) &pcnf->items[i].uid, (const U8 *) &parms[offset], 8);
                            offset += 8;
                            pcnf->items[i].specific_type = parms[offset++];
                            pcnf->items[i].charset = BEtoHost16( &parms[offset] );
                            offset += 2;
                            pcnf->items[i].name_length = len = BEtoHost16( &parms[offset] );
                            offset += 2;
                            pcnf->items[i].name_offset = pcnf->data_len;

							if( pcnf->items[i].name_length > BT_AVRCP_MAX_PLAYER_NAME_SIZE ){
								kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_TOO_LARGE_LENGTH, pcnf->items[i].name_length, BT_AVRCP_MAX_PLAYER_NAME_SIZE);
								pcnf->items[i].name_length = BT_AVRCP_MAX_PLAYER_NAME_SIZE;
							}
							// copy data into buffer
                            OS_MemCopy( (U8 *) &pcnf->data[pcnf->data_len], (const U8 *) &parms[offset], pcnf->items[i].name_length);
                            offset += len ;

                            // shift the data_len to the next string's index
                            pcnf->data_len += pcnf->items[i].name_length + 2;

                            pcnf->items[i].attribute_count = parms[offset++];

                            // while loop for attributes
                            for( j = 0; j< pcnf->items[i].attribute_count; j++){
                                if( j < BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID){
                                    pcnf->items[i].attributes[j].attribute_id = BEtoHost32( &parms[offset] );
                                    offset += 4;
                                    pcnf->items[i].attributes[j].charset = BEtoHost16( &parms[offset] );
                                    offset += 2;
                                    pcnf->items[i].attributes[j].attribute_value_len = BEtoHost16( &parms[offset] );
                                    offset += 2;

                                    len = pcnf->items[i].attributes[j].attribute_value_len;
									if( 0 != len ){
                                    OS_MemCopy( (U8 *) &pcnf->data[pcnf->data_len], (const U8 *) &parms[offset], len);
										pcnf->items[i].attributes[j].attribute_value_string_offset = pcnf->data_len;
                                    offset += len;
                                    // shift the data_len to the next string's index
                                    pcnf->data_len += len + 2;
									}

                                }else{
                                    // drop the data
                                    offset = offset + 4 + 2;    // skip the attribute id and charset
                                    len = BEtoHost16( &parms[offset] ); // attribute string
                                    offset = offset + 2 + len;  // skip the attribute string
                                    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_TOO_MANY_ITEM, j, BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID);
                                }
                            }
                            if( pcnf->items[i].attribute_count > BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID ){
                                pcnf->items[i].attribute_count = BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID;
                            }
                        }
                        break;
                        default:
                        {
                            kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UNKNOW_FOLDER_ITEM, pcnf->items[i].item_type);
                            i--;
                            parseable = FALSE; // stop the parsing
                            break;
                        }
                        break;
                    }
					
                }

                pcnf->result = BT_AVRCP_RESULT_SUCCESS;
                if( 1 == pcnf->counter && FALSE == parseable ){
                    // fault error. if there is only one item and fail to parse
                    pcnf->result = BT_AVRCP_RESULT_ERROR;
                    pcnf->error_code = error_code = BT_AVRCP_ERRCODE_INTERNAL_ERROR;
                }else{
                    // doulbe check the pcnf->counter
                    if( i > 0 && (U32) i != ((U32)pcnf->counter) ){
                        pcnf->counter = (U16) i;
                    }
                }
                
		        AvrcpHandleCBAVRCPRsp_Common(chnl_info, result, BTAVRCP_EVENT_GET_FOLDERITEMS_CNF, sizeof(BTAVRCPGetFolderItemsCnf), (U8 *)pcnf);
            }else{
            
				result = BT_AVRCP_RESULT_ERROR;
				if(  parm_len == 1){
					error_code = parms[0];
				}
				AvrcpHandleCBAVRCPRsp_ReportError(chnl_info, result, error_code, BTAVRCP_EVENT_SET_BROWSEDPLAYER_CNF, 0, (U8 *)NULL);
                AvrcpHandleCBAVRCPRspInd_ShowErrorMsg(chnl_info, pdu_id, parm_len);
            }
	    }	
        break;
        case AVRCP_BROWSE_OPCODE_CHANGE_PATH:
        {
    	    BTAVRCPChangePathCnf *pcnf;
			U32 offset = 0;

			pcnf = (BTAVRCPChangePathCnf *) &g_avrcp_data.p;
    	    
            if( parm_len >= 5 ){ // spec 6.10.4.1 page 83
				OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPChangePathCnf) );
				
				pcnf->result = BT_AVRCP_RESULT_SUCCESS;
                pcnf->status = parms[offset++];
                pcnf->num_of_items = BEtoHost32( &parms[offset] );
                offset += 4;

    		    AvrcpHandleCBAVRCPRsp_Common(chnl_info, result, BTAVRCP_EVENT_CHANGE_PATH_CNF, sizeof(BTAVRCPChangePathCnf), (U8 *)pcnf);
            }else{
				result = BT_AVRCP_ERRCODE_INVALID_PARAM;
				if( AVCTP_RESPONSE_REJECTED == avctpRsp->response && parm_len >= 1){
					result = parms[0];
				}
				AvrcpHandleCBAVRCPRsp_Common(chnl_info, result, BTAVRCP_EVENT_SET_BROWSEDPLAYER_CNF, 0, (U8 *)NULL);
                AvrcpHandleCBAVRCPRspInd_ShowErrorMsg(chnl_info, pdu_id, parm_len);
            }
        }
        break;
        case AVRCP_BROWSE_OPCODE_GET_ITEMATTRIBUTES:
        {
   		    BTAVRCPGetItemAttrCnf *pcnf; // browse cmd
			U32 len;

			pcnf = (BTAVRCPGetItemAttrCnf *) &g_avrcp_data.p;
			if( AVCTP_RESPONSE_REJECTED == avctpRsp->response ){
				OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPGetItemAttrCnf) );
				pcnf->result = BT_AVRCP_RESULT_ERROR;
				if( parm_len >= 1 ){
					pcnf->status = parms[0];
				}else{
					pcnf->status = 0x00;
				}
				return;
			}
            if( parm_len >= 2 ){ // spec 6.10.4.3 page 86
				OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPGetItemAttrCnf) );

                pcnf->status = parms[offset++];
                pcnf->counter = parms[offset++];
				pcnf->result = BT_AVRCP_RESULT_SUCCESS;

                if( pcnf->counter > BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE){
                    pcnf->counter = BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE;
                }
                
                for( i = 0; i< pcnf->counter && i < BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE ; i++ ){
					// parse the attribute value entry - page 79
	                pcnf->attribute_id[i] = BEtoHost32( &parms[offset] );
		            offset += 4;
			        pcnf->charset[i] = BEtoHost16( &parms[offset] );
				    offset += 2;
			        len = BEtoHost16( &parms[offset] ); // real offset
				    offset += 2;

					if( len < BT_AVRCP_MAX_VALUE_STRING_SIZE ){ // copy to strcuture as many as possible
						pcnf->attribute_value_len[i] = (U16) len;
					}else{
						pcnf->attribute_value_len[i] = BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE - 1;
					}
					
					OS_MemCopy( pcnf->attribute_value_string[i],  &parms[offset], pcnf->attribute_value_len[i]);
					offset += len;
                }

   		        AvrcpHandleCBAVRCPRsp_Common(chnl_info, result, BTAVRCP_EVENT_GET_ITEMATTRIBUTES_CNF, sizeof(BTAVRCPGetItemAttrCnf), (U8 *)pcnf);
            }else{
				result = BT_AVRCP_RESULT_ERROR;
				if( parm_len >= 1){
					error_code = parms[0];
				}
				AvrcpHandleCBAVRCPRsp_ReportError(chnl_info, result, error_code,BTAVRCP_EVENT_SET_BROWSEDPLAYER_CNF, 0, (U8 *)NULL);
                AvrcpHandleCBAVRCPRspInd_ShowErrorMsg(chnl_info, pdu_id, parm_len);
            }
        }
        break;
        case AVRCP_BROWSE_OPCODE_SEARCH:
        {
		    BTAVRCPSearchCnf *pcnf; 

			pcnf = (BTAVRCPSearchCnf *) &g_avrcp_data.p;

            if( parm_len >= 4 ){ // spec 6.11 page 87
				OS_MemSet( (U8 *) pcnf, 0, sizeof(BTAVRCPSearchCnf) );

				pcnf->result = BT_AVRCP_RESULT_SUCCESS;
				pcnf->status = parms[offset++];
                pcnf->uid_counter = BEtoHost16( &parms[offset] );
                offset += 2;
                pcnf->found_num_of_items = BEtoHost32( &parms[offset] );
                offset += 4;

				AvrcpHandleCBAVRCPRsp_Common(chnl_info, result, BTAVRCP_EVENT_SEARCH_CNF, sizeof(BTAVRCPSearchCnf), (U8 *)pcnf);
            }else{
				result = BT_AVRCP_ERRCODE_INVALID_PARAM;
				if( AVCTP_RESPONSE_REJECTED == avctpRsp->response && parm_len >= 1){
					result = parms[0];
				}
				AvrcpHandleCBAVRCPRsp_Common(chnl_info, result, BTAVRCP_EVENT_SET_BROWSEDPLAYER_CNF, 0, (U8 *)NULL);
                AvrcpHandleCBAVRCPRspInd_ShowErrorMsg(chnl_info, pdu_id, parm_len);
            }
        }
        break;
	    default:
	    // ignore the response packet
	    // AvrcpAssert(0);
	    OS_Report("warning! unknow response opcode");
	    break;
	}
}



/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCBAVRCPCmdInd_Receiver
 * DESCRIPTION
 *  Receive the AvrcpSpecific packet
 *  Handle the parsing of AVRCP specific packets
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  avctpCmd:  avc command's header and payload pointer
 *  pdu_id  :  avrcp pdu_id
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCBAVRCPCmdInd_Receiver(AvrcpChnlInfo *chnl_info, AvctpCmdFrame *avctpCmd, U8 pdu_id, U16 pdu_parm_len, U16 parm_len, U8 *parms){
    U8 packetType;
    U8 chnl_num = chnl_info->chnl_num;
	U8 overhead = (1+3+1+1+2); // opcode + avrcp header = 8B
	U8 packettype_offset = 3+1+1; // company-id(3B) pdu_id(1B) packettype(1B)

	// index 0 opcode
	// index 1~parm_len avrcp header: company-id(3B) pdu_id(1B) packettype(1B) length(2B)
	if( parm_len >= overhead){
        packetType = (parms[packettype_offset] & 0x3);
        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CMD_IND_RECEIVER, chnl_info->chnl_num, packetType, parm_len);
        
        if( AVRCP_PACKET_TYPE_SINGLE == packetType ){
            // directly pass it to dispatcher
            AvrcpHandleCBAVRCPCmdInd_Dispatcher(chnl_info, avctpCmd->ctype, pdu_id, pdu_parm_len, (U16)(parm_len-overhead), &parms[overhead]);
        }else{
            if( AVRCP_PACKET_TYPE_START == packetType ){
                frame_data_len[chnl_num] = 0;
                OS_MemCopy( &frame_data_buff[chnl_num][frame_data_len[chnl_num]], &parms[0], overhead);
                frame_data_len[chnl_num] += overhead;                
            }
        
            // put it into frame. if the frame size is too large throw data
            if( parm_len > overhead && (frame_data_len[chnl_num] + parm_len) < BT_AVRCP_MAX_RSP_FRAME_SIZE ){
                OS_MemCopy( &frame_data_buff[chnl_num][frame_data_len[chnl_num]], &parms[overhead], parm_len - overhead);
                frame_data_len[chnl_num] += (parm_len-overhead);
            }else{
                kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_THROW_MORE_DATA, parm_len-overhead, frame_data_len[chnl_num], BT_AVRCP_MAX_RSP_FRAME_SIZE);
            }

            if( AVRCP_PACKET_TYPE_END == packetType ){
                AvrcpHandleCBAVRCPCmdInd_Dispatcher(chnl_info, avctpCmd->ctype, pdu_id, frame_data_len[chnl_num], (U16)(frame_data_len[chnl_num]-overhead), &frame_data_buff[chnl_num][overhead]);
            }
        }
    }else{
        // not a AVRCP specific AV/C command. throw it
        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SMALL_PACKET_SIZE_FAIL, parm_len);
    }    

}

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCBAVRCPCmdInd_Dispatcher
 * DESCRIPTION
 *  Handle the parsing of AVRCP specific command packets
 *  When receiving a commmand packet, the avrcp profile uses this to
 *  dispatch payload to handlers by pdu-id
 *  Fragmented packet will be assembled by _receiver and 
 *  pass the whole packet to dispatcher when the assembled is done.
 *
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  avctpCmd:  avc header
 *  pdu_id  :  pdu_id of avrcp header
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCBAVRCPCmdInd_Dispatcher(AvrcpChnlInfo *chnl_info, U8 ctype, U8 pdu_id, U16 pdu_parm_len, U16 parm_len, U8 *parms){
    U8 result = (U8)BT_AVRCP_RESULT_SUCCESS;
    
	// Parse the content and dispatch the cmd indication to avrcpevents.c
	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CMD_IND_DISPATCHER, chnl_info->chnl_num, pdu_id, parm_len);
	
	switch(pdu_id){
		case AVRCP_SPECIFIC_OPCODE_GET_CAPABILITIES:
			// capabiility structure
			if( parms[0] == 0x02 ){
			    // reponse the BT SIG id directly
			    AVRCP_UtilSendBTSIGCapability(chnl_info, chnl_info->chnl_num);
            }else if( parms[0] == 0x03 ) { // company id
    			AvrcpHandleCBAVRCPCmdInd_Capabilities(
                    chnl_info, 
                    BT_AVRCP_RESULT_SUCCESS,
                    chnl_info->transId,
                    0,
                    ctype, //avctpCmd->ctype,
                    7,     //avctpCmd->subunitType,
                    0,     //avctpCmd->subunitId,
                    parms[0]
                    );
            }else{
				// Not in spec 6.4.1 Reject it
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_AV13_PDU_FAIL, pdu_id, parm_len);
				AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
			}
			break;
		case AVRCP_SPECIFIC_OPCODE_LIST_PLAYERAPP_SETTINGATTRIBUTE:
		    AvrcpHandleCBAVRCPCmdInd_ListPlayerAttrs(chnl_info, BT_AVRCP_RESULT_SUCCESS, 0);
			break;
		case AVRCP_SPECIFIC_OPCODE_LIST_PLAYERAPP_SETTINGVALUE:
			if( parm_len >= 1 ){
				AvrcpHandleCBAVRCPCmdInd_ListPlayerValues(chnl_info, BT_AVRCP_RESULT_SUCCESS, 0, parms[0]);
			}else{
				// Not confirm to spec 6.4.1 Reject it
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_AV13_PDU_FAIL, pdu_id, parm_len);
				AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
			}
			break;
		case AVRCP_SPECIFIC_OPCODE_GET_CURPLAYERAPP_SETTINGVALUE:
		    {
		        BTAVRCPGetCURPlayerValueInd *pind;
		        U8 i, offset;
		        
    			if( parm_len >= 1 ){
            		pind = (BTAVRCPGetCURPlayerValueInd *) &g_avrcp_data.p;
            		OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPGetCURPlayerValueInd));

    			    pind->count = parms[0];
    			    offset = 1;
    			    for( i=0 ; i< pind->count && (offset) < parm_len ; i++){
        			    pind->attribute_id[i] = parms[offset++];
        			}
    				AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_GET_CURPLAYERAPP_SETTINGVALUE_IND, sizeof(BTAVRCPGetCURPlayerValueInd), (U8 *)pind);
    			}else{
    				// Not confirm to spec 6.4.1 Reject it
    				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_AV13_PDU_FAIL, pdu_id, parm_len);
    				AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
    			}		
    		}
			break;
		case  AVRCP_SPECIFIC_OPCODE_SET_PLAYERAPP_SETTINGVALUE:
		    {
		        BTAVRCPSetPlayerValueInd *pind;
		        U8 i, offset;
		        
    			if( parm_len >= 1 ){
            		pind = (BTAVRCPSetPlayerValueInd *) &g_avrcp_data.p;
            		OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPSetPlayerValueInd));
            		    				
    			    pind->count = parms[0];
    			    offset = 1;
    			    for( i=0 ; i< pind->count && (offset) < parm_len ; i++){
        			    pind->attribute_id[i] = parms[offset++];
        			    pind->values[i] = parms[offset++];
        			}
    				AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_SET_PLAYERAPP_SETTINGVALUE_IND, sizeof(BTAVRCPSetPlayerValueInd),(U8 *)pind);
    			}else{
    				// Not confirm to spec 6.4.1 Reject it
    				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_AV13_PDU_FAIL, pdu_id, parm_len);
    				AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
    			}		
    		}
            break;
		case  AVRCP_SPECIFIC_OPCODE_GET_PLAYERAPP_SETTINGATTRIBUTETEXT:
		    {
		        BTAVRCPGetPlayerAttrTextInd *pind;
		        U8 i, offset;
		        
    			if( parm_len >= 1 ){
            		pind = (BTAVRCPGetPlayerAttrTextInd *) &g_avrcp_data.p;
            		OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPGetPlayerAttrTextInd));
    				
    			    pind->count = parms[0];
    			    offset = 1;
    			    for( i=0 ; i< pind->count && (offset) < parm_len ; i++){
        			    pind->attribute_id[i] = parms[offset++];
        			}
    				AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGATTRIBUTETEXT_IND, sizeof(BTAVRCPGetPlayerAttrTextInd), (U8 *)pind);
    			}else{
    				// Not confirm to spec 6.4.1 Reject it
    				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_AV13_PDU_FAIL, pdu_id, parm_len);
    				AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
    			}		
    		}
    		break;
		case  AVRCP_SPECIFIC_OPCODE_GET_PLAYERAPP_SETTINGVALUETEXT:
		    {
		        BTAVRCPGetPlayerValueTextInd *pind;
		        U8 i, offset;
		        
    			if( parm_len >= 2 ){
            		pind = (BTAVRCPGetPlayerValueTextInd *) &g_avrcp_data.p;
            		OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPGetPlayerValueTextInd));
            		    				
    			    // page 54
    			    pind->attribute_id = parms[0];
    			    pind->count = parms[1];
    			    offset = 2;
    			    for( i=0 ; i< pind->count && (offset) < parm_len && i< BT_AVRCP_MAX_ATTRIBUTE_VALUE_NUM; i++){
        			    pind->value_ids[i] = parms[offset++];
        			}
    				AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGVALUETEXT_IND, sizeof(BTAVRCPGetPlayerValueTextInd), (U8 *)pind);
    			}else{
    				// Not confirm to spec 6.5.6 Reject it
    				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_AV13_PDU_FAIL, pdu_id, parm_len);
    				AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
    			}		
    		}
            break;
		case  AVRCP_SPECIFIC_OPCODE_INFORM_DISPLAY_CHARSET:
		    {
		        BTAVRCPInformCharsetInd *pind;
		        U8 i, offset;

		        // sepc 6.5.7
    			if( parm_len >= 1 ){
            		pind = (BTAVRCPInformCharsetInd *) &g_avrcp_data.p;
            		OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPInformCharsetInd));
            		    				
    			    offset = 0;
    			    pind->count = parms[offset++];
    			    for( i=0; i< pind->count && i < BT_AVRCP_MAX_ATTRIBUTE_NUM && offset < parm_len; i++ ){
        			    pind->charset_ids[i] = BEtoHost16(&parms[offset]);
        			    offset += 2;
        			}
    				AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_INFORM_DISPLAY_CHARSET_IND, sizeof(BTAVRCPInformCharsetInd), (U8 *)pind);
    			}else{
    				// Not confirm to spec 6.4.1 Reject it
    				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_AV13_PDU_FAIL, pdu_id, parm_len);
    				AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
    			}		
    		}
            break;
		case  AVRCP_SPECIFIC_OPCODE_INFORM_BATTERY_STATUSOFCT:
		    {
		        BTAVRCPInformBattOfCTInd *pind;
		        U8 offset;

		        // sepc 6.5.7
    			if( parm_len >= 1 ){
            		pind = (BTAVRCPInformBattOfCTInd *) &g_avrcp_data.p;
            		OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPInformBattOfCTInd));
            		    				
    			    offset = 0;
    			    pind->battery_status = parms[offset++];
    				AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_INFORM_BATTERY_STATUSOFCT_IND, sizeof(BTAVRCPInformBattOfCTInd), (U8 *)pind);
    			}else{
    				// Not confirm to spec 6.4.1 Reject it
    				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_AV13_PDU_FAIL, pdu_id, parm_len);
    				AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
    			}		
    		}
            break;

		case  AVRCP_SPECIFIC_OPCODE_GET_ELEMENT_ATTRIBUTES:
		    {
		        BTAVRCPGetElementAttrInd *pind;
		        U8 i, offset;

		        // sepc 6.5.7
    			if( parm_len >= 9 ){
            		pind = (BTAVRCPGetElementAttrInd *) &g_avrcp_data.p.ABTAVRCPGetElementAttrInd;
            		OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPGetElementAttrInd));
            		    				
    			    offset = 0;
    			    OS_MemCopy( (U8 *) pind->identifier, (const U8 *)&parms[offset], 8);
                    offset += 8;

                    pind->count = parms[offset++];
                    if( pind->count > BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM ){
                        pind->count = BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM;
                    }
                    for( i=0; i<pind->count; i++){
                        pind->attributes[i] = BEtoHost32(&parms[offset]);
                        offset += 4;
                    }

    				AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_GET_ELEMENT_ATTRIBUTES_IND, sizeof(BTAVRCPGetElementAttrInd), (U8 *)pind);
    			}else{
    				// Not confirm to spec 6.4.1 Reject it
    				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_AV13_PDU_FAIL, pdu_id, parm_len);
    				AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
    			}		
    		}
            break;
		case  AVRCP_SPECIFIC_OPCODE_GET_PLAYERAPP_STATUS:
		    {
    		    BTAVRCPGetPlayerStatusInd *pind;
            	pind = (BTAVRCPGetPlayerStatusInd *) &g_avrcp_data.p;
            	OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPGetPlayerStatusInd));
            		    		    
		        AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_GET_PLAYERAPP_STATUS_IND, sizeof(BTAVRCPGetPlayerStatusInd), (U8 *)pind);
		    }
		    break;
		case  AVRCP_SPECIFIC_OPCODE_REGISTER_NOFITICATION:
		    {
		        BTAVRCPRegisterNotifyInd *pind;
		        if( parm_len >= 1 ){
            		pind = (BTAVRCPRegisterNotifyInd *) &g_avrcp_data.p;
            		OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPRegisterNotifyInd));
            				        	
		            pind->event_id = (U8) parms[0];
		            if( parm_len >= 5 ){
    		            pind->playback_interval  = (U32) parms[1] << 24;
    		            pind->playback_interval += (U32) parms[2] << 16;
    		            pind->playback_interval += (U32) parms[3] << 8;
    		            pind->playback_interval += (U32) parms[4] ;
    		        }else{
    		            pind->playback_interval = 0;
    		        }
    		        if( pind->event_id == 0 || pind->event_id > AVRCP_NOTIFY_EVENT_VOLUME_CHANGED ){
    		            OS_Report("[AVRCP][WRN] Invalide event_id:%d reject it !", pind->event_id );
    				    AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);    		                                
    		        }else{
        		        AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, BT_AVRCP_RESULT_SUCCESS, BTAVRCP_EVENT_REGISTER_NOFITICATION_IND, sizeof(BTAVRCPRegisterNotifyInd), (U8 *)pind );

    	            }
                }else{
                	// Not confirm to spec 6.4.1 Reject it
					kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_AV13_PDU_FAIL, pdu_id, parm_len);
				    AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
                }
            }
		break;
		case  AVRCP_SPECIFIC_OPCODE_REQUEST_CONTINUING:
		{
			avrcp_adp_context *cntx_ptr = NULL;
			AvrcpChannel *chnl;
			BtStatus status;
			ASSERT(chnl_info->chnl_num < BT_AVRCP_MAX_CHANNEL);
			cntx_ptr = &avrcp_cntx[chnl_info->chnl_num];
			chnl = &cntx_ptr->rcp_channel;

        	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_RECEIVE_CONTINUE_PACKET, parm_len);
		    // check the current chnl->curQueuePack
		    if( NULL != chnl->curQueueFrame && parm_len >= 1 ){
				if( NULL != chnl->curQueueFrame && chnl->curQueueFrame->pdu_id == parms[0] ){
				    
				    AvrcpCancelContinueTimer(chnl);
				    
					// check pdu_id
   					status = AVRCP_ContinueQueuePacket(chnl);
   					if( BT_STATUS_PENDING == status ){
   					     /* Start Timer */
   					     AvrcpStartContinueTimer(chnl);
   					}else{
   						kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CONTINUE_SEND_FAIL, status);
   					}
                    
				}else{
					kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PDUID_NOT_MATCH, chnl->curQueueFrame->pdu_id, parms[0]);
					AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
				}
    		}else if( 0 == parm_len){
                AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
    		}else{
    		    // error case
    		    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CONTI_NO_DATA);
       		    AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_CMD);
    		}
		}
		break;
		case  AVRCP_SPECIFIC_OPCODE_ABORT_CONTINUING:
		    {
    			avrcp_adp_context *cntx_ptr = NULL;
    			AvrcpChannel *chnl;
				BTAVRCPAbortContinueInd *pind;

    			ASSERT(chnl_info->chnl_num < BT_AVRCP_MAX_CHANNEL);
    			cntx_ptr = &avrcp_cntx[chnl_info->chnl_num];
    			chnl = &cntx_ptr->rcp_channel;
			
                if( parm_len >= 1 ){ // spec 6.8.2 page 64
            		pind = (BTAVRCPAbortContinueInd *) &g_avrcp_data.p;
            		OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPAbortContinueInd));
            		                	
                    pind->pdu_id = parms[0]; // parms[0] is the previous cmd's pdu-id
                   
                    if( NULL != chnl->curQueueFrame ){
            			pind = (BTAVRCPAbortContinueInd *) &g_avrcp_data.p;
            			OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPAbortContinueInd));
            		                    	
						// notify mmi
        		        AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_ABORT_CONTINUING_IND, sizeof(BTAVRCPAbortContinueInd), (U8 *)pind);

						// abort the curQueueFrame
						kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_ABORT_CONTINUE, chnl->curQueueFrame);
						Assert( (chnl->curQueueFrame != NULL) );        

						/* Reset */
						InsertTailList( &(avrcp_cntx[chnl_info->chnl_num].freePacketList) , (ListEntry *)chnl->curQueueFrame);
						chnl->curQueueFrame = NULL;
                        chnl->curTxState = AVRCP_TX_IDLE;
        			    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UPDATE_TXSTATE , chnl->curTxState);
						
						/* Stop the timer */
			            AvrcpCancelContinueTimer(chnl);
						
						AVRCP_UtilAVRCPCmdBufferRsp(chnl_info, AVCTP_RESPONSE_ACCEPTED, pdu_id, 0 , NULL);
        		    }else{
        		        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PDUID_NOT_MATCH, 0xff, pind->pdu_id);
            		    AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
        		    }
                }else{
                    AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
                }
		    }
        break;
		case  AVRCP_SPECIFIC_OPCODE_SET_ABSOLUTE_VOLUME:
		    {
    		    BTAVRCPSetAbsoluteVolumeInd *pind;
                if( parm_len >= 1 ){ // spec 6.13.2 page 91
            		pind = (BTAVRCPSetAbsoluteVolumeInd *) &g_avrcp_data.p;
            		OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPSetAbsoluteVolumeInd));
            		                	
                    pind->volume = parms[0];
        	        AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_SET_ABSOLUTE_VOLUME_IND, sizeof(BTAVRCPSetAbsoluteVolumeInd), (U8 *)pind);
                }else{
                    AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
                }
		    }		
		break;
		case  AVRCP_SPECIFIC_OPCODE_SET_ADDRESSEDPLAYER:
		    {
    		    BTAVRCPSetAddrPlayerInd *pind;
                if( parm_len >= 2 ){ // spec 6.9.1 page 66
            		pind = (BTAVRCPSetAddrPlayerInd *) &g_avrcp_data.p;
            		OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPSetAddrPlayerInd));
            		                	
                    pind->player_id = BEtoHost16( &parms[0] );
    		        AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_SET_ADDRESSEDPLAYER_IND, sizeof(BTAVRCPSetAddrPlayerInd), (U8 *)pind);
                }else{
                    AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
                }
		    }		
		break;
		case  AVRCP_SPECIFIC_OPCODE_PLAY_ITEMS:
    	    {
    		    BTAVRCPPlayItemsInd *pind; 
    		    U32 offset = 0;
                if( parm_len >= 11 ){ // spec 6.12.1 page 89
            		pind = (BTAVRCPPlayItemsInd *) &g_avrcp_data.p;
            		OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPPlayItemsInd));
            		                	
                    pind->scope = parms[offset++];
                    OS_MemCopy((U8 *)pind->uid, (const U8 *)&parms[offset], 8);
                    offset += 8;
                    pind->uid_counter = BEtoHost16( &parms[offset] );
                    offset += 2;

       		        AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_PLAY_ITEMS_IND, sizeof(BTAVRCPPlayItemsInd), (U8 *)pind);
                }else{
                    AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
                }
		    }
		break;
		case  AVRCP_SPECIFIC_OPCODE_ADD_TONOWPLAYING:
    	    {
    		    BTAVRCPAddNowPlayingInd *pind; 
    		    U32 offset = 0;
                if( parm_len >= 11 ){ // spec 6.12.2 page 90
            		pind = (BTAVRCPAddNowPlayingInd *) &g_avrcp_data.p;
            		OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPAddNowPlayingInd));
            		                	
                    pind->scope = parms[offset++];
                    OS_MemCopy((U8 *)pind->uid, (const U8 *)&parms[offset], 8);
                    offset += 8;
                    pind->uid_counter = BEtoHost16( &parms[offset] );
                    offset += 2;
       		        AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_ADD_TONOWPLAYING_IND, sizeof(BTAVRCPAddNowPlayingInd), (U8 *)pind);
                }else{
                    AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
                }
		    }
		break;
		// browse channel's PDU-id
		case  AVRCP_BROWSE_OPCODE_SET_BROWSEDPLAYER:
		case  AVRCP_BROWSE_OPCODE_GET_FOLDERITEMS:
		case  AVRCP_BROWSE_OPCODE_CHANGE_PATH:
		case  AVRCP_BROWSE_OPCODE_GET_ITEMATTRIBUTES:
		case  AVRCP_BROWSE_OPCODE_SEARCH:
			{
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_REJECT_BWS_PACKET);
				AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_CMD);
			}
			break;
		default:
			{   // AV13:add
    			AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_CMD);
			}
			break;
	}
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCBAVRCPBrowseCmdInd_Dispatcher
 * DESCRIPTION
 *  Handle the parsing of browse command of AVRCP browse packets
 *  The whole AV/C&browse payload data has been received (not fragmentable).
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  avctpCmd:  avc header
 *  pdu_id  :  opcode in AVRCP browse header
 *  
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCBAVRCPBrowseCmdInd_Dispatcher(AvrcpChnlInfo *chnl_info, AvctpCmdFrame *avctpCmd, U8 pdu_id, U16 pdu_parm_len, U16 parm_len, U8 *parms){
    U8 result = (U8)BT_AVRCP_RESULT_SUCCESS;

    OS_Report("[AVRCP] AvrcpHandleCBAVRCPBrowseCmdInd_Dispatcher pdu_id:%d parm_len:%d", pdu_id, parm_len);
	// Parse the content and dispatch the cmd indication to avrcpevents.c
	switch(pdu_id){
        case AVRCP_BROWSE_OPCODE_SET_BROWSEDPLAYER:
	    {
		    BTAVRCPSetBrowsePlayerInd *pind;
            if( parm_len >= 2 ){ // spec 6.9.3 page 69
            	pind = (BTAVRCPSetBrowsePlayerInd *) &g_avrcp_data.p.ABTAVRCPSetBrowsePlayerInd;
            	OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPSetBrowsePlayerInd));
            	            	
                pind->player_id = BEtoHost16( &parms[0] );
		        AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_SET_BROWSEDPLAYER_IND, sizeof(BTAVRCPSetBrowsePlayerInd), (U8 *)pind);
            }else{
                AVRCP_UtilSendAVRCPBrowseReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
            }
	    }		
        break;
        case AVRCP_BROWSE_OPCODE_GET_FOLDERITEMS:
	    {
		    BTAVRCPGetFolderItemsInd *pind;
		    U32 offset = 0, i;
            if( parm_len >= 10 ){ // spec 6.10.4.2 page 84
            	pind = (BTAVRCPGetFolderItemsInd *) &g_avrcp_data.p.ABTAVRCPGetFolderItemsInd;
            	OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPGetFolderItemsInd));
            	OS_Report("[AVRCP] get_folderitems 0:%d 8:%d 9:%d", parms[0], parms[8], parms[9]);
                pind->scope = parms[offset++];
                pind->start_item = BEtoHost32( &parms[offset] );
                offset += 4;
                pind->end_item = BEtoHost32( &parms[offset] );
                offset += 4;
                OS_Report("[AVRCP] get_folderitems offset:(%d) count:(%d)", offset, parms[offset]);
                pind->count = parms[offset++];

				/* when count is 0xFF, it means no attributes are requested. spec page85 */
                if( pind->count > BT_AVRCP_MAX_ATTRIBUTE_NUM && pind->count != 0xFF){
                    pind->count = BT_AVRCP_MAX_ATTRIBUTE_NUM ;
                }
                for( i = 0; i< pind->count && pind->count != 0xFF ; i++ ){
                    pind->attribute_ids[i]=  BEtoHost32( &parms[offset] );
                    offset += 4; // each attribute id is 4 Octet
                }
                
		        AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_GET_FOLDERITEMS_IND, sizeof(BTAVRCPGetFolderItemsInd), (U8 *)pind);                    
            }else{
                AVRCP_UtilSendAVRCPBrowseReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
            }
	    }	
        break;
        case AVRCP_BROWSE_OPCODE_CHANGE_PATH:
        {
    	    BTAVRCPChangePathInd *pind;
    	    U32 offset = 0;
            if( parm_len >= 11 ){ // spec 6.10.4.1 page 83
            	pind = (BTAVRCPChangePathInd *) &g_avrcp_data.p.ABTAVRCPChangePathInd;
            	OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPChangePathInd));
            	
                pind->uid_counter = BEtoHost16( &parms[offset] );
                offset += 2;
                pind->direction = parms[offset++];
                OS_MemCopy((U8 *)pind->folder_uid, (const U8 *)&parms[offset], 8);
                offset += 8;
    		        AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_CHANGE_PATH_IND, sizeof(BTAVRCPChangePathInd), (U8 *)pind);
            }else{
                AVRCP_UtilSendAVRCPBrowseReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
            }
        }
        break;
        case AVRCP_BROWSE_OPCODE_GET_ITEMATTRIBUTES:
        {
   		    BTAVRCPGetItemAttrInd *pind; // browse cmd
		    U32 offset = 0, i;
            if( parm_len >= 12 ){ // spec 6.10.4.3 page 86
            	pind = (BTAVRCPGetItemAttrInd *) &g_avrcp_data.p.ABTAVRCPGetItemAttrInd;
            	OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPGetItemAttrInd));
            	
                pind->scope = parms[offset++];
                OS_MemCopy((U8 *)pind->uid, (const U8 *)&parms[offset], 8);
                offset += 8;
                pind->uid_counter = BEtoHost16( &parms[offset] );
                offset += 2;
                pind->count = parms[offset++];
                if( pind->count > BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID){
                    pind->count = BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID;
                }
                
                for( i = 0; i< pind->count; i++ ){
                    pind->attribute_ids[i]=  BEtoHost32( &parms[offset] );
                    offset += 4; // each attribute id is 4 Octet                    
                }
   		        AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_GET_ITEMATTRIBUTES_IND, sizeof(BTAVRCPGetItemAttrInd), (U8 *)pind);
            }else{
                AVRCP_UtilSendAVRCPBrowseReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
            }
        }
        break;
        case AVRCP_BROWSE_OPCODE_SEARCH:
        {
		    BTAVRCPSearchInd *pind; 
		    U32 offset = 0;
            if( parm_len >= 4 ){ // spec 6.11 page 87
            	pind = (BTAVRCPSearchInd *) &g_avrcp_data.p.ABTAVRCPSearchInd;
            	OS_MemSet( (U8 *) pind, 0, sizeof(BTAVRCPSearchInd));
            	
                pind->charset = BEtoHost16( &parms[offset] );
                offset += 2;
                pind->length = BEtoHost16( &parms[offset] );
                offset += 2;
                OS_MemSet((U8 *)pind->search_string, 0, sizeof(pind->search_string));
                if( pind->length >= sizeof(pind->search_string)){
                    pind->length = sizeof(pind->search_string) - 1 ;
                }
                OS_MemCopy((U8 *)pind->search_string, (const U8 *)&parms[offset], pind->length);
                offset += pind->length;
   		        AvrcpHandleCBAVRCPCmdInd_Common(chnl_info, result, BTAVRCP_EVENT_SEARCH_IND, sizeof(BTAVRCPSearchInd), (U8 *)pind);
            }else{
                AVRCP_UtilSendAVRCPBrowseReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_PARAM);
            }
        }
        break;		
		default:
		    // use General Reject! not pdu_id
		    //AVRCP_UtilSendAVRCPBrowseReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_CMD);
		    AVRCP_UtilSendAVRCPBrowseReject(chnl_info, AVRCP_SPECIFIC_OPCODE_GENERAL_REJECT, BT_AVRCP_ERRCODE_INVALID_CMD);		    
		    
		break;
	}
}

/*****************************************************************************
 * FUNCTION
 *	AvrcpHandleCBAVCCommandInd
 * DESCRIPTION
 *  Handle the parsing of AVRCP specific command packets
 *  Input index 0 is the opcode or pdu-id
 *  Input pdu-id is 
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  avctpCmd:  avc header
 *  pdu_id  :  av
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCBAVCCommandInd(AvrcpChnlInfo *chnl_info, AvctpCmdFrame *avctpCmd, U16 operandLen,const U8 *operands){
	U8 pdu_id = 0;
	U16 pdu_parm_len;
        U8 l_NewOperands[9];
    // AvctpCtype ctype,U8 subunitType,U8 subunitId, AvctpOpcode opcode, U16 operandLen, U8 *operands){
    // parms[0] is the opcode of AV/C header
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CB_CMD_IND, avctpCmd->opcode, operandLen);
    
    switch(avctpCmd->opcode){
#if defined( __BT_AVRCP_V14__ ) || defined( __BT_AVRCP_V13__ )
        case AVRCP_OPCODE_VENDOR_DEPENDENT: // vendor data. parse the data
			{
				/// AVRCP specific command header: opcode(1B) + ( (3B) (1B) (1B) (2B) )  
				Assert( operands != NULL );
				if( operandLen >= 8 && NULL != operands ){
					// check BT SIG company ID
					if( 0x00 != operands[1] || 0x19 != operands[2] || 0x58 != operands[3] ){
						kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_NOT_SIG_COMPANY_ID, operands[1], operands[2], operands[3]);
					}

					pdu_id = operands[4];
					// operands[5] is ptype - ignore it
					pdu_parm_len = 0; 
					pdu_parm_len = ((U16)operands[6] << 8) + operands[7];
					if( pdu_parm_len != (operandLen-8) ){
					//	Assert(0);
					}
					AvrcpHandleCBAVRCPCmdInd_Receiver(chnl_info, (AvctpCmdFrame *)avctpCmd, (U8)pdu_id, (U16)pdu_parm_len, (U16)(operandLen), (U8 *)&operands[0]);
				}else{
					// reject as a non-implement code
					if( operandLen > 4){
					    pdu_id = operands[4];
					}
					kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SMALL_PACKET_SIZE_FAIL, operandLen );
					AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_CMD);
				}
			}
            break;
#endif            
		case AVCTP_OPCODE_UNIT_INFO:
		case AVCTP_OPCODE_SUBUNIT_INFO:
			{

				g_avrcp_unit_send_data[0] = avctpCmd->opcode; // AV/C header opcode
				g_avrcp_unit_send_data[1] = 0x07;             // AV/C 
				g_avrcp_unit_send_data[2] = 0x48;             // unit type: panel & Unit: 0x0(first unit)
				g_avrcp_unit_send_data[3] = 0xFF;             // company id(3Byte) 0xffffff
				g_avrcp_unit_send_data[4] = 0xFF;
				g_avrcp_unit_send_data[5] = 0xFF;
				// this is a avrcp request
				// 0x48 is "subtype:panel(0x9) + subunitid:0"
				// 6 is operands length of avrcp packet's operands
				// avctpCmd->opcode is avrcp 's operation code
				// avctpCmd->opcode is 0x30(AVCTP_OPCODE_UNIT_INFO) 

				// pts4.4 TC_TG_ICC_BV_01_I sends sub-unit-type 7. Sub-unit-type only 3 bits

				// use original way to send data back
				AVRCP_UitlCmdFrameRspInd(
				chnl_info->chnl_num,
				chnl_info->transId,
				AVCTP_RESPONSE_IMPLEMENTED_STABLE,
				0x1f,  //0x9
				(avctpCmd->subunitId & 7),  // 0x0
				6, g_avrcp_unit_send_data); // first byte is avrcp opcode. others are avrcp-Operands
				// 0x9 is AVRCP_SUBUNITTYPE_PANEL_VALUE. 0 is SUBUNITID

			}
			break;
        case AVRCP_OPCODE_PASS_THROUGH: // pass through data, pass to old frameCmdId
			{   // operands[0] is opcode
                AvrcpHandleCommandFrameInd(
                chnl_info->chnl_num,
                BT_AVRCP_RESULT_SUCCESS,
                chnl_info->transId,
                0,
                avctpCmd->ctype,
                avctpCmd->subunitType,
                avctpCmd->subunitId,
                operandLen,
                operands);
			}
            break;
#ifdef __BT_AVRCP_V14__            
		case AVRCP_OPCODE_BROWSE_CAHNNEL:
			{
				if( operandLen >= 3 && NULL != operands ){
					// operands[0] is the 0xff opcode
					pdu_id = operands[1];
					pdu_parm_len = BEtoHost16(&operands[2]);
					AvrcpHandleCBAVRCPBrowseCmdInd_Dispatcher(chnl_info, (AvctpCmdFrame *)avctpCmd, (U8)pdu_id, (U16)pdu_parm_len, (U16)(operandLen-3), (U8 *)&operands[4]);
				}else{
					// reject ?
					if( operandLen >= 1 && NULL != operands ){
					    AVRCP_UtilSendAVRCPBrowseReject(chnl_info, operands[0], BT_AVRCP_ERRCODE_INVALID_PARAM);
					}else{
					    AVRCP_UtilSendAVRCPBrowseReject(chnl_info, 0, BT_AVRCP_ERRCODE_INVALID_PARAM);
					}
				}
			}
			break;            
#endif			       
        default:
            // 
			kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UNKNOW_AV_TYPE, avctpCmd->opcode);
#if defined( __BT_AVRCP_V14__ ) || defined( __BT_AVRCP_V13__ )
			// don't return any packet when compile as 0!
			AVRCP_UtilSendAVRCPNotImplement(chnl_info, operandLen, (U8 *)operands);
#else
			// use original way to send data back
#if 1
	        // rejec the request
	        if( operandLen == 9 ){
                // PS TC_TG_INV_BI_01_C
                OS_MemCopy( (U8 *) l_NewOperands, (const U8 *) operands,  operandLen);
                // use offset to setup the status byte
                l_NewOperands[8] = BT_AVRCP_ERRCODE_INVALID_CMD; //
                
                AVRCP_UitlCmdFrameRspInd(
				chnl_info->chnl_num,
				chnl_info->transId,
				AVCTP_RESPONSE_REJECTED,
				0x9,  //0x9
				0x0,  //0x0
				operandLen, l_NewOperands);
                }else{
			    AVRCP_UitlCmdFrameRspInd(
				chnl_info->chnl_num,
				chnl_info->transId,
				AVCTP_RESPONSE_REJECTED,
				0x9,  //0x9
				0x0,  //0x0
				operandLen, operands);
                }
#endif
#endif
            break;
    }
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpHandleCBAVCResponse
 * DESCRIPTION
 *  Handle the parsing of AVCTP response packet
 *  index 0 is the opcode of AVCTP packet
 *  pdu-id is the avrcp specific header
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  avctpCmd:  avc header
 *  pdu_id  :  avrcp header's pdu-id
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpHandleCBAVCResponse(AvrcpChnlInfo *chnl_info, AvctpRspFrame *avctpRsp, U16 operandLen, U8 *operands){
	U8 pdu_id = 0;
	U16 pdu_parm_len;

	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_AVCTP_AVC_RESPONSE, avctpRsp->opcode, operandLen);
    // AvctpCtype ctype,U8 subunitType,U8 subunitId, AvctpOpcode opcode, U16 operandLen, U8 *operands){
    // dispatch the handle of each opcode
    
    switch(avctpRsp->opcode){
        case AVRCP_OPCODE_VENDOR_DEPENDENT: // vendor data. parse the data
			{
				/// AVRCP specific command header: opcode(1B) + ( (3B) (1B) (1B) (2B) )  
				Assert( operands != NULL );
				if( operandLen >= 8 && NULL != operands ){
					// check BT SIG company ID
					if( 0x00 != operands[1] || 0x19 != operands[2] || 0x58 != operands[3] ){
						kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_NOT_SIG_COMPANY_ID, operands[1], operands[2], operands[3]);
					}

					pdu_id = operands[4];
					// operands[5] is ptype - ignore it
					pdu_parm_len = 0; 
					pdu_parm_len = ((U16)operands[6] << 8) + operands[7];
					if( pdu_parm_len != (operandLen-8) ){
						kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PDU_LENGTH_NOT_MATCH, pdu_parm_len, (operandLen-8));
					}
					AvrcpHandleCBAVRCPRsp_Receiver(chnl_info, (AvctpRspFrame *)avctpRsp, (U8)pdu_id, (U16)pdu_parm_len, (U16)(operandLen), (U8 *)&operands[0]);
				}else{
					// reject as a non-implement code
					if( operandLen > 4){
					    pdu_id = operands[4];
					}
					AVRCP_UtilSendAVRCPReject(chnl_info, pdu_id, BT_AVRCP_ERRCODE_INVALID_CMD);
					// AvrcpAssert(0);
					OS_Report("warning ! Reject the invalid cmd");
				}
				
			}
            break;
        case AVRCP_OPCODE_PASS_THROUGH: // pass through data, pass to old frameCmdId
			{   
				AvrcpHandleCommandFrameCnf(
					chnl_info->chnl_num,
					BT_AVRCP_RESULT_SUCCESS,
					chnl_info->transId,
					avctpRsp->response,
					avctpRsp->subunitType,
					avctpRsp->subunitId,
					avctpRsp->operandLen,
					operands);
			}
            break;
		case AVRCP_OPCODE_BROWSE_CAHNNEL:
			{
				// operands[0] is opcode AVCTP header
				// operands[1] is pdu_id of AVRCP
				// operands[2~3] is payload length of AVRCP
				if( operandLen >= 3 && NULL != operands ){
					pdu_id = operands[1];
					pdu_parm_len = BEtoHost16(&operands[2]);
					// browse packet is not fragmentable by spec.
					AvrcpHandleCBAVRCPRsp_Dispatcher(chnl_info, avctpRsp->response, (U8)pdu_id, (U16)pdu_parm_len, (U16)(operandLen-4), (U8 *)&operands[4]);
				}else{
					// Ignore it.
					kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_SMALL_PACKET_SIZE_FAIL, operandLen);
				}
			}
			break;
        default:
   			kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UNKNOW_AV_TYPE, avctpRsp->opcode);
            break;
    }
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpUtilCallbackMoreData
 * DESCRIPTION
 *  Receive more AVCTP packets which uses the same transID
 *  channel's rxBuffer is used to put the AVCTP data
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  Parms   :  callback data
 * RETURNS
 *  void
 *****************************************************************************/
static void AvrcpUtilCallbackMoreData(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    avrcp_adp_context *cntx_ptr;
    U8 chnl_num;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    chnl_num = AvrcpUtilCallbackFindChannel(chnl);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];

    kal_trace( BT_TRACE_G8_AVRCP, AVRCP_CALLBACK_EVENT_CMD_MORE_DATA, cntx_ptr->rcp_role, cntx_ptr->state);

    /* After Receive Command Response */
    if (cntx_ptr->cmd_more_data == AVRCP_CMD_MORE)
    {
        /* Check if the same frame to append */
        if (!(cntx_ptr->cmdFrame.ctype == Parms->p.cmdFrame->ctype &&
              cntx_ptr->cmdFrame.subunitType == Parms->p.cmdFrame->subunitType &&
              cntx_ptr->cmdFrame.subunitId == Parms->p.cmdFrame->subunitId &&
              chnl->rxBuffer[0] == Parms->p.cmdFrame->opcode))
        {
        	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_AVC_HEADER_NOT_MATCH);
            return;
        }

		if( cntx_ptr->cmdFrame.operandLen > sizeof(chnl->rxBuffer) ){
			kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_THROW_MORE_DATA, cntx_ptr->cmdFrame.operandLen, Parms->p.cmdFrame->operandLen, sizeof(chnl->rxBuffer));
			return;
		}

        /* Assemble AVCTP packet - Append remaining data */
          OS_MemCopy(
            &chnl->rxBuffer[cntx_ptr->cmdFrame.operandLen],
            Parms->p.cmdFrame->operands,
            Parms->p.cmdFrame->operandLen);            
        cntx_ptr->cmdFrame.operandLen += Parms->p.cmdFrame->operandLen; /* Put opcode in the buffer. */

        ASSERT(cntx_ptr->cmdFrame.operandLen <= BT_AVRCP_MAX_CMD_FRAME_SIZE);

        if (!Parms->p.cmdFrame->more)
        {
	    	AvrcpChnlInfo myChnlInfo;
        
		    myChnlInfo.chnl_num = chnl_num;
    		myChnlInfo.transId = Parms->transId;        
            cntx_ptr->cmd_more_data = AVRCP_NO_DATA;

            AvrcpHandleCBAVCCommandInd(&myChnlInfo,
                Parms->p.cmdFrame,
                (U16)(cntx_ptr->cmdFrame.operandLen),
                (U8 *)chnl->rxBuffer);
        }
    }
    /* After Receive Response Response */
    else if (cntx_ptr->rsp_more_data == AVRCP_RSP_MORE)
    {
        /* Check if the same frame to append */
        if (!(cntx_ptr->rspFrame.response == Parms->p.rspFrame->response &&
              cntx_ptr->rspFrame.subunitType == Parms->p.rspFrame->subunitType &&
              cntx_ptr->rspFrame.subunitId == Parms->p.rspFrame->subunitId &&
              chnl->rxBuffer[0] == Parms->p.cmdFrame->opcode))
        {
            kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_AVC_HEADER_NOT_MATCH);
            return;
        }

		if( cntx_ptr->rspFrame.operandLen > sizeof(chnl->rxBuffer) ){
			kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_THROW_MORE_DATA, cntx_ptr->rspFrame.operandLen, Parms->p.rspFrame->operandLen, sizeof(chnl->rxBuffer));
			return;
		}

        /* Assemble AVCTP packet - Append remaining data */
        OS_MemCopy(
            &chnl->rxBuffer[cntx_ptr->rspFrame.operandLen],
            Parms->p.rspFrame->operands,
            Parms->p.rspFrame->operandLen);

        cntx_ptr->rspFrame.operandLen += Parms->p.rspFrame->operandLen; /* Put opcode in the buffer. */

        ASSERT(cntx_ptr->rspFrame.operandLen <= BT_AVRCP_MAX_AV_FRAME_SIZE);

        if (!Parms->p.rspFrame->more)
        {
	    	AvrcpChnlInfo myChnlInfo;
        
		    myChnlInfo.chnl_num = chnl_num;
    		myChnlInfo.transId = Parms->transId;        
            cntx_ptr->rsp_more_data = AVRCP_NO_DATA;
             if((U16) (cntx_ptr->rspFrame.operandLen) < BT_AVRCP_MAX_AV_FRAME_SIZE){
			AvrcpHandleCBAVCResponse(&myChnlInfo, 
                Parms->p.rspFrame,
                (U16) (cntx_ptr->rspFrame.operandLen),  // array 0 is opcode
                chnl->rxBuffer);            
             }else{
                 kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_THROW_MORE_DATA, cntx_ptr->rspFrame.operandLen, Parms->p.rspFrame->operandLen, sizeof(chnl->rxBuffer));
             }
        }
    }else{
        kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_NOT_IN_MORE_STATE );
    }

}


/*****************************************************************************
 * FUNCTION
 *  AvrcpUtilCallbackTxDone
 * DESCRIPTION
 *  The avrcp packet has been sent and there is a TxDone event from under layer
 * 
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  Parms       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
static void AvrcpUtilCallbackTxDone(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    avrcp_adp_context *cntx_ptr;
    U8 chnl_num;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    chnl_num = AvrcpUtilCallbackFindChannel(chnl);
    AvrcpAssert(chnl_num < BT_AVRCP_MAX_CHANNEL);
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, AVRCP_CALLBACK_EVENT_CMD_TX_DONE, cntx_ptr->rcp_role, cntx_ptr->state, Parms->p.cmdFrame->ctype );
#endif    

    /* Frame sent */
    switch (Parms->p.cmdFrame->ctype)
    {
        case AVRCP_CTYPE_CONTROL:
        case AVRCP_CTYPE_STATUS:
        case AVRCP_CTYPE_SPECIFIC_INQUIRY:
        case AVRCP_CTYPE_NOTIFY:
        case AVRCP_CTYPE_GENERAL_INQUIRY:
			//OS_Report("[AVRCP][UTIL] cmdFrame:0x%x cntx.cmd:0x%x cntx.chnl.cmd:0x%x", Parms->p.cmdFrame, &(cntx_ptr->rcp_channel.curTx.avrcpCmd), &cntx_ptr->cmdFrame );
            break;

        case AVRCP_RESPONSE_NOT_IMPLEMENTED:
        case AVRCP_RESPONSE_ACCEPTED:
        case AVRCP_RESPONSE_REJECTED:
        case AVRCP_RESPONSE_IN_TRANSITION:
        case AVRCP_RESPONSE_IMPLEMENTED_STABLE:
        case AVRCP_RESPONSE_CHANGED:
        case AVRCP_RESPONSE_INTERIM:
			//OS_Report("[AVRCP][UTIL] rspFrame:0x%x cntx.rsp:0x%x cntx.chnl.rsp:0x%x", Parms->p.rspFrame, &(cntx_ptr->rcp_channel.curTx.avrcpRsp), &cntx_ptr->rspFrame );
            break;
    }

    /* AV13 use chnl->curQueueFrame to add it back to freePacketList */
    
    if( NULL != chnl->curQueueFrame ){
        if( chnl->curQueueFrame->data_len == chnl->txOffset  ){
            kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_INSERT_PACKET, &(avrcp_cntx[chnl_num].freePacketList), chnl->curQueueFrame, chnl->curQueueFrame->data);
            Assert( (chnl->curQueueFrame != NULL) );        
            InsertTailList( &(avrcp_cntx[chnl_num].freePacketList) , (ListEntry *)chnl->curQueueFrame);
            chnl->curQueueFrame = NULL;
        }else{
            // [AVRCP][UTIL] has remain data. wait continue packet        
		}
    }
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpUtilCallbackTxError
 * DESCRIPTION
 *  There is a TxError. Handle the Tx error in AVCTP channel.
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  Parms       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
static void AvrcpUtilCallbackTxError(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    avrcp_adp_context *cntx_ptr;
    U8 chnl_num;

    
    chnl_num = AvrcpUtilCallbackFindChannel(chnl);
    AvrcpAssert(chnl_num < BT_AVRCP_MAX_CHANNEL);
    cntx_ptr = &avrcp_cntx[chnl_num];
    
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CB_TX_ERROR, chnl->curQueueFrame, Parms->p.queueFrame );
    
    if( NULL != chnl->curQueueFrame ){
        U8 chnl_num;
        chnl_num = AvrcpUtilCallbackFindChannel(chnl);
		kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_INSERT_PACKET, &(avrcp_cntx[chnl_num].freePacketList), chnl->curQueueFrame, chnl->curQueueFrame->data);
        InsertTailList( &(avrcp_cntx[chnl_num].freePacketList) , (ListEntry *)chnl->curQueueFrame);
        chnl->curQueueFrame = NULL;
    }
}

/*****************************************************************************
 * FUNCTION
 *  AvrcpUtilCallback
 * DESCRIPTION
 *  Handle the event of AVCTP channel from AVCTP module
 *  AvrcpUtil converts the data to ILM structure data and handles the
 *  state machine
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  Parms       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpUtilCallback(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_CALLBACK, Parms->event );
#endif    

    switch (Parms->event)
    {
        case AVRCP_EVENT_CONNECT:
            AvrcpUtilCallbackConnect(chnl, Parms);
            break;

        case AVRCP_EVENT_CONNECT_IND:
            AvrcpUtilCallbackConnectInd(chnl, Parms);
            break;

        case AVRCP_EVENT_DISCONNECT:
            AvrcpUtilCallbackDisconnect(chnl, Parms);
            break;

        case AVRCP_EVENT_COMMAND:
            AvrcpUtilCallbackCmdInd(chnl, Parms);
            break;

        case AVRCP_EVENT_RESPONSE:
        case AVRCP_EVENT_REJECT:
            AvrcpUtilCallbackCmdCnf(chnl, Parms);
            break;

        case AVRCP_EVENT_OPERANDS:
            /* AV13 should no operands in AV1.3 AV1.4 */
            AvrcpUtilCallbackMoreData(chnl, Parms);
            break;

        case AVRCP_EVENT_TX_DONE:
            {
                AvrcpUtilCallbackTxDone(chnl, Parms);
            }
            break;
         case AVRCP_EVENT_TX_ERROR:
            {
                AvrcpUtilCallbackTxError(chnl, Parms);
            }
            break;

    #if AVRCP_PANEL_SUBUNIT == XA_ENABLED
        case AVRCP_EVENT_PANEL_PRESS:
        case AVRCP_EVENT_PANEL_HOLD:
        case AVRCP_EVENT_PANEL_RELEASE:
            /* Do something if profile handle the key events. */
        break;
    #endif /* AVRCP_PANEL_SUBUNIT == XA_ENABLED */ 
    }
}


/*****************************************************************************
 * FUNCTION
 *  AvrcpBrowseUtilCallbackCmdCnf
 * DESCRIPTION
 *  Handle the parsing of response packet in browse channel.
 *  All data should be got from Parms
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  Parms       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
static void AvrcpBrowseUtilCallbackCmdCnf(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    avrcp_adp_context *cntx_ptr;
    U8 chnl_num;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    chnl_num = AvrcpUtilCallbackFindChannel(chnl);
    ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return;
    }
    cntx_ptr = &avrcp_cntx[chnl_num];

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace( BT_TRACE_G8_AVRCP, AVRCP_CALLBACK_EVENT_CMD_RSP, cntx_ptr->rcp_role, cntx_ptr->state, Parms->event );
#endif    

    cntx_ptr->rspFrame.response = Parms->p.rspFrame->response;
    cntx_ptr->rspFrame.subunitType = Parms->p.rspFrame->subunitType;
    cntx_ptr->rspFrame.subunitId = Parms->p.rspFrame->subunitId;

    // Assemble the AVCTP packets 
    chnl->txBrowseBuffer[0] = Parms->p.rspFrame->opcode;
    OS_MemCopy((U8 *)&chnl->txBrowseBuffer[1], Parms->p.rspFrame->operands, Parms->p.rspFrame->operandLen);
    
    cntx_ptr->rspFrame.operandLen = Parms->p.rspFrame->operandLen + 1;  /* Put opcode in the buffer. */

    ASSERT(cntx_ptr->rspFrame.operandLen <= BT_AVRCP_MAX_RSP_FRAME_SIZE);

    if (Parms->p.rspFrame->more)
    {
        cntx_ptr->rsp_more_data = AVRCP_RSP_MORE;
    }
    else    /* has no more data */
    {
		AvrcpChnlInfo myChnlInfo;
        
		cntx_ptr->rsp_more_data = AVRCP_NO_DATA;
		myChnlInfo.chnl_num = chnl_num;
		myChnlInfo.transId = Parms->transId;

		if( cntx_ptr->rspFrame.operandLen > 4 ){
			AvrcpHandleCBAVRCPBrowseRsp_Dispatcher( &myChnlInfo, Parms->p.rspFrame, chnl->txBrowseBuffer[1], (U16)(cntx_ptr->rspFrame.operandLen-4), (U16)(cntx_ptr->rspFrame.operandLen-4), (U8 *)&chnl->txBrowseBuffer[4]);
		}else{
			// AvrcpAssert(0);
			OS_Report("warning unknow request !");
		}

    }

}


/*****************************************************************************
 * FUNCTION
 *  AvrcpBrowseUtilCallback
 * DESCRIPTION
 *  Handle the received packets from browse channel
 *  Invoke CmdInd or Rsp dispatcher to handle the received packets.
 * PARAMETERS
 *  chnl    :  avrcp channel
 *  Parms   :  Callback Parameters      
 * RETURNS
 *  void
 *****************************************************************************/
void AvrcpBrowseUtilCallback(AvrcpChannel *chnl, AvrcpCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	avrcp_adp_context *cntx_ptr;
	U8 chnl_num;
	chnl_num = AvrcpUtilCallbackFindChannel(chnl);
	ASSERT(chnl_num < BT_AVRCP_MAX_CHANNEL);
        if( !(chnl_num < BT_AVRCP_MAX_CHANNEL) ){
            return;
        }
	cntx_ptr = &avrcp_cntx[chnl_num];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_BROWSE_CALLBACK, Parms->event );

    switch( Parms->event ){
        case AVRCP_EVENT_CONNECT_IND: // incoming connection - auto answer it
            AVCTP_ConnectRsp( &chnl->bws_chnl, TRUE);
            
            break;
        case AVRCP_EVENT_CONNECT:
            AvrcpBrowseUtilCallbackConnect(chnl, Parms);
            break;

        case AVRCP_EVENT_DISCONNECT:
            AvrcpBrowseUtilCallbackDisconnect(chnl, Parms);
            break;
        
        case AVRCP_EVENT_COMMAND:
            AvrcpUtilCallbackCmdInd(chnl, Parms);
            break;        
        case AVRCP_EVENT_RESPONSE:
        case AVRCP_EVENT_REJECT:
            AvrcpBrowseUtilCallbackCmdCnf(chnl, Parms);
            break;
        
        case AVRCP_EVENT_TX_DONE:
        case AVRCP_EVENT_TX_ERROR:
        	// retrieve the packet back
			if( NULL != chnl->curBrowseQueueFrame ){
				kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_BWS_INSERT_PACKET, &(avrcp_cntx[chnl_num].freePacketList), chnl->curBrowseQueueFrame, chnl->curBrowseQueueFrame->data);
				InsertTailList( &(avrcp_cntx[chnl_num].freePacketList) , (ListEntry *)chnl->curBrowseQueueFrame);
				chnl->curBrowseQueueFrame = NULL;
			}
			// send the next packet by AVRCP module
        break;
        default:
        break;
    }
}

/*****************************************************************************
 * FUNCTION
 *  AVRTP_UtilGetBrowseMtu
 * DESCRIPTION
 *  Get the max mtu in Browse channel
 * PARAMETERS
 *  chnl_num    :  avrcp channel number
 * RETURNS
 *  void
 *****************************************************************************/
U16 AVRTP_UtilGetBrowseMtu(U32 chnl_num){
    avrcp_adp_context *cntx_ptr = NULL;
    cntx_ptr = &avrcp_cntx[chnl_num];

    return AvrcpGetSinglePacketMtu( &(cntx_ptr->rcp_channel.bws_chnl), AVRCP_CMD_FORMAT_BROWSE);
}

void AvrcpHandleCBAVRCPRspInd_ShowErrorMsg(AvrcpChnlInfo *chnl_info, U8 pdu_id, U32 parm_len){
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PARSE_RSP_FAIL, pdu_id);
}


void AvrcpCancelContinueTimer(AvrcpChannel *chnl){
   	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PACKET_CANCEL_TIMER, chnl->startTimer, chnl->txCmdTimeout);
    if( TRUE == chnl->startTimer && FALSE == chnl->txCmdTimeout){
        EVM_CancelTimer(&chnl->txTimer);
        chnl->startTimer = FALSE;
        chnl->txCmdTimeout = FALSE;
    }
}

void AvrcpStartContinueTimer(AvrcpChannel *chnl){
	kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_PACKET_START_TIMER);
    if( TRUE == chnl->startTimer && FALSE == chnl->txCmdTimeout){
        AvrcpCancelContinueTimer(chnl);
    }

    chnl->txTimer.func = AvrcpContinueTimeout;
    chnl->txTimer.context = &chnl->chnl;
    chnl->txCmdTimeout = FALSE;
    chnl->txTimeout = BT_AVRCP_CONTINUE_WAIT_TIMEOUT; // 1 second
    
    EVM_StartTimer(&chnl->txTimer, chnl->txTimeout);
    chnl->startTimer = TRUE;
    chnl->txCmdTimeout = FALSE;
}

AvrcpChannel *AVRCP_UitlGetChannelByChnlNum(U8 chnl_num)
{
    avrcp_adp_context *cntx_ptr = NULL;

    if( chnl_num >=  BT_AVRCP_MAX_CHANNEL ){
    	  OS_Report("wrong chnl_num:%d", chnl_num);
        return 0;
    }
    
    cntx_ptr = &avrcp_cntx[chnl_num];
    if( 0 != cntx_ptr ){
        return &cntx_ptr->rcp_channel;
    }
    return 0;
}

BtStatus AVRCP_UtilAVRCPSDPQueryInd(AvrcpChannel *chnl, U8 status, U16 tg_version, U16 tg_feature, U16 ct_version, U16 ct_feature){
	U8 chnl_num;
	chnl_num = AvrcpUtilCallbackFindChannel(chnl);    
    bt_avrcp_send_sdp_query_ind(MOD_MMI, chnl_num, 0, status, tg_version, tg_feature, ct_version, ct_feature);
    return 1;
}
#endif
