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

#ifndef __AT_HS_H
#define __AT_HS_H
/****************************************************************************
 *
 * File:
 *     $Workfile:at_hs.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains an AT Headset Command Processor.
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
#if AT_HEADSET == XA_ENABLED

/*---------------------------------------------------------------------------
 * AT Headset Command Processor API layer
 *
 * The AT Command Processor API provides functions for both parsing and 
 * encoding AT commands and responses.  It is modularized into components
 * that support Headset, Hands-free, and DUN profiles.  It also provides
 * the ability to parse and encode Phonebook commands and responses, based
 * on the GSM specifications.  Support for SMS messaging commands will
 * be added in the future.
 *
 * The Headset Command Processor module provides the commands necessary
 * for the Headset Profile.  It is necessary to include this module
 * when using the Hands-free Profile, because they share some AT commands.
 */

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtCommand type
 *
 *     Headset Command and Result types.
 */
#ifndef __LINUX_SUPPRESS_ERROR__
typedef U16 AtCommand;
#endif
/**  */
#define AT_MICROPHONE_GAIN      0x0100

/**  */
#define AT_SPEAKER_GAIN         0x0101

/**  */
#define AT_KEYPAD_CONTROL       0x0102

/* End of AtCommand */


/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtHeadsetCmd
 * 
 *     This structure is used to specify the parameters associated with
 *     Headset commands. Headset commands are sent from the Headset device
 *     to the Audio Gateway. As such, this structure is used by the Headset
 *     device encoder, and the Audio Gateway decoder functions.
 */
typedef union _AtHeadsetCmd {

    /* AT_MICROPHONE_GAIN */
    struct {
        U8          gain;
    } mic;

    /* AT_SPEAKER_GAIN */
    struct {
        U8          gain;
    } speaker;

    /* AT_KEYPAD_CONTROL */
    struct {
        U8          button;
    } keypad;

} AtHeadsetCmd;
    

/*---------------------------------------------------------------------------
 * AtHeadsetResult
 * 
 *     This structure is used to specify the parameters associated with
 *     Headset results. Headset results are sent from the Audio Gateway
 *     to the Headset device. As such, this structure is used by the Audio
 *     Gateway encoder, and the Headset device decoder functions.
 */
typedef union _AtHeadsetResult {

    /* AT_MICROPHONE_GAIN */
    struct {
        U8          gain;
    } mic;  

    /* AT_SPEAKER_GAIN */
    struct {
        U8          gain;
    } speaker;  

} AtHeadsetResult;

#else /* AT_HEADSET == XA_ENABLED */

/* Stub structures to keep #defines out of AtResults/AtCommands structures. */
typedef U8 AtHeadsetCmd;
typedef U8 AtHeadsetResult;

#endif /* AT_HEADSET == XA_ENABLED */

#endif /* __AT_HS_H */
