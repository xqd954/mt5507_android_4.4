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

#ifndef BTBM_INT_H
#define BTBM_INT_H
#include "btbm.h"
/*************************************************************************
* FUNCTION                                                            
*  DummyLinkCallback
* DESCRIPTION                                                           
*   This function is register when we connect with other device.
*   This is a dummy function which will be called when we call the ME_CreateLink
*
* PARAMETERS
*      Event : Event of ME layer
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void DummyLinkCallback(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmAddLinkCallback
* DESCRIPTION                                                           
*  for Btstack internal used. To register the callback function of that link
*
* PARAMETERS
*  link: The callback function register to this link
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmAddLinkCallback(BtRemoteDevice *link);

/*************************************************************************
* FUNCTION                                                            
*  btbmRemoveLinkCallback
* DESCRIPTION                                                           
*  for Btstack internal used. To deregister the callback function of that link
*
* PARAMETERS
*  link: The callback function register to this link
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmRemoveLinkCallback(BtRemoteDevice *link);

/*************************************************************************
* FUNCTION                                                            
*  btbmStartSpecialSlaveProcedure
* DESCRIPTION                                                           
*  Special procedure for reset the role switch count and sniff count for BT chip.
*
* PARAMETERS
*  None
* RETURNS
*  None
* local AFFECTED
*   
*************************************************************************/
void btbmStartSpecialSlaveProcedure(void);

/*************************************************************************
* FUNCTION                                                            
*  BtbmAllocateFreeDevices
* DESCRIPTION                                                           
*   The function to use to allocate a new BtDeviceContext in Database
* PARAMETERS
*  bdAddr, the Bluetooth address of the BtDeviceContext
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
BtStatus BtbmAllocateFreeDevices(BD_ADDR bdAddr, BtDeviceContext **NewDevice);

/*************************************************************************
* FUNCTION                                                            
*  btbmPutLinkAsMaster
* DESCRIPTION                                                           
*  The function is used for inquiry procedure to force all ACL link as one piconet. We are the master state.
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void btbmPutLinkAsMaster(void);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerLinkCreateCancel
* DESCRIPTION                                                           
*  To handle the Create Connection Cancel Completed event
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerLinkCreateCancel(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerConnectInd
* DESCRIPTION                                                           
*  To handle the connect indication event
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerConnectInd(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerConnectCfm
* DESCRIPTION                                                           
*  To handle the connect confirm event
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerConnectCfm(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerDisconnectInd
* DESCRIPTION                                                           
*  To handle the Disconnect ind event
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerDisconnectInd(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandleModeCfm
* DESCRIPTION                                                           
*  To handle the enter/exit XXX mode completed event
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandleModeCfm(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandleEncryptionCfm
* DESCRIPTION                                                           
*  To handle the set encryption mode completed event
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandleEncryptionCfm(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandleRoleSwitchCfm
* DESCRIPTION                                                           
*  To handle the Mode change event
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandleRoleSwitchCfm(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerAuthenticatedCfm
* DESCRIPTION                                                           
*  To handle the Authentication link completed event
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerAuthenticatedCfm(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerLinkkeyDeleted
* DESCRIPTION                                                           
*  To handle the link key is removed by host
*
* PARAMETERS
*  Event: The event reported by ME layer whihc contains BD_ADDR
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerLinkkeyDeleted(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandleLinkConnectReq
* DESCRIPTION                                                           
*  Handle remote link request.
*
* PARAMETERS
*  remDev : remote device which initiate connection request.
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandleLinkConnectReq(BtRemoteDevice *remDev);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandleReportLinkState
* DESCRIPTION                                                           
*  Report the current active connection and dump the connection status in catcher log.
*
* PARAMETERS
*  None
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandleReportLinkState(BtRemoteDevice *remDev);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandleCheckPiconetState
* DESCRIPTION                                                           
*  Dump the connection status in catcher log.
*
* PARAMETERS
*  None
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
U8 btbmHandleCheckPiconetState(void);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerCheckAllLinkPowerOffStatus
* DESCRIPTION                                                           
*  To check all link in disconnected mode, if not, force to disconnect the ACL link
* PARAMETERS
*  None
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerCheckAllLinkPowerOffStatus(void);

#if defined (__BT_2_1_SIMPLE_PAIRING__) || defined (__BT_2_1_BLOCK_DEVICE__)
void btbmHandleForceDisconnectLink(BD_ADDR *link);
#endif 

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerInquiryResult
* DESCRIPTION                                                           
*  To handle the inquiry result
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void btbmHandlerInquiryResult(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerInquiryComplete
* DESCRIPTION                                                           
*  To handle the inquiry completed event
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void btbmHandlerInquiryComplete(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerInquiryCancel
* DESCRIPTION                                                           
*  To handle the inquiry cancel completed event
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void btbmHandlerInquiryCancel(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerMultipleNameQuery
* DESCRIPTION                                                           
*   The handler to deal with the btbmMultipleNameQuery operation result (ask name during inquiry procedure)
* PARAMETERS
*      Event : Event of ME layer
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void btbmHandlerMultipleNameQuery(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerRemoteNameQueryAtNoAuthEnabled
* DESCRIPTION                                                           
*   The handler to deal with the btbmRemoteNameRequestAtNoAuthEnabled operation result.
*   Only under the condition: "(for each incoming acl link) no authentication procedure is enabled on both device and our device is connected by remote firs",
*   it is necessary to use the function  btbmRemoteNameRequestAtNoAuthEnabled to ask the remote device's name  
* PARAMETERS
*      Event : Event of ME layer
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void btbmHandlerRemoteNameQueryAtNoAuthEnabled(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerPinCodeAskNameQuery
* DESCRIPTION                                                           
*   The handler to deal with PIN code Name Request operation result.
* PARAMETERS
*      Event : Event of ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerPinCodeAskNameQuery(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerSingleNameQuery
* DESCRIPTION                                                           
*   The handler to deal with the btbmRunSingleNameQuery(Normal Name Request) result.
* PARAMETERS
*      Event : Event of ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerSingleNameQuery(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerInquiryLoop
* DESCRIPTION                                                           
*   This is the main body for inquiry procedure. This may enter due to MMI's inquiry request or timeout.
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void btbmHandlerInquiryLoop(void);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerInquiryIntervalProcedure
* DESCRIPTION                                                           
*   During the inquiry procedure, we may not preceed the inquiry in full speed. Sometimes, we need to 
*   start a timer for WIFI or BT connection to keep the connection alive. This is used to open a interval
*   to keep other connections alive.
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void btbmHandlerInquiryIntervalProcedure(void);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandleDiscoveryComplete
* DESCRIPTION                                                           
*  To handle the Discovery Complete and report to MMI
*
* PARAMETERS
*  result: Discovery Completed result
*  total_number: Discovery searched number
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandleDiscoveryComplete(U8 result, U8 total_number);

#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
void btbmUtilParseInquiryEirResponse(BtDeviceContext *bdc, U8 *eir_reponse);
#endif 

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerHFPInquiryBlackList
* DESCRIPTION                                                           
*  Check the device of HFP BlackList is connected.
*
* PARAMETERS
*  None
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerHFPInquiryBlackList(void);

/*************************************************************************
* FUNCTION                                                            
*  btbmMultipleNameQuery
* DESCRIPTION                                                           
*   The function to do the Name Discovery (One of the Device Descovery Procedure)
*   To do the ME_GetRemoteDeviceName operation
* PARAMETERS
*  None
* RETURNS
*  BtStatus status of result
*           BT_STATUS_SUCCESS, No device needed to do the name discovery
*           BT_STATUS_PENDING,    No device needed to do the name discovery
* GLOBALS AFFECTED
*   
*************************************************************************/
BtStatus btbmMultipleNameQuery(void);


