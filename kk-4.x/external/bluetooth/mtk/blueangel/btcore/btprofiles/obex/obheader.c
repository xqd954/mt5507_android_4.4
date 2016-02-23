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
 * File:        obheader.c
 *
 * Description: This file contains the code for building and parsing OBEX
 *              headers.
 *
 * Created:     July 27, 1998
 *
 * Version:     MTObex 3.4
 *
 * Copyright 1998-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems,
 * Inc.  This work contains confidential and proprietary information of
 * Extended Systems, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * Jan 9 2008 mbj06032
 * [MAUI_00515012] [BT][BIP, GOEP, OBEX] add "$Log$" in file header
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#include <sys/obxalloc.h>

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

#if (XA_ERROR_CHECK == XA_ENABLED) /*|| (XA_DEBUG == XA_ENABLED)*/
extern BOOL ObIsHeaderSpaceAvail(ObexAppHandle *AppHndl, U16 HeaderLen);
#endif /* (XA_ERROR_CHECK == XA_ENABLED) || (XA_DEBUG == XA_ENABLED) */

#if OBEX_HEADER_BUILD == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBEXH_BuildByteSeq()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Builds Byte Sequence and UNICODE style OBEX headers. Used by 
 *            both client and server functions.
 *
 * Return:    TRUE  if headers could be added.
 *            FALSE if headers would exceed limits on buffer or transmit space.
 */
BOOL OBEXH_BuildByteSeq(ObexAppHandle *AppHndl, ObexHeaderType Type, const U8 *Value, U16 Len)
{
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl || !ObIsHeaderSpaceAvail(AppHndl, (U16)(Len+3)) ||
        ((Type & 0xC0) != 0x40 && (Type & 0xC0) != 0)) {
        OS_UnlockObex();
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(AppHndl && AppHndl->buffer != 0);
    ASSERT(ObIsHeaderSpaceAvail(AppHndl, (U16)(Len+3)) &&
           ((Type & 0xC0) == 0x40 || (Type & 0xC0) == 0));
    
    if (Type == OBEXH_END_BODY) {
        AppHndl->appHeaderIsEndBody = TRUE;
    }

    AppHndl->buffer[AppHndl->txLength++] = Type;  
    AppHndl->buffer[AppHndl->txLength++] = (U8)((Len+3) >> 8);
    AppHndl->buffer[AppHndl->txLength++] = (U8) (Len+3);
    if (Len > 0) {
        ASSERT( Value != 0 );
        OS_MemCopy(AppHndl->buffer+AppHndl->txLength, Value, Len);
        AppHndl->txLength += Len;
    }

    OS_UnlockObex();
    return TRUE;
}

/*---------------------------------------------------------------------------
 *            OBEXH_BuildUnicode()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Builds UNICODE style OBEX headers. Used by both client and 
 *            server functions.
 *
 * Return:    TRUE  if headers could be added.
 *            FALSE if headers would exceed limits on buffer or transmit space.
 */
BOOL OBEXH_BuildUnicode(ObexAppHandle *AppHndl, ObexHeaderType Type, const U8 *Value, U16 Len)
{
#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl || (Len % 2) != 0) {
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */    
    ASSERT(AppHndl && (Len % 2) == 0);
    
    return OBEXH_BuildByteSeq( AppHndl, Type, Value, Len );
}

/*---------------------------------------------------------------------------
 *            OBEXH_Build4Byte()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Builds 4-byte style OBEX headers. Used by both client and
 *            server functions.
 *
 * Return:    TRUE  if headers could be added.
 *            FALSE if headers would exceed limits on buffer or transmit space.
 */
BOOL OBEXH_Build4Byte(ObexAppHandle *AppHndl, ObexHeaderType Type, U32 Value)
{
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl || !ObIsHeaderSpaceAvail(AppHndl, 5) || ((Type & 0xC0) != 0xC0)) {
        OS_UnlockObex();
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(AppHndl && AppHndl->buffer != 0);
    ASSERT(ObIsHeaderSpaceAvail(AppHndl, 5) && (Type & 0xC0) == 0xC0);
    
    AppHndl->buffer[AppHndl->txLength++] = Type;
    StoreBE32( AppHndl->buffer+AppHndl->txLength, Value);
    AppHndl->txLength += 4;

    OS_UnlockObex();
    return TRUE;
}

