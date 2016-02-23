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

#ifdef __BT_MCAP__

#include <sys/mcapi.h>
#include <sys/hci.h>
#include <btalloc.h>
//#include "bt_adp_hdp.h"

BtStatus McapMclStartDisTimer(McapMcl* mcl);

/*---------------------------------------------------------------------------
 *            McapNewMcl()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize a MCAP MCL and set the L2cap Channel Id.
 *
 * Return:    McapMcl*
 */
McapMcl* McapNewMcl(BtRemoteDevice *remDev, McapMclRole role, L2capChannelId ChannelId)

{
	McapMcl*	mcl = NULL;
	U8			i;
	
    OS_Report("[MCAP]McapNewMcl");

	/*Try to find the exist MCL of this device*/
	mcl = (McapMcl*)GetHeadList(&MCAP(mcapMclList));
	while((ListEntry*)mcl != &MCAP(mcapMclList))
	{	
		if(McapDeviceAddrEqual(mcl->remDev->bdAddr, remDev->bdAddr))
		{			
			if(McapCheckMclState(mcl,MCAP_MCL_STATE_ACTIVE) || 
				McapCheckMclState(mcl,MCAP_MCL_STATE_CONNECTED) ||
				McapCheckMclState(mcl,MCAP_MCL_STATE_DISCONNECTING) ||
				McapCheckMclState(mcl,MCAP_MCL_STATE_PENDING))
			{
				OS_Report("[MCAP]MCL is not in idle state");
				if(mcl->DelTimerStart)
					McapMclStopDisTimer(mcl);
				return mcl;
			}
			mcl->mclState = MCAP_MCL_STATE_IDLE;
			mcl->l2ChannelId = ChannelId;
			mcl->mclRole = role;
			mcl->currentOp = 0xff;
			mcl->delAllMdlFlag = FALSE;
			mcl->delOneMdlFlag = FALSE;
			return mcl;
		}
        mcl = (McapMcl *)GetNextNode(&mcl->node);
	}
	
	/*Create a new MCL*/
	if(MCAP(mclNum) < NUM_MCAP_MCLS)
	{
		mcl = (McapMcl*)mcap_malloc(sizeof(McapMcl));
		if(mcl != NULL)
		{		
			OS_MemSet((U8 *)mcl, 0, sizeof(McapMcl));
			mcl->remDev = remDev;
			mcl->mclRole = role;
			mcl->l2ChannelId = ChannelId;
			mcl->currentOp = 0xff;
			mcl->delAllMdlFlag = FALSE;
			mcl->delOneMdlFlag = FALSE;
			mcl->nextMdlId = MCAP_MDLID_INITIAL;
			mcl->mclState = MCAP_MCL_STATE_IDLE;
			InitializeListHead(&mcl->mcapMdlList);
			InitializeListHead(&mcl->cmdQueue);
			InsertTailList(&MCAP(mcapMclList), &(mcl->node));
			
			for(i = 0; i < MCAP_NUM_TIMERS; i++)
			{
				mcl->timer[i].context = NULL;
			}
			mcl->delTimer.context = NULL;
			mcl->DelTimerStart = FALSE;
			
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED	
			InitializeListHead(&(mcl->rxPacketsBufferList));
			InitializeListHead(&(mcl->receivedPacketsList));
			OS_MemSet((U8 *)&(mcl->l2capRawData), 0, sizeof(mcl->l2capRawData));
			for (i = 0; i < MCAP_MAX_RETRANS_NUM ; i++)
			{
				InitializeListEntry(&(mcl->rfcRxPackets[i].node));
				mcl->rfcRxPackets[i].data = (U8 *)(mcl->l2capRawData[i]);
				InsertTailList(&(mcl->rxPacketsBufferList), &(mcl->rfcRxPackets[i].node));
			}		 
#endif
		}
		MCAP(mclNum)++;
		return mcl;
	}
	else
	{
		OS_Report("[MCAP]No resource for more MCL");
		return NULL;
	}
}

/*---------------------------------------------------------------------------
 *            McapFreeMcl()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Free a MCAP MCL.
 *
 * Return:    void
 */
void McapFreeMcl(McapMcl *mcl)
{
    OS_Report("[MCAP]McapFreeMcl");

    if(McapIsMclEmpty(mcl))
    {
    	if(mcl->DelTimerStart)
			McapMclStopDisTimer(mcl);
		McapMclStopAllTimer(mcl);
	    RemoveEntryList(&(mcl->node));
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED	
	    RemoveEntryList(&(mcl->rxPacketsBufferList));
	    RemoveEntryList(&(mcl->receivedPacketsList));
#endif
	    mcap_free(mcl);
	    MCAP(mclNum)--;
    }
    else
    {
        mcl->l2ChannelId = L2CID_INVALID;/*ALPS00437872*/
	    OS_Report("[MCAP]McapFreeMcl---MCL is not empty");
    }
}


/*---------------------------------------------------------------------------
 *            McapNewMdl()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize a MCAP MDL and set the L2cap Channel Id.
 *
 * Return:    McapMdl*
 */
McapMdl* McapNewMdl(McapMcl *mcl, McapMdlId mdlId, U8 mdepId)

