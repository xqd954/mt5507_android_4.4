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
 *     $Workfile:rfcomm.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains code for the rfcomm protocol.
 *
 * Created:
 *     July 20, 1999
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
#include "stdlib.h"
#include "btbm_int.h"
#if RFCOMM_PROTOCOL == XA_ENABLED
void OpenClientChannelPrintIndex(U8 index);
#if NUM_RF_SERVERS != 0


/*****************************************************************************
 * FUNCTION
 *  RF_ChannelInit
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel         [IN]        
 *  sec_level       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RF_ChannelInit(RfChannel *Channel, U8 sec_level)
{
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    if ((Channel->secRecord.node.Flink == 0) && (Channel->secRecord.node.Blink == 0))
    {
        InitializeListEntry(&(Channel->secRecord.node));
    }
    else if (!IsListCircular(&(Channel->secRecord.node)))
    {
        InitializeListEntry(&(Channel->secRecord.node));
    }
#endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 
    Channel->secLevel = sec_level;
}

#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)


/*****************************************************************************
 * FUNCTION
 *  RF_RegisterSecurityRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel         [IN]        
 *  incoming        [IN]        
 *  service_id      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RF_RegisterSecurityRecord(RfChannel *Channel, BOOL incoming, U8 service_id)
{
    BtStatus secStatus;

    bt_trace(TRACE_GROUP_1, CLIENT_REGISTRATION_SECURITY_RECORDBT_DEBUGx02X, Channel->secLevel);
    Channel->secRecord.level_type = Channel->secLevel;
    Channel->secRecord.level = (BSL_AUTHENTICATION_OUT | BSL_ENCRYPTION_OUT) |
        (BSL_AUTHENTICATION_IN | BSL_ENCRYPTION_IN);
    Channel->secRecord.id = SEC_RFCOMM_ID;
    if (incoming == FALSE)
    {
        Channel->secRecord.channel = (U32) Channel;
        Channel->incoming = 0;
    }
    else
    {
        Channel->secRecord.channel = service_id;
        Channel->incoming = 1;
    }
    secStatus = SEC_RegisterRecord(&(Channel->secRecord));
    bt_trace(TRACE_GROUP_1, REGISTRATION_SECURITY_RECORD_STATUSBT_DEBUGx02X, secStatus);
    return;
}


/*****************************************************************************
 * FUNCTION
 *  RF_DeRegisterSecurityRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RF_DeRegisterSecurityRecord(RfChannel *Channel)
{
    BtStatus secStatus;

    bt_trace(TRACE_GROUP_1, CLIENT_DEREGISTRATION_SECURITY_RECORDBT_DEBUGx02X, Channel->secLevel);
    secStatus = SEC_Unregister(&(Channel->secRecord));
    bt_trace(TRACE_GROUP_1, DEREGISTRATION_SECURITY_RECORD_STATUSBT_DEBUGx02X, secStatus);
    return;
}

#endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 


/*****************************************************************************
 * FUNCTION
 *  RF_RegisterServerChannel
 * DESCRIPTION
 *  Register a service on the specified RFCOMM channel.
 * PARAMETERS
 *  Channel     [IN]        
 *  Service     [IN]        
 *  Credit      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_RegisterServerChannel(RfChannel *Channel, RfService *Service, U8 Credit)
{
    BtStatus status;

    if (Channel == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if (Channel->callback == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if (Service == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if ((Channel->maxFrameSize > RF_MAX_FRAME_SIZE) || (Channel->maxFrameSize < RF_MIN_FRAME_SIZE))
    {
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();

    Channel->rxCredit = Credit;
    Channel->BusyCredit = 0;
    Channel->creditTimer.func = 0;
    Channel->initialRxCredit = Credit;
    Channel->initialFrameSize = Channel->maxFrameSize;
    Channel->remDev = 0;

    Channel->rfChFlags = 0;
    /* Get the new Server Channel */
    status = RfAllocService(Channel, Service);
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    if (status == BT_STATUS_SUCCESS)
    {
        RF_RegisterSecurityRecord(Channel, TRUE, Service->serviceId);
    }
#endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 
    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RF_DeregisterServerChannel
 * DESCRIPTION
 *  Deregister a channel from a service.
 * PARAMETERS
 *  Channel     [IN]        
 *  Service     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_DeregisterServerChannel(RfChannel *Channel, RfService *Service)
{
    BtStatus status;

    if (Channel == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if (Channel->callback == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if (Service == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if ((Service->serviceId > NUM_RF_SERVERS) || (Service->serviceId == 0))
    {
        return BT_STATUS_INVALID_PARM;
    }


    OS_LockStack();

    /* Add for [ALPS00407742]The NE about "mtkbt" pops up when you turn off BT begin
     * When channel state is DLC_STATE_SEC_PENDING, the upper layer doesn't know the
     * status, it may call RF_DeregisterServerChannel when user powers off BT, so
     * we need terminate the current operation.
     */    
    if ((Channel->state == DLC_STATE_SEC_PENDING) && (Channel->secToken.incoming == TRUE))
    {  
        Report(("RF_DeregisterServerChannel: Channel->state=DLC_STATE_SEC_PENDING"));    
        RfSendAck(DM_F, Channel->muxId, Channel->dlci);
        RfFreeDLC(Channel->muxId, Channel->dlcId);
    }    
    /* Add for [ALPS00407742]The NE about "mtkbt" pops up when you turn off BT end */

    /* Free the server channel */
    EVM_ResetTimer(&Channel->creditTimer);
    status = RfFreeServerChannel(Channel, Service);

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RF_DeregisterService
 * DESCRIPTION
 *  Deregister a service from RFCOMM.
 * PARAMETERS
 *  Service     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_DeregisterService(RfService *Service)
{
    BtStatus status;

    if (Service == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();

    /* Free the service */
    status = RfFreeService(Service);

    OS_UnlockStack();

    return status;
}
#endif /* NUM_RF_SERVERS != 0 */


/*****************************************************************************
 * FUNCTION
 *  RfOpenClientChannel
 * DESCRIPTION
 *  Open a channel to a service on the remote device after security
 *  access has been validated.
 * PARAMETERS
 *  RemDev          [IN]        
 *  ServerId        [IN]        
 *  Channel         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus RfOpenClientChannel(BtRemoteDevice *RemDev, U8 ServerId, RfChannel *Channel)
{
    L2capChannelId l2ChannelId;
    BtStatus status;
    U8 muxId;
    U8 dlcId;
    U8 dlci;
    U16 l2capTxMtu = 0;    

    Channel->remDev = RemDev;
    if (RfFindMuxIdByRemDev(RemDev, &muxId) == BT_STATUS_SUCCESS)
    {

        /* The multiplexer is established, create the DLCI */
        if (MUX(muxId).flags & MUX_FLAG_INITIATOR)
        {
            dlci = (U8) (ServerId << 1);
        }
        else
        {
            dlci = (U8) ((ServerId << 1) | 0x01);
        }

        /* An L2CAP channel already exists to the device */
        if (MUX(muxId).state == MUX_STATE_OPEN)
        {
            if (MUX(muxId).flags & MUX_FLAG_CMD_SENT)
            {
                status = BT_STATUS_BUSY;
                return status;
            }

            /* Allocate a new DLC */
            if (RfAllocDLC(muxId, dlci, Channel, &dlcId) == BT_STATUS_SUCCESS)
            {
                MUX(muxId).cDlcId = dlcId;  

                if (MUX(muxId).flags & MUX_FLAG_CREDIT_FLOW)
                {
                    /*
                     * This is not the first DLC established and credit-based flow
                     * * control has been negotiated.  Keep sending the negotiation.
                     * * If credit-based flow had not been negotiated, we cannot try
                     * * again.
                     */
                    DLC(muxId, dlcId).flags |= DLC_FLAG_CREDIT_FLOW;
                }
                Channel->creditTimer.func = 0;
                /* Send the channel negotiation */
                l2capTxMtu = L2CAP_GetTxMtu(MUX(muxId).l2ChannelId);
                if (l2capTxMtu > 5)
                {
                    DLC(muxId, dlcId).channel->maxFrameSize = (U16)
                        min(l2capTxMtu - 5, DLC(muxId, dlcId).channel->maxFrameSize);
                }
                status = RfSendParmNegotiation(Channel, CR_BIT);
                if (status == BT_STATUS_PENDING)
                {

                    /* Negotiation request sent */
                    DLC(muxId, dlcId).channel->state = DLC_STATE_NEG_PENDING;
                    bt_trace(
                        TRACE_GROUP_1,
                        RFCOMM_NEGOTIATION_SENT_SUCCESSFULLY_TO_DLCI_xD_ON_MUX_xD,
                        Channel->dlci,
                        muxId);
                }
                else
                {

                    /* Could not send the negotiation */
                    RfFreeDLC(muxId, dlcId);

                    /* If this is the last DLC, shut down the mux */
                    if (!MUX(muxId).dlcCount)
                    {
                        RfDisconnectL2CAP(muxId);
                    }
                }
            }
            else
            {

                /* No channels left, or channel is in use */
                status = BT_STATUS_NO_RESOURCES;
            }
        }
        else
        {
            /* No mux is allocated */
            status = BT_STATUS_FAILED;
        }

    }
    else
    {

        /* No connection exists yet, this side is the initator */
        dlci = (U8) (ServerId << 1);

        /* Establish a new connection to the device */
        status = L2CAP_ConnectReq(&(RFC(psm)), BT_PSM_RFCOMM, RemDev, 0, &l2ChannelId);
        if (status == BT_STATUS_PENDING)
        {

            /* Connection started, allocate a new mux */
            if (RfAllocMux(RemDev, l2ChannelId, &muxId) == BT_STATUS_SUCCESS)
            {

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
                {
                    L2capConnSettings Setting;

                    Setting.reasmBuffer = (ListEntry *)&MUX(muxId).rxPacketsBufferList;
                    Setting.receivedBuffer = (ListEntry *)&MUX(muxId).receivedPacketsList;
                    Setting.rxWindowSize = RF_NUM_RETRANSMISSION_WINDOW_SIZE;  
                    OS_MemSet(MUX(muxId).txMaxCountPerIFrame, 0, 64);
                    Setting.txMaxTransmitCount = (U8 *)MUX(muxId).txMaxCountPerIFrame;
                    L2CAP_ConnectReqAssignFlowControlBuffer(&Setting, l2ChannelId);
                }
#endif
                /* Initialize mux */
                MUX(muxId).state = MUX_STATE_CONN_PENDING;
                MUX(muxId).flags |= MUX_FLAG_INITIATOR;

                /* Allocate a new DLC */
                if (RfAllocDLC(muxId, dlci, Channel, &dlcId) == BT_STATUS_SUCCESS)
                {
                    /* Initialize DLC and channel ID */
                    MUX(muxId).cDlcId = dlcId;

                    /* This is the first DLC, try to negotiate credit flow control */
                    DLC(muxId, dlcId).flags |= DLC_FLAG_CREDIT_FLOW;

                }
                else
                {
                    /* No more available channels */
                    RfFreeMux(muxId);
                    status = BT_STATUS_NO_RESOURCES;
                }
            }
            else
            {
                /* No more available MUX's */
                status = BT_STATUS_NO_RESOURCES;
            }
        }
    }

    return status;
}

#if (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED)


/*****************************************************************************
 * FUNCTION
 *  RfSecCallback
 * DESCRIPTION
 *  Callback for security check.
 * PARAMETERS
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfSecCallback(const BtEvent *Event)
{
    RfChannel *channel = ContainingRecord(Event->p.secToken, RfChannel, secToken);
    BtStatus status;

    if (channel->state == DLC_STATE_SEC_PENDING)
    {
        if (Event->p.secToken->incoming == FALSE)
        {
            channel->secToken.remDev->refCount--;   // remove the temp refCount
            Report(("temp refCount--"));
            /* This is an outgoing connection */
            if (Event->eType == BTEVENT_ACCESS_APPROVED)
            {
                /* Security approved. Use dlci as ServerID for now. */
                status = RfOpenClientChannel(channel->secToken.remDev, channel->dlci, channel);
                if (status != BT_STATUS_PENDING)
                {
                    RfAppCallback(channel, status, RFEVENT_CLOSED, 0);
                #if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
                    if (channel->incoming == 0)
                    {
                        RF_DeRegisterSecurityRecord(channel);
                    }
                #endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 
                }
            }
            else
            {
                /* Security failed */
                channel->remDev = channel->secToken.remDev;
                RfAppCallback(channel, BT_STATUS_RESTRICTED, RFEVENT_CLOSED, 0);
                channel->remDev = 0;
            #if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
                if (channel->incoming == 0)
                {
                    RF_DeRegisterSecurityRecord(channel);
                }
            #endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 
            }
        }
        else
        {
            /* This is an incoming connection */
            if (Event->eType == BTEVENT_ACCESS_APPROVED)
            {
                /* Security approved */
                channel->state = DLC_STATE_CONN_INCOMING;
                RfAppCallback(channel, BT_STATUS_SUCCESS, RFEVENT_OPEN_IND, 0);
            }
            else
            {
                /* Security failed */
                (void)RfSendAck(DM_F, channel->muxId, channel->dlci);
                channel->state = DLC_STATE_SEC_FAILED;
                RfFreeDLC(channel->muxId, channel->dlcId);
            }
        }
    }
}
#endif /* (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED) */


