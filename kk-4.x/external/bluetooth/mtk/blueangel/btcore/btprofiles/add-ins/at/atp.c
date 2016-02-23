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
 *     $Workfile:atp.c$ for iAnywhere Blue SDK, Version 2.1.1
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
#include "bt_common.h"
#include "atp.h"
#include "sys/atpi.h"
#include "osapi.h"

const char *AT_CommonCmds[] = {
    /* AT capabilities from GSM 07.07 and V.25ter */
#if defined(USE_EXT_AT_PARSER)
    "\0",                 // "OK"
    "\0",                 // "ERROR"
    "\0",                 // "RING"
    "\0",                 // "+CME ERROR"
    "+CMEE",      // "+CMEE"
    "\0",                 // "+CSCS"
    "\0",                 // "NO CARRIER"
    "\0",                 // "BUSY"
    "\0",                 // "NO ANSWER"
    "\0",                 // "DELAYED"
    "\0",                 // "BLACKLISTED"
    "\0",                 // "+CGMI"
    "\0",                 // "+CGMM"
    "RAW",                 // "RAW"
#else
    "OK",          /* Acknowledgement to the execution of a command. */
    "ERROR",       /* Error indication code. */
    "RING",        /* Incoming call indication. */
    "+CME ERROR",  /* Error acknowledgement with error code */
    "+CMEE",       /* Enable/Disable extended error codes. */
    "+CSCS",       /* Select character set. */
    "NO CARRIER",  /* Call could not be established or connection terminated. */
    "BUSY",        /* Called party is busy. */
    "NO ANSWER",   /* Connection completion on timeout. */
    "DELAYED",     /* Calls to the number are temporarilly forbidden. */
    "BLACKLISTED", /* Calls to the number are forbidden until a manual reset. */
    "+CGMI",		/* Request manufacturer identification. reference chapter 5.1 */
    "+CGMM",		/* Request model identification. reference chapter 5.2 */
    "RAW",         /* Raw AT data */
#endif
    0
};

/* The order of the command tables must match the AT_GROUP defines in atpi.h */
#if defined(USE_EXT_AT_PARSER)
static const char **AT_CommandTable[AT_NUM_GROUPS+1] = { 
                        AT_CommonCmds, AT_HeadsetCmds, AT_HandsfreeCmds, 0 };
#else
static const char **AT_CommandTable[AT_NUM_GROUPS+1] = { 
                        AT_CommonCmds, AT_HeadsetCmds, AT_HandsfreeCmds, 
                        AT_PhonebookCmds, AT_SmsCmds, AT_DunCmds, 0 };
#endif
/*--------------------------------------------------------------------------- 
 * AT_Init()
 *
 *     Initialize the AT encoder/decoder context.
 */
AtStatus AT_Init(AtContext *Atc)
{
    I8  g, i;

    OS_MemSet((U8 *)Atc, 0, sizeof(Atc));

    /* Calculate each groups last AtCommand value. */
    for (g = 0; g < AT_NUM_GROUPS; g++) {
        if (AT_CommandTable[g] == 0)
            continue;

        for (i = 0; AT_CommandTable[g][i]; i++) 
            ; /* Searching for last entry in string table. */

        Atc->lastCmd[g] = (i - 1);
    }

    return AT_STATUS_OK;
}

#if AT_ROLE_TERMINAL == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * TE_Encode
 * 
 *     This function encodes an AT command for transmission to the gateway (ME).
 */ 
