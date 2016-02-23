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
 * Bt_adp_bm.c
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
#include "bt_common.h"
#include "stdio.h"      /* Basic C I/O functions */
#include "bt_feature.h"
#include "bt.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_struct.h"
#include "eventmgr.h"
#include "btconfig.h"
#include "bttypes.h"
#include "btbm_adp.h"
#include "btbm_export.h"
#include "sdap.h"
#include "bt_jsr82_setting.h"
#if defined (__BT_2_1_BLOCK_DEVICE__)
#include "btbmblocklist.h"
#endif 
#include "bt_maui_btbm.h"
#include "bt_adp_system.h"

bm_adp_context bma_context = {0};

#define MTKBMAC(f)  (bma_context.f)

#ifdef MTK_BTBM_TIMER_CHECK
static void BTBMTimerHandler(EvmTimer *Timer);
void BTBMTimerStart(kal_uint32 timeout_count);
void BTBMTimerCancel(void);
#endif /* MTK_BTBM_TIMER_CHECK */ 

void BTAdpHciSnoopCfg(bt_hcisnoop_config_struct *cfg);

/***************
Internal Globals 
****************/

/***************
External Globals 
****************/

extern void HciTransportError(void);
extern void HciTransportReset(void);
extern btbm_security_level_mode btbmHandleGetSecurityLevel(void);
extern U8 BTBMGapCopyLocalNameWithoutCFM(kal_uint8 *name);
extern void BTBM_Register(BMCallback Callback);
extern void btbmSdpSearchAttributePendingInquiry(U8 enabled);
#if BT_JSR82_ENABLED
extern void SDAP_ServiceSearchAttributeRaw(bt_bm_search_raw_req_struct *search_pattern);
//extern void bt_jsr82_HandleSdpReq(bt_bm_search_raw_req_struct *search_pattern);
#endif 


/*****************************************************************************
 * FUNCTION
 *  bm_adp_init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bm_adp_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_MemSet((U8*) & bma_context, 0, sizeof(bm_adp_context));
    MTKBMAC(src_module) = 0;
    MTKBMAC(direct_rsp_module) = 0;
    MTKBMAC(operation_ongoing) = FALSE;
    MTKBMAC(callback) = 0;
    MTKBMAC(bmaTimer).func = 0;
    MTKBMAC(bt_panic) = FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  bm_adp_deinit
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bm_adp_deinit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    MTKBMAC(bt_panic) = TRUE;
    BTBMTimerCancel();
}


/*****************************************************************************
 * FUNCTION
 *  bma_sendMsg
 * DESCRIPTION
 *  
 * PARAMETERS
 *  msg             [IN]        
 *  dstMod          [IN]        
 *  sap             [IN]        
 *  local_para      [IN]         
 *  peer_buff       [IN]         
 * RETURNS
 *  void
 *****************************************************************************/
void bma_sendMsg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (MTKBMAC(bt_panic) == TRUE)
    {
        return;
    }
    if (dstMod == 0)
    {
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_NOT_READY);
    #endif 
        return;
    }

    if (local_para != NULL)
    {
        BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
    }
    else
    {
        BT_SendMessage(msg, dstMod, NULL, 0);
    }
}


