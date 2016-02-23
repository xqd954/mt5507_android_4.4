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

/***************************************************************************
 *
 * File:
 *     $Workfile:rfc_sm.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains the state machine for the RFCOMM protocol.
 *
 * Created:
 *     Sept 13, 1999
 * 
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "bt_common.h"
#include "btalloc.h"
#include "sys/debug.h"
#include "btaccctrl.h"

extern void btbmHandlerL2CAPDisconnectInd(BtRemoteDevice *remDev);

#if !defined (__BT_2_1_SIMPLE_PAIRING__)
extern BtStatus DDB_FindRecord(const BD_ADDR *bdAddr, BtDeviceRecord *record);
extern U8 RfFindRemDevMUXID(BtRemoteDevice *remDev);
#endif /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 
#if RFCOMM_PROTOCOL == XA_ENABLED

/* Function prototypes */
//static void RfCommandSent(U8 MuxId);
static void RfSendDmAck(U8 MuxId, U8 *Buffer);


/*****************************************************************************
 * FUNCTION
 *  RfDlcStateDisconnected
 * DESCRIPTION
 *  Handles the disconnected state of the DLC state machine.  A
 *  SABM is the only valid event.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfDlcStateDisconnected(RfChannel *Channel, U8 Event)
{
#if (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED)
    BtStatus status;
#endif /* (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED) */

    switch (Event)
    {
        case SABM:
            /* A connection was requested */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_RECV_SABM, Channel->dlci, Channel->muxId);

    #if (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED)
            /* Check security for this channel */
            Channel->state = DLC_STATE_SEC_PENDING;
            Channel->secToken.id = SEC_RFCOMM_ID;
            Channel->secToken.remDev = MUX(Channel->muxId).remDev;
            Channel->secToken.channel = Channel->dlci >> 1;
            Channel->secToken.incoming = TRUE;
    #if !defined (__BT_2_1_SIMPLE_PAIRING__)
            /* Check if this chnl is SIMAP chnl */
            if (Channel->secToken.channel == SIMAP_CHNL_NUM_START)
            {
                BtStatus db_search_status = BT_STATUS_FAILED;
                BtDeviceRecord record;

                db_search_status = DDB_FindRecord(&Channel->secToken.remDev->bdAddr, &record);
                if (db_search_status == BT_STATUS_SUCCESS)
                {
                    /* Paired with this device previously */

                    /* If there is no any active profile of this device, it is allowed to Reset state to do exchange link key& pin code again */
                    U8 muxid = RfFindRemDevMUXID(Channel->secToken.remDev);

                    if (muxid == NUM_BT_DEVICES)
                    {
                        Assert(0);
                    }

                    bt_trace(
                        TRACE_GROUP_1,
                        BT_LOG_REMDEV_REFCOUNT_DLCCOUNT,
                        Channel->secToken.remDev->refCount,
                        muxid,
                        MUX(muxid).dlcCount,
                        Channel->secToken.remDev);
                    if ((Channel->secToken.remDev->refCount == 1 || Channel->secToken.remDev->refCount == 2) && (MUX(muxid).dlcCount == 1))
                    {
                        /*
                         * remDev->refCount =1: it means that only one active L2CAP chnl, and this L2CAP chnl is used for RFCOMM
                         * remDev->refCount =2: one L2CAP chnl used for RFCOMM, another L2CAP chnl used for un-disconnected SDP (happens in PTS3.1.0)
                         * MUX(muxid).dlcCount: current number of allocated dlc for this link
                         */

                        Channel->secToken.remDev->simap_issue_authentication_procedure = TRUE;
                        Report(("Recv SIMAP RFChnl con req: org pin_len=%d,authState=%d,encryptState=%d",
                                record.pin_code_len, Channel->secToken.remDev->authState,
                                Channel->secToken.remDev->encryptState));
                        if (record.pin_code_len < 16)
                        {
                            Channel->secToken.remDev->authState = BAS_NOT_AUTHENTICATED;
                            if (Channel->secToken.remDev->secAccessState & BAS_AUTH_COMPLETE != 0)
                            {
                                Channel->secToken.remDev->secAccessState &= (~BAS_AUTH_COMPLETE);
                            }
                            Channel->secToken.remDev->encryptState = BES_NOT_ENCRYPTED;
                            record.paired_with_simap = FALSE;
                            bt_trace(TRACE_GROUP_1, BT_LOG_CHECK_SIMAP_PIN_CODE_LEN_IN_RECV_SABM_CASE, 0);
                        }
                        else if (record.pin_code_len == 16)
                        {
                            /*
                             * Although the link key is generated from 16-digits pin code length, 
                             * peer device may delete link key to trigger pin code request again!
                             * It is necessary to set the flag simap_issue_authentication_procedure as TRUE
                             * Flag simap_issue_authentication_procedure can be reset in two cases:
                             * 1) Peer device delets link key (PIN CODE REQ will be triggered latter), this flag is reset in SecHandleLinkKeyNotify()
                             * 2) Peer device keeps link key, this flag is reset in SecHandleAuthenticateComplete()
                             * 
                             * In this case: CarKit (Simap Client) has paired with us while connecting to L2CAP with PSM=0x03 (RFCOMM)
                             */
                            record.paired_with_simap = TRUE;
                            bt_trace(TRACE_GROUP_1, BT_LOG_CHECK_SIMAP_PIN_CODE_LEN_IN_RECV_SABM_CASE, 1);
                        }
                    }
                    else
                    {
                        /* This link still has other active profile or L2CAP chnl for othe app., it can not delete link key for this higher security */
                        bt_trace(TRACE_GROUP_1, BT_LOG_NEEDS_HIGHER_SECURITY_BUT_EXISTED_ACTIVE_CON_REJECT_THIS_CHNL);
                        Channel->state = DLC_STATE_DISCONNECTED;
                        RfSendDmAck(Channel->muxId, MUX(Channel->muxId).rxBuff);
                        RfFreeDLC(Channel->muxId, Channel->dlcId);
                        return;
                    }
                }
                else
                {
                    Channel->secToken.remDev->simap_issue_authentication_procedure = TRUE;
                    bt_trace(TRACE_GROUP_1, BT_LOG_CHECK_SIMAP_PIN_CODE_LEN_IN_RECV_SABM_CASE, 2);
                }
            }

    #endif /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 
            status = SEC_AccessRequest(&(Channel->secToken));
            if (status == BT_STATUS_SUCCESS)
            {
                bt_trace(TRACE_GROUP_1, RFCOMM_IN_COMING_CONNECTION_AUTHENTICATED_HAS_ALREADY_COMPLETED);
                Channel->state = DLC_STATE_CONN_INCOMING;
                RfAppCallback(Channel, BT_STATUS_SUCCESS, RFEVENT_OPEN_IND, 0);
            }
            else if (status != BT_STATUS_PENDING)
            {
                Channel->state = DLC_STATE_DISCONNECTED;
                RfSendDmAck(Channel->muxId, MUX(Channel->muxId).rxBuff);
                RfFreeDLC(Channel->muxId, Channel->dlcId);
            }
            else if (status == BT_STATUS_PENDING)
            {
                bt_trace(TRACE_GROUP_1, RFCOMM_LAYER_FOR_INCOMING_CONNECTION_EXEC_SERVICE_LEVEL_SECURITY);
            }

    #else /* (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED) */ /* (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED) */
            Channel->state = DLC_STATE_CONN_INCOMING;
            RfAppCallback(Channel, BT_STATUS_SUCCESS, RFEVENT_OPEN_IND, 0);
    #endif /* (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED) */ /* (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED) */
            break;

        case UIH_F:
        case UIH:
        case DISC:
            RfSendDmAck(Channel->muxId, MUX(Channel->muxId).rxBuff);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfDlcStateConnected
 * DESCRIPTION
 *  Handles the connected state of the DLC state machine.
 *  Expecting rx data, packet handled, or disconnect request.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfDlcStateConnected(RfChannel *Channel, U8 Event)
{
    switch (Event)
    {
        case DISC:
            /* Disconnect requested, notify app and send response */
            Channel->state = DLC_STATE_DISC_INCOMING;
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_RECV_DISC, Channel->dlci, Channel->muxId);
            OS_Report("RFCOMM: check2 dlcid:%d txcredit:%d rxc:%d flag:0x%x", Channel->dlcId, Channel->txCredit, Channel->rxCredit, MUX(Channel->muxId).flags);
            RfAppCallback(Channel, BT_STATUS_SUCCESS, RFEVENT_CLOSE_IND, 0);
            /* (void)RfSendAck(UA_F, Channel->muxId, Channel->dlci); */
            (void)RfSendDiscUAAck(Channel->muxId, Channel->dlci);
            break;

        case L2EVENT_PACKET_HANDLED:

            /* An application packet was handled */
            RfAppCallback(Channel, MUX(Channel->muxId).l2Status, RFEVENT_PACKET_HANDLED, 0);
            break;

        case UIH_F:
        case UIH:
            /* Data has been received for the application, check the length */
            if (MUX(Channel->muxId).rxLen <= Channel->maxFrameSize)
            {
                RfAppCallback(Channel, BT_STATUS_SUCCESS, RFEVENT_DATA_IND, 0);
            }
            else
            {
                /* Invalid RX length */
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_RECV_INVALID_LENGTH);
                if (RfDisconnectDLC(Channel->muxId, Channel->dlcId) != BT_STATUS_PENDING)
                {
                    RfDisconnectL2CAP(Channel->muxId);
                }
            }
            break;

        case DM_F:
        case DM:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_DM_REPORT, Channel->dlci, Channel->muxId);

            if (MUX(Channel->muxId).dlcCount == 1)
            {
                RfDisconnectL2CAP(Channel->muxId);
            }
            else
            {
                RfFreeDLC(Channel->muxId, Channel->dlcId);
            }
            break;

        case SABM:
            /* Unexpected (invalid) event received, tear down the MUX */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALID_EVENT, Channel->dlci, Channel->muxId);
            RfDisconnectL2CAP(Channel->muxId);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfDlcStateConnPending
 * DESCRIPTION
 *  Handles the connect pending state of the DLC state machine.
 *  Expecting a UA response (accept) or a DM (reject).
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfDlcStateConnPending(RfChannel *Channel, U8 Event)
{
    /* Waiting for a UA_F for the connection */
    switch (Event)
    {
        case UA_F:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_DLC_UP, Channel->dlci, Channel->muxId);            
            /*
             * RFCOMM_INVALID_DISC
             * If there is a DISC queued, we don't send out the MSC.
             * Note: The rfChFlags is possilbe to be changed in RfCommandSent().
             *       Therefore we have to check the flag before we call RfCommandSent().
             */
            if (Channel->rfChFlags & CHNL_FLAG_DISC_QUEUED)
            {
                RfCommandSent(Channel->muxId);
                return;
            }
            RfCommandSent(Channel->muxId);
            if (!(MUX(Channel->muxId).flags & MUX_FLAG_CREDIT_FLOW) && (Channel->rxCredit == 0))
            {
                Channel->rfSignals |= RF_FLOW;
            }
            if (RfSendModemStatus(Channel, Channel->rfSignals, 0, CR_BIT) == BT_STATUS_PENDING)
            {

                DLC(Channel->muxId, Channel->dlcId).flags &= ~DLC_FLAG_REQ_SENT;

                /* MSC has been sent, now we are open */
                Channel->state = DLC_STATE_CONNECTED;
                DLC(Channel->muxId, Channel->dlcId).flags |= DLC_FLAG_MSC_PENDING;
                RfAppCallback(Channel, BT_STATUS_SUCCESS, RFEVENT_OPEN, 0);
            }
            else
            {
                /* Free the channel, because L2CAP failed to send MSC */
                RfFreeDLC(Channel->muxId, Channel->dlcId);
            }
            break;

        case DM_F:
        case DM:
            /* Connection rejected */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_DLC_REJECT, Channel->dlci, Channel->muxId);

            /*
             * RFCOMM_INVALID_DISC
             * If there is a pending DISC in cmdQueue, we remove it from the cmdQueue.
             * That's because we don't need to send the DISC after we received the DM for SABM.
             */            
            if (Channel->rfChFlags & CHNL_FLAG_DISC_QUEUED)
            {
                ListEntry *head = &(MUX(Channel->muxId).cmdQueue);
                ListEntry *tmpNode = GetHeadList(head);
                BtPacket* packet = 0;
                U8 dlci = 0;
                
                while (tmpNode != head)
                {
                    packet = (BtPacket*) tmpNode;
                    dlci = packet->header[BT_PACKET_HEADER_LEN - 3] >> 2;
                    if (dlci == Channel->dlci)
                    {
                        RemoveEntryList(tmpNode);
                        InsertTailList(&(MUX(Channel->muxId).freeList), tmpNode);                                  
                        Channel->rfChFlags &= ~CHNL_FLAG_DISC_QUEUED;
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_DISCARD_DISC_ON_DLCI_OF_MUX, dlci, Channel->muxId);
                        break;
                    }                    
                    tmpNode = tmpNode->Flink;
                }
            }
            RfCommandSent(Channel->muxId);

            /* If this is the last DLC, shut down the mux */
            if (MUX(Channel->muxId).dlcCount == 1)
            {
                RfDisconnectL2CAP(Channel->muxId);
            }
            else
            {
                /* Free the channel */
                RfFreeDLC(Channel->muxId, Channel->dlcId);
            }
            break;

        case SABM:
            /* Connect race condition */
            RfSendDmAck(Channel->muxId, MUX(Channel->muxId).rxBuff);
            break;

        case UIH_F:
        case DISC:
        case UIH:
            /* Unexpected (invalid) event received, tear down the MUX */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALID_EVENT, Channel->dlci, Channel->muxId);

            RfDisconnectL2CAP(Channel->muxId);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfDlcStateConnIncoming
 * DESCRIPTION
 *  Handles the connect incoming state of the DLC state machine.
 *  Expecting a packet handled event, which indicates that the
 *  connect accept (UA) was sent.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfDlcStateConnIncoming(RfChannel *Channel, U8 Event)
{
    if (Event == L2EVENT_PACKET_HANDLED)
    {
        /* See what kind of reponse was made */
        if (MUX(Channel->muxId).txPacket->header[BT_PACKET_HEADER_LEN - 2] == UA_F)
        {
            /* The UA_F was sent successfully */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_UA_SEND, Channel->dlci, Channel->muxId);
            if (!(MUX(Channel->muxId).flags & MUX_FLAG_CREDIT_FLOW) && (Channel->rxCredit == 0))
            {
                Channel->rfSignals |= RF_FLOW;
            }

            if (RfSendModemStatus(Channel, Channel->rfSignals, 0, CR_BIT) == BT_STATUS_PENDING)
            {

                DLC(Channel->muxId, Channel->dlcId).flags &= ~DLC_FLAG_REQ_SENT;

                /* MSC has been sent, now we are open */
                Channel->state = DLC_STATE_CONNECTED;
                DLC(Channel->muxId, Channel->dlcId).flags |= DLC_FLAG_MSC_PENDING;
                RfAppCallback(Channel, BT_STATUS_SUCCESS, RFEVENT_OPEN, 0);
            }
            else
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_MSC_CAN_NOT_SEND);
                RfFreeDLC(Channel->muxId, Channel->dlcId);
            }
        }
        else
        {
            /* A DM_F was sent or an error occured on the previous send */
            U8 MuxID = Channel->muxId;
            RfFreeDLC(Channel->muxId, Channel->dlcId);
            /* FreeMux if dlciCount=0: for some headset: if MUX is not freed, it may not send txCredit at 2nd HFP con req */
            if (MUX(MuxID).dlcCount == 0)
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_DISC_L2CAP_REJECT_SABM_DLC_COUNT_ZERO,MuxID);
                RfDisconnectL2CAP(MuxID);
            }
        }
    }
    else if (Event == DISC)
    {
        /* Disconnect requested, notify app and send response */
        Channel->state = DLC_STATE_DISC_INCOMING;
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_RECV_DISC_FOR_DLC, Channel->dlci, Channel->muxId);
        RfAppCallback(Channel, BT_STATUS_SUCCESS, RFEVENT_CLOSE_IND, 0);
        /* (void)RfSendAck(UA_F, Channel->muxId, Channel->dlci); */
        (void)RfSendDiscUAAck(Channel->muxId, Channel->dlci);
    }
    else if (Event == SABM)
    {
        OS_Report("RFCOMM: Previous SABM is in progress, ignore it (DLCI=0x%02x Mux=0x%02x Event=0x%02x)", 
            Channel->dlci, Channel->muxId, Event);
    }
    else
    {
        /* Unexpected (invalid) event received, tear down the MUX */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALID_EVENT, Channel->dlci, Channel->muxId);

        RfDisconnectL2CAP(Channel->muxId);
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfDlcStateDiscPending
 * DESCRIPTION
 *  Handles the disconnect pending state for the DLC state machine.
 *  Expecting a UA or DM responce.  Packet handled events are
 *  passed up to the application for processing.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfDlcStateDiscPending(RfChannel *Channel, U8 Event)
{
    /* Waiting for a UA_F for the connection */
    switch (Event)
    {
        case DISC:
            /* Disconnect race condition */
            /* (void)RfSendAck(UA_F, Channel->muxId, Channel->dlci); */
            (void)RfSendDiscUAAck(Channel->muxId, Channel->dlci);
            /* Drop through to next case */
        case UA_F:
        case DM_F:
        case DM:
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_DISC_PENDING_RECV_EVENT, Channel->dlci, Channel->muxId, Event, MUX(Channel->muxId).flags);    
            if ((Event == DISC) &&  (MUX(Channel->muxId).flags & MUX_FLAG_CMD_SENT))
            {
                /* If DISC Req Packet or Last User Data Packet is still on btPacketList, it should not change its state to DLC_STATE_DISCONNECTED */
                /* But if the user data packet is still in btPacketList, then DISC is still in btPacketList: They must be sent in order */
                /* Only check the DISC Req Packet is necessary */
                int i =0/*, j=0*/;
                BtPacket* discPkt = NULL;
                //BOOL nodeOnListFlag = FALSE;
                      
                for (i=0; i < (NUM_RF_CHANNELS + 3); i ++)
                {
                    discPkt = &MUX(Channel->muxId).packets[i];
                    if ((discPkt->header[BT_PACKET_HEADER_LEN - 2] == DISC) 
                        && (discPkt->headerLen == 3)
                        && (!IsNodeOnList(&MUX(Channel->muxId).freeList, &discPkt->node))
                        )
                    {
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_CHECK_UNRETURNED_DISC_PKT,1);
                        return;
#if 0
                        /* Check if this DISC Req Packet is in L2CAP txLowPriorityQueue: if it is: it means the user data pkt is still in btPacketList and not sent */
                        nodeOnListFlag = IsNodeOnList (&(MUX(Channel->muxId).remDev->txLowPriorityQueue), &discPkt->node);
                        if (nodeOnListFlag == TRUE)
                        {
                            HciHandle link_hciHandle = MUX(Channel->muxId).remDev->hciHandle;
                            
                            /* DISC Req Packet is still in the txLowPriorityQueue, it can not change its DLC state! Because it may have user data pkts in btPacketList */
                            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_DISC_PENDING_NOT_CHANGE_STATE,1);
                            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_DISC_PENDING_NOT_CHANGE_RESULT,link_hciHandle,HCC(aclTxPacketsLeft));
                            return;
                        } 
                        
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_CHECK_UNRETURNED_DISC_PKT,2);

                        /* Check if this DISC Req Packet is in btPacketList */
                        for (j = 0; j < HCI_NUM_HANDLES; j ++)
                        {    
                            nodeOnListFlag = IsNodeOnList(&HCC(con)[j].btPacketList, &discPkt->node);
                            
                            if (nodeOnListFlag == TRUE)
                            {
                                /* DISC Req Packet is still in the btPacketList, it can not change its DLC state! */
                                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_DISC_PENDING_NOT_CHANGE_STATE,2);
                                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_DISC_PENDING_NOT_CHANGE_RESULT,HCC(con)[j].hciHandle,HCC(aclTxPacketsLeft));
                                return;
                            }
                        }    
