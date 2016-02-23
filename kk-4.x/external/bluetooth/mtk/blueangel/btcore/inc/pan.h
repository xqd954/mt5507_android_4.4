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

#ifndef __PAN_H
#define __PAN_H
/****************************************************************************
 *
 * File:
 *     $Workfile:pan.h$ for XTNDAccess Blue SDK, Version 1.3
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

#include <bttypes.h>
#include <bnep.h>
#include <sdp.h>
#include <sys/pan_pkt.h>
#include <conmgr.h>

/*---------------------------------------------------------------------------
 * Personal Area Networking (PAN) layer
 *
 *      The Personal Area Networking (PAN) profile specification defines
 *      roles to enable the creation of ad-hoc networks between two or more
 *      devices and/or a connection to a remote network through wireless
 *      Bluetooth connections. 
 */

/****************************************************************************
 *
 * Configuration Constants
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 *  NUM_BT_PKTS constant
 *      Specifies the number of BtPacket structures to reserve for active
 *      transmission links. Each data packet is encapsulated in a BtPacket
 *      structure. Sufficient BtPackets must be reserved depending on number
 *      of supported devices and transmission load.
 */
#define NUM_BT_PKTS                 5

/*---------------------------------------------------------------------------
 *  NUM_PAN_ARP_ADDRS constant
 *      Specifies the number of addresses to retain in the ARP cache.
 */
#define NUM_PAN_ARP_ADDRS           10

/*---------------------------------------------------------------------------
 *  PAN_LINK_LOCAL_SUBNET constant
 *      Specifies the initial Link local address seed. When DHCP is not
 *      available, the local IP address will be calculated from this
 *      value.
 */
#define PAN_LINK_LOCAL_SUBNET       0xA9FE0000      /* 169.254.0.0 */
                                 /* 0xC0A80000 */   /* 192.168.0.0 */

/*---------------------------------------------------------------------------
 *  PAN_ROLE_PANU constant
 *      Specifies whether the PANU role is enabled. The default is
 *      XA_ENABLED.
 */
#ifndef PAN_ROLE_PANU
#define PAN_ROLE_PANU               XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 *  PAN_ROLE_GN constant
 *      Specifies whether the GN role is enabled. The default is
 *      XA_ENABLED.
 */
#ifndef PAN_ROLE_GN
#define PAN_ROLE_GN                 XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 *  PAN_ROLE_NAP constant
 *      Specifies whether the NAP role is enabled. The default is
 *      XA_ENABLED.
 */
#ifndef PAN_ROLE_NAP
#define PAN_ROLE_NAP                XA_ENABLED
#endif

/****************************************************************************
 *
 * Section: Default SDP values that can be overridden in overide.h
 *
 ***************************************************************************/

//#define PAN_PANU_NUM_ATTRIBUTES     9
//#define PAN_GN_NUM_ATTRIBUTES       11
//#define PAN_NAP_NUM_ATTRIBUTES      13
#define PAN_NUM_SEARCH_ATTRIBUTES   21

/*---------------------------------------------------------------------------
 *  PAN_PKT_TYPE_LIST constant
 *      Specify Supported Network Packet types. Each type must be prefixed
 *      with the SDP_UINT_16BIT macro.
 */
#define PAN_PKT_TYPE_LIST \
    SDP_UINT_16BIT(0x0800),         /* Uint16 IPv4 */ \
    SDP_UINT_16BIT(0x0806)          /* Uint16 ARP */


/*---------------------------------------------------------------------------
 *  PAN_SECURITY_LEVEL constant
 *      0x0000: no security,
 *      0x0001: enforced Bluetooth security,
 *      0x0002: enforce 802.1X security.
 */
#define PAN_SECURITY_LEVEL      0x0000  /* No security */

/*---------------------------------------------------------------------------
 *  PAN_PANU_SERVICE_NAME constant
 *      Specify the PANU Service Name.
 */
#define PAN_PANU_SERVICE_NAME \
    'P','e','r','s','o','n','a','l',' ','A','d','-','h','o','c',' ', \
    'U','s','e','r',' ','S','e','r','v','i','c','e', 0x00