{
	McapMdl*	mdl = NULL;
	U8			i;
	
    OS_Report("[MCAP]McapNewMdl");
	mdl = (McapMdl*)mcap_malloc(sizeof(McapMdl));
	if(mdl != NULL)
	{		
        OS_MemSet((U8 *)mdl, 0, sizeof(McapMdl));
		mdl->l2ChannelId = L2CID_INVALID;
		mdl->mdepId = mdepId;
		mdl->mdlState = MCAP_MDL_STATE_CLOSED;
		if(mdlId == MCAP_MDLID_RESERVED)
			mdl->mdlId = McapGenerateMdlId(mcl);
		else
			mdl->mdlId = mdlId;
		
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED	
		InitializeListHead( &(mdl->rxPacketsBufferList) );
		InitializeListHead( &(mdl->receivedPacketsList) );
		OS_MemSet((U8 *)&(mdl->l2capRawData), 0, sizeof(mdl->l2capRawData));
		for (i = 0; i < MCAP_MAX_RETRANS_NUM ; i++)
		{
			InitializeListEntry(&(mdl->rfcRxPackets[i].node));
			mdl->rfcRxPackets[i].data = (U8 *)(mdl->l2capRawData[i]);
			InsertTailList(&(mdl->rxPacketsBufferList), &(mdl->rfcRxPackets[i].node));
		}		 
#endif

		InsertTailList(&(mcl->mcapMdlList), &(mdl->node));
	}

	return mdl;
}

/*---------------------------------------------------------------------------
 *            McapFreeMdl()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Free a MCAP MDL.
 *
 * Return:    void
 */
void McapFreeMdl(McapMcl *mcl, McapMdl *mdl)
{
    OS_Report("[MCAP]McapFreeMdl");
    if(mcl == NULL || mdl == NULL)
    {	
    	OS_Report("[MCAP]Null MDL or Null MDL");
	return;
    }
	
    RemoveEntryList(&(mdl->node));
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED	
	RemoveEntryList(&(mdl->rxPacketsBufferList));
	RemoveEntryList(&(mdl->receivedPacketsList));
#endif
    mcap_free(mdl);

	if(McapIsMclEmpty(mcl) && !McapCheckMclState(mcl, MCAP_MCL_STATE_IDLE))
	{
		McapMclStateTrans(mcl,MCAP_MCL_STATE_CONNECTED);
		if(mcl->mclRole == MCAP_MCL_ROLE_INITIATOR)
		{
			//OS_Report("[MCAP:]Disconnect L2CAP of MCL");
        		//L2CAP_DisconnectReq(mcl->l2ChannelId);
        		McapMclStartDisTimer(mcl);
		}
	}
}

BOOL McapCheckMclState(McapMcl *mcl, McapMclState mclState)
{
    OS_Report("[MCAP]McapCheckMclState %d VS %d", mcl->mclState, mclState);
	if(mcl->mclState == mclState)
		return TRUE;
	else
		return FALSE;
}
void McapMclStateTrans(McapMcl *mcl, McapMclState mclState)
{
	OS_Report("[MCAP]McapMclStateTrans: %d -> %d", mcl->mclState, mclState);
	mcl->mclState = mclState;
}

BOOL McapCheckMdlState(McapMdl *mdl, McapMdlState mdlState)
{
	if(mdl == NULL)
		return FALSE;
	
	OS_Report("[MCAP]McapCheckMdlState %d VS %d", mdl->mdlState, mdlState);
	
	if(mdl->mdlState == mdlState)
		return TRUE;
	else
		return FALSE;
}

void McapMdlStateTrans(McapMdl *mdl, McapMdlState mdlState)
{
	OS_Report("[MCAP]McapMdlStateTrans: %d -> %d", mdl->mdlState, mdlState);
	
	mdl->mdlState = mdlState;
}


McapMdlId McapGenerateMdlId(McapMcl *mcl)
{
	McapMdl*	mdl;
	McapMdlId	mdlId;

    OS_Report("[MCAP]McapGenerateMdlId");
	mdlId = mcl->nextMdlId;
	mdl = (McapMdl*)GetHeadList(&mcl->mcapMdlList);
	do
	{
		mdl = McapGetMdlByMdlId(mcl, mdlId);
		if(!mdl)
		{
			mcl->nextMdlId = (mdlId % MCAP_MDLID_FINAL) + 1;
			OS_Report("[MCAP]Mcap Generate MdlId :%d",mdlId);
			return mdlId;
		}
		else
			mdlId = (mdlId % MCAP_MDLID_FINAL) + 1;
	}while(mdlId != mcl->nextMdlId);

	/*No MDL ID available*/
	return MCAP_MDLID_RESERVED;
}

