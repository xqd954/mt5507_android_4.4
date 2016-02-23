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
 *     $Workfile:sbc.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description: This file contains the SBC codec for the A2DP profile.
 *             
 * Created:     August 6, 2004
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
#include "overide.h"
#include "sbc.h"
#include "utils.h"

#define Assert(ex) (void)0
#define Report(ex) (void)0

#if SBC_DECODER == XA_ENABLED
/* Function prototypes */
static void SbcResetDecoderState(SbcDecoder *Decoder);
#endif

extern void OS_MemSet(U8 *dest, U8 byte, U32 len);

/****************************************************************************
 *
 * ROMable data
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *
 * Reversed CRC lookup table for CRC-8, Poly: G(X) = X8 + X4 + X3 + X2 + 1.
 */
const U8 SbcCrcTable[256] = {
    0x00, 0x1D, 0x3A, 0x27, 0x74, 0x69, 0x4E, 0x53, 0xE8, 0xF5, 0xD2, 0xCF,
    0x9C, 0x81, 0xA6, 0xBB, 0xCD, 0xD0, 0xF7, 0xEA, 0xB9, 0xA4, 0x83, 0x9E,
    0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B, 0x76, 0x87, 0x9A, 0xBD, 0xA0,
    0xF3, 0xEE, 0xC9, 0xD4, 0x6F, 0x72, 0x55, 0x48, 0x1B, 0x06, 0x21, 0x3C,
    0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04, 0x19, 0xA2, 0xBF, 0x98, 0x85,
    0xD6, 0xCB, 0xEC, 0xF1, 0x13, 0x0E, 0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40,
    0xFB, 0xE6, 0xC1, 0xDC, 0x8F, 0x92, 0xB5, 0xA8, 0xDE, 0xC3, 0xE4, 0xF9,
    0xAA, 0xB7, 0x90, 0x8D, 0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65,
    0x94, 0x89, 0xAE, 0xB3, 0xE0, 0xFD, 0xDA, 0xC7, 0x7C, 0x61, 0x46, 0x5B,
    0x08, 0x15, 0x32, 0x2F, 0x59, 0x44, 0x63, 0x7E, 0x2D, 0x30, 0x17, 0x0A,
    0xB1, 0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2, 0x26, 0x3B, 0x1C, 0x01,
    0x52, 0x4F, 0x68, 0x75, 0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80, 0x9D,
    0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8, 0x03, 0x1E, 0x39, 0x24,
    0x77, 0x6A, 0x4D, 0x50, 0xA1, 0xBC, 0x9B, 0x86, 0xD5, 0xC8, 0xEF, 0xF2,
    0x49, 0x54, 0x73, 0x6E, 0x3D, 0x20, 0x07, 0x1A, 0x6C, 0x71, 0x56, 0x4B,
    0x18, 0x05, 0x22, 0x3F, 0x84, 0x99, 0xBE, 0xA3, 0xF0, 0xED, 0xCA, 0xD7,
    0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C, 0x7B, 0x66, 0xDD, 0xC0, 0xE7, 0xFA,
    0xA9, 0xB4, 0x93, 0x8E, 0xF8, 0xE5, 0xC2, 0xDF, 0x8C, 0x91, 0xB6, 0xAB,
    0x10, 0x0D, 0x2A, 0x37, 0x64, 0x79, 0x5E, 0x43, 0xB2, 0xAF, 0x88, 0x95,
    0xC6, 0xDB, 0xFC, 0xE1, 0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09,
    0x7F, 0x62, 0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C, 0x97, 0x8A, 0xAD, 0xB0,
    0xE3, 0xFE, 0xD9, 0xC4
};

/*---------------------------------------------------------------------------
 *
 * Loudness offset table for bit allocation (4 subbands).
 */
const S8 LoudnessOffset4[4][4] = {
    {(S8)-1,  0,  0,  0},
    {(S8)-2,  0,  0,  1},
    {(S8)-2,  0,  0,  1},
    {(S8)-2,  0,  0,  1}
};

/*---------------------------------------------------------------------------
 *
 * Loudness offset table for bit allocation (8 subbands).
 */
const S8 LoudnessOffset8[4][8] = {
    {(S8)-2,  0,  0,  0,  0,  0,  0,  1},
    {(S8)-3,  0,  0,  0,  0,  0,  1,  2},
    {(S8)-4,  0,  0,  0,  0,  0,  1,  2},
    {(S8)-4,  0,  0,  0,  0,  0,  1,  2}
};

#if SBC_USE_FIXED_POINT == XA_ENABLED

#if SBC_DECODER == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Discrete Cosine Table for Synthesis filter (4 Subbands).
 */
const REAL Synth4[8][4] = {
    {0x2D413CCB, 0xD2BEC335, 0xD2BEC334, 0x2D413CCB},
    {0x187DE2A5, 0xC4DF2863, 0x3B20D79D, 0xE7821D5B},
    {0x00000000, 0x00000000, 0x00000000, 0x00000000},
    {0xE7821D5B, 0x3B20D79D, 0xC4DF2863, 0x187DE2A5},
    {0xD2BEC335, 0x2D413CCC, 0x2D413CCB, 0xD2BEC334},
    {0xC4DF2863, 0xE7821D5B, 0x187DE2A5, 0x3B20D79D},
    {0xC0000001, 0xC0000001, 0xC0000001, 0xC0000001},
    {0xC4DF2863, 0xE7821D5B, 0x187DE2A5, 0x3B20D79D}
};

/*---------------------------------------------------------------------------
 *
 * Discrete Cosine Table for Synthesis filter (8 Subbands).
 */
const REAL Synth8[16][8] = {
    {0x2D413CCB, 0xD2BEC335, 0xD2BEC334, 0x2D413CCB, 
     0x2D413CCC, 0xD2BEC335, 0xD2BEC334, 0x2D413CCB},
    {0x238E7672, 0xC13AD062, 0x0C7C5C1D, 0x3536CC51, 
     0xCAC933AF, 0xF383A3E3, 0x3EC52F9E, 0xDC71898E},
    {0x187DE2A5, 0xC4DF2863, 0x3B20D79D, 0xE7821D5B, 
     0xE7821D5B, 0x3B20D79D, 0xC4DF2863, 0x187DE2A5},
    {0x0C7C5C1D, 0xDC71898E, 0x3536CC51, 0xC13AD062, 
     0x3EC52F9E, 0xCAC933AF, 0x238E7672, 0xF383A3E3},
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000},
    {0xF383A3E3, 0x238E7672, 0xCAC933AF, 0x3EC52F9E, 
     0xC13AD062, 0x3536CC51, 0xDC71898E, 0x0C7C5C1D},
    {0xE7821D5B, 0x3B20D79D, 0xC4DF2863, 0x187DE2A5, 
     0x187DE2A5, 0xC4DF2863, 0x3B20D79D, 0xE7821D5B},
    {0xDC71898E, 0x3EC52F9E, 0xF383A3E3, 0xCAC933AF, 
     0x3536CC51, 0x0C7C5C1D, 0xC13AD062, 0x238E7672},
    {0xD2BEC335, 0x2D413CCC, 0x2D413CCB, 0xD2BEC334, 
     0xD2BEC335, 0x2D413CCC, 0x2D413CCB, 0xD2BEC334},
    {0xCAC933AF, 0x0C7C5C1D, 0x3EC52F9E, 0x238E7672, 
     0xDC71898E, 0xC13AD062, 0xF383A3E3, 0x3536CC51},
    {0xC4DF2863, 0xE7821D5B, 0x187DE2A5, 0x3B20D79D, 
     0x3B20D79D, 0x187DE2A5, 0xE7821D5B, 0xC4DF2863},
    {0xC13AD062, 0xCAC933AF, 0xDC71898E, 0xF383A3E3, 
     0x0C7C5C1D, 0x238E7672, 0x3536CC51, 0x3EC52F9E},
    {0xC0000001, 0xC0000001, 0xC0000001, 0xC0000001, 
     0xC0000001, 0xC0000001, 0xC0000001, 0xC0000001},
    {0xC13AD062, 0xCAC933AF, 0xDC71898E, 0xF383A3E3, 
     0x0C7C5C1D, 0x238E7672, 0x3536CC51, 0x3EC52F9E},
    {0xC4DF2863, 0xE7821D5B, 0x187DE2A5, 0x3B20D79D, 
     0x3B20D79D, 0x187DE2A5, 0xE7821D5B, 0xC4DF2863},
    {0xCAC933AF, 0x0C7C5C1D, 0x3EC52F9E, 0x238E7672, 
     0xDC71898E, 0xC13AD062, 0xF383A3E3, 0x3536CC51}
};

/*---------------------------------------------------------------------------
 *
 * Filter Coefficients Table for Decode (4 subbands).
 */
const REAL SbcDecodeCoeff4[40] = {
    0x00000000, 0xFFDCD635, 0xFF9E3A5A, 0xFF4CD7FB, 
    0xFF048671, 0xFF00EE38, 0xFF85B8CB, 0x00C88C56, 
    0xFD34C177, 0xFAC48ABB, 0xF89B997E, 0xF7C22381, 
    0xF96023A9, 0xFE6E1A89, 0x0760DC57, 0x13E0A192, 
    0xDD49C25C, 0xCE1546E1, 0xC0DC6B74, 0xB7DA1B5F, 
    0xB4A7C01C, 0xB7DA1B5F, 0xC0DC6B74, 0xCE1546E1, 
    0x22B63DA4, 0x13E0A192, 0x0760DC57, 0xFE6E1A89, 
    0xF96023A9, 0xF7C22381, 0xF89B997E, 0xFAC48ABB, 
    0x02CB3E89, 0x00C88C56, 0xFF85B8CB, 0xFF00EE38, 
    0xFF048671, 0xFF4CD7FB, 0xFF9E3A5A, 0xFFDCD635
};

/*---------------------------------------------------------------------------
 *
 * Filter Coefficients Table for Decode (8 subbands).
 */
const REAL SbcDecodeCoeff8[80] = {
    0x00000000, 0xFFEB7A35, 0xFFD3023B, 0xFFB74E0A, 
    0xFF9401DB, 0xFF6A9678, 0xFF3E7C30, 0xFF163463, 
    0xFEF84E59, 0xFEEC42E1, 0xFEFA922A, 0xFF2C1D29, 
    0xFF89C0B9, 0x00176FB5, 0x00D83BC8, 0x01CA61B3, 
    0xFD1A32E0, 0xFBE391A9, 0xFAA530D9, 0xF97931D4, 
    0xF87F76CF, 0xF7DB5B85, 0xF7B1E6B2, 0xF8282F70, 
    0xF9604C42, 0xFB770518, 0xFE80BC04, 0x02845235, 
    0x077EF28E, 0x0D5E4607, 0x1401A80F, 0x1B3B5DBE, 
    0xDD2F3E18, 0xD5830598, 0xCE0A9928, 0xC7113A44, 
    0xC0E3781D, 0xBBC4C441, 0xB7EF2818, 0xB58F7645, 
    0xB4C24E2B, 0xB58F7645, 0xB7EF2818, 0xBBC4C441, 
    0xC0E3781D, 0xC7113A44, 0xCE0A9928, 0xD5830598, 
    0x22D0C1E8, 0x1B3B5DBE, 0x1401A80F, 0x0D5E4607, 
    0x077EF28E, 0x02845235, 0xFE80BC04, 0xFB770518, 
    0xF9604C42, 0xF8282F70, 0xF7B1E6B2, 0xF7DB5B85, 
    0xF87F76CF, 0xF97931D4, 0xFAA530D9, 0xFBE391A9, 
    0x02E5CD20, 0x01CA61B3, 0x00D83BC8, 0x00176FB5, 
    0xFF89C0B9, 0xFF2C1D29, 0xFEFA922A, 0xFEEC42E1, 
    0xFEF84E59, 0xFF163463, 0xFF3E7C30, 0xFF6A9678, 
    0xFF9401DB, 0xFFB74E0A, 0xFFD3023B, 0xFFEB7A35
};
#endif /* SBC_DECODER == XA_ENABLED */

#if SBC_ENCODER == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Discrete Cosine Table for Analysis filter (4 Subbands).
 */
