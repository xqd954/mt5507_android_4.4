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

#include "bt_common.h"
#include "btconfig.h"
#include "sys/avalloc.h"
#include "sys/avsigmgr.h"
#include "osapi.h"
#include "sec.h"
#include "bluetooth_gap_struct.h"
#include "btbm.h"
#include "btbm_adp.h"
#include "sdap.h"
#include "gaptester.h"
#include "ddb.h"
#include "l2captester.h"
#include "btproperty.h"
#include "bt_feature.h"

#if defined(__BQE_TESTER_) && defined(__BMA_TESTER_)

extern void bma_sendMsg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff);

/*****************************************************************************
 * FUNCTION
 *  bm_adp_message_handler_inject_msg
 * DESCRIPTION
 *  
 * PARAMETERS
 *  string      [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void bm_adp_message_handler_inject_msg(char *string)
{
    if (strcmp(string, "bmapoweron") == 0)
    {
        bma_sendMsg(MSG_ID_BT_BM_LINK_ALLOW_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
    }
    else if (strcmp(string, "bmapoweroff") == 0)
    {
        bma_sendMsg(MSG_ID_BT_BM_LINK_DISALLOW_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
    }
    else if (strcmp(string, "bmainquiryno") == 0)
    {
        bt_bm_discovery_req_struct *btbm_adp_msg;

        btbm_adp_msg = (bt_bm_discovery_req_struct*) construct_local_para(sizeof(bt_bm_discovery_req_struct), TD_UL);

        btbm_adp_msg->cod = 0xffffffff;
        btbm_adp_msg->inquiry_length = 10;
        btbm_adp_msg->inquiry_number = 10;
        btbm_adp_msg->access_mode = BTBM_GENERAL_INQUIRY;
        btbm_adp_msg->discovery_mode = BTBM_DISCOVERY_WITHOUT_NAME;
        bma_sendMsg(MSG_ID_BT_BM_DISCOVERY_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }
    else if (strcmp(string, "bmainquiryname") == 0)
    {
        bt_bm_discovery_req_struct *btbm_adp_msg;

        btbm_adp_msg = (bt_bm_discovery_req_struct*) construct_local_para(sizeof(bt_bm_discovery_req_struct), TD_UL);

        btbm_adp_msg->cod = 0xffffffff;
        btbm_adp_msg->inquiry_length = 10;
        btbm_adp_msg->inquiry_number = 10;
        btbm_adp_msg->access_mode = BTBM_GENERAL_INQUIRY;
        btbm_adp_msg->discovery_mode = BTBM_DISCOVERY_WITH_NAME;
        bma_sendMsg(MSG_ID_BT_BM_DISCOVERY_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }
    else if (strcmp(string, "bmainquirycancel") == 0)
    {
        bma_sendMsg(MSG_ID_BT_BM_DISCOVERY_CANCEL_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
    }
    else if (strcmp(string, "bmawritename") == 0)
    {
        bt_bm_write_local_name_req_struct *btbm_adp_msg;
        kal_uint8 name[] = "test module";

        btbm_adp_msg =
            (bt_bm_write_local_name_req_struct*) construct_local_para(
                                                    sizeof(bt_bm_write_local_name_req_struct),
                                                    TD_UL);
        btbm_adp_msg->name_len = strlen((char*)name);
        OS_MemCopy((kal_uint8*) btbm_adp_msg->name, (kal_uint8*) name, strlen((char*)name) + 1);
        bma_sendMsg(MSG_ID_BT_BM_WRITE_LOCAL_NAME_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }
    else if (strcmp(string, "bmareadname") == 0)
    {
        bt_bm_read_remote_name_req_struct *btbm_adp_msg;

        btbm_adp_msg =
            (bt_bm_read_remote_name_req_struct*) construct_local_para(
                                                    sizeof(bt_bm_read_remote_name_req_struct),
                                                    TD_UL);
        btbm_adp_msg->bd_addr.lap = 0x00bf57f1;
        btbm_adp_msg->bd_addr.nap = 0x0060;
        btbm_adp_msg->bd_addr.uap = 0x57;
        bma_sendMsg(MSG_ID_BT_BM_READ_REMOTE_NAME_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }
    else if (strcmp(string, "bmawritescanon") == 0)
    {
        bt_bm_write_scanenable_mode_req_struct *btbm_adp_msg;

        btbm_adp_msg =
            (bt_bm_write_scanenable_mode_req_struct*)
            construct_local_para(sizeof(bt_bm_write_scanenable_mode_req_struct), TD_UL);
        btbm_adp_msg->modenotconnected = BTBM_ADP_P_ON_I_ON;
        btbm_adp_msg->modeconnected = BTBM_ADP_P_ON_I_ON;
        bma_sendMsg(
            MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ,
            MOD_BT,
            BT_APP_SAP,
            (local_para_struct*) btbm_adp_msg,
            NULL);
    }
    else if (strcmp(string, "bmawritescanoff") == 0)
    {
        bt_bm_write_scanenable_mode_req_struct *btbm_adp_msg;

        btbm_adp_msg =
            (bt_bm_write_scanenable_mode_req_struct*)
            construct_local_para(sizeof(bt_bm_write_scanenable_mode_req_struct), TD_UL);
        btbm_adp_msg->modenotconnected = BTBM_ADP_P_ON_I_OFF;
        btbm_adp_msg->modeconnected = BTBM_ADP_P_ON_I_OFF;
        bma_sendMsg(
            MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_REQ,
            MOD_BT,
            BT_APP_SAP,
            (local_para_struct*) btbm_adp_msg,
            NULL);
    }
    else if (strcmp(string, "bmareadvisiblemode") == 0)
    {
        bma_sendMsg(MSG_ID_BT_BM_READ_SCANENABLE_MODE_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
    }
    else if (strcmp(string, "bmareadaddr") == 0)
    {
        bma_sendMsg(MSG_ID_BT_BM_READ_LOCAL_ADDR_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
    }
    else if (strcmp(string, "bmareadcod") == 0)
    {
        bma_sendMsg(MSG_ID_BT_BM_READ_LOCAL_COD_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
    }
    else if (strcmp(string, "bmareadprop") == 0)
    {
        bma_sendMsg(MSG_ID_BT_BM_READ_PROPERTY_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
    }
    else if (strcmp(string, "bmasavesetting") == 0)
    {
        bma_sendMsg(MSG_ID_BT_BM_SAVE_SETTING_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
    }
    else if (strcmp(string, "bmarestoresetting") == 0)
    {
        bma_sendMsg(MSG_ID_BT_BM_RESTORE_SETTING_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
    }
    else if (strcmp(string, "bmaauthmodeon") == 0)
    {
        bt_bm_write_authentication_mode_req_struct *btbm_adp_msg;

        btbm_adp_msg =
            (bt_bm_write_authentication_mode_req_struct*)
            construct_local_para(sizeof(bt_bm_write_authentication_mode_req_struct), TD_UL);
        btbm_adp_msg->mode = BTBM_AUTHENTICATION_ENABLE;
        bma_sendMsg(
            MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_REQ,
            MOD_BT,
            BT_APP_SAP,
            (local_para_struct*) btbm_adp_msg,
            NULL);
    }
    else if (strcmp(string, "bmaauthmodeoff") == 0)
    {
        bt_bm_write_authentication_mode_req_struct *btbm_adp_msg;

        btbm_adp_msg =
            (bt_bm_write_authentication_mode_req_struct*)
            construct_local_para(sizeof(bt_bm_write_authentication_mode_req_struct), TD_UL);
        btbm_adp_msg->mode = BTBM_AUTHENTICATION_DISABLE;
        bma_sendMsg(
            MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_REQ,
            MOD_BT,
            BT_APP_SAP,
            (local_para_struct*) btbm_adp_msg,
            NULL);
    }
    else if (strcmp(string, "bmapincode") == 0)
    {
        bt_bm_pin_code_rsp_struct *btbm_adp_msg;

        btbm_adp_msg = (bt_bm_pin_code_rsp_struct*) construct_local_para(sizeof(bt_bm_pin_code_rsp_struct), TD_UL);
        btbm_adp_msg->bd_addr.lap = 0x00bf57f1;
        btbm_adp_msg->bd_addr.nap = 0x0060;
        btbm_adp_msg->bd_addr.uap = 0x57;
        btbm_adp_msg->pin_len = 0x04;
        btbm_adp_msg->pin_code[0] = '1';
        btbm_adp_msg->pin_code[1] = '2';
        btbm_adp_msg->pin_code[2] = '3';
        btbm_adp_msg->pin_code[3] = '4';
        bma_sendMsg(MSG_ID_BT_BM_PIN_CODE_RSP, MOD_BT, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }
    else if (strcmp(string, "bmadebond") == 0)
    {
        bt_bm_delete_trust_req_struct *btbm_adp_msg;

        btbm_adp_msg =
            (bt_bm_delete_trust_req_struct*) construct_local_para(sizeof(bt_bm_delete_trust_req_struct), TD_UL);
        btbm_adp_msg->bd_addr.lap = 0x00bf57f1;
        btbm_adp_msg->bd_addr.nap = 0x0060;
        btbm_adp_msg->bd_addr.uap = 0x57;

        bma_sendMsg(MSG_ID_BT_BM_DELETE_TRUST_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }
    else if (strcmp(string, "bmabondcancel") == 0)
    {
        bt_bm_bonding_cancel_req_struct *btbm_adp_msg;

        btbm_adp_msg =
            (bt_bm_bonding_cancel_req_struct*) construct_local_para(sizeof(bt_bm_bonding_cancel_req_struct), TD_UL);
        btbm_adp_msg->bd_addr.lap = 0x00bf57f1;
        btbm_adp_msg->bd_addr.nap = 0x0060;
        btbm_adp_msg->bd_addr.uap = 0x57;

        bma_sendMsg(MSG_ID_BT_BM_BONDING_CANCEL_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }
    else if (strcmp(string, "bmapaircancel") == 0)
    {
        bt_bm_pairing_cancel_req_struct *btbm_adp_msg;

        btbm_adp_msg =
            (bt_bm_pairing_cancel_req_struct*) construct_local_para(sizeof(bt_bm_pairing_cancel_req_struct), TD_UL);
        btbm_adp_msg->bd_addr.lap = 0x00bf57f1;
        btbm_adp_msg->bd_addr.nap = 0x0060;
        btbm_adp_msg->bd_addr.uap = 0x57;

        bma_sendMsg(MSG_ID_BT_BM_PAIRING_CANCEL_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }

    else if (strcmp(string, "bmabondnosdap") == 0)
    {
        bt_bm_bonding_req_struct *btbm_adp_msg;

        btbm_adp_msg = (bt_bm_bonding_req_struct*) construct_local_para(sizeof(bt_bm_bonding_req_struct), TD_UL);
        btbm_adp_msg->bd_addr.lap = 0x00bf57f1;
        btbm_adp_msg->bd_addr.nap = 0x0060;
        btbm_adp_msg->bd_addr.uap = 0x57;
        btbm_adp_msg->sdap_len = 0x00;
        bma_sendMsg(MSG_ID_BT_BM_BONDING_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }
    else if (strcmp(string, "bmabondsdap") == 0)
    {
        bt_bm_bonding_req_struct *btbm_adp_msg;

        btbm_adp_msg = (bt_bm_bonding_req_struct*) construct_local_para(sizeof(bt_bm_bonding_req_struct), TD_UL);
        btbm_adp_msg->bd_addr.lap = 0x00bf57f1;
        btbm_adp_msg->bd_addr.nap = 0x0060;
        btbm_adp_msg->bd_addr.uap = 0x57;
        btbm_adp_msg->sdap_len = 0x03;
        btbm_adp_msg->sdap_uuid[0] = 0x1101;
        btbm_adp_msg->sdap_uuid[1] = 0x1102;
        btbm_adp_msg->sdap_uuid[2] = 0x1103;
        bma_sendMsg(MSG_ID_BT_BM_BONDING_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }
    else if (strcmp(string, "bmadebond") == 0)
    {
        bt_bm_delete_trust_req_struct *btbm_adp_msg;

        btbm_adp_msg =
            (bt_bm_delete_trust_req_struct*) construct_local_para(sizeof(bt_bm_delete_trust_req_struct), TD_UL);
        btbm_adp_msg->bd_addr.lap = 0x00bf57f1;
        btbm_adp_msg->bd_addr.nap = 0x0060;
        btbm_adp_msg->bd_addr.uap = 0x57;
        bma_sendMsg(MSG_ID_BT_BM_DELETE_TRUST_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }
    else if (strcmp(string, "sdapss") == 0)
    {
        bt_bm_service_search_req_struct *btbm_adp_msg;

        btbm_adp_msg =
            (bt_bm_service_search_req_struct*) construct_local_para(sizeof(bt_bm_service_search_req_struct), TD_UL);
        btbm_adp_msg->bd_addr.lap = 0x00bf57f1;
        btbm_adp_msg->bd_addr.nap = 0x0060;
        btbm_adp_msg->bd_addr.uap = 0x57;
        btbm_adp_msg->sdap_len = 0x05;
        btbm_adp_msg->sdap_uuid[0] = 0x1101;
        btbm_adp_msg->sdap_uuid[1] = 0x1102;
        btbm_adp_msg->sdap_uuid[2] = 0x1103;
        btbm_adp_msg->sdap_uuid[4] = 0x1104;
        btbm_adp_msg->sdap_uuid[5] = 0x1105;

        bma_sendMsg(MSG_ID_BT_BM_SERVICE_SEARCH_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }
}


#endif /* defined(__BQE_TESTER_) && defined(__BMA_TESTER_) */ 



