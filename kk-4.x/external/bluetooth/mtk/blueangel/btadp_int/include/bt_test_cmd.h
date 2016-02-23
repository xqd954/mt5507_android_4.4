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

#ifndef __BT_TEST_CMD_H
#define __BT_TEST_CMD_H

typedef void (*LayerIDHdlr) (kal_uint16, kal_uint8 *);

typedef struct _LayerIDHdlrEntry
{
    kal_uint8 layer_id;
    LayerIDHdlr hdlr;

} LayerIDHdlrEntry;

extern LayerIDHdlrEntry LayerIDHdlrTable[];

/* Core Protocol */
#define L2CAP_Layer_ID     0x02
#define RFCOMM_Layer_ID 0x03
#define SDP_Layer_ID    0x04
#define AVDTP_Layer_ID     0x05
#define AVCTP_Layer_ID     0x06
#define GAP_Layer_ID    0x11

/* Core Profile */
#define GAP_Layer_ID    0x11

/* Test Command constants definition */

/* L2CAP layer */
#define TC_L2CAP_Tester_Init_Req   0x01
#define TC_L2CAP_Connect_Req        0x02
#define TC_L2CAP_DisConnect_Req   0x03
#define TC_L2CAP_Echo_Req             0x04
#define TC_L2CAP_Info_Req              0x05
#define TC_L2CAP_Send_Data           0x06

/* RFCOMM layer */
#define TC_RFCOMM_Tester_Init       0x01
#define TC_RFCOMM_Open_Channel  0x02
#define TC_RFCOMM_Enable_Server 0x03
#define TC_RFCOMM_Send_PN           0x04
#define TC_RFCOMM_Send_MSC         0x05
#define TC_RFCOMM_Send_RLS          0x06
#define TC_RFCOMM_Channel_Close  0x07
#define TC_RFCOMM_Send_Data        0x08

/* SDP layer */
#define TC_SDP_Tester_Init  0x01

/* GAP layer */
#define TC_GAP_Tester_Init  0x01
#define TC_GAP_Set_NonDiscoverable_Mode 0x02
#define TC_GAP_Set_LimitedDiscoverable_Mode 0x03
#define TC_GAP_Set_GeneralDiscoverable_Mode 0x04
#define TC_GAP_Set_NonConnectable_Mode 0x05
#define TC_GAP_Set_Connectable_Mode 0x06

#define TC_GAP_Set_NonPairable_Mode 0x07
#define TC_GAP_Set_Pairable_Mode       0x08
#define TC_GAP_Set_Security_Level1    0x09
#define TC_GAP_Set_Security_Level2    0x0A
#define TC_GAP_Set_Security_Level3    0x0B
#define TC_GAP_GeneralInquiry             0x0C
#define TC_GAP_LimitedInquiry             0x0D
#define TC_GAP_DeviceDiscovery          0x0E
#define TC_GAP_Bonding                        0x0F
#define TC_GAP_CreateACLLink             0x10
#define TC_GAP_CreateChannelLink      0x11
#define TC_GAP_DeleteLinkKey          0x12

/* AVDTP layer */
#define TC_AVDTP_Tester_Init    0x01
#define TC_AVDTP_Connect_Req 0x02
#define TC_AVDTP_Discovery_Req 0x03
#define TC_AVDTP_GetCapabilities_Req 0x04
#define TC_AVDTP_SetConfig_Req 0x05
#define TC_AVDTP_ReConfig_Req 0x06
#define TC_AVDTP_Open_Req 0x07
#define TC_AVDTP_Close_Req 0x08
#define TC_AVDTP_Start_Req 0x09
#define TC_AVDTP_Suspend_Req 0x0A
#define TC_AVDTP_Abort_Req 0x0B
#define TC_AVDTP_Discover_Rsp 0x21
#define TC_AVDTP_GetCapabilities_Rsp 0x22
#define TC_AVDTP_SetConfig_Rsp 0x23
#define TC_AVDTP_Open_Rsp 0x24
#define TC_AVDTP_Close_Rsp 0x25
#define TC_AVDTP_Start_Rsp 0x26
#define TC_AVDTP_Suspend_Rsp 0x27
#define TC_AVDTP_Abort_Rsp 0x28
#define TC_AVDTP_ReConfig_Rsp 0x29
#define TC_AVDTP_Write_Data 0x41
#define TC_AVDTP_DisconnectSignalReq 0x42
#define TC_AVDTP_GetCapabilities_Long_Rsp 0x43
#define TC_AVDTP_DisconnectSignal_Rsp 0x44
#define TC_AVDTP_Read_Data 0x45

