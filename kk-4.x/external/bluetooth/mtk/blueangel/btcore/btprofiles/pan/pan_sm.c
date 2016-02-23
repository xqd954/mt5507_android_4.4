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

/***************************************************************************
 *
 * File:
 *     $Workfile:pan_sm.c$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: Source file for bnep sample application.
 *
 * Created:     February 2, 2003
 *
 * Copyright 1999-2002 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
#ifdef __BT_PAN_PROFILE__

#include <l2cap.h>
#include <sys/pani.h>
#include <bnep.h>
#include <sys/debug.h>


static const U8 BtBaseUUID[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
};


/* Internal Functions */
static void PanHandlePacket(PanCallbackParms *parms, BnepCallbackParms *Info);


/* ---------------------------------------------------------------------------
 *                      PAN Connection Manager Event handler
 * ---------------------------------------------------------------------------
 */
void PanCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status)
{
    PanUser *pan = ContainingRecord(Handler, PanUser, cmgrHandler);
    kal_trace(BT_TRACE_G2_PROFILES, PAN_CMGRCALLBACK_EVENT_STATUS, Event, Status);
    switch (Event)
    {
    case CMEVENT_DATA_LINK_CON_CNF:
        if (pan == NULL)
        {
            kal_trace(BT_TRACE_G2_PROFILES, PAN_NOT_FOR_THE_BD_ADDR);
            return;			
        }

        if (pan->cmgrHandler.bdc == 0)
        {
            kal_trace(BT_TRACE_G2_PROFILES, PAN_CMGRCALLBACK_BDADDR0);
            return;			
        }

        if (Status != BT_STATUS_SUCCESS)
        {
            PanClearConnection(pan, Status);
            return;			
        }
        else
        {
            if (pan->state == PAN_OPENING)
            {
                BtStatus sdp_status;
                sdp_status = PanStartServiceQuery(pan, pan->dstService);
                if (sdp_status != BT_STATUS_SUCCESS && sdp_status != BT_STATUS_PENDING)
                {
                    PanClearConnection(pan, sdp_status);
                }
            }
        }
        break;	

    default:
        break; 		
    }
}

/* ---------------------------------------------------------------------------
 *                      PAN BNEP Event handler
 * ---------------------------------------------------------------------------
 */
