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
#include "btconfig.h"
#include "utils.h"
#include "meamp.h"
#include "a2mp_adp.h"

#if A2MP_PROTOCOL == XA_ENABLED
extern A2MP_MAIN_CONN *A2MP_FindMainChannel(BtRemoteDevice *link);
void BTA2MP_ParseControllerList(BtRemoteDevice *link, U8 size, U8 *data);
void BTA2MP_HandleAMPAbnormal(BtRemoteDevice *link, U8 event);
BtStatus BTA2MP_PhysicalLinkCreate(BtRemoteDevice *link);
BtStatus BTA2MP_LogicalLinkCreate(U16 L2CapId,
                                          U8 createOrAccept,
                                          BtRemoteDevice *link,
                                          U8 controllerId);

U8 BTA2MP_ParseControllerListAndAvailable(BtRemoteDevice *link, U8 size, U8 *data, U8 controllerid);
U8 A2MP_GetAMPControllerStatus(U8 controllerId);
BtStatus BTA2MP_SendDisconnectPhysicalLinkRequest(BtRemoteDevice *link, BtA2MPDisconnectPhysicalLinkReq *a2mp_parm);

#define A2MP_GetNextIdent()   (A2MPC(nextIdent) == 0 ? A2MPC(nextIdent)=2,1 : A2MPC(nextIdent)++)
extern U8 A2MP_GetControllerIdByIndex(U8 index);
extern L2capExtendedFlowSpecOption *L2Cap_GetInExtendedFlowSpec(U16 l2cap_id);
extern L2capExtendedFlowSpecOption *L2Cap_GetOutExtendedFlowSpec(U16 l2cap_id);
extern U8 A2MP_GetLogicalBestEffortIndex(BtRemoteDevice *link);
extern U8 A2MP_GetLogicalBestEffortIndex(BtRemoteDevice *link);
extern A2MP_CONN *A2MP_AllocatePSMChannel(BtRemoteDevice *link, U16 psm);
extern BTRemoteAMPLogicalLink *A2MP_FindLogicalLinkByHdl(BtRemoteAMPDevice *link, U16 logical_link_hdl);
extern void L2CAP_DisconnectL2CAPCreatedPALChannel(BtRemoteDevice *link);
extern void L2CAP_NotifyAMPChannelNotAvailable(BtRemoteDevice *link, U8 controllerId);

BtA2MPPacket *BtA2MP_GetFreeTxPacket(A2MP_MAIN_CONN *channel)
{
	BtA2MPPacket *txPacket = 0;
	if (IsListEmpty(&(channel->freeTtxPacketsList)) == 0)
		txPacket = (BtA2MPPacket *)RemoveHeadList(&(channel->freeTtxPacketsList));
	if	(txPacket->btPacket.flags & BTP_FLAG_LSYSTEM)
	{
        Assert(0);
	}
	return txPacket;
}


void A2MP_TimerHandler(EvmTimer* timer)
{
	A2MP_MAIN_CONN *channel;

	channel = (A2MP_MAIN_CONN *)timer->context;
    if((channel->state != BT_A2MP_CONTROLLER_STATE_IDLE) &&
        (channel->remDev !=0) &&
        (channel->remDev->state == BDS_CONNECTED))
    {
        channel->remDev->disconnectACLCheckCreatACLAlwaysForce = 0;
        MeDisconnectLink(channel->remDev,15);
    }
}

void A2MP_StartTimer(A2MP_MAIN_CONN *channel)
{
	EVM_ResetTimer(&channel->timer);
	channel->timer.func = A2MP_TimerHandler;
	channel->timer.context = channel;
	EVM_StartTimer(&channel->timer, 20000);
}

void A2MP_StopTimer(A2MP_MAIN_CONN *channel)
{
	EVM_ResetTimer(&channel->timer);
}


BtStatus BTA2MP_CommandReject(BtRemoteDevice *link, BtA2MPCommandReject *a2mp_parm)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	BtA2MPPacket *txPacket = 0;

	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
		return BT_STATUS_FAILED;
	a2mp_main_channel->localIdent = A2MP_GetNextIdent();
	txPacket = BtA2MP_GetFreeTxPacket(a2mp_main_channel);
	if(txPacket != 0)
	{
	    txPacket->data_buff[0] = A2MP_CODE_COMMAND_REJECT;
	    txPacket->data_buff[1] = a2mp_main_channel->localIdent;
	    txPacket->data_buff[2] = (U8)(a2mp_parm->dataSize +2);
	    txPacket->data_buff[3] = (U8)((a2mp_parm->dataSize +2) >> 8);
	    txPacket->data_buff[4] = a2mp_parm->reason;
	    txPacket->data_buff[5] = a2mp_parm->reason >> 8;        
		if(a2mp_parm->dataSize !=0)
			memcpy(txPacket->data_buff + 6, a2mp_parm->data, a2mp_parm->dataSize);
		txPacket->btPacket.data = txPacket->data_buff;
	    txPacket->btPacket.dataLen = a2mp_parm->dataSize +6;
		A2MP_SendData(link, &txPacket->btPacket);
        return BT_STATUS_SUCCESS;
	}
	else
		return BT_STATUS_FAILED;

}

BtStatus BTA2MP_SendDiscoveryRequest(BtRemoteDevice *link, BtA2MPDiscoveryReq *a2mp_parm)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	BtA2MPPacket *txPacket = 0;
	U16 length;

	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
		return BT_STATUS_FAILED;
    if(a2mp_main_channel->localIdent !=0)
    {
        Assert(0);
    }

	a2mp_main_channel->localIdent = A2MP_GetNextIdent();
	if(a2mp_parm->extendedFeatureMaskSize > (A2MP_MAX_TX_BUFFER -6))
	{
		return BT_STATUS_FAILED;
	}
	txPacket = BtA2MP_GetFreeTxPacket(a2mp_main_channel);
	if(txPacket != 0)
	{
	    a2mp_main_channel->last_opcode = A2MP_CODE_DISCOVER_REQ;
		length = 2 + a2mp_parm->extendedFeatureMaskSize;
        Report(("TX A2MP_CODE_DISCOVER_REQ"));
	    txPacket->data_buff[0] = A2MP_CODE_DISCOVER_REQ;
	    txPacket->data_buff[1] = a2mp_main_channel->localIdent;
	    txPacket->data_buff[2] = (U8)(length);
	    txPacket->data_buff[3] = (U8) (length >> 8);
	    txPacket->data_buff[4] = (U8)(A2MP_MAX_TX_BUFFER);
	    txPacket->data_buff[5] = (U8)((A2MP_MAX_TX_BUFFER) >> 8);
		memcpy(txPacket->data_buff+6, a2mp_parm->extendedFeatureMask, a2mp_parm->extendedFeatureMaskSize);
		txPacket->btPacket.data = txPacket->data_buff;
	    txPacket->btPacket.dataLen = length + 4;
		A2MP_StartTimer(a2mp_main_channel);
		A2MP_SendData(link, &(txPacket->btPacket));
        return BT_STATUS_SUCCESS;
	}
	else
		return BT_STATUS_FAILED;

}

