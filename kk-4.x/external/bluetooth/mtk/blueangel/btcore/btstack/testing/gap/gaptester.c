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

#ifdef __GAP_TESTER_
  
BtGapTesterContext gapTesterContext;
U8 gaptester_discovery_ask_without_name;
U8 gaptester_bond_without_sdp;
BOOL gaptester_inquiry_from_bt;

#define GAPTS(s) (gapTesterContext.s)

extern void BTBMGapBondingforGAPTSDevice(bt_bm_bonding_req_struct *btbm_adp_input_msg);
extern void CONVERT_BDADDR2ARRAY(U8 *dest, U32 lap, U8 uap, U16 nap);
extern void CONVERT_ARRAY2BDADDR(btbm_bd_addr_t *dest, U8 *src);
extern void L2capTesterSetSecurity(U8 level);

static void gap_tester_sendMsg(
                                        msg_type msg, 
                                        module_type dstMod ,
                                        sap_type sap,
                                        local_para_struct *local_para,
                                        peer_buff_struct *peer_buff);

static void gap_tester_sendMsg(
                                        msg_type msg, 
                                        module_type dstMod ,
                                        sap_type sap,
                                        local_para_struct *local_para,
                                        peer_buff_struct *peer_buff)
{
    ilm_struct *ilmPtr;    

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = msg;
    ilmPtr->local_para_ptr = local_para;
    ilmPtr->peer_buff_ptr = peer_buff;
    ilmPtr->dest_mod_id = dstMod;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = sap;
    msg_send_ext_queue( ilmPtr);    
}

void gapTesterInit(void)
{
    memset((U8 *)&gapTesterContext, 0, sizeof(gapTesterContext));
    gaptester_discovery_ask_without_name =1;
    gaptester_bond_without_sdp = 1;
#if defined (__BT_2_1_SIMPLE_PAIRING__)
    BTBMGapEnabledNonBondingTSDevice(0);
    BTBMGapSetSSPBondingTSDevice(0);
    l2capTesterInit();
#endif
    bma_gap_testing(gapTesterCallback);
    ME_DisableAFH();
}

void gapTesterNonDiscoverableMode(void)
{
    bt_bm_write_scanenable_mode_req_struct ptr;

//    ptr.modeconnected = 0x00;
    ptr.mode = 0x00;
    BTBMGapWriteScanEnabled(&ptr);    
}

void gapTesterLimitedDiscoverableMode(void)
{
    bt_bm_write_scanenable_mode_req_struct ptr;

//    ptr.modeconnected = 0x03 | 0x10;
    ptr.mode = 0x03 | 0x10;
    BTBMGapWriteScanEnabled(&ptr);    
}


void gapTesterGeneralDiscoverableMode(void)
{
    bt_bm_write_scanenable_mode_req_struct ptr;

//  ptr.modeconnected = 0x03;
    ptr.mode = 0x03;
    BTBMGapWriteScanEnabled(&ptr);    
}

void gapTesterNonConnectableMode(void)
{
    bt_bm_write_scanenable_mode_req_struct ptr;

//  ptr.modeconnected = 0x00;
    ptr.modenotconnected = 0x00;
    BTBMGapWriteScanEnabled(&ptr);    
}

void gapTesterConnectableMode(void)
{
    bt_bm_write_scanenable_mode_req_struct ptr;

//  ptr.modeconnected = 0x03;
    ptr.modenotconnected = 0x03;
    BTBMGapWriteScanEnabled(&ptr);    
}

void gapTesterNonPairable(void)
{
    return;
}

void gapTesterPairable(void)
{
    GAPTS(pairing_allowed) = 0x01;
}

void gapTesterSecurityLevel1(void)
{
    bt_bm_write_authentication_mode_req_struct ptr;

    ptr.mode = BTBM_AUTHENTICATION_DISABLE;
    BTBMGapSetAuthenticationMode(TRUE, &ptr);    
}

void gapTesterSecurityLevel2(void)
{
    bt_bm_write_authentication_mode_req_struct ptr;

    ptr.mode = BTBM_AUTHENTICATION_DISABLE;
    BTBMGapSetAuthenticationMode(TRUE, &ptr);    
    GAPTS(secRec).id =  SEC_L2CAP_ID;
    GAPTS(secRec).channel = BT_PSM_RFCOMM;
    GAPTS(secRec).level = BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT;
    SEC_Register(&GAPTS(secRec));    
}

