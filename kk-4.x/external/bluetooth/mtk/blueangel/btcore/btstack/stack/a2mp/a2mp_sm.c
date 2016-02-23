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
#include "sys/debug.h"
#include "btaccctrl.h"
#include "meamp.h"
#include "a2mp_adp.h"
#include "a2mp_ctl.h"

#if A2MP_PROTOCOL == XA_ENABLED
void A2MPHandleConnectInd(const BtEvent *Event);
void A2MPHandleConnectCfm(const BtEvent *Event);
void A2MPHandleDisconnectInd(const BtEvent *Event);
void A2MPHandleA2MPMainChannelDisconnected(BtRemoteDevice *link);
void A2MPHandleA2MPAMPChannelDisconnected(BtRemoteDevice *link, U16 l2cap_id);

extern BtStatus BTA2MP_LogicalLinkCreate(U16 L2CapId,
                                          U8 createOrAccept,
                                          BtRemoteDevice *link,
                                          U8 controllerId);
extern void BTA2MP_HandleAMPAbnormal(BtRemoteDevice *link, U8 event);

extern A2MP_MAIN_CONN *A2MP_FindMainChannel(BtRemoteDevice *link);
extern A2MP_MAIN_CONN *A2MP_FindMainChannelByPhysicalLink(U8 physical_link_hdl);
extern A2MP_CONN *A2MP_FindPSMChannelByCid(U16 cid);
extern BtStatus BTA2MP_StartPhysicalLink(BtRemoteDevice *link, U16 psm, U16 local_cid);

extern void L2CAP_NotifyLogicalLinkDisconnected(BtRemoteDevice *link, U16 l2cap_id);
extern void L2Cap_AMPPhysicalLinkCompleted(BtRemoteDevice *link, U16 local_cid);
#ifdef __BT_3_0_HS__
void L2Cap_NotifyLogicalChannelCreated(L2capChannelId ChannelId, U16 result);
#endif

void A2MPHandleConnectInd(const BtEvent *Event)
{
    BtRemoteDevice *remDev = 0;

    remDev = Event->p.remDev;

    if (Event->errCode == BEC_NO_ERROR)
    {
        A2MP_AllocateL2CAPFixChannel(remDev);
    }
}


void A2MPHandleConnectCfm(const BtEvent *Event)
{
    BtRemoteDevice *remDev = 0;

    remDev = Event->p.remDev;
    if (Event->errCode == BEC_NO_ERROR)
    {
	    A2MP_AllocateL2CAPFixChannel(remDev);
	}
}

void A2MPHandleDisconnectInd(const BtEvent *Event)
{
    BtRemoteDevice *remDev = 0;

    remDev = Event->p.remDev;
    A2MPHandleA2MPMainChannelDisconnected(remDev);
    if(remDev->remoteWifiAMP.physical_link_sate == BDS_CONNECTED)
        ME_DisconnectPhysicalLink(&remDev->remoteWifiAMP, 0x13);
    /* The Fix channel will be free in L2CAP layer automatically. */
}

void A2MPHandleCreatePhysicalLinkStatus(const BtEvent *Event)
{
    U8 errorCode;
    A2MP_MAIN_CONN *a2mp_main_channel;
    BtRemoteDevice *link;

    errorCode = Event->errCode;
    link = (BtRemoteDevice *)Event->p.hciAmpEvent.parms;
    a2mp_main_channel = A2MP_FindMainChannel(link);
    if(a2mp_main_channel !=0)
    {
        a2mp_main_channel->remDev->remoteWifiAMP.write_remote_assoc.length_so_far = 0;
        a2mp_main_channel->remDev->remoteWifiAMP.write_remote_assoc.amp_assoc_fragment = a2mp_main_channel->remote_amp_assoc;
        a2mp_main_channel->remDev->remoteWifiAMP.write_remote_assoc.amp_assoc_size = a2mp_main_channel->remote_amp_assoc_size;
        a2mp_main_channel->remDev->remoteWifiAMP.write_remote_assoc.amp_assoc_remain_length = a2mp_main_channel->remote_amp_assoc_size;
        ME_WriteRemoteAMPAssoc(&a2mp_main_channel->remDev->remoteWifiAMP);
    }

}

