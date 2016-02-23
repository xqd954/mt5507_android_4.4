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
 * Bt_a2dp_hdl.c
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
 *   Yi Zeng
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
#include "stdio.h"
#include "string.h"
#include "stdarg.h"


#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bt_a2dp_api.h"
#include "bluetooth_a2dp_message.h"
#include "bt_a2dp_hdl.h"


#ifdef BTMTK_ON_LINUX
#include "bt_ext_debug.h"
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "cutils/sockets.h"

#define BT_STATUS_SUCCESS 0
#define BT_STATUS_FAILED  1

#define RECONFIG_ERROR 0
#define RECONFIG_SUCCESS 1
#define START_SUCCESS 2
#define RECONFIG_NO_CHANGE 3
#define START_FAILED 4

#endif


extern int g_srvcsockfd;
extern void BTCMD_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size);

extern void btmtk_a2dp_send_signal_connect_res(module_type src_mod_id, U8 *device_addr, U8 accept);

extern void btmtk_a2dp_send_appi_bt_deactivate_request(void);
extern void bt_prompt_trace(module_type mod_id, const char *fmt,...);

kal_bool btmtk_a2dp_app_is_last_connection(void);
U8 btmtk_a2dp_find_dev_index(U8 stream_handle);
BT_A2DP_APP_STATE btmtk_a2dp_global_state(void);

#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER
U8 a2dp_fm_reconfig_procedure();
#endif

#define MAX_REMOTE_AUDIO_CAP_NUMBER 4


#define BTMTK_A2DP_MAX_APPI_COMMAND (20)

#define BTMTK_A2DP_COMMAND_REQUEST      0x00
#define BTMTK_A2DP_COMMAND_INDICATION   0x01

typedef struct _BT_A2DP_APP_DEV
{
    U8 stream_handle;
    bt_addr_struct addr;
    U8 dev_name[MAX_DEV_NAME_LEN];
    U8 is_opened;
    kal_bool is_opened_by_peer_dev;
    U8 local_start_req_counter;
    BT_A2DP_APP_STATE state;
    U16 current_message;
    
    U8 not_support_reconfig;
    U8 disconnect_pending;
} BT_A2DP_APP_DEV;


typedef struct _BT_A2DP_APP_CTX
{
    BT_A2DP_APP_DEV a2dp_app_dev[MAX_A2DP_DEV_NUM];

    U16 payload_size;
    //U8 appi_command_queue[2][BTMTK_A2DP_MAX_APPI_COMMAND]; // one dimension is used for store stream handler id
    //U8 appi_command_ongoing;  
    //U8 appi_command_req_sent;
    //U8 appi_command_pause_immediately;
    //bt_sep_info_struct local_seid;
    
    // bt_a2dp_audio_cap_struct device_audio_cap;    
    // bt_a2dp_audio_cap_struct local_audio_cap;
    // bt_a2dp_audio_cap_struct configed_audio_cap;
    
    //U8 a2dp_operation;
    //U8 streaming_pause;
    bt_addr_struct current_dev_addr;
    U8 deactivate_penging;
	bool isSuspend;
	bt_suspend_callback hfpCallback;

#ifdef BTMTK_ON_LINUX
    //U8 bConnecting;
    //U8 req_fm_a2dp_data;
	U8 send_close;
	U8 send_start;
	U8 connect_ind; //for IOT headset, after initating connect procedure wish initiating start 
	U8 open_ind;
#endif

    /* The following is used for FM over BT */
    U8 fmOverBtMode;        //For FMOverBt Feature, set 1 on, 
    kal_bool fmOverBtPostponedStart;
    kal_bool fmStartContinue;
    kal_bool fmStopContinue;
    kal_bool fmSuspending;
    bt_a2dp_audio_cap_struct local_audio_cap;

}BT_A2DP_APP_CTX;


static kal_uint32 a2dp_wait_msgList[] = { 
        MSG_ID_BT_A2DP_APPI_BT_COMMAND,
        MSG_ID_BT_A2DP_APPI_BT_CONNECT_ACCEPT_RSP,
        MSG_ID_BT_A2DP_ACTIVATE_CNF,
        MSG_ID_BT_A2DP_DEACTIVATE_CNF,
        MSG_ID_BT_A2DP_STREAM_RECONFIG_CNF,
        MSG_ID_BT_A2DP_STREAM_RECONFIG_IND,
        MSG_ID_BT_A2DP_STREAM_OPEN_CNF,
        MSG_ID_BT_A2DP_STREAM_OPEN_IND,
        MSG_ID_BT_A2DP_STREAM_START_CNF,
        MSG_ID_BT_A2DP_STREAM_START_IND,
        MSG_ID_BT_A2DP_STREAM_PAUSE_CNF,
        MSG_ID_BT_A2DP_STREAM_PAUSE_IND,
        MSG_ID_BT_A2DP_STREAM_CLOSE_CNF,
        MSG_ID_BT_A2DP_STREAM_CLOSE_IND,
        MSG_ID_BT_A2DP_STREAM_ABORT_CNF,
        MSG_ID_BT_A2DP_STREAM_ABORT_IND,
        MSG_ID_BT_A2DP_STREAM_QOS_IND,
        MSG_ID_BT_MEDIA_A2DP_CODEC_OPEN_CNF,
        MSG_ID_BT_MEDIA_A2DP_CODEC_CLOSE_CNF,
        };

void btmtk_a2dp_handle_appi_bt_command(ilm_struct *message);
void btmtk_a2dp_handle_activate_cnf(ilm_struct *message);
void btmtk_a2dp_handle_deactivate_cnf(ilm_struct *message);

A2dpStatus btmtk_a2dp_handle_stream_open_cnf(ilm_struct *message);
A2dpStatus btmtk_a2dp_handle_stream_open_ind(ilm_struct *message);
A2dpStatus btmtk_a2dp_handle_stream_start_cnf(ilm_struct *message);
A2dpStatus btmtk_a2dp_handle_stream_start_ind(ilm_struct *message);
A2dpStatus btmtk_a2dp_handle_stream_suspend_cnf(ilm_struct *message);
A2dpStatus btmtk_a2dp_handle_stream_suspend_ind(ilm_struct *message);
A2dpStatus btmtk_a2dp_handle_stream_close_cnf(ilm_struct *message);
A2dpStatus btmtk_a2dp_handle_stream_close_ind(ilm_struct *message);

void btmtk_a2dp_handle_reconfig_cnf(ilm_struct *message);
void btmtk_a2dp_handle_reconfig_ind(ilm_struct *message);
A2dpStatus btmtk_a2dp_handle_stream_abort_cnf(ilm_struct *message);
A2dpStatus btmtk_a2dp_handle_stream_abort_ind(ilm_struct *message);
void btmtk_a2dp_handle_stream_qos_ind(ilm_struct *message);
//static void a2dp_handle_stream_data_out(ilm_struct *message);
void btmtk_a2dp_handle_authorize_res(ilm_struct *message);

/* Local function */
void btmtk_a2dp_app_default_setting_initialized(void);
kal_bool btmtk_a2dp_app_verify_connection_id(U16 connection_id);
kal_bool btmtk_a2dp_app_verify_stream_handle(U8 stream_handle);
void btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE state, U8 stream_handler);
void btmtk_a2dp_app_convert_addr_from_Stack(U8 *addr_target, bt_addr_struct *addr_src);

void btmtk_a2dp_handle_operation(U8 index);
void btmtk_a2dp_state_error_handling(void);

void btmtk_a2dp_pause_immediately(void);

U8 btmtk_a2dp_appi_compare_capability(bt_a2dp_audio_cap_struct *aud_cfg1, bt_a2dp_audio_cap_struct *aud_cfg2);
U8 btmtk_a2dp_appi_ngeo_config(bt_a2dp_audio_cap_struct *cfg1, bt_a2dp_audio_cap_struct *cfg2, bt_a2dp_audio_cap_struct *result);


BT_A2DP_APP_STATE btmtk_a2dp_app_get_stream_state(U8 stream_handle);
void btmtk_a2dp_send_msg_2_audio_module(msg_type msg_id, ilm_struct * ilm, U16 size);
extern int socket_make_sockaddr_un(const char *name, int namespaceId, 
        struct sockaddr_un *p_addr, socklen_t *alen);

void a2dp_off_fmoverbtmode(void);
//void btmtk_a2dp_fm_media_start_failed(void);
int btmtk_a2dp_fm_send_start_req(void);


BT_A2DP_APP_CTX bt_a2dp_app;
static U8 g_bActivate = 0;


#define HfpCallback(a) \
	if (bt_a2dp_app.hfpCallback) bt_a2dp_app.hfpCallback(a)


static void OS_Report(const char *format, ...)
{
#if 0
    char buffer[200];   /* Output buffer */
    va_list args;

    va_start(args, format);
    _vsnprintf(buffer, 200, format, args);
    va_end(args);
//    kal("\n%s\n",buffer);
    bt_prompt_trace(MOD_BT, buffer);
#endif
}

void btmtk_a2dp_app_setsuspendflag(bool flag)
{
    bt_a2dp_app.isSuspend = flag;
}

void btmtk_a2dp_app_registerHfgCallback(bt_suspend_callback callback)
{
    bt_a2dp_app.hfpCallback = callback;
}

void btmtk_a2dp_app_deactive(void)
{
    int i = 0;
    int counter = 0;

    for (i = 0; i < MAX_A2DP_DEV_NUM; i++) 
    {
        if (bt_a2dp_app.a2dp_app_dev[i].state > BT_A2DP_APP_STATE_IDLE && 
            bt_a2dp_app.a2dp_app_dev[i].state != BT_A2DP_APP_STATE_UNKNOWN) 
        {
            if (bt_a2dp_app.a2dp_app_dev[i].current_message == 0)
                btmtk_a2dp_send_stream_close_req(MOD_BT, bt_a2dp_app.a2dp_app_dev[i].stream_handle);
                
            bt_a2dp_app.deactivate_penging = TRUE;
            counter++;
        }
    }

    if (counter == 0)
        btmtk_a2dp_send_deactivate_req(MOD_BT);
}

void btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE state, U8 stream_handler)
{
    U8 dev_index = btmtk_a2dp_find_dev_index(stream_handler);
    bt_ext_log("[A2DP] try to set stream state dev index (%d)", dev_index);
    if (dev_index == UNKNOWN_DEV_INDEX)
        return;
    
    bt_a2dp_app.a2dp_app_dev[dev_index].state = state;

    bt_ext_log("[A2DP] set stream (%d) state to (%d)", stream_handler, state);
}

kal_bool btmtk_a2dp_app_check_current_process_msg(U8 stream_handle, U16 msg_id)
{
    U8 dev_index = btmtk_a2dp_find_dev_index(stream_handle);

    bt_ext_log("[A2DP] app check current process msg, dev: %d, msg id:%d\n", dev_index, msg_id);
    
    if (dev_index == UNKNOWN_DEV_INDEX)
        return KAL_FALSE;

    if (bt_a2dp_app.a2dp_app_dev[dev_index].current_message == 0)
    {
        bt_a2dp_app.a2dp_app_dev[dev_index].current_message = msg_id;
        return KAL_TRUE;
    }

    if (MSG_ID_BT_A2DP_STREAM_CLOSE_REQ == msg_id && 
        MSG_ID_BT_A2DP_STREAM_CLOSE_REQ != bt_a2dp_app.a2dp_app_dev[dev_index].current_message) 
    {
        bt_a2dp_app.a2dp_app_dev[dev_index].disconnect_pending = 1;
        
        bt_ext_log("[A2DP] app check current message, postpond to send close req, current msg: %d", bt_a2dp_app.a2dp_app_dev[dev_index].current_message);
        /* maybe we should return false, but close command should have a high priority, so add the following statements and return true */
        bt_a2dp_app.a2dp_app_dev[dev_index].current_message = msg_id;
        return KAL_TRUE;
    }
    
    bt_ext_log("[A2DP] app check current message mismatch current: %d, last: %d", msg_id, bt_a2dp_app.a2dp_app_dev[dev_index].current_message);
    return KAL_FALSE;
}

