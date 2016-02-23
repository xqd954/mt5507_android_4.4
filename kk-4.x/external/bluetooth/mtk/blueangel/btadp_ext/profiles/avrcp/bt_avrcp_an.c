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
* bt_avrcp_api.c
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
* 
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
#if 0 
// kk has been easy-migration and don't need it
#ifdef BTMTK_ON_LINUX
#define LOG_TAG "EXTADP_AVRCP"

#include "string.h"
#include "bt_types.h"
#include "bt_os_api.h"
#include "bt_message.h"
#include "bt_mmi.h"
#include "bt_mmi_msg.h"
#include "bluetooth_avrcp_struct.h"
#include "bluetooth_avrcp_message.h"
#include "bt_avrcp_api.h"
#include "bt_avrcp_struct.h"
#include "bt_ext_debug.h"
#include "bt_config.h"
#include <errno.h>
#include <sys/socket.h>

#define ext_os_memalloc(n)				malloc(n)
#define ext_os_memfree(p)				free(p)
#define ext_os_memset(dest, c, n)		memset(dest, c, n)
#define ext_os_memcpy(dest, src, n)	    memcpy(dest, src, n)
#define ext_os_memcmp(p1, p2, n)		memcmp(p1, p2, n)
#define ext_os_strlen(s)				strlen(s)
#define ext_os_strncpy(dest, src, n)	strncpy(dest, src, n)
#define ext_os_sprintf					sprintf

// classifier for exported API   EXPort AVrcp API
#define EXP_AVAPI 

#if 0
#define BT_AVRCP_AT()   bt_ext_log("[AVRCP] %s %d", __func__, __LINE__ )
#else
#define BT_AVRCP_AT()   
#endif

// gloabl conext 
static AvrcpChannelContext g_avrpcAdpCntx;

// copy from bt_pbap_api.c
int BT_SendMessage(void *cntx, U16 msg_id, void *ptr, U16 size)
{
    int ret;
    ilm_struct *ilm;
    U16 *field;
    int sockfd;
    
    sockfd = ((AvrcpChannelContext *)cntx)->apisock;
	
    bt_ext_log("msg=%u, ptr=0x%X, size=%d", msg_id, (unsigned int)ptr, size);

    ret = -1;
    ilm = (ilm_struct*)ptr;
    /* Set src_mod to MOD_MMI */
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = MOD_BT;
    ilm->sap_id = 0;
    ilm->msg_id = (msg_type) msg_id;

    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm->ilm_data;
    *field = 1;
    field = (U16 *)&ilm->ilm_data[2];
    *field = size;

    // cal whole ilm size
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
    
    if(sockfd)
    {
        ret = send(sockfd, ptr, size, 0);
        if(ret < 0)
        {
            bt_ext_err("send msg fail : %s, %d", strerror(errno), errno);
        }
        else
        {
            bt_ext_log("send msg success : %d", ret);
        }
    }
    else
    {
        bt_ext_err("socket uninitialized");
    }
    return (ret >= 0) ? 0 : -1;
}

EXP_AVAPI U8 AVRCP_SendMessage(void *cntx, U16 id, void *data, U32 len){
	ilm_struct ilm;	
    int ret;	
    
    bt_ext_log("AVRCP_SendMessage len=%d", (int)len);
    
	memcpy( ilm.ilm_data, data, len);

    ret = BT_SendMessage(cntx, id, &ilm, len);
    if( ret >= 0)
    {
        return 0 ;
    }
	bt_ext_err("AVRCP_SendMessage failed");
	
	///TODO: should be assert here!
	return 1;
}


/************************************************************************/
EXP_AVAPI BT_BOOL btmkt_avrcp_is_activate()
{
	if( 0x3C == g_avrpcAdpCntx.enable ){
		return TRUE;
	}
	return FALSE;
}

EXP_AVAPI BT_BOOL btmkt_avrcp_init_context()
{
	if( g_avrpcAdpCntx.enable == 0x3C ){
		// show 
		bt_ext_err("btmkt_avrcp_init_context has init");
	}
	ext_os_memset( &g_avrpcAdpCntx, 0, sizeof(AvrcpChannelContext) );
	g_avrpcAdpCntx.enable = 0x3C;
	
	return TRUE;
}

/************************************************************************/

EXP_AVAPI void btmtk_avrcp_send_activate_req(AvrcpChannelContext *cntx, U8 chnl_num, U8 local_role, U8 version, U8 sdpfeature )
{
   	bt_avrcp_activate_req_struct msg;
    int ret;	

    ext_os_memset( (U8 *) &msg, 0, sizeof(msg));

    bt_ext_log("btmtk_avrcp_send_activate_req 1");
    msg.chnl_num = chnl_num;
    msg.local_role = local_role;
    msg.version = version;
    msg.sdpfeature = sdpfeature;

    bt_ext_log("AVRCP chnl_num %d version %d sdpfeature %d", chnl_num, version, sdpfeature);
     
    ret = AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_ACTIVATE_REQ, &msg, sizeof(bt_avrcp_activate_req_struct));
    
    if( ret >= 0)
    {
    	  bt_ext_log("avrcp MSG range(%d~%d)", MSG_ID_BT_AVRCP_ACTIVATE_REQ, MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_CNF);
    	  bt_ext_log("avrcp MSG_ID_BT_AVRCP_ACTIVATE_REQ=%d", MSG_ID_BT_AVRCP_ACTIVATE_REQ);
    	  bt_ext_log("avrcp MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_RSP=%d", MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_RSP);
          return;
    }
    bt_ext_err("btmtk_avrcp_send_activate_req failed");
}

EXP_AVAPI void btmtk_avrcp_send_deactivate_req(AvrcpChannelContext *cntx, U8 chnl_num )
{
	bt_avrcp_deactivate_req_struct msg;
    
	ext_os_memset( (U8 *) &msg, 0, sizeof(msg));

	msg.chnl_num = chnl_num;
    AVRCP_SendMessage( cntx,MSG_ID_BT_AVRCP_DEACTIVATE_REQ, &msg, sizeof(bt_avrcp_deactivate_req_struct));
}


