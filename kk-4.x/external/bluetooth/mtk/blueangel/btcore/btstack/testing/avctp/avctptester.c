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
#include "avdtp.h"
#include "sys/avalloc.h"
#include "sys/avsigmgr.h"
#include "Avctptester.h"
#include "osapi.h"

//add by stanley 2006-08-14: if the bluetooth_struct.h is included in your C file, also the uart_sw.h must be included. 
#include "uart_sw.h"
#include "bluetooth_struct.h"
#include "bt_test_cmd.h"


#ifdef __AVCTP_TESTER_
                                  
AvctpTesterChannel avctpTesterChnl;

static void avctp_tester_sendMsg(
                                        msg_type msg, 
                                        module_type dstMod ,
                                        sap_type sap,
                                        local_para_struct *local_para,
                                        peer_buff_struct *peer_buff);

static void avctp_tester_sendMsg(
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


void Send_AVCTP_Event_Init(U8 register_result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x06; //AVCTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVCTP_Event_Init; //TC_AVCTP_Event_Init
    param_ptr->test_cmd_rsp_buf[5]=register_result;
    param_ptr->length = 2; //fill 1 byte AVCTP_event_type
    

    BtSendMsgToL4(param_ptr);


}


void Send_AVCTP_Event_Registration_Req_Result(U8 register_result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x06; //AVCTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVCTP_Event_Registration_Req_Result; //TC_AVCTP_Event_Registration_Req_Result
    param_ptr->test_cmd_rsp_buf[5]=register_result;
    param_ptr->length = 2; //fill 1 byte AVCTP_event_type
    
    BtSendMsgToL4(param_ptr);

}


void Send_AVCTP_Event_ConnectReq_Ind(AvctpChannel *Chnl, AvctpCallbackParms *Parms)
{
    
    bt_test_cmd_rsp_struct  *param_ptr;
    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x06; //AVCTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVCTP_Event_ConnectReq_Ind; //AVCTP_Event_ConnectReq_Ind
    memcpy(&param_ptr->test_cmd_rsp_buf[5] ,Parms->p.remDev->bdAddr.addr,6);
    param_ptr->length = 7; //fill 1 byte AVCTP_event_type + 6 bytes BD_ADDR

    BtSendMsgToL4(param_ptr);


}


void Send_AVCTP_Event_ConnectReq_Cfm(AvctpChannel *Chnl, AvctpCallbackParms *Parms, U16 connect_result, U16 config_result, U16 connec_req_status)
{
    
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x06; //AVCTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVCTP_Event_ConnectReq_Cfm; //TC_AVCTP_Event_ConnectReq_Cfm
    memcpy(&param_ptr->test_cmd_rsp_buf[5] ,Parms->p.remDev->bdAddr.addr,6);

    //inx 11-12: connect result
    param_ptr->test_cmd_rsp_buf[11] = (U8)(connect_result & 0x00FF);
    param_ptr->test_cmd_rsp_buf[12] = (U8)((connect_result>>8) & 0x00FF);
    //inx 13-14: config result
    param_ptr->test_cmd_rsp_buf[13] = (U8)(config_result & 0x00FF);
    param_ptr->test_cmd_rsp_buf[14] = (U8)((config_result >> 8) & 0x00FF);
    //inx 15-16: status
    param_ptr->test_cmd_rsp_buf[15] = (U8)(connec_req_status & 0x00FF);
    param_ptr->test_cmd_rsp_buf[16] = (U8)((connec_req_status >> 8) & 0x00FF);        
            
    param_ptr->length = 13; 

    BtSendMsgToL4(param_ptr);

}



void Send_AVCTP_Event_DisConnectReq_Ind(AvctpChannel *Chnl, AvctpCallbackParms *Parms)
{
     // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x06; //AVCTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVCTP_Event_DisConnectReq_Ind; //TC_AVCTP_Event_DisConnectReq_Ind
    memcpy(&param_ptr->test_cmd_rsp_buf[5] ,Parms->p.remDev->bdAddr.addr,6);
    param_ptr->length = 7; 

    BtSendMsgToL4(param_ptr);

}

void Send_AVCTP_Event_DisConnectReq_Cfm(AvctpChannel *Chnl, AvctpCallbackParms *Parms)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x06; //AVCTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVCTP_Event_DisConnectReq_Cfm; //AVCTP_Event_DisConnectReq_Cfm
    memcpy(&param_ptr->test_cmd_rsp_buf[5] ,Parms->p.remDev->bdAddr.addr,6);

    //inx 11-12: disconnect result : what data should be filled?
    param_ptr->test_cmd_rsp_buf[11]= 0x00;
    param_ptr->test_cmd_rsp_buf[12]= 0x00;
           
    param_ptr->length = 9; 

    BtSendMsgToL4(param_ptr);


}