/*****************************************************************************
 * FUNCTION
 *  RF_OpenClientChannel
 * DESCRIPTION
 *  Open a channel to a service on the remote device.
 * PARAMETERS
 *  RemDev          [IN]        
 *  ServerId        [IN]        
 *  Channel         [IN]        
 *  Credit          [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_OpenClientChannel(BtRemoteDevice *RemDev, U8 ServerId, RfChannel *Channel, U8 Credit)
{
    BtStatus status = BT_STATUS_FAILED;
    U8 muxId;
    I8 i;

#if (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED)
    U8 oldState;
#endif /* (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED) */
    OpenClientChannelPrintIndex(0);
    if (RemDev == 0)
    {
        OpenClientChannelPrintIndex(1);
        return BT_STATUS_INVALID_PARM;
    }

    if ((ServerId == 0) || (ServerId > 30))
    {
        OpenClientChannelPrintIndex(2);
        return BT_STATUS_INVALID_PARM;
    }

    if (Channel == 0)
    {
        OpenClientChannelPrintIndex(3);
        return BT_STATUS_INVALID_PARM;
    }

    if ((Channel->callback == 0) ||
        (Channel->priority > 63) ||
        (Channel->maxFrameSize > RF_MAX_FRAME_SIZE) || (Channel->maxFrameSize < RF_MIN_FRAME_SIZE))
    {
        OpenClientChannelPrintIndex(4);
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();

    /* See if this channel is allocated already */
    if (RfFindMuxIdByRemDev(RemDev, &muxId) == BT_STATUS_SUCCESS)
    {
        for (i = 0; i < NUM_RF_CHANNELS; i++)
        {
            if ((DLC(muxId, i).flags & DLC_FLAG_ALLOCATED) && (DLC(muxId, i).channel == Channel))
            {
                OS_UnlockStack();
                OpenClientChannelPrintIndex(5);
                return BT_STATUS_IN_USE;
            }
        }
    }

    Channel->rxCredit = Credit;
    Channel->BusyCredit = 0;
    Channel->creditTimer.func = 0;
    Channel->initialRxCredit = Credit;
    Channel->initialFrameSize = Channel->maxFrameSize;
    
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    RF_RegisterSecurityRecord(Channel, FALSE, 0);
#endif 

    oldState = Channel->state;
    Channel->state = DLC_STATE_SEC_PENDING;
    Channel->secToken.id = SEC_RFCOMM_ID;
    Channel->secToken.remDev = RemDev;
    Channel->secToken.channel = (U32) Channel;
    Channel->secToken.incoming = FALSE;
    status = SEC_AccessRequest(&(Channel->secToken));
    if (status == BT_STATUS_SUCCESS)
    {
        Channel->state = oldState;
        status = RfOpenClientChannel(RemDev, ServerId, Channel);
        if (status != BT_STATUS_PENDING)
        {
                OpenClientChannelPrintIndex(6);
                /* 
                  * MAUI_01105021:
                  * Because the RF_RegisterSecurityRecord is invoked previously, if open chnl is failed: it should de-register security record 
                  * Otherwise, some profiles (e.g., BIP) it will not de-register security and reset its context directly: This will casue unexpected error!
                  */
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
                RF_DeRegisterSecurityRecord(Channel);
#endif 
                
        }
    }
    else if (status != BT_STATUS_PENDING)
    {
        Channel->state = oldState;
        status = BT_STATUS_RESTRICTED;
        /* 
         * MAUI_01921658 
         * It's possible RfOpenClientChannel() failed.
         * In this case, we have to deregister security record.
         */
    		#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
        RF_DeRegisterSecurityRecord(Channel);   
    		#endif   
    }
    else
    {
        /* Temporarily use dlci to store the requested ServerID */
        Channel->dlci = ServerId;
        RemDev->refCount++; // Increase the refCount temporarily to avoid the L2CAP link is disconencted during authentication.
        Report(("temp refCount++"));        
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RF_CloseChannel
 * DESCRIPTION
 *  Close the channel between two devices.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_CloseChannel(RfChannel *Channel)
{
    BtStatus status = BT_STATUS_FAILED;

    if (!RfIsValidChannel(Channel))
    {
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();
    /* Add for [MAUI_03164384][BT]after cancle pair process,BT headset auto disconnected begin */
    if (Channel->state == DLC_STATE_SEC_PENDING)
    {  
        Report(("RF_CloseChannel: Channel->state=DLC_STATE_SEC_PENDING"));    
        if (Channel->secToken.incoming == TRUE)
        {
            RfSendAck(DM_F, Channel->muxId, Channel->dlci);
            RfFreeDLC(Channel->muxId, Channel->dlcId);
        }
        else
        {
            AssertEval(SEC_CancelAccessRequest(&(Channel->secToken)) == BT_STATUS_SUCCESS);               
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
            RF_DeRegisterSecurityRecord(Channel);
#endif     
        }    
        OS_UnlockStack();
        return BT_STATUS_SUCCESS;
    }
    /* Add for [MAUI_03164384][BT]after cancle pair process,BT headset auto disconnected end */    

    if ((DLC(Channel->muxId, Channel->dlcId).channel))
    {
        /*
         * RFCOMM_INVALID_DISC:Add the CHNL_FLAG_DISC_QUEUED condition
         * We don't allow to send one more DISC if the flag is set.
         */
        Report(("RF_CloseChannel: Channel->state=%d", Channel->state));
        if (Channel->state == DLC_STATE_DISC_PENDING || 
            Channel->state == DLC_STATE_DISC_INCOMING ||
            Channel->rfChFlags & CHNL_FLAG_DISC_QUEUED )
        {
            /*
             * [ALPS00403568]A2DP+OPP,red screen NE+WS popped up,and BT can not switch on anymore.
             * It seems that upper layer no need to distinguish BT_STATUS_IN_PROGRESS,
             * just BT_STATUS_PENDING can replace it
             */
            OS_Report("RFCOMM: DISC operation is ongoing");            
            status = /* BT_STATUS_IN_PROGRESS */BT_STATUS_PENDING;
        }
        else if ((Channel->state == DLC_STATE_CONN_PENDING) ||
            (RfIsChannelAllocated(Channel) && Channel->state == DLC_STATE_CONNECTED))
        {
            // show the credit data
            OS_Report("RFCOMM: check21 dlcid:%d txcredit:%d rxc:%d flag:0x%x", Channel->dlcId, Channel->txCredit, Channel->rxCredit, MUX(Channel->muxId).flags);
            
            /* We can't send out DISC while the response of previous command 
             * is not received. (refer to 3GPP TS 07.10 V7.2.0 section5.4.4.1)
             */
//            MUX(Channel->muxId).flags &= ~MUX_FLAG_CMD_SENT;
            EVM_ResetTimer(&Channel->creditTimer);
            Channel->BusyCredit = 0;
            RfDisconnectDLC(Channel->muxId, Channel->dlcId);
            status = BT_STATUS_PENDING;            
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RF_RespondChannel
 * DESCRIPTION
 *  Accept or reject a Channel connect request between two devices.
 * PARAMETERS
 *  Channel     [IN]        
 *  flag        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_RespondChannel(RfChannel *Channel, BOOL flag)
{
    BtStatus status = BT_STATUS_FAILED;

    if (!RfIsValidChannel(Channel))
    {
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();

    if (RfIsChannelAllocated(Channel) && Channel->state == DLC_STATE_CONN_INCOMING)
    {
        if (flag)
        {
            Channel->creditTimer.func = 0;
            Channel->BusyCredit = 0;
            status = RfSendAck(UA_F, Channel->muxId, Channel->dlci);
        }
        else
        {
            status = RfSendAck(DM_F, Channel->muxId, Channel->dlci);
        }
    }
    else
    {
        status = BT_STATUS_FAILED;
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RF_SendData
 * DESCRIPTION
 *  Send data on an RFCOMM channel.
 * PARAMETERS
 *  Channel     [IN]        
 *  Packet      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_SendData(RfChannel *Channel, BtPacket *Packet)
{
    BtStatus status = BT_STATUS_FAILED;

    if (!RfIsValidChannel(Channel))
    {
        return BT_STATUS_INVALID_PARM;
    }

    if (Packet == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if (MUX(Channel->muxId).flags & MUX_FLAG_CREDIT_FLOW)
    {
        /* Allow one less byte of data when credit flow is used */
        if (Packet->dataLen > DLC(Channel->muxId, Channel->dlcId).channel->maxFrameSize)
        {
            return BT_STATUS_INVALID_PARM;
        }
    }

    if ((Packet->dataLen > 0) && Packet->data == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }
    if((!HCI_IsValidAclPacket())  && ((random() % 4) ==0))
    {
        if(Packet->dataLen > 9)
        {
            U16 fix = 0;
            fix = random() % Packet->dataLen;
            Packet->data[fix] = (U8) (random());
        }
#ifdef __BT_DLIGHT1_DEBUG__
        Report(("autumn send rfcomm"));
#endif
    }
    OS_LockStack();

    XASTAT_TimerStart(rfcomm_tx_timer, Packet->rfc_timer);
    if (RfIsChannelAllocated(Channel) && Channel->state == DLC_STATE_CONNECTED)
    {
        Packet->ulpContext = Channel;

        /* Insert onto the transmit queue */
        InsertTailList(&(Channel->txQueue), &(Packet->node));
        MUX(Channel->muxId).numTxPackets++;
        /* bt_trace(TRACE_GROUP_1,IN_RF_SENDDATA_DATA_SENT_ON_DLCIxD_PKTINTRANSITxD ,Channel->dlci,MUX(Channel->muxId).packetsInTransit); */
        bt_trace(
            TRACE_GROUP_1,
            MUXCHANNELMUXIDNUMTXPACKETSxD,
            MUX(Channel->muxId).numTxPackets,
            Channel->dlci,
            MUX(Channel->muxId).packetsInTransit);
        status = BT_STATUS_PENDING;
        if (!(MUX(Channel->muxId).flags & MUX_FLAG_XMIT))
        {
            /* Not already transmitting, send any queued packets. */
            /* bt_trace(TRACE_GROUP_1,IN_RF_SENDDATA_CALL_RFMUXCHECKTXQUEUES); */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_RF_SENDDATA_CHECK_TXQUEUE,Channel->dlci,Packet->dataLen);
            RfMUXCheckTxQueues(Channel->muxId);
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RF_SetModemStatus
 * DESCRIPTION
 *  Set the modem status.
 * PARAMETERS
 *  Channel         [IN]        
 *  ModemStatus     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_SetModemStatus(RfChannel *Channel, RfModemStatus *ModemStatus)
{
    BtStatus status = BT_STATUS_FAILED;
    RF_DLC *dlc;

    if (!RfIsValidChannel(Channel))
    {
        return BT_STATUS_INVALID_PARM;
    }

    if (ModemStatus == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if (ModemStatus->breakLen > 15)
    {
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();

    dlc = &DLC(Channel->muxId, Channel->dlcId);

    if (RfIsChannelAllocated(Channel) && Channel->state == DLC_STATE_CONNECTED)
    {
        if (dlc->flags & DLC_FLAG_REQ_SENT)
        {
            /* A request has already been sent */
            status = BT_STATUS_IN_PROGRESS;
        }
        else
        {
            /* No outstanding request */
            Channel->rfSignals = (RfSignals) ((ModemStatus->signals & ~RF_FLOW) | (Channel->rfSignals & RF_FLOW));

            status = RfSendModemStatus(Channel, Channel->rfSignals, ModemStatus->breakLen, CR_BIT);
        }
    }

    OS_UnlockStack();

    return status;
}

#if RF_SEND_CONTROL == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  RF_SetLineStatus
 * DESCRIPTION
 *  Set the line status.
 * PARAMETERS
 *  Channel     [IN]        
 *  Status      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_SetLineStatus(RfChannel *Channel, RfLineStatus Status)
{
    BtStatus status = BT_STATUS_FAILED;
    RF_DLC *dlc;

    if (!RfIsValidChannel(Channel))
    {
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();

    dlc = &DLC(Channel->muxId, Channel->dlcId);

    if (RfIsChannelAllocated(Channel) && Channel->state == DLC_STATE_CONNECTED)
    {
        if (dlc->flags & DLC_FLAG_REQ_SENT)
        {
            /* A request has already been sent */
            status = BT_STATUS_IN_PROGRESS;
        }
        else
        {
            status = RfSendLineStatus(Channel, Status, CR_BIT);
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RF_SendPortSettings
 * DESCRIPTION
 *  Negotiate port settings with the remote device.
 * PARAMETERS
 *  Channel             [IN]        
 *  PortSettings        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_SendPortSettings(RfChannel *Channel, RfPortSettings *PortSettings)
{
    BtStatus status = BT_STATUS_FAILED;
    RF_DLC *dlc;

    if (!RfIsValidChannel(Channel))
    {
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();

    dlc = &DLC(Channel->muxId, Channel->dlcId);

    if (RfIsChannelAllocated(Channel) && Channel->state == DLC_STATE_CONNECTED)
    {
        if (dlc->flags & DLC_FLAG_REQ_SENT)
        {
            /* A request has already been sent */
            status = BT_STATUS_IN_PROGRESS;
        }
        else
        {
            if (PortSettings == 0)
            {
                dlc->flags |= DLC_FLAG_RPN_STATUS;
            }
            status = RfSendRemotePortNeg(Channel, PortSettings, CR_BIT);
        }
    }

    OS_UnlockStack();

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RF_RespondPortSettings
 * DESCRIPTION
 *  Accept or reject port setting received from the remote device.
 * PARAMETERS
 *  Channel             [IN]        
 *  PortSettings        [IN]        
 *  delayedResponse     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_RespondPortSettings(RfChannel *Channel, RfPortSettings *PortSettings, BOOL delayedResponse)
{
    BtStatus status = BT_STATUS_FAILED;
    RF_DLC *dlc;

    if (!RfIsValidChannel(Channel))
    {
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();

    dlc = &DLC(Channel->muxId, Channel->dlcId);

    if (RfIsChannelAllocated(Channel) && dlc->flags & DLC_FLAG_REQ_INCOMING)
    {

        /* Send the reponse */
        if (!delayedResponse)
        {
            if ((status = RfSendRemotePortNeg(Channel, PortSettings, 0)) == BT_STATUS_PENDING)
            {
                DLC(Channel->muxId, Channel->dlcId).flags &= ~DLC_FLAG_REQ_INCOMING;
                status = BT_STATUS_SUCCESS;
            }
            else if (status == BT_STATUS_NO_RESOURCES)
            {
                Channel->rfChFlags |= CHNL_FLAG_NO_RESOURCE;
            }
        }
        else
        {
            Channel->rfChFlags |= CHNL_FLAG_RSP_DELAYED;
            status = BT_STATUS_SUCCESS;
        }
    }

    OS_UnlockStack();

    return status;

}
#endif /* RF_SEND_CONTROL == XA_ENABLED */

#if RF_SEND_TEST == XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  RF_SendTest
 * DESCRIPTION
 *  Tests the connection with the remote device.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_SendTest(RfChannel *Channel)
{
    BtStatus status = BT_STATUS_FAILED;
    RF_DLC *dlc;

    if (!RfIsValidChannel(Channel))
    {
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();

    dlc = &DLC(Channel->muxId, Channel->dlcId);

    if (RfIsChannelAllocated(Channel) && Channel->state == DLC_STATE_CONNECTED)
    {
        if (!(MUX(Channel->muxId).flags & MUX_FLAG_TEST_SENT) && !(dlc->flags & DLC_FLAG_REQ_SENT))
        {
            MUX(Channel->muxId).testCmdData[2] = TEST_PATTERN;
            Channel->cmdSent = TEST | CR_BIT;
            status = RfSendTest(Channel->muxId, 1, CR_BIT);
            if (status == BT_STATUS_PENDING)
            {
                EVM_StartTimer(&(Channel->timer), RF_T2_TIMEOUT);
                bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_START_TIMER_SEND_CMD_ON_DLCI_OF_MUX_TIME, Channel->cmdSent, Channel->dlci, Channel->muxId, RF_T2_TIMEOUT/1000);
                DLC(Channel->muxId, Channel->dlcId).flags |= DLC_FLAG_REQ_SENT;
                MUX(Channel->muxId).testDlcId = Channel->dlcId;
            }
        }
        else
        {
            status = BT_STATUS_IN_PROGRESS;
        }
    }

    OS_UnlockStack();

    return status;
}
#endif /* RF_SEND_TEST == XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  RF_FrameSize
 * DESCRIPTION
 *  Returns the maximum size allowed for a transmit packet.
 *  Maxmum number of bytes for a transmit packet.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U16 RF_FrameSize(RfChannel *Channel)
{
    U16 status = 0;

    if (!RfIsValidChannel(Channel))
    {
        return status;
    }

    OS_LockStack();

    if (RfIsChannelAllocated(Channel) && Channel->state == DLC_STATE_CONNECTED)
    {
        if (MUX(Channel->muxId).flags & MUX_FLAG_CREDIT_FLOW)
        {
            /* Report one less byte when credit flow control is on */
            status = (U16) (DLC(Channel->muxId, Channel->dlcId).channel->maxFrameSize - 1);
        }
        else
        {
            status = (U16) (DLC(Channel->muxId, Channel->dlcId).channel->maxFrameSize);
        }
    }

    OS_UnlockStack();

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  RF_QuerySpecVersion
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL RF_QuerySpecVersion(RfChannel *Channel)
{
    BOOL status = FALSE;

    if (!RfIsValidChannel(Channel))
    {
        return status;
    }
    if (MUX(Channel->muxId).flags & MUX_FLAG_CREDIT_FLOW)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  RF_AdvanceCredit
 * DESCRIPTION
 *  Advance transmit credit to the remote device.
 * PARAMETERS
 *  Channel     [IN]        
 *  Credit      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_AdvanceCredit(RfChannel *Channel, U8 Credit)
{
    BtStatus status = BT_STATUS_SUCCESS;

    if (!RfIsValidChannel(Channel))
    {
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();
    if (RfIsChannelAllocated(Channel) && Channel->state == DLC_STATE_CONNECTED)
    {
        if ((Credit + Channel->rxCredit + Channel->grantedCredit) <= 255)
        {
            Channel->rxCredit = (S16) (Channel->rxCredit + Credit);
            RfCheckForCreditToSend(Channel);
        }
        else
        {
            /* Too much credit advanced */
            status = BT_STATUS_FAILED;
        }
    }
    else
    {
        /* Not connected */
        status = BT_STATUS_NO_CONNECTION;
    }

    OS_UnlockStack();

    return status;
}



/*****************************************************************************
 * FUNCTION
 *  RF_CreditFlowEnabled
 * DESCRIPTION
 *  Identifies whether credit based flow control has been negotiated
 *  for the currrent RFCOMM session or not.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  TRUE - credit flow is enabled.
 *  FALSE - credit flow is not enabled.
 *****************************************************************************/
BOOL RF_CreditFlowEnabled(RfChannel *Channel)
{
    BOOL enabled = FALSE;

    if (!RfIsValidChannel(Channel))
    {
        return FALSE;
    }

    OS_LockStack();

    if (RfIsChannelAllocated(Channel))
    {
        enabled = (BOOL)((MUX(Channel->muxId).flags & MUX_FLAG_CREDIT_FLOW) ? TRUE : FALSE);
    }

    OS_UnlockStack();

    return enabled;
}


/*****************************************************************************
 * FUNCTION
 *  RF_RemoteDevice
 * DESCRIPTION
 *  Returns the remote device associated with the specified channel.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  BtRemoteDevice - a pointer to the remote device structure used
 *  for the specified channel.
 *****************************************************************************/
BtRemoteDevice *RF_RemoteDevice(RfChannel *Channel)
{
    BtRemoteDevice *remDev = 0;

    if (!RfIsValidChannel(Channel))
    {
        return 0;
    }

    OS_LockStack();

    if (RfIsChannelAllocated(Channel))
    {
        remDev = MUX(Channel->muxId).remDev;
    }

    OS_UnlockStack();

    return remDev;
}


/*****************************************************************************
 * FUNCTION
 *  RF_SendAggregateFlow
 * DESCRIPTION
 *  Send the aggregate flow command.  This is provided for
 *  Unplugfest support.
 * PARAMETERS
 *  Channel     [IN]        
 *  state       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_SendAggregateFlow(RfChannel *Channel, U8 state)
{
    BtStatus status;

    if (!RfIsValidChannel(Channel))
    {
        OS_UnlockStack();
        return FALSE;
    }

    OS_LockStack();

    /*
     * If credit flow control has been negotiated, this command
     * cannot be sent.
     */
    if (MUX(Channel->muxId).flags & MUX_FLAG_CREDIT_FLOW)
    {
        OS_UnlockStack();
        return BT_STATUS_FAILED;
    }

    if (state)
    {
        Channel->cmdSent = FCON | CR_BIT;
    }
    else
    {
        Channel->cmdSent = FCOFF | CR_BIT;
    }

    status = RfSendAggregateFlow(Channel->muxId, state, CR_BIT);
    if (status == BT_STATUS_PENDING)
    {
        EVM_StartTimer(&(Channel->timer), RF_T2_TIMEOUT);
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_START_TIMER_SEND_CMD_ON_DLCI_OF_MUX_TIME, Channel->cmdSent, Channel->dlci, Channel->muxId, RF_T2_TIMEOUT/1000);
        DLC(Channel->muxId, Channel->dlcId).flags |= DLC_FLAG_REQ_SENT;
    }

    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RF_SendBadCommand
 * DESCRIPTION
 *  Send an unsupported commmand.  This is provided for
 *  Unplugfest support.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_SendBadCommand(RfChannel *Channel)
{
    BtPacket *packet;
    BtStatus status;

    if (!RfIsValidChannel(Channel))
    {
        return FALSE;
    }

    OS_LockStack();

    if (!(IsListEmpty(&(MUX(Channel->muxId).freeList))))
    {
        packet = (BtPacket*) RemoveHeadList(&(MUX(Channel->muxId).freeList));
    }
    else
    {
        OS_UnlockStack();
        return BT_STATUS_NO_RESOURCES;
    }

    packet->dataLen = 2;
    packet->data[0] = INVALID_CMD | CR_BIT;
    Channel->cmdSent = INVALID_CMD | CR_BIT;
    packet->data[1] = EA_BIT;
    packet->ulpContext = (void*)0;
    status = RfSendDataInternal(Channel->muxId, 0, packet, BTP_FLAG_HIGH_PRIPORTY);
    if (status != BT_STATUS_PENDING)
    {
        /* Send failed */
        InsertTailList(&(MUX(Channel->muxId).freeList), &(packet->node));
    }
    else
    {
        EVM_StartTimer(&(DLC(Channel->muxId, Channel->dlcId).channel->timer), RF_T2_TIMEOUT);
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMM_START_TIMER_SEND_CMD_ON_DLCI_OF_MUX_TIME, Channel->cmdSent, Channel->dlci, Channel->muxId, RF_T2_TIMEOUT/1000);
        DLC(Channel->muxId, Channel->dlcId).flags |= DLC_FLAG_REQ_SENT;
    }

    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RF_SendBadMSC
 * DESCRIPTION
 *  Send an MSC to an invalid dlci.  This is provided for
 *  Unplugfest support.
 * PARAMETERS
 *  Channel         [IN]        
 *  ModemStatus     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_SendBadMSC(RfChannel *Channel, RfModemStatus *ModemStatus)
{
    BtStatus status;

    if (!RfIsValidChannel(Channel))
    {
        OS_UnlockStack();
        return FALSE;
    }

    OS_LockStack();

    Channel->dlci = 0x23;
    status = RfSendModemStatus(Channel, ModemStatus->signals, ModemStatus->breakLen, CR_BIT);
    OS_UnlockStack();
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RF_SendPN
 * DESCRIPTION
 *  Send a PN command.  This is provided for Unplugfest support.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_SendPN(RfChannel *Channel)
{
    return RfSendParmNegotiation(Channel, CR_BIT);
}

/*****************************************************************************
 * FUNCTION
 *  OpenClientChannelPrintIndex
 * DESCRIPTION
 *  
 * PARAMETERS
 *  index     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
void OpenClientChannelPrintIndex(U8 index)
{
    bt_trace(TRACE_GROUP_1, BT_RFCOMM_OPENCLI_FUNC_ENTRY, index);
}
BtStatus RF_OpenJSR82ClientChannel(BtRemoteDevice *RemDev, U8 ServerId, RfChannel *Channel, U8 Credit, U8 security)
{
    BtStatus status = BT_STATUS_FAILED;
    U8 muxId;
    I8 i;

#if (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED)
    U8 oldState = Channel->state;
#endif /* (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED) */
    OpenClientChannelPrintIndex(0);
    if (RemDev == 0)
    {
        OpenClientChannelPrintIndex(1);
       return BT_STATUS_INVALID_PARM;
    }

    if ((ServerId == 0) || (ServerId > 30))
    {
        OpenClientChannelPrintIndex(2);
        return BT_STATUS_INVALID_PARM;
    }

    if (Channel == 0)
    {
        OpenClientChannelPrintIndex(3);
        return BT_STATUS_INVALID_PARM;
    }

    if ((Channel->callback == 0) ||
        (Channel->priority > 63) ||
        (Channel->maxFrameSize > RF_MAX_FRAME_SIZE) || (Channel->maxFrameSize < RF_MIN_FRAME_SIZE))
    {
        OpenClientChannelPrintIndex(4);
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();

    /* See if this channel is allocated already */
    if (RfFindMuxIdByRemDev(RemDev, &muxId) == BT_STATUS_SUCCESS)
    {
        for (i = 0; i < NUM_RF_CHANNELS; i++)
        {
            if ((DLC(muxId, i).flags & DLC_FLAG_ALLOCATED) && (DLC(muxId, i).channel == Channel))
            {
                OS_UnlockStack();
                OpenClientChannelPrintIndex(5);
                return BT_STATUS_IN_USE;
            }
        }
    }

    Channel->rxCredit = Credit;
    Channel->BusyCredit = 0;
    Channel->creditTimer.func = 0;
    Channel->initialRxCredit = Credit;
    Channel->initialFrameSize = Channel->maxFrameSize;
    
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    if(security == 1)
        RF_RegisterSecurityRecord(Channel, FALSE, 0);
#endif 

    if(security == 1)
    {
        oldState = Channel->state;
        Channel->state = DLC_STATE_SEC_PENDING;
        Channel->secToken.id = SEC_RFCOMM_ID;
        Channel->secToken.remDev = RemDev;
        Channel->secToken.channel = (U32) Channel;
        Channel->secToken.incoming = FALSE;
        status = SEC_AccessRequest(&(Channel->secToken));
    }
    else
    {
        /* Check BT version */
        Report(("The remote device version is :%02x",RemDev->remoteVersion[0]));
        if( RemDev->remoteVersion[0] >= 4) /* Remote version >= 2.1*/
        {
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
            RF_RegisterSecurityRecord(Channel, FALSE, 0);
#endif         
            oldState = Channel->state;
            Channel->state = DLC_STATE_SEC_PENDING;
            Channel->secToken.id = SEC_RFCOMM_ID;
            Channel->secToken.remDev = RemDev;
            Channel->secToken.channel = (U32) Channel;
            Channel->secToken.incoming = FALSE;
            status = SEC_AccessRequest(&(Channel->secToken));
            RemDev->jsr82_used = 1;
        }
        else
        {
            status = BT_STATUS_SUCCESS;
        }
    }
    if (status == BT_STATUS_SUCCESS)
    {
        Channel->state = oldState;
        status = RfOpenClientChannel(RemDev, ServerId, Channel);
        if (status != BT_STATUS_PENDING)
        {
                OpenClientChannelPrintIndex(6);
                /* 
                  * MAUI_01105021:
                  * Because the RF_RegisterSecurityRecord is invoked previously, if open chnl is failed: it should de-register security record 
                  * Otherwise, some profiles (e.g., BIP) it will not de-register security and reset its context directly: This will casue unexpected error!
                  */
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
                RF_DeRegisterSecurityRecord(Channel);
#endif 
                
        }
    }
    else if (status != BT_STATUS_PENDING)
    {
#if (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED)
        Channel->state = oldState;
#endif
        status = BT_STATUS_RESTRICTED;
    }
    else
    {
        /* Temporarily use dlci to store the requested ServerID */
        Channel->dlci = ServerId;
        RemDev->refCount++; // Increase the refCount temporarily to avoid the L2CAP link is disconencted during authentication.
        Report(("temp refCount++"));        
    }

    OS_UnlockStack();

    return status;
}


BtStatus RF_JSR82RegisterServerChannel(RfChannel *Channel, RfService *Service, U8 Credit, U8 security)
{
    BtStatus status;

    if (Channel == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if (Channel->callback == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if (Service == 0)
    {
        return BT_STATUS_INVALID_PARM;
    }

    if ((Channel->maxFrameSize > RF_MAX_FRAME_SIZE) || (Channel->maxFrameSize < RF_MIN_FRAME_SIZE))
    {
        return BT_STATUS_INVALID_PARM;
    }

    OS_LockStack();

    Channel->rxCredit = Credit;
    Channel->BusyCredit = 0;
    Channel->creditTimer.func = 0;
    Channel->initialRxCredit = Credit;
    Channel->initialFrameSize = Channel->maxFrameSize;
    Channel->remDev = 0;

    Channel->rfChFlags = 0;
    /* Get the new Server Channel */
    status = RfAllocService(Channel, Service);
#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    if(security == 1)
    {
        if (status == BT_STATUS_SUCCESS)
        {
            RF_RegisterSecurityRecord(Channel, TRUE, Service->serviceId);
        }
    }
    else
    {
        if (status == BT_STATUS_SUCCESS)
        {
            btbmGAPSetCTSServerSecurityLow(1);
        }
    }
#endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 
    OS_UnlockStack();

    return status;
}


void RF_JSR82_DeRegisterServerChannel(void)
{
    btbmGAPSetCTSServerSecurityLow(0);
}
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */

