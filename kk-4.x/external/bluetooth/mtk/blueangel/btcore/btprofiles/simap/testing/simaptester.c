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
#ifdef __SIMAP_MAUI__
#include "sys/avalloc.h"
#include "sys/avsigmgr.h"
//#include "bluetooth_bm_struct.h"
#endif
#include "bt_simi.h"
#include "osapi.h"
#include "sdap.h"
#include "me.h"
#include "string.h"

#ifdef __SIMAP_UT_

extern kal_bool bm_init_with_sniff_mode;
extern BtStatus ME_StopSniff(BtRemoteDevice *remDev); 
extern void bt_send_msg (msg_type msg, module_type dstMod ,sap_type sap,local_para_struct *local_para,
                                                              peer_buff_struct* peer_buff);

static void simap_tester_sendMsg(
                                        msg_type msg, 
                                        module_type dstMod ,
                                        sap_type sap,
                                        local_para_struct *local_para,
                                        peer_buff_struct *peer_buff);

#ifdef __SIMAP_MAUI__
static void simap_tester_sendMsg(
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



void simaptester_handler_inject_msg(char *string)
{
    bt_test_cmd_struct *test_msg;
    
    test_msg = ( bt_test_cmd_struct*)
    	                       construct_local_para (sizeof(bt_test_cmd_struct),TD_UL);

    memcpy((U8 *)test_msg->string, (U8 *)string, strlen(string));
    simap_tester_sendMsg(
                        MSG_ID_BT_SIMAP_INJECT_MESSAGE,
                        MOD_BT,
                        BT_APP_SAP,
                        (local_para_struct*)test_msg,
                        NULL
                        );
    
}

#else
extern void SIMAP_sendMsg(
			msg_type msg,
			module_type dstMod,
			sap_type sap,
			local_para_struct *local_para,
			peer_buff_struct *peer_buff);

#endif


void simaptester_handler(ilm_struct *ilm_ptr)
{
    char *string;
    bt_test_cmd_struct *test_msg;

#ifdef __SIMAP_MAUI__
    if (ilm_ptr->msg_id == MSG_ID_BT_SIMAP_INJECT_MESSAGE)
#endif
    {
        test_msg = ((bt_test_cmd_struct *)ilm_ptr->local_para_ptr);    
        string = (char *)test_msg->string;
        Report(("inject str=%s",string));
        if(strncmp(string,"cardstatus",10)==0)
        {
            /* Update the simcardstatus */
            U8 c_1=0,c_2=0;
            if((string[10]>='0') && (string[10]<='9'))
                c_1 = string[10] - '0';
            else if((string[10]>='a') && (string[10]<='f'))
                c_1 = string[10] - 'a' + 10; 
            else if((string[10]>='A') && (string[10]<='F'))
                c_1 = string[10] - 'A' + 10;

            if((string[11]>='0') && (string[11]<='9'))
                c_2 = string[11] - '0';
            else if((string[11]>='a') && (string[11]<='f'))
                c_2 = string[11] - 'a' + 10; 
            else if((string[11]>='A') && (string[11]<='F'))
                c_2 = string[11] - 'A' + 10;

            SIM(simServer)[0].current_simcardreader_status = ((c_1<<4) | c_2);
            bt_trace(TRACE_GROUP_1,NEW_SIMCARDSTATUSxXxXxX ,c_1,c_2,SIM(simServer)[0].current_simcardreader_status);
        }
        else if(strncmp(string,"conmode1",8)==0)
        {
            /*
            #ifdef CONNECTION_MODE
                #undef CONNECTION_MODE
                #define CONNECTION_MODE SLOW_CONNECT_MODE
            #else
                #define CONNECTION_MODE SLOW_CONNECT_MODE
            #endif
            */
            bt_trace(TRACE_GROUP_1,SET_AS_SLOW_CONNECT_MODE);
            SIM(simServer)[0].connect_mode = SLOW_CONNECT_MODE;
            
        }
        else if(strncmp(string,"conmode2",8)==0)
        {
            /* The following states are determined in compile time!
               If in execution time, using catcher inject msg, it does not work to change this define value 
            #ifdef CONNECTION_MODE
                #undef CONNECTION_MODE
                #define CONNECTION_MODE FAST_CONNECT_MODE
            #else
                #define CONNECTION_MODE FAST_CONNECT_MODE
            #endif
            */
            bt_trace(TRACE_GROUP_1,SET_AS_FAST_CONNECT_MODE);
            SIM(simServer)[0].connect_mode = FAST_CONNECT_MODE;
        }
        else if(strncmp(string,"ST=",3)==0)
        {
            /* Setting supported T type */
            if(string[3]=='0') SIM(simServer)[0].supported_transport_protocol_type_capability = 0;
            else if(string[3]=='1') SIM(simServer)[0].supported_transport_protocol_type_capability = 1;
            else SIM(simServer)[0].supported_transport_protocol_type_capability = 2;
            bt_trace(TRACE_GROUP_1,NEW_SUPPORTED_T_TYPExD ,SIM(simServer)[0].supported_transport_protocol_type_capability);
        }
        else if(strncmp(string,"CT=",3)==0)
        {
            /* Setting current T type */
            if(string[3]=='0') SIM(simServer)[0].current_transport_protocol_type = 0;
            else if(string[3]=='1') SIM(simServer)[0].current_transport_protocol_type = 1;
            bt_trace(TRACE_GROUP_1,NEW_CURRENT_T_TYPExD ,SIM(simServer)[0].current_transport_protocol_type);
        }
        else if(strncmp(string,"SIMResult=",9)==0)
        {
            /* Simulate the result return from the SIM task */
            if(string[9]=='0') SIM(simServer)[0].result_from_simtask = SIM_RESULT_OK;
            else if(string[9]=='1') SIM(simServer)[0].result_from_simtask = SIM_RESULT_NO_REASON;
            else if(string[9]=='2') SIM(simServer)[0].result_from_simtask = SIM_RESULT_CARD_NOT_ACCESSIBLE;
            else if(string[9]=='3') SIM(simServer)[0].result_from_simtask = SIM_RESULT_CARD_ALREADY_OFF;
            else if(string[9]=='4') SIM(simServer)[0].result_from_simtask = SIM_RESULT_CARD_REMOVED;
            else if(string[9]=='5') SIM(simServer)[0].result_from_simtask = SIM_RESULT_CARD_ALREADY_ON;
            else if(string[9]=='6') SIM(simServer)[0].result_from_simtask = SIM_RESULT_DATA_NOT_AVAILABLE;

        }
        else if(strncmp(string,"discg",5)==0)
        {
            /* Disconnection with Graceful type */
            bt_simap_disconnect_req_struct *msg = construct_local_para(sizeof(bt_simap_disconnect_req_struct),TD_UL);

            msg->discType = SIM_DISCONNECT_GRACEFUL;
            bt_trace(TRACE_GROUP_1,SIMULATE_MMI_SEND_DISC_REQ_WITH_GRACEFUL_TYPE_TO_BT);
#ifdef __SIMAP_MAUI__
            bt_send_msg(MSG_ID_BT_SIMAP_DISCONNECT_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*)msg, NULL);
#else
	     SIMAP_sendMsg(MSG_ID_BT_SIMAP_DISCONNECT_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*)msg, NULL);
#endif
        }
        else if(strncmp(string,"disci",5)==0)
        {
            /* Disconnection with Immediate type */
           bt_simap_disconnect_req_struct *msg = construct_local_para(sizeof(bt_simap_disconnect_req_struct),TD_UL);

            msg->discType = SIM_DISCONNECT_IMMEDIATE;
            bt_trace(TRACE_GROUP_1,SIMULATE_MMI_SEND_DISC_REQ_WITH_IMMEDIATE_TYPE_TO_BT);
#ifdef __SIMAP_MAUI__
            bt_send_msg(MSG_ID_BT_SIMAP_DISCONNECT_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*)msg, NULL);
#else
	     SIMAP_sendMsg(MSG_ID_BT_SIMAP_DISCONNECT_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*)msg, NULL);