BtStatus BTA2MP_SendDiscoveryResponse(BtRemoteDevice *link, BtA2MPDiscoveryRsp *a2mp_parm)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	BtA2MPPacket *txPacket = 0;
	U16 length = 0;

	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
		return BT_STATUS_FAILED;

	if((a2mp_parm->extendedFeatureMaskSize + a2mp_parm->controllerListSize) > (A2MP_MAX_TX_BUFFER -6))
	{
		return BT_STATUS_FAILED;
	}

    a2mp_main_channel->last_rxCmdopcode = A2MP_CODE_INVALID;
	txPacket = BtA2MP_GetFreeTxPacket(a2mp_main_channel);
	if(txPacket != 0)
	{
		length = 2+ a2mp_parm->extendedFeatureMaskSize + a2mp_parm->controllerListSize;
        Report(("TX A2MP_CODE_DISCOVER_RSP"));

	    txPacket->data_buff[0] = A2MP_CODE_DISCOVER_RSP;
	    txPacket->data_buff[1] = a2mp_main_channel->remoteIdent;
	    txPacket->data_buff[2] = (U8)(length);
	    txPacket->data_buff[3] = (U8)(length >> 8);
	    txPacket->data_buff[4] = (U8)(A2MP_MAX_TX_BUFFER);
	    txPacket->data_buff[5] = (U8)((A2MP_MAX_TX_BUFFER) >> 8);
		memcpy(txPacket->data_buff+6, a2mp_parm->extendedFeatureMask, a2mp_parm->extendedFeatureMaskSize);
		memcpy(txPacket->data_buff+6+a2mp_parm->extendedFeatureMaskSize, a2mp_parm->controllerList, a2mp_parm->controllerListSize);
		txPacket->btPacket.data = txPacket->data_buff;
	    txPacket->btPacket.dataLen = length + 4;
		A2MP_SendData(link, &txPacket->btPacket);
        return BT_STATUS_SUCCESS;
	}
	else
		return BT_STATUS_FAILED;

}

BtStatus BTA2MP_SendChangeNotify(BtRemoteDevice *link, BtA2MPChangeNotify *a2mp_parm)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	BtA2MPPacket *txPacket = 0;
	U16 length = 0;

	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
		return BT_STATUS_FAILED;
    if(a2mp_main_channel->localIdent !=0)
    {
        Assert(0);
    }

	a2mp_main_channel->localIdent = A2MP_GetNextIdent();
	if(a2mp_parm->controllerListSize > (A2MP_MAX_TX_BUFFER -6))
	{
		return BT_STATUS_FAILED;
	}
	txPacket = BtA2MP_GetFreeTxPacket(a2mp_main_channel);
	if(txPacket != 0)
	{
		length = a2mp_parm->controllerListSize;
        Report(("TX A2MP_CODE_CHANGE_NOTIFY"));
	    a2mp_main_channel->last_opcode = A2MP_CODE_CHANGE_NOTIFY;
	    txPacket->data_buff[0] = A2MP_CODE_CHANGE_NOTIFY;
	    txPacket->data_buff[1] = a2mp_main_channel->localIdent;
	    txPacket->data_buff[2] = (U8)(length);
	    txPacket->data_buff[3] = (U8)(length >> 8);
		memcpy(txPacket->data_buff+4, a2mp_parm->controllerList, a2mp_parm->controllerListSize);
		txPacket->btPacket.data = txPacket->data_buff;
	    txPacket->btPacket.dataLen = length + 4;
		A2MP_StartTimer(a2mp_main_channel);
		A2MP_SendData(link, &txPacket->btPacket);
        return BT_STATUS_SUCCESS;
	}
	else
		return BT_STATUS_FAILED;

}

BtStatus BTA2MP_SendChangeResponse(BtRemoteDevice *link)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	BtA2MPPacket *txPacket = 0;
	U16 length = 0;

	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
		return BT_STATUS_FAILED;
    if(a2mp_main_channel->localIdent !=0)
    {
        Assert(0);
    }

	txPacket = BtA2MP_GetFreeTxPacket(a2mp_main_channel);
	if(txPacket != 0)
	{
        Report(("TX A2MP_CODE_CHANGE_RSP"));
	    txPacket->data_buff[0] = A2MP_CODE_CHANGE_RSP;
	    txPacket->data_buff[1] = a2mp_main_channel->remoteIdent;
	    txPacket->data_buff[2] = 0x00;
	    txPacket->data_buff[3] = 0x00;
		txPacket->btPacket.data = txPacket->data_buff;
	    txPacket->btPacket.dataLen = 4;
		A2MP_SendData(link, &txPacket->btPacket);
        return BT_STATUS_SUCCESS;
	}
	else
		return BT_STATUS_FAILED;

}

BtStatus BTA2MP_SendGetInfoRequest(BtRemoteDevice *link, BtA2MPGetInfoReq *a2mp_parm)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	BtA2MPPacket *txPacket = 0;
	U16 length = 0;

	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
		return BT_STATUS_FAILED;
    if(a2mp_main_channel->localIdent !=0)
    {
        Assert(0);
    }

	a2mp_main_channel->localIdent = A2MP_GetNextIdent();
	txPacket = BtA2MP_GetFreeTxPacket(a2mp_main_channel);
	if(txPacket != 0)
	{
	    a2mp_main_channel->last_opcode = A2MP_CODE_GET_INFO_REQ;
        Report(("TX A2MP_CODE_GET_INFO_REQ"));

	    txPacket->data_buff[0] = A2MP_CODE_GET_INFO_REQ;
	    txPacket->data_buff[1] = a2mp_main_channel->localIdent;
	    txPacket->data_buff[2] = 0x01;
	    txPacket->data_buff[3] = 0x00;
	    txPacket->data_buff[4] = a2mp_parm->controllerId;
		txPacket->btPacket.data = txPacket->data_buff;
	    txPacket->btPacket.dataLen = 5;
		A2MP_StartTimer(a2mp_main_channel);
		A2MP_SendData(link, &txPacket->btPacket);
        return BT_STATUS_SUCCESS;
	}
	else
		return BT_STATUS_FAILED;

}

BtStatus BTA2MP_SendGetInfoResponse(BtRemoteDevice *link, BtA2MPGetInfoRsp *a2mp_parm)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	BtA2MPPacket *txPacket = 0;
	U16 length;

	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
		return BT_STATUS_FAILED;
    a2mp_main_channel->last_rxCmdopcode = A2MP_CODE_INVALID;
	txPacket = BtA2MP_GetFreeTxPacket(a2mp_main_channel);
	if(txPacket != 0)
	{
        Report(("TX A2MP_CODE_GET_INFO_RSP"));
		length = 18;
		txPacket->data_buff[0] = A2MP_CODE_GET_INFO_RSP;
		txPacket->data_buff[1] = a2mp_main_channel->remoteIdent;
		txPacket->data_buff[2] = (U8)(length);
		txPacket->data_buff[3] = (U8)(length >> 8);
		txPacket->data_buff[4] = (U8)(a2mp_parm->controllerId);
		txPacket->data_buff[5] = (U8)(a2mp_parm->status);
		StoreLE32(txPacket->data_buff+6, a2mp_parm->total_bandwidth);
		StoreLE32(txPacket->data_buff+10, a2mp_parm->max_guarantee_bandwidth);
		StoreLE32(txPacket->data_buff+14, a2mp_parm->min_latency);
		StoreLE16(txPacket->data_buff+18, a2mp_parm->pal_capability);
		StoreLE16(txPacket->data_buff+20, a2mp_parm->max_amp_assoc_length);
		txPacket->btPacket.data = txPacket->data_buff;
	    txPacket->btPacket.dataLen = length + 4;
		A2MP_SendData(link, &txPacket->btPacket);
        return BT_STATUS_SUCCESS;
	}
	else
		return BT_STATUS_FAILED;
}

BtStatus BTA2MP_SendGetAMPAssocRequest(BtRemoteDevice *link, BtA2MPGetAMPAssocReq *a2mp_parm)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	BtA2MPPacket *txPacket = 0;
	U16 length = 0;

	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
		return BT_STATUS_FAILED;
    if(a2mp_main_channel->localIdent !=0)
    {
        Assert(0);
    }
	a2mp_main_channel->localIdent = A2MP_GetNextIdent();
	txPacket = BtA2MP_GetFreeTxPacket(a2mp_main_channel);
	if(txPacket != 0)
	{
        Report(("TX A2MP_CODE_GET_AMP_ASSOC_REQ"));
	    a2mp_main_channel->last_opcode = A2MP_CODE_GET_AMP_ASSOC_REQ;
	    txPacket->data_buff[0] = A2MP_CODE_GET_AMP_ASSOC_REQ;
	    txPacket->data_buff[1] = a2mp_main_channel->localIdent;
	    txPacket->data_buff[2] = 0x01;
	    txPacket->data_buff[3] = 0x00;
	    txPacket->data_buff[4] = a2mp_parm->controllerId;
		txPacket->btPacket.data = txPacket->data_buff;
	    txPacket->btPacket.dataLen = 5;
		A2MP_StartTimer(a2mp_main_channel);
		A2MP_SendData(link, &txPacket->btPacket);
        return BT_STATUS_SUCCESS;
	}
	else
		return BT_STATUS_FAILED;


}

