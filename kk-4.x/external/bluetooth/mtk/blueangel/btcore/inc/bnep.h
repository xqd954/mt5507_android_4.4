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

#ifdef __BT_BNEP__

#ifndef __BNEP_H
#define __BNEP_H
/*****************************************************************************
 *
 * File:
 *     $Workfile:bnep.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: Public types, defines, and prototypes for accessing the
 *              BNEP layer.
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

#include <btconfig.h>
#include <bttypes.h>
#include <eventmgr.h>
#include <l2cap.h>
#include <sec.h>
#include <me.h>

/*---------------------------------------------------------------------------
 * Bluetooth Network Encapsulation Protocol (BNEP) layer
 *
 *     BNEP is the protocol specified for the Personal Area
 *     Networking (PAN) profile. It is a method of encapsulating IEEE
 *     802.3/Ethernet networking protocols in L2CAP packets to be transported
 *     directly over the Bluetooth L2CAP protocol. This module of the Blue
 *     SDK provides the BNEP interfaces used with the PAN profile.
 *     
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * NUM_BNEP_PANUS constant
 * 
 *     Defines the number of concurrent PANU connections supported by this 
 *     device. Multiple concurrent connections would typically be configured
 *     for a NAP or GN. A single connection would typically be configured for
 *     a PANU.
 *
 *     The default value is 1, and the maximum is limited to 255.
 */
#ifndef NUM_BNEP_PANUS
#define NUM_BNEP_PANUS 7
#endif

#if NUM_BNEP_PANUS < 1
#error NUM_BNEP_PANUS must be greater than 0
#endif

#if NUM_BNEP_PANUS > 255
#error NUM_BNEP_PANUS must be less than 256
#endif

/*---------------------------------------------------------------------------
 * BNEP_ETHERNET_EMULATION constant
 * 
 *     Defines the emulation mode of BNEP. When BNEP_ETHERNET_EMULATION is
 *     enabled, BNEP becomes a virtual ethernet device, compressing BD_ADDRs
 *     out of the Ethernet header as appropriate. BNEP_ETHERNET_EMULATION
 *     requires L2CAP_PRELUDE_SIZE be set to at least 7 in the overide.h
 *     file. If BNEP_ETHERNET_EMULATION is disabled, BNEP expects preformatted
 *     headers and L2CAP_PRELUDE_SIZE is not required.
 *
 *     The default value is XA_ENABLED.
 */
#ifndef BNEP_ETHERNET_EMULATION
#define BNEP_ETHERNET_EMULATION XA_ENABLED
#endif

#if BNEP_ETHERNET_EMULATION == XA_ENABLED && L2CAP_PRELUDE_SIZE < 7
#error L2CAP_PRELUDE_SIZE must be at least 7
#endif

/*---------------------------------------------------------------------------
 * BNEP_NUM_TIMERS constant
 *
 *     Defines the number of timers available for signaling commands (Control
 *     Response Msg.)
 *
 *     The default value is 3 timers.
 */
#ifndef BNEP_NUM_TIMERS
#define BNEP_NUM_TIMERS     3
#endif

#if BNEP_NUM_TIMERS < 1
#error BNEP_NUM_TIMERS must be at least 1.
#endif

/*---------------------------------------------------------------------------
 * BNEP_CONTROL_TIMEOUT constant
 *
 *     Defines the response timeout value for signaling commands (Control
 *     Response Msg), in seconds. If the timer elapses without a response
 *     from the remote device, a timeout event is generated.
 *
 *     The timeout value may range from 1 to 30 seconds. The default value
 *     is 10 seconds.
 */
#ifndef BNEP_CONTROL_TIMEOUT
#define BNEP_CONTROL_TIMEOUT     20
#endif

#if BNEP_CONTROL_TIMEOUT < 1 || BNEP_CONTROL_TIMEOUT > 30
#error BNEP_CONTROL_TIMEOUT must be between 1 and 30 seconds.
#endif

/*---------------------------------------------------------------------------
 * Check the Maximum L2CAP MTU size.
 * 
 */
#if L2CAP_MTU < 1691
#error BNEP requires maximum L2CAP_MTU be at least 1691 bytes
#endif

/*---------------------------------------------------------------------------
 * Check the BT_PACKET_HEADER_LEN size.
 * 
 */
