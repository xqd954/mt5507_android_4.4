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

#ifndef __ABCSPHCI_H
#define __ABCSPHCI_H

/***************************************************************************
 *
 * File:
 *     $Workfile:abcsphci.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains definitions and structures specific
 *     to the ABCSP HCI transport driver.
 *
 * Created:
 *     December 21, 1999
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
#include "hcitrans.h"
#include "sys/rxbuff.h"
#include "eventmgr.h"
#include "config_txmsg.h"

/****************************************************************************
 *
 * Section: Configuration Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * UART_RX_BUFF_SIZE constant
 *
 *     The UART receive buffer is used to store data read from the
 *     UART for indication to the ABCSP layer. This buffer can be any
 *     size greater than 1.
 */
#ifndef UART_RX_BUFF_SIZE
#define UART_RX_BUFF_SIZE       64
#endif 

/*---------------------------------------------------------------------------
 * UART_TX_BUFF_SIZE constant
 *
 *     The UART transmit buffer is used to store data from the ABCSP
 *     layer to be written to the UART. This buffer can be any size 
 *     greater than 1.
 */
#ifndef UART_TX_BUFF_SIZE
#define UART_TX_BUFF_SIZE       64
#endif 

/*---------------------------------------------------------------------------
 * TIMER_TCONF_TIMEOUT constant
 *
 *     BCSP Link Establishment Timer: Tconf (default 1 second)
 */
#ifndef TIMER_TCONF_TIMEOUT
#define TIMER_TCONF_TIMEOUT     1000
#endif 

/*---------------------------------------------------------------------------
 * TIMER_TSHY_TIMEOUT constant
 *
 *     BCSP Link Establishment Timer: Tshy (default 1 second) 
 */
#ifndef TIMER_TSHY_TIMEOUT
#define TIMER_TSHY_TIMEOUT      1000
#endif 

/*---------------------------------------------------------------------------
 * TIMER_BCSP_TIMEOUT constant
 *
 *     BCSP Reliable Message Retransmit Timer: (default 0.25 seconds) 
 */
#ifndef TIMER_BCSP_TIMEOUT
#define TIMER_BCSP_TIMEOUT      250
#endif 

/****************************************************************************
 *
 * Section: Types and Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * TXMSG type
 *
 *     We statically allocate TXMSG structures to eliminate the need for
 *     malloc() support. Since txmsg.c defines the TXMSG structure locally
 *     we use this typedef of the same size locally. If the structure size
 *     grows in the future, xa_malloc() will detect this and Assert.
 */
typedef struct txmsg
{
    void *next;
    void *m;
    unsigned chan:4;
    unsigned seq:3;
} TXMSG;

/*---------------------------------------------------------------------------
 * ABCSP_Context structure
 *
 *     The ABCSP HCI transport driver context contains queues and state
 *     information for the HCI-ABCSP transport driver.
 */
typedef struct _ABCSP_Context
{
    /* Callback Entry points */
    TranEntry tranEntry;
    TranCallback callback;

    /* ASBCP initialization state. */
    U8 abcspInit;

    /* True when ABCSP has requested we call abcsp_pumptxmsgs() */
    BOOL pumpTx;

    /* Receive: UART buffer variables */
    U16 rxUartReadOffset;
    U16 rxUartWriteOffset;
    U8 rxUartBuff[UART_RX_BUFF_SIZE];

    /* Transmit: UART buffer variables */
    U16 txUartReadOffset;
    U16 txUartWriteOffset;
    U8 txUartBuff[UART_TX_BUFF_SIZE];

    /* Receive: HCI buffer variables */
    RxBuffHandle rxBuff;
    U16 rxBuffSize;     /* Anticipated total size of rx buffer */
    U16 rxWriteOffset;  /* Write offset into rx buffer */
    U8 rxState;
    U8 rxChannel;       /* ABCSP channel of current rx buffer */
    U8 rxStage[2];      /* Temp buffer to receive HCI conn handle */

    /* Transmit: HCI buffer variables */
    ListEntry txQueue;  /* Packets to send */
    U16 txReadOffset;   /* Number of bytes already read */
    U8 txFrag;          /* Index of fragment to process */

    /*
     * TXMSG pool is used to eliminate the need for dynamic memory.
     * It needs to be large enough to cover relq size + 1 for the unrelq.
     */
    U8 txMsgInuse;
    TXMSG txMsgPool[ABCSP_TXWINSIZE + 1];

    /* Timers used by ABCSP as well as a receive retry timer. */
    struct _timers
    {
        EvmTimer bcsp;
        EvmTimer tshy;
        EvmTimer tconf;
        EvmTimer pumpRx;
    } timers;

#if XA_DEBUG == XA_ENABLED
    ListEntry txEnroute;    /* Debug verify packets held by ABCSP */
#endif 

} ABCSP_Context;

/*
 * Define the appropriate external reference to the transport context,
 * if necessary.
 */
#if (XA_CONTEXT_PTR == XA_ENABLED)
extern ABCSP_Context *xprt;

#define ABCSP(s) (xprt->s)

#elif (XA_CONTEXT_PTR == XA_DISABLED)
extern ABCSP_Context xprt;

#define ABCSP(s) (xprt.s)
#endif 

/* Function Prototypes */
BtStatus ABCSP_Init(TranCallback tranCallback);
BtStatus ABCSP_Shutdown(void);

#endif /* __ABCSPHCI_H */ 