BOOL McapCheckRequest(U8 opCode)
{
	if(opCode == MCAP_MD_CREATE_MDL_REQ ||
		opCode == MCAP_MD_RECONNECT_MDL_REQ ||
		opCode == MCAP_MD_ABORT_MDL_REQ ||
		opCode == MCAP_MD_DELETE_MDL_REQ)
	{	
		return TRUE;
	}
	else		
		return FALSE;
}
BtStatus McapSendControl(McapMcl *mcl, McapControlPacket *controlPkt)
{
    BtStatus      	status;
	McapMdlId		mdlId;
	McapMdlId		tmpMdlId;
	BtPacket*		btPacket;
    OS_Report("[MCAP]McapSendControl %d, response code: %d, mdlId: %d", controlPkt->opCode, controlPkt->responseCode, controlPkt->mdlId);

	if(mcl->DelTimerStart)
		McapMclStopDisTimer(mcl);
/*
	if(mcl->currentOp != 0xff)
	{
		if(McapCheckRequest(controlPkt->opCode))
		{
			OS_Report("[MCAP]Waiting respond for request %d", mcl->currentOp);
			return BT_STATUS_FAILED;
		}
	}
*/
	if(MCAP(btPacketInUse) == TRUE)
	{
		OS_Report("[MCAP]Current MCL is busy, queue this command");
		
		InsertTailList(&(mcl->cmdQueue), &(controlPkt->node));
		return BT_STATUS_PENDING;
	}
	
	mcl->currentCmd = controlPkt->opCode;
	
	if(McapCheckRequest(controlPkt->opCode))
	{	
		mcl->currentOp = controlPkt->opCode;
		status = McapMclStartTimer(mcl, controlPkt);
		if(status != BT_STATUS_SUCCESS)
			return BT_STATUS_BUSY;
	}
	
	MCAP(btPacketInUse) = TRUE;
	tmpMdlId = controlPkt->mdlId;
	StoreBE16((U8*)&mdlId, tmpMdlId);
		
	btPacket = &MCAP(packet);
	btPacket->headerLen = 0;
	switch(controlPkt->opCode)
	{
		case MCAP_MD_CREATE_MDL_REQ:
			btPacket->dataLen = 5;
			btPacket->data = (U8*)mcap_malloc(btPacket->dataLen);
			btPacket->data[0] = controlPkt->opCode;
			OS_MemCopy(btPacket->data+1, (U8*)&mdlId, 2);
			btPacket->data[3] = controlPkt->mdepId;
			btPacket->data[4] = controlPkt->configuration;
			break;
		case MCAP_MD_RECONNECT_MDL_REQ:
		case MCAP_MD_ABORT_MDL_REQ:
		case MCAP_MD_DELETE_MDL_REQ:
			btPacket->dataLen = 3;
			btPacket->data = (U8*)mcap_malloc(btPacket->dataLen);
			btPacket->data[0] = controlPkt->opCode;
			OS_MemCopy(btPacket->data+1, (U8*)&mdlId, 2);
			break;
		case MCAP_MD_CREATE_MDL_RSP:
			/*Unsuccessed response code has a parameter of zero length*/
			if(controlPkt->responseCode == MCAP_SUCCESS)
				btPacket->dataLen = 5;
			else
				btPacket->dataLen = 4;
				
			btPacket->data = (U8*)mcap_malloc(btPacket->dataLen);
			btPacket->data[0] = controlPkt->opCode;
			btPacket->data[1] = controlPkt->responseCode;
			OS_MemCopy(btPacket->data+2, (U8*)&mdlId, 2);
			if(controlPkt->responseCode == MCAP_SUCCESS)
				btPacket->data[4] = controlPkt->configuration;
			break;
		case MCAP_MD_RECONNECT_MDL_RSP:
		case MCAP_MD_ABORT_MDL_RSP:
		case MCAP_MD_DELETE_MDL_RSP:
		case MCAP_ERROR_RSP:
			btPacket->dataLen = 4;
			btPacket->data = (U8*)mcap_malloc(btPacket->dataLen);
			btPacket->data[0] = controlPkt->opCode;
			btPacket->data[1] = controlPkt->responseCode;
			OS_MemCopy(btPacket->data+2, (U8*)&mdlId, 2);
			break;
		case MCAP_MD_SYNC_CAP_REQ:
		case MCAP_MD_SYNC_CAP_RSP:
		case MCAP_MD_SYNC_SET_REQ:
		case MCAP_MD_SYNC_SET_RSP:
		case MCAP_MD_SYNC_INF_IND:
		case 0x16:
			btPacket->dataLen = 3;
			btPacket->data = (U8*)mcap_malloc(btPacket->dataLen);
			btPacket->data[0] = controlPkt->opCode;
			btPacket->data[1] = controlPkt->responseCode;
			break;
		default:
			btPacket->dataLen = 3;
			btPacket->data = (U8*)mcap_malloc(btPacket->dataLen);
			btPacket->data[0] = controlPkt->opCode;
			OS_MemCopy(btPacket->data+1, (U8*)&mdlId, 2);
			break;
	}
	
    btPacket->ulpContext = controlPkt;
    btPacket->flags = BTP_FLAG_INUSE;
    btPacket->priority_type = BTP_FLAG_HIGH_PRIPORTY;
	
    status = L2CAP_SendData(mcl->l2ChannelId, btPacket);
    OS_Report("[MCAP:]Control packet send status:%d",status);
	if(status != BT_STATUS_PENDING)
	{
		if(McapCheckRequest(controlPkt->opCode))
		{
			McapMclStopTimer(mcl, tmpMdlId);
		}
		else
		{
			mcap_free(controlPkt);
		}
		MCAP(btPacketInUse) = FALSE;		
		mcap_free(btPacket->data);
	}
    return status;
}

BtStatus McapConfigMdl(McapMcl *mcl, McapMdl *mdl, U8 configuration)
{
	McapControlPacket*	controlPkt;
	BtStatus			status;
    OS_Report("[MCAP]McapConfigMdl");
	
	controlPkt = (McapControlPacket*)mcap_malloc(sizeof(McapControlPacket));
	OS_MemSet((U8 *)controlPkt, 0, sizeof(McapControlPacket));

	controlPkt->opCode = MCAP_MD_CREATE_MDL_REQ;
	controlPkt->mdlId = mdl->mdlId;
	controlPkt->mdepId = mdl->mdepId;
	controlPkt->configuration = configuration;
	status = McapSendControl(mcl, controlPkt);
	return status;
}

