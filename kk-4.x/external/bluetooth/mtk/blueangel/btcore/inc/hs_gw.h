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

#ifndef __HS_GW_H_
#define __HS_GW_H_
/****************************************************************************
 *
 * File:
 *     $Workfile:hs_gw.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: This file contains the function prototypes and definitions
 *              for the headset audio gateway profile core.
 *             
 * Created:     September 12, 2000
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
/*****************************************************************************
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 * 
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *  *
 ****************************************************************************/

#include <me_adp.h>


/* Make sure BT_HSP_GATEWAY is enabled if no other choice has been made */
#ifdef __BT_HFG_PROFILE__

#ifndef BT_HSP_GATEWAY
#define BT_HSP_GATEWAY XA_ENABLED
#endif

#else

#define BT_HSP_GATEWAY XA_DISABLED

#endif /* __BT_HFG_PROFILE__ */


#if BT_HSP_GATEWAY == XA_ENABLED

#if NUM_SCO_CONNS == 0
#error Headset Gateway requires support for SCO connections
#endif

/*---------------------------------------------------------------------------
 * Headset Audio Gateway Profile layer 
 *
 *     The Headset Audio Gateway complements the Headset Profile.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------
 * AG_SECURITY constant
 *
 *     Determines whether security is enabled for headset or not.  If 
 *     AG_SECURITY is defined, BT_SECURITY must be defined as well.
 *
 */
#if AG_SECURITY == XA_ENABLED && BT_SECURITY != XA_ENABLED
#error "Bluetooth Security must be enabled for Audio Gateway Security!"
#endif

#ifndef AG_SECURITY
#define AG_SECURITY XA_ENABLED
#endif

/*-------------------------------------------------------------------------
 * AG_SCO_SETTINGS constant
 *
 *     Determines whether SCO_SetSettings() will be called before an SCO
 *     connection is established.
 */
#ifndef AG_SCO_SETTINGS
#define AG_SCO_SETTINGS XA_ENABLED
#endif


/*-------------------------------------------------------------------------
 * AG_CONNECT_NO_RING constant
 *
 *     Defines whether to open the SCO connection w/o sending the RING.
 *     Default behavior is to send the RING, and only open the SCO 
 *     connection upon receipt of the CKPD command from the headset.
 */
#ifndef AG_CONNECT_NO_RING
#define AG_CONNECT_NO_RING XA_DISABLED
#endif

/*-------------------------------------------------------------------------
 * AG_RINGTIMER constant
 *
 *     Defines the ring timeout. Default is 5 seconds.
 */
#ifndef AG_RINGTIMER
#define AG_RINGTIMER        5000
#endif

/*-------------------------------------------------------------------------
 * AG_MAX_RING_COUNT constant
 *
 *     Defines the maximum number of rings before timing out and disconnecting.
 */
#ifndef AG_MAX_RING_COUNT
#define AG_MAX_RING_COUNT   5
#endif

/*-------------------------------------------------------------------------
 * AG_MAX_VOLUME constant
 *
 *     Defines the maximum volume level of the microphone and speaker.
 *     The spec defines this as 15 (profile_10_b.pdf p. 206).
 *     
 */
#define AG_MAX_VOLUME      15

/*-------------------------------------------------------------------------
 * AG_DEFAULT_VOLUME constant
 *
 *     Defines the default volume level of the microphone and the speaker 
 *
 *     The volume range is 0-15 (profile_10_b.pdf p. 206).  The default
 *     volume level can be overridden in overide.h.  If it is not overridden
 *     it will be set to 7.  The default volume must be less than the max
 *     volume.
 */
#ifndef AG_DEFAULT_VOLUME
#define AG_DEFAULT_VOLUME  7
#endif

#if AG_DEFAULT_VOLUME > AG_MAX_VOLUME
#error AG_DEFAULT_VOLUME must be less than AG_MAX_VOLUME
#endif


#if AG_SECURITY == XA_ENABLED
#ifndef MAX_PIN_LEN
#define MAX_PIN_LEN     16
#endif
#endif

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/






/*---------------------------------------------------------------------------
 * AgState type
 *
 *     The state machine in the audio gateway uses the following states:
 */
