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
#include "a2mp.h"
#include "bt_feature.h"
#include "a2mp_adp.h"
#include "meamp.h"

#if A2MP_PROTOCOL == XA_ENABLED

U8 BT_PAL_CONTROLLER_LIST[] = 
{
    0x00, 0x00, 0x01, /* Controller Id:0, Basic/EDR controller, full dedicated to BT */
    BT_PAL_CONTROLLERID, 0x01, 0x01 /* Controller Id:01, 802.11 AMP Controller, full dedicated to BT */
    /* We can not change controller id for 802.11 AMP because we have fixed the controller id in our codebase.
    A2MP_GetAMPControllerStatus
    A2MP_GetLocalAMPController
    */
};



extern A2MP_MAIN_CONN *A2MP_FindMainChannel(BtRemoteDevice *link);
extern U8 IsListEmpty(ListEntry *list);

void A2MPMainChannelL2Callback(L2capChannelId L2ChannelId, L2capCallbackParms *info);
void Me2A2MPCallback(const BtEvent *Event);
U8 A2MP_GetControllerIdByIndex(U8 index);
BtStatus A2MP_Init(void)
{
    U8 i=0;

    ME_InitHandler(&A2MPC(a2mpMeHandler));
    A2MPC(a2mpMeHandler).callback = Me2A2MPCallback;
    A2MPC(nextIdent) = 1;
    if(sizeof(BT_PAL_CONTROLLER_LIST) != BT_PAL_CONTROLLER_LIST_SIZE)
    {
        Assert(0);
    }
    if (ME_RegisterGlobalHandler(&A2MPC(a2mpMeHandler)) == BT_STATUS_SUCCESS)
    {
        if (ME_SetEventMask(&A2MPC(a2mpMeHandler), BEM_ALL_EVENTS) != BT_STATUS_SUCCESS)
        {
            return FALSE;
        }
	}

    A2MPC(controllerList) = (U8 *)BT_PAL_CONTROLLER_LIST;
    A2MPC(controllerListSize) = sizeof(BT_PAL_CONTROLLER_LIST);

    A2MPC(psm).callback = A2MPMainChannelL2Callback;
    A2MPC(psm).psm = BT_PSM_A2MP;
    A2MPC(psm).localMtu = 670;
    A2MPC(psm).minRemoteMtu = 670;
    A2MPC(psm).authorizedSetting = FALSE;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    A2MPC(psm).inLinkMode = L2MODE_ENHANCEDMENT_RETRANSMISSION;
    A2MPC(psm).outLinkMode = L2MODE_ENHANCEDMENT_RETRANSMISSION;
    A2MPC(psm).op_state = L2OPERATORSTATE_STATE1;
    A2MPC(psm).inTransmitMax = 0xff;
    A2MPC(psm).channelUsage = L2CAP_CHANNEL_DEDICATED;

    A2MPC(psm).inBufferSize = 670;
    A2MPC(psm).inRetransTimeout = 2000;
    A2MPC(psm).inMonitorTimeout = 12000;
    A2MPC(psm).l2capExtFlowSpec.serviceType = L2QOS_OPTION_BEST_EFFORT;
    A2MPC(psm).l2capExtFlowSpec.maxSduSize = 670;
    A2MPC(psm).l2capExtFlowSpec.sduIntArrialTime = 0xFFFFFFFF;
    A2MPC(psm).l2capExtFlowSpec.accessLatency = 0xFFFFFFFF;
    A2MPC(psm).l2capExtFlowSpec.flushTimeout = 0xFFFFFFFF;
    A2MPC(psm).lockStepNeeded = 0;
    A2MPC(psm).lockStepBRNeeded = 0;
    A2MPC(psm).highSpeedRequired = 0;    

#endif /* L2CAP_FLOW_NON_BASIC_MODE== XA_ENABLED */
    if (L2CAP_RegisterPsm(&(A2MPC(psm))) != BT_STATUS_SUCCESS)
    {
        /* Unable to register */
        return BT_STATUS_FAILED;
    }
    
    return BT_STATUS_SUCCESS;
}

