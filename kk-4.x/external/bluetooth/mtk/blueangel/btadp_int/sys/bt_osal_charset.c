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

#include "string.h"

#include "bt_common.h"
#include "bt_osal.h"
#include "bt_adp_fs.h"
#include "bt_adp_fs_ucs2.h"

#define MMI_CHSET_DBG_ASSERT ASSERT

typedef enum
{
    CHSET_BASE,
    MMI_CHSET_BASE = CHSET_BASE,
    CHSET_ARABIC_ISO,
    MMI_CHSET_ARABIC_ISO = CHSET_ARABIC_ISO,
    CHSET_ARABIC_WIN,
    MMI_CHSET_ARABIC_WIN = CHSET_ARABIC_WIN,
    /* PMT START PERSIAN */
    CHSET_PERSIAN_CP1097,
    MMI_CHSET_PERSIAN_CP1097 = CHSET_PERSIAN_CP1097,
    CHSET_PERSIAN_CP1098,
    MMI_CHSET_PERSIAN_CP1098 = CHSET_PERSIAN_CP1098,
    /* PMT END PERSIAN */
    CHSET_BALTIC_ISO,
    MMI_CHSET_BALTIC_ISO = CHSET_BALTIC_ISO,
    CHSET_BALTIC_WIN,
    MMI_CHSET_BALTIC_WIN = CHSET_BALTIC_WIN,
    CHSET_CEURO_ISO,
    MMI_CHSET_CEURO_ISO = CHSET_CEURO_ISO,
    CHSET_CEURO_WIN,
    MMI_CHSET_CEURO_WIN = CHSET_CEURO_WIN,
    CHSET_CYRILLIC_ISO,
    MMI_CHSET_CYRILLIC_ISO = CHSET_CYRILLIC_ISO,    /* ISO 8859-5  */  
    CHSET_CYRILLIC_WIN,
    MMI_CHSET_CYRILLIC_WIN = CHSET_CYRILLIC_WIN,    /* Windows-1251  */
    CHSET_GREEK_ISO,
    MMI_CHSET_GREEK_ISO = CHSET_GREEK_ISO,
    CHSET_GREEK_WIN,
    MMI_CHSET_GREEK_WIN = CHSET_GREEK_WIN,
    CHSET_HEBREW_ISO,
    MMI_CHSET_HEBREW_ISO = CHSET_HEBREW_ISO,
    CHSET_HEBREW_WIN,
    MMI_CHSET_HEBREW_WIN = CHSET_HEBREW_WIN,
    CHSET_LATIN_ISO,
    MMI_CHSET_LATIN_ISO = CHSET_LATIN_ISO,
    CHSET_NORDIC_ISO,
    MMI_CHSET_NORDIC_ISO = CHSET_NORDIC_ISO,
    CHSET_SEURO_ISO,
    MMI_CHSET_SEURO_ISO = CHSET_SEURO_ISO,
    CHSET_TURKISH_ISO,
    MMI_CHSET_TURKISH_ISO = CHSET_TURKISH_ISO,
    CHSET_TURKISH_WIN,
    MMI_CHSET_TURKISH_WIN = CHSET_TURKISH_WIN,
    CHSET_WESTERN_ISO,
    MMI_CHSET_WESTERN_ISO = CHSET_WESTERN_ISO,
    CHSET_ARMENIAN_ISO,
    MMI_CHSET_ARMENIAN_ISO  = CHSET_ARMENIAN_ISO,
    CHSET_WESTERN_WIN,
    MMI_CHSET_WESTERN_WIN = CHSET_WESTERN_WIN,
    /* Add new (8-bit) encodings above this line */
    CHSET_BIG5,
    MMI_CHSET_BIG5 = CHSET_BIG5,
    CHSET_GB2312,
    MMI_CHSET_GB2312 = CHSET_GB2312,
    CHSET_HKSCS,
    MMI_CHSET_HKSCS = CHSET_HKSCS,    
    CHSET_SJIS,
    MMI_CHSET_SJIS = CHSET_SJIS,
    CHSET_GB18030,
    MMI_CHSET_GB18030 = CHSET_GB18030,
    CHSET_UTF7,
    MMI_CHSET_UTF7 = CHSET_UTF7,
    CHSET_EUCKR,
    MMI_CHSET_EUCKR = CHSET_EUCKR,
    CHSET_THAI_WIN,
    MMI_CHSET_THAI_WIN = CHSET_THAI_WIN,
    CHSET_VIETNAMESE_WIN,
    MMI_CHSET_VIETNAMESE_WIN = CHSET_VIETNAMESE_WIN,
    CHSET_KOI8_R,
    MMI_CHSET_KOI8_R = CHSET_KOI8_R,
    CHSET_TIS_620,
    MMI_CHSET_TIS_620 = CHSET_TIS_620,

    /* PLace all encodings using algo to above this one */
    CHSET_UTF16LE,
    MMI_CHSET_UTF16LE = CHSET_UTF16LE,
    CHSET_UTF16BE,
    MMI_CHSET_UTF16BE = CHSET_UTF16BE,
    CHSET_UTF8,
    MMI_CHSET_UTF8 = CHSET_UTF8,
    CHSET_UCS2,
    MMI_CHSET_UCS2 = CHSET_UCS2,
    CHSET_ASCII,
    MMI_CHSET_ASCII = CHSET_ASCII,
    CHSET_TOTAL,
    MMI_CHSET_TOTAL = CHSET_TOTAL 
} mmi_chset_enum, chset_enum;

/* do not change the order */
typedef enum _ENCODING_SCHEMES_ENUM
{
    BASE_ENCODING_SCHEME,

    ARABIC_ISO_TO_UCS2,
    UCS2_TO_ARABIC_ISO,

    ARABIC_WINDOWS_TO_UCS2,
    UCS2_TO_ARABIC_WINDOWS,
    /* PMT START PERSIAN */
    PERSIAN_CP1097_TO_UCS2,
    UCS2_TO_PERSIAN_CP1097,

    PERSIAN_CP1098_TO_UCS2,
    UCS2_TO_PERSIAN_CP1098,
    /* PMT END PERSIAN */
    BALTIC_ISO_TO_UCS2,
    UCS2_TO_BALTIC_ISO,

    BALTIC_WINDOWS_TO_UCS2,
    UCS2_TO_BALTIC_WINDOWS,

    CENTRAL_EUROPEAN_ISO_TO_UCS2,
    UCS2_TO_CENTRAL_EUROPEAN_ISO,

    CENTRAL_EUROPEAN_WINDOWS_TO_UCS2,
    UCS2_TO_CENTRAL_EUROPEAN_WINDOWS,

    CYRILLIC_ISO_TO_UCS2,
    UCS2_TO_CYRILLIC_ISO,

    CYRILLIC_WINDOWS_TO_UCS2,
    UCS2_TO_CYRILLIC_WINDOWS,
    
    GREEK_ISO_TO_UCS2,
    UCS2_TO_GREEK_ISO,

    GREEK_WINDOWS_TO_UCS2,
    UCS2_TO_GREEK_WINDOWS,

    HEBREW_ISO_TO_UCS2,
    UCS2_TO_HEBREW_ISO,

    HEBREW_WINDOWS_TO_UCS2,
    UCS2_TO_HEBREW_WINDOWS,

    LATIN_ISO_TO_UCS2,
    UCS2_TO_LATIN_ISO,

    NORDIC_ISO_TO_UCS2,
    UCS2_TO_NORDIC_ISO,

    SOUTH_EUROPEAN_ISO_TO_UCS2,
    UCS2_TO_SOUTH_EUROPEAN_ISO,

    TURKISH_ISO_TO_UCS2,
    UCS2_TO_TURKISH_ISO,

    TURKISH_WINDOWS_TO_UCS2,
    UCS2_TO_TURKISH_WINDOWS,

    WESTERN_ISO_TO_UCS2,
    UCS2_TO_WESTERN_ISO,

    ARMENIAN_ISO_TO_UCS2,
    UCS2_TO_ARMENIAN_ISO,
    
    WESTERN_WINDOWS_TO_UCS2,
    UCS2_TO_WESTERN_WINDOWS,

    /* Add new (8-bit) encodings above this line */
    BIG5_TO_UCS2,
    UCS2_TO_BIG5,

    GB2312_TO_UCS2,
    UCS2_TO_GB2312,

    GB18030_TO_UCS2,
    UCS2_TO_GB18030,

    UTF7_TO_UCS2,
    UCS2_TO_UTF7,

    HKSCS_TO_UCS2,
    UCS2_TO_HKSCS,

    SJIS_TO_UCS2,
    UCS2_TO_SJIS,

    EUCKR_WINDOWS_TO_UCS2,
    UCS2_TO_EUCKR_WINDOWS,

    THAI_WINDOWS_TO_UCS2,
    UCS2_TO_THAI_WINDOWS,

    VIETNAMESE_WINDOWS_TO_UCS2,
    UCS2_TO_VIETNAMESE_WINDOWS,

    KOI8_R_TO_UCS2,
    UCS2_TO_KOI8_R,

    TIS_620_TO_UCS2,
    UCS2_TO_TIS_620,

    UTF16LE_TO_UCS2,
    UCS2_TO_UTF16LE,
    
    UTF16BE_TO_UCS2,
    UCS2_TO_UTF16BE,
    /* PLace all encodings using algo to above this one */

    UTF8_TO_UCS2,
    UCS2_TO_UTF8,

    ASCII_TO_UCS2,
    UCS2_TO_ASCII,
    
    CHSET_PAIR_TOTAL
} mmi_chset_pair_enum;

