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

#ifdef WIN32
#include <stdio.h>
#define LOG_DATA 0
static unsigned short save_audio_sample[16][2][8];
static void save_audio_sample_to_file(SBC_Frame_State *frame)
{
    static FILE *fp;
    static int init = 1;

    if( init )
    {
        fp = fopen( "audio_sample_dec.dat", "wb" );
        init = 0;
    }
    fwrite( save_audio_sample, sizeof(unsigned short), 16*2*8, fp );
}
#endif


/*
 * Unpacks a SBC frame at the beginning of the stream in data,
 * which has at most len bytes into frame.
 * Returns the length in bytes of the packed frame, or a negative
 * value on error. The error codes are:
 *
 *  -1   Data stream too short
 *  -2   Sync byte incorrect
 *  -3   CRC8 incorrect
 *  -4   Bitpool value out of bounds
 */
int sbc_unpack_frame(SBC_Frame_State *frame, unsigned char *data,  int len)
{
    int consumed;
    /* Will copy the parts of the header that are relevant to crc calculation here */
    unsigned char crc_header[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int crc_pos = 0;

    unsigned char sf;       /* sampling_frequency, temporarily needed as array index */

    int ch, sb, blk, bit;   /* channel, subband, block and bit standard counters */
    int bits[2][8];     /* bits distribution */
    int levels[2][8];   /* levels derived from that */

    //double scalefactor[2][8]; /* derived from frame->scale_factors */
    int scalefactor[2][8];
    SB_BUF_T sb_sample_buf = (SB_BUF_T)frame->sb_sample;

    unsigned char join;
    unsigned short audio_sample[16][2][8];

    if (len < 4)
    {
        return -1;
    }

    if (data[0] != SBC_SYNCWORD)
    {
        return -2;
    }

    frame->sbc_frame_header_data = data[1];

    sf = (data[1] >> 6) & 0x03;
    switch (sf) {
    case SBC_FS_16:
        frame->sampling_frequency = 16000;
        break;
    case SBC_FS_32:
        frame->sampling_frequency = 32000;
        break;
    case SBC_FS_44:
        frame->sampling_frequency = 44100;
        break;
    case SBC_FS_48:
        frame->sampling_frequency = 48000;
        break;
    }

    switch ((data[1] >> 4) & 0x03) {
    case SBC_NB_4:
        frame->blocks = 4;
        break;
    case SBC_NB_8:
        frame->blocks = 8;
        break;
    case SBC_NB_12:
        frame->blocks = 12;
        break;
    case SBC_NB_16:
        frame->blocks = 16;
        break;
    }

    frame->channel_mode = (data[1] >> 2) & 0x03;
    switch (frame->channel_mode) {
    case MONO:
        frame->channels = 1;
        break;
    case DUAL_CHANNEL:  /* fall-through */
    case STEREO:
    case JOINT_STEREO:
        frame->channels = 2;
        break;
    }

    frame->allocation_method = (data[1] >> 1) & 0x01;

    frame->subbands = (data[1] & 0x01) ? 8 : 4;

    frame->bitpool = data[2];

    if (((frame->channel_mode == MONO || frame->channel_mode == DUAL_CHANNEL)
         && frame->bitpool > 16 * frame->subbands)
        || ((frame->channel_mode == STEREO || frame->channel_mode == JOINT_STEREO)
        && frame->bitpool > 32 * frame->subbands))
    {
        return -4;
    }

    /* data[3] is crc, we're checking it later */

    consumed = 32;

    crc_header[0] = data[1];
    crc_header[1] = data[2];
    crc_header[2] = 0; // dummy
    crc_pos = 24; // 8-bit dummy

    if (frame->channel_mode == JOINT_STEREO)
    {
        if (len * 8 < consumed + frame->subbands)
        {
            return -1;
        }
        else
        {
            join = 0x00;
            for (sb = 0; sb < frame->subbands - 1; sb++)
            {
                join |= ((data[4] >> (7 - sb)) & 0x01) << sb;
            }
            if (frame->subbands == 4)
            {
                crc_header[crc_pos / 8] = data[4] & 0xf0;
            }
            else
            {
                crc_header[crc_pos / 8] = data[4];
            }

            consumed += frame->subbands;
            crc_pos += frame->subbands;
        }
    }

    if (len * 8 < consumed + (4 * frame->subbands * frame->channels))
    {
        return -1;
    }
    else
    {
        for (ch = 0; ch < frame->channels; ch++)
        {
            for (sb = 0; sb < frame->subbands; sb++)
            {
                /* FIXME assert(consumed % 4 == 0); */
                frame->scale_factor[ch][sb] = (data[consumed / 8] >> (4 - (consumed % 8))) & 0x0F;
                crc_header[crc_pos / 8] |= frame->scale_factor[ch][sb] << (4 - (crc_pos % 8));

                consumed += 4;
                crc_pos += 4;
            }
        }
    }

    if (data[3] != sbc_crc8(crc_header, crc_pos-8)) // 8-bit dummy
    {
        return -3;
    }

    sbc_calculate_bits(frame, bits);

    for (blk = 0; blk < frame->blocks; blk++)
    {
        for (ch = 0; ch < frame->channels; ch++)
        {
            for (sb = 0; sb < frame->subbands; sb++)
            {
                audio_sample[blk][ch][sb] = 0;
#ifdef LOG_DATA
                save_audio_sample[blk][ch][sb] = 0;
#endif
                if (bits[ch][sb] != 0)
                {
                    for (bit = 0; bit < bits[ch][sb]; bit++)
                    {
                        int b;  /* A bit */
                        if (consumed > len * 8)
                        {
                            return -1;
                        }

                        b = (data[consumed / 8] >> (7 - (consumed % 8))) & 0x01;
                        audio_sample[blk][ch][sb] |= b << (bits[ch][sb] - bit - 1);

                        consumed++;
                    }
#ifdef LOG_DATA
                    save_audio_sample[blk][ch][sb] = audio_sample[blk][ch][sb];
#endif
                }
            }
        }
    }

#ifdef LOG_DATA
    save_audio_sample_to_file( frame );
#endif

    for (ch = 0; ch < frame->channels; ch++)
    {
        for (sb = 0; sb < frame->subbands; sb++)
        {
            levels[ch][sb] = (1 << bits[ch][sb]) - 1;
            scalefactor[ch][sb] = 2 << frame->scale_factor[ch][sb];
        }
    }

    for (blk = 0; blk < frame->blocks; blk++)
    {
        for (ch = 0; ch < frame->channels; ch++)
        {
            for (sb = 0; sb < frame->subbands; sb++)
            {
                if (levels[ch][sb] > 0)
                {
                    sb_sample_buf[blk][ch][sb] = (short)(
                        scalefactor[ch][sb] * ((audio_sample[blk][ch][sb] * 2.0 + 1.0) /
                                   levels[ch][sb] - 1.0));
                }
                else
                {
                    sb_sample_buf[blk][ch][sb] = 0;
                }
            }
        }
    }

    if (frame->channel_mode == JOINT_STEREO)
    {
        for (blk = 0; blk < frame->blocks; blk++)
        {
            for (sb = 0; sb < frame->subbands; sb++)
            {
                if (join & (0x01 << sb))
                {
                    sb_sample_buf[blk][0][sb] =
                        sb_sample_buf[blk][0][sb] + sb_sample_buf[blk][1][sb];
                    sb_sample_buf[blk][1][sb] =
                        sb_sample_buf[blk][0][sb] - 2 * sb_sample_buf[blk][1][sb];
                }
            }
        }
    }

    if (consumed % 8 != 0)
    {
        consumed += 8 - (consumed % 8);
    }

    return consumed/8;
}

