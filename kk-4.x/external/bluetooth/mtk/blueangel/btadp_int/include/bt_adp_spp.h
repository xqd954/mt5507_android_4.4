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

/****************************************************************************
 *
 * File:        sppos.c
 *     $Workfile:sppos.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: 
 *      This the Windows-specific OS SPP Services layer.
 *      It is at a similar level as a BSP (Board Support Package) include
 *      file might be. That is, it is utilized by a higher layer - in this
 *      case, the Serial Port Profile.
 *
 *      Serial Port Profile Layers:
 *          
 *          application
 *          OS Device API
 *          SPP 
 *          OS SPP Services (including this file)
 *          hardware
 *
 * $Project:XTNDAccess Blue SDK$
 *
 * Copyright 1997-2005 Extended Systems, Inc.
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
#ifndef __BT_ADP_SPP_H__
#define __BT_ADP_SPP_H__


#include "spp.h"
#include "btconfig.h"

#include "bluetooth_struct.h" /* CUSTOM_SPP */


/* 
 * bt_adp_spp.h
 * 
 * This file is the header file of Internal Adaptation of SPP profile that receives message from external ADP and use profile API.
 * Flow direction: external ADP API --> message queue --> internal ADP --> profile API
 */

/* Platform dependent includes */

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Serial Port Profile Configuration API layer
 *
 *     The constants in this layer provide general configuration to
 *     communication protocol stacks compiled into the system. To change
 *     a constant, simply #define it in your overide.h include file.
 *
 *     Configuration constants here and in other configuration API files
 *     are used to make the stack more appropriate for a particular
 *     environment. Constants can be modified to allow tradeoffs
 *     between code size, RAM usage, functionality, and throughput.
 *
 *     Some constants are numeric, and others indicate whether a feature
 *     is enabled (defined as XA_ENABLED) or disabled (defined as
 *     XA_DISABLED).
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/


/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/


//extern SPPCallback SPPCB;


/****************************************************************************
 *
 * Function Prototypes
 *
 ****************************************************************************/
extern BOOL SPPDRV_Init(void);
extern void initVp(SPP_DEV_OS *dev);

extern void bt_sppclient_cmgr_event_callback(CmgrHandler *cmgr_hdlr, CmgrEvent event, BtStatus status);
extern void bt_sppserver_cmgr_event_callback(CmgrHandler *cmgr_hdlr, CmgrEvent event, BtStatus status);
extern kal_bool CheckSPPFlowControlSetting(module_type owner);

/* #ifdef CUSTOM_SPP */
extern void SPP_Register(SPPCallback Callback);

/* APP -> SPP Interface */
#if BT_SPP_AUTHORIZE_BY_MMI
extern void SPPHandleConnectIndRes(UART_PORT Port, kal_uint32 Lap, kal_uint8 Uap, kal_uint16 Nap, kal_uint8 Result);
#else /* #if BT_SPP_AUTHORIZE_BY_MMI */
extern void SPPHandleAuthRsp(UART_PORT Port, kal_bool Result);
#endif /* #if BT_SPP_AUTHORIZE_BY_MMI */
extern void SPPHandleUartOwnerCnf(UART_PORT Port);
extern void SPPHandleSCOConnectReq(kal_uint32 Lap, kal_uint8 Uap, kal_uint16 Nap, kal_uint8 Server_chnl_num);
#if SPP_CLIENT == XA_ENABLED
//extern void SPPHandleConnectReq(kal_uint32 Lap, kal_uint8 Uap, kal_uint16 Nap, kal_uint8 Server_chnl_num, U16 remoteUUID);
//extern void SPPSCOHandleConnectReq(kal_uint32 Lap, kal_uint8 Uap, kal_uint16 Nap, kal_uint8 Server_chnl_num, U16 remoteUUID);
#endif /* #if SPP_CLIENT == XA_ENABLED */
extern void SPPHandleActivateReq(SppPortType portType, char *svcName, U16 svcUUID);
extern void SPPHandleSendDataReq(UART_PORT Port);
extern void SPPHandleGetDataReq(UART_PORT Port);
extern void SPPHandleDeactivateReq(UART_PORT port);
extern void SPPHandleDisconnectReq(UART_PORT Port);
extern void SPPHandleAudioConnectReq(UART_PORT Port);
extern void SPPHandleAudioDisconnectReq(UART_PORT Port);
/* APP -> DUN Interface */
#if DUN_CLIENT == XA_ENABLED
//extern void DUNHandleConnectReq(kal_uint32 Lap, kal_uint8 Uap, kal_uint16 Nap, kal_uint8 Server_chnl_num);
#endif /* #if DUN_CLIENT == XA_ENABLED */
#if DUN_SERVER == XA_ENABLED
extern void DUNHandleActivateReq(SppPortType portType);
extern void DUNHandleDeactivateReq(UART_PORT port);
#endif /* #if DUN_SERVER == XA_ENABLED */
#if DUN_SERVER == XA_ENABLED || DUN_CLIENT == XA_ENABLED
extern void DUNHandleDisconnectReq(UART_PORT Port);
#endif /* #if DUN_SERVER == XA_ENABLED || DUN_CLIENT == XA_ENABLED */
#if BT_SPP_AUTHORIZE_BY_MMI
//extern void DUNHandleConnectIndRes(UART_PORT Port, kal_uint32 Lap, kal_uint8 Uap, kal_uint16 Nap, kal_uint8 Result);
#endif /* #if BT_SPP_AUTHORIZE_BY_MMI */
/* APP -> FAX Interface */
#if FAX_SERVER == XA_ENABLED
extern void FAXHandleActivateReq(SppPortType portType);
extern void FAXHandleDeactivateReq(UART_PORT port);
extern void FAXHandleDisconnectReq(UART_PORT Port);
#endif /* #if FAX_SERVER == XA_ENABLED */
#if BT_SPP_AUTHORIZE_BY_MMI
//extern void FAXHandleConnectIndRes(UART_PORT Port, kal_uint32 Lap, kal_uint8 Uap, kal_uint16 Nap, kal_uint8 Result);
#endif /* #if BT_SPP_AUTHORIZE_BY_MMI */
/* APP -> SPP/DUN/FAX Interface */
extern void UartHandlePlugoutCnf(UART_PORT Port);

/* SPP Uart Interface */
extern kal_uint16 Sppa_GetBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, kal_uint8 *status, module_type ownerid);
extern kal_uint16 Sppa_PutBytes(UART_PORT port, kal_uint8 *Buffaddr, kal_uint16 Length, module_type ownerid);
/* #endif end of CUSTOM_SPP */


void spp_adp_init(void);
#ifdef __SPP_THROUGHPUT_TEST__
void spp_throughput_test_start(BOOL bServer);
void spp_throughput_test_send(BOOL bServer);
#endif
void btmtk_adp_spp_handle_message(ilm_struct *ilm_ptr);



#endif /* __BT_ADP_SPP_H__ */