AtStatus TE_Encode(AtContext *Atc, const AtCommands *Command, XaBufferDesc *Out)
{
    AtStatus    status;
    const char *cmdStr = 0;
    U8          idx, grp;
    U16         len;

    CheckUnlockedParm(AT_STATUS_INVALID_PARM, Atc && Command && Out);
    
    idx = (Command->type & 0x00ff);
    grp = (Command->type & 0x0f00) >> 8;

    if (idx > Atc->lastCmd[grp]) {
        return AT_STATUS_NOT_FOUND;
    }

    cmdStr = AT_CommandTable[grp][(idx)];
    len = OS_StrLen(cmdStr);

    /* Check space for "AT"<base command>"="<cr> */
    CheckUnlockedParm(AT_STATUS_NO_RESOURCES, (Out->buffSize - Out->writeOffset) > (len + 4));

    Out->buff[Out->writeOffset++] = 'A';
    Out->buff[Out->writeOffset++] = 'T';
    OS_MemCopy(Out->buff + Out->writeOffset, (const U8*)cmdStr, len);
    Out->writeOffset += len;
    Out->buff[Out->writeOffset++] = '=';

    switch (Command->type & 0xff00) {
    case AT_GROUP_COMMON:
        status = TE_Encode_Common(Atc, Command->type, Command, Out);
        break;

#if AT_HEADSET == XA_ENABLED
    case AT_GROUP_HEADSET:
        status = TE_Encode_Headset(Atc, Command->type, &Command->p.hs, Out);
        break;
#endif /* AT_HEADSET == XA_ENABLED */

#if AT_HANDSFREE == XA_ENABLED
    case AT_GROUP_HANDSFREE:
        status = TE_Encode_Handsfree(Atc, Command->type, &Command->p.hf, Out);
        break;
#endif /* AT_HANDSFREE == XA_ENABLED */

#if AT_PHONEBOOK == XA_ENABLED
    case AT_GROUP_PHONEBOOK:
        status = TE_Encode_Phonebook(Atc, Command->type, &Command->p.pb, Out);
        break;
#endif /* AT_HANDSFREE == XA_ENABLED */

    default:
        if (Command->type & AT_TEST) {
            if ((Out->buffSize - Out->writeOffset) < 1) {
                return AT_STATUS_NO_RESOURCES;
            }
        }
        else if (Command->type & AT_READ) {
            Out->writeOffset--;      /* remove the '=' */
        } 
        else {
            Assert(0);
            return AT_STATUS_NOT_SUPPORTED;
        }
        
        Out->buff[Out->writeOffset++] = '?';
        status = AT_STATUS_OK;
        break;
    }
    
    if (status == AT_STATUS_OK)
        Out->buff[Out->writeOffset++] = '\r';

    return status;
}


/*--------------------------------------------------------------------------- 
 * TE_Decode
 * 
 *     This function decodes an AT result received by the device (TE).
 */ 
AtStatus TE_Decode(AtContext *Atc, AtResults *Result, XaBufferDesc *In)
{
    I8           i, t, skip;
    U16          len, offset;
    BOOL         match = FALSE;
    const char **table;
    char         resultStr[RESULT_STAGE_LEN];
    AtStatus     status = AT_STATUS_INVALID_PARM;

    CheckUnlockedParm(AT_STATUS_INVALID_PARM, (Atc && Result && In));

    OS_MemSet((U8 *)Result, 0, sizeof(AtResults));

    AtRemoveWhiteSpace(In);

    AtDebugOut(In);

    len = In->writeOffset - In->readOffset;

    /* For efficiency, convert the first 15 characters to uppercase
     * for case-insensitive comparison with command string table.
     */
    i = min (RESULT_STAGE_LEN, len);

    OS_MemSet((U8*)resultStr, 0, RESULT_STAGE_LEN);

    while (i-- > 0)
        resultStr[i] = ToUpper(In->buff[In->readOffset + i]);

    for (t = 0; (table = AT_CommandTable[t]) != 0; t++) {

        for (skip = 0, i = 0; table[i]; i++) {

            for (offset = 0; (offset < len) || table[i][offset + skip] == 0; offset++) {
                if (table[i][offset + skip] == 0) {
                    match = TRUE;
                    break;
                }

                if (resultStr[offset] != table[i][offset + skip]) {
                    /* We've hit a mismatch. If the StrTable entry has a space,
                     * then it's been compressed out of the input buffer so skip it.
                     */
                    if ((table[i][offset] != ' ') ||
                        (resultStr[offset] != table[i][offset + skip + 1])) {
                        break;
                    }
                    skip += 1;
                }
            }
            Assert (offset < RESULT_STAGE_LEN);

            if (match) break;
        }
        if (match) break;
    }

    if (!match) {
        return AT_STATUS_NOT_FOUND;
    }

    In->readOffset += offset;   /* Skip over <command type> */

    Result->type = (AtCommand)(i | (0x0100 * t));

    /* Strip the ':' between the command and the parameters. */
    if ((In->writeOffset > In->readOffset) && (In->buff[In->readOffset] == ':'))
        In->readOffset++;

    Assert(In->readOffset <= In->writeOffset);

    switch (t) {
    case 0:
        status = TE_Decode_Common(Atc, &Result->type, Result, In);
        break;

#if AT_HEADSET == XA_ENABLED
    case 1:
        status = TE_Decode_Headset(Atc, &Result->type, &Result->p.hs, In);
        break;
#endif /* AT_HEADSET == XA_ENABLED */

#if AT_HANDSFREE == XA_ENABLED
    case 2:
        status = TE_Decode_Handsfree(Atc, &Result->type, &Result->p.hf, In);
        break;
#endif /* AT_HANDSFREE == XA_ENABLED */

#if AT_PHONEBOOK == XA_ENABLED
    case 3:
        status = TE_Decode_Phonebook(Atc, &Result->type, &Result->p.pb, In);
        break;
#endif /* AT_PHONEBOOK == XA_ENABLED */

    default:
        Assert(0);
        break;
    }

    In->readOffset = In->writeOffset = 0;
    return status;
}
#endif /* AT_ROLE_TERMINAL == XA_ENABLED */


