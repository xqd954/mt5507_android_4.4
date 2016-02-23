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
 *   map.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file contains public API functions for the  Message Access Profile (MAP).
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
#include "bt_common.h"
#include "sdp_adp.h"
#include "sdpdb.h"
#include "goep.h"
#include "goepext.h"

#include "map_adp.h"
#include "map.h"
#include "map_obs.h"

extern void MAPSDeviceSdpDB_ReplaceRecord(U8 idx, U16 uuid, U8 *attribute, U8 size);

static ObexRespCode maps_obs_read(void *obs, U8* buff, U16 len);
static ObexRespCode maps_obs_write(void *obs, U8* buff, U16 len);
static U32 maps_obs_get_object_len(void *obs);
static ObexRespCode maps_mns_obs_read(void *obs, U8* buff, U16 len);
static U32 maps_mns_obs_get_object_len(void *obs);
static ObexRespCode mapc_obs_read(void *obs, U8* buff, U16 len);
static ObexRespCode mapc_obs_write(void *obs, U8* buff, U16 len);
static U32 mapc_obs_get_object_len(void *obs);
static ObexRespCode mapc_mns_obs_write(void *obs, U8* buff, U16 len);

#if NUM_MAP_SRVS > 0
static const ObStoreFuncTable maps_store_func_table = {
    maps_obs_read,
    maps_obs_write,
    maps_obs_get_object_len,
};

#ifdef __BT_MAP_MNS_SUPPORT__
static const ObStoreFuncTable maps_mns_store_func_table = {
    maps_mns_obs_read,
    NULL,
    maps_mns_obs_get_object_len,
};
#endif
#endif /* NUM_MAP_SRVS */

#if NUM_MAP_CLIS > 0
static const ObStoreFuncTable mapc_store_func_table = {
    mapc_obs_read,
    mapc_obs_write,
    mapc_obs_get_object_len,
};
#ifdef __BT_MAP_MNS_SUPPORT__
static const ObStoreFuncTable mapc_mns_store_func_table = {
    NULL,
    mapc_mns_obs_write,
    NULL
};
#endif
#endif /* NUM_MAP_CLIS */


//MapContext g_map_cntx = {0};
MapContext g_map_cntx ;


static U8 g_map_remote_mas_UUID[] = {
    0xBB, 0x58, 0x2B, 0x40, 0x42, 0x0C, 0x11, 0xDB,
    0xB0, 0xDE, 0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66
};

static U8 g_map_remote_mns_UUID[] = {
    0xBB, 0x58, 0x2B, 0x41, 0x42, 0x0C, 0x11, 0xDB,
    0xB0, 0xDE, 0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66
};

static U8 g_map_filler_byte = 0x30;

static const U8 g_map_bt_goep_service[] = {GOEP_PROFILE_MAP_SERVER1, GOEP_PROFILE_MAP_SERVER2, GOEP_PROFILE_MAP_SERVER3, GOEP_PROFILE_MAP_SERVER4};
static const U8 g_map_mns_bt_goep_service[] = {GOEP_PROFILE_MNS_CLIENT1, GOEP_PROFILE_MNS_CLIENT2};

#ifdef __BT_MAPS_PROFILE__
extern SdpAttribute *MapServerSdpAttributesList[NUM_MAP_SRVS];

S8 maps_get_curr_registration(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (; i < NUM_MAP_SRVS; i++)
    {
        if (g_map_cntx.servers[i].state == MAP_APP_STATE_REGISTERING)
        {
            return i;
        }
    }
    return -1;
}

S8 maps_get_curr_deregistration(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (; i < NUM_MAP_SRVS; i++)
    {
        if (g_map_cntx.servers[i].state == MAP_APP_STATE_DEREGISTERING)
        {
            return i;
        }
    }
    return -1;
}

MapServer *maps_get_server_instance(U8 instance)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (; i < NUM_MAP_SRVS; i++)
    {
        if (g_map_cntx.servers[i].appInfo.mass.instance == instance)
        {
            return &g_map_cntx.servers[i];
        }
    }
    return NULL;
}

MapServer *maps_get_server_instance_by_goep_id(U8 connId)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i, j;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < NUM_MAP_SRVS; i++)
    {
        for (j = 0; j < NUM_MAP_SRV_CONN; j++)
        {
            if (g_map_cntx.servers[i].state == MAP_APP_STATE_REGISTERED &&
                g_map_cntx.servers[i].mapConn[j].goep_conn_id == connId)
            {
                return &g_map_cntx.servers[i];
            }
        }
    }
    return NULL;
}

GoepServerApp *maps_get_server_app_by_goep_id(U8 connId)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i, j;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < NUM_MAP_SRVS; i++)
    {
        for (j = 0; j < NUM_MAP_SRV_CONN; j++)
        {
            if (g_map_cntx.servers[i].state == MAP_APP_STATE_REGISTERED &&
                g_map_cntx.servers[i].sApp[j].connId == connId)
            {
                return &g_map_cntx.servers[i].sApp[j];
            }
        }
    }
    return NULL;
}

MapServer *maps_get_server_instance_by_handler(GoepServerApp *handler)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i, j;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < NUM_MAP_SRVS; i++)
    {
        for (j = 0; j < NUM_MAP_SRV_CONN; j++)
        {
            if (&g_map_cntx.servers[i].sApp[j] == handler)
            {
                return &g_map_cntx.servers[i];
            }
        }
    }
    return NULL;
}

MapConn *maps_get_server_conn_by_goep_id(U8 goep_conn_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i, j;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < NUM_MAP_SRVS; i++)
    {
        for (j = 0; j < NUM_MAP_SRV_CONN; j++)
        {
            if (g_map_cntx.servers[i].mapConn[j].goep_conn_id == goep_conn_id)
            {
                return &g_map_cntx.servers[i].mapConn[j];
            }
        }
    }
    return NULL;
}

MapClient *maps_get_mns_client_by_goep_id(U8 conn_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int j;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (j = 0; j < NUM_MAP_SRV_CONN; j++)
    {
        if (g_map_cntx.mnsClient[j].mapConn.goep_conn_id == conn_id)
        {
            return &g_map_cntx.mnsClient[j];
        }
    }
    return NULL;
}

MapClient *maps_get_mns_client_by_handler(GoepClientApp *handler)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < NUM_MAP_SRV_CONN; i++)
    {
        if (&g_map_cntx.mnsClient[i].cApp == handler)
        {
            return &g_map_cntx.mnsClient[i];
        }
    }
    return NULL;
}

ObexRespCode maps_obs_read(void *obs, U8* buff, U16 len)
{
    map_obj_struct *push_pull_obj = (map_obj_struct *) obs;
    MapServer *server = maps_get_server_instance_by_goep_id(push_pull_obj->conn_id);
    MapConn *conn = maps_get_server_conn_by_goep_id(push_pull_obj->conn_id);
    map_server_app_callback callback = server->appInfo.mass.callback;
    maps_callback_parm_struct *parm = &conn->cbParm.sParm;

    bt_trace(TRACE_GROUP_9, BTLOG_MAPS_OBS_READ, len, push_pull_obj->offset);
    parm->event = MAPS_EVENT_DATA_REQ;
    parm->u.push_pull_obj->buff = buff;
    parm->u.push_pull_obj->buff_len = len;
    callback(parm);
    if (parm->u.push_pull_obj->buff_len == len)
    {
        push_pull_obj->offset += len;
        return OBRC_SUCCESS;
    }
    else
    {
        return OBRC_FORBIDDEN;
    }
}