void PanBnepCallback(BnepCallbackParms *Info)
{
    PanCallbackParms    parms;
    U16                 offset;
    U16                 context;
    U8                 *cx;

    parms.pan = PanFindPanuser(Info->panu);
    kal_trace(BT_TRACE_G2_PROFILES, PAN_BNEPCALLBACK, parms.pan, Info->panu);
    kal_trace(BT_TRACE_G2_PROFILES, PAN_BNEPCALLBACK_EVENT, Info->event);

    parms.status = BT_STATUS_SUCCESS;
    switch (Info->event) {
    case BNEPEVENT_OPEN:
        Report(("Pan: Received BNEPEVENT_OPEN event\n"));
        if (parms.pan == NULL)
        {
            BNEP_Close(Info->panu);
            break;			
        }

        Report(("   L2CAP Assigned Channel ID %04x.\n", Info->panu->l2ChannelId));
        if (parms.pan->state == PAN_OPENING) {
            /* Tell the PAN application to send a setup connection message */
            parms.event = PANEVENT_SETUP_CONNECTION;
            PAN(callback)(&parms);
        }
        /* if we are a listening PANU, GN or NAP, indicate the open only 
        when receive the PAN connection setup control response */
        break;

    case BNEPEVENT_OPEN_IND:
        Report(("Pan: Received BNEPEVENT_OPEN_IND event\n"));
        Report(("   on L2CAP Channel %04x.\n", Info->l2ChannelId));
        //Assert(parms.pan != NULL);

        if (parms.pan == NULL)
        {
            parms.pan = PanNewPanuser();
            if (parms.pan == NULL)
            {
                /* indicate no resources with a null BnepPanu pointer */
                BNEP_Accept(0, Info->l2ChannelId, Info->remDev);
                return;
            }
            parms.pan->state = PAN_OPEN_IND;
            BNEP_Accept(&parms.pan->bnepPanu, Info->l2ChannelId, Info->remDev);
            PAN_SetRemoteMacAddr(parms.pan, (U8 *)&parms.pan->bnepPanu.bnepAddr);
        }
        break;

    case BNEPEVENT_CLOSED:
        Report(("Pan: Received BNEPEVENT_CLOSED event with status %d\n", Info->status));
        PanClearConnection(parms.pan, Info->status);
        break;

    case BNEPEVENT_PACKET_IND:
        parms.pkt = Info->pkt;
        PanHandlePacket(&parms, Info);
        break;

    case BNEPEVENT_PACKET_HANDLED:
        Report(("Pan: received BNEPEVENT_PACKET_HANDLED event %x\n", Info->pkt));
        /* return the PanBtPacket to the pool */
        context = (U16)*((U8 *)Info->txPkt - 4); // 4 bytes alignment
        cx = (U8 *)Info->txPkt - 4;
        *((U16 *)(cx)) = PANBTPKT_FREE;
        if (context == PANBTPKT_INUSE_INT) 
        {
            Report(("Pan packet handled, pkt: %x", Info->pkt));
            pan_free(Info->pkt);
            //pan_free(Info->pktExtHdr);			
            break;
        }

        /* Pass the data packet pointer up to the App */
        parms.event = PANEVENT_DATA_HANDLED;
        switch (Info->pkt[0]) {
        case BNEP_CONTROL:
            offset = 0;
            break;

        case BNEP_GENERAL_ETHERNET:
            offset = BNEP_GNRL_HDR_SIZE - 1;
            break;

        case BNEP_COMPRESSED_ETHERNET:
            offset = BNEP_ADDR_SIZE*2;
            break;

        case BNEP_COMPRESSED_ETHERNET_SOURCE_ONLY:
        case BNEP_COMPRESSED_ETHERNET_DEST_ONLY:
            offset = BNEP_ADDR_SIZE;
            break;

        default:
            /* Packet type unknown */
            return;
        }
        parms.pkt = Info->pkt - offset;	// return data pointer from panPkt->header which is malloc by upper layer
        parms.status = Info->status;
        PAN(callback)(&parms);
        break;

    case BNEPEVENT_CONTROL_TIMEOUT:
        Report(("Pan: Received BNEPEVENT_CONTROL_TIMEOUT event\n"));
        break;

    case BNEPEVENT_EXTENSION_HEADER_IND:
        if ((Info->type & ~BNEP_E_BIT) == BNEP_EXTENSION_CONTROL) 
        {
            Report(("PAN: Received BNEP Control Extension Header type %d\n", Info->pkt[2]));
            parms.rxPktLen = (U16)Info->pkt[1];
            parms.pkt = Info->pkt + 2;	// control type
            //parms.pkt[0] = BNEP_CONTROL;
            parms.event = PANEVENT_CONTROL_IND;
            PAN(callback)(&parms);
        }
        else 
        {
            PanExtHdrNode *extHdrNode;
            Report(("Pan: Received Unknown Extension Header %d with status %d\n",
                                                Info->pkt[0], Info->status));
            if (PAN(service) == PANSVC_PANU)
                break;				

            extHdrNode = (PanExtHdrNode *)pan_malloc(sizeof(PanExtHdrNode));
            extHdrNode->exthdr = (U8 *)pan_malloc(Info->pkt[1] + BNEP_EXT_HDR_SIZE);
            OS_MemCopy(extHdrNode->exthdr, Info->pkt, Info->pkt[1] + BNEP_EXT_HDR_SIZE);
            InsertTailList(&parms.pan->exthdrList, &extHdrNode->node);
        }
        break;

    default:
        Report(("Pan: Received Unknown Event %d with status %d\n", Info->event,
                                                            Info->status));
        break;
   }
}

/* ---------------------------------------------------------------------------
 *                      BNEP Packet handler
 * ---------------------------------------------------------------------------
 */
