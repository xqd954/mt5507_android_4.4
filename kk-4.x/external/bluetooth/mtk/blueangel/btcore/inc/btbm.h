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
 * Btbm.h
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains sdap functions prototype and variable definiation of SDAP services
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
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 *
 * 11 02 2012 huirong.liao
 * [ALPS00384674]  audio path
 * .
 *
 * 03 28 2012 dlight.ting
 * [ALPS00259243] [Bluetooth] BT4.0 inquiry will crash
 * .
 *
 * 02 23 2012 dlight.ting
 * [ALPS00238652] [ICS][CTS Verifier] Bluetooth insecure server and client - 
 * .
 *
 * Mar 12 2009 mtk01411
 * [MAUI_01645202] [Bluetooth] Btadp folder restruct
 * 
 *
 * Nov 26 2008 mtk01239
 * [MAUI_01284669] [Bluetooth] New arch for BT
 * 
 *
 * Oct 7 2008 mtk01239
 * [MAUI_01251047] [Bluetooth] Checkin new scan mode.
 * 
 *
 * Aug 5 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Jul 11 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Jul 11 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Jul 2 2008 MTK02126
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Jun 6 2008 mtk01239
 * [MAUI_01156061] BT: Assert Fail: lsListCircular(&MEC(regList)) me.c 1513 - BT
 * 
 *
 * May 26 2008 mtk01239
 * [MAUI_00777619] [Bluetooth] Revise the BTBM code
 * 
 *
 * May 16 2008 mtk01239
 * [MAUI_00771864] [Bluetooth] BT 2.1 new feature TW side
 * 
 *
 * May 14 2008 mtk01239
 * [MAUI_00771864] [Bluetooth] BT 2.1 new feature TW side
 * 
 *
 * Mar 31 2008 mtk01411
 * [MAUI_00745048] [Bluetooth] Modify ask name mechanism in BTStack
 * 
 *
 * Mar 31 2008 mtk01411
 * [MAUI_00289659] [BT] can't search any more
 * 
 *
 * Mar 27 2008 mtk01411
 * [MAUI_00644078] [JSR82][1] Assert Fail: 0 btmmicm.c 1570 - MMI (src_module is set as MMI abnormally)
 * 
 *
 * Mar 24 2008 mtk01411
 * [MAUI_00286133] [JSR82] Screen stay on the "Please Wait" (No local name cnf is reported)
 * 
 *
 * Sep 3 2007 mtk01239
 * [MAUI_00541673] [Bluetooth] Update BT codes.
 * 
 *
 * Aug 20 2007 mtk01239
 * [MAUI_00535716] [Bluetooth] fix complier warming.
 * 
 *
 * Aug 20 2007 mtk01239
 * [MAUI_00237089] MTK:Bluetooth HFP_when inquiry new device during handset popup "Handfree profile dis
 * 
 *
 * Aug 4 2007 mtk01239
 * [MAUI_00526809] BT-HID can't work
 * 
 *
 * Jul 16 2007 mtk01411
 * [MAUI_00417564] [Bluetooth] Gap tester code modification for PTW BT2.0 test case
 * 
 *
 * Jun 20 2007 mtk01239
 * [MAUI_00407195] UPF_We are failed to bond with openinterface
 * 
 *
 * Jun 18 2007 mtk01239
 * [MAUI_00406886] [Bluetooth UPF27] Assert BTMMICm.c
 * 
 *
 * Jun 5 2007 mtk01239
 * [MAUI_00401290] BT-FTP auto disconnect
 * 
 *
 * May 7 2007 mtk01239
 * [MAUI_00389758] [Bluetooth] Checkin JSR82 code of BTStack
 * 
 *
 * Mar 26 2007 mtk01239
 * [MAUI_00231225] bt_hid disconnected indication pop up, but it return to disconnecting screen
 * 
 *
 * Mar 8 2007 mtk01239
 * [MAUI_00230591] bt_after remote sim mode activating canceled, the mode is still on
 * 
 *
 * Mar 5 2007 mtk01239
 * [MAUI_00357653] [BT&WLAN Coexist] Can't receive VoIP MT call when BT inquiry audio device
 * 
 *
 * Jan 9 2007 mtk01239
 * [MAUI_00357401] [Bluetooth] The name request of inquiry something has no responses
 * 
 *
 * Jan 8 2007 mtk01239
 * [MAUI_00356108] Bluetooth-Connect "Handsfree Profile" and "Connect Failed" will pop up, "Cancel" wil
 * 
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Dec 26 2006 mtk01239
 * [MAUI_00353354] [1] Assert fail: 0 bt_adp_bm.c 1720 - BT
 * 
 *
 * Dec 25 2006 mtk01239
 * [MAUI_00353354] [1] Assert fail: 0 bt_adp_bm.c 1720 - BT
 * 
 *
 * Dec 3 2006 mtk01239
 * [MAUI_00347951] [Bluetooth] Figure out what BT chip is used
 * 
 *
 * Nov 22 2006 mtk01239
 * [MAUI_00342697] BT-Bond failed and Entry passcode pop up at the same time
 * 
 *
 * Oct 30 2006 mtk01239
 * [MAUI_00337958] Bluetooth_ The screen will pop up Initializing Bluetooth now, wait a moment! whe
 * 
 *
 * Oct 30 2006 mtk01239
 * [MAUI_00336162] [1] Assert fail: g_mmi_bth_cntx.paired_dev BTMMICm.c 3304 - MMI
 * 
 *
 * Oct 4 2006 mtk00798
 * [MAUI_00331575] [New Feature] MTK BT A2DP with MP3 and SBC streaming capabilities
 *
 * 
 * Oct 4 2006 mtk01239
 * [MAUI_00334643] [Bluetooth] fix complier warning
 * 
 *
 * Aug 21 2006 mtk01239
 * [MAUI_00322918] [BT] Interaction between BT-earphone and earphone, Assert Fail: meif.c 1051 -BT occu
 * 
 *
 * Aug 7 2006 mtk01239
 * [MAUI_00215228] [BT] update Bluetooth code
 * 
 *
 * Aug 4 2006 mtk01239
 * [MAUI_00215228] [BT] update Bluetooth code
 * 
 *
 * Jul 16 2006 mtk01239
 * [MAUI_00210782] [Bluetooth][ESI]update Bluetooth codes
 * 
 *
 * Jun 19 2006 mtk01239
 * [MAUI_00203552] [Bluetooth][ESI] update bt code
 * 
 *
 * Jun 12 2006 mtk01239
 * [MAUI_00201555] [Bluetooth][ESI] update bt code
 * 
 *
 * Jun 5 2006 mtk01239
 * [MAUI_00199721] [Bluetooth][ESI] update bt code update
 * 
 *
 * Apr 24 2006 mtk01239
 * [MAUI_00189553] ESI Bluetooth project update
 * update
 *
 * Apr 3 2006 mtk00758
 * [MAUI_00184485] [New feature] Add task to simulate UART driver for unitest
 * 
 *
 * Mar 11 2006 MTK00303
 * [MAUI_00178684] [Bluetooth]ESI bluetooth protocol stack
 * add bluetooth files
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/

