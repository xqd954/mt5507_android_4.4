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

#ifndef __RFCOMM_H
#define __RFCOMM_H

/****************************************************************************
 *
 * File:
 *     $Workfile:rfcomm.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     RFCOMM component. 
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

#include "rfcomm_adp.h"

#if RFCOMM_PROTOCOL == XA_ENABLED

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * RF_RegisterServerChannel()
 *
 *     Registers a service with RFCOMM. After this call, a remote device
 *     may attempt to connect to the service using the channel provided.
 *
 *     RFCOMM allows connections from multiple remote devices to the same
 *     service. To achieve this, the RFCOMM user must call this function
 *     repeatedly to associate a set of RfChannel structures with the
 *     same RfService object. As each incoming connection arrives, an
 *     RfChannel structure is assigned and provided to the callback
 *     function with the RFEVENT_OPEN_IND event.
 * 
 * Parameters:
 *     channel - Channel to register as a server with the service indicated.
 *         The "callback" and "maxFrameSize" fields must be initialized
 *         before calling this function.  It is not necessary to set the
 *         "priority" field of the channel structure, since only the
 *         client can specify that value when connecting.  This structure
 *         is owned by RFCOMM until a successful call to RF_DeregisterService
 *         or RF_DeregisterServerChannel is made.
 *
 *     service - Service to register the channel with. If the service has
 *         not previously been used, its "serviceId" field must be
 *         initialized to 0, and this function will select a service ID.
 *
 *         When adding a new channel to an existing service, do not
 *         modify the "serviceId" field. Refer to RfService for information 
 *         on serviceId and how it relates to SDP and security records 
 *         (BtSecurityRecord).
 *
 *         This structure is owned by RFCOMM until a successful call to
 *         RF_DeregisterService is made.
 *
 *     credit - This parameter is used to specify an initial amount of credit 
 *         that will be granted to a client upon connection.  The amount of 
 *         credit that is advanced tells the client how many RFCOMM packets it 
 *         can send before flow control is shut off from client to server.  
 *         In order to allow the client to send more data, RF_AdvanceCredit must
 *         be called.  If the remote device does not support credit-based flow 
 *         control, then the flow control is controlled automatically by the FC 
 *         bit in the modem status command.
 *
 *         See RF_AdvanceCredit for a discussion of flow control.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Service was successfully registered.
 *
 *     BT_STATUS_FAILED  - Service failed to register because the number
 *         of servers or channels would exceed the maximum (NUM_RF_SERVERS or
 *         NUM_RF_CHANNELS).
 *         
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 *
 */
void RF_ChannelInit(RfChannel *Channel, U8 sec_level);

#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
void RF_RegisterSecurityRecord(RfChannel *Channel, BOOL incoming, U8 service_id);
void RF_DeRegisterSecurityRecord(RfChannel *Channel);

#endif /* (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED) */ 

BtStatus RF_RegisterServerChannel(RfChannel *Channel, RfService *Service, U8 Credit);