#if BT_PACKET_HEADER_LEN  <  (8+6+5)
#error BNEP requires BT_PACKET_HEADER_LEN be at least 19 bytes
#endif

#define ETHER_DATA_SIZE         1500
#define ETHER_HDR_SIZE          (BNEP_ADDR_SIZE*2 + sizeof(BnepEthernetProtocolType))
#define ETHER_RANGE_SIZE        (sizeof(BnepEthernetProtocolType)*2)
#define MAX_NUM_PROTOCOL_RANGES (ETH_DATA_SIZE / ETHER_RANGE_SIZE)  /* 1500 / 4 = 375 */

#define BNEP_TYPE_SIZE          sizeof(BnepPacketType)
#define BNEP_GNRL_HDR_SIZE      (BNEP_TYPE_SIZE + ETHER_HDR_SIZE)
#define BNEP_CONTROL_TYPE_SIZE  sizeof(BnepControlType)
#define BNEP_CONTROL_HDR_SIZE   (BNEP_TYPE_SIZE + BNEP_CONTROL_TYPE_SIZE)
#define BNEP_UUID_LENGTH_SIZE   sizeof(U8)
#define BNEP_LIST_LENGTH_SIZE   sizeof(U16)
#define BNEP_ADDR_SIZE          BD_ADDR_SIZE
#define BNEP_EXT_HDR_SIZE       (sizeof(BnepExtHeaderType) + sizeof(U8))
#define BNEP_E_BIT              0x80

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BnepEvent type
 *
 *     All event indications and confirmations are passed to a callback
 *     function of type BnepCallback. The "BnepCallbackParms.event" field will
 *     contain one of the event types below.
 */
typedef U8 BnepEvent;

/** A remote device has requested a connection to a local BNEP service.
 *  During processing of this event, allocate a panu and call BNEP_Open
 *  to indicate whether the panu should be opened.
 */
#define BNEPEVENT_OPEN_IND              1

/** A panu is now open and ready for data exchange.
 */
#define BNEPEVENT_OPEN                  2 

/* ToDo: Do we need the Close_Ind? Currently not implemented */
/** A request to close a panu was received.
 */
#define BNEPEVENT_CLOSE_IND             3

/** The panu is closed.
 */
#define BNEPEVENT_CLOSED                4

/** A packet was received from the remote device. The "rxPktLen"
 *  field indicates the packet size and the "pkt" union points
 *  to the packet received. The packet type is indicated by
 *  the "type" field in the return parms as well as by the first
 *  byte of the packet header for BNEP packets.
 */
#define BNEPEVENT_PACKET_IND            5

/** BNEP is finished with the data packet passed to BNEP_SendPkt or
 *  BNEP_Control. The panuser may reuse or free the packet structure.
 *
 *  The "status" field indicates whether the packet was sent successfully.
 */
#define BNEPEVENT_PACKET_HANDLED        6

/** BNEP received a timeout on the response for a Control command. The
 *  link is most likely down as L2CAP provides a reliable connection.
 */
#define BNEPEVENT_CONTROL_TIMEOUT    7

/** An extension header was received from the remote device. This event
 *  occurs only in BNEP_ETHERNET_EMULATION mode. The "pkt.extHdr" field
 *  points to the header received and the header type is indicated by the
 *  "type" field in the return parms as well as by the first byte of the
 *  header.
 */
#define BNEPEVENT_EXTENSION_HEADER_IND    8

/* End of BnepEvent */

/* Forward reference to BnepCallbackParms defined below */
typedef struct _BnepCallbackParms BnepCallbackParms;

/*---------------------------------------------------------------------------
 * BnepCallback type
 *
 *     BNEPEvents and data are passed to the application through a callback
 *     function of this type defined by the application.
 */
typedef void (*BnepCallback)(BnepCallbackParms *parms);

/* End of BnepCallback */

/*---------------------------------------------------------------------------
 * BnepState type
 */
typedef U8 BnepState;

#define BNEP_STATE_CLOSED           0
#define BNEP_STATE_CLOSE_IND        1
#define BNEP_STATE_CLOSE_PENDING    2
#define BNEP_STATE_OPEN             3
#define BNEP_STATE_OPEN_IND         4

/* End of BnepState */

/*---------------------------------------------------------------------------
 * BnepPacketType type
 *
 *      The high order bit of the BNEP packet type is the Extension Flag. The
 *      one bit Extension Flag indicates if one or more extension headers
 *      follow the BNEP header before the data payload.
 *
 */
