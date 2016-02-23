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

#include <stdio.h>
#include <string.h>
#include "mtkbt_hmac_sha2.h"

typedef unsigned char u_int8;         /* unsigned integer,  8 bits long */
typedef unsigned short u_int16;  /* unsigned integer, 16 bits long */
typedef unsigned int u_int32;         /* unsigned integer, 32 bits long */
/* u_int is already defined in bionic/libc/include/sys/types.h */
//typedef unsigned int  u_int;          /* natural unsigned integer on processor */
typedef unsigned long int u_intL;     /* natural long unsigned integer on processor */

/* 
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                    \
{                                               \
    (n) = ( (u_intL) (b)[(i)    ] << 24 )        \
        | ( (u_intL) (b)[(i) + 1] << 16 )        \
        | ( (u_intL) (b)[(i) + 2] <<  8 )        \
        | ( (u_intL) (b)[(i) + 3]       );       \
}
#endif
#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                    \
{                                               \
    (b)[(i)    ] = (u_int8) ( (n) >> 24 );       \
    (b)[(i) + 1] = (u_int8) ( (n) >> 16 );       \
    (b)[(i) + 2] = (u_int8) ( (n) >>  8 );       \
    (b)[(i) + 3] = (u_int8) ( (n)       );       \
}
#endif



/*
 * Core SHA-256 functions
 */
void LM_SspEng_sha2_starts(sha2_context *ctx)
{
	ctx->total[0] = 0;
	ctx->total[1] = 0;

	ctx->state[0] = 0x6A09E667;
	ctx->state[1] = 0xBB67AE85;
	ctx->state[2] = 0x3C6EF372;
	ctx->state[3] = 0xA54FF53A;
	ctx->state[4] = 0x510E527F;
	ctx->state[5] = 0x9B05688C;
	ctx->state[6] = 0x1F83D9AB;
	ctx->state[7] = 0x5BE0CD19;
}

static const u_int32 K[64] = {
  0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5
, 0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174
, 0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA
, 0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967
, 0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85
, 0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070
, 0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3
, 0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
};

void LM_SspEng_sha2_process(sha2_context *ctx, const u_int8 *data/*[64]*/)
{
	u_intL temp1, temp2/*W[64]*/;
	u_intL W[80];
	u_intL tmp_ctx[8];
	int i, j;

#define SHR(x,n) ((x & 0xFFFFFFFF) >> n)
#define ROTR(x,n) (SHR(x,n) | (x << (32 - n)))

#define S0(x) (ROTR(x, 7) ^ ROTR(x,18) ^  SHR(x, 3))
#define S1(x) (ROTR(x,17) ^ ROTR(x,19) ^  SHR(x,10))

#define S2(x) (ROTR(x, 2) ^ ROTR(x,13) ^ ROTR(x,22))
#define S3(x) (ROTR(x, 6) ^ ROTR(x,11) ^ ROTR(x,25))

#define F0(x,y,z) ((x & y) | (z & (x | y)))
#define F1(x,y,z) (z ^ (x & (y ^ z)))

	i = 8;

	while (i > 0)
	{
		i--;
		GET_UINT32_BE(W[i], data, (i << 2));
		GET_UINT32_BE(W[i+8], data, ((i+8)<<2));
		tmp_ctx[i] = ctx->state[i];
	}

#define _A (tmp_ctx[(8-j) & 0x7])
#define _B (tmp_ctx[(9-j) & 0x7])
#define _C (tmp_ctx[(10-j) & 0x7])
#define _D (tmp_ctx[(11-j) & 0x7])
#define _E (tmp_ctx[(12-j) & 0x7])
#define _F (tmp_ctx[(13-j) & 0x7])
#define _G (tmp_ctx[(14-j) & 0x7])
#define _H (tmp_ctx[(15-j) & 0x7])

	for (i = 0; i < 64; i++)
	{
		j = i & 0x7;
		temp1 = _H + S3(_E) + F1(_E,_F,_G) + K[i] + W[i];
		temp2 = S2(_A) + F0(_A, _B, _C);
		_D += temp1; _H = temp1 + temp2;
		W[i+16] = S1(W[i+14]) + W[i+9] + S0(W[i+1]) + W[i];
	}
	

	i = 8;

	while (i > 0)
	{
		i--;
		ctx->state[i] += tmp_ctx[i];
	}
}

