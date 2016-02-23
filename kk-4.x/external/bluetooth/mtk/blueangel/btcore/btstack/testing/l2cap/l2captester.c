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
#include "bluetooth_gap_struct.h"
#include "sys/avalloc.h"
#include "sys/avsigmgr.h"
#include "l2captester.h"
#include "osapi.h"


//add by stanley 2006-07-21: if the bluetooth_struct.h is included in your C file, also the uart_sw.h must be included. 

#include "bluetooth_struct.h"
#include "bt_test_cmd.h"
#include "bt_feature.h"
#include "l2cap_i.h"

#ifdef __L2CAP_CONFIG_DUMP__
/* Add for l2cap streaming mode test */

#endif

#ifdef __L2CAP_TESTER_
                                  
BtL2capTesterContext l2capTesterContext;

#define L2CAPTS(s) (l2capTesterContext.s)

#ifdef __GEMINI__
extern module_type Get_DualSim_Uart_Default_Owner(void);
#endif

void l2capTesterL2Callback(L2capChannelId L2ChannelId, L2capCallbackParms *info);
void l2capTesterCmgrCallback(CmgrHandler *cHandler, 
                      CmgrEvent Event, 
                      BtStatus Status);

#if L2CAP_NUM_GROUPS > 0
void l2capTesterGroupCmgrCallback(CmgrHandler *cHandler, 
                      CmgrEvent Event, 
                      BtStatus Status);
static void L2Group_Event(U16 ChannelId, L2capCallbackParms *Info);

#endif

static void l2cap_tester_sendMsg(
                                        msg_type msg, 
                                        module_type dstMod ,
                                        sap_type sap,
                                        local_para_struct *local_para,
                                        peer_buff_struct *peer_buff);

static void l2cap_tester_sendMsg(
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

void l2capTesterInit(void)
{
    if(L2CAPTS(testing_enabled) == TRUE)
    {
        Report(("L2CAP already init"));
        return;
    }
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_L2CAPTS_INIT);
    L2CAPTS(Protocol).callback = l2capTesterL2Callback;
    L2CAPTS(Protocol).psm = BT_PSM_TESTING;
    L2CAPTS(Protocol).localMtu = (L2CAP_MTU - 5);
    L2CAPTS(Protocol).minRemoteMtu = 23 + 5;
    L2CAPTS(Protocol).authorizedSetting = FALSE;    
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    L2CAPTS(Protocol).inLinkMode = L2CAPTS(Protocol).outLinkMode = L2MODE_BASIC;
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */
    L2CAPTS(testing_enabled) = TRUE;
    if (L2CAP_RegisterPsm(&(L2CAPTS(Protocol))) != BT_STATUS_SUCCESS) {
        /* Unable to register */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_L2CAPTS_UNABLEREG);
    }

#if L2CAP_NUM_GROUPS > 0
    L2CAPTS(tsgroup).psm = BT_PSM_TCS_CORDLESS;
    L2CAPTS(tsgroup).callback = L2Group_Event;
    L2CAPTS(tsgroup).maxMembers = 2;
    L2CAP_RegisterGroup(&L2CAPTS(tsgroup), &L2CAPTS(groupId));
#endif
}

