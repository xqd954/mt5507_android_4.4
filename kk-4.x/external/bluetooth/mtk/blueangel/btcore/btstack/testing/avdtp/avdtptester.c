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
#include "avdtp.h"
#include "sys/avalloc.h"
#include "sys/avsigmgr.h"
#include "Avdtptester.h"
#include "osapi.h"

//add by stanley 2006-08-14: if the bluetooth_struct.h is included in your C file, also the uart_sw.h must be included. 
#include "uart_sw.h"
#include "bluetooth_struct.h"
#include "bt_test_cmd.h"
#include "app_buff_alloc.h"


#ifdef __AVDTP_TESTER_
                                  
static AvdtpTesterStream ats;
BtAvdtpTesterContext avdtpTesterContext;
AvdtpCodec avdtp_tester_codec;

static U8 SbcSrcElements[] = {
    0x22,          /* One block size must be supported */
    0x15,
    02, 32
#if 0        
    AVDTP_TESTER_SBC_CODEC_INF_ELEMENT(
        0x22,          /* One block size must be supported */
        0x15,
        02, 59)
#endif        
};
#if 0
static U8 SbcSrcElements[] = {
    AVDTP_TESTER_SBC_CODEC_INF_ELEMENT(
        AVDTP_TESTER_SBC_CODEC_FREQ_48000 |    AVDTP_TESTER_SBC_CODEC_FREQ_44100 |
        AVDTP_TESTER_SBC_CODEC_FREQ_32000 |    AVDTP_TESTER_SBC_CODEC_FREQ_16000,
        AVDTP_TESTER_SBC_CODEC_CHNL_MODE_MONO |  AVDTP_TESTER_SBC_CODEC_CHNL_MODE_DUAL |
        AVDTP_TESTER_SBC_CODEC_CHNL_MODE_STEREO |AVDTP_TESTER_SBC_CODEC_CHNL_MODE_JOINT,
        AVDTP_TESTER_SBC_CODEC_BLOCKS_16 |        AVDTP_TESTER_SBC_CODEC_BLOCKS_12 |
        AVDTP_TESTER_SBC_CODEC_BLOCKS_8  |        AVDTP_TESTER_SBC_CODEC_BLOCKS_4,          /* One block size must be supported */
        AVDTP_TESTER_SBC_CODEC_SUBBANDS_8 |    AVDTP_TESTER_SBC_CODEC_SUBBANDS_4,            
        AVDTP_TESTER_SBC_CODEC_ALLOCATION_LOUDNESS  |   AVDTP_TESTER_SBC_CODEC_ALLOCATION_SNR,
        18, 53)
};
#endif

#define AVDTPTS(s) (avdtpTesterContext.s)
void avdtp_tester_sendMsg(
                                        msg_type msg, 
                                        module_type dstMod ,
                                        sap_type sap,
                                        local_para_struct *local_para,
                                        peer_buff_struct *peer_buff);

void avdtp_tester_sendMsg(
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

void AvdtpTesterInit()
{
    BOOL status;
    U8 i;
    OS_MemSet((U8 *)&avdtpTesterContext, 0, (U32)sizeof(BtAvdtpTesterContext));
    /* Initialize Device List */
    AVDTP_DeregisterAllChnl();
    InitializeListHead(&AVDTPTS(freeDeviceList));
    for (i = 0; i < 1; i++) {
        OS_MemSet((U8 *)&AVDTPTS(device[i]), 0, sizeof(AvdtpTesterDevice));
        InsertTailList(&AVDTPTS(freeDeviceList), &AVDTPTS(device[i]).node);
    }
    /* Initialize the Stream registration list */    
    InitializeListHead(&AVDTPTS(streamList));
    InitializeListHead(&AVDTPTS(inuseDeviceList));
    AVDTPTS(avdtpIssueCmd) = FALSE;
    /* Remove A2DP registration */
    AVDTPTS(buffer_offset) = 0;
    avdtp_tester_codec.codecType = AVDTP_CODEC_TYPE_SBC;
    avdtp_tester_codec.elemLen = sizeof(SbcSrcElements);
    avdtp_tester_codec.elements = SbcSrcElements;
    ats.type = AVDTP_TESTER_STREAM_TYPE_SOURCE;
    /* Register a channel with AVDTP */
    if (!IsListEmpty(&AVDTPTS(freeDeviceList))) 
    {
        AVDTPTS(currentDevice) = 
        (AvdtpTesterDevice *)RemoveHeadList(&AVDTPTS(freeDeviceList));
        status = AVDTP_Register(&AVDTPTS(currentDevice)->channel, 
                                AvdtpTesterCallback);
        if (status != BT_STATUS_SUCCESS) {
            InsertTailList(&AVDTPTS(freeDeviceList), 
                           &AVDTPTS(currentDevice)->node);
            //report error to Windows Tool
            Send_AVDTP_Event_Init(1);
            AVDTPTS(currentDevice) = 0;
            return;
        }
    }

    /* Register the stream with AVDTP */
    ats.stream.endPointType = ats.type;
    ats.stream.mediaType = AVDTP_MEDIA_TYPE_AUDIO;
    status = AVDTP_RegisterStream(&(ats.stream), &avdtp_tester_codec);
    if (status != BT_STATUS_SUCCESS) {
    /* Failed, deregister channel if necessary */
        (void)AVDTP_Deregister(&AVDTPTS(currentDevice)->channel);
        InsertTailList(&AVDTPTS(freeDeviceList), 
                       &AVDTPTS(currentDevice)->node);
        AVDTPTS(currentDevice) = 0;
        //report error to Windows Tool
        Send_AVDTP_Event_Init(1);
        return;
    }
    ats.stream.locStrmId = 1;
    /* Initialize the Stream */
    InitializeListHead(&ats.sbcQueue);
    ats.device = 0;
    ats.flags = 0;
    InsertTailList(&AVDTPTS(streamList), &(ats.node));
        AVDTPTS(state) = AVDTP_STRM_STATE_IDLE;

    /* Initialize Media Packet Header for SBC */
    ats.mediaHeader.version = 2;
    ats.mediaHeader.padding = 0;
    ats.mediaHeader.marker = 0;
    ats.mediaHeader.payloadType = 101;
    ats.mediaHeader.csrcCount = 0;
    ats.mediaHeader.sequenceNumber = 0;
    ats.mediaHeader.timestamp = (OS_Rand() << 16) + OS_Rand();
    ats.mediaHeader.ssrc = (OS_Rand() << 16) + OS_Rand();
    (void)CMGR_RegisterHandler(&AVDTPTS(currentDevice)->cmgrHandler, AvdtpTesterCmgrCallback);

    //report success to Windows Tool
    Send_AVDTP_Event_Init(0);
   
}


void Send_AVDTP_Event_Init(U8 register_result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Init; //TC_AVDTP_Event_Init
    param_ptr->test_cmd_rsp_buf[5]=register_result;
    param_ptr->length = 2; //fill 1 byte AVDTP_event_type
    

    BtSendMsgToL4(param_ptr);


}

void Send_AVDTP_Event_Registration_Req_Result(U8 register_result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    
     //add by stanley: 2006-0814
     param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
                                                            (U16)sizeof(bt_test_cmd_rsp_struct),
                                                            TD_CTRL);  
    
     memset(param_ptr->test_cmd_rsp_buf,0,256);
    
     param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
     param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
     param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Registration_Req_Result; //TC_AVDTP_Event_Registration_Req_Result
     param_ptr->test_cmd_rsp_buf[5]=register_result;
     param_ptr->length = 2; //fill 1 byte AVDTP_event_type
       
     BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_ConnectReq_Ind(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_ConnectReq_Ind; //TC_AVDTP_Event_ConnectReq_Ind
    memcpy(&param_ptr->test_cmd_rsp_buf[5] ,AVDTPTS(currentDevice)->cmgrHandler.remDev->bdAddr.addr,6);
    param_ptr->length = 7; //fill 1 byte AVDTP_event_type + 6 bytes BD_ADDR

    BtSendMsgToL4(param_ptr);


}


void Send_AVDTP_Event_ConnectReq_Cfm(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms, U16 connect_result, U16 config_result, U16 connec_req_status)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_ConnectReq_Cfm; //TC_AVDTP_Event_ConnectReq_Cfm
    memcpy(&param_ptr->test_cmd_rsp_buf[5] ,AVDTPTS(currentDevice)->cmgrHandler.remDev->bdAddr.addr,6);

    param_ptr->test_cmd_rsp_buf[11] = (U8)(connect_result & 0x00FF);
    param_ptr->test_cmd_rsp_buf[12] = (U8)((connect_result>>8) & 0x00FF);

    param_ptr->test_cmd_rsp_buf[13] = (U8)(config_result & 0x00FF);
    param_ptr->test_cmd_rsp_buf[14] = (U8)((config_result >> 8) & 0x00FF);

    param_ptr->test_cmd_rsp_buf[15] = (U8)(connec_req_status & 0x00FF);
    param_ptr->test_cmd_rsp_buf[16] = (U8)((connec_req_status >> 8) & 0x00FF);
    
    param_ptr->length = 13; 
    BtSendMsgToL4(param_ptr);



}

void Send_AVDTP_Event_DisConnectReq_Ind(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_DisConnectReq_Ind; //TC_AVDTP_Event_DisConnectReq_Ind
    memcpy(&param_ptr->test_cmd_rsp_buf[5] ,AVDTPTS(currentDevice)->cmgrHandler.remDev->bdAddr.addr,6);
    param_ptr->length = 7; //fill 1 byte AVDTP_event_type + 6 bytes BD_ADDR

    BtSendMsgToL4(param_ptr);



}