#ifndef _BTBM_H
#define _BTBM_H
#include "bt_feature.h"
#include "bt_devices_list.h"
#include "me_adp.h"
#include "medev_adp.h"
#include "bt_adp_bm.h"
#include "btbm_export.h"

/***************
Type Definitions 
****************/

#define BM_OP_INQUIRY               (0x01)
#define BM_OP_NAMEREQ               (0x02)
#define BM_OP_INQCANCEL            (0x04)
#define BM_OP_BONDING                (0x08)
#define BM_OP_PAIRING                  (0x10)
#define BM_OP_SETSCANMODE       (0x20)
#define BM_OP_INQINTERVAL          (0x40)
#define BM_OP_NONE                      (0x80)
#define BM_OP_NAMECANCEL               (0x100)


typedef U16 BmInquiryState;
/* Inquiry state */
/* IDLE state */
#define BM_INQUIRY_NONE                     (0x0000)
/* Preprocess state - Preprocess for search state. ex. role switch for existed links */
#define BM_INQUIRY_PREPROCESS         (0x0001)
/* Search device sate - send HCC_INQUIRY to controller and wait for event of HCE_INQUIRY_COMPLETE*/
#define BM_INQUIRY_SEARCH                 (0x0002)
/* Preprocess state - Query name for all searched devices without name */
#define BM_INQUIRY_NAME_QUERY        (0x0004)
/* Delay state - Delay some time before loop to search state to guard something */
#define BM_INQUIRY_DELAY                   (0x0008)
#define BM_INQUIRY_CANCEL                   (0x0010)