void A2MPHandleAcceptPhysicalLinkStatus(const BtEvent *Event)
{
    U8 errorCode;
    A2MP_MAIN_CONN *a2mp_main_channel;
    BtRemoteDevice *link;

    errorCode = Event->errCode;
    link = (BtRemoteDevice *)Event->p.hciAmpEvent.parms;
    a2mp_main_channel = A2MP_FindMainChannel(link);
    if(a2mp_main_channel !=0)
    {
        a2mp_main_channel->remDev->remoteWifiAMP.write_remote_assoc.length_so_far = 0;
        a2mp_main_channel->remDev->remoteWifiAMP.write_remote_assoc.amp_assoc_fragment = a2mp_main_channel->remote_amp_assoc;
        a2mp_main_channel->remDev->remoteWifiAMP.write_remote_assoc.amp_assoc_size = a2mp_main_channel->remote_amp_assoc_size;
        a2mp_main_channel->remDev->remoteWifiAMP.write_remote_assoc.amp_assoc_remain_length = a2mp_main_channel->remote_amp_assoc_size;
        ME_WriteRemoteAMPAssoc(&a2mp_main_channel->remDev->remoteWifiAMP);
    }

}

void A2MPHandlePhysicalLinkComplete(const BtEvent *Event)
{
	BT_AMP_HCI_EVENT_PHYSICAL_LINK_COMPLETE result;
	U8 *parm;
    U8 controllerId;
    A2MP_MAIN_CONN *a2mp_main_channel;
    U8 i=0;
    U8 ResponseCode;
    BtDeviceRecord record;

    parm = (U8 *)Event->p.hciAmpEvent.parms;
    controllerId = Event->p.hciAmpEvent.controllerId;

	result.status = parm[0];
	result.physical_link_hdl = parm[1];
    a2mp_main_channel = A2MP_FindMainChannelByPhysicalLink(result.physical_link_hdl);
    if ((result.status != 0x00) && (a2mp_main_channel ==0))
    {
        Report(("A2MP Channel not existing"));
        return;
    }
    else if(a2mp_main_channel ==0)
    {
        Assert(0);
    }
    a2mp_main_channel->ampPhysicalLinkCreating = 0;
    SEC_AddDeviceRecord(&record);
    a2mp_main_channel->remDev->remoteWifiAMP.initiator = 0;
    if(result.status == 0x00)
    {
        /* No Error */
        BtA2MP_HandlePhysicalChannelCreatedDuringCreateChannel(a2mp_main_channel);
        for(i=0;i<NUM_A2MP_SERVERS;i++)
        {
            Report(("a2mp_main_channel->a2mp_channel[i].state:%d",a2mp_main_channel->a2mp_channel[i].state));
            a2mp_main_channel->physicalLinkHandler = parm[1];
            if((a2mp_main_channel->a2mp_channel[i].waitPhysicalLinkCompleted == 1) &&
                (a2mp_main_channel->a2mp_channel[i].state == A2MP_STATE_ALLOCATED))
            {
                a2mp_main_channel->physicalLinkHandler = parm[1];

				L2Cap_AMPPhysicalLinkCompleted(a2mp_main_channel->remDev,
							a2mp_main_channel->a2mp_channel[i].l2ChannelId);
                a2mp_main_channel->a2mp_channel[i].waitPhysicalLinkCompleted = 0;
            }
        }
    }
    else
    {
        for(i=0;i<NUM_A2MP_SERVERS;i++)
        {
            Report(("a2mp_main_channel->a2mp_channel[i].state:%d",a2mp_main_channel->a2mp_channel[i].state));
            a2mp_main_channel->a2mp_channel[i].waitPhysicalLinkCompleted = 0;
            A2MP_FreePSMChannel(&(a2mp_main_channel->a2mp_channel[i]));
        }
        BTA2MP_HandleAMPAbnormal(a2mp_main_channel->remDev, A2MP_PHYSICAL_LINK_CREATE_FAILED_ERROR);
    }

}

