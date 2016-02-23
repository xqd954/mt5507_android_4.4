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
 * File:        bipparse.c
 *     $Workfile:bipparse.c$ for XTNDAccess Blue SDK, Version 2.x
 *     $Revision: #1 $
 *
 * Description: This file contains utility routines used to parse BIP
 *     messages.
 *
 * Created:     August 6, 2002
 *
 * $Project:XTNDAccess Blue SDK$
 *
 * Copyright 2002-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
 * Apr 11 2008 mbj06032
 * [MAUI_00653579] [BT BIP ADP] add $Log: $ to bip adp files
 * 
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 *
 ****************************************************************************/
/* Keep this header file outside the compile option to avoid compile warning */
// #include "kal_release.h"      	/* Basic data type */

#ifdef __BT_BIP_PROFILE__

#include "bipi.h" //"sys/bipi.h"
#include "bt_osal.h"

U8 BipToLower(U8 ch)
{
    /* return lowercase, if uppercase */
    if (ch >= 'A' && ch <= 'Z') {
        return ch + 32;
    }
    return ch;
}

U8 BipToUpper(U8 ch)
{
    /* return lowercase, if uppercase */
    if (ch >= 'a' && ch <= 'z') {
        return ch - 32;
    }
    return ch;
}

/*---------------------------------------------------------------------------
 *            BipPatCmp
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Does a lower-case compare of pattern with text.
 *
 * Return:    BOOL.
 *
 */
BOOL BipPatCmp(const U8 *pat, const U8 *text)
{
    U16 len = OS_StrLen((const char *)pat) - 1;

    while (len--) {
        if (*pat++ != BipToLower(*text++))
            return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------
 *            BipGetType()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Determine if obex header Type is in the mask.
 *
 * Return:    
 */
BipType BipGetType(U8 *type, BipType bipTypeMask)
{
    BipType bipType;

    /* Check for a valid type string*/
    if (BipPatCmp((const U8 *)"x-bt/img-", type) != TRUE) {
        return 0;
    }
        
    switch (BipToLower(type[9])) {
    case 'c':
        bipType = BIPTYPE_CAPABILITIES;
        break;

    case 'l':
        bipType = BIPTYPE_LISTING;
        break;

    case 'p':
        if (BipToLower(type[11]) == 'o') {
            bipType = BIPTYPE_PROPERTIES;
        }
        else if (BipToLower(type[11]) == 'r') {
            bipType = BIPTYPE_PARTIAL;
        }
        else {
            /* type[11] == 'i' */
            bipType = BIPTYPE_PRINT;
        }
        break;

    case 'i':
        bipType = BIPTYPE_IMG;
        break;

    case 't':
        bipType = BIPTYPE_THM;
        break;

    case 'a':
        if (BipToLower(type[10]) == 't') {
            bipType = BIPTYPE_ATTACHMENT;
        }
        else {
            /* type[10] == 'r' */
            bipType = BIPTYPE_ARCHIVE;
        }
        break;

    case 'm':
        bipType = BIPTYPE_MONITORING;
        break;

    case 's':
        bipType = BIPTYPE_STATUS;
        break;

    case 'd':
        bipType = BIPTYPE_DISPLAY;
        break;

    default:
        return 0;
    }

    return bipType & bipTypeMask;
}

#endif /*__BT_BIP_PROFILE__*/

