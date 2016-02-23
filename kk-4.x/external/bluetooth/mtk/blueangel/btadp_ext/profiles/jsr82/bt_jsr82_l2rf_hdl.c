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
 * bt_jsr82_hdl.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Bingyi Chen
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

/* 
 * bt_jsr82_hdl.c
 * 
 * This file is the External Adaptation handler of JSR82 used by VM.
 * Flow direction: VM <-- external ADP handler
 */

//#include "bt_common.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_jsr82_message.h"
#include "bluetooth_jsr82_struct.h"
#include "bt_jsr82_hdl.h"
#include "bt_jsr82_sdp.h"
#include "bt_struct.h"
#include "bt_jbt_interface.h"
#include "bt_jbt_internal.h"
//#include "bt_osal.h"
#include "bt_jsr82_api.h"
#include "memory.h"
#include "bt_types.h"
#include "bt_ext_debug.h"

 
/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/


/****************************************************************************
 *
 * Local function prototypes
 *
 ****************************************************************************/


/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/
#define JSR82_SESSION_CONNECT_ACCEPT   (0x01)
#define JSR82_SESSION_CONNECT_REJECT   (0x02)

extern BTMTK_EventCallBack g_JSR82_MMI_Callback;
extern JBT_L2CAP_CNTX jbt_l2cap_context;
extern JBT_SPP_CNTX jbt_spp_context;
extern int get_transaction_id();
extern JBT_JSR82_CNTX jbt_jsr82;


/*****************************************************************************
 * FUNCTION
 *  jbt_handle_session_enabled_result
 * DESCRIPTION
 *  
 * PARAMETERS
 *  msg     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jbt_handle_session_enabled_result(ilm_struct *message)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //kal_uint32 transaction_id;
    jbt_session_registration_result parms;
    jbt_list *existing_entry = NULL;
    jbt_session_info *session_entry = NULL;
    kal_uint8 list_type = 0;
    bt_jsr82_enable_service_cnf_struct *msg;
    int rc = 0;
    jbt_subsession *subsession_entry = NULL;
	BT_BOOL ret = FALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_ext_log("[JSR82][JBT] JBT jbt_handle_session_enabled_result");
    msg = (bt_jsr82_enable_service_cnf_struct *)message->ilm_data;
	
	session_entry = jbt_search_existing_session_entry(msg->ps_type, 
													JBT_LIST_SEARCH_TYPE_BY_TRANSACTION_ID, 
													msg->identify);
    if (session_entry == NULL)
    {
		bt_ext_log("[JSR82][JBT] JBT can not find an existing entry with type=%d and value=%d", JBT_LIST_SEARCH_TYPE_BY_TRANSACTION_ID, msg->identify);
        return;
    }

	list_type = jbt_convert_list_type(msg->ps_type);
	
    subsession_entry = jbt_search_existing_subsession_entry(session_entry, JBT_LIST_SEARCH_TYPE_BY_TRANSACTION_ID, msg->identify);

    bt_ext_log("[JSR82][JBT] JBT msg->result:%02x", msg->result);

    if (msg->result != JSR82_SESSION_REGISTRARION_SUCCESS)
    {
        int i = 0;
		jbt_reset_session_entry(session_entry);		
    }
    else
    {
		/* Keep the session_inx for this server session:2007-1103 */
        session_entry->index = msg->index;
        /* Kepp the channel number registered by this serice: 2007-1107 */
        session_entry->psm_channel = msg->channel;
		session_entry->sdpinfo.channel = msg->channel;
		
		//register sdp record
		ret = btmtk_jsr82_sdp_registration(&session_entry->sdpinfo);
		if (!ret)
		{
			jbt_session_general_service_deregistration(session_entry);
		}
    }

}


