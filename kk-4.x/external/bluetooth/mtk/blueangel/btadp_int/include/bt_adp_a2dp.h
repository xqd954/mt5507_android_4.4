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
 * bt_adp_a2dp.h
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions which provide 
 *
 * Author:
 * -------
 * Tina Shen
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 12 22 2011 sh.lai
 * [ALPS00103237] [ICS][Bluetooth] Migrate Bluetooth into ALPS 4.0
 * Integration change.
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#ifndef __BT_A2DP_ADP_H_
#define __BT_A2DP_ADP_H_

#include "bluetooth_a2dp_struct.h"

typedef struct _BTA2DPEventActivateCnf {
    U16                      result;
} BTA2DPEventActivateCnf;

typedef struct _BTA2DPEventDeactivateCnf {
    U16                      result;
} BTA2DPEventDeactivateCnf;

typedef struct _BTA2DPEventSignalConnectCnf {
    U16                      connect_id;
    bt_addr_struct   *device_addr;
} BTA2DPEventSignalConnectCnf;

typedef struct _BTA2DPEventSignalConnectInd {
    bt_addr_struct   *device_addr;
    U8                      device_name_size;
    U8                      *device_name;
} BTA2DPEventSignalConnectInd;

typedef struct _BTA2DPEventSignalDisconnectCnf {
    U16                     connect_id;
    bt_addr_struct   *device_addr;
} BTA2DPEventSignalDisconnectCnf;

typedef struct _BTA2DPEventSignalDisconnectInd {
    U16                     connect_id;
} BTA2DPEventSignalDisconnectInd;

typedef struct _BTA2DPEventSepDiscoveryCnf {
    U16                     connect_id;
    U8                      sep_num;
    bt_sep_info_struct      *sep_list;
} BTA2DPEventSepDiscoveryCnf;


typedef struct _BTA2DPEventSepDiscoveryInd {
    U16                     connect_id;
    U8                      sep_num;
    bt_sep_info_struct      *sep_list;
} BTA2DPEventSepDiscoveryInd;

typedef struct _BTA2DPEventCapability {
    U16                         connect_id;
    A2dpCodec                   *audio_cap;
} BTA2DPEventCapability;


typedef struct _BTA2DPEventGetCapabilitiesCnf {
    U16                     connect_id;
} BTA2DPEventGetCapabilitiesCnf;

typedef struct _BTA2DPEventGetCapabilitiesInd {
    U16                     connect_id;
    U8                      acp_seid;
} BTA2DPEventGetCapabilitiesInd;

typedef struct _BTA2DPEventSetConfigurationCnf {
    U16                     connect_id;
    U8                      stream_handle;
} BTA2DPEventSetConfigurationCnf;

typedef struct _BTA2DPEventSetConfigurationInd {
    U16                         connect_id;
    U8                          acp_seid;
    U8                          int_seid;
    U8                          stream_handle;
    A2dpCodec                   *audio_cap;
} BTA2DPEventSetConfigurationInd;

typedef struct _BTA2DPEventSetReConfigurationCnf {
    U8                      stream_handle;
} BTA2DPEventSetReConfigurationCnf;

typedef struct _BTA2DPEventSetReConfigurationInd {
    U8                          stream_handle;
    A2dpCodec                   *audio_cap;
} BTA2DPEventSetReConfigurationInd;

typedef struct _BTA2DPEventOpenStreamCnf {
    U8                      stream_handle;
    bt_addr_struct          *device_addr;
    U8                      device_name[60];
    U8                      device_name_len;
} BTA2DPEventOpenStreamCnf;

typedef struct _BTA2DPEventOpenStreamInd {
    U8                      stream_handle;
    bt_addr_struct          *device_addr;
    U8                      device_name[60];
    U8                      device_name_len;
} BTA2DPEventOpenStreamInd;

typedef struct _BTA2DPEventStartStreamCnf {
    U8                      stream_handle;
    U16                     prefer_size;
    bt_a2dp_audio_cap_struct config;
} BTA2DPEventStartStreamCnf;

typedef struct _BTA2DPEventStartStreamInd {
    U8                      stream_handle;
    U16                     prefer_size;
    bt_a2dp_audio_cap_struct config;
} BTA2DPEventStartStreamInd;

typedef struct _BTA2DPEventSuspendStreamCnf {
    U8                      stream_handle;
} BTA2DPEventSuspendStreamCnf;

typedef struct _BTA2DPEventSuspendStreamInd {
    U8                      stream_handle;
} BTA2DPEventSuspendStreamInd;

typedef struct _BTA2DPEventCloseStreamCnf {
    U8                      stream_handle;
} BTA2DPEventCloseStreamCnf;

typedef struct _BTA2DPEventCloseStreamInd {
    U8                      stream_handle;
} BTA2DPEventCloseStreamInd;

typedef struct _BTA2DPEventAbortStreamCnf {
    U8                      stream_handle;
} BTA2DPEventAbortStreamCnf;

typedef struct _BTA2DPEventAbortStreamInd {
    U8                      stream_handle;
} BTA2DPEventAbortStreamInd;

typedef struct _BTA2DPEventQualityOfServiceInd {
    U8                      stream_handle;
    U8                      qos;
} BTA2DPEventQualityOfServiceInd;


typedef struct _BTA2DPEventStreamDataSendOut {
    U8                      stream_handle;
} BTA2DPEventStreamDataSendOut;

typedef struct _BTA2DPEventStreamDataConfigChange {
    bt_a2dp_audio_cap_struct                   *audio_cap;
} BTA2DPEventStreamDataConfigChange;

typedef struct _BTA2DPEventStreamDataConfigCnf {
    bt_a2dp_audio_cap_struct                   *audio_cap;
} BTA2DPEventStreamDataConfigCnf;


