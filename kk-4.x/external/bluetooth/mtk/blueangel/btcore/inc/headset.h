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

#ifndef __HEADSET_H_
#define __HEADSET_H_
/****************************************************************************
 *
 * File:
 *     $Workfile:headset.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description: This file contains the function prototypes and definitions 
 *              for the headset profile core.
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

#include <sdp_adp.h>


/* Make sure BT_HSP_HEADSET is enabled if no other choice has been made */
#ifndef BT_HSP_HEADSET
#define BT_HSP_HEADSET XA_ENABLED
#endif

#if BT_HSP_HEADSET == XA_ENABLED

#if NUM_SCO_CONNS == 0
#error Headset requires support for SCO connections
#endif

/*---------------------------------------------------------------------------
 * Headset Profile layer
 *
 *     The Headset Profile contains all of the core functionality of a
 *     Bluetooth headset as defined in the profile spec.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

/*-------------------------------------------------------------------------
 * HS_MAX_VOLUME constant
 *
 *     Defines the maximum volume level of the microphone and speaker.
 *     The spec defines this as 15 (profile_10_b.pdf p. 206).
 *     
 */
#define HS_MAX_VOLUME      15

/*-------------------------------------------------------------------------
 * HS_DEFAULT_VOLUME constant
 *
 *     Defines the default volume level of the microphone and the speaker 
 *
 *     The volume range is 0-15 (profile_10_b.pdf p. 206).  The default
 *     volume level can be overridden in overide.h.  If it is not overridden
 *     it will be set to 7.  The default volume must be less than the max
 *     volume.
 */
#ifndef HS_DEFAULT_VOLUME
#define HS_DEFAULT_VOLUME  7
#endif

#if HS_DEFAULT_VOLUME > HS_MAX_VOLUME
#error HS_DEFAULT_VOLUME must be less than HS_MAX_VOLUME
#endif
 
/*-------------------------------------------------------------------------
 * HS_SECURITY constant
 *
 *     Determines whether security is enabled for headset or not.  If 
 *     HS_SECURITY is defined, BT_SECURITY must be defined as well.
 *
 */
#ifndef HS_SECURITY
#define HS_SECURITY XA_DISABLED
#endif

#if HS_SECURITY == XA_ENABLED && BT_SECURITY != XA_ENABLED
#error "Bluetooth Security must be enabled for Headset Security!"
#endif

#if HS_SECURITY == XA_ENABLED
#ifndef MAX_PIN_LEN
#define MAX_PIN_LEN     16
#endif
#endif

/*-------------------------------------------------------------------------
 * HS_USEDDB constant
 *
 *     Determines whether Headset will use the Device Database to store the 
 *     Bluetooth Address of the Audio Gateway in non-volatile memory. 
 *     WARNING!  If HS_USEDDB is enabled, the Headset will take over the
 *     DDB and delete all current entries in the DDB.  Only enable HS_USEDDB
 *     if no other applications will be running on this device.
 */
#if HS_USEDDB == XA_ENABLED && BT_SECURITY != XA_ENABLED
#error "Bluetooth Security must be enabled to use DDB"
#endif

#ifndef HS_USEDDB
#define HS_USEDDB XA_DISABLED
#endif

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/


/*---------------------------------------------------------------------------
 * HsState type
 *
 *     The state machine in the headset uses these states.
 */
typedef U8 HsState;

/* There may be an ACL link to the audio gateway's device, but there is not an 
 *  RFCOMM channel open or an SCO connection established.
 */
#define HS_IDLE             0

/* ACL and RFCOMM connections have been established by the audio gateway
 *  and a RING has been received, signaling a request by the audio gateway to 
 *  establish an SCO connection to the headset.  If the user does not respond
 *  and the audio gateway terminates the RFCOMM channel, the state will move
 *  to HS_IDLE. If the user accepts the connection request and the SCO connection
 *  is established, the state will move to HS_CONNECTED.
 */
#define HS_INCOMING         1

/* A user-initiated action has started an outgoing connection to an audio gateway. 
 *  The headset will establish an ACL connection and do an SDP query of the audio
 *  gateway. If the SDP query succeeds, the headset will open an RFCOMM channel
 *  to the audio gateway and will send a +CKPD command requesting an SCO 
 *  connection.  When the SCO connection is established the state will move to 
 *  HS_CONNECTED.  If the connection fails the state will move back to HS_IDLE.
 */
#define HS_OUTGOING         2