BtStatus McapSendData(McapMdl *mdl, BtPacket *btPacket, McapDataPacket *txPkt)
{
    BtStatus      	status;
    OS_Report("[MCAP]McapSendData in %d", mdl->mdlId);

	if(L2CAP_GetTxMtu(mdl->l2ChannelId)-BT_PACKET_HEADER_LEN < txPkt->dataLen)
		return BT_STATUS_INVALID_PARM;
	
	OS_MemSet((kal_uint8 *)btPacket, 0, sizeof(BtPacket));

	btPacket->headerLen = 0;
	btPacket->dataLen = txPkt->dataLen;
	btPacket->data = txPkt->data;
	
    btPacket->ulpContext = txPkt;
    btPacket->flags = BTP_FLAG_INUSE;
    btPacket->priority_type = BTP_FLAG_HIGH_PRIPORTY;
	
    status = L2CAP_SendData(mdl->l2ChannelId, btPacket);
    OS_Report("[MCAP:]Data packet send %d",status);
    return status;
}
McapMcl* McapFindMclByCid(L2capChannelId ChannelId)
{
	McapMcl*	mcl;
	McapMdl*	mdl;
	
	OS_Report("[MCAP]McapFindMdlByCid: want to find ChannelId = %d", ChannelId);
	mcl = (McapMcl*)GetHeadList(&MCAP(mcapMclList));
	
	while((ListEntry*)mcl != &MCAP(mcapMclList))
	{
		if(mcl->l2ChannelId == ChannelId)
			return mcl;
		mdl = (McapMdl*)GetHeadList(&mcl->mcapMdlList);
		while((ListEntry*)mdl != &mcl->mcapMdlList)
		{
			if(mdl->l2ChannelId == ChannelId)
			{
				OS_Report("[MCAP]McapFindMclByCid: find mcl");
				return mcl;
			}
			mdl = (McapMdl *)GetNextNode(&mdl->node);
		}
        mcl = (McapMcl *)GetNextNode(&mcl->node);
	}
	
	OS_Report("[MCAP]McapFindMclByCid: find MCL NULL");
	return NULL;
}

McapMdl* McapFindMdlByCid(McapMcl *mcl, L2capChannelId ChannelId)
{
	McapMdl*	mdl;
	
	OS_Report("[MCAP]McapFindMdlByCid: want to find ChannelId = %d", ChannelId);
	if(mcl == NULL)
	{
		OS_Report("[MCAP]McapFindMdlByCid MCL==NULL");
		return NULL;
	}
	
	mdl = (McapMdl*)GetHeadList(&mcl->mcapMdlList);
	while((ListEntry*)mdl != &mcl->mcapMdlList)
	{
		if(mdl->l2ChannelId == ChannelId)
		{
			OS_Report("[MCAP]find mdlId = %d", mdl->mdlId);
			return mdl;
		}
		mdl = (McapMdl *)GetNextNode(&mdl->node);
	}
	
	OS_Report("[MCAP]McapFindMdlByCid: find mdl NULL");
	return NULL;
}

McapMcl* McapFindMclByBTAddr(BD_ADDR addr)
{
	McapMcl*	mcl;
	
	mcl = (McapMcl*)GetHeadList(&MCAP(mcapMclList));
	while((ListEntry*)mcl != &MCAP(mcapMclList))
	{
		if(McapDeviceAddrEqual(mcl->remDev->bdAddr, addr))
		{
			OS_Report("[MCAP]McapFindMclByBTAddr: find MCL");
			return mcl;
		}
        mcl = (McapMcl *)GetNextNode(&mcl->node);
	}
	
    OS_Report("[MCAP]McapFindMclByBTAddr: find MCL NULL");
	return NULL;
}

McapMcl* McapFindMclByTimer(EvmTimer *timer)
{
	McapMcl*	mcl;
	U8			i;
    OS_Report("[MCAP]McapFindMclByTimer");
	
	mcl = (McapMcl*)GetHeadList(&MCAP(mcapMclList));
	while((ListEntry*)mcl != &MCAP(mcapMclList))
	{
		for(i = 0; i < MCAP_NUM_TIMERS; i++)
		{
			if(&(mcl->timer[i]) == timer)
				return mcl;;
		}
        mcl = (McapMcl *)GetNextNode(&mcl->node);
	}
	
	return NULL;
}


McapMdl* McapGetMdlByMdlId(McapMcl *mcl, McapMdlId mdlId)
{
	McapMdl*	mdl;
	
	OS_Report("[MCAP]McapGetMdlByMdlId: want to find mdlId = %d", mdlId);
	mdl = (McapMdl*)GetHeadList(&mcl->mcapMdlList);	
	while((ListEntry*)mdl != &mcl->mcapMdlList)
	{
		if(mdl->mdlId == mdlId)
		{
			OS_Report("[MCAP]find mdlId = %d", mdl->mdlId);
			return mdl;
		}
		mdl = (McapMdl *)GetNextNode(&mdl->node);
	}
	
	OS_Report("[MCAP]McapGetMdlByMdlId: find mdl NULL");
	return NULL;
}

BtStatus McapCheckMdepBusy(McapMcl *mcl, U8 mdepId)
{
	McapMdl*			mdl;
	U8					busyNum;
	
      OS_Report("[MCAP]McapCheckMdep");
	busyNum = 0;
	mdl = (McapMdl*)GetHeadList(&mcl->mcapMdlList);
	while((ListEntry*)mdl != &mcl->mcapMdlList)
	{
		if(mdl->mdepId == mdepId)
			busyNum++;
		mdl = (McapMdl *)GetNextNode(&mdl->node);
	}

	if(busyNum >= MCAP_MAX_CHANNEL_NUM)
		return BT_STATUS_SUCCESS;
	else
		return BT_STATUS_FAILED;
}

