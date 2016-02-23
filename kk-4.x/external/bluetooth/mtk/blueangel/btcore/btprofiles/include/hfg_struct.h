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

#ifndef __HFG_STRUCT_H__
#define __HFG_STRUCT_H__

#include "rfcomm_adp.h"
#include "atp.h"
#include "conmgr.h"

#include "at_hf.h"
#include "bluetooth_hfg_common.h"
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

/*---------------------------------------------------------------------------
 * HFG_FEATURE_THREE_WAY_CALLS constant
 *     Three way calls.  This value can be used when defining the capabilities 
 *     of the audio gateway application.
 *     See HFG_SDK_FEATURES.  
 */
#define HFG_FEATURE_THREE_WAY_CALLS      0x00000001

/*---------------------------------------------------------------------------
 * HFG_FEATURE_ECHO_NOISE constant
 *     Echo canceling and/or noise reduction function.  This value can be used 
 *     when defining the capabilities of the audio gateway application.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_ECHO_NOISE           0x00000002

/*---------------------------------------------------------------------------
 * HFG_FEATURE_VOICE_RECOGNITION constant
 *     Voice recognition function.  This value can be used when defining the 
 *     capabilities of the audio gateway application.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_VOICE_RECOGNITION    0x00000004

/*---------------------------------------------------------------------------
 * HFG_FEATURE_RING_TONE constant
 *     In-band ring tone.  This value can be used when defining the capabilities 
 *     of the audio gateway application.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_RING_TONE            0x00000008

/*---------------------------------------------------------------------------
 * HFG_FEATURE_VOICE_TAG constant
 * Attach a phone number to a voice tag.  This value can be used when defining 
 * the capabilities of the audio gateway application.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_VOICE_TAG            0x00000010

/*---------------------------------------------------------------------------
 * HFG_FEATURE_REJECT constant
 *     Reject a call.  This value can be used when defining the capabilities 
 *     of the audio gateway application. 
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_REJECT               0x00000020


/*---------------------------------------------------------------------------
 * HFG_FEATURE_ENHANCED_CALL_STATUS constant
 *     This device is capable of supporting enhanced call status features
 *     like call listing and call held indications.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_ENHANCED_CALL_STATUS 0x00000040

/*---------------------------------------------------------------------------
 * HFG_FEATURE_ENHANCED_CALL_CTRL constant
 *     This device is capable of supporting enhanced call control features
 *     like call specifying specific lines to put on hold.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_ENHANCED_CALL_CTRL   0x00000080

/*---------------------------------------------------------------------------
 * HFG_FEATURE_EXTENDED_ERRORS constant
 *     This device is capable of sending extended error codes.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_EXTENDED_ERRORS      0x00000100

/*---------------------------------------------------------------------------
 * HFG_FEATURE_CODEC_NEGOTIATION constant
 *     This device is capable of codec negotiation.
 *     See HFG_SDK_FEATURES.
 */
#define HFG_FEATURE_CODEC_NEGOTIATION    0x00000200

/*---------------------------------------------------------------------------
 * HFG_TX_BUFFER_SIZE constant
 *     Defines the maximum size of AT response data that can be transmitted.
 *     The default is large enough to handle all AT responses supported by
 *     the Hands-free SDK  If raw AT commands are sent that are larger than the
 *     default value, then this number must be increased.
 */
#ifndef HFG_TX_BUFFER_SIZE
#define HFG_TX_BUFFER_SIZE       256//1
#endif

/*---------------------------------------------------------------------------
 * HFG_RECV_BUFFER_SIZE constant
 *     Defines the maximum size of AT command data that can be received.
 *     The default is large enough to handle all AT commands supported by
 *     the handsfree profile.
 */
#ifndef HFG_RECV_BUFFER_SIZE
#define HFG_RECV_BUFFER_SIZE     256
#endif

/*---------------------------------------------------------------------------
 * HFG_USE_RESP_HOLD constant
 *     Enables the use of the Response and Hold feature, which is supported
 *     in the Japanese market.
 */
#ifndef HFG_USE_RESP_HOLD
#define HFG_USE_RESP_HOLD XA_ENABLED
#endif


/*---------------------------------------------------------------------------
 * HFG_SNIFF_TIMER constant
 *     Enable sniff mode after a defined timeout.  Sniff mode is entered
 *     when a service link is active, but no audio connection is up.  This
 *     is a power saving feature.  If this value is defined to -1, then
 *     sniff mode is disabled.
 */
#ifndef HFG_SNIFF_TIMER
#define HFG_SNIFF_TIMER 2000
#endif

/*---------------------------------------------------------------------------
 * HFG_SNIFF_MIN_INTERVAL constant
 *     Miniumum interval for sniff mode if enabled (see HFG_SNIFF_TIMER).
 *
 */
#ifndef HFG_SNIFF_MIN_INTERVAL
#define HFG_SNIFF_MIN_INTERVAL 800
#endif

/*---------------------------------------------------------------------------
 * HFG_SNIFF_MAX_INTERVAL constant
 *     Maximum interval for sniff mode if enabled (see HFG_SNIFF_TIMER).
 *
 */
#ifndef HFG_SNIFF_MAX_INTERVAL
#define HFG_SNIFF_MAX_INTERVAL 8000
#endif

/*---------------------------------------------------------------------------
 * HFG_SNIFF_ATTEMPT constant
 *     Sniff attempts for sniff mode if enabled (see HFG_SNIFF_TIMER).
 *
 */
#ifndef HFG_SNIFF_ATTEMPT
#define HFG_SNIFF_ATTEMPT 1600
#endif