A2dpStatus btmtk_a2dp_handle_message(ilm_struct *message)
{
    A2dpStatus iResult = A2DP_STATUS_NO_CHANGE;

    bt_ext_log("[A2DP] btmtk_a2dp_handle_message msg id:%d", message->msg_id);

    switch(message->msg_id)
    {
        case MSG_ID_BT_A2DP_APPI_BT_COMMAND:
            
            btmtk_a2dp_handle_appi_bt_command(message);
            break;
            
        case MSG_ID_BT_A2DP_APPI_BT_CONNECT_ACCEPT_RSP:
            
            btmtk_a2dp_handle_authorize_res(message);
            break;
            
        case MSG_ID_BT_A2DP_ACTIVATE_CNF:
            
            btmtk_a2dp_handle_activate_cnf(message);
            break;
            
        case MSG_ID_BT_A2DP_DEACTIVATE_CNF:
            
            btmtk_a2dp_handle_deactivate_cnf(message);
            break;
        
        case MSG_ID_BT_A2DP_STREAM_RECONFIG_CNF:
            
            btmtk_a2dp_handle_reconfig_cnf(message);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_RECONFIG_IND:
            
            btmtk_a2dp_handle_reconfig_ind(message);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_OPEN_CNF:   
            
            iResult = btmtk_a2dp_handle_stream_open_cnf(message);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_OPEN_IND:
            
            iResult = btmtk_a2dp_handle_stream_open_ind(message);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_START_CNF:

            iResult = btmtk_a2dp_handle_stream_start_cnf(message);

            break;
        case MSG_ID_BT_A2DP_STREAM_START_IND:

            iResult = btmtk_a2dp_handle_stream_start_ind(message);

            break;
        case MSG_ID_BT_A2DP_STREAM_PAUSE_CNF:

            iResult = btmtk_a2dp_handle_stream_suspend_cnf(message);

            break;
        case MSG_ID_BT_A2DP_STREAM_PAUSE_IND:

            iResult = btmtk_a2dp_handle_stream_suspend_ind(message);

            break;
        case MSG_ID_BT_A2DP_STREAM_CLOSE_CNF:

            iResult = btmtk_a2dp_handle_stream_close_cnf(message);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_CLOSE_IND:
            
            iResult = btmtk_a2dp_handle_stream_close_ind(message);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_ABORT_CNF:
            
            iResult = btmtk_a2dp_handle_stream_abort_cnf(message);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_ABORT_IND:
            
            iResult = btmtk_a2dp_handle_stream_abort_ind(message);
            break;
            
        case MSG_ID_BT_A2DP_STREAM_QOS_IND:
            
            btmtk_a2dp_handle_stream_qos_ind(message);
            break;
            
#if 0
        case MSG_ID_BT_A2DP_STREAM_DATA_OUT:
            a2dp_handle_stream_data_out(message);
            break;
#endif

        default:
            break;
    }

    return iResult;

}

void btmtk_a2dp_handle_appi_bt_command(ilm_struct *message)
{
    U8 addr[6];
    U8 config_sent = 0;
    U8 temp_fm_state = FALSE;
    U8 dev_index = 0;
    int i = 0;
    BT_A2DP_APP_STATE glb_state;

    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    bt_a2dp_appi_bt_command_struct *msg;
    
    msg = (bt_a2dp_appi_bt_command_struct *)message->ilm_data;

    bt_ext_log("[A2DP] btmtk_a2dp_handle_appi_bt_command cmd:%d", msg->command);

#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER

    /* For FMOverBt Feature, switch between FM and Media */
    temp_fm_state = bt_a2dp_app.fmOverBtMode;

	if((temp_fm_state == TRUE)) 
    {   
        if(msg->command == BT_A2DP_APPI_COMMAND_START)
        {
            // For Temp not admit media playing when FM is playing, until mediaplayback service call FMRadioService to send FM Power Down
            bt_ext_log("bt_a2dp_app.fmOverBtMode:true, received media start req");
			btmtk_a2dp_fm_media_start_failed();
            return;
		}
		if(msg->command == BT_A2DP_APPI_COMMAND_STOP)
        {
			return;
        }
    }

#endif    

    glb_state = btmtk_a2dp_global_state();

    switch(msg->command)
    {   
        case BT_A2DP_APPI_COMMAND_START:
            if (glb_state == BT_A2DP_APP_STATE_STREAMING)
            {
                bt_ext_log("[A2DP] process start command, global state is streaming.\n");
                
                if (bt_a2dp_app.local_audio_cap.codec_cap.sbc.max_bit_pool <= 0 || bt_a2dp_app.payload_size <= 0)
                    ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_FAILED;
                else
                    ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_SUCCESS;
                
                ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->prefer_size = bt_a2dp_app.payload_size;
                memcpy(&(((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->current_config), &bt_a2dp_app.local_audio_cap, sizeof (bt_a2dp_audio_cap_struct));	
                btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_START_CNF, ilmptr, sizeof(bt_a2dp_stream_data_start_cnf_struct));

                bt_ext_log("[A2DP] a2dp start start command localcap, cm: %d, %d, %d, %d, %d",
                                        bt_a2dp_app.local_audio_cap.codec_cap.sbc.channel_mode,
                                        bt_a2dp_app.local_audio_cap.codec_cap.sbc.subband_num,
                                        bt_a2dp_app.local_audio_cap.codec_cap.sbc.block_len,
                                        bt_a2dp_app.local_audio_cap.codec_cap.sbc.alloc_method,
                                        bt_a2dp_app.local_audio_cap.codec_cap.sbc.sample_rate,
                                        bt_a2dp_app.local_audio_cap.codec_cap.sbc.max_bit_pool);
                

                // TODO: send start command to rest device the state of which is opened.
                for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
                {
                    if (bt_a2dp_app.a2dp_app_dev[i].state == BT_A2DP_APP_STATE_OPENED &&
                        bt_a2dp_app.a2dp_app_dev[i].is_opened_by_peer_dev == KAL_FALSE)
                        btmtk_a2dp_send_stream_start_req(MOD_BT, bt_a2dp_app.a2dp_app_dev[i].stream_handle);
                }
            }
            else if(glb_state == BT_A2DP_APP_STATE_OPENED)
            {
                bt_ext_log("[A2DP] process start command, global state is opened. start req counter:%d\n", bt_a2dp_app.a2dp_app_dev[0].local_start_req_counter);

                U8 start_req_counter = 0;
                U8 delayCount = 10;

                if (bt_a2dp_app.isSuspend)
                {
                    bt_ext_log("bt_a2dp_app.isSuspend:true, received media start req");
                    btmtk_a2dp_fm_media_start_failed();
                    return;
                }
                
                for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
                {
                    bt_ext_log("[A2DP] i: %d, state: %d, is open by dev:%d", i, bt_a2dp_app.a2dp_app_dev[i].state, bt_a2dp_app.a2dp_app_dev[i].is_opened_by_peer_dev);
                    if (bt_a2dp_app.a2dp_app_dev[i].state == BT_A2DP_APP_STATE_OPENED &&
                        (bt_a2dp_app.a2dp_app_dev[i].is_opened_by_peer_dev == KAL_FALSE ||
                        bt_a2dp_app.a2dp_app_dev[i].local_start_req_counter++ > delayCount))
                    {
                        btmtk_a2dp_send_stream_start_req(MOD_BT, bt_a2dp_app.a2dp_app_dev[i].stream_handle);
                        start_req_counter++;
                    }
                }

                if (start_req_counter == 0)
                {
                    /* There are no device connected */
                    bt_ext_log("[A2DP] process start command, waiting for peer device to start a2dp.\n");
                    
                    ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_FAILED;
                    ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->prefer_size = bt_a2dp_app.payload_size;
                    btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_START_CNF, ilmptr, sizeof(bt_a2dp_stream_data_stop_cnf_struct));
                }
            }
            else
            {
                /* There are no device connected */
                bt_ext_log("[A2DP] process start command, global state is idle.\n");
                
                ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_FAILED;
                ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->prefer_size = bt_a2dp_app.payload_size;
                btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_START_CNF, ilmptr, sizeof(bt_a2dp_stream_data_stop_cnf_struct));
            }
            break;
            
        case BT_A2DP_APPI_COMMAND_STOP:
        case BT_A2DP_APPI_COMMAND_PAUSE:
            
            if (glb_state == BT_A2DP_APP_STATE_OPENED)
            {
                bt_ext_log("[A2DP] process stop command, global state is opened.\n");
                
                ((bt_a2dp_stream_data_stop_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_SUCCESS;
                btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_STOP_CNF, ilmptr, sizeof(bt_a2dp_stream_data_stop_cnf_struct));
            }
            else if(glb_state == BT_A2DP_APP_STATE_IDLE)
            {
                bt_ext_log("[A2DP] process stop command, global state is idle.\n");
                
                ((bt_a2dp_stream_data_stop_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_FAILED;
                btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_STOP_CNF, ilmptr, sizeof(bt_a2dp_stream_data_stop_cnf_struct));
            }
            else
            {
                bt_ext_log("[A2DP] process stop command, global state is streaming or unknown.\n");

                /* make sure that stop command can be responsed */
                U8 suspend_req_counter = 0;
                for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
                {
                    if (bt_a2dp_app.a2dp_app_dev[i].state == BT_A2DP_APP_STATE_STREAMING &&
                        bt_a2dp_app.a2dp_app_dev[i].current_message == 0) 
                    {
                        btmtk_a2dp_send_stream_pause_req(MOD_BT, bt_a2dp_app.a2dp_app_dev[i].stream_handle);
                        suspend_req_counter++;
                    }

                    if (suspend_req_counter == 0)
                    {
                        ((bt_a2dp_stream_data_stop_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_FAILED;
                        btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_STOP_CNF, ilmptr, sizeof(bt_a2dp_stream_data_stop_cnf_struct));
                    }
                }
            }
            break;
            
        case BT_A2DP_APPI_COMMAND_CLOSE:
            
            if (glb_state == BT_A2DP_APP_STATE_IDLE)
            {
                bt_ext_log("[A2DP] process stop command, global state is opened.\n");
                
                // ((bt_a2dp_stream_data_stop_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_SUCCESS;
                btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_DISC_IND, ilmptr, sizeof(bt_a2dp_stream_data_disc_ind_struct));
            }
            else if(glb_state >= BT_A2DP_APP_STATE_OPENED)
            {
                bt_ext_log("[A2DP] process stop command, global state is opened.\n");
                
                for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
                {
                    if (bt_a2dp_app.a2dp_app_dev[i].state == BT_A2DP_APP_STATE_STREAMING)
                        btmtk_a2dp_send_stream_close_req(MOD_BT, bt_a2dp_app.a2dp_app_dev[i].stream_handle);
                }
            }            
            break;
            
        default:

            bt_ext_log("[A2DP] received undefined command %d, ignore.\n", msg->command);
            break;
    }
#if 0
#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER
    if(msg->req_fm_a2dp_data)
    {
        //XH,For FMOverBt Feature, switch between FM and Media
        temp_fm_state = bt_a2dp_app.fmOverBtMode;
   
		if((temp_fm_state == TRUE)) 
        {   
            if(msg->command == BT_A2DP_APPI_COMMAND_START)
            {
                // For Temp not admit media playing when FM is playing, until mediaplayback service call FMRadioService to send FM Power Down
                bt_ext_log("bt_a2dp_app.fmOverBtMode:true, received start req");
				btmtk_a2dp_fm_media_start_failed();
	            return;
			}
			if(msg->command == BT_A2DP_APPI_COMMAND_STOP)
            {
				return;
            }
        }
        else
        {
			if(bt_a2dp_app.fmSuspending == TRUE)
            {
					if(msg->command == BT_A2DP_APPI_COMMAND_STOP)
                    {
						bt_a2dp_app.req_fm_a2dp_data = msg->req_fm_a2dp_data;
			        }
		    }
		}	
        
		if(bt_a2dp_app.send_start == 1)
        {
			bt_ext_log("start req have been sent");
			if(temp_fm_state == TRUE)
            {
			    btmtk_a2dp_fm_media_start_failed();
			    return;
		    }
  		}
		
		bt_ext_log("Receive BT_A2DP_APPI_COMMAND request come from a2dp_data");
        if((bt_a2dp_app.connect_ind == 1)&&(bt_a2dp_app.open_ind == 1) && (msg->command == BT_A2DP_APPI_COMMAND_START)){
			ilm_struct ilm;
			ilm_struct *ilmptr;
			ilmptr = &ilm;
       		bt_ext_log("Receive BT_A2DP_APPI_COMMAND_START, but wait peer send start req, return");
		
	        ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_FAILED;
	        ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->prefer_size = bt_a2dp_app.payload_size;
	        btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_START_CNF, ilmptr, sizeof(bt_a2dp_stream_data_start_cnf_struct));
			
			return;
        }
        if(msg->command == BT_A2DP_APPI_COMMAND_START && bt_a2dp_app.state == BT_A2DP_APP_STATE_STREAMING)
        {
            ilm_struct ilm;
            ilm_struct *ilmptr;
            ilmptr = &ilm;
    
            ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_SUCCESS;
            ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->prefer_size = bt_a2dp_app.payload_size;
            btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_START_CNF, ilmptr, sizeof(bt_a2dp_stream_data_start_cnf_struct));
            return;
        }
        
	    // XH	
        if(msg->command == BT_A2DP_APPI_COMMAND_START && bt_a2dp_app.state != BT_A2DP_APP_STATE_OPENED)
    	{
    		ilm_struct ilm;
    		ilm_struct *ilmptr;
    		ilmptr = &ilm;

    	    //if play music and then connect a2dp, command queue:discover/get capability/set config/ is on process, then start req also be sent, so if not return here, music will
    	    //be intermediated by following insert command:set config...make avdtp suspend and disconnect.
            bt_ext_log("Receive BT_A2DP_APPI_COMMAND_START, but state != BT_A2DP_APP_STATE_OPENED, return");
    		
            ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_FAILED;
            ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->prefer_size = bt_a2dp_app.payload_size;
            btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_START_CNF, ilmptr, sizeof(bt_a2dp_stream_data_start_cnf_struct));
    	    return;
    	}
        if(msg->command == BT_A2DP_APPI_COMMAND_STOP && bt_a2dp_app.state != BT_A2DP_APP_STATE_STREAMING)
        {
            ilm_struct ilm;
            ilm_struct *ilmptr;
            ilmptr = &ilm;
    
            ((bt_a2dp_stream_data_stop_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_SUCCESS;
            btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_STOP_CNF, ilmptr, sizeof(bt_a2dp_stream_data_stop_cnf_struct));
            return;
        }
		if((bt_a2dp_app.send_close == 1)&&(msg->command == BT_A2DP_APPI_COMMAND_STOP)){
			ilm_struct ilm;
            ilm_struct *ilmptr;
            ilmptr = &ilm;
    
            ((bt_a2dp_stream_data_stop_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_SUCCESS;
            btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_STOP_CNF, ilmptr, sizeof(bt_a2dp_stream_data_stop_cnf_struct));
            return;
		}
        bt_a2dp_app.req_fm_a2dp_data = msg->req_fm_a2dp_data;
    }
#endif
#endif

}

