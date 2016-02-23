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
 *     $Workfile:l2cap_utl.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains utility functions used by L2CAP.
 *
 * Created:
 *     July 15, 1999
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
#include "btrom.h"
#include "sys/l2cap_i.h"
#include "sys/l2capxp.h"
#include "sys/mei.h"
#include "bt_devices_list.h"
#include "btaccctrl.h"
#include "bt_feature.h"
#include "a2mp_ctl.h"

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
static const U8 L2CAP_BasicFEC[] = {4, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */

/****************************************************************************
 *
 * Internal Function prototypes
 *
 ****************************************************************************/
static void L2CAP_TimerFire(EvmTimer *Timer);
static void L2CAP_DiscAclLink(EvmTimer *Timer);
static L2CAP_Channel *AllocChannel(const L2capPsm *Protocol);

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
static U16 L2Cap_StartCrc(U16 RemoteCid, U16 Length);
#endif 

/****************************************************************************
 *
 * external Functions
 *
 ****************************************************************************/
extern void L2Cap_ReturnflowRxBuffer(L2CAP_Channel *Channel);

extern DEVICE_MODEL BTBMGetDeviceModel(BD_ADDR *addr);
extern BOOL btbmCheckBondingStateWhenDisconnected(U8 *addr1);
extern U16 L2Cap_EnhancedGetNextSrejRxMissingRejectTable(L2CAP_Channel *Channel, U8 *index);
extern BOOL L2Cap_EnhancedRemoveFromTxRejectTable(L2CAP_Channel *Channel, U16 ReqSeq);
extern BOOL L2Cap_EnhancedVerifySrejAllSentInRxMissingRejectTable(L2CAP_Channel *Channel);
extern void L2Cap_EnhancedRetransmissionSendAck(L2CAP_Channel *Channel, U8 sendFlag);
extern void L2Cap_EnhancedRetransmissionPassToTxFBit(L2CAP_Channel *Channel, U8 f_control, U16 ReqSeq);
extern BOOL BTCheckPktServicedTimeLong(U32 int_time, U32 time_value);

U16 L2Cap_GetSequenceWrapValue(L2CAP_Channel *Channel);
U16 L2Cap_GetUnAckNumber(L2CAP_Channel *Channel, U16 wrapValue);

void L2Cap_StartRegularCreateAMPChannelTimeoutHandler(EvmTimer *Timer);
void L2Cap_StartRegularCreateAMPChannelTimer(L2CAP_Channel *Channel, U32 timeout);
void L2Cap_StartRegularMoveAMPChannelTimeoutHandler(EvmTimer *Timer);
void L2Cap_StartRegularMoveAMPChannelTimer(L2CAP_Channel *Channel, U32 timeout);
void BTTest_L2DisconnectAMPCreatedChannel(BtRemoteDevice *link);
void BTTest_L2DisconnectAMPChannel(void);

extern void A2MP_DisconnectAMPChannel(BtRemoteDevice *link, U16 l2cap_id);


#define __Dlight_TEST_TX__

/*****************************************************************************
 * FUNCTION
 *  L2CAP_Init
 * DESCRIPTION
 *  Called when the stack is initializing to init the L2CAP module.
 * PARAMETERS
 *  void
 * RETURNS
 *  BT_STATUS_SUCCESS
 *****************************************************************************/
BtStatus L2CAP_Init(void)
{
    I16 i;

    bt_trace(TRACE_FUNC, FUNC_L2CAP_INIT);
    L2C(highPriorityNo) = 0;
    L2C(lowPriorityNo) = 0;
    L2C(lowPriorityInBB) = 0;
    L2C(heavyTrafficSource) = L2C_HEAVY_TRAFFIC_NOT_EXIST;
    /* Init protocol slots */
    for (i = 0; i < L2CAP_NUM_PROTOCOLS; i++)
    {
        L2C(protocols)[i] = 0;
    }

    /* Init channel pool */
    for (i = 0; i < L2CAP_NUM_CHANNELS; i++)
    {
        L2C(channels)[i].state = L2C_CLOSED;
        L2C(channels)[i].localCid = L2CID_INVALID;
    }

#if L2CAP_NUM_GROUPS > 0
    /* Init groups */
    for (i = 0; i < L2CAP_NUM_GROUPS; i++)
    {
        L2C(groups)[i] = 0;
    }
#endif /* L2CAP_NUM_GROUPS > 0 */ 

    /* Init Signal Packet Pool */
    OS_Report("l2cap inipkt num:%d", L2CAP_NUM_SIGNAL_PACKETS);
    InitializeListHead(&L2C(signalPackets));
    for (i = 0; i < L2CAP_NUM_SIGNAL_PACKETS + L2CAP_SIGNAL_DEPTH; i++)
    {
        InsertTailList(&L2C(signalPackets), &L2C(_sigPacket)[i].packet.node);
    }

    /*
     * Init Broadcast 'Link'. It only sends Piconet type broadcasts.
     */
    L2C(broadcastLink).state = BRXS_BROADCAST;
    L2C(broadcastLink).hciHandle = PICONET_BC_HANDLE;
    InitializeListHead(&L2C(broadcastLink).txHighPriorityQueue);
    InitializeListHead(&L2C(broadcastLink).txLowPriorityQueue);

    L2C(nextIdent) = 1;
    L2C(flags) = L2CCF_BROADCASTS_ENABLED;

#ifdef __BT_4_0_BLE__
    OS_MemSet(&L2C(fixCid)[0], 0x0, sizeof(L2capFixCid) * L2CAP_FIX_CID_NO);
#endif
#ifdef __BT_USE_CUST_DATA__
    L2C(hs_enabled) = (U32)btGetCustvalue(CUSTID_HIGH_SPEED_SUPPORT);
#else
    L2C(hs_enabled) = 0;
#endif
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_Deinit
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void L2CAP_Deinit(void)
{
    I16 i;

    /* Verify that no packets are outstanding */
    for (i = 0; i < L2CAP_NUM_SIGNAL_PACKETS + L2CAP_SIGNAL_DEPTH; i++)
    {
        L2Cap_GetSysPkt(TRUE);
    }

    return;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_GetValidChannel
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Cid     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
L2CAP_Channel *L2Cap_GetValidChannel(U16 Cid)
{
    L2CAP_Channel *ch;

    /* Check that the CID is in range */
    if (Cid < BASE_DYNAMIC_CID || Cid > LAST_DYNAMIC_CID)
    {
        return 0;
    }

    ch = &L2C(channels)[CID_TO_INDEX(Cid)];

    /* Check that the channel is in use */
    if ((ch->localCid == L2CID_INVALID) || (ch->state == L2C_CLOSED))
    {
        return 0;
    }

    Assert(ch->localCid == Cid);
    return ch;
}



/*****************************************************************************
 * FUNCTION
 *  L2Cap_Get AMP Move Channel ID
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Cid     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
L2CAP_Channel *L2Cap_GetAMPChannelId(BtRemoteDevice *link, U16 Cid)
{
    L2CAP_Channel *ch;
    U8 i=0;
    
    /* Check that the CID is in range */
    if (Cid < BASE_DYNAMIC_CID || Cid > LAST_DYNAMIC_CID)
    {
        return 0;
    }
    for(i=0; i< L2CAP_NUM_CHANNELS; i++)
    {
        
        ch = &L2C(channels)[i];
        /* Check that the channel is in use */
        if ((ch->link == link) && (ch->state != L2C_CLOSED) &&
            (ch->remoteCid == Cid))
        {
            return ch;
        }
    }

    return 0;
}



L2CAP_Channel *L2Cap_GetValidChannelFixChannelCid03(BtRemoteDevice *link)
{
	U8 i=0;
    L2CAP_Channel *ch;

    for (i = 0; i < L2CAP_NUM_CHANNELS; i++)
    {
        if ((L2C(channels)[i].remoteCid == 0x0003) &&
			(L2C(channels)[i].link == link) &&
			(L2C(channels)[i].state == L2C_OPEN))
        {
			ch = &L2C(channels)[i];
			return ch;
    	}
	}
 
	return 0;
}

#if L2CAP_NUM_GROUPS > 0


/*****************************************************************************
 * FUNCTION
 *  L2Cap_GetValidGroup
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Cid     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
L2capGroup *L2Cap_GetValidGroup(U16 Cid)
{
    L2capGroup *group;

    /* Check that the CID is in range */
    if (Cid < BASE_GROUP_CID || Cid > LAST_GROUP_CID)
    {
        return 0;
    }

    group = L2C(groups)[GROUP_CID_TO_INDEX(Cid)];

    /* Check that the Group is registered */
    if ((group == 0) || (group->cid == L2CID_INVALID))
    {
        return 0;
    }

    return group;
}
#endif /* L2CAP_NUM_GROUPS > 0 */ 

#if (XA_ERROR_CHECK == XA_ENABLED) || (XA_DEBUG == XA_ENABLED)


/*****************************************************************************
 * FUNCTION
 *  L2Cap_IsValidProtocol
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Protocol        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL L2Cap_IsValidProtocol(const L2capPsm *Protocol)
{
    I8 p;

    for (p = 0; p < L2CAP_NUM_PROTOCOLS; p++)
    {
        if (L2C(protocols)[p] == Protocol)
        {
            return TRUE;
        }
    }

    return FALSE;
}
#endif /* (XA_ERROR_CHECK == XA_ENABLED) || (XA_DEBUG == XA_ENABLED) */



/*****************************************************************************
 * FUNCTION
 *  L2Cap_FindChannelByLocalIdent
 * DESCRIPTION
 *  Locate a channel based on its local signalling identifier value.
 * PARAMETERS
 *  Ident       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
L2CAP_Channel *L2Cap_FindChannelByLocalIdent(U8 Ident)
{
    I16 i;

    for (i = 0; i < L2CAP_NUM_CHANNELS; i++)
    {
        if (L2C(channels)[i].localCid == L2CID_INVALID)
        {
            continue;
        }

        if (L2C(channels)[i].localIdent == Ident)
        {
            return L2C(channels) + i;
        }
    }
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_FindChannelByRemoteCid
 * DESCRIPTION
 *  Locate a channel based on its local signalling identifier value.
 * PARAMETERS
 *  Ident       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
L2CAP_Channel *L2Cap_FindChannelByRemoteCid(U16 RemoteCid, U16 HciHandle)
{
    I16 i;

    for (i = 0; i < L2CAP_NUM_CHANNELS; i++)
    {
        if (L2C(channels)[i].localCid == L2CID_INVALID || 0 == L2C(channels)[i].link)
        {
            continue;
        }

        if (L2C(channels)[i].remoteCid == RemoteCid)
        {
            // the same remote cid and Hci handl 
            if( 0 != L2C(channels)[i].link && HciHandle == L2C(channels)[i].link->hciHandle ){
                OS_Report("l2cap findloop link:0x%x hcihandle:0x%x", L2C(channels)[i].link, L2C(channels)[i].link->hciHandle);                
                return L2C(channels) + i;
            }
        }
    }
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_CheckForDupilicateRequest
 * DESCRIPTION
 *  Compares the current request to the last request received from the
 *  given device. Returns TRUE if this request is the same as the last.
 * PARAMETERS
 *  Ident       [IN]        
 *  Link        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
L2CAP_Channel *L2Cap_CheckForDupilicateRequest(U8 Ident, BtRemoteDevice *Link)
{
    I16 i;

    for (i = 0; i < L2CAP_NUM_CHANNELS; i++)
    {

        if (L2C(channels)[i].localCid == L2CID_INVALID)
        {
            continue;
        }

        if ((L2C(channels)[i].remoteIdent == Ident) && (L2C(channels)[i].link == Link))
        {
            return L2C(channels) + i;
        }
    }
    return 0;

}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_AllocLinkChannel
 * DESCRIPTION
 *  Allocates a channel and associates it with a link.
 * PARAMETERS
 *  Psm         [IN]        
 *  Link        [IN]        
 *  Error       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
L2CAP_Channel *L2Cap_AllocLinkChannel(const L2capPsm *Psm, BtRemoteDevice *Link, BtStatus *Error)
{
    L2CAP_Channel *new_channel;

    if (Link && Link->state == BDS_CONNECTED)
    {

        new_channel = AllocChannel(Psm);
        if (new_channel)
        {
            Link->refCount++;
            new_channel->link = Link;
            EVM_ResetTimer(&Link->discTimer);
            new_channel->link->discTimeoutLevel = BT_L2CAP_DISCONNECT_TIMEOUT_500MS;
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
            new_channel->infoReqDone = 1;
#endif
            *Error = BT_STATUS_SUCCESS;
            return new_channel;
        }
        else
        {
            *Error = BT_STATUS_NO_RESOURCES;
        }
    }
    else
    {
        *Error = BT_STATUS_NO_CONNECTION;
    }

    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_AllocPsmChannel
 * DESCRIPTION
 *  Allocates a channel for the given PSM.
 *  Search the list of registered protocols looking for a matching PSM
 * PARAMETERS
 *  Psm         [IN]        
 *  Reason      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
L2CAP_Channel *L2Cap_AllocPsmChannel(U16 Psm, U16 *Reason)
{
    I8 p;

    if (BTAccCtrlCheckPSMPolicy(Psm) == FALSE)
    {
        bt_trace(TRACE_GROUP_1, PSM_NO_ALLOW);
        *Reason = L2CONN_REJECT_NO_RESOURCES;
        return 0;
    }
    bt_trace(TRACE_GROUP_1, PSM_ALLOW);

    for (p = 0; p < L2CAP_NUM_PROTOCOLS; p++)
    {

        if (L2C(protocols)[p] && (L2C(protocols)[p]->psm == Psm))
        {
            /*
             * Found the PSM, now find a free channel. If the Alloc fails,
             * we already set the appropriate error code (which should be
             * ignored if were successful).
             */
            *Reason = L2CONN_REJECT_NO_RESOURCES;
            return AllocChannel(L2C(protocols)[p]);
        }
    }

    *Reason = L2CONN_REJECT_PSM_NOT_SUPPORTED;

    OS_Report("l2cap: cannot find psm:0x%x", Psm);
    return 0;
}

void L2CAP_DumpChannel(){
    U32 i;
    L2CAP_Channel *chnl = NULL;
    Report(("dump channel status"));
    for (i = 0; i < L2CAP_NUM_CHANNELS; i++)
    {
        chnl = &L2C(channels)[i];
        OS_Report("l2cap: idx:%d cid:%d rcid:%d state:%d psm:0x%x" , i, chnl->localCid, chnl->remoteCid, chnl->state, (chnl->psmInfo==0?0:chnl->psmInfo->psm ));
    }    
}

/*****************************************************************************
 * FUNCTION
 *  AllocChannel
 * DESCRIPTION
 *  Find a free channel and return it.
 * PARAMETERS
 *  Protocol        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static L2CAP_Channel *AllocChannel(const L2capPsm *Protocol)
{
    I16 i;

    Report(("Dlight dump channel status"));
    for (i = 0; i < L2CAP_NUM_CHANNELS; i++)
    {
        bt_trace(TRACE_FUNC, L2CAP_CHANNEL_DUMP_INFO,i, L2C(channels)[i].localCid, L2C(channels)[i].remoteCid, L2C(channels)[i].state);
    }

    for (i = 0; i < L2CAP_NUM_CHANNELS; i++)
    {

        if (L2C(channels)[i].localCid == L2CID_INVALID)
        {

            Assert(L2C(channels)[i].state == L2C_CLOSED);

            OS_MemSet((U8*) (L2C(channels) + i), 0, (U32) sizeof(L2CAP_Channel));

            L2C(channels)[i].flags = L2CHF_AUTO_CONFIG;
            L2C(channels)[i].psmInfo = Protocol;
            L2C(channels)[i].localCid = BASE_DYNAMIC_CID + (U16) i;
            L2C(channels)[i].remoteCid = L2CID_INVALID;
            L2C(channels)[i].outMtu = L2CAP_DEFAULT_MTU;
            L2C(channels)[i].configExtendedFlowSpec = 0;
            L2C(channels)[i].connectedEventReported = 0;

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
            L2C(channels)[i].freeSduBuffer = 0;
            L2C(channels)[i].rxSduBuffer = 0;
            L2C(channels)[i].rxWindowSize = 0;
            L2C(channels)[i].flowRxBuffer = 0;
            L2C(channels)[i].parsePDUOffset = L2C_READ_FRAME_STAGE_CONTROL_BYTE0;
            L2C(channels)[i].flowRxBuffer = 0;
            L2C(channels)[i].enhancedRetransRemoteBusy = 0;
            L2C(channels)[i].enhancedRetransRNRSent = 0;
            L2C(channels)[i].infoReqDone = 1;        
            L2C(channels)[i].lastRxStateAckSeq = 0;    
            L2C(channels)[i].txCountNormalSent = 0;
            L2C(channels)[i].outExtFlowSpec.identifier = 0x01;
            L2C(channels)[i].outExtFlowSpec.serviceType = 0x01;
            L2C(channels)[i].outExtFlowSpec.maxSduSize = L2CAP_DEFAULT_MTU;
            L2C(channels)[i].outExtFlowSpec.sduIntArrialTime = 0xFFFFFFFF;
            L2C(channels)[i].outExtFlowSpec.accessLatency = 0xFFFFFFFF;
            L2C(channels)[i].outExtFlowSpec.flushTimeout = 0xFFFFFFFF;
            L2C(channels)[i].inExtFlowSpec.identifier = 0x01;
            L2C(channels)[i].inExtFlowSpec.serviceType = 0x01;
            L2C(channels)[i].inExtFlowSpec.maxSduSize = L2CAP_DEFAULT_MTU;
            L2C(channels)[i].inExtFlowSpec.sduIntArrialTime = 0xFFFFFFFF;
            L2C(channels)[i].inExtFlowSpec.accessLatency = 0xFFFFFFFF;
            L2C(channels)[i].inExtFlowSpec.flushTimeout = 0xFFFFFFFF;

#endif
#ifdef __BT_3_0_HS__
            L2C(channels)[i].hciAMPHandler = 0;
            L2C(channels)[i].setupByCreateChannel = 0;
            L2C(channels)[i].txFlowAvailable = 0;
            L2C(channels)[i].rxFlowAvailable = 0;
#endif

            if (Protocol)
            {
                L2C(channels)[i].localMtu = Protocol->localMtu;
            }
            XASTAT_PeakInc(l2cap_num_channels, 1);
            return &L2C(channels)[i];
        }
    }
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_NotifyUserAndCloseChannel
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Status      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_NotifyUserAndCloseChannel(L2CAP_Channel *Channel, U16 Status)
{
    U8 preState = L2C_CLOSING;
    bt_trace(TRACE_FUNC, FUNC_L2CAP_NOTIFY_USER_AND_CLOSE_CHANNEL, Status, 0, Channel->flags, Channel->state);
    preState = Channel->state;
    Channel->state = L2C_CLOSING;
    // Status L2DISC_USER_REQUEST

    /* clean the txQueue of Channel */
    L2Cap_FlushChannelTransmits(Channel);

    /* Ensure that no channel owned packets are on the transmit queue. */
    {
        BtPacket *packet = (BtPacket*) GetHeadList(&Channel->link->txHighPriorityQueue);

        while (packet != (BtPacket*) & Channel->link->txHighPriorityQueue)
        {
            Assert(packet->llpContext != Channel->localCid);
            packet = (BtPacket*) GetNextNode(&packet->node);
        }
    }
    {
        BtPacket *packet = (BtPacket*) GetHeadList(&Channel->link->txLowPriorityQueue);

        while (packet != (BtPacket*) & Channel->link->txLowPriorityQueue)
        {
            Assert(packet->llpContext != Channel->localCid);
            packet = (BtPacket*) GetNextNode(&packet->node);
        }
    }
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
        L2Cap_ReturnSentPackets(Channel, BT_STATUS_NO_CONNECTION);