typedef U8 BnepPacketType;

#define BNEP_GENERAL_ETHERNET                   0x00
#define BNEP_CONTROL                            0x01
#define BNEP_COMPRESSED_ETHERNET                0x02
#define BNEP_COMPRESSED_ETHERNET_SOURCE_ONLY    0x03
#define BNEP_COMPRESSED_ETHERNET_DEST_ONLY      0x04

/* End of BnepPacketType */

/*---------------------------------------------------------------------------
 * BnepExtHeaderType type
 *
 */
typedef U8 BnepExtHeaderType;

#define BNEP_EXTENSION_CONTROL   0x00

/* End of BnepExtHeaderType */

/*---------------------------------------------------------------------------
 * BnepControlType type
 *
 */
typedef U8 BnepControlType;

#define BNEP_CONTROL_COMMAND_NOT_UNDERSTOOD 0x00
#define BNEP_SETUP_CONNECTION_REQUEST_MSG   0x01
#define BNEP_SETUP_CONNECTION_RESPONSE_MSG  0x02
#define BNEP_FILTER_NET_TYPE_SET_MSG        0x03
#define BNEP_FILTER_NET_TYPE_RESPONSE_MSG   0x04
#define BNEP_FILTER_MULTI_ADDR_SET_MSG      0x05
#define BNEP_FILTER_MULTI_ADDR_RESPONSE_MSG 0x06


//Paul 20110503 
#define BNEP_RETRANSMISSION_ENABLE 0x01
#define BNEP_RETRANSMISSION_TIMES 0x01
/* End of BnepControlType */

/*---------------------------------------------------------------------------
 * BnepSetupResponseMsg type
 *
 */
typedef U16 BnepSetupResponseMsg;

#define BNEP_SR_OPERATION_SUCCESS   0x0000
#define BNEP_SR_INVALID_DEST_UUID   0x0001
#define BNEP_SR_INVALID_SRC_UUID    0x0002
#define BNEP_SR_INVALID_UUID_SIZE   0x0003
#define BNEP_SR_OPERATION_FAILED    0x0004

/* End of BnepSetupResponseMsg */

/*---------------------------------------------------------------------------
 * BnepFilterResponseMsg type
 *
 */
typedef U16 BnepFilterResponseMsg;

#define BNEP_FR_OPERATION_SUCCESS   0x0000
#define BNEP_FR_UNSUPPORTED         0x0001
#define BNEP_FR_INVALID_PROTOCOL    0x0002
#define BNEP_FR_FAILED_MAX_FILTERS  0x0003
#define BNEP_FR_FAILED_SECURITY     0x0004

/* End of BnepFilterResponseMsg */

/*---------------------------------------------------------------------------
 * BnepEthernetProtocolType type
 *
 *  Some of the more common Ethernet Protocol types in BNEP packets.
 */
typedef U16 BnepEthernetProtocolType;

#define ETHER_TYPE_IPV4         0x0800
#define ETHER_TYPE_ARP          0x0806
#define ETHER_TYPE_IPV6         0x86DD

/* End of BnepEthernetProtocolType */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BNEP_ADDR structure
 *
 *      Represents a 48-bit Bluetooth or Ethernet device address.
 */
typedef struct _BNEP_ADDR
{
    U8    addr[BNEP_ADDR_SIZE];
}   BNEP_ADDR;

/*---------------------------------------------------------------------------
 * BnepPanu structure
 *
 *      Maintains connection information for a PANU channel.
 */
typedef struct _BnepPanu {
    BNEP_ADDR       bnepAddr;       /* BNEP_ADDR of remote device */
    U16             l2ChannelId;    /* L2CAP channel ID for connection */
    BnepState       state;          /* PANU state */
    BtRemoteDevice *remDev;         /* Remote device of connection */
    EvmTimer        timer[BNEP_NUM_TIMERS]; /* Timers for Control commands */
    BtPacket		controlPkt[BNEP_NUM_TIMERS];	 /* 20110504 Paul   Store control packet for retransmission */	
    U8		retransmission[BNEP_NUM_TIMERS]; /* 20110504 Paul  Remain retransmission times*/	
    TimeT           controlTimeout; /* Timeout value for Control commands */
}   BnepPanu;