#endif                        
                    }
                }
            }
            RfCommandSent(Channel->muxId);
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_DLCI_DISCONNECT, Channel->dlci, Channel->muxId);

            /* If this is the last DLC, shut down the mux */
            if (MUX(Channel->muxId).dlcCount == 1)
            {
                RfDisconnectL2CAP(Channel->muxId);
            }
            else
            {
                /* Free the channel */
                RfFreeDLC(Channel->muxId, Channel->dlcId);
            }
            break;
        }
        case L2EVENT_PACKET_HANDLED:
            if (MUX(Channel->muxId).txPacket->ulpContext != (void*)0)
            {
                RfAppCallback(Channel, MUX(Channel->muxId).l2Status, RFEVENT_PACKET_HANDLED, 0);
            }
            else
            {
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_DISC_PENDING_PKT_HANDLED,MUX(Channel->muxId).txPacket->header[BT_PACKET_HEADER_LEN - 2]);
            }
            break;

        case SABM:
            /* Unexpected (invalid) event received, tear down the MUX */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALID_EVENT_FOR_DLC, Channel->dlci, Channel->muxId);
            RfDisconnectL2CAP(Channel->muxId);
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfDlcStateDiscIncoming
 * DESCRIPTION
 *  Handles the disconnect incoming state of the DLC state machine.
 *  Expecting a packet handled event to indicate that the connect
 *  response was sent.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfDlcStateDiscIncoming(RfChannel *Channel, U8 Event)
{
    /* Waiting for UA_F packet to be handled */
    if (Event == L2EVENT_PACKET_HANDLED)
    {
        if (MUX(Channel->muxId).txPacket->ulpContext != (void*)0)
        {
            /* This is a user packet, pass it up */
            RfAppCallback(Channel, MUX(Channel->muxId).l2Status, RFEVENT_PACKET_HANDLED, 0);
        }
        else
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_DLCI_DISCONNECT, Channel->dlci, Channel->muxId);
            RfFreeDLC(Channel->muxId, Channel->dlcId);
        }
    }
    else
    {
        /* Unexpected (invalid) event received, tear down the MUX */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALID_EVENT_FOR_DLC, Channel->dlci, Channel->muxId);
        RfDisconnectL2CAP(Channel->muxId);
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfAdvanceNegPending
 * DESCRIPTION
 *  Advances the negotiate pending state.  If negOK is TRUE, a
 *  SABM is sent and the state is advanced to DLC_STATE_CON_PENDING.
 *  If negOK is FALSE, the channel is cleaned up and the state
 *  is advanced DLC_STATE_DISCONNECTED.
 * PARAMETERS
 *  Channel     [IN]        
 *  negOK       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfAdvanceNegPending(RfChannel *Channel, BOOL negOK)
{
    /* Send the SABM command */
    if (negOK == TRUE)
    {
        if (RfSendCommand(SABM, Channel->muxId, Channel->dlcId) == BT_STATUS_PENDING)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_SABM_SEND, Channel->dlci, Channel->muxId);
            Channel->state = DLC_STATE_CONN_PENDING;
        }
        else
        {
            /* Free the channel, because L2CAP failed to send SABM */
            RfFreeDLC(Channel->muxId, Channel->dlcId);
        }
    }
    else
    {
        /* Negotiation failed */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_NEGO_FAILED);
        /* If this is the last DLC, shut down the mux */
        if (MUX(Channel->muxId).dlcCount == 1)
        {
            RfDisconnectL2CAP(Channel->muxId);
        }
        else
        {
            /* Free the channel */
            RfFreeDLC(Channel->muxId, Channel->dlcId);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfDlcStateNegPending
 * DESCRIPTION
 *  A parameter negotiation was sent.  This state waits for the
 *  negotiation to be accepted or rejected.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfDlcStateNegPending(RfChannel *Channel, U8 Event)
{
    /*
     * Anything but a positive PN response will cause the 
     * * connect sequence to fail.
     */
    switch (Event)
    {
        case SABM:
            /* Could be a connect race condidition, let's try to be nice */
            RfSendDmAck(Channel->muxId, MUX(Channel->muxId).rxBuff);
            break;

        case UIH_F:
        case UA_F:
        case DISC:
        case UIH:
            /* Unexpected (invalid) event received, tear down the MUX */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALID_EVENT_FOR_DLC, Channel->dlci, Channel->muxId);
            RfDisconnectL2CAP(Channel->muxId);
            break;
        case DM_F:
        case DM:
            /* Disconnected mode - could be a reject */
            RfAdvanceNegPending(Channel, FALSE);
    }
}

