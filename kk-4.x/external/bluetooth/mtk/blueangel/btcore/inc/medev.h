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

#ifndef __MEDEV_H
#define __MEDEV_H

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

#include "medev_adp.h"

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/
U8 DS_GetDDBNumber(void);

/*---------------------------------------------------------------------------
 * DS_AddDevice()
 *
 *     Add a device to the dynamic list of known devices. A successful call
 *     to this function will generate a BTEVENT_DEVICE_ADDED event to the
 *     device selection monitor handler.
 *
 * Parameters:
 *     tmplt - A BtDeviceContext structure which provides all the known
 *         information about the device to add. Unknown fields must be
 *         initialized to zero. At a minimum, the "addr" field must be set.
 *
 *     newDevice - A pointer to a BtDeviceContext pointer that receives the
 *         newly added device.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The device has been added to the list and a
 *         reference returned in "newDevice".
 *
 *     BT_STATUS_FAILED - The device is already in the list.
 *
 *     BT_STATUS_NO_RESOURCES - The device list is full.
 *
 *     BT_STATUS_INVALID_PARM - The template parameter was invalid.
 */
BtStatus DS_AddDevice(const BtDeviceContext *tmplt, BtDeviceContext **newDevice);

/*---------------------------------------------------------------------------
 * DS_DeleteDevice()
 *
 *     Remove a device from the known device list. If the device is also in
 *     the security database, this function will not remove it from the
 *     security database. A successful call to this function will generate
 *     a BTEVENT_DEVICE_DELETED event to the device selection monitor handler.
 *
 * Parameters:
 *     addr - The 48-bit Bluetooth address of the device to remove.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The device was found and removed.
 *
 *     BT_STATUS_NOT_FOUND - The device was not found.
 *
 *     BT_STATUS_INVALID_PARM - The "addr" parameter was invalid.
 */
BtStatus DS_DeleteDevice(const BD_ADDR *addr);

/*---------------------------------------------------------------------------
 * DS_EnumDeviceList()
 *
 *     Enumerates all known devices.
 *
 * Parameters:
 *     lastDevice - A pointer to a BtDeviceContext pointer. The first time
 *         this function is called, "lastDevice" points to NULL. Upon return,
 *         "lastDevice" will point to the first element in the list of
 *         known devices.
 *
 *         On subsequent calls, "lastDevice" should point to the element
 *         provided by the prior DS_EnumDeviceList call.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The next device has been stored in "lastDevice".
 *
 *     BT_STATUS_FAILED - The device pointed to by lastDevice is not on the
 *         device list.
 *
 *     BT_STATUS_NOT_FOUND - There are no more devices on the list.
 *
 *     BT_STATUS_INVALID_PARM - The lastDevice parameter was invalid.
 */
BtStatus DS_EnumDeviceList(BtDeviceContext **lastDevice);

/*---------------------------------------------------------------------------
 * DS_FindDevice()
 *
 *     Finds the device in the device list with a matching Bluetooth address.
 *
 * Parameters:
 *     addr - The 48-bit Bluetooth address of the device to find.
 *
 * Returns:
 *     A pointer to the matching device, or zero if no device was found.
 */
BtDeviceContext *DS_FindDevice(const BD_ADDR *addr);

#endif /* __MEDEV_H */ 