static void PanHandlePacket(PanCallbackParms *parms, BnepCallbackParms *Info)
{
    //ArpHeader  *arpHdrIn;
    //IpHeader   *ipHdr;
    //UdpHeader  *udpHdr;
    //DhcpHeader *dhcpHdr;
    //IcmpHeader *icmpHdr;
    //U16         hdrType, offset;
    U16	listLength;
    U8		*filterPkt;	
    U8         *uuid, i;
    U32         service;
    //BNEP_ADDR  *macAddr;
    //U32         srcIP;
    //U32         dstIP;
    PanUser    *dstPan; 
    //PanUser    *srcPan;
    U8		*exthdr = NULL;
    U16 exthdrLen = 0;			
    BtStatus status;

    parms->event = PANEVENT_DATA_IND;

    switch (Info->type) {

    case BNEP_GENERAL_ETHERNET:
        Report(("Pan: Rx'd BNEP_GENERAL_ETHERNET packet length %d\n",
                        Info->rxPktLen));


        if (PAN(service) == PANSVC_PANU)
            goto eth_exit;			

        /* extension header handling */ 
        {
            PanExtHdrNode *exthdrNode;
            exthdrNode = (PanExtHdrNode *)GetHeadList(&parms->pan->exthdrList);	
            while ((ListEntry *)exthdrNode != &parms->pan->exthdrList)
            {
                exthdrLen += (U16)exthdrNode->exthdr[1] + BNEP_EXT_HDR_SIZE;
                exthdrNode = (PanExtHdrNode *)GetNextNode(&exthdrNode->node); 
            }
            Report(("Pan handle packet, extension header len: %d", exthdrLen));

            if (exthdrLen > 0)
            {
                U8 *exthdr_next;
                exthdr = (U8 *)pan_malloc(exthdrLen);
                exthdr_next = exthdr;				
                while (!IsListEmpty(&parms->pan->exthdrList)) 
                {
                    exthdrNode = (PanExtHdrNode *)RemoveHeadList(&parms->pan->exthdrList);
                    OS_MemCopy(exthdr_next, exthdrNode->exthdr, exthdrNode->exthdr[1] + BNEP_EXT_HDR_SIZE);
                    exthdr_next += exthdrNode->exthdr[1] + BNEP_EXT_HDR_SIZE;

                    pan_free(exthdrNode->exthdr);
                    pan_free(exthdrNode);					
                }
            }
        }


        dstPan = PanFindMacAddr(Info->pkt);
        //srcPan = PanFindMacAddr(Info->pkt + 6);

        if ((dstPan == PANUSER_BROADCAST) ||
	    (dstPan == PANUSER_MULTICAST))
        {
            status = PAN_ForwardPkt(Info->pkt, Info->rxPktLen, exthdr, exthdrLen);
            if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING)
            {
                // need to queue and send later?
            }
            // pass up a copy of the packet to its upper protocol layer			
        }
        else if ((dstPan == NULL) || 
		   (dstPan == PANUSER_NULL))
        {
            // drop the packet
            Report(("Pan drop the packet directly..."));
            pan_free(exthdr);
            return;
        }
        else if (dstPan != PANUSER_SELF)
        {
            Report(("Pan forward ethernet packet..."));
            status = PAN_ForwardPkt(Info->pkt, Info->rxPktLen, exthdr, exthdrLen);
            if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING)
            {
                // need to queue and send later?
            }
            pan_free(exthdr);
            return;			
        }

        pan_free(exthdr);

eth_exit:		
        parms->event = PANEVENT_DATA_IND;
        parms->pkt = Info->pkt;
        parms->type = PANPKT_IP;
        parms->rxPktLen = Info->rxPktLen;
        PAN(callback)(parms);   
        break;

