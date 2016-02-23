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

#ifndef __L2CAP_H
#define __L2CAP_H

/****************************************************************************
 *
 * File:
 *     $Workfile:l2cap.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     upper layer of the Logical Link Control and Adaptation Protocol.
 * 
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,
 * Inc.  This work contains confidential and proprietary information of
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "l2cap_adp.h"
#include "bt_feature.h"

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * L2CAP_InitDefaultPSM()
 *
 *     Set the default value of a psm structure when using ERTM or STM
 *
 * Parameters:
 *     protocol - Pointer to the L2capPsm structure to be registered as 'mode'
 *
 * Returns:
 *     BT_STATUS_SUCCESS - PSM was successfully registered with L2CAP.
 *
 *     BT_STATUS_FAILED - Invalid parameter value (XA_ERROR_CHECK only).
 *
 */
BtStatus L2CAP_InitDefaultPSM(L2capPsm *Protocol, L2capLinkMode mode);

/*---------------------------------------------------------------------------
 * L2CAP_RegisterPsm()
 *
 *     Registers a PSM identifier with L2CAP. Once registered, the protocol
 *     can initiate connections and data transfer as well as receive
 *     connection requests and data.
 *
 * Parameters:
 *     protocol - Pointer to the L2capPsm structure to be registered.
 *         This structure is owned by L2CAP until it is successfully
 *         deregistered. All fields in the structure must be initialized
 *         before this function is called.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - PSM was successfully registered with L2CAP.
 *
 *     BT_STATUS_FAILED - Invalid parameter value (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - Maximum number of registered PSMs reached.
 *
 *     BT_STATUS_IN_USE - The protocol is already registered (XA_ERROR_CHECK only).
 */
BtStatus L2CAP_RegisterPsm(L2capPsm *protocol);

#if L2CAP_DEREGISTER_FUNC == XA_ENABLED
/*---------------------------------------------------------------------------
 * L2CAP_DeregisterPsm()
 *
 *     Deregisters a registered protocol from L2CAP.
 *
 * Requires:
 *     L2CAP_DEREGISTER_FUNC enabled.
 *
 * Parameters:
 *     protocol - Pointer to L2capPsm structure previously registered with
 *         the L2CAP_RegisterPsm function call.
 *                
 * Returns:
 *     BT_STATUS_SUCCESS - The protocol has been deregistered.
 *
 *     BT_STATUS_FAILED - The protocol parameter was invalid (XA_ERROR_CHECK
 *         only).
 * 
 *     BT_STATUS_NOT_FOUND - The protocol was not previously registered.
 * 
 *     BT_STATUS_BUSY - The protocol has one or more active channels and
 *         cannot be deregistered.
 */
BtStatus L2CAP_DeregisterPsm(const L2capPsm *protocol);

#endif /* L2CAP_DEREGISTER_FUNC == XA_ENABLED */

/*---------------------------------------------------------------------------
 * L2CAP_ConnectReq()
 *
 *     Requests the establishment of an L2CAP channel to the specified
 *     remote protocol service and device. If this function returns
 *     BT_STATUS_PENDING, the request was successfully initiated.
 *     Connection status events will be sent to the initiating
 *     L2capPsm's callback function.
 *                
 * Parameters:
 *     protocol - Pointer to a registered L2capPsm structure.
 *     
 *     psm - PSM value of protocol to connect to on the remote device.
 *
 *     remDevice - The remote device to connect with.
 *
 *     settings - Pointer to the channel-specific connection settings. This
 *         parameter may be zero if no settings are relevant.
 *
 *     localCid - Pointer to a channel ID. If this function returns
 *         BT_STATUS_PENDING, this parameter is filled with a channel
 *         ID assigned to this connection.
 *
 * Returns:
 *     BT_STATUS_PENDING - The connection process has been successfully
 *         initialized. When the connection process is complete, the
 *         protocol service's callback will receive either the
 *         L2EVENT_CONNECTED or L2EVENT_DISCONNECTED L2capEventType.
 *
 *     BT_STATUS_INVALID_PARM - Protocol specified is not registered
 *         (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - Protocol linkMode specified basic-only and
 *         a reassembly buffer was provided in the connect settings, or
 *         the linkmode specified flow control/reassembly and no reassembly
 *         buffer was provided (XA_ERROR_CHECK only).
 * 
 *     BT_STATUS_NO_RESOURCES - L2CAP could not allocate a channel
 *         structure to initiate the connection.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *         is not active.
 *
 *     BT_STATUS_RESTRICTED - The PSM is not authorized to establish
 *         outbound connections.
 */