/*---------------------------------------------------------------------------
 *  PAN_PANU_SERVICE_DESCRIPTION constant
 *      Specify the PANU Service Description.
 */
#define PAN_PANU_SERVICE_DESCRIPTION \
    'P','e','r','s','o','n','a','l',' ','A','d','-','h','o','c',' ', \
    'U','s','e','r',' ','S','e','r','v','i','c','e', 0x00

/*---------------------------------------------------------------------------
 *  PAN_GN_SERVICE_NAME constant
 *      Specify the GN Service Name.
 */
#define PAN_GN_SERVICE_NAME \
    'G','r','o','u','p','A','d','-','h','o','c',' ', \
    'N','e','t','w','o','r','k',' ','S','e','r','v','i','c','e', 0x00

/*---------------------------------------------------------------------------
 *  PAN_GN_SERVICE_DESCRIPTION constant
 *      Specify the GN Service Description.
 */
#define PAN_GN_SERVICE_DESCRIPTION \
    'P','e','r','s','o','n','a','l',' ','G','r','o','u','p',' ', \
    'A','d','-','h','o','c',' ','N','e','t','w','o','r','k',' ', \
    'S','e','r','v','i','c','e', 0x00

/*---------------------------------------------------------------------------
 *  PAN_GN_IPV4_SUBNET constant
 *      Specify the IPv4 Subnet.
 */
#define PAN_GN_IPV4_SUBNET '1','0','.','0','.','0','.','0','/','8'

/*---------------------------------------------------------------------------
 *  PAN_GN_IPV6_SUBNET constant
 *      Specify the IPv6 Subnet.
 */
#define PAN_GN_IPV6_SUBNET 'f','e','8','0',':',':','/','4','8'

/*---------------------------------------------------------------------------
 *  PAN_NAP_SERVICE_NAME constant
 *      Specify the NAP Service Name.
 */
#define PAN_NAP_SERVICE_NAME \
    'N','e','t','w','o','r','k',' ','A','c','c','e','s','s',' ', \
    'P','o','i','n','t',' ','S','e','r','v','i','c','e', 0x00

/*---------------------------------------------------------------------------
 *  PAN_NAP_SERVICE_DESCRIPTION constant
 *      Specify the NAP Service Description.
 */
#define PAN_NAP_SERVICE_DESCRIPTION \
    'B','l','u','e','t','o','o','t','h',' ','N','A','P',' ', \
    'S','e','r','v','i','c','e', 0x00


/*---------------------------------------------------------------------------
 *  PAN_NAP_NET_ACCESS_TYPE constant
 *      0x0000: PSTN,
 *      0x0001: ISDN,
 *      0x0002: DSL,
 *      0x0003: Cable Modem,
 *      0x0004: 10Mb Ethernet,
 *      0x0005: 100Mb Ethernet,
 *      0x0006: 4Mb Token Ring,
 *      0x0007: 16Mb Token Ring,
 *      0x0008: 100Mb Token Ring,
 *      0x0009: FDDI,
 *      0x000A: GSM,
 *      0x000B: CDMA,
 *      0x000C: GPRS,
 *      0x000D: 3G,
 *      0xFFFE: other.
 */
#define PAN_NAP_NET_ACCESS_TYPE     0x0005      /* 100Mb Ethernet */

/*---------------------------------------------------------------------------
 *  PAN_NAP_MAX_NET_ACCESS_RATE constant
 *      Specify the maximum access rate for the network. This value is the
 *      the maximum data rate based on PAN_NAP_NET_ACCESS_TYPE.
 */
#define PAN_NAP_MAX_NET_ACCESS_RATE 10000000    /* 10Mb/sec */

/*---------------------------------------------------------------------------
 *  PAN_NAP_IPV4_SUBNET constant
 *      Specify the IPv4 Subnet.
 */
#define PAN_NAP_IPV4_SUBNET '1','0','.','0','.','0','.','0','/','8'

/*---------------------------------------------------------------------------
 *  PAN_NAP_IPV6_SUBNET constant
 *      Specify the IPv6 Subnet.
 */
#define PAN_NAP_IPV6_SUBNET 'f','e','8','0',':',':','/','4','8'