BtStatus BTA2MP_SendGetAMPAssocResponse(BtRemoteDevice *link, BtA2MPGetAMPAssocRsp *a2mp_parm)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	BtA2MPPacket *txPacket = 0;
	U16 length;

	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
		return BT_STATUS_FAILED;
    a2mp_main_channel->last_rxCmdopcode = A2MP_CODE_INVALID;
	txPacket = BtA2MP_GetFreeTxPacket(a2mp_main_channel);
	if(txPacket != 0)
	{
        Report(("TX A2MP_CODE_GET_AMP_ASSOC_RSP"));
		length = 2 + a2mp_parm->ampAssocSize;
		txPacket->data_buff[0] = A2MP_CODE_GET_AMP_ASSOC_RSP;
		txPacket->data_buff[1] = a2mp_main_channel->remoteIdent;
		txPacket->data_buff[2] = (U8)(length);
		txPacket->data_buff[3] = (U8)(length >> 8);
		txPacket->data_buff[4] = (U8)(a2mp_parm->controllerId);
		txPacket->data_buff[5] = (U8)(a2mp_parm->status);
		memcpy(txPacket->data_buff + 6, a2mp_parm->ampAssoc, a2mp_parm->ampAssocSize);
		txPacket->btPacket.data = txPacket->data_buff;
	    txPacket->btPacket.dataLen = length + 4;

		A2MP_SendData(link, &txPacket->btPacket);
        return BT_STATUS_SUCCESS;
	}
	else
		return BT_STATUS_FAILED;

}

BtStatus BTA2MP_SendCreatePhysicalLinkRequest(BtRemoteDevice *link, BtA2MPCreatePhysicalLinkReq *a2mp_parm)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	BtA2MPPacket *txPacket = 0;
	U16 length;

	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
		return BT_STATUS_FAILED;
    if(a2mp_main_channel->localIdent !=0)
    {
        Assert(0);
    }

	a2mp_main_channel->localIdent = A2MP_GetNextIdent();
	if((a2mp_parm->ampAssocSize + 2) > (A2MP_MAX_TX_BUFFER -4))
	{
		return BT_STATUS_FAILED;
	}
	txPacket = BtA2MP_GetFreeTxPacket(a2mp_main_channel);
	if(txPacket != 0)
	{
		length = a2mp_parm->ampAssocSize + 2;
        Report(("TX A2MP_CODE_CREATE_PHY_LINK_REQ"));
	    a2mp_main_channel->last_opcode = A2MP_CODE_CREATE_PHY_LINK_REQ;
		txPacket->data_buff[0] = A2MP_CODE_CREATE_PHY_LINK_REQ;
		txPacket->data_buff[1] = a2mp_main_channel->localIdent;
		txPacket->data_buff[2] = (U8)(length);
		txPacket->data_buff[3] = (U8)(length >> 8);
		txPacket->data_buff[4] = (U8)(a2mp_parm->localControllerId);
		txPacket->data_buff[5] = (U8)(a2mp_parm->remoteControllerId);
		memcpy(txPacket->data_buff + 6, a2mp_parm->ampAssoc, a2mp_parm->ampAssocSize);
		txPacket->btPacket.data = txPacket->data_buff;
	    txPacket->btPacket.dataLen = length + 4;
		A2MP_StartTimer(a2mp_main_channel);
		A2MP_SendData(link, &txPacket->btPacket);
        return BT_STATUS_SUCCESS;
	}
	else
		return BT_STATUS_FAILED;


}

BtStatus BTA2MP_SendCreatePhysicalLinkResponse(BtRemoteDevice *link, BtA2MPCreatePhysicalLinkRsp *a2mp_parm)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	BtA2MPPacket *txPacket = 0;
	U16 length = 0;

	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
		return BT_STATUS_FAILED;
    a2mp_main_channel->last_rxCmdopcode = A2MP_CODE_INVALID;

	txPacket = BtA2MP_GetFreeTxPacket(a2mp_main_channel);
	if(txPacket != 0)
	{
        Report(("TX A2MP_CODE_CREATE_PHY_LINK_RSP"));
	    txPacket->data_buff[0] = A2MP_CODE_CREATE_PHY_LINK_RSP;
	    txPacket->data_buff[1] = a2mp_main_channel->remoteIdent;
	    txPacket->data_buff[2] = 0x03;
	    txPacket->data_buff[3] = 0x00;
		txPacket->data_buff[4] = (U8)(a2mp_parm->localControllerId);
		txPacket->data_buff[5] = (U8)(a2mp_parm->remoteControllerId);
		txPacket->data_buff[6] = (U8)(a2mp_parm->status);
		txPacket->btPacket.data = txPacket->data_buff;
	    txPacket->btPacket.dataLen = 7;

		A2MP_SendData(link, &txPacket->btPacket);
        return BT_STATUS_SUCCESS;
	}
	else
		return BT_STATUS_FAILED;

}

BtStatus BTA2MP_SendDisconnectPhysicalLinkRequest(BtRemoteDevice *link, BtA2MPDisconnectPhysicalLinkReq *a2mp_parm)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	BtA2MPPacket *txPacket = 0;
	U16 length = 0;

	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
		return BT_STATUS_FAILED;

	a2mp_main_channel->localIdent = A2MP_GetNextIdent();
	txPacket = BtA2MP_GetFreeTxPacket(a2mp_main_channel);
    L2CAP_NotifyAMPChannelNotAvailable(link, a2mp_main_channel->localControllerId);
    L2CAP_DisconnectL2CAPCreatedPALChannel(link);
	if(txPacket != 0)
	{
        Report(("TX A2MP_CODE_DISC_PHY_LINK_REQ"));
	    a2mp_main_channel->last_opcode = A2MP_CODE_DISC_PHY_LINK_REQ;
		txPacket->data_buff[0] = A2MP_CODE_DISC_PHY_LINK_REQ;
		txPacket->data_buff[1] = a2mp_main_channel->localIdent;
		txPacket->data_buff[2] = 0x02;
		txPacket->data_buff[3] = 0x00;
		txPacket->data_buff[4] = a2mp_parm->localControllerId;
		txPacket->data_buff[5] = a2mp_parm->remoteControllerId;
		txPacket->btPacket.data = txPacket->data_buff;
		txPacket->btPacket.dataLen = 6;
        ME_DisconnectPhysicalLink(&link->remoteWifiAMP, 0x16);
		A2MP_StartTimer(a2mp_main_channel);
		A2MP_SendData(link, &txPacket->btPacket);
        return BT_STATUS_SUCCESS;
	}
	else
		return BT_STATUS_FAILED;
}

BtStatus BTA2MP_SendDisconnectPhysicalLinkResponse(BtRemoteDevice *link, BtA2MPDisconnectPhysicalLinkRsp *a2mp_parm)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	BtA2MPPacket *txPacket = 0;
	U16 length = 0;

	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
		return BT_STATUS_FAILED;

    a2mp_main_channel->last_rxCmdopcode = A2MP_CODE_INVALID;
    L2CAP_NotifyAMPChannelNotAvailable(link, a2mp_main_channel->localControllerId);
    L2CAP_DisconnectL2CAPCreatedPALChannel(link);
	txPacket = BtA2MP_GetFreeTxPacket(a2mp_main_channel);
	if(txPacket != 0)
	{
        Report(("TX A2MP_CODE_DISC_PHY_LINK_RSP"));
		txPacket->data_buff[0] = A2MP_CODE_DISC_PHY_LINK_RSP;
		txPacket->data_buff[1] = a2mp_main_channel->remoteIdent;
		txPacket->data_buff[2] = 0x03;
		txPacket->data_buff[3] = 0x00;
		txPacket->data_buff[4] = a2mp_parm->localControllerId;
		txPacket->data_buff[5] = a2mp_parm->remoteControllerId;
		txPacket->data_buff[6] = a2mp_parm->status;
		txPacket->btPacket.data = txPacket->data_buff;
		txPacket->btPacket.dataLen = 7;
		A2MP_SendData(link, &txPacket->btPacket);
        return BT_STATUS_SUCCESS;
	}
	else
		return BT_STATUS_FAILED;

}


