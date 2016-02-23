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

/* basic system service headers */
#include "bt_common.h"
#include "bluetooth_struct.h"
#include "bt.h"

#include "config.h"
#include "btconfig.h"
#include "bluetooth_gap_struct.h"
#include "btbm_adp.h"

#include "osapi.h"
#include "bt_test_cmd.h"
#include "l2captester.h"
#include "Rfcomm_adp.h"
#include "Rfcommtester.h"
#include "sdptester.h"
#include "gaptester.h"
#include "avdtp.h"
#include "avdtptester.h"
#include "avctptester.h"

#include "uart_sw.h"
#include "bluetooth_struct.h"
#include "app_buff_alloc.h"

#ifdef __BQE_TESTER_

#ifdef __GEMINI__
extern module_type Get_DualSim_Uart_Default_Owner(void);
#endif 

extern void CONVERT_ARRAY2BDADDR(btbm_bd_addr_t *dest, U8 *src);

#define BEGIN_EDFINE_LAYERIDHDLRTABLE   \
   LayerIDHdlrEntry LayerIDHdlrTable[]= \
   {

#define END_DEFINE_LAYERIDHDLRTABLE \
   };

#define INIT_LAYERIDHDLR_ENTRY(layer_id,hdlr) \
   {layer_id##_Layer_ID, hdlr}

/* define LayerIDHdlrTable and its entries */
BEGIN_EDFINE_LAYERIDHDLRTABLE 
INIT_LAYERIDHDLR_ENTRY(L2CAP, bt_l2cap_layer_id_hdlr), 
INIT_LAYERIDHDLR_ENTRY(RFCOMM,                                                                                                            bt_rfcomm_layer_id_hdlr),
INIT_LAYERIDHDLR_ENTRY(SDP, bt_sdp_layer_id_hdlr),
#ifdef __AVDTP_TESTER_
  INIT_LAYERIDHDLR_ENTRY(AVDTP, bt_avdtp_layer_id_hdlr),
#endif 
#ifdef __AVCTP_TESTER_
  INIT_LAYERIDHDLR_ENTRY(AVCTP, bt_avctp_layer_id_hdlr),
#endif 
INIT_LAYERIDHDLR_ENTRY(GAP, bt_gap_layer_id_hdlr) 
END_DEFINE_LAYERIDHDLRTABLE

/* for current usage, dst mod is MOD_ATCI */
void btTester_sendMsg(msg_type msg, module_type dstMod, sap_type sap, local_para_struct *local_para,
                      peer_buff_struct *peer_buff)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = msg;
    ilmPtr->local_para_ptr = local_para;
    ilmPtr->peer_buff_ptr = peer_buff;
    ilmPtr->dest_mod_id = dstMod;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = sap;
    msg_send_ext_queue(ilmPtr);
}


/*****************************************************************************
 * FUNCTION
 *  bt_parse_test_cmd_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  local_para_ptr      [IN]
 *  peer_buff_ptr       [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_parse_test_cmd_hdlr(local_para_struct *local_para_ptr, peer_buff_struct *peer_buff_ptr)
{
    bt_test_cmd_req_struct *msg_ptr;
    kal_uint8 layer_id = 0;
    kal_uint16 data_len = 0;
    kal_uint8 cmd_type = 0;
    kal_uint8 *test_cmd_ptr;
    int i = 0;

    msg_ptr = (bt_test_cmd_req_struct*) local_para_ptr;

    layer_id = msg_ptr->test_cmd_content[1];
    data_len = ((msg_ptr->test_cmd_content[3] << 8) | msg_ptr->test_cmd_content[2]);
    cmd_type = msg_ptr->test_cmd_content[4];
    test_cmd_ptr = (msg_ptr->test_cmd_content) + 4;

    for (i = 0; i < sizeof(LayerIDHdlrTable) / sizeof(struct _LayerIDHdlrEntry); i++)
    {
        if (layer_id == LayerIDHdlrTable[i].layer_id)
        {
            (*LayerIDHdlrTable[i].hdlr) (data_len, test_cmd_ptr);
        }

    }

}


/*****************************************************************************
 * FUNCTION
 *  bt_l2cap_layer_id_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  data_len        [IN]        
 *  test_cmd        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_l2cap_layer_id_hdlr(kal_uint16 data_len, kal_uint8 *test_cmd)
{
    kal_uint8 cmd_type = 0;
    U16 info_type = 0;
    L2capConnectRequest connect_parms;
    L2capDisconnectRequest disconnect_parms;
    L2capEchoRequest echo_req_parms;
    L2capInfoRequest info_req_parms;

    cmd_type = test_cmd[0];

    switch (cmd_type)
    {
        case TC_L2CAP_Tester_Init_Req:
            /* cmd_type: L2CAP_TESTER_INIT */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_L2CAPTESTERINIT);
    #endif 
            l2capTesterInit();
            break;

        case TC_L2CAP_Connect_Req:
            /* cmd_type: L2CAP_CONNECT_REQ */
            Assert(data_len == 7);
            memcpy((U8*) connect_parms.bd_addr, (U8*) & test_cmd[1], 6);
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_L2CA_CONNECTREQ);
    #endif 
            L2CA_ConnectReq(&connect_parms);
            break;

        case TC_L2CAP_DisConnect_Req:
            /* cmd_type: L2CAP_DISCONNECT_REQ */
            disconnect_parms.cid = test_cmd[1];
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_L2CA_DISCONNECTREQ);
    #endif 
            L2CA_TesterDisconnect(disconnect_parms.cid);
            /* L2CA_DisconnectReq(&disconnect_parms); */
            break;

        case TC_L2CAP_Echo_Req:
            /* cmd_type: L2CAP_ECHO_REQ */
            Assert(data_len == 7);
            memcpy((U8*) echo_req_parms.bd_addr, (U8*) & test_cmd[1], 6);
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_L2CA_ECHOREQ);
    #endif 
            L2CA_EchoReq(&echo_req_parms);
            break;

        case TC_L2CAP_Info_Req:
            /* cmd_type: L2CAP_INFO_REQ */
            Assert(data_len == 9);
            memcpy((U8*) info_req_parms.bd_addr, (U8*) & test_cmd[1], 6);
            info_type = (U16) ((test_cmd[8] << 8) | (test_cmd[7]));
            info_req_parms.info_type = info_type;
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_L2CA_INFOREQ);
    #endif 
            L2CA_InfoReq(&info_req_parms);
            break;

        case TC_L2CAP_Send_Data:
            /* cmd_type: L2CAP_SEND_DATA */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_L2CAPSENDDATA);
    #endif 
            L2capSendData();
            break;
        default:
            break;

    }

}