void A2MP_Deinit(void)
{
#if L2CAP_DEREGISTER_FUNC == XA_ENABLED
    L2CAP_DeregisterPsm(&(RFC(psm)));
#endif /* L2CAP_DEREGISTER_FUNC == XA_ENABLED */
}

U8 A2MP_GetLocalAMPController(U8 controllerType)
{
    U8 i=0;
    for(i=0; i<2; i++)
    {
        if(A2MP_GetControllerIdByIndex(i*3+1) == controllerType)
        {
            Report(("The controller id are :%d",A2MP_GetControllerIdByIndex(i*3)));
            return A2MP_GetControllerIdByIndex(i*3);
        }
    }
    return 0xFF;
}



A2MP_MAIN_CONN *A2MP_AllocateMainChannel(BtRemoteDevice *link)
{
	U8 i=0;
	U8 j = 0;
	A2MP_MAIN_CONN *a2mp_main_channel;
	
	for(i=0;i<NUM_BT_DEVICES; i++)
	{
		if(A2MPC(a2mp_main_channel)[i].state == A2MP_STATE_IDLE)
		{
		    OS_MemSet((U8 *)&A2MPC(a2mp_main_channel)[i], 0, sizeof(A2MP_MAIN_CONN));
			A2MPC(a2mp_main_channel)[i].state = A2MP_STATE_ALLOCATED;
            Report(("A2MP_AllocateMainChannel"));
			A2MPC(a2mp_main_channel)[i].remDev = link;
            OS_MemSet((U8 *)&link->remoteWifiAMP, 0, sizeof(BtRemoteAMPDevice));
			a2mp_main_channel = &(A2MPC(a2mp_main_channel)[i]);
			EVM_ResetTimer(&A2MPC(a2mp_main_channel)[i].timer);
			InitializeListHead(&(a2mp_main_channel->freeTtxPacketsList));
            a2mp_main_channel->last_rxCmdopcode = A2MP_CODE_INVALID;  
            a2mp_main_channel->ampNeedToCreatePhysicalLink = 0;
            a2mp_main_channel->discoverDone = 0;
            a2mp_main_channel->ampPhysicalLinkCreating = 0;
			for(j=0; j<A2MP_CHANNEL_MAX_TX_PACKET; j++)
			{
				InsertTailList(&a2mp_main_channel->freeTtxPacketsList,
						&(a2mp_main_channel->a2mp_txPacket[j].node));
                
			}
            A2MPC(a2mp_main_channel)[i].localControllerId =  A2MP_GetLocalAMPController(BT_A2MP_CONTROLLER_TYPE_80211);
            link->remoteWifiAMP.physical_link_handle = i+10;
			return &(A2MPC(a2mp_main_channel)[i]);
		}
	}
	return 0;
}


void A2MP_FreeMainChannel(A2MP_MAIN_CONN *a2mp_main_channel)
{
	if(a2mp_main_channel->state == A2MP_STATE_ALLOCATED)
	{
        Report(("A2MP_FreeMainChannel"));
	
		a2mp_main_channel->state = A2MP_STATE_IDLE;
		EVM_ResetTimer(&a2mp_main_channel->timer);
		a2mp_main_channel->remDev = 0;
	}
}

U8 A2MP_GetLogicalIndex(BtRemoteDevice *link)
{
	U8 i=0;
	A2MP_MAIN_CONN *a2mp_main_channel;

    a2mp_main_channel = A2MP_FindMainChannel(link);
    if((a2mp_main_channel !=0) &&(a2mp_main_channel->remDev !=0))
    {
    	for(i=0;i<BT_AMP_SUPPORTED_LOGICAL_CHANNEL; i++)
        {   
            if(a2mp_main_channel->remDev->remoteWifiAMP.logicalLink[i].logical_link_sate == BDS_DISCONNECTED)
            {
                a2mp_main_channel->remDev->remoteWifiAMP.logicalLink[i].link = link;
                return i;
            }
        }
    }
	return 0xFF;
}