const REAL Analyze4[8][4] = {
    {0x2D413CCB, 0xD2BEC335, 0xD2BEC334, 0x2D413CCB},
    {0x3B20D79D, 0x187DE2A5, 0xE7821D5B, 0xC4DF2863},
    {0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF},
    {0x3B20D79D, 0x187DE2A5, 0xE7821D5B, 0xC4DF2863},
    {0x2D413CCB, 0xD2BEC335, 0xD2BEC334, 0x2D413CCB},
    {0x187DE2A5, 0xC4DF2863, 0x3B20D79D, 0xE7821D5B},
    {0x00000000, 0x00000000, 0x00000000, 0x00000000},
    {0xE7821D5B, 0x3B20D79D, 0xC4DF2863, 0x187DE2A5}
};

/*---------------------------------------------------------------------------
 *
 * Discrete Cosine Table for Analysis filter (8 Subbands).
 */
const REAL Analyze8[16][8] = {
    {0x2D413CCB, 0xD2BEC335, 0xD2BEC334, 0x2D413CCB, 
     0x2D413CCC, 0xD2BEC335, 0xD2BEC334, 0x2D413CCB},
    {0x3536CC51, 0xF383A3E3, 0xC13AD062, 0xDC71898E, 
     0x238E7672, 0x3EC52F9E, 0x0C7C5C1D, 0xCAC933AF},
    {0x3B20D79D, 0x187DE2A5, 0xE7821D5B, 0xC4DF2863, 
     0xC4DF2863, 0xE7821D5B, 0x187DE2A5, 0x3B20D79D},
    {0x3EC52F9E, 0x3536CC51, 0x238E7672, 0x0C7C5C1D, 
     0xF383A3E3, 0xDC71898E, 0xCAC933AF, 0xC13AD062},
    {0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 
     0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF},
    {0x3EC52F9E, 0x3536CC51, 0x238E7672, 0x0C7C5C1D, 
     0xF383A3E3, 0xDC71898E, 0xCAC933AF, 0xC13AD062},
    {0x3B20D79D, 0x187DE2A5, 0xE7821D5B, 0xC4DF2863, 
     0xC4DF2863, 0xE7821D5B, 0x187DE2A5, 0x3B20D79D},
    {0x3536CC51, 0xF383A3E3, 0xC13AD062, 0xDC71898E, 
     0x238E7672, 0x3EC52F9E, 0x0C7C5C1D, 0xCAC933AF},
    {0x2D413CCB, 0xD2BEC335, 0xD2BEC334, 0x2D413CCB, 
     0x2D413CCC, 0xD2BEC335, 0xD2BEC334, 0x2D413CCB},
    {0x238E7672, 0xC13AD062, 0x0C7C5C1D, 0x3536CC51, 
     0xCAC933AF, 0xF383A3E3, 0x3EC52F9E, 0xDC71898E},
    {0x187DE2A5, 0xC4DF2863, 0x3B20D79D, 0xE7821D5B, 
     0xE7821D5B, 0x3B20D79D, 0xC4DF2863, 0x187DE2A5},
    {0x0C7C5C1D, 0xDC71898E, 0x3536CC51, 0xC13AD062, 
     0x3EC52F9E, 0xCAC933AF, 0x238E7672, 0xF383A3E3},
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, 
     0x00000000, 0x00000000, 0x00000000, 0x00000000},
    {0xF383A3E3, 0x238E7672, 0xCAC933AF, 0x3EC52F9E, 
     0xC13AD062, 0x3536CC51, 0xDC71898E, 0x0C7C5C1D},
    {0xE7821D5B, 0x3B20D79D, 0xC4DF2863, 0x187DE2A5, 
     0x187DE2A5, 0xC4DF2863, 0x3B20D79D, 0xE7821D5B},
    {0xDC71898E, 0x3EC52F9E, 0xF383A3E3, 0xCAC933AF, 
     0x3536CC51, 0x0C7C5C1D, 0xC13AD062, 0x238E7672}
};

/*---------------------------------------------------------------------------
 *
 * Filter Coefficients Table for Encode (4 subbands).
 */
const REAL SbcCoefficient4[40] = {
    0x00000000, 0x0008CA72, 0x00187168, 0x002CCA00, 
    0x003EDE63, 0x003FC471, 0x001E91CC, 0xFFCDDCEC, 
    0x00B2CFA1, 0x014EDD50, 0x01D9199F, 0x020F771F, 
    0x01A7F715, 0x0064795D, 0xFE27C8EB, 0xFB07D79D, 
    0x08AD8F68, 0x0C7AAE47, 0x0FC8E522, 0x12097927, 
    0x12D60FF8, 0x12097927, 0x0FC8E522, 0x0C7AAE47, 
    0xF7527098, 0xFB07D79D, 0xFE27C8EB, 0x0064795D, 
    0x01A7F715, 0x020F771F, 0x01D9199F, 0x014EDD50, 
    0xFF4D305F, 0xFFCDDCEC, 0x001E91CC, 0x003FC471, 
    0x003EDE63, 0x002CCA00, 0x00187168, 0x0008CA72
};

/*---------------------------------------------------------------------------
 *
 * Filter Coefficients Table for Encode (8 subbands).
 */
const REAL SbcCoefficient8[80] = {
    0x00000000, 0x000290B8, 0x00059FB7, 0x0009163D, 
    0x000D7FC3, 0x0012AD30, 0x00183079, 0x001D3972, 
    0x0020F634, 0x002277A3, 0x0020ADB9, 0x001A7C5A, 
    0x000EC7E8, 0xFFFD120B, 0xFFE4F888, 0xFFC6B3CB, 
    0x005CB9A3, 0x00838DCA, 0x00AB59E4, 0x00D0D9C4, 
    0x00F01125, 0x0104948E, 0x0109C328, 0x00FAFA11, 
    0x00D3F676, 0x00911F5C, 0x002FE87E, 0xFFAF75BB, 
    0xFF1021B0, 0xFE543740, 0xFD7FCAFF, 0xFC98944A, 
    0x045A183C, 0x054F9F4C, 0x063EACDA, 0x071DD8B6, 
    0x07E390FB, 0x08876777, 0x09021AFC, 0x094E1136, 
    0x0967B639, 0x094E1136, 0x09021AFC, 0x08876777, 
    0x07E390FB, 0x071DD8B6, 0x063EACDA, 0x054F9F4C, 
    0xFBA5E7C4, 0xFC98944A, 0xFD7FCAFF, 0xFE543740, 
    0xFF1021B0, 0xFFAF75BB, 0x002FE87E, 0x00911F5C, 
    0x00D3F676, 0x00FAFA11, 0x0109C328, 0x0104948E, 
    0x00F01125, 0x00D0D9C4, 0x00AB59E4, 0x00838DCA, 
    0xFFA3465D, 0xFFC6B3CB, 0xFFE4F888, 0xFFFD120B, 
    0x000EC7E8, 0x001A7C5A, 0x0020ADB9, 0x002277A3, 
    0x0020F634, 0x001D3972, 0x00183079, 0x0012AD30, 
    0x000D7FC3, 0x0009163D, 0x00059FB7, 0x000290B8
};
#endif /* SBC_ENCODER == XA_ENABLED */

#else

#if SBC_DECODER == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Discrete Cosine Table for Synthesis filter (4 Subbands).
 */
const REAL Synth4[8][4] = {
    { 7.0710678119e-001, -7.0710678118e-001, -7.0710678120e-001,  7.0710678117e-001},
    { 3.8268343237e-001, -9.2387953252e-001,  9.2387953250e-001, -3.8268343234e-001},
    { 4.8965888581e-012, -1.4689766574e-011,  2.4482944291e-011, -3.4275233828e-011},
    {-3.8268343236e-001,  9.2387953250e-001, -9.2387953252e-001,  3.8268343240e-001},
    {-7.0710678118e-001,  7.0710678120e-001,  7.0710678116e-001, -7.0710678122e-001},
    {-9.2387953251e-001, -3.8268343234e-001,  3.8268343240e-001,  9.2387953253e-001},
    {-1.0000000000e+000, -1.0000000000e+000, -1.0000000000e+000, -1.0000000000e+000},
    {-9.2387953252e-001, -3.8268343240e-001,  3.8268343231e-001,  9.2387953248e-001}
};

/*---------------------------------------------------------------------------
 *
 * Discrete Cosine Table for Synthesis filter (8 Subbands).
 */
const REAL Synth8[16][8] = {
    { 7.0710678119e-001, -7.0710678118e-001, -7.0710678120e-001,  7.0710678117e-001, 
      7.0710678120e-001, -7.0710678117e-001, -7.0710678121e-001,  7.0710678116e-001},
    { 5.5557023302e-001, -9.8078528040e-001,  1.9509032200e-001,  8.3146961231e-001, 
     -8.3146961229e-001, -1.9509032205e-001,  9.8078528041e-001, -5.5557023298e-001},
    { 3.8268343237e-001, -9.2387953252e-001,  9.2387953250e-001, -3.8268343234e-001, 
     -3.8268343240e-001,  9.2387953253e-001, -9.2387953249e-001,  3.8268343231e-001},
    { 1.9509032202e-001, -5.5557023303e-001,  8.3146961231e-001, -9.8078528041e-001, 
      9.8078528040e-001, -8.3146961228e-001,  5.5557023297e-001, -1.9509032195e-001},
    { 4.8965888581e-012, -1.4689766574e-011,  2.4482944291e-011, -3.4275233828e-011, 
      4.4070187902e-011, -5.3861589261e-011,  6.3656543334e-011, -7.3447944693e-011},
    {-1.9509032201e-001,  5.5557023301e-001, -8.3146961229e-001,  9.8078528040e-001, 
     -9.8078528041e-001,  8.3146961234e-001, -5.5557023308e-001,  1.9509032210e-001},
    {-3.8268343236e-001,  9.2387953250e-001, -9.2387953252e-001,  3.8268343240e-001, 
      3.8268343231e-001, -9.2387953249e-001,  9.2387953254e-001, -3.8268343245e-001},
    {-5.5557023301e-001,  9.8078528041e-001, -1.9509032205e-001, -8.3146961228e-001, 
      8.3146961234e-001,  1.9509032194e-001, -9.8078528039e-001,  5.5557023310e-001},
    {-7.0710678118e-001,  7.0710678120e-001,  7.0710678116e-001, -7.0710678122e-001, 
     -7.0710678114e-001,  7.0710678124e-001,  7.0710678112e-001, -7.0710678126e-001},
    {-8.3146961230e-001,  1.9509032204e-001,  9.8078528041e-001,  5.5557023297e-001, 
     -5.5557023308e-001, -9.8078528039e-001, -1.9509032191e-001,  8.3146961237e-001},
    {-9.2387953251e-001, -3.8268343234e-001,  3.8268343240e-001,  9.2387953253e-001, 
      9.2387953248e-001,  3.8268343228e-001, -3.8268343247e-001, -9.2387953256e-001},
    {-9.8078528040e-001, -8.3146961229e-001, -5.5557023298e-001, -1.9509032195e-001, 
      1.9509032210e-001,  5.5557023310e-001,  8.3146961237e-001,  9.8078528043e-001},
    {-1.0000000000e+000, -1.0000000000e+000, -1.0000000000e+000, -1.0000000000e+000, 
     -1.0000000000e+000, -1.0000000000e+000, -1.0000000000e+000, -1.0000000000e+000},
    {-9.8078528041e-001, -8.3146961232e-001, -5.5557023306e-001, -1.9509032209e-001, 
      1.9509032192e-001,  5.5557023292e-001,  8.3146961223e-001,  9.8078528037e-001},
    {-9.2387953252e-001, -3.8268343240e-001,  3.8268343231e-001,  9.2387953248e-001, 
      9.2387953255e-001,  3.8268343248e-001, -3.8268343223e-001, -9.2387953245e-001},
    {-8.3146961231e-001,  1.9509032198e-001,  9.8078528039e-001,  5.5557023309e-001, 
     -5.5557023293e-001, -9.8078528043e-001, -1.9509032216e-001,  8.3146961221e-001}
};

