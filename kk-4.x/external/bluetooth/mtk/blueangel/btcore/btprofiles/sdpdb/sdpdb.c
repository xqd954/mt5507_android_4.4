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
*  permission of MediaTek Inc. (C) 2006
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
 * SdpDb.h
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions provide to each profile to get SDP Data base service.
 *
 * Author:
 * -------
 * Dlight Ting(mtk01239)
 *      Create 2006/2/9
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
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
 * 09 03 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integration BT solution.
 *
 * Jun 27 2008 mtk01411
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Jul 15 2007 mtk01411
 * [MAUI_00416972] [Bluetooth] SPP with SCO functionality
 * 
 *
 * Apr 23 2007 mtk01411
 * [MAUI_00384849] [Bluetooth SPP Client] SPP Client and number of virtual ports modification
 * 
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Nov 27 2006 mtk01239
 * [MAUI_00346296] [Bluetooth] Sync BT Host protocol code.
 * 
 *
 * Oct 4 2006 mtk01239
 * [MAUI_00334713] [Bluetooth]Remove Report to kal_trace
 * 
 *
 * Jul 31 2006 mtk01239
 * [MAUI_00214015] [BT] update BT code and add OPP, FTP SDP record
 * 
 *
 * Jul 16 2006 mtk01239
 * [MAUI_00210782] [Bluetooth][ESI]update Bluetooth codes
 * 
 *
 * Apr 24 2006 mtk01239
 * [MAUI_00189553] ESI Bluetooth project update
 * update
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#include "bt_common.h"
#include "stdio.h"      /* Basic C I/O functions */
#include "bt.h"
#include "eventmgr.h"
#include "bluetooth_sdp_struct.h"

#include "osapi.h"
#include "sdp.h"
#include "sdpdb.h"
#include "spp.h"
#include "hs_gw.h"
#include "avrcp.h"

void sdpdba_handler(ilm_struct *ilm_ptr);

extern const U8 dialupServiceName[];
extern const U8 faxServiceName[];

extern void BTAppHandleGetAvailableRecordHandler(bt_app_sdpdb_get_handle_req_struct *);
extern void BTAppHandleRegisterRecord(bt_app_sdpdb_register_req_struct *ptr);
extern void BTAppHandleDeRegisterRecord(bt_app_sdpdb_deregister_req_struct *ptr);
extern void BTAppHandleRetrieveRecordCnf(bt_app_sdpdb_retrieve_record_cnf_struct *ptr);

/*****************************************************************************
 * FUNCTION
 *  SDPDB_GetLocalServiceName
 * DESCRIPTION
 *  This function is used to get the service provider name and name length
 * PARAMETERS
 *  service_type        [IN]        The kind of service such as SC_DIALUP_NETWORKING, SC_FAX, SC_LAN_ACCESS_PPP, SC_SERIAL_PORT
 *  service_name        [IN]        The pointer points to the service record name.
 * RETURNS
 *  U8 : The length of service name
 *****************************************************************************/
U8 SDPDB_GetLocalServiceName(U16 service_type, const U8 **service_name)
{

#if DUN_SERVER == XA_ENABLED
    if (service_type == SC_DIALUP_NETWORKING)
    {
        *service_name = dialupServiceName;
    }
    else
#endif /* DUN_SERVER == XA_ENABLED */

#if FAX_SERVER == XA_ENABLED
    if (service_type == SC_FAX)
    {
        *service_name = faxServiceName;
    }
    else
#endif /* FAX_SERVER == XA_ENABLED */

#if (SPP_SERVER == XA_ENABLED) && (NUM_SPP_SRVS > 0)
    if ((service_type >= SC_SERIAL_PORT0) && (service_type <= (SC_SERIAL_PORT0 + NUM_SPP_SRVS - 1)))
    {
        (*service_name) = SppSdpDB_GetServiceName(service_type);

    }
    else
#endif /* (SPP_SERVER == XA_ENABLED) && (NUM_SPP_SRVS > 0) */

#if SPP_SCO_SERVER == XA_ENABLED
    if ((service_type >= SC_SPP_SCO0) && (service_type <= (SC_SPP_SCO0 + NUM_SPP_SCO_SRVS - 1)))
    {
        (*service_name) = SppSdpDB_GetServiceName(service_type);

    }
    else
#endif /* SPP_SCO_SERVER == XA_ENABLED */
    {
        *service_name = 0;
        return 0;
    }

    return (U8) OS_StrLen((const char*)(*service_name));
}


