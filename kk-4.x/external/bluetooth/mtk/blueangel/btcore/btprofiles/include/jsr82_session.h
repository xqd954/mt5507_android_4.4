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
*  permission of MediaTek Inc. (C) 2007
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

/*****************************************************************************
 *
 * Filename:
 * ---------
 * jsr82_session.h
 *
 * Project:
 * --------
 * MAUI
 *
 * Description:
 * ------------
 * 
 *
 * Author:
 * -------
 * Dlight Ting
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
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
 * Oct 12 2008 mtk01411
 * [MAUI_01254027] [JSR82] Update JSR82 related codes
 * 
 *
 * Jul 11 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * May 14 2008 mtk01239
 * [MAUI_00771864] [Bluetooth] BT 2.1 new feature TW side
 * 
 *
 * Mar 25 2008 mtk01411
 * [MAUI_00737864] [JSR82] Check restore state check in set_local_cod_result hdler and delay setup conn
 * 
 *
 * Mar 15 2008 mtk01411
 * [MAUI_00281663] Fatal Error (b03): Abnormal Reset (63f423) - BT
 * 
 *
 * Feb 22 2008 mtk01411
 * [MAUI_00621026] [Bluetooth] Check in JSR82 codes
 * 
 *
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
 #ifndef __BT_JSR82_SESSION_H
#define __BT_JSR82_SESSION_H

#include "bluetooth_struct.h"
#include "bt_jsr82_setting.h"
#include "l2cap_i.h"
#include "conmgr.h"
#include "rfcomm_adp.h"
#include "bluetooth_jsr82_struct.h"
#include "bluetooth_gap_struct.h"
#include "ring.h"

#if (BT_JSR82_ENABLED == 1) || defined(__BT_JSR82_L2RF__)

#define JSR82_SPP_RX_CREDIT (16)
#define JSR82_SPP_TX_CREDIT (16)
#define JSR82_SESSION_SPP_RX_BUFSIZE (RF_MAX_FRAME_SIZE*JSR82_SPP_RX_CREDIT)  
#define JSR82_SESSION_SPP_TX_BUFSIZE (RF_MAX_FRAME_SIZE*JSR82_SPP_TX_CREDIT)

#define JSR82_SESSION_L2CAP_TX_BUFSIZE    (1024 * 16)
#define JSR82_SESSION_L2CAP_RX_BUFSIZE    (1024 * 16)

#define JSR82_SESSION_MAX_TX_BUFSIZE    (L2CAP_MTU > RF_MAX_FRAME_SIZE ? L2CAP_MTU : RF_MAX_FRAME_SIZE)

#define JSR82_MAX_ACL_NO (5)

#define JSR82_SESSION_TX_PKT_QUEUE_SIZE (4)

#define JSR82_NO_AVAILABLE_SESSION (0xFFFFFFFF)

#define JSR82_SET_ACL_SECURITY_SUCCESS (0x01)
#define JSR82_SET_ACL_SECURITY_FAILED (0x02)

#define JSR82_SESSION_CONNECT_ACCEPT   (0x01)
#define JSR82_SESSION_CONNECT_REJECT   (0x02)

#define JSR82_SESSION_TX_SUCCESS               (0x01)
#define JSR82_SESSION_TX_SIZE_TOO_LARGE   (0x02)
#define JSR82_SESSION_TX_ERROR   (0x03)

#define JSR82_SESSION_MAX_RX_DATA (339)

#define BT_JSR82_CHANNEL_STATE_IDLE     (0x00)
#define BT_JSR82_CHANNEL_STATE_OPEN     (0x01)
#define BT_JSR82_CHANNEL_STATE_CREATE_CHANNEL     (0x02)

#define BT_JSR82_AUTHORIZE_AUTO_REJECT (0x00)
#define BT_JSR82_AUTHORIZE_AUTO_ACCEPT (0x01)
#define BT_JSR82_AUTHORIZE_REQUEST_ACCEPT (0x02)

#define BT_JSR82_AUTHENTICATE_LINK_NONE (0x00)
#define BT_JSR82_ENCRYPTION_LINK_NONE (0x00)
#define BT_JSR82_AUTHENTICATE_LINK_ONGOING (0x01)
#define BT_JSR82_ENCRYPT_AUTHENTICATE_LINK_ONGOING (0x02)
#define BT_JSR82_ENCRYPTION_LINK_ONGOING (0x03)
/* Add this to follow the ISO 3309, PSM value must be odd, LSB-bit of LSB byte must be 1, LSB-bit of MSB byte must be 0: 2007-1213 */
#define BT_JSR82_DYNAMIC_PSM (0x2001)

