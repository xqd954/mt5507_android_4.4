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
 *     $Workfile:sbc_math.h$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains header definitions for the REAL (fixed 
 *     or floating-point) math functions.
 *             
 * Created:     September 14, 2004
 *
 * Copyright 2004 - 2005  Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
 
#ifndef _SBC_MATH_H_
#define _SBC_MATH_H_

#define XA_DISABLED 0   /* Disable the feature */
#define XA_ENABLED  1   /* Enable the feature */


/*---------------------------------------------------------------------------
 * Include the override file. Settings in the override file will take 
 * precendence over the settings in this file and other configuration files.
 */
#include "sbcconfig.h"

/****************************************************************************
 *
 * Base definitions used by the stack and framer
 *
 ****************************************************************************/

#define XA_INTEGER_SIZE    2
#ifndef BOOL_DEFINED
typedef int BOOL;   /* IGNORESTYLE */
#endif

typedef unsigned long  U32;
typedef unsigned short U16;
typedef unsigned char  U8;

typedef          long  S32;
typedef          short S16;
typedef          char  S8;

/* Variable sized integers. Used to optimize processor efficiency by
 * using the most efficient data size for counters, arithmatic, etc.
 */
typedef unsigned long  I32;

#if XA_INTEGER_SIZE == 4
typedef unsigned long  I16;
typedef unsigned long  I8;
#elif XA_INTEGER_SIZE == 2
typedef unsigned short I16;
typedef unsigned short I8;
#elif XA_INTEGER_SIZE == 1
typedef unsigned short I16;
typedef unsigned char  I8;
#else
#error No XA_INTEGER_SIZE specified!
#endif

typedef void (*PFV) (void);

/* Boolean Definitions */
#ifndef TRUE
#define TRUE  (1==1)
#endif /* TRUE */

#ifndef FALSE
#define FALSE (0==1)  
#endif /* FALSE */


              
/*---------------------------------------------------------------------------
 * SBC Real Math layer
 *
 *     SBC requires operations on real numbers when performing the synthesis
 *     or analysis phases.  The XTNDAccess SBC CODEC is designed to use either 
 *     C double precision floating-point or fixed-point integer based math.  
 *     Since many processor do not provide floating-point operations the 
 *     default is to use fixed-point math.  This library provides the math 
 *     functions required by the XTNDAccess SBC CODEC.  
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/
 
/*---------------------------------------------------------------------------
 * SBC_USE_FIXED_POINT constant
 *      Determines whether REAL number math is performed using fixed-point or
 *      floating-point numbers.  Fixed math for XTNDAccess SDKs uses a 32-bit
 *      17:15 format, where the high order 17 bits represent the whole part of
 *      the number and the low order 15 bits represent the fractional part of
 *      the number.
 *
 *      Fixed-point is faster than floating-point on most processors, 
 *      especially where no floating-point coprocessor is available.  The 
 *      precision of fixed-point, however, is not as good as floating-point.
 *
 *      The default is to use fixed-point math.
 */
#ifndef SBC_USE_FIXED_POINT
#define SBC_USE_FIXED_POINT XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * S64 constant
 *     Provides a 64-bit data type for use with REAL number macros when
 *     SBC_USE_FIXED_POINT is enabled.  If a 64-bit data type is available in 
 *     the target environment, then this constant can be defined to a value
 *     supported by the compiler.  If no 64-bit data type is supported by the
 *     target, then the default Fixed Point macros for the Mul and Div functions
 *     are not supported.  In this case, either macros of functions must be 
 *     provided by the developer.
 *
 *     Some compilers do not require a 64-bit data type, because interim results
 *     are stored in 64-bit registers (or two 32-bit registers).  In this case
 *     this data type can simply be defined as U32.
 *
 *     The default is to define S64 as __int64.
 */
#ifndef S64
#define S64 __int64
#endif