#define CHSET_BIT_WORD_16           16
#define CHSET_BIT_WORD_8            8

#define MERGE_TO_WCHAR(high, low)  (high << 8) + ((kal_uint8)low)
#define UTF16_HIGH_START      0xD800
#define UTF16_HIGH_END        0xDBFF
#define UTF16_LOW_START       0xDC00
#define UTF16_LOW_END         0xDFFF

#define CHSET_GUESS_CHAR_NONE           0
#define CHSET_GUESS_CHAR_ONE_BYTE       1
#define CHSET_GUESS_CHAR_TWO_BYTE       2
#define CHSET_GUESS_CHAR_THREE_BYTE     3
#define CHSET_GUESS_USE_FREQ_COEF       2
#define CHSET_GUESS_USE_SELDOM_COEF     1
#define CHSET_GUESS_USE_RARE_COEF       0
#define CHSET_GUESS_IS_UTF8_THRESHOLD   0.9

/* Supported Guess Charsets */
#define CHSET_GUESS_ALL				0x00000000
#define CHSET_GUESS_UTF8			0x00000001
#define CHSET_GUESS_UTF16LE			0x00000002
#define CHSET_GUESS_UTF16BE			0x00000004
#define CHSET_GUESS_UCS2			0x00000008
#define CHSET_GUESS_GB2312			0x00000010
#define CHSET_GUESS_BIG5			0x00000020
#define CHSET_GUESS_WESTERN_ISO		0x00000040
#define CHSET_GUESS_HKSCS                0x00000080

#ifndef CAST_PU8
#define CAST_PU8(RAW)   ((kal_uint8*)(RAW))
#endif
#ifndef CAST_CPU8
#define CAST_CPU8(RAW)  ((const kal_uint8*)(RAW))
#endif
#ifndef GET_U8_BYTE_TO_U16
#define GET_U8_BYTE_TO_U16(RAW, i)  ((kal_uint16)(CAST_CPU8(RAW)[i]))
#endif

#ifndef STR_SIZE
#define STR_SIZE(len) ((len)<<1)
#endif
#ifndef STR_AT
#define STR_AT(RAW, n) ((void*)(CAST_PU8(RAW)+STR_SIZE(n)))
#endif
#ifndef CHR_AT
#define CHR_AT(RAW, n) ((kal_uint16)(GET_U8_BYTE_TO_U16(RAW, STR_SIZE(n))+(GET_U8_BYTE_TO_U16(RAW, STR_SIZE(n)+1)<<8)))
#endif
#ifndef SET_CHR_AT
#define SET_CHR_AT(RAW, n, value)   do {                          \
                              kal_uint8 *_p = STR_AT(RAW, n);     \
                              kal_uint16 v= (kal_uint16) (value); \
                              _p[0] = (kal_uint8) (v & 0xff);     \
                              _p[1] = (kal_uint8) (v >> 8);       \
                           } while (0)
#endif

typedef struct
{
    unsigned short start_value;
    unsigned short end_value;
    unsigned short index;
} Mapping_Struct;

typedef struct
{
    const Mapping_Struct *pMappingTable;
    const kal_uint16 *pConversnTable;
    kal_uint16 TotalMapTbIndex;
    kal_uint8 Input_word_bits;
    kal_uint8 Output_word_bits;
    kal_uint16 UndefChar;   /* currently taken undefchar as quesmark */
} mmi_chset_info_struct;

typedef struct
{
    mmi_chset_enum chset;          /* charset enum */
    kal_uint32 mask;               /* charset mask value*/
    kal_uint32 prob;               /* The probability value */
    kal_uint8 (*check) (const kal_uint8*, kal_uint8*, kal_int32);  /* check function */
    kal_uint8 max_byte;            /* Max byte checked at one time */
} mmi_chset_guess_struct;

static mmi_chset_info_struct g_chset_tbl[CHSET_PAIR_TOTAL];

static const kal_uint8 g_chset_state[] = 
{
    0,

#ifdef __MMI_CHSET_ARABIC_ISO__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_ARABIC_WIN__
    1,
#else 
    0,
#endif 

    /* PMT START PERSIAN */
#ifdef __MMI_CHSET_PERSIAN_CP1097__
    1,
#else 
    0,
#endif 
#ifdef __MMI_CHSET_PERSIAN_CP1098__
    1,
#else 
    0,
#endif 
    /* PMT END PERSIAN */
#ifdef __MMI_CHSET_BALTIC_ISO__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_BALTIC_WIN__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_CEURO_ISO__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_CEURO_WIN__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_GREEK_ISO__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_CYRILLIC_ISO__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_CYRILLIC_WIN__
    1,
#else 
    0,
#endif 
#ifdef __MMI_CHSET_GREEK_WIN__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_HEBREW_ISO__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_HEBREW_WIN__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_LATIN_ISO__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_NORDIC_ISO__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_SEURO_ISO__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_TURKISH_ISO__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_TURKISH_WIN__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_WESTERN_ISO__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_ARMENIAN_ISO__
        1,
#else 
        0,
#endif

#ifdef __MMI_CHSET_WESTERN_WIN__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_BIG5__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_GB2312__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_HKSCS__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_SJIS__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_GB18030__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_UTF7__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_EUCKR_WIN__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_THAI_WINDOWS__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_VIETNAMESE_WINDOWS__
    1,
#else 
    0,
#endif 

#ifdef __MMI_CHSET_KOI8_R__
    1,
#else
    0,
#endif

#ifdef __MMI_CHSET_TIS_620__
    1,
#else
    0,
#endif

    /* PLace all encodings using algo to above this one */
    1,  /* MMI_CHSET_UTF16LE, */
    1,  /* MMI_CHSET_UTF16BE, */
    1,  /* MMI_CHSET_UTF8, */
    1,  /* MMI_CHSET_UCS2, */
    1,  /* MMI_CHSET_ASCII */
    1   /* MMI_CHSET_TOTAL */
};

static const kal_uint8 g_cheset_utf8_bytes_per_char[16] = 
{
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 2, 2, 3, 4  /* we don't support UCS4 */
};

