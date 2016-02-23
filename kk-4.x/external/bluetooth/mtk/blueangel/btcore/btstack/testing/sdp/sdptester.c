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
#include "bluetooth_gap_struct.h"
#include "btconfig.h"
#include "sdptester.h"
#include "osapi.h"
#include "l2cap.h"
#include "sdp.h"
#include "sdpi.h"

#ifdef __SDP_TESTER_    

BtSdpTesterContext sdpTesterContext;

#define SDPPTS(s) (sdpTesterContext.s)

const U8 SDPTesterProtoDescList1[] = {
    SDP_ATTRIB_HEADER_8BIT(12), 
    SDP_ATTRIB_HEADER_8BIT(3),   
    SDP_UUID_16BIT(PROT_L2CAP),  
    SDP_ATTRIB_HEADER_8BIT(5),   
    SDP_UUID_16BIT(PROT_RFCOMM), 
    SDP_UINT_8BIT(1)
};

const U8 SDPTesterProtoDescList2[] = {
    SDP_ATTRIB_HEADER_8BIT(12), 
    SDP_ATTRIB_HEADER_8BIT(3),   
    SDP_UUID_16BIT(PROT_L2CAP),  
    SDP_ATTRIB_HEADER_8BIT(5),   
    SDP_UUID_16BIT(PROT_RFCOMM), 
    SDP_UINT_8BIT(RFCOMMTESTER_RFCOMM_SRV_CHN)
};

const U8 SDPTesterClassId1[] = {
    SDP_ATTRIB_HEADER_8BIT(3),  
    SDP_UUID_16BIT(0x1101) 
};


const U8 SDPTesterClassId2[] = {
    SDP_ATTRIB_HEADER_8BIT(3),  
    SDP_UUID_16BIT(0x1102) 
};

const U8 SDPTesterServiceId1[] = {
    SDP_UUID_16BIT(0x1101) 
};


const U8 SDPTesterServiceId2[] = {
    SDP_UUID_16BIT(0x1102) 
};


const U8 SDPTesterServiceInfoTimeToLive[] = {
    SDP_UINT_32BIT(0xFFFFFFFF)
};

const U8 SDPTesterServiceAvailable[] = {
    SDP_UINT_8BIT(0xFF)
};

const U8 SDPTesterLangBaseIdList[] = {
    SDP_ATTRIB_HEADER_8BIT(9), 
    SDP_UINT_16BIT(0x656E),    
    SDP_UINT_16BIT(0x006A),    
    SDP_UINT_16BIT(0x0100)     
};


const U8 SDPTesterServiceName[] = {
    SDP_TEXT_8BIT(13),          
    'M', 'e', 'd', 'i', 'a', 't', 'e', 
    'k', ' ', 't', 'e', 's', 't'
};

const U8 SDPTesterServiceDescription[] = {
    SDP_TEXT_8BIT(13),          
    'M', 'e', 'd', 'i', 'a', 't', 'e', 
    'k', ' ', 't', 'e', 's', 't'
};

const U8 SDPTesterServiceProvidername[] = {
    SDP_TEXT_8BIT(3),          
    'M', 'T', 'K'
};

const U8 SDPTesterBrowseGroup[] = {
    SDP_ATTRIB_HEADER_8BIT(3),         
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP)
};

const U8 SDPTesterIconURL[] = {
        SDP_URL_8BIT(29),
	'h',  't', 't', 'p', ':', '/', '/',
	'u',  'r', 'l', '.', '3', '2', 'x',
	'3',  '2', 'x', '8', '.', 'p', 'n',
	'g',  '/', 'i', 'c', 'o', 'n', '/',
	'*'
};

const U8 SDPTesterDocURL[] = {
    SDP_URL_8BIT(23),          
    'h', 't', 't', 'p', ':', '/', '/',
    'm', 'y', 'f', 'a', 'k', 'e', '3',
    '/', 't', 'e', 's', 't', '.', 'd',
    'o', 'c'
};

