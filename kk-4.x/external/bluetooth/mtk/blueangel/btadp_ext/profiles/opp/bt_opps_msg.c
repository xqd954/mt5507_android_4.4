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

/*****************************************************************************
 *
 * Filename:
 * ---------
 * bt_opps_msg.c
 *
 * Project:
 * --------
 *
 *
 * Description:
 * ------------
 *   This file is for External OPP ADP to send msg to BT Queue
 *
 * Author:
 * -------
 * Daylong Chen
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 04 24 2012 dexiang.jiang
 * [ALPS00270904] [ICS TDD][BT] happen NE at system/bin/mtkbt
 * Use special req_id(large value) to pervent multi obex-based profiles conexistence conflict.
 *
 * 09 03 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integration BT solution.
 *

 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
 /// Basic header
#include "bt_goep_porting.h"

#include "bt_os_api.h"
#include "ext_osal.h"
#include "ext_nbuffer.h"
#include "ext_ucs2.h"
#include "bt_goep_porting.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_sdp_struct.h"
#include "bluetooth_struct.h"

#include "bt_opp_comm.h"
#include "bt_oppc_api.h"
#include "bt_opps_api.h"
#include "bt_oppc_hdl.h"
#include "bt_opps_hdl.h"
#include "bt_opp_comm.h"
#include "bt_ext_debug.h"

#define BT_BTH_OBEX_OBJECT_PUSH_SERVICE_UUID      (0x1105 )

/*****************************************************************************
 * FUNCTION
 *  bt_opps_obex_register_server_req
 * DESCRIPTION
 *  This function is to register OBEX server instance
 * PARAMETERS
 *  void
 *  a(?)        [IN/OUT]        First variable, used as returns
 *  b(?)        [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void bt_opps_obex_register_server_req(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_register_server_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_OPP_LOGD("[EXT] bt_opps_obex_register_server_req");

    req = (goep_register_server_req_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(*req), TD_CTRL);

    //GOPS(ops_goep_req_id) = GOPS(req_id++);
    GOPS(ops_goep_req_id) = GOPS(req_id);   //For ALPS00270904
    req->req_id = GOPS(ops_goep_req_id);
    req->uuid[0] = '\0';
    req->uuid_len = 0;
    req->tp_type = GOEP_TP_BT;
    req->need_auth = KAL_FALSE;
    req->buf_ptr = (U8*)GOPS(ops_push_buff);
    req->buf_size = (U16) GOPS(ops_push_buff_size); //TODO: workaround. the pc uart driver has some bug.

    /* compose message */
	OPP_API_TRANSFER( MSG_ID_BT_OPPS_REGISTER_SERVER_REQ, req, sizeof(goep_register_server_req_struct) );
}


/*****************************************************************************
 * FUNCTION
 *  bt_opps_obex_deregister_server_req
 * DESCRIPTION
 *  This function is to deactivate OPP server service
 * PARAMETERS
 *  void
 *  a(?)        [IN/OUT]        First variable, used as returns
 *  b(?)        [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void bt_opps_obex_deregister_server_req(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //QQQ ilm_struct *ilm_ptr;
    goep_deregister_server_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req = (goep_deregister_server_req_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(*req), TD_CTRL);

    req->goep_conn_id = GOPS(ops_goep_conn_id);

    OPP_API_TRANSFER( MSG_ID_BT_OPPS_DEREGISTER_SERVER_REQ, req , sizeof(goep_deregister_server_req_struct));
}


/*****************************************************************************
 * FUNCTION
 *  bt_opps_sdpdb_deregister_req
 * DESCRIPTION
 *  This function is to deregister OPP SDP service record
 * PARAMETERS
 *  void
 *  a(?)        [IN/OUT]        First variable, used as returns
 *  b(?)        [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void bt_opps_sdpdb_deregister_req(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //QQQ ilm_struct *ilm_ptr;
    bt_sdpdb_deregister_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req = (bt_sdpdb_deregister_req_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(*req), TD_CTRL);
    //GOEP_MEMSET( &req, 0, sizeof(bt_sdpdb_register_req_struct));
    req->uuid = BT_BTH_OBEX_OBJECT_PUSH_SERVICE_UUID;

    /* compose msg */
    ///TODO Use the sdpdb register msg
    OPP_API_TRANSFER( MSG_ID_BT_SDPDB_REGISTER_REQ, req, sizeof(bt_sdpdb_deregister_req_struct));
}


