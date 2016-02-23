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
 *   map.h
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
#ifndef __MAP_H__
#define __MAP_H__

/*---------------------------------------------------------------------------
 * MAP_MAX_PASSWORD_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication password.
 *
 *  The default value is 20.
 */
#ifndef MAP_MAX_PASSWORD_LEN
#define MAP_MAX_PASSWORD_LEN       20
#endif

/*---------------------------------------------------------------------------
 * MAP_MAX_USERID_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication username.
 *
 *  The default value is 20.
 */
#ifndef MAP_MAX_USERID_LEN
#define MAP_MAX_USERID_LEN         20
#endif

/*---------------------------------------------------------------------------
 * MAP_MAX_REALM_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication realm.
 *
 *  The default value is 20.
 */
#ifndef MAP_MAX_REALM_LEN
#define MAP_MAX_REALM_LEN          20
#endif

/*---------------------------------------------------------------------------
 * MAP_MRU_SIZE Constant
 *
 *  Defines the MAP MRU size
 *
 *  The default value is 2048.
 */
#ifndef MAP_MRU_SIZE
#define MAP_MRU_SIZE               2048
#endif

#define MAP_APP_PARM_SIZE          600

#ifdef __BT_MAPS_PROFILE__
#define MAPS_OBS_NUM (NUM_MAP_SRVS * NUM_MAP_SRV_CONN + NUM_MAP_SRV_CONN)
#else
#define MAPS_OBS_NUM 0
#endif
#ifdef __BT_MAPC_PROFILE__
#define MAPC_OBS_NUM (NUM_MAP_CLIS + NUM_MAP_CLIS)
#else
#define MAPC_OBS_NUM 0
#endif

#define MAP_MIME_EVENT_REPORT "x-bt/MAP-event-report"
#define MAP_MIME_NOTIF_REGISTRATION "x-bt/MAP-NotificationRegistration"
#define MAP_MIME_MESSAGE_STATUS "x-bt/messageStatus"
#define MAP_MIME_MESSAGE_UPDATE "x-bt/MAP-messageUpdate"
#define MAP_MIME_FOLDER_LISTING "x-obex/folder-listing"
#define MAP_MIME_MESSAGE_LISTING "x-bt/MAP-msg-listing"
#define MAP_MIME_MESSAGE "x-bt/message"


typedef enum
{
    MAP_APP_STATE_NULL,
    MAP_APP_STATE_REGISTERING,
    MAP_APP_STATE_REGISTERED,
    MAP_APP_STATE_DEREGISTERING
} MapAppState;

typedef enum
{
    MAP_CONN_STATE_NULL,
    MAP_CONN_STATE_AUTHORIZING,
    MAP_CONN_STATE_CONNECTING,
    MAP_CONN_STATE_CONNECTED,
    MAP_CONN_STATE_DISCONNECTING
} MapConnState;

typedef enum
{
    MAPAPTAG_MAX_LIST_COUNT = 0x01,
    MAPAPTAG_START_OFFSET = 0x02,
    MAPAPTAG_FILTER_MESSAGE_TYPE = 0x03,
    MAPAPTAG_FILTER_PERIOD_BEGIN = 0x04,
    MAPAPTAG_FILTER_PERIOD_END = 0x05,
    MAPAPTAG_FILTER_READ_STATUS = 0x06,
    MAPAPTAG_FILTER_RECIPIENT = 0x07,
    MAPAPTAG_FILTER_ORIGINATOR = 0x08,
    MAPAPTAG_FILTER_PRIORITY = 0x09,
    MAPAPTAG_ATTACHMENT = 0x0A,
    MAPAPTAG_TRANSPARENT = 0x0B,
    MAPAPTAG_RETRY = 0x0C,
    MAPAPTAG_NEW_MESSAGE = 0x0D,
    MAPAPTAG_NOTIFICATION_STATUS = 0x0E,
    MAPAPTAG_MAS_INSTANCE_ID = 0x0F,
    MAPAPTAG_PARAMETER_MASK = 0x10,
    MAPAPTAG_FOLDER_LISTING_SIZE = 0x11,
    MAPAPTAG_MESSAGE_LISTING_SIZE = 0x12,
    MAPAPTAG_SUBJECT_LENGTH = 0x13,
    MAPAPTAG_CHARSET = 0x14,
    MAPAPTAG_FRACTION_REQUEST = 0x15,
    MAPAPTAG_FRACTION_DELIVER = 0x16,
    MAPAPTAG_STATUS_INDICATOR = 0x17,
    MAPAPTAG_STATUS_VALUE = 0x18,
    MAPAPTAG_MSE_TIME = 0x19
} MapAppParmTag;

typedef struct
{
    U8              goep_conn_id;
    BD_ADDR         bdAddr;
    ObexConnection  obexConn;
    MapConnState    state;

    union
    {
        maps_callback_parm_struct sParm;
        mapc_callback_parm_struct cParm;
    } cbParm;  /* profile->adp callback parameter */
} MapConn;