void btmtk_a2dp_handle_activate_cnf(ilm_struct *message)
{
    bt_a2dp_activate_cnf_struct *msg;

    memset(&bt_a2dp_app, 0, sizeof(bt_a2dp_app));

    msg = (bt_a2dp_activate_cnf_struct *)message->ilm_data;

    bt_ext_log("[A2DP] A2DP Activate CNF result:%d\n", msg->result);
    btmtk_a2dp_app_default_setting_initialized(); 

    if(msg->result == BT_A2DP_RESULT_OK)
    {
        g_bActivate = 1;
        bt_ext_log("[A2DP] g_bActivate:%d\n", g_bActivate);
    }

}

void btmtk_a2dp_handle_deactivate_cnf(ilm_struct *message)
{
    bt_a2dp_deactivate_cnf_struct *msg;

    msg = (bt_a2dp_deactivate_cnf_struct *)message->ilm_data;

    memset((U8 *)&bt_a2dp_app, 0, sizeof(bt_a2dp_app));
    bt_ext_log("[A2DP] A2DP Deactivate CNF result:%d\n", msg->result);

    if(msg->result == BT_A2DP_RESULT_OK)
    {
       g_bActivate = 0;
       bt_ext_log("[A2DP] g_bActivate:%d\n", g_bActivate);
    }

}

U8 btmtk_a2dp_find_stream_handle_by_addr(bt_addr_struct *addr)
{
    int i = 0;

    for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
    {
        bt_ext_log("[a2dp] find stream hdl by addr:%x:%x:%x, state:%d", 
            bt_a2dp_app.a2dp_app_dev[i].addr.lap,
            bt_a2dp_app.a2dp_app_dev[i].addr.nap,
            bt_a2dp_app.a2dp_app_dev[i].addr.uap,
            bt_a2dp_app.a2dp_app_dev[i].state);
        
        if (bt_a2dp_app.a2dp_app_dev[i].state >= BT_A2DP_APP_STATE_OPENED)
        {
            if (addr->lap == bt_a2dp_app.a2dp_app_dev[i].addr.lap &&
                addr->nap == bt_a2dp_app.a2dp_app_dev[i].addr.nap &&
                addr->uap == bt_a2dp_app.a2dp_app_dev[i].addr.uap) {
                bt_ext_log("[A2DP] found a stream hdl %d, return%d", i, bt_a2dp_app.a2dp_app_dev[i].stream_handle);
                return bt_a2dp_app.a2dp_app_dev[i].stream_handle;
            }
        }
    }
    
    return UNKNOWN_STREAM_HANDLE;
}

U8 btmtk_a2dp_find_dev_index(U8 stream_handle)
{
    U8 index = 0;
    
    bt_ext_log("[A2DP] find device index, stream handle: %d\n", stream_handle);
    
    for (index = 0; index < MAX_A2DP_DEV_NUM; index++)
    {
        if (bt_a2dp_app.a2dp_app_dev[index].stream_handle == stream_handle)
            return index;
    }
        
    return UNKNOWN_DEV_INDEX;
}

U8 btmtk_a2dp_get_dev_index(void)
{
    U8 index = 0;
    
    bt_ext_log("[A2DP] get device index\n");
    
    for (index = 0; index < MAX_A2DP_DEV_NUM; index++)
    {
        if (bt_a2dp_app.a2dp_app_dev[index].state == BT_A2DP_APP_STATE_IDLE)
            return index;
    }
        
    return UNKNOWN_DEV_INDEX;
}

BT_A2DP_APP_STATE btmtk_a2dp_global_state(void)
{
    U8 index = 0;
    U8 dev_index = 0;
    
    BT_A2DP_APP_STATE max_state = BT_A2DP_APP_STATE_IDLE;

    for (index = 0; index < MAX_A2DP_DEV_NUM; index++)
    {        
        if ((bt_a2dp_app.a2dp_app_dev[index].state != BT_A2DP_APP_STATE_UNKNOWN) && 
            (bt_a2dp_app.a2dp_app_dev[index].state > max_state))
        {
            max_state = bt_a2dp_app.a2dp_app_dev[index].state;
        }
    }

    bt_ext_log("[A2DP] global state: %d\n", max_state);

    return max_state;
}


void btmtk_a2dp_handle_reconfig_cnf(ilm_struct *message)
{
    U8 dev_index = 0;
    bt_a2dp_stream_reconfig_cnf_struct *msg;

     
    msg = (bt_a2dp_stream_reconfig_cnf_struct *)message->ilm_data;

    dev_index = btmtk_a2dp_find_dev_index(msg->stream_handle);
    
    if((UNKNOWN_DEV_INDEX == dev_index))
    {
        bt_ext_log("[A2DP] Stream reconfig error: have not find the device context. hdl: %d\n", msg->stream_handle);
    }
    else
    {
    	bt_ext_log("[A2DP] Reconfig cnf: state:%d, result:%d, cur msg:%d", bt_a2dp_app.a2dp_app_dev[dev_index].state, 
    	    msg->result, bt_a2dp_app.a2dp_app_dev[dev_index].current_message);
        
        if (bt_a2dp_app.a2dp_app_dev[dev_index].state == BT_A2DP_APP_STATE_OPENED &&
            msg->result == BT_A2DP_RESULT_OK &&
            bt_a2dp_app.a2dp_app_dev[dev_index].current_message == MSG_ID_BT_A2DP_STREAM_RECONFIG_REQ)
        {
            bt_a2dp_app.a2dp_app_dev[dev_index].current_message = 0;

#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER

            bt_ext_log("[A2DP] FM Reconfig cnf: fmStartContinue:%d, fmOverBtMode:%d", bt_a2dp_app.fmStartContinue, bt_a2dp_app.fmOverBtMode);
            if (bt_a2dp_app.fmStartContinue == KAL_TRUE)
            {
                btmtk_a2dp_fm_controller_start_req(MOD_BT, 0);
                bt_a2dp_app.fmStartContinue = KAL_FALSE;
            }
            else
            {
                /* move fm over bt mode here */
                if (bt_a2dp_app.fmOverBtMode == TRUE)
                {
                    bt_a2dp_app.fmOverBtMode = FALSE;
                }
            }

#endif
        }
        else
        {
            btmtk_a2dp_send_stream_close_req(MOD_BT, msg->stream_handle);
        }

        /* send close request if disconnect_pending is true */
        if (TRUE == bt_a2dp_app.a2dp_app_dev[dev_index].disconnect_pending)
        {
            btmtk_a2dp_send_stream_close_req(MOD_BT, msg->stream_handle);
            bt_a2dp_app.a2dp_app_dev[dev_index].disconnect_pending = FALSE;
        }
    }
    // TODO: reconfig is useless now, reserver for future usage.
}

