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
/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bt_adp_a2dp.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Tina Shen
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
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
//#include "btbm_adp.h"
//#include "btbm_export.h"
//#include "sdap.h"
//#include "bt_jsr82_setting.h"
//#if defined (__BT_2_1_BLOCK_DEVICE__)
//#include "btbmBlocklist.h"
//#endif 
//#include "bt_maui_btbm.h"

#include "bt_adp_msg.h"
#include "a2dp.h"
#include "a2dp_inc.h"

static U8 Is_Sco_Connecting = 0;

extern void a2dp_adp_event_init(void);


#ifdef BTMTK_ON_LINUX
void BTA2DPADPStreamDataSendReq(U8 stream_handle, U8 *data, U16 len, U32 sample_count);
#else
void BTA2DPADPStreamDataSendReq(U8 stream_handle, A2DP_codec_struct *codec);
#endif
void A2DP_SignalConnectRes(bt_addr_struct *device_addr, U8 accept);

BOOL bt_adp_a2dp_unpack_audio_cap(A2dpCodec *src, bt_a2dp_audio_cap_struct *dst);


/***************
Internal Globals 
****************/

/***************
External Globals 
****************/
void a2dp_set_sco_state(U8 state)
{
    Report(("a2dp_set_sco_state(%u)", state));
    Is_Sco_Connecting = state;
}

void a2dp_adp_init(void)
{
    a2dp_adp_event_init();
}


void a2dp_adp_deinit(void)
{
}


BOOL bt_adp_a2dp_validate_audio_cap(bt_a2dp_audio_cap_struct *audio_cap)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 codec_type = audio_cap->codec_type;
    BOOL result = TRUE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (codec_type)
    {
        case BT_A2DP_SBC:
        {
            bt_a2dp_sbc_codec_cap_struct *sbc = &audio_cap->codec_cap.sbc;

            if (sbc->sample_rate > 15 || sbc->sample_rate == 0 ||
                sbc->channel_mode > 15 || sbc->channel_mode == 0 ||
                sbc->block_len > 15 || sbc->block_len == 0 ||
                sbc->subband_num > 3 || sbc->subband_num == 0 || sbc->alloc_method > 3 || sbc->alloc_method == 0)
            {
                result = FALSE;
            }
            break;
        }
        case BT_A2DP_MP3:
        {
            bt_a2dp_mp3_codec_cap_struct *mp3 = &audio_cap->codec_cap.mp3;

            if (mp3->layer > 7 || mp3->layer == 0 ||
                mp3->channel_mode > 15 || mp3->channel_mode == 0 ||
                mp3->sample_rate > 63 || mp3->sample_rate == 0 || mp3->bit_rate > 32767 || mp3->bit_rate == 0)
            {
                result = FALSE;
            }
            break;
        }
        case BT_A2DP_AAC:
        {
            bt_a2dp_aac_codec_cap_struct *aac = &audio_cap->codec_cap.aac;

            if (aac->sample_rate > 4095 || aac->sample_rate == 0 ||
                aac->channels > 3 || aac->channels == 0 || aac->bit_rate > ((1 << 23) - 1))
            {
                result = FALSE;
            }
            break;
        }
        case BT_A2DP_ATRAC:
        {
            bt_a2dp_atrac_codec_cap_struct *atrac = &audio_cap->codec_cap.atrac;

            if (atrac->version > 7 || atrac->version == 0 ||
                atrac->channel_mode > 7 || atrac->channel_mode == 0 ||
                atrac->sample_rate > 3 || atrac->sample_rate == 0 ||
                atrac->bit_rate > ((1 << 19) - 1) || atrac->bit_rate == 0)
            {
                result = FALSE;
            }
            break;
        }
        default:
            result = FALSE;
            break;
    }

    return result;
}