/* Bonding state */
#define BM_BONDING_NONE                             (0x0000)
#define BM_BONDING_ACL_CREATED              (0x0001)
#define BM_BONDING_PINCODE_REQ                (0x0002)
#define BM_BONDING_PINCODE_ENTER          (0x0004)
#define BM_BONDING_NAME_REQUESTED       (0x0008)
#define BM_BONDING_SUCCESS_COMPLETED (0x0010)
#define BM_BONDING_FAILED_COMPLETED    (0x0020)
#define BM_BONDING_CANCELING    (0x0080)
#define BM_BONDING_CANCELED   (0x0100)

#define BM_BONDING_NOT_ACTIVATE         (0x00)
#define BM_BONDING_ACTIVATE             (0x01)

#define BM_SETTING_UPDATE_VALUE    (0x00)
#define BM_SETTING_KEEP_VALUE                 (0x01)

#define BM_RESTORE_NONE_MODE                     (0x00)
#define BM_RESTORE_WRITESCAN_MODE           (0x01)
#define BM_RESTORE_LOCALNAME_MODE           (0x02)
#define BM_RESTORE_AUTHENTICATION_MODE (0x04)

#define BM_ROLE_SWITCH_NONE    (0x00)
#define BM_ROLE_SWITCH_EXIT_SNIFF         (0x01)
#define BM_ROLE_SWITCH_STOP_ENCRYPTION    (0x02)
#define BM_ROLE_SWITCH_ROLE_SWITCH            (0x03)
#define BM_ROLE_SWITCH_START_ENCRYPTION  (0x04)

#define BM_POWER_OFF (0x00)
#define BM_POWER_ON     (0x01)
#define BM_POWER_OFF_PENDING    (0x02)

#define BM_LINK_ALLOW   (0x00)
#define BM_LINK_DISALLOW      (0x01)

#define BM_WRITE_AUTHENTICATION_NONE            (0x00)
#define BM_WRITE_AUTHENTICATION_PROCESSING_NOT_REPORT  (0x01)
#define BM_WRITE_AUTHENTICATION_PROCESSING_NEED_REPORT  (0x02)

/*---------------------------------------------------------------------------
 * MAX_DEVICE_NAME_SIZE constant
 *
 *     This constant defines the maximum length of a device name that can
 *     be stored in the BtDeviceExt context. The value must be at least
 *     enough to store the BD_ADDR as a string, and no larger than the maximum
 *     device name length. The format for the device name is UTF-8.
 */

#define NUM_SUPPORTED_PROFILE_CONN  (20)

#define BM_INQUIRY_LENGTH (0x04)

#define BT_INQUIRY_GAP_INTERVAL_NO_WIFI (10)
#define BT_INQUIRY_GAP_INTERVAL_LINK_CONNECTED (500)
#define BT_INQUIRY_GAP_INTERVAL_WIFI       (1000)

