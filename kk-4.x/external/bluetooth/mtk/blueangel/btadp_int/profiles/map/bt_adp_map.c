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
 *   bt_adp_map.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file contains primitive/event handlers for Message Access Profile (MAP).
 *
 * Author:
 * -------
 *   Autumn Li
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
#if defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__)

#include <stdio.h>

#include "bt_common.h"
#include "btconfig.h"
#include "bttypes.h"

#include "bluetooth_map_message.h"
#include "bluetooth_map_struct.h"

#include "obex.h"
#include "goep.h"

#include "sec_adp.h"
#include "sdp_adp.h"
#include "map_adp.h"
#include "map.h"
#include "map_obs.h"

#include "btbm.h"
#include "bt_adp_fs.h"
#include "bt_ucs2.h"

typedef enum
{
    MAP_SERVER_STATE_NULL,
    MAP_SERVER_STATE_REGISTERED
} map_adp_server_state_enum;

typedef enum
{
    MAP_CLIENT_STATE_NULL,
    MAP_CLIENT_STATE_REGISTERED
} map_adp_client_state_enum;

typedef enum
{
    MAP_ADP_CONN_STATE_NULL,
    MAP_ADP_CONN_STATE_CONNECTING,
    MAP_ADP_CONN_STATE_CONNECTED,
    MAP_ADP_CONN_STATE_DISCONNECTING
} map_adp_conn_state_enum;

typedef struct
{
    map_adp_conn_state_enum state;
    U8 id;
    bt_map_addr_struct addr;
    map_obj_struct *push_pull_obj;
} map_conn_struct;

typedef struct
{
    U8 instance;
    map_adp_server_state_enum state;
    U8 conn_num;
    map_conn_struct conn[NUM_MAP_SRV_CONN];
} map_server_struct;

typedef struct
{
    map_adp_client_state_enum state;
    U8 instance;
    map_conn_struct conn;
} map_client_struct;

typedef struct
{
    BOOL initialized;

    U8 server_num;
#if NUM_MAP_SRVS > 0
    map_server_struct server[NUM_MAP_SRVS];
#endif
#if NUM_MAP_SRV_CONN > 0
    map_conn_struct   mns_conn[NUM_MAP_SRV_CONN];
#endif

    map_client_struct client;
    map_conn_struct   mns_server_conn;
} map_adp_context;

extern void CONVERT_BDADDRSRC2ARRAY(U8 *dest, bt_addr_struct *source);
extern void CONVERT_ARRAY2BDADDR(bt_addr_struct *dest, U8 *src);
extern void CONVERT_BDADDR2ARRAY(U8 *dest, U32 lap, U8 uap, U16 nap);

#ifdef __BT_MAP_MNS_SUPPORT__
static void maps_adp_send_mns_disconnect_ind(bt_map_addr_struct *bt_addr);
#endif

//map_adp_context g_map_adp_cntx = {0};
map_adp_context g_map_adp_cntx ;


/*****************************************************************************
 * FUNCTION
 *  map_send_msg
 * DESCRIPTION
 *  send message to external task.
 * PARAMETERS
 *  msg             [IN]        
 *  dstMod          [IN]        
 *  sap             [IN]        
 *  local_para      [IN]         
 *  peer_buff       [IN]         
 * RETURNS
 *  void
 *****************************************************************************/
void map_send_msg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{
#if 0
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct *ilmPtr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = msg;
    ilmPtr->local_para_ptr = local_para;
    ilmPtr->peer_buff_ptr = peer_buff;
    ilmPtr->dest_mod_id = dstMod;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = sap;
    msg_send_ext_queue(ilmPtr);
#endif


    if (local_para != NULL)
    {
        BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
    }
    else
    {
        BT_SendMessage(msg, dstMod, NULL, 0);
    }
}

bt_map_msg_enum map_util_translate_msg_type(map_supported_msg_enum from)
{
    bt_map_msg_enum to = 0;

    if (from & MAP_MSG_SMS_GSM)
    {
        to |= BT_MAP_MSG_SMS_GSM;
    }

    if (from & MAP_MSG_SMS_CDMA)
    {
        to |= BT_MAP_MSG_SMS_CDMA;
    }

    if (from & MAP_MSG_EMAIL)
    {
        to |= BT_MAP_MSG_EMAIL;
    }

    if (from & MAP_MSG_SMS_MMS)
    {
        to |= BT_MAP_MSG_MMS;
    }

    return to;
}

map_supported_msg_enum map_util_translate_msg_type_bt2map(bt_map_msg_enum from)
{
    map_supported_msg_enum to = 0;

    if (from & BT_MAP_MSG_SMS_GSM)
    {
        to |= MAP_MSG_SMS_GSM;
    }

    if (from & BT_MAP_MSG_SMS_CDMA)
    {
        to |= MAP_MSG_SMS_CDMA;
    }

    if (from & BT_MAP_MSG_EMAIL)
    {
        to |= MAP_MSG_EMAIL;
    }

    if (from & BT_MAP_MSG_MMS)
    {
        to |= MAP_MSG_SMS_MMS;
    }

    return to;
}

map_sdp_supported_msg_enum map_util_translate_msg_type_bt2sdp(bt_map_msg_enum from)
{
    map_sdp_supported_msg_enum to = 0;

    if (from & BT_MAP_MSG_SMS_GSM)
    {
        to |= MAP_SDP_MSG_SMS_GSM;
    }

    if (from & BT_MAP_MSG_SMS_CDMA)
    {
        to |= MAP_SDP_MSG_SMS_CDMA;
    }

    if (from & BT_MAP_MSG_EMAIL)
    {
        to |= MAP_SDP_MSG_EMAIL;
    }

    if (from & BT_MAP_MSG_MMS)
    {
        to |= MAP_SDP_MSG_SMS_MMS;
    }

    return to;
}


bt_map_msg_list_mask_enum map_util_translate_mask_type(map_msglist_parameter_enum from)
{
    return from;  /* cuz map_msglist_parameter_enum == bt_map_msg_list_mask_enum */
}

map_msglist_parameter_enum map_util_translate_mask_type_bt2map(bt_map_msg_list_mask_enum from)
{
    return from;  /* cuz map_msglist_parameter_enum == bt_map_msg_list_mask_enum */
}

bt_map_filter_status_enum map_util_translate_read_filter_type(map_filter_read_enum from)
{
    return from;  /* cuz map_filter_read_enum == bt_map_filter_status_enum */
}

map_filter_read_enum map_util_translate_read_filter_type_bt2map(bt_map_filter_status_enum from)
{
    return from;  /* cuz map_filter_read_enum == bt_map_filter_status_enum */
}

bt_map_filter_prio_enum map_util_translate_priority_filter_type(map_filter_priority_enum from)
{
    return from;  /* cuz map_filter_priority_enum == bt_map_filter_prio_enum */
}

map_filter_priority_enum map_util_translate_priority_filter_type_bt2map(bt_map_filter_prio_enum from)
{
    return from;  /* cuz map_filter_priority_enum == bt_map_filter_prio_enum */
}

bt_map_sent_op_enum map_util_translate_sent_op_type(BOOL transparent)
{
    return transparent;  /* cuz bt_map_sent_op_enum == BOOL */
}

bt_map_charset_enum map_util_translate_charset_type(map_charset_enum from)
{
    return from;  /* cuz bt_map_charset_enum == map_charset_enum */
}

map_charset_enum map_util_translate_charset_type_bt2map(bt_map_charset_enum from)
{
    return from;  /* cuz bt_map_charset_enum == map_charset_enum */
}

bt_map_fraction_req_enum map_util_translate_fraction_req_type(map_fraction_req_enum from)
{
    return from;  /* cuz bt_map_fraction_req_enum == map_fraction_req_enum */
}

map_fraction_req_enum map_util_translate_fraction_req_type_bt2map(bt_map_fraction_req_enum from)
{
    return from;  /* cuz bt_map_fraction_req_enum == map_fraction_req_enum */
}

map_fraction_deliv_enum map_util_translate_fraction_deliv_type(bt_map_fraction_rsp_enum from)
{
    return from;  /* cuz map_fraction_deliv_enum == bt_map_fraction_rsp_enum */
}

bt_map_fraction_rsp_enum map_util_translate_fraction_deliv_type_bt2map(map_fraction_deliv_enum from)
{
    return from;  /* cuz map_fraction_deliv_enum == bt_map_fraction_rsp_enum */
}

bt_map_result_enum map_util_translate_result(BtStatus from)
{
    switch (from)
    {
    case BT_STATUS_SUCCESS:
        return BT_MAP_SUCCESS;
    case BT_STATUS_BUSY:
        return BT_MAP_FAIL_BUSY;
    case BT_STATUS_TIMEOUT:
        return BT_MAP_FAIL_TIMEOUT;
    case BT_STATUS_NO_RESOURCES:
        return BT_MAP_FAIL_NO_RESOURCE;
    default:
        return BT_MAP_FAIL;
    }
}

ObexRespCode map_util_translate_map2obex_result(bt_map_result_enum from)
{
    switch (from)
    {
    case BT_MAP_SUCCESS:
        return OBRC_SUCCESS;
    case BT_MAP_FAIL_NOT_FOUND:
        return OBRC_NOT_FOUND;
    case BT_MAP_FAIL_FORBIDDEN:
        return OBRC_FORBIDDEN;
    case BT_MAP_FAIL_TIMEOUT:
        return OBRC_REQUEST_TIME_OUT;
    case BT_MAP_FAIL_NO_RESOURCE:
        return OBRC_SERVICE_UNAVAILABLE;
    case BT_MAP_FAIL_UNAUTHORIZED:
        return OBRC_UNAUTHORIZED;
    case BT_MAP_FAIL_INVALID_PARAMETER:
        return OBRC_PRECONDITION_FAILED;
    case BT_MAP_FAIL_STORAGE_FULL:
        return OBRC_DATABASE_FULL;
    case BT_MAP_FAIL_BAD_FORMAT:
        return OBRC_BAD_REQUEST;
    case BT_MAP_FAIL_NOT_SUPPORT:
        return OBRC_NOT_IMPLEMENTED;
    default:
        return OBRC_INTERNAL_SERVER_ERR;
    }
}

int map_util_compare_address(bt_map_addr_struct *a, bt_map_addr_struct *b)
{
    if ((a->lap == b->lap) && (a->nap == b->nap) && (a->uap == b->uap))
    {
        return 0;

    }
    return 1;

}

#ifdef __BT_MAPS_PROFILE__
void maps_adp_activate(ilm_struct *ilm_ptr)
{

    bt_maps_activate_cnf_struct *cnf = 
        (bt_maps_activate_cnf_struct *) construct_local_para(sizeof(bt_maps_activate_cnf_struct), TD_UL | TD_RESET);;

    if (g_map_adp_cntx.initialized)
    {
        cnf->result = BT_MAP_SUCCESS;
        g_map_adp_cntx.server_num = 0;
        OS_MemSet((U8 *)&g_map_adp_cntx.server, 0x0, sizeof(map_server_struct) * NUM_MAP_SRVS);
        OS_MemSet((U8 *)&g_map_adp_cntx.mns_conn, 0x0, sizeof(map_conn_struct) * NUM_MAP_SRV_CONN);
    }
    else
    {
        bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_ACTIVATE_NOT_INITIALIZED);
        cnf->result = BT_MAP_FAIL;
    }
    map_send_msg(MSG_ID_BT_MAPS_ACTIVATE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

void maps_adp_deactivate(ilm_struct *ilm_ptr)
{
    bt_maps_deactivate_cnf_struct *cnf = 
        (bt_maps_deactivate_cnf_struct *) construct_local_para(sizeof(bt_maps_deactivate_cnf_struct), TD_UL | TD_RESET);
    int i;

    for (i = 0; i < NUM_MAP_SRVS; i++)
    {
        if (g_map_adp_cntx.server[i].state >= MAP_SERVER_STATE_REGISTERED)
        {
            int j;
            for (j = 0; j < NUM_MAP_SRV_CONN; j++)
            {
                /* Disconnect */
                if (g_map_adp_cntx.server[i].conn[j].state == MAP_ADP_CONN_STATE_CONNECTED)
                {
                    maps_disconnect(g_map_adp_cntx.server[i].conn[j].id);
                    bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_DEACTIVATE_DISCONNECTING, 
                             g_map_adp_cntx.server[i].conn[j].addr.lap,
                             g_map_adp_cntx.server[i].conn[j].addr.uap,
                             g_map_adp_cntx.server[i].conn[j].addr.nap);
                }
            }
            maps_deregister_server(g_map_adp_cntx.server[i].instance);
        }
    }
    g_map_adp_cntx.server_num = 0;
    OS_MemSet((U8 *)&g_map_adp_cntx.server, 0x0, sizeof(map_server_struct) * NUM_MAP_SRVS);
    OS_MemSet((U8 *)&g_map_adp_cntx.mns_conn, 0x0, sizeof(map_conn_struct) * NUM_MAP_SRV_CONN);
    cnf->result = BT_MAP_SUCCESS;
    map_send_msg(MSG_ID_BT_MAPS_DEACTIVATE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

static map_server_struct *maps_adp_get_server(U8 mas_id)
{
    U8 i;

    for (i = 0; i < NUM_MAP_SRVS; i++)
    {
        if (g_map_adp_cntx.server[i].instance == mas_id)
        {
            return &g_map_adp_cntx.server[i];
        }
    }
    return NULL;
}

static BOOL maps_adp_add_connection(U8 mas_id, U8 conn_id, bt_map_addr_struct *addr)
{
    U8 i, j;

    bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_ADD_CONNECTION, mas_id, conn_id, addr->lap, addr->uap, addr->nap);
    for (i = 0; i < NUM_MAP_SRVS; i++)
    {
        map_server_struct *server = &g_map_adp_cntx.server[i];
        if (server->instance == mas_id )
        {
            map_conn_struct *new_entry = NULL;
            for (j = 0; j < NUM_MAP_SRV_CONN; j++)
            {
                if (server->conn[j].state == MAP_ADP_CONN_STATE_CONNECTED)
                {
                    if (server->conn[j].id == conn_id)
                    {
                        bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_ADD_CONNECTION_DUPLICATE);
                        return FALSE;
                    }
                }
                else if (!new_entry)
                {
                    new_entry = &server->conn[j];
                }
            }

            if (new_entry)
            {
                OS_MemSet((U8 *)new_entry, 0x0, sizeof(map_conn_struct));
                new_entry->state = MAP_ADP_CONN_STATE_CONNECTED;
                new_entry->id = conn_id;
                OS_MemCopy((U8 *)&new_entry->addr, (U8 *) addr, sizeof(bt_map_addr_struct));
            }
        }
    }
    return FALSE;
}