#if (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED)


/*****************************************************************************
 * FUNCTION
 *  RfDlcStateSecPending
 * DESCRIPTION
 *  Security is being checked on an incoming/outgoing channel.
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfDlcStateSecPending(RfChannel *Channel, U8 Event)
{
    /* UNUSED_PARAMETER(Event); */

    if (Channel->secToken.incoming == TRUE)
    {
        if (Event == DISC)
        {
            /* Disconnect requested, notify app and send response */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_RECV_DISC_FOR_DLC, Channel->dlci, Channel->muxId);
            /* (void)RfSendAck(UA_F, Channel->muxId, Channel->dlci); */
            (void)RfSendDiscUAAck(Channel->muxId, Channel->dlci);
            RfFreeDLC(Channel->muxId, Channel->dlcId);
        }
        else
        {
            /* We shouldn't receive any other events in this state */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALID_EVENT_FOR_DLC, Channel->dlci, Channel->muxId);
            RfDisconnectL2CAP(Channel->muxId);
        }
    }
    else
    {
        /*
         * Race condition, the local application is already attempting
         * to connect to this channel and wait for security.
         */
        RfSendDmAck(Channel->muxId, MUX(Channel->muxId).rxBuff);
    }
}
#endif /* (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED) */


/*****************************************************************************
 * FUNCTION
 *  RfMuxStateOpen
 * DESCRIPTION
 *  Handles the open state of the RFCOMM multiplexer.  Events for
 *  allocated channels are passed to the DLC state machine.  If
 *  a channel is not allocated, then a SABM is the only valid event.
 *  Events for DLC 0 (control channel) are passed up to the
 *  control channel state machine (see RfMuxStateMachine()).
 * PARAMETERS
 *  MuxId       [IN]        
 *  Dlci        [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfMuxStateOpen(U8 MuxId, U8 Dlci, U8 Event)
{
    U8 dlcId;

#if NUM_RF_SERVERS != 0
    RfChannel *channel;
#endif /* NUM_RF_SERVERS != 0 */

    /* UNUSED_PARAMETER(Event); */

    if (RfFindDlcIdByDlci(MuxId, Dlci, &dlcId) != BT_STATUS_SUCCESS)
    {
        /*
         * No channel has been allocated yet, only a SABM is a valid
         * event (connect request).
         */
        if (Event == SABM)
        {
        #if NUM_RF_SERVERS != 0
            /* See if a registered service exists for the specified DLCI */
            if (BTAccCtrlCheckServicePolicy((U8) (Dlci >> 1)) == FALSE)
            {
                bt_trace(TRACE_GROUP_1, SERVER_CHANNEL_IS_NOT_ALLOWED);
                goto error_exit;
            }
            if (RfGetFreeServiceChannel(MuxId, (U8) (Dlci >> 1), &channel) == BT_STATUS_SUCCESS)
            {
                /* A service is registered */

                if (!(channel->rfChFlags & CHNL_FLAG_NEG_DONE))
                {
                    /*
                     * No negotiation took place, use the default
                     * RFCOMM frame size.
                     */
                    channel->maxFrameSize = RF_DEFAULT_FRAMESIZE;
                }

                /* Try to allocate the channel */
                if (RfAllocDLC(MuxId, Dlci, channel, &dlcId) == BT_STATUS_FAILED)
                {
                    /* Cannot alloc a channel, disconnected mode */
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_OUT_OF_RESOURCE, MuxId);
                    goto error_exit;
                }
                else
                {
                    /* A channel was allocated */
                    RFC(servers)[(Dlci >> 1) - 1].inUseCnt++;
                    DLC(MuxId, dlcId).channel = channel;
                    if (MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW)
                    {
                        /* Credit flow control has been negotiated */
                        DLC(MuxId, dlcId).flags |= DLC_FLAG_CREDIT_FLOW;
                    }
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_ALLOCATED_DLCI, Dlci, MuxId);
                }
            }
            else
        #endif /* NUM_RF_SERVERS != 0 */
            {
                /* No registered service exists */
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_MUX_NOT_REGJSTERED, Dlci >> 1, MuxId);

                goto error_exit;
            }
        }
        else if ((Event == DM) || (Event == DM_F) || (Event == DISC))
        {
            /*
             * It is possible a Parm Negotiation was made, but no connect 
             * attempt will be made.
             */
        #if NUM_RF_SERVERS != 0
            if (RfGetFreeServiceChannel(MuxId, (U8) (Dlci >> 1), &channel) == BT_STATUS_SUCCESS)
            {
                /* Cancel the timer and free the resource */
                EVM_CancelTimer(&(channel->timer));
                RfFreeUsePendingChannel(channel);
            }
        #endif /* NUM_RF_SERVERS != 0 */

            /* Respond with a DM_F */
            goto error_exit;
        }
        else if (Event == L2EVENT_PACKET_HANDLED)
        {
            /* Ignore this event */
            return;
        }
        else
        {
            /* Not a valid command for an unallocated channel */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_CMD_UNALLOCATED_MUX, MuxId);

            goto error_exit;
        }
    }

    /* Call the DLC state machine */
    RFC(state)[DLC(MuxId, dlcId).channel->state] (DLC(MuxId, dlcId).channel, Event);
    return;

  error_exit:

    /* disconnected mode */
    RfSendDmAck(MuxId, MUX(MuxId).rxBuff);
}


