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
*  permission of MediaTek Inc. (C) 2005
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
 * bt_adp_cust.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to customization bt host
 *
 * Author:
 * -------
 * SH Lai
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#include "bt_common.h"
#include "bt_adp_log.h"
#include "bt_cust_api.h"
#include "bt_adp_cust.h"
#include "bluetooth_gap_struct.h"
#include "osapi.h"
#include <cutils/log.h>

typedef struct _custInfo{
    const char *name;
    void *default_value;
    BT_CUST_ID cid; 
}custInfo;

custInfo custItem[] = {
    { "pageTimeout",                            (void*)0x3800,         BT_CUST_UNKNOWN},
    { "connectionTimeout",                    (void*)0x6400,         BT_CUST_UNKNOWN},
    { "InquiryScanWindow",                  (void*)0x0012,         BT_CUST_UNKNOWN},
    { "inquiryScanInterval",                   (void*)0x1000,         BT_CUST_UNKNOWN},
    { "pageScanWindow",                      (void*)0x0012,         BT_CUST_UNKNOWN},
    { "pageScanInterval",                      (void*)0x0800,         BT_CUST_UNKNOWN},
    { "setupSyncCmdRawData",              (void*)0x3800,         BT_CUST_UNKNOWN},
    { "createConnRoleSwitchReq",           (void*)1,                BT_CUST_UNKNOWN},
    { "inquiryLengthArray",                    (void*)0x3800,         BT_CUST_UNKNOWN},
    { "pinPairingReqTimeoutActive",        (void*)0x4500,         BT_CUST_UNKNOWN},
    { "pinPairingReqTimeoutPassive",      (void*)0x4500,         BT_CUST_UNKNOWN},
    { "SSPPairingReqTimeoutActive",      (void*)0x4500,         BT_CUST_UNKNOWN},
    { "SSPPairingReqTimeoutPassive",    (void*)0x4500,         BT_CUST_UNKNOWN},
    { "SCOPktType",                             (void*)0x03C7,         BT_CUST_UNKNOWN},
    { "eSCOPktType",                           (void*)0x0388,         BT_CUST_UNKNOWN},
    { "ForceSCO",                                (void*)0,         BT_CUST_UNKNOWN},
    { "RoleSwitchRetryCount",               (void*)1,                 BT_CUST_UNKNOWN},
    { "GapInquiryShowAddress",            (void*)1,                 BT_CUST_UNKNOWN},
    { "AutoAcceptReqFromPairedDevice",  (void*)1,               BT_CUST_UNKNOWN},
    { "DelayAuthentication",                   (void*)0,               BT_CUST_UNKNOWN},
    { "DisableInBandRingtone",              (void*)1,               BT_CUST_UNKNOWN},
    { "CreateConnectionRetryNoEventReported",  (void*)0,     BT_CUST_UNKNOWN},
    { "SSPSupport",                              (void*)1,                BT_CUST_UNKNOWN},
    { "HFPSDPCache",                            (void*)1,              BT_CUST_UNKNOWN},
    { "HFPSupportMask",                         (void*)3,              BT_CUST_UNKNOWN}, 
    { "HFPVoiceRecognition",                    (void*)0,              BT_CUST_UNKNOWN},       
    { "HFPmSBCCodec",                           (void*)1,              BT_CUST_UNKNOWN},      
    { "30HSSupported",                         (void*)0,              BT_CUST_UNKNOWN},
    { "LESupport",                              (void*)1,              BT_CUST_UNKNOWN},
    { "DevSupportServices",                   (void*)(unsigned short[]){0},    BT_CUST_UNKNOWN},
    { "SDPDelayTimeout",                   (void*)500,    BT_CUST_UNKNOWN},
    { "InquiryTxPwr",                   (void*)(char[]){6, -128/* -128 as terminator */},    BT_CUST_UNKNOWN},
    { "InquiryLength",                   (void*)(unsigned char[]){4, 0},    BT_CUST_UNKNOWN},
    { "InquiryResponse",                   (void*)(unsigned char[]){BTBM_ADP_MAX_INQUIRY_NO, 0},    BT_CUST_UNKNOWN},    
    { "NoRoleSwitch",                            (void*)0,    BT_CUST_UNKNOWN},
    { "KeepAwakeTime",                        (void*)5000,    BT_CUST_UNKNOWN},
    { "LEInitiateInterval",                        (void*)0x48,    BT_CUST_UNKNOWN},
    { "LEInitiateWindow",                        (void*)0x30,    BT_CUST_UNKNOWN},
    { "LEScanInterval",                        (void*)0x140,    BT_CUST_UNKNOWN},
    { "LEScanWindow",                        (void*)0x140,    BT_CUST_UNKNOWN},
    { "LEConnIntervalMin",                   (void*)0x0100,   BT_CUST_UNKNOWN},
    { "LEConnIntervalMax",                   (void*)0x0200,   BT_CUST_UNKNOWN},
    { "LEConnLatency",                       (void*)0x0000,   BT_CUST_UNKNOWN},
    { "LESupervisionTO",                     (void*)0x0500,   BT_CUST_UNKNOWN},     
};