void btmtk_a2dp_handle_reconfig_ind(ilm_struct *message)
{
    U8 dev_index = 0;
    bt_a2dp_stream_reconfig_ind_struct *msg;

     
    msg = (bt_a2dp_stream_reconfig_ind_struct *)message->ilm_data;

    dev_index = btmtk_a2dp_find_dev_index(msg->stream_handle);
    
    if((UNKNOWN_DEV_INDEX == dev_index))
    {
        bt_ext_log("[A2DP] Stream reconfig error: have not find the device context. hdl: %d\n", msg->stream_handle);
    }
    else
    {
        if (bt_a2dp_app.a2dp_app_dev[dev_index].state == BT_A2DP_APP_STATE_OPENED)
        {
            btmtk_a2dp_send_stream_reconfig_res(
                MOD_BT, 
                BT_A2DP_RESULT_OK, 
                msg->stream_handle);
            
            return;
        }
    }
    
    btmtk_a2dp_send_stream_reconfig_res(
                MOD_BT, 
                BT_A2DP_RESULT_BAD_STATE, 
                msg->stream_handle);
    
    // TODO: reconfig is useless now, reserver for future usage.
}


A2dpStatus btmtk_a2dp_handle_stream_open_cnf(ilm_struct *message)
{
    U8 dev_index = 0;
    A2dpStatus iResult = A2DP_STATUS_NO_CHANGE;

    bt_a2dp_stream_open_cnf_struct *msg;

    msg = (bt_a2dp_stream_open_cnf_struct *)message->ilm_data;

    bt_ext_log("[A2DP] Stream Open CNF result: %d, stream_handle: %d, dev_name: %s, device_name_len: %d\n", 
        msg->result,
        msg->stream_handle,
        msg->device_name,       // maybe incrrect because device name is empty.
        msg->device_name_len);

    // XH any return result should update current_dev_addr = msg->device_addr, or application layer can't update UI.
    //bt_a2dp_app.current_dev_addr = msg->device_addr;
    memcpy(&bt_a2dp_app.current_dev_addr, &msg->device_addr, sizeof(bt_addr_struct));
    
    if((msg->result == BT_A2DP_RESULT_OK))
    {
        dev_index = btmtk_a2dp_get_dev_index();
                
        bt_ext_log("[A2DP] open cnf sucess, device index: %d\n", dev_index);
        
        if (UNKNOWN_DEV_INDEX == dev_index)
        {
            if (UNKNOWN_DEV_INDEX != btmtk_a2dp_find_dev_index(msg->stream_handle))
                return iResult;
            	
            bt_ext_log("[A2DP] device context is full, closing...");
            
            iResult = A2DP_STATUS_CHANGE_DISCONNECTED;
            btmtk_a2dp_send_stream_close_req(MOD_BT, msg->stream_handle);
            
            return iResult;
        }

        memset(&bt_a2dp_app.a2dp_app_dev[dev_index], 0, sizeof(BT_A2DP_APP_DEV));
        
        bt_a2dp_app.a2dp_app_dev[dev_index].stream_handle = msg->stream_handle;
        memcpy(&bt_a2dp_app.a2dp_app_dev[dev_index].addr, &msg->device_addr, sizeof(bt_addr_struct));
        if (msg->device_name_len > 0)
            memcpy(bt_a2dp_app.a2dp_app_dev[dev_index].dev_name, msg->device_name, msg->device_name_len + 1); // + 1 stand for "\0"
        bt_ext_log("[A2DP] open cnf dev name len2: %d, name:%s", msg->device_name_len, bt_a2dp_app.a2dp_app_dev[dev_index].dev_name);
        
        bt_a2dp_app.a2dp_app_dev[dev_index].is_opened_by_peer_dev = FALSE;
        
        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_OPENED, msg->stream_handle);

        iResult = A2DP_STATUS_CHANGE_CONNECTED;

        /* send close request if disconnect_pending is true */
        if (TRUE == bt_a2dp_app.a2dp_app_dev[dev_index].disconnect_pending)
        {
            btmtk_a2dp_send_stream_close_req(MOD_BT, msg->stream_handle);
            bt_a2dp_app.a2dp_app_dev[dev_index].disconnect_pending = FALSE;
            iResult = A2DP_STATUS_CHANGE_DISCONNECTED;
        }
    }
    else 
    {
        iResult = A2DP_STATUS_CHANGE_DISCONNECTED;
        if(msg->result == BT_A2DP_RESULT_TIMEOUT)
        {
            bt_ext_log("[A2DP] msg->result : BT_A2DP_RESULT_TIMEOUT");
        }
        else if(msg->result == BT_A2DP_RESULT_PANIC)
        {
            bt_ext_log("[A2DP] msg->result : BT_A2DP_RESULT_PANIC");
        }	
        else if (BT_A2DP_RESULT_NO_CONNECTION == msg->result)
        {
            bt_ext_log("[A2DP] msg->result : BT_A2DP_RESULT_NO_CONNECTION");
            // iResult = A2DP_STATUS_NO_CHANGE;
        }
    }

    return iResult;

}


A2dpStatus btmtk_a2dp_handle_stream_open_ind(ilm_struct *message)
{
    A2dpStatus iResult = A2DP_STATUS_NO_CHANGE;

    U16 result;
    U8 dev_index = 0;
    bt_a2dp_stream_open_ind_struct *msg;
    
    msg = (bt_a2dp_stream_open_ind_struct *)message->ilm_data;
    
    bt_ext_log("[A2DP] Stream Open IND stream_handle: %d, dev_name: %s\n", 
        msg->stream_handle,
        msg->device_name);

    //bt_a2dp_app.current_dev_addr = msg->device_addr;
    memcpy(&bt_a2dp_app.current_dev_addr, &msg->device_addr, sizeof(bt_addr_struct));

    dev_index = btmtk_a2dp_get_dev_index();
    
    if((UNKNOWN_DEV_INDEX != dev_index))        
    {
        memset(&bt_a2dp_app.a2dp_app_dev[dev_index], 0, sizeof(BT_A2DP_APP_DEV));
    	  
        bt_a2dp_app.a2dp_app_dev[dev_index].stream_handle = msg->stream_handle;
        memcpy(&bt_a2dp_app.current_dev_addr, &msg->device_addr, sizeof(bt_addr_struct));
        memcpy(&bt_a2dp_app.a2dp_app_dev[dev_index].addr, &msg->device_addr, sizeof(bt_addr_struct));
        if (msg->device_name_len > 0)
            memcpy(bt_a2dp_app.a2dp_app_dev[dev_index].dev_name, msg->device_name, msg->device_name_len + 1); // + 1 stand for "\0"

        if ( 0 != strncmp((const char*)msg->device_name, "RF-MAB2", 7) )
            bt_a2dp_app.a2dp_app_dev[dev_index].is_opened_by_peer_dev = TRUE;
        
        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_OPENED, msg->stream_handle);
        
        result = BT_A2DP_RESULT_OK;
        iResult = A2DP_STATUS_CHANGE_CONNECTED;
    }
    else
    {
        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_IDLE, msg->stream_handle);
        
        result = BT_A2DP_RESULT_BAD_STATE;
        iResult = A2DP_STATUS_CHANGE_DISCONNECTED;  
    }

    bt_ext_log("[A2DP] open ind, get dev index: %d, result: %d", dev_index, result);
    
    btmtk_a2dp_send_stream_open_res(
        MOD_BT,
        msg->stream_handle, 
        result);

    return iResult;

}


A2dpStatus btmtk_a2dp_handle_stream_start_cnf(ilm_struct *message)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    
    U8 i = 0;
    U8 dev_index = 0;
    A2dpStatus iResult = A2DP_STATUS_NO_CHANGE;
    BT_A2DP_APP_STATE last_glb_state = BT_A2DP_APP_STATE_IDLE;

    bt_a2dp_stream_start_cnf_struct *msg;

    msg = (bt_a2dp_stream_start_cnf_struct *)message->ilm_data;

    bt_ext_log("[A2DP] Stream Start CNF result:%d\n", msg->result);
    bt_ext_log("[A2DP] Stream Start CNF stream_handle: %d PayloadSize:%d\n", msg->stream_handle, msg->prefer_size);

    last_glb_state = btmtk_a2dp_global_state();
    dev_index = btmtk_a2dp_find_dev_index(msg->stream_handle);
    
    if((UNKNOWN_DEV_INDEX == dev_index))
    {
        bt_ext_log("[A2DP] Stream Start CNF error: have not find the device context.hdl: %d\n", msg->stream_handle);
        
        btmtk_a2dp_send_stream_close_req(MOD_BT, msg->stream_handle);
        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_IDLE, msg->stream_handle);
        
        iResult = A2DP_STATUS_CHANGE_DISCONNECTED;
        return iResult;
    }

    /* refresh the current device address or the UI will not update */
    memcpy(&bt_a2dp_app.current_dev_addr, &bt_a2dp_app.a2dp_app_dev[dev_index].addr, sizeof(bt_addr_struct));

    bt_ext_log("[A2DP] index:%d, state: %d, current msg: %d, start req: %d",
        dev_index,
        bt_a2dp_app.a2dp_app_dev[dev_index].state,
        bt_a2dp_app.a2dp_app_dev[dev_index].current_message,
        MSG_ID_BT_A2DP_STREAM_START_REQ);
    
    if (msg->result == BT_A2DP_RESULT_OK && 
        bt_a2dp_app.a2dp_app_dev[dev_index].state == BT_A2DP_APP_STATE_OPENED &&
        (bt_a2dp_app.a2dp_app_dev[dev_index].current_message == MSG_ID_BT_A2DP_STREAM_START_REQ ||
        bt_a2dp_app.a2dp_app_dev[dev_index].current_message == MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_START_REQ ||
        bt_a2dp_app.a2dp_app_dev[dev_index].current_message == MSG_ID_BT_A2DP_STREAM_CLOSE_REQ))
    {
        bt_a2dp_app.a2dp_app_dev[dev_index].current_message = 0;
        bt_a2dp_app.payload_size = msg->prefer_size;
        
        ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_SUCCESS;
        ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->prefer_size = msg->prefer_size;	
        memcpy(&(((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->current_config), &msg->current_config, sizeof (bt_a2dp_audio_cap_struct));	
        memcpy(&bt_a2dp_app.local_audio_cap, &msg->current_config, sizeof (bt_a2dp_audio_cap_struct));	

        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_STREAMING, msg->stream_handle);
        iResult = A2DP_STATUS_CHANGE_PLAYING;
        
        bt_a2dp_app.a2dp_app_dev[dev_index].local_start_req_counter = 0;
        bt_a2dp_app.a2dp_app_dev[dev_index].is_opened_by_peer_dev = FALSE;
        
#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER

        if (bt_a2dp_app.fmOverBtMode == FALSE)

#endif
        {
            /* if this is the first start confirm, then notify media process */
            if (last_glb_state == BT_A2DP_APP_STATE_OPENED)
                btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_START_CNF, ilmptr, sizeof(bt_a2dp_stream_data_start_cnf_struct));
        }

#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER

        if (bt_a2dp_app.fmOverBtPostponedStart == KAL_TRUE) 
        {
            if(btmtk_a2dp_fm_send_start_req()== START_SUCCESS){
				bt_ext_log("[A2DP] FM send start req success");
			}else{
				bt_ext_log("[A2DP] FM send start req failed");
			}
            bt_a2dp_app.fmOverBtPostponedStart = KAL_FALSE;
        }

#endif
        /* send close request if disconnect_pending is true */
        if (TRUE == bt_a2dp_app.a2dp_app_dev[dev_index].disconnect_pending)
        {
            btmtk_a2dp_send_stream_close_req(MOD_BT, msg->stream_handle);
            bt_a2dp_app.a2dp_app_dev[dev_index].disconnect_pending = FALSE;
            // iResult = A2DP_STATUS_CHANGE_DISCONNECTED;
        }

    }
    else
    {
        // TODO: ASSERT  instead of set current_message to zero ?
        if (bt_a2dp_app.a2dp_app_dev[dev_index].current_message == MSG_ID_BT_A2DP_STREAM_START_REQ)
            bt_a2dp_app.a2dp_app_dev[dev_index].current_message = 0;
        else {
            bt_ext_log("[A2DP] start cnf, current message mismatch: %d\n", bt_a2dp_app.a2dp_app_dev[dev_index].current_message);
            bt_a2dp_app.a2dp_app_dev[dev_index].current_message = 0;
        }
        
#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER

		if(bt_a2dp_app.fmOverBtMode == TRUE)
        {
			iResult = A2DP_STATUS_FM_PLAYING_FAILED;
			bt_a2dp_app.fmOverBtMode = FALSE;
            
			return iResult;
		}
        
#endif
        if (bt_a2dp_app.a2dp_app_dev[dev_index].state != BT_A2DP_APP_STATE_OPENED) {
            btmtk_a2dp_send_stream_close_req(MOD_BT, msg->stream_handle);
            // iResult = A2DP_STATUS_CHANGE_DISCONNECTED;
        }
        
        ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_FAILED;
        

        U8 start_counter = 0;
        for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
            if (bt_a2dp_app.a2dp_app_dev[i].current_message == MSG_ID_BT_A2DP_STREAM_START_REQ)
                start_counter++;

        bt_ext_log("[A2DP] start cnf error last glb state: %d, start count:%d", last_glb_state, start_counter);

        /* if this is the last start failed confirm message, then notify media process that A2DP start fail */
        if (last_glb_state == BT_A2DP_APP_STATE_OPENED && start_counter <= 0)
            btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_START_CNF, ilmptr, sizeof(bt_a2dp_stream_data_start_cnf_struct));
    }

    return iResult;
    
}