/*****************************************************************************
 * FUNCTION
 *  bm_adp_message_handler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void bm_adp_message_handler(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BOOL message_reject;
    BOOL return_status = TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_prompt_trace(MOD_BT, "bm_adp_message_handler");
    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_BM_READ_LOCAL_ADDR_REQ:
            MTKBMAC(direct_rsp_module) = ilm_ptr->src_mod_id;
            BTBMReadLocalBDAddress();
            break;
        case MSG_ID_BT_BM_READ_LOCAL_COD_REQ:
            MTKBMAC(direct_rsp_module) = ilm_ptr->src_mod_id;
            BTBMReadLocalDeviceClass();
            break;
        case MSG_ID_BT_BM_READ_LOCAL_UUID_REQ:
            MTKBMAC(direct_rsp_module) = ilm_ptr->src_mod_id;
            BTBMReadLocalDeviceUuidList();
            break;			
        case MSG_ID_BT_BM_GET_LINK_STATE_REQ:
            MTKBMAC(direct_rsp_module) = ilm_ptr->src_mod_id;
            BTBMReturnLinkState((bt_bm_get_link_state_req_struct *) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_BM_GET_SCATTERNET_STATE_REQ:
            MTKBMAC(direct_rsp_module) = ilm_ptr->src_mod_id;
            BTBMReturnScatternetState();
            break;
        case MSG_ID_BT_BM_GET_LINK_KEY_REQ:
            MTKBMAC(direct_rsp_module) = ilm_ptr->src_mod_id;
            BTBMGetLinkKey(&((bt_bm_get_link_key_req_struct*)ilm_ptr->local_para_ptr)->bd_addr);
            break;
    #if BT_JSR82_ENABLED
        case MSG_ID_BT_BM_SEARCH_RAW_REQ:
            MTKBMAC(ss_raw_module) = ilm_ptr->src_mod_id;
        #if 0 //autumn   
            bt_jsr82_HandleSdpReq((bt_bm_search_raw_req_struct*) ilm_ptr->local_para_ptr);
        #else
            SDAP_ServiceSearchAttributeRaw((bt_bm_search_raw_req_struct*) ilm_ptr->local_para_ptr);
        #endif    
            break;
    #endif /* BT_JSR82_ENABLED */ 
	    case MSG_ID_BT_HCISNOOP_CONFIG_REQ:
			BTAdpHciSnoopCfg((bt_hcisnoop_config_struct *)ilm_ptr->local_para_ptr);
			break;
        default:
            return_status = FALSE;
            kal_trace(TRACE_GROUP_1, BTLOG_NONDIRECT_RETURN_MESSAGE);
            break;
    }
    MTKBMAC(direct_rsp_module) = 0;
    if (return_status)
    {
        return;
    }
    if (MTKBMAC(operation_ongoing) == TRUE)
    {
        if (MTKBMAC(src_module) == ilm_ptr->src_mod_id)
        {
            message_reject = FALSE;
        }
        else
        {
            message_reject = TRUE;
        }
    }
    else
    {
        message_reject = FALSE;
        MTKBMAC(src_module) = ilm_ptr->src_mod_id;
    }
    if (message_reject == TRUE)
    {
        kal_trace(TRACE_GROUP_1,BT_BTBM_ENTER_REJECT_HANDLER);
        bma_reject_handler(ilm_ptr);
    }
    else
    {
        bt_prompt_trace(MOD_BT, "message_reject == FALSE");
    #ifdef MTK_BTBM_TIMER_CHECK
        BTBMTimerStart(70000);
    #endif 
        MTKBMAC(operation_ongoing) = TRUE;
        MTKBMAC(last_cmd) = ilm_ptr->msg_id;

        switch (ilm_ptr->msg_id)
        {
            case MSG_ID_BT_BM_DISCOVERY_REQ:
            #ifdef MTK_BTBM_TIMER_CHECK
                BTBMTimerStart(900000);
            #endif                 
                BTBMGapInquiry((bt_bm_discovery_req_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_DISCOVERY_CANCEL_REQ:
                BTBMGapInquiryCancel();
                break;
            case MSG_ID_BT_BM_WRITE_LOCAL_NAME_REQ:
                BTBMGapWriteLocalName((bt_bm_write_local_name_req_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_READ_LOCAL_NAME_REQ:
                BTBMGapReadLocalName();
                break;
            case MSG_ID_BT_BM_READ_REMOTE_NAME_REQ:
                BTBMGapRemoteNameReq((bt_bm_read_remote_name_req_struct*) ilm_ptr->local_para_ptr);
                BTBMTimerCancel();
                break;
            case MSG_ID_BT_BM_READ_REMOTE_NAME_CANCEL_REQ:
                BTBMGapRemoteNameCancelReq();
                break;
            case MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ:
                BTBMGapWriteScanEnabled((bt_bm_write_scanenable_mode_req_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_READ_SCANENABLE_MODE_REQ:
                BTBMGapReadScanEnabled();
                break;
            case MSG_ID_BT_BM_WRITE_LOCAL_COD_REQ:
                BTBMWriteLocalDeviceClass((bt_bm_write_local_cod_req_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_READ_PROPERTY_REQ:
                Assert(0);
                break;
            case MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_REQ:
                BTBMGapSetAuthenticationMode(
                    TRUE,
                    (bt_bm_write_authentication_mode_req_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_PIN_CODE_RSP:
                BTBMTimerCancel();
                BTBMTimerStart(200000);
                BTBMGapWritePINCode((bt_bm_pin_code_rsp_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_BONDING_REQ:
                BTBMTimerCancel();
                BTBMTimerStart(200000);
                BTBMGapBondingDevice((bt_bm_bonding_req_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_BONDING_CANCEL_REQ:
                BTBMGapBondingCancel((bt_bm_bonding_cancel_req_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_PAIRING_CANCEL_REQ:
            case MSG_ID_BT_BM_SAVE_SETTING_REQ:
            case MSG_ID_BT_BM_RESTORE_SETTING_REQ:
                Assert(0);
                break;
            case MSG_ID_BT_BM_SEARCH_ATTRIBUTE_REQ:
                BTBMTimerCancel();
                BTBMTimerStart(200000);
                BTBMSdapSearchAttributeService((bt_bm_search_attribute_req_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_SERVICE_SEARCH_REQ:
                BTBMTimerCancel();
                BTBMTimerStart(200000);
                BTBMSdapSearchallService((bt_bm_service_search_req_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_SERVICE_SEARCH_CANCEL_REQ:
                BTBMSdapSearchAllCancel((bt_bm_service_search_cancel_req_struct *) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_DELETE_TRUST_REQ:
                BTBMGapDeleteTrustDevice((bt_bm_delete_trust_req_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_DELETE_TRUST_ALL_REQ:
                BTBMGapDeleteTrustAllDevice();
                break;

            case MSG_ID_BT_BM_LINK_ALLOW_REQ:
                BTBMAllowAllLink();
                break;
            case MSG_ID_BT_BM_LINK_DISALLOW_REQ:
                BTBMDisconnectAllLink();
                break;
            case MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_RSP:
                BTBMTimerCancel();
                BTBMACLLinkAcceptRsp((bt_bm_link_connect_accept_rsp_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_NOT_AUTO_REQ:
                BTBMTimerCancel();
                BTBMACLLinkAcceptMode(BM_LINK_ACCEPT_CMD_NONE);
                break;
        #if defined (__BT_2_1_SIMPLE_PAIRING__)
            case MSG_ID_BT_BM_SECURITY_USER_CONFIRM_RSP:
                BTBMTimerCancel();
                BTBMTimerStart(200000);
                BTBMGapWriteUserConfirm((bt_bm_security_user_confirm_rsp_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_SECURITY_PASSKEY_ENTRY_RSP:
                BTBMTimerCancel();
                BTBMGapPasskeyEntryRsp((bt_bm_security_passkey_entry_rsp_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_SECURITY_PASSKEY_NOTIFY_RSP:
                BTBMTimerCancel();
                BTBMGapPasskeyNotifyRsp((bt_bm_security_passkey_notify_rsp_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_RSP:
                BTBMTimerCancel();
                BTBMSSPKeyPressRsp();
                break;
            case MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_CANCEL_REQ:
                BTBMTimerCancel();
                BTBMSSPKeyPressAbort((bt_bm_security_passkey_notify_cancel_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_SECURITY_OOB_DATA_RSP:
                BTBMTimerCancel();
                BTBMGapOOBDataRsp((bt_bm_security_oob_data_rsp_struct*) ilm_ptr->local_para_ptr);
                break;
        #endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 
        #if defined (__BT_2_1_BLOCK_DEVICE__)
            case MSG_ID_BT_BM_BLOCK_ACTIVE_LINK_REQ:
                BTBMTimerCancel();
                BTBMBlockDeviceForceDisconnect((bt_bm_block_active_link_req_struct*) ilm_ptr->local_para_ptr);
                break;
            case MSG_ID_BT_BM_BLOCK_LIST_UPDATE_REQ:
                BTBMTimerCancel();
                BTBMHandleUpdateBlockList((bt_bm_block_list_update_req_struct*) ilm_ptr->local_para_ptr);
                break;
        #else /* defined (__BT_2_1_BLOCK_DEVICE__) */ 
            case MSG_ID_BT_BM_BLOCK_ACTIVE_LINK_REQ:
                BTBMTimerCancel();
                BTBMAdpBlockDeviceDisconnectLinkCfm();
                break;

            case MSG_ID_BT_BM_BLOCK_LIST_UPDATE_REQ:
                BTBMTimerCancel();
                BTBMAdpBlockListUpdateCfm();
                break;
        #endif /* defined (__BT_2_1_BLOCK_DEVICE__) */ 
            default:
                kal_trace(TRACE_GROUP_1,BT_BTBM_ERROR_MESSAGE);
                Assert(0);
                break;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  bma_reject_handler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void bma_reject_handler(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    module_type src_module;
    U8 result = BTBM_ADP_FAIL_BUSY;
    msg_type message_type;
    local_para_struct *msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    src_module = ilm_ptr->src_mod_id;
    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_BM_DISCOVERY_REQ:
        {
            bt_bm_discovery_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_discovery_cnf_struct*) construct_local_para(sizeof(bt_bm_discovery_cnf_struct), TD_UL);
            btbm_adp_msg->result = BTBM_ADP_FAIL_BUSY;
            btbm_adp_msg->total_number = 0;
            message_type = MSG_ID_BT_BM_DISCOVERY_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_DISCOVERY_CANCEL_REQ:
        {
            bt_bm_discovery_cancel_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_discovery_cancel_cnf_struct*) construct_local_para(
                                                        sizeof(bt_bm_discovery_cancel_cnf_struct),
                                                        TD_UL);

            btbm_adp_msg->result = BTBM_ADP_FAIL_BUSY;
            btbm_adp_msg->total_number = 0;
            message_type = MSG_ID_BT_BM_DISCOVERY_CANCEL_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_WRITE_LOCAL_NAME_REQ:
        {
            bt_bm_write_local_name_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_write_local_name_cnf_struct*) construct_local_para(
                                                        sizeof(bt_bm_write_local_name_cnf_struct),
                                                        TD_UL);

            btbm_adp_msg->result = result;
            message_type = MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_READ_REMOTE_NAME_REQ:
        {
            bt_bm_read_remote_name_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_read_remote_name_cnf_struct*) construct_local_para(
                                                        sizeof(bt_bm_read_remote_name_cnf_struct),
                                                        TD_UL);
            btbm_adp_msg->bd_addr = ((bt_bm_read_remote_name_req_struct*) ilm_ptr->local_para_ptr)->bd_addr;
            btbm_adp_msg->result = BTBM_ADP_FAIL_BUSY;
            btbm_adp_msg->name_len = 0;
            btbm_adp_msg->cod = 0;
            message_type = MSG_ID_BT_BM_READ_REMOTE_NAME_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_READ_REMOTE_NAME_CANCEL_REQ:
        {
            bt_bm_read_remote_name_cancel_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_read_remote_name_cancel_cnf_struct*)
                construct_local_para(sizeof(bt_bm_read_remote_name_cancel_cnf_struct), TD_UL);
            
            btbm_adp_msg->result = BTBM_ADP_FAIL_BUSY;
            message_type = MSG_ID_BT_BM_READ_REMOTE_NAME_CANCEL_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ:
        {
            bt_bm_write_scanenable_mode_cnf_struct *btbm_adp_msg;
            BtStatus status = BT_STATUS_FAILED;
            U8 bm_save_mode = 0;

            /*
             * 2008-0326 Note: Do Not invoke BTBMGapWriteScanEnabled() directly
             * * If MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ is rejected due to on-going Inquiry procedure,
             * * In BTBMGapWriteScanEnabled(): the MTKBMC(bm_scan_mode) is BM_SCAN_MODE_SET_UNPAGEABLE, 
             * * so MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF will be reported and operation_ongoing flag is reset as FALSE
             */
            status = BTBMGapSaveWriteScanModeValue((bt_bm_write_scanenable_mode_req_struct*) ilm_ptr->local_para_ptr);
            kal_trace(TRACE_GROUP_1, BT_LOG_BM_REJECT_WRITE_SCAN_MODE_SAVE_MODE_AND_STATUS, bm_save_mode, status);

            btbm_adp_msg =
                (bt_bm_write_scanenable_mode_cnf_struct*)
                construct_local_para(sizeof(bt_bm_write_scanenable_mode_cnf_struct), TD_UL);
            btbm_adp_msg->result = BTBM_ADP_FAIL_BUSY;
            message_type = MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_READ_SCANENABLE_MODE_REQ:
        {
            bt_bm_read_scanenable_mode_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_read_scanenable_mode_cnf_struct*)
                construct_local_para(sizeof(bt_bm_read_scanenable_mode_cnf_struct), TD_UL);
            btbm_adp_msg->result = BTBM_ADP_FAIL_BUSY;
            btbm_adp_msg->modeconnected = 0x00;
            btbm_adp_msg->modenotconnected = 0x00;
            message_type = MSG_ID_BT_BM_READ_SCANENABLE_MODE_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_READ_LOCAL_ADDR_REQ:
        {
            bt_bm_read_local_addr_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_read_local_addr_cnf_struct*) construct_local_para(
                                                        sizeof(bt_bm_read_local_addr_cnf_struct),
                                                        TD_UL);

            btbm_adp_msg->result = result;
            btbm_adp_msg->bd_addr.lap = 0x00;
            btbm_adp_msg->bd_addr.uap = 0x00;
            btbm_adp_msg->bd_addr.nap = 0x00;
            message_type = MSG_ID_BT_BM_READ_LOCAL_ADDR_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_READ_LOCAL_COD_REQ:
        {
            bt_bm_read_local_cod_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_read_local_cod_cnf_struct*) construct_local_para(
                                                    sizeof(bt_bm_read_local_cod_cnf_struct),
                                                    TD_UL);
            btbm_adp_msg->result = result;
            btbm_adp_msg->cod = 0x00;
            message_type = MSG_ID_BT_BM_READ_LOCAL_COD_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_READ_PROPERTY_REQ:
        {
            bt_bm_read_property_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_read_property_cnf_struct*) construct_local_para(sizeof(bt_bm_read_property_cnf_struct), TD_UL);

            btbm_adp_msg->spec_version = 0x00;
            btbm_adp_msg->master_switch = 0x00;
            btbm_adp_msg->retrievable_max = 0x00;
            btbm_adp_msg->connected_max = 0x00;
            btbm_adp_msg->received_l2cap_mtu = 0x00;
            btbm_adp_msg->connected_page = 0x00;
            btbm_adp_msg->connected_inquiry = 0x00;
            btbm_adp_msg->sdp_transaction_max = 0x00;
            btbm_adp_msg->connected_page_scan = 0x00;
            btbm_adp_msg->connected_inquiry_scan = 0x00;
            message_type = MSG_ID_BT_BM_READ_PROPERTY_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_REQ:
        {
            bt_bm_write_authentication_mode_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_write_authentication_mode_cnf_struct*)
                construct_local_para(sizeof(bt_bm_write_authentication_mode_cnf_struct), TD_UL);

            btbm_adp_msg->result = result;
            message_type = MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_PIN_CODE_RSP:
            return;
        case MSG_ID_BT_BM_BONDING_REQ:
        {
            bt_bm_bonding_cnf_struct *btbm_adp_msg;

            btbm_adp_msg = (bt_bm_bonding_cnf_struct*) construct_local_para(sizeof(bt_bm_bonding_cnf_struct), TD_UL);
            btbm_adp_msg->result = result;
            btbm_adp_msg->bd_addr = ((bt_bm_bonding_req_struct*) ilm_ptr->local_para_ptr)->bd_addr;
            message_type = MSG_ID_BT_BM_BONDING_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_SAVE_SETTING_REQ:
        {
            bt_bm_save_setting_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_save_setting_cnf_struct*) construct_local_para(sizeof(bt_bm_save_setting_cnf_struct), TD_UL);
            btbm_adp_msg->result = result;
            message_type = MSG_ID_BT_BM_SAVE_SETTING_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_RESTORE_SETTING_REQ:
        {
            bt_bm_restore_setting_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_restore_setting_cnf_struct*) construct_local_para(
                                                        sizeof(bt_bm_restore_setting_cnf_struct),
                                                        TD_UL);

            btbm_adp_msg->result = result;
            message_type = MSG_ID_BT_BM_RESTORE_SETTING_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_SEARCH_ATTRIBUTE_REQ:
        {
            bt_bm_service_search_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_service_search_cnf_struct*) construct_local_para(
                                                    sizeof(bt_bm_service_search_cnf_struct),
                                                    TD_UL);
            btbm_adp_msg->result = result;
            btbm_adp_msg->bd_addr = ((bt_bm_service_search_req_struct*) ilm_ptr->local_para_ptr)->bd_addr;
            btbm_adp_msg->searched_number = 0x00;
            message_type = MSG_ID_BT_BM_SERVICE_SEARCH_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_SERVICE_SEARCH_REQ:
        {
            bt_bm_search_attribute_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_search_attribute_cnf_struct*) construct_local_para(
                                                        sizeof(bt_bm_search_attribute_cnf_struct),
                                                        TD_UL);
            btbm_adp_msg->result = result;
            btbm_adp_msg->bd_addr = ((bt_bm_search_attribute_req_struct*) ilm_ptr->local_para_ptr)->bd_addr;
            message_type = MSG_ID_BT_BM_SEARCH_ATTRIBUTE_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        case MSG_ID_BT_BM_DELETE_TRUST_REQ:
            BTBMGapDeleteTrustDevice((bt_bm_delete_trust_req_struct*) ilm_ptr->local_para_ptr);
            {
                bt_bm_delete_trust_cnf_struct *btbm_adp_msg;

                btbm_adp_msg =
                    (bt_bm_delete_trust_cnf_struct*) construct_local_para(
                                                        sizeof(bt_bm_delete_trust_cnf_struct),
                                                        TD_UL);

                btbm_adp_msg->result = result;
                btbm_adp_msg->bd_addr = ((bt_bm_delete_trust_req_struct*) ilm_ptr->local_para_ptr)->bd_addr;
                message_type = MSG_ID_BT_BM_DELETE_TRUST_CNF;
                msg = (local_para_struct*) btbm_adp_msg;
            }
            break;
        case MSG_ID_BT_BM_WRITE_LOCAL_COD_REQ:
        {
            bt_bm_write_local_cod_cnf_struct *btbm_adp_msg;

            btbm_adp_msg =
                (bt_bm_write_local_cod_cnf_struct*) construct_local_para(
                                                        sizeof(bt_bm_write_local_cod_cnf_struct),
                                                        TD_UL);
            btbm_adp_msg->result = result;
            message_type = MSG_ID_BT_BM_WRITE_LOCAL_COD_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;

        case MSG_ID_BT_BM_READ_LOCAL_NAME_REQ:
        {
            bt_bm_read_local_name_cnf_struct *btbm_adp_msg;
            kal_uint8 temp_length = 0;

            btbm_adp_msg =
                (bt_bm_read_local_name_cnf_struct*) construct_local_para(
                                                        sizeof(bt_bm_read_local_name_cnf_struct),
                                                        TD_UL);

            temp_length = BTBMGapCopyLocalNameWithoutCFM(btbm_adp_msg->name);
            btbm_adp_msg->name_len = temp_length;

            message_type = MSG_ID_BT_BM_READ_LOCAL_NAME_CNF;
            msg = (local_para_struct*) btbm_adp_msg;
        }
            break;
        default:
            return;
    }

    bma_sendMsg(message_type, src_module, BT_APP_SAP, (local_para_struct*) msg, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpGetReadVisibleModeCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  mode                    [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpGetReadVisibleModeCfm(kal_uint8 mode)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_read_scanenable_mode_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_read_scanenable_mode_cnf_struct*) construct_local_para(
                                                    sizeof(bt_bm_read_scanenable_mode_cnf_struct),
                                                    TD_UL);
    btbm_adp_msg->result = BTBM_ADP_SUCCESS;
    btbm_adp_msg->modeconnected = mode;
    btbm_adp_msg->modenotconnected = mode;
    MTKBMAC(operation_ongoing) = FALSE;

    bma_sendMsg(
        MSG_ID_BT_BM_READ_SCANENABLE_MODE_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpReadLocalDeviceClassCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  cod         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpReadLocalDeviceClassCfm(kal_uint8 result, kal_uint32 cod)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_read_local_cod_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_read_local_cod_cnf_struct*) construct_local_para(sizeof(bt_bm_read_local_cod_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    btbm_adp_msg->cod = cod;
    bma_sendMsg(
        MSG_ID_BT_BM_READ_LOCAL_COD_CNF,
        MTKBMAC(direct_rsp_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
}

/*****************************************************************************
 * FUNCTION
 *  BTBMAdpReadLocalDeviceUuidListCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  uuid_list         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpReadLocalDeviceUuidListCfm(kal_uint8 result, kal_uint32 *uuid_list)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_read_local_uuid_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_read_local_uuid_cnf_struct*) construct_local_para(sizeof(bt_bm_read_local_uuid_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    btbm_adp_msg->service_list1 = uuid_list[0];
    btbm_adp_msg->service_list2 = uuid_list[1];
    btbm_adp_msg->service_list3 = uuid_list[2];
    btbm_adp_msg->service_list4 = uuid_list[3];
    btbm_adp_msg->service_list5 = uuid_list[4];
    btbm_adp_msg->service_list6 = uuid_list[5];

    bma_sendMsg(
        MSG_ID_BT_BM_READ_LOCAL_UUID_CNF,
        MTKBMAC(direct_rsp_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
}

/*****************************************************************************
 * FUNCTION
 *  BTBMAdpWriteLocalDeviceClassCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpWriteLocalDeviceClassCfm(kal_uint8 result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_write_local_cod_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_write_local_cod_cnf_struct*) construct_local_para(sizeof(bt_bm_write_local_cod_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_WRITE_LOCAL_COD_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpReadLocalNameCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  name        [IN]       
 *  length      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpReadLocalNameCfm(kal_uint8 *name, kal_uint8 length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_read_local_name_cnf_struct *btbm_adp_msg;
    kal_uint8 temp_length = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_read_local_name_cnf_struct*) construct_local_para(sizeof(bt_bm_read_local_name_cnf_struct), TD_UL);
    if (length > BTBM_ADP_MAX_NAME_LEN)
    {
        temp_length = BTBM_ADP_MAX_NAME_LEN;
    }
    else
    {
        temp_length = length;
    }
    btbm_adp_msg->name_len = temp_length;
    OS_MemSet(btbm_adp_msg->name, 0, sizeof(btbm_adp_msg->name));
    OS_MemCopy(btbm_adp_msg->name, name, temp_length);
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_READ_LOCAL_NAME_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpReadLocalBDAddressCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  bd_addr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpReadLocalBDAddressCfm(kal_uint8 result, btbm_bd_addr_t bd_addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_read_local_addr_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_read_local_addr_cnf_struct*) construct_local_para(sizeof(bt_bm_read_local_addr_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    btbm_adp_msg->bd_addr.lap = bd_addr.lap;
    btbm_adp_msg->bd_addr.uap = bd_addr.uap;
    btbm_adp_msg->bd_addr.nap = bd_addr.nap;
    bma_sendMsg(
        MSG_ID_BT_BM_READ_LOCAL_ADDR_CNF,
        MTKBMAC(direct_rsp_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpWriteScanEnabledCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpWriteScanEnabledCfm(kal_uint8 result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_write_scanenable_mode_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_write_scanenable_mode_cnf_struct*) construct_local_para(
                                                    sizeof(bt_bm_write_scanenable_mode_cnf_struct),
                                                    TD_UL);

    btbm_adp_msg->result = result;
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpGapDiscoveryCompCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result              [IN]        
 *  total_number        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpGapDiscoveryCompCfm(kal_uint8 result, kal_uint8 total_number)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_discovery_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg = (bt_bm_discovery_cnf_struct*) construct_local_para(sizeof(bt_bm_discovery_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    btbm_adp_msg->total_number = total_number;
    kal_trace(TRACE_GROUP_1,BT_BTBM_DISCOVERY_COMPLETE, total_number);

    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(MSG_ID_BT_BM_DISCOVERY_CNF, MTKBMAC(src_module), BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);

#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMGapDiscoveryResultCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bd_addr                 [IN]        
 *  rssi                    [IN]        
 *  eir_supported           [IN]        
 *  uuid_list_supported     [IN]        
 *  servlist_completed      [IN]        
 *  uuid_list               [IN]        
 *  cod                     [IN]        
 *  name_len                [IN]        
 *  name                    [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMGapDiscoveryResultCfm(BOOL is_update,
                               btbm_bd_addr_t bd_addr,
#ifdef __BT_4_0_BLE__
                               btbm_device_type device_type,
#endif
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
                                  kal_uint8 rssi, kal_uint8 eir_supported, 
                               kal_uint8 uuid_list_supported, kal_uint8 servlist_completed, kal_uint32 *uuid_list,
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
                                  kal_uint32 cod, kal_uint8 name_len, kal_uint8 *name)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_discovery_result_ind_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_discovery_result_ind_struct*) construct_local_para(
                                                sizeof(bt_bm_discovery_result_ind_struct),
                                                TD_UL | TD_RESET);

    btbm_adp_msg->bd_addr.lap = bd_addr.lap;
    btbm_adp_msg->bd_addr.uap = bd_addr.uap;
    btbm_adp_msg->bd_addr.nap = bd_addr.nap;
#ifdef __BT_4_0_BLE__
    btbm_adp_msg->device_type = device_type;
#else
    btbm_adp_msg->device_type = BTBM_DEVICE_TYPE_BR_EDR;
#endif
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
    btbm_adp_msg->rssi = (kal_int8)rssi;
    btbm_adp_msg->supported_eir = eir_supported;
    if (uuid_list_supported == 1)
    {
        btbm_adp_msg->supported_servlist = KAL_TRUE;
        if (uuid_list != NULL)
        {
            btbm_adp_msg->service_list1 = uuid_list[0];
            btbm_adp_msg->service_list2 = uuid_list[1];
            btbm_adp_msg->service_list3 = uuid_list[2];
            btbm_adp_msg->service_list4 = uuid_list[3];
            btbm_adp_msg->service_list5 = uuid_list[4];
            btbm_adp_msg->service_list6 = uuid_list[5];
        }
    }
    else
    {
        btbm_adp_msg->supported_servlist = KAL_FALSE;
    }
    if (servlist_completed == 1)
    {
        btbm_adp_msg->servlist_completed = KAL_TRUE;
    }
    else
    {
        btbm_adp_msg->servlist_completed = KAL_FALSE;
    }
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
    btbm_adp_msg->cod = cod;
    btbm_adp_msg->name_len = name_len;
    OS_MemSet(btbm_adp_msg->name, 0, sizeof(btbm_adp_msg->name));
    if (name_len != 0)
    {
        OS_MemCopy(btbm_adp_msg->name, name, name_len);
    }
    kal_trace(TRACE_GROUP_1,BT_BTBM_DISCOVERY_BDADDR, bd_addr.nap, bd_addr.uap, bd_addr.lap);
    kal_trace(TRACE_GROUP_1,BT_BTBM_DISCOVERY_COD, cod);
    kal_trace(TRACE_GROUP_1,BT_BTBM_DISCOVERY_NAME_LEN, name_len);
    Report(("discovery with name:%s", btbm_adp_msg->name));
    if (is_update)
    {
        bma_sendMsg(
            MSG_ID_BT_BM_DISCOVERY_UPDATE_IND,
            MTKBMAC(src_module),
            BT_APP_SAP,
            (local_para_struct*) btbm_adp_msg,
            NULL);
    }
    else
    {
        bma_sendMsg(
            MSG_ID_BT_BM_DISCOVERY_RESULT_IND,
            MTKBMAC(src_module),
            BT_APP_SAP,
            (local_para_struct*) btbm_adp_msg,
            NULL);
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpGapDiscoveryCancelCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result              [IN]        
 *  total_number        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpGapDiscoveryCancelCfm(kal_uint8 result, kal_uint8 total_number)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_discovery_cancel_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_discovery_cancel_cnf_struct*) construct_local_para(sizeof(bt_bm_discovery_cancel_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    btbm_adp_msg->total_number = total_number;
    kal_trace(TRACE_GROUP_1,BT_BTBM_DISCOVERY_CANCEL, total_number);
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_DISCOVERY_CANCEL_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);

#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpWriteLocalNameCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpWriteLocalNameCfm(kal_uint8 result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_write_local_name_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_write_local_name_cnf_struct*) construct_local_para(sizeof(bt_bm_write_local_name_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpGapSetAuthenticationCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpGapSetAuthenticationCfm(kal_uint8 result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_write_authentication_mode_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_write_authentication_mode_cnf_struct*)
        construct_local_para(sizeof(bt_bm_write_authentication_mode_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    btbm_adp_msg->security_level = btbmHandleGetSecurityLevel();
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpDeleteTrustCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  bd_addr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpDeleteTrustCfm(kal_uint8 result, btbm_bd_addr_t bd_addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_delete_trust_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg = (bt_bm_delete_trust_cnf_struct*) construct_local_para(sizeof(bt_bm_delete_trust_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    btbm_adp_msg->bd_addr = bd_addr;
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_DELETE_TRUST_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpDeleteTrustAllCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpDeleteTrustAllCfm(kal_uint8 result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_delete_trust_all_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_delete_trust_all_cnf_struct*) construct_local_para(sizeof(bt_bm_delete_trust_all_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_DELETE_TRUST_ALL_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpAllowAclLinkResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpAllowAclLinkResult(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bma_sendMsg(MSG_ID_BT_BM_LINK_ALLOW_CNF, MTKBMAC(src_module), BT_APP_SAP, NULL, NULL);
    MTKBMAC(operation_ongoing) = FALSE;
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpDisallowAclLinkResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpDisallowAclLinkResult(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace(TRACE_GROUP_1,BT_BTBM_POWEROFF);
    bma_sendMsg(MSG_ID_BT_BM_LINK_DISALLOW_CNF, MTKBMAC(src_module), BT_APP_SAP, NULL, NULL);
    MTKBMAC(operation_ongoing) = FALSE;

#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpBlockDeviceDisconnectLinkCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpBlockDeviceDisconnectLinkCfm(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(MSG_ID_BT_BM_BLOCK_ACTIVE_LINK_CNF, MOD_MMI, BT_APP_SAP, NULL, NULL);

}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpBlockListUpdateCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpBlockListUpdateCfm(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(MSG_ID_BT_BM_BLOCK_LIST_UPDATE_CNF, MTKBMAC(src_module), BT_APP_SAP, NULL, NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 

}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpBondingDeviceCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  bd_addr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpBondingDeviceCfm(kal_uint8 result, btbm_bd_addr_t bd_addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_bonding_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg = (bt_bm_bonding_cnf_struct*) construct_local_para(sizeof(bt_bm_bonding_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    btbm_adp_msg->bd_addr = bd_addr;
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(MSG_ID_BT_BM_BONDING_CNF, MTKBMAC(src_module), BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);

#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpBondingCancelCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  bd_addr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpBondingCancelCfm(kal_uint8 result, btbm_bd_addr_t bd_addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_bonding_cancel_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_bonding_cancel_cnf_struct*) construct_local_para(sizeof(bt_bm_bonding_cancel_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    btbm_adp_msg->bd_addr = bd_addr;
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_BONDING_CANCEL_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);

#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpLinkKeyGeneratedInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  type        [IN]        
 *  bd_addr     [IN]        
 *  cod         [IN]        
 *  key_type        [IN]        
 *  key             [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpLinkKeyGeneratedInd(
        kal_uint8 result, 
        kal_uint8 type, 
        btbm_bd_addr_t bd_addr, 
#ifdef __BT_4_0_BLE__
        btbm_device_type device_type,
#endif
        kal_uint32 cod, 
        kal_uint8 key_type,
        kal_uint8 *key)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_bonding_result_ind_struct *btbm_adp_msg;
    module_type dstMod = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_bonding_result_ind_struct*) construct_local_para(sizeof(bt_bm_bonding_result_ind_struct), TD_RESET);

    btbm_adp_msg->result = result;
    btbm_adp_msg->bd_addr = bd_addr;
    btbm_adp_msg->cod = cod;
    btbm_adp_msg->link_key_type = key_type;
    if (key != NULL)
    {
        OS_MemCopy(btbm_adp_msg->link_key, key, BTBM_ADP_MAX_LINK_KEY_LEN);
    }
#ifdef __BT_4_0_BLE__
    btbm_adp_msg->device_type = device_type;
#else
    btbm_adp_msg->device_type = BTBM_DEVICE_TYPE_BR_EDR;
#endif
    if (type == BTBM_TYPE_PAIRING)
    {
        MTKBMAC(operation_ongoing) = FALSE;
    }

    /*
     * PIN_CODE_IND should be processed by MOD_MMI
     * * Only one exception: the gap tester mode is initialized. In this case, the MTKBMAC(callback)!=0
     * * In normal case, why not use MTKBMAC(src_module) directly?
     * * When jsr82 is enabled, java asks for inquiry and changes MTKBMAC(src_module) to MOD_JASYNC,
     * * then java asks for connecting request, during this procedure, pin code exchange is involved, but PIN_CODE_IND will sent to MOD_JASYNC 
     * * Add 2007-1222
     */
    if (MTKBMAC(callback) == 0)
    {
        dstMod = MOD_MMI;
    }
    else
    {
        dstMod = MTKBMAC(src_module);   /* gap tester mode, and MTKBMAC(src_module) is set to MOD_BT */
    }

    bma_sendMsg(
        MSG_ID_BT_BM_BONDING_RESULT_IND,
        /* MTKBMAC(src_module), */ 
        dstMod,
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);

#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpReportLinkState
 * DESCRIPTION
 *  
 * PARAMETERS
 *  active_no       [IN]        
 *  bd_addr         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpReportLinkState(kal_uint8 active_no, btbm_bd_addr_t bd_addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_link_state_ind_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg = (bt_bm_link_state_ind_struct*) construct_local_para(sizeof(bt_bm_link_state_ind_struct), TD_UL);
    btbm_adp_msg->max_number = MAX_CHIP_ACCEPTABLE_NUMBER;
    btbm_adp_msg->current_number = active_no;
    btbm_adp_msg->bd_addr.lap = bd_addr.lap;
    btbm_adp_msg->bd_addr.uap = bd_addr.uap;
    btbm_adp_msg->bd_addr.nap = bd_addr.nap;
    kal_trace(
        TRACE_GROUP_1,
        BT_LOG_BTBM_REPORT_LINK_STATE_DEBUG_INFO,
        MTKBMAC(operation_ongoing),
        MTKBMAC(src_module),
        MTKBMAC(last_cmd));

    if (MTKBMAC(src_module) == MOD_MMI)
    {
        /*
         * In this case: ACCEPT_CONNECT_IND to MMI, then MMI sends ACCEPT_RSP (src_module is MMI, operation_ongoing is TRUE),
         * * Finally: get LINK_CONNECT_IND event and reset MTKBMAC(operation_ongoing) as FALSE
         */
        MTKBMAC(operation_ongoing) = FALSE;
    }
    else
    {
        /*
         * In this case: MTKBMAC(src_module) is MOD_JASYNC instead of MOD_MMI: 
         * * If MSG_ID_BT_BM_LINK_STATE_IND reported to MOD_MMI is triggered due to ACL DISCONNECT event of A2DP
         * * Next time, MOD_MMI will send MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ to BM
         * * But, if operation (e.g., inquiry) triggered by JASYNC is still ongoing, BM must reject MMI's WRITE_SCANENABLE_MODE_REQ
         */
        kal_trace(TRACE_GROUP_1, BT_LOG_BTBM_CAN_NOT_RESET_OP_ONGOING_FLAG);
    }
    bma_sendMsg(MSG_ID_BT_BM_LINK_STATE_IND, MOD_MMI, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);

}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpPinCodeRequiredInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  pin_code_16_digit_required        [IN]        
 *  bd_addr                         [IN]        
 *  name_len                        [IN]        
 *  name                            [IN]        
 *  cod                             [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpPinCodeRequiredInd(
        kal_uint8 pin_code_16_digit_required,
        btbm_bd_addr_t bd_addr,
        kal_uint8 name_len,
        kal_uint8 *name,
        kal_uint32 cod)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_pin_code_ind_struct *btbm_adp_msg;
    module_type dstMod = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg = (bt_bm_pin_code_ind_struct*) construct_local_para(sizeof(bt_bm_pin_code_ind_struct), TD_UL);

    btbm_adp_msg->enable_16digits_pin_code = pin_code_16_digit_required;
    btbm_adp_msg->bd_addr.lap = bd_addr.lap;
    btbm_adp_msg->bd_addr.uap = bd_addr.uap;
    btbm_adp_msg->bd_addr.nap = bd_addr.nap;
    btbm_adp_msg->name_len = name_len;
    btbm_adp_msg->cod = cod;
    OS_MemSet(btbm_adp_msg->name, 0, sizeof(btbm_adp_msg->name));
    if (name_len != 0)
    {
        OS_MemCopy(btbm_adp_msg->name, name, name_len);
    }
    MTKBMAC(operation_ongoing) = FALSE;

    /*
     * PIN_CODE_IND should be processed by MOD_MMI
     * * Only one exception: the gap tester mode is initialized. In this case, the MTKBMAC(callback)!=0
     * * In normal case, why not use MTKBMAC(src_module) directly?
     * * When jsr82 is enabled, java asks for inquiry and changes MTKBMAC(src_module) to MOD_JASYNC,
     * * then java asks for connecting request, during this procedure, pin code exchange is involved, but PIN_CODE_IND will sent to MOD_JASYNC 
     * * Add 2007-1222
     */
    if (MTKBMAC(callback) == 0)
    {
        dstMod = MOD_MMI;
    }
    else
    {
        dstMod = MOD_BT;    /* gap tester mode, and MTKBMAC(src_module) is set to MOD_BT */
    }

    bma_sendMsg(
        MSG_ID_BT_BM_PIN_CODE_IND,
        /* MTKBMAC(src_module), */ 
        dstMod,
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpReadRemoteNameCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result          [IN]        
 *  bd_addr         [IN]        
 *  name_len        [IN]        
 *  name            [IN]         
 *  cod             [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpReadRemoteNameCfm(
        kal_uint8 result,
        btbm_bd_addr_t bd_addr,
        kal_uint8 name_len,
        kal_uint8 *name,
        kal_uint32 cod)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_read_remote_name_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_read_remote_name_cnf_struct*) construct_local_para(sizeof(bt_bm_read_remote_name_cnf_struct), TD_UL);

    btbm_adp_msg->bd_addr.lap = bd_addr.lap;
    btbm_adp_msg->bd_addr.uap = bd_addr.uap;
    btbm_adp_msg->bd_addr.nap = bd_addr.nap;
    btbm_adp_msg->result = result;
    btbm_adp_msg->name_len = name_len;
    OS_MemSet(btbm_adp_msg->name, 0, sizeof(btbm_adp_msg->name));
    if (name_len != 0)
    {
        OS_MemCopy(btbm_adp_msg->name, name, name_len);
    }
    btbm_adp_msg->cod = cod;
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_READ_REMOTE_NAME_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}

/*****************************************************************************
 * FUNCTION
 *  BTBMAdpReadRemoteNameCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result          [IN]        
 *  bd_addr         [IN]        
 *  name_len        [IN]        
 *  name            [IN]         
 *  cod             [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpRemoteNameInd(
        btbm_bd_addr_t bd_addr,
        kal_uint8 name_len,
        kal_uint8 *name)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_remote_name_ind_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_remote_name_ind_struct*) construct_local_para(sizeof(bt_bm_remote_name_ind_struct), TD_UL);

    btbm_adp_msg->bd_addr.lap = bd_addr.lap;
    btbm_adp_msg->bd_addr.uap = bd_addr.uap;
    btbm_adp_msg->bd_addr.nap = bd_addr.nap;
    //btbm_adp_msg->result = result;
    btbm_adp_msg->name_len = name_len;
    OS_MemSet(btbm_adp_msg->name, 0, sizeof(btbm_adp_msg->name));
    if (name_len != 0)
    {
        OS_MemCopy(btbm_adp_msg->name, name, name_len);
    }
    //btbm_adp_msg->cod = cod;
    //MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_REMOTE_NAME_IND,
        MOD_MMI,
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpReadRemoteNameCancelCfm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpReadRemoteNameCancelCfm(kal_uint8 result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_read_remote_name_cancel_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_read_remote_name_cancel_cnf_struct*)
        construct_local_para(sizeof(bt_bm_read_remote_name_cancel_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_READ_REMOTE_NAME_CANCEL_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpSSPNumericInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bd_addr             [IN]        
 *  ssp_type            [IN]        
 *  numericValue        [IN]        
 *  name_len            [IN]        
 *  name                [IN]      
 *  displayNumeric      [IN]        
 *  cod                 [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpSSPNumericInd(
        btbm_bd_addr_t bd_addr,
        kal_uint8 ssp_type,
        kal_int32 numericValue,
        kal_uint8 name_len,
        kal_uint8 *name,
        kal_bool displayNumeric,
        kal_uint32 cod)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i = 0, j = 0;
    U8 temp_char = 0;
    U32 temp_value = 0;
    U32 divid_value = 0;
    local_para_struct *btbm_adp_msg;
    U8 numeric[6];
    msg_type msg;
    module_type dstMod = ((MTKBMAC(callback) == 0) ? MOD_MMI : MOD_BT); /* gap tester mode, and MTKBMAC(src_module) is set to MOD_BT */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    kal_trace(TRACE_GROUP_1, BTLOG_NUMERICVALUExD, numericValue);
    temp_value = numericValue;
    OS_MemSet(numeric, 0, 6);
    for (i = 0; i < 6; i++)
    {
        divid_value = 1;
        for (j = 0; j < 5 - i; j++)
        {
            divid_value *= 10;
        }
        temp_char = (U8) (temp_value / divid_value);
        kal_trace(TRACE_GROUP_1, BTLOG_TEMP_VALUExD, temp_value);
        kal_trace(TRACE_GROUP_1, BTLOG_DIVID_VALUExD, divid_value);
        kal_trace(TRACE_GROUP_1, BTLOG_NUMERIC_VALUE_IxD_TEMP_CHARxD, i, temp_char);
        numeric[i] = temp_char + '0';
        kal_trace(TRACE_GROUP_1, BTLOG_TEMP_CHARxC_COVERT_TO_STRINGxC, numeric[i]);
        temp_value = temp_value - divid_value * temp_char;
    }

    switch (ssp_type)
    {
    case BT_BM_SSP_PIN_PASSKEY_ENTRY:
        if (displayNumeric == TRUE)
        {
            bt_bm_security_passkey_notify_ind_struct *ind = 
                (bt_bm_security_passkey_notify_ind_struct *) construct_local_para(sizeof(bt_bm_security_passkey_notify_ind_struct), TD_UL | TD_RESET);
            ind->bd_addr = bd_addr;
            ind->passkey = numericValue;
            ind->name_len = name_len;
            ind->cod = cod;
            if (name_len != 0)
            {
                OS_MemCopy(ind->name, name, name_len);
            }
            msg = MSG_ID_BT_BM_SECURITY_PASSKEY_NOTIFY_IND;
            btbm_adp_msg = (local_para_struct *)ind;
        }
        else
        {
            bt_bm_security_passkey_entry_ind_struct *ind = 
                (bt_bm_security_passkey_entry_ind_struct *) construct_local_para(sizeof(bt_bm_security_passkey_entry_ind_struct), TD_UL | TD_RESET);
            ind->bd_addr = bd_addr;
            ind->name_len = name_len;
            ind->cod = cod;
            if (name_len != 0)
            {
                OS_MemCopy(ind->name, name, name_len);
            }
            msg = MSG_ID_BT_BM_SECURITY_PASSKEY_ENTRY_IND;
            btbm_adp_msg = (local_para_struct *)ind;
        }
        break;

    case BT_BM_SSP_PIN_USER_CONFIRM:
    {
        bt_bm_security_user_confirm_ind_struct *ind = 
            (bt_bm_security_user_confirm_ind_struct *) construct_local_para(sizeof(bt_bm_security_user_confirm_ind_struct), TD_UL | TD_RESET);
        ind->bd_addr = bd_addr;
        ind->name_len = name_len;
        ind->cod = cod;
        if (name_len != 0)
        {
            OS_MemCopy(ind->name, name, name_len);
        }
        OS_MemCopy(ind->numeric, numeric, 6);
        Report(("displayNumeric:%d", displayNumeric));
        ind->display_numeric = displayNumeric;
        msg = MSG_ID_BT_BM_SECURITY_USER_CONFIRM_IND;
        btbm_adp_msg = (local_para_struct *)ind;
    }
    break;

    case BT_BM_SSP_PIN_OOB_DATA:
    {
        bt_bm_security_oob_data_ind_struct *ind = 
            (bt_bm_security_oob_data_ind_struct *) construct_local_para(sizeof(bt_bm_security_oob_data_ind_struct), TD_UL | TD_RESET);
        ind->bd_addr = bd_addr;
        ind->name_len = name_len;
        ind->cod = cod;
        if (name_len != 0)
        {
            OS_MemCopy(ind->name, name, name_len);
        }
        msg = MSG_ID_BT_BM_SECURITY_OOB_DATA_IND;
        btbm_adp_msg = (local_para_struct *)ind;
    }
    default:
        OS_Report("BTBMAdpSSPNumericInd : wrong ssp_type=%u", ssp_type);
        Assert(0);
        return;
    }
    
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(msg, dstMod, BT_APP_SAP, btbm_adp_msg, NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpSSPPasskeyPressInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bd_addr     [IN]        
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpSSPPasskeyPressInd(btbm_bd_addr_t bd_addr, kal_uint8 event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_security_keypress_notify_ind_struct *btbm_adp_msg;
    module_type dstMod = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_security_keypress_notify_ind_struct*)
        construct_local_para(sizeof(bt_bm_security_keypress_notify_ind_struct), TD_UL);

    btbm_adp_msg->bd_addr = bd_addr;
    btbm_adp_msg->event = event;

    MTKBMAC(operation_ongoing) = FALSE;
    if (MTKBMAC(callback) == 0)
    {
        dstMod = MOD_MMI;
    }
    else
    {
        dstMod = MTKBMAC(src_module);   /* gap tester mode, and MTKBMAC(src_module) is set to MOD_BT */
    }
    bma_sendMsg(
        MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_IND,
        dstMod,
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpConnectAcceptInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  addr        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpConnectAcceptInd(btbm_bd_addr_t addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_link_connect_accept_ind_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_link_connect_accept_ind_struct*) construct_local_para(
                                                    sizeof(bt_bm_link_connect_accept_ind_struct),
                                                    TD_UL);
    btbm_adp_msg->bd_addr = addr;
    bma_sendMsg(MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_IND, MOD_MMI, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpReportReadRSSIResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  errCode                  [IN]        
 *  rssi_value               [IN]        
 *  lap             [IN]        
 *  uap            [IN]
 *  nap            [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpReportReadRSSIResult(U8 errCode, U8 rssi_value, U32 lap, U8 uap, U16 nap)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_read_rssi_cnf_struct *rssi_cnf =
        (bt_read_rssi_cnf_struct*) construct_local_para(sizeof(bt_read_rssi_cnf_struct), TD_UL);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    rssi_cnf->result = errCode;
    rssi_cnf->rssi_value = rssi_value;
    rssi_cnf->lap = lap;
    rssi_cnf->uap = uap;
    rssi_cnf->nap = nap;
    bma_sendMsg(MSG_ID_BT_READ_RSSI_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)rssi_cnf, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpReportNetworkState
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result                  [IN]        
 *  is_master               [IN]        
 *  is_authenticated        [IN]        
 *  is_encrypted            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpReportNetworkState(kal_uint8 result, kal_bool is_master, kal_bool is_authenticated, kal_bool is_encrypted)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_get_link_state_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_get_link_state_cnf_struct*) construct_local_para(sizeof(bt_bm_get_link_state_cnf_struct), TD_UL);
    btbm_adp_msg->result = result;
    btbm_adp_msg->is_master = is_master;
    btbm_adp_msg->is_authenticated = is_authenticated;
    btbm_adp_msg->is_encrypted = is_encrypted;
    bma_sendMsg(
        MSG_ID_BT_BM_GET_LINK_STATE_CNF,
        MTKBMAC(direct_rsp_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);

}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpReportScatternetState
 * DESCRIPTION
 *  
 * PARAMETERS
 *  piconet                  [IN]        
 *  scatternet               [IN]        
 *  sco                      [IN]        
 *  esco                     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpReportScatternetState(kal_uint8 piconet, kal_uint8 scatternet, kal_uint8 sco, kal_uint8 esco)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_get_scatternet_state_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_get_scatternet_state_cnf_struct*) construct_local_para(
                                                    sizeof(bt_bm_get_scatternet_state_cnf_struct),
                                                    TD_UL);
    btbm_adp_msg->piconet = piconet;
    btbm_adp_msg->scatternet = scatternet;
    btbm_adp_msg->sco = sco;
    btbm_adp_msg->esco = esco;
    bma_sendMsg(
        MSG_ID_BT_BM_GET_SCATTERNET_STATE_CNF,
        MTKBMAC(direct_rsp_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);

}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpServiceSearchResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  uuid        [IN]        
 *  addr        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpServiceSearchResult(kal_uint32 uuid, btbm_bd_addr_t addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_service_search_result_ind_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_service_search_result_ind_struct*) construct_local_para(
                                                    sizeof(bt_bm_service_search_result_ind_struct),
                                                    TD_UL);
    btbm_adp_msg->uuid = uuid;
    btbm_adp_msg->bd_addr = addr;
    bma_sendMsg(
        MSG_ID_BT_BM_SERVICE_SEARCH_RESULT_IND,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
    BTBMTimerStart(70000);
#endif /* MTK_BTBM_TIMER_CHECK */ 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpServiceSearchComplete
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  addr        [IN]        
 *  no          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpServiceSearchComplete(kal_uint8 result, btbm_bd_addr_t addr, kal_uint8 no)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_service_search_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_service_search_cnf_struct*) construct_local_para(sizeof(bt_bm_service_search_cnf_struct), TD_UL);
    btbm_adp_msg->result = result;
    btbm_adp_msg->bd_addr = addr;
    btbm_adp_msg->searched_number = no;
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_SERVICE_SEARCH_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpServiceSearchCancel
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  addr        [IN]        
 *  no          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpServiceSearchCancel(kal_uint8 result, btbm_bd_addr_t addr, kal_uint8 no)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_service_search_cancel_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_service_search_cancel_cnf_struct*) construct_local_para(
                                                    sizeof(bt_bm_service_search_cancel_cnf_struct),
                                                    TD_UL);
    btbm_adp_msg->result = result;
    btbm_adp_msg->bd_addr = addr;
    btbm_adp_msg->searched_number = no;
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_SERVICE_SEARCH_CANCEL_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpAttributeSearchResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  data        [IN]         
 *  addr        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpAttributeSearchResult(kal_uint16 len, kal_uint8 *data, btbm_bd_addr_t addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_search_attribut_result_ind_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_search_attribut_result_ind_struct*)
        construct_local_para(sizeof(bt_bm_search_attribut_result_ind_struct), TD_UL);
    btbm_adp_msg->len = len;
    btbm_adp_msg->bd_addr = addr;
    Assert(len < 56);
    OS_MemCopy(btbm_adp_msg->data, data, len);
    OS_Report("BTBMAdpAttributeSearchResult(): send MSG_ID_BT_BM_SEARCH_ATTRIBUTE_RESULT_IND");
    bma_sendMsg(
        MSG_ID_BT_BM_SEARCH_ATTRIBUTE_RESULT_IND,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
    BTBMTimerStart(70000);
#endif /* MTK_BTBM_TIMER_CHECK */ 
}

void BTAdpHciSnoopCfg(bt_hcisnoop_config_struct *cfg)
{
	OS_Report("BTAdpHciSnoopCfg(): enable %d\n", cfg->hcisnoop_enabled);
	if(cfg->hcisnoop_enabled)
		initVirtualSniff();
	else
		deinitVirtualSniff();

    bma_sendMsg(
        MSG_ID_BT_HCISNOOP_CONFIG_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        NULL,
        NULL);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpAttributeSearchComplete
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  addr        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
#if defined(BTMTK_ON_LINUX)
void BTBMAdpAttributeSearchComplete(kal_uint8 result, btbm_bd_addr_t addr, kal_uint16 len, kal_uint8 *data)
#else
void BTBMAdpAttributeSearchComplete(kal_uint8 result, btbm_bd_addr_t addr)
#endif
{
    bt_bm_search_attribute_cnf_struct *btbm_adp_msg;

    btbm_adp_msg =
        (bt_bm_search_attribute_cnf_struct*) construct_local_para(sizeof(bt_bm_search_attribute_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    btbm_adp_msg->bd_addr = addr;
#if defined(BTMTK_ON_LINUX)
    btbm_adp_msg->len = len;
    btbm_adp_msg->bd_addr = addr;
    Assert(len < 56);
    OS_MemCopy(btbm_adp_msg->data, data, len);
#endif
    MTKBMAC(operation_ongoing) = FALSE;
    bma_sendMsg(
        MSG_ID_BT_BM_SEARCH_ATTRIBUTE_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
#ifdef MTK_BTBM_TIMER_CHECK
    BTBMTimerCancel();
#endif 
    btbmSdpSearchAttributePendingInquiry(0);

}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpAttributeSearchComplete
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  addr        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpGetLinkKeyResult(kal_uint8 result, 
                                                                btbm_bd_addr_t bd_addr, 
                                                                btbm_device_type device_type, 
                                                                kal_uint32 cod,
                                                                kal_uint8 key_type,
                                                                kal_uint8 *key,
                                                                kal_uint8 persist)
{
    bt_bm_get_link_key_cnf_struct *btbm_adp_msg;

    btbm_adp_msg =
        (bt_bm_get_link_key_cnf_struct*) construct_local_para(sizeof(bt_bm_get_link_key_cnf_struct), TD_UL);

    btbm_adp_msg->result = result;
    btbm_adp_msg->bd_addr = bd_addr;
    btbm_adp_msg->device_type = device_type;
    btbm_adp_msg->cod = cod;
    btbm_adp_msg->link_key_type = key_type;
    OS_MemSet(btbm_adp_msg->link_key, 0, sizeof(btbm_adp_msg->link_key));
    OS_MemCopy(btbm_adp_msg->link_key, key, sizeof(btbm_adp_msg->link_key));
    btbm_adp_msg->persist = persist;
    
    bma_sendMsg(
        MSG_ID_BT_BM_GET_LINK_KEY_CNF,
        MTKBMAC(src_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
}


#ifdef MTK_BTBM_TIMER_CHECK

/*****************************************************************************
 * FUNCTION
 *  BTBMTimerHandler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Timer       [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
static void BTBMTimerHandler(EvmTimer *Timer)
{
    kal_trace(TRACE_GROUP_1,BT_BTBM_TIMEOUT);
#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(TRACE_GROUP_1, BT_PANIC_MESSAGE2);
#endif 
    HciTransportReset();
    bm_adp_deinit();
}


/*****************************************************************************
 * FUNCTION
 *  BTBMTimerStart
 * DESCRIPTION
 *  
 * PARAMETERS
 *  timeout_count       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMTimerStart(kal_uint32 timeout_count)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (BTCoreVerifySysInitState() == TRUE)
    {
        kal_trace(TRACE_GROUP_1,BT_BTBM_TIMESTART);

        if (MTKBMAC(bmaTimer).func != 0)
        {
            EVM_CancelTimer(&MTKBMAC(bmaTimer));
        }
        MTKBMAC(bmaTimer).func = BTBMTimerHandler;
        EVM_StartTimer(&MTKBMAC(bmaTimer), timeout_count);
    }
}


/*****************************************************************************
 * FUNCTION
 *  BTBMTimerCancel
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMTimerCancel(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (BTCoreVerifySysInitState() == TRUE)
    {
        kal_trace(TRACE_GROUP_1,BT_BTBM_TIMECANCEL);

        if (MTKBMAC(bmaTimer).func != 0)
        {
            EVM_CancelTimer(&MTKBMAC(bmaTimer));
        }
        MTKBMAC(bmaTimer).func = 0;
    }
}

#endif /* MTK_BTBM_TIMER_CHECK */ 

#if BT_JSR82_ENABLED


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpSearchAttributeRawResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  len         [IN]        
 *  data        [IN]        
 *  addr        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpSearchAttributeRawResult(kal_uint16 len, kal_uint8 *data, btbm_bd_addr_t addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_search_raw_result_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg =
        (bt_bm_search_raw_result_struct*) construct_local_para(sizeof(bt_bm_search_raw_result_struct), TD_UL);
    if (len > 0x64)
    {
        btbm_adp_msg->len = 0x64;
    }
    else
    {
        btbm_adp_msg->len = len;
    }
    btbm_adp_msg->bd_addr = addr;
    kal_trace(TRACE_GROUP_1, BTLOG_ATTRIBUTELIST_BYTE_COUNTxD, len);
    /* Modify by mtk01411: len <64 to len<=64 : 2007-1204 */
    Assert(len <= 0x64);
    OS_MemCopy(btbm_adp_msg->data, data, btbm_adp_msg->len);
    bma_sendMsg(
        MSG_ID_BT_BM_SEARCH_RAW_RESULT,
        MTKBMAC(ss_raw_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
}


/*****************************************************************************
 * FUNCTION
 *  BTBMAdpSearchAttributeRawComplete
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 *  addr        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMAdpSearchAttributeRawComplete(kal_uint8 result, btbm_bd_addr_t addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bm_search_raw_cnf_struct *btbm_adp_msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    btbm_adp_msg = (bt_bm_search_raw_cnf_struct*) construct_local_para(sizeof(bt_bm_search_raw_cnf_struct), TD_UL);
    btbm_adp_msg->result = result;
    btbm_adp_msg->bd_addr = addr;
    bma_sendMsg(
        MSG_ID_BT_BM_SEARCH_RAW_CNF,
        MTKBMAC(ss_raw_module),
        BT_APP_SAP,
        (local_para_struct*) btbm_adp_msg,
        NULL);
}

#endif /* BT_JSR82_ENABLED */ 