BOOL bt_adp_a2dp_unpack_audio_cap(A2dpCodec *src, bt_a2dp_audio_cap_struct *dst)
{
    BOOL found = TRUE;
    U8 codec_type = src->codecType;
    U8 *codec = &src->elements[0];

    switch (codec_type)
    {
        case AVDTP_CODEC_TYPE_SBC:
        {
            bt_a2dp_sbc_codec_cap_struct *sbc = &dst->codec_cap.sbc;

            dst->codec_type = BT_A2DP_SBC;
            sbc->sample_rate = codec[0] >> 4;
            sbc->channel_mode = codec[0] & 0xf;
            sbc->block_len = codec[1] >> 4;
            sbc->subband_num = (codec[1] & 0xc) >> 2;
            sbc->alloc_method = codec[1] & 0x3;
            sbc->min_bit_pool = codec[2];
            sbc->max_bit_pool = codec[3];
            break;
        }
        case AVDTP_CODEC_TYPE_MPEG1_2_AUDIO:
        {
            bt_a2dp_mp3_codec_cap_struct *mp3 = &dst->codec_cap.mp3;

            dst->codec_type = BT_A2DP_MP3;
            mp3->layer = codec[0] >> 5;
            mp3->CRC = (BOOL)((codec[0] & 0x10) ? TRUE : FALSE);
            mp3->channel_mode = codec[0] & 0xf;
            mp3->MPF = (BOOL)((codec[1] & 0x40) ? TRUE : FALSE);
            mp3->sample_rate = codec[1] & 0x3f;
            mp3->VBR = (BOOL)((codec[2] & 0x80) ? TRUE : FALSE);
            mp3->bit_rate = (((U16) codec[2] & 0x7f) << 8) + codec[3];
            break;
        }
        case AVDTP_CODEC_TYPE_MPEG2_4_AAC:
        {
            bt_a2dp_aac_codec_cap_struct *aac = &dst->codec_cap.aac;

            dst->codec_type = BT_A2DP_AAC;
            aac->object_type = codec[0];
            aac->sample_rate = (((U16) codec[1]) << 4) + (codec[2] >> 4);
            aac->channels = (codec[2] & 0xc) >> 2;
            aac->VBR = (BOOL)((codec[3] & 0x80) ? TRUE : FALSE);
            aac->bit_rate = (((U32) codec[3] & 0x7f) << 16) + (((U32) codec[4]) << 8) + codec[5];
            break;
        }
        case AVDTP_CODEC_TYPE_ATRAC:
        {
            bt_a2dp_atrac_codec_cap_struct *atrac = &dst->codec_cap.atrac;

            dst->codec_type = BT_A2DP_ATRAC;
            atrac->version = codec[0] >> 5;
            atrac->channel_mode = (codec[0] & 0x1c) >> 2;
            atrac->sample_rate = (codec[1] & 0x30) >> 4;
            atrac->VBR = (BOOL)((codec[1] & 0x8) ? TRUE : FALSE);
            atrac->bit_rate = (((U32) codec[1] & 0x7) << 16) + (((U32) codec[2]) << 8) + codec[3];
            atrac->max_sul = (((U16) codec[4]) << 8) + codec[5];
            break;
        }
        case AVDTP_CODEC_TYPE_NON_A2DP:
        {
            dst->codec_type = BT_A2DP_NON_A2DP;
            return found;
        }
        default:
            found = FALSE;
            break;
    }

    if (found)
    {
        if (!bt_adp_a2dp_validate_audio_cap(dst))
        {
            found = FALSE;
        }
    }

    return found;
}



void a2dpa_sendMsg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{
    //ilm_struct *ilmPtr;

    if (dstMod == 0)
    {
    #ifdef MTK_BT_DEBUG_TRACE
        //kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_NOT_READY);
    #endif 
        return;
    }

    if (local_para != NULL)
    {
        BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
    }
    else
    {
        BT_SendMessage(msg, dstMod, NULL, 0);
    }
}


