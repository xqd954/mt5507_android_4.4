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
 *     $Workfile:hid_util.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description:
 *     Utility functions for the HID profile.
 *
 * Created:
 *     Aug 23, 2002
 *
 * Copyright 2002-2004 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, 
 * Inc.  This work contains confidential and proprietary information of 
 * Extended Systems, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#if defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__)

//#include "kal_release.h"
//#include "bluetooth_trc.h"
#include <sys/hidalloc.h>
#ifdef __BT_4_0_BLE__
#include "hid_blescan.h"
#endif

/*---------------------------------------------------------------------------
 *            HidAppCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Calls the application with events.
 *
 * Return:    void
 */
void HidAppCallback(HidChannel *Channel, U8 Event, U32 Status, 
                    void *ptr, U16 len)
{ 
    HidCallbackParms parms;

    parms.event = Event;
    parms.status = Status;
    parms.ptrs.trans = ptr;
    parms.len = len;
#if XA_CONTEXT_PTR == XA_ENABLED
	//bt_prompt_trace(MOD_BT,"[HID]HidAppCallback hidContext:%x",hidContext);
	if(hidContext == NULL)
		return;
//#else
	//bt_prompt_trace(MOD_BT,"[HID]HidAppCallback hidContext:%x",&hidContext);
	//if(&hidContext == NULL)
	//	return;
#endif
	//bt_prompt_trace(MOD_BT,"[HID]HidAppCallback appCallback:%x",HID(appCallback));
	if(HID(appCallback) == NULL)
		return;

    HID(appCallback)(Channel, &parms);
}

/*---------------------------------------------------------------------------
 *            HidClearConnection()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Clears an HID connection and notifies the application.
 *
 * Return:    void
 */
void HidClearConnection(HidChannel *Channel, U16 Status, U16 reason)
{
    HidTransaction *trans;
    HidInterrupt   *intr;
    U32 cbStatus;
    //Assert(Channel->cmgrHandler.bdc !=0);
    if (Channel->cmgrHandler.bdc == 0)
    {
        bt_prompt_trace(MOD_BT, "[HID]Channel->cmgrHandler.bdc == 0");
        return;
    }
    if (Channel->cmgrHandler.remDev == 0)
    {
        bt_prompt_trace(MOD_BT, "[HID]Channel->cmgrHandler.remDev == 0");
        return;
    }    
    kal_trace(BT_TRACE_G2_PROFILES,HIDCLEARCONNECTIONx02X ,Status);    

    /* Release link handler */
    /* Clean up the channel state */
    Channel->conCount = 0;
    Channel->ctrlCid = Channel->intrCid = 0;
    Channel->state = HID_STATE_CLOSED;
    Channel->flags = 0;
   
    /* Return all unsent transaction structures */
    while (!IsListEmpty(&Channel->transQueue)) {
        trans = (HidTransaction *)RemoveHeadList(&Channel->transQueue);
        HidAppCallback(Channel, HIDEVENT_TRANSACTION_COMPLETE, 
                       BT_STATUS_NO_CONNECTION, trans, sizeof(HidTransaction));
    }

    /* Return all unsent interrupt structures */
    while (!IsListEmpty(&Channel->intrQueue)) {
        intr = (HidInterrupt *)RemoveHeadList(&Channel->intrQueue);
        HidAppCallback(Channel, HIDEVENT_INTERRUPT_COMPLETE, 
                       BT_STATUS_NO_CONNECTION, intr, sizeof(HidInterrupt));
    }

    cbStatus = (reason<<16) | Status;
	
    /* Notify the application */
    HidAppCallback(Channel, HIDEVENT_CLOSED, cbStatus,
                   Channel->cmgrHandler.remDev, sizeof(BtRemoteDevice));

#ifdef __BT_4_0_BLE__
    if (Channel->cmgrHandler.bdc->devType == BT_DEV_TYPE_LE)
    {
        BtDeviceRecord record;
        
        if (SEC_FindDeviceRecord(&Channel->cmgrHandler.bdc->link->bdAddr, &record) == BT_STATUS_SUCCESS)
            HidScanAdvUpdate((U8 *)&Channel->cmgrHandler.remDev->bdAddr.addr, TRUE);
        else
            bt_prompt_trace(MOD_BT,"[HID]Key record not found, not add to the white list");
    }
#endif        

    CMGR_RemoveDataLink(&Channel->cmgrHandler);
    CMGR_RemoveCmgrWithDevice(&Channel->cmgrHandler);

    HidFreeChannel(Channel);
}