void L2capTesterSetSecurity(U8 level)
{
#ifdef __BT_2_1_SIMPLE_PAIRING__ 
    BtStatus status;

    if (IsListEmpty(&(L2CAPTS(secRecord).node)))
    {
        Report(("L2cap tester list empty"));
        InitializeListEntry(&(L2CAPTS(secRecord).node));
    }
    else
    {
        Report(("L2cap tester list not empty"));
        SEC_Unregister(&L2CAPTS(secRecord));
        InitializeListEntry(&(L2CAPTS(secRecord).node));
    }
    L2CAPTS(secRecord).id =  SEC_L2CAP_ID;
    L2CAPTS(secRecord).channel = BT_PSM_TESTING;
    L2CAPTS(secRecord).level = BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT;
    switch(level)
    {
        case BT_SEC_MED:
            L2CAPTS(secRecord).level_type = BT_SEC_MED;
            break;
        case BT_SEC_HIGH:
            L2CAPTS(secRecord).level_type = BT_SEC_HIGH;
            break;
        case BT_SEC_LOW:
            L2CAPTS(secRecord).level_type = BT_SEC_LOW;
            break;
    }
    status = SEC_RegisterRecord(&L2CAPTS(secRecord));
    Report(("SEC_RegisterRecord Status:%02x",status));
#endif

}
U8 L2CA_ConnectReq(L2capConnectRequest *parms)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BD_ADDR bd_addr;
    U16 l2ChannelId;

    /* Establish the ACL link */
    memcpy((U8 *)bd_addr.addr, (U8 *)parms->bd_addr,6);

    (void)CMGR_RegisterHandler(&L2CAPTS(cmgrHandler), l2capTesterCmgrCallback);
    status = CMGR_CreateDataLink(&L2CAPTS(cmgrHandler), &bd_addr);
    L2CAPTS(pending_command) = L2CAP_TESTER_CONNECT_REQ;
    if (status == BT_STATUS_SUCCESS) 
    {
        status = L2CAP_ConnectReq(&L2CAPTS(Protocol), BT_PSM_TESTING, L2CAPTS(cmgrHandler).remDev, 0, &l2ChannelId);
    }
    return status;
}

U8 L2CA_DisconnectReq(L2capDisconnectRequest *parms)
{
    BtStatus status = BT_STATUS_SUCCESS;
    
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_L2CAPTS_DISCONNECTREQ);
    status = L2CAP_DisconnectReq(parms->cid);
    return status;
}

U8 L2CA_EchoReq(L2capEchoRequest *parms)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BD_ADDR bd_addr;
    

    memset( (U8 *)L2CAPTS(common_data), 0x55, sizeof(L2CAPTS(common_data)));
    /* Establish the ACL link */
    memcpy((U8 *)bd_addr.addr, (U8 *)parms->bd_addr,6);

    (void)CMGR_RegisterHandler(&L2CAPTS(cmgrHandler), l2capTesterCmgrCallback);
    status = CMGR_CreateDataLink(&L2CAPTS(cmgrHandler), &bd_addr);
    L2CAPTS(pending_command) = L2CAP_TESTER_PING_REQ;

    if (status == BT_STATUS_SUCCESS) 
    {
        memset( (U8 *)L2CAPTS(common_data), 0x55, sizeof(L2CAPTS(common_data)));
        status = L2CAP_Ping(
                                &(L2CAPTS(Protocol)), 
                                L2CAPTS(cmgrHandler).remDev, 
                                (U8 *)L2CAPTS(common_data),
                                sizeof(L2CAPTS(common_data)));   
    }
    return status;
}

U8 L2CA_InfoReq(L2capInfoRequest *parms)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BD_ADDR bd_addr;
    

    memcpy((U8 *)bd_addr.addr, (U8 *)parms->bd_addr,6);
    (void)CMGR_RegisterHandler(&L2CAPTS(cmgrHandler), l2capTesterCmgrCallback);
    status = CMGR_CreateDataLink(&L2CAPTS(cmgrHandler), &bd_addr);
    L2CAPTS(pending_command) = L2CAP_TESTER_INFO_REQ;
    L2CAPTS(info_type) = parms->info_type;

    if (status == BT_STATUS_SUCCESS) 
    {
        status = L2CAP_GetInfo(&(L2CAPTS(Protocol)), L2CAPTS(cmgrHandler).remDev, L2CAPTS(info_type)); 
    }
    return status;
}