#endif

    if (Status == L2DISC_REQUEST_TIMEDOUT)
    {
      OS_Report("l2cap: NotifyUser preState:%d", preState);
      switch( preState){
        case L2C_W4_DISC_CNF:
            OS_Report("l2cap: ignore the disconnect response timeout!"); //  ALPS00263777
            break;
        case L2C_W4_CONN_CNF:
            OS_Report("l2cap: ignore the connection response timeout!"); //  ALPS00338849
            break;
        default:
      /*	        	
      if(!btbmCheckBondingStateWhenDisconnected(Channel->link->bdAddr.addr)){
        Channel->link->disconnectACLCheckCreatACLAlwaysForce = 1;
        MeDisconnectLink(Channel->link, 20);
      }
      */
      // ALPS00457191 headset (CHBV2) doesn't response l2cap-config-req during SCO is establishing
      OS_Report("l2cap: ignore the connection response timeout!"); // ALPS00457191. 
      break;
    }
    }

    if (Channel->l2capTxCount > 0)
    {
        /* Defer L2EVENT_DISCONNECTED indication until after packets complete. */
        OS_Report("channel icd:%d l2capTxcount:%d (check getpkt) %d", Channel->localCid, Channel->l2capTxCount,Status);
        Channel->result = Status;
        if( L2DISC_LINK_DISCON != Status ){
        return;
        }else{
            OS_Report("l2cap txCount not 0. error !!!"); // phy link has disconnected ! must notify upper layer
        }
    }
    Channel->state = L2C_CLOSED;
    Report(("Channel->flags:%d",Channel->flags));
    if ((Channel->flags & L2CHF_NOT_VISIBLE) == 0)
    {
        Report(("L2EVENT_DISCONNECTED"));
        L2Cap_NotifyPsm(Channel, L2EVENT_DISCONNECTED, Status);
    }
    L2Cap_FreeChannel(Channel);
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_FreeChannel
 * DESCRIPTION
 *  Return an allocated channel to the free pool.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_FreeChannel(L2CAP_Channel *Channel)
{
    BtRemoteDevice *link;
    bt_trace(
        TRACE_FUNC,
        L2CAP_L2CAP_FREECHANNELx04X_PSM_x04X_FLAGS_x04X,
        Channel->localCid,
        Channel->psmInfo->psm,
        Channel->flags);
    Channel->state = L2C_CLOSED;
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
    L2Cap_ReturnflowRxBuffer(Channel);
#endif

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
    if ((Channel->flags & (L2CHF_RTX_ACTIVE | L2CHF_ERTX_ACTIVE)) ||
        (Channel->flags2 & (L2CHF2_RETRANS_ACTIVE | L2CHF2_MONITOR_ACTIVE)))
    {
#else /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */
    if ((Channel->flags & (L2CHF_RTX_ACTIVE | L2CHF_ERTX_ACTIVE)))
    {
#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */
        EVM_CancelTimer(&Channel->timer);
    }
#ifdef __BT_3_0_HS__
    EVM_ResetTimer(&Channel->verifyAmpTimer);
    EVM_ResetTimer(&Channel->verifyMoveAmpTimer);
#endif
    /*
     * If we have a security request outstanding we will cleanup on
     * the access response callback.
     */
    if (Channel->flags & L2CHF_SEC_PENDING)
    {
        return;
    }

    Channel->localCid = L2CID_INVALID;
    Report(("link->refCount A:%d",Channel->link->refCount));

    if (0 != (link = Channel->link))
    {
        bt_trace(TRACE_GROUP_1, L2CAP_FREECHANNEL_LINK_REFCOUNT_DISCFLAG, link->refCount, link->discFlag);
        if((Channel->remoteCid != 0x0003))
        {
            Assert((signed)link->refCount > 0);
            link->refCount--;
        }

        Channel->link = 0;
        /* If everyone is done with the link then disconnect it. */
        if ((link->refCount == 0) && (link->discFlag))
        {
            L2Cap_DisconnectACLLink(link);
        }

    }
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_GetSysPkt
 * DESCRIPTION
 *  Signalling Packet Pool functions
 *  First 2 packets are reserved for High Priority signaling packet
 *  L2CAP_SIGNAL_DEPTH is 2
 *  Disconnect command is low Priority 
 *  Connect command is high Priority 
 * PARAMETERS
 *  Priority        [IN]        TRUE
 * RETURNS
 *  
 *****************************************************************************/
BtPacket *L2Cap_GetSysPkt(BOOL Priority)
{
    SignalPacket *pkt;
    I8 i = 0;

#if __BT_30_DEBUG__ == XA_ENABLED
    bt_trace(TRACE_FUNC, FUNC_L2CAP_GETSYSPKT, Priority);
#endif
    if (IsListEmpty(&L2C(signalPackets)))
    {
        OS_Report("l: getpkt empty syspkt Priority:%d", Priority);
        return 0;
    }

    if (Priority == FALSE)
    {
        pkt = (SignalPacket*) GetHeadList(&L2C(signalPackets));
        while (pkt != (SignalPacket*) & L2C(signalPackets))
        {
            if (++i > L2CAP_SIGNAL_DEPTH)
            {
                break;
            }
            pkt = (SignalPacket*) GetNextNode(&pkt->packet.node);
        }

        if (i <= L2CAP_SIGNAL_DEPTH)
        {
            OS_Report("l: getpkt no syspkt Priority:%d", Priority);
            return 0;
        }
    }
    pkt = (SignalPacket*) RemoveHeadList(&L2C(signalPackets));

    pkt->packet.data = pkt->workspace;
    pkt->packet.dataLen = pkt->packet.tailLen = pkt->packet.headerLen = 0;
    pkt->packet.flags = BTP_FLAG_LSYSTEM;

    return &pkt->packet;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_GetMtu
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Options         [IN]
 *  OptionsLen      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U16 L2Cap_GetMtu(U8 *Options, U16 OptionsLen)
{
    while (OptionsLen)
    {

        if ((*Options & ~CONFIG_OPTION_HINT) == L2CFG_OPTION_MTU)
        {
            return LEtoHost16(Options + 2);
        }

        OptionsLen -= Options[1] + 2;
        Options += Options[1] + 2;

        Assert(OptionsLen < 0x8000);
    }

    return 0;
}

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
/*
 * L2Cap_InitFlowControlSettings()
 */


/*****************************************************************************
 * FUNCTION
 *  L2Cap_InitFlowControlSettings
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_InitFlowControlSettings(L2CAP_Channel *Channel)
{
    U8 linkMode = (Channel->psmInfo->inLinkMode & (Channel->link->linkModes | L2MODE_BASIC));

    // ALPS01474851 - hogp and headset reconnection
    if(linkMode == L2MODE_UNKNOWN){
        Report(("warning ignore it. wrong linkMode:%d inLinkMode:0x%x",linkMode, Channel->psmInfo->inLinkMode));
        return;
    }
    Assert(linkMode != L2MODE_UNKNOWN);

    Report(("Channel->a2mpSupported:%d",Channel->a2mpSupported));
    Report(("linkMode:%d inLinkMode:0x%x",linkMode, Channel->psmInfo->inLinkMode));

    if(Channel->psmInfo->inLinkMode == L2MODE_BASIC || Channel->psmInfo->inLinkMode == L2MODE_UNKNOWN)
    {
        // this psm only support basic mode
        Channel->inLinkMode = LINK_MODE_BASIC;
    }
    else
    {
        Report(("Channel->psmInfo->op_state:%d link->linkmodes:0x%x",Channel->psmInfo->op_state, Channel->link->linkModes));
        switch (Channel->psmInfo->op_state)
        {
            case L2OPERATORSTATE_STATE1:
                if (linkMode & LINK_MODE_BASIC)
                {
                    Channel->inLinkMode = LINK_MODE_BASIC;
                }
                else if (linkMode & L2MODE_ENHANCEDMENT_RETRANSMISSION)
                {
                    Channel->inLinkMode = LINK_MODE_ENHANCED_RETRANSMISSION;
                }
                else if (linkMode & L2MODE_STREAMMING)
                {
                    Channel->inLinkMode = LINK_MODE_STREAMMING;
                }
                else
                {
                    Assert(0);
                }
                break;
            case L2OPERATORSTATE_STATE2:
                if (linkMode & L2MODE_ENHANCEDMENT_RETRANSMISSION)
                {
                    Channel->inLinkMode = LINK_MODE_ENHANCED_RETRANSMISSION;
                }
                else if (linkMode & L2MODE_STREAMMING)
                {
                    Channel->inLinkMode = LINK_MODE_STREAMMING;
                }
                else if (linkMode & LINK_MODE_BASIC)
                {
                    Channel->inLinkMode = LINK_MODE_BASIC;
                }
                else
                {
                    Assert(0);
                }
                break;
        
            default:
                /* Select channels initial link mode (to be negotiated). */
                if (linkMode & L2MODE_ENHANCEDMENT_RETRANSMISSION)
                {
                    Channel->inLinkMode = LINK_MODE_ENHANCED_RETRANSMISSION;
                }
                else if (linkMode & L2MODE_ENHANCEDMENT_RETRANSMISSION)
                {
                    Channel->inLinkMode = LINK_MODE_ENHANCED_RETRANSMISSION;
                }
                else if (linkMode & L2MODE_STREAMMING)
                {
                    Channel->inLinkMode = LINK_MODE_STREAMMING;
                }
                else if (linkMode & L2MODE_RETRANSMISSION)
                {
                    Channel->inLinkMode = LINK_MODE_RETRANSMISSION;
                }
                else if (linkMode & L2MODE_FLOW_CONTROL)
                {
                    Channel->inLinkMode = LINK_MODE_FLOW_CONTROL;
                }
                else
                {
                    Channel->inLinkMode = LINK_MODE_BASIC;
                }
                break;
        }

    }
    Report(("linkMode:%d",linkMode));

    if (linkMode != L2MODE_BASIC)
    {
        Channel->outMpu = L2CAP_MTU-3;
        Channel->localMpu = Channel->psmInfo->localMtu;
        Channel->transmitMax = 3;   /* DEFAULT */
        Channel->outFcsUsed = L2FCS_OPTION_FCS;       /* DEFAULT */
        Channel->inFcsUsed = L2FCS_OPTION_FCS;       /* DEFAULT */        
        Channel->outExtFlowSpec = Channel->psmInfo->l2capExtFlowSpec;
        Channel->inExtFlowSpec = Channel->psmInfo->l2capExtFlowSpec;
        /* use local value as the remote value */
        Channel->extRxWindow = 0x00;
        Channel->supportExtControl = 0;
        /* Calculate available buffer space in MPU segments (plus 1). */
        Channel->rxWindow = (U16) (Channel->psmInfo->inBufferSize / Channel->localMpu);
        Assert((Channel->rxWindow > 0) && (Channel->rxWindow <= 32));

    }
    Channel->LastConfigStateRejectCount = 0;
    Report(("A2MP Dlight a2:%d",Channel->LastConfigStateRejectCount));
    
#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
    Channel->reConfigState = FALSE;
    Channel->configLockStep = FALSE;
    Channel->lockstepRxPendingDone = FALSE;    
    Channel->lockstepNegoDone = FALSE;  
    Channel->lockstepTxPendingDone = FALSE;    
#endif
}

#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
void L2Cap_InitLockStepControlSettings(L2CAP_Channel *Channel)
{
    Channel->flags &= ~(L2CHF_IN_CONFIG_OK | L2CHF_OUT_CONFIG_OK);
    Channel->reConfigState = TRUE;
    Channel->configLockStep = TRUE;
    Channel->lockstepTxPendingDone = FALSE;
    Channel->lockstepRxPendingDone = FALSE;
    Channel->lockstepNegoDone = FALSE;
}
#endif