/*****************************************************************************
 * FUNCTION
 *  SDPDB_GetLocalServiceAttribute
 * DESCRIPTION
 *  This function is used to get the local service record by service type
 * PARAMETERS
 *  service_type        [IN]        The kind of service such as SC_DIALUP_NETWORKING, SPP, LAP, DUN, FAX, HSPAG, HFPAG, A2DP SNK/SRC, AVRCP TG/CTService
 *  attribute           [IN]        
 *  srv_chn             [OUT]       The RFCOMM service channel of the service
 * RETURNS
 *  U8 : The number of return attribute number
 *****************************************************************************/
U8 SDPDB_GetLocalServiceAttribute(U16 service_type, SdpAttribute **attribute, U8 *srv_chn)
{
    U8 attribute_num = 0;

#if DUN_SERVER == XA_ENABLED
    if (service_type == SC_DIALUP_NETWORKING)
    {
        attribute_num = SppSdpDB_GetAttribute(SC_DIALUP_NETWORKING, attribute, srv_chn);
    }
    else
#endif /* DUN_SERVER == XA_ENABLED */

#if FAX_SERVER == XA_ENABLED
    if (service_type == SC_FAX)
    {
        attribute_num = SppSdpDB_GetAttribute(SC_FAX, attribute, srv_chn);
    }
    else
#endif /* FAX_SERVER == XA_ENABLED */

#if (SPP_SERVER == XA_ENABLED) && (NUM_SPP_SRVS >0)
    if ((service_type >= SC_SERIAL_PORT0) && (service_type <= (SC_SERIAL_PORT0 + NUM_SPP_SRVS - 1)))
    {
        attribute_num = SppSdpDB_GetAttribute(service_type, attribute, srv_chn);
    }
    else
#endif /* (SPP_SERVER == XA_ENABLED) && (NUM_SPP_SRVS >0) */

#if SPP_SCO_SERVER == XA_ENABLED
    if ((service_type >= SC_SPP_SCO0) && (service_type <= (SC_SPP_SCO0 + NUM_SPP_SCO_SRVS - 1)))
    {
        attribute_num = SppSdpDB_GetAttribute(service_type, attribute, srv_chn);

    }
    else
#endif /* SPP_SCO_SERVER == XA_ENABLED */

#if BT_HSP_GATEWAY == XA_ENABLED
    if (service_type == SC_HEADSET_AUDIO_GATEWAY)
    {
        attribute_num = HspSdpDB_GetAttribute(SC_HEADSET_AUDIO_GATEWAY, attribute, srv_chn);
    }
    else
#endif /* BT_HSP_GATEWAY == XA_ENABLED */
#ifdef __BT_HFG_PROFILE__
    if (service_type == SC_HANDSFREE_AUDIO_GATEWAY)
    {
        attribute_num = HfpagSdpDB_GetAttribute(SC_HANDSFREE_AUDIO_GATEWAY, attribute, srv_chn);
    }
    else 
#endif /* __BT_HFG_PROFILE__ */
    if (service_type == SC_AUDIO_SOURCE)
    {
        attribute_num = A2dpSdpDB_GetAttribute(SC_AUDIO_SOURCE, attribute);
    }
    else if (service_type == SC_AUDIO_SINK)
    {
        attribute_num = A2dpSdpDB_GetAttribute(SC_AUDIO_SINK, attribute);
    }
#if AVRCP_APP == XA_ENABLED
    else if (service_type == SC_AV_REMOTE_CONTROL_TARGET)
    {
        attribute_num = AvrcpSdpDB_GetAttribute(SC_AV_REMOTE_CONTROL_TARGET, attribute, srv_chn);
    }
    else if (service_type == SC_AV_REMOTE_CONTROL)
    {
        attribute_num = AvrcpSdpDB_GetAttribute(SC_AV_REMOTE_CONTROL, attribute, srv_chn);
    }
#endif /* AVRCP_APP == XA_ENABLED */
    else
    {
        *attribute = 0;
        return 0;
    }

    return attribute_num;
}


