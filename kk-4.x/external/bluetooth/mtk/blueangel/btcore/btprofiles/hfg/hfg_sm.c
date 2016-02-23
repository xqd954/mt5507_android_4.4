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

/****************************************************************************
 *
 * File:
 *     $Workfile:hfg_sm.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains the connection state machine for the 
 *     hands-free component of the Hands-free SDK.
 *             
 * Created:     February 11, 2005
 *
 * Copyright 2000-2005 Extended Systems, Inc.

 * Portions copyright 2005 iAnywhere Solutions, Inc.

 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,  
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
*****************************************************************************/
/*****************************************************************************
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * 08 08 2013 liang.cheng
 * [ALPS00931922] [MR2][BT]Source code check in
 * Easy Migration default profile check in.
 *
 * 08 08 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 11 07 2012 huirong.liao
 * [ALPS00371139] [Huawei_BT_Xi'an_IOT][PLANTRONICS PLT_BB903(+)]Vocie go from phone, but icon show bt
 * .
 *
 * 05 18 2012 huirong.liao
 * [ALPS00286509] [MT6628][E1_SQC][Bird2.0_18][BT]Bird75 connect with i-Tech, reboot the phone, after reboot, the phone happen red screen (once)
 * .
 *
 * 05 17 2012 huirong.liao
 * [ALPS00102390] <elephant ES8100 >
 * .
 *
 * 10 05 2010 sh.lai
 * [ALPS00127207] [MTK BT]The audio cannot transfer to BT headset when disconnect/connect BT headset.
 * Fix CR ALPS00127207 : [MTK BT]The audio cannot transfer to BT headset when disconnect/connect BT headset.
 * [Cause] After respone of AT+CHLD? is sent the profile state is transfferred to connected and the SCO is allowed. But AP think the HFP is connected when the response of AT+CHLD? is sent to profile and request SCO immediatly. HFP will reject the SCO connection due to the state is still conencting.
 * [Solution] Change state into connected after the response of AT+CHLD? is sent to stack so that profile can accept the SCO connection request.
 *
 * Jun 16 2009 mtk80029
 * [MAUI_01869480] [BT HFP] revise code related to audio connection request
 * 
 *
 * May 26 2009 mtk80029
 * [MAUI_01836032] [BT HFP] add $Log$ to HFP/HSP profile source file
 *
 * 11 07 2012 huirong.liao
 * [ALPS00371139] [Huawei_BT_Xi'an_IOT][PLANTRONICS PLT_BB903(+)]Vocie go from phone, but icon show bt
 * .
 *  *
 ****************************************************************************/
#include "stdio.h"
#include "sys/hfgalloc.h"
#include "btalloc.h"
#include "hfg.h"
#include "rfcomm_adp.h"
#include "conmgr.h"

#define __BT_FILE__ BT_FILE_HFG_SM

#ifdef __BT_HFG_PROFILE__
#if defined( BTMTK_ON_WISESDK ) || defined( BTMTK_ON_WIN32 ) || defined(BTMTK_ON_LINUX)
U32 bt_hfg_features =( 
                       //HFG_FEATURE_VOICE_RECOGNITION    | 
                       HFG_FEATURE_ECHO_NOISE           |
                       #ifdef __BT_INBAND_RING__
                       HFG_FEATURE_RING_TONE            | 
                       #endif
                       #ifdef __HF_V15__
                       HFG_FEATURE_ENHANCED_CALL_STATUS | 
                       HFG_FEATURE_EXTENDED_ERRORS      |
                       #endif
                       HFG_FEATURE_REJECT               | 
                       HFG_FEATURE_THREE_WAY_CALLS);

U32 bt_hfg_get_supported_feature(void)
{
    return bt_hfg_features;
}

void bt_hfg_mark_supported_feature(U32 feature_flag)
{
    bt_hfg_features |= feature_flag;
}

void bt_hfg_unmark_supported_feature(U32 feature_flag)
{
    bt_hfg_features &= ~feature_flag;
}

