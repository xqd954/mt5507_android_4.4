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


#include "sbc.h"
#include <stdio.h>

#include "bt_ext_debug.h"
#include "bt_types.h"
#include "sbc_armdsp_m.h"

static const short anamatrix8[8][8] = { // reordered
    { 23170,  27246,  30274,  32138,  32767, 
      18205,  12540,   6393 },
    {-23170, -32138, -12540,  18205,  32767, 
       6393,  30274,  27246 },
    { 23170,  18205, -30274,  -6393,  32767, 
     -27246, -12540,  32138 },
    {-23170,   6393,  12540, -27246,  32767, 
      32138, -30274,  18205 },
    {-23170,  -6393,  12540,  27246,  32767, 
     -32138, -30274, -18205 },
    { 23170, -18205, -30274,   6393,  32767, 
      27246, -12540, -32138 },
    {-23170,  32138, -12540, -18205,  32767, 
      -6393,  30274, -27246 },
    { 23170, -27246,  30274, -32138,  32767, 
     -18205,  12540,  -6393 }
};

#pragma pack (4)

const short sbc_proto_8_80_fx[80]=
{	0,      21,      45,      73,       
  108,     149,     194,     234,       
  264,     276,     261,     212,       
  118,     -23,    -216,    -458,       
  742,    1052,    1371,    1671,       
 1921,    2085,    2126,    2008,       
 1696,    1161,     383,    -644,       
-1919,   -3422,   -5122,    -6971,       
 8913,   10877,   12789,    14575,       
16157,   17467,   18449,    19057,       
19262,   19057,   18449,    17467,       
16157,   14575,   12789,    10877,       
-8913,   -6971,   -5122,    -3422,       
-1919,    -644,     383,     1161,       
 1696,    2008,    2126,     2085,       
 1921,    1671,    1371,     1052,       
 -742,    -458,    -216,      -23,       
  118,     212,     261,      276,       
  264,     234,     194,      149,       
  108,      73,      45,       21};

#pragma pack ()

const short sbc_proto_4_40_fx[40] = {
     0,     18,     49,     90,    126,    128,     61,    -99, 
   358,    670,    946,   1055,    848,    201,   -943,  -2543, 
  4443,   6389,   8082,   9235,   9644,   9235,   8082,   6389, 
 -4442,  -2543,   -943,    201,    848,   1055,    946,    670, 
  -357,    -99,     61,    128,    126,     90,     49,     18
};

#if 0
/* Precomputed: anamatrix4[i][k] = cos( (i+0.5) * (k-2) * pi/4 ) */
static const double anamatrix4[4][8] = {
    {  0.707106781186548,  0.923879532511287,  1,                  0.923879532511287,
       0.707106781186548,  0.38268343236509,   0,                 -0.38268343236509  },
    { -0.707106781186547,  0.38268343236509,   1,                  0.38268343236509,
      -0.707106781186547, -0.923879532511287,  0,                  0.923879532511287 },
    { -0.707106781186548, -0.38268343236509,   1,                 -0.38268343236509,
      -0.707106781186548,  0.923879532511287,  0,                 -0.923879532511287 },
    {  0.707106781186547, -0.923879532511287,  1,                 -0.923879532511287,
       0.707106781186547, -0.38268343236509,   0,                  0.382683432365091 }
};

