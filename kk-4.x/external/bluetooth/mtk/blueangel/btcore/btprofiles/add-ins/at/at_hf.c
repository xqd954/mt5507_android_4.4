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
 *     $Workfile:at_hf.c$ for iAnywhere Blue SDK, Version 2.1.1
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

#if AT_HANDSFREE == XA_ENABLED

/*
 * Handsfree Commands and Responses
 */ 
const char *AT_HandsfreeCmds[] = {
#if defined(USE_EXT_AT_PARSER)
    "\0",                 // "A"
    "\0",                 // "D>"
    "\0",                 // "D"
    "\0",                 // "+CCWA"
    "+CHLD",      // "+CHLD"
    "\0",                 // "+CHUP"
    "+CIND",       // "+CIND"
    "\0",                 // "+CLIP"
    "+CMER",                 // "+CMER"
    "\0",                 // "+CIEV"
    "\0",                 // "+VTS"
    "\0",                 // "+BINP"
    "\0",                 // "+BLDN"
    "\0",                 // "+BVRA"
    "+BRSF",       // "+BRSF"
    "\0",                 // "+NREC"
    "\0",                 // "+BSIR"
    "\0",                 // "+BTRH"
    "\0",                 // "+CNUM"
    "\0",                 // "+CLCC"
    "\0",                 // "+COPS"
#else
    /* AT capabilities from GSM 07.07 and V.25ter */
    "A",     /* 0x0200 Call answer command. */ 
    "D>",    /* 0x0201 memory dialing. */ 
    "D",     /* 0x0202 place a call to a phone number. */
    "+CCWA", /* 0x0203 Call Waiting notification AT command. */
    "+CHLD", /* 0x0204 Call hold and multiparty handling AT command. */
    "+CHUP", /* 0x0205 Hang-up AT command. */
    "+CIND", /* 0x0206 Indicator update AT command. */
    "+CLIP", /* 0x0207 Calling Line Identification notification. */
    "+CMER", /* 0x0208 ME Event reporting activation. */
    "+CIEV", /* 0x0209 Indicator events reporting unsolicited result code. */
    "+VTS",  /* 0x020A DTMF generation AT command. */
    /* Bluetooth specific AT capabilities */
    "+BINP", /* 0x020B Command used for requesting the phone number of the last voice tag. */
    "+BLDN", /* 0x020C Command used for calling the last phone number dialed. */
    "+BVRA", /* 0x020D Enables/disables the voice recognition function in the AG. */
    "+BRSF", /* 0x020E Retrieve Supported Features. */
    "+NREC", /* 0x020F Command issued to disable any Echo Canceling and Noise reduction. */
    "+BSIR", /* 0x0210 Unsolicited result code to indicate that the in-band ring tone has been changed. */
    "+BTRH", /* 0x0211 Used by the HF to query the current "Response and Hold" state of the AG. */
    "+CNUM", /* 0x0212 Used to request the Subscriber Number information from the ME. */
    "+CLCC", /* 0x0213 List Current calls. */
    "+COPS", /* 0x0214 Used for reading and setting the network operator. */
    "+BIA",  /* 0x0215 Used for activating or deactivating the indicators individually. */
    "+BCC",  /* 0x0216 Used for requesting the AG to start the codec connection procedure. */
    "+BCS",  /* 0x0217 Used for confirming to the AG the codec. */    
    "+BAC",  /* 0x0218 Used for informing the AG about what codecs that the HF supports. */       
#endif
    0
};


#if AT_ROLE_TERMINAL == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * AT_Encode_Handsfree
 * 
 *     Encodes Handsfree Device (TE) originated AT commands. 
 */ 
