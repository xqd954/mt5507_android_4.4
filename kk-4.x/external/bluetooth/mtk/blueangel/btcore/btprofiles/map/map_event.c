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
 *   map_event.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file contains goep event handler of Message Access Profile (MAP).
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

#include "map_adp.h"
#include "map.h"
#include "map_obs.h"

/*
 * Connect: GOEP_EVENT_START -> GOEP_EVENT_HEADER_RX* -> GOEP_EVENT_CONTINUE -> GOEP_EVENT_COMPLETE
 * Set folder: GOEP_EVENT_START -> GOEP_EVENT_CONTINUE -> GOEP_EVENT_PRECOMPLETE -> GOEP_EVENT_COMPLETE
 * Pull (Get): GOEP_EVENT_START -> GOEP_EVENT_HEADER_RX* -> GOEP_EVENT_PROVIDE_OBJECT -> GOEP_EVENT_CONTINUE* -> GOEP_EVENT_PRECOMPLETE -> GOEP_EVENT_COMPLETE
 */
#ifdef __BT_MAPS_PROFILE__
void MapGoepServerCallback(GoepServerEvent *event)
{
    MapServer *server;
    MapConn *conn;
    map_server_app_callback callback;
    maps_callback_parm_struct *parm, conn_parm;
    U8 max_filter_text_len = MAP_MAX_FILTER_TEXT_LEN;

    server = maps_get_server_instance_by_handler(event->handler);
    conn = maps_get_server_conn_by_goep_id(event->handler->connId);
    callback = server->appInfo.mass.callback;

    if (conn)
    {
        parm = &conn->cbParm.sParm;
    }
    else
    {
        OS_MemSet((U8 *)&conn_parm, 0x0, sizeof(maps_callback_parm_struct));
        parm = &conn_parm;
    }
    parm->mas_instance = server->appInfo.mass.instance;
    parm->goep_conn_id = event->handler->connId;
    bt_trace(TRACE_GROUP_9, BTLOG_MAPS_GOEP_SERVER_CALLBACK, event->event, event->oper, event->handler->connId);
    switch (event->event)
    {
	case GOEP_EVENT_START:
        switch (event->oper)
        {
            case GOEP_OPER_CONNECT:
            case GOEP_OPER_DISCONNECT:
            case GOEP_OPER_SETFOLDER:
                break;
            case GOEP_OPER_PULL:
            case GOEP_OPER_PUSH:
                parm->u.push_pull_obj = maps_obs_new(event->handler->connId); /* Use MAP conn ID */
                break;
            default:
                break;
        }
        break;
    case GOEP_EVENT_CONTINUE:
        switch (event->oper)
        {
            case GOEP_OPER_ABORT:
            case GOEP_OPER_DISCONNECT:
                GOEP_ServerContinue(event->handler);
                break;
            case GOEP_OPER_CONNECT:
                {
                    if (conn && conn->state == MAP_CONN_STATE_AUTHORIZING)
                    {
                        conn->state = MAP_CONN_STATE_CONNECTING;
                        OS_MemCopy(parm->remote_addr.addr, conn->bdAddr.addr, BD_ADDR_SIZE);
                        parm->mas_instance = server->appInfo.mass.instance;
                        parm->event = MAPS_EVENT_CONNECT;
                        callback(parm);
                        break;
                    }
                }
                break;
            case GOEP_OPER_PULL:
                if (parm->u.push_pull_obj)
                {
                    if (parm->u.push_pull_obj->offset < parm->u.push_pull_obj->total_len)
                    {
                        GOEP_ServerContinue(event->handler);  /* Must. so it read remains data */
                    }
                }
                break;
            case GOEP_OPER_PUSH:
                if (parm->u.push_pull_obj)
                {
                //if (parm->u.push_pull_obj->offset < parm->u.push_pull_obj->total_len)
                //    {
                //        GOEP_ServerContinue(event->handler);  /* Must. so it read remains data */
                //}

                    if (event->info.pushpull.finalBit)
                    {
                        if (parm->u.push_pull_obj->type == MAP_OBJ_MESSAGE)
                        {
                            OS_MemCopy((U8 *)parm->u.push_pull_obj->u.put_message.folder, (U8 *)event->info.pushpull.name, MAP_MAX_FOLDER_PATH_LEN);
                            parm->event = MAPS_EVENT_PUSH_MESSAGE_RECEIVED;
                            callback(parm);
                        } 
			else if (parm->u.push_pull_obj->type == MAP_OBJ_NOTIF_REGISTRATION)
			{
			    /*Notify upper layer that the notification is set or unset when final bit is received*/
                           parm->event = MAPS_EVENT_SET_NOTIF;
			   callback(parm);
			}
			else if (parm->u.push_pull_obj->type == MAP_OBJ_MESSAGE_STATUS) 
			{
                    parm->event = MAPS_EVENT_MESSAGE_STATUS;
                    callback(parm);
			}
                    }
		    else
		    {
			GOEP_ServerContinue(event->handler);
		    }
                }
                break;
            case GOEP_OPER_SETFOLDER:
                parm->event = MAPS_EVENT_SET_FOLDER;
                if (event->info.setfolder.flags & OSPF_BACKUP)
                {
				    parm->u.set_folder.op = MAP_FOLDER_UP;
                    parm->u.set_folder.folder = (U8 *)event->info.setfolder.name;
                }
                else if (event->info.setfolder.reset)
                {
                    parm->u.set_folder.op = MAP_FOLDER_ROOT;
                }
                else
                {
				    parm->u.set_folder.op = MAP_FOLDER_DOWN;
                    parm->u.set_folder.folder = (U8 *)event->info.setfolder.name;
                }
                callback(parm);
            default:
                break;
        }
        break;
#if OBEX_AUTHENTICATION == XA_ENABLED
    case GOEP_EVENT_AUTH_CHALLENGE:
        parm->event = MAPS_EVENT_AUTH_CHALLENGE;
        parm->u.chal.obex_chal_option = event->challenge.options;
        parm->u.chal.obex_chal_nonce = event->challenge.nonce;
        parm->u.chal.obex_chal_realm = event->challenge.realm;
        callback(parm);
    case GOEP_EVENT_AUTH_RESPONSE:
        parm->event = MAPS_EVENT_AUTH_RESPONSE;
        if (GOEP_ServerVerifyAuthResponse(event->handler, server->password, 
            (U8)OS_StrLen((const char *)(server->password))))
        {
            parm->u.auth.result = MAP_AUTH_OK;
        }
        else 
        {
            GOEP_ServerAbort(event->handler, OBRC_UNAUTHORIZED);
            parm->u.auth.result = MAP_AUTH_INVALID_CREDENTIALS;
        }
        callback(parm);
        break;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
    case GOEP_EVENT_HEADER_RX:
        bt_trace(TRACE_GROUP_9, BTLOG_MAPS_GOEP_SERVER_CALLBACK_HEADER_RX, event->header.type);
        switch (event->header.type) {
        case OBEXH_TYPE:
        case OBEXH_NAME:
        case OBEXH_LENGTH:
        case OBEXH_APP_PARAMS:
            {
                U8 *buf = event->header.buffer;
                U16 len = event->header.len;
                while (len)
                {
                    U8 *p = buf;
                    U16 parm_len = 0;
                    switch (p[0])
                    {
                    case MAPAPTAG_NOTIFICATION_STATUS:
                        parm->u.push_pull_obj->u.notif_registration.enable = p[2];
                        parm_len = 3;
                        break;
                    case MAPAPTAG_MAX_LIST_COUNT:
                        if (OS_StriCmp((char *)event->info.pushpull.type, MAP_MIME_FOLDER_LISTING) == 0)
                        {
                            parm->u.push_pull_obj->u.folder_list.max_list_count = BEtoHost16(p + 2);
			    parm->u.push_pull_obj->u.folder_list.flag_max_list_count = TRUE;
                        }
                        else if (OS_StriCmp((char *)event->info.pushpull.type, MAP_MIME_MESSAGE_LISTING) == 0)
                        {
                            parm->u.push_pull_obj->u.message_list.max_list_count = BEtoHost16(p + 2);
			    parm->u.push_pull_obj->u.message_list.flag_max_list_count = TRUE;
                        }
                        parm_len = 4;
                        break;
                    case MAPAPTAG_START_OFFSET:
                        if (OS_StriCmp((char *)event->info.pushpull.type, MAP_MIME_FOLDER_LISTING) == 0)
                        {
                            parm->u.push_pull_obj->u.folder_list.list_start_offset = BEtoHost16(p + 2);
                        }
                        else if (OS_StriCmp((char *)event->info.pushpull.type, MAP_MIME_MESSAGE_LISTING) == 0)
                        {
                            parm->u.push_pull_obj->u.message_list.list_start_offset = BEtoHost16(p + 2);
                        }
                        parm_len = 4;
                        break;
                    case MAPAPTAG_SUBJECT_LENGTH:
                        parm->u.push_pull_obj->u.message_list.subject_length = *(p + 2);
			parm->u.push_pull_obj->u.message_list.flag_subject_length = TRUE;
                        parm_len = 3;
                        break;
                    case MAPAPTAG_PARAMETER_MASK:
                        parm->u.push_pull_obj->u.message_list.parameter_mask = BEtoHost32(p + 2);
                        parm_len = 6;
                        break;
                    case MAPAPTAG_FILTER_MESSAGE_TYPE:
                        parm->u.push_pull_obj->u.message_list.filter_message_type = *(p + 2);
                        parm_len = 3;
                        break;
                    case MAPAPTAG_FILTER_PERIOD_BEGIN:
                        OS_StrnCpy((char *)parm->u.push_pull_obj->u.message_list.filter_period_begin, (char *)(p + 2), 
							(*(p + 1) > MAP_MAX_DATETIME_LEN ? MAP_MAX_DATETIME_LEN : (*(p + 1))));
                        parm_len = *(p + 1) + 2;
                        break;
                    case MAPAPTAG_FILTER_PERIOD_END:
                        OS_StrnCpy((char *)parm->u.push_pull_obj->u.message_list.filter_period_end, (char *)(p + 2), 
							(*(p + 1) > MAP_MAX_DATETIME_LEN ? MAP_MAX_DATETIME_LEN : (*(p + 1))));
                        parm_len = *(p + 1) + 2;
                        break;
                    case MAPAPTAG_FILTER_READ_STATUS:
                        parm->u.push_pull_obj->u.message_list.filter_read_status = *(p + 2);
                        parm_len = 3;
                        break;
                    case MAPAPTAG_FILTER_RECIPIENT:
			OS_StrnCpy((char *)parm->u.push_pull_obj->u.message_list.filter_recipient, (char *)(p + 2), 
							(*(p + 1) > max_filter_text_len ? max_filter_text_len : (*(p + 1))));
                        parm_len = *(p + 1) + 2;
                        break;
                    case MAPAPTAG_FILTER_ORIGINATOR:
			OS_StrnCpy((char *)parm->u.push_pull_obj->u.message_list.filter_originator, (char *)(p + 2), 
							(*(p + 1) > max_filter_text_len ? max_filter_text_len : (*(p + 1))));
                        parm_len = *(p + 1) + 2;
                        break;
                    case MAPAPTAG_FILTER_PRIORITY:
                        parm->u.push_pull_obj->u.message_list.filter_priority = *(p + 2);
                        parm_len = 3;
                        break;
                    case MAPAPTAG_STATUS_INDICATOR:
                        parm->u.push_pull_obj->u.msg_status.type = *(p + 2);
                        parm_len = 3;
                        break;
                    case MAPAPTAG_STATUS_VALUE:
                        parm->u.push_pull_obj->u.msg_status.enable = *(p + 2);
                        parm_len = 3;
                        break;
                    case MAPAPTAG_TRANSPARENT:
                        parm->u.push_pull_obj->u.put_message.transparent = *(p + 2);
                        parm_len = 3;
                        break;
                    case MAPAPTAG_RETRY:
                        parm->u.push_pull_obj->u.put_message.retry = *(p + 2);
                        parm_len = 3;
                        break;
                    case MAPAPTAG_CHARSET:
                        if (event->oper == GOEP_OPER_PUSH)
                        {
                            parm->u.push_pull_obj->u.put_message.charset = *(p + 2);
                        }
                        else
                        {
                            parm->u.push_pull_obj->u.message.charset = *(p + 2);
                        }
                        parm_len = 3;
                        break;
                    case MAPAPTAG_ATTACHMENT:
                        parm->u.push_pull_obj->u.message.attachment = *(p + 2);
                        parm_len = 3;
                        break;
                    case MAPAPTAG_FRACTION_REQUEST:
                        parm->u.push_pull_obj->u.message.fraction_request = *(p + 2);
                        parm_len = 3;
                        break;
                    }
                    len -= parm_len;
                    buf += parm_len;
                }
            }
        case OBEXH_TARGET:
        default:
            break;
        }
        break;
    case GOEP_EVENT_PRECOMPLETE:
        break;
    case GOEP_EVENT_PROVIDE_OBJECT:
        OS_MemCopy(parm->remote_addr.addr, conn->bdAddr.addr, BD_ADDR_SIZE);
        switch (event->oper)
        {
            case GOEP_OPER_PUSH:
                if (OS_StriCmp((char *)event->info.pushpull.type, MAP_MIME_NOTIF_REGISTRATION) == 0 && conn)
                {
                    parm->u.push_pull_obj->type = MAP_OBJ_NOTIF_REGISTRATION;
		    /*OBEX packrt with the notification info in MAP may contain more packets */
		    /*So wait for the final bit to notify upper layer that the notification is set or unset*/
              //      callback(parm);
                }
                else if (OS_StriCmp((char *)event->info.pushpull.type, MAP_MIME_MESSAGE_UPDATE) == 0 && conn)
                {
                    parm->u.push_pull_obj->type = MAP_OBJ_INBOX_UPDATE;
                    parm->event = MAPS_EVENT_UPDATE_INBOX;
                    callback(parm);
                }
                else if (OS_StriCmp((char *)event->info.pushpull.type, MAP_MIME_MESSAGE_STATUS) == 0 && conn)
                {
                    parm->u.push_pull_obj->type = MAP_OBJ_MESSAGE_STATUS;
              //      parm->event = MAPS_EVENT_MESSAGE_STATUS;
                    OS_MemCopy((U8 *)parm->u.push_pull_obj->u.msg_status.handle, (U8 *)event->info.pushpull.name, MAP_MAX_HANDLE_STR_LEN);
             //       callback(parm);
                }
                else if (OS_StriCmp((char *)event->info.pushpull.type, MAP_MIME_MESSAGE) == 0 && conn)
                {
                    parm->u.push_pull_obj->total_len = event->info.pushpull.objectLen;
                    parm->u.push_pull_obj->type = MAP_OBJ_MESSAGE;
                    parm->event = MAPS_EVENT_PUSH_MESSAGE_START;
                    callback(parm);
                }
                GOEP_ServerAccept(event->handler, parm->u.push_pull_obj);  /* Must. or the operation would abort */
                break;
            case GOEP_OPER_PULL:
                if (OS_StriCmp((char *)event->info.pushpull.type, MAP_MIME_FOLDER_LISTING) == 0 && conn)
                {
                    parm->u.push_pull_obj->type = MAP_OBJ_FOLDER_LIST;
                    parm->event = MAPS_EVENT_GET_FOLDER_LIST;
		    maps_obs_check(parm->u.push_pull_obj);
                    callback(parm);
                }
                else if (OS_StriCmp((char *)event->info.pushpull.type, MAP_MIME_MESSAGE_LISTING) == 0 && conn)
                {
                    parm->u.push_pull_obj->type = MAP_OBJ_MESSAGE_LIST;
                    parm->event = MAPS_EVENT_GET_MESSAGE_LIST;
                    OS_MemCopy((U8 *)parm->u.push_pull_obj->u.message_list.folder, (U8 *)event->info.pushpull.name, event->info.pushpull.nameLen);
		    maps_obs_check(parm->u.push_pull_obj);
                    callback(parm);
                }
                else if (OS_StriCmp((char *)event->info.pushpull.type, MAP_MIME_MESSAGE) == 0 && conn)
                {
                    parm->u.push_pull_obj->type = MAP_OBJ_MESSAGE;
                    parm->event = MAPS_EVENT_GET_MESSAGE;
                    OS_MemCopy((U8 *)parm->u.push_pull_obj->u.message.handle, (U8 *)event->info.pushpull.name, MAP_MAX_HANDLE_STR_LEN);
                    callback(parm);
                }
                break;
        }
        //GOEP_ServerAccept(event->handler, parm->u.push_pull_obj);
        break;
    case GOEP_EVENT_COMPLETE:
        switch (event->oper)
        {
            case GOEP_OPER_CONNECT:
            case GOEP_OPER_DISCONNECT:
            case GOEP_OPER_SETFOLDER:
                break;
            case GOEP_OPER_PULL:
                parm->event = MAPS_EVENT_GET_COMPLETE;
                callback(parm);
                maps_obs_close(parm->u.push_pull_obj);
                parm->u.push_pull_obj = NULL;
                break;
            case GOEP_OPER_PUSH:
                parm->event = MAPS_EVENT_PUT_COMPLETE;
                callback(parm);
                maps_obs_close(parm->u.push_pull_obj);
                parm->u.push_pull_obj = NULL;
                break;
            default:
                break;
        }
        break;
    case GOEP_EVENT_ABORTED:
        switch (event->oper)
        {
            case GOEP_OPER_PULL:
                parm->event = MAPS_EVENT_GET_COMPLETE;
                callback(parm);
                maps_obs_close(parm->u.push_pull_obj);
                parm->u.push_pull_obj = NULL;
                break;
            case GOEP_OPER_PUSH:
                parm->event = MAPS_EVENT_PUT_COMPLETE;
                callback(parm);
                maps_obs_close(parm->u.push_pull_obj);
                parm->u.push_pull_obj = NULL;
                break;
            default:
                break;
        }
    case GOEP_EVENT_TP_CONNECTED:
        break;
    case GOEP_EVENT_TP_DISCONNECTED:
        {
            int i;
            for (i = 0; i < NUM_MAP_SRV_CONN; i++)
            {
                if (conn && conn->state != MAP_CONN_STATE_NULL)
                {
                    conn->goep_conn_id = 0;
//                    conn->obexConn.connId = 0;
                    conn->state = MAP_CONN_STATE_NULL;
                    parm->event = MAPS_EVENT_TP_DISCONNECTED;
                    OS_MemCopy(parm->remote_addr.addr, conn->bdAddr.addr, BD_ADDR_SIZE);
                    callback(parm);
                    break;
                }
            }
        }
        break;
	case GOEP_EVENT_AUTHORIZE_IND:
        {
            ObexTpConnInfo tpInfo;
            int i;

            GOEP_ServerGetTpConnInfo(event->handler, &tpInfo);  /* TBC */
            for (i = 0; i < NUM_MAP_SRV_CONN; i++)
            {
                if (server->mapConn[i].state == MAP_CONN_STATE_NULL)
                {
                    server->mapConn[i].goep_conn_id = event->handler->connId;
                    OS_MemCopy(server->mapConn[i].bdAddr.addr, tpInfo.remDev->bdAddr.addr, BD_ADDR_SIZE);
                    server->mapConn[i].state = MAP_CONN_STATE_AUTHORIZING;
                    parm->event = MAPS_EVENT_AUTHORIZE_IND;
                    OS_MemCopy(parm->remote_addr.addr, tpInfo.remDev->bdAddr.addr, BD_ADDR_SIZE);
                    callback(parm);
                    break;
                }
            }
        }
        break;
    default:
        break;
    }
}