const REAL SbcDecodeCoeff4[40] = {
     0.0000000000e+000, -2.1461959040e-003, -5.9675342800e-003, -1.0934836160e-002, 
    -1.5348807720e-002, -1.5568205960e-002, -7.4632676400e-003,  1.2240491440e-002, 
    -4.3655048000e-002, -8.1754034800e-002, -1.1550295680e-001, -1.2877571600e-001, 
    -1.0350712440e-001, -2.4529807440e-002,  1.1528690960e-001,  3.1058539760e-001, 
    -5.4237309600e-001, -7.7995136400e-001, -9.8654664800e-001, -1.1273128120e+000, 
    -1.1772613280e+000, -1.1273128120e+000, -9.8654664800e-001, -7.7995136400e-001, 
     5.4237309600e-001,  3.1058539760e-001,  1.1528690960e-001, -2.4529807440e-002, 
    -1.0350712440e-001, -1.2877571600e-001, -1.1550295680e-001, -8.1754034800e-002, 
     4.3655048000e-002,  1.2240491440e-002, -7.4632676400e-003, -1.5568205960e-002, 
    -1.5348807720e-002, -1.0934836160e-002, -5.9675342800e-003, -2.1461959040e-003
};

const REAL SbcDecodeCoeff8[80] = {
     0.0000000000e+000, -1.2526031840e-003, -2.7460514000e-003, -4.4369616160e-003, 
    -6.5913560480e-003, -9.1194005600e-003, -1.1811213520e-002, -1.4269738000e-002, 
    -1.6094603360e-002, -1.6829759120e-002, -1.5956364320e-002, -1.2932502640e-002, 
    -7.2172360160e-003,  1.4304428880e-003,  1.3197847840e-002,  2.7977396320e-002, 
    -4.5275957840e-002, -6.4235293040e-002, -8.3667554400e-002, -1.0197786800e-001, 
    -1.1722021040e-001, -1.2723648240e-001, -1.2976677680e-001, -1.2254728480e-001, 
    -1.0349744480e-001, -7.0860603200e-002, -2.3392675360e-002,  3.9326241920e-002, 
     1.1712326080e-001,  2.0887900160e-001,  3.1260110480e-001,  4.2549842560e-001, 
    -5.4399154480e-001, -6.6387806240e-001, -7.8060313440e-001, -8.8957351200e-001, 
    -9.8611638400e-001, -1.0661153200e+000, -1.1260280400e+000, -1.1631187760e+000, 
    -1.1756405440e+000, -1.1631187760e+000, -1.1260280400e+000, -1.0661153200e+000, 
    -9.8611638400e-001, -8.8957351200e-001, -7.8060313440e-001, -6.6387806240e-001, 
     5.4399154480e-001,  4.2549842560e-001,  3.1260110480e-001,  2.0887900160e-001, 
     1.1712326080e-001,  3.9326241920e-002, -2.3392675360e-002, -7.0860603200e-002, 
    -1.0349744480e-001, -1.2254728480e-001, -1.2976677680e-001, -1.2723648240e-001, 
    -1.1722021040e-001, -1.0197786800e-001, -8.3667554400e-002, -6.4235293040e-002, 
     4.5275957840e-002,  2.7977396320e-002,  1.3197847840e-002,  1.4304428880e-003, 
    -7.2172360160e-003, -1.2932502640e-002, -1.5956364320e-002, -1.6829759120e-002, 
    -1.6094603360e-002, -1.4269738000e-002, -1.1811213520e-002, -9.1194005600e-003, 
    -6.5913560480e-003, -4.4369616160e-003, -2.7460514000e-003, -1.2526031840e-003
};

#endif /* SBC_DECODER == XA_ENABLED */

#if SBC_ENCODER == XA_ENABLED
/*---------------------------------------------------------------------------
 *
 * Discrete Cosine Table for Analysis filter (4 Subbands).
 */
const REAL Analyze4[8][4] = {
    { 7.0710678119e-001, -7.0710678118e-001, -7.0710678120e-001,  7.0710678117e-001},
    { 9.2387953251e-001,  3.8268343237e-001, -3.8268343236e-001, -9.2387953251e-001},
    { 1.0000000000e+000,  1.0000000000e+000,  1.0000000000e+000,  1.0000000000e+000},
    { 9.2387953251e-001,  3.8268343237e-001, -3.8268343236e-001, -9.2387953251e-001},
    { 7.0710678119e-001, -7.0710678118e-001, -7.0710678120e-001,  7.0710678117e-001},
    { 3.8268343237e-001, -9.2387953252e-001,  9.2387953250e-001, -3.8268343234e-001},
    { 4.8965888581e-012, -1.4689766574e-011,  2.4482944291e-011, -3.4275233828e-011},
    {-3.8268343236e-001,  9.2387953250e-001, -9.2387953252e-001,  3.8268343240e-001}
};

/*---------------------------------------------------------------------------
 *
 * Discrete Cosine Table for Analysis filter (8 Subbands).
 */
const REAL Analyze8[16][8] = {
    { 7.0710678119e-001, -7.0710678118e-001, -7.0710678120e-001,  7.0710678117e-001, 
      7.0710678120e-001, -7.0710678117e-001, -7.0710678121e-001,  7.0710678116e-001},
    { 8.3146961230e-001, -1.9509032201e-001, -9.8078528040e-001, -5.5557023303e-001, 
      5.5557023301e-001,  9.8078528041e-001,  1.9509032204e-001, -8.3146961229e-001},
    { 9.2387953251e-001,  3.8268343237e-001, -3.8268343236e-001, -9.2387953251e-001, 
     -9.2387953252e-001, -3.8268343238e-001,  3.8268343235e-001,  9.2387953250e-001},
    { 9.8078528040e-001,  8.3146961230e-001,  5.5557023302e-001,  1.9509032202e-001, 
     -1.9509032201e-001, -5.5557023301e-001, -8.3146961230e-001, -9.8078528040e-001},
    { 1.0000000000e+000,  1.0000000000e+000,  1.0000000000e+000,  1.0000000000e+000, 
      1.0000000000e+000,  1.0000000000e+000,  1.0000000000e+000,  1.0000000000e+000},
    { 9.8078528040e-001,  8.3146961230e-001,  5.5557023302e-001,  1.9509032202e-001, 
     -1.9509032201e-001, -5.5557023301e-001, -8.3146961230e-001, -9.8078528040e-001},
    { 9.2387953251e-001,  3.8268343237e-001, -3.8268343236e-001, -9.2387953251e-001, 
     -9.2387953252e-001, -3.8268343238e-001,  3.8268343235e-001,  9.2387953250e-001},
    { 8.3146961230e-001, -1.9509032201e-001, -9.8078528040e-001, -5.5557023303e-001, 
      5.5557023301e-001,  9.8078528041e-001,  1.9509032204e-001, -8.3146961229e-001},
    { 7.0710678119e-001, -7.0710678118e-001, -7.0710678120e-001,  7.0710678117e-001, 
      7.0710678120e-001, -7.0710678117e-001, -7.0710678121e-001,  7.0710678116e-001},
    { 5.5557023302e-001, -9.8078528040e-001,  1.9509032200e-001,  8.3146961231e-001, 
     -8.3146961229e-001, -1.9509032205e-001,  9.8078528041e-001, -5.5557023298e-001},
    { 3.8268343237e-001, -9.2387953252e-001,  9.2387953250e-001, -3.8268343234e-001, 
     -3.8268343240e-001,  9.2387953253e-001, -9.2387953249e-001,  3.8268343231e-001},
    { 1.9509032202e-001, -5.5557023303e-001,  8.3146961231e-001, -9.8078528041e-001, 
      9.8078528040e-001, -8.3146961228e-001,  5.5557023297e-001, -1.9509032195e-001},
    { 4.8965888581e-012, -1.4689766574e-011,  2.4482944291e-011, -3.4275233828e-011, 
      4.4070187902e-011, -5.3861589261e-011,  6.3656543334e-011, -7.3447944693e-011},
    {-1.9509032201e-001,  5.5557023301e-001, -8.3146961229e-001,  9.8078528040e-001, 
     -9.8078528041e-001,  8.3146961234e-001, -5.5557023308e-001,  1.9509032210e-001},
    {-3.8268343236e-001,  9.2387953250e-001, -9.2387953252e-001,  3.8268343240e-001, 
      3.8268343231e-001, -9.2387953249e-001,  9.2387953254e-001, -3.8268343245e-001},
    {-5.5557023301e-001,  9.8078528041e-001, -1.9509032205e-001, -8.3146961228e-001, 
      8.3146961234e-001,  1.9509032194e-001, -9.8078528039e-001,  5.5557023310e-001}
};

/*---------------------------------------------------------------------------
 *
 * Filter Coefficients Table (4 subbands).
 */
const REAL SbcCoefficient4[40] = {
    0.00000000E+00,  5.36548976E-04,  1.49188357E-03,  2.73370904E-03,
    3.83720193E-03,  3.89205149E-03,  1.86581691E-03, -3.06012286E-03,
    1.09137620E-02,  2.04385087E-02,  2.88757392E-02,  3.21939290E-02,
    2.58767811E-02,  6.13245186E-03, -2.88217274E-02, -7.76463494E-02,
    1.35593274E-01,  1.94987841E-01,  2.46636662E-01,  2.81828203E-01,
    2.94315332E-01,  2.81828203E-01,  2.46636662E-01,  1.94987841E-01,
   -1.35593274E-01, -7.76463494E-02, -2.88217274E-02,  6.13245186E-03,
    2.58767811E-02,  3.21939290E-02,  2.88757392E-02,  2.04385087E-02,
   -1.09137620E-02, -3.06012286E-03,  1.86581691E-03,  3.89205149E-03,
    3.83720193E-03,  2.73370904E-03,  1.49188357E-03,  5.36548976E-04
};

/*---------------------------------------------------------------------------
 *
 * Filter Coefficients Table (8 subbands).
 */
const REAL SbcCoefficient8[80] = {
    0.00000000E+00,  1.56575398E-04,  3.43256425E-04,  5.54620202E-04,
    8.23919506E-04,  1.13992507E-03,  1.47640169E-03,  1.78371725E-03,
    2.01182542E-03,  2.10371989E-03,  1.99454554E-03,  1.61656283E-03,
    9.02154502E-04, -1.78805361E-04, -1.64973098E-03, -3.49717454E-03,
    5.65949473E-03,  8.02941163E-03,  1.04584443E-02,  1.27472335E-02,
    1.46525263E-02,  1.59045603E-02,  1.62208471E-02,  1.53184106E-02,
    1.29371806E-02,  8.85757540E-03,  2.92408442E-03, -4.91578024E-03,
   -1.46404076E-02, -2.61098752E-02, -3.90751381E-02, -5.31873032E-02,
    6.79989431E-02,  8.29847578E-02,  9.75753918E-02,  1.11196689E-01,
    1.23264548E-01,  1.33264415E-01,  1.40753505E-01,  1.45389847E-01,
    1.46955068E-01,  1.45389847E-01,  1.40753505E-01,  1.33264415E-01,
    1.23264548E-01,  1.11196689E-01,  9.75753918E-02,  8.29847578E-02,
   -6.79989431E-02, -5.31873032E-02, -3.90751381E-02, -2.61098752E-02,
   -1.46404076E-02, -4.91578024E-03,  2.92408442E-03,  8.85757540E-03,
    1.29371806E-02,  1.53184106E-02,  1.62208471E-02,  1.59045603E-02,
    1.46525263E-02,  1.27472335E-02,  1.04584443E-02,  8.02941163E-03,
   -5.65949473E-03, -3.49717454E-03, -1.64973098E-03, -1.78805361E-04,
    9.02154502E-04,  1.61656283E-03,  1.99454554E-03,  2.10371989E-03,
    2.01182542E-03,  1.78371725E-03,  1.47640169E-03,  1.13992507E-03,
    8.23919506E-04,  5.54620202E-04,  3.43256425E-04,  1.56575398E-04
};

#endif /* SBC_ENCODER == XA_ENABLED */

#endif /* SBC_USE_FIXED_POINT == XA_ENABLED */

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 *            SbcIsValidStreamInfo()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Checks the validity of an SbcStreamInfo structure.
 */