typedef U8 AgState;

/* There may be an ACL link to the headset's device, but there is not an RFCOMM
 *  channel open or an SCO connection established. 
 */
#define AG_IDLE             0

/* An ACL link does not exits, and the gateway is selecting a device to which 
 *  it will establish the ACL connection.
 */
#define AG_SELECTING        1

/* ACL and RFCOMM connections have been established by the headset and an AT
 *  command has been received, signaling a request by the headset to establish
 *  an SCO connection to the audio gateway. The audio gateway will automatically
 *  accept the connection request and will establish an SCO connection to the 
 *  headset.  If the SCO connection is successful, the state will change to 
 *  AG_CONNECTED and the application will be notified via the callback.  If 
 *  the connection is unsuccessful, the state will change to AG_IDLE, and the
 *  application will be notified.
 */
#define AG_INCOMING         2

/* A user-initiated action has started an outgoing connection to headset.
 *  The audio gateway will establish an ACL connection and do an SDP 
 *  query of the headset. If the SDP query succeeds, the audio gateway
 *  will open an RFCOMM channel to the headset and will send a RING result code
 *  requesting an SCO connection.  When the SCO connection is established the 
 *  state will change to  AG_CONNECTED.  If the connection fails the state will 
 *  move back to AG_IDLE. The application will be notified of the state change
 *  via the callback.
 */
#define AG_OUTGOING         3

/* An SCO connection has been established to the headset.  This requires
 *  that ACL and RFCOMM connections have already been established.
 */
#define AG_CONNECTED        4

/* A SCO connection establishment is ongoing. HSP is waiting the response from stack.
 */
#define AG_SCO_CONNECTING		5

/* An SCO connection has been established to the headset.
 */
#define AG_SCO_CONNECTED        	6

/* An SCO connection release is ongoing.  HSP is waiting the response from stack.
 */
#define AG_SCO_DISCONNECTING	7


/* If AG_DoDisconnect() is called on the audio gateway, the audio
 *  gateway will move into this state and will then disconnect the SCO
 *  connection, RFCOMM channel, and ACL link. When the call to disconnect the
 *  link returns, the audio gateway will change to the AG_IDLE state
 *  and will notify the application via the callback.
 */
#define AG_DISCONNECTING    	8

/* End of AgState */

typedef U8	AgAuthorize;

/* 
 * Not in authorize state
 */
#define AG_AUTH_STATE_NONE		0

/* 
 * AG_EVENT_CONNECT_IND is sent to ADP and wait ADP to accept/reject the request
 */
#define AG_AUTH_STATE_WAIT_RSP	1

/* 
 * ADP accept the request and wait for response
 */
#define AG_AUTH_STATE_ACCEPTING	2

/* 
 * ADP reject the request and wait for response
 */
#define AG_AUTH_STATE_REJECTING	3

/*---------------------------------------------------------------------------
 * AgButton type
 *
 *     The audio gateway can have the following buttons on its interface.
 */
typedef U8 AgButton;

/** The main button that controls link connection and disconnection */
#define AG_BUTTON           0

/** Microphone volume down button */
#define AG_BUTTON_MVDOWN    1

/** Microphone volume up button */
#define AG_BUTTON_MVUP      2

/** Speaker volume down button */
#define AG_BUTTON_SVDOWN    3

/** Speaker volume up button */
#define AG_BUTTON_SVUP      4

/* End of AgButton */

/*---------------------------------------------------------------------------
 * AgEvent type
 *
 *     An AgEvent is sent in the callback to the application to notify the app
 *     of any pertinent information, such as a state change or volume change.
 */
typedef U8 AgEvent;

/** The state has changed */
#define AG_STATE_CHANGE     0

/** The microphone volume has changed */
#define AG_MICVOL_CHANGE    1

/** The speaker volume has changed */
#define AG_SPKVOL_CHANGE    2


#define AG_EVENT_CONNECTED 			3
#define AG_EVENT_DISCONNECTED 		4
#define AG_EVENT_AUDIO_CONNECTED		5
#define AG_EVENT_AUDIO_DISCONNECTED	6
#define AG_EVENT_CKPD					7
#define AG_EVENT_CONNECT_IND			8

