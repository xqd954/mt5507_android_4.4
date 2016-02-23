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
 * bt_adp_bm.h
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
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * Nov 26 2008 mtk01239
 * [MAUI_01284669] [Bluetooth] New arch for BT
 * 
 *
 * Jul 15 2008 mtk01411
 * [MAUI_00804954] [Bluetooth] Support Read RSSI value request from MMI
 * 
 *
 * Jul 2 2008 MTK02126
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * May 14 2008 mtk01239
 * [MAUI_00771864] [Bluetooth] BT 2.1 new feature TW side
 * 
 *
 * Sep 12 2007 mtk01239
 * [MAUI_00546740] [Bluetooth] Checkin JSR82 code
 * 
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Dec 30 2006 mtk01239
 * [MAUI_00355159] [1] Assert Fail: 0 BTMMICm.c 1321 - MMI
 * 
 *
 * Dec 29 2006 mtk01239
 * [MAUI_00355020] Assert fail: 0 bt_adp_bm.c 1729 - BT
 * 
 *
 * Sep 4 2006 mtk01239
 * [MAUI_00326554] [1] Assert fail: remDev->state == BDS_DISCONNECTED me.c 2420 - BT
 * 
 *
 * Jul 16 2006 mtk01239
 * [MAUI_00210782] [Bluetooth][ESI]update Bluetooth codes
 * 
 *
 * Jun 5 2006 mtk01239
 * [MAUI_00199721] [Bluetooth][ESI] update bt code update
 * 
 *
 * Apr 3 2006 mtk00758
 * [MAUI_00184507] [BT]IT check in
 * 
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#include "bt_common.h"
#include "bluetooth_gap_struct.h"

#ifndef _BT_ADP_BM_H_ 
#define _BT_ADP_BM_H_



/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

#define BTBM_EVENT_READ_SCAN_MODE_RESULT             (0x01)
#define BTBM_EVENT_READ_CLASS_OF_DEVICE_RESULT             (0x02)
#define BTBM_EVENT_WRITE_LOCAl_CLASS_OF_DEVICE_RESULT       (0x03)
#define BTBM_EVENT_READ_LOCAL_DEVICE_NAME_RESULT            (0x04)
#define BTBM_EVENT_READ_LOCAL_BLUETOOTH_ADDR_RESULT         (0x05)
#define BTBM_EVENT_WRITE_SCAN_MODE_RESULT                   (0x06)
#define BTBM_EVENT_DISCOVERY_COMPLETE_RESULT                (0x07)
#define BTBM_EVENT_DISCOVERY_CANCEL_COMPLETE_RESULT         (0x08)
#define BTBM_EVENT_DISCOVERY_DEVICE_RESULT                  (0x09)
#define BTBM_EVENT_WRITE_LOCAL_DEVICE_NAME_RESULT            (0x0A)
#define BTBM_EVENT_SET_AUTHENTICATION_RESULT                (0x0B)
#define BTBM_EVENT_DELETE_ONE_TRUST_DEVICE_RESULT           (0x0C)
#define BTBM_EVENT_DELETE_ALL_TRUST_DEVICE_RESULT           (0x0D)
#define BTBM_EVENT_ALLOW_ACL_LINK_RESULT                    (0x0E)
#define BTBM_EVENT_DISALLOW_ACL_LINK_RESULT                 (0x10)
#define BTBM_EVENT_BLOCKLIST_DISCONNECT_RESULT              (0x11)
#define BTBM_EVENT_BLOCKLIST_UPDATED_RESULT                 (0x12)
#define BTBM_EVENT_BONDING_DEVICE_RESULT                    (0x13)
#define BTBM_EVENT_BONDING_DEVICE_RESULT_INDICATION         (0x14)
#define BTBM_EVENT_REPORT_ACTIVE_LINK_NO                    (0x15)
#define BTBM_EVENT_PINCODE_REQUEST_INDICATION               (0x16)
#define BTBM_EVENT_READ_REMOTE_DEVICE_NAME_RESULT           (0x17)
#define BTBM_EVENT_SSP_SECURITY_PAIRING_INDICATION          (0x18)
#define BTBM_EVENT_SSP_KEY_PRESS_INDICATION                 (0x19)
#define BTBM_EVENT_LINK_ACCEPT_CONFIRM_INDICATION           (0x1A)
#define BTBM_EVENT_REPORT_NETWORK_STATUS                    (0x1B)
#define BTBM_EVENT_REPORT_RSSI_VALUE                        (0x1C)
#define BTBM_EVENT_REPORT_SCATTERNET_STATUS                 (0x1D)
#define BTBM_EVENT_READ_REMOTE_DEVICE_NAME_CANCEL_RESULT    (0x1E)
#define BTBM_EVENT_BONDING_CANCEL_RESULT                    (0x1F)
#define BTBM_EVENT_REMOTE_DEVICE_NAME_INDICATION           (0x20)
#define BTBM_EVENT_GET_LINK_KEY                                         (0x21)

