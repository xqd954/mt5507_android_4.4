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
 *     $Workfile:hf.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains public definitions for the Hands-free SDK.
 *             
 * Created:     Februrary 11, 2005
 *
 * Copyright 2000-2005 Extended Systems, Inc.

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

#ifndef __HF_H_
#define __HF_H_

#include "rfcomm_adp.h"
#include "atp.h"
#include "conmgr.h"

/*---------------------------------------------------------------------------
 * Hands-free SDK API layer
 *
 *     The Hands-free SDK is designed to create a hands-free application 
 *     for Bluetooth.  It includes a complete implementation of the Hands-Free 
 *     Profiles v.96, v1.0 and v1.5.  It also includes some features beyond the 
 *     basic profiles that take advantage of features exported by many shipping 
 *     phones.  
 *
 *     The Hands-free SDK provides service discovery, connection management, 
 *     call management, and basic phone book management.  It also allows for 
 *     the transmission and reception of AT commands not specifically
 *     supported by Bluetooth profiles.
 *
 *     This SDK also includes an optional Call Manager component that tracks the
 *     state of calls in the audio gateway.  Many different events are 
 *     generated to provide information about calls on the audio gateway.  The
 *     Call Manager interprets these various events and translates them into a
 *     simplified event form.  The state of all calls are maintained within the
 *     Call Manager and can be queried by the application.  In addition, when
 *     calling functions that will affect the call state, the Call Manager makes
 *     intelligent decisions on which AT commands to send.  The Call Manager
 *     will also use a polling method to keep the call state information up
 *     to date if necessary.
 */

#if NUM_SCO_CONNS == 0
#error NUM_SCO_CONNS must be greater than 0
#endif

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/
 
/*---------------------------------------------------------------------------
 * HF_FEATURE_ECHO_NOISE constant
 *     Echo canceling and/or noise reduction feature.  This value can be
 *     used when defining the capabilities of the hands-free application.
 *     See HF_SDK_FEATURES.
 */
#define HF_FEATURE_ECHO_NOISE           0x00000001

/*---------------------------------------------------------------------------
 * HF_FEATURE_CALL_WAITING constant
 *     Call-waiting and 3-way calling feature.  This value can be
 *     used when defining the capabilities of the hands-free application.
 *     See HF_SDK_FEATURES.
 */
#define HF_FEATURE_CALL_WAITING         0x00000002

/*---------------------------------------------------------------------------
 * HF_FEATURE_CLI_PRESENTATION constant
 *     CLI presentation capability feature.  This value can be used when 
 *     defining the capabilities of the hands-free application.  See 
 *     HF_SDK_FEATURES.
 */
#define HF_FEATURE_CLI_PRESENTATION     0x00000004

/*---------------------------------------------------------------------------
 * HF_FEATURE_VOICE_RECOGNITION constant
 *     Voice recognition function.  This value can be used when defining the 
 *     capabilities of the hands-free application.  See HF_SDK_FEATURES.
 */
#define HF_FEATURE_VOICE_RECOGNITION    0x00000008

/*---------------------------------------------------------------------------
 * HF_FEATURE_VOLUME_CONTROL constant
 *     Remote volume control feature.  This value can be used when defining 
 *     the capabilities of the hands-free application.  See HF_SDK_FEATURES.
 */
#define HF_FEATURE_VOLUME_CONTROL       0x00000010

/*---------------------------------------------------------------------------
 * HF_FEATURE_ENHANCED_CALL_STATUS constant
 *     This device is capable of supporting enhanced call status features
 *     like call listing and call held indications.  See HF_SDK_FEATURES.
 */
#define HF_FEATURE_ENHANCED_CALL_STATUS 0x00000020

/*---------------------------------------------------------------------------
 * HF_FEATURE_ENHANCED_CALL_CTRL constant
 *     This device is capable of supporting enhanced call control features
 *     like call specifying specific lines to put on hold.  See HF_SDK_FEATURES.
 */
#define HF_FEATURE_ENHANCED_CALL_CTRL   0x00000040

/*---------------------------------------------------------------------------
 * HF_SDK_FEATURES constant
 *     Defines the supported features of the hands-free application.  Features
 *     that will be supported by the application must be advertised to the
 *     audio gateway, so that it knows the capabilities of the hands-free unit.
 *     If this value needs to be changed, it can be placed in overide.h with
 *     the features supported by the hands-free application.
 */
#ifndef HF_SDK_FEATURES
#define HF_SDK_FEATURES  (HF_FEATURE_ECHO_NOISE           | \
                          HF_FEATURE_CALL_WAITING         | \
                          HF_FEATURE_CLI_PRESENTATION     | \
                          HF_FEATURE_VOICE_RECOGNITION    | \
                          HF_FEATURE_VOLUME_CONTROL       | \
                          HF_FEATURE_ENHANCED_CALL_STATUS | \
                          HF_FEATURE_ENHANCED_CALL_CTRL)
#endif /* HF_SDK_FEATURES */

/*---------------------------------------------------------------------------
 * HF_COMMAND_TIMEOUT constant
 *     Defines the amount of time to wait until assuming an unresponsive
 *     channel exists.  When this time is up, the outstanding command 
 *     will be flushed and the application will be notified.
 */
#ifndef HF_COMMAND_TIMEOUT
#define HF_COMMAND_TIMEOUT 10000
#endif /* HF_COMMAND_TIMEOUT */

/*---------------------------------------------------------------------------
 * HF_USE_CALL_MANAGER constant
 *     Enables the Call Manager component.  This component tracks call states
 *     and presents the call states to the application layer.  Disable
 *     this feature if call states are managed by the application.
 */
#ifndef HF_USE_CALL_MANAGER
#define HF_USE_CALL_MANAGER  XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * HF_POLL_TIMEOUT constant
 *     This define is only valid if HF_USE_CALL_MANAGER is defined to 
 *     XA_ENABLED.  Defines the amount of time to wait until a poll is sent 
 *     to the audio gateway for call status when the call states are none.  
 *     Polling can help discover the call state when synchronization between 
 *     the Hands-free unit and the audio gateway is lost.  A short poll 
 *     timeout may prevent the Audio Gateway from entering a low power 
 *     state, therefore a large number is recommended.  The default is 2 
 *     minutes.  A value of 0 disables polling.
 */
#ifndef HF_POLL_TIMEOUT
#define HF_POLL_TIMEOUT 120000
#endif /* HF_COMMAND_TIMEOUT */

/*---------------------------------------------------------------------------
 * HF_POLL_TIMEOUT_ACTIVE constant
 *     This define is only valid if HF_USE_CALL_MANAGER is 
 *     defined to XA_ENABLED.  Defines the amount of time to wait 
 *     until a poll is sent to the audio gateway for call status 
 *     when the call states are other than none.  Polling can 
 *     help discover the call state when synchronization between 
 *     the Hands-free unit and the audio gateway is lost.  
 */
#ifndef HF_POLL_TIMEOUT_ACTIVE
#define HF_POLL_TIMEOUT_ACTIVE 4000
#endif /* HF_COMMAND_TIMEOUT_ACTIVE */

/*---------------------------------------------------------------------------
 * HF_RING_TIMEOUT constant
 *     This define is only valid if HF_USE_CALL_MANAGER is defined to XA_ENABLED.
 *     Defines the amount of time to wait for the next RING command before 
 *     setting the call state to HF_CALL_STATUS_NONE.  When an incoming call
 *     exists, but then is either forwarded or terminated, the hands-free 
 *     unit may not be notified.  This timeout determines how long to wait for
 *     a new RING command to determine if a call is still incoming.
 */
#ifndef HF_RING_TIMEOUT
#define HF_RING_TIMEOUT 10000
#endif /* HF_COMMAND_TIMEOUT */

/*---------------------------------------------------------------------------
 * HF_TX_BUFFER_SIZE constant
 *     Defines the maximum size of AT command data that can be transmitted.
 *     The default is large enough to handle all AT commands supported by
 *     the Hands-free SDK.  If raw AT commands are sent that are larger than the
 *     default value, then this number must be increased.
 */
#ifndef HF_TX_BUFFER_SIZE
#define HF_TX_BUFFER_SIZE       1024
#endif

/*---------------------------------------------------------------------------
 * HF_RECV_BUFFER_SIZE constant
 *     Defines the maximum size of AT response data that can be received.
 *     The default is large enough to handle all AT responses supported by
 *     the handsfree profile.
 */
#ifndef HF_RECV_BUFFER_SIZE
#define HF_RECV_BUFFER_SIZE     1024
#endif

/*---------------------------------------------------------------------------
 * HF_USE_PHONEBOOK_COMMANDS constant
 *     Enables the use of AT commands for managing the phonebook on the
 *     remote device.  
 */
#ifndef HF_USE_PHONEBOOK_COMMANDS
#define HF_USE_PHONEBOOK_COMMANDS XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * HF_USE_MESSAGING_COMMANDS constant
 *     Enables the use of AT commands for managing messages on the
 *     remote device.  Currently the only feature of the messaging API
 *     is the ability to receive notification of the existence of an SMS message.
 */
#ifndef HF_USE_MESSAGING_COMMANDS
#define HF_USE_MESSAGING_COMMANDS XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * HF_USE_RESP_HOLD constant
 *     Enables the use of the Response and Hold feature, which is supported
 *     in the Japanese market.
 */
#ifndef HF_USE_RESP_HOLD
#define HF_USE_RESP_HOLD XA_DISABLED
#endif


/*---------------------------------------------------------------------------
 * HF_SNIFF_TIMER constant
 *     Enable sniff mode after a defined timeout.  Sniff mode is entered
 *     whenever a service link is active.  This is a power saving feature.  
 *     If this value is defined to -1, then sniff mode is disabled.
 */
#ifndef HF_SNIFF_TIMER
#define HF_SNIFF_TIMER 10000
#endif

/*---------------------------------------------------------------------------
 * HF_SNIFF_MIN_INTERVAL constant
 *     Miniumum interval for sniff mode if enabled (see HF_SNIFF_TIMER).
 *
 */
#ifndef HF_SNIFF_MIN_INTERVAL
#define HF_SNIFF_MIN_INTERVAL 0x0040
#endif

/*---------------------------------------------------------------------------
 * HF_SNIFF_MAX_INTERVAL constant
 *     Maximum interval for sniff mode if enabled (see HF_SNIFF_TIMER).
 *
 */
#ifndef HF_SNIFF_MAX_INTERVAL
#define HF_SNIFF_MAX_INTERVAL 0x0800
#endif

/*---------------------------------------------------------------------------
 * HF_SNIFF_ATTEMPT constant
 *     Sniff attempts for sniff mode if enabled (see HF_SNIFF_TIMER).
 *
 */
