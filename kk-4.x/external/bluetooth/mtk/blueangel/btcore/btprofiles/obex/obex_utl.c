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
 * File:        obex_utl.c
 *
 * Description: This file contains OBEX utility functions, such as the
 *              MD5 algorithm used for OBEX Authentication and Session
 *              support.
 *
 * Created:     October 10, 2001
 *
 * Version:     MTObex 3.4
 *
 * Copyright 2000-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
 * Jul 15 2008 mbj06032
 * [MAUI_01178462] MTK:Bluetooth FTP_handset always show "cancelling" screen
 * start Tx timer after call RF_SendData; when timeout, disconnect and return the tx buffer
 *
 * Jul 7 2008 mbj06032
 * [MAUI_01084085] [BT]Should not popup "Print done."
 * don't start timer for BPP GET request
 *
 * Jun 17 2008 mbj06032
 * [MAUI_01077981] [BT]ASSERT Fail: observer.c 1085 -BT
 * 
 *
 * May 26 2008 mbj06032
 * [MAUI_01047668] [BT BIP] code revise
 * 
 *
 * May 23 2008 mbj06032
 * [MAUI_01046086] [BT OBEX] modify OBEX timer: for RFCOMM packet
 * 
 *
 * May 23 2008 mbj06032
 * [MAUI_01046086] [BT OBEX] modify OBEX timer: for RFCOMM packet
 * 
 *
 * May 13 2008 mbj06032
 * [MAUI_00093122] Screen always displays recieving X%, x/x.
 * start timer when sending out a OBEX command
 *
 * Jan 9 2008 mbj06032
 * [MAUI_00515012] [BT][BIP, GOEP, OBEX] add "$Log$" in file header
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include <obex.h>
#include <sys/obex_utl.h>
#include <osapi.h>

#include "bt.h"

#include "goep.h"
#include "obex_utl.h"


#include "bluetooth_trc.h"

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

/* Strings used with debug output messages. */
//#if XA_DEBUG == XA_ENABLED
const char *ObParserEventStr[OBPE_BUILD_RESPONSE+1] = {
                "NO_EVENT","PUT_HEADER","WRITE_HEADER","OPER_IND_START",
                "OPER_IND","DISCONNECT","OPER_COMPLETE","BUILD_RESPONSE" };
//#endif /* XA_DEBUG == XA_ENABLED */

#if OBEX_SESSION_SUPPORT == XA_ENABLED

/*---------------------------------------------------------------------------
 *            OBEX_CreateSessionId()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Create a session Id based on the device addresses and nonces 
 *            for the client and server.  This newly created session Id is
 *            then passed back in the "dest" parameter.
 *
 * Return:    OB_STATUS_SUCCESS - SessionId was created successfully.
 *            OB_STATUS_INVALID_PARM - Invalid parameter
 */
ObStatus OBEX_CreateSessionId(void *Session, U8 Dest[16]) 
{
    xMD5Context         context;
    ObexClientSession   *sess;
    
    /* NOTE: The session could be a client or server session, but the 
     *       fields used in this function are guaranteed to be in the
     *       same order.  Therfore, we can always cast to an 
     *       ObexClientSession.
     */
    sess = (ObexClientSession *)Session;
    
#if XA_ERROR_CHECK == XA_ENABLED
    if (!sess || !Dest || (sess->clientNonceLen > 16) || (sess->serverNonceLen > 16)) {
        return OB_STATUS_INVALID_PARM;
    }

#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(sess && Dest);

    /* Concatenate - client dev addr, client nonce, server dev addr,
     *               and server nonce.
     */

    xMD5Init(&context);
    /* Client Device Address: Length depends on transport used */
    xMD5Update(&context, sess->clientDevAddr, (U16)GetDevAddrLen(sess->tpType));
    /* Client Nonce: 4-16 byte nonces are valid */
    xMD5Update(&context, sess->clientNonce, sess->clientNonceLen);
    /* Server Device Address: Length depends on transport used */
    xMD5Update(&context, sess->serverDevAddr, (U16)GetDevAddrLen(sess->tpType));
    /* Server Nonce: 4-16 byte nonces are valid */
    xMD5Update(&context, sess->serverNonce, sess->serverNonceLen);
    xMD5Final(Dest, &context);

    return OB_STATUS_SUCCESS;
}
 