BtStatus BTA2MP_HandleCommandReject(BtRemoteDevice *link, A2MP_MAIN_CONN *a2mp_main_channel, U8 *data, U16 length)
{
	Report(("last_opcode failed",a2mp_main_channel->last_opcode));
	MeDisconnectLink(a2mp_main_channel->remDev,16);
    return BT_STATUS_SUCCESS;
}

void BTA2MP_HandleDiscoveryRequest(BtRemoteDevice *link, A2MP_MAIN_CONN *a2mp_main_channel, U8 *data, U16 length)
{
	BtA2MPDiscoveryRsp a2mp_rsp_parm;
    U8 extendedFeatureMask[2] = { 0x00, 0x00};

    Report(("BTA2MP_HandleDiscoveryRequest"));
    a2mp_main_channel->tx_mtu = LEtoHost16(data);
//	a2mp_parm.extendedFeatureMaskSize = length - 2;
//	a2mp_parm.extendedFeatureMask = data + 2;
    a2mp_rsp_parm.mtu = A2MP_MAX_FRAME_SIZE;
    a2mp_rsp_parm.extendedFeatureMaskSize = 0x02;
    a2mp_rsp_parm.extendedFeatureMask = (U8*)extendedFeatureMask;
    a2mp_rsp_parm.controllerList = A2MPC(controllerList);
    a2mp_rsp_parm.controllerListSize= A2MPC(controllerListSize);

    BTA2MP_SendDiscoveryResponse(link, &a2mp_rsp_parm);
}

void BTA2MP_HandleDiscoveryResponse(BtRemoteDevice *link, A2MP_MAIN_CONN *a2mp_main_channel, U8 *data, U16 length)
{
	BtA2MPDiscoveryRsp a2mp_parm;
    U8 ampStatus;
    U8 i=0;

	a2mp_parm.mtu = LEtoHost16(data);
    a2mp_main_channel->tx_mtu = LEtoHost16(data);
	a2mp_parm.extendedFeatureMaskSize = A2MP_GetExtendedFeatureMaskLength(data+2);
	if(a2mp_parm.extendedFeatureMaskSize > 30)
	{
		/* too large */
		MeDisconnectLink(link,17);
		return;
	}
	a2mp_parm.extendedFeatureMask = data + 2;
	a2mp_parm.controllerListSize = length - 2  - a2mp_parm.extendedFeatureMaskSize;
	a2mp_parm.controllerList = data + 2 + a2mp_parm.extendedFeatureMaskSize;
    a2mp_main_channel->discoverDone = 1;
    BTA2MP_ParseControllerList(link, a2mp_parm.controllerListSize,a2mp_parm.controllerList);
    Report(("A2MP index i:%d",i));
    switch (link->remoteWifiAMP.ampStatus)
    {
        case 0x00: /* Power down */
        case 0x02: /* Not available */
            /* handle the AMP is not able to create */
            if(link->remoteWifiAMP.initiator == 1)
                BTA2MP_HandleAMPAbnormal(link, A2MP_PHYSICAL_LINK_CREATE_FAILED_ERROR);
            break;
        default:
            if(link->remoteWifiAMP.initiator == 1)
                BTA2MP_PhysicalLinkCreate(link);
            break;
    }
}

void BTA2MP_HandleChangeNotify(BtRemoteDevice *link, A2MP_MAIN_CONN *a2mp_main_channel, U8 *data, U16 length)
{
	BtA2MPChangeNotify a2mp_parm;
    U8 ampStatus;

	a2mp_parm.controllerList = data;
	a2mp_parm.controllerListSize = length;
    ampStatus = BTA2MP_ParseControllerListAndAvailable(link, length, data, a2mp_main_channel->localControllerId);
    Report(("AMP status change to :%d", ampStatus));
    switch(ampStatus)
    {
        case 0x00: /* Physical power down. */
        case 0x02: /* No capability for BT */
            L2CAP_NotifyAMPChannelNotAvailable(link, 1);
        default:
            break;
    }
    BTA2MP_SendChangeResponse(link);
}

void BTA2MP_HandleChangeResponse(BtRemoteDevice *link, A2MP_MAIN_CONN *a2mp_main_channel)
{
}

void BTA2MP_HandleGetInfoRequest(BtRemoteDevice *link, A2MP_MAIN_CONN *a2mp_main_channel,  U8 *data, U16 length)
{
	U8 controllerId;
    U8 i=0;
    U8 validController = 0;
    BtA2MPGetInfoRsp a2mp_parm;
    U8 controllerNo = BT_PAL_CONTROLLER_LIST_SIZE/3;
	controllerId = data[0];
    Report(("BTA2MP_HandleGetInfoRequest"));
    if(controllerId !=0)
    {
        for(i=0;i<controllerNo; i++)
        {
            if(A2MP_GetControllerIdByIndex(i*3) == controllerId)
            {
                validController = 1;
                break;
            }
        }
    }
    if((validController == 0) || (a2mp_main_channel ==0) ||
        (a2mp_main_channel->last_rxCmdopcode != A2MP_CODE_INVALID))
    {
        OS_MemSet((U8 *)&a2mp_parm, 0, sizeof(BtA2MPGetInfoRsp));
        a2mp_parm.controllerId = controllerId;
        a2mp_parm.status = A2MP_GetAMPControllerStatus(controllerId);
        a2mp_parm.total_bandwidth = 0;
        a2mp_parm.max_guarantee_bandwidth = 0;
        a2mp_parm.min_latency = 0;
        a2mp_parm.pal_capability = 0;
        a2mp_parm.max_amp_assoc_length = 0;
        BTA2MP_SendGetInfoResponse(link, &a2mp_parm);
    }
    else
    {
        a2mp_main_channel->last_rxCmdopcode = A2MP_CODE_GET_INFO_REQ;
        ME_ReadLocalAMPInfo(&link->remoteWifiAMP);
        /* Use HCI command to request the AMP Info */
    }
}

void BTA2MP_HandleGetInfoResponse(BtRemoteDevice *link, A2MP_MAIN_CONN *a2mp_main_channel, U8 *data, U16 length)
{
	BtA2MPGetInfoRsp a2mp_parm;

	a2mp_parm.controllerId = data[0];
	a2mp_parm.status = data[1];
    if(a2mp_parm.status == 0x00)
    {
        a2mp_parm.total_bandwidth= LEtoHost32(data+2);
        a2mp_parm.max_guarantee_bandwidth= LEtoHost32(data+6);
        a2mp_parm.min_latency= LEtoHost32(data+10);
        a2mp_parm.pal_capability= LEtoHost16(data+14);
        a2mp_parm.max_amp_assoc_length = LEtoHost16(data+16);
        if ((a2mp_main_channel->stopCreatePhysicalLink != 1) &&
            (a2mp_main_channel->remDev->remoteWifiAMP.physical_link_sate == BDS_DISCONNECTED))
        {
            BtA2MPGetAMPAssocReq parms;

            parms.controllerId = a2mp_parm.controllerId;

            BTA2MP_SendGetAMPAssocRequest(link, &parms);
        }
    }
    else
    {
        BTA2MP_HandleAMPAbnormal(link, A2MP_GETINFO_RESULT_ERROR);
    }
}

