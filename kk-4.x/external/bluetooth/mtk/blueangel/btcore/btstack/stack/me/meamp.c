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

#include "bt_common.h"
#include "btalloc.h"
#include "sys/hci.h"
#include "utils.h"
#include "btconfig.h"
#include "sys/l2capxp.h"
#include "sys/sdpi.h"
#include "bt_feature.h"
#include "x_file.h"
#include "hci_amp.h"
#include "bt_adp_system.h"
#ifdef __BT_3_0_HS__
#include "meamp.h"
#include "bluetooth_a2mp_struct.h"

void Me_HandleLogicalLinkCompleteEvent(U8 len, U8 *parm, U8 controllerId);
void MeHandleAMPCompleteTask(U8 event, U8 len, U8 *parm);
void Me_AMPReportEvent(U8 *parm, U8 controllerId, U8 event_id);
void *Me_AMPAllocateMessageBuffer(U16 size);

extern BtRemoteAMPDevice *A2MP_FindAMPDevice(U8 controllerId, U8 physicalHandler);
extern BtRemoteAMPDevice *A2MP_FindAMPDeviceWhenPhysicalLinkDone(U8 controllerId);


#define RAMPDEV ((BtRemoteAMPDevice*)MEC(curOp))
#define RAMPLOGICALDEV ((BTRemoteAMPLogicalLink*)MEC(curOp))

void MeHandleCreatePhysicalLinkOp(BtOpEvent event)
{
    BtRemoteAMPDevice *link = 0;
    BtDeviceRecord record;
    BtRemoteDevice *aclLink = 0;

    link = ContainingRecord(RAMPDEV, BtRemoteAMPDevice, createPhysicalLinkOp);
    aclLink = ContainingRecord(link, BtRemoteDevice, remoteWifiAMP);


    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleAMPCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_STATUS;
#ifdef __BT_AMP_PAL_MESSAGE
            {
                bt_a2mp_create_physical_link_cmd_req_struct *msg;
                
                msg = (bt_a2mp_create_physical_link_cmd_req_struct *) 
                            Me_AMPAllocateMessageBuffer(sizeof(bt_a2mp_create_physical_link_cmd_req_struct));
                msg->physical_link_hdl = link->physical_link_handle;
                msg->dedicated_amp_key_length = 0x20;
                msg->dedicated_amp_key_type = 0x04;
                
                /* We have a remote device so see if a link key exists */
                if (SEC_FindDeviceRecord(&(aclLink->bdAddr), &record) == BT_STATUS_SUCCESS)
                {
                    switch(record.keyType)
                    {
                        case 0x04: /* unauthenticated combination key */
                            msg->dedicated_amp_key_type = 0x04; 
                            break;
                        case 0x05: /* authenticated combination key */
                            msg->dedicated_amp_key_type = 0x05;
                            break;
                        default:
                            msg->dedicated_amp_key_type = 0x03; /* Actually, it is not correct */
                            break;
                    }
                    memcpy(msg->dedicated_amp_key, record.dampKey, 32);
                }
                MEC(curTask) = HCC_CREATE_PHYSICAL_LINK;
                HciSentAMPCommand( MSG_ID_PAL_BT_CREATE_PHYSICAL_LINK_COMMAND, (local_para_struct *) msg, sizeof(bt_a2mp_create_physical_link_cmd_req_struct), BT_PAL_CONTROLLERID);
            }
#endif            
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            link->createPhysicalLinkOp.opType = BOP_NOP;
            Report(("MeHandleCreatePhysicalLinkOp complete"));
            Me_AMPReportEvent((U8 *)aclLink, BT_PAL_CONTROLLERID, BTEVENT_CREATE_PHYSICAL_LINK_CMD_STATUS);
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}

void MeHandleAcceptPhysicalLinkOp(BtOpEvent event)
{
    BtRemoteAMPDevice *link = 0;
    BtDeviceRecord record;
    BtRemoteDevice *aclLink = 0;

    link = ContainingRecord(RAMPDEV, BtRemoteAMPDevice, acceptPhysicalLinkOp);
    aclLink = ContainingRecord(link, BtRemoteDevice, remoteWifiAMP);

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleAMPCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_STATUS;
#ifdef __BT_AMP_PAL_MESSAGE
            {
                bt_a2mp_accept_physical_link_cmd_req_struct *msg;
                msg = (bt_a2mp_accept_physical_link_cmd_req_struct *) 
                            Me_AMPAllocateMessageBuffer(sizeof(bt_a2mp_accept_physical_link_cmd_req_struct));


                msg->physical_link_hdl = link->physical_link_handle;
                msg->dedicated_amp_key_type = 0x04;
                msg->dedicated_amp_key_length = 0x20;
                /* We have a remote device so see if a link key exists */
                if (SEC_FindDeviceRecord(&(aclLink->bdAddr), &record) == BT_STATUS_SUCCESS)
                {
                    switch(record.keyType)
                    {
                        case 0x03: /* debug combination key*/
                            msg->dedicated_amp_key_type = 0x03; 
                            break;
                        case 0x04: /* unauthenticated combination key */
                            msg->dedicated_amp_key_type = 0x04; 
                            break;
                        case 0x05: /* authenticated combination key */
                            msg->dedicated_amp_key_type = 0x05;
                            break;
                        default:
                            msg->dedicated_amp_key_type = 0x03; /* Actually, it is not correct */
                            break;
                    }
                    msg->dedicated_amp_key_type = 0x04;
                    msg->dedicated_amp_key_length = 0x20;                    
                    memcpy(msg->dedicated_amp_key, record.dampKey, 32);
                }
                MEC(curTask) = HCC_ACCEPT_PHYSICAL_LINK;
                HciSentAMPCommand( MSG_ID_PAL_BT_ACCEPT_PHYSICAL_LINK_COMMAND, (local_para_struct *) msg, sizeof(bt_a2mp_accept_physical_link_cmd_req_struct), BT_PAL_CONTROLLERID);
            }
#endif            
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            link->acceptPhysicalLinkOp.opType = BOP_NOP;
            Report(("MeHandleAcceptPhysicalLinkOp complete"));
            Me_AMPReportEvent((U8 *)aclLink, BT_PAL_CONTROLLERID, BTEVENT_ACCEPT_PHYSICAL_LINK_CMD_STATUS);
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}


