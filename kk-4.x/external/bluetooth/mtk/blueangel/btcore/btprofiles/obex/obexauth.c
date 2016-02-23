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
 * File:        obexauth.c
 *
 * Description: This file contains the functions that comprise the 
 *              Authentication Extensions to the OBEX Protocol API.
 *
 * Created:     September 4, 2000
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
 * Jan 9 2008 mbj06032
 * [MAUI_00515012] [BT][BIP, GOEP, OBEX] add "$Log$" in file header
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include <sys/obxalloc.h>
#include <obexauth.h>

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

#if (XA_ERROR_CHECK == XA_ENABLED) /*|| (XA_DEBUG == XA_ENABLED)*/
extern BOOL ObIsHeaderSpaceAvail(ObexAppHandle *AppHndl, U16 HeaderLen);
#endif

#if OBEX_AUTHENTICATION == XA_ENABLED
/****************************************************************************
 *
 * Internal Function Prototypes
 *
 ****************************************************************************/

/*
 * Internal Macros for calculating Challenge & Response Parameter size.
 *
 * Challenge: Realm(+2) + Options(3) + Nonce(16+2)
 * Response : UserId(+2) + Nonce(16+2) + Digest(16+2) 
 */
#define CALC_CHALLENGE_LEN(_C)   (((_C)->realmLen ? (_C)->realmLen+2 : 0) + \
                                  ((_C)->options ? 3 : 0) + 18)
#define CALC_RESPONSE_LEN(_R)    (((_R)->userIdLen ? (_R)->userIdLen+2 : 0) + 18 + 18) 

/*---------------------------------------------------------------------------
 *            OBEXH_BuildAuthChallenge
 *---------------------------------------------------------------------------
 *
 * Synopsis:  OBEX applications use this function to build an Authentication
 *            Challenge header. The header is constructed from the field 
 *            values in the ObexAuthChallenge structure.
 *
 * Return:    TRUE if header was successfully built.
 *            FALSE if headers would exceed limits on buffer or transmit space.
 */
BOOL OBEXH_BuildAuthChallenge(ObexAppHandle *AppHndl, ObexAuthChallenge *Challenge, 
                              U8 *NonceOut)
{
    U8  len;
//#if XA_DEBUG == XA_ENABLED
    U8 *orig;
//#endif /* XA_DEBUG == XA_ENABLED */

    OS_LockObex();
#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl || !Challenge ) {
        OS_UnlockObex();
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(AppHndl && Challenge);

    len = CALC_CHALLENGE_LEN(Challenge);
//#if XA_DEBUG == XA_ENABLED
    orig = AppHndl->buffer + AppHndl->txLength;
//#endif /* XA_DEBUG == XA_ENABLED */

#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObIsHeaderSpaceAvail(AppHndl, (U16)(len+3))) {
        OS_UnlockObex();
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(ObIsHeaderSpaceAvail(AppHndl, (U16)(len+3)));

    /* Build the OBEX Challenge Header Identifier */
    AppHndl->buffer[AppHndl->txLength++] = OBEXH_AUTH_CHAL;
    AppHndl->buffer[AppHndl->txLength++] = (U8)((len+3) >> 8);
    AppHndl->buffer[AppHndl->txLength++] = (U8) (len+3);

    /* Build nonce from challenge string */
    AppHndl->buffer[AppHndl->txLength++] = 0;                /* Nonce Code */
    AppHndl->buffer[AppHndl->txLength++] = AUTH_NONCE_LEN;   /* (16 bytes) */
    MD5(AppHndl->buffer+AppHndl->txLength, Challenge->challenge, Challenge->challengeLen);
    if (NonceOut)
        OS_MemCopy(NonceOut, AppHndl->buffer+AppHndl->txLength, AUTH_NONCE_LEN);
    AppHndl->txLength += AUTH_NONCE_LEN;

    /* Add Options */
    if (Challenge->options) {
        AppHndl->buffer[AppHndl->txLength++] = 1;    /* Options Code */
        AppHndl->buffer[AppHndl->txLength++] = 1;    /* Options Len */
        AppHndl->buffer[AppHndl->txLength++] = Challenge->options;   /* Options */
    }

    /* Add Realm */
    if (Challenge->realmLen) {
        AppHndl->buffer[AppHndl->txLength++] = 2;    /* Realm Code */
        AppHndl->buffer[AppHndl->txLength++] = Challenge->realmLen; /* Realm Len */
        OS_MemCopy(AppHndl->buffer+AppHndl->txLength, Challenge->realm, Challenge->realmLen);
        AppHndl->txLength += Challenge->realmLen;
    }

//#if XA_DEBUG == XA_ENABLED
    ASSERT((orig + len + 3) == (AppHndl->buffer + AppHndl->txLength));
//#endif
    OS_UnlockObex();
    return TRUE;
}