/*-------------------------------------------------------------------------
 *
 * SDP query info
 *
 * Service search attribute request for PAN.
 * The service search pattern is very specific. It contains the UUIDs
 * for PAN and L2CAP.
 *
 */

#ifndef AID_IPV4_SUBNET 
#define AID_IPV4_SUBNET                         0x030D
#endif

#ifndef AID_IPV6_SUBNET 
#define AID_IPV6_SUBNET                         0x030E
#endif

/****************************************************************************
 *
 * Types
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * PanEvent type
 *
 *     All event indications and confirmations are passed to a callback
 *     function of type PanCallback. The "PanCallbackParms.event" field will
 *     contain one of the event types below.
 */
typedef BnepEvent PanEvent;

/** Indicates an open request is in progress on a client. Use 
 *  PAN_SetupConnection to complete the connection with a BNEP Setup
 *  Connection control message. This event precedes the PANEVENT_OPEN
 *  event on a client.
 */
#define PANEVENT_SETUP_CONNECTION   0x81

/** Indicates an open request on a server. Use PAN_ControlResponse to
 *  complete the connection with a BNEP Setup Connection response. This
 *  event will follow a PANEVENT_OPEN event on a server.
 */
#define PANEVENT_SETUP_CONN_REQ     0x82

/** Indicates a BNEP control packet other than SETUP_CONNECTION has been
 *  received.
 */
#define PANEVENT_CONTROL_IND        0x83

/** Indicates an Open request is in progress on a server or complete on
 *  a client.
 */
#define PANEVENT_OPEN               0x87

/** Indicates the connection is closed.
 */
#define PANEVENT_CLOSED             0x88

/** Indicates an SDP service query failed on an open.
 */
#define PANEVENT_SDP_FAILED         0x89

/** Indicates a data packet is available. The parms.pkt parameter points
 *  to the packet. The packet is of type PanPacketType indicated in the
 *  parms.type parameter. The packet length is indicated in the
 *  parms.rxPktLen parameter.
 */
#define PANEVENT_DATA_IND           0x8a

/** Indicates the PanPacket used to send data has been released and may
 *  be freed or reused.
 */
#define PANEVENT_DATA_HANDLED       0x8b
/* End of PanEvent */

/* Forward reference to PanCallbackParms defined below */
typedef struct _PanCallbackParms PanCallbackParms;

/*---------------------------------------------------------------------------
 * PanCallback type
 *
 *      PAN Events and data are passed to the application through a callback
 *      function of this type defined by the application.
 */
typedef void (*PanCallback)(PanCallbackParms *parms);

/* End of PanCallback */

/*---------------------------------------------------------------------------
 * PanPacketType type
 *
 *      Indicates the packet type when sending or receiving a data packet.
 *      All packets include their associated IP header unless otherwise
 *      indicated.
 */
typedef BnepPacketType PanPacketType;

/* Packet type unknown.
 */
#define PANPKT_UNKNOWN              0x40

/* Packet type is General Ethernet and includes the MAC header. The packet
 * must be parsed to determine higher layer protocols. This packet is
 * normally forwarded to the remote network.
 */
#define PANPKT_ETHER_PKT            0x41

/* Packet is an ARP request.
 */
#define PANPKT_ARP_REQUEST          0x42

/* Packet is an ARP reply.
 */
#define PANPKT_ARP_REPLY            0x43

/* Packet type is IP.
 */
#define PANPKT_IP                   0x44

/* Packet is an ICMP request (Ping echo.)
 */
#define PANPKT_ICMP_REQUEST         0x45

/* Packet is an ICMP reply (Ping echo reply.)
 */
#define PANPKT_ICMP_REPLY           0x46

/* Packet type is TCP.
 */
#define PANPKT_TCP                  0x47

/* Packet type is UDP.
 */
#define PANPKT_UDP                  0x48

/* Packet is a DHCP request.
 */
#define PANPKT_DHCP_REQUEST         0x49

/* Packet is a DHCP reply.
 */
#define PANPKT_DHCP_REPLY           0x4a

/* Packet is a BNEP network filter (no IP header.)
 */
