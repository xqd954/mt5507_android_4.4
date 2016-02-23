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

/* armdsp.h
 *
 * Copyright 2001 ARM Limited. All rights reserved.
 *
 * RCS $Revision: #1 $
 * Checkin $Date: 2014/10/13 $
 * Revising $Author: dtvbm11 $
 */

/* ----------------------------------------------------------------------
 * This header file provides a set of DSP-type primitive
 * operations, such as saturating integer arithmetic and 16x16 bit
 * multiplication. The operations it provides are exactly the same
 * operations supported directly by the ARM9E.
 * 
 * This header file is intended as an example implementation. It
 * demonstrates how to use the inline assembly feature in the ARM
 * compilers, to construct intrinsic functions that provide
 * C-language access to the ARM9E's DSP capability.
 */
 
#ifndef SBC_ARMDSP_ARMDSP_H
#define SBC_ARMDSP_ARMDSP_H

typedef long long Word64;
typedef long Word32;
typedef short Word16;
#define ARMDSP_WORD32_MAX ((Word32)0x7FFFFFFF)
#define ARMDSP_WORD32_MIN ((Word32)0x80000000)

#define lo64(a) (((unsigned*) &a)[0])    /* low 32 bits of a long long */
#define hi64(a) (((int*) &a)[1])        /* high 32 bits of a long long */

#ifdef __cplusplus
extern "C" {
#endif

static inline Word32 _saturate(Word64 ret)
{
    if(ret > ARMDSP_WORD32_MAX)
    {
        ret = ARMDSP_WORD32_MAX;
    }
    else if(ret < ARMDSP_WORD32_MIN)
    {
        ret = ARMDSP_WORD32_MIN;
    }
    return (Word32)ret;
}

/*
 * Saturating 32-bit integer addition.
 */
static inline Word32 qadd(Word32 x, Word32 y)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "qadd %[ret], %[x], %[y] \n" 
       : [ret]"=r"(ret) 
       : [x]"%r"(x), [y]"r"(y));
#else
    Word64 r64;
    r64 = (Word64)x + (Word64)y;
    ret = _saturate(r64);
#endif
    return ret;
}

/*
 * Saturating 32-bit integer subtraction.
 */
static inline Word32 qsub(Word32 x, Word32 y)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "qsub %[ret], %[x], %[y] \n" 
       : [ret]"=r"(ret) 
       : [x]"%r"(x), [y]"r"(y));    
#else
    Word64 r64;
    r64 = (Word64)x - (Word64)y;
    ret = _saturate(r64);
#endif
    return ret;
}

/*
 * Saturating 32-bit integer addition, with the second operand
 * multiplied by two. (i.e. return x + 2 * y)
 */
static inline Word32 qdadd(Word32 x, Word32 y)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "qdadd %[ret], %[x], %[y] \n" 
       : [ret]"=r"(ret) 
       : [x]"%r"(x), [y]"r"(y));
#else
    Word64 r64;
    r64 = (Word64)x + 2*(Word64)y;
    ret = _saturate(r64);
#endif
    return ret;
}

/*
 * Saturating 32-bit integer subtraction, with the second operand
 * multiplied by two. (i.e. return x - 2 * y)
 */
static inline Word32 qdsub(Word32 x, Word32 y)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "qdsub %[ret], %[x], %[y] \n" 
       : [ret]"=r"(ret) 
       : [x]"%r"(x), [y]"r"(y));
#else
    Word64 r64;
    r64 = (Word64)x - 2*(Word64)y;
    ret = _saturate(r64);
#endif
    return ret;
}

/*
 * Multiply two signed 16-bit numbers, taken from the bottom or top
 * halves of the input words. For example, smulbt(x,y) multiplies
 * the bottom 16 bits of x with the top 16 bits of y.
 */

/* smulbb - multiply bottom half of x and bottom half of y */
static inline Word32 smulbb(Word32 x, Word32 y)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "smulbb %[ret], %[x], %[y] \n" 
       : [ret]"=r"(ret) 
       : [x]"%r"(x), [y]"r"(y));
#else
    x = (x << 16) >> 16;
    y = (y << 16) >> 16;
    ret = x * y;
#endif
    return ret;
}

/* smulbt - multiply bottom half of x and top half of y */
static inline Word32 smulbt(Word32 x, Word32 y)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "smulbt %[ret], %[x], %[y] \n" 
       : [ret]"=r"(ret) 
       : [x]"%r"(x), [y]"r"(y));
#else
    x = (x << 16) >> 16;
    y = y >> 16;
    ret = x * y;
#endif
    return ret;
}

/* smultb - multiply top half of x and bottom half of y */
static inline Word32 smultb(Word32 x, Word32 y)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "smultb %[ret], %[x], %[y] \n" 
       : [ret]"=r"(ret) 
       : [x]"%r"(x), [y]"r"(y));
#else
    x = x >> 16;
    y = (y << 16) >> 16;
    ret = x * y;