/*************************************************************************
* FUNCTION                                                            
*  btbmRemoteNameRequestAtNoAuthEnabled
* DESCRIPTION                                                           
*   The function to do the Single name request when no authentication procedure is enabled and our device is connected by remote device first
* PARAMETERS
*  bdc, pointer points to the address of remote device
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmRemoteNameRequestAtNoAuthEnabled(BtDeviceContext *bdc);

/*************************************************************************
* FUNCTION                                                            
*  btbmUtilConnectedDeviceNameEntryFree
* DESCRIPTION                                                           
*   Check if there is any free stored name entry or existing entry for the bdAddr.
* PARAMETERS
*   bdAddr: The address for the stored name entry
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmUtilConnectedDeviceNameEntryFree(BD_ADDR *bdAddr);

/*************************************************************************
* FUNCTION                                                            
*  btbmUtilConnectedDeviceNameEntryLookUp
* DESCRIPTION                                                           
*   The function is used to find out the map to BM name setting struct by address.
* PARAMETERS
*   bdAddr: The input Bluetooth address
* RETURNS
*  BTBMRemoteNameSetting_T The stored name sturct for the address or none.
* Local AFFECTED
*   
*************************************************************************/
BTBMRemoteNameSetting_T *btbmUtilConnectedDeviceNameEntryLookUp(BD_ADDR *bdAddr);