BtStatus McapMclCheckParms(McapMcl *mcl, McapDataPacket* dataPkt)
{
	McapOpCode			opCode;
	McapMdlId			mdlId;
	McapMdlId			tmpMdlId;
	McapMdl*			mdl;
	McapControlPacket*	controlPkt;
	U8					mdepId;
    OS_Report("[MCAP]McapMclCheckParms");

	opCode = dataPkt->data[0];
	
	controlPkt = (McapControlPacket*)mcap_malloc(sizeof(McapControlPacket));
	OS_MemSet((U8 *)controlPkt, 0, sizeof(McapControlPacket));
	controlPkt->opCode = opCode;
	
	OS_MemCopy((U8*)&tmpMdlId, dataPkt->data+1, 2);
	mdlId = BEtoHost16((U8*)&tmpMdlId);
	controlPkt->mdlId = mdlId;
	
	if(opCode == MCAP_MD_SYNC_CAP_REQ || opCode == MCAP_MD_SYNC_SET_REQ || opCode == MCAP_MD_SYNC_INF_IND)
	{
		controlPkt->opCode = opCode+1;
		controlPkt->responseCode = MCAP_REQUEST_NOT_SUPPORTED;
        	goto fail;
	}
	
	if((opCode > MCAP_MD_DELETE_MDL_RSP && opCode < MCAP_MD_SYNC_CAP_REQ)
		|| opCode > MCAP_MD_SYNC_RESERVED)
	{
		controlPkt->opCode = MCAP_ERROR_RSP;
		controlPkt->responseCode = MCAP_INVALID_OP_CODE;
		controlPkt->mdlId = MCAP_MDLID_RESERVED;
        	goto fail;
	}
	
	/*Check the parameter of the request*/
	if(McapCheckRequest(opCode))
	{	
		controlPkt->opCode = opCode+1;
		
		if(opCode == MCAP_MD_CREATE_MDL_REQ)
		{
			if(dataPkt->dataLen != 5)
			{
				controlPkt->responseCode = MCAP_INVALID_PARAM_VALUE;
				goto fail;
			}
			OS_MemCopy((U8*)&mdepId, dataPkt->data+3, 1);			
			OS_Report("[MCAP]McapMclCheckParms, mdepId = %d" + mdepId);
			if(mdepId > 0x80)
			{
				controlPkt->responseCode = MCAP_INVALID_MDEP;
				goto fail;
			}

			if(McapCheckMdepBusy(mcl, mdepId) == BT_STATUS_SUCCESS)
			{
				controlPkt->responseCode = MCAP_MDEP_BUSY;
				goto fail;
			}
		}
		else 
		{
			if(dataPkt->dataLen != 3)
			{
				controlPkt->responseCode = MCAP_INVALID_PARAM_VALUE;
				goto fail;
			}
		}
		
		mdl = McapGetMdlByMdlId(mcl, mdlId);	
		
		if(mdlId == MCAP_MDLID_RESERVED ||
			(mdlId > MCAP_MDLID_FINAL && mdlId < MCAP_ALL_MDLIDS))
		{
			controlPkt->responseCode = MCAP_INVALID_MDL;
			goto fail;
		}

		if(opCode == MCAP_MD_RECONNECT_MDL_REQ ||
			opCode == MCAP_MD_ABORT_MDL_REQ ||
			(opCode == MCAP_MD_DELETE_MDL_REQ && mdlId != MCAP_ALL_MDLIDS))
		{
			if(mdl == NULL)
			{
				controlPkt->responseCode = MCAP_INVALID_MDL;
				goto fail;
			}
		}
		/*
		else if(opCode == MCAP_MD_CREATE_MDL_REQ)
		{
			if(mdl != NULL)
			{
				if(McapCheckMdlState(mdl, MCAP_MDL_STATE_OPEN))
				{
					controlPkt->responseCode = MCAP_MDL_BUSY;
					goto fail;
				}
			}
		}*/
	}	
	return BT_STATUS_SUCCESS;
	
fail:
	McapSendControl(mcl, controlPkt);
	return BT_STATUS_FAILED;
}