BOOL bt_hfg_check_supported_feature(U32 feature_flag)
{

    if ( bt_hfg_get_supported_feature() & feature_flag )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif /* defined( BTMTK_ON_WISESDK ) || defined( BTMTK_ON_WIN32 ) */
kal_bool hfga_is_profile_support_esco(void)
{
    return TRUE;
}
#endif

/*---------------------------------------------------------------------------
 *            HfgPacketHldr()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handle packet sending
 *
 * Return:    void
 */
#if defined(USE_EXT_AT_PARSER)
void HfgPacketHldr(HfgChannel *Channel, U8 Event, AtCommands *Parms)
{
    BtStatus    status;
    AtResults  *results;
    BOOL        bAutoOK = FALSE;
 
    Report(("[HFG][API] HfgPacketHldr (define USE_EXT_AT_PARSER): Event=%d.", Event));  

    /* Preinitialize the OK response */
    Channel->cmgrHandler.errCode = BEC_NO_ERROR;
    Channel->atResults.type = AT_OK;
    results = &Channel->atResults;

    switch (Event) 
    {
    case EVENT_AT_DATA:
        Report(("[HFG][INFO] HfgPacketHldr: EVENT_AT_DATA case, Parms->type=%d.", Parms->type));
        switch (Parms->type) 
        {
        case AT_CHECK_ALIVE:
        break;
        case AT_SUPPORTED_FEATURES:
            /* Report the hands-free device features */
            Channel->hfFeatures = Parms->p.hf.features.bitmap;
            Channel->flags |= CHANNEL_FLAG_FEATURES_SENT;
            Channel->cmgrHandler.errCode = BEC_NO_ERROR;
            break;

        case (AT_INDICATORS | AT_READ):
            if (!(Channel->flags & CHANNEL_FLAG_FEATURES_SENT)) 
            {
                DBG_PRINT_FLOW(("[HFG][FLOW] AT+BRSF is not received"));
                /* FLOW : AT+BRSF is not received */
                if(HfgGetSubstate(Channel) == HFG_SUBSTATE_OPEN)
                {
                    if (!(Channel->flags & CHANNEL_FLAG_OUTGOING)) 
                    {
                        status = HfgStartServiceQuery(Channel, BSQM_FIRST);
                        if (status == BT_STATUS_PENDING) 
                        {
                            HfgChangeSubstate(Channel, HFG_SUBSTATE_OPEN_W4SDP);
                        }
                        else if(status == BT_STATUS_SUCCESS)
                        {
                            HfgReportHfFeatures(Channel);
                        }
                        else
                        {
                            /* Can't start the query */
                            HfgDisconnecting(Channel);
                        }
                    } 
                }
                else
                {
                    Report(("[HFG][INFO] HfgPacketHldr: Unexpected case in (AT_INDICATORS | AT_READ)."));
                }
            }
            break;

        case AT_EVENT_REPORTING:
            if (Parms->p.hf.report.mode == 3) {
                Channel->indEnabled = Parms->p.hf.report.ind;
            }
            #ifdef __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_THREE_WAY_CALLS))
            {
                if ( !(Channel->hfFeatures & HFG_HANDSFREE_FEATURE_CALL_WAITING) &&
                    (Channel->flags & CHANNEL_FLAG_NEGOTIATE)) 
                {
                    Channel->flags |= CHANNEL_FLAG_NEG_DONE;
                }
            }
            else
            #endif
            {
                if (Channel->flags & CHANNEL_FLAG_NEGOTIATE) 
                {
                    Channel->flags |= CHANNEL_FLAG_NEG_DONE;
                }
            }
            break;

        case (AT_CALL_HOLD | AT_TEST):
            #if __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_THREE_WAY_CALLS))
            {
                if (Channel->flags & CHANNEL_FLAG_NEGOTIATE) 
                {
                    Channel->flags |= CHANNEL_FLAG_NEG_DONE;
                }
            }
            #endif
            break;

        default:
            break;
        }
        break;

    case EVENT_RF_PACKET_SENT:
        Report(("[HFG][INFO] HfgPacketHldr: EVENT_RF_PACKET_SENT case."));
        /* Results sent internally */
        HfgCheckNegotiateDone(Channel);
        return;
    default:
        return;
    }
    // Pass AT CMD to Ext adp
    HfgAtData atData;
    atData.dataLen = Channel->atRxLen;
    atData.rawData = Channel->atRxBuffer;
    HfgAppCallback(Channel, HFG_EVENT_AT_COMMAND_DATA, 
                   BT_STATUS_SUCCESS, (U32)&atData);
}
#else
void HfgPacketHldr(HfgChannel *Channel, U8 Event, AtCommands *Parms)
{
    BtStatus    status;
    AtResults  *results;
    BOOL        bAutoOK = FALSE;
    int         i = 0;

    Report(("[HFG][API] HfgPacketHldr (not define USE_EXT_AT_PARSER): Event=%d.", Event));    

    /* Preinitialize the OK response */
    Channel->cmgrHandler.errCode = BEC_NO_ERROR;
    Channel->atResults.type = AT_OK;
    results = &Channel->atResults;

    switch (Event) 
    {
    case EVENT_AT_DATA:
        Report(("[HFG][INFO] HfgPacketHldr: EVENT_AT_DATA case, Parms->type=%d.", Parms->type));
        switch (Parms->type) 
        {
        case AT_CHECK_ALIVE:
            break;
        //brad - service level connection establishment start
        case AT_SUPPORTED_FEATURES:
            /* Respond with the supported features */
            bAutoOK = TRUE;
            Channel->atResults.type = AT_SUPPORTED_FEATURES;
        #ifdef __BT_HFG_PROFILE__
            Channel->atResults.p.hf.features.bitmap = bt_hfg_get_supported_feature();  
        #else
            Channel->atResults.p.hf.features.bitmap = 
                #ifdef __BT_INBAND_RING__
                HFG_FEATURE_RING_TONE            | 
                #endif
                #ifdef __HF_V15__
                HFG_FEATURE_ENHANCED_CALL_STATUS | 
                HFG_FEATURE_ENHANCED_CALL_CTRL   | 
                HFG_FEATURE_EXTENDED_ERRORS      |
                #endif
                HFG_FEATURE_REJECT               | 
                HFG_FEATURE_THREE_WAY_CALLS;
        #endif
            /* Report the hands-free device features */
            Channel->hfFeatures = Parms->p.hf.features.bitmap;
            Channel->flags |= CHANNEL_FLAG_FEATURES_SENT;
            Channel->cmgrHandler.errCode = BEC_NO_ERROR;
            if (bt_hfg_check_supported_feature(HFG_FEATURE_CODEC_NEGOTIATION) &&
                (Channel->hfFeatures & HFG_HANDSFREE_FEATURE_CODEC_NEGOTIATION))
            {
                Channel->bCodecNegSupported = TRUE;
            }
            else
            {
                Channel->bCodecNegSupported = FALSE;
            }
            HfgAppCallback(Channel, HFG_EVENT_HANDSFREE_FEATURES, 
                           BT_STATUS_SUCCESS, (U32)Channel->hfFeatures);
            break;

        case (AT_INDICATORS | AT_TEST):
            HfgSetupIndicatorTestRsp(Channel);
            bAutoOK = TRUE;
            break;

        case (AT_INDICATORS | AT_READ):
            if (!(Channel->flags & CHANNEL_FLAG_FEATURES_SENT)) 
            {
                DBG_PRINT_FLOW(("[HFG][FLOW] AT+BRSF is not received"));
                /* FLOW : AT+BRSF is not received */
                if(HfgGetSubstate(Channel) == HFG_SUBSTATE_OPEN)
                {
                    if (!(Channel->flags & CHANNEL_FLAG_OUTGOING)) 
                    {
                        status = HfgStartServiceQuery(Channel, BSQM_FIRST);
                        if (status == BT_STATUS_PENDING) 
                        {
                            HfgChangeSubstate(Channel, HFG_SUBSTATE_OPEN_W4SDP);
                        }
                        else if(status == BT_STATUS_SUCCESS)
                        {
                            HfgReportHfFeatures(Channel);
                        }
                        else
                        {
                            /* Can't start the query */
                            HfgDisconnecting(Channel);
                        }
                    } 
                    else 
                    {
                        Channel->flags |= CHANNEL_FLAG_FEATURES_SENT;
                        Channel->cmgrHandler.errCode = BEC_NO_ERROR;
                        HfgAppCallback(Channel, HFG_EVENT_HANDSFREE_FEATURES, 
                                       BT_STATUS_SUCCESS, (U32)Channel->hfFeatures);
                    }
                }
                else
                {
                    Report(("[HFG][INFO] HfgPacketHldr: Unexpected case in (AT_INDICATORS | AT_READ)."));
                }
            }

            #ifdef HFG_SELF_HANDLE_READ_INDICATOR
            HfgSetupIndicatorReadRsp(Channel);
            break;
            #else
            HfgAppCallback(Channel, HFG_EVENT_READ_INDICATORS, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;
            #endif

        case AT_EVENT_REPORTING:

            if (Parms->p.hf.report.mode == 3) {
                Channel->indEnabled = Parms->p.hf.report.ind;
            }

            #ifdef __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_THREE_WAY_CALLS))
            {
                if ( !(Channel->hfFeatures & HFG_HANDSFREE_FEATURE_CALL_WAITING) &&
                    (Channel->flags & CHANNEL_FLAG_NEGOTIATE)) 
                {
                    Channel->flags |= CHANNEL_FLAG_NEG_DONE;
                }
            }
            else
            #endif
            {
                if (Channel->flags & CHANNEL_FLAG_NEGOTIATE) 
                {
                    Channel->flags |= CHANNEL_FLAG_NEG_DONE;
                }
            }
            break;

        case (AT_CALL_HOLD | AT_TEST):
            HfgSetupCallHoldReadRsp(Channel);
            if ( Channel->atResults.type == (AT_CALL_HOLD | AT_TEST))
                bAutoOK = TRUE;
            #if __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_THREE_WAY_CALLS))
            {
                if (Channel->flags & CHANNEL_FLAG_NEGOTIATE) 
                {
                    Channel->flags |= CHANNEL_FLAG_NEG_DONE;
                }
            }
            #endif
            break;
            //brad - service level connection establishment end

        case AT_ANSWER:
            HfgAppCallback(Channel, HFG_EVENT_ANSWER_CALL, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;

        case AT_DIAL_NUMBER:
            HfgAppCallback(Channel, HFG_EVENT_DIAL_NUMBER, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hf.dial.number);
            return;

        case AT_DIAL_MEMORY:
            HfgAppCallback(Channel, HFG_EVENT_MEMORY_DIAL, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hf.dial.number);
            return;

        case AT_REDIAL:
            HfgAppCallback(Channel, HFG_EVENT_REDIAL, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;

        case AT_CALL_HOLD:
            HfgAppCallback(Channel, HFG_EVENT_CALL_HOLD, 
                           BT_STATUS_SUCCESS, (U32)&Parms->p.hf.hold);
            return;
            
        case AT_RESPONSE_AND_HOLD | AT_READ:
            HfgSetupErrorRsp(Channel);           
            break;

        case AT_RESPONSE_AND_HOLD:        
            bAutoOK = TRUE;
            Channel->atResults.type = AT_RESPONSE_AND_HOLD;
            Channel->atResults.p.hf.btrh.setting = Parms->p.hf.btrh.setting;
            break;           

        case AT_HANG_UP:
            HfgAppCallback(Channel, HFG_EVENT_HANGUP, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;

        case AT_GENERATE_DTMF_TONE | AT_TEST:
            break;
            
        case AT_GENERATE_DTMF_TONE:
            HfgAppCallback(Channel, HFG_EVENT_GENERATE_DTMF, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hf.dtmf.tone);
            break;

        case AT_VOICE_TAG:
        #if __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_VOICE_TAG))
            {
                HfgAppCallback(Channel, HFG_EVENT_GET_LAST_VOICE_TAG, 
                               BT_STATUS_SUCCESS, (U32)0);
                return;
            }
            else
        #endif
            {
                HfgSetupErrorRsp(Channel); 
            }
            break;

        case AT_VOICE_RECOGNITION:
        #if __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_VOICE_RECOGNITION))
            {
                Channel->voiceRec = Parms->p.hf.vrec.enabled;
                HfgAppCallback(Channel, HFG_EVENT_ENABLE_VOICE_RECOGNITION, 
                               BT_STATUS_SUCCESS, (U32)Parms->p.hf.vrec.enabled);
                return;
            }
            else
        #endif
            {
                HfgSetupErrorRsp(Channel); 
            }                
            break;

        case AT_ECHO_C_AND_NOISE_R:
        #if __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_ECHO_NOISE))
            {
                Channel->nrecDisable = TRUE;
                HfgAppCallback(Channel, HFG_EVENT_DISABLE_NREC, 
                               BT_STATUS_SUCCESS, (U32)0);
                break;
            }
            else
        #endif
            {
                HfgSetupErrorRsp(Channel); 
            }
            break;

        case AT_LIST_CURRENT_CALLS:
            HfgAppCallback(Channel, HFG_EVENT_LIST_CURRENT_CALLS, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;

        case AT_CALL_ID:
            Channel->callId = Parms->p.hf.callId.enabled;
            break;

        case AT_CALL_WAIT_NOTIFY:
            Channel->callWaiting = Parms->p.hf.wait.notify;
            break;
            
        case AT_SUBSCRIBER_NUM:
            HfgAppCallback(Channel, HFG_EVENT_QUERY_SUBSCRIBER_NUMBER, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;

        case AT_NETWORK_OPERATOR:
            if ((Parms->p.hf.networkOper.mode == 3) &&
                (Parms->p.hf.networkOper.format == 0)) 
            {
            } 
            else
            {
                HfgSetupErrorRsp(Channel); 
            }
            break;
            
        case (AT_NETWORK_OPERATOR | AT_READ):
            HfgAppCallback(Channel, HFG_EVENT_QUERY_NETWORK_OPERATOR, 
                           BT_STATUS_SUCCESS, (U32)0);
            return;

        case AT_SET_ERROR_MODE:
        #if __BT_HFG_PROFILE__
            if (bt_hfg_check_supported_feature(HFG_FEATURE_EXTENDED_ERRORS))
            {
                Channel->extendedErrors = (BOOL)Parms->p.error.mode;            
            }
            else
        #endif
            {
                results->type = AT_ERROR;
            }
            break;

        case AT_MICROPHONE_GAIN:
            HfgAppCallback(Channel, HFG_EVENT_REPORT_MIC_VOLUME, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hs.mic.gain);
            break;

        case AT_SPEAKER_GAIN:
            HfgAppCallback(Channel, HFG_EVENT_REPORT_SPK_VOLUME, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hs.speaker.gain);
            break;

        case AT_KEYPAD_CONTROL:
            HfgAppCallback(Channel, HFG_EVENT_KEYPAD_CONTROL, 
                           BT_STATUS_SUCCESS, (U32)Parms->p.hs.keypad.button);
            return;

        case AT_INDICATOR_ACTIVATION:
            for (i = 0; (i < NUM_OF_HFG_IND) && (i < Parms->p.hf.indActivation.num); i++)
            {
                if (Parms->p.hf.indActivation.changed[i])
                {
                    Channel->IndActivation[i] = Parms->p.hf.indActivation.enabled[i];
                }
            }          
            break;

        case AT_CODEC_CONNECTION:
            if (Channel->bCodecNegSupported)
            {
                HfgSendOK(Channel);
                HfgCodecConnection(Channel);
                return;            
            } 
            else
            {
                HfgSetupErrorRsp(Channel); 
            }             
            break;

        case AT_CODEC_SELECTION:
            if (Channel->bCodecNegSupported && HfgIsInCodecSelecting(Channel) &&
                (Parms->p.hf.codecSelection.id >= HFG_CODEC_CVSD) &&
                (Parms->p.hf.codecSelection.id <= HFG_CODEC_MSBC))
            {
                if (!HfgCheckTargetCodec(Channel, Parms->p.hf.codecSelection.id)) 
                {                 
                    /* codec id in response is not the same as the one sent by AG */                
                    /* In spec, we are not required to retry codec connection procedure */                
                    HfgResetCodecState(Channel);
                    HfgSetupErrorRsp(Channel); 
                }
                else
                {
                    HfgSetTargetCodec(Channel, HFG_CODEC_NONE);
                    HfgSetSelectedCodec(Channel, Parms->p.hf.codecSelection.id);
                    HfgSetCodecState(Channel, HFG_CODEC_STATE_CODEC_DECIDED);
                    if (HfgCheckSelectedCodec(Channel, HFG_CODEC_CVSD))
                    {
                        status = CMGR_CreateAudioLink(&Channel->cmgrHandler);
                    }
                    else
                    {
                        status = CMGR_CreateAudioLink_Ex(&Channel->cmgrHandler, CMGR_AUDIO_CODEC_MSBC);
                    }
                    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CREATE_AUDIO_CHANNEL, 
                        status, Channel->codecSelected);               
                    if ((status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS))
                    {
                        Channel->linkFlags |= HFG_LINK_SCO_CONNECTING;
                        HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC_W4SCO);
                    }
                    else
                    {
                        kal_trace(TRACE_GROUP_5, BT_HFG_WARN_1, __BT_FILE__, __LINE__, status);
                    }                    
                }                      
            } 
            else
            {
                HfgSetupErrorRsp(Channel); 
            }             
            break;

        case AT_AVAILABLE_CODECS:
            if (Channel->bCodecNegSupported && Parms->p.hf.availCodec.num)
            {
                HfgClearAvailCodec(Channel);
                for (i = 0; i < Parms->p.hf.availCodec.num; i++)
                {
                    HfgSetAvailCodec(Channel, Parms->p.hf.availCodec.id[i]);
                }                                     
                if (HfgIsInCodecSelecting(Channel)) 
                {
                    /* If codec connection is in progress, restart code negotiation directly */               
                    if ((Channel->targetCodec != HFG_CODEC_NONE) && 
                        (HfgCheckAvailCodec(Channel, Channel->targetCodec) == FALSE)) 
                    {
                        /* Current selected codec becomes invalid on HF */  
                        HfgSendOK(Channel);
                        HfgCodecNegotiation(Channel);
                        return;
                    }
                }
                else 
                {
                    /* Else, we reset the current selected codec/state & 
                       wait for next codec connection procedure */
                    HfgResetCodecState(Channel);
                }                        
            } 
            else
            {
                HfgSetupErrorRsp(Channel); 
            }            
            break;            
#if 0            
        case AT_SELECT_CHARACTER_SET:
        {
            #if 1
            HfgAppCallback(Channel, HFG_EVENT_SELECT_CHARSET, BT_STATUS_SUCCESS, (U32)Parms->p.charSet.type);
            return;
            #else
            HfgCHarsetType charsetType;
            status = HfgDecodeCharset(Channel, Parms->p.charSet.type, &charsetType);
              if ( status == BT_STATUS_SUCCESS )
                  {
                      Channel->charset = charsetType;
                        HfgAppCallback(Channel, HFG_EVENT_CHARSET_SELECTED, 
                                    BT_STATUS_SUCCESS, (U32)charsetType);
                }
             else
            {
                Channel->atResults.type = AT_ERROR;
                results->p.error.type = 0;
            }
             break;
            #endif
        }
        case (AT_SELECT_CHARACTER_SET|AT_TEST):
            #if 1
            HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_CHARSET, BT_STATUS_SUCCESS, 0);
            return;
            #else
            if ( BT_STATUS_SUCCESS == HfgSetupCharsetTestRsp(Channel) )
            {
                Channel->atResults.type = AT_SELECT_CHARACTER_SET|AT_TEST;
                bAutoOK = TRUE;
            }
            else
            {
                Channel->atResults.type = AT_ERROR;
                results->p.error.type = 0;
            }
            break;
            #endif
        case (AT_SELECT_CHARACTER_SET|AT_READ):
            #if 1
            HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_CHARSET, BT_STATUS_SUCCESS, 0);
            return;
            #else
            results->p.charset = HfgGetCharsetDesc(Channel, Channel->charset);
            if(results->p.charset)
            {
                Channel->atResults.type = AT_SELECT_CHARACTER_SET|AT_READ;
                bAutoOK = TRUE;
            }
            else
            {
                Channel->atResults.type = AT_ERROR;
                results->p.error.type = 0;
            }
            break;
            #endif

        case AT_MANUFACTURE_ID:
            #if 1
            HfgAppCallback(Channel, HFG_EVENT_QUERY_MANUFACTURE_ID, BT_STATUS_SUCCESS, 0);
            return;
            #else
            results->p.manufactureID = HfgGetManufactureID(Channel);
            Channel->atResults.type = AT_MANUFACTURE_ID;
            bAutoOK = TRUE;
            break;
            #endif
            
        case AT_MODEL_ID:
            #if 1
            HfgAppCallback(Channel, HFG_EVENT_QUERY_MODEL_ID, BT_STATUS_SUCCESS, 0);
            return;
            #else       
            results->p.modelID = HfgGetModelID(Channel);
            Channel->atResults.type = AT_MODEL_ID;
            bAutoOK = TRUE;
            break;
            #endif
                /* L4 will not take care VGS and VGM commands, so shall reply OK by HFP */
                //return;
            /***********************************************************
            *   Phonebook indication
            ***********************************************************/
            case (AT_SELECT_PHONEBOOK_STORAGE|AT_TEST):
                HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_PHONEBOOK, 
                                        BT_STATUS_SUCCESS, 0);
                return;
                
            case AT_SELECT_PHONEBOOK_STORAGE:
                HfgAppCallback(Channel, HFG_EVENT_SELECT_PHONEBOOK, 
                               BT_STATUS_SUCCESS, (U32)Parms->p.pb.storage.select);
                return;
                
            case (AT_SELECT_PHONEBOOK_STORAGE|AT_READ):
                HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_PHONEBOOK, 
                               BT_STATUS_SUCCESS, 0);
                return;

            case (AT_READ_PHONEBOOK_ENTRY|AT_TEST):
                HfgAppCallback(Channel, HFG_EVENT_QUERY_READ_PBENTRY_INFO, 
                   BT_STATUS_SUCCESS, 0);
                return;

            case AT_READ_PHONEBOOK_ENTRY:
                HfgAppCallback(Channel, HFG_EVENT_READ_PBENTRY, 
                   BT_STATUS_SUCCESS, (U32)&Parms->p.pb.read);
                return;

            case (AT_FIND_PHONEBOOK_ENTRY|AT_TEST):
                HfgAppCallback(Channel, HFG_EVENT_QUERY_FIND_PBENTRY_INFO, 
                   BT_STATUS_SUCCESS, 0);
                return;

            case AT_FIND_PHONEBOOK_ENTRY:
                HfgAppCallback(Channel, HFG_EVENT_FIND_PBENTRY, 
                   BT_STATUS_SUCCESS, (U32)&Parms->p.pb.find);
                return;

            case (AT_WRITE_PHONEBOOK_ENTRY|AT_TEST):
                HfgAppCallback(Channel, HFG_EVENT_QUERY_WRITE_PBENTRY_INFO, 
                   BT_STATUS_SUCCESS, 0);
                return;

            case AT_WRITE_PHONEBOOK_ENTRY:
                HfgAppCallback(Channel, HFG_EVENT_WRITE_PBENTRY, 
                   BT_STATUS_SUCCESS, (U32)&Parms->p.pb.write);
                return;
                            
            /***********************************************************
            *   SMS indication
            ***********************************************************/
            case (AT_SELECT_SMS_SERVICE|AT_TEST):   /* AT+CSMS=? */
                    HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_SMS_SERVICE, 
                       BT_STATUS_SUCCESS, 0);
                    return;
            case (AT_SELECT_SMS_SERVICE|AT_READ):   /* AT+CSMS? */
                    HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_SMS_SERVICE, 
                       BT_STATUS_SUCCESS, 0);
                return;
            case AT_SELECT_SMS_SERVICE:             /* AT+CSMS=<service> */
                    HfgAppCallback(Channel, HFG_EVENT_SELECT_SMS_SERVICE, 
                       BT_STATUS_SUCCESS, (U32)Parms->p.sms.service);
                return;
            case (AT_PREFERRED_SMS_STORAGE|AT_TEST):    /* AT+CPMS=? */
                    HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_PREF_MSG_STORAGE, 
                       BT_STATUS_SUCCESS, 0);
                return;
            case (AT_PREFERRED_SMS_STORAGE|AT_READ):    /* AT+CPMS? */
                    HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_PREF_MSG_STORAGE,
                       BT_STATUS_SUCCESS, 0);
                return;
            case AT_PREFERRED_SMS_STORAGE:              /* AT+CPMS=<mem1>[,<mem2>[,<mem3>]] */
                    HfgAppCallback(Channel, HFG_EVENT_SELECT_PREF_MSG_STORAGE,
                       BT_STATUS_SUCCESS, (U32)&Parms->p.sms.preferred);
                return;
            case (AT_SMS_MESSAGE_FORMAT|AT_TEST):       /* AT+CMGF=? */
                HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_MSG_FORMAT, 
                   BT_STATUS_SUCCESS, 0);
                return;
            case (AT_SMS_MESSAGE_FORMAT|AT_READ):       /* AT+CMGF? */
                HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_MSG_FORMAT,
                   BT_STATUS_SUCCESS, 0);
                return;
            case AT_SMS_MESSAGE_FORMAT:         /* AT+CMGF=<mode> */
                HfgAppCallback(Channel, HFG_EVENT_SELECT_MSG_FORMAT,
                   BT_STATUS_SUCCESS, (U32)Parms->p.sms.format);
                return;
            case (AT_SMS_SERVICE_CENTER|AT_READ):   /* AT+CSCA? */
                HfgAppCallback(Channel, HFG_EVENT_QUERY_SERVICE_CENTRE,
                   BT_STATUS_SUCCESS, 0);
                return;
            case AT_SMS_SERVICE_CENTER:             /* AT+CSCA=<sca>[,<tosca>] */
                HfgAppCallback(Channel, HFG_EVENT_SET_SERVICE_CENTRE,
                   BT_STATUS_SUCCESS, (U32)&Parms->p.sms.SCA);
                return;
            case (AT_SET_TEXT_MODE_PARMS|AT_READ):  /* AT+CSMP? */
                HfgAppCallback(Channel, HFG_EVENT_QUERY_TEXT_MODE_PARAMS,
                   BT_STATUS_SUCCESS, 0);
                return;
            case AT_SET_TEXT_MODE_PARMS:            /* AT+CSMP=[<fo>[,<vp>[,<pid>[,<dcs>]]]] */
                HfgAppCallback(Channel, HFG_EVENT_SET_TEXT_MODE_PARAMS,
                   BT_STATUS_SUCCESS, (U32)&Parms->p.sms.textModeParam);
                return;
            case (AT_SMS_SHOW_TEXT_MODE|AT_TEST):   /* AT+CSDH=? */
                HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_SHOW_PARAMS,
                   BT_STATUS_SUCCESS, 0);
                return;
            case (AT_SMS_SHOW_TEXT_MODE|AT_READ):   /* AT+CSDH? */
                HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_SHOW_PARAMS,
                   BT_STATUS_SUCCESS, 0);
                return;
            case AT_SMS_SHOW_TEXT_MODE:         /* AT+CSDH=[<show>] */
                HfgAppCallback(Channel, HFG_EVENT_SET_SHOW_PARAMS,
                   BT_STATUS_SUCCESS, (U32)Parms->p.sms.show);
                return;
            case (AT_NEW_MESSAGE_INDICATION|AT_TEST):   /* AT+CNMI=? */
                HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_NEW_MSG_INDICATION,
                   BT_STATUS_SUCCESS, 0);
                return;
            case (AT_NEW_MESSAGE_INDICATION|AT_READ):   /* AT+CNMI? */
                HfgAppCallback(Channel, HFG_EVENT_QUERY_SELECTED_NEW_MSG_INDICATION,
                   BT_STATUS_SUCCESS, 0);
                return;
            case AT_NEW_MESSAGE_INDICATION:         /* AT+CNMI=[<mode>[,<mt>[,<bm>[,<ds>[,<bfr>]]]]] */
                HfgAppCallback(Channel, HFG_EVENT_SET_NEW_MSG_INDICATION,
                   BT_STATUS_SUCCESS, (U32)&Parms->p.sms.newMsgIndSetting);
                return;
            case (AT_LIST_MESSAGES|AT_TEST):            /* AT+CMGL=? */
                HfgAppCallback(Channel, HFG_EVENT_QUERY_SUPPORTED_LIST_STATUS,
                   BT_STATUS_SUCCESS, 0);
                return;
            case AT_LIST_MESSAGES:                  /* AT+CMGL[=<stat>] */
                HfgAppCallback(Channel, HFG_EVENT_LIST_MSG,
                   BT_STATUS_SUCCESS, (U32)Parms->p.sms.status);
                return;
            case AT_READ_MESSAGE:                   /* AT+CMGR=<index> */
                HfgAppCallback(Channel, HFG_EVENT_READ_MSG,
                   BT_STATUS_SUCCESS, (U32)Parms->p.sms.readMsgIndex);      
                return;
            case AT_SEND_MESSAGE:                   /* TEXT MODE : AT+CMGS=<da>[,<toda>]<CR>text is entered<ctrl-Z/ESC> */
                                                    /* PDU MODE : AT+CMGS=<length><CR>PDU is given<ctrl-Z/ESC> */
                HfgAppCallback(Channel, HFG_EVENT_SEND_MSG,
                   BT_STATUS_SUCCESS, (U32)&Parms->p.sms.sendMsg);
                return;
            case AT_SEND_STORED_MESSAGE:            /* AT+CMSS=<index>[,<da>[,<toda>]] */
                HfgAppCallback(Channel, HFG_EVENT_SEND_STORED_MSG,
                   BT_STATUS_SUCCESS, (U32)&Parms->p.sms.storedMsg);
                return;
            case AT_STORE_MESSAGE:                  /* TEXT MODE : AT+CMGW=<oa/da>[,<toda/toda>[,<stat>]]<CR>text is entered<ctrl-Z/ESC> */
                                                    /* PDU MODE : AT+CMGW=<length>[,<stat>]<CR>PDU is given<ctrl-Z/ESC> */
                HfgAppCallback(Channel, HFG_EVENT_WRITE_MSG,
                   BT_STATUS_SUCCESS, (U32)&Parms->p.sms.writeMsg);
                return;
            case AT_DELETE_MESSAGE:
                HfgAppCallback(Channel, HFG_EVENT_DELETE_MSG,
                   BT_STATUS_SUCCESS, (U32)Parms->p.sms.delMsgIndex);
                return;