void A2MPHandleChannelSelected(const BtEvent *Event)
{
	U8 *parm;
    U8 controllerId;
    U8 physicalLinkHdl;
    A2MP_MAIN_CONN *a2mp_main_channel;

    parm = (U8 *)Event->p.hciAmpEvent.parms;
    controllerId = Event->p.hciAmpEvent.controllerId;
    physicalLinkHdl = parm[0];
    a2mp_main_channel = A2MP_FindMainChannelByPhysicalLink(physicalLinkHdl);
    Report(("a2mp_main_channel->remDev->remoteWifiAMP.state:%d", a2mp_main_channel->remDev->remoteWifiAMP.state));
    if((a2mp_main_channel !=0) && (a2mp_main_channel->remDev !=0) &&
        (a2mp_main_channel->remDev->remoteWifiAMP.initiator == 1))
    {
        a2mp_main_channel->remDev->remoteWifiAMP.read_local_assoc.length_so_far = 0;
        a2mp_main_channel->remDev->remoteWifiAMP.read_local_assoc.amp_assoc_length = 248;
        a2mp_main_channel->local_amp_assoc_size = 0;
        a2mp_main_channel->ampNeedToCreatePhysicalLink = 1;
        ME_ReadLocalAMPAssoc(&a2mp_main_channel->remDev->remoteWifiAMP);
    }
}

void A2MPHandlePhysicalLinkDisconnectComplete(const BtEvent *Event)
{
	BT_AMP_HCI_EVENT_PHYSICAL_LINK_DISCONNECT result;
	U8 *parm;
    U8 controllerId;
    A2MP_MAIN_CONN *a2mp_main_channel;
    U8 i=0;
    parm = (U8 *)Event->p.hciAmpEvent.parms;
    controllerId = Event->p.hciAmpEvent.controllerId;

	result.status = parm[0];
	result.physical_link_hdl = parm[1];
	result.reason = parm[2];
    a2mp_main_channel = A2MP_FindMainChannelByPhysicalLink(result.physical_link_hdl);
    if(a2mp_main_channel !=0)
    {
        a2mp_main_channel->physicalLinkHandler = 0;
        a2mp_main_channel->remDev->remoteWifiAMP.initiator = 0;
        a2mp_main_channel->remDev->remoteWifiAMP.physical_link_sate = BDS_DISCONNECTED;
    	for(i=0;i<NUM_A2MP_SERVERS; i++)
    	{
            Report(("i:%d",i));
    	    Report(("a2mp_main_channel->a2mp_channel[i].state:%d",a2mp_main_channel->a2mp_channel[i].state));
            Report(("a2mp_main_channel->a2mp_channel[i].psm:%d",a2mp_main_channel->a2mp_channel[i].psm));
          A2MP_FreePSMChannel(&a2mp_main_channel->a2mp_channel[i]);
    	}
    }
    /* No handle here, wait the logical link disconnected. */
}

void A2MPHandlePhysicalLinkWarning(const BtEvent *Event)
{
	BT_AMP_HCI_EVENT_PHYSICAL_LINK_WARNING result;
	U8 *parm;
    U8 controllerId;

    parm = (U8 *)Event->p.hciAmpEvent.parms;
    controllerId = Event->p.hciAmpEvent.controllerId;

	result.physical_link_hdl = parm[0];
	result.link_loss_reason = parm[1];
}

void A2MPHandlePhysicalLinkRecovery(const BtEvent *Event)
{
	BT_AMP_HCI_EVENT_PHYSICAL_LINK_RECOVERY result;
	U8 *parm;
    U8 controllerId;

    parm = (U8 *)Event->p.hciAmpEvent.parms;
    controllerId = Event->p.hciAmpEvent.controllerId;

	result.physical_link_hdl = parm[0];
}