void Send_AVDTP_Event_DisConnectReq_Cfm(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_DisConnectReq_Cfm; //TC_AVDTP_Event_DisConnectReq_Cfm

    //Parms->p.remDev->bdAddr.addr
    memcpy(&param_ptr->test_cmd_rsp_buf[5] ,AVDTPTS(currentDevice)->cmgrHandler.remDev->bdAddr.addr,6);

    param_ptr->test_cmd_rsp_buf[11] = Parms->error; //fill this with errorCode
    param_ptr->test_cmd_rsp_buf[12] = 0x00;

    param_ptr->length = 9; 
    BtSendMsgToL4(param_ptr);


}

void Send_AVDTP_Event_Discovery_Ind(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;
    
     // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    
    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
                                                                                             (U16)sizeof(bt_test_cmd_rsp_struct),
                                                                                             TD_CTRL);  
    
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Discovery_Ind; //TC_AVDTP_Event_Discovery_Ind
    //using acpTransId?
    param_ptr->test_cmd_rsp_buf[5]= Parms->channel->sigChnl.acpTransId;
    memcpy(&param_ptr->test_cmd_rsp_buf[6] ,AVDTPTS(currentDevice)->cmgrHandler.remDev->bdAddr.addr,6);
    param_ptr->length = 8; 
    BtSendMsgToL4(param_ptr);

}


void Send_AVDTP_Event_Discovery_Cfm(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms, U8*  acp_seids_primary_data, U16 acp_seids_primary_data_len)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;
    U8 trans_id = ((Parms->channel->sigChnl.intTransId ==0) ? 16 : Parms->channel->sigChnl.intTransId-1);
    
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley: 2006-0814
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Discovery_Cfm; //TC_AVDTP_Event_Discovery_Cfm
    param_ptr->test_cmd_rsp_buf[5]= trans_id;
    param_ptr->test_cmd_rsp_buf[6]= Parms->error; //fill this with errorCode
    //raw data len of acp_seids_primary_data
    param_ptr->test_cmd_rsp_buf[7]= (U8)(acp_seids_primary_data_len & 0x00FF);
    param_ptr->test_cmd_rsp_buf[8]=	(U8)((acp_seids_primary_data_len >> 8) & 0x00FF); 

    if(acp_seids_primary_data_len !=0)    
        memcpy(&param_ptr->test_cmd_rsp_buf[9] ,acp_seids_primary_data,acp_seids_primary_data_len);
    param_ptr->length = 5 + acp_seids_primary_data_len; 
    BtSendMsgToL4(param_ptr);


}


void Send_AVDTP_Event_GetCapabilities_Ind(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{

    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
                                                                (U16)sizeof(bt_test_cmd_rsp_struct),
                                                                TD_CTRL);  
    
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_GetCapabilities_Ind; //TC_AVDTP_Event_GetCapabilities_Ind
    param_ptr->test_cmd_rsp_buf[5] = Parms->channel->sigChnl.acpTransId;
    memcpy(&param_ptr->test_cmd_rsp_buf[6] ,AVDTPTS(currentDevice)->cmgrHandler.remDev->bdAddr.addr,6);

    //ACP SEID is stream->remStrmId ??
    param_ptr->test_cmd_rsp_buf[12] = Parms->rawData[0] >> 2;
    param_ptr->length = 9; 
    BtSendMsgToL4(param_ptr);
    
}




void Send_AVDTP_Event_GetCapabilities_Cfm(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms, U8*  stream_capabilities, U16 stream_capabilities_len)
{

    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    U8 trans_id = ((Parms->channel->sigChnl.intTransId ==0) ? 16 : Parms->channel->sigChnl.intTransId-1);

    //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
                                                                (U16)sizeof(bt_test_cmd_rsp_struct),
                                                                TD_CTRL);  
    
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_GetCapabilities_Cfm; //TC_AVDTP_Event_GetCapabilities_Cfm
    param_ptr->test_cmd_rsp_buf[5] = trans_id;
    param_ptr->test_cmd_rsp_buf[6] = Parms->error; //Event error code
    //raw data len of stream_capabilities
    param_ptr->test_cmd_rsp_buf[7]= (U8)(stream_capabilities_len & 0x00FF);
    param_ptr->test_cmd_rsp_buf[8]=	(U8)((stream_capabilities_len >> 8) & 0x00FF); 
    
    //inx 9- : List of Stream Capabilities
    if(stream_capabilities_len !=0)
        memcpy(&param_ptr->test_cmd_rsp_buf[9],stream_capabilities,stream_capabilities_len);
    
    param_ptr->length = 5+stream_capabilities_len;
    BtSendMsgToL4(param_ptr);


}

void Send_AVDTP_Event_SetConfig_Ind(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms, U8* config_parameters, U16 config_parameters_len)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_SetConfig_Ind; //AVDTP_Event_Set_Config_Ind
    param_ptr->test_cmd_rsp_buf[5]=Parms->channel->sigChnl.acpTransId;
    memcpy(&param_ptr->test_cmd_rsp_buf[6],AVDTPTS(currentDevice)->cmgrHandler.remDev->bdAddr.addr,6);
    
    //ACP SEID
    param_ptr->test_cmd_rsp_buf[12] = stream->stream.remStrmId;
    //INT SEID
    param_ptr->test_cmd_rsp_buf[13] = stream->stream.locStrmId;
    //stream handle    
    param_ptr->test_cmd_rsp_buf[14] = stream->stream.locStrmId;
    param_ptr->test_cmd_rsp_buf[15] = 0x00;
    //raw data len of config_parameters
    param_ptr->test_cmd_rsp_buf[16] = (U8)(config_parameters_len & 0x00FF);
    param_ptr->test_cmd_rsp_buf[17] = (U8)((config_parameters_len>>8) & 0x00FF);
	
    //18-
    memcpy(&param_ptr->test_cmd_rsp_buf[18], config_parameters,config_parameters_len);
    param_ptr->length = 14 + config_parameters_len;
    BtSendMsgToL4(param_ptr);
}



void Send_AVDTP_Event_SetConfig_Cfm(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms, U8*  stream_config_category, U16 stream_config_category_len)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;
    U8 trans_id = ((Parms->channel->sigChnl.intTransId ==0) ? 16 : Parms->channel->sigChnl.intTransId-1);

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_SetConfig_Cfm; //AVDTP_Event_Set_Config_Cfm
    param_ptr->test_cmd_rsp_buf[5] = trans_id;
    param_ptr->test_cmd_rsp_buf[6] = Parms->error; //Event error code
     //raw data len of stream_config_category
    param_ptr->test_cmd_rsp_buf[7]= (U8)(stream_config_category_len & 0x00FF);
    param_ptr->test_cmd_rsp_buf[8]=	(U8)((stream_config_category_len >> 8) & 0x00FF); 
	
	
    //inx 9- : First Stream Config category to fail
    memcpy(&param_ptr->test_cmd_rsp_buf[9],stream_config_category,stream_config_category_len);
    
    param_ptr->length = 5+stream_config_category_len;

    BtSendMsgToL4(param_ptr);
    

}


void Send_AVDTP_Event_GetConfig_Ind(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_SetConfig_Cfm; //AVDTP_Event_Set_Config_Cfm
    param_ptr->test_cmd_rsp_buf[5] = Parms->channel->sigChnl.acpTransId;     

    // stream handle is stream->stream.locStrmId ?
    param_ptr->test_cmd_rsp_buf[6]=stream->stream.locStrmId;
    param_ptr->test_cmd_rsp_buf[7]=0x00;
    param_ptr->length = 4;

    BtSendMsgToL4(param_ptr);
    



}


void Send_AVDTP_Event_GetConfig_Cfm(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms, U8*  stream_config_params, U16 stream_config_params_len)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;
    U8 trans_id = ((Parms->channel->sigChnl.intTransId ==0) ? 16 : Parms->channel->sigChnl.intTransId-1);

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_GetConfig_Cfm; //TC_AVDTP_Event_GetConfig_Cfm
    param_ptr->test_cmd_rsp_buf[5] = trans_id;
    //Note: AVDT_GetConfig()-> AvdtpSigStreamGetConfig() : assign stream to currentTxStream
    //only streamID (1 byte) , no stream handle(2 bytes)
    param_ptr->test_cmd_rsp_buf[6] = Parms->channel->currentTxStream->remStrmId;
    param_ptr->test_cmd_rsp_buf[7] = 0x00; 
    param_ptr->test_cmd_rsp_buf[8] = Parms->error; //Event error code            
    // raw data len of stream_config_params
    param_ptr->test_cmd_rsp_buf[9] = (U8)(stream_config_params_len & 0x00FF); 
    param_ptr->test_cmd_rsp_buf[10] = (U8)((stream_config_params_len>>8) & 0x00FF);

                
    //inx 11- : List of Stream Config parameters 
    if(stream_config_params_len !=0)
        memcpy(&param_ptr->test_cmd_rsp_buf[11], stream_config_params, stream_config_params_len);
    
    param_ptr->length = 7+stream_config_params_len;
    BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_Open_Ind(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{

    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    
    //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
                                                               (U16)sizeof(bt_test_cmd_rsp_struct),
                                                               TD_CTRL);  
    
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Open_Ind; //AVDTP_Event_Open_Ind
    param_ptr->test_cmd_rsp_buf[5] = Parms->channel->sigChnl.acpTransId;
    
    // stream handle
    param_ptr->test_cmd_rsp_buf[6] = Parms->stream->locStrmId;
    param_ptr->test_cmd_rsp_buf[7] = 0x00; 
    
    param_ptr->length = 4;
    BtSendMsgToL4(param_ptr);



}


void Send_AVDTP_Event_Open_Cfm(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{

    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;
    U8 trans_id = ((Parms->channel->sigChnl.intTransId ==0) ? 16 : Parms->channel->sigChnl.intTransId-1);

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    
    //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
                                                               (U16)sizeof(bt_test_cmd_rsp_struct),
                                                               TD_CTRL);  
    
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Open_Cfm; //TC_AVDTP_Event_Open_Cfm
    param_ptr->test_cmd_rsp_buf[5] = trans_id;
    
    //Error code
    param_ptr->test_cmd_rsp_buf[6] = Parms->error;
    
    param_ptr->length = 3;
    BtSendMsgToL4(param_ptr);



}

