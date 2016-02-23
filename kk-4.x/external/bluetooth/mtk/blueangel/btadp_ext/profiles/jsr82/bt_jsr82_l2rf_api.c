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

/* 
 * bt_jsr82_api.c
 * 
 * This file is the External Adaptation API of JSR82 used by Virtual Machine.
 * Flow direction: VM --> external ADP API
 */

#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_jsr82_message.h"
#include "bluetooth_jsr82_struct.h"
#include "bt_jsr82_api.h"
#include "bt_jsr82_hdl.h"
#include "bt_struct.h"
#include "bt_mmi_msg.h"
#include "bt_jbt_internal.h"
#include "bt_jbt_interface.h"
//#include "bt_osal.h"
#include "memory.h"
#include "malloc.h"
#include "stdio.h"
#include "bt_ext_debug.h"
#ifdef BTMTK_ON_LINUX
#include <errno.h>
#endif

#include "session_api.h"
#include "bt_session.h"

extern kal_bool btmtk_sendmsg(int profile_id, void* ptr, int size);

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
extern JBT_JSR82_CNTX jbt_jsr82;
static int transaction_id = 0;


void btmtk_jsr82_sdp_register_callback(void *context, char result, int identify);


BtStatus JSR82_SendMessage(msg_type msg_id, module_type mod_id, void *buffer, U16 size)
{
	kal_bool ret = TRUE;
	int sockfd;
	U16 *field;

	ilm_struct ilm;

	bt_ext_log("[JSR82] msg=%u, ptr=0x%X, size=%d", msg_id, buffer, size);

	memset(&ilm, 0x0, sizeof(ilm_struct));

	/* Set src_mod to MOD_MMI */
	ilm.src_mod_id = MOD_MMI;
	ilm.dest_mod_id = mod_id;
	ilm.sap_id = 0;
	ilm.msg_id = msg_id;

	/* LOCAL_PARA_HDR */
	field = (U16 *)ilm.ilm_data;
	*field = 1;
	*(++field) = size;

	 if(buffer != NULL)
        memcpy(ilm.ilm_data, buffer, size);

	// Calculate total ilm size
	size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;

	if(mod_id == MOD_BT)
	{
		ret = btmtk_sendmsg(PROFILE_JSR82, (void* )&ilm, size);
	}
	
	return (ret) ? BT_STATUS_SUCCESS : BT_STATUS_FAILED;
}

int get_transaction_id()
{
	//TODO: context protection in multi thread
	int range = PROFILE_JSR82_DATA_RANGE - PROFILE_JSR82_DATA - 1;
	int count = 0;
	while (count < range)
	{
		transaction_id = transaction_id % range + 1;
		if ((NULL == jbt_search_existing_session_entry(JSR82_SESSION_PS_RFCOMM, 
													JBT_LIST_SEARCH_TYPE_BY_TRANSACTION_ID, 
 													transaction_id)) &&
 			(NULL == jbt_search_existing_session_entry(JSR82_SESSION_PS_L2CAP, 
													JBT_LIST_SEARCH_TYPE_BY_TRANSACTION_ID, 
													transaction_id)))
		{
			break;
		}
		count ++;
	}

	if (count < range)
	{
		return transaction_id;	
	}
	else
	{
		return 0xFFFF;
	}
}


void btmtk_jbt_init(void)
{
	if (jbt_jsr82.init)
	{
		return;
	}
	jbt_l2cap_init(&jbt_jsr82.l2cap);
	jbt_spp_init(&jbt_jsr82.spp);
	jbt_session_start_monitor(&jbt_jsr82.monitorid);

	if (NULL == jbt_jsr82.mutex)
	{		
		session_mutex_create(&jbt_jsr82.mutex);
	}

	btmtk_jsr82_sdp_init();

	jbt_jsr82.init = TRUE;

	jbt_session_general_reset();

}

void btmtk_jbt_deinit(void)
{	
	if (FALSE == jbt_jsr82.init)
	{
		return;
	}

	session_mutex_lock(jbt_jsr82.mutex);

	//double check
	if (FALSE == jbt_jsr82.init)
	{
		goto exit;
	}	
	
	jbt_l2cap_deinit(&jbt_jsr82.l2cap);
	jbt_spp_deinit(&jbt_jsr82.spp);
	
	jbt_session_stop_monitor(jbt_jsr82.monitorid);
	btmtk_jsr82_sdp_deinit();
	
	jbt_jsr82.init = FALSE;
exit:
	session_mutex_unlock(jbt_jsr82.mutex);
}

BT_BOOL btmtk_jbt_check_context_active()
{
	return jbt_jsr82.init;
}