EXP_AVAPI void btmtk_avrcp_send_connect_req(AvrcpChannelContext *cntx, U8 chnl_num, U8 *addr, U8 local_role )
{
	bt_avrcp_connect_req_struct msg;

	ext_os_memset( (U8 *) &msg, 0, sizeof(msg));

	msg.chnl_num = chnl_num;
	msg.local_role = local_role;
	memcpy( (void *)&msg.device_addr, (const void *)addr, sizeof(msg.device_addr));

    AVRCP_SendMessage( cntx, MSG_ID_BT_AVRCP_CONNECT_REQ, &msg, sizeof(bt_avrcp_connect_req_struct));
}

EXP_AVAPI void btmtk_avrcp_send_browse_connect_req(AvrcpChannelContext *cntx, U8 chnl_num, U8 local_role )
{
	bt_avrcp_browse_connect_req_struct msg;

	ext_os_memset( (U8 *) &msg, 0, sizeof(msg));

	msg.chnl_num = chnl_num;
	msg.local_role = local_role;

    AVRCP_SendMessage( cntx, MSG_ID_BT_AVRCP_BROWSE_CONNECT_REQ, &msg, sizeof(bt_avrcp_browse_connect_req_struct));
}

EXP_AVAPI void btmtk_avrcp_send_browse_disconnect_req(AvrcpChannelContext *cntx, U8 chnl_num)
{
	bt_avrcp_browse_disconnect_req_struct msg;
	ext_os_memset( (U8 *) &msg, 0, sizeof(msg));

	msg.chnl_num = chnl_num;
    AVRCP_SendMessage( cntx ,MSG_ID_BT_AVRCP_BROWSE_DISCONNECT_REQ, &msg, sizeof(bt_avrcp_disconnect_req_struct));
}

EXP_AVAPI void btmtk_avrcp_send_connect_ind_rsp(AvrcpChannelContext *cntx, U8 chnl_num, BT_BOOL accept)
{
	bt_avrcp_connect_ind_rsp_struct msg;
	ext_os_memset( (U8 *) &msg, 0, sizeof(msg));

	msg.chnl_num = chnl_num;
	msg.accept_conn = accept;
    AVRCP_SendMessage( cntx,MSG_ID_BT_AVRCP_CONNECT_IND_RSP, &msg, sizeof(bt_avrcp_connect_ind_rsp_struct));
}


EXP_AVAPI void btmtk_avrcp_send_disconnect_req(AvrcpChannelContext *cntx, U8 chnl_num)
{
	bt_avrcp_disconnect_req_struct msg;
	ext_os_memset( (U8 *) &msg, 0, sizeof(msg));

	msg.chnl_num = chnl_num;
    AVRCP_SendMessage( cntx ,MSG_ID_BT_AVRCP_DISCONNECT_REQ, &msg, sizeof(bt_avrcp_disconnect_req_struct));
}

EXP_AVAPI void btmtk_avrcp_send_cmd_key_req(AvrcpChannelContext *cntx, U8 *addr, U32 keyevent)
{
	bt_avrcp_cmd_frame_req_struct msg;
	U8 press = 0;
	U8 *frame;

	ext_os_memset( (U8 *) &msg, 0, sizeof(msg));

    memcpy(msg.frame_data, addr, 6);
	frame = &msg.frame_data[0];
	
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

	//TODO: send press key
    AVRCP_SendMessage( cntx, MSG_ID_BT_AVRCP_CMD_FRAME_REQ, &msg, sizeof(bt_avrcp_cmd_frame_req_struct));

	//TODO: queue this
	
	//TODO: release key
	press = 1;
	frame[1] = ( press << 7) | keyevent;
    AVRCP_SendMessage( cntx, MSG_ID_BT_AVRCP_CMD_FRAME_REQ,  &msg, sizeof(bt_avrcp_cmd_frame_req_struct));
}

#if 1

EXP_AVAPI void btmtk_avrcp_send_group_key_event_req(AvrcpChannelContext *cntx, U8 chnl_num, U16 KeyValue, U8 press)
{
	U8 frame[8];
	U8 opcode_id = 0;

	frame[0] = 0x7E;  //opcode
	frame[1] = ( press << 7) | opcode_id; //operation_id
	frame[2] = 0x00; // length
	frame[3] = 0x00;
	frame[4] = 0x19;
	frame[5] = 0x58;
	frame[6] = (U8) ((KeyValue & (0xff00) )>> 8);
	frame[7] = (U8) (KeyValue & (0x00ff) );
	btmtk_avrcp_send_cmd_frame_req( cntx,
								 chnl_num,
								 0x00,
								 BT_AVRCP_CR_CONTROL,
								 BT_AVRCP_SUBUNIT_TYPE_PASS_THROUGH,
								 BT_AVRCP_SUBUNIT_ID_PASS_THROUGH,
								 0x08,
								 (U8 *)frame);

}


/** 
 * @brief Old void btmtk_avrcp_wise_send_key_event_req
 *
 */
EXP_AVAPI void btmtk_avrcp_send_pass_through_key_req(AvrcpChannelContext *cntx, U8 chnl_num, U16 KeyValue)
{
    U8 press = 0;
    U8 frame[3];
    U8 opcode_id;

    switch(KeyValue)
    {
    	#if 0
        case MBT_AVRCP_PLAY_PRESSED_K:
            press = 0;
            opcode_id = BT_AVRCP_OPERATION_ID_PLAY;
            break;
        #endif
        default:            
            press = 1;
            opcode_id = BT_AVRCP_OPERATION_ID_VOLUME_DOWN;
            break;            
    }
    
    frame[0] = 0x7C;
    frame[1] = ( press << 7) | opcode_id;
    frame[2] = 0x00;
    btmtk_avrcp_send_cmd_frame_req(cntx,
                                   chnl_num,
                                   0x00,
                                   BT_AVRCP_CR_CONTROL,
                                   BT_AVRCP_SUBUNIT_TYPE_PASS_THROUGH,
                                   BT_AVRCP_SUBUNIT_ID_PASS_THROUGH,
                                   0x03,
                                   (U8 *)frame);

                                   
}


