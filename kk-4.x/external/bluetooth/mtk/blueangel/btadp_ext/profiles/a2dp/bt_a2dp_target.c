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

#include "stdio.h"
#include "string.h"
#include "stdarg.h"


#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bt_a2dp_api.h"
#include "bluetooth_a2dp_message.h"

#ifdef BTMTK_ON_WISE
#include "bt_debug.h"
#endif


static void OS_Report(const char *format, ...)
{
    char buffer[200];   /* Output buffer */
    va_list args;

    va_start(args, format);
    _vsnprintf(buffer, 200, format, args);
    va_end(args);
//    kal("\n%s\n",buffer);
    bt_prompt_trace(MOD_BT, buffer);
}


typedef struct _BT_A2DP_TARGET_CTX
{
    U8  codec_status;
    U8 codec_opened;
    U8 audio_path_sbc;
}BT_A2DP_TARGET_CTX;

BT_A2DP_TARGET_CTX bt_a2dp_target;


U8 btmtk_a2dp_convent_to_cfg(
            bt_a2dp_audio_cap_struct *aud_cfg,
            U16 aud_fmt,
            U8 codec_type,
            U32 samplerate);

void btmtk_a2dp_target_initialized(void)
{
    memset(&bt_a2dp_target, 0, sizeof(bt_a2dp_target));

}

U8 btmtk_a2dp_check_codec_status(void)
{
#if 0
    if(bt_a2dp_target.codec_status == FALSE)
    {
        return 0;
    }
    else
    {
        return 1;
    }
#endif    
    if(bt_a2dp_target.codec_opened == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
void btmtk_a2dp_open_codec(U8 stream_hdl, bt_a2dp_audio_cap_struct *capability)
{
    OS_Report("SBC open btmtk_a2dp_open_codec:%02x",bt_a2dp_target.codec_status);
    if(bt_a2dp_target.audio_path_sbc == 0)
    {
        OS_Report("A2DP path is not SBC now, will not turn on sbc codec");
        return;
    }
    if(bt_a2dp_target.codec_status == FALSE)
    {
        OS_Report("SBC open codec");
        aud_send_bt_open_codec_req(MOD_MMI, 
                        capability,
                        stream_hdl, 
                        TRUE, 
                        0);
        bt_a2dp_target.codec_status = TRUE;
    }
}

void btmtk_a2dp_close_codec()
{
    OS_Report("SBC open btmtk_a2dp_close_codec:%02x",bt_a2dp_target.codec_status);
    if(bt_a2dp_target.codec_status == TRUE)
    {
        OS_Report("SBC close codec");
        aud_send_bt_close_codec_req(MOD_MMI);
        bt_a2dp_target.codec_status = FALSE;
    }
}

/*****************************************************************************
 * FUNCTION
 *  av_bt_get_aud_cfg_ex
 * DESCRIPTION
 *  
 * PARAMETERS
 *  sample_rate [IN]
 *  stereo      [IN]
 *  aud_cfg     [IN]
 * RETURNS
 *  kal_int32
 *****************************************************************************/
U32 btmtk_a2dp_get_aud_cfg(U32 sample_rate, bt_a2dp_audio_cap_struct *aud_cfg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    


    aud_cfg[0].codec_type = BT_A2DP_SBC;



    if (!btmtk_a2dp_convent_to_cfg(&aud_cfg[0], 0, BT_A2DP_SBC, sample_rate))
        return -1;

    return 0;
}



/*****************************************************************************
 * FUNCTION
 *  av_bt_calc_audio_config
 * DESCRIPTION
 *  
 * PARAMETERS
 *  aud_cfg     [IN]
 *  aud_fmt     [IN]
 *  codec_type  [IN]
 * RETURNS
 *  kal_bool
 *****************************************************************************/
U8 btmtk_a2dp_convent_to_cfg(
            bt_a2dp_audio_cap_struct *aud_cfg,
            U16 aud_fmt,
            U8 codec_type,
            U32 samplerate)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 result = 1; 

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (codec_type == BT_A2DP_SBC)
    {   /* SBC transcoding is necessary */
        bt_a2dp_sbc_codec_cap_struct *sbc = &aud_cfg->codec_cap.sbc;

        samplerate /= 1000;
        switch (samplerate)
        {
            case 7:
            case 8:
            case 15:
            case 16:
            case 31:
            case 32:
                sbc->sample_rate = 0x4; /* convert to 16000 sps */
                break;
            case 10:
            case 11:
            case 21:
            case 22:
            case 43:
            case 44:
                sbc->sample_rate = 0x2; /* convert to 16000 sps */
                break;
            case 12:
            case 23:
            case 24:
            case 47:
            case 48:
                sbc->sample_rate = 0x1; /* convert to 16000 sps */
                break;
            default:
                return KAL_FALSE;
        }


        sbc->channel_mode = 0x1;    /* default: joint-stereo */
        sbc->block_len = 0x1;       /* 16: fixed */
        sbc->subband_num = 0x1;     /* 8: fixed */
        sbc->alloc_method = 0x1;    /* loudness: fixed */
        SBC_InitiateBitPool(sbc->channel_mode, sbc->sample_rate, &sbc->min_bit_pool, &sbc->max_bit_pool);
    }
    else /* MP3 streamming, ignore */
    {     
    }

    if(result)
    {
        aud_cfg->codec_type = codec_type;
    }
    return result;
}

void btmtk_a2dp_set_codec_open_status(U8 status)
{
    if(status == 0x00)
        bt_a2dp_target.codec_opened = 0;
    else
        bt_a2dp_target.codec_opened = 1;
}

void btmtk_a2dp_switch_sbc_audio_path(U8 on_off)
{
    bt_a2dp_target.audio_path_sbc = on_off;
    if(on_off == 0x00)
    {
        btmtk_a2dp_close_codec();
        kal_sleep_task(10);
    }
    else
        btmtk_a2dp_sbc_audio_path_open();
    
}