void MeHandleDisconnectPhysicalLinkOp(BtOpEvent event)
{
    BtDisconnectAMPCommand *cmd = ContainingRecord((BtDisconnectAMPCommand*)MEC(curOp), BtDisconnectAMPCommand, disconnect);

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleAMPCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_STATUS;
#ifdef __BT_AMP_PAL_MESSAGE
            {
                bt_a2mp_physical_link_disconnect_cmd_req_struct *msg;
                msg = (bt_a2mp_physical_link_disconnect_cmd_req_struct *) 
                            Me_AMPAllocateMessageBuffer(sizeof(bt_a2mp_physical_link_disconnect_cmd_req_struct));

                msg->physical_link_hdl = cmd->physical_hdl;
                msg->reason = cmd->reason;
                MEC(curTask) = HCC_DISCONNECT_PHYSICAL_LINK;
                HciSentAMPCommand( MSG_ID_PAL_BT_DISCONNECT_PHYSICAL_LINK_COMMAND, (local_para_struct *) msg, sizeof(bt_a2mp_physical_link_disconnect_cmd_req_struct), BT_PAL_CONTROLLERID);
            }
#endif
            
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            cmd->disconnect.opType = BOP_NOP;
            Report(("MeHandleDisconnectPhysicalLinkOp complete"));
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}




void MeHandleCreateLogicalLinkOp(BtOpEvent event)
{
    BtRemoteAMPDevice *link = 0;
    BTRemoteAMPLogicalLink *logical_link;

    link = ContainingRecord(RAMPDEV, BtRemoteAMPDevice, createLogicalLinkOp);
    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleAMPCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_STATUS;
#ifdef __BT_AMP_PAL_MESSAGE
            {
                bt_a2mp_create_logical_cmd_req_struct *msg;
                msg = (bt_a2mp_create_logical_cmd_req_struct *) 
                            Me_AMPAllocateMessageBuffer(sizeof(bt_a2mp_create_logical_cmd_req_struct));

            	msg->physical_link_hdl = link->physical_link_handle;
                logical_link = &(link->logicalLink[link->currentLogicLinkOperationIndex]);
                memcpy(&msg->tx_flow_spec, logical_link->flow_spec.logical_link_tx_flow_spec, 16);
                memcpy(&msg->rx_flow_spec, logical_link->flow_spec.logical_link_tx_flow_spec, 16);                
                MEC(curTask) = HCC_CREATE_LOGICAL_LINK;
                HciSentAMPCommand(MSG_ID_PAL_BT_CREATE_LOGICAL_LINK_COMMAND, (local_para_struct *) msg, sizeof(bt_a2mp_create_logical_cmd_req_struct), BT_PAL_CONTROLLERID);
            }
#endif      
            return;
        case BOE_TASK_ERR:
            {
                U8 fakeFailedEvent[5];
                fakeFailedEvent[2] = 0x01;  
                StoreLE16(fakeFailedEvent+1, 0x00);
                fakeFailedEvent[3] = link->physical_link_handle;                
                fakeFailedEvent[4] = 0x00;                
                Me_HandleLogicalLinkCompleteEvent(5, fakeFailedEvent, BT_PAL_CONTROLLERID);
            }
        case BOE_TASK_END:
            link->createLogicalLinkOp.opType = BOP_NOP;
            Report(("MeHandleCreateLogicalLinkOp complete"));
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}

void MeHandleAcceptLogicalLinkOp(BtOpEvent event)
{
    BtRemoteAMPDevice *link = 0;
    BTRemoteAMPLogicalLink *logical_link;
    
    link = ContainingRecord(RAMPDEV, BtRemoteAMPDevice, acceptLogicalLinkOp);

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleAMPCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_STATUS;
#ifdef __BT_AMP_PAL_MESSAGE
            {
                bt_a2mp_accept_logical_cmd_req_struct *msg;
                msg = (bt_a2mp_accept_logical_cmd_req_struct *) 
                            Me_AMPAllocateMessageBuffer(sizeof(bt_a2mp_accept_logical_cmd_req_struct));
            
                msg->physical_link_hdl = link->physical_link_handle;
                logical_link = &(link->logicalLink[link->currentLogicLinkOperationIndex]);
                memcpy(&msg->tx_flow_spec, logical_link->flow_spec.logical_link_tx_flow_spec, 16);
                memcpy(&msg->rx_flow_spec, logical_link->flow_spec.logical_link_tx_flow_spec, 16);                
                MEC(curTask) = HCC_ACCEPT_LOGICAL_LINK;
                HciSentAMPCommand( MSG_ID_PAL_BT_ACCEPT_LOGICAL_LINK_COMMAND, (local_para_struct *) msg, sizeof(bt_a2mp_accept_logical_cmd_req_struct), BT_PAL_CONTROLLERID);
            }
#endif      
            return;
        case BOE_TASK_ERR:
            {
                U8 fakeFailedEvent[5];
                fakeFailedEvent[2] = 0x01;  
                StoreLE16(fakeFailedEvent+1, 0x00);
                fakeFailedEvent[3] = link->physical_link_handle;                
                fakeFailedEvent[4] = 0x00;                
                Me_HandleLogicalLinkCompleteEvent(5, fakeFailedEvent, BT_PAL_CONTROLLERID);
            }
        case BOE_TASK_END:
            link->acceptLogicalLinkOp.opType = BOP_NOP;
            Report(("MeHandleAcceptLogicalLinkOp complete"));
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();

}


void MeHandleDisconnectLogicalLinkOp(BtOpEvent event)
{
    BTRemoteAMPLogicalLink *logicalLink;
    BtRemoteAMPDevice *link;
    
    logicalLink = ContainingRecord(RAMPLOGICALDEV, BTRemoteAMPLogicalLink, disconnectLogicalLinkOp);
    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleAMPCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_STATUS;
#ifdef __BT_AMP_PAL_MESSAGE
            {
                bt_a2mp_logical_link_disconnect_cmd_req_struct *msg;
                msg = (bt_a2mp_logical_link_disconnect_cmd_req_struct *) 
                            Me_AMPAllocateMessageBuffer(sizeof(bt_a2mp_logical_link_disconnect_cmd_req_struct));

                msg->logical_link_hdl = logicalLink->logical_link_handle;
                link = (BtRemoteAMPDevice *)logicalLink->link;
                MEC(curTask) = HCC_DISCONNECT_LOGICAL_LINK;
                HciSentAMPCommand( MSG_ID_PAL_BT_DISCONNECT_LOGICAL_LINK_COMMAND, (local_para_struct *) msg, sizeof(bt_a2mp_logical_link_disconnect_cmd_req_struct), BT_PAL_CONTROLLERID);
            }
#endif      
            return;
        case BOE_TASK_ERR:
            logicalLink->logical_link_sate = BDS_DISCONNECTED;
        case BOE_TASK_END:
            logicalLink->disconnectLogicalLinkOp.opType = BOP_NOP;
            Report(("MeHandleDisconnectLogicalLinkOp complete"));
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}