void Send_AVDTP_Event_Start_Ind(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													   (U16)sizeof(bt_test_cmd_rsp_struct),
												          TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Start_Ind; //AVDTP_Event_Start_Ind
    param_ptr->test_cmd_rsp_buf[5] = Parms->channel->sigChnl.acpTransId;

    //inx 6-9 : List of stream handles
    //1st stream handle
    param_ptr->test_cmd_rsp_buf[6]= Parms->stream->locStrmId;
    param_ptr->test_cmd_rsp_buf[7]= 0x00;

    param_ptr->length = 4;

    BtSendMsgToL4(param_ptr);



}


void Send_AVDTP_Event_Start_Cfm(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;
    U8 trans_id = ((Parms->channel->sigChnl.intTransId ==0) ? 16 : Parms->channel->sigChnl.intTransId-1);

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													   (U16)sizeof(bt_test_cmd_rsp_struct),
												          TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Start_Cfm; //TC_AVDTP_Event_Start_Cfm
    param_ptr->test_cmd_rsp_buf[5] = trans_id;

    //inx 6-7 : List of stream handles
    param_ptr->test_cmd_rsp_buf[6]= Parms->stream->locStrmId;
    param_ptr->test_cmd_rsp_buf[7]= 0x00;

    //Error code
    param_ptr->test_cmd_rsp_buf[8]= Parms->error;

    param_ptr->length = 5;

    BtSendMsgToL4(param_ptr);



}


void Send_AVDTP_Event_Close_Ind(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

     //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Close_Ind; //TC_AVDTP_Event_Close_Ind
    param_ptr->test_cmd_rsp_buf[5] = Parms->channel->sigChnl.acpTransId;
            
    // inx 6-7: stream handle
    param_ptr->test_cmd_rsp_buf[6] =  Parms->stream->locStrmId;
    param_ptr->test_cmd_rsp_buf[7] = 0x00;
                
    param_ptr->length = 4;

    BtSendMsgToL4(param_ptr);



}


void Send_AVDTP_Event_Close_Cfm(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;
    U8 trans_id = ((Parms->channel->sigChnl.intTransId ==0) ? 16 : Parms->channel->sigChnl.intTransId-1);

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

     //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Close_Cfm; //TC_AVDTP_Event_Close_Cfm
    param_ptr->test_cmd_rsp_buf[5] = trans_id;
            
    // inx 6: Error code
    param_ptr->test_cmd_rsp_buf[6] =  Parms->error;
                
    param_ptr->length = 3;

    BtSendMsgToL4(param_ptr);



}

void Send_AVDTP_Event_Suspend_Ind(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{

    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Suspend_Ind; //AVDTP_Event_Suspend_Ind
    param_ptr->test_cmd_rsp_buf[5] = Parms->channel->sigChnl.acpTransId;
            
    // inx 6-7: stream handle
    param_ptr->test_cmd_rsp_buf[6] =  Parms->stream->locStrmId;
    param_ptr->test_cmd_rsp_buf[7] = 0x00; 
                
    param_ptr->length = 4;

    BtSendMsgToL4(param_ptr);




}

void Send_AVDTP_Event_Suspend_Cfm(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;
    U8 trans_id = ((Parms->channel->sigChnl.intTransId ==0) ? 16 : Parms->channel->sigChnl.intTransId-1);

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Suspend_Cfm; //AVDTP_Event_Suspend_Cfm
    param_ptr->test_cmd_rsp_buf[5] = trans_id;
            
    // inx 6-7: stream handle
    param_ptr->test_cmd_rsp_buf[6] =  Parms->stream->locStrmId;
    param_ptr->test_cmd_rsp_buf[7] = 0x00;

    param_ptr->test_cmd_rsp_buf[8] = Parms->error; //Event error code
                
    param_ptr->length = 5;

    BtSendMsgToL4(param_ptr);
                
}


void Send_AVDTP_Event_ReConfig_Ind(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms, U8* reconfig_params, U16 reconfig_params_len)
{
    
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;
    
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    //add by stanley 2006-0814     
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
															(U16)sizeof(bt_test_cmd_rsp_struct),
															TD_CTRL);  

    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_ReConfig_Ind; //AVDTP_Event_ReConfig_Ind
    param_ptr->test_cmd_rsp_buf[5] = Parms->channel->sigChnl.acpTransId;
            
    // inx 6-7: stream handle
   param_ptr->test_cmd_rsp_buf[6] =  Parms->stream->locStrmId;
   param_ptr->test_cmd_rsp_buf[7] = 0x00; 

    //raw data len of reconfig_params
    param_ptr->test_cmd_rsp_buf[8] = (U8)(reconfig_params_len & 0x00FF); 
    param_ptr->test_cmd_rsp_buf[9] = (U8)((reconfig_params_len >>8) & 0x00FF);
   


    // inx 10- : List of Reconfig stream config parameters
    memcpy(&param_ptr->test_cmd_rsp_buf[10],reconfig_params,reconfig_params_len);

    param_ptr->length = 6+reconfig_params_len;

   BtSendMsgToL4(param_ptr);
                
}


void Send_AVDTP_Event_ReConfig_Cfm(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms, U16 stream_config_category)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;
    U8 trans_id = ((Parms->channel->sigChnl.intTransId ==0) ? 16 : Parms->channel->sigChnl.intTransId-1);
    
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);  
    memset(param_ptr->test_cmd_rsp_buf,0,256);

    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_ReConfig_Cfm; //AVDTP_Event_ReConfig_Cfm
    param_ptr->test_cmd_rsp_buf[5] = trans_id;
            
    param_ptr->test_cmd_rsp_buf[6] = (U8) (stream_config_category & 0x00FF);
    param_ptr->test_cmd_rsp_buf[7] = (U8) ((stream_config_category >> 8) & 0x00FF);   
    param_ptr->test_cmd_rsp_buf[8] = Parms->error; //errCode

    param_ptr->length = 5;

    BtSendMsgToL4(param_ptr);

}


void Send_AVDTP_Event_Abort_Ind(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{
    
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;
       
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
 
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);   	   
	
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Abort_Ind; //TC_AVDTP_Event_Abort_Ind
    param_ptr->test_cmd_rsp_buf[5] = Parms->channel->sigChnl.acpTransId;

    // How to fill this field stream handle?            
    param_ptr->test_cmd_rsp_buf[6] = (U8) (stream->stream.remStrmId & 0x00FF);
    param_ptr->test_cmd_rsp_buf[7] = (U8) ((stream->stream.remStrmId >> 8) & 0x00FF);   
    
    param_ptr->length = 4;
    
    BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_Abort_Cfm(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;
    U8 trans_id = ((Parms->channel->sigChnl.intTransId ==0) ? 16 : Parms->channel->sigChnl.intTransId-1);

    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);  
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Abort_Cfm; //TC_AVDTP_Event_Abort_Cfm
    param_ptr->test_cmd_rsp_buf[5] = trans_id;
    param_ptr->test_cmd_rsp_buf[6] = Parms->error;

    param_ptr->length = 3;
    
    BtSendMsgToL4(param_ptr);


}


void Send_AVDTP_Event_ConnectReq_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_ConnectReq_Result; //TC_AVDTP_Event_ConnectReq_Result
    param_ptr->test_cmd_rsp_buf[5] = result;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);

}
   
void Send_AVDTP_Event_ConnectRsp_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);         
    memset(param_ptr->test_cmd_rsp_buf,0,256);
       
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_ConnectRsp_Result; //TC_AVDTP_Event_ConnectRsp_Result
    param_ptr->test_cmd_rsp_buf[5] = result;
   
    param_ptr->length = 2;
       
    BtSendMsgToL4(param_ptr);
}
      
void Send_AVDTP_Event_DisConnectReq_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_DisConnectReq_Result; //TC_AVDTP_Event_DisConnectReq_Result
    param_ptr->test_cmd_rsp_buf[5] = result;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);


}

void Send_AVDTP_Event_DisConnectRsp_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_DisConnectRsp_Result; //TC_AVDTP_Event_DisConnectRsp_Result
    param_ptr->test_cmd_rsp_buf[5] = result;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);


}


void Send_AVDTP_Event_DiscoveryReq_Result(AvdtpStreamDiscoverRequestResult *parms)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_DiscoveryReq_Result; //TC_AVDTP_Event_DiscoveryReq_Result
    param_ptr->test_cmd_rsp_buf[5] = parms->result;
    param_ptr->test_cmd_rsp_buf[6] = parms->transaction;    

    param_ptr->length = 3;
    
    BtSendMsgToL4(param_ptr);


}

void Send_AVDTP_Event_DiscoveryRsp_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_DiscoveryRsp_Result; //TC_AVDTP_Event_DiscoveryRsp_Result
    param_ptr->test_cmd_rsp_buf[5] = result;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);


}


void Send_AVDTP_Event_Get_CapabilitiesReq_Result(AvdtpStreamGetCapabilitiesRequestResult *parms)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;

    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Get_CapabilitiesReq_Result; //TC_AVDTP_Event_Get_CapabilitiesReq_Result
    param_ptr->test_cmd_rsp_buf[5] = parms->result;
    param_ptr->test_cmd_rsp_buf[6] = parms->transaction;    

    param_ptr->length = 3;
    
    BtSendMsgToL4(param_ptr);


}

void Send_AVDTP_Event_Get_CapabilitiesRsp_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Get_CapabilitiesRsp_Result; //TC_AVDTP_Event_Get_CapabilitiesRsp_Result
    param_ptr->test_cmd_rsp_buf[5] = result; 
    param_ptr->test_cmd_rsp_buf[6] = 0x01; //SEID value	

    param_ptr->length = 3;
    
    BtSendMsgToL4(param_ptr);


}


