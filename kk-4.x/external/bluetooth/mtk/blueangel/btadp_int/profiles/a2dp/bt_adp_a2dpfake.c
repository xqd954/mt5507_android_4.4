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
#ifdef __BT_A2DP_PROFILE__

#include "string.h"
#include "bt_common.h"
#include "stdio.h"      /* Basic C I/O functions */
#include "bt_feature.h"
#include "bt.h"
#include "bluetooth_a2dp_struct.h"
#include "bluetooth_struct.h"
#include "eventmgr.h"
#include "btconfig.h"
#include "bttypes.h"
#include "bt_message.h"
#include "bt_adp_msg.h"
#include "a2dp.h"
#include "a2dp_inc.h"
#include "bt_adp_fs.h"

#ifndef BTMTK_ON_WISE
#ifndef BT_DB_PATH
#define BT_DB_PATH "\\@btmtk\\data"
#endif
#define BT_A2DP_SBC_FILE BT_DB_PATH"\\a2dp.sbc"

extern int encodec_sbc(
        U8 min_bit_pool,
        U8 block_len, // b0: 16, b1: 12, b2: 8, b3: 4
        U8 subband_num, // b0: 8, b1: 4
        U8 alloc_method, // b0: loudness, b1: SNR
        U8 sample_rate, // b0: 48000, b1: 44100, b2: 32000, b3: 16000
        U8 channel_mode // b0: joint stereo, b1: stereo, b2: dual channel, b3: mono
        );

S32 a2dp_src_sbc_file_hdl = -1;
U8 sbc_buffer[12000];
U32 sbc_size = 0;
U32 sbc_sent_offset = 0;
int sbc_frame_size = 0;
S32 btmtk_a2dp_fake_read(int handle, U8 *buffer, U32 size);

void bt_a2dp_open_sbc_source(bt_a2dp_audio_cap_struct *config)
{
    sbc_size = 0;
    #ifdef __LINUX_SUPPRESS_ERROR__
    #else
    sbc_frame_size = encodec_sbc(config->codec_cap.sbc.min_bit_pool,
        config->codec_cap.sbc.block_len,
        config->codec_cap.sbc.subband_num,        
        config->codec_cap.sbc.alloc_method,        
        config->codec_cap.sbc.sample_rate,
        config->codec_cap.sbc.channel_mode
        );
    #endif
    if(sbc_frame_size == -1)
        sbc_frame_size = 0;
    if(a2dp_src_sbc_file_hdl != -1)
    {
        /* File opened, close again */
        btmtk_fs_close(a2dp_src_sbc_file_hdl);
        sbc_size = 0;
    }

    if((a2dp_src_sbc_file_hdl = btmtk_fs_open((const U8 *)BT_A2DP_SBC_FILE, BTMTK_FS_READ_ONLY)) < 0)
    {
        sbc_size = 0;
        return;
    }
    sbc_sent_offset = 0;
    sbc_size = fread((U8 *)sbc_buffer, 1, sizeof(sbc_buffer), (FILE *)a2dp_src_sbc_file_hdl);
    btmtk_fs_close(a2dp_src_sbc_file_hdl);
    a2dp_src_sbc_file_hdl = -1;

}

U32 bt_a2dp_fake_query_payload_size(U32 prefer_size, U32 *payload_size)
{
    U32 sample_count = 0;
    U32 available_size = 0;
    if(sbc_size != 0)
    {
        if(sbc_frame_size ==0)
        {
            sbc_frame_size = 1;
        }
        if(sbc_size - sbc_sent_offset > prefer_size)
        {
            available_size = prefer_size;
            sample_count = available_size/sbc_frame_size;
        }
        else
        {
            available_size = sbc_size - sbc_sent_offset;
            sample_count = available_size/sbc_frame_size;
        }
        if(sample_count > 0)
        {
            *payload_size = sample_count * sbc_frame_size;
        }
        else
        {
            *payload_size = available_size;
            sample_count = 1;
        }        
        
    }
    else
        *payload_size = 0x01;
	return sample_count;
}


void bt_a2dp_fake_get_payload_data(U32 prefer_size, U8 *data)
{
    if(sbc_size != 0)
    {
        OS_MemCopy(data, sbc_buffer + sbc_sent_offset ,prefer_size);
        sbc_sent_offset += prefer_size;
        if(sbc_sent_offset >= sbc_size)
            sbc_sent_offset = 0;
    }
    else
    {
        data[0] = 0x00;
    }
}

U32 bt_a2dp_fake_sent_time_interval(void)
{
    if (sbc_sent_offset == 0)
        return 1500;
    else
        return 20;
}

#endif
#endif /* __BT_A2DP_PROFILE__ */