ObexRespCode maps_obs_write(void *obs, U8* buff, U16 len)
{
    map_obj_struct *push_pull_obj = (map_obj_struct *) obs;
    MapServer *server = maps_get_server_instance_by_goep_id(push_pull_obj->conn_id);
    MapConn *conn = maps_get_server_conn_by_goep_id(push_pull_obj->conn_id);
    map_server_app_callback callback = server->appInfo.mass.callback;
    maps_callback_parm_struct *parm = &conn->cbParm.sParm;

    if (push_pull_obj->type == MAP_OBJ_NOTIF_REGISTRATION ||
        push_pull_obj->type == MAP_OBJ_INBOX_UPDATE ||
        push_pull_obj->type == MAP_OBJ_MESSAGE_STATUS)
    {
        return OBRC_SUCCESS;
    }
    else
    {
        bt_trace(TRACE_GROUP_9, BTLOG_MAPS_OBS_WRITE, len, push_pull_obj->offset);
        parm->event = MAPS_EVENT_DATA_IND;
        parm->u.push_pull_obj->buff = buff;
        parm->u.push_pull_obj->buff_len = len;
        callback(parm);
        if (parm->u.push_pull_obj->buff_len == len)
        {
            push_pull_obj->offset += len;
            return OBRC_SUCCESS;
        }
        else
        {
            return OBRC_FORBIDDEN;
        }
    }
}

static U32 maps_obs_get_object_len(void *obs)
{
    map_obj_struct *push_pull_obj = (map_obj_struct *) obs;

    bt_trace(TRACE_GROUP_9, BTLOG_MAPS_OBS_GET_OBJECT_LEN, push_pull_obj->total_len);
    return (push_pull_obj ? push_pull_obj->total_len : 0);
}

static BtStatus maps_send_response(U8 connId, U8 rsp_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    GoepServerApp *sApp = maps_get_server_app_by_goep_id(connId);
    MapConn *conn = maps_get_server_conn_by_goep_id(connId);
    ObStatus status = OB_STATUS_FAILED;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

    if(sApp != NULL && conn != NULL)
    {
        sApp->connId = conn->goep_conn_id;
        if(rsp_code != OBRC_SUCCESS)
        {
            GOEP_ServerAbort(sApp, rsp_code);
        }

        GOEP_ServerContinue(sApp);
        status = OB_STATUS_SUCCESS;
    }

    OS_UnlockObex();
    return status;
}

static BtStatus maps_pull_response(U8 connId, U8 rsp_code, map_obj_struct *obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    GoepServerApp *sApp = maps_get_server_app_by_goep_id(connId);
    MapConn *conn = maps_get_server_conn_by_goep_id(connId);
    ObStatus status = OB_STATUS_FAILED;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

    if(sApp != NULL && conn != NULL)
    {
        obj->conn_id = connId;
        sApp->connId = conn->goep_conn_id;
        GOEP_SetPullObjectLength(sApp, obj->total_len);

        if(rsp_code == OBRC_SUCCESS)
        {
            if (obj->total_len > 0) 
            {
            GOEP_ServerAccept(sApp, obj); 
        }
             else if (obj->total_len <= 0) {
          	GOEP_ServerAcceptNoObject(sApp);
             }
        }
        else
        {
            GOEP_ServerAbort(sApp, rsp_code);
        }

        GOEP_ServerContinue(sApp);
        status = OB_STATUS_SUCCESS;
    }
    
    OS_UnlockObex();
    return status;
}

BtStatus maps_register_server(map_registration_struct *server)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 idx = 0, i = 0;
    ObStatus status = OB_STATUS_FAILED;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

    for (; idx < NUM_MAP_SRVS && g_map_cntx.servers[idx].state >= MAP_APP_STATE_REGISTERING; idx++);
 //   ASSERT(idx < NUM_MAP_SRVS);
 	if (idx >= NUM_MAP_SRVS)
 	{
		OS_Report("[MAP]Err: no available server resource to register");
		return BT_STATUS_NO_RESOURCES;
	}

    OS_MemSet((U8 *)&g_map_cntx.servers[idx], 0, sizeof(MapServer));
    OS_MemCopy((U8 *)&g_map_cntx.servers[idx].appInfo, (U8 *)server, sizeof(map_registration_struct));

    g_map_cntx.servers[idx].sdpRec = MapServerSdpAttributesList[idx];
    g_map_cntx.servers[idx].state = MAP_APP_STATE_REGISTERING;
    MAPSDeviceSdpDB_ReplaceRecord(idx, AID_MAP_MAS_INSTANCE_ID, 
                                  &g_map_cntx.servers[idx].appInfo.mass.instance, 1);
    MAPSDeviceSdpDB_ReplaceRecord(idx, AID_MAP_SUPPORTED_MSG_TYPES, 
                                  (U8 *)&g_map_cntx.servers[idx].appInfo.mass.supported_msg_type, 1);
    MAPSDeviceSdpDB_ReplaceRecord(idx, (AID_SERVICE_NAME + 0x0100), 
                                  g_map_cntx.servers[idx].appInfo.mass.service_name, 
                                  (U8)OS_StrLen((char *)g_map_cntx.servers[idx].appInfo.mass.service_name));
    for (; i < NUM_MAP_SRV_CONN; i++)
    {
        g_map_cntx.servers[idx].mapConn[i].obexConn.target = g_map_remote_mas_UUID;
        g_map_cntx.servers[idx].mapConn[i].obexConn.targetLen = 16;

        g_map_cntx.servers[idx].sApp[i].bt_service = g_map_bt_goep_service[idx];
        g_map_cntx.servers[idx].sApp[i].type = g_map_bt_goep_service[idx]; /* We allow only one service to be registered in one OBEX channel */
        g_map_cntx.servers[idx].sApp[i].connFlags = GOEP_NEW_CONN;
        g_map_cntx.servers[idx].sApp[i].connId = 0;
        g_map_cntx.servers[idx].sApp[i].callback = MapGoepServerCallback;
        g_map_cntx.servers[idx].sApp[i].cm_conn_id = 0xFFFF;
#if OBEX_SERVER_CONS_SIZE > 0
        g_map_cntx.servers[idx].sApp[i].numTargets = 1;
        g_map_cntx.servers[idx].sApp[i].target[0] = &g_map_cntx.servers[idx].mapConn[i].obexConn;
#endif

        status = GOEP_RegisterServer(&g_map_cntx.servers[idx].sApp[i], &maps_store_func_table);
        if (status != OB_STATUS_SUCCESS)
        {
            goto Error;
        }

#if BT_SECURITY == XA_ENABLED
        if (server->mass.security != BSL_NO_SECURITY) 
        {
            status = GOEP_RegisterServerSecurityRecord(&g_map_cntx.servers[idx].sApp[i], server->mass.security);
            ASSERT(status == OB_STATUS_SUCCESS);
        }
#endif
    }
    ObexSdpDB_Registration(SC_MAP_SERVER);
    g_map_cntx.servers[idx].state = MAP_APP_STATE_REGISTERED;
    
Error:
#if OBEX_DEINIT_FUNCS == XA_ENABLED
    if (status != OB_STATUS_SUCCESS) 
    {
        int j;
        for (j = 0; j <= i; j++)
        {
            GOEP_DeregisterServer(&g_map_cntx.servers[idx].sApp[j]);
            g_map_cntx.servers[idx].state = MAP_APP_STATE_NULL;
        }
    }
#else
    ASSERT(status == OB_STATUS_SUCCESS);
#endif

    OS_UnlockObex();
    return status;
}