/*****************************************************************************
 * FUNCTION
 *  bt_rfcomm_layer_id_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  data_len        [IN]        
 *  test_cmd        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_rfcomm_layer_id_hdlr(kal_uint16 data_len, kal_uint8 *test_cmd)
{

#ifdef __RFCOMM_TESTER_

    kal_uint8 cmd_type = 0;
    RfcommConnectRequest open_channel_parms;

    cmd_type = test_cmd[0];
    switch (cmd_type)
    {
        case TC_RFCOMM_Tester_Init:
            /* cmd_type: RFCOMM_TESTER_INIT */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_RFCOMMTESTERINIT);
    #endif 
            RfcommTesterInit();
            break;

        case TC_RFCOMM_Open_Channel:
            /* cmd_type: RFCOMM_OPEN_CHANNEL */
            memcpy((U8*) open_channel_parms.bd_addr, (U8*) & test_cmd[1], 6);
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_RFCOMMTESTEROPENCLIENTCHANNEL);
    #endif 
            RfcommTesterOpenClientChannel(&open_channel_parms);
            break;

        case TC_RFCOMM_Enable_Server:
            /* cmd_type: RFCOMM_ENABLE_SERVER */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_RFCOMMTESTEROPENSERVER);
    #endif 
            RfcommTesterOpenServer();
            break;

        case TC_RFCOMM_Send_PN:
            /* cmd_type: RFCOMM_SEND_PN */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_RFCOMMTESTERSENDPN);
    #endif 
            RfcommTesterSendPN();
            break;

        case TC_RFCOMM_Send_MSC:
            /* cmd_type: RFCOMM_SEND_MSC */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_RFCOMMTESTERSENDMSC);
    #endif 
            RfcommTesterSendMSC(test_cmd[1]);
            break;

        case TC_RFCOMM_Send_RLS:
            /* cmd_type: RFCOMM_SEND_RLS */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_RFCOMMTESTERRLS);
    #endif 
            RfcommTesterRls();
            break;

        case TC_RFCOMM_Channel_Close:
            /* cmd_type: RFCOMM_CHANNEL_CLOSE */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_RFCOMMTESTERCLOSECHANNEL);
    #endif 
            RfcommTesterCloseChannel();
            break;

        case TC_RFCOMM_Send_Data:
            /* cmd_type: RFCOMM_SEND_DATA */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_RFCOMMTESTERDATAREQ);
    #endif 
            RfcommTesterDataReq(test_cmd[1]);
            break;
        default:
            break;

    }

#endif /* __RFCOMM_TESTER_ */ 

}


/*****************************************************************************
 * FUNCTION
 *  bt_sdp_layer_id_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  data_len        [IN]        
 *  test_cmd        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sdp_layer_id_hdlr(kal_uint16 data_len, kal_uint8 *test_cmd)
{
    kal_uint8 cmd_type = 0;

    cmd_type = test_cmd[0];
    switch (cmd_type)
    {
        case TC_SDP_Tester_Init:
            /* cmd_type: SDP_TESTER_INIT */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_SDPTESTERINIT);
    #endif 
            SdpTesterInit();
            break;
        default:
            break;

    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_gap_layer_id_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  data_len        [IN]        
 *  test_cmd        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_gap_layer_id_hdlr(kal_uint16 data_len, kal_uint8 *test_cmd)
{
    kal_uint8 cmd_type = 0;
    btbm_bd_addr_t bond_bd_addr;
    btbm_bd_addr_t acl_link_bd_addr;
    btbm_bd_addr_t channel_bd_addr;
    btbm_bd_addr_t linkkey_bd_addr;

    cmd_type = test_cmd[0];

    switch (cmd_type)
    {
        case TC_GAP_Tester_Init:
            /* cmd_type: GAP_TESTER_INIT */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERINIT);
    #endif 
            gapTesterInit();
            break;

        case TC_GAP_Set_NonDiscoverable_Mode:
            /* cmd_type: GAP_SET_NONDISCOVERABLE_MODE */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERNONDISCOVERABLEMODE);
    #endif 
            gapTesterNonDiscoverableMode();
            break;

        case TC_GAP_Set_LimitedDiscoverable_Mode:
            /* cmd_type: GAP_SET_LIMITEDDISCOVERABLE_MODE */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERLIMITEDDISCOVERABLEMODE);
    #endif 
            gapTesterLimitedDiscoverableMode();
            break;

        case TC_GAP_Set_GeneralDiscoverable_Mode:
            /* cmd_type: GAP_SET_GENERALDISCOVERABLE_MODE */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERGENERALDISCOVERABLEMODE);
    #endif 
            gapTesterGeneralDiscoverableMode();
            break;

        case TC_GAP_Set_NonConnectable_Mode:
            /* cmd_type: GAP_SET_NONCONNECTABLE_MODE */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERNONCONNECTABLEMODE);
    #endif 
            gapTesterNonConnectableMode();
            break;

        case TC_GAP_Set_Connectable_Mode:
            /* cmd_type: GAP_SET_CONNECTABLE_MODE */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERCONNECTABLEMODE);
    #endif 
            gapTesterConnectableMode();
            break;

        case TC_GAP_Set_NonPairable_Mode:
            /* cmd_type: GAP_SET_NONPAIRABLE_MODE */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERNONPAIRABLE);
    #endif 
            gapTesterNonPairable();
            break;

        case TC_GAP_Set_Pairable_Mode:
            /* cmd_type: GAP_SET_PAIRABLE_MODE */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERPAIRABLE);
    #endif 
            gapTesterPairable();
            break;

        case TC_GAP_Set_Security_Level1:
            /* cmd_type: GAP_SET_SECURITY_LEVEL1 */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERSECURITYLEVEL1);
    #endif 
            gapTesterSecurityLevel1();
            break;

        case TC_GAP_Set_Security_Level2:
            /* cmd_type: GAP_SET_SECURITY_LEVEL2 */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERSECURITYLEVEL2);
    #endif 
            gapTesterSecurityLevel2();
            break;

        case TC_GAP_Set_Security_Level3:
            /* cmd_type: GAP_SET_SECURITY_LEVEL3 */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERSECURITYLEVEL3);
    #endif 
            gapTesterSecurityLevel3(0);
            break;

        case TC_GAP_GeneralInquiry:
            /* cmd_type: GAP_GENERAL_INQUIRY */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERGENERALINQUIRY);
    #endif 
            gapTesterGeneralInquiry();
            break;

        case TC_GAP_LimitedInquiry:
            /* cmd_type: GAP_Limited_INQUIRY */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERLIMITEDINQUIRY);
    #endif 
            gapTesterLimitedInquiry();
            break;

        case TC_GAP_DeviceDiscovery:
            /* cmd_type: GAP_DEVICE_DISCOVERY */
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERDEVICEDISCOVERY);
    #endif 
            gapTesterDeviceDiscovery();
            break;

        case TC_GAP_Bonding:
            //cmd_type: GAP_BONDING
            //[question:] which bytes fill ??
            CONVERT_ARRAY2BDADDR(&bond_bd_addr, &test_cmd[1]);
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERBONDING);
    #endif 
            gapTesterBonding(bond_bd_addr);
            break;

        case TC_GAP_CreateACLLink:
            //cmd_type: GAP_CREATE_ACL_LINK
            //[question:] which bytes fill ??
            CONVERT_ARRAY2BDADDR(&acl_link_bd_addr, &test_cmd[1]);
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERCREATEACLLINK);
    #endif 
            gapTesterCreateAclLink(acl_link_bd_addr);
            break;

        case TC_GAP_CreateChannelLink:
            //cmd_type: GAP_CREATE_CHANNEL_LINK
            //[question:] which bytes fill ??
            CONVERT_ARRAY2BDADDR(&channel_bd_addr, &test_cmd[1]);
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_GAPTESTERCREATECHANNEL);
    #endif 
            gapTesterCreateChannel(channel_bd_addr);
            break;

        case TC_GAP_DeleteLinkKey:

            //cmd_type: GAP_CREATE_CHANNEL_LINK
            //[question:] which bytes fill ??
            CONVERT_ARRAY2BDADDR(&linkkey_bd_addr, &test_cmd[1]);
    #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTGAPTESTERDELETELINKKEY);
    #endif 
            gapTesterDeleteLinkKey(linkkey_bd_addr);
        default:
            break;

    }

}