/*---------------------------------------------------------------------------
 * HFG_SNIFF_TIMEOUT constant
 *     Sniff timeout for sniff mode if enabled (see HFG_SNIFF_TIMER).
 *
 */
#ifndef HFG_SNIFF_TIMEOUT
#define HFG_SNIFF_TIMEOUT 1600
#endif

/*---------------------------------------------------------------------------
 * HFG_MEMORY_EXTERNAL constant
 *     Memory is allocated externally.  When this constant is defined as
 *     XA_ENABLED, global context memory is not allocated by the Hands-free 
 *     profile code, and therfore must be allocated external to this module.  
 *     If memory is allocated external to this module, if must be allocated 
 *     using the context variable type defined in hfgalloc.h, and the entire 
 *     structure must be initialized to 0. In addition, the macro HFG() must 
 *     be defined so that the context memory can be accessed local to this module.
 *
 *     See hfgalloc.h and hfgalloc.c for an example.
 */
#ifndef HFG_MEMORY_EXTERNAL
#define HFG_MEMORY_EXTERNAL XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * HFG_SECURITY_SETTINGS constant
 *     Defines the security settings for a hands-free connection.  The default
 *     is set to BSL_DEFAULT (see BtSecurityLevel).
 */
#ifndef HFG_SECURITY_SETTINGS
#define HFG_SECURITY_SETTINGS BSL_DEFAULT
#endif                               

/*---------------------------------------------------------------------------
 * HFG_MAX_AUDIO_RETRY_COUNT constant
 *     Defines the max retry count for audio connection.
 */
#ifndef HFG_MAX_AUDIO_RETRY_COUNT
#define HFG_MAX_AUDIO_RETRY_COUNT 10
#endif

/*---------------------------------------------------------------------------
 * HFG_AUDIO_RETRY_INTERVAL constant
 *     Defines the interval for audio connection retry.
 */
#ifndef HFG_AUDIO_RETRY_INTERVAL
#define HFG_AUDIO_RETRY_INTERVAL 200
#endif

/*---------------------------------------------------------------------------
 * HFG_CODEC_NEG_MAX_COUNT constant
 *     Defines the max retry count for codec negotiation.
 */
#ifndef HFG_CODEC_NEG_MAX_COUNT
#define HFG_CODEC_NEG_MAX_COUNT 1
#endif

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * HfgEvent type
 *
 *     All indications and confirmations are sent through a callback
 *     function. Depending on the event, different elements of
 *     HfgCallbackInfo "parms" union will be valid as described below.
 */
typedef U8 HfgEvent;

/** An incoming service level connection is being established.  This happens
 *  when the hands-free device establishes the service connection.
 *  The data connection is not available yet for sending to the hands-free 
 *  device.  When the HFG_EVENT_SERVICE_CONNECTED event is received, the 
 *  channel is available for sending.
 *
 *  When this callback is received, the "HfgCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.
 */
#define HFG_EVENT_SERVICE_CONNECT_REQ        0

/** A service level connection has been established.  This can happen as the
 *  result of a call to HFG_CreateServiceLink, or if the hands-free device 
 *  establishes the service connection.  When this event has been received, a 
 *  data connection is available for sending to the hands-free device.
 *
 *  This event can also occur when an attempt to create a service level 
 *  connection (HFG_CreateServiceLink) fails.
 *
 *  When this callback is received, the "HfgCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.  In addition, the
 *  "HfgCallbackParms.errCode" fields contains the reason for disconnect.
 */
#define HFG_EVENT_SERVICE_CONNECTED          1

/** The service level connection has been released.  This can happen as the
 *  result of a call to HFG_DisconnectServiceLink, or if the hands-free device
 *  releases the service connection.  Communication with the hands-free device 
 *  is no longer possible.  In order to communicate with the hands-free device,
 *  a new service level connection must be established.
 *
 *  This event can also occur when an attempt to create a service level 
 *  connection (HFG_CreateServiceLink) fails.
 *
 *  When this callback is received, the "HfgCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.  In addition, the
 *  "HfgCallbackParms.errCode" fields contains the reason for disconnect.
 */
#define HFG_EVENT_SERVICE_DISCONNECTED       2

/** An audio connection has been established.  This event occurs whenever the
 *  audio channel (SCO) comes up, whether it is initiated by the audio gateway
 *  or the hands-free unit.
 *
 *  When this callback is received, the "HfgCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.
 */
#define HFG_EVENT_AUDIO_CONNECTED            3

/** An audio connection has been released.  This event occurs whenever the
 *  audio channel (SCO) goes down, whether it is terminated by the audio gateway
 *  or the hands-free unit.
 *
 *  When this callback is received, the "HfgCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.  In addition, the
 *  "HfgCallbackParms.errCode" fields contains the reason for disconnect.
 */
#define HFG_EVENT_AUDIO_DISCONNECTED         4
 
/** After the service level connection has been established, this event will
 *  indicate the features supported on the hands-free unit.  
 *
 *  When this callback is received, the "HfgCallbackParms.p.features" field 
 *  contains the features (see HfgFeatures).
 */
#define HFG_EVENT_HANDSFREE_FEATURES         5

#if BT_SCO_HCI_DATA == XA_ENABLED
/** Only valid if BT_SCO_HCI_DATA is set to XA_ENABLED.  Audio data has been 
 *  received from the remote device.  The data is only valid during the
 *  callback.
 *
 *  When this callback is received, the "HfCallbackParms.p.audioData" field 
 *  contains the audio data.
 */
#define HFG_EVENT_AUDIO_DATA                 6