void MeHandleLogicalLinkCancelOp(BtOpEvent event)
{
#if 0
    BtRemoteAMPDevice *link = 0;
    U8 *parm;
    
    link = ContainingRecord(RAMPDEV, BtRemoteAMPDevice, logicalLinkCancelOp);

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleAMPCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
#ifdef __BT_AMP_PAL_MESSAGE
            {
                BT_AMP_HCI_COMMAND_LOGICAL_LINK_CANCEL *msg;
                msg = (BT_AMP_HCI_COMMAND_LOGICAL_LINK_CANCEL *) 
                            Me_AMPAllocateMessageBuffer(sizeof(BT_AMP_HCI_COMMAND_LOGICAL_LINK_CANCEL));

            	msg->physical_link_handle = link->physical_link_handle;
            	msg->tx_flow_spec_id = link->creatingLogicalLinkTxFlowId;
                MEC(curTask) = HCC_LOGICAL_LINK_CANCEL;
                HciSentAMPCommand( 0, (local_para_struct *) msg, sizeof(BT_AMP_HCI_COMMAND_LOGICAL_LINK_CANCEL), link->ampControllerId);
            }
#endif      
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            parm = MEC(btEvent).p.hciAmpEvent.parms;            
            link->logicalLinkCancelOp.opType = BOP_NOP;
            Report(("MeHandleLogicalLinkCancelOp complete"));
            Me_AMPReportEvent((U8 *)parm, BT_PAL_CONTROLLERID, BTEVENT_LOGICAL_LINK_CANCEL);
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();
#endif
    Assert(0);
}

void MeHandleReadDataBlockSizeOp(BtOpEvent event)
{
    BT_AMP_HCI_EVENT_READ_BLOCK_SIZE result;
    U8 *parm;
    

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleAMPCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
#ifdef __BT_AMP_PAL_MESSAGE
            MEC(curTask) = HCC_READ_DATA_BLOCK_SIZE;
            HciSentAMPCommand( 0, NULL, 0, 0x01);
#endif
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            parm = (U8 *)MEC(btEvent).p.hciAmpEvent.parms;
            result.max_ack_data_packet_length = LEtoHost16(parm+1);
            result.data_block_length = LEtoHost16(parm+3);
            result.total_num_data_blocks = LEtoHost16(parm+5);
            HciSetAMPControllerTxBuffer(0x01, &result);
            MEC(readDataBlockSize).opType = BOP_NOP;
            Report(("MeHandleReadDataBlockSizeOp complete"));
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();

}

void MeHandleFlowSpecModifyOp(BtOpEvent event)
{
    BtRemoteAMPDevice *link = 0;
    BTRemoteAMPLogicalLink *logical_link;
    U8 *parm;

    logical_link = ContainingRecord(RAMPLOGICALDEV, BTRemoteAMPLogicalLink, flowSpecModifyOp);
    link = logical_link->link;

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleAMPCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_STATUS;
#ifdef __BT_AMP_PAL_MESSAGE
            {
                bt_a2mp_flow_spec_modify_cmd_req_struct *msg;
                msg = (bt_a2mp_flow_spec_modify_cmd_req_struct *) 
                            Me_AMPAllocateMessageBuffer(sizeof(bt_a2mp_flow_spec_modify_cmd_req_struct));

                msg->logical_link_hdl = logical_link->logical_link_handle;
                memcpy(&msg->tx_flow_spec, logical_link->flow_spec.logical_link_tx_flow_spec, 16);
                memcpy(&msg->rx_flow_spec, logical_link->flow_spec.logical_link_tx_flow_spec, 16);                
                MEC(curTask) = HCC_FLOW_SPEC_MODIFY;
                HciSentAMPCommand( MSG_ID_PAL_BT_FLOW_SPEC_MODIFY_COMMAND, (local_para_struct *) msg, sizeof(bt_a2mp_flow_spec_modify_cmd_req_struct), BT_PAL_CONTROLLERID);
            }
#endif      
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            link->flowSpecModifyOp.opType = BOP_NOP;
            parm = (U8 *)MEC(btEvent).p.hciAmpEvent.parms;            
            Report(("MeHandleFlowSpecModifyOp complete"));
            Me_AMPReportEvent((U8 *)parm, BT_PAL_CONTROLLERID, BTEVENT_AMP_FLOW_SPEC_MODIFY);
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}

void MeHandleReadLocalAmpInfo(BtOpEvent event)
{
    BtRemoteAMPDevice *link = 0;
    U8 *parm;

    link = ContainingRecord(RAMPDEV, BtRemoteAMPDevice, readLocalAmpInfo);

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleAMPCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
#ifdef __BT_AMP_PAL_MESSAGE
            {
                MEC(curTask) = HCC_READ_LOCAL_AMP_INFO;
                HciSentAMPCommand( MSG_ID_PAL_BT_READ_LOCAL_AMP_INFO_COMMAND, NULL, 0, BT_PAL_CONTROLLERID);
            }
#endif      
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            parm = (U8 *)MEC(btEvent).p.hciAmpEvent.parms;
            MEC(btEvent).p.hciAmpEvent.controllerId = BT_PAL_CONTROLLERID;
            link->readLocalAmpInfo.opType = BOP_NOP;
            Report(("MeHandleReadLocalAmpInfo complete"));
            Me_AMPReportEvent(parm, (U8)MEC(btEvent).p.hciAmpEvent.controllerId, BTEVENT_READ_LOCAL_AMP_INFO);
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();

}

