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
#include "bt_types.h"
#include "bt_ext_debug.h"

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
        fp = fopen( "audio_sample_enc.dat", "wb" );
        init = 0;
    }
    fwrite( save_audio_sample, sizeof(unsigned short), 16*2*8, fp );
}
#endif

static kal_int32 clz(kal_int32 absx)
{
    kal_int32 lz;
#if defined(__GNUC__) && defined(__arm__)    
    asm( "clz %[lz], %[absx] \n" 
    : [lz]"=r"(lz)
    : [absx]"r"(absx));
#else
    kal_int32 j = 1 << 31;
    for(lz=0; lz<32; lz++, j>>=1)
      if( absx&j ) break;
#endif
    return lz;
}

void sbc_pack_frame(SBC_Frame_State *frame, unsigned char *data, int *frame_data_len)
{
    int ch, sb, blk, i; /* channel, subband, block counters */
    int ch_num, blk_num;
    int bits[2][8];     /* bits distribution */
    SCF_T *scale_factor_ptr;
    short *sb_data_ptr;
    unsigned char *dataPtr;
    unsigned int data32;
    int left_bit_num;

    blk_num = frame->blocks;
    ch_num = frame->channels;

    dataPtr = data;
    *dataPtr ++ = SBC_SYNCWORD;
    *dataPtr ++ = frame->sbc_frame_header_data;
    *dataPtr ++ = frame->bitpool;
    dataPtr ++; // skip crc now , Can't fill in crc yet */


    for( ch=ch_num; --ch>=0; )
    {
        int max_value, scale_factor, abs_sb_data;
        scale_factor_ptr = frame->scale_factor[ch] + frame->subbands - 1;

        for( sb=frame->subbands; --sb>=0; )
        {
            max_value = 0;
            sb_data_ptr = (short*)frame->sb_sample + (ch << 3) + sb;
            for( blk=blk_num; --blk>=0; )
            {
               abs_sb_data = *sb_data_ptr; sb_data_ptr += 16;
               if( abs_sb_data<0 ) abs_sb_data = -abs_sb_data;
               if( max_value<abs_sb_data ) max_value = abs_sb_data;
            }
            if( max_value>0x7fff ) max_value = 0x7fff; // saturation
#if defined(WIN32)   // PC version
            scale_factor = 0;
            while(max_value > (2<<scale_factor)) scale_factor ++;
            *scale_factor_ptr-- = scale_factor;
#else // ARM version
           if(max_value > 1)
           {
               //tina 
               //int lz;
               //__asm { clz lz,max_value-1; }
               int lz = clz( max_value-1 );
               *scale_factor_ptr-- = 31 - lz ;
           }
           else
              *scale_factor_ptr-- = 0;
           //bt_ext_log("[A2DP] sbc_pack_frame2. scal_factor=%d. max_value=%d", *(scale_factor_ptr+1), max_value);
#endif
        }
    }

    if( frame->channel_mode==SBC_CM_JOINT_STEREO )
    {
        SCF_T *sfactor0_ptr, *sfactor1_ptr;
        short j_sb_data[32];
        int join;

        join = 0;
        sfactor0_ptr = frame->scale_factor[0] + frame->subbands - 2;
        sfactor1_ptr = frame->scale_factor[1] + frame->subbands - 2;
        for (sb = frame->subbands - 1; --sb >= 0 ; )
        {
            int  max_value0, max_value1, sfactor0, sfactor1;
            short *j_sb_data_ptr;

            max_value0 = 0;
            max_value1 = 0;
            j_sb_data_ptr = (short *)j_sb_data;
            sb_data_ptr = frame->sb_sample + sb;
            for(blk = blk_num; --blk >= 0; )
            {
                int ST0, ST1, JST0, JST1;
                
                ST0 = sb_data_ptr[0]; // L-CH
                ST1 = sb_data_ptr[8]; // R-CH
                JST0 = ( ST0+ST1 ) / 2; // M-CH
                JST1 = ( ST0-ST1 ) / 2; // S-CH
                *j_sb_data_ptr++ = (short)JST0;
				*j_sb_data_ptr++ = (short)JST1;

                if(JST0 < 0)
                    JST0 = -JST0;
                if(max_value0 < JST0)
                    max_value0 = JST0;
                
                if(JST1 < 0)
                    JST1 = -JST1;
                if(max_value1 < JST1)
                    max_value1 = JST1;
                
                sb_data_ptr+= 16;
            }
#if defined(WIN32)  // PC version
            sfactor0 = 0;
            while(max_value0 > (2 << sfactor0)) sfactor0 ++;

            sfactor1 = 0;
            while(max_value1 > (2 << sfactor1)) sfactor1 ++;
#else  // ARM version
            if(max_value0 > 1)
            {
                //tina 
                //int lz;
                //__asm { clz lz,max_value0-1; }
                int lz = clz( max_value0-1 );
                sfactor0 = 31 - lz ;
            }
            else
                sfactor0 = 0;
            
            if(max_value1 > 1)
            {
                //tina 
                //int lz;
                //__asm { clz lz,max_value1-1; }
                int lz = clz( max_value1-1 );
                sfactor1 = 31 - lz ;
            }
            else
                sfactor1 = 0;
#endif

// modify by Yi for CR: ALPS00079723 / ALPS00080681
// disable Joint Stereo mode
#if 1
            if (frame->bitpool <= 30)    // ALPS00292221 -- enable Joint Stereo Mode if bitpool less then 30
            {
            if( (( (2<< (*sfactor0_ptr)) + (2<< (*sfactor1_ptr)) )> // L+R
                ( (2<<sfactor0) + (2<<sfactor1) )) && *sfactor0_ptr!=0 && *sfactor1_ptr !=0 ) // M+S
            {
                join |= 1 << (7-sb);
                *sfactor0_ptr = sfactor0;
                *sfactor1_ptr = sfactor1;
                
                j_sb_data_ptr = (short*)j_sb_data;
                sb_data_ptr = (short*)(frame->sb_sample) + sb;
                for(blk = blk_num; --blk >= 0; )
                {
                    *sb_data_ptr = *j_sb_data_ptr++; sb_data_ptr += 8;
                    *sb_data_ptr = *j_sb_data_ptr++; sb_data_ptr += 8;
                }
            }
            }
#endif
            sfactor0_ptr--;
            sfactor1_ptr--;
        }
        *dataPtr++ = join;
    }

    for( i=0; i<ch_num; i++ ){
        int j;
        SCF_T *psf = frame->scale_factor[i];
        for( j=frame->subbands; j>0; j-=2)
        {
            unsigned int temp = (*psf++)<<4;
            temp += *psf++;
            *dataPtr++ = temp;
        }            
    }

    {
        int crc_len;
        if (frame->channel_mode == SBC_CM_JOINT_STEREO)
            crc_len = 88;
        else if(frame->channel_mode == SBC_CM_MONO)
            crc_len = 48;
        else
            crc_len = 80;
        data[3] = sbc_crc8( data + 1, crc_len );

/* KH 20091216. Should use THIS code
        if (frame->channel_mode == SBC_CM_JOINT_STEREO)
            crc_len = 16 + frame->subbands * frame->channels * 4 + frame->subbands 
        else
            crc_len = 16 + frame->subbands * frame->channels * 4 
*/
    }

    sbc_calculate_bits(frame, bits);

    left_bit_num = 32;
    data32 = 0;

    sb_data_ptr = (short*)frame->sb_sample;
    for( blk=blk_num; --blk>=0; )
    {
        int *bits_ptr;
        bits_ptr = (int*)bits;
        scale_factor_ptr = (SCF_T *)frame->scale_factor;
        for( ch=ch_num; --ch>=0; )
        {
            for( sb=frame->subbands; --sb>=0; )
            {
                int bit_num, scale_factor;
                U32 tmp_value, aud_bits;
                bit_num =*bits_ptr++;
                if(bit_num>0)
                {
                    scale_factor = *scale_factor_ptr++;
                    tmp_value = (*sb_data_ptr++) + (2 << scale_factor);
                    tmp_value = (tmp_value << bit_num) - tmp_value;
                    aud_bits = tmp_value >> (scale_factor + 2);
#ifdef LOG_DATA
                    save_audio_sample[blk_num-blk-1][ch_num-ch-1][frame->subbands-sb-1] = aud_bits;
#endif
    
                    left_bit_num -= bit_num;
                    if(left_bit_num > 0)
                    {
                        data32 |= aud_bits << left_bit_num;
                    }
                    else
                    {
                        bit_num = -left_bit_num;
                        data32 |= aud_bits >> bit_num;
                        *dataPtr++ = data32 >> 24;
                        *dataPtr++ = (data32 >> 16) & 0xFF;
                        *dataPtr++ = (data32 >> 8) & 0xFF;
                        *dataPtr++ = data32 & 0xFF;
                        left_bit_num += 32;
                        aud_bits &= (1 << bit_num)-1;
                        data32 = aud_bits << left_bit_num;
                    }
                }
                else
                {
#ifdef LOG_DATA
                    save_audio_sample[blk_num-blk][ch_num-ch][frame->subbands-sb] = 0;
#endif
                    sb_data_ptr++;
                    scale_factor_ptr++;
                }
            }
            sb_data_ptr += 8 - frame->subbands;
        }
        sb_data_ptr += ( 2 - ch_num ) * 8;
    }

    if(left_bit_num < 32)
    {
        *dataPtr++ = data32 >> 24;
        if(left_bit_num < 24)
        {
           *dataPtr++ = (data32 >> 16) & 0xFF;
           if(left_bit_num < 16)
           {
              *dataPtr++ = (data32 >> 8) & 0xFF;              
              if(left_bit_num < 8)
              {
                 *dataPtr++ = data32 & 0xFF;
              }
           }
        }
    }
    *frame_data_len = (dataPtr - data)*sizeof(*data);

#ifdef LOG_DATA
    save_audio_sample_to_file( frame );
#endif
}


