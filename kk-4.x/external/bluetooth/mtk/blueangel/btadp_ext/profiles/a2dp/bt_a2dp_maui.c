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
*  permission of MediaTek Inc. (C) 2006
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
 * bt_a2dp_maui.c
 *
 * Project:
 * --------
 * MAUI
 *
 * Description:
 * ------------
 * This file implements bt a2dp adaptation function
 *
 * Author:
 * -------
 * Dlight
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 */
#ifdef __BT_A2DP_PROFILE__

#include "kal_release.h"        /* Basic data type */
#include "stack_common.h"       /* message and module IDs */
#include "stack_msgs.h" /* enum for message IDs */
#include "app_ltlcom.h" /* Task message communiction */
#include "stacklib.h"   /* Basic type for dll, evshed, stacktimer */
#include "event_shed.h" /* Event scheduler */
#include "stack_timer.h"        /* Stack timer */
#include "stdio.h"      /* Basic C I/O functions */
#include "bttypes.h"
#include "bt_a2dp_struct.h"

#define send_msg_to_bt(src_id, msg_id, local_param_ptr) send_msg( (src_id), MOD_BT, BT_A2DP_SAP, (msg_id), (local_param_ptr) )

void send_msg(module_type src_id, module_type dst_id, U16 sap_id, U16 msg_id, void *local_param_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct *ilm_ptr = allocate_ilm(src_id);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ilm_ptr->src_mod_id = src_id;
    ilm_ptr->dest_mod_id = dst_id;
    ilm_ptr->sap_id = sap_id;
    ilm_ptr->msg_id = (msg_type) msg_id;
    ilm_ptr->local_para_ptr = (local_para_struct*) local_param_ptr;
    ilm_ptr->peer_buff_ptr = (peer_buff_struct*) NULL;

    msg_send_ext_queue(ilm_ptr);
}

/*****************************************************************************
 * FUNCTION
 *  bt_a2dp_send_stream_data_send_req
 * DESCRIPTION
 *  
 * PARAMETERS
 *  src_mod_id          [IN]        
 *  stream_handle       [IN]        
 *  codec               [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void bt_a2dp_send_stream_data_send_req(module_type src_mod_id, kal_uint8 stream_handle, A2DP_codec_struct *codec)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_a2dp_stream_data_send_req_struct *msg_p = (bt_a2dp_stream_data_send_req_struct*)
        construct_local_para(sizeof(bt_a2dp_stream_data_send_req_struct), TD_CTRL);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    msg_p->stream_handle = stream_handle;
    msg_p->codec = codec;

    send_msg_to_bt(src_mod_id, (kal_uint16) MSG_ID_BT_A2DP_STREAM_DATA_SEND_REQ, msg_p);
}



/* ========================================================== */
/* the function shall be in L1AUD */
#define A2DP_QOS_VERSION         2
#define NUM_PREV_BIT_RATES    4 /* max:8 */
#if (NUM_PREV_BIT_RATES>1)
static struct
{
    kal_uint32 br_prev[NUM_PREV_BIT_RATES - 1];
    kal_uint8 br_numerator[NUM_PREV_BIT_RATES];
    kal_uint8 br_denominator;
#if A2DP_QOS_VERSION == 2
    kal_uint8 qos_prev;
    kal_uint8 qos_threshold;
    kal_int32 bitrate_diff;
    kal_uint32 bitrate_step;
#endif /* A2DP_QOS_VERSION == 2 */ 
} qos_context;

#define A2DP_BITRATE_DEC_VALUE 6
#define A2DP_QOS_THRESHOLD 2
#define A2DP_BITRATE_INC_THRESHOLD 192


/*****************************************************************************
 * FUNCTION
 *  bt_a2dp_alg_set_bit_rate_from_qos_init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bit_rate_cur        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_a2dp_alg_set_bit_rate_from_qos_init(kal_uint32 bit_rate_cur)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int I;
    int L_32;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (I = 0; I < NUM_PREV_BIT_RATES - 1; I++)
    {
        qos_context.br_prev[I] = bit_rate_cur;
    }

    for (I = 0, L_32 = 1; I < NUM_PREV_BIT_RATES; I++, L_32 <<= 1)
    {
        qos_context.br_numerator[NUM_PREV_BIT_RATES - 1 - I] = (kal_uint8) L_32;
    }
    qos_context.br_denominator = (kal_uint8) (L_32 - 1);
#if A2DP_QOS_VERSION == 2
    qos_context.qos_prev = 0;
    qos_context.qos_threshold = A2DP_QOS_THRESHOLD;
    qos_context.bitrate_diff = (kal_int32) bit_rate_cur;
    qos_context.bitrate_step = 0;
#endif /* A2DP_QOS_VERSION == 2 */ 
}
#else /* (NUM_PREV_BIT_RATES>1) */ 