AtStatus TE_Encode_Handsfree(AtContext *Atc, U16 Type, const AtHandsfreeCmd *Command, XaBufferDesc *Output)
{
    AtStatus    status = AT_STATUS_NO_RESOURCES;
    U16         len;
    U16         avail = (Output->buffSize - Output->writeOffset) - 1;
    I8          i;
 
    switch (Type) {
    case AT_ANSWER:             /* Syntax: ATA */
    case AT_HANG_UP:            /* Syntax: AT+CHUP */
    case AT_REDIAL:             /* Syntax: AT+BLDN */
    case AT_SUBSCRIBER_NUM:     /* Syntax: AT+CNUM */
    case AT_LIST_CURRENT_CALLS: /* Syntax: AT+CLCC */
    case AT_CODEC_CONNECTION:   /* Syntax: AT+BCC */        
        /* No parameters, yank the '=' */
        Output->writeOffset--;
        break;

    case AT_DIAL_NUMBER:
        /* Syntax: ATDdd.dd */
    case AT_DIAL_MEMORY:
        /* Syntax: ATD>nnn */
        len = OS_StrLen(Command->dial.number);
        if (avail < (len + 1)) {
            goto exit;
        }

        /* V.25ter style command, yank the '=' */
        Output->writeOffset--;

        OS_MemCopy(Output->buff + Output->writeOffset, (const U8*)Command->dial.number, len);

        Output->writeOffset += len;
        Output->buff[Output->writeOffset++] = ';';
        break;

    case AT_CALL_WAIT_NOTIFY:
        /* Syntax: AT+CCWA=[<n>[,<mode>[,<class>]]] */
        if (avail < 1) {
            goto exit;
        }
        Output->buff[Output->writeOffset++] = (Command->wait.notify ? '1' : '0');
        break;

    case AT_CALL_HOLD:
        /* Syntax: AT+CHLD=<n> */
        if (avail < 2) {
            goto exit;
        }

        Output->buff[Output->writeOffset++] = Command->hold.action + '0';
        
        if (((Command->hold.action == 1) || (Command->hold.action == 2)) &&
            ((Command->hold.call > 0) && (Command->hold.call < 10))) {
            /* Add the call number to the command */
            Output->buff[Output->writeOffset++] = Command->hold.call + '0';
        }
        break;

    case AT_CALL_ID:
        /* Syntax: AT+CLIP=[<n>] */
        if (avail < 1) {
            goto exit;
        }

        Output->buff[Output->writeOffset++] = (Command->callId.enabled ? '1' : '0');
        break;

    case AT_EVENT_REPORTING:
        /* Syntax: AT+CMER=[<mode>[,<keyp>[,<disp>[,<ind>[,<bfr>]]]]] */
        if (avail < 7) {
            goto exit;
        }
        Output->buff[Output->writeOffset++] = Command->report.mode + '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Command->report.ind + '0';
        break;

    case AT_GENERATE_DTMF_TONE: 
        /* Syntax: AT+VTS=<DTMF> */
        if (avail < 1) {
            goto exit;
        }
        
        Output->buff[Output->writeOffset++] = Command->dtmf.tone;
        break;

    case AT_VOICE_TAG:
        /* Syntax: AT+BINP=<datarequest> */
        if (avail < 1) {
            goto exit;
        }
        
        Output->buff[Output->writeOffset++] = '1';
        break;

    case AT_VOICE_RECOGNITION:
        /* Syntax: AT+BVRA=<vrec> */
        if (avail < 1) {
            goto exit;
        }

        Output->buff[Output->writeOffset++] = (Command->vrec.enabled ? '1' : '0');
        break;

    case AT_SUPPORTED_FEATURES:
        /* Syntax: AT+BRSF=<HF supported features bitmap> */
        if (avail < 4) {
            goto exit;
        }
                
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, 
                                      Command->features.bitmap);
        break;

    case AT_ECHO_C_AND_NOISE_R:
        /* Syntax: AT+NREC=<nrec> */
        if (avail < 1) {
            goto exit;
        }
        
        Output->buff[Output->writeOffset++] = '0';
        break;

    case AT_RESPONSE_AND_HOLD:
        /* Syntax: AT+BTRH=<n> */
        if (avail < 2) {
            goto exit;
        }

        Output->buff[Output->writeOffset++] = Command->btrh.setting + '0';
        break;

    case AT_NETWORK_OPERATOR:
        /* Syntax: AT+COPS=[<mode>[,<format>[,<oper>]]] */
        if (avail < 3) {
            goto exit;
        }

        Output->buff[Output->writeOffset++] = Command->networkOper.mode + '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Command->networkOper.format + '0';
        break;

    case AT_INDICATOR_ACTIVATION:
        /* Syntax: AT+BIA=[[<indrep 1>][,[<indrep 2>][,...[,[<indreq n>]]]]]] */
        if ((Command->indActivation.num > AT_MAX_INDICATORS) || 
            (avail < (Command->indActivation.num * 4))) {
            goto exit;
        }

        for (i = 0; i < Command->indActivation.num; i++) {
            Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, 
                                          Command->indActivation.enabled[i]);
            Output->buff[Output->writeOffset++] = ',';
        }

        Output->writeOffset--; 
        break;   

    case AT_CODEC_SELECTION:
        /* Syntax: AT+BCS=<u> */
        if (avail < 1) {
            goto exit;
        }
        
        Output->buff[Output->writeOffset++] = Command->codecSelection.id;
        break;  

    case AT_AVAILABLE_CODECS:
        /* Syntax: AT+BAC=[<u1>[,<u2>[,...[,<un>]]]] */
        if ((Command->availCodec.num > AT_MAX_CODECS) || 
            (avail < (Command->availCodec.num * 4))) {
            goto exit;
        }
        
        for (i = 0; i < Command->availCodec.num; i++) {
            Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, 
                                          Command->availCodec.id[i]);
            Output->buff[Output->writeOffset++] = ',';
        }

        Output->writeOffset--; 
        break;         

    default:
        status = AT_STATUS_NOT_SUPPORTED;
        goto exit;
    }

    return AT_STATUS_OK;