/** Only valid if BT_SCO_HCI_DATA is set to XA_ENABLED.  Audio data has been 
 *  sent to the remote device.  This event is received by the application when
 *  the data sent by HFG_SendAudioData has been successfully sent.
 *
 *  When this callback is received, the "HfCallbackParms.p.audioPacket" field 
 *  contains the result.
 */
#define HFG_EVENT_AUDIO_DATA_SENT            7
#endif

/** The hands-free unit has requested the audio gateway to answer the incoming 
 *  call.  When the call has been answered, the application should call 
 *  HFG_SendOK().  In addtion, the appropriate indicators should be updated.
 */
#define HFG_EVENT_ANSWER_CALL                9

/** The hands-free unit has requested the audio gateway to place a call. The
 *  application should dial the number and respond with a call to HFG_SendOK().
 *  In addtion, the appropriate indicators should be updated 
 *  (see HFG_SetIndicatorValue()).
 *
 *  When this callback is received, the "HfgCallbackParms.p.phoneNumber" 
 *  parameter is valid for this event.
 */
#define HFG_EVENT_DIAL_NUMBER               10

/** The hands-free unit has requested the audio gateway to place a call to 
 *  the phone number associated with the given memory location.  The
 *  application should dial the number and respond with a call to HFG_SendOK().
 *  In addtion, the appropriate indicators should be updated 
 *  (see HFG_SetIndicatorValue()).
 *
 *  When this callback is received, the "HfgCallbackParms.p.memory" parameter 
 *  is valid for this event.
 */
#define HFG_EVENT_MEMORY_DIAL               11

/** The hands-free unit has requested the audio gateway to place a call to 
 *  the last number dialed.  The application should dial the last number dialed
 *  and respond with a call to HFG_SendOK().  In addtion, the appropriate 
 *  indicators should be updated (see HFG_SetIndicatorValue()).
 */
#define HFG_EVENT_REDIAL                    12

/** The hands-free unit has requested the audio gateway to place the current 
 *  call on hold.  The application should make the appropriate hold action
 *  and respond with a cll to HFG_SendOK().  In addtion, the appropriate 
 *  indicators should be updated (see HFG_SetIndicatorValue()).
 *
 *  When this callback is received, the "HfgCallbackParms.p.hold" parameter 
 *  is valid for this event.
 */
#define HFG_EVENT_CALL_HOLD                 13

#if HFG_USE_RESP_HOLD == XA_ENABLED
/** This event is only available when HFG_USE_RESP_HOLD is set to XA_ENABLED.
 *  The Response and Hold state has been requested from the handsfree device.
 *  The audio gateway should respond by calling HFG_SendResponseHoldState().
 *  If the the audio gateway is in any Response and Hold state, then the 
 *  application should respond by calling HFG_SendResponseHoldState() with
 *  the state set to HFG_RESP_HOLD_STATE_HOLD, followed by a call to
 *  HFG_SendOK(), othwerwise, the application should simply call HFG_SendOK()
 *  
 */
#define HFG_EVENT_QUERY_RESPONSE_HOLD              14

/** This event is only available when HFG_USE_RESP_HOLD is set to XA_ENABLED.
 *  A Response and Hold request has been received from the handsfree device.
 *  The audio gateway should take the appropriate action and respond by calling
 *  HFG_SendResponseHoldState() with the state set to the requested state.  
 *  In addtion, the appropriate indicators should be updated 
 *  (see HFG_SetIndicatorValue()).
 * 
 *  When this callback is received, the "HfCallbackParms.p.respHold" field 
 *  contains the result.
 */
#define HFG_EVENT_RESPONSE_HOLD                    15
#endif

/** The hands-free unit has requested the audio gateway to hang up the current 
 *  call.  The application should hang up the call and respond with a call
 *  to HFG_SendOK().  In addtion, the appropriate indicators should be updated 
 *  (see HFG_SetIndicatorValue()).
 */
#define HFG_EVENT_HANGUP                    16

/** The list of current calls has been requested from the hands-free device.
 *  The audio gateway should respond by calling HFG_SendCallListRsp() once
 *  for each line supported by the device.
 */                                     
#define HFG_EVENT_LIST_CURRENT_CALLS        17

/** The hands-free unit has requested that Calling Line Identification
 * notification be enabled or disabled.  
 *
 * The "HfgCallbackParms.p.enabled" parameter indicates the type of request. 
 * If "enabled" is FALSE, the application may call HFG_SendCallerId(), but no
 * indication will be sent to the remote device.
 */
#define HFG_EVENT_ENABLE_CALLER_ID          18

/** The hands-free unit has requested that Call Waiting notification be enabled
 *  or disabled. 
 *
 *  The "HfgCallbackParms.p.enabled" parameter indicates the type of request.  
 *  If "enabled" is FALSE, the application may call HFG_CallWaiting(), but no
 *  indication will be sent to the remote device.
 */
#define HFG_EVENT_ENABLE_CALL_WAITING       19

/** The hands-free unit has requested the audio gateway to transmit a 
 *  specific DTMF code to its network connection. The "dtmf" parameter 
 *  will contain the requested DTMF code.
 *
 *  When the tone has been sent to the network, call HFG_SendOk().
 */
#define HFG_EVENT_GENERATE_DTMF             20


/** The hands-free unit has requested the phone number associated with the
 *  last voice tag in the audio gateway in order to link its own voice tag to 
 *  the number.  The hands-free unit may then dial the linked phone numbers 
 *  when a voice tag is recognized.  (This procedure is only applicable for 
 *  hands-free units that support their own voice recognition functionality).
 *  The HFG_FEATURE_VOICE_TAG bit must be set in HFG_SDK_FEATURES in order
 *  to use this event.
 *
 *  In response to this event, call HFG_VoiceTagResponse() with a number for
 *  tagging, or call HFG_SendError() to reject the request.
 */
