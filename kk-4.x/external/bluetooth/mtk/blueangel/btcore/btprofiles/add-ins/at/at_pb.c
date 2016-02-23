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
 *     $Workfile:at_pb.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains an Phonebook AT Command Processor.
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

#if AT_PHONEBOOK == XA_ENABLED

const char *AT_PhonebookCmds[] = {
    /* AT capabilities from GSM 07.07 and V.25ter */
    "+CPBS", /* Used to select phonebook memory storage. */
    "+CPBR", /* Used to read phonebook entries */
    "+CPBF", /* Used to find phonebook entries matching a string */
    "+CPBW", /* Used to write phonebook entry in a location */
    0
};

static const char *PbStorageTypes[] = {
    "DC", "FD", "LD", "MC", "ME", "MT", "RC", "SM", 0
};

#if AT_ROLE_TERMINAL == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * AT_Encode_Phonebook
 * 
 *     Encodes TE originated AT commands. 
 */ 
AtStatus TE_Encode_Phonebook(AtContext *Atc, U16 Type, const AtPhonebookCmd *Command, XaBufferDesc *Output)
{
    U16     avail = (Output->buffSize - Output->writeOffset) - 1;
    U16     len, nLen;
    U8      i;

    switch (Type) {
    case AT_SELECT_PHONEBOOK_STORAGE:
        /* AT+CPBS=<storage> */
        if (avail < 4) {
            return AT_STATUS_NO_RESOURCES;
        }
                
        for (i = 0; PbStorageTypes[i]; i++) {
            if (Command->storage.select == (1 << i)) {
                Output->buff[Output->writeOffset++] = '\"';
                Output->buff[Output->writeOffset++] = PbStorageTypes[i][0];
                Output->buff[Output->writeOffset++] = PbStorageTypes[i][1];
                Output->buff[Output->writeOffset++] = '\"';
                break;
            }
        }
        if (PbStorageTypes[i] == 0) {
            return AT_STATUS_INVALID_PARM;
        }
        break;

    case AT_READ_PHONEBOOK_ENTRY:
        /* Syntax: AT+CPBR=<index1>[,<index2>] */
        if (avail < 11) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Command->read.first);

        if (Command->read.first != Command->read.last) {
            Output->buff[Output->writeOffset++] = ',';
            Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                          Command->read.last);
        }
        break;

    case AT_FIND_PHONEBOOK_ENTRY:
        /* Syntax: AT+CPBF=<findtext> */
        len = OS_StrLen(Command->find.text);
        if (avail < len) {
            return AT_STATUS_NO_RESOURCES;
        }

        OS_MemCopy(Output->buff + Output->writeOffset, (const U8*)Command->find.text, len);
        Output->writeOffset += len;
        break;

    case AT_WRITE_PHONEBOOK_ENTRY:
        /* Syntax: AT+CPBW=[<index>][,<number>[,<type>[,<text>]]] */
        len = OS_StrLen(Command->write.text);
        nLen = OS_StrLen(Command->write.number);

        if (avail < (11 + nLen + len)) {
            return AT_STATUS_NO_RESOURCES;
        }
        
        if (Command->write.index > 0)
            Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                          Command->write.index);

        Output->buff[Output->writeOffset++] = ',';

        OS_MemCopy(Output->buff + Output->writeOffset, (const U8*)Command->write.number, nLen);
        Output->writeOffset += nLen;
        Output->buff[Output->writeOffset++] = ',';

        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Command->write.type);
        Output->buff[Output->writeOffset++] = ',';

        OS_MemCopy(Output->buff + Output->writeOffset, (const U8*)Command->write.text, len);
        Output->writeOffset += len;
        break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }
    return AT_STATUS_OK;
}

/*--------------------------------------------------------------------------- 
 * AT_Decode_Phonebook
 * 
 *     Decodes AT results received by the Common Unit (TE).
 */