typedef enum 
{
    BT_JSR82_SESSION_IDLE = 0,
    BT_JSR82_SESSION_ACTIVE
}bt_jsr82_session_state;

typedef enum 
{
    BT_JSR82_SESSION_CLIENT = 0,
    BT_JSR82_SESSION_SERVER
}bt_jsr82_session_type;


/* 
 * Internal events used to notify Internal ADP
 * JSR82 Core --> INT ADP
 */
typedef enum
{
	/* Event for MMI */
	JSR82_EVENT_ENABLE_SERVICE_CNF,
	JSR82_EVENT_TURN_ON_SERVICE_CNF,
	JSR82_EVENT_TURN_OFF_SERVICE_CNF,
	JSR82_EVENT_DISABLE_SERVICE_CNF,
	JSR82_EVENT_CONNECT_IND,
	JSR82_EVENT_CONNECT_CNF,
	JSR82_EVENT_DISCONNECT_IND,
	JSR82_EVENT_TX_DATA_CNF,
	JSR82_EVENT_RX_DATA_IND,
	JSR82_EVENT_RX_READY_IND,
	JSR82_EVENT_TX_READY_IND,
	JSR82_EVENT_SET_ACL_SECURITY_CNF,
	JSR82_EVENT_DISCONNECT_ALL_ACL_CNF, 
	JSR82_EVENT_SEND_DATA_REQ, 
	JSR82_EVENT_GET_DATA_REQ, 
	JSR82_EVENT_PUT_BYTES_CNF, 
	JSR82_EVENT_GET_BYTES_CNF, 
	JSR82_EVENT_GET_AVAILABLE_DATA_LENGTH_CNF, 
	JSR82_EVENT_DATA_AVAILABLE_IND, 
	JSR82_EVENT_ASSIGN_BUFFER_CNF
} JSR82_event;

typedef struct _JSR82_event_enable_service_cnf_struct
{   
	U8 result; 
	U8 ps_type;
	U16 channel;
	U8 index;
	U32 identify;
} JSR82_event_enable_service_cnf_struct;

typedef struct _JSR82_event_turn_on_service_cnf_struct
{   
	U8 result;
	U8 ps_type;
	U8 con_id;
	U32 identify;
} JSR82_event_turn_on_service_cnf_struct;

typedef struct _JSR82_event_turn_off_service_cnf_struct
{   
	U8 result;
	U8 ps_type;
	U8 con_id;
	U32 identify;
} JSR82_event_turn_off_service_cnf_struct;

typedef struct _JSR82_event_disable_service_cnf_struct
{   
	U8 ps_type;
	U8 con_id;
	U32 identify;
	U8 result;
} JSR82_event_disable_service_cnf_struct;

typedef struct _JSR82_event_connect_ind_struct
{   
	U8 *bd_addr;
	U8 ps_type;
	U16 mtu;
	U16 in_mtu;
	U32 channel;
	U8 index;
	U32 identify;
	U16 l2cap_id;
	U8 result;
} JSR82_event_connect_ind_struct;

typedef struct _JSR82_event_connect_cnf_struct
{   
	U8 *bd_addr;
	U8 ps_type;
	U16 mtu;
	U16 in_mtu;
	U32 channel;
	U8 index;
	U32 identify;
	U16 l2cap_id;
	U8 result;
} JSR82_event_connect_cnf_struct;

typedef struct _JSR82_event_disconnect_ind_struct
{   
	U8 ps_type;
	U8 con_id;
	U32 identify;
	U16 l2cap_id;
} JSR82_event_disconnect_ind_struct;

typedef struct _JSR82_event_tx_data_cnf_struct
{   
	U8 session_inx;
	U16 l2cap_id;
	U8 result;
} JSR82_event_tx_data_cnf_struct;

typedef struct _JSR82_event_rx_data_ind_struct
{   
	U8 con_id;
	U16 l2cap_id;
	U16 length;
	U8 *data;
} JSR82_event_rx_data_ind_struct;

typedef struct _JSR82_event_rx_ready_ind_struct
{   
	U8 session_inx;
	U16 l2cap_id;
	U16 length;
	U8 ps_type;
} JSR82_event_rx_ready_ind_struct;

