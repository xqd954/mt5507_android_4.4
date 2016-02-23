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
#include <btalloc.h>
#include <sys/debug.h>

int g_mcap_ignore = 0;

/*---------------------------------------------------------------------------
 *            McapAppCallBack()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Call the application with the specified event.
 *
 * Return:    
 */
void McapAppCallBack(McapCallbackParms *parms, U16 status, McapEvent event)
{
    Assert(MCAP(callback) != 0);

    parms->status = status;
    parms->event = event;

    MCAP(callback)(parms);
}

/*---------------------------------------------------------------------------
 *            L2McapMclCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Callback routine for L2CAP connections.
 *
 * Return:    
 */
void L2McapMclCallback(L2capChannelId l2ChannelId, L2capCallbackParms *info)
{
	McapMcl*			mcl;

    OS_Report("[MCAP:]L2McapMclCallback event: %d; Cid: %d",info->event, l2ChannelId);

	/*Find exist MCL by channel ID*/
    mcl = McapFindMclByCid(l2ChannelId);

    switch (info->event) {
    case L2EVENT_CONNECT_IND:
		mcl = McapFindMclByBTAddr(info->aclLink->bdAddr);
		if(mcl != NULL)
		{
			if(McapCheckMclState(mcl, MCAP_MCL_STATE_CONNECTED) || McapCheckMclState(mcl, MCAP_MCL_STATE_ACTIVE))
			{
				OS_Report("[MCAP]MCL exsit, reject this MCL connect request");
				info->aclLink->discFlag = TRUE;
				L2CAP_ConnectRsp(l2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
				break;
			}
		}
		mcl = McapNewMcl(info->aclLink,MCAP_MCL_ROLE_ACCEPTOR,l2ChannelId);
		if(mcl == NULL)
		{
			/*Create MCL fail*/
			info->aclLink->discFlag = TRUE;
			L2CAP_ConnectRsp(l2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
			break;
		}
		else
		{
			OS_Report("[MCAP]Current MCL state: %d",mcl->mclState);
			MCAP(mclState)[mcl->mclState](mcl, l2ChannelId, info);
		}
        break;
	case L2EVENT_CONNECTED:
        if (mcl == NULL)
        {
            L2CAP_DisconnectReq(l2ChannelId);
            break;
        }
		else
		{
			OS_Report("[MCAP]Current MCL state: %d",mcl->mclState);
			MCAP(mclState)[mcl->mclState](mcl, l2ChannelId, info);
			break;
		}
    default:
        if(mcl != NULL)
		{
			OS_Report("[MCAP]Current MCL state: %d",mcl->mclState);
			MCAP(mclState)[mcl->mclState](mcl, l2ChannelId, info);
            break;
		}
		else
		{
			OS_Report("[MCAP]Empty MCL");
			return;
		}
    }
}

/*---------------------------------------------------------------------------
 *            L2McapMdlCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Callback routine for L2CAP connections.
 *
 * Return:    
 */
void L2McapMdlCallback(L2capChannelId l2ChannelId, L2capCallbackParms *info)
{
    McapCallbackParms*  parms;
	McapMcl*			mcl;
	McapMdl*			mdl;

    OS_Report("[MCAP:]L2McapMdlCallback event: %d; Cid: %d",info->event,l2ChannelId);

	/*Find exist MCL by channel ID*/
    mcl = McapFindMclByCid(l2ChannelId);
	mdl = McapFindMdlByCid(mcl, l2ChannelId);

    switch (info->event) {
    case L2EVENT_CONNECT_IND:
		if(mdl == NULL && mcl == NULL)
		{
			/*The wanted MDL can not be found use Cid because the MDL has not a valid Cid.   
			*Try to find the MCL using BT addr and then find the connect pending MDL in this MCL 
			*/
			mcl = McapFindMclByBTAddr(info->aclLink->bdAddr);
			if(mcl == NULL)
			{
				/*Find no MCL*/
				OS_Report("[MCAP]Find no MCL");
				info->aclLink->discFlag = TRUE;
				L2CAP_ConnectRsp(l2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
				break;
			}
			else
			{
				if(McapCheckMclState(mcl,MCAP_MCL_STATE_PENDING) == FALSE)
				{
					/*Wrong MCL state*/
					OS_Report("[MCAP]Wrong MCL state");
					info->aclLink->discFlag = TRUE;
					L2CAP_ConnectRsp(l2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
					break;
				}

				mdl = mcl->pendingMdl;
				if(mdl == NULL)
				{
					/*Find no MDL*/
					OS_Report("[MCAP]Find no MDL");
					info->aclLink->discFlag = TRUE;
					L2CAP_ConnectRsp(l2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
					break;
				}
				else
				{
					OS_Report("[MCAP]Current MDL state: %d",mdl->mdlState);
					MCAP(mdlState)[mdl->mdlState](mcl, mdl, l2ChannelId, info);
				}
			}
		}
		else
		{
			OS_Report("[MCAP]Find wrong MCL or MDL");
			info->aclLink->discFlag = TRUE;
			L2CAP_ConnectRsp(l2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
		}
        break;
	case L2EVENT_CONNECTED:
        if (mcl == NULL || mdl == NULL)
        {
            L2CAP_DisconnectReq(l2ChannelId);
            break;
        }
		else
		{
			OS_Report("[MCAP]Current MDL state: %d",mdl->mdlState);
			MCAP(mdlState)[mdl->mdlState](mcl, mdl, l2ChannelId, info);
		}
		break;
    default:
        if(mcl != NULL && mdl != NULL)
		{
			OS_Report("[MCAP]Current MDL state: %d",mdl->mdlState);
			MCAP(mdlState)[mdl->mdlState](mcl, mdl, l2ChannelId, info);
		}
		else
		{
			OS_Report("[MCAP]Empty MCL or MDL");
			return;
		}
        break;
    }
}

void McapMclStateIdle(McapMcl *mcl, L2capChannelId Cid, L2capCallbackParms *Parms)
{
    McapCallbackParms	appParms;
	OS_Report("[MCAP]Run MCL IDLE machine: L2CAP event = %d", Parms->event);
	appParms.mcl = mcl;	
	appParms.remDev = Parms->aclLink;

	if(Parms->event == L2EVENT_CONNECT_IND)
	{
		appParms.mcl->l2ChannelId = Cid;
		McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_MCLOPEN_IND);
	}
	else if(Parms->event == L2EVENT_CONNECTED)
	{
		McapMclStateTrans(mcl,MCAP_MCL_STATE_CONNECTED);
		McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_MCLOPENED);
	}
	else if(Parms->event == L2EVENT_DISCONNECTED)
	{
		McapAppCallBack(&appParms, BT_STATUS_FAILED, MCAPEVENT_MCLOPENED);
		McapFreeMcl(mcl);
	}
	else
		OS_Report("[MCAP]Run MCL IDLE machine: Unsupported L2CAP event");
}

void McapMclStateCommonMachine(McapMcl *mcl, L2capChannelId Cid, L2capCallbackParms *Parms)
{
    McapCallbackParms	appParms;
	McapMdl*			mdl;
	McapControlPacket*	controlPkt;
	McapDataPacket		dataPkt;
	appParms.mcl = mcl;	
	appParms.remDev = Parms->aclLink;

	switch (Parms->event) 
	{
	case L2EVENT_DISCONNECTED:
		/*Try to disconnect all connected MDL*/
		mdl = (McapMdl*)GetHeadList(&mcl->mcapMdlList); 
		while((ListEntry*)mdl != &mcl->mcapMdlList)
		{
			if(mdl->mdlState == MCAP_MDL_STATE_OPEN)
				L2CAP_DisconnectReq(mdl->l2ChannelId);
			mdl = (McapMdl *)GetNextNode(&mdl->node);
		}
		McapMclStateTrans(mcl,MCAP_MCL_STATE_IDLE);
		McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_MCLCLOSED);

		/*Free the MCL if there is no MDL in it*/
		McapFreeMcl(mcl);
		break;
	case L2EVENT_PACKET_HANDLED:
		controlPkt = (McapControlPacket *)Parms->ptrs.packet->ulpContext;
		MCAP(btPacketInUse) = FALSE;		
		mcap_free(Parms->ptrs.packet->data);
		
		OS_Report("[MCAP]opCode: %d",controlPkt->opCode);
		switch(controlPkt->opCode)
		{
		case MCAP_MD_CREATE_MDL_REQ:
		case MCAP_MD_RECONNECT_MDL_REQ:
			McapMclStateTrans(mcl,MCAP_MCL_STATE_ACTIVE);
			break;
		case MCAP_MD_CREATE_MDL_RSP:
		case MCAP_MD_RECONNECT_MDL_RSP:
			/*
			if(controlPkt->responseCode == MCAP_SUCCESS)
			{
				McapMdl*	mdl;
				McapMclStateTrans(mcl,MCAP_MCL_STATE_PENDING);
				mdl = McapGetMdlByMdlId(mcl,controlPkt->mdlId);
				if(mdl == NULL){					
					OS_Report("[MCAP]Find no MDL");
					break;
				}
				mcl->pendingMdl = mdl;
				McapMdlStateTrans(mdl,MCAP_MDL_STATE_CONN_PENDING);
			}
			*/
		case MCAP_MD_ABORT_MDL_RSP:			
			mcap_free(controlPkt);
			break;
		case MCAP_MD_DELETE_MDL_RSP:			
			if(controlPkt->responseCode == MCAP_SUCCESS)
			{
				if(controlPkt->mdlId == MCAP_ALL_MDLIDS)
				{
					McapMdl*	tmpMdl;
					/*Try to disconnect all connected MDL*/
					mcl->delAllMdlFlag = FALSE;
					mdl = (McapMdl*)GetHeadList(&mcl->mcapMdlList); 
					while((ListEntry*)mdl != &mcl->mcapMdlList)
					{
						if(!McapCheckMdlState(mdl,MCAP_MDL_STATE_DISC_PENDING))
						{
							tmpMdl = (McapMdl *)GetNextNode(&mdl->node);
							McapFreeMdl(mcl, mdl);
							mdl = tmpMdl;
							continue;
						}
						mdl = (McapMdl *)GetNextNode(&mdl->node);
					}

				}
				else
				{
					mcl->delOneMdlFlag = FALSE;
					mdl = McapGetMdlByMdlId(mcl, controlPkt->mdlId);
					if(!McapCheckMdlState(mdl, MCAP_MDL_STATE_DISC_PENDING))
					{
						McapFreeMdl(mcl, mdl);
					}
				}
			}
			mcap_free(controlPkt);
			break;
		default:
			break;
		}
		
		controlPkt = (McapControlPacket*)GetHeadList(&(mcl->cmdQueue));
		if((ListEntry*)controlPkt != &(mcl->cmdQueue))
		{
			Report(("[MCAP]Send the queued command, opCode = %d", controlPkt->opCode));
			RemoveEntryList(&(controlPkt->node));
			McapSendControl(mcl, controlPkt);
		}
		break;
	case L2EVENT_DATA_IND:
		dataPkt.data = Parms->ptrs.data;
		dataPkt.dataLen = Parms->dataLen;
		McapHandleMclDataInd(mcl, &dataPkt, &appParms);
		break;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
		case L2EVENT_FLOW_DATA_IND:
		{
			BtRxPacket *rxPacket;
						
			if(IsListEmpty(&(mcl->receivedPacketsList)) )
			{
				Report(("Data Flow ind, rx buffer shall not be empty"));
				Assert(0);
			}
			rxPacket = (BtRxPacket *)RemoveHeadList(&(mcl->receivedPacketsList) );
			dataPkt.dataLen = rxPacket->dataLen;
			dataPkt.data = rxPacket->data;
			McapHandleMclDataInd(mcl, &dataPkt, &appParms);
			rxPacket->dataLen = 0;
			InsertTailList(&(mcl->rxPacketsBufferList), &rxPacket->node);
			L2CAP_UpdateSduDataBufferSeq(Cid);
		}
		break;
#endif
	default:		
		OS_Report("[MCAP]Unsupported L2CAP event %d", Parms->event);
		break;
	}
}

void McapMclStateConnected(McapMcl *mcl, L2capChannelId Cid, L2capCallbackParms *Parms)
{
	OS_Report("[MCAP]Run MCL CONNECTED machine: L2CAP event = %d", Parms->event);
	McapMclStateCommonMachine(mcl, Cid, Parms);
}

void McapMclStateActive(McapMcl *mcl, L2capChannelId Cid, L2capCallbackParms *Parms)
{
	OS_Report("[MCAP]Run MCL ACTIVE machine: L2CAP event = %d", Parms->event);
	McapMclStateCommonMachine(mcl, Cid, Parms);
}

void McapMclStatePending(McapMcl *mcl, L2capChannelId Cid, L2capCallbackParms *Parms)
{
    McapCallbackParms	appParms;
	McapMdl*			mdl;
	McapMdlId			mdlId;
	McapControlPacket*	controlPkt;	
	McapControlPacket*	txPkt;
	McapOpCode			opCode;
	McapDataPacket		dataPkt;
	appParms.mcl = mcl;	
	appParms.remDev = Parms->aclLink;
	
	OS_Report("[MCAP]Run MCL PENDING machine: L2CAP event = %d", Parms->event);

	switch (Parms->event) 
	{
	case L2EVENT_DISCONNECTED:
		/*Try to disconnect all connected MDL*/
		mdl = (McapMdl*)GetHeadList(&mcl->mcapMdlList); 
		while((ListEntry*)mdl != &mcl->mcapMdlList)
		{
			if(mdl->mdlState == MCAP_MDL_STATE_OPEN)
				L2CAP_DisconnectReq(mdl->l2ChannelId);
			mdl = (McapMdl *)GetNextNode(&mdl->node);
		}
		McapMclStateTrans(mcl,MCAP_MCL_STATE_IDLE);
		McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_MCLCLOSED);

		/*Free the MCL if there is no MDL in it*/
		McapFreeMcl(mcl);
		break;
	case L2EVENT_PACKET_HANDLED:
		txPkt = Parms->ptrs.packet->ulpContext;
		MCAP(btPacketInUse) = FALSE;		
		mcap_free(Parms->ptrs.packet->data);
		
		switch(txPkt->opCode)
		{
		case MCAP_MD_ABORT_MDL_RSP:
			if(txPkt->responseCode == MCAP_SUCCESS)
			{
				if(McapIsMclEmpty(mcl) == FALSE)
					McapMclStateTrans(mcl,MCAP_MCL_STATE_ACTIVE);
			}
			break;
		default:
			break;
		}
		mcap_free(txPkt);
		
		txPkt = (McapControlPacket*)GetHeadList(&(mcl->cmdQueue));
		if((ListEntry*)txPkt != &(mcl->cmdQueue))
		{
			Report(("[MCAP]Send the queued command, opCode = %d", txPkt->opCode));
			RemoveEntryList(&(txPkt->node));
			McapSendControl(mcl, txPkt);
		}
		break;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
	case L2EVENT_FLOW_DATA_IND:
		{
			BtRxPacket			 *rxPacket;
			McapMdlId			tmpMdlId;
						
			if(IsListEmpty(&(mcl->receivedPacketsList)) )
			{
				Report(("Data Flow ind, rx buffer shall not be empty"));
				Assert(0);
			}
			rxPacket = (BtRxPacket *)RemoveHeadList(&(mcl->receivedPacketsList) );
			dataPkt.data = rxPacket->data;
			dataPkt.dataLen = rxPacket->dataLen;
			
			opCode = rxPacket->data[0];	
			OS_MemCopy((U8*)&tmpMdlId, rxPacket->data+1, 2);
			mdlId = BEtoHost16((U8*)&tmpMdlId);
			mdl = McapGetMdlByMdlId(mcl, mdlId);
			OS_Report("[MCAP]opCode=%d, mdlId = %d",opCode, mdlId);
			controlPkt = (McapControlPacket*)mcap_malloc(sizeof(McapControlPacket));
			switch(opCode)
			{
			case MCAP_MD_CREATE_MDL_REQ:
			case MCAP_MD_RECONNECT_MDL_REQ:
			case MCAP_MD_DELETE_MDL_REQ:
				controlPkt->opCode = opCode+1;
				controlPkt->responseCode = MCAP_INVALID_OPERATION;
				controlPkt->mdlId = mdlId;
				McapSendControl(mcl, controlPkt);
				break;
			case MCAP_MD_ABORT_MDL_REQ:
				if(McapMclCheckParms(mcl, &dataPkt) == BT_STATUS_FAILED)
					return; 
				appParms.mdl = mdl;			
				McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_ABORT_IND);
				
				controlPkt->opCode = MCAP_MD_ABORT_MDL_RSP;
				controlPkt->mdlId = mdlId;
				controlPkt->responseCode = MCAP_SUCCESS;
				McapSendControl(mcl, controlPkt);
				McapFreeMdl(mcl, mdl);
				break;
			default:
				break;
			}
			rxPacket->dataLen = 0;
			InsertTailList(&(mcl->rxPacketsBufferList), &rxPacket->node);
			L2CAP_UpdateSduDataBufferSeq(Cid);
		}
		break;		
#endif
	default:		
		OS_Report("[MCAP]Unsupported L2CAP event %d", Parms->event);
		break;
	}
}

void McapMclStateDisconnecting(McapMcl *mcl, L2capChannelId Cid, L2capCallbackParms *Parms)
{
    McapCallbackParms	appParms;
	McapMdl*			mdl;
	McapMdlId			mdlId;
	McapControlPacket*	controlPkt;	
	McapControlPacket*	txPkt;
	McapOpCode			opCode;
	McapDataPacket		dataPkt;
	appParms.mcl = mcl;	
	appParms.remDev = Parms->aclLink;
	
	OS_Report("[MCAP]Run MCL DISCONNECTINGING machine: L2CAP event = %d", Parms->event);

	switch (Parms->event) 
	{
	case L2EVENT_DISCONNECTED:
		McapMclStateTrans(mcl,MCAP_MCL_STATE_IDLE);
		if (mcl->reopen)
		{
			mcl->reopen = FALSE;
			MCAP_OpenMcl(mcl->remDev, mcl->remPsm);
		} 
		else 
		{
			McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_MCLCLOSED);

			/*Free the MCL if there is no MDL in it*/
			McapFreeMcl(mcl);
		}
		break;
	case L2EVENT_PACKET_HANDLED:		
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
	case L2EVENT_FLOW_DATA_IND:		
#endif
	default:		
		OS_Report("[MCAP]Unsupported L2CAP event %d", Parms->event);
		break;
	}
}