AtStatus TE_Decode_Phonebook(AtContext *Atc, U16 *Type, AtPhonebookResult *Result, XaBufferDesc *Input)
{
    U8             *ptr = Input->buff + Input->readOffset;
    I8              i, p;
    AtParameter     parms[AT_MAX_PARMS];
    I8              numParms;

    numParms = AtParseParameters(ptr, (U16)(Input->writeOffset - Input->readOffset), parms);

    switch (*Type) {
    case AT_SELECT_PHONEBOOK_STORAGE:
        if (parms[0].len && (ptr[parms[0].offset] == '(')) {
            /* Syntax: +CPBS: (list of supported <storage>s) */
            *Type |= AT_TEST;
            
            parms[0].offset++;  /* Skip over '(' */

            for (p = 0; (parms[p].len > 1) && (p < AT_MAX_PARMS); p++) {

                if (ptr[parms[p].offset] == '\"' && (parms[p].len > 2))
                    parms[p].offset++;  /* Strip quote from storage type */
                
                for (i = 0; PbStorageTypes[i]; i++) {
                    if ((PbStorageTypes[i][0] == ToUpper(ptr[parms[p].offset])) &&
                        (PbStorageTypes[i][1] == ToUpper(ptr[parms[p].offset+1]))) {
                        /* Found a matching storage type. */
                        Result->storage_test.supported |= (1 << i);
                        break;
                    }
                }
            }
        } else {
            /* Syntax: +CPBS: <storage>[,<used>,<total>] */
            *Type |= AT_READ;

            if (ptr[parms[0].offset] == '\"' && (parms[0].len > 2))
                parms[0].offset++;  /* Strip quote from storage type */

            for (i = 0; PbStorageTypes[i]; i++) {
                if ((PbStorageTypes[i][0] == ToUpper(ptr[parms[0].offset])) &&
                    (PbStorageTypes[i][1] == ToUpper(ptr[parms[0].offset+1]))) {
                    /* Found the matching storage type. */
                    Result->storage_read.selected = (1 << i);
                    break;
                }
            }
            if (parms[1].len)
                Result->storage_read.used = (U16)AtAtoI(ptr + parms[1].offset, parms[1].len);

            if (parms[2].len)
                Result->storage_read.total = (U16)AtAtoI(ptr + parms[2].offset, parms[2].len);
        }
        break;

    case AT_READ_PHONEBOOK_ENTRY:
        if (parms[0].len && (ptr[parms[0].offset] == '(')) {
            /* Syntax: +CPBR: (list of supported <index>s),[<nlength>],[<tlength>] */
            *Type |= AT_TEST;
            
            Result->read_test.range = (const char*)ptr + parms[0].offset + 1;

            for (p = 0; p < numParms; p++) {
                if (ptr[parms[p].offset + parms[p].len - 1] == ')') {
                    /* End of (list of supported <index>s) */
                    ptr[parms[p].offset + parms[p].len - 1] = '\0';
                    break;
                }
            }

            if (parms[++p].len > 0)
                Result->read_test.numLength = (U16)AtAtoI(ptr + parms[p].offset, parms[p].len);

            if (parms[++p].len > 0)
                Result->read_test.textLength = (U16)AtAtoI(ptr + parms[p].offset, parms[p].len);

            break;
        }
        /* Drop through for normal read phonebook result decode */

    case AT_FIND_PHONEBOOK_ENTRY:
        /* Syntax: [+CPBF: <index1>,<number>,<type>,<text>[[...] */
        /* Syntax: [+CPBR: <index1>,<number>,<type>,<text>[[...] */
        if (parms[0].len > 0)
            Result->read.index = (U16)AtAtoI(ptr + parms[0].offset, parms[0].len);

        Result->read.number = AtMakeString(ptr + parms[1].offset, parms[1].len);

        if (parms[2].len > 0)
            Result->read.type = (U8)AtAtoI(ptr + parms[2].offset, parms[2].len);

        Result->read.text = AtMakeString(ptr + parms[3].offset, parms[3].len);
        break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
#endif /* AT_ROLE_TERMINAL == XA_ENABLED */


#if AT_ROLE_MOBILE == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * AT_Encode_Phonebook
 * 
 *     Encodes Audio Gateway (ME) originated AT results.
 */ 
AtStatus ME_Encode_Phonebook(AtContext *Atc, U16 Type, const AtPhonebookResult *Result, XaBufferDesc *Output)
{
    U16     avail = (Output->buffSize - Output->writeOffset) - 2;
    I8      i;
    I16     len, nLen;

    switch (Type) {
    case AT_SELECT_PHONEBOOK_STORAGE|AT_READ:
        /* Syntax: +CPBS: <storage>[,<used>,<total>] */
        if (avail < 16) {
            return AT_STATUS_NO_RESOURCES;
        }
        for (i = 0; PbStorageTypes[i]; i++) {
            if (Result->storage_read.selected == (1 << i)) {
                break;
            }
        }
        if (PbStorageTypes[i] == 0) {
            return AT_STATUS_INVALID_PARM;
        }

        Output->buff[Output->writeOffset++] = '\"';
        Output->buff[Output->writeOffset++] = PbStorageTypes[i][0];
        Output->buff[Output->writeOffset++] = PbStorageTypes[i][1];
        Output->buff[Output->writeOffset++] = '\"';
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->storage_read.used);
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->storage_read.total);
        break;

    case AT_SELECT_PHONEBOOK_STORAGE|AT_TEST:
        /* Syntax: +CPBS: (list of supported <storage>s) */
        if (avail < ((5 * 8) + 2)) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->buff[Output->writeOffset++] = '(';

        for (i = 0; PbStorageTypes[i]; i++) {
            if (Result->storage_test.supported & (1 << i)) {
                Output->buff[Output->writeOffset++] = '\"';
                Output->buff[Output->writeOffset++] = PbStorageTypes[i][0];
                Output->buff[Output->writeOffset++] = PbStorageTypes[i][1];
                Output->buff[Output->writeOffset++] = '\"';
                Output->buff[Output->writeOffset++] = ',';
            }
        }
        Output->buff[Output->writeOffset-1] = ')'; /* Overwrite trailing ',' */
        break;

    case AT_READ_PHONEBOOK_ENTRY|AT_TEST:
        /* Syntax: +CPBR: (list of supported <index>s),[<nlength>],[<tlength>] */
        len = OS_StrLen(Result->read_test.range);
        if (avail < (len + 14)) {
            return AT_STATUS_NO_RESOURCES;
        }

        //Output->buff[Output->writeOffset++] = '(';
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->read_test.range), len);
        Output->writeOffset += len;
        //Output->buff[Output->writeOffset++] = ')';

        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->read_test.numLength);
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->read_test.textLength);
        break;

    case AT_FIND_PHONEBOOK_ENTRY|AT_TEST:
        /* Syntax: +CPBF: [<nlength>],[<tlength>] */
        if (avail < 14) {
            return AT_STATUS_NO_RESOURCES;
        }

        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->find_test.numLength);
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->find_test.textLength);
        break;
		
    case AT_READ_PHONEBOOK_ENTRY:
        /* Syntax: [+CPBR: <index1>,<number>,<type>,<text>[[...] */
    case AT_FIND_PHONEBOOK_ENTRY:
        /* Syntax: [+CPBF: <index1>,<number>,<type>,<text>[[...] */
        len = OS_StrLen(Result->read.text);
        nLen = OS_StrLen(Result->read.number);

        if (avail < (11 + nLen + len)) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->read.index);
        Output->buff[Output->writeOffset++] = ',';

	 Output->buff[Output->writeOffset++] = '\"';
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->read.number), nLen);
        Output->writeOffset += nLen;
	 Output->buff[Output->writeOffset++] = '\"';
        Output->buff[Output->writeOffset++] = ',';

        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->read.type);
        Output->buff[Output->writeOffset++] = ',';

	Output->buff[Output->writeOffset++] = '\"';
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8*)(Result->read.text), len);
        Output->writeOffset += len;
	Output->buff[Output->writeOffset++] = '\"';
        break;

    case AT_WRITE_PHONEBOOK_ENTRY|AT_TEST:
        /* Syntax: +CPBW: (list of supported <index>s),[<nlength>],(list of supported <type>s),[<tlength>] */
        len = OS_StrLen(Result->write_test.range) + OS_StrLen(Result->write_test.types);
        if (avail < (len + 14)) {
            return AT_STATUS_NO_RESOURCES;
        }

		len = OS_StrLen(Result->write_test.range);
        /* Output->buff[Output->writeOffset++] = '('; */
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->write_test.range), len);
        Output->writeOffset += len;
        /* Output->buff[Output->writeOffset++] = ')'; */

        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->write_test.numLength);

        Output->buff[Output->writeOffset++] = ',';
		len = OS_StrLen(Result->write_test.types);
        /* Output->buff[Output->writeOffset++] = '('; */
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->write_test.types), len);
        Output->writeOffset += len;
        /* Output->buff[Output->writeOffset++] = ')'; */
		
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset,
                                      Result->write_test.textLength);
		break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
        