EXP_AVAPI  void btmtk_avrcp_send_cmd_frame_req(AvrcpChannelContext *cntx,
                                            U8 chnl_num, 
                                            U8 seq_id, 
                                            U8 c_type, 
                                            U8 subunit_type, 
                                            U8 subunit_id, 
                                            U16 data_len, 
                                            U8* frame_data)
{
	bt_avrcp_cmd_frame_req_struct msg;

	msg.chnl_num = chnl_num;
 	msg.seq_id = seq_id;
	msg.c_type = c_type;
	msg.subunit_type = subunit_type;
	msg.subunit_id = subunit_id;
	msg.data_len = data_len;

	if((data_len > 0) && (frame_data != NULL))
	{
        memcpy(msg.frame_data, frame_data, data_len);
	}
    AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_CMD_FRAME_REQ, &msg, sizeof(bt_avrcp_cmd_frame_req_struct));
}

/*
* @brief send key response
* use by btmtk_avrcp_send_pass_through_rsp
*/
void btmtk_avrcp_send_cmd_frame_ind_rsp( AvrcpChannelContext *cntx,
                                                U8 chnl_num, 
                                                U8 seq_id, 
                                                U16 profile_id,
                                                U8 c_type, 
                                                U8 subunit_type, 
                                                U8 subunit_id, 
                                                U16 data_len, 
                                                U8* frame_data)
{
	bt_avrcp_cmd_frame_ind_rsp_struct msg;

	msg.chnl_num = chnl_num;
	msg.seq_id = seq_id;
	msg.profile_id  = profile_id;	
	msg.c_type = c_type;
	msg.subunit_type = subunit_type;
	msg.subunit_id = subunit_id;
	msg.data_len = data_len;

	if((data_len > 0) && (frame_data != NULL))
	{
		ext_os_memcpy((U8 *)msg.frame_data, (U8 *)frame_data, data_len);
	}

    AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_CMD_FRAME_IND_RSP, &msg, sizeof(bt_avrcp_cmd_frame_ind_rsp_struct));    
}


EXP_AVAPI void btmtk_avrcp_send_pass_through_rsp( AvrcpChannelContext *cntx,
                                                U8 chnl_num, 
                                                U8 seq_id, 
                                                U16 profile_id,
                                                U8 c_type, 
                                                U8 subunit_type, 
                                                U8 subunit_id, 
                                                U16 data_len, 
                                                U8* frame_data)
{                                                
	bt_avrcp_cmd_frame_ind_rsp_struct msg;

	msg.chnl_num = chnl_num;
	msg.seq_id = seq_id;
	msg.profile_id  = profile_id;	
	msg.c_type = c_type;
	msg.subunit_type = subunit_type;
	msg.subunit_id = subunit_id;
	msg.data_len = data_len; 
	// Note: it should be := 3
	// msg1.frame_data[0] = 0x7c;
	// msg1.frame_data[1] = msg.frame_data[1] & 0x7f;
	// msg1.frame_data[2] = 0;
	
	if((data_len > 0) && (frame_data != NULL))
	{
		ext_os_memcpy((U8 *)msg.frame_data, (U8 *)frame_data, data_len);
	}

    AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_CMD_FRAME_IND_RSP, &msg, sizeof(bt_avrcp_cmd_frame_ind_rsp_struct));    
}
#endif




void btmtk_avrcp_authorize_res(AvrcpChannelContext *cntx, U8 accept)
{
    bt_avrcp_connect_accept_res msg;

    msg.accept = accept;
    AVRCP_SendMessage( cntx,MSG_ID_BT_AVRCP_CONNECT_ACCEPT_RSP, &msg, sizeof(bt_avrcp_connect_accept_res));
}


void btmtk_avrcp_convert_addr_from_UI(bt_avrcp_dev_addr_struct *addr_target, U8 *addr_src)
{
    addr_target->lap = (U32)(addr_src[5]) | ((U32)addr_src[4] << 8) | ((U32)addr_src[3] << 16);
    addr_target->uap = addr_src[2];
    addr_target->nap = (U16)(addr_src[1]) | ((U16)addr_src[0] << 8);
}



/********************************** AVRCP14 local function ***************************/

/* AV13 inter layer */
AvrcpEventInfo g_avrcpEventArray[16];
AvrcpEventInfo g_avrcpRequestArray[1];

void avrcp_util_record_latest_request(AvrcpChannelContext *cntx, U8 idx, U8 chnl_num, U8 transId){
	g_avrcpRequestArray[idx].chnl_num = chnl_num;
	g_avrcpRequestArray[idx].transId  = transId;
}

void avrcp_util_get_latest_request(U8 idx, U8 *chnl_num, U8 *transId){
	*chnl_num = g_avrcpRequestArray[idx].chnl_num;
	*transId = g_avrcpRequestArray[idx].transId;
}

void btmtk_avrcp_send_get_capabilities_rsp(AvrcpChannelContext *cntx, U8 error, U8 count, U8 *events){
	bt_avrcp_get_capabilities_rsp_struct msg;
	int i;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.capability_id = 0x03;
		msg.count = count;
		for(i = 0; i< count; i++ ){
			msg.capability[i] = events[i];
		}		
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	}
	
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_GET_CAPABILITIES_RSP, &msg, sizeof(bt_avrcp_get_capabilities_rsp_struct));
}

void btmtk_avrcp_send_list_player_attrs_rsp(AvrcpChannelContext *cntx, U8 error, U8 count, U8 *attr_ids){
	bt_avrcp_list_playerapp_attribute_rsp_struct msg;
	int i;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.count = count;
		for(i = 0; i< count; i++ ){
			msg.attribute_id[i] = attr_ids[i];
		}
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	}
	
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_LIST_PLAYERAPP_ATTRIBUTE_RSP, &msg, sizeof(bt_avrcp_list_playerapp_attribute_rsp_struct));
}

