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

void AVRCP_UtilRegisterCB(BTAVRCPCallback Callback);
void AVRCP_UitlActivateReq(U8 chnl_num, U8 local_role, U8 version, U8 sdpfeature);
void AVRCP_UitlDeactivateReq(U8 chnl_num);
void AVRCP_UitlConnectReq(U8 chnl_num, U8 local_role, bt_avrcp_dev_addr_struct* avrcp_device_addr);
void AVRCP_UitlConnectIndRes(U8 chnl_num, BOOL accept_conn);
void AVRCP_UitlDisconnectReq(U8 chnl_num);
void AVRCP_UitlInternalDisconnectReq(U8 chnl_num);
void AVRCP_UitlCmdFrameReq(U8 chnl_num, U8 c_type, U8 subunit_type, U8 subunit_id, U16 data_len, U8* frame_data);
void AVRCP_UitlCmdFrameRspInd(U8 chnl_num, U8 seq_id, U8 c_type,  U8 subunit_type, U8 subunit_id, U16 data_len, const U8* frame_data);
void AVRCP_UtilAVRCPCmdCapabiltiesRsp(AvrcpChnlInfo *chnl_num, U8 c_type, U16 count, U8* events);
void AVRCP_UitlBrowseConnectReq(U8 chnl_num);
void AVRCP_UitlBrowseDisconnectReq(U8 chnl_num);

void AVRCP_UtilAVRCPGetCurPlayerValueRsp( bt_avrcp_get_curplayerapp_value_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdSetPlayerValueRsp( bt_avrcp_set_playerapp_value_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdGetPlayerAttrTextRsp(bt_avrcp_get_playerapp_attributetext_rsp_struct *rsp);
void AVRCP_UtilAVRCPGetPlayerValueTextRsp(bt_avrcp_get_playerapp_valuetext_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdInformCharsetRsp(bt_avrcp_inform_display_charset_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdInformBattStatusOfCTRsp(bt_avrcp_inform_battery_statusofct_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdGetElementAttributesRsp(bt_avrcp_get_element_attributes_rsp_struct *rsp);
void AVRCP_UtilAVRCPCmdGetPlayerStatusRsp(bt_avrcp_get_playerstatus_rsp_struct *rsp);


/***** AVRCP control msg *****/
void AVRCP_UtilAVRCPSendCmd_Get_Capabilities_Req(bt_avrcp_get_capabilities_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_List_Playerapp_Attribute_Req(bt_avrcp_list_playerapp_attribute_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_List_Playerapp_Value_Req(bt_avrcp_list_playerapp_value_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Get_Curplayerapp_Value_Req(bt_avrcp_get_curplayerapp_value_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Set_Playerapp_Value_Req(bt_avrcp_set_playerapp_value_req_struct *msg_p);		
void AVRCP_UtilAVRCPSendCmd_Get_Playerapp_Attributetext_Req(bt_avrcp_get_playerapp_attributetext_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Get_Playerapp_Valuetext_Req(bt_avrcp_get_playerapp_valuetext_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Inform_Display_Charset_Req(bt_avrcp_inform_display_charset_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Inform_Battery_Statusofct_Req(bt_avrcp_inform_battery_statusofct_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Get_Element_Attributes_Req(bt_avrcp_get_element_attributes_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Get_Playerstatus_Req(bt_avrcp_get_playerstatus_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Register_Nofitication_Req(bt_avrcp_register_notification_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Abort_Continueresponse_Req(bt_avrcp_abort_continueresponse_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Set_Absolute_Volume_Req(bt_avrcp_set_absolute_volume_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Set_Addressedplayer_Req(bt_avrcp_set_addressedplayer_req_struct *msg_p);
//void AVRCP_UtilAVRCPSendCmd_Get_Playeritems_Req(bt_avrcp_get_playeritems_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Get_Folderitems_Req(bt_avrcp_get_folderitems_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Set_Browsedplayer_Req(bt_avrcp_set_browsedplayer_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Change_Path_Req(bt_avrcp_change_path_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Get_Itemattributes_Req(bt_avrcp_get_itemattributes_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Play_Items_Req(bt_avrcp_play_items_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Search_Req(bt_avrcp_search_req_struct *msg_p);
void AVRCP_UtilAVRCPSendCmd_Add_Tonowplaying_Req(bt_avrcp_add_tonowplaying_req_struct *msg_p);