/*****************************************************************************
 * FUNCTION
 *  L2Cap_SetFlowControlOptions
 * DESCRIPTION
 *  Set the Config Response's Options from the Config request's Options
 *  if it is accept or pending, need to copy the response option to channel->workspace
 *  if it is unaccept, neeed to copy the acceptable parameter to channel->worksapce
 *  When checking each input options, need also check the previous option result is accept or unaccept
 *  if previous option result is unacceptable, cannot change it to pending !!!
 * PARAMETERS
 *  Channel         [IN]        
 *  Options         [IN]        
 *  OptionsLen      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U16 L2Cap_SetFlowControlOptions(L2CAP_Channel *Channel, const U8 *Options, U16 OptionsLen)
{
    U16 result = L2CFG_RESULT_ACCEPTED;
    L2capLinkMode tempLinkMode;
    Report(("OptionsLen:%d",OptionsLen));
    while (OptionsLen)
    {
        Report(("*Options:%02x",*Options));

        if ((*Options & ~CONFIG_OPTION_HINT) == L2CFG_OPTION_FLOW_CONTROL)
        {
            /* Setup config response option values in case of failure. */
            Report(("L2CFG_OPTION_FLOW_CONTROL"));

            OS_MemCopy(Channel->workspace, Options, 11);
            Channel->wsLen = 11;

            Channel->flags2 |= L2CHF2_FOUND_FC_OPTION;  /* Note that the options were received */

            if (Options[1] != 9)
            {
                /* Malformed option */
                return L2CFG_RESULT_REJECTED;
            }

            /* Convert protocol enumeration to a flag for comparison with API link modes. */
            tempLinkMode = Channel->psmInfo->outLinkMode;
            Report(("the templinkMode a:%d",tempLinkMode));
            Report(("the Channel->ampSuspend a:%d",Channel->ampSuspend));
            Report(("the Channel->a2mpSupported a:%d",Channel->a2mpSupported));
            Report(("the Options[2]:%d",Options[2]));
            Report(("the Channel->psmInfo->channelUsage a:%d",Channel->psmInfo->channelUsage));
            Report(("the op_state:%d", Channel->psmInfo->op_state));
            if( Channel->psmInfo->inLinkMode == L2MODE_BASIC || Channel->psmInfo->inLinkMode == L2MODE_UNKNOWN )
            {
                tempLinkMode = L2MODE_BASIC;
            }
            if (tempLinkMode & (1 << Options[2]))
            {
                /* Set the accepted link operating mode */
                Channel->outLinkMode = Options[2];
            }
            else if ((tempLinkMode == 0) && (Options[2] == 0x00))
            {
                /* Set the accepted link operating mode */
                Channel->outLinkMode = LINK_MODE_BASIC;
            }
            else
            {
                /* Link mode not acceptable, tell them what we would accept. */
                if (Channel->psmInfo->channelUsage == L2CAP_CHANNEL_GENERAL)
                {
                    /* Can accept the Basic mode > enhanced retransmission mode */
                    /* If config need to reconfig again, we shall use the state 1 mode as defined in L2CAP */
                    /* However, if we want to use the BASIC mode, the remote side may be state 2 device */
                    /* The first config we use the enhanced retransmission or streamming mode */
                    /* The remote side shall accept our config request,                                  */
                    /* so we will not handle if the basic mode is not accepted case */
                    Channel->workspace[2] = LINK_MODE_BASIC;
                }
                else /* Dedicated, only apply for enhanced retransmission and streaming */
                {
                    if(Channel->ampSuspend == 0)
                    {
                        Channel->workspace[2] = LINK_MODE_BASIC;
                    }
                    else
                    {
                        switch(Channel->psmInfo->op_state)
                        {
                            case L2OPERATORSTATE_STATE0:
                                Channel->workspace[2] = LINK_MODE_BASIC;
                                break;
                            case L2OPERATORSTATE_STATE2:
                                Channel->workspace[2] = LINK_MODE_BASIC;
                                Report(("A2MP Dlight a3:%d",Channel->LastConfigStateRejectCount));
                                return L2CFG_RESULT_FORCE_DISCONNECT;
                             case L2OPERATORSTATE_STATE1:
                                /* We must be enabled in such mode. */
                                if (Channel->psmInfo->outLinkMode & L2MODE_ENHANCEDMENT_RETRANSMISSION)
                                {
                                    Channel->workspace[2] = LINK_MODE_ENHANCED_RETRANSMISSION;
                                }
#if 0                                
                                else if (Channel->psmInfo->outLinkMode & L2MODE_STREAMMING)
                                {
                                    Channel->workspace[2] = LINK_MODE_STREAMMING;
                                }
#endif                                
                                else
                                {
                                    /* Try to force disconnect. No mode is accepted. */
                                    Channel->LastConfigStateRejectCount = L2RECONFIG_MAX_COUNT;
                                    Report(("A2MP Dlight a4:%d",Channel->LastConfigStateRejectCount));
                        
                                    Channel->workspace[2] = LINK_MODE_BASIC;
                                }
                                break;
                        }

                    }
                }
                Report(("A2MP Dlight a5:%d",Channel->LastConfigStateRejectCount));
                
                result = L2CFG_RESULT_UNACCEPTABLE;
            }
            /* Check the parms based on the link mode that we are/may be sending back. */
            if (Channel->workspace[2] != LINK_MODE_BASIC)
            {
                Report(("Channel->workspace[2]:%02x txWindow:%d", Channel->workspace[2], Options[3]));

                Channel->txWindow = Options[3];
                if(Channel->workspace[2] == LINK_MODE_ENHANCED_RETRANSMISSION)
                {
                    /* Enhanced retransmission Mode tx window size are 1 to 63 */
                    if ((Channel->txWindow < 1) || (Channel->txWindow > 63))
                    {
                        /* Invalid parameter value. */
                        Channel->workspace[3] = 1;
                        result = L2CFG_RESULT_UNACCEPTABLE;
                    }
                }
                else
                {
                    /* Retransmission Mode or others */
                    /* Retransmission Mode tx window size are 1 to 32 */
                    /* Stream mode doesn't need to check these parameters */
                    if( LINK_MODE_STREAMMING != Channel->workspace[2] ){
                    if ((Channel->txWindow < 1) || (Channel->txWindow > 32))
                    {
                        Report(("Channel->txWindow:%d wrong range ", Channel->txWindow ));
                        /* Invalid parameter value. */
                        Channel->workspace[3] = 1;
                        result = L2CFG_RESULT_UNACCEPTABLE;
                    }
                }
                }
                Channel->transmitMax = Options[4];
                Report(("transmitMax:%d",Channel->transmitMax));
                if(Channel->workspace[2] != LINK_MODE_ENHANCED_RETRANSMISSION)
                {
                    /* Stream mode doesn't need to check these parameters */
                    if( LINK_MODE_STREAMMING != Channel->workspace[2] ){
                    if ((Channel->transmitMax = Options[4]) < 1)
                    {
                        /* Invalid parameter value. */
                        Channel->workspace[4] = 1;
                        result = L2CFG_RESULT_UNACCEPTABLE;
                    }
                }
                }
                else
                {
                    if(Channel->transmitMax == 0x00)
                    {
                        /* Infinite transmitted */
                        Channel->transmitMax = 0xFFFF;
                    }
                }

                Channel->outMpu = LEtoHost16(Options + 9);
                if (Channel->outMpu == 0)
                {
                    /* Invalid parameter value, suggest 4 bytes. */
                    Report(("outMpu is %d", Channel->outMpu));
                    Channel->workspace[9] = 4;
                    result = L2CFG_RESULT_UNACCEPTABLE;
                }
#ifdef __BT_3_0_HS__
                if (Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
                {
                    if(Channel->hciAMPHandler == 0)
                    {
                        if(Channel->link->flushTimeOut == 0xFFFFFFFF)
                        {
                            /* No flush timeout on basic rate*/
                            Channel->retransTimeout = MINIMUM_ENHANDED_BDR_RETRANS_TIMEOUT_NO_FLUSH; /* 2 seconds */
                            Channel->monitorTimeout = MINIMUM_ENHANDED_BDR_MONITOR_TIMEOUT_NO_FLUSH; /* 12 seconds */
                        }
                        else
                        {
                            /* flush timeout on basic rate*/
                            /* 1 seconds or (flushTimeOut * 3)*/
                            Channel->retransTimeout = 
                                    max(MINIMUM_ENHANDED_BDR_RETRANS_TIMEOUT_FLUSH, 
                                        Channel->link->flushTimeOut*3); 

                            /* 12 seconds or (flushTimeOut * 6)*/                            
                            Channel->monitorTimeout = 
                                max(MINIMUM_ENHANDED_BDR_MONITOR_TIMEOUT_NO_FLUSH, 
                                    Channel->link->flushTimeOut*6);  
                            
                            /* Monitor timer shall larger than retransmission timer */

                            if(Channel->monitorTimeout < Channel->retransTimeout)
                               Channel->monitorTimeout = Channel->retransTimeout;
                            
                        }
                    }
                    else    /* AMP controller */
                    {
                        if(Channel->AMPInfo.flushTimeOut == 0xFFFFFFFF)
                        {
                            /* The value almost infinite */
                            Report(("AMP flush timeout infinite, set monitor, retrans to supervision. "));
                            Channel->monitorTimeout = Channel->AMPInfo.supervisionTimeout * 10;
                            Channel->retransTimeout = Channel->AMPInfo.supervisionTimeout * 10;
                        }
                        else
                        {
                            Channel->retransTimeout = LEtoHost16(Options + 5) + Channel->AMPInfo.flushTimeOut*3 + 500;
                            Channel->monitorTimeout = LEtoHost16(Options + 7) + Channel->AMPInfo.flushTimeOut*3 + 500;
                        }
                    }
                }
                else
                {
                    /* Use the setting value */
                    Channel->retransTimeout = LEtoHost16(Options + 5);
                    Channel->monitorTimeout = LEtoHost16(Options + 7);
                    if(Channel->workspace[2] == LINK_MODE_STREAMMING)
                    {
                        /* In Streamming Mode, the value must be 0x00 and ignore by the receiver */
                        Channel->retransTimeout = 0;
                        Channel->monitorTimeout = 0;
                    }
                }
#else
                Channel->retransTimeout = LEtoHost16(Options + 5);
                Channel->monitorTimeout = LEtoHost16(Options + 7);
                if(Channel->workspace[2] == LINK_MODE_STREAMMING)
                {
                    /* In Streamming Mode, the value must be 0x00 and ignore by the receiver */
                    Channel->retransTimeout = 0;
                    Channel->monitorTimeout = 0;
                }
#endif

            }

            if (result == L2CFG_RESULT_ACCEPTED)
            {
                Channel->wsLen = 0; /* Erase failure response */
                Channel->LastConfigStateRejectCount = 0;
                Report(("A2MP Dlight a6:%d",Channel->LastConfigStateRejectCount));
				Channel->configFlowSpec = 1;
            }
            else
            {
                Channel->LastConfigStateRejectCount++;
                Report(("A2MP Dlight a7:%d",Channel->LastConfigStateRejectCount));
                
                if(Channel->LastConfigStateRejectCount > L2RECONFIG_MAX_COUNT)
                {
                    /* Too many reconfig, try to disconnect. */
                    return L2CFG_RESULT_FORCE_DISCONNECT;
                }
            }

        }
        else if ((*Options & ~CONFIG_OPTION_HINT) == L2CFG_OPTION_FCS)
        {
#ifdef __BT_3_0_HS__        
            if(Options[2] == L2FCS_OPTION_FCS)
            {
                Channel->inFcsUsed = L2FCS_OPTION_FCS;
                /* Even we use the streamming mode to config the FCS to OFF */
            }
            else
            {
                Channel->inFcsUsed = L2FCS_OPTION_NO_FCS;
                /* Only when the previous request also not uses the FCS can change the FCS to NO FCS */
            }
#endif            
        }
        else if ((*Options & ~CONFIG_OPTION_HINT) == L2CFG_OPTION_EXT_FLOW_SPEC)
        {
#ifdef __BT_3_0_HS__
            Channel->inExtFlowSpec.identifier = Options[2];
            Channel->inExtFlowSpec.serviceType = Options[3];
            Report(("Channel->inExtFlowSpec.serviceType:%d",Channel->inExtFlowSpec.serviceType));
            Report(("Channel->inExtFlooutExtFlowSpecwSpec.serviceType:%d",Channel->outExtFlowSpec.serviceType));
            Channel->inExtFlowSpec.maxSduSize = LEtoHost16(Options + 4);
            Channel->inExtFlowSpec.sduIntArrialTime = LEtoHost32(Options + 6);
            Channel->inExtFlowSpec.accessLatency = LEtoHost32(Options + 10);
            Channel->inExtFlowSpec.flushTimeout = LEtoHost32(Options + 14);
            Channel->configExtendedFlowSpec = 1;
            if(Channel->reConfigState == TRUE)
            {
                Channel->configLockStep = TRUE;
                if( L2CFG_RESULT_ACCEPTED == result ){
                result = L2CFG_RESULT_PENDING;
                }else{
                    OS_Report("l2cap configresult:%d no change", result);
                }
                if(Channel->inExtFlowSpec.serviceType !=
                    Channel->outExtFlowSpec.serviceType)
                {
                    return L2CFG_RESULT_FORCE_DISCONNECT;
                }
            }
            if((Channel->inExtFlowSpec.serviceType != Channel->outExtFlowSpec.serviceType) &&
                (Channel->inExtFlowSpec.serviceType != L2QOS_OPTION_NO_TRAFFIC) &&
                (Channel->inExtFlowSpec.serviceType != L2QOS_OPTION_BEST_EFFORT))
            {
                Channel->inExtFlowSpec.serviceType = L2QOS_OPTION_BEST_EFFORT;
                result = L2CFG_RESULT_FLOW_SPEC_REJECT;
                OS_MemCopy(Channel->workspace, Options, 18);
                Channel->workspace[3] = L2QOS_OPTION_BEST_EFFORT;
                Channel->wsLen = 18;
            }
            if((result == L2CFG_RESULT_SUCCESS) || (result == L2CFG_RESULT_PENDING))
            {
                Channel->rxFlowAvailable = 1;
            }
#else
                return L2CFG_RESULT_REJECTED;
#endif
        }
        else if((*Options & ~CONFIG_OPTION_HINT) == L2CFG_OPTION_EXT_WIN_SIZE)
        {
            Report(("Support Ext Tx Windows Size"));
            Channel->extRxWindow = LEtoHost16(Options + 2);
            Channel->supportExtControl = 1;
        }
        OptionsLen -= Options[1] + 2;
        Options += Options[1] + 2;

        Assert(OptionsLen < 0x8000);
    }

#ifdef __BT_3_0_HS__
    if (((Channel->flags2 & L2CHF2_FOUND_FC_OPTION) == 0) && ((Channel->flags & L2CHF_MORE_CONFIG) == 0) &&
        (Channel->configLockStep == FALSE))
    {
        /*
            * We did not recieve any Flow Control options, and this is the
            * last (or only) configuration request. Run a basic (the default)
            * Flow Control option through the parser to see if it is supported.
            */
        L2Cap_SetFlowControlOptions(Channel, L2CAP_BasicFEC, sizeof(L2CAP_BasicFEC));
        return L2CFG_RESULT_ACCEPTED;
    }
#else
    if (((Channel->flags2 & L2CHF2_FOUND_FC_OPTION) == 0) && ((Channel->flags & L2CHF_MORE_CONFIG) == 0))
    {
        /*
         * We did not recieve any Flow Control options, and this is the
         * last (or only) configuration request. Run a basic (the default)
         * Flow Control option through the parser to see if it is supported.
         */
        L2Cap_SetFlowControlOptions(Channel, L2CAP_BasicFEC, sizeof(L2CAP_BasicFEC));
        return L2CFG_RESULT_ACCEPTED;
    }