static BOOL SbcIsValidStreamInfo(SbcStreamInfo *StreamInfo)
{
    /* Check the channel mode */
    switch (StreamInfo->channelMode) {
    case SBC_CHNL_MODE_DUAL_CHNL:
    case SBC_CHNL_MODE_STEREO:
    case SBC_CHNL_MODE_JOINT_STEREO:
    case SBC_CHNL_MODE_MONO:
        break;
    default:
        return FALSE;
    }

    /* Check the sampling frequency */
    if (StreamInfo->sampleFreq > 4) {
        return FALSE;
    }

    /* Check the allocation method */
    if (StreamInfo->allocMethod > 1) {
        return FALSE;
    }

    /* Check the number of blocks */
    switch (StreamInfo->numBlocks) {
    case 4:
    case 8:
    case 12:
    case 16:
        break;
    default:
        return FALSE;
    }

    /* Check the number of subbands */
    if ((StreamInfo->numSubBands !=4) && 
        (StreamInfo->numSubBands !=8)) {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------
 *            SBC_FrameLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Returns the maximum size of an encoded SBC frame, derived from
 *            the fields in an initialized StreamInfo structure.
 */
U16 SBC_FrameLen(SbcStreamInfo *StreamInfo)
{
    U16      frameLen = 0;
    U32      sampleFreq, temp = 0;
    U8       join = 0;

    /* Set the number of channels */
    if (StreamInfo->channelMode == SBC_CHNL_MODE_MONO) {
        StreamInfo->numChannels = 1;
    } else {
        StreamInfo->numChannels = 2;
    }

    if (!SbcIsValidStreamInfo(StreamInfo)) {
        goto exit;
    }

    switch (StreamInfo->sampleFreq) {
    case 0:
        sampleFreq = 16000;
        break;
    case 1:
        sampleFreq = 32000;
        break;
    case 2:
        sampleFreq = 44100;
        break;
    case 3:
        sampleFreq = 48000;
        break;
    }

    switch (StreamInfo->channelMode) {
    case SBC_CHNL_MODE_MONO:
    case SBC_CHNL_MODE_DUAL_CHNL:

        temp = StreamInfo->numBlocks * 
               StreamInfo->numChannels * 
               StreamInfo->bitPool;

        break;
    case SBC_CHNL_MODE_JOINT_STEREO:
        join = 1;
    case SBC_CHNL_MODE_STEREO:

        temp = join * StreamInfo->numSubBands + 
               StreamInfo->numBlocks * StreamInfo->bitPool;

        break;
    }

    frameLen = (U16) (4 + (4 * StreamInfo->numSubBands * 
                           StreamInfo->numChannels) / 8 + 
                      temp / 8);

    if (temp % 8) {
        frameLen++;
    }

    exit:

    return frameLen;
}


/*---------------------------------------------------------------------------
 *            SbcCrcSum4()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Bitwise CRC summing for less than 8 bits.
 */
static void SbcCrcSum4(SbcStreamInfo *StreamInfo, U8 input)
{
    U8 i, shift, bit, FCS = StreamInfo->fcs;

    /* Just sum the most significant 4 bits */
    shift = 7;
    for (i = 0; i < 4; i++) {
        bit = (0x01 & (input >> shift--)) ^ (FCS >> 7);
        if (bit) {
            FCS = ((FCS << 1) | bit) ^ 0x1C;
        } else {
            FCS = (FCS << 1);
        }
    }                                         

    StreamInfo->fcs = FCS;
}

/*---------------------------------------------------------------------------
 *            SbcMonoBitAlloc()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Performs bit allocation for a mono stream.  For dual channel
 *            streams, one channel is processed at a time and this function is
 *            called once for each channel.
 */
static void SbcMonoBitAlloc(SbcStreamInfo *StreamInfo, U8 Ch)
{
    U8  sb;
    S8  loudness;
    S8  maxBitNeed = 0;
    U8  bitCount = 0;
    U8  sliceCount = 0;
    S8  bitSlice;

    if (StreamInfo->allocMethod == SBC_ALLOC_METHOD_SNR) {
        /* SNR allocation method */
        for (sb = 0; sb < StreamInfo->numSubBands; sb++) {
            StreamInfo->bitNeed[0][sb] = StreamInfo->scale_factors[Ch][sb];
        }
    } else {
        /* Loudness allocation method */
        for (sb = 0; sb < StreamInfo->numSubBands; sb++) {
            if (StreamInfo->scale_factors[Ch][sb] == 0) {
                StreamInfo->bitNeed[0][sb] = (S8)-5;
            } else {
                if (StreamInfo->numSubBands == 4) {
                    loudness = StreamInfo->scale_factors[Ch][sb] - 
                               LoudnessOffset4[StreamInfo->sampleFreq][sb];
                } else {
                    loudness = StreamInfo->scale_factors[Ch][sb] - 
                               LoudnessOffset8[StreamInfo->sampleFreq][sb];
                }
                if (loudness > 0) {
                    /* Divide by 2 */
                    StreamInfo->bitNeed[0][sb] = loudness >> 1;
                } else {
                    StreamInfo->bitNeed[0][sb] = loudness;
                }
            }
        }
    }

    /* Calculate Max Bitneed */
    for (sb = 0; sb < StreamInfo->numSubBands; sb++) {
        if (StreamInfo->bitNeed[0][sb] > maxBitNeed) {
            maxBitNeed = StreamInfo->bitNeed[0][sb];
        }
    }

    /* Calculate bitslices */
    bitSlice = maxBitNeed + 1;
    do {
        bitSlice--;
        bitCount += sliceCount;
        sliceCount = 0;
        for (sb = 0; sb < StreamInfo->numSubBands; sb++) {
            if ((StreamInfo->bitNeed[0][sb] > (bitSlice + 1)) && 
                (StreamInfo->bitNeed[0][sb] < (bitSlice + 16))) {
                sliceCount++;
            } else if (StreamInfo->bitNeed[0][sb] == (bitSlice + 1)) {
                sliceCount += 2;
            }
        }
    } while ((bitCount + sliceCount) < StreamInfo->bitPool);

    if ((bitCount + sliceCount) == StreamInfo->bitPool) {
        bitCount += sliceCount;
        bitSlice--;
    }

    /* Distribute the bits */
    for (sb = 0; sb < StreamInfo->numSubBands; sb++) {
        if (StreamInfo->bitNeed[0][sb] < (bitSlice + 2)) {
            StreamInfo->bits[Ch][sb] = 0;
        } else {
            StreamInfo->bits[Ch][sb] = 
                min(StreamInfo->bitNeed[0][sb] - bitSlice, 16);
        }
    }

    /* Allocate remaining bits */
    sb = 0;
    while ((bitCount < StreamInfo->bitPool) && (sb < StreamInfo->numSubBands)) {
        if ((StreamInfo->bits[Ch][sb] >= 2) && (StreamInfo->bits[Ch][sb] < 16)) {
            StreamInfo->bits[Ch][sb]++;
            bitCount++;
        } else if ((StreamInfo->bitNeed[0][sb] == (bitSlice + 1)) && 
                   (StreamInfo->bitPool > (bitCount + 1))) {
            StreamInfo->bits[Ch][sb] = 2;
            bitCount += 2;
        }
        sb++;
    }

    sb = 0;
    while ((bitCount < StreamInfo->bitPool) && (sb < StreamInfo->numSubBands)) {
        if (StreamInfo->bits[Ch][sb] < 16) {
            StreamInfo->bits[Ch][sb]++;
            bitCount++;
        }
        sb++;
    }
}

/*---------------------------------------------------------------------------
 *            SbcStereoBitAlloc()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Performs bit allocation for stereo streams.  Both channels are
 *            processed together.  
 */
static void SbcStereoBitAlloc(SbcStreamInfo *StreamInfo)
{
    U8  sb, ch;
    S8  loudness;
    S8  maxBitNeed = 0;
    U8  bitCount = 0;
    U8  sliceCount = 0;
    S8  bitSlice;

    if (StreamInfo->allocMethod == SBC_ALLOC_METHOD_SNR) {
        /* SNR allocation method */
        for (ch = 0; ch < 2; ch++) {
            for (sb = 0; sb < StreamInfo->numSubBands; sb++) {
                StreamInfo->bitNeed[ch][sb] = StreamInfo->scale_factors[ch][sb];
            }
        }
    } else {
        /* Loudness allocation method */
        for (ch = 0; ch < 2; ch++) {
            for (sb = 0; sb < StreamInfo->numSubBands; sb++) {
                if (StreamInfo->scale_factors[ch][sb] == 0) {
                    StreamInfo->bitNeed[ch][sb] = (S8)-5;
                } else {
                    if (StreamInfo->numSubBands == 4) {
                        loudness = StreamInfo->scale_factors[ch][sb] - 
                                   LoudnessOffset4[StreamInfo->sampleFreq][sb];
                    } else {
                        loudness = StreamInfo->scale_factors[ch][sb] - 
                                   LoudnessOffset8[StreamInfo->sampleFreq][sb];
                    }
                    if (loudness > 0) {
                        /* Divide by 2 */
                        StreamInfo->bitNeed[ch][sb] = loudness >> 1;
                    } else {
                        StreamInfo->bitNeed[ch][sb] = loudness;
                    }
                }
            }
        }
    }

    /* Calculate Max Bitneed */
    for (ch = 0; ch < 2; ch++) {
        for (sb = 0; sb < StreamInfo->numSubBands; sb++) {
            if (StreamInfo->bitNeed[ch][sb] > maxBitNeed) {
                maxBitNeed = StreamInfo->bitNeed[ch][sb];
            }
        }
    }

    /* Calculate bitslices */
    bitSlice = maxBitNeed + 1;
    do {
        bitSlice--;
        bitCount += sliceCount;
        sliceCount = 0;
        for (ch = 0; ch < 2; ch++) {
            for (sb = 0; sb < StreamInfo->numSubBands; sb++) {
                if ((StreamInfo->bitNeed[ch][sb] > (bitSlice + 1)) && 
                    (StreamInfo->bitNeed[ch][sb] < (bitSlice + 16))) {
                    sliceCount++;
                } else if (StreamInfo->bitNeed[ch][sb] == bitSlice + 1) {
                    sliceCount += 2;
                }
            }
        }
    } while ((bitCount + sliceCount) < StreamInfo->bitPool);

    if ((bitCount + sliceCount) == StreamInfo->bitPool) {
        bitCount += sliceCount;
        bitSlice--;
    }

    /* Distribute the bits */
    for (ch = 0; ch < 2; ch++) {
        for (sb = 0; sb < StreamInfo->numSubBands; sb++) {
            if (StreamInfo->bitNeed[ch][sb] < (bitSlice + 2)) {
                StreamInfo->bits[ch][sb] = 0;
            } else {
                StreamInfo->bits[ch][sb] = 
                    min(StreamInfo->bitNeed[ch][sb] - bitSlice, 16);
            }
        }
    }

    /* Allocate remaining bits */
    sb = 0;
    ch = 0;
    while ((bitCount < StreamInfo->bitPool) && (sb < StreamInfo->numSubBands)) {
        if ((StreamInfo->bits[ch][sb] >= 2) && (StreamInfo->bits[ch][sb] < 16)) {
            StreamInfo->bits[ch][sb]++;
            bitCount++;
        } else if ((StreamInfo->bitNeed[ch][sb] == (bitSlice + 1)) && 
                   (StreamInfo->bitPool > (bitCount + 1))) {
            StreamInfo->bits[ch][sb] = 2;
            bitCount += 2;
        }
        if (ch == 1) {
            ch = 0;
            sb++;
        } else {
            ch = 1;
        }
    }

    sb = 0;
    ch = 0;
    while ((bitCount < StreamInfo->bitPool) && (sb < StreamInfo->numSubBands)) {
        if (StreamInfo->bits[ch][sb] < 16) {
            StreamInfo->bits[ch][sb]++;
            bitCount++;
        }
        if (ch == 1) {
            ch = 0;
            sb++;
        } else {
            ch = 1;
        }
    }
}

#if SBC_DECODER == XA_ENABLED
/*---------------------------------------------------------------------------
 *            SBC_InitDecoder()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initializes a Decoder structure for processing a new stream.  
 *            Must be called before each new stream.
 */
void SBC_InitDecoder(SbcDecoder *Decoder)
{
    OS_MemSet((U8*)Decoder, 0, sizeof(SbcDecoder));
}

/*---------------------------------------------------------------------------
 *            SbcParseHeader()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Parses an SBC header for frame information.
 */
static U16 SbcParseHeader(SbcDecoder *Decoder, U8 *Buffer)
{
    U8  i;
    U8 *ptr = Buffer;
    U8  numBytes;

    /* Initialize FCS */
    Decoder->streamInfo.fcs = 0x0F;

    /* Checksum first byte */
    Decoder->streamInfo.fcs = SbcCrcTable[Decoder->streamInfo.fcs ^ *ptr];

    /* Sampling Frequency */
    Decoder->streamInfo.sampleFreq = *ptr >> 6;

    /* Number of blocks */
    switch ((*ptr >> 4) & 0x03) {
    case 0:
        Decoder->streamInfo.numBlocks = 4;
        break;
    case 1:
        Decoder->streamInfo.numBlocks = 8;
        break;
    case 2:
        Decoder->streamInfo.numBlocks = 12;
        break;
    case 3:
        Decoder->streamInfo.numBlocks = 16;
        break;
    default:
        return 0;
    }

    /* Channel mode and number of Decoders */
    Decoder->streamInfo.channelMode =  ((*ptr >> 2) & 0x03);
    switch (Decoder->streamInfo.channelMode) {
    case 0:
        Decoder->streamInfo.numChannels = 1;
        break;
    case 1:
    case 2:
    case 3:
        Decoder->streamInfo.numChannels = 2;
        break;
    default:
        return 0;
    }

    /* Allocation Method */
    Decoder->streamInfo.allocMethod = (*ptr >> 1) & 0x01;

    /* Subbands */
    switch (*ptr++ & 0x01) {
    case 0:
        Decoder->streamInfo.numSubBands = 4;
        break;
    case 1:
        Decoder->streamInfo.numSubBands = 8;
        break;
    default:
        return 0;
    }

    /* Checksum second byte */
    Decoder->streamInfo.fcs = SbcCrcTable[Decoder->streamInfo.fcs ^ *ptr];

    /* Bitpool */
    Decoder->streamInfo.bitPool = *ptr++;

    /* CRC */
    Decoder->streamInfo.crc = *ptr++;
    Decoder->streamInfo.bitOffset = 24;
    numBytes = 3;

    /* Join */
    if (Decoder->streamInfo.channelMode == SBC_CHNL_MODE_JOINT_STEREO) {

        for (i = 0; i < Decoder->streamInfo.numSubBands - 1; i++) {
            Decoder->streamInfo.join[i] = (*ptr >> (7 - i)) & 0x01;
            Decoder->streamInfo.bitOffset++;
        }
        Decoder->streamInfo.join[i] = 0;
        Decoder->streamInfo.bitOffset++;
        if (Decoder->streamInfo.bitOffset == 32) {
            numBytes = 4;

            /* Checksum fourth byte */
            Decoder->streamInfo.fcs = SbcCrcTable[Decoder->streamInfo.fcs ^ *ptr];

        } else if (Decoder->streamInfo.bitOffset != 28) {
            numBytes = 0;
        }
    }

    /* Return number of complete bytes processed */
    return numBytes;
}

/*---------------------------------------------------------------------------
 *            SbcParseScaleFactors()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Parses an SBC header for scale factors.
 */
static void SbcParseScaleFactors(SbcDecoder *Decoder, U8 *Buffer)
{
    U8  ch, sb;
    U8 *ptr = Buffer;

    /* Scaling Factors */
    for (ch = 0; ch < Decoder->streamInfo.numChannels; ch++) {
        for (sb = 0; sb < Decoder->streamInfo.numSubBands; sb++) {
            if (Decoder->streamInfo.bitOffset % 8) {

                /* Sum the whole byte */
                Decoder->streamInfo.fcs = 
                SbcCrcTable[Decoder->streamInfo.fcs ^ *ptr];

                Decoder->streamInfo.scale_factors[ch][sb] = *ptr++ & 0x0F;
            } else {

                if ((ch == Decoder->streamInfo.numChannels - 1) &&
                    (sb == Decoder->streamInfo.numSubBands - 1)) {

                    /* Sum the next 4 bitys */
                    SbcCrcSum4(&Decoder->streamInfo, *ptr);
                }

                Decoder->streamInfo.scale_factors[ch][sb] = *ptr >> 4;
            }

            Decoder->streamInfo.scaleFactors[ch][sb] =
                (1 << (U32)(Decoder->streamInfo.scale_factors[ch][sb] + 1));

            Decoder->streamInfo.bitOffset += 4;
        }
    }
}

/*---------------------------------------------------------------------------
 *            SbcUnpackSample()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Unpacks an audio sample from the stream.  Samples are extracted
 *            from the stream based on the bits required for the current
 *            subband.
 */
static U16 SbcUnpackSample(SbcDecoder *Decoder, U8 **Buffer, U8 Ch, U8 Sb)
{
    U16 sample = 0;
    U8  *ptr;
    U8  bit;
    U8  bitsLeft = Decoder->streamInfo.bits[Ch][Sb];;

    do {
        ptr = *Buffer;
        bit = 8 - Decoder->streamInfo.bitOffset % 8;

        if (bitsLeft > bit) {
            /* The bits are split over multiple bytes */
            sample += *ptr & (0xFF >> (8 - bit));
            bitsLeft -= bit;
            Decoder->streamInfo.bitOffset += bit;
            sample = sample << min(8, bitsLeft);
            (*Buffer)++;
        } else {
            /* The bits are all in this byte */
            sample += (*ptr >> (bit - bitsLeft)) & (0xFF >> (8 - bitsLeft));
            Decoder->streamInfo.bitOffset += bitsLeft;
            if (bitsLeft == bit) {
                (*Buffer)++;
            }
            bitsLeft = 0;
        }
    } while (bitsLeft);

    return sample;
}

/*---------------------------------------------------------------------------
 *            SbcBuildSubbandSamples()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Unpacks samples from the stream.
 */
static void SbcBuildSubbandSamples(SbcDecoder *Decoder, U8 *Buffer)
{
    U8 ch, sb, blk;

    /* Calculate levels */
    for (ch = 0; ch < Decoder->streamInfo.numChannels; ch++) {
        for (sb = 0; sb < Decoder->streamInfo.numSubBands; sb++) {
            Decoder->streamInfo.levels[ch][sb] = 
                (1 << Decoder->streamInfo.bits[ch][sb]) - 1;
        }
    }

    /* Construct subband samples */
    for (blk = 0; blk < Decoder->streamInfo.numBlocks; blk++) {
        for (ch = 0; ch < Decoder->streamInfo.numChannels; ch++) {
            for (sb = 0; sb < Decoder->streamInfo.numSubBands; sb++) {
                if (Decoder->streamInfo.levels[ch][sb] > 0) {
                    Decoder->streamInfo.sbSample[blk][ch][sb] =
                      Decoder->streamInfo.scaleFactors[ch][sb] *
                      (Div((SbcUnpackSample(Decoder, &Buffer, ch, sb) << 1) + 1,
                           Decoder->streamInfo.levels[ch][sb]) - ONE_F);
                } else {
                    Decoder->streamInfo.sbSample[blk][ch][sb] = 0;
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------
 *            SbcJointProcessing()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Performs joint processing on streams encoded as joint stereo.
 */
static void SbcJointProcessing(SbcDecoder *Decoder)
{
    U8 blk, sb;

    for (blk = 0; blk < Decoder->streamInfo.numBlocks; blk++) {
        for (sb = 0; sb < Decoder->streamInfo.numSubBands; sb++) {
            if (Decoder->streamInfo.join[sb] == 1) {
                Decoder->streamInfo.sbSample[blk][0][sb] = 
                    Decoder->streamInfo.sbSample[blk][0][sb] + 
                    Decoder->streamInfo.sbSample[blk][1][sb];
                Decoder->streamInfo.sbSample[blk][1][sb] = 
                    Decoder->streamInfo.sbSample[blk][0][sb] - 
                    2 * Decoder->streamInfo.sbSample[blk][1][sb];
            }
        }
    }

}

/*---------------------------------------------------------------------------
 *            SbcSynthesisFilter4()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Performs synthesis of the unpacked stream samples with 4
 *            subbands.  Creates the PCM output.
 */
static void SbcSynthesisFilter4(SbcDecoder *Decoder, SbcPcmData *PcmData)
{
    U8    i, j, k, blk, ch, *ptr;
    S16   pcm;
    REAL  X[8];

    for (blk = 0; blk < Decoder->streamInfo.numBlocks; blk++) {
        for (ch = 0; ch < Decoder->streamInfo.numChannels; ch++) {

            /* Shifting */
            for (i = 79; i > 7; i--) {
                Decoder->V[ch][i] = Decoder->V[ch][i - 8];
            }

            for (i = 8; i > 0; i--) {
                Decoder->V[ch][i - 1] = 0;
            }

            /* Matrixing */
            for (k = 0; k < 8; k++) {
                for (i = 0; i < 4; i++) {
                    Decoder->V[ch][k] += 
                    MulP(Decoder->streamInfo.sbSample[blk][ch][i],
                         Synth4[k][i]);
                }
            }

            /* Build a 40 values vector U */

            /* i = 0 */
            Decoder->U[0]   = Decoder->V[ch][0];
            Decoder->U[4]   = Decoder->V[ch][12];
            Decoder->U[1]   = Decoder->V[ch][1];
            Decoder->U[5]   = Decoder->V[ch][13];
            Decoder->U[2]   = Decoder->V[ch][2];
            Decoder->U[6]   = Decoder->V[ch][14];
            Decoder->U[3]   = Decoder->V[ch][3];
            Decoder->U[7]   = Decoder->V[ch][15];

            /* i = 1 */
            Decoder->U[8]   = Decoder->V[ch][16];
            Decoder->U[12]  = Decoder->V[ch][28];
            Decoder->U[9]   = Decoder->V[ch][17];
            Decoder->U[13]  = Decoder->V[ch][29];
            Decoder->U[10]  = Decoder->V[ch][18];
            Decoder->U[14]  = Decoder->V[ch][30];
            Decoder->U[11]  = Decoder->V[ch][19];
            Decoder->U[15]  = Decoder->V[ch][31];

            /* i = 2 */
            Decoder->U[16]  = Decoder->V[ch][32];
            Decoder->U[20]  = Decoder->V[ch][44];
            Decoder->U[17]  = Decoder->V[ch][33];
            Decoder->U[21]  = Decoder->V[ch][45];
            Decoder->U[18]  = Decoder->V[ch][34];
            Decoder->U[22]  = Decoder->V[ch][46];
            Decoder->U[19]  = Decoder->V[ch][35];
            Decoder->U[23]  = Decoder->V[ch][47];

            /* i = 3 */
            Decoder->U[24]  = Decoder->V[ch][48];
            Decoder->U[28]  = Decoder->V[ch][60];
            Decoder->U[25]  = Decoder->V[ch][49];
            Decoder->U[29]  = Decoder->V[ch][61];
            Decoder->U[26]  = Decoder->V[ch][50];
            Decoder->U[30]  = Decoder->V[ch][62];
            Decoder->U[27]  = Decoder->V[ch][51];
            Decoder->U[31]  = Decoder->V[ch][63];

            /* i = 4 */
            Decoder->U[32]  = Decoder->V[ch][64];
            Decoder->U[36]  = Decoder->V[ch][76];
            Decoder->U[33]  = Decoder->V[ch][65];
            Decoder->U[37]  = Decoder->V[ch][77];
            Decoder->U[34]  = Decoder->V[ch][66];
            Decoder->U[38]  = Decoder->V[ch][78];
            Decoder->U[35]  = Decoder->V[ch][67];
            Decoder->U[39]  = Decoder->V[ch][79];

            /* Window by 40 coefficients */
            for (i = 0; i < 40; i++) {
                Decoder->W[i] = MulP(Decoder->U[i],
                                     SbcDecodeCoeff4[i]);
            }

            /* Calculate 4 audio samples */
            ptr = PcmData->data + PcmData->dataLen + (ch << 1);
            for (j = 0; j < 4; j++) {
                X[j] = 0;

                X[j] += Decoder->W[j];
                X[j] += Decoder->W[j+4];
                X[j] += Decoder->W[j+8];
                X[j] += Decoder->W[j+12];
                X[j] += Decoder->W[j+16];
                X[j] += Decoder->W[j+20];
                X[j] += Decoder->W[j+24];
                X[j] += Decoder->W[j+28];
                X[j] += Decoder->W[j+32];
                X[j] += Decoder->W[j+36];

                pcm = RealtoS16(X[j]);
                StorePCM16(ptr, (S16)pcm);

                if (Decoder->streamInfo.numChannels == 1) {
                    ptr += 2;
                } else {
                    ptr += 4;
                }
            }
        }
        PcmData->dataLen += 4 * ch << 1;
    }
}

/*---------------------------------------------------------------------------
 *            SbcSynthesisFilter8()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Performs synthesis of the unpacked stream samples with 8
 *            subbands.  Creates the PCM output.
 */
static void SbcSynthesisFilter8(SbcDecoder *Decoder, SbcPcmData *PcmData)
{
    U8 i, j, k, blk, ch, *ptr;
    S16 pcm;
    REAL X[8];

    for (blk = 0; blk < Decoder->streamInfo.numBlocks; blk++) {
        for (ch = 0; ch < Decoder->streamInfo.numChannels; ch++) {

            /* Shifting */
            for (i = 159; i > 15; i--) {
                Decoder->V[ch][i] = Decoder->V[ch][i - 16];
            }

            for (i = 16; i > 0; i--) {
                Decoder->V[ch][i - 1] = 0;
            }

            /* Matrixing */
            for (k = 0; k < 16; k++) {
                for (i = 0; i < 8; i++) {
                    Decoder->V[ch][k] += 
                    MulP(Decoder->streamInfo.sbSample[blk][ch][i],
                         Synth8[k][i]);
                }
            }

            /* Build a 80 values vector U */

            /* i = 0 */
            Decoder->U[0]  = Decoder->V[ch][0];
            Decoder->U[8]  = Decoder->V[ch][24];
            Decoder->U[1]  = Decoder->V[ch][1];
            Decoder->U[9]  = Decoder->V[ch][25];
            Decoder->U[2]  = Decoder->V[ch][2];
            Decoder->U[10] = Decoder->V[ch][26];
            Decoder->U[3]  = Decoder->V[ch][3];
            Decoder->U[11] = Decoder->V[ch][27];
            Decoder->U[4]  = Decoder->V[ch][4];
            Decoder->U[12] = Decoder->V[ch][28];
            Decoder->U[5]  = Decoder->V[ch][5];
            Decoder->U[13] = Decoder->V[ch][29];
            Decoder->U[6]  = Decoder->V[ch][6];
            Decoder->U[14] = Decoder->V[ch][30];
            Decoder->U[7]  = Decoder->V[ch][7];
            Decoder->U[15] = Decoder->V[ch][31];

            /* i = 1 */
            Decoder->U[16] = Decoder->V[ch][32];
            Decoder->U[24] = Decoder->V[ch][56];
            Decoder->U[17] = Decoder->V[ch][33];
            Decoder->U[25] = Decoder->V[ch][57];
            Decoder->U[18] = Decoder->V[ch][34];
            Decoder->U[26] = Decoder->V[ch][58];
            Decoder->U[19] = Decoder->V[ch][35];
            Decoder->U[27] = Decoder->V[ch][59];
            Decoder->U[20] = Decoder->V[ch][36];
            Decoder->U[28] = Decoder->V[ch][60];
            Decoder->U[21] = Decoder->V[ch][37];
            Decoder->U[29] = Decoder->V[ch][61];
            Decoder->U[22] = Decoder->V[ch][38];
            Decoder->U[30] = Decoder->V[ch][62];
            Decoder->U[23] = Decoder->V[ch][39];
            Decoder->U[31] = Decoder->V[ch][63];

            /* i = 2 */
            Decoder->U[32] = Decoder->V[ch][64];
            Decoder->U[40] = Decoder->V[ch][88];
            Decoder->U[33] = Decoder->V[ch][65];
            Decoder->U[41] = Decoder->V[ch][89];
            Decoder->U[34] = Decoder->V[ch][66];
            Decoder->U[42] = Decoder->V[ch][90];
            Decoder->U[35] = Decoder->V[ch][67];
            Decoder->U[43] = Decoder->V[ch][91];
            Decoder->U[36] = Decoder->V[ch][68];
            Decoder->U[44] = Decoder->V[ch][92];
            Decoder->U[37] = Decoder->V[ch][69];
            Decoder->U[45] = Decoder->V[ch][93];
            Decoder->U[38] = Decoder->V[ch][70];
            Decoder->U[46] = Decoder->V[ch][94];
            Decoder->U[39] = Decoder->V[ch][71];
            Decoder->U[47] = Decoder->V[ch][95];

            /* i = 3 */
            Decoder->U[48] = Decoder->V[ch][96];
            Decoder->U[56] = Decoder->V[ch][120];
            Decoder->U[49] = Decoder->V[ch][97];
            Decoder->U[57] = Decoder->V[ch][121];
            Decoder->U[50] = Decoder->V[ch][98];
            Decoder->U[58] = Decoder->V[ch][122];
            Decoder->U[51] = Decoder->V[ch][99];
            Decoder->U[59] = Decoder->V[ch][123];
            Decoder->U[52] = Decoder->V[ch][100];
            Decoder->U[60] = Decoder->V[ch][124];
            Decoder->U[53] = Decoder->V[ch][101];
            Decoder->U[61] = Decoder->V[ch][125];
            Decoder->U[54] = Decoder->V[ch][102];
            Decoder->U[62] = Decoder->V[ch][126];
            Decoder->U[55] = Decoder->V[ch][103];
            Decoder->U[63] = Decoder->V[ch][127];

            /* i = 4 */
            Decoder->U[64] = Decoder->V[ch][128];
            Decoder->U[72] = Decoder->V[ch][152];
            Decoder->U[65] = Decoder->V[ch][129];
            Decoder->U[73] = Decoder->V[ch][153];
            Decoder->U[66] = Decoder->V[ch][130];
            Decoder->U[74] = Decoder->V[ch][154];
            Decoder->U[67] = Decoder->V[ch][131];
            Decoder->U[75] = Decoder->V[ch][155];
            Decoder->U[68] = Decoder->V[ch][132];
            Decoder->U[76] = Decoder->V[ch][156];
            Decoder->U[69] = Decoder->V[ch][133];
            Decoder->U[77] = Decoder->V[ch][157];
            Decoder->U[70] = Decoder->V[ch][134];
            Decoder->U[78] = Decoder->V[ch][158];
            Decoder->U[71] = Decoder->V[ch][135];
            Decoder->U[79] = Decoder->V[ch][159];

            /* Window by 80 coefficients */
            for (i = 0; i < 80; i++) {
                Decoder->W[i] = MulP(Decoder->U[i],
                                     SbcDecodeCoeff8[i]);
            }

            /* Calculate 8 audio samples */
            ptr = PcmData->data + PcmData->dataLen + (ch << 1);
            for (j = 0; j < 8; j++) {
                X[j] = 0;

                X[j] += Decoder->W[j];
                X[j] += Decoder->W[j+8];
                X[j] += Decoder->W[j+16];
                X[j] += Decoder->W[j+24];
                X[j] += Decoder->W[j+32];
                X[j] += Decoder->W[j+40];
                X[j] += Decoder->W[j+48];
                X[j] += Decoder->W[j+56];
                X[j] += Decoder->W[j+64];
                X[j] += Decoder->W[j+72];

                pcm = RealtoS16(X[j]);
                StorePCM16(ptr, (S16)pcm);

                if (Decoder->streamInfo.numChannels == 1) {
                    ptr += 2;
                } else {
                    ptr += 4;
                }
            }
        }
        PcmData->dataLen += 8 * ch << 1;
    }
}

/*---------------------------------------------------------------------------
 *            SbcResetDecoderState()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Resets the parsing state for the Decoder.
 */
static void SbcResetDecoderState(SbcDecoder *Decoder)
{
    Decoder->streamInfo.bitOffset = 0;
    Decoder->parser.rxState = SBC_PARSE_SYNC;
    Decoder->parser.stageLen = 0;
    Decoder->parser.curStageOff = 0;
}

/*---------------------------------------------------------------------------
 *            SbcMuteDecoder()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Mutes the current frame based on the length of PCM data in the
 *            previous frame.  This is used when an error occurs during the
 *            decode.
 */
static void SbcMuteFrame(SbcDecoder *Decoder, SbcPcmData *PcmData)
{
    OS_MemSet(PcmData->data, 0, Decoder->maxPcmLen);
    PcmData->dataLen = Decoder->maxPcmLen;
    SbcResetDecoderState(Decoder);
}

/*---------------------------------------------------------------------------
 *            SBC_DecodeFrames()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Decodes SBC frames from the data stream.
 */
XaStatus SBC_DecodeFrames(SbcDecoder *Decoder, U8 *Buff, U16 Len, 
                          U16 *BytesDecoded, SbcPcmData *PcmData, 
                          U16 MaxPcmData)
{
    U16      n;
    U16      i;
    U16      frameBits;
    XaStatus status = XA_STATUS_CONTINUE;

    /* Store the receive buffer and length */
    *BytesDecoded = Len;
    Decoder->parser.rxBuff = Buff;

    /* Process the data in Server->parser.rxBuff of length Len */
    while (Len > 0) {
        Assert(Decoder->parser.stageLen <= SBC_MAX_PCM_DATA);
        Assert((Decoder->parser.curStageOff == 0) ||
               (Decoder->parser.curStageOff < Decoder->parser.stageLen));

        n = min((U16)(Decoder->parser.stageLen - Decoder->parser.curStageOff),
                Len);
        Assert(n <= SBC_MAX_PCM_DATA);

        /* Stage the data */
        for (i = Decoder->parser.curStageOff; n > 0; n--, i++) {
            Decoder->parser.stageBuff[i] = *(Decoder->parser.rxBuff);
            (Decoder->parser.rxBuff)++;
            Len--;
            Decoder->parser.curStageOff++;
        }

        /* Only call the state machine if the data has been completely
         * staged.
         */
        if (Decoder->parser.curStageOff == Decoder->parser.stageLen) {

            /* Execute the correct state */
            switch (Decoder->parser.rxState) {
            case SBC_PARSE_SYNC: /* 0 bytes staged */
                /* Sync with the beginning of the frame */
                do {
                    Len--;
                    if (*Decoder->parser.rxBuff == SBC_SYNC_WORD) {
                        /* Found the sync word, advance state */
                        Decoder->parser.rxState = SBC_PARSE_HEADER;
                        Decoder->parser.stageLen = 4;
                        Decoder->parser.rxBuff++;
                        break;
                    }
                    Decoder->parser.rxBuff++;
                } while (Len > 0);
                break;
            case SBC_PARSE_HEADER: /* 4 bytes staged */
                switch (SbcParseHeader(Decoder, Decoder->parser.stageBuff)) {
                case 3:
                    /* Processed 3 of the 4 complete bytes */
                    (Decoder->parser.rxBuff)--;
                    Len++;
                    /* Drop through */
                case 4:
                    /* Advance the state */
                    Decoder->parser.rxState = SBC_PARSE_SCALE_FACTORS;
                    Decoder->parser.stageLen = Decoder->streamInfo.numChannels * 
                                            Decoder->streamInfo.numSubBands / 2;
                    if (Decoder->streamInfo.bitOffset % 8) {
                        /* Processed a partial byte, need to stage 1 more byte */
                        Decoder->parser.stageLen++;
                    }

                    Decoder->maxPcmLen = Decoder->streamInfo.numChannels * 
                                         Decoder->streamInfo.numBlocks * 
                                         Decoder->streamInfo.numSubBands * 2;

                    if (PcmData == 0) {
                        /* Just getting header info */
                        Len = *BytesDecoded;
                        status = XA_STATUS_SUCCESS;
                        SbcResetDecoderState(Decoder);
                        goto exit;
                    }

                    PcmData->numChannels = Decoder->streamInfo.numChannels;
                    PcmData->sampleFreq = Decoder->streamInfo.sampleFreq;
                    break;
                default:
                    /* Couldn't process the header, skip this Decoder */
                    status = XA_STATUS_FAILED;
                    Report(("SBC: Invalid header"));
                    SbcMuteFrame(Decoder, PcmData);
                    goto exit;
                }
                break;
            case SBC_PARSE_SCALE_FACTORS:
                /* Get the scale factors */
                SbcParseScaleFactors(Decoder, Decoder->parser.stageBuff);

                /* Compare the calculated crc against the one in the bitstream */
                if (Decoder->streamInfo.fcs != Decoder->streamInfo.crc) {
                    status = XA_STATUS_FAILED;
                    Report(("SBC: Crc error!"));
                    SbcMuteFrame(Decoder, PcmData);
                    goto exit;
                }

                /* Calculate Bit Allocation */
                switch (Decoder->streamInfo.channelMode) {
                case SBC_CHNL_MODE_MONO:
                    SbcMonoBitAlloc(&Decoder->streamInfo, 0);
                    break;
                case SBC_CHNL_MODE_DUAL_CHNL:
                    SbcMonoBitAlloc(&Decoder->streamInfo, 0);
                    SbcMonoBitAlloc(&Decoder->streamInfo, 1);
                    break;
                case SBC_CHNL_MODE_STEREO:
                case SBC_CHNL_MODE_JOINT_STEREO:
                    SbcStereoBitAlloc(&Decoder->streamInfo);
                    break;
                default:
                    /* Couldn't process the scale factors, skip this frame */
                    status = XA_STATUS_FAILED;
                    Report(("SBC: Invalid channel mode\n"));
                    SbcMuteFrame(Decoder, PcmData);
                    goto exit;
                }

                /* Advance the state */
                Decoder->parser.rxState = SBC_PARSE_SAMPLES;

                /* Calculate the number of bits left in the frame */
                if ((Decoder->streamInfo.channelMode == SBC_CHNL_MODE_MONO) ||
                    (Decoder->streamInfo.channelMode == SBC_CHNL_MODE_DUAL_CHNL)) {
                    frameBits = Decoder->streamInfo.numBlocks * 
                                Decoder->streamInfo.numChannels * 
                                Decoder->streamInfo.bitPool;
                } else {
                    frameBits = Decoder->streamInfo.numBlocks * 
                                Decoder->streamInfo.bitPool;
                }

                /* Set the stage length */
                Decoder->parser.stageLen = frameBits / 8;
                if (frameBits % 8) {
                    /* Need one more byte to get all the bits */
                    Decoder->parser.stageLen++;
                }

                /* Adjust pointer if starting in the middle of a byte */
                if (Decoder->streamInfo.bitOffset % 8) {
                    /* Processed a partial byte, need to back up 1 */
                    (Decoder->parser.rxBuff)--;
                    Len++;
                    Decoder->parser.stageLen++;
                }
                break;
            case SBC_PARSE_SAMPLES:
                /* Unpack subband samples */
                SbcBuildSubbandSamples(Decoder, Decoder->parser.stageBuff);

                /* Perform joint processing if necessary */
                if (Decoder->streamInfo.channelMode == 
                    SBC_CHNL_MODE_JOINT_STEREO) {
                    SbcJointProcessing(Decoder);
                }

                /* Run samples through the synthesis filter */
                switch (Decoder->streamInfo.numSubBands) {
                case 4:
                    SbcSynthesisFilter4(Decoder, PcmData);
                    break;
                case 8:
                    SbcSynthesisFilter8(Decoder, PcmData);
                    break;
                }

                /* Start on next Frame */
                if ((MaxPcmData - PcmData->dataLen) >= Decoder->maxPcmLen) {
                    /* Decode next Frame */
                    SbcResetDecoderState(Decoder);
                } else {
                    /* PCM data block filled, return success */
                    status = XA_STATUS_SUCCESS;
                    SbcResetDecoderState(Decoder);
                    goto exit;
                }
            }

            Decoder->parser.curStageOff = 0;
        }
    }

    exit:

    *BytesDecoded -= Len;
    return status;
}
#endif

#if SBC_ENCODER == XA_ENABLED
/*---------------------------------------------------------------------------
 *            SBC_InitEncoder()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initializes an Encoder structure for processing a new stream.  
 *            Must be called before each new stream is encoded.
 */
void SBC_InitEncoder(SbcEncoder *Encoder)
{
    OS_MemSet((U8*)Encoder, 0, sizeof(SbcEncoder));
}

/*---------------------------------------------------------------------------
 *            SbcAnalysisFilter4()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Performs analysis of PCM data and creates 4 subband samples.
 */
static void SbcAnalysisFilter4(SbcEncoder *Encoder, SbcPcmData *PcmData)
{
    U8 i, k, blk, ch, *ptr;
    U16 offset = 0;

    for (blk = 0; blk < Encoder->streamInfo.numBlocks; blk++) {

        for (ch = 0; ch < Encoder->streamInfo.numChannels; ch++) {

            /* Initialize the pointer to current PCM data */
            ptr = PcmData->data + offset + (ch << 1);

            /* Shift */
            for (i = 39; i > 3; i--) {
                Encoder->X[ch][i] = Encoder->X[ch][i-4];
            }

            /* Input Samples */
            for (i = 4; i > 0; i--) {
                Encoder->X[ch][i-1] = PCMtoHost16(ptr);
                if (Encoder->streamInfo.numChannels == 1) {
                    ptr += 2;
                } else {
                    ptr += 4;
                }
            }

            /* Windowing by 40 Coefficients */
            for (i = 0; i < 40; i++) {
                Encoder->Z[i] = MulP(S16toReal(Encoder->X[ch][i]),
                                     SbcCoefficient4[i]);
            }

            /* Partial Calculation */
            for (i = 0; i < 8; i++) {
                Encoder->Y[i] = 0;

                Encoder->Y[i] += Encoder->Z[i];
                Encoder->Y[i] += Encoder->Z[i+8];
                Encoder->Y[i] += Encoder->Z[i+16];
                Encoder->Y[i] += Encoder->Z[i+24];
                Encoder->Y[i] += Encoder->Z[i+32];
            }

            /* Calculate subbands by matrixing */
            for (i = 0; i < 4; i++) {
                Encoder->streamInfo.sbSample[blk][ch][i] = 0;
                for (k = 0; k < 8; k++) {
                    Encoder->streamInfo.sbSample[blk][ch][i] += 
                    MulP(Encoder->Y[k],
                         Analyze4[k][i]);
                }
            }
        }

        /* Go to the next set of samples */
        offset += 8 * ch;
    }
}

/*---------------------------------------------------------------------------
 *            SbcAnalysisFilter8()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Performs analysis of PCM data and creates 8 subband samples.
 */
static void SbcAnalysisFilter8(SbcEncoder *Encoder, SbcPcmData *PcmData)
{
    U8 i, k, blk, ch, *ptr;
    U16 offset = 0;

    for (blk = 0; blk < Encoder->streamInfo.numBlocks; blk++) {

        for (ch = 0; ch < Encoder->streamInfo.numChannels; ch++) {

            /* Initialize the pointer to current PCM data */
            ptr = PcmData->data + offset + (ch << 1);

            /* Shift */
            for (i = 79; i > 7; i--) {
                Encoder->X[ch][i] = Encoder->X[ch][i-8];
            }

            /* Input Samples */
            for (i = 8; i > 0; i--) {
                Encoder->X[ch][i-1] = PCMtoHost16(ptr);
                if (Encoder->streamInfo.numChannels == 1) {
                    ptr += 2;
                } else {
                    ptr += 4;
                }
            }

            /* Windowing by 80 Coefficients */
            for (i = 0; i < 80; i++) {
                Encoder->Z[i] = MulP(S16toReal(Encoder->X[ch][i]), 
                                     SbcCoefficient8[i]);
            }

            /* Partial Calculation */
            for (i = 0; i < 16; i++) {

                Encoder->Y[i] = 0;

                Encoder->Y[i] += Encoder->Z[i];
                Encoder->Y[i] += Encoder->Z[i+16];
                Encoder->Y[i] += Encoder->Z[i+32];
                Encoder->Y[i] += Encoder->Z[i+48];
                Encoder->Y[i] += Encoder->Z[i+64];
            }

            /* Calculate subbands by matrixing */
            for (i = 0; i < 8; i++) {
                Encoder->streamInfo.sbSample[blk][ch][i] = 0;
                for (k = 0; k < 16; k++) {
                    Encoder->streamInfo.sbSample[blk][ch][i] += 
                    MulP(Encoder->Y[k],
                         Analyze8[k][i]);
                }
            }
        }

        /* Go to the next set of samples */
        offset += 16 * ch;
    }
}

/*---------------------------------------------------------------------------
 *            SbcCalculateScaleFactors()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Calculates the scaling factors for the current subband
 *            samples.
 */
static void SbcCalculateScaleFactors(SbcEncoder *Encoder)
{
    U8 blk, sb;
    S32 maxValue0, maxValue1, jMaxValue0, jMaxValue1;
    S32 value0, value1, jValue0, jValue1;

    for (sb = 0; sb < Encoder->streamInfo.numSubBands; sb++) {
        maxValue0 = 0;
        maxValue1 = 0;
        jMaxValue0 = 0;
        jMaxValue1 = 0;

        /* Find the maximum absolute value in the subband */
        for (blk = 0; blk < Encoder->streamInfo.numBlocks; blk++) {

            /* Channel 0 */
            if (Encoder->streamInfo.sbSample[blk][0][sb] < 0) {
                value0 = RealtoS32(-Encoder->streamInfo.sbSample[blk][0][sb]);
            } else {
                value0 = RealtoS32(Encoder->streamInfo.sbSample[blk][0][sb]);
            }

            if (value0 > maxValue0) {
                maxValue0 = value0;
            }

            if (Encoder->streamInfo.numChannels > 1) {

                /* Channel 1 */
                if (Encoder->streamInfo.sbSample[blk][1][sb] < 0) {
                    value1 = RealtoS32(-Encoder->streamInfo.sbSample[blk][1][sb]);
                } else {
                    value1 = RealtoS32(Encoder->streamInfo.sbSample[blk][1][sb]);
                }

                if (value1 > maxValue1) {
                    maxValue1 = value1;
                }

                /* Joint Processing */
                if ((Encoder->streamInfo.channelMode == 
                     SBC_CHNL_MODE_JOINT_STEREO) && 
                    (sb < (Encoder->streamInfo.numSubBands - 1))) {

                    /* Difference and Sum averages */
                    Encoder->sbJoint[blk][0] = 
                        (Encoder->streamInfo.sbSample[blk][0][sb] / 2 + 
                         Encoder->streamInfo.sbSample[blk][1][sb] / 2);
                    Encoder->sbJoint[blk][1] = 
                        (Encoder->streamInfo.sbSample[blk][0][sb] / 2 - 
                         Encoder->streamInfo.sbSample[blk][1][sb] / 2);

                    /* Channel 0 */
                    if (Encoder->sbJoint[blk][0] < 0) {
                        jValue0 = RealtoS32(-Encoder->sbJoint[blk][0]);
                    } else {
                        jValue0 = RealtoS32(Encoder->sbJoint[blk][0]);
                    }

                    if (jValue0 > jMaxValue0) {
                        jMaxValue0 = jValue0;
                    }

                    /* Channel 1 */
                    if (Encoder->sbJoint[blk][1] < 0) {
                        jValue1 = RealtoS32(-Encoder->sbJoint[blk][1]);
                    } else {
                        jValue1 = RealtoS32(Encoder->sbJoint[blk][1]);
                    }

                    if (jValue1 > jMaxValue1) {
                        jMaxValue1 = jValue1;
                    }
                }
            }
        }

        /* Determine the scale factors for channel 0 */
        Encoder->streamInfo.scale_factors[0][sb] = 0;
        while (maxValue0 > 1) {
            /* Determine the scale_factor */
            maxValue0 >>= 1;
            Encoder->streamInfo.scale_factors[0][sb]++;
        }

        Encoder->streamInfo.scaleFactors[0][sb] = 
            (1 << (U32)(Encoder->streamInfo.scale_factors[0][sb] + 1));

        if (Encoder->streamInfo.numChannels > 1) {
            /* Determine the scale factor for channel 1*/
            Encoder->streamInfo.scale_factors[1][sb] = 0;
            while (maxValue1 > 1) {
                /* Determine the scale_factor */
                maxValue1 >>= 1;
                Encoder->streamInfo.scale_factors[1][sb]++;
            }

            Encoder->streamInfo.scaleFactors[1][sb] = 
                (1 << (U32)(Encoder->streamInfo.scale_factors[1][sb] + 1));

            Encoder->streamInfo.join[sb] = 0;
            if ((Encoder->streamInfo.channelMode == 
                 SBC_CHNL_MODE_JOINT_STEREO) && 
                (sb < (Encoder->streamInfo.numSubBands - 1))) {

                /* Determine the scale factor for channel 0 */
                Encoder->sFactorsJoint[0][sb] = 0;
                while (jMaxValue0 > 1) {
                    /* Determine the scale_factor */
                    jMaxValue0 >>= 1;
                    Encoder->sFactorsJoint[0][sb]++;
                }

                /* Determine the scale factor for channel 1*/
                Encoder->sFactorsJoint[1][sb] = 0;
                while (jMaxValue1 > 1) {
                    /* Determine the scale_factor */
                    jMaxValue1 >>= 1;
                    Encoder->sFactorsJoint[1][sb]++;
                }

                if ((Encoder->streamInfo.scale_factors[0][sb] + 
                     Encoder->streamInfo.scale_factors[1][sb]) >
                    (Encoder->sFactorsJoint[0][sb] + 
                     Encoder->sFactorsJoint[1][sb])) {

                    /* Use joint coding */
                    Encoder->streamInfo.join[sb] = 1;

                    for (blk = 0; blk < Encoder->streamInfo.numBlocks; blk++) {
                        Encoder->streamInfo.sbSample[blk][0][sb] = 
                            Encoder->sbJoint[blk][0];
                        Encoder->streamInfo.sbSample[blk][1][sb] = 
                            Encoder->sbJoint[blk][1];
                    }

                    Encoder->streamInfo.scale_factors[0][sb] = 
                        Encoder->sFactorsJoint[0][sb];
                    Encoder->streamInfo.scale_factors[1][sb] = 
                        Encoder->sFactorsJoint[1][sb];

                    Encoder->streamInfo.scaleFactors[0][sb] = 
                        (1 << (U32)(Encoder->streamInfo.scale_factors[0][sb] + 1));

                    Encoder->streamInfo.scaleFactors[1][sb] = 
                        (1 << (U32)(Encoder->streamInfo.scale_factors[1][sb] + 1));
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------
 *            SbcPrepareHeader()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Creates the header for the current SBC frame and writes it to
 *            the stream buffer.
 */
static void SbcPrepareHeader(SbcEncoder *Encoder, U8 *Buff, U16 *Len)
{
    U8    sb, ch;
    U8   *crc;
    BOOL  middle = FALSE;

    /* Initialize FCS */
    Encoder->streamInfo.fcs = 0x0F;

    /* Sync Word */
    Buff[(*Len)++] = SBC_SYNC_WORD;

    /* Sampling frequency */
    Buff[*Len] = Encoder->streamInfo.sampleFreq << 6;

    /* Blocks */
    switch (Encoder->streamInfo.numBlocks) {
    case 8:
        Buff[*Len] |= 1 << 4;
        break;
    case 12:
        Buff[*Len] |= 2 << 4;
        break;
    case 16:
        Buff[*Len] |= 3 << 4;
        break;
    }

    /* Channel mode */
    Buff[*Len] |= Encoder->streamInfo.channelMode << 2;

    /* Allocation Method */
    Buff[*Len] |= Encoder->streamInfo.allocMethod << 1;

    /* Subbands */
    if (Encoder->streamInfo.numSubBands == 8) {
        Buff[*Len] |= 1;
    }

    /* Checksum first byte */
    Encoder->streamInfo.fcs = 
        SbcCrcTable[Encoder->streamInfo.fcs ^ Buff[(*Len)++]];

    /* Bitpool size */
    Buff[*Len] = Encoder->streamInfo.bitPool;

    /* Checksum second byte */
    Encoder->streamInfo.fcs = 
        SbcCrcTable[Encoder->streamInfo.fcs ^ Buff[(*Len)++]];

    /* Skip the CRC */
    crc = &Buff[*Len];
    (*Len)++;
    Encoder->streamInfo.bitOffset = *Len * 8;

    /* Join bits */
    if (Encoder->streamInfo.channelMode == SBC_CHNL_MODE_JOINT_STEREO) {
        Buff[*Len] = 0;
        for (sb = 0; sb < Encoder->streamInfo.numSubBands; sb++) {
            if (Encoder->streamInfo.join[sb]) {
                Buff[*Len] |= 1 << (7 - sb);
            }
        }

        if (Encoder->streamInfo.numSubBands == 8) {
            /* Checksum fourth byte */
            Encoder->streamInfo.fcs = SbcCrcTable[Encoder->streamInfo.fcs ^ 
                                                  Buff[(*Len)++]];
            Encoder->streamInfo.bitOffset += 8;
        } else {
            middle = TRUE;
            Encoder->streamInfo.bitOffset += 4;
        }
    }

    /* Scale Factors */
    for (ch = 0; ch < Encoder->streamInfo.numChannels; ch++) {
        for (sb = 0; sb < Encoder->streamInfo.numSubBands; sb++) {
            if (middle) {
                Buff[*Len] |= Encoder->streamInfo.scale_factors[ch][sb];

                /* Checksum the byte */
                Encoder->streamInfo.fcs = SbcCrcTable[Encoder->streamInfo.fcs ^ 
                                                      Buff[(*Len)++]];

                middle = FALSE;
            } else {
                Buff[*Len] = Encoder->streamInfo.scale_factors[ch][sb] << 4;

                if ((ch == Encoder->streamInfo.numChannels - 1) &&
                    (sb == Encoder->streamInfo.numSubBands - 1)) {

                    /* Sum the next 4 bits */
                    SbcCrcSum4(&Encoder->streamInfo, Buff[*Len]);
                    Buff[*Len] &= 0xF0;
                }

                middle = TRUE;
            }

            Encoder->streamInfo.bitOffset += 4;
        }
    }

    /* Save the CRC */
    *crc = Encoder->streamInfo.fcs;
}

/*---------------------------------------------------------------------------
 *            SbcPackSample()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Packs an audio sample into the stream.  Samples are inserted
 *            in the stream based on the bits required for the current
 *            subband.
 */
static void SbcPackSample(SbcEncoder *Encoder, U16 Sample,  U8 bits, 
                          U8 *Buffer, U16 *Len)
{
    U8  *ptr;
    U8  bit;

    do {
        ptr = Buffer + *Len;
        bit = 8 - Encoder->streamInfo.bitOffset % 8;

        if (bit == 8) {
            /* Clear the byte */
            Buffer[*Len] = 0;
        }

        if (bits > bit) {
            /* The bits are split over multiple bytes */
            *ptr |= Sample >> (bits - bit);
            bits -= bit;
            Encoder->streamInfo.bitOffset += bit;
            (*Len)++;
            Buffer[*Len] = 0;
        } else {
            /* The bits are all in this byte */
            Sample <<= (8 - bits);
            *ptr |= Sample >> (8 - bit);
            Encoder->streamInfo.bitOffset += bits;
            if (bits == bit) {
                (*Len)++;
            }
            bits = 0;
        }
    } while (bits);
}

/*---------------------------------------------------------------------------
 *            SbcQantizeSamples()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Quantizes subband samples and packs them into the stream.
 */
static void SbcQuantizeSamples(SbcEncoder *Encoder, U8 *Buff, U16 *Len)
{
    U8 blk, ch, sb;
    U16 sample;

    /* Calculate levels */
    for (ch = 0; ch < Encoder->streamInfo.numChannels; ch++) {
        for (sb = 0; sb < Encoder->streamInfo.numSubBands; sb++) {
            Encoder->streamInfo.levels[ch][sb] = 
                (1 << Encoder->streamInfo.bits[ch][sb]) - 1;
        }
    }

    if (!(Encoder->streamInfo.bitOffset % 8)) {
        *(Buff+*Len) = 0;
    }

    /* Quantize and store sample */
    for (blk = 0; blk < Encoder->streamInfo.numBlocks; blk++) {
        for (ch = 0; ch < Encoder->streamInfo.numChannels; ch++) {
            for (sb = 0; sb < Encoder->streamInfo.numSubBands; sb++) {
                if (Encoder->streamInfo.levels[ch][sb] > 0) {
                    Assert(Encoder->streamInfo.bits[ch][sb] <= 16);

                    sample = RealtoU16(
                          Mul(Div(Encoder->streamInfo.sbSample[blk][ch][sb], 
                                  Encoder->streamInfo.scaleFactors[ch][sb]) +
                              ONE_F_P, Encoder->streamInfo.levels[ch][sb]) / 2);

                    SbcPackSample(Encoder, sample, 
                                  Encoder->streamInfo.bits[ch][sb],
                                  Buff, Len);
                }

            }
        }
    }

    if (Encoder->streamInfo.bitOffset % 8) {
        (*Len)++;
    }
}

/*---------------------------------------------------------------------------
 *            SBC_EncodeFrames()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Encodes SBC frames into the PCM data stream.
 */
XaStatus SBC_EncodeFrames(SbcEncoder *Encoder, SbcPcmData *PcmData, 
                          U16 *BytesEncoded,  U8 *Buff, U16 *Len,
                          U16 MaxSbcData)
{
    U8         ch;
    XaStatus   status = XA_STATUS_SUCCESS;
    U16        bytesToEncode;
    U16        bytesLeft = PcmData->dataLen;
    U16        encodedFrameLen;
    SbcPcmData pcmData;

#if XA_DEBUG == XA_ENABLED
    U16        oldLen;
#endif

    CheckUnlockedParm(XA_STATUS_INVALID_PARM, 
                      SbcIsValidStreamInfo(&Encoder->streamInfo));

    pcmData.data = PcmData->data;
    pcmData.dataLen = PcmData->dataLen;

    encodedFrameLen = SBC_FrameLen(&Encoder->streamInfo);
    if (encodedFrameLen > MaxSbcData) {
        status = XA_STATUS_FAILED;
        goto exit;
    }

    /* Set the number of channels */
    if (Encoder->streamInfo.channelMode == SBC_CHNL_MODE_MONO) {
        Encoder->streamInfo.numChannels = 1;
    } else {
        Encoder->streamInfo.numChannels = 2;
    }

    /* Set the amount of PCM data to encode */
    bytesToEncode = Encoder->streamInfo.numChannels * 
                    Encoder->streamInfo.numSubBands * 
                    Encoder->streamInfo.numBlocks * 2;

    if (bytesLeft < bytesToEncode) {
        status = XA_STATUS_CONTINUE;
        goto exit;
    }

    *Len = 0;
    *BytesEncoded = 0;
    while (bytesLeft >= bytesToEncode) {

        /* Subband Analysis */
        if (Encoder->streamInfo.numSubBands == 4) {
            SbcAnalysisFilter4(Encoder, &pcmData);
        } else {
            SbcAnalysisFilter8(Encoder, &pcmData);
        }

        /* Scale Factors */
        SbcCalculateScaleFactors(Encoder);

        /* Bit allocation */
        switch (Encoder->streamInfo.channelMode) {
        case SBC_CHNL_MODE_DUAL_CHNL:
        case SBC_CHNL_MODE_MONO:
            for (ch = 0; ch < Encoder->streamInfo.numChannels; ch++) {
                SbcMonoBitAlloc(&Encoder->streamInfo, ch);
            }
            break;
        case SBC_CHNL_MODE_JOINT_STEREO:
        case SBC_CHNL_MODE_STEREO:
            SbcStereoBitAlloc(&Encoder->streamInfo);
            break;
        }

#if XA_DEBUG == XA_ENABLED
        oldLen = *Len;
#endif

        /* Prepare header */
        SbcPrepareHeader(Encoder, Buff, Len);

        /* Quantize and pack bits */
        SbcQuantizeSamples(Encoder, Buff, Len);

#if XA_DEBUG == XA_ENABLED
        Assert(encodedFrameLen == *Len - oldLen);
#endif

        bytesLeft -= bytesToEncode;
        *BytesEncoded += bytesToEncode;

        if ((*Len + encodedFrameLen) > MaxSbcData) {
            /* SBC buffer is filled as much as possible */
            goto exit;
        } else {
            pcmData.data += bytesToEncode;
            pcmData.dataLen -= bytesToEncode;
        }
    }

    exit:

    return status;
}

#endif