void a2dp_adp_message_handler(ilm_struct *ilm_ptr)
{
    void *msg_p = (void*)ilm_ptr->local_para_ptr;

    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_A2DP_STREAM_DATA_SEND_REQ:
        	  if (Is_Sco_Connecting) break;

            BTA2DPADPStreamDataSendReq(
                    ((bt_a2dp_stream_data_send_req_struct*) msg_p)->stream_handle,
                    ((bt_a2dp_stream_data_send_req_struct*) msg_p)->data,
                    ((bt_a2dp_stream_data_send_req_struct*) msg_p)->len, 
                    ((bt_a2dp_stream_data_send_req_struct*) msg_p)->sample_count);            

            break;

        case MSG_ID_BT_A2DP_STREAM_START_REQ:
            
            // Streamming_Handle = ((bt_a2dp_stream_start_req_struct*) msg_p)->stream_handle;

            A2DP_StreamStartReq(((bt_a2dp_stream_start_req_struct*) msg_p)->stream_handle);
            break;
            
#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER

        case MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_START_REQ:
            
            A2DP_FMContrllerStartReq(((bt_a2dp_stream_start_req_struct*) msg_p)->stream_handle);
			break;
            
		case MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_STOP_REQ:
            
            A2DP_FMContrllerStopReq(((bt_a2dp_stream_start_req_struct*) msg_p)->stream_handle);
			break;
            
		case MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_SUSPEND_REQ:
            
			A2DP_FMContrllerSuspendReq(((bt_a2dp_stream_start_req_struct*) msg_p)->stream_handle);
			break;
            
		case MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_RESUME_REQ:
            
			A2DP_FMContrllerResumeReq(((bt_a2dp_stream_start_req_struct*) msg_p)->stream_handle);
			break;
            
#endif

        case MSG_ID_BT_A2DP_STREAM_START_RES:
            
            A2DP_StreamStartRes(
                ((bt_a2dp_stream_start_res_struct*) msg_p)->result,
                ((bt_a2dp_stream_start_res_struct*) msg_p)->stream_handle);
#if 0           
            if(((bt_a2dp_stream_start_res_struct*) msg_p)->result == BT_A2DP_RESULT_OK)
            {
                Streamming_Handle = ((bt_a2dp_stream_start_res_struct*) msg_p)->stream_handle;
            }
#endif
            break;

        case MSG_ID_BT_A2DP_STREAM_PAUSE_REQ:

            A2DP_StreamSuspendReq(((bt_a2dp_stream_pause_req_struct*) msg_p)->stream_handle);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_PAUSE_RES:

            A2DP_StreamSuspendRes(
                ((bt_a2dp_stream_pause_res_struct*) msg_p)->result,
                ((bt_a2dp_stream_pause_res_struct*) msg_p)->stream_handle);
            break;
        case MSG_ID_BT_A2DP_STREAM_RECONFIG_REQ:
           
            A2DP_StreamReconfigReq(
                ((bt_a2dp_stream_reconfig_req_struct*) msg_p)->stream_handle,
                &((bt_a2dp_stream_reconfig_req_struct*) msg_p)->audio_cap);
            
            break;
            
        case MSG_ID_BT_A2DP_STREAM_RECONFIG_RES:
            
            A2DP_StreamReconfigRes(
                ((bt_a2dp_stream_reconfig_res_struct*) msg_p)->result,
                ((bt_a2dp_stream_reconfig_res_struct*) msg_p)->stream_handle);
            break;
            
        case MSG_ID_BT_A2DP_ACTIVATE_REQ:
            
            A2DP_Activate(((bt_a2dp_activate_req_struct*) msg_p)->local_role,
                ((bt_a2dp_activate_req_struct*) msg_p)->sep_num,
                ((bt_a2dp_activate_req_struct*) msg_p)->local_sep);
            break;
            
        case MSG_ID_BT_A2DP_DEACTIVATE_REQ:
            
            A2DP_Deactivate();
            break;
            
        case MSG_ID_BT_A2DP_STREAM_OPEN_REQ:

            A2DP_StreamOpenReq(&((bt_a2dp_stream_open_req_struct*) msg_p)->device_addr);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_OPEN_RES:
            
            A2DP_StreamOpenRes(
                ((bt_a2dp_stream_open_res_struct*) msg_p)->accept,
                ((bt_a2dp_stream_open_res_struct*) msg_p)->stream_handle);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_CLOSE_REQ:

            A2DP_StreamCloseReq(((bt_a2dp_stream_close_req_struct*) msg_p)->stream_handle);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_CLOSE_RES:

            A2DP_StreamCloseRes(
                ((bt_a2dp_stream_close_res_struct*) msg_p)->result,
                ((bt_a2dp_stream_close_res_struct*) msg_p)->stream_handle);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_ABORT_REQ:
            
            A2DP_StreamAbortReq(((bt_a2dp_stream_abort_req_struct*) msg_p)->stream_handle);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_ABORT_RES:
            
            A2DP_StreamAbortRes(((bt_a2dp_stream_abort_res_struct*) msg_p)->stream_handle);
            break;

        default:
            kal_trace(TRACE_GROUP_1, BTLOG_NONDIRECT_RETURN_MESSAGE);
            break;
            
    }
}