A2dpStatus btmtk_a2dp_handle_stream_start_ind(ilm_struct *message)
{
    
    U16 result;
    U8 dev_index = 0;
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    A2dpStatus iResult = A2DP_STATUS_NO_CHANGE;
    bt_a2dp_stream_start_ind_struct *msg;
    BT_A2DP_APP_STATE last_glb_state = BT_A2DP_APP_STATE_IDLE;

    msg = (bt_a2dp_stream_start_ind_struct *)message->ilm_data;
  
    bt_ext_log("[A2DP] Stream Start IND stream_handle: %d\n", msg->stream_handle);

    last_glb_state = btmtk_a2dp_global_state();
    dev_index = btmtk_a2dp_find_dev_index(msg->stream_handle);
    
    if((UNKNOWN_DEV_INDEX == dev_index))
    {
        bt_ext_log("[A2DP] Stream Start ind error: have not find the device context.hdl: %d\n", msg->stream_handle);
        
        result = BT_A2DP_RESULT_BAD_STATE;
        iResult = A2DP_STATUS_CHANGE_DISCONNECTED;
        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_IDLE, msg->stream_handle);
    }
    else
    {
        /* refresh the current device address or the UI will not update */
        memcpy(&bt_a2dp_app.current_dev_addr, &bt_a2dp_app.a2dp_app_dev[dev_index].addr, sizeof(bt_addr_struct));
        
        if (bt_a2dp_app.a2dp_app_dev[dev_index].state == BT_A2DP_APP_STATE_OPENED)
        {
            bt_ext_log("[A2DP] Stream Start ind .\n");
            bt_a2dp_app.payload_size = msg->prefer_size;
            bt_a2dp_app.a2dp_app_dev[dev_index].is_opened_by_peer_dev = FALSE;
            bt_a2dp_app.a2dp_app_dev[dev_index].local_start_req_counter = 0;
            
            ((bt_a2dp_stream_data_start_ind_struct*)ilmptr->ilm_data)->prefer_size = msg->prefer_size;	
            memcpy(&((bt_a2dp_stream_data_start_ind_struct*)ilmptr->ilm_data)->current_config, &msg->current_config, sizeof (bt_a2dp_audio_cap_struct));	
            memcpy(&bt_a2dp_app.local_audio_cap, &msg->current_config, sizeof (bt_a2dp_audio_cap_struct));	

            result = BT_A2DP_RESULT_OK;
            iResult = A2DP_STATUS_CHANGE_PLAYING;

            btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_STREAMING, msg->stream_handle);
            
            if (last_glb_state == BT_A2DP_APP_STATE_OPENED)
                btmtk_a2dp_send_msg_2_audio_module(
                    MSG_ID_BT_A2DP_STREAM_START_IND, 
                    ilmptr, 
                    sizeof(bt_a2dp_stream_data_start_ind_struct));
        }
        else
        {
            bt_ext_log("[A2DP] Stream Start ind error: bad state.\n");
            
            result = BT_A2DP_RESULT_BAD_STATE;
            iResult = A2DP_STATUS_CHANGE_DISCONNECTED;

            /* state will not change */
            // btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_OPENED, msg->stream_handle);
        }
    }

    btmtk_a2dp_send_stream_start_res(MOD_BT, result, msg->stream_handle);

#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER

    if (bt_a2dp_app.fmOverBtPostponedStart == KAL_TRUE) 
    {
        // TODO: maybe need to sleep some seconds.
        if(btmtk_a2dp_fm_send_start_req()== START_SUCCESS){
			bt_ext_log("[A2DP] FM send start req success");
		}else{
			bt_ext_log("[A2DP] FM send start req failed");
		}
        bt_a2dp_app.fmOverBtPostponedStart = KAL_FALSE;
    }

#endif

    return iResult;
}


A2dpStatus btmtk_a2dp_handle_stream_suspend_cnf(ilm_struct *message)
{

    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    
    U8 dev_index = 0;
    U8 pause_counter = 0;
    A2dpStatus iResult = A2DP_STATUS_NO_CHANGE;
    BT_A2DP_APP_STATE glb_state = BT_A2DP_APP_STATE_IDLE;

    bt_a2dp_stream_pause_cnf_struct *msg;

    msg = (bt_a2dp_stream_pause_cnf_struct *)message->ilm_data;

    bt_ext_log("[A2DP] Stream suspend CNF result:%d\n", msg->result);
    bt_ext_log("[A2DP] Stream suspend CNF stream_handle: %d\n", msg->stream_handle);

    dev_index = btmtk_a2dp_find_dev_index(msg->stream_handle);
    
    if((UNKNOWN_DEV_INDEX == dev_index))
    {
        bt_ext_log("[A2DP] Stream suspend CNF error: have not find the device context.hdl: %d\n", msg->stream_handle);
        
        btmtk_a2dp_send_stream_close_req(MOD_BT, msg->stream_handle);
        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_IDLE, msg->stream_handle);
        HfpCallback(BTIF_AV_STATE_IDLE);
        
        iResult = A2DP_STATUS_CHANGE_DISCONNECTED;
        return iResult;
    }

    /* refresh the current device address or the UI will not update */
    memcpy(&bt_a2dp_app.current_dev_addr, &bt_a2dp_app.a2dp_app_dev[dev_index].addr, sizeof(bt_addr_struct));
    
    if (msg->result == BT_A2DP_RESULT_OK && 
        bt_a2dp_app.a2dp_app_dev[dev_index].state == BT_A2DP_APP_STATE_STREAMING &&
        (bt_a2dp_app.a2dp_app_dev[dev_index].current_message == MSG_ID_BT_A2DP_STREAM_PAUSE_REQ ||
        bt_a2dp_app.a2dp_app_dev[dev_index].current_message == MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_SUSPEND_REQ ||
        bt_a2dp_app.a2dp_app_dev[dev_index].current_message == MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_STOP_REQ))
    {
        bt_a2dp_app.a2dp_app_dev[dev_index].current_message = 0;
        
#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER

        bt_ext_log("[a2dp] suspend start:%d, suspend:%d", bt_a2dp_app.fmStartContinue, bt_a2dp_app.fmSuspending);
        /* handle FM start request (continue to send reconfig and start) */
        if (bt_a2dp_app.fmStartContinue == KAL_TRUE || bt_a2dp_app.fmSuspending == KAL_TRUE)
        {
            int reconfig_result = a2dp_fm_reconfig_procedure();
    
        	if(reconfig_result == RECONFIG_SUCCESS)
            {
                btmtk_a2dp_send_stream_reconfig_req( MOD_BT,
                                                0,
                                                &bt_a2dp_app.local_audio_cap);
            } 
            else if(bt_a2dp_app.fmOverBtMode == TRUE && bt_a2dp_app.fmStartContinue == KAL_FALSE)
            {
                bt_a2dp_app.fmOverBtMode = FALSE;
            }
            bt_a2dp_app.fmSuspending = KAL_FALSE;
        }
        
#endif

        ((bt_a2dp_stream_data_stop_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_SUCCESS;

        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_OPENED, msg->stream_handle);
        iResult = A2DP_STATUS_CHANGE_CONNECTED;

        glb_state = btmtk_a2dp_global_state();

        /* if this is the last pause confirm message, then notify media process that A2DP has been suspended */
        if (glb_state == BT_A2DP_APP_STATE_OPENED)
            btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_STOP_CNF, ilmptr, sizeof(bt_a2dp_stream_data_start_cnf_struct));

        /* send close request if disconnect_pending is true */
        if (TRUE == bt_a2dp_app.a2dp_app_dev[dev_index].disconnect_pending)
        {
            btmtk_a2dp_send_stream_close_req(MOD_BT, msg->stream_handle);
            bt_a2dp_app.a2dp_app_dev[dev_index].disconnect_pending = FALSE;
            iResult = A2DP_STATUS_CHANGE_DISCONNECTED;
        }
        HfpCallback(BTIF_AV_STATE_OPENED);
    }
    else
    {
        if (bt_a2dp_app.a2dp_app_dev[dev_index].current_message != MSG_ID_BT_A2DP_STREAM_PAUSE_REQ) 
        {
            bt_ext_log("[A2DP] suspend cnf, current message mismatch: %d\n", bt_a2dp_app.a2dp_app_dev[dev_index].current_message);
        }
        
        bt_a2dp_app.a2dp_app_dev[dev_index].current_message = 0;

        btmtk_a2dp_send_stream_close_req(MOD_BT, msg->stream_handle);

        iResult = A2DP_STATUS_CHANGE_DISCONNECTED;
        HfpCallback(BTIF_AV_STATE_IDLE);
    }

    return iResult;
    
}


