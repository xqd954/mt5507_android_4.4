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
 *     $Workfile:at_dun.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains an Dial Up Networking AT Command Processor.
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
#include "atp.h"
#include "sys/atpi.h"
#include "osapi.h"

#if AT_DUN == XA_ENABLED

const char *AT_DunCmds[] = {
    "&C",    // Circuit 109 (Received line signal detector) (DCD)
    "&D",    // Circuit 108 (Data terminal ready) (DTR)
    "&F",    // Set to Factory-defined Configuration
    "+GCAP", // Request Complete Capabilities List
    "+GMI",  // Request Manufacturer Identification
    "+GMM",  // Request Model Identification
    "+GMR",  // Request Revision Identification
    "A",     // Answer
    "D",     // Dial
    "E",     // Command Echo
    "H",     // Hook Control
    "L",     // Monitor Speaker Loudness
    "M",     // Monitor Speaker Mode
    "O",     // Return to Online Data
    "P",     // Select Pulse Dialling
    "Q",     // Result Code Suppression
    "S0",    // Automatic Answer
    "S10",   // Automatic Disconnect Delay
    "S3",    // Command Line Termination Character
    "S4",    // Response Formatting Character
    "S5",    // Command Line Editing Character
    "S6",    // Pause Before Blind Dialling
    "S7",    // Connection Completion Timeout (may be ignored)
    "S8",    // Comma Dial Modifier Time
    "T",     // Select Tone Dialling
    "V",     // DCE Response Format
    "X",     // Result Code Selection and Call Progress Monitoring Control
    "Z",     // Reset To Default Configuration
    0
};

#if AT_ROLE_TERMINAL == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * AT_Encode_Dun
 * 
 *     Encodes TE originated AT commands. 
 */ 
AtStatus TE_Encode_Dun(AtContext *Atc, U16 Type, const AtDunCmd *Command, XaBufferDesc *Output)
{
    U16     avail = (Output->buffSize - Output->writeOffset) - 1;

    switch (Type) {
    default:
        return AT_STATUS_NOT_SUPPORTED;
    }
    return AT_STATUS_OK;
}

/*--------------------------------------------------------------------------- 
 * AT_Decode_Dun
 * 
 *     Decodes AT results received by the Common Unit (TE).
 */
AtStatus TE_Decode_Dun(AtContext *Atc, U16 *Type, AtDunResult *Result, XaBufferDesc *Input)
{
    U8             *ptr = Input->buff + Input->readOffset;
    AtParameter     parms[AT_MAX_PARMS];

    AtParseParameters(ptr, (U16)(Input->writeOffset - Input->readOffset), parms);

    switch (*Type) {
    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
#endif /* AT_ROLE_TERMINAL == XA_ENABLED */


#if AT_ROLE_MOBILE == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * AT_Encode_Dun
 * 
 *     Encodes Audio Gateway (ME) originated AT results.
 */ 
AtStatus ME_Encode_Dun(AtContext *Atc, U16 Type, const AtDunResult *Result, XaBufferDesc *Output)
{
    U16     avail = (Output->buffSize - Output->writeOffset) - 2;

    switch (Type) {
    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
        
/*--------------------------------------------------------------------------- 
 * AT_Decode_Dun
 * 
 *     Decodes AT commands received by the Gateway (ME).
 */
AtStatus ME_Decode_Dun(AtContext *Atc, U16 *Type, AtDunCmd *Command, XaBufferDesc *Input)
{
    U8             *ptr = Input->buff + Input->readOffset;
    AtParameter     parms[AT_MAX_PARMS];

    AtParseParameters(ptr, (U16)(Input->writeOffset - Input->readOffset), parms);

    switch (*Type) {
    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
#endif /* AT_ROLE_MOBILE == XA_ENABLED */

#endif /* AT_DUN == XA_ENABLED */