#define TC_AVDTP_Event_Registration_Req 0xEF
#define TC_AVDTP_EVENT_RSP_TEST 0xFF
/* AVDTP layer : Event Type */
#define TC_AVDTP_Event_Init 0x01
#define TC_AVDTP_Event_ConnectReq_Ind 0x02
#define TC_AVDTP_Event_ConnectReq_Cfm 0x03
#define TC_AVDTP_Event_DisConnectReq_Ind 0x04
#define TC_AVDTP_Event_DisConnectReq_Cfm 0x05
#define TC_AVDTP_Event_Discovery_Ind 0x06
#define TC_AVDTP_Event_Discovery_Cfm 0x07
#define TC_AVDTP_Event_GetCapabilities_Ind 0x08
#define TC_AVDTP_Event_GetCapabilities_Cfm 0x09
#define TC_AVDTP_Event_SetConfig_Ind 0x0A
#define TC_AVDTP_Event_SetConfig_Cfm 0x0B
#define TC_AVDTP_Event_GetConfig_Ind 0x0C
#define TC_AVDTP_Event_GetConfig_Cfm 0x0D
#define TC_AVDTP_Event_Open_Ind 0x0E
#define TC_AVDTP_Event_Open_Cfm 0x0F
#define TC_AVDTP_Event_Close_Ind 0x10
#define TC_AVDTP_Event_Close_Cfm 0x11
#define TC_AVDTP_Event_Start_Ind 0x12
#define TC_AVDTP_Event_Start_Cfm 0x13
#define TC_AVDTP_Event_Suspend_Ind 0x14
#define TC_AVDTP_Event_Suspend_Cfm 0x15
#define TC_AVDTP_Event_ReConfig_Ind 0x16
#define TC_AVDTP_Event_ReConfig_Cfm 0x17
#define TC_AVDTP_Event_Abort_Ind 0x18
#define TC_AVDTP_Event_Abort_Cfm 0x19

#define TC_AVDTP_Event_ConnectReq_Result 0x30
#define TC_AVDTP_Event_ConnectRsp_Result 0x31
#define TC_AVDTP_Event_DisConnectReq_Result 0x32
#define TC_AVDTP_Event_DisConnectRsp_Result 0x33
#define TC_AVDTP_Event_DiscoveryReq_Result 0x34
#define TC_AVDTP_Event_DiscoveryRsp_Result 0x35
#define TC_AVDTP_Event_Get_CapabilitiesReq_Result 0x36
#define TC_AVDTP_Event_Get_CapabilitiesRsp_Result 0x37
#define TC_AVDTP_Event_Set_ConfigReq_Result 0x38
#define TC_AVDTP_Event_Set_ConfigRsp_Result 0x39
#define TC_AVDTP_Event_Get_ConfigReq_Result 0x3A
#define TC_AVDTP_Event_Get_ConfigRsp_Result 0x3B
#define TC_AVDTP_Event_OpenReq_Result 0x3C
#define TC_AVDTP_Event_OpenRsp_Result 0x3D
#define TC_AVDTP_Event_CloseReq_Result 0x3E
#define TC_AVDTP_Event_CloseRsp_Result 0x3F
#define TC_AVDTP_Event_StartReq_Result 0x40
#define TC_AVDTP_Event_StartRsp_Result 0x41
#define TC_AVDTP_Event_SuspendReq_Result 0x42
#define TC_AVDTP_Event_SuspendRsp_Result 0x43
#define TC_AVDTP_Event_ReConfigReq_Result 0x44
#define TC_AVDTP_Event_ReConfigRsp_Result 0x45
#define TC_AVDTP_Event_AbortReq_Result 0x46
#define TC_AVDTP_Event_AbortRsp_Result 0x47
#define TC_AVDTP_Event_WriteReq_Result 0x48
#define TC_AVDTP_Event_ReadReq_Result 0x49

