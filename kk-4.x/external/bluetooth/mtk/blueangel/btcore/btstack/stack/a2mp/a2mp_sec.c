/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include "bt_common.h"
#include "xatypes.h"

const U8 key_id_gamp[] = { 0x67, 0x61, 0x6d, 0x70};
const U8 key_id_802b[] = {'8', '0', '2', 'b'};

void LM_SspEng_sha2_hmac(const unsigned char *buf, unsigned int buflen, unsigned char *key, unsigned int keylen, unsigned char *digest);

#ifdef __BT_3_0_HS__

void BTA2MP_GenerateInitGAMPLK(U8 *link_key, U8 *result)
{
    U8 W[32];
    U8 W1[32];    
    U8 i=0;

    /* The link_key[0] is LSB */
    /* The link_key[15] is MSB */
    /* W[15] is MSB, so copy link key directly */
    for(i=0;i<16;i++)
    {
        W[i] = link_key[i];
        /* W[15] = link[15] */
    }
    for(i=0;i<16;i++)
    {
        W[i+16] = link_key[i];
        /* W[31] = link[15] */
    }
#if 0    
    W[31] = 0xc2;
    W[30] = 0x34;
    W[29] = 0xc1;
    W[28] = 0x19;
    W[27] = 0x8f;
    W[26] = 0x3b;
    W[25] = 0x52;
    W[24] = 0x01;
    W[23] = 0x86;
    W[22] = 0xab;
    W[21] = 0x92;
    W[20] = 0xa2;
    W[19] = 0xf8;
    W[18] = 0x74;
    W[17] = 0x93;
    W[16] = 0x4e;
    W[15] = 0xc2;
    W[14] = 0x34;
    W[13] = 0xc1;
    W[12] = 0x19;
    W[11] = 0x8f;
    W[10] = 0x3b;
    W[9] = 0x52;
    W[8] = 0x01;
    W[7] = 0x86;
    W[6] = 0xab;
    W[5] = 0x92;
    W[4] = 0xa2;
    W[3] = 0xf8;
    W[2] = 0x74;
    W[1] = 0x93;
    W[0] = 0x4e;
#endif
    //hmac_sha256(W, 32, key_id_gamp, 4, W1, 32);
    LM_SspEng_sha2_hmac(key_id_gamp, 4, W, 32, W1);
/*    W1[31] is MSB */
/*    W1[0] is LSB */
    memcpy(result, W1, 32);
    
}

void BTA2MP_DedicatedAMPLK(U8 *gamp, U8 *result)
{
    U8 W[32];
    U8 W1[32];
    U8 i=0;

    memcpy(W, gamp, 32);

    //hmac_sha256(W, 32, key_id_802b, 4, W1, 32);
    LM_SspEng_sha2_hmac(key_id_802b, 4, W, 32, W1);
    memcpy(result, W1, 32);

}

void BTA2MP_ReGenerateAMPLK(U8 *gamp, U8 *result)
{
    U8 W[32];
    U8 W1[32];
    U8 i=0;
    memcpy(W, gamp, 32);

    //hmac_sha256(W, 32, key_id_gamp, 4, W1, 32);
    LM_SspEng_sha2_hmac(key_id_gamp, 4, W, 32, W1);
    memcpy(result, W1, 32);


}

#endif
