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

#ifndef _UART_H_
#define _UART_H_

/***************************************************************************
 *
 * File:
 *     $Workfile:uart.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Header file for UART hardware driver.
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

#include "bttypes.h"

/*---------------------------------------------------------------------------
 * UART Driver API layer
 *
 *     The UART hardware driver is a set of functions used by the UART
 *     transport to exchange data with Bluetooth radio hardware. If you
 *     are porting the stack to a new hardware platform, you must provide
 *     the UART functions described here.
 *
 *     If your system will require an interrupt service routine (ISR) to
 *     handle UART receive and transmit events, we recommend that you create
 *     a high-priority driver thread. When an event occurs, the ISR should
 *     simply signal the thread, then exit. This minimizes the time spent
 *     in interrupt mode. The driver thread then reads or writes data and
 *     indicates events as described below.
 *
 *     If it is not possible to create a separate, high-priority driver
 *     thread, the interrupt may issue the required callback events itself.
 *     UART_Read and UART_Write must then be designed to operate in interrupt
 *     mode (UART_Write is also called from the stack task.)
 *
 *     Some operating systems take care of the ISR internally, and provide
 *     an API to read and write UART data. In this case, the high-priority
 *     driver thread receives its signals from the operating system instead of
 *     a custom ISR.
 *
 *     If a high-priority thread is used, OS_StopHardware and OS_ResumeHardware
 *     can be implemented using a mutual-exclusion semaphore, as shown
 *     in the sample Windows code. If only an ISR is used, then
 *     OS_StopHardware and OS_ResumeHardware should disable/re-enable
 *     UART-related interrupts.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * UartEvent type
 *
 *     UART events are provided to the stack through the UartCallback.
 */
typedef U8 UartEvent;

/* Indicates that data is available and can be read with UART_Read. */
#define UE_DATA_TO_READ          1

/*
 * Indicates that the data transmission requested by UART_Write is
 * complete.
 */
#define UE_WRITE_COMPLETE        2

/*
 * Inidicates that the UART has completed initialization. Note that the
 * hardware resource (see OS_StopHardware) should not be allocated
 * by the UART hardware driver when delivering this callback. 
 */
#define UE_INIT_COMPLETE         3

/*
 * Indicates that the UART has completed shutdown. Note that the
 * hardware resource (see OS_StopHardware) should not be allocated
 * by the UART hardware driver when delivering this callback. 
 */
#define UE_SHUTDOWN_COMPLETE     4

/* End of UartEvent */

/*---------------------------------------------------------------------------
 * UartCallback type
 *
 *     A UartCallback function is provided to the UART hardware driver.
 *     When data is available to be read, the UART driver must call the
 *     callback function with an event of
 *     UE_DATA_TO_READ. The stack will immediately call UART_Read
 *     repeatedly until all data has been read.
 *
 *     The UART must also call the callback function with UE_WRITE_COMPLETE
 *     when all data written with UART_Write has been transmitted.
 */
typedef void (*UartCallback) (UartEvent event);

/* End of UartCallback */

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * UART_Init()
 *
 *     Called by the stack (during EVM_Init) to initialize the UART hardware
 *     driver. This function should perform the following tasks:
 *
 *     1) Initialize all local memory.
 *
 *     2) Open and initialize the communications port using OS-specific
 *     calls.
 *
 *     3) Initialize a thread or interrupt service routine to process
 *     communications port events.
 *
 *     4) Register a callback function. When a read or write event occurs, 
 *     the UART hardware driver calls the callback function with the 
 *     appropriate event.
 *
 * Parameters:
 *     func - Callback function to call with events.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Indicates that initialization was successful.
 *
 *     BT_STATUS_PENDING - Indicates that initialization was started and
 *         a notification will be sent when initialization is complete
 *         (UE_INIT_COMPLETE).
 *
 *     BT_STATUS_FAILED - Indicate that the port could not be opened
 *         or that some other initialization error occurred.
 */
BtStatus UART_Init(UartCallback func);

/*---------------------------------------------------------------------------
 * UART_Shutdown()
 *
 *     Called by the stack (during shutdown) to release any resources
 *     allocated by the UART_Init function. This may include closing
 *     the communications port, killing the I/O thread, deregistering
 *     the interrupt service routine, etc.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Indicates that shutdown was successful.
 *
 *     BT_STATUS_PENDING - Indicates that shutdown was started and
 *         a notification will be sent when shutdown is complete
 *         (UE_SHUTDOWN_COMPLETE).
 *
 *     BT_STATUS_FAILED - Indicate that the port could not be closed
 *         or that some other error occurred.
 */
BtStatus UART_Shutdown(void);

/*---------------------------------------------------------------------------
 * UART_Read()
 *
 *     Called by the stack to read bytes from the UART.
 *
 *     When the UART driver first detects that data has been received, it
 *     notifies the stack by calling the callback with the
 *     UE_DATA_TO_READ event. During processing of this callback, the
 *     stack calls UART_Read repeatedly until all available data has
 *     been read.
 *
 *     This function must not block. If no bytes are currently available,
 *     UART_Read returns 0.
 *
 * Parameters:
 *     buffer - Buffer to fill with received bytes
 *
 *     length - Maximum number of bytes to read
 *
 * Returns:
 *     int - The number of bytes actually copied into "buffer". If no
 *         more bytes are available, UART_Read returns 0.
 */
U16 UART_Read(U8 *buffer, U16 length);

/*---------------------------------------------------------------------------
 * UART_Write()
 *
 *     Called by the stack to begin writing bytes to the UART.
 *     This function should not block, but return immediately after
 *     initializing the write.
 *
 *     This function is first called by the stack thread. When the UART
 *     driver determines that the caller's buffer is free, it calls the
 *     callback function with the
 *     UE_WRITE_COMPLETE event. During processing of the event,
 *     UART_Write may be called once to provide more transmit data to the
 *     hardware driver.
 *
 * Parameters:
 *      buffer - Bytes to send.
 *
 *      length - Number of bytes in "buffer".
 *
 * Returns:
 *      int - The number of bytes sent to the UART. Returns 0 if a
 *          transport error occurred.
 *
 */
U16 UART_Write(const U8 *buffer, U16 length);

/*---------------------------------------------------------------------------
 * UART_SetSpeed() 
 *
 *     Called to change the speed of the UART.
 *
 * Parameters:
 *     speed - Speed (in KBits/sec) at which the UART should operate
 *         (57 or 115)
 */
void UART_SetSpeed(U16 speed);

/*---------------------------------------------------------------------------
 * UART_SetFlowControl() 
 *
 *     Called to change the flow control of the UART.
 *
 * Parameters:
 *     ena_flowctrl - 0:none flow control
 *                    1:hw flow control
 */
void UART_SetFlowControl(U8 ena_flowctrl);

#endif /* _UART_H_ */ 