void BTA2MP_HandleGetAMPAssocRequest(BtRemoteDevice *link, U8 *data, U16 length)
{
	BtA2MPGetAMPAssocRsp a2mp_parm;
    A2MP_MAIN_CONN *a2mp_main_channel;
	U8 controllerId;
    U8 i=0;
    U8 validController = 0;
    U8 controllerNo = BT_PAL_CONTROLLER_LIST_SIZE/3;

	controllerId = data[0];
    a2mp_main_channel = A2MP_FindMainChannel(link);
    if(controllerId !=0) /* Controller ID 0x00 is BR/EDR, Can not get AMP Assoc*/
    {
        for(i=0;i<controllerNo; i++)
        {
            if(A2MP_GetControllerIdByIndex(i*3) == controllerId)
            {
                validController = 1;
                break;
            }
        }
    }
    if((validController == 0) || (a2mp_main_channel ==0) ||
        (a2mp_main_channel->last_rxCmdopcode != A2MP_CODE_INVALID))
    {
        OS_MemSet((U8 *)&a2mp_parm, 0, sizeof(BtA2MPGetAMPAssocRsp));
        a2mp_parm.controllerId = data[0];
        a2mp_parm.status = A2MP_GetAMPControllerStatus(controllerId);
        a2mp_parm.ampAssocSize = 0;
        a2mp_parm.ampAssoc = NULL;
        BTA2MP_SendGetAMPAssocResponse(link, &a2mp_parm);
    }
    else
    {
        a2mp_main_channel->last_rxCmdopcode = A2MP_CODE_GET_AMP_ASSOC_REQ;
        link->remoteWifiAMP.read_local_assoc.length_so_far = 0;
        link->remoteWifiAMP.read_local_assoc.amp_assoc_length = 248;
        a2mp_main_channel->local_amp_assoc_size = 0;
        ME_ReadLocalAMPAssoc(&link->remoteWifiAMP);
        /* Use HCI command to request the AMP Info */
    }

}

void BTA2MP_HandleGetAMPAssocResponse(BtRemoteDevice *link, A2MP_MAIN_CONN *a2mp_main_channel, U8 *data, U16 length)
{
	BtA2MPGetAMPAssocRsp a2mp_parm;

	a2mp_parm.controllerId = data[0];
	a2mp_parm.status = data[1];
	a2mp_parm.ampAssocSize = length -2;
	a2mp_parm.ampAssoc = data+2;
    if(a2mp_parm.status !=0)
    {
        BTA2MP_HandleAMPAbnormal(link, A2MP_GETAMPASSOC_RESULT_ERROR);
        return;
    }
    if(a2mp_main_channel !=0)
    {
        if(a2mp_parm.ampAssocSize > BT_MAX_AMP_ASSOC_SIZE)
            a2mp_parm.ampAssocSize = BT_MAX_AMP_ASSOC_SIZE;
        if ((a2mp_main_channel->stopCreatePhysicalLink == 1) ||
            (a2mp_main_channel->remDev->remoteWifiAMP.physical_link_sate != BDS_DISCONNECTED))
        {
            return;
        }

        memcpy(a2mp_main_channel->remote_amp_assoc, a2mp_parm.ampAssoc, a2mp_parm.ampAssocSize);
        a2mp_main_channel->remote_amp_assoc_size = a2mp_parm.ampAssocSize;
        a2mp_main_channel->waitAmpPhysicalLinkResponse = 1;        
        ME_CreatePhysicalLink(&link->remoteWifiAMP);
    }
}

void BTA2MP_HandleCreatePhysicalLinkRequest(BtRemoteDevice *link, A2MP_MAIN_CONN *a2mp_main_channel, U8 *data, U16 length)
{
	BtA2MPCreatePhysicalLinkReq a2mp_parm;
    BtA2MPCreatePhysicalLinkRsp a2mp_parm_rsp;

	a2mp_parm.localControllerId = data[0];  /* It is the remote ControllerId */
	a2mp_parm.remoteControllerId = data[1]; /* It is our local ControllerId */
	a2mp_parm.ampAssocSize = length -2;
	a2mp_parm.ampAssoc = data+2;
    a2mp_parm_rsp.localControllerId = a2mp_parm.remoteControllerId; /* It is our local ControllerId */
    a2mp_parm_rsp.remoteControllerId = a2mp_parm.localControllerId; /* It is the remote ControllerId */
    a2mp_parm_rsp.status = 0x00;
    Report(("Handle BTA2MP_HandleCreatePhysicalLinkRequest"));
    if(link->encryptState != BES_ENCRYPTED)
    {
        a2mp_parm_rsp.status = 0x06;
        /* Security Violation */
    }
    else if(a2mp_main_channel !=0)
    {
        Report(("a2mp_main_channel not 0"));

        if(a2mp_parm.ampAssocSize > BT_MAX_AMP_ASSOC_SIZE)
            a2mp_parm.ampAssocSize = BT_MAX_AMP_ASSOC_SIZE;
        if(a2mp_parm.remoteControllerId != a2mp_main_channel->localControllerId)
        {
            Report(("controller id not match:%d, %d",a2mp_parm.remoteControllerId,a2mp_main_channel->localControllerId));

            a2mp_parm_rsp.status = 0x01;
            a2mp_main_channel->remote_amp_assoc_size = 0;
        }
        else if(link->remoteWifiAMP.physical_link_sate == BDS_CONNECTED)
        {
            a2mp_parm_rsp.status = 0x05;
            /* Physical Link Already exists */
            Report(("Physical Link Already exists"));
        }
        else if((link->remoteWifiAMP.physical_link_sate != BDS_DISCONNECTED) &&
                (link->remoteWifiAMP.physical_link_sate != BDS_OUT_CON))
        {
            a2mp_parm_rsp.status = 0x02;
            /* Unable to start physical link creation */
            Report(("Unable to start physical link creation"));
        }
        else
        {
            a2mp_main_channel->remote_amp_assoc_size = a2mp_parm.ampAssocSize;
            memcpy(a2mp_main_channel->remote_amp_assoc, a2mp_parm.ampAssoc, a2mp_main_channel->remote_amp_assoc_size);
            Report(("a2mp_main_channel->last_opcode:%d",a2mp_main_channel->last_opcode));
            if((a2mp_main_channel->last_opcode == A2MP_CODE_CREATE_PHY_LINK_REQ) &&
                (A2MP_HandleCreatePhysicalLinkCollision(a2mp_main_channel->remDev) == TRUE))
            {
                Report(("Collision happens"));
                a2mp_parm_rsp.status = 0x03; /* Collision*/
            }
            else
            {
                if(a2mp_main_channel->last_opcode == A2MP_CODE_CREATE_PHY_LINK_REQ)
                {
                    Report(("Try to disconnect previous create link operation"));
                    ME_DisconnectPhysicalLink(&link->remoteWifiAMP, 0x13);
                    link->remoteWifiAMP.state = BDS_WAIT_PHYSICALLINK_COLLISION_ACCEPT;
                }
                else
                {
                    Report(("ME_AcceptPhysicalLink"));
                    a2mp_main_channel->waitAmpPhysicalLinkResponse = 1;
                    ME_AcceptPhysicalLink(&link->remoteWifiAMP);
                }
                link->remoteWifiAMP.exists = 1;
                link->remoteWifiAMP.remoteControllerId = a2mp_parm_rsp.remoteControllerId; /* It is the remote ControllerId */
            }
        }
    }
    else
    {
        a2mp_parm_rsp.status = 0x01;
    }
    Report(("a2mp_parm_rsp.status:%d",a2mp_parm_rsp.status));

    BTA2MP_SendCreatePhysicalLinkResponse(link, &a2mp_parm_rsp);
}