#ifndef HF_SNIFF_ATTEMPT
#define HF_SNIFF_ATTEMPT 0x0160
#endif

/*---------------------------------------------------------------------------
 * HF_SNIFF_TIMEOUT constant
 *     Sniff timeout for sniff mode if enabled (see HF_SNIFF_TIMER).
 *
 */
#ifndef HF_SNIFF_TIMEOUT
#define HF_SNIFF_TIMEOUT 0x0160
#endif

/*---------------------------------------------------------------------------
 * HF_MEMORY_EXTERNAL constant
 *     Memory is allocated externally.  When this constant is defined as
 *     XA_ENABLED, global context memory is not allocated by the Hands-free 
 *     profile code, and therefore must be allocated external to this module.  
 *     If memory is allocated external to this module, if must be allocated 
 *     using the context variable type defined in hfalloc.h, and the entire 
 *     structure must be initialized to 0. In addition, the macro HF() must 
 *     be defined so that the context memory can be accessed local to this module.
 *
 *     See hfalloc.h and hfalloc.c for an example.
 */
#ifndef HF_MEMORY_EXTERNAL
#define HF_MEMORY_EXTERNAL XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * HF_SECURITY_SETTINGS constant
 *     Defines the security settings for a incoming hands-free serial port
 *     connection.  The default is set to BSL_AUTHENTICATION_IN 
 *     (see BtSecurityLevel).
 */
#ifndef HF_SECURITY_SETTINGS
#define HF_SECURITY_SETTINGS BSL_AUTHENTICATION_IN
#endif                               

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HfEvent type
 *
 *     All indications and confirmations are sent through a callback
 *     function. Depending on the event, different elements of
 *     the HfCallbackParms "HfCallbackParms.p" union will be valid.
 */
typedef U16 HfEvent;

/** An incoming service level connection is being established.  This happens
 *  when the audio gateway establishes the service connection.
 *  The data connection is not available yet for issuing commands to the audio 
 *  gateway.  When the HF_EVENT_SERVICE_CONNECT event is received, the 
 *  channel is available for issuing commands.
 *
 *  When this callback is received, the "HfCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.
 */
#define HF_EVENT_SERVICE_CONNECT_REQ   0

/** A service level connection has been established.  This can happen as the
 *  result of a call to HF_CreateServiceLink, or if the audio gateway 
 *  establishes the service connection.  When this event has been received, a 
 *  data connection is available for issuing commands to the audio gateway.  
 *
 *  When this callback is received, the "HfCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.
 */
#define HF_EVENT_SERVICE_CONNECTED      1

/** The service level connection has been released.  This can happen as the
 *  result of a call to HF_DisconnectServiceLink, or if the audio gateway
 *  releases the service connection.  Communication with the audio gateways is 
 *  no longer possible.  In order to communicate with the audio gateway, a new 
 *  service level connection must be established.
 *
 *  This event can also occur when an attempt to create a service level 
 *  connection (HF_CreateServiceLink) fails.
 *
 *  When this callback is received, the "HfCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.  In addition, the
 *  "HfCallbackParms.errCode" fields contains the reason for disconnect.
 */
#define HF_EVENT_SERVICE_DISCONNECTED   2

/** An audio connection has been established.  This event occurs whenever the
 *  audio channel (SCO) comes up, whether it is initiated by the audio gateway
 *  or the hands-free unit.
 *
 *  When this callback is received, the "HfCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.
 */
#define HF_EVENT_AUDIO_CONNECTED        3

/** An audio connection has been released.  This event occurs whenever the
 *  audio channel (SCO) goes down, whether it is terminated by the audio gateway
 *  or the hands-free unit.
 *
 *  When this callback is received, the "HfCallbackParms.p.remDev" field 
 *  contains a pointer to the remote device context.  In addition, the
 *  "HfCallbackParms.errCode" fields contains the reason for disconnect.
 */
#define HF_EVENT_AUDIO_DISCONNECTED     4
 
#if BT_SCO_HCI_DATA == XA_ENABLED
/** Only valid if BT_SCO_HCI_DATA is set to XA_ENABLED.  Audio data has been 
 *  received from the remote device.  The data is only valid during the
 *  callback.
 *
 *  When this callback is received, the "HfCallbackParms.p.audioData" field 
 *  contains the audio data.
 */
#define HF_EVENT_AUDIO_DATA             5

/** Only valid if BT_SCO_HCI_DATA is set to XA_ENABLED.  Audio data has been 
 *  sent to the remote device.  This event is received by the application when
 *  the data sent by HF_SendAudioData has been successfully sent.
 *
 *  When this callback is received, the "HfCallbackParms.p.audioPacket" field 
 *  contains the result.
 */
#define HF_EVENT_AUDIO_DATA_SENT        6
#endif

/** After the service level connection has been established, this event will
 *  indicate the features supported on the audio gateway.  
 *
 *  When this callback is received, the "HfCallbackParms.p.features" field 
 *  contains the features (see HfFeatures).
 */
#define HF_EVENT_GATEWAY_FEATURES       7

/** After the service level connection has been established, this event will
 *  indicate the hold features (3-Way calling) supported on the audio gateway.  
 *
 *  When this callback is received, the "HfCallbackParms.p.holdFeatures" field 
 *  contains the features (see HfGwHoldFeatures).
 */
#define HF_EVENT_GW_HOLD_FEATURES       8

#if HF_USE_CALL_MANAGER == XA_ENABLED
/** This event is generated only if HF_USE_CALL_MANAGER is defined as
 *  XA_ENABLED.  After the service level connection has been established, this 
 *  event will indicate the call state of the current connection.  Whenever the 
 *  call state changes, this event is generated.
 *
 *  When this callback is received, the "HfCallbackParms.p.callStateParms" field 
 *  contains the current call state (see HfCallStateParms).
 */                                     
#define HF_EVENT_CALL_STATE             9

/** This event is generated only if HF_USE_CALL_MANAGER is defined as
 *  XA_ENABLED.  The identification of the call has been received from the audio
 *  gateway.  
 * 
 *  When this callback is received, the "HfCallbackParms.p.callerId" 
 *  field contains a pointer to the ASCII string representation of the number 
 *  (NULL terminated).
 */ 
#define HF_EVENT_CALLER_ID             10

/** This event is generated only if HF_USE_CALL_MANAGER is defined as
 *  XA_ENABLED.  The Call Manager has determined the CLCC command is supported 
 *  by the device.  If this event is received, all call state information can be 
 *  considered reliable. 
 */
#define HF_EVENT_CALL_LISTING_ENABLED  11

#else

/** This event is generated only if HF_USE_CALL_MANAGER is defined as
 *  XA_DISABLED.  After the service level connection has been established, this 
 *  event will indicate whether at least one call is active on the gateway
 *  device.  Whenever all calls are terminated, or when there were no calls and 
 *  a new call is created, this event is generated.
 *
 *  When this callback is received, the "HfCallbackParms.p.call" field 
 *  contains the current call state (see HfCallActiveState).
 */                                     
#define HF_EVENT_CALL_IND              12

/** This event is generated only if HF_USE_CALL_MANAGER is defined as
 *  XA_DISABLED.  After the service level connection has been established, this 
 *  event will indicate the current call setup state.  Whenever the call setup
 *  state changes, this event is generated.
 *
 *  When this callback is received, the "HfCallbackParms.p.callSetup" field 
 *  contains the current call setup state (see HfCallSetupState).
 */                                     
#define HF_EVENT_CALLSETUP_IND         13

/** This event is generated only if HF_USE_CALL_MANAGER is defined as
 *  XA_DISABLED.  After the service level connection has been established, this 
 *  event will indicate the current call held state.  Whenever the held
 *  state changes, this event is generated.
 *
 *  When this callback is received, the "HfCallbackParms.p.callHeld" field 
 *  contains the current call held state (see HfCallHeldState).
 */                                     
#define HF_EVENT_CALLHELD_IND          14

/** This event is generated only if HF_USE_CALL_MANAGER is defined as
 *  XA_DISABLED.  When an incoming call is received on the audio gateway, this
 *  event is generated to indicate the incoming ring.
 */
#define HF_EVENT_RING_IND              15

/** This event is generated only if HF_USE_CALL_MANAGER is defined as
 *  XA_DISABLED.  When call waiting is supported on the audio gateway and an
 *  incoming call is received while another call is active, this event is 
 *  received.
 *
 *  When this callback is received, the "HfCallbackParms.p.callWaitParms" field 
 *  contains informationa about the waiting call (see HfCallWaitParms).
 */                                     
#define HF_EVENT_WAIT_NOTIFY           16

/** This event is generated only if HF_USE_CALL_MANAGER is defined as
 *  XA_DISABLED.  If caller ID notification is active, this event is received
 *  when an incoming call is detected on the audio gateway.
 *
 *  When this callback is received, the "HfCallbackParms.p.callerIdParms" field 
 *  contains the current caller ID informaiton (see HfCallerIdParms).
 */                                     
#define HF_EVENT_CALLER_ID_NOTIFY      17

/** This event is generated only if HF_USE_CALL_MANAGER is defined as
 *  XA_DISABLED.  This event is received once for each call which exists on 
 *  the audio gateway.  This event is received after calling 
 *  HF_ListCurrentCalls().
 *
 *  When this callback is received, the "HfCallbackParms.p.callListParms" field 
 *  contains the current caller ID informaiton (see HfCallListParms).
 */                                     
#define HF_EVENT_CURRENT_CALL_STATE    18

#if HF_USE_RESP_HOLD == XA_ENABLED
/** This event is only available when HF_USE_RESP_HOLD is set to XA_ENABLED
 *  and HF_USE_CALL_MANAGER is set to XA_DISABLED.  The Response and Hold state
 *  has been received from the audio gateway.  This event is generated in 
 *  response to a call to HF_QueryResponseHold() or HF_ResponseHold().  
 * 
 *  When this callback is received, the "HfCallbackParms.p.respHold" field 
 *  contains the result.
 */
#define HF_EVENT_RESPONSE_HOLD         19
#endif
#endif

/** The service indicator has been received from the audio gateway.
 * 
 *  When this callback is received, the "HfCallbackParms.p.service" 
 *  field contains a pointer to the service state.
 */ 
#define HF_EVENT_SERVICE_IND           20

/** The battery indicator has been received from the audio gateway.
 * 
 *  When this callback is received, the "HfCallbackParms.p.battery" 
 *  field contains a pointer to the battery level.
 */ 
#define HF_EVENT_BATTERY_IND           21

