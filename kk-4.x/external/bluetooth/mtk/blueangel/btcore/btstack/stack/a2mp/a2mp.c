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
#include "L2cap_AMP.h"

#if A2MP_PROTOCOL == XA_ENABLED
extern BtStatus BTA2MP_PhysicalLinkCreate(BtRemoteDevice *link);
extern A2MP_CONN *A2MP_AllocatePSMChannel(BtRemoteDevice *link, U16 psm);
extern A2MP_CONN *A2MP_AllocatePSMChannelWaiting(BtRemoteDevice *link, U16 psm);
extern void A2MP_FreePSMChannel(A2MP_CONN *a2mp_channel);
extern A2MP_MAIN_CONN *A2MP_FindMainChannel(BtRemoteDevice *link);
extern A2MP_CONN *A2MP_FindPSMChannelByCid(U16 cid);


BtStatus A2MPHandleCreateServerChannelReq(
                                        BtRemoteDevice *RemDev,
										U16 l2ChannelId,
										U16	ampPsm,
										U8 remoteControlId)
{
	A2MP_CONN *Channel;
	A2MP_MAIN_CONN *Main_Channel;
	L2capAMPConnSettings settings;
	BtStatus status;
    U16 ResponseCode;

	Channel = A2MP_AllocatePSMChannel(RemDev, ampPsm);
	/* RFCOMM move to AMP */

	if(Channel == 0)
	{
	    Report(("A2MPHandleCreateServerChannelReq channel ==0"));
        Channel = A2MP_AllocatePSMChannelWaiting(RemDev, ampPsm);
        if(Channel == 0)
        {
            Report(("A2MPHandleCreateServerChannelReq channel ==0"));
            /* Conflict create channel request, reject one. */
            status = L2CAP_CreateChannelRsp(l2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0x00);
            return BT_STATUS_FAILED;
        }
	}
    Main_Channel = A2MP_FindMainChannel(RemDev);
    if(Main_Channel ==0)
    {
	    Report(("A2MPHandleCreateServerChannelReq Main_Channel ==0"));
        status = L2CAP_CreateChannelRsp(l2ChannelId, L2CONN_REJECT_CONTROLLER_ID_NOT_SUPPORTED, 0x00);
        A2MP_FreePSMChannel(Channel);
		return BT_STATUS_FAILED;
	}
    Channel->remDev = RemDev;
	Channel->l2ChannelId = l2ChannelId;

	settings.remoteAmpControllerId = remoteControlId;
    Report(("A2MPHandleCreateServerChannelReq remoteControlId:%d",remoteControlId));
    Report(("A2MPHandleCreateServerChannelReq Main_Channel->localControllerId:%d",Main_Channel->localControllerId));

    if(remoteControlId != Main_Channel->localControllerId)
    {

        ResponseCode = L2CONN_REJECT_CONTROLLER_ID_NOT_SUPPORTED;
        status = L2CAP_CreateChannelRsp(l2ChannelId, ResponseCode, 0x00);
        A2MP_FreePSMChannel(Channel);
        return BT_STATUS_FAILED;
    }
    Report(("BTA2MP_PhysicalLinkCreate accept"));

    ResponseCode = L2CONN_ACCEPTED;
    status = L2CAP_CreateChannelRsp(l2ChannelId, ResponseCode, 0x00);
    return BT_STATUS_SUCCESS;

}


BtStatus A2MPHandleCreateServerChannelFailed(BtRemoteDevice *link, U16 l2ChannelId)
{
    U8 i=0;
    A2MP_MAIN_CONN *a2mp_main_channel;
    a2mp_main_channel = A2MP_FindMainChannel(link);
    for(i=0;i<NUM_A2MP_SERVERS;i++)
    {
        if((a2mp_main_channel->a2mp_channel[i].state == A2MP_STATE_ALLOCATED) &&
            (a2mp_main_channel->a2mp_channel[i].l2ChannelId == l2ChannelId))
            A2MP_FreePSMChannel(&(a2mp_main_channel->a2mp_channel[i]));
    }

    return BT_STATUS_SUCCESS;
}


#endif /* A2MP_PROTOCOL == XA_ENABLED */

