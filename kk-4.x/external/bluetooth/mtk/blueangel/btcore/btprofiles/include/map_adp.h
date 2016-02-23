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
 *   map_adp.h
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
#ifndef __MAP_ADP_H__
#define __MAP_ADP_H__

#define MAP_MAX_FOLDER_PATH_LEN 512  /* MAP Spec. */
#define MAP_MAX_SERVICE_NAME_LEN 256
#define MAP_MAX_FILTER_TEXT_LEN 255  /* OBEX TLV limitation */
#define MAP_MAX_DATE_LEN 16      /* OBEX Spec. */
#define MAP_MAX_DATETIME_LEN 20      /* OBEX Spec. */
#define MAP_MAX_HANDLE_STR_LEN 32    /* MAP Spec.: 16 UTF16BE hex digits */
#define MAP_MAX_TEMP_FILE_NAME_LEN 80
#define MAP_MAX_LIST_COUNT 1024
#define MAP_MAX_SUBJECT_LEN 255

typedef enum
{
	MAP_SDP_MSG_EMAIL = 0x01,
	MAP_SDP_MSG_SMS_GSM = 0x02,
    MAP_SDP_MSG_SMS_CDMA = 0x04,
    MAP_SDP_MSG_SMS_MMS = 0x08	
} map_sdp_supported_msg_enum;

typedef enum
{
    MAP_MSG_SMS_GSM = 0x01,
    MAP_MSG_SMS_CDMA = 0x02,
    MAP_MSG_EMAIL = 0x04,
    MAP_MSG_SMS_MMS = 0x08
} map_supported_msg_enum;  /* Ref spec. */

typedef enum
{
    MAPS_EVENT_CONNECT,
    MAPS_EVENT_AUTH_CHALLENGE,
    MAPS_EVENT_AUTH_RESPONSE,
    MAPS_EVENT_TP_DISCONNECTED,
    MAPS_EVENT_AUTHORIZE_IND,
    MAPS_EVENT_DATA_REQ,
    MAPS_EVENT_DATA_IND,
    MAPS_EVENT_SET_FOLDER,
    MAPS_EVENT_SET_NOTIF,
    MAPS_EVENT_GET_FOLDER_LIST,
    MAPS_EVENT_GET_MESSAGE_LIST,
    MAPS_EVENT_GET_MESSAGE,
    MAPS_EVENT_GET_COMPLETE,
    MAPS_EVENT_PUSH_MESSAGE_START,
    MAPS_EVENT_PUSH_MESSAGE_RECEIVED,
    MAPS_EVENT_PUT_COMPLETE,
    MAPS_EVENT_UPDATE_INBOX,
    MAPS_EVENT_MESSAGE_STATUS,
    MAPS_EVENT_MNS_CONNECTED,
    MAPS_EVENT_MNS_DISCONNECTED,
    MAPS_EVENT_MNS_PUT_COMPLETE
} maps_event_enum;

typedef enum
{
    MAPC_EVENT_CONNECTED,
    MAPC_EVENT_DISCONNECTED,
    MAPC_EVENT_DATA_IND,
    MAPC_EVENT_DATA_REQ,
    MAPC_EVENT_PUT_COMPLETE,
    MAPC_EVENT_PUT_ABORTED,
    MAPC_EVENT_GET_COMPLETE,
    MAPC_EVENT_GET_ABORTED,
    MAPC_EVENT_SET_FOLDER_COMPLETE,
    MAPC_EVENT_MNS_AUTHORIZE_IND,
    MAPC_EVENT_MNS_CONNECT,
    MAPC_EVENT_MNS_EVENT_REPORT_START,
    MAPC_EVENT_MNS_PUT_COMPLETE,
    MAPC_EVENT_MNS_PUT_ABORTED,
    MAPC_EVENT_MNS_DISCONNECTED
} mapc_event_enum;

typedef enum
{
    MAP_OBJ_MNS_EVENT,
    MAP_OBJ_FOLDER_LIST,
    MAP_OBJ_FOLDER_LIST_SIZE,
    MAP_OBJ_MESSAGE_LIST,
    MAP_OBJ_MESSAGE_LIST_SIZE,
    MAP_OBJ_MESSAGE,
    MAP_OBJ_NOTIF_REGISTRATION,
    MAP_OBJ_MESSAGE_STATUS,
    MAP_OBJ_INBOX_UPDATE
} map_obj_enum;