U8 L2capSendData(void)
{
    BtStatus status;
    
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_L2CAPTS_SENDDATAREQ);
    L2CAPTS(btp).flags = BTP_FLAG_INUSE;
    L2CAPTS(btp).headerLen = 0;
    memset( (U8 *)L2CAPTS(common_data), 0x55, sizeof(L2CAPTS(common_data)));
    
    L2CAPTS(btp).data = (U8 *)L2CAPTS(common_data);
    L2CAPTS(btp).dataLen = sizeof(L2CAPTS(common_data));
    L2CAPTS(btp).priority_type = BTP_FLAG_HIGH_PRIPORTY ;    
    status = L2CAP_SendData(L2CAPTS(cid), &L2CAPTS(btp));
    return status;
}

void l2capTesterCmgrCallback(CmgrHandler *cHandler, 
                      CmgrEvent Event, 
                      BtStatus Status)
{
    L2capChannelId l2ChannelId;
    BtStatus status;
    
    //add by stanley:2006-0721
    local_para_struct* msg;
    bt_test_cmd_rsp_struct  *param_ptr;
    
    switch (Event) {

    case CMEVENT_DATA_LINK_CON_CNF:
        if (Status == BT_STATUS_SUCCESS) 
        {
            /* ACL is connected, start SDP Query */
            switch(L2CAPTS(pending_command))
            {
                case L2CAP_TESTER_CONNECT_REQ:
                    cHandler->remDev->discFlag = FALSE;
                    status = L2CAP_ConnectReq(&L2CAPTS(Protocol), BT_PSM_TESTING, cHandler->remDev, 0, &l2ChannelId);
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_L2CAPTS_CONNECTREQ,l2ChannelId);                    
                    L2CAPTS(cid) = l2ChannelId;

                    //add by stanley:2006-0721
   
		     param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  

                    memset(param_ptr->test_cmd_rsp_buf,0,256);

                    param_ptr->test_cmd_rsp_buf[0]=0x03; //logs type
                    param_ptr->test_cmd_rsp_buf[1]=0x02; //L2cap category
    
                    kal_sprintf((kal_char*)(param_ptr->test_cmd_rsp_buf+4),"L2CA_ConnectReq,using cid=%d",l2ChannelId);
                    Report(("str data send to L4C:%s",param_ptr->test_cmd_rsp_buf+4));
                    param_ptr->length= strlen((kal_char*)param_ptr->test_cmd_rsp_buf+4);
                    bt_trace(TRACE_GROUP_1,STR_DATA_LENxD ,param_ptr->length);

                    //fill the data length field of string data for rsp
                    param_ptr->test_cmd_rsp_buf[2]= (U8)(param_ptr->length  & 0x00ff);
                    param_ptr->test_cmd_rsp_buf[3]= (U8)((param_ptr->length >> 8) & 0x00ff);

                    //rsp_buf's total length = param_ptr->length + 4 bytes header
                    param_ptr->length += 4;
    
                    msg = (local_para_struct*)param_ptr;

                    bt_trace(TRACE_GROUP_1,CALL_BTTESTER_SENDMSG);
    
#if 0    
                    btTester_sendMsg(MSG_ID_BT_L4C_TEST_CMD_CNF, 
#ifdef __GEMINI__
                                                    Get_DualSim_Uart_Default_Owner(),
#else
                                                    MOD_ATCI, 
#endif 
                                                    INVALID_SAP, 
                                                    msg, 
                                                    NULL);
#endif                                                    
                    
                    return;
                case L2CAP_TESTER_PING_REQ:
                    memset( (U8 *)L2CAPTS(common_data), 0x55, sizeof(L2CAPTS(common_data)));
                    status = L2CAP_Ping(
                                            &(L2CAPTS(Protocol)), 
                                            L2CAPTS(cmgrHandler).remDev, 
                                            (U8 *)L2CAPTS(common_data),
                                            sizeof(L2CAPTS(common_data)));
                    return;
                case L2CAP_TESTER_INFO_REQ:
                    status = L2CAP_GetInfo(&(L2CAPTS(Protocol)), L2CAPTS(cmgrHandler).remDev, L2CAPTS(info_type));
                    return;
                default:
                    status = BT_STATUS_FAILED;
                    break;
            }
            if (Status == BT_STATUS_PENDING) 
            {
                return;
            }
        }
        /* Disconnect and call the application */
        (void)CMGR_RemoveDataLink(&L2CAPTS(cmgrHandler));
        (void)CMGR_DeregisterHandler(&L2CAPTS(cmgrHandler));
        break;
    }
}