void *checkCustvalue(CUST_ID id, void *value){
    int useDefault = 0;
    
    switch(id){
    case CUSTID_PAGE_TIMEOUT:
        /* Range : 0x0001 V 0xFFFF */
        if(((unsigned int)value) == 0) useDefault = 1;
        break;
    case CUSTID_CONNECTION_TIMEOUT:
        /* Range : 0x0001 V 0xFFFF */
        if(((unsigned int)value) == 0) useDefault = 1;
        break;
    case CUSTID_INQUIRY_SCAN_WINDOW:
        /* Range : 0x0011 V 0x1000 */
        if(((unsigned int)value) < 0x0011 ||
            ((unsigned int)value) > 0x1000) 
            useDefault = 1;
        break;
    case CUSTID_INQUIRY_SCAN_INTERVAL:
        /* Range : 0x0012 V 0x1000 */
        if(((unsigned int)value) < 0x0012 ||
            ((unsigned int)value) > 0x1000) 
            useDefault = 1;
        break;
    case CUSTID_PAGE_SCAN_WINDOW:
        /* Range : 0x0011 V 0x1000 */
        if(((unsigned int)value) < 0x0011 ||
            ((unsigned int)value) > 0x1000) 
            useDefault = 1;
        break;
    case CUSTID_PAGE_SCAN_INTERVAL:
        /* Range : 0x0012 V 0x1000 */
        if(((unsigned int)value) < 0x0012 ||
            ((unsigned int)value) > 0x1000) 
            useDefault = 1;
        break;
    case CUSTID_SETUP_SYNC_CMD_RAWDATA:
        // TBD
        break;
    case CUSTID_CREATE_CONN_ROLE_SWITCH_REQ:
        /* Range : 0, 1 */
        if(((unsigned int)value) != 0 &&
            ((unsigned int)value) != 1) 
            useDefault = 1;
        break;
    case CUSTID_INQUIRY_LENGTH_ARRAY:
        // TBD
        break;
    case CUSTID_PIN_PAIRING_REQ_TIMEOUT_ACTIVE:
        /* Range : 0x0000 V 0xBB80 */
        if(((unsigned int)value) > 0xBB80) 
            useDefault = 1;
        break;
    case CUSTID_PIN_PAIRING_REQ_TIMEOUT_PASSIVE:
        /* Range : 0x0000 V 0xBB80 */
        if(((unsigned int)value) > 0xBB80) 
            useDefault = 1;
        break;
    case CUSTID_SSP_PAIRING_REQ_TIMEOUT_ACTIVE:
        /* Range : 0x0000 V 0xBB80 */
        if(((unsigned int)value) > 0xBB80) 
            useDefault = 1;
        break;
    case CUSTID_SSP_PAIRING_REQ_TIMEOUT_PASSIVE:
        /* Range : 0x0000 V 0xBB80 */
        if(((unsigned int)value) > 0xBB80) 
            useDefault = 1;
        break;
    case CUSTID_SCO_PKT_TYPE:
        /* Range : bitmask 0x03C7 */
        if((((unsigned int)value)&(~0x03C7)) > 0 )  useDefault = 1;
        break;
    case CUSTID_ESCO_PKY_TYPE:
        break;
    case CUSTID_FORCE_SCO:
        /* Range : 0, 1 */
        if(((unsigned int)value) != 0 &&
            ((unsigned int)value) != 1) 
            useDefault = 1;
        break;
    case CUSTID_ROLE_SWITCH_RETRY_COUNT:
        /* Range : 1 V 5 */
        if(((unsigned int)value) == 0 ||
            ((unsigned int)value) > 5)
            useDefault = 1;
        break;
    case CUSTID_GAP_INQUIRY_SHOW_ADDRESS:
        /* Range : 0, 1 */
        if(((unsigned int)value) != 0 &&
            ((unsigned int)value) != 1) 
            useDefault = 1;
        break;
    case CUSTID_AUTO_ACCEPT_REQ_FROM_PAIRED_DEVICE:
        /* Range : 0, 1 */
        if(((unsigned int)value) != 0 &&
            ((unsigned int)value) != 1) 
            useDefault = 1;
        break;
    case CUSTID_DELAY_AUTHENTICATION:
        /* Range : 0x00 V 0x20 */
        if(((unsigned int)value) > 0x20)
            useDefault = 1;        
        break;
    case CUSTID_DISABLE_INBAND_RINGTONE:
        /* Range : 0, 1 */
        if(((unsigned int)value) != 0 &&
            ((unsigned int)value) != 1) 
            useDefault = 1;
        break;
    case CUSTID_CREATE_CONNECTION_RETRY_NO_EVENT_REPORTED:
        /* Range : 0, 1 */
        if(((unsigned int)value) != 0 &&
            ((unsigned int)value) != 1) 
            useDefault = 1;
        break;
    case CUSTID_SSP_SUPPORTED:
        /* Range : 0, 1 */
        if(((unsigned int)value) != 0 &&
            ((unsigned int)value) != 1) 
            useDefault = 1;
        break;
    case CUSTID_HFP_SDP_CACHE:
        /* Range : 0 - 2 */
        if(((unsigned int)value) > 2)
            useDefault = 1;
        break;
    case CUSTID_HFP_SUPPORT_MASK:
        /* Range : 0 - 3 */
        if (((unsigned int)value) > 3)
            useDefault = 1;
        break;   
    case CUSTID_HFP_VOICE_RECOGNITION:
        /* Range : 0 - 1 */
        if (((unsigned int)value) > 1)
            useDefault = 1;
        break;          
    case CUSTID_HFP_MSBC_CODEC:
        /* Range : 0 - 1 */
        if (((unsigned int)value) > 1)
            useDefault = 1;
        break;        
    case CUSTID_HIGH_SPEED_SUPPORT:
        /* Range : 0 - 1 */
        if(((unsigned int)value) > 1)
            useDefault = 1;
        break;
    case CUSTID_LE_SUPPORT:
        /* Range : 0 - 1 */
        if(((unsigned int)value) > 1)
            useDefault = 1;
        break;
    case CUSTID_DEV_SUPPORT_SERVICES:
        /* Range : U16 array list with 0 terminator */
        if(value == NULL)
            useDefault = 1;
        break;
    case CUSTID_SDP_DELAY_TIMEOUT:
        /* Range : 0 - 3000 */
        if(((unsigned int)value) > 3000) 
            useDefault = 1;
        break;
    case CUSTID_INQUIRY_TX_POWER:
        /* Range : -70 - 20 */
        if(value == NULL)
            useDefault = 1;
        break;
    case CUSTID_INQUIRY_LENGTH:
        /* Range : 1 - 255 */
        if(value == NULL)
            useDefault = 1;
        break;
    case CUSTID_INQUIRY_RESPONSE:
        /* Range : 1 - 255 */
        if(value == NULL)
            useDefault = 1;                
        break;       
    case CUSTID_NO_ROLE_SWITCH:
        /* Range : (0, 1) */
        if(value == NULL)
            useDefault = 1;                
        break;
    case CUSTID_KEEP_AWAKE_TIME:
        /* Range : >= 0 */
        if(value == NULL)
            useDefault = 1;
        break;
    case CUSTID_LE_INITIATE_INTERVAL:
        /* Range : 0x4 - 0x4000 */
        if(value == NULL)
            useDefault = 1;
        break;
    case CUSTID_LE_INITIATE_WINDOW:
        /* Range : 0x4 - 0x4000 */
        if(value == NULL)
            useDefault = 1;
        break;
    case CUSTID_LE_SCAN_INTERVAL:
        /* Range : 0x4 - 0x4000 */
        if(value == NULL)
            useDefault = 1;
        break;
    case CUSTID_LE_SCAN_WINDOW:
        /* Range : 0x4 - 0x4000 */
        if(value == NULL)
            useDefault = 1;
        break;
    case CUSTID_LE_CONN_INTERVAL_MIN:
        /* Range : 0x6 - 0x0C80 */
        if((((unsigned int)value) < 0x0006) ||
            (((unsigned int)value) > 0x0C80)) 
            useDefault = 1;
        break;
    case CUSTID_LE_CONN_INTERVAL_MAX:
        /* Range : 0x6 - 0x0C80 */
        if((((unsigned int)value) < 0x0006) ||
            (((unsigned int)value) > 0x0C80))
            useDefault = 1;
        break;
    case CUSTID_LE_CONN_LATENCY:
        /* Range : 0x0 - 0x01F3 */
        if(((unsigned int)value) > 0x01F3)
            useDefault = 1;
        break;
    case CUSTID_LE_SUPERVISION_TO:
        /* Range : 0xA - 0x0C80 */
        if((((unsigned int)value) < 0x000A) ||
            (((unsigned int)value) > 0x0C80))
            useDefault = 1;
        break;        
    default:
        bt_prompt_trace(MOD_BT, "unknown cust id : %d", id);
        break;
    }
    if(useDefault){
        bt_prompt_trace(MOD_BT, "[CUST] check value(%x) failed with config %s default value 0x%x used", value, custItem[id].name, custItem[id].default_value);
        return custItem[id].default_value;
    }
    return value;
}