#define BT_BTBM_NO_SCO (0x00)
#define BT_BTBM_MASTER_SCO (0x01)
#define BT_BTBM_SLAVE_SCO (0x02)

#ifndef BOOL
#define BOOL unsigned char 
#endif 

#define BT_BM_SSP_KEYPRESS_STARTED      0x00
#define BT_BM_SSP_KEYPRESS_ENTERED      0x01
#define BT_BM_SSP_KEYPRESS_ERASED       0x02
#define BT_BM_SSP_KEYPRESS_CLEARED      0x03
#define BT_BM_SSP_KEYPRESS_COMPLETED    0x04

#ifdef __BT_4_0_BLE__
typedef enum
{
    BT_BM_DUAL_INQUIRY_NONE = 0x0,
    BT_BM_DUAL_INQUIRY_BR_EDR = 0x01,
    BT_BM_DUAL_INQUIRY_LE = 0x02
} BTBMDualInquiryType;
#endif

typedef enum
{
    BT_BM_ISTATE_NAME_REQ_SENT     = 0x0002,
    BT_BM_ISTATE_NAME_REQ_FINISHED = 0x0004,
    BT_BM_ISTATE_GATT_REQ_SENT     = 0x0008,
    BT_BM_ISTATE_GATT_REQ_FINISHED = 0x0010,
} BTBMInquiredDeviceState;


#define BT_BM_BLE_SERVICE_NOT_AVAILABLE 0x00
#define BT_BM_BLE_SERVICE_SEARCHING     0x01
#define BT_BM_BLE_SERVICE_AVAILABLE     0x02

#define BT_BM_BLE_NAME_NOT_AVAILABLE 0x00
#define BT_BM_BLE_NAME_SEARCHING     0x01
#define BT_BM_BLE_NAME_AVAILABLE     0x02


typedef struct _BTBMInquiredDevice
{
    BOOL used;
    BD_ADDR remote_addr;
    BtDeviceContext *bdc;
    U8 deviceNameAvailable;
    U8 deviceNameSearchCount;	
#ifdef __BT_4_0_BLE__
    BtDevType devType;
    LeAddrType addrType;
    U8 queuedGattDevNameLen;
    U8 queuedGattDevName[BTBM_ADP_MAX_NAME_LEN+1];
#endif
} BTBMInquiredDevice;

typedef struct _BTBMSaveSetting
{
    U8 writescan_mode;
    U8 le_writescan_mode;
    U8 len;
    U8 local_name[BTBM_ADP_MAX_NAME_LEN + 1];
#ifdef __BT_4_0_BLE__
    U8 appearance[2];
#endif
} BTBMSaveSetting_T;

typedef struct _BTBMConn_struct_t
{
    BtDeviceContext *bdc;
    U16 uuid;
    U8 used;
} BTBMConn_struct_t;

typedef struct _BTBMReqDevice_struct_t
{
    btbm_bd_addr_t bd_addr;
    U8 name_len;
    U8 name[BTBM_ADP_MAX_NAME_LEN + 1];
} BTBMReqDevice_struct_t;

typedef struct _BTBMRemoteNameSetting
{
    U8 dev_name_len;
    char dev_name[BTBM_ADP_MAX_NAME_LEN + 1];   /* Name can be recorded in Bond/Pair/No Auth Name Query Procedure */
    BtRemoteDevice *link;                       /* When ACL LINK is CONNECTED, the field is not empty */
    U8 wait_for_name_req;                       /* wait for nameTok2 to ask the remote device's name */
    BOOL OwnerofNameTok2;
} BTBMRemoteNameSetting_T;

typedef struct _BTBMSSPRemoteDeviceInfo
{
    U8 addr[6];
    U8 ioCapability;
    U8 OOBDataPresent;
    U8 authRequirements;
    U8 localAuthRequirements;
    U8 sspType;
    U32 numericValue;
    U8 keyPressNo;
    BOOL keyPressEventOccurs;
    BOOL keyPressEventReporting;
} BTBMSSPRemoteDeviceInfo_T;