void gapTesterSecurityLevel3(U8 encrypt_mode)
{
    bt_bm_write_authentication_mode_req_struct ptr;
    if(encrypt_mode == 0)
        //For GAP PTW70 Test cases: TC_SEC_AUT_BV_01_C and TC_SEC_SEM_BV_03_C
        ptr.mode = BTBM_AUTHENTICATION_ENABLE_LEVEL3_ENCRYPT_OFF;
    else
        ptr.mode = BTBM_AUTHENTICATION_ENABLE_LEVEL3_ENCRYPT_ON;
    BTBMGapSetAuthenticationMode(TRUE, &ptr);    
}

void gapTesterEncryptionOff(void)
{
    BTBMGapSetEncryptionOff();    
}

void gapTesterGeneralInquiry(void)
{
    bt_bm_discovery_req_struct ptr;

    ptr.access_mode = BTBM_GENERAL_INQUIRY;
    ptr.cod = 0xFFFFFFFF;
    ptr.discovery_mode = BTBM_DISCOVERY_WITHOUT_NAME;
    ptr.inquiry_length = 0x10;
    ptr.inquiry_number = 0x10;
    gaptester_inquiry_from_bt = TRUE;
    BTBMGapInquiry(&ptr);    
}

void gapTesterLimitedInquiry(void)
{
    bt_bm_discovery_req_struct ptr;

    ptr.access_mode = BTBM_LIMITED_INQUIRY;
    ptr.cod = 0xFFFFFFFF;
    ptr.discovery_mode = BTBM_DISCOVERY_WITHOUT_NAME;
    ptr.inquiry_length = 0x10;
    ptr.inquiry_number = 0x10;
    gaptester_inquiry_from_bt = TRUE;
    BTBMGapInquiry(&ptr);    
}

void gapTesterDeviceDiscovery(void)
{
    bt_bm_discovery_req_struct ptr;

    ptr.access_mode = BTBM_GENERAL_INQUIRY;
    ptr.cod = 0xFFFFFFFF;
    ptr.discovery_mode = BTBM_DISCOVERY_WITH_NAME;
    ptr.inquiry_length = 60;
    ptr.inquiry_number = 0x10;
    gaptester_inquiry_from_bt = TRUE;
    BTBMGapInquiry(&ptr);    
}

void gapTesterBonding(btbm_bd_addr_t bd_addr)
{
    bt_bm_bonding_req_struct ptr;

    ptr.bd_addr = bd_addr;
    ptr.sdap_len = 0x00;
    BTBMGapBondingDevice(&ptr);
}

void gapTesterAuth(btbm_bd_addr_t bd_addr)
{
    bt_bm_bonding_req_struct ptr;

    ptr.bd_addr = bd_addr;
    ptr.sdap_len = 0x00;
    BTBMGapBondingforGAPTSDevice(&ptr);
}

void gapTesterCreateAclLink(btbm_bd_addr_t bd_addr)
{
    bt_bm_acl_link_req_struct ptr;

    ptr.bd_addr = bd_addr;
    BTBMGapLinkCreate(&ptr);
}

void gapTesterCreateChannel(btbm_bd_addr_t bd_addr)
{
    L2capConnectRequest ptr;

    CONVERT_BDADDR2ARRAY(ptr.bd_addr, bd_addr.lap, bd_addr.uap, bd_addr.nap);
    l2capTesterInit();
    L2CA_ConnectReq(&ptr);
}

void gapTesterDeleteLinkKey(btbm_bd_addr_t bd_addr)
{
    bt_bm_delete_trust_req_struct ptr;

    ptr.bd_addr = bd_addr;
    BTBMGapDeleteTrustDevice(&ptr);    
}

/*Add by mtk01411: 2007-0703*/
void gapTesterSetFilterCoD(U32 setcod)
{

    BTBMGapSetFilterCoD(setcod);
}