/*---------------------------------------------------------------------------
 * BnepCallbackParms structure
 *
 *     Describes a callback event and any data that relates to the event.
 *     Fields in the BnepPanu structure are valid depending on the "event"
 *     field.
 */
struct _BnepCallbackParms
{
    /* Completion status based on the event type. Reason will be
     * L2capDiscReason for BNEPEVENT_CLOSED events or BtStatus for others.
     */
    U16             status;

    BnepEvent       event;      /* Type of BNEP event */

    /* Group: The following fields are used for connection management. */
    BnepPanu       *panu;       /* Valid after a connection is established */
    U16             l2ChannelId;/* Valid only on a BNEPEVENT_OPEN_IND event */
    BtRemoteDevice *remDev;     /* Valid only on a BNEPEVENT_OPEN_IND event */


    /* Group: The following fields are valid for BNEPEVENT_PACKET_HANDLED
     * and/or BNEPEVENT_PACKET_IND events
     */

    /* Valid on a BNEPEVENT_PACKET_HANDLED and BNEPEVENT_PACKET_IND events. */
    BnepPacketType  type;

    /* Points to the sent BtPacket. Valid on BNEPEVENT_PACKET_HANDLED
     * events.
     */
    BtPacket       *txPkt;

    /* On a BNEPEVENT_PACKET_HANDLED event, points to the sent BNEP or Ethernet
     * packet. On a BNEPEVENT_PACKET_IND event, points to the received packet.
     * The type is indicated by the BnepPacketType field above.
     */
    U8             *pkt;


    U8             *pktExtHdr;
    /* Length of received BNEP or Ethernet packet. Valid on
     * BNEPEVENT_PACKET_IND events.
     */
    U16             rxPktLen;

};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BNEP_Init()
 *
 *     Registers an application callback to receive BNEP events. This 
 *     function must be called before any other BNEP functions.
 *
 * Parameters:
 *     callback - Identifies the application function that will be called
 *        with BNEP events.
 *
 * Returns:
 *     TRUE - BNEP was successfully initialized.
 *
 *     FALSE  - BNEP failed to initialize.  This can
 *              happen when BNEP cannot be registered with L2CAP.
 */
BOOL BNEP_Init(void);

/*---------------------------------------------------------------------------
 * BNEP_DeInit()
 *
 *     De-initializes the BNEP layer.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - BNEP was successfully deinitialized.
 * 
 *     BT_STATUS_FAILED  - BNEP was not previously initialized
 *         (XA_ERROR_CHECK only).
 */
BtStatus BNEP_DeInit(void);

/*---------------------------------------------------------------------------
 * BNEP_Register()
 *
 *      Registers an application callback to receive BNEP events. This 
 *      function must be called before any other BNEP functions.
 *
 * Parameters:
 *      callback - Identifies the application function that will be called
 *        with BNEP events.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The BNEP application callback Function was
 *      successfully registered.
 *
 *      BT_STATUS_FAILED  - BNEP callback failed to register.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 *      BT_STATUS_NO_RESOURCES - The BNEP callback was already registered. 
 */
BtStatus BNEP_Register(BnepCallback Callback);

/*---------------------------------------------------------------------------
 * BNEP_DeRegister()
 *
 *      De-registers the BNEP callback. After making this call
 *      successfully, the callback specified in BNEP_Register will
 *      receive no further events.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The BNEP callback was successfully deregistered.
 * 
 *      BT_STATUS_FAILED  - A BNEP callback was not previously registered
 *         (XA_ERROR_CHECK only).
 */
BtStatus BNEP_DeRegister(void);