#if 0
        switch (hdrType) {
        case PAN_EHTERTYPE_ARP:
            parms->pkt = Info->pkt + ETHER_HDR_SIZE + offset;
            arpHdrIn = (ArpHeader *)parms->pkt;

            srcIP = BEtoHost32(arpHdrIn->srcIP);
            dstIP = BEtoHost32(arpHdrIn->dstIP);
            
            if (BEtoHost16(arpHdrIn->op) == ARP_REQUEST) {
                /* Handle the request internally */
                Report(("PAN: Received ARP Request\n"));

                /* Is the ARP for the net? */ 
                macAddr = 0;
                /*if (PAN(service) != PANSVC_PANU) {
                    macAddr = PAN_SendArp(parms->pan, arpHdrIn->dstIP);
                    srcIP = arpHdrIn->dstIP;
                }*/

                /* or, Is the ARP for me? */ 
                dstPan = PanFindIpAddr(dstIP);
                bt_prompt_trace(MOD_BT, "pan arp request local ip: %x", PAN(ipAddr));
                if (!macAddr && (dstPan == PANUSER_SELF)) {
                    macAddr = &PAN(macAddr);
                    srcIP = PAN(ipAddr);
                }

                /* Prevent Address test ARPs as they invalidate my Router */
                else if (dstPan == parms->pan) {
                    return;
                }

                /* Is this a gratuitous ARP? */
                else if (dstIP == srcIP &&
                    PanFindMacAddr(arpHdrIn->srcMac) == parms->pan) {
                    /* yes, is the address on my subnet? */
                    if ((srcIP & PAN(ipMask)) == PAN(ipNet)) {
                        /* yes, update table, don't reply or forward */
                        //parms->pan->remIpAddr = arpHdrIn->srcIP;

#if PAN_ROLE_NAP == XA_ENABLED
                        /* Is the address routed yet? */
                        //if (!PAN_PanUserRouted(parms->pan)) { 
                            /* no, add the address to my proxy router */
                            //PAN_AddRouterAddr(parms->pan);
                        //}
#endif
                        kal_trace(BT_TRACE_G2_PROFILES, PAN_HANDLEPACKETIND_GRATUITOUS_ARPREQUEST);
                    }
                    return;
                }
                //else if (macAddr) {
                    /* Reply to the source MAC if it's not broadcast */
                    //OS_MemCopy(Info->pkt, arpHdrIn->srcMac, 6);
                //}

                if (macAddr) {
                    /* construct the ARP reply and send it back */

                    /* Modify the MAC header */
                    //OS_MemCopy(Info->pkt, Info->pkt + 6, 6);
                    //OS_MemCopy(Info->pkt + 6, macAddr->addr, 6);

                    /* Modify the ARP header */
                    OS_MemCopy(arpHdrIn->dstMac, arpHdrIn->srcMac, 6);
                    OS_MemCopy(arpHdrIn->dstIP, arpHdrIn->srcIP, 4);
                    OS_MemCopy(arpHdrIn->srcMac, macAddr->addr, 6);
                    StoreBE32(arpHdrIn->srcIP, srcIP);
                    StoreBE16(arpHdrIn->op, ARP_REPLY);
                    parms->event = PANEVENT_DATA_IND;
                    parms->pkt = arpHdrIn;
                    parms->type = PANPKT_ARP_REQUEST;
                    parms->rxPktLen = Info->rxPktLen - (ETHER_HDR_SIZE + offset);
                    PAN(callback)(parms);	
                }
            }
            else if (BEtoHost16(arpHdrIn->op) == ARP_REPLY) {
                Report(("PAN: Received ARP Reply\n"));
                Report(("Destination MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n", 
                    arpHdrIn->dstMac[0], arpHdrIn->dstMac[1], arpHdrIn->dstMac[2],
                    arpHdrIn->dstMac[3], arpHdrIn->dstMac[4], arpHdrIn->dstMac[5]));

                /* Update my Address table */
                if (PanFindMacAddr(arpHdrIn->dstMac) == PANUSER_SELF) {
                    PanUpdateArpTable(parms->pan, arpHdrIn->srcMac, srcIP);
                }

                /* Pass the reply up to the App */
                parms->event = PANEVENT_DATA_IND;
                parms->rxPktLen = Info->rxPktLen - (ETHER_HDR_SIZE + offset);
                parms->type = PANPKT_ARP_REPLY;
                PAN(callback)(parms);  
            }
            break;

        case PAN_EHTERTYPE_IP:
            Report(("PAN: Received IPv4 Packet\n"));
            parms->pkt = Info->pkt + ETHER_HDR_SIZE + offset;
            ipHdr = (IpHeader *)parms->pkt;
            parms->event = PANEVENT_DATA_IND;
            parms->rxPktLen = Info->rxPktLen - ETHER_HDR_SIZE - offset;
            parms->type = PANPKT_IP;

            dstPan = PanFindMacAddr(Info->pkt);
            srcPan = PanFindMacAddr(Info->pkt + 6);			

            bt_prompt_trace(MOD_BT, "PAN received ipv4 packet, dstmac: %x, %x, %x", Info->pkt[0], Info->pkt[1], Info->pkt[2]);
            bt_prompt_trace(MOD_BT, "PAN received ipv4 packet, selfmac: %x, %x, %x", PAN(macAddr).addr[0], PAN(macAddr).addr[1], PAN(macAddr).addr[2]);
            bt_prompt_trace(MOD_BT, "PAN: Received IPv4 Packet, protocol: %d", ipHdr->proto);
            if (ipHdr->proto == IPPROTO_UDP)
            {
                UdpHeader *udpHdr;
                DhcpHeader *dhcpHdr;
                udpHdr = (UdpHeader *)((U8 *)ipHdr + sizeof(IpHeader));
                bt_prompt_trace(MOD_BT, "PAN: Received UDP Packet, src port: %d", BEtoHost16((U8 *)&udpHdr->srcPort));
                // handle DHCP request broadcast packet, do not forward
                if (BEtoHost16((U8 *)&udpHdr->srcPort) == BOOTP_SERVER)
                {
                    dhcpHdr = (DhcpHeader *)((U8 *)udpHdr + sizeof(UdpHeader));
                    Report(("PAN: Received DHCP reply packet option: %d\n",
                        dhcpHdr->options[2]));
                    //parms->type = PANPKT_DHCP_REPLY;
                    if (dhcpHdr->options[2] == DHCPACK) 
                    {
                        PanUpdateArpTable(parms->pan, Info->pkt + 6, ipHdr->srcIP);
                    }
                    PAN(callback)(parms);
                    return;
                }
                else if (BEtoHost16((U8 *)&udpHdr->srcPort) == BOOTP_CLIENT)
                {
                    dhcpHdr = (DhcpHeader *)((U8 *)udpHdr + sizeof(UdpHeader));
                    Report(("PAN: Received DHCP request packet option: %d\n",
                        dhcpHdr->options[2]));
                    //parms->type = PANPKT_DHCP_REQUEST;
                    PAN(callback)(parms);
                    return;
                }
            }

            bt_prompt_trace(MOD_BT, "pan receive ip packet dstPan: %x", dstPan);
            /* Is this packet for me? */
            if (dstPan == PANUSER_SELF ||
                dstPan == PANUSER_BROADCAST ||
                dstPan == PANUSER_MULTICAST ||
                dstPan == PANUSER_NULL) {
                /* yes, send it up. */
                PAN(callback)(parms);
            }

            /* Forward this packet to anyone? */
            if (srcPan == parms->pan && dstPan != PANUSER_SELF &&
                PAN(service) != PANSVC_PANU) {
                /* yes, forward it on */
                //parms->pkt = Info->pkt;
                //parms->type = PANPKT_ETHER_PKT;
                //parms->rxPktLen = Info->rxPktLen;
                //PAN(callback)(parms);
                PAN_ForwardPkt(Info->pkt, Info->rxPktLen);
            }
            break;

        default:
            Report(("PAN: Received Unknown Packet\n"));
            break;
        }
        break;