/*---------------------------------------------------------------------------
 * RF_DeregisterServerChannel()
 *
 *     Deregisters a channel from an RFCOMM service. After this call, 
 *     the RfChannel structure may be reused or deallocated by the RFCOMM user.
 *
 * Parameters:
 *     channel - Channel to deregister from the service.
 *
 *     service - Service from which the channel will be deregistered.  The
 *         channel must be closed before making this call.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The channel was successfully deregistered.
 *
 *     BT_STATUS_FAILED - The service or channel could not be found.
 *
 *     BT_STATUS_BUSY - The channel is currently open.  The channel must
 *         be successfully closed with RF_CloseChannel before calling
 *         this function.
 * 
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_DeregisterServerChannel(RfChannel *Channel, RfService *Service);

/*---------------------------------------------------------------------------
 * RF_DeregisterService()
 *
 *     Deregisters an RFCOMM service. After this call, the RfService
 *     structure and all associated RfChannel structures may be reused or
 *     deallocated by the RFCOMM user.
 *
 *     When deregistering the service, it's a good idea to remove the
 *     corresponding SDP entry.
 *
 * Parameters:
 *     service - Service to deregister. All channels associated with the
 *         service must be disconnected (RF_CloseChannel) before the
 *         service is deregistered.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The service was successfully deregistered.
 *
 *     BT_STATUS_FAILED - The service was invalid.
 *
 *     BT_STATUS_BUSY - The service has an open channel. All channels
 *         must be successfully closed with RF_CloseChannel before calling
 *         this function.
 * 
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_DeregisterService(RfService *service);

/*---------------------------------------------------------------------------
 * RF_OpenClientChannel()
 *
 *     Attempts to establish a channel with a remote service. The RFCOMM
 *     user must identify the remote service by its device and remote
 *     server ID.
 * 
 * Parameters:
 *     remDev - Identifies the remote device. 
 *
 *     serviceId - Identifies the RFCOMM server ID on the remote device.
 *         The server ID can be determined by querying the remote device's
 *         SDP server.
 *
 *     channel - Provides information about the client channel. The
 *         "callback", "maxFrameSize", and "priority" fields must
 *         be initialized prior to the call. This structure is owned by 
 *         RFCOMM until the channel is closed.
 *
 *     credit - This parameter is used to specify an initial amount of credit 
 *         that will be granted to a client upon connection.  The amount of 
 *         credit that is advanced tells the client how many RFCOMM packets it 
 *         can send before flow control is shut off from client to server.  
 *         In order to allow the client to send more data, RF_AdvanceCredit must
 *         be called.  If the remote device does not support credit-based flow 
 *         control, then the flow control is controlled automatically by the FC 
 *         bit in the modem status command.
 *
 *         See RF_AdvanceCredit for a discussion of flow control.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request to open the channel was sent.
 *         If the remote device accepts the request, RFCOMM will
 *         generate an RFEVENT_OPEN event. If the channel is rejected, RFCOMM
 *         will generate an RFEVENT_CLOSED event.
 *
 *     BT_STATUS_FAILED - The L2CAP connection is not ready for RFCOMM
 *         data
 *
 *     BT_STATUS_NO_RESOURCES - The system has run out of resources
 *         and cannot open the channel.  A channel must be closed before a
 *         new channel can be opened.
 *
 *     BT_STATUS_RESTRICTED - The channel failed a security check.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_OpenClientChannel(BtRemoteDevice *RemDev, U8 ServerId, RfChannel *Channel, U8 Credit);

/* Function used by the two macros below */


/*---------------------------------------------------------------------------
 * RF_SendData()
 *
 *     Sends data on the specified channel. When data transmission is
 *     complete, RFCOMM generates an RFEVENT_PACKET_HANDLED event for the
 *     channel. The "status" field of the RfCallbackParms structure
 *     will indicate whether the transmission was successful.
 *
 * Parameters:
 *     channel - Identifies the channel upon which the data is sent.
 *
 *     packet - The data to be sent. This structure is owned by RFCOMM
 *         until it is returned with the RFEVENT_PACKET_HANDLED event.
 *
 * Returns:      
 *     BT_STATUS_PENDING - The data has been queued. If the data is sent
 *         successfully, an RFEVENT_PACKET_HANDLED event will arrive with
 *         a "status" of BT_STATUS_SUCCESS.  If the transmission fails, the
 *         same event will be received with a status specifying the reason.
 *
 *     BT_STATUS_FAILED - The channel is not in the correct state.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK).
 */
BtStatus RF_SendData(RfChannel *channel, BtPacket *packet);