#endif
    return result;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_GetNextSegment
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Segment     [IN]        
 *  Sdu         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL L2Cap_GetNextSegment(BtPacket *Segment, BtPacket *Sdu, U8 checkPacketTxComplete)
{
    I16 dlen = 0, hlen = 0, tlen = 0, i = 0;
    I16 dataLen, headerLen, tailLen, pduLen;
    L2CAP_Channel *channel;
    U16 segEnd = 0;
    U16 wrapValue = 0;
    
    Assert(Sdu && Segment);

    channel = &L2C(channels)[CID_TO_INDEX(Sdu->llpContext)];
    if(channel->state != L2C_OPEN)
        return FALSE;
    if (Sdu->segNum == 0)
    {
        /* Initialize the SDU for transmission. */
        Sdu->segStart = channel->txComposeSeq;
        i = (Sdu->dataLen + Sdu->headerLen + Sdu->tailLen);
        Sdu->segNum = (U16) ((i / channel->outMpu) + ((i % channel->outMpu) ? 1 : 0));
        if(Sdu->segNum >=2)
        {
            Report(("Sdu->dataLen:%d",Sdu->dataLen));
            Report(("channel->outMpu:%d",channel->outMpu));    
            Report(("Sdu->segStart:%d",Sdu->segStart));            
            Report(("SDU num:%d",Sdu->segNum));
            
        }
        /* Bump the segNum for zero length SDU's */
        if (Sdu->segNum == 0)
        {
            Sdu->segNum = 1;
        }
        channel->txComposeSeq = ((channel->txComposeSeq + (Sdu->segNum)) & (L2Cap_GetSequenceWrapValue(channel)));
#if __BT_30_DEBUG__ == XA_ENABLED
        Report(("channel->txComposeSeq:%d",channel->txComposeSeq));
#endif
        Assert((Sdu->segNum < 0x3f));
    }
    if((Sdu->segNum == 1) && (channel->sendSeq != Sdu->segStart))
    {
        return FALSE;
    }
    else
    {
        segEnd = ((Sdu->segStart+ Sdu->segNum) & L2Cap_GetSequenceWrapValue(channel));
        if(Sdu->segStart > segEnd)
        {
            /* wrap happens */
            /* SegEnd xxxxxxxx SegStart Shall return */
            if((channel->sendSeq >= segEnd) && (channel->sendSeq < Sdu->segStart))
                return FALSE;
            
        }
        else
        {
            /* Sdu->segStart <= segEnd */
            if(channel->sendSeq < Sdu->segStart)
                return FALSE;
            if(channel->sendSeq >= segEnd)
                return FALSE;
        }
    }
#if __BT_30_DEBUG__ == XA_ENABLED
    bt_trace(
        TRACE_GROUP_1,
        L2CAP_GETNEXTSEGMENTBT_DEBUGx02X_SDU_xX_SEGMENT_xD_OF_xD_NS__xD,
        channel->localCid,
        Sdu,
        ((channel->sendSeq - Sdu->segStart) & L2Cap_GetSequenceWrapValue(channel)) + 1,
        Sdu->segNum,
        channel->sendSeq);
#endif
    *Segment = *Sdu;

    dataLen = Sdu->dataLen;
    headerLen = Sdu->headerLen;
    tailLen = Sdu->tailLen;
    wrapValue = L2Cap_GetSequenceWrapValue(channel);
    for (i = 0; i <= (U16) ((channel->sendSeq - Sdu->segStart) & (wrapValue)); i++)
    {
        if ((dataLen) < channel->outMpu)
        {
            pduLen = (dataLen + headerLen + tailLen);
        }
        else
        {
            pduLen = channel->outMpu+ headerLen + tailLen;
        }

        hlen = min(pduLen, headerLen);
        dlen = min(pduLen - hlen, dataLen);
        tlen = min(pduLen - hlen - dlen, tailLen);

        if (i < (U16) ((channel->sendSeq - Sdu->segStart) & (wrapValue)))
        {
            /* update the header length, the last fragment will not update */
            headerLen -= hlen;
            dataLen -= dlen;
            tailLen -= tlen;
        }
    }
    Assert(i <= Sdu->segNum);
    if(checkPacketTxComplete == 1)
    {

        if ((((segEnd - channel->sendSeq) & (wrapValue)) == 1) &&
            (Sdu->remoteCid >= 0x0040))

        {
            channel->txCountNormalSent--;
        }
    }
    /*
     * Update segment pointers to skip over sent SDU data and 
     * set segment field lengths to a MPU sized buffer.
     */
    Segment->headerLen = (U8) hlen;
    Segment->data += (Sdu->dataLen - dataLen);
    Segment->dataLen = (U16) dlen;
    Segment->tail += (Sdu->tailLen - tailLen);
    Segment->tailLen = (U16) tlen;

    /* Cache the total SDU length in the segment ulpContext field. */
    Segment->ulpContext = (void*)(Sdu->dataLen + Sdu->headerLen + Sdu->tailLen);
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("Segment headerLen:%d,%d,%d",Segment->headerLen, Segment->dataLen, Segment->tailLen));
#endif
    Assert((Segment->headerLen + Segment->dataLen + Segment->tailLen) <= (channel->outMpu));
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_GetNextSegmentBySrej
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Segment     [IN]        
 *  Sdu         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL L2Cap_GetNextSegmentBySrej(BtPacket *Segment, BtPacket *Sdu, U16 *ReqSeq)
{
    I16 dlen = 0, hlen = 0, tlen = 0, i = 0;
    I16 dataLen, headerLen, tailLen, pduLen;
    L2CAP_Channel *channel;
    U16 segEnd = 0;
    U16 wrapValue = 0;
    
    Assert(Sdu && Segment);

    channel = &L2C(channels)[CID_TO_INDEX(Sdu->llpContext)];
    if(channel->state != L2C_OPEN)
        return FALSE;

    wrapValue = L2Cap_GetSequenceWrapValue(channel);
	if(channel->enhancedSrejListTxCount ==0)
	{
		/* No waiting TX SREJ I frame. */
		return FALSE;
	}
	*ReqSeq = channel->enhancedTxSrejFrame[0].ReqSeq;

    if((Sdu->segNum == 1) && (*ReqSeq != Sdu->segStart))
    {
        return FALSE;
    }
    else
    {
        segEnd = ((Sdu->segStart+ Sdu->segNum) & wrapValue);
        if(Sdu->segStart >= segEnd)
        {
            /* wrap happens */
            /* SegEnd xxxxxxxx SegStart Shall return */
            if((*ReqSeq >= segEnd) && (*ReqSeq < Sdu->segStart))
                return FALSE;
        }
        else
        {
            /* Sdu->segStart < segEnd */
            if(*ReqSeq < Sdu->segStart)
                return FALSE;
            if(*ReqSeq >= segEnd)
                return FALSE;
        }
    }

    bt_trace(
        TRACE_GROUP_1,
        L2CAP_GETNEXTSEGMENTBT_DEBUGx02X_SDU_xX_SEGMENT_xD_OF_xD_NS__xD,
        channel->localCid,
        Sdu,
        ((channel->sendSeq - Sdu->segStart) & L2Cap_GetSequenceWrapValue(channel)) + 1,
        Sdu->segNum,
        channel->sendSeq);
    *Segment = *Sdu;

    dataLen = Sdu->dataLen;
    headerLen = Sdu->headerLen;
    tailLen = Sdu->tailLen;
    for (i = 0; i <= (U16) ((*ReqSeq - Sdu->segStart) & (wrapValue)); i++)
    {
        Report(("dataLen:%d",dataLen));
        Report(("headerLen:%d",headerLen));
        Report(("tailLen:%d",tailLen));
        Report(("channel->outMpu:%d",channel->outMpu));

        if ((dataLen + headerLen + tailLen) < channel->outMpu)
        {
            pduLen = (dataLen + headerLen + tailLen);
        }
        else
        {
            pduLen = channel->outMpu;
        }

        hlen = min(pduLen, headerLen);
        dlen = min(pduLen - hlen, dataLen);
        tlen = min(pduLen - hlen - dlen, tailLen);

        if (i < (U16) ((*ReqSeq - Sdu->segStart) & (wrapValue)))
        {
            /* update the header length, the last fragment will not update */
            headerLen -= hlen;
            dataLen -= dlen;
            tailLen -= tlen;
        }
    }
    Assert(i <= Sdu->segNum);

    /*
     * Update segment pointers to skip over sent SDU data and 
     * set segment field lengths to a MPU sized buffer.
     */
    Segment->headerLen = (U8) hlen;
    Segment->data += (Sdu->dataLen - dataLen);
    Segment->dataLen = (U16) dlen;
    Segment->tail += (Sdu->tailLen - tailLen);
    Segment->tailLen = (U16) tlen;

    /* Cache the total SDU length in the segment ulpContext field. */
    Segment->ulpContext = (void*)(Sdu->dataLen + Sdu->headerLen + Sdu->tailLen);

    Assert((Segment->headerLen + Segment->dataLen + Segment->tailLen) <= (channel->outMpu));
    L2Cap_EnhancedRemoveFromTxRejectTable(channel, *ReqSeq);

    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_BuildIFrameHeader
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Segment     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_BuildIFrameHeader(BtPacket *Segment, U8 IframeBuildType)
{
    L2CAP_Channel *channel;
    I16 offset;
    U8 sar; /* SAR field shifted to bits 16:15 */
    U8 final_bit = 0x00;
	U8 final_bit_shall_set = 0;
    U16 fcs;
    U8 supportExtControl = 0;
    U16 wrapValue = 0x00;
    
    channel = &L2C(channels)[CID_TO_INDEX(Segment->llpContext)];
    supportExtControl = L2CAP_GetControlFieldType(channel);
    
    if(supportExtControl != L2CAP_CONTROL_FIELD_EXTENDED)
        offset = BT_PACKET_HEADER_LEN - Segment->headerLen - 2;
    else
        offset = BT_PACKET_HEADER_LEN - Segment->headerLen - 4;
    wrapValue = L2Cap_GetSequenceWrapValue(channel);

    /* Append the I-Frame header to the packet. */
    if (Segment->segStart == channel->sendSeq)
    {
        if (Segment->segNum > 1)
        {
            /* First segment in SDU, insert the SDU length. */
                if(supportExtControl != L2CAP_CONTROL_FIELD_EXTENDED)
                {
                sar = 0x40; /* Start of L2CAP SDU */
                }
                else
                {
                    sar = 0x01; /* Start of L2CAP SDU */
                }

                Segment->header[BT_PACKET_HEADER_LEN - Segment->headerLen -2] = (U8) ((U32) (Segment->ulpContext));
                Segment->header[BT_PACKET_HEADER_LEN - Segment->headerLen -1] = (U8) ((U32) (Segment->ulpContext) >> 8);
                Segment->headerLen += 2;
                offset -= 2;
        }
        else
        {
            sar = 0x00; /* Unsegmented L2CAP SDU */
        }
    }
    else
    {

        /* Continuation or last segment in SDU */
        if (((Segment->segStart + Segment->segNum - 1) & (L2Cap_GetSequenceWrapValue(channel))) == channel->sendSeq)
        {
            if(supportExtControl != L2CAP_CONTROL_FIELD_EXTENDED)
                sar = 0x80; /* End of L2CAP SDU */
            else
                sar = 0x02;
        }
        else
        {
            if(supportExtControl != L2CAP_CONTROL_FIELD_EXTENDED)
                sar = 0xC0; /* Continuation of L2CAP SDU */
            else
                sar = 0x03;
        }
    }

    /* Add the Control fields: N(S), N(R) (aka V(B)), SAR. */
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("E channel->enhancedNeedToSendFinal:%d",channel->enhancedNeedToSendFinal));    
#endif    
    if(channel->enhancedNeedToSendFinal == 0x01)
    {
        channel->enhancedNeedToSendFinal = 0;
		final_bit_shall_set = 1;
    }	
    switch(supportExtControl)
    {
        case L2CAP_CONTROL_FIELD_STANDARD:
            Segment->header[offset + 0] = (U8) ((channel->sendSeq << 1));
#if __BT_30_DEBUG__ == XA_ENABLED
            Report(("Dlight: I frame Send:%d",channel->sendSeq));
#endif
            Segment->header[offset + 1] = (U8) ((channel->buffSeq) | sar);
            Segment->headerLen += 2;
            break;
        case L2CAP_CONTROL_FIELD_ENHANCED:
            if(final_bit_shall_set !=0)
            {
                final_bit = 0x80;
                Segment->pollFinal &= (~L2CONTROLFRAMEWITHFINAL);
            }
            Segment->header[offset] = (U8) ((channel->sendSeq << 1) | final_bit);
#if __BT_30_DEBUG__ == XA_ENABLED
            Report(("Dlight: I frame Send:%d",channel->sendSeq));
#endif
            if(channel->inLinkMode == LINK_MODE_STREAMMING)
            {
                Segment->internalUsed = channel->sendSeq;
                Segment->header[offset + 1] = (U8) (sar);
                /* ReqSeq is 0x00 */
            }
            else
            {
                Segment->header[offset + 1] = (U8) ((channel->buffSeq) | sar);
            }
            Segment->headerLen += 2;
            break;
        case L2CAP_CONTROL_FIELD_EXTENDED:
            if(final_bit_shall_set !=0)
            {
                final_bit = 0x02;
                Segment->pollFinal &= (~L2CONTROLFRAMEWITHFINAL);
            }   
            if(channel->inLinkMode == LINK_MODE_STREAMMING)
            {
                Segment->internalUsed = channel->sendSeq;
                StoreLE16(Segment->header + offset, final_bit);
                /* ReqSeq is 0x00 */
            }
            else
            {
                StoreLE16(Segment->header + offset, ((channel->buffSeq << 2) | final_bit));

            }
            StoreLE16(Segment->header + offset + 2, channel->sendSeq << 2);
            Segment->header[offset + 2] |= sar;
#if __BT_30_DEBUG__ == XA_ENABLED
            Report(("Dlight: I frame Send:%d",channel->sendSeq));
#endif
            Segment->headerLen += 4;
            break;
    }
    channel->lastRxStateAckSeq = channel->buffSeq;

    if ((channel->flags2 & L2CHF2_NO_RETRANS_IN) && 
        (channel->supportExtControl == L2CAP_CONTROL_FIELD_STANDARD))
    {
        Assert(channel->inLinkMode == LINK_MODE_RETRANSMISSION);
        /* The application has requested that retransmissions be suspended. */
        Segment->header[offset + 0] |= 0x80;
    }
    switch(IframeBuildType)
    {
        case L2IFRAMEINITCREATE:
            channel->txIframeCount[channel->sendSeq] = 1;
            break;
        case L2IFRAMERETRANSMITTED:
            if (channel->txIframeCount[channel->sendSeq] < 0xFF)
            {
                /* If retransmitted count == 0xFF. It only happens in infinite retransmitted, 
                              it is not necessary to increase the tx Count*/
                channel->txIframeCount[channel->sendSeq]++;    
            }
            break;
    }

    /* Used for retransmission */
    /* When retransmission happens in HCI, but it is acked in L2CAP */
    /* The HCI layer will check that and will not send the packet */
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("The Seq:%d, retransmission number:%d",channel->sendSeq,channel->txIframeCount[channel->sendSeq]));
#endif
    Segment->ulpContext = (void *)&(channel->txIframeCount[channel->sendSeq]);
    /* Increment the send sequence number. */
    channel->sendSeq = (channel->sendSeq + 1) & 0x003f;
    if(channel->sendSeq == channel->txComposeSeq)
    {
        channel->verifyRetransmissionQueueState = 0;
#if __BT_30_DEBUG__ == XA_ENABLED
        Report(("Dlight set the verifyRetransmissionQueueState to 0"));
#endif
    }
    /* We're sending an updated N(R) so clear the Send-RR flag. */
    /* Only when the Poll is not set can remove the RR */
    if((channel->enhancedPollFinal & L2CONTROLFRAMEWITHPOLL) ==0)
        channel->flags2 &= ~L2CHF2_SEND_RR;

    /* Append FCS to I-Frame */
    if((channel->outFcsUsed != L2FCS_OPTION_NO_FCS) || (channel->inFcsUsed != L2FCS_OPTION_NO_FCS))
    {
        fcs = L2Cap_StartCrc(channel->remoteCid, (U16) (Segment->headerLen + Segment->dataLen + Segment->tailLen + 2));
        fcs = L2Cap_ComputeCrc(Segment->header + offset, Segment->headerLen, fcs);
        fcs = L2Cap_ComputeCrc(Segment->data, Segment->dataLen, fcs);
        fcs = L2Cap_ComputeCrc(Segment->tail, Segment->tailLen, fcs);
        StoreLE16(Segment->fcs, fcs);
        Segment->flags |= BTP_FLAG_FCS;
    }
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("supportExtControl:%d",supportExtControl));
    Report(("headerLen:%d,dataLen:%d, tailLen:%d",Segment->headerLen,Segment->dataLen, Segment->tailLen));    
    Report(("outMpu:%d,sar:%d",channel->outMpu,sar));        
#endif
}




/*****************************************************************************
 * FUNCTION
 *  L2Cap_BuildIFrameHeader
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Segment     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_BuildIFrameHeaderSrej(BtPacket *Segment, U16 ReqSeq)
{
    L2CAP_Channel *channel;
    I16 offset;
    U8 sar; /* SAR field shifted to bits 16:15 */
    U8 final_bit = 0x00;
	U8 final_bit_shall_set = 0;
    U16 fcs;
    U8 supportExtControl = 0;
    U16 wrapValue = 0x00;
    
    channel = &L2C(channels)[CID_TO_INDEX(Segment->llpContext)];
    supportExtControl = L2CAP_GetControlFieldType(channel);
    
    if(supportExtControl != L2CAP_CONTROL_FIELD_EXTENDED)
        offset = BT_PACKET_HEADER_LEN - Segment->headerLen - 2;
    else
        offset = BT_PACKET_HEADER_LEN - Segment->headerLen - 4;
    wrapValue = L2Cap_GetSequenceWrapValue(channel);
    Report(("channel->enhancedNeedToSendFinal:%d",channel->enhancedNeedToSendFinal));
    if(channel->enhancedNeedToSendFinal == 0x01)
    {
        channel->enhancedNeedToSendFinal = 0;
		final_bit_shall_set = 1;
    }	
    /* Append the I-Frame header to the packet. */
    if (Segment->segStart == ReqSeq)
    {
        if (Segment->segNum > 1)
        {
            /* First segment in SDU, insert the SDU length. */
                sar = 0x40; /* Start of L2CAP SDU */
                Segment->header[offset + 0] = (U8) ((U32) (Segment->ulpContext));
                Segment->header[offset + 1] = (U8) ((U32) (Segment->ulpContext) >> 8);
                Segment->headerLen += 2;
                offset -= 2;
        }
        else
        {
            sar = 0x00; /* Unsegmented L2CAP SDU */
        }
    }
    else
    {

        /* Continuation or last segment in SDU */
        if (((Segment->segStart + Segment->segNum - 1) & wrapValue) == ReqSeq)
        {
            sar = 0x80; /* End of L2CAP SDU */
        }
        else
        {
            sar = 0xC0; /* Continuation of L2CAP SDU */
        }
    }

    /* Add the Control fields: N(S), N(R) (aka V(B)), SAR. */
    switch(supportExtControl)
    {
        case L2CAP_CONTROL_FIELD_STANDARD:
            Segment->header[offset + 0] = (U8) ((ReqSeq << 1));
            Report(("Dlight: I frame Send a:%d",ReqSeq));
            Segment->header[offset + 1] = (U8) ((channel->buffSeq) | sar);
            Segment->headerLen += 2;
            break;
        case L2CAP_CONTROL_FIELD_ENHANCED:
            if(final_bit_shall_set !=0)
            {
                final_bit = 0x80;
                Segment->pollFinal &= (~L2CONTROLFRAMEWITHFINAL);
            }
            Segment->header[offset] = (U8) ((ReqSeq << 1) | final_bit);
            Report(("Dlight: I frame Send b:%d",ReqSeq));
            Segment->header[offset + 1] = (U8) ((channel->buffSeq) | sar);
            Segment->headerLen += 2;
            break;
        case L2CAP_CONTROL_FIELD_EXTENDED:
            if(final_bit_shall_set !=0)
            {
                final_bit = 0x02;
                Segment->pollFinal &= (~L2CONTROLFRAMEWITHFINAL);
            }   
            StoreLE16(Segment->header + offset, ((channel->buffSeq << 2) | final_bit));
            StoreLE16(Segment->header + offset + 2, ReqSeq << 2);
            Segment->header[offset + 3] |= sar;
            Report(("Dlight: I frame Send c:%d",ReqSeq));
            Segment->headerLen += 4;
            break;
    }

    if (channel->txIframeCount[ReqSeq] < 0xFF)
    {
        /* If retransmitted count == 0xFF. It only happens in infinite retransmitted, 
                      it is not necessary to increase the tx Count*/
        channel->txIframeCount[ReqSeq]++;    
    }

    /* Used for retransmission */
    /* When retransmission happens in HCI, but it is acked in L2CAP */
    /* The HCI layer will check that and will not send the packet */
    Report(("The Seq:%d, retransmission number:%d", ReqSeq, channel->txIframeCount[ReqSeq]));
    Segment->ulpContext = (void *)&(channel->txIframeCount[ReqSeq]);

    /* We're sending an updated N(R) so clear the Send-RR flag. */

    /* Append FCS to I-Frame */
    if((channel->outFcsUsed != L2FCS_OPTION_NO_FCS) || (channel->inFcsUsed != L2FCS_OPTION_NO_FCS))
    {
        fcs = L2Cap_StartCrc(channel->remoteCid, (U16) (Segment->headerLen + Segment->dataLen + Segment->tailLen + 2));
        fcs = L2Cap_ComputeCrc(Segment->header + offset, Segment->headerLen, fcs);
        fcs = L2Cap_ComputeCrc(Segment->data, Segment->dataLen, fcs);
        fcs = L2Cap_ComputeCrc(Segment->tail, Segment->tailLen, fcs);
        StoreLE16(Segment->fcs, fcs);
        Segment->flags |= BTP_FLAG_FCS;
    }
}



/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendSFrame
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_SendSFrame(L2CAP_Channel *Channel)
{
    BtPacket *frame;
    BtStatus status;
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("Dlihgt Channel Send S:%08x,%04x",Channel, Channel->localCid));
#endif
    Channel->flags &= ~L2CHF_SEND_REQUEST;
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("Channel in link mode:%02x",Channel->inLinkMode));
#endif
    if (0 != (frame = L2Cap_GetSysPkt(FALSE)))
    {
        /*
         * Put the empty frame on the TX Queue. The L2CAP_HciTxReady() 
         * function will fill in the S-Frame information.
         */
        frame->priority_type = BTP_FLAG_HIGH_PRIPORTY;
        status = L2Cap_SendSFrameData(Channel, frame, Channel->remoteCid);
#if __BT_30_DEBUG__ == XA_ENABLED
        {
            Report(("DEBUG: S frame status:%02x", status));
        }
#endif
        if (status == BT_STATUS_PENDING)
        {
            if ((Channel->flags2 & L2CHF2_SEND_REJECT) ||
                (Channel->flags2 & L2CHF2_SEND_RNR) ||
                (Channel->flags2 & L2CHF2_SEND_SREJ))
            {
                Channel->flags2 |= L2CHF2_REJECT_ACTIVE;
            }
            return;
        }

        /* Send failed, return the packet. */
        ReturnSysPkt(frame);
    }
    else
    {
        Report(("DEBUG: No S frame system packet"));
    }

    Channel->flags |= L2CHF_SEND_REQUEST;
    L2C(flags) |= L2CCF_NEED_SYSPACKET;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_BuildSFrame
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Packet      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtPacket *L2Cap_BuildSFrame(L2CAP_Channel *Channel, BtPacket *Packet)
{
    U16 fcs = 0;
    U8 type, nr = 0;
    U8 supportExtControl = 0;
    I16 offset;
    U8 poll_bit = 0x00;
    U8 final_bit = 0x00;
    
    Packet->headerLen = 0;
    
    supportExtControl = L2CAP_GetControlFieldType(Channel);
#ifdef __BT_3_0_HS__
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("Set channel f:%08x to %d",Channel, Channel->ampSuspend));
#endif
    if(Channel->ampSuspend == 1)
        return 0;