/*****************************************************************************
 * FUNCTION
 *  bt_opps_connect_rsp
 * DESCRIPTION
 *  This function is to handle OPP server OBEX connect response
 * PARAMETERS
 *  sc_authorise_ind_p      [?]
 *  a(?)                    [IN/OUT]        First variable, used as returns
 *  b(?)                    [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void bt_opps_connect_rsp(U8 goep_conn_id, U8 rsp_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_connect_rsp_struct *rsp;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    rsp = (goep_connect_rsp_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(*rsp), TD_CTRL);

    rsp->goep_conn_id = goep_conn_id;
    rsp->rsp_code = rsp_code;

    OPP_API_TRANSFER( MSG_ID_BT_OPPS_CONNECT_RSP, rsp , sizeof(goep_connect_rsp_struct));
}


/*****************************************************************************
 * FUNCTION
 *  bt_opps_push_rsp
 * DESCRIPTION
 *  This function is to handle OPP server OBEX push response
 * PARAMETERS
 *  sc_authorise_ind_p      [?]
 *  a(?)                    [IN/OUT]        First variable, used as returns
 *  b(?)                    [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void bt_opps_push_rsp(U8 goep_conn_id, U8 rsp_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_push_rsp_struct *rsp;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    rsp = (goep_push_rsp_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(*rsp), TD_CTRL);

    rsp->goep_conn_id = goep_conn_id;
    rsp->rsp_code = rsp_code;

    OPP_API_TRANSFER( MSG_ID_BT_OPPS_PUSH_RSP, rsp , sizeof(goep_push_rsp_struct));
}


/*****************************************************************************
 * FUNCTION
 *  bt_opps_pull_rsp
 * DESCRIPTION
 *  This function is to handle OPP server OBEX pull response
 * PARAMETERS
 *  sc_authorise_ind_p      [?]
 *  a(?)                    [IN/OUT]        First variable, used as returns
 *  b(?)                    [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void bt_opps_pull_rsp(U8 ops_goep_conn_id, U8 rsp_code, goep_pkt_type_enum type, U32 total, U32 len, const S8 *mime)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_pull_rsp_struct *rsp;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    rsp = (goep_pull_rsp_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(*rsp), TD_CTRL);

    rsp->goep_conn_id = (U8)ops_goep_conn_id;
    rsp->rsp_code = (U8)rsp_code;
    rsp->total_obj_len = total;
    rsp->pkt_type = (U16) type;
    rsp->frag_len = (U16) len;
    rsp->frag_ptr = (U8*) GOPS(ops_push_buff);
    if(NULL == mime){
        ext_strncpy( (S8*)rsp->obj_mime_type, "", GOEP_MAX_MIME_TYPE );
    }else{
        ext_strncpy( (S8*)rsp->obj_mime_type, mime, GOEP_MAX_MIME_TYPE );
    }

    OPP_API_TRANSFER( MSG_ID_BT_OPPS_PULL_RSP, rsp , sizeof(goep_pull_rsp_struct));
}


/*****************************************************************************
 * FUNCTION
 *  bt_opps_authorize_rsp
 * DESCRIPTION
 *  This function is to handle OPP server OBEX authorize response
 * PARAMETERS
 *  sc_authorise_ind_p      [?]
 *  a(?)                    [IN/OUT]        First variable, used as returns
 *  b(?)                    [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void bt_opps_authorize_rsp(U8 ops_goep_conn_id, U8 rsp_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_authorize_rsp_struct *rsp;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    rsp = (goep_authorize_rsp_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(*rsp), TD_CTRL);

    rsp->goep_conn_id = (U8)ops_goep_conn_id;
    rsp->rsp_code = (U8)rsp_code;

    OPP_API_TRANSFER( MSG_ID_BT_OPPS_AUTHORIZE_RSP, rsp , sizeof(goep_authorize_rsp_struct));
}

/*****************************************************************************
 * FUNCTION
 *  bt_opps_abort_rsp
 * DESCRIPTION
 *  This function is to handle OPP server OBEX abort response
 * PARAMETERS
 *  sc_authorise_ind_p      [?]
 *  a(?)                    [IN/OUT]        First variable, used as returns
 *  b(?)                    [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void bt_opps_abort_rsp(U8 goep_conn_id, U8 rsp_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_abort_rsp_struct *rsp;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    rsp = (goep_abort_rsp_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(*rsp), TD_CTRL);

    rsp->goep_conn_id = goep_conn_id;
    rsp->rsp_code = rsp_code;

    OPP_API_TRANSFER( MSG_ID_BT_OPPS_ABORT_RSP, rsp , sizeof(goep_abort_rsp_struct));
}


/*****************************************************************************
 * FUNCTION
 *  bt_oppc_disconnect_req
 * DESCRIPTION
 *  This function is to handle OPP client OBEX disconnect request
 * PARAMETERS
 *  sc_authorise_ind_p      [?]
 *  a(?)                    [IN/OUT]        First variable, used as returns
 *  b(?)                    [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void bt_opps_disconnect_req(U8 goep_conn_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_disconnect_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req = (goep_disconnect_req_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(*req), TD_CTRL);
    GOEP_MEMSET( (U8 *)req, 0, sizeof(goep_disconnect_req_struct));
    req->goep_conn_id = goep_conn_id;

    /* This request ID is only useful while aborting connecting request */
    req->req_id = GOPS(ops_goep_req_id);
    req->uuid[0] = '\0';
    req->uuid_len = 0;

    OPP_API_TRANSFER( MSG_ID_BT_GOEPC_DISCONNECT_REQ, req , sizeof(goep_disconnect_req_struct));
}

void bt_opps_send_msg_toself(U32 msg_id, void *p_local_para, U32 u4Size)
{
    btmtk_goep_show_id( msg_id, TRUE, MOD_MMI);
    BTCMD_SendMessage(msg_id, MOD_EXTERNAL, p_local_para,(U16) u4Size ) ;

    if( NULL != p_local_para ){
        FREE_LOCAL_PARA((void*)p_local_para);
    }

}