#endif        
            default:
            {
                HfgAtData at_data;
                at_data.dataLen = Channel->atRxLen;
                at_data.rawData = (U8*)&Channel->atRxBuffer[0];
                HfgAppCallback(Channel, HFG_EVENT_AT_COMMAND_DATA, 
                               BT_STATUS_SUCCESS, (U32)&at_data);            
                return;  
            }           
        }
        break;

    case EVENT_RF_PACKET_SENT:
        Report(("[HFG][INFO] HfgPacketHldr: EVENT_RF_PACKET_SENT case."));
        /* Results sent internally */
        if (Channel->flags & CHANNEL_FLAG_NEG_DONE) 
        {
            /* Negotiation is complete */
            Channel->flags &= ~(CHANNEL_FLAG_NEG_DONE | 
                            CHANNEL_FLAG_NEGOTIATE);

            /* Service channel is up, tell the app */
            /* FLOW : Negotiation done */
            if(HfgGetSubstate(Channel) == HFG_SUBSTATE_OPEN)
            {
                DBG_PRINT_FLOW(("[HFG][FLOW] Negotiation done"));
                HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC);
            }
            /* WCNCR00000458: VW polo only supports HFP 0.99, if VW initiates HFP connection, 
               AG will trigger SDP query and the HFG state will go wrong. Merge HFG code from
               wise_sp */            
            else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_OPEN_W4SDP)
            {
                /* To prevent the SDP query is not completed */
                DBG_PRINT_FLOW(("[HFG][FLOW] Negotiation done in state HFG_SUBSTATE_OPEN_W4SDP"));
                HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC);
            }
            else
            {
                Report(("[HFG][INFO] HfgPacketHldr: Unexpected state (%d) when SLC is up.", 
                    HfgGetSubstate(Channel)));
                HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC);
            }
        }
        return;

    default:
        Report(("[HFG][INFO] HfgPacketHldr: Unsupported case."));
        break;
    }

    results = HfgAllocResponse();
    if(!results)
    {
        status = BT_STATUS_NO_RESOURCES;
        HfgDisconnecting(Channel);
    }
    else
    {
        OS_MemCopy((U8*)results, (const U8*)&Channel->atResults, sizeof(HfgResponse));
        status = AtSendResults(Channel, results, bAutoOK);
    }

    if (status != BT_STATUS_PENDING) 
    {
        if(results)
            HfgFreeResponse(results);
    }
    else
    {
        results->flag |= AT_RESULT_FLAG_SELF_HANDLED;
    }
}
#endif /* USE_EXT_AT_PARSER */
/*---------------------------------------------------------------------------
 *            HfRfCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  RFCOMM callback for the HF state machine.
 *
 * Return:    void
 */
