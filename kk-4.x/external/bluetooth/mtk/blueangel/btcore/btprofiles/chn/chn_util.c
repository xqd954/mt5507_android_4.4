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

/****************************************************************************
 *
 * File:
 *     $Workfile:hfg_util.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains utility functions for the Hands-free SDK.
 *             
 * Created:     February 11, 2005
 *
 * Copyright 2000-2005 Extended Systems, Inc.

 * Portions copyright 2005 iAnywhere Solutions, Inc.

 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,  
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
/*****************************************************************************
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 * 
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *  *
 ****************************************************************************/
#include "chn.h"
#include "conmgr.h"
#include "btalloc.h"
#include "bt_feature.h"

BtStatus ChnChangeSubstate(ChnChannel *Channel, U8 newState)
{
    BtRemoteDevice *remDev;
    U8 preState = ChnGetSubstate(Channel);
    BtStatus    status;

    /* Pre-actions of changing substate */
    switch(newState)
    {
    case CHN_SUBSTATE_CLOSED1:
        /* FLOW : Enter */
        /* FLOW : Deregister server channel */
        ChnDeregisterServerChannel(Channel);
        /* FLOW : Register server channel */
        ChnRegisterServerChannel(Channel);
        /* FLOW : Disassociate ACL link */
        ChnCloseChannel(Channel);
        /* Clear channel state */
        Channel->flags = 0;
        Channel->queryFlags = 0;
	  Channel->bTxInProgress = FALSE;
	 ChnResetTxBuffer(Channel);
        break;
    case CHN_SUBSTATE_CONN_OUT1:
    case CHN_SUBSTATE_CONN_OUT2:
        /* FLOW : Deregister server channel */
        //ChnDeregisterServerChannel(Channel);
        //Channel->flags |= CHANNEL_FLAG_OUTGOING;
        break;
    case CHN_SUBSTATE_CONN_OUT3:
    	break;
    case CHN_SUBSTATE_CONN_IN1:
    	break;
    case CHN_SUBSTATE_CONN_IN2:
    	break;
    case CHN_SUBSTATE_DISC1:
    	break;
    case CHN_SUBSTATE_OPEN1:
    	break;
    case CHN_SUBSTATE_OPEN2:
    	break;
    case CHN_SUBSTATE_SLC1:
        /* SLC is created, so open SCO is allowed */
        DBG_PRINT_FLOW(("[CHN][FLOW] substate is S2 : %s", (preState == CHN_SUBSTATE_SLC2)?"Yes":"No"));
        if(preState != CHN_SUBSTATE_SLC2)
        {
            status = CMGR_RegisterScoHandler(&Channel->cmgrHandler);
            DBG_PRINT_FLOW(("[CHN][FLOW] Register SCO handler : %d", status));
        }
        break;
    case CHN_SUBSTATE_SLC2:
        break;
    default:
        return FALSE;
    }

    DBG_PRINT_FLOW( ("[CHN][FLOW]  Change state from 0x%X to 0x%X", ChnGetSubstate(Channel), newState) );
    /* Change to new substate */
    ChnGetSubstate(Channel) = newState;

    /* Post-actions of changing substate */
    switch(newState)
    {
    case CHN_SUBSTATE_CLOSED1:
        /* FLOW : Callback CHN_EVENT_SERVICE_DISCONNECTED */
        //remDev = CMGR_GetRemoteLink(&Channel->cmgrHandler);
        remDev = CMGR_GetRemoteDevice(&Channel->cmgrHandler);
        if (remDev ==0)
            remDev = Channel->cmgrHandler.remDev;
        ChnAppCallback(	Channel, 
                                 CHN_EVENT_SERVICE_DISCONNECTED,
                                 BT_STATUS_SUCCESS, 
                                 (U32)&remDev->bdAddr);
        Channel->cmgrHandler.remDev = 0;
        Channel->cmgrHandler.bdc = 0;
        /* FLOW : C1 Exit */
        break;
    case CHN_SUBSTATE_CONN_OUT1:
        break;
    case CHN_SUBSTATE_CONN_OUT2:
        break;
    case CHN_SUBSTATE_CONN_OUT3:
        break;
    case CHN_SUBSTATE_CONN_IN1:
        /* FLOW : Callback CHN_EVENT_AUTH_REQ */
        remDev = Channel->remDev;
        ChnAppCallback(	Channel, 
        				CHN_EVENT_AUTH_REQ,
                           	BT_STATUS_SUCCESS, 
                           	(U32)&remDev->bdAddr);
        break;
    case CHN_SUBSTATE_CONN_IN2:
    	break;
    case CHN_SUBSTATE_DISC1:
    	break;
    case CHN_SUBSTATE_OPEN1:
        /* FLOW : Callback CHN_EVENT_RFCOMM_CONNECTED */
        /* Indicate the CHN currently negotiate with HF device */
        //Channel->flags |= CHANNEL_FLAG_NEGOTIATE;
        remDev = Channel->remDev;
        ChnAppCallback(Channel, 
        				CHN_EVENT_RFCOMM_CONNECTED,
                                  BT_STATUS_SUCCESS, 
                                 (U32)&remDev->bdAddr);
        break;
    case CHN_SUBSTATE_OPEN2:
    	break;
    case CHN_SUBSTATE_SLC1:
        /* FLOW : Callback CHN_EVENT_SERVICE_CONNECTED */
        DBG_PRINT_FLOW(("[CHN][FLOW] Previous substate is S2 : %s", (preState == CHN_SUBSTATE_SLC2)?"Yes":"No"));
        if(preState != CHN_SUBSTATE_SLC2)
        {
            ChnReportServiceConnected(Channel);
        }
        break;
    case CHN_SUBSTATE_SLC2:
        break;
    default:
        return FALSE;
    }

    /*Report(("- ChnChangeSubstate: already changed to <%d>.", ChnGetSubstate(Channel)));*/
    return TRUE;
}