void btmtk_avrcp_send_list_player_values_rsp(AvrcpChannelContext *cntx, U8 error, U8 attr_id, U8 count, U8 *values){
	bt_avrcp_list_playerapp_value_rsp_struct msg;
	int i;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.attribute_id = attr_id;
		msg.count = count;
		for(i = 0; i< count; i++ ){
			msg.attribute_values[i] = values[i];
		}
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	}
	
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_LIST_PLAYERAPP_VALUE_RSP, &msg, sizeof(bt_avrcp_list_playerapp_value_rsp_struct));		
}

void btmtk_avrcp_send_get_curplayer_value_rsp(AvrcpChannelContext *cntx, U8 error, U8 count, U8 *attr_ids, U8 *values){
	bt_avrcp_get_curplayerapp_value_rsp_struct msg;
	int i;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;

		msg.count = count;
		for(i = 0; i< count; i++ ){
			msg.attribute_id[i] = attr_ids[i];
			msg.attribute_value[i] = values[i];
		}
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	}
	
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_RSP, &msg, sizeof(bt_avrcp_get_curplayerapp_value_rsp_struct));		
}

void btmtk_avrcp_send_set_player_value_rsp(AvrcpChannelContext *cntx, U8 error){
	bt_avrcp_set_playerapp_value_rsp_struct msg;
	ext_os_memset( &msg, 0, sizeof(msg));
	
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	}
	
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_SET_PLAYERAPP_VALUE_RSP, &msg, sizeof(bt_avrcp_set_playerapp_value_rsp_struct) );		
}


/* global variable for keeping repsonse data */
bt_avrcp_get_playerapp_attributetext_rsp_struct  g_avrcp_playerapp_attr_rsp;
bt_avrcp_get_playerapp_valuetext_rsp_struct      g_avrcp_playerapp_value_rsp;

/**
* @brief a loop function to reponse get_player_attr_text
* MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_RSP
* @param index the index of current item. beginning with 0.
* @param total the total number. when index equal to total, the response will be sent.
* @param error the error code when the error is not 0, it is AVRCP_ERRORCODE (AVRCP spec. 6.15.3)
*/
void btmtk_avrcp_send_get_player_attr_text_rsp(AvrcpChannelContext *cntx, U8 error, U8 index, U8 total, U8 attr_id, U16 charset, U8 strlen, U8 *strText){
	bt_avrcp_get_playerapp_attributetext_rsp_struct *pmsg;
	pmsg = &g_avrcp_playerapp_attr_rsp;
	if( index == 0 || error != 0 ){
		// clean up the data
		ext_os_memset( pmsg, 0, sizeof(bt_avrcp_get_playerapp_attributetext_rsp_struct));
	}
	
	pmsg->chnl_num = 0;
	pmsg->seq_id = cntx->seq_id;
	
	if( error == 0 ){
		pmsg->reject = FALSE;
		if( index < BT_AVRCP_MAX_ATTRIBUTE_NUM ){
			pmsg->count = total;
			pmsg->attribute_id[index] = attr_id;
			pmsg->charset[index] = charset;
			pmsg->attribute_string_len[index] = strlen;
			if( pmsg->attribute_string_len[index] >= BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE ){
				pmsg->attribute_string_len[index] = BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE -1;
			}
			memcpy( &pmsg->attribute_string[index], strText,  pmsg->attribute_string_len[index]);
		}else{
			// ignore too many item
			bt_ext_log("AVRCP send_get_player_attr_text_rsp ignore index:%d total:%d", index, total );			
		}
	}else{
		pmsg->reject = TRUE;
		pmsg->error_code = error;
	}
	
	if( ( total > 0 && (index+1) == total) || error != 0 || total == 0 ){
		AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_RSP, pmsg, sizeof(bt_avrcp_get_playerapp_attributetext_rsp_struct) );
	}
}

/**
* MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_RSP
*/
void btmtk_avrcp_send_get_player_value_text_value_rsp(AvrcpChannelContext *cntx, U8 error, U8 index, U8 total, U8 attr_id, U8 value_id, U16 charset, U8 strlen, U8 *strText){
	bt_avrcp_get_playerapp_valuetext_rsp_struct *pmsg;
	pmsg = &g_avrcp_playerapp_value_rsp;
	
	BT_AVRCP_AT();
	
	if( index == 0 || error != 0 ){
		// clean up the data
		bt_ext_log("avrcp:clean g_avrcp_playerapp_value_rsp" );
		ext_os_memset( pmsg, 0, sizeof(bt_avrcp_get_playerapp_valuetext_rsp_struct));
	}
	
	pmsg->chnl_num = 0;
	pmsg->seq_id = cntx->seq_id;
	
	if( error == 0 ){
		pmsg->reject = FALSE;
		if( index < BT_AVRCP_MAX_ATTRIBUTE_NUM ){
			pmsg->count = total;
			pmsg->attribute_id = attr_id;
			pmsg->value_ids[index] = value_id;
			pmsg->charset[index] = charset;
			pmsg->value_string_len[index] = strlen;
			if( pmsg->value_string_len[index] >= BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE ){
				pmsg->value_string_len[index] = BT_AVRCP_MAX_ATTRIBUTE_STRING_SIZE -1;
			}
			memcpy( &pmsg->value_string[index], strText,  pmsg->value_string_len[index]);
		}else{
			// ignore too many item
			bt_ext_log("AVRCP get_player_value_text_value_rsp ignore index:%d total:%d", index, total );			
		}
	}else{
		pmsg->reject = TRUE;
		pmsg->error_code = error;
	}
	
	if( (index+1) == total || error != 0 || total == 0 ){
		AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_RSP, pmsg, sizeof(bt_avrcp_get_playerapp_valuetext_rsp_struct) );
	}	
	
	BT_AVRCP_AT();
}

void btmtk_avrcp_send_inform_charsetset_rsp(AvrcpChannelContext *cntx, U8 error){
	bt_avrcp_inform_display_charset_rsp_struct msg;
	
	BT_AVRCP_AT();
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	}
	
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_INFORM_DISPLAY_CHARSET_RSP, &msg, sizeof(bt_avrcp_inform_display_charset_rsp_struct));
	
	BT_AVRCP_AT();
}