typedef struct _BMContent_T
{
    U16 bm_opactivity;

    /* Inquiry-related */
    U8 mmi_inquiry_length;
    U8 mmi_inquiry_number;
#ifdef __BT_USE_CUST_DATA__
    U8 loop_inquiry_cust_tx_pwr_idx;
    U8 loop_inquiry_cust_len_idx;
    U8 loop_inquiry_cust_num_idx;
#endif
    U32 mmi_inquiry_time;
    U8 access_mode;
    U8 discovered_device_no;                                /* Record the total matched searched devices */
    U8 inquiry_data;
    U32 filter_cod;                                         /* Record the cod that the MMI layer want to response */
    BOOL inquiry_with_name;                     /* To record the inquiry type to discovery the name */
    BTBMInquiredDevice inquiredDevices[BTBM_ADP_MAX_INQUIRY_NO];
    EvmTimer inquiry_preprocess_timer;
    EvmTimer inquiry_loop_timer;
#ifdef __BT_4_0_BLE__
    btbm_discovery_type dual_discovery_type;
    BTBMDualInquiryType dual_inquiry_type_cur;
    U8 LeAdvIndex;
#endif
   
    U8 power_on_off_mode;
    U8 link_allow_state;
    U8 bm_scan_opactivity;
    U8 bm_scan_mode;
    U8 bm_security_mode;                                    /* security mode of current devices setting */
    U8 wait_conection_acp_rsp;
    U8 write_authentication_state;
    U8 hid_broadcom_iot_role_switch;
    U8 hfp_auto_disconnect_black_list;
    #ifdef __INDEPENDENT_INQUIRY_FLOW__
    BmInquiryState inquiry_state;
    #endif
    U16 bonding_state;
    BD_ADDR remote_addr;                                    /* Record the current operation remote bd_address */
    MeCommandToken nameTok;                                 /* Name request token for inquiry and pin code request to ask name */
    MeCommandToken nameTok2;                                /* Name request token for the case "incoming acl link (no auth enabled on both devices)" */
    MeCommandToken nameTok3;                                /* Name request token for JSR82 ask name */
    #ifdef __INDEPENDENT_INQUIRY_FLOW__
    MeCommandToken nameTok4;                                /* Name request token for Inquiry */
    #endif
    BTBMConn_struct_t conn[NUM_SUPPORTED_PROFILE_CONN];
    BTBMRemoteNameSetting_T DevRemNameTbl[NUM_BT_DEVICES];  /* This table is used to record the remote device's name */
    BtDeviceContext *pin_bdc;                               /* Record pervious request the PIN code device entry */
    BtRemoteDevice *connect_acp_link;
    BTBMSaveSetting_T bm_setting;
    BTBMReqDevice_struct_t pin_req_device;
    U32 sdap_uuid[BTBM_ADP_MAX_SDAP_UUID_NO];
    U8 sdap_len;
#if defined (__BT_2_1_SIMPLE_PAIRING__)
    BTBMSSPRemoteDeviceInfo_T SSPDeviceInfo;
#endif 
    U8 scoData;
    EvmTimer name_cancel_timer;
    EvmTimer pin_req_timer;
    EvmTimer active_name_timer;
    BMCallback bmCB;
#ifdef __GAP_TESTER_
    U8 gapNonBondingState;
    U8 gapCurrentBondingState;
#endif /* __GAP_TESTER_ */ 
#if defined (__BT_2_1_BLOCK_DEVICE__)
    U8 blockNo;
    U8 blockAddr[BTBM_ADP_MAX_BLOCK_LIST_NO *6];
#endif /* defined (__BT_2_1_BLOCK_DEVICE__) */
#if (BT_READ_RSSI == XA_ENABLED)
    BOOL rssi_op_ongoing;
#endif 
    U8 ctsSdpAttributeSearchPendingInquiry;
    U8 CTSTest;
    U8 inquiryShoAddress;
    EvmTimer service_search_delay_timer;
    BD_ADDR service_search_addr;
} BMCONTENT_T;