static const double anamatrix8[8][16] = {
    {  0.70710678118655, 0.83146961230255, 0.92387953251129, 0.98078528040323, 
       1.00000000000000, 0.98078528040323, 0.92387953251129, 0.83146961230255, 
       0.70710678118655, 0.55557023301960, 0.38268343236509, 0.19509032201613, 
       0.00000000000000, -0.19509032201613, -0.38268343236509, -0.55557023301960 },
    { -0.70710678118655, -0.19509032201613, 0.38268343236509, 0.83146961230255, 
       1.00000000000000, 0.83146961230255, 0.38268343236509, -0.19509032201613, 
      -0.70710678118655, -0.98078528040323, -0.92387953251129, -0.55557023301960, 
      -0.00000000000000, 0.55557023301960, 0.92387953251129, 0.98078528040323 },
    { -0.70710678118655, -0.98078528040323, -0.38268343236509, 0.55557023301960, 
       1.00000000000000, 0.55557023301960, -0.38268343236509, -0.98078528040323, 
      -0.70710678118655, 0.19509032201613, 0.92387953251129, 0.83146961230255, 
       0.00000000000000, -0.83146961230255, -0.92387953251129, -0.19509032201613 },
    {  0.70710678118655, -0.55557023301960, -0.92387953251129, 0.19509032201613, 
       1.00000000000000, 0.19509032201613, -0.92387953251129, -0.55557023301960, 
       0.70710678118655, 0.83146961230255, -0.38268343236509, -0.98078528040323, 
      -0.00000000000000, 0.98078528040323, 0.38268343236509, -0.83146961230255 },
    {  0.70710678118655, 0.55557023301960, -0.92387953251129, -0.19509032201613, 
       1.00000000000000, -0.19509032201613, -0.92387953251129, 0.55557023301960, 
       0.70710678118655, -0.83146961230255, -0.38268343236509, 0.98078528040323, 
       0.00000000000000, -0.98078528040323, 0.38268343236509, 0.83146961230254 },
    { -0.70710678118655, 0.98078528040323, -0.38268343236509, -0.55557023301960, 
       1.00000000000000, -0.55557023301960, -0.38268343236509, 0.98078528040323, 
      -0.70710678118655, -0.19509032201613, 0.92387953251129, -0.83146961230255, 
      -0.00000000000000, 0.83146961230254, -0.92387953251129, 0.19509032201613 },
    { -0.70710678118655, 0.19509032201613, 0.38268343236509, -0.83146961230255, 
       1.00000000000000, -0.83146961230255, 0.38268343236509, 0.19509032201613, 
      -0.70710678118655, 0.98078528040323, -0.92387953251129, 0.55557023301960, 
      -0.00000000000000, -0.55557023301960, 0.92387953251129, -0.98078528040323 },
    {  0.70710678118655, -0.83146961230255, 0.92387953251129, -0.98078528040323, 
       1.00000000000000, -0.98078528040323, 0.92387953251129, -0.83146961230255, 
       0.70710678118655, -0.55557023301960, 0.38268343236509, -0.19509032201613, 
      -0.00000000000000, 0.19509032201613, -0.38268343236509, 0.55557023301961 }
/*
    {  0.923879532511287,  0.98078528040323,   1,                  0.98078528040323,
       0.923879532511287,  0.831469612302545,  0.707106781186548,  0.555570233019602,
       0.38268343236509,   0.195090322016128,  0,                 -0.195090322016128,
      -0.38268343236509,  -0.555570233019602, -0.707106781186547, -0.831469612302545 },
    {  0.38268343236509,   0.831469612302545,  1,                  0.831469612302545,
       0.38268343236509,  -0.195090322016128, -0.707106781186547, -0.98078528040323,
      -0.923879532511287, -0.555570233019602,  0,                  0.555570233019602,
       0.923879532511287,  0.98078528040323,   0.707106781186548,  0.195090322016129 },
    { -0.38268343236509,   0.555570233019602,  1,                  0.555570233019602,
      -0.38268343236509,  -0.98078528040323,  -0.707106781186548,  0.195090322016128,
       0.923879532511287,  0.831469612302545,  0,                 -0.831469612302545,
      -0.923879532511287, -0.195090322016128,  0.707106781186547,  0.980785280403231 },
    { -0.923879532511287,  0.195090322016128,  1,                  0.195090322016128,
      -0.923879532511287, -0.555570233019602,  0.707106781186547,  0.831469612302545,
      -0.38268343236509,  -0.980785280403231,  0,                  0.98078528040323,
       0.382683432365091, -0.831469612302545, -0.707106781186547,  0.555570233019602 },
    { -0.923879532511287, -0.195090322016128,  1,                 -0.195090322016128,
      -0.923879532511287,  0.555570233019602,  0.707106781186548, -0.831469612302545,
      -0.382683432365091,  0.98078528040323,   0,                 -0.980785280403231,
       0.38268343236509,   0.831469612302545, -0.707106781186546, -0.555570233019603 },
    { -0.38268343236509,  -0.555570233019602,  1,                 -0.555570233019602,
      -0.38268343236509,   0.98078528040323,  -0.707106781186547, -0.195090322016128,
       0.923879532511287, -0.831469612302545,  0,                  0.831469612302545,
      -0.923879532511287,  0.195090322016128,  0.707106781186548, -0.98078528040323  },
    {  0.38268343236509,  -0.831469612302545,  1,                 -0.831469612302545,
       0.38268343236509,   0.195090322016129, -0.707106781186547,  0.980785280403231,
      -0.923879532511286,  0.555570233019602,  0,                 -0.555570233019603,
       0.923879532511288, -0.98078528040323,   0.707106781186546, -0.195090322016128 },
    {  0.923879532511287, -0.98078528040323,   1,                 -0.98078528040323,
       0.923879532511287, -0.831469612302545,  0.707106781186547, -0.555570233019602,
       0.38268343236509,  -0.195090322016129,  0,                  0.19509032201613,
      -0.382683432365091,  0.555570233019606, -0.707106781186548,  0.831469612302547 }
*/
};