void Send_AVCTP_Event_MessageRecReq_Ind(AvctpChannel *Chnl, AvctpCallbackParms *Parms)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    U16 msg_len =0;
    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  
    
    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x06; //AVCTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVCTP_Event_MessageRecReq_Ind; //TC_AVCTP_Event_MessageRecReq_Ind
    memcpy(&param_ptr->test_cmd_rsp_buf[5] ,Parms->p.remDev->bdAddr.addr,6);

    //TransactionID
    param_ptr->test_cmd_rsp_buf[11]= Chnl->avtpChnl.acpTransId;

    //Type : cmd type or rsp type
    param_ptr->test_cmd_rsp_buf[12]= avctpTesterChnl.current_operation;
    msg_len = avctpTesterChnl.data_offset; 

    //Raw Data len: 
    param_ptr->test_cmd_rsp_buf[13] = (U8) (msg_len & 0x00FF);
    param_ptr->test_cmd_rsp_buf[14] = (U8) ((msg_len>>8) & 0x00FF);
	
    memcpy(&param_ptr->test_cmd_rsp_buf[15],avctpTesterChnl.data, avctpTesterChnl.data_offset);
    param_ptr->length = 11 + msg_len; 
    BtSendMsgToL4(param_ptr);  

}


void Send_AVCTP_Event_ConnectReq_Result(U8 result)
{
     // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  
    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x06; //AVCTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVCTP_Event_ConnectReq_Result; //TC_AVCTP_Event_ConnectReq_Result
    param_ptr->test_cmd_rsp_buf[5]= result;
    param_ptr->length = 2; 
    BtSendMsgToL4(param_ptr);      

}

void Send_AVCTP_Event_ConnectRsp_Result(U8 result)
{
     // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  
    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x06; //AVCTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVCTP_Event_ConnectRsp_Result; //TC_AVCTP_Event_ConnectRsp_Result
    param_ptr->test_cmd_rsp_buf[5]= result;
    param_ptr->length = 2; 
    BtSendMsgToL4(param_ptr);      

}


void Send_AVCTP_Event_DisConnectReq_Result(U8 result)
{
     // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  
    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x06; //AVCTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVCTP_Event_DisConnectReq_Result; //TC_AVCTP_Event_DisConnectReq_Result
    param_ptr->test_cmd_rsp_buf[5]= result;
    param_ptr->length = 2; 
    BtSendMsgToL4(param_ptr);      

}

void Send_AVCTP_Event_DisConnectRsp_Result(U8 result)
{
     // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  
    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x06; //AVCTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVCTP_Event_DisConnectRsp_Result; //TC_AVCTP_Event_DisConnectRsp_Result
    param_ptr->test_cmd_rsp_buf[5]= result;
    param_ptr->length = 2; 
    BtSendMsgToL4(param_ptr);      

}

void Send_AVCTP_Event_MessageRecReq_Result(U8 result)
{
     // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  
    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x06; //AVCTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVCTP_Event_MessageRecReq_Result; //TC_AVCTP_Event_MessageRecReq_Result
    param_ptr->test_cmd_rsp_buf[5]= result;
    param_ptr->length = 2; 
    BtSendMsgToL4(param_ptr);      

}

void Send_AVCTP_Event_MessageRecRsp_Result(U8 result)
{
     // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  
    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x06; //AVCTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVCTP_Event_MessageRecRsp_Result; //TC_AVCTP_Event_MessageRecRsp_Result
    param_ptr->test_cmd_rsp_buf[5]= result;
    param_ptr->length = 2; 
    BtSendMsgToL4(param_ptr);      

}


void AvctpTesterInit(void)
{
    BOOL status;
    AVCTP_DeregisterAllChnl();    
    AVCTP_Deinit();
    /* Initialize Memory */
    status = AVCTP_Init();
    avctpTesterChnl.data_offset = 0;
    avctpTesterChnl.special_state = 0;
    status = AVCTP_Register(&avctpTesterChnl.chnl, AvctpTesterCallback);
    if (status == BT_STATUS_SUCCESS) 
    {
        OS_MemSet((U8*)&avctpTesterChnl.cmgrHandler, 0, sizeof(CmgrHandler));
        //report success to Windows Tool
        Send_AVCTP_Event_Init(0);
    }
    else Send_AVCTP_Event_Init(1); //report error to Windows Tool
    return;
}

U8 AVCT_ConnectReq(AvctpConnectRequest *parms)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BD_ADDR bd_addr;
     
    /* Establish the ACL link */
    memcpy((U8 *)bd_addr.addr, (U8 *)parms->bd_addr,6);

    (void)CMGR_RegisterHandler(&avctpTesterChnl.cmgrHandler, AvctpTesterCmgrCallback);
    status = CMGR_CreateDataLink(&avctpTesterChnl.cmgrHandler, &bd_addr);
    if (status == BT_STATUS_SUCCESS) 
    {
        avctpTesterChnl.special_state = 1;        
        status = AVCTP_Connect(&avctpTesterChnl.chnl, avctpTesterChnl.cmgrHandler.remDev);
    }
    if(status == BT_STATUS_IN_USE)
    {
        avctpTesterChnl.special_state = 0;        
        Send_AVCTP_Event_ConnectReq_Result(0x01);
    }
    return status;
}