/*************************************************************************
* FUNCTION                                                            
*  BTBMInit
* DESCRIPTION                                                           
*   This function is used to initialize the context of btbm.
*
* PARAMETERS
*  None
* RETURNS
*  init result
* GLOBALS AFFECTED
*   
*************************************************************************/
BOOL BTBMInit(void);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGetDeviceModel
* DESCRIPTION                                                           
*   The function is used to get remote device model (Limited listed device, not all devices)
*
* PARAMETERS
*  addr: Remote device address
* RETURNS
*  Device Model enum
* GLOBALS AFFECTED
*   
*************************************************************************/
DEVICE_MODEL BTBMGetDeviceModel(BD_ADDR *addr);

DEVICE_MODEL BTBMGetDeviceModelByVersion(BtRemoteDevice *remDev);

DEVICE_MODEL BTBMGetDeviceModelByAddr(BD_ADDR *addr);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapGetWholeProperty
* DESCRIPTION                                                           
*   This function is used to read the whole Bluetooth property.
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapGetWholeProperty(void);

U8 BTBMGapCopyLocalNameWithoutCFM(U8 *name);

#ifdef __GAP_TESTER_
#if defined (__BT_2_1_SIMPLE_PAIRING__)
/*************************************************************************
* FUNCTION                                                            
*  BTBMGapEnabledNonBondingTSDevice
* DESCRIPTION                                                           
*   This function is used to enabled auto reject bonding procedure in GAP v2.1 test cases.
*
* PARAMETERS
*  enabled: 0x00: disabled 
*            0x01: enabled
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapEnabledNonBondingTSDevice(U8 enabled);

/*************************************************************************
* FUNCTION                                                            
*  gapCurrentBondingState
* DESCRIPTION                                                           
*   This function is used to enabled dedicated bonding or general bonding in GAP v2.1 test cases.
*
* PARAMETERS
*  enabled: 0x00: disabled 
*            0x01: enabled
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapSetSSPBondingTSDevice(U8 type);

#endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapBondingforGAPTSDevice
* DESCRIPTION                                                           
*   This function is used to make the bond with remote device in GAP test cases.
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
void BTBMGapBondingforGAPTSDevice(bt_bm_bonding_req_struct *btbm_adp_input_msg);
#endif /* __GAP_TESTER_ */ 

/*************************************************************************
* FUNCTION                                                            
*  BTBMLocalWriteScanEnabled
* DESCRIPTION                                                           
*  The function is used by BT stack internally. When we connect, pairing, bonding, inquiry, disconnect,
*  we will turn the discovery mode off or on. The function is used by internal stack to do the procedure.
*  When the special scenario is finished, the discovery mode will be set to the original mode.
*
* PARAMETERS
*  mode: The discovery mode shall be turn off or restore to origial value
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMLocalWriteScanEnabled(U8 mode);

/*************************************************************************
* FUNCTION                                                            
*  BTBMLocalAllowAllLink
* DESCRIPTION                                                           
*   It is used by SAP profile to set the btstack into ready to accept new connection. 
*   After that, we can accept new connection
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMLocalAllowAllLink(void);

/*************************************************************************
* FUNCTION                                                            
*  BTBMReturnNormalLinkState
* DESCRIPTION                                                           
*  The function is set the BT stack as accept new link state or reject all link state. In which state depends on
*  the previous state. Currently, only the SIM will use it.
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMReturnNormalLinkState(void);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGetLocalStoredName
* DESCRIPTION                                                           
*  The function is used to get connected device name
*
* PARAMETERS
*  bdAddr: The remote device Bluetooth Address
* RETURNS
*  The pointer points to the connected device name.
* GLOBALS AFFECTED
*   
*************************************************************************/
U8 *BTBMGetLocalStoredName(U8 *bdAddr);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapSetEncryptionOff
* DESCRIPTION                                                           
*   The function is used to turn the encryption to OFF.
*
* PARAMETERS
*    None   
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapSetEncryptionOff(void);