/*---------------------------------------------------------------------------
 *            HidFindUnusedChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Finds a registered channel which has not been allocated yet.
 *
 * Return:    The channel matching the search criteria.
 */
HidChannel * HidFindUnusedChannel(U8 searchType, BtRemoteDevice *RemDev)
{
    HidChannel *channel = NULL;
    HidChannel *savedChannel = NULL;

    /* Walk the list of channels */
    channel = (HidChannel *)GetHeadList(&HID(hidList));
    kal_trace(BT_TRACE_G2_PROFILES,HIDFINDUNUSEDCHANNELx02X ,searchType);    

    while ((ListEntry *)channel != &HID(hidList)) {
        if (((searchType == HID_FIND_CTRL_CID) && (channel->ctrlCid == 0)) ||
            ((searchType == HID_FIND_INTR_CID) && (channel->intrCid == 0))) {
            /* Found an unallocated CID */
            if ((channel->cmgrHandler.bdc) &&
                (channel->cmgrHandler.bdc->link == RemDev)) {
                /* This is definintely the one */
                savedChannel = channel;
                kal_trace(BT_TRACE_G2_PROFILES,HID_THIS_IS_DEFININTELY_THE_ONE); 
                break;
            } 
        }

        /* Try the next channel */
        channel = (HidChannel *)GetNextNode(&channel->node);
    }

    return savedChannel;
}

BT_BOOL HidCheckValidConnect(U8 searchType, L2capChannelId Cid, BtRemoteDevice *RemDev)
{
    HidChannel *channel = NULL;
    HidChannel *savedChannel = NULL;

    /* Walk the list of channels */
    channel = (HidChannel *)GetHeadList(&HID(hidList));

    while ((ListEntry *)channel != &HID(hidList)) {
        if ((channel->cmgrHandler.bdc) &&
            (channel->cmgrHandler.bdc->link == RemDev)) {
            if (((searchType == HID_FIND_CTRL_CID) && (channel->ctrlCid != Cid)) ||
                ((searchType == HID_FIND_INTR_CID) && (channel->intrCid != Cid))) {
                kal_trace(BT_TRACE_G2_PROFILES,HID_INVALID_CONNECT_REQUEST); 
                return FALSE;
            } 
        }

        /* Try the next channel */
        channel = (HidChannel *)GetNextNode(&channel->node);
    }

    return TRUE;
}

HidChannel * HidNewChannel(void)
{
    HidChannel *channel = NULL;

    channel = (HidChannel *)hid_malloc(sizeof(HidChannel));
    if (channel != NULL)
    {
        OS_MemSet((U8 *)channel, 0, sizeof(HidChannel));
        InitializeListHead(&channel->transQueue);
        InitializeListHead(&channel->intrQueue);
#ifdef __BT_4_0_BLE__
        InitializeListHead(&channel->uuidQueue);
        InitializeListHead(&channel->reportGattTable);
#endif
        InsertTailList(&HID(hidList), &(channel->node));
        CMGR_RegisterHandler(&channel->cmgrHandler,HidCmgrCallback);                 
    }

    return channel;	
}