/*****************************************************************************
 * FUNCTION
 *  RfMuxStateConnPending
 * DESCRIPTION
 *  Handles the connect pending state of the RFCOMM multiplexer.
 *  Expecting an UA or DM event for DLC 0.  Parameter negotiation
 *  for the DLC requested by the application is initiated here.
 *  If DLC 0 is not up yet, it is initiated here as well.
 * PARAMETERS
 *  MuxId       [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfMuxStateConnPending(U8 MuxId, U8 Event)
{
    U16 l2capTxMtu = 0;
    
    if (Event == L2EVENT_CONNECTED)
    {
        /* Bring up DLC 0 */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_SABM_SENT, MuxId);
        AssertEval(RfSendCommand(SABM, MuxId, INVALID_DLC_ID) == BT_STATUS_PENDING);
    }
    else if (Event == UA_F)
    {
        /* Done initializing the multiplexer */
        RfCommandSent(MuxId);
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_MUX_UP, MuxId);

        MUX(MuxId).state = MUX_STATE_OPEN;

        /* Send Negotiation request for the DLC */
        l2capTxMtu = L2CAP_GetTxMtu(MUX(MuxId).l2ChannelId);
        if (l2capTxMtu > 5)
        {
            DLC(MuxId, MUX(MuxId).cDlcId).channel->maxFrameSize = min(l2capTxMtu - 5, DLC(MuxId, MUX(MuxId).cDlcId).channel->maxFrameSize);
        }   
        if (RfSendParmNegotiation(DLC(MuxId, MUX(MuxId).cDlcId).channel, CR_BIT) != BT_STATUS_PENDING)
        {

            /* Could not sent the negotiation */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_CANNT_SEND_NEGO, DLC(MuxId, MUX(MuxId).cDlcId).channel->dlci, MuxId);

            /* If this is the last DLC, shut down the mux */
            if (MUX(MuxId).dlcCount == 1)
            {
                RfDisconnectL2CAP(MuxId);
            }
            else
            {
                /* Free the channel */
                RfFreeDLC(MuxId, MUX(MuxId).cDlcId);
            }
        }
        else
        {
            /* Negotiation request sent */
            DLC(MuxId, MUX(MuxId).cDlcId).channel->state = DLC_STATE_NEG_PENDING;
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_NOGO_SEND_SUCCESS, DLC(MuxId, MUX(MuxId).cDlcId).channel->dlci, MuxId);
        }
    }
    else if ((Event == DM) || (Event == DM_F) || (Event == SABM) ||
             (Event == DISC) || (Event == UIH) || (Event == UIH_F))
    {
        /* Connection rejected (or invalid event) */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_CONN_DLCI_REJECT, MuxId);
        /* Shut down the mux */
        RfDisconnectL2CAP(MuxId);
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfMuxStateConnIncoming
 * DESCRIPTION
 *  Handles the connect incoming state of the RFCOMM multiplexer.
 * PARAMETERS
 *  MuxId       [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfMuxStateConnIncoming(U8 MuxId, U8 Event)
{
    /* Waiting for an SABM on DLC 0 */
    if (Event == SABM)
    {
        /* Respond to SABM */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_SEND_UA, MuxId);
        /* (void)RfSendAck(UA_F, MuxId, 0); */
        (void)RfSendDiscUAAck(MuxId, 0);
    }
    else if (Event == L2EVENT_PACKET_HANDLED)
    {
        /* The UA_F was sent sucessfully */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_MUX_UP, MuxId);

        MUX(MuxId).state = MUX_STATE_OPEN;
    }
    else if ((Event == UA_F) || (Event == DM_F) || (Event == DM) ||
             (Event == DISC) || (Event == UIH) || (Event == UIH_F))
    {
        /* Invalid event */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALUD_UNINIT_MUX, MuxId);

        RfDisconnectL2CAP(MuxId);
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfMuxStateMachine
 * DESCRIPTION
 *  State machine for specified Mux.
 * PARAMETERS
 *  MuxId       [IN]        
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfMuxStateMachine(U8 MuxId, U8 Event)
{
    U8 dlci = 0;
    U8 dlcId;
    RfChannel *channel;
    U8 *ptr;

    if (Event == L2EVENT_DISCONNECTED)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_L2CAP_DISCONNECT);

        RfFreeMux(MuxId);
        return;
    }

    /* Get the DLCI value for the event */
    if (Event == L2EVENT_PACKET_HANDLED)
    {

        ptr = &(MUX(MuxId).txPacket->header[BT_PACKET_HEADER_LEN - 1]);

        if ((MUX(MuxId).txPacket->ulpContext == (void*)0) || (MUX(MuxId).txPacket->ulpContext == (void*)1))
        {
            /* Internal packet */
            if (*(ptr - 1) & 0x01)
            {
                /* 1 byte length */
                dlci = *(ptr - 2) >> 2;
            }
            else
            {
                /* 2 byte length */
                dlci = *(ptr - 3) >> 2;
            }

            if (RfFindDlcIdByDlci(MuxId, dlci, &dlcId) == BT_STATUS_SUCCESS)
            {
                if (DLC(MuxId, dlcId).channel->rfChFlags & CHNL_FLAG_NO_RESOURCE)
                {
                    /* Tell the app that a resource is free */
                    DLC(MuxId, dlcId).channel->rfChFlags &= ~CHNL_FLAG_NO_RESOURCE;
                    RfAppCallback(DLC(MuxId, dlcId).channel, BT_STATUS_SUCCESS, RFEVENT_RESOURCE_FREE, 0);
                }
            }

            if (MUX(MuxId).txPacket->ulpContext == (void*)1)
            {
                /* Credit packet */
                bt_trace(TRACE_GROUP_1, BT_RFCOMM_CREDIT_PKT_SENT_DONE);
                return;
            }
            else
            {
                bt_trace(TRACE_GROUP_1,BT_RFCOMM_MUX_NON_USER_NON_CREDIT_PKT_HANDLED);
            }
        }
        else
        {
            /* User data */
            channel = (RfChannel*) (MUX(MuxId).txPacket->ulpContext);
            dlci = channel->dlci;
        }

    }
    else if (Event != L2EVENT_CONNECTED)
    {
        dlci = (MUX(MuxId).rxBuff[0] & 0xFC) >> 2;
    }

    /* Process the event for all other states */
    switch (MUX(MuxId).state)
    {
        case MUX_STATE_OPEN:
            if (dlci == 0)
            {
                switch (Event)
                {
                    case DISC:
                        /* Multiplexer is being shut down */
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_DISC_MUX, MuxId);
                        /* (void)RfSendAck(UA_F, MuxId, dlci); */
                        (void)RfSendDiscUAAck(MuxId, dlci);
                        break;
                    case DM:
                    case DM_F:
                    case SABM:
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_RECV_SABMDM);
                        RfDisconnectL2CAP(MuxId);
                        break;
                    default:
                        /* This event is for the mutliplexer control channel */
                        RfControlChannel(MuxId, Event);
                        break;
                }
            }
            else
            {
                /* This event is for an open DLC */
                RfMuxStateOpen(MuxId, dlci, Event);
            }
            break;
        case MUX_STATE_CONN_PENDING:
            /* Waiting for a UA_F response to DLC 0 */
            if (dlci == 0)
            {
                RfMuxStateConnPending(MuxId, Event);
            }
            else
            {
                /* Shut down the mux */
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALUD_UNINIT_MUX, MuxId);

                RfDisconnectL2CAP(MuxId);
            }
            break;
        case MUX_STATE_CONN_INCOMING:
            if (dlci == 0)
            {
                RfMuxStateConnIncoming(MuxId, Event);
            }
            else
            {
                /* Shut down the mux */
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALUD_UNINIT_MUX, MuxId);
                RfDisconnectL2CAP(MuxId);
            }
            break;

        case MUX_STATE_CLOSED:
            if ((Event == UA_F) || (Event == DM_F) || (Event == DM) ||
                (Event == DISC) || (Event == UIH) || (Event == SABM) || (Event == UIH_F))
            {
                /* This mux is not initialized - disconnected mode */
                RfSendDmAck(MuxId, MUX(MuxId).rxBuff);
            }
            break;
        default:
            Assert(0);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfL2Callback
 * DESCRIPTION
 *  Callback routine for L2CAP connections.
 * PARAMETERS
 *  L2ChannelId     [IN]        
 *  info            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfL2Callback(L2capChannelId L2ChannelId, L2capCallbackParms *info)
{
    BOOL validFrame = TRUE;
    U8 lenSize;
    U8 muxId;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    L2capConnSettings setting;
    BtRxPacket *rxPacket;
#endif

    switch (info->event)
    {
        case L2EVENT_CONNECT_IND:
            /* Request from a new machine.  A new multiplexer must be added */
            if (RfAllocMux(info->aclLink, L2ChannelId, &muxId) != BT_STATUS_SUCCESS)
            {
                /* Unable to allocate mux, reject the L2CAP connect indication */
                AssertEval(L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0) == BT_STATUS_PENDING);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_SEND_CONNECT_REJECT, muxId);
            }
            else
            {
                /* Accept the connection */
#if L2CAP_FLOW_NON_BASIC_MODE == XA_DISABLED
                if (L2CAP_ConnectRsp(L2ChannelId, L2CONN_ACCEPTED, 0) != BT_STATUS_PENDING)
#else
                setting.reasmBuffer = (ListEntry *)&MUX(muxId).rxPacketsBufferList;
                setting.receivedBuffer = (ListEntry *)&MUX(muxId).receivedPacketsList;
                setting.rxWindowSize = RF_NUM_RETRANSMISSION_WINDOW_SIZE;
                OS_MemSet(MUX(muxId).txMaxCountPerIFrame, 0, 64);
                setting.txMaxTransmitCount = (U8 *)MUX(muxId).txMaxCountPerIFrame;
                if (L2CAP_ConnectRsp(L2ChannelId, L2CONN_ACCEPTED, &setting) != BT_STATUS_PENDING)
#endif
                {
                    RfFreeMux(muxId);
                    return;
                }

                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INCOMING_CONN, muxId);
                MUX(muxId).state = MUX_STATE_CONN_INCOMING;
            }
            break;
        case L2EVENT_DISCONNECTED:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_L2CAP_DISCONNECT_EVENT);

        case L2EVENT_CONNECTED:
        #if XA_SNIFFER == XA_ENABLED
            if (info->event == L2EVENT_CONNECTED)
            {
                SnifferRegisterEndpoint(SPI_L2CAP, &L2ChannelId, info->aclLink, SPI_RFCOMM);
            }
        #endif /* XA_SNIFFER == XA_ENABLED */ 
            /* MTK continuing falling down to the next case */
        case L2EVENT_PACKET_HANDLED:
            if (RfFindMuxIdByRemDev(info->aclLink, &muxId) == BT_STATUS_SUCCESS)
            {
                /* Found the multiplexer */
                if (info->event == L2EVENT_PACKET_HANDLED)
                {
                    MUX(muxId).l2Status = (BtStatus) info->status;
                    MUX(muxId).txPacket = info->ptrs.packet;
                    if ((info->ptrs.packet->ulpContext == (void*)0) || (info->ptrs.packet->ulpContext == (void*)1))
                    {
                        if (
                    #if RF_SEND_TEST == XA_ENABLED
                               (info->ptrs.packet != &(MUX(muxId).testCmdPacket)) &&
                    #endif /* RF_SEND_TEST == XA_ENABLED */
                               (info->ptrs.packet != &(MUX(muxId).testRspPacket)))
                        {
                            InsertTailList(&(MUX(muxId).freeList), &(info->ptrs.packet->node));
                        }
                    }
                }

                else if (info->event == L2EVENT_DISCONNECTED)
                {
                    /* Check If SIM Access Profile connection exists or not */
                    bt_trace(
                        TRACE_GROUP_1,
                        RFL2CALLBACKL2EVENT_CLOSED_FOR_SECURITY_SERVICE_LEVEL2_TO_CHECK_PARING_STATE);
                    if (MUX(muxId).l2ChannelId != L2ChannelId)
                    {
                        Report(("Since L2ChannelId is different from that stored in MUX, just ignore it"));
                        break;
                    }
                }
                RfMuxStateMachine(muxId, info->event);
            }
            else if (info->event == L2EVENT_CONNECTED)
            {
                /* Connected, but no MUX exists */
                AssertEval(L2CAP_DisconnectReq(L2ChannelId) == BT_STATUS_PENDING);
            }
            break;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
        case L2EVENT_FLOW_DATA_IND:
            Report(("L2EVENT_FLOW_DATA_IND"));
            if (RfFindMuxIdByRemDev(info->aclLink, &muxId) == BT_STATUS_SUCCESS)
            {
                if(IsListEmpty(&MUX(muxId).receivedPacketsList))
                {
                    Report(("Data Flow ind, rx buffer shall not be empty"));
                    Assert(0);
                }
                rxPacket = (BtRxPacket *)RemoveHeadList(&MUX(muxId).receivedPacketsList);
                info->ptrs.data = rxPacket->data;
                info->dataLen = rxPacket->dataLen;
                rxPacket->dataLen = 0;
                InsertTailList((&MUX(muxId).rxPacketsBufferList), &rxPacket->node);
                L2CAP_UpdateSduDataBufferSeq(MUX(muxId).l2ChannelId);
            }