static int g_inited = 0;
static void cust_log(const char *log){
    bt_prompt_trace(MOD_BT, log);
}

static int initBTCust(){
    int size = sizeof(custItem)/sizeof(custItem[0]);
    int i;

    bt_prompt_trace(MOD_BT, "[CUST] initBTCust");
    initCust(cust_log);
    for(i = 0;i < size;i++){
        custItem[i].cid = getCustID(custItem[i].name);
        bt_prompt_trace(MOD_BT, "[CUST] %d, %s : cid=%d", i, custItem[i].name, custItem[i].cid);
    }
    g_inited = 1;
    return 1;
}

void *btGetCustvalue(CUST_ID id){
    BT_CUST_ID cid;
    unsigned int value;

    Assert(id < NUM_OF_CUSTID);
    if(!g_inited){
        initBTCust();
    }

    cid = custItem[id].cid;
    if( cid == BT_CUST_UNKNOWN || getCustValue(cid, &value) == 0 ){
        value = (unsigned int)custItem[id].default_value;
        bt_prompt_trace(MOD_BT, "[CUST] btGetCustvalue failed : id=%u, cid=%u, use default value 0x%x", id, cid, value);
    }else{
        value = (unsigned int)checkCustvalue(id, (void*)value);
        bt_prompt_trace(MOD_BT, "[CUST] return cust(%s) value(0x%x) : cid=%u", custItem[id].name, value, cid);
    }
    return (void*)value;
}

