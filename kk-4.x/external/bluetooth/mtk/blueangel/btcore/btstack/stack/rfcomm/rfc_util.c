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
 *     $Workfile:rfc_util.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Utility functions for the RFCOMM protocol.
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
#include "rfcomm.h"
#include "bt_feature.h"
#include "sys/rfcexp.h"

#if RFCOMM_PROTOCOL == XA_ENABLED

/* Function prototypes */
static void RfFreeAllDLCs(U8 MuxId);
static void RfServiceTimeoutHandler(EvmTimer *timer);

#define  IS_MAP_SERVER(serviceType) \
    (serviceType >= 10 && serviceType <= 15)

/*****************************************************************************
 * FUNCTION
 *  RF_Init
 * DESCRIPTION
 *  Initialize RFCOMM.  Register with L2CAP, initialize state and
 *  other variables.
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RF_Init(void)
{
#if NUM_RF_SERVERS != 0
    I8 i;
#endif /* NUM_RF_SERVERS != 0 */

    /* Register RFCOMM with L2CAP */
    RFC(psm).callback = RfL2Callback;
    RFC(psm).psm = BT_PSM_RFCOMM;
    RFC(psm).localMtu = RF_MAX_FRAME_SIZE + 5;
    RFC(psm).minRemoteMtu = RF_MIN_FRAME_SIZE + 5;
    RFC(psm).authorizedSetting = FALSE;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
//    RFC(psm).inLinkMode = RFC(psm).outLinkMode = L2MODE_BASIC | L2MODE_RETRANSMISSION | L2MODE_FLOW_CONTROL;
    RFC(psm).inLinkMode = L2MODE_BASIC;
    RFC(psm).outLinkMode = L2MODE_BASIC;


//    RFC(psm).outLinkMode = L2MODE_BASIC | L2MODE_FLOW_CONTROL;
    RFC(psm).inTransmitMax = 50;
    RFC(psm).channelUsage = L2CAP_CHANNEL_GENERAL;

    RFC(psm).inBufferSize = (RF_MAX_FRAME_SIZE + 5)*10;
    RFC(psm).inRetransTimeout = 200;
    RFC(psm).inMonitorTimeout = 200;
    RFC(psm).l2capExtFlowSpec.identifier  = 0x01;
    RFC(psm).l2capExtFlowSpec.serviceType = L2QOS_OPTION_GUARANTEED;
    RFC(psm).l2capExtFlowSpec.maxSduSize = 0xFFFF;
    RFC(psm).l2capExtFlowSpec.sduIntArrialTime = 0xFFFFFFFF;
    RFC(psm).l2capExtFlowSpec.accessLatency = 0xFFFFFFFF;
    RFC(psm).l2capExtFlowSpec.flushTimeout = 0xFFFFFFFF;
#ifdef __BT_2_1_ERTM__
    RFC(psm).lockStepNeeded = 0;
    RFC(psm).highSpeedRequired = 0;
#endif

#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */
    if (L2CAP_RegisterPsm(&(RFC(psm))) != BT_STATUS_SUCCESS)
    {
        /* Unable to register */
        return BT_STATUS_FAILED;
    }

#if NUM_RF_SERVERS != 0
    for (i = 0; i < NUM_RF_SERVERS; i++)
    {
        InitializeListHead(&(RFC(servers)[i].channelList));
    }
#endif /* NUM_RF_SERVERS != 0 */

    /* Load the state table */
    RFC(state)[DLC_STATE_DISCONNECTED] = RfDlcStateDisconnected;
    RFC(state)[DLC_STATE_CONNECTED] = RfDlcStateConnected;
    RFC(state)[DLC_STATE_CONN_PENDING] = RfDlcStateConnPending;
    RFC(state)[DLC_STATE_CONN_INCOMING] = RfDlcStateConnIncoming;
    RFC(state)[DLC_STATE_DISC_PENDING] = RfDlcStateDiscPending;
    RFC(state)[DLC_STATE_DISC_INCOMING] = RfDlcStateDiscIncoming;
    RFC(state)[DLC_STATE_NEG_PENDING] = RfDlcStateNegPending;
#if (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED)
    RFC(state)[DLC_STATE_SEC_PENDING] = RfDlcStateSecPending;
#endif 
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  RF_Deinit
 * DESCRIPTION
 *  Deinitialize RFCOMM.  Deregister with L2CAP.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void RF_Deinit(void)
{
#if L2CAP_DEREGISTER_FUNC == XA_ENABLED
    L2CAP_DeregisterPsm(&(RFC(psm)));
#endif /* L2CAP_DEREGISTER_FUNC == XA_ENABLED */
}


/*****************************************************************************
 * FUNCTION
 *  RfIsValidChannel
 * DESCRIPTION
 *  Validate the channel structure.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL RfIsValidChannel(RfChannel *Channel)
{
    if (Channel == 0)
    {
        return FALSE;
    }

    if (Channel->dlcId >= NUM_RF_CHANNELS)
    {
        return FALSE;
    }

    if (Channel->muxId >= NUM_BT_DEVICES)
    {
        return FALSE;
    }

    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  RfIsChannelAllocated
 * DESCRIPTION
 *  See if a channel is in the connected state.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL RfIsChannelAllocated(RfChannel *Channel)
{
    if (DLC(Channel->muxId, Channel->dlcId).channel != Channel)
    {
        return FALSE;
    }

    if (!(DLC(Channel->muxId, Channel->dlcId).flags & DLC_FLAG_ALLOCATED))
    {
        return FALSE;
    }

    return TRUE;
}

#if NUM_RF_SERVERS != 0


/*****************************************************************************
 * FUNCTION
 *  RfFreeUsePendingChannel
 * DESCRIPTION
 *  Free a service resource that was requested, but never connected.
 * PARAMETERS
 *  channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfFreeUsePendingChannel(RfChannel *channel)
{
    Assert(channel->rfChFlags & CHNL_FLAG_USE_PENDING);
    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_CHNL_REGED_NOT_USED);
    channel->maxFrameSize = channel->initialFrameSize;
    channel->rxCredit = channel->initialRxCredit;
    channel->rfChFlags = 0;
}


/*****************************************************************************
 * FUNCTION
 *  RfServiceTimeoutHandler
 * DESCRIPTION
 *  Timeout handler for PN and RPN requests for a service before
 *  a connection is established.  If a subsequent SABM is not
 *  received withing a reasonable amount of time, then this timer
 *  will fire and free up the service resource.
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfServiceTimeoutHandler(EvmTimer *timer)
{
    U8 serviceId = (U16) ((I32) timer->context) & 0x00FF;
    U8 muxId = (U16) ((I32) timer->context) >> 8;
    RfChannel *channel;
bt_prompt_trace(MOD_BT, "[BT] RfServiceTimeoutHandler");
    if (RfGetFreeServiceChannel(muxId, serviceId, &channel) == BT_STATUS_SUCCESS)
    {
        /*
         * This channel was requested, but then never used.  A PN
         * or RPN request was received, but no SABM.
         */

        /* Check for active link */
        if ((ME_GetCurrentMode(MUX(muxId).remDev) == BLM_HOLD_MODE) ||
            (ME_GetCurrentMode(MUX(muxId).remDev) == BLM_PARK_MODE))
        {
            /* The link is in hold, park or sniff, restart the timer. */
            EVM_StartTimer(timer, SRV_CHNL_PENDING_TIMEOUT);
            return;
        }

        RfFreeUsePendingChannel(channel);
    }

}
#endif /* NUM_RF_SERVERS != 0 */ 

