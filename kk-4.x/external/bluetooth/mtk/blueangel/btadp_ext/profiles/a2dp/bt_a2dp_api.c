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
 * Bt_a2dp_api.c
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
#include "string.h"
#include "bt_types.h"
#include "bt_message.h"
#include "bt_mmi.h"
#include "bluetooth_struct.h"
#include "bluetooth_a2dp_struct.h"
#include "bluetooth_a2dp_message.h"
#include "bt_mmi_msg.h"
#include "bt_a2dp_hdl.h"
#include "bt_a2dp_api.h"
#ifdef MTKA2DP_WISE
#include "MBTType.h"
#include "MBTSdc.h"
#include "bt_common.h"
#include "mbt_debugmsg.h"
#endif
#ifdef BTMTK_ON_LINUX
#include <errno.h>
#include <sys/socket.h>
#include "bt_ext_debug.h"
#endif
#include "bt_ext_apis.h"
#include "bluetooth_avrcp_struct.h" //for a2dp_avrcp_key
#include "bluetooth_avrcp_message.h"

/*******************************************************************************
*   Due to current there is no BT common headser files, I define some common BT
*   used types here
********************************************************************************/
typedef I8 BtStatus;

#define BT_STATUS_SUCCESS 0
#define BT_STATUS_FAILED  1
#define BT_STATUS_PENDING 2

#define bt_trace(...) ((void)0)


void btmtk_a2dp_convert_addr_from_UI(bt_addr_struct *addr_target, U8 *addr_src);
extern void btmtk_a2dp_send_msg_2_audio_module(msg_type msg_id, ilm_struct * ilm, U16 size);
extern kal_bool btmtk_a2dp_app_is_last_connection(void);

extern kal_bool btmtk_a2dp_app_check_current_process_msg(U8 stream_handle, U16 msg_id);
extern void btmtk_a2dp_app_deactive(void);
extern U8 btmtk_a2dp_find_stream_handle_by_addr(bt_addr_struct *addr);



static int g_sockfd;
int g_srvcsockfd;
static struct sockaddr_un g_ext_addr;
static socklen_t   g_ext_addrlen;





#ifdef BTMTK_ON_LINUX

void btmtk_a2dp_setSockAddress(struct sockaddr_un *addr, socklen_t addrlen)
{
    if(addr && addrlen)
   {
        bt_ext_log("[A2DP] btmtk_a2dp_setSockAddress %s", &addr->sun_path[1]);
        memcpy(&g_ext_addr, addr, sizeof(*addr));
        g_ext_addrlen = addrlen;
    }
}

BtStatus A2DP_SendMessage(msg_type msg_id, module_type mod_id, ilm_struct * ilm, U16 size)
{
    int ret;
    int sockfd;
    U16 *field;
	
    bt_ext_log("[A2DP] msg=%u, ptr=0x%X, size=%d", msg_id, (unsigned int)ilm, size);

    ret = -1;
    /* Set src_mod to MOD_MMI */
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = mod_id;
    ilm->sap_id = 0;
    ilm->msg_id = msg_id;

    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm->ilm_data;
    *field = 1;
    *(++field) = size;
    //field = (U16 *)&ilm->ilm_data[2];
    //*field = size;

    // cal whole ilm size
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;

    if(mod_id == MOD_BT)
    {
        btmtk_sendmsg(PROFILE_A2DP, ilm, size);
    }
    else
    {   // MOD_MMI
        if(g_srvcsockfd >= 0)
        {
            // ret = send(g_srvcsockfd, ilm, size, 0);
            ret = sendto(g_srvcsockfd, ilm, size, 0, (const struct sockaddr*)&g_ext_addr, g_ext_addrlen);
            if(ret < 0)
            {
                bt_ext_err("[A2DP] send msg fail : %s, %d", strerror(errno), errno);
            }
            else
            {
                bt_ext_log("[A2DP] send msg to g_srvcsockfd success : %d", ret);
            }
        }
        else
        {
            bt_ext_err("[A2DP] g_srvcsockfd uninitialized");
        }
    }
    return (ret >= 0) ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
}

#endif