typedef struct _JSR82_event_tx_ready_ind_struct
{   
	U8 session_inx;
	U16 l2cap_id;
	U8 ps_type;
	BOOL tx_empty;
} JSR82_event_tx_ready_ind_struct;

typedef struct _JSR82_event_set_acl_security_cnf_struct
{   
	U8 result;
	U32 transaction_id;
	U8 *addr;
} JSR82_event_set_acl_security_cnf_struct;

typedef struct _JSR82_event_send_data_req_struct
{   
	U8 session_inx;
	U16 l2cap_id;
} JSR82_event_send_data_req_struct;

typedef struct _JSR82_event_get_data_req_struct
{   
	U8 session_inx;
} JSR82_event_get_data_req_struct;

typedef struct _JSR82_event_put_bytes_cnf_struct
{   
	U8 ps_type;
	U8 index;
	U16 l2cap_id;
	U16 length;
} JSR82_event_put_bytes_cnf_struct;

typedef struct _JSR82_event_get_bytes_cnf_struct
{   
	U8 ps_type;
	U8 index;
	U16 l2cap_id;
	U16 length;
	U8 *buffAddr;
} JSR82_event_get_bytes_cnf_struct;

typedef struct _JSR82_event_get_available_data_length_cnf_struct
{   
	U8 ps_type;
	U8 index;
	U16 l2cap_id;
	I16 length;
} JSR82_event_get_available_data_length_cnf_struct;

typedef struct _JSR82_event_data_available_ind_struct
{   
	U8 ps_type;
	U8 index;
	U16 l2cap_id;
	U16 length;
#ifndef BTMTK_ON_LINUX
	U8 *buffer;
#endif
} JSR82_event_data_available_ind_struct;

typedef struct _JSR82_event_assign_buffer_cnf_struct
{   
	U8 index;
	U16 l2cap_id;
} JSR82_event_assign_buffer_cnf_struct;

typedef struct _JSR82CallbackParms
{
    JSR82_event event;
    module_type dstMod;

    union
    {
       /* JSR82 Core --> MMI */
    	JSR82_event_enable_service_cnf_struct enable_service_cnf;
       JSR82_event_turn_on_service_cnf_struct turn_on_service_cnf;
       JSR82_event_turn_off_service_cnf_struct turn_off_service_cnf;
       JSR82_event_disable_service_cnf_struct disable_service_cnf;
       JSR82_event_connect_ind_struct connect_ind;
       JSR82_event_connect_cnf_struct connect_cnf;
       JSR82_event_disconnect_ind_struct disconnect_ind;
       JSR82_event_tx_data_cnf_struct tx_data_cnf;
       JSR82_event_rx_data_ind_struct	rx_data_ind;
       JSR82_event_rx_ready_ind_struct rx_ready_ind;
       JSR82_event_tx_ready_ind_struct tx_ready_ind;
       JSR82_event_set_acl_security_cnf_struct set_acl_security_cnf;
       JSR82_event_send_data_req_struct send_data_req;
       JSR82_event_get_data_req_struct get_data_req;
       JSR82_event_put_bytes_cnf_struct put_bytes_cnf;
       JSR82_event_get_bytes_cnf_struct get_bytes_cnf;
       JSR82_event_get_available_data_length_cnf_struct get_available_data_length_cnf;
       JSR82_event_data_available_ind_struct data_available_ind;
       JSR82_event_assign_buffer_cnf_struct assign_buffer_cnf;
    } p;
}JSR82CallbackParms;


typedef void (*JSR82Callback) (JSR82CallbackParms *Parms);

typedef struct _BT_JSR82_L2cap_struct_t
{
    U8                        l2cap_con_state;
    U16                     l2capLocalCid;
    L2CAP_Channel *channel;
} BT_JSR82_L2cap_struct_t;

typedef struct _BT_JSR82_Packet_struct_t
{
	ListEntry				node;
	U8						data[JSR82_SESSION_MAX_TX_BUFSIZE];
} BT_JSR82_Packet_struct_t;


