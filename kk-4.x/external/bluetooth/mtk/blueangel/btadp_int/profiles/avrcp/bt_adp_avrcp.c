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

#ifdef __BT_AVRCP_PROFILE__

/* General header file */
#include "bt_common.h"
#include "bt_adp_avrcp.h"
#include "avrcp_inc.h"
#if defined(BTMTK_ON_LINUX)
#include "sys/stat.h"
#include "a2dp.h" // for BT_A2DP_STATE_STREAMING
#include "a2dp_inc.h" // for BT_A2DP_STATE_STREAMING
#endif
static void avrcp_app_callback(BTAVRCPCallbackParms *Parms);

void AVRCP_UtilAVRCPCmdListPlayerAttributesRsp(bt_avrcp_list_playerapp_attribute_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdListPlayerValuesRsp(bt_avrcp_list_playerapp_value_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdRegisterNotificationRsp(bt_avrcp_register_notification_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdAbortContinueResponseRsp(bt_avrcp_abort_continueresponse_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdSetAbsoluteVolumeRsp(bt_avrcp_set_absolute_volume_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdSetAddressedPlayerRsp(bt_avrcp_set_addressedplayer_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdGetPlayerItemsRsp(bt_avrcp_get_playeritems_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdGetFolderItemsRsp(bt_avrcp_get_folderitems_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdSetBrowsedPlayerRsp(bt_avrcp_set_browsedplayer_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdChangePathRsp(bt_avrcp_change_path_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdGetItemAttributesRsp(bt_avrcp_get_itemattributes_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdPlayItemsRsp(bt_avrcp_play_items_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdSearchRsp(bt_avrcp_search_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdAddToNowPlayingRsp(bt_avrcp_add_tonowplaying_rsp_struct *rsp);
void AVRCP_UtilAVRCPSDP_Query_Req(bt_avrcp_sdp_query_req_struct *req_p);

#define AVRCP_PAUSE_PRESSED_CODE     0x46
#define AVRCP_PAUSE_RELEASEED_CODE   0xc6
U8 g_avrcp_ignore_pause_release = FALSE;

#ifdef BTMTK_ON_LINUX
#ifdef BTMTK_AVRCP_IGNORE_TWICE_PAUSE
extern U8 A2dpGetState(void);
#endif
#endif

/*****************************************************************************
 * FUNCTION
 *  bt_a2p_ad2p_init
 * DESCRIPTION
 *  register callback function to a2dp.c
 * PARAMETERS
 *  void
 *****************************************************************************/
void bt_adp_avrcp_init(void)
{
    AVRCP_UtilRegisterCB(avrcp_app_callback);
#if defined(BTMTK_ON_LINUX)    
    // change uinput to 666 mode
    OS_Report("[AVRCP] change uinput mode");
	chmod("/dev/uinput", S_IRUSR | S_IRGRP | S_IROTH |S_IWUSR | S_IWGRP | S_IWOTH );
#endif
}


void avrcp_send_msg(
            module_type dst_id,
            msg_type msg_id,
            local_para_struct *local_para,
            peer_buff_struct *peer_buff)

{
   if (local_para != NULL)
   {
       BT_SendMessage(msg_id, dst_id, local_para, local_para->msg_len);
   }
   else
   {
       BT_SendMessage(msg_id, dst_id, NULL, 0);
   }
}


void bt_avrcp_send_activate_cnf( module_type dst_mod_id, U8 chnl_num, U16 result, U8 version)
{
	bt_avrcp_activate_cnf_struct* msg_p = (bt_avrcp_activate_cnf_struct*)
            construct_local_para(sizeof(bt_avrcp_activate_cnf_struct), TD_CTRL);

	msg_p->chnl_num = chnl_num;
	msg_p->result = result;
        msg_p->version = version;

	avrcp_send_msg( dst_mod_id, (U16)MSG_ID_BT_AVRCP_ACTIVATE_CNF, (local_para_struct *)msg_p, NULL );
}


void bt_avrcp_send_deactivate_cnf( module_type dst_mod_id, U8 chnl_num, U16 result )
{
	bt_avrcp_deactivate_cnf_struct* msg_p = (bt_avrcp_deactivate_cnf_struct*)
            construct_local_para(sizeof(bt_avrcp_deactivate_cnf_struct), TD_CTRL);

	msg_p->chnl_num = chnl_num;
	msg_p->result = result;

	avrcp_send_msg( dst_mod_id, (U16)MSG_ID_BT_AVRCP_DEACTIVATE_CNF, (local_para_struct *)msg_p, NULL );
}

void bt_avrcp_send_connect_cnf( module_type dst_mod_id, U8 chnl_num, U16 result, bt_avrcp_dev_addr_struct* device_addr, U8 connect_id)
{
	bt_avrcp_connect_cnf_struct* msg_p = (bt_avrcp_connect_cnf_struct*)
            construct_local_para(sizeof(bt_avrcp_connect_cnf_struct), TD_CTRL);

	msg_p->chnl_num = chnl_num;
	msg_p->result = result;
	msg_p->connect_id = connect_id;
	OS_MemCopy((U8 *) &msg_p->device_addr, (U8 *)device_addr, sizeof(bt_avrcp_dev_addr_struct) );

	avrcp_send_msg( dst_mod_id, (U16)MSG_ID_BT_AVRCP_CONNECT_CNF, (local_para_struct *)msg_p, NULL );
}

void bt_avrcp_send_connect_ind( module_type dst_mod_id, 
                                        U8 chnl_num, 
                                        bt_avrcp_dev_addr_struct* device_addr, 
                                        U8 connect_id,
                                        U8 device_name_size,
                                        U8 *device_name)
{
	bt_avrcp_connect_ind_struct* msg_p = (bt_avrcp_connect_ind_struct*)
            construct_local_para(sizeof(bt_avrcp_connect_ind_struct), TD_CTRL);

	msg_p->chnl_num = chnl_num;
	msg_p->connect_id = connect_id;
    msg_p->device_name_size = device_name_size;
	OS_MemCopy( (U8 *)&msg_p->device_addr, (U8 *)device_addr, sizeof(bt_avrcp_dev_addr_struct) );
    if(device_name_size > 59)
    {
        msg_p->device_name_size = 59;
        OS_MemCopy(msg_p->device_name, device_name, 59);
    }
    else
    {
        msg_p->device_name_size = device_name_size;
        OS_MemCopy(msg_p->device_name, device_name, device_name_size);
    }	
    
    // clean flag
    g_avrcp_ignore_pause_release = FALSE;
  
	avrcp_send_msg( dst_mod_id, (U16)MSG_ID_BT_AVRCP_CONNECT_IND, (local_para_struct *)msg_p, NULL );
}


void bt_avrcp_send_disconnect_cnf( module_type dst_mod_id, U8 chnl_num, U16 result, U8 connect_id )
{
	bt_avrcp_disconnect_cnf_struct* msg_p = (bt_avrcp_disconnect_cnf_struct*)
            construct_local_para(sizeof(bt_avrcp_disconnect_cnf_struct), TD_CTRL);

	msg_p->chnl_num = chnl_num;
	msg_p->result = result;
	msg_p->connect_id = connect_id;

	avrcp_send_msg( dst_mod_id, (U16)MSG_ID_BT_AVRCP_DISCONNECT_CNF, (local_para_struct *)msg_p, NULL );
}

void bt_avrcp_send_disconnect_ind( module_type dst_mod_id, U8 chnl_num, U8 connect_id)
{
	bt_avrcp_disconnect_ind_struct* msg_p = (bt_avrcp_disconnect_ind_struct*)
            construct_local_para(sizeof(bt_avrcp_disconnect_ind_struct), TD_CTRL);

	msg_p->chnl_num = chnl_num;
	msg_p->connect_id = connect_id;

	avrcp_send_msg( dst_mod_id, (U16)MSG_ID_BT_AVRCP_DISCONNECT_IND, (local_para_struct *)msg_p, NULL );
}


void bt_avrcp_send_cmd_frame_cnf( module_type dst_mod_id, U8 chnl_num, U8 seq_id, U8	c_type,	U8	subunit_type, U8 subunit_id,	U16 data_len, U8 *frame_data)
{
	bt_avrcp_cmd_frame_cnf_struct* msg_p = (bt_avrcp_cmd_frame_cnf_struct*)
            construct_local_para(sizeof(bt_avrcp_cmd_frame_cnf_struct), TD_CTRL);

	msg_p->chnl_num = chnl_num;
	msg_p->seq_id = seq_id;
	msg_p->c_type = c_type;
	msg_p->subunit_type = subunit_type;
	msg_p->subunit_id = subunit_id;
	msg_p->data_len = data_len;

	if((data_len > 0) && (frame_data != NULL))
	{
        OS_MemCopy(msg_p->frame_data, frame_data, data_len);
	}

	avrcp_send_msg( dst_mod_id, (U16)MSG_ID_BT_AVRCP_CMD_FRAME_CNF, (local_para_struct *)msg_p, NULL );
}

void bt_avrcp_send_cmd_frame_ind_rsp_toself( module_type dst_mod_id, U8 chnl_num, U8 seq_id, U8	c_type,	U8	subunit_type, U8 subunit_id,	U16 data_len, U8 *frame_data)
{
	bt_avrcp_cmd_frame_ind_rsp_struct* msg_p = (bt_avrcp_cmd_frame_ind_rsp_struct*)
            construct_local_para(sizeof(bt_avrcp_cmd_frame_ind_rsp_struct), TD_CTRL);

	msg_p->chnl_num = chnl_num;
	msg_p->seq_id = seq_id;
	msg_p->c_type = c_type;
	msg_p->subunit_type = subunit_type;
	msg_p->subunit_id = subunit_id;
	msg_p->data_len = data_len;

	if((data_len > 0) && (frame_data != NULL))
	{
        OS_MemCopy(msg_p->frame_data, frame_data, data_len);
	}

	avrcp_send_msg( dst_mod_id, (U16)MSG_ID_BT_AVRCP_CMD_FRAME_IND_RSP, (local_para_struct *)msg_p, NULL );
}

void bt_avrcp_send_sdp_query_ind(module_type dst_mod_id, U8 chnl_num, U8 seq_id, U8 status, U16 tg_version, U16 tg_feature, U16 ct_version, U16 ct_feature)
{
	bt_avrcp_sdp_query_ind_struct* msg_p = (bt_avrcp_sdp_query_ind_struct*)
            construct_local_para(sizeof(bt_avrcp_sdp_query_ind_struct), TD_CTRL);

	msg_p->chnl_num = chnl_num;
	msg_p->seq_id = seq_id;
	msg_p->status = status;
    msg_p->tg_version = tg_version;
	msg_p->remote_tg_support_feature = tg_feature;
    
    msg_p->ct_version = ct_version;    
	msg_p->remote_ct_support_feature = ct_feature;


    OS_Report("[AVRCP] sdp query ind status:%d tg:0x%x tgfeature:0x%x", status, tg_version, tg_feature);
        OS_Report("[AVRCP] sdp query ind status:%d ct:0x%x ctfeature:0x%x", status, ct_version, ct_feature);
	avrcp_send_msg( dst_mod_id, (U16)MSG_ID_BT_AVRCP_SDP_QUERY_IND, (local_para_struct *)msg_p, NULL );
}

void bt_avrcp_send_test_cmd_req( bt_test_cmd_req_struct *cmd)
{
    // MSG_ID_BT_AVRCP_TEST_CMD_REQ
    U8 status = 0;
	bt_test_cmd_req_struct* msg_p = (bt_test_cmd_req_struct*) construct_local_para(sizeof(bt_test_cmd_req_struct), TD_CTRL);

    OS_MemCopy( msg_p->test_cmd_content, cmd->test_cmd_content, sizeof(cmd->test_cmd_content) );

	avrcp_send_msg( MOD_MMI, (U16)MSG_ID_BT_AVRCP_TEST_CMD_REQ, (local_para_struct *)msg_p, NULL );
}



U8 avrcp_is_A2DP_streaming(){
	U8 ret = TRUE;
#ifdef BTMTK_AVRCP_IGNORE_TWICE_PAUSE
#ifdef BTMTK_ON_LINUX
	// use A2DP function to get the streaming status
	U8 state;
	state = A2dpGetState();
	Report(("[AVRCP] a2dp state:%d", state));
	if( BT_A2DP_STATE_STREAMING == state){
		ret = TRUE;
	}else{
		ret = FALSE;
	}
#else
	ret = TRUE; // always assume it is streaming	
#endif
#endif

	return ret;
}

U8 avrcp_is_pause_cmd_frame_ind(bt_avrcp_cmd_frame_ind_struct *ind){
	if( NULL == ind ){
		return FALSE;
	}
	if( 3 == ind->data_len && 0 == ind->c_type && 9 == ind->subunit_type && 0x7c == ind->frame_data[0] ){
		// a panel's passthrough command
		if( AVRCP_PAUSE_PRESSED_CODE == ind->frame_data[1] || AVRCP_PAUSE_RELEASEED_CODE == ind->frame_data[1] ){
			// 0x46 is pressed pause key code. 0xc6 is released pause key code.
			return TRUE;
		}
	}
	return FALSE;
}

void bt_avrcp_send_cmd_frame_ind( module_type dst_mod_id, U8 chnl_num, U8 seq_id, U16 profile_id,	U8	c_type,	U8	subunit_type, U8 subunit_id,	U16 data_len, U8 *frame_data)
{
	bt_avrcp_cmd_frame_ind_struct* msg_p = (bt_avrcp_cmd_frame_ind_struct*)
            construct_local_para(sizeof(bt_avrcp_cmd_frame_ind_struct), TD_CTRL);

	msg_p->chnl_num = chnl_num;
	msg_p->seq_id = seq_id;
	msg_p->profile_id = profile_id;	
	msg_p->c_type = c_type;
	msg_p->subunit_type = subunit_type;
	msg_p->subunit_id = subunit_id;
	msg_p->data_len = data_len;

	if( data_len > BT_AVRCP_MAX_CMD_FRAME_SIZE){
		// ignore it
		free_local_para((void *)msg_p);
		Report(("[AVRCP][WRN] AVRCP receive too many data. Throw it!"));
		return;
	}

	ASSERT(	data_len <= BT_AVRCP_MAX_CMD_FRAME_SIZE);

	if((data_len > 0) && (frame_data != NULL))
	{
        OS_MemCopy(msg_p->frame_data, frame_data, data_len);
	}

#ifdef BTMTK_ON_LINUX
#ifdef BTMTK_AVRCP_IGNORE_TWICE_PAUSE
	// Enhance ignore the pause
	if( TRUE == avrcp_is_pause_cmd_frame_ind(msg_p)  ){
		if( TRUE == g_avrcp_ignore_pause_release && AVRCP_PAUSE_RELEASEED_CODE == frame_data[1] ){
			g_avrcp_ignore_pause_release = FALSE;
			
			free_local_para((void *)msg_p);
			Report(("[AVRCP][WRN] AVRCP ignore pause-release key "));
			// send back to remote device. msg_p has been freed.
			AVRCP_UitlCmdFrameRspInd(chnl_num, seq_id, c_type==0?9:c_type, subunit_type, subunit_id, data_len, frame_data);
			return;
		}
		
		if( TRUE != avrcp_is_A2DP_streaming() ){
			// no streamming, ignore the press of pause
			if( AVRCP_PAUSE_PRESSED_CODE == frame_data[1] ){
				g_avrcp_ignore_pause_release = TRUE;
			}
			free_local_para((void *)msg_p);
			Report(("[AVRCP][WRN] AVRCP ignore pause-pressed key in pause streaming"));
			// send back to remote device. msg_p has been freed.
			AVRCP_UitlCmdFrameRspInd(chnl_num, seq_id, c_type==0?9:c_type, subunit_type, subunit_id, data_len, frame_data);
			return;
		}
	}	
#else
	// CR ALPS00047354 Plantronics590 always send pause (even in pause stream)
#endif	
#endif

	avrcp_send_msg( dst_mod_id, (U16)MSG_ID_BT_AVRCP_CMD_FRAME_IND, (local_para_struct *)msg_p, NULL );
}

static void bt_avrcp_send_command_ind(module_type dst_mod_id, U16 msg_id, U8 *data, U16 data_size){
	U8 *msg_p = NULL;
    
	if((data_size > sizeof(local_para_struct)) && (data != NULL))
	{
        msg_p = (U8 *) construct_local_para(data_size, TD_CTRL);
        // copy the append data except the header, local_para_struct
        OS_MemCopy(msg_p+sizeof(local_para_struct), data+sizeof(local_para_struct), data_size-sizeof(local_para_struct));
	}else{
	    Assert(0);
	}
	if( msg_id < MSG_ID_BT_AVRCP_GROUP_START || msg_id > MSG_ID_BT_AVRCP_GROUP_END){
	    Assert(0);
	}

    avrcp_send_msg( dst_mod_id, (U16)msg_id, (local_para_struct *)msg_p, NULL );    
}

/**
* @brief callback function (from stack/profile to int-adapter layer)
* @see avrcp_adp_message_handler
*/
static void avrcp_app_callback(BTAVRCPCallbackParms *Parms)
{
    bt_avrcp_sdp_query_req_struct sdp_query_req;
    //Report(("avrcp_app_callback with event = %d",Parms->event));
    kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_UTIL_APP_CALLBACK, Parms->event );

    switch (Parms->event)
    {
        case BTAVRCP_EVENT_ACTIVATE_CNF:
            bt_avrcp_send_activate_cnf(
            	  MOD_MMI,
            	  Parms->p.AVRCPEventActivateCnf.chnl_num, 
            	  Parms->result,
            	  Parms->p.AVRCPEventActivateCnf.version);
            break;
            
        case BTAVRCP_EVENT_DEACTIVATE_CNF:
            bt_avrcp_send_deactivate_cnf(
            	  MOD_MMI,
            	  Parms->p.AVRCPEventActivateCnf.chnl_num, 
            	  Parms->result);
            break;
            
        case BTAVRCP_EVENT_CONNECT_CNF:
            bt_avrcp_send_connect_cnf(
                MOD_MMI, 
                Parms->p.AVRCPEventConnectCnf.chnl_num, 
                Parms->result, 
                &Parms->p.AVRCPEventConnectCnf.dev_addr, 
                Parms->p.AVRCPEventConnectCnf.connection_id);

            // ask the sdp record
            OS_Report("[AVRCP] auto do sdp query to remote avrcp sdp");
            sdp_query_req.chnl_num = Parms->p.AVRCPEventConnectCnf.chnl_num;
            sdp_query_req.query_role = 0xff;
            sdp_query_req.seq_id = 0;
            AVRCP_UtilAVRCPSDP_Query_Req(&sdp_query_req);
            break;

        case BTAVRCP_EVENT_CONNECT_IND:
            bt_avrcp_send_connect_ind(
                MOD_MMI,
                Parms->p.AVRCPEventConnectInd.chnl_num, 
                &Parms->p.AVRCPEventConnectInd.dev_addr, 
                Parms->p.AVRCPEventConnectInd.connection_id,
                Parms->p.AVRCPEventConnectInd.device_name_size,
                Parms->p.AVRCPEventConnectInd.device_name
                );
            break;

        case BTAVRCP_EVENT_DISCONNECT_CNF:
            bt_avrcp_send_disconnect_cnf(
                MOD_MMI,
                Parms->p.AVRCPEventDisconnectCnf.chnl_num,
                Parms->result,
                Parms->p.AVRCPEventDisconnectCnf.connection_id);
            break;

        case BTAVRCP_EVENT_DISCONNECT_IND:
            bt_avrcp_send_disconnect_ind(
                MOD_MMI, 
                Parms->p.AVRCPEventDisconnectInd.chnl_num, 
                Parms->p.AVRCPEventDisconnectInd.connection_id);
            break;

        case BTAVRCP_EVENT_COMMAND_FRAME_CNF:
            bt_avrcp_send_cmd_frame_cnf(
                MOD_MMI,
                Parms->p.AVRCPEventCmdFrameCnf.chnl_num, 
                Parms->p.AVRCPEventCmdFrameCnf.seq_id, 
                Parms->p.AVRCPEventCmdFrameCnf.ctype, 
                Parms->p.AVRCPEventCmdFrameCnf.subunit_type, 
                Parms->p.AVRCPEventCmdFrameCnf.subunit_id, 
                Parms->p.AVRCPEventCmdFrameCnf.data_len, 
                Parms->p.AVRCPEventCmdFrameCnf.frame_data);
            break;

        case BTAVRCP_EVENT_COMMAND_FRAME_IND:
            bt_avrcp_send_cmd_frame_ind(
                MOD_MMI, 
                Parms->p.AVRCPEventCmdFrameInd.chnl_num, 
                Parms->p.AVRCPEventCmdFrameInd.seq_id, 
                Parms->p.AVRCPEventCmdFrameInd.profile_id, 
                Parms->p.AVRCPEventCmdFrameInd.ctype, 
                Parms->p.AVRCPEventCmdFrameInd.subunit_type, 
                Parms->p.AVRCPEventCmdFrameInd.subunit_id, 
                Parms->p.AVRCPEventCmdFrameInd.data_len, 
                Parms->p.AVRCPEventCmdFrameInd.frame_data);
            break;
        /* AV13 browse channel */
        case BTAVRCP_EVENT_BROWSE_CONNECT_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_BROWSE_CONNECT_CNF,
                (U8 *) &Parms->p.AVRCPEventBrowseConnectCnf,
                sizeof(BTAVRCPEventBrowseConnectCnf));            
            break;
        case BTAVRCP_EVENT_BROWSE_CONNECT_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_BROWSE_CONNECT_IND,
                (U8 *) &Parms->p.AVRCPEventBrowseConnectInd,
                sizeof(BTAVRCPEventBrowseConnectInd));            
            break;
        case BTAVRCP_EVENT_BROWSE_DISCONNECT_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_BROWSE_DISCONNECT_IND,
                (U8 *) &Parms->p.AVRCPEventBrowseDisconnectInd,
                sizeof(BTAVRCPEventBrowseDisconnectInd));            
            break;
            
        /* AV13 AVRCP13 feature */
        case BTAVRCP_EVENT_GET_CAPABILITIES_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_CAPABILITIES_IND,
                (U8 *) &Parms->p.AVRCPGetCapabilitiesInd,
                sizeof(BTAVRCPGetCapabilitiesInd));
            break;
        case BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGATTRIBUTE_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_LIST_PLAYERAPP_ATTRIBUTE_IND,
                (U8 *) &Parms->p.AVRCPListPlayerappAttributeInd,
                sizeof(BTAVRCPListPlayerappAttributeInd));
            break;
        case BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGVALUE_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_LIST_PLAYERAPP_VALUE_IND,
                (U8 *) &Parms->p.AVRCPListPlayerappValueInd,
                sizeof(BTAVRCPListPlayerappValueInd));
            break;
		case BTAVRCP_EVENT_GET_CURPLAYERAPP_SETTINGVALUE_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_IND,
                (U8 *) &Parms->p.ABTAVRCPGetCURPlayerValueInd,
                sizeof(BTAVRCPGetCURPlayerValueInd));
            break;
		case BTAVRCP_EVENT_SET_PLAYERAPP_SETTINGVALUE_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_SET_PLAYERAPP_VALUE_IND,
                (U8 *) &Parms->p.ABTAVRCPSetPlayerValueInd,
                sizeof(BTAVRCPSetPlayerValueInd));
            break;
		case BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGATTRIBUTETEXT_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_IND,
                (U8 *) &Parms->p.ABTAVRCPGetPlayerAttrTextInd,
                sizeof(BTAVRCPGetPlayerAttrTextInd));
            break;
		case BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGVALUETEXT_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_IND,
                (U8 *) &Parms->p.ABTAVRCPGetPlayerValueTextInd,
                sizeof(BTAVRCPGetPlayerValueTextInd));
            break;
		case BTAVRCP_EVENT_INFORM_DISPLAY_CHARSET_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_INFORM_DISPLAY_CHARSET_IND,
                (U8 *) &Parms->p.ABTAVRCPInformCharsetInd,
                sizeof(BTAVRCPInformCharsetInd));
            break;
		case BTAVRCP_EVENT_INFORM_BATTERY_STATUSOFCT_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_INFORM_BATTERY_STATUSOFCT_IND,
                (U8 *) &Parms->p.ABTAVRCPInformBattOfCTInd,
                sizeof(BTAVRCPInformBattOfCTInd));
            break;
		case BTAVRCP_EVENT_GET_ELEMENT_ATTRIBUTES_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_IND,
                (U8 *) &Parms->p.ABTAVRCPGetElementAttrInd,
                sizeof(BTAVRCPGetElementAttrInd));
            break;
		case BTAVRCP_EVENT_GET_PLAYERAPP_STATUS_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_IND,
                (U8 *) &Parms->p.ABTAVRCPGetPlayerStatusInd,
                sizeof(BTAVRCPGetPlayerStatusInd));
            break;
        case BTAVRCP_EVENT_REGISTER_NOFITICATION_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_IND,
                (U8 *) &Parms->p.ABTAVRCPRegisterNotifyInd,
                sizeof(BTAVRCPRegisterNotifyInd ));
            break;
		case BTAVRCP_EVENT_REQUEST_CONTINUING_IND:
		    Assert(0); // it should not happen
		    break;
		case BTAVRCP_EVENT_ABORT_CONTINUING_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_ABORT_CONTINUERESPONSE_IND,
                (U8 *) &Parms->p.ABTAVRCPAbortContinueInd,
                sizeof(BTAVRCPAbortContinueInd ));
		    break;
		case BTAVRCP_EVENT_SET_ABSOLUTE_VOLUME_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_SET_ABSOLUTE_VOLUME_IND,
                (U8 *) &Parms->p.ABTAVRCPSetAbsoluteVolumeInd,
                sizeof(BTAVRCPSetAbsoluteVolumeInd ));
		    break;
		case BTAVRCP_EVENT_SET_ADDRESSEDPLAYER_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_IND,
                (U8 *) &Parms->p.ABTAVRCPSetAddrPlayerInd,
                sizeof(BTAVRCPSetAddrPlayerInd ));
		    break;
		case BTAVRCP_EVENT_SET_BROWSEDPLAYER_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_SET_BROWSEDPLAYER_IND,
                    (U8 *) &Parms->p.ABTAVRCPSetBrowsePlayerInd,
                    sizeof(BTAVRCPSetBrowsePlayerInd ));
    		break;
		case BTAVRCP_EVENT_GET_FOLDERITEMS_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_FOLDERITEMS_IND,
                (U8 *) &Parms->p.ABTAVRCPGetFolderItemsInd,
                sizeof(BTAVRCPGetFolderItemsInd));
		    break;
		case BTAVRCP_EVENT_CHANGE_PATH_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_CHANGE_PATH_IND,
                (U8 *) &Parms->p.ABTAVRCPChangePathInd,
                sizeof(BTAVRCPChangePathInd));
		    break;
		case BTAVRCP_EVENT_GET_ITEMATTRIBUTES_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_ITEMATTRIBUTES_IND,
                (U8 *) &Parms->p.ABTAVRCPGetItemAttrInd,
                sizeof(BTAVRCPGetItemAttrInd));
		    break;
		case BTAVRCP_EVENT_PLAY_ITEMS_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_PLAY_ITEMS_IND,
                (U8 *) &Parms->p.ABTAVRCPPlayItemsInd,
                sizeof(BTAVRCPPlayItemsInd ));
		    break;
		case BTAVRCP_EVENT_SEARCH_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_SEARCH_IND,
                (U8 *) &Parms->p.ABTAVRCPSearchInd,
                sizeof(BTAVRCPSearchInd ));
		    break;
		case BTAVRCP_EVENT_ADD_TONOWPLAYING_IND:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_IND,
                (U8 *) &Parms->p.ABTAVRCPAddNowPlayingInd,
                sizeof(BTAVRCPAddNowPlayingInd ));
		    break;
        /***** AVRCP control msg *****/
        case BTAVRCP_EVENT_GET_CAPABILITIES_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_CAPABILITIES_CNF,
                (U8 *) &Parms->p.ABTAVRCPAddNowPlayingInd,
                sizeof(BTAVRCPGetCapabilitiesCnf ));
            break;
        case BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGATTRIBUTE_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_LIST_PLAYERAPP_ATTRIBUTE_CNF ,
                (U8 *) &Parms->p.ABTAVRCPAddNowPlayingInd,
                sizeof(BTAVRCPListPlayerappAttributeCnf ));
            break;
        case BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGVALUE_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_LIST_PLAYERAPP_VALUE_CNF,
                (U8 *) &Parms->p.ABTAVRCPListPlayerappValueCnf,
                sizeof(BTAVRCPListPlayerappValueCnf));
            break;
        case BTAVRCP_EVENT_GET_CURPLAYERAPP_SETTINGVALUE_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_CNF,
                (U8 *) &Parms->p.ABTAVRCPGetCURPlayerValueCnf,
                sizeof(BTAVRCPGetCURPlayerValueCnf));
            break;
        case BTAVRCP_EVENT_SET_PLAYERAPP_SETTINGVALUE_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_SET_PLAYERAPP_VALUE_CNF,
                (U8 *) &Parms->p.ABTAVRCPSetPlayerValueCnf,
                sizeof(BTAVRCPSetPlayerValueCnf));
            break;
        case BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGATTRIBUTETEXT_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_CNF,
                (U8 *) &Parms->p.ABTAVRCPGetPlayerAttrTextCnf,
                sizeof(BTAVRCPGetPlayerAttrTextCnf));
            break;
        case BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGVALUETEXT_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_CNF,
                (U8 *) &Parms->p.ABTAVRCPGetPlayerValueTextCnf,
                sizeof(BTAVRCPGetPlayerValueTextCnf));
            break;
        case BTAVRCP_EVENT_INFORM_DISPLAY_CHARSET_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_INFORM_DISPLAY_CHARSET_CNF,
                (U8 *) &Parms->p.ABTAVRCPInformCharsetCnf,
                sizeof(BTAVRCPInformCharsetCnf));
            break;
        case BTAVRCP_EVENT_INFORM_BATTERY_STATUSOFCT_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_INFORM_BATTERY_STATUSOFCT_CNF,
                (U8 *) &Parms->p.ABTAVRCPInformBattOfCTCnf,
                sizeof(BTAVRCPInformBattOfCTCnf));
            break;
        case BTAVRCP_EVENT_GET_ELEMENT_ATTRIBUTES_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF,
                (U8 *) &Parms->p.ABTAVRCPGetElementAttrCnf,
                sizeof(BTAVRCPGetElementAttrCnf));
            break;
        case BTAVRCP_EVENT_GET_PLAYERAPP_STATUS_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_CNF,
                (U8 *) &Parms->p.ABTAVRCPGetPlayerStatusCnf,
                sizeof(BTAVRCPGetPlayerStatusCnf));
            break;
        case BTAVRCP_EVENT_REGISTER_NOFITICATION_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_CNF,
                (U8 *) &Parms->p.ABTAVRCPRegisterNotifyCnf,
                sizeof(BTAVRCPRegisterNotifyCnf));
            break;
        case BTAVRCP_EVENT_REQUEST_CONTINUING_CNF:
            kal_trace( BT_TRACE_G8_AVRCP, BT_AVRCP_IGNORE_CONTINUE_EVENT );
            break;
         case BTAVRCP_EVENT_ABORT_CONTINUING_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_ABORT_CONTINUERESPONSE_CNF,
                (U8 *) &Parms->p.ABTAVRCPAbortContinueCnf,
                sizeof(BTAVRCPAbortContinueCnf));
            break;
        case BTAVRCP_EVENT_SET_ABSOLUTE_VOLUME_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_SET_ABSOLUTE_VOLUME_CNF,
                (U8 *) &Parms->p.ABTAVRCPSetAbsoluteVolumeCnf,
                sizeof(BTAVRCPSetAbsoluteVolumeCnf));
            break;
        case BTAVRCP_EVENT_SET_ADDRESSEDPLAYER_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_CNF,
                (U8 *) &Parms->p.ABTAVRCPSetAddrPlayerCnf,
                sizeof(BTAVRCPSetAddrPlayerCnf));
            break;
        case BTAVRCP_EVENT_SET_BROWSEDPLAYER_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_SET_BROWSEDPLAYER_CNF,
                (U8 *) &Parms->p.ABTAVRCPSetBrowsePlayerCnf,
                sizeof(BTAVRCPSetBrowsePlayerCnf));
          break;
        case BTAVRCP_EVENT_GET_FOLDERITEMS_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_FOLDERITEMS_CNF,
                (U8 *) &Parms->p.ABTAVRCPGetFolderItemsCnf,
                sizeof(BTAVRCPGetFolderItemsCnf));
           break;
        case BTAVRCP_EVENT_CHANGE_PATH_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_CHANGE_PATH_CNF,
                (U8 *) &Parms->p.ABTAVRCPChangePathCnf,
                sizeof(BTAVRCPChangePathCnf));
            break;
        case BTAVRCP_EVENT_GET_ITEMATTRIBUTES_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_GET_ITEMATTRIBUTES_CNF,
                (U8 *) &Parms->p.ABTAVRCPGetItemAttrCnf,
                sizeof(BTAVRCPGetItemAttrCnf ));
            break;
        case BTAVRCP_EVENT_PLAY_ITEMS_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_PLAY_ITEMS_CNF,
                (U8 *) &Parms->p.ABTAVRCPPlayItemsCnf,
                sizeof(BTAVRCPPlayItemsCnf));
            break;
        case BTAVRCP_EVENT_SEARCH_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_SEARCH_CNF,
                (U8 *) &Parms->p.ABTAVRCPSearchCnf,
                sizeof(BTAVRCPSearchCnf));
            break;
        case BTAVRCP_EVENT_ADD_TONOWPLAYING_CNF:
            bt_avrcp_send_command_ind(
                MOD_MMI,
                MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_CNF,
                (U8 *) &Parms->p.ABTAVRCPAddNowPlayingCnf,
                sizeof(BTAVRCPAddNowPlayingCnf));
            break;