BtStatus maps_deregister_server(U8 instance)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    MapServer *server = maps_get_server_instance(instance);
    ObStatus status = OB_STATUS_FAILED;
    int i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (g_map_cntx.initialized != TRUE || server == NULL)
    {
        bt_trace(TRACE_GROUP_9, BTLOG_MAPS_DEREGISTER_SERVER_CHECK_FAILED, g_map_cntx.initialized);
        status = OB_STATUS_SUCCESS;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    bt_trace(TRACE_GROUP_9, BTLOG_MAPS_DEREGISTER_SERVER, instance);
    server->state = MAP_APP_STATE_DEREGISTERING;
    ObexSdpDB_DeRegistration(SC_MAP_SERVER);
    for (i = 0; i < NUM_MAP_SRV_CONN; i++)
    {
        status = GOEP_DeregisterServer(&server->sApp[i]);
        if (status == OB_STATUS_SUCCESS)
        {
#if BT_SECURITY == XA_ENABLED
            if (server->appInfo.mass.security != BSL_NO_SECURITY)
            {
                GOEP_UnregisterServerSecurityRecord(&server->sApp[i]);
            }
#endif
            server->state = MAP_APP_STATE_NULL;
        }
    }

    OS_MemSet((U8 *)server, 0x00, sizeof(MapServer));
Error:
    OS_UnlockObex();
    return status;
}


BtStatus maps_authorize_rsp(BD_ADDR *bdAddr, U8 rsp_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    MapServer *server;
    ObStatus status = OB_STATUS_FAILED;
    U8 i, j;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

    for (i = 0; i < NUM_MAP_SRVS; i++)
    {
        server = &g_map_cntx.servers[i];
        for (j = 0; j < NUM_MAP_SRV_CONN; j++)
        {
            if (server->mapConn[j].state == MAP_CONN_STATE_AUTHORIZING && 
                AreBdAddrsEqual(bdAddr, &server->mapConn[j].bdAddr))
            {
                server->sApp[j].connId = (U8)server->mapConn[j].goep_conn_id;
                GOEP_ServerAuthorizeResponse(&server->sApp[j], rsp_code);
                status = OB_STATUS_SUCCESS;
                break;
            }
        }
    }

    OS_UnlockObex();
    return status;
}


BtStatus maps_connect_rsp(U8 connId, BD_ADDR *bdAddr, BOOL accept)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    MapServer *server = maps_get_server_instance_by_goep_id(connId);
    GoepServerApp *sApp = maps_get_server_app_by_goep_id(connId);
    ObStatus status = OB_STATUS_SUCCESS;
    U8 i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (server == NULL)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }

    if (server->state != MAP_APP_STATE_REGISTERED)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }
#endif

    if (accept == TRUE)
    {
        for (i = 0; i < NUM_MAP_SRV_CONN; i++)
        {
            if (server->mapConn[i].state == MAP_CONN_STATE_CONNECTING && server->mapConn[i].goep_conn_id == connId)
            {
                server->mapConn[i].state = MAP_CONN_STATE_CONNECTED;
                break;
            }
        }
    }
    else
    {
        GOEP_ServerAbort(sApp, OBRC_FORBIDDEN);
    }		
    GOEP_ServerContinue(sApp);

Error:    
    OS_UnlockObex();
    return status;
}


BtStatus maps_disconnect(U8 connId)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
//    GoepServerApp *sApp = maps_get_server_app_by_conn(connId);
//    MapConn *conn = maps_get_server_conn_by_conn_id(connId);
    GoepServerApp *sApp = maps_get_server_app_by_goep_id(connId);
    MapConn *conn = maps_get_server_conn_by_goep_id(connId);
    ObStatus status = OB_STATUS_SUCCESS;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

    if (sApp && conn)
    {
        sApp->connId = (U8)conn->goep_conn_id;
        status = GOEP_ServerTpDisconnect(sApp);
    }

    OS_UnlockObex();
    return status;
}

BtStatus maps_abort(U8 connId, U8 rsp_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
//    GoepServerApp *sApp = maps_get_server_app_by_conn(connId);
//    MapConn *conn = maps_get_server_conn_by_conn_id(connId);
    GoepServerApp *sApp = maps_get_server_app_by_goep_id(connId);
    MapConn *conn = maps_get_server_conn_by_goep_id(connId);
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex(); 

#if XA_ERROR_CHECK == XA_ENABLED
    if (!sApp) 
    {
        goto Error;
    }
#endif
    if (sApp && conn)
    {
        switch (conn->state)
        {
        case MAP_CONN_STATE_CONNECTED:
            sApp->connId = (U8)conn->goep_conn_id;
            status = GOEP_ServerAbort(sApp, rsp_code);
            GOEP_ServerContinue(sApp);
            break;
        default:
            break;
        }
    }   
Error:
    OS_UnlockObex();
    return status;
}

BtStatus maps_set_folder_response(U8 connId, U8 rsp_code)
{
    return maps_send_response(connId, rsp_code);
}


BtStatus maps_get_folder_listing_response(U8 connId, U8 rsp_code, map_obj_struct *obj)
{
    GoepServerApp *sApp = maps_get_server_app_by_goep_id(connId);
    U8  *ap = g_map_cntx.appParms ;

    ap[0] = MAPAPTAG_FOLDER_LISTING_SIZE;
    ap[1] = 2;
    StoreBE16(ap + 2, obj->u.folder_list.list_count);
    if (GOEP_ServerQueueHeader(sApp, OBEXH_APP_PARAMS, ap, 4) == FALSE) 
    {
        GOEP_ServerAbort(sApp, OBRC_INTERNAL_SERVER_ERR);
        return OB_STATUS_FAILED;
    }
    return maps_pull_response(connId, rsp_code, obj);
}


BtStatus maps_get_message_listing_response(U8 connId, U8 rsp_code, map_obj_struct *obj)
{
    GoepServerApp *sApp = maps_get_server_app_by_goep_id(connId);
    U8 *ap = g_map_cntx.appParms ;
    U16 len;

    if (rsp_code == OBRC_SUCCESS)
    {
        U16 i = 0;
        ap[i++] = MAPAPTAG_NEW_MESSAGE;
        ap[i++] = 1;
        ap[i++] = obj->u.message_list.new_message;
        ap[i++] = MAPAPTAG_MSE_TIME;
        len = OS_StrLen((char *)obj->u.message_list.mse_time);
        ap[i++] = len + 1;
        OS_StrnCpy((char *)&ap[i], (char *)obj->u.message_list.mse_time, len);
        i += len;
        ap[i++] = '\0';
        ap[i++] = MAPAPTAG_MESSAGE_LISTING_SIZE;
        ap[i++] = 2;
        StoreBE16(ap + i, obj->u.message_list.list_count);
        i += 2;

        if (GOEP_ServerQueueHeader(sApp, OBEXH_APP_PARAMS, ap, i) == FALSE) 
        {
            GOEP_ServerAbort(sApp, OBRC_INTERNAL_SERVER_ERR);
            return OB_STATUS_FAILED;
        }
    }
    return maps_pull_response(connId, rsp_code, obj);
}


BtStatus maps_get_message_response(U8 connId, U8 rsp_code, map_obj_struct *obj)
{
    GoepServerApp *sApp = maps_get_server_app_by_goep_id(connId);
    U8 *ap = g_map_cntx.appParms ;

    if (rsp_code == OBRC_SUCCESS)
    {
        U16 i = 0;
        ap[i++] = MAPAPTAG_FRACTION_DELIVER;
        ap[i++] = 1;
        ap[i++] = obj->u.message.fraction_deliver;

        if (GOEP_ServerQueueHeader(sApp, OBEXH_APP_PARAMS, ap, i) == FALSE) 
        {
            GOEP_ServerAbort(sApp, OBRC_INTERNAL_SERVER_ERR);
            return OB_STATUS_FAILED;
        }
    }
    return maps_pull_response(connId, rsp_code, obj);
}


BtStatus maps_set_notification_registration_response(U8 connId, U8 rsp_code)
{
    return maps_send_response(connId, rsp_code);
}