BtStatus L2CAP_ConnectReq(
            const L2capPsm *protocol,
            L2capPsmValue psm,
            BtRemoteDevice *remDevice,
            L2capConnSettings *settings,
            L2capChannelId *localCid);


#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
void L2CAP_ConnectReqAssignFlowControlBuffer(
            L2capConnSettings *Settings,
            U16 ChannelId);
#endif
            
/*---------------------------------------------------------------------------
 * L2CAP_ConnectRsp()
 *
 *     Indicates the response to a connection request (L2EVENT_CONNECT_IND
 *     L2capEventType) from a remote device.
 *
 * Parameters:
 *     cid - Channel ID assigned by L2CAP to this connection. This
 *         ID must match the one provided by the L2EVENT_CONNECT_IND event
 *         callback.
 *
 *     status - Response code used in the response to the remote device.
 *         This value may indicate success, pending, or rejection. 
 * 
 *     settings - Pointer to the channel-specific connection settings. This
 *         parameter may be zero if no settings are relevant.
 *
 * Returns:
 *     BT_STATUS_PENDING - The connection response packet has been initiated
 *         successfully. One of the following events will arrive to
 *         indicate completion:
 *
 *         L2EVENT_CONNECTED, if the connection was established successfully.
 *
 *         L2EVENT_DISCONNECTED, if the connection failed. Check the callback
 *         info's "status" parameter for the exact reason.
 *
 *         L2EVENT_COMPLETE, when the remote device has been informed of the
 *         L2CONN_PENDING connection status. L2CAP_ConnectRsp must
 *         be called a second time.
 *
 *     BT_STATUS_FAILED - Invalid channel ID (XA_ERROR_CHECK only), or
 *         L2CAP_ConnectRsp was called at the wrong time. Protocol linkMode
 *         specified basic-only and a reassembly buffer was provided in the
 *         connect settings, or the linkmode specified flow control/reassembly
 *         and no reassembly buffer was provided (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_IN_PROGRESS - A response is already in progress. Wait for
 *         L2EVENT_COMPLETE before calling L2CAP_ConnectRsp.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2CAP_ConnectRsp(L2capChannelId cid, L2capConnStatus status, L2capConnSettings *settings);

BtStatus L2Cap_AuthorizeConnectRsp(U16 ChannelId, BOOL result);

/*---------------------------------------------------------------------------
 * L2CAP_DisconnectReq()
 *
 *     Requests the disconnection of the specified L2CAP channel.
 *
 * Parameters:
 *     cid - The channel to disconnect.
 *
 * Returns:      
 *     BT_STATUS_PENDING - The disconnect request has been initiated
 *         successfully. When the disconnect is complete, the
 *         L2EVENT_DISCONNECTED event is provided to the L2CAP user.
 * 
 *     BT_STATUS_IN_PROGRESS - The channel is already disconnecting
 *         (XA_ERROR_CHECK Only).    
 *
 *     BT_STATUS_FAILED - The channel is invalid or could not be
 *         disconnected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2CAP_DisconnectReq(L2capChannelId cid);

/*---------------------------------------------------------------------------
 * L2CAP_SendData()
 *
 *      Sends data on the specified channel.
 *
 * Parameters:
 *      cid - Channel to send the data over. The channel must be
 *          connected or refer to a registered group.
 * 
 *      packet - Data packet describing data to transmit. The caller
 *          must initialize all necessary BtPacket fields, including
 *          "data", "dataLen", "flags", "ulpContext", "tail", "tailLen",
 *          "header" and "headerLen" as described below.
 *
 *          The "data" pointer and "dataLen" length indicate the payload
 *          data to be transmitted by L2CAP.
 *
 *          The "header" array is filled with higher-layer protocol
 *          header data. This array is filled back-to-front so that
 *          protocols may fill in data as the packet is handed down
 *          the protocol stack. "headerLen" indicates the number of
 *          valid header bytes at the end of the "header" array.
 *
 *          The "ulpContext" field may be filled with a protocol-specific
 *          value. L2CAP and lower layers ignore this field.
 *
 *          The "flags" field may be set as described by the BtPacketFlags
 *          type.
 *
 *          The "tail" pointer may point to a data buffer that is appended
 *          to the end of the packet (after "data"). The field "tailLen"
 *          is set to the number of bytes pointed to by "tail". If this field
 *          is used, the packet flag BTP_FLAG_TAIL must be set.        
 * 
 *          The packet's total size ("headerLen" plus "dataLen" plus
 *          "tailLen", if present) must be less than or equal to
 *          the remote device's L2CAP MTU. For group channels,
 *          the total size should typically be kept below 670 bytes. 
 *          
 * Returns:
 *     BT_STATUS_PENDING - Packet was successfully queued for transmission on
 *         the channel. The completion of the send request is indicated by an
 *         L2EVENT_PACKET_HANDLED event.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2CAP_SendData(L2capChannelId cid, BtPacket *packet);
#ifdef __BT_3_0_UNICAST__
BtStatus L2CAP_SendUnicastData(const L2capPsm *Protocol, BtRemoteDevice *link, BtPacket *Packet);
#endif
BtStatus L2CAP_AvSendData(L2capChannelId ChannelId, BtPacket *Packet);

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
/*---------------------------------------------------------------------------
 * L2CAP_UpdateSduDataBufferSeq()
 *
 *     Channels that use Flow Control and Retransmission mode are capable of
 *     metering the inbound flow of data. The L2CAP user must accept each
 *     SDU that is indicated on the channel. Once the SDU has been
 *     accepted, the L2CAP layer will permit the remote device to transmit
 *     more data in additional SDUs.
 *
 *     If the reception of an SDU triggers the L2CAP user to send an SDU
 *     back to the remote peer, it is recommended that this function call be
 *     executed after issuing the L2CAP_SendData() request. This will help
 *     minimize the transmission of unnecessary acknowledgements to the 
 *     remote peer.
 *
 * Parameters:
 *     cid - The channel on which the SDU was received.
 * Returns:
 *     BT_STATUS_INVALID_PARM - The channel is invalid or it is not configured
 *         for flow control/retransmission mode or the NumBytes parameter
 *         exceeds the number of bytes outstanding (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_SUCCESS - The buffer space has been accepted.
 *
 */