/* Internal utility functions for managing OBEX Sessions including:
 * - verifying header components
 * - creating Nonces and Session IDs
 * - managing suspend timers
 * - finding sessions
 */

/*
 * ObCreateNonce
 *
 * Calculate the Nonce based on the system time and a random number
 * combined with the MD5 algorithm to give it 16 bytes.
 */
void ObCreateNonce(U8 *Dest) {
    U32     val;

    val = OS_GetSystemTime() + OS_Rand();
    
    /* Copy the Nonce to the destination Session Context */
    MD5(Dest, &val, 4);
}

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*
 * ProcessResumeSequenceNum
 *
 * Evaluate the Session sequence number supplied in the OBEX Session Resume
 * response, and determine if any retransmission is required.
 *
 * Return: TRUE if the sequence number was acceptable.
 *         FALSE if the session cannot be resumed.
 */
BOOL ProcessResumeSequenceNum(ObexClientApp *ObexApp, U8 SeqNum) 
{
    ObSessionError  seqState = ERR_DISCONNECT;

    /* Validate the next sequence number received from the server
     * during a Session Resume operation.
     */

    if (SeqNum == OCC(activeSession)->seqNum) {
        if (SeqNum == OCC(activeSession)->nextSeqNum) {
            /* Everything is normal.  We have no outstanding
             * packets, since our current and next sequence
             * numbers are identical. No retransmitting needed
             */
            seqState = ERR_NONE;
        } 
        else if ((U8)(OCC(activeSession)->seqNum+1) == (OCC(activeSession)->nextSeqNum))
            seqState = ERR_RETRANSMIT;
    }
    else if (SeqNum == OCC(activeSession)->nextSeqNum) {
        if ((U8)(OCC(activeSession)->seqNum+1) == (OCC(activeSession)->nextSeqNum))
            seqState = ERR_RETRANSMIT_NO_HEADERS;
    }

    OCC(seqNumError) = seqState;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_RESUME_SEQUENCE_STATUSxD_EXPECT_SEQNUMxD_NEXTSEQNUMxD_GOT_SEQNUMxD , 
           OCC(seqNumError), OCS(seqNum), OCS(nextSeqNum), SeqNum);

    return (seqState != ERR_DISCONNECT);
}

/*
 * VerifyClientSequenceNum
 *
 * Evaluate the Session sequence number supplied in an OBEX response packet
 * during an active session. The sequence number MUST be in sequence.
 */
ObSessionError VerifyClientSequenceNum(ObexClientApp *ObexApp, U8 SeqNum) 
{
    /* Validate the next sequence number received from the server -
     * for normal sequence number header exchanges during an active session.
     */
    if ((SeqNum == OCS(nextSeqNum)) &&
        ((U8)(OCS(seqNum)+1) == OCS(nextSeqNum))) {
        return ERR_NONE;
    }

    return ERR_DISCONNECT;
}
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */


/*
 * ObexParseSessionParms
 *
 * Parses session parms as they arrive
 */
