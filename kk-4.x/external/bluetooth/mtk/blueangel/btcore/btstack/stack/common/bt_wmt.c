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


#include "bt_common.h"
#include "bt_feature.h"
#include "btalloc.h"
#include "btconfig.h"
#include "bt_wmt_api.h"
#include "osapi.h"
#include "bt_message.h"
#include "bluetooth_bwcs.h"
#include "wmt_struct.h"
#include "bt_wmt_porting.h"
#include "me.h"

#ifdef __BT_SINGLE_ANTENNA__

#define ILM_HEADER_SIZE (sizeof(ilm_struct)-MAX_ILM_BUFFER_SIZE)
#define CAL_ILM_SIZE(type) (ILM_HEADER_SIZE+sizeof(type))

extern U8 BTQueryPowerOnState(void);

/* Call by btstack, send message to WMT */
void BT_WMT_Send_RSSI_Indication(U8 *addr, S8 rssi, U8 result)
{
#ifdef ANDROID
#if 1
    wmt_bt_query_rssi_ind_struct *ind =
        (wmt_bt_query_rssi_ind_struct*) construct_local_para(sizeof(wmt_bt_query_rssi_ind_struct), TD_RESET);
    OS_MemCopy(ind->bd_address, addr, 6);
    ind->is_valid = result;
    ind->bt_rssi = rssi;
    BT_SendMessage(MSG_ID_WMT_BT_QUERY_RSSI_IND, MOD_BWCS, ind, sizeof(wmt_bt_query_rssi_ind_struct));
#else
    ilm_struct ilm;
    wmt_bt_query_rssi_ind_struct *ptr = (wmt_bt_query_rssi_ind_struct*)ilm.ilm_data;
    ilm.msg_id = MSG_ID_WMT_BT_QUERY_RSSI_IND;
    OS_MemCopy(ptr->bd_address, addr, 6);
    //memcpy(ptr->bd_address, addr, 6);
    ptr->is_valid = result;
    ptr->bt_rssi = rssi;
    sent_bwcs_msg((void*)&ilm, CAL_ILM_SIZE(wmt_bt_query_rssi_ind_struct));
#endif
#else
    wmt_bt_query_rssi_ind_struct *ptr =
        construct_local_para(sizeof(wmt_bt_query_rssi_ind_struct), TD_UL | TD_RESET);
    memcpy(ptr->bd_address, addr, 6);
    ptr->is_valid = result;
    ptr->bt_rssi = rssi;
    bt_send_msg(MSG_ID_BT_WMT_QUERY_RSSI_IND, MOD_BWCS, BWCS_BT_SAP, ptr, NULL);
#endif    
}

/* Call by btstack, send message to WMT */
void BT_WMT_Send_Set_Rx_Range_Cnf(void)
{
#ifdef ANDROID
#if 1
    BT_SendMessage(MSG_ID_WMT_BT_SET_RX_RANGE_CNF, MOD_BWCS, NULL, 0);
#else
    ilm_struct ilm;
    ilm.msg_id = MSG_ID_WMT_BT_SET_RX_RANGE_CNF;
    sent_bwcs_msg((void*)&ilm, ILM_HEADER_SIZE);
#endif
#else
    bt_send_msg(MSG_ID_WMT_BT_SET_RX_RANGE_CNF, MOD_BWCS, BWCS_BT_SAP, NULL, NULL);
#endif
}

/* Call by btstack, send message to WMT */

void BT_WMT_Send_Tx_Default_Power_Cnf(void)
{
#ifdef ANDROID
#if 1
    BT_SendMessage(MSG_ID_WMT_BT_SET_DEFAULT_TX_POWER_CNF, MOD_BWCS, NULL, 0);
#else
    ilm_struct ilm;
    ilm.msg_id = MSG_ID_WMT_BT_SET_DEFAULT_TX_POWER_CNF;
    sent_bwcs_msg((void*)&ilm, ILM_HEADER_SIZE);
#endif
#else
    bt_send_msg(MSG_ID_WMT_BT_SET_DEFAULT_TX_POWER_CNF, MOD_BWCS, BWCS_BT_SAP, NULL, NULL);
#endif
}