/*************************************************************************
* FUNCTION                                                                                                        
*  BTBMCheckAllowScoConnectReq
* DESCRIPTION                                                           
*   The function is used to provide the ckeck for SCO can be connected or not.
*   If the current power on off state is power on, the SCO can be accepted. Otherwise, the SCO will be rejected.
*
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
U8 BTBMCheckAllowScoConnectReq(void);

#ifdef __BQE_TESTER_
/*************************************************************************
* FUNCTION                                                            
*  BTBMGapLinkCreate
* DESCRIPTION                                                           
*     The function is used to GAP, SDP  testing to create ACL link
*
* PARAMETERS
*  bt_bm_acl_link_req_struct 
*       bd_addr: The remote device address

* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapLinkCreate(bt_bm_acl_link_req_struct *btbm_adp_input_msg);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapLinkClose
* DESCRIPTION                                                           
*     The function is used to GAP, SDP  testing to close ACL link
*
* PARAMETERS
*  bt_bm_acl_link_close_struct 
*       bd_addr: The remote device address

* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void BTBMGapLinkClose(bt_bm_acl_link_close_struct *btbm_adp_input_msg);
#endif /* __BQE_TESTER_ */ 

/*************************************************************************
* FUNCTION                                                            
*  BTBMProfileAllocateConnIndex
* DESCRIPTION                                                           
*  All profiles want to get a unique index by UUID and address. The BTBM will assign a unique index for them.
*  If no connection is available, the system will assert.
* PARAMETERS
*   uuid: The request profile's UUID
*   addr: The request connection address.
* RETURNS
*  U8 unique index.
* GLOBALS AFFECTED
*   
*************************************************************************/
U8 BTBMProfileAllocateConnIndex(U32 uuid, BD_ADDR addr);

/*************************************************************************
* FUNCTION                                                            
*  BTBMProfileReleaseConnIndex
* DESCRIPTION                                                           
*  The function is used to return the unique index allocated by btbm
*
* PARAMETERS
*  index: The allocated index
* RETURNS
*  The result of the release operation.
* GLOBALS AFFECTED
*   
*************************************************************************/
BOOL BTBMProfileReleaseConnIndex(U8 index);

/*************************************************************************
* FUNCTION                                                            
*  BTBMProfileSearchConnIndex
* DESCRIPTION                                                           
*  The function is used to search the unique index by UUID and address
*
* PARAMETERS
*   uuid: The request profile's UUID
*   addr: The request connection address.
* RETURNS
*  The searched unque index.
* GLOBALS AFFECTED
*   
*************************************************************************/
U8 BTBMProfileSearchConnIndex(U32 uuid, BD_ADDR addr);

/*************************************************************************
* FUNCTION                                                            
*  Me2BMCallback
* DESCRIPTION                                                           
*   This function is register to ME.
*   When ME has events, the Me2BMCallback will also be called
*
* PARAMETERS
*      Event : Event of ME layer
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
void Me2BMCallback(const BtEvent *Event);

/*************************************************************************
* FUNCTION                                                            
*  btbmDisconnectAclLinkWithoutForce
* DESCRIPTION                                                           
*           
*
* PARAMETERS
*              link 
* RETURNS
*  None
* Local AFFECTED
*   
*************************************************************************/
void btbmDisconnectAclLinkWithoutForce(BtRemoteDevice *link, U8 reason, U8 index);

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

/*************************************************************************
* FUNCTION                                                            
*  BTBMDoRoleSwitch
* DESCRIPTION                                                           
*  To perform the role switch operation to be the master for a ACL link
*
* PARAMETERS
*  link: the ACL link
* RETURNS
*  None
* GLOBALS AFFECTED
*   
*************************************************************************/
BtStatus BTBMDoRoleSwitch(BtRemoteDevice *link);

