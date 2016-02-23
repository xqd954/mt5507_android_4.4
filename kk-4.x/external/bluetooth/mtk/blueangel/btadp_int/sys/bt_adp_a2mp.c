/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifdef __BT_HS_30__
#include "string.h"

#include "bt_common.h"
#include "stdio.h"      /* Basic C I/O functions */
#include "bt_feature.h"
#include "bt.h"
#include "bluetooth_a2mp_struct.h"
#include "bluetooth_struct.h"
#include "btconfig.h"
#include "bttypes.h"
#include "hci.h"
#include "hci_amp.h"
#include "meamp.h"

U8 amp_rxHciEvent[256];

void HciSentAMPEvent(
                        msg_type msg,
                        local_para_struct *local_para)

{
    if(local_para != NULL)
        BT_SendMessage(msg, MOD_BT, local_para, local_para->msg_len);
    else
        BT_SendMessage(msg, MOD_BT, NULL, 0);
}

pal_bt_data_event_struct *a2mp_fast_handle_loca_parms;
U8 amp_rx_count=0;
U32 rx_count = 0;
U16 sequence_no1;

void a2mp_adp_message_handler(ilm_struct *ilm_ptr)
{
    void *msg_p = (void*)ilm_ptr->local_para_ptr;
    U16 hci_eventlen = 0;
    
    OS_MemSet((U8 *)amp_rxHciEvent, 0, sizeof(amp_rxHciEvent));
    
    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_PAL_BT_READ_LOCAL_AMP_INFO_COMPLETE_EVENT:
            {
                bt_a2mp_read_local_amp_info_cmd_cnf_struct *parms;
                parms = (bt_a2mp_read_local_amp_info_cmd_cnf_struct *)ilm_ptr->local_para_ptr;
                amp_rxHciEvent[0] = HCE_COMMAND_COMPLETE;
                amp_rxHciEvent[1] = 34;
                amp_rxHciEvent[2] = 0x01;                
                StoreLE16(amp_rxHciEvent+3, HCC_READ_LOCAL_AMP_INFO);                
                amp_rxHciEvent[5] = parms->status;
                amp_rxHciEvent[6] = parms->amp_status;
                StoreLE32(amp_rxHciEvent+7, parms->total_bandwidth);
                StoreLE32(amp_rxHciEvent+11, parms->max_guarantee_bandwidth);
                StoreLE32(amp_rxHciEvent+15, parms->min_latency);
                StoreLE32(amp_rxHciEvent+19, parms->max_pdu_size);
                amp_rxHciEvent[23] = parms->controller_type;
                StoreLE32(amp_rxHciEvent+24, parms->pal_capability);
                StoreLE32(amp_rxHciEvent+26, parms->max_amp_assoc_length);
                StoreLE32(amp_rxHciEvent+28, parms->max_flush_timeout);                
                StoreLE32(amp_rxHciEvent+32, parms->best_effort_flush_timeout); 
                HciProcessAMPRxEvent(amp_rxHciEvent, 36, BT_PAL_CONTROLLERID);
            }
            break;
        case MSG_ID_PAL_BT_READ_LOCAL_AMP_ASSOC_COMPLETE_EVENT:
            {
                bt_a2mp_read_local_amp_assoc_cmd_cnf_struct *parms;
                parms = (bt_a2mp_read_local_amp_assoc_cmd_cnf_struct *)ilm_ptr->local_para_ptr;
                amp_rxHciEvent[0] = HCE_COMMAND_COMPLETE;
                amp_rxHciEvent[1] = 7+parms->amp_assoc_fragment_size;
                amp_rxHciEvent[2] = 0x01;                
                StoreLE16(amp_rxHciEvent+3, HCC_READ_LOCAL_AMP_ASSOC);
                amp_rxHciEvent[5] = parms->status; /* status */
                amp_rxHciEvent[6] = parms->physical_link_hdl;
                StoreLE16(amp_rxHciEvent+7, parms->amp_assoc_remaining_length);
                memcpy(amp_rxHciEvent+9, parms->amp_assoc_fragment, parms->amp_assoc_fragment_size);
                Report(("MSG_ID_PAL_BT_READ_LOCAL_AMP_ASSOC_COMPLETE_EVENT"));
                HciProcessAMPRxEvent(amp_rxHciEvent, 9+parms->amp_assoc_fragment_size, BT_PAL_CONTROLLERID);
            }
            break;
        case MSG_ID_PAL_BT_CREATE_PHYSICAL_LINK_STATUS_EVENT:
            {
                bt_a2mp_create_physical_link_cmd_cnf_struct *parms;
                parms = (bt_a2mp_create_physical_link_cmd_cnf_struct *)ilm_ptr->local_para_ptr;
                amp_rxHciEvent[0] = HCE_COMMAND_STATUS;
                amp_rxHciEvent[1] = 4;
                amp_rxHciEvent[2] = parms->status;                
                amp_rxHciEvent[3] = 0x01;
                StoreLE16(amp_rxHciEvent+4, HCC_CREATE_PHYSICAL_LINK);
                HciProcessAMPRxEvent(amp_rxHciEvent, 5, BT_PAL_CONTROLLERID);
            }
            break;
        case MSG_ID_PAL_BT_DATA_EVENT:
#if 1
            {
				
                a2mp_fast_handle_loca_parms = (pal_bt_data_event_struct *)ilm_ptr->local_para_ptr;
                HciProcessAMPRxData(a2mp_fast_handle_loca_parms);
				
#ifdef __BT_3_0_HS__
                HciCheckForTxAMPDataSend();
#endif	
				
            }
#endif
#if 0
            {
                pal_bt_data_event_struct *parms;
                parms = (pal_bt_data_event_struct *)ilm_ptr->local_para_ptr;
                amp_rx_count++;
                if(amp_rx_count == 1)
                {
                    HciProcessAMPRxData(parms);
                    HCI_Process();
                }
                else
                {
                    U16 sequence_no;
                    pal_bt_data_event_struct *parms;
                    parms = (pal_bt_data_event_struct *)ilm_ptr->local_para_ptr;                    

                    rx_count++;
                    amp_rx_count = 2;
                    sequence_no = BEtoHost16(parms->aucData_p);

                    if(sequence_no != (sequence_no1+1))
                    {
                        Report(("Miss data:%d", (sequence_no - sequence_no1 -1)));
                    }
                    sequence_no1 = sequence_no;

                    BTCoreGetSystemTick2();
                    Report(("rx count:%d, seq:%d", rx_count, sequence_no));

                }

            }
#endif
#if 0
            {
                pal_bt_data_event_struct *parms;
                U32 test1=0;
                U32 failed;
                parms = (pal_bt_data_event_struct *)ilm_ptr->local_para_ptr;
                HciProcessAMPRxData(parms);
                HCI_Process();
                Report(("PAL TX start with Tx failed:%d", failed));
                
                do
                {
                    failed = UARTRAN_AMP_ForceTxDataTest();
                    test1++;
                }
                while(test1< 10000);
                Report(("PAL TX done with Tx failed:%d", failed));
            }
#endif

            break;
        case MSG_ID_PAL_BT_ACCEPT_PHYSICAL_LINK_STATUS_EVENT:
            {
                bt_a2mp_accept_physical_link_cmd_cnf_struct *parms;
                parms = (bt_a2mp_accept_physical_link_cmd_cnf_struct *)ilm_ptr->local_para_ptr;
                amp_rxHciEvent[0] = HCE_COMMAND_STATUS;
                amp_rxHciEvent[1] = 4;
                amp_rxHciEvent[2] = parms->status;                
                amp_rxHciEvent[3] = 0x01;
                StoreLE16(amp_rxHciEvent+4, HCC_ACCEPT_PHYSICAL_LINK);
                HciProcessAMPRxEvent(amp_rxHciEvent, 5, BT_PAL_CONTROLLERID);
            }
            break;   
        case MSG_ID_PAL_BT_CHANNEL_SELECTED_EVENT:
            {
                bt_a2mp_channel_selected_evt_ind_struct *parms;
                parms = (bt_a2mp_channel_selected_evt_ind_struct *)ilm_ptr->local_para_ptr;
                amp_rxHciEvent[0] = (U8)HCE_CHANNEL_SELECTED;
                amp_rxHciEvent[1] = 1;
                amp_rxHciEvent[2] = parms->physical_link_hdl;                
                Me_HandleAmpRxHCIEventMessage(amp_rxHciEvent, 3, BT_PAL_CONTROLLERID);
            }            
            break;
            
        case MSG_ID_PAL_BT_PHYSICAL_LINK_COMPLETE_EVENT:
            {
                bt_a2mp_physical_link_completed_evt_ind_struct *parms;
                parms = (bt_a2mp_physical_link_completed_evt_ind_struct *)ilm_ptr->local_para_ptr;
                amp_rxHciEvent[0] = (U8)HCE_PHYSICAL_LINK_COMPLETE;
                amp_rxHciEvent[1] = 2;
                amp_rxHciEvent[2] = parms->status;                
                amp_rxHciEvent[3] = parms->physical_link_hdl;                
                Me_HandleAmpRxHCIEventMessage(amp_rxHciEvent, 4, BT_PAL_CONTROLLERID);
            }   
            break;
        case MSG_ID_PAL_BT_CHNAGE_AMP_STATUS_EVENT:
            {
                bt_a2mp_amp_status_change_evt_struct *parms1;
                parms1 = (bt_a2mp_amp_status_change_evt_struct *)ilm_ptr->local_para_ptr;
                amp_rxHciEvent[0] = (U8)HCE_AMP_STATUS_CHANGE;
                amp_rxHciEvent[1] = 2;
                amp_rxHciEvent[2] = parms1->status;                
                amp_rxHciEvent[3] = parms1->amp_status;                
                Report(("MSG_ID_PAL_BT_CHNAGE_AMP_STATUS_EVENT:%d, %d",parms1->status, parms1->amp_status));
                Me_HandleAmpRxHCIEventMessage(amp_rxHciEvent, 4, BT_PAL_CONTROLLERID); 


            }
            break;
        case MSG_ID_PAL_BT_WRITE_REMOTE_AMP_ASSOC_COMPLETE_EVENT:
            {
                bt_a2mp_write_remote_amp_assoc_cmd_cnf_struct *parms;
                parms = (bt_a2mp_write_remote_amp_assoc_cmd_cnf_struct *)ilm_ptr->local_para_ptr;
                amp_rxHciEvent[0] = (U8)HCE_COMMAND_COMPLETE;
                amp_rxHciEvent[1] = 4;
                amp_rxHciEvent[2] = 0x01;                
                StoreLE16(amp_rxHciEvent+3, HCC_WRITE_REMOTE_AMP_ASSOC);
                amp_rxHciEvent[5] = 0x00; /* status */
                amp_rxHciEvent[6] = parms->physical_link_hdl;
                HciProcessAMPRxEvent(amp_rxHciEvent, 7, BT_PAL_CONTROLLERID);
            }
            break;
        case MSG_ID_PAL_BT_DISCONNECT_PHYSICAL_LINK_STATUS_EVENT:
            {
                bt_a2mp_physical_link_disconnect_cmd_status_cnf_struct *parms;
                parms = (bt_a2mp_physical_link_disconnect_cmd_status_cnf_struct *)ilm_ptr->local_para_ptr;
                Report(("MSG_ID_PAL_BT_DISCONNECT_PHYSICAL_LINK_STATUS_EVENT"));
                amp_rxHciEvent[0] = (U8)HCE_COMMAND_STATUS;
                amp_rxHciEvent[1] = 4;
                amp_rxHciEvent[2] = parms->status;                
                amp_rxHciEvent[3] = 0x01;
                StoreLE16(amp_rxHciEvent+4, HCC_DISCONNECT_PHYSICAL_LINK);
                HciProcessAMPRxEvent(amp_rxHciEvent, 6, BT_PAL_CONTROLLERID);
            }
            break;
        case MSG_ID_PAL_BT_DISCONNECT_PHYSICAL_LINK_COMPLETE_EVENT:
            {
                bt_a2mp_physical_link_disconnect_completed_evt_struct *parms;
                parms = (bt_a2mp_physical_link_disconnect_completed_evt_struct *)ilm_ptr->local_para_ptr;
                Report(("MSG_ID_PAL_BT_DISCONNECT_PHYSICAL_LINK_COMPLETE_EVENT"));
                amp_rxHciEvent[0] = (U8)HCE_DISCONNECT_PHYSICAL_LINK;
                amp_rxHciEvent[1] = 3;
                amp_rxHciEvent[2] = parms->status;  
                amp_rxHciEvent[3] = parms->physical_link_hdl;                
                amp_rxHciEvent[4] = parms->reason;                
                Me_HandleAmpRxHCIEventMessage(amp_rxHciEvent, 5, BT_PAL_CONTROLLERID);
            }
            break;  
        case MSG_ID_PAL_BT_CREATE_LOGICAL_LINK_STATUS_EVENT:
            {
                bt_a2mp_create_logical_cmd_cnf_struct *parms;
                parms = (bt_a2mp_create_logical_cmd_cnf_struct *)ilm_ptr->local_para_ptr;
                amp_rxHciEvent[0] = (U8)HCE_COMMAND_STATUS;
                amp_rxHciEvent[1] = 4;
                amp_rxHciEvent[2] = parms->status;                
                amp_rxHciEvent[3] = 0x01;
                StoreLE16(amp_rxHciEvent+4, HCC_CREATE_LOGICAL_LINK);
                HciProcessAMPRxEvent(amp_rxHciEvent, 6, BT_PAL_CONTROLLERID);
            }
            break;
        case MSG_ID_PAL_BT_ACCEPT_LOGICAL_LINK_STATUS_EVENT:
            {
                bt_a2mp_accept_logical_cmd_cnf_struct *parms;
                parms = (bt_a2mp_accept_logical_cmd_cnf_struct *)ilm_ptr->local_para_ptr;
                amp_rxHciEvent[0] = (U8)HCE_COMMAND_STATUS;
                amp_rxHciEvent[1] = 4;
                amp_rxHciEvent[2] = parms->status;                
                amp_rxHciEvent[3] = 0x01;
                StoreLE16(amp_rxHciEvent+4, HCC_ACCEPT_LOGICAL_LINK);
                HciProcessAMPRxEvent(amp_rxHciEvent, 6, 0x01);
            }
            break;            
        case MSG_ID_PAL_BT_LOGICAL_LINK_COMPLETE_EVENT:
            {
                bt_a2mp_logical_link_completed_evt_struct *parms;
                parms = (bt_a2mp_logical_link_completed_evt_struct *)ilm_ptr->local_para_ptr;
                amp_rxHciEvent[0] = (U8)HCE_LOGICAL_LINK_COMPLETE;
                amp_rxHciEvent[1] = 5;
                amp_rxHciEvent[2] = parms->status;  
                StoreLE16(amp_rxHciEvent+3, parms->logical_link_hdl);
                amp_rxHciEvent[5] = parms->physical_link_hdl;                
                amp_rxHciEvent[6] = parms->tx_flow_spec_id; 
                Report(("pal logical complete event:%d", parms->status));
                Me_HandleAmpRxHCIEventMessage(amp_rxHciEvent, 7, BT_PAL_CONTROLLERID);
            }
            break;   
        case MSG_ID_PAL_BT_DISCONNECT_LOGICAL_LINK_COMPLETE_EVENT:
            {
                bt_a2mp_logical_link_disconnect_completed_evt_struct *parms;
                parms = (bt_a2mp_logical_link_disconnect_completed_evt_struct *)ilm_ptr->local_para_ptr;
                Report(("MSG_ID_PAL_BT_DISCONNECT_LOGICAL_LINK_COMPLETE_EVENT"));

                amp_rxHciEvent[0] = (U8)HCE_DISCONNECT_LOGICAL_LINK;
                amp_rxHciEvent[1] = 4;
                amp_rxHciEvent[2] = parms->status;  
                StoreLE16(amp_rxHciEvent+3, parms->logical_link_hdl);                
                amp_rxHciEvent[5] = parms->reason;                
                Me_HandleAmpRxHCIEventMessage(amp_rxHciEvent, 6, BT_PAL_CONTROLLERID);
            }
            break;  
        case MSG_ID_PAL_BT_NUM_OF_COMPLETE_DATA_BLOCKS_EVENT:
            {
                bt_a2mp_amp_num_of_data_block_evt_struct *parms;
                parms = (bt_a2mp_amp_num_of_data_block_evt_struct *)ilm_ptr->local_para_ptr;
                amp_rxHciEvent[0] = (U8)HCE_NUM_OF_COMPLETE_DATA_BLOCKS;
                amp_rxHciEvent[1] = 3 + parms->ucNum_of_handles * 6;
                StoreLE16(amp_rxHciEvent+2, parms->u2Total_num_data_blocks);
                amp_rxHciEvent[4] = parms->ucNum_of_handles;
                {
                    U8 i=0;
                    for(i=0; i < parms->ucNum_of_handles; i++)
                    {
                        StoreLE16(amp_rxHciEvent+i*6+5, parms->au2Handle[i]);
                        StoreLE16(amp_rxHciEvent+i*6+7, parms->au2Num_of_completed_packet[i]);
                        StoreLE16(amp_rxHciEvent+i*6+9, parms->au2Num_of_completed_blocks[i]);                        
                    }
                }
                Me_HandleAmpRxHCIEventMessage(amp_rxHciEvent, 5 + parms->ucNum_of_handles * 6, BT_PAL_CONTROLLERID);
                
            }
            break;
        case MSG_ID_PAL_BT_DISCONNECT_LOGICAL_LINK_STATUS_EVENT:
            {
                bt_a2mp_logical_link_disconnect_cmd_status_cnf_struct *parms;
                parms = (bt_a2mp_logical_link_disconnect_cmd_status_cnf_struct *)ilm_ptr->local_para_ptr;
                Report(("MSG_ID_PAL_BT_DISCONNECT_LOGICAL_LINK_STATUS_EVENT"));
                amp_rxHciEvent[0] = (U8)HCE_COMMAND_STATUS;
                amp_rxHciEvent[1] = 4;
                amp_rxHciEvent[2] = parms->status;                
                amp_rxHciEvent[3] = 0x01;
                StoreLE16(amp_rxHciEvent+4, HCC_DISCONNECT_LOGICAL_LINK);
                HciProcessAMPRxEvent(amp_rxHciEvent, 6, BT_PAL_CONTROLLERID);
            }
            break;            
#if 0    
        case MSG_ID_PAL_BT_READ_LOCAL_AMP_INFO_COMMAND:
            {
                bt_a2mp_read_local_amp_info_cmd_cnf_struct *bt_msg;
                bt_msg =
                    (bt_a2mp_read_local_amp_info_cmd_cnf_struct*) construct_local_para(sizeof(bt_a2mp_read_local_amp_info_cmd_cnf_struct), TD_UL);

                bt_msg->amp_status = 0x01;
                bt_msg->total_bandwidth = 0xffffffff;
                bt_msg->max_guarantee_bandwidth = 0xffffffff;     
                bt_msg->min_latency = 0xffffffff;                 
                bt_msg->max_pdu_size = 670;                                 
                bt_msg->controller_type = 0x01;                                 
                bt_msg->pal_capability = 0x00;
                bt_msg->max_amp_assoc_length = 0xffff;
                bt_msg->max_flush_timeout = 0xffffffff;
                bt_msg->best_effort_flush_timeout = 0xffffffff;                
                HciSentAMPEvent(MSG_ID_PAL_BT_READ_LOCAL_AMP_INFO_COMPLETE_EVENT, bt_msg);
                break;
            }
        case MSG_ID_PAL_BT_READ_LOCAL_AMP_ASSOC_COMMAND:
            {
                U16 data_size;
                bt_a2mp_read_local_amp_assoc_cmd_req_struct *input_msg;
                bt_a2mp_read_local_amp_assoc_cmd_cnf_struct *bt_msg;
                input_msg = (bt_a2mp_read_local_amp_assoc_cmd_req_struct *)ilm_ptr->local_para_ptr;
                
                bt_msg =
                    (bt_a2mp_read_local_amp_assoc_cmd_cnf_struct*) construct_local_para(sizeof(bt_a2mp_read_local_amp_assoc_cmd_cnf_struct), TD_UL);
                if(input_msg->length_so_far > amp_assoc_size)
                    bt_msg->status = 0x01;
                else
                    bt_msg->status = 0x00;

                bt_msg->physical_link_hdl = input_msg->physical_link_hdl;
                if(input_msg->length_so_far + input_msg->amp_assoc_length >= amp_assoc_size)
                {
                    data_size = amp_assoc_size - (input_msg->length_so_far);
                    bt_msg->amp_assoc_remaining_length = 0;
                    OS_MemCopy((U8 *)bt_msg->amp_assoc_fragment, temp_amp_assoc+input_msg->length_so_far, data_size);
                }
                else
                {
                    bt_msg->amp_assoc_remaining_length = amp_assoc_size - (input_msg->length_so_far + input_msg->amp_assoc_length);
                    data_size = input_msg->amp_assoc_length;
                    OS_MemCopy((U8 *)bt_msg->amp_assoc_fragment, temp_amp_assoc+input_msg->length_so_far, data_size);
                }
                bt_msg->amp_assoc_fragment_size = data_size;
                HciSentAMPEvent(MSG_ID_PAL_BT_READ_LOCAL_AMP_ASSOC_COMPLETE_EVENT, bt_msg);
            }
            break;   
        case MSG_ID_PAL_BT_WRITE_REMOTE_AMP_ASSOC_COMMAND:
            {
                bt_a2mp_write_remote_amp_assoc_cmd_req_struct *input_msg;
                bt_a2mp_write_remote_amp_assoc_cmd_cnf_struct *bt_msg;
                input_msg = (bt_a2mp_write_remote_amp_assoc_cmd_req_struct *)ilm_ptr->local_para_ptr;
                
                bt_msg =
                    (bt_a2mp_write_remote_amp_assoc_cmd_cnf_struct*) construct_local_para(sizeof(bt_a2mp_write_remote_amp_assoc_cmd_cnf_struct), TD_UL);
                bt_msg->status = 0x00;
                bt_msg->physical_link_hdl = input_msg->physical_link_hdl;
                HciSentAMPEvent(MSG_ID_PAL_BT_WRITE_REMOTE_AMP_ASSOC_COMPLETE_EVENT, bt_msg);
            }
            break;              
        case MSG_ID_PAL_BT_ACCEPT_PHYSICAL_LINK_COMMAND:
            {
                bt_a2mp_accept_physical_link_cmd_cnf_struct *bt_msg;
                bt_msg =
                    (bt_a2mp_accept_physical_link_cmd_cnf_struct*) construct_local_para(sizeof(bt_a2mp_accept_physical_link_cmd_cnf_struct), TD_UL);
                bt_msg->status = 0x00;                
                HciSentAMPEvent(MSG_ID_PAL_BT_ACCEPT_PHYSICAL_LINK_STATUS_EVENT, bt_msg);
            }
            break;
        case MSG_ID_PAL_BT_CREATE_PHYSICAL_LINK_COMMAND:
            {
                bt_a2mp_create_physical_link_cmd_cnf_struct *bt_msg;
                bt_msg =
                    (bt_a2mp_create_physical_link_cmd_cnf_struct*) construct_local_para(sizeof(bt_a2mp_create_physical_link_cmd_cnf_struct), TD_UL);

                bt_msg->status = 0x00;                
                HciSentAMPEvent(MSG_ID_PAL_BT_CREATE_PHYSICAL_LINK_STATUS_EVENT, bt_msg);
                break;
            }
            break;   
        case MSG_ID_PAL_BT_DISCONNECT_PHYSICAL_LINK_COMMAND:
            {
                bt_a2mp_physical_link_disconnect_cmd_status_cnf_struct *bt_msg;
                
                bt_msg =
                    (bt_a2mp_physical_link_disconnect_cmd_status_cnf_struct*) construct_local_para(sizeof(bt_a2mp_physical_link_disconnect_cmd_status_cnf_struct), TD_UL);

                bt_msg->status = 0x00;   
                HciSentAMPEvent(MSG_ID_PAL_BT_DISCONNECT_PHYSICAL_LINK_STATUS_EVENT, bt_msg);
            }
            {
                bt_a2mp_physical_link_disconnect_completed_evt_struct *bt_msg1;
                
                bt_msg1 =
                    (bt_a2mp_physical_link_disconnect_completed_evt_struct*) construct_local_para(sizeof(bt_a2mp_physical_link_disconnect_completed_evt_struct), TD_UL);
            
                bt_msg1->status = 0x00;  
                bt_msg1->physical_link_hdl = 10;
                bt_msg1->reason = 0x16;
                HciSentAMPEvent(MSG_ID_PAL_BT_DISCONNECT_PHYSICAL_LINK_COMPLETE_EVENT, bt_msg1);
                break;
            
            }
            break;            
        case MSG_ID_PAL_BT_CREATE_LOGICAL_LINK_COMMAND:
            {
                bt_a2mp_create_logical_cmd_req_struct *input_msg;
                
                input_msg = (bt_a2mp_create_logical_cmd_req_struct *)ilm_ptr->local_para_ptr;
                {
                    bt_a2mp_create_logical_cmd_cnf_struct *bt_msg;
                    
                    bt_msg =
                        (bt_a2mp_create_logical_cmd_cnf_struct*) construct_local_para(sizeof(bt_a2mp_create_logical_cmd_cnf_struct), TD_UL);
                
                    bt_msg->status = 0x00;   
                    HciSentAMPEvent(MSG_ID_PAL_BT_CREATE_LOGICAL_LINK_STATUS_EVENT, bt_msg);
                }
                {
                    bt_a2mp_logical_link_completed_evt_struct *bt_msg1;
                    
                    bt_msg1 =
                        (bt_a2mp_logical_link_completed_evt_struct*) construct_local_para(sizeof(bt_a2mp_logical_link_completed_evt_struct), TD_UL);
                
                    bt_msg1->status = 0x00;  
                    bt_msg1->physical_link_hdl = 10;
                    bt_msg1->logical_link_hdl = 96;
                    bt_msg1->tx_flow_spec_id = 10;
                    HciSentAMPEvent(MSG_ID_PAL_BT_LOGICAL_LINK_COMPLETE_EVENT, bt_msg1);
                    break;
                }
            }
#if 0        
            {
                bt_a2mp_create_logical_cmd_cnf_struct *bt_msg;
                
                bt_msg =
                    (bt_a2mp_create_logical_cmd_cnf_struct*) construct_local_para(sizeof(bt_a2mp_create_logical_cmd_cnf_struct), TD_UL);
        
                bt_msg->status = 0x00;   
                HciSentAMPEvent(MSG_ID_PAL_BT_CREATE_LOGICAL_LINK_STATUS_EVENT, bt_msg);
            }
            {
                bt_a2mp_logical_link_completed_evt_struct *bt_msg1;
                
                bt_msg1 =
                    (bt_a2mp_logical_link_completed_evt_struct*) construct_local_para(sizeof(bt_a2mp_logical_link_completed_evt_struct), TD_UL);
            
                bt_msg1->status = 0x00;  
                bt_msg1->physical_link_hdl = 10;
                bt_msg1->logical_link_hdl = 96;
                bt_msg1->tx_flow_spec_id = 10;
                HciSentAMPEvent(MSG_ID_PAL_BT_LOGICAL_LINK_COMPLETE_EVENT, bt_msg1);
                break;
            }
#endif            
            break; 
        case MSG_ID_PAL_BT_ACCEPT_LOGICAL_LINK_COMMAND:
            {
                bt_a2mp_accept_logical_cmd_req_struct *input_msg;
                
                input_msg = (bt_a2mp_accept_logical_cmd_req_struct *)ilm_ptr->local_para_ptr;
                {
                    bt_a2mp_accept_logical_cmd_cnf_struct *bt_msg;
                    
                    bt_msg =
                        (bt_a2mp_accept_logical_cmd_cnf_struct*) construct_local_para(sizeof(bt_a2mp_accept_logical_cmd_cnf_struct), TD_UL);
                
                    bt_msg->status = 0x00;   
                    HciSentAMPEvent(MSG_ID_PAL_BT_ACCEPT_LOGICAL_LINK_STATUS_EVENT, bt_msg);
                }
                {
                    bt_a2mp_logical_link_completed_evt_struct *bt_msg1;
                    
                    bt_msg1 =
                        (bt_a2mp_logical_link_completed_evt_struct*) construct_local_para(sizeof(bt_a2mp_logical_link_completed_evt_struct), TD_UL);
                
                    bt_msg1->status = 0x00;  
                    bt_msg1->physical_link_hdl = 10;
                    bt_msg1->logical_link_hdl = 96;
                    bt_msg1->tx_flow_spec_id = 10;
                    HciSentAMPEvent(MSG_ID_PAL_BT_LOGICAL_LINK_COMPLETE_EVENT, bt_msg1);
                    break;
                }
            }

#if 0
            {
                bt_a2mp_accept_logical_cmd_cnf_struct *bt_msg;
                
                bt_msg =
                    (bt_a2mp_accept_logical_cmd_cnf_struct*) construct_local_para(sizeof(bt_a2mp_accept_logical_cmd_cnf_struct), TD_UL);
        
                bt_msg->status = 0x00;   
                HciSentAMPEvent(MSG_ID_PAL_BT_ACCEPT_LOGICAL_LINK_STATUS_EVENT, bt_msg);
            }
            {
                bt_a2mp_logical_link_completed_evt_struct *bt_msg1;
                
                bt_msg1 =
                    (bt_a2mp_logical_link_completed_evt_struct*) construct_local_para(sizeof(bt_a2mp_logical_link_completed_evt_struct), TD_UL);
            
                bt_msg1->status = 0x00;  
                bt_msg1->physical_link_hdl = 10;
                bt_msg1->logical_link_hdl = 96;
                bt_msg1->tx_flow_spec_id = 10;
                HciSentAMPEvent(MSG_ID_PAL_BT_LOGICAL_LINK_COMPLETE_EVENT, bt_msg1);
                break;
            }
#endif            
            break; 
        case MSG_ID_PAL_BT_DISCONNECT_LOGICAL_LINK_COMMAND:
            {
                bt_a2mp_logical_link_disconnect_cmd_status_cnf_struct *bt_msg;
                
                bt_msg =
                    (bt_a2mp_logical_link_disconnect_cmd_status_cnf_struct*) construct_local_para(sizeof(bt_a2mp_logical_link_disconnect_cmd_status_cnf_struct), TD_UL);

                bt_msg->status = 0x00;   
                HciSentAMPEvent(MSG_ID_PAL_BT_DISCONNECT_LOGICAL_LINK_STATUS_EVENT, bt_msg);
            }
            {
                bt_a2mp_logical_link_disconnect_completed_evt_struct *bt_msg1;
                
                bt_msg1 =
                    (bt_a2mp_logical_link_disconnect_completed_evt_struct*) construct_local_para(sizeof(bt_a2mp_logical_link_disconnect_completed_evt_struct), TD_UL);
            
                bt_msg1->status = 0x00;  
                bt_msg1->logical_link_hdl = 96;
                bt_msg1->reason = 0x16;
                HciSentAMPEvent(MSG_ID_PAL_BT_DISCONNECT_LOGICAL_LINK_COMPLETE_EVENT, bt_msg1);
                break;
            }
            break;
        case MSG_ID_PAL_BT_FLOW_SPEC_MODIFY_COMMAND:
            {
                bt_a2mp_flow_spec_modify_cmd_status_cnf_struct *bt_msg;
                
                bt_msg =
                    (bt_a2mp_flow_spec_modify_cmd_status_cnf_struct*) construct_local_para(sizeof(bt_a2mp_flow_spec_modify_cmd_status_cnf_struct), TD_UL);

                bt_msg->status = 0x00;   
                HciSentAMPEvent(MSG_ID_PAL_BT_FLOW_SPEC_MODIFY_STATUS_EVENT, bt_msg);
            }
            {
                bt_a2mp_flow_spec_modify_completed_evt_struct *bt_msg1;
                
                bt_msg1 =
                    (bt_a2mp_flow_spec_modify_completed_evt_struct*) construct_local_para(sizeof(bt_a2mp_flow_spec_modify_completed_evt_struct), TD_UL);
            
                bt_msg1->status = 0x00;  
                bt_msg1->logical_link_hdl = 96;
                HciSentAMPEvent(MSG_ID_PAL_BT_FLOW_SPEC_MODIFY_COMPLETE_EVENT, bt_msg1);
                break;
            }
            break;
#endif

        default:
            //kal_trace(TRACE_GROUP_1, BTLOG_NONDIRECT_RETURN_MESSAGE);
            Report(("Unknown message id:%d",ilm_ptr->msg_id));
            break;
    }
}

#endif
