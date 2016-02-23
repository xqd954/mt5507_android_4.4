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

#ifndef __MEDEV_ADP_H
#define __MEDEV_ADP_H

/****************************************************************************
 *
 * File:
 *     $Workfile:medev.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the Bluetooth
 *     Device Selection functionality of the Management Entity.
 * 
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,
 * Inc.  This work contains confidential and proprietary information of
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "me_adp.h"
#include "sdp_adp.h"
#include "bt_feature.h"

/*---------------------------------------------------------------------------
 * Device Selection (DS) layer
 *
 *     The Bluetooth Device Selector API is a part of the
 *     Management Entity. It provides the functionality necessary to
 *     keep track of all devices within the operating range
 *     of the local radio (the "known device list").
 * 
 *     The known device list is a dynamic list of devices found during
 *     inquiry and from the security database. Information about devices
 *     in the known device list are found in BtDeviceContext structures which
 *     are accessible via the DS_ APIs. The Device Selector API also provides
 *     functions for enumerating the list of known devices, and requesting
 *     selection of the most appropriate device via an external "device
 *     selection monitor".
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtDeviceStatus type
 *
 *     The Bluetooth Device Status tracks the status of each device.
 */

typedef enum 
{
    BDS_NONE              = 0x00,
    BDS_BONDING           = 0x01,
    BDS_CONNECTING        = 0x02,
    BDS_NAME_REQESTING    = 0x04
}BtDeviceStatus;

/* End of BtDeviceStatus */

/*---------------------------------------------------------------------------
 * BtDeviceQualityMask type
 *
 *     The Bluetooth Device Quality Mask is a set of flags used with the
 *     BtDeviceQuality structure to indicate which fields in the structure
 *     have been set.
 */
typedef U8 BtDeviceQualityMask;

#define BDQM_NONE                0x00
#define BDQM_SERVICE_CLASS       0x01
#define BDQM_DEVICE_CLASS        0x02
#define BDQM_SERVICE_UUID        0x04
#define BDQM_DEVICE_STATUS       0x08

/* End of BtDeviceQualityMask */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * BtDeviceContext structure
 *
 *      This Bluetooth Device context structure is used to keep track of
 *      each known device. The structure also contains information about
 *      the device that is used when determining the best device to connect
 *      to and what parameters to use when establishing an ACL connection.
 *
 *      All fields in this structure are public but should be treated as
 *      read-only! The single exception is the "context" field which can be
 *      modified by the device selection manager.
 */
struct _BtDeviceContext
{
    ListEntry node; /* Internal, do not use. */

    /* Group: The following fields are valid for all known devices */


    /* The 48-bit Bluetooth device address of this device. */
    BD_ADDR addr;

    /*
     * The Page Scan Information received during the most recent inquiry
     * result from this device.
     */
    BtPageScanInfo psi;

    /*
     * The Class of Device setting received during the most recent inquiry
     * result or connection request from this device.
     */
    BtClassOfDevice classOfDevice;

#if DS_NUM_SERVICES > 0
    /* The list of Service Class UUIDs known to exist on this device. */
    SdpServiceClassUuid services[DS_NUM_SERVICES];
#endif /* DS_NUM_SERVICES > 0 */ 

    /*
     * If there is an active ACL connection to this device, this field 
     * contains a pointer to the matching remote device structure.
     */
    BtRemoteDevice *link;
    BtDeviceStatus under_used;
    /*
     * Reserved for use by the device selection monitor handler to
     * store extended information about this device. The ME
     * does not access or modify this field. This is the only field in
     * this structure that may be modified outside of the ME.
     */
    void *context;
#if defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__)
    U8 rssi;
    U8 support_eir;
    U8 support_service_list;
    U8 device_name_length;
    U8 *device_name;
    U32 eir_response[6];
#endif /* defined (__BT_2_1_ENTENDED_INQUIRY_RESPONSE__) */ 
#if defined (__BT_2_1_SIMPLE_PAIRING__)
    U8 gapKeyType;
#endif 
#ifdef __BT_4_0_BLE__
    BtDevType devType;
    LeAddrType addrType;
    LeAdFlags adFlag;
#ifdef __BT_4_0_ACTIVE_SCAN__    
    U8 cachedData[MAX_CACHED_EIR_DATA_LEN];
    U8 cachedDataLen;
#endif    
#endif
    /* === Internal Use Only === */
    ListEntry active;

} ;

/*---------------------------------------------------------------------------
 * BtSelectDeviceToken structure
 *
 *     The Select Device Token is used when requesting the Bluetooth
 *     device selection manager to select an appropriate device based
 *     on the device quality criteria.
 */
typedef struct _BtSelectDeviceToken
{
    ListEntry node; /* === Internal Use Only === */

    /*
     * The caller must set the callback function. It is called
     * when the command is complete.
     */
    BtCallBack callback;

    /*
     * The result field is available when the request is complete. If the
     * errCode of the request indicated success, a pointer to the selected
     * device is provided here.
     */
    BtDeviceContext *result;

    /* === Internal Use Only === */
    U8 state;

} BtSelectDeviceToken;
#endif /* __MEDEV_ADP_H */