typedef U8 BTA2DPEvent;

#define BTA2DP_EVENT_ACTIVATE_CNF                  0x00
#define BTA2DP_EVENT_DEACTIVATE_CNF                0x01
#define BTA2DP_EVENT_CONNECT_CNF                   0x02
#define BTA2DP_EVENT_CONNECT_IND                   0x03
#define BTA2DP_EVENT_DISCONNECT_CNF                0x04
#define BTA2DP_EVENT_DISCONNECT_IND                0x05
#define BTA2DP_EVENT_DISCOVERY_SEID_CNF            0x06
#define BTA2DP_EVENT_DISCOVERY_SEID_IND            0x07
#define BTA2DP_EVENT_CAPABILITY                    0x08
#define BTA2DP_EVENT_GET_CAPABILITY_CNF            0x09
#define BTA2DP_EVENT_GET_CAPABILITY_IND            0x0A
#define BTA2DP_EVENT_SET_CONFIGURATION_CNF         0x0B
#define BTA2DP_EVENT_SET_CONFIGURATION_IND         0x0C
#define BTA2DP_EVENT_SET_RECONFIGURATION_CNF       0x0D
#define BTA2DP_EVENT_SET_RECONFIGURATION_IND       0x0E
#define BTA2DP_EVENT_STREAM_OPEN_CNF               0x0F
#define BTA2DP_EVENT_STREAM_OPEN_IND               0x10
#define BTA2DP_EVENT_STREAM_START_CNF              0x11
#define BTA2DP_EVENT_STREAM_START_IND              0x12
#define BTA2DP_EVENT_STREAM_SUSPEND_CNF            0x13
#define BTA2DP_EVENT_STREAM_SUSPEND_IND            0x14
#define BTA2DP_EVENT_STREAM_CLOSE_CNF              0x15
#define BTA2DP_EVENT_STREAM_CLOSE_IND              0x16
#define BTA2DP_EVENT_STREAM_ABORT_CNF              0x17
#define BTA2DP_EVENT_STREAM_ABORT_IND              0x18
#define BTA2DP_EVENT_STREAM_QOS_IND                0x19
#define BTA2DP_EVENT_STREAM_DATA_OUT               0x1A
#define BTA2DP_EVENT_STREAM_START_IND_CNF       0x1B
#define BTA2DP_EVENT_STREAM_DATA_CONFIG_CHANGE     0x1C
#define BTA2DP_EVENT_STREAM_DATA_CONFIG_CNF     0x1D

struct _BTA2DPCallbackParms {
    BTA2DPEvent    event;   /* Event associated with the callback       */
    U16            result;
    union {
        BTA2DPEventActivateCnf              A2DPEventActivateCnf;
        BTA2DPEventDeactivateCnf            A2DPEventDeactivateCnf;
        BTA2DPEventSignalConnectCnf         A2DPEventSignalConnectCnf;
        BTA2DPEventSignalConnectInd         A2DPEventSignalConnectInd;
        BTA2DPEventSignalDisconnectCnf      A2DPEventSignalDisconnectCnf;
        BTA2DPEventSignalDisconnectInd      A2DPEventSignalDisconnectInd;
        BTA2DPEventSepDiscoveryCnf          A2DPEventSepDiscoveryCnf;
        BTA2DPEventSepDiscoveryInd          A2DPEventSepDiscoveryInd;
        BTA2DPEventCapability               A2DPEventCapability;        
        BTA2DPEventGetCapabilitiesCnf       A2DPEventGetCapabilitiesCnf;
        BTA2DPEventGetCapabilitiesInd       A2DPEventGetCapabilitiesInd;
        BTA2DPEventSetConfigurationCnf      A2DPEventSetConfigurationCnf;
        BTA2DPEventSetConfigurationInd      A2DPEventSetConfigurationInd;
        BTA2DPEventSetReConfigurationCnf    A2DPEventSetReConfigurationCnf;
        BTA2DPEventSetReConfigurationInd    A2DPEventSetReConfigurationInd;
        BTA2DPEventOpenStreamCnf            A2DPEventOpenStreamCnf;
        BTA2DPEventOpenStreamInd            A2DPEventOpenStreamInd;
        BTA2DPEventStartStreamCnf           A2DPEventStartStreamCnf;
        BTA2DPEventStartStreamInd           A2DPEventStartStreamInd;
        BTA2DPEventSuspendStreamCnf         A2DPEventSuspendStreamCnf;
        BTA2DPEventSuspendStreamInd         A2DPEventSuspendStreamInd;
        BTA2DPEventCloseStreamCnf           A2DPEventCloseStreamCnf;
        BTA2DPEventCloseStreamInd           A2DPEventCloseStreamInd;
        BTA2DPEventAbortStreamCnf           A2DPEventAbortStreamCnf;
        BTA2DPEventAbortStreamInd           A2DPEventAbortStreamInd;
        BTA2DPEventQualityOfServiceInd      A2DPEventQualityOfServiceInd;
        BTA2DPEventStreamDataSendOut     A2DPEventStreamDataSentOut;
        BTA2DPEventStreamDataConfigChange     A2DPEventStreamDataConfigChange;
        BTA2DPEventStreamDataConfigCnf     A2DPEventStreamDataConfigCnf;
    } p;
};

typedef struct _BTA2DPCallbackParms BTA2DPCallbackParms;

typedef void (*BTA2DPCallback)(BTA2DPCallbackParms *Parms);

/* Exported API's */
extern void A2DP_Register(BTA2DPCallback A2DPCallback);
extern void bt_a2dp_adp_init(void);
extern void bt_a2dp_send_stream_handler(U8 stream_handle, A2DP_codec_struct *codec);

#endif 