void Send_AVDTP_Event_Set_ConfigReq_Result(AvdtpStreamSetConfigurationRequestResult *parms)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Set_ConfigReq_Result; //TC_AVDTP_Event_Set_ConfigReq_Result
    param_ptr->test_cmd_rsp_buf[5] = parms->result;
    param_ptr->test_cmd_rsp_buf[6] = parms->transaction;    
    param_ptr->test_cmd_rsp_buf[7] = (U8)(parms->stream_handle & 0x00FF);    
    param_ptr->test_cmd_rsp_buf[8] = (U8)((parms->stream_handle>>8) & 0x00FF);    


    param_ptr->length = 5;
    
    BtSendMsgToL4(param_ptr);


}

void Send_AVDTP_Event_Set_ConfigRsp_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Set_ConfigRsp_Result; //TC_AVDTP_Event_Set_ConfigRsp_Result
    param_ptr->test_cmd_rsp_buf[5] = result;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);


}


void Send_AVDTP_Event_Get_ConfigRsp_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_Get_ConfigRsp_Result; //TC_AVDTP_Event_Get_ConfigRsp_Result
    param_ptr->test_cmd_rsp_buf[5] = result;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);


}

void Send_AVDTP_Event_OpenReq_Result(AvdtpStreamOpenRequestResult *parms)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_OpenReq_Result; //TC_AVDTP_Event_OpenReq_Result
    param_ptr->test_cmd_rsp_buf[5] = parms->result;
    param_ptr->test_cmd_rsp_buf[6] = parms->transaction;    

    param_ptr->length = 3;
    
    BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_OpenRsp_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_OpenRsp_Result; //TC_AVDTP_Event_OpenRsp_Result
    param_ptr->test_cmd_rsp_buf[5] = result;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_CloseReq_Result(AvdtpStreamCloseRequestResult *parms)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_CloseReq_Result; //TC_AVDTP_Event_CloseReq_Result
    param_ptr->test_cmd_rsp_buf[5] = parms->result;
    param_ptr->test_cmd_rsp_buf[6] = parms->transaction;    

    param_ptr->length = 3;
    
    BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_CloseRsp_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_CloseRsp_Result; //TC_AVDTP_Event_CloseRsp_Result
    param_ptr->test_cmd_rsp_buf[5] = result;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_StartReq_Result(AvdtpStreamStartRequestResult *parms)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_StartReq_Result; //TC_AVDTP_Event_StartReq_Result
    param_ptr->test_cmd_rsp_buf[5] = parms->result;
    param_ptr->test_cmd_rsp_buf[6] = parms->transaction;

    param_ptr->length = 3;
    
    BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_StartRsp_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_StartRsp_Result; //TC_AVDTP_Event_StartRsp_Result
    param_ptr->test_cmd_rsp_buf[5] = result;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_SuspendReq_Result(AvdtpStreamSuspendRequestResult *parms)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_SuspendReq_Result; //TC_AVDTP_Event_SuspendReq_Result
    param_ptr->test_cmd_rsp_buf[5] = parms->result;
    param_ptr->test_cmd_rsp_buf[6] = parms->transaction;    

    param_ptr->length = 3;
    
    BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_SuspendRsp_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_SuspendRsp_Result; //TC_AVDTP_Event_SuspendRsp_Result
    param_ptr->test_cmd_rsp_buf[5] = result;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);

}


void Send_AVDTP_Event_ReConfigReq_Result(AvdtpStreamReConfigurationRequestResult *parms)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_ReConfigReq_Result; //TC_AVDTP_Event_ReConfigReq_Result
    param_ptr->test_cmd_rsp_buf[5] = parms->result;
    param_ptr->test_cmd_rsp_buf[6] = parms->transaction;
    param_ptr->length = 3;
    
    BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_ReConfigRsp_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_ReConfigRsp_Result; //TC_AVDTP_Event_ReConfigRsp_Result
    param_ptr->test_cmd_rsp_buf[5] = result;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_AbortReq_Result(AvdtpStreamAbortRequestResult *parms)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_AbortReq_Result; //TC_AVDTP_Event_AbortReq_Result
    param_ptr->test_cmd_rsp_buf[5] = parms->result;
    param_ptr->test_cmd_rsp_buf[6] = parms->transaction;

    param_ptr->length = 3;
    
    BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_WriteReq_Result(AvdtpStreamDataRequestResult *parms)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_WriteReq_Result; //TC_AVDTP_Event_AbortReq_Result
    param_ptr->test_cmd_rsp_buf[5] = parms->result;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);

}

void Send_AVDTP_Event_AbortRsp_Result(U8 result)
{
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_AbortRsp_Result; //TC_AVDTP_Event_AbortRsp_Result
    param_ptr->test_cmd_rsp_buf[5] = result;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);

}


void Send_AVDTP_Event_ReadStreamDataReq_Result(AvdtpReadDataRequestResult *parms)
{
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]=TC_AVDTP_Event_ReadReq_Result; //TC_AVDTP_Event_AbortRsp_Result
    //result 
    param_ptr->test_cmd_rsp_buf[5] = parms->result;

    //stream handle
    param_ptr->test_cmd_rsp_buf[6] = (U8)(parms->stream_handle & 0x00FF);
    param_ptr->test_cmd_rsp_buf[7] = (U8)(parms->stream_handle>>8 & 0x00FF);

    //len of read data
    param_ptr->test_cmd_rsp_buf[8] = (U8)(parms->length & 0x00FF);
    param_ptr->test_cmd_rsp_buf[9] = (U8)((parms->length>>8) & 0x00FF);
    //readed data (inbuffer)
    if(parms->length !=0)
        memcpy(&param_ptr->test_cmd_rsp_buf[10],parms->inbuffer,parms->length);
    
    //time info	
    param_ptr->test_cmd_rsp_buf[10+parms->length] = (U8)(parms->time_info & 0x000000FF);
    param_ptr->test_cmd_rsp_buf[11+parms->length] = (U8)((parms->time_info>>8) & 0x000000FF);
    param_ptr->test_cmd_rsp_buf[12+parms->length] = (U8)((parms->time_info>>16) & 0x000000FF);
    param_ptr->test_cmd_rsp_buf[13+parms->length] = (U8)((parms->time_info>>24) & 0x000000FF);
    //marker
    param_ptr->test_cmd_rsp_buf[14+parms->length] = (parms->marker & 0x00FF);
    param_ptr->test_cmd_rsp_buf[15+parms->length] = ((parms->marker>>8) & 0x00FF);
  
    //payload type
    param_ptr->test_cmd_rsp_buf[16+parms->length] = parms->payload_type;
    //reliability	
    param_ptr->test_cmd_rsp_buf[17+parms->length] = parms->reliability;
    param_ptr->length = 14+parms->length;
   
    BtSendMsgToL4(param_ptr);


}


void Send_AVDTP_Event_Reject_Result(U8 event_opcode, U8 errcode)
{
	
    // add by stanley
    bt_test_cmd_rsp_struct  *param_ptr;
    param_ptr = (bt_test_cmd_rsp_struct *)construct_local_para(
													    (U16)sizeof(bt_test_cmd_rsp_struct),
													    TD_CTRL);      
    memset(param_ptr->test_cmd_rsp_buf,0,256);
    
    param_ptr->test_cmd_rsp_buf[0]=0x02; //event type
    param_ptr->test_cmd_rsp_buf[1]=0x05; //AVDTP category
    param_ptr->test_cmd_rsp_buf[4]= event_opcode; 
    param_ptr->test_cmd_rsp_buf[5] = errcode;

    param_ptr->length = 2;
    
    BtSendMsgToL4(param_ptr);


}



U8 AVDT_ConnectReq(AvdtpConnectRequest *parms)
{
    BtStatus status = BT_STATUS_SUCCESS;
    AvdtpTesterDevice *device;
    BD_ADDR bd_addr;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_CONNECT_REQ);
    /* Establish the ACL link */
    memcpy((U8 *)bd_addr.addr, (U8 *)parms->bd_addr,6);
    device = AVDTPTS(currentDevice);
    status =  AvdtpTesterCreateAclLink(&bd_addr, &device->cmgrHandler, AvdtpTesterCmgrCallback);
    if (status == BT_STATUS_PENDING) 
    {
        /* Connection started */
        InsertTailList(&AVDTPTS(inuseDeviceList), &device->node);
    } 
    else if (status == BT_STATUS_SUCCESS) 
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_CREATE_CONN);
    }
    else if (status == BT_STATUS_PENDING) 
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_FAILED_CONN);
        return AVDTP_RSP_REJECT;        
    }
    if((status == BT_STATUS_PENDING) || (status == BT_STATUS_SUCCESS))
    {
        /* Initialize the device */
        device->currentStream = &ats;
        device->flags = AVDTP_TESTER_DEV_FLAG_OUTGOING;
        ats.device = device;
    }
    return AVDTP_RSP_ACCEPT;
}

U8 AVDT_Discover_Req(AvdtpStreamDiscoverRequest *parms)
{
    AvdtpChannel *Chnl;
    BtStatus status = BT_STATUS_FAILED;
    AvdtpStreamDiscoverRequestResult rsp_parms;
    
    Chnl = &AVDTPTS(currentDevice)->channel;
    status =  AVDTP_DiscoverStreams(Chnl);

    rsp_parms.result = 0x00;
    rsp_parms.transaction = Chnl->sigChnl.intTransId;
    Send_AVDTP_Event_DiscoveryReq_Result(&rsp_parms);
    if(status != BT_STATUS_PENDING)
    {
        parms->transaction = 0x00;
        return AVDTP_RSP_REJECT;
    }
    else
    {
        parms->transaction = Chnl->sigChnl.intTransId;
        return AVDTP_RSP_ACCEPT;
    }
}