A2dpStatus btmtk_a2dp_handle_stream_suspend_ind(ilm_struct *message)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    
    A2dpStatus iResult = A2DP_STATUS_NO_CHANGE;

    bt_a2dp_stream_pause_ind_struct *msg;
    U16 result, dev_index = 0;


    msg = (bt_a2dp_stream_pause_ind_struct *)message->ilm_data;

    dev_index = btmtk_a2dp_find_dev_index(msg->stream_handle);
    
    if((UNKNOWN_DEV_INDEX == dev_index))
    {
        bt_ext_log("[A2DP] Stream suspend ind error: have not find the device context. hdl: %d\n", msg->stream_handle);
        
        btmtk_a2dp_send_stream_close_req(MOD_BT, msg->stream_handle);
        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_IDLE, msg->stream_handle);
        
        iResult = A2DP_STATUS_CHANGE_DISCONNECTED;
        return iResult;
    }

    /* refresh the current device address or the UI will not update */
    memcpy(&bt_a2dp_app.current_dev_addr, &bt_a2dp_app.a2dp_app_dev[dev_index].addr, sizeof(bt_addr_struct));

    if (bt_a2dp_app.a2dp_app_dev[dev_index].state == BT_A2DP_APP_STATE_STREAMING)
    {
        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_OPENED, msg->stream_handle);
        
        result = BT_A2DP_RESULT_OK;
        iResult = A2DP_STATUS_CHANGE_CONNECTED;
    }
    else
    {
        result = BT_A2DP_RESULT_BAD_STATE;
        // TODO: need to close streaming channel ?
    }   

    btmtk_a2dp_send_stream_pause_res(
        MOD_BT, 
        result, 
        msg->stream_handle);

    // if (!strncmp((const char*)&bt_a2dp_app.a2dp_app_dev[0].dev_name[0], "HM3500", 6))
    {
        ((bt_a2dp_stream_data_stop_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_SUCCESS;
        btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_STOP_CNF, ilmptr, sizeof(bt_a2dp_stream_data_start_cnf_struct));
    }
    
#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER

    if (bt_a2dp_app.fmOverBtMode == TRUE)
    {
        bt_a2dp_app.fmOverBtMode = FALSE;
    }

    /* need to reconfig codec in order to transmit audio data */
    // TODO: maybe we should move the following statement to a2dp.c
    int reconfig_result = a2dp_fm_reconfig_procedure();
    
    if(reconfig_result == RECONFIG_SUCCESS)
    {
        btmtk_a2dp_send_stream_reconfig_req( MOD_BT,
                                        0,
                                        &bt_a2dp_app.local_audio_cap);
    }
        
#endif

    bt_ext_log("[A2DP] Stream Suspend IND stream_handle: %d\n", msg->stream_handle);

    return iResult;

}

A2dpStatus btmtk_a2dp_handle_stream_close_cnf(ilm_struct *message)
{
    U8 dev_index = 0;
    bt_a2dp_stream_close_cnf_struct *msg;

    A2dpStatus iResult = A2DP_STATUS_NO_CHANGE;
    
    msg = (bt_a2dp_stream_close_cnf_struct *)message->ilm_data;
    dev_index = btmtk_a2dp_find_dev_index(msg->stream_handle);  

    bt_ext_log("[A2DP] Stream Close CNF result:%d\n", msg->result);
    bt_ext_log("[A2DP] Stream Close CNF stream_handle: %d\n", msg->stream_handle);

    if((UNKNOWN_DEV_INDEX == dev_index))
    {
        bt_ext_log("[A2DP] Stream close cnf error: have not find the device context. hdl: %d\n", msg->stream_handle);
        
        iResult = A2DP_STATUS_CHANGE_DISCONNECTED;
        return iResult;
    }
    
    /* refresh the current device address or the UI will not update */
    memcpy(&bt_a2dp_app.current_dev_addr, &bt_a2dp_app.a2dp_app_dev[dev_index].addr, sizeof(bt_addr_struct));
    
    if((msg->result == BT_A2DP_RESULT_OK))
    {
        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_IDLE, msg->stream_handle);
        
        iResult = A2DP_STATUS_CHANGE_DISCONNECTED;

        /* try to deactive a2dp when deactivate_pending set to true */
        if (btmtk_a2dp_global_state() == BT_A2DP_APP_STATE_IDLE && bt_a2dp_app.deactivate_penging) 
        {
            btmtk_a2dp_send_deactivate_req(MOD_BT);
            bt_a2dp_app.deactivate_penging = FALSE;
        }
        memset(&bt_a2dp_app.a2dp_app_dev[dev_index], 0 , sizeof (BT_A2DP_APP_DEV));
        bt_a2dp_app.a2dp_app_dev[dev_index].stream_handle = UNKNOWN_STREAM_HANDLE;

        
#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER

        bt_a2dp_app.fmOverBtMode = FALSE;
        bt_a2dp_app.fmOverBtPostponedStart = FALSE;
        bt_a2dp_app.fmStartContinue = FALSE;
        bt_a2dp_app.fmStopContinue = FALSE;
        bt_a2dp_app.fmSuspending = FALSE;
            
#endif

    }
/*
    if (bt_a2dp_app.a2dp_app_dev[dev_index].current_message != MSG_ID_BT_A2DP_STREAM_CLOSE_REQ)
    {
        bt_ext_log("[A2DP] close cnf, current message mismatch: %d\n", bt_a2dp_app.a2dp_app_dev[dev_index].current_message);
    }
    bt_a2dp_app.a2dp_app_dev[dev_index].current_message = 0;
*/    
    return iResult;
    // TODO: if close confirm is rejected, should disconnect signal channel in GAVDP layer.
}

A2dpStatus btmtk_a2dp_handle_stream_close_ind(ilm_struct *message)
{
    U8 dev_index = 0;
    bt_a2dp_stream_close_ind_struct *msg;

    A2dpStatus iResult = A2DP_STATUS_NO_CHANGE;
 
    msg = (bt_a2dp_stream_close_ind_struct *)message->ilm_data;
    dev_index = btmtk_a2dp_find_dev_index(msg->stream_handle);  

    bt_ext_log("[A2DP] close ind, dev index:%d, stream hdl:%d", dev_index, msg->stream_handle);
    if((UNKNOWN_DEV_INDEX == dev_index))
    {
        bt_ext_log("[A2DP] Stream close cnf error: have not find the device context. hdl: %d\n", msg->stream_handle);
        
        btmtk_a2dp_send_stream_close_res(MOD_BT, BT_A2DP_RESULT_OK, msg->stream_handle);
        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_IDLE, msg->stream_handle);
        return A2DP_STATUS_CHANGE_DISCONNECTED;
    }
    
    /* refresh the current device address or the UI will not update */
    memcpy(&bt_a2dp_app.current_dev_addr, &bt_a2dp_app.a2dp_app_dev[dev_index].addr, sizeof(bt_addr_struct));
    
    if((btmtk_a2dp_app_get_stream_state(msg->stream_handle) == BT_A2DP_APP_STATE_STREAMING) ||
        (btmtk_a2dp_app_get_stream_state(msg->stream_handle) == BT_A2DP_APP_STATE_OPENED))
    {
        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_IDLE, msg->stream_handle);
        
        bt_a2dp_app.a2dp_app_dev[dev_index].stream_handle = UNKNOWN_STREAM_HANDLE;
        bt_a2dp_app.a2dp_app_dev[dev_index].is_opened_by_peer_dev = FALSE;
        bt_a2dp_app.a2dp_app_dev[dev_index].local_start_req_counter = 0;
    }

    btmtk_a2dp_send_stream_close_res(
        MOD_BT, 
        BT_A2DP_RESULT_OK, 
        msg->stream_handle);

    iResult = A2DP_STATUS_CHANGE_DISCONNECTED;

    if (btmtk_a2dp_global_state() == BT_A2DP_APP_STATE_IDLE)
    {
        /* notify audio process that A2DP is going to disconnected */
        ilm_struct ilm;
        ilm_struct *ilmptr;
        ilmptr = &ilm;
        
        btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_DISC_IND, ilmptr, sizeof(bt_a2dp_stream_data_disc_ind_struct));


        /* try to deactive a2dp when deactivate_pending set to true */
        if (bt_a2dp_app.deactivate_penging) 
        {
            usleep(3000); // sleep 300 millisec in order to disconnect signal channel
            btmtk_a2dp_send_deactivate_req(MOD_BT);
            bt_a2dp_app.deactivate_penging = FALSE;
        }
    }
    
    
#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER

    if (bt_a2dp_app.fmOverBtMode == TRUE)
    {
        bt_a2dp_app.fmOverBtMode = FALSE;
    }
            
#endif

    bt_ext_log("[A2DP] Stream Close IND stream_handle: %d\n", msg->stream_handle);

    return iResult;

}

A2dpStatus btmtk_a2dp_handle_stream_abort_cnf(ilm_struct *message)
{
    U8 dev_index = 0;
    bt_a2dp_stream_abort_cnf_struct *msg;

    msg = (bt_a2dp_stream_abort_cnf_struct *)message->ilm_data;
    dev_index = btmtk_a2dp_find_dev_index(msg->stream_handle);

    bt_ext_log("[A2DP] a2dp stream abort cnf, stream_handle:%d, dev_index: %d\n", msg->stream_handle, dev_index);

    if (UNKNOWN_DEV_INDEX == dev_index) {
        bt_ext_log("[A2DP] a2dp streaming abort cnf error, reason: bad stream handle %d\n", msg->stream_handle);
        return A2DP_STATUS_NO_CHANGE;
    }

    /* refresh the current device address or the UI will not update */
    memcpy(&bt_a2dp_app.current_dev_addr, &bt_a2dp_app.a2dp_app_dev[dev_index].addr, sizeof(bt_addr_struct));

    // should not reset device context because lower layer will send close indication to external
    // memset(&bt_a2dp_app.a2dp_app_dev[dev_index], 0, sizeof(BT_A2DP_APP_DEV));
    // bt_a2dp_app.a2dp_app_dev[dev_index].stream_handle = UNKNOWN_STREAM_HANDLE;

#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER
    
    if (bt_a2dp_app.fmOverBtMode == TRUE)
    {
        bt_a2dp_app.fmOverBtMode = FALSE;
    }
                
#endif
    btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_IDLE, msg->stream_handle);
    btmtk_a2dp_send_stream_close_req(MOD_BT, msg->stream_handle);

    return A2DP_STATUS_NO_CHANGE;
}

A2dpStatus btmtk_a2dp_handle_stream_abort_ind(ilm_struct *message)
{
    U8 dev_index = 0;
    bt_a2dp_stream_abort_ind_struct *msg;

    msg = (bt_a2dp_stream_abort_ind_struct *)message->ilm_data;
    dev_index = btmtk_a2dp_find_dev_index(msg->stream_handle);

    bt_ext_log("[A2DP] a2dp stream abort ind, stream_handle:%d, dev_index: %d\n", msg->stream_handle, dev_index);

    if (UNKNOWN_DEV_INDEX == dev_index) {
        bt_ext_log("[A2DP] a2dp streaming abort ind error, reason: bad stream handle %d\n", msg->stream_handle);
        return A2DP_STATUS_NO_CHANGE;
    }

    /* refresh the current device address or the UI will not update */
    memcpy(&bt_a2dp_app.current_dev_addr, &bt_a2dp_app.a2dp_app_dev[dev_index].addr, sizeof(bt_addr_struct));
    
    // should not reset device context, just need to change the state to disconnected.
    // memset(&bt_a2dp_app.a2dp_app_dev[dev_index], 0, sizeof(BT_A2DP_APP_DEV));
    // bt_a2dp_app.a2dp_app_dev[dev_index].stream_handle = UNKNOWN_STREAM_HANDLE;

    btmtk_a2dp_send_stream_abort_res(
        MOD_BT, 
        msg->stream_handle);

    btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_IDLE, msg->stream_handle);

#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER
        
    if (bt_a2dp_app.fmOverBtMode == TRUE)
    {
        bt_a2dp_app.fmOverBtMode = FALSE;
    }
                
#endif

    return A2DP_STATUS_NO_CHANGE;
}