BtStatus maps_set_message_status_response(U8 connId, U8 rsp_code)
{
    return maps_send_response(connId, rsp_code);
}


BtStatus maps_push_message_response(U8 connId, U8 rsp_code, U16 *handle, U16 len)
{
    GoepServerApp *sApp = maps_get_server_app_by_goep_id(connId);
    if (GOEP_ServerQueueHeader(sApp, OBEXH_NAME, (U8 *)handle, len) == FALSE) 
    {
        GOEP_ServerAbort(sApp, OBRC_INTERNAL_SERVER_ERR);
        return OB_STATUS_FAILED;
    }
    return maps_send_response(connId, rsp_code);
}


BtStatus maps_update_inbox_response(U8 connId, U8 rsp_code)
{
    return maps_send_response(connId, rsp_code);
}


#ifdef __BT_MAP_MNS_SUPPORT__
ObexRespCode maps_mns_obs_read(void *obs, U8* buff, U16 len)
{
    map_obj_struct *push_pull_obj = (map_obj_struct *) obs;
    MapClient *client = maps_get_mns_client_by_goep_id(push_pull_obj->conn_id);
    map_server_app_callback callback = client->appInfo.mnsc.callback;
    maps_callback_parm_struct *parm = &client->mapConn.cbParm.sParm;

    bt_trace(TRACE_GROUP_9, BTLOG_MAPS_MNS_OBS_READ, len, push_pull_obj->offset);
    parm->event = MAPS_EVENT_DATA_REQ;
    parm->u.push_pull_obj->buff = buff;
    parm->u.push_pull_obj->buff_len = len;
    callback(parm);
    if (parm->u.push_pull_obj->buff_len == len)
    {
        push_pull_obj->offset += len;
        return OBRC_SUCCESS;
    }

    else
    {
        return OBRC_FORBIDDEN;
    }
}

static U32 maps_mns_obs_get_object_len(void *obs)
{
    map_obj_struct *obj = (map_obj_struct *) obs;
    return (obj ? obj->total_len : 0);
}

BtStatus maps_mns_register_client(map_registration_struct *mns_client)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_FAILED;
    int i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
    
    OS_MemSet((U8 *)&g_map_cntx.mnsClient, 0, sizeof(MapClient) * NUM_MAP_SRV_CONN);
    for (i = 0; i < NUM_MAP_SRV_CONN; i++)
    {
        OS_MemCopy((U8 *)&g_map_cntx.mnsClient[i].appInfo, (U8 *)mns_client, sizeof(map_registration_struct));
        g_map_cntx.mnsClient[i].cApp.type = g_map_mns_bt_goep_service[i];
        g_map_cntx.mnsClient[i].cApp.callback = (GoepClientCallback) MapMnsGoepClientCallback;
        g_map_cntx.mnsClient[i].cApp.connFlags = GOEP_NEW_CONN;
        g_map_cntx.mnsClient[i].cApp.connId = 0;
        g_map_cntx.mnsClient[i].mapConn.state = MAP_CONN_STATE_NULL;
        status = GOEP_RegisterClient(&g_map_cntx.mnsClient[i].cApp, &maps_mns_store_func_table);
        
	OS_Report("maps_mns_register_client GOEP_RegisterClient status:%d", status);
		
        if (status == OB_STATUS_SUCCESS)
        {
            g_map_cntx.mnsClient[i].state = MAP_APP_STATE_REGISTERED;
        }
    }

    OS_UnlockObex();
    return status;
}


BtStatus maps_mns_deregister_client(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    int i;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
    
#if OBEX_DEINIT_FUNCS == XA_ENABLED
    for (i = 0; i < NUM_MAP_SRV_CONN; i++)
    {
        status = GOEP_DeregisterClient(&g_map_cntx.mnsClient[i].cApp);
        OS_MemSet((U8 *)&g_map_cntx.mnsClient[i], 0, sizeof(MapClient));
    }
#endif

    OS_UnlockObex();
    return status;
}


BtStatus maps_mns_connect(BD_ADDR *remote_addr, U8 obex_chal_option, U8 *obex_chal_nonce, U8 *obex_chal_realm)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    ObexTpAddr addr;
    MapClient *new_entry = NULL;
    GoepConnectReq *connect_req;
    int i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

    for (i = 0; i < NUM_MAP_SRV_CONN; i++)
    {
        if(g_map_cntx.mnsClient[i].mapConn.state > MAP_CONN_STATE_NULL)
        {
            if (AreBdAddrsEqual(remote_addr, &g_map_cntx.mnsClient[i].mapConn.bdAddr))
            {
                status = OB_STATUS_BUSY;
                goto Error;
            }
        }
        else if (new_entry == NULL)
        {
            new_entry = &g_map_cntx.mnsClient[i];
        }
    }

    OS_MemCopy(new_entry->mapConn.bdAddr.addr, remote_addr->addr, BD_ADDR_SIZE);
    new_entry->chal.options = obex_chal_option;
    OS_MemCopy(new_entry->chalNonce, obex_chal_nonce, AUTH_NONCE_LEN);
    OS_MemCopy(new_entry->chalRealm, obex_chal_realm, MAP_MAX_REALM_LEN);
    new_entry->chal.challenge = new_entry->chalNonce;
    new_entry->chal.challengeLen = AUTH_NONCE_LEN;
    new_entry->chal.realm = new_entry->chalRealm;
    new_entry->chal.realmLen = (U8)OS_StrLen((char *)new_entry->chal.realm);
    new_entry->mapConn.state = MAP_CONN_STATE_CONNECTING;

    connect_req = &new_entry->cApp.connect_req;
    OS_MemSet((U8 *) connect_req, 0, sizeof(GoepConnectReq));
    connect_req->target = g_map_remote_mns_UUID;
    connect_req->targetLen = 16;
    connect_req->mru = MAP_MRU_SIZE;
	//MAP does not need OBEX authenticaqtion 
 /*   connect_req->challenge = &new_entry->chal;
    */
    
    addr.type = OBEX_TP_BLUETOOTH;
    addr.proto.bt.uuid = SC_MAP_MNS_SERVER;
    OS_MemCopy(addr.proto.bt.addr.addr, remote_addr->addr, sizeof(BD_ADDR));
    status = GOEP_TpConnect(&new_entry->cApp, &addr);

    if (status == OB_STATUS_PENDING)
    {
    }
    else if (status == OB_STATUS_SUCCESS)
    {
    	/* TP connected, build OBEX connect */
        status = GOEP_Connect(&new_entry->cApp, connect_req);

        if (status != OB_STATUS_PENDING)
        {
            GOEP_TpDisconnect(&new_entry->cApp);
        }
    }

Error:
    OS_UnlockObex();
    return status;
}


BtStatus maps_mns_disconnect(U8 connId)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    MapClient *client = maps_get_mns_client_by_goep_id(connId);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
    
    if(client->mapConn.state == MAP_CONN_STATE_CONNECTING)
    {
        status = GOEP_TpDisconnect(&client->cApp);
        if(status == OB_STATUS_PENDING)
        {
            client->mapConn.state = MAP_CONN_STATE_DISCONNECTING;
        }
    }
    else
    {
        status = GOEP_Disconnect(&client->cApp);

        if (status == OB_STATUS_SUCCESS)
        {
            client->mapConn.state = MAP_CONN_STATE_NULL;
        }
        else if (status == OB_STATUS_PENDING)
        {
            client->mapConn.state = MAP_CONN_STATE_DISCONNECTING;
        }
        else
        {
            status = GOEP_TpDisconnect(&client->cApp);
            if(status == OB_STATUS_PENDING)
            {
                client->mapConn.state = MAP_CONN_STATE_DISCONNECTING;
            }
        }
    }

    OS_UnlockObex();
    return status;
}