/*---------------------------------------------------------------------------
 *            OBEXH_BuildAuthResponse
 *---------------------------------------------------------------------------
 *
 * Synopsis:  This function is used by applications to build an Authentication
 *            response to a received Challenge. The header is constructed from
 *            the fields in the ObexAuthResponse structure.
 *
 * Return:    TRUE if header was successfully built.
 *            FALSE if headers would exceed limits on buffer or transmit space.
 */
BOOL OBEXH_BuildAuthResponse(ObexAppHandle *AppHndl, ObexAuthResponse *Response, 
                             U8 *Nonce)
{
    U8          len;
    xMD5Context context;
//#if XA_DEBUG == XA_ENABLED
    U8 *orig;
//#endif /* XA_DEBUG == XA_ENABLED */

    OS_LockObex();
#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl || !Response || !Nonce) {
        OS_UnlockObex();
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(AppHndl && Response && Nonce);

    len = CALC_RESPONSE_LEN(Response);
//#if XA_DEBUG == XA_ENABLED
    orig = AppHndl->buffer + AppHndl->txLength;
//#endif /* XA_DEBUG == XA_ENABLED */

#if XA_ERROR_CHECK == XA_ENABLED
    if (!ObIsHeaderSpaceAvail(AppHndl, (U16)(len+3))) {
        OS_UnlockObex();
        return FALSE;
    }
#endif
    ASSERT(ObIsHeaderSpaceAvail(AppHndl, (U16)(len+3)));

    /* Build the OBEX Response Header Identifier */
    AppHndl->buffer[AppHndl->txLength++] = OBEXH_AUTH_RESP;
    AppHndl->buffer[AppHndl->txLength++] = (U8)((len+3) >> 8);
    AppHndl->buffer[AppHndl->txLength++] = (U8) (len+3);

    /* Build digest from nonce and password */
    AppHndl->buffer[AppHndl->txLength++] = 0;                /* Request Digest Code */
    AppHndl->buffer[AppHndl->txLength++] = AUTH_NONCE_LEN;   /* Digest Length (16 bytes) */
    xMD5Init(&context);
    xMD5Update(&context, Nonce, AUTH_NONCE_LEN);
    xMD5Update(&context, (U8 *)":", 1);
    xMD5Update(&context, Response->password, Response->passwordLen);
    xMD5Final(AppHndl->buffer+AppHndl->txLength, &context);
    AppHndl->txLength += AUTH_NONCE_LEN;

    /* Add UserId */
    if (Response->userIdLen) {
        AppHndl->buffer[AppHndl->txLength++] = 1;    /* UserId */
        AppHndl->buffer[AppHndl->txLength++] = Response->userIdLen; /* Realm Len */
        OS_MemCopy(AppHndl->buffer+AppHndl->txLength, Response->userId, Response->userIdLen);
        AppHndl->txLength += Response->userIdLen;
    }

    /* Add Nonce */
    AppHndl->buffer[AppHndl->txLength++] = 2;                /* Nonce */
    AppHndl->buffer[AppHndl->txLength++] = AUTH_NONCE_LEN;   /* (16 bytes) */
    OS_MemCopy(AppHndl->buffer+AppHndl->txLength, Nonce, AUTH_NONCE_LEN);
    AppHndl->txLength += AUTH_NONCE_LEN;

//#if XA_DEBUG == XA_ENABLED
    ASSERT((orig + len + 3) == (AppHndl->buffer + AppHndl->txLength));
//#endif
    OS_UnlockObex();
    return TRUE;
}

/*---------------------------------------------------------------------------
 *            OBEXH_ParseAuthChallenge
 *---------------------------------------------------------------------------
 *
 * Synopsis: Applications use this function to collect Authentication
 *           Challenge information as it is indicated in OBxx_HEADER_RX events.
 *           This function converts the raw header data into the provided
 *           structure. It must be called during every OBxx_HEADER_RX event
 *           when the header type is OBEX_AUTH_CHAL.
 *
 * Return:   TRUE when the entire header has been processed.
 */