#define HFG_EVENT_GET_LAST_VOICE_TAG        21



/** Enables/disables the voice recognition function resident in the audio
 *  gateway (as indicated by the "enabled" parameter). If the HF enables voice
 *  recognition, the audio gateway must keep the voice recognition enabled 
 *  until either:
 *
 *      1) The HF disables it.
 *      2) The service link is disconnected.
 *      3) The duration of time supported by the audio gateway's 
 *         implementation has elapsed.
 *
 * In this last case the audio gateway must notify the hands-free unit that 
 * it has disabled voice recognition by calling HFG_DisableVoiceRec().
 * The HFG_FEATURE_VOICE_RECOGNITION bit must be set in HFG_SDK_FEATURES in order
 * to receive this event.
 */
#define HFG_EVENT_ENABLE_VOICE_RECOGNITION  22



/** The hands-free unit has requested the audio gateway to disable the noise
 *  reduction and echo canceling (NREC) functions resident in the audio 
 *  gateway.
 *
 *  If the audio gateway supports NREC it must disable these features for 
 *  the duration of the service link.  The HFG_FEATURE_ECHO_NOISE bit must be 
 *  set in HFG_SDK_FEATURES in order to receive this event.
 */
#define HFG_EVENT_DISABLE_NREC              23


/** The hands-free has informed the audio gateway of its microphone volume 
 *  level. 
 *
 *  The "HfgCallbackParms.p.gain" parameter is valid.
 */
#define HFG_EVENT_REPORT_MIC_VOLUME         24

/** The hands-free has informed the audio gateway of its speaker volume 
 *  level. 
 *
 *  The "HfgCallbackParms.p.gain" parameter is valid.
 */
#define HFG_EVENT_REPORT_SPK_VOLUME         25

/** The hands-free device has requested the network operator from the
 *  audio gateway.  The audio gateway should respond by calling
 *  HFG_SendNetworkOperatorRsp().
 */
#define HFG_EVENT_QUERY_NETWORK_OPERATOR    26
 
/** The hands-free device has requested the subscriber number from the
 *  audio gateway.  The audio gateway should respond by calling
 *  HFG_SendSubscriberNumberRsp().
 */
#define HFG_EVENT_QUERY_SUBSCRIBER_NUMBER   27

/** The hands-free device has requested that extended error codes be enabled.
 *  When extended errors are enabled, a call to HFG_SendError() will send
 *  extended errors, otherwise it will only repspond with an "ERROR" response.
 */
#define HFG_EVENT_ENABLE_EXTENDED_ERRORS    28

/** An unsupported AT command has been received from the audio gateway.  This 
 *  event is received for AT commands that are not handled by the internal 
 *  Hands-free AT parser.  The application must make an appropriate response
 *  and call HFG_SendOK() to complete the response.
 *
 *  When this callback is received, the "HfgCallbackParms.p.data" field 
 *  contains the AT command data.
 */
#define HFG_EVENT_AT_COMMAND_DATA           29

/** Whenever a response has been set to the remote device, this event is
 *  received to confirm that the repsonse was sent.
 * 
 *  When this event is received, the "HfgCallbackParms.p.response" field
 *  contains a pointer to the response structure that was used to send
 *  the reponse.
 */
#define HFG_EVENT_RESPONSE_COMPLETE         30

/** 
 *  When RFCOMM is connected, HFP still have to run negotiation process to 
 *  complete the SLC connection. HFP sends HFG_EVENT_RFCOMM_CONNECTED
 *  to inform ADP the RFCOMM channel is connected and ready to negotiate. ADP
 *  can can do some proprietary actions.
 */
#define HFG_EVENT_RFCOMM_CONNECTED      31

/** 
 *  When receive RFEVENT_OPEN_IND, HFP request ADP by sending HFG_EVENT_AUTH_REQ
 *  to confirm if it want to accept the connection request.
 */
#define HFG_EVENT_AUTH_REQ              32

/** 
 *  When creating SLC, HFP need to negotiation with HF device. HF device have to read indicator 
 *  values from AG. HFP will receive AT+CIND CMD and it does not know the current values of indicators.
 *  So HFP will send event HFG_EVENT_READ_INDICATORS to ADP and ADP has to pass current values 
 *  of indicators to HFP.
 */
#define HFG_EVENT_READ_INDICATORS       33

/** 
 *  This event indicate the button on the headset is pressed. It shall be only received when connected 
 *  to headset no HF device
 */
#define HFG_EVENT_KEYPAD_CONTROL                34

/** 
 *  This event is only used when the message based interface is used. It is used to confirm the result 
 *  of activation
 */
#define HFG_EVENT_ACTIVATE_CONFIRM           35

/** 
 *  This event is only used when the message based interface is used. It is used to confirm the result 
 *  of deactivation
 */
#define HFG_EVENT_DEACTIVATE_CONFIRM           36

#define HFG_EVENT_CONNECT_CONFIRM               37
#define HFG_EVENT_ACCEPT_CHANNEL_CONFIRM        38
#define HFG_EVENT_REJECT_CHANNEL_CONFIRM        39
#define HFG_EVENT_DISCONNECT_CONFIRM            40
#define HFG_EVENT_SCO_CONNECT_CONFIRM           41
#define HFG_EVENT_SCO_DISCONNECT_CONFIRM        42