BtStatus maps_mns_send_event(U8 connId, map_obj_struct *obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    MapClient *client = maps_get_mns_client_by_goep_id(connId);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (client == NULL)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }
    if (client->mapConn.state < MAP_CONN_STATE_CONNECTED)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }
#endif
    client->pushPull.name = NULL;
    client->pushPull.type = (U8 *)MAP_MIME_EVENT_REPORT;
	client->pushPull.objectLen = maps_obs_get_object_len(obj);
    client->pushPull.object = obj;
    g_map_cntx.appParms[0] = MAPAPTAG_MAS_INSTANCE_ID;
    g_map_cntx.appParms[1] = 1;
    g_map_cntx.appParms[2] = obj->u.event_report.mas_id;
    if (GOEP_ClientQueueHeader(&client->cApp, OBEXH_APP_PARAMS, g_map_cntx.appParms, 3) == FALSE)
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }
    
    client->cApp.connId = client->mapConn.goep_conn_id;
    status = GOEP_Push(&client->cApp, &client->pushPull);

Error:
    OS_UnlockObex();
    return status;
}


#endif
#endif

#ifdef __BT_MAPC_PROFILE__
MapClient *mapc_get_client_by_handler(GoepClientApp *handler)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (&g_map_cntx.client.cApp == handler)
    {
        return &g_map_cntx.client;
    }
    return NULL;
}

ObexRespCode mapc_obs_read(void *obs, U8* buff, U16 len)
{
    map_obj_struct *push_pull_obj = (map_obj_struct *) obs;
    map_client_app_callback callback = g_map_cntx.client.appInfo.masc.callback;
    mapc_callback_parm_struct *parm = &g_map_cntx.client.mapConn.cbParm.cParm;

    bt_trace(TRACE_GROUP_9, BTLOG_MAPC_OBS_READ, len, push_pull_obj->offset);
    parm->event = MAPC_EVENT_DATA_REQ;
    parm->u.push_pull_obj->buff = buff;
    parm->u.push_pull_obj->buff_len = len;
    callback(parm);
    if (parm->u.push_pull_obj->buff_len == len)
    {
        push_pull_obj->offset += len;
        return OBRC_SUCCESS;
    }
    else
    {
        return OBRC_FORBIDDEN;
    }
}

ObexRespCode mapc_obs_write(void *obs, U8* buff, U16 len)
{
    map_obj_struct *push_pull_obj = (map_obj_struct *) obs;
    map_client_app_callback callback = g_map_cntx.client.appInfo.masc.callback;
    mapc_callback_parm_struct *parm = &g_map_cntx.client.mapConn.cbParm.cParm;

    if (push_pull_obj->type == MAP_OBJ_NOTIF_REGISTRATION ||
        push_pull_obj->type == MAP_OBJ_INBOX_UPDATE ||
        push_pull_obj->type == MAP_OBJ_MESSAGE_STATUS)
    {
        return OBRC_SUCCESS;
    }
    else
    {
        bt_trace(TRACE_GROUP_9, BTLOG_MAPC_OBS_WRITE, len, push_pull_obj->offset);
        parm->event = MAPC_EVENT_DATA_IND;
        parm->u.push_pull_obj->buff = buff;
        parm->u.push_pull_obj->buff_len = len;
        callback(parm);
        if (parm->u.push_pull_obj->buff_len == len)
        {
            push_pull_obj->offset += len;
            push_pull_obj->total_len += len;
            return OBRC_SUCCESS;
        }
        else
        {
            return OBRC_FORBIDDEN;
        }
    }
}

static U32 mapc_obs_get_object_len(void *obs)
{
    map_obj_struct *push_pull_obj = (map_obj_struct *) obs;

    bt_trace(TRACE_GROUP_9, BTLOG_MAPC_OBS_GET_OBJECT_LEN, push_pull_obj->total_len);
    return (push_pull_obj ? push_pull_obj->total_len : 0);
}

BtStatus mapc_register_client(map_registration_struct *client)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_FAILED;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
    
    OS_MemSet((U8 *)&g_map_cntx.client, 0, sizeof(MapClient));
    OS_MemCopy((U8 *)&g_map_cntx.client.appInfo, (U8 *)client, sizeof(map_registration_struct));
    g_map_cntx.client.cApp.type = GOEP_PROFILE_MAP_CLIENT;
    g_map_cntx.client.cApp.callback = (GoepClientCallback) MapGoepClientCallback;
    g_map_cntx.client.cApp.connFlags = GOEP_NEW_CONN;
    g_map_cntx.client.cApp.connId = 0;
    g_map_cntx.client.state = MAP_APP_STATE_REGISTERING;
    g_map_cntx.client.mapConn.state = MAP_CONN_STATE_NULL;

    status = GOEP_RegisterClient(&g_map_cntx.client.cApp, &mapc_store_func_table);
    if (status == OB_STATUS_SUCCESS)
    {
        g_map_cntx.client.state = MAP_APP_STATE_REGISTERED;
    }

    OS_UnlockObex();
    return status;
}


BtStatus mapc_deregister_client(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
    
#if OBEX_DEINIT_FUNCS == XA_ENABLED
    status = GOEP_DeregisterClient(&g_map_cntx.client.cApp);
    OS_MemSet((U8 *)&g_map_cntx.client, 0, sizeof(MapClient));
    g_map_cntx.client.state = MAP_APP_STATE_NULL;
#endif

    OS_UnlockObex();
    return status;
}


BtStatus mapc_connect(BD_ADDR *remote_addr, U8 obex_chal_option, U8 *obex_chal_nonce, U8 *obex_chal_realm)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    ObexTpAddr addr;
    MapClient *client = &g_map_cntx.client;
    GoepConnectReq *connect_req;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
    
    OS_MemCopy(client->mapConn.bdAddr.addr, remote_addr->addr, BD_ADDR_SIZE);
    client->chal.options = obex_chal_option;
    OS_MemCopy(client->chalNonce, obex_chal_nonce, AUTH_NONCE_LEN);
    OS_MemCopy(client->chalRealm, obex_chal_realm, MAP_MAX_REALM_LEN);
    client->chal.challenge = client->chalNonce;
    client->chal.challengeLen = AUTH_NONCE_LEN;
    client->chal.realm = client->chalRealm;
    client->chal.realmLen = (U8) OS_StrLen((char *)client->chal.realm);
    client->mapConn.state = MAP_CONN_STATE_CONNECTING;

    connect_req = &client->cApp.connect_req;
    OS_MemSet((U8 *) connect_req, 0, sizeof(GoepConnectReq));
    connect_req->target = g_map_remote_mas_UUID;
    connect_req->targetLen = 16;
    connect_req->mru = MAP_MRU_SIZE;
    connect_req->challenge = &client->chal;
    
    addr.type = OBEX_TP_BLUETOOTH;
    addr.proto.bt.uuid = SC_MAP_SERVER;
    OS_MemCopy((U8 *) &(addr.proto.bt.addr), (U8 *) remote_addr, sizeof(BD_ADDR));
    status = GOEP_TpConnect(&client->cApp, &addr);
    if (status == OB_STATUS_PENDING)
    {
    }
    else if (status == OB_STATUS_SUCCESS)
    {
    	/* TP connected, build OBEX connect */
        status = GOEP_Connect(&client->cApp, connect_req);

        if (status != OB_STATUS_PENDING)
        {
            GOEP_TpDisconnect(&client->cApp);
        }
    }

    OS_UnlockObex();
    return status;
}