typedef struct _BT_JSR82_SESSION_Buffer_struct_t
{
    U8 *mainRecord;
    U8                      data[JSR82_SESSION_MAX_RX_DATA];
    CmgrHandler         	cmgrHandler;
	SdpQueryToken			sdpQueryToken;  // for sdp query
	
    RfChannel             	rfc;
    /* For L2CAP, it use session_buffer's l2capCtx field's l2cap_con_state to determine the connection is still active or not */
    BT_JSR82_L2cap_struct_t l2capCtx;
    U16 highWaterMarkValue; /* Add for SPP session*/
    U8   devCredit; /* Add for SPP session */
} BT_JSR82_SESSION_Buffer_struct_t;

typedef struct _BT_JSR82_SESSION_AP_Record_Context_struct_t
{
    bt_jsr82_session_type client_server;
    U8 bd_addr[6];
    U8 ps_type;
    U32 channel;
    U8 con_id;  /* Note by mtk01411: con_id is the same as session_inx of the array session[] */
    U32 identify;

	U8 uuid_128[16];
    /* channel_state's value: IDLE or OPEN is valid only for RFCOMM to determine the connection is still active or not 
      * For L2CAP, this channel_state must always be IDLE, and it use session_buffer's l2capCtx field's l2cap_con_state to determine the connection is still active or not 
      */
    U8 channel_state;
    /* For server role: this status value is ACTIVE when enable service and IDLE when disable service 
      * For client role: this status valus is ACTIVE when trying to setup connection request and IDLE when setup failure or disconnected
      */
    bt_jsr82_session_state status; 
    U8 *session_buffer;
    U8 authorize;
    BtPacket txPacket[JSR82_SESSION_TX_PKT_QUEUE_SIZE];
    ListEntry ApTxQueue;
	U8 *txPktPool;
    ListEntry ApTxPktList;
    U8 security_value;
    U16 mtu;
    /* Add 2007-1214: For L2EVENT_DISCONNECTED, the chnl returned from L2CAP_GetValidChannel() is NULL due to channel->state is CLOSED 
      * This field is used for bt_jsr82_ACLRemoveLink() usage and kept in bt_jsr82_ACLAddLink()
      */
    BtRemoteDevice* link;
    /* Bingyi_2010.03.26: Add ring buffer in core context */
    RingBuf WriteRingBuf;
    RingBuf ReadRingBuf;
    kal_bool is_tx_empty;
    kal_bool notifyReadyToWrite; 
    kal_bool readyToRead;
    U16 wait_data_available_ind_rsp_length;	// Record if DATA_AVAILABLE_IND has been sent to MMI; Update this when DATA_AVAILABLE_IND_RSP is received.
    U8 *deliverBufPtr;		// Buffer pointer used for DATA_AVAILABLE_IND (INT ==> EXT)
    U16 deliverBufSize;	// Size of deliver buffer
#ifdef BTMTK_ON_LINUX
    U8 *deliverPutBufPtr;	// Deliver Put buffer pointer (EXT ==> INT)
    EvmTimer channelTimer;
#endif


} BT_JSR82_SESSION_AP_Record_Context_struct_t;

typedef struct _BT_JSR82_L2CAP_PSM_struct_t
{
    L2capPsm             l2cap;
    BtSecurityRecord   l2capSecRecord;
    U8                          used;
    U8                          used_no;
    U8                          security_level;
    /* 2010.04.28 Bingyi: Add this attribute to distinguish between Server & Client with the same PSM channel. */
    bt_jsr82_session_type client_server;
}BT_JSR82_L2CAP_PSM_struct_t;


typedef struct _BT_JSR82_ACL_LINK_struct_t
{
    U8                          used;
    BtRemoteDevice              *link;
    U8                          used_no;
    U8                          authentication_link_state;
    U8                          encrypt_link_state;
    U32                        authentication_transaction_id;
}BT_JSR82_ACL_LINK_struct_t;


typedef struct _BT_JSR82_SESSION_AP_CONTEXT_struct_t
{
    BT_JSR82_SESSION_AP_Record_Context_struct_t session[JSR82_MAX_SESSION_NO];
    BT_JSR82_L2CAP_PSM_struct_t l2cap_psm_info[JSR82_MAX_PSM_NO];
    BT_JSR82_ACL_LINK_struct_t active_link[JSR82_MAX_ACL_NO];
    U32 l2cap_psm_usage_table;
    U32 rfcomm_channel_usage_table;
    EvmTimer jsr82_disconnect_all_acl;
    U8  jsr82_disconnect_all_acl_counter;
    U8  vm_request_disc_all_link_but_pending_conreq_no;
    BtRemoteDevice* vm_disc_link_ptr;
    L2capPsm             dummyL2capPsm;
    /* Add to decrease the frequency of disc and con req op at same time (delay setup connection request usage) : 2008-0321 */
    EvmTimer jsr82_con_req_timer;
    bt_jsr82_connect_req_struct jsr82_con_req;
    EvmTimer jsr82_sdp_req_timer;
    bt_bm_search_raw_req_struct jsr82_sdp_req;
} BT_JSR82_SESSION_AP_CONTEXT_struct_t;