const U8 SDPTesterExecURL[] = {
    SDP_URL_8BIT(15),          
    'h', 't', 't', 'p', ':', '/', '/',
    'm', 'y', 'f', 'a', 'k', 'e', '2',
    '/', 
};

const U8 SDPTesterVersionNumberList[] = {
    SDP_ATTRIB_HEADER_8BIT(3), 
    SDP_UINT_16BIT(0x0100),    
};

const U8 SDPTesterServiceDatabaseState[] = {
    SDP_UINT_32BIT(0xFFFFFFFF),    
};

const U8 SDPTesterProfileDescList1[] = {
    SDP_ATTRIB_HEADER_8BIT(6),  
    SDP_UUID_16BIT(SC_SERIAL_PORT),
    SDP_UINT_16BIT(0x0100)
};

const U8 SDPTesterProfileDescList2[] = {
    SDP_ATTRIB_HEADER_8BIT(6),  
    SDP_UUID_16BIT(SC_SERIAL_PORT),
    SDP_UINT_16BIT(0x0100)
};


SdpAttribute SdpTesterAttributes1[] = {
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, SDPTesterClassId1), 
    SDP_ATTRIBUTE(AID_SERVICE_ID, SDPTesterServiceId1),
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, SDPTesterProtoDescList1),
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, SDPTesterBrowseGroup), 
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, SDPTesterLangBaseIdList),
    SDP_ATTRIBUTE(AID_SERVICE_INFO_TIME_TO_LIVE, SDPTesterServiceInfoTimeToLive),    
    SDP_ATTRIBUTE((AID_SERVICE_AVAILABILITY), SDPTesterServiceAvailable),
    SDP_ATTRIBUTE((AID_BT_PROFILE_DESC_LIST), SDPTesterProfileDescList1),             
    SDP_ATTRIBUTE((AID_DOC_URL), SDPTesterDocURL), 
    SDP_ATTRIBUTE((AID_CLIENT_EXEC_URL), SDPTesterExecURL),
    SDP_ATTRIBUTE((AID_ICON_URL), SDPTesterIconURL), 
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), SDPTesterServiceName),
    SDP_ATTRIBUTE((AID_SERVICE_DESCRIPTION + 0x0100), SDPTesterServiceDescription),    
    SDP_ATTRIBUTE((AID_PROVIDER_NAME + 0x0100), SDPTesterServiceProvidername),        
    SDP_ATTRIBUTE((AID_VERSION_NUMBER_LIST), SDPTesterVersionNumberList),     
    SDP_ATTRIBUTE((AID_SERVICE_DATABASE_STATE), SDPTesterServiceDatabaseState)         
};

SdpAttribute SdpTesterAttributes2[] = {
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, SDPTesterClassId2), 
    SDP_ATTRIBUTE(AID_SERVICE_ID, SDPTesterServiceId2),
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, SDPTesterProtoDescList2),
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, SDPTesterBrowseGroup), 
    SDP_ATTRIBUTE(AID_LANG_BASE_ID_LIST, SDPTesterLangBaseIdList),
    SDP_ATTRIBUTE(AID_SERVICE_INFO_TIME_TO_LIVE, SDPTesterServiceInfoTimeToLive),    
    SDP_ATTRIBUTE((AID_SERVICE_AVAILABILITY), SDPTesterServiceAvailable),
    SDP_ATTRIBUTE((AID_BT_PROFILE_DESC_LIST), SDPTesterProfileDescList2),             
    SDP_ATTRIBUTE((AID_DOC_URL), SDPTesterDocURL), 
    SDP_ATTRIBUTE((AID_CLIENT_EXEC_URL), SDPTesterExecURL),
    SDP_ATTRIBUTE((AID_ICON_URL), SDPTesterIconURL), 
    SDP_ATTRIBUTE((AID_SERVICE_NAME + 0x0100), SDPTesterServiceName),
    SDP_ATTRIBUTE((AID_SERVICE_DESCRIPTION + 0x0100), SDPTesterServiceDescription),    
    SDP_ATTRIBUTE((AID_PROVIDER_NAME + 0x0100), SDPTesterServiceProvidername),        
    SDP_ATTRIBUTE((AID_VERSION_NUMBER_LIST), SDPTesterVersionNumberList),     
    SDP_ATTRIBUTE((AID_SERVICE_DATABASE_STATE), SDPTesterServiceDatabaseState)        
};