/*---------------------------------------------------------------------------
 *            OBEXH_Build1Byte()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Builds 1-byte style OBEX headers. Used by both client and 
 *            server functions.
 *
 * Return:    TRUE  if headers could be added.
 *            FALSE if headers would exceed limits on buffer or transmit space.
 */
BOOL OBEXH_Build1Byte(ObexAppHandle *AppHndl, ObexHeaderType Type, U8 Value)
{
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl || !ObIsHeaderSpaceAvail(AppHndl, 2) || (Type & 0xC0) != 0x80) {
        OS_UnlockObex();
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(AppHndl && AppHndl->buffer != 0);
    ASSERT(ObIsHeaderSpaceAvail(AppHndl, 2) && (Type & 0xC0) == 0x80);
    
    AppHndl->buffer[AppHndl->txLength++] = Type;
    AppHndl->buffer[AppHndl->txLength++] = Value;

    OS_UnlockObex();
    return TRUE;
}


/*---------------------------------------------------------------------------
 *            OBEXH_BuildEmptyEndBody()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Builds empty OBEX headers. Used by both client and 
 *            server functions.
 *
 * Return:    TRUE  if headers could be added.
 *            FALSE if headers would exceed limits on buffer or transmit space.
 */
BOOL OBEXH_BuildEmptyEndBody(ObexAppHandle *AppHndl)
{
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl || !ObIsHeaderSpaceAvail(AppHndl, 1)) {
        OS_UnlockObex();
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    ASSERT(AppHndl && AppHndl->buffer != 0);
    ASSERT(ObIsHeaderSpaceAvail(AppHndl, 1));
    
    AppHndl->buffer[AppHndl->txLength++] = OBEXH_END_BODY;
    AppHndl->buffer[AppHndl->txLength++] = 0x00;
    AppHndl->buffer[AppHndl->txLength++] = 0x03;

    OS_UnlockObex();
    return TRUE;
}
/*---------------------------------------------------------------------------
 *            OBEXH_FlushHeaders()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function is used to flush any constructed headers from
 *            the header buffer. This function is used when OBEXH_Build...()
 *            functions are used and the operation is cancelled before the
 *            headers are sent or if the requested Operation fails to start.
 *
 * Return:    void
 */
void OBEXH_FlushHeaders(ObexAppHandle *AppHndl)
{
    ASSERT(AppHndl);

    OS_LockObex();
    AppHndl->txLength = 0;
    OS_UnlockObex();
}

/*---------------------------------------------------------------------------
 *            OBEXH_GetAvailableTxHeaderLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the amount of header space available for adding more
 *            headers. This value is constrained by the OBEX Packet size,
 *            the application provided header buffer size and any headers
 *            which are already present in the buffer.
 *
 * Return:    Available header buffer length in bytes.
 */
U16 OBEXH_GetAvailableTxHeaderLen(ObexAppHandle *AppHndl)
{
#if OBEX_SERVER_CONS_SIZE > 0 || OBEX_SESSION_SUPPORT == XA_ENABLED
    void        *ObexApp = AppHndl;
#endif /* OBEX_SERVER_CONS_SIZE > 0 || OBEX_SESSION_SUPPORT == XA_ENABLED */
    U16            avail;

    ASSERT(AppHndl);

    OS_LockObex();
    /* Just assume Connect to get a size that will always work. */
    avail = ObParserMaxHbSize(&AppHndl->parser, OB_OPCODE_CONNECT);

#if (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED)
    if (IsServerParser(&AppHndl->parser)) {
#if OBEX_ROLE_SERVER == XA_ENABLED
        avail -= OSC(protoTxLen);
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */
    }
#if OBEX_SESSION_SUPPORT == XA_ENABLED
    else {
        ASSERT(IsClientParser(&AppHndl->parser));
#if OBEX_ROLE_CLIENT == XA_ENABLED
        avail -= OCC(protoTxLen);
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */
    }
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
#endif /* (OBEX_SERVER_CONS_SIZE > 0) || (OBEX_SESSION_SUPPORT == XA_ENABLED) */

    avail = min(avail, AppHndl->length);
    avail -= AppHndl->txLength;

    OS_UnlockObex();

    return avail;
}

#endif /* OBEX_HEADER_BUILD == XA_ENABLED */


#if (OBEX_HEADER_PARSE == XA_ENABLED) && (OBEX_USE_HEADER_MACROS == XA_DISABLED)
/*---------------------------------------------------------------------------
 *            Header Processing Functions
 *---------------------------------------------------------------------------
 *
 * Synopsis:  The following functions are used by a client or server
 *            application to retrieve OBEX header information from the parser.
 */