/* An SCO connection has been established to an audio gateway.  This requires
 *  that an ACL and RFCOMM connections have also been established.
 */
#define HS_CONNECTED        3

/* If HS_Disconnect() is called on the headset, the headset will move into this
 *  state and will send a +CKPD command to the audio gateway requesting a
 *  disconnect.  The audio gateway will then disconnect the SCO connection
 *  and the RFCOMM channel. When the RFCOMM channel has been disconnected the 
 *  headset will move to the HS_IDLE state.  The audio gateway may optionally 
 *  close the ACL connection.
 */
#define HS_DISCONNECTING    4

/* End of HsState */

/*---------------------------------------------------------------------------
 * HsButton type
 *
 *     The headset audio gateway can have the following buttons on its
 *     interface.
 */
typedef U8 HsButton;

/* The main button that controls link connection and disconnection. */
#define HS_BUTTON           0

/* Microphone volume down button. */
#define HS_BUTTON_MVDOWN    1

/* Microphone volume up button. */
#define HS_BUTTON_MVUP      2

/* Speaker volume down button. */
#define HS_BUTTON_SVDOWN    3

/* Speaker volume up button. */
#define HS_BUTTON_SVUP      4

/* End of HsButton */

/*---------------------------------------------------------------------------
 * HsEvent type
 *
 *     An HsEvent is sent in the callback to the application to notify the app
 *     of any pertinent information, such as a state change or volume change.
 */
typedef U8 HsEvent;

/* The state has changed. */
#define HS_STATE_CHANGE     0

/* The microphone volume has changed. */
#define HS_MICVOL_CHANGE    1

/* The speaker volume has changed. */
#define HS_SPKVOL_CHANGE    2

/* The device has received a PIN request. Respond with HS_EnterPinCode */
#define HS_PIN_REQUEST      3

/* End of HsEvent */

/*-------------------------------------------------------------------------
 * HsVolumeType type
 *
 *     Specifies an audio entity with adjustable volume. HS_ChangeVolume
 *     is used to set the volume.
 */
typedef U8 HsVolumeType;

#define HS_MIC 0x4D /*'M'*/    /* Microphone  */

#define HS_SPK 0x53 /*'S'*/    /* Speaker  */

/* End of HsVolumeType */


/*-------------------------------------------------------------------------
 * HsVolumeChange type
 *
 *     Specifies the kind of volume change required for an audio entity.
 */
typedef U8 HsVolumeChange;

#define HS_UPVOL    0     /* Increased volume (louder) */
#define HS_DOWNVOL  1     /* Decreased volume (quieter) */
#define HS_SETVOL   2     /* Absolute volume (requests volume to be set to
                           * a specific level)
                           */
/* End of HsVolumeChange */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HsCallbackInfo structure
 *
 * A pointer to an HsCallbackInfo struct is sent to the app's callback notifying
 * the app of any state or volume changes.  If the event is HS_STATE_CHANGE,
 * State will contain the new state of the headset. If event is
 * HS_MICVOL_CHANGE, Volume contains the new microphone volume.  If event is
 * HS_SPKVOL_CHANGE, Volume contains the new speaker volume. The status field
 * is valid for HS_MICVOL_CHANGE and HS_SPKVOL_CHANGE events.
 *
 */
typedef struct _HsCallbackInfo {
    HsEvent     event;

    union {
        struct {
            BtStatus    status;
            U8          volume;
        } vol;
        HsState         state;
    } info;
} HsCallbackInfo;

typedef void (*HsCallback)(HsCallbackInfo *);

/*---------------------------------------------------------------------------
 * HsRegistration structure
 *
 * The HsRegistration struct contain a pointer to the app's callback function
 * and is passed into HS_Register().  A structure is used for future expandability.
 *
 */
typedef struct _HsRegistration {
    HsCallback  callback;
} HsRegistration;


/*---------------------------------------------------------------------------
 * HsDeviceContext structure
 *
 * The application must register an HsDeviceContext, before establishing a
 * connection.
 *
 */
typedef struct _HsDeviceContext {

    /* The BD_ADDR of the AG */
    BD_ADDR         addr;

} HsDeviceContext;


#if HS_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 * HsPinCodeReq structure
 *
 * Passed into HS_EnterPinCode(). Contains the PIN, TrustLevel and the BD_ADDR
 * of the gateway.
 *
 */