#if RF_SEND_CONTROL == XA_ENABLED
/*---------------------------------------------------------------------------
 * RF_SetModemStatus()
 *
 *     Sends a modem status command to the remote device.  This function
 *     allows the application to send flow control and V.24 signals to the
 *     remote device.
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the connected channel for this action.
 *
 *     modemStatus - The modem status structure.  The "signals" field should
 *         be initialized with the V.24 bits and/or flow control bit.
 *         The "breakLen" field should contain a value between 0 and 15,
 *         signifying the length of a break signal in 200 ms units.
 *         If "breakLen" is zero, no break signal is sent.
 *
 *         The "modemStatus" structure is owned by RFCOMM until either an
 *         RFEVENT_MODEM_STATUS_CNF or RFEVENT_CLOSED event is received.
 *
 *         The FC bit of the modem status is used for flow control of the
 *         channel.  Applications do not need to set this bit, however,
 *         because flow control is automatically managed using credits.
 *
 *         See RF_AdvanceCredit for a discussion of flow control.
 *
 * Returns:
 *     BT_STATUS_PENDING - The modem status will be sent to the remote
 *         device.  If the remote device receives the status, the
 *         channel's callback function will receive an RFEVENT_MODEM_STATUS_CNF
 *         event. If the operation times out, the callback function will
 *         receive an RFEVENT_CLOSED event.
 *
 *     BT_STATUS_FAILED - The channel is not open.
 *
 *     BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         outstanding for this channel.  Wait for the status event associated 
 *         with the request before sending a new request. The functions that 
 *         make control channel requests are:  RF_SetModemStatus, 
 *         RF_SetLineStatus, RF_RequestPortSettings, RF_RequestPortStatus, 
 *         and RF_SendTest.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_SetModemStatus(RfChannel *channel, RfModemStatus *modemStatus);

/*---------------------------------------------------------------------------
 * RF_SetLineStatus()
 *
 *     Sends the line status command to the remote device.  This function
 *     allows the RFCOMM user to communicate overrun, framing, and parity
 *     errors to the remote device.
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 *
 *     lineStatus - The line status bit mask.  This value should be
 *         initialized with the appropriate line status bits set.
 *
 * Returns:
 *     BT_STATUS_PENDING - The line status will be sent to the remote
 *         device.  When the remote device receives the status, RFCOMM
 *         will generate an RFEVENT_LINE_STATUS_CNF event. If the operation
 *         times out, RFCOMM will generate RFEVENT_CLOSED.
 *
 *     BT_STATUS_FAILED - The channel is not open.
 *
 *     BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         outstanding for this channel.  Wait for the status event associated 
 *         with the request before sending a new request. The functions that 
 *         make control channel requests are:  RF_SetModemStatus, 
 *         RF_SetLineStatus, RF_RequestPortSettings, RF_RequestPortStatus, 
 *         and RF_SendTest.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_SetLineStatus(RfChannel *channel, RfLineStatus lineStatus);

/* Function used by two macros below */
BtStatus RF_SendPortSettings(RfChannel *Channel, RfPortSettings *PortSettings);

/*---------------------------------------------------------------------------
 * RF_RequestPortSettings()
 *
 *     Requests a change to the current port settings. The caller may set
 *     only a subset of the fields in the "portSettings" parameter. The
 *     "parmMask" field of the structure identifies which fields are
 *     important.
 *
 *     After a successful call to this function, RFCOMM exchanges
 *     the parameters with the remote device on the channel. After the
 *     remote device responds, RFCOMM generates an RFEVENT_PORT_NEG_CNF event
 *     to indicate which settings were accepted or rejected. Bits in
 *     the "parmMask" field indicate whether the parameter was accepted
 *     (bit set) or rejected (bit clear).
 *
 *     If the remote device rejects some of the parameters, subsequent
 *     requests can be sent with modified parameters until a final
 *     agreement is reached.
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 * 
 * Parameters:
 *     channel - Identifies the channel for this action.
 *
 *     portSettings - A pointer to an RfPortSettings structure, initialized
 *         with the desired port settings. This structure is owned by
 *         RFCOMM until either the RFEVENT_PORT_NEG_CNF or RFEVENT_CLOSED event
 *         is received.
 *
 * Returns:
 *     BT_STATUS_PENDING - The port negotiation will be sent to the
 *         remote device.  If the remote device responds to the request,
 *         RFCOMM sends an RFEVENT_PORT_NEG_CNF event. If the operation times
 *         out, RFCOMM sends an RFEVENT_CLOSED event.
 *
 *     BT_STATUS_FAILED - The channel is not open.
 *
 *     BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         outstanding for this channel.  Wait for the status event associated 
 *         with the request before sending a new request. The functions that 
 *         make control channel requests are:  RF_SetModemStatus, 
 *         RF_SetLineStatus, RF_RequestPortSettings, RF_RequestPortStatus, 
 *         and RF_SendTest.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_RequestPortSettings(RfChannel *channel, RfPortSettings *portSettings);

#define RF_RequestPortSettings(c, p) RF_SendPortSettings(c, p)

/*---------------------------------------------------------------------------
 * RF_RequestPortStatus()
 *
 *     Requests the status of the port settings for the remote device.
 *
 *     After a successful call to this function, the remote device responds
 *     with its current port settings. 
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 *
 * Returns:
 *     BT_STATUS_PENDING - The request will be sent to the remote device.  
 *         If the remote device responds to the request, RFCOMM sends an 
 *         RFEVENT_PORT_STATUS_CNF event. If the operation times out, RFCOMM 
 *         sends an RFEVENT_CLOSED event.  
 *
 *     BT_STATUS_FAILED - The channel is not open.
 *
 *     BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         outstanding for this channel.  Wait for the status event associated 
 *         with the request before sending a new request. The functions that 
 *         make control channel requests are:  RF_SetModemStatus, 
 *         RF_SetLineStatus, RF_RequestPortSettings, RF_RequestPortStatus, 
 *         and RF_SendTest.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_RequestPortStatus(RfChannel *channel);

#define RF_RequestPortStatus(c) RF_SendPortSettings(c, (RfPortSettings *)0)

/* Function used by two macros below */
BtStatus RF_RespondPortSettings(RfChannel *Channel, RfPortSettings *PortSettings, BOOL delayedResponse);