/** The signal strength indicator has been received from the audio gateway.
 * 
 *  When this callback is received, the "HfCallbackParms.p.signal" 
 *  field contains a pointer to the signal strength.
 */ 
#define HF_EVENT_SIGNAL_IND            22

/** The roam indicator has been received from the audio gateway.
 * 
 *  When this callback is received, the "HfCallbackParms.p.roam" 
 *  field contains a pointer to the roam state.
 */ 
#define HF_EVENT_ROAM_IND              23

#if HF_USE_MESSAGING_COMMANDS
/** This indicator is only available if HF_USE_MESSAGING_COMMANDS is set to
 *  XA_ENABLED.  The 'SMS indicator has been received from the audio gateway.
 * 
 *  When this callback is received, the "HfCallbackParms.p.sms" 
 *  field contains a pointer to the message state.
 */ 
#define HF_EVENT_SMS_IND               24
#endif

/** The voice recognition state has changed.  This event occurs if the
 *  audio gateway changes the state of voice recognition.
 *
 *  When this callback is received, the "HfCallbackParms.p.voiceRecognition" 
 *  field contains state of voice recognition.
 */
#define HF_EVENT_VOICE_REC_STATE       25

/** A number was returned in response to the HF_GetLastVoiceTag function.
 *
 *  When this callback is received, the "HfCallbackParms.p.data" field contains 
 *  a pointer to the ASCII string representation of the number (NULL terminated).
 */
#define HF_EVENT_VOICE_TAG_NUMBER      26

/** The speaker volume has been received from the audio gateway.
 */
#define HF_EVENT_SPEAKER_VOLUME        27

/** The mic volume has been received from the audio gateway.
 */
#define HF_EVENT_MIC_VOLUME            28

/** The in-band ring tone setting has been received from the audio gateway.
 * 
 *  When this callback is received, the "HfCallbackParms.p.inBandRing" 
 *  field contains a pointer to the In-Band ring state.
 */ 
#define HF_EVENT_IN_BAND_RING          29

/** The network operator string has been received from the remote device.
 * 
 *  When this callback is received, the "HfCallbackParms.p.networkOper" 
 *  field contains a pointer to the operator string state.
 */ 
#define HF_EVENT_NETWORK_OPERATOR      30

/** The susbscriber number has been received from the audio gateway.
 * 
 *  When this callback is received, the "HfCallbackParms.p.subscriberNum" 
 *  field contains a pointer to the subscriber number.
 */ 
#define HF_EVENT_SUBSCRIBER_NUMBER     31

/** The NO CARRIER event has been received from the audio gateway.
 */
#define HF_EVENT_NO_CARRIER            32

/** The BUSY event has been received from the audio gateway.
 */
#define HF_EVENT_BUSY                  33

/** The NO ANSWER event has been received from the audio gateway.
 */
#define HF_EVENT_NO_ANSWER             34

/** The DELAYED event has been received from the audio gateway.
 */
#define HF_EVENT_DELAYED               35

/** The BLACKLISTED event has been received from the audio gateway.
 */
#define HF_EVENT_BLACKLISTED           36

#if HF_USE_PHONEBOOK_COMMANDS == XA_ENABLED
/** The supported phonebook storage types have been received from the audio 
 *  gateway.  This event occurs in response to a call to HF_QueryPhonebooks.
 * 
 *  When this callback is received, the "HfCallbackParms.p.phonebooks" 
 *  field contains a bitmask of phonebook type supported on the gateway 
 *  (see HfPhonebooks).
 */ 
#define HF_EVENT_PHONEBOOK_STORAGE     37

/** The phonebook storage information has been received from the audio 
 *  gateway.  This event occurs in response to a call to 
 *  HF_GetCurrentPhonebooksInfo.  
 * 
 *  When this callback is received, the "HfCallbackParms.p.phonebookInfo" 
 *  field contains a pointer to a structure containing the phonebook information.
 */ 
#define HF_EVENT_PHONEBOOK_INFO        38

/** The number of phonebook entries has been received from the audio 
 *  gateway.  This event occurs in response to a call to 
 *  HF_GetPhonebookSize.  
 * 
 *  When this callback is received, the "HfCallbackParms.p.phonebookSize" field
 *  contains a pointer to a structure containing the phonebook information.
 */ 
#define HF_EVENT_PHONEBOOK_SIZE        39

/** A phonebook entry has been received from the audio gateway.  This event 
 *  occurs in response to a call to HF_ReadPhonebookEntries or 
 *  HF_FindPhonebookEntries. 
 * 
 *  When this callback is received, the "HfCallbackParms.p.phonebookEntry" field
 *  contains a pointer to a structure containing the phonebook information.
 */ 
#define HF_EVENT_PHONEBOOK_ENTRY       40
#endif

/** An result code has been received from the audio gateway.  This event is 
 *  received for unsolicited result codes not handled by the internal 
 *  Hands-free AT parser.
 *
 *  When this callback is received, the "HfCallbackParms.p.data" field 
 *  contains the AT result code.
 */
#define HF_EVENT_AT_RESULT_DATA        41

/** A command to the audio gateway has completed.  This event is received
 *  when the processing a command is complete.
 *
 *  When this callback is received, the "HfCallbackParms.p.command" field 
 *  contains the command that was sent.  If "HfCallbackParms.status is set
 *  to BT_STATUS_FAILED, then "HfCallbackParms.p.command->cmeError" contains
 *  the command error (if known).
 */
#define HF_EVENT_COMMAND_COMPLETE      42

/* End of HfEvent */

/*---------------------------------------------------------------------------
 * HfSniffExitPolicy type
 *
 * Defines the policy for exiting sniff mode.  While it is possible to both
 * send data a create SCO connections in sniff mode, it may be desired to
 * exit sniff mode for performance or compatibility reasons.
 */
typedef U8 HfSniffExitPolicy;

/** Exit sniff mode whenever there is data to send.
 */
#define HF_SNIFF_EXIT_ON_SEND  CMGR_SNIFF_EXIT_ON_SEND

/** Exit sniff mode whenever an audio link is being created.
 */
#define HF_SNIFF_EXIT_ON_AUDIO CMGR_SNIFF_EXIT_ON_AUDIO

/* End of HfSniffExitPolicy */

/*--------------------------------------------------------------------------
 * HfCommandType type
 *      
 *     Type of logical command.  This value is returned in the 
 *     "HfCallbackParms.p.command" structure with the HF_EVENT_COMMAND_COMPLETE
 *     event.  It indicates the API call associated with this command structure.
 *     Any API call that takes a HfCommand parameter receives the
 *     HF_EVENT_COMMAND_COMPLETE event when the command has completed.  The
 *     "HfCallbackParms.p.command->type" field identifies the API call
 *     that was made.
 */
typedef U8 HfCommandType;

/* HF_AnswerCall */
#define HF_COMMAND_ANSWER_CALL             0

/* HF_DialNumber */
#define HF_COMMAND_DIAL_NUMBER             1

/* HF_MemoryDial */
#define HF_COMMAND_DIAL_MEMORY             2

/* HF_Redial */
#define HF_COMMAND_REDIAL                  3

/* HF_CallHold */
#define HF_COMMAND_CALL_HOLD               4

#if HF_USE_RESP_HOLD == XA_ENABLED
/* HF_QueryResponseHold */
#define HF_COMMAND_QUERY_RESPONSE_HOLD     5

/* HF_ResponseHold */
#define HF_COMMAND_RESPONSE_HOLD           6
#endif

/* HF_Hangup */
#define HF_COMMAND_HANGUP_CALL             7

/* HF_ListCurrentCalls */
#define HF_COMMAND_LIST_CURRENT_CALLS      8

/* HF_EnableCallerIdNotify */
#define HF_COMMAND_ENABLE_CID_NOTIFY       9

/* HF_EnableCallWaitNotify */
#define HF_COMMAND_ENABLE_WAIT_NOTIFY      10

/* HF_GenerateDtmf() */
#define HF_COMMAND_GENERATE_DTMF           11

/* HF_GetLastVoiceTag */
#define HF_COMMAND_GET_LAST_VOICE_TAG      12

/* HF_VoiceRecognition */
#define HF_COMMAND_VOICE_RECOGNITION       13

/* HF_DisableNREC */
#define HF_COMMAND_DISABLE_NREC            14

/* HF_ReportMicVolume */
#define HF_COMMAND_REPORT_MIC_VOLUME       15

/* HF_ReportSpeakerVolume */
#define HF_COMMAND_REPORT_SPEAKER_VOLUME   16

/* HF_QueryNetworkOperator */
#define HF_COMMAND_QUERY_NETWORK_OPER      17

/* HF_QuerySubscriberNumber */
#define HF_COMMAND_QUERY_SUBSCRIBER_NUM    18

/* HF_EnableExtendedErrors */
#define HF_COMMAND_ENABLE_EXTENDED_ERR     19

/* HF_SendAtCommand */
#define HF_COMMAND_SEND_AT_COMMAND         20

/* HF_QueryPhonebooks */
#define HF_COMMAND_QUERY_PB                21

/* HF_SelectPhonebook */
#define HF_COMMAND_SELECT_PB               22

/* HF_GetCurrentPhonebookInfo */
#define HF_COMMAND_GET_CURRENT_PB_INFO     23

/* HF_GetPhonebookSize */
#define HF_COMMAND_GET_PB_SIZE             24

/* HF_ReadPhonebookEntries */
#define HF_COMMAND_READ_PB_ENTRIES         25

/* HF_FindPhonebookEntries */
#define HF_COMMAND_FIND_PB_ENTRIES         26

/* HF_WritePhonebookEntry */
#define HF_COMMAND_WRITE_PB_ENTRY          27

/* End of HfCommandType */

/*--------------------------------------------------------------------------
 * HfCmeError type
 *
 */
typedef AtError HfCmeError;