#define BTBM_EVENT_SERVICE_SEARCH_RESULT                    (0x31)
#define BTBM_EVENT_SERVICE_SEARCH_COMPLETED                 (0x32)
#define BTBM_EVENT_ATTRIBUTE_SEARCH_RESULT                  (0x33)
#define BTBM_EVENT_ATTRIBUTE_SEARCH_COMPLETED               (0x34)
#define BTBM_EVENT_SERVICE_SEARCH_CANCELLED                 (0x35)

#define BTBM_EVENT_READ_UUID_LIST_RESULT			(0x36)

typedef U8 BMEvent;

typedef struct _BMEventReadCodResult {
    U8            result;
    U32           cod;
} BMEventReadCodResult;

typedef struct _BMEventReadLocalDevNameResult {
    U8            length;
    U8            *name;
} BMEventReadLocalDevNameResult;

typedef struct _BMEventReadLocalBDAddrResult {
    U8                  result;
    btbm_bd_addr_t      bd_addr;
} BMEventReadLocalBDAddrResult;

typedef struct _BMEventReadLocalUuidListResult {
    U8                  result;
    U32                *uuid_list;
} BMEventReadLocalUuidListResult;

typedef struct _BMEventDiscoveryCompletedResult {
    U8                  result;
    U8                  total_no;
} BMEventDiscoveryCompletedResult;

typedef struct _BMEventDiscoveryCancelCompletedResult {
    U8                  result;
    U8                  total_no;
} BMEventDiscoveryCancelCompletedResult;

typedef struct _BMEventDiscoveryDeviceResult {
    BOOL                            is_update;
    btbm_bd_addr_t                  bd_addr;
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
    U8                              rssi;
    U8                              eir_supported;
    U8                              uuid_list_supported;
    U8                              servlist_completed;
    U32                             *uuid_list;
#endif
    U32                             cod;
    U8                              name_len;
    U8                              *name;
#ifdef __BT_4_0_BLE__
    btbm_device_type                dev_type;
#endif
} BMEventDiscoveryDeviceResult;

typedef struct _BMEventDeleteOneTrustDeviceResult {
    U8                  result;
    btbm_bd_addr_t      bd_addr;
} BMEventDeleteOneTrustDeviceResult;

typedef struct _BMEventBondCompleteResult {
    U8                  result;
    btbm_bd_addr_t      bd_addr;
} BMEventBondCompleteResult;

typedef struct _BMEventBondCancelResult {
    U8                  result;
    btbm_bd_addr_t      bd_addr;
} BMEventBondCancelResult;

typedef struct _BMEventLinkKeyGenerateResult {
    U8                  result;
    U8                  type;
    U8                  key_type;
    btbm_bd_addr_t      bd_addr;
    U32                 cod;
    U8                  *key;
#ifdef __BT_4_0_BLE__
    btbm_device_type device_type;
#endif
} BMEventLinkKeyGenerateResult;

typedef struct _BMEventPinCodeRequestIndication {
    U8                  pin_code_16_digit_required;
    btbm_bd_addr_t      bd_addr;
    U8                  name_len;
    U8                  *name;
    U32                 cod;
} BMEventPinCodeRequestIndication;

typedef struct _BMEventReadRemoteNameResult {
    U8                  result;
    btbm_bd_addr_t      bd_addr;
    U8                  name_len;
    U8                  *name;
    U32                 cod;
} BMEventReadRemoteNameResult;

typedef struct _BMEventReadRemoteNameCancelResult {
    U8                  result;
} BMEventReadRemoteNameCancelResult;

typedef struct _BMEventRemoteNameIndication {
    btbm_bd_addr_t      bd_addr;
    U8                  name_len;
    U8                  *name;
} BMEventRemoteNameIndication;

typedef struct _BMEventSSPSecurityPairingIndication {
    btbm_bd_addr_t      bd_addr;
    U8                  ssp_type;
    U32                 numericValue;
    U8                  name_len;
    U8                  *name;
    BOOL                displayNumeric;
    U32                 cod;
} BMEventSSPSecurityPairingIndication;

typedef struct _BMEventSSPKeyPressIndication {
    btbm_bd_addr_t      bd_addr;
    U8                  event;
} BMEventSSPKeyPressIndication;