/*****************************************************************************
 * FUNCTION
 *  jbt_session_service_registration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ps_type             [IN]        
 *  mtu                 [IN]        
 *  security            [IN]
 * 					==> Authentication or Encryption is necessary: 0x01
 * 						else: 0x00
 *  transaction_id      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BT_BOOL btmtk_jsr82_session_service_registration(
            kal_uint8 ps_type,
            kal_uint8 *uuid128,
            kal_uint8 *name,            
            kal_uint32 namesize,
            kal_uint16 mtu,
            kal_uint8 security,
            kal_uint32 transaction_id,
            kal_int32		*fd)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    jbt_list *free_entry = NULL;
    jbt_session_info *session_entry = NULL;
    kal_uint16 max_mtu = JSR82_SESSION_PS_RFCOMM_MTU;
    kal_uint8 list_type = 0;
	
	SESSION_RESULT sret = SR_NOERROR;

	int fds[2];
	U32 sessionids[2];

	BT_BOOL jret = FALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_ext_log("[JSR82][JBT] JBT jbt_session_service_registration");

	//lock context
	if (FALSE == btmtk_jbt_check_context_active())
	{
	 	bt_ext_err("[JSR82][JBT] JSR82 does not init");
		return FALSE;
	}
	session_mutex_lock(jbt_jsr82.mutex);
	//lock context again
	if (FALSE == btmtk_jbt_check_context_active())
	{
	 	bt_ext_err("[JSR82][JBT] JSR82 does not init");
		jret = FALSE;
		goto exit;
	}
	
    /* Modify from or to and condition: 2007-1102 */
    if ((security != 0) && ((security != 1)))
    {
	 	bt_ext_log("[JSR82][JBT] JBT Command Reject B");
        jret = FALSE;
		goto exit;
    }

	session_entry = jbt_allocate_one_available_session_entry(ps_type);	
    if (session_entry == NULL)
    {
		bt_ext_log("[JSR82][JBT] JBT can not allocate a free entry");
       	jret = FALSE;
		goto exit;
    }
	list_type = jbt_convert_list_type(ps_type);
	
    /* The actual service channel registered by this service is dependend on Stack's cureent channel number usage: 2007-1107 */
    session_entry->psm_channel = 0;

	if (mtu > max_mtu)
	{
		mtu = max_mtu;
	}

    transaction_id = get_transaction_id();

	sret = jbt_iternal_session_attach(sessionids, fds);
	if (SR_NOERROR != sret)
	{
		bt_ext_log("[JSR82][JBT] fail to attach session");
		jbt_free_one_existing_entry(list_type, (jbt_list *)session_entry);
		jret = FALSE;
		goto exit;
	}

	//fds[0] --> app
	//fds[1] keep it for self
	*fd = fds[0];
	session_entry->fd = fds[1];


	session_entry->session_id = sessionids[1];
	session_entry->twin_session_id = sessionids[0];

	//sdp info
	memcpy(session_entry->sdpinfo.uuid, uuid128, BTMTK_SDP_UUID_128_BIT_SIZE);
	strncpy((char *)session_entry->sdpinfo.name, (char *)name, SDP_REGISTRATION_NAME_MAX_LEN);
	session_entry->sdpinfo.identify = sessionids[1];
	session_entry->sdpinfo.callback = btmtk_jsr82_sdp_register_callback;
	session_entry->sdpinfo.context = session_entry;

	session_entry->transaction_id = transaction_id;
    session_entry->ps_type = ps_type;
    session_entry->initiator = JBT_SESSION_INITIATOR_FALSE;
    session_entry->index = 0xFF;
	session_entry->mtu = mtu;
	session_entry->security_value = security;

    if(jbt_session_general_service_registration(session_entry))
    {
		jret = TRUE;
		goto exit;
	}
	else
	{
		bt_ext_log("[JSR82][JBT] fail to register service");
		sret = bt_session_lock(session_entry->session_id, FALSE);
		sret = bt_session_lock(session_entry->twin_session_id, FALSE);
		jbt_reset_session_entry(session_entry);		
		jret = FALSE;
		goto exit;
	}

exit:
	session_mutex_unlock(jbt_jsr82.mutex);
	return jret;

}