#if 0            
        case BTAVRCP_EVENT_SDP_QUERY_IND:
            bt_avrcp_send_sdp_query_ind(
                MOD_MMI,
                Parms->p.ABTAVRCPSdpQueryCnf.chnl_num,
                Parms->p.ABTAVRCPSdpQueryCnf.seq_id,
                Parms->p.ABTAVRCPSdpQueryCnf.status,
                Parms->p.ABTAVRCPSdpQueryCnf.tg_version,                
                Parms->p.ABTAVRCPSdpQueryCnf.remote_tg_support_feature,
                Parms->p.ABTAVRCPSdpQueryCnf.ct_version,                
                Parms->p.ABTAVRCPSdpQueryCnf.remote_ct_support_feature
                );
            break;
#endif            
        default:
        {
            // Assert(0);  /// AV13:add 
            // When receiving a unknow cmd, profile should reject it and doesn't pass it to upper layer.
            OS_Report("warning ignore the event:%d", Parms->event);
        }            
            break;

    }

}

/**
 * @brief Handle ILM message from ext-adapter layer
 *
 */
void avrcp_adp_message_handler(ilm_struct *ilm_ptr)
{
    void *msg_p = (void*)ilm_ptr->local_para_ptr;
    U8 u8Version, u8SdpFeature; 

    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_AVRCP_ACTIVATE_REQ:
            OS_Report("bt_avrcp_activate_req_structsize is:%d", sizeof(bt_avrcp_activate_req_struct) );
            if( ((bt_avrcp_activate_req_struct*) msg_p)->msg_len < 8 ){
                // use the default value
                u8SdpFeature = 0x01;
            }else{
                u8SdpFeature = ((bt_avrcp_activate_req_struct*) msg_p)->sdpfeature;
            }
            if( ((bt_avrcp_activate_req_struct*) msg_p)->msg_len < 6 ){
                // use the default compiled value
                u8Version = 0; 
            }else{
                u8Version = ((bt_avrcp_activate_req_struct*) msg_p)->version;
            }
           
            if( BTCoreVerifySysInitState() == FALSE )
            {
                OS_Report("[AVRCP] warning bt is off ");
                bt_avrcp_send_activate_cnf( MOD_MMI, 
                ((bt_avrcp_activate_req_struct*) msg_p)->chnl_num,
                BT_AVRCP_RESULT_BAD_STATE, 
                u8Version);
            }else{
            AVRCP_UitlActivateReq(
            	  ((bt_avrcp_activate_req_struct*) msg_p)->chnl_num,
            	  ((bt_avrcp_activate_req_struct*) msg_p)->local_role,
            	  u8Version,
            	  u8SdpFeature
            	  );
            }
            break;
        case MSG_ID_BT_AVRCP_DEACTIVATE_REQ:
            AVRCP_UitlDeactivateReq(((bt_avrcp_deactivate_req_struct*) msg_p)->chnl_num);
            break;
        case MSG_ID_BT_AVRCP_CONNECT_REQ:
            AVRCP_UitlConnectReq(
            	  ((bt_avrcp_connect_req_struct*) msg_p)->chnl_num,
            	  ((bt_avrcp_connect_req_struct*) msg_p)->local_role,
            	  &((bt_avrcp_connect_req_struct*) msg_p)->device_addr);
            break;
        case MSG_ID_BT_AVRCP_CONNECT_IND_RSP:
            AVRCP_UitlConnectIndRes(
            	  ((bt_avrcp_connect_ind_rsp_struct*) msg_p)->chnl_num,
            	  ((bt_avrcp_connect_ind_rsp_struct*) msg_p)->accept_conn);
            break;
        case MSG_ID_BT_AVRCP_DISCONNECT_REQ:
            AVRCP_UitlDisconnectReq(((bt_avrcp_disconnect_req_struct*) msg_p)->chnl_num);
            break;
        case MSG_ID_BT_AVRCP_CMD_FRAME_REQ:
            AVRCP_UitlCmdFrameReq(
            	  ((bt_avrcp_cmd_frame_req_struct*) msg_p)->chnl_num,
            	  ((bt_avrcp_cmd_frame_req_struct*) msg_p)->c_type,
            	  ((bt_avrcp_cmd_frame_req_struct*) msg_p)->subunit_type,
            	  ((bt_avrcp_cmd_frame_req_struct*) msg_p)->subunit_id,
            	  ((bt_avrcp_cmd_frame_req_struct*) msg_p)->data_len,
            	  ((bt_avrcp_cmd_frame_req_struct*) msg_p)->frame_data);
            break;
        case MSG_ID_BT_AVRCP_CMD_FRAME_IND_RSP:
            AVRCP_UitlCmdFrameRspInd(
            	  ((bt_avrcp_cmd_frame_ind_rsp_struct*) msg_p)->chnl_num,
            	  ((bt_avrcp_cmd_frame_ind_rsp_struct*) msg_p)->seq_id,
            	  ((bt_avrcp_cmd_frame_ind_rsp_struct*) msg_p)->c_type,
            	  ((bt_avrcp_cmd_frame_ind_rsp_struct*) msg_p)->subunit_type,
            	  ((bt_avrcp_cmd_frame_ind_rsp_struct*) msg_p)->subunit_id,
            	  ((bt_avrcp_cmd_frame_ind_rsp_struct*) msg_p)->data_len,
            	  ((bt_avrcp_cmd_frame_ind_rsp_struct*) msg_p)->frame_data);
            break;
        case MSG_ID_BT_AVRCP_BROWSE_CONNECT_REQ:
            AVRCP_UitlBrowseConnectReq(
            	  ((bt_avrcp_connect_req_struct*) msg_p)->chnl_num
            	  );
            break;
        case MSG_ID_BT_AVRCP_BROWSE_DISCONNECT_REQ:
            AVRCP_UitlBrowseDisconnectReq(
            	  ((bt_avrcp_connect_req_struct*) msg_p)->chnl_num
            	  );
            break;
        case MSG_ID_BT_AVRCP_GET_CAPABILITIES_RSP:
			{
				AvrcpChnlInfo myInfo;
				myInfo.chnl_num = ((bt_avrcp_get_capabilities_rsp_struct*) msg_p)->chnl_num;
				myInfo.transId = ((bt_avrcp_get_capabilities_rsp_struct*) msg_p)->seq_id;
				AVRCP_UtilAVRCPCmdCapabiltiesRsp(
				 &myInfo,
				 0,
				 (U16)((bt_avrcp_get_capabilities_rsp_struct*) msg_p)->count,
				 ((bt_avrcp_get_capabilities_rsp_struct*) msg_p)->capability
				);
			}
            break;
        case MSG_ID_BT_AVRCP_LIST_PLAYERAPP_ATTRIBUTE_RSP:
            AVRCP_UtilAVRCPCmdListPlayerAttributesRsp(
             (bt_avrcp_list_playerapp_attribute_rsp_struct*) msg_p
            );
            break;
        case MSG_ID_BT_AVRCP_LIST_PLAYERAPP_VALUE_RSP:
            AVRCP_UtilAVRCPCmdListPlayerValuesRsp(
             (bt_avrcp_list_playerapp_value_rsp_struct*) msg_p
            );
            break;
		case MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_RSP:
            AVRCP_UtilAVRCPGetCurPlayerValueRsp(
             (bt_avrcp_get_curplayerapp_value_rsp_struct*) msg_p
            );
		    break;
		case MSG_ID_BT_AVRCP_SET_PLAYERAPP_VALUE_RSP:
            AVRCP_UtilAVRCPCmdSetPlayerValueRsp(
             (bt_avrcp_set_playerapp_value_rsp_struct *) msg_p
            );
		    break;
		case MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_RSP:
            AVRCP_UtilAVRCPCmdGetPlayerAttrTextRsp(
             (bt_avrcp_get_playerapp_attributetext_rsp_struct*) msg_p
            );
		    break;
		case MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_RSP:
            AVRCP_UtilAVRCPGetPlayerValueTextRsp(
             (bt_avrcp_get_playerapp_valuetext_rsp_struct*) msg_p
            );
		    break;
		case MSG_ID_BT_AVRCP_INFORM_DISPLAY_CHARSET_RSP:
            AVRCP_UtilAVRCPCmdInformCharsetRsp(
             (bt_avrcp_inform_display_charset_rsp_struct*) msg_p
            );
		    break;
		case MSG_ID_BT_AVRCP_INFORM_BATTERY_STATUSOFCT_RSP:
            AVRCP_UtilAVRCPCmdInformBattStatusOfCTRsp(
             (bt_avrcp_inform_battery_statusofct_rsp_struct*) msg_p
            );
		    break;
		case MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_RSP:
            AVRCP_UtilAVRCPCmdGetElementAttributesRsp(
             (bt_avrcp_get_element_attributes_rsp_struct*) msg_p
            );
		    break;
		case MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_RSP:
            AVRCP_UtilAVRCPCmdGetPlayerStatusRsp(
             (bt_avrcp_get_playerstatus_rsp_struct*) msg_p
            );
		    break;
        case MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_RSP:
            AVRCP_UtilAVRCPCmdRegisterNotificationRsp(
            (bt_avrcp_register_notification_rsp_struct *) msg_p
            );
            break;
		case MSG_ID_BT_AVRCP_ABORT_CONTINUERESPONSE_RSP:
            AVRCP_UtilAVRCPCmdAbortContinueResponseRsp( 
				(bt_avrcp_abort_continueresponse_rsp_struct*) msg_p );
		    break;
		case MSG_ID_BT_AVRCP_SET_ABSOLUTE_VOLUME_RSP:
    		AVRCP_UtilAVRCPCmdSetAbsoluteVolumeRsp( 
				(bt_avrcp_set_absolute_volume_rsp_struct*) msg_p );
		    break;
		case MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_RSP:
            AVRCP_UtilAVRCPCmdSetAddressedPlayerRsp( 
				(bt_avrcp_set_addressedplayer_rsp_struct*) msg_p );
		    break;
		case MSG_ID_BT_AVRCP_GET_PLAYERITEMS_RSP:
		    AVRCP_UtilAVRCPCmdGetPlayerItemsRsp( 
				(bt_avrcp_get_playeritems_rsp_struct*) msg_p );
		    break;
		case MSG_ID_BT_AVRCP_GET_FOLDERITEMS_RSP:
			AVRCP_UtilAVRCPCmdGetFolderItemsRsp( 
				(bt_avrcp_get_folderitems_rsp_struct*) msg_p );
		    break;
		case MSG_ID_BT_AVRCP_SET_BROWSEDPLAYER_RSP:
			AVRCP_UtilAVRCPCmdSetBrowsedPlayerRsp( 
				(bt_avrcp_set_browsedplayer_rsp_struct*) msg_p );
		    break;
		case MSG_ID_BT_AVRCP_CHANGE_PATH_RSP:
			AVRCP_UtilAVRCPCmdChangePathRsp( 
				(bt_avrcp_change_path_rsp_struct *) msg_p );
		    break;
		case MSG_ID_BT_AVRCP_GET_ITEMATTRIBUTES_RSP:
			AVRCP_UtilAVRCPCmdGetItemAttributesRsp( 
				(bt_avrcp_get_itemattributes_rsp_struct*) msg_p );
		    break;
		case MSG_ID_BT_AVRCP_PLAY_ITEMS_RSP:
			AVRCP_UtilAVRCPCmdPlayItemsRsp( 
				(bt_avrcp_play_items_rsp_struct*) msg_p );
		    break;
		case MSG_ID_BT_AVRCP_SEARCH_RSP:
			AVRCP_UtilAVRCPCmdSearchRsp( 
				(bt_avrcp_search_rsp_struct*) msg_p );
		    break;
		case MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_RSP:
			AVRCP_UtilAVRCPCmdAddToNowPlayingRsp( 
				(bt_avrcp_add_tonowplaying_rsp_struct*) msg_p );
		    break;
 
        /***** AVRCP control msg *****/
		case    MSG_ID_BT_AVRCP_GET_CAPABILITIES_REQ:
		{
            AVRCP_UtilAVRCPSendCmd_Get_Capabilities_Req( 
            (bt_avrcp_get_capabilities_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_LIST_PLAYERAPP_ATTRIBUTE_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_List_Playerapp_Attribute_Req( 
		    (bt_avrcp_list_playerapp_attribute_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_LIST_PLAYERAPP_VALUE_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_List_Playerapp_Value_Req( 
		    (bt_avrcp_list_playerapp_value_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Get_Curplayerapp_Value_Req( 
		    (bt_avrcp_get_curplayerapp_value_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_SET_PLAYERAPP_VALUE_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Set_Playerapp_Value_Req( 
		    (bt_avrcp_set_playerapp_value_req_struct *) msg_p);		
		}
		    break;
		case    MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Get_Playerapp_Attributetext_Req( 
		    (bt_avrcp_get_playerapp_attributetext_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_REQ:
		{
            AVRCP_UtilAVRCPSendCmd_Get_Playerapp_Valuetext_Req( 
            (bt_avrcp_get_playerapp_valuetext_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_INFORM_DISPLAY_CHARSET_REQ:
		{
            AVRCP_UtilAVRCPSendCmd_Inform_Display_Charset_Req( 
            (bt_avrcp_inform_display_charset_req_struct *) msg_p);		
		}
		    break;
		case    MSG_ID_BT_AVRCP_INFORM_BATTERY_STATUSOFCT_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Inform_Battery_Statusofct_Req( 
	    	(bt_avrcp_inform_battery_statusofct_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Get_Element_Attributes_Req( 
    		(bt_avrcp_get_element_attributes_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Get_Playerstatus_Req( 
		    (bt_avrcp_get_playerstatus_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_REQ:
		{
    		AVRCP_UtilAVRCPSendCmd_Register_Nofitication_Req( 
    		(bt_avrcp_register_notification_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_ABORT_CONTINUERESPONSE_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Abort_Continueresponse_Req( 
		    (bt_avrcp_abort_continueresponse_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_SET_ABSOLUTE_VOLUME_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Set_Absolute_Volume_Req( 
		    (bt_avrcp_set_absolute_volume_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_SET_ADDRESSEDPLAYER_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Set_Addressedplayer_Req( 
		    (bt_avrcp_set_addressedplayer_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_GET_PLAYERITEMS_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Get_Folderitems_Req( 
		    (bt_avrcp_get_folderitems_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_GET_FOLDERITEMS_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Get_Folderitems_Req( 
		    (bt_avrcp_get_folderitems_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_SET_BROWSEDPLAYER_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Set_Browsedplayer_Req( 
		    (bt_avrcp_set_browsedplayer_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_CHANGE_PATH_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Change_Path_Req( 
		    (bt_avrcp_change_path_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_GET_ITEMATTRIBUTES_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Get_Itemattributes_Req( 
		    (bt_avrcp_get_itemattributes_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_PLAY_ITEMS_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Play_Items_Req( 
		    (bt_avrcp_play_items_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_SEARCH_REQ:
		{
		    AVRCP_UtilAVRCPSendCmd_Search_Req( 
	    	(bt_avrcp_search_req_struct *) msg_p);
		}
		    break;
		case    MSG_ID_BT_AVRCP_ADD_TONOWPLAYING_REQ:
		{
    		AVRCP_UtilAVRCPSendCmd_Add_Tonowplaying_Req( 
		    (bt_avrcp_add_tonowplaying_req_struct *) msg_p);
		}
		    break;
        case MSG_ID_BT_AVRCP_SDP_QUERY_REQ:
            AVRCP_UtilAVRCPSDP_Query_Req(
                (bt_avrcp_sdp_query_req_struct *) msg_p);
            break;
        case MSG_ID_BT_AVRCP_TEST_CMD_REQ:
            // send to MMI
            OS_Report("[AVRCP] forward cmd ");
            bt_avrcp_send_test_cmd_req((bt_test_cmd_req_struct *) msg_p);
            break;
        default:
			AvrcpAssert(0); // AV13:add
            break;
    }
}

#endif /* __BT_AVRCP_PROFILE__ */