/*---------------------------------------------------------------------------
 * SBC_USE_FIXED_MACROS constant
 *      Determines whether macros are used for the Fixed Point
 *      Mul and Div functions.  Default macros are available in C as long as
 *      a valid S64 data type is available in the target environment. 
 *      If a 64-bit data type is not available, then either assembly language
 *      routines (preferably using a 64-bit register) or C language routines
 *      must be used (provided by the developer).  If Assembly language 
 *      or C language routines are provided, they can be defined as macros
 *      or as external functions.  If they are defined as functions, then
 *      this constant must be disabled.  If SBC_USE_FIXED_POINT is disabled,
 *      then macros are used for floating-point calculations, and this 
 *      constant is ignored.
 *
 *      The default is to use fixed-point macros.
 */
#ifndef SBC_USE_FIXED_MACROS
#define SBC_USE_FIXED_MACROS XA_ENABLED
#endif

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/
 
/*---------------------------------------------------------------------------
 * REAL type
 *
 * Fixed-point math (17:15) or double precision floating-point type.  If 
 * SBC_USE_FIXED_POINT is used, then REAL is defined as a 32-bit signed
 * value, otherwise it is defined as a double precision floating-point value.
 * Lookup table values are stored in 2:32 format for increased precision, 
 * therefore the Mul() function must take this into account.  The
 * Div() function always operates on 17:15 fixed numbers.
 */
#if SBC_USE_FIXED_POINT == XA_ENABLED
typedef S32 REAL;
#else
typedef double REAL;
#endif
/* End of REAL */

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * S16toReal()
 *
 *     Converts a signed 16-bit integer to a real number (17:15 fixed or float).
 *
 * Parameters:
 *
 *     Integer - a 16 bit signed integer.
 *
 * Returns:
 *
 *     A real number (fixed or float).
 */
REAL S16toReal(S16 Integer);

/*---------------------------------------------------------------------------
 * RealtoS16()
 *
 *     Converts a real number (17:15 fixed or float) to a signed 16 bit integer.
 *
 * Parameters:
 *
 *     Real - a real number (fixed or float).
 *
 * Returns:
 *
 *     A 16 bit signed integer.
 */
S16 RealtoS16(REAL Real);

/*---------------------------------------------------------------------------
 * RealtoU16()
 *
 *     Converts a real number (17:15 fixed or float) to an unsigned 16 bit integer.
 *
 * Parameters:
 *
 *     Real - a real number (fixed or float).
 *
 * Returns:
 *
 *     A 16 bit unsigned integer.
 */
U16 RealtoU16(REAL Integer);

/*---------------------------------------------------------------------------
 * RealtoS32()
 *
 *     Converts a real number (17:15 fixed or float) to a signed 32-bit integer.
 *
 * Parameters:
 *
 *     Real - a real number (fixed or float).
 *
 * Returns:
 *
 *     A 32-bit signed integer.
 */
S32 RealtoS32(REAL Real);

/*---------------------------------------------------------------------------
 * Div()
 *
 *     Divides two real numbers (17:15 fixed or float).  For fixed, both 
 *     numbers are always in 17:15 format.
 *
 *     For fixed-point math, the following formula is used.
 *
 *     (x << 15) / y
 *
 * Parameters:
 *
 *     x - a real number (fixed or float).
 *
 *     y - a real number (fixed or float).
 *
 * Returns:
 *
 *     Results of the divistion (fixed or float).
 */
REAL Div(REAL x, REAL y);

/*---------------------------------------------------------------------------
 * Mul()
 *
 *     Multiplies two real numbers (fixed or float).  For fixed, both of the
 *     numbers are in 17:15 format.
 *
 *     For fixed-point math, the following formula is used.
 *
 *     (x * y) >> 15
 *
 * Parameters:
 *
 *     x - a real number (fixed or float).
 *
 *     y - a real number (fixed or float).
 *
 * Returns:
 *
 *     Results of the multiplication (fixed or float).
 */
REAL Mul(REAL x, REAL y);