void btmtk_a2dp_handle_stream_qos_ind(ilm_struct *message)
{
    bt_a2dp_stream_qos_ind_struct *msg;

    msg = (bt_a2dp_stream_qos_ind_struct *)message->ilm_data;

#if 1
    bt_ext_log("[A2DP] Stream QoS IND stream_handle: %d\n", msg->stream_handle);
    bt_ext_log("[A2DP] Stream QoS IND qos: %d\n", msg->qos);
#endif

    /* notify audio process the QoS value */
    
    btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_QOS_IND, message, sizeof(bt_a2dp_stream_qos_ind_struct));
}


void btmtk_a2dp_handle_authorize_res(ilm_struct *message)
{
/*
    bt_a2dp_connect_accept_res_struct *msg;
    U8 addr[6];
    
    msg = (bt_a2dp_connect_accept_res_struct *)message->ilm_data;

    btmtk_a2dp_app_convert_addr_from_Stack((U8 *)&addr, &bt_a2dp_app.addr);

    if(msg->accept == 0)
    {
        bt_a2dp_app.connected = 0;
        bt_a2dp_app.get_remote_audio_cap_state = 0;
        btmtk_a2dp_app_set_state(BT_A2DP_APP_STATE_IDLE);
        bt_a2dp_app.a2dp_operation = BT_A2DP_APPI_COMMAND_NONE;
        btmtk_a2dp_send_signal_connect_res(MOD_BT, (U8 *)&addr, 0);
    }
    else
    {       
        btmtk_a2dp_send_signal_connect_res(MOD_BT, (U8 *)&addr, 1);
    }
*/
}


kal_bool btmtk_a2dp_app_is_last_connection(void)
{
    U8 opened_dev_num = 0;
    U8 i = 0;

    for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
    {
        if (bt_a2dp_app.a2dp_app_dev[i].state >= BT_A2DP_APP_STATE_OPENED)
            opened_dev_num++;
    }

    if (opened_dev_num > 1)
        return KAL_FALSE;
    else
        return KAL_TRUE;
}


void btmtk_a2dp_app_default_setting_initialized(void)
{  
    U8 i = 0;

    memset(&bt_a2dp_app, 0, sizeof(BT_A2DP_APP_CTX));

    for (i = 0; i < MAX_A2DP_DEV_NUM; i++)
        bt_a2dp_app.a2dp_app_dev[i].stream_handle = UNKNOWN_STREAM_HANDLE;

    bt_a2dp_app.local_audio_cap.codec_type = BT_A2DP_SBC;
    bt_a2dp_app.local_audio_cap.codec_cap.sbc.min_bit_pool = 19;   
    bt_a2dp_app.local_audio_cap.codec_cap.sbc.max_bit_pool = 75;   
    bt_a2dp_app.local_audio_cap.codec_cap.sbc.block_len = 0x01;   
    bt_a2dp_app.local_audio_cap.codec_cap.sbc.subband_num = 0x01;   
    bt_a2dp_app.local_audio_cap.codec_cap.sbc.alloc_method = 0x01;   
    bt_a2dp_app.local_audio_cap.codec_cap.sbc.sample_rate = 0x2;    
    bt_a2dp_app.local_audio_cap.codec_cap.sbc.channel_mode = 0x1; 
}

kal_bool btmtk_a2dp_app_verify_connection_id(U16 connection_id)
{ 
    return TRUE;
}


kal_bool btmtk_a2dp_app_verify_stream_handle(U8 stream_handle)
{
    return TRUE;
}

void btmtk_a2dp_app_set_stream_state(BT_A2DP_APP_STATE state, U8 stream_handle)
{
    U8 index = btmtk_a2dp_find_dev_index(stream_handle);

    if (index != UNKNOWN_DEV_INDEX) 
    {
        bt_a2dp_app.a2dp_app_dev[index].state = state;
        bt_ext_log("[A2DP] btmtk_a2dp_app_set_state, stream handle: %d, new state: %d\n", state);
    }
    else
        bt_ext_log("[A2DP] btmtk_a2dp_app_set_state, have not found the device index\n");
}

BT_A2DP_APP_STATE btmtk_a2dp_app_get_stream_state(U8 stream_handle)
{
    U8 index = 0;
    
    index = btmtk_a2dp_find_dev_index(stream_handle);
    
    if (UNKNOWN_DEV_INDEX == index)
        return BT_A2DP_APP_STATE_UNKNOWN;
    
    return (bt_a2dp_app.a2dp_app_dev[index].state);

}

void btmtk_a2dp_handle_operation(U8 index)
{

    bt_ext_log("[A2DP] btmtk_a2dp_handle_operation:%02x", index);
    if (index > MAX_A2DP_DEV_NUM) return;

    if(bt_a2dp_app.a2dp_app_dev[index].current_message == 0)
    {
        /* Only when an outgoing command is completed, we can change to another state */
        switch(bt_a2dp_app.a2dp_app_dev[index].state)
        {
            case BT_A2DP_APP_STATE_IDLE:
                // btmtk_a2dp_state_idle();
                bt_ext_log("[A2DP] btmtk_a2dp_handle_operation idle successfully");
                break;
            //case BT_A2DP_APP_STATE_CONFIGURED:
                // btmtk_a2dp_state_configed();
                //break;
            case BT_A2DP_APP_STATE_OPENED:
                // btmtk_a2dp_state_opened();
                break;
            case BT_A2DP_APP_STATE_STREAMING:
                // btmtk_a2dp_state_streaming();
                break;
            default:
                break;
        }
    }
}



void btmtk_a2dp_state_error_handling(void)
{
    // bt_a2dp_app.a2dp_operation = BT_A2DP_APPI_COMMAND_NONE;
}


void btmtk_a2dp_app_convert_addr_from_Stack(U8 *addr_target, bt_addr_struct *addr_src)
{
    addr_target[5] = (unsigned char)(addr_src->lap & 0x000000FF);
    addr_target[4] = (unsigned char)((addr_src->lap & 0x0000FF00) >> 8);
    addr_target[3] = (unsigned char)((addr_src->lap & 0x00FF0000) >> 16);
    addr_target[2] = (unsigned char)(addr_src->uap);
    addr_target[1] = (unsigned char)((addr_src->nap & 0x00FF));
    addr_target[0] = (unsigned char)((addr_src->nap & 0xFF00) >> 8);
}


U8 btmtk_a2dp_is_connected(void)
{
#if 0
    if(bt_a2dp_app.connected == 1)
        return 1;
    else
        return 0;
#endif
    // TODO: modify it.
    return 1;
}

U8 btmtk_a2dp_appi_compare_capability(bt_a2dp_audio_cap_struct *aud_cfg1, bt_a2dp_audio_cap_struct *aud_cfg2)
{
    U8 result = 0;
    bt_a2dp_sbc_codec_cap_struct *sbc_cfg1, *sbc_cfg2;
    
    bt_ext_log("[A2DP] btmtk_a2dp_appi_compare_capability");
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_cfg1->codec_type == aud_cfg2->codec_type)
    {
        switch (aud_cfg1->codec_type)
        {
            case BT_A2DP_SBC:
                sbc_cfg1 = &aud_cfg1->codec_cap.sbc;
                sbc_cfg2 = &aud_cfg2->codec_cap.sbc;
                OS_Report("data1:%02x,%02x",sbc_cfg1->block_len,sbc_cfg2->block_len);
                OS_Report("data2:%02x,%02x",sbc_cfg1->subband_num,sbc_cfg2->subband_num);
                OS_Report("data3:%02x,%02x",sbc_cfg1->alloc_method,sbc_cfg2->alloc_method);
                OS_Report("data4:%02x,%02x",sbc_cfg1->sample_rate,sbc_cfg2->sample_rate);
                OS_Report("data5:%02x,%02x",sbc_cfg1->channel_mode,sbc_cfg2->channel_mode);

                result = ((sbc_cfg1->block_len == sbc_cfg2->block_len &&
                    sbc_cfg1->subband_num == sbc_cfg2->subband_num &&
                    ((sbc_cfg1->alloc_method & sbc_cfg2->alloc_method) != 0) &&
                    sbc_cfg1->sample_rate == sbc_cfg2->sample_rate &&
                    ((sbc_cfg1->channel_mode & sbc_cfg2->channel_mode)!=0))?1:0);
				ALOGD("result:%d",result);
				
                break;
        }
    }
    return result;
}

U8 btmtk_a2dp_appi_ngeo_config(
                                            bt_a2dp_audio_cap_struct *cfg1, 
                                            bt_a2dp_audio_cap_struct *cfg2, 
                                            bt_a2dp_audio_cap_struct *result)
{
    U8 i=0;
#ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][A2DP][func] btmtk_a2dp_appi_ngeo_config");
#endif
    ALOGD("[BTMTK][A2DP][func] btmtk_a2dp_appi_ngeo_config");
    memset(result, 0, sizeof(bt_a2dp_audio_cap_struct));
    result->codec_type = cfg1->codec_type;
    if(cfg1->codec_cap.sbc.max_bit_pool > cfg2->codec_cap.sbc.max_bit_pool)
        result->codec_cap.sbc.max_bit_pool = cfg2->codec_cap.sbc.max_bit_pool;                        
    else 
        result->codec_cap.sbc.max_bit_pool = cfg1->codec_cap.sbc.max_bit_pool;
    if(cfg1->codec_cap.sbc.min_bit_pool < cfg2->codec_cap.sbc.min_bit_pool)
        result->codec_cap.sbc.min_bit_pool = cfg2->codec_cap.sbc.min_bit_pool;                        
    else 
        result->codec_cap.sbc.min_bit_pool = cfg1->codec_cap.sbc.min_bit_pool;
    if(result->codec_cap.sbc.max_bit_pool < result->codec_cap.sbc.min_bit_pool)
    {
        OS_Report("bit pool error");
		ALOGE("[BTMTK][A2DP][func] bit pool error");
        return 0;                    
    }
    for(i=0;i<4;i++)
    {
        
        if (((cfg1->codec_cap.sbc.block_len & (0x01<< i)) !=0) &&
            ((cfg2->codec_cap.sbc.block_len & (0x01<< i)) !=0))
        {
            result->codec_cap.sbc.block_len = 0x01 << i;
            break;
        }
    }
    if (result->codec_cap.sbc.block_len == 0x00)
    {
        OS_Report("block_len error");
		ALOGE("[BTMTK][A2DP][func] block_len error");
        return 0;
    }
    for(i=0;i<2;i++)
    {
        if (((cfg1->codec_cap.sbc.subband_num & (0x01<< i)) !=0) &&
            ((cfg2->codec_cap.sbc.subband_num & (0x01<< i)) !=0))
        {
            result->codec_cap.sbc.subband_num = 0x01 << i;
            break;
        }
    }
    if (result->codec_cap.sbc.subband_num == 0x00)
    {
        OS_Report("subband_num error");
		ALOGE("[BTMTK][A2DP][func] subband_num error");
        return 0;                    
    }                
    for(i=0;i<2;i++)
    {
        if (((cfg1->codec_cap.sbc.alloc_method & (0x01<< i)) !=0) &&
            ((cfg2->codec_cap.sbc.alloc_method & (0x01<< i)) !=0))
        {
            result->codec_cap.sbc.alloc_method = 0x01 << i;
            break;
        }
    }
    if (result->codec_cap.sbc.alloc_method == 0x00)
    {
        OS_Report("alloc_method error");
		ALOGE("[BTMTK][A2DP][func] alloc_method error");
        return 0;                   
    }                   
    for(i=0;i<4;i++)
    {
        if (((cfg1->codec_cap.sbc.sample_rate & (0x01<< i)) !=0) &&
            ((cfg2->codec_cap.sbc.sample_rate & (0x01<< i)) !=0))
        {
            result->codec_cap.sbc.sample_rate = 0x01 << i;
            break;
        }
    }
    if (result->codec_cap.sbc.sample_rate == 0x00)
    {
        OS_Report("sample_rate error");
		ALOGE("[BTMTK][A2DP][func] sample_rate error");
        return 0;                    
    }                   
    for(i=0;i<4;i++)
    {
        if (((cfg1->codec_cap.sbc.channel_mode & (0x01<< i)) !=0) &&
            ((cfg2->codec_cap.sbc.channel_mode & (0x01<< i)) !=0))
        {
            result->codec_cap.sbc.channel_mode = 0x01 << i;
            break;
        }
    }
    if (result->codec_cap.sbc.channel_mode == 0x00)
    {
        OS_Report("channel_mode error");
		ALOGE("[BTMTK][A2DP][func] channel_mode error");
        return 0;
    }          
    
    return 1;

}