BtStatus ChnRegisterServerChannel(ChnChannel *Channel)
{
    BtStatus status = BT_STATUS_FAILED;
    BOOL bReg = (Channel->rfChannel->userContext != NULL)?TRUE:FALSE;

    /*Report( ("[CHN][%s]  +Channel==x%X", __func__, (U32)Channel) );*/
    /* FLOW : Register server channel */
    DBG_PRINT_FLOW(("[CHN][FLOW] Server channel registered : %s", bReg?"Yes":"No"));
    if(bReg == FALSE)
    {
        status = RF_RegisterServerChannel(Channel->rfChannel, &Channel->chnService, 1);
        DBG_PRINT_FLOW(("[CHN][FLOW] Register server channel : %d", status));
        if(status != BT_STATUS_SUCCESS)
        {      
            DBG_PRINT_ERROR(("[CHN][ERR] Register server channel failed : %d - (%s,%d)", status, __FILE__, __LINE__));
            Assert(0);
        }
        else	
        {      
            Channel->rfChannel->userContext = (void*)1;
        }
    }
    else
    {
        DBG_PRINT_WARN(("[CHN][WARN] Server channel already registered - (%s,%d)", __FILE__, __LINE__));
    }
    Report( ("[CHN][ChnRegisterServerChannel]  - return status==x%X", status) );
    return status;
}

BtStatus ChnDeregisterServerChannel(ChnChannel *Channel)
{
    BtStatus status = BT_STATUS_FAILED;
    BOOL bReg = (Channel->rfChannel->userContext != NULL)?TRUE:FALSE;
    
    /*Report( ("[CHN][%s]  +Channel==x%X", __func__, (U32)Channel) );*/
    DBG_PRINT_FLOW(("[CHN][FLOW] Server channel registered : %s", bReg?"Yes":"No"));
    /* FLOW : Deregister server channel */
    if(Channel->rfChannel->userContext != NULL)
    {
        status = RF_DeregisterServerChannel(Channel->rfChannel, &Channel->chnService);
        DBG_PRINT_FLOW(("[CHN][FLOW] Deregister server channel : %d", status));
        if(status != BT_STATUS_SUCCESS)
        {
            /* It might be failed due to authentication is ongoing */
            DBG_PRINT_WARN(("[CHN][WARN] ChnDeregisterServerChannel failed. status=%d - (%s,%d)", status, __FILE__, __LINE__));
        }
        else
        {
            Channel->rfChannel->userContext = NULL;
        }	
    }	
    Report( ("[CHN][ChnDeregisterServerChannel]  - return status==x%X", status) );
    return status;
}