#if AT_ROLE_MOBILE == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * ME_Encode
 * 
 *     This function encodes an AT result for transmission to the device (TE).
 */ 
AtStatus ME_Encode(AtContext *Atc, const AtResults *Result, XaBufferDesc *Out)
{
    const char *cmdStr = 0;
    AtCommand   idx, grp;
    U16         len;
    AtStatus    status = AT_STATUS_INVALID_PARM;

    CheckUnlockedParm(AT_STATUS_INVALID_PARM, (Atc && Result && Out));

    idx = (Result->type & 0x00ff);
    grp = (Result->type & 0x0f00) >> 8;

    if (idx > Atc->lastCmd[grp] || grp >= AT_NUM_GROUPS) {
        return AT_STATUS_NOT_FOUND;
    }

    cmdStr = AT_CommandTable[grp][(idx)];
    len = OS_StrLen(cmdStr);

    /* Check space for "AT"<base command>"="<cr> */
    /* Check space for <cr><lf><base command>":"<cr><lf> */
    CheckUnlockedParm(AT_STATUS_NO_RESOURCES, (Out->buffSize - Out->writeOffset) > (len + 6));

    Out->buff[Out->writeOffset++] = '\r';
    Out->buff[Out->writeOffset++] = '\n';
    OS_MemCopy(Out->buff + Out->writeOffset, (const U8 *)cmdStr, len);
    Out->writeOffset += len;
    Out->buff[Out->writeOffset++] = ':';
    Out->buff[Out->writeOffset++] = ' ';  /* Required by some Ericsson handsets */

    switch (Result->type & 0x0f00) {
    case AT_GROUP_COMMON:
        status = ME_Encode_Common(Atc, Result->type, Result, Out);
        break;

#if AT_HEADSET == XA_ENABLED
    case AT_GROUP_HEADSET:
        status = ME_Encode_Headset(Atc, Result->type, &Result->p.hs, Out);
        break;
#endif /* AT_HEADSET == XA_ENABLED */

#if AT_HANDSFREE == XA_ENABLED
    case AT_GROUP_HANDSFREE:
        status = ME_Encode_Handsfree(Atc, Result->type, &Result->p.hf, Out);
        break;
#endif /* AT_HANDSFREE == XA_ENABLED */

#if AT_PHONEBOOK == XA_ENABLED
    case AT_GROUP_PHONEBOOK:
        status = ME_Encode_Phonebook(Atc, Result->type, &Result->p.pb, Out);
        break;
#endif /* AT_PHONEBOOK == XA_ENABLED */

#if AT_SMS == XA_ENABLED
	case AT_GROUP_SMS:
		status = ME_Encode_Sms(Atc, Result->type, &Result->p.sms, Out);
		break;
#endif

    default:
        Assert(0);
        break;
    }

    if ((status == AT_STATUS_OK) && (Result->type != AT_RAW)) {
        Out->buff[Out->writeOffset++] = '\r';
        Out->buff[Out->writeOffset++] = '\n';
    }

    return status;
}

    
/*--------------------------------------------------------------------------- 
 * ME_Decode
 * 
 *     This function decodes AT commands received by a gateway (ME).
 */ 
