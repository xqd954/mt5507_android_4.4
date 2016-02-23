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


#if 0
const double sbc_proto_4_40[40] = {
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

const double sbc_proto_8_80[80] = {
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
#endif

/* A2DP specification: Appendix B, page 69 */
static const int sbc_offset4[4][4] = {
    { -1, 0, 0, 0 },
    { -2, 0, 0, 1 },
    { -2, 0, 0, 1 },
    { -2, 0, 0, 1 }
};

/* A2DP specification: Appendix B, page 69 */
static const int sbc_offset8[4][8] = {
    { -2, 0, 0, 0, 0, 0, 0, 1 },
    { -3, 0, 0, 0, 0, 0, 1, 2 },
    { -4, 0, 0, 0, 0, 0, 1, 2 },
    { -4, 0, 0, 0, 0, 0, 1, 2 }
};

/*
 * Calculates the CRC-8 of the first len bits in data
 */
static const unsigned char crc_table[256] = {
    0x00, 0x1D, 0x3A, 0x27, 0x74, 0x69, 0x4E, 0x53,
    0xE8, 0xF5, 0xD2, 0xCF, 0x9C, 0x81, 0xA6, 0xBB,
    0xCD, 0xD0, 0xF7, 0xEA, 0xB9, 0xA4, 0x83, 0x9E,
    0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B, 0x76,
    0x87, 0x9A, 0xBD, 0xA0, 0xF3, 0xEE, 0xC9, 0xD4,
    0x6F, 0x72, 0x55, 0x48, 0x1B, 0x06, 0x21, 0x3C,
    0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04, 0x19,
    0xA2, 0xBF, 0x98, 0x85, 0xD6, 0xCB, 0xEC, 0xF1,
    0x13, 0x0E, 0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40,
    0xFB, 0xE6, 0xC1, 0xDC, 0x8F, 0x92, 0xB5, 0xA8,
    0xDE, 0xC3, 0xE4, 0xF9, 0xAA, 0xB7, 0x90, 0x8D,
    0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65,
    0x94, 0x89, 0xAE, 0xB3, 0xE0, 0xFD, 0xDA, 0xC7,
    0x7C, 0x61, 0x46, 0x5B, 0x08, 0x15, 0x32, 0x2F,
    0x59, 0x44, 0x63, 0x7E, 0x2D, 0x30, 0x17, 0x0A,
    0xB1, 0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2,
    0x26, 0x3B, 0x1C, 0x01, 0x52, 0x4F, 0x68, 0x75,
    0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80, 0x9D,
    0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8,
    0x03, 0x1E, 0x39, 0x24, 0x77, 0x6A, 0x4D, 0x50,
    0xA1, 0xBC, 0x9B, 0x86, 0xD5, 0xC8, 0xEF, 0xF2,
    0x49, 0x54, 0x73, 0x6E, 0x3D, 0x20, 0x07, 0x1A,
    0x6C, 0x71, 0x56, 0x4B, 0x18, 0x05, 0x22, 0x3F,
    0x84, 0x99, 0xBE, 0xA3, 0xF0, 0xED, 0xCA, 0xD7,
    0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C, 0x7B, 0x66,
    0xDD, 0xC0, 0xE7, 0xFA, 0xA9, 0xB4, 0x93, 0x8E,
    0xF8, 0xE5, 0xC2, 0xDF, 0x8C, 0x91, 0xB6, 0xAB,
    0x10, 0x0D, 0x2A, 0x37, 0x64, 0x79, 0x5E, 0x43,
    0xB2, 0xAF, 0x88, 0x95, 0xC6, 0xDB, 0xFC, 0xE1,
    0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09,
    0x7F, 0x62, 0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C,
    0x97, 0x8A, 0xAD, 0xB0, 0xE3, 0xFE, 0xD9, 0xC4
};

unsigned char sbc_crc8(unsigned char * data, int len)
{
    unsigned char crc = 0x0f;
    int i;
    unsigned char octet;
    unsigned char *dataPtr;

    dataPtr = data;
    crc = crc_table[crc ^ *dataPtr++];
    crc = crc_table[crc ^ *dataPtr++];

    dataPtr++;
    for (i = (len >> 3) - 2 ; i > 0; i--)
        crc = crc_table[crc ^ *dataPtr++];

    octet = *dataPtr;
    for (i = 0; i < len % 8; i++) {
        char bit = ((octet ^ crc) & 0x80) >> 7;
        crc = ((crc & 0x7f) << 1) ^ (bit ? 0x1d : 0);
        octet = octet << 1;
    }
	return( crc );
}

/*
 * Code straight from the spec to calculate the bits array
 * Takes a pointer to the frame in question, a pointer to the bits array and the sampling frequency (as 2 bit integer)
 */
void sbc_calculate_bits(SBC_Frame_State *frame, int (*bits)[8])
{
    unsigned char sf;       /* Sampling frequency as temporary value for table lookup */

    sf = (frame->sbc_frame_header_data >> 6) & 0x03;

    if (frame->channel_mode == MONO || frame->channel_mode == DUAL_CHANNEL) {
        int bitneed[2][8], loudness, max_bitneed, bitcount, slicecount, bitslice;
        int ch, sb;

        for (ch = 0; ch < frame->channels; ch++)
        {
            max_bitneed = 0;
            if (frame->allocation_method == SNR)
            {
                for (sb = 0; sb < frame->subbands; sb++)
                {
                    int bitneed_t = frame->scale_factor[ch][sb];
                    bitneed[ch][sb] = bitneed_t;
                    if( max_bitneed<bitneed_t ) max_bitneed = bitneed_t;
                }
            }
            else
            {
                for (sb = 0; sb < frame->subbands; sb++)
                {
                    if (frame->scale_factor[ch][sb] == 0) {
                        bitneed[ch][sb] = -5;
                    }
                    else
                    {
                        if (frame->subbands == 4)
                        {
                            loudness = frame->scale_factor[ch][sb] - sbc_offset4[sf][sb];
                        }
                        else
                        {
                            loudness = frame->scale_factor[ch][sb] - sbc_offset8[sf][sb];
                        }
                        if (loudness > 0)
                        {
                            int bitneed_t = loudness / 2;
                            bitneed[ch][sb] = bitneed_t;
                            if( max_bitneed<bitneed_t ) max_bitneed = bitneed_t;
                        }
                        else
                        {
                            bitneed[ch][sb] = loudness;
                        }
                    }
                }
            }

            bitcount = 0;
            slicecount = 0;
            bitslice = max_bitneed + 1;
            do {
                bitslice--;
                bitcount += slicecount;
                slicecount = 0;
                for (sb = 0; sb < frame->subbands; sb++)
                {
                    if ((bitneed[ch][sb] > bitslice + 1) && (bitneed[ch][sb] < bitslice + 16))
                    {
                        slicecount++;
                    }
                    else if (bitneed[ch][sb] == bitslice + 1)
                    {
                        slicecount += 2;
                    }
                }
            } while (bitcount + slicecount < frame->bitpool);

            if (bitcount + slicecount == frame->bitpool)
            {
                bitcount += slicecount;
                bitslice--;
            }

            for (sb = 0; sb < frame->subbands; sb++)
            {
                if (bitneed[ch][sb] < bitslice + 2)
                {
                    bits[ch][sb] = 0;
                }
                else
                {
                    bits[ch][sb] = bitneed[ch][sb] - bitslice;
                    if (bits[ch][sb] > 16)
                        bits[ch][sb] = 16;
                }
            }

            sb = 0;
            while (bitcount < frame->bitpool && sb < frame->subbands)
            {
                if ((bits[ch][sb] >= 2) && (bits[ch][sb] < 16))
                {
                    bits[ch][sb]++;
                    bitcount++;
                }
                else if ((bitneed[ch][sb] == bitslice + 1) && (frame->bitpool > bitcount + 1))
                {
                    bits[ch][sb] = 2;
                    bitcount += 2;
                }
                sb++;
            }

            sb = 0;
            while (bitcount < frame->bitpool && sb < frame->subbands)
            {
                if (bits[ch][sb] < 16)
                {
                    bits[ch][sb]++;
                    bitcount++;
                }
                sb++;
            }

        }
    }
    else if (frame->channel_mode == STEREO || frame->channel_mode == JOINT_STEREO)
    {
        int bitneed[2][8], loudness, max_bitneed, bitcount, slicecount, bitslice;
        int ch, sb, sbit;
        int *bitneed_ptr, *offset_ptr, *bits_ptr;
        SCF_T *scale_ptr;

        max_bitneed = 0;
        if (frame->allocation_method == SNR)
        {
            for (ch = 0; ch < 2; ch++)
            {
                for (sb = 0; sb < frame->subbands; sb++)
                {
                    int bitneed_t = frame->scale_factor[ch][sb];
                    bitneed[ch][sb] = bitneed_t;
                    if( max_bitneed<bitneed_t ) max_bitneed = bitneed_t;
                }
            }
        }
        else
        {
            bitneed_ptr = (int*)bitneed;
            scale_ptr = (SCF_T *)frame->scale_factor;
            max_bitneed = 0;
            for (ch = 2; --ch >= 0; )
            {
                offset_ptr = (int*)sbc_offset8[sf];
                for (sb = frame->subbands; --sb >= 0; )
                {
                    if (*scale_ptr == 0)
                    {
                        *bitneed_ptr++ = -5;
                        offset_ptr++;
                        scale_ptr++;
                    }
                    else
                    {
                        loudness = (*scale_ptr++) - *offset_ptr++;

                        if (loudness > 0)
                        {
                            int bitneed_t = loudness / 2;
                            *bitneed_ptr++ = bitneed_t;
                             if( max_bitneed<bitneed_t ) max_bitneed = bitneed_t;
                        }
                        else
                        {
                            *bitneed_ptr++ = loudness;
                        }
                    }
                }
            }
        }

        sbit = frame->bitpool >> 4;
        bitcount = 0;
        slicecount = 0;
        if(sbit >= 2)
        {
            int A;
            bitslice = max_bitneed - sbit;
            bitneed_ptr = (int*)bitneed;
            for(ch = 16; --ch >=0;)
            {
                A = *bitneed_ptr++;
                if(A > bitslice)
                {
                    bitcount += (A- bitslice);
                    slicecount ++;
                }
            }
         }
         else
         {
             bitslice = max_bitneed;
         }

         do {
             int A;
             bitslice--;
             bitcount += slicecount;
             slicecount = 0;
             bitneed_ptr = (int*)bitneed;
             for(ch = 16; --ch >= 0; )
             {
                 A = (*bitneed_ptr++) - bitslice - 1 ;
                 if(A > 0 && A < 15)
                 {
                     slicecount ++;
                 }
                 else if(A == 0)
                 {
                     slicecount += 2;
                 }
             }
         } while (bitcount + slicecount < frame->bitpool);

         if (bitcount + slicecount == frame->bitpool)
         {
             bitcount += slicecount;
             bitslice--;
         }

         bitneed_ptr = (int*) bitneed;
         bits_ptr = (int*) bits;

         for(ch = 16; --ch >= 0; )
         {
             int A;
             A = (*bitneed_ptr++) - bitslice;
             if(A >= 2)
             {
                 if(A > 16)
                 {
                     A = 16;
                 }
             }
             else
             {
                 A = 0;
             }
             *bits_ptr++ = A;
         }

         ch = 0;
         sb = 0;
         while ((bitcount < frame->bitpool) && (sb < frame->subbands))
         {
             if ((bits[ch][sb] >= 2) && (bits[ch][sb] < 16))
             {
                 bits[ch][sb]++;
                 bitcount++;
             }
             else if ((bitneed[ch][sb] == bitslice + 1) && (frame->bitpool > bitcount + 1))
             {
                 bits[ch][sb] = 2;
                 bitcount += 2;
             }
             
             if (ch == 1)
             {
                 ch = 0;
                 sb++;
             }
             else
             {
                 ch = 1;
             }
         }

         ch = 0;
         sb = 0;
         while ((bitcount < frame->bitpool) && (sb < frame->subbands))
         {
             if (bits[ch][sb] < 16)
             {
                 bits[ch][sb]++;
                 bitcount++;
             }
             
             if (ch == 1)
             {
                 ch = 0;
                 sb++;
             }
             else
             {
                 ch = 1;
             }
         }
    }
}


