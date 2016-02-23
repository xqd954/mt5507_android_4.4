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

#ifndef __USTRAN_H
#define __USTRAN_H

/***************************************************************************
 *
 * File:
 *     $Workfile:usbtrans.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *      This file contains definitions and structures specific
 *      to the HCI USB hardware driver.
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

#include "hcitrans.h"
#include "usb.h"

/* Variables for preserving the state of each input endpoint */
typedef struct
{
    U8 state;
#define         USB_RXS_GET_INIT      1
#define         USB_RXS_GET_HEADER    2
#define         USB_RXS_GET_BUFFER    3
#define         USB_RXS_GET_DATA      4
    U8 header[8];
    U8 headerLen;
    U16 totalLen;
    U16 maxReadLen;
    HciBuffer *rxBuffer;
    RxBuffHandle rxBuffHandle;
    U8 *ptr;
} UsbRxStateContext;

/*
 * These defines along with UsbRxStateContext track the input state machines
 * in hcitrans.c
 */
enum
{
    EVENT_STATE,
    ACL_STATE,
    SCO_STATE,
    NUM_RX_STATES   /* Always last */
};

/* 
 *  Macros used by USB transport driver.
 */

#define USB_ReadBulk(buf, len) \
                USB_Read(USB_BULK_READ, buf, len)
#define USB_ReadIso(buf, len) \
                USB_Read(USB_ISO_READ, buf, len)
#define USB_ReadEvent(buf, len) \
                USB_Read(USB_INTERRUPT_READ, buf, len)
#define USB_WriteBulk(buf, len, notify) \
                USB_Write(USB_BULK_WRITE, buf, len, notify)

#define USB_WriteIso(buf, len, notify) \
                USB_Write(USB_ISO_WRITE, buf, len, notify)

#define USB_WriteControl(buf, len, notify) \
                USB_Write(USB_CONTROL_WRITE, buf, len, notify)

/*
 * Note that the 'notify' parameter on the following macros is for the 
 * initial state. For reads it will remain static. Each call to USB_Write()
 * provides a 'notify' parameter that replaces the initial one. 
 */
#define USB_RegisterReadBulkCallback(func) \
                USB_RegisterCallback(USB_BULK_READ, func, TRUE)

#define USB_RegisterReadIsoCallback(func) \
                USB_RegisterCallback(USB_ISO_READ, func, TRUE)

#define USB_RegisterWriteBulkCallback(func) \
                USB_RegisterCallback(USB_BULK_WRITE, func, FALSE)

#define USB_RegisterWriteIsoCallback(func) \
                USB_RegisterCallback(USB_ISO_WRITE, func, FALSE)

#define USB_RegisterReadInterruptCallback(func) \
                USB_RegisterCallback(USB_INTERRUPT_READ, func, TRUE)

#define USB_RegisterWriteControlCallback(func) \
                USB_RegisterCallback(USB_CONTROL_WRITE, func, FALSE)

/*
 * Transport driver context
 * 
 * Contains receive buffers, queues, and state information for the HCI 
 * transport driver.
 */
typedef struct _USTRAN_Context
{

    /*
     * HCITRANS.C variables 
     */

    /* Callback Entry points */
    TranEntry tranEntry;

    /* Lists and Queues */
    ListEntry txQueue;

    /* TX State variables */
    HciPacket *txPacket;
    EvmTimer watchdog;                  /* Watchdog timer for HCI events */
    U8 unackedCommand;                  /* TRUE when there is an unacknowledged command */
    HciPacket *hciPacket;
#if HCI_USB_TX_BUFFER_SIZE > 0
    U8 txBuf[HCI_USB_TX_BUFFER_SIZE];   /* Transmission buffer */
#else 
    U8 txBuf[1];                        /* Avoid compiler error */
#endif 

    /*
     * Variables private to rxDataEvent() 
     */
    UsbRxStateContext rxContext[NUM_RX_STATES];

} USTRAN_Context;

/*
 * Define the appropriate external reference to the transport context,
 * if necessary.
 */
#if (XA_CONTEXT_PTR == XA_ENABLED)
extern USTRAN_Context *usxprt;

#define USTRAN(s) (usxprt->s)

#elif (XA_CONTEXT_PTR == XA_DISABLED)
extern USTRAN_Context usxprt;

#define USTRAN(s) (usxprt.s)
#endif 

/* Function prototypes */
BtStatus USBTRAN_Init(TranCallback tranCallback);
BtStatus USBTRAN_Shutdown(void);

#endif /* __USTRAN_H */ 