BtStatus L2CAP_UpdateSduDataBufferSeq(L2capChannelId cid);

/*---------------------------------------------------------------------------
 * L2CAP_GetConnectionMode()
 *
 *     Retrieves the link mode of the connected channel.
 *
 * Parameters:
 *     Cid - Channel ID to query.
 *
 *     In - When true, returns the inbound link mode. When false, returns
 *          the outbound link mode.
 *
 * Returns:
 *     Link Mode - The L2capLinkMode selected during channel configuration.
 *
 *     0 - If the channel does not exist or is not connected (XA_ERROR_CHECK only).
 */
L2capLinkMode L2CAP_GetConnectionMode(L2capChannelId Cid, BOOL In);

/*---------------------------------------------------------------------------
 * L2CAP_DisableRetransmissions()
 *
 *     Disables the retransmission of data packets from the remote peer.
 *
 * Parameters:
 *     Cid - Channel ID.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Retransmissions have been disabled.
 *
 *     BT_STATUS_FAILED - The channel is not configured for retransmissions.
 */
BtStatus L2CAP_DisableRetransmissions(L2capChannelId Cid);

/*---------------------------------------------------------------------------
 * L2CAP_EnableRetransmissions()
 *
 *     Enables the retransmission of data packets from the remote peer.
 *
 * Parameters:
 *     Cid - Channel ID.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Retransmissions have been enabled.
 *
 *     BT_STATUS_FAILED - The channel is not configured for retransmissions.
 */