void McapHandleMclDataInd(McapMcl *mcl, McapDataPacket* dataPkt, McapCallbackParms *parms)
{
	McapOpCode			opCode;
	McapMdlId			mdlId = 0;
	McapMdlId			tmpMdlId;
	McapMdl*			mdl;
	McapResponseCode	responseCode = MCAP_INVALID_OP_CODE;
	McapControlPacket*	controlPkt;
	
	if(mcl->DelTimerStart)
		McapMclStopDisTimer(mcl);

	opCode = dataPkt->data[0];	
    OS_Report("[MCAP]McapHandleMclDataInd, opCode=%d",opCode);

	if(McapMclCheckParms(mcl, dataPkt) == BT_STATUS_FAILED)
		return;	
	
	if(McapCheckRequest(opCode))
	{	
		OS_MemCopy((U8*)&tmpMdlId, dataPkt->data+1, 2);
		mdlId = BEtoHost16((U8*)&tmpMdlId);
		OS_Report("[MCAP]mdlId=%d",mdlId);
	}
	else if(opCode == MCAP_MD_CREATE_MDL_RSP ||
		opCode == MCAP_MD_RECONNECT_MDL_RSP ||
		opCode == MCAP_MD_ABORT_MDL_RSP ||
		opCode == MCAP_MD_DELETE_MDL_RSP)
	{
		//mcl->currentOp maybe changed when receiveing L2CAP_PACKET_HANDLED and sendcontrolop to l2cap
	/*
		if(mcl->currentOp != (opCode - 1))
		{
			OS_Report("[MCAP]Unexpected response code: current op %d", mcl->currentOp);
			return;
		}
		mcl->currentOp = 0xff;
		*/
		
		OS_MemCopy((U8*)&tmpMdlId, dataPkt->data+2, 2);
		mdlId = BEtoHost16((U8*)&tmpMdlId);
		responseCode = dataPkt->data[1];
		OS_Report("[MCAP]responseCode=%d, mdlId=%d",responseCode, mdlId);
	}
	mdl = McapGetMdlByMdlId(mcl, mdlId);
	
	controlPkt = (McapControlPacket*)mcap_malloc(sizeof(McapControlPacket));
	switch(opCode)
	{
	case MCAP_MD_CREATE_MDL_REQ:
		if(mdl == NULL)
			mdl = McapNewMdl(mcl, mdlId, dataPkt->data[3]);
		else
			mdl->mdepId = dataPkt->data[3];
		
		parms->mdl = mdl;
		parms->l2config = dataPkt->data[4];
		McapAppCallBack(parms, BT_STATUS_SUCCESS, MCAPEVENT_MDLCONFIG_IND);
		break;
	case MCAP_MD_RECONNECT_MDL_REQ:
		if(mdl != NULL)
		{
			if(McapCheckMdlState(mdl, MCAP_MDL_STATE_CLOSED))
			{
				McapMclStateTrans(mcl,MCAP_MCL_STATE_PENDING);
				mcl->pendingMdl = mdl;
				McapMdlStateTrans(mdl,MCAP_MDL_STATE_CONN_PENDING);
				controlPkt->responseCode = MCAP_SUCCESS;
			}
			else
				controlPkt->responseCode = MCAP_INVALID_OPERATION;
		}
		else
			controlPkt->responseCode = MCAP_INVALID_MDL;
			
		controlPkt->opCode = MCAP_MD_RECONNECT_MDL_RSP;
		controlPkt->mdlId = mdlId;
		McapSendControl(mcl, controlPkt);
		break;
	case MCAP_MD_ABORT_MDL_REQ:
		/*
		if(mdl != NULL)
		{
			controlPkt->responseCode = MCAP_SUCCESS;
			parms->mdl = mdl;
			McapAppCallBack(parms, BT_STATUS_SUCCESS, MCAPEVENT_ABORT_IND);
			McapFreeMdl(mcl, mdl);
		}
		else
		*/
		controlPkt->responseCode = MCAP_INVALID_OPERATION;
		controlPkt->opCode = MCAP_MD_ABORT_MDL_RSP;
		controlPkt->mdlId = mdlId;
		McapSendControl(mcl, controlPkt);
		break;
	case MCAP_MD_DELETE_MDL_REQ:
		if(McapCheckMclState(mcl, MCAP_MCL_STATE_ACTIVE) || McapCheckMclState(mcl, MCAP_MCL_STATE_CONNECTED))
		{
			if(mdlId == MCAP_ALL_MDLIDS)
			{	
				BOOL	HaveDelAction = FALSE;
				/*Try to disconnect all connected MDL*/
				mcl->delAllMdlFlag = TRUE;
				mdl = (McapMdl*)GetHeadList(&mcl->mcapMdlList); 
				while((ListEntry*)mdl != &mcl->mcapMdlList)
				{
					if(McapCheckMdlState(mdl,MCAP_MDL_STATE_OPEN))
					{	
						HaveDelAction = TRUE;
						mdl->delFlag = TRUE;
						McapMdlStateTrans(mdl,MCAP_MDL_STATE_DISC_PENDING);
						L2CAP_DisconnectReq(mdl->l2ChannelId);
					}
					mdl = (McapMdl *)GetNextNode(&mdl->node);
				}
				if(HaveDelAction != TRUE)
				{
					controlPkt->mdlId = mdlId;
					controlPkt->opCode = MCAP_MD_DELETE_MDL_RSP;
					controlPkt->responseCode = MCAP_SUCCESS;
					McapSendControl(mcl, controlPkt);	
				}
			}
			else
			{
				mcl->delOneMdlFlag = TRUE;
				if(McapCheckMdlState(mdl,MCAP_MDL_STATE_OPEN))
				{
					mdl->delFlag = TRUE;
					McapMdlStateTrans(mdl,MCAP_MDL_STATE_DISC_PENDING);
					L2CAP_DisconnectReq(mdl->l2ChannelId);
				}
				else
				{
					controlPkt->responseCode = MCAP_SUCCESS;			
					controlPkt->opCode = MCAP_MD_DELETE_MDL_RSP;
					controlPkt->mdlId = mdlId;
					McapSendControl(mcl, controlPkt);
				}
			}
		}
		break;
	case MCAP_MD_CREATE_MDL_RSP:
		McapMclStopTimer(mcl, mdlId);
		parms->mdl = mdl;
		parms->l2config = dataPkt->data[4];
		if(responseCode != MCAP_SUCCESS)
		{
			McapAppCallBack(parms, BT_STATUS_FAILED, MCAPEVENT_MDLCONFIG_CNF);
			McapFreeMdl(mcl, mdl);
		}
		else
		{
			McapAppCallBack(parms, BT_STATUS_SUCCESS, MCAPEVENT_MDLCONFIG_CNF);
		}
		break;
	case MCAP_MD_RECONNECT_MDL_RSP:
		McapMclStopTimer(mcl, mdlId);
		if(responseCode != MCAP_SUCCESS)
		{
			parms->mdl = mdl;
			McapAppCallBack(parms, BT_STATUS_FAILED, MCAPEVENT_MDLCONFIG_CNF);
			McapFreeMdl(mcl, mdl);
		}
		else
		{
			BtStatus	status;
#ifdef __BT_2_1_ERTM__
			MCAP(dataPsm).inLinkMode = mdl->l2config.inLinkMode;
#endif
			status = L2CAP_ConnectReq(&(MCAP(dataPsm)), mdl->mdlPsm, 
							mcl->remDev, 0, &mdl->l2ChannelId); 
			OS_Report("[MCAP:]Establishing L2CAP connection for MDL: CID = %d", mdl->l2ChannelId);
			if (status != BT_STATUS_PENDING) {
				OS_Report("[MCAP]L2CAP connect request failed");
				McapFreeMdl(mcl, mdl);
				return;
			}
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
			if( BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status )
			{
				L2capConnSettings Setting;
		
				Setting.reasmBuffer = (ListEntry *)&(mdl->rxPacketsBufferList);
				Setting.receivedBuffer = (ListEntry *)&(mdl->receivedPacketsList);
				Setting.rxWindowSize = MCAP_MAX_RETRANS_NUM;  
				 OS_MemSet((U8 *)(mdl->txMaxCountPerIFrame), 0, 128); /* Always 128 */
				 OS_MemSet((U8 *)(mdl->txSrejList), 0, sizeof(mdl->txSrejList) ); 
				Setting.txMaxTransmitCount = (U8 *)(mdl->txMaxCountPerIFrame);
				Setting.txSrejList = (U8 *)(mdl->txSrejList);		  
				Setting.maxSrejNo = RF_L2CAP_MAX_SREJ_SIZE;
		
				L2CAP_ConnectReqAssignFlowControlBuffer(&Setting, mdl->l2ChannelId);	
			}
#endif
			McapMdlStateTrans(mdl,MCAP_MDL_STATE_CONN_PENDING);
		}
		break;
	case MCAP_MD_ABORT_MDL_RSP:
		McapMclStopTimer(mcl, mdlId);
		if(McapCheckMclState(mcl, MCAP_MCL_STATE_ACTIVE) || McapCheckMclState(mcl, MCAP_MCL_STATE_CONNECTED))
		{
			parms->mdl = mdl;
			if(responseCode == MCAP_SUCCESS)
			{
				McapAppCallBack(parms, BT_STATUS_SUCCESS, MCAPEVENT_ABORTED);
				McapFreeMdl(mcl, mdl);
			}
			else
				McapAppCallBack(parms, BT_STATUS_FAILED, MCAPEVENT_ABORTED);
		}
		break;
	case MCAP_MD_DELETE_MDL_RSP:
		McapMclStopTimer(mcl, mdlId);
		if(McapCheckMclState(mcl, MCAP_MCL_STATE_ACTIVE) || McapCheckMclState(mcl, MCAP_MCL_STATE_CONNECTED))
		{
			if(responseCode != MCAP_SUCCESS)
			{
				parms->mdl = mdl;
				McapAppCallBack(parms, BT_STATUS_FAILED, MCAPEVENT_MDLCLOSED);
			}
			else
			{	
				if(mdlId != MCAP_ALL_MDLIDS)
				{
					if(McapCheckMdlState(mdl,MCAP_MDL_STATE_DISC_PENDING))
					{
						OS_Report("[MCAP:]Disconnect L2CAP of mdl: CID = %d",mdl->l2ChannelId);
						L2CAP_DisconnectReq(mdl->l2ChannelId);		
					}
					else
						McapFreeMdl(mcl, mdl);
				}
				else
				{
					McapMdl*	tmpMdl;
					mdl = (McapMdl*)GetHeadList(&mcl->mcapMdlList); 
					while((ListEntry*)mdl != &mcl->mcapMdlList)
					{
						if(McapCheckMdlState(mdl,MCAP_MDL_STATE_DISC_PENDING))
						{
							OS_Report("[MCAP:]Disconnect L2CAP of mdl: CID = %d",mdl->l2ChannelId);
							L2CAP_DisconnectReq(mdl->l2ChannelId);
						}
						else
						{
							tmpMdl = (McapMdl *)GetNextNode(&mdl->node);
							McapFreeMdl(mcl, mdl);
							mdl = tmpMdl;
							continue;
						}
						mdl = (McapMdl *)GetNextNode(&mdl->node);
					}
				}
			}
		}
		break;
	default:
		OS_Report("[MCAP]Unsupported command in this state.");
		break;
	}
}