AtStatus ME_Decode(AtContext *Atc, AtCommands *Command, XaBufferDesc *In)
{
    I8           i, t;
    U16          len, offset=0;
    BOOL         match = FALSE;
    const char **table;
    char         cmdStr[COMMAND_STAGE_LEN];
    AtStatus     status = AT_STATUS_INVALID_PARM;

    CheckUnlockedParm(AT_STATUS_INVALID_PARM, (Atc && Command && In));

	// Handle message data of SMS for send and write
	if(Command->bContinue)
	{
		if(Command->bContinue)
		{
			switch(Command->type)
			{
			#if AT_SMS == XA_ENABLED
			case AT_SEND_MESSAGE:
				Command->p.sms.sendMsg.msg = AtMakeString(&In->buff[In->readOffset], (U16)(In->writeOffset - In->readOffset));
				break;
			case AT_STORE_MESSAGE:
				Command->p.sms.writeMsg.msg = AtMakeString(&In->buff[In->readOffset], (U16)(In->writeOffset - In->readOffset));
				break;
                                          #endif
			default:
				break;
			}
			Command->bContinue = FALSE;
		}
		return AT_STATUS_OK;
	}

    OS_MemSet((U8 *)Command, 0, sizeof(AtCommands));

    AtRemoveWhiteSpace(In);

    AtDebugOut(In);
    
    /* Commands we process must start with "AT" and end with <cr> */
    if (((In->buff[In->readOffset] != 'A') && (In->buff[In->readOffset] != 'a')) || 
        ((In->buff[In->readOffset + 1] != 'T') && (In->buff[In->readOffset + 1] != 't')))  {
        return AT_STATUS_NOT_FOUND;
    }

    len = (In->writeOffset - In->readOffset) - 1;

	if(len <= 1)
	{
		/* It is check alive CMD */
		Command->type = AT_CHECK_ALIVE;
		status = AT_STATUS_OK;
	       goto exit;
	}


    /* For efficiency, convert the first 6 characters to uppercase
     * for case-insensitive comparison with command string table.
     */
    i = min (COMMAND_STAGE_LEN, len);

    OS_MemSet((U8*)cmdStr, 0, COMMAND_STAGE_LEN);

    while (i-- > 0)
        cmdStr[i] = ToUpper(In->buff[In->readOffset + 2 + i]);

    for (t = 0; (table = AT_CommandTable[t]) != 0; t++) 
    {
        for (i = 0; table[i]; i++) 
        {
            if(table[i][0] == 0) continue;
            for (offset = 0; offset < len; offset++) 
            {
                if (table[i][offset] == 0) 
                {
                    match = TRUE;
                    break;
                }
                if (cmdStr[offset] != table[i][offset]) 
                {
                    break;
                }
            }
            Assert (offset < COMMAND_STAGE_LEN);

            if (match) 
            {
                break;
            }
        }
        if (match) 
        {
            break;
        }
    }

    if (!match) 
    {
        return AT_STATUS_NOT_FOUND;
    }

    In->readOffset += offset + 2;   /* Skip over 'AT'<command type> */

    Command->type = (AtCommand)(i | (0x0100 * t));

    /* Check for Test and Read commands. */
    len = In->writeOffset - In->readOffset;

    if (((len == 1) || (len == 2)) && (In->buff[In->writeOffset - 1] == '?')) {
        if (In->buff[In->readOffset] == '=')
            Command->type |= AT_TEST;
        else Command->type |= AT_READ;

        In->readOffset += len;

        status = AT_STATUS_OK;
        goto exit;
    }
    else if (len && (In->buff[In->readOffset] == '=')) {
        /* Strip off equals sign between command and value */
        In->readOffset++;
    }

    Assert(In->readOffset <= In->writeOffset);

    switch (t) {
    case 0:
        status = ME_Decode_Common(Atc, &Command->type, Command, In);
        break;

#if AT_HEADSET == XA_ENABLED
    case 1:
        status = ME_Decode_Headset(Atc, &Command->type, &Command->p.hs, In);
        break;
#endif /* AT_HEADSET == XA_ENABLED */

#if AT_HANDSFREE == XA_ENABLED
    case 2:
        status = ME_Decode_Handsfree(Atc, &Command->type, &Command->p.hf, In);
        break;
#endif /* AT_HANDSFREE == XA_ENABLED */

#if AT_PHONEBOOK == XA_ENABLED
    case 3:
        status = ME_Decode_Phonebook(Atc, &Command->type, &Command->p.pb, In);
        break;
#endif /* AT_PHONEBOOK == XA_ENABLED */

#if AT_SMS == XA_ENABLED
	case 4:
		status = ME_Decode_Sms(Atc, &Command->type, &Command->p.sms, In);
		break;
#endif /* AT_SMS == XA_ENABLED */


    default:
        Assert(0);
        break;
    }

exit:
	if(status != AT_STATUS_CONTINUE)
    In->readOffset = In->writeOffset = 0;
	else
		Command->bContinue = TRUE;
    return status;
}