/*--------------------------------------------------------------------------- 
 * AT_Decode_Phonebook
 * 
 *     Decodes AT commands received by the Gateway (ME).
 */
AtStatus ME_Decode_Phonebook(AtContext *Atc, U16 *Type, AtPhonebookCmd *Command, XaBufferDesc *Input)
{
    U8             *ptr = Input->buff + Input->readOffset;
    AtParameter     parms[AT_MAX_PARMS];
    I8              i;

    AtParseParameters(ptr, (U16)(Input->writeOffset - Input->readOffset), parms);

    switch (*Type) {
    case AT_SELECT_PHONEBOOK_STORAGE:
        /* AT+CPBS=<storage> */
        if (parms[0].len > 1) {

            if (ptr[parms[0].offset] == '\"' && (parms[0].len > 2))
                parms[0].offset++;  /* Strip quote from storage type */

            for (i = 0; PbStorageTypes[i]; i++) {
                if ((PbStorageTypes[i][0] == ToUpper(ptr[parms[0].offset])) &&
                    (PbStorageTypes[i][1] == ToUpper(ptr[parms[0].offset+1]))) {
                    /* Found the matching storage type. */
                    Command->storage.select = (1 << i);
                    break;
                }
            }
		if(PbStorageTypes[i] == 0)
			return AT_STATUS_INVALID_PARM;
        }
        break;

    case AT_READ_PHONEBOOK_ENTRY:
        /* Syntax: AT+CPBR=<index1>[,<index2>] */
        if (parms[0].len > 0)
            Command->read.first = (U16)AtAtoI(ptr + parms[0].offset, parms[0].len);
	 else
		return AT_STATUS_INVALID_PARM;
        if (parms[1].len > 0)
            Command->read.last = (U16)AtAtoI(ptr + parms[1].offset, parms[1].len);
        else Command->read.last = Command->read.first;
        break;

    case AT_FIND_PHONEBOOK_ENTRY:
        /* Syntax: AT+CPBF=<findtext> */
	 if (parms[0].len > 0)
        Command->find.text = AtMakeString(ptr + parms[0].offset, parms[0].len);
	 else
	 	return AT_STATUS_INVALID_PARM;
        break;

    case AT_WRITE_PHONEBOOK_ENTRY:
        /* Syntax: AT+CPBW=[<index>][,<number>[,<type>[,<text>]]] */
        if (parms[0].len > 0)
            Command->write.index = (U16)AtAtoI(ptr + parms[0].offset, parms[0].len);
	 if (parms[1].len > 0)
        Command->write.number = AtMakeString(ptr + parms[1].offset, parms[1].len);
	 else
	 	Command->write.number = NULL;

        if (parms[2].len > 0)
            Command->write.type = (U8)AtAtoI(ptr + parms[2].offset, parms[2].len);

	 if (parms[1].len > 0)
        Command->write.text = AtMakeString(ptr + parms[3].offset, parms[3].len);
	 else
	 	Command->write.text = NULL;
        break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
#endif /* AT_ROLE_MOBILE == XA_ENABLED */

#endif /* AT_PHONEBOOK == XA_ENABLED */

