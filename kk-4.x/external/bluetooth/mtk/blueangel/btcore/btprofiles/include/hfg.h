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

/****************************************************************************
 *
 * File:
 *     $Workfile:hfg.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains the hands free profile.
 *             
 * Created:     September 20, 2001
 *
 * Copyright 2001-2004 Extended Systems, Inc.

 * Portions copyright 2005 iAnywhere Solutions, Inc.

 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,  
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __BT_HFG_H__
#define __BT_HFG_H__

#include "hfg_struct.h"

/*---------------------------------------------------------------------------
 * Hands-Free Audio Gateway SDK layer
 *
 * This API allows an application to act as the "Hands-Free Audio Gateway"
 * (AG) role of the Hands-Free Profile. Devices in this role are responsible
 * for providing an interface to a telephone network (such as GSM) under
 * the direct control of a "Hands-Free Unit". For example, the AG may be
 * a mobile phone interacting with a hands-free unit installed in a car.
 *
 * Within this API, there are two "levels" of links: service and
 * audio, each with their own function calls and events. A service-level
 * connection allows the hands-free unit to send commands to the AG, such as 
 * HFG_SendRing().  An audio-level link allows full-duplex sound to be exchanged 
 * between the hands-free unit and the AG. Using this API, the service link must
 * be established BEFORE attempting to open an audio link.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/
 #if defined ( BTMTK_ON_WISESDK ) || defined ( BTMTK_ON_WISE )
#define DBG_PRINT_API(s)            /* Prefix : [HFG][API]*/
#define DBG_PRINT_CALLBACK(s)       /* Prefix : [HFG][CALLBACK]*/
#define DBG_PRINT_EVENT(s)          /* Prefix : [HFG][EVENT]*/
#define DBG_PRINT_FLOW(s)           /* Prefix : [HFG][FLOW]*/
#define DBG_PRINT_AT(s)             /* Prefix : [HFG][AT]*/
#define DBG_PRINT_ERROR(s)          /* Prefix : [HFG][ERR]*/
#define DBG_PRINT_WARN(s)           /* Prefix : [HFG][WARN]*/
 #elif defined ( BTMTK_ON_WIN32 )
#define DBG_PRINT_API(s)            OS_Report s  /* Prefix : [HFG][API]*/
#define DBG_PRINT_CALLBACK(s)       OS_Report s  /* Prefix : [HFG][CALLBACK]*/
#define DBG_PRINT_EVENT(s)          OS_Report s  /* Prefix : [HFG][EVENT]*/
#define DBG_PRINT_FLOW(s)           OS_Report s  /* Prefix : [HFG][FLOW]*/
#define DBG_PRINT_AT(s)             OS_Report s  /* Prefix : [HFG][AT]*/
#define DBG_PRINT_ERROR(s)          OS_Report s  /* Prefix : [HFG][ERR]*/
#define DBG_PRINT_WARN(s)           OS_Report s  /* Prefix : [HFG][WARN]*/
#else
#define DBG_PRINT_API(s)            OS_Report s  /* Prefix : [HFG][API]*/
#define DBG_PRINT_CALLBACK(s)       OS_Report s  /* Prefix : [HFG][CALLBACK]*/
#define DBG_PRINT_EVENT(s)          OS_Report s  /* Prefix : [HFG][EVENT]*/
#define DBG_PRINT_FLOW(s)           OS_Report s  /* Prefix : [HFG][FLOW]*/
#define DBG_PRINT_AT(s)             OS_Report s  /* Prefix : [HFG][AT]*/
#define DBG_PRINT_ERROR(s)          OS_Report s  /* Prefix : [HFG][ERR]*/
#define DBG_PRINT_WARN(s)           OS_Report s  /* Prefix : [HFG][WARN]*/
#endif

