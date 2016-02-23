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

/*****************************************************************************
 *
 * File:
 *     $Workfile:pan_util.c$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: This file contains code for the BNEP protocol. 
 *
 * Created:     Jan 12, 2003
 *
 * Copyright 2000-2002 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, Inc.  
 * This work contains confidential and proprietary information of Extended 
 * Systems, Inc. which is protected by copyright, trade secret, trademark and 
 * other intellectual property rights.
 *
 ****************************************************************************/
#ifdef __BT_PAN_PROFILE__

#include <sys/pani.h>
#include <sys/bnepi.h>

#define PanIsNum(n)         (n >= '0' && n <= '9')
static U32 PanBnepToX(BNEP_ADDR *addr);

/*---------------------------------------------------------------------------
 *            PanClearConnection()
 *---------------------------------------------------------------------------
 */
void PanClearConnection(PanUser *pan, U16 status)
{
    PanCallbackParms    parms;
    PanNettypeFilterNode *nettype_node;
    PanMulticastFilterNode *multiaddr_node;
    PanExtHdrNode *exthdr_node;

    kal_trace(BT_TRACE_G2_PROFILES, PAN_CLEAR_CONNECTION_STATUS, status);
	
    /* We may get this as the app is closing.
    * Make sure the callback is still valid.
    */
    if (PAN(callback) && pan) 
    {
        parms.pan = pan;
        parms.event = PANEVENT_CLOSED;
        parms.status = status;
        parms.service = PAN(service);
        PAN(callback)(&parms);
    }

    CMGR_RemoveDataLink(&pan->cmgrHandler);
    CMGR_RemoveCmgrWithDevice(&pan->cmgrHandler);

    while (!IsListEmpty(&pan->nettypeFilterList)) 
    {
        nettype_node = (PanNettypeFilterNode *)RemoveHeadList(&pan->nettypeFilterList);
        pan_free(nettype_node);
    }

    while (!IsListEmpty(&pan->multicastFilterList)) 
    {
        multiaddr_node = (PanMulticastFilterNode *)RemoveHeadList(&pan->multicastFilterList);
        pan_free(multiaddr_node);
    }

    while (!IsListEmpty(&pan->exthdrList)) 
    {
        exthdr_node = (PanExtHdrNode *)RemoveHeadList(&pan->exthdrList);
        pan_free(exthdr_node->exthdr);
        pan_free(exthdr_node);
    }

    PanFreePanuser(pan);
}

/*---------------------------------------------------------------------------
 *            PanNewPanuser()
 *---------------------------------------------------------------------------
 */
PanUser *PanNewPanuser(void)
{
    PanUser *pan = NULL;

    pan = (PanUser *)pan_malloc(sizeof(PanUser));
    if (pan != NULL)
    {
        OS_MemSet((U8 *)pan, 0, sizeof(PanUser));
        InsertTailList(&PAN(panList), &(pan->node));

        InitializeListHead(&pan->nettypeFilterList);
        InitializeListHead(&pan->multicastFilterList);
        InitializeListHead(&pan->exthdrList);

        CMGR_RegisterHandler(&pan->cmgrHandler, PanCmgrCallback);	   
    }
    return pan;	
}

/*---------------------------------------------------------------------------
 *            PanFreePanuser()
 *---------------------------------------------------------------------------
 */
void PanFreePanuser(PanUser *pan)
{
    CMGR_DeregisterHandler(&pan->cmgrHandler);
    RemoveEntryList(&(pan->node));
    pan_free(pan);
}

/*---------------------------------------------------------------------------
 *            PanFindPanuser()
 *---------------------------------------------------------------------------
 */
PanUser *PanFindPanuser(BnepPanu *bnepPanu)
{
    PanUser *pan = NULL;
    PanUser *savedPan = NULL;

    if (bnepPanu != 0) 
    {
        pan = (PanUser *)GetHeadList(&PAN(panList));
        while ((ListEntry *)pan != &PAN(panList))
        {
            if (&pan->bnepPanu == bnepPanu)
            {
                /* Found the matching pan */
                savedPan = pan;
                break;				
            }
            /* Try the next node in list */	
            pan = (PanUser *)GetNextNode(&pan->node);
        }
    }

    return savedPan;
}

/*---------------------------------------------------------------------------
 *            PanMacCmp()
 *---------------------------------------------------------------------------
 */
static BOOL PanMacCmp(U8 *mac1, U8 *mac2)
{
    U8 i;

    for (i=0; i<6; i++) {
        if (mac1[i] != mac2[i]) {
            break;
        }
    }
    if (i == 6) {
        return TRUE;
    }
    return FALSE;
}

/*---------------------------------------------------------------------------
 *            PanFindMacAddr()
 *---------------------------------------------------------------------------
 */