#endif /* AT_ROLE_MOBILE == XA_ENABLED */


/*---------------------------------------------------------------------------
 *            AtItoA()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Convert integer value to base 10 ASCII. Returns number of
 *            bytes consumed by string (NOT null-terminated).
 */
U8 AtItoA(U8 *buff, U32 val)
{
    U8  temp[11];    /* holds string in reverse; length of "4294967295" */
    U8  i = 0, len;

    do {
        temp[i++] = (U8)((val % 10) + '0');
        val = val/10;
    } while (val > 0);

    len = i;

    while (i > 0) {
        *buff++ = temp[--i];
    }

    return len;
}


/*--------------------------------------------------------------------------- 
 * AtAtoI 
 * 
 *     Converts a base-10 number string to an unsigned 32-bit value. 
 */ 
U32 AtAtoI(const U8 *a, U8 len)
{ 
    U32     i = 0; 

    while (len-- && (*a >= '0') && (*a <= '9'))
        i = (i * 10) + (*a++ - '0'); 

    return i; 
} 


/*--------------------------------------------------------------------------- 
 * AtParseParameters
 * 
 *     Parses the result parameters.
 */ 
U8 AtParseParameters(U8 *input, U16 inputLen, AtParameter parms[AT_MAX_PARMS])
{
    U8      parmNum = 0;
    BOOL    inParm = FALSE;
    BOOL    inQuote = FALSE;
    U16     i;

    OS_MemSet((U8 *)parms, 0, (sizeof(AtParameter) * AT_MAX_PARMS));

    for (i = 0; i < inputLen; i++) {
        if (!inParm) {
            parms[parmNum].offset = i;
            inParm = TRUE;
        }

        if (input[i] == '\"')
            inQuote = ~inQuote;     /* inQuote = (inQuote ? FALSE : TRUE); */

        if (!inQuote && inParm && (input[i] == ',')) { 

            parms[parmNum].len = i - parms[parmNum].offset;
            inParm = FALSE;

            if (++parmNum == AT_MAX_PARMS) {
                Assert(0);
                break;
            }
        }
    }

    if (inParm) {
        parms[parmNum].len = i - parms[parmNum].offset;
        ++parmNum;
    }

    return parmNum;
}


/*---------------------------------------------------------------------------
 * AT_Range2MinMax
 *
 *     Returns the first and last values found in range strings that contain
 *     a list of integers, such as "(0,1)", "(0-5)" or "(0,1,2)". The input
 *     must be a null-terminated string, devoid of whitespace. On error *Min
 *     and *Max == 0xff. If only one value is present Min == Max.
 */