static map_conn_struct *maps_adp_get_conn_info(U8 conn_id)
{
    U8 i, j;

    for (i = 0; i < NUM_MAP_SRVS; i++)
    {
        for (j = 0; j < NUM_MAP_SRV_CONN; j++)
        {
            if (g_map_adp_cntx.server[i].conn[j].state > MAP_ADP_CONN_STATE_CONNECTING && g_map_adp_cntx.server[i].conn[j].id == conn_id)
            {
                return &g_map_adp_cntx.server[i].conn[j];
            }
        }
    }
    return NULL;
}

static map_conn_struct *maps_adp_get_conn_info_by_addr(U8 mas_id, bt_map_addr_struct *addr)
{
    U8 i;
    map_server_struct *server = maps_adp_get_server(mas_id);

    if (server)
    {
        for (i = 0; i < NUM_MAP_SRV_CONN; i++)
        {
            if (server->conn[i].state > MAP_ADP_CONN_STATE_CONNECTING && map_util_compare_address(&server->conn[i].addr, addr) == 0)
            {
                return &server->conn[i];
            }
        }
    }
    return NULL;
}

static map_conn_struct *maps_adp_get_mns_conn_info_by_addr(bt_map_addr_struct *addr)
{
    U8 i, num = NUM_MAP_SRV_CONN;

    for (i = 0; i < num; i++)
    {
        map_conn_struct *conn = &g_map_adp_cntx.mns_conn[i];
        if (conn->state != MAP_ADP_CONN_STATE_NULL && map_util_compare_address(&conn->addr, addr) == 0)
        {
            return conn;
        }
    }
    return NULL;
}