void MeHandleReadLocalAmpAssoc(BtOpEvent event)
{
    BtRemoteAMPDevice *link = 0;
    U8 *parm;

    link = ContainingRecord(RAMPDEV, BtRemoteAMPDevice, readLocalAmpAssoc);

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleAMPCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
#ifdef __BT_AMP_PAL_MESSAGE
            {
                bt_a2mp_read_local_amp_assoc_cmd_req_struct *msg;
                msg = (bt_a2mp_read_local_amp_assoc_cmd_req_struct *) 
                            Me_AMPAllocateMessageBuffer(sizeof(bt_a2mp_read_local_amp_assoc_cmd_req_struct));

            	msg->physical_link_hdl = link->physical_link_handle;
            	msg->length_so_far = link->read_local_assoc.length_so_far;
            	msg->amp_assoc_length = link->read_local_assoc.amp_assoc_length;
                OS_Report("[PAL] length_so_far:%d amp_assoc_length:%d msg_len:%d", msg->length_so_far, msg->amp_assoc_length, sizeof(bt_a2mp_read_local_amp_assoc_cmd_req_struct));
                
                MEC(curTask) = HCC_READ_LOCAL_AMP_ASSOC;
                HciSentAMPCommand( MSG_ID_PAL_BT_READ_LOCAL_AMP_ASSOC_COMMAND, (local_para_struct *) msg, sizeof(bt_a2mp_read_local_amp_assoc_cmd_req_struct), BT_PAL_CONTROLLERID);
            }
#endif      
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            parm = (U8 *)MEC(btEvent).p.hciAmpEvent.parms;
            parm[0] = (U8)MEC(btEvent).p.hciAmpEvent.length;
            link->readLocalAmpAssoc.opType = BOP_NOP;
            Report(("MeHandleReadLocalAmpAssoc complete"));
            Me_AMPReportEvent((U8 *)parm, (U8)MEC(btEvent).p.hciAmpEvent.controllerId, BTEVENT_READ_LOCAL_AMP_ASSOC);
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();

}

void MeHandleWriteRemoteAmpAssoc(BtOpEvent event)
{
    BtRemoteAMPDevice *link = 0;
    U8 *parm;

    link = ContainingRecord(RAMPDEV, BtRemoteAMPDevice, writeRemoteAmpAssoc);

    switch (event)
    {
        case BOE_START:
            MEC(taskHandler) = MeHandleAMPCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
#ifdef __BT_AMP_PAL_MESSAGE
            {
                bt_a2mp_write_remote_amp_assoc_cmd_req_struct *msg;
                msg = (bt_a2mp_write_remote_amp_assoc_cmd_req_struct *) 
                            Me_AMPAllocateMessageBuffer(sizeof(bt_a2mp_write_remote_amp_assoc_cmd_req_struct));

            	msg->physical_link_hdl = link->physical_link_handle;
            	msg->length_so_far = link->write_remote_assoc.length_so_far;
                if((link->write_remote_assoc.amp_assoc_size - msg->length_so_far) < 248 )
                {
                    msg->amp_assoc_fragment_size = link->write_remote_assoc.amp_assoc_size - msg->length_so_far;
                }
                else
                {
                    msg->amp_assoc_fragment_size = 248;                    
                }
                memcpy(msg->amp_assoc_fragment, 
                       link->write_remote_assoc.amp_assoc_fragment + link->write_remote_assoc.length_so_far,
                       msg->amp_assoc_fragment_size);
                link->write_remote_assoc.length_so_far += msg->amp_assoc_fragment_size;
            	msg->amp_assoc_remaining_length = link->write_remote_assoc.amp_assoc_size - link->write_remote_assoc.length_so_far + msg->amp_assoc_fragment_size;
                
                MEC(curTask) = HCC_WRITE_REMOTE_AMP_ASSOC;
                HciSentAMPCommand( MSG_ID_PAL_BT_WRITE_REMOTE_AMP_ASSOC_COMMAND, (local_para_struct *) msg, sizeof(bt_a2mp_write_remote_amp_assoc_cmd_req_struct), BT_PAL_CONTROLLERID);
            }
#endif      
            return;
        case BOE_TASK_ERR:
        case BOE_TASK_END:
            parm = (U8 *)MEC(btEvent).p.hciAmpEvent.parms;
            link->writeRemoteAmpAssoc.opType = BOP_NOP;
            Report(("MeHandleWriteRemoteAmpAssoc complete"));
            Me_AMPReportEvent((U8 *)parm, BT_PAL_CONTROLLERID, BTEVENT_WRITE_REMOTE_AMP_ASSOC);
            break;
    }
    /* Start another operation if one exists */
    MeOperationEnd();

}

BtStatus ME_CreatePhysicalLink(BtRemoteAMPDevice *link)
{
    BtStatus status;

    if (IsNodeOnList(&(MEC(opList)), &(link->createPhysicalLinkOp.node)))
    {
        return BT_STATUS_FAILED;
    }
    if ((link->physical_link_sate != BDS_DISCONNECTED))
    {
        Assert(0);
        return BT_STATUS_FAILED;
    }
    link->physical_link_sate = BDS_OUT_CON;

    if (link->createPhysicalLinkOp.opType == BOP_NOP)
    {
        link->createPhysicalLinkOp.opType = BOP_CREATE_PHYSICAL_LINK;
        InsertTailList(&MEC(opList), &(link->createPhysicalLinkOp.node));
        MeStartOperation();
    }
    return BT_STATUS_PENDING;
}

BtStatus ME_AcceptPhysicalLink(BtRemoteAMPDevice *link)
{
    if (IsNodeOnList(&(MEC(opList)), &(link->acceptPhysicalLinkOp.node)))
    {
        return BT_STATUS_FAILED;
    }
    link->physical_link_sate = BDS_IN_CON;
   
    if (link->acceptPhysicalLinkOp.opType == BOP_NOP)
    {
        link->acceptPhysicalLinkOp.opType = BOP_ACCEPT_PHYSICAL_LINK;
        InsertTailList(&MEC(opList), &(link->acceptPhysicalLinkOp.node));
        MeStartOperation();
    }
    return BT_STATUS_PENDING;

}

extern BOOL BTCoreIsDeinitializing(void);

