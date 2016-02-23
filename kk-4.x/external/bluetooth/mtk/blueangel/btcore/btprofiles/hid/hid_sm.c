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
 *     $Workfile:hid_sm.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description:
 *     State machine for the HID profile.
 *
 * Created:
 *     Aug 19, 2002
 *
 * Copyright 2002-2004 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, 
 * Inc.  This work contains confidential and proprietary information of 
 * Extended Systems, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#if defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__)

//#include "kal_release.h"
//#include "bluetooth_trc.h"
#include <sys/hidalloc.h>
#include <sys/hidi.h>
#include "conmgr.h"

#ifdef __BT_4_0_BLE__
extern BtStatus GattClientConnect(BtRemoteDevice *link);
extern BtStatus GattDisconnect(BtRemoteDevice *link);
#endif


/*---------------------------------------------------------------------------
 *            HidCmgrCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Called by connection manager with link state events.
 *
 * Return:    (See header file)
 *
 */
void HidCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status) 
{
    BtStatus ret;
    HidChannel *channel = ContainingRecord(Handler, HidChannel, cmgrHandler);
    kal_trace(BT_TRACE_G2_PROFILES,HIDCMGRCALLBACKx02X ,Event);
    switch (Event) {
    case CMEVENT_DATA_LINK_CON_CNF:
        if (channel == NULL)
        {
            kal_trace(BT_TRACE_G2_PROFILES,HID_NOT_THE_CONNECT_BD_ADDR);
            return;
        }

        if(channel->cmgrHandler.bdc == 0)
        {
            kal_trace(BT_TRACE_G2_PROFILES,HIDCHANNELCMGRHANDLERBDC0);
            return;
        }
		
        kal_trace(BT_TRACE_G2_PROFILES,HID_STATUSx02X ,Status);
        if (Status != BT_STATUS_SUCCESS) 
        {
            /* Indicate the query response failure to the application */
            HidClearConnection(channel, Status, 0);
            return;
        }
        else
        {
#ifdef __BT_4_0_BLE__
            kal_trace(BT_TRACE_BLE_PROFILES, HID_CMGRCALLBACK_DEVTYPE, channel->cmgrHandler.bdc->devType);
            if (channel->cmgrHandler.bdc->devType == BT_DEV_TYPE_LE)
            {
                ret = GattClientConnect(channel->cmgrHandler.remDev);
                Report(("[HID] GattClientConnect ret: %d", ret));
                /* Since HID Gatt callback will be called even if connection is existed, there is unnecessary
                to handle return success case here. */
/*
                if (ret == BT_STATUS_SUCCESS)
                {
                    ret = HidStartGattServiceQuery(channel);
                    if (ret != BT_STATUS_PENDING)
                    {
                        channel->flags &= ~CHNL_FLAG_SERVICE_QUERY;
                        HidAppCallback(channel, HIDEVENT_QUERY_CNF, ret,
                                       &channel->queryRsp, (U16)(sizeof(HidQueryRsp)));
                    }
                }
*/                
            }
            else
#endif
            {
                if (channel->flags & CHNL_FLAG_SERVICE_QUERY)
                {
                    HidStartServiceQuery(channel);
                }
                else
                {
                    kal_trace(BT_TRACE_G2_PROFILES,HID_START_L2CAP_CONNECTION);
                    /* Start the L2CAP connection */
                    if (L2CAP_ConnectReq(&HID(hidCtrlPsm), BT_PSM_HID_CTRL, 
                                          channel->cmgrHandler.remDev, 0, 
                                          &(channel->ctrlCid)) == BT_STATUS_PENDING) {
                        channel->state = HID_STATE_CONN_PENDING;
                        channel->flags |= CHNL_FLAG_INITIATOR;
                        break;
                    }
                }
            }
        }
        break;
	case CMEVENT_DATA_LINK_DIS:
	{
		HidClearConnection(channel, Status, 0);
		return;
	}
    default:
        break;
    }
}

/*---------------------------------------------------------------------------
 *            HidL2Callback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  L2CAP callback function for the Channel.
 *
 * Return:    void
 */
