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

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 * hfg_seq.c
 *
 * Project:
 * --------
 * MAUI
 *
 * Description:
 * ------------
 * This file handles hfg sequence number 
 *
 * Author:
 * -------
 * Elvis Lin
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * Oct 24 2008 mtk00560
 * [MAUI_01261987] [BT][CHE] Modify APIs and remove static variables
 * 
 *
 * Jul 29 2008 mtk00560
 * [MAUI_00804575] [BT] to add sequence numer for opening HF virtual port and using SBC encoder
 * 
 *
 * Jul 18 2008 mtk00560
 * [MAUI_00804575] [BT] to add sequence numer for opening HF virtual port and using SBC encoder
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_HFG_PROFILE__
#ifdef __BT_SEQ_SUPPORT__
#include "stdio.h"      		/* Basic C I/O functions */
#include "kal_release.h"        /* Basic data type */
#include "kal_trace.h"  		/* Trace Header Files */
#include "bluetooth_trc.h"      /* bt trace definition */
#include "bt_hfg.h"

//#include "init.h" 				/* MCU version */
#include "che_api.h"      	/* bt trace definition */

#define __BT_FILE__	BT_FILE_HFG_SEQ

/***************************************************************************** 
 * Definations
 ****************************************************************************/
#define SEQUENCE_NUM_0_LENGHT 16
#define SEQUENCE_NUM_1_LENGHT 16 
#define SEQUENCE_NUM_2_LENGHT 16 
/***************************************************************************** 
* External Variable
*****************************************************************************/
/***************************************************************************** 
* External functions
*****************************************************************************/
extern kal_char* release_verno(void);
extern int rand(void);

/***************************************************************************** 
* Globol Variable
*****************************************************************************/
/*****************************************************************************
 * FUNCTION
 *  hfga_get_sequence_number
 * DESCRIPTION
 *  to get the sequence number to know how to open HF virtual port
 * PARAMETERS
 *  seq1     [OUT]  the output sequence number 1 and the lenght is 16.     
 *  seq2     [OUT]  the output sequence number 2 and the lenght is 16.
 * RETURNS
 *  result   KAL_TRUE means that need to use sequence numbers to open HF virtual port
 *			 KAL_FALSE means that donot need to use sequence numbers to open HF vitual port, open it directly
 *****************************************************************************/
kal_bool hfga_get_sequence_number(kal_uint8 *seq1, kal_uint8 *seq2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    static STCHE hash_cntx;
    kal_uint8 input_0[SEQUENCE_NUM_0_LENGHT];
    kal_uint8 input_1[SEQUENCE_NUM_1_LENGHT]= 
		{'7', '6', '2', 'f', 'h', 
		 'z', '6', 'y', '8', 'q', 
		 'a', 'b', '8', '7', '9', 
		 'q'}; 
    kal_uint8 hash_input[SEQUENCE_NUM_0_LENGHT+SEQUENCE_NUM_1_LENGHT];
	kal_uint32 rand_num;   
	kal_bool result = KAL_TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

	if (0)
	{
		/* randomly decide if sequence numbers are required for opening HF virtual port */
		result = KAL_FALSE;
	}
	else
	{


		rand_num = rand();
		input_1[0]=  (rand_num&0xff000000 >> 24);
		input_1[7]=  (rand_num&0x00ff0000 >> 16);
		input_1[10]= (rand_num&0x0000ff00 >> 8);
		input_1[15]= (rand_num&0x000000ff);		
		
		/* input 0 */
		kal_mem_cpy((kal_uint8 *)input_0,(kal_uint8 *)release_verno(), SEQUENCE_NUM_0_LENGHT);
		/* seq 1 */
		kal_mem_cpy((kal_uint8 *)seq1,(kal_uint8 *)input_1, SEQUENCE_NUM_0_LENGHT);

		/* hash_input */
		kal_mem_cpy((kal_uint8 *)hash_input, (kal_uint8 *)input_0, SEQUENCE_NUM_0_LENGHT);	
		kal_mem_cpy((kal_uint8 *)(hash_input+SEQUENCE_NUM_0_LENGHT), (kal_uint8 *)input_1, SEQUENCE_NUM_1_LENGHT);	
		

	    ASSERT(((kal_int32)(&hash_cntx) & 0x03) == 0);

	    che_init(&hash_cntx,CHE_MD5);
	    che_process(&hash_cntx, 
					CHE_MD5, 
					CHE_MODE_NULL, 
					CHE_HASH, 
					(kal_uint8*)hash_input, 
					(kal_uint8*)seq2, 
					(SEQUENCE_NUM_0_LENGHT+SEQUENCE_NUM_1_LENGHT), 
					KAL_TRUE);

	    che_deinit(&hash_cntx);
		
		result = KAL_TRUE;

	    kal_trace(BT_TRACE_G5_HF, BT_HFG_ADP_HFP_RAND_NUM, rand_num);


	    kal_trace(
	        BT_TRACE_G5_HF,
	        BT_HFG_ADP_HFP_VERNO, 
	        input_0[0], input_0[1], input_0[2], input_0[3], input_0[4], input_0[5], input_0[6], input_0[7], 
	        input_0[8], input_0[9], input_0[10], input_0[11], input_0[12], input_0[13], input_0[14], input_0[15]);

	    kal_trace(
	        BT_TRACE_G5_HF,
	        BT_HFG_ADP_HFP_SEQ1, 
	        seq1[0], seq1[1], seq1[2], seq1[3], seq1[4], seq1[5], seq1[6], seq1[7], 
	        seq1[8], seq1[9], seq1[10],  seq1[11], seq1[12], seq1[13], seq1[14], seq1[15]);
	    kal_trace(
	        BT_TRACE_G5_HF,
	        BT_HFG_ADP_HFP_SEQ2, 
	        seq2[0], seq2[1], seq2[2], seq2[3], seq2[4], seq2[5], seq2[6], seq2[7], 
	        seq2[8], seq2[9], seq2[10],  seq2[11], seq2[12], seq2[13], seq2[14], seq2[15]);

		
	}

    return result;
}

#endif /* __BT_SEQ_SUPPORT__ */
#endif /* __BT_HFG_PROFILE__ */