U8 AVDT_Get_Capabilities_Req(AvdtpStreamGetCapabilitiesRequest *parms)
{
    AvdtpChannel *Chnl;
    BtStatus status = BT_STATUS_FAILED;
    AvdtpStreamGetCapabilitiesRequestResult rsp_parms;
    
    Chnl = &AVDTPTS(currentDevice)->channel;
    status =  AVDTP_GetCapabilities(Chnl, parms->acp_seid);
    rsp_parms.result = 0x00;
    rsp_parms.transaction = Chnl->sigChnl.intTransId;
    Send_AVDTP_Event_Get_CapabilitiesReq_Result(&rsp_parms);
    if(status != BT_STATUS_PENDING)
    {
        parms->transaction = 0x00;
        return AVDTP_RSP_REJECT;
    }
    else
    {
        parms->transaction = Chnl->sigChnl.intTransId;
        return AVDTP_RSP_ACCEPT;
    }
}

U8 AVDT_Set_Configuration_Req(AvdtpStreamSetConfigurationRequest *parms)
{
    BtStatus status = BT_STATUS_FAILED;
    AvdtpChannel *Chnl;
    AvdtpStreamSetConfigurationRequestResult rsp_parms;

    Chnl = &AVDTPTS(currentDevice)->channel;
    avdtp_tester_codec.codecType = parms->codecType;
    avdtp_tester_codec.elemLen = parms->element_len;
    memcpy(avdtp_tester_codec.elements, parms->element, sizeof(SbcSrcElements));
    
    status =  AVDTP_SetConfig(Chnl, &(ats.stream), parms->acp_seid, &avdtp_tester_codec);
    rsp_parms.result = 0x00;
    rsp_parms.transaction = Chnl->sigChnl.intTransId;
    rsp_parms.stream_handle = ats.stream.locStrmId;
    Send_AVDTP_Event_Set_ConfigReq_Result(&rsp_parms);
    
    if(status != BT_STATUS_PENDING)
    {
        parms->transaction = 0x00;
        return AVDTP_RSP_REJECT;
    }
    else
    {
        parms->transaction = Chnl->sigChnl.intTransId;
        AVDTPTS(state) = AVDTP_STRM_STATE_CONFIGURED;
        return AVDTP_RSP_ACCEPT;
    }
}

U8 AVDT_ReConfigure_Req(AvdtpStreamReConfigurationRequest *parms)
{
    BtStatus status = BT_STATUS_FAILED;
    AvdtpChannel *Chnl;
    AvdtpStreamReConfigurationRequestResult rsp_parms;

    Chnl = &AVDTPTS(currentDevice)->channel;
    if(parms->stream_handle != ats.stream.locStrmId)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_RECONFIG_ERR_HDL);
        return AVDTP_RSP_REJECT;
    }
    avdtp_tester_codec.codecType = parms->codecType;
    avdtp_tester_codec.elemLen = parms->element_len;
    memcpy(avdtp_tester_codec.elements, parms->element, sizeof(SbcSrcElements));
    
    status =  AVDTP_Reconfig(&(ats.stream), &avdtp_tester_codec);
    rsp_parms.result = 0x00;
    rsp_parms.transaction = Chnl->sigChnl.intTransId;
    Send_AVDTP_Event_ReConfigReq_Result(&rsp_parms);

    if(status != BT_STATUS_PENDING)
    {
        parms->transaction = 0x00;
        return AVDTP_RSP_REJECT;
    }
    else
    {
        parms->transaction = Chnl->sigChnl.intTransId;
        return AVDTP_RSP_ACCEPT;
    }
}

U8 AVDT_Open_Req(AvdtpStreamOpenRequest *parms)
{
    BtStatus status = BT_STATUS_FAILED;
    AvdtpChannel *Chnl;
    AvdtpStreamOpenRequestResult rsp_parms;

    Chnl = &AVDTPTS(currentDevice)->channel;
    status =  AVDTP_OpenStream(Chnl, &(ats.stream));
    rsp_parms.result = 0x00;
    rsp_parms.transaction = Chnl->sigChnl.intTransId;
    Send_AVDTP_Event_OpenReq_Result(&rsp_parms);
    AVDTPTS(avdtpIssueCmd) = TRUE;

    if(status != BT_STATUS_PENDING)
    {
        parms->transaction = 0x00;
        return AVDTP_RSP_REJECT;
    }
    else
    {
        parms->transaction = Chnl->sigChnl.intTransId;
        return AVDTP_RSP_ACCEPT;
    }
}

U8 AVDT_Close_Req(AvdtpStreamCloseRequest *parms)
{
    BtStatus status = BT_STATUS_FAILED;    
    AvdtpChannel *Chnl;
    AvdtpStreamCloseRequestResult rsp_parms;

    Chnl = &AVDTPTS(currentDevice)->channel;
    if(parms->stream_handle != ats.stream.locStrmId)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_CLOSE_ERR_HDL);
        return AVDTP_RSP_REJECT;
    }        
    status =  AVDTP_CloseStream(&(ats.stream));
    rsp_parms.result = 0x00;
    rsp_parms.transaction = Chnl->sigChnl.intTransId;
    Send_AVDTP_Event_CloseReq_Result(&rsp_parms);    
    AVDTPTS(avdtpIssueCmd) = TRUE;

    if(status != BT_STATUS_PENDING)
    {
        parms->transaction = 0x00;
        return AVDTP_RSP_REJECT;
    }
    else
    {
        parms->transaction = Chnl->sigChnl.intTransId;
        return AVDTP_RSP_ACCEPT;
    }
}

U8 AVDT_Start_Req(AvdtpStreamStartRequest *parms)
{
    BtStatus status = BT_STATUS_FAILED;
    AvdtpChannel *Chnl;
    AvdtpStreamStartRequestResult rsp_parms;

    Chnl = &AVDTPTS(currentDevice)->channel;
    if(parms->stream_handle[0] != ats.stream.locStrmId)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_START_ERR_HDL);
        return AVDTP_RSP_REJECT;
    }    
    status =  AVDTP_StartStream(&(ats.stream));
    rsp_parms.result = 0x00;
    rsp_parms.transaction = Chnl->sigChnl.intTransId;
    Send_AVDTP_Event_StartReq_Result(&rsp_parms);        
    AVDTPTS(avdtpIssueCmd) = TRUE;

    if(status != BT_STATUS_PENDING)
    {
        parms->transaction = 0x00;
        return AVDTP_RSP_REJECT;
    }
    else
    {
        parms->transaction = Chnl->sigChnl.intTransId;
        return AVDTP_RSP_ACCEPT;
    }
}

U8 AVDT_Suspend_Req(AvdtpStreamSuspendRequest *parms)
{
    BtStatus status = BT_STATUS_FAILED;
    AvdtpChannel *Chnl;
    AvdtpStreamSuspendRequestResult rsp_parms;

    Chnl = &AVDTPTS(currentDevice)->channel;    
    if(parms->stream_handle[0] != ats.stream.locStrmId)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_SUSPEND_ERR_HDL);
        return AVDTP_RSP_REJECT;
    }        
    status =  AVDTP_SuspendStream(&(ats.stream));
    rsp_parms.result = 0x00;
    rsp_parms.transaction = Chnl->sigChnl.intTransId;
    Send_AVDTP_Event_SuspendReq_Result(&rsp_parms);     
    AVDTPTS(avdtpIssueCmd) = TRUE;
    
    if(status != BT_STATUS_PENDING)
    {
        parms->transaction = 0x00;
        return AVDTP_RSP_REJECT;
    }
    else
    {
        parms->transaction = Chnl->sigChnl.intTransId;
        return AVDTP_RSP_ACCEPT;
    }
}

U8 AVDT_Abort_Req(AvdtpStreamAbortRequest *parms)
{
    AvdtpChannel *Chnl;
    BtStatus status = BT_STATUS_FAILED;
    AvdtpStreamAbortRequestResult rsp_parms;

    Chnl = &AVDTPTS(currentDevice)->channel;
    if(parms->stream_handle != ats.stream.locStrmId)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_ABORT_ERR_HDL);
        return AVDTP_RSP_REJECT;
    }            
    status =  AVDTP_AbortStream(&(ats.stream));
    rsp_parms.result = 0x00;
    rsp_parms.transaction = Chnl->sigChnl.intTransId;
    Send_AVDTP_Event_AbortReq_Result(&rsp_parms);     
    AVDTPTS(avdtpIssueCmd) = TRUE;
    
    if(status != BT_STATUS_PENDING)
    {
        parms->transaction = 0x00;
        return AVDTP_RSP_REJECT;
    }
    else
    {
        parms->transaction = Chnl->sigChnl.intTransId;
        return AVDTP_RSP_ACCEPT;
    }
}

U8 AVDT_DisconnectSignal_Req(void)
{
    AvdtpChannel *Chnl;
    
    Chnl = &AVDTPTS(currentDevice)->channel;
    AVDTP_Disconnect(Chnl);
    return BT_STATUS_SUCCESS;

}


U8 AVDT_Discover_Rsp(AvdtpstreamdiscoverResponse *parms)
{
    U8 rsp;
    BtStatus status = BT_STATUS_FAILED;

    if (parms->error_code != AVDTP_ERR_NO_ERROR)
    {
        Send_AVDTP_Event_Reject_Result(TC_AVDTP_Event_DiscoveryRej_Result ,0);
        rsp = AVDTP_RSP_REJECT;
    }
    else
    {
        Send_AVDTP_Event_DiscoveryRsp_Result(0);
        rsp = AVDTP_RSP_ACCEPT;
    }
    AVDTPTS(currentDevice)->channel.sigChnl.acpTransId = parms->transaction;
    status =  AVDTP_DiscoverStreamsWithErrorRsp(rsp, &AVDTPTS(currentDevice)->channel, 
                                parms->stream_info_num/4, parms->stream_info, parms->error_code);
    return status;
}