void l2captester_handler(ilm_struct *ilm_ptr)
{
    char *string;
    bt_test_cmd_struct *test_msg;
		if (ilm_ptr->msg_id == MSG_ID_BT_TEST_CMD_REQ)    	
    {
        test_msg = ((bt_test_cmd_struct *)ilm_ptr->local_para_ptr);    
        string = (char *)test_msg->string;    
        if(strncmp(string, "init",4)==0)
        {
            l2capTesterInit();
        }
        else if(strncmp(string, "senddata",8)==0)
        {
            L2capSendData();
        }        
        else if(strncmp(string, "conn",4)==0)
        {
            U8 bd_addr[6];
            L2capConnectRequest parms;
            
            memset(bd_addr, 0, 6);
            if(!l2capTester_get_bdaddr_from_string(bd_addr, string+4))
            {
                return;
            }    		
            memcpy((U8 *)parms.bd_addr, (U8 *)bd_addr, 6);
            L2CA_ConnectReq(&parms);
        }    
        else if(strncmp(string, "disconn",7)==0)
        {
            L2capDisconnectRequest parms;
            parms.cid = L2CAPTS(cid);
            L2CA_DisconnectReq(&parms);
        }     
        else if(strncmp(string, "info",4)==0)
        {
            L2capInfoRequest parms;
            U8 bd_addr[6];
            
            memset(bd_addr, 0, 6);
            if(!l2capTester_get_bdaddr_from_string(bd_addr, string+4))
            {
                return;
            }    		
            memcpy((U8 *)parms.bd_addr, (U8 *)bd_addr, 6);
            parms.info_type = 0x0002;
            L2CA_InfoReq(&parms);
        }     
        else if(strncmp(string, "echo",4)==0)
        {
            L2capEchoRequest parms;
            U8 bd_addr[6];
            
            memset(bd_addr, 0, 6);
            if(!l2capTester_get_bdaddr_from_string(bd_addr, string+4))
            {
                return;
            }    		
            memcpy((U8 *)parms.bd_addr, (U8 *)bd_addr, 6);
            L2CA_EchoReq(&parms);
        }     
        else if(strncmp(string, "encry",5)==0)
        {
            bt_bm_write_authentication_mode_req_struct ptr;

            ptr.mode = BTBM_AUTHENTICATION_ENABLE_LEVEL3_ENCRYPT_ON;
            BTBMGapSetAuthenticationMode(TRUE, &ptr); 
        }             
#if L2CAP_NUM_GROUPS > 0
        else if(strncmp(string, "groupreq",8)==0)
        {
            L2capGroupAddr parms;
            U8 bd_addr[6];
            
            bt_trace(TRACE_GROUP_1,GROUP_ADD_REQUEST);
            memset(bd_addr, 0, 6);
            if(!l2capTester_get_bdaddr_from_string(bd_addr, string+8))
            {
                return;
            }    		
            memcpy((U8 *)parms.bd_addr, (U8 *)bd_addr, 6);
            L2CA_AddGroup(&parms);
        }   
        else if(strncmp(string, "grouprem",8)==0)
        {
            L2capGroupAddr parms;
            U8 bd_addr[6];
            
            bt_trace(TRACE_GROUP_1,GROUP_REMOVE_REQUEST);
            memset(bd_addr, 0, 6);
            if(!l2capTester_get_bdaddr_from_string(bd_addr, string+8))
            {
                return;
            }    		
            memcpy((U8 *)parms.bd_addr, (U8 *)bd_addr, 6);
            L2CA_RemoveGroup(&parms);
        }   
        else if(strncmp(string, "sendgroupdata",13)==0)
        {
            bt_trace(TRACE_GROUP_1,SEND_GROUP_DATA);
            L2CA_GroupDataReq();
        }
        else if(strncmp(string, "sendunidata",11)==0)
        {
            bt_trace(TRACE_GROUP_1,SEND_GROUP_DATA);
            L2CA_UniCastDataReq();
        }
        else if(strncmp(string, "setencry", 8)==0)
        {
            Report(("set encryption on"));
            BTBMGapSetEncryptionOn();        
        }
#endif
    }
}