void A2MPHandleLogicalLinkComplete(const BtEvent *Event)
{
	BT_AMP_HCI_EVENT_LOGICAL_LINK_COMPLETE result;
	U8 *parm;
    U8 controllerId;
    A2MP_MAIN_CONN *a2mp_main_channel;
    U8 i=0;
    BtRemoteAMPDevice *link;

    parm = (U8 *)Event->p.hciAmpEvent.parms;
    controllerId = Event->p.hciAmpEvent.controllerId;
	result.status = parm[0];
	result.logical_link_hdl = LEtoHost16(parm+1);
	result.physical_link_hdl = parm[3];
	result.tx_flow_spec_id = parm[4];
    a2mp_main_channel = A2MP_FindMainChannelByPhysicalLink(result.physical_link_hdl);
    if(a2mp_main_channel ==0)
        Assert(0);
    link = &a2mp_main_channel->remDev->remoteWifiAMP;

    for(i=0;i< BT_AMP_SUPPORTED_LOGICAL_CHANNEL; i++)
    {
        if(link->logicalLink[i].logical_link_sate == BDS_LOGICAL_CON)
        {
            if (result.status == 0x00)
            {
                link->logicalLink[i].logical_link_sate = BDS_CONNECTED;
                Report(("Create Logical Link success"));
                L2Cap_NotifyLogicalChannelCreated(link->logicalLink[i].l2cap_id, BT_STATUS_SUCCESS);
            }
            else
            {
                Report(("Create Logical Link failed"));
                link->logicalLink[i].logical_link_sate = BDS_DISCONNECTED;
                L2Cap_NotifyLogicalChannelCreated(link->logicalLink[i].l2cap_id, BT_STATUS_FAILED);
            }
        }
    }
}

void A2MPHandleLogicalLinkDisconnectComplete(const BtEvent *Event)
{
	BT_AMP_HCI_EVENT_LOGICAL_LINK_DISCONNECT result;
	BTRemoteAMPLogicalLink *parm;


    parm = (BTRemoteAMPLogicalLink *)Event->p.hciAmpEvent.parms;
    Report(("A2MPHandleLogicalLinkDisconnectComplete"));
    L2CAP_NotifyLogicalLinkDisconnected(parm->link, parm->l2cap_id);
}

void A2MPHandleFlowSpecModifyComplete(const BtEvent *Event)
{
	BT_AMP_HCI_EVENT_FLOW_STATUS_MODIFY result;
	U8 *parm;
    U8 controllerId;

    parm = (U8 *)Event->p.hciAmpEvent.parms;
    controllerId = Event->p.hciAmpEvent.controllerId;

	result.status = parm[0];
	result.con_hdl = LEtoHost16(parm+1);
}


void A2MPHandleAMPStatusChange(const BtEvent *Event)
{
	BT_AMP_HCI_EVENT_AMP_STATUS_CHANGE result;
	U8 *parm;
    BtA2MPChangeNotify a2mp_parm;
    U8 controllerId;
	A2MP_MAIN_CONN *main_channel;
    U8 i = 0;

    parm = (U8 *)Event->p.hciAmpEvent.parms;
    controllerId = Event->p.hciAmpEvent.controllerId;

    a2mp_parm.controllerList = (U8 *)A2MPC(controllerList);
    a2mp_parm.controllerListSize = (U16)A2MPC(controllerListSize);
    if(parm[0] == 0x00)
    {
        Report(("Change the controller List"));
        A2MP_ChangeAMPControllerStatus(Event->p.hciAmpEvent.controllerId, parm[1]);
        for(i=0;i<NUM_BT_DEVICES; i++)
        {
            main_channel = &A2MPC(a2mp_main_channel)[i];
            Report(("A2MPC(a2mp_main_channel)[%d].state:%d",i, main_channel->state));
            Report(("A2MPC(a2mp_main_channel)[%d].discoverDone:%d",i, main_channel->discoverDone));
            if((main_channel->state == A2MP_STATE_ALLOCATED) &&
                (main_channel->discoverDone == 1))
            {
                BTA2MP_SendChangeNotify(main_channel->remDev,&a2mp_parm);
            }
        }
#if 0
    Report(("AMP status change to :%d", parm[1]));
    switch(parm[1])
    {
        case 0x00: /* Physical power down. */
        case 0x02: /* No capability for BT */
            L2CAP_NotifyAMPChannelNotAvailable(link, 1);
        default:
            break;
    }
#endif

   }
}