BtStatus ME_DisconnectPhysicalLink(BtRemoteAMPDevice *link, U8 reason)
{
    U8 i=0;
    BtDisconnectAMPCommand *cmd;
    
    Report(("ME_DisconnectPhysicalLink"));
    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        cmd = (BtDisconnectAMPCommand *)&MEC(disconnectAMPCommand[i]);
        if (IsNodeOnList(&(MEC(opList)), &(cmd->disconnect.node)))
        {
            Report(("Disconnect Op in OpList"));
        }
        else
        {
            Report(("Disconnect Op not in OpList"));
            break;
        }
    }
    if(i== NUM_BT_DEVICES)
    {
        Report(("No available disconnect Op not in OpList"));    
        return BT_STATUS_FAILED;
    }
    if(link->physical_link_sate == BDS_DISCONNECTED)
        return BT_STATUS_SUCCESS;
       
    if (cmd->disconnect.opType == BOP_NOP)
    {
        if(BTCoreIsDeinitializing() == TRUE)
        {
#ifdef __BT_AMP_PAL_MESSAGE
                bt_a2mp_physical_link_disconnect_cmd_req_struct *msg;
                msg = (bt_a2mp_physical_link_disconnect_cmd_req_struct *) 
                            Me_AMPAllocateMessageBuffer(sizeof(bt_a2mp_physical_link_disconnect_cmd_req_struct));

                msg->physical_link_hdl = cmd->physical_hdl;
                msg->reason = cmd->reason;
                BT_SendMessage(
                    MSG_ID_PAL_BT_DISCONNECT_PHYSICAL_LINK_COMMAND, 
                    MOD_PAL, 
                    (local_para_struct *) msg, 
                    sizeof(bt_a2mp_physical_link_disconnect_cmd_req_struct));
#endif
        }
        else
        {
            cmd->disconnect.opType = BOP_DISCONNECT_PHYSICAL_LINK;
            cmd->ampControllerId = BT_PAL_CONTROLLERID;
            cmd->physical_hdl = link->physical_link_handle;
            InsertTailList(&MEC(opList), &(cmd->disconnect.node));
            MeStartOperation();
        }

    }
    return BT_STATUS_PENDING;

}


BtStatus ME_DisconnectAMPPhysicalLinkByHandler(U8 physical_link_handle, U8 reason)
{
    U8 i=0;
    BtDisconnectAMPCommand *cmd;
    Report(("ME_DisconnectAMPPhysicalLinkByHandler"));

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        cmd = (BtDisconnectAMPCommand *)&MEC(disconnectAMPCommand[i]);
        if (IsNodeOnList(&(MEC(opList)), &(cmd->disconnect.node)))
        {
            Report(("Disconnect Op in OpList"));
        }
        else
        {
            Report(("Disconnect Op not in OpList"));
            break;
        }
    }
    if(i== NUM_BT_DEVICES)
    {
        Report(("No available disconnect Op not in OpList"));    
        return BT_STATUS_FAILED;
    }
       
    if (cmd->disconnect.opType == BOP_NOP)
    {
        cmd->disconnect.opType = BOP_DISCONNECT_PHYSICAL_LINK;
        cmd->ampControllerId = BT_PAL_CONTROLLERID;
        cmd->physical_hdl = physical_link_handle;
        InsertTailList(&MEC(opList), &(cmd->disconnect.node));
        MeStartOperation();
    }
    return BT_STATUS_PENDING;

}


void Me_SetCurrentLogicalLinkOperation(BtRemoteAMPDevice *link, U8 index)
{
    link->currentLogicLinkOperationIndex = index;
}

BtStatus ME_CreateLogicalLink(BtRemoteAMPDevice *link, U8 index)
{
    U8 i=0;
    
    if (IsNodeOnList(&(MEC(opList)), &(link->createLogicalLinkOp.node)))
    {
        return BT_STATUS_FAILED;
    }
    if ((link->logicalLink[index].logical_link_sate != BDS_DISCONNECTED))
    {
        return BT_STATUS_FAILED;
    }
    if (link->createLogicalLinkOp.opType == BOP_NOP)
    {
        link->logicalLink[index].logical_link_sate = BDS_OUT_CON;
        link->createLogicalLinkOp.opType = BOP_CREATE_LOGICAL_LINK;
        InsertTailList(&MEC(opList), &(link->createLogicalLinkOp.node));
        MeStartOperation();
    }
    return BT_STATUS_PENDING;

}

BtStatus ME_AcceptLogicalLink(BtRemoteAMPDevice *link, U8 index)
{
    if (IsNodeOnList(&(MEC(opList)), &(link->acceptLogicalLinkOp.node)))
    {
        return BT_STATUS_FAILED;
    }
    if ((link->logicalLink[index].logical_link_sate != BDS_DISCONNECTED))
    {
        return BT_STATUS_FAILED;
    }
    if (link->acceptLogicalLinkOp.opType == BOP_NOP)
    {
        link->logicalLink[index].logical_link_sate = BDS_OUT_CON;
        link->acceptLogicalLinkOp.opType = BOP_ACCEPT_LOGICAL_LINK;
        InsertTailList(&MEC(opList), &(link->acceptLogicalLinkOp.node));
        MeStartOperation();
    }
    return BT_STATUS_PENDING;

}


BtStatus ME_DisconnectLogicalLink(BtRemoteAMPDevice *link, U16 handler)
{
    U8 i=0;
    for(i=0; i< BT_AMP_SUPPORTED_LOGICAL_CHANNEL; i++)
    {
        if(link->logicalLink[i].logical_link_handle == handler)
            break;
    }
    if(i == BT_AMP_SUPPORTED_LOGICAL_CHANNEL)
        return BT_STATUS_FAILED;
    if (IsNodeOnList(&(MEC(opList)), &(link->logicalLink[i].disconnectLogicalLinkOp.node)))
    {
        return BT_STATUS_FAILED;
    }
    if ((link->logicalLink[i].logical_link_sate != BDS_CONNECTED))
    {
        return BT_STATUS_FAILED;
    }

    if (link->logicalLink[i].disconnectLogicalLinkOp.opType == BOP_NOP)
    {
        link->logicalLink[i].logical_link_sate = BDS_OUT_DISC;
        link->logicalLink[i].disconnectLogicalLinkOp.opType = BOP_DISCONNECT_LOGICAL_LINK;
        InsertTailList(&MEC(opList), &(link->logicalLink[i].disconnectLogicalLinkOp.node));
        MeStartOperation();
    }
    return BT_STATUS_PENDING;

}

BtStatus ME_LogicalLinkCancel(BtRemoteAMPDevice *link, U8 index)
{
    if (IsNodeOnList(&(MEC(opList)), &(link->logicalLinkCancelOp.node)))
    {
        return BT_STATUS_FAILED;
    }

    if ((link->logicalLink[index].logical_link_sate == BDS_CONNECTED))
    {
        Assert(0);
        return BT_STATUS_FAILED;
    }

    if (link->logicalLinkCancelOp.opType == BOP_NOP)
    {
        link->logicalLinkCancelOp.opType = BOP_LOGICAL_LINK_CANCEL;
        InsertTailList(&MEC(opList), &(link->logicalLinkCancelOp.node));
        MeStartOperation();
    }
    return BT_STATUS_PENDING;

}