/*************************************************************************
* FUNCTION                                                            
*  BTBMCheckBMStatusNotExistSnifferMode
* DESCRIPTION                                                           
*  Check no device in outgoing connection and inquiry procedure.
*
* PARAMETERS
*  None
* RETURNS
*  TRUE for in inquiry procedure or device in outgoing connection mode
* GLOBALS AFFECTED
*   
*************************************************************************/
BOOL BTBMCheckBMStatusNotExistSnifferMode(void);

/*************************************************************************
* FUNCTION                                                            
*  BTBMCheckSpecialSlaveProcedure
* DESCRIPTION                                                           
*  Do special procedure for BT chip.
*
* PARAMETERS
*  None
* RETURNS
*  None
* Global AFFECTED
*   
*************************************************************************/
BOOL BTBMCheckSpecialSlaveProcedure(void);

#ifdef __BQE_TESTER_
/*************************************************************************
* FUNCTION                                                            
*  BTBMGapSetFilterCoD
* DESCRIPTION                                                           
*  To set the filter cod
* PARAMETERS
*  the set cod value
* RETURNS
*  None
* Global AFFECTED
*   
*************************************************************************/
void BTBMGapSetFilterCoD(U32);

/*************************************************************************
* FUNCTION                                                            
*  BTBMGapSetDiscoveryWithoutName
* DESCRIPTION                                                           
*  To set the discovery mode as Discovery without name, the pure inquiry procedure.
* PARAMETERS
*  None
* RETURNS
*  None
* Global AFFECTED
*   
*************************************************************************/
void BTBMGapSetDiscoveryWithoutName(void);
#endif /* __BQE_TESTER_ */ 

/*************************************************************************
* FUNCTION                                                            
*  BTBMCheckAclConnectRequestNotAddPendingCon
* DESCRIPTION                                                           
*  To check if there is any pending con waiting for MMI's accept.
* PARAMETERS
*  None
* RETURNS
*  TRUE if there are more than one pending connection waiting for accept. FALSE, no connection is waiting.
* Global AFFECTED
*   
*************************************************************************/
BOOL BTBMCheckAclConnectRequestNotAddPendingCon(void);

/*************************************************************************
* FUNCTION                                                            
*  BTBMHIDNoRoleSwitch
* DESCRIPTION                                                           
*  The HID profiles will set the value that if we connect with broadcom chip, we will not do the role switch operation.
* PARAMETERS
*  ON_OFF: set to on or off
* RETURNS
*  None
* Global AFFECTED
*   
*************************************************************************/
void BTBMHIDNoRoleSwitch(U8 ON_OFF);

/*************************************************************************
* FUNCTION                                                            
*  BTBMChangeScattnetRoleForSCO
* DESCRIPTION                                                           
*  To provide the check if the scatternet exists, if in scatternet state,
*  we will try to do the role switch for one active link.(Not all link)
* PARAMETERS
*  The remdev who requests the piconet capability
* RETURNS
*  The current piconet state(FALSE) or scatternet state(TRUE)
* Global AFFECTED
*   
*************************************************************************/
BOOL BTBMChangeScattnetRoleForSCO(BtRemoteDevice *remDev);
void BTBMGetScattnetStatus(U8 *piconet, U8 *scatternet, U8 *sco, U8 *esco, U8 *msco, U8 *ssco);
void BTBMChangeAllLinkPolcy(BtRemoteDevice *remDev, BOOL disabled);
void BTBMHandleUpdateBlockList(bt_bm_block_list_update_req_struct *btbm_adp_input_msg);


BtStatus BTBMGATT_Init(void);

BtStatus BTBMGATT_GetRemoteDeviceName(BtDeviceContext *bdc, BtCallBack callback);

#endif /* _BTBM_H */ 