void btmtk_avrcp_send_battery_status_rsp(AvrcpChannelContext *cntx, U8 error){
	bt_avrcp_inform_battery_statusofct_rsp_struct msg;
	
	BT_AVRCP_AT();
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 
 	
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_INFORM_BATTERY_STATUSOFCT_RSP, &msg, sizeof(bt_avrcp_inform_battery_statusofct_rsp_struct));
	
	BT_AVRCP_AT();
}

bt_avrcp_get_element_attributes_rsp_struct g_avrcp_get_element_rsp;
int g_avrcp_get_element_rsp_text_offset;

/**
* MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_RSP
*/
void btmtk_avrcp_send_get_element_attributes_rsp(AvrcpChannelContext *cntx, U8 error, U8 index, U8 total, U8 attr_id, U16 charset, U16 strlen, U8 *strText){
	bt_avrcp_get_element_attributes_rsp_struct *pmsg;
	U8 *pElement_data;
	int remain_space = 0;
	pmsg = &g_avrcp_get_element_rsp;
	if( index == 0 || error != 0 ){
		// clean up the data
		bt_ext_log("AVRCP send_get_element_attributes cleanup data" );
		ext_os_memset( pmsg, 0, sizeof(bt_avrcp_get_element_attributes_rsp_struct));
		g_avrcp_get_element_rsp_text_offset = 0;
		pmsg->count = 0;
	}
	
	pmsg->chnl_num = 0;
	pmsg->seq_id = cntx->seq_id;
	if( g_avrcp_get_element_rsp_text_offset > BT_AVRCP_MAX_ELEMENET_BUFFER_SIZE ){
		bt_ext_log("AVRCP btmtk_avrcp_send_get_element_attributes_rsp Wrong text_offset:%d", g_avrcp_get_element_rsp_text_offset );
		g_avrcp_get_element_rsp_text_offset = 0;
	}
	
	if( error == 0 ){
		pmsg->reject = FALSE;
		bt_ext_log("AVRCP send_get_element_attributes_rsp raw i:%d total:%d attid:%d strlen:%d", index, total, attr_id, strlen);
		if( index < BT_AVRCP_MAX_GET_ELEMENT_ATTR_NUM ){
			
			pmsg->elements[index].attribute_id = attr_id;
			pmsg->elements[index].charset = charset;
			
			if( g_avrcp_get_element_rsp_text_offset >= BT_AVRCP_MAX_ELEMENET_BUFFER_SIZE ){
				// ignore it
				bt_ext_log("AVRCP send_get_element_attributes No space for index:%d tex_offset:%d", index, g_avrcp_get_element_rsp_text_offset );
			}else{
				
			    // bury9090 ALPS002777081 don't send the empty attr_id
			    if( attr_id != 0 && strlen != 0 ){
				// no more space for more data
				pmsg->count++;
				pmsg->elements[index].value_len = strlen;
				
				if( pmsg->elements[index].value_len >= BT_AVRCP_MAX_ELEMENET_BUFFER_SIZE ){
					pmsg->elements[index].value_len = BT_AVRCP_MAX_ELEMENET_BUFFER_SIZE -1;
				}
				remain_space = (BT_AVRCP_MAX_ELEMENET_BUFFER_SIZE-g_avrcp_get_element_rsp_text_offset-1);
				if( pmsg->elements[index].value_len >= remain_space ){
					pmsg->elements[index].value_len = remain_space;
				}
				
				pElement_data = &pmsg->element_data[g_avrcp_get_element_rsp_text_offset];
				pmsg->elements[index].value_text_offset = g_avrcp_get_element_rsp_text_offset;
				memcpy( pElement_data, strText, pmsg->elements[index].value_len );
				
				g_avrcp_get_element_rsp_text_offset += (pmsg->elements[index].value_len +1);
				
				bt_ext_log("AVRCP send_get_element_attributes strlen:%d offset:%d g_offset:%d", strlen, pmsg->elements[index].value_text_offset , g_avrcp_get_element_rsp_text_offset );
			    }else{
			  	// 
			  	bt_ext_log("AVRCP send_get_element_attributes ignore empty attrib attri_id:%d strlen:%d", attr_id, strlen);
			    }
			}
		}else{
			// ignore too many item
			bt_ext_log("AVRCP send_get_element_attributes ignore index:%d total:%d", index, total );			
		}
	}else{
		pmsg->reject = TRUE;
		pmsg->error_code = error;
	}
	
	if( ( (index+1) == total && total > 0) || error != 0 || total == 0 ){
		// total:0 error:!0 should send result immediately
		AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_RSP, pmsg, sizeof(bt_avrcp_get_element_attributes_rsp_struct) );
	}	
}	
	
void btmtk_avrcp_send_get_playstatus_rsp(AvrcpChannelContext *cntx, U8 error, U32 song_length, U32 song_position, U8 status){
	bt_avrcp_get_playerstatus_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.song_length = song_length;
		msg.song_position = song_position;
		msg.play_status = status;		
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_RSP, &msg, sizeof(bt_avrcp_get_playerstatus_rsp_struct));	
}

void btmtk_avrcp_send_reg_notievent_rsp(AvrcpChannelContext *cntx, bt_avrcp_register_notification_rsp_struct *rsp){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	ext_os_memcpy( &msg, rsp, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));	
}

void btmtk_avrcp_send_reg_notievent_playback_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim, U8 status){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.is_interim = interim;
		msg.event_id = AVRCP_NOTIFY_EVENT_PLAYBACK_STATUS_CHANGED;
		msg.status = status;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 
	
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));		
}

void btmtk_avrcp_send_reg_notievent_track_changed_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim, U8 *identifier){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.is_interim = interim;
		msg.event_id = AVRCP_NOTIFY_EVENT_TRACK_CHANGED;
		ext_os_memcpy( &msg.identifier, identifier, 8);
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 
	
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));	
}

void btmtk_avrcp_send_reg_notievent_reached_end_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.is_interim = interim;
		msg.event_id = AVRCP_NOTIFY_EVENT_TRACK_REACHED_END;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));	
}

void btmtk_avrcp_send_reg_notievent_reached_start_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.is_interim = interim;
		msg.event_id = AVRCP_NOTIFY_EVENT_TRACK_REACHED_START;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));	
}