static void sbc_analyze_four(int ch, short *pcm_input, SB_Type *sb_output, SBC_Encoder_State *state)
{
    int i, k;

    /* Input 4 New Audio Samples */
    for (i = 39; i >= 4; i--)
    {
        state->X[ch][i] = state->X[ch][i - 4];
    }
    for (i = 3; i >= 0; i--)
    {
        //state->X[ch][i] = frame->pcm_sample[ch][blk * 4 + (3 - i)];
        state->X[ch][i] = (float)pcm_input[i];
    }
    
    /* Windowing by 40 coefficients */
    for (i = 0; i < 40; i++)
    {
        state->Z[ch][i] = sbc_proto_4_40[i] * state->X[ch][i];
    }

    /* Partial calculation */
    for (i = 0; i < 8; i++)
    {
        state->Y[ch][i] = 0;
        for (k = 0; k < 5; k++)
        {
            state->Y[ch][i] += state->Z[ch][i + k * 8];
        }
    }

    /* Calculate 4 subband samples by Matrixing */
    for (i = 0; i < 4; i++) {
        state->S[ch][i] = 0;
        for (k = 0; k < 8; k++)
            state->S[ch][i] += anamatrix4[i][k] * state->Y[ch][k];
    }

    /* Output 4 Subband Samples */
    for (i = 0; i < 4; i++)
    {
#ifdef SB_CLIPPING
        if(state->S[ch][i] > 32767.0)
        {
            state->S[ch][i] = 32767;
        }

        if(state->S[ch][i] < -32768.0)
        {
            state->S[ch][i] = -32768;
        }
#endif
        //frame->sb_sample[blk][ch][i] = state->S[ch][i];
        sb_output[i] = (SB_Type)state->S[ch][i];
    }
}
#endif