void BTA2DPAdpActivateCfm(U16 result)
{
    bt_a2dp_activate_cnf_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_activate_cnf_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_activate_cnf_struct),
                                                    TD_UL);
    bta2dp_adp_msg->result = result;

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_ACTIVATE_CNF,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpDeactivateCfm(U16 result)
{
    bt_a2dp_deactivate_cnf_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_deactivate_cnf_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_deactivate_cnf_struct),
                                                    TD_UL);
    bta2dp_adp_msg->result = result;

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_DEACTIVATE_CNF,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpSetReconfigurationCfm(U16 result, U8 stream_handle)
{
    bt_a2dp_stream_reconfig_cnf_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_reconfig_cnf_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_reconfig_cnf_struct),
                                                    TD_UL);

    bta2dp_adp_msg->result = result;
    bta2dp_adp_msg->stream_handle = stream_handle;

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_RECONFIG_CNF,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpSetReconfigurationInd(U8 stream_handle, A2dpCodec *audio_cap)
{
    bt_a2dp_stream_reconfig_ind_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_reconfig_ind_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_reconfig_ind_struct),
                                                    TD_UL);

    bta2dp_adp_msg->stream_handle = stream_handle;
    bt_adp_a2dp_unpack_audio_cap(audio_cap, &bta2dp_adp_msg->audio_cap);    

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_RECONFIG_IND,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpStreamOpenCfm(U16 result, 
    U8 stream_handle, 
    bt_addr_struct *device_addr, 
    U8 *dev_name, U8 name_len)
{
    bt_a2dp_stream_open_cnf_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_open_cnf_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_open_cnf_struct),
                                                    TD_UL);

    bta2dp_adp_msg->result = result;
    bta2dp_adp_msg->stream_handle = stream_handle;
    memcpy(&bta2dp_adp_msg->device_addr, device_addr, sizeof (bt_addr_struct));
    memcpy(bta2dp_adp_msg->device_name, dev_name, name_len + 1);
    bta2dp_adp_msg->device_name_len = name_len;

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_OPEN_CNF,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpStreamOpenInd(U8 stream_handle,
    bt_addr_struct *device_addr, 
    U8 *dev_name, U8 name_len)
{
    bt_a2dp_stream_open_ind_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_open_ind_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_open_ind_struct),
                                                    TD_UL);

    bta2dp_adp_msg->stream_handle = stream_handle;
    memcpy(&bta2dp_adp_msg->device_addr, device_addr, sizeof (bt_addr_struct));
    memcpy(bta2dp_adp_msg->device_name, dev_name, name_len + 1);
    bta2dp_adp_msg->device_name_len = name_len;

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_OPEN_IND,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpStreamStartCfm(U16 result, U8 stream_handle, U16 prefer_size, bt_a2dp_audio_cap_struct *config)
{
    bt_a2dp_stream_start_cnf_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_start_cnf_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_start_cnf_struct),
                                                    TD_UL);

    bta2dp_adp_msg->result = result;
    bta2dp_adp_msg->stream_handle = stream_handle;
    bta2dp_adp_msg->prefer_size = prefer_size;
    memcpy(&bta2dp_adp_msg->current_config, config, sizeof(bt_a2dp_audio_cap_struct));

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_START_CNF,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpStreamStartInd(U8 stream_handle, U16 prefer_size, bt_a2dp_audio_cap_struct *config)
{
    bt_a2dp_stream_start_ind_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_start_ind_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_start_ind_struct),
                                                    TD_UL);

    bta2dp_adp_msg->stream_handle = stream_handle;
    bta2dp_adp_msg->prefer_size = prefer_size;
    memcpy(&bta2dp_adp_msg->current_config, config, sizeof(bt_a2dp_audio_cap_struct));

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_START_IND,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpStreamSuspendCfm(U16 result, U8 stream_handle)
{
    bt_a2dp_stream_pause_cnf_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_pause_cnf_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_pause_cnf_struct),
                                                    TD_UL);

    bta2dp_adp_msg->result = result;
    bta2dp_adp_msg->stream_handle = stream_handle;

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_PAUSE_CNF,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpStreamSuspendInd(U8 stream_handle)
{
    bt_a2dp_stream_pause_ind_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_pause_ind_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_pause_ind_struct),
                                                    TD_UL);

    bta2dp_adp_msg->stream_handle = stream_handle;

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_PAUSE_IND,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpStreamCloseCfm(U16 result, U8 stream_handle)
{
    bt_a2dp_stream_close_cnf_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_close_cnf_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_close_cnf_struct),
                                                    TD_UL);

    bta2dp_adp_msg->result = result;
    bta2dp_adp_msg->stream_handle = stream_handle;

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_CLOSE_CNF,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpStreamCloseInd(U8 stream_handle)
{
    bt_a2dp_stream_close_ind_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_close_ind_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_close_ind_struct),
                                                    TD_UL);

    bta2dp_adp_msg->stream_handle = stream_handle;

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_CLOSE_IND,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpStreamAbortCfm(U8 stream_handle)
{
    bt_a2dp_stream_abort_cnf_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_abort_cnf_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_abort_cnf_struct),
                                                    TD_UL);

    bta2dp_adp_msg->stream_handle = stream_handle;

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_ABORT_CNF,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpStreamAbortInd(U8 stream_handle)
{
    bt_a2dp_stream_abort_ind_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_abort_ind_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_abort_ind_struct),
                                                    TD_UL);

    bta2dp_adp_msg->stream_handle = stream_handle;

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_ABORT_IND,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPAdpStreamQoSInd(U8 stream_handle, U8 qos)
{
    bt_a2dp_stream_qos_ind_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_qos_ind_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_qos_ind_struct),
                                                    TD_UL);

    bta2dp_adp_msg->stream_handle = stream_handle;
    bta2dp_adp_msg->qos = qos;

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_QOS_IND,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