/* Indicate the charset is selected */
#define HFG_EVENT_QUERY_SUPPORTED_CHARSET       43
#define HFG_EVENT_QUERY_SELECTED_CHARSET        44
#define HFG_EVENT_SELECT_CHARSET                45

#define HFG_EVENT_QUERY_MODEL_ID                46
#define HFG_EVENT_QUERY_MANUFACTURE_ID          47

/****************************************************************************
*   Phone Book event
****************************************************************************/
#define HFG_EVENT_PB                0x40
/* +CPBS */
/* AT+CPBS=<pb> */
#define HFG_EVENT_QUERY_SUPPORTED_PHONEBOOK  (HFG_EVENT_PB|0x1)  
/* AT+CPBS=? */
#define HFG_EVENT_SELECT_PHONEBOOK           (HFG_EVENT_PB|0x2)  
/* AT+CPBS? */
#define HFG_EVENT_QUERY_SELECTED_PHONEBOOK   (HFG_EVENT_PB|0x3)  

/* +CPBR */
/* AT+CPBR=<n> */
#define HFG_EVENT_QUERY_READ_PBENTRY_INFO    (HFG_EVENT_PB|0x4) 
/* AT+CPBR=? */
#define HFG_EVENT_READ_PBENTRY               (HFG_EVENT_PB|0x5)  

/* +CPBF */
/* AT+CPBF=? */
#define HFG_EVENT_QUERY_FIND_PBENTRY_INFO    (HFG_EVENT_PB|0x6)  
/* AT+CPBF=<n> */
#define HFG_EVENT_FIND_PBENTRY               (HFG_EVENT_PB|0x7)  

/* +CPBW */
/* AT+CPBW */
#define HFG_EVENT_QUERY_WRITE_PBENTRY_INFO   (HFG_EVENT_PB|0x8) 
/* AT+CPBW=? */
#define HFG_EVENT_WRITE_PBENTRY              (HFG_EVENT_PB|0x9)  
/* End of HfgEvent */

/****************************************************************************
*   SMS event
****************************************************************************/
#define HFG_EVENT_SMS                0x80
/****************************/
/* AT_SELECT_SMS_SERVICE    */
/****************************/
/* MSG_ID_BT_HFG_QUERY_SUPPORTED_SMS_SERVICE_IND */
/* AT+CSMS=? */
#define HFG_EVENT_QUERY_SUPPORTED_SMS_SERVICE               (HFG_EVENT_SMS|0x1) 
/* MSG_ID_BT_HFG_QUERY_SELECTED_SMS_SERVICE_IND */
/* AT+CSMS? */
#define HFG_EVENT_QUERY_SELECTED_SMS_SERVICE                (HFG_EVENT_SMS|0x2) 
/* MSG_ID_BT_HFG_SELECT_SMS_SERVICE_IND */
/* AT+CSMS=<service> */
#define HFG_EVENT_SELECT_SMS_SERVICE                        (HFG_EVENT_SMS|0x3) 

/* AT_PREFERRED_SMS_STORAGE */
/* AT+CPMS=? */
#define HFG_EVENT_QUERY_SUPPORTED_PREF_MSG_STORAGE          (HFG_EVENT_SMS|0x4) 
/* AT+CPMS? */
#define HFG_EVENT_QUERY_SELECTED_PREF_MSG_STORAGE           (HFG_EVENT_SMS|0x5) 
/* AT+CPMS=<mem1>[,<mem2>[,<mem3>]] */
#define HFG_EVENT_SELECT_PREF_MSG_STORAGE                   (HFG_EVENT_SMS|0x6) 

/* AT_SMS_MESSAGE_FORMAT */
/* AT+CMGF=? */
#define HFG_EVENT_QUERY_SUPPORTED_MSG_FORMAT                (HFG_EVENT_SMS|0x7)
/* AT+CMGF? */
#define HFG_EVENT_QUERY_SELECTED_MSG_FORMAT                 (HFG_EVENT_SMS|0x8) 
/* AT+CMGF=<mode> */
#define HFG_EVENT_SELECT_MSG_FORMAT                         (HFG_EVENT_SMS|0x9) 

/* AT_SMS_SERVICE_CENTER */
/* AT+CSCA? */
#define HFG_EVENT_QUERY_SERVICE_CENTRE                      (HFG_EVENT_SMS|0xA)
/* AT+CSCA=<sca>[,<tosca>] */
#define HFG_EVENT_SET_SERVICE_CENTRE                        (HFG_EVENT_SMS|0xB) 

/* AT_SET_TEXT_MODE_PARMS */
/* AT+CSMP? */
#define HFG_EVENT_QUERY_TEXT_MODE_PARAMS                    (HFG_EVENT_SMS|0xC) 
/* AT+CSMP=[<fo>[,<vp>[,<pid>[,<dcs>]]]] */
#define HFG_EVENT_SET_TEXT_MODE_PARAMS                      (HFG_EVENT_SMS|0xD)    

/* AT_SMS_SHOW_TEXT_MODE */
/* AT+CSDH=? */
#define HFG_EVENT_QUERY_SUPPORTED_SHOW_PARAMS               (HFG_EVENT_SMS|0xE) 
/* AT+CSDH? */
#define HFG_EVENT_QUERY_SELECTED_SHOW_PARAMS                (HFG_EVENT_SMS|0xF) 

/* AT+CSMP=[<fo>[,<vp>[,<pid>[,<dcs>]]]] */
#define HFG_EVENT_SET_SHOW_PARAMS                           (HFG_EVENT_SMS|0x10)  