/* End of AgEvent */

/*-------------------------------------------------------------------------
 * AgVolumeType type
 *
 *     Specifies an audio entity with adjustable volume. AG_ChangeVolume
 *     is used to set the volume.
 */
typedef U8 AgVolumeType;

#define AG_MIC  0x4D /* 'M' Microphone  */

#define AG_SPK  0x53 /* 'S' Speaker  */

/* End of AgVolumeType */

/*-------------------------------------------------------------------------
 * AgVolumeChange type
 *
 *     Specifies the kind of volume change required for an audio entity.
 */
typedef U8 AgVolumeChange;

#define AG_UPVOL    0     /* Increased volume (louder) */
#define AG_DOWNVOL  1     /* Decreased volume (quieter) */
#define AG_SETVOL   2     /* Absolute volume (requests volume to be set to
                           * a specific level)
                           */

/* End of AgVolumeChange */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AgCallbackInfo structure
 *
 * A point to an AgCallbackInfo struct is sent to the app's callback notifying
 * the app of any start or volume changes.  If the event is AG_STATE_CHANGE,
 * State will contain the new state of the audio gateway. If event is
 * AG_MICVOL_CHANGE, Volume contains the new microphone volume.  If event is
 * AG_SPKVOL_CHANGE, Volume contains the new speaker volume. If event is 
 * AG_PIN_REQUEST, bdAddr contains the BD_ADDR of the headset.
 *
 */
typedef struct _AgCallbackInfo {
    AgEvent  event;

    union {
        U8          volume;
        AgState     state;
#if AG_SECURITY == XA_ENABLED
        BD_ADDR     bdAddr;
#endif

	BtRemoteDevice *remDev;

    } info;
} AgCallbackInfo;


typedef void (*AgCallback)(AgCallbackInfo *);
/*---------------------------------------------------------------------------
 * AgRegistration structure
 *
 * The AgRegistration struct contains a pointer to the app's callback function
 * and is passed into AG_Register().  A struct is used for future expandability.
 *
 */
typedef struct _AgRegistration {
    AgCallback  callback;
} AgRegistration;


/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * AG_Register()
 *
 *     Initializes all necessary data structures, registers the
 *     RfServerChannel, registers the headset's callback handler with the ME,
 *     and adds the appropriate SDP record.  If AG_SECURITY is enabled it will
 *     also register a security record and a pairing handler.
 *
 * Parameters:
 *     reg - A structure containing a pointer to the callback function
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation completed successfully.
 *
 *     BT_STATUS_BUSY -  The operation failed because the gateway has already
 *                         been initialized.  
 *
 *     BT_STATUS_FAILED - the operation failed because the RfServerChannel
 *                        registration failed, or the SDP record entry failed
 *                        (XA_ERROR_CHECK builds only)
 */
BtStatus AG_Register(const AgRegistration *reg);

/*---------------------------------------------------------------------------
 * AG_Deregister()
 *
 *      Deregisters the RFCOMM server channel, cancels any timers, removes the 
 *      SDP records, and changes the state to AG_IDLE.  If AG_SECURITY is
 *      enabled it will also unregister the security record
 * 
 * Parameters:
 *      NONE
 *
 * Returns:
 *     BT_STATUS_SUCCESS - operation completed successfully.
 *
 *     BT_STATUS_FAILED - The audio gateway has not been initialized.
 *
 *     BT_STATUS_BUSY - The operation failed because a connection is still
 *                      open to the headset.
 */
BtStatus AG_Deregister(void);


/*---------------------------------------------------------------------------
 * AG_Connect()
 *
 *	Connect to a HS device of which the address is specified in parameter "headset."
 *    When the connection is successfully created, 
 *
 * Parameters:
 *      headset - The 48-bit Bluetooth device address of the Headset.
 *
 * Returns:
 *
 *      BT_STATUS_PENDING - The operation is queued and wait for response.
 *
 *      BT_STATUS_FAILED -  There is an existing connection or failed due to create ACL,
 *						SDP query, or open RFCOMM channel.
 */
