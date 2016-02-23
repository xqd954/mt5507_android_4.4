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
 *     $Workfile:sbc_math.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains routines for fixed point math functions.
 *             
 * Created:     September 14, 2004
 *
 * Copyright 2004 - 2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, Inc.  
 * This work contains confidential and proprietary information of Extended 
 * Systems, Inc. which is protected by copyright, trade secret, trademark and 
 * other intellectual property rights.
 *
 ****************************************************************************/

#include "sbc_math.h"



#if SBC_USE_FIXED_MACROS == XA_DISABLED
#if SBC_USE_FIXED_POINT == XA_ENABLED
#if SBC_USE_FIXED_ASM == XA_ENABLED

/*---------------------------------------------------------------------------
 *  The following Mul, MulP, and Div functions are implemented for the Intel
 *  architecture with 64 bit registers using the following formula:
 *
 *  Mul:  (x * y) >> 15
 *
 *  MulP:  (x * y) >> 30
 *
 *  Div:  (x << 15) / y
 */

/* X86 Assembly routine for fixed multiply */
REAL Mul(REAL x, REAL y)
{
    __asm {
        mov  eax, x
        xor  edx, edx
        imul y
        shrd eax, edx, 15
    }
}

/* X86 Assembly routine for fixed multiply for high precision */
REAL MulP(REAL x, REAL y)
{
    __asm {
        mov  eax, x
        xor  edx, edx
        imul y
        shrd eax, edx, 30
    }
}

/* X86 Assembly routine for an fixed divide */
REAL Div(REAL x, REAL y)
{
    __asm {
        mov  edx, x
        xor  eax, eax
        shrd eax, edx, 17
        sar  edx, 17
        idiv y
    }
}

#else

/*---------------------------------------------------------------------------
 *  The following Mul, MulP and Div functions are portable, but not as efficient
 *  as the assembly routines.  They do not, however, require a 64 bit data 
 *  type, or a compiler that handles interim results in a 64 bit register.
 */

/* C routine for fixed multiply */
REAL Mul(REAL x, REAL y)
{
    S32 result;
    U8 sign = 0;
    
    if (x < 0)
    {
        sign = 1;
        x = -x;
    }

    if (y < 0)
    {
        sign ^= 1;
        y = -y;
    }

    result =  ((x & 0x0000FFFF) * (y & 0x0000FFFF)) >> 15;
    result += ((x >> 16) * (y & 0x0000FFFF)) << 1;
    result += ((y >> 16) * (x & 0x0000FFFF)) << 1;
    result += ((x >> 16) * (y >> 16)) >> 1;

    return sign ? -result : result;
} 

/* C routine for fixed multiply for high precision */
REAL MulP(REAL x, REAL y)
{
    S32 result;
    U8 sign = 0;
    
    if (x < 0)
    {
        sign = 1;
        x = -x;
    }

    if (y < 0)
    {
        sign ^= 1;
        y = -y;
    }

    result =  ((x & 0x0000FFFF) * (y & 0x0000FFFF)) >> 30;
    result += (((x >> 16) * (y & 0x0000FFFF)) >> 14);
    result += (((y >> 16) * (x & 0x0000FFFF)) >> 14);
    result += ((x >> 16) * (y >> 16)) << 2;

    return sign ? -result : result;
} 

/* C routine for an fixed divide */
REAL Div(REAL x, REAL y)
{
    S32 result = 0;
    U32 tmp;
    U8 sign = 0;
    
    if (x < 0)
    {
        sign = 1;
        x = -x;
    }

    if (y < 0)
    {
        sign ^= 1;
        y = -y;
    }

    tmp    = ((U32)(x & 0xFFFF0000) / y) << 15;
    tmp   += (((U32)(x & 0x0000FFFF) << 16) / y) >> 1;

    result = tmp;

    return sign ? -result : result;
}

#endif
#endif /* SBC_USE_FIXED_POINT == XA_ENABLED */
#endif /* SBC_USE_FIXED_MACROS == XA_DISABLED */