extern BT_JSR82_SESSION_AP_CONTEXT_struct_t bt_jsr82_session_ap_context;
#define BTJ82SEAP(f)  (bt_jsr82_session_ap_context.f)


void JSR82_Register(JSR82Callback Callback);
void bt_jsr82_init_session_buffer(BT_JSR82_SESSION_Buffer_struct_t* session_buf);
void BTJSR82_SessionApInit(void);
void BTJSR82_DeInitAll(void);
void bt_jsr82_HandleJSR82Deactivate(void);
U8 bt_jsr82_get_L2capPSMIndex(U16 channel, U16 mtu, U8 security_level, bt_jsr82_session_type client_server);
U8 bt_jsr82_SearchL2capContext(U16 l2cap_id);
BtStatus bt_jsr82_AddNewL2capToContext(BT_JSR82_SESSION_Buffer_struct_t *ptr, U16 l2cap_id);
BtStatus bt_jsr82_AddCreateL2capToContext(BT_JSR82_SESSION_Buffer_struct_t *ptr, U16 l2cap_id);
void bt_jsr82_ReleaseL2capFromContext(BT_JSR82_SESSION_Buffer_struct_t *ptr, U16 l2cap_id);


void bt_jsr82_L2capRegisterSecurity(BtSecurityRecord *l2capSecRecord, U8 sec_value, U16 channel);
U8 bt_jsr82_find_L2capPSMInContext(U16 channel, U16 L2capChannelId);
U8 bt_jsr82_find_L2capPSMListenInContext(U16 channel, U16 L2capChannelId);
void bt_jsr82_free_L2capPSMIndex(U32 channel, U8 secruity_level, bt_jsr82_session_type client_server);
void bt_jsr82_ClearSessionContext(U8 session_inx);
void bt_jsr82_ACLAddLink(BtRemoteDevice *link);
void bt_jsr82_ACLRemoveLink(BtRemoteDevice *link);
void bt_jsr82_ACLCheckAndDisconnectAclNo(void);

void bt_jsr82_ACLCheckDisconnectTimer(EvmTimer *Timer);
void bt_jsr82_HandleConReqTimeoutHdlr(EvmTimer* timer);
void BTJSR82_RfCallbackFunc(RfChannel *chan, RfCallbackParms *parms);
void BTJSR82_L2capCallback(L2capChannelId L2ChannelId, L2capCallbackParms *info);
void BTJSR82_L2CapDummyCallback(L2capChannelId L2ChannelId, L2capCallbackParms * info);
void bt_jsr82_CmgrClientCallback(CmgrHandler *Handler, CmgrEvent Event,
                                  BtStatus Status);

void bt_jsr82_CmgrServerCallback(CmgrHandler *Handler, CmgrEvent Event,
                                  BtStatus Status);

void bt_jsr82_HandleSessionApEnableService(bt_jsr82_enable_service_req_struct *ptr);
void bt_jsr82_HandleSessionApConnectReq(bt_jsr82_connect_req_struct *ptr);
void bt_jsr82_HandleSessionApRfcommConnectReq(bt_jsr82_connect_req_struct *ptr);
void bt_jsr82_HandleSessionApL2capConnectReq(bt_jsr82_connect_req_struct *ptr);

void bt_jsr82_HandleSessionApConnectRsp(bt_jsr82_connect_rsp_struct *ptr);
void bt_jsr82_HandleSessionApDisConnectReq(bt_jsr82_disconnect_req_struct *ptr);
void bt_jsr82_HandleSessionApRxCfn(bt_jsr82_rx_data_rsp_struct *ptr);
void bt_jsr82_HandleSessionApTxReq(bt_jsr82_tx_data_req_struct*ptr);
void bt_jsr82_HandleSessionApDisableService(bt_jsr82_disable_service_req_struct *ptr);

void bt_jsr82_HandleSessionApTurnOnService(bt_jsr82_turnon_service_req_struct *ptr);
void bt_jsr82_HandleSessionApTurnOffService(bt_jsr82_turnoff_service_req_struct *ptr);