PanUser *PanFindMacAddr(U8 *macAddr)
{
    U8 bcAddr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    U8 nulAddr[6] = {0};
    PanUser *pan = NULL;

    /* is it mine? */
    if (PanMacCmp(macAddr, PAN(macAddr).addr)) {
        return PANUSER_SELF;
    }

    /* check for broadcast */
    if (PanMacCmp(macAddr, bcAddr)) {
        return PANUSER_BROADCAST;
    }

    /* check for multicast */
    /* Ethernet uses the low-order bit of the high-order octet to distinguish
    *   conventional unicast address from multicast address.
    *   -unicast set the bit to 0
    *   -multicast set the bit to 1
    */	
    if (macAddr[0] & 0x1)
    {
        return PANUSER_MULTICAST;
    }

    /* check for null */
    if (PanMacCmp(macAddr, nulAddr)) {
        return PANUSER_NULL;
    }

    pan = (PanUser *)GetHeadList(&PAN(panList));
    while ((ListEntry *)pan != &PAN(panList))
    {
        if (PanMacCmp(macAddr, pan->bnepPanu.bnepAddr.addr))
        {
            /* Found the matching pan */
            return pan;
        }
        /* Try the next node in list */	
        pan = (PanUser *)GetNextNode(&pan->node);
    }

    /* Didn't find a PanUser */
    return 0;
}

/*---------------------------------------------------------------------------
 *            PanFindIpAddr()
 *---------------------------------------------------------------------------
 */
PanUser *PanFindIpAddr(U32 ipAddr)
{
    PanUser *pan = NULL;

    bt_prompt_trace(MOD_BT, "pan find ip addr: %x", ipAddr);
    /* check for broadcast */
    if (ipAddr == 0xffffffff) {
        return PANUSER_BROADCAST;
    }

    /* check for null */
    if (ipAddr == 0) {
        return PANUSER_NULL;
    }

    /* check for self */
    if (ipAddr == PAN(ipAddr)) {
        return PANUSER_SELF;
    }

    pan = (PanUser *)GetHeadList(&PAN(panList));
    while ((ListEntry *)pan != &PAN(panList))
    {
        if (pan->remIpAddr == ipAddr)
        {
            /* Found the matching pan */
            return pan;
        }
        /* Try the next node in list */	
        pan = (PanUser *)GetNextNode(&pan->node);
    }

    /* Didn't find a PanUser */
    return 0;
}

/*---------------------------------------------------------------------------
 *            PanUpdateArpTable()
 *---------------------------------------------------------------------------
 */
void PanUpdateArpTable(PanUser *pan, U8 *macAddr, U32 ipAddr)
{
    U8          i, youngest = 0, oldest = 0;
    PanUser    *panTmp, *macTmp;

    /* NAPs and GNs store addresses in their connection table */
    panTmp = PanFindIpAddr(ipAddr);
    macTmp = PanFindMacAddr(macAddr);
    if (PAN(service) != PANSVC_PANU &&
        !panTmp && macTmp == pan &&
        panTmp != PANUSER_BROADCAST &&
        panTmp != PANUSER_NULL &&
        panTmp != PANUSER_SELF) {
        pan->remIpAddr = ipAddr;
        return;
    }

    /* Check if I already have the IP Address in my table */
    for (i=0; i<NUM_PAN_ARP_ADDRS; i++) {
        if (PAN(panAddr)[i].ipAddr == ipAddr) {
            OS_MemCopy(PAN(panAddr)[i].macAddr.addr, macAddr, 6);
            return;
        }
        if (PAN(panAddr)[youngest].age < PAN(panAddr)[i].age) {
            youngest = i;
        }
        if (PAN(panAddr)[oldest].age > PAN(panAddr)[i].age) {
            oldest = i;
        }
    }

    /* We didn't find it so add the new IP Address */
    for (i=0; i<NUM_PAN_ARP_ADDRS; i++) {
        if (PAN(panAddr)[i].ipAddr == 0) {
            PAN(panAddr)[i].ipAddr = ipAddr;
            PAN(panAddr)[i].age = PAN(panAddr)[youngest].age + 1;
            OS_MemCopy(PAN(panAddr)[i].macAddr.addr, macAddr, 6);
            return;
        }
    }

    /* no open slots, replace the oldest */
    PAN(panAddr)[oldest].ipAddr = ipAddr;
    PAN(panAddr)[oldest].age = PAN(panAddr)[youngest].age + 1;
    OS_MemCopy(PAN(panAddr)[oldest].macAddr.addr, macAddr, 6);
}