void BTA2DPADPStreamDataSendReq(U8 stream_handle, U8 *data, U16 len, U32 sample_count)
{

    A2DP_SendStreamData(stream_handle, data, len, sample_count);
#if 0
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 *buffer;
    BT_A2DP_RESULT_ENUM result;

    Report(("[A2DP] BTA2DPADPStreamDataSendReq len:%d sample_count:%d", len, sample_count));

    //allocate stream buffer size (preferred_payload_size + 12)
    result = A2DP_GetStreamBuffer(stream_handle, len, &buffer);
    if (result != BT_A2DP_RESULT_OK)
    {
        /* This could happen only if there is no more memory available */
        return;
    }

    /* Get payload */
    memcpy(buffer, data, len);
    Report(("[A2DP] StreamSendDataReq"));
    result = A2DP_StreamSendDataReq(stream_handle, buffer, len, sample_count);
    Report(("[A2DP] A2DP_StreamSendDataReq result:%d", result));
    if (result != BT_A2DP_RESULT_OK)
    {
        Report(("[A2DP] StreamSendDataReq result = %d", result));
    }
#endif
}

void BTA2DPAdpStreamDataOut(U8 stream_handle)
{
    bt_a2dp_stream_data_out_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_data_out_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_data_out_struct),
                                                    TD_UL);

    bta2dp_adp_msg->stream_handle = stream_handle;

    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_DATA_OUT,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}


void BTA2DPAdpStreamDataConfigChange(bt_a2dp_audio_cap_struct *audio_cap)
{
    bt_a2dp_stream_data_config_change_struct *bta2dp_adp_msg;
    bta2dp_adp_msg =
        (bt_a2dp_stream_data_config_change_struct*) construct_local_para(
                                                    sizeof(bt_a2dp_stream_data_config_change_struct),
                                                    TD_UL);

    memcpy(&(bta2dp_adp_msg->audio_cap), audio_cap, sizeof(bt_a2dp_audio_cap_struct));
    a2dpa_sendMsg(
        MSG_ID_BT_A2DP_STREAM_DATA_CONFIG_CHANGE,
        MOD_EXTERNAL,
        BT_APP_SAP,
        (local_para_struct*) bta2dp_adp_msg,
        NULL);

}

#endif /* __BT_A2DP_PROFILE__ */