void gapTesterCallback(ilm_struct *ilm_ptr)
{
    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_BM_DISCOVERY_RESULT_IND:
            {
                bt_bm_discovery_result_ind_struct *msg = (bt_bm_discovery_result_ind_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_DISCOVERY_BDADDR, msg->bd_addr.nap,msg->bd_addr.uap,msg->bd_addr.lap,msg->cod);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_NAME_LEN, msg->name_len);
            }
            break;
        case MSG_ID_BT_BM_DISCOVERY_CNF:
            {
                bt_bm_discovery_cnf_struct *msg = (bt_bm_discovery_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_DISCOVERY_COMPLETE,msg->result,msg->total_number);
            }
            break;
        case MSG_ID_BT_BM_DISCOVERY_CANCEL_CNF:
            {
                bt_bm_discovery_cancel_cnf_struct *msg = (bt_bm_discovery_cancel_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_DISCOVERY_CANCEL, msg->result,msg->total_number);
            }
            break;
        case MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF:
            {
                bt_bm_write_local_name_cnf_struct *msg = (bt_bm_write_local_name_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_WRITE_LOCAL_NAME, msg->result);
            }
            break;
        case MSG_ID_BT_BM_READ_REMOTE_NAME_CNF:
            {
                bt_bm_read_remote_name_cnf_struct *msg = (bt_bm_read_remote_name_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BDADDR, msg->bd_addr.nap,msg->bd_addr.uap,msg->bd_addr.lap);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_DISCOVERY_RESULT_NAME_LEN, msg->result,msg->name_len);
                {
                    int i;
                    for (i=0; i< msg->name_len;i++)
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_DISCOVERY_NAME_CHAR, msg->name[i]);
                }
            }
            break;
        case MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF:
            {
                bt_bm_write_scanenable_mode_cnf_struct *msg = (bt_bm_write_scanenable_mode_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_SET_WRITESCAN_RESULT,msg->result);
            }            
            break;
        case MSG_ID_BT_BM_READ_SCANENABLE_MODE_CNF:
            {
                bt_bm_read_scanenable_mode_cnf_struct *msg = (bt_bm_read_scanenable_mode_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_READ_WRITESCAN_RESULT,msg->result, msg->modenotconnected,msg->modeconnected);
            }                     
            break;
        case MSG_ID_BT_BM_READ_LOCAL_ADDR_CNF:
            {
                bt_bm_read_local_addr_cnf_struct *msg = (bt_bm_read_local_addr_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BDADDR, msg->bd_addr.nap,msg->bd_addr.uap,msg->bd_addr.lap);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_READ_LOCAL_ADDR_RESULT,msg->result);
            }                     
            break;
        case MSG_ID_BT_BM_READ_LOCAL_COD_CNF:
            {
                bt_bm_read_local_cod_cnf_struct *msg = (bt_bm_read_local_cod_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BDADDR, msg->result,msg->cod);
            }                
            break;
        case MSG_ID_BT_BM_READ_PROPERTY_CNF:
            break;
        case MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_CNF:
            {
                bt_bm_write_authentication_mode_cnf_struct *msg = (bt_bm_write_authentication_mode_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_SET_AUTHENTICATION_RESULT, msg->result);
            }                
            break;
        case MSG_ID_BT_BM_PIN_CODE_IND:
            {
                bt_bm_pin_code_ind_struct *msg = (bt_bm_pin_code_ind_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BDADDR, msg->bd_addr.nap,msg->bd_addr.uap,msg->bd_addr.lap);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_NAME_LEN, msg->name_len);
                {
                    int i;
                    for (i=0; i< msg->name_len;i++)
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_DISCOVERY_NAME_CHAR, msg->name[i]);
                }
                {
                    bt_bm_pin_code_rsp_struct ptr;
                    ptr.bd_addr.lap = msg->bd_addr.lap;
                    ptr.bd_addr.nap = msg->bd_addr.nap;
                    ptr.bd_addr.uap = msg->bd_addr.uap;
                    ptr.pin_code[0] = '0';
                    ptr.pin_code[1] = '0';        
                    ptr.pin_code[2] = '0';
                    ptr.pin_code[3] = '0';        
                    if(GAPTS(pairing_allowed) !=0)
                        ptr.pin_len= 0x04;
                    else
                        ptr.pin_len= 0x00;
                    BTBMGapWritePINCode(&ptr);
                }
            }                
            break;
#if defined (__BT_2_1_SIMPLE_PAIRING__)
        case MSG_ID_BT_BM_SECURITY_USER_CONFIRM_IND:
            {
                bt_bm_security_user_confirm_ind_struct *msg = (bt_bm_security_user_confirm_ind_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BDADDR, msg->bd_addr.nap,msg->bd_addr.uap,msg->bd_addr.lap);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_NAME_LEN, msg->name_len);
                {
                    int i;
                    for (i=0; i< msg->name_len;i++)
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_DISCOVERY_NAME_CHAR, msg->name[i]);
                }
                {
                    bt_bm_security_user_confirm_rsp_struct ptr;
                    ptr.bd_addr.lap = msg->bd_addr.lap;
                    ptr.bd_addr.nap = msg->bd_addr.nap;
                    ptr.bd_addr.uap = msg->bd_addr.uap;
                    ptr.accept = 0x01;
                    BTBMGapWriteUserConfirm(&ptr);
                }
            }                

            break;