void A2MPHandleLogicalLinkCancel(const BtEvent *Event)
{
    BT_AMP_HCI_EVENT_PHYSICAL_LINK_CANCEL result;
	U8 *parm;
    U8 controllerId;

    parm = (U8 *)Event->p.hciAmpEvent.parms;
    controllerId = Event->p.hciAmpEvent.controllerId;
    result.physical_link_handle = parm[1];
    result.tx_flow_spec_id = parm[2];

}

void A2MPHandleReadLocalAMPInfo(const BtEvent *Event)
{
    BtA2MPGetInfoRsp result;
    U8 *parm;
    U8 controllerId;
    U8 i=0;

	A2MP_MAIN_CONN *main_channel;

    parm = (U8 *)Event->p.hciAmpEvent.parms;
    result.controllerId = Event->p.hciAmpEvent.controllerId;
    if(Event->errCode ==0)
        result.status = 0x00; /* AMP get info response success*/
    else
        result.status = 0x01; /* AMP get info response failed */
    result.total_bandwidth = LEtoHost32(parm+2);
    result.max_guarantee_bandwidth = LEtoHost32(parm+6);
    result.min_latency = LEtoHost32(parm+10);
    result.max_pdu_size = LEtoHost32(parm+14);
    result.controller_type = parm[18];
    result.pal_capability = LEtoHost16(parm+19);
    result.max_amp_assoc_length = LEtoHost16(parm+21);
    result.max_flush_timeout = LEtoHost32(parm+23);
    result.best_effort_flush_timeout= LEtoHost32(parm+27);
    for(i=0;i<NUM_BT_DEVICES; i++)
    {
        if(A2MPC(a2mp_main_channel)[i].state == A2MP_STATE_ALLOCATED)
        {
            main_channel = &A2MPC(a2mp_main_channel)[i];
            if(main_channel->last_rxCmdopcode == A2MP_CODE_GET_INFO_REQ)
            {
                BTA2MP_SendGetInfoResponse(main_channel->remDev, &result);
                main_channel->last_rxCmdopcode = A2MP_CODE_INVALID;
            }
        }
    }


}

void A2MPHandleReadLocalAMPAssoc(const BtEvent *Event)
{
    BT_AMP_HCI_EVENT_READ_LOCAL_AMP_ASSOC result;
	U8 *parm;
    U8 controllerId;
    A2MP_MAIN_CONN *a2mp_channel;
    U8 length;
    U8 amp_assoc_size;
    BtA2MPGetAMPAssocRsp a2mp_parm;
    BtA2MPCreatePhysicalLinkReq a2mp_create_parm;

    parm = (U8 *)Event->p.hciAmpEvent.parms;
    controllerId = Event->p.hciAmpEvent.controllerId;
    result.physical_link_handle = parm[1];
    result.amp_assoc_remaining_length = LEtoHost16(parm+2);
    result.amp_assoc_fragment = parm+4;

    a2mp_channel = A2MP_FindMainChannelByPhysicalLink(result.physical_link_handle);
    if(a2mp_channel ==0)
    {
        /* Error*/
        Assert(0);
    }
    if(Event->errCode !=0)
    {
        a2mp_parm.controllerId = controllerId;
        a2mp_parm.ampAssocSize = 0x00;
        a2mp_parm.ampAssoc = NULL;
        a2mp_parm.status = 0x01;
        BTA2MP_SendGetAMPAssocResponse(a2mp_channel->remDev, &a2mp_parm);
        return;
    }
    if(parm[0] <= 4)
        Assert(0);
    amp_assoc_size = (parm[0] -4);
    a2mp_channel->remDev->remoteWifiAMP.read_local_assoc.amp_assoc_length = 248;
    a2mp_channel->remDev->remoteWifiAMP.read_local_assoc.length_so_far += amp_assoc_size;

    memcpy(a2mp_channel->local_amp_assoc + a2mp_channel->local_amp_assoc_size,
        result.amp_assoc_fragment,
        amp_assoc_size);
    a2mp_channel->local_amp_assoc_size += amp_assoc_size;
    if((result.amp_assoc_remaining_length - amp_assoc_size) !=0)
        ME_ReadLocalAMPAssoc(&a2mp_channel->remDev->remoteWifiAMP);
    else
    {
        if(a2mp_channel->last_rxCmdopcode == A2MP_CODE_GET_AMP_ASSOC_REQ)
        {
            a2mp_parm.controllerId = controllerId;
            a2mp_parm.ampAssocSize = a2mp_channel->local_amp_assoc_size;
            a2mp_parm.ampAssoc = a2mp_channel->local_amp_assoc;
            a2mp_parm.status = 0x00;
            BTA2MP_SendGetAMPAssocResponse(a2mp_channel->remDev, &a2mp_parm);
        }
        if(a2mp_channel->ampNeedToCreatePhysicalLink == 1)
        {
            a2mp_channel->ampNeedToCreatePhysicalLink = 0;
            a2mp_create_parm.ampAssoc = a2mp_channel->local_amp_assoc;
            a2mp_create_parm.ampAssocSize = a2mp_channel->local_amp_assoc_size;
            a2mp_create_parm.localControllerId = a2mp_channel->localControllerId;
            a2mp_create_parm.remoteControllerId = a2mp_channel->remDev->remoteWifiAMP.remoteControllerId;
            BTA2MP_SendCreatePhysicalLinkRequest(a2mp_channel->remDev, &a2mp_create_parm);
        }
    }
}

