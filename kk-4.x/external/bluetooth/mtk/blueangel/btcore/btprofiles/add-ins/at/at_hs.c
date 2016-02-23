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
 *     $Workfile:at_hs.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains an AT Command Processor.
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

#if AT_HEADSET == XA_ENABLED

/*
 * Headset Commands and Responses
 */ 
const char *AT_HeadsetCmds[] = {
    /* AT capabilities from GSM 07.07 and V.25ter */
#if defined(USE_EXT_AT_PARSER)
    "\0",    // "+VGM"
    "\0",    // "+VGS"
    "\0",    // "+CKPD"
#else
    "+VGM",  /* 0x0100 Microphone gain */
    "+VGS",  /* 0x0101 Speaker gain    */
    "+CKPD", /* 0x0102 Keypad control (AT+CKPD=200 used for Headset button press) */
#endif
    0
};

#if AT_ROLE_TERMINAL == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * AT_Encode_Headset
 * 
 *     Encodes Headset Device (TE) originated AT commands.
 */ 
AtStatus TE_Encode_Headset(AtContext *Atc, U16 Type, const AtHeadsetCmd *Command, XaBufferDesc *Output)
{
    U16     avail = (Output->buffSize - Output->writeOffset) - 1;

    switch (Type) {
    case AT_MICROPHONE_GAIN:
        /* Syntax: AT+VGM=<gain> */
        if (avail < 3) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Command->mic.gain);
        break;
        
    case AT_SPEAKER_GAIN:
        /* Syntax: AT+VGS=<gain> */
        if (avail < 3) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Command->speaker.gain);
        break;

    case AT_KEYPAD_CONTROL:
        /* Syntax: +CKPD=<keys>[,<time>[,<pause>]] */
        if (avail < 3) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Command->keypad.button);
        break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}

/*--------------------------------------------------------------------------- 
 * AT_Decode_Headset
 * 
 *     Decodes AT results received by the Headset Unit (TE).
 */ 
AtStatus TE_Decode_Headset(AtContext *Atc, U16 *Type, AtHeadsetResult *Result, XaBufferDesc *Input)
{
    U8             *ptr;
    AtParameter     parms[AT_MAX_PARMS];

    ptr = Input->buff + Input->readOffset;

    AtParseParameters(ptr, (U16)(Input->writeOffset - Input->readOffset), parms);

    switch (*Type) {
    case AT_MICROPHONE_GAIN:
        /* Syntax: AT+VGM=<gain> */
        if (parms[0].len > 0)
            Result->mic.gain = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
        break;
        
    case AT_SPEAKER_GAIN:
        /* Syntax: AT+VGS=<gain> */
        if (parms[0].len > 0)
            Result->speaker.gain = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
        break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
#endif /* AT_ROLE_TERMINAL == XA_ENABLED */


#if AT_ROLE_MOBILE == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * AT_Encode_Headset
 * 
 *     Encodes Audio Gateway (ME) originated AT results.
 */ 
AtStatus ME_Encode_Headset(AtContext *Atc, U16 Type, const AtHeadsetResult *Result, XaBufferDesc *Output)
{
    U16     avail = (Output->buffSize - Output->writeOffset) - 2;

    switch (Type) {
    case AT_MICROPHONE_GAIN:
        /* Syntax: +VGM=<gain> */
        if (avail < 3) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->mic.gain);
        break;
        
    case AT_SPEAKER_GAIN:
        /* Syntax: +VGS=<gain> */
        if (avail < 3) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->speaker.gain);
        break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
        

/*--------------------------------------------------------------------------- 
 * AT_Decode_Headset
 * 
 *     Decodes AT commands received by the Audio Gateway (ME).
 */ 
AtStatus ME_Decode_Headset(AtContext *Atc, U16 *Type, AtHeadsetCmd *Command, XaBufferDesc *Input)
{
    U8             *ptr;
    AtParameter     parms[AT_MAX_PARMS];

    ptr = Input->buff + Input->readOffset;

    AtParseParameters(ptr, (U16)(Input->writeOffset - Input->readOffset), parms);

    switch (*Type) {
    case AT_MICROPHONE_GAIN:
        /* Syntax: +VGM=<gain> */
        if (parms[0].len > 0)
            Command->mic.gain = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
        break;
        
    case AT_SPEAKER_GAIN:
        /* Syntax: +VGS=<gain> */
        if (parms[0].len > 0)
            Command->speaker.gain = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
        break;

    case AT_KEYPAD_CONTROL:
        /* Syntax: +CKPD=<keys>[,<time>[,<pause>]] */
        if (parms[0].len > 0)
            Command->keypad.button = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
        break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}

#endif /* AT_ROLE_MOBILE == XA_ENABLED */

#endif /* AT_HEADSET == XA_ENABLED */