/*---------------------------------------------------------------------------
 * RF_AcceptPortSettings()
 *
 *     Accepts or rejects the port settings received during an
 *     RFEVENT_PORT_NEG_IND event.
 *
 *     This function must be called during the RFEVENT_PORT_NEG_IND callback,
 *     unless processing of the Port Negotiation has been delayed by calling
 *     RF_DelayPortRsp.  If called during the callback, the 
 *     RfPortSettings structure provided in the "ptrs.portSettings" field can 
 *     be modified and used for the "PortSettings" parameter.  Only the 
 *     "parmMask" field of the RfPortSettings structure should be modified to 
 *     indicate whether a parameter is accepted or rejected.
 *
 *     Processing of the Port Negotiation request can be delayed by calling 
 *     RF_DelayPortRsp.  This allows the application to accept the
 *     port settings outside the callback context.  If processing is
 *     delayed, it is important for the application to respond within 10 seconds,
 *     otherwise, a link disconnect may result.  The application will also
 *     be required to allocate an RfPortSettings structure, copy the port
 *     settings from the "ptrs.portSettings" field receive during the
 *     RFEVENT_PORT_NEG_IND event into that data structure, set the response
 *     mask to the appropriate value, and provide a pointer to this structure 
 *     in the "PortSettings" parameter.
 *
 *     If neither this function nor RF_DelayPortRsp are called during the
 *     callback, the stack will automatically respond to the 
 *     RFEVENT_PORT_NEG_IND by accepting the requested parameters.
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 * 
 *     portSettings - Contains port settings received in the RFEVENT_PORT_NEG_IND
 *         event.  The "parmMask" field should be set with bits that indicate 
 *         which parameters are valid.  To accept a parameter, set its bit
 *         to 1. To reject a parameter, set its bit to 0.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The port settings have been accepted or rejected
 *         as indicated, and an indication will be sent to the remote
 *         device.
 *
 *     BT_STATUS_FAILED - The channel is not in the correct state or a request
 *         is already outstanding.
 *
 *     BT_STATUS_NO_RESOURCES - There are no available resources for sending
 *         this response.  When resources are free, an RFEVENT_RESOURCE_FREE
 *         event will be sent to the application callback function.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_AcceptPortSettings(RfChannel *channel, RfPortSettings *portSettings);

#define RF_AcceptPortSettings(c, p) RF_RespondPortSettings(c, p, FALSE)

/*---------------------------------------------------------------------------
 * RF_SendPortStatus()
 *
 *     Sends the current port settings (specified by "portSettings") when they
 *     have been requested by the RFEVENT_PORT_STATUS_IND event. 
 *
 *     This function must be called during the RFEVENT_PORT_NEG_IND callback,
 *     unless processing of the Port Status request has been delayed by calling
 *     RF_DelayPortRsp. If called during the callback, the 
 *     RfPortSettings structure provided in the "ptrs.portSettings" field can 
 *     be modified and used for the "PortSettings" parameter.  Each field should
 *     be set to the current state of each port parameter.
 *
 *     Processing of the Port Status request can be delayed by calling 
 *     RF_DelayPortRsp.  This allows the application to report the port status 
 *     outside the callback context.  If processing is delayed, it is 
 *     important for the application to respond within 10 seconds, otherwise, a 
 *     link disconnect may result.  Also, the application is required to 
 *     allocate an RfPortSettings structure, copy the port status into that 
 *     structure, and provide a pointer to this structure in the "PortSettings" 
 *     parameter.
 *
 *     If neither this function nor RF_DelayPortRsp are called during the
 *     callback, the stack will automatically respond to the 
 *     RFEVENT_PORT_STATUS_IND by sending the default Serial Port Profile
 *     port settings of:  9600 baud, 8 data bits, 1 stop bit, no parity.
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 * 
 *     portSettings - Contains the current port settings maintained by the 
 *         application or port entity.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The port settings have been accepted or rejected
 *         as indicated.
 *
 *     BT_STATUS_FAILED - The channel is not in the correct state or a request
 *         is already outstanding.
 *
 *     BT_STATUS_NO_RESOURCES - There are no available resources for sending
 *         this response.  When resources are free, an RFEVENT_RESOURCE_FREE
 *         event will be sent to the application callback function.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_SendPortStatus(RfChannel *channel, RfPortSettings *portSettings);

#define RF_SendPortStatus(c, p) RF_RespondPortSettings(c, p, FALSE)

/*---------------------------------------------------------------------------
 * RF_DelayPortRsp()
 *
 *     Delays the processing of an RFEVENT_PORT_NEG_IND or an
 *     RFEVENT_PORT_STATUS_IND event so that a response may be sent outside
 *     the context of the callback.
 * 
 *     This function must be called during the RFEVENT_PORT_NEG_IND or 
 *     RFEVENT_PORT_STATUS_IND callbacks.  When this function is called during 
 *     the callback, no response will be sent to the remote device until
 *     the application calls RF_AcceptPortSettings or RF_SendPortStatus.
 *     It is important for the application to respond within 10 seconds,
 *     otherwise, a link disconnect may result.  
 *
 * Requires:
 *     RF_SEND_CONTROL enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 * 
 * Returns:
 *     BT_STATUS_SUCCESS - The port response will be delayed.
 *
 *     BT_STATUS_FAILED - The channel is not in the correct state.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_DelayPortRsp(RfChannel *channel);

#define RF_DelayPortRsp(c) RF_RespondPortSettings(c, 0L, TRUE)

#endif /* RF_SEND_CONTROL == XA_ENABLED */