BtStatus AG_Connect(const BD_ADDR *headset);

/*---------------------------------------------------------------------------
 * AG_Disconnect()
 *
 *	Remove the existed HSP connection.
 *
 * Returns:
 *
 *      BT_STATUS_PENDING - The operation is queued and wait for response.
 *
 *      BT_STATUS_FAILED -  There is an existing connection or failed due to create ACL,
 *						SDP query, or open RFCOMM channel.
 */
BtStatus AG_Disconnect(void);

/*---------------------------------------------------------------------------
 * AG_CreateAudioLink()
 *
 *	Open audio link to connected HS device.
 *
 * Returns:
 *
 *      BT_STATUS_SUCCESS -  Audio connection is successfully created.
 *      BT_STATUS_PENDING - The creation operation is queued and wait for response.
 *	  BT_STATUS_NO_CONNECTION - Data connection is not exist.
 *
 */
BtStatus AG_CreateAudioLink(void);

/*---------------------------------------------------------------------------
 * AG_DisconnectAudioLink()
 *
 *	Close the existed audio connection.
 *
 * Returns:
 *
 *      BT_STATUS_PENDING - The operation is queued and wait for response.
 *
 *      BT_STATUS_FAILED -  There is no existing audio connection.
 */
BtStatus AG_DisconnectAudioLink(void);

/*---------------------------------------------------------------------------
 * AG_AcceptLink()
 *
 *	Close the existed audio connection.
 *
 * Returns:
 *
 *      BT_STATUS_PENDING - The operation is queued and wait for response.
 *
 *      BT_STATUS_FAILED -  There is no existing audio connection.
 */
BtStatus AG_AcceptLink(void);

/*---------------------------------------------------------------------------
 * AG_RejectLink()
 *
 *	Close the existed audio connection.
 *
 * Returns:
 *
 *      BT_STATUS_PENDING - The operation is queued and wait for response.
 *
 *      BT_STATUS_FAILED -  There is no existing audio connection.
 */
BtStatus AG_RejectLink(void);

/*---------------------------------------------------------------------------
 * AG_RegisterHeadset()
 *
 *      Registers the Bluetooth device address of the hedset with the gateway.
 *      This function can be used to override the automatic headset device
 *      selection procedures.
 *
 * Parameters:
 *      headset - The 48-bit Bluetooth device address of the Headset.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation completed successfully.
 *
 *      BT_STATUS_FAILED -  There is an existing connection.
 */
BtStatus AG_RegisterHeadset(const BD_ADDR *headset);

/*---------------------------------------------------------------------------
 * AG_EnableSecurity()
 *
 *      Turns security on or off (according to the value of the flag)
 *
 * Parameters:
 *      flag - If flag is true, security will be enabled, else it will be disabled
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation completed successfully.
 *
 *     BT_STATUS_FAILED - An error occurred while registering the security record.
 */
#if AG_SECURITY == XA_ENABLED
BtStatus AG_EnableSecurity(BOOL flag);
#endif

/*---------------------------------------------------------------------------
 * AG_DoConnect()
 *
 *      Starts the connection process to the headset.  The app's callback will
 *      be notified of a state change to AG_OUTGOING.  An ACL connection will
 *      be established and then the audio gateway will send an SDP query
 *      to the headset.  If the SDP query is successful the audio gateway 
 *      will open an RFCOMM channel to the headset and will then send a RING
 *      result code requesting an SCO connection.  The headset, upon receiving 
 *      this command will notify the headset application of the incoming call.
 *      If the headset application accepts the incoming call, the headset will send
 *      an AT+CKPD command to the audio gateway accepting the call.  When the
 *      audio gateway receives this command it will establish an SCO
 *      connection to the headset.  When the SCO connection has been established 
 *      the audio gateway will change its state to AG_CONNECTED and notify
 *      the application of the state change via the callback. This function
 *      returns after starting the ACL link.  If anything fails after this point
 *      in the connection setup, such as the RFCOMM channel or SDP query, the
 *      app's callback will be notified of a state change to AG_IDLE.
 *
 * Parameters:
 *      NONE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The connection has already been established.
 *
 *      BT_STATUS_PENDING - The connection has started, the app will be
 *                          notified when the connection is complete via
 *                          the callback.
 *
 *      BT_STATUS_FAILED -  The audio gateway has not been initialized, or
 *                          there was an error selecting the device to
 *                          connect to.
 */