BtStatus L2CAP_EnableRetransmissions(L2capChannelId Cid);

#endif /* L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED */

/*---------------------------------------------------------------------------
 * L2CAP_GetTxMtu()
 *
 *     Retrieves the transmit MTU of the connected channel.
 *
 * Parameters:
 *     cid - Channel ID to query.
 *
 * Returns:
 *     Size value - The maximum transmission unit size.
 *
 *     0 - If the channel does not exist or is not connected (XA_ERROR_CHECK only).
 */
U16 L2CAP_GetTxMtu(L2capChannelId cid);

#if L2CAP_PING_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * L2CAP_Ping()
 *
 *     Sends an Echo Request to the L2CAP protocol on the specified remote
 *     device. The data length should not exceed the default L2CAP
 *     signaling MTU (44 bytes).
 *
 * Requires:
 *     L2CAP_PING_SUPPORT enabled.
 *
 * Parameters:
 *     pingClient - Client registration structure used by L2CAP to track
 *         the Ping request. The caller must set the callback field.
 *         
 *     remAddr - Bluetooth address of device to ping.
 *
 *     data - Data to send to remote device.
 *
 *     dataLen - Length of "data" buffer in bytes. Must be less than 44.
 *
 * Returns:
 *     BT_STATUS_PENDING - Ping request was successfully queued for
 *         transmit. The result is indicated by an L2EVENT_COMPLETE event,
 *         sent to the "pingClient" callback function, when a response is
 *         received or the request times out. Check the L2capCallbackParms
 *         "status" parameter for the completion status. Any data echoed
 *         back can be found in the callback info's "data" and "dataLen"
 *         fields.
 *
 *     BT_STATUS_NO_RESOURCES - L2CAP could not allocate a signaling
 *         channel or packet.
 * 
 *     BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *         is not active.
 */
BtStatus L2CAP_Ping(const L2capPsm *pingClient, BtRemoteDevice *remDevice, const U8 *data, U16 dataLen);

#endif /* L2CAP_PING_SUPPORT == XA_ENABLED */

#if L2CAP_GET_INFO_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * L2CAP_GetInfo()
 *
 *     Sends an Info Request to the L2CAP protocol on the specified remote
 *     device. Info requests are used to exchange implementation-specific
 *     information regarding L2CAP's capabilities.
 *
 * Requires:
 *     L2CAP_GET_INFO_SUPPORT enabled.
 *
 * Parameters:
 *     infoClient - Client registration structure used by L2CAP to track
 *         the Info request. The caller must set the callback field.
 *
 *     remAddr - Bluetooth address of the device to query.
 *
 *     type - Type of information to request.
 *
 * Returns:
 *     BT_STATUS_PENDING - Sending of an Information Request has been
 *         successfully initiated. The result is indicated by an
 *         L2EVENT_COMPLETE event when a response is received or the request
 *         times out. The result is reported in the L2capCallbackParms
 *         "status" parameter. The result value & length is conveyed in the
 *         callback "data" and "dataLen" fields.
 *
 *     BT_STATUS_NO_RESOURCES - L2CAP could not allocate a signaling
 *         channel or packet.
 * 
 *     BT_STATUS_NO_CONNECTION - The ACL link to the specified device
 *         is not active.
 */