void HfgRfCallback(RfChannel *rfChannel, RfCallbackParms *Parms)
{
    HfgChannel  *Channel;
    U8          event = 0;
    U16         offset = 0;
    BtStatus status = BT_STATUS_SUCCESS;

    Channel = HfgFindRegisteredChannel(rfChannel);
    Assert(Channel != 0);

    kal_trace(TRACE_GROUP_5, BT_HFG_CBK_RF_CALLBACK, Channel, Parms->event, Parms->status);
    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, 
        HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);
    
    switch (Parms->event) 
    {
    case RFEVENT_OPEN_IND:
        DBG_PRINT_EVENT(("[HFG][EVENT] RFCOMM Event : RFEVENT_OPEN_IND."));
        if(Channel)
        {
            OS_MemCopy((U8*)Channel->bdAddr.addr, (U8*)Parms->ptrs.remDev->bdAddr.addr, sizeof(BD_ADDR));
            if(HfgGetSubstate(Channel) == HFG_SUBSTATE_CLOSED)
            {
                HfgChangeSubstate(Channel, HFG_SUBSTATE_CONN_IN_W4RSP);
            }
            else
            {
                Report(("[HFG][INFO] HfgRfCallback: Only in Closed, the server channel is registered."));
                /* Only in C1, the server channel is registered, so there shall no open ind received in other 
                substates other than C1. */
                if( BT_STATUS_PENDING == RF_RejectChannel(rfChannel) )
                    Channel->linkFlags |= HFG_LINK_DISCONNECTING;       
            }
        }
        else
        {
            Report(("[HFG][INFO] HfgRfCallback: RFEVENT_OPEN_IND: HFP shall never get into this case."));
            /* HFP shall never get into this case */
            DBG_PRINT_ERROR(("[HFG][ERR] Unexpected substate=0x%X - (%d,%d)", 
                HfgGetSubstate(Channel), __BT_FILE__, __LINE__));
            (void)RF_RejectChannel(rfChannel);
        }
        break;

    case RFEVENT_OPEN:
        DBG_PRINT_EVENT(("[HFG][EVENT] RFCOMM Event : RFEVENT_OPEN."));
        /* Register a SCO handler */
        if (Channel) 
        {
            OS_MemCopy((U8*)Channel->bdAddr.addr, (U8*)Parms->ptrs.remDev->bdAddr.addr, sizeof(BD_ADDR));
            //CMGR_SetDeviceAsMaster(&(Channel->cmgrHandler));
            HfgEnableSniffTimerMode(&(Channel->cmgrHandler));
            Channel->linkFlags |= HFG_LINK_HANDSFREE;
            Channel->linkFlags &= ~(HFG_LINK_CONNECTING|HFG_LINK_DISCONNECTING);
            Report(("[HFG][INFO] HfgRfCallback: RFCOMM HF connection established."));

            switch(HfgGetSubstate(Channel))
            {
            case HFG_SUBSTATE_CONN_OUT_W4RFCOMM:
            case HFG_SUBSTATE_CONN_IN_W4RFCOMM:
                DBG_PRINT_FLOW(("[HFG][FLOW] Is headset profile : %s", (Channel->type == AG_TYPE_HS)?"Yes":"No"));
                if(Channel->type == AG_TYPE_HS)
                {
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_OPEN);
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC);
                }
                else
                {
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_OPEN);
                }
                break;
            case HFG_SUBSTATE_DISC:
                HfgDisconnecting(Channel);
                break;
            default:
                Report(("[HFG][INFO] HfgRfCallback: RFEVENT_OPEN: HFG shall not get into this case."));
                /* HFG shall not get into this case. */
                DBG_PRINT_ERROR(("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", 
                    HfgGetSubstate(Channel), __BT_FILE__, __LINE__));
                break;
            }
        }
        break;
    case RFEVENT_DATA_IND:
        DBG_PRINT_EVENT(("[HFG][EVENT] RFCOMM Event : RFEVENT_DATA_IND."));
        if (Channel) 
        {
            Report(("[HFG][INFO] HfgRfCallback: RFCOMM HF data received."));
            RF_AdvanceCredit(rfChannel, 1);
            event = EVENT_AT_DATA;
            while (offset < Parms->dataLen) 
            {
                DBG_PRINT_AT(("[HFG][AT] Remain data > 0. Remain len=%d", Parms->dataLen));
                if (HfgParseRfcommData(Channel, Parms, &offset, &Channel->atParms) == BT_STATUS_SUCCESS) 
                {
                    HfgPacketHldr(Channel, event, &Channel->atParms);
                    Channel->atParms.type = 0;
                    Channel->atRxLen = 0;
                }
            }
        }
        break;

    case RFEVENT_PACKET_HANDLED:
        DBG_PRINT_EVENT(("[HFG][EVENT] RFCOMM Event : RFEVENT_PACKET_HANDLED, Status=%d", Parms->status));
        if (Channel) 
        {
            AtResults *result;
            AtCommand cmd;
            status = Parms->status;
            if(status != BT_STATUS_SUCCESS)
            {
                /* Only when RFCOMM channel is closed, it does not return success */
                DBG_PRINT_AT(("[HFG][AT] RFEVENT_PACKET_HANDLED : Fail"));
            }
            else if(IsListEmpty(&Channel->rspQueue))
            {
                kal_trace(TRACE_GROUP_5, BT_HFG_INFO_RESPONSE_QUEUE_EMPTY);
                HfgDisconnecting(Channel);
                status = BT_STATUS_FAILED;
            }
            else if(Channel->bytesToSend > 0)
            {
                kal_trace( TRACE_GROUP_5, BT_HFG_INFO_AT_SEND_REMAIN_DATA, Channel->bytesToSend);
                /* Send more data */
                Parms->ptrs.packet->data += Parms->ptrs.packet->dataLen;
                Parms->ptrs.packet->dataLen = Channel->bytesToSend;
                status = AtSendRfPacket(Channel, Parms->ptrs.packet);
            }
            else
            {
                result = (AtResults*)(Channel->rspQueue.Flink);
                if(result->flag & AT_RESULT_FLAG_AUTO_OK)
                {
                    DBG_PRINT_AT(("[HFG][AT] Auto OK response : Yes"));
                    cmd = result->type;
                    result->type = AT_OK;
                    status = sendResults(Channel, result);
                    result->type = cmd;
                    result->flag &= ~AT_RESULT_FLAG_AUTO_OK;
                }
                else
                {
                    DBG_PRINT_AT(("[HFG][AT] Auto OK response : No"));
                }
            }

            /* If status is pending, it means the result is not completed */
            /* Traverse the rspQueue to send next result. */
            while(status != BT_STATUS_PENDING && !IsListEmpty(&Channel->rspQueue))
            {
                event = EVENT_RF_PACKET_SENT;
                HfgPacketHldr(Channel, event, NULL);
                HfgReportPacketCompleted(Channel, status);
                if(IsListEmpty(&Channel->rspQueue))
                {
                    DBG_PRINT_AT(("[HFG][AT] rspQueue empty : Yes"));
                    break;
                }
                else
                {
                    DBG_PRINT_AT(("[HFG][AT] rspQueue empty : No"));
                    result = (AtResults*)(Channel->rspQueue.Flink);
                    status = sendResults(Channel, result);
                }
            }
        } 
        else 
        {
            Report(("[HFG][INFO] HfgRfCallback: RFEVENT_PACKET_HANDLED: No HFG channel."));
            return;
        }
        break;

    case RFEVENT_CLOSED:
        DBG_PRINT_EVENT(("[HFG][EVENT] RFCOMM Event : RFEVENT_CLOSED"));
        if (Channel) 
        {
            HfgDisableSniffTimerMode(&(Channel->cmgrHandler));
            Channel->linkFlags &= ~HFG_LINK_HANDSFREE;
            Channel->linkFlags &= ~(HFG_LINK_CONNECTING|HFG_LINK_DISCONNECTING);
            switch(HfgGetSubstate(Channel))
            {
            case HFG_SUBSTATE_CONN_OUT_W4RFCOMM:
            case HFG_SUBSTATE_CONN_IN_W4RSP:
            case HFG_SUBSTATE_CONN_IN_W4RFCOMM:
                Channel->reason = HFG_DISCREASON_RFCOMM;
                HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED);
                break;                
            case HFG_SUBSTATE_OPEN:
                Channel->reason = HFG_DISCREASON_NORMAL;
                HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED);
                break;
            case HFG_SUBSTATE_OPEN_W4SDP:
            case HFG_SUBSTATE_SLC:
            case HFG_SUBSTATE_SLC_W4SCO:
            case HFG_SUBSTATE_DISC:
                HfgDisconnecting(Channel);
                break;
            default:
                DBG_PRINT_ERROR(("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", 
                    HfgGetSubstate(Channel), __BT_FILE__, __LINE__));
                break;
            }
        }
        break;

    default:
        DBG_PRINT_EVENT(("[HFG][EVENT] RFCOMM ignore other events : %d", Parms->event));
        break;
    }
}