typedef enum
{
    MAP_MSG_STATUS_READ = 0x0,
    MAP_MSG_STATUS_DELETE = 0x1
} map_msg_status_enum;  /* Ref spec. */

typedef enum
{
    MAP_MSGLIST_PARAM_ALL     =      0x00000000,
    MAP_MSGLIST_PARAM_SUBJECT =      0x00000001,
    MAP_MSGLIST_PARAM_DATETIME =     0x00000002,
    MAP_MSGLIST_PARAM_SENDER_NAME =  0x00000004,
    MAP_MSGLIST_PARAM_SENDER_ADDR =  0x00000008,
    MAP_MSGLIST_PARAM_REC_NAME =     0x00000010,
    MAP_MSGLIST_PARAM_REC_ADDR =     0x00000020,
    MAP_MSGLIST_PARAM_TYPE =         0x00000040,
    MAP_MSGLIST_PARAM_SIZE =         0x00000080,
    MAP_MSGLIST_PARAM_REC_STATUS =   0x00000100,
    MAP_MSGLIST_PARAM_TEXT =         0x00000200,
    MAP_MSGLIST_PARAM_ATTACH_SIZE =  0x00000400,
    MAP_MSGLIST_PARAM_PRIORITY =     0x00000800,
    MAP_MSGLIST_PARAM_READ =         0x00001000,
    MAP_MSGLIST_PARAM_SENT =         0x00002000,
    MAP_MSGLIST_PARAM_PROTECTED =    0x00004000,
    MAP_MSGLIST_PARAM_REPLYTO_ADDR = 0x00008000,

    /* Reserved */
} map_msglist_parameter_enum;  /* Ref. Spec */

typedef enum
{
    MAP_FILTER_NONE = 0x0,    /* no filter */
    MAP_FILTER_READ = 0x1,    /* get unread only */
    MAP_FILTER_UNREAD = 0x2   /* get read only */
} map_filter_read_enum;  /* Ref. Spec */

typedef enum
{
    /* MAP_FILTER_NONE = 0x0,*/        /* no  filter. Ref. map_filter_read_enum */ 
    MAP_FILTER_NO_HIGH = 0x1,     /* get high only */
    MAP_FILTER_HIGH = 0x2         /* get non-high only */
} map_filter_priority_enum;  /* Ref. Spec */

typedef enum
{
    MAP_CHARSET_NATIVE = 0x0,
    MAP_CHARSET_UTF8
} map_charset_enum;  /* Ref. Spec */

typedef enum
{
    MAP_FRACTION_REQ_FIRST = 0x0,
    MAP_FRACTION_REQ_NEXT,
    MAP_FRACTION_REQ_NO
} map_fraction_req_enum;  /* Ref. Spec */

typedef enum
{
    MAP_FRACTION_DELIV_MORE = 0x0,
    MAP_FRACTION_DELIV_LAST,
    MAP_FRACTION_DELIV_NO
} map_fraction_deliv_enum;  /* Ref. Spec */

typedef enum
{
    MAP_FOLDER_ROOT,
    MAP_FOLDER_UP,
    MAP_FOLDER_DOWN
} map_folder_op_enum;

typedef enum
{
    MAP_AUTH_OK,
    MAP_AUTH_NO_RCVD_AUTH_RESP,
    MAP_AUTH_INVALID_CREDENTIALS
} map_auth_result_enum;

typedef enum
{
    MAP_OPER_CONNECT,
    MAP_OPER_DISCONNECT,
    MAP_OPER_PULL,
    MAP_OPER_PUSH,
    MAP_OPER_SETFOLDER,
    MAP_OPER_ABORT
} map_op_enum;