void HidFreeChannel(HidChannel *channel)
{
    U16 report_id_index;
    U8 report_format_index;
    U8 report_unit_index;
    U8 report_descriptor_index;
    HidReportConstructor *reportCons;

    bt_prompt_trace(MOD_BT,"[HID]Free channel->reportCons");
    if(channel->reportCons != NULL)
    {
    		for(report_descriptor_index = 0; report_descriptor_index < HID_REPORTDESCRIPTOR_MAX; report_descriptor_index++)
    		{
    			reportCons = channel->reportCons[report_descriptor_index];
			if(reportCons == NULL)
			{
				bt_prompt_trace(MOD_BT,"[HID]reportCons is NULL. report_descriptor_index = %d", report_descriptor_index);
				continue;
			}
			for(report_id_index=0; report_id_index<HID_REPORT_ID_MAX; report_id_index++)
			{
				if(&reportCons[report_id_index] != NULL)
				{
					if(reportCons[report_id_index].reportFormat !=NULL)
					{
						for(report_format_index=0; report_format_index<3; report_format_index++)
						{
							if(&reportCons[report_id_index].reportFormat[report_format_index] != NULL)
							{
								if(reportCons[report_id_index].reportFormat[report_format_index].reportUnit !=NULL)
								{
									for(report_unit_index=0; report_unit_index<HID_REPORT_UNIT_MAX_SIZE; report_unit_index++)
									{
										if(&reportCons[report_id_index].reportFormat[report_format_index].reportUnit[report_unit_index] != NULL)
										{
											if(reportCons[report_id_index].reportFormat[report_format_index].reportUnit[report_unit_index].hidUsage!= NULL)
											{	
												hid_free(reportCons[report_id_index].reportFormat[report_format_index].reportUnit[report_unit_index].hidUsage);
											}
										}
										else
										{
											bt_prompt_trace(MOD_BT,"[HID]channel->reportCons[report_id_index].reportFormat[report_format_index].reportUnit[report_unit_index] == NULL");
											bt_prompt_trace(MOD_BT,"[HID]report_id_index=%d,report_format_index=%d,report_unit_index=%d",report_id_index,report_format_index,report_unit_index);
										}
									}
									hid_free(reportCons[report_id_index].reportFormat[report_format_index].reportUnit );
								}
							}
							else
							{
								bt_prompt_trace(MOD_BT,"[HID]channel->reportCons[report_id_index].reportFormat[report_format_index] == NULL");
								bt_prompt_trace(MOD_BT,"[HID]report_id_index=%d,report_format_index=%d",report_id_index,report_format_index);
							}
						}
						hid_free(reportCons[report_id_index].reportFormat);
					}
				}			
				else
				{
					bt_prompt_trace(MOD_BT,"[HID]channel->reportCons[report_id_index] == NULL, report_id_index = %d",report_id_index);
				}
			}
			hid_free(reportCons);
		}
    }
    bt_prompt_trace(MOD_BT,"[HID]Free channel->reportCons successfully");
    hid_free(channel->queryRsp.descriptorList);
    CMGR_DeregisterHandler(&channel->cmgrHandler);
    RemoveEntryList(&(channel->node));
    hid_free(channel);
    bt_prompt_trace(MOD_BT,"[HID]Free channel successfully");
}

/*---------------------------------------------------------------------------
 *            HidFindChannelByCid()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Finds the registered channel using the L2CAP CID.
 *
 * Return:    The channel matching the search criteria.
 */
HidChannel * HidFindChannelByCid(L2capChannelId Cid, BtRemoteDevice *RemDev)
{
    HidChannel *channel = NULL;
    HidChannel *savedChannel = NULL;

    /* Walk the list of channels */
    channel = (HidChannel *)GetHeadList(&HID(hidList));
    while ((ListEntry *)channel != &HID(hidList)) {
        if (((channel->ctrlCid == Cid) || (channel->intrCid == Cid)) &&
            ((channel->cmgrHandler.bdc !=0) &&
            channel->cmgrHandler.bdc->link == RemDev)) {
            /* Found the matching channel */
            savedChannel = channel;
            break;
        }
            
        /* Try the next channel */
        channel = (HidChannel *)GetNextNode(&channel->node);
    }

    return savedChannel;
}

/*---------------------------------------------------------------------------
 *            HidFindChannelByLink()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Find the channel by remote device link. Only for connection initiator
 *
 * Return:    The channel matching the search criteria.
 */
HidChannel *HidFindChannelByLink(BtRemoteDevice *RemDev)
{
    HidChannel *channel = NULL;

    /* Walk the list of channels */
    channel = (HidChannel *)GetHeadList(&HID(hidList));
    while ((ListEntry *)channel != &HID(hidList)) {
        if ((channel->cmgrHandler.bdc !=0) &&
            channel->cmgrHandler.bdc->link == RemDev) {
            /* Found the matching channel */
            return channel;
        }
            
        /* Try the next channel */
        channel = (HidChannel *)GetNextNode(&channel->node);
    }

    return NULL;
}

/*---------------------------------------------------------------------------
 *            HidFindChannelByAddr()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Find the channel by remote address.
 *
 * Return:    The channel matching the search criteria.
 */