typedef struct _BMEventReportNetworkStatus {
    U8                  result;
    BOOL                is_master;
    BOOL                is_authenticated;
    BOOL                is_encrypted;
} BMEventReportNetworkStatus;

typedef struct _BMEventReportRSSIValue {
    U8                  error_code;
    U8                  rssi_value;
    btbm_bd_addr_t      bd_addr;   
} BMEventReportRSSIValue;


typedef struct _BMEventReportScatternetStatus {
    U8                  piconet;
    U8                  scatternet;
    U8                  sco;
    U8                  esco;
} BMEventReportScatternetStatus;
    
typedef struct _BMEventReportServiceSearchResult {
    U32                 uuid;
    btbm_bd_addr_t      bd_addr;   
} BMEventReportServiceSearchResult;

typedef struct _BMEventReportServiceSearchComplete {
    U8                  result;
    btbm_bd_addr_t      bd_addr;
    U8                  no;
} BMEventReportServiceSearchComplete;

typedef struct _BMEventReportAttributeSearchResult {
    btbm_bd_addr_t      bd_addr;
    U16                 len;
    U8                  *data;
} BMEventReportAttributeSearchResult;

typedef struct _BMEventReportAttributeSearchComplete {
    U8                  result;
    btbm_bd_addr_t      bd_addr;
#if defined(BTMTK_ON_LINUX)
    U16                 len;
    U8                  *data;
#endif
} BMEventReportAttributeSearchComplete;

typedef struct _BMEventReportLinkStateResult {
    U8                  ActiveLinkNumber;
    btbm_bd_addr_t      bd_addr;
} BMEventReportLinkStateResult;

typedef struct _BMEventGetLinkKeyResult {
    U8                      result;
    U8                      key_type;
    btbm_bd_addr_t   bd_addr;
    U32                    cod;
    U8                      *key;
    btbm_device_type device_type;
    U8                      persist;  /* If the link is only for this connection */
} BMEventGetLinkKeyResult;

struct _BMCallbackParms {
    BMEvent    event;   /* Event associated with the callback       */
    union {
        void                                    *ptr;
        U8                                      result;
        U8                                      ReadScanModeResult;
        btbm_bd_addr_t                          bd_addr;
        BMEventReadCodResult                    ReadCodResult;
        BMEventReadLocalDevNameResult           ReadLocalNameResult;
        BMEventReadLocalBDAddrResult            ReadLocalAddrResult;
        BMEventReadLocalUuidListResult           	ReadLocalUuidListResult;	
        BMEventDiscoveryCompletedResult         DiscoveryCompleteResult;
        BMEventDiscoveryCancelCompletedResult   DiscoveryCancelCompleteResult;
        BMEventDiscoveryDeviceResult            DiscoveryDeviceResult;
        BMEventDeleteOneTrustDeviceResult       DeleteOneTrustDeviceResult;
        BMEventBondCompleteResult               BondingDeviceCompleteResult;
        BMEventBondCancelResult                 BondCancelResult;
        BMEventLinkKeyGenerateResult            LinkKeyGereratedResult;
        BMEventPinCodeRequestIndication         PinCodeRequestIndication;
        BMEventReadRemoteNameResult             ReadRemoteNameResult;
        BMEventRemoteNameIndication             RemoteNameIndication;
        BMEventReadRemoteNameCancelResult       ReadRemoteNameCancelResult;
        BMEventSSPSecurityPairingIndication     SSPSecurityPairingIndication;
        BMEventSSPKeyPressIndication            SSPKeyPressIndication;
        BMEventReportNetworkStatus              ReportNetworkStatus;
        BMEventReportRSSIValue                  ReportRSSIValue;
        BMEventReportScatternetStatus           ReportScatternetStatus;
        BMEventReportServiceSearchResult        ReportServiceSearchResult;
        BMEventReportServiceSearchComplete      ReportServiceSearchComplete;
        BMEventReportAttributeSearchResult      ReportAttributeSearchResult;
        BMEventReportAttributeSearchComplete    ReportAttributeSearchComplete;
        BMEventReportLinkStateResult            ReportLinkStateResult;
        BMEventGetLinkKeyResult                     ReportLinkKeyResult;
    } p;
};

typedef struct _BMCallbackParms BMCallbackParms;

typedef void (*BMCallback)(BMCallbackParms *Parms);

void BTBM_Register(BMCallback Callback);

void bm_adp_event_init(void);

void bm_adp_message_handler(ilm_struct *ilm_ptr);

#endif

