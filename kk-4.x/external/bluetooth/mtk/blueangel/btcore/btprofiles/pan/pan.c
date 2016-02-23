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
 *     $Workfile:pan.c$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: This file contains code for the BNEP protocol. 
 *
 * Created:     Jan 5, 2003
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

#include <assert.h>
//#include <sys/bnepi.h>
#include <btbm.h>
#include <bnep.h>
#include <sys/pani.h>

/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/
#if XA_CONTEXT_PTR == XA_ENABLED
static PanContext      tempContext;
PanContext          *pancx = {&tempContext};
#else /* XA_CONTEXT_PTR == XA_ENABLED */
PanContext           pancx;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

extern U8 PanSdpDB_AddRecord(U16 service_type);
extern U8 PanSdpDB_RemoveRecord(U16 service_type);


/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            PAN_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize PAN. This should be the first function called for
 *            PAN. 
 *
 * Return:    TRUE - PAN was successfully initialized.
 *            FALSE  - PAN failed to initialize.
 */
BOOL PAN_Init(void)
{
    OS_LockStack();

    BNEP_Init();
    
#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8 *)pancx, 0, sizeof(PanContext));
#else /* XA_CONTEXT_PTR == XA_ENABLED */
    OS_MemSet((U8 *)&pancx, 0, sizeof(PanContext));
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

    /* Initialize Lists */
    InitializeListHead(&PAN(panList));

    kal_trace(BT_TRACE_G2_PROFILES, PAN_INITIALIZED);

    OS_UnlockStack();
    return TRUE;
}

/*---------------------------------------------------------------------------
 *  PAN_Register
 */
