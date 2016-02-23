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



/* basic system service headers */
#include "bt_common.h"
#ifdef __SIMAP_MAUI__
#include "kal_trace.h"          /* Trace Header Files */
#include "uart_sw.h"
//#include "bt_maui_utility.h"

#endif
#include "stdio.h"              /* Basic C I/O functions */
#include "bt.h"
#include "bt_simi.h"
//#include "bt_adp_simap.h"
#include "bluetooth_struct.h"
#include "bluetooth_trc.h"
#include "sdpdb.h"
#include "config.h"
#include "bluetooth_simap_message.h"


extern void SIMAP_sendMsg(
			msg_type msg,
			module_type dstMod,
			sap_type sap,
			local_para_struct *local_para,
			peer_buff_struct *peer_buff);


#ifdef __SIMAP_UT_

void bt_fakesim_connect_req_hdlr(void)
{
    bt_sim_connect_cnf_struct *bt_sim_connect_cnf = construct_local_para(sizeof(bt_sim_connect_cnf_struct),TD_UL);
    /* The atr_rsp content is the same with Nokia N91 */
    U8 atr_rsp[13]={0x3B,0x39,0x11,0x00,0x29,0x81,0x2A,0x01,0xA3,0xA2,0xA0,0x4F,0xC6};
    bt_trace(TRACE_GROUP_1,FAKESIM_RECEIVES_BT_SIM_CONNECT_REQ);
    bt_sim_connect_cnf->atr_len = 13;
    OS_MemCopy((U8*)bt_sim_connect_cnf->atr,atr_rsp, bt_sim_connect_cnf->atr_len);
    bt_sim_connect_cnf->current_transport_protocol_type = TRANS_PRO_TO;
    /* 2006-11-28: Test for err handling by stanley */
    //bt_sim_connect_cnf->result = SIM_RESULT_OK;
    bt_sim_connect_cnf->result = SIM(simServer)[0].concode;
    bt_sim_connect_cnf->supported_transport_protocol_type_capability = TRANS_PRO_TO_T1; /* This means */
#ifdef __SIMAP_MAUI__
    bt_send_msg(MSG_ID_BT_SIM_CONNECT_CNF, MOD_BT, PS_SIM_SAP, (local_para_struct*)bt_sim_connect_cnf, NULL);
#else
    SIMAP_sendMsg(MSG_ID_BT_SIM_CONNECT_CNF, MOD_BT, 0, (local_para_struct*)bt_sim_connect_cnf, NULL);
#endif
}

void bt_fakesim_reset_req_hdlr(void)
{
    bt_sim_reset_cnf_struct *bt_sim_reset_cnf = construct_local_para(sizeof(bt_sim_reset_cnf_struct),TD_UL);
    /* The atr_rsp content is the same with Nokia N91 */
    U8 atr_rsp[13]={0x3B,0x39,0x11,0x00,0x29,0x81,0x2A,0x01,0xA3,0xA2,0xA0,0x4F,0xC6};

    bt_trace(TRACE_GROUP_1,FAKESIM_RECEIVES_BT_SIM_RESET_REQ);
    bt_sim_reset_cnf->atr_len = 13;
    OS_MemCopy((U8*)bt_sim_reset_cnf->atr,atr_rsp, bt_sim_reset_cnf->atr_len);
    //bt_sim_reset_cnf->result = SIM_RESULT_CARD_NOT_ACCESSIBLE;
    bt_sim_reset_cnf->result = SIM(simServer)[0].resetcode;
    bt_sim_reset_cnf->current_transport_protocol_type = TRANS_PRO_TO;
#ifdef __SIMAP_MAUI__
    bt_send_msg(MSG_ID_BT_SIM_RESET_CNF, MOD_BT, PS_SIM_SAP, (local_para_struct*)bt_sim_reset_cnf, NULL);
#else
    SIMAP_sendMsg(MSG_ID_BT_SIM_RESET_CNF, MOD_BT, 0, (local_para_struct*)bt_sim_reset_cnf, NULL);
#endif
}

void bt_fakesim_disconnect_req_hdlr(void)
{
    bt_sim_disconnect_cnf_struct *bt_sim_disconnect_cnf = construct_local_para(sizeof(bt_sim_disconnect_cnf_struct),TD_UL);
    bt_trace(TRACE_GROUP_1,FAKESIM_RECEIVES_BT_SIM_DISCONNECT_REQ);
    //bt_sim_disconnect_cnf->result = SIM_RESULT_OK;
    bt_sim_disconnect_cnf->result = SIM(simServer)[0].disccode;
#ifdef __SIMAP_MAUI__
    bt_send_msg(MSG_ID_BT_SIM_DISCONNECT_CNF, MOD_BT, PS_SIM_SAP, (local_para_struct*)bt_sim_disconnect_cnf, NULL);
#else
    SIMAP_sendMsg(MSG_ID_BT_SIM_DISCONNECT_CNF, MOD_BT, 0, (local_para_struct*)bt_sim_disconnect_cnf, NULL);
#endif
}