typedef struct {
    ListEntry node;
    U8 conn_id;

    map_obj_enum type;
    U8 file[MAP_MAX_TEMP_FILE_NAME_LEN + 1];
    S32 fhandle;
    U8* buff;
    U32 buff_len;
    U32 offset;
    U32 total_len;

    union
    {
        struct {
            U8 mas_id;
        } event_report;

        struct {
            BOOL enable;
        } notif_registration;

        struct {
            char handle[MAP_MAX_HANDLE_STR_LEN+ 2];  /* [M] */
            map_msg_status_enum type;
            BOOL enable;
        } msg_status;

        struct {
            map_folder_op_enum op;
            char folder[MAP_MAX_FOLDER_PATH_LEN];
        } set_folder;

        struct {
            U16 max_list_count;     /* REQ [O] */
            U16 list_start_offset;  /* REQ [O] */
	    BOOL flag_max_list_count;

            U16 list_count;         /* RSP [C] */
        } folder_list;

        struct {
            char folder[MAP_MAX_FOLDER_PATH_LEN];  /* [M] */
            U16 max_list_count;     /* REQ [O] */
            U16 list_start_offset;  /* REQ [O] */
            U8 subject_length;      /* REQ [O] */
            map_msglist_parameter_enum parameter_mask;     /* REQ [O] */

            /* REQ [O] Filters */
            map_supported_msg_enum filter_message_type;    /* 0: no filter, 1: filter out the type */
            U8 filter_period_begin[MAP_MAX_DATE_LEN + 1];  /* YYYYMMDDTHHMMSS or YYYYMMDDTHHMMSSZ */
            U8 filter_period_end[MAP_MAX_DATE_LEN + 1];  /* YYYYMMDDTHHMMSS or YYYYMMDDTHHMMSSZ */
            map_filter_read_enum filter_read_status;
            U8 filter_recipient[MAP_MAX_FILTER_TEXT_LEN];  /* UTF8 */
            U8 filter_originator[MAP_MAX_FILTER_TEXT_LEN];  /* UTF8 */
            map_filter_priority_enum filter_priority;

	    BOOL flag_max_list_count;
	    BOOL flag_subject_length;

            BOOL new_message;   /* RSP [C] */
            U16 list_count;     /* RSP [C] */
            U8 mse_time[MAP_MAX_DATETIME_LEN + 1];  /* YYYYMMDDTHHMMSS+hhmm or YYYYMMDDTHHMMSS */
        } message_list;

        struct {
            map_fraction_deliv_enum fraction_deliver;  /* RSP [O] */

            char handle[MAP_MAX_HANDLE_STR_LEN+ 2];  /* REQ [M] */
            BOOL attachment;                         /* REQ [M] 0: exclude attachment, 1: include attachment */
            map_charset_enum charset;                /* REQ [M] */
            map_fraction_req_enum fraction_request;  /* REQ [O] */
        } message;

        struct {
            char folder[MAP_MAX_FOLDER_PATH_LEN];  /* REQ [M] */
            BOOL transparent;                      /* REQ [O] 0: save&send (default), 1: sent */
            BOOL retry;                            /* REQ [O] 0: no send retry, 1: retry send (default) */
            map_charset_enum charset;              /* REQ [M] */

            char handle[MAP_MAX_HANDLE_STR_LEN+ 2];  /* RSP[M] */
        } put_message;
    } u;
} map_obj_struct;


typedef struct
{
    maps_event_enum  event;

    U8              mas_instance; 
    BD_ADDR         remote_addr;
    U8              goep_conn_id;

    union {
        struct {
            U8 obex_chal_option;
            U8 *obex_chal_nonce;
            U8 *obex_chal_realm;
        } chal;

        struct {
            map_auth_result_enum result;
        } auth;

        struct {
            U8 *folder;
            map_folder_op_enum op;
        } set_folder;

        map_obj_struct *push_pull_obj;
    }u;
} maps_callback_parm_struct;

typedef struct
{
    mapc_event_enum  event;

    U8              mas_instance; 
    BD_ADDR         remote_addr;
    U8              goep_conn_id;

    union {
        map_obj_struct *push_pull_obj;
    }u;
} mapc_callback_parm_struct;

typedef void (*map_server_app_callback)(maps_callback_parm_struct *parms);
typedef void (*map_client_app_callback)(mapc_callback_parm_struct *parms);

typedef union
{
    struct {
        U8 instance;
        map_supported_msg_enum supported_msg_type;
        U8 service_name[MAP_MAX_SERVICE_NAME_LEN + 1];  
        BtSecurityLevel security;
        map_server_app_callback callback;
    } mass;

    struct {
        map_server_app_callback callback;
    } mnsc;

    struct {
        map_client_app_callback callback;
    } masc;

    struct {
        BtSecurityLevel security;
        map_client_app_callback callback;
    } mnss;
} map_registration_struct;


#endif  /* __MAP_ADP_H__ */