#endif
        case MSG_ID_BT_BM_BONDING_CNF:
            {
                bt_bm_bonding_cnf_struct *msg = (bt_bm_bonding_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BONDING_CNF_RESULT, msg->result);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BDADDR, msg->bd_addr.nap,msg->bd_addr.uap,msg->bd_addr.lap);
            }                  
            break;
        case MSG_ID_BT_BM_BONDING_RESULT_IND:
            {
                bt_bm_bonding_result_ind_struct *msg = (bt_bm_bonding_result_ind_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BONDING_RESULT_IND, msg->result,msg->cod);
            }                
            break;
        case MSG_ID_BT_BM_SAVE_SETTING_CNF:
            {
                bt_bm_save_setting_cnf_struct *msg = (bt_bm_save_setting_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_SAVE_SETTING_RESULT, msg->result);
            }                 
            break;
        case MSG_ID_BT_BM_RESTORE_SETTING_CNF:
            {
                bt_bm_restore_setting_cnf_struct *msg = (bt_bm_restore_setting_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_RESTORE_SETTING_RESULT, msg->result);
            }             
            break;
        case MSG_ID_BT_BM_DELETE_TRUST_CNF:
            {
                bt_bm_delete_trust_cnf_struct *msg = (bt_bm_delete_trust_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_DELETE_TRUST_RESULT, msg->result);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BDADDR, msg->bd_addr.nap,msg->bd_addr.uap,msg->bd_addr.lap);
            }                  
            break;            
        case MSG_ID_BT_BM_BONDING_CANCEL_CNF:
            {
                bt_bm_bonding_cancel_cnf_struct *msg = (bt_bm_bonding_cancel_cnf_struct*)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BONDING_CANCEL_RESULT, msg->result);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BDADDR, msg->bd_addr.nap,msg->bd_addr.uap,msg->bd_addr.lap);
            }                  
            break;
        case MSG_ID_BT_BM_PAIRING_CANCEL_CNF:
            {
                bt_bm_pairing_cancel_cnf_struct *msg = (bt_bm_pairing_cancel_cnf_struct*)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_PAIRING_CANCEL_RESULT, msg->result);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BDADDR, msg->bd_addr.nap,msg->bd_addr.uap,msg->bd_addr.lap);
            }                  
            break;

        case MSG_ID_BT_BM_SERVICE_SEARCH_RESULT_IND:
            {
                bt_bm_service_search_result_ind_struct *msg = (bt_bm_service_search_result_ind_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_SUPPORTED_UUID, msg->uuid);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BDADDR, msg->bd_addr.nap,msg->bd_addr.uap,msg->bd_addr.lap);
            }             
            break;
        case MSG_ID_BT_BM_SERVICE_SEARCH_CNF:
            {
                bt_bm_service_search_cnf_struct *msg = (bt_bm_service_search_cnf_struct *)ilm_ptr->local_para_ptr;
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_SEARCHED_COMPLETE_RESULT, msg->result, msg->searched_number);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BDADDR, msg->bd_addr.nap,msg->bd_addr.uap,msg->bd_addr.lap);
            }             
            break;
    }

}

void gaptester_handler(ilm_struct *ilm_ptr)
{
    char *string;
    bt_test_cmd_struct *test_msg;
    if (ilm_ptr->msg_id == MSG_ID_BT_GAP_TEST_CMD_REQ)
    {
        test_msg = ((bt_test_cmd_struct *)ilm_ptr->local_para_ptr);    
        string = (char *)test_msg->string;    
        if(strncmp(string, "init",4)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_INIT);
            gapTesterInit();
        }
        else if(strncmp(string, "nondiscovery",12)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_NONDISCOVERY);        
            gapTesterNonDiscoverableMode();
        }
        else if(strncmp(string, "limiteddiscovery",16)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_LIMITEDDISCOVERY);             
            gapTesterLimitedDiscoverableMode();
        }
        else if(strncmp(string, "generaldiscovery",16)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_GENERALDISCOVERY);             
            gapTesterGeneralDiscoverableMode();
        }
        else if(strncmp(string, "nonconnect",10)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_NONCONNECTABLE);                     
            gapTesterNonConnectableMode();
        }
        else if(strncmp(string, "nonpaired",9)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_NONPAIRED);            
            gapTesterConnectableMode();