void btmtk_avrcp_send_reg_notievent_pos_changed_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim, U32 position){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.is_interim = interim;
		msg.pos = position;
		msg.event_id = AVRCP_NOTIFY_EVENT_PLAYBACK_POS_CHANGED;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));	
}

void btmtk_avrcp_send_reg_notievent_battery_status_changed_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim, U8 status){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.is_interim = interim;
		msg.status = status;
		msg.event_id = AVRCP_NOTIFY_EVENT_BATT_STATUS_CHANGED;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));	
}

void btmtk_avrcp_send_reg_notievent_system_status_changed_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim, U8 status){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.is_interim = interim;
		msg.status = status;
		msg.event_id = AVRCP_NOTIFY_EVENT_SYSTEM_STATUS_CHANGED;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));	
}
void btmtk_avrcp_send_reg_notievent_now_playing_content_changed_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.is_interim = interim;
		msg.event_id = AVRCP_NOTIFY_EVENT_NOW_PLAYING_CONTENT_CHANGED;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));	
}

void btmtk_avrcp_send_reg_notievent_player_appsettings_changed_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim, U8 count, U8 *attr_ids, U8 *value_ids){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.is_interim = interim;
		msg.event_id = AVRCP_NOTIFY_EVENT_PLAYER_APPLICATION_SETTING_CHANGED;
		
		if( count > BT_AVRCP_MAX_ATTRIBUTE_NUM ){
			count = BT_AVRCP_MAX_ATTRIBUTE_NUM;
		}
		msg.count = count;
		ext_os_memcpy(&msg.attribute_id, attr_ids, count);
		ext_os_memcpy(&msg.attribute_value, value_ids, count);		
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));	
}

void btmtk_avrcp_send_reg_notievent_availplayers_changed_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.is_interim = interim;
		msg.event_id = AVRCP_NOTIFY_EVENT_AVAILABLE_PLAYERS_CHANGED;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));	
}

void btmtk_avrcp_send_reg_notievent_addredplayer_changed_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim, U16 player_id, U16 uid_counter){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.is_interim = interim;
		msg.id = player_id;
		msg.uid_counter = uid_counter;
		msg.event_id = AVRCP_NOTIFY_EVENT_ADDRESSED_PLAYER_CHANGED;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));	
}

void btmtk_avrcp_send_reg_notievent_uids_changed_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim, U16 uid_counter){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.is_interim = interim;
		msg.uid_counter = uid_counter;
		msg.event_id = AVRCP_NOTIFY_EVENT_UIDS_CHANGED;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));	
}

void btmtk_avrcp_send_reg_notievent_volume_changed_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim, U8 volume){
	bt_avrcp_register_notification_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.is_interim = interim;
		msg.volume = volume;
		msg.event_id = AVRCP_NOTIFY_EVENT_VOLUME_CHANGED;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP, &msg, sizeof(bt_avrcp_register_notification_rsp_struct));	
}

/*AVRCP 14 */

void btmtk_avrcp_send_set_absolute_volume_rsp(AvrcpChannelContext *cntx, U8 error, U8 status, U8 volume){
	bt_avrcp_set_absolute_volume_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.volume = volume;
	}else{
		msg.reject = TRUE;
		msg.error_code = status;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_SET_ABSOLUTE_VOLUME_RSP, &msg, sizeof(bt_avrcp_set_absolute_volume_rsp_struct));	
}


void btmtk_avrcp_send_set_addressedplayer_rsp(AvrcpChannelContext *cntx, U8 error, U8 status){
	bt_avrcp_set_addressedplayer_rsp_struct msg;
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.status = status;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_RSP, &msg, sizeof(bt_avrcp_set_addressedplayer_rsp_struct));	
}

bt_avrcp_get_folderitems_rsp_struct g_get_folderitems_rsp;
int g_get_folderitems_data_offset;

void btmtk_avrcp_reset_send_get_folderitems_rsp(AvrcpChannelContext *cntx){
	bt_avrcp_get_folderitems_rsp_struct *pmsg;
	pmsg = &g_get_folderitems_rsp;
	
	bt_ext_log("[AVRCP] btmtk_avrcp_reset_send_get_folderitems_rsp");
	BT_AVRCP_AT();
	
	ext_os_memset( &g_get_folderitems_rsp, 0, sizeof(bt_avrcp_get_folderitems_rsp_struct));
	g_get_folderitems_data_offset = 0;
	bt_ext_log("[AVRCP] clean-up g_get_folderitems_rsp");
	
	BT_AVRCP_AT();
}

void btmtk_avrcp_config_send_get_folderitems_attribute_rsp(AvrcpChannelContext *cntx, U8 item, U8 attritem, U32 attrID, U16 charset, U16 attrvalue_len, const U8 *attrvalue){
 // config send-folderitems' attributes
	bt_avrcp_get_folderitems_rsp_struct *pmsg;
	bt_avrcp_attrbute_struct *pattr;
	
	BT_AVRCP_AT();
	pmsg = &g_get_folderitems_rsp; 
	bt_ext_log("[AVRCP] config_send_folderitems_attribute_rsp item:%d total:%d attritem:%ld", item, attritem, attrID);
	
	if( item < BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE ){
		if( attritem < BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID ){
			pattr = &(pmsg->items[item].attributes[attritem]);
			pmsg->items[item].attribute_count = attritem +1 ; // increase the attr count
			
			pattr->attribute_id = attrID;
			pattr->charset = charset;
			pattr->attribute_value_len = attrvalue_len;
			pattr->attribute_value_string_offset = g_get_folderitems_data_offset;
			if( (g_get_folderitems_data_offset) >= BT_AVRCP_MAX_FOLDER_BUFFER_SIZE ){
				bt_ext_log("[AVRCP] too many data. no enough buffer. ignore it");
				pattr->attribute_value_len = 0;
			}else{
				if( (g_get_folderitems_data_offset + attrvalue_len ) >= BT_AVRCP_MAX_FOLDER_BUFFER_SIZE ) {
					bt_ext_log("[AVRCP] too many data. no enough buffer");
					pattr->attribute_value_len = BT_AVRCP_MAX_FOLDER_BUFFER_SIZE - g_get_folderitems_data_offset ;
				}	
				memcpy( &pmsg->data[g_get_folderitems_data_offset], attrvalue, pattr->attribute_value_len);
				g_get_folderitems_data_offset += (pattr->attribute_value_len + 1); // Add one space
				pmsg->data_len = g_get_folderitems_data_offset;
			}
			
		}else{
			bt_ext_log("[AVRCP] attritem %d > BT_AVRCP_MAX_MEDIA_ATTRIBUTE_ID. ignore it", item);	
		}
	}else{
		bt_ext_log("[AVRCP] item %d > BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE. ignore it", item);
	}
	
	BT_AVRCP_AT();	
}