/* AT_NEW_MESSAGE_INDICATION */
/* AT+CNMI=? */
#define HFG_EVENT_QUERY_SUPPORTED_NEW_MSG_INDICATION        (HFG_EVENT_SMS|0x11)  
/* AT+CNMI? */
#define HFG_EVENT_QUERY_SELECTED_NEW_MSG_INDICATION         (HFG_EVENT_SMS|0x12)  
/* AT+CNMI=[<mode>[,<mt>[,<bm>[,<ds>[,<bfr>]]]]] */
#define HFG_EVENT_SET_NEW_MSG_INDICATION                    (HFG_EVENT_SMS|0x13)   

/* AT_LIST_MESSAGES */
/* AT+CMGL=? */
#define HFG_EVENT_QUERY_SUPPORTED_LIST_STATUS               (HFG_EVENT_SMS|0x14) 
/* AT+CMGL[=<stat>] */
#define HFG_EVENT_LIST_MSG                                  (HFG_EVENT_SMS|0x15)    

/* AT_READ_MESSAGE */
/* AT+CMGR=<index> */
#define HFG_EVENT_READ_MSG                                  (HFG_EVENT_SMS|0x16)    

/* AT_SEND_MESSAGE */
/* TEXT MODE : AT+CMGS=<da>[,<toda>]<CR>text is entered<ctrl-Z/ESC> */
/* PDU MODE : AT+CMGS=<length><CR>PDU is given<ctrl-Z/ESC> */
#define HFG_EVENT_SEND_MSG                                  (HFG_EVENT_SMS|0x17) 

/* AT_SEND_STORED_MESSAGE */
/* AT+CMSS=<index>[,<da>[,<toda>]] */
#define HFG_EVENT_SEND_STORED_MSG                           (HFG_EVENT_SMS|0x18) 

/* AT_STORE_MESSAGE */
/* TEXT MODE : AT+CMGW=<oa/da>[,<toda/toda>[,<stat>]]<CR>text is entered<ctrl-Z/ESC> */
/* PDU MODE : AT+CMGW=<length>[,<stat>]<CR>PDU is given<ctrl-Z/ESC> */
#define HFG_EVENT_WRITE_MSG                                 (HFG_EVENT_SMS|0x19)   

/* AT_DELETE_MESSAGE */
/* AT+CMGD=<index> */
#define HFG_EVENT_DELETE_MSG                                (HFG_EVENT_SMS|0x1A)    

/*---------------------------------------------------------------------------
 * HfgSniffExitPolicy type
 *
 * Defines the policy for exiting sniff mode.  While it is possible to both
 * send data a create SCO connections in sniff mode, it may be desired to
 * exit sniff mode for performance or compatibility reasons.
 */
typedef U8 HfgSniffExitPolicy;

/** Exit sniff mode whenever there is data to send.
 */
#define HFG_SNIFF_EXIT_ON_SEND  CMGR_SNIFF_EXIT_ON_SEND

/** Exit sniff mode whenever an audio link is being created.
 */
#define HFG_SNIFF_EXIT_ON_AUDIO CMGR_SNIFF_EXIT_ON_AUDIO

/* End of HfgSniffExitPolicy */

/*--------------------------------------------------------------------------
 * HfgCmeError type
 *
 */
typedef AtError HfgCmeError;

#define HFG_CME_NO_CONNECTION                 ATCME_NO_CONNECTION
#define HFG_CME_OP_NOT_ALLOWED                ATCME_OP_NOT_ALLOWED
#define HFG_CME_OP_NOT_SUPPORTED              ATCME_OP_NOT_SUPPORTED
#define HFG_CME_PH_SIM_PIN_REQUIRED           ATCME_PH_SIM_PIN_REQUIRED
#define HFG_CME_SIM_NOT_INSERTED              ATCME_SIM_NOT_INSERTED
#define HFG_CME_SIM_PIN_REQUIRED              ATCME_SIM_PIN_REQUIRED
#define HFG_CME_SIM_PUK_REQUIRED              ATCME_SIM_PUK_REQUIRED
#define HFG_CME_SIM_FAILURE                   ATCME_SIM_FAILURE
#define HFG_CME_SIM_BUSY                      ATCME_SIM_BUSY
#define HFG_CME_INCORRECT_PASSWORD            ATCME_INCORRECT_PASSWORD
#define HFG_CME_SIM_PIN2_REQUIRED             ATCME_SIM_PIN2_REQUIRED
#define HFG_CME_SIM_PUK2_REQUIRED             ATCME_SIM_PUK2_REQUIRED
#define HFG_CME_MEMORY_FULL                   ATCME_MEMORY_FULL
#define HFG_CME_INVALID_INDEX                 ATCME_INVALID_INDEX
#define HFG_CME_MEMORY_FAILURE                ATCME_MEMORY_FAILURE
#define HFG_CME_TEXT_STRING_TOO_LONG          ATCME_TEXT_STRING_TOO_LONG
#define HFG_CME_INVALID_CHARS_IN_TEXT_STRING  ATCME_INVALID_CHARS_IN_TEXT_STRING
#define HFG_CME_DIAL_STRING_TOO_LONG          ATCME_DIAL_STRING_TOO_LONG
#define HFG_CME_INVALID_CHARS_IN_DIAL_STRING  ATCME_INVALID_CHARS_IN_DIAL_STRING
#define HFG_CME_NO_NETWORK_SERVICE            ATCME_NO_NETWORK_SERVICE
#define HFG_CME_NETWORK_NOT_ALLOWED           ATCME_NETWORK_NOT_ALLOWED
#define HFG_CME_UNKNOWN                       ATCME_UNKNOWN