static kal_uint8 mmi_chset_guess_is_utf8_bytes(const kal_uint8 *buff, kal_int32 left_size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint16 ucs2 = 0xFFFF;    /* invalid character */
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (*buff < 0x7F)
    {
        return CHSET_GUESS_CHAR_ONE_BYTE;
    }
    else if ((*buff & 0xF0) == 0xE0)
    {
        if (left_size < CHSET_GUESS_CHAR_TWO_BYTE)
        {
            return CHSET_GUESS_CHAR_NONE;
        }
        else if (((*(buff + 1) & 0xC0) == 0x80) && ((*(buff + 2) & 0xC0) == 0x80))
        {
            ucs2 = ((kal_uint16)(buff[0] & 0x0f) << 12) |
                    ((kal_uint16)(buff[1] ^ 0x80) << 6) |
                     (kal_uint16)(buff[2] ^ 0x80);
            if (ucs2 >= 0x0800/* && ucs2 <= 0xFFFF */)
            {
                return CHSET_GUESS_CHAR_THREE_BYTE;
            }
        }
    }
    else if ((*buff & 0xE0) == 0xC0)
    {
        if (left_size < CHSET_GUESS_CHAR_ONE_BYTE)
        {
            return CHSET_GUESS_CHAR_NONE;
        }
        else if (((*(buff + 1) & 0xC0) == 0x80))
        {
            ucs2 = ((kal_uint16)(buff[0] & 0x1f) << 6) | (kal_uint16)(buff[1] ^ 0x80);
            if (ucs2 >= 0x0080 && ucs2 <= 0x07FF)
            {
                return CHSET_GUESS_CHAR_TWO_BYTE;
            }   
        }
    }
    return CHSET_GUESS_CHAR_NONE;
}

static kal_bool mmi_chset_guess_is_utf8(const kal_uint8 *buff, kal_int32 size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int32 i = 0;
    kal_uint8 ret = 0;
    kal_uint32 byte_num = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if((buff[0] == 0xEF) && (buff[1] == 0xBB) && (buff[2] == 0xBF))
    {
        return KAL_TRUE;
    }
    
    while (i <= size - CHSET_GUESS_CHAR_ONE_BYTE)
    {
        ret = mmi_chset_guess_is_utf8_bytes(&buff[i], (size - CHSET_GUESS_CHAR_ONE_BYTE - i));
        if (ret)
        {
            i += ret;
            byte_num += ret;
        }
        else
        {
            i++;
        }
    }
    
    /* Return if UTF8 encode byte num reach the threshold */
    if (byte_num > CHSET_GUESS_IS_UTF8_THRESHOLD * size)
    {
        return KAL_TRUE;
    }
    else
    {
        return KAL_FALSE;
    }
}