BtStatus L2CAP_GetInfo(const L2capPsm *infoClient, BtRemoteDevice *remDevice, L2capInfoType type);

#endif /* L2CAP_GET_INFO_SUPPORT == XA_ENABLED */

#if L2CAP_NUM_GROUPS > 0
/*---------------------------------------------------------------------------
 * L2CAP_RegisterGroup()
 *
 *     Registers an L2CAP group structure and allocates a channel for
 *     the group.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     group - Group structure used by L2CAP to track group membership.
 *         L2CAP retains ownership of this structure for the duration of
 *         the group. The caller must initialize the "maxMembers", "psm",
 *         and "callback" fields prior to calling this function.
 *
 *     groupId - Pointer to a Channel ID that receives the Group ID
 *         assigned by L2CAP. 
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Group was successfully registered.
 *
 *     BT_STATUS_INVALID_PARM - Group could not be registered because a parameter
 *         was invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - There are no more group entries available
 *         in L2CAP. Increase the L2CAP_NUM_GROUPS configuration parameter
 *         to avoid this error.
 */
BtStatus L2CAP_RegisterGroup(L2capGroup *group, L2capChannelId *groupId);

/*---------------------------------------------------------------------------
 * L2CAP_GroupDestroy()
 *
 *     Removes a group from L2CAP.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     group - Group structure used to register the group. After successful
 *         completion of this call L2CAP no longer maintains ownership of
 *         this group.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The group was removed.
 *
 *     BT_STATUS_FAILED  - The group was not registered (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_BUSY - An operation concerning this group is in progress.
 *         Wait for completion before destroying the group.
 */
BtStatus L2CAP_GroupDestroy(L2capGroup *Group);

/*---------------------------------------------------------------------------
 * L2CAP_GroupAddMember()
 *
 *     Adds a Bluetooth device address to the membership list.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     groupId - Channel ID assigned to the group by L2CAP_RegisterGroup.
 *
 *     newMember - Bluetooth address of device to add.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The device was added to group membership.
 *
 *     BT_STATUS_FAILED - A parameter was invalid (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_NO_RESOURCES - Group member list is full.
 */
BtStatus L2CAP_GroupAddMember(L2capChannelId GroupId, const BD_ADDR *newMember);

/*---------------------------------------------------------------------------
 * L2CAP_GroupRemoveMember()
 *
 *     Removes a device from the group.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     groupId - Channel ID assigned to the group by L2CAP_RegisterGroup.
 * 
 *     member - Bluetooth address of device to remove.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Device was removed from group membership.
 *
 *     BT_STATUS_FAILED - A parameter was invalid (XA_ERROR_CHECK only).
 *                                    
 *     BT_STATUS_NOT_FOUND - The device was not a member of the group.
 */
BtStatus L2CAP_GroupRemoveMember(L2capChannelId GroupId, const BD_ADDR *Member);

/*---------------------------------------------------------------------------
 * L2CAP_GroupGetMembers()
 *
 *     Retrieves a list of members for the specified group.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     groupId - Channel ID assigned to the group by L2CAP_RegisterGroup.
 *
 *     list - Pointer that receives the address of the member list. Do
 *         not modify this list.
 *
 *     count - Pointer that receives the number of BD_ADDR entries in
 *         "list".
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The group member list has been returned.
 *
 *     BT_STATUS_FAILED - A parameter was invalid (XA_ERROR_CHECK only).
 */
BtStatus L2CAP_GroupGetMembers(L2capChannelId GroupId, BD_ADDR **List, U8 *Count);

/*---------------------------------------------------------------------------
 * L2CAP_DisableCLT()
 *
 *     Disables incoming connectionless traffic. By default, incoming
 *     connectionless traffic is enabled.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     psm - Not used at this time.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - Call was successful.
 */