#ifdef __AVDTP_TESTER_


/*****************************************************************************
 * FUNCTION
 *  bt_avdtp_layer_id_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  data_len        [IN]        
 *  test_cmd        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_avdtp_layer_id_hdlr(kal_uint16 data_len, kal_uint8 *test_cmd)
{
    kal_uint8 cmd_type = 0;
    AvdtpConnectRequest connect_req_parms;
    AvdtpStreamDiscoverRequest discovery_req_parms;
    AvdtpStreamGetCapabilitiesRequest get_capabilities_parms;
    AvdtpStreamSetConfigurationRequest set_config_parms;
    AvdtpStreamReConfigurationRequest re_config_parms;
    AvdtpStreamOpenRequest open_req_parms;
    AvdtpStreamCloseRequest close_req_parms;
    AvdtpStreamStartRequest start_req_parms;
    AvdtpStreamSuspendRequest suspend_req_parms;
    AvdtpStreamAbortRequest abort_req_parms;
    AvdtpstreamdiscoverResponse discovery_res_parms;
    AvdtpStreamGetCapabilitiesResponse get_capabilities_res_parms;
    AvdtpStreamSetConfigurationResponse set_config_rsp_parms;
    AvdtpStreamOpenResponse open_rsp_parms;
    AvdtpStreamCloseResponse close_rsp_parms;
    AvdtpStreamStartResponse start_rsp_parms;
    AvdtpStreamSuspendResponse suspend_rsp_parms;
    AvdtpStreamAbortResponse abort_rsp_parms;
    AvdtpStreamReConfigurationResponse reconfig_parms;
    AvdtpWriteDataReq write_data_parms;
    AvdtpReadDataReq read_data_parms;

    U16 str_data_len = 0;
    local_para_struct *msg;
    bt_test_cmd_rsp_struct *param_ptr;

    cmd_type = test_cmd[0];

    switch (cmd_type)
    {
        case TC_AVDTP_EVENT_RSP_TEST:
            /* for testing rsp event */
            param_ptr =
                (bt_test_cmd_rsp_struct*) construct_local_para((kal_uint16) sizeof(bt_test_cmd_rsp_struct), TD_CTRL);

            memset(param_ptr->test_cmd_rsp_buf, 0, 256);

            if (test_cmd[1] == TC_AVDTP_Event_Init)
            {
                /* Test for TC_AVDTP_Event_Init */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_Init;   /* AVDTP_Event_Init */
                param_ptr->test_cmd_rsp_buf[5] = 0x00;  /* Registeration result : 0x00 success */
                param_ptr->length = 2;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_ConnectReq_Ind)
            {
                /* Test for AVDTP_ConnectReq_Ind */
                unsigned char addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_ConnectReq_Ind; /* TC_AVDTP_Event_ConnectReq_Ind */

                memcpy(&param_ptr->test_cmd_rsp_buf[5], addr, 6);
                param_ptr->length = 7;  /* fill 1 byte AVDTP_event_type + 6 bytes BD_ADDR */

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_ConnectReq_Cfm)
            {
                /* Test for TC_AVDTP_Event_ConnectReq_Cfm */
                unsigned char addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_ConnectReq_Cfm; /* TC_AVDTP_Event_ConnectReq_Cfm */
                memcpy(&param_ptr->test_cmd_rsp_buf[5], addr, 6);

                /* connect result */
                param_ptr->test_cmd_rsp_buf[11] = 0x00;
                param_ptr->test_cmd_rsp_buf[12] = 0x00;
                /* config result */
                param_ptr->test_cmd_rsp_buf[13] = 0x00;
                param_ptr->test_cmd_rsp_buf[14] = 0x00;
                /* status */
                param_ptr->test_cmd_rsp_buf[15] = 0x00;
                param_ptr->test_cmd_rsp_buf[16] = 0x00;

                param_ptr->length = 13;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_DisConnectReq_Ind)
            {
                /* Test for TC_AVDTP_Event_DisConnectReq_Ind */
                unsigned char addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_DisConnectReq_Ind;      /* TC_AVDTP_Event_DisConnectReq_Ind */
                memcpy(&param_ptr->test_cmd_rsp_buf[5], addr, 6);
                param_ptr->length = 7;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_DisConnectReq_Cfm)
            {
                /* Test for TC_AVDTP_Event_DisConnectReq_Cfm */
                unsigned char addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_DisConnectReq_Cfm;      /* TC_AVDTP_Event_DisConnectReq_Cfm */
                memcpy(&param_ptr->test_cmd_rsp_buf[5], addr, 6);
                param_ptr->test_cmd_rsp_buf[11] = 0;    /* disconnect_result */
                param_ptr->test_cmd_rsp_buf[12] = 0;    /* disconnect_result */
                param_ptr->length = 9;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_Discovery_Ind)
            {
                /* Test for TC_AVDTP_Event_Discovery_Ind */
                unsigned char addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_Discovery_Ind;  /* TC_AVDTP_Event_Discovery_Ind */
                param_ptr->test_cmd_rsp_buf[5] = 1;
                memcpy(&param_ptr->test_cmd_rsp_buf[6], addr, 6);

                param_ptr->length = 8;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_Discovery_Cfm)
            {
                /* Test for TC_AVDTP_Event_Discovery_Cfm */
                unsigned short len = 10;

                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_Discovery_Cfm;  /* TC_AVDTP_Event_Discovery_Cfm */
                param_ptr->test_cmd_rsp_buf[5] = 11;    /* transactionID */

                /*
                 * // inx6-15 : ACP SEIDs primary data 
                 * param_ptr->test_cmd_rsp_buf[5+10+1] = 0; //Event error code
                 * param_ptr->length = 13;
                 */
                param_ptr->test_cmd_rsp_buf[6] = 0; /* errCode */
                param_ptr->test_cmd_rsp_buf[7] = (unsigned char)(len & 0x00FF);
                param_ptr->test_cmd_rsp_buf[8] = (unsigned char)((len >> 8) & 0x00FF);

                /* inx 9-: raw data of  ACP SEIDs primary data */
                param_ptr->length = 5 + len;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_GetCapabilities_Ind)
            {
                /* Test for TC_AVDTP_Event_GetCapabilities_Ind */
                unsigned char addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_GetCapabilities_Ind;    /* TC_AVDTP_Event_GetCapabilities_Ind */
                param_ptr->test_cmd_rsp_buf[5] = 2;
                memcpy(&param_ptr->test_cmd_rsp_buf[6], addr, 6);

                /* ACP SEID */
                param_ptr->test_cmd_rsp_buf[12] = 12;

                param_ptr->length = 9;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_GetCapabilities_Cfm)
            {
                /* Test for TC_AVDTP_Event_GetCapabilities_Cfm */
                unsigned short len = 10;

                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_GetCapabilities_Cfm;    /* TC_AVDTP_Event_GetCapabilities_Cfm */
                param_ptr->test_cmd_rsp_buf[5] = 11;    /* transactionID */

                /*
                 * // inx6-15 : List of stream capabilities 
                 * param_ptr->test_cmd_rsp_buf[5+10+1] = 0; //Event error code
                 * param_ptr->length = 13;
                 */
                param_ptr->test_cmd_rsp_buf[6] = 0; /* errCode */
                param_ptr->test_cmd_rsp_buf[7] = (unsigned char)(len & 0x00FF);
                param_ptr->test_cmd_rsp_buf[8] = (unsigned char)((len >> 8) & 0x00FF);

                /* inx 9-: raw data of  List of stream capabilities */
                param_ptr->length = 5 + len;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_SetConfig_Ind)
            {
                /* Test for TC_AVDTP_Event_SetConfig_Ind */
                unsigned short len = 10;

                unsigned char addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_SetConfig_Ind;  /* TC_AVDTP_Event_SetConfig_Ind */
                param_ptr->test_cmd_rsp_buf[5] = 2;
                memcpy(&param_ptr->test_cmd_rsp_buf[6], addr, 6);

                /* ACP SEID */
                param_ptr->test_cmd_rsp_buf[12] = 12;
                /* INT SEID */
                param_ptr->test_cmd_rsp_buf[13] = 11;

                /* stream handle ? */
                param_ptr->test_cmd_rsp_buf[14] = 11;
                param_ptr->test_cmd_rsp_buf[15] = 0x00;

                param_ptr->test_cmd_rsp_buf[16] = (unsigned char)(len & 0x00FF);
                param_ptr->test_cmd_rsp_buf[17] = (unsigned char)((len >> 8) & 0x00FF);
                /* inx:18- : List of config parameters */
                param_ptr->length = 14 + len;

                /*       
                 * //inx 16-25 : List of config parameters
                 * param_ptr->length = 22;
                 */

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_SetConfig_Cfm)
            {
                /* Test for TC_AVDTP_Event_SetConfig_Cfm */
                unsigned short len = 10;

                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_SetConfig_Cfm;  /* TC_AVDTP_Event_SetConfig_Cfm */
                param_ptr->test_cmd_rsp_buf[5] = 11;    /* transactionID */
                param_ptr->test_cmd_rsp_buf[6] = 0x00;  /* errCode */
                param_ptr->test_cmd_rsp_buf[7] = (unsigned char)(len & 0x00FF);
                param_ptr->test_cmd_rsp_buf[8] = (unsigned char)((len >> 8) & 0x00FF);
                /* inx 9-: First stream config category to fail */
                param_ptr->length = 5 + len;

                /*       
                 * //inx 6-15 : First Stream Config category to fail
                 * param_ptr->test_cmd_rsp_buf[16] = 0; //Event error code
                 * param_ptr->length = 13;
                 */

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_GetConfig_Ind)
            {
                /* Test for TC_AVDTP_Event_GetConfig_Ind */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_GetConfig_Ind;  /* TC_AVDTP_Event_GetConfig_Ind */
                param_ptr->test_cmd_rsp_buf[5] = 0x01;  /* stream handle: 2 bytes */
                param_ptr->test_cmd_rsp_buf[6] = 0x00;
                param_ptr->length = 3;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_GetConfig_Cfm)
            {
                /* Test for TC_AVDTP_Event_GetConfig_Cfm */
                unsigned short len = 10;

                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_GetConfig_Cfm;  /* TC_AVDTP_Event_GetConfig_Cfm */
                param_ptr->test_cmd_rsp_buf[5] = 1;
                //Note: AVDT_GetConfig()-> AvdtpSigStreamGetConfig() : assign stream to currentTxStream
                //only streamID (1 byte) , no stream handle(2 bytes)
                param_ptr->test_cmd_rsp_buf[6] = 12;
                param_ptr->test_cmd_rsp_buf[7] = 0x00;
                param_ptr->test_cmd_rsp_buf[8] = 0x00;  /* errCode */
                param_ptr->test_cmd_rsp_buf[9] = (unsigned char)(len & 0x00FF);
                param_ptr->test_cmd_rsp_buf[10] = (unsigned char)((len >> 8) & 0x00FF);
                /* inx 11- : List of Stream config data */
                param_ptr->length = 7 + len;

                /*  
                 * //inx 8-17 : First Stream Config category to fail
                 * param_ptr->test_cmd_rsp_buf[18] = 0; //Event error code
                 * param_ptr->length = 15;
                 */

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_Open_Ind)
            {
                /* Test for TC_AVDTP_Event_Open_Ind */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_Open_Ind;       /* TC_AVDTP_Event_Open_Ind */
                param_ptr->test_cmd_rsp_buf[5] = 2;

                /* stream handle */
                param_ptr->test_cmd_rsp_buf[6] = 1;
                param_ptr->test_cmd_rsp_buf[7] = 0x00;

                param_ptr->length = 4;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_Open_Cfm)
            {
                /* Test for TC_AVDTP_Event_Open_Cfm */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_Open_Cfm;       /* TC_AVDTP_Event_Open_Cfm */
                param_ptr->test_cmd_rsp_buf[5] = 0x00;  /* Registeration result : 0x00 success */
                param_ptr->length = 2;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_Close_Ind)
            {
                /* Test for TC_AVDTP_Event_Close_Ind */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_Close_Ind;      /* TC_AVDTP_Event_Close_Ind */
                param_ptr->test_cmd_rsp_buf[5] = 2;

                /* inx 6-7: stream handle */
                param_ptr->test_cmd_rsp_buf[6] = 1;
                param_ptr->test_cmd_rsp_buf[7] = 0x00;

                param_ptr->length = 4;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_Close_Cfm)
            {
                /* Test for TC_AVDTP_Event_Close_Cfm */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_Close_Cfm;      /* TC_AVDTP_Event_Close_Cfm */
                param_ptr->test_cmd_rsp_buf[5] = 1;
                param_ptr->test_cmd_rsp_buf[6] = 0;     /* Event error code */

                param_ptr->length = 3;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_Start_Ind)
            {
                /* Test for TC_AVDTP_Event_Start_Ind */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_Start_Ind;      /* TC_AVDTP_Event_Start_Ind */
                param_ptr->test_cmd_rsp_buf[5] = 2;

                /* inx 6-9 : List of stream handles */

                param_ptr->length = 6;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_Start_Cfm)
            {
                /* Test for TC_AVDTP_Event_Start_Cfm */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_Start_Cfm;      /* TC_AVDTP_Event_Start_Cfm */
                param_ptr->test_cmd_rsp_buf[5] = 1;

                /* inx 6-7: stream handle */
                param_ptr->test_cmd_rsp_buf[6] = 1;
                param_ptr->test_cmd_rsp_buf[7] = 0x00;

                param_ptr->test_cmd_rsp_buf[8] = 0; /* Event error code */

                param_ptr->length = 5;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_Suspend_Ind)
            {
                /* Test for TC_AVDTP_Event_Suspend_Ind */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_Suspend_Ind;    /* TC_AVDTP_Event_Suspend_Ind */
                param_ptr->test_cmd_rsp_buf[5] = 2;

                /* inx 6-7: stream handle */
                param_ptr->test_cmd_rsp_buf[6] = 1;
                param_ptr->test_cmd_rsp_buf[7] = 0x00;

                /* inx 9-8 : another stream handle */

                param_ptr->length = 6;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_Suspend_Cfm)
            {
                /* Test for TC_AVDTP_Event_Suspend_Cfm */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_Suspend_Cfm;    /* TC_AVDTP_Event_Suspend_Cfm */
                param_ptr->test_cmd_rsp_buf[5] = 1;

                /* inx 6-7: stream handle */
                param_ptr->test_cmd_rsp_buf[6] = 1;
                param_ptr->test_cmd_rsp_buf[7] = 0x00;

                param_ptr->test_cmd_rsp_buf[8] = 0; /* Event error code */

                param_ptr->length = 5;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_ReConfig_Ind)
            {
                /* Test for TC_AVDTP_Event_ReConfig_Ind */
                unsigned short len = 10;

                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_ReConfig_Ind;   /* TC_AVDTP_Event_ReConfig_Ind */
                param_ptr->test_cmd_rsp_buf[5] = 2;

                /* inx 6-7: stream handle */
                param_ptr->test_cmd_rsp_buf[6] = 1;
                param_ptr->test_cmd_rsp_buf[7] = 0x00;

                param_ptr->test_cmd_rsp_buf[8] = (unsigned char)(len & 0x00FF);
                param_ptr->test_cmd_rsp_buf[9] = (unsigned char)((len >> 8) & 0x00FF);
                /* inx 10- : List of Reconfig stream config parameters */
                param_ptr->length = 6 + len;
                /* 
                 * // inx 8-17 : List of Reconfig stream config parameters
                 * param_ptr->length = 14;
                 */
                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_ReConfig_Cfm)
            {
                /* Test for TC_AVDTP_Event_Suspend_Cfm */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_ReConfig_Cfm;   /* TC_AVDTP_Event_ReConfig_Cfm */
                param_ptr->test_cmd_rsp_buf[5] = 0x00;  /* Registeration result : 0x00 success */
                param_ptr->length = 2;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_Abort_Ind)
            {
                /* Test for TC_AVDTP_Event_Abort_Ind */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_Abort_Ind;      /* TC_AVDTP_Event_Abort_Ind */
                param_ptr->test_cmd_rsp_buf[5] = 2;

                /* inx 6-7: stream handle */
                param_ptr->test_cmd_rsp_buf[6] = 1;
                param_ptr->test_cmd_rsp_buf[7] = 0x00;

                param_ptr->length = 4;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVDTP_Event_Abort_Cfm)
            {
                /* Test for TC_AVDTP_Event_Suspend_Cfm */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x05;  /* AVDTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVDTP_Event_Abort_Cfm;      /* TC_AVDTP_Event_Abort_Cfm */
                param_ptr->test_cmd_rsp_buf[5] = 1;
                param_ptr->test_cmd_rsp_buf[6] = 0;     /* Event error code */

                param_ptr->length = 3;

                BtSendMsgToL4(param_ptr);
            }
            break;

        case TC_AVDTP_Tester_Init:
            /* cmd_type: AVDTP_TESTER_INIT */
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDTPTESTERINIT);
        #endif 
            AvdtpTesterInit();
            break;

        case TC_AVDTP_Event_Registration_Req:
            // cmd_type : AVDTP_Event_Registration_Req
            // return successful rsp here
            Send_AVDTP_Event_Registration_Req_Result(0);
            break;

        case TC_AVDTP_Connect_Req:
            /* cmd_type: AVDTP_CONNECT_REQ */
            Assert(data_len == 7);
            memcpy((U8*) connect_req_parms.bd_addr, (U8*) & test_cmd[1], 6);
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_CONNECTREQ);
        #endif 
            AVDT_ConnectReq(&connect_req_parms);
            break;

        case TC_AVDTP_Discovery_Req:
            /* cmd_type: AVDTP_DISCOVERY_REQ */
            discovery_req_parms.transaction = (U8) test_cmd[1];
            memcpy((U8*) discovery_req_parms.bd_addr, (U8*) & test_cmd[2], 6);
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_DISCOVER_REQ);
        #endif 
            AVDT_Discover_Req(&discovery_req_parms);
            break;

        case TC_AVDTP_GetCapabilities_Req:
            /* cmd_type: AVDTP_GETCAPABILITIES_REQ */
            get_capabilities_parms.transaction = (U8) test_cmd[1];
            memcpy((U8*) get_capabilities_parms.bd_addr, (U8*) & test_cmd[2], 6);
            get_capabilities_parms.acp_seid = (U8) test_cmd[8];
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_GET_CAPABILITIES_REQ);
        #endif 
            AVDT_Get_Capabilities_Req(&get_capabilities_parms);
            break;

        case TC_AVDTP_SetConfig_Req:
            /* cmd_type: AVDTP_SETCONFIG_REQ */
            set_config_parms.transaction = (U8) test_cmd[1];
            memcpy((U8*) set_config_parms.bd_addr, (U8*) & test_cmd[2], 6);
            set_config_parms.acp_seid = (U8) test_cmd[8];
            set_config_parms.int_seid = (U8) test_cmd[9];
            set_config_parms.codecType = (U8) test_cmd[10];
            set_config_parms.element_len = (U8) test_cmd[11];
            /* set zero for array's each entry */
            memset((U8*) set_config_parms.element, 0, sizeof(set_config_parms.element));
            memcpy((U8*) set_config_parms.element, (U8*) & test_cmd[12], set_config_parms.element_len);
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_SET_CONFIGURATION_REQ);
        #endif 
            AVDT_Set_Configuration_Req(&set_config_parms);

            break;

        case TC_AVDTP_ReConfig_Req:
            /* cmd_type: AVDTP_RECONFIG_REQ */
            re_config_parms.transaction = (U8) test_cmd[1];
            re_config_parms.stream_handle = (U16) (test_cmd[3] << 8 | test_cmd[2]);
            re_config_parms.codecType = (U8) test_cmd[4];
            re_config_parms.element_len = (U8) test_cmd[5];
            /* set zero for array's each entry */
            memset((U8*) re_config_parms.element, 0, sizeof(re_config_parms.element));
            memcpy((U8*) re_config_parms.element, (U8*) & test_cmd[6], re_config_parms.element_len);
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_RECONFIGURE_REQ);
        #endif 
            AVDT_ReConfigure_Req(&re_config_parms);
            break;

        case TC_AVDTP_Open_Req:
            /* cmd_type: AVDTP_OPEN_REQ */
            open_req_parms.transaction = (U8) test_cmd[1];
            open_req_parms.stream_handle = (U16) (test_cmd[3] << 8 | test_cmd[2]);
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_OPEN_REQ);
        #endif 
            AVDT_Open_Req(&open_req_parms);
            break;
        case TC_AVDTP_DisconnectSignalReq:
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_DISCONNECTSIGNAL_REQ);
        #endif 
            AVDT_DisconnectSignal_Req();
            break;
        case TC_AVDTP_Close_Req:
            /* cmd_type: AVDTP_CLOSE_REQ */
            close_req_parms.transaction = (U8) test_cmd[1];
            close_req_parms.stream_handle = (U16) (test_cmd[3] << 8 | test_cmd[2]);
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_CLOSE_REQ);
        #endif 
            AVDT_Close_Req(&close_req_parms);
            break;

        case TC_AVDTP_Start_Req:
            /* cmd_type: AVDTP_START_REQ */
            start_req_parms.transaction = (U8) test_cmd[1];
            /* check this: API_doc only list 3 stream handles, but code's define 50 */
            start_req_parms.stream_handle[0] = (U16) (test_cmd[3] << 8 | test_cmd[2]);
            start_req_parms.stream_handle[1] = (U16) (test_cmd[5] << 8 | test_cmd[4]);
            start_req_parms.stream_handle[2] = (U16) (test_cmd[7] << 8 | test_cmd[6]);
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_START_REQ);
        #endif 
            AVDT_Start_Req(&start_req_parms);
            break;

        case TC_AVDTP_Suspend_Req:
            /* cmd_type: AVDTP_SUSPEND_REQ */
            suspend_req_parms.transaction = (U8) test_cmd[1];
            suspend_req_parms.stream_handle[0] = (U16) (test_cmd[3] << 8 | test_cmd[2]);
            suspend_req_parms.stream_handle[1] = (U16) (test_cmd[5] << 8 | test_cmd[4]);
            suspend_req_parms.stream_handle[2] = (U16) (test_cmd[7] << 8 | test_cmd[6]);
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_SUSPEND_REQ);
        #endif 
            AVDT_Suspend_Req(&suspend_req_parms);
            break;

        case TC_AVDTP_Abort_Req:
            /* cmd_type: AVDTP_ABORT_REQ */
            abort_req_parms.transaction = (U8) test_cmd[1];
            abort_req_parms.stream_handle = (U16) (test_cmd[3] << 8 | test_cmd[2]);
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_ABORT_REQ);
        #endif 
            AVDT_Abort_Req(&abort_req_parms);
            break;

        case TC_AVDTP_Discover_Rsp:
            /* cmd_type: AVDTP_DISCOVERY_RSP */
            discovery_res_parms.transaction = (U8) test_cmd[1];
            memcpy((U8*) discovery_res_parms.bd_addr, (U8*) & test_cmd[2], 6);
            discovery_res_parms.stream_info_num = (U8) test_cmd[8];
            /* set zero for array's each entry first */
            memset((U8*) discovery_res_parms.stream_info, 0, sizeof(discovery_res_parms.stream_info));
            memcpy((U8*) discovery_res_parms.stream_info, (U8*) & test_cmd[9], discovery_res_parms.stream_info_num);
            /* Assume that the data I sent, without zero-padding */
            discovery_res_parms.error_code = (U8) test_cmd[9 + discovery_res_parms.stream_info_num];
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_DISCOVER_RSP);
        #endif 
            AVDT_Discover_Rsp(&discovery_res_parms);
            break;

        case TC_AVDTP_GetCapabilities_Rsp:
            /* cmd_type: AVDTP_GETCAPABILITIES_RSP */
            get_capabilities_res_parms.transaction = (U8) test_cmd[1];
            memcpy((U8*) get_capabilities_res_parms.bd_addr, (U8*) & test_cmd[2], 6);
            /* SEID valie */
            get_capabilities_res_parms.seid = (U8) test_cmd[8];
            /* [question:] need length or not ? */
            get_capabilities_res_parms.capabilities_len = (U8) test_cmd[9];
            /* set zero for array's each entry first */
            memset((U8*) get_capabilities_res_parms.capabilities, 0, sizeof(get_capabilities_res_parms.capabilities));
            memcpy(
                (U8*) get_capabilities_res_parms.capabilities,
                (U8*) & test_cmd[10],
                get_capabilities_res_parms.capabilities_len);
            /* Assume that the data I sent, without zero-padding */
            get_capabilities_res_parms.error_code = (U8) test_cmd[10 + get_capabilities_res_parms.capabilities_len];
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_GET_CAPABILITIES_RSP);
        #endif 
            AVDT_Get_Capabilities_Rsp(&get_capabilities_res_parms);
            break;
        case TC_AVDTP_GetCapabilities_Long_Rsp:
            /* cmd_type: AVDTP_GETCAPABILITIES_RSP */
            get_capabilities_res_parms.transaction = (U8) test_cmd[1];
            memcpy((U8*) get_capabilities_res_parms.bd_addr, (U8*) & test_cmd[2], 6);

            /* SEID valie */
            get_capabilities_res_parms.seid = (U8) test_cmd[8];

            /* [question:] need length or not ? */
            get_capabilities_res_parms.capabilities_len = (U8) test_cmd[9];
            /* set zero for array's each entry first */
            memset((U8*) get_capabilities_res_parms.capabilities, 0, sizeof(get_capabilities_res_parms.capabilities));
            memcpy(
                (U8*) get_capabilities_res_parms.capabilities,
                (U8*) & test_cmd[10],
                get_capabilities_res_parms.capabilities_len);
            /* Assume that the data I sent, without zero-padding */
            get_capabilities_res_parms.error_code = (U8) test_cmd[10 + get_capabilities_res_parms.capabilities_len];
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_GET_CAPABILITIES_LONG_RSP);
        #endif 
            AVDT_Get_Capabilities_LongRsp(&get_capabilities_res_parms);
            break;
        case TC_AVDTP_SetConfig_Rsp:
            /* cmd_type: AVDTP_SETCONFIG_RSP (0803-version API modified,but struct is not modified now) */

            /*
             * set_config_rsp_parms.transaction = (U8) test_cmd[1];
             * set_config_rsp_parms.stream_handle = (U16) ( test_cmd[3]<<8 | test_cmd[2]);
             * memcpy((U8*) set_config_rsp_parms.category, (U8*)&test_cmd[4], 5);
             * set_config_rsp_parms.error_code = (U8) test_cmd[9];
             */

            set_config_rsp_parms.transaction = (U8) test_cmd[1];
            memset(set_config_rsp_parms.category, 0, 5);
            set_config_rsp_parms.category[0] = test_cmd[2];
            set_config_rsp_parms.error_code = test_cmd[3];
            set_config_rsp_parms.stream_handle = 0x0000;

        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_SET_CONFIGURATION_RSP);
        #endif 
            AVDT_Set_Configuration_Rsp(&set_config_rsp_parms);
            break;

        case TC_AVDTP_Open_Rsp:
            /* cmd_type: AVDTP_OPEN_RSP */
            open_rsp_parms.transaction = (U8) test_cmd[1];
            open_rsp_parms.stream_handle = (U16) (test_cmd[3] << 8 | test_cmd[2]);
            open_rsp_parms.error_code = (U8) test_cmd[4];
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_OPEN_RSP);
        #endif 
            AVDT_Open_Rsp(&open_rsp_parms);

            break;
        case TC_AVDTP_Close_Rsp:
            /* cmd_type: AVDTP_CLOSE_RSP */
            close_rsp_parms.transaction = (U8) test_cmd[1];
            close_rsp_parms.stream_handle = (U16) (test_cmd[3] << 8 | test_cmd[2]);
            close_rsp_parms.error_code = (U8) test_cmd[4];
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_CLOSE_RSP);
        #endif 
            AVDT_Close_Rsp(&close_rsp_parms);
            break;

        case TC_AVDTP_Start_Rsp:
            /* cmd_type: AVDTP_START_RSP */
            start_rsp_parms.transaction = (U8) test_cmd[1];
            start_rsp_parms.stream_handle = (U16) (test_cmd[3] << 8 | test_cmd[2]);
            start_rsp_parms.error_code = (U8) test_cmd[4];
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_START_RSP);
        #endif 
            AVDT_Start_Rsp(&start_rsp_parms);
            break;

        case TC_AVDTP_Suspend_Rsp:
            /* cmd_type: AVDTP_SUSPEND_RSP */
            suspend_rsp_parms.transaction = (U8) test_cmd[1];
            suspend_rsp_parms.stream_handle = (U16) (test_cmd[3] << 8 | test_cmd[2]);
            /* [question:] first_failed_stream_handle is needed? */
            suspend_rsp_parms.error_code = (U8) test_cmd[4];
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_SUSPEND_RSP);
        #endif 
            AVDT_Suspend_Rsp(&suspend_rsp_parms);
            break;

        case TC_AVDTP_Abort_Rsp:
            /* cmd_type: AVDTP_ABORT_RSP */
            abort_rsp_parms.transaction = (U8) test_cmd[1];
            abort_rsp_parms.stream_handle = (U16) (test_cmd[3] << 8 | test_cmd[2]);
            abort_rsp_parms.error_code = (U8) test_cmd[4];
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_ABORT_RSP);
        #endif 
            AVDT_Abort_Rsp(&abort_rsp_parms);
            break;

        case TC_AVDTP_ReConfig_Rsp:
            //cmd_type: AVDTP_ReConfig_Rsp (this cmd is added on 0803-API)
            //[question:] data struct of this cmd is not defined
            reconfig_parms.transaction = (U8) test_cmd[1];
            memset(reconfig_parms.category, 0, 5);
            reconfig_parms.category[0] = (U8) test_cmd[2];
            reconfig_parms.error_code = (U8) test_cmd[3];
            reconfig_parms.stream_handle = 0x0000;
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_RE_CONFIGURATION_RSP);
        #endif 
            AVDT_Re_Configuration_Rsp(&reconfig_parms);

            break;

        case TC_AVDTP_Write_Data:
            /* cmd_type: AVDTP_WRITE_DATA */
            write_data_parms.stream_handle = (U16) ((test_cmd[2] << 8) | test_cmd[1]);
            write_data_parms.length = (U16) ((test_cmd[4] << 8) | test_cmd[3]);
            if (write_data_parms.length > 20)
            {
                write_data_parms.length = 20;
            }
            memcpy(write_data_parms.outbuffer, &test_cmd[5], write_data_parms.length);
            write_data_parms.time_info =
                (U32) ((test_cmd[5 + write_data_parms.length + 3] << 24) |
                       (test_cmd[5 + write_data_parms.length + 2] << 16) | (test_cmd[5 + write_data_parms.length + 1] <<
                                                                            8) | (test_cmd[5 +
                                                                                           write_data_parms.length]));

            write_data_parms.payload_type = test_cmd[5 + write_data_parms.length + 4];
            write_data_parms.marker =
                (U16) ((test_cmd[5 + write_data_parms.length + 6] << 8) | test_cmd[5 + write_data_parms.length + 5]);
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_WRITE_DATA);
        #endif 
            AVDT_Write_Data(&write_data_parms);
            break;
        case TC_AVDTP_Read_Data:
            /* cmd type: AVDTP_READ_DATA */
            read_data_parms.stream_handle = (U16) ((test_cmd[2] << 8) | test_cmd[1]);
            read_data_parms.expected_length = (U16) ((test_cmd[4] << 8) | test_cmd[3]);
            /* if(read_data_parms.expected_length >= 4) */
            {
                read_data_parms.buffer =
                    (U32) ((test_cmd[8] << 24) | (test_cmd[7] << 16) | (test_cmd[6] << 8) | (test_cmd[5]));
            }
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_AVDT_READ_DATA);
        #endif 
            AVDT_Read_Data(&read_data_parms);
        default:
            break;

    }

}
#endif /* __AVDTP_TESTER_ */ 