void MapMnsGoepClientCallback(GoepClientEvent *event)
{
    MapClient *client;
    ObStatus status = OB_STATUS_SUCCESS;
    maps_callback_parm_struct *parm;
    map_server_app_callback callback;
    
    client = maps_get_mns_client_by_handler(event->handler);
    parm = &client->mapConn.cbParm.sParm;
    parm->u.push_pull_obj = maps_obs_find(event->handler->connId);
    parm->goep_conn_id = client->mapConn.goep_conn_id;
    callback = client->appInfo.mnsc.callback;
    bt_trace(TRACE_GROUP_9, BTLOG_MAPS_GOEP_MNS_CLIENT_CALLBACK, event->event, event->oper, event->handler->connId, client->mapConn.goep_conn_id);

    switch (event->event)
    {
    case GOEP_EVENT_TP_CONNECTED:
        {
            client->mapConn.goep_conn_id = event->handler->connId;
            status = GOEP_Connect(&client->cApp, &client->cApp.connect_req);
        }
        break;
    case GOEP_EVENT_CONTINUE:
        switch (event->oper)
        {
            case GOEP_OPER_ABORT:
            case GOEP_OPER_DISCONNECT:
                GOEP_ClientContinue(&client->cApp);
                break;
            case GOEP_OPER_PULL:
            case GOEP_OPER_PUSH:
                if (parm->u.push_pull_obj && (parm->u.push_pull_obj->offset < parm->u.push_pull_obj->total_len))
                {
                    GOEP_ClientContinue(&client->cApp);
                }
                break;
        }
        break;
    case GOEP_EVENT_COMPLETE:
        switch (event->oper) 
        {
        case GOEP_OPER_CONNECT:
            client->mapConn.state = MAP_CONN_STATE_CONNECTED;
//            client->mapConn.obexConn.connId = event->handler->obexConnId;
            OS_MemCopy(parm->remote_addr.addr, client->mapConn.bdAddr.addr, BD_ADDR_SIZE);
            parm->event = MAPS_EVENT_MNS_CONNECTED;
            callback(parm);
            break;
        case GOEP_OPER_DISCONNECT:
            GOEP_TpDisconnect(&client->cApp);
            break;
        case GOEP_OPER_PUSH:
            parm->event = MAPS_EVENT_MNS_PUT_COMPLETE;
            callback(parm);
            maps_obs_close(parm->u.push_pull_obj);
            parm->u.push_pull_obj = NULL;
            break;
        }
        break;
    case GOEP_EVENT_TP_DISCONNECTED:
        {
            if (client->mapConn.state != MAP_CONN_STATE_NULL)
            {
                client->mapConn.goep_conn_id = 0;
//                client->mapConn.obexConn.connId = 0;
                client->mapConn.state = MAP_CONN_STATE_NULL;
                parm->event = MAPS_EVENT_MNS_DISCONNECTED;
                OS_MemCopy(parm->remote_addr.addr, client->mapConn.bdAddr.addr, BD_ADDR_SIZE);
                callback(parm);
                break;
            }
        }
        break;
    }
}
#endif

