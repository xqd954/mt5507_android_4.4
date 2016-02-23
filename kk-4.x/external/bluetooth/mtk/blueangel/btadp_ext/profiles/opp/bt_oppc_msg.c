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
 * bt_oppc_msg.c
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
#include "bt_goep_porting.h"
#include "bt_mmi_msg.h"

#include "bluetooth_gap_struct.h"
#include "bluetooth_struct.h"
#include "bt_os_api.h"
#include "ext_osal.h"
#include "ext_nbuffer.h"
//#include "goep.h"

#include "bt_opp_comm.h"
#include "bt_oppc_api.h"
#include "bt_opps_api.h"
#include "bt_oppc_hdl.h"
#include "bt_opps_hdl.h"
#include "ext_ucs2.h"
#include "bt_ext_debug.h"

#include "stdio.h"

/*
 * OPP Client Out Event MMI -> OBEX
 */

/*****************************************************************************
 * FUNCTION
 *  bt_oppc_connect_req
 * DESCRIPTION
 *  This function is to handle OPP client OBEX connect request
 * PARAMETERS
 *  sc_authorise_ind_p      [?]
 *  a(?)                    [IN/OUT]        First variable, used as returns
 *  b(?)                    [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void bt_oppc_connect_req(U16 opp_client_max_packet_len, btbm_bd_addr_t dest_addr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    goep_connect_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req = (goep_connect_req_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(*req), TD_CTRL);
    GOEP_MEMSET((U8*)req, 0, sizeof(goep_connect_req_struct));

	req->uuid[0] = '\0';
	req->uuid_len = 0;
	//GOPC(opc_goep_req_id) = GOPC(req_id++);
	GOPC(opc_goep_req_id) = GOPC(req_id);   //For ALPS00270904, each obex-based profile use req_id for aborting while connecting.
	req->req_id = GOPC(opc_goep_req_id);
	req->bd_addr.lap = (U32)dest_addr.lap;
	req->bd_addr.uap = (U8)dest_addr.uap;
	req->bd_addr.nap = (U16)dest_addr.nap;
	/*goep_connect_req->mru*/
	req->tp_type = GOEP_TP_BT;
	req->buf_ptr = (U8*)(GOPC(opc_push_buff));
    req->buf_size = opp_client_max_packet_len;
    req->auth_use = KAL_FALSE;
    req->client_pwd_len = 0;
    req->client_realm_len = 0;

    OPP_API_TRANSFER( MSG_ID_BT_OPPC_CONNECT_REQ, req, sizeof(goep_connect_req_struct) );
    //printf( "connect_req id %d\n", MSG_ID_BT_OPPC_CONNECT_REQ);
    //BT_SendMessage(MSG_ID_BT_OPPC_CONNECT_REQ, 0, req);
}


/*****************************************************************************
 * FUNCTION
 *  bt_oppc_abort_req
 * DESCRIPTION
 *  This function is to handle OPP client OBEX abort put request
 * PARAMETERS
 *  sc_authorise_ind_p      [?]
 *  a(?)                    [IN/OUT]        First variable, used as returns
 *  b(?)                    [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void bt_oppc_abort_req(U8 goep_conn_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    goep_abort_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req = (goep_abort_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_abort_req_struct), TD_CTRL);
    GOEP_MEMSET((U8*) req, 0, sizeof(req));

    req->goep_conn_id = goep_conn_id;

    OPP_API_TRANSFER( MSG_ID_BT_OPPC_ABORT_REQ, req , sizeof(goep_abort_req_struct));
}


/*****************************************************************************
 * FUNCTION
 *  bt_oppc_push_req
 * DESCRIPTION
 *  This function is to handle OPP client OBEX push request
 * PARAMETERS
 *  sc_authorise_ind_p      [?]
 *  a(?)                    [IN/OUT]        First variable, used as returns
 *  b(?)                    [IN]            Second variable
 * RETURNS
 *  void
 *****************************************************************************/
