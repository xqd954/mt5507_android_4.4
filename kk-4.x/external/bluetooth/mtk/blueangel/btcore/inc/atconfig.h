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

#ifndef __ATCONFIG_H
#define __ATCONFIG_H
/****************************************************************************
 *
 * File:
 *     $Workfile:atconfig.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains an AT Command Processor configuration 
 *     constants.
 *             
 * Copyright 2002-2005 Extended Systems, Inc.

 * Portions copyright 2005 iAnywhere Solutions, Inc.

 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,  
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "config.h"

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_HEADSET  
#define AT_HEADSET          XA_ENABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_HANDSFREE
#define AT_HANDSFREE        XA_ENABLED
#endif

#if AT_HANDSFREE == XA_ENABLED
/* Headset commands must be included when using handsfree. */
#undef AT_HEADSET
#define AT_HEADSET          XA_ENABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_PHONEBOOK
//#define AT_PHONEBOOK        XA_DISABLED
#define AT_PHONEBOOK        XA_ENABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_SMS       
//#define AT_SMS              XA_DISABLED
#define AT_SMS              XA_ENABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_DUN      
#define AT_DUN              XA_DISABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_ROLE_TERMINAL
//#define AT_ROLE_TERMINAL        XA_DISABLED
#define AT_ROLE_TERMINAL        XA_ENABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_ROLE_MOBILE
#define AT_ROLE_MOBILE          XA_ENABLED
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_MAX_PARMS
#define AT_MAX_PARMS        45
#endif

#if (AT_MAX_PARMS < 16) || (AT_MAX_PARMS > 255)
#error "AT_MAX_PARMS must be between 16 and 255."
#endif

/*----------------------------------------------------------------------------
 *
 */
#ifndef AT_MAX_INDICATORS
#define AT_MAX_INDICATORS   9
#endif

#if (AT_MAX_INDICATORS < 1) || (AT_MAX_INDICATORS > 255)
#error "AT_MAX_INDICATORS must be between 8 and 255."
#endif

#ifndef AT_MAX_CODECS
#define AT_MAX_CODECS   2
#endif

#if (AT_MAX_CODECS < 1) || (AT_MAX_CODECS > 11)
#error "AT_MAX_CODECS must be between 1 and 10."
#endif

#endif /* __ATCONFIG_H */