void HidL2Callback(L2capChannelId Cid, L2capCallbackParms *Parms)
{
    HidChannel *channel;

    channel = HidFindChannelByCid(Cid, Parms->aclLink);
    kal_trace(BT_TRACE_G2_PROFILES,HIDPARMSEVENTx02X ,Parms->event);
    kal_trace(BT_TRACE_G2_PROFILES,HID_CID_x02X ,Cid);
    kal_trace(BT_TRACE_G2_PROFILES,HID_CHANNELx08X , channel);
    switch (Parms->event) 
    {
    case L2EVENT_CONNECT_AUTORIZED:
        if (channel == NULL) 
        {
            U8 searchType;
            if (Parms->owner.psm == &HID(hidCtrlPsm))
                searchType = HID_FIND_CTRL_CID;
            else
                searchType = HID_FIND_INTR_CID;

            channel = HidFindUnusedChannel(searchType, Parms->aclLink);
            if (channel == NULL)
            {
                if (!HidCheckValidConnect(searchType, Cid, Parms->aclLink))
                {
                    Parms->aclLink->discFlag = TRUE;
                    L2CAP_ConnectRsp(Cid, L2CONN_REJECT_NO_RESOURCES, 0);
                    break;
                }

                //  Get a new channel 
                channel = HidNewChannel();
                if (channel == NULL) 
                {
                    kal_trace(BT_TRACE_G2_PROFILES,CHANNEL_IS_0_RETURN);
                    /* No channel available, reject the request */
                    Parms->aclLink->discFlag = TRUE;
                    L2CAP_ConnectRsp(Cid, L2CONN_REJECT_NO_RESOURCES, 0);
                    break;
                }
                channel->cmgrHandler.remDev = Parms->aclLink;
            }
        }
        if (channel->conCount == 0)
        {
            /* Only create ACL link for the first L2CAP connection */
            if (CMGR_CreateDataLink(&channel->cmgrHandler, 
                                    &Parms->aclLink->bdAddr) != BT_STATUS_SUCCESS) 
            {
                /* Could not create a link to the ACL */
                (void)L2CAP_ConnectRsp(Cid, L2CONN_REJECT_NO_RESOURCES, 0);
                break;
            }
        }

        if (Parms->owner.psm == &HID(hidCtrlPsm)) 
            channel->ctrlCid = Cid;
        else 
            channel->intrCid = Cid;
        /* Call the state machine */
        if (channel != 0) 
        {
            kal_trace(BT_TRACE_G2_PROFILES,HID_A_SMAx02XBx02X ,channel->state,Cid);
            HID(hidState)[channel->state](channel, Cid, Parms);
        }
        else
            kal_trace(BT_TRACE_G2_PROFILES,HIDCHANNEL_0);
        break;
    case L2EVENT_CONNECT_IND:
#ifdef HID_AUTHORIZATION
        //Assert(channel !=0);
	if(channel ==0){
		bt_prompt_trace(MOD_BT,"[HID]channel ==0");
		return;
		}
        L2CAP_ConnectRsp(Cid, L2CONN_ACCEPTED, 0);
#else
        if (channel == 0) 
        {
            /* Get a new channel */
            if (Parms->owner.psm == &HID(hidCtrlPsm)) 
                channel = HidFindUnusedChannel(HID_FIND_CTRL_CID, Parms->aclLink);
            else 
                channel = HidFindUnusedChannel(HID_FIND_INTR_CID, Parms->aclLink);
        }

        if (channel == 0) 
        {
            /* No channel available, reject the request */
            L2CAP_ConnectRsp(Cid, L2CONN_REJECT_NO_RESOURCES, 0);
            break;
        }
        //Assert(channel !=0);
	if(channel ==0){
		bt_prompt_trace(MOD_BT,"[HID]channel ==0");
		return;
		}
        if (CMGR_CreateDataLink(&channel->cmgrHandler, 
                                &Parms->aclLink->bdAddr) != BT_STATUS_SUCCESS) 
       {
            /* Could not create a link to the ACL */
            (void)L2CAP_ConnectRsp(Cid, L2CONN_REJECT_NO_RESOURCES, 0);
            break;
        }        /* Drop through */    
        //Assert(channel !=0);
	if(channel ==0){
		bt_prompt_trace(MOD_BT,"[HID]channel ==0");
		return;
		}
        if (Parms->owner.psm == &HID(hidCtrlPsm)) 
            channel->ctrlCid = Cid;
        else 
            channel->intrCid = Cid;

        /* Call the state machine */
        if (channel != 0) 
        {
            kal_trace(BT_TRACE_G2_PROFILES,HID_B_SMAx02XBx02X ,channel->state,Cid);
            HID(hidState)[channel->state](channel, Cid, Parms);
        }
        else
            kal_trace(BT_TRACE_G2_PROFILES,HIDCHANNEL_0);

#endif
        break;
    case L2EVENT_CONNECTED:
        /* Save the channel ID for the channel being requested */
        if (channel == 0)
        {
            L2CAP_DisconnectReq(Cid);
            break;
        }
        
        if (Parms->owner.psm == &HID(hidCtrlPsm)) 
            channel->ctrlCid = Cid;
        else 
            channel->intrCid = Cid;

        /* Call the state machine */
        if (channel != 0) 
        {
            kal_trace(BT_TRACE_G2_PROFILES,HID_C_SMAx02XBx02X ,channel->state,Cid);
            HID(hidState)[channel->state](channel, Cid, Parms);
        }
        else
            kal_trace(BT_TRACE_G2_PROFILES,HIDCHANNEL_0);
        break;
    case L2EVENT_DISCONNECTED:
        CMGR_HidNoRoleSwitch(0);           
        if (channel != 0) 
        {
            kal_trace(BT_TRACE_G2_PROFILES,HID_D_SMAx02XBx02X ,channel->state,Cid);
            HID(hidState)[channel->state](channel, Cid, Parms);
        }
        else
        {
            kal_trace(BT_TRACE_G2_PROFILES,UNUSED_CHANNEL_0_IGNORE_IT);
        }
        break;
    default:
        /* Call the state machine */
        if (channel != 0) 
        {
            kal_trace(BT_TRACE_G2_PROFILES,HID_E_SMAx02XBx02X ,channel->state,Cid);
            HID(hidState)[channel->state](channel, Cid, Parms);
        }
        else
            kal_trace(BT_TRACE_G2_PROFILES,HIDCHANNEL_0);
        break;
    }
}