#ifdef __BT_MAPC_PROFILE__
extern MapContext g_map_cntx;

void MapGoepClientCallback(GoepClientEvent *event)
{
    MapClient *client;
    ObStatus status = OB_STATUS_SUCCESS;
    mapc_callback_parm_struct *parm;
    map_client_app_callback callback;
    
    client = &g_map_cntx.client;
    parm = &client->mapConn.cbParm.cParm;
    parm->u.push_pull_obj = mapc_obs_find(event->handler->connId);
    parm->goep_conn_id = client->mapConn.goep_conn_id;
    callback = client->appInfo.masc.callback;
    bt_trace(TRACE_GROUP_9, BTLOG_MAPC_GOEP_CLINET_CALLBACK, event->event, event->oper, event->handler->connId, client->mapConn.goep_conn_id);

    switch (event->event)
    {
    case GOEP_EVENT_TP_CONNECTED:
        {
            client->mapConn.goep_conn_id = event->handler->connId;
            status = GOEP_Connect(&client->cApp, &client->cApp.connect_req);
        }
        break;
    case GOEP_EVENT_CONTINUE:
        switch (event->oper)
        {
            case GOEP_OPER_ABORT:
            case GOEP_OPER_DISCONNECT:
                GOEP_ClientContinue(&client->cApp);
                break;
            case GOEP_OPER_PULL:
                if (parm->u.push_pull_obj /*&& (parm->u.push_pull_obj->offset < parm->u.push_pull_obj->total_len)*/)
                {
                    GOEP_ClientContinue(&client->cApp);
                }
            case GOEP_OPER_PUSH:
                if (parm->u.push_pull_obj && (parm->u.push_pull_obj->offset < parm->u.push_pull_obj->total_len))
                {
                    GOEP_ClientContinue(&client->cApp);
                }
                break;
        }
        break;
    case GOEP_EVENT_HEADER_RX:
        bt_trace(TRACE_GROUP_9, BTLOG_MAPC_GOEP_CLIENT_CALLBACK_HEADER_RX, event->header.type);
        switch (event->header.type) {
        case OBEXH_NAME:
            if (parm->u.push_pull_obj && parm->u.push_pull_obj->type == MAP_OBJ_MESSAGE)
            {
                OS_MemCopy(parm->u.push_pull_obj->u.put_message.handle, event->header.buffer, MAP_MAX_HANDLE_STR_LEN);
            }
            break;
        case OBEXH_APP_PARAMS:
            {
                U8 *buf = event->header.buffer;
                U16 len = event->header.len;
                while (len)
                {
                    U8 *p = buf;
                    U16 parm_len = 0;
                    switch (p[0])
                    {
                    case MAPAPTAG_FOLDER_LISTING_SIZE:
                        parm->u.push_pull_obj->u.folder_list.list_count = BEtoHost16(p + 2);
                        parm_len = 4;
                        break;
                    case MAPAPTAG_MESSAGE_LISTING_SIZE:
                        parm->u.push_pull_obj->u.message_list.list_count = BEtoHost16(p + 2);
                        parm_len = 4;
                        break;
                    case MAPAPTAG_NEW_MESSAGE:
                        parm->u.push_pull_obj->u.message_list.new_message = p[2];
                        parm_len = 3;
                        break;
                    case MAPAPTAG_MSE_TIME:
                        OS_StrnCpy(parm->u.push_pull_obj->u.message_list.mse_time, p + 2, MAP_MAX_DATETIME_LEN);
                        parm_len = *(p + 1) + 2;
                        break;
                    case MAPAPTAG_FRACTION_DELIVER:
                        parm->u.push_pull_obj->u.message.fraction_deliver = p[2];
                        parm_len = 3;
                        break;
                    }
                    len -= parm_len;
                    buf += parm_len;
                }
            }
        case OBEXH_TARGET:
        default:
            break;
        }
        break;
    case GOEP_EVENT_COMPLETE:
        switch (event->oper) 
        {
        case GOEP_OPER_CONNECT:
            client->mapConn.state = MAP_CONN_STATE_CONNECTED;
            OS_MemCopy(parm->remote_addr.addr, client->mapConn.bdAddr.addr, BD_ADDR_SIZE);
            parm->event = MAPC_EVENT_CONNECTED;
            callback(parm);
            break;
        case GOEP_OPER_DISCONNECT:
            GOEP_TpDisconnect(&client->cApp);
            break;
        case GOEP_OPER_PUSH:
            parm->event = MAPC_EVENT_PUT_COMPLETE;
            callback(parm);
            mapc_obs_close(parm->u.push_pull_obj);
            parm->u.push_pull_obj = NULL;
            break;
        case GOEP_OPER_PULL:
            parm->event = MAPC_EVENT_GET_COMPLETE;
            callback(parm);
            mapc_obs_close(parm->u.push_pull_obj);
            parm->u.push_pull_obj = NULL;
            break;
        case GOEP_OPER_SETFOLDER:
            parm->event = MAPC_EVENT_SET_FOLDER_COMPLETE;
            callback(parm);
            mapc_obs_close(parm->u.push_pull_obj);
            parm->u.push_pull_obj = NULL;
            break;
        }
        break;
    case GOEP_EVENT_ABORTED:
        switch (event->oper)
        {
            case GOEP_OPER_PULL:
                parm->event = MAPC_EVENT_GET_ABORTED;
                callback(parm);
                mapc_obs_close(parm->u.push_pull_obj);
                parm->u.push_pull_obj = NULL;
                break;
            case GOEP_OPER_PUSH:
                parm->event = MAPC_EVENT_PUT_ABORTED;
                callback(parm);
                mapc_obs_close(parm->u.push_pull_obj);
                parm->u.push_pull_obj = NULL;
                break;
            default:
                break;
        }
        break;
    case GOEP_EVENT_TP_DISCONNECTED:
        {
            if (client->mapConn.state != MAP_CONN_STATE_NULL)
            {
                client->mapConn.goep_conn_id = 0;
                client->mapConn.state = MAP_CONN_STATE_NULL;
                parm->event = MAPC_EVENT_DISCONNECTED;
                OS_MemCopy(parm->remote_addr.addr, client->mapConn.bdAddr.addr, BD_ADDR_SIZE);
                callback(parm);
                break;
            }
        }
        break;
    }
}