typedef struct _HsPinCodeReq {
    U8          Pin[MAX_PIN_LEN];
    U8          PinLen;
    U8          TrustLevel;
    BD_ADDR     *bdAddr;
} HsPinCodeReq;
#endif /* HS_SECURITY == XA_ENABLED */

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * HS_Register()
 *
 *     Initializes all necessary data structures, registers the
 *     RfServerChannel, registers the headset's callback handler with the ME,
 *     and adds the appropriate SDP record. If HS_SECURITY is enabled it will
 *     register the required handlers.  If HS_USEDDB is enabled it will search
 *     the DDB and register the first BD_ADDR found as the audio gateway. 
 *     HS_Register() must be called before any other HS_ functions are used.
 *
 * Parameters:
 *     reg - A structure containing a pointer to the callback function.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_BUSY -  The operation failed because the Headset has already
 *                         been initialized.  
 *
 *     BT_STATUS_FAILED - The operation failed because either the RFCOMM channel
 *                        had problems registering, the SDP record wasn't added
 *                        or the security mode didn't set properly. 
 *                        (XA_ERROR_CHECK builds only)
 */
BtStatus HS_Register(const HsRegistration *reg);


/*---------------------------------------------------------------------------
 * HS_Deregister()
 *
 *      Deregisters the RFCOMM server channel, cancels any timers, removes the 
 *      SDP records, and changes the state to HS_IDLE.
 * 
 * Parameters:
 *      NONE
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The operation completed successfully.
 *
 *     BT_STATUS_FAILED -  Headset has not been initialized.
 * 
 *     BT_STATUS_BUSY - The operation failed because a connection is still
 *                      open to the audio gateway.
 */
BtStatus HS_Deregister(void);


/*---------------------------------------------------------------------------
 * HS_RegisterGateway()
 *
 *      Sets Headset.Hdc equal to the HsDeviceContext passed in.  This function
 *      must be called before a connection can be established if HS_USEDDB is
 *      disabled.  
 *
 * Parameters:
 *      Hdc - A HsDeviceContext structure containing the connection
 *            information necessary to establish a connection to the audio gateway
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation completed successfully.
 *
 *      BT_STATUS_FAILED -  Hdc was null or there is an existing connection.
 */
BtStatus HS_RegisterGateway(HsDeviceContext *Hdc);

/*---------------------------------------------------------------------------
 * HS_EnableSecurity()
 *
 *      Turns security on or off (according to the value of the flag)
 *
 * Parameters:
 *      flag - TRUE to enable security, FALSE to disable
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Operation completed successfully.
 *
 *      BT_STATUS_FAILED -  An error occurred while turning on security mode 3.
 */
#if HS_SECURITY == XA_ENABLED
BtStatus HS_EnableSecurity(BOOL flag);
#endif

/*---------------------------------------------------------------------------
 * HS_EnterPinCode()
 *
 *     If a PIN request is received, the app is notified via its callback.  
 *     The app is then responsible for calling this function w/ the PIN which 
 *     a valid pin code.
 *
 * Parameters:
 *     pinParms - pin code parameters (see HsPinCodeReq).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - the pin code was accepted.
 *
 *     BT_STATUS_FAILED - the pin code was not accepted.
 */
#if HS_SECURITY == XA_ENABLED
BtStatus HS_EnterPinCode(HsPinCodeReq *pinParms);
#endif

/*---------------------------------------------------------------------------
 * HS_DoConnect()
 *      Starts the connection process to the audio gateway.  The app's callback
 *      will be notified of a state change to HS_OUTGOING.  An ACL connection
 *      will be established and then the headset will send an SDP query to the 
 *      audio gateway.  If the SDP query is successful the headset will open an
 *      RFCOMM channel to the audio gateway and will then send a +CKPD
 *      command requesting an SCO connection.  The audio gateway, upon
 *      receiving this command should open an SCO connection.  When the SCO
 *      connection is established the headset will call the app's callback with 
 *      a state change (new state will be HS_CONNECTED).  If the connection
 *      fails after this function returns (during the SDP query, RFCOMM
 *      connection, etc) the app's callback will be notified of a state
 *      change to HS_IDLE.
 *
 * Returns:
 *      BT_STATUS_PENDING - The connection has started, the app will be
 *                          notified when the connection is complete via
 *                          the callback.
 *
 *      BT_STATUS_FAILED -  A HsDeviceContext has not been registered, or
 *                          the ACL link connection failed.
 */
BtStatus HS_DoConnect(void);