#endif
        case L2EVENT_DATA_IND:
            /* Handle incoming data */
            if (RfFindMuxIdByRemDev(info->aclLink, &muxId) == BT_STATUS_SUCCESS)
            {

                /* Found an existing Mux */
                MUX(muxId).rxBuff = info->ptrs.data;

                /* Determine size of the length field. */
                if ((U8) MUX(muxId).rxBuff[2] & 0x01)
                {
                    /* 1 Byte Length */
                    lenSize = 1;
                    MUX(muxId).rxLen = MUX(muxId).rxBuff[2] >> 1;
                }
                else
                {
                    /* 2 Byte Length */
                    lenSize = 2;
                    MUX(muxId).rxLen = (MUX(muxId).rxBuff[2] >> 1) | (MUX(muxId).rxBuff[3] << 7);
                }

                /* Check FCS */
                if ((MUX(muxId).rxBuff[1] == UIH) || (MUX(muxId).rxBuff[1] == UIH_F))
                {
                    if (MUX(muxId).rxBuff[1] == UIH_F)
                    {
                        /*
                         * UIH_F has a credit bytes. Here lenSize is increased to later
                         * * verify data length. 
                         */
                        lenSize++;
                    }

                    /* Calculate FCS on address and control bytes. */
                    validFrame = RfIsValidFCS(MUX(muxId).rxBuff, 2, MUX(muxId).rxBuff[info->dataLen - 1]);
                }
                else
                {
                    /* Calculate FCS on address, control, and length bytes. */
                    validFrame = RfIsValidFCS(
                                    MUX(muxId).rxBuff,
                                    (U8) (lenSize + 2),
                                    MUX(muxId).rxBuff[info->dataLen - 1]);
                }

                if (!validFrame)
                {
                    /* Shut down the multiplexer, and tear down L2CAP channel */
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALID_FCS, muxId);

                }

                /* Check length */
                if (validFrame)
                {
                    if (info->dataLen != (MUX(muxId).rxLen + lenSize + 3))
                    {
                        validFrame = FALSE;
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_INVALUD_RX_LENGTH, muxId);
                    }
                }

                if (!validFrame)
                {
                    RfDisconnectL2CAP(muxId);
                }
                else
                {
                    /* Valid frame received, Call the Mux state machine */
                    RfMuxStateMachine(muxId, MUX(muxId).rxBuff[1]);
                }
            }
            else
            {
                /* No mux found - disconnected mode */
                RfSendDmAck(muxId, MUX(muxId).rxBuff);
            }
            break;
