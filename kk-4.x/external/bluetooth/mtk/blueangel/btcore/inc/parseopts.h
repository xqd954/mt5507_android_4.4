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

/***************************************************************************
 *
 * File:
 *     $Workfile:parseopts.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file the header file for the code which parses and exports the 
 *     command line options for the stack.
 *
 * Created:
 *     October 12, 1999
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
#ifndef __PARSEOPTS_H_
#define __PARSEOPTS_H_

// #if BT_STACK == XA_ENABLED
#include "bttypes.h"

/* Table of known radio types */

#define RADIO_TYPE_DEFAULT          0
#define RADIO_TYPE_ERICSSON_UART  1
#define RADIO_TYPE_SIW_UART       2
#define RADIO_TYPE_SIW_USB        3
#define RADIO_TYPE_TI_UART        4
#define RADIO_TYPE_INFINEON_UART  5
#define RADIO_TYPE_CSR_UART       6
#define RADIO_TYPE_CSR_USB        7
#define RADIO_TYPE_CSR_BCSP       8
#define RADIO_TYPE_MOTOROLA_UART  9
#define RADIO_TYPE_ESI_INET       10
#define RADIO_TYPE_IA_INET        10
#define RADIO_TYPE_BROADCOM_UART  11
#define RADIO_TYPE_GENERIC_UART   12
#define RADIO_TYPE_ST_MICRO_UART  13
#define RADIO_TYPE_MTKBT_UART     14
// #endif /* BT_STACK == XA_ENABLED */

// #if IRDA_STACK == XA_ENABLED
/* Table of known IrDA adapter types */
#define ESI_9680_7401_ADAPTER   1       /* Default */
#define NULL_ADAPTER            2

/* Frame types */
#define FRM_TYPE_COM            1       /* Default */
#define FRM_TYPE_INET           2

// #endif /* IRDA_STACK == XA_ENABLED */

/*---------------------------------------------------------------------------
 * Local types 
 */
// #if BT_STACK == XA_ENABLED
typedef struct _UserOpts
{
    U16 portNum;        /* User-selected port number */
    U16 speed;          /* UART Baud rate (115 or 57) */
    U16 radioType;      /* Brand of radio */
    U16 tranType;       /* Transport type */
    BOOL startSniffer;  /* Start sniffer during initialization */

} UserOpts;

// #endif /* BT_STACK == XA_ENABLED */

// #if IRDA_STACK == XA_ENABLED
typedef struct _IrUserOpts
{
    U16 portNum;        /* User-selected port number */
    U16 adapterType;    /* Brand of radio */
    U16 framerType;     /* Framer type to use */
} IrUserOpts;

// #endif /* IRDA_STACK == XA_ENABLED */

/*---------------------------------------------------------------------------
 * ParseStandardOptions()
 *
 *     Parses option pointed to by 'opt' parameter. If the parameter is
 * recognized, the 'opt' pointer is advanced past the option.
 */
BOOL ParseStandardOptions(char **opt);

// #if BT_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * getPortNumOption()
 *     Returns the user-selected port number.
 */
U16 getPortNumOption(void);

/*---------------------------------------------------------------------------
 * getSpeedOption()
 *     Returns the user-selected bps rate.
 */
U16 getSpeedOption(void);

/*---------------------------------------------------------------------------
 * getRadioType()
 *     Returns the user-selected radio type.
 */
U16 getRadioType(void);

/*---------------------------------------------------------------------------
 * getSnifferOption()
 *     Returns the user-selected open sniffer at startup setting.
 */
BOOL getSnifferOption(void);

// #endif /* BT_STACK == XA_ENABLED */

#if IRDA_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * getIrPortNumOption()
 *     Returns the user-selected port number.
 */
U16 getIrPortNumOption(void);

/*---------------------------------------------------------------------------
 * getRadioType()
 *     Returns the IrDA adapter type
 */
U16 getIrAdapterType(void);

/*---------------------------------------------------------------------------
 * getFramerType()
 *     Returns the framer type selected (FRM_TYPE_*)
 */
U16 getFramerType();

#endif /* IRDA_STACK == XA_ENABLED */

/*---------------------------------------------------------------------------
 * getErrorMesage()
 *     Returns the user-selected bps rate.
 */
const char *getErrorMessage(void);

/*---------------------------------------------------------------------------
 * getStandardOptions()
 *     Returns a string which describes the standard options
 */
const char *getStandardOptions(void);

#endif /* __PARSEOPTS_H_ */ 