BtStatus AG_DoConnect(void);

/*---------------------------------------------------------------------------
 * AG_DoDisconnect()
 *      If the audio gateway application requests a disconnect from the 
 *      headset, the audio gateway will close the SCO connection, close
 *      the RFCOMM channel and disconnect the ACL link (if the ACL link is being
 *      used by another application, the disconnect ACL link command just returns
 *      success).  Then the audio gateway will change its state to AG_IDLE
 *      and will notify the application via the callback.
 *
 * Parameters:
 *      NONE
 *
 * Returns:
 *      BT_STATUS_SUCCESS - All links have been disconnected.
 *
 *      BT_STATUS_PENDING - Disconnect has started, app will be notified
 *                          via the callback when disconnect is complete.
 *
 *      BT_STATUS_FAILED - No current connection, or connection failed to 
 *                         disconnect.
 */
BtStatus AG_DoDisconnect(void);

/*---------------------------------------------------------------------------
 * AG_ChangeVolume()
 *
 *      Changes the volume of microphone or speaker and sends a message to the
 *      headset.  The application is notified (via the callback) of the volume 
 *      change.
 *
 * Parameters:
 *      type -    Either AG_MIC (for Microphone) or AG_SPK (for Speaker).
 *
 *      dir -     Either AG_UPVOL, AG_DOWNVOL, or AG_SETVOL.
 *
 *      vol -     If dir is set to AG_SETVOL, then a valid volume level must
 *                be supplied.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The volume change message has been sent to the gateway.
 *
 *      BT_STATUS_BUSY - No RFCOMM packets available.
 *
 *      BT_STATUS_FAILED - There was an error sending the RFCOMM message.
 *
 *      BT_STATUS_INVALID_PARM - Invalid volume level (XA_ERROR_CHECK only).
 */
BtStatus AG_ChangeVolume(AgVolumeType type, AgVolumeChange dir, U8 vol);

/*---------------------------------------------------------------------------
 * AG_ButtonPressed()
 *
 *      This function is for convenience only, depending on which button was 
 *      pressed and on what state the audio gateway is in, it will make calls 
 *      to other API functions.  An app can call the other API functions 
 *      (AG_DoConnect(), AG_DoDisconnect() and AG_ChangeVolume() ) directly if
 *      it wishes.
 * 
 * Parameters: 
 *      ButtonNum - The ID of the button that was pressed.
 *
 * Returns: 
 *      All valid return values from AG_DoConnect(), AG_DoDisconnect(), and 
 *      AG_ChangeVolume().
 *
 */
BtStatus AG_ButtonPressed(AgButton   buttonNum);

/*---------------------------------------------------------------------------
 * AG_GetState()
 *
 *      This function is also for convenience.  The app will be notified of any 
 *      changes to the state through the callback function. The app can call
 *      this function if it loses track of the state.
 * 
 * Parameters:
 *
 * Returns:
 *      AgState - The current state of the audio gateway
 */
AgState AG_GetState(void);

/*---------------------------------------------------------------------------
 * AG_GetRemoteDevice()
 *
 * 
 * Parameters:
 *
 * Returns:
 *      BtRemoteDevice - Pointer to the remote device.
 */
BtRemoteDevice *AG_GetRemoteDevice(void);
#if defined( HSP_PARTITION )
BtStatus AG_SendRing(void);
BtStatus AG_SendMessage(AgMsgType type);
#else
BtStatus MTKAgSendRing(void);
BtStatus MTKAgCloseLink(void);
BtStatus MTKAgOpenSCO(void);
BtStatus MTKAgCloseSCO(void);
#endif
void MTKAgSetConnectLevel(U8 level);
void MTKAgChangeState(AgState newstate);


#endif /* BT_HSP_GATEWAY */
#endif /* _HS_GW_H_ */