/*****************************************************************************
 * FUNCTION
 *  jbt_session_service_deregistration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ps_type             [IN]        
 *  transaction_id      [IN]        
 *  con_id              [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BT_BOOL btmtk_jsr82_session_service_deregistration(kal_uint8 ps_type, kal_uint32 transaction_id, kal_uint8 con_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct ilm;
    ilm_struct *ilmptr;
    jbt_list *existing_entry = NULL;
    jbt_session_info *session_entry = NULL;

	BT_BOOL jret = FALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_ext_log("[JSR82][JBT] JBT jbt_session_service_deregistration");

	//lock context
	if (FALSE == btmtk_jbt_check_context_active())
	{
	 	bt_ext_err("[JSR82][JBT] JSR82 does not init");
		return FALSE;
	}
	session_mutex_lock(jbt_jsr82.mutex);
	//lock context again
	if (FALSE == btmtk_jbt_check_context_active())
	{
	 	bt_ext_err("[JSR82][JBT] JSR82 does not init");
		jret = FALSE;
		goto exit;
	}

	session_entry = jbt_search_existing_session_entry(ps_type, JBT_LIST_SEARCH_TYPE_BY_INX, con_id);

    if (session_entry == NULL)
    {
		bt_ext_log("[JSR82][JBT] JBT can not find an existing entry with type=%d and value=%d", JBT_LIST_SEARCH_TYPE_BY_INX, con_id);
        jret = FALSE;
		goto exit;
    }

    session_entry->transaction_id = transaction_id;

    jret = jbt_session_general_service_deregistration(session_entry);	

exit:
	session_mutex_unlock(jbt_jsr82.mutex);
    return jret;
}


/*****************************************************************************
 * FUNCTION
 *  jbt_session_connect_req
 * DESCRIPTION
 *  
 * PARAMETERS
 *  transaction_id      [IN]        
 *  bd_addr             [IN]        
 *  ps_type             [IN]        
 *  psm_channel         [IN]        
 *  mtu                 [IN]        
 *  security_value      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BT_BOOL btmtk_jsr82_session_connect_req(
            kal_uint32 transaction_id,
            kal_uint8 *bd_addr,
            kal_uint8 *uuid128,
            kal_uint8 ps_type,
            kal_uint16 psm_channel,
            kal_uint16 mtu,
            kal_uint8 security_value,
            kal_int32* fd)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct ilm;
    ilm_struct *ilmptr;
    jbt_list *free_entry = NULL;
    jbt_session_info *session_entry = NULL;
    jbt_subsession *subsession_entry = NULL;
    kal_uint8 block_inx = 0;

	kal_uint8 list_type = 0;
	
	U32 sessionid;

	SESSION_RESULT sret = SR_NOERROR;

	BT_BOOL jret = FALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_ext_log("[JSR82][JBT] JBT jbt_session_connect_req");

	//lock context
	if (FALSE == btmtk_jbt_check_context_active())
	{
	 	bt_ext_err("[JSR82][JBT] JSR82 does not init");
		return FALSE;
	}
	session_mutex_lock(jbt_jsr82.mutex);
	//lock context again
	if (FALSE == btmtk_jbt_check_context_active())
	{
	 	bt_ext_err("[JSR82][JBT] JSR82 does not init");
		jret = FALSE;
		goto exit;
	}

    ASSERT((ps_type == JSR82_SESSION_PS_RFCOMM) || (ps_type == JSR82_SESSION_PS_L2CAP));

    // TODO: We should allow multi-connection scenario usage.
    // RFCOMM do NOT allow 2 connections with the same channel between two devices.
    // L2CAP: 2 connections in the same PSM channel from a device is acceptable.
    /* Check if this bd_address and its channel is in activeList or not: 2007-1213 */
    if (JSR82_SESSION_PS_RFCOMM == ps_type)
    {
	    if (jbt_check_already_connect_chnl_and_addr(bd_addr, ps_type, psm_channel))
	    {	       
			jret = FALSE;
			goto exit;
	    }
    }    

    session_entry = jbt_allocate_one_available_session_entry(ps_type);	
    if (session_entry == NULL)
    {
		bt_ext_log("[JSR82][JBT] JBT can not allocate a free entry");        
		jret = FALSE;
		goto exit;
    }

	
    transaction_id = get_transaction_id();

    subsession_entry = jbt_allocate_one_available_subsession_entry(session_entry);
    if (subsession_entry != NULL)
    {
        if (subsession_entry->subsession_state == JBT_SESSION_STATE_IDLE)
        {
            subsession_entry->subsession_state = JBT_SESSION_STATE_CONNECTING;
            subsession_entry->transaction_id = transaction_id;
        }
        else
        {
            /* session_state is in an error state */
			bt_ext_err("[JSR82] the subsession state is abnormal %d", subsession_entry->subsession_state);
            
			jret = FALSE;
			goto exit;
        }
    }
	/*
     * Modify: if the bd_addr is sent to btstack via array format, it must be from low byte to high byte [0] to [5] 
     * This function invoked by JVM passes the addr format from high to low ([0] to [6])
     * The bd_addr array returned to JVM is also fro, high to low ([0] to [6]) 
     */
    jbt_ConvertBdAddrOrder(subsession_entry->bd_addr,bd_addr);
	
 //   memcpy(subsession_entry->bd_addr, bd_addr, 6);
    session_entry->transaction_id = transaction_id;
    session_entry->ps_type = ps_type;
    /* Add 2007-1213 */
    session_entry->psm_channel = psm_channel;
    session_entry->initiator = JBT_SESSION_INITIATOR_TRUE;
    session_entry->con_req_op.operation_state = JBT_OPERATION_STATE_ONGOING;
    session_entry->transaction_id = transaction_id;
	subsession_entry->transaction_id = transaction_id;
	session_entry->security_value = security_value;
	session_entry->mtu = mtu;

	sret = jbt_session_attach(transaction_id, &sessionid,(int *)fd);
	if (SR_NOERROR != sret)
	{
		jbt_reset_session_entry(session_entry);
		jret = FALSE;
		goto exit;
	}
	session_entry->session_id = sessionid;

	subsession_entry->subsession_id = 0;
	
	memcpy(session_entry->uuid_128, uuid128, 16);


	if (FALSE == jbt_session_general_connect(session_entry, subsession_entry))
	{
		bt_ext_log("[JSR82][JBT] fail to connect");
		sret = bt_session_lock(session_entry->session_id, FALSE);
		jbt_reset_session_entry(session_entry);		
		jret = FALSE;
	} 
	else
	{
    	jret = TRUE;
	}