#if NUM_RF_SERVERS != 0


/*****************************************************************************
 * FUNCTION
 *  RfAllocService
 * DESCRIPTION
 *  Find an open server slot and assign it.
 *  If no open slot exists, then an error is returned and the channel
 *  number is set to 0 (1 - 30 is valid).
 * PARAMETERS
 *  Channel     [IN]        
 *  Service     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfAllocService(RfChannel *Channel, RfService *Service)
{
    BtStatus status = BT_STATUS_FAILED;
    I8 i;
    I8 slot = 0xfe;
    /* Some Profile will set serviceId as 0, e.g., HFG_Register() */ 
    if (Service->serviceId == 0)
    {
        /* Look for a new service */
        for (i = NUM_RF_SERVERS; i > 0; i--)
        {
            /* Search for an open slot */
            if (!(RFC(servers)[i - 1].flags & SRV_FLAG_ALLOCATED))
            {
                /* Free slot */
                slot = i - 1;
                Service->serviceId = (U8) i;
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                /* Make sure the channel is not already registered */
                if (IsNodeOnList(&RFC(servers[i - 1].channelList), &(Channel->node)))
                {
                    status = BT_STATUS_IN_USE;
                    break;
                }
            }
        }
    }
    else if (Service->serviceId <= NUM_RF_SERVERS)
    {
        /* Adding a channel to the service */
        slot = Service->serviceId - 1;
#ifdef __BT_MAPS_PROFILE__
        if (RFC(servers)[slot].service != 0x00 && !IS_MAP_SERVER(Service->serviceId))
#else
        if (RFC(servers)[slot].service != 0x00)
#endif
        {
            if (RFC(servers)[slot].service == Service)
            {
                status = BT_STATUS_SUCCESS;
            }
            else
            {
                status = BT_STATUS_FAILED;
            }
        }
        else
        {
            status = BT_STATUS_SUCCESS;
        }
    }
    if (status == BT_STATUS_SUCCESS)
    {
        /* Add the channel to the list of channels for the service */
        if (!IsNodeOnList(&(RFC(servers)[slot].channelList), &(Channel->node)))
        {
            InsertTailList(&(RFC(servers[slot].channelList)), &(Channel->node));
            RFC(servers)[slot].flags |= SRV_FLAG_ALLOCATED;
            RFC(servers)[slot].service = Service;
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_REG_CHNL_SERV, slot + 1);
        }
        else
        {
            status = BT_STATUS_FAILED;
        }
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfFreeServerChannel
 * DESCRIPTION
 *  Free a service.
 * PARAMETERS
 *  Channel     [IN]        
 *  Service     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfFreeServerChannel(RfChannel *Channel, RfService *Service)
{
    BtStatus status = BT_STATUS_FAILED;
    U8 slot = Service->serviceId - 1;

    /* Free the server channel */
    if (IsNodeOnList(&(RFC(servers)[slot].channelList), &(Channel->node)))
    {
        if (!(Channel->rfChFlags & CHNL_FLAG_IN_USE))
        {
            /* The channel is not in use, remove it from the service */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_DE_REG_CHNL, Service->serviceId);
            RemoveEntryList(&(Channel->node));
            if (Channel->rfChFlags & CHNL_FLAG_USE_PENDING)
            {
                EVM_CancelTimer(&(Channel->timer));
            }

            if (IsListEmpty(&(RFC(servers)[slot].channelList)))
            {
                /*
                 * This is the last channel register on this service, 
                 * free the service ID 
                 */
                RFC(servers)[slot].flags = 0;
            	RFC(servers)[slot].service = 0;
            }        
            #if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
            RF_DeRegisterSecurityRecord(Channel);            
            if (!IsListEmpty(&(RFC(servers)[slot].channelList)))
            {
                /* Replace original security record by new one */
                bt_trace(TRACE_GROUP_1, RF01084421);
                Channel = (RfChannel*) GetHeadList(&(RFC(servers)[slot].channelList));
                RF_RegisterSecurityRecord(Channel, TRUE, Service->serviceId);
            }
            #endif 

            status = BT_STATUS_SUCCESS;
        }
        else
        {
            /* The channel is still in use */
            status = BT_STATUS_BUSY;
        }
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfFreeService
 * DESCRIPTION
 *  Frees all channels from a service.
 * PARAMETERS
 *  Service     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfFreeService(RfService *Service)
{
    BtStatus status = BT_STATUS_FAILED;
    U8 slot = Service->serviceId - 1;
    RfChannel *channel;

    if (RFC(servers)[slot].inUseCnt == 0)
    {
        /* Free the service (no channels are in use) */
        if (RFC(servers)[slot].service == Service)
        {
            if (RFC(servers)[slot].flags & SRV_FLAG_ALLOCATED)
            {
                /* Free all channels from the service */
                while (!IsListEmpty(&(RFC(servers)[slot].channelList)))
                {
                    channel = (RfChannel*) GetHeadList(&(RFC(servers)[slot].channelList));
                    AssertEval(RfFreeServerChannel(channel, Service) == BT_STATUS_SUCCESS);
                }
            }

            /* Free the service */
            RFC(servers)[slot].flags = 0;
            RFC(servers)[slot].service = 0;
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_DE_REG_ALL_CHNL, Service->serviceId);
            status = BT_STATUS_SUCCESS;
        }
    }
    else
    {
        status = BT_STATUS_BUSY;
    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfGetFreeServiceChannel
 * DESCRIPTION
 *  Get an unused channel from the specified service.
 * PARAMETERS
 *  MuxId           [IN]        
 *  ServiceId       [IN]        
 *  Channel         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfGetFreeServiceChannel(U8 MuxId, U8 ServiceId, RfChannel **Channel)
{
    RfChannel *entry;
    BtStatus status = BT_STATUS_FAILED;

    if ((ServiceId > 0) && ((ServiceId - 1) < NUM_RF_SERVERS) && RFC(servers)[ServiceId - 1].flags & SRV_FLAG_ALLOCATED)
    {

        Assert(!IsListEmpty(&(RFC(servers)[ServiceId - 1].channelList)));
        entry = (RfChannel*) GetHeadList(&(RFC(servers)[ServiceId - 1].channelList));

        /* Walk the list */
        while ((ListEntry*) entry != &(RFC(servers)[ServiceId - 1].channelList))
        {
            if (!(entry->rfChFlags & CHNL_FLAG_IN_USE))
            {

                *Channel = entry;

                if ((*Channel)->rfChFlags & CHNL_FLAG_USE_PENDING)
                {
                    if ((*Channel)->muxId == MuxId)
                    {
                        /* This channel has been requested once already */
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_CHNL_REQ_PREVIOUS);
                        status = BT_STATUS_SUCCESS;
                        break;
                    }
                }
                else
                {
                    /* The channel is free and has not be requested previously */
                    bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_CHNL_REQ_FIRST);
                    (*Channel)->rfChFlags |= CHNL_FLAG_USE_PENDING;
                    (*Channel)->muxId = MuxId;
                    (*Channel)->timer.func = RfServiceTimeoutHandler;
                    (*Channel)->timer.context = (void*)(MuxId << 8 | ServiceId);
                    EVM_StartTimer(&(*Channel)->timer, SRV_CHNL_PENDING_TIMEOUT);
                    status = BT_STATUS_SUCCESS;
                    break;
                }

            }
            entry = (RfChannel*) GetNextNode(&(entry->node));
        }
    }

    return status;
}
#endif /* NUM_RF_SERVERS != 0 */