U8 AVDT_Get_Capabilities_Rsp(AvdtpStreamGetCapabilitiesResponse *parms)
{
    U8 rsp;
    BtStatus status = BT_STATUS_FAILED;

    if (parms->error_code != AVDTP_ERR_NO_ERROR)
    {
        Send_AVDTP_Event_Reject_Result(TC_AVDTP_Event_GetCapabilitiesRej_Result, 0);
        rsp = AVDTP_RSP_REJECT;
    }
    else
    {
        Send_AVDTP_Event_Get_CapabilitiesRsp_Result(0);
        rsp = AVDTP_RSP_ACCEPT;
    }
    AVDTPTS(currentDevice)->channel.sigChnl.acpTransId = parms->transaction;
    status =  AVDTP_GetCapabilitiesRsp(rsp, &AVDTPTS(currentDevice)->channel, &(ats.stream), parms->error_code);
    return status;
}

U8 AVDT_Get_Capabilities_LongRsp(AvdtpStreamGetCapabilitiesResponse *parms)
{
    U8 rsp;
    BtStatus status = BT_STATUS_FAILED;
    U8 rsp_string[] = { 0x01, 0x00, 
                                    0x07, 0x06, 0x00, 0x00, 0x11, 0x15, 0x14, 0x20,
                                    0x07, 0x7C, 0xC0,0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00 };
                                    

    if (parms->error_code != AVDTP_ERR_NO_ERROR)
    {
        rsp = AVDTP_RSP_REJECT;
    }
    else
    {
        rsp = AVDTP_RSP_ACCEPT;
    }
    AVDTPTS(currentDevice)->channel.sigChnl.acpTransId = parms->transaction;
    status =  AVDTP_GetCapabilitiesLongRsp(&AVDTPTS(currentDevice)->channel, &(ats.stream), rsp_string, sizeof(rsp_string));
    Send_AVDTP_Event_Get_CapabilitiesRsp_Result(0);    
    return status;
}

U8 AVDT_Set_Configuration_Rsp(AvdtpStreamSetConfigurationResponse *parms)
{
    U8 rsp;
    BtStatus status = BT_STATUS_FAILED;
    AVDTPTS(currentDevice)->channel.sigChnl.acpTransId = parms->transaction;

    if (parms->error_code != AVDTP_ERR_NO_ERROR)
    {
        rsp = AVDTP_RSP_REJECT;
        Send_AVDTP_Event_Reject_Result(TC_AVDTP_Event_SetConfigRej_Result, 0);
        status =  AVDTP_SetConfigRsp(
                                            rsp, 
                                            &AVDTPTS(currentDevice)->channel, 
                                            &(ats.stream), 
                                            parms->error_code, 
                                            parms->category[0]);

    }
    else
    {
        rsp = AVDTP_RSP_ACCEPT;
        Send_AVDTP_Event_Set_ConfigRsp_Result(0);
        status =  AVDTP_SetConfigRsp(
                                            rsp, 
                                            &AVDTPTS(currentDevice)->channel, 
                                            &(ats.stream), 
                                            parms->error_code, 
                                            AVDTP_SRV_CAT_MEDIA_TRANSPORT);
    }
    return status;
}

U8 AVDT_Re_Configuration_Rsp(AvdtpStreamReConfigurationResponse *parms)
{
    U8 rsp;
    BtStatus status = BT_STATUS_FAILED;
    AVDTPTS(currentDevice)->channel.sigChnl.acpTransId = parms->transaction;

    if (parms->error_code != AVDTP_ERR_NO_ERROR)
    {
        rsp = AVDTP_RSP_REJECT;
        Send_AVDTP_Event_Reject_Result(TC_AVDTP_Event_ReConfigRej_Result, 0);
        status =  AVDTP_ReconfigRsp(
                                            rsp, 
                                            &(ats.stream), 
                                            parms->error_code, 
                                            parms->category[0]);

    }
    else
    {
        rsp = AVDTP_RSP_ACCEPT;
        Send_AVDTP_Event_ReConfigRsp_Result(0);
        status =  AVDTP_ReconfigRsp(
                                            rsp, 
                                            &(ats.stream), 
                                            parms->error_code, 
                                            AVDTP_SRV_CAT_MEDIA_CODEC);

    }
    return status;
}

U8 AVDT_Open_Rsp(AvdtpStreamOpenResponse *parms)
{
    U8 rsp;
    BtStatus status = BT_STATUS_FAILED;
    if (parms->error_code != AVDTP_ERR_NO_ERROR)
    {
        Send_AVDTP_Event_Reject_Result(TC_AVDTP_Event_OpenRej_Result, 0);
        rsp = AVDTP_RSP_REJECT;
    }
    else
    {
         Send_AVDTP_Event_OpenRsp_Result(0);
        rsp = AVDTP_RSP_ACCEPT;
    }
    AVDTPTS(currentDevice)->channel.sigChnl.acpTransId = parms->transaction;
    status =  AVDTP_OpenStreamRsp(rsp, &(ats.stream), parms->error_code);
    return status;
}

U8 AVDT_Close_Rsp(AvdtpStreamCloseResponse *parms)
{
    U8 rsp;
    BtStatus status = BT_STATUS_FAILED;
    if (parms->error_code != AVDTP_ERR_NO_ERROR)
    {
        Send_AVDTP_Event_Reject_Result(TC_AVDTP_Event_CloseRej_Result, 0);
        rsp = AVDTP_RSP_REJECT;
    }
    else
    {
        Send_AVDTP_Event_CloseRsp_Result(0);
        rsp = AVDTP_RSP_ACCEPT;
    }
    AVDTPTS(currentDevice)->channel.sigChnl.acpTransId = parms->transaction;
    status =  AVDTP_CloseStreamRsp(rsp, &(ats.stream), parms->error_code);
    return status;
}

U8 AVDT_Start_Rsp(AvdtpStreamStartResponse *parms)
{
    U8 rsp;
    BtStatus status = BT_STATUS_FAILED;

    if (parms->error_code != AVDTP_ERR_NO_ERROR)
    {
        Send_AVDTP_Event_Reject_Result(TC_AVDTP_Event_StartRej_Result, 0);
        rsp = AVDTP_RSP_REJECT;
    }
    else
    {
        Send_AVDTP_Event_StartRsp_Result(0);
        rsp = AVDTP_RSP_ACCEPT;
    }
    AVDTPTS(currentDevice)->channel.sigChnl.acpTransId = parms->transaction;
    status =  AVDTP_StartStreamRsp(rsp, &(ats.stream), parms->error_code);
    return status;
}

U8 AVDT_Suspend_Rsp(AvdtpStreamSuspendResponse *parms)
{
    U8 rsp;
    BtStatus status = BT_STATUS_FAILED;

    if (parms->error_code != AVDTP_ERR_NO_ERROR)
    {
        Send_AVDTP_Event_Reject_Result(TC_AVDTP_Event_SuspendRej_Result, 0);
        rsp = AVDTP_RSP_REJECT;
    }
    else
    {
        Send_AVDTP_Event_SuspendRsp_Result(0);
        rsp = AVDTP_RSP_ACCEPT;
    }
    AVDTPTS(currentDevice)->channel.sigChnl.acpTransId = parms->transaction;
    status =  AVDTP_SuspendStreamRsp(rsp, &(ats.stream), parms->error_code);
    return status;
}

U8 AVDT_Abort_Rsp(AvdtpStreamAbortResponse *parms)
{
    BtStatus status = BT_STATUS_FAILED;

    AVDTPTS(currentDevice)->channel.sigChnl.acpTransId = parms->transaction;
    status =  AVDTP_AbortStreamRsp(&(ats.stream));
    Send_AVDTP_Event_AbortRsp_Result(0);
    return status;
}

void AVDT_HandleRxData(U8 *data, U16 length)
{
    ats.mediaHeader.version = (data[0] & 0x03);
    ats.mediaHeader.padding = (data[0] & 0x04) >> 3;
    
    ats.mediaHeader.csrcCount = (data[0] & 0xF0) >> 4;
    ats.mediaHeader.marker = (data[1] & 0x01);
    ats.mediaHeader.payloadType = (data[1] & 0x7F);
    ats.timeInc = ((U32)data[7]) | ((U32)data[6] << 8) |((U32)data[5] << 16)|((U32)data[4]<<24);
    OS_MemSet((U8 *)AVDTPTS(avdtpTxRxDataBuffer), 0, sizeof(AVDTPTS(avdtpTxRxDataBuffer)));
    if((length - 12)> sizeof(AVDTPTS(avdtpTxRxDataBuffer)))
    {
        OS_MemCopy((U8 *)AVDTPTS(avdtpTxRxDataBuffer), data+12, 
                        sizeof(AVDTPTS(avdtpTxRxDataBuffer)));
        AVDTPTS(buffer_offset) = sizeof(AVDTPTS(avdtpTxRxDataBuffer));
    }
    else
    {
        OS_MemCopy((U8 *)AVDTPTS(avdtpTxRxDataBuffer), data+12, 
                        length);
        AVDTPTS(buffer_offset) = length;
    }
}

void AVDT_Read_Data(AvdtpReadDataReq *parms)
{
    AvdtpChannel *Chnl;
    AvdtpReadDataRequestResult rsp_parms;
    
    Chnl = &AVDTPTS(currentDevice)->channel;
    if((parms->stream_handle != ats.stream.locStrmId) ||(AVDTPTS(state) != AVDTP_STRM_STATE_STREAMING) )
    {
        rsp_parms.result = 0x02;
        rsp_parms.length = 0x00;
        rsp_parms.payload_type = 0x00;
        rsp_parms.time_info = 0x00 ;
        rsp_parms.stream_handle = parms->stream_handle;
        rsp_parms.reliability = 0x01;
        rsp_parms.marker = 0x00;
    }            
    else
    {
        rsp_parms.result = 0x00;
        rsp_parms.length = 0x00;
        if(AVDTPTS(buffer_offset) > parms->expected_length)
            rsp_parms.length = parms->expected_length;
        else
            rsp_parms.length = AVDTPTS(buffer_offset);
        if(rsp_parms.length > sizeof(rsp_parms.inbuffer))
            rsp_parms.length = sizeof(rsp_parms.inbuffer);
        memcpy(rsp_parms.inbuffer, AVDTPTS(avdtpTxRxDataBuffer), rsp_parms.length);
        rsp_parms.payload_type = ats.mediaHeader.payloadType;
        rsp_parms.time_info = ats.timeInc ;
        rsp_parms.stream_handle = parms->stream_handle;
        rsp_parms.reliability = 0x00;
        rsp_parms.marker = ats.mediaHeader.marker;
    }
    Send_AVDTP_Event_ReadStreamDataReq_Result(&rsp_parms);

}

U8 AVDT_Write_Data(AvdtpWriteDataReq *parms)
{
    BtStatus status = BT_STATUS_FAILED;
    U16      maxPacketLen;
    BtPacket *btp;
    U8       headerLen;
    AvdtpChannel *chnl;
    AvdtpStreamDataRequestResult rsp_parms;
    if (AVDTPTS(state) == AVDTP_STRM_STATE_STREAMING) 
    {
        ats.mediaHeader.version = 2;
        ats.mediaHeader.marker = 0;
        ats.mediaHeader.padding = 0;
        ats.mediaHeader.csrcCount = 0;
        ats.mediaHeader.ssrc = parms->stream_handle;
        ats.timeInc = parms->time_info;
        ats.mediaHeader.timestamp = parms->time_info;
        ats.mediaHeader.sequenceNumber++;
        ats.mediaHeader.marker = parms->marker;
        ats.mediaHeader.payloadType = parms->payload_type;
        chnl = &AVDTPTS(currentDevice)->channel;
        if(chnl ==0)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTP_CHANNEL_IS_ZERO);
            rsp_parms.result = 0x01;
            Send_AVDTP_Event_WriteReq_Result(&rsp_parms);
            return FALSE;
        }
        btp = (BtPacket *)RemoveHeadList(&chnl->rtp_pkt_List);
        OS_MemSet((U8 *)AVDTPTS(avdtpTxRxDataBuffer), 0, sizeof(AVDTPTS(avdtpTxRxDataBuffer)));
        headerLen = AVDTP_CreateMediaHeader(&ats.mediaHeader, 
                         (U8 *)AVDTPTS(avdtpTxRxDataBuffer));
        if((parms->length + headerLen)> sizeof(AVDTPTS(avdtpTxRxDataBuffer)))
        {
            OS_MemCopy((U8 *)AVDTPTS(avdtpTxRxDataBuffer)+headerLen, parms->outbuffer, 
                sizeof(AVDTPTS(avdtpTxRxDataBuffer)));
            btp->dataLen = headerLen + sizeof(AVDTPTS(avdtpTxRxDataBuffer));

         }
        else
        {
            OS_MemCopy((U8 *)AVDTPTS(avdtpTxRxDataBuffer)+headerLen, parms->outbuffer, 
                (parms->length));
            btp->dataLen = headerLen + parms->length;
        }
        btp->data = (U8 *)AVDTPTS(avdtpTxRxDataBuffer);
        btp->headerLen = 0;
        btp->tailLen = 0;
        btp->flags = BTP_FLAG_INUSE;        
        status = AVDTP_StreamSendRawPacket(&ats.stream, btp);
    }
    if(status !=BT_STATUS_PENDING)
        rsp_parms.result = 0x01;
    else
        rsp_parms.result = 0x00;
    Send_AVDTP_Event_WriteReq_Result(&rsp_parms);
    return status;
}










void AvdtpTesterCallback(AvdtpChannel *Chnl, AvdtpCallbackParms *Parms)
{
    AvdtpTesterDevice *device = ContainingRecord(Chnl, AvdtpTesterDevice, channel);
    AvdtpTesterStream*stream = ContainingRecord(Parms->stream, AvdtpTesterStream, stream);
    BtStatus status;

    // add by stanley
    U16 str_data_len = 0;
    local_para_struct* msg;
    bt_test_cmd_rsp_struct  *param_ptr;

    bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_EVENT);
    switch (Parms->event)
    {
        case AVDTP_EVENT_CONNECT_IND:
            CMGR_CreateDataLink(&AVDTPTS(currentDevice)->cmgrHandler, &Parms->p.remDev->bdAddr);
            Send_AVDTP_Event_ConnectReq_Ind(Chnl,Parms);
            (void)AVDTP_ConnectRsp(Chnl, TRUE);
            break;
        case AVDTP_EVENT_CONNECT:
            if (device->flags & AVDTP_TESTER_DEV_FLAG_OUTGOING)
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_EVENT_CONNECT);
             } 
            else 
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_EVENT_CONNECTED);
            }
            break;
        case AVDTP_EVENT_DISCONNECT:

            Send_AVDTP_Event_DisConnectReq_Cfm(Chnl,Parms);
            
            break;
        case AVDTP_EVENT_STREAM_INFO:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_SEID_INFO,Parms->p.streamInfo->id, Parms->p.streamInfo->inUse);
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_MEDIA_INFO,Parms->p.streamInfo->mediaType, Parms->p.streamInfo->streamType);
            {
                U8 data[4];
                data[0] = Parms->p.streamInfo->id;
                data[1] = Parms->p.streamInfo->inUse;
                data[2] = Parms->p.streamInfo->mediaType;
                data[3] = Parms->p.streamInfo->streamType;
                Send_AVDTP_Event_Discovery_Cfm(Chnl,Parms, data, 4);
            }
            break;
        case AVDTP_EVENT_DISCOVER_CNF:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_EVENT_DISCOVERY_CNF, Parms->error);
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_MEDIA_INFO,Parms->p.streamInfo->mediaType, Parms->p.streamInfo->streamType);
            if(Parms->error !=0)
            {
                Send_AVDTP_Event_Discovery_Cfm(Chnl,Parms, NULL, 0);
            }
            break;

        case AVDTP_EVENT_CAPABILITY:
            /* DO nothing */
            break;
        case AVDTP_EVENT_CFG_CAPABILITY:
            /* DO nothing */
            break;
        case AVDTP_EVENT_GET_CAP_CNF:
            if(Parms->error !=0)
                Send_AVDTP_Event_GetCapabilities_Cfm(Chnl,Parms,NULL,0);
            else
                Send_AVDTP_Event_GetCapabilities_Cfm(Chnl,Parms,AVDTPTS(avdtpTxRxDataBuffer), AVDTPTS(buffer_offset));
            AVDTPTS(buffer_offset) = 0;
            break;
        case AVDTP_EVENT_SET_CFG_CNF:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_EVENT_SET_CFG_CNF,Parms->error);            
            Send_AVDTP_Event_SetConfig_Cfm(Chnl,Parms,Parms->rawData,Parms->rawDataLen);
            break;
        case AVDTP_EVENT_GET_CAP_TS_CNF:
            stream = device->currentStream;
            memcpy(AVDTPTS(avdtpTxRxDataBuffer)+AVDTPTS(buffer_offset),Parms->rawData,Parms->rawDataLen);
            AVDTPTS(buffer_offset) += Parms->rawDataLen;
            break;
        case AVDTP_EVENT_GET_CFG_CNF:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_EVENT_GET_CFG_CNF,Parms->error);            
            Send_AVDTP_Event_GetCapabilities_Cfm(Chnl,Parms,Parms->rawData,Parms->rawDataLen);
            break;
        case AVDTP_EVENT_STREAM_RECONFIG_CNF:
            /* A stream has been reconfigured */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_EVENT_STREAM_RECONFIG_CNF,Parms->error);
            if(Parms->rawDataLen !=0)
                Send_AVDTP_Event_ReConfig_Cfm(Chnl, Parms,Parms->rawData[0]);
            else
                Send_AVDTP_Event_ReConfig_Cfm(Chnl, Parms,0);
            break;
        case AVDTP_EVENT_STREAM_OPEN:
            /* A stream is open */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_EVENT_STREAM_OPEN,Parms->error); 
           if(AVDTPTS(avdtpIssueCmd))
               Send_AVDTP_Event_Open_Cfm(Chnl,Parms);    
            AVDTPTS(avdtpIssueCmd) = FALSE;
            break;
        case AVDTP_EVENT_STREAM_SECURITY_CNF:
            /* Security Data has been confirmed */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_EVENT_SECURITY_CNF,Parms->error);  
            break;
            /* Add by mtk01239, shall be handler by TIM */
        case AVDTP_EVENT_DISCOVER_IND:
            Send_AVDTP_Event_Discovery_Ind(Chnl,Parms);
            break;
        case AVDTP_EVENT_GET_CAP_IND:
            Send_AVDTP_Event_GetCapabilities_Ind(Chnl,Parms);
            break;
        case AVDTP_EVENT_SET_CFG_IND:
            Send_AVDTP_Event_SetConfig_Ind(Chnl, Parms, Parms->rawData, Parms->rawDataLen);
            break;
        case AVDTP_EVENT_STREAM_OPEN_IND:
            Send_AVDTP_Event_Open_Ind(Chnl,Parms);
            stream->device = device;
            stream->device->currentStream = stream;
            break;
        case AVDTP_EVENT_STREAM_START_IND:
            //2nd parameters: stream handles: two stream handles : total 4 bytes
            Send_AVDTP_Event_Start_Ind(Chnl,Parms);
            break;
        case AVDTP_EVENT_STREAM_CLOSE_IND:
            Send_AVDTP_Event_Close_Ind(Chnl,Parms);
            stream->flags |= AVDTP_TESTER_STRM_FLAG_CLOSE_ACP | AVDTP_TESTER_STRM_FLAG_IDLE;
            break;
        case AVDTP_EVENT_STREAM_RECONFIG_IND:
           Send_AVDTP_Event_ReConfig_Ind(Chnl, Parms, Parms->rawData, Parms->rawDataLen);
           break;
        case AVDTP_EVENT_STREAM_SUSPEND_IND:
            Send_AVDTP_Event_Suspend_Ind(Chnl,Parms);
            break;
        case AVDTP_EVENT_STREAM_ABORT_IND:
            Send_AVDTP_Event_Abort_Ind(Chnl,Parms);
            stream->flags |= AVDTP_TESTER_STRM_FLAG_CLOSE_ACP | AVDTP_TESTER_STRM_FLAG_IDLE;
            break;
        case AVDTP_EVENT_STREAM_STARTED:
            if(AVDTPTS(avdtpIssueCmd))
                Send_AVDTP_Event_Start_Cfm(Chnl,Parms);    
            AVDTPTS(avdtpIssueCmd) = FALSE;
            AVDTPTS(state) = AVDTP_STRM_STATE_STREAMING;
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_EVENT_STREAM_STARTED, Parms->error);
            break;
        case AVDTP_EVENT_STREAM_SUSPENDED:
            if(AVDTPTS(avdtpIssueCmd))
                Send_AVDTP_Event_Suspend_Cfm(Chnl, Parms);
            AVDTPTS(avdtpIssueCmd) = FALSE;
            AVDTPTS(state) = AVDTP_STRM_STATE_OPEN;
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_AVDTPTS_EVENT_STREAM_SUSPENDED, Parms->error);
            if(Parms->error == AVDTP_ERR_NO_ERROR)
            {        
                 CMGR_ClearSniffTimer(&device->cmgrHandler);
            }
             break;
        case AVDTP_EVENT_STREAM_ABORTED:
            if(AVDTPTS(avdtpIssueCmd))
                Send_AVDTP_Event_Abort_Cfm(Chnl,Parms);
            AVDTPTS(state) = AVDTP_STRM_STATE_IDLE;
            AVDTPTS(avdtpIssueCmd) = FALSE;
            break;
        case AVDTP_EVENT_STREAM_CLOSED:
            if(AVDTPTS(avdtpIssueCmd))
                Send_AVDTP_Event_Close_Cfm(Chnl,Parms);
            AVDTPTS(state) = AVDTP_STRM_STATE_IDLE;
            AVDTPTS(avdtpIssueCmd) = FALSE;
            if(Parms->error !=0)
            {
                stream->flags |= AVDTP_TESTER_STRM_FLAG_CLOSE_ACP | AVDTP_TESTER_STRM_FLAG_IDLE;
                stream->device = device;
            }
            break;
        case AVDTP_EVENT_STREAM_DATA_IND:
            AVDT_HandleRxData(Parms->p.data, Parms->len);
            /* Stream Data is incoming */
            break;
        case AVDTP_EVENT_STREAM_PACKET_SENT:
            /* Stream Data has been sent */
            // 2006-0814: if the data buffer is allocated previously before sending, now it is time to release this buffer : Parms.p.data
            bt_trace(TRACE_GROUP_1,RELEASE_THE_SPACE_ALLOCATED_IN_AVDT_WRITE_DATA);
            break;
    }
}
 