U8 AVCT_ConnectRsp(AvctpConnectResponse *parms)
{
    BtStatus status;
    if(parms->connect_result == 0x00)
        status = AVCTP_ConnectRsp(&avctpTesterChnl.chnl, TRUE);
    else
        status = AVCTP_ConnectRsp(&avctpTesterChnl.chnl, FALSE);
    Send_AVCTP_Event_ConnectRsp_Result(0x00);
    return status;
}

U8 AVCT_DisconnectReq(AvctpDisconnectRequest *parms)
{
    BtStatus status;
    status = AVCTP_Disconnect(&avctpTesterChnl.chnl);
    avctpTesterChnl.special_state = 1;
    Send_AVCTP_Event_DisConnectReq_Result(0x00);
    return status;
}

U8 AVCT_SendMessageReq(AvctpSendMessaegReq *parms)
{
    BtStatus status;
    AvctpRawFrame raw_data;
    avctpTesterChnl.raw_data.length = parms->length;
    avctpTesterChnl.raw_data.transaction_id = parms->transaction_id;
    avctpTesterChnl.raw_data.type = parms->type;
    avctpTesterChnl.raw_data.pid = parms->pid;
    memset((U8 *)avctpTesterChnl.data, 0x55, sizeof(avctpTesterChnl.data));
    avctpTesterChnl.raw_data.data = (U8 *)avctpTesterChnl.data;
    avctpTesterChnl.raw_data.length = parms->length;
    status = AVCTP_SendRawMessage(&avctpTesterChnl.chnl,&(avctpTesterChnl.raw_data));
    Send_AVCTP_Event_MessageRecReq_Result(0x00);
    return status;
}



/*---------------------------------------------------------------------------
 * AvCtpCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Callback function for AVCTP.
 *
 * Return:    void
 */
void AvctpTesterCallback(AvctpChannel *Chnl, AvctpCallbackParms *Parms)
{
/*    AvctpTesterChannel *chnl = &avctpTesterChnl; */

    // add by stanley
    U16 str_data_len = 0;
    local_para_struct* msg;
    bt_test_cmd_rsp_struct  *param_ptr;


    switch(Parms->event) 
    {
        case AVCTP_EVENT_CONNECT_IND:
            //add by stanley:2006-0826
            CMGR_CreateDataLink(&avctpTesterChnl.cmgrHandler, &(Parms->p.remDev->bdAddr));
            Send_AVCTP_Event_ConnectReq_Ind(Chnl,Parms);
            break;
        case AVCTP_EVENT_CONNECT:
            // 3rd parameter: Connect Result, 4th parameter: Config Result, 5th parameter: Status    
            if (avctpTesterChnl.special_state)
            Send_AVCTP_Event_ConnectReq_Cfm(Chnl,Parms,0x0000,0x0000,0x0000);
            avctpTesterChnl.special_state = 0;
            break;
            
        case AVCTP_EVENT_DISCONNECT:
            if (avctpTesterChnl.special_state)
                Send_AVCTP_Event_DisConnectReq_Cfm(Chnl,Parms);
            else
                Send_AVCTP_Event_DisConnectReq_Ind(Chnl,Parms);
            avctpTesterChnl.special_state = 0;
            break;
        case AVCTP_EVENT_RESPONSE:
            //add by stanley: 2006-0826 receiving rsp frame
            if((Chnl->rxRawType == AVTP_PACKET_TYPE_START) || (Chnl->rxRawType == AVTP_PACKET_TYPE_SINGLE))  
                avctpTesterChnl.data_offset = 0;
            avctpTesterChnl.current_operation=1;         
            OS_MemCopy((U8 *)avctpTesterChnl.data +avctpTesterChnl.data_offset, Chnl->rxRawData,Chnl->rxRawLength);            
            avctpTesterChnl.data_offset += Chnl->rxRawLength;
            if(!Chnl->rx.rspFrame.more)
                Send_AVCTP_Event_MessageRecReq_Ind(Chnl,Parms);
            break;

        case AVCTP_EVENT_COMMAND:
            if((Chnl->rxRawType == AVTP_PACKET_TYPE_START) || (Chnl->rxRawType == AVTP_PACKET_TYPE_SINGLE))  
                avctpTesterChnl.data_offset = 0;
            avctpTesterChnl.current_operation=0;         
            OS_MemCopy((U8 *)avctpTesterChnl.data+avctpTesterChnl.data_offset, Chnl->rxRawData,Chnl->rxRawLength);            
            avctpTesterChnl.data_offset += Chnl->rxRawLength;
            if(!Chnl->rx.cmdFrame.more)
                Send_AVCTP_Event_MessageRecReq_Ind(Chnl,Parms);

            break;
        case AVCTP_EVENT_OPERANDS:
            if((Chnl->rxRawType == AVTP_PACKET_TYPE_START) || 
              (Chnl->rxRawType == AVTP_PACKET_TYPE_SINGLE))  
                avctpTesterChnl.data_offset = 0;
            OS_MemCopy((U8 *)avctpTesterChnl.data +avctpTesterChnl.data_offset , Chnl->rxRawData,Chnl->rxRawLength);            
            avctpTesterChnl.data_offset += Chnl->rxRawLength;
            if(avctpTesterChnl.current_operation ==0)
            {
                if(!Chnl->rx.rspFrame.more)
                    Send_AVCTP_Event_MessageRecReq_Ind(Chnl,Parms);
            }
            else
            {
                if(!Chnl->rx.cmdFrame.more)
                    Send_AVCTP_Event_MessageRecReq_Ind(Chnl,Parms);
            }

            break;
        case AVCTP_EVENT_TX_DONE:
            break;
    }
}