#if RF_SEND_TEST == XA_ENABLED
/*---------------------------------------------------------------------------
 * RF_SendTest()
 *
 *     Sends a TEST frame. When the test result is received, RFCOMM generates
 *     an RFEVENT_TEST_CNF event. During the event, the "status" field of the
 *     RfCallbackParms structure contains the result of the test.
 *
 *     If the status is set to BT_STATUS_SUCCESS, the test was successful
 *     (a valid test response was received). If the status is
 *     BT_STATUS_FAILED, the data transmitted did not match the received
 *     data.
 *
 * Requires:
 *     RF_SEND_TEST enabled.
 *
 * Parameters:
 *     channel - Identifies the channel for this action.
 *
 * Returns:
 *     BT_STATUS_PENDING - The test operation was started successfully. The
 *         result will be returned in the RFEVENT_TEST_CNF event. If the
 *         connection times out before the response is detected, RFEVENT_CLOSED
 *         is indicated instead.
 *
 *     BT_STATUS_FAILED - The channel is not open.
 *
 *     BT_STATUS_IN_PROGRESS -  An RFCOMM control channel request is already 
 *         outstanding for this channel.  Wait for the status event associated 
 *         with the request before sending a new request. The functions that 
 *         make control channel requests are:  RF_SetModemStatus, 
 *         RF_SetLineStatus, RF_RequestPortSettings, RF_RequestPortStatus, 
 *         and RF_SendTest.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus RF_SendTest(RfChannel *Channel);
#endif /* RF_SEND_TEST == XA_ENABLED */