void bt_oppc_push_req(U8 goep_conn_id, goep_pkt_type_enum pkt_type, U32 total_obj_len, S8 *obj_name, S8 *obj_mime, U32 *frag_ptr, U16 frag_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    goep_push_req_struct *req;
    U16 i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_OPP_LOGD("[EXT] bt_oppc_push_req: conn_id(%d), mime_type(%s), obj_name(%s), total_bytes(%d), ptr(%d)", goep_conn_id, obj_mime, obj_name, total_obj_len, frag_ptr);

    req = (goep_push_req_struct*) CONSTRUCT_LOCAL_PARAM(sizeof(goep_push_req_struct), TD_CTRL);
    GOEP_MEMSET((U8*)req, 0, sizeof(goep_push_req_struct));

    req->goep_conn_id = goep_conn_id;
    req->pkt_type = pkt_type;
    req->put_type = GOEP_PUT_NORMAL;
    GOEP_MEMCPY(req->obj_mime_type, obj_mime, GOEP_MAX_MIME_TYPE);
#if 1
    for (i = 0; i < GOEP_MAX_OBJ_NAME; i++)
    {
        req->obj_name[i] = obj_name[i+1];
        req->obj_name[i+1] = obj_name[i];
        i++;
    }
#else
    ext_ucs2ncpy( req->obj_name, obj_name, sizeof(req->obj_name)  );
#endif
    req->total_obj_len = total_obj_len;
    req->frag_ptr = (U8*)frag_ptr;
    req->frag_len = frag_len;

    OPP_API_TRANSFER( MSG_ID_BT_OPPC_PUSH_REQ, req , sizeof(goep_push_req_struct));
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
void bt_oppc_disconnect_req(U8 goep_conn_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_disconnect_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req = (goep_disconnect_req_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(*req), TD_CTRL);
    GOEP_MEMSET((U8*)req, 0, sizeof(goep_disconnect_req_struct));
    req->goep_conn_id = goep_conn_id;

    /* This request ID is only useful while aborting connecting request */
    req->req_id = GOPC(opc_goep_req_id);
    req->uuid[0] = '\0';
    req->uuid_len = 0;

    OPP_API_TRANSFER( MSG_ID_BT_OPPC_DISCONNECT_REQ, req , sizeof(goep_disconnect_req_struct));
}


void bt_oppc_register_client_req(U8 goep_conn_id, U32 count, U32 timetick){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_register_client_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req = (goep_register_client_req_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(*req), TD_CTRL);
    GOEP_MEMSET((U8*)req, 0, sizeof(goep_register_client_req_struct));
    req->goep_conn_id = goep_conn_id;

    /* This request ID is only useful while aborting connecting request */
    req->req_id = GOPC(opc_goep_req_id);
    req->uuid[0] = '\0';
    req->uuid_len = 0;
    req->req_id = (U8) count;
    req->timeticker = timetick;

    OPP_API_TRANSFER( MSG_ID_BT_GOEPC_REGISTER_CLIENT_REQ, req , sizeof(goep_register_client_req_struct));

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
void bt_opps_tpdisconnect_req(U8 goep_conn_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    goep_disconnect_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req = (goep_disconnect_req_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(goep_disconnect_req_struct), TD_CTRL);
    GOEP_MEMSET((U8*)req, 0, sizeof(goep_disconnect_req_struct));
    req->goep_conn_id = goep_conn_id;

    /* This request ID is only useful while aborting connecting request */
    req->req_id = GOPC(opc_goep_req_id);

    OPP_API_TRANSFER( MSG_ID_BT_OPPC_DISCONNECT_REQ, req , sizeof(goep_disconnect_req_struct));
}


void bt_oppc_pull_req(U8 goep_conn_id, goep_pkt_type_enum pkt_type, U8 *ucName, U8 *ucMime){
    goep_pull_req_struct *req;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    req = (goep_pull_req_struct*)CONSTRUCT_LOCAL_PARAM(sizeof(goep_pull_req_struct), TD_CTRL);
    GOEP_MEMSET((U8*)req, 0, sizeof(goep_pull_req_struct));

    req->goep_conn_id = GOPC(opc_goep_conn_id);

    ///MIME is ASCII string
    if( NULL != ucName ){
        /// copy ucs2
        ext_ucs2ncpy((S8 *)req->obj_name, (S8 *)ucName, GOEP_MAX_OBJ_NAME/sizeof(U16));
    }
    if( NULL != ucMime){
        /// copy ascii
        ext_strncpy((S8 *)req->obj_mime_type, (S8 *)ucMime, GOEP_MAX_MIME_TYPE);
    }

    req->pkt_type = pkt_type;

    OPP_API_TRANSFER( MSG_ID_BT_OPPC_PULL_REQ, req , sizeof(goep_pull_req_struct));
}


void bt_oppc_send_msg_toself(U32 msg_id, void *p_local_para, U32 u4Size)
{
    btmtk_goep_show_id( msg_id, TRUE, MOD_MMI);
    BTCMD_SendMessage(msg_id, MOD_EXTERNAL, p_local_para,(U16) u4Size ) ;

    if( NULL != p_local_para ){
        FREE_LOCAL_PARA((void*)p_local_para);
    }
}