HidChannel *HidFindChannelByAddr(BD_ADDR *bdAddr)
{
    HidChannel *channel = NULL;
    BtRemoteDevice *remDev = NULL;   

    if (bdAddr)
    {
        /* Walk the list of channels */
        channel = (HidChannel *)GetHeadList(&HID(hidList));
        while ((ListEntry *)channel != &HID(hidList)) 
        {
            remDev = CMGR_GetRemoteDevice(&channel->cmgrHandler);
            if (remDev && OS_MemCmp(bdAddr->addr, 6, remDev->bdAddr.addr, 6)) 
            {
                /* Found the matching channel */
                OS_Report("HidFindChannelByAddr Found");
                return channel;
            }
                
            /* Try the next channel */
            channel = (HidChannel *)GetNextNode(&channel->node);
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------
 *            HidSendTransPacket()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send a transaction packet.
 *
 * Return:    Status of L2CAP send.
 */
BtStatus HidSendTransPacket(HidChannel *Channel, 
                            HidTransaction *Trans,
                            BtPacket *packet)
{
    BtStatus       status;
    U8             parms = 0;
    U8             offset = 0;

    packet->dataLen = 0;
    kal_trace(BT_TRACE_G2_PROFILES,HIDSENDTRANSPACKET); 
    switch (Trans->type) {

    //Add by stanley: 2007-0607
    case HID_HANDSHAKE:
        /* Setup the handshake packet */
        packet->header[BT_PACKET_HEADER_LEN - 1] = HID_HANDSHAKE;
        packet->header[BT_PACKET_HEADER_LEN - 1] += Trans->resultCode;
        packet->headerLen = 1;
        break;
    case HID_TRANS_CONTROL:
        /* Set up the control packet */
        packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_CONTROL;
        packet->header[BT_PACKET_HEADER_LEN - 1] += Trans->parm.control;
        packet->headerLen = 1;
        break;
    case HID_TRANS_GET_REPORT:
        /* Calculate the header size */
        offset++;
        if (Trans->parm.reportReq->useId) {
            offset++;
        }
        if (Trans->parm.reportReq->bufferSize > 0) {
            parms = 0x08;
            offset += 2;
        }

        /* Request header */
        packet->headerLen = offset;
        parms += Trans->parm.reportReq->reportType;
        packet->header[BT_PACKET_HEADER_LEN - offset] = HID_MSG_GET_REPORT;
        packet->header[BT_PACKET_HEADER_LEN - offset--] += parms;

        /* Report ID */
        if (Trans->parm.reportReq->useId) {
            packet->header[BT_PACKET_HEADER_LEN - offset--] = 
                                                Trans->parm.reportReq->reportId;
        }

        /* Buffer Size */
        if (Trans->parm.reportReq->bufferSize > 0) {
            StoreLE16(&packet->header[BT_PACKET_HEADER_LEN - offset--],
                      Trans->parm.reportReq->bufferSize);
        }
        break;
    case HID_TRANS_SET_REPORT:
        /* Set up the report */
        if (Trans->flags & TRANS_FLAG_START) {
            packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_SET_REPORT;
	   switch(Trans->parm.report->reportType)
	   {
		case HID_REPORT_INPUT:
			packet->header[BT_PACKET_HEADER_LEN - 1] |=HID_REPORT_TYPE_INPUT;
			break;
		case HID_REPORT_OUTPUT:
			packet->header[BT_PACKET_HEADER_LEN - 1] |=HID_REPORT_TYPE_OUTPUT;
			break;
		case HID_REPORT_FEATURE:
			packet->header[BT_PACKET_HEADER_LEN - 1] |=HID_REPORT_TYPE_FEATURE;
			break;
		default:
			break;
	   }
            Trans->offset = 0;
        } else {
            packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_DATC;
        }
        packet->headerLen = 1;
        packet->data = Trans->parm.report->data + Trans->offset;
        packet->dataLen = min(Trans->parm.report->dataLen - Trans->offset, 
                              L2CAP_GetTxMtu(Channel->ctrlCid) - 1);
        Trans->offset += packet->dataLen;
        break;
    case HID_TRANS_GET_PROTOCOL:
        /* Set up the get protocol request */
        packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_GET_PROTOCOL;
        packet->headerLen = 1;
        break;
    case HID_TRANS_SET_PROTOCOL:
        /* Set up the set protocol request */
        packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_SET_PROTOCOL;
        packet->header[BT_PACKET_HEADER_LEN - 1] += Trans->parm.protocol;
        packet->headerLen = 1;
        break;
    case HID_TRANS_GET_IDLE_RATE:
        /* Set up the get idle request */
        packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_GET_IDLE_RATE;
        packet->headerLen = 1;
        break;
    case HID_TRANS_SET_IDLE_RATE:
        /* Set up the set idle request */
        packet->header[BT_PACKET_HEADER_LEN - 2] = HID_MSG_SET_IDLE_RATE;
        packet->header[BT_PACKET_HEADER_LEN - 1] = Trans->parm.idleRate;
        packet->headerLen = 2;
        break;
    case HID_TRANS_SET_REPORT_RSP:
    case HID_TRANS_SET_PROTOCOL_RSP:
    case HID_TRANS_SET_IDLE_RATE_RSP:
        /* Respond with a handshake */
        packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_HANDSHAKE;
        packet->header[BT_PACKET_HEADER_LEN - 1] += Trans->resultCode;
        packet->headerLen = 1;
        break;
    case HID_TRANS_GET_REPORT_RSP:
        /* Return the requested report */
        if (Trans->resultCode == HID_RESULT_SUCCESS) {
            if (Trans->flags & TRANS_FLAG_START) {
                packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_DATA;
                Trans->offset = 0;
	       Trans->flags &=~ TRANS_FLAG_START;
            } else {
                packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_DATC;
            }
            packet->header[BT_PACKET_HEADER_LEN - 1] += Trans->parm.report->reportType;
            packet->headerLen = 1;
            packet->data = Trans->parm.report->data + Trans->offset;
            packet->dataLen = min(Trans->parm.report->dataLen - Trans->offset, 
                                  L2CAP_GetTxMtu(Channel->ctrlCid) - 1);
            Trans->offset += packet->dataLen;
	 if (packet->dataLen != (L2CAP_GetTxMtu(Channel->ctrlCid) - 1)) {
                        /* There is no more to send */
                        Trans->flags |= TRANS_FLAG_SENT;
                    }
        } else {
            packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_HANDSHAKE;
            packet->header[BT_PACKET_HEADER_LEN - 1] += Trans->resultCode;
            packet->headerLen = 1;
        }
        break;
    case HID_TRANS_GET_PROTOCOL_RSP:
        /* Return the protocol */
        if (Trans->resultCode == HID_RESULT_SUCCESS) {
            packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_DATA;
            packet->header[BT_PACKET_HEADER_LEN - 1] += HID_REPORT_OTHER;
            packet->headerLen = 1;
            packet->data = &Trans->parm.protocol;
            packet->dataLen = 1;
        } else {
            packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_HANDSHAKE;
            packet->header[BT_PACKET_HEADER_LEN - 1] += Trans->resultCode;
            packet->headerLen = 1;
        }
        break;
    case HID_TRANS_GET_IDLE_RATE_RSP:
        /* Return the idle rate */
        if (Trans->resultCode == HID_RESULT_SUCCESS) {
            packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_DATA;
            packet->header[BT_PACKET_HEADER_LEN - 1] += HID_REPORT_OTHER;
            packet->headerLen = 1;
            packet->data = &Trans->parm.idleRate;
            packet->dataLen = 1;
        } else {
            packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_HANDSHAKE;
            packet->header[BT_PACKET_HEADER_LEN - 1] += Trans->resultCode;
            packet->headerLen = 1;
        }
        break;
    }

    /* Send the packet */
    kal_trace(BT_TRACE_G2_PROFILES,BEFORE_L2CAP_SEND_TRANSTYPExD ,Trans->type);
	
    packet->ulpContext = Trans;
    packet->flags = BTP_FLAG_INUSE;
    packet->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2CAP_SendData(Channel->ctrlCid, packet);

    return status;
}

/*---------------------------------------------------------------------------
 *            HidSendNextTransaction()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send the next transaction in the queue.
 *
 * Return:    Status of L2CAP send.
 */
BtStatus HidSendNextTransaction(HidChannel *Channel)
{
    BtStatus        status = BT_STATUS_SUCCESS;
    HidTransaction *trans;
    BtPacket       *packet;
    kal_trace(BT_TRACE_G2_PROFILES,HIDSENDNEXTTRANSACTION); 
    if (!IsListEmpty(&Channel->transQueue)) {
        status = BT_STATUS_PENDING;
        if (!(Channel->flags & CHNL_FLAG_TRANS_PENDING)) {
            /* Get the next transaction */
            trans = (HidTransaction *)GetHeadList(&Channel->transQueue);

#ifdef __BT_4_0_BLE__
            if (Channel->cmgrHandler.bdc->devType == BT_DEV_TYPE_LE)
            {
                status = HidSendLeTransPacket(Channel, trans);
                if (status == BT_STATUS_PENDING)
                {
                    Channel->flags |= CHNL_FLAG_TRANS_PENDING;
                }
            }
            else
#endif
            {
                /* Send as many packets as possible */
                while (!IsListEmpty(&HID(packetPool))) {
                    /* Get a packet */
                    packet = (BtPacket *)RemoveHeadList(&HID(packetPool));
                
                    /* Send it */
                    status = HidSendTransPacket(Channel, trans, packet);
                    if (status == BT_STATUS_PENDING) {
                        /* Packet was sent, see if there is more to send */
                        Channel->flags |= CHNL_FLAG_TRANS_PENDING;
                        if (packet->dataLen != 
                                         (L2CAP_GetTxMtu(Channel->ctrlCid) - 1)) {
                            /* There is no more to send */
                            break;
                        }
                    } else {
                        /* Transmit failed, fail the call */
                        trans->flags |= TRANS_FLAG_ERROR;
                        InsertTailList(&HID(packetPool), &packet->node);
                        break;
                    }
                }
            }
        }
    }
     
    return status;
}

/*---------------------------------------------------------------------------
 *            HidCompleteTransaction()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Complete a transaction and look for a new one to start.
 *
 * Return:    void
 */
void HidCompleteTransaction(HidChannel *Channel, HidTransaction *Trans,
                            BtStatus Status)
{
    HidTransaction *trans;
    BtStatus        status;
    kal_trace(BT_TRACE_G2_PROFILES,HIDCOMPLETETRANSACTION); 
    RemoveEntryList(&Trans->node);
    Channel->flags &= ~CHNL_FLAG_TRANS_PENDING;
    HidAppCallback(Channel, HIDEVENT_TRANSACTION_COMPLETE, 
                   Status, Trans, sizeof(HidTransaction));

    /* Check for a new transtacion */
    while (!IsListEmpty(&Channel->transQueue)) {
        status = HidSendNextTransaction(Channel);
        //Assert(status != BT_STATUS_SUCCESS);
	if(status == BT_STATUS_SUCCESS)
		return;
        if (status == BT_STATUS_PENDING) {
            /* New transaction started */
            break;
        } else {
            /* Transaction failed */
            trans = (HidTransaction *)RemoveHeadList(&Channel->transQueue);
            Channel->flags &= ~CHNL_FLAG_TRANS_PENDING;
            HidAppCallback(Channel, HIDEVENT_TRANSACTION_COMPLETE,
                           status, trans, sizeof(HidTransaction));
        }
    }
}

/*---------------------------------------------------------------------------
 *            HidSendIntrPacket()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send a interrupt packet.
 *
 * Return:    Status of L2CAP send.
 */
BtStatus HidSendIntrPacket(HidChannel *Channel, HidInterrupt *Intr,
                            BtPacket *packet)
{
    BtStatus status;

    /* Set up the report */
    if (Intr->flags & INTR_FLAG_START) {
        packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_DATA;
        Intr->offset = 0;
	Intr->flags &= ~INTR_FLAG_START;
    } else {
        packet->header[BT_PACKET_HEADER_LEN - 1] = HID_MSG_DATC;
    }
    packet->header[BT_PACKET_HEADER_LEN - 1] |= (Intr->reportType & 0x03);
    packet->headerLen = 1;
    packet->data = Intr->data + Intr->offset;
    packet->dataLen = min(Intr->dataLen - Intr->offset, 
                          L2CAP_GetTxMtu(Channel->intrCid) - 1);
    Intr->offset += packet->dataLen;

	if (packet->dataLen != (L2CAP_GetTxMtu(Channel->intrCid) - 1)) {
	/* There is no more to send */
		Intr->flags |= INTR_FLAG_SENT;
	}

    /* Send the packet */
    packet->ulpContext = Intr;
    packet->flags = BTP_FLAG_INUSE;
    packet->priority_type = BTP_FLAG_HIGH_PRIPORTY;
    status = L2CAP_SendData(Channel->intrCid, packet);
    kal_trace(BT_TRACE_G2_PROFILES,L2CAP_HID_TX_STATUSx02X ,status);
    return status;
}

/*---------------------------------------------------------------------------
 *            HidCompleteInterrupt()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Complete an interrupt and look for a new one to start.
 *
 * Return:    void
 */
void HidCompleteInterrupt(HidChannel *Channel, HidInterrupt *Intr,
                          BtStatus Status)
{
    HidInterrupt *intr;
    BtStatus        status;
	bt_prompt_trace(MOD_BT,"[HID]HidCompleteInterrupt");

    RemoveEntryList(&Intr->node);
    
    Channel->flags &= ~CHNL_FLAG_INTR_PENDING;
    HidAppCallback(Channel, HIDEVENT_INTERRUPT_COMPLETE, 
                   Status, Intr, sizeof(HidInterrupt));

    /* Check for a new interrupt */
    while (!IsListEmpty(&Channel->intrQueue)) {
        status = HidSendNextInterrupt(Channel);
       //Assert(status != BT_STATUS_SUCCESS);
		if(status == BT_STATUS_SUCCESS){
			bt_prompt_trace(MOD_BT,"[HID]status == BT_STATUS_SUCCESS");
			return;
			}
        if (status == BT_STATUS_PENDING) {
            /* New interrupt started */
            break;
        } else {
            /* Interrupt failed */
            intr = (HidInterrupt *)RemoveHeadList(&Channel->intrQueue);
            Channel->flags &= ~CHNL_FLAG_INTR_PENDING;
            HidAppCallback(Channel, HIDEVENT_INTERRUPT_COMPLETE,
                           status, intr, sizeof(HidInterrupt));
        }
    }
}

/*---------------------------------------------------------------------------
 *            HidSendNextInterrupt()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send the next interrupt in the queue.
 *
 * Return:    Status of L2CAP send.
 */
BtStatus HidSendNextInterrupt(HidChannel *Channel)
{
    BtStatus        status = BT_STATUS_SUCCESS;
    HidInterrupt   *intr;
    BtPacket       *packet;

    if (!IsListEmpty(&Channel->intrQueue)) {
        status = BT_STATUS_PENDING;
        if (!(Channel->flags & CHNL_FLAG_INTR_PENDING)) {
            /* Get the next interrupt */
            intr = (HidInterrupt *)GetHeadList(&Channel->intrQueue);

#ifdef __BT_4_0_BLE__
            if (Channel->cmgrHandler.bdc->devType == BT_DEV_TYPE_LE)
            {
                status = HidSendLeIntrPacket(Channel, intr);
                if (status == BT_STATUS_PENDING)
                {
                    Channel->flags |= CHNL_FLAG_TRANS_PENDING;
                }
            }
            else
#endif
            {

                /* Send as many packets as possible */
                while (!IsListEmpty(&HID(packetPool))) {
                    /* Get a packet */
                    packet = (BtPacket *)RemoveHeadList(&HID(packetPool));
            
                    /* Send it */
                    status = HidSendIntrPacket(Channel, intr, packet);
                    if (status == BT_STATUS_PENDING) {
                        /* Packet was sent, see if there is more to send */
                        Channel->flags |= CHNL_FLAG_INTR_PENDING;
                        if (packet->dataLen != 
                            (L2CAP_GetTxMtu(Channel->intrCid) - 1)) {
                            /* There is no more to send */
                            break;
                        }
                    } else {
                        /* Transmit failed, fail the call */
                        intr->flags |= INTR_FLAG_ERROR;
                        InsertTailList(&HID(packetPool), &packet->node);
                        break;
                    }
                }
            }
        }
    }
     
    return status;
}

void HidShowBytes(void *s, int n)
{
    U8 *start  =  (U8*)s;
    int i = 0;
    
    bt_prompt_trace(MOD_BT,"[HID][OFFSET] ADDRESS: VALUE\n\n");

    for  (  i  =   0 ; i  <  n; i ++ )
    {
        bt_prompt_trace(MOD_BT, "[HID][%4d] %8x: %2x\n ",i,start+i,*(start+i));

        if  ((i  +   1 )  %   4   ==   0 )
        {
            bt_prompt_trace(MOD_BT, " ----------------------\n " );
        } 
    } // for 
}


#endif