/* Call by btstack, send message to WMT */
void BT_WMT_Send_Tx_Update_Power_Cnf(void)
{
#ifdef ANDROID
#if 1
    BT_SendMessage(MSG_ID_WMT_BT_UPDATE_CONN_TX_POWER_CNF, MOD_BWCS, NULL, 0);
#else
    ilm_struct ilm;
    ilm.msg_id = MSG_ID_WMT_BT_UPDATE_CONN_TX_POWER_CNF;
    sent_bwcs_msg((void*)&ilm, ILM_HEADER_SIZE);
#endif
#else
    bt_send_msg(MSG_ID_WMT_BT_UPDATE_CONN_TX_POWER_CNF, MOD_BWCS, BWCS_BT_SAP, NULL, NULL);
#endif
}

/* Call by btstack, send message to WMT */
void BT_WMT_A2DP_Send_Status(BtRemoteDevice *link, U8 a2dp_state)
{
#ifdef ANDROID
    unsigned short event;
    switch(a2dp_state) {
    case 0:
        event = BWCS_BT_AVDTP_SUSPEND;
        break;
    case 1:
        event = BWCS_BT_AVDTP_STREAM;
        break;
    case 3:
        event = BWCS_BT_AVDTP_DISCONN;
        break;
    default:
        return;
    }
    avdtp_bwcs_send_internal_event(link->hciHandle, link->bdAddr.addr, event);
#else
    wmt_bt_connection_status_ind_struct *ptr =
        construct_local_para(sizeof(wmt_bt_connection_status_ind_struct), TD_UL | TD_RESET);
    ptr->bt_eventid = WMT_BT_AVDTP_STATE;
    if(a2dp_state == 0)
        ptr->bt_avdtp_state = BT_AVDTP_STATE_SUSPENDED;
    if(a2dp_state == 1)
        ptr->bt_avdtp_state = BT_AVDTP_STATE_STREAMING;
    if(a2dp_state == 3)
        ptr->bt_avdtp_state = BT_AVDTP_STATE_DISCONNECTED;
    ptr->valid_conn_num = 0x01;
    ptr->bt_conn.bt_avdtp_state = ptr->bt_avdtp_state;
    memcpy(ptr->bt_conn.bd_address, link->bdAddr.addr, 6);
    ptr->bt_conn.bt_conn_handle = link->hciHandle;
    ptr->bt_conn.bt_link_type = 0x01;
    bt_send_msg(MSG_ID_BT_WMT_CONN_STATUS_UPDATE_IND, MOD_BWCS, BWCS_BT_SAP, ptr, NULL);
#endif    
}

/* Call by btstack, send message to WMT */
void BT_WMT_Stack_Send_Status(U8 on_off)
{
#ifdef ANDROID
    bwcs_send_internal_event(on_off ? WMT_BT_STACK_UP : WMT_BT_STACK_DOWN);
#else
    wmt_bt_connection_status_ind_struct *ptr =
        construct_local_para(sizeof(wmt_bt_connection_status_ind_struct), TD_UL | TD_RESET);
    if(on_off == 0)
        ptr->bt_eventid = WMT_BT_STACK_DOWN;
    else
        ptr->bt_eventid = WMT_BT_STACK_UP;
    ptr->valid_conn_num = 0x0;
    bt_send_msg(MSG_ID_BT_WMT_CONN_STATUS_UPDATE_IND, MOD_BWCS, BWCS_BT_SAP, ptr, NULL);
#endif
}

/* Call by btstack, send message to WMT */
void BT_WMT_ACL_Send_Status(BtRemoteDevice *link, U8 on_off)
{
#ifdef ANDROID
    sent_aclsco_connection_info_event(link->bdAddr.addr, link->hciHandle, 0x01, (on_off != 0) ? BWCS_BT_CONN : BWCS_BT_DISCONN);
#else
    wmt_bt_connection_status_ind_struct *ptr =
        construct_local_para(sizeof(wmt_bt_connection_status_ind_struct), TD_UL | TD_RESET);
    if(on_off == 0)
        ptr->bt_eventid = WMT_BT_DISCONNECTION;
    else
        ptr->bt_eventid = WMT_BT_CONNECTION;
    ptr->valid_conn_num = 0x1;
    ptr->bt_conn.bt_avdtp_state = BT_AVDTP_STATE_NONE;
    memcpy(ptr->bt_conn.bd_address, link->bdAddr.addr, 6);
    ptr->bt_conn.bt_conn_handle = link->hciHandle;
    ptr->bt_conn.bt_link_type = 0x01;
    ptr->bt_avdtp_state = BT_AVDTP_STATE_NONE;
    bt_send_msg(MSG_ID_BT_WMT_CONN_STATUS_UPDATE_IND, MOD_BWCS, BWCS_BT_SAP, ptr, NULL);
#endif    
}