/*---------------------------------------------------------------------------
 *            HidStateClosed()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Closed state for the HID channel.
 *
 * Return:    void
 */
void HidStateClosed(HidChannel *Channel, L2capChannelId Cid, 
                        L2capCallbackParms *Parms)
{
    kal_trace(BT_TRACE_G2_PROFILES,HIDSTATECLOSEDx02X ,Parms->event);

    if ((Parms->event == L2EVENT_CONNECT_IND)  ||
        (Parms->event == L2EVENT_CONNECT_AUTORIZED))
    {
        //Channel->cmgrHandler.callback = HidCmgrCallback;
        if (CMGR_CreateDataLink(&Channel->cmgrHandler, 
                                &Parms->aclLink->bdAddr) != BT_STATUS_SUCCESS) 
        {
            /* Could not create a link to the ACL */
            (void)L2CAP_ConnectRsp(Cid, L2CONN_REJECT_NO_RESOURCES, 0);
            return;
        }

        Channel->state = HID_STATE_CONN_PENDING;
        HidAppCallback(Channel, HIDEVENT_OPEN_IND, 
                           BT_STATUS_SUCCESS, Channel->cmgrHandler.remDev,
                           sizeof(BtRemoteDevice));
    }
    else {
        /* Ignore any other events */
        kal_trace(BT_TRACE_G2_PROFILES,HID_CHANNEL_CLOSED_STATE_RECEIVED_AN_UNEXPECTED_EVENT_x02X , Parms->event);
    }
}

/*---------------------------------------------------------------------------
 *            HidStateOpen()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open state for the HID channel.
 *
 * Return:    void
 */