#define PANPKT_NET_FILTER           0x4b

/* Packet is a BNEP multicast filter (no IP header.)
 */
#define PANPKT_MCAST_FILTER         0x4c

/* Packet type is Compressed Ethernet with destination MAC only 
 */
#define PANPKT_ETHER_DEST_ONLY_PKT            0x4d

/* Packet type is Compressed Ethernet 
 */
#define PANPKT_ETHER_COMPRESSED_PKT	0x50

/* Packet type is Compressed Ethernet with source MAC only 
 */
#define PANPKT_ETHER_SRC_ONLY_PKT	0x51

/* Packet type is Ethernet with extension header 
 */
#define PANPKT_EXTENSION_HEADER_BIT   0x80
/* End of PanPacketType */

/*---------------------------------------------------------------------------
 * PanState type
 *
 *     PAN Connection state.
 */
typedef U8 PanState;

/* PAN is uninitialized.
 */
#define PAN_CLOSED    0x00

 /* Establishing a connection.
 */
#define PAN_OPENING          0x01

/* Establishing a connection.
 */
#define PAN_OPEN_IND         0x02

/* Connection is up.
 */
#define PAN_OPENED           0x03

/* Ping in progress.
 */
#define PAN_PING_REPLY       0x05// t.zh shall be unused

/* DHCP Discover in progress.
 */
#define PAN_DHCPDISCOVER     0x06// t.zh shall be unused

/* DHCP Offer in progress.
 */
#define PAN_DHCPOFFER        0x07// t.zh shall be unused

/* DHCP Acknowledge in progress.
 */
#define PAN_DHCPACK          0x08// t.zh shall be unused
/* End of PanState */

/*---------------------------------------------------------------------------
 * PanServiceType type
 *
 *      Describes the PAN Service type. Used when registering and on a
 *      connect operation.
 */
typedef U16 PanServiceType;

/* PANU Peer-to-Peer Service */
#define PANSVC_PANU        SC_PANU

/* GN Service */
#define PANSVC_GN          SC_GN

/* NAP Service */
#define PANSVC_NAP         SC_NAP
/* End of PanServiceType */

/****************************************************************************
 *
 * Section: Structures
 *
 ***************************************************************************/

/*----------------------------------------------------------------------
 * PanService structure
 *
 *  PAN service registration structure used to register a service
 *  through the PAN_Register function.
 */
typedef struct _PanService {
    PanCallback     callback;
    PanServiceType  type;
} PanService;
/* End of PanService */

/*----------------------------------------------------------------------
 * PanPacket structure
 *
 *  PAN Packet allocation structure.
 */
typedef struct _PanPacket {
    U8  header[BNEP_GNRL_HDR_SIZE];
    U8  body[0];
} PanPacket;
/* End of PanPacket */

/*----------------------------------------------------------------------
 * PanSetupConnPkt structure
 *
 *  PAN Setup Connection Request allocation structure.
 */
typedef struct _PanSetupConnPkt {
    U8  header[BNEP_CONTROL_HDR_SIZE];
    U8  uuidSize;
    U8  dstUuid[2];
    U8  srcUuid[2];
} PanSetupConnPkt;
/* End of PanSetupConnPkt */

/*----------------------------------------------------------------------
 * PanControlRespPkt structure
 *
 *  PAN Control response message allocation structure.
 */
typedef struct _PanControlRespPkt {
    U8  header[BNEP_CONTROL_HDR_SIZE];
    U8  message[2];
} PanControlRespPkt;
/* End of PanControlRespPkt */

/*----------------------------------------------------------------------
 * PanFilterPkt structure
 *
 *  PAN Network Protocol Filter request allocation structure.
 */
typedef struct _PanFilterPkt {
    U8  header[BNEP_CONTROL_HDR_SIZE];
    U8  listLen[2];
    U8  list[0];
} PanFilterPkt;
/* End of PanFilterPkt */

/* ---------------------------------------------------------------------------
 * BtPacket container
 */
typedef struct _PanBtPkt
{
    U16         context;
    BtPacket    btPkt;
} PanBtPkt;