#define TC_AVDTP_Event_DiscoveryRej_Result 0x60
#define TC_AVDTP_Event_GetCapabilitiesRej_Result 0x61
#define TC_AVDTP_Event_SetConfigRej_Result 0x62
#define TC_AVDTP_Event_GetConfigRej_Result 0x63
#define TC_AVDTP_Event_ReConfigRej_Result 0x64
#define TC_AVDTP_Event_OpenRej_Result 0x65
#define TC_AVDTP_Event_StartRej_Result 0x66
#define TC_AVDTP_Event_CloseRej_Result 0x67
#define TC_AVDTP_Event_SuspendRej_Result 0x68

#define TC_AVDTP_Event_Registration_Req_Result 0xEF

/* AVCTP layer */
#define TC_AVCTP_Tester_Init 0x01
#define TC_AVCTP_Connect_Req 0x02
#define TC_AVCTP_Connect_Rsp 0x03
#define TC_AVCTP_DisConnect_Req 0x04
#define TC_AVCTP_SendMsg_Req 0x05
#define TC_AVCTP_SendLongMsg_Req 0x06
#define TC_AVCTP_ReadMsg_Req 0x07
#define TC_AVCTP_Event_Registration_Req 0xEF

/* AVCTP layer : Event Type */
#define TC_AVCTP_Event_Init 0x01
#define TC_AVCTP_Event_ConnectReq_Ind 0x02
#define TC_AVCTP_Event_ConnectReq_Cfm 0x03
#define TC_AVCTP_Event_DisConnectReq_Ind 0x04
#define TC_AVCTP_Event_DisConnectReq_Cfm 0x05
#define TC_AVCTP_Event_MessageRecReq_Ind 0x06

#define TC_AVCTP_Event_ConnectReq_Result 0x30
#define TC_AVCTP_Event_ConnectRsp_Result 0x31
#define TC_AVCTP_Event_DisConnectReq_Result 0x32
#define TC_AVCTP_Event_DisConnectRsp_Result 0x33
#define TC_AVCTP_Event_MessageRecReq_Result 0x34
#define TC_AVCTP_Event_MessageRecRsp_Result 0x35
#define TC_AVCTP_Event_Registration_Req_Result 0xEF

#define TC_AVCTP_EVENT_RSP_TEST 0xFF

extern void btTester_sendMsg(
                msg_type msg,
                module_type dstMod,
                sap_type sap,
                local_para_struct *local_para,
                peer_buff_struct *peer_buff);
extern void bt_parse_test_cmd_hdlr(local_para_struct *local_para_ptr, peer_buff_struct *peer_buff_ptr);
extern void bt_l2cap_layer_id_hdlr(kal_uint16 data_len, kal_uint8 *test_cmd);
extern void bt_rfcomm_layer_id_hdlr(kal_uint16 data_len, kal_uint8 *test_cmd);
extern void bt_sdp_layer_id_hdlr(kal_uint16 data_len, kal_uint8 *test_cmd);
extern void bt_gap_layer_id_hdlr(kal_uint16 data_len, kal_uint8 *test_cmd);
extern void bt_avdtp_layer_id_hdlr(kal_uint16 data_lenss, kal_uint8 *test_cmd);
extern void bt_avctp_layer_id_hdlr(kal_uint16 data_len, kal_uint8 *test_cmd);
extern void BtSendMsgToL4(bt_test_cmd_rsp_struct *param_ptr);

#endif /* __BT_TEST_CMD_H */ 