/*******************************************************************
*   initialize functions
*******************************************************************/
void btmtk_a2dp_sep_default_setting_initialized(bt_a2dp_audio_cap_struct *audio_cap, int sep_num)
{
    int i = 0;
    // in android, only support SBC codec
    for (i = 0; i < sep_num; i++) 
    {
        audio_cap[i].codec_type = BT_A2DP_SBC;
        audio_cap[i].codec_cap.sbc.min_bit_pool = 19;   
        audio_cap[i].codec_cap.sbc.max_bit_pool = 75;   
        audio_cap[i].codec_cap.sbc.block_len = 0x01;   
        audio_cap[i].codec_cap.sbc.subband_num = 0x01;   
        audio_cap[i].codec_cap.sbc.alloc_method = 0x01;   
#if BTMTK_ON_LINUX
        // in android, the sample rate of stream source data always be 44100 for the output PCM data of andio flinger is 44.1
        audio_cap[i].codec_cap.sbc.sample_rate = 0x2;   
#else
        audio_cap[i].codec_cap.sbc.sample_rate = 0xf;   
#endif
        audio_cap[i].codec_cap.sbc.channel_mode = 0xf;   
     }

}


/*******************************************************************
*   Exported functions
*******************************************************************/
void btmtk_a2dp_send_activate_req(
        int srvcsock, 
        int sockfd)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    bt_a2dp_audio_cap_struct local_audio_cap[MAX_A2DP_DEV_NUM];
    ilmptr = &ilm;

    if (btmtk_a2dp_is_activate()) return;
    
    bt_trace(TRACE_FUNC, BTLOG_MBT_A2DP_SOURCE_ENABLED);
    g_srvcsockfd = srvcsock;
    g_sockfd = sockfd;
    
    btmtk_a2dp_sep_default_setting_initialized(local_audio_cap, MAX_A2DP_SEP_NUM);
    ((bt_a2dp_activate_req_struct*)ilmptr->ilm_data)->local_role = BT_A2DP_SOURCE;
    ((bt_a2dp_activate_req_struct*)ilmptr->ilm_data)->sep_num = MAX_A2DP_SEP_NUM;
    memcpy(&(((bt_a2dp_activate_req_struct*)ilmptr->ilm_data)->local_sep), local_audio_cap, MAX_A2DP_SEP_NUM * sizeof(bt_a2dp_audio_cap_struct));

    A2DP_SendMessage(MSG_ID_BT_A2DP_ACTIVATE_REQ, MOD_BT, ilmptr, sizeof(bt_a2dp_activate_req_struct));
}


void btmtk_a2dp_send_appi_bt_deactivate_request(void)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    if (!btmtk_a2dp_is_activate()) return;
    
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->command = BT_A2DP_APPI_COMMAND_DEACTIVATED;
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->size = 0;
    A2DP_SendMessage(MSG_ID_BT_A2DP_APPI_BT_COMMAND, MOD_MMI, ilmptr, sizeof(bt_a2dp_appi_bt_command_struct));
}

void btmtk_a2dp_send_deactivate_req(module_type src_mod_id)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    bt_trace(TRACE_FUNC, BTLOG_MBT_A2DP_SOURCE_DISABLED);
    A2DP_SendMessage(MSG_ID_BT_A2DP_DEACTIVATE_REQ, MOD_BT, ilmptr, sizeof(bt_a2dp_deactivate_req_struct));

}

void btmtk_a2dp_deactive_req(void)
{
    btmtk_a2dp_app_deactive();
}

