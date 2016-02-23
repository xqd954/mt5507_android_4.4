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

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
 *
 * Filename:
 * ---------
 * bt_adp_bm_event.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions which provide the service to be the adaption layer 
 *   between JAVA and MMI
 *
 * Author:
 * -------
 * Dlight Ting(mtk01239)
 *      Create 2006/2/9
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/

#include "stdio.h"      /* Basic C I/O functions */
#include "bt_common.h"
#include "bt_feature.h"
#include "bt.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_struct.h"
#include "btbm_adp.h"
#include "sdap.h"
#include "btbm_export.h"
#include "bt_maui_btbm.h"
#if defined (__BT_2_1_BLOCK_DEVICE__)
#include "btbmblocklist.h"
#endif 

/***************
Internal Globals 
****************/

/***************
External Globals 
****************/

/***************************
Internal function prototypes
****************************/

void bm_app_callback(BMCallbackParms *Parms);

/*********************
Function Definitions 
**********************/


/*****************************************************************************
 * FUNCTION
 *  bm_adp_event_init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bm_adp_event_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bm_adp_init();
    BTBM_Register(bm_app_callback);
}


/*****************************************************************************
 * FUNCTION
 *  bm_app_callback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Parms       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void bm_app_callback(BMCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (Parms->event)
    {
        case BTBM_EVENT_READ_SCAN_MODE_RESULT:
            /* BTBM Event of Read visible mode request */
            BTBMAdpGetReadVisibleModeCfm(Parms->p.ReadScanModeResult);
            break;
        case BTBM_EVENT_READ_CLASS_OF_DEVICE_RESULT:
            BTBMAdpReadLocalDeviceClassCfm(Parms->p.ReadCodResult.result, Parms->p.ReadCodResult.cod);
            break;
        case BTBM_EVENT_READ_UUID_LIST_RESULT:
            BTBMAdpReadLocalDeviceUuidListCfm(Parms->p.ReadLocalUuidListResult.result, Parms->p.ReadLocalUuidListResult.uuid_list);			
            break;
        case BTBM_EVENT_WRITE_LOCAl_CLASS_OF_DEVICE_RESULT:
            BTBMAdpWriteLocalDeviceClassCfm(Parms->p.result);
            break;
        case BTBM_EVENT_READ_LOCAL_DEVICE_NAME_RESULT:
            BTBMAdpReadLocalNameCfm(Parms->p.ReadLocalNameResult.name, Parms->p.ReadLocalNameResult.length);
            break;
        case BTBM_EVENT_READ_LOCAL_BLUETOOTH_ADDR_RESULT:
            BTBMAdpReadLocalBDAddressCfm(Parms->p.ReadLocalAddrResult.result, Parms->p.ReadLocalAddrResult.bd_addr);
            break;
        case BTBM_EVENT_WRITE_SCAN_MODE_RESULT:
            BTBMAdpWriteScanEnabledCfm(Parms->p.result);
            break;
        case BTBM_EVENT_DISCOVERY_COMPLETE_RESULT:
            BTBMAdpGapDiscoveryCompCfm(
                Parms->p.DiscoveryCompleteResult.result,
                Parms->p.DiscoveryCompleteResult.total_no);
            break;
        case BTBM_EVENT_DISCOVERY_CANCEL_COMPLETE_RESULT:
            BTBMAdpGapDiscoveryCancelCfm(
                Parms->p.DiscoveryCancelCompleteResult.result,
                Parms->p.DiscoveryCancelCompleteResult.total_no);
            break;
        case BTBM_EVENT_DISCOVERY_DEVICE_RESULT:
            BTBMGapDiscoveryResultCfm(Parms->p.DiscoveryDeviceResult.is_update,
                                  Parms->p.DiscoveryDeviceResult.bd_addr,
#ifdef __BT_4_0_BLE__
                                  Parms->p.DiscoveryDeviceResult.dev_type,
#endif
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
                                  Parms->p.DiscoveryDeviceResult.rssi, 
                                  Parms->p.DiscoveryDeviceResult.eir_supported, 
                                  Parms->p.DiscoveryDeviceResult.uuid_list_supported, 
                                  Parms->p.DiscoveryDeviceResult.servlist_completed,
                                  Parms->p.DiscoveryDeviceResult.uuid_list,
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
                                  Parms->p.DiscoveryDeviceResult.cod, 
                                  Parms->p.DiscoveryDeviceResult.name_len, 
                                  Parms->p.DiscoveryDeviceResult.name);
            break;
        case BTBM_EVENT_WRITE_LOCAL_DEVICE_NAME_RESULT:
            BTBMAdpWriteLocalNameCfm(Parms->p.result);
            break;
        case BTBM_EVENT_SET_AUTHENTICATION_RESULT:
            BTBMAdpGapSetAuthenticationCfm(Parms->p.result);
            break;
        case BTBM_EVENT_DELETE_ONE_TRUST_DEVICE_RESULT:
            BTBMAdpDeleteTrustCfm(
                Parms->p.DeleteOneTrustDeviceResult.result,
                Parms->p.DeleteOneTrustDeviceResult.bd_addr);
            break;
        case BTBM_EVENT_DELETE_ALL_TRUST_DEVICE_RESULT:
            BTBMAdpDeleteTrustAllCfm(Parms->p.result);
            break;
        case BTBM_EVENT_ALLOW_ACL_LINK_RESULT:            
            BTBMAdpAllowAclLinkResult();
            break;
        case BTBM_EVENT_DISALLOW_ACL_LINK_RESULT:
            BTBMAdpDisallowAclLinkResult();
            break;
        case BTBM_EVENT_BLOCKLIST_DISCONNECT_RESULT:
            /* Parms->p.bd_addr is the address */
            BTBMAdpBlockDeviceDisconnectLinkCfm();
            break;
        case BTBM_EVENT_BLOCKLIST_UPDATED_RESULT:
            BTBMAdpBlockListUpdateCfm();
            break;
        case BTBM_EVENT_BONDING_DEVICE_RESULT:
            BTBMAdpBondingDeviceCfm(
                Parms->p.BondingDeviceCompleteResult.result,
                Parms->p.BondingDeviceCompleteResult.bd_addr);
            break;
        case BTBM_EVENT_BONDING_DEVICE_RESULT_INDICATION:
            BTBMAdpLinkKeyGeneratedInd(Parms->p.LinkKeyGereratedResult.result,
                                       Parms->p.LinkKeyGereratedResult.type,
                                       Parms->p.LinkKeyGereratedResult.bd_addr,
#ifdef __BT_4_0_BLE__
                                       Parms->p.LinkKeyGereratedResult.device_type,
#endif
                                       Parms->p.LinkKeyGereratedResult.cod,
                                       Parms->p.LinkKeyGereratedResult.key_type,
                                       Parms->p.LinkKeyGereratedResult.key);
            break;
        case BTBM_EVENT_BONDING_CANCEL_RESULT:
            BTBMAdpBondingCancelCfm(Parms->p.BondCancelResult.result, Parms->p.BondCancelResult.bd_addr);
            break;
        case BTBM_EVENT_REPORT_ACTIVE_LINK_NO: 
            BTBMAdpReportLinkState(Parms->p.ReportLinkStateResult.ActiveLinkNumber, 
                                   Parms->p.ReportLinkStateResult.bd_addr);
            break;
        case BTBM_EVENT_PINCODE_REQUEST_INDICATION:
            BTBMAdpPinCodeRequiredInd(Parms->p.PinCodeRequestIndication.pin_code_16_digit_required,
                                      Parms->p.PinCodeRequestIndication.bd_addr,
                                      Parms->p.PinCodeRequestIndication.name_len,                                      
                                      Parms->p.PinCodeRequestIndication.name,
                                      Parms->p.PinCodeRequestIndication.cod);
            break;
        case BTBM_EVENT_READ_REMOTE_DEVICE_NAME_RESULT:
            BTBMAdpReadRemoteNameCfm(Parms->p.ReadRemoteNameResult.result,
                                     Parms->p.ReadRemoteNameResult.bd_addr,
                                     Parms->p.ReadRemoteNameResult.name_len,
                                     Parms->p.ReadRemoteNameResult.name,
                                     Parms->p.ReadRemoteNameResult.cod);
            break;
        case BTBM_EVENT_REMOTE_DEVICE_NAME_INDICATION:
            BTBMAdpRemoteNameInd(Parms->p.RemoteNameIndication.bd_addr,
                                     Parms->p.RemoteNameIndication.name_len,
                                     Parms->p.RemoteNameIndication.name);
            break;
        case BTBM_EVENT_READ_REMOTE_DEVICE_NAME_CANCEL_RESULT:
            BTBMAdpReadRemoteNameCancelCfm(Parms->p.ReadRemoteNameCancelResult.result);
            break;
        case BTBM_EVENT_SSP_SECURITY_PAIRING_INDICATION:
            BTBMAdpSSPNumericInd(Parms->p.SSPSecurityPairingIndication.bd_addr,
                                 Parms->p.SSPSecurityPairingIndication.ssp_type,
                                 Parms->p.SSPSecurityPairingIndication.numericValue,
                                 Parms->p.SSPSecurityPairingIndication.name_len,
                                 Parms->p.SSPSecurityPairingIndication.name,
                                 Parms->p.SSPSecurityPairingIndication.displayNumeric,
                                 Parms->p.SSPSecurityPairingIndication.cod);
            break;
        case BTBM_EVENT_SSP_KEY_PRESS_INDICATION:
            BTBMAdpSSPPasskeyPressInd(Parms->p.SSPKeyPressIndication.bd_addr,
                                      Parms->p.SSPKeyPressIndication.event);
            break;
        case BTBM_EVENT_LINK_ACCEPT_CONFIRM_INDICATION:
            BTBMAdpConnectAcceptInd(Parms->p.bd_addr);
            break;
        case BTBM_EVENT_REPORT_NETWORK_STATUS:
            BTBMAdpReportNetworkState(Parms->p.ReportNetworkStatus.result,
                                      Parms->p.ReportNetworkStatus.is_master,  
                                      Parms->p.ReportNetworkStatus.is_authenticated,  
                                      Parms->p.ReportNetworkStatus.is_encrypted);  
            break;
        case BTBM_EVENT_REPORT_RSSI_VALUE:
            BTBMAdpReportReadRSSIResult(Parms->p.ReportRSSIValue.error_code,
                                        Parms->p.ReportRSSIValue.rssi_value,
                                        Parms->p.ReportRSSIValue.bd_addr.lap,
                                        Parms->p.ReportRSSIValue.bd_addr.uap,
                                        Parms->p.ReportRSSIValue.bd_addr.nap);
            break;
        case BTBM_EVENT_REPORT_SCATTERNET_STATUS:
            BTBMAdpReportScatternetState(Parms->p.ReportScatternetStatus.piconet,
                                         Parms->p.ReportScatternetStatus.scatternet,  
                                         Parms->p.ReportScatternetStatus.sco,  
                                         Parms->p.ReportScatternetStatus.esco);  
            break;
        case BTBM_EVENT_SERVICE_SEARCH_RESULT:
            BTBMAdpServiceSearchResult(Parms->p.ReportServiceSearchResult.uuid, 
                                       Parms->p.ReportServiceSearchResult.bd_addr);
            break;
        case BTBM_EVENT_SERVICE_SEARCH_COMPLETED:
            BTBMAdpServiceSearchComplete(Parms->p.ReportServiceSearchComplete.result, 
                                       Parms->p.ReportServiceSearchComplete.bd_addr,
                                       Parms->p.ReportServiceSearchComplete.no);

            break;
        case BTBM_EVENT_SERVICE_SEARCH_CANCELLED:
            BTBMAdpServiceSearchCancel(Parms->p.ReportServiceSearchComplete.result, 
                                       Parms->p.ReportServiceSearchComplete.bd_addr,
                                       Parms->p.ReportServiceSearchComplete.no);
            break;
        case BTBM_EVENT_ATTRIBUTE_SEARCH_RESULT:
            BTBMAdpAttributeSearchResult(Parms->p.ReportAttributeSearchResult.len,
                                         Parms->p.ReportAttributeSearchResult.data,
                                         Parms->p.ReportAttributeSearchResult.bd_addr);
            break;
        case BTBM_EVENT_ATTRIBUTE_SEARCH_COMPLETED:
#if defined(BTMTK_ON_LINUX)
            BTBMAdpAttributeSearchComplete(Parms->p.ReportAttributeSearchComplete.result, 
                                           Parms->p.ReportAttributeSearchComplete.bd_addr,
                                           Parms->p.ReportAttributeSearchComplete.len,
                                           Parms->p.ReportAttributeSearchComplete.data);
#else
            BTBMAdpAttributeSearchComplete(Parms->p.ReportAttributeSearchComplete.result, 
                                           Parms->p.ReportAttributeSearchComplete.bd_addr);
#endif
            break;
        case BTBM_EVENT_GET_LINK_KEY:
            BTBMAdpGetLinkKeyResult(Parms->p.ReportLinkKeyResult.result,
                                       Parms->p.ReportLinkKeyResult.bd_addr,
                                       Parms->p.ReportLinkKeyResult.device_type,
                                       Parms->p.ReportLinkKeyResult.cod,
                                       Parms->p.ReportLinkKeyResult.key_type,
                                       Parms->p.ReportLinkKeyResult.key,
                                       Parms->p.ReportLinkKeyResult.persist);
            break;  
    }
}

