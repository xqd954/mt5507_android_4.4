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

#ifndef __BT_OSAL_CHARSET_CONVERT_H__
#define __BT_OSAL_CHARSET_CONVERT_H__

typedef enum
{
    BTMTK_CHSET_NONE,
    BTMTK_CHSET_ARABIC_ISO,
    BTMTK_CHSET_ARABIC_WIN,
    /* PMT START PERSIAN */
    BTMTK_CHSET_PERSIAN_CP1097,
    BTMTK_CHSET_PERSIAN_CP1098,
    /* PMT END PERSIAN */
    BTMTK_CHSET_BALTIC_ISO,
    BTMTK_CHSET_BALTIC_WIN,
    BTMTK_CHSET_CEURO_ISO,
    BTMTK_CHSET_CEURO_WIN,
    BTMTK_CHSET_CYRILLIC_ISO,
    BTMTK_CHSET_CYRILLIC_WIN,
    BTMTK_CHSET_GREEK_ISO,
    BTMTK_CHSET_GREEK_WIN,
    BTMTK_CHSET_HEBREW_ISO,
    BTMTK_CHSET_HEBREW_WIN,
    BTMTK_CHSET_LATIN_ISO,
    BTMTK_CHSET_NORDIC_ISO,
    BTMTK_CHSET_SEURO_ISO,
    BTMTK_CHSET_TURKISH_ISO,
    BTMTK_CHSET_TURKISH_WIN,
    BTMTK_CHSET_WESTERN_ISO,
    BTMTK_CHSET_ARMENIAN_ISO,
    BTMTK_CHSET_WESTERN_WIN,
    /* Add new (8-bit) encodings above this line */
    BTMTK_CHSET_BIG5,
    BTMTK_CHSET_GB2312,
    BTMTK_CHSET_HKSCS,
    BTMTK_CHSET_SJIS,
    BTMTK_CHSET_GB18030,
    BTMTK_CHSET_UTF7,
    BTMTK_CHSET_EUCKR,
    BTMTK_CHSET_THAI_WIN,
    BTMTK_CHSET_VIETNAMESE_WIN,
    BTMTK_CHSET_KOI8_R,
    BTMTK_CHSET_TIS_620,

    /* PLace all encodings using algo to above this one */
    BTMTK_CHSET_UTF16LE,
    BTMTK_CHSET_UTF16BE,
    BTMTK_CHSET_UTF8,
    BTMTK_CHSET_UCS2,
    BTMTK_CHSET_ASCII,
    BTMTK_CHSET_TOTAL,
} BTMTK_CHSET_ENUM;

S32 mmi_chset_ucs2_to_utf8(U8 *utf8, U16 ucs2);

S32 btmtk_chset_ucs2_to_utf8_string(U8 *dest, S32 dest_size, U8 *src);

S32 btmtk_chset_ucs2_to_utf8_string_ex(U8 *dest, S32 dest_size, U8 *src, U32 *src_end_pos);

S32 btmtk_charset_ucs2_to_utf8_length_in_bytes(const U8 *raw);

S32 btmtk_chset_utf8_to_ucs2(U8 *dest, U8 *utf8);

S32 btmtk_chset_utf8_to_ucs2_string(U8 *dest, S32 dest_size, U8 *src);

S32 btmtk_chset_utf8_to_ucs2_length_in_bytes(const U8 *raw);

BTMTK_CHSET_ENUM btmtk_chset_get_file_encoding(U16* src_filename);

/* src_encode : indicate src_filename's encoding, 
 *              if not available, set it to BTMTK_CHSET_NONE 
 *              and then the function will guess encoding fristly 
 * return value:
 *          0 : failure
 *          other value : success
 */
U8 btmtk_chset_set_file_encoding(U16* src_filename, BTMTK_CHSET_ENUM src_encode, U16* dst_filename, BTMTK_CHSET_ENUM dst_encode);

#endif
