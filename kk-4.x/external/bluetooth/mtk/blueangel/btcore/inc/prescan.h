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

#ifndef __PRESCAN_H
#define __PRESCAN_H

/***************************************************************************
 *
 * File:
 *     $Workfile:prescan.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains code that provides a place to create Prescan
 *     functions.
 *
 * Created:
 *     September 27, 2000
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

/*---------------------------------------------------------------------------
 * Prescan Module layer
 *
 *     Bluetooth technology is still somewhat in its infancy, and the 
 *     specification is still in a small state of flux.  The iAnywhere 
 *     Blue SDK is kept up to date with the very latest spec, however, it
 *     is required to work with a large number of different radios.  Because
 *     things are changing, and because interpretations in the operation
 *     of the HCI exist, the Blue SDK provides a mechanism for providing
 *     compatibility with radios that vary from the latest specification.
 *
 *     This mechanism is provided through something called a Prescan Module.
 *     Prescan modules are capable of hooking some low level HCI routines
 *     and creating a behavior that is consistent to the expectations of the
 *     upper layers of the stack (ME in particular).    
 *
 *     Most radio modules follow the HCI specification pretty well, with
 *     only a small amount of variation from the spec.  These differences
 *     are easily handled in the ME.  When a radio has a behavior that the
 *     ME cannot handle, then a Prescan Module may be necessary.
 *
 *     This file contains functions exported by Prescan Modules provided
 *     by iAnywhere Solutions.  Each section contains an API and a description
 *     of it's use.
 */

/****************************************************************************
 *
 * Add module-specific entries here, including data structures and 
 * initialization prototypes.
 *
 ****************************************************************************/

#include "sys/hci.h"
#include "sys/evmxp.h"
#include "eventmgr.h"

#if HCI_ALLOW_PRESCAN == XA_ENABLED

/****************************************************************************
 *
 *  Constants
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Some versions of the Infineon Bluemoon Single/CR Devlepmnet Board 1.2 
 * report incorrect HCI and LMP versions and respond improperly to the
 * HCC_ACCEPT_SYNC_CON_REQ.  This prescan enforces a version number of 2 for 
 * both LMP and HCI, so that eSCO and other Bluetooth 1.2 (and 2.0) 
 * features will work properly.  The response to HCC_ACCEPT_SYNC_CON_REQ is
 * also fixed up.
 *
 * If PRESCAN_INFINEON_FIXES is set to XA_ENABLED, the Infineon prescan
 * will set the LMP and HCI versions to 2 and fix the response to
 * HCC_ACCEPT_SYNC_CON_REQ.
 *
 * PRESCAN_INFINEON_FIXES should be enabled for any build being used with
 * an Infineon module that exhibits these behaviors.
 */
#ifndef PRESCAN_INFINEON_FIXES
#define PRESCAN_INFINEON_FIXES XA_DISABLED
#endif 

/****************************************************************************
 *
 *  Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PrescanContext structure
 *
 *     This structure contains the global context for all prescan modules
 *     provided by iAnywhere Solutions.  The context for a particular module
 *     is conditionally included or excluded.
 */
typedef struct _PrescanContext
{
    ListEntry prescanList;  /* List of prescan modules */

    /* Module specific data */

#if PRESCAN_INFINEON_FIXES == XA_ENABLED
    /* Infineon */
    HciPrescanHandler infHandler;
    BOOL acceptSyncSent;
#endif /* PRESCAN_INFINEON_FIXES == XA_ENABLED */

} PrescanContext;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * INFINEON_Init()
 *
 *     Initializes special support for INFINEON radio modules.  This function
 *     installs a prescan module to improve the compatibility of the iAnywhere
 *     BlueSDK protocol stack and the INFINEON Bluemoon radio module.
 *
 * Parameters:
 *     void
 *
 * Returns:
 *     void
 */
BOOL INFINEON_Init(void);

#endif /* HCI_ALLOW_PRESCAN == XA_ENABLED */

#endif /* __PRESCAN_H */ 