#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
        case L2EVENT_LOCKSTEP_CONFIG_REQ:
            L2CAP_SendLockStepConfigRsp(L2ChannelId, BT_STATUS_SUCCESS);
            break;
#endif
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfSendCmdRsp
 * DESCRIPTION
 *  Sets up a command or response for transmistion and transmits.
 * PARAMETERS
 *  CmdRsp          [IN]        
 *  MuxId           [IN]        
 *  Packet          [IN]        
 *  priority        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus RfSendCmdRsp(U8 CmdRsp, U8 MuxId, BtPacket *Packet, U8 priority)
{
    BtStatus status = BT_STATUS_PENDING;
    U8 dlci = Packet->header[BT_PACKET_HEADER_LEN - 3] >> 2;    
    
    Report(("RfSendCmdRsp"));
    /* Initialize the transmit packet */
    Packet->header[BT_PACKET_HEADER_LEN - 2] = CmdRsp;
    Packet->header[BT_PACKET_HEADER_LEN - 1] = 0x01;
    Packet->headerLen = 3;
    Packet->flags = BTP_FLAG_TAIL;
    Packet->header[0] = RfCalcFCS(Packet->header + BT_PACKET_HEADER_LEN - 3, 3);
    Packet->tail = Packet->header;
    Packet->tailLen = 1;
    Packet->dataLen = 0;
    Packet->priority_type = priority;

    if ((CmdRsp == SABM) || (CmdRsp == DISC))
    {
        /* This is a command */
        if (MUX(MuxId).flags & MUX_FLAG_CMD_SENT)
        {
            /* Command outstanding, queue the command for later */
            InsertTailList(&(MUX(MuxId).cmdQueue), &(Packet->node));
            /* 
             * RFCOMM_INVALID_DISC:
             * We set CHNL_FLAG_DISC_QUEUED flag to denote DISC is queued in cmdQueue.
             */
            if (CmdRsp == DISC)
            {
                U8 dlcId = 0;
                /* It's abnormal to send DISC to an invalid dlci. */
                if (RfFindDlcIdByDlci(MuxId, dlci, &dlcId) == BT_STATUS_SUCCESS)
                {
                    DLC(MuxId, dlcId).channel->rfChFlags |= CHNL_FLAG_DISC_QUEUED;
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_DISC_IS_QUEUED_ON_DLCI_OF_MUX, dlci, MuxId);
                }
                else
                {
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_FATAL, 2);
                }                
            }
        }
        else
        {
            status = L2CAP_SendData(MUX(MuxId).l2ChannelId, Packet);
            if (status == BT_STATUS_PENDING)
            {
                /* Command was sent */
                if ((CmdRsp == SABM) && (dlci != 0))
                {
                    /* Make timeout shorter when authentication has been done */
                    if (MUX(MuxId).remDev->authState == BAS_AUTHENTICATED)
                    {
                        EVM_StartTimer(&(MUX(MuxId).timer), RF_AUTHORIZE_TIMEOUT);
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_START_TIMER_SEND_CMD_ON_DLCI_OF_MUX_TIME, CmdRsp, dlci, MuxId, RF_T1_TIMEOUT/1000);
                    }
                    else
                    {
                        EVM_StartTimer(&(MUX(MuxId).timer), RF_CONNECT_TIMEOUT);
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_START_TIMER_SEND_CMD_ON_DLCI_OF_MUX_TIME, CmdRsp, dlci, MuxId, RF_CONNECT_TIMEOUT/1000);
                    }
                }
                else
                {
                    EVM_StartTimer(&(MUX(MuxId).timer), RF_T1_TIMEOUT);
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_START_TIMER_SEND_CMD_ON_DLCI_OF_MUX_TIME, CmdRsp, dlci, MuxId, RF_T1_TIMEOUT/1000);
                }
                MUX(MuxId).flags |= MUX_FLAG_CMD_SENT;
            }
        }
    }
    else
    {
        /* This is a response */
        status = L2CAP_SendData(MUX(MuxId).l2ChannelId, Packet);
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfSendCommand
 * DESCRIPTION
 *  Routine for sending a command.
 * PARAMETERS
 *  Command     [IN]        
 *  MuxId       [IN]        
 *  DlcId       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfSendCommand(U8 Command, U8 MuxId, U8 DlcId)
{
    BtPacket *packet;
    U8 dlci;
    BtStatus status;

    /* Get a transmit packet */
    packet = RfCtlGetControlPacket(MuxId);
    if (packet == 0)
    {
        return BT_STATUS_NO_RESOURCES;
    }

#if 0    
    if (!(IsListEmpty(&(MUX(MuxId).freeList))))
    {
        packet = (BtPacket*) RemoveHeadList(&(MUX(MuxId).freeList));
    }
    else
    {
        return BT_STATUS_NO_RESOURCES;
    }
#endif

    if (DlcId == INVALID_DLC_ID)
    {
        dlci = 0;
    }
    else
    {
        dlci = DLC(MuxId, DlcId).channel->dlci;
    }

    if (MUX(MuxId).flags & MUX_FLAG_INITIATOR)
    {
        /* Command */
        packet->header[BT_PACKET_HEADER_LEN - 3] = ((dlci << 2) | EA_BIT) | CR_BIT;
    }
    else
    {
        /* Response */
        packet->header[BT_PACKET_HEADER_LEN - 3] = ((dlci << 2) | EA_BIT) & ~CR_BIT;
    }

    packet->ulpContext = (void*)0;
    if (Command != DISC)
    {
        status = RfSendCmdRsp(Command, MuxId, packet, BTP_FLAG_HIGH_PRIPORTY);
    }
    else
    {
        status = RfSendCmdRsp(Command, MuxId, packet, BTP_FLAG_LOW_PRIPORTY);
    }
    if (status != BT_STATUS_PENDING)
    {
        /* Send failed */
        InsertTailList(&(MUX(MuxId).freeList), &(packet->node));
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfCommandSent
 * DESCRIPTION
 *  Called when an acknowledgement has been received.
 * PARAMETERS
 *  MuxId       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
/* static */void RfCommandSent(U8 MuxId)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtPacket *packet;
    U8 cmd = 0;
    U8 dlci = 0;
    U8 dlcId = 0;

    Report(("RfCommandSent"));
    MUX(MuxId).flags &= ~MUX_FLAG_CMD_SENT;
    EVM_CancelTimer(&(MUX(MuxId).timer));

    if (!IsListEmpty(&(MUX(MuxId).cmdQueue)))
    {
        packet = (BtPacket*) RemoveHeadList(&(MUX(MuxId).cmdQueue));
        packet->priority_type = BTP_FLAG_HIGH_PRIPORTY;
        cmd = packet->header[BT_PACKET_HEADER_LEN - 2];
        dlci = packet->header[BT_PACKET_HEADER_LEN - 3] >> 2;
        
        if ((cmd != SABM) && (RfFindDlcIdByDlci(MuxId, dlci, &dlcId) != BT_STATUS_SUCCESS))
        {
            return;
        }
        status = L2CAP_SendData(MUX(MuxId).l2ChannelId, packet);
        if (status == BT_STATUS_PENDING)
        {
            if (cmd == SABM)
            {
                if (MUX(MuxId).remDev->authState == BAS_AUTHENTICATED)
                {
                    /* Make timeout shorter when authentication has been done */
                    EVM_StartTimer(&(MUX(MuxId).timer), RF_AUTHORIZE_TIMEOUT);
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_START_TIMER_SEND_CMD_ON_DLCI_OF_MUX_TIME, SABM, dlci, MuxId, RF_T1_TIMEOUT/1000);
                }
                else
                {
                    EVM_StartTimer(&(MUX(MuxId).timer), RF_CONNECT_TIMEOUT);
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_START_TIMER_SEND_CMD_ON_DLCI_OF_MUX_TIME, SABM, dlci, MuxId, RF_CONNECT_TIMEOUT/1000);
                }
            }
            else
            {
                EVM_StartTimer(&(MUX(MuxId).timer), RF_T1_TIMEOUT);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_START_TIMER_SEND_CMD_ON_DLCI_OF_MUX_TIME, packet->header[BT_PACKET_HEADER_LEN - 2], dlci, MuxId, RF_T1_TIMEOUT/1000);
                /*
                 * RFCOMM_INVALID_DISC
                 * We have to change state after passing DISC to L2CAP
                 */
                //status = RfFindDlcIdByDlci(MuxId, dlci, &dlcId);
                //if (status == BT_STATUS_SUCCESS)
                //{
                    DLC(MuxId, dlcId).channel->state = DLC_STATE_DISC_PENDING;
                    DLC(MuxId, dlcId).channel->rfChFlags &= ~CHNL_FLAG_DISC_QUEUED;
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_CHNL_FLAG_DISC_QUEUED_IS_CLEARED);
                //}
                //else
                //{                    
                    //bt_trace(BT_TRACE_G1_PROTOCOL, BT_FATAL, 1);
                //}
            }
            MUX(MuxId).flags |= MUX_FLAG_CMD_SENT;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfSendAck
 * DESCRIPTION
 *  Routine for sending an acknowledgement.
 * PARAMETERS
 *  Response        [IN]        
 *  MuxId           [IN]        
 *  Dlci            [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfSendAck(U8 Response, U8 MuxId, U8 Dlci)
{
    BtPacket *packet;
    BtStatus status;

    bt_trace(TRACE_GROUP_1, RFCOMM_SEND_ACK_RESPONSE, Response);
    packet = RfCtlGetControlPacket(MuxId);
    if (packet == 0)
    {
        return BT_STATUS_NO_RESOURCES;
    }

#if 0  
    /* Get a transmit packet */
    if (!IsListEmpty(&(MUX(MuxId).freeList)))
    {
        packet = (BtPacket*) RemoveHeadList(&(MUX(MuxId).freeList));
    }
    else
    {
        return BT_STATUS_NO_RESOURCES;
    }
#endif

    if (MUX(MuxId).flags & MUX_FLAG_INITIATOR)
    {
        packet->header[BT_PACKET_HEADER_LEN - 3] = ((Dlci << 2) | EA_BIT) & ~CR_BIT;
    }
    else
    {
        packet->header[BT_PACKET_HEADER_LEN - 3] = ((Dlci << 2) | EA_BIT) | CR_BIT;
    }

    packet->ulpContext = (void*)0;
    status = RfSendCmdRsp(Response, MuxId, packet, BTP_FLAG_HIGH_PRIPORTY);
    if (status != BT_STATUS_PENDING)
    {
        /* Send failed */
        InsertTailList(&(MUX(MuxId).freeList), &(packet->node));
    }
    if ((Response == DM_F) && (MUX(MuxId).dlcCount == 0))
    {
        RfDisconnectL2CAP(MuxId);
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfSendDiscUAAck
 * DESCRIPTION
 *  Routine for sending an UA acknowledgement.
 * PARAMETERS
 *  MuxId       [IN]        
 *  Dlci        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfSendDiscUAAck(U8 MuxId, U8 Dlci)
{
    BtPacket *packet;
    BtStatus status;

    /* Get a transmit packet */
    packet = RfCtlGetControlPacket(MuxId);
    if (packet == 0)
    {
        return BT_STATUS_NO_RESOURCES;
    }
    
#if 0    
    if (!IsListEmpty(&(MUX(MuxId).freeList)))
    {
        packet = (BtPacket*) RemoveHeadList(&(MUX(MuxId).freeList));
    }
    else
    {
        return BT_STATUS_NO_RESOURCES;
    }
#endif

    if (MUX(MuxId).flags & MUX_FLAG_INITIATOR)
    {
        packet->header[BT_PACKET_HEADER_LEN - 3] = ((Dlci << 2) | EA_BIT) & ~CR_BIT;
    }
    else
    {
        packet->header[BT_PACKET_HEADER_LEN - 3] = ((Dlci << 2) | EA_BIT) | CR_BIT;
    }

    packet->ulpContext = (void*)0;
    status = RfSendCmdRsp(UA_F, MuxId, packet, BTP_FLAG_LOW_PRIPORTY);
    if (status != BT_STATUS_PENDING)
    {
        /* Send failed */
        InsertTailList(&(MUX(MuxId).freeList), &(packet->node));
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfSendDmAck
 * DESCRIPTION
 *  Routine for sending a DM response.
 * PARAMETERS
 *  MuxId       [IN]        
 *  Buffer      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfSendDmAck(U8 MuxId, U8 *Buffer)
{
    if (Buffer[1] != DM_F)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_SEND_DM_F, (Buffer[0] & 0xFC) >> 2, MuxId);

        (void)RfSendAck(DM_F, MuxId, (U8) (Buffer[0] >> 2));
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfSendDataInternal
 * DESCRIPTION
 *  Send a UIH data packet.
 * PARAMETERS
 *  MuxId           [IN]        
 *  Dlci            [IN]        
 *  Packet          [IN]        
 *  priority        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfSendDataInternal(U8 MuxId, U8 Dlci, BtPacket *Packet, U8 priority)
{
    U8 offset;
    U8 hLen;
    U8 dlcId;
    BtStatus status;
    RfChannel *channel = (void*)0;

    offset = Packet->dataLen > 127 ? 4 : 3;
    Packet->priority_type = priority;
    /* See if credit flow control is being used and adjust header len */
    if (RfFindDlcIdByDlci(MuxId, Dlci, &dlcId) == BT_STATUS_SUCCESS)
    {
        channel = DLC(MuxId, dlcId).channel;
        if ((MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW) && (channel->rxCredit))
        {
            offset++;
        }
    }

    hLen = offset;

    /* Create the address */
    if (MUX(MuxId).flags & MUX_FLAG_INITIATOR)
    {
        Packet->header[BT_PACKET_HEADER_LEN - offset--] = ((Dlci << 2) | EA_BIT) | CR_BIT;
    }
    else
    {
        Packet->header[BT_PACKET_HEADER_LEN - offset--] = ((Dlci << 2) | EA_BIT) & ~CR_BIT;
    }

    /*
     * Set the UIH message type
     * * Check to see if credit flow control is enabled 
     * * and there is credit to grant.
     */
    if ((dlcId != INVALID_DLC_ID) && (MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW) && (channel->rxCredit))
    {
        Packet->header[BT_PACKET_HEADER_LEN - offset--] = UIH_F;
    }
    else
    {
        Packet->header[BT_PACKET_HEADER_LEN - offset--] = UIH;
    }

    /* Calculate the length using ISO/IEC 13239:1997 */
    if (Packet->dataLen > 127)
    {
        Packet->header[BT_PACKET_HEADER_LEN - offset--] = ((Packet->dataLen & 0x007F) << 1);
        Packet->header[BT_PACKET_HEADER_LEN - offset--] = ((Packet->dataLen & 0x7F80) >> 7);
    }
    else
    {
        Packet->header[BT_PACKET_HEADER_LEN - offset--] = ((Packet->dataLen << 1) | 0x01);
    }

    Packet->headerLen = hLen;

    /* Add the credit field */
    if ((dlcId != INVALID_DLC_ID) && (MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW) && (channel->rxCredit))
    {
        Packet->header[BT_PACKET_HEADER_LEN - offset--] = (U8) channel->rxCredit;
        channel->grantedCredit += channel->rxCredit;
        channel->rxCredit = 0;
    }

    Packet->flags &= BTP_FLAG_INUSE;
    Packet->flags |= BTP_FLAG_TAIL;
    Packet->header[0] = RfCalcFCS(Packet->header + BT_PACKET_HEADER_LEN - hLen, 2);
    Packet->tail = Packet->header;
    Packet->tailLen = 1;
    status = L2CAP_SendData(MUX(MuxId).l2ChannelId, Packet);
    return status;
}

#endif /* RFCOMM_PROTOCOL == XA_ENABLED */