void bt_fakesim_apdu_req_hdlr(U16 len, U8* apdu_req_data)
{
    U8 apdu_rsp[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
    bt_sim_apdu_cnf_struct *bt_sim_apdu_cnf = construct_local_para(sizeof(bt_sim_apdu_cnf_struct),TD_UL);
    U16 i=0;
    
    bt_trace(TRACE_GROUP_1,FAKESIM_RECEIVES_BT_SIM_APDU_REQ);
    bt_trace(TRACE_GROUP_1,APDU_TYPEBT_DEBUGx02X ,apdu_req_data[1]);
    for(i=0; i < len; i++) bt_trace(TRACE_GROUP_1,BT_DEBUGx02X ,apdu_req_data[i]);

    
    Assert(apdu_req_data[0]==0xa0);
    if(apdu_req_data[1]==0xa4)
    {
        /* Select APDU */
        Assert(apdu_req_data[4]==0x02); /* length */ 
        bt_trace(TRACE_GROUP_1,SELECT_APDU_AND_FILE_IDBT_DEBUGx02X_BT_DEBUGx02X ,apdu_req_data[5],apdu_req_data[6]);
        bt_sim_apdu_cnf->apdu_rsp_len = 2;
        bt_sim_apdu_cnf->apdu_rsp[0]=0x9F;
        bt_sim_apdu_cnf->apdu_rsp[1]=0x17;
    }
    else if(apdu_req_data[1]==0xf2)
    {
        /* Status APDU */
        bt_trace(TRACE_GROUP_1,STATUS_APDU_AND_P3_LENBT_DEBUGx02X ,apdu_req_data[4]);
        bt_sim_apdu_cnf->apdu_rsp_len = 24;
        bt_sim_apdu_cnf->apdu_rsp[0]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[1]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[2]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[3]=0xd8;
        bt_sim_apdu_cnf->apdu_rsp[4]=0x3f;
        bt_sim_apdu_cnf->apdu_rsp[5]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[6]=0x01;
        bt_sim_apdu_cnf->apdu_rsp[7]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[8]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[9]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[10]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[11]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[12]=0x0a;
        bt_sim_apdu_cnf->apdu_rsp[13]=0x95;
        bt_sim_apdu_cnf->apdu_rsp[14]=0x03;
        bt_sim_apdu_cnf->apdu_rsp[15]=0x01;
        bt_sim_apdu_cnf->apdu_rsp[16]=0x08;
        bt_sim_apdu_cnf->apdu_rsp[17]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[18]=0x83;
        bt_sim_apdu_cnf->apdu_rsp[19]=0x8a;
        bt_sim_apdu_cnf->apdu_rsp[20]=0x83;
        bt_sim_apdu_cnf->apdu_rsp[21]=0x8a;
        bt_sim_apdu_cnf->apdu_rsp[22]=0x90;
        bt_sim_apdu_cnf->apdu_rsp[23]=0x00;
        
    }
    else if(apdu_req_data[1]==0xc0)
    {
        /* Get Response APDU */
        bt_trace(TRACE_GROUP_1,GET_RESPONSE_APDU_AND_P3_LENBT_DEBUGx02X ,apdu_req_data[4]);
        bt_sim_apdu_cnf->apdu_rsp_len = 25;
        bt_sim_apdu_cnf->apdu_rsp[0]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[1]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[2]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[3]=0xd8;
        bt_sim_apdu_cnf->apdu_rsp[4]=0x3f;
        bt_sim_apdu_cnf->apdu_rsp[5]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[6]=0x01;
        bt_sim_apdu_cnf->apdu_rsp[7]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[8]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[9]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[10]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[11]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[12]=0x0a;
        bt_sim_apdu_cnf->apdu_rsp[13]=0x95;
        bt_sim_apdu_cnf->apdu_rsp[14]=0x03;
        bt_sim_apdu_cnf->apdu_rsp[15]=0x01;
        bt_sim_apdu_cnf->apdu_rsp[16]=0x08;
        bt_sim_apdu_cnf->apdu_rsp[17]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[18]=0x83;
        bt_sim_apdu_cnf->apdu_rsp[19]=0x8a;
        bt_sim_apdu_cnf->apdu_rsp[20]=0x83;
        bt_sim_apdu_cnf->apdu_rsp[21]=0x8a;
        bt_sim_apdu_cnf->apdu_rsp[22]=0x00;
        bt_sim_apdu_cnf->apdu_rsp[23]=0x90;
        bt_sim_apdu_cnf->apdu_rsp[24]=0x00;

    }
    else 
    {
        /* APDU RSP here is fake information for testing purpose */
        bt_sim_apdu_cnf->apdu_rsp_len = 16;
        OS_MemCopy((U8*)bt_sim_apdu_cnf->apdu_rsp, apdu_rsp, bt_sim_apdu_cnf->apdu_rsp_len);
    }
    
    bt_sim_apdu_cnf->result = SIM_RESULT_OK;
#ifdef __SIMAP_MAUI__
    bt_send_msg(MSG_ID_BT_SIM_APDU_CNF, MOD_BT, PS_SIM_SAP, (local_para_struct*)bt_sim_apdu_cnf, NULL);
#else
    SIMAP_sendMsg(MSG_ID_BT_SIM_APDU_CNF, MOD_BT, 0, (local_para_struct*)bt_sim_apdu_cnf, NULL);
#endif
}

void bt_fakesim_poweroff_req_hdlr(void)
{
    bt_sim_power_off_cnf_struct *bt_sim_power_off_cnf = construct_local_para(sizeof(bt_sim_power_off_cnf_struct),TD_UL);
    
    bt_trace(TRACE_GROUP_1,FAKESIM_RECEIVES_BT_SIM_POWEROFF_REQ);
    //bt_sim_power_off_cnf->result = SIM_RESULT_OK;
    bt_sim_power_off_cnf->result = SIM(simServer)[0].offcode;
#ifdef __SIMAP_MAUI__
    bt_send_msg(MSG_ID_BT_SIM_POWER_OFF_CNF, MOD_BT, PS_SIM_SAP, (local_para_struct*)bt_sim_power_off_cnf, NULL);
#else
    SIMAP_sendMsg(MSG_ID_BT_SIM_POWER_OFF_CNF, MOD_BT, 0, (local_para_struct*)bt_sim_power_off_cnf, NULL);
#endif
}


void bt_fakesim_poweron_req_hdlr(void)
{
    bt_sim_reset_cnf_struct *bt_sim_reset_cnf = construct_local_para(sizeof(bt_sim_reset_cnf_struct),TD_UL); 
    /* The atr_rsp content is the same with Nokia N91 */
    U8 atr_rsp[13]={0x3B,0x39,0x11,0x00,0x29,0x81,0x2A,0x01,0xA3,0xA2,0xA0,0x4F,0xC6};
    
    bt_trace(TRACE_GROUP_1,FAKESIM_RECEIVES_BT_SIM_RESET_REQ_DUE_TO_POWERON);
    bt_sim_reset_cnf->atr_len = 13;
    OS_MemCopy((U8*)bt_sim_reset_cnf->atr,atr_rsp, bt_sim_reset_cnf->atr_len);
    //bt_sim_reset_cnf->result = SIM_RESULT_OK;
    bt_sim_reset_cnf->result = SIM(simServer)[0].oncode;
    bt_sim_reset_cnf->current_transport_protocol_type = TRANS_PRO_TO;
#ifdef __SIMAP_MAUI__
    bt_send_msg(MSG_ID_BT_SIM_RESET_CNF, MOD_BT, PS_SIM_SAP, (local_para_struct*)bt_sim_reset_cnf, NULL);
#else
    SIMAP_sendMsg(MSG_ID_BT_SIM_RESET_CNF, MOD_BT, 0, (local_para_struct*)bt_sim_reset_cnf, NULL);
#endif    
}

void bt_fakesim_set_transprotocol_type_req_hdlr(void)
{
    bt_sim_reset_cnf_struct *bt_sim_reset_cnf = construct_local_para(sizeof(bt_sim_reset_cnf_struct),TD_UL); 
    /* The atr_rsp content is the same with Nokia N91 */
    U8 atr_rsp[13]={0x3B,0x39,0x11,0x00,0x29,0x81,0x2A,0x01,0xA3,0xA2,0xA0,0x4F,0xC6};
    
    bt_trace(TRACE_GROUP_1,FAKESIM_RECEIVES_BT_SIM_RESET_REQ_DUE_TO_SET_TRANSPORT_PRO_TYPE);
    bt_sim_reset_cnf->atr_len = 13;
    OS_MemCopy((U8*)bt_sim_reset_cnf->atr,atr_rsp, bt_sim_reset_cnf->atr_len);
    //bt_sim_reset_cnf->result = SIM_RESULT_OK;
    bt_sim_reset_cnf->result = SIM(simServer)[0].tcode;
    bt_sim_reset_cnf->current_transport_protocol_type = TRANS_PRO_TO;
#ifdef __SIMAP_MAUI__
    bt_send_msg(MSG_ID_BT_SIM_RESET_CNF, MOD_BT, PS_SIM_SAP, (local_para_struct*)bt_sim_reset_cnf, NULL);
#else
    SIMAP_sendMsg(MSG_ID_BT_SIM_RESET_CNF, MOD_BT, 0, (local_para_struct*)bt_sim_reset_cnf, NULL);
#endif
}

#endif 