void HidStateOpen(HidChannel *Channel, L2capChannelId Cid, 
                  L2capCallbackParms *Parms)
{
    HidTransaction *trans = NULL;
    HidInterrupt   *intr = NULL;
    U16 mtu;
    
    kal_trace(BT_TRACE_G2_PROFILES,HIDSTATEOPENx02X ,Parms->event);    
    switch (Parms->event) 
    {
    case L2EVENT_DISCON_IND:
    case L2EVENT_DISCONNECTED:
        Channel->state = HID_STATE_DISC_PENDING;
        if (!(Channel->flags & CHNL_FLAG_TERMINATOR)) 
        {
            /* Call the application to indicate the connection is going down.
                 L2capDiscReason provided in status*/
            HidAppCallback(Channel, HIDEVENT_CLOSE_IND, Parms->status,
                           Channel->cmgrHandler.remDev, 
                           sizeof(BtRemoteDevice));
        }
        /* Connection is going down */
        HidStateDiscPending(Channel, Cid, Parms);
        break;
        
    case L2EVENT_PACKET_HANDLED:
        kal_trace(BT_TRACE_G2_PROFILES,CIDxDCTRLCIDxD ,Cid,Channel->ctrlCid);
        if (Cid == Channel->ctrlCid) 
        {
            trans = (HidTransaction *)Parms->ptrs.packet->ulpContext;
            kal_trace(BT_TRACE_G2_PROFILES,HIDSTATEOPEN1_PKT_RETURNED_WITH_TRANS_TYPExD ,trans->type);
            if (trans->flags & TRANS_FLAG_ERROR) 
            {
                /* Ignore this event, transaction failed */
                kal_trace(BT_TRACE_G2_PROFILES,HIDSTATEOPENPKT_RETURNED_WITH_ERR_FLAG);
                InsertTailList(&HID(packetPool), &Parms->ptrs.packet->node);
                break;
            }
            mtu = L2CAP_GetTxMtu(Channel->ctrlCid);
        }
        else
        {
            /* Interrupt channel */
            intr = (HidInterrupt *)Parms->ptrs.packet->ulpContext;
            if (intr->flags & INTR_FLAG_ERROR) 
            {
                /* Ignore this event, interrupt failed */
                InsertTailList(&HID(packetPool), &Parms->ptrs.packet->node);
                break;
            }
            mtu = L2CAP_GetTxMtu(Channel->intrCid);
        }

        if (Parms->ptrs.packet->dataLen < mtu - 1)
        {
            /* A packet was handled, put it back in the pool */
            /* Only return packet when no more data need to be sent */            
            InsertTailList(&HID(packetPool), &Parms->ptrs.packet->node);
        }

        /* See if there is more data to send */
        if (Cid == Channel->ctrlCid) 
        {
            /* Check for more data */
            if (Parms->ptrs.packet->dataLen >= mtu -1) 
          {
                /* More data to send, send another packet */
                kal_trace(BT_TRACE_G2_PROFILES,HIDSTATEOPEN_SEND_MORE_DATA);
                //trans->flags &= ~TRANS_FLAG_START;
	       if(!(trans->flags & TRANS_FLAG_SENT))
	                if (HidSendTransPacket(Channel, trans, Parms->ptrs.packet) 
	                                                        != BT_STATUS_PENDING) 
	                {
	                    /* Complete the transaction with an error */
	                    trans->flags |= TRANS_FLAG_ERROR;

	                    InsertTailList(&HID(packetPool), &Parms->ptrs.packet->node);

	                    /* Complete the transaction with an error, check for a new
	                     * transaction.
	                     */
	                    HidCompleteTransaction(Channel, trans, BT_STATUS_FAILED);
	                }
            } 
            else 
            {
                /* No more data to send in this transaction, complete this
                 * transaction (if no response is expected) and see if there 
                 * is another transaction to start.
                 */
                trans->flags |= TRANS_FLAG_SENT;
                kal_trace(BT_TRACE_G2_PROFILES,HIDSTATEOPEN2_PKT_RETURNED_WITH_TRANS_TYPExD ,trans->type);
                switch (trans->type) {
                case HID_TRANS_CONTROL:
                case HID_TRANS_SET_REPORT_RSP:
                case HID_TRANS_SET_PROTOCOL_RSP:
                case HID_TRANS_SET_IDLE_RATE_RSP:
                case HID_TRANS_GET_REPORT_RSP:
                case HID_TRANS_GET_PROTOCOL_RSP:
                case HID_TRANS_GET_IDLE_RATE_RSP:
                //Add by stanley: 2007-0607
                case HID_HANDSHAKE:
                    kal_trace(BT_TRACE_G2_PROFILES,HIDCOMPLETETRANSACTION_WITH_TYPExD ,trans->type);
                    HidCompleteTransaction(Channel, trans, BT_STATUS_SUCCESS);
                    break;
                }
            }
        } 
        else 
        {

            /* Check for more data */
            if (Parms->ptrs.packet->dataLen >= mtu -1) 
          {
                /* More data to send, send another packet */
                //intr->flags &= ~INTR_FLAG_START;
	       if(!(intr->flags & INTR_FLAG_SENT))
	                if (HidSendIntrPacket(Channel, intr, Parms->ptrs.packet) 
	                                                        != BT_STATUS_PENDING) 
	                {
	                    /* Complete the interrupt with an error */
	                    intr->flags |= INTR_FLAG_ERROR;
	                    InsertTailList(&HID(packetPool), &Parms->ptrs.packet->node);
	                    /* Complete the interrupt with an error, check for a new
	                     * interrupt.
	                     */
	                    HidCompleteInterrupt(Channel, intr, BT_STATUS_FAILED);
	                }
            } 
            else 
            {
                /* No more data to send in this interrupt, complete this
                 * interrupt.
                 */
                 
                	intr->flags |= INTR_FLAG_SENT;
	    
                	HidCompleteInterrupt(Channel, intr, BT_STATUS_SUCCESS);
                
            }
        }
        break;
    case L2EVENT_DATA_IND:
        if (Cid == Channel->ctrlCid) 
        {
            //Assert(Channel !=0);
		if(Channel ==0){
			bt_prompt_trace(MOD_BT,"[HID]channel ==0");
			return;
			}
            /* A control channel request was received */
            HidControlChannel(Channel, Cid, Parms);
        } 
        else 
        {
            //Assert(Channel !=0);
		if(Channel ==0){
			bt_prompt_trace(MOD_BT,"[HID]channel ==0");
			return;
			}
            /* Application data was received */
            HidInterruptChannel(Channel, Cid, Parms);
        }
        break;
    default:
        /* Ignore any other events */
        kal_trace(BT_TRACE_G2_PROFILES,HID_SERVER_OPEN_STATE_RECEIVED_AN_UNEXPECTED_EVENT_x02X , Parms->event);
        break;
    }
}