#define AT_CME_NO_CONNECTION                 ATCME_NO_CONNECTION
#define AT_CME_OP_NOT_ALLOWED                ATCME_OP_NOT_ALLOWED
#define AT_CME_OP_NOT_SUPPORTED              ATCME_OP_NOT_SUPPORTED
#define AT_CME_PH_SIM_PIN_REQUIRED           ATCME_PH_SIM_PIN_REQUIRED
#define AT_CME_SIM_NOT_INSERTED              ATCME_SIM_NOT_INSERTED
#define AT_CME_SIM_PIN_REQUIRED              ATCME_SIM_PIN_REQUIRED
#define AT_CME_SIM_PUK_REQUIRED              ATCME_SIM_PUK_REQUIRED
#define AT_CME_SIM_FAILURE                   ATCME_SIM_FAILURE
#define AT_CME_SIM_BUSY                      ATCME_SIM_BUSY
#define AT_CME_INCORRECT_PASSWORD            ATCME_INCORRECT_PASSWORD
#define AT_CME_SIM_PIN2_REQUIRED             ATCME_SIM_PIN2_REQUIRED
#define AT_CME_SIM_PUK2_REQUIRED             ATCME_SIM_PUK2_REQUIRED
#define AT_CME_MEMORY_FULL                   ATCME_MEMORY_FULL
#define AT_CME_INVALID_INDEX                 ATCME_INVALID_INDEX
#define AT_CME_MEMORY_FAILURE                ATCME_MEMORY_FAILURE
#define AT_CME_TEXT_STRING_TOO_LONG          ATCME_TEXT_STRING_TOO_LONG
#define AT_CME_INVALID_CHARS_IN_TEXT_STRING  ATCME_INVALID_CHARS_IN_TEXT_STRING
#define AT_CME_DIAL_STRING_TOO_LONG          ATCME_DIAL_STRING_TOO_LONG
#define AT_CME_INVALID_CHARS_IN_DIAL_STRING  ATCME_INVALID_CHARS_IN_DIAL_STRING
#define AT_CME_NO_NETWORK_SERVICE            ATCME_NO_NETWORK_SERVICE
#define AT_CME_NETWORK_NOT_ALLOWED           ATCME_NETWORK_NOT_ALLOWED
#define AT_CME_UNKNOWN                       ATCME_UNKNOWN

/* End of HfCmeError */

/*--------------------------------------------------------------------------
 * HfGatewayVersion type
 *      
 *     Gateway profile version discovered during the SDP query.  The service
 *     connection will be limited to the capabilities of this profile version.
 */
typedef U16  HfGatewayVersion;

/* Unable to determine the Hands Free Profile version that is supported */
#define HF_GW_VERSION_UNKNOWN 0x0000

/* Supports Version 0.96 of the Hands Free Profile */
#define HF_GW_VERSION_0_96    0x0100

/* Supports Version 1.0 of the Hands Free Profile */
#define HF_GW_VERSION_1_0     0x0101

/* Supports Version 1.5 of the Hands Free Profile */
#define HF_GW_VERSION_1_5     0x0105

/* End of HfGatewayVersion */

/*---------------------------------------------------------------------------
 * HfGatewayFeatures type
 *      
 *  Bit mask specifying the gateway feature set.  The service connection will 
 *  be limited to the features advertised by the profile.
 */
typedef U32 HfGatewayFeatures;

/** 3-way calling
 */
#define HF_GW_FEATURE_3_WAY              0x00000001

/** Echo canceling and/or noise reduction function
 */
#define HF_GW_FEATURE_ECHO_NOISE         0x00000002

/** Voice recognition function
 */
#define HF_GW_FEATURE_VOICE_RECOGNITION  0x00000004

/** In-band ring tone
 */
#define HF_GW_FEATURE_IN_BAND_RING       0x00000008

/** Voice tag
 */
#define HF_GW_FEATURE_VOICE_TAG          0x00000010

/** Reject a call
 */
#define HF_GW_FEATURE_CALL_REJECT        0x00000020

/** Enhanced Call Status
 */
#define HF_GW_FEATURE_ENH_CALL_STATUS    0x00000040

/** Enhanced Call Control
 */
#define HF_GW_FEATURE_ENH_CALL_CTRL      0x00000080

/* End of HfGatewayFeatures */

/*---------------------------------------------------------------------------
 * HfGwHoldFeatures type
 *      
 *  Bit mask specifying the gateway's 3-Way calling (hold) feature set.  
 *  The service connection will be limited to the features advertised by the 
 *  profile.
 */
typedef U8 HfGwHoldFeatures;

/** Releases all held calls or sets User Determined User Busy
 * (UDUB) for a waiting call.
 */
#define HF_GW_HOLD_RELEASE_HELD_CALLS      0x01

/** Releases all active calls (if any exist) and accepts the other
 * (held or waiting) call. 
 */
#define HF_GW_HOLD_RELEASE_ACTIVE_CALLS    0x02

/** Releases a specific call. */
#define HF_GW_HOLD_RELEASE_SPECIFIC_CALL   0x04

/** Places all active calls (if any exist) on hold and accepts the
 * other (held or waiting) call.
 */
#define HF_GW_HOLD_HOLD_ACTIVE_CALLS       0x08

/** Places a specific call on hold. */
#define HF_GW_HOLD_HOLD_SPECIFIC_CALL      0x10

/** Adds a held call to the conversation.
 */
#define HF_GW_HOLD_ADD_HELD_CALL           0x20

/** Connects the two calls and disconnects the AG from
 * both calls (Explicit Call Transfer).
 */
#define HF_GW_HOLD_CALL_TRANSFER           0x40

/* End of HfGwHoldFeatures */

/*---------------------------------------------------------------------------
 * HfSignalStrength type
 *      
 *  Contains the last signal strength reading from the audio gateway.
 */
typedef U8 HfSignalStrength;

/* End of HfSignalStrength */

/* Forward references */
typedef struct _HfCallbackParms HfCallbackParms;
typedef struct _HfChannel HfChannel;
typedef struct _HfCommand HfCommand;

/*---------------------------------------------------------------------------
 * HfCallback type
 *
 * A function of this type is called to indicate events to the application.
 */
typedef void (*HfCallback)(HfChannel *Channel, HfCallbackParms *Info);

/* End of HfCallback */

/*---------------------------------------------------------------------------
 * HfCmdOverride type
 *
 * A function of this type is called when a function override is registered.
 */
typedef BtStatus (*HfCmdOverride)(HfChannel *Channel, HfCommand *Command);

/* End of HfCmdOverride */

/*---------------------------------------------------------------------------
 * HfCallStatus type
 *
 * Defines the current state of a call.  Not all states are supported by all 
 * audio gateways.  At the very minimum, HF_CALL_STATUS_NONE, 
 * HF_CALL_STATUS_OUTGOING, HF_CALL_STATUS_INCOMING, and HF_CALL_STATUS_ACTIVE 
 * will be supported.
 */
typedef U8 HfCallStatus;

/** An active call exists.
 */
#define HF_CALL_STATUS_ACTIVE     0

/** The call is held.
 */
#define HF_CALL_STATUS_HELD       1

/** A call is outgoing.  This state occurs when attempting a call using any
 *  of the dialing functions.
 */
#define HF_CALL_STATUS_DIALING    2

/** The remote party is being alerted.
 */
#define HF_CALL_STATUS_ALERTING   3

/** A call is incoming.  It can be answered with HFG_AnswerCall or 
 *  HFG_Hangup.  This state occurs when a call is being established by a
 *  remote party.
 */
#define HF_CALL_STATUS_INCOMING   4

/** The call is waiting.  This state occurs only when the audio gateway supports
 *  3-Way calling.
 */
#define HF_CALL_STATUS_WAITING    5

/** No active call
 */
#define HF_CALL_STATUS_NONE       0xFF

/** Unknow call state
 */
#define HF_CALL_STATUS_UNKNOWN    0xFE
/* End of HfCallStatus */

/*---------------------------------------------------------------------------
 * HfCallMode type
 *
 * Defines the current mode of a call.
 */
typedef U8 HfCallMode;

/* Voice Call */
#define HF_CALL_MODE_VOICE     0

/* Data Call */
#define HF_CALL_MODE_DATA      1

/* FAX Call */
#define HF_CALL_MODE_FAX       2

/* End of HfCallMode */

/*---------------------------------------------------------------------------
 * HfCallActiveState type
 *
 * This type is used only if HF_USE_CALL_MANAGER is defined as XA_DISABLED.  
 * Defines the current call state indicated by the audio gateway.  
 */
typedef U8 HfCallActiveState;

/* No call exists on the audio gateway */
#define HF_CALL_NONE           0

/* A call is active ont the audio gateway */
#define HF_CALL_ACTIVE         1

/* End of HfCallActiveState */

/*---------------------------------------------------------------------------
 * HfCallSetupState type
 *
 * This type is used only if HF_USE_CALL_MANAGER is defined as XA_DISABLED.  
 * Defines the current call setup state indicated by the audio gateway.  
 */
typedef U8 HfCallSetupState;

/* No outgoing or incoming call is present on the audio gateway */
#define HF_CALL_SETUP_NONE     0

/* An incoming call is present on the audio gateway */
#define HF_CALL_SETUP_IN       1

/* An outgoing call is present on the audio gateway */
#define HF_CALL_SETUP_OUT      2

/* An outgoing call is being alerted on the audio gateway */
#define HF_CALL_SETUP_ALERT    3

/* End of HfCallSetupState */

/*---------------------------------------------------------------------------
 * HfCallHeldState type
 *
 * This type is used only if HF_USE_CALL_MANAGER is defined as XA_DISABLED.  
 * Defines the current call held state indicated by the audio gateway.  
 */
typedef U8 HfCallHeldState;

/* No call is in the held state on the audio gateway */
#define HF_CALL_HELD_NONE      0

/* A call is in the held state on the audio gateway */
#define HF_CALL_HELD           1

/* End of HfCallHeldState */

/*---------------------------------------------------------------------------
 * HfHoldAction type
 *
 * Defines the action taken when calling the HF_CallHold function.
 */
typedef U8 HfHoldAction;

/** Releases all held calls or sets User Determined User Busy
 * (UDUB) for a waiting call.
 */
#define HF_HOLD_RELEASE_HELD_CALLS   0

/** Releases all active calls (if any exist) and accepts the other
 * (held or waiting) call. 
 *
 * If a call index is specified, will release the specific call.
 */
#define HF_HOLD_RELEASE_ACTIVE_CALLS 1

/** Places all active calls (if any exist) on hold and accepts the
 * other (held or waiting) call.
 *
 * If a call index is specified, will put all calls on hold except the 
 * specified call.
 */
#define HF_HOLD_HOLD_ACTIVE_CALLS    2

/** Adds a held call to the conversation.
 */
#define HF_HOLD_ADD_HELD_CALL        3

/** Connects the two calls and disconnects the AG from
 * both calls (Explicit Call Transfer).
 */
#define HF_HOLD_CALL_TRANSFER        4

/* End of HfHoldAction */

#if HF_USE_RESP_HOLD == XA_ENABLED
/*---------------------------------------------------------------------------
 * HfResponseHoldAction type
 *
 * Defines the action of Response and Hold.  These are only valid in the
 * when HF_USE_RESP_HOLD is set to XA_ENABLED.
 */
typedef U8 HfResponseHoldAction;

/* Incoming call is put on hold */
#define HF_RESP_HOLD_STATE_HOLD      0

/* Held Incoming call is accepted */
#define HF_RESP_HOLD_STATE_ACCEPT    1