void AvctpTesterCmgrCallback(CmgrHandler *cHandler, 
                      CmgrEvent Event, 
                      BtStatus Status)
{
    switch (Event) {

    case CMEVENT_DATA_LINK_CON_CNF:
        if (Status == BT_STATUS_SUCCESS) 
        {
            /* ACL is connected, start SDP Query */
            avctpTesterChnl.special_state =1;
            AVCTP_Connect(&avctpTesterChnl.chnl, cHandler->remDev);
            if (Status == BT_STATUS_PENDING) 
            {
                return;
            }
        }
        /* Disconnect and call the application */
        (void)CMGR_RemoveDataLink(&avctpTesterChnl.cmgrHandler);
        avctpTesterChnl.cmgrHandler.remDev = 0;
        avctpTesterChnl.cmgrHandler.bdc = 0;              
        break;
    }
}

void avctptester_handler(ilm_struct *ilm_ptr)
{
    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_AVCTP_ACTIVATE_REQ:
            AvctpTesterInit();
            break;
        case MSG_ID_BT_AVCTP_CONNECT_REQ:
            AVCT_ConnectReq((AvctpConnectRequest *)ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_AVCTP_DATA_REQ:
            AVCT_SendMessageReq((AvctpSendMessaegReq *)ilm_ptr->local_para_ptr);
            break;
    }
}

void avctp_handler_inject_msg(char *string)
{
    if(strncmp(string, "init",4)==0)
    {
        avctp_tester_sendMsg(
                                MSG_ID_BT_AVCTP_ACTIVATE_REQ,
                                MOD_BT,
                                BT_APP_SAP,
                                NULL,
                                NULL
                                );
    }
    else if(strncmp(string, "message",7)==0)
    {    
        AvctpSendMessaegReq*adp_msg;
        adp_msg = ( AvctpSendMessaegReq *)
        	                       construct_local_para (sizeof(AvctpSendMessaegReq),TD_UL);
        avctp_tester_sendMsg(
                                MSG_ID_BT_AVCTP_DATA_REQ,
                                MOD_BT,
                                BT_APP_SAP,
                                (local_para_struct*)adp_msg,
                                NULL
                                );
    
    }
    else if(strncmp(string, "connect",7)==0)
    {
        AvctpConnectRequest *adp_msg;
        adp_msg = ( AvctpConnectRequest *)
        	                       construct_local_para (sizeof(AvctpConnectRequest),TD_UL);
#if 0 /* Itech A2DP*/
        adp_msg->bd_addr[0] = 0xD7;
        adp_msg->bd_addr[1] = 0x8C;
        adp_msg->bd_addr[2] = 0x34;
        adp_msg->bd_addr[3] = 0x3C;
        adp_msg->bd_addr[4] = 0x0D;
        adp_msg->bd_addr[5] = 0x00;   
#endif         
        /* Samsung A2DP */
        adp_msg->bd_addr[0] = 0x6D;
        adp_msg->bd_addr[1] = 0x53;
        adp_msg->bd_addr[2] = 0xD0;
        adp_msg->bd_addr[3] = 0x60;
        adp_msg->bd_addr[4] = 0x10;
        adp_msg->bd_addr[5] = 0x00;   

        avctp_tester_sendMsg(
                                MSG_ID_BT_AVCTP_CONNECT_REQ,
                                MOD_BT,
                                BT_APP_SAP,
                                (local_para_struct*)adp_msg,
                                NULL
                                );
    }
      
}


#endif