void McapMdlStateClosed(McapMcl *mcl, McapMdl *mdl, L2capChannelId Cid, L2capCallbackParms *Parms)
{
	/*Need not handle any L2CAP events*/
	return;
}

void McapMdlStateOpen(McapMcl *mcl, McapMdl *mdl, L2capChannelId Cid, L2capCallbackParms *Parms)
{
    McapCallbackParms	appParms;
	McapDataPacket		dataPkt;
	BtStatus				status;
	OS_Report("[MCAP]Run MDL OPEN machine: L2CAP event = %d", Parms->event);
	
	appParms.mcl = mcl;
	appParms.mdl = mdl;
	appParms.remDev = Parms->aclLink;

	switch(Parms->event)
	{
	case L2EVENT_DISCON_IND:
		McapMdlStateTrans(mdl,MCAP_MDL_STATE_DISC_PENDING);
		break;	
	case L2EVENT_DISCONNECTED:
		if(mdl->resetFlag == TRUE)
		{
			if(mdl->connFlag == TRUE)
			{
				status = L2CAP_ConnectReq(&(MCAP(dataPsm)), mdl->mdlPsm, 
										mcl->remDev, 0, &mdl->l2ChannelId);
				OS_Report("[MCAP:]Establishing L2CAP connection for MDL: CID = %d", mdl->l2ChannelId);
				if (status != BT_STATUS_PENDING)
				{
					OS_Report("[MCAP]L2CAP connect request failed %d", status);
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
			}
			McapMdlStateTrans(mdl,MCAP_MDL_STATE_CONN_PENDING);
			mdl->resetFlag = FALSE;
			break;
		}

		McapMdlStateTrans(mdl,MCAP_MDL_STATE_CLOSED);
		mdl->l2ChannelId = L2CID_INVALID;
		mdl->connFlag = FALSE;
		McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_MDLCLOSED);
		break;
	case L2EVENT_PACKET_HANDLED:
		appParms.txPkt = Parms->ptrs.packet;
		McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_PACKET_HANDLED);
		break;
	case L2EVENT_DATA_IND:
		dataPkt.data = Parms->ptrs.data;
		dataPkt.dataLen = Parms->dataLen;
		appParms.rxPkt = &dataPkt;
		McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_PACKET_IND);
		break;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
	case L2EVENT_FLOW_DATA_IND:
	{
		BtRxPacket *rxPacket;
								
		if(IsListEmpty(&(mdl->receivedPacketsList)) )
		{
			Report(("Data Flow ind, rx buffer shall not be empty"));
			Assert(0);
		}
		rxPacket = (BtRxPacket *)RemoveHeadList(&(mdl->receivedPacketsList) );
		dataPkt.dataLen = rxPacket->dataLen;
		dataPkt.data = rxPacket->data;
		appParms.rxPkt = &dataPkt;
		rxPacket->dataLen = 0;
		InsertTailList(&(mdl->rxPacketsBufferList), &rxPacket->node);
		L2CAP_UpdateSduDataBufferSeq(Cid);
		//if(mdl->l2config.inLinkMode == L2MODE_STREAMMING && (g_mcap_ignore++%4) == 0)
			//break;
		McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_PACKET_IND);
	}
	break;