BtStatus L2CAP_DisableCLT(U16 Psm);

/*---------------------------------------------------------------------------
 * L2CAP_EnableCLT()
 *
 *     Enables incoming connectionless traffic. By default, incoming
 *     connectionless traffic is enabled.
 *
 * Requires:
 *     L2CAP_NUM_GROUPS greater than 0.
 *
 * Parameters:
 *     psm - Not used at this time.
 *
 * Returns:
 *    BT_STATUS_SUCCESS - Call was successful.
 */
BtStatus L2CAP_EnableCLT(U16 Psm);

#endif /* L2CAP_NUM_GROUPS > 0 */

#if L2CAP_FLEXIBLE_CONFIG == XA_ENABLED
/*---------------------------------------------------------------------------
 * L2CAP_SetAutoConfigMode()
 *
 *     This function is used to toggle the auto configuration mode in 
 *     L2CAP either on or off. By default auto configuration is on. When
 *     auto configuration is on L2CAP will manage the connection configuration
 *     process. When off, the application is responsible for managing the
 *     configuration.
 *
 *     This function can only be called when the channel is in the unconnected
 *     state. Or when a Connect Request is being indicated.
 *
 * Requires:
 *     L2CAP_FLEXIBLE_CONFIG enabled.
 *
 * Parameters:
 *      ChannelId - Channel to set the auto configuration mode of.
 *
 *      On - TRUE to enable auto configuration, FALSE to disable it.
 */
void L2CAP_SetAutoConfigMode(L2capChannelId ChannelId, BOOL On);

/*---------------------------------------------------------------------------
 * L2CAP_SendConfigReq()
 *
 *     Sends an L2CAP Configuration Request. If any configuration options
 *     are provided they must be in the data area of the BtPacket. The
 *     L2CAP protocol layer will not add any options to the request. This
 *     PDU should be sent when the L2EVENT_CONFIGURING is received. It is
 *     not permitted to wait for an L2EVENT_CONFIG_IND before initiating
 *     local configuration as this can lead to a deadlock. It is also not
 *     permitted to send a configuration request immediately after sending
 *     a connect response, you must wait for the L2EVENT_CONFIGURING event.
 *
 *     If a configuration setting present in the L2capPsm structure is
 *     negotiated using this function, the registered L2capPsm structure
 *     must be updated to reflect the negotiated value.
 *
 * Requires:
 *     L2CAP_FLEXIBLE_CONFIG enabled and channel auto-configure disabled.
 *
 * Parameters:
 *     ChannelId - Channel to send the data over. The channel must be
 *         in the connected or configuration state.
 * 
 *     Request - Data packet describing data to transmit. The caller
 *         must initialize all necessary BtPacket fields, including
 *         "data", "dataLen", "flags", "ulpContext", and "headerLen"
 *         as described below.
 *
 *         The "data" pointer and "dataLen" length indicate the 
 *         configuration options to be transmitted by L2CAP. L2CAP will affix
 *         the configuration option header. Only the options themselves can
 *         be present. 
 *
 *         The "header" array reserved by L2CAP when using this function.
 *         Therefore "headerLen" should be zero.
 *
 *         The "ulpContext" field may be filled with a protocol-specific
 *         value. L2CAP and lower layers ignore this field.
 *
 *         The "flags" field may be set as described by the BtPacketFlags
 *         type.
 *
 *     Flags - Flags for this configuration request.
 *
 *
 * Returns:
 *     BT_STATUS_PENDING - Packet was successfully queued for transmission
 *         on the channel. The completion of the send request is indicated
 *         by an L2EVENT_PACKET_HANDLED event.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2CAP_SendConfigReq(L2capChannelId ChannelId, BtPacket *Request, L2capConfigFlags Flags);

/*---------------------------------------------------------------------------
 * L2CAP_SendConfigRsp()
 *
 *     Sends an L2CAP Configuration Response. If any configuration options
 *     are provided they must be in the data area of the BtPacket. The
 *     L2CAP protocol layer will not add any options to the response. This
 *     PDU must be sent upon reception of an L2EVENT_CONFIG_IND event.
 *
 *     If a configuration setting present in the L2capPsm structure is
 *     negotiated using this function, the registered L2capPsm structure
 *     must be updated to reflect the negotiated value.
 *
 * Requires:
 *     L2CAP_FLEXIBLE_CONFIG enabled and channel auto-configure disabled.
 *
 * Parameters:
 *     ChannelId - Channel to send the data over. The channel must be
 *         in the configuration state.
 * 
 *     Request - Data packet describing data to transmit. The caller
 *         must initialize all necessary BtPacket fields, including
 *         "data", "dataLen", "flags", "ulpContext", and "headerLen"
 *         as described below.
 *
 *         The "data" pointer and "dataLen" length indicate the 
 *         configuration options to be transmitted by L2CAP. L2CAP will affix
 *         the configuration option header. Only the options themselves can
 *         be present.
 *         
 *         The "header" array reserved by L2CAP when using this function.
 *         Therefore "headerLen" should be zero.
 *         
 *         The "ulpContext" field may be filled with a protocol-specific
 *         value. L2CAP and lower layers ignore this field.
 *
 *         The "flags" field may be set as described by the BtPacketFlags
 *         type.
 *
 *     Result - The result of the configuration request
 *
 *     Flags - Flags for this configuration response.
 *
 * Returns:
 *     BT_STATUS_PENDING - Packet was successfully queued for transmission
 *         on the channel. The completion of the send request is indicated
 *         by an L2EVENT_PACKET_HANDLED event.
 *
 *     BT_STATUS_FAILED - The channel is invalid or is not connected.
 *
 *     BT_STATUS_NO_CONNECTION - The ACL link associated with this 
 *         channel is no longer active.
 */