U32 bt_jsr82_SessionGetFreePSMIndex(void);
void bt_jsr82_SessionReleasePSMIndex(U32 psm_id);

U32 bt_jsr82_SessionGetFreeServChnIndex(void);

void bt_jsr82_SessionReleaseServChnIndex(U32 channel);

U8 bt_jsr82_getCurrentCredit(U8 session_inx);
	
/* Add by mtk01411: for SPP credit calculation */    
void bt_jsr82_advanceCredit(U8 session_inx, kal_bool check_rx_bytes);
/* Add by mtk01411: for SPP connection*/
void bt_jsr82_HandleSessionApGetSppDataReq(bt_jsr82_spp_get_data_req_struct* ptr);
/* Add by mtk01411: for SPP connection*/
void bt_jsr82_sendToRfComm(U8 session_inx);
void bt_jsr82_send(U8 index, U16 l2cap_id);
void bt_jsr82_HandleAuthenticationCnf(const BtEvent * Event);
void bt_jsr82_HandleAuthenticated(const BtEvent * Event);
void bt_jsr82_HandleSetACLSecurityReq(bt_jsr82_set_acl_security_req_struct* ptr);

/* Bingyi_2010.03.26: Buffer Management related */
kal_int16 jsr82_session_RxBytes(kal_uint8 session_type, kal_uint8 session_inx, kal_uint16 subsession_id);
kal_int16 jsr82_session_RxFree(kal_uint8 search_type, kal_uint8 session_type, kal_uint16 subsession_id, kal_uint32 value);
kal_int8 jsr82_session_DevRX(
            kal_uint8 session_type,
            kal_uint8 session_inx,
            kal_uint16 subsession_id,
            char buf[],
            U16 *len);
void jsr82_session_CheckReadyToWrite(U8 session_type, U8 session_inx, kal_uint16 subsession_id, void *one_session);
void jsr82_session_ReturnBuf(kal_uint8 session_type, kal_uint8 session_inx, kal_uint16 subsession_id, char *buf, U16 len);
kal_int8 jsr82_session_DevTX(
            kal_uint8 session_type,
            kal_uint8 session_inx,
            kal_uint16 subsession_id,
            char **buf,
            U16 *len);
void jsr82_session_DevTxEmpty(kal_uint8 session_type, kal_uint8 session_inx, kal_uint16 subsession_id);
kal_int8 jsr82_session_allocate_RWRingBuf(kal_uint8 session_type, U8 index);
kal_uint8 jsr82_session_free_RWRingBuf(U8 index);
kal_int16 jsr82_session_PutBytes(bt_jsr82_put_bytes_req_struct *ptr);
kal_int16 jsr82_session_GetBytes(bt_jsr82_get_bytes_req_struct *ptr);
kal_int16 jsr82_session_BufFreeSpace(
            kal_uint8 session_type,
            kal_uint8 session_inx,
            kal_uint16 subsession_id,
            kal_uint8 buf_type);
kal_int16 jsr82_session_BufAvailableDataLen(
            kal_uint8 session_type,
            kal_uint8 session_inx,
            kal_uint16 subsession_id,
            kal_uint8 buf_type);
kal_int16 jsr82_session_BufSize(kal_uint8 session_type);
void jsr82_session_SendDataAvailableInd(U8 index);
BOOL jsr82_session_DataRemainInBuff(U8 session_type, U8 session_inx, kal_uint16 l2cap_id);
void jsr82_session_DataAvailableIndRsp(bt_jsr82_data_available_ind_rsp_struct *ptr);
void jsr82_session_AssignDeliverBufferReq(bt_jsr82_assign_buffer_req_struct *ptr);

#ifdef BTMTK_ON_LINUX
void *jsr82_core_malloc_ashm(char *memName, U8 index, U8 type);
void jsr82_core_free_ashm(U8 index, U8 type);
#endif

BtStatus bt_jsr82_sdp_query(BT_JSR82_SESSION_Buffer_struct_t *session_buffer, U8 *uuid_128);
void bt_jsr82_rfcomm_connect(BT_JSR82_SESSION_AP_Record_Context_struct_t *session);



#endif /* BT_JSR82_ENABLED || defined(__BT_JSR82_L2RF__) */
#endif 	/* __BT_JSR82_SESSION_H */
