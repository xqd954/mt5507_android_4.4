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
#include "conmgr.h"
#include "btbm.h"
#include "btbm_adp.h"
#include "sdap.h"
#include "sdaptester.h"
#include "medev.h"
#include "l2captester.h"

#ifdef __SDAP_TESTER_
  
extern void CONVERT_BDADDR2ARRAY(U8 *dest, U32 lap, U8 uap, U16 nap);
extern void CONVERT_ARRAY2BDADDR(btbm_bd_addr_t *dest, U8 *src);

BtSdapTesterContext sdapTesterContext;

static U16 SDAP_SERVICE_SEARCH_UUID[6] = {0x1101, 0x1102, 0x1103, 0x1104, 0x1105, 0x1106 };

#define SDAPTS(s) (sdapTesterContext.s)

static void sdap_tester_sendMsg(
                                        msg_type msg, 
                                        module_type dstMod ,
                                        sap_type sap,
                                        local_para_struct *local_para,
                                        peer_buff_struct *peer_buff);

static void sdap_tester_sendMsg(
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

void sdapTesterInit(void)
{
    memset((U8 *)&sdapTesterContext, 0, sizeof(sdapTesterContext));
    bma_gap_testing(sdapTesterCallback);
}

void sdapTesterGeneralInquiry(void)
{
    bt_bm_discovery_req_struct ptr;

    ptr.access_mode = BTBM_GENERAL_INQUIRY;
    ptr.cod = 0xFFFFFFFF;
    ptr.discovery_mode = BTBM_DISCOVERY_WITHOUT_NAME;
    ptr.inquiry_length = 0x10;
    ptr.inquiry_number = 0x10;
    BTBMGapInquiry(&ptr);    
}

void sdapTesterLimitedInquiry(void)
{
    bt_bm_discovery_req_struct ptr;

    ptr.access_mode = BTBM_LIMITED_INQUIRY;
    ptr.cod = 0xFFFFFFFF;
    ptr.discovery_mode = BTBM_DISCOVERY_WITHOUT_NAME;
    ptr.inquiry_length = 0x10;
    ptr.inquiry_number = 0x10;
    BTBMGapInquiry(&ptr);    
}

void sdapTesterNameDiscovery(void)
{
    bt_bm_discovery_req_struct ptr;

    ptr.access_mode = BTBM_GENERAL_INQUIRY;
    ptr.cod = 0xFFFFFFFF;
    ptr.discovery_mode = BTBM_DISCOVERY_WITH_NAME;
    ptr.inquiry_length = 0x10;
    ptr.inquiry_number = 0x10;
    BTBMGapInquiry(&ptr);    
}

void sdapTesterCreateAclLink(btbm_bd_addr_t bd_addr)
{
    bt_bm_acl_link_req_struct ptr;
    ptr.bd_addr = bd_addr;
    BTBMGapLinkCreate(&ptr);
}


void sdapTesterCreateAclLinkWithPIN(btbm_bd_addr_t bd_addr)
{
    bt_bm_write_authentication_mode_req_struct ptr;
    bt_bm_acl_link_req_struct ptr1;
    
    ptr1.bd_addr = bd_addr;
    ptr.mode = BTBM_AUTHENTICATION_ENABLE;
    BTBMGapSetAuthenticationMode(TRUE, &ptr);      
    BTBMGapLinkCreate(&ptr1);
}

void sdapTesterCheckAclLinkState(btbm_bd_addr_t bd_addr)
{
    BtDeviceContext *bdc;   
    BD_ADDR addr;

    CONVERT_BDADDR2ARRAY((U8 *)addr.addr, bd_addr.lap, bd_addr.uap, bd_addr.nap);
    if ((bdc = DS_FindDevice(&addr)) !=0)
    {
        if (bdc->state == BDS_CONNECTED)
        {
        }
        else
        {
        }
    }
}


void sdapTesterDeviceList(btbm_bd_addr_t bd_addr)
{
    BtDeviceContext *bdc=0;   
    BD_ADDR *addr =0;

    /* Clear "In Range" status for all devices. */
    while (DS_EnumDeviceList(&bdc) == BT_STATUS_SUCCESS) 
    {
        if (bdc->state == BDS_CONNECTED)
        {
            addr = &bdc->addr;
        }
    }
}

void sdapTesterCreateChannel(btbm_bd_addr_t bd_addr)
{
    L2capConnectRequest ptr;

    CONVERT_BDADDR2ARRAY(ptr.bd_addr, bd_addr.lap, bd_addr.uap, bd_addr.nap);
    l2capTesterInit();
    L2CA_ConnectReq(&ptr);
}

void sdapTesterCloseChannel(btbm_bd_addr_t bd_addr)
{
    bt_bm_acl_link_close_struct ptr;

    ptr.bd_addr = bd_addr;
    BTBMGapLinkClose(&ptr);
}

void sdapTesterServiceSearchUnConnected(btbm_bd_addr_t bd_addr)
{
    BtStatus status;
    
    /* Establish the ACL link */
    SDAPTS(search_type) = SDAP_TYPE_SERVICE_SEARCH;
    CONVERT_BDADDR2ARRAY(SDAPTS(addr).addr, bd_addr.lap, bd_addr.uap, bd_addr.nap);
    (void)CMGR_RegisterHandler(&SDAPTS(cmgrHandler), sdapTesterCmgrCallback);
    status = CMGR_CreateDataLink(&SDAPTS(cmgrHandler), &SDAPTS(addr));
    if (status == BT_STATUS_SUCCESS) 
    {
        sdapTesterServiceSearchConnected(bd_addr);
    }
}

void sdapTesterServiceSearchConnectedAttributeUnConnected(btbm_bd_addr_t bd_addr)
{
    BtStatus status = BT_STATUS_SUCCESS;
     
    /* Establish the ACL link */
    SDAPTS(search_type) = SDAP_TYPE_SERVICE_SEARCH_ATTRIBUTE;
    CONVERT_BDADDR2ARRAY(SDAPTS(addr).addr, bd_addr.lap, bd_addr.uap, bd_addr.nap);
    (void)CMGR_RegisterHandler(&SDAPTS(cmgrHandler), sdapTesterCmgrCallback);
    status = CMGR_CreateDataLink(&SDAPTS(cmgrHandler), &SDAPTS(addr));
    if (status == BT_STATUS_SUCCESS) 
    {
        sdapTesterServiceSearchConnectedAttributeConnected(bd_addr);
        return;
    }
}

void sdapTesterServiceSearchConnected(btbm_bd_addr_t bd_addr)
{
    bt_bm_service_search_req_struct ptr;

    ptr.bd_addr = bd_addr;
    ptr.sdap_len = 5;    
    ptr.sdap_uuid[0] = 0x1101;
    ptr.sdap_uuid[1] = 0x1102;
    ptr.sdap_uuid[2] = 0x1103;    
    ptr.sdap_uuid[3] = 0x1104;
    ptr.sdap_uuid[4] = 0x1105;    
    SDAPTS(search_type) = SDAP_TYPE_SERVICE_SEARCH;
    BTBMSdapSearchallService(&ptr);
}


void sdapTesterServiceSearchConnectedAttributeConnected(btbm_bd_addr_t bd_addr)
{
    BtStatus status = BT_STATUS_FAILED;
    BtDeviceContext *bdc;   
    sdap_service_search_struct search_pattern;
    
    SDAPTS(search_stage) = 0;
    SDAPTS(search_type) = SDAP_TYPE_SERVICE_SEARCH_ATTRIBUTE;
    CONVERT_BDADDR2ARRAY(SDAPTS(addr).addr, bd_addr.lap, bd_addr.uap, bd_addr.nap);
    if ((bdc = DS_FindDevice(&SDAPTS(addr))) !=0)
    {
        search_pattern.rm = bdc->link;
        search_pattern.sqt = &SDAPTS(sdpQueryToken);
        search_pattern.uuid = SDAP_SERVICE_SEARCH_UUID[SDAPTS(search_stage)];    
        search_pattern.callback = sdapTesterQueryCallback;    
        status = SDAP_ServiceSearch(search_pattern);
    }
    return;
} 

void sdapTesterServiceSearchConnectedUtility(void)
{
    BtDeviceContext *bdc;
    sdap_service_search_struct search_pattern;
    switch (SDAPTS(search_type))
    {
        case SDAP_TYPE_SERVICE_SEARCH_ATTRIBUTE:
        if (SDAPTS(search_stage) < (sizeof (SDAP_SERVICE_SEARCH_UUID)/2))
        {
            if ((bdc = DS_FindDevice(&SDAPTS(addr))) !=0)
            {
                SDAPTS(search_stage)++;
                search_pattern.rm = bdc->link;
                search_pattern.sqt = &SDAPTS(sdpQueryToken);
                search_pattern.uuid = SDAP_SERVICE_SEARCH_UUID[SDAPTS(search_stage)];    
                search_pattern.callback = sdapTesterQueryCallback;    
                SDAP_ServiceSearch(search_pattern);
            }
        }
        break;
    }
}

void sdapTesterQueryCallback(
                        SdpQueryToken *sqt, U8 result, U8 serv_chn)
{
    switch (result) 
    {
        case BT_STATUS_SUCCESS:
            switch (SDAPTS(search_type))
            {
                case SDAP_TYPE_SERVICE_SEARCH_ATTRIBUTE:
                break;
            }
            sdapTesterServiceSearchConnectedUtility();
            break;
        case BT_STATUS_FAILED:
            break;
        case BT_STATUS_NOSERVICES:
            switch (SDAPTS(search_type))
            {
                case SDAP_TYPE_SERVICE_SEARCH:
                break;
            }
            sdapTesterServiceSearchConnectedUtility();
            break;            
    }
}

void sdapTesterCmgrCallback(CmgrHandler *cHandler, 
                      CmgrEvent Event, 
                      BtStatus Status)
{
    btbm_bd_addr_t bd_addr;
    
    switch (Event) {

    case CMEVENT_DATA_LINK_CON_CNF:
        if (Status == BT_STATUS_SUCCESS) 
        {
            CONVERT_ARRAY2BDADDR(&bd_addr, cHandler->bdc->addr.addr);
            switch(SDAPTS(search_type))
            {
                case SDAP_TYPE_SERVICE_SEARCH_ATTRIBUTE:
                    sdapTesterServiceSearchConnectedAttributeConnected(bd_addr);
                    break;
                case SDAP_TYPE_SERVICE_SEARCH:
                    sdapTesterServiceSearchConnected(bd_addr);
                    break;
            }
            return;
        }
        /* Disconnect and call the application */
        (void)CMGR_RemoveDataLink(&SDAPTS(cmgrHandler));
        (void)CMGR_DeregisterHandler(&SDAPTS(cmgrHandler));
        break;
    }
}


void sdapTesterCallback(ilm_struct *ilm_ptr)
{
    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_BM_DISCOVERY_RESULT_IND:
            {
                bt_bm_discovery_result_ind_struct *msg = (bt_bm_discovery_result_ind_struct *)ilm_ptr->local_para_ptr;
            }
            break;
        case MSG_ID_BT_BM_DISCOVERY_CNF:
            {
                bt_bm_discovery_cnf_struct *msg = (bt_bm_discovery_cnf_struct *)ilm_ptr->local_para_ptr;
            }
            break;
        case MSG_ID_BT_BM_DISCOVERY_CANCEL_CNF:
            {
                bt_bm_discovery_cancel_cnf_struct *msg = (bt_bm_discovery_cancel_cnf_struct *)ilm_ptr->local_para_ptr;
            }
            break;
        case MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF:
            {
                bt_bm_write_local_name_cnf_struct *msg = (bt_bm_write_local_name_cnf_struct *)ilm_ptr->local_para_ptr;
            }
            break;
        case MSG_ID_BT_BM_READ_REMOTE_NAME_CNF:
            {
                bt_bm_read_remote_name_cnf_struct *msg = (bt_bm_read_remote_name_cnf_struct *)ilm_ptr->local_para_ptr;
            }
            break;
        case MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF:
            {
                bt_bm_write_scanenable_mode_cnf_struct *msg = (bt_bm_write_scanenable_mode_cnf_struct *)ilm_ptr->local_para_ptr;
            }            
            break;
        case MSG_ID_BT_BM_READ_SCANENABLE_MODE_CNF:
            {
                bt_bm_read_scanenable_mode_cnf_struct *msg = (bt_bm_read_scanenable_mode_cnf_struct *)ilm_ptr->local_para_ptr;
            }                     
            break;
        case MSG_ID_BT_BM_READ_LOCAL_ADDR_CNF:
            {
                bt_bm_read_local_addr_cnf_struct *msg = (bt_bm_read_local_addr_cnf_struct *)ilm_ptr->local_para_ptr;
            }                     
            break;
        case MSG_ID_BT_BM_READ_LOCAL_COD_CNF:
            {
                bt_bm_read_local_cod_cnf_struct *msg = (bt_bm_read_local_cod_cnf_struct *)ilm_ptr->local_para_ptr;
            }                
            break;
        case MSG_ID_BT_BM_READ_PROPERTY_CNF:
            {
                bt_bm_read_property_cnf_struct *msg = (bt_bm_read_property_cnf_struct *)ilm_ptr->local_para_ptr;
             }                
            break;
        case MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_CNF:
            {
                bt_bm_write_authentication_mode_cnf_struct *msg = (bt_bm_write_authentication_mode_cnf_struct *)ilm_ptr->local_para_ptr;
            }                
            break;
        case MSG_ID_BT_BM_PIN_CODE_IND:
            {
                bt_bm_pin_code_ind_struct *msg = (bt_bm_pin_code_ind_struct *)ilm_ptr->local_para_ptr;
                {
                    bt_bm_pin_code_rsp_struct ptr;
                    ptr.bd_addr.lap = msg->bd_addr.lap;
                    ptr.bd_addr.nap = msg->bd_addr.nap;
                    ptr.bd_addr.uap = msg->bd_addr.uap;
                    ptr.pin_code[0] = '0';
                    ptr.pin_code[1] = '0';        
                    ptr.pin_code[2] = '0';
                    ptr.pin_code[3] = '0';        
                    ptr.pin_len= 0x04;
                    BTBMGapWritePINCode(&ptr);
                }
            }                
            break;
        case MSG_ID_BT_BM_BONDING_CNF:
            {
                bt_bm_bonding_cnf_struct *msg = (bt_bm_bonding_cnf_struct *)ilm_ptr->local_para_ptr;
            }                  
            break;
        case MSG_ID_BT_BM_BONDING_RESULT_IND:
            {
                bt_bm_bonding_result_ind_struct *msg = (bt_bm_bonding_result_ind_struct *)ilm_ptr->local_para_ptr;
            }                
            break;
        case MSG_ID_BT_BM_SAVE_SETTING_CNF:
            {
                bt_bm_save_setting_cnf_struct *msg = (bt_bm_save_setting_cnf_struct *)ilm_ptr->local_para_ptr;
            }                 
            break;
        case MSG_ID_BT_BM_RESTORE_SETTING_CNF:
            {
                bt_bm_restore_setting_cnf_struct *msg = (bt_bm_restore_setting_cnf_struct *)ilm_ptr->local_para_ptr;
            }             
            break;
        case MSG_ID_BT_BM_DELETE_TRUST_CNF:
            {
                bt_bm_delete_trust_cnf_struct *msg = (bt_bm_delete_trust_cnf_struct *)ilm_ptr->local_para_ptr;
            }                  
            break;            
        case MSG_ID_BT_BM_BONDING_CANCEL_CNF:
            {
                bt_bm_bonding_cancel_cnf_struct *msg = (bt_bm_bonding_cancel_cnf_struct*)ilm_ptr->local_para_ptr;
            }                  
            break;
        case MSG_ID_BT_BM_PAIRING_CANCEL_CNF:
            {
                bt_bm_pairing_cancel_cnf_struct *msg = (bt_bm_pairing_cancel_cnf_struct*)ilm_ptr->local_para_ptr;
            }                  
            break;

        case MSG_ID_BT_BM_SERVICE_SEARCH_RESULT_IND:
            {
                bt_bm_service_search_result_ind_struct *msg = (bt_bm_service_search_result_ind_struct *)ilm_ptr->local_para_ptr;
            }             
            break;
        case MSG_ID_BT_BM_SERVICE_SEARCH_CNF:
            {
                bt_bm_service_search_cnf_struct *msg = (bt_bm_service_search_cnf_struct *)ilm_ptr->local_para_ptr;
            }             
            break;
    }
}



void sdaptester_handler(ilm_struct *ilm_ptr)
{
}


void sdap_tester_handler_inject_msg(char *string)
{
}


#endif