/*---------------------------------------------------------------------------
 * RF_FrameSize()
 *
 *     Returns the maximum size allowed for a transmit or receive data.  The 
 *     value returned is only valid for an active channel. The maximum size 
 *     will vary for each channel and is based on negotiated channel parameters.
 *
 * Parameters:
 *     Channel - Identifies the channel for this action.
 *
 * Returns:
 *     U16 - The maximum number of bytes allowed for a transmit packet.
 */
U16 RF_FrameSize(RfChannel *Channel);
BOOL RF_QuerySpecVersion(RfChannel *Channel);


/*---------------------------------------------------------------------------
 * RF_CreditFlowEnabled()
 *
 *     Identifies whether credit based flow control has been negotiated 
 *     for the current RFCOMM session or not.  Credit flow control is
 *     negotiated in RFCOMM.  If credit flow control is enabled locally and
 *     the remote device is capable of credit flow control, then this function 
 *     will return TRUE.  It must be called only after the channel is open
 *     (RFEVENT_OPEN is received).  
 *
 *     See RF_AdvanceCredit for a discussion of flow control.
 *
 * Parameters:
 *     Channel - Identifies the channel for this action.
 *
 * Returns:
 *     TRUE - credit flow is enabled.
 *
 *     FALSE - credit flow is not enabled.
 */
BOOL RF_CreditFlowEnabled(RfChannel *Channel);

/*---------------------------------------------------------------------------
 * RF_RemoteDevice()
 *
 *     Returns the remote device associated with the specified channel.
 *
 * Parameters:
 *     Channel - Identifies the channel for this action.
 *
 * Returns:
 *     BtRemoteDevice - a pointer to the remote device structure used for 
 *        the specified channel (zero if no remote device can be found).
 */
BtRemoteDevice *RF_RemoteDevice(RfChannel *Channel);

/* Added for unplugfest support */

#define RFEVENT_FLOW_OFF_CNF      0x44
#define RFEVENT_FLOW_ON_CNF       0x45
#define INVALID_CMD               0xDC
#define RFEVENT_INVALID_CMD_CNF   0x46
#define CHNL_FLAG_NO_CFC          0x80

BtStatus RF_SendAggregateFlow(RfChannel *Channel, U8 state);
BtStatus RF_SendAggregateFlowOff(RfChannel *Channel);

#define RF_SendAggregateFlowOff(c) RF_SendAggregateFlow(c, RF_FLOW_OFF)
BtStatus RF_SendAggregateFlowOn(RfChannel *Channel);

#define RF_SendAggregateFlowOn(c) RF_SendAggregateFlow(c, RF_FLOW_ON)
BtStatus RF_SendBadCommand(RfChannel *Channel);
BtStatus RF_SendBadMSC(RfChannel *Channel, RfModemStatus *ModemStatus);
BtStatus RF_SendPN(RfChannel *Channel);

void RF_DisableCFC(RfChannel *Channel);

#define RF_DisableCFC(c) ((c)->rfChFlags |= CHNL_FLAG_NO_CFC)
void RF_EnableCFC(RfChannel *Channel);

#define RF_EnableCFC(c)  ((c)->rfChFlags &= ~CHNL_FLAG_NO_CFC)

/* JSR82 API */
BtStatus RF_JSR82RegisterServerChannel(RfChannel *Channel, RfService *Service, U8 Credit, U8 security);
void RF_JSR82_DeRegisterServerChannel(void);
BtStatus RF_OpenJSR82ClientChannel(BtRemoteDevice *RemDev, U8 ServerId, RfChannel *Channel, U8 Credit, U8 security);

#endif /* RFCOMM_PROTOCOL == XA_ENABLED */

#endif /* __RFCOMM_H */ 

