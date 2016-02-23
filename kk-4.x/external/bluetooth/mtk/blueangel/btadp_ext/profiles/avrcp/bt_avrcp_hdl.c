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
#include "bluetooth_avrcp_struct.h"
#include "bluetooth_avrcp_message.h"
#include "bt_mmi_msg.h"
#include "bt_avrcp_api.h"

#include "bt_ext_debug.h"

static U8 g_avrcp_norelease_handle = FALSE;
static U8 g_avrcp_pressed_keycode = 0; // if remote disconnects before sending release key, use this to send twice
static U8 g_avrcp_browsing_connected = FALSE;
static U8 g_avrcp_connected_pts = FALSE;

static U8 g_avrcp_version = 14;
static U8 g_avrcp_sdpfeature = 0x01;

/* Save each AVRCP msg ind & req id's seq number */
U8 g_avrcp_seq_id_database[MSG_ID_BT_AVRCP_GROUP_END - MSG_ID_BT_AVRCP_GROUP_START + 1]; // save msg ids
U8 g_avrcp_req_event_database[0x0f]; // save reg event ids
U8 g_avrcp_auto_browse_connect = FALSE;
S32 g_avrcp_internal_get_playerstatus = 0;
U8 g_avrcp_player_play_status = 0;
U8 g_avrcp_addr[6]; // bluetooth address BD_ADDR
U8 g_avrcp_device_name[32]; // keep name of device

/* btif_rc.c */
int btrc_handle_get_playerstatus_ind();
int btrc_handle_register_notification_ind(uint32_t event_id, uint32_t param);
int btrc_handle_get_element_attributes(uint8_t num_attr, uint32_t *p_attrs);

void initLocalSaveSeqId(){
	memset( g_avrcp_seq_id_database, 0, sizeof(g_avrcp_seq_id_database) );
	memset( g_avrcp_req_event_database, 0, sizeof(g_avrcp_req_event_database) );
	
	g_avrcp_internal_get_playerstatus = 0;
}

void *avrcp_util_get_ilm_data(ilm_struct *message){
#ifndef BTMTK_ON_WISE 
	return (void *)message->ilm_data;
#else
	return (void *)message->local_para_ptr;
#endif
} 

void saveSeqId(int msg_id, U8 seq_id){
	if( (unsigned int) msg_id > MSG_ID_BT_AVRCP_GROUP_END || (unsigned int) msg_id < MSG_ID_BT_AVRCP_GROUP_START ){
		bt_ext_log("[BT][AVRCP][JNI] fail to save seq_id for msg_id:%d seq_id:%d", msg_id, seq_id);
		return;
	}
	
	msg_id = msg_id - MSG_ID_BT_AVRCP_GROUP_START;
	if( (unsigned int)msg_id >= sizeof( g_avrcp_seq_id_database ) ){
		bt_ext_log("[BT][AVRCP][JNI] seq_id_databse too small msg_id:%d seq_id:%d", msg_id, seq_id);
		return;
	}
	
	g_avrcp_seq_id_database[msg_id] = seq_id;
}

U8 getSavedSeqId(int msg_id){
	if( (unsigned int) msg_id > MSG_ID_BT_AVRCP_GROUP_END || (unsigned int) msg_id < MSG_ID_BT_AVRCP_GROUP_START ){
		bt_ext_log("[BT][AVRCP][JNI] fail to save seq_id for msg_id:%d", msg_id);
		return 0;
	}
	
	msg_id = msg_id - MSG_ID_BT_AVRCP_GROUP_START;
	if( (unsigned int)msg_id >= sizeof( g_avrcp_seq_id_database ) ){
		bt_ext_log("[BT][AVRCP][JNI] seq_id_databse too small msg_id:%d ", msg_id);
		return 0;
	}
	
    return g_avrcp_seq_id_database[msg_id];
}

#if 0
typedef struct
{
    kal_uint32 lap;
    kal_uint8 uap;
    kal_uint16 nap;
} bt_addr_struct;
#endif
static void btmtk_util_convert_string2bdaddr(const char *source, bt_addr_struct *dest)
{
    unsigned char addr[6];
    int i;
    char *ptr = (char*)source;

    //assert(dest != NULL && source != NULL);
    bt_ext_log("convert from %s", ptr);
    for (i = 5; i >= 0; i--)
    {
        addr[i] = strtoul(ptr, &ptr, 16);
        ptr++;
        bt_ext_log("convert to addr[%d]=0x%X", i, addr[i]);
    }
    dest->lap = addr[0] + (addr[1] << 8) + (addr[2] << 16);
    dest->uap = addr[3];
    dest->nap = addr[4] + (addr[5] << 8);
    bt_ext_log("[API] btmtk_util_convert_string2bdaddr: %s => (0x%06X, 0x%02X, 0x%04X)", source, (unsigned int)dest->lap, dest->uap, dest->nap);
}