void BTA2MP_HandleCreatePhysicalLinkResponse(BtRemoteDevice *link, A2MP_MAIN_CONN *a2mp_main_channel, U8 *data, U16 length)
{
	BtA2MPCreatePhysicalLinkRsp a2mp_parm;

	a2mp_parm.localControllerId = data[0];
	a2mp_parm.remoteControllerId = data[1];
	a2mp_parm.status = data[2];
    a2mp_main_channel = A2MP_FindMainChannel(link);
    Report(("BTA2MP_HandleCreatePhysicalLinkResponse:%d",data[2]));
    if((a2mp_parm.status !=0) && (a2mp_parm.status != 0x03))
    {
        ME_DisconnectPhysicalLink(&link->remoteWifiAMP, 0x13);
        link->remoteWifiAMP.physical_link_sate = BDS_OUT_DISC;
        BTA2MP_HandleAMPAbnormal(link, A2MP_CREATE_CHANNEL_RSP_ERROR);
    }
#ifdef __A2MP_UT__
    else
        Me_SendA2MPPhysicalCompleteEvent(0x00, 10);
#endif
}

void BTA2MP_HandleDisconnectPhysicalLinkRequest(BtRemoteDevice *link,A2MP_MAIN_CONN *a2mp_main_channel, U8 *data, U16 length)
{
	BtA2MPDisconnectPhysicalLinkReq a2mp_parm;
    BtA2MPDisconnectPhysicalLinkRsp a2mp_parm_rsp;

	a2mp_parm.localControllerId = data[0];
	a2mp_parm.remoteControllerId = data[1];
    a2mp_parm_rsp.localControllerId = a2mp_parm.localControllerId;
    a2mp_parm_rsp.remoteControllerId = a2mp_parm.remoteControllerId;
    a2mp_parm_rsp.status = 0x00;
    if(a2mp_main_channel !=0)
    {
        if(a2mp_parm.remoteControllerId != a2mp_main_channel->localControllerId)
        {
            a2mp_parm_rsp.status = 0x01;
        }
        else
        {
            if(link->remoteWifiAMP.exists ==0)
                a2mp_parm_rsp.status = 0x02;
            else
                ME_DisconnectPhysicalLink(&link->remoteWifiAMP, 0x16);
        }
    }
    BTA2MP_SendDisconnectPhysicalLinkResponse(link, &a2mp_parm_rsp);
}

void BTA2MP_HandleDisconnectPhysicalLinkResponse(BtRemoteDevice *link, A2MP_MAIN_CONN *a2mp_main_channel, U8 *data, U16 length)
{
	BtA2MPDisconnectPhysicalLinkRsp a2mp_parm;

	a2mp_parm.localControllerId = data[0];
	a2mp_parm.remoteControllerId = data[1];
	a2mp_parm.status = data[2];
}

void BTA2MP_HandleCommandResponse(A2MP_MAIN_CONN *channel)
{
    A2MP_StopTimer(channel);
    channel->localIdent = 0;

}

void BtA2MP_HandlePhysicalChannelCreatedDuringCreateChannel(A2MP_MAIN_CONN *a2mp_main_channel)
{
    if(a2mp_main_channel == 0)
    {
        return;
    }
    a2mp_main_channel->stopCreatePhysicalLink = 1;

}

void BTA2MP_RecvDataInd(BtRemoteDevice *link, U8 *data, U16 data_len)
{
	U8 opcode;
	U8 identify;
	U16 length;
	U8 *raw_data;
	A2MP_MAIN_CONN *a2mp_main_channel;

    Report(("BTA2MP_RecvDataInd length:d", data_len));

	opcode = data[0];
	identify = data[1];
	length = LEtoHost16(data+2);
	raw_data = data+4;

    Report(("BTA2MP_RecvDataInd opcode:02x", opcode));
	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel == 0)
	{
		Report(("A2MP main channel empty"));
		return;
	}
	if((opcode % 0x02) == 0)
	{
	    a2mp_main_channel->remoteIdent = identify;
        Report(("A2MP req opcode:%d", opcode));
		switch(opcode)
		{
			case A2MP_CODE_DISCOVER_REQ:
                Report(("RX A2MP_CODE_DISCOVER_REQ"));
				A2MP_StopTimer(a2mp_main_channel);
				BTA2MP_HandleDiscoveryRequest(link, a2mp_main_channel, raw_data, length);
				break;
			case A2MP_CODE_CHANGE_NOTIFY:
                Report(("RX A2MP_CODE_CHANGE_NOTIFY"));
				BTA2MP_HandleChangeNotify(link, a2mp_main_channel, raw_data, length);
				break;
			case A2MP_CODE_GET_INFO_REQ:
                Report(("RX A2MP_CODE_GET_INFO_REQ"));
				BTA2MP_HandleGetInfoRequest(link, a2mp_main_channel, raw_data, length);
				break;
			case A2MP_CODE_GET_AMP_ASSOC_REQ:
                Report(("RX A2MP_CODE_GET_AMP_ASSOC_REQ"));
				BTA2MP_HandleGetAMPAssocRequest(link, raw_data, length);
				break;
			case A2MP_CODE_CREATE_PHY_LINK_REQ:
                Report(("RX A2MP_CODE_CREATE_PHY_LINK_REQ"));
				BTA2MP_HandleCreatePhysicalLinkRequest(link, a2mp_main_channel, raw_data, length);
				break;
			case A2MP_CODE_DISC_PHY_LINK_REQ:
                Report(("RX A2MP_CODE_DISC_PHY_LINK_REQ"));
				BTA2MP_HandleDisconnectPhysicalLinkRequest(link, a2mp_main_channel, raw_data, length);
				break;
            default:
                {
                    BtA2MPCommandReject a2mp_parm_reject;

                    a2mp_parm_reject.data = NULL;
                    a2mp_parm_reject.dataSize = 0;
                    a2mp_parm_reject.reason = 0x0000;
                    BTA2MP_CommandReject(link, &a2mp_parm_reject);
                }
                break;
		}
	}
	else
	{

		A2MP_StopTimer(a2mp_main_channel);
		if(a2mp_main_channel->localIdent == identify)
		{
			a2mp_main_channel->localIdent = 0;
		}
		else
		{
			Report(("Unknown identifier"));
//			return;
		}
        Report(("A2MP rsp opcode:%d", opcode));
        a2mp_main_channel->last_opcode = A2MP_CODE_INVALID;
		switch(opcode)
		{
			case A2MP_CODE_COMMAND_REJECT:
                Report(("RX A2MP_CODE_COMMAND_REJECT"));
				BTA2MP_HandleCommandReject(link, a2mp_main_channel, raw_data, length);
				break;
			case A2MP_CODE_DISCOVER_RSP:
                Report(("RX A2MP_CODE_DISCOVER_RSP"));
				BTA2MP_HandleDiscoveryResponse(link, a2mp_main_channel, raw_data, length);
				break;
			case A2MP_CODE_CHANGE_RSP:
                Report(("RX A2MP_CODE_CHANGE_RSP"));
				BTA2MP_HandleChangeResponse(link, a2mp_main_channel);
				break;
			case A2MP_CODE_GET_INFO_RSP:
                Report(("RX A2MP_CODE_GET_INFO_RSP"));
				BTA2MP_HandleGetInfoResponse(link, a2mp_main_channel, raw_data, length);
				break;
			case A2MP_CODE_GET_AMP_ASSOC_RSP:
                Report(("RX A2MP_CODE_GET_AMP_ASSOC_RSP"));
				BTA2MP_HandleGetAMPAssocResponse(link, a2mp_main_channel, raw_data, length);
				break;
			case A2MP_CODE_CREATE_PHY_LINK_RSP:
                Report(("RX A2MP_CODE_CREATE_PHY_LINK_RSP"));
 				BTA2MP_HandleCreatePhysicalLinkResponse(link, a2mp_main_channel, raw_data, length);
				break;
			case A2MP_CODE_DISC_PHY_LINK_RSP:
                Report(("RX A2MP_CODE_DISC_PHY_LINK_RSP"));
				BTA2MP_HandleDisconnectPhysicalLinkResponse(link, a2mp_main_channel, raw_data, length);
				break;
		}
	}
}