#endif
    return ret;
}

/* smultt - multiply top half of x and top half of y */
static inline Word32 smultt(Word32 x, Word32 y)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "smultt %[ret], %[x], %[y] \n" 
       : [ret]"=r"(ret) 
       : [x]"%r"(x), [y]"r"(y));
#else
    x = x >> 16;
    y = y >> 16;
    ret = x * y;
#endif
    return ret;
}

/*
 * Multiply two signed 16-bit numbers, taken from the bottom or top
 * halves of the input words x and y, and add to the input word a.
 * For example, smlabt(x,y,a) multiplies the bottom 16 bits of x with
 * the top 16 bits of y, and then adds a.
 */

/* smlabb - multiply bottom half of x and bottom half of y */
static inline Word32 smlabb(Word32 x, Word32 y, Word32 a)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "smlabb %[ret], %[x], %[y], %[a] \n" 
    : [ret]"=r"(ret) 
    : [x]"%r"(x), [y]"r"(y), [a]"r"(a) );
#else
    x = (x << 16) >> 16;
    y = (y << 16) >> 16;
    ret = x * y + a;
#endif
    return ret;
}

/* smlabt - multiply bottom half of x and top half of y */
static inline Word32 smlabt(Word32 x, Word32 y, Word32 a)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "smlabt %[ret], %[x], %[y], %[a] \n" 
    : [ret]"=r"(ret) 
    : [x]"%r"(x), [y]"r"(y), [a]"r"(a) );
#else
    x = (x << 16) >> 16;
    y = y >> 16;
    ret = x * y + a;
#endif
    return ret;
}

/* smlatb - multiply top half of x and bottom half of y */
static inline Word32 smlatb(Word32 x, Word32 y, Word32 a)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "smlatb %[ret], %[x], %[y], %[a] \n" 
    : [ret]"=r"(ret) 
    : [x]"%r"(x), [y]"r"(y), [a]"r"(a) );
#else
    x = x >> 16;
    y = (y << 16) >> 16;
    ret = x * y + a;
#endif
    return ret;
}

/* smlatt - multiply top half of x and top half of y */
static inline Word32 smlatt(Word32 x, Word32 y, Word32 a)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "smlatt %[ret], %[x], %[y], %[a] \n" 
    : [ret]"=r"(ret) 
    : [x]"%r"(x), [y]"r"(y), [a]"r"(a) );
#else
    x = x >> 16;
    y = y >> 16;
    ret = x * y + a;
#endif
    return ret;
}

/*
 * Multiply two signed 16-bit numbers, taken from the bottom or top
 * halves of the input words x and y, and add to the 64-bit integer
 * stored in the input words lo and hi. For example,
 * smlalbt(&lo,&hi,x,y) multiplies the bottom 16 bits of x with the
 * top 16 bits of y, and then adds the result to (hi,lo).
 * 
 * WARNING: if you use these primitives too often inside the same
 * function, they may fail to be inlined.
 */

/* smlalbb - multiply bottom half of x and bottom half of y */
static inline void smlalbb(Word32 *lo, Word32 *hi, Word32 x, Word32 y)
{
#if defined(__GNUC__) && defined(__arm__)
    asm( "smlalbb %[lo], %[hi], %[x], %[y] \n" 
    : [lo]"=r"(*lo), [hi]"=r"(*hi)
    : [x]"%r"(x), [y]"r"(y) );
#else
    Word64 ret;
    ret = *lo;
    ret+= (long long)*hi<<32;
    x = (x << 16) >> 16;
    y = (y << 16) >> 16;
    ret = x * y + ret;
    *lo = (ret<<32)>>32;
    *hi = ret>>32;
#endif
   
}

/* smlalbt - multiply bottom half of x and top half of y */
static inline void smlalbt(Word32 *lo, Word32 *hi, Word32 x, Word32 y)
{
#if defined(__GNUC__) && defined(__arm__)
    asm( "smlalbt %[lo], %[hi], %[x], %[y] \n" 
    : [lo]"=r"(*lo), [hi]"=r"(*hi)
    : [x]"%r"(x), [y]"r"(y) );
#else
    Word64 ret;
    ret = *lo;
    ret+= (long long)*hi<<32;
    x = (x << 16) >> 16;
    y = y >> 16;
    ret = x * y + ret;
    *lo = (ret<<32)>>32;
    *hi = ret>>32;
#endif
 
}

/* smlaltb - multiply top half of x and bottom half of y */
static inline void smlaltb(Word32 *lo, Word32 *hi, Word32 x, Word32 y)
{
#if defined(__GNUC__) && defined(__arm__)
    asm( "smlaltb %[lo], %[hi], %[x], %[y] \n" 
    : [lo]"=r"(*lo), [hi]"=r"(*hi) 
    : [x]"%r"(x), [y]"r"(y) );
#else
    Word64 ret;
    ret = *lo;
    ret+= (long long)*hi<<32;
    x = x >> 16;
    y = (y << 16) >> 16;
    ret = x * y + ret;
    *lo = (ret<<32)>>32;
    *hi = ret>>32;
#endif

}