void btmtk_a2dp_authorize_res(U8 accept, U8 stream_handle)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    ((bt_a2dp_connect_accept_res_struct*)ilmptr->ilm_data)->accept = accept;
    ((bt_a2dp_connect_accept_res_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;
    A2DP_SendMessage(MSG_ID_BT_A2DP_APPI_BT_CONNECT_ACCEPT_RSP, MOD_BT, ilmptr, sizeof(bt_a2dp_connect_accept_res_struct));

}

// TODO:  need to notify UI when reach the max connection request.
void btmtk_a2dp_send_stream_open_req(module_type src_mod_id, bt_addr_struct *device_addr, U8 local_role)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    bt_ext_log("[A2DP] send open request to addr: (%02x:%02x:%02x:%02x:%02x:%02x)",
    	        device_addr[0],device_addr[1],device_addr[2],
                device_addr[3],device_addr[4],device_addr[5]);

    ((bt_a2dp_signal_connect_req_struct*)ilmptr->ilm_data)->local_role = local_role;
    memcpy(&(((bt_a2dp_stream_open_req_struct*)ilmptr->ilm_data)->device_addr), device_addr, sizeof(bt_addr_struct));
    A2DP_SendMessage(MSG_ID_BT_A2DP_STREAM_OPEN_REQ, MOD_BT, ilmptr, sizeof(bt_a2dp_signal_connect_req_struct));
    btmtk_a2dp_inform_connection();
}


void btmtk_a2dp_send_stream_open_res(module_type src_mod_id, U8 stream_handle, U16 accept)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    ((bt_a2dp_stream_open_res_struct*)ilmptr->ilm_data)->accept = accept;
    ((bt_a2dp_stream_open_res_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;
    A2DP_SendMessage(MSG_ID_BT_A2DP_STREAM_OPEN_RES, MOD_BT, ilmptr, sizeof(bt_a2dp_signal_connect_res_struct));
}

void btmtk_a2dp_close_device(bt_addr_struct *addr)
{
    U8 stream_hdl = UNKNOWN_STREAM_HANDLE;

    stream_hdl = btmtk_a2dp_find_stream_handle_by_addr(addr);
    bt_ext_log("[A2DP] close device, stream hdl: %d", stream_hdl);
    
    if (stream_hdl != UNKNOWN_STREAM_HANDLE)
    {
        if (stream_hdl >= MAX_A2DP_DEV_NUM) {
            bt_ext_log("[A2DP] close device failed, stream handler error.");
            return;
        }
        btmtk_a2dp_send_stream_close_req(MOD_BT, stream_hdl);
    }
    else
        bt_ext_log("[A2DP] have not found the stream handler of this addr: %x%x%x",
        addr->lap, addr->nap, addr->uap);
}

void btmtk_a2dp_send_stream_close_req(module_type src_mod_id, U8 stream_handle)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    if (!btmtk_a2dp_app_check_current_process_msg(stream_handle, MSG_ID_BT_A2DP_STREAM_CLOSE_REQ))
    {
        bt_ext_log("[A2DP] send close request fail.");
        return;
    }
    
    ((bt_a2dp_stream_close_req_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;

    A2DP_SendMessage(MSG_ID_BT_A2DP_STREAM_CLOSE_REQ, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_close_req_struct));

    // send disconnecting_ind to a2dp stream service
    // TODO: should judge whether this is the last connection

    if (btmtk_a2dp_app_is_last_connection())
    {
        ilm_struct ilm;
        ilm_struct *ilmptr;
        ilmptr = &ilm;

        btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_DISCONNECTING_IND, ilmptr, sizeof(bt_a2dp_stream_data_disconnecting_ind_struct));

        #ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER
            a2dp_off_fmoverbtmode();
        #endif
    }
}

void btmtk_a2dp_send_stream_close_res(module_type src_mod_id, U16 result, U8 stream_handle)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    ((bt_a2dp_stream_close_res_struct*)ilmptr->ilm_data)->result = result;
    ((bt_a2dp_stream_close_res_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;

    A2DP_SendMessage(MSG_ID_BT_A2DP_STREAM_CLOSE_RES, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_close_res_struct));
}


void btmtk_a2dp_send_stream_reconfig_req(
        module_type src_mod_id,
        U8 stream_handle,
        bt_a2dp_audio_cap_struct *audio_cap)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    if (!btmtk_a2dp_app_check_current_process_msg(stream_handle, MSG_ID_BT_A2DP_STREAM_RECONFIG_REQ))
    {
        bt_ext_log("[A2DP] send reconfig request fail.");
        return;
    }
    
    ((bt_a2dp_stream_reconfig_req_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;
    memcpy(&(((bt_a2dp_stream_reconfig_req_struct*)ilmptr->ilm_data)->audio_cap), audio_cap, sizeof(bt_a2dp_audio_cap_struct));

    A2DP_SendMessage(MSG_ID_BT_A2DP_STREAM_RECONFIG_REQ, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_reconfig_req_struct));

}