static void LM_SspEng_sha2_update(sha2_context *ctx, const u_int8 *input, u_int len)
{
	u_intL left, fill;

	if (len > 0){
        	left = ctx->total[0] & HASH_BLOCK_SIZE_MASK;
        	fill = HASH_BLOCK_SIZE - left;

        	ctx->total[0] += len;
        	ctx->total[0] &= 0xFFFFFFFF;

        	if(ctx->total[0] < len)
        		ctx->total[1]++;
        	
        	if(left && (len >= fill)) 
        	{
        		memcpy((void *)(ctx->buffer + left), input, fill);
        		len -= fill;
        		input  += fill;
        		LM_SspEng_sha2_process(ctx, ctx->buffer);
        		left = 0;
        	}
			fill = (len / HASH_BLOCK_SIZE);
        	while( fill-- )
        	{
        		LM_SspEng_sha2_process( ctx, input );
        		input  += HASH_BLOCK_SIZE;
        	}
			len &= HASH_BLOCK_SIZE_MASK;
        	if( len )
        	{
        		memcpy((void *)(ctx->buffer + left), input, len);
        	}
       }
}

void LM_SspEng_sha2_finish(sha2_context *ctx, u_int8 *digest/*[32]*/)
{
	int i = 8;
	u_intL padn;
	u_int8 sha2_padding[HASH_BLOCK_SIZE + 8];

	memset((void*)sha2_padding, 0, HASH_BLOCK_SIZE);
	sha2_padding[0] = 0x80;

	padn = ctx->total[0] & 0x3F;
	padn = ( padn < 56 ) ? ( 56 - padn ) : ( 120 - padn );

	PUT_UINT32_BE(((ctx->total[0] >> 29) | (ctx->total[1] <<  3)), &sha2_padding[padn], 0);
	PUT_UINT32_BE((ctx->total[0] <<  3),  &sha2_padding[padn], 4);
	padn += 8;
	
	LM_SspEng_sha2_update(ctx, sha2_padding, padn);
    
	while (i > 0)
	{
		i--;
		PUT_UINT32_BE(ctx->state[i], digest, (i<<2));
	}
}

void LM_SspEng_sha2_hmac(const u_int8 *buf, u_int buflen, u_int8 *key, u_int keylen, u_int8 *digest)
//void LM_SspEng_sha2_hmac(unsigned char *buf, unsigned int buflen, unsigned char *key, unsigned int keylen, unsigned char *digest)
{
	u_int i;
	sha2_context ctx;
	u_int8 k_ipad[64];
	u_int8 k_opad[64];
	u_int8 tmpbuf[32];

	memset(k_ipad, 0x36, 64); 
	memset(k_opad, 0x5C, 64); 		
	for (i = 0; i < keylen; i++) 
	{       
		k_ipad[i] ^= key[i];
		k_opad[i] ^= key[i];
	}

	LM_SspEng_sha2_starts(&ctx);
	LM_SspEng_sha2_update(&ctx, k_ipad, 64); 
	LM_SspEng_sha2_update(&ctx, buf, buflen);
	LM_SspEng_sha2_finish(&ctx, tmpbuf);

	LM_SspEng_sha2_starts(&ctx);
	LM_SspEng_sha2_update(&ctx, k_opad, 64);
	LM_SspEng_sha2_update(&ctx, tmpbuf, 32);
	LM_SspEng_sha2_finish(&ctx, digest);


	memset(k_ipad, 0, 64);
	memset(k_opad, 0, 64);
	memset(tmpbuf, 0, 32);
	memset(&ctx, 0, sizeof(sha2_context));
}