void BTA2MP_ParseControllerList(BtRemoteDevice *link, U8 size, U8 *data)
{
    U8 parseOffset =0;
    if(size >= 54)
    {
        size = 54;
    }
    if((size % 3) ==0)
    {
        do
        {
            Report(("Controller id:%d", data[parseOffset]));
            Report(("Controller type:%d", data[parseOffset+1]));
            Report(("Controller status:%d", data[parseOffset+2]));
            switch (data[parseOffset+1])
            {
                case BT_A2MP_CONTROLLER_TYPE_BREDR:
                    /* BR, EDR always exists */
                    break;
                case BT_A2MP_CONTROLLER_TYPE_80211:
                    link->remoteWifiAMP.exists = 1;
                    link->remoteWifiAMP.ampStatus = data[parseOffset+2];
                    link->remoteWifiAMP.remoteControllerId = data[parseOffset];
                    break;

            }
            parseOffset +=3;
        }
        while (parseOffset != size);
    }
}

U8 BTA2MP_ParseControllerListAndAvailable(BtRemoteDevice *link, U8 size, U8 *data, U8 controllerid)
{
    U8 parseOffset =0;
    if(size >= 54)
    {
        size = 54;
    }
    if((size % 3) ==0)
    {
        do
        {
            Report(("Controller id:%d", data[parseOffset]));
            Report(("Controller type:%d", data[parseOffset+1]));
            Report(("Controller status:%d", data[parseOffset+2]));
            switch (data[parseOffset+1])
            {
                case BT_A2MP_CONTROLLER_TYPE_BREDR:
                    /* BR, EDR always exists */
                    break;
                case BT_A2MP_CONTROLLER_TYPE_80211:
                    link->remoteWifiAMP.exists = 1;
                    link->remoteWifiAMP.ampStatus = data[parseOffset+2];
                    link->remoteWifiAMP.remoteControllerId = data[parseOffset];
                    break;

            }
            if(data[parseOffset] == controllerid)
            {
                return data[parseOffset+2];
            }
            parseOffset +=3;
        }
        while (parseOffset != size);
    }
    return 0x00;
}

BtStatus BTA2MP_PhysicalLinkCreate(BtRemoteDevice *link)
{
	BtA2MPGetInfoReq a2dp_info;
    A2MP_MAIN_CONN *a2mp_channel;

    /* If the discovery AMP is not done, wait the discover done. After discover done,
           The A2MP will create Physical link.
       */
    Report(("BTA2MP_PhysicalLinkCreate detected:%d, exists:%d",
                            link->remoteWifiAMP.detected,
                            link->remoteWifiAMP.exists));

    a2mp_channel = A2MP_FindMainChannel(link);
    if(a2mp_channel ==0)
        return BT_STATUS_FAILED;
    Report(("BTA2MP_PhysicalLinkCreate discoverDone:%d",
                            a2mp_channel->discoverDone));

    if(a2mp_channel->ampPhysicalLinkCreating == 1)
    {
        Report(("Pervious create channel pending"));
        return BT_STATUS_PENDING;
    }
    switch (A2MP_GetAMPControllerStatus(BT_PAL_CONTROLLERID))
    {
        case 0x00:
        case 0x02:
            /* Unable to create physical channel */
            return BT_STATUS_FAILED;
    }
    if((link->remoteWifiAMP.detected == 1) &&
        (a2mp_channel->discoverDone == 1) &&
        (link->remoteWifiAMP.exists == 1))
    {
        a2dp_info.controllerId = link->remoteWifiAMP.remoteControllerId;
        a2mp_channel->ampPhysicalLinkCreating = 1;
        link->remoteWifiAMP.initiator = 1;
        BTA2MP_SendGetInfoRequest(link, &a2dp_info);
        a2mp_channel->stopCreatePhysicalLink = 0;
        return BT_STATUS_PENDING;
    }
    if((link->remoteWifiAMP.detected == 1) &&
        (a2mp_channel->discoverDone == 1) &&
        (link->remoteWifiAMP.exists == 0))
    {
        Report(("AMP is not existing"));
        return BT_STATUS_FAILED;
    }
    return BT_STATUS_PENDING;
}

void BTA2MP_PhysicalLinkClose(BtRemoteDevice *link)
{
	BtA2MPDisconnectPhysicalLinkReq a2dp_info;
    A2MP_MAIN_CONN *a2mp_channel;

    if (link->remoteWifiAMP.exists == 1)
    {
        a2mp_channel = A2MP_FindMainChannel(link);
        if(a2mp_channel !=0)
        {
            a2dp_info.localControllerId = a2mp_channel->localControllerId;
            a2dp_info.remoteControllerId = link->remoteWifiAMP.remoteControllerId;
            BTA2MP_SendDisconnectPhysicalLinkRequest(link, &a2dp_info);
        }
    }
}

BtStatus BTA2MP_StartPhysicalLink(BtRemoteDevice *link, U16 psm, U16 local_cid)
{
    BtA2MPDiscoveryReq discoveryReq;
    A2MP_CONN *amp_channel;
	A2MP_MAIN_CONN *main_channel;

    main_channel = A2MP_FindMainChannel(link);
    amp_channel = A2MP_AllocatePSMChannel(link, psm);

    if(amp_channel ==0)
    {
        Report(("No enough resource to create AMP channel"));
        return BT_STATUS_FAILED;
    }

    amp_channel->psm = psm;
    amp_channel->remDev = link;
    amp_channel->waitPhysicalLinkCompleted = 1;
	amp_channel->l2ChannelId = local_cid;
    Report(("A2MP discovery Done:%d",main_channel->discoverDone));
    Report(("A2MP detected:%d",link->remoteWifiAMP.detected));

    Report(("Amp Status:%d",link->remoteWifiAMP.ampStatus));
    switch (link->remoteWifiAMP.ampStatus)
    {
        case 0x00:
        case 0x02:
            /* Unable to create physical channel */
            A2MP_FreePSMChannel(amp_channel);
            return BT_STATUS_FAILED;

    }
    switch (A2MP_GetAMPControllerStatus(BT_PAL_CONTROLLERID))
    {
        case 0x00:
        case 0x02:
            /* Unable to create physical channel */
            A2MP_FreePSMChannel(amp_channel);
            return BT_STATUS_FAILED;
    }
    if(main_channel->discoverDone == 1)
    {
        Report(("A2MP info->aclLink->remoteWifiAMP.exists:%d",link->remoteWifiAMP.exists));

        if(link->remoteWifiAMP.exists == 0)
        {
            /* Discovery done but no WifiAMP exists, return directly */
            Report(("No AMP controller found. "));
            /* Send event to PSM to notify L2CAP connected. */
            A2MP_FreePSMChannel(amp_channel);
            return BT_STATUS_FAILED;
        }
        BTA2MP_PhysicalLinkCreate(link);
    }
    else if(link->remoteWifiAMP.detected == 0)
    {
        U8 extendedFeatureMask[2];
        Report(("A2MP BTA2MP_SendDiscoveryRequest"));

        /* Start AMP discovery procedure. */
        link->remoteWifiAMP.detected = 1;
        link->remoteWifiAMP.exists = 0;
        discoveryReq.mtu = 670;
        discoveryReq.extendedFeatureMaskSize = 0;
        extendedFeatureMask[0] = 0x00;
        extendedFeatureMask[1] = 0x00;
        discoveryReq.extendedFeatureMask = (U8 *)extendedFeatureMask;
        discoveryReq.extendedFeatureMaskSize = 2;
        BTA2MP_SendDiscoveryRequest(link, &discoveryReq);
    }
    return BT_STATUS_PENDING;

}