BtStatus mapc_disconnect(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    MapClient *client = &g_map_cntx.client;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();
    
    if(client->mapConn.state == MAP_CONN_STATE_CONNECTING)
    {
        status = GOEP_TpDisconnect(&client->cApp);
        if(status == OB_STATUS_PENDING)
        {
            client->mapConn.state = MAP_CONN_STATE_DISCONNECTING;
        }
    }
    else
    {
        status = GOEP_Disconnect(&client->cApp);

        if (status == OB_STATUS_SUCCESS)
        {
            client->mapConn.state = MAP_CONN_STATE_NULL;
        }
        else if (status == OB_STATUS_PENDING)
        {
            client->mapConn.state = MAP_CONN_STATE_DISCONNECTING;
        }
        else
        {
            status = GOEP_TpDisconnect(&client->cApp);
            if(status == OB_STATUS_PENDING)
            {
                client->mapConn.state = MAP_CONN_STATE_DISCONNECTING;
            }
        }
    }

    OS_UnlockObex();
    return status;
}


BtStatus mapc_set_notification_registration(map_obj_struct *obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    MapClient *client = &g_map_cntx.client;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (client->mapConn.state < MAP_CONN_STATE_CONNECTED)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }
#endif
    client->pushPull.name = NULL;
    client->pushPull.type = MAP_MIME_NOTIF_REGISTRATION;
    client->pushPull.object = obj;
    client->pushPull.objectLen = obj->total_len;
    g_map_cntx.appParms[0] = MAPAPTAG_NOTIFICATION_STATUS;
    g_map_cntx.appParms[1] = 1;
    g_map_cntx.appParms[2] = obj->u.notif_registration.enable;
    if (GOEP_ClientQueueHeader(&client->cApp, OBEXH_APP_PARAMS, g_map_cntx.appParms, 3) == FALSE)
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }
    
    status = GOEP_Push(&client->cApp, &client->pushPull);

Error:
    OS_UnlockObex();
    return status;
}


BtStatus mapc_set_message_status(map_obj_struct *obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    MapClient *client = &g_map_cntx.client;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (client->mapConn.state < MAP_CONN_STATE_CONNECTED)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }
#endif
    client->pushPull.name = NULL;  /* UTF-16BE */
    client->pushPull.type = MAP_MIME_MESSAGE_STATUS;
    client->pushPull.buf_ptr = &g_map_filler_byte;
    client->pushPull.buf_len = obj->total_len;
    client->pushPull.objectLen = obj->total_len;
    client->pushPull.object = obj;
    client->mapConn.cbParm.cParm.u.push_pull_obj = obj;
    g_map_cntx.appParms[0] = MAPAPTAG_STATUS_INDICATOR;
    g_map_cntx.appParms[1] = 1;
    g_map_cntx.appParms[2] = obj->u.msg_status.type;
    g_map_cntx.appParms[3] = MAPAPTAG_STATUS_VALUE;
    g_map_cntx.appParms[4] = 1;
    g_map_cntx.appParms[5] = obj->u.msg_status.enable;
    if (GOEP_ClientQueueHeader(&client->cApp, OBEXH_APP_PARAMS, g_map_cntx.appParms, 6) == FALSE)
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }
    
    status = GOEP_Push(&client->cApp, &client->pushPull);

Error:
    OS_UnlockObex();
    return status;
}


BtStatus mapc_push_message(map_obj_struct *obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    MapClient *client = &g_map_cntx.client;
    U16 i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (client->mapConn.state < MAP_CONN_STATE_CONNECTED)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }
#endif
    client->pushPull.name = (GoepUniType *)obj->u.put_message.folder;
    client->pushPull.type = MAP_MIME_MESSAGE;
    client->pushPull.buf_len = obj->total_len;
    client->pushPull.objectLen = obj->total_len;
    client->pushPull.object = obj;
    client->mapConn.cbParm.cParm.u.push_pull_obj = obj;
    g_map_cntx.appParms[i++] = MAPAPTAG_TRANSPARENT;
    g_map_cntx.appParms[i++] = 1;
    g_map_cntx.appParms[i++] = obj->u.put_message.transparent;
    g_map_cntx.appParms[i++] = MAPAPTAG_RETRY;
    g_map_cntx.appParms[i++] = 1;
    g_map_cntx.appParms[i++] = obj->u.put_message.retry;
    g_map_cntx.appParms[i++] = MAPAPTAG_CHARSET;
    g_map_cntx.appParms[i++] = 1;
    g_map_cntx.appParms[i++] = obj->u.put_message.charset;

    ASSERT (i <= MAP_APP_PARM_SIZE);
    if (GOEP_ClientQueueHeader(&client->cApp, OBEXH_APP_PARAMS, g_map_cntx.appParms, i) == FALSE)
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }
    
    status = GOEP_Push(&client->cApp, &client->pushPull);
Error:
    OS_UnlockObex();
    return status;
}


BtStatus mapc_update_inbox(map_obj_struct *obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    MapClient *client = &g_map_cntx.client;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (client->mapConn.state < MAP_CONN_STATE_CONNECTED)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }
#endif
    client->pushPull.name = NULL;  /* UTF-16BE */
    client->pushPull.type = MAP_MIME_MESSAGE_UPDATE;
    client->pushPull.buf_ptr = &g_map_filler_byte;
    client->pushPull.buf_len = 1;
    client->pushPull.objectLen = 1;
    client->pushPull.object = obj;
    status = GOEP_Push(&client->cApp, &client->pushPull);

Error:
    OS_UnlockObex();
    return status;
}


BtStatus mapc_get_folder_listing(map_obj_struct *obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    MapClient *client = &g_map_cntx.client;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (client->mapConn.state < MAP_CONN_STATE_CONNECTED)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }
#endif
    client->pushPull.name = NULL;
    client->pushPull.type = MAP_MIME_FOLDER_LISTING;
    client->pushPull.object = obj;
    client->mapConn.cbParm.cParm.u.push_pull_obj = obj;
    g_map_cntx.appParms[0] = MAPAPTAG_MAX_LIST_COUNT;
    g_map_cntx.appParms[1] = 2;
    StoreBE16(&g_map_cntx.appParms[2], obj->u.folder_list.max_list_count);
    g_map_cntx.appParms[4] = MAPAPTAG_START_OFFSET;
    g_map_cntx.appParms[5] = 2;
    StoreBE16(&g_map_cntx.appParms[6], obj->u.folder_list.list_start_offset);
    if (GOEP_ClientQueueHeader(&client->cApp, OBEXH_APP_PARAMS, g_map_cntx.appParms, 8) == FALSE)
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }
    
    status = GOEP_Pull(&client->cApp, &client->pushPull, FALSE);

Error:
    OS_UnlockObex();
    return status;
}


BtStatus mapc_get_message_listing(map_obj_struct *obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    MapClient *client = &g_map_cntx.client;
    U16 i = 0;
    U8 len = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (client->mapConn.state < MAP_CONN_STATE_CONNECTED)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }
