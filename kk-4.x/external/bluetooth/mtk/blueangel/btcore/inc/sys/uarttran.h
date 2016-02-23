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

#ifndef __UATRAN_H
#define __UATRAN_H

/***************************************************************************
 *
 * File:
 *     $Workfile:uarttran.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains definitions and structures specific
 *     to the UART HCI transport driver.
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
#include "bt_feature.h"

/* MS to wait for HCI events */
#define UTP_UART_EVENT_WAIT_TIME  20000
#define UTP_UART_SLEEP_WAIT_TIME  1000
#define UTP_HOST_SLEEP_TIME 5
#define UTP_CONTROLLER_SLEEP_TIME 4

/* Receive States */
#define RXS_GET_TYPE      1
#define RXS_GET_SEQ       2
#define RXS_GET_HEADER    3
#define RXS_GET_BUFFER    4
#define RXS_GET_DATA      5

/* Transmit States */
#define TXS_IDLE          1
#define TXS_TYPE          2
#define TXS_HEADER        3
#define TXS_DATA          4

/* Packet types or indicators */
#define IND_COMMAND         (U8)0x01
#define IND_ACL_DATA        (U8)0x02
#define IND_SCO_DATA        (U8)0x03
#define IND_EVENT           (U8)0x04
#define IND_ERROR_MSG       (U8)0x05
#define IND_NEGOTIATION     (U8)0x06

#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
#define BT_UART_MT66XX_RETRANSMISSION_OFF   (U8)0x00
#define BT_UART_MT66XX_RETRANSMISSION_ON    (U8)0x01


#define BT_UART_MT66XX_FLUSH_DATA_OFF       (U8)0x00
#define BT_UART_MT66XX_FLUSH_DATA_ON        (U8)0x01


#endif

/*
 * Transport driver context
 * 
 * Contains queues and state information for the HCI-UART
 * transport driver.
 */
typedef struct _UATRAN_AMPTxMessage
{
    U8 total_number;
    void *ptr;
} UATRAN_AMPTxMessage;



typedef struct _UATRAN_Context
{

    /* Transmit queues */
    ListEntry txQueue;
    ListEntry txEnroute;

    /* Callback Entry points */
    TranEntry tranEntry;
    TranCallback callback;

    /* RX State variables */
    U16 maxReadLen;
    U16 totalLen;
    U8 buffType;
    U8 *ptr;
    U8 header[4];
    U8 headerLen;
    RxBuffHandle rxBuffHandle;
    U8 rxState;
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
    U8 mt66xxTransmissionFlag;
    U8 uartFlushAllRxDataFlag;
    EvmTimer hciAckTimer;
#endif
    U16 txPacketOffset;
    /* TX State variables */
    HciPacket *txPacket;
    HciPacket *pendingTxPacket;
    U8 txState;
    const U8 *txBp;     /* pointer to location to write from */
    U16 txLen;          /* num bytes in buffer to write */
    U8 txType;          /* type of packet we're transmitting */
    U8 txFrag;          /* index of fragment to process */
    EvmTimer watchdog;  /* Watchdog timer for HCI events */
    U8 unackedCommand;  /* TRUE when there is an unacknowledged command */
    U8 txrx;            /* TRUE when there is one tx or rx operation */
    U8 hostSleep;       /* True: host enter sleep mode */
    U32 uartLastTxRx;
#ifdef __ENABLE_SLEEP_MODE__ 
    U8 wakeupCmdMade;
    U8 wakeupCounter;
    EvmTimer wakeupControllerTimer;
#endif /* __ENABLE_SLEEP_MODE__ */
    EvmTimer verifyHostSleepTimer;
    U8 wakeupFactorMode;
    UATRAN_AMPTxMessage ampTxData;

#ifdef __KEEP_HOST_AWAKE__
    EvmTimer awakeTimer;
    U8 awake_wakelock_acquired;      /* Indicate if wake lock is acquired */
    U32 awakeTimeout;
#endif
} UATRAN_Context;

/*
 * Define the appropriate external reference to the transport context,
 * if necessary.
 */
#if (XA_CONTEXT_PTR == XA_ENABLED)
extern UATRAN_Context *uaxprt;

#define UATRAN(s) (uaxprt->s)

#elif (XA_CONTEXT_PTR == XA_DISABLED)
extern UATRAN_Context uaxprt;

#define UATRAN(s) (uaxprt.s)
#endif 

/* Function prototypes */
BtStatus UARTTRAN_Init(TranCallback tranCallback);
BtStatus UARTTRAN_Shutdown(void);
void UARTTRAN_ResetRxState(void);
#ifdef __BT_MT66XX_HCI_RETRANSMISSION__
void UARTTRAN_EnableMT66xxFlowControl(void);
#endif
#endif /* __UATRAN_H */ 