/* Held Incoming call is rejected */
#define HF_RESP_HOLD_STATE_REJECT    2

/* End of HfResponseHoldAction */
#endif

#if HF_USE_PHONEBOOK_COMMANDS == XA_ENABLED
/*---------------------------------------------------------------------------
 * HfPhonebooks type
 *
 * This type is only available when HF_USE_PHONEBOOK_COMMANDS is set to XA_ENABLED.
 * Defines the type of phone books supported in a gateway.
 */
typedef AtPbStorageType HfPhonebooks;

/** GW dialed calls list */
#define HF_PB_DIALED_CALLS          AT_PBS_DIALED_CALLS

/** SIM fixed-dialing-phonebook list */
#define HF_PB_FIXED_DIAL            AT_PBS_FIXED_DIAL

/** SIM last-dialing-phonebook list */
#define HF_PB_LAST_DIAL             AT_PBS_LAST_DIAL

/** GW missed calls list */
#define HF_PB_MISSED_CALLS          AT_PBS_MISSED_CALLS

/** GW phonebook list */
#define HF_PB_PHONE                 AT_PBS_ME_PHONEBOOK

/** Combined GW and SIM phonebook list */
#define HF_PB_COMBINED              AT_PBS_ME_SIM_COMBINED

/** GW received calls list */
#define HF_PB_RECEIVED_CALLS        AT_PBS_RECEIVED_CALLS

/** SIM phonebook list */
#define HF_PB_SIM                   AT_PBS_SIM_PHONEBOOK

/* End of HfPhonebooks */

#endif

/*---------------------------------------------------------------------------
 * Poll Interval type
 *
 *     Indicates the interval between CLCC polls.
 */
typedef U8 PollInterval;

/** Used for CIEV indications.
 */
#define PL_UNKNOWN 0x00

/** Used for CIND polling indications.
 */
#define PL_SLOW    0x0F

/** Used for service connection events.
 */
#define PL_FAST    0xF0

/* End of PollInterval */

#if BT_SCO_HCI_DATA == XA_ENABLED
/*---------------------------------------------------------------------------
 * HfAudioData type
 *
 * This type is only available when BT_SCO_HCI_DATA is set to XA_ENABLED.
 * Contains audio data received from the remote device.
 */
typedef CmgrAudioData HfAudioData;

/* End of HfAudioData */
#endif

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HfCommand structure
 *
 * Contains the Command type and parameters for sending Hands-free SDK commands.
 */
struct _HfCommand {
    /* Used Internally by the Hands-free SDK */
    ListEntry   node;

    /* The type of command */
    U8          type;

    /* The command parameters */
    U32         parms[6];

    /* The command status */
    BtStatus    status;

    /* CME Error when command fails */
    HfCmeError  cmeError;

    /* Application context */
    void       *context;

    /* === Internal use only === */
    U8          state;

#if HF_USE_CALL_MANAGER == XA_ENABLED
    U16          callMgrFlags;
#endif

};

/*---------------------------------------------------------------------------
 * HfAtData structure
 *
 *     Defines the structure containing raw AT data.
 */
typedef struct _HfAtData {
    U8         *data;
    U16         dataLen;
} HfAtData;

#if HF_USE_CALL_MANAGER == XA_ENABLED
/*---------------------------------------------------------------------------
 * HfCallStateParms structure
 *
 *     Used to indicate the current call state to the hands-free application.
 */
typedef struct _HfCallStateParms {

    /* Identifies the line (1 or 2) whose state has changed */
    U8    line;

    /* The call index (used by Hold commands) */
    U8    index;

    /* Identifies a multiparty call */
    BOOL  multiParty;

    /* Identifies the current call state */
    HfCallStatus state;
} HfCallStateParms;

/*---------------------------------------------------------------------------
 * HfCallIdParms structure
 *
 *     This type is defined only if HF_USE_CALL_MANAGER is defined as XA_ENABLED.  
 *     Used to identify the calling (or called) number.
 */
typedef struct _HfCallIdParms {
    /* The line associated with the call */
    U8    line;

    /* The call index *used by Hold commands) */
    U8    index;

    /* Identifies a multiparty call */
    BOOL  multiParty;

    /* The calling number on incoming calls, the called number on outgoing */
    const char *number;

    /* Type of address */
    U8    type;

} HfCallIdParms;

#else
    
/*---------------------------------------------------------------------------
 * HfCallWaitParms structure
 *
 *     This type is defined only if HF_USE_CALL_MANAGER is defined as XA_DISABLED.  
 *     Used to identify the waiting call.
 */
typedef struct _HfCallWaitParms {
    /* Phone number of the waiting call */
    const char     *number;

    /* Voice parameters */
    U8              classmap;

    /* Type of address */
    U8              type;
} HfCallWaitParms;

/*---------------------------------------------------------------------------
 * HfCallerIdParms structure
 *
 *     This type is defined only if HF_USE_CALL_MANAGER is defined as XA_DISABLED.  
 *     Used to identify the calling number.
 */
typedef struct _HfCallerIdParms {
    /* Phone number of the caller */
    const char     *number;

    /* Type of address */
    U8              type;
} HfCallerIdParms;

/*---------------------------------------------------------------------------
 * HfCallListParms structure
 *
 *     This type is defined only if HF_USE_CALL_MANAGER is defined as XA_DISABLED.  
 *     Used to identify the listed calls on the audio gateway.
 */
typedef struct _HfCallListParms{
    /* Index of the call on the audio gateway (1 based) */
    U8              index;

    /* 0 - Mobile Originated, 1 = Mobile Terminated */
    U8              dir;

    /* Call state (see HfCallState) */
    HfCallStatus   state;

    /* Call mode (see HfCallMode) */
    HfCallMode      mode;

    /* 0 - Not Multiparty, 1 - Multiparty */
    U8              multiParty;
    
    /* Phone number of the call */
    const char     *number;

    /* Type of address */
    U8              type;
} HfCallListParms;

#endif

/*---------------------------------------------------------------------------
 * HfSubscriberNum structure
 *
 *     Used to identify the subscriber number.
 */
typedef struct _HfSubscriberNum {
    /* String phone number of format specified by "type". */
    const char     *number;

    /* Phone number format */
    AtNumberFormat    type;

    /* Service related to the phone number. */
    U8              service;
} HfSubscriberNum;

/*---------------------------------------------------------------------------
 * HfChannel structure
 *
 *     The Hands-free channel.  This structure is used to identify a Hands-free 
 *     SDK channel
 */
struct _HfChannel {

    /* === Internal use only === */

    /* Registration variables */
    ListEntry           node;  
    HfCallback          callback;         /* Application callback function    */
    RfChannel          *hfChannel;        /* RFCOMM channel used for hands-free
                                           * client or server connection.
                                           */
    /* Transmit Queue */
    ListEntry           cmdQueue;         /* List of logical commands         */
    ListEntry           txQueue;          /* List of AT commands              */
    HfCommand          *nextCommand;      /* The next command to execute      */

    /* Connection State Variables */
    U8                  state;            /* Current connection state         */
    U16                 flags;            /* Current connection flags         */
    U8                  linkFlags;        /* Levels of service connected      */

    /* Gateway State Information */
    HfGatewayVersion    version;          /* Profile version parsed from SDP  */
    HfGatewayFeatures   gwFeatures;       /* Profile features parsed from SDP */
    HfGwHoldFeatures    gwHoldFeatures;   /* 3-Way calling hold features      */
    BOOL                nrec;             /* State of noise red/echo canc     */
    BOOL                ibRing;           /* State of in-band ringing         */
    BOOL                voiceRec;         /* State of voice recognition       */
    BOOL                callId;           /* State of caller id nofitication  */
    BOOL                callWaiting;      /* State of call waiting            */
    U8                  micGain;          /* Current microphone gain          */
    U8                  speakerGain;      /* Current speaker gain             */

    /* SDP variables for client */
    SdpQueryToken       sdpQueryToken;    /* Used to query the service        */
    U8                  queryFlags;       /* Defines which SDP entries were
                                           * parsed from the gateway.
                                           */
    U8                  rfServerChannel;  /* When connecting hf client        */

    /* Channel Resources */
    CmgrHandler         cmgrHandler;
    AtCommands          atCommand;
    AtCommands         *currentAtCommand;
    AtCommands         *lastAtCommand;
    XaBufferDesc        atBuffer;
    BtPacket            atTxPacket;
    U8                  atTxData[HF_TX_BUFFER_SIZE];
    U16                 bytesToSend;
    EvmTimer            atTimer;
    EvmTimer            ringTimer;
    U8                  atRxBuffer[HF_RECV_BUFFER_SIZE];
    U16                 atRxLen;

#if HF_USE_CALL_MANAGER == XA_ENABLED
    /* Polling resources */
    EvmTimer            pollTimer;
    AtCommands          pollCommand;
    BtPacket            pollPacket;
    U8                  pollData[RF_MAX_FRAME_SIZE];
    U8                  pollFlags;
    PollInterval        hfPollInterval;

    /* Call State */
    HfCallStatus        callState[2];     /* Current Call States              */
    BOOL                multiParty;
    U8                  currentContext;   /* Context for the current op       */
#endif

};

#if HF_USE_PHONEBOOK_COMMANDS == XA_ENABLED
/*---------------------------------------------------------------------------
 * HfPhonebookInfo structure
 *
 * This type is only available when HF_USE_PHONEBOOK_COMMANDS is set to 
 * XA_ENABLED. A pointer to this structure is sent to the application's callback
 * function notifying the application when phonebook storage information is 
 * received (see HF_GetPhonebookInfo).
 */
typedef struct _HfPhonebookInfo {
    HfPhonebooks   selected;  /* Selected phonebook type */
    U16            used;      /* Number of entries used  */
    U16            total;     /* Total number of entries */
} HfPhonebookInfo;

/*---------------------------------------------------------------------------
 * HfPhonebookSize structure
 *
 * This type is only available when HF_USE_PHONEBOOK_COMMANDS is set to 
 * XA_ENABLED.  A pointer to this structure is sent to the application's 
 * callback function notifying the application when phonebook size information
 * is received (see HF_GetPhonebookSize).
 */
typedef struct _HfPhonebookSize {
    U16         index1;     /* First Entry           */
    U16         index2;     /* Last Entry            */
    U16         numberLen;  /* Maximum number length */
    U16         textLen;    /* Maximum text length   */
} HfPhonebookSize;

/*---------------------------------------------------------------------------
 * HfPhonebookEntry structure
 *
 * This type is only available when HF_USE_PHONEBOOK_COMMANDS is set to 
 * XA_ENABLED.  A pointer to this structure is sent to the application's 
 * callback function notifying the application when a phonebook entry is 
 * received (see HF_ReadPhonebookEntries and HF_FindPhonebookEntries).
 */