/*---------------------------------------------------------------------------
 * BNEP_Open()
 *
 *      Opens a BNEP panu with a remote device. After getting
 *      a response from the remote device, the callback function will receive
 *      a BNEPEVENT_OPEN event.
 *
 * Parameters:
 *      remDev - Identifies the remote device. 
 *      panu - A pointer to a new BnepPanu structure 
 *            that will be used to maintain this panu connection.
 *
 * Returns:
 *      BT_STATUS_PENDING - The process to start the BNEP panu 
 *         service was successful.
 *
 *      BT_STATUS_FAILED  - Either BNEP_Register() was not called first or
 *         the BNEP panu has already been opened.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus BNEP_Open(BtRemoteDevice *remDev, BnepPanu *panu);

/*---------------------------------------------------------------------------
 * BNEP_Accept()
 *
 *      Accepts a BNEP panu connection from a remote device in response to a
 *      BNEPEVENT_OPEN_IND event. After completing the open, the callback
 *      function will receive a BNEPEVENT_OPEN event.
 *
 * Parameters:
 *      panu - A pointer to a new BnepPanu structure that will be used to
 *            maintain this panu connection. Pass in a null pointer to deny
 *            the connection.
 *      l2ChannelId - L2CAP channel ID obtained from the BNEP_OPEN_IND event.
 *      remDev - Remote device obtained from the BNEP_OPEN_IND event.
 *
 * Returns:
 *      BT_STATUS_PENDING - The process to accept the BNEP panu 
 *         connection was successful.
 *
 *      BT_STATUS_FAILED  - Either BNEP_Register() was not called first or
 *         the BNEP panu has already been opened.
 *
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus BNEP_Accept(BnepPanu *panu, U16 l2ChannelId, BtRemoteDevice *remDev);

/*---------------------------------------------------------------------------
 * BNEP_Close()
 *
 *      Closes the given BNEP panu. After the BNEP panu is 
 *      closed a BNEP_CLOSED event will be sent to the BNEP application
 *      callback. BNEP_Open() and BNEP_DeRegister() are the only functions
 *      that can be called after this call is completed.
 *
 * Parameters:
 *      panu -  A pointer to the opened BnepPanu structure. 
 *
 * Returns:
 *      BT_STATUS_PENDING - The process to begin closing the BNEP 
 *         panu was successful.
 *
 *      BT_STATUS_SUCCESS - The BNEP panu has been closed.
 *
 *      BT_STATUS_FAILED  - A BNEP connection is currently in progress. 
 *
 *      BT_STATUS_BUSY    - The panu is already closing or
 *         performing another signaling command.
 *
 *      BT_STATUS_NO_RESOURCES - L2CAP could not allocate a signaling
 *         packet.
 * 
 *      BT_STATUS_NO_CONNECTION - The BNEP panu link to the specified 
           device is not active.
 * 
 *      BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus BNEP_Close(BnepPanu *panu);

/*---------------------------------------------------------------------------
 * BNEP_SendEtherPkt()
 *
 *      Send an Ethernet packet to the remote device. The etherPkt parameter
 *      points to a valid ethernet packet to be encapsulated in the BNEP
 *      packet. If extHdr is non-zero, the extension header is inserted
 *      in the BNEP packet and sent as well.

 *
 * Parameters:
 *      panu - Identifies the BNEP PANU channel that owns the packet.
 *      btPacket - A pointer to the BtPacket used to send the Ethernet packet
 *                 through L2CAP.
 *      etherPkt - A pointer to the Ethernet packet to send.
 *      etherPktLen - Length of the Ethernet packet.
 *      extHdr - If extHdr is non-zero, it points to one or more BNEP
 *               extension headers concatenated together with correct types
 *               and E bits. The extension header(s) will be inserted and
 *               sent with the Ethernet packet.
 *
 * Returns:    
 *      BT_STATUS_PENDING - The send packet process has been successfully 
 *          started.
 *
 *      BT_STATUS_INVALID_PARM - An invalid parameter was used.
 *
 *      BT_STATUS_FAILED - L2CAP_Send failure.
 *
 *      BT_STATUS_IN_PROGRESS - BNEP is busy sending another packet.
 */
BtStatus BNEP_SendEtherPkt(BnepPanu *panu,
                           BtPacket *btPacket,
                           U8 *etherPkt,
                           U16 etherPktLen,
                           U8 *extHdr);

/*---------------------------------------------------------------------------
 * BNEP_SendPkt()
 *
 *      Send a BNEP packet to the remote device. The bnepPkt parameter
 *      points to one of the valid BNEP packet types: General, Compressed,
 *      Source only compressed or Destination only compressed and may
 *      contain extension headers.
 *
 * Parameters:
 *      panu - Identifies the BNEP PANU channel that owns the packet.
 *      btPacket - A pointer to the BtPacket used to send the BNEP packet
 *                 through L2CAP.
 *      bnepPkt - A pointer to the BNEP packet to send.
 *      bnepPktLen - Length of the BNEP packet.
 *
 * Returns:    
 *      BT_STATUS_PENDING - The send packet process has been successfully 
 *          started.
 *
 *      BT_STATUS_INVALID_PARM - An invalid parameter was used.
 *
 *      BT_STATUS_FAILED - L2CAP_Send failure.
 *
 *      BT_STATUS_IN_PROGRESS - BNEP is busy sending another packet.
 */