#endif

	default:
		break;
	}

}

void McapMdlStateConnPending(McapMcl *mcl, McapMdl *mdl, L2capChannelId Cid, L2capCallbackParms *Parms)
{
    McapCallbackParms	appParms;
	L2capConnSettings 	Setting;
	OS_Report("[MCAP]Run MDL Connect Pending machine: L2CAP event = %d", Parms->event);
	appParms.mcl = mcl;
	appParms.mdl = mdl;
	appParms.remDev = Parms->aclLink;

	switch(Parms->event)
	{
	case L2EVENT_CONNECT_IND:
		mdl->l2ChannelId = Cid;
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED	
//#if 0			
		Setting.reasmBuffer = (ListEntry *)&(mdl->rxPacketsBufferList);
		Setting.receivedBuffer = (ListEntry *)&(mdl->receivedPacketsList);
		Setting.rxWindowSize = MCAP_MAX_RETRANS_NUM;  
		OS_MemSet((U8 *)(mdl->txMaxCountPerIFrame), 0, 128); /* Always 128 */
		OS_MemSet((U8 *)(mdl->txSrejList), 0, sizeof(mdl->txSrejList) ); 
		Setting.txMaxTransmitCount = (U8 *)(mdl->txMaxCountPerIFrame);
		Setting.txSrejList = (U8 *)(mdl->txSrejList);		  
		Setting.maxSrejNo = RF_L2CAP_MAX_SREJ_SIZE; 
					
		L2CAP_ConnectRsp(Cid, L2CONN_ACCEPTED, &Setting);
#else
		L2CAP_ConnectRsp(Cid, L2CONN_ACCEPTED, NULL);
#endif
		break;
	case L2EVENT_CONNECTED:
		McapMdlStateTrans(mdl,MCAP_MDL_STATE_OPEN);
		if(mcl->pendingMdl != NULL)
		{
			if(mdl->mdlId == mcl->pendingMdl->mdlId)
			{
				mcl->pendingMdl = NULL;
				McapMclStateTrans(mcl,MCAP_MCL_STATE_ACTIVE);
			}
		}
		
		if(mcl->currentCmd == MCAP_MD_RECONNECT_MDL_RSP)
		{
#ifdef __BT_2_1_ERTM__
			OS_Report("[MCAP]mdl->l2config.minRemoteMtu VS L2CAP_GetTxMtu(mdl->l2ChannelId: %d vs %d", mdl->l2config.minRemoteMtu, L2CAP_GetTxMtu(mdl->l2ChannelId));
			OS_Report("[MCAP]mdl->l2config.inLinkMode VS L2CAP_GetConnectionMode(mdl->l2ChannelId, FALSE): %d vs %d", mdl->l2config.inLinkMode, L2CAP_GetConnectionMode(mdl->l2ChannelId, FALSE));
			if(mdl->l2config.minRemoteMtu != L2CAP_GetTxMtu(mdl->l2ChannelId)
				|| mdl->l2config.inLinkMode != L2CAP_GetConnectionMode(mdl->l2ChannelId, FALSE))
			{
				OS_Report("[MCAP]Changed L2CAP configuration of reconnect, disconnect");
				L2CAP_DisconnectReq(mdl->l2ChannelId);
				break;
			}
#endif
		}
		else
			mdl->l2config.minRemoteMtu = L2CAP_GetTxMtu(mdl->l2ChannelId);
		
		if(mdl->connFlag)
			McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_MDLOPENED_CNF);
		else
			McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_MDLOPENED_IND);
		break;
	case L2EVENT_DISCONNECTED:
		if(mcl->pendingMdl != NULL)
		{
			if(mdl->mdlId == mcl->pendingMdl->mdlId)
			{
				mcl->pendingMdl = NULL;
			}
		}
		mdl->l2ChannelId = L2CID_INVALID;
		if(mdl->connFlag)
			McapAppCallBack(&appParms, BT_STATUS_FAILED, MCAPEVENT_MDLOPENED_CNF);
		else
			McapAppCallBack(&appParms, BT_STATUS_FAILED, MCAPEVENT_MDLOPENED_IND);
		McapFreeMdl(mcl, mdl);
		break;
	default:
		break;
	}
}