/*****************************************************************************
 * FUNCTION
 *  jbt_handle_session_deregistration_result
 * DESCRIPTION
 *  
 * PARAMETERS
 *  msg     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jbt_handle_session_deregistration_result(ilm_struct *message)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //kal_uint32 transaction_id;
    jbt_session_deregistration_result parms;
    jbt_list *existing_entry = NULL;
    jbt_session_info *session_entry = NULL;
    jbt_subsession *subsession_entry = NULL;
    bt_jsr82_disable_service_cnf_struct *msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	msg = (bt_jsr82_disable_service_cnf_struct *)message->ilm_data;
    bt_ext_log("[JSR82][JBT] JBT jbt_handle_session_deregistration_result msg->ps_type:%02x", msg->ps_type);

	if (msg->ps_type == 0x00)
    {
        parms.result = JBT_CMD_FAILED;
        goto report_it;
    }
   	session_entry = jbt_search_existing_session_entry(msg->ps_type, 
													JBT_LIST_SEARCH_TYPE_BY_INX, 
													msg->index);
    if (session_entry == NULL)
    {
		bt_ext_log("[JSR82][JBT] JBT can not find an existing entry with type=%d and value=%d", JBT_LIST_SEARCH_TYPE_BY_INX, msg->index);
        return;
    }
		
    /* Check result */
    if (msg->result == BT_JSR82_SESSION_DISABLED_SUCCESS)
    {
        int i = 0;
		bt_ext_log("[JSR82][JBT] JBT session_entry->initiator:%d", session_entry->initiator);

        if (session_entry->initiator != JBT_SESSION_INITIATOR_FALSE)
        {
            ASSERT(0);
        }
        jbt_reset_session_entry(session_entry);      
    }

report_it:
	return;

}



/*****************************************************************************
 * FUNCTION
 *  jbt_handle_session_connect_ind
 * DESCRIPTION
 *  For Server side: It will receive connect_ind
 * PARAMETERS
 *  msg     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jbt_handle_session_connect_ind(ilm_struct *message)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    jbt_session_connect_ind parms;
    jbt_list *existing_entry = NULL;
    jbt_session_info *session_entry = NULL;
    jbt_subsession *subsession_entry = NULL;
    kal_uint8 list_type = 0;
    bt_jsr82_connect_ind_struct *msg;
	bt_jsr82_connect_rsp_struct rsp;
	kal_uint8 result = JSR82_SESSION_CONNECT_REJECT;
	SESSION_RESULT sret = SR_NOERROR;
	U32 sessionid = 0;
	int fd = 0;
	kal_int8 rc = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    msg = (bt_jsr82_connect_ind_struct *)message->ilm_data;

	memset(&rsp, 0x0, sizeof(bt_jsr82_connect_rsp_struct));

	session_entry = jbt_search_existing_session_entry(msg->ps_type, 
													JBT_LIST_SEARCH_TYPE_BY_INX, 
													msg->index);
    if (session_entry == NULL)
    {
		bt_ext_log("[JSR82][JBT] JBT can not find an existing entry with type=%d and value=%d", JBT_LIST_SEARCH_TYPE_BY_INX, msg->index);
        goto exit;
    }	

	subsession_entry = jbt_allocate_one_available_subsession_entry(session_entry);
	if(NULL == subsession_entry)
	{
		goto exit;
	}
	rc = jbt_session_allocate_RWRingBuf(list_type, subsession_entry, msg->index);
	if ((signed char)rc < 0)
	{
		bt_ext_log("[JSR82][JBT] fail to allocate ringbuffer %d",rc);
        goto exit;
	}

	sret = jbt_session_attach(get_transaction_id(), &sessionid,&fd);
	if (SR_NOERROR != sret)
	{
		bt_ext_err("[JSR82][JBT] fail to attach %d", sret);
        jbt_reset_subsession_entry(subsession_entry);
		goto exit;
	}
	subsession_entry->fd = fd;
		
	subsession_entry->subsession_id = sessionid;
	subsession_entry->mtu = msg->mtu;
	subsession_entry->l2cap_id = msg->l2cap_id;

	
    /* Clear transaction_id in context */
	session_entry->transaction_id = 0;
	if (subsession_entry) 
	{
    	subsession_entry->transaction_id = 0;
	}
	

    jbt_ConvertBdAddrOrder(subsession_entry->bd_addr, msg->bd_addr);
	memcpy(msg->bd_addr, subsession_entry->bd_addr, 6);
	session_entry->active_count++;
	#if (JBT_MAX_SUBSESSION_NO == 1)
        ASSERT(session_entry->active_count == 1);
    #endif 
	subsession_entry->subsession_state = JBT_SESSION_STATE_CONNECTED;
	subsession_entry->l2cap_id = msg->l2cap_id;
	result = JSR82_SESSION_CONNECT_ACCEPT;

	bt_ext_log("[JSR82]jbt_jsr82_channel_connected");
	jbt_jsr82_channel_connected(session_entry, subsession_entry);

