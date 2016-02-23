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
 * Filename:
 * ---------
 * bt.h
 *
 * Project:
 * --------
 * MAUI
 *
 * Description:
 * ------------
 * This file is the global include file for all MTK Bluetooth modules.
 *
 * Author:
 * -------
 * Brad Chang
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * Jul 11 2009 mtk02126
 * [MAUI_01703270] Modify traces
 * 
 *
 * Sep 19 2008 mtk01239
 * [MAUI_01198727] MTK:Bluetooth_handset popup "device busy" and play video handset no sound .
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
 * Feb 19 2008 mtk01411
 * [MAUI_00621026] [Bluetooth] Check in JSR82 codes
 * 
 *
 * Oct 22 2007 mtk01239
 * [MAUI_00563448] [Bluetooth] patch BT code.
 * 
 *
 * Oct 22 2007 mtk01239
 * [MAUI_00563448] [Bluetooth] patch BT code.
 * 
 *
 * Jul 16 2007 mtk01239
 * [MAUI_00417220] [EM] Bluetooth RF Test Item Support
 * 
 *
 * Jan 5 2007 mtk01239
 * [MAUI_00354875] [Bluetooth] Provide a at command to query bt status for MP testing procedure
 * 
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Oct 31 2006 mtk01411
 * [MAUI_00338541] [1] Fatal Error (c05): Cancel another guy's event 1=c05 2=86854a8 - BT
 * Bug fix to avoid starting a timer during BT power off procedure
 *
 * Oct 10 2006 MTK00758
 * [MAUI_00017092] [BT]Modify test mode procedure
 * 
 *
 * Sep 3 2006 MTK00758
 * [MAUI_00326545] [BT][Factory mode]Check in
 * 
 *
 * May 8 2006 MTK00758
 * [MAUI_00192324] [BT]Resovle reset issue
 * 
 *
 * May 1 2006 MTK00758
 * [MAUI_00191069] [New feature]Modify L4C and MTK Bluetooth interface to common interface
 * 
 *
 * Apr 17 2006 MTK00758
 * [MAUI_00188141] [Bluetooth]Power on/off and reset procedure
 * 
 *
 * Apr 3 2006 mtk00758
 * [MAUI_00184485] [New feature] Add task to simulate UART driver for unitest
 * 
 *
 * Mar 11 2006 MTK00303
 * [MAUI_00178684] [Bluetooth]ESI bluetooth protocol stack
 * add bluetooth files
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef _BT_H
#define _BT_H
#if BTMTK_ON_WISE
#include "uart.h"

/***************************************************************************** 
* Define
*****************************************************************************/

/***************************************************************************** 
* Typedef 
*****************************************************************************/

/* global context */
typedef struct
{
    eventid event_id;
    event_scheduler *evsh_sched;    /* Event scheduler of MTK Bluetooth task */
    stack_timer_struct base_timer;  /* Base timer of event scheduler */
    UartCallback UartCallback;
    kal_bool os_init;               /* set to TRUE in OS_Init(); set to FALSE in OS_Deinit() */
    kal_bool entering_poweroff;     /* This flag is turned on when receiving POWER_OFF_REQ: using it to prohibiting to start timer during power off procedure */
    kal_bool NotifyEVM;             /* True: notifyEVM is sent to extern queue, False: no notifyEVM */
    kal_uint8 l1_sleep_handle;
    kal_bool UartDataReady;         /* True: MSG_ID_BT_DATA_TO_READ_IND is sent to extern queue */
    U8 reset_count;
    kal_bool is_reset;              /* True: is in reset state */
    kal_bool is_poweron;            /* True: is in poweron state */
    kal_bool is_factory_mode;       /* True: is in Factory mode */
    kal_bool is_engineering_mode;   /* TRUE: is in Engineer  Mode */
    U8 bt_em_state;          /* The Engineer Mode State */
    U8 bt_em_operation;      /* The Engineer Mode State */
    U8 *bt_rftest_buffer;
    U8 bt_test_mode;         /* 0: normal mode, 1: enter test mode, 2: leave test mode */
    U8 bt_read_addr;         /* 0: normal mode, 1: power on and read the address */
    U8 power_status;         /* 0: power off, 1: power on */
    kal_mutexid bt_uart_rx_speed;
    U8 uart_rx_slow;
    U8 uart_rx_slow_msg;
    U8 uart_rx_current_setting;
} bt_context_struct;

/***************************************************************************** 
* Extern Global Variable
*****************************************************************************/
extern bt_context_struct *bt_p;

/***************************************************************************** 
* Extern Global Function
*****************************************************************************/
#endif /* BTMTK_ON_WISE */
#endif /* _BT_H */ 