/*---------------------------------------------------------------------------
 *            OBEXH_GetHeaderType()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the type of the current header.
 *
 * Return:    ObexHeaderType
 */
ObexHeaderType OBEXH_GetHeaderType(ObexAppHandle *AppHndl)
{
    ObexParser     *oparser;

    ASSERT(AppHndl && IsObexLocked());

    oparser = &AppHndl->parser;
    ASSERT(oparser->rxState == OBSC_RX_PUT_HEAD3 ||
           oparser->rxState == OBSC_RX_PUT_HEAD2);

    return oparser->header;
}

/*---------------------------------------------------------------------------
 *            OBEXH_GetTotalHeaderLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the total length of the current header.
 *            For use only with Byte Sequence and Unicode headers.
 *
 * Return:    Total header length in bytes
 */
U16 OBEXH_GetTotalHeaderLen(ObexAppHandle *AppHndl)
{
    ObexParser     *oparser;

    ASSERT(AppHndl && IsObexLocked());

    oparser = &AppHndl->parser;
    ASSERT(oparser->rxState == OBSC_RX_PUT_HEAD3 ||
           oparser->rxState == OBSC_RX_PUT_HEAD2);

    return oparser->totalHeaderLen;
}

/*---------------------------------------------------------------------------
 *            OBEXH_GetHeaderAvailLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the currently available (to copy) header length.
 *            For use only with Byte Sequence and Unicode headers.
 *            
 * Return:    Available header length in bytes
 */
U16 OBEXH_GetHeaderAvailLen(ObexAppHandle *AppHndl)
{
    ObexParser  *oparser;

    ASSERT(AppHndl && IsObexLocked());

    oparser = &AppHndl->parser;
    ASSERT(oparser->rxState == OBSC_RX_PUT_HEAD3 ||
           oparser->rxState == OBSC_RX_PUT_HEAD2);

    return oparser->dataLen;
}

/*---------------------------------------------------------------------------
 *            OBEXH_GetHeaderBuff()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns a pointer to the current headers buffer. For use
 *            only with Byte Sequence and Unicode headers.
 *
 * Return:    Buffer pointer
 */
U8 *OBEXH_GetHeaderBuff(ObexAppHandle *AppHndl)
{
    ObexParser  *oparser;

    ASSERT(AppHndl && IsObexLocked());

    oparser = &AppHndl->parser;
    ASSERT( oparser->rxState == OBSC_RX_PUT_HEAD3 ||
            oparser->rxState == OBSC_RX_PUT_HEAD2 );

    return oparser->rxBuff;
}

/*---------------------------------------------------------------------------
 *            OBEXH_GetHeader4Byte()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the value of a 4 byte style header.
 *
 * Return:    Value of header
 */
U32 OBEXH_GetHeader4Byte(ObexAppHandle *AppHndl)
{
    ObexParser  *oparser;

    ASSERT(AppHndl && IsObexLocked());

    oparser = &AppHndl->parser;
    ASSERT(oparser->rxState == OBSC_RX_PUT_HEAD2);

    return BEtoHost32(oparser->stageBuff);
}

/*---------------------------------------------------------------------------
 *            OBEXH_GetHeader1Byte()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the value of a 1 byte style header.
 *
 * Return:    Value of header 
 */
U8 OBEXH_GetHeader1Byte(ObexAppHandle *AppHndl)
{
    ObexParser   *oparser;

    ASSERT(AppHndl && IsObexLocked());

    oparser = &AppHndl->parser;
    ASSERT(oparser->rxState == OBSC_RX_PUT_HEAD2);

    return oparser->stageBuff[0];
}
#endif /* (OBEX_HEADER_PARSE == XA_ENABLED) && (OBEX_USE_HEADER_MACROS == XA_DISABLED) */


#if OBEX_TLV_PARSER == XA_ENABLED
/*---------------------------------------------------------------------------
 *            OBEXH_ParseTlv
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Sniffs out a TLV from the current header
 *
 * Return:    TRUE  if parsing was successful.
 *            FALSE if an error occurred or no more TLVs are present.
 */