BOOL OBEXH_ParseAuthChallenge(ObexAppHandle *AppHndl, ObexAuthChallengeInfo *Info)
{
#if OBEX_MAX_REALM_LEN > 0
    U16     toCopy;
#endif
    ObexTlv tlv;

#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl || !Info) {
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(AppHndl && Info && IsObexLocked());

    if ((AppHndl->parser).dataLen == 0) {
#if OBEX_MAX_REALM_LEN > 0
        /* Initialize the realm length */
        Info->realmLen = 0;
#endif
    }

    /* Parse Authentication Challenge tags */
    while (OBEXH_ParseTlv(AppHndl, &tlv))
    {
        switch(tlv.tag) {

        case 0: /* Nonce (fixed length, 16 bytes) */
            
            /* If the length is incorrect, clear the NONCE */
            if (tlv.length == AUTH_NONCE_LEN) {
                ASSERT(tlv.valueLen == tlv.length);
                OS_MemCopy(Info->nonce, tlv.value, tlv.valueLen);
            }
            break;

        case 1: /* Options (fixed length, 1 byte) */
            if (tlv.length == 1) {
                Info->options = *tlv.value;
            }
            break;

#if OBEX_MAX_REALM_LEN > 0
        case 2: /* Realm (variable length) */
            toCopy = min(tlv.valueLen, (OBEX_MAX_REALM_LEN - Info->realmLen));

            OS_MemCopy(Info->realm + Info->realmLen, tlv.value, toCopy);
            Info->realmLen += toCopy;
            break;
#endif
        default:
            continue;
        }
    }

    /* Indicate whether the entire header has been parsed. */
    return ((AppHndl->parser).headerLen == 0);
}

/*---------------------------------------------------------------------------
 *            OBEXH_ParseAuthResponse
 *---------------------------------------------------------------------------
 *
 * Synopsis: Applications use this function to collect Authentication
 *           Response information as it is indicated in OBxx_HEADER_RX events.
 *           This function converts the raw header data into the provided
 *           structure. It must be called during every OBxx_HEADER_RX event
 *           when the header type is OBEX_AUTH_RESP.
 *
 * Return:   TRUE when the entire header has been processed.
 */
BOOL OBEXH_ParseAuthResponse(ObexAppHandle *AppHndl, ObexAuthResponseInfo *Info)
{
#if OBEX_MAX_USERID_LEN > 0
    U16     toCopy;
#endif
    ObexTlv tlv;

#if XA_ERROR_CHECK == XA_ENABLED
    if (!AppHndl || !Info) {
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(AppHndl && Info && IsObexLocked());

    if ((AppHndl->parser).dataLen == 0) {
        /* Initialize the User ID length */
#if OBEX_MAX_USERID_LEN > 0
        Info->userIdLen = 0;
#endif
    }

    /* Parse Authentication Response tags */
    while (OBEXH_ParseTlv(AppHndl, &tlv))
    {
        switch(tlv.tag)
        {
        case 0: /* Request-Digest (fixed length, 16 bytes) */
            if (tlv.length == AUTH_NONCE_LEN) {
                OS_MemCopy(Info->digest, tlv.value, tlv.valueLen); 
            }
            break;

#if OBEX_MAX_USERID_LEN > 0
        case 1: /* User Id (variable length) */
            toCopy = min(tlv.valueLen, (OBEX_MAX_USERID_LEN - Info->userIdLen));

            OS_MemCopy(Info->userId + Info->userIdLen, tlv.value, toCopy);
            Info->userIdLen += toCopy;
            break;
#endif /* OBEX_MAX_USERID_LEN > 0 */

        case 2: /* Nonce (fixed length, 16 bytes) */
            if (tlv.length == AUTH_NONCE_LEN) {
                OS_MemCopy(Info->nonce,  tlv.value, tlv.valueLen);
            }
            break;
        }
    }

    /* Indicate whether the entire header has been parsed. */
    return ((AppHndl->parser).headerLen == 0);
}

/*---------------------------------------------------------------------------
 *            OBEXH_VerifyAuthResponse
 *---------------------------------------------------------------------------
 *
 * Synopsis: This function is used by both client & servers to verify the
 *           received authentication response digest against the expected value.
 *
 * Return:   TRUE - The response is authenticated.
 */
BOOL OBEXH_VerifyAuthResponse(ObexAuthResponseVerify *Verify)
{
    xMD5Context     context;
    U8              digest[AUTH_NONCE_LEN];

#if XA_ERROR_CHECK == XA_ENABLED
    if (!Verify) {
        return FALSE;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */
    ASSERT(Verify);
    
    xMD5Init(&context);
    xMD5Update(&context, Verify->nonce, AUTH_NONCE_LEN);
    xMD5Update(&context, (U8 *)":", 1);
    xMD5Update(&context, Verify->password, Verify->passwordLen);
    xMD5Final(digest, &context);

    return OS_MemCmp(digest, AUTH_NONCE_LEN, Verify->digest, AUTH_NONCE_LEN);
}

#endif