/* End of HfgCmeError */

/*---------------------------------------------------------------------------
 * HfgResponse type
 *      
 *  Structure used to send AT responses.
 */
typedef AtResults HfgResponse;

/* End of HfgResponse */

/*--------------------------------------------------------------------------
 * HfgAtMode type
 *      
 *     Possible modes for AT result code event reporting from the audio gateway.
 *     Only modes 0 and 3 are covered in the HF spec.
 */
typedef U8  HfgAtMode;

/** Buffer unsolicited result codes in the gateway; if gateway's result code 
 *  buffer is full, codes can be buffered in some other place or the oldest ones can
 *  be discarded.
 */
#define HFG_AT_BUFFER_RESULTS           0x00

/** Forward unsolicited result codes directly to the hands-free unit; 
 *  gateway-handsfree  link specific inband technique used to embed result codes
 *  and data when the gateway is in on-line data mode.
 */
#define HFG_AT_FORWARD_RESULTS          0x03

/* End of HfgAtMode */


/*--------------------------------------------------------------------------
 * HfgAtReport type
 *      
 *     Possible states for AT event reporting from the AG.
 *     Only modes 0 and 1 are covered in the HF spec.
 */
typedef U8 HfgAtReport;

/** No indicator event reporting.
 */
#define HFG_AT_NO_EVENT_REPORTING       0x00

/** Indicator event reporting using result code +CIEV.
 */
#define HFG_AT_EVENT_REPORTING          0x01

/* End of HfgAtReport */

/*--------------------------------------------------------------------------
 * HfgRejectFeature type
 *      
 *     Used as the reject feature in the SDP record and BRSF command.
 */
typedef U8  HfgRejectFeature;

/** No ablility to reject call 
 */
#define HFG_REJECT_NOT_SUPPORTED      0x00

/** Ability to reject supported
 */
#define HFG_REJECT_SUPPORTED          0x01

/* End of HfgRejectFeature */

/*--------------------------------------------------------------------------
 * HfgAvailCodecs type
 *      
 *  Bit mask specifying the HF's available codecs.
 *-------------------------------------------------------------------------*/
typedef U32  HfgAvailCodecs;

#define HFG_AVAIL_CODEC_CVSD          0x00000001
#define HFG_AVAIL_CODEC_MSBC          0x00000002

/*--------------------------------------------------------------------------
 * End of HfgAvailCodecs
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * HfgCodec type
 *      
 *     Code ID for audio connection.
 */
typedef enum
{
    HFG_CODEC_NONE = 0x00,
    HFG_CODEC_CVSD = 0x01,
    HFG_CODEC_MSBC = 0x02,
}HfgCodec;

#define NUM_OF_HFG_CODEC              0x02
/* End of HfgCodec */

/* Forward References */
typedef struct _HfgChannel HfgChannel;

#if BT_SCO_HCI_DATA == XA_ENABLED
/*---------------------------------------------------------------------------
 * HfgAudioData type
 *
 * This type is only available when BT_SCO_HCI_DATA is set to XA_ENABLED.
 * Contains audio data received from the remote device.
 */
typedef CmgrAudioData HfgAudioData;

/* End of HfgAudioData */
#endif

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * HfgCallbackParms structure
 *
 * This structure is sent to the application's callback to notify it of
 * any state changes.
 */
/* event         : Event associated with the callback       */
/* status        : Status of the callback event             */
/* errCode       : Error code (reason) on disconnect events */
/* button        : AT+CKPD=200 */
/* respHold      : Only valid if HFG_USE_RESP_HOLD is set to XA_ENABLED.*/
/* pbStorage     : HFG_EVENT_SELECT_PHONEBOOK */
/* pbRead        : HFG_EVENT_READ_PBENTRY */
/* pbFind        : HFG_EVENT_FIND_PBENTRY */
/* pbWrite       : HFG_EVENT_WRITE_PBENTRY */ 
/* service       : HFG_EVENT_SELECT_SMS_SERVICE */
/* prefStorage   : HFG_EVENT_QUERY_SELECTED_PREF_MSG_STORAGE    */
/* format        : HFG_EVENT_SELECT_MSG_FORMAT(0: PDU mode, 1: Text mode) */
/* serviceCentre : HFG_EVENT_SET_SERVICE_CENTRE */
/* textParams    : HFG_EVENT_SET_TEXT_MODE_PARAMS */
/* show          : HFG_EVENT_SET_SHOW_PARAMS */
/* newMsgInd     : HFG_EVENT_SET_NEW_MSG_INDICATION */
/* stat          : HFG_EVENT_LIST_MSG */
/* readMsgIndex  : HFG_EVENT_READ_MSG */
/* delMsgIndex   : HFG_EVENT_DELETE_MSG */
/* sendMsg       : HFG_EVENT_SEND_MSG */
/* storedMsg     : HFG_EVENT_SEND_STORED_MSG */
/* writeMsg      : HFG_EVENT_WRITE_MSG */
typedef struct _HfgCallbackParms HfgCallbackParms;
struct _HfgCallbackParms 
{
    HfgEvent    event;   
    I8          status;
    U8          errCode;

    /* For certain events, a single member of this union will be valid.
     * See HfgEvent documentation for more information.
     */
    union {
        void                      *ptr;
        void                      *context;
        HfgHandsFreeFeatures       features;
        BOOL                       enabled;
        const char                *phoneNumber;
        const char                *memory;        
        U16                        index;
        U8                         dtmf;
        U8                         gain;
        U8                         button; 
        HfgHold                   *hold;
        HfgHandsFreeVersion        version;
        void                      *response;
        BD_ADDR                   *addr;
        HfgResponseHold            respHold;    

#if BT_SCO_HCI_DATA == XA_ENABLED
        HfgAudioData              *audioData;   
        BtPacket                  *audioPacket; 
#endif
        HfgAtData                 *data;
        const char                *charset;