U8 A2MP_GetLogicalBestEffortIndex(BtRemoteDevice *link)
{
	U8 i=0;
	A2MP_MAIN_CONN *a2mp_main_channel;

    a2mp_main_channel = A2MP_FindMainChannel(link);
    if((a2mp_main_channel !=0) &&(a2mp_main_channel->remDev !=0))
    {
    	for(i=0;i<BT_AMP_SUPPORTED_LOGICAL_CHANNEL; i++)
        {   
            if(((a2mp_main_channel->remDev->remoteWifiAMP.logicalLink[i].logical_link_sate == BDS_CONNECTED) ||
                (a2mp_main_channel->remDev->remoteWifiAMP.logicalLink[i].logical_link_sate == BDS_OUT_CON))
                &&
                (link->remoteWifiAMP.logicalLink[i].logical_link_type <= 0x01))
            {
                return i;
            }
        }
    }
	return 0xFF;
}


A2MP_CONN *A2MP_AllocatePSMChannel(BtRemoteDevice *link, U16 psm)
{
	U8 i=0;
	U8 j = 0;
	A2MP_CONN *a2mp_channel;
	A2MP_MAIN_CONN *a2mp_main_channel;
	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel ==0)
    {
        Report(("A2MP_AllocatePSMChannel A2MP main channel 0"));
		return 0x00;
    }

	for(i=0;i<NUM_A2MP_SERVERS; i++)
	{
        Report(("i:%d",i));
	    Report(("a2mp_main_channel->a2mp_channel[i].state:%d",a2mp_main_channel->a2mp_channel[i].state));
        Report(("a2mp_main_channel->a2mp_channel[i].psm:%d",a2mp_main_channel->a2mp_channel[i].psm));        
		if((a2mp_main_channel->a2mp_channel[i].state != A2MP_STATE_IDLE) &&
			(a2mp_main_channel->a2mp_channel[i].psm == psm))
		{
			return 0;
		}
	}
	
	for(i=0;i<NUM_A2MP_SERVERS; i++)
	{
        Report(("i:%d",i));
	    Report(("a2mp_main_channel->a2mp_channel[i].state:%d",a2mp_main_channel->a2mp_channel[i].state));
        Report(("a2mp_main_channel->a2mp_channel[i].psm:%d",a2mp_main_channel->a2mp_channel[i].psm));        
		if(a2mp_main_channel->a2mp_channel[i].state == A2MP_STATE_IDLE)
		{
            Report(("a2mp_main_channel->a2mp_channel[i].state:%d",a2mp_main_channel->a2mp_channel[i].state));        
			a2mp_main_channel->a2mp_channel[i].state = A2MP_STATE_ALLOCATED;
			a2mp_main_channel->physicalLinkHandler = 0;            
			a2mp_main_channel->a2mp_channel[i].psm = psm;			
			a2mp_channel = &a2mp_main_channel->a2mp_channel[i];
            Report(("a2mp_channel:%08x",a2mp_channel));
			return (a2mp_channel);
		}
	}
	return 0;
}

/* If a channel is allocated but not sent the move channel request */
A2MP_CONN *A2MP_AllocatePSMChannelWaiting(BtRemoteDevice *link, U16 psm)
{
	U8 i=0;
	U8 j = 0;
	A2MP_CONN *a2mp_channel;
	A2MP_MAIN_CONN *a2mp_main_channel;
	a2mp_main_channel = A2MP_FindMainChannel(link);
	if(a2mp_main_channel ==0)
		return 0x00;

	for(i=0;i<NUM_A2MP_SERVERS; i++)
	{
		if((a2mp_main_channel->a2mp_channel[i].state == A2MP_STATE_ALLOCATED) &&
			(a2mp_main_channel->a2mp_channel[i].psm == psm) &&
			(a2mp_main_channel->a2mp_channel[i].waitPhysicalLinkCompleted == 1))
		{
		    a2mp_main_channel->a2mp_channel[i].waitPhysicalLinkCompleted = 0;
            a2mp_channel = &a2mp_main_channel->a2mp_channel[i];
			return a2mp_channel;
		}
	}
	return 0;
}