#endif

    case BNEP_CONTROL:
        kal_trace(BT_TRACE_G2_PROFILES, PAN_BNEPCONTROL_PKTTYPE, Info->pkt[1]);
        switch (Info->pkt[1]) {
        case BNEP_FILTER_NET_TYPE_SET_MSG:
        {
            PanNettypeFilterNode *nettype_node;
            listLength = BEtoHost16(&Info->pkt[2]);
            Report(("PAN: Received net type filter list length: %d", listLength));
            parms->status = BNEP_FR_OPERATION_SUCCESS;
            if (listLength == 0)
            {
                /* remove previous filter if there is any */
                while (!IsListEmpty(&parms->pan->nettypeFilterList)) 
                {
                    nettype_node = (PanNettypeFilterNode *)RemoveHeadList(&parms->pan->nettypeFilterList);
                    pan_free(nettype_node);
                }
                goto nettype_exit;
            }

            /* save nettype filter into filter list */
            filterPkt = Info->pkt+4;
            while (listLength > 0)
            {
                nettype_node = (PanNettypeFilterNode *)pan_malloc(sizeof(PanNettypeFilterNode));
                nettype_node->nettypeStart = BEtoHost16(filterPkt);
                nettype_node->nettypeEnd = BEtoHost16(filterPkt+2);
                Report(("PAN: Received net type filter, start: %d, end; %d", nettype_node->nettypeStart, nettype_node->nettypeEnd));

                if (nettype_node->nettypeStart > nettype_node->nettypeEnd)
                {
                    parms->status = BNEP_FR_INVALID_PROTOCOL;
                    break;					
                }
                filterPkt += 4;
                listLength -= 4;				
                InsertTailList(&parms->pan->nettypeFilterList, &(nettype_node->node));
            }

nettype_exit:
            parms->pkt = Info->pkt+1;
            parms->event = PANEVENT_CONTROL_IND;
            //parms->rxPktLen = Info->rxPktLen - 1;
            PAN(callback)(parms);
            break;
        }
		
        case BNEP_FILTER_MULTI_ADDR_SET_MSG:
        {
            PanMulticastFilterNode *multiaddr_node;
            listLength = BEtoHost16(&Info->pkt[2]);
            Report(("PAN: Received multicast address filter list length: %d", listLength));
            parms->status = BNEP_FR_OPERATION_SUCCESS;
            if (listLength == 0)
            {
                /* remove previous filter if there is any */
                while (!IsListEmpty(&parms->pan->multicastFilterList)) 
                {
                    multiaddr_node = (PanMulticastFilterNode *)RemoveHeadList(&parms->pan->multicastFilterList);
                    pan_free(multiaddr_node);
                }
                goto multiaddr_exit;
            }

            /* save multicast address filter into filter list */
            filterPkt = Info->pkt+4;
            while (listLength > 0)
            {
                multiaddr_node = (PanMulticastFilterNode *)pan_malloc(sizeof(PanMulticastFilterNode));
                OS_MemCopy(multiaddr_node->multiaddrStart, filterPkt, 6);				
                OS_MemCopy(multiaddr_node->multiaddrEnd, filterPkt+6, 6);
                filterPkt += 12;
                listLength -= 12;
                InsertTailList(&parms->pan->multicastFilterList, &(multiaddr_node->node));
            }
			
multiaddr_exit:
            parms->pkt = Info->pkt+1;
            parms->event = PANEVENT_CONTROL_IND;
            //parms->rxPktLen = Info->rxPktLen - 1;
            PAN(callback)(parms);
            break;
        }
		
        case BNEP_FILTER_NET_TYPE_RESPONSE_MSG:
        case BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG:
            parms->pkt = Info->pkt+1;
            parms->event = PANEVENT_CONTROL_IND;
            //parms->rxPktLen = Info->rxPktLen;
            PAN(callback)(parms);
            break;

        case BNEP_SETUP_CONNECTION_REQUEST_MSG:
            /* Check the UUID size */
            if (!Info->pkt[2] || Info->pkt[2] > 16) {
                parms->status = BNEP_SR_INVALID_UUID_SIZE;
                goto conreq_exit;
            }

            /* Check the Destination UUID */
            uuid = Info->pkt + 3;
            if (Info->pkt[2] == 2) {
                service = BEtoHost16(uuid);
            }
            else {
                service = BEtoHost32(uuid);
            }
            if (service == PANSVC_NAP ||
                service == PANSVC_GN||
                service == PANSVC_PANU) {
                PAN(service) = service;
            }
            else {
                // PANU service is supported now
                parms->status = BNEP_SR_INVALID_DEST_UUID;
                goto conreq_exit;
            }
                 
            for (i=4; (Info->pkt[2] > 4) && (i < Info->pkt[2] - 4); i++) {
                if (BtBaseUUID[i] != uuid[i]) {
                    parms->status = BNEP_SR_INVALID_DEST_UUID;
                    goto conreq_exit;
                }
            }

            /* Check the Source UUID */
            uuid += Info->pkt[2];
            if (Info->pkt[2] == 2) {
                service = BEtoHost16(uuid);
            }
            else {
                service = BEtoHost32(uuid);
            }
            if (service != PANSVC_PANU && service != PANSVC_NAP) {
                parms->status = BNEP_SR_INVALID_SRC_UUID;
                goto conreq_exit;
            }
            for (i=4; (Info->pkt[2] > 4) && (i < Info->pkt[2] - 4); i++) {
                if (BtBaseUUID[i] != uuid[i]) {
                    parms->status = BNEP_SR_INVALID_SRC_UUID;
                    goto conreq_exit;
                }
            }
            parms->pan->dstService = service;
                
            /* Everything looks good */
            parms->status = BNEP_SR_OPERATION_SUCCESS;
            parms->service = PAN(service);


conreq_exit:
            parms->event = PANEVENT_SETUP_CONN_REQ;
            PAN(callback)(parms);
            break;

        case BNEP_SETUP_CONNECTION_RESPONSE_MSG:
            parms->pkt = Info->pkt;
            parms->status = BEtoHost16((U8 *)parms->pkt + 2);
            if (parms->status == BNEP_SR_OPERATION_SUCCESS) {
                parms->event = PANEVENT_OPEN;  // connection initiator
                parms->service = PAN(service);
                parms->pan->state = PAN_OPENED;
            }
            else {
                Report(("PAN: SetupConnection failed: %d\n", parms->status));
                BNEP_Close(&parms->pan->bnepPanu);
                break;
            }
            PAN(callback)(parms);
            break;

        case BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD:
            Report(("Pan: Rx'd control command not understood, ignore it..."));
            break;			

        default:
            // unknown control command
            parms->event = PANEVENT_CONTROL_IND;
            parms->pkt = Info->pkt + 1;						
            //parms->rxPktLen = Info->rxPktLen - 1;
            PAN(callback)(parms);
            break;
        }
        break;

    default:
        Report(("Pan: Rx'd unrecognized packet type %d\n", Info->type));
        break;
    }
}
#endif