BtStatus BTA2MP_LogicalLinkCreate(U16 L2CapId,
                                          U8 createOrAccept,
                                          BtRemoteDevice *link,
                                          U8 controllerId)
{
    U8 index = 0;
    U8 bestEffortIndex =0;
    L2capExtendedFlowSpecOption *inExtFlowSpec;
    L2capExtendedFlowSpecOption *outExtFlowSpec;
    U8 *linkFlowSpec;
    
    Report(("controllerId:%d",controllerId));
    if(controllerId == 0x00) /* BR and EDR*/
        return BT_STATUS_SUCCESS;
    index = A2MP_GetLogicalIndex(link);
    Report(("index:%d",index));
    
    if(index == 0xFF)
    {
        return BT_STATUS_FAILED;
    }
    Report(("link:%08x", link));
    Report(("link->remoteWifiAMP:%08x", link->remoteWifiAMP));
    Report(("link->remoteWifiAMP.physical_link_sate:%d",link->remoteWifiAMP.physical_link_sate));

    if(link->remoteWifiAMP.physical_link_sate != BDS_CONNECTED)
    {
        return BT_STATUS_FAILED;
    }
    inExtFlowSpec = L2Cap_GetInExtendedFlowSpec(L2CapId);
    outExtFlowSpec = L2Cap_GetInExtendedFlowSpec(L2CapId);

    linkFlowSpec = (U8 *)(link->remoteWifiAMP.logicalLink[index].flow_spec.logical_link_rx_flow_spec);
    linkFlowSpec[0] = inExtFlowSpec->identifier;
    linkFlowSpec[1] = inExtFlowSpec->serviceType;
    StoreLE16(linkFlowSpec+2, inExtFlowSpec->maxSduSize);
    StoreLE32(linkFlowSpec+4, inExtFlowSpec->sduIntArrialTime);
    StoreLE32(linkFlowSpec+8, inExtFlowSpec->accessLatency);
    StoreLE32(linkFlowSpec+12, inExtFlowSpec->flushTimeout);

    linkFlowSpec = (U8 *)(link->remoteWifiAMP.logicalLink[index].flow_spec.logical_link_tx_flow_spec);
    linkFlowSpec[0] = outExtFlowSpec->identifier;
    linkFlowSpec[1] = outExtFlowSpec->serviceType;
    StoreLE16(linkFlowSpec+2, outExtFlowSpec->maxSduSize);
    StoreLE32(linkFlowSpec+4, outExtFlowSpec->sduIntArrialTime);
    StoreLE32(linkFlowSpec+8, outExtFlowSpec->accessLatency);
    StoreLE32(linkFlowSpec+12, outExtFlowSpec->flushTimeout);

    link->remoteWifiAMP.logicalLink[index].logical_link_type = inExtFlowSpec->serviceType;
    link->remoteWifiAMP.logicalLink[index].l2cap_id = L2CapId;
    Report(("inExtFlowSpec->serviceType:%d",inExtFlowSpec->serviceType));
    if(inExtFlowSpec->serviceType <= 0x01)
    {
        bestEffortIndex =  A2MP_GetLogicalBestEffortIndex(link);
        if(bestEffortIndex != 0xFF)
        {
            link->remoteWifiAMP.logicalLink[index].logical_link_handle =
                link->remoteWifiAMP.logicalLink[bestEffortIndex].logical_link_handle;
            link->remoteWifiAMP.logicalLink[index].logical_link_sate =
                link->remoteWifiAMP.logicalLink[bestEffortIndex].logical_link_sate;
            if(link->remoteWifiAMP.logicalLink[bestEffortIndex].logical_link_sate == BDS_CONNECTED)
                return BT_STATUS_SUCCESS;
            else
            {
                return BT_STATUS_PENDING;
            }
        }
    }

    Me_SetCurrentLogicalLinkOperation(&link->remoteWifiAMP, index);
    if(createOrAccept == BT_AMP_COMMAND_CREATE_LOGICAL_LINK)
        ME_CreateLogicalLink(&link->remoteWifiAMP, index);
    else
        ME_AcceptLogicalLink(&link->remoteWifiAMP, index);
    return BT_STATUS_PENDING;
}

BtStatus BTA2MP_LogicalLinkClose(BtRemoteDevice *link, U16 logical_link_hdl)
{
    BTRemoteAMPLogicalLink *logical_link;
    BtStatus status = BT_STATUS_FAILED;
    A2MP_MAIN_CONN *a2mp_channel;
    U8 i=0;

    a2mp_channel = A2MP_FindMainChannel(link);

    if(link->remoteWifiAMP.physical_link_sate == BDS_CONNECTED)
    {
    	for(i=0;i<BT_AMP_SUPPORTED_LOGICAL_CHANNEL; i++)
    	{
    		if((link->remoteWifiAMP.logicalLink[i].logical_link_sate == BDS_CONNECTED) &&
    			(link->remoteWifiAMP.logicalLink[i].logical_link_handle == logical_link_hdl) &&
    			(link->remoteWifiAMP.logicalLink[i].logical_link_type <= 0x01))
    		{
    			return BT_STATUS_SUCCESS;
    		}
    	}
        /* Not the Best effort channel */
        logical_link = (BTRemoteAMPLogicalLink *)A2MP_FindLogicalLinkByHdl(&(link->remoteWifiAMP), logical_link_hdl);
        if(logical_link->logical_link_sate == BDS_CONNECTED)
        {
            status = ME_DisconnectLogicalLink(&(link->remoteWifiAMP), logical_link_hdl);
        }
        else if(logical_link->logical_link_sate == BDS_OUT_DISC)
        {
            status = BT_STATUS_PENDING;
        }
        else
        {
            status = BT_STATUS_FAILED;
        }
    }
    return status;

}

BtStatus BTA2MP_StartPhysicalLinkOnly(BtRemoteDevice *link)
{
    BtA2MPDiscoveryReq discoveryReq;
	A2MP_MAIN_CONN *main_channel;
	U8 extendedFeatureMask[2];

    main_channel = A2MP_FindMainChannel(link);
    Report(("A2MP discovery Done:%d",main_channel->discoverDone));
    Report(("A2MP detected:%d",link->remoteWifiAMP.detected));

    Report(("Amp Status:%d",link->remoteWifiAMP.ampStatus));
    Report(("A2MP BTA2MP_SendDiscoveryRequest"));
    /* Start AMP discovery procedure. */
	link->remoteWifiAMP.initiator = 1;
    link->remoteWifiAMP.detected = 1;
    link->remoteWifiAMP.exists = 0;
    discoveryReq.mtu = 670;
    discoveryReq.extendedFeatureMaskSize = 0;
    extendedFeatureMask[0] = 0x00;
    extendedFeatureMask[1] = 0x00;
    discoveryReq.extendedFeatureMask = (U8 *)extendedFeatureMask;
    discoveryReq.extendedFeatureMaskSize = 2;
    BTA2MP_SendDiscoveryRequest(link, &discoveryReq);
    return BT_STATUS_PENDING;
}


BtStatus BTA2MP_FlowSpecModify(BtRemoteAMPDevice *link, U16 logical_hdl)
{
    BTRemoteAMPLogicalLink *logical_link;
    BtStatus status = BT_STATUS_FAILED;

    if(link->physical_link_sate == BDS_CONNECTED)
    {
        logical_link = A2MP_FindLogicalLinkByHdl(link, logical_hdl);
        if(logical_link->logical_link_sate == BDS_CONNECTED)
        {
            status = ME_FlowSpecModify(logical_link);
        }
        else if(logical_link->logical_link_sate == BDS_OUT_DISC)
        {
            status = BT_STATUS_PENDING;
        }
        else
        {
            status = BT_STATUS_FAILED;
        }

    }

    return status;

}


void BTA2MP_HandleAMPAbnormal(BtRemoteDevice *link, U8 event)
{

}


U8 BTA2MP_CheckDiscoveryDone(BtRemoteDevice *link)
{
    A2MP_MAIN_CONN *a2mp_channel;

    a2mp_channel = A2MP_FindMainChannel(link);
    if(a2mp_channel ==0)
        return 0;

    if((link->remoteWifiAMP.detected == 1) &&
        (a2mp_channel->discoverDone == 1) &&
        (link->remoteWifiAMP.exists == 1))

	if((link->remoteWifiAMP.detected == 1) &&
		(a2mp_channel->discoverDone == 1) &&
		(link->remoteWifiAMP.physical_link_sate == BDS_CONNECTED))
	{
		return 0x01;
	}
	return 0x00;

}


#endif /* A2MP_PROTOCOL == XA_ENABLED */