BtStatus ME_FlowSpecModify(BTRemoteAMPLogicalLink *logical_link)
{
    if (IsNodeOnList(&(MEC(opList)), &(logical_link->flowSpecModifyOp.node)))
    {
        return BT_STATUS_FAILED;
    }
    if (logical_link->flowSpecModifyOp.opType == BOP_NOP)
    {
        logical_link->flowSpecModifyOp.opType = BOP_FLOW_SPEC_MODIFY;
        InsertTailList(&MEC(opList), &(logical_link->flowSpecModifyOp.node));
        MeStartOperation();
    }
    return BT_STATUS_PENDING;

}


BtStatus ME_ReadDataBlockSize(void)
{
#if 0
    if (IsNodeOnList(&(MEC(opList)), &(MEC(readDataBlockSize).node)))
    {
        return BT_STATUS_FAILED;
    }
    if (MEC(readDataBlockSize).opType == BOP_NOP)
    {
        MEC(readDataBlockSize).opType = BOP_READ_DATA_BLOCK_SIZE;
        InsertTailList(&MEC(opList), &(MEC(readDataBlockSize).node));
        MeStartOperation();
    }
#endif
    BT_AMP_HCI_EVENT_READ_BLOCK_SIZE result;

	result.max_ack_data_packet_length = 1492;
	result.data_block_length = 1492;
	result.total_num_data_blocks = 1;
    HciSetAMPControllerTxBuffer(0x01, &result);
    return BT_STATUS_PENDING;

}

BtStatus ME_ReadLocalAMPInfo(BtRemoteAMPDevice *link)
{
    if (IsNodeOnList(&(MEC(opList)), &(link->readLocalAmpInfo.node)))
    {
        return BT_STATUS_FAILED;
    }
    if (link->readLocalAmpInfo.opType == BOP_NOP)
    {
        link->readLocalAmpInfo.opType = BOP_READ_LOCAL_AMP_INFO;
        InsertTailList(&MEC(opList), &(link->readLocalAmpInfo.node));
        MeStartOperation();
    }
    return BT_STATUS_PENDING;

}

BtStatus ME_ReadLocalAMPAssoc(BtRemoteAMPDevice *link)
{
    if (IsNodeOnList(&(MEC(opList)), &(link->readLocalAmpAssoc.node)))
    {
    return BT_STATUS_FAILED;
    }
    if (link->readLocalAmpAssoc.opType == BOP_NOP)
    {
        link->readLocalAmpAssoc.opType = BOP_READ_LOCAL_AMP_ASSOC;
        InsertTailList(&MEC(opList), &(link->readLocalAmpAssoc.node));
        MeStartOperation();
    }
    return BT_STATUS_PENDING;

}

BtStatus ME_WriteRemoteAMPAssoc(BtRemoteAMPDevice *link)
{
    if (IsNodeOnList(&(MEC(opList)), &(link->writeRemoteAmpAssoc.node)))
    {
        return BT_STATUS_FAILED;
    }
    if (link->writeRemoteAmpAssoc.opType == BOP_NOP)
    {
        link->writeRemoteAmpAssoc.opType = BOP_WRITE_REMOTE_AMP_ASSOC;
        InsertTailList(&MEC(opList), &(link->writeRemoteAmpAssoc.node));
        MeStartOperation();
    }
    return BT_STATUS_PENDING;
    
}

void Me_HandlePhysicalLinkCompleteEvent(U8 len, U8 *parm, U8 controllerId)
{
	BT_AMP_HCI_EVENT_PHYSICAL_LINK_COMPLETE event;
    BtRemoteAMPDevice *link;

	event.status = parm[0];
	event.physical_link_hdl = parm[1];
    link = A2MP_FindAMPDeviceWhenPhysicalLinkDone(controllerId);
    Report(("Link:%08x", link));
    if( link != NULL )
    {
        Report(("parm[0]:%08x", parm[0]));
        Report(("link->physical_link_sate:%08x", link->physical_link_sate));

        if(parm[0] == 0x00)
        {
            link->physical_link_sate = BDS_CONNECTED;
        }
        else
            link->physical_link_sate = BDS_DISCONNECTED;
        Me_AMPReportEvent((U8 *)parm, controllerId, BTEVENT_PHYSICAL_LINK_COMPLETE);
    }
    else
    {
    
        Report(("event.status:%08x", event.status));
        if(event.status == 0x00)
        {
            OS_Report("[ERR] A2MP_FindAMPDevice link is NULL by id:%d", controllerId);
            ME_DisconnectAMPPhysicalLinkByHandler(event.physical_link_hdl, 0x13);
        }
    }
}

void Me_HandleChannelSelectedEvent(U8 len, U8 *parm, U8 controllerId)
{
    Me_AMPReportEvent((U8 *)parm, controllerId, BTEVENT_CHANNEL_SELECTED);
}

void Me_HandleDisconnectPhysicalLinkCompleteEvent(U8 len, U8 *parm, U8 controllerId)
{
	BT_AMP_HCI_EVENT_PHYSICAL_LINK_DISCONNECT event;
    BtRemoteAMPDevice *link;
    
	event.status = parm[0];
	event.physical_link_hdl = parm[1];
	event.reason = parm[2];
    link = A2MP_FindAMPDevice(controllerId, parm[1]);
    if(link == 0x00)
        return;
    if(link->physical_link_sate == BDS_WAIT_PHYSICALLINK_COLLISION_ACCEPT)
    {
        /* A2MP collision, need to wait the disconnected. */
        ME_AcceptPhysicalLink(link);
    }
    else

    {
        link->physical_link_sate = BDS_DISCONNECTED;
        Me_AMPReportEvent((U8 *)parm, controllerId, BTEVENT_DISCONNECT_PHYSICAL_LINK);
    }
}

void Me_HandlePhysicalLinkWarningEvent(U8 len, U8 *parm, U8 controllerId)
{
	BT_AMP_HCI_EVENT_PHYSICAL_LINK_WARNING event;
	
	event.physical_link_hdl = parm[0];
	event.link_loss_reason = parm[1];
    Me_AMPReportEvent((U8 *)parm, controllerId, BTEVENT_PHYSICAL_LINK_EARLY_WARNING);
}

