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

#include "bt_common.h"
#include "bt_feature.h"
#ifdef __MTK_STP_SUPPORTED__ 
  #ifndef BTMTK_ON_LINUX
#include "mtkstp.h"
  #endif
#endif

#if defined (__BTMODULE_MT6236__)
#define BT_Transport_TurnOnPower(a, b)       BTIF_TurnOnPower(a, b)
#define BT_Transport_Open(a, b)              BTIF_Open(a, b)
#define BT_Transport_SetDCBConfig(a, b, c)   BTIF_SetDCBConfig(a, b, c)
#define BT_Transport_Close(a, b)             BTIF_Close(a, b)
#define BT_Transport_GetBytes(a, b, c, d, e) BTIF_GetBytes(a, b, c, d, e)
#define BT_Transport_PutBytes(a, b, c, d)    BTIF_PutBytes(a, b, c, d)
#elif defined (__MTK_STP_SUPPORTED__)
/* In MT6616: BT Host only uses the following two APIs provided by the task MOD_WMT */
#define BT_Transport_GetBytes(a, b, c, d, e) mtkstp_receive_data(b, c, BT_TASK_INDX)
#define BT_Transport_PutBytes(a, b, c, d)    mtkstp_send_data(b, c, BT_TASK_INDX)
/* The following three APIs will be disallowed to be invoked in MT6616 */
/* BT Host -> requests BT Controller to Power On: radiomod.c will invoke WMT's PowerOn API to execute UART_Open */
/* BT Host -> requests BT Controller to Power Off: radiomod.c will invoke WMT's PowerOff API to execute UART_Close */
#define BT_Transport_Open(a, b)              NULL
#define BT_Transport_Close(a, b)             NULL
#define BT_Transport_SetDCBConfig(a, b, c)   NULL
#define BT_Transport_TurnOnPower(a, b) NULL
#else
#define BT_Transport_TurnOnPower(a, b)       UART_TurnOnPower(a, b)
#define BT_Transport_Open(a, b)              UART_Open(a, b)
#define BT_Transport_SetDCBConfig(a, b, c)   UART_SetDCBConfig(a, b, c)
#define BT_Transport_Close(a, b)             UART_Close(a, b)
#define BT_Transport_GetBytes(a, b, c, d, e) UART_GetBytes(a, b, c, d, e)
#define BT_Transport_PutBytes(a, b, c, d)    UART_PutBytes(a, b, c, d)
#endif