/*---------------------------------------------------------------------------
 *            HidStateConnPending()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Connection Pending state for the HID channel.
 *
 * Return:    void
 */
void HidStateConnPending(HidChannel *Channel, L2capChannelId Cid, 
                             L2capCallbackParms *Parms)
{
    //BtSniffInfo sniffInfo;
    BtStatus status;
    
    kal_trace(BT_TRACE_G2_PROFILES,HIDSTATECONNPENDINGx02X ,Parms->event);    
    switch (Parms->event) 
    {
    case L2EVENT_CONNECT_AUTORIZED:
        L2Cap_AuthorizeConnectRsp(Cid, TRUE);
        break;
    case L2EVENT_CONNECT_IND:
        /* A second connection was requested */
        status = L2CAP_ConnectRsp(Cid, L2CONN_ACCEPTED, 0);
        kal_trace(BT_TRACE_G2_PROFILES,L2CAP_CONNECTRSPx02X ,status);
        break;
    case L2EVENT_CONNECTED:
        /* A connection is up */
        //Assert(Channel->conCount < 2);
	if(Channel->conCount >= 2){
		bt_prompt_trace(MOD_BT,"[HID]Channel->conCount >= 2");
		return;
		}
        Channel->conCount++;
        CMGR_HidNoRoleSwitch(1);
        if (Channel->conCount == 2) 
        {
            /* Notify the application */
            Channel->state = HID_STATE_OPEN;
            HidAppCallback(Channel, HIDEVENT_OPEN, BT_STATUS_SUCCESS,
                           Channel->cmgrHandler.remDev, 
                           sizeof(BtRemoteDevice));

            /* Put it on the list of registered handlers */
            CMGR_SetDeviceAsMaster(&Channel->cmgrHandler);
//            CMGR_ActivateSniffTimer(&Channel->cmgrHandler);
        } 
        else if (Channel->flags & CHNL_FLAG_INITIATOR) 
        {
            /* The channel initiated the connection, establish the 
             * interrupt channel 
             */
            if (Channel->intrCid == Cid)
            {
                /* This means control channel has broken before
                    interrupt channel established */
                L2CAP_DisconnectReq(Cid);
                break;
            }
            if (L2CAP_ConnectReq(&HID(hidIntrPsm), BT_PSM_HID_INTR, 
                                 Channel->cmgrHandler.remDev, 0,
                                 &(Channel->intrCid)) != BT_STATUS_PENDING) 
             {
                /* Connect request failed, tear down the first connection */
                status = L2CAP_DisconnectReq(Channel->ctrlCid);
                kal_trace(BT_TRACE_G2_PROFILES,HID_L2CAP_DISCONNECT_REQ_CID_CIDx02X_STATUSx02X ,Channel->ctrlCid, status);
            }
        }
        break;
    case L2EVENT_DISCONNECTED:
        /* Connection is going down */
        CMGR_HidNoRoleSwitch(0);        
        HidStateDiscPending(Channel, Cid, Parms);
        break;
    default:
        /* Ignore any other events */
        kal_trace(BT_TRACE_G2_PROFILES,HID_CHANNEL_CONN_PENDING_STATE_RECEIVED_AN_UNEXPECTED_EVENT_x02X , Parms->event);
    }
}