/*---------------------------------------------------------------------------
 *            HfgServiceConnectionCallback()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Called by device manager with link state events.
 *
 * Return:    (See header file)
 *
 */
void HfgServiceConnectionCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status) 
{
    HfgChannel *Channel = ContainingRecord(Handler, HfgChannel, cmgrHandler);
    BtStatus return_code;

    kal_trace(TRACE_GROUP_5, BT_HFG_CBK_CMGR_CALLBACK, Channel, Event, Status);
    kal_trace(TRACE_GROUP_5, BT_HFG_INFO_CHANNEL_STATE, HfgGetSubstate(Channel), Channel->linkFlags, Channel->flags, Channel->queryFlags);

    switch (Event) 
    {
    case CMEVENT_DATA_LINK_CON_CNF:
        DBG_PRINT_EVENT(("[HFG][EVENT] CMGR Event : CMEVENT_DATA_LINK_CON_CNF, Status=%d.", Status));
        if (Status == BT_STATUS_SUCCESS) 
            Channel->linkFlags |= HFG_LINK_ACL;
        else
            Channel->linkFlags &= ~HFG_LINK_ACL;
        
        Channel->linkFlags &= ~HFG_LINK_ACL_CONNECTING;
        if(HfgGetSubstate(Channel) == HFG_SUBSTATE_CONN_OUT_W4ACL)
        {
            if (Status == BT_STATUS_SUCCESS) 
            {
                /* FLOW : Start SDP query */
                if ((return_code = HfgStartServiceQuery(Channel, BSQM_FIRST)) == BT_STATUS_PENDING)
                {
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_CONN_OUT_W4SDP);
                }
                else if(return_code == BT_STATUS_SUCCESS)
                {
                    return_code = HfgOpenRfChannel(Channel);
                }
                else
                {
                    Channel->reason = HFG_DISCREASON_ACL_CONN;
                    HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED);
                }
            }
            else
            {
                Channel->reason = HFG_DISCREASON_ACL_CONN;            
                /* This event shall not be received in other substates other than C1 */
                HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED);
            }
        }
        else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_DISC)
        {
            HfgDisconnecting(Channel);
        }
        else
        {
            DBG_PRINT_ERROR(("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", 
                HfgGetSubstate(Channel), __BT_FILE__, __LINE__));
            Channel->reason = HFG_DISCREASON_ACL_CONN;            
            HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED);
        }
        break;

    case CMEVENT_DATA_LINK_DIS:
        /* We shall never run into this case */
        if(!OS_MemCmp(Channel->bdAddr.addr, 6, Handler->remDev->bdAddr.addr, 6)){
            DBG_PRINT_ERROR(("[HFG][EVENT] Unexpected CMEVENT_DATA_LINK_DIS of address : %x:%x:%x:%x:%x:%x", 
            Handler->remDev->bdAddr.addr[0],
            Handler->remDev->bdAddr.addr[1],
            Handler->remDev->bdAddr.addr[2],
            Handler->remDev->bdAddr.addr[3],
            Handler->remDev->bdAddr.addr[4],
            Handler->remDev->bdAddr.addr[5]));
        }
        DBG_PRINT_EVENT(("[HFG][EVENT] CMGR Event : CMEVENT_DATA_LINK_DIS, Status=%d.", Status));
        DBG_PRINT_ERROR(("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", 
            HfgGetSubstate(Channel), __BT_FILE__, __LINE__));
        Channel->reason = HFG_DISCREASON_ACL_DISC;
        HfgChangeSubstate(Channel, HFG_SUBSTATE_CLOSED);
        break;

    case CMEVENT_AUDIO_LINK_CON:
        DBG_PRINT_EVENT(("[HFG][EVENT] CMGR Event : CMEVENT_AUDIO_LINK_CON, Status=%d.", Status));
        Channel->linkFlags &= ~HFG_LINK_SCO_CONNECTING;
        if(HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC)
        {
            /* Remote connect SCO link */
            /* It could happen in SLC but no action needed */
            /* Only report event when remote request connection success */
            if(Status == BT_STATUS_SUCCESS)
            {
                HfgConfigureSpeechChannel(Channel->codecSelected);            
                HfgAppCallback(Channel, HFG_EVENT_AUDIO_CONNECTED, 
                               Status, (U32)&Channel->bdAddr.addr);
            }
        }
        else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC_W4SCO)
        {
            HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC);        
            if (Status == BT_STATUS_SUCCESS)
            {
                HfgResetCodecRetryCount(Channel);           
                HfgConfigureSpeechChannel(Channel->codecSelected);
                HfgAppCallback(Channel, HFG_EVENT_AUDIO_CONNECTED, 
                               BT_STATUS_SUCCESS, (U32)&Channel->bdAddr.addr);
            }
            else
            {
                /* Restart codec-selection if SCO failed */
                if (Channel->bCodecNegSupported) 
                {
                    if (HfgIsInCodecDecided(Channel) && 
                        !HfgCheckSelectedCodec(Channel, HFG_CODEC_CVSD)) 
                    {
                        /* if the previous non-CVSD is failed, try CVSD again */
                        HfgCodecNegotiation(Channel);
                        break;
                    } 
                    else 
                    { 
                        /* if codec is not selected or CVSD is selected, we should stop trying */
                        HfgResetCodecState(Channel);
                    }
                }               
                HfgAppCallback(Channel, HFG_EVENT_AUDIO_DISCONNECTED, 
                               BT_STATUS_SUCCESS, (U32)&Channel->bdAddr.addr);                    
            }
        }
        else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_DISC)
        {
            HfgDisconnecting(Channel);
        }
        else
        {
            DBG_PRINT_ERROR(("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", 
                HfgGetSubstate(Channel), __BT_FILE__, __LINE__));
        }
        break;

    case CMEVENT_AUDIO_LINK_DIS:
        DBG_PRINT_EVENT(("[HFG][EVENT] CMGR Event : CMEVENT_AUDIO_LINK_DIS, Status=%d.", Status));
        Channel->linkFlags &= ~HFG_LINK_SCO_DISCONNECTING;       
        HfgAppCallback(Channel, HFG_EVENT_AUDIO_DISCONNECTED, 
                       Status, (U32)&Channel->bdAddr.addr);        
        if(HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC)
        {
            /* Remote disconnect SCO link */
            /* It could happen in SLC but no action needed */
        }
        else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_SLC_W4SCO)
        {
            HfgChangeSubstate(Channel, HFG_SUBSTATE_SLC);       
        }
        else if(HfgGetSubstate(Channel) == HFG_SUBSTATE_DISC)
        {
            HfgDisconnecting(Channel);
        }
        else
        {
            DBG_PRINT_ERROR(("[HFG][ERR] Unexpected substate==0x%X - (%d,%d)", 
                HfgGetSubstate(Channel), __BT_FILE__, __LINE__));
        }
        break;

    #if BT_SCO_HCI_DATA == XA_ENABLED
    case CMEVENT_AUDIO_DATA:
        DBG_PRINT_EVENT(("[HFG][EVENT] CMGR Event : CMEVENT_AUDIO_DATA, Status=%d.", Status));
        HfgAppCallback(Channel, HFG_EVENT_AUDIO_DATA, Status, (U32)&Handler->audioData);
        break;

    case CMEVENT_AUDIO_DATA_SENT:
        DBG_PRINT_EVENT(("[HFG][EVENT] CMGR Event : CMEVENT_AUDIO_DATA_SENT, Status=%d.", Status));
        HfgAppCallback(Channel, HFG_EVENT_AUDIO_DATA_SENT, Status, (U32)&Handler->audioPacket);
        break;
    #endif
    }
}