BtStatus BNEP_SendPkt(BnepPanu *panu,
                      BtPacket *btPacket,
                      U8 *bnepPkt,
                      U16 bnepPktLen);

/*---------------------------------------------------------------------------
 * BNEP_Control()
 *
 *      Sends a BNEP control packet.
 *
 * Parameters:
 *      panu - Identifies the BNEP PANU channel that owns the packet.
 *      btPacket - A pointer to the BtPacket used to send the Control
 *                 packet through L2CAP.
 *      controlPkt - A pointer to the Control packet to send.
 *      controlPktLen - Length of the Control packet.
 *
 * Returns:    
 *      BT_STATUS_PENDING - The process has been successfully started.
 *
 *      BT_STATUS_INVALID_PARM - An invalid parameter was used.
 *
 *      BT_STATUS_FAILED - L2CAP_Send failure.
 *
 *      BT_STATUS_IN_PROGRESS - A control command is already in process.
 */
BtStatus BNEP_Control(BnepPanu *panu, 
                      BtPacket *btPacket,
                      U8 *controlPkt,
                      U16 controlPktLen);

/*---------------------------------------------------------------------------
 * BNEP_SetControlTimeout()
 *
 *      Sets the timeout value for future BNEP_Control BNEP_CONTROL
 *      commands for the given PANU channel. The default value is specified
 *      with the BNEP_CONTROL_TIMEOUT overide. This function would
 *      typically be used to adjust the timeout value on BNEP_Control retries.
 *
 * Parameters:
 *      panu - Identifies the BNEP PANU channel for the timeout adjustment.
 *      controlTimeout - Timeout value in seconds.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The timeout value was set.
 *
 *      BT_STATUS_INVALID_PARM - The timeout value was not within 1 and 30.
 */
BtStatus BNEP_SetControlTimeout(BnepPanu *panu, TimeT controlTimeout);

/*---------------------------------------------------------------------------
 * BnepGetLocalBnepAddr()
 *
 *      Get the BD_ADDR of the local device and return it in BNEP_ADDR form.
 *
 * Parameters:
 *      bnepAddr - Pointer to storage for the local BNEP_ADDR.
 *
 * Returns:
 *      None
 */
void BnepGetLocalBnepAddr(BNEP_ADDR *bnepAddr);

/*---------------------------------------------------------------------------
 * BnepGetRemoteBnepAddr()
 *
 *      Get the BD_ADDR of the remote device and return it in BNEP_ADDR form.
 *
 * Parameters:
 *      panu - Identifies the BNEP PANU channel for the timeout adjustment.
 *      bnepAddr - Pointer to storage for the remote BNEP_ADDR.
 *
 * Returns:
 *      None
 */
void BnepGetRemoteBnepAddr(BnepPanu *panu, BNEP_ADDR *bnepAddr);

/*---------------------------------------------------------------------------
 * bnep_addr_aton()
 *
 *      Convert a MSB ASCII aa:bb:cc:dd:ee:ff to a MSB BNEP_ADDR value. The
 *      BNEP_ADDR can be either a Bluetooth device address or an ethernet
 *      address.
 *
 * Parameters:
 *      addrString - BNEP address string to convert.
 *      bnepAddr - Pointer to storage for the BNEP_ADDR.
 *
 * Returns:
 *      None
 */
void bnep_addr_aton(const char *addrString, BNEP_ADDR *bnepAddr);

/*---------------------------------------------------------------------------
 * bnep_addr_ntoa()
 *
 *      Convert a MSB BNEP_ADDR to MSB ASCII notation aa:bb:cc:dd:ee:ff.
 *      AddrString should point to no less than 20 bytes. It will be
 *      null-terminated upon return.
 *
 * Parameters:
 *      bnepAddr - Pointer to BNEP_ADDR to convert.
 *      addrString - Pointer to storage for the converted BNEP address string.
 *
 * Returns:
 *      pointer to the converted/provided AddrString
 */
char *bnep_addr_ntoa(const BNEP_ADDR *bnepAddr, char *addrString);

#endif /* __BNEP_H */

#endif /*__BT_BNEP__*/