#if 0
BtStatus ChnFlushRespQueue(ChnChannel *Channel)
{
    BtStatus status = BT_STATUS_SUCCESS;
    AtResults *atResults;
    U16 count = 0, callback = 0;
    
    /* Report( ("[CHN][%s]  +Channel==x%X", __func__, (U32)Channel) ); */

    while (!IsListEmpty(&Channel->rspQueue)) 
    {
        atResults = (AtResults*)RemoveHeadList(&Channel->rspQueue);
        if(atResults != &Channel->atResults)
        {
            /* Results sent from application */
            ChnAppCallback(Channel, CHN_EVENT_RESPONSE_COMPLETE, 
                                   BT_STATUS_NO_CONNECTION, (U32)atResults);
            callback++;
        } 
        count++;
    }
    DBG_PRINT_FLOW(("[CHN][FLOW] Flush response queue : flush %d entry, callback %d times", count, callback));
    
    /*Report( ("[CHN][%s]  - return status==x%X", __func__, status) );*/
    return status;
}
#endif

BtStatus ChnDisconnecting(ChnChannel *Channel)
{
    BtStatus status = BT_STATUS_PENDING;
    U16 substate = ChnGetSubstate(Channel);
    /*
    Report( ("[CHN][%s]  +Channel==x%X, substate=0x%X", __func__, (U32)Channel, ChnGetSubstate(Channel)) );
    */
    DBG_PRINT_FLOW(("[CHN][FLOW] Disconnecting : substate=0x%X", ChnGetSubstate(Channel)));

    if(Channel->queryFlags & SDP_QUERY_FLAG_ONGOING)
    {
        /* SDP query is ongoing */
        DBG_PRINT_FLOW(("[CHN][FLOW] SDP query ongoing"));
        goto Pending;
    }

    if(Channel->linkFlags & CHN_LINK_SCO_CONNECTING)
    {
        /* SCO connection creation ongoing */
        DBG_PRINT_FLOW(("[CHN][FLOW] SCO connecting ongoing"));
        goto Pending;
    }

    if(Channel->linkFlags & CHN_LINK_SCO_DISCONNECTING)
    {
        /* SCO connection disconnecting ongoing */
        DBG_PRINT_FLOW(("[CHN][FLOW] SCO disconnecting ongoing"));
        goto Pending;
    }

    if(substate == CHN_SUBSTATE_CONN_IN2 || substate == CHN_SUBSTATE_CONN_OUT3)
    {
        /* Waiting RFCOMM confirm */
        DBG_PRINT_FLOW(("[CHN][FLOW] Substate is I2/O3. Waiting RFCOMM confirm"));
        goto Pending;        
    }

    if(substate == CHN_SUBSTATE_CONN_OUT1 )
    {
        /* Waiting CMGR data link confirm */
        DBG_PRINT_FLOW(("[CHN][FLOW] Substate is O1. Waiting CMGR data link confirm"));
        goto Pending;
    }

    if(ChnGetSubstate(Channel) != CHN_SUBSTATE_DISC1)
    {
        DBG_PRINT_FLOW(("[CHN][FLOW] Substate is not D1."));
        ChnChangeSubstate(Channel, CHN_SUBSTATE_DISC1);
    }
        
    if(CMGR_IsAudioUp(&(Channel->cmgrHandler)))
    {
        DBG_PRINT_FLOW(("[CHN][FLOW] SCO connection existed"));
        status = CMGR_RemoveAudioLink(&Channel->cmgrHandler);
        DBG_PRINT_FLOW(("[CHN][FLOW] Close Audio channel : %d", status));
        Report(("[CHN][ChnDisconnecting] Remove audio link : return %d", status));
        if( status == BT_STATUS_PENDING || status == BT_STATUS_IN_PROGRESS )
        {
            status = BT_STATUS_PENDING;
            Channel->linkFlags |= CHN_LINK_SCO_DISCONNECTING;
            goto Pending;
        }
    }

    DBG_PRINT_FLOW(("[CHN][FLOW] Deregister SCO handler"));
    CMGR_DeregisterScoHandler(&Channel->cmgrHandler);
    
    if(Channel->linkFlags & CHN_LINK_HANDSFREE)
    {
        DBG_PRINT_FLOW(("[CHN][FLOW] RFCOMM channel existed"));
        status = RF_CloseChannel(Channel->rfChannel);
        DBG_PRINT_FLOW(("[CHN][FLOW] Close RFCOMM channel : %d", status));
        if( status == BT_STATUS_PENDING || status == BT_STATUS_IN_PROGRESS )
        {
            goto Pending;
        }
    }

    //ChnFlushRespQueue(Channel);

    ChnChangeSubstate(Channel, CHN_SUBSTATE_CLOSED1);
Pending:
    DBG_PRINT_FLOW(("[CHN][FLOW] Disconnecting return : %d", status));
    /*Report( ("[CHN][%s]  - return status==x%X", __func__, status) );*/
    return status;
}