typedef struct
{
    MapAppState         state;
    map_registration_struct appInfo;
    
    SdpAttribute        *sdpRec;
    
#if NUM_MAP_SRV_CONN > 0
    GoepServerApp       sApp[NUM_MAP_SRV_CONN];/* GOEP server instance */
    MapConn             mapConn[NUM_MAP_SRV_CONN];
#endif

#if OBEX_AUTHENTICATION == XA_ENABLED
    U8                  password[MAP_MAX_PASSWORD_LEN];  /* OBEX Authentication password */
    U8                  userId[MAP_MAX_USERID_LEN];  /* OBEX Authentication userId */
    U8                  realm[MAP_MAX_REALM_LEN];  /* OBEX Authentication realm */
    ObexAuthChallenge   chal;
    ObexAuthResponse    resp;
    ObexAuthChallengeInfo challengeInfo;
    ObexAuthResponseInfo ResponseInfo;
    U8                  chalStr[25];
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
} MapServer;

typedef struct {
    MapAppState     state;
    map_registration_struct appInfo;
    MapConn         mapConn;

    GoepClientApp   cApp;     /* GOEP client instance */
    GoepObjectReq   pushPull;
    
    ObexAuthChallenge chal;
    U8                chalNonce[AUTH_NONCE_LEN];
    U8                chalRealm[MAP_MAX_REALM_LEN];
} MapClient;

/* MAP Context */
typedef struct
{
    BOOL initialized;

#if NUM_MAP_CLIS > 0
    MapClient client;
    MapServer mnsServer;
    ListEntry obsListC;
    map_obj_struct obsEntriesC[MAPC_OBS_NUM];
#endif

#if NUM_MAP_SRVS > 0
    MapServer servers[NUM_MAP_SRVS];
    MapClient mnsClient[NUM_MAP_SRV_CONN];
    ListEntry obsListS;
    map_obj_struct obsEntriesS[MAPS_OBS_NUM];
#endif

    U8             appParms[MAP_APP_PARM_SIZE];
}   MapContext;

BtStatus map_init(void);
BtStatus map_deinit(void);

S8 maps_get_curr_registration(void);
S8 maps_get_curr_deregistration(void);
MapServer *maps_get_server_instance_by_handler(GoepServerApp *handler);
MapConn *maps_get_server_conn_by_goep_id(U8 goep_conn_id);
MapClient *maps_get_mns_client_by_handler(GoepClientApp *handler);

MapClient *mapc_get_client_by_handler(GoepClientApp *handler);

BtStatus maps_register_server(map_registration_struct *server);
BtStatus maps_deregister_server(U8 instance);
BtStatus maps_authorize_rsp(BD_ADDR *addr, U8 rsp_code);
BtStatus maps_connect_rsp(U8 connId, BD_ADDR *bdAddr, BOOL accept);
BtStatus maps_disconnect(U8 connId);
BtStatus maps_abort(U8 connId, U8 rsp_code);
BtStatus maps_set_folder_response(U8 connId, U8 rsp_code);
BtStatus maps_get_folder_listing_response(U8 connId, U8 rsp_code, map_obj_struct *obj);
BtStatus maps_get_message_listing_response(U8 connId, U8 rsp_code, map_obj_struct *obj);
BtStatus maps_get_message_response(U8 connId, U8 rsp_code, map_obj_struct *obj);
BtStatus maps_set_notification_registration_response(U8 connId, U8 rsp_code);
BtStatus maps_set_message_status_response(U8 connId, U8 rsp_code);
BtStatus maps_push_message_response(U8 connId, U8 rsp_code, U16 *handle, U16 len);
BtStatus maps_update_inbox_response(U8 connId, U8 rsp_code);
BtStatus maps_mns_register_client(map_registration_struct *mns_client);
BtStatus maps_mns_deregister_client(void);
BtStatus maps_mns_connect(BD_ADDR *remote_addr, U8 obex_chal_option, U8 *obex_chal_nonce, U8 *obex_chal_realm);
BtStatus maps_mns_disconnect(U8 connId);
BtStatus maps_mns_send_event(U8 connId, map_obj_struct *obj);

BtStatus mapc_register_client(map_registration_struct *client);
BtStatus mapc_deregister_client(void);
BtStatus mapc_connect(BD_ADDR *remote_addr, U8 obex_chal_option, U8 *obex_chal_nonce, U8 *obex_chal_realm);
BtStatus mapc_disconnect(void);
BtStatus mapc_set_notification_registration(map_obj_struct *obj);
BtStatus mapc_set_message_status(map_obj_struct *obj);
BtStatus mapc_push_message(map_obj_struct *obj);
BtStatus mapc_update_inbox(map_obj_struct *obj);
BtStatus mapc_get_folder_listing(map_obj_struct *obj);
BtStatus mapc_get_message_listing(map_obj_struct *obj);
BtStatus mapc_get_message(map_obj_struct *obj);
BtStatus mapc_set_folder(map_obj_struct *obj);
BtStatus mapc_abort(void);

BtStatus mapc_mns_register_server(map_registration_struct *mns_server);
BtStatus mapc_mns_authorize_rsp(BD_ADDR *bdAddr, U8 rsp_code);
BtStatus mapc_mns_connect_rsp(U8 connId, BD_ADDR *bdAddr, BOOL accept);
BtStatus mapc_mns_disconnect(U32 connId);
BtStatus mapc_mns_send_event_response(U32 connId, U8 rsp_code);

void MapGoepServerCallback(GoepServerEvent *event);
void MapMnsGoepClientCallback(GoepClientEvent *event);
void MapGoepClientCallback(GoepClientEvent *event);
void MapMnsGoepServerCallback(GoepServerEvent *event);

#endif  /* __MAP_H__ */