void McapMdlStateDiscPending(McapMcl *mcl, McapMdl *mdl, L2capChannelId Cid, L2capCallbackParms *Parms)
{
    McapCallbackParms	appParms;
	McapControlPacket*	controlPkt;
	McapMdl* 			tmpMdl;
	
	OS_Report("[MCAP]Run MDL Disconnect Pending machine: L2CAP event = %d", Parms->event);
	appParms.mcl = mcl;
	appParms.mdl = mdl;
	appParms.remDev = Parms->aclLink;
	controlPkt = (McapControlPacket*)mcap_malloc(sizeof(McapControlPacket));

	switch(Parms->event)
	{
	case L2EVENT_PACKET_HANDLED:
		appParms.txPkt = Parms->ptrs.packet;
		McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_PACKET_HANDLED);
		break;
	case L2EVENT_DISCONNECTED:
		if(mdl->resetFlag == TRUE)
		{
			McapMdlStateTrans(mdl,MCAP_MDL_STATE_CONN_PENDING);
			mdl->l2ChannelId = L2CID_INVALID;
			mdl->resetFlag = FALSE;
			break;
		}
		McapMdlStateTrans(mdl,MCAP_MDL_STATE_CLOSED);
		mdl->l2ChannelId = L2CID_INVALID;
		mdl->connFlag = FALSE;
		McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_MDLCLOSED);
		if(mdl->delFlag == TRUE)
		{
			if(mcl->delAllMdlFlag == TRUE)
			{
				BOOL 	AllMdlDeleted = TRUE;
				tmpMdl = (McapMdl*)GetHeadList(&mcl->mcapMdlList); 
				while((ListEntry*)tmpMdl != &mcl->mcapMdlList)
				{
					if(!McapCheckMdlState(tmpMdl,MCAP_MDL_STATE_CLOSED))
					{	
						AllMdlDeleted = FALSE;
					}
					tmpMdl = (McapMdl *)GetNextNode(&tmpMdl->node);
				}
				
				if(AllMdlDeleted)
				{
					controlPkt->mdlId = MCAP_ALL_MDLIDS;
					controlPkt->opCode = MCAP_MD_DELETE_MDL_RSP;
					controlPkt->responseCode = MCAP_SUCCESS;
					McapSendControl(mcl, controlPkt);					
				}
			}
			else
			{
				if(mcl->delOneMdlFlag == TRUE)
				{
					controlPkt->mdlId = mdl->mdlId;
					controlPkt->opCode = MCAP_MD_DELETE_MDL_RSP;
					controlPkt->responseCode = MCAP_SUCCESS;
					McapSendControl(mcl, controlPkt);					
					//McapFreeMdl(mcl, mdl);
				}
			}
		}
		break;
	default:
		break;
	}
}

#endif /* __BT_MCAP__ */

