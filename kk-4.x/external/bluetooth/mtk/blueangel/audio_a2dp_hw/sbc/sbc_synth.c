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

/* Precomputed: synmatrix4[k][i] = cos( (i+0.5) * (k+2.0) * pi/4.0 ) */
static const double synmatrix4[8][4] =  {
    {  0.707106781186548, -0.707106781186547, -0.707106781186548,  0.707106781186547 },
    {  0.38268343236509,  -0.923879532511287,  0.923879532511287, -0.38268343236509  },
    {  0,                  0,                  0,                  0                 },
    { -0.38268343236509,   0.923879532511287, -0.923879532511287,  0.382683432365091 },
    { -0.707106781186547,  0.707106781186548,  0.707106781186547, -0.707106781186547 },
    { -0.923879532511287, -0.38268343236509,   0.382683432365091,  0.923879532511288 },
    { -1,                 -1,                 -1,                 -1                 },
    { -0.923879532511287, -0.382683432365091,  0.38268343236509,   0.923879532511287 }
};

/* Precomputed: synmatrix8[k][i] = cos( (i+0.5) * (k+4.0) * pi/8.0 ) */
static const double synmatrix8[16][8] = {
    {  0.707106781186548, -0.707106781186547, -0.707106781186548,  0.707106781186547,
       0.707106781186548, -0.707106781186547, -0.707106781186547,  0.707106781186547 },
    {  0.555570233019602, -0.98078528040323,   0.195090322016128,  0.831469612302545,
      -0.831469612302545, -0.195090322016128,  0.980785280403231, -0.555570233019602 },
    {  0.38268343236509,  -0.923879532511287,  0.923879532511287, -0.38268343236509,
      -0.382683432365091,  0.923879532511287, -0.923879532511286,  0.38268343236509  },
    {  0.195090322016128, -0.555570233019602,  0.831469612302545, -0.980785280403231,
       0.98078528040323,  -0.831469612302545,  0.555570233019602, -0.195090322016129 },
    {  0,                  0,                  0,                  0,
       0,                  0,                  0,                  0                 },
    { -0.195090322016128,  0.555570233019602, -0.831469612302545,  0.98078528040323,
      -0.980785280403231,  0.831469612302545, -0.555570233019603,  0.19509032201613  },
    { -0.38268343236509,   0.923879532511287, -0.923879532511287,  0.382683432365091,
       0.38268343236509,  -0.923879532511287,  0.923879532511288, -0.382683432365091 },
    { -0.555570233019602,  0.98078528040323,  -0.195090322016128, -0.831469612302545,
       0.831469612302545,  0.195090322016128, -0.98078528040323,   0.555570233019606 },
    { -0.707106781186547,  0.707106781186548,  0.707106781186547, -0.707106781186547,
      -0.707106781186546,  0.707106781186548,  0.707106781186546, -0.707106781186548 },
    { -0.831469612302545,  0.195090322016129,  0.980785280403231,  0.555570233019602,
      -0.555570233019603, -0.98078528040323,  -0.195090322016128,  0.831469612302547 },
    { -0.923879532511287, -0.38268343236509,   0.382683432365091,  0.923879532511288,
       0.923879532511287,  0.382683432365089, -0.382683432365091, -0.923879532511287 },
    { -0.98078528040323,  -0.831469612302545, -0.555570233019602, -0.195090322016129,
       0.19509032201613,   0.555570233019606,  0.831469612302547,  0.980785280403231 },
    { -1,                 -1,                 -1,                 -1,
      -1,                 -1,                 -1,                 -1                 },
    { -0.98078528040323,  -0.831469612302546, -0.555570233019603, -0.19509032201613,
       0.195090322016128,  0.555570233019604,  0.831469612302545,  0.98078528040323  },
    { -0.923879532511287, -0.382683432365091,  0.38268343236509,   0.923879532511287,
       0.923879532511288,  0.382683432365088, -0.382683432365089, -0.923879532511285 },
    { -0.831469612302545,  0.195090322016127,  0.98078528040323,   0.555570233019603,
      -0.555570233019601, -0.98078528040323,  -0.195090322016131,  0.831469612302545 }
};