/* smlaltt - multiply top half of x and top half of y */
static inline void smlaltt(Word32 *lo, Word32 *hi, Word32 x, Word32 y)
{
#if defined(__GNUC__) && defined(__arm__)
    asm( "smlaltt %[lo], %[hi], %[x], %[y] \n" 
    : [lo]"=r"(*lo), [hi]"=r"(*hi)
    : [x]"%r"(x), [y]"r"(y) );
#else
    Word64 ret;
    ret = *lo;
    ret+= (long long)*hi<<32;
    x = x >> 16;
    y = y >> 16;
    ret = x * y + ret;
    *lo = (ret<<32)>>32;
    *hi = ret>>32;
#endif
    
}

/*
 * Multiply a 32-bit signed integer (the input word x) with a
 * 16-bit signed integer (taken from either the top or the bottom
 * half of the input word y). Discard the bottom 16 bits of the
 * 48-bit product.
 */

/* smulwb - multiply x by the bottom half of y */
static inline Word32 smulwb(Word32 x, Word32 y)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "smulwb %[ret], %[x], %[y] \n" 
       : [ret]"=r"(ret) 
       : [x]"%r"(x), [y]"r"(y));
#else
    Word64 ret64;
    y = (y << 16) >> 16;
    ret64 = (Word64)x * (Word64)y;
    ret = (Word32)(ret64 >> 16);
#endif
    return ret;
}

/* smulwt - multiply x by the top half of y */
static inline Word32 smulwt(Word32 x, Word32 y)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "smulwt %[ret], %[x], %[y] \n" 
       : [ret]"=r"(ret) 
       : [x]"%r"(x), [y]"r"(y));
#else
    Word64 ret64;
    y = y >> 16;
    ret64 = (Word64)x * (Word64)y;
    ret = (Word32)(ret64 >> 16);
#endif
    return ret;
}

/*
 * Multiply a 32-bit signed integer (the input word x) with a
 * 16-bit signed integer (taken from either the top or the bottom
 * half of the input word y). Discard the bottom 16 bits of the
 * 48-bit product, and add to the input word a.
 */

/* smlawb - multiply x by the bottom half of y */
static inline Word32 smlawb(Word32 x, Word32 y, Word32 a)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "smlawb %[ret], %[x], %[y], %[a] \n" 
    : [ret]"=r"(ret) 
    : [x]"%r"(x), [y]"r"(y), [a]"r"(a) );
#else
    Word64 ret64;
    y = (y << 16) >> 16;
    ret64 = (Word64)x * (Word64)y;
    ret = (Word32)(ret64 >> 16) + a;
#endif
    return ret;
}

/* smlawt - multiply x by the top half of y */
static inline Word32 smlawt(Word32 x, Word32 y, Word32 a)
{
    Word32 ret;
#if defined(__GNUC__) && defined(__arm__)
    asm( "smlawt %[ret], %[x], %[y], %[a] \n" 
    : [ret]"=r"(ret) 
    : [x]"%r"(x), [y]"r"(y), [a]"r"(a) );
#else
    Word64 ret64;
    y = y >> 16;
    ret64 = (Word64)x * (Word64)y;
    ret = (Word32)(ret64 >> 16) + a;
#endif
    return ret;
}
/*
static inline Word32 smull(Word32 x, Word32 y)
{
	  Word64 ret;
	  
#if defined(__GNUC__) && defined(__arm__)
    asm( "smull %[low], %[high], %[x], %[y] \n" 
    : [low]"=r"(lo64(ret)), [high]"=r"(hi64(ret))
    : [x]"%r"(x), [y]"r"(y));
    
    return hi64(ret);
#else
    ret = (Word64)x * (Word64)y;
    return((Word32)hi64(ret));
#endif
}

static inline Word64 smlal(Word64 LL_acc, Word32 x, Word32 y)
{
#if defined(__GNUC__) && defined(__arm__)
    asm( "smlal %[low], %[high], %[x], %[y] \n" 
    : [low]"=r"(lo64(LL_acc)), [high]"=r"(hi64(LL_acc))
    : [x]"%r"(x), [y]"r"(y));
#else
    LL_acc += (Word64)x * (Word64)y;
#endif
    return LL_acc;
}


static inline Word32 clz(Word32 absx)
{
    Word32 lz;
#if defined(__GNUC__) && defined(__arm__)    
    asm( "clz %[lz], %[absx] \n" 
    : [lz]"=r"(lz)
    : [absx]"r"(absx));            
#else
    Word32 j = 1 << 31;
    for(lz=0; lz<32; lz++, j>>=1)
        {  if( absx&j )
                  break;
        }
#endif
    return lz;
}
*/
#ifdef __cplusplus
}
#endif

#endif /* SBC_ARMDSP_ARMDSP_H */