#ifdef __AVCTP_TESTER_


/*****************************************************************************
 * FUNCTION
 *  bt_avctp_layer_id_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  data_len        [IN]        
 *  test_cmd        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_avctp_layer_id_hdlr(kal_uint16 data_len, kal_uint8 *test_cmd)
{
    kal_uint8 cmd_type = 0;
    AvctpConnectRequest connect_req_parms;
    AvctpConnectResponse connect_rsp_parms;
    AvctpDisconnectRequest disconnect_req_parms;
    AvctpSendMessaegReq send_msg_req_parms;
    U16 str_data_len = 0;
    local_para_struct *msg;
    bt_test_cmd_rsp_struct *param_ptr;

    cmd_type = test_cmd[0];
    switch (cmd_type)
    {
        case TC_AVCTP_EVENT_RSP_TEST:

            /* for testing rsp event */
            param_ptr =
                (bt_test_cmd_rsp_struct*) construct_local_para((kal_uint16) sizeof(bt_test_cmd_rsp_struct), TD_CTRL);

            memset(param_ptr->test_cmd_rsp_buf, 0, 256);

            if (test_cmd[1] == TC_AVCTP_Event_Init)
            {
                /* Test for TC_AVCTP_Event_Init */
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x06;  /* AVCTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVCTP_Event_Init;   /* AVCTP_Event_Init */
                param_ptr->test_cmd_rsp_buf[5] = 0x00;  /* Registeration result : 0x00 success */
                param_ptr->length = 2;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVCTP_Event_ConnectReq_Ind)
            {
                /* Test for TC_AVCTP_Event_ConnectReq_Ind */
                unsigned char addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x06;  /* AVCTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVCTP_Event_ConnectReq_Ind; /* TC_AVCTP_Event_ConnectReq_Ind */
                memcpy(&param_ptr->test_cmd_rsp_buf[5], addr, 6);
                param_ptr->length = 7;                  /* fill 1 byte AVCTP_event_type + 6 bytes BD_ADDR */

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVCTP_Event_ConnectReq_Cfm)
            {
                /* Test for TC_AVCTP_Event_ConnectReq_Cfm */
                unsigned char addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
                unsigned short connect_result = 0x0000;
                unsigned short config_result = 0x0000;
                unsigned short status = 0x0000;

                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x06;  /* AVCTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVCTP_Event_ConnectReq_Cfm; /* TC_AVCTP_Event_ConnectReq_Cfm */
                param_ptr->test_cmd_rsp_buf[5] = 0x00;  /* Registeration result : 0x00 success */
                memcpy(&param_ptr->test_cmd_rsp_buf[5], addr, 6);
                param_ptr->test_cmd_rsp_buf[11] = (unsigned char)(connect_result & 0xFF);
                param_ptr->test_cmd_rsp_buf[12] = (unsigned char)((connect_result >> 8) & 0xFF);
                param_ptr->test_cmd_rsp_buf[13] = (unsigned char)(config_result & 0xFF);
                param_ptr->test_cmd_rsp_buf[14] = (unsigned char)((config_result >> 8) & 0xFF);
                param_ptr->test_cmd_rsp_buf[15] = (unsigned char)(status & 0xFF);
                param_ptr->test_cmd_rsp_buf[16] = (unsigned char)((status >> 8) & 0xFF);
                param_ptr->length = 13;

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVCTP_Event_DisConnectReq_Ind)
            {
                /* Test for TC_AVCTP_Event_DisConnectReq_Ind */
                unsigned char addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x06;  /* AVCTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVCTP_Event_DisConnectReq_Ind;      /* TC_AVCTP_Event_DisConnectReq_Ind */
                memcpy(&param_ptr->test_cmd_rsp_buf[5], addr, 6);
                param_ptr->length = 7;                  /* fill 1 byte AVCTP_event_type + 6 bytes BD_ADDR */

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVCTP_Event_DisConnectReq_Cfm)
            {
                /* Test for TC_AVCTP_Event_DisConnectReq_Cfm */
                unsigned char addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
                unsigned short disconnect_result = 0x0000;

                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x06;  /* AVCTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVCTP_Event_DisConnectReq_Cfm;      /* TC_AVCTP_Event_DisConnectReq_Cfm */
                memcpy(&param_ptr->test_cmd_rsp_buf[5], addr, 6);
                param_ptr->test_cmd_rsp_buf[11] = (unsigned char)(disconnect_result & 0xFF);
                param_ptr->test_cmd_rsp_buf[12] = (unsigned char)((disconnect_result >> 8) & 0xFF);

                param_ptr->length = 9;  /* fill 1 byte AVCTP_event_type + 6 bytes BD_ADDR + 2 bytes disconnect_result */

                BtSendMsgToL4(param_ptr);
            }
            else if (test_cmd[1] == TC_AVCTP_Event_MessageRecReq_Ind)
            {
                /* Test for TC_AVCTP_Event_MessageRec_Ind */
                unsigned char addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
                unsigned short data_len = 50;
                unsigned char data_buf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
                    0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
                    0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E,
                    0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
                    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32
                };

                param_ptr->test_cmd_rsp_buf[0] = 0x02;  /* event type */
                param_ptr->test_cmd_rsp_buf[1] = 0x06;  /* AVCTP category */
                param_ptr->test_cmd_rsp_buf[4] = TC_AVCTP_Event_MessageRecReq_Ind;      /* TC_AVCTP_Event_MessageRecReq_Ind */
                memcpy(&param_ptr->test_cmd_rsp_buf[5], addr, 6);
                param_ptr->test_cmd_rsp_buf[11] = 1;    /* Transction ID */
                param_ptr->test_cmd_rsp_buf[12] = 2;    /* Type */
                param_ptr->test_cmd_rsp_buf[13] = (unsigned char)(data_len & 0xFF);
                param_ptr->test_cmd_rsp_buf[14] = (unsigned char)((data_len >> 8) & 0xFF);
                memcpy(&param_ptr->test_cmd_rsp_buf[15], data_buf, data_len);
                param_ptr->length = 11 + data_len;
                BtSendMsgToL4(param_ptr);
            }

            break;

        case TC_AVCTP_Tester_Init:
            /* cmd_type: AVCTP_Tester_Init */
            AvctpTesterInit();
            break;

        case TC_AVCTP_Event_Registration_Req:
            // cmd_type : AVCTP_Event_Registration_Req
            // return successful rsp here
            Send_AVCTP_Event_Registration_Req_Result(0);
            break;
        case TC_AVCTP_Connect_Req:
            /* cmd_type: AVCTP_Connect_Req */
            memcpy((U8*) connect_req_parms.bd_addr, (U8*) & test_cmd[1], 6);
            AVCT_ConnectReq(&connect_req_parms);
            break;

        case TC_AVCTP_Connect_Rsp:
            /* cmd_type: AVCTP_Connect_Rsp */
            memcpy((U8*) connect_rsp_parms.bd_addr, (U8*) & test_cmd[1], 6);
            connect_rsp_parms.connect_result = test_cmd[7];
            AVCT_ConnectRsp(&connect_rsp_parms);
            break;

        case TC_AVCTP_DisConnect_Req:
            /* cmd_type: AVCTP_DisConnect_Req */
            memcpy((U8*) disconnect_req_parms.bd_addr, (U8*) & test_cmd[1], 6);
            disconnect_req_parms.pid = (U16) ((test_cmd[8] << 8) | (test_cmd[7]));
            AVCT_DisconnectReq(&disconnect_req_parms);
            break;

        case TC_AVCTP_SendMsg_Req:
            /* cmd_type: AVCTP_SendMessage_Req */
            memcpy((U8*) send_msg_req_parms.bd_addr, (U8*) & test_cmd[1], 6);
            send_msg_req_parms.transaction_id = test_cmd[7];
            send_msg_req_parms.type = test_cmd[8];
            send_msg_req_parms.pid = (U16) ((test_cmd[10] << 8) | (test_cmd[9]));
            send_msg_req_parms.length = (U16) ((test_cmd[12] << 8) | (test_cmd[11]));
            /* set zero for array's each entry first */
            AVCT_SendMessageReq(&send_msg_req_parms);
            break;

        case TC_AVCTP_SendLongMsg_Req:
            /* cmd_type:AVCTP_SendLongMsg_Req */
            memcpy((U8*) send_msg_req_parms.bd_addr, (U8*) & test_cmd[1], 6);
            send_msg_req_parms.transaction_id = test_cmd[7];
            send_msg_req_parms.type = test_cmd[8];
            send_msg_req_parms.pid = (U16) ((test_cmd[10] << 8) | (test_cmd[9]));
            send_msg_req_parms.length = (U16) ((test_cmd[12] << 8) | (test_cmd[11]));
            /* set zero for array's each entry first */
            memset((U8*) send_msg_req_parms.data, 0, sizeof(send_msg_req_parms.data));
            memcpy((U8*) send_msg_req_parms.data, (U8*) & test_cmd[13], send_msg_req_parms.length);
            //[0827] call LongMsg_Req function? not yet implement!    
            //AVCT_SendMessageReq(&send_msg_req_parms);

            break;

        case TC_AVCTP_ReadMsg_Req:
            /* cmd_type:AVCTP_ReadMsg_Req */
            memcpy((U8*) send_msg_req_parms.bd_addr, (U8*) & test_cmd[1], 6);
            send_msg_req_parms.transaction_id = test_cmd[7];
            send_msg_req_parms.type = test_cmd[8];
            send_msg_req_parms.pid = (U16) ((test_cmd[10] << 8) | (test_cmd[9]));
            /* Specify how many sizes of data to read ? */
            send_msg_req_parms.length = (U16) ((test_cmd[12] << 8) | (test_cmd[11]));

            /* [0827] call which function? not yet implement! */

            break;

        default:
            break;

    }

}
#endif /* __AVCTP_TESTER_ */ 


/*****************************************************************************
 * FUNCTION
 *  BtSendMsgToL4
 * DESCRIPTION
 *  
 * PARAMETERS
 *  param_ptr       [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void BtSendMsgToL4(bt_test_cmd_rsp_struct *param_ptr)
{
    local_para_struct *msg;
    module_type dst_mod = 0;

    /* fill the data length field of string data for rsp */
    param_ptr->test_cmd_rsp_buf[2] = (U8) (param_ptr->length & 0x00ff);
    param_ptr->test_cmd_rsp_buf[3] = (U8) ((param_ptr->length >> 8) & 0x00ff);

    /* rsp_buf's total length = str_data_len + 4 bytes header */
    param_ptr->length += 4;

    msg = (local_para_struct*) param_ptr;

#ifdef MTK_BT_DEBUG_TRACE
    kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCALL_BTTESTER_SENDMSG);
#endif 

#ifdef __GEMINI__
    dst_mod = Get_DualSim_Uart_Default_Owner();
#else 
    dst_mod = MOD_ATCI;
#endif 

    btTester_sendMsg(MSG_ID_BT_L4C_TEST_CMD_CNF, dst_mod, INVALID_SAP, msg, NULL);

}

#endif /* __BQE_TESTER_ */ 