/*---------------------------------------------------------------------------
 *            ChnAppCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sets up the callback parameters and calls the application.
 *
 * Return:    void
 */
void ChnAppCallback(ChnChannel *Channel, ChnEvent Event, 
                    BtStatus Status, U32 data)
{
    ChnCallbackParms chnParms;
	
    chnParms.event = Event;
    chnParms.p.ptr = (void *)data;
    chnParms.status = Status;
    chnParms.errCode = Channel->cmgrHandler.errCode;

    DBG_PRINT_CALLBACK(("[CHN][CALLBACK] Event=%d, Status=%d, data=0x%X, errCode=%d", Event, Status, data, chnParms.errCode));
    Channel->callback(Channel, &chnParms);
}

/*---------------------------------------------------------------------------
 *            ChnReportServiceConnected()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Indicates that the service level connection has been established.
 *
 * Return:    void
 */
void ChnReportServiceConnected(ChnChannel *Channel)
{
    Report( ("[CHN][ChnReportServiceConnected]  +Channel==x%X", (U32)Channel) );

    /* Service channel is up, tell the app */
    //Channel->state = CHN_STATE_OPEN;
    Channel->cmgrHandler.errCode = BEC_NO_ERROR;
    ChnAppCallback(Channel, CHN_EVENT_SERVICE_CONNECTED, BT_STATUS_SUCCESS, 
                   (U32)&Channel->cmgrHandler.bdc->link->bdAddr);

    Report( ("[CHN][ChnReportServiceConnected]  -") );
}

/*---------------------------------------------------------------------------
 *            ChnCloseChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close ACL association and set link flag to 0
 *
 * Return (BtStatus) : BT_STATUS_SUCCESS means successfully disassociated.
 */
BtStatus ChnCloseChannel(ChnChannel *Channel)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BOOL bACL = ((Channel->linkFlags & CHN_LINK_ACL)==CHN_LINK_ACL)?TRUE:FALSE;

    DBG_PRINT_FLOW(("[CHN][FLOW] ACL associated : %s", bACL?"Yes":"No"));
    /* FLOW : Disassociate ACL link */
    if (bACL) 
    {
        status = CMGR_RemoveDataLink(&Channel->cmgrHandler);
        DBG_PRINT_FLOW(("[CHN][FLOW] Disassociate ACL link : %d", status));
        Channel->linkFlags = 0;
        if(status != BT_STATUS_SUCCESS)
        {
            DBG_PRINT_ERROR(("[CHN][ERR] CMGR_RemoveDataLink : %d - (%s,%d)", status, __FILE__, __LINE__));
            Assert(0);
        }
    }
    /* No sniff timer needed any more */
    (void)CMGR_ClearSniffTimer(&Channel->cmgrHandler);
    return status;
}