void *btGetDevCustvalueById(DEV_ID did, CUST_ID id){
    BT_CUST_ID cid;
    unsigned int value;
    
    Assert(id < NUM_OF_CUSTID);
    if(!g_inited){
        initBTCust();
    }

    cid = custItem[id].cid;
    if( getDevCustValue(did, cid, &value) == 0){
        value = (unsigned int)custItem[id].default_value;
        bt_prompt_trace(MOD_BT, "[CUST] btGetDevCustvalueById failed : did=%d, cid=%d", did, cid);
    }else{
        value = (unsigned int)checkCustvalue(id, (void*)value);
        bt_prompt_trace(MOD_BT, "[CUST] return dev(%u) cust(%s) value(0x%x) : cid=%u", did, custItem[id].name, value, cid);
    }
    return (void*)value;
}

DEV_ID btGetDevID(const char *dev_name){
    if(!g_inited){
        initBTCust();
    }
    return getDeviceID(dev_name);
}

void *btGetDevCustvalue(const char *dev_name, CUST_ID id){
    BT_DEVICE_ID did;

    Assert(id < NUM_OF_CUSTID);
    if(!g_inited){
        initBTCust();
    }
    
    did = getDeviceID(dev_name);
    return btGetDevCustvalueById(did, id);
}

void dumpCustTable(void){
    int size = sizeof(custItem)/sizeof(custItem[0]);
    int i;
    bt_prompt_trace(MOD_BT, "[CUST] dumpCustTable");
    for(i = 0;i < size;i++){
        bt_prompt_trace(MOD_BT, "[CUST] %d : name=%s, def=0x%x, cid=%u", 
            i,
            custItem[i].name,
            custItem[i].default_value,
            custItem[i].cid);
    }
}