#if defined (__BT_2_1_SIMPLE_PAIRING__)
            BTBMGapEnabledNonBondingTSDevice(1);
#endif
            gapTesterNonPairable();

        }
        else if(strncmp(string, "paired",6)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_PAIRED);          
            gapTesterConnectableMode();
#if defined (__BT_2_1_SIMPLE_PAIRING__)
            BTBMGapEnabledNonBondingTSDevice(0);
#endif
            gapTesterPairable();
        }
        else if(strncmp(string, "seclevel3e",10)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_SECLEVEL3);          
            gapTesterPairable();
            DDB_FlushAllRecord();
            gapTesterConnectableMode();
            if(string[10]=='0')
                //With encryption mode: Disable
                gapTesterSecurityLevel3(0);
            else
                //With encryption mode: Enable
                gapTesterSecurityLevel3(1);
        }
        else if(strncmp(string, "seclevel1",9)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_SECLEVEL1);          
            gapTesterPairable();
            gapTesterConnectableMode();
            gapTesterSecurityLevel1();
        }
#if defined (__BT_2_1_SIMPLE_PAIRING__)
        else if(strncmp(string, "sspbond0",8)==0)
        {
            Report(("Dedicated bonding"));
            BTBMGapSetSSPBondingTSDevice(0);
        }
        else if(strncmp(string, "sspbond1",8)==0)
        {
            Report(("General bonding"));
            BTBMGapSetSSPBondingTSDevice(1);
        }
        else if(strncmp(string, "sspbond2",8)==0)
        {
            Report(("Non bonding"));
            BTBMGapSetSSPBondingTSDevice(2);
        }       
        else if(strncmp(string, "sspbond3",8)==0)
        {
            Report(("Dedicated bonding"));
            BTBMGapSetSSPBondingTSDevice(3);
        }
        else if(strncmp(string, "sspbond4",8)==0)
        {
            Report(("General bonding"));
            BTBMGapSetSSPBondingTSDevice(4);
        }
        else if(strncmp(string, "sspbond5",8)==0)
        {
            Report(("Non bonding"));
            BTBMGapSetSSPBondingTSDevice(5);
        }            
        else if(strncmp(string, "sspchannel0",11)==0)
        {
            Report(("Set security channel to low"));
            L2capTesterSetSecurity(BT_SEC_LOW);
        }
        else if(strncmp(string, "sspchannel1",11)==0)
        {
            Report(("Set security channel to med"));
            L2capTesterSetSecurity(BT_SEC_MED);
        }
        else if(strncmp(string, "sspchannel2",11)==0)
        {
            Report(("Set security channel to high"));
            L2capTesterSetSecurity(BT_SEC_HIGH);
        }