exit:
    return status;
}

/*--------------------------------------------------------------------------- 
 * AT_Decode_Handsfree
 * 
 *     Decodes AT results received by the Handsfree Unit (TE).
 */ 
AtStatus TE_Decode_Handsfree(AtContext *Atc, U16 *Type, AtHandsfreeResult *Result, XaBufferDesc *Input)
{
    I8              i, x, p, s;
    BOOL            specific;
    U8             *ptr;
    AtParameter     parms[AT_MAX_PARMS];
    U8              flag = 0, numParms;

    ptr = Input->buff + Input->readOffset;

    numParms = AtParseParameters(ptr, (U16)(Input->writeOffset - Input->readOffset), parms);

    switch (*Type) {
    case AT_CALL_WAIT_NOTIFY:
        /* Syntax: +CCWA: <number>, <type>, <class> */
        if (parms[0].len > 0)
            Result->wait.number = AtMakeString(ptr + parms[0].offset, parms[0].len);

        if (parms[1].len > 0)
            Result->wait.type = (U8)AtAtoI(ptr + parms[1].offset, parms[1].len);

        if (parms[2].len > 0)
            Result->wait.classmap = (U8)AtAtoI(ptr + parms[2].offset, parms[2].len);
        break;

    case AT_CALL_HOLD:
        *Type |= AT_TEST;    /* Only valid as a test command */

        /* Syntax: [+CHLD:(list of supported <n>s)] */
        for (i = 0; i < AT_MAX_PARMS; i++) {
                
            for (specific = FALSE, x = 0; x < parms[i].len; x++) {
                if ((ptr[parms[i].offset + x] >= '0') && (ptr[parms[i].offset + x] <= '9'))
                    flag = ptr[parms[i].offset + x];
                else if ((ptr[parms[i].offset + x] == 'x') || (ptr[parms[i].offset + x] == 'X'))
                    specific = TRUE;
            }
            switch (flag) {
            case '0':
                Result->hold_test.flags |= AT_FLAG_HOLD_RELEASE_HELD_CALLS;
                break;
            case '1':
                if (specific)
                    Result->hold_test.flags |= AT_FLAG_HOLD_RELEASE_SPECIFIC_CALL;
                else Result->hold_test.flags |= AT_FLAG_HOLD_RELEASE_ACTIVE_CALLS;
                break;
            case '2':
                if (specific)
                    Result->hold_test.flags |= AT_FLAG_HOLD_HOLD_SPECIFIC_CALL;
                else Result->hold_test.flags |= AT_FLAG_HOLD_HOLD_ACTIVE_CALLS;
                break;
            case '3':
                Result->hold_test.flags |= AT_FLAG_HOLD_ADD_HELD_CALL;
                break;
            case '4':
                Result->hold_test.flags |= AT_FLAG_HOLD_EXPLICIT_TRANSFER;
                break;
            }
        }
        break;

    case AT_INDICATORS:
        /* The result be to either a TEST or READ command. */
        if (ptr[parms[0].offset] == '(') {
            /* Syntax: +CIND: (<desc>,(list of <ind>s)) [,(<desc>,(list of <ind>s))[,...]] */
            *Type |= AT_TEST;

            s = i = p = 0;
            while ((p < numParms) && (i < AT_MAX_INDICATORS)) {
                switch (s) {
                case 0: /* Parameter should be: "(<desc>" */
                    if (ptr[parms[p].offset] != '(')
                        break;

                    Result->indicators_test.ind[i].description = AtMakeString(ptr + parms[p].offset + 1, 
                                                                              (U16)(parms[p].len - 1));
                    s++;
                    break;

                case 1: /* Parameter should be: "(<ind>" or "(<ind>))" */
                    if (ptr[parms[p].offset] != '(')
                        break;

                    Result->indicators_test.ind[i].range = (const char*)(ptr + parms[p].offset + 1);
                    s++;
                    /* Drop into next case to handle single <ind> case. */

                case 2: /* Parameter should be: "<ind>" or "<ind>))" */
                    if ((ptr[parms[p].offset + parms[p].len - 1] == ')') &&
                        (ptr[parms[p].offset + parms[p].len - 2] == ')')) {
                        /* End of (list of <ind>s)) */
                        ptr[parms[p].offset + parms[p].len - 2] = '\0';

                        /* Advance indicator index and reset state */
                        i++;
                        s = 0;
                    }
                    break;
                }
                p++;        /* Next parameter */
            }

            Result->indicators_test.num = (U8)i;

        } else {
            /* Syntax: +CIND:[<ind>[,<ind>[,...]]] */
            *Type |= AT_READ;

            for (i = 0; (i < numParms) && (i <AT_MAX_INDICATORS); i++)
                Result->indicators_read.ind[i].value = (U8)AtAtoI(ptr + parms[i].offset, parms[i].len);

            Result->indicators_test.num = numParms;
        }
        break;

    case AT_CALL_ID:
        /* Syntax: +CLIP:<number>,<type>[,<subaddr>,<satype>[,[<alpha>] [,<CLI validity>]]] */
        Result->callId.number = AtMakeString(ptr + parms[0].offset, parms[0].len);
        Result->callId.type = (U8)AtAtoI(ptr + parms[1].offset, parms[1].len);
        break;

    case AT_INDICATOR_EVENT:
        /* Syntax: +CIEV: <ind>,<value> */
        Result->event.indicator = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
        Result->event.value = (U8)AtAtoI(ptr + parms[1].offset, parms[1].len);
        break;

    case AT_VOICE_TAG:
        /* Syntax: +BINP:<dataresp> */
        Result->voiceTag.number = AtMakeString(ptr + parms[0].offset, parms[0].len);
        break;

    case AT_VOICE_RECOGNITION:
        /* Syntax: +BVRA=<vrec> */
        if (parms[0].len > 0) {
            if (ptr[parms[0].offset] != '0')
                Result->vrec.enabled = TRUE;
        }
        break;

    case AT_SUPPORTED_FEATURES:
        /* Syntax: +BRSF: <AG supported features bitmap> */
        Result->features.bitmap = AtAtoI(ptr + parms[0].offset, parms[0].len);
        break;

    case AT_IN_BAND_RING_TONE:
        /* Syntax: +BSIR: <bsir> */
        if (parms[0].len > 0) {
            if (ptr[parms[0].offset] != '0')
                Result->inBandRing.enabled = TRUE;
        }
        break;

    case AT_RESPONSE_AND_HOLD:
        /* Syntax: +BTRH=<n> */
        if (parms[0].len > 0)
            Result->btrh.setting = ptr[parms[0].offset] - '0';
        break;

    case AT_SUBSCRIBER_NUM:
        /* Syntax: +CNUM: [<alpha2>],<number2>,<type2>[,<speed>,<service>[,<itc>]] */
        Result->subscribeNum.number = AtMakeString(ptr + parms[1].offset, parms[1].len);

        if (parms[2].len > 0)
            Result->subscribeNum.type = (U8)AtAtoI(ptr + parms[2].offset, parms[2].len);

        if (parms[4].len > 0)
            Result->subscribeNum.service = ptr[parms[4].offset] - '0';
        break;

    case AT_LIST_CURRENT_CALLS:
        /* Syntax: [+CLCC: <id1>,<dir>,<stat>,<mode>,<mpty>[,<number>,<type>[,<alpha>]] */
        if (parms[0].len > 0)
            Result->currentCalls.index = ptr[parms[0].offset] - '0';
        else
            return AT_STATUS_NOT_SUPPORTED;

        /* Some Mot phones don't inlude this bit */
        if (parms[1].len > 0)
            Result->currentCalls.dir = ptr[parms[1].offset] - '0';
        else
            Result->currentCalls.dir = 0xFF;

        if (parms[2].len > 0)
            Result->currentCalls.state = ptr[parms[2].offset] - '0';
        else
            return AT_STATUS_NOT_SUPPORTED;

        if (parms[3].len > 0)
            Result->currentCalls.mode = ptr[parms[3].offset] - '0';
        else
            return AT_STATUS_NOT_SUPPORTED;

        if (parms[4].len > 0)
            Result->currentCalls.multiParty = ptr[parms[4].offset] - '0';
        else
            return AT_STATUS_NOT_SUPPORTED;

        Result->currentCalls.number = AtMakeString(ptr + parms[5].offset, parms[5].len);

        if (parms[6].len > 0)
            Result->currentCalls.type = (U8)AtAtoI(ptr + parms[5].offset, parms[5].len);
        break;

    case AT_NETWORK_OPERATOR:
        /* Syntax: +COPS: <mode>[,<format>,<oper>] */
        if (parms[0].len && ptr[parms[0].offset] == '(') {
            /* This looks like a test result, which we don't support. */
            return AT_STATUS_NOT_SUPPORTED;
        }
        *Type |= AT_READ;

        if (parms[0].len > 0)
            Result->networkOper_read.mode = ptr[parms[0].offset] - '0';
        if (parms[1].len > 0)
            Result->networkOper_read.format = ptr[parms[1].offset] - '0';
        if (parms[2].len > 0)
            Result->networkOper_read.oper = AtMakeString(ptr + parms[2].offset, parms[2].len);
        break;

    case AT_CODEC_SELECTION:
        /* Syntax: +BCS:<u> */
        Result->codecSelection.id = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);        
        break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
#endif /* AT_ROLE_TERMINAL == XA_ENABLED */


#if AT_ROLE_MOBILE == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * AT_Encode_Handsfree
 * 
 *     Encodes Audio Gateway (ME) originated AT results.
 */ 
AtStatus ME_Encode_Handsfree(AtContext *Atc, U16 Type, const AtHandsfreeResult *Result, XaBufferDesc *Output)
{
    AtStatus    status = AT_STATUS_NO_RESOURCES;
    U16         avail = (Output->buffSize - Output->writeOffset) - 2;
    U16         len;
    I8          i;

    switch (Type) {
    case AT_CALL_WAIT_NOTIFY:
        /* Syntax: +CCWA: <number>, <type>, <class> */
        if (Result->wait.number)
        {
            len = OS_StrLen(Result->wait.number);
        }
        else
        {
            len = 0;
        }

        if (avail < (len + 4)) {
            goto exit;
        }
        Output->buff[Output->writeOffset++] = '\"';
        if (len)
        {
            OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)Result->wait.number, len);
            Output->writeOffset += len;
        }
        Output->buff[Output->writeOffset++] = '\"';

        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->wait.type);
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Result->wait.classmap + '0';
        break;

    case AT_CALL_HOLD|AT_TEST:
        /* Syntax: [+CHLD:(list of supported <n>s)] */
        if (Result->hold_test.flags == 0) {
            status = AT_STATUS_INVALID_PARM;
            goto exit;
        }
            
        if (avail < 17) {
            goto exit;
        }

        Output->buff[Output->writeOffset++] = '(';
        
        for (i = 0; i < 7; i++) {
            if ((Result->hold_test.flags & (1 << i)) == 0)
                continue;

            switch (1 << i) {
            case AT_FLAG_HOLD_RELEASE_HELD_CALLS:
                Output->buff[Output->writeOffset++] = '0';
                break;
            case AT_FLAG_HOLD_RELEASE_ACTIVE_CALLS:
                Output->buff[Output->writeOffset++] = '1';
                break;
            case AT_FLAG_HOLD_RELEASE_SPECIFIC_CALL:
                Output->buff[Output->writeOffset++] = '1';
                Output->buff[Output->writeOffset++] = 'x';
                break;
            case AT_FLAG_HOLD_HOLD_ACTIVE_CALLS:
                Output->buff[Output->writeOffset++] = '2';
                break;
            case AT_FLAG_HOLD_HOLD_SPECIFIC_CALL:
                Output->buff[Output->writeOffset++] = '2';
                Output->buff[Output->writeOffset++] = 'x';
                break;
            case AT_FLAG_HOLD_ADD_HELD_CALL:
                Output->buff[Output->writeOffset++] = '3';
                break;
            case AT_FLAG_HOLD_EXPLICIT_TRANSFER:
                Output->buff[Output->writeOffset++] = '4';
                break;
            }
            Output->buff[Output->writeOffset++] = ',';
        }
        /* Overwrite trailing comma */
        Output->buff[(Output->writeOffset-1)] = ')';
        break;

    case AT_INDICATORS|AT_TEST:
        /* Syntax: +CIND: (<desc>,(list of <ind>s)) [,(<desc>,(list of <ind>s))[,...]] */
        if (Result->indicators_test.num > AT_MAX_INDICATORS) {
            status = AT_STATUS_INVALID_PARM;
            goto exit;
        }
        
        for (i = len = 0; i < Result->indicators_test.num; i++)
            len += OS_StrLen(Result->indicators_test.ind[i].range) + 8 +
                   OS_StrLen(Result->indicators_test.ind[i].description);

        if (avail < len) {
            goto exit;
        }

        for (i = 0; i < Result->indicators_test.num; i++) {
            Output->buff[Output->writeOffset++] = '(';
            Output->buff[Output->writeOffset++] = '\"';
            len = OS_StrLen(Result->indicators_test.ind[i].description);
            OS_MemCopy(Output->buff + Output->writeOffset, 
                       (const U8 *)Result->indicators_test.ind[i].description, len);
            Output->writeOffset += len;
            Output->buff[Output->writeOffset++] = '\"';
            Output->buff[Output->writeOffset++] = ',';
            Output->buff[Output->writeOffset++] = '(';
            len = OS_StrLen(Result->indicators_test.ind[i].range);
            OS_MemCopy(Output->buff + Output->writeOffset, 
                       (const U8 *)Result->indicators_test.ind[i].range, len);
            Output->writeOffset += len;
            Output->buff[Output->writeOffset++] = ')';

            Output->buff[Output->writeOffset++] = ')';
            Output->buff[Output->writeOffset++] = ',';
        }
        Output->writeOffset--;      /* Removes trailing comma */
        break;
        
    case AT_INDICATORS|AT_READ:
        /* Syntax: +CIND=[<ind>[,<ind>[,...]]] */
        if (Result->indicators_test.num > AT_MAX_INDICATORS) {
            status = AT_STATUS_INVALID_PARM;
            goto exit;
        }

        if (avail < (Result->indicators_read.num * 4)) {
            goto exit;
        }

        for (i = 0; i < Result->indicators_read.num; i++) {
            Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, 
                                          Result->indicators_read.ind[i].value);
            Output->buff[Output->writeOffset++] = ',';
        }

        Output->writeOffset--;          /* Remove the trailing comma. */
        break;

    case AT_CALL_ID:
        /* Syntax: +CLIP:<number>,<type>[,<subaddr>,<satype>[,[<alpha>] [,<CLI validity>]]] */
        if (Result->callId.number)
        {
            len = OS_StrLen(Result->callId.number);
        }
        else
        {
            len = 0;
        }

        if (avail < (len + 4)) {
            goto exit;
        }
        Output->buff[Output->writeOffset++] = '\"';
        if (len)
        {
            OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)Result->callId.number, len);
            Output->writeOffset += len;
        }
        Output->buff[Output->writeOffset++] = '\"';
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->callId.type);
        break;

    case AT_INDICATOR_EVENT:
        /* Syntax: +CIEV: <ind>,<value> */
        if (avail <  7) {
            goto exit;
        }
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->event.indicator);
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->event.value);
        break;

    case AT_VOICE_TAG:
        /* Syntax: +BINP:<dataresp> */
        len = OS_StrLen(Result->voiceTag.number);
        if (avail < len) {
            goto exit;
        }
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)Result->voiceTag.number, len);
        Output->writeOffset += len;
        break;

    case AT_VOICE_RECOGNITION:
        /* Syntax: +BVRA=<vrec> */
        if (avail < 1) {
            goto exit;
        }

        Output->buff[Output->writeOffset++] = (Result->vrec.enabled ? '1' : '0');
        break;

    case AT_SUPPORTED_FEATURES:
        /* Syntax: +BRSF: <AG supported features bitmap> */
        if (avail < 4) {
            goto exit;
        }
        
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, 
                                      Result->features.bitmap);
        break;

    case AT_IN_BAND_RING_TONE:
        /* Syntax: +BSIR: <bsir> */
        if (avail < 1) {
            goto exit;
        }

        Output->buff[Output->writeOffset++] = (Result->inBandRing.enabled ? '1' : '0');
        break;

    case AT_RESPONSE_AND_HOLD:
        /* Syntax: +BTRH=<n> */
        if (avail < 2) {
            goto exit;
        }

        Output->buff[Output->writeOffset++] = Result->btrh.setting + '0';
        break;

    case AT_SUBSCRIBER_NUM:
        /* Syntax: +CNUM: [<alpha1>],<number1>,<type1>[,<speed>,<service>[,<itc>]] */
        if(Result->subscribeNum.number)
            len = OS_StrLen(Result->subscribeNum.number);
        else
            len = 0;
        if (avail < (len + 8)) {
            goto exit;
        }
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = '\"';
        if(len)
        {
            OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)Result->subscribeNum.number, len);
            Output->writeOffset += len;
        }
        Output->buff[Output->writeOffset++] = '\"';
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, 
                                      Result->subscribeNum.type);
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Result->subscribeNum.service + '0';
        break;

    case AT_LIST_CURRENT_CALLS:
        /* Syntax: [+CLCC: <id1>,<dir>,<stat>,<mode>,<mpty>[,<number>,<type>[,<alpha>]] */
        if(Result->currentCalls.number)
            len = OS_StrLen(Result->currentCalls.number);
        else
            len= 0;
        if (avail < (len + 12)) {
            goto exit;
        }
        Output->buff[Output->writeOffset++] = Result->currentCalls.index + '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Result->currentCalls.dir + '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Result->currentCalls.state + '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Result->currentCalls.mode + '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Result->currentCalls.multiParty + '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = '\"';
        if(len)
        {
            OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)Result->currentCalls.number, len);
            Output->writeOffset += len;
        }
        Output->buff[Output->writeOffset++] = '\"';
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, 
                                      Result->currentCalls.type);
        break;

    case AT_NETWORK_OPERATOR|AT_READ:
        /* Syntax: +COPS: <mode>[,<format>,<oper>] */
        if (Result->networkOper_read.oper)
        {
            len = OS_StrLen(Result->networkOper_read.oper);
        }
        else
        {
            len = 0;
        }

        if (avail < (4 + len)) {
            goto exit;
        }

        Output->buff[Output->writeOffset++] = Result->networkOper_read.mode + '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Result->networkOper_read.format + '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = '\"';
        if (len)
        {
            OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)Result->networkOper_read.oper, len);
            Output->writeOffset += len;            
        }
        Output->buff[Output->writeOffset++] = '\"';
        break;

    case AT_CODEC_SELECTION:
        /* Syntax: +BCS:<u> */
        if (avail < 1) {
            goto exit;
        }
        
        Output->buff[Output->writeOffset++] = Result->codecSelection.id + '0';
        break;         
        
    default:
        status = AT_STATUS_NOT_SUPPORTED;
        goto exit;
    }

    return AT_STATUS_OK;