/*************************************************************************
* FUNCTION                                                            
*  btbmCheckPendingforNameToken2
* DESCRIPTION                                                           
*   The function is used when the previous name req is completed, we will check if there is another name requ.
*   shall be preceeded.
* PARAMETERS
*   None
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmCheckPendingforNameToken2(void);

/*************************************************************************
* FUNCTION                                                            
*  btbmNameReqAtNoAuthEnabled
* DESCRIPTION                                                           
*   The utility function to do the name request operation (for each incoming link) under no authentication procedure enabled and connected by remote device first
* PARAMETERS
*   bdc, pointer points to the address of remote device
*  callback, the callback function when the name request operation is finished
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
BtStatus btbmNameReqAtNoAuthEnabled(BtDeviceContext *bdc, BtCallBack callback);

/*************************************************************************
* FUNCTION                                                            
*  btbmUtilConnectedDeviceNameEntryAllocate
* DESCRIPTION                                                           
*   The function is used to find out the connected device map to BM name setting struct.
* PARAMETERS
*   remDev: Active ACL connected device
* RETURNS
*  BTBMRemoteNameSetting_T The stored name sturct for the ACL link.
* Local AFFECTED
*   
*************************************************************************/
BTBMRemoteNameSetting_T *btbmUtilConnectedDeviceNameEntryAllocate(BtRemoteDevice *remDev);

/*************************************************************************
* FUNCTION                                                            
*  btbmNameReq
* DESCRIPTION                                                           
*   The utility function to do the name request operation
* PARAMETERS
*   bdc, pointer points to the address of remote device
*  callback, the callback function when the name request operation is finished
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
BtStatus btbmNameReq(MeCommandToken *token, BtDeviceContext *bdc, BtCallBack callback);

/*************************************************************************
* FUNCTION                                                            
*  btbmUtilCheckInquiryTime
* DESCRIPTION                                                           
*  The function is used to check the inquiry procedure timeout or not.
*
* PARAMETERS
*   None
* RETURNS
*   The time interval between the start inquiry time and current time.
* GLOBALS AFFECTED
*   
*************************************************************************/
U32 btbmUtilCheckInquiryTime(void);

/*************************************************************************
* FUNCTION                                                            
*  btbmInquiryIntervalTimeout
* DESCRIPTION                                                           
*  The inquiry interval timeout handler function.
*
* PARAMETERS
*  Timer information
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmInquiryIntervalTimeout(EvmTimer *Timer);

/*************************************************************************
* FUNCTION                                                            
*  btbmInquiryPreprocessTimeout
* DESCRIPTION                                                           
*  The inquiry timeout of special slave procedure handler function.
*
* PARAMETERS
*  Timer information
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmInquiryPreprocessTimeout(EvmTimer *Timer);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerPinCodeRequest
* DESCRIPTION                                                           
*  To handle the Pin code request event
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerPinCodeRequest(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerPairingComplete
* DESCRIPTION                                                           
*  To handle the Pin Code completed
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerPairingComplete(const BtEvent *Event);

#if defined (__BT_2_1_SIMPLE_PAIRING__)
/*************************************************************************
* FUNCTION                                                            
*  btbmHandleIoCapabilityRequest
* DESCRIPTION                                                           
*  Handle the IoCapability Request from remote peer.
*
* PARAMETERS
*  BtRemoteDevice *remDev remote device
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandleIoCapabilityRequest(BtRemoteDevice *remDev);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandleIoCapabilityResponse
* DESCRIPTION                                                           
*  Handle the IoCapability response from remote peer.
*
* PARAMETERS
*  BtEvent *event the Event reported from ME layer.
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandleIoCapabilityResponse(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandleUserConfirmRequest
* DESCRIPTION                                                           
*  Handle the User confirm Request.
*
* PARAMETERS
*  BtEvent *event the Event reported from ME layer.
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandleUserConfirmRequest(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlePasskeyEntryRequest
* DESCRIPTION                                                           
*  Handle the Passkey Entry Request.
*
* PARAMETERS
*  BtEvent *event the Event reported from ME layer.
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlePasskeyEntryNotification(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlePasskeyPressIndication
* DESCRIPTION                                                           
*  Handle the Key Press Indication.
*
* PARAMETERS
*  BtEvent *event the Event reported from ME layer.
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlePasskeyPressIndication(const BtEvent *Event);
void btbmHandleOobDataRequest(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerSimplePairingComplete
* DESCRIPTION                                                           
*  To handle the SimplePairing completed
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerSimplePairingComplete(const BtEvent *Event);
#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlePinReqTimeout
* DESCRIPTION                                                           
*  When the 200 ms timeout from the name request complete event, we will report the Pin code request event
*  to MMI.
* PARAMETERS
*  None
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlePinReqTimeout(EvmTimer *Timer);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlePinReqStartOperation
* DESCRIPTION                                                           
*  When we receive the Name request completed event during pairing or bonding procedure, 
*  we shall start a 200ms timer to report the PIN code request event to MMI
*
* PARAMETERS
*  bd_addr: The remote pairing or bonding address
*     name_len: The remote pairing or bonding name length
*     name: The remote pairing or bonding name
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlePinReqStartOperation(btbm_bd_addr_t bd_addr, U8 name_len, U8 *name);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlePinReqCancelOperation
* DESCRIPTION                                                           
*  To Reset the PinRequest Timer.
* PARAMETERS
*  None
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlePinReqCancelOperation(void);

/*************************************************************************
* FUNCTION                                                            
*  btbmUtilCheckBondingState
* DESCRIPTION                                                           
*   The handle the current bonding procedure.
*
* PARAMETERS
*   bonding_state: The current bonding state;
* RETURNS
*   void
* GLOBALS AFFECTED
*   
*************************************************************************/
void btbmUtilCheckBondingState(U8 bonding_state, BtErrorCode errCode);