void btmtk_avrcp_config_send_get_folderitems_rsp(AvrcpChannelContext *cntx, U8 item, U8 total, avrcp_folder_mixed_item* folder, U16 name_len, const U8 *name){
	 // config send-folderitem's item
	bt_avrcp_get_folderitems_rsp_struct *pmsg;
	pmsg = &g_get_folderitems_rsp;
	
	bt_ext_log("[AVRCP] config_send_folderitems_rsp item:%d total:%d", item, total);
	BT_AVRCP_AT();
	
	if( item < BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE ){	
		// copy folder to pmsg
		pmsg->counter = item + 1;
		pmsg->items[item].item_type = folder->item_type;
		memcpy( pmsg->items[item].uid, folder->uid, 8);
		pmsg->items[item].player_id = folder->player_id;
		pmsg->items[item].specific_type = folder->specific_type;
		pmsg->items[item].is_playable = folder->is_playable;
		pmsg->items[item].sub_type = folder->sub_type;
		pmsg->items[item].status = folder->status;
		memcpy( pmsg->items[item].mask, folder->mask, 16);
		pmsg->items[item].charset = folder->charset;
		
		pmsg->items[item].name_length = name_len;
		pmsg->items[item].name_offset = g_get_folderitems_data_offset;
		/* put name into buffer */
		if( (g_get_folderitems_data_offset) >= BT_AVRCP_MAX_FOLDER_BUFFER_SIZE ){
			bt_ext_log("[AVRCP] too many data. no enough buffer. ignore it");
			pmsg->items[item].name_length = 0;
		}else{
			if( (g_get_folderitems_data_offset + pmsg->items[item].name_length ) >= BT_AVRCP_MAX_FOLDER_BUFFER_SIZE ) {
				bt_ext_log("[AVRCP] too many data. no enough buffer");
				pmsg->items[item].name_length = BT_AVRCP_MAX_FOLDER_BUFFER_SIZE - g_get_folderitems_data_offset ;
			}
			
			memcpy( &pmsg->data[g_get_folderitems_data_offset], name, pmsg->items[item].name_length);
			g_get_folderitems_data_offset += (pmsg->items[item].name_length + 1); // Add one space
			pmsg->data_len = g_get_folderitems_data_offset;
		}		
		
		pmsg->items[item].item_length = 8 + 2 + 1 + 1 +4 + 1 + 16 + 2 + 2 + pmsg->items[item].name_length;
		
	}else{
		bt_ext_log("[AVRCP] item %d > BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE. ignore it", item);
	}	
		 
	BT_AVRCP_AT();
	 
}
void btmtk_avrcp_send_get_folderitems_rsp(AvrcpChannelContext *cntx, U8 error, U8 status, U16 uid_counter){
 // send out the message
	bt_avrcp_get_folderitems_rsp_struct *pmsg;
	pmsg = &g_get_folderitems_rsp;
	 
	bt_ext_log("[AVRCP] send_get_folderitems_rsp error:%d status:%d uid_counter:%d", error, status, uid_counter);
	// send data out
	pmsg->seq_id = cntx->seq_id;
	if( 0 == error ){
		pmsg->reject = FALSE;
		pmsg->status = 0x04;
	}else{
		pmsg->reject = TRUE;
		pmsg->status = status;  // Use PTS TC_TG_MCN_NP_BV_09_C to test no playlist
		pmsg->error_code = status;		
	}
	
	pmsg->uid_counter = uid_counter;
	bt_ext_log("[AVRCP] SendMessage _AVRCP_GET_FOLDERITEMS_RSP");
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_GET_FOLDERITEMS_RSP, pmsg, sizeof(bt_avrcp_get_folderitems_rsp_struct));	
	
	BT_AVRCP_AT(); 
}


bt_avrcp_set_browsedplayer_rsp_struct g_set_browsedplayer_rsp;
int g_set_browsedplayer_offset = 0;

void btmtk_avrcp_config_set_browsedplayer_rsp(AvrcpChannelContext *cntx, U8 item, U8 total, U16 name_len, U8 *foldername){
	bt_avrcp_set_browsedplayer_rsp_struct *pmsg;
	pmsg = (bt_avrcp_set_browsedplayer_rsp_struct *)&g_set_browsedplayer_rsp;
	
	BT_AVRCP_AT();
	
	if( item == 0 ){
		ext_os_memset( &g_set_browsedplayer_rsp, 0, sizeof(bt_avrcp_set_browsedplayer_rsp_struct));
		g_set_browsedplayer_offset = 0;
		bt_ext_log("AVRCP btmtk_avrcp_config_set_browsedplayer cleanup" );			
	}
	
	if( item >= total && item != 0 ){
		bt_ext_log("AVRCP btmtk_avrcp_config_set_browsedplayer error item(%d)>=total(%d)", item, total );
		return;
	}
	
	if( item < BT_AVRCP_MAX_FOLDER_DEPTH_NUM ){
		//save data 
		pmsg->folder_depth = total;
		if( pmsg->folder_depth > 0 ){
			pmsg->foldername_len[item] = name_len;
			if( (pmsg->foldername_len[item]+g_set_browsedplayer_offset) > BT_AVRCP_MAX_FOLDER_BUFFER_SIZE ){
				// nomore space to save folder name
				pmsg->foldername_len[item] = 0;
			}else{
				ext_os_memcpy( &pmsg->folder_data[g_set_browsedplayer_offset], foldername, pmsg->foldername_len[item]);
				pmsg->foldername_offset[item] = g_set_browsedplayer_offset;
				g_set_browsedplayer_offset += pmsg->foldername_len[item];
				g_set_browsedplayer_offset += 1;
			}
		}else{
			// config root
			bt_ext_log("AVRCP btmtk_avrcp_config_set_browsedplayer config root path folder_depth:%d", total );
		}
	}else{
		// ignore data
		bt_ext_log("AVRCP btmtk_avrcp_config_set_browsedplayer too much data ignore it %d", g_set_browsedplayer_offset );
	}
	// fill data into g_set_browsedplayer_rsp
	
	BT_AVRCP_AT();
}

