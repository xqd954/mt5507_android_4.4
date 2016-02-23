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
*  permission of MediaTek Inc. (C) 2006
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
 *
 * Filename:
 * ---------
 * BtaccCtrl.h
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains sdap functions prototype and variable definiation of SDAP services
 *
 * Author:
 * -------
 * Dlight Ting(mtk01239)
 *      Create 2006/2/9
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * Mar 12 2009 mtk01411
 * [MAUI_01645202] [Bluetooth] Btadp folder restruct
 * 
 *
 * Jul 2 2008 MTK02126
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Mar 26 2007 mtk01239
 * [MAUI_00231225] bt_hid disconnected indication pop up, but it return to disconnecting screen
 * 
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Dec 31 2006 mtk01239
 * [MAUI_00355405] [Bluetooth] turn off the panic instead of assert
 * 
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/

#ifndef _BTACCCTRL_H
#define _BTACCCTRL_H
#include "me_adp.h"

/***************
Type Definitions 
****************/

#define  BT_PSM_POLICY_NONE     (0x00000000)
#define BT_PSM_POLICY_RFCOMM     (0x00000001)
#define BT_PSM_POLICY_SDP            (0x00000002)
#define BT_PSM_POLICY_AVDTP        (0x00000004)
#define BT_PSM_POLICY_AVCTP        (0x00000008)
#define BT_PSM_POLICY_HID            (0x00000010)
#define BT_PSM_POLICY_BNEP          (0x00000020)
#define BT_PSM_POLICY_TCS            (0x00000040)
#define BT_PSM_POLICY_ALL           (0xFFFFFFFF)

#define BT_SERVICE_POLICY_NONE     (0x00000000)
#define BT_SERVICE_POLICY_SPP     (0x00000001)
#define BT_SERVICE_POLICY_OPP     (0x00000002)
#define BT_SERVICE_POLICY_FTP     (0x00000004)
#define BT_SERVICE_POLICY_HSP     (0x00000008)
#define BT_SERVICE_POLICY_HFP     (0x00000010)
#define BT_SERVICE_POLICY_A2DP     (0x00000020)
#define BT_SERVICE_POLICY_AVRCP     (0x00000040)
#define BT_SERVICE_POLICY_BIP     (0x00000080)
#define BT_SERVICE_POLICY_BPP     (0x00000100)
#define BT_SERVICE_POLICY_SAP     (0x00000200)
#define BT_SERVICE_POLICY_FAX     (0x00000400)
#define BT_SERVICE_POLICY_TEST      (0x00008000)
#define BT_SERVICE_POLICY_ALL           (0xFFFFFFFF)

typedef struct _BtAccCtrlContent_T
{
    BOOL accCtrlSet;
    U32 allowedPsm;
    U32 allowedService;
    BtRemoteDevice *accLink;

} BTACCCTRLCONTENT_T;

BOOL BTAccCtrlInit(void);
BOOL BTAccCtrlSet(U32 allowed_PSM, U32 allowedService);
BOOL BTAccCtrlUnset(void);
BOOL BTAccCtrlAllowDeviceWithService(BtRemoteDevice *link, U32 allowed_PSM, U32 allowedService);

BOOL BTAccCtrlResetServiceTable(U32 allowed_PSM, U32 allowedService);

BOOL BTAccCtrlCheckConnectPolicy(BD_ADDR *bdAddr);

BOOL BTAccCtrlCheckPSMPolicy(U16 PSM_info);

BOOL BTAccCtrlCheckServicePolicy(U8 serv_chnl);

#endif /* _BTACCCTRL_H */ 