#endif
    client->pushPull.name = (GoepUniType *)obj->u.message_list.folder;  /* UTF-16BE */
    client->pushPull.type = MAP_MIME_MESSAGE_LISTING;
    client->pushPull.object = obj;
    client->mapConn.cbParm.cParm.u.push_pull_obj = obj;
    g_map_cntx.appParms[i++] = MAPAPTAG_MAX_LIST_COUNT;
    g_map_cntx.appParms[i++] = 2;
    StoreBE16(&g_map_cntx.appParms[i], obj->u.message_list.max_list_count);
    i += 2;
    if (obj->u.message_list.list_start_offset != 0)
    {
        g_map_cntx.appParms[i++] = MAPAPTAG_START_OFFSET;
        g_map_cntx.appParms[i++] = 2;
        StoreBE16(&g_map_cntx.appParms[i], obj->u.message_list.list_start_offset);
        i += 2;
    }
    g_map_cntx.appParms[i++] = MAPAPTAG_SUBJECT_LENGTH;
    g_map_cntx.appParms[i++] = 1;
    g_map_cntx.appParms[i++] = obj->u.message_list.subject_length;
    if (obj->u.message_list.parameter_mask != 0)
    {
        g_map_cntx.appParms[i++] = MAPAPTAG_PARAMETER_MASK;
        g_map_cntx.appParms[i++] = 4;
        StoreBE32(&g_map_cntx.appParms[i], obj->u.message_list.parameter_mask);
        i += 4;
    }

    /* Filters */
    if (obj->u.message_list.filter_message_type != 0)
    {
        g_map_cntx.appParms[i++] = MAPAPTAG_FILTER_MESSAGE_TYPE;
        g_map_cntx.appParms[i++] = 1;
        g_map_cntx.appParms[i++] = obj->u.message_list.filter_message_type;
    }
    if ((len = (U8)OS_StrLen((char *)obj->u.message_list.filter_period_begin)) != 0)
    {
        g_map_cntx.appParms[i++] = MAPAPTAG_FILTER_PERIOD_BEGIN;
        g_map_cntx.appParms[i++] = len;
        OS_MemCopy(&g_map_cntx.appParms[i], obj->u.message_list.filter_period_begin, len);
        i += len;
    }
    if ((len = (U8)OS_StrLen((char *)obj->u.message_list.filter_period_end)) != 0)
    {
        g_map_cntx.appParms[i++] = MAPAPTAG_FILTER_PERIOD_END;
        g_map_cntx.appParms[i++] = len;
        OS_MemCopy(&g_map_cntx.appParms[i], obj->u.message_list.filter_period_end, len);
        i += len;
    }
    if (obj->u.message_list.filter_read_status != 0)
    {
        g_map_cntx.appParms[i++] = MAPAPTAG_FILTER_READ_STATUS;
        g_map_cntx.appParms[i++] = 1;
        g_map_cntx.appParms[i++] = obj->u.message_list.filter_read_status;
    }
    if ((len = (U8)OS_StrLen(obj->u.message_list.filter_recipient)) != 0)
    {
        g_map_cntx.appParms[i++] = MAPAPTAG_FILTER_RECIPIENT;
        g_map_cntx.appParms[i++] = len;
        OS_MemCopy(&g_map_cntx.appParms[i], obj->u.message_list.filter_recipient, len);
        i += len;
    }
    if ((len = (U8)OS_StrLen(obj->u.message_list.filter_originator)) != 0)
    {
        g_map_cntx.appParms[i++] = MAPAPTAG_FILTER_ORIGINATOR;
        g_map_cntx.appParms[i++] = len;
        OS_MemCopy(&g_map_cntx.appParms[i], obj->u.message_list.filter_originator, len);
        i += len;
    }
    if (obj->u.message_list.filter_priority != 0)
    {
        g_map_cntx.appParms[i++] = MAPAPTAG_FILTER_PRIORITY;
        g_map_cntx.appParms[i++] = 1;
        g_map_cntx.appParms[i++] = obj->u.message_list.filter_priority;
    }

    ASSERT (i <= MAP_APP_PARM_SIZE);
    if (GOEP_ClientQueueHeader(&client->cApp, OBEXH_APP_PARAMS, g_map_cntx.appParms, i) == FALSE)
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }
    
    status = GOEP_Pull(&client->cApp, &client->pushPull, FALSE);

Error:
    OS_UnlockObex();
    return status;
}


BtStatus mapc_get_message(map_obj_struct *obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    MapClient *client = &g_map_cntx.client;
    U16 i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (client->mapConn.state < MAP_CONN_STATE_CONNECTED)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }
#endif
    client->pushPull.name = (GoepUniType *)obj->u.message.handle; /* UTF-16BE */
    client->pushPull.type = MAP_MIME_MESSAGE;
    client->pushPull.object = obj;
    client->mapConn.cbParm.cParm.u.push_pull_obj = obj;
    g_map_cntx.appParms[i++] = MAPAPTAG_ATTACHMENT;
    g_map_cntx.appParms[i++] = 1;
    g_map_cntx.appParms[i++] = obj->u.message.attachment;
    g_map_cntx.appParms[i++] = MAPAPTAG_CHARSET;
    g_map_cntx.appParms[i++] = 1;
    g_map_cntx.appParms[i++] = obj->u.message.charset;
    if (obj->u.message.fraction_request != MAP_FRACTION_REQ_NO)
    {
        g_map_cntx.appParms[i++] = MAPAPTAG_FRACTION_REQUEST;
        g_map_cntx.appParms[i++] = 1;
        g_map_cntx.appParms[i++] = obj->u.message.fraction_request;
    }

    ASSERT (i <= MAP_APP_PARM_SIZE);
    if (GOEP_ClientQueueHeader(&client->cApp, OBEXH_APP_PARAMS, g_map_cntx.appParms, i) == FALSE)
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }
    
    status = GOEP_Pull(&client->cApp, &client->pushPull, FALSE);

Error:
    OS_UnlockObex();
    return status;
}


BtStatus mapc_set_folder(map_obj_struct *obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    GoepFolderReq req;
    MapClient *client = &g_map_cntx.client;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (client->mapConn.state < MAP_CONN_STATE_CONNECTED)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }
#endif

    OS_MemSet((U8 *)&req, 0x0, sizeof(GoepFolderReq));
    switch (obj->u.set_folder.op)
    {
    case MAP_FOLDER_ROOT:
        req.reset = TRUE;
        req.flags = OSPF_DONT_CREATE;
        break;
    case MAP_FOLDER_UP:
        req.reset = FALSE;
        req.flags = (OSPF_DONT_CREATE|OSPF_BACKUP);
        req.name = (GoepUniType *)obj->u.set_folder.folder;
        break;
    case MAP_FOLDER_DOWN:
        req.reset = FALSE;
        req.flags = OSPF_DONT_CREATE;
        req.name = (GoepUniType *)obj->u.set_folder.folder;
        break;
    default:
        ASSERT(0);
    }
    
    status = GOEP_SetFolder(&client->cApp, &req);

Error:
    OS_UnlockObex();
    return status;
}


BtStatus mapc_abort(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
    MapClient *client = &g_map_cntx.client;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

    status = GOEP_ClientAbort(&client->cApp);

    OS_UnlockObex();
    return status;
}


#ifdef __BT_MAP_MNS_SUPPORT__
extern SdpAttribute MapClientSdpAttributes[];

static ObexRespCode mapc_mns_obs_write(void *obs, U8* buff, U16 len)
{
    map_obj_struct *push_pull_obj = (map_obj_struct *) obs;
    MapServer *server = &g_map_cntx.mnsServer;
    map_client_app_callback callback = server->appInfo.mnss.callback;
    mapc_callback_parm_struct *parm = &server->mapConn[0].cbParm.cParm;

    bt_trace(TRACE_GROUP_9, BTLOG_MAPC_MNS_OBS_WRITE, len, push_pull_obj->offset);
    parm->event = MAPC_EVENT_DATA_IND;
    parm->u.push_pull_obj->buff = buff;
    parm->u.push_pull_obj->buff_len = len;
    callback(parm);
    if (parm->u.push_pull_obj->buff_len == len)
    {
        push_pull_obj->offset += len;
        return OBRC_SUCCESS;
    }

    else
    {
        return OBRC_FORBIDDEN;
    }
}