#endif

    if(supportExtControl != L2CAP_CONTROL_FIELD_EXTENDED)
        offset = BT_PACKET_HEADER_LEN - 4;
    else
        offset = BT_PACKET_HEADER_LEN - 6;

    /*
     * Need to send an RR or a Reject. We only need to send one,
     * but if both are flagged, the the Reject takes precidence. 
     */

    if (Channel->flags2 & L2CHF2_SEND_REJECT)
    {
        if(supportExtControl != L2CAP_CONTROL_FIELD_EXTENDED)
        {
            /* Set S = 1 + S Frame*/
            /* Request the remote side to send data again from ReqSeq */
            type = 0x05;    /* Reject */ 
        }
        else
        {
            type = 0x01; /* Reject */
        }
        nr = Channel->recvSeq;
    }
    else if (Channel->flags2 & L2CHF2_SEND_RR)
    {
        if(supportExtControl != L2CAP_CONTROL_FIELD_EXTENDED)
        {
            type = 0x01; /* RR */
        }
        else
        {
            /* Set S = 0 + S Frame*/
            type = 0x00; /* RR */
        }
        nr = Channel->buffSeq;
    }
    else if (Channel->flags2 & L2CHF2_SEND_RNR)
    {
        if(supportExtControl != L2CAP_CONTROL_FIELD_EXTENDED)
        {
            type = 0x09; /* RNR */
        }
        else
        {
            type = 0x02; /* RNR */
        }
        nr = Channel->buffSeq;
    } 
    else if (Channel->flags2 & L2CHF2_SEND_SREJ)
    {
        if(supportExtControl != L2CAP_CONTROL_FIELD_EXTENDED)
        {
            type = 0x0D; /* SREJ */
        }
        else
        {
            type = 0x03; /* SREJ */
        }
        nr = Channel->buffSeq;
    }    
    else
    {
        /* N(R) was already sent in an I-Frame. Return the system packet. */
#if __BT_30_DEBUG__ == XA_ENABLED
		Report(("l2data Build S frame(in I-Frame) ignore nr:%d lcid:%d", nr, Channel->localCid));
#endif

        Channel->l2capTxCount--;
        if (Packet->priority_type == BTP_FLAG_HIGH_PRIPORTY)
        {
            if (L2C(highPriorityNo) != 0)
            {
                L2C(highPriorityNo)--;
            }
        }     
        ReturnSysPkt(Packet);
        Report(("N(R) in I frame"));
        return 0;
    }
	
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("l2data Build S frame nr:%d lcid:%d", nr, Channel->localCid));
#endif

    Channel->lastRxStateAckSeq = nr;
    Channel->flags2 &= ~(L2CHF2_SEND_REJECT | L2CHF2_SEND_RR | L2CHF2_SEND_RNR | L2CHF2_SEND_SREJ );
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("G channel->enhancedNeedToSendFinal:%d",Channel->enhancedNeedToSendFinal));    
#endif
    if(Channel->enhancedNeedToSendFinal == 0x01)
    {
        Report(("Build S frame set the Final bit to 1"));
        Channel->enhancedPollFinal |= L2CONTROLFRAMEWITHFINAL;
        Channel->enhancedNeedToSendFinal = 0;
    }
    /* Build the S-Frame */
    switch(supportExtControl)
    {
        case L2CAP_CONTROL_FIELD_STANDARD:
            /* The Retransmission bits is always set to 0 when flow control mode. */
            if (Channel->flags2 & L2CHF2_NO_RETRANS_IN)
            {
                Assert(Channel->inLinkMode == LINK_MODE_RETRANSMISSION);
                /* The application has requested that retransmissions be suspended. */
                type |= 0x80;   /* Set Retransmission disable bit */
            }
            Packet->header[offset] = type;
            Packet->header[offset+1] = nr;
            Packet->headerLen = 0x02;
            break;
        case L2CAP_CONTROL_FIELD_ENHANCED:
            if((Channel->enhancedPollFinal & L2CONTROLFRAMEWITHFINAL) !=0)
            {   
                type |= 0x80;
            }
            if((Channel->enhancedPollFinal & L2CONTROLFRAMEWITHPOLL) !=0)
            {   
                type |= 0x10;
            }
            Channel->enhancedPollFinal = 0;
            Packet->header[offset] = type;
            Packet->header[offset+1] = nr;
            Packet->headerLen = 0x02;
            break;
        case L2CAP_CONTROL_FIELD_EXTENDED:
            if((Channel->enhancedPollFinal & L2CONTROLFRAMEWITHFINAL) !=0)
            {   
                final_bit = 0x02;
            }
            if((Channel->enhancedPollFinal & L2CONTROLFRAMEWITHPOLL) !=0)
            {   
                poll_bit = 0x04;
            }
            StoreLE16(Packet->header + offset, ((nr << 2) | final_bit | 0x01));
            StoreLE16(Packet->header + offset + 2, poll_bit | type);
            Channel->enhancedPollFinal = 0;
            Packet->headerLen = 0x04;
            break;
    }

    /* Append FCS to S-Frame */
    if((Channel->outFcsUsed != L2FCS_OPTION_NO_FCS) || (Channel->inFcsUsed != L2FCS_OPTION_NO_FCS))
    {    
        fcs = L2Cap_StartCrc(Channel->remoteCid, Packet->headerLen+2);
        fcs = L2Cap_ComputeCrc(Packet->header + offset, Packet->headerLen, fcs);
        Packet->header[offset + Packet->headerLen] = (U8) (fcs);
        Packet->header[offset + Packet->headerLen+1] = (U8) (fcs >> 8);
        Packet->headerLen += 2;
    }

    return Packet;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_BuildSFrame
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Packet      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtPacket *L2Cap_BuildSFrameSREJ(L2CAP_Channel *Channel, BtPacket *Packet)
{
    U16 fcs = 0;
    U8 type = 0;
    U8 supportExtControl = 0;
    I16 offset;
    U8 poll_bit = 0x00;
    U8 final_bit = 0x00;
    U16 sRej_ReqSeq = 0;
    U8 sRejIndex=0;
#ifdef __BT_3_0_HS__
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("Set channel f:%08x to %d",Channel, Channel->ampSuspend));
#endif

    if(Channel->ampSuspend == 1)
        return 0;