void btmtk_avrcp_send_set_browsedplayer_rsp(AvrcpChannelContext *cntx, U8 error, U8 status, U16 uid_counter, U32 num, U16 charset){
	bt_avrcp_set_browsedplayer_rsp_struct *pmsg;
	pmsg = (bt_avrcp_set_browsedplayer_rsp_struct *)&g_set_browsedplayer_rsp;
	
	BT_AVRCP_AT();
	
	if( error != 0 ){
		ext_os_memset( pmsg, 0, sizeof(bt_avrcp_set_browsedplayer_rsp_struct));
	}
	pmsg->chnl_num = 0;
	pmsg->seq_id = cntx->seq_id;
	if( error == 0 ){
		pmsg->reject = FALSE;
		pmsg->status = status;
		pmsg->uid_counter = uid_counter;
		pmsg->num_of_items = num;
		pmsg->char_set = charset;
	}else{
		pmsg->reject = TRUE;
		pmsg->error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_SET_BROWSEDPLAYER_RSP, pmsg, sizeof(bt_avrcp_set_browsedplayer_rsp_struct));
	
	BT_AVRCP_AT();
}

void btmtk_avrcp_send_change_path_rsp(AvrcpChannelContext *cntx, U8 error, U8 status, U32 num_of_items){
	bt_avrcp_change_path_rsp_struct msg;
	
	BT_AVRCP_AT();
	
	ext_os_memset( &msg, 0, sizeof(msg));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.status = status;
		msg.num_of_items = num_of_items;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_CHANGE_PATH_RSP, &msg, sizeof(bt_avrcp_change_path_rsp_struct));
	
	BT_AVRCP_AT();
}

bt_avrcp_get_itemattributes_rsp_struct g_get_itemattributes_rsp;

void btmtk_avrcp_send_get_itemattributes_rsp(AvrcpChannelContext *cntx, U8 error, U8 status, U8 item, U8 total, U32 attr_id, U16 charset, U16 len, U8 *string){
	bt_avrcp_get_itemattributes_rsp_struct *pmsg;
	pmsg = &g_get_itemattributes_rsp;
	
	BT_AVRCP_AT();
	
	if( error != 0 || item == 0){
		bt_ext_log("avrcp:clean g_get_itemattributes_rsp" );		
		ext_os_memset( &g_get_itemattributes_rsp, 0, sizeof(bt_avrcp_get_itemattributes_rsp_struct));
	}
	
	if( item < BT_AVRCP_MAX_GET_ELEMENT_ITEM_SIZE ){
		pmsg->status = status;
		pmsg->counter = item+1;
		pmsg->attribute_id[item] = attr_id;
		pmsg->charset[item] = charset;
		pmsg->attribute_value_len[item] = len;
		if( pmsg->attribute_value_len[item] >= BT_AVRCP_MAX_VALUE_STRING_SIZE ){
			pmsg->attribute_value_len[item] = BT_AVRCP_MAX_VALUE_STRING_SIZE - 1;
		}
		ext_os_memcpy( &pmsg->attribute_value_string[item], string, pmsg->attribute_value_len[item]);
	}
	
	if( error != 0 || (item+1) == total ){
		AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_GET_ITEMATTRIBUTES_RSP, pmsg, sizeof(bt_avrcp_get_itemattributes_rsp_struct));	
	}
	
	BT_AVRCP_AT();
}


void btmtk_avrcp_send_play_items_rsp(AvrcpChannelContext *cntx, U8 error, U8 status){
	bt_avrcp_play_items_rsp_struct msg;
	ext_os_memset( &msg, 0, sizeof(bt_avrcp_play_items_rsp_struct));
	
	BT_AVRCP_AT();
	
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.status = status;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_PLAY_ITEMS_RSP, &msg, sizeof(bt_avrcp_play_items_rsp_struct));	
	
	BT_AVRCP_AT();
}


void btmtk_avrcp_send_search_rsp(AvrcpChannelContext *cntx, U8 error, U8 status, U16 uid_counter, U32 found_num_of_items){
	bt_avrcp_search_rsp_struct msg;
	ext_os_memset( &msg, 0, sizeof(bt_avrcp_search_rsp_struct));
	
	BT_AVRCP_AT();
	
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.status =status;
		msg.uid_counter = uid_counter;
		msg.found_num_of_items = found_num_of_items;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_SEARCH_RSP, &msg, sizeof(bt_avrcp_search_rsp_struct));	
	
	BT_AVRCP_AT();
}



void btmtk_avrcp_send_add_tonowplaying_rsp(AvrcpChannelContext *cntx, U8 error, U8 status){
	bt_avrcp_add_tonowplaying_rsp_struct msg;
	
	BT_AVRCP_AT();
	
	ext_os_memset( &msg, 0, sizeof(bt_avrcp_add_tonowplaying_rsp_struct));
	msg.chnl_num = 0;
	msg.seq_id = cntx->seq_id;
	if( error == 0 ){
		msg.reject = FALSE;
		msg.status = status;
	}else{
		msg.reject = TRUE;
		msg.error_code = error;
	} 	
		
	AVRCP_SendMessage(cntx, MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_RSP, &msg, sizeof(bt_avrcp_add_tonowplaying_rsp_struct));	
	
	BT_AVRCP_AT();
}

x
#endif
#endif