static kal_uint8 mmi_chset_guess_is_gb2312_bytes(const kal_uint8 *buff, kal_uint8 *coef, kal_int32 left_size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (*buff < 0x7F)
    {
        *coef = CHSET_GUESS_USE_FREQ_COEF;
        return CHSET_GUESS_CHAR_ONE_BYTE;
    }
    else if (left_size < CHSET_GUESS_CHAR_ONE_BYTE)
    {
        return CHSET_GUESS_CHAR_NONE;
    }
    else if ((*buff >= 0xA1 && *buff <= 0xFE) && 
             (*(buff + 1) >= 0xA1 && *(buff + 1) <= 0xFE))
    {
        if ((*buff >= 0xA1 && *buff <= 0xA9) || (*buff >= 0xB0 && *buff <= 0xCF))
        {
            *coef = CHSET_GUESS_USE_FREQ_COEF;
            return CHSET_GUESS_CHAR_TWO_BYTE;
        }
        else if (*buff >= 0xD0 && *buff <= 0xF7)
        {
            *coef = CHSET_GUESS_USE_SELDOM_COEF;
            return CHSET_GUESS_CHAR_TWO_BYTE;
        }
        else
        {
            *coef = CHSET_GUESS_USE_RARE_COEF;
            return CHSET_GUESS_CHAR_TWO_BYTE;
        }
    }
    else
    {
        return CHSET_GUESS_CHAR_NONE;
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_chset_guess_is_big5_bytes
 * DESCRIPTION
 *  Check if the given buff is big5 character.
 *  BIG5 range:
 *    LSB:  0x40-0x7E, 0xA1-0xFE
 *    MSB:  0xA1-0xA3, 0xC6-0xC8 (Symbol)
 *          0xA4-0xC5            (L1 Chinese character)
 *          0xC9-0xF9            (L2 Chinese character)
 * PARAMETERS
 *  buff                   [IN]        String buffer need to check
 *  coef                   [OUT]       The coefficient of character used frequency
 *  left_size              [IN]        Left byte number not been guessed in the String buffer
 * RETURNS
 *  character bytes checked
 *****************************************************************************/
static kal_uint8 mmi_chset_guess_is_big5_bytes(const kal_uint8 *buff, kal_uint8 *coef, kal_int32 left_size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (*buff < 0x7F)
    {
        *coef = CHSET_GUESS_USE_FREQ_COEF;
        return CHSET_GUESS_CHAR_ONE_BYTE;
    }
    else if (left_size < CHSET_GUESS_CHAR_ONE_BYTE)
    {
        return CHSET_GUESS_CHAR_NONE;
    }
    else if (((*(buff + 1) >= 0x40 && *(buff + 1) <= 0x7E) ||
              (*(buff + 1) >= 0xA1 && *(buff + 1) <= 0xFE)) &&
              (*buff >= 0xA1 && *buff <= 0xFE))
    {
        if (*buff >= 0xA1 && *buff <= 0xC8)
        {
            *coef = CHSET_GUESS_USE_FREQ_COEF;
            return CHSET_GUESS_CHAR_TWO_BYTE;
        }
        else if (*buff >= 0xC9 && *buff <= 0xF9)
        {
            *coef = CHSET_GUESS_USE_SELDOM_COEF;
            return CHSET_GUESS_CHAR_TWO_BYTE;
        }
        else
        {
            *coef = CHSET_GUESS_USE_RARE_COEF;
            return CHSET_GUESS_CHAR_TWO_BYTE;
        }
    }
    else
    {
        return CHSET_GUESS_CHAR_NONE;
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_chset_guess_is_hkscs_bytes
 * DESCRIPTION
 *  Check if the given buff is hkscs character.
 *  HKSCS range:  --> to be filled
 *    LSB:  
 *    MSB:  
 * PARAMETERS
 *  buff                   [IN]        String buffer need to check
 *  coef                   [OUT]       The coefficient of character used frequency
 *  left_size              [IN]        Left byte number not been guessed in the String buffer
 * RETURNS
 *  character bytes checked
 *****************************************************************************/
static kal_uint8 mmi_chset_guess_is_hkscs_bytes(const kal_uint8 *buff, kal_uint8 *coef, kal_int32 left_size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (*buff < 0x7F)
    {
        *coef = CHSET_GUESS_USE_FREQ_COEF;
        return CHSET_GUESS_CHAR_ONE_BYTE;
    }
    else if (left_size < CHSET_GUESS_CHAR_ONE_BYTE)
    {
        return CHSET_GUESS_CHAR_NONE;
    }
    else if (((*(buff + 1) >= 0x40 && *(buff + 1) <= 0x7E) ||
              (*(buff + 1) >= 0xA1 && *(buff + 1) <= 0xFE)) &&
              (*buff >= 0x81 && *buff <= 0xFE))
    {
        if (*buff >= 0x81 && *buff <= 0xC8)
        {
            *coef = CHSET_GUESS_USE_FREQ_COEF;
            return CHSET_GUESS_CHAR_TWO_BYTE;
        }
        else if (*buff >= 0xC9 && *buff <= 0xF9)
        {
            *coef = CHSET_GUESS_USE_SELDOM_COEF;
            return CHSET_GUESS_CHAR_TWO_BYTE;
        }
        else
        {
            *coef = CHSET_GUESS_USE_RARE_COEF;
            return CHSET_GUESS_CHAR_TWO_BYTE;
        }
    }
    else
    {
        return CHSET_GUESS_CHAR_NONE;
    }
}



/*****************************************************************************
 * FUNCTION
 *  mmi_chset_guess_is_western_iso_bytes
 * DESCRIPTION
 *  Check if the given buff is ISO8859-1 character.
 *  Frequent used: 0x20-0x7E
 *  Seldom used:   0xA0-0xFF
 * PARAMETERS
 *  buff                   [IN]        String buffer need to check
 *  coef                   [OUT]       The coefficient of character used frequency
 *  left_size              [IN]        Left byte number not been guessed in the String buffer
 * RETURNS
 *  character bytes checked
 *****************************************************************************/
static kal_uint8 mmi_chset_guess_is_western_iso_bytes(const kal_uint8 *buff, kal_uint8 *coef, kal_int32 left_size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (*buff >= 0x20 && *buff <= 0x7E)
    {
        *coef = CHSET_GUESS_USE_FREQ_COEF;
        return CHSET_GUESS_CHAR_ONE_BYTE;
    }
    else if (*buff >= 0xA0 /* && *buff <= 0xFF */)
    {
        *coef = CHSET_GUESS_USE_SELDOM_COEF;
        return CHSET_GUESS_CHAR_ONE_BYTE;
    }
    else
    {
        return CHSET_GUESS_CHAR_NONE;
    }
}

static mmi_chset_enum mmi_chset_guess(
                   const char *buff,
                   kal_int32 guess_size,
                   mmi_chset_enum preferred_chset,
                   kal_uint32 suspect_chset_mask)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    const kal_uint8 *string_buff = (kal_uint8*)buff;
    kal_int32 i = 0;
    kal_uint8 chset_cnt = 0;
    kal_uint8 chset_num = 0;
    kal_uint8 prefer_chset_idx = 0;
    kal_uint8 default_chset_idx = 0;
    kal_uint8 ret = 0;
    kal_uint8 coef = 0;
    kal_uint32 pro = 0;
    kal_uint32 max_pro = 0;
    kal_bool is_conflict = KAL_FALSE;
    
    mmi_chset_enum g_chset_text_encoding = MMI_CHSET_UCS2; /* zgyu */
    
    mmi_chset_enum ret_chset = g_chset_text_encoding;    
    mmi_chset_guess_struct chset_guess_charset[] = 
    {
        {MMI_CHSET_WESTERN_ISO, CHSET_GUESS_WESTERN_ISO, 0, mmi_chset_guess_is_western_iso_bytes, CHSET_GUESS_CHAR_ONE_BYTE},
        {MMI_CHSET_BIG5, CHSET_GUESS_BIG5, 0, mmi_chset_guess_is_big5_bytes, CHSET_GUESS_CHAR_TWO_BYTE},
        {MMI_CHSET_HKSCS, CHSET_GUESS_HKSCS, 0, mmi_chset_guess_is_hkscs_bytes, CHSET_GUESS_CHAR_TWO_BYTE},
        {MMI_CHSET_GB2312, CHSET_GUESS_GB2312, 0, mmi_chset_guess_is_gb2312_bytes, CHSET_GUESS_CHAR_TWO_BYTE}
    };

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Only check BOM */
    if ( ((suspect_chset_mask == CHSET_GUESS_ALL) ||
        (suspect_chset_mask & CHSET_GUESS_UTF16LE)) &&
        g_chset_state[MMI_CHSET_UTF16LE] )
    {
        if (string_buff[0] == 0xFF && string_buff[1] == 0xFE)
        {
            return MMI_CHSET_UTF16LE;
        }
    }

    if ( ((suspect_chset_mask == CHSET_GUESS_ALL) ||
        (suspect_chset_mask & CHSET_GUESS_UTF16BE))&&
        g_chset_state[MMI_CHSET_UTF16BE] )
    {
        if (string_buff[0] == 0xFE && string_buff[1] == 0xFF)
        {
            return MMI_CHSET_UTF16BE;
        }
    }

    /* Check UTF8 */
    if (((suspect_chset_mask == CHSET_GUESS_ALL) ||
        (suspect_chset_mask & CHSET_GUESS_UTF8))&&
        g_chset_state[MMI_CHSET_UTF8] )
    {
        if (mmi_chset_guess_is_utf8(string_buff, guess_size) == KAL_TRUE)
        {
            return MMI_CHSET_UTF8;
        }
    }

    /* Try the charset in suspect list. if suspect list is CHSET_GUESS_ALL, try all supported charset */
    chset_num = sizeof(chset_guess_charset) / sizeof(mmi_chset_guess_struct);
    for (chset_cnt = 0; chset_cnt < chset_num; chset_cnt++)
    {
        if (((suspect_chset_mask == CHSET_GUESS_ALL) ||
            (suspect_chset_mask & chset_guess_charset[chset_cnt].mask))&&
            g_chset_state[chset_guess_charset[chset_cnt].chset] )
        {
            i = 0;
            pro = 0;
            string_buff = (kal_uint8*)buff;

            if (chset_guess_charset[chset_cnt].chset == preferred_chset)
            {
                prefer_chset_idx = chset_cnt + 1;
            }            
            else if (chset_guess_charset[chset_cnt].chset == g_chset_text_encoding)
            {
                default_chset_idx = chset_cnt + 1;
            }

            /* compute the probability of this charset */
            while (i <= guess_size - CHSET_GUESS_CHAR_ONE_BYTE)
            {
                ret = chset_guess_charset[chset_cnt].check(&string_buff[i], &coef, (guess_size - CHSET_GUESS_CHAR_ONE_BYTE - i));
                if (ret)
                {
                    i += ret;
                    pro += (ret * coef);
                }
                else
                {
                    i++;
                }
            }

            /* check if two or more charsets have the max probability */
            if (pro > max_pro)
            {
                is_conflict = KAL_FALSE;
                max_pro = pro;
                ret_chset = chset_guess_charset[chset_cnt].chset;
            }
            else if (pro == max_pro)
            {
                is_conflict = KAL_TRUE;
                max_pro = pro;
                ret_chset = chset_guess_charset[chset_cnt].chset;
            }
            chset_guess_charset[chset_cnt].prob = pro;
        }
    }

    /* If more than one charsets have the max probability */
    if (is_conflict)
    {
        /* Return preferred_charset if it has the max probability */
        if (prefer_chset_idx > 0 &&
            chset_guess_charset[prefer_chset_idx - 1].prob == max_pro)
        {
            return preferred_chset;
        }
        
        /* Return default encoding type of cell phone if it has the max probability */
        if (default_chset_idx > 0 &&
            chset_guess_charset[default_chset_idx - 1].prob == max_pro)
        {
            return g_chset_text_encoding;
        }
    }
    
    /* Special case handling */
    /* Add special case handling here */

    /* return max probability charset */
    return ret_chset;
}

static __inline int mmi_chset_ucs2_to_utf8_len(kal_uint16 ucs2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ucs2 < (kal_uint16) 0x80)
    {
        return 1;
    }
    else if (ucs2 < (kal_uint16) 0x800)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

S32 btmtk_chset_utf8_to_ucs2(kal_uint8 *dest, kal_uint8 *utf8)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8 c = utf8[0];
    int count = g_cheset_utf8_bytes_per_char[c >> 4];
    kal_uint16 ucs2 = 0xFFFF;   /* invalid character */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (count)
    {
        case 1:
            ucs2 = (kal_uint16) c;
            break;
        case 2:
            if (utf8[1])
            {
                ucs2 = ((kal_uint16) (c & 0x1f) << 6) | (kal_uint16) (utf8[1] ^ 0x80);
            }
            break;
        case 3:
            if (utf8[1] && utf8[2])
            {
                ucs2 = ((kal_uint16) (c & 0x0f) << 12)
                    | ((kal_uint16) (utf8[1] ^ 0x80) << 6) | (kal_uint16) (utf8[2] ^ 0x80);
            }
            break;
        case 4:
            break;
        default:
            count = 1;   /* the character cannot be converted, return 1 means always consume 1 byte */
            break;
    }

    SET_CHR_AT(dest, 0, ucs2);

    return count;
}

kal_int32 btmtk_chset_utf8_to_ucs2_string_ex(
            kal_uint8 *dest,
            kal_int32 dest_size,
            kal_uint8 *src,
            kal_uint32 *src_end_pos)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int pos = 0;
    int cnt;
    int src_len = strlen((const char*)src);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef MMI_CHSET_UTF8_ALLOW_BOM
    if (src_len >= 3 && mmi_chset_is_utf8_BOM(src))
    {
        src += 3;
        src_len -= 3;
    }
#endif /* MMI_CHSET_UTF8_ALLOW_BOM */ 

    MMI_CHSET_DBG_ASSERT((dest_size % 2) == 0);
    dest_size -= (dest_size % 2);
    *src_end_pos = (kal_uint32) src; /* set src_end_pos first */

    if (dest_size < 2)
    {
        MMI_CHSET_DBG_ASSERT(0);    /* dest wont be NULL-terminated */
        return 0;
    }

    while (*src && pos < dest_size - 2)
    {
        cnt = btmtk_chset_utf8_to_ucs2(dest + pos, src);
         
        if (
             (kal_uint32)(src - (*src_end_pos)) >= (kal_uint32)(src_len - cnt) &&
             (*(dest + pos) == 0xFF && *(dest + pos + 1) == 0xFF) &&
            !(*src == 0xEF && *(src+1) == 0xBF && *(src+2) == 0xBF)
            )
        {
            /* 
             * If src isn't 0xEF, 0xBF, 0xBF and its remain length is not enough but dest is 0xFFFF, we will abort the process.
             * dest data is invalid character because src data is not enough to convert 
             */
            break;
        }
        if (cnt == 0)   /* abnormal case */
        {
        #ifdef MMI_CHSET_UTF8_FAULT_TOLERANT
            src++;
        #else 
            break;
        #endif 
        }        
        else    /* normal case */
        {
            src += cnt;
            pos += 2;
        }
    }
    *src_end_pos = (kal_uint32) src;
    dest[pos] = 0;
    dest[pos + 1] = 0;
    return pos + 2;
}


static kal_uint16 EncodingDecoding(
            mmi_chset_pair_enum Scheme,
            char *pOutBuffer,
            char *pInBuffer,
            kal_int32 dest_size,
            kal_uint32 *src_end_pos)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (Scheme == UTF8_TO_UCS2)
    {
        return (kal_uint16) btmtk_chset_utf8_to_ucs2_string_ex(
                                (kal_uint8*) pOutBuffer,
                                dest_size,
                                (kal_uint8*) pInBuffer,
                                src_end_pos);
    }
    else if (Scheme == UCS2_TO_UTF8)
    {
        return (kal_uint16) btmtk_chset_ucs2_to_utf8_string_ex(
                                (kal_uint8*) pOutBuffer,
                                dest_size,
                                (kal_uint8*) pInBuffer,
                                src_end_pos);
    }
#ifdef __MMI_CHSET_BIG5__
    else if (BIG5_TO_UCS2 <= Scheme && Scheme <= UCS2_TO_BIG5)  /* Patch added to use different algorithm for chinese encodings */
    {
        return mmi_chset_encode_decode_algo2(Scheme, pOutBuffer, pInBuffer, dest_size, src_end_pos);    /* Length is hardcode for now to maximum */
    }
#endif /* __MMI_CHSET_BIG5__ */ 
#ifdef __MMI_CHSET_GB2312__
    else if (GB2312_TO_UCS2 <= Scheme && Scheme <= UCS2_TO_GB2312)      /* Patch added to use different algorithm for chinese encodings */
    {
        return mmi_chset_encode_decode_algo2(Scheme, pOutBuffer, pInBuffer, dest_size, src_end_pos);    /* Length is hardcode for now to maximum */
    }
#endif /* __MMI_CHSET_GB2312__ */ 
#ifdef __MMI_CHSET_GB18030__
    else if (GB18030_TO_UCS2 == Scheme)
    {
        return mmi_chset_gb18030_to_ucs2_str(pOutBuffer, pInBuffer, dest_size, src_end_pos);
    }
    else if (Scheme == UCS2_TO_GB18030)
    {
        return mmi_chset_ucs2_to_gb18030_str(pOutBuffer, pInBuffer, dest_size, src_end_pos);
    }
#endif /* __MMI_CHSET_GB18030__ */ 
#ifdef __MMI_CHSET_HKSCS__
    else if (HKSCS_TO_UCS2 <= Scheme && Scheme <= UCS2_TO_HKSCS)  /* Patch added to use different algorithm for chinese encodings */
    {
        return mmi_chset_encode_decode_algo2(Scheme, pOutBuffer, pInBuffer, dest_size, src_end_pos);    /* Length is hardcode for now to maximum */
    }
#endif /* __MMI_CHSET_HKSCS__ */ 

#ifdef __MMI_CHSET_EUCKR_WIN__
    else if (Scheme == EUCKR_WINDOWS_TO_UCS2)
    {
        return mmi_chset_encode_decode_algo3(Scheme, pOutBuffer, pInBuffer, dest_size, src_end_pos);
    }
#endif /* __MMI_CHSET_EUCKR_WIN__ */ 
#ifdef __MMI_CHSET_EUCKR_WIN__
    else if (Scheme == UCS2_TO_EUCKR_WINDOWS)
    {
        return mmi_chset_encode_decode_algo3(Scheme, pOutBuffer, pInBuffer, dest_size, src_end_pos);
    }
#endif /* __MMI_CHSET_EUCKR_WIN__ */ 

#ifdef __MMI_CHSET_ALPHABET_SUPPORT__
    else    /* For all of the rest encodings. */
    {
        return mmi_chset_encode_decode(Scheme, pOutBuffer, pInBuffer, dest_size, src_end_pos);
    }
#else /* __MMI_CHSET_ALPHABET_SUPPORT__ */ 
    else
    {
        return 0;
    }
#endif /* __MMI_CHSET_ALPHABET_SUPPORT__ */ 
}
#ifdef __LINUX_SUPPRESS_ERROR__
S32 mmi_chset_ucs2_to_utf8(kal_uint8 *utf8, kal_uint16 ucs2)
#else
static S32 mmi_chset_ucs2_to_utf8(kal_uint8 *utf8, kal_uint16 ucs2)
#endif
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int count;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ucs2 < (kal_uint16) 0x80)
    {
        count = 1;
    }
    else if (ucs2 < (kal_uint16) 0x800)
    {
        count = 2;
    }
    else
    {
        count = 3;
    }
    switch (count)
    {
        case 3:
            utf8[2] = 0x80 | (ucs2 & 0x3f);
            ucs2 = ucs2 >> 6;
            ucs2 |= 0x800;
        case 2:
            utf8[1] = 0x80 | (ucs2 & 0x3f);
            ucs2 = ucs2 >> 6;
            ucs2 |= 0xc0;
        case 1:
            utf8[0] = (kal_uint8) ucs2;
    }
    return count;
}