/*****************************************************************************
 * FUNCTION
 *  bt_a2dp_alg_set_bit_rate_from_qos_init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bit_rate_cur        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_a2dp_alg_set_bit_rate_from_qos_init(kal_uint32 bit_rate_cur)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    (void)bit_rate_cur;
    return;
}
#endif /* (NUM_PREV_BIT_RATES>1) */ 


/*****************************************************************************
 * FUNCTION
 *  bt_a2dp_alg_set_bit_rate_from_qos
 * DESCRIPTION
 *  
 * PARAMETERS
 *  qos                 [IN]        
 *  bit_rate_prev       [IN]        
 *  SetCodecBitRate     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_uint32 bt_a2dp_alg_set_bit_rate_from_qos(
            kal_uint8 qos,
            kal_uint32 bit_rate_prev,
            kal_uint32(*SetCodecBitRate) (kal_uint32))
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint32 bit_rate_cur;
    kal_int32 bit_rate_step;

#if (NUM_PREV_BIT_RATES>1)
    int I;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bit_rate_cur = bit_rate_prev * qos_context.br_numerator[0];
    for (I = 0; I < NUM_PREV_BIT_RATES - 1; I++)
    {
        bit_rate_cur += qos_context.br_prev[I] * qos_context.br_numerator[I + 1];
    }
    bit_rate_cur /= qos_context.br_denominator;
#else /* (NUM_PREV_BIT_RATES>1) */ 
    bit_rate_cur = bit_rate_prev;
#endif /* (NUM_PREV_BIT_RATES>1) */ 

    ASSERT(qos <= 10);  /* qos shall be in the range of [0 10) */

#if A2DP_QOS_VERSION == 1
    if (qos > 5)
    {
        bit_rate_step = ((kal_uint32) qos - 5) * bit_rate_cur / 10;
        bit_rate_cur = bit_rate_cur - bit_rate_step;
    }
    else if (qos < 5)
    {
        bit_rate_step = (5 - (kal_uint32) qos) * bit_rate_cur / 10;
        bit_rate_cur = bit_rate_cur + bit_rate_step;
    }
#elif A2DP_QOS_VERSION==2
    if (qos < qos_context.qos_prev)
    {
        bit_rate_step = -qos_context.bitrate_diff / (qos_context.qos_prev - qos);
    }
    else if (qos != 0)
    {
        bit_rate_step = 2 * qos_context.bitrate_step;
    }
    else
    {
        bit_rate_step = qos_context.bitrate_step;
    }
    if (qos_context.bitrate_step > 0)
    {
        qos_context.bitrate_step = (qos_context.bitrate_step + bit_rate_step + 1) >> 1;
    }
    else
    {
        qos_context.bitrate_step = A2DP_BITRATE_DEC_VALUE;
    }

    if (qos_context.bitrate_step > 3 * A2DP_BITRATE_DEC_VALUE)
    {
        qos_context.bitrate_step = 3 * A2DP_BITRATE_DEC_VALUE;
    }

    if (qos >= qos_context.qos_threshold)
    {
        bit_rate_step = qos_context.bitrate_step * (qos - qos_context.qos_threshold + 1);
        if (bit_rate_cur < bit_rate_step)
        {
            bit_rate_cur = 0;
        }
        else
        {
            bit_rate_cur = bit_rate_cur - bit_rate_step;
        }
    }
    else if (qos == 0)
    {
        if (bit_rate_cur > A2DP_BITRATE_INC_THRESHOLD)
        {
            bit_rate_step = 6;
        }
        else
        {
            bit_rate_step = ((A2DP_BITRATE_INC_THRESHOLD + 32 - bit_rate_cur + 16) >> 5) * 6;
        }
        bit_rate_cur = bit_rate_step +  /* (( bit_rate_step>6 )? bit_rate_step: 6) */
            ((bit_rate_prev < bit_rate_cur) ? bit_rate_prev : bit_rate_cur);
    }
    qos_context.qos_prev = qos;
#else 
#error Not defined!
#endif 

    if (SetCodecBitRate != NULL)
    {
        bit_rate_cur = SetCodecBitRate(bit_rate_cur);
    }
#if A2DP_QOS_VERSION == 2
    qos_context.bitrate_diff = bit_rate_cur - bit_rate_prev;
#endif 

#if (NUM_PREV_BIT_RATES>1)
    for (I = NUM_PREV_BIT_RATES - 3; I >= 0; I--)
    {
        qos_context.br_prev[I + 1] = qos_context.br_prev[I];
    }
    qos_context.br_prev[0] = bit_rate_prev;
#endif /* (NUM_PREV_BIT_RATES>1) */ 

    return bit_rate_cur;
}
#endif