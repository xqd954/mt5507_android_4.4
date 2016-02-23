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
 * File:
 *     $Workfile:transprt.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     Multiple radio support transport driver.
 * 
 * Copyright 2005 Extended Systems, Inc.
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

#include "parseopts.h"
#include "bt_common.h"
#include "sys/usbtrans.h"
#include "sys/uarttran.h"


/* External transport initialization functions */
BtStatus INETTRAN_Init(TranCallback tranCallback);
BtStatus INETTRAN_Shutdown(void);

/*---------------------------------------------------------------------------
 *            TRAN_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Called by the HCI driver to initialize the transport.  This 
 *            initialization can happen synchronously or asynchronously.
 *
 * Returns:   <See hcitrans.h>
 */
BtStatus TRAN_Init(TranCallback tranCallback)
{
    BtStatus status = BT_STATUS_SUCCESS;

    bt_trace(TRACE_GROUP_1,TRAN_INIT_STARTED);

    switch(getRadioType()) {
    case RADIO_TYPE_SIW_USB:
    case RADIO_TYPE_CSR_USB:
	#if !defined(__MTK_TARGET__)&& defined(__BT_USB_DRV__)
        status = USBTRAN_Init(tranCallback);
	#endif
        break;
    case RADIO_TYPE_MTKBT_UART: //add by brad
    default:
        status = UARTTRAN_Init(tranCallback);
        break;
    }

    return status;
}

/*---------------------------------------------------------------------------
 *            TRAN_Shutdown()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Called by the HCI driver to shutdown the transport.  This 
 *            shutdown can happen synchronously or asynchronously.
 *
 * Returns:   <See hcitrans.h>
 */
BtStatus TRAN_Shutdown(void)
{
    BtStatus status = BT_STATUS_SUCCESS;

    switch(getRadioType()) {
    case RADIO_TYPE_SIW_USB:
    case RADIO_TYPE_CSR_USB:
	#if !defined(__MTK_TARGET__)&& defined(__BT_USB_DRV__)
        status = USBTRAN_Shutdown();
	#endif
        break;
    case RADIO_TYPE_MTKBT_UART:
    default:
        status = UARTTRAN_Shutdown();
        break;
    }

    return status;
}