void A2MP_FreePSMChannel(A2MP_CONN *a2mp_channel)
{
	if(a2mp_channel->state == A2MP_STATE_ALLOCATED)
	{
		a2mp_channel->state = A2MP_STATE_IDLE;
		a2mp_channel->psm = 0;
        a2mp_channel->waitPhysicalLinkCompleted = 0;
	}
}

A2MP_MAIN_CONN *A2MP_FindMainChannel(BtRemoteDevice *link)
{
	U8 i=0;
	A2MP_MAIN_CONN *a2mp_main_channel;

	for(i=0;i<NUM_BT_DEVICES; i++)
	{
		if((A2MPC(a2mp_main_channel)[i].state == A2MP_STATE_ALLOCATED) &&
			(A2MPC(a2mp_main_channel)[i].remDev == link))
		{
			return &A2MPC(a2mp_main_channel)[i];
		}
	}
	return 0;
}

A2MP_MAIN_CONN *A2MP_FindMainChannelByPhysicalLink(U8 physical_link_hdl)
{
	U8 i=0;
	A2MP_MAIN_CONN *a2mp_main_channel;

	for(i=0;i<NUM_BT_DEVICES; i++)
	{
		if((A2MPC(a2mp_main_channel)[i].state == A2MP_STATE_ALLOCATED) &&
			(A2MPC(a2mp_main_channel)[i].remDev->remoteWifiAMP.physical_link_handle == physical_link_hdl))
		{
			return &A2MPC(a2mp_main_channel)[i];
		}
	}
	return 0;
}

BTRemoteAMPLogicalLink *A2MP_FindLogicalLinkByHdl(BtRemoteAMPDevice *link, U16 logical_link_hdl)
{

	U8 i=0;

	for(i=0;i<BT_AMP_SUPPORTED_LOGICAL_CHANNEL; i++)
	{
		if((link->logicalLink[i].logical_link_sate == BDS_CONNECTED) &&
			(link->logicalLink[i].logical_link_handle == logical_link_hdl))
		{
			return &link->logicalLink[i];
		}
	}
	return 0;
}



A2MP_CONN *A2MP_FindPSMChannelByCid(U16 cid)
{
	U8 i=0;
	U8 j=0;
	A2MP_CONN *a2mp_channel;
	A2MP_MAIN_CONN *a2mp_main_channel;

	for(i=0;i<NUM_BT_DEVICES; i++)
	{
		a2mp_main_channel = &A2MPC(a2mp_main_channel)[i];

		if(a2mp_main_channel->state == A2MP_STATE_ALLOCATED)
		{
			for(j=0;j<NUM_A2MP_SERVERS;j++)
			{
				if((a2mp_main_channel->a2mp_channel[j].state == A2MP_STATE_ALLOCATED) &&
					(a2mp_main_channel->a2mp_channel[j].l2ChannelId == cid))
                    return &(a2mp_main_channel->a2mp_channel[j]);
			}
		}
	}
	return 0;
}



#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)

void A2MP_RegisterSecurityRecord(void)
{
    BtStatus secStatus;

#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
	InitializeListEntry(&(A2MPC(secRecord).node));
	A2MPC(secRecord).id = SEC_L2CAP_ID;
	A2MPC(secRecord).channel = BT_PSM_A2MP;
	A2MPC(secRecord).level = BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT;
	A2MPC(secRecord).level_type = BT_SEC_MED;
#endif 

	secStatus = SEC_RegisterRecord(&A2MPC(secRecord));
    bt_trace(TRACE_GROUP_1, REGISTRATION_SECURITY_RECORD_STATUSBT_DEBUGx02X, secStatus);
    return;
}