BOOL ObexParseSessionParms(ObexAppHandle *AppHndl, 
                            ObexSessionParms   *sp)
{
    ObexTlv tlv;

    /* For each TLV found... */
    while (OBEXH_ParseTlv(AppHndl, &tlv))
    {
        /* All Session parms are 16 bytes or less. Make sure then that we receive
         * them contiguously.
         */
        ASSERT((tlv.length > 16) || (tlv.length == tlv.valueLen));

        /* For each tag, validate length and copy data into sp */
        switch(tlv.tag)
        {
        case SESSION_TAG_DEV_ADDR:
            /* 4 byte (TCP and IRDA) and 6 byte (Bluetooth) device addresses are valid */
            if ((tlv.length != 4) && (tlv.length != 6)) continue;

            OS_MemCopy(sp->device, tlv.value, tlv.valueLen);
            sp->deviceLen = tlv.valueLen;
            break;

        case SESSION_TAG_NONCE:
            /* 4-16 byte nonces are valid */
            if ((tlv.length < 4 ) || (tlv.length > 16)) continue;

            OS_MemCopy(sp->nonce, tlv.value, tlv.valueLen);
            sp->nonceLen = tlv.valueLen;
            break;

        case SESSION_TAG_ID:
            /* 16 byte session ids are valid */
            if (tlv.length != 16) continue;

            OS_MemCopy(sp->sessionId, tlv.value, tlv.valueLen);
            break;

        case SESSION_TAG_SEQ_NUM:
            /* 1 byte sequence numbers are valid */
            if (tlv.length != 1) continue;

            sp->sequenceNum = *tlv.value;
            break;

        case SESSION_TAG_OPCODE:
            /* 1 byte session opcode is valid */
            if (tlv.length != 1) continue;

            sp->opcode = *tlv.value;
            break;

        case SESSION_TAG_TIMEOUT:
            /* 4 byte timeouts are valid */
            if (tlv.length != 4) continue;

            /* Convert from BE to Host. If the Timeout = Don't Care, ignore it. */
            if ((sp->timeout = BEtoHost32(tlv.value)) == 0) continue;
            break;

        default:
            continue;
        }

        /* Keep track of which parameters we've received. */
        sp->fields |= (1 << tlv.tag);
    }
    return TRUE;
}


#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

#if OBEX_SESSION_SUPPORT == XA_ENABLED || OBEX_AUTHENTICATION == XA_ENABLED

/*---------------------------------------------------------------------------
 *                      MD5 Message Digest Code
 *---------------------------------------------------------------------------
 *
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 */

/*
 * Shuffle the bytes into little-endian order within words, as per the
 * MD5 spec.  Note: this code works regardless of the U8 order.
 */
static void byteSwap(U32 *buf, U16 words)
{
    U8 *p = (U8 *)buf;

    do {
        *buf++ = (U32)((unsigned)p[3] << 8 | p[2]) << 16 |
                      ((unsigned)p[1] << 8 | p[0]);
        p += 4;
    } while (--words);
}

/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
void xMD5Init(xMD5Context *ctx)
{
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bytes[0] = 0;
    ctx->bytes[1] = 0;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
void xMD5Update(xMD5Context *ctx, const U8 *buf, U16 len)
{
    U32 t;

    /* Update U8 count */
    t = ctx->bytes[0];
    if ((ctx->bytes[0] = t + len) < t)
        ctx->bytes[1]++;        /* Carry from low to high */

    t = 64 - (t & 0x3f); /* Space avail in ctx->in (at least 1) */
    if ((unsigned)t > len) {
        OS_MemCopy((U8 *)ctx->in + 64 - (unsigned)t, buf, len);
        return;
    }

    /* First chunk is an odd size */
    OS_MemCopy((U8 *)ctx->in + 64 - (unsigned)t, buf, (U16)t);
    byteSwap(ctx->in, 16);
    xMD5Transform(ctx->buf, ctx->in);
    buf += (unsigned)t;
    len -= (unsigned)t;

    /* Process data in 64-byte chunks */
    while (len >= 64) {
        OS_MemCopy((U8 *)ctx->in, buf, 64);
        byteSwap(ctx->in, 16);
        xMD5Transform(ctx->buf, ctx->in);
        buf += 64;
        len -= 64;
    }

    /* Handle any remaining bytes of data. */
    OS_MemCopy((U8 *)ctx->in, buf, len);
}

/*
 * Final wrapup - pad to 64-U8 boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
void xMD5Final(U8 digest[AUTH_NONCE_LEN], xMD5Context *ctx)
{
    S16 count = (S16)(ctx->bytes[0] & 0x3f); /* Bytes in ctx->in */
    U8 *p = (U8 *)ctx->in + count;           /* First unused byte */

    /* Set the first char of padding to 0x80. There is always room.*/
    *p++ = 0x80;

    /* Bytes of padding needed to make 56 bytes (-8..55) */
    count = 56 - 1 - count;

    if (count < 0) {        /* Padding forces an extra block */
        OS_MemSet(p, 0, count+8);
        byteSwap(ctx->in, 16);
        xMD5Transform(ctx->buf, ctx->in);
        p = (U8 *)ctx->in;
        count = 56;
    }
    OS_MemSet(p, 0, count+8);
    byteSwap(ctx->in, 14);

    /* Append length in bits and transform */
    ctx->in[14] = ctx->bytes[0] << 3;
    ctx->in[15] = ctx->bytes[1] << 3 | ctx->bytes[0] >> 29;
    xMD5Transform(ctx->buf, ctx->in);

    byteSwap(ctx->buf, 4);
    OS_MemCopy(digest, (U8 *)ctx->buf, 16);
    OS_MemSet((U8 *)&ctx, 0, sizeof(ctx));
}