void Me_HandlePhysicalLinkRecoveryEvent(U8 len, U8 *parm, U8 controllerId)
{
	BT_AMP_HCI_EVENT_PHYSICAL_LINK_RECOVERY event;

	event.physical_link_hdl = parm[0];
    Me_AMPReportEvent((U8 *)parm, controllerId, BTEVENT_PHYSICAL_LINK_RECOVERY);

}

void Me_HandleLogicalLinkCompleteEvent(U8 len, U8 *parm, U8 controllerId)
{
	BT_AMP_HCI_EVENT_LOGICAL_LINK_COMPLETE event;
    BtRemoteAMPDevice *link;
    U8 i = 0;
    U8 j = 0;
    
	event.status = parm[0];
	event.logical_link_hdl = LEtoHost16(parm+1);
	event.physical_link_hdl = parm[3];
	event.tx_flow_spec_id = parm[4];
    link = A2MP_FindAMPDevice(controllerId, parm[3]);
    
    if(parm[0] == 0x00)
    {
        HciAddAMPConnection((HciHandle) (LEtoHost16(&(parm[1])) & 0x0FFF)/*, 0x01*/);
        if(link == 0x00)
            return;
        if(link->currentLogicLinkOperationIndex == 0xFF)
            Assert(0);
        /* BDS_LOGICAL_CON is used to notify the logical conn event belongs to */
        i = link->currentLogicLinkOperationIndex;

        link->logicalLink[i].logical_link_sate = BDS_LOGICAL_CON;
        link->logicalLink[i].logical_link_handle= event.logical_link_hdl;
        link->currentLogicLinkOperationIndex = 0xFF;
    }
    else
    {
        if(link == 0x00)
            return;    
        if(link->currentLogicLinkOperationIndex == 0xFF)
            Assert(0);
        /* BDS_LOGICAL_CON is used to notify the logical conn event belongs to */
        i = link->currentLogicLinkOperationIndex;

        link->logicalLink[i].logical_link_sate = BDS_LOGICAL_CON;
        link->currentLogicLinkOperationIndex = 0xFF;

    }
    if(link->logicalLink[i].logical_link_type <= 0x01)
    {
        for(j=0;j< BT_AMP_SUPPORTED_LOGICAL_CHANNEL; j++)
        {
            if((link->logicalLink[j].logical_link_type <= 0x01) &&
                (link->logicalLink[j].logical_link_sate == BDS_OUT_CON))
            {
                link->logicalLink[j].logical_link_handle = event.logical_link_hdl;
                link->logicalLink[i].logical_link_sate = BDS_LOGICAL_CON;
            }
        }
    }
           
    Me_AMPReportEvent((U8 *)parm, controllerId, BTEVENT_LOGICAL_LINK_COMPLETE);
}

void Me_HandleDisconnectLogicalLinkCompleteEvent(U8 len, U8 *parm, U8 controllerId)
{
	BT_AMP_HCI_EVENT_LOGICAL_LINK_DISCONNECT event;
    BtRemoteAMPDevice *link = 0;
    BtRemoteAMPDevice *tempLink;
    
    BtRemoteDevice *remDev;
    U8 i=0;
    U8 j=0;
    U8 *logicalLink = 0;
        
	event.status = parm[0];
	event.logical_link_hdl = LEtoHost16(parm+1);
	event.reason = parm[3];

    for (j = 0; j < NUM_BT_DEVICES; j++)
    {
        remDev = &(MEC(devTable)[j]);
        if (remDev->state != BDS_DISCONNECTED)
        {
            tempLink = (BtRemoteAMPDevice *)&remDev->remoteWifiAMP;
            for(i=0;i<BT_AMP_SUPPORTED_LOGICAL_CHANNEL;i++)
            {
                Report(("index i:%d", i));
                Report(("index logical_link_sate:%d", tempLink->logicalLink[i].logical_link_sate));            
                Report(("LEtoHost16(parm+1):%d", LEtoHost16(parm+1)));     
                if(event.logical_link_hdl == tempLink->logicalLink[i].logical_link_sate)
                {   
                    link = tempLink;
                    break;
                }
            }
        }
    }

    if(link == 0x00)
    {
        Report(("Not found logical link handler"));
        return;
    }
    
    Report(("Me_HandleDisconnectLogicalLinkCompleteEvent: parm[0]:%d", parm[0]));
    if(parm[0] == 0x00)
    {
        HciDeleteAMPConnection((HciHandle) (LEtoHost16(&(parm[1])) & 0x0FFF));
        if(link == 0x00)
            return;
        for(i=0;i<BT_AMP_SUPPORTED_LOGICAL_CHANNEL;i++)
        {
            Report(("index i:%d", i));
            Report(("index logical_link_sate:%d", link->logicalLink[i].logical_link_sate));            
            Report(("LEtoHost16(parm+1):%d", LEtoHost16(parm+1)));                        
        
            if((link->logicalLink[i].logical_link_sate >= BDS_CONNECTED) &&
                (link->logicalLink[i].logical_link_handle == LEtoHost16(parm+1)))
            {
                link->logicalLink[i].logical_link_sate = BDS_DISCONNECTED;
                link->logicalLink[i].logical_link_handle = 0;
                logicalLink = (U8 *)&link->logicalLink[i];
                Me_AMPReportEvent((U8 *)logicalLink, controllerId, BTEVENT_DISCONNECT_LOGICAL_LINK);
            }
        }
    }
}


void Me_HandleNumOfCompletedDataBlockEvent(U8 len, U8 *parm, U8 controllerId)
{
    U16 handles;
    U16 totalNumDataBlocks;
    U8 numHandles;
    U16 numPacketsComplete;
    U16 numOfBlock;
    I16 i;

    totalNumDataBlocks = LEtoHost16(parm);
    numHandles = parm[2];
    for (i = 0; i < numHandles; i++)
    {
        handles = LEtoHost16(parm+(i*4+3));
        numPacketsComplete = LEtoHost16(parm + 2 +(i*4+3));
        numOfBlock = LEtoHost16(parm + 4 +(i*4+3));        
        HciReportAMPDataBlocksEvent(handles,numPacketsComplete,numOfBlock);
    }

}