S32 btmtk_chset_ucs2_to_utf8_string_ex(U8*dest, S32 dest_size, U8* src, U32* src_end_pos)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int pos = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (src[0] || src[1])
    {
        if (pos + mmi_chset_ucs2_to_utf8_len(CHR_AT(src, 0)) >= dest_size)      /* leave space of '\0' */
        {
            MMI_CHSET_DBG_ASSERT(0);
            break;
        }

        pos += mmi_chset_ucs2_to_utf8(dest + pos, CHR_AT(src, 0));
        src += 2;
        if (pos >= dest_size - 1)
        {
            break;
        }
    }
    if (pos >= dest_size)
    {
        MMI_CHSET_DBG_ASSERT(0);
        dest[dest_size - 1] = 0;
    }
    else
    {
        dest[pos] = 0;
    }
    *src_end_pos = (kal_uint32) src;
    return pos + 1;
}

static void mmi_chset_simple_convert(
			 kal_int8 *dest,
			 kal_int32 dest_size,
			 kal_int8 *src,
                         kal_uint32 *src_end_pos)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    while ((*src != 0) && (dest_size > 2))
    {
        if (*src < 0x7F)
        {
	    *dest++ = *src++;
	    *dest++ = 0x00;
	}
	else
	{
	    *dest++ = 0x3F;
	    *dest++ = 0x00;
	}
        
        dest_size -= 2;
    }

    *src_end_pos = (kal_uint32) src;
    
    *dest++ = 0x00;
    *dest = 0x00;

}