/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f,w,x,y,z,in,s) \
         (w += f(x,y,z) + in, w = (w<<s | w>>(32-s)) + x)

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
void xMD5Transform(U32 buf[4], U32 const in[AUTH_NONCE_LEN])
{
    U32 a, b, c, d;

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);
    
    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);
    
    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);
        
    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}


void MD5(void *dest, void *orig, U16 len)
{
    xMD5Context context;

    xMD5Init(&context);
    xMD5Update(&context, orig, len);
    xMD5Final(dest, &context);
}

#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED || OBEX_AUTHENTICATION == XA_ENABLED */


static kal_uint32 obex_time_msec_2_tick(kal_uint32 msec)
{
   kal_uint32 num_ticks; 

   num_ticks = (msec*1000)/4615;

   while(num_ticks*4615 < (msec*1000))
      num_ticks++;

   return num_ticks;
}

void obex_start_timer(EvmTimer *timer_id_p, kal_uint32 timer_duration, kal_timer_func_ptr funct_ptr, void* hf_param)
{
    //bt_prompt_trace(MOD_BT, "[BT] obex_start_timer");
#if 0 //TODO
	/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    eventid timer_evt_id = *timer_id_p;

    if(bt_p->os_init == TRUE)
    {
        if (timer_evt_id != 0)
        {
            evshed_cancel_event (bt_p->evsh_sched, timer_id_p);
            *timer_id_p = 0;
        }

        *timer_id_p = evshed_set_event (bt_p->evsh_sched,
    		                            funct_ptr, 
    		                            hf_param, 
    		                            obex_time_msec_2_tick(timer_duration));
        //Report(("[OBEX TIMER] obex_start_timer: evshed_set_event timer_id=%d", *timer_id_p));
    }
#else
	EvmTimer *p_timer;
    
	p_timer = (EvmTimer *) timer_id_p;
	p_timer->context = hf_param;
        // bt_prompt_trace(MOD_BT, "[BT] obex_start_timer");
	p_timer->func =  (EvmTimerNotify) funct_ptr;
	EVM_StartTimer(p_timer, (timer_duration) );
#endif
	
}

void obex_stop_timer(EvmTimer *timer_id_p)
{
        // bt_prompt_trace(MOD_BT, "[BT] obex_stop_timer");
	EVM_CancelTimer((EvmTimer *)timer_id_p);
}

#if OBEX_SUPPORT_TIMEOUT == XA_ENABLED