/*---------------------------------------------------------------------------
 *            ChnOpenChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open ACL association and set set CHN_LINK_ACL to linkflag 
 *
 * Return (BtStatus) : BT_STATUS_SUCCESS means successfully disassociated.
 */
BtStatus ChnOpenChannel(ChnChannel *Channel, BD_ADDR *Addr)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BOOL bACL = ((Channel->linkFlags & CHN_LINK_ACL)==CHN_LINK_ACL)?TRUE:FALSE;

    DBG_PRINT_FLOW(("[CHN][FLOW] ACL associated : %s", bACL?"Yes":"No"));
    if( !bACL )
    {
        status = CMGR_CreateDataLink(&Channel->cmgrHandler, Addr);
        DBG_PRINT_FLOW(("[CHN][FLOW] Associate ACL link : %d", status));
        if (status == BT_STATUS_SUCCESS)
        {
            /* Start the service search */
            Channel->linkFlags |= CHN_LINK_ACL;
        }
        else if(status != BT_STATUS_PENDING)
        {
            DBG_PRINT_ERROR(("[CHN][ERR] CMGR_CreateDataLink failed. status=%d - (%s,%d)", status, __FILE__, __LINE__));
            Assert(0);
        }
    }

    Report( ("[CHN][ChnOpenChannel] - return status==x%X", status) );
    return status;
}

/*---------------------------------------------------------------------------
 *            ChnFindChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Find the Chn channel based on the remote device.
 *
 * Return:    A pointer to the hf channel, NULL if not found.
 */
ChnChannel * ChnFindChannel(BtRemoteDevice *remDev)
{
    ChnChannel *Channel;

    /*Report( ("[CHN][%s] +BtRemoveDevice==x%X", __func__, (U32)remDev) );*/
    
    Channel = (ChnChannel*)GetHeadList(&chnContext->channelList);
    while (&Channel->node != &chnContext->channelList) {
        if ((Channel->cmgrHandler.bdc) && 
            (CMGR_GetRemoteDevice(&Channel->cmgrHandler) == remDev)) {
            break;
        }

        /* Look at the next node */
        Channel = (ChnChannel*)GetNextNode(&Channel->node);
    }

    if (&Channel->node == &chnContext->channelList) {
        /* Not found */
        Channel = 0;
    }

    /*Report( ("[CHN][%s] - return ChnChannel==x%X", __func__, ChnChannel) );*/

    return Channel;
}

ChnChannel * ChnFindRegisteredChannel(RfChannel *rfChannel)
{
    	ChnChannel *Channel = NULL;

    Report( ("[CHN][ChnFindRegisteredChannel] +RfChannel==x%X", (U32)rfChannel) );

    	Channel = (ChnChannel *)GetHeadList(&chnContext->channelList);
    	while (&Channel->node != &chnContext->channelList) 
	{
        if (Channel->rfChannel == rfChannel) 
            		break;

        /* Look at the next node */
        Channel = (ChnChannel *)GetNextNode(&Channel->node);
    }

    	if (&Channel->node == &chnContext->channelList) 
	{
        /* Not found */
        	Channel = NULL;
    }

    Report( ("[CHN][ChnFindRegisteredChannel] - return ChnChannel==x%X", Channel) );
    

    return Channel;
}

/*---------------------------------------------------------------------------
 *            ChnAllocRfChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Allocate an RF channel (make it "in use").
 *
 * Return:    A pointer to the RF channel, NULL if not found.
 */