void btmtk_a2dp_send_stream_reconfig_res(module_type src_mod_id, U16 result, U8 stream_handle)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    ((bt_a2dp_stream_reconfig_res_struct*)ilmptr->ilm_data)->result = result;
    ((bt_a2dp_stream_reconfig_res_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;

    A2DP_SendMessage(MSG_ID_BT_A2DP_STREAM_RECONFIG_RES, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_reconfig_res_struct));
}


void btmtk_a2dp_send_stream_start_req(module_type src_mod_id, U8 stream_handle)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    bt_ext_log("[A2DP] send start request msg id:%d.", MSG_ID_BT_A2DP_STREAM_START_REQ);
    if (!btmtk_a2dp_app_check_current_process_msg(stream_handle, MSG_ID_BT_A2DP_STREAM_START_REQ))
    {
        bt_ext_log("[A2DP] send start request fail.");
        return;
    }
    
    ((bt_a2dp_stream_start_req_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;

    A2DP_SendMessage(MSG_ID_BT_A2DP_STREAM_START_REQ, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_start_req_struct));

}


void btmtk_a2dp_send_stream_start_res(module_type src_mod_id, U16 result, U8 stream_handle)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    ((bt_a2dp_stream_start_res_struct*)ilmptr->ilm_data)->result = result;
    ((bt_a2dp_stream_start_res_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;

    A2DP_SendMessage(MSG_ID_BT_A2DP_STREAM_START_RES, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_start_res_struct));
}

void btmtk_a2dp_send_stream_pause_req(module_type src_mod_id, U8 stream_handle)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    if (!btmtk_a2dp_app_check_current_process_msg(stream_handle, MSG_ID_BT_A2DP_STREAM_PAUSE_REQ))
    {
        bt_ext_log("[A2DP] send pause request fail.");
        return;
    }
    
    ((bt_a2dp_stream_pause_req_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;
    A2DP_SendMessage(MSG_ID_BT_A2DP_STREAM_PAUSE_REQ, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_pause_req_struct));

}

void btmtk_a2dp_send_stream_pause_res(module_type src_mod_id, U16 result, U8 stream_handle)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    
    ((bt_a2dp_stream_pause_res_struct*)ilmptr->ilm_data)->result = result;
    ((bt_a2dp_stream_pause_res_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;
    A2DP_SendMessage(MSG_ID_BT_A2DP_STREAM_PAUSE_RES, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_pause_res_struct));
}


void btmtk_a2dp_send_stream_abort_req(module_type src_mod_id, U8 stream_handle)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    if (!btmtk_a2dp_app_check_current_process_msg(stream_handle, MSG_ID_BT_A2DP_STREAM_ABORT_REQ))
    {
        bt_ext_log("[A2DP] send abort request fail.");
        return;
    }
    
    ((bt_a2dp_stream_abort_req_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;

    A2DP_SendMessage(MSG_ID_BT_A2DP_STREAM_ABORT_REQ, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_abort_req_struct));

}

void btmtk_a2dp_send_stream_abort_res(module_type src_mod_id, U8 stream_handle)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    
    ((bt_a2dp_stream_abort_res_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;

    A2DP_SendMessage(MSG_ID_BT_A2DP_STREAM_ABORT_RES, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_abort_res_struct));
}