void MapMnsGoepServerCallback(GoepServerEvent *event)
{
    mapc_callback_parm_struct *parm;
    map_client_app_callback callback;
    MapServer *server = &g_map_cntx.mnsServer;

    parm = &server->mapConn[0].cbParm.cParm;
    parm->u.push_pull_obj = mapc_obs_find(event->handler->connId);
    parm->goep_conn_id = event->handler->connId;
    callback = g_map_cntx.client.appInfo.masc.callback;
    bt_trace(TRACE_GROUP_9, BTLOG_MAPC_GOEP_MNS_SERVER_CALLBACK, event->event, event->oper, event->handler->connId);

    switch (event->event)
    {
	case GOEP_EVENT_AUTHORIZE_IND:
        {
            ObexTpConnInfo tpInfo;

            GOEP_ServerGetTpConnInfo(event->handler, &tpInfo);  /* TBC */
            if (server->mapConn[0].state == MAP_CONN_STATE_NULL)
            {
                server->mapConn[0].goep_conn_id = event->handler->connId;
                OS_MemCopy(server->mapConn[0].bdAddr.addr, tpInfo.remDev->bdAddr.addr, BD_ADDR_SIZE);
                server->mapConn[0].state = MAP_CONN_STATE_AUTHORIZING;
                parm->event = MAPC_EVENT_MNS_AUTHORIZE_IND;
                OS_MemCopy(parm->remote_addr.addr, tpInfo.remDev->bdAddr.addr, BD_ADDR_SIZE);
                callback(parm);
                break;
            }
        }
        break;
    case GOEP_EVENT_CONTINUE:
        switch (event->oper)
        {
        case GOEP_OPER_ABORT:
        case GOEP_OPER_DISCONNECT:
            GOEP_ServerContinue(event->handler);
            break;
        case GOEP_OPER_CONNECT:
            {
                if (server->mapConn[0].state == MAP_CONN_STATE_AUTHORIZING)
                {
                    server->mapConn[0].state = MAP_CONN_STATE_CONNECTING;
                    OS_MemCopy(parm->remote_addr.addr, server->mapConn[0].bdAddr.addr, BD_ADDR_SIZE);
                    parm->mas_instance = server->appInfo.mass.instance;
                    parm->event = MAPC_EVENT_MNS_CONNECT;
                    callback(parm);
                    break;
                }
            }
            break;
        case GOEP_OPER_PUSH:
            if (parm->u.push_pull_obj)
            {
                GOEP_ServerContinue(event->handler);  /* Must. so it read remains data */
            }
            break;
        }
        break;
    case GOEP_EVENT_PROVIDE_OBJECT:
        OS_MemCopy(parm->remote_addr.addr, server->mapConn[0].bdAddr.addr, BD_ADDR_SIZE);
        switch (event->oper)
        {
        case GOEP_OPER_PUSH:
            if (OS_StriCmp(event->info.pushpull.type, MAP_MIME_EVENT_REPORT) == 0)
            {
                parm->event = MAPC_EVENT_MNS_EVENT_REPORT_START;
                callback(parm);
            }
            GOEP_ServerAccept(event->handler, parm->u.push_pull_obj);  /* Must. or the operation would abort */
            break;
        }
        break;
    case GOEP_EVENT_COMPLETE:
        switch (event->oper) 
        {
        case GOEP_OPER_PUSH:
            parm->event = MAPC_EVENT_MNS_PUT_COMPLETE;
            callback(parm);
            mapc_obs_close(parm->u.push_pull_obj);
            parm->u.push_pull_obj = NULL;
            break;
        }
        break;
    case GOEP_EVENT_ABORTED:
        switch (event->oper) 
        {
        case GOEP_OPER_PUSH:
            parm->event = MAPC_EVENT_MNS_PUT_ABORTED;
            callback(parm);
            mapc_obs_close(parm->u.push_pull_obj);
            parm->u.push_pull_obj = NULL;
            break;
        }
        break;
    case GOEP_EVENT_TP_DISCONNECTED:
        {
            if (server->mapConn[0].state != MAP_CONN_STATE_NULL)
            {
                server->mapConn[0].goep_conn_id = 0;
                server->mapConn[0].state = MAP_CONN_STATE_NULL;
                parm->event = MAPC_EVENT_MNS_DISCONNECTED;
                OS_MemCopy(parm->remote_addr.addr, server->mapConn[0].bdAddr.addr, BD_ADDR_SIZE);
                callback(parm);
                break;
            }
        }
        break;
    }
}
#endif
#endif /* defined(__BT_MAPS_PROFILE__) || defined(__BT_MAPC_PROFILE__) */