BtStatus AvdtpTesterCreateAclLink(BD_ADDR *Addr, CmgrHandler *Handler, CmgrCallback Callback)
{
    AvdtpTesterDevice *device = ContainingRecord(Handler, AvdtpTesterDevice, cmgrHandler);
    BtStatus status;
    AVDTPTS(state) = AVDTP_STRM_STATE_IDLE;
#if 0  
    /* quality field is not defined in struct _BtSelectDeviceToken */
    Handler->devQuery.quality.mask = 
                            (BDQM_DEVICE_CLASS|BDQM_DEVICE_STATUS|
                             BDQM_SERVICE_CLASS|BDQM_SERVICE_UUID);
    Handler->devQuery.quality.status = BDS_IN_RANGE;
    Handler->devQuery.quality.deviceClass = COD_MAJOR_AUDIO;
    Handler->devQuery.quality.serviceUuid = SC_AUDIO_VIDEO;
#endif    
    status = CMGR_CreateDataLink(Handler, Addr);
    if (status == BT_STATUS_SUCCESS) 
    {
        AVDTP_Connect(&(device->channel), device->cmgrHandler.remDev);
    }
    return status;
}


void AvdtpTesterCmgrCallback(CmgrHandler *cHandler, 
                      CmgrEvent Event, 
                      BtStatus Status)
{
    AvdtpTesterDevice *device = ContainingRecord(cHandler, AvdtpTesterDevice, cmgrHandler);
    
    switch (Event) {

    case CMEVENT_DATA_LINK_CON_CNF:
        if (Status == BT_STATUS_SUCCESS) 
        {
            /* ACL is connected, start SDP Query */
            Status = AVDTP_Connect(&(device->channel), cHandler->remDev);
            if (Status == BT_STATUS_PENDING) 
            {
                return;
            }
        }
        /* Disconnect and call the application */
        (void)CMGR_RemoveDataLink(&device->cmgrHandler);
        (void)CMGR_DeregisterHandler(&device->cmgrHandler);
        break;
    case CMEVENT_DATA_LINK_DIS:
        AVDTPTS(state) = AVDTP_STRM_STATE_IDLE;
        break;
    }
}

void avdtptester_handler(ilm_struct *ilm_ptr)
{
    char *string;

    bt_test_cmd_struct *test_msg;
    if (ilm_ptr->msg_id == MSG_ID_BT_AVDTP_TEST_CMD_REQ)
    {
        test_msg = ((bt_test_cmd_struct *)ilm_ptr->local_para_ptr);    
        string = (char *)test_msg->string;    

        if(strncmp(string, "init",4)==0)
        {
            bt_trace(TRACE_GROUP_1,AVDTP_INIT);
            AvdtpTesterInit();
        }
        else if(strncmp(string, "connect",7)==0)
        {
            AvdtpConnectRequest adp_msg;
            bt_trace(TRACE_GROUP_1,AVDTP_AVDT_CONNECTREQ);
            AVDT_ConnectReq((AvdtpConnectRequest *)&adp_msg);
        }
        else if(strncmp(string, "discovery",9) ==0)
        {
        
            AvdtpStreamDiscoverRequest adp_msg;
            bt_trace(TRACE_GROUP_1,AVDTP_AVDT_DISCOVER_REQ);
            AVDT_Discover_Req((AvdtpStreamDiscoverRequest *)&adp_msg);
        }
        else if(strncmp(string, "get",3) ==0)
        {
            AvdtpStreamGetCapabilitiesRequest adp_msg;
            adp_msg.acp_seid = string[3]-'0';
            bt_trace(TRACE_GROUP_1,AVDTP_AVDT_GET_CAPABILITIES_REQ);
            AVDT_Get_Capabilities_Req((AvdtpStreamGetCapabilitiesRequest *)&adp_msg);
        }

        else if(strncmp(string, "set",3) ==0)
        {
            AvdtpStreamSetConfigurationRequest adp_msg;
            adp_msg.acp_seid = string[3]-'0';
            adp_msg.int_seid = string[4]-'0';
            adp_msg.codecType = AVDTP_CODEC_TYPE_SBC;
            adp_msg.element_len = sizeof(SbcSrcElements);   
            bt_trace(TRACE_GROUP_1,AVDTP_AVDT_SET_CONFIGURATION_REQ);
            AVDT_Set_Configuration_Req((AvdtpStreamSetConfigurationRequest *)&adp_msg);
        }
        else if(strncmp(string, "reconfig",8) ==0)
        {
            AvdtpStreamReConfigurationRequest adp_msg;
            adp_msg.stream_handle= string[3]-'0';
            adp_msg.codecType = AVDTP_CODEC_TYPE_SBC;
            adp_msg.element_len = sizeof(SbcSrcElements);    
            OS_MemCopy((U8 *)adp_msg.element,(U8 *) SbcSrcElements, sizeof(SbcSrcElements));
            AVDT_ReConfigure_Req((AvdtpStreamReConfigurationRequest *)&adp_msg);
        }
        else if(strncmp(string, "open",4) ==0)
        {
            AvdtpStreamOpenRequest adp_msg;
            adp_msg.stream_handle= string[4]-'0';
            AVDT_Open_Req((AvdtpStreamOpenRequest *)&adp_msg);
        }
        else if(strncmp(string, "close",5) ==0)
        {
            AvdtpStreamCloseRequest adp_msg;
            adp_msg.stream_handle= string[5]-'0';
            AVDT_Close_Req((AvdtpStreamCloseRequest *)&adp_msg);
        }
        else if(strncmp(string, "start",5) ==0)
        {
            AvdtpStreamStartRequest adp_msg;
            adp_msg.stream_handle[0] = string[5]-'0';
            AVDT_Start_Req((AvdtpStreamStartRequest *)&adp_msg);
        }
        else if(strncmp(string, "suspend",7) ==0)
        {
            AvdtpStreamSuspendRequest adp_msg;
            adp_msg.stream_handle[0] = string[7]-'0';
            AVDT_Suspend_Req((AvdtpStreamSuspendRequest *)&adp_msg);
        }
        else if(strncmp(string, "abort",5) ==0)
        {
            AvdtpStreamAbortRequest adp_msg;
            adp_msg.stream_handle= string[5]-'0';
            AVDT_Abort_Req((AvdtpStreamAbortRequest *)&adp_msg);

        }
    }
}

void avdtp_handler_inject_msg(char *string)
{
    bt_test_cmd_struct *test_msg;

    test_msg = ( bt_test_cmd_struct*)
                           construct_local_para (sizeof(bt_test_cmd_struct),TD_UL);

    memcpy((U8 *)test_msg->string, (U8 *)string, strlen(string));
    avdtp_tester_sendMsg(
                            MSG_ID_BT_AVDTP_TEST_CMD_REQ,
                            MOD_BT,
                            BT_APP_SAP,
                            (local_para_struct*)test_msg,
                            NULL
                            );

}


#endif