void AT_Range2MinMax(const char *Range, U16 *Min, U16 *Max)
{
    I8      i;
    U16     len;

    Assert(Min && Max && Range);

    *Min = *Max = 0xff;

    len = OS_StrLen(Range);

    /* Find first (minimum) value in range */
    for (i = 0; i < len; i++) {
        if (Range[i] < '0' || Range[i] > '9')
            continue;
        *Min = *Max = (U16)AtAtoI((const U8 *)(Range + i), (U8)(len - i));
        break;
    }

    /* Find last (maximum) value in range */
    for (i = len; i > 0; i--) {
        if (Range[i - 1] != ',' && Range[i - 1] != '-')
            continue;

        *Max = (U16)AtAtoI((const U8 *)(Range + i), (U8)(len - i));
        break;
    }
}


/*--------------------------------------------------------------------------- 
 * AtRemoveWhiteSpace
 * 
 *     Removes white space from the input.
 */ 
void AtRemoveWhiteSpace(XaBufferDesc *Input)
{
    U8      i;
    U8     *dst = Input->buff;
    U8      ch;
    BOOL    inQuote = FALSE;

    for (i = 0; i < Input->writeOffset; i++) {
        /* Scan forward consuming any whitespace characters. */
        ch = Input->buff[i];

        if (!inQuote && ((ch == ' ') || (ch == '\t'))) {
            continue;
        }
        
        if (ch == '\"')
            inQuote = ~inQuote;     /* inQuote = (inQuote ? FALSE : TRUE); */

        *dst++ = ch;
    }
    /* Set null terminator to prevent                     */ 
    /* some one use strlen to calculate the length */
    *dst = 0;
    /* Update the length. */
    Input->writeOffset = (U16)(dst - Input->buff);
}

/*--------------------------------------------------------------------------- 
 * AtMakeString
 *
 *     Null-terminates a string parameter and removes double-quotes if present.
 */
char *AtMakeString(U8 *Text, U16 Len)
{
    if (Len == 0) {
        /* Empty string result. */
        return "";
    }

    /* Strip quotes if they exist. */
    if ((Text[0] == '\"') && (Text[Len-1] == '\"')) {
        Text++;
        Len -= 2;
    }
    Text[Len] = '\0';

    return (char *)Text;
}

#if AT_ROLE_TERMINAL == XA_ENABLED

/*--------------------------------------------------------------------------- 
 * AT_Encode_Common
 * 
 *     Encodes TE originated AT commands. 
 */ 
AtStatus TE_Encode_Common(AtContext *Atc, U16 Type, const AtCommands *Command, XaBufferDesc *Output)
{
    U16     avail = (Output->buffSize - Output->writeOffset) -1;
    U16     len;

    switch (Type) {
    case AT_SET_ERROR_MODE:
        /* Syntax: AT+CMEE=[<n>] */
        if (avail < 1) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->buff[Output->writeOffset++] = Command->p.error.mode + '0';
        break;

#if (AT_HANDSFREE == XA_ENABLED) || (AT_PHONEBOOK == XA_ENABLED) || (AT_SMS == XA_ENABLED)
    case AT_SELECT_CHARACTER_SET:
        /* Syntax: AT+CSCS=[<chset>] */
        len = OS_StrLen(Command->p.charSet.type);
        if (avail < len) {
            return AT_STATUS_NO_RESOURCES;
        }
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8*)Command->p.charSet.type, len);
        Output->writeOffset += len;
        break;
#endif /* (AT_HANDSFREE == XA_ENABLED) || (AT_PHONEBOOK == XA_ENABLED) */

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }
    return AT_STATUS_OK;
}

/*--------------------------------------------------------------------------- 
 * AT_Decode_Common
 * 
 *     Decodes AT results received by the Common Unit (TE).
 */