void A2MP_DeRegisterSecurityRecord(void)
{
    BtStatus secStatus;

    secStatus = SEC_Unregister((BtSecurityRecord *)&(A2MPC(secRecord).node));
    bt_trace(TRACE_GROUP_1, DEREGISTRATION_SECURITY_RECORD_STATUSBT_DEBUGx02X, secStatus);
    return;
}

BtStatus A2MP_AllocateL2CAPFixChannel(BtRemoteDevice *link)
{
	U8 i;
	A2MP_MAIN_CONN *a2mp_main_channel;
	L2capConnSettings setting;
	BtStatus status;
	
	a2mp_main_channel = A2MP_AllocateMainChannel(link);
	if(a2mp_main_channel  == 0)
		return BT_STATUS_FAILED;
    InitializeListHead(&a2mp_main_channel->rxPacketsBufferList);
    InitializeListHead(&a2mp_main_channel->receivedPacketsList);
    
    OS_MemSet((U8 *)&a2mp_main_channel->l2capRawData, 0, sizeof(a2mp_main_channel->l2capRawData));
    for (i = 0; i < A2MP_NUM_RETRANSMISSION_WINDOW_SIZE ; i++)
    {
        InitializeListEntry(&(a2mp_main_channel->RxPackets[i].node));
        a2mp_main_channel->RxPackets[i].data = (U8 *)(a2mp_main_channel->l2capRawData[i]);
        InsertTailList(&a2mp_main_channel->rxPacketsBufferList, &a2mp_main_channel->RxPackets[i].node);
    }        
	
	setting.reasmBuffer = (ListEntry *)&a2mp_main_channel->rxPacketsBufferList;
	setting.receivedBuffer = (ListEntry *)&a2mp_main_channel->receivedPacketsList;
	setting.rxWindowSize = A2MP_NUM_RETRANSMISSION_WINDOW_SIZE;
	OS_MemSet((U8 *)a2mp_main_channel->txMaxCountPerIFrame, 0, 128);
	OS_MemSet((U8 *)a2mp_main_channel->txSrejList, 0, sizeof(a2mp_main_channel->txSrejList));
	setting.txMaxTransmitCount = (U8 *)a2mp_main_channel->txMaxCountPerIFrame;
	setting.txSrejList = (U8 *)a2mp_main_channel->txSrejList;
	setting.maxSrejNo = A2MP_L2CAP_MAX_SREJ_SIZE;


	status = L2CAP_AllocateFixChannel(&A2MPC(psm), link, &setting, &(a2mp_main_channel->localCid));
	return status;
}

BtStatus A2MP_FreeL2CAPFixChannel(BtRemoteDevice *link)
{
	A2MP_MAIN_CONN *a2mp_main_channel;
	a2mp_main_channel = A2MP_FindMainChannel(link);

	L2CAP_FreeFixChannel(a2mp_main_channel->localCid);
    return BT_STATUS_SUCCESS;
}

BtStatus A2MP_SendData(BtRemoteDevice *link, BtPacket *Packet)
{
	U8 i=0;
	BtStatus status;
	
	for(i=0; i< NUM_BT_DEVICES; i++)
	{
		if (A2MPC(a2mp_main_channel)[i].remDev == link)
		{
			break;
		}
	}
	if(i== NUM_BT_DEVICES)
	{
		Report(("A2MP Channel not exists"));
	}
    Packet->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2CAP_SendFixChannelData(A2MPC(a2mp_main_channel)[i].localCid, 0x03, link, Packet);
	return status;
}




#endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 

U16 A2MP_GetExtendedFeatureMaskLength(U8 *data)
{
	U16 i=0;
	U16 length = 0;
	U8 *ptr = data;
	do
	{
		length += 2;

		if((ptr[1] & 0x80) !=0)
		{
			ptr = data+2;
		}
		else
		{
			break;
		}
		i++;
	}while(i <= 30);
	return length;
}

U8 A2MP_GetControllerIdByIndex(U8 index)
{
    U8 result;
    
    result = BT_PAL_CONTROLLER_LIST[index];
    return result;

}