typedef enum _BtFileEnum
{
    BT_FILE_HFG,            /* 0 */
    BT_FILE_HFG_AT,         /* 1 */
    BT_FILE_HFG_SDP,        /* 2 */
    BT_FILE_HFG_SEQ,        /* 3 */
    BT_FILE_HFG_SM,         /* 4 */
    BT_FILE_HFG_UTIL,       /* 5 */
}BtFileEnum;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * hfg_init()
 *
 *     Initialize the Audio Gateway SDK.  This function should only be called
 *     once, normally at sytem initialization time.  The calling of this 
 *     function can be specified in overide.h using the XA_LOAD_LIST macro
 *     (i.e. #define XA_LOAD_LIST XA_MODULE(HFG)).
 *
 * Returns:
 *     TRUE - Initialization was successful
 *
 *     FALSE - Initialization failed.
 */
BOOL hfg_init(void);

/*---------------------------------------------------------------------------
 * hfg_register()
 *
 *     Registers and initializes a channel for use in creating and receiving
 *     service level connections.  Registers the Hands-Free profile audio
 *     gateway with SDP.  The application callback function is also bound
 *     to the channel.
 *
 * Parameters:
 *     Channel - Contains a pointer to the channel structure that will be
 *         initialized and registered.
 *
 *     Callback - The application callback function that will receive events.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_IN_USE - The operation failed because the channel has already
 *         been initialized. 
 *
 *     BT_STATUS_FAILED - The operation failed because either the RFCOMM
 *         channel or the SDP record could not be registered.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_register(HfgChannel *Channel, HfgCallback Callback, BOOL bHeadset);

/*---------------------------------------------------------------------------
 * HFG_Deregister()
 *
 *     Deregisters the channel.  The channel becomes unbound from RFCOMM and
 *     SDP, and can no longer be used for creating service level connections.
 *
 * Parameters:
 *     Channel - Contains a pointer to the channel structure that will be
 *         deregistered.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_BUSY - The operation failed because a service level connection 
 *         is still open to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_deregister(HfgChannel *Channel);

/*---------------------------------------------------------------------------
 * HFG_CreateServiceLink()
 *
 *     Creates a service level connection with the hands-free unit.
 *     This includes performing SDP Queries to find the appropriate service
 *     and opening an RFCOMM channel.  The success of the operation is indicated 
 *     by the HFG_EVENT_SERVICE_CONNECTED event.  If the connection fails, the
 *     application is notified by the HFG_EVENT_SERVICE_DISCONNECTED event. 
 * 
 *     If an ACL link does not exist to the audio gateway, one will be 
 *     created first.  If desired, however, the ACL link can be established 
 *     prior to calling this function.
 *
 * Parameters:
 *
 *     Channel - Pointer to a registered channel structure.
 *
 *     Addr - The Bluetooth address of the remote device.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the connection has been created (via the callback 
 *         function registered by HFG_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_BUSY - The operation failed because a connection is already
 *         open to the remote device, or a new connection is being created.
 *
 *     BT_STATUS_FAILED - The channel has not been registered.
 *
 *     BT_STATUS_CONNECTION_FAILED - The connection failed.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_create_service_link(HfgChannel *Channel, BD_ADDR *Addr);

/*---------------------------------------------------------------------------
 * HFG_DisconnectServiceLink()
 *
 *     Releases the service level connection with the hands-free unit. This will 
 *     close the RFCOMM channel and will also close the SCO and ACL links if no 
 *     other services are active, and no other link handlers are in use 
 *     (ME_CreateLink).  When the operation is complete the application will be 
 *     notified by the HFG_EVENT_SERVICE_DISCONNECTED event.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the service level connection is down (via the callback 
 *         function registered by HFG_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_disconnect_service_link(HfgChannel *Channel);

/*---------------------------------------------------------------------------
 * HFG_CreateAudioLink()
 *
 *     Creates an audio (SCO) link to the hands-free unit. The success of the 
 *     operation is indicated by the HFG_EVENT_AUDIO_CONNECTED event.  If the 
 *     connection fails, the application is notified by the 
 *     HFG_EVENT_AUDIO_DISCONNECTED event.  
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the audio link has been established (via the callback 
 *         function registered by HFG_Register).
 *
 *     BT_STATUS_SUCCESS - The audio (SCO) link already exists.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service level 
 *         connection does not exist to the audio gateway.
 *
 *     BT_STATUS_FAILED - An audio connection already exists.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_create_audio_link(HfgChannel *Channel);

/*---------------------------------------------------------------------------
 * HFG_DisconnectAudioLink()
 *
 *     Releases the audio connection with the hands-free unit.  When the 
 *     operation is complete, the application will be notified by the 
 *     HFG_EVENT_SERVICE_DISCONNECTED event.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the audio connection is down (via the callback 
 *         function registered by HFG_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_disconnect_audio_link(HfgChannel *Channel);

/*---------------------------------------------------------------------------
 * HFG_AcceptConnect()
 *
 *     Accept connection request from remote devices. If operation is successfully completed,
 *     HFG sends HFG_EVENT_SERVICE_CONNECTED event to ADP. If the connection can not be 
 *     created, HFG sends HFG_EVENT_SERVICE_DISCONNECTED event to ADP.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the audio connection is up or down (via the callback 
 *         function registered by HFG_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_accept_connect(HfgChannel *Channel);

/*---------------------------------------------------------------------------
 * HFG_RejectConnect()
 *
 *     Reject connection request from remote devices. If operation is successfully completed,
 *     HFG sends HFG_EVENT_SERVICE_CONNECTED event to ADP. 
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the audio connection is down (via the callback 
 *         function registered by HFG_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_reject_connect(HfgChannel *Channel);

#if BT_SCO_HCI_DATA == XA_ENABLED
/*---------------------------------------------------------------------------
 * HFG_SendAudioData()
 *
 *     Sends the specified audio data on the audio link.
 *
 * Requires:
 *     BT_SCO_HCI_DATA enabled.
 *
 * Parameters:
 *     Channel - The Channel over which to send the audio data.
 *
 *     packet - The packet of data to send. After this call, the Hands-free
 *         SDK owns the packet. When the packet has been transmitted
 *         to the host controller, HFG_EVENT_AUDIO_DATA_SENT is sent to the
 *         application
 *
 * Returns:
 *     BT_STATUS_PENDING - The packet was queued successfully.
 *
 *     BT_STATUS_NO_CONNECTION - No audio connection exists.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus hfg_send_audio_data(HfgChannel *Channel, BtPacket *Packet);
#define hfg_send_audio_data(c, p) SCO_SendData(c->cmgrHandler.scoConnect, p)
#endif

/*---------------------------------------------------------------------------
 * HFG_SetIndicatorValue()
 *
 *     Sets the current value for an indicator.  If a service level connection
 *     is active and indicator reporting is currently enabled, the the state 
 *     of the modified indicator is reported to the hands-free device.  If no 
 *     service level connection exists, the current value is changed and will 
 *     be reported during the establishment of the service level connection.
 *     If indicator reporting is disabled, the value of the indicator will only
 *     be reported when requested by the hands-free unit (AT+CIND).
 *
 *     Upon registration of an Audio Gateway (HFG_Register()), all indicators
 *     are initialized to 0.  To properly initialize all indicators, this
 *     function must be called once for each indicator prior to establishing
 *     a service level connection.
 *
 *     Indicators must be sent to the hands-free device as specified by the
 *     hands-free v1.5 specification.  Indicators are sent in the order that
 *     calls are made to this function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Indicator - Indicator type.
 *
 *     Value - The value of the indicator.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The indicator value was set and the Response
 *         structure is available for use.
 *
 *     BT_STATUS_PENDING - The indicator value was set and queued for
 *         sending to the hands-free unit.  When the response has been sent, 
 *         the HFG_EVENT_RESPONSE_COMPLETE event will be received by the
 *         application.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized.
 */
BtStatus hfg_set_indicator_value(HfgChannel *Channel, HfgIndicator Indicator, 
                               U8 value, HfgResponse *Response);

/* Forward Reference used by several functions defined below */
BtStatus hfg_send_hf_results(HfgChannel *Channel, AtCommand Command, 
                           U32 Parms, U16 ParmLen, HfgResponse *Response,
                           BOOL done);

U8 hfg_needSuspendA2DPStreaming(HfgChannel *Channel);

/*---------------------------------------------------------------------------
 * HFG_SendOK
 *     Sends an OK response to the hands-free device.  This function must
 *     be called after receiving several events (see the description of each
 *     event).
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_PENDING - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_send_ok(HfgChannel *Channel, HfgResponse *Response);
#define hfg_send_ok(c, r) hfg_send_hf_results(c, AT_OK, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_SendError()
 *
 *     Sends an ERROR result code to the HF.  This function may be called 
 *     after receiving several events when an error condition exists (see 
 *     the description of each event).  If extended error codes are enabled,
 *     the value specified in the 'Error' parameter will be sent with the
 *     extended error response.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Error - Extended error to be sent (if enabled).
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_send_error(HfgChannel *Channel, HfgCmeError Error,
                       HfgResponse *Response);
#define hfg_send_error(c, e, r) hfg_send_hf_results(c, AT_ERROR, (U32)(e), 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_SendRing()
 *
 *     Notifies the HF of an incoming call.  This call is repeated periodically
 *     as long as the call is still incoming.  If caller ID is enabled, a
 *     call to HFG_SendCallerId() should be called after calling HFG_SendRing().
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_send_ring(HfgChannel *Channel, HfgResponse *Response);
#define hfg_send_ring(c, r) hfg_send_hf_results(c, AT_RING, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_SendCallListRsp
 *
 *     Sends the call listing response to the hands-free device (see
 *     HFG_EVENT_LIST_CURRENT_CALLS).  This function should be called for each 
 *     line supported on the audio gateway with the state of any call set 
 *     appropriately.  If no call is active on the specified line, a response 
 *     (+CLCC) will not be sent.  If a call is is any state besides 
 *     HFG_CALL_STATUS_NONE, then a response will be sent.  On the final call 
 *     to this function, FinalFlag should be set.  This will send an OK response 
 *     in addtion to +CLCC (if sent).
 *
 *     If it is known that no call exists on any line, it is acceptable to call 
 *     HFG_SendOK() instead of calling this function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Parms - A structure containing the call status information for the
 *         specified line.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 *     FinalFlag -  Set to TRUE when the final call is make to this function.
 *
 * Returns:
 *     BT_STATUS_PENDING - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_send_call_list_rsp(HfgChannel *Channel, HfgCallListParms *Parms, 
                             HfgResponse *Response, BOOL FinalFlag);
#define hfg_send_call_list_rsp(c, p, r, f) hfg_send_hf_results(c, AT_LIST_CURRENT_CALLS, (U32)p, sizeof(HfgCallListParms), r, f)

/*---------------------------------------------------------------------------
 * HFG_SendCallerId()
 *
 *     Sends a Calling Line Identification result code containing the phone
 *     number and type of the incoming call.  This function should be called
 *     immediately after HFG_SendRing() if Calling Line Identification Notification
 *     has been enabled by the HF.  If caller ID notification has been disabled
 *     by the remote device, no notification will be sent even if this funcion
 *     is called.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     CallId - A structure containing the number and type of the 
 *         incoming call.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_PENDING - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NOT_SUPPORTED - Caller ID notification is disabled.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_send_caller_id(HfgChannel *Channel, HfgCallerIdParms *CallId, HfgResponse *Response);
#define hfg_send_caller_id(c, i, r)  hfg_send_hf_results(c, AT_CALL_ID, (U32)i, sizeof(HfgCallerIdParms), r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_SendSubscriberNumberRsp
 *
 *     This function is called in response to a request for the subscriber
 *     number (see HFG_EVENT_QUERY_SUBSCRIBER_NUMBER).  It is not necessary 
 *     to call HFG_SendOK() after calling this function.
 * 
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     SbuNum - A structure containing the subscriber number information.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_PENDING - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_send_subscriber_number_rsp(HfgChannel *Channel, HfgSubscriberNum *SubNum, 
                                     HfgResponse *Response, BOOL FinalFlag);
#define hfg_send_subscriber_number_rsp(c, s, r, f)  hfg_send_hf_results(c, AT_SUBSCRIBER_NUM, (U32)s, sizeof(HfgSubscriberNum), r, f)

/*---------------------------------------------------------------------------
 * HFG_SendNetworkOperatorRsp
 *     This function is called in response to a request for the network 
 *     operator information (see HFG_EVENT_QUERY_NETWORK_OPERATOR).
 *     It is not necessary to call HFG_SendOK() after calling this function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Oper - A structure containing the operator information.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_PENDING - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_send_network_operator_rsp(HfgChannel *Channel, HfgNetworkOperator *Oper, HfgResponse *Response);
#define hfg_send_network_operator_rsp(c, o, r)  hfg_send_hf_results(c, AT_NETWORK_OPERATOR | AT_READ, (U32)o, sizeof(HfgNetworkOperator), r, TRUE)

/*---------------------------------------------------------------------------
 * HFG_SendMicVolume()
 *
 * Notifies the HF of the AG's current microphone volume level. 
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     gain - current volume level.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The speaker volume level has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_send_mic_volume(HfgChannel *Channel, U8 Gain, HfgResponse *Response);
#define hfg_send_mic_volume(c, g, r)  hfg_send_hf_results(c, AT_MICROPHONE_GAIN, (U32)(g), 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_SendSpeakerVolume()
 *
 * Notifies the HF of the AG's current speaker volume level.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     gain - current volume level.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The speaker volume level has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_send_speaker_volume(HfgChannel *Channel, U8 Gain, HfgResponse *Response);
#define hfg_send_speaker_volume(c, g, r)  hfg_send_hf_results(c, AT_SPEAKER_GAIN, (U32)(g), 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_SendRingToneStatus()
 *
 *     Notifies the HF of in-band ring tone status.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Enabled - TRUE if in-band ring tone enabled, FALSE otherwise.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_send_ringtone_status(HfgChannel *Channel, BOOL Enabled, HfgResponse *Response);
#define hfg_send_ringtone_status(c, e, r)  hfg_send_hf_results(c, AT_IN_BAND_RING_TONE, (U32)(e), 0, r, FALSE)

#if HFG_USE_RESP_HOLD == XA_ENABLED
/*---------------------------------------------------------------------------
 * hfg_send_response_hold_state()
 *
 *     Notifies the HF of state of Response and Hold.  This function is called
 *     to report the Response and Hold state in response to a request by
 *     the hands-free unit (see HFG_RESPONSE_HOLD), or upon an action taken on 
 *     the audio gateway.
 *
 *     This function is also called in respone to a query for the Response
 *     and Hold state from the hands-free unit (see HFG_QUERY_RESPONSE_HOLD).
 *     This function should be called with the 'State' parameter set to 
 *     HFG_RESP_HOLD_STATE_HOLD if the audio gateway is in the Response and 
 *     Hold state, followed by a call to HFG_SendOK().  Otherwise, the 
 *     application should simply call HFG_SendOK().
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     state - The current Resonse and Hold state.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_send_response_hold_state(HfgChannel *Channel, HfgResponseHold State, HfgResponse *Response);
#define hfg_send_response_hold_state(c, s, r)  hfg_send_hf_results(c, AT_RESPONSE_AND_HOLD, (U32)s, sizeof(HfgResponseHold), r, FALSE)
#endif

/*---------------------------------------------------------------------------
 * hfg_call_waiting()
 *
 *     Notifies the HF of a waiting call (if the HF has enabled the Call 
 *     Waiting Notification feature)
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     CallWait - A structure containing the number, type, and class of the 
 *         incoming call.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_call_waiting(HfgChannel *Channel, HfgCallWaitParms *CallWait, HfgResponse *Response);
#define hfg_call_waiting(c, w, r)  hfg_send_hf_results(c, AT_CALL_WAIT_NOTIFY, (U32)w, sizeof(HfgCallWaitParms), r, FALSE)


/*---------------------------------------------------------------------------
 * hfg_read_indicator()
 *
 *     Response of read indicator CMD (AT+CIND?). Returns current values of all indicators
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     ReadIndicator - A structure containing values of all supported indicators 
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_read_indicator(HfgChannel *Channel, HfgReadIndicatorParms *ReadIndicator, HfgResponse *Response);
#define hfg_read_indicator(c, i, r)  hfg_send_hf_results(c, (AT_INDICATORS|AT_READ), (U32)i, sizeof(HfgReadIndicatorParms), r, TRUE)

/*---------------------------------------------------------------------------
 * HFG_EnableVoiceRec()
 *
 *     Notifies the HF that voice recognition has been disabled (if the HF has
 *     activated the voice recognition functionality in the AG)
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Enabled - TRUE if voice recognition is active, otherwise FALSE>
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_enable_vr(HfgChannel *Channel, BOOL Enabled, HfgResponse *Response);
#define hfg_enable_vr(c, e, r)  hfg_send_hf_results(c, AT_VOICE_RECOGNITION, (U32)(e), 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_VoiceTagRsp()
 *
 *     Called by the app to return the phone number associated with the VoiceTag
 *     request to the HF.  It is not necessary to call HFG_SendOK() after 
 *     calling this function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Number - A structure containing the phone number associated with the
 *         last voice tag.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_voice_tag_rsp(HfgChannel *Channel, const char *number, HfgResponse *Response);
#define hfg_voice_tag_rsp(c, n, r)  hfg_send_hf_results(c, AT_VOICE_TAG, (U32)n, sizeof(n), r, TRUE)

/*---------------------------------------------------------------------------
 * HFG_NoCarrierRsp()
 *
 *     Called by the app to send the "NO CARRIER" response to the HF.  This
 *     response can be sent in addition to the "+CME ERROR:" response.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_no_carrier_rsp(HfgChannel *Channel, HfgResponse *Response);
#define hfg_no_carrier_rsp(c, r)  hfg_send_hf_results(c, AT_NO_CARRIER, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_BusyRsp()
 *
 *     Called by the app to send the "BUSY" response to the HF.  This
 *     response can be sent in addition to the "+CME ERROR:" response.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_busy_rsp(HfgChannel *Channel, HfgResponse *Response);
#define hfg_busy_rsp(c, r)  hfg_send_hf_results(c, AT_BUSY, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_NoAnswerRsp()
 *
 *     Called by the app to send the "NO ANSER" response to the HF.  This
 *     response can be sent in addition to the "+CME ERROR:" response.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_no_answer_rsp(HfgChannel *Channel, HfgResponse *Response);
#define hfg_no_answer_rsp(c, r)  hfg_send_hf_results(c, AT_NO_ANSWER, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_DelayedRsp()
 *
 *     Called by the app to send the "DELAYED" response to the HF.  This
 *     response can be sent in addition to the "+CME ERROR:" response.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_delayed_rsp(HfgChannel *Channel, HfgResponse *Response);
#define hfg_delayed_rsp(c, r)  hfg_send_hf_results(c, AT_DELAYED, 0, 0, r, FALSE)

/*---------------------------------------------------------------------------
 * HFG_BlacklistedRsp()
 *
 *     Called by the app to send the "BLACKLISTED" response to the HF.  This
 *     response can be sent in addition to the "+CME ERROR:" response.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The result code has been sent to the HF.
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - A Service Level Connection does not exist.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_blacklisted_rsp(HfgChannel *Channel, HfgResponse *Response);
#define hfg_blacklisted_rsp(c, r)  hfg_send_hf_results(c, AT_BLACKLISTED, 0, 0, r, FALSE)


BtStatus hfg_send_supported_charset_rsp(HfgChannel *Channel, const char *supported, HfgResponse *Response);
#define hfg_send_supported_charset_rsp(c, s, r)  hfg_send_hf_results(c, (AT_SELECT_CHARACTER_SET|AT_TEST), (U32)s, 0, r, FALSE)

BtStatus hfg_send_selected_charset_rsp(HfgChannel *Channel, const char *selected, HfgResponse *Response);
#define hfg_send_selected_charset_rsp(c, s, r)  hfg_send_hf_results(c, (AT_SELECT_CHARACTER_SET|AT_READ), (U32)s, 0, r, FALSE)

BtStatus hfg_send_model_id_rsp(HfgChannel *Channel, const char *modelId, HfgResponse *Response);
#define hfg_send_model_id_rsp(c, s, r)  hfg_send_hf_results(c, AT_MODEL_ID, (U32)s, 0, r, TRUE)

BtStatus hfg_send_manufacture_id_rsp(HfgChannel *Channel, const char *manufactureId, HfgResponse *Response);
#define hfg_send_manufacture_id_rsp(c, s, r)  hfg_send_hf_results(c, AT_MANUFACTURE_ID, (U32)s, 0, r, TRUE)

/*---------------------------------------------------------------------------
 * HFG_SendAtResponse()
 *
 *     Sends any AT response.  The 'AtString' parameter must be initialized,
 *     and the AT response must be formatted properly.  It is not necessary
 *     to add CR/LF at the beginning and end of the string.
 *
 *     When the AT response has been sent, the HFG_EVENT_RESPONSE_COMPLETE
 *     event will be received by the application's callback function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     AtString - An properly formated AT response.
 *   
 *     Response - A response structure to be used for transmitting the
 *         response.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the response has been sent (via the callback function 
 *         registered by HFG_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter is invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus hfg_send_at_response(HfgChannel *Channel, const char *AtString, HfgResponse *Response, BOOL bDone);
#define hfg_send_at_response(c, s, r, d)  hfg_send_hf_results(c, AT_RAW, (U32)s, sizeof(s), r, d)

/*---------------------------------------------------------------------------
 * hfg_is_nrec_enabled()
 *
 *     Returns TRUE if Noise Reduction and Echo Cancelling is enabled in the
 *     audio gateway.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE - NREC is enabled in the AG.
 *
 *     FALSE - NREC is disabled in the AG.
 */
/*
BOOL hfg_is_nrec_enabled(HfgChannel *Channel);
*/

/*---------------------------------------------------------------------------
 * hfg_is_inbandring_Enabled()
 *
 *     Returns TRUE if In-band Ringing is enabled in the audio gateway.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE - In-band ringing is enabled in the AG.
 *
 *     FALSE - In-band ringing is disabled in the AG.
 */
/*
BOOL hfg_is_inbandring_Enabled(HfgChannel *Channel);
*/

/*---------------------------------------------------------------------------
 * hfg_is_callidnotify_enabled()
 *
 *     Returns TRUE if Caller ID notification is enabled in the audio gateway.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE - Caller ID notification is enabled in the AG.
 *
 *     FALSE - Caller ID notification is disabled in the AG.
 */
/*
BOOL hfg_is_callidnotify_enabled(HfgChannel *Channel);
*/
/*---------------------------------------------------------------------------
 * hfg_is_vr_active()
 *
 *     Returns TRUE if Voice Recognition is active in the audio gateway.  
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE - Voice Recognition is active in the AG.
 *
 *     FALSE - Voice Recognition is inactive in the AG.
 */
/*
BOOL hfg_is_vr_active(HfgChannel *Channel);
*/

/*---------------------------------------------------------------------------
 * hfg_is_callwaiting_active()
 *
 *     Returns TRUE if Call Waiting is active in the audio gateway.  
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE - Call Waiting is active in the AG.
 *
 *     FALSE - Call Waiting is inactive in the AG.
 */
/*
BOOL hfg_is_callwaiting_active(HfgChannel *Channel);
*/

#if HFG_SNIFF_TIMER >= 0
/*---------------------------------------------------------------------------
 * hfg_enable_sniff_mode
 *
 *     Enables/Disables placing link into sniff mode.
 *
 * Requires:
 *     HFG_SNIFF_TIMER >= 0.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Enabled - If TRUE, sniff mode will be used, otherwise sniff mode is
 *               disabled
 */
BtStatus hfg_enable_sniff_mode(HfgChannel *Channel, BOOL Enable);

/*---------------------------------------------------------------------------
 * hfg_is_sniff_mode_enabled
 *
 *     Returns TRUE when sniff mode is enabled on the specified handler.
 *
 * Requires:
 *     HFG_SNIFF_TIMER >= 0.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE if sniff mode is enabled.
 *
 *     FALSE if sniff mode is not enabled.
 */
BOOL hfg_is_sniff_mode_enabled(HfgChannel *Channel);
#define hfg_is_sniff_mode_enabled(c) (CMGR_GetSniffTimer(&((c)->cmgrHandler)) > 0)
#endif

/*---------------------------------------------------------------------------
 * hfg_set_sniff_exit_policy()
 *
 *     Sets the policy for exiting sniff mode on the specified channel.  The 
 *     policy can be set to HFG_SNIFF_EXIT_ON_SEND or HFG_SNIFF_EXIT_ON_AUDIO_LINK.
 *     These values can also be OR'd together to enable both (See 
 *     HfgSniffExitPolicy).
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 * 
 *     Policy - Bitmask that defines the policy for exiting sniff mode.
 *
 * Returns:
 *
 *     BT_STATUS_SUCCESS - The policy was set successfully.
 *
 *     BT_STATUS_NOT_FOUND - Could not set the sniff policy, because  
 *         Handler is not registered.
 */
BtStatus hfg_set_sniff_exit_policy(HfgChannel *Channel, HfgSniffExitPolicy Policy);
#define hfg_set_sniff_exit_policy(c, p) CMGR_SetSniffExitPolicy(&((c)->cmgrHandler), (p));

/*---------------------------------------------------------------------------
 * hfg_get_sniff_exit_policy()
 *
 *     Gets the policy for exiting sniff mode on the specified channel.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 * 
 * Returns:
 *
 *     HfSniffExitPolicy
 */
HfgSniffExitPolicy hfg_get_sniff_exit_policy(HfgChannel *Channel);
#define hfg_get_sniff_exit_policy(c)  CMGR_GetSniffExitPolicy(&((c)->cmgrHandler))

/*---------------------------------------------------------------------------
 * hfg_set_master_role()
 *
 *     Attempts to keep the local device in the Master role.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Flag - TRUE if this device wants to be the master, otherwise FALSE.
 * 
 * Returns:
 *
 *     BtStatus
 */
BtStatus hfg_set_master_role(HfgChannel *Channel, BOOL Flag);
#define hfg_set_master_role(c, f)  CMGR_SetMasterRole(&((c)->cmgrHandler), f)

/*******************************************************************
*   Phonebook related result
*******************************************************************/
BtStatus hfg_send_pb_supported_storage_rsp(HfgChannel *Channel, HfgPbStorageSupported *supported, HfgResponse *Response);
#define hfg_send_pb_supported_storage_rsp(c, s, r)  hfg_send_hf_results(c, (AT_SELECT_PHONEBOOK_STORAGE|AT_TEST), (U32)s, sizeof(HfgPbStorageSupported), r, TRUE)

BtStatus hfg_send_pb_selected_storage_rsp(HfgChannel *Channel, HfgPbStorageSelected *selected, HfgResponse *Response);
#define hfg_send_pb_selected_storage_rsp(c, s, r)  hfg_send_hf_results(c, (AT_SELECT_PHONEBOOK_STORAGE|AT_READ), (U32)s, sizeof(HfgPbStorageSelected), r, TRUE)

BtStatus hfg_send_pb_read_info_rsp(HfgChannel *Channel, HfgPbReadInfo *info, HfgResponse *Response);
#define hfg_send_pb_read_info_rsp(c, s, r)  hfg_send_hf_results(c, (AT_READ_PHONEBOOK_ENTRY|AT_TEST), (U32)s, sizeof(HfgPbReadInfo), r, TRUE)

BtStatus hfg_send_pb_read_rsp(HfgChannel *Channel, HfgPbEntry *entry, HfgResponse *Response, BOOL FinalFlag);
#define hfg_send_pb_read_rsp(c, s, r, f)  hfg_send_hf_results(c, AT_READ_PHONEBOOK_ENTRY, (U32)s, sizeof(HfgPbEntry), r, f)

BtStatus hfg_send_pb_find_info_rsp(HfgChannel *Channel, HfgPbFindInfo *info, HfgResponse *Response);
#define hfg_send_pb_find_info_rsp(c, s, r)  hfg_send_hf_results(c, (AT_FIND_PHONEBOOK_ENTRY|AT_TEST), (U32)s, sizeof(HfgPbFindInfo), r, TRUE)

BtStatus hfg_send_pb_find_rsp(HfgChannel *Channel, HfgPbEntry *entry, HfgResponse *Response, BOOL FinalFlag);
#define hfg_send_pb_find_rsp(c, s, r, f)  hfg_send_hf_results(c, AT_FIND_PHONEBOOK_ENTRY, (U32)s, sizeof(HfgPbEntry), r, f)

BtStatus hfg_send_pb_write_info_rsp(HfgChannel *Channel, HfgPbWriteInfo *info, HfgResponse *Response);
#define hfg_send_pb_write_info_rsp(c, s, r)  hfg_send_hf_results(c, (AT_WRITE_PHONEBOOK_ENTRY|AT_TEST), (U32)s, sizeof(HfgPbWriteInfo), r, TRUE)

/*******************************************************************
*   SMS related result
*******************************************************************/
/* MSG_ID_BT_HFG_SUPPORTED_SMS_SERVICE_REQ  */
BtStatus hfg_send_sms_supported_service_rsp(HfgChannel *Channel, const char *supported, HfgResponse *Response);
#define hfg_send_sms_supported_service_rsp(c, s, r)  hfg_send_hf_results(c, (AT_SELECT_SMS_SERVICE|AT_TEST), (U32)s, 0, r, TRUE)

/* MSG_ID_BT_HFG_SELECTED_SMS_SERVICE_REQ       */  
BtStatus hfg_send_sms_selected_service_rsp(HfgChannel *Channel, HfgSMSService_read *selected, HfgResponse *Response);
#define hfg_send_sms_selected_service_rsp(c, s, r)  hfg_send_hf_results(c, (AT_SELECT_SMS_SERVICE|AT_READ), (U32)s, sizeof(HfgSMSService_read), r, TRUE)

/* MSG_ID_BT_HFG_SMS_SERVICE_REQ                */
BtStatus hfg_send_sms_select_service_rsp(HfgChannel *Channel, HfgSMSService_result *service, HfgResponse *Response);
#define hfg_send_sms_select_service_rsp(c, s, r)  hfg_send_hf_results(c, AT_SELECT_SMS_SERVICE, (U32)s, sizeof(HfgSMSService_result), r, FALSE)

/* MSG_ID_BT_HFG_SUPPORTED_PREF_MSG_STORAGE_REQ             */
BtStatus hfg_send_sms_supported_pref_storage_rsp(HfgChannel *Channel, HfgSMSPrefStorage_test *supported, HfgResponse *Response);
#define hfg_send_sms_supported_pref_storage_rsp(c, s, r)  hfg_send_hf_results(c, (AT_PREFERRED_SMS_STORAGE|AT_TEST), (U32)s, sizeof(HfgSMSPrefStorage_test), r, TRUE)

/* MSG_ID_BT_HFG_SELECTED_PREF_MSG_STORAGE_REQ              */
BtStatus hfg_send_sms_selected_pref_storage_rsp(HfgChannel *Channel, HfgSMSPrefStorage_read *selected, HfgResponse *Response);
#define hfg_send_sms_selected_pref_storage_rsp(c, s, r)  hfg_send_hf_results(c, (AT_PREFERRED_SMS_STORAGE|AT_READ), (U32)s, sizeof(HfgSMSPrefStorage_read), r, TRUE)

/* MSG_ID_BT_HFG_PREF_MSG_STORAGE_REQ                           */
BtStatus hfg_send_sms_select_pref_storage_rsp(HfgChannel *Channel, HfgSMSPrefStorage_result *pref, HfgResponse *Response);
#define hfg_send_sms_select_pref_storage_rsp(c, s, r)  hfg_send_hf_results(c, AT_PREFERRED_SMS_STORAGE, (U32)s, sizeof(HfgSMSPrefStorage_result), r, TRUE)

/* MSG_ID_BT_HFG_SUPPORTED_MSG_FORMAT_REQ                       */  
BtStatus hfg_send_sms_supported_format_rsp(HfgChannel *Channel, const char *supported, HfgResponse *Response);
#define hfg_send_sms_supported_format_rsp(c, s, r)  hfg_send_hf_results(c, (AT_SMS_MESSAGE_FORMAT|AT_TEST), (U32)s, 0, r, TRUE)

/* MSG_ID_BT_HFG_SELECTED_MSG_FORMAT_REQ                        */
BtStatus hfg_send_sms_selected_format_rsp(HfgChannel *Channel, U8 format, HfgResponse *Response);
#define hfg_send_sms_selected_format_rsp(c, s, r)  hfg_send_hf_results(c, (AT_SMS_MESSAGE_FORMAT|AT_READ), (U32)s, 0, r, TRUE)

/* MSG_ID_BT_HFG_SERVICE_CENTRE_REQ                         */
BtStatus hfg_send_sms_service_centre_rsp(HfgChannel *Channel, HfgSMSSrviceCentre_read *sc, HfgResponse *Response);
#define hfg_send_sms_service_centre_rsp(c, s, r)  hfg_send_hf_results(c, (AT_SMS_SERVICE_CENTER|AT_READ), (U32)s, sizeof(HfgSMSSrviceCentre_read), r, TRUE)

/* MSG_ID_BT_HFG_TEXT_MODE_PARAMS_REQ                           */
BtStatus hfg_send_sms_text_mode_params_rsp(HfgChannel *Channel, HfgSMSTextModeParam_read *params, HfgResponse *Response);
#define hfg_send_sms_text_mode_params_rsp(c, s, r)  hfg_send_hf_results(c, (AT_SET_TEXT_MODE_PARMS|AT_READ), (U32)s, sizeof(HfgSMSTextModeParam_read), r, TRUE)

/* MSG_ID_BT_HFG_SUPPORTED_SHOW_PARAMS_REQ                  */
BtStatus hfg_send_sms_supported_show_params_rsp(HfgChannel *Channel, const char *supported, HfgResponse *Response);
#define hfg_send_sms_supported_show_params_rsp(c, s, r)  hfg_send_hf_results(c, (AT_SMS_SHOW_TEXT_MODE|AT_TEST), (U32)s, 0, r, TRUE)

/* MSG_ID_BT_HFG_SELECTED_SHOW_PARAMS_REQ                       */
BtStatus hfg_send_sms_selected_show_params_rsp(HfgChannel *Channel, U8 show, HfgResponse *Response);
#define hfg_send_sms_selected_show_params_rsp(c, s, r)  hfg_send_hf_results(c, (AT_SMS_SHOW_TEXT_MODE|AT_READ), (U32)s, 0,r, TRUE)

/* MSG_ID_BT_HFG_SUPPORTED_NEW_MSG_INDICATION_REQ           */
BtStatus hfg_send_sms_supported_new_msg_indication_rsp(HfgChannel *Channel, const char *supported, HfgResponse *Response);
#define hfg_send_sms_supported_new_msg_indication_rsp(c, s, r)  hfg_send_hf_results(c, (AT_NEW_MESSAGE_INDICATION|AT_TEST), (U32)s, 0, r, TRUE)

/* MSG_ID_BT_HFG_SELECTED_NEW_MSG_INDICATION_REQ                */
BtStatus hfg_send_sms_selected_new_msg_indication_rsp(HfgChannel *Channel, HfgSMSIndSetting_read *selected, HfgResponse *Response);
#define hfg_send_sms_selected_new_msg_indication_rsp(c, s, r)  hfg_send_hf_results(c, (AT_NEW_MESSAGE_INDICATION|AT_READ), (U32)s, sizeof(HfgSMSIndSetting_read), r, TRUE)

/* MSG_ID_BT_HFG_NEW_MSG_INDICATION_REQ             */
BtStatus hfg_send_sms_new_msg_indication(HfgChannel *Channel, HfgSMSNewMsgInd *newMsg, HfgResponse *Response);
#define hfg_send_sms_new_msg_indication(c, s, r)  hfg_send_hf_results(c, AT_SMS_DELIVER_INDICATION, (U32)s, sizeof(HfgSMSNewMsgInd), r, FALSE)

/* MSG_ID_BT_HFG_SUPPORTED_LIST_STATUS_REQ                      */
BtStatus hfg_send_sms_supported_list_status_rsp(HfgChannel *Channel, const char *supported, HfgResponse *Response);
#define hfg_send_sms_supported_list_status_rsp(c, s, r)  hfg_send_hf_results(c, (AT_LIST_MESSAGES|AT_TEST), (U32)s, 0, r, TRUE)

/* MSG_ID_BT_HFG_LIST_MSG_REQ                                   */
BtStatus hfg_send_sms_list_msg_rsp(HfgChannel *Channel, HfgSMSList_result *info, HfgResponse *Response, BOOL FinalFlag);
#define hfg_send_sms_list_msg_rsp(c, s, r, f)  hfg_send_hf_results(c, AT_LIST_MESSAGES, (U32)s, sizeof(HfgSMSList_result), r, f)

/* MSG_ID_BT_HFG_READ_MSG_REQ                                   */
BtStatus hfg_send_sms_read_msg_rsp(HfgChannel *Channel, HfgSMSRead_result *info, HfgResponse *Response);
#define hfg_send_sms_read_msg_rsp(c, s, r)  hfg_send_hf_results(c, AT_READ_MESSAGE, (U32)s, sizeof(HfgSMSRead_result), r, TRUE)

/* MSG_ID_BT_HFG_SEND_MSG_REQ                                   */
BtStatus hfg_send_sms_send_msg_rsp(HfgChannel *Channel, HfgSMSSend_result *rsp, HfgResponse *Response);
#define hfg_send_sms_send_msg_rsp(c, s, r)  hfg_send_hf_results(c, AT_SEND_MESSAGE, (U32)s, sizeof(HfgSMSSend_result), r, TRUE)

/* MSG_ID_BT_HFG_SEND_STORED_MSG_REQ                            */
BtStatus hfg_send_sms_send_stored_msg_rsp(HfgChannel *Channel, HfgSMSSendStored_result *rsp, HfgResponse *Response);
#define hfg_send_sms_send_stored_msg_rsp(c, s, r)  hfg_send_hf_results(c, AT_SEND_STORED_MESSAGE, (U32)s, sizeof(HfgSMSSendStored_result), r, TRUE)

/* MSG_ID_BT_HFG_WRITE_MSG_REQ                                  */
BtStatus hfg_send_sms_write_msg_rsp(HfgChannel *Channel, U16 index, HfgResponse *Response);
#define hfg_send_sms_write_msg_rsp(c, s, r)  hfg_send_hf_results(c, AT_STORE_MESSAGE, (U32)s, 0, r, TRUE)

/* MSG_ID_BT_HFG_WRITE_MSG_REQ                                  */
BtStatus hfg_send_sms_error(HfgChannel *Channel, U16 error, HfgResponse *Response);
#define hfg_send_sms_error(c, e, r)  hfg_send_hf_results(c, AT_SMS_FAILURE_RESULT, (U32)e, 0, r, FALSE)

#endif /* __BT_HFG_H__ */