exit:
	session_mutex_unlock(jbt_jsr82.mutex);
	return jret;
}


/*****************************************************************************
 * FUNCTION
 *  jbt_session_disconnect_req
 * DESCRIPTION
 *  
 * PARAMETERS
 *  transaction_id      [IN]        
 *  ps_type             [IN]        
 *  con_id              [IN]        
 *  l2cap_id            [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BT_BOOL btmtk_jsr82_session_disconnect_req(kal_uint32 transaction_id, kal_uint8 ps_type, U8 con_id, kal_uint16 l2cap_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	ilm_struct ilm;
    ilm_struct *ilmptr;
    jbt_list *existing_entry = NULL;
    jbt_session_info *session_entry = NULL;
    jbt_subsession *subsession_entry = NULL;

	BT_BOOL jret = FALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	bt_ext_log("[JSR82][JBT] JBT jbt_session_disconnect_req");

	//lock context
	if (FALSE == btmtk_jbt_check_context_active())
	{
	 	bt_ext_err("[JSR82][JBT] JSR82 does not init");
		return FALSE;
	}
	session_mutex_lock(jbt_jsr82.mutex);
	//lock context again
	if (FALSE == btmtk_jbt_check_context_active())
	{
	 	bt_ext_err("[JSR82][JBT] JSR82 does not init");
		jret = FALSE;
		goto exit;
	}

	session_entry = jbt_search_existing_session_entry(ps_type, JBT_LIST_SEARCH_TYPE_BY_INX, con_id);

    if (session_entry == NULL)
    {
		bt_ext_log("[JSR82][JBT] JBT can not find an existing entry with type=%d and value=%d", JBT_LIST_SEARCH_TYPE_BY_INX, con_id);
        jret = FALSE;
		goto exit;
    }

    subsession_entry = jbt_search_existing_subsession_entry(session_entry, JBT_LIST_SEARCH_TYPE_BY_INX, l2cap_id);

    ASSERT(l2cap_id == subsession_entry->subsession_id);
    

    session_entry->transaction_id = transaction_id;
    session_entry->ps_type = ps_type;

    if (subsession_entry->subsession_state == JBT_SESSION_STATE_CONNECTED)
    {
        subsession_entry->subsession_state = JBT_SESSION_STATE_DISCONNECTING;
    }
    else
    {
        /* return FALSE: due to error session state */
        jret = FALSE;
		goto exit;
    }

    jret = jbt_session_general_disconnect(session_entry, subsession_entry);

exit:
	session_mutex_unlock(jbt_jsr82.mutex);
    return jret;
}




void btmtk_jsr82_sdp_register_callback(void *context, BT_BOOL result, int identify)
{
	jbt_session_info *session_entry = (jbt_session_info *)context;
	jbt_subsession *subsession_entry = NULL;

	bt_ext_log("[JSR82]btmtk_jsr82_sdp_register_callback, result[%d]", result);
	if (FALSE == result)
	{
		subsession_entry = jbt_search_existing_subsession_entry(session_entry, JBT_LIST_SEARCH_TYPE_BY_SESSION_ID, identify);
		if (JBT_SESSION_STATE_CONNECTED == subsession_entry->subsession_state)
		{
			jbt_session_general_disconnect(session_entry, subsession_entry);
		}
	
		jbt_session_general_service_deregistration(session_entry);
	}
	else
	{
		jbt_jsr82_session_registered(session_entry);
	}
}