static void OBEX_ClientRequestTimeout(void *hf_param)
{
    ObexAppHandle *appHndl = (ObexAppHandle *) ((EvmTimer *)hf_param)->context; /// BT-Task changs to use EVM_TIMER 
    ObexClientApp *ObexApp = (ObexClientApp*)appHndl;
	ObStatus status;

	ASSERT(appHndl != NULL);
    kal_trace(BT_TRACE_G6_OBEX, OBEX_CLIENT_REQTIMEOUT, appHndl, ObexApp->clnt_req_timer_id,appHndl->tpStatus);
	
//	if (ObexApp->clnt_req_timer_id == 0)
	if( 0 == hf_param)
	{
	    Report( ("[OBEX][TIMER][WRN] OBEX_ClientRequestTimeout the hf_param is NULL !") );
	    return;
	}

    /* clear the timer id */
    // ObexApp->clnt_req_timer_id = 0;

	/* client is already DISCONNECTING, do nothing */
	if (appHndl->tpStatus == OB_TP_DISCONNECTING || appHndl->tpStatus == OB_TP_DISCONNECTED)
	{
	    kal_trace(BT_TRACE_G6_OBEX, OBEX_CLIENT_DISCONNECTING_ED,appHndl->tpStatus);
	    return;
	}

    /* call OBEX_ClientTpDisconnect to disconnect the transport layer */
	status = OBEX_ClientTpDisconnect(ObexApp, TRUE);

    ObexApp->req_timeout = TRUE;

	kal_trace(BT_TRACE_G6_OBEX, OBEX_CLIENTTPDISCONNECT_RETURN, status);
}


void OBEX_ClientStartReqTimer(ObexAppHandle *AppHndl, ObexOpcode cmd)
{
	kal_uint32 timer_dur = 0;
    ObexClientApp *ObexApp = (ObexClientApp*)AppHndl;
	ObexOpcode cmd_with_no_fb = cmd & ~OB_FINAL_BIT;

    ObexApp->req_timeout = FALSE;
		
    switch (cmd_with_no_fb)
    {
        case OB_OPCODE_CONNECT:
			timer_dur = OBEX_CONNECT_REQ_TIMER_DUR;
			break;

        case OB_OPCODE_ABORT:
			timer_dur = OBEX_ABORT_REQ_TIMER_DUR;
			break;

        case OB_OPCODE_DISCONNECT:
			timer_dur = OBEX_DISCONNECT_REQ_TIMER_DUR;
			break;

        case OB_OPCODE_GET:
            /* don't start timer for BPP client */
            if (ObexApp->goep_service_type == GOEP_PROFILE_BPP)
            {
                return;
            }
			timer_dur = OBEX_GET_REQ_TIMER_DUR;
			break;

        case OB_OPCODE_PUT:
        case OB_OPCODE_SET_PATH:
			timer_dur = OBEX_GENERAL_REQ_TIMER_DUR;
			break;

        default:
			timer_dur = 0;
			break;
    }


    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_START_TIMER,cmd,AppHndl,timer_dur);    

	if (timer_dur > 0)
	{
	    obex_start_timer(&(ObexApp->clnt_req_timer_id), 
		             timer_dur,
		             OBEX_ClientRequestTimeout,
		             AppHndl);
	}
}

void OBEX_ClientStopReqTimer(ObexAppHandle *AppHndl)
{
    ObexClientApp *ObexApp = (ObexClientApp*)AppHndl;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_STOP_TIMER, AppHndl,ObexApp->clnt_req_timer_id);

	obex_stop_timer(&(ObexApp->clnt_req_timer_id));

	// ObexApp->clnt_req_timer_id = 0; // MAUI w0946
}


void OBEX_ClientRestartReqTimer(ObexAppHandle *AppHndl)
{
    ObexClientApp *ObexApp = (ObexClientApp*)AppHndl;

    kal_trace(BT_TRACE_G6_OBEX,OBEX_CLIENT_RESTART_TIMER, AppHndl,ObexApp->clnt_req_timer_id);

    /* first stop the timer */
	obex_stop_timer(&(ObexApp->clnt_req_timer_id));

	// ObexApp->clnt_req_timer_id = 0; // MAUI w0946

    /* then start the timer witch OBEX_RFCOMM_PACKET_TIMER_DUR */
    obex_start_timer(&(ObexApp->clnt_req_timer_id), 
		             OBEX_RFCOMM_PACKET_TIMER_DUR,
		             OBEX_ClientRequestTimeout,
		             AppHndl);    
}

#endif