void bt_l2captester_handler_help(){
    OS_Report("l2cap: bt_l2captester_handler_help");
    OS_Report("l2 config <cid> - reconfig (config_req) the channel");    
    OS_Report("l2 dump - dump the channel");
    OS_Report("l2 disc <cid> - disc the channel");
    OS_Report("l2 disc <cid> - disc the channel");
}


void bt_l2captester_handler(char *string){
    U32 ij = 0, j = 0, cid = 0;
    BtPacket *req = NULL;
    BtStatus status = BT_STATUS_FAILED;
    BOOL l_handled = FALSE;

    if( NULL == string ){
        return;
    }
    
    // "l2 config 42"
    if(strncmp(string, "l2", 2)==0){
        string +=2;
        while( *string == ' '  || string == NULL ){
            string++;
        }
        if(strncmp(string, "config", 6)==0){
            l_handled = TRUE;
            string += 6;

            cid = atoi(string);
            OS_Report("l2cap: config_req cid:%d", cid);

            req = L2Cap_GetSysPkt(FALSE);
            if( req != NULL ){
              status = L2CAP_SendConfigReq(cid, req, 0);
              OS_Report("l2cap: status:%d", status);
              if(status != BT_STATUS_PENDING){
                 L2Cap_ReturnSysPkt(req);
              }
            }else{
              OS_Report("l2cap: fail status:%d", status);
            }
        }
        else if(strncmp(string, "dump", 4)==0){
            l_handled = TRUE;
            string += 4;
            L2CAP_DumpChannel();
        }else if(strncmp(string, "sniff", 5)==0){
            // sniff the link
            
        }else if(strncmp(string, "unsni", 5)==0){
            // unsniff the link
            
        }else if(strncmp(string, "master", 5)==0){
            // change to local as master role
            l_handled = TRUE;
            string += 5;
            
        }else if(strncmp(string, "slave", 5)==0){
            // change to local as slave role
            l_handled = TRUE;
            string += 5;
            
        }else if(strncmp(string, "disc", 4)==0){
            l_handled = TRUE;
            string += 4;
            
            cid = atoi(string);
            OS_Report("l2cap: disc cid:%d", cid); 
            status = L2CAP_DisconnectReq(cid);
            OS_Report("l2cap: status:%d", status);
        }
            
    }else{
        OS_Report("l2cap: ignore !!! str:%s", string);
    }
    if( TRUE != l_handled  ){
        bt_l2captester_handler_help();
    }
}

void l2cap_handler_inject_msg(char *string)
{
#if 0
    bt_test_cmd_struct *test_msg;

    test_msg = ( bt_test_cmd_struct*)
    	                       construct_local_para (sizeof(bt_test_cmd_struct),TD_UL);

	memcpy((U8 *)test_msg->string, (U8 *)string, strlen(string));
    l2cap_tester_sendMsg(
                            MSG_ID_BT_L2CAP_TEST_CMD_REQ,
                            MOD_BT,
                            BT_APP_SAP,
                            (local_para_struct*)test_msg,
                            NULL
                            );
#endif                            
}

