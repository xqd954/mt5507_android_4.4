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


#ifndef _SBC_H_
#define _SBC_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SBC_SYNCWORD 0x9C
/* sampling frequency */
#define SBC_FS_16	0x00
#define SBC_FS_32	0x01
#define SBC_FS_44	0x02
#define SBC_FS_48	0x03

/* nrof_blocks */
#define SBC_NB_4	0x00
#define SBC_NB_8	0x01
#define SBC_NB_12	0x02
#define SBC_NB_16	0x03

/* channel mode */
#define SBC_CM_MONO		0x00
#define SBC_CM_DUAL_CHANNEL	0x01
#define SBC_CM_STEREO		0x02
#define SBC_CM_JOINT_STEREO	0x03

#define MONO		0x00
#define DUAL_CHANNEL	0x01
#define STEREO		0x02
#define JOINT_STEREO	0x03

/* allocation mode */
#define SBC_AM_LOUDNESS		0x00
#define SBC_AM_SNR		0x01

#define LOUDNESS		0x00
#define SNR		0x01

/* subbands */
#define SBC_SB_4	0x00
#define SBC_SB_8	0x01

#define SB_NUM 		8

typedef struct{
	float S[2][8];				/* Subband samples */
	float X[2][8];				/* Audio samples */
	float V[2][160], U[2][80], W[2][80];	/* Vectors */
}SBC_Decoder_State;

typedef struct{
        short X[2][80];
}SBC_Encoder_State;


#if 1
#define SB_CLIPPING 
typedef short SB_Type;
#else 
typedef double SB_Type;
#endif 

typedef unsigned char SCF_T;

typedef struct {
	int sampling_frequency;	/* in kHz */
	int blocks;
	int channels;
	int subbands;
	int channel_mode;
	int allocation_method;
	int bitpool;
	short *sb_sample;
	unsigned char sbc_frame_header_data;
	SCF_T scale_factor[2][8];
} SBC_Frame_State;

typedef short (*SB_BUF_T)[2][8];
typedef short (*PCM_BUF_T)[16*8];
int sbc_analyze_audio(int srcChannels, SBC_Encoder_State *sbc_state, SBC_Frame_State *frame, short *pcm_sample);
int sbc_synthesize_audio(SBC_Decoder_State *sbc_state, SBC_Frame_State *frame, short pcm_sample[2][16*8]);
void sbc_pack_frame(SBC_Frame_State *frame, unsigned char *frame_data,  int *frame_data_len); 
int sbc_unpack_frame(SBC_Frame_State *frame, unsigned char *data,  int len);


/* Common part in encoder and decoder */
extern const double sbc_proto_4_40[40];
extern const double sbc_proto_8_80[80];

unsigned char sbc_crc8(unsigned char * data, int len);
void sbc_calculate_bits(SBC_Frame_State *frame, int (*bits)[8]);

#ifdef __cplusplus
}
#endif

#endif