BOOL OBEXH_ParseTlv(ObexAppHandle *AppHndl, ObexTlv *tlv)
{
    U8          *buffer;
    U16          len;
    U8           toUse;
    BOOL success = FALSE;

    CheckUnlockedParm(FALSE, AppHndl && tlv);

    OS_LockObex();

    /* On the first OBxx_HEADER_RX indication there is no data available.
     * Use this event to initialize the parser.
     */
    if (AppHndl->parser.dataLen == 0) {
        /* ASSERT that we are in the correct internal state to parse
         * this header, and that it is a byte sequence header
         */
        ASSERT((AppHndl->parser).rxState == OBSC_RX_PUT_HEAD2);
        ASSERT(((AppHndl->parser).header & 0xC0) == 0x40);

        /* Initialize key fields */
        AppHndl->tlv.state = 0; 
        buffer = 0;
        len = 0;
        goto Done;
    }

    /* Use these locals as aliases for the AppHndl fields */
    buffer = AppHndl->tlv.headerBuff;
    len = AppHndl->tlv.headerLen;

    if (buffer == 0) {
        buffer = AppHndl->parser.rxBuff;
        len = AppHndl->parser.dataLen;
    }

    while (len)
    {
        switch (AppHndl->tlv.state) {
        case 0: /* Buff points to option type */
            AppHndl->tlv.tag = *buffer++;
            AppHndl->tlv.state = 1;
            len--;
            break;

        case 1: /* Buff points to option length */
            AppHndl->tlv.length = *buffer++;
            AppHndl->tlv.state = 2;
            AppHndl->tlv.valuePos = 0;
            len--;

            /* No break; fall through to handle option data */
        case 2:
            /* Update length of option data we currently need. */
            toUse = min(AppHndl->tlv.length - AppHndl->tlv.valuePos, len);

            /* Fill in tlv values in case they are necessary */
            tlv->tag = AppHndl->tlv.tag;
            tlv->length = AppHndl->tlv.length;

            /* Is reassembly both necessary and possible for this TLV? */
#if OBEX_TLV_BUFF_SIZE > 0
            if ((toUse < AppHndl->tlv.length) &&
                (AppHndl->tlv.length < OBEX_TLV_BUFF_SIZE)) {

                /* Append recevied data into our staging buffer */
                OS_MemCopy(AppHndl->tlv.value + AppHndl->tlv.valuePos,
                    buffer, toUse);
                AppHndl->tlv.valuePos += toUse;
                tlv->value = AppHndl->tlv.value;
                tlv->valueLen = AppHndl->tlv.length;
                len -= toUse;
                buffer += toUse;
            
                /* Indicate success ONLY if we have completed staging */
                tlv->remainLen = (tlv->length - AppHndl->tlv.valuePos);
                success = (tlv->remainLen == 0);
            } else {
#endif /* OBEX_TLV_BUFF_SIZE > 0 */
            
                /* If reassembly is impossible or unnecessary, simply copy
                 * tag information */
                tlv->value = buffer;
                tlv->valueLen = toUse;
                AppHndl->tlv.valuePos += toUse;
                len -= toUse;
                buffer += toUse;

                /* Indicate "last" if we are indicating the last chunk of bytes
                 * as counted by valuePos
                 */
                tlv->remainLen = (tlv->length - AppHndl->tlv.valuePos);
                /* Indicate success because staging isn't going to happen */
                success = TRUE;

#if OBEX_TLV_BUFF_SIZE > 0
            }
#endif  /* OBEX_TLV_BUFF_SIZE > 0 */

            if (success)
            {
                /* If we have reached the end of the tlv then reset state for
                 * next time */
                if (tlv->remainLen == 0) {
                    AppHndl->tlv.state = 0;
                }
                goto Done;
            }
            break;
        }
    }

    /* If control gets here we have exhausted the buffer */
    buffer = 0;

Done:
    /* Reset buffer/length from locals and return success */
    AppHndl->tlv.headerBuff = buffer;
    AppHndl->tlv.headerLen = len;

    OS_UnlockObex();
    return success;
}
#endif /* OBEX_TLV_PARSER == XA_ENABLED */


#if (XA_ERROR_CHECK == XA_ENABLED) /*|| (XA_DEBUG == XA_ENABLED)*/
BOOL ObIsHeaderSpaceAvail(ObexAppHandle *AppHndl, U16 HeaderLen)
{
    if (((AppHndl->txLength+HeaderLen) > AppHndl->length) ||
        ((AppHndl->txLength+HeaderLen) >
         ObParserMaxHbSize((&AppHndl->parser),
                           (AppHndl->parser).opcode))) {
        return FALSE;
    }
    return TRUE;
}
#endif /* (XA_ERROR_CHECK == XA_ENABLED) || (XA_DEBUG == XA_ENABLED) */