static kal_int8 mmi_chset_get_utf_byte_order(const kal_int8 *str_bom)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_wchar wc;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    wc = MERGE_TO_WCHAR(str_bom[0], str_bom[1]);

    switch (wc)
    {
        case 0xFFFE:
            return MMI_CHSET_UTF16LE;

        case 0xFEFF:
            return MMI_CHSET_UTF16BE;

        case 0xEFBB:
            if (0xBF == str_bom[2])
                return MMI_CHSET_UTF8;
            else
                return -1;

        default:
            return -1;
    }
}

static kal_int32 mmi_chset_utf16_to_ucs2_string(
                    kal_int8 *dest_ucs2,
                    kal_int32 dest_size,
                    kal_int8 *src_utf16,
                    mmi_chset_enum utf16_type,
                    kal_uint32 *src_end_pos)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int8 utf16_bo;
    kal_int32 count = 0;
    kal_wchar wc1;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(dest_ucs2 && src_utf16 && src_end_pos);
    ASSERT((MMI_CHSET_UTF16BE == utf16_type) || (MMI_CHSET_UTF16LE == utf16_type));

    utf16_bo = mmi_chset_get_utf_byte_order(src_utf16);

    if ((kal_int8)(-1) != utf16_bo)
    {
        src_utf16 += 2;
        /* needn't convert the beginning BOM */
    }
    else
    {
        utf16_bo = utf16_type;
    }

    while ((src_utf16[0] || src_utf16[1]) && (count < (dest_size - 2)))
    {
        wc1 = ((MMI_CHSET_UTF16BE == utf16_bo) ?
               MERGE_TO_WCHAR(src_utf16[0], src_utf16[1]) : MERGE_TO_WCHAR(src_utf16[1], src_utf16[0]));

        /* we ignore surrogate pair */
        if (((wc1 >= UTF16_HIGH_START) && (wc1 <= UTF16_HIGH_END)) ||
            ((wc1 >= UTF16_LOW_START) && (wc1 <= UTF16_LOW_END)))
        {
            src_utf16 += 2;
        }
        else
        {
            /* normal case */
            dest_ucs2[0] = wc1 & 0xFF;
            dest_ucs2[1] = wc1 >> 8;
            src_utf16 += 2;
            dest_ucs2 += 2;
            count += 2;
        }
    }

    dest_ucs2[0] = '\0';
    dest_ucs2[1] = '\0';

    *src_end_pos = (kal_uint32) src_utf16;
    return (count + 2);

}

static kal_int32 mmi_chset_ucs2_to_utf16_string(
                    kal_int8 *dest_utf16,
                    kal_int32 dest_size,
                    mmi_chset_enum utf16_type,
                    kal_bool include_bom,
                    kal_int8 *src_ucs2,
                    kal_uint32 *src_end_pos)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int32 count = 0;
    kal_wchar wc;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(dest_utf16 && src_ucs2 && src_end_pos);
    ASSERT((MMI_CHSET_UTF16BE == utf16_type) || (MMI_CHSET_UTF16LE == utf16_type));

    if (include_bom)
    {
        if (MMI_CHSET_UTF16BE == utf16_type)
        {
            dest_utf16[0] = (kal_int8) 0xFF;
            dest_utf16[1] = (kal_int8) 0xFE;
        }
        else
        {
            dest_utf16[0] = (kal_int8) 0xFE;
            dest_utf16[1] = (kal_int8) 0xFF;
        }
        dest_utf16 += 2;
        dest_size -= 2;
        count += 2;
    }

    while ((src_ucs2[0] || src_ucs2[1]) && (count < (dest_size - 2)))
    {
        wc = MERGE_TO_WCHAR(src_ucs2[1], src_ucs2[0]);

        /* UTF-16 surrogate values are illegal in UCS2; 
           0xffff or 0xfffe are both reserved values */
        if (!(wc >= UTF16_HIGH_START && wc <= UTF16_LOW_END))
        {
            if (MMI_CHSET_UTF16LE == utf16_type)
            {
                dest_utf16[0] = src_ucs2[0];
                dest_utf16[1] = src_ucs2[1];
            }
            else
            {
                dest_utf16[0] = src_ucs2[1];
                dest_utf16[1] = src_ucs2[0];
            }
            dest_utf16 += 2;
            count += 2;
        }
        src_ucs2 += 2;
    }

    dest_utf16[0] = '\0';
    dest_utf16[1] = '\0';

    *src_end_pos = (kal_uint32) src_ucs2;
    return (count + 2);

}

static kal_int32 mmi_chset_copy_to_dest(
            mmi_chset_enum src_type,
            char *src_buff,
            char *dest_buff,
            kal_int32 dest_size,
            kal_uint32 *src_end_pos)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8 WordBytes = 0;
    mmi_chset_pair_enum Scheme;
    kal_int32 count = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (src_type == MMI_CHSET_UCS2)
    {
        WordBytes = CHSET_BIT_WORD_16;
    }
    else
    {
        Scheme = (mmi_chset_pair_enum) (BASE_ENCODING_SCHEME + (src_type - MMI_CHSET_BASE) * 2 - 1);
        WordBytes = g_chset_tbl[Scheme].Input_word_bits;
    }

    if (WordBytes == CHSET_BIT_WORD_16)
    {
        for (count = 0; src_buff[count] != 0 || src_buff[count + 1] != 0; count = count + 2)
        {
            if (count >= dest_size - 2)
            {
                count = (kal_uint16) dest_size - 2;
                break;
            }
            dest_buff[count] = src_buff[count];
            dest_buff[count + 1] = src_buff[count + 1];
        }
        *src_end_pos = (kal_uint32) (src_buff + count);
        dest_buff[count] = 0;
        dest_buff[++count] = 0;
        return count + 1;
    }
    else if(WordBytes == CHSET_BIT_WORD_8)
    {
        for (count = 0; src_buff[count] != 0; count++)
        {
            if (count >= dest_size - 1)
            {
                count = (kal_uint16) dest_size - 1;
                break;
            }
            dest_buff[count] = src_buff[count];
        }
        *src_end_pos = (kal_uint32) (src_buff + count);
        dest_buff[count] = 0;
        return count + 1;
    }
    else
    {
        return 0;
    }
}

static kal_int32 mmi_chset_convert_ex(
            mmi_chset_enum src_type,
            mmi_chset_enum dest_type,
            char *src_buff,
            char *dest_buff,
            kal_int32 dest_size,
            kal_uint32 *src_end_pos)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* unsupported charset */
    *src_end_pos = (kal_uint32) src_buff;

    if (src_type >= MMI_CHSET_TOTAL || dest_type >= MMI_CHSET_TOTAL)
    {
        return 0;
    }

    /* If src_type == dest_type */
    if (src_type == dest_type)
    {
        return mmi_chset_copy_to_dest(src_type, src_buff, dest_buff, dest_size, src_end_pos);
    }

    if (g_chset_state[src_type] == 0) /* If source type is disabled, but is 8 bit based simple code page*/
    {
        if (src_type <= MMI_CHSET_WESTERN_WIN)
	{
	    mmi_chset_simple_convert((kal_int8 *)dest_buff, dest_size, (kal_int8 *)src_buff, src_end_pos);			
	}

        return dest_size;
    }

    if (g_chset_state[dest_type] == 0)
    {
        return 0;
    }

    if (src_type == MMI_CHSET_UCS2 && dest_type == MMI_CHSET_UTF8)
    {
        return EncodingDecoding(UCS2_TO_UTF8, dest_buff, src_buff, dest_size, src_end_pos);
    }
    else if (src_type == MMI_CHSET_UTF8 && dest_type == MMI_CHSET_UCS2)
    {
        return EncodingDecoding(UTF8_TO_UCS2, dest_buff, src_buff, dest_size, src_end_pos);
    }
    /* UTF-16 */
    else if (src_type == MMI_CHSET_UTF16LE && dest_type == MMI_CHSET_UCS2)
    {
        return mmi_chset_utf16_to_ucs2_string(
                (kal_int8*) dest_buff,
                dest_size,
                (kal_int8*) src_buff,
                MMI_CHSET_UTF16LE,
                src_end_pos);
    }
    else if (src_type == MMI_CHSET_UCS2 && dest_type == MMI_CHSET_UTF16LE)
    {
        return mmi_chset_ucs2_to_utf16_string(
                (kal_int8*) dest_buff,
                dest_size,
                MMI_CHSET_UTF16LE,
                KAL_FALSE,
                (kal_int8*) src_buff,
                src_end_pos);
    }
    else if (src_type == MMI_CHSET_UTF16BE && dest_type == MMI_CHSET_UCS2)
    {
        return mmi_chset_utf16_to_ucs2_string(
                (kal_int8*) dest_buff,
                dest_size,
                (kal_int8*) src_buff,
                MMI_CHSET_UTF16BE,
                src_end_pos);
    }
    else if (src_type == MMI_CHSET_UCS2 && dest_type == MMI_CHSET_UTF16BE)
    {
        return mmi_chset_ucs2_to_utf16_string(
                (kal_int8*) dest_buff,
                dest_size,
                MMI_CHSET_UTF16BE,
                KAL_FALSE,
                (kal_int8*) src_buff,
                src_end_pos);
    }