void btmtk_A2dp_send_appi_bt_disconnect_request (U8* addr)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    bt_trace(TRACE_FUNC, BTLOG_MBT_A2DP_SOURCE_DISCONNECT,
                addr[0],addr[1],addr[2],
                addr[3],addr[4],addr[5]);

    bt_ext_log("[A2DP] send_appi_bt_disconnect_request (%02x:%02x:%02x:%02x:%02x:%02x)",
    	                addr[0],addr[1],addr[2], addr[3],addr[4],addr[5]);


    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->command = BT_A2DP_APPI_COMMAND_DISCONNECT;
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->size = 6;
    memcpy(((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->data, addr, 6);
    A2DP_SendMessage(MSG_ID_BT_A2DP_APPI_BT_COMMAND, MOD_MMI, ilmptr, sizeof(bt_a2dp_appi_bt_command_struct));
}

void btmtk_A2dp_send_appi_bt_start_request (void)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    bt_ext_log("[A2DP] btmtk_A2dp_send_appi_bt_start_request");

    bt_trace(TRACE_FUNC, BTLOG_MBT_A2DP_SOURCE_START);

    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->command = BT_A2DP_APPI_COMMAND_START;
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->size = 0;
    A2DP_SendMessage(MSG_ID_BT_A2DP_APPI_BT_COMMAND, MOD_MMI, ilmptr, sizeof(bt_a2dp_appi_bt_command_struct));
}

void btmtk_A2dp_send_appi_bt_stop_request (void)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    bt_ext_log("[A2DP] btmtk_A2dp_send_appi_bt_stop_request");
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->command = BT_A2DP_APPI_COMMAND_STOP;
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->size = 0;
    bt_trace(TRACE_FUNC, BTLOG_MBT_A2DP_SOURCE_STOP);
    A2DP_SendMessage(MSG_ID_BT_A2DP_APPI_BT_COMMAND, MOD_MMI, ilmptr, sizeof(bt_a2dp_appi_bt_command_struct));
}


void btmtk_A2dp_send_appi_bt_pause_request (void)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    
    bt_trace(TRACE_FUNC, BTLOG_MBT_A2DP_SOURCE_PAUSE);
    
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->command = BT_A2DP_APPI_COMMAND_PAUSE;
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->size = 0;
    A2DP_SendMessage(MSG_ID_BT_A2DP_APPI_BT_COMMAND, MOD_MMI, ilmptr, sizeof(bt_a2dp_appi_bt_command_struct));
}


void btmtk_A2dp_send_appi_bt_resume_request (void)
{
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;

    bt_trace(TRACE_FUNC, BTLOG_MBT_A2DP_SOURCE_RESUME);

    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->command = BT_A2DP_APPI_COMMAND_RESUME;
    ((bt_a2dp_appi_bt_command_struct*)ilmptr->ilm_data)->size = 0;

    A2DP_SendMessage(MSG_ID_BT_A2DP_APPI_BT_COMMAND, MOD_MMI, ilmptr, sizeof(bt_a2dp_appi_bt_command_struct));
}