exit:

	memcpy(rsp.bd_addr, msg->bd_addr, 6);
	rsp.channel = msg->channel;
	rsp.mtu = msg->mtu;
	rsp.ps_type = msg->ps_type;
	rsp.index = msg->index;
	rsp.l2cap_id = msg->l2cap_id;
	rsp.identify = msg->identify;
	rsp.result = result;
	rsp.sessionid = sessionid;
	rsp.fd = fd;	
    JSR82_SendMessage(MSG_ID_BT_JSR82_CONNECT_RSP, MOD_BT, &rsp, sizeof(bt_jsr82_connect_rsp_struct));
	return;

}


/*****************************************************************************
 * FUNCTION
 *  jbt_handle_session_disconnect_ind
 * DESCRIPTION
 *  
 * PARAMETERS
 *  msg     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jbt_handle_session_disconnect_ind(ilm_struct *message)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    jbt_session_disconnect_ind parms;
    jbt_list *existing_entry = NULL;
    jbt_session_info *session_entry = NULL;
    jbt_subsession *subsession_entry = NULL;
    kal_uint8 list_type = 0;
    bt_jsr82_disconnect_ind_struct *msg;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    msg = (bt_jsr82_disconnect_ind_struct *)message->ilm_data;
    bt_ext_log("[JSR82][JBT] JBT bt_handle_session_disconnect_ind parms.ps_type:%02x", msg->ps_type);
	
	session_entry = jbt_search_existing_session_entry(msg->ps_type, 
													JBT_LIST_SEARCH_TYPE_BY_INX, 
													msg->index);
    if (session_entry == NULL)
    {
		bt_ext_log("[JSR82][JBT] JBT can not find an existing entry with type=%d and value=%d", JBT_LIST_SEARCH_TYPE_BY_INX, msg->index);
        return;
    }	

    subsession_entry = jbt_search_existing_subsession_entry(session_entry, JBT_LIST_SEARCH_TYPE_BY_INX, msg->l2cap_id);
	if (NULL == subsession_entry)
	{
		bt_ext_log("[JSR82][JBT] JBT can not find an existing sub entry with type=%d and value=%d", JBT_LIST_SEARCH_TYPE_BY_INX, msg->l2cap_id);
        return;
	}

	session_entry->active_count--;
	
#if (JBT_MAX_SUBSESSION_NO == 1)
    ASSERT(session_entry->active_count == 0);
#endif 

    if (session_entry->initiator == JBT_SESSION_INITIATOR_TRUE)
    {
		jbt_reset_session_entry(session_entry);
    }
    else
    {
    	jbt_reset_subsession_entry(subsession_entry);		
    }
}


/*****************************************************************************
 * FUNCTION
 *  jbt_handle_session_connect_req_cnf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  msg     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jbt_handle_session_connect_req_cnf(ilm_struct *message)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    jbt_session_connect_cnf parms;
    jbt_list *existing_entry = NULL;
    jbt_session_info *session_entry = NULL;
    jbt_subsession *subsession_entry = NULL;
    kal_uint8 list_type = 0;
    bt_jsr82_connect_cnf_struct *msg;
    int rc = 0;    /* 0: means successful */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* For connect_req_result: An existing entry allocated for connect_req has no index information only transaction_id */
    msg = (bt_jsr82_connect_cnf_struct *)message->ilm_data;
    bt_ext_log("[JSR82][JBT] JBT bt_handle_session_connect_req_cnf parms.ps_type:%02x", msg->ps_type);
    session_entry = (jbt_session_info*) existing_entry;

	session_entry = jbt_search_existing_session_entry(msg->ps_type, 
													JBT_LIST_SEARCH_TYPE_BY_TRANSACTION_ID, 
													msg->identify);
    if (session_entry == NULL)
    {
		bt_ext_log("[JSR82][JBT] JBT can not find an existing entry with type=%d and value=%d", JBT_LIST_SEARCH_TYPE_BY_TRANSACTION_ID, msg->identify);
        return;
    }	
	
    /* Check result */
    session_entry->con_req_op.operation_state = JBT_OPERATION_STATE_NONE;
    bt_ext_log("[JSR82][JBT] JBT bt_handle_session_connect_req_cnf msg->result:%02x", msg->result);

    /* Search the actual subsession_entry for this connection_request_cnf:2007-1103 */
    subsession_entry = jbt_search_existing_subsession_entry(session_entry, JBT_LIST_SEARCH_TYPE_BY_TRANSACTION_ID, msg->identify);
	if (NULL == subsession_entry)
	{
		bt_ext_log("[JSR82][JBT] JBT can not find an existing sub entry with type=%d and value=%d", JBT_LIST_SEARCH_TYPE_BY_TRANSACTION_ID, msg->identify);
		goto exit;
	}

    /* Clear transaction_id in context */
    session_entry->transaction_id = 0;
    subsession_entry->transaction_id = 0;

	if (msg->result != JSR82_SESSION_CONNECT_CLIENT_SUCCESS)
    {
        jbt_reset_session_entry(session_entry);
    }
    else
    {
	 	bt_ext_log("[JSR82][JBT] JBT: SPP con req, allocate rw ring buffer");

		kal_uint8 list_type = jbt_convert_list_type(msg->ps_type);
		rc = jbt_session_allocate_RWRingBuf(list_type, subsession_entry, msg->index);

		if (rc < 0)
		{
			jbt_reset_session_entry(session_entry);
			return;
		}

        /* Because the connect_request's result is suceesful, now keeps its session_index for this spp session entry:2007-1024 */
        session_entry->index = msg->index;
        session_entry->active_count++;
    #if (JBT_MAX_SUBSESSION_NO == 1)
        ASSERT(session_entry->active_count == 1);
    #endif 
        /* Keep the l2cap_id value, if the l2cap_id is 0: it is a spp session; otherwise, it is a l2cap session: Add 2007-1103 */
        subsession_entry->l2cap_id = msg->l2cap_id;
        subsession_entry->subsession_state = JBT_SESSION_STATE_CONNECTED;

    }