static void sbc_synthesize_four(int ch, SB_Type *sb_input, short *pcm_output,
                SBC_Decoder_State *state)
{
    int i, j, k;

    /* Input 4 New Subband Samples */
    for (i = 0; i < 4; i++)
    {
        //state->S[ch][i] = frame->sb_sample[blk][ch][i];
        state->S[ch][i] = sb_input[i];
    }

    /* Shifting */
    for (i = 79; i >= 8; i--)
    {
        state->V[ch][i] = state->V[ch][i - 8];
    }

    /* Matrixing */
    for (k = 0; k < 8; k++)
    {
        state->V[ch][k] = 0;
        for (i = 0; i < 4; i++)
        {
            state->V[ch][k] += synmatrix4[k][i] * state->S[ch][i];
        }
    }

    /* Build a 40 values vector U */
    for (i = 0; i <= 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            state->U[ch][i * 8 + j] = state->V[ch][i * 16 + j];
            state->U[ch][i * 8 + j + 4] = state->V[ch][i * 16 + j + 12];
        }
    }

    /* Window by 40 coefficients */
    for (i = 0; i < 40; i++)
    {
        state->W[ch][i] = state->U[ch][i] * sbc_proto_4_40[i] * (-4);
    }

    /* Calculate 4 audio samples */
    for (j = 0; j < 4; j++)
    {
        state->X[ch][j] = 0;
        for (i = 0; i < 10; i++)
        {
            state->X[ch][j] += state->W[ch][j + 4 * i];
        }
    }

    /* Output 4 reconstructed Audio Samples */
    for (i = 0; i < 4; i++)
    {
        //frame->pcm_sample[ch][blk * 4 + i] = state->X[ch][i];
        pcm_output[i] = (short)state->X[ch][i];
    }
}

static void sbc_synthesize_eight(int ch, SB_Type *sb_input, short *pcm_output,
                SBC_Decoder_State *state)
{
    int i, j, k;

    /* Input 8 New Subband Samples */
    for (i = 0; i < 8; i++)
    {
        //state->S[ch][i] = frame->sb_sample[blk][ch][i];
        state->S[ch][i] = sb_input[i];
    }

    /* Shifting */
    for (i = 159; i >= 16; i--)
    {
        state->V[ch][i] = state->V[ch][i - 16];
    }

    /* Matrixing */
    for (k = 0; k < 16; k++)
    {
        state->V[ch][k] = 0;
        for (i = 0; i < 8; i++)
        {
            state->V[ch][k] += synmatrix8[k][i] * state->S[ch][i];
        }
    }

    /* Build a 80 values vector U */
    for (i = 0; i <= 4; i++)
    {
        for (j = 0; j < 8; j++)
        {
            state->U[ch][i * 16 + j] = state->V[ch][i * 32 + j];
            state->U[ch][i * 16 + j + 8] = state->V[ch][i * 32 + j + 24];
        }
    }

    /* Window by 80 coefficients */
    for (i = 0; i < 80; i++)
    {
        //state->W[ch][i] = state->U[ch][i] * sbc_proto_8_80[i] * (-4);
        state->W[ch][i] = state->U[ch][i] * sbc_proto_8_80[i] * (-8); // YS fixed open source bug
    }

    /* Calculate 8 audio samples */
    for (j = 0; j < 8; j++)
    {
        state->X[ch][j] = 0;
        for (i = 0; i < 10; i++)
        {
            state->X[ch][j] += state->W[ch][j + 8 * i];
        }
    }

    /* Ouput 8 reconstructed Audio Samples */
    for (i = 0; i < 8; i++)
    {
#ifdef SB_CLIPPING
        if(state->X[ch][i] > 32767.0)
        {
            state->X[ch][i] = 32767;
        }

        if(state->X[ch][i] < -32768.0)
        {
            state->X[ch][i] = -32768;
        }
#endif
        //frame->pcm_sample[ch][blk * 8 + i] = state->X[ch][i];
        pcm_output[i] = (short)state->X[ch][i];
    }
}

int sbc_synthesize_audio(SBC_Decoder_State *sbc_state, SBC_Frame_State *frame, short pcm_sample[2][16*8])
{
    int ch, blk, i;
    short pcm_output[8];
    SB_BUF_T sb_sample_buf = (SB_BUF_T)(frame->sb_sample);

    switch (frame->subbands)
    {
        case 4:
        {
            for (ch = 0; ch < frame->channels; ch++)
            {
                for (blk = 0; blk < frame->blocks; blk++)
                {
                    sbc_synthesize_four(ch, sb_sample_buf[blk][ch], pcm_output, sbc_state);
                    for(i = 0 ; i < 4 ; i ++)
                        pcm_sample[ch][blk * 4 + i]  = pcm_output[i];
                }
            }            
            break;
        }
        case 8:
        {
            for (ch = 0; ch < frame->channels; ch++)
            {
                for (blk = 0; blk < frame->blocks; blk++)
                {
                    sbc_synthesize_eight(ch, sb_sample_buf[blk][ch], pcm_output, sbc_state);
                    for(i = 0 ; i < 8 ; i ++)
                        pcm_sample[ch][blk * 8 + i] = pcm_output[i];
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