/*---------------------------------------------------------------------------
 * MulP()
 *
 *     Multiplies two real numbers (fixed or float) with high precision.  For 
 *     fixed, one of the numbers is in 17:15 format and the other is in 2:30
 *     format.  This requires a 30-bit shift right after multiplying the two 
 *     numbers.
 *
 *     For fixed-point math, the following formula is used.
 *
 *     (x * y) >> 30
 *
 * Parameters:
 *
 *     x - a real number (fixed or float).
 *
 *     y - a real number (fixed or float).
 *
 * Returns:
 *
 *     Results of the multiplication (fixed or float).
 */
REAL MulP(REAL x, REAL y);

#if SBC_USE_FIXED_POINT == XA_ENABLED

/* Some useful contants */
#define ONE_F       (0x00008000)
#define ONE_F_P     (ONE_F << 15) /* For higher precision calculation */
#define TWO_F       (0x00010000)
#define FOUR_F      (0x00020000)
#define EIGHT_F     (0x00040000)
#define ONE_HALF_F  (0x00004000)

/* Signed 16 to REAL */
#define S16toReal(x) ((REAL)(((S32)(x)) << 15))

/* Clip a positive signed number */
#define ClipPos(x) (((x) > 0x3FFF8000) ? 0x3FFF8000 : (x))

/* Clip a negative signed number */
#define ClipNeg(x) (((x) < 0xC0000000) ? 0xC0000000 : (x))

/* Clip a value to largest or smallest 16 bit signed value */
#define Clip(x) (((x) < 0) ? ClipNeg(x) : ClipPos(x))

/* REAL to signed 16 bit value */
#define RealtoS16(x) ((S16)((REAL)(Clip(x)) >> 15))

/* REAL to unsigned 16 bit value */
#define RealtoU16(x) ((U16)((REAL)(Clip(x)) >> 15))

/* REAL to signed 32-bit value */
#define RealtoS32(x) ((S32)((REAL)(x) >> 15))

#if SBC_USE_FIXED_MACROS == XA_ENABLED
#ifndef Mul
/* Multiply a REAL by a REAL */
#define Mul(x,y) (REAL)(((S64)(x) * (S64)(y)) >> 15)  
#endif

#ifndef MulP
/* Multiply a REAL by a REAL with high precision */
#define MulP(x,y) (REAL)(((S64)(x) * (S64)(y)) >> 30)  
#endif

#ifndef Div
/* Divide a REAL by a REAL */
#define Div(x,y) (REAL)(((S64)(x) << 15) / (y))
#endif
#endif /* SBC_USE_FIXED_MACROS == XA_ENABLED */
#else /* SBC_USE_FIXED_POINT == XA_ENABLED */

/* Some useful contants */
#define ONE_F      (1.0)
#define ONE_F_P    (1.0)
#define TWO_F      (2.0)
#define FOUR_F     (4.0)
#define EIGHT_F    (8.0)
#define ONE_HALF_F (0.5)

/* Signed 16 to REAL */
#define S16toReal(x) ((REAL)((S32)(x)))

/* Clip a positive signed number */
#define ClipPos(x) (((x) > 32767.0) ? 32767.0 : (x))

/* Clip a negative signed number */
#define ClipNeg(x) (((x) < -32767.0) ? -32767.0 : (x))

/* Clip a value to largest or smallest 16 bit signed value */
#define Clip(x) (((x) < 0.0) ? ClipNeg(x) : ClipPos(x))

/* REAL to signed 16 bit value */
#define RealtoS16(x) ((S16)(Clip(x)))

/* REAL to unsigned 16 bit value */
#define RealtoU16(x) ((U16)(Clip(x)))

/* REAL to signed 32-bit value */
#define RealtoS32(x) ((S32)(x))

/* Multiply a REAL by a REAL  */
#define Mul(x,y) ((REAL)(x)*(REAL)(y))

/* Multiply a REAL by a REAL with high precision */
#define MulP(x,y) ((REAL)(x)*(REAL)(y))

/* Divide a REAL by a REAL */
#define Div(x,y) ((REAL)(x)/(REAL)(y))

#endif /* SBC_USE_FIXED_POINT == XA_ENABLED */

#endif /* _SBC_MATH_H_ */