#define AVRCP_EVENT_PLAYBACK_STATUS_CHANGED                0x01
#define AVRCP_EVENT_TRACK_CHANGED                          0x02
#define AVRCP_EVENT_TRACK_REACHED_END                      0x03
#define AVRCP_EVENT_TRACK_REACHED_START                    0x04
#define AVRCP_EVENT_PLAYBACK_POS_CHANGED                   0x05
#define AVRCP_EVENT_BATT_STATUS_CHANGED                    0x06
#define AVRCP_EVENT_SYSTEM_STATUS_CHANGED                  0x07
#define AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED     0x08
#define AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED            0x09
#define AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED              0x0a
#define AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED               0x0b
#define AVRCP_EVENT_UIDS_CHANGED                           0x0c
#define AVRCP_EVENT_VOLUME_CHANGED                         0x0d

void saveRegEventSeqId(U8 event_id, U8 seq_id ){
	if( event_id >= sizeof(g_avrcp_req_event_database) ){
		ALOGE("[BT][AVRCP][JNI] seq_id_databse doesn't have event_id:%d ", event_id);
		return;
	}
	g_avrcp_req_event_database[event_id] = seq_id;
}

U8 getSavedRegEventSeqId(U8 event_id){
	if( event_id > sizeof(g_avrcp_req_event_database) ){
		return 0;
	}
	return g_avrcp_req_event_database[event_id];
}


static int check_norelease_handle(const char* device_name){
	if( 0 == strcmp( (const char *) device_name, "Nokia CK-20W" ) ) {
		// has the same 
		g_avrcp_norelease_handle = TRUE;
	}
	else if( 0 == strcmp( (const char *) device_name, "i.Tech Clip Radio" ) ){
		g_avrcp_norelease_handle = FALSE; // for testing
	}
    else if( 0 == strcmp( (const char *) device_name, "MW600" ) ){
		//g_avrcp_norelease_handle = TRUE; // MW600 wait for release for more 
	}
	else{
		g_avrcp_norelease_handle = FALSE;
	}
	
	if( 0 == strncmp( (const char *) device_name, "PTS", 3 ) ) {
		g_avrcp_connected_pts = TRUE;
		bt_ext_log("[BT][AVRCP] pts connected");
	}else{
		g_avrcp_connected_pts = FALSE;
	}
	
	bt_ext_log("[BT][AVRCP] check_norelease normal mode");	
	return TRUE;
}

static void CONVERT_BDADDR2ARRAY(U8 *dest, bt_addr_struct *source)
{
    dest[0] = (U8) (source->lap & 0x000000FF);
    dest[1] = (U8) ((source->lap & 0x0000FF00) >> 8);
    dest[2] = (U8) ((source->lap & 0x00FF0000) >> 16);
    dest[3] = source->uap;
    dest[4] = (U8) (source->nap & 0x00FF);
    dest[5] = (U8) ((source->nap & 0xFF00) >> 8);

}

