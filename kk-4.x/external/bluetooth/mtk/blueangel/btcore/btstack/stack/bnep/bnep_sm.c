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
 *     $Workfile:bnep_sm.c$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: This file contains the state machine for the BNEP protocol.
 *
 * Created:     Feb 28, 2001
 * 
 * Copyright 1999-2004 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
#ifdef __BT_BNEP__

#include <sys/bnepi.h>
#include <btalloc.h>
#include <sys/debug.h>

static void HandleDataInd(BnepCallbackParms *parms, L2capCallbackParms *info);
static void HandlePacketHandled(BnepCallbackParms *parms, L2capCallbackParms *info);
static U8 *HandleExtHeader(BnepPanu *panu, BnepCallbackParms *parms);

/*---------------------------------------------------------------------------
 *            BnepAppCallBack()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Call the application with the specified event.
 *
 * Return:    
 */
void BnepAppCallBack(BnepCallbackParms *parms, U16 status, BnepEvent event)
{
    Assert(BNEP(callback) != 0);

    parms->status = status;
    parms->event = event;

    BNEP(callback)(parms);
}

/*---------------------------------------------------------------------------
 *            L2BnepCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Callback routine for L2CAP connections.
 *
 * Return:    
 */
void L2BnepCallback(U16 l2ChannelId, L2capCallbackParms *info)
{
    BnepCallbackParms   parms;

    //Report(("BNEP: PANU connection %s\n", pL2CAP_Event(info->event)));

    parms.panu = FindBnepPanu(l2ChannelId);
    if (parms.panu != 0) {
        switch (info->event) {
        case L2EVENT_CONNECT_IND:
            /* this should never happen */
            Assert(0);
            break;
        case L2EVENT_CONNECTED:
            parms.panu->state = BNEP_STATE_OPEN;
            BnepAppCallBack(&parms, BT_STATUS_SUCCESS, BNEPEVENT_OPEN);
            break;
        case L2EVENT_DISCON_IND:
            parms.panu->state = BNEP_STATE_CLOSE_IND;
            break;
        case L2EVENT_DISCONNECTED:
            /* Remove the panu from BNEP context */
            DeInitBnepPanu(parms.panu);
            BnepAppCallBack(&parms, info->status, BNEPEVENT_CLOSED);
            break;
        case L2EVENT_DATA_IND:
            HandleDataInd(&parms, info);
            break;
        case L2EVENT_PACKET_HANDLED:
            HandlePacketHandled(&parms, info);
            break;
        default:
            Report(("BNEP: Unexpected L2EVENT %d on channel %d\n", info->event, l2ChannelId));
            break;
        }
        return;
    }

    /* Unknown panu, check if connect indication */
    switch (info->event) {
    case L2EVENT_CONNECT_IND:
        /* Request from a new device. */
        if (BNEP(callback) == 0) {
            L2CAP_ConnectRsp(l2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
            return;
        }
        parms.l2ChannelId = l2ChannelId;
        parms.remDev = info->aclLink;
        BnepAppCallBack(&parms, BT_STATUS_SUCCESS, BNEPEVENT_OPEN_IND);
        break;
    case L2EVENT_DISCONNECTED:
        /* Occurs when connection is not accepted */
        break;
    default:
        /* not a known panu, report error */
        Report(("BNEP: Unexpected L2EVENT %d on channel %d\n", info->event, l2ChannelId));
        /* Assert(0); */
        break;
    }
}

/*---------------------------------------------------------------------------
 *            HandleDataInd()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process BNEP extension headers.
 *
 * Return:    
 */
static void HandleDataInd(BnepCallbackParms *parms, L2capCallbackParms *info)
{
    U8         *p;
    EvmTimer   *timer;
    U8 ctrlPktIdx = 0;

    parms->pkt = info->ptrs.data;
    parms->type = parms->pkt[0];
    p = parms->pkt;
    parms->rxPktLen = info->dataLen;

    Report(("[BNEP]HandleDataInd+++\n"));
    Report(("[BNEP]HandleDataInd BNEP type[%x]\n",parms->type ));

	
    switch (parms->type & ~BNEP_E_BIT) {
    case BNEP_CONTROL:
        switch (parms->pkt[BnepControlPkt_hdr_controlType]) {
        case BNEP_SETUP_CONNECTION_REQUEST_MSG:
            /* if the second connection request received from the same peer, cancel previous timeout timer */
            timer = BnepFindPanuTimer(parms->panu, BNEP_SETUP_CONNECTION_RESPONSE_MSG);			
            if (timer != 0) {
                EVM_CancelTimer(timer);
                timer->func = 0;
                timer->context = 0;
            }		
        case BNEP_FILTER_NET_TYPE_SET_MSG:
        case BNEP_FILTER_MULTI_ADDR_SET_MSG:
            Report(("BNEP: Received Control Packet\n"));
            break;
        case BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD:
        case BNEP_SETUP_CONNECTION_RESPONSE_MSG:
        case BNEP_FILTER_NET_TYPE_RESPONSE_MSG:
        case BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG:
            Report(("BNEP: Received Control Response Packet\n"));
            /* response received, cancel timeout timer */
            timer = BnepFindPanuTimer(parms->panu,
                        parms->pkt[BnepControlPkt_hdr_controlType]);
            if (timer != 0) {
                EVM_CancelTimer(timer);
                /* clear the timer func so it can be used again */
                timer->func = 0;
                timer->context = 0;
                //20110504 Paul
                ctrlPktIdx = BnepFindRestransmissionCtrlPktIdx(&parms->panu, timer);
		  if (ctrlPktIdx != BNEP_NUM_TIMERS)
		  {
                    memset( (void*)(parms->panu->controlPkt + ctrlPktIdx), 0, sizeof(BtPacket));
	             parms->panu->retransmission[ctrlPktIdx] = 0;
            	  }
                //20110504 Paul end				
            }
            break;
        default:
            /* Unknown control packet type. We will still indicate this
             * packet to the application so it can respond with
             * BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD */
            break;
        }	
        break;
#if BNEP_ETHERNET_EMULATION == XA_ENABLED
    case BNEP_GENERAL_ETHERNET:
        Report(("BNEP: Received Gen'rl Ether Packet\n"));
        /* strip off the BNEP header */
        parms->pkt = p + BNEP_TYPE_SIZE;
        parms->rxPktLen -= BNEP_TYPE_SIZE;
        break;
    case BNEP_COMPRESSED_ETHERNET:
        Report(("BNEP: Received Compressed Ether Packet\n"));
        /* Add on an Ethernet header */
        parms->pkt = p - (BNEP_ADDR_SIZE * 2) + BNEP_TYPE_SIZE;
        parms->rxPktLen += BNEP_ADDR_SIZE * 2 - BNEP_TYPE_SIZE;
        /* restore addresses */
        OS_MemCopy(parms->pkt + EtherPacket_hdr_dest_addr,
                                        (U8 *)&BNEP(bnepAddr), BNEP_ADDR_SIZE);
        OS_MemCopy(parms->pkt + EtherPacket_hdr_source_addr,
                                        (U8 *)&parms->panu->bnepAddr, BNEP_ADDR_SIZE);
        /* set packet type */
        parms->type = BNEP_GENERAL_ETHERNET | (parms->type & BNEP_E_BIT);
        break;
    case BNEP_COMPRESSED_ETHERNET_SOURCE_ONLY:
        Report(("BNEP: Received Comp Src Ether Packet\n"));
        /* Add on an Ethernet header */
        parms->pkt = p - BNEP_ADDR_SIZE + BNEP_TYPE_SIZE;
        parms->rxPktLen += BNEP_ADDR_SIZE - BNEP_TYPE_SIZE;
        /* restore the destination address */
        OS_MemCopy(parms->pkt + EtherPacket_hdr_dest_addr,
                                        (U8 *)&BNEP(bnepAddr), BNEP_ADDR_SIZE);
        /* set packet type */
        parms->type = BNEP_GENERAL_ETHERNET | (parms->type & BNEP_E_BIT);
        break;
    case BNEP_COMPRESSED_ETHERNET_DEST_ONLY:
        Report(("BNEP: Received Comp Dest Ether Packet\n"));
        /* Add on an Ethernet header */
        parms->pkt = p - BNEP_ADDR_SIZE + BNEP_TYPE_SIZE;
        parms->rxPktLen += BNEP_ADDR_SIZE - BNEP_TYPE_SIZE;
        /* restore the source address (1st move dest addr) */
        OS_MemCopy(parms->pkt + EtherPacket_hdr_dest_addr,
              parms->pkt + EtherPacket_hdr_source_addr, BNEP_ADDR_SIZE);
        OS_MemCopy(parms->pkt + EtherPacket_hdr_source_addr,
                                        (U8 *)&parms->panu->bnepAddr, BNEP_ADDR_SIZE);
        /* set packet type */
        parms->type = BNEP_GENERAL_ETHERNET | (parms->type & BNEP_E_BIT);
        break;
#else /* BNEP_ETHERNET_EMULATION == XA_DISABLED */
    case BNEP_GENERAL_ETHERNET:
        Report(("BNEP: Received Gen'rl Ether Packet\n"));
        break;
    case BNEP_COMPRESSED_ETHERNET:
        Report(("BNEP: Received Compressed Ether Packet\n"));
        break;
    case BNEP_COMPRESSED_ETHERNET_SOURCE_ONLY:
        Report(("BNEP: Received Comp Src Ether Packet\n"));
        break;
    case BNEP_COMPRESSED_ETHERNET_DEST_ONLY:
        Report(("BNEP: Received Comp Dest Ether Packet\n"));
        break;
#endif /* BNEP_ETHERNET_EMULATION == XA_ENABLED */
    default:
        /* Unknown BNEP type field. No response required, so return. */
        return;
        break;
    }

    if (parms->type & BNEP_E_BIT) {
#if BNEP_ETHERNET_EMULATION == XA_ENABLED
        /* parse out the extension headers and send separately */

        Report(("[BNEP]HandleDataInd  HandleExtHeader\n"));
        parms->pkt = HandleExtHeader(parms->panu, parms);
        
        /* clear the E bit and send it if anything left */
        parms->type &= ~BNEP_E_BIT;
        if (parms->pkt) {
            if (parms->type == BNEP_CONTROL) {
                parms->pkt[0] &= ~BNEP_E_BIT;
//Paul 20110512
                if (parms->pkt[1] <=  BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG) {
                    Report(("[BNEP]HandleDataInd Control packet with entension header end---\n"));
                    return;
                }					
//Paul 20110512 end
            }
        }
        else return;
#else
        /* check for control response messages */
        HandleExtHeader(parms->panu, parms);
#endif /* BNEP_ETHERNET_EMULATION == XA_ENABLED */
    }
    Report(("[BNEP]HandleDataInd BnepAppCallBack BNEPEVENT_PACKET_IND---\n"));
    BnepAppCallBack(parms, BT_STATUS_SUCCESS, BNEPEVENT_PACKET_IND);
}

/*---------------------------------------------------------------------------
 *            HandleExtHeader()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Process BNEP extension headers.
 *
 * Return:    
 */
static U8 *HandleExtHeader(BnepPanu *panu, BnepCallbackParms *pParms)
{
    U8                 *p, savedHdr[ETHER_HDR_SIZE];
    U8                 *e;
    BOOL                eHeader, lastHeader;
    BnepCallbackParms   parms;
    EvmTimer           *timer;
#if BNEP_ETHERNET_EMULATION == XA_ENABLED
    U8                  extHdr[BNEP_EXT_HDR_SIZE + 255];
#endif /* BNEP_ETHERNET_EMULATION == XA_ENABLED */
    U16                 headerLen;

    parms.panu = panu;
    parms.pkt = pParms->pkt;

    Report(("[BNEP]HandleExtHeader+++\n"));
    Report(("[BNEP]HandleExtHeader BNEP type[%x]\n",pParms->type));
    if ((pParms->type & ~BNEP_E_BIT) == BNEP_CONTROL) {
        switch (parms.pkt[BnepControlPkt_hdr_controlType]) {
        case BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD:
            /* this is an invalid extension header */
            parms.pkt[0] &= ~BNEP_E_BIT;
            return parms.pkt;
            break;
        case BNEP_SETUP_CONNECTION_REQUEST_MSG:
            headerLen = BNEP_CONTROL_HDR_SIZE + BNEP_UUID_LENGTH_SIZE +
                parms.pkt[BnepControlPkt_data_length]*2; 
            break;
        case BNEP_FILTER_NET_TYPE_SET_MSG:
        case BNEP_FILTER_MULTI_ADDR_SET_MSG:
            headerLen = BNEP_CONTROL_HDR_SIZE + BNEP_LIST_LENGTH_SIZE +
                BEtoHost16(&parms.pkt[BnepControlPkt_data_length]); 
            break;
        case BNEP_SETUP_CONNECTION_RESPONSE_MSG:
        case BNEP_FILTER_NET_TYPE_RESPONSE_MSG:
        case BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG:
            headerLen = BNEP_CONTROL_HDR_SIZE + sizeof(BnepSetupResponseMsg);
            Report(("BNEP: Control Response\n"));
            break;
        default:
            Report(("BNEP: HandleExtHeader received unknown control type %u\n",
                parms.pkt[BnepControlPkt_hdr_controlType]));
            return parms.pkt;
            break;
        }
//Paul 20110512 
	 pParms->pkt[0] &= ~BNEP_E_BIT;
	 pParms->type =	pParms->pkt[0];
        Report(("[BNEP]HandleDataInd BnepAppCallBack BNEPEVENT_PACKET_IND report known control packet first\n"));
        BnepAppCallBack(pParms, BT_STATUS_SUCCESS, BNEPEVENT_PACKET_IND);				
//Paul 20110512 end			
    }
    else {
        headerLen = ETHER_HDR_SIZE;
        OS_MemCopy(savedHdr, pParms->pkt, ETHER_HDR_SIZE);
    }
	
    e = parms.pkt + headerLen;

    /* set up the extension header indication flags */
    if (e[0] & BNEP_E_BIT) {
        eHeader = TRUE;
        lastHeader = FALSE;
    }
    else {
        eHeader = FALSE;
        lastHeader = TRUE;
    }

    while (eHeader || lastHeader) {
        if ((e[0] & ~BNEP_E_BIT) == BNEP_EXTENSION_CONTROL) {
            switch (e[BnepFilterExtHdr_filterType]) {
            case BNEP_SETUP_CONNECTION_REQUEST_MSG:
            case BNEP_FILTER_NET_TYPE_SET_MSG:
            case BNEP_FILTER_MULTI_ADDR_SET_MSG:
                Report(("BNEP: Received Control Packet\n"));
                break;
            case BNEP_SETUP_CONNECTION_RESPONSE_MSG:
            case BNEP_FILTER_NET_TYPE_RESPONSE_MSG:
            case BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG:
                Report(("BNEP: Received Control Response Packet\n"));
                /* response received, cancel timeout timer */
                timer = BnepFindPanuTimer(parms.panu,
                            e[BnepFilterExtHdr_filterType]);
                if (timer != 0) {
                    EVM_CancelTimer(timer);
                    /* clear the timer func so it can be used again */
                    timer->func = 0;
                    timer->context = 0;
                }
                break;
            default:
                break;
            }
        }
	
#if BNEP_ETHERNET_EMULATION == XA_ENABLED
        /* parse out an extension header and send it */
        OS_MemCopy(extHdr, e, (U16)(BNEP_EXT_HDR_SIZE + e[BnepExtensionHeader_hdrDataLen]));
        parms.pkt = extHdr;
        parms.type = extHdr[0];
        BnepAppCallBack(&parms, BT_STATUS_SUCCESS, BNEPEVENT_EXTENSION_HEADER_IND);
#endif /* BNEP_ETHERNET_EMULATION == XA_ENABLED */

        /* jump to the next header */
        e = e + BNEP_EXT_HDR_SIZE + e[BnepExtensionHeader_hdrDataLen];

        /* check the next header for E bit */
        if (lastHeader) {
            eHeader = lastHeader = FALSE;
        }
        else if (e[0] & BNEP_E_BIT) {
            eHeader = TRUE;
        }
        else {
            lastHeader = TRUE;
        }
    }

#if BNEP_ETHERNET_EMULATION == XA_ENABLED
    Report(("[BNEP]HandleExtHeader BNEP_ETHERNET_EMULATION == XA_ENABLED\n"));
    /* get the header size */
    switch (pParms->type & ~BNEP_E_BIT) {
    case BNEP_CONTROL:
        /* control packets don't have a payload */
        p = pParms->pkt;
        pParms->rxPktLen = headerLen;
        break;
    default:
	
        p = e;
    Report(("[BNEP]HandleExtHeader get rid of extHdr\n")); 
        // check if there is anything left in the packet 
        if (pParms->pkt + pParms->rxPktLen <= p) {
            return 0;
        }

        // move the Ethernet or BNEP header up to the packet payload 
        p -= headerLen;
        OS_MemCopy(p, savedHdr, headerLen);

        // adjust the length 
        pParms->rxPktLen -= p - pParms->pkt;
        break;
    }
#else
    Report(("[BNEP]HandleExtHeader BNEP_ETHERNET_EMULATION != XA_ENABLED\n"));
    p = pParms->pkt;
#endif /* BNEP_ETHERNET_EMULATION == XA_ENABLED */
    Report(("[BNEP]HandleExtHeader---\n"));
    /* return a pointer to the eHeader stripped BNEP Packet */
    return p;
}

#define L2CAP_CONS_HEADER_LEN 4 /* get this from l2capi.h */
static void HandlePacketHandled(BnepCallbackParms *parms, L2capCallbackParms *info)
{
    S8 offset = info->ptrs.packet->headerLen - L2CAP_CONS_HEADER_LEN;

//Paul 20110926	
    U16 status = BT_STATUS_SUCCESS;

    if ( offset != BNEP_TYPE_SIZE &&
          offset != BNEP_GNRL_HDR_SIZE && 
          offset != BNEP_GNRL_HDR_SIZE - BNEP_ADDR_SIZE &&
          offset != BNEP_GNRL_HDR_SIZE - BNEP_ADDR_SIZE*2 )
    {
        offset = info->ptrs.packet->headerLen;
        status = BT_STATUS_FAILED;
    }	
//    if ( 0 >= offset  || offset > BT_PACKET_HEADER_LEN )
//    {
//        BnepAppCallBack(parms, BT_STATUS_FAILED, BNEPEVENT_PACKET_HANDLED);    
//        return;
//    }	
//Paul 20110926 end



	
    parms->pktExtHdr = NULL;   	
    parms->txPkt = info->ptrs.packet;
    //parms->type = info->ptrs.packet->header[BT_PACKET_HEADER_LEN - 
    //                    (info->ptrs.packet->headerLen - L2CAP_CONS_HEADER_LEN)];
    parms->type = info->ptrs.packet->header[BT_PACKET_HEADER_LEN - offset];

    /* set up the handled packet pointer */
    if (parms->txPkt->ulpContext == BNEP_SendEtherPkt) {
//Paul 20110510 
/*		
        if (parms->type & BNEP_E_BIT) {
            parms->pkt = parms->txPkt->tail - ETHER_HDR_SIZE;			
        }
        else {
            parms->pkt = parms->txPkt->data - ETHER_HDR_SIZE;
        }
        parms->type &= ~BNEP_E_BIT;
*/
        if (parms->type & BNEP_E_BIT) {
            parms->pkt = parms->txPkt->tail - ETHER_HDR_SIZE;
            parms->pktExtHdr = parms->txPkt->data;				
        }
        else {
            parms->pkt = parms->txPkt->data - ETHER_HDR_SIZE;
        }
        parms->type &= ~BNEP_E_BIT;		
//Paul 20110510 end		
    }
    else if (parms->txPkt->ulpContext == BNEP_Control) {
        parms->pkt = parms->txPkt->data - BNEP_TYPE_SIZE;
    }
    else {
        parms->pkt = parms->txPkt->data - BNEP_TYPE_SIZE;
    }

    //BnepAppCallBack(parms, BT_STATUS_SUCCESS, BNEPEVENT_PACKET_HANDLED);
    if ( BT_STATUS_FAILED == status ) {
        BnepAppCallBack(parms, status, BNEPEVENT_PACKET_HANDLED);		
    } 
    else
    {
        BnepAppCallBack(parms, info->status, BNEPEVENT_PACKET_HANDLED);
    }
	

}

#endif /* __BT_BNEP__ */