#ifdef MTK_BT_FM_OVER_BT_VIA_CONTROLLER
void btmtk_a2dp_fm_controller_start_req(module_type src_mod_id, U8 stream_handle){
	ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    
    bt_ext_log("[A2DP] btmtk_a2dp_fm_controller_start_req");

    if (!btmtk_a2dp_app_check_current_process_msg(stream_handle, MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_START_REQ))
    {
        bt_ext_log("[A2DP] send fm start request fail.");
        return;
    }
    
    ((bt_a2dp_stream_start_req_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;

    A2DP_SendMessage(MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_START_REQ, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_start_req_struct));
}	

void btmtk_a2dp_fm_controller_suspend_req(module_type src_mod_id, U8 stream_handle){
	ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    
    bt_ext_log("[A2DP] btmtk_a2dp_fm_controller_suspend_req");

    if (!btmtk_a2dp_app_check_current_process_msg(stream_handle, MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_SUSPEND_REQ))
    {
        bt_ext_log("[A2DP] send reconfig request fail.");
        return;
    }
    
    ((bt_a2dp_stream_start_req_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;

    A2DP_SendMessage(MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_SUSPEND_REQ, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_start_req_struct));
}
/*

void btmtk_a2dp_fm_controller_resume_req(module_type src_mod_id, U8 stream_handle){
	ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    bt_ext_log("[A2DP] btmtk_a2dp_fm_controller_resume_req");
    ((bt_a2dp_stream_start_req_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;

    A2DP_SendMessage(MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_RESUME_REQ, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_start_req_struct));
}
*/

void btmtk_a2dp_fm_controller_stop_req(module_type src_mod_id, U8 stream_handle){
	ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    
    bt_ext_log("[A2DP] btmtk_a2dp_fm_controller_stop_req");

    if (!btmtk_a2dp_app_check_current_process_msg(stream_handle, MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_STOP_REQ))
    {
        bt_ext_log("[A2DP] send reconfig request fail.");
        return;
    }
    
    ((bt_a2dp_stream_start_req_struct*)ilmptr->ilm_data)->stream_handle = stream_handle;

    A2DP_SendMessage(MSG_ID_BT_A2DP_FM_VIA_CONTROLLER_STOP_REQ, MOD_BT, ilmptr, sizeof(bt_a2dp_stream_start_req_struct));
}
#endif


void btmtk_a2dp_convert_addr_from_UI(bt_addr_struct *addr_target, U8 *addr_src)
{
    addr_target->lap = (U32)(addr_src[5]) | ((U32)addr_src[4] << 8) | ((U32)addr_src[3] << 16);
    addr_target->uap = addr_src[2];
    addr_target->nap = (U16)(addr_src[1]) | ((U16)addr_src[0] << 8);
}

/**
 * @brief send avrcp key
 * This is to send avrcp key as a AVRCP Controller
 * Use keyevent  AVRCP_POP_VOLUME_UP(0x41) to send vol-up
 * Use keyevent  AVRCP_POP_VOLUME_DOWN(0x42) to send vol-down
 */
void btmtk_a2dp_avrcp_send_cmd_key_req(U32 keyevent){
	// compose a avrcp key req
    ilm_struct ilm;
    ilm_struct *ilmptr;
    	
    bt_avrcp_cmd_frame_req_struct msg;
    U8 press = 0;
    U8 *frame;

    ilmptr = &ilm;
    frame = &msg.frame_data[0];
    bt_ext_log("[A2DP_avrcp] btmtk_a2dp_avrcp_send_cmd_key_req keyevent:%d", (int)keyevent);
	
	// press 
    press = 0;
    frame[0] = 0x7C;
    frame[1] = ( press << 7) | keyevent;
    frame[2] = 0x00;

    msg.chnl_num = 0;
    msg.c_type = BT_AVRCP_CR_CONTROL;
	msg.subunit_type = BT_AVRCP_SUBUNIT_TYPE_PASS_THROUGH;
	msg.subunit_id = BT_AVRCP_SUBUNIT_ID_PASS_THROUGH;
	msg.data_len = 3;

	memcpy( ilmptr->ilm_data, &msg, sizeof(bt_avrcp_cmd_frame_req_struct) );
    A2DP_SendMessage( MSG_ID_BT_AVRCP_CMD_FRAME_REQ, MOD_BT, ilmptr, sizeof(bt_avrcp_cmd_frame_req_struct));

	press = 1;
	frame[1] = ( press << 7) | keyevent;
	
	memcpy( ilmptr->ilm_data, &msg, sizeof(bt_avrcp_cmd_frame_req_struct) );
    A2DP_SendMessage( MSG_ID_BT_AVRCP_CMD_FRAME_REQ, MOD_BT, ilmptr, sizeof(bt_avrcp_cmd_frame_req_struct));
}

void btmtk_a2dp_send_wifi_connect_req(){
    //-----------------------------------------------------------------
    //       send wifi req to a2dp stream to decrease throughput
    //-----------------------------------------------------------------
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    
    btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_WIFI_CONNECTED, ilmptr, sizeof(bt_a2dp_stream_data_disconnecting_ind_struct));

}

void btmtk_a2dp_send_wifi_disconnect_req(){
	//-----------------------------------------------------------------
    //       send wifi req to a2dp stream to decrease throughput
    //-----------------------------------------------------------------
    ilm_struct ilm;
    ilm_struct *ilmptr;
    ilmptr = &ilm;
    
    btmtk_a2dp_send_msg_2_audio_module(MSG_ID_BT_A2DP_STREAM_DATA_WIFI_DISCONNECTED, ilmptr, sizeof(bt_a2dp_stream_data_disconnecting_ind_struct));
}