#endif
	sRej_ReqSeq = L2Cap_EnhancedGetNextSrejRxMissingRejectTable(Channel, &sRejIndex);
    Report(("sRejIndex:%d to %d",sRejIndex, Channel->enhancedMaxMissingSrejNo));

    if(sRejIndex == Channel->enhancedMaxMissingSrejNo)
    {
        /* N(R) was already sent in an I-Frame. Return the system packet. */
        Channel->l2capTxCount--;
        ReturnSysPkt(Packet);
        Report(("All Srej sent out"));
        return 0;
    }
    Packet->headerLen = 0;
    
    supportExtControl = L2CAP_GetControlFieldType(Channel);

    if(supportExtControl != L2CAP_CONTROL_FIELD_EXTENDED)
        offset = BT_PACKET_HEADER_LEN - 4;
    else
        offset = BT_PACKET_HEADER_LEN - 6;

    /*
     * Need to send an RR or a Reject. We only need to send one,
     * but if both are flagged, the the Reject takes precidence. 
     */
    Report(("Build S frame"));
    if (Channel->flags2 & L2CHF2_SEND_SREJ)
    {
        if(supportExtControl != L2CAP_CONTROL_FIELD_EXTENDED)
        {
            type = 0x0D; /* SREJ */
        }
        else
        {
            type = 0x03; /* SREJ */
        }
    }    
    else
    {
        Report(("MUST SRej is enabled"));
        Assert(0);
    }

    Channel->flags2 &= ~(L2CHF2_SEND_REJECT | L2CHF2_SEND_RR | L2CHF2_SEND_RNR | L2CHF2_SEND_SREJ );
    Report(("H channel->enhancedNeedToSendFinal:%d",Channel->enhancedNeedToSendFinal));    

    if(Channel->enhancedNeedToSendFinal == 0x01)
    {
        Channel->enhancedPollFinal |= L2CONTROLFRAMEWITHFINAL;
        Channel->enhancedNeedToSendFinal = 0;
    }

    /* Build the S-Frame */
    switch(supportExtControl)
    {
        case L2CAP_CONTROL_FIELD_STANDARD:
            /* The Retransmission bits is always set to 0 when flow control mode. */
            if (Channel->flags2 & L2CHF2_NO_RETRANS_IN)
            {
                Assert(Channel->inLinkMode == LINK_MODE_RETRANSMISSION);
                /* The application has requested that retransmissions be suspended. */
                type |= 0x80;   /* Set Retransmission disable bit */
            }
            Packet->header[offset] = type;
            Packet->header[offset+1] = sRej_ReqSeq;
            Packet->headerLen = 0x02;
            break;
        case L2CAP_CONTROL_FIELD_EXTENDED:
            if((Channel->enhancedPollFinal & L2CONTROLFRAMEWITHFINAL) !=0)
            {   
                final_bit = 0x02;
            }
            if((Channel->enhancedPollFinal & L2CONTROLFRAMEWITHPOLL) !=0)
            {   
                poll_bit = 0x04;
            }
            StoreLE16(Packet->header + offset, ((sRej_ReqSeq << 2) | final_bit | 0x01));
            StoreLE16(Packet->header + offset + 2, poll_bit | type);
            Channel->enhancedPollFinal = 0;
            Packet->headerLen = 0x04;
            break;
        case L2CAP_CONTROL_FIELD_ENHANCED:
            if((Channel->enhancedPollFinal & L2CONTROLFRAMEWITHFINAL) !=0)
            {   
                type |= 0x80;
            }
            if((Channel->enhancedPollFinal & L2CONTROLFRAMEWITHPOLL) !=0)
            {   
                type |= 0x10;
            }
            Channel->enhancedPollFinal = 0;
            Packet->header[offset] = type;
            Packet->header[offset+1] = sRej_ReqSeq;
            Channel->enhancedPollFinal = 0;
            Packet->headerLen = 0x02;
            break;
    }

    /* Append FCS to S-Frame */
    if((Channel->outFcsUsed != L2FCS_OPTION_NO_FCS) || (Channel->inFcsUsed != L2FCS_OPTION_NO_FCS))
    {    
        fcs = L2Cap_StartCrc(Channel->remoteCid, Packet->headerLen+2);
        fcs = L2Cap_ComputeCrc(Packet->header + offset, Packet->headerLen, fcs);
        Packet->header[offset + Packet->headerLen] = (U8) (fcs);
        Packet->header[offset + Packet->headerLen+1] = (U8) (fcs >> 8);
        Report(("S frame fcs:%04x",fcs));
        Packet->headerLen += 2;
    }	
	if( L2Cap_EnhancedVerifySrejAllSentInRxMissingRejectTable(Channel) == FALSE)
    {
	    Report(("SREJ Queue not empty, try S with SREJ again"));
	    Channel->flags2 |= (L2CHF2_SEND_SREJ);
	    L2Cap_SendSFrame(Channel);
    }
    return Packet;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessReqSeq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Control     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL L2Cap_ProcessReqSeq(L2CAP_Channel *Channel, U16 Control, U8 f_control, U8 *acked)
{
    U8 i=0;
    U16 previousAck;
    U16 rxAckSeq;
    U16 duplicateAck = 0;
    U8 controlField;
    U16 wrapValue = 0;
    U16 nonAckNo;
    
    controlField = L2CAP_GetControlFieldType(Channel);

    previousAck = Channel->ackSeq;
    if(controlField == L2CAP_CONTROL_FIELD_EXTENDED)
    {
        rxAckSeq = (U16) (Control >> 2);
    }
    else
    {
        rxAckSeq = (U16) (Control & 0x3f);
    }
#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("Local Cid:%d, ackSeq:%d, sendSeq:%d",Channel->localCid, Channel->ackSeq, Channel->sendSeq));
    Report(("DEBUG: The remote side send ReqSeq:%d",rxAckSeq));
#endif
    /* N(R) is valid if: mod64(N(R) - V(A)) <= mod64(V(S) - V(A)) */
    /* The sequence number is valid */
    /* The acknum may larger than the unacked because the remote ack old ack number */
    {
        /* Valid N(R), update V(A). */
        /* Update the ack sequence from the received frame */
#if __BT_30_DEBUG__ == XA_ENABLED
        Report(("DEBUG:Channel->ackSeq:%d",Channel->ackSeq));
        Report(("DEBUG:Channel->txComposeSeq:%d",Channel->txComposeSeq));
#endif
        if(Channel->ackSeq == rxAckSeq)
        {
            duplicateAck = 1;
        }
        if(Channel->txComposeSeq > Channel->ackSeq)
        {
        	/* ackSeq ,       txComposeSeq */
            /* No wrap */
            if((rxAckSeq > Channel->ackSeq) && (rxAckSeq <= Channel->txComposeSeq))
            {
	            /* ackSeq ,    rxAckSeq,   txComposeSeq */	
                /* rxAckSeq shall be updated to new one */
                Channel->ackSeq = rxAckSeq;
            }
            else if((rxAckSeq == Channel->ackSeq) || (rxAckSeq == Channel->txComposeSeq))
            {
                duplicateAck = 1;
            }
			else
			{
				Report(("L2CAP ReqSeq CheckFail0"));
				Report(("L2CAP with Invalid ReqSeq:%d",rxAckSeq));
				Report(("L2CAP with Invalid ackSeq:%d",Channel->ackSeq));				
				Report(("L2CAP with Invalid txComposeSeq:%d",Channel->txComposeSeq));								
				return FALSE;
			}
        }
        else if((Channel->txComposeSeq == Channel->ackSeq) && (Channel->txComposeSeq == rxAckSeq))
        {
            duplicateAck = 1;
        }
        else if(Channel->txComposeSeq == Channel->ackSeq)
        {
        	/* rxAckSeq is not the illgeal one. */
			Report(("L2CAP ReqSeq CheckFail1 ackSeq == txComposeSeq(not sent yet)!"));
			Report(("L2CAP with Invalid ReqSeq:%d",rxAckSeq));
			Report(("L2CAP with Invalid ackSeq:%d",Channel->ackSeq));				
			Report(("L2CAP with Invalid txComposeSeq:%d",Channel->txComposeSeq));								
			return FALSE;
        }		
        else
        {
            /* Wrap happens,  */
            /* 
                        ackSeq = 62;
                        rxAckSeq = 0x00;
                        txComposeSeq = 0x00;
                    */
            if((rxAckSeq > Channel->ackSeq) || (rxAckSeq <= Channel->txComposeSeq))
            {
                /* rxAckSeq shall be updated to new one */
                Channel->ackSeq = rxAckSeq;
            }
            else if((rxAckSeq == Channel->ackSeq) || (rxAckSeq == Channel->txComposeSeq))
            {
                duplicateAck = 1;
            }
			else
			{
				Report(("L2CAP ReqSeq CheckFail2"));
				Report(("L2CAP with Invalid ReqSeq:%d",rxAckSeq));
				Report(("L2CAP with Invalid ackSeq:%d",Channel->ackSeq));				
				Report(("L2CAP with Invalid txComposeSeq:%d",Channel->txComposeSeq));								
				return FALSE;
			}
        }
#if __BT_30_DEBUG__ == XA_ENABLED
        Report(("DEBUG:Channel->ackSeq:%d",Channel->ackSeq));    
#endif
        if(Channel->txComposeSeq > Channel->ackSeq)
        {
            /* No wrap */
            if(Channel->sendSeq <= Channel->ackSeq)
            {
                /* We are not necessary to retransmission the acked packet */
                /* Move sendSeq to ackSeq */
                Channel->sendSeq = Channel->ackSeq;
            }
        }
        else if(Channel->txComposeSeq == Channel->ackSeq)
        {
            Channel->sendSeq = Channel->ackSeq;
        }
        else
        {
            /* Wrap happens,  */
            /* We are not necessary to retransmission the acked packet */
            /* Move sendSeq to ackSeq */
            if((Channel->sendSeq > Channel->txComposeSeq) && (Channel->sendSeq <= Channel->ackSeq))
            {
                Channel->sendSeq = Channel->ackSeq;
            }
        }
#if __BT_30_DEBUG__ == XA_ENABLED
        Report(("DEBUG:Channel->sendSeq:%d",Channel->sendSeq));
#endif
        wrapValue = L2Cap_GetSequenceWrapValue(Channel);
        for(i = previousAck; i != Channel->ackSeq; i = ((i+1)%(wrapValue+1)))
        {
            Channel->txIframeCount[i] = 0;
        }
        nonAckNo = L2Cap_GetUnAckNumber(Channel, wrapValue);
#if __BT_30_DEBUG__ == XA_ENABLED
        Report(("L2cap enhanced nonAckNo:%d",nonAckNo));
#endif
//        if((duplicateAck ==0) && (nonAckNo ==0))
        if((duplicateAck == 0))

        {
            if((Channel->enhancedWait_F == 0) ||
                ((Channel->enhancedWait_F == 0) && (f_control == 1)))
            {
#if __BT_30_DEBUG__ == XA_ENABLED
                Report(("DEBUG: New Ack, stop timer"));
#endif
                L2Cap_StopTimer(Channel);
            }
        }
        else if(duplicateAck !=0)
        {
#if __BT_30_DEBUG__ == XA_ENABLED
            Report(("DEBUG: Duplicate ACK, just return"));
#endif
            return TRUE;

        }
        /* If packets are queued, request to send data. */

        if (Channel->l2capTxCount > 0)
        {
            Channel->flagReportReturnPacketToUpper = 1;
            if(Channel->txIFrameRetransmissionInBasebandCount ==0)
            {
                /* No retransmission I frame on going on Baseband, we can update to the upper layer. */
                /* Return any packets acknowledged by the N(R) we just received. */
                L2Cap_ReturnSentPackets(Channel, BT_STATUS_SUCCESS);
                Channel->flagReportReturnPacketToUpper = 0;
            }
            L2Cap_IssueHciRequestToSend(Channel->link);
        }

        
        if (Channel->state == L2C_OPEN)
        {
            if (Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
            {
                L2Cap_EnhancedRetransmissionPassToTxFBit(Channel, f_control, Channel->ackSeq);
                nonAckNo = L2Cap_GetUnAckNumber(Channel, wrapValue);
                if(1)
                {
                    if(f_control == 1)
                    {
                        if((Channel->enhancedRejActioned == 0) &&
                            (nonAckNo !=0))
                        {
                            Channel->sendSeq = Channel->ackSeq;
                            L2Cap_IssueHciRequestToSend(Channel->link);                            
                        }
                        else{
                           Channel->enhancedRejActioned = 0; 
                        }
                    }
#ifdef __BT_3_0_HS__
                    Report(("Send RR a8 :%d",nonAckNo));
#endif
                    L2Cap_EnhancedRetransmissionSendAck(Channel, 0);
                    *acked = 1;
                }
                if(f_control == 0)
                {
                    /* If F =0, no timer action. just return */
                    return TRUE;
                }
            }
            if ((Channel->ackSeq != Channel->txComposeSeq) && !(Channel->flags2 & L2CHF2_NO_RETRANS_OUT))
            {
                /* Still has unack frame, no retransmission timer */
#if __BT_30_DEBUG__ == XA_ENABLED
                Report(("Start L2Cap_StartRetransmitTimer 4 timer"));
#endif
                L2Cap_StartRetransmitTimer(Channel);
            }
            else
            {
#if __BT_30_DEBUG__ == XA_ENABLED
                Report(("Start L2Cap_StartMonitorTimer 4 timer"));
#endif
                L2Cap_StartMonitorTimer(Channel);
            }
        }
        return TRUE;
    }

    /*
     * Invalid N(R): In flow control only mode, we automatically advance the
     * ackSeq on timer expiration. This could cause us to receive an apparently
     * bad Nr if the packet is very delayed. Just ignore it.
     */
    if (Channel->inLinkMode == LINK_MODE_FLOW_CONTROL)
    {
        return TRUE;
    }

    /* Invalid N(R): Close the channel! */
    return FALSE;
}




/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessReqSeq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Control     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL L2Cap_ProcessReqSeqNotUpdateValue(L2CAP_Channel *Channel, U16 Control, U16 *ReqSeq)
{
    U8 i=0;
    U16 rxReqSeq;
    U8 controlField;
    BOOL result = FALSE;
    
    controlField = L2CAP_GetControlFieldType(Channel);

    if(controlField == L2CAP_CONTROL_FIELD_EXTENDED)
    {
        rxReqSeq = (U16) (Control >> 2);
    }
    else
    {
        rxReqSeq = (U16) (Control & 0x3f);
    }
    *ReqSeq = rxReqSeq;
    Report(("Local Cid:%d, ackSeq:%d, sendSeq:%d",Channel->localCid, Channel->ackSeq, Channel->sendSeq));
    Report(("DEBUG: The remote side send ReqSeq:%d",rxReqSeq));
    {
        Report(("DEBUG:Channel->ackSeq:%d",Channel->ackSeq));
        Report(("DEBUG:Channel->txComposeSeq:%d",Channel->txComposeSeq));
        if(Channel->txComposeSeq > Channel->ackSeq)
        {
            /* In sequence, ackSeq, rxReqSeq, TxComposeSeq */
            if ((rxReqSeq >= Channel->ackSeq) && (rxReqSeq <= Channel->txComposeSeq))
                result = TRUE;
        }
        else
        {
            /* In sequence, 
                        rxReqSeq, TxComposeSeq  ackSeq or 
                        TxComposeSeq  ackSeq, rxReqSeq
                    */
            if ((rxReqSeq <= Channel->txComposeSeq) || (rxReqSeq >= Channel->ackSeq))
                result = TRUE;

        }
    }
    /* Invalid N(R) */
    return result;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessTxSeq
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Control     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL L2Cap_ProcessTxSeq(L2CAP_Channel *Channel, U16 Control)
{
    U16 ns;
    U8 controlField;

    controlField = L2CAP_GetControlFieldType(Channel);
    if(controlField != L2CAP_CONTROL_FIELD_EXTENDED)
    {
        ns = (U16) ((Control >> 1) & 0x3f);

    }
    else
    {
        ns = (U16) (Control >> 2);
    }
    /* TxSeq */

#if __BT_30_DEBUG__ == XA_ENABLED
    Report(("TX remote send TxSeq:%d", ns));
#endif
    if (ns == Channel->recvSeq)
    {
        /* N(S) is in-sequence, process the frame. V(R) is updated later. */
#if __BT_30_DEBUG__ == XA_ENABLED        
        Report(("DEBUG Rx expected data"));
#endif
        Channel->flags2 &= ~L2CHF2_REJECT_ACTIVE;
        return TRUE;
    }

    /*
     * IMHO: A valid N(S) is within rxWindow of V(R)-1 either greater or less than.
     * This is more robust than is specified. So
     *    V(R) - rxWindow <= N(S) < V(R) + rxWindow
     */
    if (((ns - Channel->recvSeq) & (L2Cap_GetSequenceWrapValue(Channel))) < Channel->rxWindow)
    {
        /* ns < recvSeq  + rxWindow, received exceeed our expected. It means a packet lost. */
        /* N(S) is valid but it's out-of-sequence. Send a reject if we haven't already. */
        Report(("DEBUG Sequence failed 1"));
        if ((Channel->inLinkMode == LINK_MODE_RETRANSMISSION) && ((Channel->flags2 & L2CHF2_REJECT_ACTIVE) == 0))
        {

            /* Send a REJECT Packet */
            Channel->flags2 |= L2CHF2_SEND_REJECT;
            L2Cap_SendSFrame(Channel);
        }
        /*  Allow processing of the frame's control fields. */
        return TRUE;
    }

    if (((Channel->recvSeq - ns) & (L2Cap_GetSequenceWrapValue(Channel))) <= Channel->rxWindow)
    {
        Report(("DEBUG Sequence failed 2"));

        /*
         * N(S) is valid but it's a duplicate. Allow processing 
         * of the frame's control fields. 
         */
        return TRUE;
    }

    /* N(S) is invalid, don't process frame. */
    return FALSE;
}

/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessFlowControlRetransmissionTimeout
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Control     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
void L2Cap_ProcessFlowControlRetransmissionTimeout(L2CAP_Channel *Channel)
{
    U8 previousAck;
    
    previousAck = Channel->ackSeq;
    Report(("Local Cid:%d, ackSeq:%d, sendSeq:%d",Channel->localCid, Channel->ackSeq, Channel->sendSeq));
    if(Channel->txComposeSeq != Channel->ackSeq)
    {
        Channel->ackSeq = (U16) ((Channel->ackSeq + 1) & (L2Cap_GetSequenceWrapValue(Channel)));
        Channel->txIframeCount[previousAck] = 0;
//        L2Cap_ReturnSentPackets(Channel, BT_STATUS_TIMEOUT);
        L2Cap_ReturnSentPackets(Channel, BT_STATUS_SUCCESS);
        L2Cap_IssueHciRequestToSend(Channel->link);
    }
    else
    {
        Report(("Retransmission timout but all data been acked"));
    }
    if (Channel->state == L2C_OPEN)
    {
        if (Channel->ackSeq != Channel->txComposeSeq)
        {
            /* Still has unack frame, no retransmission timer */
            L2Cap_StartRetransmitTimer(Channel);
        }
        else
        {
            Report(("Start L2Cap_StartMonitorTimer 4 timer"));
            L2Cap_StartMonitorTimer(Channel);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ProcessRbit
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Control     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_ProcessRbit(L2CAP_Channel *Channel, U8 Control)
{
    /* Check for retransmission disable flag changes. */
    if (Control & 0x80)
    {
        /* Retransmission disable bit is set now! */
        /* If the flag2 is already set, do nothing. */
        if ((Channel->flags2 & L2CHF2_NO_RETRANS_OUT) == 0)
        {
            /*
             * The remote peer just disabled retransmissiions. 
             * Stop the retransmission timer and start the monitor timer.
             */
            Channel->flags2 |= L2CHF2_NO_RETRANS_OUT;
            
            /*
             * Set the retransmission setting to ON by L2CHF2_NO_RETRANS_OUT
             */

            if (Channel->flags2 & L2CHF2_RETRANS_ACTIVE)
            {
                /* Retransmission is activated, stop it. */
                L2Cap_StopTimer(Channel);
            }
            /* Start the monitor timer */
            L2Cap_StartMonitorTimer(Channel);
        }
    }
    else if (Channel->flags2 & L2CHF2_NO_RETRANS_OUT)
    {
        /* 
         * The new retransmission disable bit is set to OFF,
         * The previous operation is set to retransmission disable ON.
         */
         
        /* Remote side just enabled retransmissions. */
        Channel->flags2 &= ~L2CHF2_NO_RETRANS_OUT;
        /* Clear the flag2 */

        /* Stop Monitor timer and request to send now that we're unblocked. */
        L2Cap_StopTimer(Channel);
        /* Stop all timer */
        L2Cap_IssueHciRequestToSend(Channel->link);

        /* Retransmission previous packet */
        
        /* If segments in transit, start the retransmit timer. */
        if (Channel->sendSeq != Channel->ackSeq)
        {
            L2Cap_StartRetransmitTimer(Channel);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ReturnSentPackets
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Status      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_ReturnSentPackets(L2CAP_Channel *Channel, BtStatus Status)
{
    BtPacket *packet, *next;
    ListEntry *queue = 0;

    queue = &(Channel->link->txNonAckQueue);
    packet = (BtPacket*) GetHeadList(queue);
    while (packet != (BtPacket*) queue)
    {
        next = (BtPacket*) GetNextNode(&packet->node);

        if (packet->llpContext == Channel->localCid)
        {
            if ((packet->remoteCid != 0x0001) && (packet->segNum > 0) && ((packet->flags & BTP_FLAG_LSYSTEM) == 0))
            {
                if(Status != BT_STATUS_NO_CONNECTION)
                {
                    /* It's an BT1.2 connection oriented data packet. */
                    if (packet->segNum <= ((Channel->ackSeq - packet->segStart) & (L2Cap_GetSequenceWrapValue(Channel))))
                    {
                        /* All segments have been acknowledged, return it to the application. */
                        Assert(((Channel->ackSeq - packet->segStart) & 0x3f) < 64);
                        RemoveEntryList(&packet->node);
                        L2CAP_HciTxDone(packet, Status, Channel->link->hciHandle);
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    /* Free and return to upper layer when the connection is free. */
                    RemoveEntryList(&packet->node);
                    L2CAP_HciTxDone(packet, Status, Channel->link->hciHandle);
                }
            }
        }
        packet = next;
    }
}
#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  L2Cap_FlushChannelTransmits
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_FlushChannelTransmits(L2CAP_Channel *Channel)
{
    BtPacket *packet, *next;
    ListEntry *queue = NULL;
    U8 i = 0;

    for (i = 0; i < 2; i++)
    {
        switch (i)
        {
            case 0x00:
                queue = &(Channel->link->txHighPriorityQueue);
                break;
            case 0x01:
                queue = &(Channel->link->txLowPriorityQueue);
                break;
            default:
                Assert(0);
                break;
        }
        packet = (BtPacket*) GetHeadList(queue);
        while (packet != (BtPacket*) queue)
        {
            next = (BtPacket*) GetNextNode(&packet->node);

            if (packet->llpContext == Channel->localCid)
            {
                RemoveEntryList(&packet->node);
                L2CAP_HciTxDone(packet, BT_STATUS_NO_CONNECTION, Channel->link->hciHandle);
            }
            packet = next;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_FlushOnePktTransmits
 * DESCRIPTION
 *  
 * PARAMETERS
 *  channel_id      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U8 L2Cap_FlushOnePktTransmits(L2capChannelId channel_id)
{
    BtPacket *packet, *next;
    L2CAP_Channel *channel;
    ListEntry *queue = NULL;
    U8 i = 0;
    if( channel_id < BASE_DYNAMIC_CID || CID_TO_INDEX(channel_id) > L2CAP_NUM_CHANNELS ){
        // out of range
        return BT_STATUS_NO_CONNECTION;
    }

    channel = &L2C(channels)[CID_TO_INDEX(channel_id)];
    for (i = 0; i < 2; i++)
    {
        switch (i)
        {
            case 0x00:
                queue = &(channel->link->txHighPriorityQueue);
                break;
            case 0x01:
                queue = &(channel->link->txLowPriorityQueue);
                break;
            default:
                break;
        }
        packet = (BtPacket*) GetHeadList(queue);
        do
        {
            if (packet->llpContext == channel->localCid)
            {
                RemoveEntryList(&packet->node);
                L2CAP_HciTxDone(packet, BT_STATUS_NO_CONNECTION, channel->link->hciHandle);
                return 1;
            }
            next = (BtPacket*) GetNextNode(&packet->node);
            packet = next;
        }
        while (packet != (BtPacket*) queue);

    }
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_EnableHeavyRTTraffic
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_EnableHeavyRTTraffic(void)
{
    L2C(heavyTrafficSource) = L2C_HEAVY_TRAFFIC_EXIST;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_DisableHeavyRTTraffic
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_DisableHeavyRTTraffic(void)
{
    L2C(heavyTrafficSource) = L2C_HEAVY_TRAFFIC_NOT_EXIST;
}

/*
 * L2Cap_NotifyPsm()
 */


/*****************************************************************************
 * FUNCTION
 *  L2Cap_NotifyPsm
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 *  Event       [IN]        
 *  Status      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/

void L2Cap_NotifyPsm(L2CAP_Channel *Channel, L2capEventType Event, U16 Status)
{
    //OS_Report("l2cap  NoifyPSM chid:%d event:%d status:%d", Channel->localCid, Event, Status);
    if( 0 == Channel ){
        OS_Report("l2cap ignore null channel");
        return;
    }
    if (Channel->flags & L2CHF_NOT_VISIBLE)
    {
        /*
         * The Upper layer protocol or applicaiton does not know of this
         * channel so consume all events.
         */
        bt_trace(TRACE_GROUP_1, L2CAP_NOTIFYPSM_CONSUMED_NOTIFICATION_EVENT_xX, Event);
        return;
    }
    L2C(callbackParms).event = Event;
    L2C(callbackParms).status = Status;
    if (Channel->psmInfo->callback == 0)
    {
        Assert(0);
    }
    Channel->psmInfo->callback(Channel->localCid, &L2C(callbackParms));
    L2C(callbackParms).event = 0;
}

#ifdef __BT_4_0_BLE__
void L2Cap_NotifyBLE(L2capFixCid *fixCid, L2capEventType Event, U16 Status, U16 cid)
{
    L2C(callbackParms).event = Event;
    L2C(callbackParms).status = Status;
    if (fixCid->callback == 0)
    {
        Assert(0);
    }
    fixCid->callback(cid, &L2C(callbackParms));
    L2C(callbackParms).event = 0;
}
#endif

/*****************************************************************************
 * FUNCTION
 *  L2Cap_QueryHeavyTraffic
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
BOOL L2Cap_QueryHeavyTraffic(void)
{
    if (L2C(heavyTrafficSource) == L2C_HEAVY_TRAFFIC_EXIST)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*
 * ValidateOptions()
 */


/*****************************************************************************
 * FUNCTION
 *  ValidateOptions
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Options         [IN]        
 *  OptionsLen      [IN]        
 *  Len             [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BOOL ValidateOptions(const U8 *Options, U16 OptionsLen, U16 *Len)
{
    *Len = 0;

    L2Cap_DumpHex("config option ValidateOptions", (const char*)Options, OptionsLen); //OptionsLen has been changed

    while (OptionsLen)
    {
        bt_trace(TRACE_GROUP_1, L2CAP_VALIDATEOPTIONS_OPTION_xD_LEN_xD, *Options, *(Options + 1));
        *Len += Options[1] + 2;

        /* Validate Options 1 by 1 ?? */

        OptionsLen -= Options[1] + 2;
        Options += Options[1] + 2;

        if (OptionsLen >= 0x8000)
        {
            /* Invalid Options encoding */
            OS_Report("L2cap ValidateOptions fail!");
            return FALSE;
        }
    }

    /* Options encoding is valid */
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ValidateCommand
 * DESCRIPTION
 *  
 * PARAMETERS
 *  RxData          [IN]        
 *  RxDataLen       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BOOL L2Cap_ValidateCommand(const U8 *RxData, U16 RxDataLen)
{
    U16 len, cmdLen;

    while (RxDataLen)
    {

        /* First make sure the claimed PDU size is all there. */
        len = LEtoHost16(RxData + 2) + SIGNAL_HEADER_LEN;

        if (RxDataLen < len)
        {
            return FALSE;
        }
        cmdLen = 0;

        /* Now, Calculate the required PDU size. */
        switch (RxData[0])
        {
            case LLC_COMMAND_REJ:
                switch (LEtoHost16(RxData + 4))
                {
                        /* Reason */
                    case 0:
                        cmdLen = 6;
                        break;
                    case 1:
                        cmdLen = 8;
                        break;
                    case 2:
                        cmdLen = 10;
                        break;
                    default:
                        cmdLen = len;
                }
                break;

            case LLC_CONN_REQ:
                cmdLen = 8;
                break;

            case LLC_CONN_RSP:
                cmdLen = 12;
                break;

            case LLC_CONFIG_REQ:
                if ((len > 8) && (!ValidateOptions(RxData + 8, (U16) (len - 8), &cmdLen)))
                {
                    return FALSE;
                }
                cmdLen += 8;
                break;

            case LLC_CONFIG_RSP:
                if ((len > 10) && (!ValidateOptions(RxData + 10, (U16) (len - 10), &cmdLen)))
                {
                    return FALSE;
                }
                cmdLen += 10;
                break;

            case LLC_DISC_REQ:
                cmdLen = 8;
                break;

            case LLC_DISC_RSP:
                cmdLen = 8;
                break;

            case LLC_ECHO_REQ:
                cmdLen = len;
                break;

            case LLC_ECHO_RSP:
                cmdLen = len;
                break;

            case LLC_INFO_REQ:
                if ((LEtoHost16(RxData + 4) == L2INFO_CONNECTIONLESS_MTU) ||
                    (LEtoHost16(RxData + 4) == L2INFO_EXTENDED_FEATURES))
                {
                    cmdLen = 6;
                }
                else
                {
                    cmdLen = len;   /* Unknown INFO_TYPE */
                }
                break;

            case LLC_INFO_RSP:
                if (LEtoHost16(RxData + 4) == L2INFO_CONNECTIONLESS_MTU)
                {
                    if (LEtoHost16(RxData + 6) == 0)    /* result == success */
                    {
                        cmdLen = 10;
                    }
                    else
                    {
                        cmdLen = 8;
                    }
                }
                else if (LEtoHost16(RxData + 4) == L2INFO_EXTENDED_FEATURES)
                {
                    if (LEtoHost16(RxData + 6) == 0)    /* result == success */
                    {
                        cmdLen = 12;
                    }
                    else
                    {
                        cmdLen = 8;
                    }
                }
                else
                {
                    cmdLen = len;   /* Unknown INFO_TYPE */
                }
                break;

            default:
                /* We do not recognize this command so we cannot verify it */
                return TRUE;

        }

        /* Finally, make sure the PDU size is what it should be. */
        if (cmdLen != len)
        {
            return FALSE;
        }

        RxDataLen -= len;
        RxData += len;
    }

    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_IssueHciRequestToSend
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Link        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_IssueHciRequestToSend(BtRemoteDevice *Link)
{
#if L2CAP_FLOW_NON_BASIC_MODE== XA_DISABLED
    ListEntry *queue;

    if (IsListEmpty(&Link->txHighPriorityQueue) != 0)
    {
        queue = &(Link->txHighPriorityQueue);
    }
    else
    {
        queue = &(Link->txLowPriorityQueue);
    }
    if (!IsListEmpty(queue) && (Link->state == BDS_CONNECTED) && Link->okToSend && (Link->mode != BLM_HOLD_MODE))
    {
        /* Request to send */
        AssertEval(HCI_RequestToSend(Link->hciHandle) == BT_STATUS_SUCCESS);
        return;
    }
    else
    {
        bt_trace(TRACE_GROUP_1, L2CAP_ISSUEHCIREQUESTTOSEND_HCI_REQUEST_TO_SEND_CALLED);
    }
#else /* For FLOW control enabled */
    if ((Link->state == BDS_CONNECTED) && Link->okToSend && (Link->mode != BLM_HOLD_MODE))
    {
        /* Request to send */
        AssertEval(HCI_RequestToSend(Link->hciHandle) == BT_STATUS_SUCCESS);
        return;
    }
    else
    {
        bt_trace(TRACE_GROUP_1, L2CAP_ISSUEHCIREQUESTTOSEND_HCI_REQUEST_TO_SEND_CALLED);
    }

#endif
}

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  L2Cap_ComputeCrc
 * DESCRIPTION
 *  Notice that one of L2Cap_InitCrc() or L2Cap_StartCrc() must be
 *  used to generate the starting CRC parameter to this function.
 * PARAMETERS
 *  Buffer      [IN]        
 *  Length      [IN]        
 *  Crc         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U16 L2Cap_ComputeCrc(const U8 *Buffer, U16 Length, U16 Crc)
{
    U16 tmp;

    while (Length--)
    {
        /* Compute checksum of lower four bits */
        tmp = L2CrcTab[Crc & 0xF];
        Crc = (Crc >> 4) & 0x0FFF;
        Crc = Crc ^ tmp ^ L2CrcTab[*Buffer & 0xF];

        /* Now compute checksum of upper four bits */
        tmp = L2CrcTab[Crc & 0xF];
        Crc = (Crc >> 4) & 0x0FFF;
        Crc = Crc ^ tmp ^ L2CrcTab[(*Buffer >> 4) & 0xF];

        Buffer++;
    }

    return Crc;
}

#if 0
/*
 * L2Cap_InitCrc()
 */
U16 L2Cap_InitCrc(U16 Init)
{
    U16 i;
    U16 crc = 0;

    /* Reflects the lower 15 bits of initial CRC value */
    for (i = 0; i <= 15; i++) {
        if (Init & (1 << i))
            crc |= 1 << (15 - i);
    }

    return crc;
}
#endif /* 0 */ 


/*****************************************************************************
 * FUNCTION
 *  L2Cap_StartCrc
 * DESCRIPTION
 *  
 * PARAMETERS
 *  RemoteCid       [IN]        
 *  Length          [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static U16 L2Cap_StartCrc(U16 RemoteCid, U16 Length)
{
    U8 hdr[4];

    hdr[0] = (U8) (Length);
    hdr[1] = (U8) (Length >> 8);
    hdr[2] = (U8) (RemoteCid);
    hdr[3] = (U8) (RemoteCid >> 8);

    return L2Cap_ComputeCrc(hdr, 4, L2Cap_InitCrc(0));
}
#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */

/*
 * RTX & ERTX Timer functions
 */


/*****************************************************************************
 * FUNCTION
 *  L2CAP_TimerFire
 * DESCRIPTION
 *  Called by some timer management function when the timer expires.
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2CAP_TimerFire(EvmTimer *Timer)
{
    L2CAP_Channel *channel;
    I8 expired;
    BtStatus status = BT_STATUS_SUCCESS;
    S32 currentMode = 0;

    bt_prompt_trace(MOD_BT, "[BT] L2CAP_TimerFire");  

    channel = ContainingRecord(Timer, L2CAP_Channel, timer);

    if (channel->flags & L2CHF_RTX_ACTIVE)
    {
        expired = LLC_RTX_EXPIRED;
        XASTAT_CounterInc(l2cap_rtx_timeout, 1);
    }
    else if (channel->flags & L2CHF_ERTX_ACTIVE)
    {
        expired = LLC_ERTX_EXPIRED;
        if (channel->ertxTimer == 20)
        {
            if (BTBMGetDeviceModel(&(channel->link->bdAddr)) != DEVICE_MOTO_HT820)
            {
                channel->ertxTimer = 100;
                EVM_StartTimer(Timer, channel->ertxTimer);
                return;
            }
        }
        XASTAT_CounterInc(l2cap_ertx_timeout, 1);
        /* Check for active link */
        if (ME_GetCurrentMode(channel->link) != BLM_ACTIVE_MODE)
        {
            /* The link is in hold, park or sniff. Defer the timeout for 1 second. */
            if (channel->link->state == BDS_CONNECTED)
            {
                if (channel->link->mode != BLM_ACTIVE_MODE)
                {
                    /* Alert the ME that we have data to send. */
                    MeHandleDataReqWhileNotActive(channel->link);
                    channel->link->discFlag = TRUE;
                }
            }
            EVM_StartTimer(Timer, 1000);
            return;
        }
    }
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
    else if (channel->flags2 & L2CHF2_RETRANS_ACTIVE)
    {
        expired = LLC_RETRANS_EXPIRED;
        XASTAT_CounterInc(l2cap_retrans_timeout, 1);
    }
    else if (channel->flags2 & L2CHF2_MONITOR_ACTIVE)
    {
        expired = LLC_MONITOR_EXPIRED;
        XASTAT_CounterInc(l2cap_monitor_timeout, 1);
    }
#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */
    else
    {
        return; /* Timer must have been cancelled (really late) */
    }

    /* Check for active link */
    if (ME_GetCurrentMode(channel->link) != BLM_ACTIVE_MODE)
    {
        /* The link is in hold, park or sniff. Defer the timeout for 1 second. */
        bt_trace(TRACE_GROUP_1, RTX_TIMEOUT_IN_SNIFF_MODE);
#ifdef __BT_3_0_HS__
        if(channel->hciAMPHandler == 0)
#endif
        {
        currentMode = ME_GetCurrentMode(channel->link);
        status = ME_StopSniff(channel->link);
        OS_Report("l2cap: MECurrentMode:%d StopSniff status:%d", currentMode, status); // ALPS01074135 - show ME reutrn value
        EVM_StartTimer(Timer, 1000);
        return;
    }
    }

    /* Clear timer flags */
    channel->flags &= ~(L2CHF_RTX_ACTIVE | L2CHF_ERTX_ACTIVE);
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
    channel->flags2 &= ~(L2CHF2_RETRANS_ACTIVE | L2CHF2_MONITOR_ACTIVE);
#endif 

    L2Cap_StateMachine(channel, expired);
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_StartRtx
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_StartRtx(L2CAP_Channel *Channel)
{
    Assert((Channel->flags & (L2CHF_RTX_ACTIVE | L2CHF_ERTX_ACTIVE)) == 0);

    /*  bt_trace(TRACE_GROUP_1,STARTING_RTXBT_DEBUGx04X , Channel->localCid); */

    Channel->timer.func = L2CAP_TimerFire;
    Channel->flags |= L2CHF_RTX_ACTIVE;

    /* Timeout value is in milliseconds */
    bt_trace(TRACE_GROUP_1, RTX_SECONDS);
    EVM_StartTimer(&Channel->timer, (TimeT) (L2CAP_RTX_TIMEOUT * 1000));
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_StartErtx
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_StartErtx(L2CAP_Channel *Channel)
{
    Assert((Channel->flags & (L2CHF_RTX_ACTIVE | L2CHF_ERTX_ACTIVE)) == 0);

    Channel->timer.func = L2CAP_TimerFire;
    Channel->flags |= L2CHF_ERTX_ACTIVE;

    /*
     * Timeout value is in milliseconds. Special case for the W4_CONN_RSP 
     * timer because we're tracking a timeout on the the remote device.
     */
    Channel->ertxTimer = 20;
    if (Channel->state == L2C_W4_CONN_RSP)
    {
        EVM_StartTimer(&Channel->timer, (TimeT) (60 * 1000));   /* Maximum E-RTX */
        bt_trace(TRACE_GROUP_1, ERTX_60_SECONDS);
    }
    else
    {
        bt_trace(TRACE_GROUP_1, ERTX_20_SECONDS);
        EVM_StartTimer(&Channel->timer, (TimeT) (Channel->ertxTimer * 1000));

    }
}

#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED


/*****************************************************************************
 * FUNCTION
 *  L2Cap_StartRetransmitTimer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_StartRetransmitTimer(L2CAP_Channel *Channel)
{
    U32 timeoutValue;

    Assert((Channel->flags & (L2CHF_RTX_ACTIVE | L2CHF_ERTX_ACTIVE)) == 0);
    Assert(Channel->state == L2C_OPEN);

    if (Channel->flags2 & L2CHF2_MONITOR_ACTIVE)
    {
        /* Stop the monitor timer before starting the retransmit timer. */
        L2Cap_StopTimer(Channel);
    }

    if ((Channel->flags2 & L2CHF2_RETRANS_ACTIVE) == 0)
    {
        Channel->timer.func = L2CAP_TimerFire;
        Channel->flags2 &= ~L2CHF2_MONITOR_ACTIVE;        
        Channel->flags2 |= L2CHF2_RETRANS_ACTIVE;
        if(Channel->hciAMPHandler == L2PHY_TPYE_BASICEDR)
            timeoutValue = max(Channel->retransTimeout, MINIMUM_RETRANS_TIMEOUT);            
        else
            timeoutValue = 500;
        
//        Report(("L2Cap_StartRetransmitTimer:%d",timeoutValue));
        /* Timeout value is in milliseconds */
        EVM_StartTimer(&Channel->timer, timeoutValue);
    }
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_StartMonitorTimer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_StartMonitorTimer(L2CAP_Channel *Channel)
{
    U32 timeoutValue;
    
    Assert((Channel->flags & (L2CHF_RTX_ACTIVE | L2CHF_ERTX_ACTIVE)) == 0);
    Assert(Channel->state == L2C_OPEN);

    if ((Channel->flags2 & L2CHF2_MONITOR_ACTIVE) == 0)
    {
        Channel->timer.func = L2CAP_TimerFire;
        Channel->flags2 &= ~L2CHF2_RETRANS_ACTIVE;
        Channel->flags2 |= L2CHF2_MONITOR_ACTIVE;
        if(Channel->hciAMPHandler == L2PHY_TPYE_BASICEDR)
            timeoutValue = max(Channel->monitorTimeout, MINIMUM_MONITOR_TIMEOUT);            
        else
            timeoutValue = 500;
        /* Timeout value is in milliseconds */
        Report(("L2Cap_StartMonitorTimer:%d",timeoutValue));
        EVM_StartTimer(&Channel->timer, timeoutValue);
    }
}
#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */


/*****************************************************************************
 * FUNCTION
 *  L2Cap_StopTimer
 * DESCRIPTION
 *  Stops whatever timer is running.
 * PARAMETERS
 *  Channel     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_StopTimer(L2CAP_Channel *Channel)
{
//	Report(("L2CAP stop timer"));
    Channel->flags &= ~(L2CHF_RTX_ACTIVE | L2CHF_ERTX_ACTIVE);
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
    Channel->flags2 &= ~(L2CHF2_RETRANS_ACTIVE | L2CHF2_MONITOR_ACTIVE);
#endif 

    EVM_CancelTimer(&Channel->timer);
}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_DisconnectACLLink
 * DESCRIPTION
 *  
 * PARAMETERS
 *  link        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void L2Cap_DisconnectACLLink(BtRemoteDevice *link)
{
    bt_trace(TRACE_GROUP_1, L2CAP_FREE_OPEN_TIMER);
    EVM_ResetTimer(&link->discTimer);
    link->discTimer.func = L2CAP_DiscAclLink;
    link->discTimer.context = link;
    switch (link->discTimeoutLevel)
    {
        case BT_L2CAP_DISCONNECT_TIMEOUT_500MS:
            bt_trace(TRACE_GROUP_1, L2CAP_START_A_DISC_TIMER_BT_L2CAP_DISCONNECT_TIMEOUT_500MS);
            EVM_StartTimer(&link->discTimer, 500);
            break;
        case BT_L2CAP_DISCONNECT_TIMEOUT_50S:
            bt_trace(TRACE_GROUP_1, L2CAP_START_A_DISC_TIMER_BT_L2CAP_DISCONNECT_TIMEOUT_50S);
            EVM_StartTimer(&link->discTimer, 50000);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_DiscAclLink
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void L2CAP_DiscAclLink(EvmTimer *Timer)
{
    BtRemoteDevice *link;

    link = (BtRemoteDevice*) Timer->context;
    link->discTimer.func = 0;
    bt_trace(TRACE_GROUP_1, L2CAP_DiscAclLink_with_discFlag, link->discFlag);

    if ((link->state == BDS_CONNECTED) && (link->refCount == 0) && (link->discFlag))
    {
        if (!btbmCheckBondingStateWhenDisconnected(link->bdAddr.addr))
        {
            bt_trace(TRACE_GROUP_1, L2CAP_DISCACLLINK);
            link->disconnectACLCheckCreatACLAlwaysForce = 0;
            MeDisconnectLink(link,21);
        }
    }
}

U16 L2CAP_GetL2CAPRemoteCid(L2capChannelId ChannelId)
{
    I16 i;

    for (i = 0; i < L2CAP_NUM_CHANNELS; i++)
    {
        if (L2C(channels)[i].localCid == ChannelId)
        {
            return L2C(channels)[i].remoteCid;
        }
     }
    return 0x00;

}

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
U8 L2CAP_GetControlFieldType(L2CAP_Channel *Channel)
{
    U8 type = L2CAP_CONTROL_FIELD_STANDARD; /* Standard Control Field */

    if(Channel->inLinkMode == LINK_MODE_RETRANSMISSION)
    {
        type = L2CAP_CONTROL_FIELD_STANDARD;   /* Support Standard Field*/
    }
    if(Channel->inLinkMode == LINK_MODE_FLOW_CONTROL)
    {
        type = L2CAP_CONTROL_FIELD_STANDARD;   /* Support Standard Field*/
    }


    if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
    {
        if(Channel->supportExtControl !=0)
        {
            type = L2CAP_CONTROL_FIELD_EXTENDED;   /* Support Extended Control Field*/
        }
        else
        {
            type = L2CAP_CONTROL_FIELD_ENHANCED;   /* Support Enhanced Control Field*/
        }
    }
    if(Channel->inLinkMode == LINK_MODE_STREAMMING)
    {
        if(Channel->supportExtControl !=0)
        {
            type = L2CAP_CONTROL_FIELD_EXTENDED;   /* Support Extended Control Field*/
        }
        else
        {
            type = L2CAP_CONTROL_FIELD_ENHANCED;   /* Support Enhanced Control Field*/
        }
    }

    return type;
    
}

U16 L2Cap_GetSequenceWrapValue(L2CAP_Channel *Channel)
{
    U16 wrapValue;

    if (Channel->supportExtControl == 1)
    {
        wrapValue = 0x3fff;
    }
    else
    {
        wrapValue = 0x003f;
    }
    return wrapValue;
}

/**
 * @brief Has more TxWindow for composing packets
 * txComposeSeq: next TxSeq number
 * sendSeq: next send sequence number
 *              when received a REJ, the sendSeq will be set to the REJ's ReqSeq
 * ackSeq: next expected sequence number of remote device. the ackSeq-1 has been received.
 */
BOOL L2Cap_IsTxWindowAvailable(L2CAP_Channel *Channel)
{
    BOOL result = FALSE;
    U16 wrapValue;
    
    wrapValue = L2Cap_GetSequenceWrapValue(Channel);
    
    switch(Channel->inLinkMode){
        case LINK_MODE_ENHANCED_RETRANSMISSION:
        {
        if(Channel->enhancedWait_F)
        {
            /* in WAIT_F State, Pending New Tx Data, See spec page 164 of 394 */
#if __BT_30_DEBUG__ == XA_ENABLED
            Report(("In wait F, pending new tx data cid:%d", Channel->localCid));
#endif
            return FALSE;
            }
            // ALPS00094896: ftp client+server gets files
            if((Channel->enhancedRetransRemoteBusy == 0) &&
                ((((Channel->txComposeSeq - Channel->ackSeq) & wrapValue) < Channel->txWindow)))
            {
                return TRUE;
            }
        }
        break;
        case LINK_MODE_STREAMMING:
            result = TRUE;
        break;
        default:
        {
        result = 
            ((((Channel->txComposeSeq - Channel->ackSeq) & wrapValue) < Channel->txWindow) ? TRUE : FALSE);
        }
        break;
    }
    
#if __BT_30_DEBUG__ == XA_ENABLED
    if(result == FALSE)
    {
        Report(("TX windows full"));
    }
#endif    
    return result; 
}

BOOL L2Cap_IsRetransTxAvailable(L2CAP_Channel *Channel)
{
    BOOL result = FALSE;
    U16 wrapValue;
    
    wrapValue = L2Cap_GetSequenceWrapValue(Channel);
#ifdef __BT_3_0_HS__

		if(Channel->ampSuspend == 1)
			return FALSE;
#endif 
    if(Channel->inLinkMode == LINK_MODE_ENHANCED_RETRANSMISSION)
    {

        if((Channel->enhancedRetransRemoteBusy == 0) &&
            ((((Channel->sendSeq - Channel->ackSeq) & wrapValue) < Channel->txWindow)))
        {
            return TRUE;
        }
    }
    else
    {
        result = 
            ((((Channel->sendSeq - Channel->ackSeq) & wrapValue) < Channel->txWindow) ? TRUE : FALSE);
    }
    return result;
}


U16 L2Cap_GetUnAckNumber(L2CAP_Channel *Channel, U16 wrapValue)
{
    U8 number;
    
    number = ((Channel->sendSeq - Channel->ackSeq) & wrapValue);
    return number;

}

U16 L2Cap_GetRxUnAckNumber(L2CAP_Channel *Channel, U16 wrapValue)
{
    /* To get the number that we did not response the remote side*/
    U8 number;
    number = ((Channel->recvSeq - Channel->lastRxStateAckSeq) & wrapValue);
    return number;

}


L2capExtendedFlowSpecOption *L2Cap_GetInExtendedFlowSpec(U16 l2cap_id)
{
    L2CAP_Channel *channel;
    channel = &L2C(channels)[CID_TO_INDEX(l2cap_id)];
    return &(channel->inExtFlowSpec);
}

L2capExtendedFlowSpecOption *L2Cap_GetOutExtendedFlowSpec(U16 l2cap_id)
{
    L2CAP_Channel *channel;
    channel = &L2C(channels)[CID_TO_INDEX(l2cap_id)];
    return &(channel->outExtFlowSpec);
}

void L2Cap_InitLockStepConfig(L2CAP_Channel *Channel)
{
    if(Channel->outLinkMode == 0x00)
        Channel->outLinkMode = LINK_MODE_ENHANCED_RETRANSMISSION;
#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
    Channel->waitPreviousChannelConnected = 0;
#endif
#if L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED
    L2Cap_InitFlowControlSettings(Channel);
#endif 
#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
    L2Cap_InitLockStepControlSettings(Channel);
#endif
}

#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
void L2Cap_StartLockStepConfig(U8 local_cid)
{
	L2CAP_Channel *channel;
    U32 features;

    channel = &L2C(channels)[CID_TO_INDEX(local_cid)];
    features = channel->link->l2cap_feature;
    if (features & L2EXTENDED_FEATURE_EXTENDED_FLOW_OPTION)
    {
        Report(("Support L2EXTENDED_FEATURE_EXTENDED_FLOW_OPTION on BR/EDR"));
    channel->state = L2C_CONFIG;
    L2Cap_InitLockStepConfig(channel);
    SendAutoLockStepConfigReq(channel);

    }
    else
    {
        Report(("Not Support L2EXTENDED_FEATURE_EXTENDED_FLOW_OPTION on BR/EDR"));
        channel->reConfigState = FALSE;
        channel->lockstepNegoDone = TRUE;
        channel->configLockStep = FALSE;
        channel->lockstepTxPendingDone = FALSE;
        channel->rxFlowAvailable = 1;
        channel->txFlowAvailable = 1;
        channel->connectedEventReported = 0;
        L2Cap_NotifyPsm(channel, L2EVENT_LOCK_STEP_DONE, BT_STATUS_SUCCESS);
        L2Cap_NotifyPsm(channel, L2EVENT_CONNECTED, BT_STATUS_SUCCESS);
    }
    

}

U8 L2Cap_VerifyLockStepConfig(L2CAP_Channel *channel)
{
    L2capConfigFlags flags;
    U8 *response;
    U8 *options;
    I8 optionLen;
    U16 result = L2CFG_RESULT_SUCCESS;
    U16 remote, local, outMtu;
    
    flags = LEtoHost16(L2C(currRxData) + 6);
    if (flags & L2CFG_FLAG_MORE)
    {
        /* Not lock step config */
        return 0;
    }
    
    /* Build Configure response packet in channel workspace. */
    options = L2C(currRxData) + 8;
    optionLen = L2C(currRxDataLen) - 4;
    while (optionLen)
    {
        switch ((*options & ~CONFIG_OPTION_HINT))
        {
            case L2CFG_OPTION_EXT_FLOW_SPEC:
                break;
            default:
                /* Not lock step config */
                return 0;
        }
        optionLen -= options[1] + 2;
        options += options[1] + 2;
    }
    /* Only extended flow spec or no option.*/
    return 1;
}
#endif

#ifdef __BT_3_0_HS__
void L2Cap_NotifyLogicalChannelCreated(L2capChannelId ChannelId, U16 result)
{
    L2CAP_Channel *channel;
    U16 result_code;
    
    OS_LockStack();
    channel = L2Cap_GetValidChannel(ChannelId);
    if ((channel == 0) || ((channel->state < L2C_W4_MOVE) && (channel->state != L2C_CONFIG)))
    {
        OS_UnlockStack();
        return;
    }


    /* Normal operation for L2CAP create logical channel */
    if(result == BT_STATUS_SUCCESS)
        L2Cap_StateMachine(channel, LLC_AMP_LOGICAL_LINK_IND_SUCCESS);
    else
        L2Cap_StateMachine(channel, LLC_AMP_LOGICAL_LINK_IND_FAILED);

    OS_UnlockStack();
    return;

}

void L2Cap_NotifyFromA2MPGetInfoError(BtRemoteDevice *link)
{
    L2CAP_Channel *channel;
    U16 result_code;
    U8 i=0;
    OS_LockStack();
    for(i=0; i< L2CAP_NUM_CHANNELS; i++)
    {
        channel = &L2C(channels)[i];
        /* Check that the channel is in use */
        if (channel->localCid == L2CID_INVALID)
        {
            continue;
        }
        if (channel->link != link)
        {
            continue;
        }        
        if ((channel->state == L2C_OPEN)&&
            (channel->waitPreviousChannelConnected == 1))
        {
            /* Get info not done, the move request will not send out. */
            /* L2CAP channel wait previous channel create logical channel */
            channel->waitPreviousChannelConnected = 0;
            continue;
        }
        if (channel->state == L2C_CONFIG)
        {
            /* Get info not done, the move request will not send out. */
            /* L2CAP channel wait previous channel create logical channel */
            L2Cap_StateMachine(channel, LLC_AMP_LOGICAL_LINK_IND_FAILED);
            continue;
        }
    }

    OS_UnlockStack();
    return;

}

void L2Cap_NotifyFromA2MPAMPPhysicalLinkError(BtRemoteDevice *link)
{
    L2CAP_Channel *channel;
    U16 result_code;
    U8 i=0;
    OS_LockStack();
    for(i=0; i< L2CAP_NUM_CHANNELS; i++)
    {
        channel = &L2C(channels)[i];
        /* Check that the channel is in use */
        if (channel->localCid == L2CID_INVALID)
        {
            continue;
        }
        if (channel->link != link)
        {
            continue;
        }        
        if ((channel->state == L2C_OPEN)&&
            (channel->waitPreviousChannelConnected == 1))
        {
            /* Get info not done, the move request will not send out. */
            /* L2CAP channel wait previous channel create logical channel */
            channel->waitPreviousChannelConnected = 0;
            continue;
        }
    }

    OS_UnlockStack();
    return;

}

void L2Cap_StartRegularCreateAMPChannelTimeoutHandler(EvmTimer *Timer)
{
#if 0
    L2CAP_Channel *channel;
    I8 expired;

    channel = ContainingRecord(Timer, L2CAP_Channel, verifyAmpTimer);
    if(channel->state == L2C_OPEN)
    {
        Report(("L2Cap_StartRegularCreateAMPChannelTimeoutHandler"));
        {
            U32 current_time = 0;

            current_time = OS_GetSystemTime();
        }

        if (!BTCheckPktServicedTimeLong(channel->link->lastTxRxOverDataChannel, MS_TO_TICKS(5000)))
        {
            Report(("Verify AMP channel existing"));
            /* If last tx/rx over amp still less than 5 seconds interval, the amp channel still need to remain */
            if((channel->link->remoteWifiAMP.detected == 1) &&
                (channel->link->remoteWifiAMP.exists == 1) &&
    			(channel->link->remoteWifiAMP.physical_link_sate == BDS_DISCONNECTED))
        {
            L2CAP_Channel *ampChannel;

            channel->waitPreviousChannelConnected = 1;
            Report(("Notify A2MP to create channel again"));

            ampChannel = L2Cap_GetValidChannelFixChannelCid03(channel->link);
            L2C(callbackParms).aclLink = channel->link;
            L2C(callbackParms).ptrs.NotifyA2MPPsmConnected.psm = channel->psmInfo->psm;
            L2Cap_NotifyPsm(ampChannel, L2EVENT_NOTIFY_A2MP_PSM_CONNECTED, BT_STATUS_SUCCESS);
            }
        }
        else
        {
            Report(("Verify The AMP channel is not required"));
            if(channel->link->remoteWifiAMP.physical_link_sate == BDS_CONNECTED)
            {
                /* Disconnect Created Channel */
                BTTest_L2DisconnectAMPCreatedChannel(channel->link);
            }
        }
    }
    Report(("L2Cap_StartRegularCreateAMPChannelTimer"));
    L2Cap_StartRegularCreateAMPChannelTimer(channel, 5000);
#endif
}

void L2Cap_StartRegularCreateAMPChannelTimer(L2CAP_Channel *Channel, U32 timeout)
{
#if 0
    EVM_ResetTimer(&Channel->verifyAmpTimer);
    Channel->verifyAmpTimer.func = L2Cap_StartRegularCreateAMPChannelTimeoutHandler;
    EVM_StartTimer(&Channel->verifyAmpTimer, timeout);
#endif
}

BtSniffInfo ampSnifferInfo;

void L2Cap_StartRegularMoveAMPChannelTimeoutHandler(EvmTimer *Timer)
{
    L2CAP_Channel *channel;
    I8 expired;

    channel = ContainingRecord(Timer, L2CAP_Channel, verifyMoveAmpTimer);
    if(channel->state == L2C_OPEN)
    {
        bt_trace(TRACE_FUNC, L2CAP_REGULAR_MOVE_AMP_PARAM,channel->link->remoteWifiAMP.detected,
            channel->link->remoteWifiAMP.exists,
            channel->link->remoteWifiAMP.physical_link_sate,
            channel->hciAMPHandler);
        if((channel->link->remoteWifiAMP.detected == 1) &&
            (channel->link->remoteWifiAMP.exists == 1) &&
			(channel->link->remoteWifiAMP.physical_link_sate == BDS_CONNECTED) &&
			(channel->hciAMPHandler == 0))
        {
            Report(("Notify upper layer to move channel again"));
            L2C(callbackParms).aclLink = channel->link;        
            L2Cap_NotifyPsm(channel, L2EVENT_NOTIFY_PSM_CHECK_MOVE, BT_STATUS_SUCCESS);
        }
        else if((channel->link->remoteWifiAMP.detected == 1) &&
            (channel->link->remoteWifiAMP.exists == 1) &&
			(channel->link->remoteWifiAMP.physical_link_sate == BDS_CONNECTED) &&
			(channel->hciAMPHandler != 0))
        {
            ampSnifferInfo.maxInterval = 800;
            ampSnifferInfo.minInterval = 300;
            ampSnifferInfo.attempt = 4;
            ampSnifferInfo.timeout = 1;

            // check packet list on the link
            if( NULL != channel->link ){
                if( IsListEmpty(&channel->link->txHighPriorityQueue) &&  IsListEmpty(&channel->link->txLowPriorityQueue) ){
            ME_StartSniff(channel->link, &ampSnifferInfo);          
                }else{
                    // still have data on queue
                    // Report(("sniff check: highqueue empty:%d lowqueue empty:%d ",IsListEmpty(&channel->link->txHighPriorityQueue), IsListEmpty(&channel->link->txLowPriorityQueue)));
                }
            }
        }
#if 0       
        else if((channel->link->remoteWifiAMP.detected == 1) &&
            (channel->link->remoteWifiAMP.exists == 1) &&
			(channel->link->remoteWifiAMP.physical_link_sate == BDS_DISCONNECTED) &&
			(channel->link->remoteWifiAMP.physical_link_sate < BDS_CONNECTED))
        {
            Report(("Trt to create physical link"));
            L2C(callbackParms).aclLink = channel->link;        
            BTA2MP_StartPhysicalLink(channel->link, channel->psmInfo->psm, channel->localCid);
        }
        else if((channel->link->remoteWifiAMP.detected == 1) &&
            (channel->link->remoteWifiAMP.exists == 1) &&
			(channel->link->remoteWifiAMP.physical_link_sate == BDS_CONNECTED) &&
			(channel->hciAMPHandler != 0))
        {
            BTA2MP_PhysicalLinkClose(channel->link);
        }
#endif        
    }
    bt_trace(TRACE_FUNC, L2CAP_REGULAR_MOVE_AMP_TIMER);
    L2Cap_StartRegularMoveAMPChannelTimer(channel, 5000);
}

void L2Cap_StartRegularMoveAMPChannelTimer(L2CAP_Channel *Channel, U32 timeout)
{
    bt_trace(TRACE_FUNC, L2CAP_REGULAR_MOVE_AMP_TIMER);
    EVM_ResetTimer(&Channel->verifyMoveAmpTimer);
    Channel->verifyMoveAmpTimer.func = L2Cap_StartRegularMoveAMPChannelTimeoutHandler;
    EVM_StartTimer(&Channel->verifyMoveAmpTimer, timeout);
}

void L2CAP_NotifyLogicalLinkDisconnected(BtRemoteDevice *link, U16 l2cap_id)
{
    L2CAP_Channel *channel;
    U8 i=0;
    
    OS_LockStack();
    for(i=0; i< L2CAP_NUM_CHANNELS; i++)
    {
        channel = &L2C(channels)[i];
        /* Check that the channel is in use */
        if (channel->localCid == L2CID_INVALID)
        {
            continue;
        }
        if (channel->link != link)
        {
            continue;
        }       
        if(channel->state == L2C_CLOSED)
        {
            continue;
        }
        if(channel->localCid == l2cap_id)
            break;
    }
    if(i == L2CAP_NUM_CHANNELS)
    {
        return;
    }
    if(channel->link->state == BDS_CONNECTED)
    {
        L2C(callbackParms).aclLink = channel->link;
        L2Cap_NotifyPsm(channel, L2EVENT_NOTIFY_PSM_MOVE_BACK_TO_BR, BT_STATUS_SUCCESS);
    }

    OS_UnlockStack();
    return;

}

void L2CAP_NotifyAMPChannelNotAvailable(BtRemoteDevice *link, U8 controllerId)
{
    L2CAP_Channel *channel;
    U8 i=0;

    OS_LockStack();
    for(i=0; i< L2CAP_NUM_CHANNELS; i++)
    {
        channel = &L2C(channels)[i];
        /* Check that the channel is in use */
        if (channel->localCid == L2CID_INVALID)
        {
            continue;
        }
        if (channel->link != link)
        {
            continue;
        }
        if(channel->state == L2C_CLOSED)
        {
            continue;
        }
        if(channel->currentAmpControllerId == controllerId)
        {
            if(channel->link->state == BDS_CONNECTED)
            {
                L2C(callbackParms).aclLink = channel->link;
                L2Cap_NotifyPsm(channel, L2EVENT_NOTIFY_PSM_MOVE_BACK_TO_BR, BT_STATUS_SUCCESS);
            }
        }
    }
    if(i == L2CAP_NUM_CHANNELS)
    {
        return;
    }

    OS_UnlockStack();
    return;

}


void L2CAP_NotifyLogicalLinkNotAvailable(BtRemoteDevice *link, U16 l2cap_id)
{
    L2CAP_Channel *channel;
    U8 i=0;

    OS_LockStack();
    for(i=0; i< L2CAP_NUM_CHANNELS; i++)
    {
        channel = &L2C(channels)[i];
        /* Check that the channel is in use */
        if (channel->localCid == L2CID_INVALID)
        {
            continue;
        }
        if (channel->link != link)
        {
            continue;
        }
        if(channel->localCid == l2cap_id)
            break;
    }
    if(i == L2CAP_NUM_CHANNELS)
    {
        return;
    }
    if(channel->link->state == BDS_CONNECTED)
    {
        L2C(callbackParms).aclLink = channel->link;        
        L2Cap_NotifyPsm(channel, L2EVENT_NOTIFY_PSM_MOVE_BACK_TO_BR, BT_STATUS_SUCCESS);
    }
        
    OS_UnlockStack();
    return;

}


void L2CAP_AssignChannelAMPHandler(L2CAP_Channel *channel, U8 controllerId)
{
    U8 i=0;

    Report(("L2CAP_AssignChannelAMPHandler ctrid:%d", controllerId));
    if(controllerId == 0x00)
    {
        channel->hciAMPHandler = 0;
    }
    else
    {
        for(i=0; i< BT_AMP_SUPPORTED_LOGICAL_CHANNEL;i++)
        {
            Report(("L2CAP_AssignChannelAMPHandler a:%d",channel->link->remoteWifiAMP.logicalLink[i].logical_link_sate));
            Report(("L2CAP_AssignChannelAMPHandler b:%d",channel->link->remoteWifiAMP.logicalLink[i].l2cap_id));
            Report(("L2CAP_AssignChannelAMPHandler c:%d",channel->localCid));
            Report(("L2CAP_AssignChannelAMPHandler d:%d",channel->link->remoteWifiAMP.logicalLink[i].logical_link_handle));


            if((channel->link->remoteWifiAMP.logicalLink[i].logical_link_sate != BDS_DISCONNECTED) &&
                (channel->link->remoteWifiAMP.logicalLink[i].l2cap_id == channel->localCid))
            {
                Report(("L2CAP_AssignChannelAMPHandler:%d",channel->link->remoteWifiAMP.logicalLink[i].logical_link_handle));
                channel->hciAMPHandler = channel->link->remoteWifiAMP.logicalLink[i].logical_link_handle;
                break;
            }
        }
    }
}

void L2CAP_DisconnectL2CAPCreatedPALChannel(BtRemoteDevice *link)
{
    L2CAP_Channel *channel;
    U8 i=0;

    OS_LockStack();
    for(i=0; i< L2CAP_NUM_CHANNELS; i++)
    {
        channel = &L2C(channels)[i];
        /* Check that the channel is in use */
        if (channel->localCid == L2CID_INVALID)
        {
            continue;
        }
        if (channel->link != link)
        {
            continue;
        }
    }

    OS_UnlockStack();
    return;

}



void BTTest_L2DisconnectAMPCreatedChannel(BtRemoteDevice *link)
{
    L2CAP_Channel *channel;
    U8 i=0;

    OS_LockStack();
    for(i=0; i< L2CAP_NUM_CHANNELS; i++)
    {
        channel = &L2C(channels)[i];
        /* Check that the channel is in use */
        if (channel->localCid == L2CID_INVALID)
        {
            continue;
        }
        if((channel->state == L2C_OPEN) &&
            (channel->link == link))
        {
            L2CAP_DisconnectReq(channel->localCid);
            return;
        }
    }
    OS_UnlockStack();
    return;
}



void BTTest_L2DisconnectAMPLink(void)
{
    L2CAP_Channel *channel;
    U8 i=0;

    OS_LockStack();
    for(i=0; i< L2CAP_NUM_CHANNELS; i++)
    {
        channel = &L2C(channels)[i];
        /* Check that the channel is in use */
        if (channel->localCid == L2CID_INVALID)
        {
            continue;
        }
        if((channel->state == L2C_OPEN))
        {
            A2MP_DisconnectAMPChannel(channel->link, channel->localCid);
        }
    }
    if(i == L2CAP_NUM_CHANNELS)
    {
        return;
    }

    OS_UnlockStack();
    return;
}


void BTTest_L2DisconnectAMPChannel(void)
{
    L2CAP_Channel *channel;
    U8 i=0;

    OS_LockStack();
    for(i=0; i< L2CAP_NUM_CHANNELS; i++)
    {
        channel = &L2C(channels)[i];
        /* Check that the channel is in use */
        if (channel->localCid == L2CID_INVALID)
        {
            continue;
        }
        if((channel->state == L2C_OPEN))
        {
            BTA2MP_PhysicalLinkClose(channel->link);
        }
    }
    if(i == L2CAP_NUM_CHANNELS)
    {
        return;
    }

    OS_UnlockStack();
    return;
}

void L2Cap_AMPPhysicalLinkCompleted(BtRemoteDevice *link, U16 local_cid)
{
    L2CAP_Channel *channel;
    U16 result_code;
    U8 i=0;

    OS_LockStack();
    Report(("L2Cap_AMPPhysicalLinkCompleted: cid:%d",local_cid));
    channel = &L2C(channels)[CID_TO_INDEX(local_cid)];
    Report(("setupByCreateChannel: %d",channel->setupByCreateChannel));
    Report(("channel: cid:%d",channel->localCid));

    if(channel->setupByCreateChannel == 1)
    {
        return;
    }

    if(channel->localCid == local_cid)
    {
        L2C(callbackParms).aclLink = channel->link;
        L2Cap_NotifyPsm(channel, L2EVENT_READY_TO_MOVE, BT_STATUS_SUCCESS);
        return;
    }

    OS_UnlockStack();
    return;

}


#endif 



#endif