exit:
    return status;
}
        
/*--------------------------------------------------------------------------- 
 * AT_Decode_Handsfree
 * 
 *     Decodes AT commands received by the Audio Gateway (ME).
 */ 
AtStatus ME_Decode_Handsfree(AtContext *Atc, U16 *Type, AtHandsfreeCmd *Command, XaBufferDesc *Input)
{
    U8             *ptr;
    AtParameter     parms[AT_MAX_PARMS];
    int             i = 0;

    ptr = Input->buff + Input->readOffset;

    AtParseParameters(ptr, (U16)(Input->writeOffset - Input->readOffset), parms);

    switch (*Type) {
    case AT_ANSWER:             /* Syntax: ATA */
    case AT_HANG_UP:            /* Syntax: AT+CHUP */
    case AT_REDIAL:             /* Syntax: AT+BLDN */
    case AT_SUBSCRIBER_NUM:     /* Syntax: AT+CNUM */
    case AT_LIST_CURRENT_CALLS: /* Syntax: AT+CLCC */
    case AT_CODEC_CONNECTION:   /* Syntax: AT+BCC */          
        break;

    case AT_DIAL_NUMBER:
        /* Syntax: ATDdd.dd */
    case AT_DIAL_MEMORY:
        /* Syntax: ATD>nnn */
        /* shall not remove the semi-colon */
        if (parms[0].len > 0) {
            /* Strip trailing semi-colon if present. */
            if ((ptr[parms[0].offset + parms[0].len - 1]) == ';')
                parms[0].len--;

            Command->dial.number = AtMakeString(ptr + parms[0].offset, parms[0].len);
        }
        break;

    case AT_CALL_WAIT_NOTIFY:
        /* Syntax: AT+CCWA=[<n>[,<mode>[,<class>]]] */
        if (parms[0].len > 0) {
            if (ptr[parms[0].offset] != '0')
                Command->wait.notify = TRUE;
        }
        break;

    case AT_CALL_HOLD:
        /* Syntax: AT+CHLD=<n> */
        if (parms[0].len > 0) {
            /* Action can be (0-4) and call can be (1-9) */
            Command->hold.action = ptr[parms[0].offset] - '0';

            /* Add the call number to the command? */
            if (parms[0].len > 1)
                Command->hold.call = ptr[parms[0].offset + 1] - '0';
        }
        break;

    case AT_CALL_ID:
        /* Syntax: AT+CLIP=[<n>] */
        if (parms[0].len > 0) {
            if (ptr[parms[0].offset] != '0')
                Command->callId.enabled = TRUE;
        }
        break;

    case AT_EVENT_REPORTING:
        /* Syntax: AT+CMER=[<mode>[,<keyp>[,<disp>[,<ind>[,<bfr>]]]]] */
        if (parms[0].len > 0)
            Command->report.mode = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
        if (parms[3].len > 0)
            Command->report.ind = (U8)AtAtoI(ptr + parms[3].offset, parms[3].len);
        break;

    case AT_GENERATE_DTMF_TONE: 
        /* Syntax: AT+VTS=<DTMF> */
        if (parms[0].len > 0)
            //Command->dtmf.tone = ptr[parms[0].offset];
			Command->dtmf.tone = *AtMakeString(ptr + parms[0].offset, parms[0].len);
        break;
        
    case AT_VOICE_TAG:
        /* Syntax: AT+BINP=<datarequest> */
        if (parms[0].len > 0) {
            /* <datarequest> must be '1' for VOICE_TAG. */
            if (ptr[parms[0].offset] != '1') {
                return AT_STATUS_NOT_SUPPORTED;
            }
        }
        break;

    case AT_VOICE_RECOGNITION:
        /* Syntax: AT+BVRA=<vrec> */
        if (parms[0].len > 0) {
            if (ptr[parms[0].offset] != '0')
                Command->vrec.enabled = TRUE;
        }
        break;

    case AT_SUPPORTED_FEATURES:
        /* Syntax: AT+BRSF=<HF supported features bitmap> */
        Command->features.bitmap = AtAtoI(ptr + parms[0].offset, parms[0].len);
        break;

    case AT_ECHO_C_AND_NOISE_R:
        if (parms[0].len > 0) {
            /* <nrec> must be '0' */
            if (ptr[parms[0].offset] != '0') {
                return AT_STATUS_NOT_SUPPORTED;
            }
        }
        break;

    case AT_RESPONSE_AND_HOLD:
        /* Syntax: AT+BTRH=<n> */
        if (parms[0].len > 0)
            Command->btrh.setting = ptr[parms[0].offset] - '0';
        break;

    case AT_NETWORK_OPERATOR:
        /* Syntax: AT+COPS=[<mode>[,<format>[,<oper>]]] */
        if (parms[0].len > 0)
            Command->networkOper.mode = ptr[parms[0].offset] - '0';
        if (parms[1].len > 0)
            Command->networkOper.format = ptr[parms[1].offset] - '0';
        break;

    case AT_INDICATOR_ACTIVATION:
        /* Syntax: AT+BIA=[[<indrep 1>][,[<indrep 2>][,...[,[<indreq n>]]]]]] */
        for (i = 0; i < AT_MAX_INDICATORS; i++)
        {
            if (parms[i].len > 0)
            {
                Command->indActivation.changed[i] = TRUE;              
                if (AtAtoI(ptr + parms[i].offset, parms[i].len))
                {
                    Command->indActivation.enabled[i] = TRUE;
                }
                else
                {
                    Command->indActivation.enabled[i] = FALSE;
                }              
            }
            else
            {
                Command->indActivation.changed[i] = FALSE;
            }
        }
        Command->indActivation.num = AT_MAX_INDICATORS;
        break;   

    case AT_CODEC_SELECTION:
        /* Syntax: AT+BCS=<u> */
        if (parms[0].len > 0)
            Command->codecSelection.id = ptr[parms[0].offset] - '0';        
        break;  

    case AT_AVAILABLE_CODECS:
        /* Syntax: AT+BAC=[<u1>[,<u2>[,...[,<un>]]]] */
        for (i = 0; i < AT_MAX_INDICATORS; i++)
        {
            if (parms[i].len > 0)
            {
                Command->availCodec.id[Command->availCodec.num++] = ptr[parms[i].offset] - '0';  
            }
        }        
        break;           

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
#endif /* AT_ROLE_MOBILE == XA_ENABLED */


/*--------------------------------------------------------------------------- 
 * AT_MapHfIndicators
 *
 *     Builds Map table to convert remote indicator numbers to local indicator
 *     table. Map[remote_ind] = Local indicator "Known" table index.
 */
void AT_MapHfIndicators(const char *Known[], 
                        U8 Map[], 
                        const U8 maxNumInd, 
                        AtHandsfreeResult *Remote)
{
    I8      i, ii;
    char    indStage[INDICATOR_STAGE_LEN];

    Assert(Remote->indicators_test.num <= maxNumInd);
    for (ii = 0; ii < Remote->indicators_test.num; ii++) {
        Map[ii] = 0xff;

        /* Convert received indicator string to upper case. */
        for (i = 0; i < INDICATOR_STAGE_LEN; i++) {
            indStage[i] = ToUpper(Remote->indicators_test.ind[ii].description[i]);
            if (Remote->indicators_test.ind[ii].description[i] == '\0')
                break;
        }

        /* Match received indicator against all known indicators. */
        for (i = 0; Known[i]; i++) {
            if (!OS_StrCmp(Known[i], indStage)) {
                Map[ii] = (U8)i;
                break;
            }
        }
    }
}

#endif /* AT_HANDSFREE == XA_ENABLED */