#endif

        else if(strncmp(string, "seclevel2",9)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_SECLEVEL2);          
            gapTesterPairable();
            gapTesterConnectableMode();
            gapTesterSecurityLevel2();
        }
        else if(strncmp(string, "ginq",4)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_GENERALINQUIRY);           
            bt_trace(TRACE_GROUP_1,GAP_TESTER_SET_TO_GENERAL_INQUIRY);
            gapTesterGeneralInquiry();
        }    
        else if(strncmp(string, "linq",4)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_LIMITEDINQUIRY);           
            gapTesterLimitedInquiry();
        }    
        else if(strncmp(string, "ssp4linkauth",12)==0)
        {
            Report(("Authenticated Link"));
            btbmGAPTesterAuthenticationLink();
        }            
        else if(strncmp(string, "ssp4linkency",12)==0)
        {
            Report(("encryption  Link"));
            btbmGAPTesterEncryptedLink();
        }            
        else if(strncmp(string, "devicediscovery",15)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_DEVICEDISCOVERY);              
            gapTesterPairable();
            gapTesterDeviceDiscovery();
        }    
        else if(strncmp(string, "auth",4)==0)
        {
            U8 bd_addr[6];
            btbm_bd_addr_t remote_addr;

            gapTesterPairable();
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BONDING);              

            memset(bd_addr, 0, 6);
            if(!gapTester_get_bdaddr_from_string(bd_addr, string+4))
            {
                return;
            }
            CONVERT_ARRAY2BDADDR(&remote_addr, bd_addr);
            gapTesterAuth(remote_addr);
        }    
        else if(strncmp(string, "bond",4)==0)
        {
            U8 bd_addr[6];
            btbm_bd_addr_t remote_addr;

            gapTesterPairable();
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_BONDING);              

            memset(bd_addr, 0, 6);
            if(!gapTester_get_bdaddr_from_string(bd_addr, string+4))
            {
                return;
            }
            CONVERT_ARRAY2BDADDR(&remote_addr, bd_addr);
            gapTesterBonding(remote_addr);
        }    
        else if(strncmp(string, "deletekey",9)==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_DELETEKEY);             
            DDB_FlushAllRecord();
        }    
        else if(strncmp(string, "createlink",10)==0)
        {
            U8 bd_addr[6];
            btbm_bd_addr_t remote_addr;

            gapTesterPairable();
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_CREATELINK);  
            memset(bd_addr, 0, 6);
            if(!gapTester_get_bdaddr_from_string(bd_addr, string+10))
            {
                return;
            }
            CONVERT_ARRAY2BDADDR(&remote_addr, bd_addr);
            gapTesterCreateAclLink(remote_addr);

        }    
        else if(strncmp(string, "createChannel",13)==0)
        {
            U8 bd_addr[6];
            btbm_bd_addr_t remote_addr;

            gapTesterPairable();            
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_GAPTS_CREATECHNL);  
            memset(bd_addr, 0, 6);
            if(!gapTester_get_bdaddr_from_string(bd_addr, string+13))
            {
                return;
            }
            CONVERT_ARRAY2BDADDR(&remote_addr, bd_addr);
            gapTesterCreateChannel(remote_addr);
        }    
        /*Add by mtk01411: 2007-0703*/
        else if(strncmp(string,"setcod",6) ==0)
        {
            bt_trace(TRACE_GROUP_1,GAPTESTER_SET_FILTER_CODBT_DEBUGx04X ,BT_DEFAULT_COD);
            gapTesterSetFilterCoD(BT_DEFAULT_COD);
        }
        /*Add by mtk01411: 2007-0703*/
        else if(strncmp(string,"reinitcod",9) ==0)
        {
            bt_trace(TRACE_GROUP_1,GAPTESTER_SET_FILTER_COD_TO_0XFFFFFFFF);
            gapTesterSetFilterCoD(0xFFFFFFFF);
        }
        /*Add by mtk01411: 2007-0704*/
        else if(strncmp(string,"inqnoname",9) ==0)
        {
            bt_trace(TRACE_GROUP_1,GAPTESTER_SET_DISCOVERY_WITHOUT_NAME_MODE);
            gaptester_discovery_ask_without_name = 1;
        
        }
        /*Add by mtk01411: 2007-0704*/
        else if(strncmp(string,"inqname",7) ==0)
        {
            bt_trace(TRACE_GROUP_1,GAPTESTER_SET_DISCOVERY_WITH_NAME_MODE);
            gaptester_discovery_ask_without_name = 0;
        
        }
        
    }
}


void gap_tester_handler_inject_msg(char *string)
{
    bt_test_cmd_struct *test_msg;

    test_msg = ( bt_test_cmd_struct*)
                           construct_local_para (sizeof(bt_test_cmd_struct),TD_UL);

    memcpy((U8 *)test_msg->string, (U8 *)string, strlen(string));
    gap_tester_sendMsg(
                            MSG_ID_BT_GAP_TEST_CMD_REQ,
                            MOD_BT,
                            BT_APP_SAP,
                            (local_para_struct*)test_msg,
                            NULL
                            );

    
}

BOOL gapTester_get_bdaddr_from_string(U8 *bd_addr, char *string)
{
    U8 i=0;
    for(i=0;i<12;i++)
    {
        if ((string[i] >='0') && (string[i] <='9'))
        {
        string[i] -= '0';
        }
        else if ((string[i] <='f') && (string[i] >='a'))
        {
        string[i] = string[i] - 'a'+10;
        }
        else if ((string[i] <='F') && (string[i] >='A'))
        {
        string[i] = string[i] - 'A'+10;
        }
        else
            return FALSE;
    }
    bd_addr[5] = (string[0]) << 4 | (string[1]);
    bd_addr[4] = (string[2]) << 4 | (string[3]);
    bd_addr[3] = (string[4]) << 4 | (string[5]);
    bd_addr[2] = (string[6]) << 4 | (string[7]);
    bd_addr[1] = (string[8]) << 4 | (string[9]);
    bd_addr[0] = (string[10]) << 4 | (string[11]);
    return TRUE;
}

#endif