/* Call by btstack, send message to WMT */
void BT_WMT_SCO_Send_Status(BtRemoteDevice *link, U8 on_off)
{
#ifdef ANDROID
    sent_aclsco_connection_info_event(link->bdAddr.addr, link->hciHandle, 0, (on_off != 0) ? BWCS_BT_CONN : BWCS_BT_DISCONN);
#else
    wmt_bt_connection_status_ind_struct *ptr =
        construct_local_para(sizeof(wmt_bt_connection_status_ind_struct), TD_UL | TD_RESET);
    //Report(("BT_WMT_SCO_Send_Status"));
    if(on_off == 0)
        ptr->bt_eventid = WMT_BT_DISCONNECTION;
    else
        ptr->bt_eventid = WMT_BT_CONNECTION;
    ptr->valid_conn_num = 0x1;
    ptr->bt_conn.bt_avdtp_state = BT_AVDTP_STATE_NONE;
    memcpy(ptr->bt_conn.bd_address, link->bdAddr.addr, 6);
    ptr->bt_conn.bt_conn_handle = link->hciHandle;
    ptr->bt_conn.bt_link_type = 0x00;
    ptr->bt_avdtp_state = BT_AVDTP_STATE_NONE;
    bt_send_msg(MSG_ID_BT_WMT_CONN_STATUS_UPDATE_IND, MOD_BWCS, BWCS_BT_SAP, ptr, NULL);
#endif    
}

/* Call when WMT rx message */
void BT_WMT_Set_Range_handler(U8 high_range, U8 low_range)
{
    ME_SetRangeRequest(high_range, low_range);
}

/* Call when WMT rx message */
void BT_WMT_Set_Default_Tx_Power_handler(U8 ucPower)
{
    ME_SetDefaultPower(ucPower);
}

/* Call when WMT rx message */
void BT_WMT_Set_Update_Tx_Power_handler(U32 con_hdl, U8 ucPower)
{
    ME_SetUpdatePower(con_hdl, ucPower);
}

/* Call when WMT rx message */
void BT_WMT_Query_RSSI_handler(U32 conn_handle)
{
    U8 addr[6];
    U8 i=0;
    BtRemoteDevice *link = NULL;
    U8 found = 0;

    //OS_MemCopy(addr, input_addr, 6);

    while( found == 0 && i < NUM_BT_DEVICES )
    {
        link = (BtRemoteDevice *)&MEC(devTable[i++]);
        if(link->state == BDS_CONNECTED)
        {
            //if(OS_MemCmp(addr, 6, link->bdAddr.addr, 6))
            if(link->hciHandle == conn_handle)
            {
                OS_MemCopy(addr, link->bdAddr.addr, 6);
                found = 1;
            }
        }
    }

    if(found == 0)
    {
        BT_WMT_Send_RSSI_Indication(addr, 1, 0x00);
    }
    else
    {
        if (ME_ReadWifiRSSI(link) != BT_STATUS_PENDING)
        {
            BT_WMT_Send_RSSI_Indication(addr, 2, 0x00);
        }
    }

}

void BT_WMT_Query_State_handler()
{
#if 1
    wmt_bt_query_state_cnf_struct *cnf =
        (wmt_bt_query_state_cnf_struct*) construct_local_para(sizeof(wmt_bt_query_state_cnf_struct), TD_RESET);
    if ( BTQueryPowerOnState() == 0 ) {
        Report(("BT in status deinit"));
        cnf->state = 0;
    }else {
        Report(("BT in status init"));
        cnf->state = 1;
    }
    BT_SendMessage(MSG_ID_WMT_BT_QUERY_STATE_CNF, MOD_BWCS, cnf, sizeof(wmt_bt_query_state_cnf_struct));
#else
    ilm_struct ilm;
    ilm.msg_id = MSG_ID_WMT_BT_QUERY_STATE_CNF;
    wmt_bt_query_state_cnf_struct *cnf = (wmt_bt_query_state_cnf_struct*)ilm.ilm_data;
    if ( BTCoreVerifySysInitState() == 0 ) {
        cnf->state = 0;
    }else {
        cnf->state = 1;
    }

    sent_bwcs_msg((void*)&ilm, CAL_ILM_SIZE(wmt_bt_query_state_cnf_struct));
#endif
}

#endif

