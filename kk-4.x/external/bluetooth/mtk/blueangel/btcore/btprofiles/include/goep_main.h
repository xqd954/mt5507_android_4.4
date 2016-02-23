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

/*****************************************************************************
 *
 * Filename:
 * ---------
 * goep_main.h
 *
 * Project:
 * --------
 *   SUPERMAN6229
 *
 * Description:
 * ------------
 *   This is header file of the goep interface for OPP and FTP profile
 *
 * Author:
 * -------
 * Attar Hsu
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Log$
 *
 * Sep 2 2008 mbj06032
 * [MAUI_01106205] [BT GOEP] Send OPP Client SDP result (OPP server's supported format list) to MMI
 * 
 *
 * Sep 28 2007 mbj06032
 * [MAUI_00470068] [BT][GOEP] when GET/PUT failed, tell application the response code from server
 * tell the application the detail failed rsp code from server
 *
 * Apr 20 2007 mtk00560
 * [MAUI_00384677] [MMI][BT] to check-in for CR423159
 * 
 *
 * Jan 2 2007 mtk00511
 * [MAUI_00355578] [BT][GOEP] Remove compile warning
 * 
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Nov 13 2006 mtk00511
 * [MAUI_00343002] [GOEP] Add OBEX authentication support
 * 
 *
 * Oct 3 2006 mtk00511
 * [MAUI_00332583] [1]ASSERT fail:0 BTMMIFtp.c 1514-MMI
 * 
 *
 * Sep 17 2006 mtk00511
 * [MAUI_00330296] [GOEP]Fix OPP IOT issue with Widcomm
 * 
 *
 * Sep 12 2006 mtk00511
 * [MAUI_00329160] [GOEP]Check in GOEP profile and OBEX stack
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef _GOEP_MAIN_H
#define _GOEP_MAIN_H

/// OPP and FTP Profile invokes this directly without ilm
void goep_event_handler(U32 msg_id, void* data);

/// OPP and FTP profile receives event through this function. (for backward compatible)
extern void goep_approfile_handler(U32 msg_id, void* data, U32 size);
extern void bt_opp_profile_event_hdlr(U32 msg_id, void* data, U32 u4Size);
extern void bt_ftp_profile_event_hdlr(U32 msg_id, void* data, U32 u4Size);

/// Handler function - request msg from OPP or FTP
extern void goep_register_server_req_hdlr(goep_register_server_req_struct* register_server_req);
extern void goep_deregister_server_req_hdlr(goep_deregister_server_req_struct* deregister_server_req);
extern void goep_connect_req_hdlr(goep_connect_req_struct* goep_connect_req);
extern void goep_send_authorize_ind_msg(U8 goep_conn_id, goep_bd_addr_struct* bd_addr_p, U8* dev_name);
extern void goep_auth_req_hdlr(goep_auth_req_struct* goep_auth_req );
extern void goep_push_req_hdlr(goep_push_req_struct* goep_push_req );
extern void goep_pull_req_hdlr(goep_pull_req_struct* goep_pull_req);
extern void goep_set_folder_req_hdlr(goep_set_folder_req_struct* goep_set_folder_req);
extern void goep_abort_req_hdlr(goep_abort_req_struct* goep_abort_req );
extern void goep_disconnect_req_hdlr(goep_disconnect_req_struct* goep_disconnect_req);
extern void goep_TpDisconnect_req_hdlr(goep_disconnect_req_struct* goep_disconnect_req);
/// Handler function - response msg from OPP or FTP
extern void goep_authorize_rsp_hdlr(goep_authorize_rsp_struct* goep_authorize_res ); ///< Receive this before OBEX Connection
extern void goep_connect_rsp_hdlr(goep_connect_rsp_struct* goep_connect_res);
extern void goep_auth_rsp_hdlr(goep_auth_rsp_struct* goep_auth_res);
extern void goep_push_rsp_hdlr(goep_push_rsp_struct* goep_push_res );
extern void goep_pull_rsp_hdlr(goep_pull_rsp_struct* goep_pull_res);
extern void goep_set_folder_rsp_hdlr(goep_set_folder_rsp_struct* goep_set_folder_res);
extern void goep_abort_rsp_hdlr(goep_abort_rsp_struct* goep_abort_res);

/// GOEP export functions
extern void goep_bd_addr_array2struct(BD_ADDR *src, goep_bd_addr_struct *dst);
extern void goep_bd_addr_stuct2array(goep_bd_addr_struct *src, BD_ADDR *dst);

/// GOEP internal functions
extern int goep_find_free_goep_id(int isclient, U8 *uuid, U8 uuid_len);
extern GoepServerApp * goep_find_server_with_goep_conn_id(int goep_conn_id);
extern GoepClientApp * goep_find_client_with_goep_conn_id(int goep_conn_id);

extern int goep_find_bt_uuid_with_obex_uuid(U32* bt_uuid_p, U8* obex_uuid_str);
extern int goep_find_obex_uuid_with_bt_uuid(U32* bt_uuid_p, U8* obex_uuid_str, U8* obex_uuid_len);

/// GOEP internal functions - send callback msg
extern void goep_send_cnf_msg(U32 cnf_msg_id, U8 goep_conn_id, ObStatus rsp_code, U8* uuid_p, U8 uuid_len);

extern void goep_send_register_cnf_msg(U32 cnf_msg_id, U8 goep_conn_id, ObStatus rsp_code, U8* uuid_p, U8 uuid_len, U8 req_id);
extern void goep_send_connect_cnf_msg(U8 req_id, U8 goep_conn_id, ObStatus rsp_code, U16 peer_mru, U8* uuid_p, U8 uuid_len);
extern void goep_send_push_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, U8* uuid_p, U8 uuid_len);
extern void goep_send_pull_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, goep_pkt_type_enum pkt_type, 
                            U8* obj_mime_type, U32 total_obj_len, U8* frag_ptr, 
                            U16 frag_len, U8* uuid_p, U8 uuid_len);
extern void goep_send_set_folder_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, U8* uuid_p, U8 uuid_len);
extern void goep_send_abort_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8* uuid_p, U8 uuid_len);
extern void goep_send_auth_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8* uuid_p, U8 uuid_len);

extern void goep_send_authorize_ind_msg(U8 goep_conn_id, goep_bd_addr_struct* bd_addr_p, U8* dev_name);
extern void goep_send_auth_ind_msg(U8 goep_conn_id, U8* realm, U8 realm_len, U8* uuid_p, U8 uuid_len, U8 options);
extern void goep_send_connect_ind_msg(U8 goep_conn_id, U16 peer_mru, U8* uuid_p, U8 uuid_len, U8 challenge, U8 response);
extern void goep_send_push_ind_msg(U8 goep_conn_id, goep_pkt_type_enum pkt_type, 
                            goep_put_type_enum put_type, U32 total_obj_len,
                            U8* obj_name, U16 obj_name_len, 
                            U8* obj_type, U16 obj_type_len, 
                            U8* frag_ptr, U16 frag_len,
                            U8* uuid_p, U8 uuid_len);
extern void goep_send_pull_ind_msg(U8 goep_conn_id, goep_pkt_type_enum pkt_type,
                            U8* obj_mime_type, U8* obj_name, U16 obj_name_len,
                            U8* uuid_p, U8 uuid_len);
extern void goep_send_set_folder_ind_msg(U8 goep_conn_id, goep_set_folder_type_enum setpath_flag,
                            U8* folder_name, U16 folder_name_len, U8* uuid_p, U8 uuid_len);
extern void goep_send_abort_ind_msg(U8 goep_conn_id, U8* uuid_p, U8 uuid_len);
extern void goep_send_disconnect_ind_msg(U8 req_id, U8 goep_conn_id, U8* uuid_p, U8 uuid_len, U8 reason);

/// Extra message
extern void goep_send_opp_supported_formats_ind_msg(U8 goep_conn_id, U8 supported_list);
void goep_send_action_ind_msg(U8 goep_conn_id, U8 action, U8 permission, 
                            U8* folder_name, U16 folder_name_len, U8* dst_name, U16 dst_name_len, U8* uuid_p, U8 uuid_len);
void goep_action_rsp_hdlr(goep_action_rsp_struct* goep_action_rsp);
void goep_action_req_hdlr(goep_action_req_struct* goep_action_req);
void goep_send_action_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, U8* uuid_p, U8 uuid_len);


#endif
