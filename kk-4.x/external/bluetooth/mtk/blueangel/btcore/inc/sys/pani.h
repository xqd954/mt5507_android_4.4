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

#ifndef __PANI_H
#define __PANI_H
/****************************************************************************
 *
 * File:
 *     $Workfile:pani.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description:   This file specifies defines and function prototypes for the
 *                BIP application.
 * 
 * Created:       December 20, 2002
 *
 * $Project:XTNDAccess Blue SDK$
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

#include <pan.h>

/****************************************************************************
 *
 * Data Structures
 *
 ***************************************************************************/
#define PANBTPKT_FREE       0
#define PANBTPKT_INUSE      1
#define PANBTPKT_INUSE_INT  2
#define PANBTPKT_INUSE_EXT  3
#define PANUSER_BROADCAST   ((PanUser *)0xffffffff)
#define PANUSER_NULL        ((PanUser *)0xfffffffe)
#define PANUSER_SELF        ((PanUser *)0xfffffffd)
#define PANUSER_MULTICAST   ((PanUser *)0xfffffffc)


typedef struct _PanNettypeFilterNode
{
    ListEntry node;
    U16	nettypeStart;
    U16 	nettypeEnd;	
} PanNettypeFilterNode;

typedef struct _PanMulticastFilterNode
{
    ListEntry node;
    U8		multiaddrStart[6];
    U8 	multiaddrEnd[6];	
} PanMulticastFilterNode;

typedef struct _PanExtHdrNode
{
    ListEntry node;
    U8 *exthdr;	
} PanExtHdrNode;

typedef struct _PanAddrTbl
{
    BNEP_ADDR   macAddr;
    U32         ipAddr;
    U32         age;
} PanAddrTbl;


/* PAN Context */
typedef struct _PanContext
{
    ListEntry        panList;	/* each node in the list is a PanUser, it may be PANU, NAP or GN node */
    PanCallback     callback;           /* Application callback function */
    PanServiceType  service;            /* current active PAN service (connected) */
    //BNEP_ADDR       bnepAddr;           /* Local device BD_ADDR in BE order */
    BNEP_ADDR       macAddr;            /* Local device MAC Address */
    U32             ipAddr;             /* Local device IP Address */
    U32             ipBc;               /* IP broadcast address */
    U32             ipMask;             /* IP subnet mask */
    U32             ipNet;              /* IP network ID */
    //U32             dhcpAddr;           /* DHCP server address */
    //U32             gwAddr;             /* Default Gateway address */

    /* It is used when DHCP server does not exist */
    U32             linkLocalIpAddr;    /* Local device Link Local Address */
    PanAddrTbl      panAddr[NUM_PAN_ARP_ADDRS]; /* MAC to IP Address table */

    U8		serviceCount;		/* only useful if service registration is for one role only in one session */

} PanContext;

#if XA_CONTEXT_PTR == XA_ENABLED
extern PanContext *pancx;
#define PAN(s)  (pancx->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern PanContext pancx;
#define PAN(s)  (pancx.s)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */


#define pan_malloc(size)		get_ctrl_buffer(size)
#define pan_free(p)			do{if(p){free_ctrl_buffer(p); p = NULL;}}while(0)		


void PanCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status);
BtStatus PanStartServiceQuery(PanUser *pan, U16 uuid);
void PanSdpCallback(SdpQueryToken *sqt, U8 result, U8 serv_chn);
BOOL PanSetLinkLocalAddr(void);
void PanBnepCallback(BnepCallbackParms *);
BtStatus PanInitPanUser(PanUser *pan);
void     PanDeInitPanUser(PanUser *pan);

#if PAN_ROLE_NAP == XA_ENABLED
#define  PanGetNapInfo() (PANIP_GetAdapterInfo())
#endif

U32     PAN_ResolveIpAddr(U8 *addr);

void PanClearConnection(PanUser *pan, U16 status);
PanUser *PanNewPanuser(void);
void PanFreePanuser(PanUser *pan);

void PanRevertBdAddr(BD_ADDR *dstBdAddr, BD_ADDR *srcBdAddr);
PanUser *PanFindPanuser(BnepPanu *bnepPanu);
PanUser *PanFindIpAddr(U32 ipAddr);
PanUser *PanFindMacAddr(U8 *macAddr);
void PanUpdateArpTable(PanUser *pan, U8 *macAddr, U32 ipAddr);
BOOL PanCheckFilterDB(PanUser *pan, U8 *pkt, BOOL unicast);
PanBtPkt *PanFindFreeBtPkt(PanUser *pan);

#endif /* __PANI_H */