void Me_HandleAMPStatusChangeEvent(U8 len, U8 *parm, U8 controllerId)
{
	BT_AMP_HCI_EVENT_AMP_STATUS_CHANGE event;

	event.status = parm[0];
	event.amp_Status = parm[1];
    Me_AMPReportEvent((U8 *)parm, controllerId, BTEVENT_AMP_STATUS_CHANGE);
    
}

void MeHandleAMPCompleteTask(U8 event, U8 len, U8 *parm)
{
	U16 opcode;
    
	opcode = LEtoHost16(parm);
    switch (event)
    {
        case HCE_COMMAND_COMPLETE:
            opcode = LEtoHost16((parm + 3));
            MEC(btEvent).errCode = parm[5];
            MEC(btEvent).p.hciAmpEvent.length = len-5;
            MEC(btEvent).p.hciAmpEvent.controllerId = parm[0];    
            MEC(btEvent).p.hciAmpEvent.parms = parm + 5;    
            break;
        case HCE_COMMAND_STATUS:
            /* Save the status in case it is an error */
            opcode = LEtoHost16((parm + 4));
            MEC(btEvent).errCode = parm[2];
            MEC(btEvent).p.hciAmpEvent.length = len-6;
            MEC(btEvent).p.hciAmpEvent.controllerId = parm[0];    
            MEC(btEvent).p.hciAmpEvent.parms = parm + 6;    
            break;
        default:
            Assert(0);
    }

    /* Length infomation is inside, we use parm[1] to pass the controller Id. */
    /* So we are not required to change the function API. */
    
    /* Make sure the opcode matches. If not ignore the response */
    if (opcode == MEC(curTask))
    {
        /* Clear the task if this is the end. */
        if ((event != HCE_COMMAND_STATUS) || ((event == HCE_COMMAND_STATUS) && (parm[3] != 0)))
        {
            MEC(curTask) = 0;
        }

        /* Call the operation handler */
        if (MEC(btEvent).errCode == 0)
        {
            MEC(opHandler) (BOE_TASK_END);
        }
        else
        {
            bt_trace(TRACE_GROUP_1, MEHCI_COMMAND_COMPLETE_ERROR, MEC(btEvent).errCode);
            MEC(opHandler) (BOE_TASK_ERR);
        }
    }
    else
    {
        bt_trace(TRACE_GROUP_1, ME_MEHANDLECOMPLETETASK_OPCODE_MISMATCH_OPCODE__xX_CURTASK__xX, opcode, MEC(curTask));
    }
}

void Me_HandleAmpRxHCIEventMessage(U8 *parm, U16 length, U8 controllerId)
{
    U16 opcode;
    U8 event;
    U8 *data;
    BOOL useTaskHandler = FALSE;
    event = parm[0];
    data = parm + 2;
    if(event != 72)    
        Report(("Me_HandleAmpRxHCIEventMessage event:%d", event));
    switch (event)
    {
        case HCE_COMMAND_COMPLETE:
            opcode = LEtoHost16((parm + 3));
            useTaskHandler = TRUE;
            break;
        case HCE_COMMAND_STATUS:
            opcode = LEtoHost16((parm + 4));
            useTaskHandler = TRUE;
            break;
        case HCE_PHYSICAL_LINK_COMPLETE:
            Me_HandlePhysicalLinkCompleteEvent(length, data, controllerId);
            return;
        case HCE_CHANNEL_SELECTED:
            Me_HandleChannelSelectedEvent(length, data, controllerId);
            return;
        case HCE_DISCONNECT_PHYSICAL_LINK:
            Me_HandleDisconnectPhysicalLinkCompleteEvent(length, data, controllerId);
            return;
        case HCE_PHYSICAL_LINK_EARLY_WARNING:
            Me_HandlePhysicalLinkWarningEvent(length, data, controllerId);
            return;
        case HCE_PHYSICAL_LINK_RECOVERY:
            Me_HandlePhysicalLinkRecoveryEvent(length, data, controllerId);
            return;
        case HCE_LOGICAL_LINK_COMPLETE:
            Me_HandleLogicalLinkCompleteEvent(length, data, controllerId);
            return;
        case HCE_DISCONNECT_LOGICAL_LINK:
            Report(("HCE_DISCONNECT_LOGICAL_LINK"));
            Me_HandleDisconnectLogicalLinkCompleteEvent(length, data, controllerId);
            return;                
        case HCE_NUM_OF_COMPLETE_DATA_BLOCKS:
            {
                U32 rx_timer_tick;
                rx_timer_tick = BTCoreGetSystemTick1();
            }
            Me_HandleNumOfCompletedDataBlockEvent(length, data, controllerId);
            return;
        case HCE_AMP_STATUS_CHANGE:
            Me_HandleAMPStatusChangeEvent(length, data, controllerId);
            return;
        default:
            opcode = 0;
            return;
    }
    
    /* Use the taskHandler if it matched above. */
    if ((MEC(taskHandler) != 0) && (useTaskHandler == TRUE))
    {
        /* Call the task handler */
        /* Length infomation is inside, we use parm[1] to pass the controller Id. */
        /* So we are not required to change the function API. */
        parm[0] = controllerId; 
        MEC(taskHandler) (event, length, parm);
    }
    else
    {
        return;
        Assert(0);
    }
}
void *Me_AMPAllocateMessageBuffer(U16 size)
{
    void *bt_msg;

    bt_msg =
        (void*) construct_local_para(size, TD_UL);

    return bt_msg;
}

void Me_AMPReportEvent(U8 *parm, U8 controllerId, U8 event_id)
{
    MEC(btEvent).p.hciAmpEvent.parms = parm;
    MEC(btEvent).p.hciAmpEvent.controllerId = controllerId;
    MEC(btEvent).eType = event_id;
    MeReportNMResults();

}


#ifdef __A2MP_UT__
void Me_SendA2MPPhysicalCompleteEvent(U8 status, U8 physical_link_hdl)
{
    bt_a2mp_physical_link_completed_evt_ind_struct *bt_msg;

    bt_msg =
        (bt_a2mp_physical_link_completed_evt_ind_struct*) construct_local_para(sizeof(bt_a2mp_physical_link_completed_evt_ind_struct), TD_UL);
    bt_msg->status = status;
    bt_msg->physical_link_hdl = physical_link_hdl;                
    HciSentAMPEvent(MSG_ID_PAL_BT_PHYSICAL_LINK_COMPLETE_EVENT, bt_msg);
}

#endif
#endif