void A2MPHandleWriteRemoteAMPAssoc(const BtEvent *Event)
{
    BT_AMP_HCI_EVENT_WRITE_REMOTE_AMP_ASSOC result;
	U8 *parm;
    U8 controllerId;
    A2MP_MAIN_CONN *a2mp_channel;

    parm = (U8 *)Event->p.hciAmpEvent.parms;
    controllerId = Event->p.hciAmpEvent.controllerId;
    result.physical_link_handle = parm[1];
    a2mp_channel = A2MP_FindMainChannelByPhysicalLink(result.physical_link_handle);
    if(a2mp_channel !=0)
    {
        if(a2mp_channel->remDev->remoteWifiAMP.write_remote_assoc.length_so_far !=
            a2mp_channel->remDev->remoteWifiAMP.write_remote_assoc.amp_assoc_size)
        {
            ME_WriteRemoteAMPAssoc(&a2mp_channel->remDev->remoteWifiAMP);
        }
        else
        {
            a2mp_channel->remDev->remoteWifiAMP.write_remote_assoc.length_so_far = 0;
            a2mp_channel->remDev->remoteWifiAMP.write_remote_assoc.amp_assoc_fragment = 0;
            a2mp_channel->remDev->remoteWifiAMP.write_remote_assoc.amp_assoc_size = 0;
            a2mp_channel->remDev->remoteWifiAMP.write_remote_assoc.amp_assoc_remain_length = 0;
        }
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
void A2MPMainChannelL2Callback(L2capChannelId L2ChannelId, L2capCallbackParms *info)
{
	A2MP_MAIN_CONN *main_channel;
	A2MP_CONN *amp_psm_channel;
	BtRxPacket *rxPacket;
	U8 i=0;

	Report(("A2MPMainChannelL2Callback:%d,L2ChannelId:%d", info->event,L2ChannelId));
    switch (info->event)
    {
        case L2EVENT_DISCONNECTED:
             break;
        case L2EVENT_NOTIFY_AMP_PAL_CHANNEL_DISCONNECTED:
            main_channel = A2MP_FindMainChannel(info->aclLink);
            if(main_channel !=0)
            {
                Report(("The L2ChannelId:%d",L2ChannelId));
                Report(("The main_channel->localCid:%d",main_channel->localCid));
                 if(info->ptrs.NotifyL2CAPA2MPPDisconnected.localCid != main_channel->localCid)
//                    ME_DisconnectPhysicalLink(&info->aclLink->remoteWifiAMP, 0x13);
                {
                    BTA2MP_PhysicalLinkClose(info->aclLink);
                }
            }
             break;
        case L2EVENT_NOTIFY_AMP_NORMAL_CHANNEL_DISCONNECTED:
            main_channel = A2MP_FindMainChannel(info->aclLink);
            if(main_channel !=0)
            {
                Report(("The L2ChannelId:%d",L2ChannelId));
                Report(("The main_channel->localCid:%d",main_channel->localCid));
                for(i=0; i< BT_AMP_SUPPORTED_LOGICAL_CHANNEL; i++)
                {
                    Report(("%d,%d,%d,%d",main_channel->remDev->remoteWifiAMP.logicalLink[i].l2cap_id,
                        main_channel->remDev->remoteWifiAMP.logicalLink[i].logical_link_sate,
                        main_channel->remDev->remoteWifiAMP.logicalLink[i].logical_link_type,
                        info->ptrs.NotifyL2CAPA2MPPDisconnected.localCid));
                    if((main_channel->remDev->remoteWifiAMP.logicalLink[i].l2cap_id == info->ptrs.NotifyL2CAPA2MPPDisconnected.localCid) &&
                        (main_channel->remDev->remoteWifiAMP.logicalLink[i].logical_link_sate == BDS_CONNECTED) &&
                        (main_channel->remDev->remoteWifiAMP.logicalLink[i].logical_link_type > 0x01))
                    {
                        ME_DisconnectLogicalLink(&info->aclLink->remoteWifiAMP, main_channel->remDev->remoteWifiAMP.logicalLink[i].logical_link_handle);
                        break;
                    }
                }
            }
            break;
        case L2EVENT_AMP_CHANNEL_DETECTED:
            if(info->status == 0x00)
            {
                if(info->aclLink->remoteWifiAMP.detected == 0)
                {
                    BtA2MPDiscoveryReq discoveryReq;
                    U8 extendedFeatureMask[2];

                    /* Start AMP discovery procedure. */
                    info->aclLink->remoteWifiAMP.detected = 1;
                    info->aclLink->remoteWifiAMP.exists = 0;
                    discoveryReq.mtu = 670;
                    discoveryReq.extendedFeatureMaskSize = 0;
                    extendedFeatureMask[0] = 0x00;
                    extendedFeatureMask[1] = 0x00;
                    discoveryReq.extendedFeatureMask = (U8 *)extendedFeatureMask;
                    discoveryReq.extendedFeatureMaskSize = 2;
                    BTA2MP_SendDiscoveryRequest(info->aclLink, &discoveryReq);
                }
                else
                {
                    Report(("Remote A2MP feature detected before, no A2MP operation"));
                }
            }
            break;
        case L2EVENT_PACKET_HANDLED:
            {
                BtA2MPPacket *a2mptxPacket;
    			main_channel = A2MP_FindMainChannel(info->aclLink);
    			if	(info->ptrs.packet->flags & BTP_FLAG_LSYSTEM)
    				Assert(0);

                a2mptxPacket = ContainingRecord(info->ptrs.packet, BtA2MPPacket, btPacket);
    			InsertTailList(&main_channel->freeTtxPacketsList, &(a2mptxPacket->node));
            }
            break;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
        case L2EVENT_FLOW_DATA_IND:
            Report(("L2EVENT_FLOW_DATA_IND"));
            main_channel = A2MP_FindMainChannel(info->aclLink);
            rxPacket = (BtRxPacket *)RemoveHeadList(&main_channel->receivedPacketsList);
            info->ptrs.data = rxPacket->data;
            info->dataLen = rxPacket->dataLen;
            BTA2MP_RecvDataInd(info->aclLink, info->ptrs.data, info->dataLen);
            rxPacket->dataLen = 0;
            InsertTailList(&main_channel->rxPacketsBufferList, &rxPacket->node);
            L2CAP_UpdateSduDataBufferSeq(main_channel->localCid);
			break;
#endif

        case L2EVENT_LOCKSTEP_CONFIG_REQ:
            Report(("info->ptrs.lockstepConfigReq.createChannelnitiator:%d",info->ptrs.lockstepConfigReq.createChannelnitiator));
            Assert(0);
            break;
		case L2EVENT_MOVE_CHANNEL_REQ:
/*
			info->ptrs.moveChannelReq;
*/
			amp_psm_channel = A2MP_FindPSMChannelByCid(L2ChannelId);
			break;
		case L2EVENT_MOVE_CHANNEL_RSP:
			amp_psm_channel = A2MP_FindPSMChannelByCid(L2ChannelId);
/*
			info->ptrs.moveChannelRsp.result;
*/

			break;
		case L2EVENT_MOVE_CHANNEL_CNF_REQ:
			amp_psm_channel = A2MP_FindPSMChannelByCid(L2ChannelId);
/*
			info->ptrs.moveChannelCnfReq.result;
*/
			break;
		case L2EVENT_MOVE_CHANNEL_CNF_RSP:
			amp_psm_channel = A2MP_FindPSMChannelByCid(L2ChannelId);
			break;
    }
}


void Me2A2MPCallback(const BtEvent *Event)
{
    switch (Event->eType)
    {
        case BTEVENT_LINK_DISCONNECT:
            A2MPHandleDisconnectInd(Event);
            break;
        case BTEVENT_LINK_CONNECT_IND:
            A2MPHandleConnectInd(Event);
            break;
        case BTEVENT_LINK_CONNECT_CNF:
            A2MPHandleConnectCfm(Event);
            break;
        case BTEVENT_PHYSICAL_LINK_COMPLETE:
            A2MPHandlePhysicalLinkComplete(Event);
            break;
        case BTEVENT_CREATE_PHYSICAL_LINK_CMD_STATUS:
            A2MPHandleCreatePhysicalLinkStatus(Event);
            break;
        case BTEVENT_ACCEPT_PHYSICAL_LINK_CMD_STATUS:
            A2MPHandleAcceptPhysicalLinkStatus(Event);
            break;
        case BTEVENT_CHANNEL_SELECTED:
            A2MPHandleChannelSelected(Event);
            break;
        case BTEVENT_DISCONNECT_PHYSICAL_LINK:
            A2MPHandlePhysicalLinkDisconnectComplete(Event);
            break;
        case BTEVENT_PHYSICAL_LINK_EARLY_WARNING:
            A2MPHandlePhysicalLinkWarning(Event);
            break;
        case BTEVENT_PHYSICAL_LINK_RECOVERY:
            A2MPHandlePhysicalLinkRecovery(Event);
            break;
        case BTEVENT_LOGICAL_LINK_COMPLETE:
            A2MPHandleLogicalLinkComplete(Event);
            break;
        case BTEVENT_LOGICAL_LINK_CANCEL:
            A2MPHandleLogicalLinkCancel(Event);
            break;
        case BTEVENT_DISCONNECT_LOGICAL_LINK:
            A2MPHandleLogicalLinkDisconnectComplete(Event);
            break;
        case BTEVENT_AMP_STATUS_CHANGE:
            A2MPHandleAMPStatusChange(Event);
            break;
        case BTEVENT_AMP_FLOW_SPEC_MODIFY:
            A2MPHandleFlowSpecModifyComplete(Event);
            break;
        case BTEVENT_READ_LOCAL_AMP_INFO:
            A2MPHandleReadLocalAMPInfo(Event);
            break;
        case BTEVENT_READ_LOCAL_AMP_ASSOC:
            A2MPHandleReadLocalAMPAssoc(Event);
            break;
        case BTEVENT_WRITE_REMOTE_AMP_ASSOC:
            A2MPHandleWriteRemoteAMPAssoc(Event);
            break;
        default:
            break;
    }
}

void A2MPHandleA2MPMainChannelDisconnected(BtRemoteDevice *link)
{
	A2MP_MAIN_CONN *main_channel = 0;

    main_channel = A2MP_FindMainChannel(link);
    if(main_channel !=0)
    {
        A2MP_StopTimer(main_channel);
        A2MP_FreeMainChannel(main_channel);
    }
}


#endif