BtStatus PAN_Register(PanService *service)
{
    BtStatus    status;

    OS_LockStack();

    if (PAN(serviceCount) == 0) 
    {
        status = BNEP_Register(PanBnepCallback);
        if (status == BT_STATUS_SUCCESS) 
        {
            
            PAN(callback) = service->callback;
            if (PanSetLinkLocalAddr() != TRUE)
            {
                status = BT_STATUS_FAILED;
            }
            else 
            {
                status = PanSdpDB_AddRecord(service->type);
            }
        }
    }
    else 
    {
        kal_trace(BT_TRACE_G2_PROFILES, PAN_REGISTER_BUSY);
        // Mutiple service record registration; 	
        status = PanSdpDB_AddRecord(service->type);
    }

    if (status == BT_STATUS_SUCCESS) 
    {
        PAN(serviceCount)++;			
    }

    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *  PAN_Deregister
 */
BtStatus PAN_Deregister(PanService *service)
{
    BtStatus    status;
    //U8          i;

    OS_LockStack();

    if (PAN(serviceCount) == 0)
    {
        return BT_STATUS_SUCCESS;
    }

    status = PanSdpDB_RemoveRecord(service->type);
    if (status == BT_STATUS_SUCCESS)
    {
        PAN(serviceCount)--;

        if (PAN(serviceCount) == 0)
        {
            status = BNEP_DeRegister();
            if (status == BT_STATUS_SUCCESS)
            {
                PAN(callback) = 0;
            }
        }
    }

    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *  PAN_Open
 */
BtStatus PAN_Open(PanUser **panChannel,
                   PanServiceType srcService,
                   PanServiceType dstService,
                   BD_ADDR *addr)
{
    BtStatus    status = BT_STATUS_FAILED;
    PanUser *pan;
    BD_ADDR remoteAddr;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, addr != 0);

    OS_LockStack();

    kal_trace(BT_TRACE_G2_PROFILES, PAN_OPENCONNECTION);
    if ((*panChannel != NULL) && IsNodeOnList(&PAN(panList), &((*panChannel)->node)))
    {
        pan = *panChannel;
    }
    else
    {
        pan = PanNewPanuser();
        if (pan == NULL)
        {
            return BT_STATUS_NO_RESOURCES;
        }
    }


    if (pan->state == PAN_CLOSED)
    {
        pan->dstService = dstService;
        PAN(service) = srcService;

        PanRevertBdAddr(&remoteAddr, addr);
        status = CMGR_CreateDataLink(&pan->cmgrHandler, &remoteAddr);
        kal_trace(BT_TRACE_G2_PROFILES, PAN_CREATEDATALINK_STATUS, status);	
        if (status == BT_STATUS_SUCCESS)
        {
            /* Perform an SDP Query */
            status = PanStartServiceQuery(pan, dstService);
        }

        kal_trace(BT_TRACE_G2_PROFILES, PAN_OPENCONNECTION_STATUS, status);
        
        if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING)
        {
            CMGR_RemoveDataLink(&pan->cmgrHandler);
            PanFreePanuser(pan);
            return status;		
        }
        
        pan->state = PAN_OPENING;
        *panChannel = pan;
        PAN_SetRemoteMacAddr(pan, (U8 *)addr);
    }


    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *  PAN_Close
 */
BtStatus PAN_Close(PanUser *pan)
{
    BtStatus status = BT_STATUS_FAILED;

    /* Validate parameters */
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, pan != 0);

    OS_LockStack();

    CheckLockedParm(BT_STATUS_NOT_FOUND, 
                    IsNodeOnList(&PAN(panList), &(pan->node)));

    // check state
    if (pan->state != PAN_CLOSED)
    {
        status = BNEP_Close(&pan->bnepPanu);
    }

    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *  PAN_SetupConnection
 */
BtStatus PAN_SetupConnection(PanUser *pan,
                             PanSetupConnPkt *setupPkt)
{
    BtStatus    status = BT_STATUS_FAILED;
    PanBtPkt   *panBtPkt;
    U16        *context;
    U8         *p, *pkt;

    OS_LockStack();
    panBtPkt = PanFindFreeBtPkt(pan);
    if (panBtPkt == 0) {
        status = BT_STATUS_NO_RESOURCES;
        goto fail;
    }
    context = (U16 *)(&panBtPkt->btPkt) - 1;
    *context = PANBTPKT_INUSE_EXT;

    pkt = p = setupPkt->header;
    p[0] = BNEP_CONTROL;
    p[1] = BNEP_SETUP_CONNECTION_REQUEST_MSG;
    p[2] = 2;
    p += 3;
    StoreBE16(p, pan->dstService);

    p += 2;
    StoreBE16(p, PAN(service));

    status = BNEP_Control(&pan->bnepPanu, &panBtPkt->btPkt, pkt, sizeof(PanSetupConnPkt));

fail:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *  PAN_FilterProtocol
 */
BtStatus PAN_FilterProtocol(PanUser *pan, PanFilterPkt *protoPkt)
{
    U16         len = sizeof(PanFilterPkt) + BEtoHost16(protoPkt->listLen);
    BtStatus    status = BT_STATUS_FAILED;
    PanBtPkt   *panBtPkt;
    U16        *context;
    //U8          listlen[2];
    U8         *pkt;

    OS_LockStack();
    panBtPkt = PanFindFreeBtPkt(pan);
    if (panBtPkt == 0) {
        status = BT_STATUS_NO_RESOURCES;
        goto fail;
    }

    context = (U16 *)(&panBtPkt->btPkt) - 1;
    *context = PANBTPKT_INUSE_EXT;

    //OS_MemCopy(listlen, (U8 *)&protoPkt->listLen, 2);
    pkt = protoPkt->list - 4;
    pkt[0] = BNEP_CONTROL;
    pkt[1] = BNEP_FILTER_NET_TYPE_SET_MSG;
    OS_MemCopy(pkt + 2, protoPkt->listLen, 2); 

    status = BNEP_Control(&pan->bnepPanu, &panBtPkt->btPkt, pkt, len);

fail:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *  PAN_FilterMulticast
 */
BtStatus PAN_FilterMulticast(PanUser *pan, PanFilterPkt *multiPkt)
{
    U16         len = sizeof(PanFilterPkt) + BEtoHost16(multiPkt->listLen);
    BtStatus    status = BT_STATUS_FAILED;
    PanBtPkt   *panBtPkt;
    U16        *context;
    //U8          listlen[2];
    U8         *pkt;

    OS_LockStack();
    panBtPkt = PanFindFreeBtPkt(pan);
    if (panBtPkt == 0) {
        status = BT_STATUS_NO_RESOURCES;
        goto fail;
    }

    context = (U16 *)(&panBtPkt->btPkt) - 1;
    *context = PANBTPKT_INUSE_EXT;

    //OS_MemCopy(listlen, (U8 *)&multiPkt->listLen, 2);
    pkt = multiPkt->list - 4;
    pkt[0] = BNEP_CONTROL;
    pkt[1] = BNEP_FILTER_MULTI_ADDR_SET_MSG;
    OS_MemCopy(pkt + 2, multiPkt->listLen, 2); 

    status = BNEP_Control(&pan->bnepPanu, &panBtPkt->btPkt, pkt, len);

fail:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *  PAN_ControlResponse
 */
BtStatus PAN_ControlResponse(PanUser *pan, BnepControlType type,
                             PanControlRespPkt *controlRespPkt, U16 len)
{
    BtStatus    status = BT_STATUS_FAILED;
    PanBtPkt   *panBtPkt;
    U16        *context;
    U8         *pkt;

    OS_LockStack();
    panBtPkt = PanFindFreeBtPkt(pan);
    if (panBtPkt == 0) {
        status = BT_STATUS_NO_RESOURCES;
        goto fail;
    }
    context = (U16 *)(&panBtPkt->btPkt) - 1;
    *context = PANBTPKT_INUSE_EXT;


    pkt = controlRespPkt->header;
    pkt[0] = BNEP_CONTROL;
    pkt[1] = type;

    status = BNEP_Control(&pan->bnepPanu, &panBtPkt->btPkt,
               pkt, len);

    if (status == BT_STATUS_FAILED)
    {
        goto fail;
    }

    if (type == BNEP_SETUP_CONNECTION_RESPONSE_MSG && 
        BEtoHost16(controlRespPkt->message) == BNEP_SR_OPERATION_SUCCESS)
    {
        PanCallbackParms parms;
        parms.pan = pan;
        parms.event = PANEVENT_OPEN;
        parms.service = PAN(service);
        PAN(callback)(&parms);
        pan->state = PAN_OPENED;

        /* Remote initiate the connection, do role switch if we are GN or NAP */
        if (PAN(service) == PANSVC_NAP ||
            PAN(service) == PANSVC_GN)
        {
            Report(("Pan do role switch"));
            BTBMDoRoleSwitch(pan->bnepPanu.remDev);
        }
    }

fail:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *  PAN_SendPkt
 */
BtStatus PAN_SendPkt(PanUser *pan, PanPacket *panPkt,
                     PanPacketType type,
                     U16 len)
{
    PanUser *dstPan = pan;
    BtStatus    status = BT_STATUS_FAILED;
    PanBtPkt   *panBtPkt;
    U8         *bnepPkt = panPkt->body;
    IpHeader   *ipHdr;
    BNEP_ADDR  *macAddr;

    OS_LockStack();
    switch (type & ~PANPKT_EXTENSION_HEADER_BIT) {		
    case PANPKT_ETHER_PKT:
        len++;
        bnepPkt--;
        if ( type & PANPKT_EXTENSION_HEADER_BIT )
        {
            bnepPkt[0] =BNEP_GENERAL_ETHERNET  | BNEP_E_BIT;	
        }
        else
        {
            bnepPkt[0] =BNEP_GENERAL_ETHERNET ;		
        }
        Report(("pan send packet bnep type: %x", bnepPkt[0]));
        break;

    case PANPKT_ETHER_COMPRESSED_PKT:
        bnepPkt -= (BNEP_GNRL_HDR_SIZE - BNEP_ADDR_SIZE*2);
        OS_MemCopy(bnepPkt, panPkt->body, len);
        len = len - BNEP_ADDR_SIZE*2+ 1;
        bnepPkt =  bnepPkt + BNEP_ADDR_SIZE*2-1;
        if ( type & PANPKT_EXTENSION_HEADER_BIT )
        {
            bnepPkt[0] =BNEP_COMPRESSED_ETHERNET  | BNEP_E_BIT;	
        }
        else
        {
            bnepPkt[0] =BNEP_COMPRESSED_ETHERNET ;		
        }
        break;

    case PANPKT_ETHER_SRC_ONLY_PKT:
        bnepPkt -= (BNEP_GNRL_HDR_SIZE - BNEP_ADDR_SIZE);
        OS_MemCopy(bnepPkt, panPkt->body, len);
        len =  len - BNEP_ADDR_SIZE + 1;
        bnepPkt =  bnepPkt + BNEP_ADDR_SIZE -1;
        if ( type & PANPKT_EXTENSION_HEADER_BIT )
        {
            bnepPkt[0] = BNEP_COMPRESSED_ETHERNET_SOURCE_ONLY  | BNEP_E_BIT;	
        }
        else
        {
            bnepPkt[0] = BNEP_COMPRESSED_ETHERNET_SOURCE_ONLY;		
        }
        break;
		
    case PANPKT_ETHER_DEST_ONLY_PKT:
        bnepPkt -= (BNEP_GNRL_HDR_SIZE - BNEP_ADDR_SIZE);
        OS_MemCopy(panPkt->body + 6, panPkt->body, 6);		
        OS_MemCopy(bnepPkt, panPkt->body, len);
        len =  len - BNEP_ADDR_SIZE + 1;
        bnepPkt =  bnepPkt + BNEP_ADDR_SIZE -1;
        if ( type & PANPKT_EXTENSION_HEADER_BIT )
        {
            bnepPkt[0] = BNEP_COMPRESSED_ETHERNET_DEST_ONLY  | BNEP_E_BIT;	
        }
        else
        {
            bnepPkt[0] = BNEP_COMPRESSED_ETHERNET_DEST_ONLY;		
        }
        break;

#if 0
    case PANPKT_ARP_REQUEST:
        dstPan = PANUSER_BROADCAST;
        len += (BNEP_GNRL_HDR_SIZE - BNEP_ADDR_SIZE);
        bnepPkt -= (BNEP_GNRL_HDR_SIZE - BNEP_ADDR_SIZE);
        StoreBE16(bnepPkt + BNEP_ADDR_SIZE + 1, 0x0806);  /* ARP header type */

        /* Set broadcast addr, should this be a multicast addr? */
        OS_MemSet(bnepPkt + 1, 0xff, 6);
        bnepPkt[0] = BNEP_COMPRESSED_ETHERNET_DEST_ONLY;
        break;

    case PANPKT_ARP_REPLY:
        assert(pan != NULL);
        len += 3;
        bnepPkt -= 2;
        StoreBE16(bnepPkt, 0x0806);  /* ARP header type */
        bnepPkt--;
        bnepPkt[0] = BNEP_COMPRESSED_ETHERNET;
        break;

    case PANPKT_IP:
        ipHdr = (IpHeader *)bnepPkt;
        bt_prompt_trace(MOD_BT, "pan send ip packet, protocol: %x", ipHdr->proto);

        len += (BNEP_GNRL_HDR_SIZE - BNEP_ADDR_SIZE);
        bnepPkt -= (BNEP_GNRL_HDR_SIZE - BNEP_ADDR_SIZE);
        StoreBE16(bnepPkt + BNEP_ADDR_SIZE + 1, 0x0800);  /* IP header type */
        /* Get dst mac address via dst IP */
        if (dstPan != NULL)
        {
            OS_MemCopy(bnepPkt + 1, (U8 *)&dstPan->bnepPanu.bnepAddr, 6);
        }
        else
        {
            dstPan = PanFindIpAddr(BEtoHost32(ipHdr->dstIP));
            if (dstPan != NULL)
            {
                if (dstPan == PANUSER_SELF)
                {
                    OS_MemCopy(bnepPkt + 1, (U8 *)&PAN(macAddr), 6);
                }
                else if (dstPan == PANUSER_BROADCAST ||
                    dstPan == PANUSER_NULL)
                {
                    OS_MemSet(bnepPkt + 1, 0xff, 6);
                }
                else
                {
                    OS_MemCopy(bnepPkt + 1, (U8 *)&dstPan->bnepPanu.bnepAddr, 6);
                }
            }
            else
            {
                U32  dstIp = (ipHdr->dstIP[0] << 24) |
					(ipHdr->dstIP[1] << 16) |
					(ipHdr->dstIP[2] << 8) |
					(ipHdr->dstIP[3] );
				
                // find in arp table
                macAddr = PAN_SendArp(dstIp);
                if (!macAddr) {
                    status = BT_STATUS_FAILED;
                    goto fail;
                }
                OS_MemCopy(bnepPkt + 1, macAddr->addr, 6);
            }
        }
        bnepPkt[0] = BNEP_COMPRESSED_ETHERNET_DEST_ONLY;
        break;
#endif 

    case PANPKT_UNKNOWN:
    default:
        status = BT_STATUS_FAILED;
        goto fail;
    }

    panBtPkt = PanFindFreeBtPkt(dstPan);
    if (panBtPkt == 0) {
        status = BT_STATUS_NO_RESOURCES;
        goto fail;
    }
    panBtPkt->context = PANBTPKT_INUSE_EXT;
    panBtPkt->btPkt.priority_type = BTP_FLAG_HIGH_PRIPORTY;
    Report(("pan send packet bnep type 2: %x", bnepPkt[0]));

    status = BNEP_SendPkt(&dstPan->bnepPanu, &panBtPkt->btPkt, bnepPkt, len);

fail:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *  PAN_ForwardPkt
 */
BtStatus PAN_ForwardPkt(U8 *pkt, U16 len, U8 *exthdr, U16 exthdrlen)
{
    BtStatus    status = BT_STATUS_FAILED;
    PanBtPkt   *panBtPkt;
    U16        *context;
    PanUser    *dstPan, *srcPan;
    U8 *pktForward;
    U8 *exthdrForward = NULL;	

    OS_LockStack();
    dstPan = PanFindMacAddr(pkt);
    srcPan = PanFindMacAddr(pkt + 6);


    if (dstPan == 0) 
    {
        /* no local devices targeted */
        kal_trace(BT_TRACE_G2_PROFILES, PAN_FORWARDPKT_NODST);
        status = BT_STATUS_DEVICE_NOT_FOUND;
    }
    else if (dstPan == PANUSER_BROADCAST || 
		dstPan == PANUSER_MULTICAST ||
		dstPan == PANUSER_NULL) 
    {
        kal_trace(BT_TRACE_G2_PROFILES, PAN_FORWARDPKT_BROADCAST);
        /* This is a broadcast, send to all connected devices */
        dstPan = (PanUser *)GetHeadList(&PAN(panList));
        while ((ListEntry *)dstPan != &PAN(panList))
        {
            if ((dstPan->state == PAN_OPENED) &&
    	    (dstPan != srcPan || !srcPan))
            {
                /* Check filter database (net type filter and multicast filter)*/
                if (PanCheckFilterDB(dstPan, pkt, FALSE))
                {
                    if (exthdr != NULL)
                    {
                        /* If there is any extension header, just forward without payload anyway, and set protocol type to 0 */
                        U8 *proto = pkt + ETHER_HDR_SIZE - sizeof(BnepEthernetProtocolType);
                        if (BEtoHost16(proto) == PAN_EHTERTYPE_VLAN) 
                        {
                            len = ETHER_HDR_SIZE + 4;
                            StoreBE16(proto + 4, 0);							
                        }
                        else
                        {
                            len = ETHER_HDR_SIZE;
                            StoreBE16(proto, 0);							
                        }
                    }
                    else
                    {
                        /* It shall be dropped, try the next node */
                        dstPan = (PanUser *)GetNextNode(&dstPan->node);
                        continue;
                    }
                }

                /* Found the matching pan */
                panBtPkt = PanFindFreeBtPkt(dstPan);
                if (panBtPkt == 0) {
                    status = BT_STATUS_NO_RESOURCES;
                    goto exit;
                }
/*
                pktForward = (U8 *)pan_malloc(len);
                OS_MemCopy(pktForward, pkt, len);

                if (exthdr != NULL)
                {
                    exthdrForward = (U8 *)pan_malloc(exthdrlen);
                    OS_MemCopy(exthdrForward, exthdr, exthdrlen);
                }
*/

                pktForward = (U8 *)pan_malloc(len + exthdrlen);
                OS_MemCopy(pktForward, pkt, len);
                if (exthdr != NULL)
                {
                    exthdrForward = pktForward + len;
                    OS_MemCopy(exthdrForward, exthdr, exthdrlen);		
                }
                
                Report(("Pan forward broadcast packet, pkt: %x, exthdr: %x", pktForward, exthdrForward));
                
                panBtPkt->context = PANBTPKT_INUSE_INT;
                panBtPkt->btPkt.priority_type = BTP_FLAG_HIGH_PRIPORTY;
                status = BNEP_SendEtherPkt(&dstPan->bnepPanu,
                    &panBtPkt->btPkt, pktForward, len, exthdrForward);
            }
            /* Try the next node in list */	
            dstPan = (PanUser *)GetNextNode(&dstPan->node);
        }
    }
    else
    {
        kal_trace(BT_TRACE_G2_PROFILES, PAN_FORWARDPKT_TOPEER);
        if (PanCheckFilterDB(dstPan, pkt, TRUE))
        {
            if (exthdr != NULL)
            {
                /* If there is any extension header, just forward without payload anyway, and set protocol type to 0 */
                U8 *proto = pkt + ETHER_HDR_SIZE - sizeof(BnepEthernetProtocolType);
                if (BEtoHost16(proto) == PAN_EHTERTYPE_VLAN) 
                {
                    len = ETHER_HDR_SIZE + 4;
                    StoreBE16(proto + 4, 0);							
                }
                else
                {
                    len = ETHER_HDR_SIZE;
                    StoreBE16(proto, 0);							
                }
            }
            else
            {
                /* It shall be dropped */
                goto exit;			
            }
        }
		
        panBtPkt = PanFindFreeBtPkt(dstPan);
        if (panBtPkt == 0) {
            status = BT_STATUS_NO_RESOURCES;
            goto exit;
        }

/*
        pktForward = (U8 *)pan_malloc(len);
        OS_MemCopy(pktForward, pkt, len);

        if (exthdr != NULL)
        {
            exthdrForward = (U8 *)pan_malloc(exthdrlen);
            OS_MemCopy(exthdrForward, exthdr, exthdrlen);
        }
*/
        pktForward = (U8 *)pan_malloc(len + exthdrlen);
        OS_MemCopy(pktForward, pkt, len);
        if (exthdr != NULL)
        {
            exthdrForward = pktForward + len;
            OS_MemCopy(exthdrForward, exthdr, exthdrlen);		
        }

        Report(("Pan forward unicast packet, pkt: %x, exthdr: %x", pktForward, exthdrForward));
		
        context = (U16 *)(&panBtPkt->btPkt) - 1;
        *context = PANBTPKT_INUSE_INT;
        panBtPkt->btPkt.priority_type = BTP_FLAG_HIGH_PRIPORTY;
        status = BNEP_SendEtherPkt(&dstPan->bnepPanu, &panBtPkt->btPkt, pktForward, len, exthdrForward);
    }

    Report(("Pan forward status: %d", status));

exit:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *  PAN_SendArp
 */
BNEP_ADDR *PAN_SendArp(U32 destIP)
{
    BNEP_ADDR  *mAddr = 0;
    U8          i;

    OS_LockStack();

    for (i=0; i<NUM_PAN_ARP_ADDRS; i++) {
        if (PAN(panAddr)[i].ipAddr == destIP) {
            mAddr = &PAN(panAddr)[i].macAddr;
        }
    }

    OS_UnlockStack();
    return mAddr;
}

/*---------------------------------------------------------------------------
 *  PAN_SetRemoteMacAddr
 */
BtStatus PAN_SetRemoteMacAddr(PanUser *pan, U8 *macAddr)
{
    BtStatus status = BT_STATUS_SUCCESS;

    OS_LockStack();
    OS_MemCopy(pan->destMac.addr, macAddr, 6);
    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 *  PAN_SetLocalMacAddr
 */
BtStatus PAN_SetLocalMacAddr(U8 *macAddr)
{
    BtStatus status = BT_STATUS_SUCCESS;

    OS_LockStack();
    OS_MemCopy((U8 *)&PAN(macAddr.addr), macAddr, 6);
    OS_UnlockStack();

    return status;
}

#endif