#if 0
void LM_SspEng_f1(u_int8 *C_Out, u_int8 *PK_In1, u_int8 *PK_In2, u_int8 *N_In1, u_int8 *ri_In1)
{
	int i;
	int keylen = 16; //key length is 128 bits (16 bytes) for function f1
	u_int8 hmac_sha2sum[32]; //output is 256 bits, thus 256/8=32 chars  			
	u_int8 msg[80];
	//msg = U||V||Y
	//U: 192 bits = 24 bytes
	//V: 192 bits = 24 bytes
	//Y: 8 bits = 1 bytes
	//total: 392 bits = 49 bytes	

	memset(msg, 0, 80);
	memcpy(msg, PK_In1, 24);
	memcpy(msg+24, PK_In2, 24);
	*(msg+48) = *ri_In1;

	LM_SspEng_sha2_hmac(msg, 49, N_In1, keylen, hmac_sha2sum);
	//take 128 MSB of the output 
	for ( i = 0; i < 16; i++)
	{
		C_Out[i] = hmac_sha2sum[i];
	}
}

void LM_SspEng_g(u_intL *V_Out, u_int8 *PK_In1, u_int8 *PK_In2, u_int8 *N_In1, u_int8 *N_In2)
{
	u_int8 sha2sum[32];
	sha2_context ctx;
	u_int8 msg[80];
	//msg = U||V||X||Y
	//U: 192 bits = 24 bytes
	//V: 192 bits = 24 bytes
	//X: 128 bits = 16 bytes
	//Y: 128 bits = 16 bytes
	//total: 640 bits = 80 bytes
	u_intL tmp;

	memset(msg, 0, 80);
	memcpy(msg, PK_In1, 24);
	memcpy(msg+24, PK_In2, 24);
	memcpy(msg+48, N_In1, 16);
	memcpy(msg+64, N_In2, 16);

	LM_SspEng_sha2_starts(&ctx);
	LM_SspEng_sha2_update(&ctx, msg, 80);
	LM_SspEng_sha2_finish(&ctx, sha2sum);

	GET_UINT32_BE(tmp, sha2sum, 28);
	*V_Out = tmp;
	*V_Out %= 1000000;
}

void LM_SspEng_f2(u_int8 *LK_Out, u_int8 *DHKey_str, u_int8 *N_In1, u_int8 *N_In2, 
		u_int8 *KeyID_In, u_int8 *Addr_In1, u_int8 *Addr_In2)
{

	int i;
	int keylen = 24; //key length is 192 bits (24 bytes) for function f2
	u_int8 hmac_sha2sum[32]; //output is 256 bits, thus 256/8=32 chars 
	u_int8 msg[48]; 
	//msg = U||V||X||Y||Z
	//U: 128 bits = 16 bytes
	//V: 128 bits = 16 bytes
	//X: 32 bits = 4 bytes
	//Y: 48 bits = 6 bytes
	//Z: 48 bits = 6 bytes
	//total: 384 bits = 48 bytes	

	memset(msg, 0, 48);
	memcpy(msg, N_In1, 16);
	memcpy(msg+16, N_In2, 16);
	memcpy(msg+32, KeyID_In, 4);
	for (i = 0; i < 6; i++)
	{
		msg[41-i] = *(Addr_In1 + i);
		msg[47-i] = *(Addr_In2 + i);
	}
//	memcpy(msg+36, Addr_In1, 6);
//	memcpy(msg+42, Addr_In2, 6);

	LM_SspEng_sha2_hmac(msg, 48, DHKey_str, keylen, hmac_sha2sum);
	//take 128 MSB of the output 
	for ( i = 0; i < 16; i++)
	{
		LK_Out[i] = hmac_sha2sum[i];
	}
}

