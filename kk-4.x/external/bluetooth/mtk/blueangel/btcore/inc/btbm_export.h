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

#ifndef _BTBM_EXPORT_H
#define _BTBM_EXPORT_H

/***************
Type Definitions 
****************/
#define BM_LINK_ACCEPT_CMD_AUTO     (0x00)
#define BM_LINK_ACCEPT_CMD_NONE     (0x01)
#define BM_LINK_ACCEPT_CMD_WAIT     (0x02)

#define BT_BM_SSP_PIN_NONE              0x00
#define BT_BM_SSP_PIN_GENERAL_CODE      0x01
#define BT_BM_SSP_PIN_USER_CONFIRM      0x02
#define BT_BM_SSP_PIN_PASSKEY_ENTRY     0x03
#define BT_BM_SSP_PIN_OOB_DATA          0x04
/*************************************************************************
* FUNCTION                                                            
*  BTBMReadLocalBDAddress
* DESCRIPTION                                                           
*   This function is used to read the local bluetooth address.
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMReadLocalBDAddress(void);

/*************************************************************************
* FUNCTION                                                            
*  BTBMReadLocalBDAddress
* DESCRIPTION                                                           
*   This function is used to read the local bluetooth class of device.
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMReadLocalDeviceClass(void);

/*****************************************************************************
 * FUNCTION
 *  BTBMReadLocalDeviceUuidList
 * DESCRIPTION
 *  This function is used to read the local bluetooth list of uuid.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTBMReadLocalDeviceUuidList(void);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapInquiry
* DESCRIPTION                                                           
*   This function is used to inquiry device.
*
* PARAMETERS
*  bt_bm_discovery_req_struct
*  cod:            The device class of device
*  inquiry_length:    The max inquiry time * 1.28 s
*  inquiry_number:  The max inquiry number
*  access_mode:     Device to listen the Generic access channel or Limited access channel
*  discovery_mode: Discovery with name discovery or no name discovery
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapInquiry(bt_bm_discovery_req_struct *btbm_adp_input_msg);


/*************************************************************************
* FUNCTION                                                            
*  BTBMGapInquiryCancel
* DESCRIPTION                                                           
*   This function is used to make the command to cancel inquiry operation.
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapInquiryCancel(void);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapWriteLocalName
* DESCRIPTION                                                           
*   This function is used to make the write the local device name
*
* PARAMETERS
*  bt_bm_write_local_name_req_struct
*      name_len: device name length
*      name: device name 
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapWriteLocalName(bt_bm_write_local_name_req_struct *btbm_adp_input_msg);


/*************************************************************************
* FUNCTION                                                            
*  BTBMGapReadLocalName
* DESCRIPTION                                                           
*   This function is used to Read local device name
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapReadLocalName(void);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapRemoteNameReq
* DESCRIPTION                                                           
*   This function is used to make the read the remote device name
*
* PARAMETERS
*  bt_bm_read_remote_name_req_struct
*      bd_addr: Remote bluetooth address
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapRemoteNameReq(bt_bm_read_remote_name_req_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapWriteScanEnabled
* DESCRIPTION                                                           
*   This function is used to set the Bluetooth Writescan enable mode.
*
* PARAMETERS
*  bt_bm_write_scanenable_mode_req_struct
*       mode; writescan enabled mode
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapWriteScanEnabled(bt_bm_write_scanenable_mode_req_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapReadScanEnabled
* DESCRIPTION                                                           
*   This function is used to read the visible mode setting.
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapReadScanEnabled(void);

/*************************************************************************
* FUNCTION                                                            
*    BTBMWriteLocalDeviceClass
* DESCRIPTION                                                           
*    The function is used to write local cod   
*
* PARAMETERS
*  btbm_adp_input_msg: 
*              cod : The input message to write the cod.
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMWriteLocalDeviceClass(bt_bm_write_local_cod_req_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapSetAuthenticationMode
* DESCRIPTION                                                           
*   This function is used to set the device into authentication enable/disable mode
*
* PARAMETERS
*  bt_bm_write_authentication_mode_req_struct
*      mode: The authentication enable/disable mode setting
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapSetAuthenticationMode(BOOL reported, bt_bm_write_authentication_mode_req_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapWritePINCode
* DESCRIPTION                                                           
*   This function is used to response the PIN Code request when the user press the pin code
*
* PARAMETERS
*  bt_bm_pin_code_rsp_struct
*      bd_addr: Remote bluetooth address
*      pin_len: The length of pin code
*      pin_code: The PIN CODE
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapWritePINCode(bt_bm_pin_code_rsp_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapBondingDevice
* DESCRIPTION                                                           
*   This function is used to make the bond with remote device
*
* PARAMETERS
*  bt_bm_bonding_req_struct
*      bd_addr: Remote bluetooth address
*      sdap_len: The sdap uuid number, if there is no sdap operation need to do,set sdap_len =0
*      sdap_uuid: The UUID list of the SDAP operation
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapBondingDevice(bt_bm_bonding_req_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMSdapSearchAttributeService
* DESCRIPTION                                                           
*   This function is used to search remote device supported services and attribute
*
* PARAMETERS
*  bt_bm_search_attribute_req_struct
*      bd_addr: Remote bluetooth address
*      sdap_uuid: The sdap service uuid
*      attribute_id: The attribute id
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMSdapSearchAttributeService(bt_bm_search_attribute_req_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMSdapSearchallService
* DESCRIPTION                                                           
*   This function is used to search remote device supported services
*
* PARAMETERS
*  bt_bm_service_search_req_struct
*      bd_addr: Remote bluetooth address
*      sdap_len: The sdap uuid number, the sdap_len must >0
*      sdap_uuid: The UUID list of the SDAP operation
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMSdapSearchallService(bt_bm_service_search_req_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapDeleteTrustDevice
* DESCRIPTION                                                           
*  This function is used to delete the link key of the specified device
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapDeleteTrustDevice(bt_bm_delete_trust_req_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapDeleteTrustAllDevice
* DESCRIPTION                                                           
*   This function is used to delete all the link key
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapDeleteTrustAllDevice(void);

/*************************************************************************
* FUNCTION                                                            
*  BTBMAllowAllLink
* DESCRIPTION                                                           
*   The function is used by MMI power on procedure to ready to accept new connection.
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMAllowAllLink(void);

/*************************************************************************
* FUNCTION                                                            
*  BTBMDisconnectAllLink
* DESCRIPTION                                                           
*   The function provide the MMI to disconnect all active ACL link.
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMDisconnectAllLink(void);

/*************************************************************************
* FUNCTION                                                            
*  BTBMACLLinkAcceptRsp
* DESCRIPTION                                                           
*  After the connect ind event to BTstack, the btstack will send a confirm message to MMI, MMI shall 
*  response the accept or not for that connection.
*
* PARAMETERS
*  bt_bm_link_connect_accept_rsp_struct
*       result: Accept that connection or not
*       bd_addr: the Bluetooth address
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMACLLinkAcceptRsp(bt_bm_link_connect_accept_rsp_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMACLLinkAcceptMode
* DESCRIPTION                                                           
*  The MMI will set the bt stack can accept connection automatically or not.
*
* PARAMETERS
*  mode: The MMI to BT SAP defined auto accept value or not.
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMACLLinkAcceptMode(U8 mode);

#if defined (__BT_2_1_SIMPLE_PAIRING__)
void BTBMGapPasskeyEntryRsp(bt_bm_security_passkey_entry_rsp_struct *btbm_adp_input_msg);
void BTBMGapPasskeyNotifyRsp(bt_bm_security_passkey_notify_rsp_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapWritePINCode
* DESCRIPTION                                                           
*   This function is used to response the PIN Code request when the user press the pin code
*
* PARAMETERS
*  bt_bm_security_user_confirm_rsp_struct
*      bd_addr: Remote bluetooth address
*      accept: The user accept the SSP or not
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapWriteUserConfirm(bt_bm_security_user_confirm_rsp_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMSSPKeyPressRsp
* DESCRIPTION                                                           
*  The MMI will response that it has received the Key Press Indication.
*  We can send the next key press indication to MMI.
*  This can aviod too many messages allocated and send to MMI at the same time.
*
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMSSPKeyPressRsp(void);

/*************************************************************************
* FUNCTION                                                            
*  BTBMSSPKeyPressAbort
* DESCRIPTION                                                           
*  The MMI will request to disconnect the ACL
*
* PARAMETERS
*  bt_bm_security_passkey_notify_cancel_struct the bluetooth address
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMSSPKeyPressAbort(bt_bm_security_passkey_notify_cancel_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapOOBDataRsp
* DESCRIPTION                                                           
*  
*
* PARAMETERS
*  
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapOOBDataRsp(bt_bm_security_oob_data_rsp_struct *btbm_adp_input_msg);
#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 

void BTBMBlockDeviceForceDisconnect(bt_bm_block_active_link_req_struct *btbm_adp_input_msg);
BtStatus BTBMGapSaveWriteScanModeValue(bt_bm_write_scanenable_mode_req_struct *btbm_adp_input_msg);
void BTBMReturnLinkState(bt_bm_get_link_state_req_struct *ptr);
void BTBMReturnScatternetState(void);
void BTBMGetLinkKey(btbm_bd_addr_t *bd_addr);
void BTBMGapRemoteNameCancelReq(void);
void BTBMGapBondingCancel(bt_bm_bonding_cancel_req_struct *btbm_adp_input_msg);
void BTBMSdapSearchAllCancel(bt_bm_service_search_cancel_req_struct *btbm_adp_input_msg);

#endif /* _BTBM_EXPORT_H */