/*---------------------------------------------------------------------------
 * HS_DoAccept()
 *
 *      The audio gateway has established an ACL connection and opened an
 *      RFCOMM channel to the headset; the audio gateway has also sent a RING
 *      command requesting an SCO connection.  The state of the headset should
 *      be HS_INCOMING and the app has been notified via the callback of the
 *      state change.  If the app does not accept the call, the audio
 *      gateway should eventually disconnect the RFCOMM channel.  If this happens
 *      the state will return to HS_IDLE and the app will be notified via the
 *      callback.
 * 
 *
 * Returns:
 *      BT_STATUS_SUCCESS - Message has been sent, waiting for connection to
 *                          be established.
 *
 *      BT_STATUS_FAILED -  No incoming call, or message was not sent due to 
 *                          RFCOMM problems.
 *
 *      BT_STATUS_BUSY   -  The accept message was not sent because there are
 *                          no BtPackets available.
 */
BtStatus HS_DoAccept(void);

/*---------------------------------------------------------------------------
 * HS_DoDisconnect()
 *
 *      Sends a disconnection request to the audio gateway.
 *
 *      The headset will switch states to HS_DISCONNECTING and send a +CKPD
 *      command requesting the audio gateway to disconnect.  The audio
 *      gateway will disconnect the SCO connection and the RFCOMM channel.
 *      When the RFCOMM channel has been disconnected the headset will
 *      switch states to HS_IDLE and notify the app via the callback.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - All links have been disconnected.
 *
 *      BT_STATUS_PENDING - Disconnect has started, app will be notified
 *                          via the callback will disconnect is complete.
 *
 *      BT_STATUS_BUSY   -  The accept message was not sent because there are
 *                          no BtPackets available.
 */
BtStatus HS_DoDisconnect(void);

/*---------------------------------------------------------------------------
 * HS_ChangeVolume()
 *
 *      Changes the volume of microphone or speaker and sends a message to the
 *      audio gateway notifying it of the volume change.  The application is 
 *      notified (via the callback) of the volume change.
 *
 * Parameters:
 *      type - Either HS_MIC (for Microphone) or HS_SPK (for Speaker)
 *
 *      dir -  Either HS_UPVOL, HS_DOWNVOL, or HS_SETVOL
 *
 *      vol -  If dir is set to HS_SETVOL, then a valid absolute volume level
 *             must be supplied. This must be between 0 and HS_MAX_VOLUME.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The volume change message has been sent to the
 *                          gateway.
 *
 *      BT_STATUS_BUSY -    No RFCOMM packets available.
 *
 *      BT_STATUS_FAILED -  There was an error sending the RFCOMM message.
 *
 *      BT_STATUS_INVALID_PARM - Invalid volume level (XA_ERROR_CHECK only).
 */
BtStatus HS_ChangeVolume(HsVolumeType type, HsVolumeChange dir, U8 vol);

/*---------------------------------------------------------------------------
 * HS_ButtonPressed()
 *
 *      All actions from the interface are mapped to this function.  
 *      Depending on which button is pressed and on the current state of the 
 *      headset, various actions will occur (ie create a connection, send volume
 *      change commands, disconnect, etc.)
 *      This function is not necessary, as all the calls it makes are to other
 *      API functions.  It is included for convenience only.
 *
 * Parameters: 
 *      buttonNum - The ID of the button that was pressed.
 *
 * Returns:
 *      BT_STATUS_FAILED - A volume button was pressed without an open RFCOMM
 *                         channel to the audio gateway.
 *
 *      All valid return values for HS_ChangeVolume(), HS_DoConnect(), and
 *      HS_DoAccept().
 */
BtStatus HS_ButtonPressed(HsButton buttonNum);

/*---------------------------------------------------------------------------
 * HS_GetState()
 *
 *      This function is also for convenience.  The app will be notified of any 
 *      changes to the state through the callback function. The app can call
 *      this function at any time to retrieve the current state of the headset.
 * 
 * Parameters:
 *      NONE
 *
 * Returns:
 *      HsState - The current state of the audio gateway.
 */
HsState HS_GetState(void);

/*---------------------------------------------------------------------------
 * HS_GetRemoteDevice()
 *
 * 
 * Parameters:
 *
 * Returns:
 *      BtRemoteDevice - Current remote device
 */
BtRemoteDevice *HS_GetRemoteDevice(void);

#endif /* BT_HSP_HEADSET */
#endif /* _HEADSET_H_ */