void btmtk_avrcp_handle_message(ilm_struct *message)
{
    int i = 0;
    AvrcpChannelContext apCntx;
    AvrcpChannelContext *cntx = &apCntx;
    
	switch(message->msg_id)
	{
	        /* common message */
	    case MSG_ID_BT_AVRCP_ACTIVATE_CNF:
			initLocalSaveSeqId();
			
	        break;
	    case MSG_ID_BT_AVRCP_DEACTIVATE_CNF:
	
	        break;
	    case MSG_ID_BT_AVRCP_CONNECT_IND:
	    	{
	            bt_avrcp_connect_ind_struct *ind;
	            ind = (bt_avrcp_connect_ind_struct *)message->ilm_data;
	            
	            /* for IOT NOKIA doesn't send release key */
	            check_norelease_handle( (const char *)ind->device_name);	    	
                memset( g_avrcp_device_name, 0, sizeof(g_avrcp_device_name));
                memcpy( g_avrcp_device_name, ind->device_name, sizeof(g_avrcp_device_name)-1);
	            
	            // init the SeqId
		    	initLocalSaveSeqId();
		    	
		    	btmtk_avrcp_send_connect_ind_rsp(0, ind->chnl_num, 1);
	    	}
			break;
	    case MSG_ID_BT_AVRCP_CONNECT_CNF:
	    	{
	            bt_avrcp_connect_cnf_struct *ind;
	            ind = (bt_avrcp_connect_cnf_struct *)message->ilm_data;
	            
	            if( ind->result == 0x1000 ){//success
	            	bt_ext_log("[BT][AVRCP] MSG_ID_BT_AVRCP_CONN cnf addr nap:0x%x", ind->device_addr.nap, ind->device_addr.uap);
	            	CONVERT_BDADDR2ARRAY( &g_avrcp_addr, &ind->device_addr);
	            	
	            }else{
	            	//
	            	bt_ext_log("[BT][AVRCP] MSG_ID_BT_AVRCP_CONN cnf fail:0x%x", ind->result);
	            }
			}
	    case MSG_ID_BT_AVRCP_DISCONNECT_CNF:
			//break;			
		case MSG_ID_BT_AVRCP_DISCONNECT_IND:
			{
			bt_avrcp_disconnect_ind_struct* ind;
			ind = (bt_avrcp_disconnect_ind_struct *)message->ilm_data;
			
			// CR ALPS00049181 
			if( 0 != g_avrcp_pressed_keycode ){
			   bt_ext_log("[BT][AVRCP] MSG_ID_BT_AVRCP_DISC pressed_keycode:0x%x", g_avrcp_pressed_keycode);
			   avrcp_input_sendkey(g_avrcp_pressed_keycode, 0);
			   g_avrcp_pressed_keycode = 0; // avoid connect/disconect and send again.
			}
			
			g_avrcp_browsing_connected = FALSE;
			g_avrcp_auto_browse_connect= FALSE;
            }			
			break;
        case MSG_ID_BT_AVRCP_CMD_FRAME_IND:
                   {
                       int key = 0;
                       int press = 0;
                       bt_avrcp_cmd_frame_ind_struct* ind;
                       ind = (bt_avrcp_cmd_frame_ind_struct *)message->ilm_data;
                       
                       saveSeqId( MSG_ID_BT_AVRCP_CMD_FRAME_IND, ind->seq_id);

                       bt_ext_log("[BT][AVRCP] MSG_ID_BT_AVRCP_CMD_FRAME_IND size:%d rawkey:%d data_len:%d", ind->data_len, key, ind->data_len);
                       
                       // response ok Immediately
                       if( 3 == ind->data_len )
                       {
                        
                            press = ((ind->frame_data[1] & 0x80) == 0x80 )?0:1;
                            key = ind->frame_data[1] & 0x7f; // remove the press

                            // Pass to AP or send it to nInput
                            if( 1 == avrcp_is_ready() ){
                            	if ( g_avrcp_norelease_handle ){
                            		if( press == 1 ){
                            			avrcp_input_sendkey(key, 1);
                            			// this is no release. so send the release key when sending press key
                            			avrcp_input_sendkey(key, 0);
                            		}
                            	}else{
	                            	// only send in pressed state
	                            	if( press == 0 ){
	                            		avrcp_input_sendkey(key, 0);
	                            		// CR ALPS00049181 
	                            		g_avrcp_pressed_keycode = 0;
	                            	}else{
	                            		// press
	                            		avrcp_input_sendkey(key, 1);
	                            		// CR ALPS00049181 "disconnect before release key" causes key driver repeats the key
	                            		g_avrcp_pressed_keycode = key;										
	                            	}
	                            }
                            	
                            }else{
	                            // pass to nInput
                                
	                        }

							// send response as success
                            btmtk_avrcp_send_pass_through_rsp(
			                        0,
			                        ind->chnl_num,
			                        ind->seq_id,
			                        ind->profile_id,
			                        BT_AVRCP_CR_ACCEPT,
			                        BT_AVRCP_SUBUNIT_TYPE_PASS_THROUGH,
			                        BT_AVRCP_SUBUNIT_ID_PASS_THROUGH,
			                        ind->data_len,
			                        ind->frame_data
			                        );
							 
							
                        }else if( 8 == ind->data_len ){
                        	// group key not support yet
                        	btmtk_avrcp_send_pass_through_rsp(
                               0,
                            ind->chnl_num,
                            ind->seq_id,
                            ind->profile_id,
                            BT_AVRCP_CR_NOT_IMPLEMENT,
                            BT_AVRCP_SUBUNIT_TYPE_PASS_THROUGH,
                            BT_AVRCP_SUBUNIT_ID_PASS_THROUGH,
                            ind->data_len,
                            ind->frame_data
                            );
                            
                            press = ((ind->frame_data[1] & 0x80) == 0x80 )?1:0; // 254: press 126: release
                            key = ind->frame_data[7]; // 0 next_group 1 prev_group
                            if( ind->frame_data[3] != 0 || ind->frame_data[4] != 0x19 || ind->frame_data[5] != 0x58){
	                            for( i = 0; i< 16 && i < ind->data_len ; i++){
	                            	bt_ext_log("[BT][AVRCP] dump i:%d  vaule:0x%x", i, ind->frame_data[i]);
	                            }
                            }
                            
                        	
                       	}else{
                           btmtk_avrcp_send_pass_through_rsp(
                               0,
                            ind->chnl_num,
                            ind->seq_id,
                            ind->profile_id,
                            BT_AVRCP_CR_NOT_IMPLEMENT,
                            BT_AVRCP_SUBUNIT_TYPE_PASS_THROUGH,
                            BT_AVRCP_SUBUNIT_ID_PASS_THROUGH,
                            ind->data_len,
                            ind->frame_data
                            );
                            if( ind->data_len > 8 && ind->frame_data[0] == 0x7C && ((ind->frame_data[1]&0x7F) == 0x7E) ){
                                // Group Navigation is not support
                                bt_ext_log("[BT][AVRCP] return no support group-navigation");
                            }else{
                                bt_ext_log("[BT][AVRCP] unknow indication - dump first 16 hex value. ");
                                for( i = 0; i< 16 && i < ind->data_len ; i++){
                                    bt_ext_log("[BT][AVRCP] dump i:%d  vaule:0x%x", i, ind->frame_data[i]);
                                }
                            }
                        }
                   }
          break;
      case MSG_ID_BT_AVRCP_GET_CAPABILITIES_IND:
      	// return the capability automatically
      	{
      			bt_avrcp_get_capabilities_ind_struct *msg;
                U8 events[13], count = 0;
	            msg = (bt_avrcp_get_capabilities_ind_struct *)avrcp_util_get_ilm_data(message);
      		        		  
		    	events[count++] = 1;  // playstatus change
			    events[count++] = 2;  // track change
			    events[count++] = 5;  // pos
			    if( msg != 0 ){
			        memset( &apCntx, 0, sizeof(apCntx));
			        apCntx.seq_id = msg->seq_id;
			        btmtk_avrcp_send_get_capabilities_rsp(&apCntx , 0, count, events);
			    }
		    }
      	break;
      case MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_IND:
      	{
      	    // pass to porting layer
      	    
      			bt_avrcp_get_element_attributes_ind_struct *ind;
                U8 events[13], count;
                U8 i = 0;
                
	            ind = (bt_avrcp_get_element_attributes_ind_struct *)avrcp_util_get_ilm_data(message);
	            saveSeqId( message->msg_id, ind->seq_id);

                bt_ext_log("[BT][AVRCP] ind MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_IND ");
                if( ind->count == 0 ){
                    // change it to all. pts TC_TG_MDI_BV_04_C
                    for( i = 0; i< 7; i++){
                        ind->attributes[i] = i+1;
                        ind->count++;
                    }
                }
                btrc_handle_get_element_attributes( ind->count, ind->attributes);
        }
      	break;
      case MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_IND:
      	{
                bt_avrcp_get_playerstatus_ind_struct *ind;
                
	            ind = (bt_avrcp_get_playerstatus_ind_struct *)avrcp_util_get_ilm_data(message);
	            saveSeqId( message->msg_id, ind->seq_id);
         
                bt_ext_log("[BT][AVRCP] ind MSG_ID_BT_AVRCP_GET_PLAYERSTATUS_IND ");
                btrc_handle_get_playerstatus_ind();       
      	}
      	break;
        case MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_IND:
      	{
      	        AvrcpChannelContext avrcpCntx;
                AvrcpChannelContext *cntx = &avrcpCntx;
                int interim = 0;        
    
      			bt_avrcp_register_notification_ind_struct *ind;
      			bt_avrcp_register_notification_rsp_struct local_rsp;
                ind = (bt_avrcp_register_notification_ind_struct *)avrcp_util_get_ilm_data(message);
				
				bt_ext_log("[BT][AVRCP] ind MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_IND ");
				saveSeqId( message->msg_id, ind->seq_id);
				saveRegEventSeqId( ind->event_id, ind->seq_id);
				
				if( 0 == g_avrcp_internal_get_playerstatus ){
				    g_avrcp_internal_get_playerstatus++;
				    if( 1 == btrc_handle_get_playerstatus_ind() ){
				        //fail
				        g_avrcp_internal_get_playerstatus--;
				    }
				}
				
				if( 1 ==  btrc_handle_register_notification_ind( ind->event_id, ind->playback_interval) ){
				    // fail !
				    memset( &local_rsp, 0, sizeof(local_rsp));
				    local_rsp.reject = 1;
				    local_rsp.error_code = 0;
				    
				    cntx->seq_id = ind->seq_id;
 				    local_rsp.seq_id = ind->seq_id; 
				    local_rsp.is_interim = 0;
				    local_rsp.event_id = ind->event_id;
				    local_rsp.pos = local_rsp.volume = ind->playback_interval;
				    bt_ext_log("[BT][AVRCP] warning avrpc pass to rc fail ! message->msg:%d event_id:%d", message->msg_id, ind->event_id);
				    btmtk_avrcp_send_reg_notievent_rsp( cntx, &local_rsp); // 1 is fail
				}
				
        }
      	break;
        case MSG_ID_BT_AVRCP_LIST_PLAYERAPP_ATTRIBUTE_IND:
               {
                bt_avrcp_list_playerapp_attribute_ind_struct* ind = 0;
				ind = (bt_avrcp_list_playerapp_attribute_ind_struct *)avrcp_util_get_ilm_data(message);
                
                cntx->seq_id = ind->seq_id;
                saveSeqId( message->msg_id, ind->seq_id);
                
                //void btmtk_avrcp_send_list_player_attrs_rsp(AvrcpChannelContext *cntx, U8 error, U8 count, U8 *attr_ids){
                btmtk_avrcp_send_list_player_attrs_rsp( cntx, BT_AVRCP_ERRCODE_INVALID_CMD, 0, 0);
		       }
               break;
           case MSG_ID_BT_AVRCP_LIST_PLAYERAPP_VALUE_IND:
               {
					// one byte 
					bt_avrcp_list_playerapp_value_ind_struct* ind;
					ind = (bt_avrcp_list_playerapp_value_ind_struct *)avrcp_util_get_ilm_data(message);
					saveSeqId( message->msg_id, ind->seq_id);
					
				    //btmtk_avrcp_send_list_player_values_rsp(AvrcpChannelContext *cntx, U8 error, U8 attr_id, U8 count, U8 *values){
				    btmtk_avrcp_send_list_player_values_rsp( cntx, BT_AVRCP_ERRCODE_INVALID_CMD, 0, 0, 0);
		       }
               break;
           case MSG_ID_BT_AVRCP_GET_CURPLAYERAPP_VALUE_IND:
               {
					// one byte, one byte array
					bt_avrcp_get_curplayerapp_value_ind_struct* ind;
					ind = (bt_avrcp_get_curplayerapp_value_ind_struct *)avrcp_util_get_ilm_data(message);
					saveSeqId( message->msg_id, ind->seq_id);
					
		            //void btmtk_avrcp_send_get_curplayer_value_rsp(AvrcpChannelContext *cntx, U8 error, U8 count, U8 *attr_ids, U8 *values){
		            btmtk_avrcp_send_get_curplayer_value_rsp( cntx, BT_AVRCP_ERRCODE_INVALID_CMD, 0, 0, 0);
               }
               break;
           case MSG_ID_BT_AVRCP_SET_PLAYERAPP_VALUE_IND:
               {
					
					bt_avrcp_set_playerapp_value_ind_struct* ind;
					ind = (bt_avrcp_set_playerapp_value_ind_struct *)avrcp_util_get_ilm_data(message);
					saveSeqId( message->msg_id, ind->seq_id);
					
					//btmtk_avrcp_send_set_player_value_rsp(AvrcpChannelContext *cntx, U8 error){
					btmtk_avrcp_send_set_player_value_rsp( cntx, BT_AVRCP_ERRCODE_INVALID_CMD);
			   }
		       break;
           case MSG_ID_BT_AVRCP_GET_PLAYERAPP_ATTRIBUTETEXT_IND:
               {
					bt_avrcp_get_playerapp_attributetext_ind_struct* ind;
					ind = (bt_avrcp_get_playerapp_attributetext_ind_struct *)avrcp_util_get_ilm_data(message);
					saveSeqId( message->msg_id, ind->seq_id);
					
					//void btmtk_avrcp_send_get_player_attr_text_rsp(AvrcpChannelContext *cntx, U8 error, U8 index, U8 total, U8 attr_id, U16 charset, U8 strlen, U8 *strText){
					btmtk_avrcp_send_get_player_attr_text_rsp( cntx, BT_AVRCP_ERRCODE_INVALID_CMD, 0, 0, 0, 0, 0, 0);
               }
               break;
           case MSG_ID_BT_AVRCP_GET_PLAYERAPP_VALUETEXT_IND:
               {
					bt_avrcp_get_playerapp_valuetext_ind_struct* ind;
					ind = (bt_avrcp_get_playerapp_valuetext_ind_struct *)avrcp_util_get_ilm_data(message);
					saveSeqId( message->msg_id, ind->seq_id);
					
					//btmtk_avrcp_send_get_player_value_text_value_rsp(AvrcpChannelContext *cntx, U8 error, U8 index, U8 total, U8 attr_id, U8 value_id, U16 charset, U8 strlen, U8 *strText){
					btmtk_avrcp_send_get_player_value_text_value_rsp( cntx, BT_AVRCP_ERRCODE_INVALID_CMD, 0, 0, 0, 0, 0, 0, 0);
               }
               break;
           case MSG_ID_BT_AVRCP_INFORM_DISPLAY_CHARSET_IND:
               {
					bt_avrcp_inform_display_charset_ind_struct* ind;
					ind = (bt_avrcp_inform_display_charset_ind_struct *)avrcp_util_get_ilm_data(message);
					saveSeqId( message->msg_id, ind->seq_id);
					
				    //btmtk_avrcp_send_inform_charsetset_rsp(AvrcpChannelContext *cntx, U8 error){
				    btmtk_avrcp_send_inform_charsetset_rsp( cntx, BT_AVRCP_ERRCODE_INVALID_CMD);
               }
               break;
           case MSG_ID_BT_AVRCP_INFORM_BATTERY_STATUSOFCT_IND:
               {
                   bt_avrcp_inform_battery_statusofct_ind_struct* ind;
                   ind = (bt_avrcp_inform_battery_statusofct_ind_struct *)avrcp_util_get_ilm_data(message);
                   saveSeqId( message->msg_id, ind->seq_id);
                   
                   //void btmtk_avrcp_send_battery_status_rsp(AvrcpChannelContext *cntx, U8 error){
                   btmtk_avrcp_send_battery_status_rsp( cntx, BT_AVRCP_ERRCODE_INVALID_CMD);
               }
               break;
           case MSG_ID_BT_AVRCP_SDP_QUERY_IND:
               {
                   // udpate the remote's sdp information
                   bt_avrcp_sdp_query_ind_struct* ind;
                   ind = (bt_avrcp_sdp_query_ind_struct *)avrcp_util_get_ilm_data(message);
                   bt_ext_log("[BT][AVRCP] status:%d tg_version:0x%0x ct_version:0x%0x", ind->status, ind->tg_version, ind->ct_version);
                   bt_ext_log("[BT][AVRCP] status:%d remote_tg_support_feature:0x%0x remote_ct_support_feature:0x%0x", ind->status, ind->remote_tg_support_feature, ind->remote_ct_support_feature);

					ind->remote_ct_support_feature &= (~0x02);
					bt_ext_log("[BT][AVRCP]change remote_ct_support_feature:0x%x",ind->remote_ct_support_feature);
                   // workaround for MW600 sdp query
                   if( 0 == strncmp("MW600", g_avrcp_device_name, 6) ){
                        // change ct version to 0x0104
                        ind->ct_version = 0x0104;
                        ind->remote_ct_support_feature |= 0x02; //set as category2 to support BTRC_FEAT_ABSOLUTE_VOLUME
                        bt_ext_log("[BT][AVRCP] match MW600 v1.4 device ct_feature:0x%x", ind->remote_ct_support_feature);
                   }
                   if( 0 == strncmp("SBH50", g_avrcp_device_name, 6) ){
                        // change ct version to 0x0104
                        ind->ct_version = 0x0104;
                        ind->remote_ct_support_feature |= 0x02; //set as category2 to support BTRC_FEAT_ABSOLUTE_VOLUME                        
                        bt_ext_log("[BT][AVRCP] match SBH50 v1.4 device");                        
                   }             
                   if( 0 == strncmp("PTS-", g_avrcp_device_name, 6) ){
                        // change ct version to 0x0104
                        bt_ext_log("[BT][AVRCP] match PTS v1.4 device");                        
                   }             
                    
                   btrc_handle_rc_features(g_avrcp_addr, ind->ct_version, ind->remote_ct_support_feature);
                   
               }
               break;
            case MSG_ID_BT_AVRCP_SET_ABSOLUTE_VOLUME_CNF:
                {
                   bt_avrcp_set_absolute_volume_cnf_struct* ind;
                   ind = (bt_avrcp_set_absolute_volume_cnf_struct *)avrcp_util_get_ilm_data(message);
                   
                   bt_ext_log("[BT][AVRCP] SET_VOLUME_CNF seq_id:%d result:%d error:%d volume:%d", ind->seq_id, ind->result, ind->error_code, ind->volume);
                   
                   handle_rc_metamsg_cmd_set_volume_cnf(ind);
                    
                }
                break;
            case MSG_ID_BT_AVRCP_REGISTER_NOTIFICATION_CNF:
                {
                    bt_avrcp_register_notification_cnf_struct* ind;
                    ind = (bt_avrcp_register_notification_cnf_struct *)avrcp_util_get_ilm_data(message);
                    bt_ext_log("[BT][AVRCP] REG_NITIFICATION_CNF event_id:%d interim:%d", ind->event_id, ind->is_interim);
                    bt_ext_log("[BT][AVRCP] REG_NITIFICATION_CNF seq_id:%d result:%d error:%d", ind->seq_id, ind->result, ind->error_code, ind->volume);
                    
                    handle_rc_metamsg_cmd_reg_cnf(ind);
                }
                break;
            case MSG_ID_BT_AVRCP_TEST_CMD_REQ:
                {
                     // accept the test command and use easy process
                     bt_avrcp_test_cmd_req_struct *ind;
                     U8 *string = 0;
                     static U8 volume = 1;
                     bt_addr_struct addr = { 0x445566, 0x2233, 0x11 };
                     //U8 addr[6] = {0x66, 0x55, 0x44, 0x33, 0x22, 0x11 };
                     
                     ind = (bt_avrcp_test_cmd_req_struct *)avrcp_util_get_ilm_data(message);

                     bt_ext_log("[BT][AVRCP] bt_test_cmd_req_struct cmd=(%s)", ind->test_cmd_content, strlen(ind->test_cmd_content));

                     string = ind->test_cmd_content;
                     if( 0 == strncmp( "avrcp conn", string, 10) ){
                        // connect to 
                        if( strlen(string) > (12+6-1) ){
                            btmtk_util_convert_string2bdaddr( string+11, &addr);
                            bt_ext_log("[BT][AVRCP] bt_test_cmd_req_struct addr (%s) (0x%x 0x%x 0x%x)", string+11, addr.nap, addr.uap, addr.lap);
                        }
                        
                        btmtk_avrcp_send_connect_req( cntx, 0, &addr, 0);  
                     }
                     else if( 0 == strncmp( "avrcp disc", string, 10) ){
                        // disconect
                        btmtk_avrcp_send_disconnect_req( cntx, 0);
                     }
                     else if( 0 == strncmp( "avrcp setv", string, 10) ){
                        // set volumn
                        //btmtk_avrcp_send_set_absolute_volume_req( cntx, volume++);
                        if( strlen(string) > 11 ){
                            volume = atoi(string+10);
                        }else{
                            volume = ((volume+10)%100);
                        }
                        bt_ext_log("[BT][AVRCP] volume:%d", volume);
                        set_volume( volume );
                     }
                     else if( 0 == strncmp( "avrcp regv", string, 10) ){
                        // register volumn
                        btmtk_avrcp_send_reg_notievent_volume_changed_req( cntx);
                     }
                     
                }
                break;
	    	default:
				bt_ext_log("[BT][AVRCP] warning no handle !! message->msg:%d", message->msg_id);
	        break;
	
	}
}