static SdpRecord SDP_tester_record1;
static SdpRecord SDP_tester_record2;


static void sdp_tester_sendMsg(
                                        msg_type msg, 
                                        module_type dstMod ,
                                        sap_type sap,
                                        local_para_struct *local_para,
                                        peer_buff_struct *peer_buff);

static void sdp_tester_sendMsg(
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

void SdpTesterInit()
{
    OS_MemSet((U8 *)&SDP_tester_record1, 0, sizeof(SdpRecord));
    OS_MemSet((U8 *)&SDP_tester_record2, 0, sizeof(SdpRecord));
    SDP_Deinit();
    SDP_Init();
    SDPPTS(special_testing_enabled) = FALSE;
    SDPPTS(testing_enabled) = TRUE;    
    SDP_tester_record1.attribs = (SdpAttribute *)&SdpTesterAttributes1;
    SDP_tester_record1.num =   sizeof(SdpTesterAttributes1) / 
                                            sizeof(*(SdpTesterAttributes1));
    SDP_AddRecord(&SDP_tester_record1);   
    SDP_tester_record1.recordState = 0xAABB;
}

void SdpRFCOMMTesterInit()
{
    OS_MemSet((U8 *)&SDP_tester_record1, 0, sizeof(SdpRecord));
    OS_MemSet((U8 *)&SDP_tester_record2, 0, sizeof(SdpRecord));
    SDP_Deinit();
    SDP_Init();
    SDPPTS(special_testing_enabled) = FALSE;
    SDPPTS(testing_enabled) = TRUE;    
    SDP_tester_record2.attribs = (SdpAttribute *)&SdpTesterAttributes2;
    SDP_tester_record2.num =   sizeof(SdpTesterAttributes2) / 
                                            sizeof(*(SdpTesterAttributes2));
    SDP_AddRecord(&SDP_tester_record2);   
    SDP_tester_record1.recordState = 0xAABB;
}

void SdpTesterSpecialTesting(BOOL enabled)
{
    if(enabled)
        SDPPTS(special_testing_enabled) = TRUE;
    else
        SDPPTS(special_testing_enabled) = FALSE;
}
void sdptester_handler(ilm_struct *ilm_ptr)
{
    char *string;
    bt_test_cmd_struct *test_msg;

    if (ilm_ptr->msg_id == MSG_ID_BT_SDP_TEST_ACTIVATE_REQ)
    {
        test_msg = ((bt_test_cmd_struct *)ilm_ptr->local_para_ptr);    
        string = (char *)test_msg->string;    
        if(strncmp(string, "init",4)==0)
        {
            SdpTesterInit();
        }
        else if(strncmp(string, "special",7)==0)
        {
            SdpTesterSpecialTesting(TRUE);
        }
        else if(strncmp(string, "normal",6)==0)
        {
            SdpTesterSpecialTesting(FALSE);
        }            
    }
}

void sdp_tester_handler_inject_msg(char *string)
{
    bt_test_cmd_struct *test_msg;

    test_msg = ( bt_test_cmd_struct*)
    	                       construct_local_para (sizeof(bt_test_cmd_struct),TD_UL);

    memcpy((U8 *)test_msg->string, (U8 *)string, strlen(string));
    sdp_tester_sendMsg(
                            MSG_ID_BT_SDP_TEST_ACTIVATE_REQ,
                            MOD_BT,
                            BT_APP_SAP,
                            (local_para_struct*)test_msg,
                            NULL
                            );    
}
#endif /* __SDP_TESTER_ */

BOOL sdp_query_testing_state(void)
{
    return TRUE;
}