AtStatus TE_Decode_Common(AtContext *Atc, U16 *Type, AtResults *Result, XaBufferDesc *Input)
{
    U8             *ptr = Input->buff + Input->readOffset;
    AtParameter     parms[AT_MAX_PARMS];

    AtParseParameters(ptr, (U16)(Input->writeOffset - Input->readOffset), parms);

    switch (*Type) {
    case AT_OK:         /* Syntax: OK */
    case AT_ERROR:      /* Syntax: ERROR */
    case AT_RING:       /* Syntax: RING */
    case AT_NO_CARRIER: /* Syntax: NO CARRIER */
    case AT_BUSY:       /* Syntax: BUSY */
    case AT_NO_ANSWER:  /* Syntax: NO ANSWER */
    case AT_DELAYED:    /* Syntax: DELAYED */
    case AT_BLACKLISTED:/* Syntax: BLACKLISTED */
        break;

    case AT_EXTENDED_ERROR:
        /* Syntax: +CME ERROR: <err> */
        if (parms[0].len)
            Result->p.error.type = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
        break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    Input->readOffset = Input->writeOffset = 0;
    return AT_STATUS_OK;
}
#endif /* AT_ROLE_TERMINAL == XA_ENABLED */


#if AT_ROLE_MOBILE == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * AT_Encode_Common
 * 
 *     Encodes Audio Gateway (ME) originated AT results.
 */ 
AtStatus ME_Encode_Common(AtContext *Atc, U16 Type, const AtResults *Result, XaBufferDesc *Output)
{
    U16 	 len;
    U16     avail = (Output->buffSize - Output->writeOffset) - 2;

    switch (Type) {
    case AT_OK:         /* Syntax: OK */
    case AT_ERROR:      /* Syntax: ERROR */
    case AT_RING:       /* Syntax: RING */
    case AT_NO_CARRIER: /* Syntax: NO CARRIER */
    case AT_BUSY:       /* Syntax: BUSY */
    case AT_NO_ANSWER:  /* Syntax: NO ANSWER */
    case AT_DELAYED:    /* Syntax: DELAYED */
    case AT_BLACKLISTED:/* Syntax: BLACKLISTED */
        /* No parameters, yank the ':' & ' ' */
        Output->writeOffset -= 2;
        break;

    case AT_EXTENDED_ERROR:
        /* Syntax: +CME ERROR: <err> */
        if (avail < 3) {
            return AT_STATUS_NO_RESOURCES;
        }

        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, 
                                      Result->p.error.type);
        break;

    case AT_MANUFACTURE_ID:
		/* Subtract "<Cr><Lf>+CGMI: " */
		/* Output->writeOffset -= 7; */ /* For WISE requirement */
		Output->buff[Output->writeOffset++] = '\"';
		OS_MemCopy(Output->buff + Output->writeOffset, (const U8*)Result->p.manufactureID, OS_StrLen((const char *)Result->p.manufactureID));
		Output->writeOffset += OS_StrLen((const char *)Result->p.manufactureID);
		Output->buff[Output->writeOffset++] = '\"';
		break;
    case AT_MODEL_ID:
		/* Subtract "<Cr><Lf>+CGMM: " */
		/* Output->writeOffset -= 7; */ /* For WISE requirement */
		Output->buff[Output->writeOffset++] = '\"';		
		OS_MemCopy(Output->buff + Output->writeOffset, (const U8*)Result->p.modelID, OS_StrLen((const char *)Result->p.modelID));
		Output->writeOffset += OS_StrLen((const char *)Result->p.modelID);		
		Output->buff[Output->writeOffset++] = '\"';
		break;

    case (AT_SELECT_CHARACTER_SET|AT_TEST):
		#if 1
		len = OS_StrLen(Result->p.charset_test);
		if(len == 0)
		{
			return AT_STATUS_INVALID_PARM;
		}
		else if(avail < len+2)
		{
			return AT_STATUS_NO_RESOURCES;
		}
		else
		{
			OS_MemCopy(Output->buff + Output->writeOffset, (const U8*)Result->p.charset_test, len);
			Output->writeOffset += len;
		}
		#else
		if(Result->p.charset_test > 0)
		{
			U8 i, offset = Output->writeOffset;

			if(avail < 2)
				return AT_STATUS_NO_RESOURCES;
			else
				avail -= 2;
			
			Output->buff[Output->writeOffset++] = '(';
			for(i = 0;i < Result->p.charset_test.num; i++)
			{
				if( i != 0 )
				{
					if(avail == 0)
						return AT_STATUS_NO_RESOURCES;
					Output->buff[Output->writeOffset++] = ',';
				}
				
				len = OS_StrLen(Result->p.charset_test.charset[i]);
				if( avail < len+2 )
				{
					return AT_STATUS_NO_RESOURCES;
				}
				else
				{
					Output->buff[Output->writeOffset++] = '\"';
					OS_MemCopy(Output->buff + Output->writeOffset,Result->p.charset_test.charset[i], len);
					Output->writeOffset += len;					
					Output->buff[Output->writeOffset++] = '\"';
					avail -= (len+2);
				}				
		   	}
			Output->buff[Output->writeOffset++] = ')';
	   	}
		else
		{
			return AT_STATUS_INVALID_PARM;
		}
		#endif
		break;

    case AT_SELECT_CHARACTER_SET|AT_READ:
		len = OS_StrLen(Result->p.charset);
		if(len == 0)
		{
			return AT_STATUS_INVALID_PARM;
		}
		else if(avail < len+2)
		{
			return AT_STATUS_NO_RESOURCES;
		}
		else
		{
			Output->buff[Output->writeOffset++] = '\"';
			OS_MemCopy(Output->buff + Output->writeOffset, (const U8*)Result->p.charset, len);
			Output->writeOffset += len;
			Output->buff[Output->writeOffset++] = '\"';
		}
		break;
		
    case AT_RAW:
        Output->writeOffset -= 7; /* Subtract "Cr Lf RAW: " */
        OS_MemCopy(Output->buff + Output->writeOffset, 
                   Result->p.data, OS_StrLen((const char *)Result->p.data));
        Output->writeOffset += OS_StrLen((const char *)Result->p.data);
        break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
        
/*--------------------------------------------------------------------------- 
 * AT_Decode_Common
 * 
 *     Decodes AT commands received by the Gateway (ME).
 */
AtStatus ME_Decode_Common(AtContext *Atc, U16 *Type, AtCommands *Command, XaBufferDesc *Input)
{
    U8             *ptr = Input->buff + Input->readOffset;
    AtParameter     parms[AT_MAX_PARMS];

    AtParseParameters(ptr, (U16)(Input->writeOffset - Input->readOffset), parms);

	switch (*Type) 
	{
    case AT_SET_ERROR_MODE:
        /* Syntax: AT+CMEE=[<n>] */
        if (parms[0].len)
            Command->p.error.mode = ptr[parms[0].offset] - '0';
        break;

#if (AT_HANDSFREE == XA_ENABLED) || (AT_PHONEBOOK == XA_ENABLED) || (AT_SMS == XA_ENABLED)
    case AT_SELECT_CHARACTER_SET:
        /* Syntax: AT+CSCS=[<chset>] */
        /* Temporary mark this for MAUI, because HFP ADP send ATCMD string directly to L4. Call 
            AtMakeString will strip tail \" character that will make the AT CMD incomplete */
		Command->p.charSet.type = AtMakeString(ptr + parms[0].offset, parms[0].len);
        break;
#endif /* (AT_HANDSFREE == XA_ENABLED) || (AT_PHONEBOOK == XA_ENABLED) */

	case AT_MANUFACTURE_ID:
	case AT_MODEL_ID:
		break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    Input->readOffset = Input->writeOffset = 0;
    return AT_STATUS_OK;
}
#endif /* AT_ROLE_MOBILE == XA_ENABLED */

#if XA_DEBUG_PRINT == XA_ENABLED
#define LINE_WIDTH      16

void AtDebugOut(XaBufferDesc *Input)
{
    const U8 *buffer, *p;
    U16       len;
    U16       po;

    p = buffer = Input->buff + Input->readOffset;
    len = Input->writeOffset - Input->readOffset;

    po = 0;
    while (p < buffer+len) {

        /* Dumps the data in basic ASCII dump format */
        po = 0;
        while (po < LINE_WIDTH) {
            if (p+po < buffer+len)
            {
                bt_trace(TRACE_GROUP_1,BTBT_DEBUGx02X , p[po]);
            }
            else ;
            if (++po == LINE_WIDTH/2) ;
        }
        ;

        po = 0;
        while (po < LINE_WIDTH) {
            if (p+po < buffer+len)
            {
                bt_trace(TRACE_GROUP_1,x1C , ((p[po] >= ' ' && p[po] <= '~') ? p[po] : '.'));
            }
            else break;
            po++;
        }

        ;
        p += po;
    }
}

#endif /* XA_DEBUG_PRINT == XA_ENABLED */