/*****************************************************************************
 * FUNCTION
 *  sdpdba_sendMsg
 * DESCRIPTION
 *  
 * PARAMETERS
 *  msg             [IN]        
 *  dstMod          [IN]        
 *  sap             [IN]        
 *  local_para      [IN]        
 *  peer_buff       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void sdpdba_sendMsg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{
/*    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = msg;
    ilmPtr->local_para_ptr = local_para;
    ilmPtr->peer_buff_ptr = peer_buff;
    ilmPtr->dest_mod_id = dstMod;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = sap;
    msg_send_ext_queue(ilmPtr);*/

    if (local_para != NULL)
    {
        BT_SendMessage(msg, MOD_EXTERNAL, local_para, local_para->msg_len);
    }
    else
    {
        BT_SendMessage(msg, MOD_EXTERNAL, NULL, 0);
    }
}


/*****************************************************************************
 * FUNCTION
 *  sdpdba_handler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void sdpdba_handler(ilm_struct *ilm_ptr)
{
    U8 status;
    bt_sdpdb_register_cnf_struct *rsp_msg;

    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_SDPDB_REGISTER_REQ:
            status = ObexSdpDB_Registration(((bt_sdpdb_register_req_struct*) ilm_ptr->local_para_ptr)->uuid);
            kal_trace(TRACE_GROUP_1, REGISTRATION_RESULTx02X, status);
            rsp_msg =
                (bt_sdpdb_register_cnf_struct*) construct_local_para(sizeof(bt_sdpdb_register_cnf_struct), TD_UL);
            rsp_msg->uuid = ((bt_sdpdb_register_req_struct*) ilm_ptr->local_para_ptr)->uuid;
            rsp_msg->result = status;
            sdpdba_sendMsg(
                MSG_ID_BT_SDPDB_REGISTER_CNF,
                ilm_ptr->src_mod_id,
                BT_APP_SAP,
                (local_para_struct*) rsp_msg,
                NULL);
            break;
        case MSG_ID_BT_SDPDB_DEREGISTER_REQ:
            status = ObexSdpDB_DeRegistration(((bt_sdpdb_deregister_req_struct*) ilm_ptr->local_para_ptr)->uuid);
            kal_trace(TRACE_GROUP_1, DEREGISTRATION_RESULTx02X, status);
            rsp_msg =
                (bt_sdpdb_register_cnf_struct*) construct_local_para(sizeof(bt_sdpdb_deregister_cnf_struct), TD_UL);
            rsp_msg->uuid = ((bt_sdpdb_deregister_req_struct*) ilm_ptr->local_para_ptr)->uuid;
            rsp_msg->result = status;
            sdpdba_sendMsg(
                MSG_ID_BT_SDPDB_DEREGISTER_CNF,
                ilm_ptr->src_mod_id,
                BT_APP_SAP,
                (local_para_struct*) rsp_msg,
                NULL);
            break;
#if BT_JSR82_ENABLED
        case MSG_ID_BT_APP_SDPDB_GET_HANDLE_REQ:
            BTAppHandleGetAvailableRecordHandler((bt_app_sdpdb_get_handle_req_struct *)ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_APP_SDPDB_REGISTER_REQ:
            BTAppHandleRegisterRecord((bt_app_sdpdb_register_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_APP_SDPDB_DEREGISTER_REQ:
            BTAppHandleDeRegisterRecord((bt_app_sdpdb_deregister_req_struct*) ilm_ptr->local_para_ptr);
            break;
        case MSG_ID_BT_APP_SDPDB_RETRIEVE_RECOED_CNF:
            BTAppHandleRetrieveRecordCnf((bt_app_sdpdb_retrieve_record_cnf_struct*) ilm_ptr->local_para_ptr);
            break;
#endif
        default:
            kal_trace(TRACE_GROUP_1, UNKNOWN_MESSAGE);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  sdpdba_handler_inject_msg
 * DESCRIPTION
 *  
 * PARAMETERS
 *  string      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void sdpdba_handler_inject_msg(char *string)
{
    #ifdef __LINUX_SUPPRESS_ERROR__
    #else
    kal_trace(TRACE_GROUP_1, SDPDBA_RX_CMD_STRING);
    if (OS_StrCmp(string, "regopp") == 0)
    {
        bt_sdpdb_register_req_struct *req_msg;

        kal_trace(TRACE_GROUP_1, REGOPP);
        req_msg = (bt_sdpdb_register_req_struct*) construct_local_para(sizeof(bt_sdpdb_register_req_struct), TD_UL);

        req_msg->uuid = (U32) SC_OBEX_OBJECT_PUSH;
        sdpdba_sendMsg(MSG_ID_BT_SDPDB_REGISTER_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) req_msg, NULL);
    }
    else if (OS_StrCmp(string, "deregopp") == 0)
    {
        bt_sdpdb_deregister_req_struct *req_msg;

        kal_trace(TRACE_GROUP_1, DEREGOPP);
        req_msg =
            (bt_sdpdb_deregister_req_struct*) construct_local_para(sizeof(bt_sdpdb_deregister_req_struct), TD_UL);

        req_msg->uuid = (U32) SC_OBEX_OBJECT_PUSH;
        sdpdba_sendMsg(MSG_ID_BT_SDPDB_DEREGISTER_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) req_msg, NULL);
    }
    else if (OS_StrCmp(string, "regftp") == 0)
    {
        bt_sdpdb_register_req_struct *req_msg;

        kal_trace(TRACE_GROUP_1, REGFTP);
        req_msg = (bt_sdpdb_register_req_struct*) construct_local_para(sizeof(bt_sdpdb_register_req_struct), TD_UL);

        req_msg->uuid = (U32) SC_OBEX_FILE_TRANSFER;
        sdpdba_sendMsg(MSG_ID_BT_SDPDB_REGISTER_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) req_msg, NULL);
    }
    else if (OS_StrCmp(string, "deregftp") == 0)
    {
        bt_sdpdb_deregister_req_struct *req_msg;

        kal_trace(TRACE_GROUP_1, DEREGFTP);
        req_msg =
            (bt_sdpdb_deregister_req_struct*) construct_local_para(sizeof(bt_sdpdb_deregister_req_struct), TD_UL);

        req_msg->uuid = (U32) SC_OBEX_FILE_TRANSFER;
        sdpdba_sendMsg(MSG_ID_BT_SDPDB_DEREGISTER_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) req_msg, NULL);
    }
    else if (OS_StrCmp(string, "getoppchn") == 0)
    {
        U8 server_channel = 0;

        server_channel = ObexSdpDB_GetServChn((U32) SC_OBEX_OBJECT_PUSH, 0);
        kal_trace(TRACE_GROUP_1, OPP_SERV_CHANNELx02X, server_channel);
    }
    else if (OS_StrCmp(string, "getftpchn") == 0)
    {
        U8 server_channel = 0;

        server_channel = ObexSdpDB_GetServChn((U32) SC_OBEX_FILE_TRANSFER, 0);
        kal_trace(TRACE_GROUP_1, FTP_SERV_CHANNELx02X, server_channel);
    }
    #endif
}