void btmtk_a2dp_pause_immediately(void)
{
    // bt_a2dp_app.appi_command_pause_immediately = 1;
}

void btmtk_a2dp_get_bdaddr(bt_addr_struct *addr)
{
    memcpy(addr, &bt_a2dp_app.current_dev_addr, sizeof(bt_addr_struct));
    bt_ext_log("[A2DP] jni get bd address: %x,%x,%x", addr->lap, addr->nap, addr->uap);
}

void btmtk_a2dp_inform_connection(void)
{
    // bt_a2dp_app.bConnecting =  TRUE;
}

A2dpStatus btmtk_a2dp_get_ap_state(void)
{
/*
   if(bt_a2dp_app.bConnecting || bt_a2dp_app.state == BT_A2DP_APP_STATE_CONFIGURED)
       return A2DP_STATUS_CHANGE_CONNECTING;
   else if(bt_a2dp_app.state == BT_A2DP_APP_STATE_OPENED)
   	return A2DP_STATUS_CHANGE_CONNECTED;
   else if(bt_a2dp_app.state == BT_A2DP_APP_STATE_STREAMING)
       return A2DP_STATUS_CHANGE_PLAYING;
   else
   	return A2DP_STATUS_CHANGE_DISCONNECTED;
*/
    // TODO: modify it.
    return A2DP_STATUS_CHANGE_DISCONNECTED;
}

kal_bool btmtk_a2dp_is_activate(void)
{
    return g_bActivate;
}

U16 btmtk_a2dp_get_payload_size(void)
{
    return bt_a2dp_app.payload_size;
}

void btmtk_a2dp_send_msg_2_audio_module(msg_type msg_id, ilm_struct * ilm, U16 size)
{
    int ret;
    struct sockaddr_un data_addr;
    socklen_t   data_addrlen;
    U16 *field;
    //-----------------------------------------------------------------
    //           set a2dp data socket address
    //-----------------------------------------------------------------
    //data_addr.sun_family = AF_UNIX;
    //strcpy(data_addr.sun_path, BT_SOCK_NAME_EXT_ADP_A2DP_DATA);
    //data_addrlen = (offsetof (struct sockaddr_un, sun_path) + strlen (data_addr.sun_path) + 1);    
    socket_make_sockaddr_un(BT_SOCK_NAME_EXT_ADP_A2DP_DATA, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&data_addr, &data_addrlen);

    /* Set src_mod to MOD_MMI */
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = MOD_MMI;
    ilm->sap_id = 0;
    ilm->msg_id = msg_id;

    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm->ilm_data;
    *field = 1;
    *(++field) = size;

    // cal whole ilm size
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
    bt_ext_log("[A2DP] A2DPHDL_SendMessage size: %d", size);
    if(g_srvcsockfd>=0)
    {
        ret = sendto(g_srvcsockfd, ilm, size, 0, (const struct sockaddr*)&data_addr, data_addrlen);
        if(ret < 0)
        {
            bt_ext_err("[A2DP] data send msg to a2dp data fail : %s, %d", strerror(errno), errno);
        }
        else
        {
            bt_ext_log("[A2DP] data send msg to to a2dp data success : %d", ret);
        }
    }
    else
    {
        bt_ext_err("[A2DP] g_srvcsockfd uninitialized");
    }

    return;
}

void btmtk_a2dp_fm_media_start_failed(void)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->result = BT_STATUS_FAILED;
    ((bt_a2dp_stream_data_start_cnf_struct*)ilmptr->ilm_data)->prefer_size = bt_a2dp_app.payload_size;
    btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_START_CNF, ilmptr, sizeof(bt_a2dp_stream_data_start_cnf_struct));
}

#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER

int btmtk_a2dp_fm_send_start_req(void)
{
    bt_ext_log("[A2DP] btmtk_a2dp_fm_send_start_req");

	if(bt_a2dp_app.fmOverBtMode == TRUE)
    {
	 	ALOGE("[A2DP] FM IS STARTING");
		return START_FAILED;
	}
    
	if(bt_a2dp_app.a2dp_app_dev[0].is_opened_by_peer_dev == KAL_TRUE ||
       bt_a2dp_app.a2dp_app_dev[0].current_message == MSG_ID_BT_A2DP_STREAM_START_REQ)
    {
		bt_ext_log("[A2DP] postponed to send fm start req");
		bt_a2dp_app.fmOverBtPostponedStart = KAL_TRUE; // wait for start ind and sending the second FM start request.
		return START_SUCCESS;
	}

    bt_a2dp_app.fmOverBtMode = TRUE;

    /* FM enabled only when MAX_A2DP_DEV_NUM == 1 */
	if(bt_a2dp_app.a2dp_app_dev[0].state == BT_A2DP_APP_STATE_STREAMING &&
       bt_a2dp_app.a2dp_app_dev[0].current_message == 0)
    {
        bt_ext_log("[A2DP] BT_A2DP_APP_STATE_STREAMING, send stop req~");
        
		// BT controller need stop audio then restart to listen fm data, and this procedure don't need a2dp data to know.
        btmtk_a2dp_send_stream_pause_req(MOD_BT, 0);
        bt_a2dp_app.fmStartContinue = KAL_TRUE;
        
        return START_SUCCESS;

	}else if (bt_a2dp_app.a2dp_app_dev[0].current_message == MSG_ID_BT_A2DP_STREAM_PAUSE_REQ)
	{
        bt_ext_log("[A2DP] BT_A2DP_APP_STATE_STREAMING, sended stop req~");
        bt_a2dp_app.fmStartContinue = KAL_TRUE;
        
        return START_SUCCESS;
	}
    
	int reconfig_result = a2dp_fm_reconfig_procedure();
    
	if(reconfig_result == RECONFIG_SUCCESS)
    {
        btmtk_a2dp_send_stream_reconfig_req( MOD_BT,
                                            0,
                                            &bt_a2dp_app.local_audio_cap);
        
        bt_a2dp_app.fmStartContinue = KAL_TRUE;

        return START_SUCCESS;
	}
    else if(reconfig_result == RECONFIG_NO_CHANGE)
    {
		bt_ext_log("[A2DP] RECONFIG_NO_CHANGE");
	}
	else
    {
		return RECONFIG_ERROR;
	}

    btmtk_a2dp_fm_controller_start_req(MOD_BT, 0);

	return START_SUCCESS;
}


void btmtk_a2dp_fm_send_stop_req(void)
{

	bt_ext_log("[A2DP] btmtk_a2dp_fm_send_stop_req");
    
	if(bt_a2dp_app.fmOverBtMode == FALSE)
    {
		ALOGE("[A2DP] FM IS STOPPED");
		return;
	}
    bt_a2dp_app.fmSuspending = KAL_TRUE;
    
    if (bt_a2dp_app.a2dp_app_dev[0].state == BT_A2DP_APP_STATE_STREAMING)
    {
	    btmtk_a2dp_fm_controller_stop_req(MOD_BT, 0);
        return;
    }
    
	if(a2dp_fm_reconfig_procedure()== RECONFIG_SUCCESS)
    {
		btmtk_a2dp_send_stream_reconfig_req( MOD_BT,
                                            0,
                                            &bt_a2dp_app.local_audio_cap);
	}
    else
	{
		return ;
	}
    
}
	     
int btmtk_a2dp_fm_media_reconfig(void)
{
    
	bt_ext_log("[A2DP] btmtk_a2dp_fm_media_reconfig, fmOverBtMode:%d, fmSuspending:%d", 
        bt_a2dp_app.fmOverBtMode, bt_a2dp_app.fmSuspending);
    
	if( bt_a2dp_app.fmOverBtMode && bt_a2dp_app.fmSuspending == KAL_FALSE)
    {
		if(bt_a2dp_app.local_audio_cap.codec_cap.sbc.sample_rate == 0x1 )
        {
			bt_ext_log("[A2DP] return no change 1");
			return RECONFIG_NO_CHANGE;
		}
        else
		{
		    bt_ext_log("[A2DP] reconfig sample_rate into 48K");
			bt_a2dp_app.local_audio_cap.codec_cap.sbc.min_bit_pool = 19;   
		    bt_a2dp_app.local_audio_cap.codec_cap.sbc.max_bit_pool = 53;   
		    bt_a2dp_app.local_audio_cap.codec_cap.sbc.block_len = 0x01;   
		    bt_a2dp_app.local_audio_cap.codec_cap.sbc.subband_num = 0x01;   
		    bt_a2dp_app.local_audio_cap.codec_cap.sbc.alloc_method = 0x01; 
			bt_a2dp_app.local_audio_cap.codec_cap.sbc.sample_rate = 0x01; 
			bt_a2dp_app.local_audio_cap.codec_cap.sbc.channel_mode = 0x01; 
		}
	}
    
	if((bt_a2dp_app.fmOverBtMode && bt_a2dp_app.fmSuspending == KAL_TRUE)||
        (!bt_a2dp_app.fmOverBtMode))
	{
		if(bt_a2dp_app.local_audio_cap.codec_cap.sbc.sample_rate == 0x2)
        {
            bt_ext_log("[A2DP] return no change 2");
            return RECONFIG_NO_CHANGE;
        }
        else
        {
		    bt_ext_log("[A2DP] reconfig sample_rate into 44.1K");
			bt_a2dp_app.local_audio_cap.codec_cap.sbc.min_bit_pool = 19;   
		    bt_a2dp_app.local_audio_cap.codec_cap.sbc.max_bit_pool = 75;   
		    bt_a2dp_app.local_audio_cap.codec_cap.sbc.block_len = 0x01;   
		    bt_a2dp_app.local_audio_cap.codec_cap.sbc.subband_num = 0x01;   
		    bt_a2dp_app.local_audio_cap.codec_cap.sbc.alloc_method = 0x01;   
		    bt_a2dp_app.local_audio_cap.codec_cap.sbc.sample_rate = 0x02;    
		    bt_a2dp_app.local_audio_cap.codec_cap.sbc.channel_mode = 0x01; 
        }
		
    }

	return RECONFIG_SUCCESS;
    
}

U8 a2dp_fm_reconfig_procedure()
{
    bt_ext_log("[A2DP] a2dp_fm_reconfig_procedure");
    
    U8 result = 0;
    
    result = btmtk_a2dp_fm_media_reconfig();
    
    return result;
}

void a2dp_off_fmoverbtmode(void)
{
	if(bt_a2dp_app.fmOverBtMode == TRUE)
    {
		bt_a2dp_app.fmOverBtMode = FALSE;
	}
}

#endif