PanBtPkt *PanFindFreeBtPkt(PanUser *pan)
{
    U8 i;

    if (pan == NULL)
        return 0;
	
    for (i=0; i<NUM_BT_PKTS; i++) {
        if (pan->pktPool[i].context == PANBTPKT_FREE) {
            pan->pktPool[i].context = PANBTPKT_INUSE;
            return &pan->pktPool[i];
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------
 *            PanSetLinkLocalAddr()
 *---------------------------------------------------------------------------
 */
BOOL PanSetLinkLocalAddr()
{
    U32 tAddr;

    if (PAN(ipAddr) == 0) {
        /* Mask in Link local start range 169.254.1.0 */
        tAddr = PAN_LINK_LOCAL_SUBNET | 0x00000100;

        /* OR in the BNEP addr */
        tAddr |= (PanBnepToX(&BNEP(bnepAddr)) & 0x0000FFFF);
    }
    else {
        /* Recalculate starting with the previous address */
        tAddr = PAN_GetLocalIpAddr();
        tAddr += (PanBnepToX(&BNEP(bnepAddr)) & 0x0000FFFF);
    }
    
    /* Mask off Link local end range 169.254.254.255 */
    tAddr &= (PAN_LINK_LOCAL_SUBNET | 0x0000FEFF);

    /* Store the Link local address */
    StoreBE32((U8 *)&PAN(linkLocalIpAddr), tAddr);

    /* Make the Link local address my addess until DHCP changes it. */
    StoreBE32((U8 *)&PAN(ipAddr), tAddr);

    /* copy the BNEP address to the PAN context */
    //OS_MemCopy((U8 *)&PAN(bnepAddr.addr), (U8 *)&(BNEP(bnepAddr).addr), 6);

    /* use the BNEP address as my MAC address */
    OS_MemCopy((U8 *)&PAN(macAddr.addr), (U8 *)&(BNEP(bnepAddr).addr), 6);
    return TRUE;
}

void PanRevertBdAddr(BD_ADDR *dstBdAddr, BD_ADDR *srcBdAddr)
{
    U8 i;
    for (i = 0; i < BD_ADDR_SIZE; i++) {
        dstBdAddr->addr[i] = srcBdAddr->addr[BD_ADDR_SIZE-1-i];
    }
}

static BOOL PanMultiaddrCmp(U8 *startAddr, U8 *endAddr, U8 *dstAddr)
{
    U8 i;
    BOOL eq_start = FALSE;
    BOOL eq_end = FALSE;
    BOOL dropped = FALSE;

    Report(("Pan multicast cmp address start: %x, %x, %x, %x, %x, %x", startAddr[0], startAddr[1], startAddr[2], 
		startAddr[3], startAddr[4], startAddr[5]));
    Report(("Pan multicast cmp address end: %x, %x, %x, %x, %x, %x", endAddr[0], endAddr[1], endAddr[2], 
		endAddr[3], endAddr[4], endAddr[5]));
    Report(("Pan multicast cmp address dst: %x, %x, %x, %x, %x, %x", dstAddr[0], dstAddr[1], dstAddr[2], 
		dstAddr[3], dstAddr[4], dstAddr[5]));

    for (i = 0; i < 6; i++)
    {
        if (!eq_start && !eq_end)
        {
            /* Range value: R1, R2; 
                 To be checked value: Val 
            */   
            /*
                case1: R1 = R2 = Val
            */
            if (startAddr[i] == endAddr[i])
            {
                if (dstAddr[i] != startAddr[i])
                {
                    dropped = TRUE;  // out of range, drop it
                    break;
                }
                continue;				
            }
            /*
                case2: R1 < Val < R2
            */
            else if ((startAddr[i] < dstAddr[i]) &&
                         (endAddr[i] > dstAddr[i]))
            {
                 dropped = FALSE;	 // in the range, NOT drop it
                 break;
            }
            /*
                case3: Val = R1 < R2
            */
            else if (startAddr[i] == dstAddr[i])
            {
                eq_start = TRUE;
            }
            /*
                case4: R1 < R2 = Val
            */
            else if (endAddr[i] == dstAddr[i])
            {
                eq_end = TRUE;
            }
            /*
                case5: Val < R1 < R2
                case6: R1 < R2 < Val
             */	
            else
            {
                dropped = TRUE;
                break;				
            }
        }
        else if (eq_start && !eq_end)  
        {
            if (dstAddr[i] < startAddr[i])
            {
                dropped = TRUE;  // out of range, drop it
                break;
            }
            else if (dstAddr[i] > startAddr[i])	
            {
                dropped = FALSE;  // in the ranged, NOT drop it
                break;
            }
            OS_Report(("[PAN] pan multiaddr compare, equal to start range"));
        }
        else if (!eq_start && eq_end)
        {
            if (dstAddr[i] > endAddr[i])
            {
                dropped = TRUE;  // out of range, drop it
                break;
            }
            else if (dstAddr[i] < endAddr[i])
            {
                dropped = FALSE;  // in the ranged, NOT drop it
                break;
            }
            OS_Report(("[PAN] pan multiaddr compare, equal to end range"));
        }
        else
        {
            OS_Report(("[PAN] pan multiaddr compare, error entering..."));
        }
    }
    return dropped;
}

static BOOL PanCheckNettypeFilter(PanUser *pan, U16 nettype)
{
    BOOL dropped = TRUE;
    PanNettypeFilterNode *nettype_node;

    if (IsListEmpty(&pan->nettypeFilterList))
        return FALSE;	

    nettype_node = (PanNettypeFilterNode *)GetHeadList(&pan->nettypeFilterList);
    while ((ListEntry *)nettype_node != &pan->nettypeFilterList)
    {
        if ((nettype >= nettype_node->nettypeStart) &&
             (nettype <= nettype_node->nettypeEnd))
        {
            dropped = FALSE;
            break;			
        }
        /* Try the next filter range */
        nettype_node = (PanNettypeFilterNode *)GetNextNode(&nettype_node->node);
    }
    return dropped;
}

static BOOL PanCheckMultiaddrFilter(PanUser *pan, U8 *dstAddr)
{
    BOOL dropped = TRUE;
    PanMulticastFilterNode *multiaddr_node;

    if (IsListEmpty(&pan->multicastFilterList))
        return FALSE;	

    Report(("Pan check multicast filter"));

    multiaddr_node = (PanMulticastFilterNode *)GetHeadList(&pan->multicastFilterList);	
    while ((ListEntry *)multiaddr_node != &pan->multicastFilterList)
    {
        if (!PanMultiaddrCmp(multiaddr_node->multiaddrStart, 
			multiaddr_node->multiaddrEnd, dstAddr))
        {
            /* already in the current filter range */
            dropped = FALSE;
            break;			
        }
        /* Try the next filter range */
        multiaddr_node = (PanMulticastFilterNode *)GetNextNode(&multiaddr_node->node);
    }
    return dropped;
}

/*--------------------------------------------------------------------------- 
 * PanCheckFilterDB 
 * 
 *     Check if the packet is included in the filter range. 
 *     return
 *         TRUE: packet is NOT in the filter range. It shall be dropped.
 *         FALSE: packet is in the filter range. It shall NOT be filtered.
 */ 
BOOL PanCheckFilterDB(PanUser *pan, U8 *pkt, BOOL unicast)
{
    U16 nettype;
    U8 dstMac[6];

    /* 1st: Check net type filter database */
    nettype = BEtoHost16(pkt + ETHER_HDR_SIZE - 2);
    
    /* The IEEE 802.1Q tag header, which has a size of 4 bytes, occupies 
    *	the 2 byte Networking protocol type field (ETPID) in the BNEP header 
    *	as well as additional two bytes (TCI) in the BNEP payload. The actual 
    *	networking protocol type for this packet is located in the BNEP payload 
    *	after the remaining part of the 802.1Q tag header.
    *
    *	Devices MAY use the IEEE 802.1Q tag header to determine prioritization
    *	of the packets.
    */
    if (nettype == PAN_EHTERTYPE_VLAN) 
    {
        nettype = BEtoHost16(pkt + ETHER_HDR_SIZE + 2);
    }

    Report(("Pan check 1st filter database, nettype: %ld", nettype));

    if (PanCheckNettypeFilter(pan, nettype))
        return TRUE;

    if (unicast)
        return FALSE;		

    Report(("Pan check 2st filter database..."));

    /* 2st: Check multicast address filter database (unicast is excluded) */
    OS_MemCopy(dstMac, pkt, 6);
    if (PanCheckMultiaddrFilter(pan, dstMac))
        return TRUE;		

    return FALSE;
}

/*--------------------------------------------------------------------------- 
 * ItoA 
 * 
 *     Converts an unsigned 32-bit value to a base-10 number string. 
 */ 
void PanItoA(U8 *a, U32 i) 
{ 
    U8  rem; 
    U8  str[11], *dest = str; 

    if (i == 0) 
        *dest++ = '0'; 

    while (i) { 
        rem = (U8)(i % 10); 
        *dest++ = rem + '0'; 
        i = i / 10; 
    } 

    while (--dest >= str) 
        *a++ = *dest; 

    *a = 0; 
} 

/*--------------------------------------------------------------------------- 
 * PanBnepToX 
 * 
 *     Generate a psuedo-random Hex number for Link Local IP address. 
 */ 
static U32 PanBnepToX(BNEP_ADDR *addr)
{ 
    U32 x = 0;
    U8  i, *a = addr->addr;

    for (i=6; i>0; i--) {
        x = (x * 16) + (*a++); 
    }

    return x; 
} 
#endif