        U16                        pbStorage;          
        HfgPbRead                 *pbRead;               
        HfgPbFind                 *pbFind;                  
        HfgPbWrite                *pbWrite;                     

        HfgSMSService_cmd          service;            
        HfgSMSPrefStorage_cmd     *prefStorage;   
        HfgSMSFormat_cmd           format;         
        HfgSMSSrviceCentre_cmd    *serviceCentre; 
        HfgSMSTextModeParam_cmd   *textParams;    
        HfgSMSShowParams_cmd       show;           
        HfgSMSIndSetting_cmd      *newMsgInd; 
        HfgSMSList_cmd             stat;           
        HfgSMSRead_cmd             readMsgIndex;   
        HfgSMSDelete_cmd           delMsgIndex;    
        HfgSMSSend_cmd            *sendMsg;       
        HfgSMSSendStored_cmd      *storedMsg;     
        HfgSMSWrite_cmd           *writeMsg;      
    } p;
};

/*---------------------------------------------------------------------------
 * HfgCallback type
 *
 * A function of this type is called to indicate Hands-Free events to
 * the application.
 */
typedef void (*HfgCallback)(HfgChannel *Channel, HfgCallbackParms *Parms);

/* End of HfgCallback */

/*---------------------------------------------------------------------------
 * HfgChannel structure
 *
 *     The Audio gateway channel.  This structure is used to identify a
 *     connection to the hands-free device.
 */
/* type            : Indicate registered as a HSG(0) or HFG(1) */
/* callback        : Application callback function    */
/* rfChannel       : RFCOMM channel used for audio gateway connection. */
/* rspQueue        : List of AT responses             */
/* state           : Current connection state         */
/* flags           : Current connection flags         */
/* linkFlags       : Levels of service connected      */
/* version         : Profile version parsed from SDP  */
/* hfFeatures      : Profile features parsed from SDP */
/* IndIndex        : Index of each indicator */
/* IndicatorValue  : store current indicator value */
/* indEnabled      : Sending of indicators is enabled */
/* service         : Service indicator                */
/* call            : The call indicator               */
/* callSetup       : Call setup indicator             */
/* held            : Call held indicator              */
/* battery         : Battery charge indicator         */
/* signal          : Signal strength indicator        */
/* roaming         : Roaming indicator                */
/* callWaiting     : Call wait notification active    */
/* callId          : Caller ID notification active    */
/* extendedErrors  : Extended errors enabled          */
/* nrecDisable     : Noise Red/Echo Cancel disabled   */
/* voiceRec        : Voice Recogintion active         */
/* ibRing          : In-band ringing active           */
/* sdpQueryToken   : Used to query the service        */
/* queryFlags      : Defines which SDP entries were parsed from the gateway.*/
/* rfServerChannel : When connecting AG client        */

struct _HfgChannel 
{

    /* === Internal use only === */

    /* Registration variables */
    ListEntry            node;  
    U8                   type;     
    HfgCallback          callback;         
    RfChannel           *rfChannel;        
    ListEntry            rspQueue;         

    /* Connection State Variables */
    U8                   state;            
    U16                  flags;            
    U8                   linkFlags;        

    /* HandsFree State Information */
    HfgHandsFreeVersion  version;          
    HfgHandsFreeFeatures hfFeatures;       

    /* headset specific information from SDP */
    BOOL                 bRemoteVolControl;
    
    /* Indicators information */
    U8                   IndIndex[NUM_OF_HFG_IND];     
    U16                  IndicatorValue[NUM_OF_HFG_IND];  
    BOOL                 IndActivation[NUM_OF_HFG_IND];

    /* Indicator status */
    BOOL                 indEnabled;       
    BOOL                 service;          
    BOOL                 call;             
    HfgCallSetupState    callSetup;        
    HfgHoldState         held;             
    U8                   battery;          
    U8                   signal;           
    U8                   roaming;          

    /* Feature State Information */
    BOOL                 callWaiting;      
    BOOL                 callId;           
    BOOL                 extendedErrors;   
    BOOL                 nrecDisable;      
    BOOL                 voiceRec;         
    BOOL                 ibRing;           

    /* SDP variables for client */
    SdpQueryToken        sdpQueryToken;     
    U8                   queryFlags;        
    U8                   rfServerChannel;   

    HfgCHarsetType       charset;
    /* Channel Resources */
    CmgrHandler          cmgrHandler;
    BD_ADDR              bdAddr;
    AtCommands           atParms;
    AtResults            atResults;
    AtResults           *currentAtResults;
    AtResults           *lastAtResults;
    XaBufferDesc         atBuffer;
    XaBufferDesc         atRxBufDesc;
    BtPacket             atTxPacket;
    U8                  *atTxData;
    U16                  bytesToSend;
    U8                  *atRxBuffer;
    U16                  atRxLen;
#if defined(__HFG_BLOCK_SIMULTANEOUS_CONNECTION__)
    kal_bool             bSuspended;
#endif

    HfgDiscReason        reason;    
    U8                   codecState;
    U8                   codecRetryCount;       
    HfgCodec             codecSelected;
    HfgCodec             targetCodec;   
    BOOL                 bCodecNegSupported;
    U32                  availCodecs;
};
#endif /* __BT_HFG_STRUCT_H__ */