#ifdef __BMA_TESTER_
/*****************************************************************************
 * FUNCTION
 *  bma_tester_handler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void bma_tester_handler(ilm_struct *ilm_ptr)
{
    if (MTKBMAC(callback) != 0)
    {
        MTKBMAC(callback) (ilm_ptr);
    }
    else
    {
    #if defined(__BQE_TESTER_) && defined(__BMA_TESTER_)
        switch (ilm_ptr->msg_id)
        {
            case MSG_ID_BT_BM_LINK_ALLOW_CNF:
                break;
            case MSG_ID_BT_BM_LINK_DISALLOW_CNF:
                break;
            case MSG_ID_BT_BM_DISCOVERY_RESULT_IND:
                break;
            case MSG_ID_BT_BM_DISCOVERY_CNF:
                break;
            case MSG_ID_BT_BM_DISCOVERY_CANCEL_CNF:
                break;
            case MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF:
                break;
            case MSG_ID_BT_BM_READ_REMOTE_NAME_CNF:
                break;
            case MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF:
                break;
            case MSG_ID_BT_BM_READ_SCANENABLE_MODE_CNF:
                break;
            case MSG_ID_BT_BM_READ_LOCAL_ADDR_CNF:
                break;
            case MSG_ID_BT_BM_READ_LOCAL_COD_CNF:
                break;
            case MSG_ID_BT_BM_READ_PROPERTY_CNF:
                break;
            case MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_CNF:
                break;
            case MSG_ID_BT_BM_PIN_CODE_IND:
                break;
            case MSG_ID_BT_BM_BONDING_CNF:
                break;
            case MSG_ID_BT_BM_BONDING_RESULT_IND:
                break;
            case MSG_ID_BT_BM_SAVE_SETTING_CNF:
                break;
            case MSG_ID_BT_BM_RESTORE_SETTING_CNF:
                break;
            case MSG_ID_BT_BM_DELETE_TRUST_CNF:
                break;
            case MSG_ID_BT_BM_BONDING_CANCEL_CNF:
                break;
            case MSG_ID_BT_BM_PAIRING_CANCEL_CNF:
                break;
            case MSG_ID_BT_BM_SERVICE_SEARCH_RESULT_IND:
                break;
            case MSG_ID_BT_BM_SERVICE_SEARCH_CNF:
                break;
        }
    #endif /* defined(__BQE_TESTER_) && defined(__BMA_TESTER_) */ 
    }
}

#endif