void LM_SspEng_f3(u_int8 *E_Out, u_int8 *DHKey_str, u_int8 *N_In1, u_int8 *N_In2,
		u_int8 *r_In1, u_int8 *IOcap_A, u_int8 *Addr_In1, u_int8 *Addr_In2)
{		

	int i;
	int keylen = 24; //key length is 128 bits (16 bytes) for function f1
	u_int8 hmac_sha2sum[32]; //output is 256 bits, thus 256/8=32 chars 
	u_int8 msg[64];
	//msg = U||V||W||X||Y||Z
	//U: 128 bits = 16 bytes
	//V: 128 bits = 16 bytes
	//W: 128 bits = 16 bytes
	//X: 16 bits = 2 bytes
	//Y: 48 bits = 6 bytes
	//Z: 48 bits = 6 bytes
	//total: 496 bits = 62 bytes	

	memset(msg, 0, 64);
	memcpy(msg, N_In1, 16);
	memcpy(msg+16, N_In2, 16);
	memcpy(msg+32, r_In1, 16);
	memcpy(msg+48, IOcap_A, 3);
	for (i = 0; i < 6; i++)
	{
		msg[56-i] = *(Addr_In1 + i);
		msg[62-i] = *(Addr_In2 + i);
	}
//	memcpy(msg+51, Addr_In1, 6);
//	memcpy(msg+57, Addr_In2, 6);

	LM_SspEng_sha2_hmac( msg, 63, DHKey_str, keylen, hmac_sha2sum);
	//take 128 MSB of the output 
	for ( i = 0; i < 16; i++)
	{
		E_Out[i] = hmac_sha2sum[i];
	}
}

ecPoint_Affine *p_ec_Q;
Large_num_NAF *p_ec_Knaf;
ecPoint_Affine *p_ec_P;
elliptic_curve_para *p_ec_para;

uWord32 cycle_cnt_max = 3000;
Word32 bin_inv_flag = 1;

void LM_SspEng_ec_p192_mult(t_lmp_link* p_link)
{
	t_ec_state *ec_state = &g_ssp_config.ec_state;

	if (ec_state->main_state == EC_STATE_END)
	{
		LMssp_core_DH_Key_Ready(p_link);
	}
	else
	{
		ec_point_mult_wNAF_sub(p_ec_Q, p_ec_Knaf, p_ec_P, p_ec_para, bin_inv_flag, ec_state, cycle_cnt_max);
		LMtmr_Set_Timer(0x01, &LM_SspEng_ec_p192_mult, p_link, 1);
	}
}

void LM_SspEng_ec_p192(ecPoint_Affine *Q, 
				Large_num_NAF *Knaf,
				ecPoint_Affine *P,
				elliptic_curve_para *ec_para,
				uWord8 op_type,
				t_lmp_link *p_link)
{
#if (MTK_DEV_USE_LOOP_ECC_MULT==1)
	t_ec_state *ec_state = &g_ssp_config.ec_state;

	ec_state->main_state = EC_STATE_INIT;
	p_ec_Q = Q;
	p_ec_Knaf = Knaf;
	p_ec_P = P;
	p_ec_para = ec_para;

	if (op_type == EC_P192_DIRECT)
	{
		while (ec_state->main_state != EC_STATE_END)
		{
			ec_point_mult_wNAF_sub(p_ec_Q, p_ec_Knaf, p_ec_P, p_ec_para, bin_inv_flag, ec_state, cycle_cnt_max);
		}
		g_ssp_config.dh_key_ready = TRUE;
	}
	else
	{
		LMtmr_Set_Timer(0x01, &LM_SspEng_ec_p192_mult, p_link, 1);
	}
#else // non loop ecc_mult
	ec_point_mult_wNAF(Q, Knaf, P, ec_para, bin_inv_flag);
		g_ssp_config.dh_key_ready = TRUE;
#endif
}
#endif