static void sbc_analyze_eight(int ch, int step, short *pcm_input, SB_Type *sb_output, SBC_Encoder_State *state)
{
    int i;
    kal_int32 *lptr1, *lptr2, *lptr3;
    kal_int16 *ptr1, *ptr2;
    long Y[16];

    /* Input 8 Audio Samples */
    lptr1 = (kal_int32 *)(&state->X[ch][78]);
    lptr2 = (kal_int32 *)(&state->X[ch][70]);
    i = 9;
    do {
        kal_int32 L_1, L_2;
        L_1 = *lptr2--;
        L_2 = *lptr2--;
        *lptr1-- = L_1;
        *lptr1-- = L_2;
        L_1 = *lptr2--;
        L_2 = *lptr2--;
        *lptr1-- = L_1;
        *lptr1-- = L_2;
    } while( --i>0 );

    ptr1 = (kal_int16 *)lptr1;
    ptr1 += 1;
    ptr2 = (kal_int16 *)(&pcm_input[0]);

    if (step == 2) {
        *ptr1-- = *ptr2;
        *ptr1-- = *(ptr2+2);
        *ptr1-- = *(ptr2+4);
        *ptr1-- = *(ptr2+6);
        *ptr1-- = *(ptr2+8);
        *ptr1-- = *(ptr2+10);
        *ptr1-- = *(ptr2+12);
        *ptr1-- = *(ptr2+14);
    } else {
        kal_int16 S_1, S_2;
        
        S_1 = *ptr2++;
        S_2 = *ptr2++;
        *ptr1-- = S_1;
        *ptr1-- = S_2;
        S_1 = *ptr2++;
        S_2 = *ptr2++;
        *ptr1-- = S_1;
        *ptr1-- = S_2;
        S_1 = *ptr2++;
        S_2 = *ptr2++;
        *ptr1-- = S_1;
        *ptr1-- = S_2;
        S_1 = *ptr2++;
        S_2 = *ptr2++;
        *ptr1-- = S_1;
        *ptr1-- = S_2;
    }

    /* Windowing by 80 coefficients */
    /* Partial calculation */
    lptr1 = (kal_int32 *)&state->X[ch][0];
    lptr2 = (kal_int32 *)&sbc_proto_8_80_fx[0];
    lptr3 = Y;
    i = 8;
    do {
        kal_int32 L_sum1, L_sum2;
        kal_int32 L_1, L_2, L_3, L_4;

        L_1 = *lptr1; lptr1 += 8;
        L_2 = *lptr2; lptr2 += 8;
        L_3 = *lptr1; lptr1 += 8;
        L_4 = *lptr2; lptr2 += 8;
        L_sum1 = smulbb( L_1, L_2 );
        L_sum2 = smultt( L_1, L_2 );
        L_sum1 = smlabb( L_3, L_4, L_sum1 );
        L_sum2 = smlatt( L_3, L_4, L_sum2 );

        L_1 = *lptr1; lptr1 += 8;
        L_2 = *lptr2; lptr2 += 8;
        L_3 = *lptr1; lptr1 += 8;
        L_4 = *lptr2; lptr2 += 8;
        L_sum1 = smlabb( L_1, L_2, L_sum1 );
        L_sum2 = smlatt( L_1, L_2, L_sum2 );
        L_1 = *lptr1; lptr1 -= 31;
        L_2 = *lptr2; lptr2 -= 31;
        L_sum1 = smlabb( L_3, L_4, L_sum1 );
        L_sum2 = smlatt( L_3, L_4, L_sum2 );
        L_sum1 = smlabb( L_1, L_2, L_sum1 );
        L_sum2 = smlatt( L_1, L_2, L_sum2 );
        *lptr3++ = L_sum1 >> 3 ;
        *lptr3++ = L_sum2 >> 3 ;
    } while( --i>0 );

    /* Calculate 8 subband samples by Matrixing */
    /* Output 8 Subband Samples */
    ptr1 = sb_output;
    lptr1 = (kal_int32 *)&anamatrix8[0][0];
    lptr3 = (kal_int32 *)&anamatrix8[4][0];
    i = 4;
    do {
        register kal_int32 L_sum1, L_sum2;
        lptr2 = Y;
        {
            kal_int32  L_1, L_2, L_3, L_4, L_Y, L_Y2;
            L_Y = Y[0];
            L_Y2 = Y[8];
            L_1 = lptr1[0];
            L_2 = lptr3[0];
            L_sum1 = smulwb( L_Y, L_1 );
            L_sum2 = smulwb( L_Y, L_2 );
            L_Y = Y[1];
            L_sum1 = smlawb( L_Y2, L_1, L_sum1 );
            L_sum2 = smlawb( L_Y2, L_2, L_sum2 );
            L_Y2 = Y[7];
            L_sum1 = smlawt( L_Y, L_1, L_sum1 );
            L_sum2 = smlawt( L_Y, L_2, L_sum2 );
            L_sum1 = smlawt( L_Y2, L_1, L_sum1 );
            L_sum2 = smlawt( L_Y2, L_2, L_sum2 );

            L_Y = Y[2];
            L_Y2 = Y[6];
            L_3 = lptr1[1];
            L_4 = lptr3[1];
            L_sum1 = smlawb( L_Y, L_3, L_sum1 );
            L_sum2 = smlawb( L_Y, L_4, L_sum2 );
            L_Y = Y[3];
            L_sum1 = smlawb( L_Y2, L_3, L_sum1 );
            L_sum2 = smlawb( L_Y2, L_4, L_sum2 );
            L_Y2 = Y[5];
            L_sum1 = smlawt( L_Y, L_3, L_sum1 );
            L_sum2 = smlawt( L_Y, L_4, L_sum2 );
            L_sum1 = smlawt( L_Y2, L_3, L_sum1 );
            L_sum2 = smlawt( L_Y2, L_4, L_sum2 );
            
            L_Y = Y[4];
            L_Y2 = Y[9];
            L_1 = lptr1[2];
            L_2 = lptr3[2];
            L_sum1 = smlawb( L_Y, L_1, L_sum1 );
            L_sum2 = smlawb( L_Y, L_2, L_sum2 );
            L_Y = Y[15];
            L_sum1 = smlawt( L_Y2, L_1, L_sum1 );
            L_sum2 = smlawt( L_Y2, L_2, L_sum2 );
            L_sum1 = qsub( L_sum1, smulwt( L_Y, L_1 ) );
            L_sum2 = qsub( L_sum2, smulwt( L_Y, L_2 ) );

            L_Y = Y[14];
            L_Y2 = Y[10];
            L_3 = lptr1[3];
            L_4 = lptr3[3];
            L_sum1 = smlawb( L_Y2, L_3, L_sum1 );
            L_sum2 = smlawb( L_Y2, L_4, L_sum2 );
            L_sum1 = qsub( L_sum1, smulwb( L_Y, L_3 ) );
            L_sum2 = qsub( L_sum2, smulwb( L_Y, L_4 ) );
            L_Y2 = Y[11];
            L_Y = Y[13];
            L_sum1 = smlawt( L_Y2, L_3, L_sum1 );
            L_sum2 = smlawt( L_Y2, L_4, L_sum2 );
            L_sum1 = qsub( L_sum1, smulwt( L_Y, L_3 ) );
            L_sum2 = qsub( L_sum2, smulwt( L_Y, L_4 ) );

            lptr1 += 4;
            lptr3 += 4;
         }

        L_sum1 = qadd( L_sum1, L_sum1 );
        L_sum2 = qadd( L_sum2, L_sum2 );
        L_sum1 = qadd( L_sum1, L_sum1 );
        L_sum2 = qadd( L_sum2, L_sum2 );
        L_sum1 = qdadd( 0x8000, L_sum1 );
        L_sum2 = qdadd( 0x8000, L_sum2 );
        L_sum1 = L_sum1 >> 16;
        L_sum2 = L_sum2 >> 16;
        *ptr1++ = (Word16)L_sum1;
        *ptr1++ = (Word16)L_sum2;
    } while( --i>0 );
}



int sbc_analyze_audio(int srcChannels, SBC_Encoder_State *sbc_state, SBC_Frame_State *frame, short *pcm_sample)
{
    int ch, blk;
    SB_BUF_T sb_sample_buf = (SB_BUF_T)(frame->sb_sample);

    switch (frame->subbands)
    {
        case 8:
        {
            int step = srcChannels;
            short *pSample;
            
            for (ch = 0; ch < frame->channels; ch++)
            {
                pSample = pcm_sample + ch;
                
                for (blk = 0; blk < frame->blocks; blk++)
                {
                    sbc_analyze_eight(ch, step, pSample, sb_sample_buf[blk][ch], sbc_state);
                    pSample += (step<<3);
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return( frame->subbands * frame->blocks );
}