#endif
        }
	 else if(strncmp(string, "con",3) == 0)
	 {
		/* Add this test cmd for private test : 2007-01-17*/
		SIM(simServer)[0].exec_mode = 2; /* exec_mode=2: For MMI test to show SIM Acceess Profile */
		SIM(simServer)[0].available = FALSE;
		SIM(simServer)[0].state = SIM_SERVER_STATE_CONN_PENDING;
		SIM(simServer)[0].substate |= BT_SIM_CONNECT_REQ_SENT; 
		bt_trace(TRACE_GROUP_1,SIMULATE_SIM_CLIENT_SENDS_CONNECT_REQ_AND_TEST_MODExD ,SIM(simServer)[0].exec_mode);
#ifdef __SIMAP_MAUI__
		bt_send_msg(MSG_ID_BT_SIM_CONNECT_REQ, MOD_SIM, PS_SIM_SAP, NULL, NULL); 
#else
		SIMAP_sendMsg(MSG_ID_BT_SIM_CONNECT_REQ, MOD_SIM, PS_SIM_SAP, NULL, NULL);
#endif
	 }
	 else if(strncmp(string,"exitsniff",9) == 0)
	 {
              Assert(SIM(simServer)[0].remDev != NULL);
		bt_trace(TRACE_GROUP_1,EXIT_SNIFF_MODE_FOR_SAP_CONNECTION);	  
		ME_StopSniff(SIM(simServer)[0].remDev);
	 }
	 /*
	 else if(strncmp(string,"nosniff",7) ==0)
	 {
		bm_init_with_sniff_mode = KAL_FALSE; 
		bt_trace(TRACE_GROUP_1,IN_THIS_MODE_TEST_SAP_WITHOUT_SNIFF_MODE_SUPPORT);
	 }
	 else if(strncmp(string,"noswitch",8) == 0)
	 {
		
	 }
	 */
        else if(strncmp(string,"execmode",8)==0)
        {
            if(string[8] == '1')
            {
                /* Using UT mode to send msg to my fake_sim */
                SIM(simServer)[0].exec_mode = 1;
                bt_trace(TRACE_GROUP_1,CHANGE_TO_UT_MODE);
            }
            else if(string[8] == '2')
            {
                /* Using SIM task to process BT module's message */
                SIM(simServer)[0].exec_mode = 0;
                bt_trace(TRACE_GROUP_1,CHANGE_TO_IT_MODE);
            }
        }
        else if(strncmp(string,"concode",7)==0)
        {
            
            /* Simulate the CONNECT_RSP result return from the SIM task */
            if(string[7]=='0') SIM(simServer)[0].concode = SIM_RESULT_OK;
            else if(string[7]=='1') SIM(simServer)[0].concode = SIM_RESULT_NO_REASON;
            else if(string[7]=='2') SIM(simServer)[0].concode = SIM_RESULT_CARD_NOT_ACCESSIBLE;
            else if(string[7]=='3') SIM(simServer)[0].concode = SIM_RESULT_CARD_ALREADY_OFF;
            else if(string[7]=='4') SIM(simServer)[0].concode = SIM_RESULT_CARD_REMOVED;
            else if(string[7]=='5') SIM(simServer)[0].concode = SIM_RESULT_CARD_ALREADY_ON;
            else if(string[7]=='6') SIM(simServer)[0].concode = SIM_RESULT_DATA_NOT_AVAILABLE;

        }
        else if(strncmp(string,"resetcode",9)==0)
        {    
            /* Simulate the RESET_RSP result return from the SIM task */
            if(string[9]=='0') SIM(simServer)[0].resetcode = SIM_RESULT_OK;
            else if(string[9]=='1') SIM(simServer)[0].resetcode = SIM_RESULT_NO_REASON;
            else if(string[9]=='2') SIM(simServer)[0].resetcode = SIM_RESULT_CARD_NOT_ACCESSIBLE;
            else if(string[9]=='3') SIM(simServer)[0].resetcode = SIM_RESULT_CARD_ALREADY_OFF;
            else if(string[9]=='4') SIM(simServer)[0].resetcode = SIM_RESULT_CARD_REMOVED;
            else if(string[9]=='5') SIM(simServer)[0].resetcode = SIM_RESULT_CARD_ALREADY_ON;
            else if(string[9]=='6') SIM(simServer)[0].resetcode = SIM_RESULT_DATA_NOT_AVAILABLE;

        }
        else if(strncmp(string,"disccode",8)==0)
        {
            /* Simulate the DISC_RSP result return from the SIM task */
            if(string[8]=='0') SIM(simServer)[0].disccode = SIM_RESULT_OK;
            else if(string[8]=='1') SIM(simServer)[0].disccode = SIM_RESULT_NO_REASON;
            else if(string[8]=='2') SIM(simServer)[0].disccode = SIM_RESULT_CARD_NOT_ACCESSIBLE;
            else if(string[8]=='3') SIM(simServer)[0].disccode = SIM_RESULT_CARD_ALREADY_OFF;
            else if(string[8]=='4') SIM(simServer)[0].disccode = SIM_RESULT_CARD_REMOVED;
            else if(string[8]=='5') SIM(simServer)[0].disccode = SIM_RESULT_CARD_ALREADY_ON;
            else if(string[8]=='6') SIM(simServer)[0].disccode = SIM_RESULT_DATA_NOT_AVAILABLE;
           
        }
        else if(strncmp(string,"offcode",7)==0)
        {
            /* Simulate the POWEROFF_RSP result return from the SIM task */
            if(string[7]=='0') SIM(simServer)[0].offcode = SIM_RESULT_OK;
            else if(string[7]=='1') SIM(simServer)[0].offcode = SIM_RESULT_NO_REASON;
            else if(string[7]=='2') SIM(simServer)[0].offcode = SIM_RESULT_CARD_NOT_ACCESSIBLE;
            else if(string[7]=='3') SIM(simServer)[0].offcode = SIM_RESULT_CARD_ALREADY_OFF;
            else if(string[7]=='4') SIM(simServer)[0].offcode = SIM_RESULT_CARD_REMOVED;
            else if(string[7]=='5') SIM(simServer)[0].offcode = SIM_RESULT_CARD_ALREADY_ON;
            else if(string[7]=='6') SIM(simServer)[0].offcode = SIM_RESULT_DATA_NOT_AVAILABLE;

        }
        else if(strncmp(string,"oncode",6)==0)
        {
            /* Simulate the DISC_RSP result return from the SIM task */
            if(string[6]=='0') SIM(simServer)[0].oncode = SIM_RESULT_OK;
            else if(string[6]=='1') SIM(simServer)[0].oncode = SIM_RESULT_NO_REASON;
            else if(string[6]=='2') SIM(simServer)[0].oncode = SIM_RESULT_CARD_NOT_ACCESSIBLE;
            else if(string[6]=='3') SIM(simServer)[0].oncode = SIM_RESULT_CARD_ALREADY_OFF;
            else if(string[6]=='4') SIM(simServer)[0].oncode = SIM_RESULT_CARD_REMOVED;
            else if(string[6]=='5') SIM(simServer)[0].oncode = SIM_RESULT_CARD_ALREADY_ON;
            else if(string[6]=='6') SIM(simServer)[0].oncode = SIM_RESULT_DATA_NOT_AVAILABLE;

        }
        else if(strncmp(string,"tcode",5)==0)
        {
            /* Simulate the SET_T_RSP result return from the SIM task */
            if(string[5]=='0') SIM(simServer)[0].tcode = SIM_RESULT_OK;
            else if(string[5]=='1') SIM(simServer)[0].tcode = SIM_RESULT_NO_REASON;
            else if(string[5]=='2') SIM(simServer)[0].tcode = SIM_RESULT_CARD_NOT_ACCESSIBLE;
            else if(string[5]=='3') SIM(simServer)[0].tcode = SIM_RESULT_CARD_ALREADY_OFF;
            else if(string[5]=='4') SIM(simServer)[0].tcode = SIM_RESULT_CARD_REMOVED;
            else if(string[5]=='5') SIM(simServer)[0].tcode = SIM_RESULT_CARD_ALREADY_ON;
            else if(string[5]=='6') SIM(simServer)[0].tcode = SIM_RESULT_DATA_NOT_AVAILABLE;

        }
        else if(strncmp(string,"active",6)==0)
        {
            bt_trace(TRACE_GROUP_1,SIMULATE_MMI_TO_SEND_ACTIVATE_MSG_TO_SIMAP);
#ifdef __SIMAP_MAUI__
            bt_send_msg(MSG_ID_BT_SIMAP_ACTIVATE_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
#else
	     SIMAP_sendMsg(MSG_ID_BT_SIMAP_ACTIVATE_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
#endif
        }
        else if(strncmp(string,"deactive",8)==0)
        {
            bt_trace(TRACE_GROUP_1,SIMULATE_MMI_TO_SEND_DEACTIVATE_MSG_TO_SIMAP);
#ifdef __SIMAP_MAUI__
            bt_send_msg(MSG_ID_BT_SIMAP_DEACTIVATE_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
#else
	     SIMAP_sendMsg(MSG_ID_BT_SIMAP_DEACTIVATE_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
#endif
        }
        else if(strncmp(string,"authrsp",7)==0)
        {
            /* 
             * authrspX: 
             * X == 1: Accept
             * X == 0: Reject
             */
            bt_sim_connect_cnf_struct *msg = construct_local_para(sizeof(bt_sim_connect_cnf_struct),TD_UL);

	     if(string[7]=='0')
	     {
	     	msg->result = FALSE;
	     }
	     else if (string[7]=='1')
	     {
	     	msg->result = TRUE;
	     }
            bt_trace(TRACE_GROUP_1,SIMULATE_MMI_TO_SEND_AUTHORIZATION_RESPONSE_MSG_TO_SIMAP, msg->result);
#ifdef __SIMAP_MAUI__
            bt_send_msg(MSG_ID_BT_SIMAP_AUTH_RSP, MOD_BT, BT_APP_SAP, (local_para_struct*)msg, NULL);
#else
	     SIMAP_sendMsg(MSG_ID_BT_SIMAP_AUTH_RSP, MOD_BT, BT_APP_SAP, (local_para_struct*)msg, NULL);
#endif
        }
        else if (strncmp(string,"cardnotaccess",13)==0)
        {
            bt_trace(TRACE_GROUP_1, SIMULATE_SIM_CARD_NOT_ACCESSIBLE_FOR_PTS_TESTING);
#ifdef __SIMAP_MAUI__
            bt_send_msg(MSG_ID_BT_SIMAP_SEND_STATUS_IND_WITH_UNAVAILABLE_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
#else
	     SIMAP_sendMsg(MSG_ID_BT_SIMAP_SEND_STATUS_IND_WITH_UNAVAILABLE_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
#endif
        }
    }/* End of MSG_ID_BT_SIMAP_INJECT_MESSAGE */   

}
#endif //__SIMAP_UT_