/*--------------------------------------------------------------------------
 * PanUser structure
 *
 *      Maintains PANU connection info and status during an application
 *      session. A NAP, GN or PANU uses this structure to maintain
 *      information on connected PANU, NAP or GN. The PanUser structure is
 *      allocated by the profile and is passed into the SDK through
 *      the PAN_Open function by a client or Open ind by a
 *      server.
 */
typedef struct _PanUser {
    ListEntry         node;

    /* === Internal use only === */
    BnepPanu        bnepPanu;       /* remote BnepPanu */
    BNEP_ADDR       destMac;        /* storage for dest MAC address */
    PanState        state;
    PanBtPkt        pktPool[NUM_BT_PKTS];       /* BtPacket pool */

    /* Filter list */
    ListEntry	nettypeFilterList;
    ListEntry	multicastFilterList;

    /* Unknown extension header list */
    ListEntry 	exthdrList;	

    CmgrHandler       cmgrHandler;

    /* NAP */
    U32             remIpAddr;              
    //U32             remAddrContext;              

    /* SDP Client */
    PanServiceType  dstService;
    SdpQueryToken   sdpQueryToken;
    //U8              sdpSrchAttrib[PAN_NUM_SEARCH_ATTRIBUTES];
} PanUser;
/* End of PanUser */

/*---------------------------------------------------------------------------
 * PanCallbackParms structure
 *
 *      Describes a callback event and any data that relates to the event.
 */
struct _PanCallbackParms
{
    /* Completion status based on the event type. Reason will be
     * L2capDiscReason for PANEVENT_CLOSED events, BnepFilterResponseMsg for PANEVENT_CONTROL_IND
     * event or BtStatus for others.
     */
    U16             status;

    PanEvent        event;      /* Type of PANEVENT_XX event */

    /* Handle to the PanUser structure associated with this event. Valid
     * after a connection is established.
     */
    PanUser        *pan;

    /* Type of service that currently connected. Valid on
     * PANEVENT_OPEN and PANEVENT_SETUP_CONN_REQ events.
     */
    PanServiceType	service;

    /* Type of packet associated with this event. Valid on
     * PANEVENT_DATA_HANDLED and PANEVENT_DATA_IND events.
     */
    PanPacketType   type;

    /* On PANEVENT_DATA_HANDLED event, points to the sent data packet.
     * On PANEVENT_DATA_IND event, points to the received packet.
     * The PanPacketType field above indicates the type.
     */
    U8             *pkt;

    /* Length of received data packet. Valid on
     * PANEVENT_DATA_IND events.
     */
    U16             rxPktLen;

};
/* End of PanCallbackParms */

/****************************************************************************
 *
 * Function Reference
 *
 ***************************************************************************/

/****************************************************************************
 *
 * Section: Common functions for Client or Server.
 *
 ***************************************************************************/

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
BOOL PAN_Init(void);

/*---------------------------------------------------------------------------
 * PAN_Register()
 *
 *      Register a callback for events associated with the PanService. An
 *      application may register for only one service in a session.
 *
 * Parameters:
 *      service - pointer to PanService structure with the PanCallback and
 *                PanServiceType fields initialized for the desired service.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The registration succeeded.
 *
 *      BT_STATUS_FAILED - The registration failed.
 */
BtStatus PAN_Register(PanService *service);

/*---------------------------------------------------------------------------
 * PAN_Deregister()
 *
 *      Deregister an application from the PanService.
 *
 * Parameters:
 *      service - pointer to PanService structure previously registered with
 *                PAN_Register with the PanCallback and PanServiceType fields
 *                set to the service to be removed.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The deregistration succeeded.
 *
 *      BT_STATUS_FAILED - The deregistration failed.
 */
BtStatus PAN_Deregister(PanService *service);

/*---------------------------------------------------------------------------
 * PAN_Close()
 *
 *      Close the connection associated with the PanUser structure.
 *
 * Parameters:
 *      pan - handle to the PanUser structure used to maintain the
 *          connection.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The close succeeded.
 *
 *      BT_STATUS_FAILED - The close failed.
 */
BtStatus PAN_Close(PanUser *pan);

