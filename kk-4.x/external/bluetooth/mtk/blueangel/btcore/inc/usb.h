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

#ifndef __USB_H
#define __USB_H

/***************************************************************************
 *
 * File:
 *     $Workfile:usb.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains information peculiar to the interface between
 *     the HCI Driver and the RS232 HCI Transport Driver.
 *
 * Created:
 *     February 2, 2000
 *
 * Copyright 2000-2005 Extended Systems, Inc.
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

#include "bttypes.h"

/*---------------------------------------------------------------------------
 * USB Driver API layer
 *
 *     This layer defines the API between the generic USB transport driver
 *     and a platform-specific USB hardware driver. If you are porting
 *     the stack to a new hardware platform that supports USB, you must
 *     provide the USB functions described here.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * UsbEndPoint type
 *
 *     Describes the various types of data pipes used to communicate with
 *     a USB device. Values begin at 0 and are contiguous.
 */
typedef U8 UsbEndPoint;

#define USB_CONTROL_WRITE   0   /* Command packets from host to controller */
#define USB_INTERRUPT_READ  1   /* Event packets from controller to host */
#define USB_BULK_READ       2   /* Bulk read */
#define USB_BULK_WRITE      3   /* Bulk write */

#if (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS > 0)
/*
 * Group: If ISO pipes are available, set MAX_PIPES to 6 so these
 * pipes can be used. Otherwise leave MAX_PIPES at 4 to avoid using them.
 */
#define USB_ISO_READ        4   /* Isochronous read */
#define USB_ISO_WRITE       5   /* Isochronous write */

#define MAX_PIPES           6   /* Total number of pipes */

#else /* (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS > 0) */
#define MAX_PIPES           4   /* Total number of pipes */
#endif /* (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS > 0) */ 
/* End of UsbEndPoint */

/*---------------------------------------------------------------------------
 * UsbCallback type
 *
 *    This callback type is used to indicate callback events to the USB
 *    transport driver.
 */
typedef BOOL(*UsbCallback) (void);

/* End of UsbCallback */

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * USB_Init()
 *
 *     Initializes the USB driver. This should cause all tasks/interrupts
 *     to be set up as necessary.
 *
 * Returns:
 *     TRUE - Initialization was successful.
 *
 *     FALSE - Ports or other resources could not be allocated.
 */
BOOL USB_Init(void);

/*---------------------------------------------------------------------------
 * USB_Deinit()
 *
 *     Deinitializes the USB driver, freeing all ports and resources.
 */
void USB_Deinit(void);

/*---------------------------------------------------------------------------
 * USB_Read()
 *
 *     Reads a block of data from a USB port.
 *
 * Parameters:
 *     type   - Type of pipe to read from
 *
 *     buffer - Buffer in which to write data
 *
 *     length - Length of available data in "buffer"
 *
 * Returns:
 *     integer - Number of bytes actually read
 */
int USB_Read(UsbEndPoint type, U8 *buffer, I16 length);

/*---------------------------------------------------------------------------
 * USB_Write()
 *
 *    Starts writing a block of data to the USB port.
 *
 * Parameters:
 *     type   - Type of pipe to write to
 *
 *     buffer - Buffer of data to write
 *
 *     length - Length of "buffer"
 *
 *     notify - Indicates whether the pipe's notification callback should
 *         be called (this overrides any initial state provided by
 *         USB_RegisterCallback).
 *
 * Returns:
 *     number of bytes written
 */
U32 USB_Write(UsbEndPoint type, U8 *buffer, U32 length, U8 notify);

/*---------------------------------------------------------------------------
 * USB_RegisterCallback()
 *
 *     Registers a callback for the specified pipe. For read pipes, the
 *     callback should be called whenever read data is available. For write
 *     pipes, the callback should be called whenever a notifiable write
 *     is completed (see USB_Write).
 *
 * Parameters:
 *     type - Type of pipe for which this callback applies.
 *
 *     func - Callback function to call.
 *
 *     notify - Initial notification status. If FALSE, do not immediately
 *         signal the event.
 */
void USB_RegisterCallback(UsbEndPoint type, UsbCallback func, BOOL notify);

#endif /* __USB_H */ 