void McapMclTimeoutFun(EvmTimer *timer)
{	
	McapControlPacket*		controlPkt;
	McapCallbackParms		parms;
	McapMcl*				mcl;
	McapMdl*				mdl;
		
	OS_Report("[MCAP]McapMcl timer timeout.");
	controlPkt = (McapControlPacket*)timer->context;
	timer->context = NULL;

	mcl = McapFindMclByTimer(timer);
	if(mcl == NULL)
	{
		OS_Report("[MCAP]No mcl found with this timer.");
		return;
	}
	mcl->currentOp = 0xff;
	mdl = McapGetMdlByMdlId(mcl, controlPkt->mdlId);
	if(mdl == NULL)
	{
		OS_Report("[MCAP]No mdl found.");
		return;
	}
	parms.mcl = mcl;
	parms.mdl = mdl;
	parms.remDev = mcl->remDev;
	
	switch(controlPkt->opCode)
	{
	case MCAP_MD_CREATE_MDL_REQ:
		McapAppCallBack(&parms, BT_STATUS_FAILED, MCAPEVENT_MDLCONFIG_CNF);
		McapFreeMdl(mcl,mdl);
		break;	
	case MCAP_MD_RECONNECT_MDL_REQ:
		McapAppCallBack(&parms, BT_STATUS_FAILED, MCAPEVENT_MDLOPENED_CNF);
		break;	
	case MCAP_MD_ABORT_MDL_REQ:
		McapAppCallBack(&parms, BT_STATUS_FAILED, MCAPEVENT_ABORTED);
		break;
	case MCAP_MD_DELETE_MDL_REQ:
		McapAppCallBack(&parms, BT_STATUS_FAILED, MCAPEVENT_MDLCLOSED);
		break;
	default:
		break;
	}
}