/*************************************************************************
* FUNCTION                                                            
*  btbmUtilCheckPairingState
* DESCRIPTION                                                           
*   The handle the current pairing procedure.
*
* PARAMETERS
*   remote_addr: The remote Bluetooth address
*   pairing_state: The current pairing statel
* RETURNS
*   void
* GLOBALS AFFECTED
*   
*************************************************************************/
void btbmUtilCheckPairingState(BD_ADDR *remote_addr, U8 pairing_state, BtErrorCode errCode);

/*************************************************************************
* FUNCTION                                                            
*  btbmUtilPairingProcedureClear
* DESCRIPTION                                                           
*   To clear the current pairing or bonding state
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void btbmUtilPairingProcedureClear(void);


/*************************************************************************
* FUNCTION                                                            
*  btbmCheckBondingState
* DESCRIPTION                                                           
*  Check the current state in Bonding state.
*
* PARAMETERS
*  None
* RETURNS
*  TRUE in bonding mode, FALSE is not in bonding mode
* Global AFFECTED
*   
*************************************************************************/
BOOL btbmCheckBondingState(void);

/*************************************************************************
* FUNCTION                                                            
*  btbmCheckCreateLinkKeyState
* DESCRIPTION                                                           
*  Check the current state in Bonding or Pairing state.
*
* PARAMETERS
*  None
* RETURNS
*  TRUE in bonding mode, FALSE is not in CreateLinkKey state
* Global AFFECTED
*   
*************************************************************************/
BOOL btbmCheckCreateLinkKeyState(void);

/*************************************************************************
* FUNCTION                                                            
*  btbmCheckBondingStateWhenDisconnected
* DESCRIPTION                                                           
*  Check the current state of a special device in Bonding state when the L2CAP try to disconnect ACL link.
*
* PARAMETERS
*  addr1: The checked device address
* RETURNS
*  TRUE in bonding mode, FALSE is not in bonding mode
* Global AFFECTED
*   
*************************************************************************/
BOOL btbmCheckBondingStateWhenDisconnected(U8 *addr1);

#if defined (__BT_2_1_SIMPLE_PAIRING__)
U8 btbmCalculateSSPLinkKeyStrength(void);
#endif 

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerAccessModeChange
* DESCRIPTION                                                           
*  To handle the Write Scan enabled completed 
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerAccessModeChange(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerChangeLocalName
* DESCRIPTION                                                           
*  To handle the write local device name completed 
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerChangeLocalName(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandlerSecurityChange
* DESCRIPTION                                                           
*  To handle the write authentication mode completed event
*
* PARAMETERS
*  Event: The event reported by ME layer
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmHandlerSecurityChange(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmHandleGetSecurityLevel
* DESCRIPTION                                                           
*  To Get the current BT security level
* PARAMETERS
*  None
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
btbm_security_level_mode btbmHandleGetSecurityLevel(void);

void btbmStartServiceSearch(void);
void btbmSendLinkkeyDelInd(U8 result, const BD_ADDR *BdAddr);
void btbmSendRemoteNameInd(btbm_bd_addr_t bd_addr, U8 name_len, U8 *name);

#ifdef __BT_4_0_BLE__
void btbmGattHandleRemNameReqTimeoutCompleted(EvmTimer *timer);
#endif

/***************************
External function prototypes
****************************/
extern void ConnMgrSnifferTimeOutImmediately(BtRemoteDevice *link);
extern void BT_State_Report2WLAN(U32 value, U32 mask);
extern U32 WLAN_State_Report2BT(void);
extern void MeWriteSupervisionTimeout(BtRemoteDevice *remDev);

#ifdef __BT_SIM_PROFILE__
extern BOOL SimApServerInCloseState(BtRemoteDevice *remdev, U8 *simsrv_state);
#endif 

void btbmGAPSetCTSServerSecurityLow(U8 value);



#endif /* BTBM_INT_H */ 