void l2capTesterL2Callback(L2capChannelId L2ChannelId, L2capCallbackParms *info)
{

   //add by stanley:2006-0721
    local_para_struct* msg;
    bt_test_cmd_rsp_struct  *param_ptr;


    bt_trace(BT_TRACE_G1_PROTOCOL, BT_L2CAPTS_EVENT, info->event);
    switch (info->event) 
    {
    case L2EVENT_CONNECT_IND:
        L2CAPTS(cid) = L2ChannelId;

        //add by stanley:2006-0721
   
        param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  

        memset(param_ptr->test_cmd_rsp_buf,0,256);

        param_ptr->test_cmd_rsp_buf[0]=0x03; //logs type
        param_ptr->test_cmd_rsp_buf[1]=0x02; //L2cap category
    
        kal_sprintf((kal_char*)(param_ptr->test_cmd_rsp_buf+4),"L2CA_ConnectReq,using cid=%d",L2ChannelId);
        Report(("str data send to L4C:%s",param_ptr->test_cmd_rsp_buf+4));
        param_ptr->length= strlen((kal_char*)param_ptr->test_cmd_rsp_buf+4);
        bt_trace(TRACE_GROUP_1,STR_DATA_LENxD ,param_ptr->length);

         //fill the data length field of string data for rsp
         param_ptr->test_cmd_rsp_buf[2]= (U8)(param_ptr->length  & 0x00ff);
         param_ptr->test_cmd_rsp_buf[3]= (U8)((param_ptr->length >> 8) & 0x00ff);

         //rsp_buf's total length = param_ptr->length + 4 bytes header
         param_ptr->length += 4;
    
         msg = (local_para_struct*)param_ptr;

#if 0
         bt_trace(TRACE_GROUP_1,CALL_BTTESTER_SENDMSG);
    
         btTester_sendMsg(MSG_ID_BT_L4C_TEST_CMD_CNF, 
#ifdef __GEMINI__
                                        Get_DualSim_Uart_Default_Owner(),
#else
                                        MOD_ATCI, 
#endif
                                        INVALID_SAP, 
                                        msg, 
                                        NULL);
        
#endif        
        L2CAP_ConnectRsp(L2ChannelId,
                                   L2CONN_ACCEPTED, 0);        
        break;
    case L2EVENT_DISCONNECTED:
    case L2EVENT_CONNECTED:
    case L2EVENT_PACKET_HANDLED:
        break;
    case L2EVENT_DATA_IND:
        break;
    case L2EVENT_PING_COMPLETE:
        break;
    case L2EVENT_INFO_COMPLETE:
        break;
    }
}


// add by stanley : 2006-0719
void L2CA_TesterDisconnect(U8 cid)
{
    L2capDisconnectRequest parms;
    if(cid == 0x00)
        parms.cid = L2CAPTS(cid);
    else
        parms.cid = cid;
    bt_trace(TRACE_GROUP_1,L2CAP_TESTER_DISCONN_REQ);
    L2CA_DisconnectReq(&parms);
}


#if L2CAP_NUM_GROUPS > 0

U8 L2CA_AddGroup(L2capGroupAddr *parms)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BD_ADDR bd_addr;
    U16 l2ChannelId;
    CmgrHandler     *cmgrHandler; 
    /* Establish the ACL link */
    
    memcpy((U8 *)bd_addr.addr, (U8 *)parms->bd_addr,6);
    if(L2CAPTS(cmgrHandler1).bdc !=0)
        cmgrHandler = &L2CAPTS(cmgrHandler1);
    else
        cmgrHandler = &L2CAPTS(cmgrHandler2);
    (void)CMGR_RegisterHandler(cmgrHandler, l2capTesterGroupCmgrCallback);
    status = CMGR_CreateDataLink(cmgrHandler, &bd_addr);
    if((cmgrHandler->bdc !=0) && (cmgrHandler->bdc->link !=0))
        CMGR_DisableSniffTimer(cmgrHandler);
    if (status == BT_STATUS_SUCCESS) 
    {
        L2CAP_GroupAddMember(L2CAPTS(groupId), &bd_addr);
    }
    return status;
}