typedef struct _HfPhonebookEntry {
    U16             index;     /* Location in the phone book     */
    AtNumberFormat  type;
    const char     *number;    /* The phone number               */
    const char     *text;      /* The text part                  */
} HfPhonebookEntry;
#endif

/*---------------------------------------------------------------------------
 * HfCallbackParms structure
 *
 * A pointer to this structure is sent to the application's callback function 
 * notifying the application of any state changes or important events.
 */
struct _HfCallbackParms {
    HfEvent    event;    /* Event associated with the callback       */

    BtStatus    status;  /* Status of the callback event             */
    BtErrorCode errCode; /* Error code (reason) on disconnect events */

    /* For certain events, a single member of this union will be valid.
     * See HfEvent documentation for more information.
     */
    union {
        void             *ptr;
        BtRemoteDevice   *remDev;
        HfGatewayFeatures features;
        HfGwHoldFeatures  holdFeatures;
        BOOL              service;
        BOOL              roam;
        BOOL              inBandRing;
        BOOL              voiceRecognition;
        U8                battery;
        U8                signal;
        U8                gain;
        HfCommand        *command;
        U8               *networkOper;
        HfSubscriberNum  *subscriberNum;

#if HF_USE_MESSAGING_COMMANDS
        BOOL              sms;           /* Only valid if HF_USE_MESSAGING_COMMANDS
                                          * is set to XA_ENABLED.
                                          */
#endif

#if HF_USE_RESP_HOLD == XA_ENABLED
        HfResponseHoldAction respHold;   /* Only valid if HF_USE_RESP_HOLD
                                          * is set to XA_ENABLED.
                                          */
#endif

#if HF_USE_CALL_MANAGER == XA_ENABLED
        HfCallStateParms *callStateParms;/* Only valid if HF_USE_CALL_MANAGER
                                          * is set to XA_ENABLED.
                                          */

        HfCallIdParms    *callIdParms;   /* Only valid if HF_USE_CALL_MANAGER
                                          * is set to XA_ENABLED.
                                          */
#else
        HfCallActiveState call;          /* Only valid if HF_USE_CALL_MANAGER
                                          * is set to XA_DISABLED.
                                          */

        HfCallSetupState  callSetup;     /* Only valid if HF_USE_CALL_MANAGER
                                          * is set to XA_DISABLED.
                                          */

        HfCallHeldState   callHeld;      /* Only valid if HF_USE_CALL_MANAGER
                                          * is set to XA_DISABLED.
                                          */

        HfCallWaitParms  *callWaitParms; /* Only valid if HF_USE_CALL_MANAGER
                                          * is set to XA_DISABLED.
                                          */

        HfCallerIdParms  *callerIdParms; /* Only valid if HF_USE_CALL_MANAGER
                                          * is set to XA_DISABLED.
                                          */

        HfCallListParms  *callListParms; /* Only valid if HF_USE_CALL_MANAGER
                                          * is set to XA_DISABLED.
                                          */
#endif

#if HF_USE_PHONEBOOK_COMMANDS == XA_ENABLED
        HfPhonebooks      phonebooks;    /* Only valid if HF_USE_PHONEBOOK_COMMANDS
                                          * is set to XA_ENABLED.
                                          */

        HfPhonebookInfo  *phonebookInfo; /* Only valid if HF_USE_PHONEBOOK_COMMANDS
                                          * is set to XA_ENABLED.
                                          */

        HfPhonebookSize  *phonebookSize; /* Only valid if HF_USE_PHONEBOOK_COMMANDS
                                          * is set to XA_ENABLED.
                                          */

        HfPhonebookEntry *phonebookEntry;/* Only valid if HF_USE_PHONEBOOK_COMMANDS
                                          * is set to XA_ENABLED.
                                          */
#endif

#if BT_SCO_HCI_DATA == XA_ENABLED
        HfAudioData      *audioData;     /* Only valid if BT_SCO_HCI_DATA is set
                                          * to XA_ENABLED
                                          */
        BtPacket         *audioPacket;   /* Only valid if BT_SCO_HCI_DATA is set
                                          * to XA_ENABLED
                                          */
#endif