/*---------------------------------------------------------------------------
 * PAN_SendPkt()
 *
 *      Send a data packet created by the application. A PANEVENT_DATA_HANDLED
 *      will be sent to the registered callback to notify when the
 *      packet can be returned to the memory pool.
 *
 * Parameters:
 *      pan - handle to the PanUser structure used to maintain the
 *          connection.
 *      panPkt - pointer to the data packet encapsulated in the PanPacket
 *          structure.
 *      type - data packet type.
 *      len - length of data packet.
 *
 * Returns:
 *      BT_STATUS_PENDING - The send succeeded.
 *
 *      BT_STATUS_FAILED - The send failed.
 */
BtStatus PAN_SendPkt(PanUser *pan, PanPacket *panPkt, PanPacketType type, U16 len);

/*---------------------------------------------------------------------------
 * PAN_SendArp()
 *
 *      Send an ARP to resolve the specified IP address to a MAC address.
 *
 * Parameters:
 *      pan - handle to the PanUser structure associated with this address.
 *      destIP - IP address to resolve to a MAC address.
 *
 * Returns:
 *      The MAC address in a BNEP_ADDR structure.
 */
BNEP_ADDR  *PAN_SendArp(U32 destIP);

/*---------------------------------------------------------------------------
 * PAN_GetPanUserState()
 *
 *      Get the current state of the PanUser.
 *
 * Parameters:
 *      pan - handle to the PanUser structure associated with this address.
 *
 * Returns:
 *      The PanState of the referenced PanUser.
 */
PanState PAN_GetPanUserState(PanUser *pan);
#define  PAN_GetPanUserState(a) (a->state)

/*---------------------------------------------------------------------------
 * PAN_SetRemoteMacAddr()
 *
 *      Set the MAC address of the remote PAN device.
 *
 * Parameters:
 *      pan - handle to the PanUser structure used to maintain the
 *          connection.
 *      macAddr - MAC address of remote PAN device.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The registration succeeded.
 *
 *      BT_STATUS_FAILED - The registration failed.
 */
BtStatus PAN_SetRemoteMacAddr(PanUser *pan, U8 *macAddr);

/*---------------------------------------------------------------------------
 * PAN_SetLocalMacAddr()
 *
 *      Set the MAC address of the local PAN device. It is necessary if
 *      the network device is existed.
 *
 * Parameters:
 *      pan - handle to the PanUser structure used to maintain the
 *          connection.
 *      macAddr - MAC address of remote PAN device.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The registration succeeded.
 *
 *      BT_STATUS_FAILED - The registration failed.
 */
BtStatus PAN_SetLocalMacAddr(U8 *macAddr);


#include <sys/pani.h>
/*---------------------------------------------------------------------------
 * PAN_GetLocalBnepAddr()
 *
 *      Get the local BNEP address.
 *
 * Parameters:
 *      None.
 *
 * Returns:
 *      A pointer to a BNEP_ADDR structure containing the local BNEP address.
 */
BNEP_ADDR *PAN_GetLocalBnepAddr();
#define  PAN_GetLocalBnepAddr() (&PAN(bnepAddr))

/*---------------------------------------------------------------------------
 * PAN_GetLocalMacAddr()
 *
 *      Get the local MAC address.
 *
 * Parameters:
 *      None.
 *
 * Returns:
 *      A pointer to a BNEP_ADDR structure containing the local MAC address.
 */
U8 *PAN_GetLocalMacAddr();
#define  PAN_GetLocalMacAddr() (PAN(macAddr.addr))

/*---------------------------------------------------------------------------
 * PAN_SetLocalIpAddr()
 *
 *      Set the local IP address.
 *
 * Parameters:
 *      addr - local IP address.
 *
 * Returns:
 *      None.
 */
void     PAN_SetLocalIpAddr(U32 addr);
#define  PAN_SetLocalIpAddr(a) (StoreLE32((U8 *)&PAN(ipAddr), (a)))

/*---------------------------------------------------------------------------
 * PAN_GetLocalIpAddr()
 *
 *      Get the local IP address.
 *
 * Parameters:
 *      None
 *
 * Returns:
 *      The local IP address as an U32.
 */