BtStatus mapc_mns_register_server(map_registration_struct *mns_server)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_FAILED;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

    OS_MemSet((U8 *)&g_map_cntx.mnsServer, 0, sizeof(MapServer));
    OS_MemCopy((U8 *)&g_map_cntx.mnsServer.appInfo, (U8 *)mns_server, sizeof(map_registration_struct));
    g_map_cntx.mnsServer.sdpRec = MapClientSdpAttributes;
    g_map_cntx.mnsServer.state = MAP_APP_STATE_REGISTERING;

    g_map_cntx.mnsServer.mapConn[0].obexConn.target = g_map_remote_mns_UUID;
    g_map_cntx.mnsServer.mapConn[0].obexConn.targetLen = 16;
    g_map_cntx.mnsServer.sApp[0].bt_service = GOEP_PROFILE_MAP_CLIENT;
    g_map_cntx.mnsServer.sApp[0].type = GOEP_PROFILE_MAP_CLIENT; /* We allow only one service to be registered in one OBEX channel */
    g_map_cntx.mnsServer.sApp[0].connFlags = GOEP_NEW_CONN;
    g_map_cntx.mnsServer.sApp[0].connId = 0;
    g_map_cntx.mnsServer.sApp[0].callback = MapMnsGoepServerCallback;
    g_map_cntx.mnsServer.sApp[0].cm_conn_id = 0xFFFF;
#if OBEX_SERVER_CONS_SIZE > 0
    g_map_cntx.mnsServer.sApp[0].numTargets = 1;
    g_map_cntx.mnsServer.sApp[0].target[0] = &g_map_cntx.mnsServer.mapConn[0].obexConn;
#endif

    status = GOEP_RegisterServer(&g_map_cntx.mnsServer.sApp[0], &mapc_mns_store_func_table);
    if (status != OB_STATUS_SUCCESS)
    {
        goto Error;
    }

#if BT_SECURITY == XA_ENABLED
    if (mns_server->mnss.security != BSL_NO_SECURITY) 
    {
        status = GOEP_RegisterServerSecurityRecord(&g_map_cntx.mnsServer.sApp[0], mns_server->mnss.security);
        ASSERT(status == OB_STATUS_SUCCESS);
    }
#endif
    ObexSdpDB_Registration(SC_MAP_MNS_SERVER);
    g_map_cntx.mnsServer.state = MAP_APP_STATE_REGISTERED;

#if OBEX_DEINIT_FUNCS == XA_ENABLED
    if (status != OB_STATUS_SUCCESS) {
        GOEP_DeregisterServer(&g_map_cntx.mnsServer.sApp[0]);
        g_map_cntx.mnsServer.state = MAP_APP_STATE_NULL;
        goto Error;
    }
#else
    ASSERT(status == OB_STATUS_SUCCESS);
#endif

Error:
    OS_UnlockObex();
    return status;
}


BtStatus mapc_mns_deregister_server(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    MapServer *server = &g_map_cntx.mnsServer;
    ObStatus status = OB_STATUS_FAILED;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (g_map_cntx.initialized != TRUE)
    {
        status = OB_STATUS_SUCCESS;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    server->state = MAP_APP_STATE_DEREGISTERING;
    ObexSdpDB_DeRegistration(SC_MAP_MNS_SERVER);
    status = GOEP_DeregisterServer(&server->sApp[0]);

    if (status == OB_STATUS_SUCCESS)
    {
#if BT_SECURITY == XA_ENABLED
        if (server->appInfo.mnss.security != BSL_NO_SECURITY)
        {
            GOEP_UnregisterServerSecurityRecord(&server->sApp[0]);
        }
#endif
        server->state = MAP_APP_STATE_NULL;
    }

    OS_MemSet((U8 *)server, 0x00, sizeof(MapServer));
Error:
    OS_UnlockObex();
    return status;
}


BtStatus mapc_mns_authorize_rsp(BD_ADDR *bdAddr, U8 rsp_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    MapServer *server = &g_map_cntx.mnsServer;
    ObStatus status = OB_STATUS_SUCCESS;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (server == NULL)
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }

    if (server->state != MAP_APP_STATE_REGISTERED)
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }

    if (AreBdAddrsEqual(bdAddr, &server->mapConn[0].bdAddr) == FALSE)
    {
        status = OB_STATUS_BUSY;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    if (server->mapConn[0].state == MAP_CONN_STATE_AUTHORIZING)
    {
        GOEP_ServerAuthorizeResponse(&server->sApp[0], rsp_code);
    }

Error:
    OS_UnlockObex();
    return status;
}

BtStatus mapc_mns_connect_rsp(U8 connId, BD_ADDR *bdAddr, BOOL accept)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    MapServer *server = &g_map_cntx.mnsServer;
    GoepServerApp *sApp = server->sApp;
    ObStatus status = OB_STATUS_SUCCESS;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (server == NULL)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }

    if (server->state != MAP_APP_STATE_REGISTERED)
    {
        status = OB_STATUS_NO_CONNECT;
        goto Error;
    }
#endif

    if (accept == TRUE)
    {
        if (server->mapConn[0].state == MAP_CONN_STATE_CONNECTING && server->mapConn[0].goep_conn_id == connId)
        {
            server->mapConn[0].state = MAP_CONN_STATE_CONNECTED;
        }
    }
    else
    {
        GOEP_ServerAbort(sApp, OBRC_FORBIDDEN);
    }		
    GOEP_ServerContinue(sApp);

Error:    
    OS_UnlockObex();
    return status;
}


BtStatus mapc_mns_disconnect(U32 connId)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    MapServer *server = &g_map_cntx.mnsServer;
    ObStatus status = OB_STATUS_SUCCESS;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (server == NULL)
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }

    if (server->state != MAP_APP_STATE_REGISTERED)
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }

    if (server->sApp[0].connId != connId)
    {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    status = GOEP_ServerTpDisconnect(&server->sApp[0]);

Error:
    OS_UnlockObex();
    return status;
}


BtStatus mapc_mns_send_event_response(U32 connId, U8 rsp_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    MapServer *server = &g_map_cntx.mnsServer;
    ObStatus status = OB_STATUS_SUCCESS;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_LockObex();

#if XA_ERROR_CHECK == XA_ENABLED
    if (server == NULL)
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }

    if (server->state != MAP_APP_STATE_REGISTERED)
    {
        status = OB_STATUS_FAILED;
        goto Error;
    }

    if (server->sApp[0].connId != connId)
    {
        status = OB_STATUS_INVALID_PARM;
        goto Error;
    }
#endif /* XA_ERROR_CHECK == XA_ENABLED */

    if(rsp_code != OBRC_SUCCESS)
    {
        GOEP_ServerAbort(&g_map_cntx.mnsServer.sApp[0], rsp_code);
    }
    GOEP_ServerContinue(&g_map_cntx.mnsServer.sApp[0]);

Error:
    OS_UnlockObex();
    return status;
}

#endif
#endif

BtStatus map_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
     OS_MemSet((U8 *)&g_map_cntx, 0x0, sizeof(MapContext));
	map_obs_init();
    g_map_cntx.initialized = TRUE;
    return status;
}

BtStatus map_deinit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = OB_STATUS_SUCCESS;
#ifdef __BT_MAPS_PROFILE__
    int i;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < NUM_MAP_MNS_SRVS; i++)
    {
        maps_deregister_server(g_map_cntx.servers[i].appInfo.mass.instance);
    }
  #ifdef __BT_MAP_MNS_SUPPORT__
    maps_mns_deregister_client();
  #endif
#endif
#ifdef __BT_MAPC_PROFILE__
    mapc_deregister_client();
  #ifdef __BT_MAP_MNS_SUPPORT__
    mapc_mns_deregister_server();
  #endif
#endif
    OS_MemSet((U8 *)&g_map_cntx, 0x0, sizeof(MapContext));
    return status;
}

#endif /* defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__) */