        HfAtData         *data;
    } p;
};

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HF_Init()
 *
 *     Initialize the Hands-free SDK.  This function should only be called
 *     once, normally at system initialization time.  The calling of this 
 *     function can be specified in overide.h using the XA_LOAD_LIST macro
 *     (i.e. #define XA_LOAD_LIST XA_MODULE(HF)).
 *
 * Returns:
 *     TRUE - Initialization was successful
 *
 *     FALSE - Initialization failed.
 */
BOOL HF_Init(void);

/*---------------------------------------------------------------------------
 * HF_Register()
 *
 *     Registers and initializes a channel for use in creating or receiving 
 *     service level connections.  Registers the Hands-Free profile with SDP. 
 *     The application callback function is also bound to the channel.
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
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_Register(HfChannel *Channel, HfCallback Callback);

/*---------------------------------------------------------------------------
 * HF_Deregister()
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
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_Deregister(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_CreateServiceLink()
 *
 *     Creates a service level connection with the audio gateway.
 *     This includes performing SDP Queries to find the appropriate service
 *     and opening an RFCOMM channel.  The success of the operation is indicated 
 *     by the HF_EVENT_SERVICE_CONNECT event.  If the connection fails, the
 *     application is notified by the HF_EVENT_SERVICE_DISCONNECT event. 
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
 *         function registered by HF_Register).
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
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_CreateServiceLink(HfChannel *Channel, BD_ADDR *Addr);

/*---------------------------------------------------------------------------
 * HF_DisconnectServiceLink()
 *
 *     Releases the service level connection with the audio gateway. This will 
 *     close the RFCOMM channel and will also close the SCO and ACL links if no 
 *     other services are active, and no other link handlers are in use 
 *     (ME_CreateLink).  When the operation is complete the application will be 
 *     notified by the HF_EVENT_SERVICE_DISCONNECT event.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the service level connection is down (via the callback 
 *         function registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_DisconnectServiceLink(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_CreateAudioLink()
 *
 *     Creates an audio (SCO) link to the audio gateway. The success of the 
 *     operation is indicated by the HF_EVENT_AUDIO_CONNECTED event.  If the 
 *     connection fails, the application is notified by the 
 *     HF_EVENT_AUDIO_DISCONNECTED event.  
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the audio link has been established (via the callback 
 *         function registered by HF_Register).
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
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_CreateAudioLink(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_DisconnectAudioLink()
 *
 *     Releases the audio connection with the audio gateway.  When the 
 *     operation is complete the application will be notified by the 
 *     HF_EVENT_SERVICE_DISCONNECT event.
 * 
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the audio connection is down (via the callback 
 *         function registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_DisconnectAudioLink(HfChannel *Channel);

#if BT_SCO_HCI_DATA == XA_ENABLED
/*---------------------------------------------------------------------------
 * HF_SendAudioData()
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
 *         to the host controller, HF_EVENT_AUDIO_DATA_SENT is sent to the
 *         application
 *
 * Returns:
 *     BT_STATUS_PENDING - The packet was queued successfully.
 *
 *     BT_STATUS_NO_CONNECTION - No audio connection exists.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus HF_SendAudioData(HfChannel *Channel, BtPacket *Packet);
#define HF_SendAudioData(c, p) SCO_SendData(c->cmgrHandler.scoConnect, p)
#endif

/*---------------------------------------------------------------------------
 * HF_AnswerCall()
 *
 *     Answers an incoming call. This function is called after receiving a
 *     HF_EVENT_CALL_STATE event that indicates an incoming call.  To reject
 *     the incoming call, use the HF_Hangup function.  When the call is
 *     accepted or rejected by the gateway, the application will be notified of 
 *     the call state change by the HF_EVENT_CALL_STATE event.
 *
 *     In addition, the HF_EVENT_COMMAND_COMPLETE event will be received.
 *
 *     When an active call exists and a second incoming call is indicated, 
 *     the HF_AnswerCall function will perform the equivalent of HF_CallHold 
 *     with a "HfHoldAction" of HF_HOLD_HOLD_ACTIVE_CALLS.  If call waiting
 *     is disabled, notification of a second incoming call will not occur (see 
 *     HF_EnableCallWaitNotify).
 *     
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - No incoming call exists.
 */
BtStatus HF_AnswerCall(HfChannel *Channel, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_DialNumber()
 *
 *     Initiates an outgoing call using a phone number.
 *
 *     During the process of calling, the HF_EVENT_CALL_STATE event will be
 *     generated to show the progress of the call.  Not all states are
 *     applicable to all services.  At a minimum, the application will be
 *     notified of the following states:  HF_CALL_STATUS_OUTGOING and
 *     HF_CALL_STATUS_ACTIVE.
 *  
 *     In addition, the HF_EVENT_COMMAND_COMPLETE event will be received.
 *
 *     If a call is already active, it must be put on hold before calling this
 *     function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Number - An ASCII string containing the number to be dialed.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_FAILED - A call cannot be made.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_DialNumber(HfChannel *Channel, U8 *Number, U16 Len, 
                       HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_MemoryDial()
 *
 *     Initiates an outgoing call using a memory location on the phone.
 *
 *     During the process of calling, the HF_EVENT_CALL_STATE event will be
 *     generated to show the progress of the call.  Not all states are
 *     applicable to all services.  At a minimum, the application will be
 *     notified of the following states:  HF_CALL_STATUS_OUTGOING and
 *     HF_CALL_STATUS_ACTIVE.
 *
 *     In addition, the HF_EVENT_COMMAND_COMPLETE event will be received.
 *
 *     If a call is already active, it must be put on hold before calling this
 *     function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Location - An ascii string containing the memory location 
 *                to be dialed.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_FAILED - A call cannot be made.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_MemoryDial(HfChannel *Channel, U8 *Location, U16 Len, 
                       HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_Redial()
 *
 *     Initiates an outgoing call based on the last number dialed in the 
 *     audio gateway.
 *
 *     During the process of calling, the HF_EVENT_CALL_STATE event will be
 *     generated to show the progress of the call.  Not all states are
 *     applicable to all services.  At a minimum, the application will be
 *     notified of the following states:  HF_CALL_STATUS_OUTGOING and
 *     HF_CALL_STATUS_ACTIVE.
 *
 *     In addition, the HF_EVENT_COMMAND_COMPLETE event will be received.
 *
 *     If a call is already active, it must be put on hold before calling this
 *     function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_FAILED - A call cannot be made.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_Redial(HfChannel *Channel, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_CallHold()
 *
 *     Issues a command to the audio gateway to manage multi-party calling.
 *     This function allows the application to perform explicit handling of
 *     3-Way calls (see HfHoldAction).  During the process of this command, 
 *     the HF_EVENT_CALL_STATE event will be generated to show the state 
 *     change of the call.
 *  
 *     In addition, the HF_EVENT_COMMAND_COMPLETE event will be received.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     HoldAction - Describes the type of hold function.
 *
 *     Index - Call to which the action applies.  Ignored if HoldAction is
 *            not 1 or 2.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_CallHold(HfChannel *Channel, HfHoldAction HoldAction, 
                     U8 Index, HfCommand *Command);

#if HF_USE_RESP_HOLD == XA_ENABLED
/*---------------------------------------------------------------------------
 * HF_QueryResponseHold()
 *
 *     This function is only available if HF_USE_RESP_HOLD is set to XA_ENABLED.
 *     Issues a Query for the state of Response and Hold on the audio gateway.  
 *     This feature is typically supported in Japanese markets
 *     (see HfResponseHoldState).  After the query is received by the audio
 *     gateway, the HF_EVENT_RESPONSE_HOLD_IND event will be received.
 *
 *     If HF_USE_CALL_MANAGER is set to XA_ENABLED, an HF_EVENT_CALL_STATE_IND
 *     event is received instead of the HF_EVENT_RESPONSE_HOLD_IND event.
 *
 *     In addition, the HF_EVENT_COMMAND_COMPLETE event will be received.
 *
 * Requires:
 *     HF_USE_RESP_HOLD enabled.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_QueryResponseHold(HfChannel *Channel, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_ResponseHold()
 *
 *     This function is only available if HF_USE_RESP_HOLD is set to XA_ENABLED.
 *     Issues a Response and Hold command to the audio gateway to manage 
 *     incoming calls.  This feature is typically supported in Japanese
 *     markets (see HfResponseHoldState).  When the specified action is
 *     taken, the HF_EVENT_RESPONSE_HOLD_IND event will be received, which
 *     should indicate the action that was taken.
 *
 *     In addition, the HF_EVENT_COMMAND_COMPLETE event will be received.
 *
 * Requires:
 *     HF_USE_RESP_HOLD enabled.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     RespHoldAction - Describes the type of action to take.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_ResponseHold(HfChannel *Channel, HfHoldAction RespHoldAction, 
                         HfCommand *Command);

#endif

/*---------------------------------------------------------------------------
 * HF_Hangup()
 *
 *     Terminates an existing (active) call, rejects an incoming call, or
 *     cancels an outgoing call. This function can be called whenever an active 
 *     call exists or after receiving a HF_EVENT_CALL_STATE event that indicates 
 *     an incoming or outgoing call.  When the call is terminated, the application
 *     will be notified of the call state change by the HF_EVENT_CALL_STATE event.
 *
 *     In addition, the HF_EVENT_COMMAND_COMPLETE event will be received.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 *
 *     BT_STATUS_FAILED - No call exists.
 */
BtStatus HF_Hangup(HfChannel *Channel, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_ListCurrentCalls()
 *
 *     Queries the audio gateway for any calls.
 *
 *     After making this call, the HF_EVENT_CALL_LIST_IND event will be
 *     generated for each call on the audio gateway.
 *
 *     If HF_USE_CALL_MANAGER is set to XA_ENABLED, an HF_EVENT_CALL_STATE_IND
 *     event is received instead of the HF_EVENT_CALL_LIST_IND event.
 *
 *     In addition, the HF_EVENT_COMMAND_COMPLETE event will be received.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_ListCurrentCalls(HfChannel *Channel, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_EnableCallerIdNotify()
 *
 *     Enables notification of the calling line identification.  When this
 *     command is complete, the HF_EVENT_COMMAND_COMPLETE event will be
 *     received.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Enabled - TRUE or FALSE.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_EnableCallerIdNotify(HfChannel *Channel, 
                                 BOOL Enabled, 
                                 HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_EnableCallWaitNotify()
 *
 *     Enables notification of call waiting.  When this command is complete, 
 *     the HF_EVENT_COMMAND_COMPLETE event will be received.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Enabled - TRUE or FALSE.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_EnableCallWaitNotify(HfChannel *Channel, BOOL Enabled, 
                                 HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_GenerateDtmf()
 *
 *     Send a DTMF code to the network. A call MUST be ongoing in order
 *     to generate a DTMF code.  The HF_EVENT_COMMAND_COMPLETE event will be 
 *     received when the DTMF code is sent.
 *
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     dtmfTone - A single ASCII character in the set 0-9, #, *, A-D.
 *
 * Returns:
 *     BT_STATUS_PENDING - The dtmf code was sent. The app will be
 *         notified with an HF_EVENT_DTMF_SENT event when the 
 *         AG's response is received.
 *                          
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_BUSY - No available RFCOMM packets.
 *
 *     BT_STATUS_FAILED - A service link is not active.
 *
 *     BT_STATUS_INVALID_PARM - Invalid ASCII character.
 *
 */
BtStatus HF_GenerateDtmf(HfChannel *Channel, U8 dtmfTone, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_GetLastVoiceTag()
 *
 *     Retrieves the number associated with the last voice tag recorded in the
 *     Hands-free unit.  When the number is received, the application's callback 
 *     function will receive the HF_EVENT_COMMAND_COMPLETE event.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_GetLastVoiceTag(HfChannel *Channel, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_EnableVoiceRecognition()
 *
 *     Enables/Disables voice recognition on the audio gateway.  When this
 *     command is complete, the HF_EVENT_COMMAND_COMPLETE event will be
 *     received.    
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Enabled - Set to TRUE if voice recognition is enabled, and FALSE
 *         if it is disabled.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The specified mode is already set.
 *
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_FAILED - Could not initiate voice recognition.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_EnableVoiceRecognition(HfChannel *Channel, BOOL Enabled, 
                                   HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_DisableNREC()
 *
 *     Disables noise reduction and echo canceling. When this command is 
 *     complete, the HF_EVENT_COMMAND_COMPLETE event will be received.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_DisableNREC(HfChannel *Channel, 
                        HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_ReportMicVolume()
 *
 *     Reports the current microphone volume of the Hands-free device.
 *
 *     When the command issued as a result of this call is completed,
 *     the HF_EVENT_COMMAND_COMPLETE event will be received by the
 *     application's callback function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Gain - The current gain level (0-15).
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_ReportMicVolume(HfChannel *Channel, U8 Gain, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_ReportSpeakerVolume()
 *
 *     Reports the current speaker volume of the Hands-free device.
 *
 *     When the command issued as a result of this call is completed,
 *     the HF_EVENT_COMMAND_COMPLETE event will be received by the
 *     application's callback function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Gain - The current gain level (0-15).
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_ReportSpeakerVolume(HfChannel *Channel, U8 Gain, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_QueryNetworkOperator()
 *
 *     Queries the audio gateway for the Network Operator.  The 
 *     HF_EVENT_NETWORK_OPERATOR event will be received from the audio gateway.
 *
 *     When the command issued as a result of this call is completed,
 *     the HF_EVENT_COMMAND_COMPLETE event will be received by the
 *     application's callback function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_QueryNetworkOperator(HfChannel *Channel, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_QuerySubscriberNumber()
 *
 *     Queries the audio gateway for the Subscriber Number.  The
 *     HF_EVENT_SUBSCRIBER_NUMBER event will be received from the audio gateway.
 *
 *     When the command issued as a result of this call is completed,
 *     the HF_EVENT_COMMAND_COMPLETE event will be received by the
 *     application's callback function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_QuerySubscriberNumber(HfChannel *Channel, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_EnableExtendedErrors()
 *
 *     Enables extended error codes on the audio gateway (see HfCmeError).
 *
 *     When the command issued as a result of this call is completed,
 *     the HF_EVENT_COMMAND_COMPLETE event will be received by the
 *     application's callback function.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_EnableExtendedErrors(HfChannel *Channel, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_SendAtCommand()
 *
 *     Sends any AT command.  The 'AtString' parameter must be initialized,
 *     and the AT command must be a properly formatted AT Commands.  
 *
 *     When the AT command is completed, the HF_EVENT_COMMAND_COMPLETE 
 *     event will be received by the application's callback function.  In 
 *     addition, any unsolicited result code not recognized by the Hands-free SDK
 *     will generate an HF_EVENT_AT_RESULT_DATA event.
 *
 *     When HF_USE_CALL_MANAGER, sending AT commands that affect the call state 
 *     should be avoided, because they may compromise the internal call states 
 *     of the Call Manager.  Use the API calls provided for initiating, 
 *     answering, or changing the state of ongoing calls.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     AtString - A pointer to an initialized AT Command string.
 *   
 *     Command - A command structure to be used for transmitting the
 *         command.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_SendAtCommand(HfChannel *Channel, const char *AtString,
                          HfCommand *Command);

#if HF_USE_PHONEBOOK_COMMANDS == XA_ENABLED
/*---------------------------------------------------------------------------
 * HF_QueryPhonebooks()
 *
 *     This function is only available when HF_USE_PHONEBOOK_COMMANDS is set to 
 *     XA_ENABLED.  Queries the Audio Gateway for its supported phonebooks.  
 *     When this command is issued successfully, the HF_EVENT_PHONEBOOK_STORAGE
 *     event will be received, followed by the HF_COMMAND_COMPLETE event.
 *
 * Requires:
 *     HF_USE_PHONEBOOK_COMMANDS enabled;
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *   
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_QueryPhonebooks(HfChannel *Channel, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_SelectPhonebook()
 *
 *     This function is only available when HF_USE_PHONEBOOK_COMMANDS is set to 
 *     XA_ENABLED.  Makes the specific phonebook the active one for subsequent 
 *     calls.  When this the HF_COMMAND_COMPLETE event will be received.
 *
 * Requires:
 *     HF_USE_PHONEBOOK_COMMANDS enabled;
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Command - A command structure to be used for transmitting the
 *         command.
 *   
 * Returns: 
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_SelectPhonebook(HfChannel *Channel, 
                            HfPhonebooks Phonebook, 
                            HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_GetCurrentPhonebookInfo()
 *
 *     This function is only available when HF_USE_PHONEBOOK_COMMANDS is set to 
 *     XA_ENABLED.  Queries for the active phonebook.  When this command is 
 *     issued successfully, the HF_EVENT_PHONEBOOK_INFO event will be received, 
 *     followed by the HF_COMMAND_COMPLETE event.
 *
 * Requires:
 *     HF_USE_PHONEBOOK_COMMANDS enabled;
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_GetCurrentPhonebookInfo(HfChannel *Channel, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_GetPhonebookSize()
 *
 *     This function is only available when HF_USE_PHONEBOOK_COMMANDS is set to 
 *     XA_ENABLED.  Gets the number of entries and the size of the elements of 
 *     the active phonebook.  When this command is issued successfully, the 
 *     HF_EVENT_PHONEBOOK_INFO event will be received, followed by the 
 *     HF_COMMAND_COMPLETE event.
 *
 * Requires:
 *     HF_USE_PHONEBOOK_COMMANDS enabled;
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_GetPhonebookSize(HfChannel *Channel, HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_ReadPhonebooksEntries()
 *
 *     This function is only available when HF_USE_PHONEBOOK_COMMANDS is set to XA_ENABLED.
 *     Reads entries from the specified phonebook in the specified 
 *     range.  When this command is issued successfully, the 
 *     HF_EVENT_PHONEBOOK_ENTRY event will be received, followed by the 
 *     HF_COMMAND_COMPLETE event.
 *
 * Requires:
 *     HF_USE_PHONEBOOK_COMMANDS enabled;
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_ReadPhonebookEntries(HfChannel *Channel, 
                                 U16 From, U16 To, 
                                 HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_FindPhonebookEntries()
 *
 *     This function is only available when HF_USE_PHONEBOOK_COMMANDS is set to
 *     XA_ENABLED.  Finds all entries beginning with the specified string.  
 *     Queries the Audio Gateway for its supported phonebooks.  When this 
 *     command is issued successfully, the HF_EVENT_PHONEBOOK_ENTRY event will 
 *     be received, followed by the HF_COMMAND_COMPLETE event.
 *
 * Requires:
 *     HF_USE_PHONEBOOK_COMMANDS enabled;
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_FindPhonebookEntries(HfChannel *Channel, const char *Text, 
                                 HfCommand *Command);

/*---------------------------------------------------------------------------
 * HF_WritePhonebookEntry()
 *
 *     This function is only available when HF_USE_PHONEBOOK_COMMANDS is set to
 *     XA_ENABLED.  Writes the phonebook entry.  When this the request is done
 *     HF_COMMAND_COMPLETE event will be received.
 *
 * Requires:
 *     HF_USE_PHONEBOOK_COMMANDS enabled;
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started, the application will be 
 *         notified when the command has completed (via the callback function 
 *         registered by HF_Register).
 *
 *     BT_STATUS_NOT_FOUND - The specifiec channel has not been registered.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because a service link
 *         does not exist to the audio gateway.
 *
 *     BT_STATUS_INVALID_PARM - A parameter invalid or not properly 
 *         initialized (XA_ERROR_CHECK only).
 */
BtStatus HF_WritePhonebookEntry(HfChannel *Channel, 
                                U16 Index, 
                                const char *Number, 
                                const char *Text, 
                                U8  Type,
                                HfCommand *Command);
#endif

/*---------------------------------------------------------------------------
 * HF_IsChannelOpen()
 *
 *     Returns true of a channel is open.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *
 *     TRUE - The specified channel is open.
 *
 *     FALSE - The channel is closed.
 */
BOOL HF_IsChannelOpen(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_IsACLConnected()
 *
 *     Returns TRUE if the ACL link is up.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE - The ACL link is connected.
 *
 *     FALSE - The ACL link is disconnected.
 */
BOOL HF_IsACLConnected(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_IsHandsfreeConnected()
 *
 *     Returns TRUE if the handsfree profile is connected.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE - The handsfree profile is connected.
 *
 *     FALSE - The handsfree profile is not connected.
 */
BOOL HF_IsHandsfreeConnected(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_IsAudioConnected()
 *
 *     Returns TRUE if an audio connection is up.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE - An audio link is up.
 *     
 *     FALSE - No audio link is up.
 */
BOOL HF_IsAudioConnected(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_RemoteDevice()
 *
 *     Returns the remote device associated with the channel.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     A pointer to the remote device.
 */
BtRemoteDevice * HF_RemoteDevice(HfChannel *Channel);

#if HF_USE_CALL_MANAGER == XA_ENABLED
/*---------------------------------------------------------------------------
 * HF_CallState()
 *
 *     This function exists only if HF_USE_CALL_MANAGER is defined as XA_ENABLED.  
 *     Returns the current call state of the specified line.
 *
 * Requires:
 *     HF_USE_CALL_MANAGER enabled;
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *     Line - The line associated with a call.
 *
 * Returns:
 *     HfCallStatus
 */
HfCallStatus HF_CallState(HfChannel *Channel, U8 Line);
#endif

/*---------------------------------------------------------------------------
 * HF_ProfileVersion()
 *
 *     Returns the profile version retrieved from the SDP database of the
 *     remote device for this service connection.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     HfGatewayProfile
 *
 */
HfGatewayVersion HF_ProfileVersion(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_GatewayFeatures()
 *
 *     Returns the gateway features of the audio gateway.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     HfGatewayVersion
 *
 */
HfGatewayFeatures HF_GatewayFeatures(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_GatewayHoldFeatures()
 *
 *     Returns the hold features of the audio gateway.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     HfGatewayHoldFeatures
 *
 */
HfGwHoldFeatures HF_GatewayHoldFeatures(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_SpeakerVolume()
 *
 *     Returns the current value of speaker volume.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     The current value of speaker gain.
 */
U8 HF_SpeakerVolume(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_MicVolume()
 *
 *     Returns the current value of microphone volume.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     The current value of microphone gain.
 */
U8 HF_MicVolume(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_IsNRECEnabled()
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
BOOL HF_IsNRECEnabled(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_IsInbandRingEnabled()
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
BOOL HF_IsInbandRingEnabled(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_IsCallIdNotifyEnabled()
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
BOOL HF_IsCallIdNotifyEnabled(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_IsVoiceRecActive()
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
BOOL HF_IsVoiceRecActive(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_IsCallWaitingActive()
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
BOOL HF_IsCallWaitingActive(HfChannel *Channel);

/*---------------------------------------------------------------------------
 * HF_RegisterCmdOverride()
 *
 *     Registers a callback function to be called any time the command
 *     state machine is called.
 *
 *     When an API function is called in the Hands-free SDK, the request is queued
 *     up to be executed when the channel becomes available.  When the channel
 *     is available for the next command on the queue, the command state machine
 *     is called repeatedly to allow the work of the logical command to be
 *     performed.  Each call the command state machine results in
 *     either an AT command being sent, or completion of a command.  Each time 
 *     an AT command is sent and a response is received (OK, ERROR, CME ERROR, or 
 *     a command timeout), the state machine is called again.  When the work of the 
 *     logical command is completed, the command state machine completes the 
 *     command and notifies the application.
 *
 *     This function allows the registration of a callback function that will
 *     be called each time the command state machine is called.  This allows
 *     an application to override the default behavior of an API call.  The
 *     callback function can determine which function call is being executed and
 *     set up its own AT commands for transmission.
 *
 *     When the override function sets up an AT command for transmission, it
 *     must return BT_STATUS_PENDING.  This indicates to the command state
 *     machine that it must send the AT command.  When the override function 
 *     has completed its work, it must return BT_STATUS_SUCCESS.  This indicates
 *     to the command state machine that it must complete the command and
 *     notify the application (no AT command will be sent).  If the override
 *     functions does not perform any work, it must return BT_STATUS_FAILED to
 *     indicate to the command state machine that it must take the default
 *     action.
 *
 *     In order to use this feature properly, AT commands must be set up using
 *     the internal HfChannel structure field "atCommand."  This field is not 
 *     normally a public field, but it can be accessed directly and is of the
 *     type AtCommandValues (see HF_SendAtCommand).  The override function
 *     must not send the AT command itself, but must rely on the command
 *     state machine to transmit the command.
 *                               .^..
 *     In addition, the override function can look at the "state" field of
 *     the HfCommand structure to determine how many times the command state
 *     machine has been called for this particular command.  Each time the
 *     state machine is called, the "state" field is incremented.  Initially
 *     it will have the value of 0.  If multiple AT commands must be sent for
 *     a particular logical command, the "state" field will help to identify
 *     the command to send, or if the command is complete.
 *
 *     This feature adds a lot of power, but must be used properly as defined
 *     above in order for the internal states of the SDK to be maintained
 *     properly.
 *
 *     To unregister the callback, this function should be called with a NULL
 *     parameter.
 *
 * Parameters:
 *     Callback - A pointer to the function that will be called.
 *
 * Returns:
 *     void
 */
void HF_RegisterCmdOverride(HfCmdOverride Callback);

#if HF_SNIFF_TIMER >= 0
/*---------------------------------------------------------------------------
 * HF_EnableSniffMode
 *
 *     Enables/Disables placing link into sniff mode.
 *
 * Requires:
 *     HF_SNIFF_TIMER >= 0.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 *     Enabled - If TRUE, sniff mode will be used, otherwise sniff mode is
 *               disabled
 */
BtStatus HF_EnableSniffMode(HfChannel *Channel, BOOL Enabled);

/*---------------------------------------------------------------------------
 * HF_IsSniffModeEnabled
 *
 *     Returns TRUE when sniff mode is enabled on the specified handler.
 *
 * Requires:
 *     HF_SNIFF_TIMER >= 0.
 *
 * Parameters:
 *     Channel - Pointer to a registered channel structure.
 *
 * Returns:
 *     TRUE if sniff mode is enabled.
 *
 *     FALSE if sniff mode is not enabled.
 */
BOOL HF_IsSniffModeEnabled(HfChannel *Channel);
#define HF_IsSniffModeEnabled(c) (CMGR_GetSniffTimer(&((c)->cmgrHandler)) > 0)
#endif

/*---------------------------------------------------------------------------
 * HF_SetSniffExitPolicy()
 *
 *     Sets the policy for exiting sniff mode on the specified channel.  The 
 *     policy can be set to HF_SNIFF_EXIT_ON_SEND or HF_SNIFF_EXIT_ON_AUDIO_LINK.
 *     These values can also be OR'd together to enable both (See 
 *     HfSniffExitPolicy).
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
BtStatus HF_SetSniffExitPolicy(HfChannel *Channel, HfSniffExitPolicy Policy);
#define HF_SetSniffExitPolicy(c, p) CMGR_SetSniffExitPolicy(&((c)->cmgrHandler), (p));

/*---------------------------------------------------------------------------
 * HF_GetSniffExitPolicy()
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
HfSniffExitPolicy HF_GetSniffExitPolicy(HfChannel *Channel);
#define HF_GetSniffExitPolicy(c)  CMGR_GetSniffExitPolicy(&((c)->cmgrHandler))

#endif /* __HF_H_ */