#ifdef __MMI_CHSET_UTF7__
    else if (src_type == MMI_CHSET_UCS2 && dest_type == MMI_CHSET_UTF7)
    {
        return mmi_chset_ucs2_to_utf7_str(dest_buff, src_buff, dest_size, src_end_pos);
    }

    else if (src_type == MMI_CHSET_UTF7 && dest_type == MMI_CHSET_UCS2)
    {
        return mmi_chset_utf7_to_ucs2_str(dest_buff, src_buff, dest_size, src_end_pos);
    }
#endif /* __MMI_CHSET_UTF7__ */ 
#ifdef __MMI_CHSET_EUCKR_WIN__
    else if (src_type == MMI_CHSET_UCS2 && dest_type == MMI_CHSET_EUCKR)
    {
        return EncodingDecoding(UCS2_TO_EUCKR_WINDOWS, dest_buff, src_buff, dest_size, src_end_pos);
    }

    else if (src_type == MMI_CHSET_EUCKR && dest_type == MMI_CHSET_UCS2)
    {
        return EncodingDecoding(EUCKR_WINDOWS_TO_UCS2, dest_buff, src_buff, dest_size, src_end_pos);
    }
#endif /* __MMI_CHSET_EUCKR_WIN__ */ 
#ifdef __MMI_CHSET_THAI_WINDOWS__
    else if (src_type == MMI_CHSET_UCS2 && dest_type == MMI_CHSET_THAI_WIN)
    {
        return EncodingDecoding(UCS2_TO_THAI_WINDOWS, dest_buff, src_buff, dest_size, src_end_pos);
    }

    else if (src_type == MMI_CHSET_THAI_WIN && dest_type == MMI_CHSET_UCS2)
    {
        return EncodingDecoding(THAI_WINDOWS_TO_UCS2, dest_buff, src_buff, dest_size, src_end_pos);
    }
#endif /* __MMI_CHSET_THAI_WINDOWS__ */ 
#ifdef __MMI_CHSET_VIETNAMESE_WINDOWS__
    else if (src_type == MMI_CHSET_UCS2 && dest_type == MMI_CHSET_VIETNAMESE_WIN)
    {
        return EncodingDecoding(UCS2_TO_VIETNAMESE_WINDOWS, dest_buff, src_buff, dest_size, src_end_pos);
    }

    else if (src_type == MMI_CHSET_VIETNAMESE_WIN && dest_type == MMI_CHSET_UCS2)
    {
        return EncodingDecoding(VIETNAMESE_WINDOWS_TO_UCS2, dest_buff, src_buff, dest_size, src_end_pos);
    }
#endif /* __MMI_CHSET_VIETNAMESE_WINDOWS__ */ 
#ifdef __MMI_CHSET_KOI8_R__
    else if (src_type == MMI_CHSET_UCS2 && dest_type == MMI_CHSET_KOI8_R)
    {
        return EncodingDecoding(UCS2_TO_KOI8_R, dest_buff, src_buff, dest_size, src_end_pos);
    }
	
    else if (src_type == MMI_CHSET_KOI8_R && dest_type == MMI_CHSET_UCS2)
    {
        return EncodingDecoding(KOI8_R_TO_UCS2, dest_buff, src_buff, dest_size, src_end_pos);
    }
#endif /* __MMI_CHSET_KOI8_R__ */
#ifdef __MMI_CHSET_TIS_620__
    else if (src_type == MMI_CHSET_UCS2 && dest_type == MMI_CHSET_TIS_620)
    {
        return EncodingDecoding(UCS2_TO_TIS_620, dest_buff, src_buff, dest_size, src_end_pos);
    }
	
    else if (src_type == MMI_CHSET_TIS_620 && dest_type == MMI_CHSET_UCS2)
    {
        return EncodingDecoding(TIS_620_TO_UCS2, dest_buff, src_buff, dest_size, src_end_pos);
    }
#endif /* __MMI_CHSET_TIS_620__ */

#ifdef __MMI_CHSET_GB18030__
    else if (src_type == MMI_CHSET_GB18030 && dest_type == MMI_CHSET_UCS2)
    {
        return mmi_chset_gb18030_to_ucs2_str(dest_buff, src_buff, dest_size, src_end_pos);
    }
    else if (src_type == MMI_CHSET_UCS2 && dest_type == MMI_CHSET_GB18030)
    {
        return mmi_chset_ucs2_to_gb18030_str(dest_buff, src_buff, dest_size, src_end_pos);
    }
#endif /* __MMI_CHSET_GB18030__ */

#ifdef __MMI_CHSET_SJIS__
    else if (src_type == MMI_CHSET_SJIS && dest_type == MMI_CHSET_UCS2)
    {
        return mmi_chset_sjis_to_ucs2((kal_uint8 *)dest_buff, dest_size, (kal_uint8 *)src_buff, src_end_pos);
    }
    else if (src_type == MMI_CHSET_UCS2 && dest_type == MMI_CHSET_SJIS)
    {
        return mmi_chset_ucs2_to_sjis((kal_uint8 *)dest_buff, dest_size, (kal_uint8 *)src_buff, src_end_pos);
    }
#endif /* __MMI_CHSET_SJIS__ */

#if defined(__MMI_CHSET_ALPHABET_SUPPORT__) || defined(__MMI_CHSET_IDEOGRAM_SUPPORT__)
    else    /* intermedia conversion required */
    {
        if (src_type == MMI_CHSET_UCS2)
        {
            if( MMI_CHSET_ASCII == dest_type)
            {
                return mmi_chset_ucs2_to_ascii(dest_buff, src_buff, dest_size, src_end_pos);                          
            }   
            else
            {           
                return EncodingDecoding(
                    (mmi_chset_pair_enum) (BASE_ENCODING_SCHEME + (dest_type - MMI_CHSET_BASE) * 2),
                    dest_buff,
                    src_buff,
                    dest_size,
                    src_end_pos);
            }
            
        }
        else if (dest_type == MMI_CHSET_UCS2)
        {
            if( MMI_CHSET_ASCII == src_type)
            {
                return mmi_chset_ascii_to_ucs2(dest_buff, src_buff, dest_size, src_end_pos);          
            }   
            else
            {           
                
                return EncodingDecoding(
                    (mmi_chset_pair_enum) (BASE_ENCODING_SCHEME + (src_type - MMI_CHSET_BASE) * 2 - 1),
                    dest_buff,
                    src_buff,
                    dest_size,
                    src_end_pos);
            }
        }
        else
        {
            kal_uint16 result_len;
            kal_uint32 temp_src_end_pos = 0;

            char *tmp_buff = get_ctrl_buffer(CHSET_TMP_BUFF_SIZE);

            EncodingDecoding(
                (mmi_chset_pair_enum) (BASE_ENCODING_SCHEME + (src_type - MMI_CHSET_BASE) * 2 - 1),
                tmp_buff,
                src_buff,
                CHSET_TMP_BUFF_SIZE,
                src_end_pos);

            result_len = EncodingDecoding(
                            (mmi_chset_pair_enum) (BASE_ENCODING_SCHEME + (dest_type - MMI_CHSET_BASE) * 2),
                            dest_buff,
                            tmp_buff,
                            dest_size,
                            &temp_src_end_pos);

            free_ctrl_buffer(tmp_buff);
            return result_len;
        }
    }