RfChannel *ChnAllocRfChannel(void)
{
    RfChannel *rfChannel = 0;

    Report(("+ ChnAllocRfChannel."));
	
    if (!IsListEmpty(&chnContext->freeRfChannelList)) 
    {
        rfChannel = (RfChannel *)RemoveHeadList(&chnContext->freeRfChannelList); 
    }

    if (rfChannel) 
    {
        rfChannel->callback = ChnRfCallback;
        rfChannel->maxFrameSize = RF_MAX_FRAME_SIZE;
        rfChannel->priority = RF_DEFAULT_PRIORITY;
    }
    else
    {
        /* Allocate fail */
        DBG_PRINT_ERROR(("[CHN][ERR] Alloc RfChannel failed - (%s,%d)", __FILE__, __LINE__));
    }


    Report(("- ChnAllocRfChannel."));
    return rfChannel;
}

/*---------------------------------------------------------------------------
 *            ChnFreeRfChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Frees an RF channel (makes it available).
 *
 * Return:    void
 */
void ChnFreeRfChannel(RfChannel *rfChannel)
{
    Report(("+ ChnFreeRfChannel."));

    if (!IsNodeOnList(&chnContext->freeRfChannelList, &rfChannel->node)) 
    {
        rfChannel->userContext = NULL;
        InsertTailList(&chnContext->freeRfChannelList, &rfChannel->node);
    }
    Report(("- ChnFreeRfChannel."));
}

U16 ChnRemainTxData(ChnChannel *Channel)
{
	XaBufferDesc *pBuf = &Channel->txBuffer;
	U16 size;

	size = (pBuf->writeOffset +  CHN_TX_BUFFER_SIZE);
	size -= pBuf->readOffset;
	size &=CHN_TX_BUFFER_MODE;
	return size;
}

U16 ChnEmptyTxSize(ChnChannel *Channel)
{
	XaBufferDesc *pBuf = &Channel->txBuffer;

	return (CHN_TX_BUFFER_MODE - ChnRemainTxData(Channel));
}

void ChnResetTxBuffer(ChnChannel *Channel)
{
	Channel->txBuffer.readOffset = Channel->txBuffer.writeOffset = 0;
	Channel->txBuffer.buff = Channel->txData;
}

U16 ChnWriteTxBuffer(ChnChannel *Channel, U8 *buf, U16 len)
{
	U16 remain = ChnEmptyTxSize(Channel);
	U16 edge = CHN_TX_BUFFER_SIZE-Channel->txBuffer.writeOffset;
	U8*	ptr = Channel->txBuffer.buff+Channel->txBuffer.writeOffset;
	U16 writeLen;

	if(len > remain)
		len = remain;
	writeLen = len;
	if(len > edge)
	{
		OS_MemCopy(ptr, buf, edge);
		buf += edge;
		ptr = Channel->txBuffer.buff;
		Channel->txBuffer.writeOffset = 0;
		len -= edge;
	}
	if(len > 0)
	{
		OS_MemCopy(ptr, buf, len);
		Channel->txBuffer.writeOffset += len;
	}

	if(!Channel->bTxInProgress && (ChnRemainTxData(Channel) > 0))
		ChnSendTxBuffer(Channel);

	return writeLen;
}

BtStatus ChnSendTxBuffer(ChnChannel *Channel)
{
	BtStatus status = BT_STATUS_FAILED;
	BtPacket *Packet = &Channel->txPacket;
	U16	len = ChnRemainTxData(Channel);

	if(len == 0 || Channel->bTxInProgress)
		return status;

	if(len > (CHN_TX_BUFFER_SIZE-Channel->txBuffer.readOffset))
		len = (CHN_TX_BUFFER_SIZE-Channel->txBuffer.readOffset);
	if (len > RF_FrameSize(Channel->rfChannel)) 
    	{
        	len = RF_FrameSize(Channel->rfChannel);
    	}

    	Channel->txPacket.dataLen = len;
    	Channel->txPacket.data = (Channel->txBuffer.buff+Channel->txBuffer.readOffset);

	status = RF_SendData(Channel->rfChannel, Packet);
	if(status == BT_STATUS_PENDING)
	{
		Channel->txBuffer.readOffset += len;
		Channel->bTxInProgress = TRUE;
	}
	return status;
}