/*---------------------------------------------------------------------------
 *            HidStateDiscPending()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Disconnect Pending state for the HID channel.
 *
 * Return:    void
 */
void HidStateDiscPending(HidChannel *Channel, L2capChannelId Cid, 
                             L2capCallbackParms *Parms)
{
    BtStatus status;
    
    kal_trace(BT_TRACE_G2_PROFILES,HIDSTATEDISCPENDINGx02X_CIDx02X ,Parms->event, Cid, Parms->status);    
    CMGR_HidNoRoleSwitch(0);        
    
    switch (Parms->event) 
    {
    case L2EVENT_DISCONNECTED:
        /* The connection is down, adjust the connection reference count */
        kal_trace(BT_TRACE_G2_PROFILES,HID_CHANNELCONCOUNTx02X ,Channel->conCount);
        if (Channel->conCount > 0) 
        {
            Channel->conCount--;
        }
        kal_trace(BT_TRACE_G2_PROFILES,CHANNELCTRLCIDx02XCHANNELINTRCIDx02X , Channel->ctrlCid, Channel->intrCid);
    
        if (Cid == Channel->ctrlCid) 
        {
            Channel->ctrlCid = 0;
        } 
        else 
        {
            Channel->intrCid = 0;
        }

        if (Channel->conCount > 0)
        {
            if (Channel->flags & CHNL_FLAG_TERMINATOR) 
            {
                kal_trace(BT_TRACE_G2_PROFILES,HID_TRY_TO_DISCONNECT_INTR_CHANNEL);
                /* First connection disconnect is complete, disconnect the 2nd */
                /* Disconnect control channel secondly */
                status = L2CAP_DisconnectReq(Channel->ctrlCid);
                kal_trace(BT_TRACE_G2_PROFILES,HID_L2CAP_DISCONNECT_REQ_CID_CIDx02X_STATUSx02X ,Channel->intrCid, status);
            }
        } 
        else if (Channel->state == HID_STATE_CONN_PENDING && 
                (Channel->ctrlCid != 0 || Channel->intrCid != 0))
        {
            /* This means the connection is still ongoing */
        }
        else 
        {
            kal_trace(BT_TRACE_G2_PROFILES,HIDCLEARCONNECTION);
            if (Channel->flags & CHNL_FLAG_SERVICE_QUERY)
            {
                /* if SDP query is not completed yet, not remove the connection until SDP query is done */
                Channel->bSDPDiscPending = TRUE;
            }
            else
            {
                /* Clear the connection structure and notify the application */
                HidClearConnection(Channel, BT_STATUS_SUCCESS, Parms->status);
            }
        }
    }

}

#endif