BOOL A2MP_HandleCreatePhysicalLinkCollision(BtRemoteDevice *link)
{
	U8 *local_addr;
	U8 *remote_addr;
	U8 i=0;
	
	local_addr = (U8*) MEC(bdaddr).addr;
	remote_addr = (U8*) link->bdAddr.addr; 
	for(i=0;i<6;i++)
	{
		if(local_addr[i] > remote_addr[i])
		{
			return TRUE;
		}
		else if(local_addr[i] < remote_addr[i])
		{
			return FALSE;
		}
	}
	return TRUE;
}

void A2MP_ChangeAMPControllerStatus(U8 controllerId, U8 status)
{
    U8 i=0;

    for(i=0; i< sizeof(BT_PAL_CONTROLLER_LIST); i++)
    {
        if(BT_PAL_CONTROLLER_LIST[i*3] == controllerId)
        {
            BT_PAL_CONTROLLER_LIST[i*3 +2] = status;
        }
    }
}

U8 A2MP_GetAMPControllerStatus(U8 controllerId)
{
    U8 i=0;
    U8 status = 0x00;

    for(i=0; i< sizeof(BT_PAL_CONTROLLER_LIST); i++)
    {
        if(BT_PAL_CONTROLLER_LIST[i*3] == controllerId)
        {
            status = BT_PAL_CONTROLLER_LIST[i*3 +2];
            break;
        }
    }
    Report(("Controller ID:%d, status:%d",controllerId, status));
    return status;
}

U8 A2MP_GetActivatedControllerId(BtRemoteDevice *link)
{
	A2MP_MAIN_CONN *main_channel;
	A2MP_CONN *amp_psm_channel;	
	U8 i=0;
	
    main_channel = A2MP_FindMainChannel(link);
    return (main_channel->localControllerId);
}

void A2MP_DisconnectAMPChannel(BtRemoteDevice *link, U16 l2cap_id)
{
    ME_DisconnectPhysicalLink(&link->remoteWifiAMP, 0x13);
}

void A2MP_DisconnectRFCOMMChannel(U16 l2cap_id)
{
    L2CAP_DisconnectReq(l2cap_id);
}

BtRemoteAMPDevice *A2MP_FindAMPDevice(U8 controllerId, U8 physicalHandler)
{
	A2MP_MAIN_CONN *main_channel;
    I8 i;
    
	for(i=0;i<NUM_BT_DEVICES; i++)
	{
		if((A2MPC(a2mp_main_channel)[i].state == A2MP_STATE_ALLOCATED) &&
			(A2MPC(a2mp_main_channel)[i].localControllerId == controllerId) &&
			(A2MPC(a2mp_main_channel)[i].physicalLinkHandler == physicalHandler))
		{
			return &A2MPC(a2mp_main_channel)[i].remDev->remoteWifiAMP;
		}
	}
    return 0;
}

BtRemoteAMPDevice *A2MP_FindAMPDeviceWhenPhysicalLinkDone(U8 controllerId)
{
	A2MP_MAIN_CONN *main_channel;
    I8 i;
    
	for(i=0;i<NUM_BT_DEVICES; i++)
	{
	    Report(("A2MP_FindAMPDevice index:%d",i));
		if((A2MPC(a2mp_main_channel)[i].state == A2MP_STATE_ALLOCATED) &&
			(A2MPC(a2mp_main_channel)[i].localControllerId == controllerId) &&
			(A2MPC(a2mp_main_channel)[i].waitAmpPhysicalLinkResponse == 1))
		{
		    Report(("A2MPC(a2mp_main_channel)[i].remDev:%08x", A2MPC(a2mp_main_channel)[i].remDev));
            A2MPC(a2mp_main_channel)[i].waitAmpPhysicalLinkResponse = 1;
			return &A2MPC(a2mp_main_channel)[i].remDev->remoteWifiAMP;
		}
	}
    return 0;
}


#endif /* A2MP_PROTOCOL == XA_ENABLED */

