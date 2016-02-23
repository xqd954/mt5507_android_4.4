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
#include "bt_feature.h"
#include "btalloc.h"
#include "btrom.h"
#include "sys/l2cap_i.h"
#include "sys/l2capxp.h"
#include "sys/mei.h"
#include "bt_devices_list.h"
#include "btaccctrl.h"

#ifdef __BT_3_0_HS__

/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendCreateChannelReqInternal
 * DESCRIPTION
 *
 * PARAMETERS
 *  Channel     [IN]
 * RETURNS
 * 
 *****************************************************************************/
BtStatus L2Cap_SendCreateChannelReqInternal(
	L2CAP_Channel *Channel, U8 RemoteControllerId)
{
    BtPacket *req;
    BtStatus status;

    if ((req = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        Channel->flags |= L2CHF_SEND_REQUEST;
        L2C(flags) |= L2CCF_NEED_SYSPACKET;
        return BT_STATUS_PENDING;
    }
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
	if (Channel->psmInfo->inLinkMode != L2MODE_BASIC)
	{

		if (Channel->link->linkModes == L2MODE_UNKNOWN)
		{
			/* Need to request the devices extended features before we can connect. */
			Channel->infoReqDone = 0;
			return L2Cap_SendFeaturesReq(Channel, req);
		}

		if ((Channel->psmInfo->inLinkMode & Channel->link->linkModes) == 0)
		{
			/* No shared link modes, fail the connect */
			return BT_STATUS_NOT_SUPPORTED;
		}
	}
#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */


    Channel->localIdent = GetNextIdent();

	req->header[BT_PACKET_HEADER_LEN - 8] = LLC_CREATE_CHANNEL_REQ;
	req->header[BT_PACKET_HEADER_LEN - 7] = Channel->localIdent;
	req->header[BT_PACKET_HEADER_LEN - 6] = 5;
	req->header[BT_PACKET_HEADER_LEN - 5] = 0;
	req->header[BT_PACKET_HEADER_LEN - 4] = (U8) (Channel->ampPsm);
	req->header[BT_PACKET_HEADER_LEN - 3] = (U8) (Channel->ampPsm >> 8);
	req->header[BT_PACKET_HEADER_LEN - 2] = (U8) (Channel->localCid);
	req->header[BT_PACKET_HEADER_LEN - 1] = (U8) (Channel->localCid >> 8);
	req->headerLen = 8;
	req->data[0] = (U8) RemoteControllerId;
	req->dataLen = 1;
    req->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    Report(("L2CAP AMP L2Cap_SendCreateChannelReqInternal"));
    status = L2Cap_Send(Channel, req, 0x0001);
    if (status != BT_STATUS_PENDING)
    {
        ReturnSysPkt(req);
    }
    return status;
}



/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendCreateChannelReqInternal
 * DESCRIPTION
 *
 * PARAMETERS
 *  Channel     [IN]
 * RETURNS
 *
 *****************************************************************************/
BtStatus L2Cap_SendCreateChannelRspInternal(
	L2CAP_Channel *Channel, U8 rsp_status)
{
    BtPacket *rsp;
    BtStatus status;

    if ((rsp = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        Channel->flags |= L2CHF_SEND_REQUEST;
        L2C(flags) |= L2CCF_NEED_SYSPACKET;
        return BT_STATUS_PENDING;
    }
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    if ((Channel->result == L2CONN_ACCEPTED) && (Channel->psmInfo->inLinkMode != L2MODE_BASIC))
    {

        if (Channel->link->linkModes == L2MODE_UNKNOWN)
        {
            /* Need to request the devices extended features before we can respond. */
            Channel->infoReqDone = 0;
            return L2Cap_SendFeaturesReq(Channel, rsp);
        }
        if (((Channel->psmInfo->inLinkMode & Channel->link->linkModes) == 0) &&
			(Channel->link->linkModes != 1))
        {
            /* No shared link modes, drop down to fail the connect. */
            Channel->result = L2CONN_REJECT_NO_RESOURCES;   
            /* Not the true reason! */
        }
    }
#endif

    if(Channel->result == 0x00)
    {
        rsp_status = 0x00;
        Channel->setupByCreateChannel = 1;
    }

	rsp->header[BT_PACKET_HEADER_LEN - 8] = LLC_CREATE_CHANNEL_RSP;
	rsp->header[BT_PACKET_HEADER_LEN - 7] = Channel->remoteIdent;
	rsp->header[BT_PACKET_HEADER_LEN - 6] = 8;
	rsp->header[BT_PACKET_HEADER_LEN - 5] = 0;
	rsp->header[BT_PACKET_HEADER_LEN - 4] = (U8) (Channel->localCid);
	rsp->header[BT_PACKET_HEADER_LEN - 3] = (U8) (Channel->localCid >> 8);
	rsp->header[BT_PACKET_HEADER_LEN - 2] = (U8) (Channel->remoteCid);
	rsp->header[BT_PACKET_HEADER_LEN - 1] = (U8) (Channel->remoteCid >> 8);
	rsp->headerLen = 8;
	rsp->data[0] = (U8) (Channel->result);
    rsp->data[1] = (U8) (Channel->result >> 8);
    rsp->data[2] = (U8) rsp_status;
    rsp->data[3] = (U8) (rsp_status >> 8);
    rsp->dataLen = 4;
    Report(("L2CAP AMP L2Cap_SendCreateChannelRspInternal with Channel->result:%02x",Channel->result));
    rsp->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2Cap_Send(Channel, rsp, 0x0001);
    if (status != BT_STATUS_PENDING)
    {
        ReturnSysPkt(rsp);
    }

    return status;
}



/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendCreateChannelReqInternal
 * DESCRIPTION
 *
 * PARAMETERS
 *  Channel     [IN]
 * RETURNS
 *
 *****************************************************************************/
BtStatus L2Cap_SendMoveChannelReqInternal(
	L2CAP_Channel *Channel, U8 InitiatorControllerId)
{
    BtPacket *req;
    BtStatus status;

    if ((req = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        Channel->flags |= L2CHF_SEND_REQUEST;
        L2C(flags) |= L2CCF_NEED_SYSPACKET;
        return BT_STATUS_PENDING;
    }

    Channel->localIdent = GetNextIdent();

	req->header[BT_PACKET_HEADER_LEN - 7] = LLC_MOV_CHANNEL_REQ;
	req->header[BT_PACKET_HEADER_LEN - 6] = Channel->localIdent;
	req->header[BT_PACKET_HEADER_LEN - 5] = 3;
	req->header[BT_PACKET_HEADER_LEN - 4] = 0;
	req->header[BT_PACKET_HEADER_LEN - 3] = (U8) (Channel->localCid);
	req->header[BT_PACKET_HEADER_LEN - 2] = (U8) (Channel->localCid >> 8);
	req->header[BT_PACKET_HEADER_LEN - 1] = (U8) InitiatorControllerId;
	req->headerLen = 7;

    req->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2Cap_Send(Channel, req, 0x0001);
    if (status != BT_STATUS_PENDING)
    {
        ReturnSysPkt(req);
    }

    return status;
}



/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendCreateChannelReqInternal
 * DESCRIPTION
 *
 * PARAMETERS
 *  Channel     [IN]
 * RETURNS
 *
 *****************************************************************************/
BtStatus L2Cap_SendMoveChannelRspInternal(
	L2CAP_Channel *Channel)
{
	BtPacket *rsp;
	BtStatus status;

	if ((rsp = L2Cap_GetSysPkt(FALSE)) == 0)
	{
		Channel->flags |= L2CHF_SEND_REQUEST;
		L2C(flags) |= L2CCF_NEED_SYSPACKET;
		return BT_STATUS_PENDING;
	}

	rsp->header[BT_PACKET_HEADER_LEN - 8] = LLC_MOV_CHANNEL_RSP;
	rsp->header[BT_PACKET_HEADER_LEN - 7] = Channel->remoteMoveIdent;
	rsp->header[BT_PACKET_HEADER_LEN - 6] = 4;
	rsp->header[BT_PACKET_HEADER_LEN - 5] = 0;
	rsp->header[BT_PACKET_HEADER_LEN - 4] = (U8) (Channel->remoteCid);
	rsp->header[BT_PACKET_HEADER_LEN - 3] = (U8) (Channel->remoteCid >> 8);
	rsp->header[BT_PACKET_HEADER_LEN - 2] = (U8) (Channel->result);
	rsp->header[BT_PACKET_HEADER_LEN - 1] = (U8) (Channel->result >> 8);
	rsp->headerLen = 8;
    rsp->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2Cap_Send(Channel, rsp, 0x0001);
    if (status != BT_STATUS_PENDING)
    {
        ReturnSysPkt(rsp);
    }

	return status;

}


/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendCreateChannelReqInternal
 * DESCRIPTION
 *
 * PARAMETERS
 *  Channel     [IN]
 * RETURNS
 *
 *****************************************************************************/
BtStatus L2Cap_SendMoveChannelCnfReqInternal(
	L2CAP_Channel *Channel)
{
    BtPacket *req;
    BtStatus status;

    if ((req = L2Cap_GetSysPkt(FALSE)) == 0)
    {
        Channel->flags |= L2CHF_SEND_REQUEST;
        L2C(flags) |= L2CCF_NEED_SYSPACKET;
        return BT_STATUS_PENDING;
    }

    Channel->localIdent = GetNextIdent();

	req->header[BT_PACKET_HEADER_LEN - 8] = LLC_MOV_CHANNEL_CNF_REQ;
	req->header[BT_PACKET_HEADER_LEN - 7] = Channel->localIdent;
	req->header[BT_PACKET_HEADER_LEN - 6] = 4;
	req->header[BT_PACKET_HEADER_LEN - 5] = 0;
	req->header[BT_PACKET_HEADER_LEN - 4] = (U8) (Channel->localCid);
	req->header[BT_PACKET_HEADER_LEN - 3] = (U8) (Channel->localCid >> 8);
	req->header[BT_PACKET_HEADER_LEN - 2] = (U8) (Channel->result);
	req->header[BT_PACKET_HEADER_LEN - 1] = (U8) (Channel->result >> 8);
	req->headerLen = 8;

    req->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2Cap_Send(Channel, req, 0x0001);
    if (status != BT_STATUS_PENDING)
    {
        ReturnSysPkt(req);
    }

    return status;
}



/*****************************************************************************
 * FUNCTION
 *  L2Cap_SendCreateChannelReqInternal
 * DESCRIPTION
 *
 * PARAMETERS
 *  Channel     [IN]
 * RETURNS
 *
 *****************************************************************************/
BtStatus L2Cap_SendMoveChannelCnfRspInternal(
	L2CAP_Channel *Channel)
{
	BtPacket *rsp;
	BtStatus status;

	if ((rsp = L2Cap_GetSysPkt(FALSE)) == 0)
	{
		Channel->flags |= L2CHF_SEND_REQUEST;
		L2C(flags) |= L2CCF_NEED_SYSPACKET;
		return BT_STATUS_PENDING;
	}

	rsp->header[BT_PACKET_HEADER_LEN - 6] = LLC_MOV_CHANNEL_CNF_RSP;
	rsp->header[BT_PACKET_HEADER_LEN - 5] = Channel->remoteMoveIdent;
	rsp->header[BT_PACKET_HEADER_LEN - 4] = 2;
	rsp->header[BT_PACKET_HEADER_LEN - 3] = 0;
	rsp->header[BT_PACKET_HEADER_LEN - 2] = (U8) (Channel->remoteCid);
	rsp->header[BT_PACKET_HEADER_LEN - 1] = (U8) (Channel->remoteCid >> 8);
	rsp->headerLen = 6;
    status = L2Cap_Send(Channel, rsp, 0x0001);
    if (status != BT_STATUS_PENDING)
    {
        ReturnSysPkt(rsp);
    }
	return status;

}


#endif