#else /* defined(__MMI_CHSET_ALPHABET_SUPPORT__) || defined(__MMI_CHSET_IDEOGRAM_SUPPORT__) */ 
    return 0;
#endif /* defined(__MMI_CHSET_ALPHABET_SUPPORT__) || defined(__MMI_CHSET_IDEOGRAM_SUPPORT__) */ 

}


kal_int32 btmtk_chset_ucs2_to_utf8(kal_uint8 *utf8, kal_uint16 ucs2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int count;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ucs2 < (kal_uint16) 0x80)
    {
        count = 1;
    }
    else if (ucs2 < (kal_uint16) 0x800)
    {
        count = 2;
    }
    else
    {
        count = 3;
    }
    switch (count)
    {
        case 3:
            utf8[2] = 0x80 | (ucs2 & 0x3f);
            ucs2 = ucs2 >> 6;
            ucs2 |= 0x800;
        case 2:
            utf8[1] = 0x80 | (ucs2 & 0x3f);
            ucs2 = ucs2 >> 6;
            ucs2 |= 0xc0;
        case 1:
            utf8[0] = (kal_uint8) ucs2;
    }
    return count;
}


S32 btmtk_chset_ucs2_to_utf8_string(U8 *dest, S32 dest_size, U8 *src)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint32 src_end_pos = (kal_uint32) src;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return btmtk_chset_ucs2_to_utf8_string_ex(dest, dest_size, src, &src_end_pos);
}

static S32 kal_dchar_strlen(const char *str)
{
   char *temp=(char *)str;
   S32 len=0;

   while ( *temp!='\0' || *(temp+1)!='\0' )
   {
      temp+=2;
      len+=2;
   }

   return len;
}

S32 btmtk_charset_ucs2_to_utf8_length_in_bytes(const kal_uint8 *raw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int len = (kal_dchar_strlen((const char*)raw)) / 2;
    int utf8_len = 0;
    int i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < len; i++)
    {
        utf8_len += mmi_chset_ucs2_to_utf8_len(CHR_AT(raw, i));
    }

    return utf8_len;
}


S32 btmtk_chset_utf8_to_ucs2_string(U8 *dest, S32 dest_size, U8 *src)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint32 src_end_pos = (kal_uint32) src;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return btmtk_chset_utf8_to_ucs2_string_ex(dest, dest_size, src, &src_end_pos);
}


S32 btmtk_chset_utf8_strlen(const kal_uint8 *s)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int count = 0;
    int rawlen;
    int d;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (s == NULL)
    {
        ASSERT(0);
        return 0;
    }

    rawlen = strlen((const char*)s);

#ifdef MMI_CHSET_UTF8_ALLOW_BOM
    if (rawlen >= 3 && mmi_chset_is_utf8_BOM(s))
    {
        s += 3;
        rawlen -= 3;
    }
#endif /* MMI_CHSET_UTF8_ALLOW_BOM */ 

    while (*s != '\0' && rawlen > 0)
    {
        d = g_cheset_utf8_bytes_per_char[*s >> 4];
        if (d == 0)
        {
        #ifdef MMI_CHSET_UTF8_FAULT_TOLERANT
            /* skip this character */
            rawlen--;
            s++;
            continue;
        #else /* MMI_CHSET_UTF8_FAULT_TOLERANT */ 
            ASSERT(0);
            return count;
        #endif /* MMI_CHSET_UTF8_FAULT_TOLERANT */ 
        }
        if (d > rawlen)
        {
            return count;
        }

        rawlen -= d;
        s += d;

        count++;
    }

    ASSERT(rawlen == 0);

    return count;
}
S32 btmtk_chset_utf8_to_ucs2_length_in_bytes(const kal_uint8 *raw)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return STR_SIZE(btmtk_chset_utf8_strlen(raw));
}

BTMTK_CHSET_ENUM btmtk_chset_get_file_encoding(U16* src_filename)
{
    BTMTK_FS_HANDLE fh;
    char buf[100];
    S32 size = 0;
    mmi_chset_enum chset;

    if (!src_filename)
        return BTMTK_CHSET_NONE;

    fh = btmtk_fs_open_ucs2(src_filename, BTMTK_FS_READ_ONLY);
    if (fh <= 0)
        return BTMTK_CHSET_NONE;
    
    btmtk_fs_read_ucs2(fh, buf, 100, &size);
    btmtk_fs_close_ucs2(fh);

    if (size == 0)
        return BTMTK_CHSET_NONE;

    chset = mmi_chset_guess(buf, size, 0, CHSET_GUESS_ALL);
    /* maybe there should be a convert!!! */
    return (BTMTK_CHSET_ENUM)chset;
}

U8 btmtk_chset_set_file_encoding(U16* src_filename, BTMTK_CHSET_ENUM src_encode, U16* dst_filename, BTMTK_CHSET_ENUM dst_encode)
{
    U8* buf;
    U8* buf_encode = NULL;
    U8* p;
    U8 ret;
    S32 read_size = 0;
    S32 write_size = 0;
    S32 encode_size;
    BTMTK_FS_HANDLE src_fh;
    BTMTK_FS_HANDLE dst_fh;

    if (!src_filename || !dst_filename || dst_encode == BTMTK_CHSET_NONE)
        return 0;

    src_fh = btmtk_fs_open_ucs2(src_filename, BTMTK_FS_READ_ONLY);
    if (src_fh <= 0)
        return 0;

    dst_fh = btmtk_fs_open_ucs2(dst_filename, BTMTK_FS_CREATE_ALWAYS);
    if (dst_fh <= 0)
    {
        btmtk_fs_close_ucs2(src_fh);
        return 0;
    }

    /* then we will always think convert successfully */
    buf = get_ctrl_buffer(1024);
    

    ret = 0;

    while(BTMTK_FS_OK == btmtk_fs_read_ucs2(src_fh, buf, 1022, &read_size))
    {
        if (!read_size)
            break;

        /* get charset */
        if (src_encode == BTMTK_CHSET_NONE)
        {
            src_encode = mmi_chset_guess((const char*)buf, 100, CHSET_UTF8, CHSET_GUESS_ALL);
            if (src_encode == BTMTK_CHSET_NONE)
                break;

            if (src_encode != dst_encode)
                buf_encode = get_ctrl_buffer(1024);
        }
        p = buf;
        p[read_size] = 0;
        p[read_size + 1] = 0;

        if (src_encode != dst_encode)
        {
            U8* p_check = p;
            while ((encode_size = mmi_chset_convert_ex(src_encode, dst_encode, (char*)p, (char*)buf_encode, 1024, (U32*)&p)) > 0)
            {
                if (BTMTK_FS_OK != btmtk_fs_write_ucs2(dst_fh, buf_encode, encode_size, &write_size) || write_size != encode_size)
                {
                    ret = 0;
                    break;
                }
                if ((S32)(p - buf) >= read_size)
                    break;
                 
                /* there is '\0' or "\0\0" in string
                 * we do not support this case now
                */
                if (p_check == p)
                    break;
                p_check = p;
            }
            if ((S32)(p - buf) != read_size)
            {
                ret = 0;
                break;
            }
            else
            {
                ret = 1;
            }
        }
        else
        {
            if (BTMTK_FS_OK != btmtk_fs_write_ucs2(dst_fh, p, read_size, &write_size) || read_size != write_size)
            {
                ret = 0;
                break;
            }
            else
                ret = 1;
        }

    }

    btmtk_fs_close_ucs2(src_fh);
    btmtk_fs_close_ucs2(dst_fh);
    free_ctrl_buffer(buf);
    if (buf_encode)
        free_ctrl_buffer(buf_encode);
    return ret;
}