BtStatus McapMclStartTimer(McapMcl *mcl, McapControlPacket *controlPkt)
{
	EvmTimer*	mclTimer;
	U8			i;
	OS_Report("[MCAP]McapMclStartTimer.");

	for(i = 0; i < MCAP_NUM_TIMERS; i++)
	{
		if(mcl->timer[i].context == NULL)
		{
			mclTimer = &(mcl->timer[i]);
			break;
		}
	}
	OS_Report("[MCAP]Timer %d start.", i);

	/*No timer avialable*/
	if(i == MCAP_NUM_TIMERS)
	{
		OS_Report("[MCAP]No timer to start.");
		return BT_STATUS_FAILED;
	}
	
    mclTimer->func = McapMclTimeoutFun;
    mclTimer->context = (void *)controlPkt;
    EVM_StartTimer(mclTimer, MCAP_CONTROL_TIMEOUT);
	return BT_STATUS_SUCCESS;	
}

void McapMclStopTimer(McapMcl *mcl, McapMdlId mdiId)
{
	U8		i;
	McapControlPacket*		controlPkt;
	EvmTimer*	mclTimer;
	OS_Report("[MCAP]McapMclStopTimer.");

	for(i = 0; i < MCAP_NUM_TIMERS; i++)
	{
		controlPkt = (McapControlPacket*)mcl->timer[i].context;
		if(controlPkt != NULL)
		{
			OS_Report("[MCAP]controlPkt->mdlId = %d.", controlPkt->mdlId);
			if(controlPkt->mdlId== mdiId)
			{
				mclTimer = &(mcl->timer[i]);
				break;
			}
		}
	}
	
	OS_Report("[MCAP]Timer %d stop.", i);
	
	if(i == MCAP_NUM_TIMERS)
	{
		OS_Report("[MCAP]No timer found when stop timer.");
		return;
	}
	
    EVM_CancelTimer(mclTimer);
	mclTimer->context = NULL;
	mcap_free(controlPkt);
}

void McapMclStopAllTimer(McapMcl *mcl)
{
	U8		i;
	McapControlPacket*		controlPkt = NULL;
	EvmTimer*				mclTimer = NULL;
	OS_Report("[MCAP]McapMclStopAllTimer.");

	for(i = 0; i < MCAP_NUM_TIMERS; i++)
	{
		controlPkt = (McapControlPacket*)mcl->timer[i].context;
		if(controlPkt != NULL)
		{
			OS_Report("[MCAP]Timer %d stop.", i);
			mclTimer = &(mcl->timer[i]);
		    EVM_CancelTimer(mclTimer);
			mclTimer->context = NULL;
			mcap_free(controlPkt);
		}
	}
}


void McapMclDisTimeoutFun(EvmTimer *timer)
{	
	McapMcl*				mcl;
	
	OS_Report("[MCAP]McapMcl disconnect timer timeout.");
	OS_Report("[MCAP:]Disconnect L2CAP of MCL");
	
	mcl = (McapMcl*)timer->context;
	mcl->DelTimerStart = FALSE;
	timer->context = NULL;
	McapMclStateTrans(mcl,MCAP_MCL_STATE_DISCONNECTING);
	L2CAP_DisconnectReq(mcl->l2ChannelId);
}


BtStatus McapMclStartDisTimer(McapMcl* mcl)
{
	OS_Report("[MCAP]McapMclStartDisTimer.");
	mcl->DelTimerStart = TRUE;
    	mcl->delTimer.func = McapMclDisTimeoutFun;
    	mcl->delTimer.context = (void *)mcl;
    	EVM_StartTimer(&(mcl->delTimer), 10000);
	return BT_STATUS_SUCCESS;	

}

void McapMclStopDisTimer(McapMcl* mcl)
{
	OS_Report("[MCAP]McapMclStopDisTimer.");
    	EVM_CancelTimer(&(mcl->delTimer));
	mcl->DelTimerStart = FALSE;
	mcl->delTimer.context = NULL;
}


BOOL McapDeviceAddrEqual(BD_ADDR src, BD_ADDR des)
{
	if(src.addr[0] == des.addr[0] && \
			src.addr[1] == des.addr[1] && \
			src.addr[2] == des.addr[2] && \
			src.addr[3] == des.addr[3] && \
			src.addr[4] == des.addr[4] && \
			src.addr[5] == des.addr[5])
		return TRUE;
	else
		return FALSE;
}

BOOL McapIsMclEmpty(McapMcl *mcl)
{	
	return IsListEmpty(&(mcl->mcapMdlList));
}


#endif /* __BT_MCAP__ */