/*****************************************************************************
 * FUNCTION
 *  RfAllocMux
 * DESCRIPTION
 *  Allocate a multiplexer for the specified Remote Device.
 * PARAMETERS
 *  RemDev          [IN]        
 *  L2ChannelId     [IN]        
 *  MuxId           [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfAllocMux(BtRemoteDevice *RemDev, L2capChannelId L2ChannelId, U8 *MuxId)
{
    BtStatus status = BT_STATUS_FAILED;
    I8 i;
    I8 slot = 0;
    U8 *ptr;

    for (i = NUM_BT_DEVICES; i > 0; i--)
    {
        /* Search for an open slot */
        if (MUX(i - 1).flags & MUX_FLAG_ALLOCATED)
        {
            /* Slot is occupied */
            if (RemDev == MUX(i - 1).remDev)
            {
                /* Error, already allocated */
                slot = i - 1;
                status = BT_STATUS_IN_USE;
                break;
            }
        }
        else
        {
            /* Free slot */
            slot = i - 1;
            status = BT_STATUS_SUCCESS;
        }
    }

    if (status == BT_STATUS_SUCCESS)
    {
        /* Initialize this mux */
        ptr = MUX(slot).reqBuff;
        InitializeListHead(&(MUX(slot).freeList));
        for (i = 0; i < (NUM_RF_CHANNELS + 3); i++)
        {
            /* Initialize Tx packets for control channel */
            MUX(slot).packets[i].data = ptr;
            ptr += MAX_CMD_RSP_SIZE;
            InsertTailList(&(MUX(slot).freeList), &(MUX(slot).packets[i].node));
        }

    #if RF_SEND_TEST == XA_ENABLED
        MUX(slot).testCmdPacket.data = MUX(slot).testCmdData;
    #endif /* RF_SEND_TEST == XA_ENABLED */
        MUX(slot).testRspPacket.data = MUX(slot).testRspData;

        InitializeListHead(&(MUX(slot).cmdQueue));

        MUX(slot).remDev = RemDev;
        MUX(slot).l2ChannelId = L2ChannelId;
        MUX(slot).flags = MUX_FLAG_ALLOCATED;
        MUX(slot).state = MUX_STATE_CLOSED;
        MUX(slot).timer.func = RfTimeoutHandler;
        MUX(slot).timer.context = (void*)((slot << 8) | INVALID_DLC_ID);
        MUX(slot).packetsInTransit = 0;
        InitializeListHead(&(MUX(slot).priorityList));
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
        InitializeListHead(&MUX(slot).rxPacketsBufferList);
        InitializeListHead(&MUX(slot).receivedPacketsList);
        OS_MemSet((U8 *)&MUX(slot).l2capRawData, 0, sizeof(MUX(slot).l2capRawData));
        for (i = 0; i < RF_NUM_RETRANSMISSION_WINDOW_SIZE ; i++)
        {
            InitializeListEntry(&(MUX(slot).rfcRxPackets[i].node));
            MUX(slot).rfcRxPackets[i].data = (U8 *)(MUX(slot).l2capRawData[i]);
            InsertTailList(&MUX(slot).rxPacketsBufferList, &MUX(slot).rfcRxPackets[i].node);
        }        
#endif
    }

    *MuxId = (U8) (slot);

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfFreeMux
 * DESCRIPTION
 *  Free the multiplexer specified by MuxId.
 * PARAMETERS
 *  MuxId       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfFreeMux(U8 MuxId)
{
    if (MUX(MuxId).flags & MUX_FLAG_ALLOCATED)
    {

        if (MUX(MuxId).flags & MUX_FLAG_CMD_SENT)
        {
            /* Command outstanding, cancel the timer */
            EVM_CancelTimer(&(MUX(MuxId).timer));
        }

        /* Free the multiplexer */
        MUX(MuxId).state = MUX_STATE_CLOSED;
        MUX(MuxId).flags = 0;
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_MUX_FREEED, MuxId);

        /* Free all DLCs */
        if (MUX(MuxId).dlcCount)
        {
            RfFreeAllDLCs(MuxId);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfFindMuxIdByRemDev
 * DESCRIPTION
 *  Find the multiplexer associated with the Remote Device.
 * PARAMETERS
 *  RemDev      [IN]        
 *  MuxId       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfFindMuxIdByRemDev(BtRemoteDevice *RemDev, U8 *MuxId)
{
    I8 i;
    BtStatus status = BT_STATUS_FAILED;

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        /* Search for an allocated mux */
        if (MUX(i).flags & MUX_FLAG_ALLOCATED)
        {
            if (MUX(i).remDev == RemDev)
            {
                *MuxId = (U8) i;
                status = BT_STATUS_SUCCESS;
                break;
            }
        }
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfAllocDLC
 * DESCRIPTION
 *  Allocate a DLC in the designated multiplexer.
 * PARAMETERS
 *  MuxId       [IN]        
 *  Dlci        [IN]        
 *  Channel     [IN]        
 *  DlcId       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfAllocDLC(U8 MuxId, U8 Dlci, RfChannel *Channel, U8 *DlcId)
{
    I8 i;
    I8 slot = 0;
    BtStatus status = BT_STATUS_FAILED;
    RF_DLC *ptr;

    for (i = NUM_RF_CHANNELS; i > 0; i--)
    {
        /* Search for an open slot */
        if (!(DLC(MuxId, i - 1).flags & DLC_FLAG_ALLOCATED))
        {
            /* Free slot */
            slot = i - 1;
            status = BT_STATUS_SUCCESS;
        }
        else
        {
            if (DLC(MuxId, i - 1).channel->dlci == Dlci)
            {
                /* This channel is already in use */
                status = BT_STATUS_IN_USE;
                break;
            }
        }
    }

    if (status == BT_STATUS_SUCCESS)
    {
        /* Initialize the structure */
        if (Channel->rfChFlags & CHNL_FLAG_USE_PENDING)
        {
            EVM_CancelTimer(&(Channel->timer));
            Channel->rfChFlags &= ~CHNL_FLAG_USE_PENDING;
        }

        *DlcId = (U8) slot;
        MUX(MuxId).dlcCount++;
        DLC(MuxId, slot).channel = Channel;
        DLC(MuxId, slot).flags |= DLC_FLAG_ALLOCATED;
        Channel->muxId = MuxId;
        Channel->dlcId = (U8) slot;
        Channel->dlci = Dlci;
        Channel->timer.func = RfTimeoutHandler;
        Channel->timer.context = (void*)(MuxId << 8 | slot);
        Channel->state = DLC_STATE_DISCONNECTED;

        Channel->rfChFlags = CHNL_FLAG_IN_USE;

        Channel->rfSignals = RF_RTR | RF_RTC | RF_DV;

        InitializeListHead(&(Channel->txQueue));

        /* Find a spot on the priority list */
        if (!IsListEmpty(&(MUX(MuxId).priorityList)))
        {
            ptr = (RF_DLC*) GetHeadList(&(MUX(MuxId).priorityList));
            while (GetNextNode(&(ptr->node)) != &(MUX(MuxId).priorityList))
            {
                if (Channel->priority > ptr->channel->priority)
                {
                    ptr = (RF_DLC*) GetNextNode(&(ptr->node));
                }
                else
                {
                    /* Found the right spot in the priority list */
                    break;
                }
            }
        }
        else
        {
            ptr = (RF_DLC*) (&(MUX(MuxId).priorityList));
        }

        /* Insert into the priority list */
        InsertHeadList((ListEntry*) ptr, &(DLC(MuxId, slot).node));
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_ALLOCATED_DLCI_ON_MUX, Dlci, MuxId);

    }

    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfFreeDLC
 * DESCRIPTION
 *  Free the DLC in the specified multiplexer.
 * PARAMETERS
 *  MuxId       [IN]        
 *  DlcId       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfFreeDLC(U8 MuxId, U8 DlcId)
{
    RfChannel *channel;
    BOOL do_callback = TRUE;

#if NUM_RF_SERVERS != 0
    RF_SERVER *server;
#endif /* NUM_RF_SERVERS != 0 */

    if ((DLC(MuxId, DlcId).flags & DLC_FLAG_ALLOCATED) ==0)
    {
        Assert(0);
    }
    if ((MUX(MuxId).dlcCount) ==0)
    {
        Assert(0);
    }

    channel = DLC(MuxId, DlcId).channel;

    if ((DLC(MuxId, DlcId).flags & DLC_FLAG_REQ_SENT) || (DLC(MuxId, DlcId).flags & DLC_FLAG_MSC_PENDING))
    {
        /* A timer is running if a request has been sent */
        EVM_CancelTimer(&(DLC(MuxId, DlcId).channel->timer));
    }

#if (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED)
    if (DLC(MuxId, DlcId).channel->state == DLC_STATE_SEC_PENDING)
    {
        /* Cancel the security access request */
        do_callback = FALSE;
        AssertEval(SEC_CancelAccessRequest(&(channel->secToken)) == BT_STATUS_SUCCESS);
    }
    else if (DLC(MuxId, DlcId).channel->state == DLC_STATE_SEC_FAILED)
    {
        do_callback = FALSE;
    }
#endif /* (BT_SECURITY == XA_ENABLED) && (RF_SECURITY == XA_ENABLED) */

    DLC(MuxId, DlcId).channel->state = DLC_STATE_DISCONNECTED;

    /* Free the DLC */
    MUX(MuxId).dlcCount--;
    DLC(MuxId, DlcId).flags = 0;

    DLC(MuxId, DlcId).channel->rfChFlags = 0;

    /* Restore the initial credit */
    DLC(MuxId, DlcId).channel->rxCredit = DLC(MuxId, DlcId).channel->initialRxCredit;

#if NUM_RF_SERVERS != 0
    /* See if this DLC was bound to a local server */
    if (((DLC(MuxId, DlcId).channel->dlci >> 1) - 1) < NUM_RF_SERVERS)
    {
        server = &(RFC(servers)[(DLC(MuxId, DlcId).channel->dlci >> 1) - 1]);
        if (IsNodeOnList(&(server->channelList), &(DLC(MuxId, DlcId).channel->node)))
        {
            DLC(MuxId, DlcId).channel->maxFrameSize = DLC(MuxId, DlcId).channel->initialFrameSize;
            server->inUseCnt--;
        }
    }
#endif /* NUM_RF_SERVERS != 0 */

    /* Return all packets */
    while (!IsListEmpty(&(DLC(MuxId, DlcId).channel->txQueue)))
    {
        MUX(MuxId).txPacket = (BtPacket*) RemoveHeadList(&(DLC(MuxId, DlcId).channel->txQueue));
        RfAppCallback(DLC(MuxId, DlcId).channel, BT_STATUS_NO_CONNECTION, RFEVENT_PACKET_HANDLED, 0);
    }

    /* Remove the DLC from the priority list */
    RemoveEntryList(&(DLC(MuxId, DlcId).node));
    channel->remDev = 0;
    DLC(MuxId, DlcId).channel = 0;

    if (do_callback)
    {
    #if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
        if (channel->incoming == 0)
        {
            RF_DeRegisterSecurityRecord(channel);
        }
    #endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 
        RfAppCallback(channel, BT_STATUS_SUCCESS, RFEVENT_CLOSED, 0);
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfFreeAllDLCs
 * DESCRIPTION
 *  Free all the DLCs in the specified multiplexer.
 * PARAMETERS
 *  MuxId       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void RfFreeAllDLCs(U8 MuxId)
{
    I8 i;

    for (i = 0; i < NUM_RF_CHANNELS; i++)
    {
        if (DLC(MuxId, i).flags & DLC_FLAG_ALLOCATED)
        {
            RfFreeDLC(MuxId, (U8) i);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfFindDlcIdByDlci
 * DESCRIPTION
 *  Find the DLC in the multiplexer by the Dlci value.
 * PARAMETERS
 *  MuxId       [IN]        
 *  Dlci        [IN]        
 *  DlcId       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfFindDlcIdByDlci(U8 MuxId, U8 Dlci, U8 *DlcId)
{
    I8 i;
    BtStatus status = BT_STATUS_FAILED;

    for (i = 0; i < NUM_RF_CHANNELS; i++)
    {
        /* Search for an allocate channel with the correct DLC ID */
        if (DLC(MuxId, i).flags & DLC_FLAG_ALLOCATED)
        {
            if (DLC(MuxId, i).channel->dlci == Dlci)
            {
                *DlcId = (U8) i;
                status = BT_STATUS_SUCCESS;
                break;
            }
        }
    }

    if (status == BT_STATUS_FAILED)
    {
        *DlcId = INVALID_DLC_ID;
    }

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  RfFindConnPendingChannel
 * DESCRIPTION
 *  Find the channel in DLC_STATE_CONN_PENDING state in the multiplexer.
 * PARAMETERS
 *  MuxId       [IN]           
 * RETURNS
 *  RfChannel   
 *****************************************************************************/
RfChannel *RfFindConnPendingChannel(U8 MuxId)
{
    I8 i;
    RfChannel *channel = NULL;

    for (i = 0; i < NUM_RF_CHANNELS; i++)
    {       
        if (DLC(MuxId, i).flags & DLC_FLAG_ALLOCATED)    
        {
            if (DLC(MuxId, i).channel->state == DLC_STATE_CONN_PENDING)
            {
                channel = DLC(MuxId, i).channel;
                break;
            }
        }
    }

    return channel;
}

/*****************************************************************************
 * FUNCTION
 *  RfDisconnectDLC
 * DESCRIPTION
 *  Disconnect Data Link Channel.
 * PARAMETERS
 *  MuxId       [IN]        
 *  DlcId       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus RfDisconnectDLC(U8 MuxId, U8 DlcId)
{
    BtStatus status;

    /*
     * RFCOMM_INVALID_DISC:
     * We didn't change the state if CHNL_FLAG_DISC_QUEUED is on
     * We will change the state after the packet is passed to L2CAP.
     */
    if ((status = RfSendCommand(DISC, MuxId, DlcId)) == BT_STATUS_PENDING && 
        !(DLC(MuxId, DlcId).channel->rfChFlags & CHNL_FLAG_DISC_QUEUED))
    {
        DLC(MuxId, DlcId).channel->state = DLC_STATE_DISC_PENDING;
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  RfDisconnectL2CAP
 * DESCRIPTION
 *  Disconnect L2CAP.
 * PARAMETERS
 *  MuxId       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfDisconnectL2CAP(U8 MuxId)
{
    BtStatus status = BT_STATUS_FAILED;
    status = L2CAP_DisconnectReq(MUX(MuxId).l2ChannelId);
    
    if (status != BT_STATUS_PENDING && status != BT_STATUS_IN_PROGRESS)
    {
        /* There is some problem below, just abort */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_COULD_NOT_L2CAP_SHUTDOWN);
        RfFreeMux(MuxId);
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfTimeoutHandler
 * DESCRIPTION
 *  Timer handler for RFCOMM's acknowledgement/response timer.
 * PARAMETERS
 *  timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfTimeoutHandler(EvmTimer *timer)
{
    RfChannel *channel = NULL;    
    U8 dlcId = (U16) ((I32) timer->context) & 0x00FF;
    U8 muxId = (U16) ((I32) timer->context) >> 8;

bt_prompt_trace(MOD_BT, "[BT] RfTimeoutHandler");    
    if(timer->context == (void*)(0xFFFFFFFF))
    {
        Assert(0);
    
#ifdef __BT_MP_RELEASE__    
        OS_MemSet((U8 *)&btCtxrfc, 0, sizeof(btCtxrfc));
        return;
#else
        Assert(0);
#endif
        timer->func = 0;

    }
    /* Check for active link */
    if ((ME_GetCurrentMode(MUX(muxId).remDev) == BLM_HOLD_MODE) ||
        (ME_GetCurrentMode(MUX(muxId).remDev) == BLM_PARK_MODE))
    {
        /* The link is in hold or park mode, restart the timer. */
        EVM_StartTimer(timer, RF_T1_TIMEOUT);
        return;
    }

    if (dlcId != INVALID_DLC_ID)    /* RfChannel.timer timeout */
    {
        /* A request was sent, but no response was received */
        DLC(muxId, dlcId).flags &= ~DLC_FLAG_REQ_SENT;
    }
    else /* MUX(slot).timer timeout */
    {
        MUX(muxId).flags &= ~MUX_FLAG_CMD_SENT;
    }

    /* MAUI_03126607 && MAUI_03036057: when SABM is timeout under dlcCount isn't 1, 
       in order not to affect other DLC, we send DISC firstly, without waiting ack, free this DLC */ 
    if ((dlcId == INVALID_DLC_ID) && (MUX(muxId).dlcCount > 1))
    {
        channel = RfFindConnPendingChannel(muxId);    
        if (channel)
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_NO_RSP, 
                channel->dlci, muxId, channel->state, MUX(muxId).dlcCount);           
            if (channel->rfChFlags & CHNL_FLAG_DISC_QUEUED)
            {
                RfCommandSent(muxId);
            }    
            else
            {
                RfSendCommand(DISC, muxId, channel->dlcId);
            }
            MUX(muxId).flags &= ~MUX_FLAG_CMD_SENT;            
            EVM_CancelTimer(&(MUX(muxId).timer));               
            RfFreeDLC(muxId, channel->dlcId);   
        }
        else
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_LINK_LOSS, HCC(aclTxPacketsLeft));           
            RfDisconnectL2CAP(muxId);
        }
    }
    else
    {
        /* Assuming a link loss, shutting down multiplexer */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_LINK_LOSS, HCC(aclTxPacketsLeft));

        /* disconnect the MUX to conform to RFCOMM spec*/
        RfDisconnectL2CAP(muxId);
    }
    
    /*    
    if (DLC(muxId, MUX(muxId).cDlcId).channel->state == DLC_STATE_CONN_PENDING)
    {
        RfDisconnectDLC(muxId, MUX(muxId).cDlcId);
    }
    else
    {
        RfDisconnectL2CAP(muxId);
    }
    */
}


/*****************************************************************************
 * FUNCTION
 *  RfAppCallback
 * DESCRIPTION
 *  Call the application with the specified event.
 * PARAMETERS
 *  Channel     [IN]        
 *  status      [IN]        
 *  Event       [IN]        
 *  data        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfAppCallback(RfChannel *Channel, BtStatus status, U8 Event, void *data)
{
    RfCallbackParms parms;

#if RF_SEND_CONTROL == XA_DISABLED
    /* UNUSED_PARAMETER(data); */
#endif /* RF_SEND_CONTROL == XA_ENABLED */

    switch (Event)
    {
        case RFEVENT_PACKET_HANDLED:

            /* An application packet has been sent */
            if (MUX(Channel->muxId).packetsInTransit)
            {
                (MUX(Channel->muxId).packetsInTransit)--;
            }
            parms.ptrs.packet = MUX(Channel->muxId).txPacket;
            if(status == 0x11)
            {
                Report(("flush occurs"));
            }
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_PKT_RETURNED, Channel->dlci, status, parms.ptrs.packet->dataLen);
            RfMUXCheckTxQueues(Channel->muxId);
            break;
        case RFEVENT_DATA_IND:
            /* Data has been received for the application */
            if ((U8) MUX(Channel->muxId).rxBuff[2] & 0x01)
            {
                /* 1 Byte Length */
                parms.ptrs.data = MUX(Channel->muxId).rxBuff + 3;
                parms.dataLen = MUX(Channel->muxId).rxBuff[2] >> 1;
            }
            else
            {
                /* 2 Byte Length */
                parms.ptrs.data = MUX(Channel->muxId).rxBuff + 4;
                parms.dataLen = (MUX(Channel->muxId).rxBuff[2] >> 1) | (MUX(Channel->muxId).rxBuff[3] << 7);
            }

            if (MUX(Channel->muxId).flags & MUX_FLAG_CREDIT_FLOW)
            {
                /* Credit flow control is in effect */
                if (MUX(Channel->muxId).rxBuff[1] == UIH_F)
                {
                    /* Check for an overflow */
                    if ((Channel->txCredit + parms.ptrs.data[0]) < Channel->txCredit)
                    {
                        /* Overflow of transmit credit */
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_TX_CREDIT_OVERFLOW);
                        if (RfDisconnectDLC(Channel->muxId, Channel->dlcId) != BT_STATUS_PENDING)
                        {
                            RfDisconnectL2CAP(Channel->muxId);
                        }
                        return;
                    }
                    else
                    {
                        /* Get the credit amount */
                        Channel->txCredit += parms.ptrs.data[0];
                        parms.ptrs.data++;
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_NEW_TXCREDIT, Channel->txCredit);
                        RfMUXCheckTxQueues(Channel->muxId);
                    }
                }

                if (parms.dataLen > 0)
                {
                    /* Data was received */
                    if (Channel->grantedCredit == 0)
                    {
                        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_DATA_RECV_NO_CREDIT);
                        if (RfDisconnectDLC(Channel->muxId, Channel->dlcId) != BT_STATUS_PENDING)
                        {
                            RfDisconnectL2CAP(Channel->muxId);
                        }
                        return;
                    }
                    Channel->grantedCredit--;
                }
            }
            else
            {
                /* MSC flow control is in effect */
                if (parms.dataLen > 0)
                {
                    Channel->rxCredit--;
                }
            }

            if (parms.dataLen == 0)
            {
                /* 0 length data, ignore it */
                return;
            }
            break;
        case RFEVENT_MODEM_STATUS_IND:
            parms.ptrs.modemStatus = (RfModemStatus*) data;
            parms.dataLen = 2;
            break;

        #if RF_SEND_CONTROL == XA_ENABLED
        case RFEVENT_LINE_STATUS_IND:
            parms.ptrs.lineStatus = (RfLineStatus*) data;
            parms.dataLen = 1;
            break;
        case RFEVENT_PORT_NEG_IND:
        case RFEVENT_PORT_NEG_CNF:
        case RFEVENT_PORT_STATUS_CNF:
            if (data)
            {
                parms.ptrs.portSettings = (RfPortSettings*) data;
                parms.dataLen = (U16) sizeof(RfPortSettings);
            }
            else
            {
                parms.dataLen = 0;
            }
            break;

        #endif /* RF_SEND_CONTROL == XA_ENABLED */

        case RFEVENT_OPEN_IND:
        case RFEVENT_OPEN:
            parms.ptrs.remDev = MUX(Channel->muxId).remDev;
            parms.dataLen = (U16) sizeof(BtRemoteDevice);
            break;
        case RFEVENT_CLOSE_IND:
        case RFEVENT_CLOSED:
            if (MUX(Channel->muxId).remDev != 0)
            {
                parms.ptrs.remDev = MUX(Channel->muxId).remDev;
            }
            else
            {
                parms.ptrs.remDev = Channel->remDev;
            }
            parms.dataLen = (U16) sizeof(BtRemoteDevice);
            EVM_ResetTimer(&Channel->creditTimer);
            Channel->BusyCredit = 0;
            break;

        default:
            parms.ptrs.data = 0;
            parms.dataLen = 0;
            break;
    }

    parms.status = status;
    parms.event = Event;
    Channel->callback(Channel, &parms);

    if (Event == RFEVENT_DATA_IND)
    {
        RfCheckForCreditToSend(Channel);
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfCheckForCreditToSend
 * DESCRIPTION
 *  Checks to see if credit must be sent to the remote device.  To
 *  keep from sending an empty UIH frame every time credit is
 *  advanced, check to see if the amount of outstanding credits
 *  is larger than the granted credits.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfCheckForCreditToSend(RfChannel *Channel)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtPacket *packet;
    BOOL reqSent;

    if ((Channel->state != DLC_STATE_CONNECTED) || !(DLC(Channel->muxId, Channel->dlcId).flags & DLC_FLAG_MSC_RCVD))
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_NO_MSC);
        return;
    }

    /* Send credit if necessary */
    if (MUX(Channel->muxId).flags & MUX_FLAG_CREDIT_FLOW)
    {
        /* Credit flow control is enabled */
        if ((Channel->rxCredit > 0) && (Channel->rxCredit >= Channel->grantedCredit))
        {
            /* There is credit to be granted */
            if (IsListEmpty(&(Channel->txQueue)) || (Channel->txCredit == 0))
            {
                /* Credit must be sent in it's own 0 length packet */
                if (!(IsListEmpty(&(MUX(Channel->muxId).freeList))))
                {
                    packet = (BtPacket*) RemoveHeadList(&(MUX(Channel->muxId).freeList));
                    packet->dataLen = 0;
                    packet->ulpContext = (void*)1;
                    status = RfSendDataInternal(Channel->muxId, Channel->dlci, packet, BTP_FLAG_HIGH_PRIPORTY);
                    if (status != BT_STATUS_PENDING)
                    {
                        /* Send failed */
                        InsertTailList(&(MUX(Channel->muxId).freeList), &(packet->node));
                    }
                }
            }
        }
    }
    else
    {
        /* Credit flow was not negotiated */
        if (!(DLC(Channel->muxId, Channel->dlcId).flags & DLC_FLAG_MSC_PENDING))
        {
            /* A request is not already outstanding */
            reqSent = DLC(Channel->muxId, Channel->dlcId).flags & DLC_FLAG_REQ_SENT;
            if (Channel->rxCredit <= 0)
            {
                /* Out of credit, send an MSC to stop the flow */
                if (!(Channel->rfSignals & RF_FLOW))
                {
                    Channel->rfSignals |= RF_FLOW;
                    status = RfSendModemStatus(Channel, Channel->rfSignals, 0, CR_BIT);
                    if (status == BT_STATUS_PENDING)
                    {
                        if (!reqSent)
                        {
                            DLC(Channel->muxId, Channel->dlcId).flags &= ~DLC_FLAG_REQ_SENT;
                        }
                        DLC(Channel->muxId, Channel->dlcId).flags |= DLC_FLAG_MSC_PENDING;
                    }
                }
            }
            else
            {
                /* Credit available, send an MSC to start the flow */
                if (Channel->rfSignals & RF_FLOW)
                {
                    Channel->rfSignals &= ~RF_FLOW;
                    status = RfSendModemStatus(Channel, Channel->rfSignals, 0, CR_BIT);
                    if (status == BT_STATUS_PENDING)
                    {
                        if (!reqSent)
                        {
                            DLC(Channel->muxId, Channel->dlcId).flags &= ~DLC_FLAG_REQ_SENT;
                        }
                        DLC(Channel->muxId, Channel->dlcId).flags |= DLC_FLAG_MSC_PENDING;
                    }
                }
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfIsMuxFlowOn
 * DESCRIPTION
 *  Checks to see if MUX flow control is on or off.
 * PARAMETERS
 *  MuxId       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BOOL RfIsMuxFlowOn(U8 MuxId)
{
    if (MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW)
    {
        return TRUE;
    }
    else
    {
        if (MUX(MuxId).flags & MUX_FLAG_FCOFF)
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfIsDlcFlowOn
 * DESCRIPTION
 *  Checks to see if DLC flow control is on or off.
 * PARAMETERS
 *  MuxId       [IN]        
 *  DlcId       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BOOL RfIsDlcFlowOn(U8 MuxId, U8 DlcId)
{
    if (DLC(MuxId, DlcId).flags & DLC_FLAG_MSC_RCVD)
    {

        if (MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW)
        {
            if (DLC(MuxId, DlcId).channel->txCredit)
            {
                return TRUE;
            }
            else
            {
                bt_trace(TRACE_GROUP_1, RFISDLCFLOWONTXCREIDT0_ON_DLCIxD, DLC(MuxId, DlcId).channel->dlci);
                return FALSE;
            }
        }
        else
        {
            if (DLC(MuxId, DlcId).flags & DLC_FLAG_FCOFF)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }

    }
    else
    {
        return FALSE;
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfMUXCheckNextTxQueue
 * DESCRIPTION
 *  Sends data on the next queue in the priority list with data to
 *  send and flow control on.
 * PARAMETERS
 *  MuxId       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfMUXCheckNextTxQueue(U8 MuxId)
{
    BtPacket *packet;
    RF_DLC *startNode = MUX(MuxId).cDlc;
    BtStatus status;

    while (IsListEmpty(&(MUX(MuxId).cDlc->channel->txQueue)) || !RfIsDlcFlowOn(MuxId, MUX(MuxId).cDlc->channel->dlcId))
    {

        if (IsListEmpty(&(MUX(MuxId).cDlc->channel->txQueue)))
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_CHECK_NEXT_TXQUEUE_EMPTY_DLCI, MUX(MuxId).cDlc->channel->dlci);
        }
        if (!RfIsDlcFlowOn(MuxId, MUX(MuxId).cDlc->channel->dlcId))
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_CHECK_NEXT_TXQUEUE_FLOWOFF_DLCI, MUX(MuxId).cDlc->channel->dlci);
        }

        /* No data to send on the current DLC */
        MUX(MuxId).cDlc = (RF_DLC*) GetNextNode(&(MUX(MuxId).cDlc->node));
        if (MUX(MuxId).cDlc == (RF_DLC*) & (MUX(MuxId).priorityList))
        {
            /* At the end of the list, go to the beginning */
            MUX(MuxId).cDlc = (RF_DLC*) GetNextNode(&(MUX(MuxId).cDlc->node));
        }

        if (MUX(MuxId).cDlc == startNode)
        {
            /* We've gone completely through the list once */
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_CLEAR_MUX_FLAG_XMIT1);
            MUX(MuxId).flags &= ~MUX_FLAG_XMIT;
            return;
        }
    }

    /* A packet is available and flow is on, send packet from the TX queue */
    if ((MUX(MuxId).cDlc->channel->state == DLC_STATE_CONNECTED) && !(MUX(MuxId).cDlc->flags & DLC_FLAG_MSC_PENDING))
    {

        U8 pkt_num = MUX(MuxId).packetsInTransit;
        U8 dlci_value = MUX(MuxId).cDlc->channel->dlci;

        packet = (BtPacket*) RemoveHeadList(&(MUX(MuxId).cDlc->channel->txQueue));
        if (packet->priority_type != BTP_FLAG_HIGH_PRIPORTY)
        {
            packet->priority_type = BTP_FLAG_LOW_PRIPORTY;
        }
        if ((status = RfSendDataInternal(
                        MuxId,
                        MUX(MuxId).cDlc->channel->dlci,
                        packet,
                        packet->priority_type)) == BT_STATUS_PENDING)
        {
            (MUX(MuxId).packetsInTransit)++;

            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_PKT_INTRANSMIT_DLCI, pkt_num, dlci_value);
            if (MUX(MuxId).flags & MUX_FLAG_CREDIT_FLOW)
            {
                /* Decrement the flow control */
                if (MUX(MuxId).cDlc->channel->txCredit == 0)
                {
                    Assert(0);
                }
                if (packet->dataLen > 0)
                {
                    MUX(MuxId).cDlc->channel->txCredit--;
                }
                bt_trace(BT_TRACE_G1_PROTOCOL, 
                    BT_RFCOMMM_SENDDATA_INTERNAL_DLCI_TXCREDIT,
                    dlci_value,
                    MUX(MuxId).cDlc->channel->txCredit);
            }
        }
        else
        {
            /* Return to queue */
            InsertHeadList(&(MUX(MuxId).cDlc->channel->txQueue), &(packet->node));
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_CLEAR_MUX_FLAG_XMIT2, status, dlci_value);
            MUX(MuxId).flags &= ~MUX_FLAG_XMIT;

            // status = 0x17(BT_STATUS_NO_CONNECTION), This is due to receive DISC CONNECT EVENT
            // status = 0x01(BT_STATUS_FAILED), may be described as follows:
            // a. In L2CAP_SendData(): Not vaild channel, MUX(MUXId).l2ChannelId is wrong
            // b. In L2CAP_SendData(): Not in L2C_OPEN state
            // c. In L2CAP_SendData(): set private flags for Packet flag (Expected are only BTP_FLAG_TAIL and BTP_FLAG_INUSE)
            // d. In L2CAP_Send(): RemoteCid > 0x0002 and packlen > outMTU   
            //Assert(0);                    
            return;
        }

        MUX(MuxId).numTxPackets--;
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_TX_NUMBER_MINUS, MUX(MuxId).numTxPackets);
    }
    else
    {
        /* A packet could not be sent */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_CLEAR_MUX_FLAG_XMIT3);
        MUX(MuxId).flags &= ~MUX_FLAG_XMIT;
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfMUXCheckTxQueues
 * DESCRIPTION
 *  Sends data on the first queue in the priority list with data to
 *  send and flow control on.
 * PARAMETERS
 *  MuxId       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void RfMUXCheckTxQueues(U8 MuxId)
{
    if (RfIsMuxFlowOn(MuxId))
    {
        U8 pktinTransit_num = MUX(MuxId).packetsInTransit;

        if (!(MUX(MuxId).flags & MUX_FLAG_XMIT))
        {
            /* Not already transmitting */
            MUX(MuxId).flags |= MUX_FLAG_XMIT;
            Assert(!IsListEmpty(&(MUX(MuxId).priorityList)));
            MUX(MuxId).cDlc = (RF_DLC*) GetHeadList(&(MUX(MuxId).priorityList));
        }

        bt_trace(BT_TRACE_G1_PROTOCOL, BT_RFCOMMM_CHECKTXQUEUE, pktinTransit_num);
        while ((MUX(MuxId).packetsInTransit < RF_MAX_PACKETS_IN_TRANSIT) && (MUX(MuxId).flags & MUX_FLAG_XMIT))
        {
            RfMUXCheckNextTxQueue(MuxId);
        }
    }
}

#if !defined (__BT_2_1_SIMPLE_PAIRING__)


/*****************************************************************************
 * FUNCTION
 *  RfFindRemDevMUXID
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 RfFindRemDevMUXID(BtRemoteDevice *remDev)
{
    int i = 0;

    for (i = 0; i < NUM_BT_DEVICES; i++)
    {
        /* Search for an open slot */
        if (MUX(i).flags & MUX_FLAG_ALLOCATED)
        {
            if (remDev == MUX(i).remDev)
            {
                return i;
            }
        }
    }
    return i;
}
#endif /* !defined (__BT_2_1_SIMPLE_PAIRING__) */ 

#ifdef __GEMINI__


/*****************************************************************************
 * FUNCTION
 *  Set_DualSim_Uart_Setting
 * DESCRIPTION
 *  
 * PARAMETERS
 *  value       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void Set_DualSim_Uart_Setting(U8 value)
{
    RFC(dual_sim_uart_setting) = value;
}


/*****************************************************************************
 * FUNCTION
 *  Get_DualSim_Uart_Setting
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 Get_DualSim_Uart_Setting()
{
    return RFC(dual_sim_uart_setting);
}


/*****************************************************************************
 * FUNCTION
 *  Get_DualSim_Uart_Default_Owner
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
module_type Get_DualSim_Uart_Default_Owner()
{
    if (RFC(dual_sim_uart_setting) == 0)
    {
        return MOD_ATCI;
    }
    else
    {
        return MOD_ATCI_2;
    }
}
#endif /* __GEMINI__ */ 

#endif /* RFCOMM_PROTOCOL == XA_ENABLED */