U32      PAN_GetLocalIpAddr();
#define  PAN_GetLocalIpAddr() (BEtoHost32((U8 *)&PAN(ipAddr)))

/*---------------------------------------------------------------------------
 * PAN_SetRemoteIpAddr()
 *
 *      Set the remote IP address.
 *
 * Parameters:
 *      pan - handle to the PanUser structure used to maintain the
 *          connection.
 *      addr - remote IP address.
 *
 * Returns:
 *      None.
 */
void     PAN_SetRemoteIpAddr(PanUser *pan, U32 addr);
#define  PAN_SetRemoteIpAddr(a,b) (StoreBE32((U8 *)&(a->remIpAddr), (b)))

/*---------------------------------------------------------------------------
 * PAN_GetRemoteIpAddr()
 *
 *      Get the remote IP address.
 *
 * Parameters:
 *      pan - handle to the PanUser structure used to maintain the
 *          connection.
 *
 * Returns:
 *      The remote IP address as an U32.
 */
U32      PAN_GetRemoteIpAddr(PanUser *pan);
#define  PAN_GetRemoteIpAddr(a) (BEtoHost32((U8 *)&(a->remIpAddr)))

/*---------------------------------------------------------------------------
 * PAN_SetDhcpAddr()
 *
 *      Set the address to use for DHCP requests.
 *
 * Parameters:
 *      addr - DHCP address to use.
 *
 * Returns:
 *      None.
 */
void     PAN_SetDhcpAddr(U32 addr);
#define  PAN_SetDhcpAddr(a) (StoreBE32((U8 *)&PAN(dhcpAddr), (a)))

/*---------------------------------------------------------------------------
 * PAN_GetDhcpAddr()
 *
 *      Get the address to use for DHCP requests.
 *
 * Parameters:
 *      None.
 *
 * Returns:
 *      DHCP address to use as an U32.
 */
U32      PAN_GetDhcpAddr();
#define  PAN_GetDhcpAddr() (BEtoHost32((U8 *)&PAN(dhcpAddr)))

/*---------------------------------------------------------------------------
 * PAN_ResetLinkLocalAddr()
 *
 *      Recalculate the link local address. Typically used when a collision
 *      with current link local address is detected.
 *
 * Parameters:
 *      None.
 *
 * Returns:
 *      None. Use PAN_GetLocalIpAddr to retrieve the new address.
 */
void     PAN_ResetLinkLocalAddr();
#define  PAN_ResetLinkLocalAddr() (PanSetLinkLocalAddr())

/*---------------------------------------------------------------------------
 * PAN_PanUserRouted()
 *
 *      Indicate the router table state of the PanUser.
 *
 * Parameters:
 *      pan - handle to the PanUser structure in the Router table.
 *
 * Returns:
 *      TRUE - The PanUser IP address is in the router table.
 *
 *      FALSE - The PanUser IP address is not in the router table.
 */
BOOL PAN_PanUserRouted(PanUser *pan);
#define  PAN_PanUserRouted(a) (a->remAddrContext != 0)

/****************************************************************************
 *
 * Section: Server functions.
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * PAN_Listen()
 *
 *      Make a PanUser available for receiving open connection requests.
 *
 * Parameters:
 *      pan - handle to the PanUser structure used to maintain the
 *          connection.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The listen succeeded.
 *
 *      BT_STATUS_FAILED - The listen failed.
 */
BtStatus PAN_Listen(PanUser *pan);

/*---------------------------------------------------------------------------
 * PAN_ControlResponse()
 *
 *      Send a BNEP Setup Connection message from a server to complete an
 *      open connection request.
 *
 * Parameters:
 *      pan - handle to the PanUser structure used to maintain the
 *          connection.
 *      type - BNEP Control response type.
 *      controlRespPkt - control response packet.
 *
 * Returns:
 *      BT_STATUS_PENDING - The send succeeded.
 *
 *      BT_STATUS_FAILED - The send failed.
 */
BtStatus PAN_ControlResponse(PanUser *pan, BnepControlType type,
                             PanControlRespPkt *controlRespPkt, U16 len);

/*---------------------------------------------------------------------------
 * PAN_ForwardPkt()
 *
 *      Forward a data packet received from the BNEP protocol stack. The
 *      packet must include the Ethernet MAC header. No PANEVENT_DATA_HANDLED
 *      event will follow this function, as it was received from the BNEP
 *      stack and does not require allocation from the application memory
 *      pool.
 *
 * Parameters:
 *      pan - handle to the PanUser structure used to maintain the
 *          connection.
 *      pkt - pointer to the Ethernet data packet received from the BNEP
 *          stack.
 *      len - length of data packet.
 *
 * Returns:
 *      BT_STATUS_PENDING - The send succeeded.
 *
 *      BT_STATUS_FAILED - The send failed.
 */
BtStatus PAN_ForwardPkt(U8 *pkt, U16 len, U8 *exthdr, U16 exthdrlen);

/*---------------------------------------------------------------------------
 * PAN_AddRouterAddr()
 *
 *      Associate an IP address with a PanUser in the router table. The IP
 *      address of the PanUser must be set prior to this call with
 *      PAN_SetRemoteIpAddr(). PAN_PanUserRouted() may be used to check if
 *      the PanUser is already in the router table.
 *
 * Parameters:
 *      pan - handle to the PanUser structure associated with this address.
 *
 * Returns:
 *      None.
 */
void PAN_AddRouterAddr(PanUser *pan);

/*---------------------------------------------------------------------------
 * PAN_DeleteRouterAddr()
 *
 *      Delete an IP address associated with a PanUser from the router
 *      table.
 *
 * Parameters:
 *      ipAddr - IP address in host byte order to delete from router table.
 *
 * Returns:
 *      None.
 */
void PAN_DeleteRouterAddr(U32 ipAddr);

/****************************************************************************
 *
 * Section: Client functions.
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * PAN_Open()
 *
 *      Initiate an open request for a PAN service connection.
 *
 * Parameters:
 *      remDev - The target remote device for the connection.
 *      pan - handle to the PanUser structure used to maintain the
 *          connection.
 *      tgtService - Service required for the connection.
 *
 * Returns:
 *      BT_STATUS_PENDING - The open request succeeded.
 *
 *      BT_STATUS_FAILED - The open request failed.
 */
BtStatus PAN_Open(PanUser **panChannel, PanServiceType srcService, PanServiceType dstService, BD_ADDR *addr);

/*---------------------------------------------------------------------------
 * PAN_SetupConnection()
 *
 *      Send a BNEP Setup Connection message from a client to complete an
 *      open connection request.
 *
 * Parameters:
 *      pan - handle to the PanUser structure used to maintain the
 *          connection.
 *      setupPkt - Setup connection packet.
 *
 * Returns:
 *      BT_STATUS_PENDING - The send succeeded.
 *
 *      BT_STATUS_FAILED - The send failed.
 */
BtStatus PAN_SetupConnection(PanUser *pan, PanSetupConnPkt *setupPkt);

/*---------------------------------------------------------------------------
 * PAN_FilterProtocol()
 *
 *      Send a BNEP Network Protocol Filter message from a client to
 *      request protocol filters on a connection.
 *
 * Parameters:
 *      pan - handle to the PanUser structure used to maintain the
 *          connection.
 *      protoPkt - pointer to Network Protocol filter packet.
 *
 * Returns:
 *      BT_STATUS_PENDING - The send succeeded.
 *
 *      BT_STATUS_FAILED - The send failed.
 */
BtStatus PAN_FilterProtocol(PanUser *pan, PanFilterPkt *protoPkt);

/*---------------------------------------------------------------------------
 * PAN_FilterMulticast()
 *
 *      Send a BNEP Multicast Filter message from a client to
 *      request multicast filters on a connection.
 *
 * Parameters:
 *      pan - handle to the PanUser structure used to maintain the
 *          connection.
 *      multiPkt - pointer to multicast filter packet.
 *
 * Returns:
 *      BT_STATUS_PENDING - The send succeeded.
 *
 *      BT_STATUS_FAILED - The send failed.
 */
BtStatus PAN_FilterMulticast(PanUser *pan, PanFilterPkt *multiPkt);

#endif /* __PAN_H */