U8 L2CA_RemoveGroup(L2capGroupAddr *parms)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BD_ADDR bd_addr;
   
    memcpy((U8 *)bd_addr.addr, (U8 *)parms->bd_addr,6);
    L2CAP_GroupRemoveMember(L2CAPTS(groupId), &bd_addr);
}

void L2CA_GroupDataReq(void)
{
    bt_trace(TRACE_GROUP_1,L2CAP_GROUP_DATA_REQUEST);
    L2CAPTS(btp).flags = BTP_FLAG_INUSE;
    L2CAPTS(btp).headerLen = 0;
    memset( (U8 *)L2CAPTS(common_data), 0x55, 10);
    L2CAPTS(btp).data = (U8 *)L2CAPTS(common_data);
    L2CAPTS(btp).dataLen = 10;
    L2CAPTS(btp).priority_type = BTP_FLAG_LOW_PRIPORTY ;    
    L2CAP_SendData(L2CAPTS(groupId), &L2CAPTS(btp));
}

#ifdef __BT_3_0_UNICAST__
void L2CA_UniCastDataReq(void)
{
    BtRemoteDevice *link;
    if(L2CAPTS(cmgrHandler1).bdc !=0)
        link = L2CAPTS(cmgrHandler1).bdc->link;
    else if(L2CAPTS(cmgrHandler2).bdc !=0)
        link = L2CAPTS(cmgrHandler2).bdc->link;
    else if (L2CAPTS(cmgrHandler).bdc !=0)
        link = L2CAPTS(cmgrHandler).bdc->link;
    else
        Assert(0);
        
    Report(("UniCast group send data"));
    L2CAPTS(btp).flags = BTP_FLAG_INUSE | BTP_FLAG_RDEV_FISRT;
    L2CAPTS(btp).headerLen = 0;
    memset( (U8 *)L2CAPTS(common_data), 0x55, 10);
    L2CAPTS(btp).data = (U8 *)L2CAPTS(common_data);
    L2CAPTS(btp).dataLen = 10;
    L2CAPTS(btp).priority_type = BTP_FLAG_LOW_PRIPORTY ;    
    L2CAP_SendUnicastData(&L2CAPTS(Protocol), link, &L2CAPTS(btp));
}
#endif

static void L2Group_Event(U16 ChannelId, L2capCallbackParms *Info)
{
    if (Info->event == L2EVENT_PACKET_HANDLED) 
    {
    }
    else if (Info->event == L2EVENT_DATA_IND)
    {
    }
}

void l2capTesterGroupCmgrCallback(CmgrHandler *cHandler, 
                      CmgrEvent Event, 
                      BtStatus Status)
{
    L2capChannelId l2ChannelId;
    BtStatus status;
    
    switch (Event) 
    {
        case CMEVENT_DATA_LINK_CON_CNF:
            if (Status == BT_STATUS_SUCCESS) 
            {
                bt_trace(TRACE_GROUP_1,ACL_LINK_CREATE_SUCCESSFULLY);
                L2CAP_GroupAddMember(L2CAPTS(groupId), &(cHandler->bdc->link->bdAddr));
            }
            break;
        case CMEVENT_DATA_LINK_DIS:
            L2CAP_GroupRemoveMember(L2CAPTS(groupId), &cHandler->bdc->link->bdAddr);
            cHandler->bdc =0;
            break;
    }
}

#endif // endif  L2CAP_NUM_GROUPS > 0

#endif

BOOL l2cap_query_testing_state(void)
{
#ifdef __L2CAP_TESTER_
        if(L2CAPTS(testing_enabled))
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
#else
    return TRUE;
#endif
}

BOOL l2capTester_get_bdaddr_from_string(U8 *bd_addr, char *string)
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