static void maps_adp_send_authorize_ind(bt_map_addr_struct *bt_addr, U8 *dev_name)
{
    bt_maps_authorize_ind_struct *ind = 
        (bt_maps_authorize_ind_struct *) construct_local_para(sizeof(bt_maps_authorize_ind_struct), TD_UL | TD_RESET);

    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    OS_MemCopy(ind->dev_name, dev_name, BT_MAP_MAX_DEV_NAME_LEN);
    map_send_msg(MSG_ID_BT_MAPS_AUTHORIZE_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void maps_adp_send_connect_ind(U8 conn_id, bt_map_addr_struct *bt_addr, U8 *dev_name, U8 mas_id)
{
    bt_maps_connect_ind_struct *ind = 
        (bt_maps_connect_ind_struct *) construct_local_para(sizeof(bt_maps_connect_ind_struct), TD_UL | TD_RESET);

    ind->conn_id = conn_id;
    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    OS_MemCopy(ind->dev_name, dev_name, BT_MAP_MAX_DEV_NAME_LEN);
    ind->mas_id = mas_id;
    map_send_msg(MSG_ID_BT_MAPS_CONNECT_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void maps_adp_send_disconnect_cnf(bt_map_result_enum result, U8 mas_id, bt_map_addr_struct *bt_addr)
{
    bt_maps_disconnect_cnf_struct *ind = 
        (bt_maps_disconnect_cnf_struct *) construct_local_para(sizeof(bt_maps_disconnect_cnf_struct), TD_UL | TD_RESET);

    ind->result = result;
    ind->mas_id = mas_id;
    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    map_send_msg(MSG_ID_BT_MAPS_DISCONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void maps_adp_send_disconnect_ind(U8 mas_id, bt_map_addr_struct *bt_addr)
{
    bt_maps_disconnect_ind_struct *ind = 
        (bt_maps_disconnect_ind_struct *) construct_local_para(sizeof(bt_maps_disconnect_ind_struct), TD_UL | TD_RESET);

    ind->mas_id = mas_id;
    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    map_send_msg(MSG_ID_BT_MAPS_DISCONNECT_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void maps_adp_send_set_folder_ind(U8 mas_id, bt_map_addr_struct *bt_addr, U8 *folder, bt_map_folder_op_enum flag)
{
    bt_maps_set_folder_ind_struct *ind = 
        (bt_maps_set_folder_ind_struct *) construct_local_para(sizeof(bt_maps_set_folder_ind_struct), TD_UL | TD_RESET);

    ind->mas_id = mas_id;
    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    OS_MemCopy(ind->folder, folder, BT_MAP_MAX_FOLDER_NAME_LEN);
    ind->flag = flag;
    map_send_msg(MSG_ID_BT_MAPS_SET_FOLDER_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void maps_adp_send_set_notif_registration_ind(U8 mas_id, bt_map_addr_struct *bt_addr, BOOL is_on)
{
    bt_maps_set_notif_registration_ind_struct *ind = 
        (bt_maps_set_notif_registration_ind_struct *) construct_local_para(sizeof(bt_maps_set_notif_registration_ind_struct), TD_UL | TD_RESET);

    ind->mas_id = mas_id;
    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    ind->on = is_on;
    map_send_msg(MSG_ID_BT_MAPS_SET_NOTIF_REGISTRATION_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void maps_adp_send_set_message_status_ind(U8 mas_id, bt_map_addr_struct *bt_addr, U8 *handle, bt_map_msg_status_enum status)
{
    bt_maps_set_message_status_ind_struct *ind = 
        (bt_maps_set_message_status_ind_struct *) construct_local_para(sizeof(bt_maps_set_message_status_ind_struct), TD_UL | TD_RESET);

    ind->mas_id = mas_id;
    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    OS_StrnCpy((char *)ind->handle, (char *)handle, BT_MAP_MAX_HANDLE_STR_LEN);
    ind->status = status;
    map_send_msg(MSG_ID_BT_MAPS_SET_MESSAGE_STATUS_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void maps_adp_send_get_folder_listing_ind(U8 mas_id, bt_map_addr_struct *bt_addr, U16 list_size, U16 list_offset)
{
    bt_maps_get_folder_listing_ind_struct *ind = 
        (bt_maps_get_folder_listing_ind_struct *) construct_local_para(sizeof(bt_maps_get_folder_listing_ind_struct), TD_UL | TD_RESET);

    ind->mas_id = mas_id;
    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    ind->list_size = list_size;
    ind->list_offset = list_offset;
    map_send_msg(MSG_ID_BT_MAPS_GET_FOLDER_LISTING_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void maps_adp_send_get_message_listing_ind(U8 mas_id, 
                                                  bt_map_addr_struct *bt_addr, 
                                                  U8 *folder,
                                                  U16 list_size, 
                                                  U16 list_offset,
                                                  U8 max_subject_len,
                                                  bt_map_msg_list_mask_enum mask,
                                                  bt_map_filter_msg_enum filter_msg,
                                                  U8 *filter_begin,
                                                  U8 *filter_end,
                                                  bt_map_filter_status_enum filter_status,
                                                  U8 *filter_rec,
                                                  U8 *filter_orig,
                                                  bt_map_filter_prio_enum filter_prio)
{
    bt_maps_get_message_listing_ind_struct *ind = 
        (bt_maps_get_message_listing_ind_struct *) construct_local_para(sizeof(bt_maps_get_message_listing_ind_struct), TD_UL | TD_RESET);

    ind->mas_id = mas_id;
    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    OS_StrnCpy((char *)ind->child_folder, (char *)folder, BT_MAP_MAX_FOLDER_NAME_LEN);
    ind->list_size = list_size;
    ind->list_offset = list_offset;
    ind->max_subject_len = max_subject_len;
    ind->mask = mask;
    ind->filter_msg = filter_msg;
    OS_StrnCpy((char *)ind->filter_begin, (char *)filter_begin, BT_MAP_MAX_DATE_STR_LEN);
    OS_StrnCpy((char *)ind->filter_end, (char *)filter_end, BT_MAP_MAX_DATE_STR_LEN);
    ind->filter_status = filter_status;
    OS_StrnCpy((char *)ind->filter_rec, (char *)filter_rec, BT_MAP_MAX_CONTACT_STR_LEN);
    OS_StrnCpy((char *)ind->filter_orig, (char *)filter_orig, BT_MAP_MAX_CONTACT_STR_LEN);
    ind->filter_prio = filter_prio;
    map_send_msg(MSG_ID_BT_MAPS_GET_MESSAGE_LISTING_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void maps_adp_send_get_message_ind(U8 mas_id, 
                                          bt_map_addr_struct *bt_addr,
                                          U8 *handle,
                                          kal_bool attachment,
                                          bt_map_charset_enum charset,
                                          bt_map_fraction_req_enum fraction_req)
{
    bt_maps_get_message_ind_struct *ind = 
        (bt_maps_get_message_ind_struct *) construct_local_para(sizeof(bt_maps_get_message_ind_struct), TD_UL | TD_RESET);

    ind->mas_id = mas_id;
    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    OS_StrnCpy((char *)ind->handle, (char *)handle, BT_MAP_MAX_HANDLE_STR_LEN);
    ind->attachment = attachment;
    ind->charset = charset;
    ind->fraction_req = fraction_req;
    map_send_msg(MSG_ID_BT_MAPS_GET_MESSAGE_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void maps_adp_send_push_message_ind(U8 mas_id, 
                                          bt_map_addr_struct *bt_addr,
                                          U8 *folder,
                                          bt_map_sent_op_enum sent_op,
                                          kal_bool retry,
                                          bt_map_charset_enum charset,
                                          U8 *file)
{
    bt_maps_push_message_ind_struct *ind = 
        (bt_maps_push_message_ind_struct *) construct_local_para(sizeof(bt_maps_push_message_ind_struct), TD_UL | TD_RESET);

    ind->mas_id = mas_id;
    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    OS_StrnCpy((char *)ind->child_folder, (char *)folder, BT_MAP_MAX_HANDLE_STR_LEN);
    ind->sent_op = sent_op;
    ind->retry = retry;
    ind->charset = charset;
    OS_StrnCpy((char *)ind->file, (char *)file, BT_MAP_MAX_TEMP_FILE_NAME_LEN);
    ind->data_size = btmtk_fs_get_filesize(ind->file);
    map_send_msg(MSG_ID_BT_MAPS_PUSH_MESSAGE_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void maps_adp_send_update_inbox_ind(U8 mas_id, bt_map_addr_struct *bt_addr)
{
    bt_maps_update_inbox_ind_struct *ind = 
        (bt_maps_update_inbox_ind_struct *) construct_local_para(sizeof(bt_maps_update_inbox_ind_struct), TD_UL | TD_RESET);

    ind->mas_id = mas_id;
    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    map_send_msg(MSG_ID_BT_MAPS_UPDATE_INBOX_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void maps_adp_send_mns_connect_cnf(bt_map_result_enum result, bt_map_addr_struct *bt_addr, U8 *dev_name)
{
    bt_maps_mns_connect_cnf_struct *cnf = 
        (bt_maps_mns_connect_cnf_struct *) construct_local_para(sizeof(bt_maps_mns_connect_cnf_struct), TD_UL | TD_RESET);
    cnf->result = result;
    OS_MemCopy((U8 *)&cnf->addr, (U8 *)bt_addr, sizeof(bt_map_addr_struct));
	if (dev_name != NULL)
	{
    OS_StrnCpy((char *)&cnf->dev_name, (char *)dev_name, BT_MAP_MAX_DEV_NAME_LEN);
	}
    map_send_msg(MSG_ID_BT_MAPS_MNS_CONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

static void maps_adp_send_mns_disconnect_cnf(bt_map_result_enum result, bt_map_addr_struct *bt_addr)
{
    bt_maps_mns_disconnect_cnf_struct *cnf = 
        (bt_maps_mns_disconnect_cnf_struct *) construct_local_para(sizeof(bt_maps_mns_disconnect_cnf_struct), TD_UL | TD_RESET);
    cnf->result = result;
    OS_MemCopy((U8 *)&cnf->addr, (U8 *)bt_addr, sizeof(bt_map_addr_struct));
    map_send_msg(MSG_ID_BT_MAPS_MNS_DISCONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

static void maps_adp_send_mns_event_cnf(bt_map_result_enum result, U8 mas_id, bt_map_addr_struct *bt_addr)
{
    bt_maps_mns_send_event_cnf_struct *cnf = 
        (bt_maps_mns_send_event_cnf_struct *) construct_local_para(sizeof(bt_maps_mns_send_event_cnf_struct), TD_UL | TD_RESET);
    cnf->result = result;
    cnf->mas_id = mas_id;
    OS_MemCopy((U8 *)&cnf->addr, (U8 *)bt_addr, sizeof(bt_map_addr_struct));
    map_send_msg(MSG_ID_BT_MAPS_MNS_SEND_EVENT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

void maps_app_callback(maps_callback_parm_struct *parm)
{
    map_obj_struct *push_pull_obj;
    char buf1[BT_MAP_MAX_FOLDER_NAME_LEN + 1] = {0}; 
    char buf2[BT_MAP_MAX_FOLDER_NAME_LEN + 1] = {0}; 
    bt_map_addr_struct bd_addr;

    CONVERT_ARRAY2BDADDR(&bd_addr, parm->remote_addr.addr);
    switch (parm->event)
    {
    case MAPS_EVENT_DATA_REQ:
        push_pull_obj = parm->u.push_pull_obj;
        if (push_pull_obj->fhandle < 0)
        {
            push_pull_obj->fhandle = btmtk_fs_open(push_pull_obj->file, BTMTK_FS_READ_ONLY);
        }

        if (push_pull_obj->fhandle >= 0)
        {
            push_pull_obj->buff_len = btmtk_fs_read(push_pull_obj->fhandle, push_pull_obj->buff, push_pull_obj->buff_len);
            bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_APP_CALLBACK_DATA_REQ, push_pull_obj->buff_len);
            if (push_pull_obj->offset + push_pull_obj->buff_len >= push_pull_obj->total_len)
            {
                btmtk_fs_close(push_pull_obj->fhandle);
                bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_APP_CALLBACK_DATA_REQ_DONE);
            }
        }
        else
        {
            push_pull_obj->buff_len = 0;
            bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_APP_CALLBACK_DATA_REQ_FAIL);
        }
        break;
    case MAPS_EVENT_DATA_IND:
        push_pull_obj = parm->u.push_pull_obj;
        if (push_pull_obj->fhandle < 0)
        {
            push_pull_obj->fhandle = btmtk_fs_open(push_pull_obj->file, BTMTK_FS_READ_WRITE | BTMTK_FS_APPEND | BTMTK_FS_CREATE);
        }

        if (push_pull_obj->fhandle >= 0)
        {
            push_pull_obj->buff_len = btmtk_fs_write(push_pull_obj->fhandle, push_pull_obj->buff, push_pull_obj->buff_len);
            bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_APP_CALLBACK_DATA_IND, push_pull_obj->buff_len);
		    if ((push_pull_obj->offset + push_pull_obj->buff_len >= push_pull_obj->total_len) && push_pull_obj->total_len > 0)
            {
                btmtk_fs_close(push_pull_obj->fhandle);
                bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_APP_CALLBACK_DATA_IND_DONE);
            }
        }
        else
        {
            push_pull_obj->buff_len = 0;
            bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_APP_CALLBACK_DATA_IND_FAIL);
        }
        break;
    case MAPS_EVENT_AUTHORIZE_IND:
        {
            U8 *dev_name;
            dev_name = BTBMGetLocalStoredName(parm->remote_addr.addr);
            maps_adp_send_authorize_ind(&bd_addr, dev_name);
        }
        break;
    case MAPS_EVENT_TP_DISCONNECTED:
        {
            map_conn_struct *conn = maps_adp_get_conn_info_by_addr(parm->mas_instance, &bd_addr);
            if (conn)
            {
                bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_APP_CALLBACK_TP_DISCONNECTED, conn->state);
                if (conn->state == MAP_ADP_CONN_STATE_DISCONNECTING)
                {
                    maps_adp_send_disconnect_cnf(BT_MAP_SUCCESS, parm->mas_instance, &bd_addr);
                }
                else if (conn->state != MAP_ADP_CONN_STATE_NULL)
                {
                    maps_adp_send_disconnect_ind(parm->mas_instance, &bd_addr);
                }
                conn->state = MAP_ADP_CONN_STATE_NULL;
            }
            else
            {
                bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_APP_CALLBACK_TP_DISCONNECTED_NO_CONN);
                maps_adp_send_disconnect_ind(parm->mas_instance, &bd_addr);
            }
        }
        break;
    case MAPS_EVENT_CONNECT:
        {
            U8 *dev_name;
            dev_name = BTBMGetLocalStoredName(parm->remote_addr.addr);
            maps_adp_send_connect_ind(parm->goep_conn_id, &bd_addr, dev_name, parm->mas_instance);
        }
        break;
    case MAPS_EVENT_SET_FOLDER:
        {
            bt_map_folder_op_enum flag;

            bt_ucs2swap_endian((U8 *)buf1, parm->u.set_folder.folder);
            bt_chset_ucs2_to_utf8_string((U8 *)buf2, BT_MAP_MAX_FOLDER_NAME_LEN + 1, (U8 *)buf1);
            switch(parm->u.set_folder.op)
            {
            case MAP_FOLDER_ROOT:
                flag = BT_MAP_FOLDER_OP_ROOT;
                break;
            case MAP_FOLDER_DOWN:
                flag = BT_MAP_FOLDER_OP_DOWN;
                break;
            case MAP_FOLDER_UP:
                if (parm->u.set_folder.folder && OS_StrLen(buf2))
                {
                    flag = BT_MAP_FOLDER_OP_NEXT;
                }
                else
                {
                    flag = BT_MAP_FOLDER_OP_UP;
                }
                break;
			default:
				{
					map_conn_struct *conn = maps_adp_get_conn_info_by_addr(parm->mas_instance, &bd_addr);
                	maps_set_folder_response(conn->id, OBRC_PRECONDITION_FAILED);
					return;
				}
            }
            maps_adp_send_set_folder_ind(parm->mas_instance, &bd_addr, (U8 *)buf2, flag);   
        }
        break;
    case MAPS_EVENT_GET_FOLDER_LIST:
        {
            map_conn_struct *conn = maps_adp_get_conn_info_by_addr(parm->mas_instance, &bd_addr);
            map_obj_struct *obj = parm->u.push_pull_obj;
            conn->push_pull_obj = obj;
            maps_adp_send_get_folder_listing_ind(parm->mas_instance, &bd_addr, obj->u.folder_list.max_list_count, obj->u.folder_list.list_start_offset);
        }
        break;
    case MAPS_EVENT_GET_MESSAGE_LIST:
        {
            map_conn_struct *conn = maps_adp_get_conn_info_by_addr(parm->mas_instance, &bd_addr);
            map_obj_struct *obj = parm->u.push_pull_obj;
            conn->push_pull_obj = obj;
            bt_ucs2swap_endian((U8 *)buf1, (U8 *)(obj->u.message_list.folder));
            bt_chset_ucs2_to_utf8_string((U8 *)buf2, BT_MAP_MAX_FOLDER_NAME_LEN + 1, (U8 *)buf1);
            maps_adp_send_get_message_listing_ind(parm->mas_instance, 
                                                  &bd_addr, 
                                                  (U8 *)buf2, 
                                                  obj->u.message_list.max_list_count,
                                                  obj->u.message_list.list_start_offset,
                                                  obj->u.message_list.subject_length,
                                                  map_util_translate_mask_type(obj->u.message_list.parameter_mask),
                                                  map_util_translate_msg_type(obj->u.message_list.filter_message_type),
                                                  obj->u.message_list.filter_period_begin,
                                                  obj->u.message_list.filter_period_end,
                                                  map_util_translate_read_filter_type(obj->u.message_list.filter_read_status),
                                                  obj->u.message_list.filter_recipient,
                                                  obj->u.message_list.filter_originator,
                                                  map_util_translate_priority_filter_type(obj->u.message_list.filter_priority));
        }
        break;
    case MAPS_EVENT_GET_MESSAGE:
        {
            map_conn_struct *conn = maps_adp_get_conn_info_by_addr(parm->mas_instance, &bd_addr);
            map_obj_struct *obj = parm->u.push_pull_obj;
            conn->push_pull_obj = obj;
            bt_ucs2swap_endian((U8 *)buf1, (U8 *)(obj->u.message.handle));
            bt_chset_ucs2_to_utf8_string((U8 *)buf2, BT_MAP_MAX_FOLDER_NAME_LEN + 1, (U8 *)buf1);
            maps_adp_send_get_message_ind(parm->mas_instance, 
                                          &bd_addr, 
                                          (U8 *)buf2, 
                                          obj->u.message.attachment,
                                          map_util_translate_charset_type(obj->u.message.charset),
                                          map_util_translate_fraction_req_type(obj->u.message.fraction_request));
        }
    case MAPS_EVENT_PUSH_MESSAGE_START:
        {
            map_conn_struct *conn = maps_adp_get_conn_info_by_addr(parm->mas_instance, &bd_addr);
            map_obj_struct *obj = parm->u.push_pull_obj;
            conn->push_pull_obj = obj;
            sprintf((char *)obj->file, BT_MAPS_MESSAGE_PUSH_FILE, conn->id);
        }
        break;
    case MAPS_EVENT_PUSH_MESSAGE_RECEIVED:
        {
            map_obj_struct *obj = parm->u.push_pull_obj;
	    /*MCE may be not including Length field in Obex header*/
	    if (obj->total_len == 0)
            {
                btmtk_fs_close(obj->fhandle);
	    }
            bt_ucs2swap_endian((U8 *)buf1, (U8 *)obj->u.put_message.folder);
            bt_chset_ucs2_to_utf8_string((U8 *)buf2, BT_MAP_MAX_FOLDER_NAME_LEN + 1, (U8 *)buf1);
            maps_adp_send_push_message_ind(parm->mas_instance, 
                                           &bd_addr, 
                                           (U8 *)buf2, 
                                           map_util_translate_sent_op_type(obj->u.put_message.transparent),
                                           obj->u.put_message.retry,
                                           map_util_translate_charset_type(obj->u.put_message.charset),
                                           obj->file);
        }
        break;
    case MAPS_EVENT_PUT_COMPLETE:
    case MAPS_EVENT_GET_COMPLETE:
        {
            map_conn_struct *conn = maps_adp_get_conn_info_by_addr(parm->mas_instance, &bd_addr);
            conn->push_pull_obj = NULL;
        }
        break;
    case MAPS_EVENT_SET_NOTIF:
        maps_adp_send_set_notif_registration_ind(parm->mas_instance, &bd_addr, parm->u.push_pull_obj->u.notif_registration.enable);
        break;
    case MAPS_EVENT_UPDATE_INBOX:
        maps_adp_send_update_inbox_ind(parm->mas_instance, &bd_addr);
        break;
    case MAPS_EVENT_MESSAGE_STATUS:
        {
            map_obj_struct *obj = parm->u.push_pull_obj;
            bt_map_msg_status_enum status;
            
            bt_ucs2swap_endian((U8 *)buf1, (U8 *)obj->u.msg_status.handle);
            bt_chset_ucs2_to_utf8_string((U8 *)buf2, BT_MAP_MAX_FOLDER_NAME_LEN + 1, (U8 *)buf1);
            if (obj->u.msg_status.type == MAP_MSG_STATUS_READ)
            {
                status = (obj->u.msg_status.enable) ? BT_MAP_MSG_STATUS_READ : BT_MAP_MSG_STATUS_UNREAD;
            }
            else if (obj->u.msg_status.type == MAP_MSG_STATUS_DELETE)
            {
                status = (obj->u.msg_status.enable) ? BT_MAP_MSG_STATUS_DELETE : BT_MAP_MSG_STATUS_UNDELETE;
            }
            else
            {
                map_conn_struct *conn = maps_adp_get_conn_info_by_addr(parm->mas_instance, &bd_addr);
                maps_set_message_status_response(conn->id, OBRC_PRECONDITION_FAILED);
				break;
            }
            maps_adp_send_set_message_status_ind(parm->mas_instance, &bd_addr, (U8 *)buf2, status);
        }
        break;
    case MAPS_EVENT_MNS_CONNECTED:
        {
            U8 *dev_name;
            map_conn_struct *conn = maps_adp_get_mns_conn_info_by_addr(&bd_addr);
            bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_APP_CALLBACK_MNS_CONNECTED, conn->state);
            if (conn->state == MAP_ADP_CONN_STATE_CONNECTING)
            {
                conn->state = MAP_ADP_CONN_STATE_CONNECTED;
                conn->id = parm->goep_conn_id;
                dev_name = BTBMGetLocalStoredName(parm->remote_addr.addr);
                maps_adp_send_mns_connect_cnf(BT_MAP_SUCCESS, &bd_addr, dev_name);
            }
        }
        break;
    case MAPS_EVENT_MNS_DISCONNECTED:
        {
            map_conn_struct *conn;

            conn = maps_adp_get_mns_conn_info_by_addr(&bd_addr);
            bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_APP_CALLBACK_MNS_DISCONNECTED, conn->state);
            if (conn->state == MAP_ADP_CONN_STATE_DISCONNECTING)
            {
                maps_adp_send_mns_disconnect_cnf(BT_MAP_SUCCESS, &bd_addr);
            }
            else if (conn->state != MAP_ADP_CONN_STATE_NULL)
            {
                maps_adp_send_mns_disconnect_ind(&bd_addr);
            }
            conn->state = MAP_ADP_CONN_STATE_NULL;
        }
        break;
    case MAPS_EVENT_MNS_PUT_COMPLETE:
        {
            map_obj_struct *obj = parm->u.push_pull_obj;
            maps_adp_send_mns_event_cnf(BT_MAP_SUCCESS, obj->u.event_report.mas_id, &bd_addr);
        }
        break;
    default:
        break;
    }
}

void maps_adp_register_server(ilm_struct *ilm_ptr)
{

    bt_maps_register_req_struct *msg = ilm_ptr->local_para_ptr;
    bt_maps_register_cnf_struct *cnf = 
        (bt_maps_register_cnf_struct *) construct_local_para(sizeof(bt_maps_register_cnf_struct), TD_UL | TD_RESET);
    map_registration_struct reg;
    BtStatus status;

    reg.mass.instance = msg->mas_id;
    reg.mass.supported_msg_type = map_util_translate_msg_type_bt2sdp(msg->msg_type);
    OS_StrnCpy((char *)reg.mass.service_name, (char *)msg->srv_name, MAP_MAX_SERVICE_NAME_LEN + 1);
    reg.mass.security = BSL_DEFAULT_ALL;
    reg.mass.callback = maps_app_callback;
    status = maps_register_server(&reg);
    if (status == OB_STATUS_SUCCESS)
    {
        int idx = 0;
        g_map_adp_cntx.server_num++;
#ifdef __BT_MAP_MNS_SUPPORT__
        if (g_map_adp_cntx.server_num == 1)
        {
            reg.mnsc.callback = maps_app_callback;
            maps_mns_register_client(&reg);
        }
#endif

        for (; idx < NUM_MAP_SRVS && g_map_adp_cntx.server[idx].state >= MAP_SERVER_STATE_REGISTERED; idx++);
		if (idx < NUM_MAP_SRVS) 
		{
        g_map_adp_cntx.server[idx].instance = msg->mas_id;
        g_map_adp_cntx.server[idx].state = MAP_SERVER_STATE_REGISTERED;
    }
		else 
		{
			status = OB_STATUS_INVALID_PARM;
		}
    }

    cnf->result = map_util_translate_result(status);
    cnf->mas_id = msg->mas_id;
    map_send_msg(MSG_ID_BT_MAPS_REGISTER_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

void maps_adp_deregister_server(ilm_struct *ilm_ptr)
{

    bt_maps_deregister_req_struct *msg = ilm_ptr->local_para_ptr;
    bt_maps_deregister_cnf_struct *cnf = 
        (bt_maps_deregister_cnf_struct *) construct_local_para(sizeof(bt_maps_deregister_cnf_struct), TD_UL | TD_RESET);
    BtStatus status;

    status = maps_deregister_server(msg->mas_id);
    if (status == OB_STATUS_SUCCESS)
    {
        int idx, j;
        g_map_adp_cntx.server_num--;
#ifdef __BT_MAP_MNS_SUPPORT__
        if (g_map_adp_cntx.server_num == 0)
        {
            status = maps_mns_deregister_client();
        }
#endif
        for (idx = 0; idx < NUM_MAP_SRVS && g_map_adp_cntx.server[idx].instance != msg->mas_id; idx++);
		if (idx < NUM_MAP_SRVS) 
		{
        /* Disconnect */
        for (j = 0; j < NUM_MAP_SRV_CONN; j++)
        {
            /* Disconnect */
            if (g_map_adp_cntx.server[idx].conn[j].state == MAP_ADP_CONN_STATE_CONNECTED)
            {
                maps_disconnect(g_map_adp_cntx.server[idx].conn[j].id);
            }
        }
        g_map_adp_cntx.server[idx].conn_num = 0;
        g_map_adp_cntx.server[idx].state = MAP_SERVER_STATE_NULL;
        g_map_adp_cntx.server[idx].instance = 0;
    }
		else
		{
			status = OB_STATUS_INVALID_PARM;
		}
    }

    cnf->result = map_util_translate_result(status);
    cnf->mas_id = msg->mas_id;
    map_send_msg(MSG_ID_BT_MAPS_DEREGISTER_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

void maps_adp_authorize_rsp(ilm_struct *ilm_ptr)
{
    bt_maps_authorize_rsp_struct *msg = ilm_ptr->local_para_ptr;
    BD_ADDR remote_addr;
    U8 rsp_code;

    rsp_code = (msg->accept) ? GOEP_STATUS_SUCCESS : GOEP_STATUS_FAILED;
    CONVERT_BDADDR2ARRAY(remote_addr.addr, msg->addr.lap, msg->addr.uap, msg->addr.nap);
    maps_authorize_rsp(&remote_addr, rsp_code);
}

void maps_adp_connect_rsp(ilm_struct *ilm_ptr)
{
    bt_maps_connect_rsp_struct *msg = ilm_ptr->local_para_ptr;
    BD_ADDR remote_addr;

    if (msg->accept)
    {
        maps_adp_add_connection(msg->mas_id, (U8) msg->conn_id, &msg->addr);
    }
    CONVERT_BDADDR2ARRAY(remote_addr.addr, msg->addr.lap, msg->addr.uap, msg->addr.nap);
    maps_connect_rsp((U8) msg->conn_id, &remote_addr, msg->accept);
}

void maps_adp_disconnect(ilm_struct *ilm_ptr)
{

    bt_maps_disconnect_req_struct *msg = ilm_ptr->local_para_ptr;
    bt_maps_disconnect_cnf_struct *cnf =
        (bt_maps_disconnect_cnf_struct *) construct_local_para(sizeof(bt_maps_disconnect_cnf_struct), TD_UL | TD_RESET);
    map_conn_struct *conn;
    BtStatus status;

    if ((conn = maps_adp_get_conn_info_by_addr(msg->mas_id, &msg->addr)) != NULL)
    {
        status = maps_disconnect(conn->id);
        bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_DISCONNECT, status);
        if (status == BT_STATUS_PENDING)
        {
            conn->state = MAP_ADP_CONN_STATE_DISCONNECTING;
            return;
        }
        else
        {
            conn->state = MAP_ADP_CONN_STATE_NULL;
            cnf->result = map_util_translate_result(status);
        }
        cnf->mas_id = msg->mas_id;
        OS_MemCopy((U8 *)&cnf->addr, (U8 *)&conn->addr, sizeof(bt_map_addr_struct));
    }
    else
    {
        cnf->result = BT_MAP_SUCCESS;
		OS_MemCopy((U8 *)&cnf->addr, (U8 *)&msg->addr, sizeof(bt_map_addr_struct));
    }
    map_send_msg(MSG_ID_BT_MAPS_DISCONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

void maps_adp_abort(ilm_struct *ilm_ptr)
{

    bt_maps_abort_req_struct *msg = ilm_ptr->local_para_ptr;
    bt_maps_abort_cnf_struct *cnf =
        (bt_maps_abort_cnf_struct *) construct_local_para(sizeof(bt_maps_abort_cnf_struct), TD_UL | TD_RESET);
    map_conn_struct *conn;

    if ((conn = maps_adp_get_conn_info_by_addr(msg->mas_id, &msg->addr)) != NULL)
    {
        maps_abort(conn->id, OBRC_FORBIDDEN);
    }

    cnf->result = BT_MAP_SUCCESS;
    cnf->mas_id = msg->mas_id;
    OS_MemCopy((U8 *)&cnf->addr, (U8 *)&msg->addr, sizeof(bt_map_addr_struct));
    map_send_msg(MSG_ID_BT_MAPS_ABORT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

void maps_adp_set_folder_response(ilm_struct *ilm_ptr)
{

    bt_maps_set_folder_rsp_struct *msg = ilm_ptr->local_para_ptr;
    map_conn_struct *conn;
    if ((conn = maps_adp_get_conn_info_by_addr(msg->mas_id, &msg->addr)) != NULL)
    {
        U8 rsp_code = map_util_translate_map2obex_result(msg->result);
        maps_set_folder_response(conn->id, rsp_code);
    }
}

void maps_adp_get_folder_listing_response(ilm_struct *ilm_ptr)
{

    bt_maps_get_folder_listing_rsp_struct *msg = ilm_ptr->local_para_ptr;
    map_conn_struct *conn;

    if ((conn = maps_adp_get_conn_info_by_addr(msg->mas_id, &msg->addr)) != NULL)
    {
        U8 rsp_code = map_util_translate_map2obex_result(msg->result);
        if (rsp_code != OBRC_SUCCESS)
        {
            msg->list_size = 0;
            msg->data_size = 0;
        }
        conn->push_pull_obj->total_len = msg->data_size;
        OS_StrnCpy((char *)conn->push_pull_obj->file, (char *)msg->file, MAP_MAX_TEMP_FILE_NAME_LEN);
        conn->push_pull_obj->u.folder_list.list_count = msg->list_size;
        maps_get_folder_listing_response(conn->id, rsp_code, conn->push_pull_obj);
    }
}

void maps_adp_get_message_listing_response(ilm_struct *ilm_ptr)
{
    bt_maps_get_message_listing_rsp_struct *msg = ilm_ptr->local_para_ptr;
    map_conn_struct *conn;
    
    if ((conn = maps_adp_get_conn_info_by_addr(msg->mas_id, &msg->addr)) != NULL)
    {
        U8 rsp_code = map_util_translate_map2obex_result(msg->result);
        if (rsp_code != OBRC_SUCCESS)
        {
            msg->list_size = 0;
            msg->data_size = 0;
        }
//        conn->push_pull_obj->offset = 0;
//        conn->push_pull_obj->fhandle = -1;
        conn->push_pull_obj->total_len = msg->data_size;
        OS_StrnCpy((char *)conn->push_pull_obj->file, (char *)msg->file, MAP_MAX_TEMP_FILE_NAME_LEN);
        OS_StrnCpy((char *)conn->push_pull_obj->u.message_list.mse_time, (char *)msg->datetime, MAP_MAX_DATETIME_LEN);
        conn->push_pull_obj->u.message_list.list_count = msg->list_size;
        conn->push_pull_obj->u.message_list.new_message = msg->unread;
        maps_get_message_listing_response(conn->id, rsp_code, conn->push_pull_obj);
    }
}

void maps_adp_get_message_response(ilm_struct *ilm_ptr)
{
    bt_maps_get_message_rsp_struct *msg = ilm_ptr->local_para_ptr;
    map_conn_struct *conn;

    if ((conn = maps_adp_get_conn_info_by_addr(msg->mas_id, &msg->addr)) != NULL)
    {
        U8 rsp_code = map_util_translate_map2obex_result(msg->result);
        if (rsp_code != OBRC_SUCCESS)
        {
            msg->data_size = 0;
        }

        conn->push_pull_obj->total_len = msg->data_size;
        OS_StrnCpy((char *)conn->push_pull_obj->file, (char *)msg->file, MAP_MAX_TEMP_FILE_NAME_LEN);
        conn->push_pull_obj->u.message.fraction_deliver = map_util_translate_fraction_deliv_type(msg->fraction_rsp);
        if (conn->push_pull_obj->u.message.fraction_request != MAP_FRACTION_REQ_NO &&
            conn->push_pull_obj->u.message.fraction_deliver == MAP_FRACTION_DELIV_NO)
        {
            conn->push_pull_obj->u.message.fraction_deliver = MAP_FRACTION_DELIV_LAST;
        }
        maps_get_message_response(conn->id, rsp_code, conn->push_pull_obj);
    }
}

void maps_adp_set_notification_registration_response(ilm_struct *ilm_ptr)
{
    bt_maps_set_notif_registration_rsp_struct *msg = ilm_ptr->local_para_ptr;
    map_conn_struct *conn;

    if ((conn = maps_adp_get_conn_info_by_addr(msg->mas_id, &msg->addr)) != NULL)
    {
        U8 rsp_code = map_util_translate_map2obex_result(msg->result);
        maps_set_notification_registration_response(conn->id, rsp_code);
    }

}

void maps_adp_set_message_status_response(ilm_struct *ilm_ptr)
{
    bt_maps_set_message_status_rsp_struct *msg = ilm_ptr->local_para_ptr;
    map_conn_struct *conn;
    if ((conn = maps_adp_get_conn_info_by_addr(msg->mas_id, &msg->addr)) != NULL)
    {
        U8 rsp_code = map_util_translate_map2obex_result(msg->result);
        maps_set_message_status_response(conn->id, rsp_code);
    }
}

void maps_adp_push_message_response(ilm_struct *ilm_ptr)
{
    bt_maps_push_message_rsp_struct *msg = ilm_ptr->local_para_ptr;
    map_conn_struct *conn;

    if ((conn = maps_adp_get_conn_info_by_addr(msg->mas_id, &msg->addr)) != NULL)
    {
        U8 buf1[MAP_MAX_HANDLE_STR_LEN + 2] = {0};
        U8 buf2[MAP_MAX_HANDLE_STR_LEN + 2] = {0};
        U8 rsp_code = map_util_translate_map2obex_result(msg->result);

        if (rsp_code == OBRC_SUCCESS)
        {
            bt_chset_utf8_to_ucs2_string(buf1, MAP_MAX_HANDLE_STR_LEN + 2, msg->handle);
            bt_ucs2swap_endian(buf2, buf1);
        }
        maps_push_message_response(conn->id, rsp_code, (U16 *)buf2, (U16)(bt_ucs2strlen((char *)buf2) * 2));
    }
}

void maps_adp_update_inbox_response(ilm_struct *ilm_ptr)
{
    bt_maps_update_inbox_rsp_struct *msg = ilm_ptr->local_para_ptr;
    map_conn_struct *conn;

    if ((conn = maps_adp_get_conn_info_by_addr(msg->mas_id, &msg->addr)) != NULL)
    {
        U8 rsp_code = map_util_translate_map2obex_result(msg->result);
        maps_update_inbox_response(conn->id, rsp_code);
    }
}

#ifdef __BT_MAP_MNS_SUPPORT__
static void maps_adp_send_mns_disconnect_ind(bt_map_addr_struct *bt_addr)
{
    bt_maps_mns_disconnect_ind_struct *ind = 
        (bt_maps_mns_disconnect_ind_struct *) construct_local_para(sizeof(bt_maps_mns_disconnect_ind_struct), TD_UL | TD_RESET);

    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    map_send_msg(MSG_ID_BT_MAPS_MNS_DISCONNECT_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

void maps_adp_mns_connect(ilm_struct *ilm_ptr)
{
    bt_maps_mns_connect_req_struct *msg = ilm_ptr->local_para_ptr;
    BD_ADDR remote_addr;
    BtStatus status;
    map_conn_struct *conn;
    U8 *dev_name;

    if ((conn = maps_adp_get_mns_conn_info_by_addr(&msg->addr)) != NULL)
    {
        bt_trace(TRACE_GROUP_9, BTLOG_MAPS_ADP_MNS_CONNECT_DUPLICATE, conn->state);
        if (conn->state == MAP_ADP_CONN_STATE_CONNECTED)
        {
            maps_adp_send_mns_connect_cnf(BT_MAP_SUCCESS, &msg->addr, NULL);
        }
        else if (conn->state == MAP_ADP_CONN_STATE_DISCONNECTING)
        {
            maps_adp_send_mns_connect_cnf(BT_MAP_FAIL_BUSY, &msg->addr, NULL);
        }
        return;
    }
    else
    {
        int i;
        for (i = 0; i < NUM_MAP_SRV_CONN; i++)
        {
            if (g_map_adp_cntx.mns_conn[i].state == MAP_ADP_CONN_STATE_NULL)
            {
                conn = &g_map_adp_cntx.mns_conn[i];
                OS_MemCopy((U8 *)&conn->addr, (U8 *) &msg->addr, sizeof(bt_map_addr_struct));
				break;
            }
        }
        Assert(i < NUM_MAP_SRV_CONN);
    }

    conn->state = MAP_ADP_CONN_STATE_CONNECTING;
    CONVERT_BDADDR2ARRAY(remote_addr.addr, msg->addr.lap, msg->addr.uap, msg->addr.nap);
    status = maps_mns_connect(&remote_addr, AUTH_OPT_DEFAULT, NULL, NULL);

    if (status != BT_STATUS_PENDING)
    {
        if (status == BT_STATUS_SUCCESS)
        {
            conn->state = MAP_ADP_CONN_STATE_CONNECTED;
        }
        else
        {
            conn->state = MAP_ADP_CONN_STATE_NULL;
        }
        dev_name = BTBMGetLocalStoredName(remote_addr.addr);
        maps_adp_send_mns_connect_cnf(map_util_translate_result(status), &msg->addr, dev_name);
    }
}

void maps_adp_mns_disconnect(ilm_struct *ilm_ptr)
{
    bt_maps_mns_disconnect_req_struct *msg = ilm_ptr->local_para_ptr;
    map_conn_struct *conn;
    BtStatus status;

    if ((conn = maps_adp_get_mns_conn_info_by_addr(&msg->addr)) != NULL && conn->state != MAP_ADP_CONN_STATE_NULL)
    {
        if (conn->state == MAP_ADP_CONN_STATE_CONNECTED || conn->state == MAP_ADP_CONN_STATE_CONNECTING)
        {
            conn->state = MAP_ADP_CONN_STATE_DISCONNECTING;
            status = maps_mns_disconnect(conn->id);
            
            if (status != BT_STATUS_PENDING)
            {
                conn->state = MAP_ADP_CONN_STATE_NULL;
                maps_adp_send_mns_disconnect_cnf(map_util_translate_result(status), &msg->addr);
            }
        }
    }
    else
    {
        maps_adp_send_mns_disconnect_cnf(BT_MAP_SUCCESS, &msg->addr);
    }
}

void maps_adp_mns_send_event(ilm_struct *ilm_ptr)
{
    bt_maps_mns_send_event_req_struct *msg = ilm_ptr->local_para_ptr;
    map_conn_struct *conn;

    if ((conn = maps_adp_get_mns_conn_info_by_addr(&msg->addr)) != NULL)
    {
//        g_map_adp_cntx.mns_event.op = MAP_MNS_PUSH_EVENT;
        conn->push_pull_obj = maps_obs_new(conn->id);  /* Use MNS conn ID */
//        conn->push_pull_obj->fhandle = -1;
        OS_StrnCpy((char *)conn->push_pull_obj->file, (char *)msg->file, MAP_MAX_TEMP_FILE_NAME_LEN);
        conn->push_pull_obj->type = MAP_OBJ_MNS_EVENT;
        conn->push_pull_obj->u.event_report.mas_id = msg->mas_id;
        conn->push_pull_obj->total_len = msg->data_size;
        maps_mns_send_event(conn->id, conn->push_pull_obj);
    }
}
#endif /* __BT_MAP_MNS_SUPPORT__ */

#endif /* __BT_MAPS_PROFILE__ */

#ifdef __BT_MAPC_PROFILE__
static void mapc_adp_send_connect_cnf(bt_map_result_enum result, bt_map_addr_struct *bt_addr, U8 *dev_name, U8 mas_id)
{
    bt_mapc_connect_cnf_struct *cnf = 
        (bt_mapc_connect_cnf_struct *) construct_local_para(sizeof(bt_mapc_connect_cnf_struct), TD_UL | TD_RESET);

    cnf->result = result;
    cnf->addr.lap = bt_addr->lap;
    cnf->addr.uap = bt_addr->uap;
    cnf->addr.nap = bt_addr->nap;
    OS_MemCopy(cnf->dev_name, dev_name, BT_MAP_MAX_DEV_NAME_LEN);
    cnf->mas_id = mas_id;
    map_send_msg(MSG_ID_BT_MAPC_CONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);   
}

static void mapc_adp_send_disconnect_cnf(bt_map_result_enum result, bt_map_addr_struct *bt_addr, U8 mas_id)
{
    bt_mapc_disconnect_cnf_struct *cnf = 
        (bt_mapc_disconnect_cnf_struct *) construct_local_para(sizeof(bt_mapc_disconnect_cnf_struct), TD_UL | TD_RESET);

    cnf->result = result;
    cnf->addr.lap = bt_addr->lap;
    cnf->addr.uap = bt_addr->uap;
    cnf->addr.nap = bt_addr->nap;
    cnf->mas_id = mas_id;
    map_send_msg(MSG_ID_BT_MAPC_DISCONNECT_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);   
}

static void mapc_adp_send_disconnect_ind(bt_map_addr_struct *bt_addr, U8 mas_id)
{
    bt_mapc_disconnect_ind_struct *ind = 
        (bt_mapc_disconnect_ind_struct *) construct_local_para(sizeof(bt_mapc_disconnect_ind_struct), TD_UL | TD_RESET);

    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    ind->mas_id = mas_id;
    map_send_msg(MSG_ID_BT_MAPC_DISCONNECT_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void mapc_adp_send_set_folder_cnf(bt_map_result_enum result, bt_map_addr_struct *bt_addr, U8 mas_id)
{
    bt_mapc_set_folder_cnf_struct *cnf = 
        (bt_mapc_set_folder_cnf_struct *) construct_local_para(sizeof(bt_mapc_set_folder_cnf_struct), TD_UL | TD_RESET);

    cnf->result = result;
    cnf->addr.lap = bt_addr->lap;
    cnf->addr.uap = bt_addr->uap;
    cnf->addr.nap = bt_addr->nap;
    cnf->mas_id = mas_id;
    map_send_msg(MSG_ID_BT_MAPC_SET_FOLDER_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);   
}

static void mapc_adp_send_get_folder_list_cnf(bt_map_result_enum result, 
                                              bt_map_addr_struct *bt_addr, 
                                              U8 mas_id, 
                                              U16 list_size, 
                                              U16 data_size,
                                              U8 *file)
{
    bt_mapc_get_folder_listing_cnf_struct *cnf = 
        (bt_mapc_get_folder_listing_cnf_struct *) construct_local_para(sizeof(bt_mapc_get_folder_listing_cnf_struct), TD_UL | TD_RESET);

    cnf->result = result;
    cnf->addr.lap = bt_addr->lap;
    cnf->addr.uap = bt_addr->uap;
    cnf->addr.nap = bt_addr->nap;
    cnf->mas_id = mas_id;
    cnf->list_size = list_size;
    cnf->data_size = data_size;
    if (file)
    {
        OS_StrnCpy(cnf->file, file, BT_MAP_MAX_TEMP_FILE_NAME_LEN);
    }
    map_send_msg(MSG_ID_BT_MAPC_GET_FOLDER_LISTING_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);   
}

static void mapc_adp_send_get_folder_list_size_cnf(bt_map_result_enum result, bt_map_addr_struct *bt_addr, U8 mas_id, U16 list_size)
{
    bt_mapc_get_folder_listing_size_cnf_struct *cnf = 
        (bt_mapc_get_folder_listing_size_cnf_struct *) construct_local_para(sizeof(bt_mapc_get_folder_listing_size_cnf_struct), TD_UL | TD_RESET);

    cnf->result = result;
    cnf->addr.lap = bt_addr->lap;
    cnf->addr.uap = bt_addr->uap;
    cnf->addr.nap = bt_addr->nap;
    cnf->mas_id = mas_id;
    cnf->list_size = list_size;
    map_send_msg(MSG_ID_BT_MAPC_GET_FOLDER_LISTING_SIZE_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);   
}

static void mapc_adp_send_get_message_list_cnf(bt_map_result_enum result, 
                                               bt_map_addr_struct *bt_addr, 
                                               U8 mas_id, 
                                               U16 list_size, 
                                               U16 data_size,
                                               U8 *file,
                                               kal_bool unread,
                                               U8 *datetime)
{
    bt_mapc_get_message_listing_cnf_struct *cnf = 
        (bt_mapc_get_message_listing_cnf_struct *) construct_local_para(sizeof(bt_mapc_get_message_listing_cnf_struct), TD_UL | TD_RESET);

    cnf->result = result;
    cnf->addr.lap = bt_addr->lap;
    cnf->addr.uap = bt_addr->uap;
    cnf->addr.nap = bt_addr->nap;
    cnf->mas_id = mas_id;
    cnf->list_size = list_size;
    cnf->data_size = data_size;
    if (file)
    {
        OS_StrnCpy(cnf->file, file, BT_MAP_MAX_TEMP_FILE_NAME_LEN);
    }
    cnf->unread = unread;
    if (datetime)
    {
        OS_StrnCpy(cnf->datetime, datetime, BT_MAP_MAX_DATETIME_STR_LEN);
    }
    map_send_msg(MSG_ID_BT_MAPC_GET_MESSAGE_LISTING_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);   
}

static void mapc_adp_send_get_message_list_size_cnf(bt_map_result_enum result, 
                                                    bt_map_addr_struct *bt_addr, 
                                                    U8 mas_id, 
                                                    U16 list_size, 
                                                    kal_bool unread,
                                                    U8 *datetime)
{
    bt_mapc_get_message_listing_size_cnf_struct *cnf = 
        (bt_mapc_get_message_listing_size_cnf_struct *) construct_local_para(sizeof(bt_mapc_get_message_listing_size_cnf_struct), TD_UL | TD_RESET);

    cnf->result = result;
    cnf->addr.lap = bt_addr->lap;
    cnf->addr.uap = bt_addr->uap;
    cnf->addr.nap = bt_addr->nap;
    cnf->mas_id = mas_id;
    cnf->list_size = list_size;
    cnf->unread = unread;
    OS_StrnCpy(cnf->datetime, datetime, BT_MAP_MAX_DATETIME_STR_LEN);
    map_send_msg(MSG_ID_BT_MAPC_GET_MESSAGE_LISTING_SIZE_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);   
}

static void mapc_adp_send_get_message_cnf(bt_map_result_enum result, 
                                               bt_map_addr_struct *bt_addr, 
                                               U8 mas_id, 
                                               bt_map_fraction_rsp_enum fraction_rsp, 
                                               U16 data_size,
                                               U8 *file)
{
    bt_mapc_get_message_cnf_struct *cnf = 
        (bt_mapc_get_message_cnf_struct *) construct_local_para(sizeof(bt_mapc_get_message_cnf_struct), TD_UL | TD_RESET);

    cnf->result = result;
    cnf->addr.lap = bt_addr->lap;
    cnf->addr.uap = bt_addr->uap;
    cnf->addr.nap = bt_addr->nap;
    cnf->mas_id = mas_id;
    cnf->fraction_rsp = fraction_rsp;
    cnf->data_size = data_size;
    if (file)
    {
        OS_StrnCpy(cnf->file, file, BT_MAP_MAX_TEMP_FILE_NAME_LEN);
    }
    map_send_msg(MSG_ID_BT_MAPC_GET_MESSAGE_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);   
}

static void mapc_adp_send_push_message_cnf(bt_map_result_enum result, 
                                               bt_map_addr_struct *bt_addr, 
                                               U8 mas_id, 
                                               U8 *handle)
{
    bt_mapc_push_message_cnf_struct *cnf = 
        (bt_mapc_push_message_cnf_struct *) construct_local_para(sizeof(bt_mapc_push_message_cnf_struct), TD_UL | TD_RESET);

    cnf->result = result;
    cnf->addr.lap = bt_addr->lap;
    cnf->addr.uap = bt_addr->uap;
    cnf->addr.nap = bt_addr->nap;
    cnf->mas_id = mas_id;
    if (handle)
    {
        OS_StrnCpy(cnf->handle, handle, BT_MAP_MAX_HANDLE_STR_LEN);
    }
    map_send_msg(MSG_ID_BT_MAPC_PUSH_MESSAGE_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);   
}

static void mapc_adp_send_set_message_status_cnf(bt_map_result_enum result, bt_map_addr_struct *bt_addr, U8 mas_id)
{
    bt_mapc_set_message_status_cnf_struct *cnf = 
        (bt_mapc_set_message_status_cnf_struct *) construct_local_para(sizeof(bt_mapc_set_message_status_cnf_struct), TD_UL | TD_RESET);

    cnf->result = result;
    cnf->addr.lap = bt_addr->lap;
    cnf->addr.uap = bt_addr->uap;
    cnf->addr.nap = bt_addr->nap;
    cnf->mas_id = mas_id;
    map_send_msg(MSG_ID_BT_MAPC_SET_MESSAGE_STATUS_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);   
}

static void mapc_adp_send_inbox_update_cnf(bt_map_result_enum result, bt_map_addr_struct *bt_addr, U8 mas_id)
{
    bt_mapc_update_inbox_cnf_struct *cnf = 
        (bt_mapc_update_inbox_cnf_struct *) construct_local_para(sizeof(bt_mapc_update_inbox_cnf_struct), TD_UL | TD_RESET);

    cnf->result = result;
    cnf->addr.lap = bt_addr->lap;
    cnf->addr.uap = bt_addr->uap;
    cnf->addr.nap = bt_addr->nap;
    cnf->mas_id = mas_id;
    map_send_msg(MSG_ID_BT_MAPC_UPDATE_INBOX_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct *)cnf, NULL);   
}

static void mapc_adp_send_mns_authorize_ind(bt_map_addr_struct *bt_addr, U8 *dev_name)
{
    bt_mapc_mns_authorize_ind_struct *ind = 
        (bt_mapc_mns_authorize_ind_struct *) construct_local_para(sizeof(bt_mapc_mns_authorize_ind_struct), TD_UL | TD_RESET);

    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    OS_MemCopy(ind->dev_name, dev_name, BT_MAP_MAX_DEV_NAME_LEN);
    map_send_msg(MSG_ID_BT_MAPC_MNS_AUTHORIZE_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

static void mapc_adp_send_mns_event_ind(bt_map_addr_struct *bt_addr, U8 mas_id, U16 data_size, U8 *file)
{
    bt_mapc_mns_send_event_ind_struct *ind = 
        (bt_mapc_mns_send_event_ind_struct *) construct_local_para(sizeof(bt_mapc_mns_send_event_ind_struct), TD_UL | TD_RESET);

    ind->addr.lap = bt_addr->lap;
    ind->addr.uap = bt_addr->uap;
    ind->addr.nap = bt_addr->nap;
    ind->mas_id = mas_id;
    ind->data_size = data_size;
    if (file)
    {
        OS_StrnCpy(ind->file, file, BT_MAP_MAX_TEMP_FILE_NAME_LEN);
    }
    map_send_msg(MSG_ID_BT_MAPC_MNS_SEND_EVENT_IND, MOD_MMI, BT_APP_SAP, (local_para_struct *)ind, NULL);   
}

void mapc_app_callback(mapc_callback_parm_struct *parm)
{
    char buf1[BT_MAP_MAX_FOLDER_NAME_LEN + 1] = {0}; 
    char buf2[BT_MAP_MAX_FOLDER_NAME_LEN + 1] = {0}; 
    bt_map_addr_struct bd_addr;
    map_obj_struct *push_pull_obj;

    CONVERT_ARRAY2BDADDR(&bd_addr, parm->remote_addr.addr);
    switch (parm->event)
    {
    case MAPC_EVENT_DATA_IND:
        push_pull_obj = parm->u.push_pull_obj;
        if (push_pull_obj->fhandle < 0)
        {
            push_pull_obj->fhandle = btmtk_fs_open(push_pull_obj->file, BTMTK_FS_READ_WRITE | BTMTK_FS_APPEND | BTMTK_FS_CREATE);
        }

        if (push_pull_obj->fhandle >= 0)
        {
            push_pull_obj->buff_len = btmtk_fs_write(push_pull_obj->fhandle, push_pull_obj->buff, push_pull_obj->buff_len);
        }
        else
        {
            push_pull_obj->buff_len = 0;
        }
        break;
    case MAPC_EVENT_DATA_REQ:
        switch (parm->u.push_pull_obj->type)
        {
        case MAP_OBJ_NOTIF_REGISTRATION:
        case MAP_OBJ_MESSAGE_STATUS:
            *parm->u.push_pull_obj->buff = 0x30;
            break;
        case MAP_OBJ_MESSAGE:
            push_pull_obj = parm->u.push_pull_obj;
            if (push_pull_obj->fhandle < 0)
            {
                push_pull_obj->fhandle = btmtk_fs_open(push_pull_obj->file, BTMTK_FS_READ_ONLY);
            }
    
            if (push_pull_obj->fhandle >= 0)
            {
                push_pull_obj->buff_len = btmtk_fs_read(push_pull_obj->fhandle, push_pull_obj->buff, push_pull_obj->buff_len);
            }
            else
            {
                push_pull_obj->buff_len = 0;
            }
            break;
        }
        break;
    case MAPC_EVENT_SET_FOLDER_COMPLETE:
        mapc_adp_send_set_folder_cnf(BT_MAP_SUCCESS, &bd_addr, g_map_adp_cntx.client.instance);
        break;
    case MAPC_EVENT_PUT_COMPLETE:
        if (parm->u.push_pull_obj->fhandle >= 0)
        {
            btmtk_fs_close(parm->u.push_pull_obj->fhandle);
        }
        switch (parm->u.push_pull_obj->type)
        {
        case MAP_OBJ_MESSAGE_STATUS:
            {
                mapc_adp_send_set_message_status_cnf(BT_MAP_SUCCESS, &bd_addr, g_map_adp_cntx.client.instance);
            }
            break;
        case MAP_OBJ_INBOX_UPDATE:
            {
                mapc_adp_send_inbox_update_cnf(BT_MAP_SUCCESS, &bd_addr, g_map_adp_cntx.client.instance);
            }
            break;
        case MAP_OBJ_MESSAGE:
            {
                bt_ucs2swap_endian(buf1, parm->u.push_pull_obj->u.put_message.handle);
                bt_chset_ucs2_to_utf8_string(buf2, BT_MAP_MAX_FOLDER_NAME_LEN + 1, buf1);
                mapc_adp_send_push_message_cnf(BT_MAP_SUCCESS, 
                                              &bd_addr, 
                                              g_map_adp_cntx.client.instance,
                                              buf2);
            }
            break;
        }
        break;
    case MAPC_EVENT_GET_COMPLETE:
        if (parm->u.push_pull_obj->fhandle >= 0)
        {
            btmtk_fs_close(parm->u.push_pull_obj->fhandle);
        }
        switch (parm->u.push_pull_obj->type)
        {
        case MAP_OBJ_FOLDER_LIST:
            {
                mapc_adp_send_get_folder_list_cnf(BT_MAP_SUCCESS, 
                                                  &bd_addr, 
                                                  g_map_adp_cntx.client.instance,
                                                  parm->u.push_pull_obj->u.folder_list.list_count,
                                                  (U16) parm->u.push_pull_obj->total_len,
                                                  parm->u.push_pull_obj->file);
            }
            break;
        case MAP_OBJ_FOLDER_LIST_SIZE:
            {
                mapc_adp_send_get_folder_list_size_cnf(BT_MAP_SUCCESS, 
                                                  &bd_addr, 
                                                  g_map_adp_cntx.client.instance,
                                                  parm->u.push_pull_obj->u.folder_list.list_count);
            }
            break;
        case MAP_OBJ_MESSAGE_LIST:
            {
                mapc_adp_send_get_message_list_cnf(BT_MAP_SUCCESS, 
                                                  &bd_addr, 
                                                  g_map_adp_cntx.client.instance,
                                                  parm->u.push_pull_obj->u.message_list.list_count,
                                                  (U16) parm->u.push_pull_obj->total_len,
                                                  parm->u.push_pull_obj->file,
                                                  parm->u.push_pull_obj->u.message_list.new_message,
                                                  parm->u.push_pull_obj->u.message_list.mse_time);
            }
            break;
        case MAP_OBJ_MESSAGE_LIST_SIZE:
            {
                mapc_adp_send_get_message_list_size_cnf(BT_MAP_SUCCESS, 
                                                  &bd_addr, 
                                                  g_map_adp_cntx.client.instance,
                                                  parm->u.push_pull_obj->u.message_list.list_count,
                                                  parm->u.push_pull_obj->u.message_list.new_message,
                                                  parm->u.push_pull_obj->u.message_list.mse_time);
            }
            break;
        case MAP_OBJ_MESSAGE:
            {
                mapc_adp_send_get_message_cnf(BT_MAP_SUCCESS, 
                                              &bd_addr, 
                                              g_map_adp_cntx.client.instance,
                                              map_util_translate_fraction_deliv_type_bt2map(parm->u.push_pull_obj->u.message.fraction_deliver),
                                              (U16) parm->u.push_pull_obj->total_len,
                                              parm->u.push_pull_obj->file);
            }
            break;
        }
        break;
    case MAPC_EVENT_PUT_ABORTED:
        if (parm->u.push_pull_obj->fhandle >= 0)
        {
            btmtk_fs_close(parm->u.push_pull_obj->fhandle);
        }
        switch (parm->u.push_pull_obj->type)
        {
        case MAP_OBJ_MESSAGE:
            mapc_adp_send_push_message_cnf(BT_MAP_FAIL, &bd_addr, g_map_adp_cntx.client.instance, NULL);
            break;
        }
        break;
    case MAPC_EVENT_GET_ABORTED:
        if (parm->u.push_pull_obj->fhandle >= 0)
        {
            btmtk_fs_close(parm->u.push_pull_obj->fhandle);
        }
        switch (parm->u.push_pull_obj->type)
        {
        case MAP_OBJ_FOLDER_LIST:
            mapc_adp_send_get_folder_list_cnf(BT_MAP_FAIL, &bd_addr, g_map_adp_cntx.client.instance, 0, 0, NULL);
            break;
        case MAP_OBJ_FOLDER_LIST_SIZE:
            mapc_adp_send_get_folder_list_size_cnf(BT_MAP_FAIL, &bd_addr, g_map_adp_cntx.client.instance, 0);
            break;
        case MAP_OBJ_MESSAGE_LIST:
            mapc_adp_send_get_message_list_cnf(BT_MAP_FAIL, &bd_addr, g_map_adp_cntx.client.instance, 0, 0, NULL, FALSE, NULL);
            break;
        case MAP_OBJ_MESSAGE_LIST_SIZE:
            mapc_adp_send_get_message_list_size_cnf(BT_MAP_FAIL, &bd_addr, g_map_adp_cntx.client.instance, 0, FALSE, NULL);
            break;
        case MAP_OBJ_MESSAGE:
            mapc_adp_send_get_message_cnf(BT_MAP_FAIL, &bd_addr, g_map_adp_cntx.client.instance, 0, 0, NULL);
            break;
        }
        break;
    case MAPC_EVENT_CONNECTED:
        {
            U8 *dev_name;
            map_conn_struct *conn = &g_map_adp_cntx.client.conn;
            if (conn->state == MAP_ADP_CONN_STATE_CONNECTING)
            {
                conn->state = MAP_ADP_CONN_STATE_CONNECTED;
                conn->id = parm->goep_conn_id;
                dev_name = BTBMGetLocalStoredName(parm->remote_addr.addr);
                mapc_adp_send_connect_cnf(BT_MAP_SUCCESS, &bd_addr, dev_name, g_map_adp_cntx.client.instance);
            }
        }
        break;
    case MAPC_EVENT_DISCONNECTED:
        {
            map_conn_struct *conn = &g_map_adp_cntx.client.conn;

            if (conn->state == MAP_ADP_CONN_STATE_DISCONNECTING)
            {
                mapc_adp_send_disconnect_cnf(BT_MAP_SUCCESS, &bd_addr, parm->mas_instance);
            }
            else if (conn->state != MAP_ADP_CONN_STATE_NULL)
            {
                mapc_adp_send_disconnect_ind(&bd_addr, parm->mas_instance);
            }
            conn->state = MAP_ADP_CONN_STATE_NULL;
        }
        break;
    case MAPC_EVENT_MNS_AUTHORIZE_IND:
        {
            U8 *dev_name;
            map_conn_struct *conn = &g_map_adp_cntx.mns_server_conn;
            conn->state = MAP_ADP_CONN_STATE_CONNECTING;
            conn->id = g_map_adp_cntx.client.instance;
            CONVERT_ARRAY2BDADDR(&conn->addr, parm->remote_addr.addr);
            dev_name = BTBMGetLocalStoredName(parm->remote_addr.addr);
            mapc_adp_send_mns_authorize_ind(&bd_addr, dev_name);
        }
        break;
    case MAPC_EVENT_MNS_CONNECT:
        {
            map_conn_struct *conn = &g_map_adp_cntx.mns_server_conn;
            mapc_mns_connect_rsp(parm->goep_conn_id, &parm->remote_addr, TRUE);
            conn->state = MAP_ADP_CONN_STATE_CONNECTED;
        }
        break;
    case MAPC_EVENT_MNS_EVENT_REPORT_START:
        {
            map_conn_struct *conn = &g_map_adp_cntx.mns_server_conn;
            conn->push_pull_obj = mapc_obs_new(parm->goep_conn_id); /* Use MNS conn ID */
            conn->push_pull_obj->type = MAP_OBJ_MNS_EVENT;
            sprintf(conn->push_pull_obj->file, BT_MAPC_EVENT_REPORT_FILE, g_map_adp_cntx.client.instance);
            btmtk_fs_delete(conn->push_pull_obj->file);
            parm->u.push_pull_obj = conn->push_pull_obj;
        }
        break;
    case MAPC_EVENT_MNS_PUT_COMPLETE:
        if (parm->u.push_pull_obj->fhandle >= 0)
        {
            btmtk_fs_close(parm->u.push_pull_obj->fhandle);
        }
        mapc_adp_send_mns_event_ind(&bd_addr, parm->mas_instance, (U16)parm->u.push_pull_obj->total_len, parm->u.push_pull_obj->file);
        break;
    case MAPC_EVENT_MNS_PUT_ABORTED:
        if (parm->u.push_pull_obj->fhandle >= 0)
        {
            btmtk_fs_close(parm->u.push_pull_obj->fhandle);
        }
        break;
    case MAPC_EVENT_MNS_DISCONNECTED:
        {
            map_conn_struct *conn = &g_map_adp_cntx.mns_server_conn;
            conn->state = MAP_ADP_CONN_STATE_NULL;
        }
        break;
    }
}

void mapc_adp_activate(ilm_struct *ilm_ptr)
{
    bt_mapc_activate_cnf_struct *cnf = 
        (bt_mapc_activate_cnf_struct *) construct_local_para(sizeof(bt_mapc_activate_cnf_struct), TD_UL | TD_RESET);;

    if (g_map_adp_cntx.initialized)
    {
        map_registration_struct mas_client, mns_server;
        cnf->result = BT_MAP_SUCCESS;

        mas_client.masc.callback = mapc_app_callback;
        mapc_register_client(&mas_client);
#ifdef __BT_MAP_MNS_SUPPORT__
        mns_server.mnss.security = BSL_DEFAULT_ALL;
        mns_server.mnss.callback = mapc_app_callback;
        mapc_mns_register_server(&mns_server);
#else
        mns_server.mnss.callback = NULL;
#endif
        g_map_adp_cntx.client.state = MAP_CLIENT_STATE_REGISTERED;
    }
    else
    {
        cnf->result = BT_MAP_FAIL;
    }
    map_send_msg(MSG_ID_BT_MAPC_ACTIVATE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

void mapc_adp_deactivate(ilm_struct *ilm_ptr)
{
    bt_mapc_deactivate_cnf_struct *cnf = 
        (bt_mapc_deactivate_cnf_struct *) construct_local_para(sizeof(bt_mapc_deactivate_cnf_struct), TD_UL | TD_RESET);

    if (g_map_adp_cntx.client.state >= MAP_CLIENT_STATE_REGISTERED)
    {
        /* Disconnect */
        if (g_map_adp_cntx.client.conn.state == MAP_ADP_CONN_STATE_CONNECTED)
        {
            mapc_disconnect();
        }
        mapc_deregister_client();
    }
    OS_MemSet((U8 *)&g_map_adp_cntx.client, 0x0, sizeof(map_client_struct));
    OS_MemSet((U8 *)&g_map_adp_cntx.mns_server_conn, 0x0, sizeof(map_conn_struct));
    cnf->result = BT_MAP_SUCCESS;
    map_send_msg(MSG_ID_BT_MAPC_DEACTIVATE_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

void mapc_adp_connect(ilm_struct *ilm_ptr)
{
    bt_mapc_connect_req_struct *msg = ilm_ptr->local_para_ptr;
    BD_ADDR remote_addr;
    BtStatus status;
    map_conn_struct *conn;
    U8 *dev_name = NULL;
    U16 addr_len = sizeof(bt_map_addr_struct);
    
    conn = &g_map_adp_cntx.client.conn;
    switch (conn->state)
    {
    case MAP_ADP_CONN_STATE_NULL:
        OS_MemCopy((U8 *)&conn->addr, (U8 *) &msg->addr, addr_len);
        conn->state = MAP_ADP_CONN_STATE_CONNECTING;
        CONVERT_BDADDR2ARRAY(remote_addr.addr, msg->addr.lap, msg->addr.uap, msg->addr.nap);
        status = mapc_connect(&remote_addr, AUTH_OPT_DEFAULT, NULL, NULL);
        g_map_adp_cntx.client.instance = msg->mas_id;
    
        if (status != BT_STATUS_PENDING)
        {
            if (status == BT_STATUS_SUCCESS)
            {
                conn->state = MAP_ADP_CONN_STATE_CONNECTED;
            }
            else
            {
                conn->state = MAP_ADP_CONN_STATE_NULL;
            }
            dev_name = BTBMGetLocalStoredName(remote_addr.addr);
            mapc_adp_send_connect_cnf(map_util_translate_result(status), &msg->addr, dev_name, msg->mas_id);
        }
        break;
    case MAP_ADP_CONN_STATE_CONNECTING:
        if (map_util_compare_address(&conn->addr, &msg->addr) || g_map_adp_cntx.client.instance != msg->mas_id)
        {
            mapc_adp_send_connect_cnf(BT_MAP_FAIL_BUSY, &msg->addr, NULL, msg->mas_id);
        }
        break;
    case MAP_ADP_CONN_STATE_CONNECTED:
        if (map_util_compare_address(&conn->addr, &msg->addr) || g_map_adp_cntx.client.instance != msg->mas_id)
        {
            mapc_adp_send_connect_cnf(BT_MAP_FAIL_BUSY, &msg->addr, NULL, msg->mas_id);
        }
        else
        {
            mapc_adp_send_connect_cnf(BT_MAP_SUCCESS, &msg->addr, NULL, msg->mas_id);
        }
        break;
    case MAP_ADP_CONN_STATE_DISCONNECTING:
        mapc_adp_send_connect_cnf(BT_MAP_FAIL_BUSY, &msg->addr, NULL, msg->mas_id);
        break;
    default:
        ASSERT(0);
    }
}

void mapc_adp_disconnect(ilm_struct *ilm_ptr)
{
    bt_mapc_disconnect_req_struct *msg = ilm_ptr->local_para_ptr;
    map_conn_struct *conn;
    BtStatus status;

    conn = &g_map_adp_cntx.client.conn;
    if (conn->state != MAP_ADP_CONN_STATE_NULL)
    {
        if (conn->state == MAP_ADP_CONN_STATE_CONNECTED || conn->state == MAP_ADP_CONN_STATE_CONNECTING)
        {
            conn->state = MAP_ADP_CONN_STATE_DISCONNECTING;
            status = mapc_disconnect();
            
            if (status != BT_STATUS_PENDING)
            {
                conn->state = MAP_ADP_CONN_STATE_NULL;
                mapc_adp_send_disconnect_cnf(map_util_translate_result(status), &msg->addr, msg->mas_id);
            }
        }
    }
    else
    {
        mapc_adp_send_disconnect_cnf(BT_MAP_SUCCESS, &msg->addr, msg->mas_id);
    }
}

void mapc_adp_set_folder(ilm_struct *ilm_ptr)
{

    bt_mapc_set_folder_req_struct *msg = ilm_ptr->local_para_ptr;
    map_conn_struct *conn;
    char str[MAP_MAX_FOLDER_PATH_LEN];
    U16 addr_len = sizeof(bt_map_addr_struct);

    conn = &g_map_adp_cntx.client.conn;
    if (g_map_adp_cntx.client.instance == msg->mas_id &&
        map_util_compare_address(&conn->addr, &msg->addr) == 0)
    {
        conn->push_pull_obj = mapc_obs_new(g_map_adp_cntx.client.conn.id); /* Use MAP conn ID */
        switch (msg->flag)
        {
        case BT_MAP_FOLDER_OP_ROOT:
            conn->push_pull_obj->u.set_folder.op = MAP_FOLDER_ROOT;
            break;
        case BT_MAP_FOLDER_OP_NEXT:
            bt_chset_utf8_to_ucs2_string(str, MAP_MAX_FOLDER_PATH_LEN, msg->folder);
            bt_ucs2swap_endian(conn->push_pull_obj->u.set_folder.folder, str);
            /* fall-throught */
        case BT_MAP_FOLDER_OP_UP:
            conn->push_pull_obj->u.set_folder.op = MAP_FOLDER_UP;
            break;
        case BT_MAP_FOLDER_OP_DOWN:
            conn->push_pull_obj->u.set_folder.op = MAP_FOLDER_DOWN;
            bt_chset_utf8_to_ucs2_string(str, MAP_MAX_FOLDER_PATH_LEN, msg->folder);
            bt_ucs2swap_endian(conn->push_pull_obj->u.set_folder.folder, str);
            break;
        }
        mapc_set_folder(conn->push_pull_obj);
    }
}

void mapc_adp_get_folder_listing_size(ilm_struct *ilm_ptr)
{
    bt_mapc_get_folder_listing_size_req_struct *msg = ilm_ptr->local_para_ptr;
    U16 addr_len = sizeof(bt_map_addr_struct);
    map_conn_struct *conn;

    conn = &g_map_adp_cntx.client.conn;
    if (g_map_adp_cntx.client.instance == msg->mas_id &&
        map_util_compare_address(&conn->addr, &msg->addr) == 0)
    {
        conn->push_pull_obj = mapc_obs_new(g_map_adp_cntx.client.conn.id);
        conn->push_pull_obj->type = MAP_OBJ_FOLDER_LIST_SIZE;
        conn->push_pull_obj->u.folder_list.max_list_count = 0;
        conn->push_pull_obj->u.folder_list.list_start_offset = 0;
        mapc_get_folder_listing(conn->push_pull_obj);
    }
}

void mapc_adp_get_folder_listing(ilm_struct *ilm_ptr)
{
    bt_mapc_get_folder_listing_req_struct *msg = ilm_ptr->local_para_ptr;
    U16 addr_len = sizeof(bt_map_addr_struct);
    map_conn_struct *conn;

    conn = &g_map_adp_cntx.client.conn;
    if (g_map_adp_cntx.client.instance == msg->mas_id &&
        map_util_compare_address(&conn->addr, &msg->addr) == 0)
    {
        conn->push_pull_obj = mapc_obs_new(g_map_adp_cntx.client.conn.id);
        conn->push_pull_obj->type = MAP_OBJ_FOLDER_LIST;
        sprintf(conn->push_pull_obj->file, BT_MAPC_FOLDER_LIST_FILE, msg->mas_id);
        btmtk_fs_delete(conn->push_pull_obj->file);
        conn->push_pull_obj->u.folder_list.max_list_count = msg->list_size;
        conn->push_pull_obj->u.folder_list.list_start_offset = msg->list_offset;
        mapc_get_folder_listing(conn->push_pull_obj);
    }
}

void mapc_adp_get_message_listing_size(ilm_struct *ilm_ptr)
{
    bt_mapc_get_message_listing_size_req_struct *msg = ilm_ptr->local_para_ptr;
    U16 addr_len = sizeof(bt_map_addr_struct);
    char str[MAP_MAX_FOLDER_PATH_LEN];
    map_conn_struct *conn;

    conn = &g_map_adp_cntx.client.conn;
    if (g_map_adp_cntx.client.instance == msg->mas_id &&
        map_util_compare_address(&conn->addr, &msg->addr) == 0)
    {
        conn->push_pull_obj = mapc_obs_new(g_map_adp_cntx.client.conn.id);
        conn->push_pull_obj->type = MAP_OBJ_MESSAGE_LIST_SIZE;
        bt_chset_utf8_to_ucs2_string(str, MAP_MAX_FOLDER_PATH_LEN, msg->child_folder);
        bt_ucs2swap_endian(conn->push_pull_obj->u.message_list.folder, str);
        conn->push_pull_obj->u.message_list.max_list_count = 0;
        conn->push_pull_obj->u.message_list.list_start_offset = 0;
        mapc_get_message_listing(conn->push_pull_obj);
    }
}

void mapc_adp_get_message_listing(ilm_struct *ilm_ptr)
{
    bt_mapc_get_message_listing_req_struct *msg = ilm_ptr->local_para_ptr;
    U16 addr_len = sizeof(bt_map_addr_struct);
    char str[MAP_MAX_FOLDER_PATH_LEN];
    map_conn_struct *conn;

    conn = &g_map_adp_cntx.client.conn;
    if (g_map_adp_cntx.client.instance == msg->mas_id &&
        map_util_compare_address(&conn->addr, &msg->addr) == 0)
    {
        conn->push_pull_obj = mapc_obs_new(g_map_adp_cntx.client.conn.id);
        conn->push_pull_obj->type = MAP_OBJ_MESSAGE_LIST;
        sprintf(conn->push_pull_obj->file, BT_MAPC_MESSAGE_LIST_FILE, msg->mas_id);
        btmtk_fs_delete(conn->push_pull_obj->file);
        bt_chset_utf8_to_ucs2_string(str, MAP_MAX_FOLDER_PATH_LEN, msg->child_folder);
        bt_ucs2swap_endian(conn->push_pull_obj->u.message_list.folder, str);
        conn->push_pull_obj->u.message_list.max_list_count = msg->list_size;
        conn->push_pull_obj->u.message_list.list_start_offset = msg->list_offset;
        conn->push_pull_obj->u.message_list.subject_length = msg->max_subject_len;
        conn->push_pull_obj->u.message_list.parameter_mask = map_util_translate_mask_type_bt2map(msg->mask);
        conn->push_pull_obj->u.message_list.filter_message_type = map_util_translate_msg_type_bt2map(msg->filter_msg);
        OS_StrnCpy(conn->push_pull_obj->u.message_list.filter_period_begin, msg->filter_begin, MAP_MAX_DATE_LEN);
        OS_StrnCpy(conn->push_pull_obj->u.message_list.filter_period_end, msg->filter_end, MAP_MAX_DATE_LEN);
        conn->push_pull_obj->u.message_list.filter_read_status = map_util_translate_read_filter_type_bt2map(msg->filter_status);
        OS_StrnCpy(conn->push_pull_obj->u.message_list.filter_recipient, msg->filter_rec, MAP_MAX_FILTER_TEXT_LEN);
        OS_StrnCpy(conn->push_pull_obj->u.message_list.filter_originator, msg->filter_orig, MAP_MAX_FILTER_TEXT_LEN);
        conn->push_pull_obj->u.message_list.filter_priority = map_util_translate_priority_filter_type_bt2map(msg->filter_prio);
        mapc_get_message_listing(conn->push_pull_obj);
    }
}

void mapc_adp_get_message(ilm_struct *ilm_ptr)
{
    bt_mapc_get_message_req_struct *msg = ilm_ptr->local_para_ptr;
    U16 addr_len = sizeof(bt_map_addr_struct);
    char str[MAP_MAX_FOLDER_PATH_LEN];
    map_conn_struct *conn;

    conn = &g_map_adp_cntx.client.conn;
    if (g_map_adp_cntx.client.instance == msg->mas_id &&
        map_util_compare_address(&conn->addr, &msg->addr) == 0)
    {
        conn->push_pull_obj = mapc_obs_new(g_map_adp_cntx.client.conn.id);
        conn->push_pull_obj->type = MAP_OBJ_MESSAGE;
        sprintf(conn->push_pull_obj->file, BT_MAPC_MESSAGE_GET_FILE, msg->mas_id);
        btmtk_fs_delete(conn->push_pull_obj->file);
        bt_chset_utf8_to_ucs2_string(str, MAP_MAX_FOLDER_PATH_LEN, msg->handle);
        bt_ucs2swap_endian(conn->push_pull_obj->u.message.handle, str);
        conn->push_pull_obj->u.message.attachment = msg->attachment;
        conn->push_pull_obj->u.message.charset = map_util_translate_charset_type_bt2map(msg->charset);
        conn->push_pull_obj->u.message.fraction_request = map_util_translate_fraction_req_type_bt2map(msg->fraction_req);
        conn->push_pull_obj->u.message.fraction_deliver = MAP_FRACTION_DELIV_NO;
        mapc_get_message(conn->push_pull_obj);
    }
}

void mapc_adp_set_notif_registration(ilm_struct *ilm_ptr)
{
    bt_mapc_set_notif_registration_req_struct *msg = ilm_ptr->local_para_ptr;
    U16 addr_len = sizeof(bt_map_addr_struct);
    map_conn_struct *conn;

    conn = &g_map_adp_cntx.client.conn;
    if (g_map_adp_cntx.client.instance == msg->mas_id &&
        map_util_compare_address(&conn->addr, &msg->addr) == 0)
    {
        conn->push_pull_obj = mapc_obs_new(g_map_adp_cntx.client.conn.id);
        conn->push_pull_obj->type = MAP_OBJ_NOTIF_REGISTRATION;
        conn->push_pull_obj->u.notif_registration.enable = msg->on;
        conn->push_pull_obj->total_len = 1;
        mapc_set_notification_registration(conn->push_pull_obj);
    }
}

void mapc_adp_set_message_status(ilm_struct *ilm_ptr)
{
    bt_mapc_set_message_status_req_struct *msg = ilm_ptr->local_para_ptr;
    U16 addr_len = sizeof(bt_map_addr_struct);
    char str[MAP_MAX_FOLDER_PATH_LEN];
    map_conn_struct *conn;

    conn = &g_map_adp_cntx.client.conn;
    if (g_map_adp_cntx.client.instance == msg->mas_id &&
        map_util_compare_address(&conn->addr, &msg->addr) == 0)
    {
        conn->push_pull_obj = mapc_obs_new(g_map_adp_cntx.client.conn.id);
        conn->push_pull_obj->type = MAP_OBJ_MESSAGE_STATUS;
        conn->push_pull_obj->total_len = 1;
        bt_chset_utf8_to_ucs2_string(str, MAP_MAX_FOLDER_PATH_LEN, msg->handle);
        bt_ucs2swap_endian(conn->push_pull_obj->u.msg_status.handle, str);
        switch(msg->status)
        {
        case BT_MAP_MSG_STATUS_READ:
            conn->push_pull_obj->u.msg_status.type = MAP_MSG_STATUS_READ;
            conn->push_pull_obj->u.msg_status.enable = TRUE;
            break;
        case BT_MAP_MSG_STATUS_UNREAD:
            conn->push_pull_obj->u.msg_status.type = MAP_MSG_STATUS_READ;
            conn->push_pull_obj->u.msg_status.enable = FALSE;
            break;
        case BT_MAP_MSG_STATUS_DELETE:
            conn->push_pull_obj->u.msg_status.type = MAP_MSG_STATUS_DELETE;
            conn->push_pull_obj->u.msg_status.enable = TRUE;
            break;
        case BT_MAP_MSG_STATUS_UNDELETE:
            conn->push_pull_obj->u.msg_status.type = MAP_MSG_STATUS_DELETE;
            conn->push_pull_obj->u.msg_status.enable = FALSE;
            break;
        }
        mapc_set_message_status(conn->push_pull_obj);
    }
}

void mapc_adp_push_message(ilm_struct *ilm_ptr)
{
    bt_mapc_push_message_req_struct *msg = ilm_ptr->local_para_ptr;
    U16 addr_len = sizeof(bt_map_addr_struct);
    char str[MAP_MAX_FOLDER_PATH_LEN];
    map_conn_struct *conn;

    conn = &g_map_adp_cntx.client.conn;
    if (g_map_adp_cntx.client.instance == msg->mas_id &&
        map_util_compare_address(&conn->addr, &msg->addr) == 0)
    {
        conn->push_pull_obj = mapc_obs_new(g_map_adp_cntx.client.conn.id);
        conn->push_pull_obj->type = MAP_OBJ_MESSAGE;
        OS_StrnCpy(conn->push_pull_obj->file, msg->file, MAP_MAX_TEMP_FILE_NAME_LEN);
        conn->push_pull_obj->total_len = msg->data_size;
        bt_chset_utf8_to_ucs2_string(str, MAP_MAX_FOLDER_PATH_LEN, msg->child_folder);
        bt_ucs2swap_endian(conn->push_pull_obj->u.msg_status.handle, str);
        conn->push_pull_obj->u.put_message.transparent = (msg->sent_op== BT_MAP_SENT);
        conn->push_pull_obj->u.put_message.retry = msg->retry;
        conn->push_pull_obj->u.put_message.charset = map_util_translate_charset_type_bt2map(msg->charset);
        mapc_push_message(conn->push_pull_obj);
    }
}

void mapc_adp_update_inbox(ilm_struct *ilm_ptr)
{
    bt_mapc_update_inbox_req_struct *msg = ilm_ptr->local_para_ptr;
    U16 addr_len = sizeof(bt_map_addr_struct);
    map_conn_struct *conn;

    conn = &g_map_adp_cntx.client.conn;
    if (g_map_adp_cntx.client.instance == msg->mas_id &&
        map_util_compare_address(&conn->addr, &msg->addr) == 0)
    {
        conn->push_pull_obj = mapc_obs_new(g_map_adp_cntx.client.conn.id);
        conn->push_pull_obj->type = MAP_OBJ_INBOX_UPDATE;
        conn->push_pull_obj->total_len = 1;
        mapc_update_inbox(conn->push_pull_obj);
    }
}

void mapc_adp_mns_authorize_response(ilm_struct *ilm_ptr)
{
    bt_mapc_mns_authorize_rsp_struct *msg = ilm_ptr->local_para_ptr;
    BD_ADDR remote_addr;
    U8 rsp_code;

    rsp_code = (msg->accept) ? GOEP_STATUS_SUCCESS : GOEP_STATUS_FAILED;
    CONVERT_BDADDR2ARRAY(remote_addr.addr, msg->addr.lap, msg->addr.uap, msg->addr.nap);
    mapc_mns_authorize_rsp(&remote_addr, rsp_code);
}

void mapc_adp_mns_disconnect(ilm_struct *ilm_ptr)
{

    bt_mapc_mns_disconnect_req_struct *msg = ilm_ptr->local_para_ptr;
    bt_mapc_mns_disconnect_cnf_struct *cnf =
        (bt_mapc_mns_disconnect_cnf_struct *) construct_local_para(sizeof(bt_mapc_mns_disconnect_cnf_struct), TD_UL | TD_RESET);
    map_conn_struct *conn;
    BtStatus status;
    U16 addr_len = sizeof(bt_map_addr_struct);

    conn = &g_map_adp_cntx.client.conn;
    if (map_util_compare_address(&conn->addr, &msg->addr) == 0)
    {
        status = mapc_mns_disconnect(conn->id);
        if (status == BT_STATUS_PENDING)
        {
            conn->state = MAP_ADP_CONN_STATE_DISCONNECTING;
            return;
        }
        else
        {
            conn->state = MAP_ADP_CONN_STATE_NULL;
            cnf->result = map_util_translate_result(status);
        }
        OS_MemCopy((U8 *)&cnf->addr, (U8 *)&conn->addr, sizeof(bt_map_addr_struct));
    }
    else
    {
        cnf->result = BT_MAP_SUCCESS;
    }
    map_send_msg(MSG_ID_BT_MAPC_MNS_DISCONNECT_CNF, ilm_ptr->src_mod_id, BT_APP_SAP, (local_para_struct *)cnf, NULL);
}

void mapc_adp_mns_send_event_response(ilm_struct *ilm_ptr)
{
    bt_mapc_mns_send_event_rsp_struct *msg = ilm_ptr->local_para_ptr;
    map_conn_struct *conn;
    U16 addr_len = sizeof(bt_map_addr_struct);

    conn = &g_map_adp_cntx.client.conn;
    if (g_map_adp_cntx.client.instance == msg->mas_id &&
        map_util_compare_address(&conn->addr, &msg->addr) == 0)
    {
        U8 rsp_code = map_util_translate_map2obex_result(msg->result);
        mapc_mns_send_event_response(conn->id, rsp_code);
    }
}

void mapc_adp_abort(ilm_struct *ilm_ptr)
{
    bt_mapc_abort_req_struct *msg = ilm_ptr->local_para_ptr;
    U16 addr_len = sizeof(bt_map_addr_struct);

    if (g_map_adp_cntx.client.instance == msg->mas_id &&
        map_util_compare_address(&g_map_adp_cntx.client.conn.addr, &msg->addr) == 0)
    {
        mapc_abort();
    }
}
#endif

void map_adp_init(ilm_struct *ilm_ptr)
{
    OS_Report("map_adp_init");
    if (g_map_adp_cntx.initialized == FALSE)
    {

        OS_MemSet((U8 *)&g_map_adp_cntx, 0x0, sizeof(map_adp_context));

        /* Create working folder */
        if (!btmtk_fs_is_dir_exist((U8 *)MAP_ADP_WORK_FOLDER))
        {
            btmtk_fs_create_dir((U8 *)MAP_ADP_WORK_FOLDER);
        }
        map_init();
    }
    g_map_adp_cntx.initialized = TRUE;
}

void map_adp_deinit(ilm_struct *ilm_ptr)
{
    if (g_map_adp_cntx.initialized == TRUE)
    {
        map_deinit();
        g_map_adp_cntx.initialized = FALSE;
    }
}


void map_adp_message_handler(ilm_struct *ilm_ptr)
{
    switch (ilm_ptr->msg_id)
    {
#ifdef __BT_MAPS_PROFILE__
    case MSG_ID_BT_MAPS_ACTIVATE_REQ:
        maps_adp_activate(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_DEACTIVATE_REQ:
        maps_adp_deactivate(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_REGISTER_REQ:
        maps_adp_register_server(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_DEREGISTER_REQ:
        maps_adp_deregister_server(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_AUTHORIZE_RSP:
        maps_adp_authorize_rsp(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_CONNECT_RSP:
        maps_adp_connect_rsp(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_DISCONNECT_REQ:
        maps_adp_disconnect(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_ABORT_REQ:
        maps_adp_abort(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_SET_FOLDER_RSP:
        maps_adp_set_folder_response(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_GET_FOLDER_LISTING_RSP:
        maps_adp_get_folder_listing_response(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_GET_MESSAGE_LISTING_RSP:
        maps_adp_get_message_listing_response(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_GET_MESSAGE_RSP:
        maps_adp_get_message_response(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_SET_NOTIF_REGISTRATION_RSP:
        maps_adp_set_notification_registration_response(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_SET_MESSAGE_STATUS_RSP:
        maps_adp_set_message_status_response(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_PUSH_MESSAGE_RSP:
        maps_adp_push_message_response(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_UPDATE_INBOX_RSP:
        maps_adp_update_inbox_response(ilm_ptr);
        break;
#ifdef __BT_MAP_MNS_SUPPORT__
    case MSG_ID_BT_MAPS_MNS_CONNECT_REQ:
        maps_adp_mns_connect(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_MNS_DISCONNECT_REQ:
        maps_adp_mns_disconnect(ilm_ptr);
        break;
    case MSG_ID_BT_MAPS_MNS_SEND_EVENT_REQ:
        maps_adp_mns_send_event(ilm_ptr);
        break;
#endif
#endif
#ifdef __BT_MAPC_PROFILE__
    case MSG_ID_BT_MAPC_ACTIVATE_REQ:
        mapc_adp_activate(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_DEACTIVATE_REQ:
        mapc_adp_deactivate(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_CONNECT_REQ:
        mapc_adp_connect(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_DISCONNECT_REQ:
        mapc_adp_disconnect(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_SET_FOLDER_REQ:
        mapc_adp_set_folder(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_GET_FOLDER_LISTING_SIZE_REQ:
        mapc_adp_get_folder_listing_size(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_GET_FOLDER_LISTING_REQ:
        mapc_adp_get_folder_listing(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_GET_MESSAGE_LISTING_SIZE_REQ:
        mapc_adp_get_message_listing_size(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_GET_MESSAGE_LISTING_REQ:
        mapc_adp_get_message_listing(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_GET_MESSAGE_REQ:
        mapc_adp_get_message(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_SET_NOTIF_REGISTRATION_REQ:
        mapc_adp_set_notif_registration(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_SET_MESSAGE_STATUS_REQ:
        mapc_adp_set_message_status(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_PUSH_MESSAGE_REQ:
        mapc_adp_push_message(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_UPDATE_INBOX_REQ:
        mapc_adp_update_inbox(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_ABORT_REQ:
        mapc_adp_abort(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_MNS_AUTHORIZE_RSP:
        mapc_adp_mns_authorize_response(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_MNS_DISCONNECT_REQ:
        mapc_adp_mns_disconnect(ilm_ptr);
        break;
    case MSG_ID_BT_MAPC_MNS_SEND_EVENT_RSP:
        mapc_adp_mns_send_event_response(ilm_ptr);
        break;
#endif
    }
}

#endif /* defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__) */