exit:
	return;
}

void btmtk_jsr82_reset_cnf(ilm_struct *ptr)
{
	bt_jsr82_reset_cnf_struct *cnf_p = (bt_jsr82_reset_cnf_struct*)ptr->ilm_data;
    bt_ext_log("[GAP] btmtk_jsr82_reset_cnf: %d", cnf_p->result);
}


void btmtk_jsr82_handle_message(void *pContext, ilm_struct *message)
{
	session_mutex_lock(jbt_jsr82.mutex);
	switch (message->msg_id)
	{
		/* JSR82 --> VM */
		case MSG_ID_BT_JSR82_ENABLE_SERVICE_CNF:
			jbt_handle_session_enabled_result(message);
			break;
			
		case MSG_ID_BT_JSR82_DISABLE_SERVICE_CNF:
			jbt_handle_session_deregistration_result(message);
			break;

		case MSG_ID_BT_JSR82_CONNECT_IND:
			jbt_handle_session_connect_ind(message);
			break;
			
		case MSG_ID_BT_JSR82_CONNECT_CNF:
			jbt_handle_session_connect_req_cnf(message);
			break;			
		
		case MSG_ID_BT_JSR82_DISCONNECT_IND:
			jbt_handle_session_disconnect_ind(message);
			break;	
			
		case MSG_ID_BT_APP_SDPDB_GET_HANDLE_CNF:
			btmtk_jsr82_get_handler_cnf(message);
			break;
			
		case MSG_ID_BT_APP_SDPDB_REGISTER_CNF:
			btmtk_jsr82_create_record_cnf(message);
			break;
			
		case MSG_ID_BT_APP_SDPDB_DEREGISTER_CNF:
			btmtk_jsr82_remove_record_cnf(message);
			break;
			
		case MSG_ID_BT_JSR82_RESET_CNF:
			btmtk_jsr82_reset_cnf(message);
			break;
			
		default:
			break;
	}
	session_mutex_unlock(jbt_jsr82.mutex);
}





