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

#ifndef __PAN_PKT_H
#define __PAN_PKT_H
/*****************************************************************************
 *
 * File:
 *     $Workfile:pan_pkt.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: This file contains code for the PAN profile. 
 *
 * Created:     Feb 7, 2003
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

//#include <pshpack1.h>


#define PAN_EHTERTYPE_IP		0x0800	// Internet Protocol, Version 4 (IPv4)
#define PAN_EHTERTYPE_IPV6	0x86DD	// Internet Protocol, Version 6 (IPv6)
#define PAN_EHTERTYPE_ARP	0x0806	// Address Resolution Protocol (ARP)
#define PAN_EHTERTYPE_VLAN	0x8100	// VLAN-tagged frame (IEEE 802.1Q)

#define ARP_REQUEST	0x01
#define ARP_REPLY		0x02

#define IPPROTO_IP      0               /* dummy for IP */
#define IPPROTO_ICMP    1               /* Control Message Protocol */
#define IPPROTO_IPV4    4               /* IPv4 */
#define IPPROTO_IPV6    6               /* IPv6 */
#define IPPROTO_TCP     6               /* TCP */
#define IPPROTO_UDP     17              /* UDP */

#define ICMP_ECHO       8
#define ICMP_ECHOREPLY  0
#define ICMP_MIN        8   /* minimum icmp packet size (just header) */

#define BOOTP_SERVER    0x0043
#define BOOTP_CLIENT    0x0044
#define DHCPDISCOVER    1 
#define DHCPOFFER       2 
#define DHCPREQUEST     3 
#define DHCPDECLINE     4 
#define DHCPACK         5 
#define DHCPNAK         6 
#define DHCPRELEASE     7 

/* ---------------------------------------------------------------------------
 * ARP header
 */
typedef struct _ArpHeader {
    U8     macType[2];        /* MAC address type */
    U8     protoType[2];      /* Protocol address type */
    U8      macLen;         /* MAC address length */
    U8      protoLen;       /* Protocol address length */
    U8     op[2];             /* ARP operation (1=request, 2=reply) */
    U8      srcMac[6];      /* MAC source address */
    U8     srcIP[4];          /* IP source address */
    U8      dstMac[6];      /* MAC destination address */
    U8     dstIP[4];          /* IP destination address */
} ArpHeader;

/* ---------------------------------------------------------------------------
 * IP header
 */
typedef struct _IpHeader {
    U8      hdrLen:4;       /* length of the header in 32 bit units */
    U8      version:4;      /* Version of IP */
    U8      tos;            /* Type of service */
    U8     totalLen[2];       /* total length of the packet */
    U16     id;             /* unique identifier */
    U16     offset;         /* fragment offset */
    U8      ttl;            /* time to live */
    U8      proto;          /* protocol (TCP, UDP etc) */
    U16     cksum;          /* IP checksum */

    U8     srcIP[4];          /* source address */
    U8     dstIP[4];          /* destination address */
} IpHeader;

/* ---------------------------------------------------------------------------
 * ICMP header
 */
typedef struct _IcmpHeader {
    U8      type;
    U8      code;           /* type sub code */
    U16     cksum;
    U16     id;
    U16     seq;

    U32     timestamp;      /* Timestamp at the beginning of data */
} IcmpHeader;

/* ---------------------------------------------------------------------------
 * UDP header
 */
typedef struct _UdpHeader {
    U16     srcPort;        /* Source port # */
    U16     dstPort;        /* Destination port # */
    U16     len;            /* Udp packet length */
    U16     cksum;          /* Udp checksum (optional) */
} UdpHeader;

/* ---------------------------------------------------------------------------
 * DHCP header
 */
typedef struct _DhcpHeader {
    U8      op;
    U8      htype;
    U8      hlen;
    U8      hops;
    U32     xid;
    U16     secs;
    U16     flags;
    U32     ciaddr;
    U32     yiaddr;
    U32     siaddr;
    U32     giaddr;
    U8      chaddr[16];
    U8      sname[64];
    U8      file[128];
    U8      cookie[4];
    U8      options[312];
} DhcpHeader;

/* ---------------------------------------------------------------------------
 * TCP header
 */
typedef struct _TcpHeader {
    U16     srcPort;        /* Source port # */
    U16     dstPort;        /* Destination port # */
    U32     seq;
    U32     ack;
    U8      reserved:4;     /* reserved */
    U8      offset:4;       /* length of the header, in 32 bit units */
    U8      flags;
    U16     window;
    U16     cksum;          /* calculated from TCP Pseudo Header */
    U16     urgent;
    U8      options[0];
    /* 0 to 44 bytes of options may be placed at the end of the TCP header.
     * The options must be padded to make the TCP header length a multiple
     * of 32 bits.
     */
} TcpHeader;

/* ---------------------------------------------------------------------------
 * TCP Pseudo header (used for checksum calculation)
 */
typedef struct _TcpPseudoHeader {
    U32     srcIP;          /* Source IP address */
    U32     dstIP;          /* Destination IP address */
    U8      zero;
    U8      proto;          /* IP protocol */
    U16     len;            /* totoal length */
} TcpPseudoHeader;

//#include <poppack.h>
#endif /* __PAN_PKT_H */