BtStatus L2CAP_SendConfigRsp(
            L2capChannelId ChannelId,
            BtPacket *Response,
            L2capConfigResult Result,
            L2capConfigFlags Flags);

#endif /* L2CAP_FLEXIBLE_CONFIG == XA_ENABLED */

#define BT_INVALID_PSM      BT_CLIENT_ONLY_PSM

void L2Cap_EnableHeavyRTTraffic(void);
void L2Cap_DisableHeavyRTTraffic(void);

void L2Cap_SecAccessRsp(const BtEvent *Event);
void L2Cap_SecAMPAccessRsp(const BtEvent *Event);

BtStatus L2CAP_SendFixChannelData(U16 local_cid, U16 remote_cid, BtRemoteDevice *link, BtPacket *Packet);
#ifdef __BT_2_1_ERTM__
BtStatus L2CAP_AllocateFixChannel(const L2capPsm *Protocol, 
										BtRemoteDevice *link, 
										L2capConnSettings *Settings,
										U16 *localCid);
void L2CAP_FreeFixChannel(L2capChannelId id);
#endif

#if L2CAP_FLOW_CONFIG_LOCKSTEP == XA_ENABLED
void L2Cap_StartLockStepConfig(U8 local_cid);
BtStatus L2CAP_SendLockStepConfigReq(L2capChannelId ChannelId);
BtStatus L2CAP_SendLockStepConfigRsp(L2capChannelId ChannelId, U16 result);
#endif

BtStatus L2CAP_SetRetransDisableFlag(L2capChannelId Cid, BOOL Disable);

#define L2CAP_EnableRetransmissions(_CID)   L2CAP_SetRetransDisableFlag((_CID), FALSE)
#define L2CAP_DisableRetransmissions(_CID)  L2CAP_SetRetransDisableFlag((_CID), TRUE)

void L2Cap_DumpHex(const char *prefix, const char *data, U32 datalen);

#endif /* __L2CAP_H */ 

U8 L2CAP_IsHeavyLoading(void);

